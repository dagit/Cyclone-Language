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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 884 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 901
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 903
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 906
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 908
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 910
extern void*Cyc_Absyn_double_type;void*Cyc_Absyn_wchar_type();
void*Cyc_Absyn_gen_float_type(unsigned i);
# 913
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 917
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 919
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 936
void*Cyc_Absyn_exn_type();
# 944
extern void*Cyc_Absyn_fat_bound_type;
# 946
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 948
void*Cyc_Absyn_bounds_one();
# 955
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 958
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 960
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 964
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 972
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 976
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 982
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp();
# 1052
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1063
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1117
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1119
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1122
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1124
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 65 "warn.h"
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 68
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 70
void*Cyc_Warn_impos2(struct _fat_ptr);
# 72
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
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp538;_tmp538.tag=0U,({struct _fat_ptr _tmp628=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp538.f1=_tmp628;});_tmp538;});void*_tmp0[1U];_tmp0[0]=& _tmp1;({unsigned _tmp629=loc;Cyc_Warn_err2(_tmp629,_tag_fat(_tmp0,sizeof(void*),1U));});});}
# 64
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 68
if(topt == 0){
# 70
({void*_tmp62A=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=26U,_tmp3->f1=des;_tmp3;});e->r=_tmp62A;});
return;}{
# 73
void*t=*topt;
void*_tmp4=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmp4;void*_tmp5=_stmttmp0;struct Cyc_Absyn_Tqual _tmp7;void*_tmp6;union Cyc_Absyn_AggrInfo _tmp8;switch(*((int*)_tmp5)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 20U){_LL1: _tmp8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp8;
# 76
{union Cyc_Absyn_AggrInfo _tmp9=info;struct Cyc_Absyn_Aggrdecl*_tmpA;if((_tmp9.UnknownAggr).tag == 1){_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp539;_tmp539.tag=0U,({struct _fat_ptr _tmp62B=({const char*_tmpD="struct type improperly set";_tag_fat(_tmpD,sizeof(char),27U);});_tmp539.f1=_tmp62B;});_tmp539;});void*_tmpB[1U];_tmpB[0]=& _tmpC;({unsigned _tmp62C=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp62C,_tag_fat(_tmpB,sizeof(void*),1U));});});}else{_LLC: _tmpA=*(_tmp9.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmpA;
({void*_tmp62D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->tag=29U,_tmpE->f1=ad->name,_tmpE->f2=0,_tmpE->f3=des,_tmpE->f4=ad;_tmpE;});e->r=_tmp62D;});}}_LL9:;}
# 80
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmp6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5)->f1).elt_type;_tmp7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5)->f1).tq;_LL4: {void*at=_tmp6;struct Cyc_Absyn_Tqual aq=_tmp7;
({void*_tmp62E=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->tag=26U,_tmpF->f1=des;_tmpF;});e->r=_tmp62E;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp62F=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=30U,_tmp10->f1=t,_tmp10->f2=des;_tmp10;});e->r=_tmp62F;});goto _LL0;default: _LL7: _LL8:
({void*_tmp630=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=26U,_tmp11->f1=des;_tmp11;});e->r=_tmp630;});goto _LL0;}_LL0:;}}
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53A;_tmp53A.tag=0U,({struct _fat_ptr _tmp631=({const char*_tmp16="assignment in test";_tag_fat(_tmp16,sizeof(char),19U);});_tmp53A.f1=_tmp631;});_tmp53A;});void*_tmp14[1U];_tmp14[0]=& _tmp15;({unsigned _tmp632=e->loc;Cyc_Warn_warn2(_tmp632,_tag_fat(_tmp14,sizeof(void*),1U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 111
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53F;_tmp53F.tag=0U,({struct _fat_ptr _tmp633=({const char*_tmp1F="test of ";_tag_fat(_tmp1F,sizeof(char),9U);});_tmp53F.f1=_tmp633;});_tmp53F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53E;_tmp53E.tag=0U,_tmp53E.f1=msg_part;_tmp53E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53D;_tmp53D.tag=0U,({struct _fat_ptr _tmp634=({const char*_tmp1E=" has type ";_tag_fat(_tmp1E,sizeof(char),11U);});_tmp53D.f1=_tmp634;});_tmp53D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53C;_tmp53C.tag=2U,_tmp53C.f1=(void*)_check_null(e->topt);_tmp53C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B;_tmp53B.tag=0U,({
struct _fat_ptr _tmp635=({const char*_tmp1D=" instead of integral or pointer type";_tag_fat(_tmp1D,sizeof(char),37U);});_tmp53B.f1=_tmp635;});_tmp53B;});void*_tmp17[5U];_tmp17[0]=& _tmp18,_tmp17[1]=& _tmp19,_tmp17[2]=& _tmp1A,_tmp17[3]=& _tmp1B,_tmp17[4]=& _tmp1C;({unsigned _tmp636=e->loc;Cyc_Warn_err2(_tmp636,_tag_fat(_tmp17,sizeof(void*),5U));});});}
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
({union Cyc_Absyn_Cnst _tmp637=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp637;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _LL23: _tmp2F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp2F;
# 170
({union Cyc_Absyn_Cnst _tmp638=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp638;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _LL25: _tmp2E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_LL26: {enum Cyc_Absyn_Sign sn=_tmp2E;
_tmp2D=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _LL27: _tmp2D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp2D;
# 176
({union Cyc_Absyn_Cnst _tmp639=Cyc_Absyn_Int_c(sn,i);*c=_tmp639;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f2 != 0){_LL2B: _tmp2C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f2)->hd;_LL2C: {void*t1=_tmp2C;
# 185
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=9U,({struct Cyc_Absyn_Exp*_tmp63A=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp31->f1=_tmp63A;});_tmp31;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3U: _LL29: if(i == 0){_LL2A: {
# 180
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp63B=({union Cyc_Absyn_Cnst _tmp540;(_tmp540.Null_c).tag=1U,(_tmp540.Null_c).val=0;_tmp540;});*c=_tmp63B;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}}else{goto _LL2D;}default: _LL2D: _LL2E:
# 187
 return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL20:;}}case 1U: _LL15: _LL16:
# 190
 if(topt != 0){
void*_tmp32=Cyc_Tcutil_compress(*topt);void*_stmttmp5=_tmp32;void*_tmp33=_stmttmp5;void*_tmp34;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33)->tag == 3U){_LL30: _tmp34=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33)->f1).ptr_atts).nullable;_LL31: {void*nbl=_tmp34;
# 193
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36=({struct Cyc_Warn_String_Warn_Warg_struct _tmp542;_tmp542.tag=0U,({struct _fat_ptr _tmp63C=({const char*_tmp38="Used NULL when expecting a value of type ";_tag_fat(_tmp38,sizeof(char),42U);});_tmp542.f1=_tmp63C;});_tmp542;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp37=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp541;_tmp541.tag=2U,_tmp541.f1=(void*)*topt;_tmp541;});void*_tmp35[2U];_tmp35[0]=& _tmp36,_tmp35[1]=& _tmp37;({unsigned _tmp63D=e->loc;Cyc_Warn_err2(_tmp63D,_tag_fat(_tmp35,sizeof(void*),2U));});});
return*topt;}}else{_LL32: _LL33:
 goto _LL2F;}_LL2F:;}{
# 198
struct Cyc_List_List*_tmp39=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp39;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->tag=3U,({void*_tmp642=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->v=tenv_tvs;_tmp3A;}));(_tmp3C->f1).elt_type=_tmp642;}),({
struct Cyc_Absyn_Tqual _tmp641=Cyc_Absyn_empty_tqual(0U);(_tmp3C->f1).elt_tq=_tmp641;}),
({void*_tmp640=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->v=tenv_tvs;_tmp3B;}));((_tmp3C->f1).ptr_atts).rgn=_tmp640;}),((_tmp3C->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
void*_tmp63F=Cyc_Tcutil_any_bounds(tenv_tvs);((_tmp3C->f1).ptr_atts).bounds=_tmp63F;}),({
void*_tmp63E=Cyc_Tcutil_any_bool(tenv_tvs);((_tmp3C->f1).ptr_atts).zero_term=_tmp63E;}),((_tmp3C->f1).ptr_atts).ptrloc=0;_tmp3C;});}case 8U: _LL17: _tmp23=(_tmp21.String_c).val;_LL18: {struct _fat_ptr s=_tmp23;
# 205
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp22=s;goto _LL1A;}default: _LL19: _tmp22=(_tmp21.Wstring_c).val;_LL1A: {struct _fat_ptr s=_tmp22;
# 208
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 212
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp643=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp643,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 217
void*_tmp3D=({void*_tmp647=string_elt_typ;void*_tmp646=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp645=Cyc_Absyn_const_tqual(0U);void*_tmp644=
Cyc_Absyn_thin_bounds_exp(elen);
# 217
Cyc_Absyn_atb_type(_tmp647,_tmp646,_tmp645,_tmp644,Cyc_Absyn_true_type);});void*t=_tmp3D;
# 219
if(topt == 0)
return t;{
void*_tmp3E=Cyc_Tcutil_compress(*topt);void*_stmttmp6=_tmp3E;void*_tmp3F=_stmttmp6;struct Cyc_Absyn_Tqual _tmp40;switch(*((int*)_tmp3F)){case 4U: _LL35: _tmp40=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).tq;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp40;
# 225
return({void*_tmp64A=string_elt_typ;struct Cyc_Absyn_Tqual _tmp649=tq;struct Cyc_Absyn_Exp*_tmp648=elen;Cyc_Absyn_array_type(_tmp64A,_tmp649,_tmp648,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL37: _LL38:
# 230
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp64D=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp64C=loc;void*_tmp64B=t;Cyc_Tcutil_silent_castable(_tmp64D,_tmp64C,_tmp64B,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
return*topt;}
# 235
return t;default: _LL39: _LL3A:
 return t;}_LL34:;}}}}}_LL0:;}
# 242
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp41=*((void**)_check_null(b));void*_stmttmp7=_tmp41;void*_tmp42=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp43;struct Cyc_Absyn_Vardecl*_tmp44;struct Cyc_Absyn_Vardecl*_tmp45;struct Cyc_Absyn_Fndecl*_tmp46;struct Cyc_Absyn_Vardecl*_tmp47;struct _tuple1*_tmp48;switch(*((int*)_tmp42)){case 0U: _LL1: _tmp48=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp42)->f1;_LL2: {struct _tuple1*q=_tmp48;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp543;_tmp543.tag=0U,({struct _fat_ptr _tmp64E=({const char*_tmp4B="unresolved binding in tcVar";_tag_fat(_tmp4B,sizeof(char),28U);});_tmp543.f1=_tmp64E;});_tmp543;});void*_tmp49[1U];_tmp49[0]=& _tmp4A;({unsigned _tmp64F=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp64F,_tag_fat(_tmp49,sizeof(void*),1U));});});}case 1U: _LL3: _tmp47=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp42)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp47;
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
({void*_tmp650=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=39U,_tmp4F->f1=i;_tmp4F;});e->r=_tmp650;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
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
if(!({struct Cyc_RgnOrder_RgnPO*_tmp652=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp651=e;Cyc_Tcutil_coerce_arg(_tmp652,_tmp651,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp57=({struct Cyc_Warn_String_Warn_Warg_struct _tmp547;_tmp547.tag=0U,({struct _fat_ptr _tmp653=({const char*_tmp5C="descriptor has type ";_tag_fat(_tmp5C,sizeof(char),21U);});_tmp547.f1=_tmp653;});_tmp547;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp546;_tmp546.tag=2U,_tmp546.f1=(void*)t;_tmp546;});struct Cyc_Warn_String_Warn_Warg_struct _tmp59=({struct Cyc_Warn_String_Warn_Warg_struct _tmp545;_tmp545.tag=0U,({
struct _fat_ptr _tmp654=({const char*_tmp5B=" but argument has type ";_tag_fat(_tmp5B,sizeof(char),24U);});_tmp545.f1=_tmp654;});_tmp545;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp544;_tmp544.tag=2U,_tmp544.f1=(void*)_check_null(e->topt);_tmp544;});void*_tmp56[4U];_tmp56[0]=& _tmp57,_tmp56[1]=& _tmp58,_tmp56[2]=& _tmp59,_tmp56[3]=& _tmp5A;({unsigned _tmp655=e->loc;Cyc_Tcexp_err_and_explain(_tmp655,_tag_fat(_tmp56,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp656=({struct Cyc_List_List*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->hd=(void*)arg_cnt,_tmp5D->tl=*alias_arg_exps;_tmp5D;});*alias_arg_exps=_tmp656;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !
Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(((struct Cyc_Absyn_Exp*)args->hd)->loc);}
# 316
if(desc_types != 0)
({void*_tmp5E=0U;({unsigned _tmp658=fmt->loc;struct _fat_ptr _tmp657=({const char*_tmp5F="too few arguments";_tag_fat(_tmp5F,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp658,_tmp657,_tag_fat(_tmp5E,sizeof(void*),0U));});});
if(args != 0){
({void*_tmp60=0U;({unsigned _tmp65A=((struct Cyc_Absyn_Exp*)args->hd)->loc;struct _fat_ptr _tmp659=({const char*_tmp61="too many arguments";_tag_fat(_tmp61,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp65A,_tmp659,_tag_fat(_tmp60,sizeof(void*),0U));});});
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp549;_tmp549.tag=0U,({struct _fat_ptr _tmp65B=({const char*_tmp66="expecting numeric type but found ";_tag_fat(_tmp66,sizeof(char),34U);});_tmp549.f1=_tmp65B;});_tmp549;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp548;_tmp548.tag=2U,_tmp548.f1=(void*)t;_tmp548;});void*_tmp63[2U];_tmp63[0]=& _tmp64,_tmp63[1]=& _tmp65;({unsigned _tmp65C=loc;Cyc_Warn_err2(_tmp65C,_tag_fat(_tmp63,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 336
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp68=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54B;_tmp54B.tag=0U,({struct _fat_ptr _tmp65D=({const char*_tmp6A="expecting integral or * type but found ";_tag_fat(_tmp6A,sizeof(char),40U);});_tmp54B.f1=_tmp65D;});_tmp54B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp69=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54A;_tmp54A.tag=2U,_tmp54A.f1=(void*)t;_tmp54A;});void*_tmp67[2U];_tmp67[0]=& _tmp68,_tmp67[1]=& _tmp69;({unsigned _tmp65E=loc;Cyc_Warn_err2(_tmp65E,_tag_fat(_tmp67,sizeof(void*),2U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 341
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54D;_tmp54D.tag=0U,({struct _fat_ptr _tmp65F=({const char*_tmp6E="expecting integral type but found ";_tag_fat(_tmp6E,sizeof(char),35U);});_tmp54D.f1=_tmp65F;});_tmp54D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54C;_tmp54C.tag=2U,_tmp54C.f1=(void*)t;_tmp54C;});void*_tmp6B[2U];_tmp6B[0]=& _tmp6C,_tmp6B[1]=& _tmp6D;({unsigned _tmp660=loc;Cyc_Warn_err2(_tmp660,_tag_fat(_tmp6B,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 345
{void*_tmp6F=t;void*_tmp70;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->tag == 3U){_LLE: _tmp70=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp70;
# 347
struct Cyc_Absyn_Exp*_tmp71=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp71;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp73=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54E;_tmp54E.tag=0U,({struct _fat_ptr _tmp661=({const char*_tmp74="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp74,sizeof(char),53U);});_tmp54E.f1=_tmp661;});_tmp54E;});void*_tmp72[1U];_tmp72[0]=& _tmp73;({unsigned _tmp662=loc;Cyc_Warn_err2(_tmp662,_tag_fat(_tmp72,sizeof(void*),1U));});});
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp76=({struct Cyc_Warn_String_Warn_Warg_struct _tmp550;_tmp550.tag=0U,({struct _fat_ptr _tmp663=({const char*_tmp78="numelts requires pointer type, not ";_tag_fat(_tmp78,sizeof(char),36U);});_tmp550.f1=_tmp663;});_tmp550;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp77=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54F;_tmp54F.tag=2U,_tmp54F.f1=(void*)t;_tmp54F;});void*_tmp75[2U];_tmp75[0]=& _tmp76,_tmp75[1]=& _tmp77;({unsigned _tmp664=loc;Cyc_Warn_err2(_tmp664,_tag_fat(_tmp75,sizeof(void*),2U));});});}_LLD:;}
# 353
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp551;_tmp551.tag=0U,({struct _fat_ptr _tmp665=({const char*_tmp7B="Non-unary primop";_tag_fat(_tmp7B,sizeof(char),17U);});_tmp551.f1=_tmp665;});_tmp551;});void*_tmp79[1U];_tmp79[0]=& _tmp7A;({unsigned _tmp666=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp666,_tag_fat(_tmp79,sizeof(void*),1U));});});}_LL0:;}
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
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp80=({struct Cyc_Warn_String_Warn_Warg_struct _tmp554;_tmp554.tag=0U,({
struct _fat_ptr _tmp667=({const char*_tmp84="type ";_tag_fat(_tmp84,sizeof(char),6U);});_tmp554.f1=_tmp667;});_tmp554;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp81=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp553;_tmp553.tag=2U,_tmp553.f1=(void*)_check_null(e1->topt);_tmp553;});struct Cyc_Warn_String_Warn_Warg_struct _tmp82=({struct Cyc_Warn_String_Warn_Warg_struct _tmp552;_tmp552.tag=0U,({struct _fat_ptr _tmp668=({const char*_tmp83=" cannot be used here";_tag_fat(_tmp83,sizeof(char),21U);});_tmp552.f1=_tmp668;});_tmp552;});void*_tmp7F[3U];_tmp7F[0]=& _tmp80,_tmp7F[1]=& _tmp81,_tmp7F[2]=& _tmp82;({struct Cyc_Tcenv_Tenv*_tmp66A=te;unsigned _tmp669=e1->loc;Cyc_Tcexp_expr_err(_tmp66A,_tmp669,0,_tag_fat(_tmp7F,sizeof(void*),3U));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp86=({struct Cyc_Warn_String_Warn_Warg_struct _tmp557;_tmp557.tag=0U,({
struct _fat_ptr _tmp66B=({const char*_tmp8A="type ";_tag_fat(_tmp8A,sizeof(char),6U);});_tmp557.f1=_tmp66B;});_tmp557;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp87=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp556;_tmp556.tag=2U,_tmp556.f1=(void*)_check_null(e2->topt);_tmp556;});struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp555;_tmp555.tag=0U,({struct _fat_ptr _tmp66C=({const char*_tmp89=" cannot be used here";_tag_fat(_tmp89,sizeof(char),21U);});_tmp555.f1=_tmp66C;});_tmp555;});void*_tmp85[3U];_tmp85[0]=& _tmp86,_tmp85[1]=& _tmp87,_tmp85[2]=& _tmp88;({struct Cyc_Tcenv_Tenv*_tmp66E=te;unsigned _tmp66D=e2->loc;Cyc_Tcexp_expr_err(_tmp66E,_tmp66D,0,_tag_fat(_tmp85,sizeof(void*),3U));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 382
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp8B=t1;void*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;struct Cyc_Absyn_Tqual _tmp8D;void*_tmp8C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->tag == 3U){_LL1: _tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_type;_tmp8D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_tq;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).rgn;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).nullable;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).bounds;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).zero_term;_LL2: {void*et=_tmp8C;struct Cyc_Absyn_Tqual tq=_tmp8D;void*r=_tmp8E;void*n=_tmp8F;void*b=_tmp90;void*zt=_tmp91;
# 387
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp93=({struct Cyc_Warn_String_Warn_Warg_struct _tmp558;_tmp558.tag=0U,({struct _fat_ptr _tmp66F=({const char*_tmp94="can't do arithmetic on abstract pointer type";_tag_fat(_tmp94,sizeof(char),45U);});_tmp558.f1=_tmp66F;});_tmp558;});void*_tmp92[1U];_tmp92[0]=& _tmp93;({unsigned _tmp670=e1->loc;Cyc_Warn_err2(_tmp670,_tag_fat(_tmp92,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp96=({struct Cyc_Warn_String_Warn_Warg_struct _tmp559;_tmp559.tag=0U,({struct _fat_ptr _tmp671=({const char*_tmp97="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmp97,sizeof(char),49U);});_tmp559.f1=_tmp671;});_tmp559;});void*_tmp95[1U];_tmp95[0]=& _tmp96;({unsigned _tmp672=e1->loc;Cyc_Warn_err2(_tmp672,_tag_fat(_tmp95,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp99=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55B;_tmp55B.tag=0U,({struct _fat_ptr _tmp673=({const char*_tmp9B="expecting int but found ";_tag_fat(_tmp9B,sizeof(char),25U);});_tmp55B.f1=_tmp673;});_tmp55B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55A;_tmp55A.tag=2U,_tmp55A.f1=(void*)t2;_tmp55A;});void*_tmp98[2U];_tmp98[0]=& _tmp99,_tmp98[1]=& _tmp9A;({unsigned _tmp674=e2->loc;Cyc_Warn_err2(_tmp674,_tag_fat(_tmp98,sizeof(void*),2U));});});{
struct Cyc_Absyn_Exp*_tmp9C=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp9C;
if(eopt == 0)
return t1;
# 398
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmp9D=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmp9D;struct _tuple16 _tmp9E=_stmttmpA;int _tmpA0;unsigned _tmp9F;_LL6: _tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;_LL7: {unsigned i=_tmp9F;int known=_tmpA0;
if(known && i == (unsigned)1)
({void*_tmpA1=0U;({unsigned _tmp676=e1->loc;struct _fat_ptr _tmp675=({const char*_tmpA2="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpA2,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp676,_tmp675,_tag_fat(_tmpA1,sizeof(void*),0U));});});}}{
# 409
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA3=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=3U,(_tmpA4->f1).elt_type=et,(_tmpA4->f1).elt_tq=tq,((_tmpA4->f1).ptr_atts).rgn=r,((_tmpA4->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpA4->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpA4->f1).ptr_atts).zero_term=zt,((_tmpA4->f1).ptr_atts).ptrloc=0;_tmpA4;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpA3;
# 412
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,Cyc_Absyn_Other_coercion);
return(void*)new_t1;}}}}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 419
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp561;_tmp561.tag=0U,({struct _fat_ptr _tmp677=({const char*_tmpAF="pointer arithmetic on values of different ";_tag_fat(_tmpAF,sizeof(char),43U);});_tmp561.f1=_tmp677;});_tmp561;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp560;_tmp560.tag=0U,({
struct _fat_ptr _tmp678=({const char*_tmpAE="types (";_tag_fat(_tmpAE,sizeof(char),8U);});_tmp560.f1=_tmp678;});_tmp560;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55F;_tmp55F.tag=2U,_tmp55F.f1=(void*)t1;_tmp55F;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55E;_tmp55E.tag=0U,({struct _fat_ptr _tmp679=({const char*_tmpAD=" != ";_tag_fat(_tmpAD,sizeof(char),5U);});_tmp55E.f1=_tmp679;});_tmp55E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55D;_tmp55D.tag=2U,_tmp55D.f1=(void*)t2;_tmp55D;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55C;_tmp55C.tag=0U,({struct _fat_ptr _tmp67A=({const char*_tmpAC=")";_tag_fat(_tmpAC,sizeof(char),2U);});_tmp55C.f1=_tmp67A;});_tmp55C;});void*_tmpA5[6U];_tmpA5[0]=& _tmpA6,_tmpA5[1]=& _tmpA7,_tmpA5[2]=& _tmpA8,_tmpA5[3]=& _tmpA9,_tmpA5[4]=& _tmpAA,_tmpA5[5]=& _tmpAB;({unsigned _tmp67B=e1->loc;Cyc_Tcexp_err_and_explain(_tmp67B,_tag_fat(_tmpA5,sizeof(void*),6U));});});
return Cyc_Absyn_sint_type;}
# 431
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp67C=t1_elt;Cyc_Unify_unify(_tmp67C,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp567;_tmp567.tag=0U,({struct _fat_ptr _tmp67D=({const char*_tmpBA="pointer arithmetic on values of different ";_tag_fat(_tmpBA,sizeof(char),43U);});_tmp567.f1=_tmp67D;});_tmp567;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp566;_tmp566.tag=0U,({
struct _fat_ptr _tmp67E=({const char*_tmpB9="types(";_tag_fat(_tmpB9,sizeof(char),7U);});_tmp566.f1=_tmp67E;});_tmp566;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp565;_tmp565.tag=2U,_tmp565.f1=(void*)t1;_tmp565;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp564;_tmp564.tag=0U,({struct _fat_ptr _tmp67F=({const char*_tmpB8=" != ";_tag_fat(_tmpB8,sizeof(char),5U);});_tmp564.f1=_tmp67F;});_tmp564;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp563;_tmp563.tag=2U,_tmp563.f1=(void*)t2;_tmp563;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp562;_tmp562.tag=0U,({struct _fat_ptr _tmp680=({const char*_tmpB7=")";_tag_fat(_tmpB7,sizeof(char),2U);});_tmp562.f1=_tmp680;});_tmp562;});void*_tmpB0[6U];_tmpB0[0]=& _tmpB1,_tmpB0[1]=& _tmpB2,_tmpB0[2]=& _tmpB3,_tmpB0[3]=& _tmpB4,_tmpB0[4]=& _tmpB5,_tmpB0[5]=& _tmpB6;({unsigned _tmp681=e1->loc;Cyc_Tcexp_err_and_explain(_tmp681,_tag_fat(_tmpB0,sizeof(void*),6U));});});
# 436
({void*_tmpBB=0U;({unsigned _tmp683=e1->loc;struct _fat_ptr _tmp682=({const char*_tmpBC="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpBC,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp683,_tmp682,_tag_fat(_tmpBB,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp687=e1;Cyc_Tcutil_unchecked_cast(_tmp687,({void*_tmp686=t1_elt;void*_tmp685=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp684=
Cyc_Absyn_empty_tqual(0U);
# 437
Cyc_Absyn_star_type(_tmp686,_tmp685,_tmp684,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 440
return Cyc_Absyn_sint_type;}
# 442
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpBD=0U;({unsigned _tmp689=e1->loc;struct _fat_ptr _tmp688=({const char*_tmpBE="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpBE,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp689,_tmp688,_tag_fat(_tmpBD,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpBF=0U;({unsigned _tmp68B=e1->loc;struct _fat_ptr _tmp68A=({const char*_tmpC0="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpC0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp68B,_tmp68A,_tag_fat(_tmpBF,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56B;_tmp56B.tag=0U,({struct _fat_ptr _tmp68C=({const char*_tmpC7="expecting either ";_tag_fat(_tmpC7,sizeof(char),18U);});_tmp56B.f1=_tmp68C;});_tmp56B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpC3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56A;_tmp56A.tag=2U,_tmp56A.f1=(void*)t1;_tmp56A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp569;_tmp569.tag=0U,({struct _fat_ptr _tmp68D=({const char*_tmpC6=" or int but found ";_tag_fat(_tmpC6,sizeof(char),19U);});_tmp569.f1=_tmp68D;});_tmp569;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpC5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp568;_tmp568.tag=2U,_tmp568.f1=(void*)t2;_tmp568;});void*_tmpC1[4U];_tmpC1[0]=& _tmpC2,_tmpC1[1]=& _tmpC3,_tmpC1[2]=& _tmpC4,_tmpC1[3]=& _tmpC5;({unsigned _tmp68E=e2->loc;Cyc_Tcexp_err_and_explain(_tmp68E,_tag_fat(_tmpC1,sizeof(void*),4U));});});
return t1;}
# 451
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp68F=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp68F,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({void*_tmpC8=0U;({unsigned _tmp691=e1->loc;struct _fat_ptr _tmp690=({const char*_tmpC9="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpC9,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp691,_tmp690,_tag_fat(_tmpC8,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp695=e2;Cyc_Tcutil_unchecked_cast(_tmp695,({void*_tmp694=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp693=
Cyc_Tcutil_pointer_region(t2);
# 456
struct Cyc_Absyn_Tqual _tmp692=
# 458
Cyc_Absyn_empty_tqual(0U);
# 456
Cyc_Absyn_star_type(_tmp694,_tmp693,_tmp692,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 461
({void*_tmpCA=0U;({unsigned _tmp697=e1->loc;struct _fat_ptr _tmp696=({const char*_tmpCB="thin pointer subtraction!";_tag_fat(_tmpCB,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp697,_tmp696,_tag_fat(_tmpCA,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 464
({void*_tmpCC=0U;({unsigned _tmp699=e1->loc;struct _fat_ptr _tmp698=({const char*_tmpCD="coercing thin pointer to integer to support subtraction";_tag_fat(_tmpCD,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp699,_tmp698,_tag_fat(_tmpCC,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 467
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpCE=0U;({unsigned _tmp69B=e1->loc;struct _fat_ptr _tmp69A=({const char*_tmpCF="coercing pointer to integer to support subtraction";_tag_fat(_tmpCF,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp69B,_tmp69A,_tag_fat(_tmpCE,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 472
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 475
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 483
if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind ||({
void*_tmp69C=t1;Cyc_Unify_unify(_tmp69C,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 488
if(({struct Cyc_RgnOrder_RgnPO*_tmp69F=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp69E=loc;void*_tmp69D=t2;Cyc_Tcutil_silent_castable(_tmp69F,_tmp69E,_tmp69D,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 492
if(({struct Cyc_RgnOrder_RgnPO*_tmp6A2=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6A1=loc;void*_tmp6A0=t1;Cyc_Tcutil_silent_castable(_tmp6A2,_tmp6A1,_tmp6A0,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 496
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 501
{struct _tuple0 _tmpD0=({struct _tuple0 _tmp56C;({void*_tmp6A4=Cyc_Tcutil_compress(t1);_tmp56C.f1=_tmp6A4;}),({void*_tmp6A3=Cyc_Tcutil_compress(t2);_tmp56C.f2=_tmp6A3;});_tmp56C;});struct _tuple0 _stmttmpB=_tmpD0;struct _tuple0 _tmpD1=_stmttmpB;void*_tmpD3;void*_tmpD2;switch(*((int*)_tmpD1.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f2)->tag == 3U){_LL1: _tmpD2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f1)->f1).elt_type;_tmpD3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f2)->f1).elt_type;_LL2: {void*t1a=_tmpD2;void*t2a=_tmpD3;
# 503
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD1.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD1.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD1.f2)->f1)->tag == 3U){_LL3: _LL4:
# 507
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 511
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp570;_tmp570.tag=0U,({struct _fat_ptr _tmp6A5=({const char*_tmpDA="comparison not allowed between ";_tag_fat(_tmpDA,sizeof(char),32U);});_tmp570.f1=_tmp6A5;});_tmp570;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56F;_tmp56F.tag=2U,_tmp56F.f1=(void*)t1;_tmp56F;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56E;_tmp56E.tag=0U,({struct _fat_ptr _tmp6A6=({const char*_tmpD9=" and ";_tag_fat(_tmpD9,sizeof(char),6U);});_tmp56E.f1=_tmp6A6;});_tmp56E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56D;_tmp56D.tag=2U,_tmp56D.f1=(void*)t2;_tmp56D;});void*_tmpD4[4U];_tmpD4[0]=& _tmpD5,_tmpD4[1]=& _tmpD6,_tmpD4[2]=& _tmpD7,_tmpD4[3]=& _tmpD8;({unsigned _tmp6A7=loc;Cyc_Tcexp_err_and_explain(_tmp6A7,_tag_fat(_tmpD4,sizeof(void*),4U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 517
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 519
enum Cyc_Absyn_Primop _tmpDB=p;switch(_tmpDB){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 523
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 526
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL15: _LL16:
# 535
 goto _LL18;case Cyc_Absyn_Neq: _LL17: _LL18:
 goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A:
# 538
 goto _LL1C;case Cyc_Absyn_Lt: _LL1B: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL1F: _LL20:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);default: _LL21: _LL22:
# 543
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp571;_tmp571.tag=0U,({struct _fat_ptr _tmp6A8=({const char*_tmpDE="bad binary primop";_tag_fat(_tmpDE,sizeof(char),18U);});_tmp571.f1=_tmp6A8;});_tmp571;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpDC,sizeof(void*),1U));});}_LL0:;}
# 547
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 555
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpDF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpDF;
void*_tmpE0=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpE0;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp573;_tmp573.tag=0U,({struct _fat_ptr _tmp6A9=({const char*_tmpE4="expecting numeric type but found ";_tag_fat(_tmpE4,sizeof(char),34U);});_tmp573.f1=_tmp6A9;});_tmp573;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp572;_tmp572.tag=2U,_tmp572.f1=(void*)t;_tmp572;});void*_tmpE1[2U];_tmpE1[0]=& _tmpE2,_tmpE1[1]=& _tmpE3;({unsigned _tmp6AA=e->loc;Cyc_Warn_err2(_tmp6AA,_tag_fat(_tmpE1,sizeof(void*),2U));});});
return t;}
# 562
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpE5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpE5;int _tmpE6=_stmttmpC;switch(_tmpE6){case 0U: _LL1: _LL2:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpE8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp574;_tmp574.tag=0U,({struct _fat_ptr _tmp6AB=({const char*_tmpE9="primitive operator has 0 arguments";_tag_fat(_tmpE9,sizeof(char),35U);});_tmp574.f1=_tmp6AB;});_tmp574;});void*_tmpE7[1U];_tmpE7[0]=& _tmpE8;({struct Cyc_Tcenv_Tenv*_tmp6AE=te;unsigned _tmp6AD=loc;void**_tmp6AC=topt;Cyc_Tcexp_expr_err(_tmp6AE,_tmp6AD,_tmp6AC,_tag_fat(_tmpE7,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);goto _LL0;case 2U: _LL5: _LL6:
 t=({struct Cyc_Tcenv_Tenv*_tmp6B3=te;unsigned _tmp6B2=loc;void**_tmp6B1=topt;enum Cyc_Absyn_Primop _tmp6B0=p;struct Cyc_Absyn_Exp*_tmp6AF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp6B3,_tmp6B2,_tmp6B1,_tmp6B0,_tmp6AF,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});goto _LL0;default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp575;_tmp575.tag=0U,({struct _fat_ptr _tmp6B4=({const char*_tmpEC="primitive operator has > 2 arguments";_tag_fat(_tmpEC,sizeof(char),37U);});_tmp575.f1=_tmp6B4;});_tmp575;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEB;({struct Cyc_Tcenv_Tenv*_tmp6B7=te;unsigned _tmp6B6=loc;void**_tmp6B5=topt;Cyc_Tcexp_expr_err(_tmp6B7,_tmp6B6,_tmp6B5,_tag_fat(_tmpEA,sizeof(void*),1U));});});}_LL0:;}
# 570
return t;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 573
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
void*_tmpED=Cyc_Tcutil_compress(t);void*t=_tmpED;
void*_tmpEE=t;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Tqual _tmpF1;void*_tmpF0;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Datatypefield*_tmpF3;struct Cyc_Absyn_Aggrdecl*_tmpF4;switch(*((int*)_tmpEE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpF4=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmpF4;
# 577
if(ad->impl == 0){
({void*_tmpF5=0U;({unsigned _tmp6B9=loc;struct _fat_ptr _tmp6B8=({const char*_tmpF6="attempt to write an abstract aggregate";_tag_fat(_tmpF6,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6B9,_tmp6B8,_tag_fat(_tmpF5,sizeof(void*),0U));});});
return 0;}
# 581
_tmpF2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpF3=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmpF3;
# 593
{struct Cyc_List_List*_tmpFB=df->typs;struct Cyc_List_List*fs=_tmpFB;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmpFC=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmpFC;struct _tuple17*_tmpFD=_stmttmpD;void*_tmpFF;struct Cyc_Absyn_Tqual _tmpFE;_LLE: _tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmpFE;void*t=_tmpFF;
if(tq.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmp102=({struct Cyc_String_pa_PrintArg_struct _tmp576;_tmp576.tag=0U,({struct _fat_ptr _tmp6BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(df->name));_tmp576.f1=_tmp6BA;});_tmp576;});void*_tmp100[1U];_tmp100[0]=& _tmp102;({unsigned _tmp6BC=loc;struct _fat_ptr _tmp6BB=({const char*_tmp101="attempt to over-write a datatype field (%s) with a const member";_tag_fat(_tmp101,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp6BC,_tmp6BB,_tag_fat(_tmp100,sizeof(void*),1U));});});
return 0;}
# 599
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 601
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpF2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEE)->f2;_LL4: {struct Cyc_List_List*fs=_tmpF2;
# 583
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpF7=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmpF7;
if((f->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpFA=({struct Cyc_String_pa_PrintArg_struct _tmp577;_tmp577.tag=0U,_tmp577.f1=(struct _fat_ptr)((struct _fat_ptr)*f->name);_tmp577;});void*_tmpF8[1U];_tmpF8[0]=& _tmpFA;({unsigned _tmp6BE=loc;struct _fat_ptr _tmp6BD=({const char*_tmpF9="attempt to over-write an aggregate with const member %s";_tag_fat(_tmpF9,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp6BE,_tmp6BD,_tag_fat(_tmpF8,sizeof(void*),1U));});});
return 0;}
# 589
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 591
return 1;}case 4U: _LL7: _tmpF0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEE)->f1).elt_type;_tmpF1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEE)->f1).tq;_LL8: {void*elt_type=_tmpF0;struct Cyc_Absyn_Tqual tq=_tmpF1;
# 603
if(tq.real_const){
({void*_tmp103=0U;({unsigned _tmp6C0=loc;struct _fat_ptr _tmp6BF=({const char*_tmp104="attempt to over-write a const array";_tag_fat(_tmp104,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6C0,_tmp6BF,_tag_fat(_tmp103,sizeof(void*),0U));});});
return 0;}
# 607
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmpEF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpEE)->f1;_LLA: {struct Cyc_List_List*fs=_tmpEF;
# 609
for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp105=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp105;struct _tuple17*_tmp106=_stmttmpE;void*_tmp108;struct Cyc_Absyn_Tqual _tmp107;_LL11: _tmp107=_tmp106->f1;_tmp108=_tmp106->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmp107;void*t=_tmp108;
if(tq.real_const){
({void*_tmp109=0U;({unsigned _tmp6C2=loc;struct _fat_ptr _tmp6C1=({const char*_tmp10A="attempt to over-write a tuple field with a const member";_tag_fat(_tmp10A,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp6C2,_tmp6C1,_tag_fat(_tmp109,sizeof(void*),0U));});});
return 0;}
# 615
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}
# 617
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 625
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 628
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp10B=e->r;void*_stmttmpF=_tmp10B;void*_tmp10C=_stmttmpF;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct _fat_ptr*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct _fat_ptr*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Vardecl*_tmp116;struct Cyc_Absyn_Vardecl*_tmp117;struct Cyc_Absyn_Vardecl*_tmp118;struct Cyc_Absyn_Vardecl*_tmp119;switch(*((int*)_tmp10C)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)){case 3U: _LL1: _tmp119=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp119;
_tmp118=vd;goto _LL4;}case 4U: _LL3: _tmp118=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp118;
_tmp117=vd;goto _LL6;}case 5U: _LL5: _tmp117=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp117;
_tmp116=vd;goto _LL8;}case 1U: _LL7: _tmp116=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp116;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23U: _LL9: _tmp114=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_tmp115=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10C)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp114;struct Cyc_Absyn_Exp*e2=_tmp115;
# 635
{void*_tmp11A=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp11A;void*_tmp11B=_stmttmp10;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Tqual _tmp11D;struct Cyc_Absyn_Tqual _tmp11E;switch(*((int*)_tmp11B)){case 3U: _LL18: _tmp11E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11B)->f1).elt_tq;_LL19: {struct Cyc_Absyn_Tqual tq=_tmp11E;
_tmp11D=tq;goto _LL1B;}case 4U: _LL1A: _tmp11D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11B)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp11D;
if(!tq.real_const)return;goto _LL17;}case 6U: _LL1C: _tmp11C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp11B)->f1;_LL1D: {struct Cyc_List_List*ts=_tmp11C;
# 639
struct _tuple16 _tmp11F=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp11=_tmp11F;struct _tuple16 _tmp120=_stmttmp11;int _tmp122;unsigned _tmp121;_LL21: _tmp121=_tmp120.f1;_tmp122=_tmp120.f2;_LL22: {unsigned i=_tmp121;int known=_tmp122;
if(!known){
({void*_tmp123=0U;({unsigned _tmp6C4=e->loc;struct _fat_ptr _tmp6C3=({const char*_tmp124="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp124,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6C4,_tmp6C3,_tag_fat(_tmp123,sizeof(void*),0U));});});
return;}
# 644
{struct _handler_cons _tmp125;_push_handler(& _tmp125);{int _tmp127=0;if(setjmp(_tmp125.handler))_tmp127=1;if(!_tmp127){
{struct _tuple17*_tmp128=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i);struct _tuple17*_stmttmp12=_tmp128;struct _tuple17*_tmp129=_stmttmp12;struct Cyc_Absyn_Tqual _tmp12A;_LL24: _tmp12A=_tmp129->f1;_LL25: {struct Cyc_Absyn_Tqual tq=_tmp12A;
if(!tq.real_const){_npop_handler(0U);return;}}}
# 645
;_pop_handler();}else{void*_tmp126=(void*)Cyc_Core_get_exn_thrown();void*_tmp12B=_tmp126;void*_tmp12C;if(((struct Cyc_List_Nth_exn_struct*)_tmp12B)->tag == Cyc_List_Nth){_LL27: _LL28:
# 647
 return;}else{_LL29: _tmp12C=_tmp12B;_LL2A: {void*exn=_tmp12C;(int)_rethrow(exn);}}_LL26:;}}}
goto _LL17;}}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 651
goto _LL0;}case 21U: _LLB: _tmp112=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_tmp113=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10C)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp112;struct _fat_ptr*f=_tmp113;
# 653
{void*_tmp12D=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp12D;void*_tmp12E=_stmttmp13;struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_Aggrdecl**_tmp130;switch(*((int*)_tmp12E)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp130=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)->f1).KnownAggr).val;_LL2D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp130;
# 655
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp12F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12E)->f2;_LL2F: {struct Cyc_List_List*fs=_tmp12F;
# 660
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 665
goto _LL0;}case 22U: _LLD: _tmp110=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_tmp111=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10C)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp110;struct _fat_ptr*f=_tmp111;
# 667
{void*_tmp131=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp131;void*_tmp132=_stmttmp14;struct Cyc_Absyn_Tqual _tmp134;void*_tmp133;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132)->tag == 3U){_LL33: _tmp133=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132)->f1).elt_type;_tmp134=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132)->f1).elt_tq;_LL34: {void*elt_typ=_tmp133;struct Cyc_Absyn_Tqual tq=_tmp134;
# 669
if(!tq.real_const){
void*_tmp135=Cyc_Tcutil_compress(elt_typ);void*_stmttmp15=_tmp135;void*_tmp136=_stmttmp15;struct Cyc_List_List*_tmp137;struct Cyc_Absyn_Aggrdecl**_tmp138;switch(*((int*)_tmp136)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp136)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp136)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp138=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp136)->f1)->f1).KnownAggr).val;_LL39: {struct Cyc_Absyn_Aggrdecl**adp=_tmp138;
# 672
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp137=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp136)->f2;_LL3B: {struct Cyc_List_List*fs=_tmp137;
# 677
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 683
goto _LL32;}}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 686
goto _LL0;}case 20U: _LLF: _tmp10F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp10F;
# 688
{void*_tmp139=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp139;void*_tmp13A=_stmttmp16;struct Cyc_Absyn_Tqual _tmp13B;struct Cyc_Absyn_Tqual _tmp13C;switch(*((int*)_tmp13A)){case 3U: _LL3F: _tmp13C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13A)->f1).elt_tq;_LL40: {struct Cyc_Absyn_Tqual tq=_tmp13C;
_tmp13B=tq;goto _LL42;}case 4U: _LL41: _tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13A)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp13B;
if(!tq.real_const)return;goto _LL3E;}default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 693
goto _LL0;}case 12U: _LL11: _tmp10E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp10E;
_tmp10D=e1;goto _LL14;}case 13U: _LL13: _tmp10D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp10D;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 698
({struct Cyc_String_pa_PrintArg_struct _tmp13F=({struct Cyc_String_pa_PrintArg_struct _tmp578;_tmp578.tag=0U,({struct _fat_ptr _tmp6C5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp578.f1=_tmp6C5;});_tmp578;});void*_tmp13D[1U];_tmp13D[0]=& _tmp13F;({unsigned _tmp6C7=e->loc;struct _fat_ptr _tmp6C6=({const char*_tmp13E="attempt to write a const location: %s";_tag_fat(_tmp13E,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp6C7,_tmp6C6,_tag_fat(_tmp13D,sizeof(void*),1U));});});}
# 701
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 704
({struct Cyc_Tcenv_Tenv*_tmp6C8=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp6C8,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp141=({struct Cyc_Warn_String_Warn_Warg_struct _tmp579;_tmp579.tag=0U,({struct _fat_ptr _tmp6C9=({const char*_tmp142="increment/decrement of non-lvalue";_tag_fat(_tmp142,sizeof(char),34U);});_tmp579.f1=_tmp6C9;});_tmp579;});void*_tmp140[1U];_tmp140[0]=& _tmp141;({struct Cyc_Tcenv_Tenv*_tmp6CC=te;unsigned _tmp6CB=loc;void**_tmp6CA=topt;Cyc_Tcexp_expr_err(_tmp6CC,_tmp6CB,_tmp6CA,_tag_fat(_tmp140,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 710
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp143=0U;({unsigned _tmp6CE=e->loc;struct _fat_ptr _tmp6CD=({const char*_tmp144="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp144,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6CE,_tmp6CD,_tag_fat(_tmp143,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp145=0U;({unsigned _tmp6D0=e->loc;struct _fat_ptr _tmp6CF=({const char*_tmp146="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp146,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp6D0,_tmp6CF,_tag_fat(_tmp145,sizeof(void*),0U));});});}else{
# 719
({struct Cyc_String_pa_PrintArg_struct _tmp149=({struct Cyc_String_pa_PrintArg_struct _tmp57A;_tmp57A.tag=0U,({struct _fat_ptr _tmp6D1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp57A.f1=_tmp6D1;});_tmp57A;});void*_tmp147[1U];_tmp147[0]=& _tmp149;({unsigned _tmp6D3=e->loc;struct _fat_ptr _tmp6D2=({const char*_tmp148="expecting arithmetic or ? type but found %s";_tag_fat(_tmp148,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp6D3,_tmp6D2,_tag_fat(_tmp147,sizeof(void*),1U));});});}}
# 721
return t;}}
# 725
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 727
({struct Cyc_Tcenv_Tenv*_tmp6D5=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp6D4=e1;Cyc_Tcexp_tcTest(_tmp6D5,_tmp6D4,({const char*_tmp14A="conditional expression";_tag_fat(_tmp14A,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 734
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp14B=({struct Cyc_List_List _tmp580;_tmp580.hd=e3,_tmp580.tl=0;_tmp580;});struct Cyc_List_List l1=_tmp14B;
struct Cyc_List_List _tmp14C=({struct Cyc_List_List _tmp57F;_tmp57F.hd=e2,_tmp57F.tl=& l1;_tmp57F;});struct Cyc_List_List l2=_tmp14C;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6D6=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp6D6,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57E;_tmp57E.tag=0U,({struct _fat_ptr _tmp6D7=({const char*_tmp153="conditional clause types do not match: ";_tag_fat(_tmp153,sizeof(char),40U);});_tmp57E.f1=_tmp6D7;});_tmp57E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57D;_tmp57D.tag=2U,_tmp57D.f1=(void*)_check_null(e2->topt);_tmp57D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp150=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57C;_tmp57C.tag=0U,({
struct _fat_ptr _tmp6D8=({const char*_tmp152=" != ";_tag_fat(_tmp152,sizeof(char),5U);});_tmp57C.f1=_tmp6D8;});_tmp57C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp151=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57B;_tmp57B.tag=2U,_tmp57B.f1=(void*)_check_null(e3->topt);_tmp57B;});void*_tmp14D[4U];_tmp14D[0]=& _tmp14E,_tmp14D[1]=& _tmp14F,_tmp14D[2]=& _tmp150,_tmp14D[3]=& _tmp151;({unsigned _tmp6D9=loc;Cyc_Tcexp_err_and_explain(_tmp6D9,_tag_fat(_tmp14D,sizeof(void*),4U));});});
return t;}}}
# 744
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 746
({struct Cyc_Tcenv_Tenv*_tmp6DB=te;struct Cyc_Absyn_Exp*_tmp6DA=e1;Cyc_Tcexp_tcTest(_tmp6DB,_tmp6DA,({const char*_tmp154="logical-and expression";_tag_fat(_tmp154,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6DD=te;struct Cyc_Absyn_Exp*_tmp6DC=e2;Cyc_Tcexp_tcTest(_tmp6DD,_tmp6DC,({const char*_tmp155="logical-and expression";_tag_fat(_tmp155,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 752
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 754
({struct Cyc_Tcenv_Tenv*_tmp6DF=te;struct Cyc_Absyn_Exp*_tmp6DE=e1;Cyc_Tcexp_tcTest(_tmp6DF,_tmp6DE,({const char*_tmp156="logical-or expression";_tag_fat(_tmp156,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6E1=te;struct Cyc_Absyn_Exp*_tmp6E0=e2;Cyc_Tcexp_tcTest(_tmp6E1,_tmp6E0,({const char*_tmp157="logical-or expression";_tag_fat(_tmp157,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 760
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 767
({struct Cyc_Tcenv_Tenv*_tmp6E2=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp6E2,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 772
{void*_tmp158=Cyc_Tcutil_compress(t1);void*_stmttmp17=_tmp158;void*_tmp159=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp159)->tag == 4U){_LL1: _LL2:
({void*_tmp15A=0U;({unsigned _tmp6E4=loc;struct _fat_ptr _tmp6E3=({const char*_tmp15B="cannot assign to an array";_tag_fat(_tmp15B,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp6E4,_tmp6E3,_tag_fat(_tmp15A,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 777
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp15C=0U;({unsigned _tmp6E6=loc;struct _fat_ptr _tmp6E5=({const char*_tmp15D="type is abstract (can't determine size)";_tag_fat(_tmp15D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6E6,_tmp6E5,_tag_fat(_tmp15C,sizeof(void*),0U));});});
# 781
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp15F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0U,({struct _fat_ptr _tmp6E7=({const char*_tmp160="assignment to non-lvalue";_tag_fat(_tmp160,sizeof(char),25U);});_tmp581.f1=_tmp6E7;});_tmp581;});void*_tmp15E[1U];_tmp15E[0]=& _tmp15F;({struct Cyc_Tcenv_Tenv*_tmp6EA=te;unsigned _tmp6E9=loc;void**_tmp6E8=topt;Cyc_Tcexp_expr_err(_tmp6EA,_tmp6E9,_tmp6E8,_tag_fat(_tmp15E,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e2))
Cyc_Tcexp_unique_consume_err(e2->loc);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6EC=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6EB=e2;Cyc_Tcutil_coerce_assign(_tmp6EC,_tmp6EB,t1);})){
void*_tmp161=({struct Cyc_Warn_String_Warn_Warg_struct _tmp163=({struct Cyc_Warn_String_Warn_Warg_struct _tmp585;_tmp585.tag=0U,({struct _fat_ptr _tmp6ED=({const char*_tmp168="type mismatch: ";_tag_fat(_tmp168,sizeof(char),16U);});_tmp585.f1=_tmp6ED;});_tmp585;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp164=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp584;_tmp584.tag=2U,_tmp584.f1=(void*)t1;_tmp584;});struct Cyc_Warn_String_Warn_Warg_struct _tmp165=({struct Cyc_Warn_String_Warn_Warg_struct _tmp583;_tmp583.tag=0U,({struct _fat_ptr _tmp6EE=({const char*_tmp167=" != ";_tag_fat(_tmp167,sizeof(char),5U);});_tmp583.f1=_tmp6EE;});_tmp583;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp166=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp582;_tmp582.tag=2U,_tmp582.f1=(void*)t2;_tmp582;});void*_tmp162[4U];_tmp162[0]=& _tmp163,_tmp162[1]=& _tmp164,_tmp162[2]=& _tmp165,_tmp162[3]=& _tmp166;({struct Cyc_Tcenv_Tenv*_tmp6F1=te;unsigned _tmp6F0=loc;void**_tmp6EF=topt;Cyc_Tcexp_expr_err(_tmp6F1,_tmp6F0,_tmp6EF,_tag_fat(_tmp162,sizeof(void*),4U));});});void*result=_tmp161;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}}else{
# 794
enum Cyc_Absyn_Primop _tmp169=(enum Cyc_Absyn_Primop)po->v;enum Cyc_Absyn_Primop p=_tmp169;
struct Cyc_Absyn_Exp*_tmp16A=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp16A;
void*_tmp16B=Cyc_Tcexp_tcBinPrimop(te,loc,0,p,e1copy,e2);void*t_result=_tmp16B;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
t_result=t1;else{
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp16C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58A;_tmp58A.tag=0U,({
struct _fat_ptr _tmp6F2=({const char*_tmp175="Cannot use this operator in an assignment when ";_tag_fat(_tmp175,sizeof(char),48U);});_tmp58A.f1=_tmp6F2;});_tmp58A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp589;_tmp589.tag=0U,({
struct _fat_ptr _tmp6F3=({const char*_tmp174="the arguments have types ";_tag_fat(_tmp174,sizeof(char),26U);});_tmp589.f1=_tmp6F3;});_tmp589;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp170=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp588;_tmp588.tag=2U,_tmp588.f1=(void*)t1;_tmp588;});struct Cyc_Warn_String_Warn_Warg_struct _tmp171=({struct Cyc_Warn_String_Warn_Warg_struct _tmp587;_tmp587.tag=0U,({struct _fat_ptr _tmp6F4=({const char*_tmp173=" and ";_tag_fat(_tmp173,sizeof(char),6U);});_tmp587.f1=_tmp6F4;});_tmp587;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp172=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp586;_tmp586.tag=2U,_tmp586.f1=(void*)t2;_tmp586;});void*_tmp16D[5U];_tmp16D[0]=& _tmp16E,_tmp16D[1]=& _tmp16F,_tmp16D[2]=& _tmp170,_tmp16D[3]=& _tmp171,_tmp16D[4]=& _tmp172;({struct Cyc_Tcenv_Tenv*_tmp6F7=te;unsigned _tmp6F6=loc;void**_tmp6F5=topt;Cyc_Tcexp_expr_err(_tmp6F7,_tmp6F6,_tmp6F5,_tag_fat(_tmp16D,sizeof(void*),5U));});});
# 800
void*result=_tmp16C;
# 803
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}}
# 807
return t_result;}
# 809
return t1;}}}
# 813
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp6F8=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp6F8,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp6FA=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp6F9=topt;Cyc_Tcexp_tcExp(_tmp6FA,_tmp6F9,e2);});
return(void*)_check_null(e2->topt);}
# 820
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 823
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 826
{void*_tmp176=Cyc_Tcutil_compress(t1);void*_stmttmp18=_tmp176;void*_tmp177=_stmttmp18;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177)->f1)->f1 == 0){_LL1: _LL2:
# 828
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
# 831
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 835
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp178=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp178;struct Cyc_Absyn_Datatypefield _tmp179=_stmttmp19;enum Cyc_Absyn_Scope _tmp17D;unsigned _tmp17C;struct Cyc_List_List*_tmp17B;struct _tuple1*_tmp17A;_LLA: _tmp17A=_tmp179.name;_tmp17B=_tmp179.typs;_tmp17C=_tmp179.loc;_tmp17D=_tmp179.sc;_LLB: {struct _tuple1*n=_tmp17A;struct Cyc_List_List*typs=_tmp17B;unsigned loc=_tmp17C;enum Cyc_Absyn_Scope sc=_tmp17D;
# 838
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 841
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 845
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp17E=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp17E;struct Cyc_Absyn_Datatypefield _tmp17F=_stmttmp1A;enum Cyc_Absyn_Scope _tmp183;unsigned _tmp182;struct Cyc_List_List*_tmp181;struct _tuple1*_tmp180;_LLD: _tmp180=_tmp17F.name;_tmp181=_tmp17F.typs;_tmp182=_tmp17F.loc;_tmp183=_tmp17F.sc;_LLE: {struct _tuple1*n=_tmp180;struct Cyc_List_List*typs=_tmp181;unsigned loc=_tmp182;enum Cyc_Absyn_Scope sc=_tmp183;
# 848
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 852
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp6FC=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6FB=e;Cyc_Tcutil_coerce_arg(_tmp6FC,_tmp6FB,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 856
({struct Cyc_Warn_String_Warn_Warg_struct _tmp185=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58E;_tmp58E.tag=0U,({struct _fat_ptr _tmp6FD=({const char*_tmp18A="can't find a field in ";_tag_fat(_tmp18A,sizeof(char),23U);});_tmp58E.f1=_tmp6FD;});_tmp58E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp186=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58D;_tmp58D.tag=2U,_tmp58D.f1=(void*)tu;_tmp58D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp187=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58C;_tmp58C.tag=0U,({struct _fat_ptr _tmp6FE=({const char*_tmp189=" to inject a value of type ";_tag_fat(_tmp189,sizeof(char),28U);});_tmp58C.f1=_tmp6FE;});_tmp58C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp188=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58B;_tmp58B.tag=2U,_tmp58B.f1=(void*)t1;_tmp58B;});void*_tmp184[4U];_tmp184[0]=& _tmp185,_tmp184[1]=& _tmp186,_tmp184[2]=& _tmp187,_tmp184[3]=& _tmp188;({unsigned _tmp6FF=e->loc;Cyc_Warn_err2(_tmp6FF,_tag_fat(_tmp184,sizeof(void*),4U));});});
return 0;}
# 861
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 867
struct Cyc_List_List*_tmp18B=args;struct Cyc_List_List*es=_tmp18B;
int _tmp18C=0;int arg_cnt=_tmp18C;
struct Cyc_Tcenv_Tenv*_tmp18D=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp18D;
struct Cyc_Tcenv_Tenv*_tmp18E=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp18E;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 876
void*_tmp18F=t;void*_tmp195;void*_tmp194;void*_tmp193;void*_tmp192;struct Cyc_Absyn_Tqual _tmp191;void*_tmp190;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->tag == 3U){_LL1: _tmp190=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).elt_type;_tmp191=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).elt_tq;_tmp192=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).rgn;_tmp193=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).nullable;_tmp194=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).bounds;_tmp195=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).zero_term;_LL2: {void*t1=_tmp190;struct Cyc_Absyn_Tqual tq=_tmp191;void*rgn=_tmp192;void*x=_tmp193;void*b=_tmp194;void*zt=_tmp195;
# 881
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 883
Cyc_Tcutil_check_nonzero_bound(loc,b);{
void*_tmp196=Cyc_Tcutil_compress(t1);void*_stmttmp1B=_tmp196;void*_tmp197=_stmttmp1B;struct Cyc_List_List*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_List_List*_tmp1A0;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_VarargInfo*_tmp19E;int _tmp19D;struct Cyc_List_List*_tmp19C;void*_tmp19B;struct Cyc_Absyn_Tqual _tmp19A;void*_tmp199;struct Cyc_List_List*_tmp198;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->tag == 5U){_LL6: _tmp198=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).tvars;_tmp199=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).effect;_tmp19A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).ret_tqual;_tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).ret_type;_tmp19C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).args;_tmp19D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).c_varargs;_tmp19E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).cyc_varargs;_tmp19F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).rgn_po;_tmp1A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).attributes;_tmp1A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).requires_clause;_tmp1A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).requires_relns;_tmp1A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).ensures_clause;_tmp1A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).ensures_relns;_LL7: {struct Cyc_List_List*tvars=_tmp198;void*eff=_tmp199;struct Cyc_Absyn_Tqual res_tq=_tmp19A;void*res_typ=_tmp19B;struct Cyc_List_List*args_info=_tmp19C;int c_vararg=_tmp19D;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp19E;struct Cyc_List_List*rgn_po=_tmp19F;struct Cyc_List_List*atts=_tmp1A0;struct Cyc_Absyn_Exp*req=_tmp1A1;struct Cyc_List_List*req_relns=_tmp1A2;struct Cyc_Absyn_Exp*ens=_tmp1A3;struct Cyc_List_List*ens_relns=_tmp1A4;
# 888
if(tvars != 0 || rgn_po != 0)
({void*_tmp1A5=0U;({unsigned _tmp701=e->loc;struct _fat_ptr _tmp700=({const char*_tmp1A6="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1A6,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp701,_tmp700,_tag_fat(_tmp1A5,sizeof(void*),0U));});});
# 892
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 894
while(es != 0 && args_info != 0){
# 896
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp703=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp702=e1;Cyc_Tcutil_coerce_arg(_tmp703,_tmp702,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1B6="actual argument has type ";_tag_fat(_tmp1B6,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1B5=" but formal has type ";_tag_fat(_tmp1B5,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp706=({unsigned long _tmp705=({unsigned long _tmp704=Cyc_strlen((struct _fat_ptr)s0);_tmp704 + Cyc_strlen((struct _fat_ptr)s1);});_tmp705 + Cyc_strlen((struct _fat_ptr)s2);});_tmp706 + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({void*_tmp1A7=0U;({unsigned _tmp709=e1->loc;struct _fat_ptr _tmp708=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1AA=({struct Cyc_String_pa_PrintArg_struct _tmp592;_tmp592.tag=0U,_tmp592.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp592;});struct Cyc_String_pa_PrintArg_struct _tmp1AB=({struct Cyc_String_pa_PrintArg_struct _tmp591;_tmp591.tag=0U,_tmp591.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp591;});struct Cyc_String_pa_PrintArg_struct _tmp1AC=({struct Cyc_String_pa_PrintArg_struct _tmp590;_tmp590.tag=0U,_tmp590.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp590;});struct Cyc_String_pa_PrintArg_struct _tmp1AD=({struct Cyc_String_pa_PrintArg_struct _tmp58F;_tmp58F.tag=0U,_tmp58F.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp58F;});void*_tmp1A8[4U];_tmp1A8[0]=& _tmp1AA,_tmp1A8[1]=& _tmp1AB,_tmp1A8[2]=& _tmp1AC,_tmp1A8[3]=& _tmp1AD;({struct _fat_ptr _tmp707=({const char*_tmp1A9="%s\n\t%s\n%s\n\t%s.";_tag_fat(_tmp1A9,sizeof(char),15U);});Cyc_aprintf(_tmp707,_tag_fat(_tmp1A8,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp709,_tmp708,_tag_fat(_tmp1A7,sizeof(void*),0U));});});else{
# 908
({void*_tmp1AE=0U;({unsigned _tmp70C=e1->loc;struct _fat_ptr _tmp70B=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp596;_tmp596.tag=0U,_tmp596.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp596;});struct Cyc_String_pa_PrintArg_struct _tmp1B2=({struct Cyc_String_pa_PrintArg_struct _tmp595;_tmp595.tag=0U,_tmp595.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp595;});struct Cyc_String_pa_PrintArg_struct _tmp1B3=({struct Cyc_String_pa_PrintArg_struct _tmp594;_tmp594.tag=0U,_tmp594.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp594;});struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp593;_tmp593.tag=0U,_tmp593.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp593;});void*_tmp1AF[4U];_tmp1AF[0]=& _tmp1B1,_tmp1AF[1]=& _tmp1B2,_tmp1AF[2]=& _tmp1B3,_tmp1AF[3]=& _tmp1B4;({struct _fat_ptr _tmp70A=({const char*_tmp1B0="%s%s%s%s.";_tag_fat(_tmp1B0,sizeof(char),10U);});Cyc_aprintf(_tmp70A,_tag_fat(_tmp1AF,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp70C,_tmp70B,_tag_fat(_tmp1AE,sizeof(void*),0U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 913
if(alias_coercion)
({struct Cyc_List_List*_tmp70D=({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->hd=(void*)arg_cnt,_tmp1B7->tl=*alias_arg_exps;_tmp1B7;});*alias_arg_exps=_tmp70D;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 924
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1B8=(void*)a->hd;void*_stmttmp1C=_tmp1B8;void*_tmp1B9=_stmttmp1C;int _tmp1BC;int _tmp1BB;enum Cyc_Absyn_Format_Type _tmp1BA;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1B9)->tag == 19U){_LLB: _tmp1BA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1B9)->f1;_tmp1BB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1B9)->f2;_tmp1BC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1B9)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp1BA;int fmt_arg_pos=_tmp1BB;int arg_start_pos=_tmp1BC;
# 928
{struct _handler_cons _tmp1BD;_push_handler(& _tmp1BD);{int _tmp1BF=0;if(setjmp(_tmp1BD.handler))_tmp1BF=1;if(!_tmp1BF){
# 930
{struct Cyc_Absyn_Exp*_tmp1C0=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1C0;
# 932
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 936
fmt_args=({struct Cyc_Core_Opt*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));({struct Cyc_List_List*_tmp70E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp1C1->v=_tmp70E;});_tmp1C1;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1C2=ft;switch(_tmp1C2){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 940
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 943
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 946
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 949
goto _LLF;}_LLF:;}}
# 930
;_pop_handler();}else{void*_tmp1BE=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C3=_tmp1BE;void*_tmp1C4;if(((struct Cyc_List_Nth_exn_struct*)_tmp1C3)->tag == Cyc_List_Nth){_LL17: _LL18:
# 951
({void*_tmp1C5=0U;({unsigned _tmp710=loc;struct _fat_ptr _tmp70F=({const char*_tmp1C6="bad format arguments";_tag_fat(_tmp1C6,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp710,_tmp70F,_tag_fat(_tmp1C5,sizeof(void*),0U));});});goto _LL16;}else{_LL19: _tmp1C4=_tmp1C3;_LL1A: {void*exn=_tmp1C4;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 956
if(args_info != 0)({void*_tmp1C7=0U;({unsigned _tmp712=loc;struct _fat_ptr _tmp711=({const char*_tmp1C8="too few arguments for function";_tag_fat(_tmp1C8,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp712,_tmp711,_tag_fat(_tmp1C7,sizeof(void*),0U));});});else{
# 958
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({void*_tmp1C9=0U;({unsigned _tmp714=loc;struct _fat_ptr _tmp713=({const char*_tmp1CA="too many arguments for function";_tag_fat(_tmp1CA,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp714,_tmp713,_tag_fat(_tmp1C9,sizeof(void*),0U));});});else{
# 965
struct Cyc_Absyn_VarargInfo _tmp1CB=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1D=_tmp1CB;struct Cyc_Absyn_VarargInfo _tmp1CC=_stmttmp1D;int _tmp1CE;void*_tmp1CD;_LL1C: _tmp1CD=_tmp1CC.type;_tmp1CE=_tmp1CC.inject;_LL1D: {void*vt=_tmp1CD;int inject=_tmp1CE;
struct Cyc_Absyn_VarargCallInfo*_tmp1CF=({struct Cyc_Absyn_VarargCallInfo*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->num_varargs=0,_tmp1E5->injectors=0,_tmp1E5->vai=cyc_vararg;_tmp1E5;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1CF;
# 969
*vararg_call_info=vci;
# 971
if(!inject)
# 973
for(0;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp716=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp715=e1;Cyc_Tcutil_coerce_arg(_tmp716,_tmp715,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59A;_tmp59A.tag=0U,({struct _fat_ptr _tmp717=({const char*_tmp1D6="vararg requires type ";_tag_fat(_tmp1D6,sizeof(char),22U);});_tmp59A.f1=_tmp717;});_tmp59A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1D2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp599;_tmp599.tag=2U,_tmp599.f1=(void*)vt;_tmp599;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp598;_tmp598.tag=0U,({
struct _fat_ptr _tmp718=({const char*_tmp1D5=" but argument has type ";_tag_fat(_tmp1D5,sizeof(char),24U);});_tmp598.f1=_tmp718;});_tmp598;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1D4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp597;_tmp597.tag=2U,_tmp597.f1=(void*)_check_null(e1->topt);_tmp597;});void*_tmp1D0[4U];_tmp1D0[0]=& _tmp1D1,_tmp1D0[1]=& _tmp1D2,_tmp1D0[2]=& _tmp1D3,_tmp1D0[3]=& _tmp1D4;({unsigned _tmp719=loc;Cyc_Tcexp_err_and_explain(_tmp719,_tag_fat(_tmp1D0,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp71A=({struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->hd=(void*)arg_cnt,_tmp1D7->tl=*alias_arg_exps;_tmp1D7;});*alias_arg_exps=_tmp71A;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vt)&& !
Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);}else{
# 990
void*_tmp1D8=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1E=_tmp1D8;void*_tmp1D9=_stmttmp1E;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Datatypedecl*_tmp1DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1DA=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f1)->f1).KnownDatatype).val;_tmp1DB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp1DA;struct Cyc_List_List*targs=_tmp1DB;
# 994
struct Cyc_Absyn_Datatypedecl*_tmp1DC=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp1DC;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1DF=({struct Cyc_String_pa_PrintArg_struct _tmp59B;_tmp59B.tag=0U,({
struct _fat_ptr _tmp71B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(vt));_tmp59B.f1=_tmp71B;});_tmp59B;});void*_tmp1DD[1U];_tmp1DD[0]=& _tmp1DF;({unsigned _tmp71D=loc;struct _fat_ptr _tmp71C=({const char*_tmp1DE="can't inject into abstract datatype %s";_tag_fat(_tmp1DE,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp71D,_tmp71C,_tag_fat(_tmp1DD,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 1005
({void*_tmp71E=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp71E,Cyc_Tcenv_curr_rgn(te));});{
# 1007
struct Cyc_List_List*_tmp1E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp1E0;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1012
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt))&& !
Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);}{
# 1018
struct Cyc_Absyn_Datatypefield*_tmp1E1=({struct Cyc_Tcenv_Tenv*_tmp722=te;struct Cyc_Absyn_Exp*_tmp721=e1;void*_tmp720=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp71F=inst;Cyc_Tcexp_tcInjection(_tmp722,_tmp721,_tmp720,_tmp71F,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp1E1;
if(f != 0)
({struct Cyc_List_List*_tmp724=({
struct Cyc_List_List*_tmp723=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp723,({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));
_tmp1E2->hd=f,_tmp1E2->tl=0;_tmp1E2;}));});
# 1020
vci->injectors=_tmp724;});}}}
# 1024
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({void*_tmp1E3=0U;({unsigned _tmp726=loc;struct _fat_ptr _tmp725=({const char*_tmp1E4="bad inject vararg type";_tag_fat(_tmp1E4,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp726,_tmp725,_tag_fat(_tmp1E3,sizeof(void*),0U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1030
if(*alias_arg_exps == 0)
# 1039 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1041
return res_typ;}}}else{_LL8: _LL9:
# 1043
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59D;_tmp59D.tag=0U,({struct _fat_ptr _tmp727=({const char*_tmp1E9="expected pointer to function but found ";_tag_fat(_tmp1E9,sizeof(char),40U);});_tmp59D.f1=_tmp727;});_tmp59D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59C;_tmp59C.tag=2U,_tmp59C.f1=(void*)t;_tmp59C;});void*_tmp1E6[2U];_tmp1E6[0]=& _tmp1E7,_tmp1E6[1]=& _tmp1E8;({struct Cyc_Tcenv_Tenv*_tmp72A=te;unsigned _tmp729=loc;void**_tmp728=topt;Cyc_Tcexp_expr_err(_tmp72A,_tmp729,_tmp728,_tag_fat(_tmp1E6,sizeof(void*),2U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1046
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59F;_tmp59F.tag=0U,({struct _fat_ptr _tmp72B=({const char*_tmp1ED="expected pointer to function but found ";_tag_fat(_tmp1ED,sizeof(char),40U);});_tmp59F.f1=_tmp72B;});_tmp59F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1EC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59E;_tmp59E.tag=2U,_tmp59E.f1=(void*)t;_tmp59E;});void*_tmp1EA[2U];_tmp1EA[0]=& _tmp1EB,_tmp1EA[1]=& _tmp1EC;({struct Cyc_Tcenv_Tenv*_tmp72E=te;unsigned _tmp72D=loc;void**_tmp72C=topt;Cyc_Tcexp_expr_err(_tmp72E,_tmp72D,_tmp72C,_tag_fat(_tmp1EA,sizeof(void*),2U));});});}_LL0:;}}}
# 1050
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1EE=Cyc_Absyn_exn_type();void*exception_type=_tmp1EE;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp730=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp72F=e;Cyc_Tcutil_coerce_arg(_tmp730,_tmp72F,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A3;_tmp5A3.tag=0U,({struct _fat_ptr _tmp731=({const char*_tmp1F5="expected ";_tag_fat(_tmp1F5,sizeof(char),10U);});_tmp5A3.f1=_tmp731;});_tmp5A3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A2;_tmp5A2.tag=2U,_tmp5A2.f1=(void*)exception_type;_tmp5A2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A1;_tmp5A1.tag=0U,({struct _fat_ptr _tmp732=({const char*_tmp1F4=" but found ";_tag_fat(_tmp1F4,sizeof(char),12U);});_tmp5A1.f1=_tmp732;});_tmp5A1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=2U,_tmp5A0.f1=(void*)_check_null(e->topt);_tmp5A0;});void*_tmp1EF[4U];_tmp1EF[0]=& _tmp1F0,_tmp1EF[1]=& _tmp1F1,_tmp1EF[2]=& _tmp1F2,_tmp1EF[3]=& _tmp1F3;({unsigned _tmp733=loc;Cyc_Warn_err2(_tmp733,_tag_fat(_tmp1EF,sizeof(void*),4U));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1062
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1064
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1066
({void*_tmp734=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp734;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1F6=t1;void*_tmp1FC;void*_tmp1FB;void*_tmp1FA;void*_tmp1F9;struct Cyc_Absyn_Tqual _tmp1F8;void*_tmp1F7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->tag == 3U){_LL1: _tmp1F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).elt_type;_tmp1F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).elt_tq;_tmp1F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).ptr_atts).rgn;_tmp1FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).ptr_atts).nullable;_tmp1FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).ptr_atts).bounds;_tmp1FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).ptr_atts).zero_term;_LL2: {void*t0=_tmp1F7;struct Cyc_Absyn_Tqual tq=_tmp1F8;void*rt=_tmp1F9;void*x=_tmp1FA;void*b=_tmp1FB;void*zt=_tmp1FC;
# 1070
{void*_tmp1FD=Cyc_Tcutil_compress(t0);void*_stmttmp1F=_tmp1FD;void*_tmp1FE=_stmttmp1F;struct Cyc_List_List*_tmp20B;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_List_List*_tmp207;struct Cyc_List_List*_tmp206;struct Cyc_Absyn_VarargInfo*_tmp205;int _tmp204;struct Cyc_List_List*_tmp203;void*_tmp202;struct Cyc_Absyn_Tqual _tmp201;void*_tmp200;struct Cyc_List_List*_tmp1FF;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->tag == 5U){_LL6: _tmp1FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).tvars;_tmp200=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).effect;_tmp201=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).ret_tqual;_tmp202=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).ret_type;_tmp203=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).args;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).c_varargs;_tmp205=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).cyc_varargs;_tmp206=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).rgn_po;_tmp207=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).attributes;_tmp208=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).requires_clause;_tmp209=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).requires_relns;_tmp20A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).ensures_clause;_tmp20B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).ensures_relns;_LL7: {struct Cyc_List_List*tvars=_tmp1FF;void*eff=_tmp200;struct Cyc_Absyn_Tqual rtq=_tmp201;void*rtyp=_tmp202;struct Cyc_List_List*args=_tmp203;int c_varargs=_tmp204;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp205;struct Cyc_List_List*rpo=_tmp206;struct Cyc_List_List*atts=_tmp207;struct Cyc_Absyn_Exp*req=_tmp208;struct Cyc_List_List*req_relns=_tmp209;struct Cyc_Absyn_Exp*ens=_tmp20A;struct Cyc_List_List*ens_relns=_tmp20B;
# 1072
struct Cyc_List_List*instantiation=0;
# 1074
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);
({unsigned _tmp738=loc;struct Cyc_Tcenv_Tenv*_tmp737=te;struct Cyc_List_List*_tmp736=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp735=k;Cyc_Tctyp_check_type(_tmp738,_tmp737,_tmp736,_tmp735,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));({struct _tuple14*_tmp739=({struct _tuple14*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp20C->f2=(void*)ts->hd;_tmp20C;});_tmp20D->hd=_tmp739;}),_tmp20D->tl=instantiation;_tmp20D;});}
# 1080
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp20F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A4;_tmp5A4.tag=0U,({struct _fat_ptr _tmp73A=({const char*_tmp210="too many type variables in instantiation";_tag_fat(_tmp210,sizeof(char),41U);});_tmp5A4.f1=_tmp73A;});_tmp5A4;});void*_tmp20E[1U];_tmp20E[0]=& _tmp20F;({struct Cyc_Tcenv_Tenv*_tmp73D=te;unsigned _tmp73C=loc;void**_tmp73B=topt;Cyc_Tcexp_expr_err(_tmp73D,_tmp73C,_tmp73B,_tag_fat(_tmp20E,sizeof(void*),1U));});});
# 1085
if(tvars == 0){
rpo=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,rpo);
Cyc_Tcenv_check_rgn_partial_order(te,loc,rpo);
rpo=0;}{
# 1090
void*new_fn_typ=({
struct Cyc_List_List*_tmp73E=instantiation;Cyc_Tcutil_substitute(_tmp73E,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp212=_cycalloc(sizeof(*_tmp212));
_tmp212->tag=5U,(_tmp212->f1).tvars=tvars,(_tmp212->f1).effect=eff,(_tmp212->f1).ret_tqual=rtq,(_tmp212->f1).ret_type=rtyp,(_tmp212->f1).args=args,(_tmp212->f1).c_varargs=c_varargs,(_tmp212->f1).cyc_varargs=cyc_varargs,(_tmp212->f1).rgn_po=rpo,(_tmp212->f1).attributes=atts,(_tmp212->f1).requires_clause=req,(_tmp212->f1).requires_relns=req_relns,(_tmp212->f1).ensures_clause=ens,(_tmp212->f1).ensures_relns=ens_relns;_tmp212;}));});
# 1095
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->tag=3U,(_tmp211->f1).elt_type=new_fn_typ,(_tmp211->f1).elt_tq=tq,((_tmp211->f1).ptr_atts).rgn=rt,((_tmp211->f1).ptr_atts).nullable=x,((_tmp211->f1).ptr_atts).bounds=b,((_tmp211->f1).ptr_atts).zero_term=zt,((_tmp211->f1).ptr_atts).ptrloc=0;_tmp211;});}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1098
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1101
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp214=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=0U,({struct _fat_ptr _tmp73F=({const char*_tmp216="expecting polymorphic type but found ";_tag_fat(_tmp216,sizeof(char),38U);});_tmp5A6.f1=_tmp73F;});_tmp5A6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp215=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=2U,_tmp5A5.f1=(void*)t1;_tmp5A5;});void*_tmp213[2U];_tmp213[0]=& _tmp214,_tmp213[1]=& _tmp215;({struct Cyc_Tcenv_Tenv*_tmp742=te;unsigned _tmp741=loc;void**_tmp740=topt;Cyc_Tcexp_expr_err(_tmp742,_tmp741,_tmp740,_tag_fat(_tmp213,sizeof(void*),2U));});});}}
# 1105
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1107
({unsigned _tmp746=loc;struct Cyc_Tcenv_Tenv*_tmp745=te;struct Cyc_List_List*_tmp744=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp743=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1107
Cyc_Tctyp_check_type(_tmp746,_tmp745,_tmp744,_tmp743,1,0,t);});
# 1109
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp749=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp748=loc;void*_tmp747=t2;Cyc_Tcutil_silent_castable(_tmp749,_tmp748,_tmp747,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1115
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp74C=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp74B=loc;void*_tmp74A=t2;Cyc_Tcutil_castable(_tmp74C,_tmp74B,_tmp74A,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1122
Cyc_Unify_unify(t2,t);{
void*_tmp217=({struct Cyc_Warn_String_Warn_Warg_struct _tmp219=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=0U,({struct _fat_ptr _tmp74D=({const char*_tmp21E="cannot cast ";_tag_fat(_tmp21E,sizeof(char),13U);});_tmp5AA.f1=_tmp74D;});_tmp5AA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=2U,_tmp5A9.f1=(void*)t2;_tmp5A9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp21B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=0U,({struct _fat_ptr _tmp74E=({const char*_tmp21D=" to ";_tag_fat(_tmp21D,sizeof(char),5U);});_tmp5A8.f1=_tmp74E;});_tmp5A8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=2U,_tmp5A7.f1=(void*)t;_tmp5A7;});void*_tmp218[4U];_tmp218[0]=& _tmp219,_tmp218[1]=& _tmp21A,_tmp218[2]=& _tmp21B,_tmp218[3]=& _tmp21C;({struct Cyc_Tcenv_Tenv*_tmp750=te;unsigned _tmp74F=loc;Cyc_Tcexp_expr_err(_tmp750,_tmp74F,& t,_tag_fat(_tmp218,sizeof(void*),4U));});});void*result=_tmp217;
Cyc_Unify_explain_failure();
return result;}}}}
# 1131
{struct _tuple0 _tmp21F=({struct _tuple0 _tmp5AB;_tmp5AB.f1=e->r,({void*_tmp751=Cyc_Tcutil_compress(t);_tmp5AB.f2=_tmp751;});_tmp5AB;});struct _tuple0 _stmttmp20=_tmp21F;struct _tuple0 _tmp220=_stmttmp20;void*_tmp224;void*_tmp223;void*_tmp222;int _tmp221;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp220.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220.f2)->tag == 3U){_LL1: _tmp221=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp220.f1)->f1).fat_result;_tmp222=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220.f2)->f1).ptr_atts).nullable;_tmp223=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220.f2)->f1).ptr_atts).bounds;_tmp224=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220.f2)->f1).ptr_atts).zero_term;_LL2: {int fat_result=_tmp221;void*nbl=_tmp222;void*bds=_tmp223;void*zt=_tmp224;
# 1135
if((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl)){
struct Cyc_Absyn_Exp*_tmp225=({void*_tmp752=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp752,bds);});struct Cyc_Absyn_Exp*eopt=_tmp225;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp226=e;struct Cyc_Absyn_Exp*e2=_tmp226;
if((Cyc_Evexp_eval_const_uint_exp(e2)).f1 == (unsigned)1)
({void*_tmp227=0U;({unsigned _tmp754=loc;struct _fat_ptr _tmp753=({const char*_tmp228="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp228,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp754,_tmp753,_tag_fat(_tmp227,sizeof(void*),0U));});});}}
# 1143
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1146
return t;}}
# 1150
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp229=0;void**topt2=_tmp229;
struct Cyc_Absyn_Tqual _tmp22A=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp22A;
int _tmp22B=0;int nullable=_tmp22B;
if(topt != 0){
void*_tmp22C=Cyc_Tcutil_compress(*topt);void*_stmttmp21=_tmp22C;void*_tmp22D=_stmttmp21;void*_tmp230;struct Cyc_Absyn_Tqual _tmp22F;void**_tmp22E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->tag == 3U){_LL1: _tmp22E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->f1).elt_type;_tmp22F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->f1).elt_tq;_tmp230=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp22E;struct Cyc_Absyn_Tqual tq=_tmp22F;void*n=_tmp230;
# 1157
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1171
({struct Cyc_Tcenv_Tenv*_tmp756=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp755=topt2;Cyc_Tcexp_tcExpNoInst(_tmp756,_tmp755,e);});
# 1174
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp231=0U;({unsigned _tmp758=e->loc;struct _fat_ptr _tmp757=({const char*_tmp232="Cannot take the address of an alias-free path";_tag_fat(_tmp232,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp758,_tmp757,_tag_fat(_tmp231,sizeof(void*),0U));});});
# 1180
{void*_tmp233=e->r;void*_stmttmp22=_tmp233;void*_tmp234=_stmttmp22;struct Cyc_Absyn_Exp*_tmp236;struct Cyc_Absyn_Exp*_tmp235;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp234)->tag == 23U){_LL6: _tmp235=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp234)->f1;_tmp236=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp234)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp235;struct Cyc_Absyn_Exp*e2=_tmp236;
# 1182
{void*_tmp237=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp23=_tmp237;void*_tmp238=_stmttmp23;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp238)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1187
({void*_tmp759=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp759;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1190
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1195
{void*_tmp239=e->r;void*_stmttmp24=_tmp239;void*_tmp23A=_stmttmp24;switch(*((int*)_tmp23A)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp23A)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp23A)->f3 == 1){_LL12: _LL13:
# 1198
({void*_tmp23B=0U;({unsigned _tmp75B=e->loc;struct _fat_ptr _tmp75A=({const char*_tmp23C="cannot take the address of a @tagged union member";_tag_fat(_tmp23C,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp75B,_tmp75A,_tag_fat(_tmp23B,sizeof(void*),0U));});});goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1203
struct _tuple15 _tmp23D=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp25=_tmp23D;struct _tuple15 _tmp23E=_stmttmp25;void*_tmp240;int _tmp23F;_LL17: _tmp23F=_tmp23E.f1;_tmp240=_tmp23E.f2;_LL18: {int is_const=_tmp23F;void*rgn=_tmp240;
# 1205
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({void*_tmp241=0U;({unsigned _tmp75D=e->loc;struct _fat_ptr _tmp75C=({const char*_tmp242="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp242,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp75D,_tmp75C,_tag_fat(_tmp241,sizeof(void*),0U));});});{
# 1208
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const){
tq.print_const=1;
tq.real_const=1;}{
# 1214
void*t=nullable?
Cyc_Absyn_star_type((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type):
 Cyc_Absyn_at_type((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type);
return t;}}}}}
# 1221
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
if(te->allow_valueof)
# 1225
return Cyc_Absyn_uint_type;
# 1227
({unsigned _tmp760=loc;struct Cyc_Tcenv_Tenv*_tmp75F=te;struct Cyc_List_List*_tmp75E=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp760,_tmp75F,_tmp75E,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp245=({struct Cyc_String_pa_PrintArg_struct _tmp5AC;_tmp5AC.tag=0U,({
struct _fat_ptr _tmp761=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5AC.f1=_tmp761;});_tmp5AC;});void*_tmp243[1U];_tmp243[0]=& _tmp245;({unsigned _tmp763=loc;struct _fat_ptr _tmp762=({const char*_tmp244="sizeof applied to type %s, which has unknown size here";_tag_fat(_tmp244,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp763,_tmp762,_tag_fat(_tmp243,sizeof(void*),1U));});});
if(topt != 0){
void*_tmp246=Cyc_Tcutil_compress(*topt);void*_stmttmp26=_tmp246;void*_tmp247=_stmttmp26;void*_tmp249;void*_tmp248;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f2 != 0){_LL1: _tmp248=_tmp247;_tmp249=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f2)->hd;_LL2: {void*tagtyp=_tmp248;void*tt=_tmp249;
# 1236
struct Cyc_Absyn_Exp*_tmp24A=Cyc_Absyn_sizeoftype_exp(t,0U);struct Cyc_Absyn_Exp*e=_tmp24A;
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24B=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->tag=9U,_tmp24C->f1=e;_tmp24C;});struct Cyc_Absyn_ValueofType_Absyn_Type_struct*tt2=_tmp24B;
if(Cyc_Unify_unify(tt,(void*)tt2))return tagtyp;
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1242
return Cyc_Absyn_uint_type;}
# 1245
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1252
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1254
({unsigned _tmp766=loc;struct Cyc_Tcenv_Tenv*_tmp765=te;struct Cyc_List_List*_tmp764=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp766,_tmp765,_tmp764,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp24D=fs;struct Cyc_List_List*l=_tmp24D;
void*_tmp24E=t_orig;void*t=_tmp24E;
for(0;l != 0;l=l->tl){
void*_tmp24F=(void*)l->hd;void*n=_tmp24F;
void*_tmp250=n;unsigned _tmp251;struct _fat_ptr*_tmp252;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp250)->tag == 0U){_LL1: _tmp252=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp250)->f1;_LL2: {struct _fat_ptr*n=_tmp252;
# 1262
int bad_type=1;
{void*_tmp253=Cyc_Tcutil_compress(t);void*_stmttmp27=_tmp253;void*_tmp254=_stmttmp27;struct Cyc_List_List*_tmp255;struct Cyc_Absyn_Aggrdecl**_tmp256;switch(*((int*)_tmp254)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp256=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp256;
# 1265
if((*adp)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields);
if(!((unsigned)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp259=({struct Cyc_String_pa_PrintArg_struct _tmp5AD;_tmp5AD.tag=0U,_tmp5AD.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5AD;});void*_tmp257[1U];_tmp257[0]=& _tmp259;({unsigned _tmp768=loc;struct _fat_ptr _tmp767=({const char*_tmp258="no field of struct/union has name %s";_tag_fat(_tmp258,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp768,_tmp767,_tag_fat(_tmp257,sizeof(void*),1U));});});else{
# 1270
t=t2;}
bad_type=0;
goto _LL5;}}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp255=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp254)->f2;_LL9: {struct Cyc_List_List*fields=_tmp255;
# 1274
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp25C=({struct Cyc_String_pa_PrintArg_struct _tmp5AE;_tmp5AE.tag=0U,_tmp5AE.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5AE;});void*_tmp25A[1U];_tmp25A[0]=& _tmp25C;({unsigned _tmp76A=loc;struct _fat_ptr _tmp769=({const char*_tmp25B="no field of struct/union has name %s";_tag_fat(_tmp25B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp76A,_tmp769,_tag_fat(_tmp25A,sizeof(void*),1U));});});else{
# 1278
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1283
if(bad_type){
if(l == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp25F=({struct Cyc_String_pa_PrintArg_struct _tmp5AF;_tmp5AF.tag=0U,({struct _fat_ptr _tmp76B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5AF.f1=_tmp76B;});_tmp5AF;});void*_tmp25D[1U];_tmp25D[0]=& _tmp25F;({unsigned _tmp76D=loc;struct _fat_ptr _tmp76C=({const char*_tmp25E="%s is not a known struct/union type";_tag_fat(_tmp25E,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp76D,_tmp76C,_tag_fat(_tmp25D,sizeof(void*),1U));});});else{
# 1287
struct _fat_ptr _tmp260=({struct Cyc_String_pa_PrintArg_struct _tmp273=({struct Cyc_String_pa_PrintArg_struct _tmp5B6;_tmp5B6.tag=0U,({struct _fat_ptr _tmp76E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp5B6.f1=_tmp76E;});_tmp5B6;});void*_tmp271[1U];_tmp271[0]=& _tmp273;({struct _fat_ptr _tmp76F=({const char*_tmp272="(%s)";_tag_fat(_tmp272,sizeof(char),5U);});Cyc_aprintf(_tmp76F,_tag_fat(_tmp271,sizeof(void*),1U));});});struct _fat_ptr s=_tmp260;
struct Cyc_List_List*x;
for(x=fs;x != l;x=x->tl){
void*_tmp261=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp28=_tmp261;void*_tmp262=_stmttmp28;unsigned _tmp263;struct _fat_ptr*_tmp264;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp262)->tag == 0U){_LLD: _tmp264=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp262)->f1;_LLE: {struct _fat_ptr*n=_tmp264;
# 1292
s=({struct Cyc_String_pa_PrintArg_struct _tmp267=({struct Cyc_String_pa_PrintArg_struct _tmp5B1;_tmp5B1.tag=0U,_tmp5B1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5B1;});struct Cyc_String_pa_PrintArg_struct _tmp268=({struct Cyc_String_pa_PrintArg_struct _tmp5B0;_tmp5B0.tag=0U,_tmp5B0.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5B0;});void*_tmp265[2U];_tmp265[0]=& _tmp267,_tmp265[1]=& _tmp268;({struct _fat_ptr _tmp770=({const char*_tmp266="%s.%s";_tag_fat(_tmp266,sizeof(char),6U);});Cyc_aprintf(_tmp770,_tag_fat(_tmp265,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp263=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp262)->f1;_LL10: {unsigned n=_tmp263;
# 1294
s=({struct Cyc_String_pa_PrintArg_struct _tmp26B=({struct Cyc_String_pa_PrintArg_struct _tmp5B3;_tmp5B3.tag=0U,_tmp5B3.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5B3;});struct Cyc_Int_pa_PrintArg_struct _tmp26C=({struct Cyc_Int_pa_PrintArg_struct _tmp5B2;_tmp5B2.tag=1U,_tmp5B2.f1=(unsigned long)((int)n);_tmp5B2;});void*_tmp269[2U];_tmp269[0]=& _tmp26B,_tmp269[1]=& _tmp26C;({struct _fat_ptr _tmp771=({const char*_tmp26A="%s.%d";_tag_fat(_tmp26A,sizeof(char),6U);});Cyc_aprintf(_tmp771,_tag_fat(_tmp269,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}
# 1296
({struct Cyc_String_pa_PrintArg_struct _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp5B5;_tmp5B5.tag=0U,_tmp5B5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5B5;});struct Cyc_String_pa_PrintArg_struct _tmp270=({struct Cyc_String_pa_PrintArg_struct _tmp5B4;_tmp5B4.tag=0U,({struct _fat_ptr _tmp772=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5B4.f1=_tmp772;});_tmp5B4;});void*_tmp26D[2U];_tmp26D[0]=& _tmp26F,_tmp26D[1]=& _tmp270;({unsigned _tmp774=loc;struct _fat_ptr _tmp773=({const char*_tmp26E="%s == %s is not a struct/union type";_tag_fat(_tmp26E,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp774,_tmp773,_tag_fat(_tmp26D,sizeof(void*),2U));});});}}
# 1299
goto _LL0;}}else{_LL3: _tmp251=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp250)->f1;_LL4: {unsigned n=_tmp251;
# 1301
int bad_type=1;
{void*_tmp274=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp274;void*_tmp275=_stmttmp29;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_Absyn_Datatypefield*_tmp278;struct Cyc_Absyn_Aggrdecl**_tmp279;switch(*((int*)_tmp275)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp279=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp279;
# 1304
if((*adp)->impl == 0)
goto _LL11;
_tmp277=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp278=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp278;
# 1324
if(({unsigned _tmp775=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp775 < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp284=({struct Cyc_Int_pa_PrintArg_struct _tmp5B8;_tmp5B8.tag=1U,({
unsigned long _tmp776=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp5B8.f1=_tmp776;});_tmp5B8;});struct Cyc_Int_pa_PrintArg_struct _tmp285=({struct Cyc_Int_pa_PrintArg_struct _tmp5B7;_tmp5B7.tag=1U,_tmp5B7.f1=(unsigned long)((int)n);_tmp5B7;});void*_tmp282[2U];_tmp282[0]=& _tmp284,_tmp282[1]=& _tmp285;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp283="datatype field has too few components: %d < %d";_tag_fat(_tmp283,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp778,_tmp777,_tag_fat(_tmp282,sizeof(void*),2U));});});else{
# 1328
if(n != (unsigned)0)
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp286=0U;({unsigned _tmp77A=loc;struct _fat_ptr _tmp779=({const char*_tmp287="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp287,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_fat(_tmp286,sizeof(void*),0U));});});}}
# 1333
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp277=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp275)->f2;_LL15: {struct Cyc_List_List*fields=_tmp277;
# 1308
if(({unsigned _tmp77B=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp77B <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp27C=({struct Cyc_Int_pa_PrintArg_struct _tmp5BA;_tmp5BA.tag=1U,({
unsigned long _tmp77C=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp5BA.f1=_tmp77C;});_tmp5BA;});struct Cyc_Int_pa_PrintArg_struct _tmp27D=({struct Cyc_Int_pa_PrintArg_struct _tmp5B9;_tmp5B9.tag=1U,_tmp5B9.f1=(unsigned long)((int)n);_tmp5B9;});void*_tmp27A[2U];_tmp27A[0]=& _tmp27C,_tmp27A[1]=& _tmp27D;({unsigned _tmp77E=loc;struct _fat_ptr _tmp77D=({const char*_tmp27B="struct/union has too few components: %d <= %d";_tag_fat(_tmp27B,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp77E,_tmp77D,_tag_fat(_tmp27A,sizeof(void*),2U));});});else{
# 1312
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp276=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp275)->f1;_LL17: {struct Cyc_List_List*l=_tmp276;
# 1316
if(({unsigned _tmp77F=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp77F <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp280=({struct Cyc_Int_pa_PrintArg_struct _tmp5BC;_tmp5BC.tag=1U,({
unsigned long _tmp780=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp5BC.f1=_tmp780;});_tmp5BC;});struct Cyc_Int_pa_PrintArg_struct _tmp281=({struct Cyc_Int_pa_PrintArg_struct _tmp5BB;_tmp5BB.tag=1U,_tmp5BB.f1=(unsigned long)((int)n);_tmp5BB;});void*_tmp27E[2U];_tmp27E[0]=& _tmp280,_tmp27E[1]=& _tmp281;({unsigned _tmp782=loc;struct _fat_ptr _tmp781=({const char*_tmp27F="tuple has too few components: %d <= %d";_tag_fat(_tmp27F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp782,_tmp781,_tag_fat(_tmp27E,sizeof(void*),2U));});});else{
# 1320
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1335
 goto _LL11;}_LL11:;}
# 1337
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp28A=({struct Cyc_String_pa_PrintArg_struct _tmp5BD;_tmp5BD.tag=0U,({struct _fat_ptr _tmp783=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5BD.f1=_tmp783;});_tmp5BD;});void*_tmp288[1U];_tmp288[0]=& _tmp28A;({unsigned _tmp785=loc;struct _fat_ptr _tmp784=({const char*_tmp289="%s is not a known type";_tag_fat(_tmp289,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp785,_tmp784,_tag_fat(_tmp288,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1342
return Cyc_Absyn_uint_type;}}
# 1346
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp28B=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp28B;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp28C=t;void*_tmp290;void*_tmp28F;void*_tmp28E;void*_tmp28D;switch(*((int*)_tmp28C)){case 1U: _LL1: _LL2: {
# 1352
struct Cyc_List_List*_tmp291=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp291;
void*_tmp292=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A->v=tenv_tvs;_tmp29A;}));void*t2=_tmp292;
void*_tmp293=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->v=tenv_tvs;_tmp299;}));void*rt=_tmp293;
void*_tmp294=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp294;
void*_tmp295=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp295;
struct Cyc_Absyn_PtrAtts _tmp296=({struct Cyc_Absyn_PtrAtts _tmp5BE;_tmp5BE.rgn=rt,({void*_tmp786=Cyc_Tcutil_any_bool(tenv_tvs);_tmp5BE.nullable=_tmp786;}),_tmp5BE.bounds=b,_tmp5BE.zero_term=zt,_tmp5BE.ptrloc=0;_tmp5BE;});struct Cyc_Absyn_PtrAtts atts=_tmp296;
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp297=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->tag=3U,(_tmp298->f1).elt_type=t2,({struct Cyc_Absyn_Tqual _tmp787=Cyc_Absyn_empty_tqual(0U);(_tmp298->f1).elt_tq=_tmp787;}),(_tmp298->f1).ptr_atts=atts;_tmp298;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_typ=_tmp297;
Cyc_Unify_unify(t,(void*)new_typ);
_tmp28D=t2;_tmp28E=rt;_tmp28F=b;_tmp290=zt;goto _LL4;}case 3U: _LL3: _tmp28D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).elt_type;_tmp28E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).rgn;_tmp28F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).bounds;_tmp290=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).zero_term;_LL4: {void*t2=_tmp28D;void*rt=_tmp28E;void*b=_tmp28F;void*zt=_tmp290;
# 1362
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp29B=Cyc_Tcutil_compress(t2);void*_stmttmp2A=_tmp29B;void*_tmp29C=_stmttmp2A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp29C)->tag == 5U){_LL8: _LL9:
# 1367
 if(Cyc_Tc_aggressive_warn)
({void*_tmp29D=0U;({unsigned _tmp789=loc;struct _fat_ptr _tmp788=({const char*_tmp29E="unnecessary dereference for function type";_tag_fat(_tmp29E,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp789,_tmp788,_tag_fat(_tmp29D,sizeof(void*),0U));});});
return t;}else{_LLA: _LLB:
# 1371
({void*_tmp29F=0U;({unsigned _tmp78B=loc;struct _fat_ptr _tmp78A=({const char*_tmp2A0="can't dereference abstract pointer type";_tag_fat(_tmp2A0,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp78B,_tmp78A,_tag_fat(_tmp29F,sizeof(void*),0U));});});}_LL7:;}
# 1374
return t2;}default: _LL5: _LL6:
# 1376
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C0;_tmp5C0.tag=0U,({struct _fat_ptr _tmp78C=({const char*_tmp2A4="expecting *, @, or ? type but found ";_tag_fat(_tmp2A4,sizeof(char),37U);});_tmp5C0.f1=_tmp78C;});_tmp5C0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BF;_tmp5BF.tag=2U,_tmp5BF.f1=(void*)t;_tmp5BF;});void*_tmp2A1[2U];_tmp2A1[0]=& _tmp2A2,_tmp2A1[1]=& _tmp2A3;({struct Cyc_Tcenv_Tenv*_tmp78F=te;unsigned _tmp78E=loc;void**_tmp78D=topt;Cyc_Tcexp_expr_err(_tmp78F,_tmp78E,_tmp78D,_tag_fat(_tmp2A1,sizeof(void*),2U));});});}_LL0:;}}
# 1381
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1385
({struct Cyc_Tcenv_Tenv*_tmp790=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp790,0,e);});
# 1387
({int _tmp791=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp791;});{
void*_tmp2A5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp2B=_tmp2A5;void*_tmp2A6=_stmttmp2B;struct Cyc_List_List*_tmp2A8;enum Cyc_Absyn_AggrKind _tmp2A7;struct Cyc_List_List*_tmp2AA;struct Cyc_Absyn_Aggrdecl*_tmp2A9;switch(*((int*)_tmp2A6)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A6)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A6)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp2A9=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A6)->f1)->f1).KnownAggr).val;_tmp2AA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A6)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2A9;struct Cyc_List_List*ts=_tmp2AA;
# 1390
struct Cyc_Absyn_Aggrfield*_tmp2AB=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2AB;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2AD=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5C4;_tmp5C4.tag=5U,_tmp5C4.f1=ad;_tmp5C4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3;_tmp5C3.tag=0U,({struct _fat_ptr _tmp792=({const char*_tmp2B2=" has no ";_tag_fat(_tmp2B2,sizeof(char),9U);});_tmp5C3.f1=_tmp792;});_tmp5C3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C2;_tmp5C2.tag=0U,_tmp5C2.f1=*f;_tmp5C2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C1;_tmp5C1.tag=0U,({struct _fat_ptr _tmp793=({const char*_tmp2B1=" member";_tag_fat(_tmp2B1,sizeof(char),8U);});_tmp5C1.f1=_tmp793;});_tmp5C1;});void*_tmp2AC[4U];_tmp2AC[0]=& _tmp2AD,_tmp2AC[1]=& _tmp2AE,_tmp2AC[2]=& _tmp2AF,_tmp2AC[3]=& _tmp2B0;({struct Cyc_Tcenv_Tenv*_tmp796=te;unsigned _tmp795=loc;void**_tmp794=topt;Cyc_Tcexp_expr_err(_tmp796,_tmp795,_tmp794,_tag_fat(_tmp2AC,sizeof(void*),4U));});});
# 1394
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp797=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp797,finfo->type);});
# 1400
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C7;_tmp5C7.tag=0U,({struct _fat_ptr _tmp798=({const char*_tmp2B8="cannot read union member ";_tag_fat(_tmp2B8,sizeof(char),26U);});_tmp5C7.f1=_tmp798;});_tmp5C7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C6;_tmp5C6.tag=0U,_tmp5C6.f1=*f;_tmp5C6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C5;_tmp5C5.tag=0U,({
struct _fat_ptr _tmp799=({const char*_tmp2B7=" since it is not `bits-only'";_tag_fat(_tmp2B7,sizeof(char),29U);});_tmp5C5.f1=_tmp799;});_tmp5C5;});void*_tmp2B3[3U];_tmp2B3[0]=& _tmp2B4,_tmp2B3[1]=& _tmp2B5,_tmp2B3[2]=& _tmp2B6;({struct Cyc_Tcenv_Tenv*_tmp79C=te;unsigned _tmp79B=loc;void**_tmp79A=topt;Cyc_Tcexp_expr_err(_tmp79C,_tmp79B,_tmp79A,_tag_fat(_tmp2B3,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1406
if(!({void*_tmp79D=t2;Cyc_Unify_unify(_tmp79D,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=0U,({struct _fat_ptr _tmp79E=({const char*_tmp2BE="must use pattern-matching to access field ";_tag_fat(_tmp2BE,sizeof(char),43U);});_tmp5CA.f1=_tmp79E;});_tmp5CA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=0U,_tmp5C9.f1=*f;_tmp5C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C8;_tmp5C8.tag=0U,({struct _fat_ptr _tmp79F=({const char*_tmp2BD="\n\tdue to existential type variables.";_tag_fat(_tmp2BD,sizeof(char),37U);});_tmp5C8.f1=_tmp79F;});_tmp5C8;});void*_tmp2B9[3U];_tmp2B9[0]=& _tmp2BA,_tmp2B9[1]=& _tmp2BB,_tmp2B9[2]=& _tmp2BC;({struct Cyc_Tcenv_Tenv*_tmp7A2=te;unsigned _tmp7A1=loc;void**_tmp7A0=topt;Cyc_Tcexp_expr_err(_tmp7A2,_tmp7A1,_tmp7A0,_tag_fat(_tmp2B9,sizeof(void*),3U));});});}
# 1409
return t2;}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp2A7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A6)->f1;_tmp2A8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A6)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp2A7;struct Cyc_List_List*fs=_tmp2A8;
# 1411
struct Cyc_Absyn_Aggrfield*_tmp2BF=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2BF;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CD;_tmp5CD.tag=0U,({struct _fat_ptr _tmp7A3=({const char*_tmp2C5="type has no ";_tag_fat(_tmp2C5,sizeof(char),13U);});_tmp5CD.f1=_tmp7A3;});_tmp5CD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CC;_tmp5CC.tag=0U,_tmp5CC.f1=*f;_tmp5CC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=0U,({struct _fat_ptr _tmp7A4=({const char*_tmp2C4=" member";_tag_fat(_tmp2C4,sizeof(char),8U);});_tmp5CB.f1=_tmp7A4;});_tmp5CB;});void*_tmp2C0[3U];_tmp2C0[0]=& _tmp2C1,_tmp2C0[1]=& _tmp2C2,_tmp2C0[2]=& _tmp2C3;({struct Cyc_Tcenv_Tenv*_tmp7A7=te;unsigned _tmp7A6=loc;void**_tmp7A5=topt;Cyc_Tcexp_expr_err(_tmp7A7,_tmp7A6,_tmp7A5,_tag_fat(_tmp2C0,sizeof(void*),3U));});});
# 1416
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1418
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D0;_tmp5D0.tag=0U,({struct _fat_ptr _tmp7A8=({const char*_tmp2CB="cannot read union member ";_tag_fat(_tmp2CB,sizeof(char),26U);});_tmp5D0.f1=_tmp7A8;});_tmp5D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CF;_tmp5CF.tag=0U,_tmp5CF.f1=*f;_tmp5CF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CE;_tmp5CE.tag=0U,({struct _fat_ptr _tmp7A9=({const char*_tmp2CA=" since it is not `bits-only'";_tag_fat(_tmp2CA,sizeof(char),29U);});_tmp5CE.f1=_tmp7A9;});_tmp5CE;});void*_tmp2C6[3U];_tmp2C6[0]=& _tmp2C7,_tmp2C6[1]=& _tmp2C8,_tmp2C6[2]=& _tmp2C9;({struct Cyc_Tcenv_Tenv*_tmp7AC=te;unsigned _tmp7AB=loc;void**_tmp7AA=topt;Cyc_Tcexp_expr_err(_tmp7AC,_tmp7AB,_tmp7AA,_tag_fat(_tmp2C6,sizeof(void*),3U));});});
return finfo->type;}default: _LL5: _LL6:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D2;_tmp5D2.tag=0U,({struct _fat_ptr _tmp7AD=({const char*_tmp2CF="expecting struct or union, found ";_tag_fat(_tmp2CF,sizeof(char),34U);});_tmp5D2.f1=_tmp7AD;});_tmp5D2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2CE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D1;_tmp5D1.tag=2U,_tmp5D1.f1=(void*)_check_null(e->topt);_tmp5D1;});void*_tmp2CC[2U];_tmp2CC[0]=& _tmp2CD,_tmp2CC[1]=& _tmp2CE;({struct Cyc_Tcenv_Tenv*_tmp7B0=te;unsigned _tmp7AF=loc;void**_tmp7AE=topt;Cyc_Tcexp_expr_err(_tmp7B0,_tmp7AF,_tmp7AE,_tag_fat(_tmp2CC,sizeof(void*),2U));});});}_LL0:;}}
# 1426
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1429
({struct Cyc_Tcenv_Tenv*_tmp7B1=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp7B1,0,e);});
# 1431
({int _tmp7B2=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp7B2;});
{void*_tmp2D0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp2C=_tmp2D0;void*_tmp2D1=_stmttmp2C;void*_tmp2D5;void*_tmp2D4;void*_tmp2D3;void*_tmp2D2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->tag == 3U){_LL1: _tmp2D2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).elt_type;_tmp2D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).rgn;_tmp2D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).bounds;_tmp2D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).zero_term;_LL2: {void*t2=_tmp2D2;void*rt=_tmp2D3;void*b=_tmp2D4;void*zt=_tmp2D5;
# 1434
Cyc_Tcutil_check_nonzero_bound(loc,b);
{void*_tmp2D6=Cyc_Tcutil_compress(t2);void*_stmttmp2D=_tmp2D6;void*_tmp2D7=_stmttmp2D;struct Cyc_List_List*_tmp2D9;enum Cyc_Absyn_AggrKind _tmp2D8;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Aggrdecl*_tmp2DA;switch(*((int*)_tmp2D7)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D7)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D7)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp2DA=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D7)->f1)->f1).KnownAggr).val;_tmp2DB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D7)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2DA;struct Cyc_List_List*ts=_tmp2DB;
# 1437
struct Cyc_Absyn_Aggrfield*_tmp2DC=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2DC;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D7;_tmp5D7.tag=0U,({struct _fat_ptr _tmp7B3=({const char*_tmp2E5="type ";_tag_fat(_tmp2E5,sizeof(char),6U);});_tmp5D7.f1=_tmp7B3;});_tmp5D7;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2DF=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5D6;_tmp5D6.tag=5U,_tmp5D6.f1=ad;_tmp5D6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D5;_tmp5D5.tag=0U,({struct _fat_ptr _tmp7B4=({const char*_tmp2E4=" has no ";_tag_fat(_tmp2E4,sizeof(char),9U);});_tmp5D5.f1=_tmp7B4;});_tmp5D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=0U,_tmp5D4.f1=*f;_tmp5D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=0U,({struct _fat_ptr _tmp7B5=({const char*_tmp2E3=" member";_tag_fat(_tmp2E3,sizeof(char),8U);});_tmp5D3.f1=_tmp7B5;});_tmp5D3;});void*_tmp2DD[5U];_tmp2DD[0]=& _tmp2DE,_tmp2DD[1]=& _tmp2DF,_tmp2DD[2]=& _tmp2E0,_tmp2DD[3]=& _tmp2E1,_tmp2DD[4]=& _tmp2E2;({struct Cyc_Tcenv_Tenv*_tmp7B8=te;unsigned _tmp7B7=loc;void**_tmp7B6=topt;Cyc_Tcexp_expr_err(_tmp7B8,_tmp7B7,_tmp7B6,_tag_fat(_tmp2DD,sizeof(void*),5U));});});
# 1441
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t3=finfo->type;
if(ts != 0){
struct Cyc_List_List*_tmp2E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);struct Cyc_List_List*inst=_tmp2E6;
t3=Cyc_Tcutil_substitute(inst,finfo->type);}{
# 1447
struct Cyc_Absyn_Kind*_tmp2E7=Cyc_Tcutil_type_kind(t3);struct Cyc_Absyn_Kind*t3_kind=_tmp2E7;
# 1450
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,t3_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2E8=Cyc_Tcutil_compress(t3);void*_stmttmp2E=_tmp2E8;void*_tmp2E9=_stmttmp2E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2E9)->tag == 4U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DA;_tmp5DA.tag=0U,({struct _fat_ptr _tmp7B9=({const char*_tmp2EF="cannot get member ";_tag_fat(_tmp2EF,sizeof(char),19U);});_tmp5DA.f1=_tmp7B9;});_tmp5DA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D9;_tmp5D9.tag=0U,_tmp5D9.f1=*f;_tmp5D9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D8;_tmp5D8.tag=0U,({
struct _fat_ptr _tmp7BA=({const char*_tmp2EE=" since its type is abstract";_tag_fat(_tmp2EE,sizeof(char),28U);});_tmp5D8.f1=_tmp7BA;});_tmp5D8;});void*_tmp2EA[3U];_tmp2EA[0]=& _tmp2EB,_tmp2EA[1]=& _tmp2EC,_tmp2EA[2]=& _tmp2ED;({struct Cyc_Tcenv_Tenv*_tmp7BD=te;unsigned _tmp7BC=loc;void**_tmp7BB=topt;Cyc_Tcexp_expr_err(_tmp7BD,_tmp7BC,_tmp7BB,_tag_fat(_tmp2EA,sizeof(void*),3U));});});}_LLC:;}
# 1459
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !
Cyc_Tcutil_is_bits_only_type(t3))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1462
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DD;_tmp5DD.tag=0U,({struct _fat_ptr _tmp7BE=({const char*_tmp2F5="cannot read union member ";_tag_fat(_tmp2F5,sizeof(char),26U);});_tmp5DD.f1=_tmp7BE;});_tmp5DD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=0U,_tmp5DC.f1=*f;_tmp5DC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DB;_tmp5DB.tag=0U,({
struct _fat_ptr _tmp7BF=({const char*_tmp2F4=" since it is not `bits-only'";_tag_fat(_tmp2F4,sizeof(char),29U);});_tmp5DB.f1=_tmp7BF;});_tmp5DB;});void*_tmp2F0[3U];_tmp2F0[0]=& _tmp2F1,_tmp2F0[1]=& _tmp2F2,_tmp2F0[2]=& _tmp2F3;({struct Cyc_Tcenv_Tenv*_tmp7C2=te;unsigned _tmp7C1=loc;void**_tmp7C0=topt;Cyc_Tcexp_expr_err(_tmp7C2,_tmp7C1,_tmp7C0,_tag_fat(_tmp2F0,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
if(!({void*_tmp7C3=t3;Cyc_Unify_unify(_tmp7C3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E0;_tmp5E0.tag=0U,({
struct _fat_ptr _tmp7C4=({const char*_tmp2FB="must use pattern-matching to access field ";_tag_fat(_tmp2FB,sizeof(char),43U);});_tmp5E0.f1=_tmp7C4;});_tmp5E0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DF;_tmp5DF.tag=0U,_tmp5DF.f1=*f;_tmp5DF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DE;_tmp5DE.tag=0U,({
struct _fat_ptr _tmp7C5=({const char*_tmp2FA="\n\tdue to extistential types";_tag_fat(_tmp2FA,sizeof(char),28U);});_tmp5DE.f1=_tmp7C5;});_tmp5DE;});void*_tmp2F6[3U];_tmp2F6[0]=& _tmp2F7,_tmp2F6[1]=& _tmp2F8,_tmp2F6[2]=& _tmp2F9;({struct Cyc_Tcenv_Tenv*_tmp7C8=te;unsigned _tmp7C7=loc;void**_tmp7C6=topt;Cyc_Tcexp_expr_err(_tmp7C8,_tmp7C7,_tmp7C6,_tag_fat(_tmp2F6,sizeof(void*),3U));});});}
# 1470
return t3;}}}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp2D8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D7)->f1;_tmp2D9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D7)->f2;_LL9: {enum Cyc_Absyn_AggrKind k=_tmp2D8;struct Cyc_List_List*fs=_tmp2D9;
# 1472
struct Cyc_Absyn_Aggrfield*_tmp2FC=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2FC;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E3;_tmp5E3.tag=0U,({struct _fat_ptr _tmp7C9=({const char*_tmp302="type has no ";_tag_fat(_tmp302,sizeof(char),13U);});_tmp5E3.f1=_tmp7C9;});_tmp5E3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E2;_tmp5E2.tag=0U,_tmp5E2.f1=*f;_tmp5E2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp300=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E1;_tmp5E1.tag=0U,({struct _fat_ptr _tmp7CA=({const char*_tmp301=" field";_tag_fat(_tmp301,sizeof(char),7U);});_tmp5E1.f1=_tmp7CA;});_tmp5E1;});void*_tmp2FD[3U];_tmp2FD[0]=& _tmp2FE,_tmp2FD[1]=& _tmp2FF,_tmp2FD[2]=& _tmp300;({struct Cyc_Tcenv_Tenv*_tmp7CD=te;unsigned _tmp7CC=loc;void**_tmp7CB=topt;Cyc_Tcexp_expr_err(_tmp7CD,_tmp7CC,_tmp7CB,_tag_fat(_tmp2FD,sizeof(void*),3U));});});
# 1477
if(((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp304=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=0U,({struct _fat_ptr _tmp7CE=({const char*_tmp308="cannot read union member ";_tag_fat(_tmp308,sizeof(char),26U);});_tmp5E6.f1=_tmp7CE;});_tmp5E6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp305=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=0U,_tmp5E5.f1=*f;_tmp5E5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp306=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=0U,({
struct _fat_ptr _tmp7CF=({const char*_tmp307=" since it is not `bits-only'";_tag_fat(_tmp307,sizeof(char),29U);});_tmp5E4.f1=_tmp7CF;});_tmp5E4;});void*_tmp303[3U];_tmp303[0]=& _tmp304,_tmp303[1]=& _tmp305,_tmp303[2]=& _tmp306;({struct Cyc_Tcenv_Tenv*_tmp7D2=te;unsigned _tmp7D1=loc;void**_tmp7D0=topt;Cyc_Tcexp_expr_err(_tmp7D2,_tmp7D1,_tmp7D0,_tag_fat(_tmp303,sizeof(void*),3U));});});
return finfo->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1483
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1486
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp30A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=0U,({struct _fat_ptr _tmp7D3=({const char*_tmp30C="expecting struct or union pointer, found ";_tag_fat(_tmp30C,sizeof(char),42U);});_tmp5E8.f1=_tmp7D3;});_tmp5E8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp30B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=2U,_tmp5E7.f1=(void*)_check_null(e->topt);_tmp5E7;});void*_tmp309[2U];_tmp309[0]=& _tmp30A,_tmp309[1]=& _tmp30B;({struct Cyc_Tcenv_Tenv*_tmp7D6=te;unsigned _tmp7D5=loc;void**_tmp7D4=topt;Cyc_Tcexp_expr_err(_tmp7D6,_tmp7D5,_tmp7D4,_tag_fat(_tmp309,sizeof(void*),2U));});});}
# 1491
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1493
struct _tuple16 _tmp30D=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple16 _stmttmp2F=_tmp30D;struct _tuple16 _tmp30E=_stmttmp2F;int _tmp310;unsigned _tmp30F;_LL1: _tmp30F=_tmp30E.f1;_tmp310=_tmp30E.f2;_LL2: {unsigned i=_tmp30F;int known=_tmp310;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp312=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=0U,({
struct _fat_ptr _tmp7D7=({const char*_tmp313="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp313,sizeof(char),47U);});_tmp5E9.f1=_tmp7D7;});_tmp5E9;});void*_tmp311[1U];_tmp311[0]=& _tmp312;({struct Cyc_Tcenv_Tenv*_tmp7D9=te;unsigned _tmp7D8=loc;Cyc_Tcexp_expr_err(_tmp7D9,_tmp7D8,0,_tag_fat(_tmp311,sizeof(void*),1U));});});{
struct _handler_cons _tmp314;_push_handler(& _tmp314);{int _tmp316=0;if(setjmp(_tmp314.handler))_tmp316=1;if(!_tmp316){
{void*_tmp317=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp317;};_pop_handler();}else{void*_tmp315=(void*)Cyc_Core_get_exn_thrown();void*_tmp318=_tmp315;void*_tmp319;if(((struct Cyc_List_Nth_exn_struct*)_tmp318)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1500
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp31B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=0U,({struct _fat_ptr _tmp7DA=({const char*_tmp322="index is ";_tag_fat(_tmp322,sizeof(char),10U);});_tmp5EE.f1=_tmp7DA;});_tmp5EE;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp31C=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5ED;_tmp5ED.tag=11U,_tmp5ED.f1=(int)i;_tmp5ED;});struct Cyc_Warn_String_Warn_Warg_struct _tmp31D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=0U,({struct _fat_ptr _tmp7DB=({const char*_tmp321=" but tuple has only ";_tag_fat(_tmp321,sizeof(char),21U);});_tmp5EC.f1=_tmp7DB;});_tmp5EC;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp31E=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=11U,({
int _tmp7DC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp5EB.f1=_tmp7DC;});_tmp5EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp31F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=0U,({struct _fat_ptr _tmp7DD=({const char*_tmp320=" fields";_tag_fat(_tmp320,sizeof(char),8U);});_tmp5EA.f1=_tmp7DD;});_tmp5EA;});void*_tmp31A[5U];_tmp31A[0]=& _tmp31B,_tmp31A[1]=& _tmp31C,_tmp31A[2]=& _tmp31D,_tmp31A[3]=& _tmp31E,_tmp31A[4]=& _tmp31F;({struct Cyc_Tcenv_Tenv*_tmp7DF=te;unsigned _tmp7DE=loc;Cyc_Tcexp_expr_err(_tmp7DF,_tmp7DE,0,_tag_fat(_tmp31A,sizeof(void*),5U));});});}else{_LL6: _tmp319=_tmp318;_LL7: {void*exn=_tmp319;(int)_rethrow(exn);}}_LL3:;}}}}}
# 1505
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1507
struct Cyc_Tcenv_Tenv*_tmp323=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp323;
({struct Cyc_Tcenv_Tenv*_tmp7E0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7E0,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp7E1=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7E1,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp325=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0U,({struct _fat_ptr _tmp7E2=({const char*_tmp327="expecting int subscript, found ";_tag_fat(_tmp327,sizeof(char),32U);});_tmp5F0.f1=_tmp7E2;});_tmp5F0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp326=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=2U,_tmp5EF.f1=(void*)t2;_tmp5EF;});void*_tmp324[2U];_tmp324[0]=& _tmp325,_tmp324[1]=& _tmp326;({struct Cyc_Tcenv_Tenv*_tmp7E5=te;unsigned _tmp7E4=e2->loc;void**_tmp7E3=topt;Cyc_Tcexp_expr_err(_tmp7E5,_tmp7E4,_tmp7E3,_tag_fat(_tmp324,sizeof(void*),2U));});});{
# 1516
void*_tmp328=t1;struct Cyc_List_List*_tmp329;void*_tmp32E;void*_tmp32D;void*_tmp32C;struct Cyc_Absyn_Tqual _tmp32B;void*_tmp32A;switch(*((int*)_tmp328)){case 3U: _LL1: _tmp32A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).elt_type;_tmp32B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).elt_tq;_tmp32C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).ptr_atts).rgn;_tmp32D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).ptr_atts).bounds;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp32A;struct Cyc_Absyn_Tqual tq=_tmp32B;void*rt=_tmp32C;void*b=_tmp32D;void*zt=_tmp32E;
# 1520
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp32F=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp32F;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp330=eopt;struct Cyc_Absyn_Exp*e3=_tmp330;
struct _tuple16 _tmp331=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp30=_tmp331;struct _tuple16 _tmp332=_stmttmp30;int _tmp334;unsigned _tmp333;_LL8: _tmp333=_tmp332.f1;_tmp334=_tmp332.f2;_LL9: {unsigned i=_tmp333;int known_i=_tmp334;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp335=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp31=_tmp335;struct _tuple16 _tmp336=_stmttmp31;int _tmp338;unsigned _tmp337;_LLB: _tmp337=_tmp336.f1;_tmp338=_tmp336.f2;_LLC: {unsigned j=_tmp337;int known_j=_tmp338;
if(known_j){
struct _tuple16 _tmp339=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp32=_tmp339;struct _tuple16 _tmp33A=_stmttmp32;int _tmp33C;unsigned _tmp33B;_LLE: _tmp33B=_tmp33A.f1;_tmp33C=_tmp33A.f2;_LLF: {unsigned j=_tmp33B;int knownj=_tmp33C;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp33D=0U;({unsigned _tmp7E7=loc;struct _fat_ptr _tmp7E6=({const char*_tmp33E="subscript will fail at run-time";_tag_fat(_tmp33E,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7E7,_tmp7E6,_tag_fat(_tmp33D,sizeof(void*),0U));});});}}}}}}
# 1536
if(emit_warning)
({void*_tmp33F=0U;({unsigned _tmp7E9=e2->loc;struct _fat_ptr _tmp7E8=({const char*_tmp340="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp340,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp7E9,_tmp7E8,_tag_fat(_tmp33F,sizeof(void*),0U));});});}else{
# 1540
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp341=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp33=_tmp341;struct _tuple16 _tmp342=_stmttmp33;int _tmp344;unsigned _tmp343;_LL11: _tmp343=_tmp342.f1;_tmp344=_tmp342.f2;_LL12: {unsigned i=_tmp343;int known=_tmp344;
if(known)
# 1545
({unsigned _tmp7EC=loc;unsigned _tmp7EB=i;void*_tmp7EA=b;Cyc_Tcutil_check_bound(_tmp7EC,_tmp7EB,_tmp7EA,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1548
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp345=0U;({unsigned _tmp7EE=e1->loc;struct _fat_ptr _tmp7ED=({const char*_tmp346="subscript applied to pointer to one object";_tag_fat(_tmp346,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp7EE,_tmp7ED,_tag_fat(_tmp345,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1553
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp347=0U;({unsigned _tmp7F0=e1->loc;struct _fat_ptr _tmp7EF=({const char*_tmp348="can't subscript an abstract pointer";_tag_fat(_tmp348,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp7F0,_tmp7EF,_tag_fat(_tmp347,sizeof(void*),0U));});});
return t;}case 6U: _LL3: _tmp329=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp328)->f1;_LL4: {struct Cyc_List_List*ts=_tmp329;
return Cyc_Tcexp_ithTupleType(te,loc,ts,e2);}default: _LL5: _LL6:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp34A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0U,({struct _fat_ptr _tmp7F1=({const char*_tmp34C="subscript applied to ";_tag_fat(_tmp34C,sizeof(char),22U);});_tmp5F2.f1=_tmp7F1;});_tmp5F2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp34B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=2U,_tmp5F1.f1=(void*)t1;_tmp5F1;});void*_tmp349[2U];_tmp349[0]=& _tmp34A,_tmp349[1]=& _tmp34B;({struct Cyc_Tcenv_Tenv*_tmp7F4=te;unsigned _tmp7F3=loc;void**_tmp7F2=topt;Cyc_Tcexp_expr_err(_tmp7F4,_tmp7F3,_tmp7F2,_tag_fat(_tmp349,sizeof(void*),2U));});});}_LL0:;}}}
# 1563
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp34D=Cyc_Tcutil_compress(*topt);void*_stmttmp34=_tmp34D;void*_tmp34E=_stmttmp34;struct Cyc_List_List*_tmp34F;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp34E)->tag == 6U){_LL1: _tmp34F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp34E)->f1;_LL2: {struct Cyc_List_List*ts=_tmp34F;
# 1569
if(({int _tmp7F5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp7F5 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1572
goto _LL0;
# 1574
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp350=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp350;
({struct Cyc_Tcenv_Tenv*_tmp7F6=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7F6,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1579
({struct Cyc_RgnOrder_RgnPO*_tmp7F8=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7F7=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp7F8,_tmp7F7,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp352=_cycalloc(sizeof(*_tmp352));({struct _tuple17*_tmp7F9=({struct _tuple17*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp351->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp351;});_tmp352->hd=_tmp7F9;}),_tmp352->tl=fields;_tmp352;});}
# 1582
done=1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1586
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp7FA=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7FA,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));({struct _tuple17*_tmp7FC=({struct _tuple17*_tmp353=_cycalloc(sizeof(*_tmp353));({struct Cyc_Absyn_Tqual _tmp7FB=Cyc_Absyn_empty_tqual(0U);_tmp353->f1=_tmp7FB;}),_tmp353->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp353;});_tmp354->hd=_tmp7FC;}),_tmp354->tl=fields;_tmp354;});}
# 1591
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355->tag=6U,({struct Cyc_List_List*_tmp7FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp355->f1=_tmp7FD;});_tmp355;});}
# 1595
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1600
struct _tuple9*_tmp356=targ;void*_tmp357;_LL1: _tmp357=_tmp356->f3;_LL2: {void*t=_tmp357;
({unsigned _tmp801=loc;struct Cyc_Tcenv_Tenv*_tmp800=te;struct Cyc_List_List*_tmp7FF=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp7FE=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1601
Cyc_Tctyp_check_type(_tmp801,_tmp800,_tmp7FF,_tmp7FE,1,0,t);});
# 1603
({void*_tmp802=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358->tag=36U,_tmp358->f1=0,_tmp358->f2=des;_tmp358;});orig_exp->r=_tmp802;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1616 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1619
void*res_t2;
int _tmp359=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp359;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp35A=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370->tag=0U,({union Cyc_Absyn_Cnst _tmp803=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp370->f1=_tmp803;});_tmp370;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp35A;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1627
if(zero_term){
struct Cyc_Absyn_Exp*_tmp35B=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp35B;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp35C=0U;({unsigned _tmp805=e->loc;struct _fat_ptr _tmp804=({const char*_tmp35D="zero-terminated array doesn't end with zero.";_tag_fat(_tmp35D,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp805,_tmp804,_tag_fat(_tmp35C,sizeof(void*),0U));});});}
# 1632
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp808=res;struct Cyc_Absyn_Tqual _tmp807=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1633
struct Cyc_Absyn_Exp*_tmp806=sz_exp;Cyc_Absyn_array_type(_tmp808,_tmp807,_tmp806,
# 1635
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1637
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1640
if(!({struct Cyc_RgnOrder_RgnPO*_tmp80A=Cyc_Tcenv_curr_rgnpo(te);void*_tmp809=res;Cyc_Tcutil_coerce_list(_tmp80A,_tmp809,es);}))
# 1642
({struct Cyc_String_pa_PrintArg_struct _tmp360=({struct Cyc_String_pa_PrintArg_struct _tmp5F3;_tmp5F3.tag=0U,({
struct _fat_ptr _tmp80B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(res));_tmp5F3.f1=_tmp80B;});_tmp5F3;});void*_tmp35E[1U];_tmp35E[0]=& _tmp360;({unsigned _tmp80D=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp80C=({const char*_tmp35F="elements of array do not all have the same type (%s)";_tag_fat(_tmp35F,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp80D,_tmp80C,_tag_fat(_tmp35E,sizeof(void*),1U));});});
# 1645
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1650
void*_tmp361=(void*)ds->hd;void*_stmttmp35=_tmp361;void*_tmp362=_stmttmp35;struct Cyc_Absyn_Exp*_tmp363;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp362)->tag == 1U){_LL1: _LL2:
# 1652
({void*_tmp364=0U;({unsigned _tmp80F=loc;struct _fat_ptr _tmp80E=({const char*_tmp365="only array index designators are supported";_tag_fat(_tmp365,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp80F,_tmp80E,_tag_fat(_tmp364,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp363=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp362)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp363;
# 1655
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp366=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp36=_tmp366;struct _tuple16 _tmp367=_stmttmp36;int _tmp369;unsigned _tmp368;_LL6: _tmp368=_tmp367.f1;_tmp369=_tmp367.f2;_LL7: {unsigned i=_tmp368;int known=_tmp369;
if(!known)
({void*_tmp36A=0U;({unsigned _tmp811=e->loc;struct _fat_ptr _tmp810=({const char*_tmp36B="index designator cannot use sizeof or offsetof";_tag_fat(_tmp36B,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp811,_tmp810,_tag_fat(_tmp36A,sizeof(void*),0U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp36E=({struct Cyc_Int_pa_PrintArg_struct _tmp5F5;_tmp5F5.tag=1U,_tmp5F5.f1=(unsigned long)offset;_tmp5F5;});struct Cyc_Int_pa_PrintArg_struct _tmp36F=({struct Cyc_Int_pa_PrintArg_struct _tmp5F4;_tmp5F4.tag=1U,_tmp5F4.f1=(unsigned long)((int)i);_tmp5F4;});void*_tmp36C[2U];_tmp36C[0]=& _tmp36E,_tmp36C[1]=& _tmp36F;({unsigned _tmp813=e->loc;struct _fat_ptr _tmp812=({const char*_tmp36D="expecting index designator %d but found %d";_tag_fat(_tmp36D,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp813,_tmp812,_tag_fat(_tmp36C,sizeof(void*),2U));});});}
# 1662
goto _LL0;}}}}_LL0:;}}}
# 1666
return res_t2;}
# 1670
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1673
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp371=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp37=_tmp371;void*_tmp372=_stmttmp37;void*_tmp373;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->f2 != 0){_LL1: _tmp373=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->f2)->hd;_LL2: {void*t=_tmp373;
# 1678
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp374=({void*_tmp814=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp814,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp374;
b->topt=bound->topt;
# 1683
bound=b;}
# 1685
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1687
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_String_pa_PrintArg_struct _tmp377=({struct Cyc_String_pa_PrintArg_struct _tmp5F6;_tmp5F6.tag=0U,({
struct _fat_ptr _tmp815=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp5F6.f1=_tmp815;});_tmp5F6;});void*_tmp375[1U];_tmp375[0]=& _tmp377;({unsigned _tmp817=bound->loc;struct _fat_ptr _tmp816=({const char*_tmp376="expecting unsigned int, found %s";_tag_fat(_tmp376,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp817,_tmp816,_tag_fat(_tmp375,sizeof(void*),1U));});});}_LL0:;}
# 1692
if(!(vd->tq).real_const)
({void*_tmp378=0U;({struct _fat_ptr _tmp818=({const char*_tmp379="comprehension index variable is not declared const!";_tag_fat(_tmp379,sizeof(char),52U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp818,_tag_fat(_tmp378,sizeof(void*),0U));});});
# 1695
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
void**_tmp37A=0;void**topt2=_tmp37A;
struct Cyc_Absyn_Tqual*_tmp37B=0;struct Cyc_Absyn_Tqual*tqopt=_tmp37B;
void**_tmp37C=0;void**ztopt=_tmp37C;
# 1701
if(topt != 0){
void*_tmp37D=Cyc_Tcutil_compress(*topt);void*_stmttmp38=_tmp37D;void*_tmp37E=_stmttmp38;void*_tmp382;struct Cyc_Absyn_Exp*_tmp381;struct Cyc_Absyn_Tqual _tmp380;void*_tmp37F;struct Cyc_Absyn_PtrInfo _tmp383;switch(*((int*)_tmp37E)){case 3U: _LL6: _tmp383=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37E)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp383;
# 1704
topt2=({void**_tmp384=_cycalloc(sizeof(*_tmp384));*_tmp384=x.elt_type;_tmp384;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp385=_cycalloc(sizeof(*_tmp385));*_tmp385=x.elt_tq;_tmp385;});
ztopt=({void**_tmp386=_cycalloc(sizeof(*_tmp386));*_tmp386=(x.ptr_atts).zero_term;_tmp386;});
goto _LL5;}case 4U: _LL8: _tmp37F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E)->f1).elt_type;_tmp380=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E)->f1).tq;_tmp381=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E)->f1).num_elts;_tmp382=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E)->f1).zero_term;_LL9: {void*t=_tmp37F;struct Cyc_Absyn_Tqual tq=_tmp380;struct Cyc_Absyn_Exp*b=_tmp381;void*zt=_tmp382;
# 1709
topt2=({void**_tmp387=_cycalloc(sizeof(*_tmp387));*_tmp387=t;_tmp387;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp388=_cycalloc(sizeof(*_tmp388));*_tmp388=tq;_tmp388;});
ztopt=({void**_tmp389=_cycalloc(sizeof(*_tmp389));*_tmp389=zt;_tmp389;});
goto _LL5;}default: _LLA: _LLB:
# 1714
 goto _LL5;}_LL5:;}{
# 1717
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1719
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(body))
Cyc_Tcexp_unique_consume_err(body->loc);
if(te->le == 0){
# 1723
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp38A=0U;({unsigned _tmp81A=bound->loc;struct _fat_ptr _tmp819=({const char*_tmp38B="bound is not constant";_tag_fat(_tmp38B,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp81A,_tmp819,_tag_fat(_tmp38A,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp38C=0U;({unsigned _tmp81C=bound->loc;struct _fat_ptr _tmp81B=({const char*_tmp38D="body is not constant";_tag_fat(_tmp38D,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp81C,_tmp81B,_tag_fat(_tmp38C,sizeof(void*),0U));});});}
# 1728
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1731
struct Cyc_Absyn_Exp*_tmp38E=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp38E;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1735
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt))&& !
Cyc_Tcutil_is_noalias_path(body))
Cyc_Tcexp_unique_consume_err(body->loc);{
# 1739
void*_tmp38F=({void*_tmp81F=t;struct Cyc_Absyn_Tqual _tmp81E=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp81D=bound;Cyc_Absyn_array_type(_tmp81F,_tmp81E,_tmp81D,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1739
void*res=_tmp38F;
# 1741
return res;}}}}
# 1745
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1748
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp390=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp39=_tmp390;void*_tmp391=_stmttmp39;void*_tmp392;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp391)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp391)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp391)->f2 != 0){_LL1: _tmp392=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp391)->f2)->hd;_LL2: {void*t=_tmp392;
# 1753
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp393=({void*_tmp820=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp820,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp393;
b->topt=bound->topt;
bound=b;}
# 1758
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1760
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_String_pa_PrintArg_struct _tmp396=({struct Cyc_String_pa_PrintArg_struct _tmp5F7;_tmp5F7.tag=0U,({
struct _fat_ptr _tmp821=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp5F7.f1=_tmp821;});_tmp5F7;});void*_tmp394[1U];_tmp394[0]=& _tmp396;({unsigned _tmp823=bound->loc;struct _fat_ptr _tmp822=({const char*_tmp395="expecting unsigned int, found %s";_tag_fat(_tmp395,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp823,_tmp822,_tag_fat(_tmp394,sizeof(void*),1U));});});}_LL0:;}{
# 1765
void**_tmp397=0;void**topt2=_tmp397;
struct Cyc_Absyn_Tqual*_tmp398=0;struct Cyc_Absyn_Tqual*tqopt=_tmp398;
void**_tmp399=0;void**ztopt=_tmp399;
# 1769
if(topt != 0){
void*_tmp39A=Cyc_Tcutil_compress(*topt);void*_stmttmp3A=_tmp39A;void*_tmp39B=_stmttmp3A;void*_tmp39F;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Tqual _tmp39D;void*_tmp39C;struct Cyc_Absyn_PtrInfo _tmp3A0;switch(*((int*)_tmp39B)){case 3U: _LL6: _tmp3A0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39B)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp3A0;
# 1772
topt2=({void**_tmp3A1=_cycalloc(sizeof(*_tmp3A1));*_tmp3A1=x.elt_type;_tmp3A1;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));*_tmp3A2=x.elt_tq;_tmp3A2;});
ztopt=({void**_tmp3A3=_cycalloc(sizeof(*_tmp3A3));*_tmp3A3=(x.ptr_atts).zero_term;_tmp3A3;});
goto _LL5;}case 4U: _LL8: _tmp39C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39B)->f1).elt_type;_tmp39D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39B)->f1).tq;_tmp39E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39B)->f1).num_elts;_tmp39F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39B)->f1).zero_term;_LL9: {void*t=_tmp39C;struct Cyc_Absyn_Tqual tq=_tmp39D;struct Cyc_Absyn_Exp*b=_tmp39E;void*zt=_tmp39F;
# 1777
topt2=({void**_tmp3A4=_cycalloc(sizeof(*_tmp3A4));*_tmp3A4=t;_tmp3A4;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));*_tmp3A5=tq;_tmp3A5;});
ztopt=({void**_tmp3A6=_cycalloc(sizeof(*_tmp3A6));*_tmp3A6=zt;_tmp3A6;});
goto _LL5;}default: _LLA: _LLB:
# 1782
 goto _LL5;}_LL5:;}
# 1785
({unsigned _tmp826=loc;struct Cyc_Tcenv_Tenv*_tmp825=te;struct Cyc_List_List*_tmp824=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp826,_tmp825,_tmp824,& Cyc_Tcutil_tmk,1,1,t);});
if(topt2 != 0)Cyc_Unify_unify(*topt2,t);
# 1788
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp3A7=0U;({unsigned _tmp828=bound->loc;struct _fat_ptr _tmp827=({const char*_tmp3A8="bound is not constant";_tag_fat(_tmp3A8,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp828,_tmp827,_tag_fat(_tmp3A7,sizeof(void*),0U));});});}
# 1792
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1795
struct Cyc_Absyn_Exp*_tmp3A9=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp3A9;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;
# 1799
({void*_tmp3AA=0U;({unsigned _tmp82A=loc;struct _fat_ptr _tmp829=({const char*_tmp3AB="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp3AB,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp82A,_tmp829,_tag_fat(_tmp3AA,sizeof(void*),0U));});});}{
# 1802
void*_tmp3AC=({void*_tmp82D=t;struct Cyc_Absyn_Tqual _tmp82C=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp82B=bound;Cyc_Absyn_array_type(_tmp82D,_tmp82C,_tmp82B,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1802
void*res=_tmp3AC;
# 1804
return res;}}}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1817 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1821
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp3AD=_cycalloc(sizeof(*_tmp3AD));*_tmp3AD=ad;_tmp3AD;});}else{
# 1827
{struct _handler_cons _tmp3AE;_push_handler(& _tmp3AE);{int _tmp3B0=0;if(setjmp(_tmp3AE.handler))_tmp3B0=1;if(!_tmp3B0){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1827
;_pop_handler();}else{void*_tmp3AF=(void*)Cyc_Core_get_exn_thrown();void*_tmp3B1=_tmp3AF;void*_tmp3B2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3B1)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1830
({struct Cyc_String_pa_PrintArg_struct _tmp3B5=({struct Cyc_String_pa_PrintArg_struct _tmp5F8;_tmp5F8.tag=0U,({struct _fat_ptr _tmp82E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp5F8.f1=_tmp82E;});_tmp5F8;});void*_tmp3B3[1U];_tmp3B3[0]=& _tmp3B5;({unsigned _tmp830=loc;struct _fat_ptr _tmp82F=({const char*_tmp3B4="unbound struct/union name %s";_tag_fat(_tmp3B4,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp830,_tmp82F,_tag_fat(_tmp3B3,sizeof(void*),1U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp3B2=_tmp3B1;_LL4: {void*exn=_tmp3B2;(int)_rethrow(exn);}}_LL0:;}}}
# 1833
*ad_opt=ad;
*tn=ad->name;}
# 1836
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3B8=({struct Cyc_String_pa_PrintArg_struct _tmp5F9;_tmp5F9.tag=0U,({struct _fat_ptr _tmp831=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp3B9="struct";_tag_fat(_tmp3B9,sizeof(char),7U);}):({const char*_tmp3BA="union";_tag_fat(_tmp3BA,sizeof(char),6U);}));_tmp5F9.f1=_tmp831;});_tmp5F9;});void*_tmp3B6[1U];_tmp3B6[0]=& _tmp3B8;({unsigned _tmp833=loc;struct _fat_ptr _tmp832=({const char*_tmp3B7="can't construct abstract %s";_tag_fat(_tmp3B7,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp833,_tmp832,_tag_fat(_tmp3B6,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1844
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp3BB=Cyc_Tcenv_new_status(te);enum Cyc_Tcenv_NewStatus status=_tmp3BB;
if((int)status == (int)1U)
te2=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);else{
# 1853
te2=Cyc_Tcenv_set_new_status(status,te);}{
# 1855
struct _tuple13 _tmp3BC=({struct _tuple13 _tmp602;({struct Cyc_List_List*_tmp834=Cyc_Tcenv_lookup_type_vars(te2);_tmp602.f1=_tmp834;}),_tmp602.f2=Cyc_Core_heap_region;_tmp602;});struct _tuple13 env=_tmp3BC;
struct Cyc_List_List*_tmp3BD=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp3BD;
struct Cyc_List_List*_tmp3BE=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp3BE;
struct Cyc_List_List*_tmp3BF=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp3BF;
struct Cyc_List_List*_tmp3C0=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp3C0;
struct Cyc_List_List*_tmp3C1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp3C1;
void*res_typ;
# 1866
if(topt != 0){
void*_tmp3C2=Cyc_Tcutil_compress(*topt);void*_stmttmp3B=_tmp3C2;void*_tmp3C3=_stmttmp3B;struct Cyc_List_List*_tmp3C5;struct Cyc_Absyn_Aggrdecl**_tmp3C4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp3C4=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->f1)->f1).KnownAggr).val;_tmp3C5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp3C4;struct Cyc_List_List*all_typs2=_tmp3C5;
# 1869
if(*adptr2 == *adptr){
{struct Cyc_List_List*_tmp3C6=all_typs;struct Cyc_List_List*ats=_tmp3C6;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1874
res_typ=*topt;
goto _LL5;}
# 1877
goto _LL9;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1879
 res_typ=({union Cyc_Absyn_AggrInfo _tmp835=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp835,all_typs);});}_LL5:;}else{
# 1882
res_typ=({union Cyc_Absyn_AggrInfo _tmp836=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp836,all_typs);});}{
# 1885
struct Cyc_List_List*_tmp3C7=*ts;struct Cyc_List_List*user_ex_ts=_tmp3C7;
struct Cyc_List_List*_tmp3C8=exist_typs;struct Cyc_List_List*ex_ts=_tmp3C8;
while(user_ex_ts != 0 && ex_ts != 0){
# 1889
({unsigned _tmp839=loc;struct Cyc_Tcenv_Tenv*_tmp838=te2;struct Cyc_List_List*_tmp837=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp839,_tmp838,_tmp837,& Cyc_Tcutil_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1895
if(user_ex_ts != 0)
({void*_tmp3C9=0U;({unsigned _tmp83B=loc;struct _fat_ptr _tmp83A=({const char*_tmp3CA="too many explicit witness types";_tag_fat(_tmp3CA,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp83B,_tmp83A,_tag_fat(_tmp3C9,sizeof(void*),0U));});});
# 1898
*ts=exist_typs;{
# 1901
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3CB=(struct _tuple19*)fields->hd;struct _tuple19*_stmttmp3C=_tmp3CB;struct _tuple19*_tmp3CC=_stmttmp3C;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_Absyn_Aggrfield*_tmp3CD;_LLB: _tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp3CD;struct Cyc_Absyn_Exp*field_exp=_tmp3CE;
void*_tmp3CF=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp3CF;
({struct Cyc_Tcenv_Tenv*_tmp83C=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp83C,& inst_fieldtyp,field_exp);});
# 1909
if(!({struct Cyc_RgnOrder_RgnPO*_tmp83E=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp83D=field_exp;Cyc_Tcutil_coerce_arg(_tmp83E,_tmp83D,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp601;_tmp601.tag=0U,({struct _fat_ptr _tmp83F=({const char*_tmp3DC="field ";_tag_fat(_tmp3DC,sizeof(char),7U);});_tmp601.f1=_tmp83F;});_tmp601;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp600;_tmp600.tag=0U,_tmp600.f1=*field->name;_tmp600;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FF;_tmp5FF.tag=0U,({struct _fat_ptr _tmp840=({const char*_tmp3DB=" of ";_tag_fat(_tmp3DB,sizeof(char),5U);});_tmp5FF.f1=_tmp840;});_tmp5FF;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3D4=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5FE;_tmp5FE.tag=5U,_tmp5FE.f1=ad;_tmp5FE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FD;_tmp5FD.tag=0U,({
struct _fat_ptr _tmp841=({const char*_tmp3DA=" expects type ";_tag_fat(_tmp3DA,sizeof(char),15U);});_tmp5FD.f1=_tmp841;});_tmp5FD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3D6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5FC;_tmp5FC.tag=2U,_tmp5FC.f1=(void*)inst_fieldtyp;_tmp5FC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FB;_tmp5FB.tag=0U,({struct _fat_ptr _tmp842=({const char*_tmp3D9=" != ";_tag_fat(_tmp3D9,sizeof(char),5U);});_tmp5FB.f1=_tmp842;});_tmp5FB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3D8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=2U,_tmp5FA.f1=(void*)_check_null(field_exp->topt);_tmp5FA;});void*_tmp3D0[8U];_tmp3D0[0]=& _tmp3D1,_tmp3D0[1]=& _tmp3D2,_tmp3D0[2]=& _tmp3D3,_tmp3D0[3]=& _tmp3D4,_tmp3D0[4]=& _tmp3D5,_tmp3D0[5]=& _tmp3D6,_tmp3D0[6]=& _tmp3D7,_tmp3D0[7]=& _tmp3D8;({unsigned _tmp843=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp843,_tag_fat(_tmp3D0,sizeof(void*),8U));});});}}{
# 1914
struct Cyc_List_List*_tmp3DD=0;struct Cyc_List_List*rpo_inst=_tmp3DD;
{struct Cyc_List_List*_tmp3DE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3DE;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct _tuple0*_tmp846=({struct _tuple0*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));({void*_tmp845=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3DF->f1=_tmp845;}),({
void*_tmp844=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3DF->f2=_tmp844;});_tmp3DF;});
# 1916
_tmp3E0->hd=_tmp846;}),_tmp3E0->tl=rpo_inst;_tmp3E0;});}}
# 1919
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}}
# 1925
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1927
{void*_tmp3E1=Cyc_Tcutil_compress(ts);void*_stmttmp3D=_tmp3E1;void*_tmp3E2=_stmttmp3D;struct Cyc_List_List*_tmp3E4;enum Cyc_Absyn_AggrKind _tmp3E3;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2)->tag == 7U){_LL1: _tmp3E3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2)->f1;_tmp3E4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp3E3;struct Cyc_List_List*fs=_tmp3E4;
# 1929
if((int)k == (int)1U)
({void*_tmp3E5=0U;({unsigned _tmp848=loc;struct _fat_ptr _tmp847=({const char*_tmp3E6="expecting struct but found union";_tag_fat(_tmp3E6,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp848,_tmp847,_tag_fat(_tmp3E5,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3E7=(struct _tuple19*)fields->hd;struct _tuple19*_stmttmp3E=_tmp3E7;struct _tuple19*_tmp3E8=_stmttmp3E;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Aggrfield*_tmp3E9;_LL6: _tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp3E9;struct Cyc_Absyn_Exp*field_exp=_tmp3EA;
({struct Cyc_Tcenv_Tenv*_tmp84A=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp849=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp84A,_tmp849,field_exp);});
# 1938
if(!({struct Cyc_RgnOrder_RgnPO*_tmp84C=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp84B=field_exp;Cyc_Tcutil_coerce_arg(_tmp84C,_tmp84B,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp608;_tmp608.tag=0U,({struct _fat_ptr _tmp84D=({const char*_tmp3F4="field ";_tag_fat(_tmp3F4,sizeof(char),7U);});_tmp608.f1=_tmp84D;});_tmp608;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp607;_tmp607.tag=0U,_tmp607.f1=*field->name;_tmp607;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp606;_tmp606.tag=0U,({
struct _fat_ptr _tmp84E=({const char*_tmp3F3=" of struct expects type ";_tag_fat(_tmp3F3,sizeof(char),25U);});_tmp606.f1=_tmp84E;});_tmp606;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3EF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp605;_tmp605.tag=2U,_tmp605.f1=(void*)field->type;_tmp605;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp604;_tmp604.tag=0U,({struct _fat_ptr _tmp84F=({const char*_tmp3F2=" != ";_tag_fat(_tmp3F2,sizeof(char),5U);});_tmp604.f1=_tmp84F;});_tmp604;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp603;_tmp603.tag=2U,_tmp603.f1=(void*)_check_null(field_exp->topt);_tmp603;});void*_tmp3EB[6U];_tmp3EB[0]=& _tmp3EC,_tmp3EB[1]=& _tmp3ED,_tmp3EB[2]=& _tmp3EE,_tmp3EB[3]=& _tmp3EF,_tmp3EB[4]=& _tmp3F0,_tmp3EB[5]=& _tmp3F1;({unsigned _tmp850=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp850,_tag_fat(_tmp3EB,sizeof(void*),6U));});});}}
# 1943
goto _LL0;}}}else{_LL3: _LL4:
({void*_tmp3F5=0U;({struct _fat_ptr _tmp851=({const char*_tmp3F6="tcAnonStruct: wrong type";_tag_fat(_tmp3F6,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp851,_tag_fat(_tmp3F5,sizeof(void*),0U));});});}_LL0:;}
# 1946
return ts;}
# 1950
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1953
struct _tuple13 _tmp3F7=({struct _tuple13 _tmp610;({struct Cyc_List_List*_tmp852=Cyc_Tcenv_lookup_type_vars(te);_tmp610.f1=_tmp852;}),_tmp610.f2=Cyc_Core_heap_region;_tmp610;});struct _tuple13 env=_tmp3F7;
struct Cyc_List_List*_tmp3F8=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3F8;
struct Cyc_List_List*_tmp3F9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp3F9;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp853=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp853,all_typs);});
# 1958
if(topt != 0){
void*_tmp3FA=Cyc_Tcutil_compress(*topt);void*_stmttmp3F=_tmp3FA;void*_tmp3FB=_stmttmp3F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FB)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FB)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1964
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(inst != 0)t=Cyc_Tcutil_substitute(inst,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp855=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp854=e;Cyc_Tcutil_coerce_arg(_tmp855,_tmp854,t,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp3FE=({struct Cyc_String_pa_PrintArg_struct _tmp60B;_tmp60B.tag=0U,({
# 1974
struct _fat_ptr _tmp856=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp60B.f1=_tmp856;});_tmp60B;});struct Cyc_String_pa_PrintArg_struct _tmp3FF=({struct Cyc_String_pa_PrintArg_struct _tmp60A;_tmp60A.tag=0U,({struct _fat_ptr _tmp857=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp60A.f1=_tmp857;});_tmp60A;});struct Cyc_String_pa_PrintArg_struct _tmp400=({struct Cyc_String_pa_PrintArg_struct _tmp609;_tmp609.tag=0U,({
struct _fat_ptr _tmp858=(struct _fat_ptr)((struct _fat_ptr)(e->topt == 0?(struct _fat_ptr)({const char*_tmp401="?";_tag_fat(_tmp401,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp609.f1=_tmp858;});_tmp609;});void*_tmp3FC[3U];_tmp3FC[0]=& _tmp3FE,_tmp3FC[1]=& _tmp3FF,_tmp3FC[2]=& _tmp400;({unsigned _tmp85A=e->loc;struct _fat_ptr _tmp859=({const char*_tmp3FD="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_fat(_tmp3FD,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp85A,_tmp859,_tag_fat(_tmp3FC,sizeof(void*),3U));});});
Cyc_Unify_explain_failure();}}
# 1979
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp403=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D;_tmp60D.tag=0U,({
struct _fat_ptr _tmp85B=({const char*_tmp405="too many arguments for datatype constructor ";_tag_fat(_tmp405,sizeof(char),45U);});_tmp60D.f1=_tmp85B;});_tmp60D;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp404=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp60C;_tmp60C.tag=1U,_tmp60C.f1=tuf->name;_tmp60C;});void*_tmp402[2U];_tmp402[0]=& _tmp403,_tmp402[1]=& _tmp404;({struct Cyc_Tcenv_Tenv*_tmp85E=te;unsigned _tmp85D=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp85C=topt;Cyc_Tcexp_expr_err(_tmp85E,_tmp85D,_tmp85C,_tag_fat(_tmp402,sizeof(void*),2U));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp407=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60F;_tmp60F.tag=0U,({
struct _fat_ptr _tmp85F=({const char*_tmp409="too few arguments for datatype constructor ";_tag_fat(_tmp409,sizeof(char),44U);});_tmp60F.f1=_tmp85F;});_tmp60F;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp408=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp60E;_tmp60E.tag=1U,_tmp60E.f1=tuf->name;_tmp60E;});void*_tmp406[2U];_tmp406[0]=& _tmp407,_tmp406[1]=& _tmp408;({struct Cyc_Tcenv_Tenv*_tmp862=te;unsigned _tmp861=loc;void**_tmp860=topt;Cyc_Tcexp_expr_err(_tmp862,_tmp861,_tmp860,_tag_fat(_tmp406,sizeof(void*),2U));});});
return res;}}
# 1989
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1991
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1994
if(topt == 0)
return 0;{
void*_tmp40A=Cyc_Tcutil_compress(*topt);void*_stmttmp40=_tmp40A;void*_tmp40B=_stmttmp40;void*_tmp40C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40B)->tag == 3U){_LL1: _tmp40C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40B)->f1).elt_type;_LL2: {void*elt_typ=_tmp40C;
# 1998
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 2004
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2006
enum Cyc_Absyn_AliasQual _tmp40D=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp41=_tmp40D;enum Cyc_Absyn_AliasQual _tmp40E=_stmttmp41;if(_tmp40E == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 2012
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2016
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 2020
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp40F=Cyc_Tcutil_compress(handle_type);void*_stmttmp42=_tmp40F;void*_tmp410=_stmttmp42;void*_tmp411;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f2 != 0){_LL1: _tmp411=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f2)->hd;_LL2: {void*r=_tmp411;
# 2025
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2029
({struct Cyc_String_pa_PrintArg_struct _tmp414=({struct Cyc_String_pa_PrintArg_struct _tmp611;_tmp611.tag=0U,({
struct _fat_ptr _tmp863=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp611.f1=_tmp863;});_tmp611;});void*_tmp412[1U];_tmp412[0]=& _tmp414;({unsigned _tmp865=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _fat_ptr _tmp864=({const char*_tmp413="expecting region_t type but found %s";_tag_fat(_tmp413,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp865,_tmp864,_tag_fat(_tmp412,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2036
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp866=Cyc_Absyn_uniquergn_exp();*ropt=_tmp866;});}}}
# 2043
({struct Cyc_Tcenv_Tenv*_tmp867=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp867,& Cyc_Absyn_uint_type,*e);});{
# 2052 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp415=0U;({struct _fat_ptr _tmp868=({const char*_tmp416="calloc with empty type";_tag_fat(_tmp416,sizeof(char),23U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp868,_tag_fat(_tmp415,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp86B=loc;struct Cyc_Tcenv_Tenv*_tmp86A=te;struct Cyc_List_List*_tmp869=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp86B,_tmp86A,_tmp869,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp419=({struct Cyc_String_pa_PrintArg_struct _tmp612;_tmp612.tag=0U,({struct _fat_ptr _tmp86C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(elt_type));_tmp612.f1=_tmp86C;});_tmp612;});void*_tmp417[1U];_tmp417[0]=& _tmp419;({unsigned _tmp86E=loc;struct _fat_ptr _tmp86D=({const char*_tmp418="calloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp418,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp86E,_tmp86D,_tag_fat(_tmp417,sizeof(void*),1U));});});
num_elts=*e;
one_elt=0;}else{
# 2065
void*er=(*e)->r;
retry_sizeof: {
void*_tmp41A=er;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_Absyn_Exp*_tmp41B;void*_tmp41D;switch(*((int*)_tmp41A)){case 17U: _LL6: _tmp41D=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp41A)->f1;_LL7: {void*t2=_tmp41D;
# 2069
elt_type=t2;
({void**_tmp86F=({void**_tmp41E=_cycalloc(sizeof(*_tmp41E));*_tmp41E=elt_type;_tmp41E;});*t=_tmp86F;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41A)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41A)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41A)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41A)->f2)->tl)->tl == 0){_LL8: _tmp41B=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41A)->f2)->hd;_tmp41C=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41A)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp41B;struct Cyc_Absyn_Exp*e2=_tmp41C;
# 2076
{struct _tuple0 _tmp41F=({struct _tuple0 _tmp615;_tmp615.f1=e1->r,_tmp615.f2=e2->r;_tmp615;});struct _tuple0 _stmttmp43=_tmp41F;struct _tuple0 _tmp420=_stmttmp43;void*_tmp421;void*_tmp422;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp420.f1)->tag == 17U){_LLD: _tmp422=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp420.f1)->f1;_LLE: {void*t1=_tmp422;
# 2079
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t1)){
# 2082
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t1))
({struct Cyc_String_pa_PrintArg_struct _tmp425=({struct Cyc_String_pa_PrintArg_struct _tmp613;_tmp613.tag=0U,({struct _fat_ptr _tmp870=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp613.f1=_tmp870;});_tmp613;});void*_tmp423[1U];_tmp423[0]=& _tmp425;({unsigned _tmp872=loc;struct _fat_ptr _tmp871=({const char*_tmp424="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp424,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp872,_tmp871,_tag_fat(_tmp423,sizeof(void*),1U));});});else{
# 2085
*is_calloc=1;}}
# 2088
elt_type=t1;
({void**_tmp873=({void**_tmp426=_cycalloc(sizeof(*_tmp426));*_tmp426=elt_type;_tmp426;});*t=_tmp873;});
num_elts=e2;
one_elt=0;
goto _LLC;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp420.f2)->tag == 17U){_LLF: _tmp421=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp420.f2)->f1;_LL10: {void*t2=_tmp421;
# 2095
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 2098
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp429=({struct Cyc_String_pa_PrintArg_struct _tmp614;_tmp614.tag=0U,({struct _fat_ptr _tmp874=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp614.f1=_tmp874;});_tmp614;});void*_tmp427[1U];_tmp427[0]=& _tmp429;({unsigned _tmp876=loc;struct _fat_ptr _tmp875=({const char*_tmp428="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp428,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp876,_tmp875,_tag_fat(_tmp427,sizeof(void*),1U));});});else{
# 2101
*is_calloc=1;}}
# 2104
elt_type=t2;
({void**_tmp877=({void**_tmp42A=_cycalloc(sizeof(*_tmp42A));*_tmp42A=elt_type;_tmp42A;});*t=_tmp877;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2111
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2113
 No_sizeof: {
# 2116
struct Cyc_Absyn_Exp*_tmp42B=*e;struct Cyc_Absyn_Exp*real_e=_tmp42B;
{void*_tmp42C=real_e->r;void*_stmttmp44=_tmp42C;void*_tmp42D=_stmttmp44;struct Cyc_Absyn_Exp*_tmp42E;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42D)->tag == 14U){_LL14: _tmp42E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42D)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp42E;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2121
{void*_tmp42F=Cyc_Tcutil_compress((void*)_check_null(real_e->topt));void*_stmttmp45=_tmp42F;void*_tmp430=_stmttmp45;void*_tmp431;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp430)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp430)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp430)->f2 != 0){_LL19: _tmp431=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp430)->f2)->hd;_LL1A: {void*tagt=_tmp431;
# 2123
{void*_tmp432=Cyc_Tcutil_compress(tagt);void*_stmttmp46=_tmp432;void*_tmp433=_stmttmp46;struct Cyc_Absyn_Exp*_tmp434;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp433)->tag == 9U){_LL1E: _tmp434=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp433)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp434;
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2127
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2130
elt_type=Cyc_Absyn_char_type;
({void**_tmp878=({void**_tmp435=_cycalloc(sizeof(*_tmp435));*_tmp435=elt_type;_tmp435;});*t=_tmp878;});
num_elts=*e;
one_elt=0;}
# 2135
goto _LL5;}_LL5:;}}
# 2139
*is_fat=!one_elt;
# 2142
{void*_tmp436=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp437;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp437=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp437;
# 2144
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({void*_tmp438=0U;({unsigned _tmp87A=loc;struct _fat_ptr _tmp879=({const char*_tmp439="malloc with existential types not yet implemented";_tag_fat(_tmp439,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp87A,_tmp879,_tag_fat(_tmp438,sizeof(void*),0U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2151
void*(*_tmp43A)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp43A;
void*_tmp43B=Cyc_Absyn_false_type;void*zero_term=_tmp43B;
if(topt != 0){
void*_tmp43C=Cyc_Tcutil_compress(*topt);void*_stmttmp47=_tmp43C;void*_tmp43D=_stmttmp47;void*_tmp440;void*_tmp43F;void*_tmp43E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43D)->tag == 3U){_LL28: _tmp43E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43D)->f1).ptr_atts).nullable;_tmp43F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43D)->f1).ptr_atts).bounds;_tmp440=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43D)->f1).ptr_atts).zero_term;_LL29: {void*n=_tmp43E;void*b=_tmp43F;void*zt=_tmp440;
# 2156
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2161
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({void*_tmp441=0U;({unsigned _tmp87C=loc;struct _fat_ptr _tmp87B=({const char*_tmp442="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp442,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp87C,_tmp87B,_tag_fat(_tmp441,sizeof(void*),0U));});});
*is_calloc=1;}{
# 2167
struct Cyc_Absyn_Exp*_tmp443=({void*_tmp87D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp87D,b);});struct Cyc_Absyn_Exp*eopt=_tmp443;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp444=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp444;
int _tmp445=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp445;
if(is_constant && Cyc_Evexp_same_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp881=elt_type;void*_tmp880=rgn;struct Cyc_Absyn_Tqual _tmp87F=Cyc_Absyn_empty_tqual(0U);void*_tmp87E=b;Cyc_Absyn_atb_type(_tmp881,_tmp880,_tmp87F,_tmp87E,zero_term);});}{
# 2175
void*_tmp446=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_stmttmp48=_tmp446;void*_tmp447=_stmttmp48;void*_tmp448;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f2 != 0){_LL2D: _tmp448=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f2)->hd;_LL2E: {void*tagtyp=_tmp448;
# 2177
struct Cyc_Absyn_Exp*_tmp449=({void*_tmp882=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp882,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp449;
# 2179
if(Cyc_Evexp_same_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp886=elt_type;void*_tmp885=rgn;struct Cyc_Absyn_Tqual _tmp884=Cyc_Absyn_empty_tqual(0U);void*_tmp883=b;Cyc_Absyn_atb_type(_tmp886,_tmp885,_tmp884,_tmp883,zero_term);});}
# 2183
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2187
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2190
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp88A)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=ptr_maker;void*_tmp889=elt_type;void*_tmp888=rgn;struct Cyc_Absyn_Tqual _tmp887=Cyc_Absyn_empty_tqual(0U);_tmp88A(_tmp889,_tmp888,_tmp887,zero_term);});}}}
# 2195
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2197
struct Cyc_Tcenv_Tenv*_tmp44A=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp44A;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp44B=(void*)_check_null(e1->topt);void*t1=_tmp44B;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2204
{void*_tmp44C=Cyc_Tcutil_compress(t1);void*_stmttmp49=_tmp44C;void*_tmp44D=_stmttmp49;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44D)->tag == 4U){_LL1: _LL2:
({void*_tmp44E=0U;({unsigned _tmp88C=loc;struct _fat_ptr _tmp88B=({const char*_tmp44F="cannot assign to an array";_tag_fat(_tmp44F,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp88C,_tmp88B,_tag_fat(_tmp44E,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2209
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp450=0U;({unsigned _tmp88E=loc;struct _fat_ptr _tmp88D=({const char*_tmp451="Swap not allowed for non-pointer or non-word-sized types.";_tag_fat(_tmp451,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp88E,_tmp88D,_tag_fat(_tmp450,sizeof(void*),0U));});});
# 2213
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp453=({struct Cyc_Warn_String_Warn_Warg_struct _tmp616;_tmp616.tag=0U,({struct _fat_ptr _tmp88F=({const char*_tmp454="swap non-lvalue";_tag_fat(_tmp454,sizeof(char),16U);});_tmp616.f1=_tmp88F;});_tmp616;});void*_tmp452[1U];_tmp452[0]=& _tmp453;({struct Cyc_Tcenv_Tenv*_tmp892=te;unsigned _tmp891=e1->loc;void**_tmp890=topt;Cyc_Tcexp_expr_err(_tmp892,_tmp891,_tmp890,_tag_fat(_tmp452,sizeof(void*),1U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp456=({struct Cyc_Warn_String_Warn_Warg_struct _tmp617;_tmp617.tag=0U,({struct _fat_ptr _tmp893=({const char*_tmp457="swap non-lvalue";_tag_fat(_tmp457,sizeof(char),16U);});_tmp617.f1=_tmp893;});_tmp617;});void*_tmp455[1U];_tmp455[0]=& _tmp456;({struct Cyc_Tcenv_Tenv*_tmp896=te;unsigned _tmp895=e2->loc;void**_tmp894=topt;Cyc_Tcexp_expr_err(_tmp896,_tmp895,_tmp894,_tag_fat(_tmp455,sizeof(void*),1U));});});
# 2218
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp459=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61B;_tmp61B.tag=0U,({struct _fat_ptr _tmp897=({const char*_tmp45E="type mismatch: ";_tag_fat(_tmp45E,sizeof(char),16U);});_tmp61B.f1=_tmp897;});_tmp61B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp45A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61A;_tmp61A.tag=2U,_tmp61A.f1=(void*)t1;_tmp61A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp45B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp619;_tmp619.tag=0U,({struct _fat_ptr _tmp898=({const char*_tmp45D=" != ";_tag_fat(_tmp45D,sizeof(char),5U);});_tmp619.f1=_tmp898;});_tmp619;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp45C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp618;_tmp618.tag=2U,_tmp618.f1=(void*)t2;_tmp618;});void*_tmp458[4U];_tmp458[0]=& _tmp459,_tmp458[1]=& _tmp45A,_tmp458[2]=& _tmp45B,_tmp458[3]=& _tmp45C;({struct Cyc_Tcenv_Tenv*_tmp89B=te;unsigned _tmp89A=loc;void**_tmp899=topt;Cyc_Tcexp_expr_err(_tmp89B,_tmp89A,_tmp899,_tag_fat(_tmp458,sizeof(void*),4U));});});
return Cyc_Absyn_void_type;}}}
# 2227
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp89C=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp89C,s,1);});
# 2230
while(1){
void*_tmp45F=s->r;void*_stmttmp4A=_tmp45F;void*_tmp460=_stmttmp4A;struct Cyc_Absyn_Stmt*_tmp461;struct Cyc_Absyn_Stmt*_tmp462;struct Cyc_Absyn_Exp*_tmp463;switch(*((int*)_tmp460)){case 1U: _LL1: _tmp463=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp460)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp463;
# 2234
void*_tmp464=(void*)_check_null(e->topt);void*t=_tmp464;
if(!({void*_tmp89D=t;Cyc_Unify_unify(_tmp89D,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp466=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp89E=({const char*_tmp468="statement expression returns type ";_tag_fat(_tmp468,sizeof(char),35U);});_tmp61D.f1=_tmp89E;});_tmp61D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp467=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61C;_tmp61C.tag=2U,_tmp61C.f1=(void*)t;_tmp61C;});void*_tmp465[2U];_tmp465[0]=& _tmp466,_tmp465[1]=& _tmp467;({unsigned _tmp89F=loc;Cyc_Tcexp_err_and_explain(_tmp89F,_tag_fat(_tmp465,sizeof(void*),2U));});});
return t;}case 2U: _LL3: _tmp462=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp460)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp462;
s=s2;continue;}case 12U: _LL5: _tmp461=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp460)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp461;
s=s1;continue;}default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp46A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61E;_tmp61E.tag=0U,({
struct _fat_ptr _tmp8A0=({const char*_tmp46B="statement expression must end with expression";_tag_fat(_tmp46B,sizeof(char),46U);});_tmp61E.f1=_tmp8A0;});_tmp61E;});void*_tmp469[1U];_tmp469[0]=& _tmp46A;({struct Cyc_Tcenv_Tenv*_tmp8A3=te;unsigned _tmp8A2=loc;void**_tmp8A1=topt;Cyc_Tcexp_expr_err(_tmp8A3,_tmp8A2,_tmp8A1,_tag_fat(_tmp469,sizeof(void*),1U));});});}_LL0:;}}
# 2245
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp8A4=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp8A4,0,e);}));
{void*_tmp46C=t;struct Cyc_Absyn_Aggrdecl*_tmp46D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46C)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46C)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46C)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp46D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46C)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp46D;
# 2249
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2252
({struct Cyc_Warn_String_Warn_Warg_struct _tmp46F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp620;_tmp620.tag=0U,({struct _fat_ptr _tmp8A5=({const char*_tmp471="expecting @tagged union but found ";_tag_fat(_tmp471,sizeof(char),35U);});_tmp620.f1=_tmp8A5;});_tmp620;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp470=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61F;_tmp61F.tag=2U,_tmp61F.f1=(void*)t;_tmp61F;});void*_tmp46E[2U];_tmp46E[0]=& _tmp46F,_tmp46E[1]=& _tmp470;({unsigned _tmp8A6=loc;Cyc_Warn_err2(_tmp8A6,_tag_fat(_tmp46E,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}
# 2255
return Cyc_Absyn_uint_type;}
# 2259
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2263
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp472=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));struct Cyc_Tcenv_Tenv*te=_tmp472;
if(*rgn_handle != 0){
# 2268
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp473=Cyc_Tcutil_compress(handle_type);void*_stmttmp4B=_tmp473;void*_tmp474=_stmttmp4B;void*_tmp475;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474)->f2 != 0){_LL1: _tmp475=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474)->f2)->hd;_LL2: {void*r=_tmp475;
# 2273
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2277
({struct Cyc_String_pa_PrintArg_struct _tmp478=({struct Cyc_String_pa_PrintArg_struct _tmp621;_tmp621.tag=0U,({
struct _fat_ptr _tmp8A7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp621.f1=_tmp8A7;});_tmp621;});void*_tmp476[1U];_tmp476[0]=& _tmp478;({unsigned _tmp8A9=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _fat_ptr _tmp8A8=({const char*_tmp477="expecting region_t type but found %s";_tag_fat(_tmp477,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp8A9,_tmp8A8,_tag_fat(_tmp476,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2281
if(topt != 0){
# 2284
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp8AA;});}}}{
# 2291
void*_tmp479=e1->r;void*_stmttmp4C=_tmp479;void*_tmp47A=_stmttmp4C;struct Cyc_List_List*_tmp47B;struct Cyc_List_List*_tmp47D;struct Cyc_Core_Opt*_tmp47C;switch(*((int*)_tmp47A)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2296
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp47E=0U;({struct _fat_ptr _tmp8AB=({const char*_tmp47F="tcNew: comprehension returned non-array type";_tag_fat(_tmp47F,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8AB,_tag_fat(_tmp47E,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8AE=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8AD=loc;void*_tmp8AC=res_typ;Cyc_Tcutil_silent_castable(_tmp8AE,_tmp8AD,_tmp8AC,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2308
return res_typ;}case 36U: _LLA: _tmp47C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp47A)->f1;_tmp47D=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp47A)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp47C;struct Cyc_List_List*des=_tmp47D;
# 2310
({void*_tmp8AF=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->tag=26U,_tmp480->f1=des;_tmp480;});e1->r=_tmp8AF;});
_tmp47B=des;goto _LLD;}case 26U: _LLC: _tmp47B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp47A)->f1;_LLD: {struct Cyc_List_List*des=_tmp47B;
# 2313
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp481=Cyc_Tcutil_compress(*topt);void*_stmttmp4D=_tmp481;void*_tmp482=_stmttmp4D;void*_tmp485;struct Cyc_Absyn_Tqual _tmp484;void**_tmp483;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp482)->tag == 3U){_LL15: _tmp483=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp482)->f1).elt_type;_tmp484=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp482)->f1).elt_tq;_tmp485=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp482)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp483;struct Cyc_Absyn_Tqual tq=_tmp484;void*zt=_tmp485;
# 2319
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2325
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp486=0U;({struct _fat_ptr _tmp8B0=({const char*_tmp487="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp487,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8B0,_tag_fat(_tmp486,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2334
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8B3=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8B2=loc;void*_tmp8B1=res_typ;Cyc_Tcutil_silent_castable(_tmp8B3,_tmp8B2,_tmp8B1,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2341
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp47A)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2346
void*_tmp488=({void*_tmp8B7=Cyc_Absyn_char_type;void*_tmp8B6=rgn;struct Cyc_Absyn_Tqual _tmp8B5=Cyc_Absyn_const_tqual(0U);void*_tmp8B4=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8B7,_tmp8B6,_tmp8B5,_tmp8B4,Cyc_Absyn_true_type);});void*topt2=_tmp488;
# 2348
void*_tmp489=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp489;
return({void*_tmp8BB=t;void*_tmp8BA=rgn;struct Cyc_Absyn_Tqual _tmp8B9=Cyc_Absyn_empty_tqual(0U);void*_tmp8B8=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp8BB,_tmp8BA,_tmp8B9,_tmp8B8,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2353
void*_tmp48A=({void*_tmp8BF=Cyc_Absyn_wchar_type();void*_tmp8BE=rgn;struct Cyc_Absyn_Tqual _tmp8BD=Cyc_Absyn_const_tqual(0U);void*_tmp8BC=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8BF,_tmp8BE,_tmp8BD,_tmp8BC,Cyc_Absyn_true_type);});void*topt2=_tmp48A;
# 2355
void*_tmp48B=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp48B;
return({void*_tmp8C3=t;void*_tmp8C2=rgn;struct Cyc_Absyn_Tqual _tmp8C1=Cyc_Absyn_empty_tqual(0U);void*_tmp8C0=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp8C3,_tmp8C2,_tmp8C1,_tmp8C0,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2362
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp48C=Cyc_Tcutil_compress(*topt);void*_stmttmp4E=_tmp48C;void*_tmp48D=_stmttmp4E;struct Cyc_Absyn_Tqual _tmp48F;void**_tmp48E;switch(*((int*)_tmp48D)){case 3U: _LL1A: _tmp48E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D)->f1).elt_type;_tmp48F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D)->f1).elt_tq;_LL1B: {void**elttype=_tmp48E;struct Cyc_Absyn_Tqual tq=_tmp48F;
# 2368
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D)->f1)->tag == 18U){_LL1C: _LL1D:
# 2372
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2378
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2380
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt)&& !Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp490=_cycalloc(sizeof(*_tmp490));
_tmp490->tag=3U,(_tmp490->f1).elt_type=telt,({struct Cyc_Absyn_Tqual _tmp8C6=Cyc_Absyn_empty_tqual(0U);(_tmp490->f1).elt_tq=_tmp8C6;}),
((_tmp490->f1).ptr_atts).rgn=rgn,({void*_tmp8C5=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmp490->f1).ptr_atts).nullable=_tmp8C5;}),({
void*_tmp8C4=Cyc_Absyn_bounds_one();((_tmp490->f1).ptr_atts).bounds=_tmp8C4;}),((_tmp490->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp490->f1).ptr_atts).ptrloc=0;_tmp490;});
# 2387
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp8C9=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8C8=loc;void*_tmp8C7=res_typ;Cyc_Tcutil_silent_castable(_tmp8C9,_tmp8C8,_tmp8C7,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}
# 2393
return res_typ;}}}}_LL5:;}}
# 2399
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp8CB=t=({void*_tmp8CA=t;Cyc_Tcutil_promote_array(_tmp8CA,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp8CB;});
return t;}
# 2409
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2413
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(e->loc);{
void*_tmp491=e->r;void*_stmttmp4F=_tmp491;void*_tmp492=_stmttmp4F;switch(*((int*)_tmp492)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp492)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2422
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array_type(t)){
t=({void*_tmp8CC=t;Cyc_Tcutil_promote_array(_tmp8CC,(Cyc_Tcutil_addressof_props(e)).f2,0);});
Cyc_Tcutil_unchecked_cast(e,t,Cyc_Absyn_Other_coercion);}
# 2427
return t;}_LL0:;}}
# 2439 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp493=e->r;void*_stmttmp50=_tmp493;void*_tmp494=_stmttmp50;struct Cyc_Absyn_Exp*_tmp495;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp494)->tag == 12U){_LL1: _tmp495=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp494)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp495;
# 2443
Cyc_Tcexp_tcExpNoInst(te,topt,e2);
({void*_tmp8CD=Cyc_Absyn_pointer_expand((void*)_check_null(e2->topt),0);e2->topt=_tmp8CD;});
e->topt=e2->topt;
goto _LL0;}}else{_LL3: _LL4:
# 2449
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp8CE=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp8CE;});
# 2452
{void*_tmp496=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp51=_tmp496;void*_tmp497=_stmttmp51;void*_tmp49D;void*_tmp49C;void*_tmp49B;void*_tmp49A;struct Cyc_Absyn_Tqual _tmp499;void*_tmp498;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->tag == 3U){_LL6: _tmp498=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->f1).elt_type;_tmp499=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->f1).elt_tq;_tmp49A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->f1).ptr_atts).rgn;_tmp49B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->f1).ptr_atts).nullable;_tmp49C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->f1).ptr_atts).bounds;_tmp49D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->f1).ptr_atts).zero_term;_LL7: {void*t=_tmp498;struct Cyc_Absyn_Tqual tq=_tmp499;void*rt=_tmp49A;void*x=_tmp49B;void*b=_tmp49C;void*zt=_tmp49D;
# 2454
{void*_tmp49E=Cyc_Tcutil_compress(t);void*_stmttmp52=_tmp49E;void*_tmp49F=_stmttmp52;struct Cyc_List_List*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_List_List*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_List_List*_tmp4A8;struct Cyc_List_List*_tmp4A7;struct Cyc_Absyn_VarargInfo*_tmp4A6;int _tmp4A5;struct Cyc_List_List*_tmp4A4;void*_tmp4A3;struct Cyc_Absyn_Tqual _tmp4A2;void*_tmp4A1;struct Cyc_List_List*_tmp4A0;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->tag == 5U){_LLB: _tmp4A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).tvars;_tmp4A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).effect;_tmp4A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).ret_tqual;_tmp4A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).ret_type;_tmp4A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).args;_tmp4A5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).c_varargs;_tmp4A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).cyc_varargs;_tmp4A7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).rgn_po;_tmp4A8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).attributes;_tmp4A9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).requires_clause;_tmp4AA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).requires_relns;_tmp4AB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).ensures_clause;_tmp4AC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49F)->f1).ensures_relns;_LLC: {struct Cyc_List_List*tvs=_tmp4A0;void*eff=_tmp4A1;struct Cyc_Absyn_Tqual rtq=_tmp4A2;void*rtyp=_tmp4A3;struct Cyc_List_List*args=_tmp4A4;int c_varargs=_tmp4A5;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp4A6;struct Cyc_List_List*rpo=_tmp4A7;struct Cyc_List_List*atts=_tmp4A8;struct Cyc_Absyn_Exp*req=_tmp4A9;struct Cyc_List_List*req_relns=_tmp4AA;struct Cyc_Absyn_Exp*ens=_tmp4AB;struct Cyc_List_List*ens_relns=_tmp4AC;
# 2456
if(tvs != 0){
struct _tuple13 _tmp4AD=({struct _tuple13 _tmp622;({struct Cyc_List_List*_tmp8CF=Cyc_Tcenv_lookup_type_vars(te);_tmp622.f1=_tmp8CF;}),_tmp622.f2=Cyc_Core_heap_region;_tmp622;});struct _tuple13 env=_tmp4AD;
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tvs);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);
# 2464
rpo=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,rpo);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,rpo);{
void*ftyp=({struct Cyc_List_List*_tmp8D0=inst;Cyc_Tcutil_substitute(_tmp8D0,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));
_tmp4B2->tag=5U,(_tmp4B2->f1).tvars=0,(_tmp4B2->f1).effect=eff,(_tmp4B2->f1).ret_tqual=rtq,(_tmp4B2->f1).ret_type=rtyp,(_tmp4B2->f1).args=args,(_tmp4B2->f1).c_varargs=c_varargs,(_tmp4B2->f1).cyc_varargs=cyc_varargs,(_tmp4B2->f1).rgn_po=0,(_tmp4B2->f1).attributes=atts,(_tmp4B2->f1).requires_clause=req,(_tmp4B2->f1).requires_relns=req_relns,(_tmp4B2->f1).ensures_clause=ens,(_tmp4B2->f1).ensures_relns=ens_relns;_tmp4B2;}));});
# 2471
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4AE=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->tag=3U,(_tmp4B1->f1).elt_type=ftyp,(_tmp4B1->f1).elt_tq=tq,((_tmp4B1->f1).ptr_atts).rgn=rt,((_tmp4B1->f1).ptr_atts).nullable=x,((_tmp4B1->f1).ptr_atts).bounds=b,((_tmp4B1->f1).ptr_atts).zero_term=zt,((_tmp4B1->f1).ptr_atts).ptrloc=0;_tmp4B1;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_typ=_tmp4AE;
# 2473
struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp4AF;
({void*_tmp8D1=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->tag=13U,_tmp4B0->f1=inner,_tmp4B0->f2=ts;_tmp4B0;});e->r=_tmp8D1;});
e->topt=(void*)new_typ;}}
# 2477
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2480
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2483
goto _LL0;}_LL0:;}
# 2485
return(void*)_check_null(e->topt);}
# 2493
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2495
struct Cyc_List_List*_tmp4B3=0;struct Cyc_List_List*decls=_tmp4B3;
# 2497
{void*_tmp4B4=fn_exp->r;void*_stmttmp53=_tmp4B4;void*_tmp4B5=_stmttmp53;struct Cyc_List_List*_tmp4B6;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->tag == 10U){_LL1: _tmp4B6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2: {struct Cyc_List_List*es=_tmp4B6;
# 2499
{void*_tmp4B7=e->r;void*_stmttmp54=_tmp4B7;void*_tmp4B8=_stmttmp54;struct Cyc_List_List*_tmp4B9;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B8)->tag == 10U){_LL6: _tmp4B9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B8)->f2;_LL7: {struct Cyc_List_List*es2=_tmp4B9;
# 2501
struct Cyc_List_List*_tmp4BA=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp4BA;
int _tmp4BB=0;int arg_cnt=_tmp4BB;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp627;_tmp627.tag=0U,({struct _fat_ptr _tmp8D2=({const char*_tmp4C4="bad count ";_tag_fat(_tmp4C4,sizeof(char),11U);});_tmp627.f1=_tmp8D2;});_tmp627;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4BE=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp626;_tmp626.tag=11U,_tmp626.f1=arg_cnt;_tmp626;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp8D3=({const char*_tmp4C3="/";_tag_fat(_tmp4C3,sizeof(char),2U);});_tmp625.f1=_tmp8D3;});_tmp625;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4C0=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp624;_tmp624.tag=11U,_tmp624.f1=(int)arg_exps->hd;_tmp624;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp623;_tmp623.tag=0U,({
struct _fat_ptr _tmp8D4=({const char*_tmp4C2=" for alias coercion!";_tag_fat(_tmp4C2,sizeof(char),21U);});_tmp623.f1=_tmp8D4;});_tmp623;});void*_tmp4BC[5U];_tmp4BC[0]=& _tmp4BD,_tmp4BC[1]=& _tmp4BE,_tmp4BC[2]=& _tmp4BF,_tmp4BC[3]=& _tmp4C0,_tmp4BC[4]=& _tmp4C1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4BC,sizeof(void*),5U));});
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2513
struct _tuple12 _tmp4C5=({struct Cyc_Absyn_Exp*_tmp8D5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp8D5,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp55=_tmp4C5;struct _tuple12 _tmp4C6=_stmttmp55;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Decl*_tmp4C7;_LLB: _tmp4C7=_tmp4C6.f1;_tmp4C8=_tmp4C6.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp4C7;struct Cyc_Absyn_Exp*ve=_tmp4C8;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->hd=d,_tmp4C9->tl=decls;_tmp4C9;});
arg_exps=arg_exps->tl;}}}
# 2518
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp4CA=0U;({struct _fat_ptr _tmp8D6=({const char*_tmp4CB="not a function call!";_tag_fat(_tmp4CB,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8D6,_tag_fat(_tmp4CA,sizeof(void*),0U));});});}_LL5:;}
# 2521
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp4CC=0U;({struct _fat_ptr _tmp8D7=({const char*_tmp4CD="not a function call!";_tag_fat(_tmp4CD,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8D7,_tag_fat(_tmp4CC,sizeof(void*),0U));});});}_LL0:;}
# 2525
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp4CE=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp4CE;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp8DA=({struct Cyc_Absyn_Decl*_tmp8D9=d;struct Cyc_Absyn_Stmt*_tmp8D8=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp8D9,_tmp8D8,e->loc);});Cyc_Absyn_stmt_exp(_tmp8DA,e->loc);});
decls=decls->tl;}
# 2531
e->topt=0;
e->r=fn_exp->r;}
# 2536
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2540
{void*_tmp4CF=e->r;void*_stmttmp56=_tmp4CF;void*_tmp4D0=_stmttmp56;void*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;struct _fat_ptr*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D5;int*_tmp4DB;struct Cyc_Absyn_Exp**_tmp4DA;void***_tmp4D9;struct Cyc_Absyn_Exp**_tmp4D8;int*_tmp4D7;struct Cyc_Absyn_Enumfield*_tmp4DD;void*_tmp4DC;struct Cyc_Absyn_Enumfield*_tmp4DF;struct Cyc_Absyn_Enumdecl*_tmp4DE;struct Cyc_Absyn_Datatypefield*_tmp4E2;struct Cyc_Absyn_Datatypedecl*_tmp4E1;struct Cyc_List_List*_tmp4E0;struct Cyc_List_List*_tmp4E4;void*_tmp4E3;struct Cyc_Absyn_Aggrdecl**_tmp4E8;struct Cyc_List_List*_tmp4E7;struct Cyc_List_List**_tmp4E6;struct _tuple1**_tmp4E5;int*_tmp4EB;void*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;int*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Vardecl*_tmp4EC;struct Cyc_Absyn_Stmt*_tmp4F0;struct Cyc_List_List*_tmp4F1;struct Cyc_List_List*_tmp4F3;struct _tuple9*_tmp4F2;struct Cyc_List_List*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;int*_tmp4FA;int*_tmp4F9;struct _fat_ptr*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F7;int*_tmp4FE;int*_tmp4FD;struct _fat_ptr*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_List_List*_tmp501;void*_tmp500;void*_tmp502;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp**_tmp504;struct Cyc_Absyn_Exp*_tmp506;enum Cyc_Absyn_Coercion*_tmp509;struct Cyc_Absyn_Exp*_tmp508;void*_tmp507;struct Cyc_List_List*_tmp50B;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Core_Opt*_tmp517;struct Cyc_Absyn_Exp*_tmp516;enum Cyc_Absyn_Incrementor _tmp51A;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_List_List*_tmp51C;enum Cyc_Absyn_Primop _tmp51B;void**_tmp51D;union Cyc_Absyn_Cnst*_tmp51E;struct Cyc_List_List*_tmp520;struct Cyc_Core_Opt*_tmp51F;struct Cyc_Absyn_VarargCallInfo**_tmp523;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_Exp*_tmp521;int*_tmp527;struct Cyc_Absyn_VarargCallInfo**_tmp526;struct Cyc_List_List*_tmp525;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp528;switch(*((int*)_tmp4D0)){case 12U: _LL1: _tmp528=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp528;
# 2545
Cyc_Tcexp_tcExpNoInst(te,0,e2);
return;}case 10U: _LL3: _tmp524=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp525=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp526=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_tmp527=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D0)->f4;if(!(*_tmp527)){_LL4: {struct Cyc_Absyn_Exp*e1=_tmp524;struct Cyc_List_List*es=_tmp525;struct Cyc_Absyn_VarargCallInfo**vci=_tmp526;int*resolved=_tmp527;
# 2549
({void*_tmp529=0U;({struct _fat_ptr _tmp8DB=({const char*_tmp52A="unresolved function in tcExpNoInst";_tag_fat(_tmp52A,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8DB,_tag_fat(_tmp529,sizeof(void*),0U));});});}}else{_LL1B: _tmp521=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp522=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp523=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp521;struct Cyc_List_List*es=_tmp522;struct Cyc_Absyn_VarargCallInfo**vci=_tmp523;
# 2582
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp52B;_push_handler(& _tmp52B);{int _tmp52D=0;if(setjmp(_tmp52B.handler))_tmp52D=1;if(!_tmp52D){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp52C=(void*)Cyc_Core_get_exn_thrown();void*_tmp52E=_tmp52C;void*_tmp52F;if(((struct Cyc_Core_Failure_exn_struct*)_tmp52E)->tag == Cyc_Core_Failure){_LL58: _LL59:
# 2589
 ok=0;
fn_exp=e;
goto _LL57;}else{_LL5A: _tmp52F=_tmp52E;_LL5B: {void*exn=_tmp52F;(int)_rethrow(exn);}}_LL57:;}}}
# 2593
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2600
goto _LL0;}}case 36U: _LL5: _tmp51F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp520=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp51F;struct Cyc_List_List*des=_tmp520;
# 2553
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp51E=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp51E;
# 2558
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp51D=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LLA: {void**b=_tmp51D;
# 2560
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
# 2562
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp51B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp51C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp51B;struct Cyc_List_List*es=_tmp51C;
# 2564
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp519=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp51A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp519;enum Cyc_Absyn_Incrementor i=_tmp51A;
# 2566
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp516=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp517=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp518=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp516;struct Cyc_Core_Opt*popt=_tmp517;struct Cyc_Absyn_Exp*e2=_tmp518;
# 2568
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp513=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp514=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp515=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp513;struct Cyc_Absyn_Exp*e2=_tmp514;struct Cyc_Absyn_Exp*e3=_tmp515;
# 2570
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp511=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp512=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp511;struct Cyc_Absyn_Exp*e2=_tmp512;
# 2572
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp50F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp510=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp50F;struct Cyc_Absyn_Exp*e2=_tmp510;
# 2574
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp50D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp50E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp50D;struct Cyc_Absyn_Exp*e2=_tmp50E;
# 2576
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp50C=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp50C;
# 2602
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp50A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp50B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp50A;struct Cyc_List_List*ts=_tmp50B;
# 2604
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp507=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp508=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp509=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D0)->f4;_LL22: {void*t1=_tmp507;struct Cyc_Absyn_Exp*e1=_tmp508;enum Cyc_Absyn_Coercion*c=_tmp509;
# 2606
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp506=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp506;
# 2608
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp504=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp505=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp504;struct Cyc_Absyn_Exp*e1=_tmp505;
# 2610
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp503=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp503;
# 2612
void*_tmp530=Cyc_Tcexp_tcExpNoPromote(te,0,e1);void*t1=_tmp530;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 17U: _LL29: _tmp502=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL2A: {void*t1=_tmp502;
# 2615
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp500=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp501=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL2C: {void*t1=_tmp500;struct Cyc_List_List*l=_tmp501;
# 2617
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp4FF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp4FF;
# 2619
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp4FB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4FC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp4FD=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_tmp4FE=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D0)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp4FB;struct _fat_ptr*f=_tmp4FC;int*is_tagged=_tmp4FD;int*is_read=_tmp4FE;
# 2621
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp4F7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4F8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp4F9=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_tmp4FA=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D0)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp4F7;struct _fat_ptr*f=_tmp4F8;int*is_tagged=_tmp4F9;int*is_read=_tmp4FA;
# 2623
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp4F5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4F6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp4F5;struct Cyc_Absyn_Exp*e2=_tmp4F6;
# 2625
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp4F4=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL36: {struct Cyc_List_List*es=_tmp4F4;
# 2627
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp4F2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4F3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL38: {struct _tuple9*t1=_tmp4F2;struct Cyc_List_List*des=_tmp4F3;
# 2629
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp4F1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL3A: {struct Cyc_List_List*des=_tmp4F1;
# 2633
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp531=Cyc_Tcutil_compress(*topt);void*_stmttmp57=_tmp531;void*_tmp532=_stmttmp57;void*_tmp535;struct Cyc_Absyn_Tqual*_tmp534;void**_tmp533;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->tag == 4U){_LL5D: _tmp533=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).elt_type;_tmp534=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).tq;_tmp535=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).zero_term;_LL5E: {void**et=_tmp533;struct Cyc_Absyn_Tqual*etq=_tmp534;void*zt=_tmp535;
# 2639
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5C;}}else{_LL5F: _LL60:
 goto _LL5C;}_LL5C:;}
# 2645
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp4F0=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp4F0;
# 2647
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp4EC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4ED=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp4EE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_tmp4EF=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D0)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp4EC;struct Cyc_Absyn_Exp*e1=_tmp4ED;struct Cyc_Absyn_Exp*e2=_tmp4EE;int*iszeroterm=_tmp4EF;
# 2649
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp4E9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4EA=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp4EB=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp4E9;void*t1=_tmp4EA;int*iszeroterm=_tmp4EB;
# 2651
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp4E5=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4E6=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp4E7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_tmp4E8=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D0)->f4;_LL42: {struct _tuple1**tn=_tmp4E5;struct Cyc_List_List**ts=_tmp4E6;struct Cyc_List_List*args=_tmp4E7;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4E8;
# 2653
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp4E3=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4E4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL44: {void*ts=_tmp4E3;struct Cyc_List_List*args=_tmp4E4;
# 2655
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp4E0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4E1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_tmp4E2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D0)->f3;_LL46: {struct Cyc_List_List*es=_tmp4E0;struct Cyc_Absyn_Datatypedecl*tud=_tmp4E1;struct Cyc_Absyn_Datatypefield*tuf=_tmp4E2;
# 2657
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp4DE=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4DF=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp4DE;struct Cyc_Absyn_Enumfield*ef=_tmp4DF;
# 2659
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp4DC=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4DD=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL4A: {void*t2=_tmp4DC;struct Cyc_Absyn_Enumfield*ef=_tmp4DD;
# 2661
t=t2;goto _LL0;}case 34U: _LL4B: _tmp4D7=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1).is_calloc;_tmp4D8=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1).rgn;_tmp4D9=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1).elt_type;_tmp4DA=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1).num_elts;_tmp4DB=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1).fat_result;_LL4C: {int*is_calloc=_tmp4D7;struct Cyc_Absyn_Exp**ropt=_tmp4D8;void***t2=_tmp4D9;struct Cyc_Absyn_Exp**e2=_tmp4DA;int*isfat=_tmp4DB;
# 2663
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp4D5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4D6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp4D5;struct Cyc_Absyn_Exp*e2=_tmp4D6;
# 2665
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp4D3=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_tmp4D4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4D0)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp4D3;struct _fat_ptr*f=_tmp4D4;
# 2667
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp4D2=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp4D2;
# 2669
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 39U: _LL53: _tmp4D1=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4D0)->f1;_LL54: {void*t2=_tmp4D1;
# 2671
if(!te->allow_valueof)
({void*_tmp536=0U;({unsigned _tmp8DD=e->loc;struct _fat_ptr _tmp8DC=({const char*_tmp537="valueof(-) can only occur within types";_tag_fat(_tmp537,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp8DD,_tmp8DC,_tag_fat(_tmp536,sizeof(void*),0U));});});
# 2679
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL55: _LL56:
# 2683
 t=Cyc_Absyn_void_type;
goto _LL0;}_LL0:;}
# 2686
e->topt=t;}

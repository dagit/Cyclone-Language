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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 305
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 312
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 493
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 495
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 887 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 904
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 906
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 909
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 911
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 913
extern void*Cyc_Absyn_double_type;void*Cyc_Absyn_wchar_type();
void*Cyc_Absyn_gen_float_type(unsigned i);
# 916
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 920
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 922
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 939
void*Cyc_Absyn_exn_type();
# 947
extern void*Cyc_Absyn_fat_bound_type;
# 949
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 951
void*Cyc_Absyn_bounds_one();
# 953
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 957
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 959
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 961
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 965
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 973
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
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
# 1055
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1120
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1122
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1124
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1127
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1129
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FD;_tmp4FD.tag=0U,({struct _fat_ptr _tmp5F7=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp4FD.f1=_tmp5F7;});_tmp4FD;});void*_tmp0[1U];_tmp0[0]=& _tmp1;({unsigned _tmp5F8=loc;Cyc_Warn_err2(_tmp5F8,_tag_fat(_tmp0,sizeof(void*),1U));});});}
# 64
static struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp3;
void*_tmp4=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->v=tenv_tvs;_tmpA;}));void*t2=_tmp4;
void*_tmp5=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->v=tenv_tvs;_tmp9;}));void*rt=_tmp5;
void*_tmp6=Cyc_Tcutil_any_bool(tenv_tvs);void*nbl=_tmp6;
void*_tmp7=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp7;
void*_tmp8=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp8;
return({struct Cyc_Absyn_PtrInfo _tmp4FE;_tmp4FE.elt_type=t2,({struct Cyc_Absyn_Tqual _tmp5F9=Cyc_Absyn_empty_tqual(0U);_tmp4FE.elt_tq=_tmp5F9;}),(_tmp4FE.ptr_atts).rgn=rt,(_tmp4FE.ptr_atts).nullable=nbl,(_tmp4FE.ptr_atts).bounds=b,(_tmp4FE.ptr_atts).zero_term=zt,(_tmp4FE.ptr_atts).ptrloc=0;_tmp4FE;});}
# 74
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 78
if(topt == 0){
# 80
({void*_tmp5FA=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp5FA;});
return;}{
# 83
void*t=*topt;
void*_tmpC=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmpC;void*_tmpD=_stmttmp0;struct Cyc_Absyn_Tqual _tmpF;void*_tmpE;union Cyc_Absyn_AggrInfo _tmp10;switch(*((int*)_tmpD)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 20U){_LL1: _tmp10=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp10;
# 86
{union Cyc_Absyn_AggrInfo _tmp11=info;struct Cyc_Absyn_Aggrdecl*_tmp12;if((_tmp11.UnknownAggr).tag == 1){_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF;_tmp4FF.tag=0U,({struct _fat_ptr _tmp5FB=({const char*_tmp15="struct type improperly set";_tag_fat(_tmp15,sizeof(char),27U);});_tmp4FF.f1=_tmp5FB;});_tmp4FF;});void*_tmp13[1U];_tmp13[0]=& _tmp14;({unsigned _tmp5FC=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp5FC,_tag_fat(_tmp13,sizeof(void*),1U));});});}else{_LLC: _tmp12=*(_tmp11.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmp12;
({void*_tmp5FD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=29U,_tmp16->f1=ad->name,_tmp16->f2=0,_tmp16->f3=des,_tmp16->f4=ad;_tmp16;});e->r=_tmp5FD;});}}_LL9:;}
# 90
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).elt_type;_tmpF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).tq;_LL4: {void*at=_tmpE;struct Cyc_Absyn_Tqual aq=_tmpF;
({void*_tmp5FE=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=26U,_tmp17->f1=des;_tmp17;});e->r=_tmp5FE;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp5FF=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=30U,_tmp18->f1=t,_tmp18->f2=des;_tmp18;});e->r=_tmp5FF;});goto _LL0;default: _LL7: _LL8:
({void*_tmp600=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=26U,_tmp19->f1=des;_tmp19;});e->r=_tmp600;});goto _LL0;}_LL0:;}}
# 100
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 104
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp1A=e->r;void*_stmttmp1=_tmp1A;void*_tmp1B=_stmttmp1;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->f2 == 0){_LL1: _LL2:
# 107
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp500;_tmp500.tag=0U,({struct _fat_ptr _tmp601=({const char*_tmp1E="assignment in test";_tag_fat(_tmp1E,sizeof(char),19U);});_tmp500.f1=_tmp601;});_tmp500;});void*_tmp1C[1U];_tmp1C[0]=& _tmp1D;({unsigned _tmp602=e->loc;Cyc_Warn_warn2(_tmp602,_tag_fat(_tmp1C,sizeof(void*),1U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 115
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp505;_tmp505.tag=0U,({struct _fat_ptr _tmp603=({const char*_tmp27="test of ";_tag_fat(_tmp27,sizeof(char),9U);});_tmp505.f1=_tmp603;});_tmp505;});struct Cyc_Warn_String_Warn_Warg_struct _tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _tmp504;_tmp504.tag=0U,_tmp504.f1=msg_part;_tmp504;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp503;_tmp503.tag=0U,({struct _fat_ptr _tmp604=({const char*_tmp26=" has type ";_tag_fat(_tmp26,sizeof(char),11U);});_tmp503.f1=_tmp604;});_tmp503;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp23=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp502;_tmp502.tag=3U,_tmp502.f1=(void*)e->topt;_tmp502;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24=({struct Cyc_Warn_String_Warn_Warg_struct _tmp501;_tmp501.tag=0U,({
struct _fat_ptr _tmp605=({const char*_tmp25=" instead of integral or pointer type";_tag_fat(_tmp25,sizeof(char),37U);});_tmp501.f1=_tmp605;});_tmp501;});void*_tmp1F[5U];_tmp1F[0]=& _tmp20,_tmp1F[1]=& _tmp21,_tmp1F[2]=& _tmp22,_tmp1F[3]=& _tmp23,_tmp1F[4]=& _tmp24;({unsigned _tmp606=e->loc;Cyc_Warn_err2(_tmp606,_tag_fat(_tmp1F,sizeof(void*),5U));});});}
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
({union Cyc_Absyn_Cnst _tmp607=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp607;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _LL23: _tmp37=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp37;
# 174
({union Cyc_Absyn_Cnst _tmp608=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp608;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _LL25: _tmp36=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL26: {enum Cyc_Absyn_Sign sn=_tmp36;
_tmp35=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _LL27: _tmp35=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp35;
# 180
({union Cyc_Absyn_Cnst _tmp609=Cyc_Absyn_Int_c(sn,i);*c=_tmp609;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2 != 0){_LL2B: _tmp34=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2)->hd;_LL2C: {void*t1=_tmp34;
# 189
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=9U,({struct Cyc_Absyn_Exp*_tmp60A=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp39->f1=_tmp60A;});_tmp39;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3U: _LL29: if(i == 0){_LL2A: {
# 184
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp60B=({union Cyc_Absyn_Cnst _tmp506;(_tmp506.Null_c).tag=1U,(_tmp506.Null_c).val=0;_tmp506;});*c=_tmp60B;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}}else{goto _LL2D;}default: _LL2D: _LL2E:
# 191
 return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL20:;}}case 1U: _LL15: _LL16:
# 194
 if(topt != 0){
void*_tmp3A=Cyc_Tcutil_compress(*topt);void*_stmttmp5=_tmp3A;void*_tmp3B=_stmttmp5;void*_tmp3C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->tag == 3U){_LL30: _tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->f1).ptr_atts).nullable;_LL31: {void*nbl=_tmp3C;
# 197
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp508;_tmp508.tag=0U,({struct _fat_ptr _tmp60C=({const char*_tmp40="Used NULL when expecting a value of type ";_tag_fat(_tmp40,sizeof(char),42U);});_tmp508.f1=_tmp60C;});_tmp508;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp507;_tmp507.tag=2U,_tmp507.f1=(void*)*topt;_tmp507;});void*_tmp3D[2U];_tmp3D[0]=& _tmp3E,_tmp3D[1]=& _tmp3F;({unsigned _tmp60D=e->loc;Cyc_Warn_err2(_tmp60D,_tag_fat(_tmp3D,sizeof(void*),2U));});});
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
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp60E=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp60E,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 219
void*_tmp42=({void*_tmp612=string_elt_typ;void*_tmp611=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp610=Cyc_Absyn_const_tqual(0U);void*_tmp60F=
Cyc_Absyn_thin_bounds_exp(elen);
# 219
Cyc_Absyn_atb_type(_tmp612,_tmp611,_tmp610,_tmp60F,Cyc_Absyn_true_type);});void*t=_tmp42;
# 221
if(topt == 0)
return t;{
void*_tmp43=Cyc_Tcutil_compress(*topt);void*_stmttmp6=_tmp43;void*_tmp44=_stmttmp6;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 4U: _LL35: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp45;
# 227
return({void*_tmp615=string_elt_typ;struct Cyc_Absyn_Tqual _tmp614=tq;struct Cyc_Absyn_Exp*_tmp613=elen;Cyc_Absyn_array_type(_tmp615,_tmp614,_tmp613,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL37: _LL38:
# 232
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp618=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp617=loc;void*_tmp616=t;Cyc_Tcutil_silent_castable(_tmp618,_tmp617,_tmp616,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
return*topt;}
# 237
return t;default: _LL39: _LL3A:
 return t;}_LL34:;}}}}}_LL0:;}
# 244
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp46=*((void**)_check_null(b));void*_stmttmp7=_tmp46;void*_tmp47=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp48;struct Cyc_Absyn_Vardecl*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4A;struct Cyc_Absyn_Fndecl*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;struct _tuple1*_tmp4D;switch(*((int*)_tmp47)){case 0U: _LL1: _tmp4D=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp47)->f1;_LL2: {struct _tuple1*q=_tmp4D;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp509;_tmp509.tag=0U,({struct _fat_ptr _tmp619=({const char*_tmp50="unresolved binding in tcVar";_tag_fat(_tmp50,sizeof(char),28U);});_tmp509.f1=_tmp619;});_tmp509;});void*_tmp4E[1U];_tmp4E[0]=& _tmp4F;({unsigned _tmp61A=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp61A,_tag_fat(_tmp4E,sizeof(void*),1U));});});}case 1U: _LL3: _tmp4C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp47)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp4C;
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
({void*_tmp61B=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=39U,_tmp54->f1=i;_tmp54;});e->r=_tmp61B;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
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
if(!({struct Cyc_RgnOrder_RgnPO*_tmp61D=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp61C=e;Cyc_Tcutil_coerce_arg(_tmp61D,_tmp61C,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50D;_tmp50D.tag=0U,({struct _fat_ptr _tmp61E=({const char*_tmp61="descriptor has type ";_tag_fat(_tmp61,sizeof(char),21U);});_tmp50D.f1=_tmp61E;});_tmp50D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp50C;_tmp50C.tag=2U,_tmp50C.f1=(void*)t;_tmp50C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50B;_tmp50B.tag=0U,({
struct _fat_ptr _tmp61F=({const char*_tmp60=" but argument has type ";_tag_fat(_tmp60,sizeof(char),24U);});_tmp50B.f1=_tmp61F;});_tmp50B;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp50A;_tmp50A.tag=3U,_tmp50A.f1=(void*)e->topt;_tmp50A;});void*_tmp5B[4U];_tmp5B[0]=& _tmp5C,_tmp5B[1]=& _tmp5D,_tmp5B[2]=& _tmp5E,_tmp5B[3]=& _tmp5F;({unsigned _tmp620=e->loc;Cyc_Tcexp_err_and_explain(_tmp620,_tag_fat(_tmp5B,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp621=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=(void*)arg_cnt,_tmp62->tl=*alias_arg_exps;_tmp62;});*alias_arg_exps=_tmp621;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(((struct Cyc_Absyn_Exp*)args->hd)->loc);}
# 302
if(desc_types != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50E;_tmp50E.tag=0U,({struct _fat_ptr _tmp622=({const char*_tmp65="too few arguments";_tag_fat(_tmp65,sizeof(char),18U);});_tmp50E.f1=_tmp622;});_tmp50E;});void*_tmp63[1U];_tmp63[0]=& _tmp64;({unsigned _tmp623=fmt->loc;Cyc_Warn_err2(_tmp623,_tag_fat(_tmp63,sizeof(void*),1U));});});
if(args != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50F;_tmp50F.tag=0U,({struct _fat_ptr _tmp624=({const char*_tmp68="too many arguments";_tag_fat(_tmp68,sizeof(char),19U);});_tmp50F.f1=_tmp624;});_tmp50F;});void*_tmp66[1U];_tmp66[0]=& _tmp67;({unsigned _tmp625=((struct Cyc_Absyn_Exp*)args->hd)->loc;Cyc_Warn_err2(_tmp625,_tag_fat(_tmp66,sizeof(void*),1U));});});
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp511;_tmp511.tag=0U,({struct _fat_ptr _tmp626=({const char*_tmp6E="expecting numeric type but found ";_tag_fat(_tmp6E,sizeof(char),34U);});_tmp511.f1=_tmp626;});_tmp511;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp510;_tmp510.tag=2U,_tmp510.f1=(void*)t;_tmp510;});void*_tmp6B[2U];_tmp6B[0]=& _tmp6C,_tmp6B[1]=& _tmp6D;({unsigned _tmp627=loc;Cyc_Warn_err2(_tmp627,_tag_fat(_tmp6B,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 336
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp513;_tmp513.tag=0U,({struct _fat_ptr _tmp628=({const char*_tmp72="expecting integral or * type but found ";_tag_fat(_tmp72,sizeof(char),40U);});_tmp513.f1=_tmp628;});_tmp513;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp71=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp512;_tmp512.tag=2U,_tmp512.f1=(void*)t;_tmp512;});void*_tmp6F[2U];_tmp6F[0]=& _tmp70,_tmp6F[1]=& _tmp71;({unsigned _tmp629=loc;Cyc_Warn_err2(_tmp629,_tag_fat(_tmp6F,sizeof(void*),2U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 341
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmp515;_tmp515.tag=0U,({struct _fat_ptr _tmp62A=({const char*_tmp76="expecting integral type but found ";_tag_fat(_tmp76,sizeof(char),35U);});_tmp515.f1=_tmp62A;});_tmp515;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp75=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp514;_tmp514.tag=2U,_tmp514.f1=(void*)t;_tmp514;});void*_tmp73[2U];_tmp73[0]=& _tmp74,_tmp73[1]=& _tmp75;({unsigned _tmp62B=loc;Cyc_Warn_err2(_tmp62B,_tag_fat(_tmp73,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 345
{void*_tmp77=t;void*_tmp78;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->tag == 3U){_LLE: _tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp78;
# 347
struct Cyc_Absyn_Exp*_tmp79=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp79;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp516;_tmp516.tag=0U,({struct _fat_ptr _tmp62C=({const char*_tmp7C="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp7C,sizeof(char),53U);});_tmp516.f1=_tmp62C;});_tmp516;});void*_tmp7A[1U];_tmp7A[0]=& _tmp7B;({unsigned _tmp62D=loc;Cyc_Warn_err2(_tmp62D,_tag_fat(_tmp7A,sizeof(void*),1U));});});
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp518;_tmp518.tag=0U,({struct _fat_ptr _tmp62E=({const char*_tmp80="numelts requires pointer type, not ";_tag_fat(_tmp80,sizeof(char),36U);});_tmp518.f1=_tmp62E;});_tmp518;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp517;_tmp517.tag=2U,_tmp517.f1=(void*)t;_tmp517;});void*_tmp7D[2U];_tmp7D[0]=& _tmp7E,_tmp7D[1]=& _tmp7F;({unsigned _tmp62F=loc;Cyc_Warn_err2(_tmp62F,_tag_fat(_tmp7D,sizeof(void*),2U));});});}_LLD:;}
# 353
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp82=({struct Cyc_Warn_String_Warn_Warg_struct _tmp519;_tmp519.tag=0U,({struct _fat_ptr _tmp630=({const char*_tmp83="Non-unary primop";_tag_fat(_tmp83,sizeof(char),17U);});_tmp519.f1=_tmp630;});_tmp519;});void*_tmp81[1U];_tmp81[0]=& _tmp82;({unsigned _tmp631=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp631,_tag_fat(_tmp81,sizeof(void*),1U));});});}_LL0:;}
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
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51C;_tmp51C.tag=0U,({struct _fat_ptr _tmp632=({const char*_tmp8C="type ";_tag_fat(_tmp8C,sizeof(char),6U);});_tmp51C.f1=_tmp632;});_tmp51C;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp89=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp51B;_tmp51B.tag=3U,_tmp51B.f1=(void*)e1->topt;_tmp51B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51A;_tmp51A.tag=0U,({struct _fat_ptr _tmp633=({const char*_tmp8B=" cannot be used here";_tag_fat(_tmp8B,sizeof(char),21U);});_tmp51A.f1=_tmp633;});_tmp51A;});void*_tmp87[3U];_tmp87[0]=& _tmp88,_tmp87[1]=& _tmp89,_tmp87[2]=& _tmp8A;({struct Cyc_Tcenv_Tenv*_tmp635=te;unsigned _tmp634=e1->loc;Cyc_Tcexp_expr_err(_tmp635,_tmp634,0,_tag_fat(_tmp87,sizeof(void*),3U));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp8E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51F;_tmp51F.tag=0U,({struct _fat_ptr _tmp636=({const char*_tmp92="type ";_tag_fat(_tmp92,sizeof(char),6U);});_tmp51F.f1=_tmp636;});_tmp51F;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp51E;_tmp51E.tag=3U,_tmp51E.f1=(void*)e2->topt;_tmp51E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51D;_tmp51D.tag=0U,({struct _fat_ptr _tmp637=({const char*_tmp91=" cannot be used here";_tag_fat(_tmp91,sizeof(char),21U);});_tmp51D.f1=_tmp637;});_tmp51D;});void*_tmp8D[3U];_tmp8D[0]=& _tmp8E,_tmp8D[1]=& _tmp8F,_tmp8D[2]=& _tmp90;({struct Cyc_Tcenv_Tenv*_tmp639=te;unsigned _tmp638=e2->loc;Cyc_Tcexp_expr_err(_tmp639,_tmp638,0,_tag_fat(_tmp8D,sizeof(void*),3U));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 380
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp93=t1;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;struct Cyc_Absyn_Tqual _tmp95;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->tag == 3U){_LL1: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_type;_tmp95=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_tq;_tmp96=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).rgn;_tmp97=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).nullable;_tmp98=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).bounds;_tmp99=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).zero_term;_LL2: {void*et=_tmp94;struct Cyc_Absyn_Tqual tq=_tmp95;void*r=_tmp96;void*n=_tmp97;void*b=_tmp98;void*zt=_tmp99;
# 385
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp520;_tmp520.tag=0U,({struct _fat_ptr _tmp63A=({const char*_tmp9C="can't do arithmetic on abstract pointer type";_tag_fat(_tmp9C,sizeof(char),45U);});_tmp520.f1=_tmp63A;});_tmp520;});void*_tmp9A[1U];_tmp9A[0]=& _tmp9B;({unsigned _tmp63B=e1->loc;Cyc_Warn_err2(_tmp63B,_tag_fat(_tmp9A,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp521;_tmp521.tag=0U,({struct _fat_ptr _tmp63C=({const char*_tmp9F="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmp9F,sizeof(char),49U);});_tmp521.f1=_tmp63C;});_tmp521;});void*_tmp9D[1U];_tmp9D[0]=& _tmp9E;({unsigned _tmp63D=e1->loc;Cyc_Warn_err2(_tmp63D,_tag_fat(_tmp9D,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp523;_tmp523.tag=0U,({struct _fat_ptr _tmp63E=({const char*_tmpA3="expecting int but found ";_tag_fat(_tmpA3,sizeof(char),25U);});_tmp523.f1=_tmp63E;});_tmp523;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp522;_tmp522.tag=2U,_tmp522.f1=(void*)t2;_tmp522;});void*_tmpA0[2U];_tmpA0[0]=& _tmpA1,_tmpA0[1]=& _tmpA2;({unsigned _tmp63F=e2->loc;Cyc_Warn_err2(_tmp63F,_tag_fat(_tmpA0,sizeof(void*),2U));});});{
struct Cyc_Absyn_Exp*_tmpA4=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmpA4;
if(eopt == 0)
return t1;
# 396
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmpA5=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmpA5;struct _tuple16 _tmpA6=_stmttmpA;int _tmpA8;unsigned _tmpA7;_LL6: _tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_LL7: {unsigned i=_tmpA7;int known=_tmpA8;
if(known && i == (unsigned)1)
({void*_tmpA9=0U;({unsigned _tmp641=e1->loc;struct _fat_ptr _tmp640=({const char*_tmpAA="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpAA,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp641,_tmp640,_tag_fat(_tmpA9,sizeof(void*),0U));});});}}{
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp529;_tmp529.tag=0U,({struct _fat_ptr _tmp642=({const char*_tmpB7="pointer arithmetic on values of different ";_tag_fat(_tmpB7,sizeof(char),43U);});_tmp529.f1=_tmp642;});_tmp529;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp528;_tmp528.tag=0U,({
struct _fat_ptr _tmp643=({const char*_tmpB6="types (";_tag_fat(_tmpB6,sizeof(char),8U);});_tmp528.f1=_tmp643;});_tmp528;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp527;_tmp527.tag=2U,_tmp527.f1=(void*)t1;_tmp527;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp526;_tmp526.tag=0U,({struct _fat_ptr _tmp644=({const char*_tmpB5=" != ";_tag_fat(_tmpB5,sizeof(char),5U);});_tmp526.f1=_tmp644;});_tmp526;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp525;_tmp525.tag=2U,_tmp525.f1=(void*)t2;_tmp525;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp524;_tmp524.tag=0U,({struct _fat_ptr _tmp645=({const char*_tmpB4=")";_tag_fat(_tmpB4,sizeof(char),2U);});_tmp524.f1=_tmp645;});_tmp524;});void*_tmpAD[6U];_tmpAD[0]=& _tmpAE,_tmpAD[1]=& _tmpAF,_tmpAD[2]=& _tmpB0,_tmpAD[3]=& _tmpB1,_tmpAD[4]=& _tmpB2,_tmpAD[5]=& _tmpB3;({unsigned _tmp646=e1->loc;Cyc_Tcexp_err_and_explain(_tmp646,_tag_fat(_tmpAD,sizeof(void*),6U));});});
return Cyc_Absyn_sint_type;}
# 429
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp647=t1_elt;Cyc_Unify_unify(_tmp647,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52F;_tmp52F.tag=0U,({struct _fat_ptr _tmp648=({const char*_tmpC2="pointer arithmetic on values of different ";_tag_fat(_tmpC2,sizeof(char),43U);});_tmp52F.f1=_tmp648;});_tmp52F;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52E;_tmp52E.tag=0U,({
struct _fat_ptr _tmp649=({const char*_tmpC1="types(";_tag_fat(_tmpC1,sizeof(char),7U);});_tmp52E.f1=_tmp649;});_tmp52E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52D;_tmp52D.tag=2U,_tmp52D.f1=(void*)t1;_tmp52D;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52C;_tmp52C.tag=0U,({struct _fat_ptr _tmp64A=({const char*_tmpC0=" != ";_tag_fat(_tmpC0,sizeof(char),5U);});_tmp52C.f1=_tmp64A;});_tmp52C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52B;_tmp52B.tag=2U,_tmp52B.f1=(void*)t2;_tmp52B;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52A;_tmp52A.tag=0U,({struct _fat_ptr _tmp64B=({const char*_tmpBF=")";_tag_fat(_tmpBF,sizeof(char),2U);});_tmp52A.f1=_tmp64B;});_tmp52A;});void*_tmpB8[6U];_tmpB8[0]=& _tmpB9,_tmpB8[1]=& _tmpBA,_tmpB8[2]=& _tmpBB,_tmpB8[3]=& _tmpBC,_tmpB8[4]=& _tmpBD,_tmpB8[5]=& _tmpBE;({unsigned _tmp64C=e1->loc;Cyc_Tcexp_err_and_explain(_tmp64C,_tag_fat(_tmpB8,sizeof(void*),6U));});});
# 434
({void*_tmpC3=0U;({unsigned _tmp64E=e1->loc;struct _fat_ptr _tmp64D=({const char*_tmpC4="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpC4,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp64E,_tmp64D,_tag_fat(_tmpC3,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp652=e1;Cyc_Tcutil_unchecked_cast(_tmp652,({void*_tmp651=t1_elt;void*_tmp650=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp64F=
Cyc_Absyn_empty_tqual(0U);
# 435
Cyc_Absyn_star_type(_tmp651,_tmp650,_tmp64F,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 438
return Cyc_Absyn_sint_type;}
# 440
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpC5=0U;({unsigned _tmp654=e1->loc;struct _fat_ptr _tmp653=({const char*_tmpC6="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpC6,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp654,_tmp653,_tag_fat(_tmpC5,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpC7=0U;({unsigned _tmp656=e1->loc;struct _fat_ptr _tmp655=({const char*_tmpC8="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpC8,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp656,_tmp655,_tag_fat(_tmpC7,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp533;_tmp533.tag=0U,({struct _fat_ptr _tmp657=({const char*_tmpCF="expecting either ";_tag_fat(_tmpCF,sizeof(char),18U);});_tmp533.f1=_tmp657;});_tmp533;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp532;_tmp532.tag=2U,_tmp532.f1=(void*)t1;_tmp532;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp531;_tmp531.tag=0U,({struct _fat_ptr _tmp658=({const char*_tmpCE=" or int but found ";_tag_fat(_tmpCE,sizeof(char),19U);});_tmp531.f1=_tmp658;});_tmp531;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp530;_tmp530.tag=2U,_tmp530.f1=(void*)t2;_tmp530;});void*_tmpC9[4U];_tmpC9[0]=& _tmpCA,_tmpC9[1]=& _tmpCB,_tmpC9[2]=& _tmpCC,_tmpC9[3]=& _tmpCD;({unsigned _tmp659=e2->loc;Cyc_Tcexp_err_and_explain(_tmp659,_tag_fat(_tmpC9,sizeof(void*),4U));});});
return t1;}
# 449
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp65A=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp65A,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({void*_tmpD0=0U;({unsigned _tmp65C=e1->loc;struct _fat_ptr _tmp65B=({const char*_tmpD1="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpD1,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp65C,_tmp65B,_tag_fat(_tmpD0,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp660=e2;Cyc_Tcutil_unchecked_cast(_tmp660,({void*_tmp65F=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp65E=
Cyc_Tcutil_pointer_region(t2);
# 454
struct Cyc_Absyn_Tqual _tmp65D=
# 456
Cyc_Absyn_empty_tqual(0U);
# 454
Cyc_Absyn_star_type(_tmp65F,_tmp65E,_tmp65D,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 459
({void*_tmpD2=0U;({unsigned _tmp662=e1->loc;struct _fat_ptr _tmp661=({const char*_tmpD3="thin pointer subtraction!";_tag_fat(_tmpD3,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp662,_tmp661,_tag_fat(_tmpD2,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 462
({void*_tmpD4=0U;({unsigned _tmp664=e1->loc;struct _fat_ptr _tmp663=({const char*_tmpD5="coercing thin pointer to integer to support subtraction";_tag_fat(_tmpD5,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp664,_tmp663,_tag_fat(_tmpD4,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 465
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpD6=0U;({unsigned _tmp666=e1->loc;struct _fat_ptr _tmp665=({const char*_tmpD7="coercing pointer to integer to support subtraction";_tag_fat(_tmpD7,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp666,_tmp665,_tag_fat(_tmpD6,sizeof(void*),0U));});});
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
void*_tmp667=t1;Cyc_Unify_unify(_tmp667,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 486
if(({struct Cyc_RgnOrder_RgnPO*_tmp66A=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp669=loc;void*_tmp668=t2;Cyc_Tcutil_silent_castable(_tmp66A,_tmp669,_tmp668,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 490
if(({struct Cyc_RgnOrder_RgnPO*_tmp66D=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp66C=loc;void*_tmp66B=t1;Cyc_Tcutil_silent_castable(_tmp66D,_tmp66C,_tmp66B,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 494
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 499
{struct _tuple0 _tmpD8=({struct _tuple0 _tmp534;({void*_tmp66F=Cyc_Tcutil_compress(t1);_tmp534.f1=_tmp66F;}),({void*_tmp66E=Cyc_Tcutil_compress(t2);_tmp534.f2=_tmp66E;});_tmp534;});struct _tuple0 _stmttmpB=_tmpD8;struct _tuple0 _tmpD9=_stmttmpB;void*_tmpDB;void*_tmpDA;switch(*((int*)_tmpD9.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9.f2)->tag == 3U){_LL1: _tmpDA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9.f1)->f1).elt_type;_tmpDB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9.f2)->f1).elt_type;_LL2: {void*t1a=_tmpDA;void*t2a=_tmpDB;
# 501
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD9.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD9.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD9.f2)->f1)->tag == 3U){_LL3: _LL4:
# 505
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 509
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp538;_tmp538.tag=0U,({struct _fat_ptr _tmp670=({const char*_tmpE2="comparison not allowed between ";_tag_fat(_tmpE2,sizeof(char),32U);});_tmp538.f1=_tmp670;});_tmp538;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp537;_tmp537.tag=2U,_tmp537.f1=(void*)t1;_tmp537;});struct Cyc_Warn_String_Warn_Warg_struct _tmpDF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp536;_tmp536.tag=0U,({struct _fat_ptr _tmp671=({const char*_tmpE1=" and ";_tag_fat(_tmpE1,sizeof(char),6U);});_tmp536.f1=_tmp671;});_tmp536;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp535;_tmp535.tag=2U,_tmp535.f1=(void*)t2;_tmp535;});void*_tmpDC[4U];_tmpDC[0]=& _tmpDD,_tmpDC[1]=& _tmpDE,_tmpDC[2]=& _tmpDF,_tmpDC[3]=& _tmpE0;({unsigned _tmp672=loc;Cyc_Tcexp_err_and_explain(_tmp672,_tag_fat(_tmpDC,sizeof(void*),4U));});});
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp539;_tmp539.tag=0U,({struct _fat_ptr _tmp673=({const char*_tmpE6="bad binary primop";_tag_fat(_tmpE6,sizeof(char),18U);});_tmp539.f1=_tmp673;});_tmp539;});void*_tmpE4[1U];_tmpE4[0]=& _tmpE5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpE4,sizeof(void*),1U));});}_LL0:;}
# 545
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 553
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpE7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpE7;
void*_tmpE8=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpE8;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpEA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B;_tmp53B.tag=0U,({struct _fat_ptr _tmp674=({const char*_tmpEC="expecting numeric type but found ";_tag_fat(_tmpEC,sizeof(char),34U);});_tmp53B.f1=_tmp674;});_tmp53B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53A;_tmp53A.tag=2U,_tmp53A.f1=(void*)t;_tmp53A;});void*_tmpE9[2U];_tmpE9[0]=& _tmpEA,_tmpE9[1]=& _tmpEB;({unsigned _tmp675=e->loc;Cyc_Warn_err2(_tmp675,_tag_fat(_tmpE9,sizeof(void*),2U));});});
return t;}
# 560
{struct Cyc_List_List*_tmpED=es;struct Cyc_List_List*es2=_tmpED;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _tmpEE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpEE;int _tmpEF=_stmttmpC;switch(_tmpEF){case 0U: _LL1: _LL2:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53C;_tmp53C.tag=0U,({struct _fat_ptr _tmp676=({const char*_tmpF2="primitive operator has 0 arguments";_tag_fat(_tmpF2,sizeof(char),35U);});_tmp53C.f1=_tmp676;});_tmp53C;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF1;({struct Cyc_Tcenv_Tenv*_tmp679=te;unsigned _tmp678=loc;void**_tmp677=topt;Cyc_Tcexp_expr_err(_tmp679,_tmp678,_tmp677,_tag_fat(_tmpF0,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);case 2U: _LL5: _LL6:
 return({struct Cyc_Tcenv_Tenv*_tmp67E=te;unsigned _tmp67D=loc;void**_tmp67C=topt;enum Cyc_Absyn_Primop _tmp67B=p;struct Cyc_Absyn_Exp*_tmp67A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp67E,_tmp67D,_tmp67C,_tmp67B,_tmp67A,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53D;_tmp53D.tag=0U,({struct _fat_ptr _tmp67F=({const char*_tmpF5="primitive operator has > 2 arguments";_tag_fat(_tmpF5,sizeof(char),37U);});_tmp53D.f1=_tmp67F;});_tmp53D;});void*_tmpF3[1U];_tmpF3[0]=& _tmpF4;({struct Cyc_Tcenv_Tenv*_tmp682=te;unsigned _tmp681=loc;void**_tmp680=topt;Cyc_Tcexp_expr_err(_tmp682,_tmp681,_tmp680,_tag_fat(_tmpF3,sizeof(void*),1U));});});}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 570
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);void*t=_tmpF6;
void*_tmpF7=t;struct Cyc_List_List*_tmpF8;struct Cyc_Absyn_Tqual _tmpFA;void*_tmpF9;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Datatypefield*_tmpFC;struct Cyc_Absyn_Aggrdecl*_tmpFD;switch(*((int*)_tmpF7)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpFD=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmpFD;
# 574
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpFF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53F;_tmp53F.tag=0U,({struct _fat_ptr _tmp683=({const char*_tmp101="attempt to write abstract ";_tag_fat(_tmp101,sizeof(char),27U);});_tmp53F.f1=_tmp683;});_tmp53F;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp100=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp53E;_tmp53E.tag=6U,_tmp53E.f1=ad;_tmp53E;});void*_tmpFE[2U];_tmpFE[0]=& _tmpFF,_tmpFE[1]=& _tmp100;({unsigned _tmp684=loc;Cyc_Warn_err2(_tmp684,_tag_fat(_tmpFE,sizeof(void*),2U));});});
return 0;}
# 578
_tmpFB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpFC=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmpFC;
# 590
{struct Cyc_List_List*_tmp107=df->typs;struct Cyc_List_List*fs=_tmp107;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp108=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmp108;struct _tuple17*_tmp109=_stmttmpD;void*_tmp10B;struct Cyc_Absyn_Tqual _tmp10A;_LLE: _tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp10A;void*t=_tmp10B;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp542;_tmp542.tag=0U,({struct _fat_ptr _tmp685=({const char*_tmp111="attempt to overwrite a datatype field (";_tag_fat(_tmp111,sizeof(char),40U);});_tmp542.f1=_tmp685;});_tmp542;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10E=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp541;_tmp541.tag=1U,_tmp541.f1=df->name;_tmp541;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp540;_tmp540.tag=0U,({
struct _fat_ptr _tmp686=({const char*_tmp110=") with a const member";_tag_fat(_tmp110,sizeof(char),22U);});_tmp540.f1=_tmp686;});_tmp540;});void*_tmp10C[3U];_tmp10C[0]=& _tmp10D,_tmp10C[1]=& _tmp10E,_tmp10C[2]=& _tmp10F;({unsigned _tmp687=loc;Cyc_Warn_err2(_tmp687,_tag_fat(_tmp10C,sizeof(void*),3U));});});
return 0;}
# 597
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 599
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpFB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF7)->f2;_LL4: {struct Cyc_List_List*fs=_tmpFB;
# 580
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp102=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp102;
if((f->tq).real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp104=({struct Cyc_Warn_String_Warn_Warg_struct _tmp544;_tmp544.tag=0U,({struct _fat_ptr _tmp688=({const char*_tmp106="attempt to overwrite an aggregate with const member ";_tag_fat(_tmp106,sizeof(char),53U);});_tmp544.f1=_tmp688;});_tmp544;});struct Cyc_Warn_String_Warn_Warg_struct _tmp105=({struct Cyc_Warn_String_Warn_Warg_struct _tmp543;_tmp543.tag=0U,_tmp543.f1=*f->name;_tmp543;});void*_tmp103[2U];_tmp103[0]=& _tmp104,_tmp103[1]=& _tmp105;({unsigned _tmp689=loc;Cyc_Warn_err2(_tmp689,_tag_fat(_tmp103,sizeof(void*),2U));});});
return 0;}
# 586
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 588
return 1;}case 4U: _LL7: _tmpF9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).elt_type;_tmpFA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).tq;_LL8: {void*elt_type=_tmpF9;struct Cyc_Absyn_Tqual tq=_tmpFA;
# 601
if(tq.real_const){
({void*_tmp112=0U;({unsigned _tmp68B=loc;struct _fat_ptr _tmp68A=({const char*_tmp113="attempt to overwrite a const array";_tag_fat(_tmp113,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp68B,_tmp68A,_tag_fat(_tmp112,sizeof(void*),0U));});});
return 0;}
# 605
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmpF8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF7)->f1;_LLA: {struct Cyc_List_List*fs=_tmpF8;
# 607
for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp114=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp114;struct _tuple17*_tmp115=_stmttmpE;void*_tmp117;struct Cyc_Absyn_Tqual _tmp116;_LL11: _tmp116=_tmp115->f1;_tmp117=_tmp115->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmp116;void*t=_tmp117;
if(tq.real_const){
({void*_tmp118=0U;({unsigned _tmp68D=loc;struct _fat_ptr _tmp68C=({const char*_tmp119="attempt to overwrite a tuple field with a const member";_tag_fat(_tmp119,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp68D,_tmp68C,_tag_fat(_tmp118,sizeof(void*),0U));});});
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp133=({struct Cyc_Warn_String_Warn_Warg_struct _tmp545;_tmp545.tag=0U,({struct _fat_ptr _tmp68E=({const char*_tmp134="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp134,sizeof(char),47U);});_tmp545.f1=_tmp68E;});_tmp545;});void*_tmp132[1U];_tmp132[0]=& _tmp133;({unsigned _tmp68F=e->loc;Cyc_Warn_err2(_tmp68F,_tag_fat(_tmp132,sizeof(void*),1U));});});
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp547;_tmp547.tag=0U,({struct _fat_ptr _tmp690=({const char*_tmp150="attempt to write a const location: ";_tag_fat(_tmp150,sizeof(char),36U);});_tmp547.f1=_tmp690;});_tmp547;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp14F=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp546;_tmp546.tag=4U,_tmp546.f1=e;_tmp546;});void*_tmp14D[2U];_tmp14D[0]=& _tmp14E,_tmp14D[1]=& _tmp14F;({unsigned _tmp691=e->loc;Cyc_Warn_err2(_tmp691,_tag_fat(_tmp14D,sizeof(void*),2U));});});}
# 699
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 702
({struct Cyc_Tcenv_Tenv*_tmp692=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp692,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp152=({struct Cyc_Warn_String_Warn_Warg_struct _tmp548;_tmp548.tag=0U,({struct _fat_ptr _tmp693=({const char*_tmp153="increment/decrement of non-lvalue";_tag_fat(_tmp153,sizeof(char),34U);});_tmp548.f1=_tmp693;});_tmp548;});void*_tmp151[1U];_tmp151[0]=& _tmp152;({struct Cyc_Tcenv_Tenv*_tmp696=te;unsigned _tmp695=loc;void**_tmp694=topt;Cyc_Tcexp_expr_err(_tmp696,_tmp695,_tmp694,_tag_fat(_tmp151,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 708
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp154=0U;({unsigned _tmp698=e->loc;struct _fat_ptr _tmp697=({const char*_tmp155="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp155,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp698,_tmp697,_tag_fat(_tmp154,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp156=0U;({unsigned _tmp69A=e->loc;struct _fat_ptr _tmp699=({const char*_tmp157="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp157,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp69A,_tmp699,_tag_fat(_tmp156,sizeof(void*),0U));});});}else{
# 717
({struct Cyc_Warn_String_Warn_Warg_struct _tmp159=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54A;_tmp54A.tag=0U,({struct _fat_ptr _tmp69B=({const char*_tmp15B="expecting arithmetic or ? type but found ";_tag_fat(_tmp15B,sizeof(char),42U);});_tmp54A.f1=_tmp69B;});_tmp54A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp15A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp549;_tmp549.tag=2U,_tmp549.f1=(void*)t;_tmp549;});void*_tmp158[2U];_tmp158[0]=& _tmp159,_tmp158[1]=& _tmp15A;({unsigned _tmp69C=e->loc;Cyc_Warn_err2(_tmp69C,_tag_fat(_tmp158,sizeof(void*),2U));});});}}
# 719
return t;}}
# 723
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 725
({struct Cyc_Tcenv_Tenv*_tmp69E=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp69D=e1;Cyc_Tcexp_tcTest(_tmp69E,_tmp69D,({const char*_tmp15C="conditional expression";_tag_fat(_tmp15C,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*_tmp15D=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tako:& Cyc_Tcutil_tmko;struct Cyc_Core_Opt*ko=_tmp15D;
void*_tmp15E=({struct Cyc_Core_Opt*_tmp69F=ko;Cyc_Absyn_new_evar(_tmp69F,Cyc_Tcenv_lookup_opt_type_vars(te));});void*t=_tmp15E;
struct Cyc_List_List _tmp15F=({struct Cyc_List_List _tmp550;_tmp550.hd=e3,_tmp550.tl=0;_tmp550;});struct Cyc_List_List l1=_tmp15F;
struct Cyc_List_List _tmp160=({struct Cyc_List_List _tmp54F;_tmp54F.hd=e2,_tmp54F.tl=& l1;_tmp54F;});struct Cyc_List_List l2=_tmp160;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6A0=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp6A0,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp162=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54E;_tmp54E.tag=0U,({struct _fat_ptr _tmp6A1=({const char*_tmp167="conditional clause types do not match: ";_tag_fat(_tmp167,sizeof(char),40U);});_tmp54E.f1=_tmp6A1;});_tmp54E;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp163=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp54D;_tmp54D.tag=3U,_tmp54D.f1=(void*)e2->topt;_tmp54D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp164=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54C;_tmp54C.tag=0U,({
struct _fat_ptr _tmp6A2=({const char*_tmp166=" != ";_tag_fat(_tmp166,sizeof(char),5U);});_tmp54C.f1=_tmp6A2;});_tmp54C;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp165=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp54B;_tmp54B.tag=3U,_tmp54B.f1=(void*)e3->topt;_tmp54B;});void*_tmp161[4U];_tmp161[0]=& _tmp162,_tmp161[1]=& _tmp163,_tmp161[2]=& _tmp164,_tmp161[3]=& _tmp165;({unsigned _tmp6A3=loc;Cyc_Tcexp_err_and_explain(_tmp6A3,_tag_fat(_tmp161,sizeof(void*),4U));});});
return t;}}
# 739
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 741
({struct Cyc_Tcenv_Tenv*_tmp6A5=te;struct Cyc_Absyn_Exp*_tmp6A4=e1;Cyc_Tcexp_tcTest(_tmp6A5,_tmp6A4,({const char*_tmp168="logical-and expression";_tag_fat(_tmp168,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6A7=te;struct Cyc_Absyn_Exp*_tmp6A6=e2;Cyc_Tcexp_tcTest(_tmp6A7,_tmp6A6,({const char*_tmp169="logical-and expression";_tag_fat(_tmp169,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 745
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 747
({struct Cyc_Tcenv_Tenv*_tmp6A9=te;struct Cyc_Absyn_Exp*_tmp6A8=e1;Cyc_Tcexp_tcTest(_tmp6A9,_tmp6A8,({const char*_tmp16A="logical-or expression";_tag_fat(_tmp16A,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6AB=te;struct Cyc_Absyn_Exp*_tmp6AA=e2;Cyc_Tcexp_tcTest(_tmp6AB,_tmp6AA,({const char*_tmp16B="logical-or expression";_tag_fat(_tmp16B,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 753
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 760
({struct Cyc_Tcenv_Tenv*_tmp6AC=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp6AC,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 765
{void*_tmp16C=Cyc_Tcutil_compress(t1);void*_stmttmp17=_tmp16C;void*_tmp16D=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16D)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp551;_tmp551.tag=0U,({struct _fat_ptr _tmp6AD=({const char*_tmp170="cannot assign to an array";_tag_fat(_tmp170,sizeof(char),26U);});_tmp551.f1=_tmp6AD;});_tmp551;});void*_tmp16E[1U];_tmp16E[0]=& _tmp16F;({unsigned _tmp6AE=loc;Cyc_Warn_err2(_tmp6AE,_tag_fat(_tmp16E,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 770
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp172=({struct Cyc_Warn_String_Warn_Warg_struct _tmp552;_tmp552.tag=0U,({struct _fat_ptr _tmp6AF=({const char*_tmp173="type is abstract (can't determine size)";_tag_fat(_tmp173,sizeof(char),40U);});_tmp552.f1=_tmp6AF;});_tmp552;});void*_tmp171[1U];_tmp171[0]=& _tmp172;({unsigned _tmp6B0=loc;Cyc_Warn_err2(_tmp6B0,_tag_fat(_tmp171,sizeof(void*),1U));});});
# 774
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp175=({struct Cyc_Warn_String_Warn_Warg_struct _tmp553;_tmp553.tag=0U,({struct _fat_ptr _tmp6B1=({const char*_tmp176="assignment to non-lvalue";_tag_fat(_tmp176,sizeof(char),25U);});_tmp553.f1=_tmp6B1;});_tmp553;});void*_tmp174[1U];_tmp174[0]=& _tmp175;({struct Cyc_Tcenv_Tenv*_tmp6B4=te;unsigned _tmp6B3=loc;void**_tmp6B2=topt;Cyc_Tcexp_expr_err(_tmp6B4,_tmp6B3,_tmp6B2,_tag_fat(_tmp174,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e2))
Cyc_Tcexp_unique_consume_err(e2->loc);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6B6=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6B5=e2;Cyc_Tcutil_coerce_assign(_tmp6B6,_tmp6B5,t1);})){
void*_tmp177=({struct Cyc_Warn_String_Warn_Warg_struct _tmp179=({struct Cyc_Warn_String_Warn_Warg_struct _tmp557;_tmp557.tag=0U,({struct _fat_ptr _tmp6B7=({const char*_tmp17E="type mismatch: ";_tag_fat(_tmp17E,sizeof(char),16U);});_tmp557.f1=_tmp6B7;});_tmp557;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp17A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp556;_tmp556.tag=2U,_tmp556.f1=(void*)t1;_tmp556;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp555;_tmp555.tag=0U,({struct _fat_ptr _tmp6B8=({const char*_tmp17D=" != ";_tag_fat(_tmp17D,sizeof(char),5U);});_tmp555.f1=_tmp6B8;});_tmp555;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp17C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp554;_tmp554.tag=2U,_tmp554.f1=(void*)t2;_tmp554;});void*_tmp178[4U];_tmp178[0]=& _tmp179,_tmp178[1]=& _tmp17A,_tmp178[2]=& _tmp17B,_tmp178[3]=& _tmp17C;({struct Cyc_Tcenv_Tenv*_tmp6BB=te;unsigned _tmp6BA=loc;void**_tmp6B9=topt;Cyc_Tcexp_expr_err(_tmp6BB,_tmp6BA,_tmp6B9,_tag_fat(_tmp178,sizeof(void*),4U));});});void*result=_tmp177;
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
void*_tmp181=({struct Cyc_Warn_String_Warn_Warg_struct _tmp183=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55C;_tmp55C.tag=0U,({
struct _fat_ptr _tmp6BC=({const char*_tmp18A="Cannot use this operator in an assignment when ";_tag_fat(_tmp18A,sizeof(char),48U);});_tmp55C.f1=_tmp6BC;});_tmp55C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp184=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55B;_tmp55B.tag=0U,({
struct _fat_ptr _tmp6BD=({const char*_tmp189="the arguments have types ";_tag_fat(_tmp189,sizeof(char),26U);});_tmp55B.f1=_tmp6BD;});_tmp55B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp185=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55A;_tmp55A.tag=2U,_tmp55A.f1=(void*)t1;_tmp55A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp186=({struct Cyc_Warn_String_Warn_Warg_struct _tmp559;_tmp559.tag=0U,({struct _fat_ptr _tmp6BE=({const char*_tmp188=" and ";_tag_fat(_tmp188,sizeof(char),6U);});_tmp559.f1=_tmp6BE;});_tmp559;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp187=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp558;_tmp558.tag=2U,_tmp558.f1=(void*)t2;_tmp558;});void*_tmp182[5U];_tmp182[0]=& _tmp183,_tmp182[1]=& _tmp184,_tmp182[2]=& _tmp185,_tmp182[3]=& _tmp186,_tmp182[4]=& _tmp187;({struct Cyc_Tcenv_Tenv*_tmp6C1=te;unsigned _tmp6C0=loc;void**_tmp6BF=topt;Cyc_Tcexp_expr_err(_tmp6C1,_tmp6C0,_tmp6BF,_tag_fat(_tmp182,sizeof(void*),5U));});});
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
({struct Cyc_Tcenv_Tenv*_tmp6C2=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp6C2,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp6C4=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp6C3=topt;Cyc_Tcexp_tcExp(_tmp6C4,_tmp6C3,e2);});
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
if(({struct Cyc_RgnOrder_RgnPO*_tmp6C6=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6C5=e;Cyc_Tcutil_coerce_arg(_tmp6C6,_tmp6C5,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 847
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp560;_tmp560.tag=0U,({struct _fat_ptr _tmp6C7=({const char*_tmp19F="can't find a field in ";_tag_fat(_tmp19F,sizeof(char),23U);});_tmp560.f1=_tmp6C7;});_tmp560;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55F;_tmp55F.tag=2U,_tmp55F.f1=(void*)tu;_tmp55F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp19C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55E;_tmp55E.tag=0U,({struct _fat_ptr _tmp6C8=({const char*_tmp19E=" to inject value of type ";_tag_fat(_tmp19E,sizeof(char),26U);});_tmp55E.f1=_tmp6C8;});_tmp55E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55D;_tmp55D.tag=2U,_tmp55D.f1=(void*)t1;_tmp55D;});void*_tmp199[4U];_tmp199[0]=& _tmp19A,_tmp199[1]=& _tmp19B,_tmp199[2]=& _tmp19C,_tmp199[3]=& _tmp19D;({unsigned _tmp6C9=e->loc;Cyc_Warn_err2(_tmp6C9,_tag_fat(_tmp199,sizeof(void*),4U));});});
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
void*_tmp1AB=Cyc_Tcutil_compress(t1);void*_stmttmp1B=_tmp1AB;void*_tmp1AC=_stmttmp1B;struct Cyc_Absyn_Vardecl*_tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_List_List*_tmp1B5;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_VarargInfo*_tmp1B3;int _tmp1B2;struct Cyc_List_List*_tmp1B1;void*_tmp1B0;struct Cyc_Absyn_Tqual _tmp1AF;void*_tmp1AE;struct Cyc_List_List*_tmp1AD;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->tag == 5U){_LL6: _tmp1AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).tvars;_tmp1AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).effect;_tmp1AF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ret_tqual;_tmp1B0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ret_type;_tmp1B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).args;_tmp1B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).c_varargs;_tmp1B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).cyc_varargs;_tmp1B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).rgn_po;_tmp1B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).attributes;_tmp1B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).requires_clause;_tmp1B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).requires_relns;_tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ensures_clause;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ensures_relns;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).return_value;_LL7: {struct Cyc_List_List*tvars=_tmp1AD;void*eff=_tmp1AE;struct Cyc_Absyn_Tqual res_tq=_tmp1AF;void*res_typ=_tmp1B0;struct Cyc_List_List*args_info=_tmp1B1;int c_vararg=_tmp1B2;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp1B3;struct Cyc_List_List*rgn_po=_tmp1B4;struct Cyc_List_List*atts=_tmp1B5;struct Cyc_Absyn_Exp*req=_tmp1B6;struct Cyc_List_List*req_relns=_tmp1B7;struct Cyc_Absyn_Exp*ens=_tmp1B8;struct Cyc_List_List*ens_relns=_tmp1B9;struct Cyc_Absyn_Vardecl*ret_var=_tmp1BA;
# 879
if(tvars != 0 || rgn_po != 0)
({void*_tmp1BB=0U;({unsigned _tmp6CB=e->loc;struct _fat_ptr _tmp6CA=({const char*_tmp1BC="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1BC,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp6CB,_tmp6CA,_tag_fat(_tmp1BB,sizeof(void*),0U));});});
# 883
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 885
while(es != 0 && args_info != 0){
# 887
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6CD=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6CC=e1;Cyc_Tcutil_coerce_arg(_tmp6CD,_tmp6CC,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1CC="actual argument has type ";_tag_fat(_tmp1CC,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1CB=" but formal has type ";_tag_fat(_tmp1CB,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp6D0=({unsigned long _tmp6CF=({unsigned long _tmp6CE=Cyc_strlen((struct _fat_ptr)s0);_tmp6CE + Cyc_strlen((struct _fat_ptr)s1);});_tmp6CF + Cyc_strlen((struct _fat_ptr)s2);});_tmp6D0 + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({void*_tmp1BD=0U;({unsigned _tmp6D3=e1->loc;struct _fat_ptr _tmp6D2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C0=({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0U,_tmp564.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp564;});struct Cyc_String_pa_PrintArg_struct _tmp1C1=({struct Cyc_String_pa_PrintArg_struct _tmp563;_tmp563.tag=0U,_tmp563.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp563;});struct Cyc_String_pa_PrintArg_struct _tmp1C2=({struct Cyc_String_pa_PrintArg_struct _tmp562;_tmp562.tag=0U,_tmp562.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp562;});struct Cyc_String_pa_PrintArg_struct _tmp1C3=({struct Cyc_String_pa_PrintArg_struct _tmp561;_tmp561.tag=0U,_tmp561.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp561;});void*_tmp1BE[4U];_tmp1BE[0]=& _tmp1C0,_tmp1BE[1]=& _tmp1C1,_tmp1BE[2]=& _tmp1C2,_tmp1BE[3]=& _tmp1C3;({struct _fat_ptr _tmp6D1=({const char*_tmp1BF="%s\n\t%s\n%s\n\t%s.";_tag_fat(_tmp1BF,sizeof(char),15U);});Cyc_aprintf(_tmp6D1,_tag_fat(_tmp1BE,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp6D3,_tmp6D2,_tag_fat(_tmp1BD,sizeof(void*),0U));});});else{
# 899
({void*_tmp1C4=0U;({unsigned _tmp6D6=e1->loc;struct _fat_ptr _tmp6D5=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C7=({struct Cyc_String_pa_PrintArg_struct _tmp568;_tmp568.tag=0U,_tmp568.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp568;});struct Cyc_String_pa_PrintArg_struct _tmp1C8=({struct Cyc_String_pa_PrintArg_struct _tmp567;_tmp567.tag=0U,_tmp567.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp567;});struct Cyc_String_pa_PrintArg_struct _tmp1C9=({struct Cyc_String_pa_PrintArg_struct _tmp566;_tmp566.tag=0U,_tmp566.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp566;});struct Cyc_String_pa_PrintArg_struct _tmp1CA=({struct Cyc_String_pa_PrintArg_struct _tmp565;_tmp565.tag=0U,_tmp565.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp565;});void*_tmp1C5[4U];_tmp1C5[0]=& _tmp1C7,_tmp1C5[1]=& _tmp1C8,_tmp1C5[2]=& _tmp1C9,_tmp1C5[3]=& _tmp1CA;({struct _fat_ptr _tmp6D4=({const char*_tmp1C6="%s%s%s%s.";_tag_fat(_tmp1C6,sizeof(char),10U);});Cyc_aprintf(_tmp6D4,_tag_fat(_tmp1C5,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp6D6,_tmp6D5,_tag_fat(_tmp1C4,sizeof(void*),0U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 904
if(alias_coercion)
({struct Cyc_List_List*_tmp6D7=({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=(void*)arg_cnt,_tmp1CD->tl=*alias_arg_exps;_tmp1CD;});*alias_arg_exps=_tmp6D7;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 915
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1CE=(void*)a->hd;void*_stmttmp1C=_tmp1CE;void*_tmp1CF=_stmttmp1C;int _tmp1D2;int _tmp1D1;enum Cyc_Absyn_Format_Type _tmp1D0;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CF)->tag == 19U){_LLB: _tmp1D0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CF)->f1;_tmp1D1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CF)->f2;_tmp1D2=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CF)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp1D0;int fmt_arg_pos=_tmp1D1;int arg_start_pos=_tmp1D2;
# 919
{struct _handler_cons _tmp1D3;_push_handler(& _tmp1D3);{int _tmp1D5=0;if(setjmp(_tmp1D3.handler))_tmp1D5=1;if(!_tmp1D5){
# 921
{struct Cyc_Absyn_Exp*_tmp1D6=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1D6;
# 923
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 927
fmt_args=({struct Cyc_Core_Opt*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));({struct Cyc_List_List*_tmp6D8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp1D7->v=_tmp6D8;});_tmp1D7;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1D8=ft;switch(_tmp1D8){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 931
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 934
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 937
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 940
goto _LLF;}_LLF:;}}
# 921
;_pop_handler();}else{void*_tmp1D4=(void*)Cyc_Core_get_exn_thrown();void*_tmp1D9=_tmp1D4;void*_tmp1DA;if(((struct Cyc_List_Nth_exn_struct*)_tmp1D9)->tag == Cyc_List_Nth){_LL17: _LL18:
# 942
({void*_tmp1DB=0U;({unsigned _tmp6DA=loc;struct _fat_ptr _tmp6D9=({const char*_tmp1DC="bad format arguments";_tag_fat(_tmp1DC,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp6DA,_tmp6D9,_tag_fat(_tmp1DB,sizeof(void*),0U));});});goto _LL16;}else{_LL19: _tmp1DA=_tmp1D9;_LL1A: {void*exn=_tmp1DA;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 947
if(args_info != 0)({void*_tmp1DD=0U;({unsigned _tmp6DC=loc;struct _fat_ptr _tmp6DB=({const char*_tmp1DE="too few arguments for function";_tag_fat(_tmp1DE,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp6DC,_tmp6DB,_tag_fat(_tmp1DD,sizeof(void*),0U));});});else{
# 949
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({void*_tmp1DF=0U;({unsigned _tmp6DE=loc;struct _fat_ptr _tmp6DD=({const char*_tmp1E0="too many arguments for function";_tag_fat(_tmp1E0,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp6DE,_tmp6DD,_tag_fat(_tmp1DF,sizeof(void*),0U));});});else{
# 956
struct Cyc_Absyn_VarargInfo _tmp1E1=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1D=_tmp1E1;struct Cyc_Absyn_VarargInfo _tmp1E2=_stmttmp1D;int _tmp1E4;void*_tmp1E3;_LL1C: _tmp1E3=_tmp1E2.type;_tmp1E4=_tmp1E2.inject;_LL1D: {void*vt=_tmp1E3;int inject=_tmp1E4;
struct Cyc_Absyn_VarargCallInfo*_tmp1E5=({struct Cyc_Absyn_VarargCallInfo*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->num_varargs=0,_tmp1FD->injectors=0,_tmp1FD->vai=cyc_vararg;_tmp1FD;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1E5;
# 960
*vararg_call_info=vci;
# 962
if(!inject)
# 964
for(0;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6E0=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6DF=e1;Cyc_Tcutil_coerce_arg(_tmp6E0,_tmp6DF,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56C;_tmp56C.tag=0U,({struct _fat_ptr _tmp6E1=({const char*_tmp1EC="vararg requires type ";_tag_fat(_tmp1EC,sizeof(char),22U);});_tmp56C.f1=_tmp6E1;});_tmp56C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56B;_tmp56B.tag=2U,_tmp56B.f1=(void*)vt;_tmp56B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56A;_tmp56A.tag=0U,({
struct _fat_ptr _tmp6E2=({const char*_tmp1EB=" but argument has type ";_tag_fat(_tmp1EB,sizeof(char),24U);});_tmp56A.f1=_tmp6E2;});_tmp56A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1EA=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp569;_tmp569.tag=3U,_tmp569.f1=(void*)e1->topt;_tmp569;});void*_tmp1E6[4U];_tmp1E6[0]=& _tmp1E7,_tmp1E6[1]=& _tmp1E8,_tmp1E6[2]=& _tmp1E9,_tmp1E6[3]=& _tmp1EA;({unsigned _tmp6E3=loc;Cyc_Tcexp_err_and_explain(_tmp6E3,_tag_fat(_tmp1E6,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp6E4=({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=(void*)arg_cnt,_tmp1ED->tl=*alias_arg_exps;_tmp1ED;});*alias_arg_exps=_tmp6E4;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vt)&& !
Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);}else{
# 981
void*_tmp1EE=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1E=_tmp1EE;void*_tmp1EF=_stmttmp1E;struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Datatypedecl*_tmp1F0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1F0=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f1)->f1).KnownDatatype).val;_tmp1F1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp1F0;struct Cyc_List_List*targs=_tmp1F1;
# 985
struct Cyc_Absyn_Datatypedecl*_tmp1F2=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp1F2;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56E;_tmp56E.tag=0U,({struct _fat_ptr _tmp6E5=({const char*_tmp1F6="can't inject into abstract ";_tag_fat(_tmp1F6,sizeof(char),28U);});_tmp56E.f1=_tmp6E5;});_tmp56E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56D;_tmp56D.tag=2U,_tmp56D.f1=(void*)vt;_tmp56D;});void*_tmp1F3[2U];_tmp1F3[0]=& _tmp1F4,_tmp1F3[1]=& _tmp1F5;({unsigned _tmp6E6=loc;Cyc_Warn_err2(_tmp6E6,_tag_fat(_tmp1F3,sizeof(void*),2U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 995
({void*_tmp6E7=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp6E7,Cyc_Tcenv_curr_rgn(te));});{
# 997
struct Cyc_List_List*_tmp1F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp1F7;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1002
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt))&& !
Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);}{
# 1008
struct Cyc_Absyn_Datatypefield*_tmp1F8=({struct Cyc_Tcenv_Tenv*_tmp6EB=te;struct Cyc_Absyn_Exp*_tmp6EA=e1;void*_tmp6E9=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp6E8=inst;Cyc_Tcexp_tcInjection(_tmp6EB,_tmp6EA,_tmp6E9,_tmp6E8,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp1F8;
if(f != 0)
({struct Cyc_List_List*_tmp6ED=({
struct Cyc_List_List*_tmp6EC=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6EC,({struct Cyc_List_List*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9->hd=f,_tmp1F9->tl=0;_tmp1F9;}));});
# 1010
vci->injectors=_tmp6ED;});}}}
# 1013
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56F;_tmp56F.tag=0U,({struct _fat_ptr _tmp6EE=({const char*_tmp1FC="bad inject vararg type";_tag_fat(_tmp1FC,sizeof(char),23U);});_tmp56F.f1=_tmp6EE;});_tmp56F;});void*_tmp1FA[1U];_tmp1FA[0]=& _tmp1FB;({unsigned _tmp6EF=loc;Cyc_Warn_err2(_tmp6EF,_tag_fat(_tmp1FA,sizeof(void*),1U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1019
if(*alias_arg_exps == 0)
# 1028 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1030
return res_typ;}}}else{_LL8: _LL9:
# 1032
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp571;_tmp571.tag=0U,({struct _fat_ptr _tmp6F0=({const char*_tmp201="expected pointer to function but found ";_tag_fat(_tmp201,sizeof(char),40U);});_tmp571.f1=_tmp6F0;});_tmp571;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp200=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp570;_tmp570.tag=2U,_tmp570.f1=(void*)t;_tmp570;});void*_tmp1FE[2U];_tmp1FE[0]=& _tmp1FF,_tmp1FE[1]=& _tmp200;({struct Cyc_Tcenv_Tenv*_tmp6F3=te;unsigned _tmp6F2=loc;void**_tmp6F1=topt;Cyc_Tcexp_expr_err(_tmp6F3,_tmp6F2,_tmp6F1,_tag_fat(_tmp1FE,sizeof(void*),2U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1035
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp203=({struct Cyc_Warn_String_Warn_Warg_struct _tmp573;_tmp573.tag=0U,({struct _fat_ptr _tmp6F4=({const char*_tmp205="expected pointer to function but found ";_tag_fat(_tmp205,sizeof(char),40U);});_tmp573.f1=_tmp6F4;});_tmp573;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp204=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp572;_tmp572.tag=2U,_tmp572.f1=(void*)t;_tmp572;});void*_tmp202[2U];_tmp202[0]=& _tmp203,_tmp202[1]=& _tmp204;({struct Cyc_Tcenv_Tenv*_tmp6F7=te;unsigned _tmp6F6=loc;void**_tmp6F5=topt;Cyc_Tcexp_expr_err(_tmp6F7,_tmp6F6,_tmp6F5,_tag_fat(_tmp202,sizeof(void*),2U));});});}_LL0:;}}}
# 1039
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp206=Cyc_Absyn_exn_type();void*exception_type=_tmp206;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6F9=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6F8=e;Cyc_Tcutil_coerce_arg(_tmp6F9,_tmp6F8,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp208=({struct Cyc_Warn_String_Warn_Warg_struct _tmp577;_tmp577.tag=0U,({struct _fat_ptr _tmp6FA=({const char*_tmp20D="expected ";_tag_fat(_tmp20D,sizeof(char),10U);});_tmp577.f1=_tmp6FA;});_tmp577;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp209=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp576;_tmp576.tag=2U,_tmp576.f1=(void*)exception_type;_tmp576;});struct Cyc_Warn_String_Warn_Warg_struct _tmp20A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp575;_tmp575.tag=0U,({struct _fat_ptr _tmp6FB=({const char*_tmp20C=" but found ";_tag_fat(_tmp20C,sizeof(char),12U);});_tmp575.f1=_tmp6FB;});_tmp575;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp20B=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp574;_tmp574.tag=3U,_tmp574.f1=(void*)e->topt;_tmp574;});void*_tmp207[4U];_tmp207[0]=& _tmp208,_tmp207[1]=& _tmp209,_tmp207[2]=& _tmp20A,_tmp207[3]=& _tmp20B;({unsigned _tmp6FC=loc;Cyc_Warn_err2(_tmp6FC,_tag_fat(_tmp207,sizeof(void*),4U));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1050
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1052
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp20E=t1;struct Cyc_Absyn_PtrAtts _tmp211;struct Cyc_Absyn_Tqual _tmp210;void*_tmp20F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20E)->tag == 3U){_LL1: _tmp20F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20E)->f1).elt_type;_tmp210=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20E)->f1).elt_tq;_tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20E)->f1).ptr_atts;_LL2: {void*t=_tmp20F;struct Cyc_Absyn_Tqual tq=_tmp210;struct Cyc_Absyn_PtrAtts atts=_tmp211;
# 1055
{void*_tmp212=Cyc_Tcutil_compress(t);void*_stmttmp1F=_tmp212;void*_tmp213=_stmttmp1F;struct Cyc_Absyn_FnInfo _tmp214;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->tag == 5U){_LL6: _tmp214=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->f1;_LL7: {struct Cyc_Absyn_FnInfo info=_tmp214;
# 1057
struct Cyc_List_List*_tmp215=info.tvars;struct Cyc_List_List*tvars=_tmp215;
struct Cyc_List_List*instantiation=0;
# 1060
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);
({unsigned _tmp700=loc;struct Cyc_Tcenv_Tenv*_tmp6FF=te;struct Cyc_List_List*_tmp6FE=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6FD=k;Cyc_Tctyp_check_type(_tmp700,_tmp6FF,_tmp6FE,_tmp6FD,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp217=_cycalloc(sizeof(*_tmp217));({struct _tuple14*_tmp701=({struct _tuple14*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp216->f2=(void*)ts->hd;_tmp216;});_tmp217->hd=_tmp701;}),_tmp217->tl=instantiation;_tmp217;});}
# 1066
info.tvars=tvars;
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp219=({struct Cyc_Warn_String_Warn_Warg_struct _tmp578;_tmp578.tag=0U,({struct _fat_ptr _tmp702=({const char*_tmp21A="too many type variables in instantiation";_tag_fat(_tmp21A,sizeof(char),41U);});_tmp578.f1=_tmp702;});_tmp578;});void*_tmp218[1U];_tmp218[0]=& _tmp219;({struct Cyc_Tcenv_Tenv*_tmp705=te;unsigned _tmp704=loc;void**_tmp703=topt;Cyc_Tcexp_expr_err(_tmp705,_tmp704,_tmp703,_tag_fat(_tmp218,sizeof(void*),1U));});});
# 1072
if(tvars == 0){
({struct Cyc_List_List*_tmp706=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,info.rgn_po);info.rgn_po=_tmp706;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,info.rgn_po);
info.rgn_po=0;}{
# 1077
void*new_fn_typ=({struct Cyc_List_List*_tmp707=instantiation;Cyc_Tcutil_substitute(_tmp707,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->tag=5U,_tmp21B->f1=info;_tmp21B;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp579;_tmp579.elt_type=new_fn_typ,_tmp579.elt_tq=tq,_tmp579.ptr_atts=atts;_tmp579;}));}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1081
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1084
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp21D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57B;_tmp57B.tag=0U,({struct _fat_ptr _tmp708=({const char*_tmp21F="expecting polymorphic type but found ";_tag_fat(_tmp21F,sizeof(char),38U);});_tmp57B.f1=_tmp708;});_tmp57B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57A;_tmp57A.tag=2U,_tmp57A.f1=(void*)t1;_tmp57A;});void*_tmp21C[2U];_tmp21C[0]=& _tmp21D,_tmp21C[1]=& _tmp21E;({struct Cyc_Tcenv_Tenv*_tmp70B=te;unsigned _tmp70A=loc;void**_tmp709=topt;Cyc_Tcexp_expr_err(_tmp70B,_tmp70A,_tmp709,_tag_fat(_tmp21C,sizeof(void*),2U));});});}
# 1088
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1090
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1092
({void*_tmp70C=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp70C;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1097
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1099
({unsigned _tmp710=loc;struct Cyc_Tcenv_Tenv*_tmp70F=te;struct Cyc_List_List*_tmp70E=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp70D=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1099
Cyc_Tctyp_check_type(_tmp710,_tmp70F,_tmp70E,_tmp70D,1,0,t);});
# 1101
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp713=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp712=loc;void*_tmp711=t2;Cyc_Tcutil_silent_castable(_tmp713,_tmp712,_tmp711,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1107
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp716=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp715=loc;void*_tmp714=t2;Cyc_Tcutil_castable(_tmp716,_tmp715,_tmp714,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1114
Cyc_Unify_unify(t2,t);{
void*_tmp220=({struct Cyc_Warn_String_Warn_Warg_struct _tmp222=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57F;_tmp57F.tag=0U,({struct _fat_ptr _tmp717=({const char*_tmp227="cannot cast ";_tag_fat(_tmp227,sizeof(char),13U);});_tmp57F.f1=_tmp717;});_tmp57F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp223=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57E;_tmp57E.tag=2U,_tmp57E.f1=(void*)t2;_tmp57E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp224=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57D;_tmp57D.tag=0U,({struct _fat_ptr _tmp718=({const char*_tmp226=" to ";_tag_fat(_tmp226,sizeof(char),5U);});_tmp57D.f1=_tmp718;});_tmp57D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp225=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57C;_tmp57C.tag=2U,_tmp57C.f1=(void*)t;_tmp57C;});void*_tmp221[4U];_tmp221[0]=& _tmp222,_tmp221[1]=& _tmp223,_tmp221[2]=& _tmp224,_tmp221[3]=& _tmp225;({struct Cyc_Tcenv_Tenv*_tmp71A=te;unsigned _tmp719=loc;Cyc_Tcexp_expr_err(_tmp71A,_tmp719,& t,_tag_fat(_tmp221,sizeof(void*),4U));});});void*result=_tmp220;
Cyc_Unify_explain_failure();
return result;}}}}{
# 1123
struct _tuple0 _tmp228=({struct _tuple0 _tmp580;_tmp580.f1=e->r,({void*_tmp71B=Cyc_Tcutil_compress(t);_tmp580.f2=_tmp71B;});_tmp580;});struct _tuple0 _stmttmp20=_tmp228;struct _tuple0 _tmp229=_stmttmp20;void*_tmp22D;void*_tmp22C;void*_tmp22B;int _tmp22A;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp229.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229.f2)->tag == 3U){_LL1: _tmp22A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp229.f1)->f1).fat_result;_tmp22B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229.f2)->f1).ptr_atts).nullable;_tmp22C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229.f2)->f1).ptr_atts).bounds;_tmp22D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229.f2)->f1).ptr_atts).zero_term;_LL2: {int fat_result=_tmp22A;void*nbl=_tmp22B;void*bds=_tmp22C;void*zt=_tmp22D;
# 1127
if((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl)){
struct Cyc_Absyn_Exp*_tmp22E=({void*_tmp71C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp71C,bds);});struct Cyc_Absyn_Exp*eopt=_tmp22E;
if(eopt != 0){
if((Cyc_Evexp_eval_const_uint_exp(e)).f1 == (unsigned)1)
({void*_tmp22F=0U;({unsigned _tmp71E=loc;struct _fat_ptr _tmp71D=({const char*_tmp230="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp230,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp71E,_tmp71D,_tag_fat(_tmp22F,sizeof(void*),0U));});});}}
# 1133
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;}_LL0:;}}}
# 1139
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp231=0;void**topt2=_tmp231;
struct Cyc_Absyn_Tqual _tmp232=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp232;
int _tmp233=0;int nullable=_tmp233;
if(topt != 0){
void*_tmp234=Cyc_Tcutil_compress(*topt);void*_stmttmp21=_tmp234;void*_tmp235=_stmttmp21;void*_tmp238;struct Cyc_Absyn_Tqual _tmp237;void**_tmp236;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp235)->tag == 3U){_LL1: _tmp236=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp235)->f1).elt_type;_tmp237=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp235)->f1).elt_tq;_tmp238=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp235)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp236;struct Cyc_Absyn_Tqual tq=_tmp237;void*n=_tmp238;
# 1146
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1160
({struct Cyc_Tcenv_Tenv*_tmp720=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp71F=topt2;Cyc_Tcexp_tcExpNoInst(_tmp720,_tmp71F,e);});
# 1162
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0U,({struct _fat_ptr _tmp721=({const char*_tmp23B="cannot take the address of an alias-free path";_tag_fat(_tmp23B,sizeof(char),46U);});_tmp581.f1=_tmp721;});_tmp581;});void*_tmp239[1U];_tmp239[0]=& _tmp23A;({unsigned _tmp722=e->loc;Cyc_Warn_err2(_tmp722,_tag_fat(_tmp239,sizeof(void*),1U));});});
# 1167
{void*_tmp23C=e->r;void*_stmttmp22=_tmp23C;void*_tmp23D=_stmttmp22;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_Exp*_tmp23E;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23D)->tag == 23U){_LL6: _tmp23E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23D)->f1;_tmp23F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23D)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp23E;struct Cyc_Absyn_Exp*e2=_tmp23F;
# 1169
{void*_tmp240=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp23=_tmp240;void*_tmp241=_stmttmp23;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp241)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1174
({void*_tmp723=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp723;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1177
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1182
{void*_tmp242=e->r;void*_stmttmp24=_tmp242;void*_tmp243=_stmttmp24;switch(*((int*)_tmp243)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp243)->f3 == 1){_LL12: _LL13:
# 1185
({void*_tmp244=0U;({unsigned _tmp725=e->loc;struct _fat_ptr _tmp724=({const char*_tmp245="cannot take the address of a @tagged union member";_tag_fat(_tmp245,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp725,_tmp724,_tag_fat(_tmp244,sizeof(void*),0U));});});goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1190
struct _tuple15 _tmp246=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp25=_tmp246;struct _tuple15 _tmp247=_stmttmp25;void*_tmp249;int _tmp248;_LL17: _tmp248=_tmp247.f1;_tmp249=_tmp247.f2;_LL18: {int is_const=_tmp248;void*rgn=_tmp249;
# 1192
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({void*_tmp24A=0U;({unsigned _tmp727=e->loc;struct _fat_ptr _tmp726=({const char*_tmp24B="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp24B,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp727,_tmp726,_tag_fat(_tmp24A,sizeof(void*),0U));});});{
# 1195
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _tmp728=tq.real_const=1;tq.print_const=_tmp728;});
# 1199
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type);}}}}
# 1203
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1206
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _tmp72B=loc;struct Cyc_Tcenv_Tenv*_tmp72A=te;struct Cyc_List_List*_tmp729=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp72B,_tmp72A,_tmp729,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp584;_tmp584.tag=0U,({struct _fat_ptr _tmp72C=({const char*_tmp251="sizeof applied to type ";_tag_fat(_tmp251,sizeof(char),24U);});_tmp584.f1=_tmp72C;});_tmp584;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp24E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp583;_tmp583.tag=2U,_tmp583.f1=(void*)t;_tmp583;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp582;_tmp582.tag=0U,({struct _fat_ptr _tmp72D=({const char*_tmp250=" which has unknown size here";_tag_fat(_tmp250,sizeof(char),29U);});_tmp582.f1=_tmp72D;});_tmp582;});void*_tmp24C[3U];_tmp24C[0]=& _tmp24D,_tmp24C[1]=& _tmp24E,_tmp24C[2]=& _tmp24F;({unsigned _tmp72E=loc;Cyc_Warn_err2(_tmp72E,_tag_fat(_tmp24C,sizeof(void*),3U));});});
if(topt == 0)
return Cyc_Absyn_uint_type;{
void*_tmp252=Cyc_Tcutil_compress(*topt);void*_stmttmp26=_tmp252;void*_tmp253=_stmttmp26;void*_tmp254;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp253)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp253)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp253)->f2 != 0){_LL1: _tmp254=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp253)->f2)->hd;_LL2: {void*tt=_tmp254;
# 1216
if(({void*_tmp730=tt;Cyc_Unify_unify(_tmp730,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255->tag=9U,({struct Cyc_Absyn_Exp*_tmp72F=Cyc_Absyn_sizeoftype_exp(t,0U);_tmp255->f1=_tmp72F;});_tmp255;}));}))
return Cyc_Tcutil_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_uint_type;}_LL0:;}}
# 1223
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)
return sf->type;
return 0;}
# 1231
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1233
({unsigned _tmp733=loc;struct Cyc_Tcenv_Tenv*_tmp732=te;struct Cyc_List_List*_tmp731=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp733,_tmp732,_tmp731,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp256=fs;struct Cyc_List_List*l=_tmp256;
void*_tmp257=t_orig;void*t=_tmp257;
for(0;l != 0;l=l->tl){
void*_tmp258=(void*)l->hd;void*n=_tmp258;
void*_tmp259=n;unsigned _tmp25A;struct _fat_ptr*_tmp25B;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp259)->tag == 0U){_LL1: _tmp25B=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp259)->f1;_LL2: {struct _fat_ptr*n=_tmp25B;
# 1241
int bad_type=1;
{void*_tmp25C=Cyc_Tcutil_compress(t);void*_stmttmp27=_tmp25C;void*_tmp25D=_stmttmp27;struct Cyc_List_List*_tmp25E;struct Cyc_Absyn_Aggrdecl**_tmp25F;switch(*((int*)_tmp25D)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp25F=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp25F;
# 1244
if((*adp)->impl == 0)
goto _LL5;
_tmp25E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL9;}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp25E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp25D)->f2;_LL9: {struct Cyc_List_List*fields=_tmp25E;
# 1248
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp261=({struct Cyc_Warn_String_Warn_Warg_struct _tmp586;_tmp586.tag=0U,({struct _fat_ptr _tmp734=({const char*_tmp263="no field of struct/union has name %s";_tag_fat(_tmp263,sizeof(char),37U);});_tmp586.f1=_tmp734;});_tmp586;});struct Cyc_Warn_String_Warn_Warg_struct _tmp262=({struct Cyc_Warn_String_Warn_Warg_struct _tmp585;_tmp585.tag=0U,_tmp585.f1=*n;_tmp585;});void*_tmp260[2U];_tmp260[0]=& _tmp261,_tmp260[1]=& _tmp262;({unsigned _tmp735=loc;Cyc_Warn_err2(_tmp735,_tag_fat(_tmp260,sizeof(void*),2U));});});else{
# 1252
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1257
if(bad_type){
if(l == fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp265=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp588;_tmp588.tag=2U,_tmp588.f1=(void*)t;_tmp588;});struct Cyc_Warn_String_Warn_Warg_struct _tmp266=({struct Cyc_Warn_String_Warn_Warg_struct _tmp587;_tmp587.tag=0U,({struct _fat_ptr _tmp736=({const char*_tmp267=" is not a known struct/union type";_tag_fat(_tmp267,sizeof(char),34U);});_tmp587.f1=_tmp736;});_tmp587;});void*_tmp264[2U];_tmp264[0]=& _tmp265,_tmp264[1]=& _tmp266;({unsigned _tmp737=loc;Cyc_Warn_err2(_tmp737,_tag_fat(_tmp264,sizeof(void*),2U));});});else{
# 1261
struct _fat_ptr _tmp268=({struct Cyc_String_pa_PrintArg_struct _tmp27E=({struct Cyc_String_pa_PrintArg_struct _tmp591;_tmp591.tag=0U,({struct _fat_ptr _tmp738=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp591.f1=_tmp738;});_tmp591;});void*_tmp27C[1U];_tmp27C[0]=& _tmp27E;({struct _fat_ptr _tmp739=({const char*_tmp27D="(%s)";_tag_fat(_tmp27D,sizeof(char),5U);});Cyc_aprintf(_tmp739,_tag_fat(_tmp27C,sizeof(void*),1U));});});struct _fat_ptr s=_tmp268;
{struct Cyc_List_List*x=fs;for(0;x != l;x=x->tl){
void*_tmp269=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp28=_tmp269;void*_tmp26A=_stmttmp28;unsigned _tmp26B;struct _fat_ptr*_tmp26C;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp26A)->tag == 0U){_LLD: _tmp26C=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp26A)->f1;_LLE: {struct _fat_ptr*n=_tmp26C;
s=({struct Cyc_String_pa_PrintArg_struct _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp58A;_tmp58A.tag=0U,_tmp58A.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp58A;});struct Cyc_String_pa_PrintArg_struct _tmp270=({struct Cyc_String_pa_PrintArg_struct _tmp589;_tmp589.tag=0U,_tmp589.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp589;});void*_tmp26D[2U];_tmp26D[0]=& _tmp26F,_tmp26D[1]=& _tmp270;({struct _fat_ptr _tmp73A=({const char*_tmp26E="%s.%s";_tag_fat(_tmp26E,sizeof(char),6U);});Cyc_aprintf(_tmp73A,_tag_fat(_tmp26D,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp26B=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp26A)->f1;_LL10: {unsigned n=_tmp26B;
s=({struct Cyc_String_pa_PrintArg_struct _tmp273=({struct Cyc_String_pa_PrintArg_struct _tmp58C;_tmp58C.tag=0U,_tmp58C.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp58C;});struct Cyc_Int_pa_PrintArg_struct _tmp274=({struct Cyc_Int_pa_PrintArg_struct _tmp58B;_tmp58B.tag=1U,_tmp58B.f1=(unsigned long)((int)n);_tmp58B;});void*_tmp271[2U];_tmp271[0]=& _tmp273,_tmp271[1]=& _tmp274;({struct _fat_ptr _tmp73B=({const char*_tmp272="%s.%d";_tag_fat(_tmp272,sizeof(char),6U);});Cyc_aprintf(_tmp73B,_tag_fat(_tmp271,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}}
# 1267
({struct Cyc_Warn_String_Warn_Warg_struct _tmp276=({struct Cyc_Warn_String_Warn_Warg_struct _tmp590;_tmp590.tag=0U,_tmp590.f1=(struct _fat_ptr)s;_tmp590;});struct Cyc_Warn_String_Warn_Warg_struct _tmp277=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58F;_tmp58F.tag=0U,({struct _fat_ptr _tmp73C=({const char*_tmp27B=" == ";_tag_fat(_tmp27B,sizeof(char),5U);});_tmp58F.f1=_tmp73C;});_tmp58F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp278=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58E;_tmp58E.tag=2U,_tmp58E.f1=(void*)t;_tmp58E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp279=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58D;_tmp58D.tag=0U,({struct _fat_ptr _tmp73D=({const char*_tmp27A=" is not a struct/union type";_tag_fat(_tmp27A,sizeof(char),28U);});_tmp58D.f1=_tmp73D;});_tmp58D;});void*_tmp275[4U];_tmp275[0]=& _tmp276,_tmp275[1]=& _tmp277,_tmp275[2]=& _tmp278,_tmp275[3]=& _tmp279;({unsigned _tmp73E=loc;Cyc_Warn_err2(_tmp73E,_tag_fat(_tmp275,sizeof(void*),4U));});});}}
# 1270
goto _LL0;}}else{_LL3: _tmp25A=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp259)->f1;_LL4: {unsigned n=_tmp25A;
# 1272
int bad_type=1;
{void*_tmp27F=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp27F;void*_tmp280=_stmttmp29;struct Cyc_List_List*_tmp281;struct Cyc_List_List*_tmp282;struct Cyc_Absyn_Datatypefield*_tmp283;struct Cyc_Absyn_Aggrdecl**_tmp284;switch(*((int*)_tmp280)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp280)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp280)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp284=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp280)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp284;
# 1275
if((*adp)->impl == 0)
goto _LL11;
_tmp282=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp280)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp283=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp280)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp283;
# 1295
if(({unsigned _tmp73F=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp73F < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp292=({struct Cyc_Int_pa_PrintArg_struct _tmp593;_tmp593.tag=1U,({
unsigned long _tmp740=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp593.f1=_tmp740;});_tmp593;});struct Cyc_Int_pa_PrintArg_struct _tmp293=({struct Cyc_Int_pa_PrintArg_struct _tmp592;_tmp592.tag=1U,_tmp592.f1=(unsigned long)((int)n);_tmp592;});void*_tmp290[2U];_tmp290[0]=& _tmp292,_tmp290[1]=& _tmp293;({unsigned _tmp742=loc;struct _fat_ptr _tmp741=({const char*_tmp291="datatype field has too few components: %d < %d";_tag_fat(_tmp291,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp742,_tmp741,_tag_fat(_tmp290,sizeof(void*),2U));});});else{
# 1299
if(n != (unsigned)0)
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp294=0U;({unsigned _tmp744=loc;struct _fat_ptr _tmp743=({const char*_tmp295="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp295,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp744,_tmp743,_tag_fat(_tmp294,sizeof(void*),0U));});});}}
# 1304
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp282=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp280)->f2;_LL15: {struct Cyc_List_List*fields=_tmp282;
# 1279
if(({unsigned _tmp745=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp745 <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp286=({struct Cyc_Warn_String_Warn_Warg_struct _tmp597;_tmp597.tag=0U,({struct _fat_ptr _tmp746=({const char*_tmp28B="struct/union has too few components: ";_tag_fat(_tmp28B,sizeof(char),38U);});_tmp597.f1=_tmp746;});_tmp597;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp287=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp596;_tmp596.tag=12U,({
int _tmp747=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp596.f1=_tmp747;});_tmp596;});struct Cyc_Warn_String_Warn_Warg_struct _tmp288=({struct Cyc_Warn_String_Warn_Warg_struct _tmp595;_tmp595.tag=0U,({struct _fat_ptr _tmp748=({const char*_tmp28A=" <= ";_tag_fat(_tmp28A,sizeof(char),5U);});_tmp595.f1=_tmp748;});_tmp595;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp289=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp594;_tmp594.tag=12U,_tmp594.f1=(int)n;_tmp594;});void*_tmp285[4U];_tmp285[0]=& _tmp286,_tmp285[1]=& _tmp287,_tmp285[2]=& _tmp288,_tmp285[3]=& _tmp289;({unsigned _tmp749=loc;Cyc_Warn_err2(_tmp749,_tag_fat(_tmp285,sizeof(void*),4U));});});else{
# 1283
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp281=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp280)->f1;_LL17: {struct Cyc_List_List*l=_tmp281;
# 1287
if(({unsigned _tmp74A=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp74A <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp28E=({struct Cyc_Int_pa_PrintArg_struct _tmp599;_tmp599.tag=1U,({
unsigned long _tmp74B=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp599.f1=_tmp74B;});_tmp599;});struct Cyc_Int_pa_PrintArg_struct _tmp28F=({struct Cyc_Int_pa_PrintArg_struct _tmp598;_tmp598.tag=1U,_tmp598.f1=(unsigned long)((int)n);_tmp598;});void*_tmp28C[2U];_tmp28C[0]=& _tmp28E,_tmp28C[1]=& _tmp28F;({unsigned _tmp74D=loc;struct _fat_ptr _tmp74C=({const char*_tmp28D="tuple has too few components: %d <= %d";_tag_fat(_tmp28D,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp74D,_tmp74C,_tag_fat(_tmp28C,sizeof(void*),2U));});});else{
# 1291
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1306
 goto _LL11;}_LL11:;}
# 1308
if(bad_type)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp297=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59B;_tmp59B.tag=2U,_tmp59B.f1=(void*)t;_tmp59B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp298=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59A;_tmp59A.tag=0U,({struct _fat_ptr _tmp74E=({const char*_tmp299=" is not a known type";_tag_fat(_tmp299,sizeof(char),21U);});_tmp59A.f1=_tmp74E;});_tmp59A;});void*_tmp296[2U];_tmp296[0]=& _tmp297,_tmp296[1]=& _tmp298;({unsigned _tmp74F=loc;Cyc_Warn_err2(_tmp74F,_tag_fat(_tmp296,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 1313
return Cyc_Absyn_uint_type;}}
# 1317
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp29A=t;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29A)->tag == 1U){_LL1: _LL2: {
# 1323
struct Cyc_Absyn_PtrInfo _tmp29B=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp29B;
({void*_tmp751=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp29C=_cycalloc(sizeof(*_tmp29C));({struct Cyc_List_List*_tmp750=Cyc_Tcenv_lookup_type_vars(te);_tmp29C->v=_tmp750;});_tmp29C;}));pi.elt_type=_tmp751;});{
void*_tmp29D=Cyc_Absyn_pointer_type(pi);void*new_typ=_tmp29D;
Cyc_Unify_unify(t,new_typ);
t=Cyc_Tcutil_compress(t);
goto _LL0;}}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1331
void*_tmp29E=t;void*_tmp2A2;void*_tmp2A1;void*_tmp2A0;void*_tmp29F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->tag == 3U){_LL6: _tmp29F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->f1).elt_type;_tmp2A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->f1).ptr_atts).rgn;_tmp2A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->f1).ptr_atts).bounds;_tmp2A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->f1).ptr_atts).zero_term;_LL7: {void*t2=_tmp29F;void*rt=_tmp2A0;void*b=_tmp2A1;void*zt=_tmp2A2;
# 1333
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2A3=Cyc_Tcutil_compress(t2);void*_stmttmp2A=_tmp2A3;void*_tmp2A4=_stmttmp2A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2A4)->tag == 5U){_LLB: _LLC:
# 1338
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59C;_tmp59C.tag=0U,({struct _fat_ptr _tmp752=({const char*_tmp2A7="unnecessary dereference for function type";_tag_fat(_tmp2A7,sizeof(char),42U);});_tmp59C.f1=_tmp752;});_tmp59C;});void*_tmp2A5[1U];_tmp2A5[0]=& _tmp2A6;({unsigned _tmp753=loc;Cyc_Warn_warn2(_tmp753,_tag_fat(_tmp2A5,sizeof(void*),1U));});});
return t;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59D;_tmp59D.tag=0U,({struct _fat_ptr _tmp754=({const char*_tmp2AA="cannot dereference abstract pointer type";_tag_fat(_tmp2AA,sizeof(char),41U);});_tmp59D.f1=_tmp754;});_tmp59D;});void*_tmp2A8[1U];_tmp2A8[0]=& _tmp2A9;({unsigned _tmp755=loc;Cyc_Warn_err2(_tmp755,_tag_fat(_tmp2A8,sizeof(void*),1U));});});}_LLA:;}
# 1343
return t2;}}else{_LL8: _LL9:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59F;_tmp59F.tag=0U,({struct _fat_ptr _tmp756=({const char*_tmp2AE="expecting pointer type but found ";_tag_fat(_tmp2AE,sizeof(char),34U);});_tmp59F.f1=_tmp756;});_tmp59F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2AD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59E;_tmp59E.tag=2U,_tmp59E.f1=(void*)t;_tmp59E;});void*_tmp2AB[2U];_tmp2AB[0]=& _tmp2AC,_tmp2AB[1]=& _tmp2AD;({struct Cyc_Tcenv_Tenv*_tmp759=te;unsigned _tmp758=loc;void**_tmp757=topt;Cyc_Tcexp_expr_err(_tmp759,_tmp758,_tmp757,_tag_fat(_tmp2AB,sizeof(void*),2U));});});}_LL5:;}}}
# 1348
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1352
({int _tmp75A=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp75A;});{
void*_tmp2AF=Cyc_Tcutil_compress(aggr_type);void*_stmttmp2B=_tmp2AF;void*_tmp2B0=_stmttmp2B;struct Cyc_List_List*_tmp2B2;enum Cyc_Absyn_AggrKind _tmp2B1;struct Cyc_List_List*_tmp2B4;struct Cyc_Absyn_Aggrdecl*_tmp2B3;switch(*((int*)_tmp2B0)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp2B3=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0)->f1)->f1).KnownAggr).val;_tmp2B4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2B3;struct Cyc_List_List*ts=_tmp2B4;
# 1355
struct Cyc_Absyn_Aggrfield*_tmp2B5=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2B5;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2B7=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5A3;_tmp5A3.tag=6U,_tmp5A3.f1=ad;_tmp5A3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A2;_tmp5A2.tag=0U,({struct _fat_ptr _tmp75B=({const char*_tmp2BC=" has no ";_tag_fat(_tmp2BC,sizeof(char),9U);});_tmp5A2.f1=_tmp75B;});_tmp5A2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A1;_tmp5A1.tag=0U,_tmp5A1.f1=*f;_tmp5A1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=0U,({struct _fat_ptr _tmp75C=({const char*_tmp2BB=" member";_tag_fat(_tmp2BB,sizeof(char),8U);});_tmp5A0.f1=_tmp75C;});_tmp5A0;});void*_tmp2B6[4U];_tmp2B6[0]=& _tmp2B7,_tmp2B6[1]=& _tmp2B8,_tmp2B6[2]=& _tmp2B9,_tmp2B6[3]=& _tmp2BA;({struct Cyc_Tcenv_Tenv*_tmp75F=te;unsigned _tmp75E=loc;void**_tmp75D=topt;Cyc_Tcexp_expr_err(_tmp75F,_tmp75E,_tmp75D,_tag_fat(_tmp2B6,sizeof(void*),4U));});});
# 1359
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp760=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp760,finfo->type);});{
struct Cyc_Absyn_Kind*_tmp2BD=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*t2_kind=_tmp2BD;
# 1367
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2BE=Cyc_Tcutil_compress(t2);void*_stmttmp2C=_tmp2BE;void*_tmp2BF=_stmttmp2C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BF)->tag == 4U){_LL8: _LL9:
 goto _LL7;}else{_LLA: _LLB:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=0U,({struct _fat_ptr _tmp761=({const char*_tmp2C5="cannot get member ";_tag_fat(_tmp2C5,sizeof(char),19U);});_tmp5A6.f1=_tmp761;});_tmp5A6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=0U,_tmp5A5.f1=*f;_tmp5A5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A4;_tmp5A4.tag=0U,({
struct _fat_ptr _tmp762=({const char*_tmp2C4=" since its type is abstract";_tag_fat(_tmp2C4,sizeof(char),28U);});_tmp5A4.f1=_tmp762;});_tmp5A4;});void*_tmp2C0[3U];_tmp2C0[0]=& _tmp2C1,_tmp2C0[1]=& _tmp2C2,_tmp2C0[2]=& _tmp2C3;({struct Cyc_Tcenv_Tenv*_tmp765=te;unsigned _tmp764=loc;void**_tmp763=topt;Cyc_Tcexp_expr_err(_tmp765,_tmp764,_tmp763,_tag_fat(_tmp2C0,sizeof(void*),3U));});});}_LL7:;}
# 1375
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=0U,({struct _fat_ptr _tmp766=({const char*_tmp2CB="cannot read union member ";_tag_fat(_tmp2CB,sizeof(char),26U);});_tmp5A9.f1=_tmp766;});_tmp5A9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=0U,_tmp5A8.f1=*f;_tmp5A8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=0U,({
struct _fat_ptr _tmp767=({const char*_tmp2CA=" since it is not `bits-only'";_tag_fat(_tmp2CA,sizeof(char),29U);});_tmp5A7.f1=_tmp767;});_tmp5A7;});void*_tmp2C6[3U];_tmp2C6[0]=& _tmp2C7,_tmp2C6[1]=& _tmp2C8,_tmp2C6[2]=& _tmp2C9;({struct Cyc_Tcenv_Tenv*_tmp76A=te;unsigned _tmp769=loc;void**_tmp768=topt;Cyc_Tcexp_expr_err(_tmp76A,_tmp769,_tmp768,_tag_fat(_tmp2C6,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1381
if(!({void*_tmp76B=t2;Cyc_Unify_unify(_tmp76B,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AC;_tmp5AC.tag=0U,({struct _fat_ptr _tmp76C=({const char*_tmp2D1="must use pattern-matching to access field ";_tag_fat(_tmp2D1,sizeof(char),43U);});_tmp5AC.f1=_tmp76C;});_tmp5AC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AB;_tmp5AB.tag=0U,_tmp5AB.f1=*f;_tmp5AB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=0U,({struct _fat_ptr _tmp76D=({const char*_tmp2D0="\n\tdue to existential type variables.";_tag_fat(_tmp2D0,sizeof(char),37U);});_tmp5AA.f1=_tmp76D;});_tmp5AA;});void*_tmp2CC[3U];_tmp2CC[0]=& _tmp2CD,_tmp2CC[1]=& _tmp2CE,_tmp2CC[2]=& _tmp2CF;({struct Cyc_Tcenv_Tenv*_tmp770=te;unsigned _tmp76F=loc;void**_tmp76E=topt;Cyc_Tcexp_expr_err(_tmp770,_tmp76F,_tmp76E,_tag_fat(_tmp2CC,sizeof(void*),3U));});});}
# 1384
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp2B1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B0)->f1;_tmp2B2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B0)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp2B1;struct Cyc_List_List*fs=_tmp2B2;
# 1386
struct Cyc_Absyn_Aggrfield*_tmp2D2=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2D2;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AF;_tmp5AF.tag=0U,({struct _fat_ptr _tmp771=({const char*_tmp2D8="type has no ";_tag_fat(_tmp2D8,sizeof(char),13U);});_tmp5AF.f1=_tmp771;});_tmp5AF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AE;_tmp5AE.tag=0U,_tmp5AE.f1=*f;_tmp5AE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AD;_tmp5AD.tag=0U,({struct _fat_ptr _tmp772=({const char*_tmp2D7=" member";_tag_fat(_tmp2D7,sizeof(char),8U);});_tmp5AD.f1=_tmp772;});_tmp5AD;});void*_tmp2D3[3U];_tmp2D3[0]=& _tmp2D4,_tmp2D3[1]=& _tmp2D5,_tmp2D3[2]=& _tmp2D6;({struct Cyc_Tcenv_Tenv*_tmp775=te;unsigned _tmp774=loc;void**_tmp773=topt;Cyc_Tcexp_expr_err(_tmp775,_tmp774,_tmp773,_tag_fat(_tmp2D3,sizeof(void*),3U));});});
# 1391
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1393
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B2;_tmp5B2.tag=0U,({struct _fat_ptr _tmp776=({const char*_tmp2DE="cannot read union member ";_tag_fat(_tmp2DE,sizeof(char),26U);});_tmp5B2.f1=_tmp776;});_tmp5B2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B1;_tmp5B1.tag=0U,_tmp5B1.f1=*f;_tmp5B1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B0;_tmp5B0.tag=0U,({struct _fat_ptr _tmp777=({const char*_tmp2DD=" since it is not `bits-only'";_tag_fat(_tmp2DD,sizeof(char),29U);});_tmp5B0.f1=_tmp777;});_tmp5B0;});void*_tmp2D9[3U];_tmp2D9[0]=& _tmp2DA,_tmp2D9[1]=& _tmp2DB,_tmp2D9[2]=& _tmp2DC;({struct Cyc_Tcenv_Tenv*_tmp77A=te;unsigned _tmp779=loc;void**_tmp778=topt;Cyc_Tcexp_expr_err(_tmp77A,_tmp779,_tmp778,_tag_fat(_tmp2D9,sizeof(void*),3U));});});
return finfo->type;}default: _LL5: _LL6:
# 1396
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B4;_tmp5B4.tag=0U,({struct _fat_ptr _tmp77B=({const char*_tmp2E2="expecting struct or union, found ";_tag_fat(_tmp2E2,sizeof(char),34U);});_tmp5B4.f1=_tmp77B;});_tmp5B4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2E1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B3;_tmp5B3.tag=2U,_tmp5B3.f1=(void*)aggr_type;_tmp5B3;});void*_tmp2DF[2U];_tmp2DF[0]=& _tmp2E0,_tmp2DF[1]=& _tmp2E1;({struct Cyc_Tcenv_Tenv*_tmp77E=te;unsigned _tmp77D=loc;void**_tmp77C=topt;Cyc_Tcexp_expr_err(_tmp77E,_tmp77D,_tmp77C,_tag_fat(_tmp2DF,sizeof(void*),2U));});});}_LL0:;}}
# 1401
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1405
({struct Cyc_Tcenv_Tenv*_tmp77F=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp77F,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,(void*)_check_null(e->topt),f,is_tagged,is_read);}
# 1410
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1414
void*t2=({struct Cyc_Tcenv_Tenv*_tmp781=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _tmp780=loc;Cyc_Tcexp_tcDeref(_tmp781,_tmp780,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1419
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1421
struct _tuple16 _tmp2E3=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple16 _stmttmp2D=_tmp2E3;struct _tuple16 _tmp2E4=_stmttmp2D;int _tmp2E6;unsigned _tmp2E5;_LL1: _tmp2E5=_tmp2E4.f1;_tmp2E6=_tmp2E4.f2;_LL2: {unsigned i=_tmp2E5;int known=_tmp2E6;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B5;_tmp5B5.tag=0U,({
struct _fat_ptr _tmp782=({const char*_tmp2E9="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp2E9,sizeof(char),47U);});_tmp5B5.f1=_tmp782;});_tmp5B5;});void*_tmp2E7[1U];_tmp2E7[0]=& _tmp2E8;({struct Cyc_Tcenv_Tenv*_tmp784=te;unsigned _tmp783=loc;Cyc_Tcexp_expr_err(_tmp784,_tmp783,0,_tag_fat(_tmp2E7,sizeof(void*),1U));});});{
struct _handler_cons _tmp2EA;_push_handler(& _tmp2EA);{int _tmp2EC=0;if(setjmp(_tmp2EA.handler))_tmp2EC=1;if(!_tmp2EC){{void*_tmp2ED=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp2ED;};_pop_handler();}else{void*_tmp2EB=(void*)Cyc_Core_get_exn_thrown();void*_tmp2EE=_tmp2EB;void*_tmp2EF;if(((struct Cyc_List_Nth_exn_struct*)_tmp2EE)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1427
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BA;_tmp5BA.tag=0U,({struct _fat_ptr _tmp785=({const char*_tmp2F8="index is ";_tag_fat(_tmp2F8,sizeof(char),10U);});_tmp5BA.f1=_tmp785;});_tmp5BA;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2F2=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B9;_tmp5B9.tag=12U,_tmp5B9.f1=(int)i;_tmp5B9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B8;_tmp5B8.tag=0U,({struct _fat_ptr _tmp786=({const char*_tmp2F7=" but tuple has only ";_tag_fat(_tmp2F7,sizeof(char),21U);});_tmp5B8.f1=_tmp786;});_tmp5B8;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2F4=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B7;_tmp5B7.tag=12U,({
int _tmp787=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp5B7.f1=_tmp787;});_tmp5B7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B6;_tmp5B6.tag=0U,({struct _fat_ptr _tmp788=({const char*_tmp2F6=" fields";_tag_fat(_tmp2F6,sizeof(char),8U);});_tmp5B6.f1=_tmp788;});_tmp5B6;});void*_tmp2F0[5U];_tmp2F0[0]=& _tmp2F1,_tmp2F0[1]=& _tmp2F2,_tmp2F0[2]=& _tmp2F3,_tmp2F0[3]=& _tmp2F4,_tmp2F0[4]=& _tmp2F5;({struct Cyc_Tcenv_Tenv*_tmp78A=te;unsigned _tmp789=loc;Cyc_Tcexp_expr_err(_tmp78A,_tmp789,0,_tag_fat(_tmp2F0,sizeof(void*),5U));});});}else{_LL6: _tmp2EF=_tmp2EE;_LL7: {void*exn=_tmp2EF;(int)_rethrow(exn);}}_LL3:;}}}}}
# 1432
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1434
struct Cyc_Tcenv_Tenv*_tmp2F9=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp2F9;
({struct Cyc_Tcenv_Tenv*_tmp78B=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp78B,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp78C=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp78C,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BC;_tmp5BC.tag=0U,({struct _fat_ptr _tmp78D=({const char*_tmp2FD="expecting int subscript, found ";_tag_fat(_tmp2FD,sizeof(char),32U);});_tmp5BC.f1=_tmp78D;});_tmp5BC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BB;_tmp5BB.tag=2U,_tmp5BB.f1=(void*)t2;_tmp5BB;});void*_tmp2FA[2U];_tmp2FA[0]=& _tmp2FB,_tmp2FA[1]=& _tmp2FC;({struct Cyc_Tcenv_Tenv*_tmp790=te;unsigned _tmp78F=e2->loc;void**_tmp78E=topt;Cyc_Tcexp_expr_err(_tmp790,_tmp78F,_tmp78E,_tag_fat(_tmp2FA,sizeof(void*),2U));});});{
# 1443
void*_tmp2FE=t1;struct Cyc_List_List*_tmp2FF;void*_tmp304;void*_tmp303;void*_tmp302;struct Cyc_Absyn_Tqual _tmp301;void*_tmp300;switch(*((int*)_tmp2FE)){case 3U: _LL1: _tmp300=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).elt_type;_tmp301=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).elt_tq;_tmp302=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).rgn;_tmp303=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).bounds;_tmp304=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp300;struct Cyc_Absyn_Tqual tq=_tmp301;void*rt=_tmp302;void*b=_tmp303;void*zt=_tmp304;
# 1447
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp305=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp305;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp306=eopt;struct Cyc_Absyn_Exp*e3=_tmp306;
struct _tuple16 _tmp307=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2E=_tmp307;struct _tuple16 _tmp308=_stmttmp2E;int _tmp30A;unsigned _tmp309;_LL8: _tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;_LL9: {unsigned i=_tmp309;int known_i=_tmp30A;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp30B=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2F=_tmp30B;struct _tuple16 _tmp30C=_stmttmp2F;int _tmp30E;unsigned _tmp30D;_LLB: _tmp30D=_tmp30C.f1;_tmp30E=_tmp30C.f2;_LLC: {unsigned j=_tmp30D;int known_j=_tmp30E;
if(known_j){
struct _tuple16 _tmp30F=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp30=_tmp30F;struct _tuple16 _tmp310=_stmttmp30;int _tmp312;unsigned _tmp311;_LLE: _tmp311=_tmp310.f1;_tmp312=_tmp310.f2;_LLF: {unsigned j=_tmp311;int knownj=_tmp312;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp313=0U;({unsigned _tmp792=loc;struct _fat_ptr _tmp791=({const char*_tmp314="subscript will fail at run-time";_tag_fat(_tmp314,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp792,_tmp791,_tag_fat(_tmp313,sizeof(void*),0U));});});}}}}}}
# 1463
if(emit_warning)
({void*_tmp315=0U;({unsigned _tmp794=e2->loc;struct _fat_ptr _tmp793=({const char*_tmp316="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp316,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp794,_tmp793,_tag_fat(_tmp315,sizeof(void*),0U));});});}else{
# 1467
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp317=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp31=_tmp317;struct _tuple16 _tmp318=_stmttmp31;int _tmp31A;unsigned _tmp319;_LL11: _tmp319=_tmp318.f1;_tmp31A=_tmp318.f2;_LL12: {unsigned i=_tmp319;int known=_tmp31A;
if(known)
# 1472
({unsigned _tmp797=loc;unsigned _tmp796=i;void*_tmp795=b;Cyc_Tcutil_check_bound(_tmp797,_tmp796,_tmp795,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1475
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp31B=0U;({unsigned _tmp799=e1->loc;struct _fat_ptr _tmp798=({const char*_tmp31C="subscript applied to pointer to one object";_tag_fat(_tmp31C,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp799,_tmp798,_tag_fat(_tmp31B,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1480
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp31D=0U;({unsigned _tmp79B=e1->loc;struct _fat_ptr _tmp79A=({const char*_tmp31E="can't subscript an abstract pointer";_tag_fat(_tmp31E,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp79B,_tmp79A,_tag_fat(_tmp31D,sizeof(void*),0U));});});
return t;}case 6U: _LL3: _tmp2FF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FE)->f1;_LL4: {struct Cyc_List_List*ts=_tmp2FF;
return Cyc_Tcexp_ithTupleType(te,loc,ts,e2);}default: _LL5: _LL6:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp320=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BE;_tmp5BE.tag=0U,({struct _fat_ptr _tmp79C=({const char*_tmp322="subscript applied to ";_tag_fat(_tmp322,sizeof(char),22U);});_tmp5BE.f1=_tmp79C;});_tmp5BE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp321=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BD;_tmp5BD.tag=2U,_tmp5BD.f1=(void*)t1;_tmp5BD;});void*_tmp31F[2U];_tmp31F[0]=& _tmp320,_tmp31F[1]=& _tmp321;({struct Cyc_Tcenv_Tenv*_tmp79F=te;unsigned _tmp79E=loc;void**_tmp79D=topt;Cyc_Tcexp_expr_err(_tmp79F,_tmp79E,_tmp79D,_tag_fat(_tmp31F,sizeof(void*),2U));});});}_LL0:;}}}
# 1490
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp323=Cyc_Tcutil_compress(*topt);void*_stmttmp32=_tmp323;void*_tmp324=_stmttmp32;struct Cyc_List_List*_tmp325;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp324)->tag == 6U){_LL1: _tmp325=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp324)->f1;_LL2: {struct Cyc_List_List*ts=_tmp325;
# 1497
if(({int _tmp7A0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp7A0 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
goto _LL0;
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp326=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp326;
({struct Cyc_Tcenv_Tenv*_tmp7A1=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7A1,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1504
({struct Cyc_RgnOrder_RgnPO*_tmp7A3=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7A2=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp7A3,_tmp7A2,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp328=_cycalloc(sizeof(*_tmp328));({struct _tuple17*_tmp7A4=({struct _tuple17*_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp327->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp327;});_tmp328->hd=_tmp7A4;}),_tmp328->tl=fields;_tmp328;});}
# 1507
done=1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1511
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp7A5=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7A5,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp32A=_cycalloc(sizeof(*_tmp32A));({struct _tuple17*_tmp7A7=({struct _tuple17*_tmp329=_cycalloc(sizeof(*_tmp329));({struct Cyc_Absyn_Tqual _tmp7A6=Cyc_Absyn_empty_tqual(0U);_tmp329->f1=_tmp7A6;}),_tmp329->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp329;});_tmp32A->hd=_tmp7A7;}),_tmp32A->tl=fields;_tmp32A;});}
# 1516
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->tag=6U,({struct Cyc_List_List*_tmp7A8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp32B->f1=_tmp7A8;});_tmp32B;});}
# 1520
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1525
struct _tuple9*_tmp32C=targ;void*_tmp32D;_LL1: _tmp32D=_tmp32C->f3;_LL2: {void*t=_tmp32D;
({unsigned _tmp7AC=loc;struct Cyc_Tcenv_Tenv*_tmp7AB=te;struct Cyc_List_List*_tmp7AA=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp7A9=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1526
Cyc_Tctyp_check_type(_tmp7AC,_tmp7AB,_tmp7AA,_tmp7A9,1,0,t);});
# 1528
({void*_tmp7AD=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->tag=36U,_tmp32E->f1=0,_tmp32E->f2=des;_tmp32E;});orig_exp->r=_tmp7AD;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1541 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1544
void*res_t2;
int _tmp32F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp32F;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp330=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp346=_cycalloc(sizeof(*_tmp346));_tmp346->tag=0U,({union Cyc_Absyn_Cnst _tmp7AE=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp346->f1=_tmp7AE;});_tmp346;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp330;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1552
if(zero_term){
struct Cyc_Absyn_Exp*_tmp331=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp331;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp332=0U;({unsigned _tmp7B0=e->loc;struct _fat_ptr _tmp7AF=({const char*_tmp333="zero-terminated array doesn't end with zero.";_tag_fat(_tmp333,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp7B0,_tmp7AF,_tag_fat(_tmp332,sizeof(void*),0U));});});}
# 1557
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp7B3=res;struct Cyc_Absyn_Tqual _tmp7B2=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1558
struct Cyc_Absyn_Exp*_tmp7B1=sz_exp;Cyc_Absyn_array_type(_tmp7B3,_tmp7B2,_tmp7B1,
# 1560
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1562
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1565
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7B5=Cyc_Tcenv_curr_rgnpo(te);void*_tmp7B4=res;Cyc_Tcutil_coerce_list(_tmp7B5,_tmp7B4,es);}))
# 1567
({struct Cyc_String_pa_PrintArg_struct _tmp336=({struct Cyc_String_pa_PrintArg_struct _tmp5BF;_tmp5BF.tag=0U,({
struct _fat_ptr _tmp7B6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(res));_tmp5BF.f1=_tmp7B6;});_tmp5BF;});void*_tmp334[1U];_tmp334[0]=& _tmp336;({unsigned _tmp7B8=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp7B7=({const char*_tmp335="elements of array do not all have the same type (%s)";_tag_fat(_tmp335,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp7B8,_tmp7B7,_tag_fat(_tmp334,sizeof(void*),1U));});});
# 1570
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1575
void*_tmp337=(void*)ds->hd;void*_stmttmp33=_tmp337;void*_tmp338=_stmttmp33;struct Cyc_Absyn_Exp*_tmp339;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp338)->tag == 1U){_LL1: _LL2:
# 1577
({void*_tmp33A=0U;({unsigned _tmp7BA=loc;struct _fat_ptr _tmp7B9=({const char*_tmp33B="only array index designators are supported";_tag_fat(_tmp33B,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp7BA,_tmp7B9,_tag_fat(_tmp33A,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp339=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp338)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp339;
# 1580
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp33C=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp34=_tmp33C;struct _tuple16 _tmp33D=_stmttmp34;int _tmp33F;unsigned _tmp33E;_LL6: _tmp33E=_tmp33D.f1;_tmp33F=_tmp33D.f2;_LL7: {unsigned i=_tmp33E;int known=_tmp33F;
if(!known)
({void*_tmp340=0U;({unsigned _tmp7BC=e->loc;struct _fat_ptr _tmp7BB=({const char*_tmp341="index designator cannot use sizeof or offsetof";_tag_fat(_tmp341,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp7BC,_tmp7BB,_tag_fat(_tmp340,sizeof(void*),0U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp344=({struct Cyc_Int_pa_PrintArg_struct _tmp5C1;_tmp5C1.tag=1U,_tmp5C1.f1=(unsigned long)offset;_tmp5C1;});struct Cyc_Int_pa_PrintArg_struct _tmp345=({struct Cyc_Int_pa_PrintArg_struct _tmp5C0;_tmp5C0.tag=1U,_tmp5C0.f1=(unsigned long)((int)i);_tmp5C0;});void*_tmp342[2U];_tmp342[0]=& _tmp344,_tmp342[1]=& _tmp345;({unsigned _tmp7BE=e->loc;struct _fat_ptr _tmp7BD=({const char*_tmp343="expecting index designator %d but found %d";_tag_fat(_tmp343,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp7BE,_tmp7BD,_tag_fat(_tmp342,sizeof(void*),2U));});});}
# 1587
goto _LL0;}}}}_LL0:;}}}
# 1591
return res_t2;}
# 1595
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1598
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp347=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp35=_tmp347;void*_tmp348=_stmttmp35;void*_tmp349;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp348)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp348)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp348)->f2 != 0){_LL1: _tmp349=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp348)->f2)->hd;_LL2: {void*t=_tmp349;
# 1603
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp34A=({void*_tmp7BF=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7BF,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp34A;
b->topt=bound->topt;
# 1608
bound=b;}
# 1610
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1612
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp34C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3;_tmp5C3.tag=0U,({struct _fat_ptr _tmp7C0=({const char*_tmp34E="expecting unsigned int, found ";_tag_fat(_tmp34E,sizeof(char),31U);});_tmp5C3.f1=_tmp7C0;});_tmp5C3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp34D=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5C2;_tmp5C2.tag=3U,_tmp5C2.f1=(void*)bound->topt;_tmp5C2;});void*_tmp34B[2U];_tmp34B[0]=& _tmp34C,_tmp34B[1]=& _tmp34D;({unsigned _tmp7C1=bound->loc;Cyc_Warn_err2(_tmp7C1,_tag_fat(_tmp34B,sizeof(void*),2U));});});}_LL0:;}
# 1616
if(!(vd->tq).real_const)
({void*_tmp34F=0U;({struct _fat_ptr _tmp7C2=({const char*_tmp350="comprehension index variable is not declared const!";_tag_fat(_tmp350,sizeof(char),52U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp7C2,_tag_fat(_tmp34F,sizeof(void*),0U));});});
# 1619
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
void**_tmp351=0;void**topt2=_tmp351;
struct Cyc_Absyn_Tqual*_tmp352=0;struct Cyc_Absyn_Tqual*tqopt=_tmp352;
void**_tmp353=0;void**ztopt=_tmp353;
# 1625
if(topt != 0){
void*_tmp354=Cyc_Tcutil_compress(*topt);void*_stmttmp36=_tmp354;void*_tmp355=_stmttmp36;void*_tmp359;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Tqual _tmp357;void*_tmp356;struct Cyc_Absyn_PtrInfo _tmp35A;switch(*((int*)_tmp355)){case 3U: _LL6: _tmp35A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp355)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp35A;
# 1628
topt2=({void**_tmp35B=_cycalloc(sizeof(*_tmp35B));*_tmp35B=x.elt_type;_tmp35B;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp35C=_cycalloc(sizeof(*_tmp35C));*_tmp35C=x.elt_tq;_tmp35C;});
ztopt=({void**_tmp35D=_cycalloc(sizeof(*_tmp35D));*_tmp35D=(x.ptr_atts).zero_term;_tmp35D;});
goto _LL5;}case 4U: _LL8: _tmp356=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp355)->f1).elt_type;_tmp357=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp355)->f1).tq;_tmp358=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp355)->f1).num_elts;_tmp359=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp355)->f1).zero_term;_LL9: {void*t=_tmp356;struct Cyc_Absyn_Tqual tq=_tmp357;struct Cyc_Absyn_Exp*b=_tmp358;void*zt=_tmp359;
# 1633
topt2=({void**_tmp35E=_cycalloc(sizeof(*_tmp35E));*_tmp35E=t;_tmp35E;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp35F=_cycalloc(sizeof(*_tmp35F));*_tmp35F=tq;_tmp35F;});
ztopt=({void**_tmp360=_cycalloc(sizeof(*_tmp360));*_tmp360=zt;_tmp360;});
goto _LL5;}default: _LLA: _LLB:
# 1638
 goto _LL5;}_LL5:;}{
# 1641
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1643
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(body))
Cyc_Tcexp_unique_consume_err(body->loc);
if(te->le == 0){
# 1647
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp361=0U;({unsigned _tmp7C4=bound->loc;struct _fat_ptr _tmp7C3=({const char*_tmp362="bound is not constant";_tag_fat(_tmp362,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp7C4,_tmp7C3,_tag_fat(_tmp361,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp363=0U;({unsigned _tmp7C6=bound->loc;struct _fat_ptr _tmp7C5=({const char*_tmp364="body is not constant";_tag_fat(_tmp364,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp7C6,_tmp7C5,_tag_fat(_tmp363,sizeof(void*),0U));});});}
# 1652
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1655
struct Cyc_Absyn_Exp*_tmp365=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp365;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1659
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt))&& !
Cyc_Tcutil_is_noalias_path(body))
Cyc_Tcexp_unique_consume_err(body->loc);{
# 1663
void*_tmp366=({void*_tmp7C9=t;struct Cyc_Absyn_Tqual _tmp7C8=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp7C7=bound;Cyc_Absyn_array_type(_tmp7C9,_tmp7C8,_tmp7C7,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1663
void*res=_tmp366;
# 1665
return res;}}}}
# 1669
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1672
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp367=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp37=_tmp367;void*_tmp368=_stmttmp37;void*_tmp369;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368)->f2 != 0){_LL1: _tmp369=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368)->f2)->hd;_LL2: {void*t=_tmp369;
# 1677
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp36A=({void*_tmp7CA=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7CA,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp36A;
b->topt=bound->topt;
bound=b;}
# 1682
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1684
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C5;_tmp5C5.tag=0U,({struct _fat_ptr _tmp7CB=({const char*_tmp36E="expecting unsigned int, found ";_tag_fat(_tmp36E,sizeof(char),31U);});_tmp5C5.f1=_tmp7CB;});_tmp5C5;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp36D=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5C4;_tmp5C4.tag=3U,_tmp5C4.f1=(void*)bound->topt;_tmp5C4;});void*_tmp36B[2U];_tmp36B[0]=& _tmp36C,_tmp36B[1]=& _tmp36D;({unsigned _tmp7CC=bound->loc;Cyc_Warn_err2(_tmp7CC,_tag_fat(_tmp36B,sizeof(void*),2U));});});}_LL0:;}{
# 1688
void**_tmp36F=0;void**topt2=_tmp36F;
struct Cyc_Absyn_Tqual*_tmp370=0;struct Cyc_Absyn_Tqual*tqopt=_tmp370;
void**_tmp371=0;void**ztopt=_tmp371;
# 1692
if(topt != 0){
void*_tmp372=Cyc_Tcutil_compress(*topt);void*_stmttmp38=_tmp372;void*_tmp373=_stmttmp38;void*_tmp377;struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Tqual _tmp375;void*_tmp374;struct Cyc_Absyn_PtrInfo _tmp378;switch(*((int*)_tmp373)){case 3U: _LL6: _tmp378=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp378;
# 1695
topt2=({void**_tmp379=_cycalloc(sizeof(*_tmp379));*_tmp379=x.elt_type;_tmp379;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp37A=_cycalloc(sizeof(*_tmp37A));*_tmp37A=x.elt_tq;_tmp37A;});
ztopt=({void**_tmp37B=_cycalloc(sizeof(*_tmp37B));*_tmp37B=(x.ptr_atts).zero_term;_tmp37B;});
goto _LL5;}case 4U: _LL8: _tmp374=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373)->f1).elt_type;_tmp375=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373)->f1).tq;_tmp376=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373)->f1).num_elts;_tmp377=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373)->f1).zero_term;_LL9: {void*t=_tmp374;struct Cyc_Absyn_Tqual tq=_tmp375;struct Cyc_Absyn_Exp*b=_tmp376;void*zt=_tmp377;
# 1700
topt2=({void**_tmp37C=_cycalloc(sizeof(*_tmp37C));*_tmp37C=t;_tmp37C;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp37D=_cycalloc(sizeof(*_tmp37D));*_tmp37D=tq;_tmp37D;});
ztopt=({void**_tmp37E=_cycalloc(sizeof(*_tmp37E));*_tmp37E=zt;_tmp37E;});
goto _LL5;}default: _LLA: _LLB:
# 1705
 goto _LL5;}_LL5:;}
# 1708
({unsigned _tmp7CF=loc;struct Cyc_Tcenv_Tenv*_tmp7CE=te;struct Cyc_List_List*_tmp7CD=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7CF,_tmp7CE,_tmp7CD,& Cyc_Tcutil_tmk,1,1,t);});
if(topt2 != 0)Cyc_Unify_unify(*topt2,t);
# 1711
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp37F=0U;({unsigned _tmp7D1=bound->loc;struct _fat_ptr _tmp7D0=({const char*_tmp380="bound is not constant";_tag_fat(_tmp380,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp7D1,_tmp7D0,_tag_fat(_tmp37F,sizeof(void*),0U));});});}
# 1715
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1718
struct Cyc_Absyn_Exp*_tmp381=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp381;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;
# 1722
({void*_tmp382=0U;({unsigned _tmp7D3=loc;struct _fat_ptr _tmp7D2=({const char*_tmp383="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp383,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp7D3,_tmp7D2,_tag_fat(_tmp382,sizeof(void*),0U));});});}{
# 1725
void*_tmp384=({void*_tmp7D6=t;struct Cyc_Absyn_Tqual _tmp7D5=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp7D4=bound;Cyc_Absyn_array_type(_tmp7D6,_tmp7D5,_tmp7D4,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1725
void*res=_tmp384;
# 1727
return res;}}}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1740 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1744
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp385=_cycalloc(sizeof(*_tmp385));*_tmp385=ad;_tmp385;});}else{
# 1750
{struct _handler_cons _tmp386;_push_handler(& _tmp386);{int _tmp388=0;if(setjmp(_tmp386.handler))_tmp388=1;if(!_tmp388){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1750
;_pop_handler();}else{void*_tmp387=(void*)Cyc_Core_get_exn_thrown();void*_tmp389=_tmp387;void*_tmp38A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp389)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1753
({struct Cyc_String_pa_PrintArg_struct _tmp38D=({struct Cyc_String_pa_PrintArg_struct _tmp5C6;_tmp5C6.tag=0U,({struct _fat_ptr _tmp7D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp5C6.f1=_tmp7D7;});_tmp5C6;});void*_tmp38B[1U];_tmp38B[0]=& _tmp38D;({unsigned _tmp7D9=loc;struct _fat_ptr _tmp7D8=({const char*_tmp38C="unbound struct/union name %s";_tag_fat(_tmp38C,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp7D9,_tmp7D8,_tag_fat(_tmp38B,sizeof(void*),1U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp38A=_tmp389;_LL4: {void*exn=_tmp38A;(int)_rethrow(exn);}}_LL0:;}}}
# 1756
*ad_opt=ad;
*tn=ad->name;}
# 1759
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp390=({struct Cyc_String_pa_PrintArg_struct _tmp5C7;_tmp5C7.tag=0U,({struct _fat_ptr _tmp7DA=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp391="struct";_tag_fat(_tmp391,sizeof(char),7U);}):({const char*_tmp392="union";_tag_fat(_tmp392,sizeof(char),6U);}));_tmp5C7.f1=_tmp7DA;});_tmp5C7;});void*_tmp38E[1U];_tmp38E[0]=& _tmp390;({unsigned _tmp7DC=loc;struct _fat_ptr _tmp7DB=({const char*_tmp38F="can't construct abstract %s";_tag_fat(_tmp38F,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp7DC,_tmp7DB,_tag_fat(_tmp38E,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1767
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp393=Cyc_Tcenv_new_status(te);enum Cyc_Tcenv_NewStatus status=_tmp393;
if((int)status == (int)1U)
te2=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);else{
# 1776
te2=Cyc_Tcenv_set_new_status(status,te);}{
# 1778
struct _tuple13 _tmp394=({struct _tuple13 _tmp5D0;({struct Cyc_List_List*_tmp7DD=Cyc_Tcenv_lookup_type_vars(te2);_tmp5D0.f1=_tmp7DD;}),_tmp5D0.f2=Cyc_Core_heap_region;_tmp5D0;});struct _tuple13 env=_tmp394;
struct Cyc_List_List*_tmp395=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp395;
struct Cyc_List_List*_tmp396=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp396;
struct Cyc_List_List*_tmp397=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp397;
struct Cyc_List_List*_tmp398=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp398;
struct Cyc_List_List*_tmp399=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp399;
void*res_typ;
# 1789
if(topt != 0){
void*_tmp39A=Cyc_Tcutil_compress(*topt);void*_stmttmp39=_tmp39A;void*_tmp39B=_stmttmp39;struct Cyc_List_List*_tmp39D;struct Cyc_Absyn_Aggrdecl**_tmp39C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp39C=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->f1)->f1).KnownAggr).val;_tmp39D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp39C;struct Cyc_List_List*all_typs2=_tmp39D;
# 1792
if(*adptr2 == *adptr){
{struct Cyc_List_List*_tmp39E=all_typs;struct Cyc_List_List*ats=_tmp39E;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1797
res_typ=*topt;
goto _LL5;}
# 1800
goto _LL9;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1802
 res_typ=({union Cyc_Absyn_AggrInfo _tmp7DE=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp7DE,all_typs);});}_LL5:;}else{
# 1805
res_typ=({union Cyc_Absyn_AggrInfo _tmp7DF=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp7DF,all_typs);});}{
# 1808
struct Cyc_List_List*_tmp39F=*ts;struct Cyc_List_List*user_ex_ts=_tmp39F;
struct Cyc_List_List*_tmp3A0=exist_typs;struct Cyc_List_List*ex_ts=_tmp3A0;
while(user_ex_ts != 0 && ex_ts != 0){
# 1812
({unsigned _tmp7E2=loc;struct Cyc_Tcenv_Tenv*_tmp7E1=te2;struct Cyc_List_List*_tmp7E0=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp7E2,_tmp7E1,_tmp7E0,& Cyc_Tcutil_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1818
if(user_ex_ts != 0)
({void*_tmp3A1=0U;({unsigned _tmp7E4=loc;struct _fat_ptr _tmp7E3=({const char*_tmp3A2="too many explicit witness types";_tag_fat(_tmp3A2,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7E4,_tmp7E3,_tag_fat(_tmp3A1,sizeof(void*),0U));});});
# 1821
*ts=exist_typs;{
# 1824
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3A3=(struct _tuple19*)fields->hd;struct _tuple19*_stmttmp3A=_tmp3A3;struct _tuple19*_tmp3A4=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Aggrfield*_tmp3A5;_LLB: _tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A4->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp3A5;struct Cyc_Absyn_Exp*field_exp=_tmp3A6;
void*_tmp3A7=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp3A7;
({struct Cyc_Tcenv_Tenv*_tmp7E5=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp7E5,& inst_fieldtyp,field_exp);});
# 1832
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7E7=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp7E6=field_exp;Cyc_Tcutil_coerce_arg(_tmp7E7,_tmp7E6,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CF;_tmp5CF.tag=0U,({struct _fat_ptr _tmp7E8=({const char*_tmp3B4="field ";_tag_fat(_tmp3B4,sizeof(char),7U);});_tmp5CF.f1=_tmp7E8;});_tmp5CF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CE;_tmp5CE.tag=0U,_tmp5CE.f1=*field->name;_tmp5CE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CD;_tmp5CD.tag=0U,({struct _fat_ptr _tmp7E9=({const char*_tmp3B3=" of ";_tag_fat(_tmp3B3,sizeof(char),5U);});_tmp5CD.f1=_tmp7E9;});_tmp5CD;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3AC=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5CC;_tmp5CC.tag=6U,_tmp5CC.f1=ad;_tmp5CC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=0U,({
struct _fat_ptr _tmp7EA=({const char*_tmp3B2=" expects type ";_tag_fat(_tmp3B2,sizeof(char),15U);});_tmp5CB.f1=_tmp7EA;});_tmp5CB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3AE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=2U,_tmp5CA.f1=(void*)inst_fieldtyp;_tmp5CA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=0U,({struct _fat_ptr _tmp7EB=({const char*_tmp3B1=" != ";_tag_fat(_tmp3B1,sizeof(char),5U);});_tmp5C9.f1=_tmp7EB;});_tmp5C9;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3B0=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5C8;_tmp5C8.tag=3U,_tmp5C8.f1=(void*)field_exp->topt;_tmp5C8;});void*_tmp3A8[8U];_tmp3A8[0]=& _tmp3A9,_tmp3A8[1]=& _tmp3AA,_tmp3A8[2]=& _tmp3AB,_tmp3A8[3]=& _tmp3AC,_tmp3A8[4]=& _tmp3AD,_tmp3A8[5]=& _tmp3AE,_tmp3A8[6]=& _tmp3AF,_tmp3A8[7]=& _tmp3B0;({unsigned _tmp7EC=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp7EC,_tag_fat(_tmp3A8,sizeof(void*),8U));});});}}{
# 1837
struct Cyc_List_List*_tmp3B5=0;struct Cyc_List_List*rpo_inst=_tmp3B5;
{struct Cyc_List_List*_tmp3B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3B6;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct _tuple0*_tmp7EF=({struct _tuple0*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({void*_tmp7EE=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3B7->f1=_tmp7EE;}),({
void*_tmp7ED=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3B7->f2=_tmp7ED;});_tmp3B7;});
# 1839
_tmp3B8->hd=_tmp7EF;}),_tmp3B8->tl=rpo_inst;_tmp3B8;});}}
# 1842
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}}
# 1848
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1850
{void*_tmp3B9=Cyc_Tcutil_compress(ts);void*_stmttmp3B=_tmp3B9;void*_tmp3BA=_stmttmp3B;struct Cyc_List_List*_tmp3BC;enum Cyc_Absyn_AggrKind _tmp3BB;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3BA)->tag == 7U){_LL1: _tmp3BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3BA)->f1;_tmp3BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3BA)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp3BB;struct Cyc_List_List*fs=_tmp3BC;
# 1852
if((int)k == (int)1U)
({void*_tmp3BD=0U;({unsigned _tmp7F1=loc;struct _fat_ptr _tmp7F0=({const char*_tmp3BE="expecting struct but found union";_tag_fat(_tmp3BE,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp7F1,_tmp7F0,_tag_fat(_tmp3BD,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3BF=(struct _tuple19*)fields->hd;struct _tuple19*_stmttmp3C=_tmp3BF;struct _tuple19*_tmp3C0=_stmttmp3C;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Aggrfield*_tmp3C1;_LL6: _tmp3C1=_tmp3C0->f1;_tmp3C2=_tmp3C0->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp3C1;struct Cyc_Absyn_Exp*field_exp=_tmp3C2;
({struct Cyc_Tcenv_Tenv*_tmp7F3=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp7F2=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp7F3,_tmp7F2,field_exp);});
# 1861
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7F5=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7F4=field_exp;Cyc_Tcutil_coerce_arg(_tmp7F5,_tmp7F4,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D6;_tmp5D6.tag=0U,({struct _fat_ptr _tmp7F6=({const char*_tmp3CC="field ";_tag_fat(_tmp3CC,sizeof(char),7U);});_tmp5D6.f1=_tmp7F6;});_tmp5D6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D5;_tmp5D5.tag=0U,_tmp5D5.f1=*field->name;_tmp5D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=0U,({
struct _fat_ptr _tmp7F7=({const char*_tmp3CB=" of struct expects type ";_tag_fat(_tmp3CB,sizeof(char),25U);});_tmp5D4.f1=_tmp7F7;});_tmp5D4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3C7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=2U,_tmp5D3.f1=(void*)field->type;_tmp5D3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D2;_tmp5D2.tag=0U,({struct _fat_ptr _tmp7F8=({const char*_tmp3CA=" != ";_tag_fat(_tmp3CA,sizeof(char),5U);});_tmp5D2.f1=_tmp7F8;});_tmp5D2;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3C9=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5D1;_tmp5D1.tag=3U,_tmp5D1.f1=(void*)field_exp->topt;_tmp5D1;});void*_tmp3C3[6U];_tmp3C3[0]=& _tmp3C4,_tmp3C3[1]=& _tmp3C5,_tmp3C3[2]=& _tmp3C6,_tmp3C3[3]=& _tmp3C7,_tmp3C3[4]=& _tmp3C8,_tmp3C3[5]=& _tmp3C9;({unsigned _tmp7F9=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp7F9,_tag_fat(_tmp3C3,sizeof(void*),6U));});});}}
# 1866
goto _LL0;}}}else{_LL3: _LL4:
({void*_tmp3CD=0U;({struct _fat_ptr _tmp7FA=({const char*_tmp3CE="tcAnonStruct: wrong type";_tag_fat(_tmp3CE,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp7FA,_tag_fat(_tmp3CD,sizeof(void*),0U));});});}_LL0:;}
# 1869
return ts;}
# 1873
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1876
struct _tuple13 _tmp3CF=({struct _tuple13 _tmp5DE;({struct Cyc_List_List*_tmp7FB=Cyc_Tcenv_lookup_type_vars(te);_tmp5DE.f1=_tmp7FB;}),_tmp5DE.f2=Cyc_Core_heap_region;_tmp5DE;});struct _tuple13 env=_tmp3CF;
struct Cyc_List_List*_tmp3D0=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3D0;
struct Cyc_List_List*_tmp3D1=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp3D1;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp7FC=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp7FC,all_typs);});
# 1881
if(topt != 0){
void*_tmp3D2=Cyc_Tcutil_compress(*topt);void*_stmttmp3D=_tmp3D2;void*_tmp3D3=_stmttmp3D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D3)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D3)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1887
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(inst != 0)t=Cyc_Tcutil_substitute(inst,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7FE=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7FD=e;Cyc_Tcutil_coerce_arg(_tmp7FE,_tmp7FD,t,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp3D6=({struct Cyc_String_pa_PrintArg_struct _tmp5D9;_tmp5D9.tag=0U,({
# 1897
struct _fat_ptr _tmp7FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp5D9.f1=_tmp7FF;});_tmp5D9;});struct Cyc_String_pa_PrintArg_struct _tmp3D7=({struct Cyc_String_pa_PrintArg_struct _tmp5D8;_tmp5D8.tag=0U,({struct _fat_ptr _tmp800=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5D8.f1=_tmp800;});_tmp5D8;});struct Cyc_String_pa_PrintArg_struct _tmp3D8=({struct Cyc_String_pa_PrintArg_struct _tmp5D7;_tmp5D7.tag=0U,({
struct _fat_ptr _tmp801=(struct _fat_ptr)((struct _fat_ptr)(e->topt == 0?(struct _fat_ptr)({const char*_tmp3D9="?";_tag_fat(_tmp3D9,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp5D7.f1=_tmp801;});_tmp5D7;});void*_tmp3D4[3U];_tmp3D4[0]=& _tmp3D6,_tmp3D4[1]=& _tmp3D7,_tmp3D4[2]=& _tmp3D8;({unsigned _tmp803=e->loc;struct _fat_ptr _tmp802=({const char*_tmp3D5="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_fat(_tmp3D5,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp803,_tmp802,_tag_fat(_tmp3D4,sizeof(void*),3U));});});
Cyc_Unify_explain_failure();}}
# 1902
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DB;_tmp5DB.tag=0U,({
struct _fat_ptr _tmp804=({const char*_tmp3DD="too many arguments for datatype constructor ";_tag_fat(_tmp3DD,sizeof(char),45U);});_tmp5DB.f1=_tmp804;});_tmp5DB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3DC=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5DA;_tmp5DA.tag=1U,_tmp5DA.f1=tuf->name;_tmp5DA;});void*_tmp3DA[2U];_tmp3DA[0]=& _tmp3DB,_tmp3DA[1]=& _tmp3DC;({struct Cyc_Tcenv_Tenv*_tmp807=te;unsigned _tmp806=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp805=topt;Cyc_Tcexp_expr_err(_tmp807,_tmp806,_tmp805,_tag_fat(_tmp3DA,sizeof(void*),2U));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DD;_tmp5DD.tag=0U,({
struct _fat_ptr _tmp808=({const char*_tmp3E1="too few arguments for datatype constructor ";_tag_fat(_tmp3E1,sizeof(char),44U);});_tmp5DD.f1=_tmp808;});_tmp5DD;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3E0=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=1U,_tmp5DC.f1=tuf->name;_tmp5DC;});void*_tmp3DE[2U];_tmp3DE[0]=& _tmp3DF,_tmp3DE[1]=& _tmp3E0;({struct Cyc_Tcenv_Tenv*_tmp80B=te;unsigned _tmp80A=loc;void**_tmp809=topt;Cyc_Tcexp_expr_err(_tmp80B,_tmp80A,_tmp809,_tag_fat(_tmp3DE,sizeof(void*),2U));});});
return res;}}
# 1912
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1914
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1917
if(topt == 0)
return 0;{
void*_tmp3E2=Cyc_Tcutil_compress(*topt);void*_stmttmp3E=_tmp3E2;void*_tmp3E3=_stmttmp3E;void*_tmp3E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E3)->tag == 3U){_LL1: _tmp3E4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E3)->f1).elt_type;_LL2: {void*elt_typ=_tmp3E4;
# 1921
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1927
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1929
enum Cyc_Absyn_AliasQual _tmp3E5=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp3F=_tmp3E5;enum Cyc_Absyn_AliasQual _tmp3E6=_stmttmp3F;if(_tmp3E6 == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 1935
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 1939
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 1943
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3E7=Cyc_Tcutil_compress(handle_type);void*_stmttmp40=_tmp3E7;void*_tmp3E8=_stmttmp40;void*_tmp3E9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f2 != 0){_LL1: _tmp3E9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f2)->hd;_LL2: {void*r=_tmp3E9;
# 1948
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1952
({struct Cyc_String_pa_PrintArg_struct _tmp3EC=({struct Cyc_String_pa_PrintArg_struct _tmp5DF;_tmp5DF.tag=0U,({
struct _fat_ptr _tmp80C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp5DF.f1=_tmp80C;});_tmp5DF;});void*_tmp3EA[1U];_tmp3EA[0]=& _tmp3EC;({unsigned _tmp80E=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _fat_ptr _tmp80D=({const char*_tmp3EB="expecting region_t type but found %s";_tag_fat(_tmp3EB,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp80E,_tmp80D,_tag_fat(_tmp3EA,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 1959
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp80F=Cyc_Absyn_uniquergn_exp();*ropt=_tmp80F;});}}}
# 1966
({struct Cyc_Tcenv_Tenv*_tmp810=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp810,& Cyc_Absyn_uint_type,*e);});{
# 1975 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3ED=0U;({struct _fat_ptr _tmp811=({const char*_tmp3EE="calloc with empty type";_tag_fat(_tmp3EE,sizeof(char),23U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp811,_tag_fat(_tmp3ED,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp814=loc;struct Cyc_Tcenv_Tenv*_tmp813=te;struct Cyc_List_List*_tmp812=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp814,_tmp813,_tmp812,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp3F1=({struct Cyc_String_pa_PrintArg_struct _tmp5E0;_tmp5E0.tag=0U,({struct _fat_ptr _tmp815=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(elt_type));_tmp5E0.f1=_tmp815;});_tmp5E0;});void*_tmp3EF[1U];_tmp3EF[0]=& _tmp3F1;({unsigned _tmp817=loc;struct _fat_ptr _tmp816=({const char*_tmp3F0="calloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3F0,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp817,_tmp816,_tag_fat(_tmp3EF,sizeof(void*),1U));});});
num_elts=*e;
one_elt=0;}else{
# 1988
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3F2=er;struct Cyc_Absyn_Exp*_tmp3F4;struct Cyc_Absyn_Exp*_tmp3F3;void*_tmp3F5;switch(*((int*)_tmp3F2)){case 17U: _LL6: _tmp3F5=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F2)->f1;_LL7: {void*t2=_tmp3F5;
# 1992
elt_type=t2;
({void**_tmp818=({void**_tmp3F6=_cycalloc(sizeof(*_tmp3F6));*_tmp3F6=elt_type;_tmp3F6;});*t=_tmp818;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F2)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F2)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F2)->f2)->tl)->tl == 0){_LL8: _tmp3F3=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F2)->f2)->hd;_tmp3F4=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F2)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp3F3;struct Cyc_Absyn_Exp*e2=_tmp3F4;
# 1999
{struct _tuple0 _tmp3F7=({struct _tuple0 _tmp5E3;_tmp5E3.f1=e1->r,_tmp5E3.f2=e2->r;_tmp5E3;});struct _tuple0 _stmttmp41=_tmp3F7;struct _tuple0 _tmp3F8=_stmttmp41;void*_tmp3F9;void*_tmp3FA;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F8.f1)->tag == 17U){_LLD: _tmp3FA=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F8.f1)->f1;_LLE: {void*t1=_tmp3FA;
# 2002
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t1)){
# 2005
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t1))
({struct Cyc_String_pa_PrintArg_struct _tmp3FD=({struct Cyc_String_pa_PrintArg_struct _tmp5E1;_tmp5E1.tag=0U,({struct _fat_ptr _tmp819=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp5E1.f1=_tmp819;});_tmp5E1;});void*_tmp3FB[1U];_tmp3FB[0]=& _tmp3FD;({unsigned _tmp81B=loc;struct _fat_ptr _tmp81A=({const char*_tmp3FC="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3FC,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp81B,_tmp81A,_tag_fat(_tmp3FB,sizeof(void*),1U));});});else{
# 2008
*is_calloc=1;}}
# 2011
elt_type=t1;
({void**_tmp81C=({void**_tmp3FE=_cycalloc(sizeof(*_tmp3FE));*_tmp3FE=elt_type;_tmp3FE;});*t=_tmp81C;});
num_elts=e2;
one_elt=0;
goto _LLC;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F8.f2)->tag == 17U){_LLF: _tmp3F9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F8.f2)->f1;_LL10: {void*t2=_tmp3F9;
# 2018
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 2021
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp401=({struct Cyc_String_pa_PrintArg_struct _tmp5E2;_tmp5E2.tag=0U,({struct _fat_ptr _tmp81D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp5E2.f1=_tmp81D;});_tmp5E2;});void*_tmp3FF[1U];_tmp3FF[0]=& _tmp401;({unsigned _tmp81F=loc;struct _fat_ptr _tmp81E=({const char*_tmp400="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp400,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp81F,_tmp81E,_tag_fat(_tmp3FF,sizeof(void*),1U));});});else{
# 2024
*is_calloc=1;}}
# 2027
elt_type=t2;
({void**_tmp820=({void**_tmp402=_cycalloc(sizeof(*_tmp402));*_tmp402=elt_type;_tmp402;});*t=_tmp820;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2034
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2036
 No_sizeof: {
# 2039
struct Cyc_Absyn_Exp*_tmp403=*e;struct Cyc_Absyn_Exp*real_e=_tmp403;
{void*_tmp404=real_e->r;void*_stmttmp42=_tmp404;void*_tmp405=_stmttmp42;struct Cyc_Absyn_Exp*_tmp406;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp405)->tag == 14U){_LL14: _tmp406=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp405)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp406;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2044
{void*_tmp407=Cyc_Tcutil_compress((void*)_check_null(real_e->topt));void*_stmttmp43=_tmp407;void*_tmp408=_stmttmp43;void*_tmp409;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp408)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp408)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp408)->f2 != 0){_LL19: _tmp409=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp408)->f2)->hd;_LL1A: {void*tagt=_tmp409;
# 2046
{void*_tmp40A=Cyc_Tcutil_compress(tagt);void*_stmttmp44=_tmp40A;void*_tmp40B=_stmttmp44;struct Cyc_Absyn_Exp*_tmp40C;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp40B)->tag == 9U){_LL1E: _tmp40C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp40B)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp40C;
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2050
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2053
elt_type=Cyc_Absyn_char_type;
({void**_tmp821=({void**_tmp40D=_cycalloc(sizeof(*_tmp40D));*_tmp40D=elt_type;_tmp40D;});*t=_tmp821;});
num_elts=*e;
one_elt=0;}
# 2058
goto _LL5;}_LL5:;}}
# 2062
*is_fat=!one_elt;
# 2065
{void*_tmp40E=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp40F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40E)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40E)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp40F=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40E)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp40F;
# 2067
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({void*_tmp410=0U;({unsigned _tmp823=loc;struct _fat_ptr _tmp822=({const char*_tmp411="malloc with existential types not yet implemented";_tag_fat(_tmp411,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp823,_tmp822,_tag_fat(_tmp410,sizeof(void*),0U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2074
void*(*_tmp412)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp412;
void*_tmp413=Cyc_Absyn_false_type;void*zero_term=_tmp413;
if(topt != 0){
void*_tmp414=Cyc_Tcutil_compress(*topt);void*_stmttmp45=_tmp414;void*_tmp415=_stmttmp45;void*_tmp418;void*_tmp417;void*_tmp416;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->tag == 3U){_LL28: _tmp416=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).ptr_atts).nullable;_tmp417=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).ptr_atts).bounds;_tmp418=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).ptr_atts).zero_term;_LL29: {void*n=_tmp416;void*b=_tmp417;void*zt=_tmp418;
# 2079
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2084
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({void*_tmp419=0U;({unsigned _tmp825=loc;struct _fat_ptr _tmp824=({const char*_tmp41A="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp41A,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp825,_tmp824,_tag_fat(_tmp419,sizeof(void*),0U));});});
*is_calloc=1;}{
# 2090
struct Cyc_Absyn_Exp*_tmp41B=({void*_tmp826=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp826,b);});struct Cyc_Absyn_Exp*eopt=_tmp41B;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp41C=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp41C;
int _tmp41D=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp41D;
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp82A=elt_type;void*_tmp829=rgn;struct Cyc_Absyn_Tqual _tmp828=Cyc_Absyn_empty_tqual(0U);void*_tmp827=b;Cyc_Absyn_atb_type(_tmp82A,_tmp829,_tmp828,_tmp827,zero_term);});}{
# 2098
void*_tmp41E=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_stmttmp46=_tmp41E;void*_tmp41F=_stmttmp46;void*_tmp420;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f2 != 0){_LL2D: _tmp420=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f2)->hd;_LL2E: {void*tagtyp=_tmp420;
# 2100
struct Cyc_Absyn_Exp*_tmp421=({void*_tmp82B=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp82B,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp421;
# 2102
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp82F=elt_type;void*_tmp82E=rgn;struct Cyc_Absyn_Tqual _tmp82D=Cyc_Absyn_empty_tqual(0U);void*_tmp82C=b;Cyc_Absyn_atb_type(_tmp82F,_tmp82E,_tmp82D,_tmp82C,zero_term);});}
# 2106
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2110
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2113
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp833)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=ptr_maker;void*_tmp832=elt_type;void*_tmp831=rgn;struct Cyc_Absyn_Tqual _tmp830=Cyc_Absyn_empty_tqual(0U);_tmp833(_tmp832,_tmp831,_tmp830,zero_term);});}}}
# 2118
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2120
struct Cyc_Tcenv_Tenv*_tmp422=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp422;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp423=(void*)_check_null(e1->topt);void*t1=_tmp423;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2127
{void*_tmp424=Cyc_Tcutil_compress(t1);void*_stmttmp47=_tmp424;void*_tmp425=_stmttmp47;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp425)->tag == 4U){_LL1: _LL2:
({void*_tmp426=0U;({unsigned _tmp835=loc;struct _fat_ptr _tmp834=({const char*_tmp427="cannot assign to an array";_tag_fat(_tmp427,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp835,_tmp834,_tag_fat(_tmp426,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2132
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp428=0U;({unsigned _tmp837=loc;struct _fat_ptr _tmp836=({const char*_tmp429="Swap not allowed for non-pointer or non-word-sized types.";_tag_fat(_tmp429,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp837,_tmp836,_tag_fat(_tmp428,sizeof(void*),0U));});});
# 2136
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp42B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=0U,({struct _fat_ptr _tmp838=({const char*_tmp42C="swap non-lvalue";_tag_fat(_tmp42C,sizeof(char),16U);});_tmp5E4.f1=_tmp838;});_tmp5E4;});void*_tmp42A[1U];_tmp42A[0]=& _tmp42B;({struct Cyc_Tcenv_Tenv*_tmp83B=te;unsigned _tmp83A=e1->loc;void**_tmp839=topt;Cyc_Tcexp_expr_err(_tmp83B,_tmp83A,_tmp839,_tag_fat(_tmp42A,sizeof(void*),1U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp42E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=0U,({struct _fat_ptr _tmp83C=({const char*_tmp42F="swap non-lvalue";_tag_fat(_tmp42F,sizeof(char),16U);});_tmp5E5.f1=_tmp83C;});_tmp5E5;});void*_tmp42D[1U];_tmp42D[0]=& _tmp42E;({struct Cyc_Tcenv_Tenv*_tmp83F=te;unsigned _tmp83E=e2->loc;void**_tmp83D=topt;Cyc_Tcexp_expr_err(_tmp83F,_tmp83E,_tmp83D,_tag_fat(_tmp42D,sizeof(void*),1U));});});
# 2141
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp431=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=0U,({struct _fat_ptr _tmp840=({const char*_tmp436="type mismatch: ";_tag_fat(_tmp436,sizeof(char),16U);});_tmp5E9.f1=_tmp840;});_tmp5E9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp432=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=2U,_tmp5E8.f1=(void*)t1;_tmp5E8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp433=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=0U,({struct _fat_ptr _tmp841=({const char*_tmp435=" != ";_tag_fat(_tmp435,sizeof(char),5U);});_tmp5E7.f1=_tmp841;});_tmp5E7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp434=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=2U,_tmp5E6.f1=(void*)t2;_tmp5E6;});void*_tmp430[4U];_tmp430[0]=& _tmp431,_tmp430[1]=& _tmp432,_tmp430[2]=& _tmp433,_tmp430[3]=& _tmp434;({struct Cyc_Tcenv_Tenv*_tmp844=te;unsigned _tmp843=loc;void**_tmp842=topt;Cyc_Tcexp_expr_err(_tmp844,_tmp843,_tmp842,_tag_fat(_tmp430,sizeof(void*),4U));});});
return Cyc_Absyn_void_type;}}}
# 2149
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp845=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp845,s,1);});
# 2152
while(1){
void*_tmp437=s->r;void*_stmttmp48=_tmp437;void*_tmp438=_stmttmp48;struct Cyc_Absyn_Stmt*_tmp439;struct Cyc_Absyn_Stmt*_tmp43A;struct Cyc_Absyn_Exp*_tmp43B;switch(*((int*)_tmp438)){case 1U: _LL1: _tmp43B=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp43B;
# 2156
void*_tmp43C=(void*)_check_null(e->topt);void*t=_tmp43C;
if(!({void*_tmp846=t;Cyc_Unify_unify(_tmp846,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=0U,({struct _fat_ptr _tmp847=({const char*_tmp440="statement expression returns type ";_tag_fat(_tmp440,sizeof(char),35U);});_tmp5EB.f1=_tmp847;});_tmp5EB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp43F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=2U,_tmp5EA.f1=(void*)t;_tmp5EA;});void*_tmp43D[2U];_tmp43D[0]=& _tmp43E,_tmp43D[1]=& _tmp43F;({unsigned _tmp848=loc;Cyc_Tcexp_err_and_explain(_tmp848,_tag_fat(_tmp43D,sizeof(void*),2U));});});
return t;}case 2U: _LL3: _tmp43A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp43A;
s=s2;continue;}case 12U: _LL5: _tmp439=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp439;
s=s1;continue;}default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp442=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=0U,({
struct _fat_ptr _tmp849=({const char*_tmp443="statement expression must end with expression";_tag_fat(_tmp443,sizeof(char),46U);});_tmp5EC.f1=_tmp849;});_tmp5EC;});void*_tmp441[1U];_tmp441[0]=& _tmp442;({struct Cyc_Tcenv_Tenv*_tmp84C=te;unsigned _tmp84B=loc;void**_tmp84A=topt;Cyc_Tcexp_expr_err(_tmp84C,_tmp84B,_tmp84A,_tag_fat(_tmp441,sizeof(void*),1U));});});}_LL0:;}}
# 2167
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp84D=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp84D,0,e);}));
{void*_tmp444=t;struct Cyc_Absyn_Aggrdecl*_tmp445;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp444)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp444)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp444)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp445=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp444)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp445;
# 2171
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2175
({struct Cyc_Warn_String_Warn_Warg_struct _tmp447=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=0U,({struct _fat_ptr _tmp84E=({const char*_tmp449="expecting @tagged union but found ";_tag_fat(_tmp449,sizeof(char),35U);});_tmp5EE.f1=_tmp84E;});_tmp5EE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp448=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5ED;_tmp5ED.tag=2U,_tmp5ED.f1=(void*)t;_tmp5ED;});void*_tmp446[2U];_tmp446[0]=& _tmp447,_tmp446[1]=& _tmp448;({unsigned _tmp84F=loc;Cyc_Warn_err2(_tmp84F,_tag_fat(_tmp446,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}
# 2178
return Cyc_Absyn_uint_type;}
# 2181
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
({struct Cyc_Tcenv_Tenv*_tmp851=te;struct Cyc_Absyn_Exp*_tmp850=e;Cyc_Tcexp_tcTest(_tmp851,_tmp850,({const char*_tmp44A="@assert";_tag_fat(_tmp44A,sizeof(char),8U);}));});
if(!Cyc_Absyn_no_side_effects_exp(e))
({void*_tmp44B=0U;({unsigned _tmp853=loc;struct _fat_ptr _tmp852=({const char*_tmp44C="@assert expression may have side effects";_tag_fat(_tmp44C,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp853,_tmp852,_tag_fat(_tmp44B,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 2189
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2193
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp44D=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));struct Cyc_Tcenv_Tenv*te=_tmp44D;
if(*rgn_handle != 0){
# 2198
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp44E=Cyc_Tcutil_compress(handle_type);void*_stmttmp49=_tmp44E;void*_tmp44F=_stmttmp49;void*_tmp450;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f2 != 0){_LL1: _tmp450=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f2)->hd;_LL2: {void*r=_tmp450;
# 2203
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2207
({struct Cyc_Warn_String_Warn_Warg_struct _tmp452=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0U,({struct _fat_ptr _tmp854=({const char*_tmp454="expecting region_t type but found ";_tag_fat(_tmp454,sizeof(char),35U);});_tmp5F0.f1=_tmp854;});_tmp5F0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp453=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=2U,_tmp5EF.f1=(void*)handle_type;_tmp5EF;});void*_tmp451[2U];_tmp451[0]=& _tmp452,_tmp451[1]=& _tmp453;({unsigned _tmp855=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;Cyc_Warn_err2(_tmp855,_tag_fat(_tmp451,sizeof(void*),2U));});});
# 2209
goto _LL0;}_LL0:;}else{
# 2211
if(topt != 0){
# 2214
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp856=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp856;});}}}{
# 2221
void*_tmp455=e1->r;void*_stmttmp4A=_tmp455;void*_tmp456=_stmttmp4A;struct Cyc_List_List*_tmp457;struct Cyc_List_List*_tmp459;struct Cyc_Core_Opt*_tmp458;switch(*((int*)_tmp456)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2226
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp45A=0U;({struct _fat_ptr _tmp857=({const char*_tmp45B="tcNew: comprehension returned non-array type";_tag_fat(_tmp45B,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp857,_tag_fat(_tmp45A,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp85A=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp859=loc;void*_tmp858=res_typ;Cyc_Tcutil_silent_castable(_tmp85A,_tmp859,_tmp858,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2238
return res_typ;}case 36U: _LLA: _tmp458=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp456)->f1;_tmp459=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp456)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp458;struct Cyc_List_List*des=_tmp459;
# 2240
({void*_tmp85B=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=26U,_tmp45C->f1=des;_tmp45C;});e1->r=_tmp85B;});
_tmp457=des;goto _LLD;}case 26U: _LLC: _tmp457=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp456)->f1;_LLD: {struct Cyc_List_List*des=_tmp457;
# 2243
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp45D=Cyc_Tcutil_compress(*topt);void*_stmttmp4B=_tmp45D;void*_tmp45E=_stmttmp4B;void*_tmp461;struct Cyc_Absyn_Tqual _tmp460;void**_tmp45F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45E)->tag == 3U){_LL15: _tmp45F=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45E)->f1).elt_type;_tmp460=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45E)->f1).elt_tq;_tmp461=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45E)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp45F;struct Cyc_Absyn_Tqual tq=_tmp460;void*zt=_tmp461;
# 2249
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2255
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp462=0U;({struct _fat_ptr _tmp85C=({const char*_tmp463="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp463,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp85C,_tag_fat(_tmp462,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2264
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp85F=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp85E=loc;void*_tmp85D=res_typ;Cyc_Tcutil_silent_castable(_tmp85F,_tmp85E,_tmp85D,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2271
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp456)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2276
void*_tmp464=({void*_tmp863=Cyc_Absyn_char_type;void*_tmp862=rgn;struct Cyc_Absyn_Tqual _tmp861=Cyc_Absyn_const_tqual(0U);void*_tmp860=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp863,_tmp862,_tmp861,_tmp860,Cyc_Absyn_true_type);});void*topt2=_tmp464;
# 2278
void*_tmp465=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp465;
return({void*_tmp867=t;void*_tmp866=rgn;struct Cyc_Absyn_Tqual _tmp865=Cyc_Absyn_empty_tqual(0U);void*_tmp864=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp867,_tmp866,_tmp865,_tmp864,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2283
void*_tmp466=({void*_tmp86B=Cyc_Absyn_wchar_type();void*_tmp86A=rgn;struct Cyc_Absyn_Tqual _tmp869=Cyc_Absyn_const_tqual(0U);void*_tmp868=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp86B,_tmp86A,_tmp869,_tmp868,Cyc_Absyn_true_type);});void*topt2=_tmp466;
# 2285
void*_tmp467=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp467;
return({void*_tmp86F=t;void*_tmp86E=rgn;struct Cyc_Absyn_Tqual _tmp86D=Cyc_Absyn_empty_tqual(0U);void*_tmp86C=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp86F,_tmp86E,_tmp86D,_tmp86C,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2292
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp468=Cyc_Tcutil_compress(*topt);void*_stmttmp4C=_tmp468;void*_tmp469=_stmttmp4C;struct Cyc_Absyn_Tqual _tmp46B;void**_tmp46A;switch(*((int*)_tmp469)){case 3U: _LL1A: _tmp46A=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469)->f1).elt_type;_tmp46B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469)->f1).elt_tq;_LL1B: {void**elttype=_tmp46A;struct Cyc_Absyn_Tqual tq=_tmp46B;
# 2298
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp469)->f1)->tag == 18U){_LL1C: _LL1D:
# 2302
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2308
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2310
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt)&& !Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);{
struct Cyc_Absyn_PtrInfo _tmp46C=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp46C;
pi.elt_type=telt;
(pi.ptr_atts).rgn=rgn;
({void*_tmp870=Cyc_Absyn_bounds_one();(pi.ptr_atts).bounds=_tmp870;});
(pi.ptr_atts).zero_term=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp873=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp872=loc;void*_tmp871=res_typ;Cyc_Tcutil_silent_castable(_tmp873,_tmp872,_tmp871,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}
# 2324
return res_typ;}}}}}_LL5:;}}
# 2330
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp875=t=({void*_tmp874=t;Cyc_Tcutil_promote_array(_tmp874,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp875;});
return t;}
# 2340
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2344
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(e->loc);{
void*_tmp46D=e->r;void*_stmttmp4D=_tmp46D;void*_tmp46E=_stmttmp4D;switch(*((int*)_tmp46E)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2353
 if(Cyc_Tcutil_is_array_type(t))
({void*_tmp877=t=({void*_tmp876=t;Cyc_Tcutil_promote_array(_tmp876,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp877;});
return t;}_LL0:;}}
# 2366 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*t=({void*_tmp878=Cyc_Tcutil_compress(Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0));e->topt=_tmp878;});
# 2370
{void*_tmp46F=e->r;void*_stmttmp4E=_tmp46F;void*_tmp470=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp471;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp470)->tag == 12U){_LL1: _tmp471=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp470)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp471;
return t;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2375
void*_tmp472=t;void*_tmp473;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp472)->tag == 3U){_LL6: _tmp473=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp472)->f1).elt_type;_LL7: {void*t2=_tmp473;
# 2377
void*_tmp474=Cyc_Tcutil_compress(t2);void*_stmttmp4F=_tmp474;void*_tmp475=_stmttmp4F;struct Cyc_Absyn_FnInfo _tmp476;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp475)->tag == 5U){_LLB: _tmp476=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp475)->f1;_LLC: {struct Cyc_Absyn_FnInfo info=_tmp476;
# 2379
struct _tuple13 _tmp477=({struct _tuple13 _tmp5F1;({struct Cyc_List_List*_tmp879=Cyc_Tcenv_lookup_type_vars(te);_tmp5F1.f1=_tmp879;}),_tmp5F1.f2=Cyc_Core_heap_region;_tmp5F1;});struct _tuple13 env=_tmp477;
struct Cyc_List_List*_tmp478=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);struct Cyc_List_List*inst=_tmp478;
struct Cyc_List_List*_tmp479=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*ts=_tmp479;
struct Cyc_Absyn_Exp*_tmp47A=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp47A;
({void*_tmp87A=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->tag=13U,_tmp47B->f1=inner,_tmp47B->f2=ts;_tmp47B;});e->r=_tmp87A;});
({void*_tmp87B=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_tmp87B;});
return(void*)_check_null(e->topt);}}else{_LLD: _LLE:
 return t;}_LLA:;}}else{_LL8: _LL9:
# 2388
 return t;}_LL5:;}}}
# 2397
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2399
struct Cyc_List_List*_tmp47C=0;struct Cyc_List_List*decls=_tmp47C;
# 2401
{void*_tmp47D=fn_exp->r;void*_stmttmp50=_tmp47D;void*_tmp47E=_stmttmp50;struct Cyc_List_List*_tmp47F;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47E)->tag == 10U){_LL1: _tmp47F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47E)->f2;_LL2: {struct Cyc_List_List*es=_tmp47F;
# 2403
{void*_tmp480=e->r;void*_stmttmp51=_tmp480;void*_tmp481=_stmttmp51;struct Cyc_List_List*_tmp482;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp481)->tag == 10U){_LL6: _tmp482=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp481)->f2;_LL7: {struct Cyc_List_List*es2=_tmp482;
# 2405
struct Cyc_List_List*_tmp483=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp483;
int _tmp484=0;int arg_cnt=_tmp484;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp486=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F6;_tmp5F6.tag=0U,({struct _fat_ptr _tmp87C=({const char*_tmp48D="bad count ";_tag_fat(_tmp48D,sizeof(char),11U);});_tmp5F6.f1=_tmp87C;});_tmp5F6;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp487=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=12U,_tmp5F5.f1=arg_cnt;_tmp5F5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp488=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=0U,({struct _fat_ptr _tmp87D=({const char*_tmp48C="/";_tag_fat(_tmp48C,sizeof(char),2U);});_tmp5F4.f1=_tmp87D;});_tmp5F4;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp489=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=12U,_tmp5F3.f1=(int)arg_exps->hd;_tmp5F3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp48A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0U,({
struct _fat_ptr _tmp87E=({const char*_tmp48B=" for alias coercion!";_tag_fat(_tmp48B,sizeof(char),21U);});_tmp5F2.f1=_tmp87E;});_tmp5F2;});void*_tmp485[5U];_tmp485[0]=& _tmp486,_tmp485[1]=& _tmp487,_tmp485[2]=& _tmp488,_tmp485[3]=& _tmp489,_tmp485[4]=& _tmp48A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp485,sizeof(void*),5U));});
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2417
struct _tuple12 _tmp48E=({struct Cyc_Absyn_Exp*_tmp87F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp87F,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp52=_tmp48E;struct _tuple12 _tmp48F=_stmttmp52;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Decl*_tmp490;_LLB: _tmp490=_tmp48F.f1;_tmp491=_tmp48F.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp490;struct Cyc_Absyn_Exp*ve=_tmp491;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->hd=d,_tmp492->tl=decls;_tmp492;});
arg_exps=arg_exps->tl;}}}
# 2422
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp493=0U;({struct _fat_ptr _tmp880=({const char*_tmp494="not a function call!";_tag_fat(_tmp494,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp880,_tag_fat(_tmp493,sizeof(void*),0U));});});}_LL5:;}
# 2425
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp495=0U;({struct _fat_ptr _tmp881=({const char*_tmp496="not a function call!";_tag_fat(_tmp496,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp881,_tag_fat(_tmp495,sizeof(void*),0U));});});}_LL0:;}
# 2429
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp497=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp497;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp884=({struct Cyc_Absyn_Decl*_tmp883=d;struct Cyc_Absyn_Stmt*_tmp882=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp883,_tmp882,e->loc);});Cyc_Absyn_stmt_exp(_tmp884,e->loc);});
decls=decls->tl;}
# 2435
e->topt=0;
e->r=fn_exp->r;}
# 2440
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2444
{void*_tmp498=e->r;void*_stmttmp53=_tmp498;void*_tmp499=_stmttmp53;void*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct _fat_ptr*_tmp49E;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp49F;int*_tmp4A5;struct Cyc_Absyn_Exp**_tmp4A4;void***_tmp4A3;struct Cyc_Absyn_Exp**_tmp4A2;int*_tmp4A1;struct Cyc_Absyn_Enumfield*_tmp4A7;void*_tmp4A6;struct Cyc_Absyn_Enumfield*_tmp4A9;struct Cyc_Absyn_Enumdecl*_tmp4A8;struct Cyc_Absyn_Datatypefield*_tmp4AC;struct Cyc_Absyn_Datatypedecl*_tmp4AB;struct Cyc_List_List*_tmp4AA;struct Cyc_List_List*_tmp4AE;void*_tmp4AD;struct Cyc_Absyn_Aggrdecl**_tmp4B2;struct Cyc_List_List*_tmp4B1;struct Cyc_List_List**_tmp4B0;struct _tuple1**_tmp4AF;int*_tmp4B5;void*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B3;int*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Vardecl*_tmp4B6;struct Cyc_Absyn_Stmt*_tmp4BA;struct Cyc_List_List*_tmp4BB;struct Cyc_List_List*_tmp4BD;struct _tuple9*_tmp4BC;struct Cyc_List_List*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4BF;int*_tmp4C4;int*_tmp4C3;struct _fat_ptr*_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C1;int*_tmp4C8;int*_tmp4C7;struct _fat_ptr*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_List_List*_tmp4CB;void*_tmp4CA;void*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Exp**_tmp4CE;struct Cyc_Absyn_Exp*_tmp4D0;enum Cyc_Absyn_Coercion*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D2;void*_tmp4D1;struct Cyc_List_List*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Core_Opt*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;enum Cyc_Absyn_Incrementor _tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_List_List*_tmp4E6;enum Cyc_Absyn_Primop _tmp4E5;void**_tmp4E7;union Cyc_Absyn_Cnst*_tmp4E8;struct Cyc_List_List*_tmp4EA;struct Cyc_Core_Opt*_tmp4E9;struct Cyc_Absyn_VarargCallInfo**_tmp4ED;struct Cyc_List_List*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EE;switch(*((int*)_tmp499)){case 12U: _LL1: _tmp4EE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp4EE;
# 2449
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=(void*)_check_null(e2->topt);
return;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp499)->f4 == 0){_LL3: _LL4:
# 2453
({void*_tmp4EF=0U;({struct _fat_ptr _tmp885=({const char*_tmp4F0="unresolved function in tcExpNoInst";_tag_fat(_tmp4F0,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp885,_tag_fat(_tmp4EF,sizeof(void*),0U));});});}else{_LL1B: _tmp4EB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4EC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4ED=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp4EB;struct Cyc_List_List*es=_tmp4EC;struct Cyc_Absyn_VarargCallInfo**vci=_tmp4ED;
# 2476
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp4F1;_push_handler(& _tmp4F1);{int _tmp4F3=0;if(setjmp(_tmp4F1.handler))_tmp4F3=1;if(!_tmp4F3){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp4F2=(void*)Cyc_Core_get_exn_thrown();void*_tmp4F4=_tmp4F2;void*_tmp4F5;if(((struct Cyc_Core_Failure_exn_struct*)_tmp4F4)->tag == Cyc_Core_Failure){_LL5A: _LL5B:
# 2481
 ok=0;
fn_exp=e;
goto _LL59;}else{_LL5C: _tmp4F5=_tmp4F4;_LL5D: {void*exn=_tmp4F5;(int)_rethrow(exn);}}_LL59:;}}}
# 2485
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2492
goto _LL0;}}case 36U: _LL5: _tmp4E9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4EA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp4E9;struct Cyc_List_List*des=_tmp4EA;
# 2457
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp4E8=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp4E8;
# 2461
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp4E7=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LLA: {void**b=_tmp4E7;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp4E5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4E6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp4E5;struct Cyc_List_List*es=_tmp4E6;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp4E3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4E4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp4E3;enum Cyc_Absyn_Incrementor i=_tmp4E4;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp4E0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4E1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4E2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp4E0;struct Cyc_Core_Opt*popt=_tmp4E1;struct Cyc_Absyn_Exp*e2=_tmp4E2;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp4DD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4DE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4DF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp4DD;struct Cyc_Absyn_Exp*e2=_tmp4DE;struct Cyc_Absyn_Exp*e3=_tmp4DF;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp4DB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4DC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp4DB;struct Cyc_Absyn_Exp*e2=_tmp4DC;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp4D9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4DA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp4D9;struct Cyc_Absyn_Exp*e2=_tmp4DA;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp4D7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4D8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp4D7;struct Cyc_Absyn_Exp*e2=_tmp4D8;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp4D6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp4D6;
# 2493
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp4D4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4D5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp4D4;struct Cyc_List_List*ts=_tmp4D5;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp4D1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4D2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4D3=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp499)->f4;_LL22: {void*t1=_tmp4D1;struct Cyc_Absyn_Exp*e1=_tmp4D2;enum Cyc_Absyn_Coercion*c=_tmp4D3;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp4D0=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp4D0;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp4CE=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4CF=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp4CE;struct Cyc_Absyn_Exp*e1=_tmp4CF;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp4CD=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp4CD;
_tmp4CC=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17U: _LL29: _tmp4CC=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL2A: {void*t1=_tmp4CC;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp4CA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4CB=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL2C: {void*t1=_tmp4CA;struct Cyc_List_List*l=_tmp4CB;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp4C9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp4C9;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp4C5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4C6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4C7=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_tmp4C8=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp499)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp4C5;struct _fat_ptr*f=_tmp4C6;int*is_tagged=_tmp4C7;int*is_read=_tmp4C8;
# 2503
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp4C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4C3=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_tmp4C4=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp499)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp4C1;struct _fat_ptr*f=_tmp4C2;int*is_tagged=_tmp4C3;int*is_read=_tmp4C4;
# 2505
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp4BF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4C0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp4BF;struct Cyc_Absyn_Exp*e2=_tmp4C0;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp4BE=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL36: {struct Cyc_List_List*es=_tmp4BE;
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp4BC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4BD=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL38: {struct _tuple9*t1=_tmp4BC;struct Cyc_List_List*des=_tmp4BD;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp4BB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL3A: {struct Cyc_List_List*des=_tmp4BB;
# 2512
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4F6=Cyc_Tcutil_compress(*topt);void*_stmttmp54=_tmp4F6;void*_tmp4F7=_stmttmp54;void*_tmp4FA;struct Cyc_Absyn_Tqual*_tmp4F9;void**_tmp4F8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F7)->tag == 4U){_LL5F: _tmp4F8=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F7)->f1).elt_type;_tmp4F9=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F7)->f1).tq;_tmp4FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F7)->f1).zero_term;_LL60: {void**et=_tmp4F8;struct Cyc_Absyn_Tqual*etq=_tmp4F9;void*zt=_tmp4FA;
# 2518
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}
# 2524
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp4BA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp4BA;
# 2526
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp4B6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4B7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4B8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_tmp4B9=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp499)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp4B6;struct Cyc_Absyn_Exp*e1=_tmp4B7;struct Cyc_Absyn_Exp*e2=_tmp4B8;int*iszeroterm=_tmp4B9;
# 2528
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp4B3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4B4=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4B5=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp4B3;void*t1=_tmp4B4;int*iszeroterm=_tmp4B5;
# 2530
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp4AF=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4B0=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4B1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_tmp4B2=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp499)->f4;_LL42: {struct _tuple1**tn=_tmp4AF;struct Cyc_List_List**ts=_tmp4B0;struct Cyc_List_List*args=_tmp4B1;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4B2;
# 2532
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp4AD=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4AE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL44: {void*ts=_tmp4AD;struct Cyc_List_List*args=_tmp4AE;
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp4AA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4AB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_tmp4AC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp499)->f3;_LL46: {struct Cyc_List_List*es=_tmp4AA;struct Cyc_Absyn_Datatypedecl*tud=_tmp4AB;struct Cyc_Absyn_Datatypefield*tuf=_tmp4AC;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp4A8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4A9=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp4A8;struct Cyc_Absyn_Enumfield*ef=_tmp4A9;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp4A6=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4A7=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL4A: {void*t2=_tmp4A6;struct Cyc_Absyn_Enumfield*ef=_tmp4A7;
t=t2;goto _LL0;}case 34U: _LL4B: _tmp4A1=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp499)->f1).is_calloc;_tmp4A2=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp499)->f1).rgn;_tmp4A3=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp499)->f1).elt_type;_tmp4A4=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp499)->f1).num_elts;_tmp4A5=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp499)->f1).fat_result;_LL4C: {int*is_calloc=_tmp4A1;struct Cyc_Absyn_Exp**ropt=_tmp4A2;void***t2=_tmp4A3;struct Cyc_Absyn_Exp**e2=_tmp4A4;int*isfat=_tmp4A5;
# 2538
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp49F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp4A0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp49F;struct Cyc_Absyn_Exp*e2=_tmp4A0;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp49D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_tmp49E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp49D;struct _fat_ptr*f=_tmp49E;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp49C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp49C;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 42U: _LL53: _tmp49B=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL54: {struct Cyc_Absyn_Exp*e=_tmp49B;
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 39U: _LL55: _tmp49A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL56: {void*t2=_tmp49A;
# 2544
if(!te->allow_valueof)
({void*_tmp4FB=0U;({unsigned _tmp887=e->loc;struct _fat_ptr _tmp886=({const char*_tmp4FC="valueof(-) can only occur within types";_tag_fat(_tmp4FC,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp887,_tmp886,_tag_fat(_tmp4FB,sizeof(void*),0U));});});
# 2552
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL57: _LL58:
# 2556
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 2558
e->topt=t;}

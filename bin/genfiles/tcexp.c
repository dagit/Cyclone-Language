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
# 110 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
extern struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
extern void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 306
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 313
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 415 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};
# 471
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Absyn_No_throw_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 497
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 499
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 507
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 514
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 532
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 895 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_compress(void*);
# 916
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 918
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 921
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 923
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 925
extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_wchar_type (void);
void*Cyc_Absyn_gen_float_type(unsigned i);
# 928
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 932
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 934
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 951
void*Cyc_Absyn_exn_type (void);
# 959
extern void*Cyc_Absyn_fat_bound_type;
# 961
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 963
void*Cyc_Absyn_bounds_one (void);
# 965
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 969
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term,void*rel);
# 971
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term,void*rel);
# 973
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term,void*rel);
# 977
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt,void*rel);
# 985
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 989
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 993
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void);
# 1067
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1132
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1134
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1136
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1139
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1141
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66 "warn.h"
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 43 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 31 "tcutil.h"
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 41
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 53
int Cyc_Tcutil_is_bound_one(void*);
# 55
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 65
void*Cyc_Tcutil_pointer_region(void*);
# 68
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 78
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*);
# 106
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*,void*,struct Cyc_List_List*);
# 110
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 112
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 117
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple12{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 120
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 122
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 132
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 151
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 162
void Cyc_Tcutil_check_nonzero_bound(unsigned,void*);
# 164
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 166
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 177
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 185
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple15{int f1;void*f2;};
# 194
struct _tuple15 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 217
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 228
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 231
int Cyc_Tcutil_zeroable_type(void*);
# 235
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 240
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 38
extern struct Cyc_Absyn_Kind Cyc_Kinds_tak;
# 40
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 48
extern struct Cyc_Core_Opt Cyc_Kinds_ako;
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_trko;
extern struct Cyc_Core_Opt Cyc_Kinds_tako;
# 59
extern struct Cyc_Core_Opt Cyc_Kinds_tmko;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 144
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 146
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*eff);
# 151
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_List_List*po);
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 33
void*Cyc_CurRgn_instantiate(void*fntype,void*currgn);struct _tuple16{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 53
extern int Cyc_Evexp_okay_szofarg(void*t);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 34
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr msg_part);
void Cyc_Tcexp_check_consume(unsigned,void*,struct Cyc_Absyn_Exp*);
# 32
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*);
# 52 "attributes.h"
int Cyc_Atts_attribute_cmp(void*,void*);
# 47 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr ap){
# 49
Cyc_Warn_verr2(loc,ap);
return topt != 0?*topt: Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 52
static void Cyc_Tcexp_err_and_explain(unsigned loc,struct _fat_ptr ap){
Cyc_Warn_verr2(loc,ap);
Cyc_Unify_explain_failure();}
# 57
void Cyc_Tcexp_check_consume(unsigned loc,void*t,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
# 62
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp539;_tmp539.tag=0U,({struct _fat_ptr _tmp673=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp539.f1=_tmp673;});_tmp539;});void*_tmp0[1U];_tmp0[0]=& _tmp1;({unsigned _tmp674=loc;Cyc_Warn_err2(_tmp674,_tag_fat(_tmp0,sizeof(void*),1U));});});}
# 66
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp3;
void*_tmp4=Cyc_Absyn_new_evar(& Cyc_Kinds_tako,({struct Cyc_Core_Opt*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->v=tenv_tvs;_tmpB;}));void*t2=_tmp4;
void*_tmp5=Cyc_Absyn_new_evar(& Cyc_Kinds_trko,({struct Cyc_Core_Opt*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->v=tenv_tvs;_tmpA;}));void*rt=_tmp5;
void*_tmp6=Cyc_Tcutil_any_bool(tenv_tvs);void*nbl=_tmp6;
void*_tmp7=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp7;
void*_tmp8=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp8;
void*_tmp9=Cyc_Tcutil_any_bool(tenv_tvs);void*rel=_tmp9;
return({struct Cyc_Absyn_PtrInfo _tmp53A;_tmp53A.elt_type=t2,({struct Cyc_Absyn_Tqual _tmp675=Cyc_Absyn_empty_tqual(0U);_tmp53A.elt_tq=_tmp675;}),(_tmp53A.ptr_atts).rgn=rt,(_tmp53A.ptr_atts).nullable=nbl,(_tmp53A.ptr_atts).bounds=b,(_tmp53A.ptr_atts).zero_term=zt,(_tmp53A.ptr_atts).ptrloc=0,(_tmp53A.ptr_atts).released=rel;_tmp53A;});}
# 77
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 81
if(topt == 0){
# 83
({void*_tmp676=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->tag=26U,_tmpC->f1=des;_tmpC;});e->r=_tmp676;});
return;}{
# 86
void*t=*topt;
void*_tmpD=Cyc_Absyn_compress(t);void*_stmttmp0=_tmpD;void*_tmpE=_stmttmp0;struct Cyc_Absyn_Tqual _tmp10;void*_tmpF;union Cyc_Absyn_AggrInfo _tmp11;switch(*((int*)_tmpE)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->f1)->tag == 20U){_LL1: _tmp11=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp11;
# 89
{union Cyc_Absyn_AggrInfo _tmp12=info;struct Cyc_Absyn_Aggrdecl*_tmp13;if((_tmp12.UnknownAggr).tag == 1){_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B;_tmp53B.tag=0U,({struct _fat_ptr _tmp677=({const char*_tmp16="struct type improperly set";_tag_fat(_tmp16,sizeof(char),27U);});_tmp53B.f1=_tmp677;});_tmp53B;});void*_tmp14[1U];_tmp14[0]=& _tmp15;({unsigned _tmp678=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp678,_tag_fat(_tmp14,sizeof(void*),1U));});});}else{_LLC: _tmp13=*(_tmp12.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmp13;
({void*_tmp679=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=29U,_tmp17->f1=ad->name,_tmp17->f2=0,_tmp17->f3=des,_tmp17->f4=ad;_tmp17;});e->r=_tmp679;});}}_LL9:;}
# 93
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmpF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE)->f1).elt_type;_tmp10=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE)->f1).tq;_LL4: {void*at=_tmpF;struct Cyc_Absyn_Tqual aq=_tmp10;
({void*_tmp67A=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=26U,_tmp18->f1=des;_tmp18;});e->r=_tmp67A;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp67B=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=30U,_tmp19->f1=t,_tmp19->f2=des;_tmp19;});e->r=_tmp67B;});goto _LL0;default: _LL7: _LL8:
({void*_tmp67C=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->tag=26U,_tmp1A->f1=des;_tmp1A;});e->r=_tmp67C;});goto _LL0;}_LL0:;}}
# 103
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 107
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp1B=e->r;void*_stmttmp1=_tmp1B;void*_tmp1C=_stmttmp1;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1C)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1C)->f2 == 0){_LL1: _LL2:
# 110
 if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53C;_tmp53C.tag=0U,({struct _fat_ptr _tmp67D=({const char*_tmp1F="assignment in test";_tag_fat(_tmp1F,sizeof(char),19U);});_tmp53C.f1=_tmp67D;});_tmp53C;});void*_tmp1D[1U];_tmp1D[0]=& _tmp1E;({unsigned _tmp67E=e->loc;Cyc_Warn_warn2(_tmp67E,_tag_fat(_tmp1D,sizeof(void*),1U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 118
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _tmp541;_tmp541.tag=0U,({struct _fat_ptr _tmp67F=({const char*_tmp28="test of ";_tag_fat(_tmp28,sizeof(char),9U);});_tmp541.f1=_tmp67F;});_tmp541;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp540;_tmp540.tag=0U,_tmp540.f1=msg_part;_tmp540;});struct Cyc_Warn_String_Warn_Warg_struct _tmp23=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53F;_tmp53F.tag=0U,({struct _fat_ptr _tmp680=({const char*_tmp27=" has type ";_tag_fat(_tmp27,sizeof(char),11U);});_tmp53F.f1=_tmp680;});_tmp53F;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp24=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp53E;_tmp53E.tag=3U,_tmp53E.f1=(void*)e->topt;_tmp53E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53D;_tmp53D.tag=0U,({
struct _fat_ptr _tmp681=({const char*_tmp26=" instead of integral or pointer type";_tag_fat(_tmp26,sizeof(char),37U);});_tmp53D.f1=_tmp681;});_tmp53D;});void*_tmp20[5U];_tmp20[0]=& _tmp21,_tmp20[1]=& _tmp22,_tmp20[2]=& _tmp23,_tmp20[3]=& _tmp24,_tmp20[4]=& _tmp25;({unsigned _tmp682=e->loc;Cyc_Warn_err2(_tmp682,_tag_fat(_tmp20,sizeof(void*),5U));});});}
# 140 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 145
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
union Cyc_Absyn_Cnst _tmp29=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _stmttmp2=_tmp29;union Cyc_Absyn_Cnst _tmp2A=_stmttmp2;struct _fat_ptr _tmp2B;struct _fat_ptr _tmp2C;int _tmp2E;enum Cyc_Absyn_Sign _tmp2D;int _tmp2F;switch((_tmp2A.String_c).tag){case 2U: switch(((_tmp2A.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 return Cyc_Absyn_uchar_type;default: _LL5: _LL6:
 return Cyc_Absyn_char_type;}case 3U: _LL7: _LL8:
 return Cyc_Absyn_wchar_type();case 4U: if(((_tmp2A.Short_c).val).f1 == Cyc_Absyn_Unsigned){_LL9: _LLA:
 return Cyc_Absyn_ushort_type;}else{_LLB: _LLC:
 return Cyc_Absyn_sshort_type;}case 6U: if(((_tmp2A.LongLong_c).val).f1 == Cyc_Absyn_Unsigned){_LLD: _LLE:
 return Cyc_Absyn_ulonglong_type;}else{_LLF: _LL10:
 return Cyc_Absyn_slonglong_type;}case 7U: _LL11: _tmp2F=((_tmp2A.Float_c).val).f2;_LL12: {int i=_tmp2F;
# 160
if(topt == 0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_tmp30=Cyc_Absyn_compress(*topt);void*_stmttmp3=_tmp30;void*_tmp31=_stmttmp3;int _tmp32;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f1)->tag == 2U){_LL1C: _tmp32=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f1)->f1;_LL1D: {int i=_tmp32;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E: _LL1F:
 return Cyc_Absyn_gen_float_type((unsigned)i);}_LL1B:;}}case 5U: _LL13: _tmp2D=((_tmp2A.Int_c).val).f1;_tmp2E=((_tmp2A.Int_c).val).f2;_LL14: {enum Cyc_Absyn_Sign csn=_tmp2D;int i=_tmp2E;
# 170
if(topt == 0)
return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;{
void*_tmp33=Cyc_Absyn_compress(*topt);void*_stmttmp4=_tmp33;void*_tmp34=_stmttmp4;void*_tmp35;enum Cyc_Absyn_Sign _tmp36;enum Cyc_Absyn_Sign _tmp37;enum Cyc_Absyn_Sign _tmp38;enum Cyc_Absyn_Sign _tmp39;switch(*((int*)_tmp34)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f1)->f2){case Cyc_Absyn_Char_sz: _LL21: _tmp39=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f1)->f1;_LL22: {enum Cyc_Absyn_Sign sn=_tmp39;
# 174
({union Cyc_Absyn_Cnst _tmp683=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp683;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _LL23: _tmp38=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp38;
# 177
({union Cyc_Absyn_Cnst _tmp684=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp684;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _LL25: _tmp37=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f1)->f1;_LL26: {enum Cyc_Absyn_Sign sn=_tmp37;
_tmp36=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _LL27: _tmp36=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp36;
# 183
({union Cyc_Absyn_Cnst _tmp685=Cyc_Absyn_Int_c(sn,i);*c=_tmp685;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f2 != 0){_LL2B: _tmp35=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f2)->hd;_LL2C: {void*t1=_tmp35;
# 192
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->tag=9U,({struct Cyc_Absyn_Exp*_tmp686=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp3A->f1=_tmp686;});_tmp3A;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3U: _LL29: if(i == 0){_LL2A: {
# 187
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp687=({union Cyc_Absyn_Cnst _tmp542;(_tmp542.Null_c).tag=1U,(_tmp542.Null_c).val=0;_tmp542;});*c=_tmp687;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}}else{goto _LL2D;}default: _LL2D: _LL2E:
# 194
 return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL20:;}}case 1U: _LL15: _LL16:
# 197
 if(topt != 0){
void*_tmp3B=Cyc_Absyn_compress(*topt);void*_stmttmp5=_tmp3B;void*_tmp3C=_stmttmp5;void*_tmp3D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C)->tag == 3U){_LL30: _tmp3D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C)->f1).ptr_atts).nullable;_LL31: {void*nbl=_tmp3D;
# 200
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp544;_tmp544.tag=0U,({struct _fat_ptr _tmp688=({const char*_tmp41="Used NULL when expecting a value of type ";_tag_fat(_tmp41,sizeof(char),42U);});_tmp544.f1=_tmp688;});_tmp544;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp40=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp543;_tmp543.tag=2U,_tmp543.f1=(void*)*topt;_tmp543;});void*_tmp3E[2U];_tmp3E[0]=& _tmp3F,_tmp3E[1]=& _tmp40;({unsigned _tmp689=e->loc;Cyc_Warn_err2(_tmp689,_tag_fat(_tmp3E,sizeof(void*),2U));});});
return*topt;}}else{_LL32: _LL33:
 goto _LL2F;}_LL2F:;}{
# 205
struct Cyc_Absyn_PtrInfo _tmp42=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp42;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
return Cyc_Absyn_pointer_type(pi);}case 8U: _LL17: _tmp2C=(_tmp2A.String_c).val;_LL18: {struct _fat_ptr s=_tmp2C;
# 210
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp2B=s;goto _LL1A;}default: _LL19: _tmp2B=(_tmp2A.Wstring_c).val;_LL1A: {struct _fat_ptr s=_tmp2B;
# 213
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 217
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp68A=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp68A,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 222
void*_tmp43=({void*_tmp68F=string_elt_typ;void*_tmp68E=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp68D=Cyc_Absyn_const_tqual(0U);void*_tmp68C=
Cyc_Absyn_thin_bounds_exp(elen);
# 222
void*_tmp68B=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_tmp68F,_tmp68E,_tmp68D,_tmp68C,_tmp68B,Cyc_Absyn_false_type);});void*t=_tmp43;
# 224
if(topt == 0)
return t;{
void*_tmp44=Cyc_Absyn_compress(*topt);void*_stmttmp6=_tmp44;void*_tmp45=_stmttmp6;struct Cyc_Absyn_Tqual _tmp46;switch(*((int*)_tmp45)){case 4U: _LL35: _tmp46=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp45)->f1).tq;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp46;
# 230
return({void*_tmp692=string_elt_typ;struct Cyc_Absyn_Tqual _tmp691=tq;struct Cyc_Absyn_Exp*_tmp690=elen;Cyc_Absyn_array_type(_tmp692,_tmp691,_tmp690,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL37: _LL38:
# 235
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp695=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp694=loc;void*_tmp693=t;Cyc_Tcutil_silent_castable(_tmp695,_tmp694,_tmp693,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
return*topt;}
# 240
return t;default: _LL39: _LL3A:
 return t;}_LL34:;}}}}}_LL0:;}
# 247
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp47=*((void**)_check_null(b));void*_stmttmp7=_tmp47;void*_tmp48=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4A;struct Cyc_Absyn_Vardecl*_tmp4B;struct Cyc_Absyn_Fndecl*_tmp4C;struct Cyc_Absyn_Vardecl*_tmp4D;struct _tuple1*_tmp4E;switch(*((int*)_tmp48)){case 0U: _LL1: _tmp4E=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp48)->f1;_LL2: {struct _tuple1*q=_tmp4E;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50=({struct Cyc_Warn_String_Warn_Warg_struct _tmp545;_tmp545.tag=0U,({struct _fat_ptr _tmp696=({const char*_tmp51="unresolved binding in tcVar";_tag_fat(_tmp51,sizeof(char),28U);});_tmp545.f1=_tmp696;});_tmp545;});void*_tmp4F[1U];_tmp4F[0]=& _tmp50;({unsigned _tmp697=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp697,_tag_fat(_tmp4F,sizeof(void*),1U));});});}case 1U: _LL3: _tmp4D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp48)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp4D;
# 253
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2U: _LL5: _tmp4C=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp48)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmp4C;
# 260
if(fd->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5U: _LL7: _tmp4B=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp48)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp4B;
_tmp4A=vd;goto _LLA;}case 4U: _LL9: _tmp4A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp48)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp4A;
_tmp49=vd;goto _LLC;}default: _LLB: _tmp49=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp48)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp49;
# 266
if(te->allow_valueof){
void*_tmp52=Cyc_Absyn_compress(vd->type);void*_stmttmp8=_tmp52;void*_tmp53=_stmttmp8;void*_tmp54;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53)->f2 != 0){_LLE: _tmp54=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53)->f2)->hd;_LLF: {void*i=_tmp54;
({void*_tmp698=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=39U,_tmp55->f1=i;_tmp55;});e->r=_tmp698;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 271
return vd->type;}}_LL0:;}
# 275
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 282
void*_tmp56=fmt->r;void*_stmttmp9=_tmp56;void*_tmp57=_stmttmp9;struct _fat_ptr _tmp58;struct _fat_ptr _tmp59;switch(*((int*)_tmp57)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp57)->f1).String_c).tag == 8){_LL1: _tmp59=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp57)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp59;
_tmp58=s;goto _LL4;}}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp57)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp57)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp58=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp57)->f2)->r)->f1).String_c).val;_LL4: {struct _fat_ptr s=_tmp58;
# 285
struct Cyc_List_List*_tmp5A=type_getter(te,(struct _fat_ptr)s,isCproto,fmt->loc);struct Cyc_List_List*desc_types=_tmp5A;
if(opt_args == 0)
return;{
struct Cyc_List_List*_tmp5B=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp5B;
# 290
for(0;desc_types != 0 && args != 0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp69A=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp699=e;Cyc_Tcutil_coerce_arg(_tmp69A,_tmp699,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp549;_tmp549.tag=0U,({struct _fat_ptr _tmp69B=({const char*_tmp62="descriptor has type ";_tag_fat(_tmp62,sizeof(char),21U);});_tmp549.f1=_tmp69B;});_tmp549;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp548;_tmp548.tag=2U,_tmp548.f1=(void*)t;_tmp548;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp547;_tmp547.tag=0U,({
struct _fat_ptr _tmp69C=({const char*_tmp61=" but argument has type ";_tag_fat(_tmp61,sizeof(char),24U);});_tmp547.f1=_tmp69C;});_tmp547;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp60=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp546;_tmp546.tag=3U,_tmp546.f1=(void*)e->topt;_tmp546;});void*_tmp5C[4U];_tmp5C[0]=& _tmp5D,_tmp5C[1]=& _tmp5E,_tmp5C[2]=& _tmp5F,_tmp5C[3]=& _tmp60;({unsigned _tmp69D=e->loc;Cyc_Tcexp_err_and_explain(_tmp69D,_tag_fat(_tmp5C,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp69E=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->hd=(void*)arg_cnt,_tmp63->tl=*alias_arg_exps;_tmp63;});*alias_arg_exps=_tmp69E;});
Cyc_Tcexp_check_consume(e->loc,t,e);}
# 304
if(desc_types != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp65=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54A;_tmp54A.tag=0U,({struct _fat_ptr _tmp69F=({const char*_tmp66="too few arguments";_tag_fat(_tmp66,sizeof(char),18U);});_tmp54A.f1=_tmp69F;});_tmp54A;});void*_tmp64[1U];_tmp64[0]=& _tmp65;({unsigned _tmp6A0=fmt->loc;Cyc_Warn_err2(_tmp6A0,_tag_fat(_tmp64,sizeof(void*),1U));});});
if(args != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp68=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54B;_tmp54B.tag=0U,({struct _fat_ptr _tmp6A1=({const char*_tmp69="too many arguments";_tag_fat(_tmp69,sizeof(char),19U);});_tmp54B.f1=_tmp6A1;});_tmp54B;});void*_tmp67[1U];_tmp67[0]=& _tmp68;({unsigned _tmp6A2=((struct Cyc_Absyn_Exp*)args->hd)->loc;Cyc_Warn_err2(_tmp6A2,_tag_fat(_tmp67,sizeof(void*),1U));});});
# 309
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}
# 312
return;}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 316
 if(opt_args == 0)
return;
{struct Cyc_List_List*_tmp6A=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp6A;for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)args->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt),(struct Cyc_Absyn_Exp*)args->hd);}}
# 322
return;}_LL0:;}
# 326
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 328
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp6B=p;switch(_tmp6B){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 332
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54D;_tmp54D.tag=0U,({struct _fat_ptr _tmp6A3=({const char*_tmp6F="expecting numeric type but found ";_tag_fat(_tmp6F,sizeof(char),34U);});_tmp54D.f1=_tmp6A3;});_tmp54D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54C;_tmp54C.tag=2U,_tmp54C.f1=(void*)t;_tmp54C;});void*_tmp6C[2U];_tmp6C[0]=& _tmp6D,_tmp6C[1]=& _tmp6E;({unsigned _tmp6A4=loc;Cyc_Warn_err2(_tmp6A4,_tag_fat(_tmp6C,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 336
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp71=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54F;_tmp54F.tag=0U,({struct _fat_ptr _tmp6A5=({const char*_tmp73="expecting integral or * type but found ";_tag_fat(_tmp73,sizeof(char),40U);});_tmp54F.f1=_tmp6A5;});_tmp54F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp72=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54E;_tmp54E.tag=2U,_tmp54E.f1=(void*)t;_tmp54E;});void*_tmp70[2U];_tmp70[0]=& _tmp71,_tmp70[1]=& _tmp72;({unsigned _tmp6A6=loc;Cyc_Warn_err2(_tmp6A6,_tag_fat(_tmp70,sizeof(void*),2U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 341
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp75=({struct Cyc_Warn_String_Warn_Warg_struct _tmp551;_tmp551.tag=0U,({struct _fat_ptr _tmp6A7=({const char*_tmp77="expecting integral type but found ";_tag_fat(_tmp77,sizeof(char),35U);});_tmp551.f1=_tmp6A7;});_tmp551;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp76=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp550;_tmp550.tag=2U,_tmp550.f1=(void*)t;_tmp550;});void*_tmp74[2U];_tmp74[0]=& _tmp75,_tmp74[1]=& _tmp76;({unsigned _tmp6A8=loc;Cyc_Warn_err2(_tmp6A8,_tag_fat(_tmp74,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 345
{void*_tmp78=t;void*_tmp79;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78)->tag == 3U){_LLE: _tmp79=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp79;
# 347
struct Cyc_Absyn_Exp*_tmp7A=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp7A;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp552;_tmp552.tag=0U,({struct _fat_ptr _tmp6A9=({const char*_tmp7D="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp7D,sizeof(char),53U);});_tmp552.f1=_tmp6A9;});_tmp552;});void*_tmp7B[1U];_tmp7B[0]=& _tmp7C;({unsigned _tmp6AA=loc;Cyc_Warn_err2(_tmp6AA,_tag_fat(_tmp7B,sizeof(void*),1U));});});
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp554;_tmp554.tag=0U,({struct _fat_ptr _tmp6AB=({const char*_tmp81="numelts requires pointer type, not ";_tag_fat(_tmp81,sizeof(char),36U);});_tmp554.f1=_tmp6AB;});_tmp554;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp80=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp553;_tmp553.tag=2U,_tmp553.f1=(void*)t;_tmp553;});void*_tmp7E[2U];_tmp7E[0]=& _tmp7F,_tmp7E[1]=& _tmp80;({unsigned _tmp6AC=loc;Cyc_Warn_err2(_tmp6AC,_tag_fat(_tmp7E,sizeof(void*),2U));});});}_LLD:;}
# 353
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp83=({struct Cyc_Warn_String_Warn_Warg_struct _tmp555;_tmp555.tag=0U,({struct _fat_ptr _tmp6AD=({const char*_tmp84="Non-unary primop";_tag_fat(_tmp84,sizeof(char),17U);});_tmp555.f1=_tmp6AD;});_tmp555;});void*_tmp82[1U];_tmp82[0]=& _tmp83;({unsigned _tmp6AE=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp6AE,_tag_fat(_tmp82,sizeof(void*),1U));});});}_LL0:;}
# 360
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp85=(void*)_check_null(e1->topt);void*t1=_tmp85;
void*_tmp86=(void*)_check_null(e2->topt);void*t2=_tmp86;
void*_tmp87=Cyc_Tcutil_max_arithmetic_type(t1,t2);void*new_typ=_tmp87;
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,Cyc_Absyn_No_coercion);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,Cyc_Absyn_No_coercion);
return new_typ;}
# 370
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 373
if(!checker(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp558;_tmp558.tag=0U,({struct _fat_ptr _tmp6AF=({const char*_tmp8D="type ";_tag_fat(_tmp8D,sizeof(char),6U);});_tmp558.f1=_tmp6AF;});_tmp558;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp557;_tmp557.tag=3U,_tmp557.f1=(void*)e1->topt;_tmp557;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp556;_tmp556.tag=0U,({struct _fat_ptr _tmp6B0=({const char*_tmp8C=" cannot be used here";_tag_fat(_tmp8C,sizeof(char),21U);});_tmp556.f1=_tmp6B0;});_tmp556;});void*_tmp88[3U];_tmp88[0]=& _tmp89,_tmp88[1]=& _tmp8A,_tmp88[2]=& _tmp8B;({struct Cyc_Tcenv_Tenv*_tmp6B2=te;unsigned _tmp6B1=e1->loc;Cyc_Tcexp_expr_err(_tmp6B2,_tmp6B1,0,_tag_fat(_tmp88,sizeof(void*),3U));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55B;_tmp55B.tag=0U,({struct _fat_ptr _tmp6B3=({const char*_tmp93="type ";_tag_fat(_tmp93,sizeof(char),6U);});_tmp55B.f1=_tmp6B3;});_tmp55B;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp90=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp55A;_tmp55A.tag=3U,_tmp55A.f1=(void*)e2->topt;_tmp55A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp91=({struct Cyc_Warn_String_Warn_Warg_struct _tmp559;_tmp559.tag=0U,({struct _fat_ptr _tmp6B4=({const char*_tmp92=" cannot be used here";_tag_fat(_tmp92,sizeof(char),21U);});_tmp559.f1=_tmp6B4;});_tmp559;});void*_tmp8E[3U];_tmp8E[0]=& _tmp8F,_tmp8E[1]=& _tmp90,_tmp8E[2]=& _tmp91;({struct Cyc_Tcenv_Tenv*_tmp6B6=te;unsigned _tmp6B5=e2->loc;Cyc_Tcexp_expr_err(_tmp6B6,_tmp6B5,0,_tag_fat(_tmp8E,sizeof(void*),3U));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 380
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
void*_tmp94=t1;void*_tmp9B;void*_tmp9A;void*_tmp99;void*_tmp98;void*_tmp97;struct Cyc_Absyn_Tqual _tmp96;void*_tmp95;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->tag == 3U){_LL1: _tmp95=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).elt_type;_tmp96=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).elt_tq;_tmp97=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).rgn;_tmp98=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).nullable;_tmp99=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).bounds;_tmp9A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).zero_term;_tmp9B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).released;_LL2: {void*et=_tmp95;struct Cyc_Absyn_Tqual tq=_tmp96;void*r=_tmp97;void*n=_tmp98;void*b=_tmp99;void*zt=_tmp9A;void*rel=_tmp9B;
# 385
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55C;_tmp55C.tag=0U,({struct _fat_ptr _tmp6B7=({const char*_tmp9E="can't do arithmetic on abstract pointer type";_tag_fat(_tmp9E,sizeof(char),45U);});_tmp55C.f1=_tmp6B7;});_tmp55C;});void*_tmp9C[1U];_tmp9C[0]=& _tmp9D;({unsigned _tmp6B8=e1->loc;Cyc_Warn_err2(_tmp6B8,_tag_fat(_tmp9C,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55D;_tmp55D.tag=0U,({struct _fat_ptr _tmp6B9=({const char*_tmpA1="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmpA1,sizeof(char),49U);});_tmp55D.f1=_tmp6B9;});_tmp55D;});void*_tmp9F[1U];_tmp9F[0]=& _tmpA0;({unsigned _tmp6BA=e1->loc;Cyc_Warn_err2(_tmp6BA,_tag_fat(_tmp9F,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55F;_tmp55F.tag=0U,({struct _fat_ptr _tmp6BB=({const char*_tmpA5="expecting int but found ";_tag_fat(_tmpA5,sizeof(char),25U);});_tmp55F.f1=_tmp6BB;});_tmp55F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55E;_tmp55E.tag=2U,_tmp55E.f1=(void*)t2;_tmp55E;});void*_tmpA2[2U];_tmpA2[0]=& _tmpA3,_tmpA2[1]=& _tmpA4;({unsigned _tmp6BC=e2->loc;Cyc_Warn_err2(_tmp6BC,_tag_fat(_tmpA2,sizeof(void*),2U));});});{
struct Cyc_Absyn_Exp*_tmpA6=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmpA6;
if(eopt == 0)
return t1;
# 396
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmpA7=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmpA7;struct _tuple16 _tmpA8=_stmttmpA;int _tmpAA;unsigned _tmpA9;_LL6: _tmpA9=_tmpA8.f1;_tmpAA=_tmpA8.f2;_LL7: {unsigned i=_tmpA9;int known=_tmpAA;
if(known && i == (unsigned)1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp560;_tmp560.tag=0U,({struct _fat_ptr _tmp6BD=({const char*_tmpAD="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpAD,sizeof(char),70U);});_tmp560.f1=_tmp6BD;});_tmp560;});void*_tmpAB[1U];_tmpAB[0]=& _tmpAC;({unsigned _tmp6BE=e1->loc;Cyc_Warn_warn2(_tmp6BE,_tag_fat(_tmpAB,sizeof(void*),1U));});});}}{
# 407
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAE=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=3U,(_tmpAF->f1).elt_type=et,(_tmpAF->f1).elt_tq=tq,((_tmpAF->f1).ptr_atts).rgn=r,((_tmpAF->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpAF->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpAF->f1).ptr_atts).zero_term=zt,((_tmpAF->f1).ptr_atts).ptrloc=0,((_tmpAF->f1).ptr_atts).released=rel;_tmpAF;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpAE;
# 410
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,Cyc_Absyn_Other_coercion);
return(void*)new_t1;}}}}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 415
static void*Cyc_Tcexp_relqual(void*t){
void*_tmpB0=t;void*_tmpB1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0)->tag == 3U){_LL1: _tmpB1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0)->f1).ptr_atts).released;_LL2: {void*rel=_tmpB1;
# 418
return rel;}}else{_LL3: _LL4:
 return Cyc_Absyn_false_type;}_LL0:;}
# 424
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp566;_tmp566.tag=0U,({struct _fat_ptr _tmp6BF=({const char*_tmpBC="pointer arithmetic on values of different ";_tag_fat(_tmpBC,sizeof(char),43U);});_tmp566.f1=_tmp6BF;});_tmp566;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp565;_tmp565.tag=0U,({
struct _fat_ptr _tmp6C0=({const char*_tmpBB="types (";_tag_fat(_tmpBB,sizeof(char),8U);});_tmp565.f1=_tmp6C0;});_tmp565;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp564;_tmp564.tag=2U,_tmp564.f1=(void*)t1;_tmp564;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp563;_tmp563.tag=0U,({struct _fat_ptr _tmp6C1=({const char*_tmpBA=" != ";_tag_fat(_tmpBA,sizeof(char),5U);});_tmp563.f1=_tmp6C1;});_tmp563;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp562;_tmp562.tag=2U,_tmp562.f1=(void*)t2;_tmp562;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp561;_tmp561.tag=0U,({struct _fat_ptr _tmp6C2=({const char*_tmpB9=")";_tag_fat(_tmpB9,sizeof(char),2U);});_tmp561.f1=_tmp6C2;});_tmp561;});void*_tmpB2[6U];_tmpB2[0]=& _tmpB3,_tmpB2[1]=& _tmpB4,_tmpB2[2]=& _tmpB5,_tmpB2[3]=& _tmpB6,_tmpB2[4]=& _tmpB7,_tmpB2[5]=& _tmpB8;({unsigned _tmp6C3=e1->loc;Cyc_Tcexp_err_and_explain(_tmp6C3,_tag_fat(_tmpB2,sizeof(void*),6U));});});
return Cyc_Absyn_sint_type;}
# 436
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp6C4=t1_elt;Cyc_Unify_unify(_tmp6C4,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56C;_tmp56C.tag=0U,({struct _fat_ptr _tmp6C5=({const char*_tmpC7="pointer arithmetic on values of different ";_tag_fat(_tmpC7,sizeof(char),43U);});_tmp56C.f1=_tmp6C5;});_tmp56C;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56B;_tmp56B.tag=0U,({
struct _fat_ptr _tmp6C6=({const char*_tmpC6="types(";_tag_fat(_tmpC6,sizeof(char),7U);});_tmp56B.f1=_tmp6C6;});_tmp56B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpC0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56A;_tmp56A.tag=2U,_tmp56A.f1=(void*)t1;_tmp56A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp569;_tmp569.tag=0U,({struct _fat_ptr _tmp6C7=({const char*_tmpC5=" != ";_tag_fat(_tmpC5,sizeof(char),5U);});_tmp569.f1=_tmp6C7;});_tmp569;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpC2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp568;_tmp568.tag=2U,_tmp568.f1=(void*)t2;_tmp568;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp567;_tmp567.tag=0U,({struct _fat_ptr _tmp6C8=({const char*_tmpC4=")";_tag_fat(_tmpC4,sizeof(char),2U);});_tmp567.f1=_tmp6C8;});_tmp567;});void*_tmpBD[6U];_tmpBD[0]=& _tmpBE,_tmpBD[1]=& _tmpBF,_tmpBD[2]=& _tmpC0,_tmpBD[3]=& _tmpC1,_tmpBD[4]=& _tmpC2,_tmpBD[5]=& _tmpC3;({unsigned _tmp6C9=e1->loc;Cyc_Tcexp_err_and_explain(_tmp6C9,_tag_fat(_tmpBD,sizeof(void*),6U));});});
# 441
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56D;_tmp56D.tag=0U,({struct _fat_ptr _tmp6CA=({const char*_tmpCA="coercing fat pointer to thin pointer for subtraction";_tag_fat(_tmpCA,sizeof(char),53U);});_tmp56D.f1=_tmp6CA;});_tmp56D;});void*_tmpC8[1U];_tmpC8[0]=& _tmpC9;({unsigned _tmp6CB=e1->loc;Cyc_Warn_warn2(_tmp6CB,_tag_fat(_tmpC8,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp6D0=e1;Cyc_Tcutil_unchecked_cast(_tmp6D0,({void*_tmp6CF=t1_elt;void*_tmp6CE=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp6CD=
Cyc_Absyn_empty_tqual(0U);
# 442
void*_tmp6CC=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_tmp6CF,_tmp6CE,_tmp6CD,_tmp6CC,
Cyc_Tcexp_relqual(t1));}),Cyc_Absyn_Other_coercion);});
# 445
return Cyc_Absyn_sint_type;}
# 447
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56E;_tmp56E.tag=0U,({struct _fat_ptr _tmp6D1=({const char*_tmpCD="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpCD,sizeof(char),50U);});_tmp56E.f1=_tmp6D1;});_tmp56E;});void*_tmpCB[1U];_tmpCB[0]=& _tmpCC;({unsigned _tmp6D2=e1->loc;Cyc_Warn_err2(_tmp6D2,_tag_fat(_tmpCB,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56F;_tmp56F.tag=0U,({struct _fat_ptr _tmp6D3=({const char*_tmpD0="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpD0,sizeof(char),54U);});_tmp56F.f1=_tmp6D3;});_tmp56F;});void*_tmpCE[1U];_tmpCE[0]=& _tmpCF;({unsigned _tmp6D4=e1->loc;Cyc_Warn_err2(_tmp6D4,_tag_fat(_tmpCE,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp573;_tmp573.tag=0U,({struct _fat_ptr _tmp6D5=({const char*_tmpD7="expecting ";_tag_fat(_tmpD7,sizeof(char),11U);});_tmp573.f1=_tmp6D5;});_tmp573;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp572;_tmp572.tag=2U,_tmp572.f1=(void*)t1;_tmp572;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp571;_tmp571.tag=0U,({struct _fat_ptr _tmp6D6=({const char*_tmpD6=" or int but found ";_tag_fat(_tmpD6,sizeof(char),19U);});_tmp571.f1=_tmp6D6;});_tmp571;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp570;_tmp570.tag=2U,_tmp570.f1=(void*)t2;_tmp570;});void*_tmpD1[4U];_tmpD1[0]=& _tmpD2,_tmpD1[1]=& _tmpD3,_tmpD1[2]=& _tmpD4,_tmpD1[3]=& _tmpD5;({unsigned _tmp6D7=e2->loc;Cyc_Tcexp_err_and_explain(_tmp6D7,_tag_fat(_tmpD1,sizeof(void*),4U));});});
return t1;}
# 456
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp6D8=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp6D8,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp574;_tmp574.tag=0U,({struct _fat_ptr _tmp6D9=({const char*_tmpDA="coercing fat pointer to thin pointer for subtraction";_tag_fat(_tmpDA,sizeof(char),53U);});_tmp574.f1=_tmp6D9;});_tmp574;});void*_tmpD8[1U];_tmpD8[0]=& _tmpD9;({unsigned _tmp6DA=e1->loc;Cyc_Warn_warn2(_tmp6DA,_tag_fat(_tmpD8,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp6DF=e2;Cyc_Tcutil_unchecked_cast(_tmp6DF,({void*_tmp6DE=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp6DD=
Cyc_Tcutil_pointer_region(t2);
# 461
struct Cyc_Absyn_Tqual _tmp6DC=
# 463
Cyc_Absyn_empty_tqual(0U);
# 461
void*_tmp6DB=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_tmp6DE,_tmp6DD,_tmp6DC,_tmp6DB,
# 463
Cyc_Tcexp_relqual(t2));}),Cyc_Absyn_Other_coercion);});}
# 466
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp575;_tmp575.tag=0U,({struct _fat_ptr _tmp6E0=({const char*_tmpDD="thin pointer subtraction!";_tag_fat(_tmpDD,sizeof(char),26U);});_tmp575.f1=_tmp6E0;});_tmp575;});void*_tmpDB[1U];_tmpDB[0]=& _tmpDC;({unsigned _tmp6E1=e1->loc;Cyc_Warn_warn2(_tmp6E1,_tag_fat(_tmpDB,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;}
# 469
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp576;_tmp576.tag=0U,({struct _fat_ptr _tmp6E2=({const char*_tmpE0="coercing thin pointer to integer for subtraction";_tag_fat(_tmpE0,sizeof(char),49U);});_tmp576.f1=_tmp6E2;});_tmp576;});void*_tmpDE[1U];_tmpDE[0]=& _tmpDF;({unsigned _tmp6E3=e1->loc;Cyc_Warn_warn2(_tmp6E3,_tag_fat(_tmpDE,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 472
if(Cyc_Tcutil_is_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp577;_tmp577.tag=0U,({struct _fat_ptr _tmp6E4=({const char*_tmpE3="coercing pointer to integer for subtraction";_tag_fat(_tmpE3,sizeof(char),44U);});_tmp577.f1=_tmp6E4;});_tmp577;});void*_tmpE1[1U];_tmpE1[0]=& _tmpE2;({unsigned _tmp6E5=e1->loc;Cyc_Warn_warn2(_tmp6E5,_tag_fat(_tmpE1,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 477
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 480
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 488
if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind ||({
void*_tmp6E6=t1;Cyc_Unify_unify(_tmp6E6,Cyc_Absyn_new_evar(& Cyc_Kinds_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 493
if(({struct Cyc_RgnOrder_RgnPO*_tmp6E9=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6E8=loc;void*_tmp6E7=t2;Cyc_Tcutil_silent_castable(_tmp6E9,_tmp6E8,_tmp6E7,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 497
if(({struct Cyc_RgnOrder_RgnPO*_tmp6EC=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6EB=loc;void*_tmp6EA=t1;Cyc_Tcutil_silent_castable(_tmp6EC,_tmp6EB,_tmp6EA,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 501
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 506
{struct _tuple0 _tmpE4=({struct _tuple0 _tmp578;({void*_tmp6EE=Cyc_Absyn_compress(t1);_tmp578.f1=_tmp6EE;}),({void*_tmp6ED=Cyc_Absyn_compress(t2);_tmp578.f2=_tmp6ED;});_tmp578;});struct _tuple0 _stmttmpB=_tmpE4;struct _tuple0 _tmpE5=_stmttmpB;void*_tmpE7;void*_tmpE6;switch(*((int*)_tmpE5.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE5.f2)->tag == 3U){_LL1: _tmpE6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE5.f1)->f1).elt_type;_tmpE7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE5.f2)->f1).elt_type;_LL2: {void*t1a=_tmpE6;void*t2a=_tmpE7;
# 508
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE5.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE5.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE5.f2)->f1)->tag == 3U){_LL3: _LL4:
# 512
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 516
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57C;_tmp57C.tag=0U,({struct _fat_ptr _tmp6EF=({const char*_tmpEE="comparison not allowed between ";_tag_fat(_tmpEE,sizeof(char),32U);});_tmp57C.f1=_tmp6EF;});_tmp57C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpEA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57B;_tmp57B.tag=2U,_tmp57B.f1=(void*)t1;_tmp57B;});struct Cyc_Warn_String_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57A;_tmp57A.tag=0U,({struct _fat_ptr _tmp6F0=({const char*_tmpED=" and ";_tag_fat(_tmpED,sizeof(char),6U);});_tmp57A.f1=_tmp6F0;});_tmp57A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpEC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp579;_tmp579.tag=2U,_tmp579.f1=(void*)t2;_tmp579;});void*_tmpE8[4U];_tmpE8[0]=& _tmpE9,_tmpE8[1]=& _tmpEA,_tmpE8[2]=& _tmpEB,_tmpE8[3]=& _tmpEC;({unsigned _tmp6F1=loc;Cyc_Tcexp_err_and_explain(_tmp6F1,_tag_fat(_tmpE8,sizeof(void*),4U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 522
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 524
enum Cyc_Absyn_Primop _tmpEF=p;switch(_tmpEF){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 528
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 531
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL15: _LL16:
# 540
 goto _LL18;case Cyc_Absyn_Neq: _LL17: _LL18:
 goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A:
 goto _LL1C;case Cyc_Absyn_Lt: _LL1B: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL1F: _LL20:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);default: _LL21: _LL22:
# 547
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57D;_tmp57D.tag=0U,({struct _fat_ptr _tmp6F2=({const char*_tmpF2="bad binary primop";_tag_fat(_tmpF2,sizeof(char),18U);});_tmp57D.f1=_tmp6F2;});_tmp57D;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpF0,sizeof(void*),1U));});}_LL0:;}
# 551
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 559
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpF3=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpF3;
void*_tmpF4=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpF4;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57F;_tmp57F.tag=0U,({struct _fat_ptr _tmp6F3=({const char*_tmpF8="expecting numeric type but found ";_tag_fat(_tmpF8,sizeof(char),34U);});_tmp57F.f1=_tmp6F3;});_tmp57F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57E;_tmp57E.tag=2U,_tmp57E.f1=(void*)t;_tmp57E;});void*_tmpF5[2U];_tmpF5[0]=& _tmpF6,_tmpF5[1]=& _tmpF7;({unsigned _tmp6F4=e->loc;Cyc_Warn_err2(_tmp6F4,_tag_fat(_tmpF5,sizeof(void*),2U));});});
return t;}
# 566
{struct Cyc_List_List*_tmpF9=es;struct Cyc_List_List*es2=_tmpF9;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _tmpFA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpFA;int _tmpFB=_stmttmpC;switch(_tmpFB){case 0U: _LL1: _LL2:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpFD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp580;_tmp580.tag=0U,({struct _fat_ptr _tmp6F5=({const char*_tmpFE="primitive operator has 0 arguments";_tag_fat(_tmpFE,sizeof(char),35U);});_tmp580.f1=_tmp6F5;});_tmp580;});void*_tmpFC[1U];_tmpFC[0]=& _tmpFD;({struct Cyc_Tcenv_Tenv*_tmp6F8=te;unsigned _tmp6F7=loc;void**_tmp6F6=topt;Cyc_Tcexp_expr_err(_tmp6F8,_tmp6F7,_tmp6F6,_tag_fat(_tmpFC,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);case 2U: _LL5: _LL6:
 return({struct Cyc_Tcenv_Tenv*_tmp6FD=te;unsigned _tmp6FC=loc;void**_tmp6FB=topt;enum Cyc_Absyn_Primop _tmp6FA=p;struct Cyc_Absyn_Exp*_tmp6F9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp6FD,_tmp6FC,_tmp6FB,_tmp6FA,_tmp6F9,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp100=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0U,({struct _fat_ptr _tmp6FE=({const char*_tmp101="primitive operator has > 2 arguments";_tag_fat(_tmp101,sizeof(char),37U);});_tmp581.f1=_tmp6FE;});_tmp581;});void*_tmpFF[1U];_tmpFF[0]=& _tmp100;({struct Cyc_Tcenv_Tenv*_tmp701=te;unsigned _tmp700=loc;void**_tmp6FF=topt;Cyc_Tcexp_expr_err(_tmp701,_tmp700,_tmp6FF,_tag_fat(_tmpFF,sizeof(void*),1U));});});}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 576
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
t=Cyc_Absyn_compress(t);{
void*_tmp102=t;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Tqual _tmp105;void*_tmp104;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Datatypefield*_tmp107;struct Cyc_Absyn_Aggrdecl*_tmp108;switch(*((int*)_tmp102)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp102)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp102)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp108=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp102)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp108;
# 580
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp583;_tmp583.tag=0U,({struct _fat_ptr _tmp702=({const char*_tmp10C="attempt to write abstract ";_tag_fat(_tmp10C,sizeof(char),27U);});_tmp583.f1=_tmp702;});_tmp583;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp10B=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp582;_tmp582.tag=6U,_tmp582.f1=ad;_tmp582;});void*_tmp109[2U];_tmp109[0]=& _tmp10A,_tmp109[1]=& _tmp10B;({unsigned _tmp703=loc;Cyc_Warn_err2(_tmp703,_tag_fat(_tmp109,sizeof(void*),2U));});});
return 0;}
# 584
_tmp106=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp102)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmp107=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp102)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmp107;
# 596
{struct Cyc_List_List*_tmp112=df->typs;struct Cyc_List_List*fs=_tmp112;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp113=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmp113;struct _tuple17*_tmp114=_stmttmpD;void*_tmp116;struct Cyc_Absyn_Tqual _tmp115;_LLE: _tmp115=_tmp114->f1;_tmp116=_tmp114->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp115;void*t=_tmp116;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp118=({struct Cyc_Warn_String_Warn_Warg_struct _tmp586;_tmp586.tag=0U,({struct _fat_ptr _tmp704=({const char*_tmp11C="attempt to overwrite a datatype field (";_tag_fat(_tmp11C,sizeof(char),40U);});_tmp586.f1=_tmp704;});_tmp586;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp119=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp585;_tmp585.tag=1U,_tmp585.f1=df->name;_tmp585;});struct Cyc_Warn_String_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp584;_tmp584.tag=0U,({
struct _fat_ptr _tmp705=({const char*_tmp11B=") with a const member";_tag_fat(_tmp11B,sizeof(char),22U);});_tmp584.f1=_tmp705;});_tmp584;});void*_tmp117[3U];_tmp117[0]=& _tmp118,_tmp117[1]=& _tmp119,_tmp117[2]=& _tmp11A;({unsigned _tmp706=loc;Cyc_Warn_err2(_tmp706,_tag_fat(_tmp117,sizeof(void*),3U));});});
return 0;}
# 603
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 605
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmp106=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp102)->f2;_LL4: {struct Cyc_List_List*fs=_tmp106;
# 586
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp10D=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp10D;
if((f->tq).real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp588;_tmp588.tag=0U,({struct _fat_ptr _tmp707=({const char*_tmp111="attempt to overwrite an aggregate with const member ";_tag_fat(_tmp111,sizeof(char),53U);});_tmp588.f1=_tmp707;});_tmp588;});struct Cyc_Warn_String_Warn_Warg_struct _tmp110=({struct Cyc_Warn_String_Warn_Warg_struct _tmp587;_tmp587.tag=0U,_tmp587.f1=*f->name;_tmp587;});void*_tmp10E[2U];_tmp10E[0]=& _tmp10F,_tmp10E[1]=& _tmp110;({unsigned _tmp708=loc;Cyc_Warn_err2(_tmp708,_tag_fat(_tmp10E,sizeof(void*),2U));});});
return 0;}
# 592
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 594
return 1;}case 4U: _LL7: _tmp104=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102)->f1).elt_type;_tmp105=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102)->f1).tq;_LL8: {void*elt_type=_tmp104;struct Cyc_Absyn_Tqual tq=_tmp105;
# 607
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp589;_tmp589.tag=0U,({struct _fat_ptr _tmp709=({const char*_tmp11F="attempt to overwrite a const array";_tag_fat(_tmp11F,sizeof(char),35U);});_tmp589.f1=_tmp709;});_tmp589;});void*_tmp11D[1U];_tmp11D[0]=& _tmp11E;({unsigned _tmp70A=loc;Cyc_Warn_err2(_tmp70A,_tag_fat(_tmp11D,sizeof(void*),1U));});});
return 0;}
# 611
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmp103=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp102)->f1;_LLA: {struct Cyc_List_List*fs=_tmp103;
# 613
for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp120=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp120;struct _tuple17*_tmp121=_stmttmpE;void*_tmp123;struct Cyc_Absyn_Tqual _tmp122;_LL11: _tmp122=_tmp121->f1;_tmp123=_tmp121->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmp122;void*t=_tmp123;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp125=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58A;_tmp58A.tag=0U,({struct _fat_ptr _tmp70B=({const char*_tmp126="attempt to overwrite a tuple field with a const member";_tag_fat(_tmp126,sizeof(char),55U);});_tmp58A.f1=_tmp70B;});_tmp58A;});void*_tmp124[1U];_tmp124[0]=& _tmp125;({unsigned _tmp70C=loc;Cyc_Warn_err2(_tmp70C,_tag_fat(_tmp124,sizeof(void*),1U));});});
return 0;}
# 619
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}
# 621
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}}
# 629
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 632
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp127=e->r;void*_stmttmpF=_tmp127;void*_tmp128=_stmttmpF;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12B;struct _fat_ptr*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct _fat_ptr*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Vardecl*_tmp132;struct Cyc_Absyn_Vardecl*_tmp133;struct Cyc_Absyn_Vardecl*_tmp134;struct Cyc_Absyn_Vardecl*_tmp135;switch(*((int*)_tmp128)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp128)->f1)){case 3U: _LL1: _tmp135=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp128)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp135;
_tmp134=vd;goto _LL4;}case 4U: _LL3: _tmp134=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp128)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp134;
_tmp133=vd;goto _LL6;}case 5U: _LL5: _tmp133=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp128)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp133;
_tmp132=vd;goto _LL8;}case 1U: _LL7: _tmp132=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp128)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp132;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23U: _LL9: _tmp130=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp128)->f1;_tmp131=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp128)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp130;struct Cyc_Absyn_Exp*e2=_tmp131;
# 639
{void*_tmp136=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp136;void*_tmp137=_stmttmp10;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Tqual _tmp139;struct Cyc_Absyn_Tqual _tmp13A;switch(*((int*)_tmp137)){case 3U: _LL18: _tmp13A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp137)->f1).elt_tq;_LL19: {struct Cyc_Absyn_Tqual tq=_tmp13A;
_tmp139=tq;goto _LL1B;}case 4U: _LL1A: _tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp137)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp139;
if(!tq.real_const)return;goto _LL17;}case 6U: _LL1C: _tmp138=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp137)->f1;_LL1D: {struct Cyc_List_List*ts=_tmp138;
# 643
struct _tuple16 _tmp13B=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp11=_tmp13B;struct _tuple16 _tmp13C=_stmttmp11;int _tmp13E;unsigned _tmp13D;_LL21: _tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;_LL22: {unsigned i=_tmp13D;int known=_tmp13E;
if(!known){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp140=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58B;_tmp58B.tag=0U,({struct _fat_ptr _tmp70D=({const char*_tmp141="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp141,sizeof(char),47U);});_tmp58B.f1=_tmp70D;});_tmp58B;});void*_tmp13F[1U];_tmp13F[0]=& _tmp140;({unsigned _tmp70E=e->loc;Cyc_Warn_err2(_tmp70E,_tag_fat(_tmp13F,sizeof(void*),1U));});});
return;}
# 648
{struct _handler_cons _tmp142;_push_handler(& _tmp142);{int _tmp144=0;if(setjmp(_tmp142.handler))_tmp144=1;if(!_tmp144){
{struct _tuple17*_tmp145=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i);struct _tuple17*_stmttmp12=_tmp145;struct _tuple17*_tmp146=_stmttmp12;struct Cyc_Absyn_Tqual _tmp147;_LL24: _tmp147=_tmp146->f1;_LL25: {struct Cyc_Absyn_Tqual tq=_tmp147;
if(!tq.real_const){_npop_handler(0U);return;}}}
# 649
;_pop_handler();}else{void*_tmp143=(void*)Cyc_Core_get_exn_thrown();void*_tmp148=_tmp143;void*_tmp149;if(((struct Cyc_List_Nth_exn_struct*)_tmp148)->tag == Cyc_List_Nth){_LL27: _LL28:
# 651
 return;}else{_LL29: _tmp149=_tmp148;_LL2A: {void*exn=_tmp149;(int)_rethrow(exn);}}_LL26:;}}}
goto _LL17;}}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 655
goto _LL0;}case 21U: _LLB: _tmp12E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp128)->f1;_tmp12F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp128)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp12E;struct _fat_ptr*f=_tmp12F;
# 657
{void*_tmp14A=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp14A;void*_tmp14B=_stmttmp13;struct Cyc_List_List*_tmp14C;struct Cyc_Absyn_Aggrdecl**_tmp14D;switch(*((int*)_tmp14B)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14B)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp14D=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14B)->f1)->f1).KnownAggr).val;_LL2D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp14D;
# 659
struct Cyc_Absyn_Aggrfield*sf=adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp14C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14B)->f2;_LL2F: {struct Cyc_List_List*fs=_tmp14C;
# 663
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 668
goto _LL0;}case 22U: _LLD: _tmp12C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp128)->f1;_tmp12D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp128)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp12C;struct _fat_ptr*f=_tmp12D;
# 670
{void*_tmp14E=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp14E;void*_tmp14F=_stmttmp14;struct Cyc_Absyn_Tqual _tmp151;void*_tmp150;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14F)->tag == 3U){_LL33: _tmp150=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14F)->f1).elt_type;_tmp151=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14F)->f1).elt_tq;_LL34: {void*elt_typ=_tmp150;struct Cyc_Absyn_Tqual tq=_tmp151;
# 672
if(!tq.real_const){
void*_tmp152=Cyc_Absyn_compress(elt_typ);void*_stmttmp15=_tmp152;void*_tmp153=_stmttmp15;struct Cyc_List_List*_tmp154;struct Cyc_Absyn_Aggrdecl**_tmp155;switch(*((int*)_tmp153)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp153)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp153)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp155=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp153)->f1)->f1).KnownAggr).val;_LL39: {struct Cyc_Absyn_Aggrdecl**adp=_tmp155;
# 675
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp154=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp153)->f2;_LL3B: {struct Cyc_List_List*fs=_tmp154;
# 680
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 686
goto _LL32;}}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 689
goto _LL0;}case 20U: _LLF: _tmp12B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp128)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp12B;
# 691
{void*_tmp156=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp156;void*_tmp157=_stmttmp16;struct Cyc_Absyn_Tqual _tmp158;struct Cyc_Absyn_Tqual _tmp159;switch(*((int*)_tmp157)){case 3U: _LL3F: _tmp159=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp157)->f1).elt_tq;_LL40: {struct Cyc_Absyn_Tqual tq=_tmp159;
_tmp158=tq;goto _LL42;}case 4U: _LL41: _tmp158=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp157)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp158;
if(!tq.real_const)return;goto _LL3E;}default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 696
goto _LL0;}case 12U: _LL11: _tmp12A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp128)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp12A;
_tmp129=e1;goto _LL14;}case 13U: _LL13: _tmp129=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp128)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp129;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 701
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58D;_tmp58D.tag=0U,({struct _fat_ptr _tmp70F=({const char*_tmp15D="attempt to write a const location: ";_tag_fat(_tmp15D,sizeof(char),36U);});_tmp58D.f1=_tmp70F;});_tmp58D;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp15C=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp58C;_tmp58C.tag=4U,_tmp58C.f1=e;_tmp58C;});void*_tmp15A[2U];_tmp15A[0]=& _tmp15B,_tmp15A[1]=& _tmp15C;({unsigned _tmp710=e->loc;Cyc_Warn_err2(_tmp710,_tag_fat(_tmp15A,sizeof(void*),2U));});});}
# 704
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 707
({struct Cyc_Tcenv_Tenv*_tmp711=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp711,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp15F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58E;_tmp58E.tag=0U,({struct _fat_ptr _tmp712=({const char*_tmp160="increment/decrement of non-lvalue";_tag_fat(_tmp160,sizeof(char),34U);});_tmp58E.f1=_tmp712;});_tmp58E;});void*_tmp15E[1U];_tmp15E[0]=& _tmp15F;({struct Cyc_Tcenv_Tenv*_tmp715=te;unsigned _tmp714=loc;void**_tmp713=topt;Cyc_Tcexp_expr_err(_tmp715,_tmp714,_tmp713,_tag_fat(_tmp15E,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 713
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Kinds_tmk))
({void*_tmp161=0U;({unsigned _tmp717=e->loc;struct _fat_ptr _tmp716=({const char*_tmp162="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp162,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp717,_tmp716,_tag_fat(_tmp161,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp163=0U;({unsigned _tmp719=e->loc;struct _fat_ptr _tmp718=({const char*_tmp164="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp164,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp719,_tmp718,_tag_fat(_tmp163,sizeof(void*),0U));});});}else{
# 722
({struct Cyc_Warn_String_Warn_Warg_struct _tmp166=({struct Cyc_Warn_String_Warn_Warg_struct _tmp590;_tmp590.tag=0U,({struct _fat_ptr _tmp71A=({const char*_tmp168="expecting arithmetic or ? type but found ";_tag_fat(_tmp168,sizeof(char),42U);});_tmp590.f1=_tmp71A;});_tmp590;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp167=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58F;_tmp58F.tag=2U,_tmp58F.f1=(void*)t;_tmp58F;});void*_tmp165[2U];_tmp165[0]=& _tmp166,_tmp165[1]=& _tmp167;({unsigned _tmp71B=e->loc;Cyc_Warn_err2(_tmp71B,_tag_fat(_tmp165,sizeof(void*),2U));});});}}
# 724
return t;}}
# 728
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 730
({struct Cyc_Tcenv_Tenv*_tmp71D=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp71C=e1;Cyc_Tcexp_tcTest(_tmp71D,_tmp71C,({const char*_tmp169="conditional expression";_tag_fat(_tmp169,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*_tmp16A=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_tako:& Cyc_Kinds_tmko;struct Cyc_Core_Opt*ko=_tmp16A;
void*_tmp16B=({struct Cyc_Core_Opt*_tmp71E=ko;Cyc_Absyn_new_evar(_tmp71E,Cyc_Tcenv_lookup_opt_type_vars(te));});void*t=_tmp16B;
struct Cyc_List_List _tmp16C=({struct Cyc_List_List _tmp596;_tmp596.hd=e3,_tmp596.tl=0;_tmp596;});struct Cyc_List_List l1=_tmp16C;
struct Cyc_List_List _tmp16D=({struct Cyc_List_List _tmp595;_tmp595.hd=e2,_tmp595.tl=& l1;_tmp595;});struct Cyc_List_List l2=_tmp16D;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp71F=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp71F,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp594;_tmp594.tag=0U,({struct _fat_ptr _tmp720=({const char*_tmp174="conditional clause types do not match: ";_tag_fat(_tmp174,sizeof(char),40U);});_tmp594.f1=_tmp720;});_tmp594;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp170=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp593;_tmp593.tag=3U,_tmp593.f1=(void*)e2->topt;_tmp593;});struct Cyc_Warn_String_Warn_Warg_struct _tmp171=({struct Cyc_Warn_String_Warn_Warg_struct _tmp592;_tmp592.tag=0U,({
struct _fat_ptr _tmp721=({const char*_tmp173=" != ";_tag_fat(_tmp173,sizeof(char),5U);});_tmp592.f1=_tmp721;});_tmp592;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp172=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp591;_tmp591.tag=3U,_tmp591.f1=(void*)e3->topt;_tmp591;});void*_tmp16E[4U];_tmp16E[0]=& _tmp16F,_tmp16E[1]=& _tmp170,_tmp16E[2]=& _tmp171,_tmp16E[3]=& _tmp172;({unsigned _tmp722=loc;Cyc_Tcexp_err_and_explain(_tmp722,_tag_fat(_tmp16E,sizeof(void*),4U));});});
return t;}}
# 744
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 746
({struct Cyc_Tcenv_Tenv*_tmp724=te;struct Cyc_Absyn_Exp*_tmp723=e1;Cyc_Tcexp_tcTest(_tmp724,_tmp723,({const char*_tmp175="logical-and expression";_tag_fat(_tmp175,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp726=te;struct Cyc_Absyn_Exp*_tmp725=e2;Cyc_Tcexp_tcTest(_tmp726,_tmp725,({const char*_tmp176="logical-and expression";_tag_fat(_tmp176,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 750
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 752
({struct Cyc_Tcenv_Tenv*_tmp728=te;struct Cyc_Absyn_Exp*_tmp727=e1;Cyc_Tcexp_tcTest(_tmp728,_tmp727,({const char*_tmp177="logical-or expression";_tag_fat(_tmp177,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp72A=te;struct Cyc_Absyn_Exp*_tmp729=e2;Cyc_Tcexp_tcTest(_tmp72A,_tmp729,({const char*_tmp178="logical-or expression";_tag_fat(_tmp178,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 758
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 765
({struct Cyc_Tcenv_Tenv*_tmp72B=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp72B,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 770
{void*_tmp179=Cyc_Absyn_compress(t1);void*_stmttmp17=_tmp179;void*_tmp17A=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17A)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp597;_tmp597.tag=0U,({struct _fat_ptr _tmp72C=({const char*_tmp17D="cannot assign to an array";_tag_fat(_tmp17D,sizeof(char),26U);});_tmp597.f1=_tmp72C;});_tmp597;});void*_tmp17B[1U];_tmp17B[0]=& _tmp17C;({unsigned _tmp72D=loc;Cyc_Warn_err2(_tmp72D,_tag_fat(_tmp17B,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 775
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp598;_tmp598.tag=0U,({struct _fat_ptr _tmp72E=({const char*_tmp180="type is abstract (can't determine size)";_tag_fat(_tmp180,sizeof(char),40U);});_tmp598.f1=_tmp72E;});_tmp598;});void*_tmp17E[1U];_tmp17E[0]=& _tmp17F;({unsigned _tmp72F=loc;Cyc_Warn_err2(_tmp72F,_tag_fat(_tmp17E,sizeof(void*),1U));});});
# 779
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp599;_tmp599.tag=0U,({struct _fat_ptr _tmp730=({const char*_tmp183="assignment to non-lvalue";_tag_fat(_tmp183,sizeof(char),25U);});_tmp599.f1=_tmp730;});_tmp599;});void*_tmp181[1U];_tmp181[0]=& _tmp182;({struct Cyc_Tcenv_Tenv*_tmp733=te;unsigned _tmp732=loc;void**_tmp731=topt;Cyc_Tcexp_expr_err(_tmp733,_tmp732,_tmp731,_tag_fat(_tmp181,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
Cyc_Tcexp_check_consume(e2->loc,t2,e2);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp735=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp734=e2;Cyc_Tcutil_coerce_assign(_tmp735,_tmp734,t1);})){
void*_tmp184=({struct Cyc_Warn_String_Warn_Warg_struct _tmp186=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59D;_tmp59D.tag=0U,({struct _fat_ptr _tmp736=({const char*_tmp18B="type mismatch: ";_tag_fat(_tmp18B,sizeof(char),16U);});_tmp59D.f1=_tmp736;});_tmp59D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp187=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59C;_tmp59C.tag=2U,_tmp59C.f1=(void*)t1;_tmp59C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp188=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59B;_tmp59B.tag=0U,({struct _fat_ptr _tmp737=({const char*_tmp18A=" != ";_tag_fat(_tmp18A,sizeof(char),5U);});_tmp59B.f1=_tmp737;});_tmp59B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp189=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59A;_tmp59A.tag=2U,_tmp59A.f1=(void*)t2;_tmp59A;});void*_tmp185[4U];_tmp185[0]=& _tmp186,_tmp185[1]=& _tmp187,_tmp185[2]=& _tmp188,_tmp185[3]=& _tmp189;({struct Cyc_Tcenv_Tenv*_tmp73A=te;unsigned _tmp739=loc;void**_tmp738=topt;Cyc_Tcexp_expr_err(_tmp73A,_tmp739,_tmp738,_tag_fat(_tmp185,sizeof(void*),4U));});});void*result=_tmp184;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}
# 790
return t1;}{
# 792
struct Cyc_Absyn_Exp*_tmp18C=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp18C;
void*_tmp18D=Cyc_Tcexp_tcBinPrimop(te,loc,0,(enum Cyc_Absyn_Primop)po->v,e1copy,e2);void*t_result=_tmp18D;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
return t1;
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp18E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp190=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A2;_tmp5A2.tag=0U,({
struct _fat_ptr _tmp73B=({const char*_tmp197="Cannot use this operator in an assignment when ";_tag_fat(_tmp197,sizeof(char),48U);});_tmp5A2.f1=_tmp73B;});_tmp5A2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp191=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A1;_tmp5A1.tag=0U,({
struct _fat_ptr _tmp73C=({const char*_tmp196="the arguments have types ";_tag_fat(_tmp196,sizeof(char),26U);});_tmp5A1.f1=_tmp73C;});_tmp5A1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp192=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=2U,_tmp5A0.f1=(void*)t1;_tmp5A0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp193=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59F;_tmp59F.tag=0U,({struct _fat_ptr _tmp73D=({const char*_tmp195=" and ";_tag_fat(_tmp195,sizeof(char),6U);});_tmp59F.f1=_tmp73D;});_tmp59F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp194=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59E;_tmp59E.tag=2U,_tmp59E.f1=(void*)t2;_tmp59E;});void*_tmp18F[5U];_tmp18F[0]=& _tmp190,_tmp18F[1]=& _tmp191,_tmp18F[2]=& _tmp192,_tmp18F[3]=& _tmp193,_tmp18F[4]=& _tmp194;({struct Cyc_Tcenv_Tenv*_tmp740=te;unsigned _tmp73F=loc;void**_tmp73E=topt;Cyc_Tcexp_expr_err(_tmp740,_tmp73F,_tmp73E,_tag_fat(_tmp18F,sizeof(void*),5U));});});
# 797
void*result=_tmp18E;
# 800
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}
# 804
return t_result;}}}}
# 808
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp741=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp741,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp743=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp742=topt;Cyc_Tcexp_tcExp(_tmp743,_tmp742,e2);});
return(void*)_check_null(e2->topt);}
# 815
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 818
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 821
{void*_tmp198=Cyc_Absyn_compress(t1);void*_stmttmp18=_tmp198;void*_tmp199=_stmttmp18;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f1)->f1 == 0){_LL1: _LL2:
# 823
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
# 826
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 830
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp19A=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp19A;struct Cyc_Absyn_Datatypefield _tmp19B=_stmttmp19;enum Cyc_Absyn_Scope _tmp19F;unsigned _tmp19E;struct Cyc_List_List*_tmp19D;struct _tuple1*_tmp19C;_LLA: _tmp19C=_tmp19B.name;_tmp19D=_tmp19B.typs;_tmp19E=_tmp19B.loc;_tmp19F=_tmp19B.sc;_LLB: {struct _tuple1*n=_tmp19C;struct Cyc_List_List*typs=_tmp19D;unsigned loc=_tmp19E;enum Cyc_Absyn_Scope sc=_tmp19F;
# 833
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 836
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 840
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp1A0=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp1A0;struct Cyc_Absyn_Datatypefield _tmp1A1=_stmttmp1A;enum Cyc_Absyn_Scope _tmp1A5;unsigned _tmp1A4;struct Cyc_List_List*_tmp1A3;struct _tuple1*_tmp1A2;_LLD: _tmp1A2=_tmp1A1.name;_tmp1A3=_tmp1A1.typs;_tmp1A4=_tmp1A1.loc;_tmp1A5=_tmp1A1.sc;_LLE: {struct _tuple1*n=_tmp1A2;struct Cyc_List_List*typs=_tmp1A3;unsigned loc=_tmp1A4;enum Cyc_Absyn_Scope sc=_tmp1A5;
# 843
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 847
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp745=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp744=e;Cyc_Tcutil_coerce_arg(_tmp745,_tmp744,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 851
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=0U,({struct _fat_ptr _tmp746=({const char*_tmp1AC="can't find a field in ";_tag_fat(_tmp1AC,sizeof(char),23U);});_tmp5A6.f1=_tmp746;});_tmp5A6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=2U,_tmp5A5.f1=(void*)tu;_tmp5A5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A4;_tmp5A4.tag=0U,({struct _fat_ptr _tmp747=({const char*_tmp1AB=" to inject value of type ";_tag_fat(_tmp1AB,sizeof(char),26U);});_tmp5A4.f1=_tmp747;});_tmp5A4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1AA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A3;_tmp5A3.tag=2U,_tmp5A3.f1=(void*)t1;_tmp5A3;});void*_tmp1A6[4U];_tmp1A6[0]=& _tmp1A7,_tmp1A6[1]=& _tmp1A8,_tmp1A6[2]=& _tmp1A9,_tmp1A6[3]=& _tmp1AA;({unsigned _tmp748=e->loc;Cyc_Warn_err2(_tmp748,_tag_fat(_tmp1A6,sizeof(void*),4U));});});
return 0;}
# 856
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 862
struct Cyc_List_List*_tmp1AD=args;struct Cyc_List_List*es=_tmp1AD;
int _tmp1AE=0;int arg_cnt=_tmp1AE;
struct Cyc_Tcenv_Tenv*_tmp1AF=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp1AF;
struct Cyc_Tcenv_Tenv*_tmp1B0=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp1B0;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
# 871
void*_tmp1B1=t;void*_tmp1B7;void*_tmp1B6;void*_tmp1B5;void*_tmp1B4;struct Cyc_Absyn_Tqual _tmp1B3;void*_tmp1B2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->tag == 3U){_LL1: _tmp1B2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->f1).elt_type;_tmp1B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->f1).elt_tq;_tmp1B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->f1).ptr_atts).rgn;_tmp1B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->f1).ptr_atts).nullable;_tmp1B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->f1).ptr_atts).bounds;_tmp1B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->f1).ptr_atts).zero_term;_LL2: {void*t1=_tmp1B2;struct Cyc_Absyn_Tqual tq=_tmp1B3;void*rgn=_tmp1B4;void*x=_tmp1B5;void*b=_tmp1B6;void*zt=_tmp1B7;
# 876
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 878
Cyc_Tcutil_check_nonzero_bound(loc,b);
# 881
t1=({void*_tmp749=Cyc_Absyn_compress(t1);Cyc_CurRgn_instantiate(_tmp749,Cyc_Tcenv_curr_lifo_rgn(te));});{
void*_tmp1B8=t1;struct Cyc_Absyn_Vardecl*_tmp1C6;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_VarargInfo*_tmp1BF;int _tmp1BE;struct Cyc_List_List*_tmp1BD;void*_tmp1BC;struct Cyc_Absyn_Tqual _tmp1BB;void*_tmp1BA;struct Cyc_List_List*_tmp1B9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->tag == 5U){_LL6: _tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).tvars;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).effect;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).ret_tqual;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).ret_type;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).args;_tmp1BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).c_varargs;_tmp1BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).cyc_varargs;_tmp1C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).rgn_po;_tmp1C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).attributes;_tmp1C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).requires_clause;_tmp1C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).requires_relns;_tmp1C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).ensures_clause;_tmp1C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).ensures_relns;_tmp1C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B8)->f1).return_value;_LL7: {struct Cyc_List_List*tvars=_tmp1B9;void*eff=_tmp1BA;struct Cyc_Absyn_Tqual res_tq=_tmp1BB;void*res_typ=_tmp1BC;struct Cyc_List_List*args_info=_tmp1BD;int c_vararg=_tmp1BE;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp1BF;struct Cyc_List_List*rgn_po=_tmp1C0;struct Cyc_List_List*atts=_tmp1C1;struct Cyc_Absyn_Exp*req=_tmp1C2;struct Cyc_List_List*req_relns=_tmp1C3;struct Cyc_Absyn_Exp*ens=_tmp1C4;struct Cyc_List_List*ens_relns=_tmp1C5;struct Cyc_Absyn_Vardecl*ret_var=_tmp1C6;
# 886
if(tvars != 0 || rgn_po != 0)
({void*_tmp1C7=0U;({unsigned _tmp74B=e->loc;struct _fat_ptr _tmp74A=({const char*_tmp1C8="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1C8,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp74B,_tmp74A,_tag_fat(_tmp1C7,sizeof(void*),0U));});});
# 890
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 892
while(es != 0 && args_info != 0){
# 894
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp74D=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp74C=e1;Cyc_Tcutil_coerce_arg(_tmp74D,_tmp74C,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1DA="actual argument has type ";_tag_fat(_tmp1DA,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1D9=" but formal has type ";_tag_fat(_tmp1D9,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp750=({unsigned long _tmp74F=({unsigned long _tmp74E=Cyc_strlen((struct _fat_ptr)s0);_tmp74E + Cyc_strlen((struct _fat_ptr)s1);});_tmp74F + Cyc_strlen((struct _fat_ptr)s2);});_tmp750 + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AD;_tmp5AD.tag=0U,_tmp5AD.f1=s0;_tmp5AD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AC;_tmp5AC.tag=0U,({struct _fat_ptr _tmp751=({const char*_tmp1D3="\n\t";_tag_fat(_tmp1D3,sizeof(char),3U);});_tmp5AC.f1=_tmp751;});_tmp5AC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AB;_tmp5AB.tag=0U,_tmp5AB.f1=s1;_tmp5AB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=0U,({struct _fat_ptr _tmp752=({const char*_tmp1D2="\n";_tag_fat(_tmp1D2,sizeof(char),2U);});_tmp5AA.f1=_tmp752;});_tmp5AA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=0U,_tmp5A9.f1=s2;_tmp5A9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=0U,({struct _fat_ptr _tmp753=({const char*_tmp1D1="\n\t";_tag_fat(_tmp1D1,sizeof(char),3U);});_tmp5A8.f1=_tmp753;});_tmp5A8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=0U,_tmp5A7.f1=s3;_tmp5A7;});void*_tmp1C9[7U];_tmp1C9[0]=& _tmp1CA,_tmp1C9[1]=& _tmp1CB,_tmp1C9[2]=& _tmp1CC,_tmp1C9[3]=& _tmp1CD,_tmp1C9[4]=& _tmp1CE,_tmp1C9[5]=& _tmp1CF,_tmp1C9[6]=& _tmp1D0;({unsigned _tmp754=e1->loc;Cyc_Warn_err2(_tmp754,_tag_fat(_tmp1C9,sizeof(void*),7U));});});else{
# 906
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B1;_tmp5B1.tag=0U,_tmp5B1.f1=s0;_tmp5B1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B0;_tmp5B0.tag=0U,_tmp5B0.f1=s1;_tmp5B0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AF;_tmp5AF.tag=0U,_tmp5AF.f1=s2;_tmp5AF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AE;_tmp5AE.tag=0U,_tmp5AE.f1=s3;_tmp5AE;});void*_tmp1D4[4U];_tmp1D4[0]=& _tmp1D5,_tmp1D4[1]=& _tmp1D6,_tmp1D4[2]=& _tmp1D7,_tmp1D4[3]=& _tmp1D8;({unsigned _tmp755=e1->loc;Cyc_Warn_err2(_tmp755,_tag_fat(_tmp1D4,sizeof(void*),4U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 911
if(alias_coercion)
({struct Cyc_List_List*_tmp756=({struct Cyc_List_List*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->hd=(void*)arg_cnt,_tmp1DB->tl=*alias_arg_exps;_tmp1DB;});*alias_arg_exps=_tmp756;});
Cyc_Tcexp_check_consume(e1->loc,t2,e1);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 921
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1DC=(void*)a->hd;void*_stmttmp1B=_tmp1DC;void*_tmp1DD=_stmttmp1B;int _tmp1E0;int _tmp1DF;enum Cyc_Absyn_Format_Type _tmp1DE;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1DD)->tag == 19U){_LLB: _tmp1DE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1DD)->f1;_tmp1DF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1DD)->f2;_tmp1E0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1DD)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp1DE;int fmt_arg_pos=_tmp1DF;int arg_start_pos=_tmp1E0;
# 925
{struct _handler_cons _tmp1E1;_push_handler(& _tmp1E1);{int _tmp1E3=0;if(setjmp(_tmp1E1.handler))_tmp1E3=1;if(!_tmp1E3){
# 927
{struct Cyc_Absyn_Exp*_tmp1E4=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1E4;
# 929
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 933
fmt_args=({struct Cyc_Core_Opt*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));({struct Cyc_List_List*_tmp757=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp1E5->v=_tmp757;});_tmp1E5;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1E6=ft;switch(_tmp1E6){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 937
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 940
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 943
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 946
goto _LLF;}_LLF:;}}
# 927
;_pop_handler();}else{void*_tmp1E2=(void*)Cyc_Core_get_exn_thrown();void*_tmp1E7=_tmp1E2;void*_tmp1E8;if(((struct Cyc_List_Nth_exn_struct*)_tmp1E7)->tag == Cyc_List_Nth){_LL17: _LL18:
# 948
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B2;_tmp5B2.tag=0U,({struct _fat_ptr _tmp758=({const char*_tmp1EB="bad format arguments";_tag_fat(_tmp1EB,sizeof(char),21U);});_tmp5B2.f1=_tmp758;});_tmp5B2;});void*_tmp1E9[1U];_tmp1E9[0]=& _tmp1EA;({unsigned _tmp759=loc;Cyc_Warn_err2(_tmp759,_tag_fat(_tmp1E9,sizeof(void*),1U));});});goto _LL16;}else{_LL19: _tmp1E8=_tmp1E7;_LL1A: {void*exn=_tmp1E8;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 953
if(args_info != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B3;_tmp5B3.tag=0U,({struct _fat_ptr _tmp75A=({const char*_tmp1EE="too few arguments for function";_tag_fat(_tmp1EE,sizeof(char),31U);});_tmp5B3.f1=_tmp75A;});_tmp5B3;});void*_tmp1EC[1U];_tmp1EC[0]=& _tmp1ED;({unsigned _tmp75B=loc;Cyc_Warn_err2(_tmp75B,_tag_fat(_tmp1EC,sizeof(void*),1U));});});else{
# 956
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B4;_tmp5B4.tag=0U,({struct _fat_ptr _tmp75C=({const char*_tmp1F1="too many arguments for function";_tag_fat(_tmp1F1,sizeof(char),32U);});_tmp5B4.f1=_tmp75C;});_tmp5B4;});void*_tmp1EF[1U];_tmp1EF[0]=& _tmp1F0;({unsigned _tmp75D=loc;Cyc_Warn_err2(_tmp75D,_tag_fat(_tmp1EF,sizeof(void*),1U));});});else{
# 963
struct Cyc_Absyn_VarargInfo _tmp1F2=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1C=_tmp1F2;struct Cyc_Absyn_VarargInfo _tmp1F3=_stmttmp1C;int _tmp1F5;void*_tmp1F4;_LL1C: _tmp1F4=_tmp1F3.type;_tmp1F5=_tmp1F3.inject;_LL1D: {void*vt=_tmp1F4;int inject=_tmp1F5;
struct Cyc_Absyn_VarargCallInfo*_tmp1F6=({struct Cyc_Absyn_VarargCallInfo*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->num_varargs=0,_tmp20E->injectors=0,_tmp20E->vai=cyc_vararg;_tmp20E;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1F6;
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
if(!({struct Cyc_RgnOrder_RgnPO*_tmp75F=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp75E=e1;Cyc_Tcutil_coerce_arg(_tmp75F,_tmp75E,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B8;_tmp5B8.tag=0U,({struct _fat_ptr _tmp760=({const char*_tmp1FD="vararg requires type ";_tag_fat(_tmp1FD,sizeof(char),22U);});_tmp5B8.f1=_tmp760;});_tmp5B8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B7;_tmp5B7.tag=2U,_tmp5B7.f1=(void*)vt;_tmp5B7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B6;_tmp5B6.tag=0U,({
struct _fat_ptr _tmp761=({const char*_tmp1FC=" but argument has type ";_tag_fat(_tmp1FC,sizeof(char),24U);});_tmp5B6.f1=_tmp761;});_tmp5B6;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1FB=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5B5;_tmp5B5.tag=3U,_tmp5B5.f1=(void*)e1->topt;_tmp5B5;});void*_tmp1F7[4U];_tmp1F7[0]=& _tmp1F8,_tmp1F7[1]=& _tmp1F9,_tmp1F7[2]=& _tmp1FA,_tmp1F7[3]=& _tmp1FB;({unsigned _tmp762=loc;Cyc_Tcexp_err_and_explain(_tmp762,_tag_fat(_tmp1F7,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp763=({struct Cyc_List_List*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->hd=(void*)arg_cnt,_tmp1FE->tl=*alias_arg_exps;_tmp1FE;});*alias_arg_exps=_tmp763;});
Cyc_Tcexp_check_consume(e1->loc,vt,e1);}else{
# 986
void*_tmp1FF=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1D=_tmp1FF;void*_tmp200=_stmttmp1D;struct Cyc_List_List*_tmp202;struct Cyc_Absyn_Datatypedecl*_tmp201;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp200)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp200)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp200)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp201=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp200)->f1)->f1).KnownDatatype).val;_tmp202=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp200)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp201;struct Cyc_List_List*targs=_tmp202;
# 990
struct Cyc_Absyn_Datatypedecl*_tmp203=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp203;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp205=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BA;_tmp5BA.tag=0U,({struct _fat_ptr _tmp764=({const char*_tmp207="can't inject into abstract ";_tag_fat(_tmp207,sizeof(char),28U);});_tmp5BA.f1=_tmp764;});_tmp5BA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp206=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B9;_tmp5B9.tag=2U,_tmp5B9.f1=(void*)vt;_tmp5B9;});void*_tmp204[2U];_tmp204[0]=& _tmp205,_tmp204[1]=& _tmp206;({unsigned _tmp765=loc;Cyc_Warn_err2(_tmp765,_tag_fat(_tmp204,sizeof(void*),2U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 1000
({void*_tmp766=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp766,Cyc_Tcenv_curr_rgn(te));});{
# 1002
struct Cyc_List_List*_tmp208=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp208;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1007
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_check_consume(e1->loc,(void*)_check_null(e1->topt),e1);}{
# 1011
struct Cyc_Absyn_Datatypefield*_tmp209=({struct Cyc_Tcenv_Tenv*_tmp76A=te;struct Cyc_Absyn_Exp*_tmp769=e1;void*_tmp768=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp767=inst;Cyc_Tcexp_tcInjection(_tmp76A,_tmp769,_tmp768,_tmp767,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp209;
if(f != 0)
({struct Cyc_List_List*_tmp76C=({
struct Cyc_List_List*_tmp76B=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp76B,({struct Cyc_List_List*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->hd=f,_tmp20A->tl=0;_tmp20A;}));});
# 1013
vci->injectors=_tmp76C;});}}}
# 1016
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BB;_tmp5BB.tag=0U,({struct _fat_ptr _tmp76D=({const char*_tmp20D="bad inject vararg type";_tag_fat(_tmp20D,sizeof(char),23U);});_tmp5BB.f1=_tmp76D;});_tmp5BB;});void*_tmp20B[1U];_tmp20B[0]=& _tmp20C;({unsigned _tmp76E=loc;Cyc_Warn_err2(_tmp76E,_tag_fat(_tmp20B,sizeof(void*),1U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1022
if(*alias_arg_exps == 0)
# 1031 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1033
return res_typ;}}}else{_LL8: _LL9:
# 1035
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp210=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BD;_tmp5BD.tag=0U,({struct _fat_ptr _tmp76F=({const char*_tmp212="expected pointer to function but found ";_tag_fat(_tmp212,sizeof(char),40U);});_tmp5BD.f1=_tmp76F;});_tmp5BD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp211=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BC;_tmp5BC.tag=2U,_tmp5BC.f1=(void*)t;_tmp5BC;});void*_tmp20F[2U];_tmp20F[0]=& _tmp210,_tmp20F[1]=& _tmp211;({struct Cyc_Tcenv_Tenv*_tmp772=te;unsigned _tmp771=loc;void**_tmp770=topt;Cyc_Tcexp_expr_err(_tmp772,_tmp771,_tmp770,_tag_fat(_tmp20F,sizeof(void*),2U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1038
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp214=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BF;_tmp5BF.tag=0U,({struct _fat_ptr _tmp773=({const char*_tmp216="expected pointer to function but found ";_tag_fat(_tmp216,sizeof(char),40U);});_tmp5BF.f1=_tmp773;});_tmp5BF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp215=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BE;_tmp5BE.tag=2U,_tmp5BE.f1=(void*)t;_tmp5BE;});void*_tmp213[2U];_tmp213[0]=& _tmp214,_tmp213[1]=& _tmp215;({struct Cyc_Tcenv_Tenv*_tmp776=te;unsigned _tmp775=loc;void**_tmp774=topt;Cyc_Tcexp_expr_err(_tmp776,_tmp775,_tmp774,_tag_fat(_tmp213,sizeof(void*),2U));});});}_LL0:;}}}
# 1042
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp217=Cyc_Absyn_exn_type();void*exception_type=_tmp217;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp778=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp777=e;Cyc_Tcutil_coerce_arg(_tmp778,_tmp777,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp219=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3;_tmp5C3.tag=0U,({struct _fat_ptr _tmp779=({const char*_tmp21E="expected ";_tag_fat(_tmp21E,sizeof(char),10U);});_tmp5C3.f1=_tmp779;});_tmp5C3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C2;_tmp5C2.tag=2U,_tmp5C2.f1=(void*)exception_type;_tmp5C2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp21B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C1;_tmp5C1.tag=0U,({struct _fat_ptr _tmp77A=({const char*_tmp21D=" but found ";_tag_fat(_tmp21D,sizeof(char),12U);});_tmp5C1.f1=_tmp77A;});_tmp5C1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp21C=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5C0;_tmp5C0.tag=3U,_tmp5C0.f1=(void*)e->topt;_tmp5C0;});void*_tmp218[4U];_tmp218[0]=& _tmp219,_tmp218[1]=& _tmp21A,_tmp218[2]=& _tmp21B,_tmp218[3]=& _tmp21C;({unsigned _tmp77B=loc;Cyc_Warn_err2(_tmp77B,_tag_fat(_tmp218,sizeof(void*),4U));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1053
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1055
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
{void*_tmp21F=t1;struct Cyc_Absyn_PtrAtts _tmp222;struct Cyc_Absyn_Tqual _tmp221;void*_tmp220;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21F)->tag == 3U){_LL1: _tmp220=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21F)->f1).elt_type;_tmp221=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21F)->f1).elt_tq;_tmp222=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21F)->f1).ptr_atts;_LL2: {void*t=_tmp220;struct Cyc_Absyn_Tqual tq=_tmp221;struct Cyc_Absyn_PtrAtts atts=_tmp222;
# 1058
{void*_tmp223=Cyc_Absyn_compress(t);void*_stmttmp1E=_tmp223;void*_tmp224=_stmttmp1E;struct Cyc_Absyn_FnInfo _tmp225;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp224)->tag == 5U){_LL6: _tmp225=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp224)->f1;_LL7: {struct Cyc_Absyn_FnInfo info=_tmp225;
# 1060
struct Cyc_List_List*_tmp226=info.tvars;struct Cyc_List_List*tvars=_tmp226;
struct Cyc_List_List*instantiation=0;
# 1063
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Kinds_bk);
({unsigned _tmp77F=loc;struct Cyc_Tcenv_Tenv*_tmp77E=te;struct Cyc_List_List*_tmp77D=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp77C=k;Cyc_Tctyp_check_type(_tmp77F,_tmp77E,_tmp77D,_tmp77C,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp228=_cycalloc(sizeof(*_tmp228));({struct _tuple14*_tmp780=({struct _tuple14*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp227->f2=(void*)ts->hd;_tmp227;});_tmp228->hd=_tmp780;}),_tmp228->tl=instantiation;_tmp228;});}
# 1069
info.tvars=tvars;
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp22A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C4;_tmp5C4.tag=0U,({struct _fat_ptr _tmp781=({const char*_tmp22B="too many type variables in instantiation";_tag_fat(_tmp22B,sizeof(char),41U);});_tmp5C4.f1=_tmp781;});_tmp5C4;});void*_tmp229[1U];_tmp229[0]=& _tmp22A;({struct Cyc_Tcenv_Tenv*_tmp784=te;unsigned _tmp783=loc;void**_tmp782=topt;Cyc_Tcexp_expr_err(_tmp784,_tmp783,_tmp782,_tag_fat(_tmp229,sizeof(void*),1U));});});
# 1075
if(tvars == 0){
({struct Cyc_List_List*_tmp785=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,info.rgn_po);info.rgn_po=_tmp785;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,info.rgn_po);
info.rgn_po=0;}{
# 1080
void*new_fn_typ=({struct Cyc_List_List*_tmp786=instantiation;Cyc_Tcutil_substitute(_tmp786,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->tag=5U,_tmp22C->f1=info;_tmp22C;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp5C5;_tmp5C5.elt_type=new_fn_typ,_tmp5C5.elt_tq=tq,_tmp5C5.ptr_atts=atts;_tmp5C5;}));}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1084
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1087
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp22E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C7;_tmp5C7.tag=0U,({struct _fat_ptr _tmp787=({const char*_tmp230="expecting polymorphic type but found ";_tag_fat(_tmp230,sizeof(char),38U);});_tmp5C7.f1=_tmp787;});_tmp5C7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp22F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C6;_tmp5C6.tag=2U,_tmp5C6.f1=(void*)t1;_tmp5C6;});void*_tmp22D[2U];_tmp22D[0]=& _tmp22E,_tmp22D[1]=& _tmp22F;({struct Cyc_Tcenv_Tenv*_tmp78A=te;unsigned _tmp789=loc;void**_tmp788=topt;Cyc_Tcexp_expr_err(_tmp78A,_tmp789,_tmp788,_tag_fat(_tmp22D,sizeof(void*),2U));});});}
# 1091
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1093
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1095
({void*_tmp78B=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp78B;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1100
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1102
({unsigned _tmp78F=loc;struct Cyc_Tcenv_Tenv*_tmp78E=te;struct Cyc_List_List*_tmp78D=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp78C=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_tak:& Cyc_Kinds_tmk;
# 1102
Cyc_Tctyp_check_type(_tmp78F,_tmp78E,_tmp78D,_tmp78C,1,0,t);});
# 1104
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp792=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp791=loc;void*_tmp790=t2;Cyc_Tcutil_silent_castable(_tmp792,_tmp791,_tmp790,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1110
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp795=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp794=loc;void*_tmp793=t2;Cyc_Tcutil_castable(_tmp795,_tmp794,_tmp793,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1117
Cyc_Unify_unify(t2,t);{
void*_tmp231=({struct Cyc_Warn_String_Warn_Warg_struct _tmp233=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=0U,({struct _fat_ptr _tmp796=({const char*_tmp238="cannot cast ";_tag_fat(_tmp238,sizeof(char),13U);});_tmp5CB.f1=_tmp796;});_tmp5CB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp234=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=2U,_tmp5CA.f1=(void*)t2;_tmp5CA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp235=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=0U,({struct _fat_ptr _tmp797=({const char*_tmp237=" to ";_tag_fat(_tmp237,sizeof(char),5U);});_tmp5C9.f1=_tmp797;});_tmp5C9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp236=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C8;_tmp5C8.tag=2U,_tmp5C8.f1=(void*)t;_tmp5C8;});void*_tmp232[4U];_tmp232[0]=& _tmp233,_tmp232[1]=& _tmp234,_tmp232[2]=& _tmp235,_tmp232[3]=& _tmp236;({struct Cyc_Tcenv_Tenv*_tmp799=te;unsigned _tmp798=loc;Cyc_Tcexp_expr_err(_tmp799,_tmp798,& t,_tag_fat(_tmp232,sizeof(void*),4U));});});void*result=_tmp231;
Cyc_Unify_explain_failure();
return result;}}}}{
# 1126
struct _tuple0 _tmp239=({struct _tuple0 _tmp5CC;_tmp5CC.f1=e->r,({void*_tmp79A=Cyc_Absyn_compress(t);_tmp5CC.f2=_tmp79A;});_tmp5CC;});struct _tuple0 _stmttmp1F=_tmp239;struct _tuple0 _tmp23A=_stmttmp1F;void*_tmp23F;void*_tmp23E;void*_tmp23D;void*_tmp23C;int _tmp23B;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp23A.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A.f2)->tag == 3U){_LL1: _tmp23B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp23A.f1)->f1).fat_result;_tmp23C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A.f2)->f1).ptr_atts).nullable;_tmp23D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A.f2)->f1).ptr_atts).bounds;_tmp23E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A.f2)->f1).ptr_atts).zero_term;_tmp23F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A.f2)->f1).ptr_atts).released;_LL2: {int fat_result=_tmp23B;void*nbl=_tmp23C;void*bds=_tmp23D;void*zt=_tmp23E;void*rel=_tmp23F;
# 1130
if(((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl))&& !
Cyc_Tcutil_force_type2bool(0,rel)){
struct Cyc_Absyn_Exp*_tmp240=({void*_tmp79B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp79B,bds);});struct Cyc_Absyn_Exp*eopt=_tmp240;
if(eopt != 0){
if((Cyc_Evexp_eval_const_uint_exp(e)).f1 == (unsigned)1)
({void*_tmp241=0U;({unsigned _tmp79D=loc;struct _fat_ptr _tmp79C=({const char*_tmp242="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp242,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp79D,_tmp79C,_tag_fat(_tmp241,sizeof(void*),0U));});});}}
# 1137
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;}_LL0:;}}}
# 1143
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp243=0;void**topt2=_tmp243;
struct Cyc_Absyn_Tqual _tmp244=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp244;
int _tmp245=0;int nullable=_tmp245;
if(topt != 0){
void*_tmp246=Cyc_Absyn_compress(*topt);void*_stmttmp20=_tmp246;void*_tmp247=_stmttmp20;void*_tmp24A;struct Cyc_Absyn_Tqual _tmp249;void**_tmp248;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp247)->tag == 3U){_LL1: _tmp248=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp247)->f1).elt_type;_tmp249=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp247)->f1).elt_tq;_tmp24A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp247)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp248;struct Cyc_Absyn_Tqual tq=_tmp249;void*n=_tmp24A;
# 1150
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1164
({struct Cyc_Tcenv_Tenv*_tmp79F=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp79E=topt2;Cyc_Tcexp_tcExpNoInst(_tmp79F,_tmp79E,e);});
# 1166
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CD;_tmp5CD.tag=0U,({struct _fat_ptr _tmp7A0=({const char*_tmp24D="cannot take the address of an alias-free path";_tag_fat(_tmp24D,sizeof(char),46U);});_tmp5CD.f1=_tmp7A0;});_tmp5CD;});void*_tmp24B[1U];_tmp24B[0]=& _tmp24C;({unsigned _tmp7A1=e->loc;Cyc_Warn_err2(_tmp7A1,_tag_fat(_tmp24B,sizeof(void*),1U));});});
# 1171
{void*_tmp24E=e->r;void*_stmttmp21=_tmp24E;void*_tmp24F=_stmttmp21;struct Cyc_Absyn_Exp*_tmp251;struct Cyc_Absyn_Exp*_tmp250;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24F)->tag == 23U){_LL6: _tmp250=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24F)->f1;_tmp251=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24F)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp250;struct Cyc_Absyn_Exp*e2=_tmp251;
# 1173
{void*_tmp252=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp22=_tmp252;void*_tmp253=_stmttmp22;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp253)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1178
({void*_tmp7A2=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp7A2;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1181
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1186
{void*_tmp254=e->r;void*_stmttmp23=_tmp254;void*_tmp255=_stmttmp23;switch(*((int*)_tmp255)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp255)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp255)->f3 == 1){_LL12: _LL13:
# 1189
({struct Cyc_Warn_String_Warn_Warg_struct _tmp257=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CE;_tmp5CE.tag=0U,({struct _fat_ptr _tmp7A3=({const char*_tmp258="cannot take the address of a @tagged union member";_tag_fat(_tmp258,sizeof(char),50U);});_tmp5CE.f1=_tmp7A3;});_tmp5CE;});void*_tmp256[1U];_tmp256[0]=& _tmp257;({unsigned _tmp7A4=e->loc;Cyc_Warn_err2(_tmp7A4,_tag_fat(_tmp256,sizeof(void*),1U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1195
struct _tuple15 _tmp259=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp24=_tmp259;struct _tuple15 _tmp25A=_stmttmp24;void*_tmp25C;int _tmp25B;_LL17: _tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;_LL18: {int is_const=_tmp25B;void*rgn=_tmp25C;
# 1197
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CF;_tmp5CF.tag=0U,({struct _fat_ptr _tmp7A5=({const char*_tmp25F="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp25F,sizeof(char),60U);});_tmp5CF.f1=_tmp7A5;});_tmp5CF;});void*_tmp25D[1U];_tmp25D[0]=& _tmp25E;({unsigned _tmp7A6=e->loc;Cyc_Warn_err2(_tmp7A6,_tag_fat(_tmp25D,sizeof(void*),1U));});});{
# 1200
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _tmp7A7=tq.real_const=1;tq.print_const=_tmp7A7;});
# 1204
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}}}}
# 1208
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1211
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _tmp7AA=loc;struct Cyc_Tcenv_Tenv*_tmp7A9=te;struct Cyc_List_List*_tmp7A8=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7AA,_tmp7A9,_tmp7A8,& Cyc_Kinds_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp261=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D2;_tmp5D2.tag=0U,({struct _fat_ptr _tmp7AB=({const char*_tmp265="sizeof applied to type ";_tag_fat(_tmp265,sizeof(char),24U);});_tmp5D2.f1=_tmp7AB;});_tmp5D2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp262=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D1;_tmp5D1.tag=2U,_tmp5D1.f1=(void*)t;_tmp5D1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp263=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D0;_tmp5D0.tag=0U,({struct _fat_ptr _tmp7AC=({const char*_tmp264=" which has unknown size here";_tag_fat(_tmp264,sizeof(char),29U);});_tmp5D0.f1=_tmp7AC;});_tmp5D0;});void*_tmp260[3U];_tmp260[0]=& _tmp261,_tmp260[1]=& _tmp262,_tmp260[2]=& _tmp263;({unsigned _tmp7AD=loc;Cyc_Warn_err2(_tmp7AD,_tag_fat(_tmp260,sizeof(void*),3U));});});
if(topt == 0)
return Cyc_Absyn_uint_type;{
void*_tmp266=Cyc_Absyn_compress(*topt);void*_stmttmp25=_tmp266;void*_tmp267=_stmttmp25;void*_tmp268;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f2 != 0){_LL1: _tmp268=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f2)->hd;_LL2: {void*tt=_tmp268;
# 1221
if(({void*_tmp7AF=tt;Cyc_Unify_unify(_tmp7AF,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->tag=9U,({struct Cyc_Absyn_Exp*_tmp7AE=Cyc_Absyn_sizeoftype_exp(t,0U);_tmp269->f1=_tmp7AE;});_tmp269;}));}))
return Cyc_Absyn_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_uint_type;}_LL0:;}}
# 1228
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)
return sf->type;
return 0;}
# 1236
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1238
({unsigned _tmp7B2=loc;struct Cyc_Tcenv_Tenv*_tmp7B1=te;struct Cyc_List_List*_tmp7B0=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7B2,_tmp7B1,_tmp7B0,& Cyc_Kinds_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp26A=fs;struct Cyc_List_List*l=_tmp26A;
void*_tmp26B=t_orig;void*t=_tmp26B;
for(0;l != 0;l=l->tl){
void*_tmp26C=(void*)l->hd;void*n=_tmp26C;
void*_tmp26D=n;unsigned _tmp26E;struct _fat_ptr*_tmp26F;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp26D)->tag == 0U){_LL1: _tmp26F=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp26D)->f1;_LL2: {struct _fat_ptr*n=_tmp26F;
# 1246
int bad_type=1;
{void*_tmp270=Cyc_Absyn_compress(t);void*_stmttmp26=_tmp270;void*_tmp271=_stmttmp26;struct Cyc_List_List*_tmp272;struct Cyc_Absyn_Aggrdecl**_tmp273;switch(*((int*)_tmp271)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp271)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp271)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp273=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp271)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp273;
# 1249
if((*adp)->impl == 0)
goto _LL5;
_tmp272=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL9;}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp272=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp271)->f2;_LL9: {struct Cyc_List_List*fields=_tmp272;
# 1253
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp275=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=0U,({struct _fat_ptr _tmp7B3=({const char*_tmp277="no field of struct/union has name %s";_tag_fat(_tmp277,sizeof(char),37U);});_tmp5D4.f1=_tmp7B3;});_tmp5D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp276=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=0U,_tmp5D3.f1=*n;_tmp5D3;});void*_tmp274[2U];_tmp274[0]=& _tmp275,_tmp274[1]=& _tmp276;({unsigned _tmp7B4=loc;Cyc_Warn_err2(_tmp7B4,_tag_fat(_tmp274,sizeof(void*),2U));});});else{
# 1257
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1262
if(bad_type){
if(l == fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp279=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D6;_tmp5D6.tag=2U,_tmp5D6.f1=(void*)t;_tmp5D6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D5;_tmp5D5.tag=0U,({struct _fat_ptr _tmp7B5=({const char*_tmp27B=" is not a known struct/union type";_tag_fat(_tmp27B,sizeof(char),34U);});_tmp5D5.f1=_tmp7B5;});_tmp5D5;});void*_tmp278[2U];_tmp278[0]=& _tmp279,_tmp278[1]=& _tmp27A;({unsigned _tmp7B6=loc;Cyc_Warn_err2(_tmp7B6,_tag_fat(_tmp278,sizeof(void*),2U));});});else{
# 1266
struct _fat_ptr _tmp27C=({struct Cyc_String_pa_PrintArg_struct _tmp292=({struct Cyc_String_pa_PrintArg_struct _tmp5DF;_tmp5DF.tag=0U,({struct _fat_ptr _tmp7B7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp5DF.f1=_tmp7B7;});_tmp5DF;});void*_tmp290[1U];_tmp290[0]=& _tmp292;({struct _fat_ptr _tmp7B8=({const char*_tmp291="(%s)";_tag_fat(_tmp291,sizeof(char),5U);});Cyc_aprintf(_tmp7B8,_tag_fat(_tmp290,sizeof(void*),1U));});});struct _fat_ptr s=_tmp27C;
{struct Cyc_List_List*x=fs;for(0;x != l;x=x->tl){
void*_tmp27D=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp27=_tmp27D;void*_tmp27E=_stmttmp27;unsigned _tmp27F;struct _fat_ptr*_tmp280;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp27E)->tag == 0U){_LLD: _tmp280=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp27E)->f1;_LLE: {struct _fat_ptr*n=_tmp280;
s=({struct Cyc_String_pa_PrintArg_struct _tmp283=({struct Cyc_String_pa_PrintArg_struct _tmp5D8;_tmp5D8.tag=0U,_tmp5D8.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5D8;});struct Cyc_String_pa_PrintArg_struct _tmp284=({struct Cyc_String_pa_PrintArg_struct _tmp5D7;_tmp5D7.tag=0U,_tmp5D7.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5D7;});void*_tmp281[2U];_tmp281[0]=& _tmp283,_tmp281[1]=& _tmp284;({struct _fat_ptr _tmp7B9=({const char*_tmp282="%s.%s";_tag_fat(_tmp282,sizeof(char),6U);});Cyc_aprintf(_tmp7B9,_tag_fat(_tmp281,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp27F=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp27E)->f1;_LL10: {unsigned n=_tmp27F;
s=({struct Cyc_String_pa_PrintArg_struct _tmp287=({struct Cyc_String_pa_PrintArg_struct _tmp5DA;_tmp5DA.tag=0U,_tmp5DA.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5DA;});struct Cyc_Int_pa_PrintArg_struct _tmp288=({struct Cyc_Int_pa_PrintArg_struct _tmp5D9;_tmp5D9.tag=1U,_tmp5D9.f1=(unsigned long)((int)n);_tmp5D9;});void*_tmp285[2U];_tmp285[0]=& _tmp287,_tmp285[1]=& _tmp288;({struct _fat_ptr _tmp7BA=({const char*_tmp286="%s.%d";_tag_fat(_tmp286,sizeof(char),6U);});Cyc_aprintf(_tmp7BA,_tag_fat(_tmp285,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}}
# 1272
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DE;_tmp5DE.tag=0U,_tmp5DE.f1=(struct _fat_ptr)s;_tmp5DE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp28B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DD;_tmp5DD.tag=0U,({struct _fat_ptr _tmp7BB=({const char*_tmp28F=" == ";_tag_fat(_tmp28F,sizeof(char),5U);});_tmp5DD.f1=_tmp7BB;});_tmp5DD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=2U,_tmp5DC.f1=(void*)t;_tmp5DC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp28D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DB;_tmp5DB.tag=0U,({struct _fat_ptr _tmp7BC=({const char*_tmp28E=" is not a struct/union type";_tag_fat(_tmp28E,sizeof(char),28U);});_tmp5DB.f1=_tmp7BC;});_tmp5DB;});void*_tmp289[4U];_tmp289[0]=& _tmp28A,_tmp289[1]=& _tmp28B,_tmp289[2]=& _tmp28C,_tmp289[3]=& _tmp28D;({unsigned _tmp7BD=loc;Cyc_Warn_err2(_tmp7BD,_tag_fat(_tmp289,sizeof(void*),4U));});});}}
# 1275
goto _LL0;}}else{_LL3: _tmp26E=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp26D)->f1;_LL4: {unsigned n=_tmp26E;
# 1277
int bad_type=1;
{void*_tmp293=Cyc_Absyn_compress(t);void*_stmttmp28=_tmp293;void*_tmp294=_stmttmp28;struct Cyc_List_List*_tmp295;struct Cyc_List_List*_tmp296;struct Cyc_Absyn_Datatypefield*_tmp297;struct Cyc_Absyn_Aggrdecl**_tmp298;switch(*((int*)_tmp294)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp298=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp298;
# 1280
if((*adp)->impl == 0)
goto _LL11;
_tmp296=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp297=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp297;
# 1299
if(({unsigned _tmp7BE=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp7BE < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp2A9=({struct Cyc_Int_pa_PrintArg_struct _tmp5E1;_tmp5E1.tag=1U,({
unsigned long _tmp7BF=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp5E1.f1=_tmp7BF;});_tmp5E1;});struct Cyc_Int_pa_PrintArg_struct _tmp2AA=({struct Cyc_Int_pa_PrintArg_struct _tmp5E0;_tmp5E0.tag=1U,_tmp5E0.f1=(unsigned long)((int)n);_tmp5E0;});void*_tmp2A7[2U];_tmp2A7[0]=& _tmp2A9,_tmp2A7[1]=& _tmp2AA;({unsigned _tmp7C1=loc;struct _fat_ptr _tmp7C0=({const char*_tmp2A8="datatype field has too few components: %d < %d";_tag_fat(_tmp2A8,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp7C1,_tmp7C0,_tag_fat(_tmp2A7,sizeof(void*),2U));});});else{
# 1303
if(n != (unsigned)0)
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp2AB=0U;({unsigned _tmp7C3=loc;struct _fat_ptr _tmp7C2=({const char*_tmp2AC="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp2AC,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp7C3,_tmp7C2,_tag_fat(_tmp2AB,sizeof(void*),0U));});});}}
# 1308
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp296=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp294)->f2;_LL15: {struct Cyc_List_List*fields=_tmp296;
# 1284
if(({unsigned _tmp7C4=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp7C4 <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=0U,({struct _fat_ptr _tmp7C5=({const char*_tmp29F="struct/union has too few components: ";_tag_fat(_tmp29F,sizeof(char),38U);});_tmp5E5.f1=_tmp7C5;});_tmp5E5;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp29B=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=12U,({
int _tmp7C6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp5E4.f1=_tmp7C6;});_tmp5E4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E3;_tmp5E3.tag=0U,({struct _fat_ptr _tmp7C7=({const char*_tmp29E=" <= ";_tag_fat(_tmp29E,sizeof(char),5U);});_tmp5E3.f1=_tmp7C7;});_tmp5E3;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp29D=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5E2;_tmp5E2.tag=12U,_tmp5E2.f1=(int)n;_tmp5E2;});void*_tmp299[4U];_tmp299[0]=& _tmp29A,_tmp299[1]=& _tmp29B,_tmp299[2]=& _tmp29C,_tmp299[3]=& _tmp29D;({unsigned _tmp7C8=loc;Cyc_Warn_err2(_tmp7C8,_tag_fat(_tmp299,sizeof(void*),4U));});});else{
# 1288
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp295=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp294)->f1;_LL17: {struct Cyc_List_List*l=_tmp295;
# 1292
if(({unsigned _tmp7C9=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp7C9 <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=0U,({struct _fat_ptr _tmp7CA=({const char*_tmp2A6="tuple has too few components: ";_tag_fat(_tmp2A6,sizeof(char),31U);});_tmp5E9.f1=_tmp7CA;});_tmp5E9;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2A2=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=12U,({int _tmp7CB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp5E8.f1=_tmp7CB;});_tmp5E8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=0U,({struct _fat_ptr _tmp7CC=({const char*_tmp2A5=" <= ";_tag_fat(_tmp2A5,sizeof(char),5U);});_tmp5E7.f1=_tmp7CC;});_tmp5E7;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2A4=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=12U,_tmp5E6.f1=(int)n;_tmp5E6;});void*_tmp2A0[4U];_tmp2A0[0]=& _tmp2A1,_tmp2A0[1]=& _tmp2A2,_tmp2A0[2]=& _tmp2A3,_tmp2A0[3]=& _tmp2A4;({unsigned _tmp7CD=loc;Cyc_Warn_err2(_tmp7CD,_tag_fat(_tmp2A0,sizeof(void*),4U));});});else{
# 1295
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1310
 goto _LL11;}_LL11:;}
# 1312
if(bad_type)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2AE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=2U,_tmp5EB.f1=(void*)t;_tmp5EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=0U,({struct _fat_ptr _tmp7CE=({const char*_tmp2B0=" is not a known type";_tag_fat(_tmp2B0,sizeof(char),21U);});_tmp5EA.f1=_tmp7CE;});_tmp5EA;});void*_tmp2AD[2U];_tmp2AD[0]=& _tmp2AE,_tmp2AD[1]=& _tmp2AF;({unsigned _tmp7CF=loc;Cyc_Warn_err2(_tmp7CF,_tag_fat(_tmp2AD,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 1317
return Cyc_Absyn_uint_type;}}
# 1321
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
{void*_tmp2B1=t;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B1)->tag == 1U){_LL1: _LL2: {
# 1327
struct Cyc_Absyn_PtrInfo _tmp2B2=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp2B2;
({void*_tmp7D1=Cyc_Absyn_new_evar(& Cyc_Kinds_ako,({struct Cyc_Core_Opt*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));({struct Cyc_List_List*_tmp7D0=Cyc_Tcenv_lookup_type_vars(te);_tmp2B3->v=_tmp7D0;});_tmp2B3;}));pi.elt_type=_tmp7D1;});{
void*_tmp2B4=Cyc_Absyn_pointer_type(pi);void*new_typ=_tmp2B4;
Cyc_Unify_unify(t,new_typ);
t=Cyc_Absyn_compress(t);
goto _LL0;}}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1335
void*_tmp2B5=t;void*_tmp2B9;void*_tmp2B8;void*_tmp2B7;void*_tmp2B6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5)->tag == 3U){_LL6: _tmp2B6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5)->f1).elt_type;_tmp2B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5)->f1).ptr_atts).rgn;_tmp2B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5)->f1).ptr_atts).bounds;_tmp2B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5)->f1).ptr_atts).zero_term;_LL7: {void*t2=_tmp2B6;void*rt=_tmp2B7;void*b=_tmp2B8;void*zt=_tmp2B9;
# 1337
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Kinds_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2BA=Cyc_Absyn_compress(t2);void*_stmttmp29=_tmp2BA;void*_tmp2BB=_stmttmp29;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BB)->tag == 5U){_LLB: _LLC:
# 1342
 if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=0U,({struct _fat_ptr _tmp7D2=({const char*_tmp2BE="unnecessary dereference for function type";_tag_fat(_tmp2BE,sizeof(char),42U);});_tmp5EC.f1=_tmp7D2;});_tmp5EC;});void*_tmp2BC[1U];_tmp2BC[0]=& _tmp2BD;({unsigned _tmp7D3=loc;Cyc_Warn_warn2(_tmp7D3,_tag_fat(_tmp2BC,sizeof(void*),1U));});});
return t;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5ED;_tmp5ED.tag=0U,({struct _fat_ptr _tmp7D4=({const char*_tmp2C1="cannot dereference abstract pointer type";_tag_fat(_tmp2C1,sizeof(char),41U);});_tmp5ED.f1=_tmp7D4;});_tmp5ED;});void*_tmp2BF[1U];_tmp2BF[0]=& _tmp2C0;({unsigned _tmp7D5=loc;Cyc_Warn_err2(_tmp7D5,_tag_fat(_tmp2BF,sizeof(void*),1U));});});}_LLA:;}
# 1347
return t2;}}else{_LL8: _LL9:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=0U,({struct _fat_ptr _tmp7D6=({const char*_tmp2C5="expecting pointer type but found ";_tag_fat(_tmp2C5,sizeof(char),34U);});_tmp5EF.f1=_tmp7D6;});_tmp5EF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=2U,_tmp5EE.f1=(void*)t;_tmp5EE;});void*_tmp2C2[2U];_tmp2C2[0]=& _tmp2C3,_tmp2C2[1]=& _tmp2C4;({struct Cyc_Tcenv_Tenv*_tmp7D9=te;unsigned _tmp7D8=loc;void**_tmp7D7=topt;Cyc_Tcexp_expr_err(_tmp7D9,_tmp7D8,_tmp7D7,_tag_fat(_tmp2C2,sizeof(void*),2U));});});}_LL5:;}}}
# 1352
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1356
({int _tmp7DA=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp7DA;});{
void*_tmp2C6=Cyc_Absyn_compress(aggr_type);void*_stmttmp2A=_tmp2C6;void*_tmp2C7=_stmttmp2A;struct Cyc_List_List*_tmp2C9;enum Cyc_Absyn_AggrKind _tmp2C8;struct Cyc_List_List*_tmp2CB;struct Cyc_Absyn_Aggrdecl*_tmp2CA;switch(*((int*)_tmp2C7)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp2CA=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7)->f1)->f1).KnownAggr).val;_tmp2CB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2CA;struct Cyc_List_List*ts=_tmp2CB;
# 1359
struct Cyc_Absyn_Aggrfield*_tmp2CC=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2CC;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2CE=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=6U,_tmp5F3.f1=ad;_tmp5F3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0U,({struct _fat_ptr _tmp7DB=({const char*_tmp2D3=" has no ";_tag_fat(_tmp2D3,sizeof(char),9U);});_tmp5F2.f1=_tmp7DB;});_tmp5F2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=0U,_tmp5F1.f1=*f;_tmp5F1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0U,({struct _fat_ptr _tmp7DC=({const char*_tmp2D2=" member";_tag_fat(_tmp2D2,sizeof(char),8U);});_tmp5F0.f1=_tmp7DC;});_tmp5F0;});void*_tmp2CD[4U];_tmp2CD[0]=& _tmp2CE,_tmp2CD[1]=& _tmp2CF,_tmp2CD[2]=& _tmp2D0,_tmp2CD[3]=& _tmp2D1;({struct Cyc_Tcenv_Tenv*_tmp7DF=te;unsigned _tmp7DE=loc;void**_tmp7DD=topt;Cyc_Tcexp_expr_err(_tmp7DF,_tmp7DE,_tmp7DD,_tag_fat(_tmp2CD,sizeof(void*),4U));});});
# 1363
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp7E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp7E0,finfo->type);});{
struct Cyc_Absyn_Kind*_tmp2D4=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*t2_kind=_tmp2D4;
# 1371
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2D5=Cyc_Absyn_compress(t2);void*_stmttmp2B=_tmp2D5;void*_tmp2D6=_stmttmp2B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D6)->tag == 4U){_LL8: _LL9:
 goto _LL7;}else{_LLA: _LLB:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F6;_tmp5F6.tag=0U,({struct _fat_ptr _tmp7E1=({const char*_tmp2DC="cannot get member ";_tag_fat(_tmp2DC,sizeof(char),19U);});_tmp5F6.f1=_tmp7E1;});_tmp5F6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=0U,_tmp5F5.f1=*f;_tmp5F5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=0U,({
struct _fat_ptr _tmp7E2=({const char*_tmp2DB=" since its type is abstract";_tag_fat(_tmp2DB,sizeof(char),28U);});_tmp5F4.f1=_tmp7E2;});_tmp5F4;});void*_tmp2D7[3U];_tmp2D7[0]=& _tmp2D8,_tmp2D7[1]=& _tmp2D9,_tmp2D7[2]=& _tmp2DA;({struct Cyc_Tcenv_Tenv*_tmp7E5=te;unsigned _tmp7E4=loc;void**_tmp7E3=topt;Cyc_Tcexp_expr_err(_tmp7E5,_tmp7E4,_tmp7E3,_tag_fat(_tmp2D7,sizeof(void*),3U));});});}_LL7:;}
# 1379
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F9;_tmp5F9.tag=0U,({struct _fat_ptr _tmp7E6=({const char*_tmp2E2="cannot read union member ";_tag_fat(_tmp2E2,sizeof(char),26U);});_tmp5F9.f1=_tmp7E6;});_tmp5F9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F8;_tmp5F8.tag=0U,_tmp5F8.f1=*f;_tmp5F8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F7;_tmp5F7.tag=0U,({
struct _fat_ptr _tmp7E7=({const char*_tmp2E1=" since it is not `bits-only'";_tag_fat(_tmp2E1,sizeof(char),29U);});_tmp5F7.f1=_tmp7E7;});_tmp5F7;});void*_tmp2DD[3U];_tmp2DD[0]=& _tmp2DE,_tmp2DD[1]=& _tmp2DF,_tmp2DD[2]=& _tmp2E0;({struct Cyc_Tcenv_Tenv*_tmp7EA=te;unsigned _tmp7E9=loc;void**_tmp7E8=topt;Cyc_Tcexp_expr_err(_tmp7EA,_tmp7E9,_tmp7E8,_tag_fat(_tmp2DD,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1385
if(!({void*_tmp7EB=t2;Cyc_Unify_unify(_tmp7EB,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FC;_tmp5FC.tag=0U,({struct _fat_ptr _tmp7EC=({const char*_tmp2E8="must use pattern-matching to access field ";_tag_fat(_tmp2E8,sizeof(char),43U);});_tmp5FC.f1=_tmp7EC;});_tmp5FC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FB;_tmp5FB.tag=0U,_tmp5FB.f1=*f;_tmp5FB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=0U,({struct _fat_ptr _tmp7ED=({const char*_tmp2E7="\n\tdue to existential type variables.";_tag_fat(_tmp2E7,sizeof(char),37U);});_tmp5FA.f1=_tmp7ED;});_tmp5FA;});void*_tmp2E3[3U];_tmp2E3[0]=& _tmp2E4,_tmp2E3[1]=& _tmp2E5,_tmp2E3[2]=& _tmp2E6;({struct Cyc_Tcenv_Tenv*_tmp7F0=te;unsigned _tmp7EF=loc;void**_tmp7EE=topt;Cyc_Tcexp_expr_err(_tmp7F0,_tmp7EF,_tmp7EE,_tag_fat(_tmp2E3,sizeof(void*),3U));});});}
# 1388
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp2C8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C7)->f1;_tmp2C9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C7)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp2C8;struct Cyc_List_List*fs=_tmp2C9;
# 1390
struct Cyc_Absyn_Aggrfield*_tmp2E9=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2E9;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FF;_tmp5FF.tag=0U,({struct _fat_ptr _tmp7F1=({const char*_tmp2EF="type has no ";_tag_fat(_tmp2EF,sizeof(char),13U);});_tmp5FF.f1=_tmp7F1;});_tmp5FF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FE;_tmp5FE.tag=0U,_tmp5FE.f1=*f;_tmp5FE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FD;_tmp5FD.tag=0U,({struct _fat_ptr _tmp7F2=({const char*_tmp2EE=" member";_tag_fat(_tmp2EE,sizeof(char),8U);});_tmp5FD.f1=_tmp7F2;});_tmp5FD;});void*_tmp2EA[3U];_tmp2EA[0]=& _tmp2EB,_tmp2EA[1]=& _tmp2EC,_tmp2EA[2]=& _tmp2ED;({struct Cyc_Tcenv_Tenv*_tmp7F5=te;unsigned _tmp7F4=loc;void**_tmp7F3=topt;Cyc_Tcexp_expr_err(_tmp7F5,_tmp7F4,_tmp7F3,_tag_fat(_tmp2EA,sizeof(void*),3U));});});
# 1395
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1397
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp602;_tmp602.tag=0U,({struct _fat_ptr _tmp7F6=({const char*_tmp2F5="cannot read union member ";_tag_fat(_tmp2F5,sizeof(char),26U);});_tmp602.f1=_tmp7F6;});_tmp602;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp601;_tmp601.tag=0U,_tmp601.f1=*f;_tmp601;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp600;_tmp600.tag=0U,({struct _fat_ptr _tmp7F7=({const char*_tmp2F4=" since it is not `bits-only'";_tag_fat(_tmp2F4,sizeof(char),29U);});_tmp600.f1=_tmp7F7;});_tmp600;});void*_tmp2F0[3U];_tmp2F0[0]=& _tmp2F1,_tmp2F0[1]=& _tmp2F2,_tmp2F0[2]=& _tmp2F3;({struct Cyc_Tcenv_Tenv*_tmp7FA=te;unsigned _tmp7F9=loc;void**_tmp7F8=topt;Cyc_Tcexp_expr_err(_tmp7FA,_tmp7F9,_tmp7F8,_tag_fat(_tmp2F0,sizeof(void*),3U));});});
return finfo->type;}default: _LL5: _LL6:
# 1400
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp604;_tmp604.tag=0U,({struct _fat_ptr _tmp7FB=({const char*_tmp2F9="expecting struct or union, found ";_tag_fat(_tmp2F9,sizeof(char),34U);});_tmp604.f1=_tmp7FB;});_tmp604;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2F8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp603;_tmp603.tag=2U,_tmp603.f1=(void*)aggr_type;_tmp603;});void*_tmp2F6[2U];_tmp2F6[0]=& _tmp2F7,_tmp2F6[1]=& _tmp2F8;({struct Cyc_Tcenv_Tenv*_tmp7FE=te;unsigned _tmp7FD=loc;void**_tmp7FC=topt;Cyc_Tcexp_expr_err(_tmp7FE,_tmp7FD,_tmp7FC,_tag_fat(_tmp2F6,sizeof(void*),2U));});});}_LL0:;}}
# 1405
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1409
({struct Cyc_Tcenv_Tenv*_tmp7FF=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp7FF,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,(void*)_check_null(e->topt),f,is_tagged,is_read);}
# 1414
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1418
void*t2=({struct Cyc_Tcenv_Tenv*_tmp801=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _tmp800=loc;Cyc_Tcexp_tcDeref(_tmp801,_tmp800,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1423
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1425
struct Cyc_Tcenv_Tenv*_tmp2FA=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp2FA;
({struct Cyc_Tcenv_Tenv*_tmp802=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp802,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp803=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp803,0,e2);});{
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp606;_tmp606.tag=0U,({struct _fat_ptr _tmp804=({const char*_tmp2FE="expecting int subscript, found ";_tag_fat(_tmp2FE,sizeof(char),32U);});_tmp606.f1=_tmp804;});_tmp606;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp605;_tmp605.tag=2U,_tmp605.f1=(void*)t2;_tmp605;});void*_tmp2FB[2U];_tmp2FB[0]=& _tmp2FC,_tmp2FB[1]=& _tmp2FD;({struct Cyc_Tcenv_Tenv*_tmp807=te;unsigned _tmp806=e2->loc;void**_tmp805=topt;Cyc_Tcexp_expr_err(_tmp807,_tmp806,_tmp805,_tag_fat(_tmp2FB,sizeof(void*),2U));});});{
# 1434
void*_tmp2FF=t1;struct Cyc_List_List*_tmp300;void*_tmp305;void*_tmp304;void*_tmp303;struct Cyc_Absyn_Tqual _tmp302;void*_tmp301;switch(*((int*)_tmp2FF)){case 3U: _LL1: _tmp301=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->f1).elt_type;_tmp302=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->f1).elt_tq;_tmp303=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->f1).ptr_atts).rgn;_tmp304=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->f1).ptr_atts).bounds;_tmp305=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp301;struct Cyc_Absyn_Tqual tq=_tmp302;void*rt=_tmp303;void*b=_tmp304;void*zt=_tmp305;
# 1438
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp306=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp306;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp307=eopt;struct Cyc_Absyn_Exp*e3=_tmp307;
struct _tuple16 _tmp308=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2C=_tmp308;struct _tuple16 _tmp309=_stmttmp2C;int _tmp30B;unsigned _tmp30A;_LL8: _tmp30A=_tmp309.f1;_tmp30B=_tmp309.f2;_LL9: {unsigned i=_tmp30A;int known_i=_tmp30B;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp30C=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2D=_tmp30C;struct _tuple16 _tmp30D=_stmttmp2D;int _tmp30F;unsigned _tmp30E;_LLB: _tmp30E=_tmp30D.f1;_tmp30F=_tmp30D.f2;_LLC: {unsigned j=_tmp30E;int known_j=_tmp30F;
if(known_j){
struct _tuple16 _tmp310=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2E=_tmp310;struct _tuple16 _tmp311=_stmttmp2E;int _tmp313;unsigned _tmp312;_LLE: _tmp312=_tmp311.f1;_tmp313=_tmp311.f2;_LLF: {unsigned j=_tmp312;int knownj=_tmp313;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp314=0U;({unsigned _tmp809=loc;struct _fat_ptr _tmp808=({const char*_tmp315="subscript will fail at run-time";_tag_fat(_tmp315,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp809,_tmp808,_tag_fat(_tmp314,sizeof(void*),0U));});});}}}}}}
# 1454
if(emit_warning)
({void*_tmp316=0U;({unsigned _tmp80B=e2->loc;struct _fat_ptr _tmp80A=({const char*_tmp317="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp317,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp80B,_tmp80A,_tag_fat(_tmp316,sizeof(void*),0U));});});}else{
# 1458
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp318=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2F=_tmp318;struct _tuple16 _tmp319=_stmttmp2F;int _tmp31B;unsigned _tmp31A;_LL11: _tmp31A=_tmp319.f1;_tmp31B=_tmp319.f2;_LL12: {unsigned i=_tmp31A;int known=_tmp31B;
if(known)
# 1463
({unsigned _tmp80E=loc;unsigned _tmp80D=i;void*_tmp80C=b;Cyc_Tcutil_check_bound(_tmp80E,_tmp80D,_tmp80C,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1466
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp31C=0U;({unsigned _tmp810=e1->loc;struct _fat_ptr _tmp80F=({const char*_tmp31D="subscript applied to pointer to one object";_tag_fat(_tmp31D,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp810,_tmp80F,_tag_fat(_tmp31C,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1471
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp31F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp607;_tmp607.tag=0U,({struct _fat_ptr _tmp811=({const char*_tmp320="can't subscript an abstract pointer";_tag_fat(_tmp320,sizeof(char),36U);});_tmp607.f1=_tmp811;});_tmp607;});void*_tmp31E[1U];_tmp31E[0]=& _tmp31F;({unsigned _tmp812=e1->loc;Cyc_Warn_err2(_tmp812,_tag_fat(_tmp31E,sizeof(void*),1U));});});
return t;}case 6U: _LL3: _tmp300=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FF)->f1;_LL4: {struct Cyc_List_List*ts=_tmp300;
# 1476
struct _tuple16 _tmp321=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp30=_tmp321;struct _tuple16 _tmp322=_stmttmp30;int _tmp324;unsigned _tmp323;_LL14: _tmp323=_tmp322.f1;_tmp324=_tmp322.f2;_LL15: {unsigned i=_tmp323;int known=_tmp324;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp326=({struct Cyc_Warn_String_Warn_Warg_struct _tmp608;_tmp608.tag=0U,({
struct _fat_ptr _tmp813=({const char*_tmp327="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp327,sizeof(char),47U);});_tmp608.f1=_tmp813;});_tmp608;});void*_tmp325[1U];_tmp325[0]=& _tmp326;({struct Cyc_Tcenv_Tenv*_tmp815=te;unsigned _tmp814=loc;Cyc_Tcexp_expr_err(_tmp815,_tmp814,0,_tag_fat(_tmp325,sizeof(void*),1U));});});{
struct _handler_cons _tmp328;_push_handler(& _tmp328);{int _tmp32A=0;if(setjmp(_tmp328.handler))_tmp32A=1;if(!_tmp32A){{void*_tmp32B=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp32B;};_pop_handler();}else{void*_tmp329=(void*)Cyc_Core_get_exn_thrown();void*_tmp32C=_tmp329;void*_tmp32D;if(((struct Cyc_List_Nth_exn_struct*)_tmp32C)->tag == Cyc_List_Nth){_LL17: _LL18:
# 1482
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp32F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D;_tmp60D.tag=0U,({struct _fat_ptr _tmp816=({const char*_tmp336="index is ";_tag_fat(_tmp336,sizeof(char),10U);});_tmp60D.f1=_tmp816;});_tmp60D;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp330=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp60C;_tmp60C.tag=12U,_tmp60C.f1=(int)i;_tmp60C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp331=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60B;_tmp60B.tag=0U,({struct _fat_ptr _tmp817=({const char*_tmp335=" but tuple has only ";_tag_fat(_tmp335,sizeof(char),21U);});_tmp60B.f1=_tmp817;});_tmp60B;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp332=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp60A;_tmp60A.tag=12U,({
int _tmp818=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp60A.f1=_tmp818;});_tmp60A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp333=({struct Cyc_Warn_String_Warn_Warg_struct _tmp609;_tmp609.tag=0U,({struct _fat_ptr _tmp819=({const char*_tmp334=" fields";_tag_fat(_tmp334,sizeof(char),8U);});_tmp609.f1=_tmp819;});_tmp609;});void*_tmp32E[5U];_tmp32E[0]=& _tmp32F,_tmp32E[1]=& _tmp330,_tmp32E[2]=& _tmp331,_tmp32E[3]=& _tmp332,_tmp32E[4]=& _tmp333;({struct Cyc_Tcenv_Tenv*_tmp81B=te;unsigned _tmp81A=loc;Cyc_Tcexp_expr_err(_tmp81B,_tmp81A,0,_tag_fat(_tmp32E,sizeof(void*),5U));});});}else{_LL19: _tmp32D=_tmp32C;_LL1A: {void*exn=_tmp32D;(int)_rethrow(exn);}}_LL16:;}}}}}default: _LL5: _LL6:
# 1485
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp338=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60F;_tmp60F.tag=0U,({struct _fat_ptr _tmp81C=({const char*_tmp33A="subscript applied to ";_tag_fat(_tmp33A,sizeof(char),22U);});_tmp60F.f1=_tmp81C;});_tmp60F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp339=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp60E;_tmp60E.tag=2U,_tmp60E.f1=(void*)t1;_tmp60E;});void*_tmp337[2U];_tmp337[0]=& _tmp338,_tmp337[1]=& _tmp339;({struct Cyc_Tcenv_Tenv*_tmp81F=te;unsigned _tmp81E=loc;void**_tmp81D=topt;Cyc_Tcexp_expr_err(_tmp81F,_tmp81E,_tmp81D,_tag_fat(_tmp337,sizeof(void*),2U));});});}_LL0:;}}}
# 1490
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp33B=Cyc_Absyn_compress(*topt);void*_stmttmp31=_tmp33B;void*_tmp33C=_stmttmp31;struct Cyc_List_List*_tmp33D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp33C)->tag == 6U){_LL1: _tmp33D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp33C)->f1;_LL2: {struct Cyc_List_List*ts=_tmp33D;
# 1497
if(({int _tmp820=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp820 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
goto _LL0;
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp33E=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp33E;
({struct Cyc_Tcenv_Tenv*_tmp821=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp821,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1504
({struct Cyc_RgnOrder_RgnPO*_tmp823=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp822=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp823,_tmp822,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp340=_cycalloc(sizeof(*_tmp340));({struct _tuple17*_tmp824=({struct _tuple17*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp33F->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp33F;});_tmp340->hd=_tmp824;}),_tmp340->tl=fields;_tmp340;});}
# 1507
done=1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1511
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp825=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp825,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp342=_cycalloc(sizeof(*_tmp342));({struct _tuple17*_tmp827=({struct _tuple17*_tmp341=_cycalloc(sizeof(*_tmp341));({struct Cyc_Absyn_Tqual _tmp826=Cyc_Absyn_empty_tqual(0U);_tmp341->f1=_tmp826;}),_tmp341->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp341;});_tmp342->hd=_tmp827;}),_tmp342->tl=fields;_tmp342;});}
# 1516
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343->tag=6U,({struct Cyc_List_List*_tmp828=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp343->f1=_tmp828;});_tmp343;});}
# 1520
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1525
struct _tuple9*_tmp344=targ;void*_tmp345;_LL1: _tmp345=_tmp344->f3;_LL2: {void*t=_tmp345;
({unsigned _tmp82C=loc;struct Cyc_Tcenv_Tenv*_tmp82B=te;struct Cyc_List_List*_tmp82A=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp829=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_tak:& Cyc_Kinds_tmk;
# 1526
Cyc_Tctyp_check_type(_tmp82C,_tmp82B,_tmp82A,_tmp829,1,0,t);});
# 1528
({void*_tmp82D=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp346=_cycalloc(sizeof(*_tmp346));_tmp346->tag=36U,_tmp346->f1=0,_tmp346->f2=des;_tmp346;});orig_exp->r=_tmp82D;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1541 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1544
void*res_t2;
int _tmp347=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp347;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Kinds_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp348=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->tag=0U,({union Cyc_Absyn_Cnst _tmp82E=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp366->f1=_tmp82E;});_tmp366;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp348;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1552
if(zero_term){
struct Cyc_Absyn_Exp*_tmp349=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp349;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp34A=0U;({unsigned _tmp830=e->loc;struct _fat_ptr _tmp82F=({const char*_tmp34B="zero-terminated array doesn't end with zero.";_tag_fat(_tmp34B,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp830,_tmp82F,_tag_fat(_tmp34A,sizeof(void*),0U));});});}
# 1557
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp833=res;struct Cyc_Absyn_Tqual _tmp832=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1558
struct Cyc_Absyn_Exp*_tmp831=sz_exp;Cyc_Absyn_array_type(_tmp833,_tmp832,_tmp831,
# 1560
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1562
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1565
if(!({struct Cyc_RgnOrder_RgnPO*_tmp835=Cyc_Tcenv_curr_rgnpo(te);void*_tmp834=res;Cyc_Tcutil_coerce_list(_tmp835,_tmp834,es);}))
# 1567
({struct Cyc_Warn_String_Warn_Warg_struct _tmp34D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp612;_tmp612.tag=0U,({struct _fat_ptr _tmp836=({const char*_tmp351="elements of array do not all have the same type (";_tag_fat(_tmp351,sizeof(char),50U);});_tmp612.f1=_tmp836;});_tmp612;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp34E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp611;_tmp611.tag=2U,_tmp611.f1=(void*)res;_tmp611;});struct Cyc_Warn_String_Warn_Warg_struct _tmp34F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp610;_tmp610.tag=0U,({
struct _fat_ptr _tmp837=({const char*_tmp350=")";_tag_fat(_tmp350,sizeof(char),2U);});_tmp610.f1=_tmp837;});_tmp610;});void*_tmp34C[3U];_tmp34C[0]=& _tmp34D,_tmp34C[1]=& _tmp34E,_tmp34C[2]=& _tmp34F;({unsigned _tmp838=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;Cyc_Warn_err2(_tmp838,_tag_fat(_tmp34C,sizeof(void*),3U));});});
# 1570
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
void*_tmp352=(void*)ds->hd;void*_stmttmp32=_tmp352;void*_tmp353=_stmttmp32;struct Cyc_Absyn_Exp*_tmp354;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp353)->tag == 1U){_LL1: _LL2:
# 1575
({struct Cyc_Warn_String_Warn_Warg_struct _tmp356=({struct Cyc_Warn_String_Warn_Warg_struct _tmp613;_tmp613.tag=0U,({struct _fat_ptr _tmp839=({const char*_tmp357="only array index designators are supported";_tag_fat(_tmp357,sizeof(char),43U);});_tmp613.f1=_tmp839;});_tmp613;});void*_tmp355[1U];_tmp355[0]=& _tmp356;({unsigned _tmp83A=loc;Cyc_Warn_err2(_tmp83A,_tag_fat(_tmp355,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _tmp354=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp353)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp354;
# 1577
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp358=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp33=_tmp358;struct _tuple16 _tmp359=_stmttmp33;int _tmp35B;unsigned _tmp35A;_LL6: _tmp35A=_tmp359.f1;_tmp35B=_tmp359.f2;_LL7: {unsigned i=_tmp35A;int known=_tmp35B;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp35D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp614;_tmp614.tag=0U,({struct _fat_ptr _tmp83B=({const char*_tmp35E="index designator cannot use sizeof or offsetof";_tag_fat(_tmp35E,sizeof(char),47U);});_tmp614.f1=_tmp83B;});_tmp614;});void*_tmp35C[1U];_tmp35C[0]=& _tmp35D;({unsigned _tmp83C=e->loc;Cyc_Warn_err2(_tmp83C,_tag_fat(_tmp35C,sizeof(void*),1U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp360=({struct Cyc_Warn_String_Warn_Warg_struct _tmp618;_tmp618.tag=0U,({struct _fat_ptr _tmp83D=({const char*_tmp365="expecting index designator ";_tag_fat(_tmp365,sizeof(char),28U);});_tmp618.f1=_tmp83D;});_tmp618;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp361=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp617;_tmp617.tag=12U,_tmp617.f1=offset;_tmp617;});struct Cyc_Warn_String_Warn_Warg_struct _tmp362=({struct Cyc_Warn_String_Warn_Warg_struct _tmp616;_tmp616.tag=0U,({struct _fat_ptr _tmp83E=({const char*_tmp364=" but found ";_tag_fat(_tmp364,sizeof(char),12U);});_tmp616.f1=_tmp83E;});_tmp616;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp363=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp615;_tmp615.tag=12U,_tmp615.f1=(int)i;_tmp615;});void*_tmp35F[4U];_tmp35F[0]=& _tmp360,_tmp35F[1]=& _tmp361,_tmp35F[2]=& _tmp362,_tmp35F[3]=& _tmp363;({unsigned _tmp83F=e->loc;Cyc_Warn_err2(_tmp83F,_tag_fat(_tmp35F,sizeof(void*),4U));});});}
goto _LL0;}}}}_LL0:;}}}
# 1587
return res_t2;}struct _tuple19{void**f1;struct Cyc_Absyn_Tqual*f2;void**f3;};
# 1592
static struct _tuple19 Cyc_Tcexp_ptrEltAtts(void**topt){
if(!((unsigned)topt))
return({struct _tuple19 _tmp619;_tmp619.f1=0,_tmp619.f2=0,_tmp619.f3=0;_tmp619;});{
void*_tmp367=Cyc_Absyn_compress(*topt);void*_stmttmp34=_tmp367;void*_tmp368=_stmttmp34;struct Cyc_Absyn_ArrayInfo _tmp369;struct Cyc_Absyn_PtrInfo _tmp36A;switch(*((int*)_tmp368)){case 3U: _LL1: _tmp36A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368)->f1;_LL2: {struct Cyc_Absyn_PtrInfo x=_tmp36A;
# 1597
return({struct _tuple19 _tmp61A;({void**_tmp842=({void**_tmp36B=_cycalloc(sizeof(*_tmp36B));*_tmp36B=x.elt_type;_tmp36B;});_tmp61A.f1=_tmp842;}),({struct Cyc_Absyn_Tqual*_tmp841=({struct Cyc_Absyn_Tqual*_tmp36C=_cycalloc(sizeof(*_tmp36C));*_tmp36C=x.elt_tq;_tmp36C;});_tmp61A.f2=_tmp841;}),({void**_tmp840=({void**_tmp36D=_cycalloc(sizeof(*_tmp36D));*_tmp36D=(x.ptr_atts).zero_term;_tmp36D;});_tmp61A.f3=_tmp840;});_tmp61A;});}case 4U: _LL3: _tmp369=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp368)->f1;_LL4: {struct Cyc_Absyn_ArrayInfo x=_tmp369;
return({struct _tuple19 _tmp61B;({void**_tmp845=({void**_tmp36E=_cycalloc(sizeof(*_tmp36E));*_tmp36E=x.elt_type;_tmp36E;});_tmp61B.f1=_tmp845;}),({struct Cyc_Absyn_Tqual*_tmp844=({struct Cyc_Absyn_Tqual*_tmp36F=_cycalloc(sizeof(*_tmp36F));*_tmp36F=x.tq;_tmp36F;});_tmp61B.f2=_tmp844;}),({void**_tmp843=({void**_tmp370=_cycalloc(sizeof(*_tmp370));*_tmp370=x.zero_term;_tmp370;});_tmp61B.f3=_tmp843;});_tmp61B;});}default: _LL5: _LL6:
 return({struct _tuple19 _tmp61C;_tmp61C.f1=0,_tmp61C.f2=0,_tmp61C.f3=0;_tmp61C;});}_LL0:;}}
# 1604
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1607
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp371=Cyc_Absyn_compress((void*)_check_null(bound->topt));void*_stmttmp35=_tmp371;void*_tmp372=_stmttmp35;void*_tmp373;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->f2 != 0){_LL1: _tmp373=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->f2)->hd;_LL2: {void*t=_tmp373;
# 1612
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp374=({void*_tmp846=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp846,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp374;
b->topt=bound->topt;
bound=b;}
# 1617
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1619
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp376=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61E;_tmp61E.tag=0U,({struct _fat_ptr _tmp847=({const char*_tmp378="expecting unsigned int, found ";_tag_fat(_tmp378,sizeof(char),31U);});_tmp61E.f1=_tmp847;});_tmp61E;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp377=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp61D;_tmp61D.tag=3U,_tmp61D.f1=(void*)bound->topt;_tmp61D;});void*_tmp375[2U];_tmp375[0]=& _tmp376,_tmp375[1]=& _tmp377;({unsigned _tmp848=bound->loc;Cyc_Warn_err2(_tmp848,_tag_fat(_tmp375,sizeof(void*),2U));});});}_LL0:;}
# 1623
if(!(vd->tq).real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61F;_tmp61F.tag=0U,({struct _fat_ptr _tmp849=({const char*_tmp37B="comprehension index variable is not declared const!";_tag_fat(_tmp37B,sizeof(char),52U);});_tmp61F.f1=_tmp849;});_tmp61F;});void*_tmp379[1U];_tmp379[0]=& _tmp37A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp379,sizeof(void*),1U));});
# 1626
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
# 1629
struct _tuple19 _tmp37C=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp36=_tmp37C;struct _tuple19 _tmp37D=_stmttmp36;void**_tmp380;struct Cyc_Absyn_Tqual*_tmp37F;void**_tmp37E;_LL6: _tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;_tmp380=_tmp37D.f3;_LL7: {void**topt2=_tmp37E;struct Cyc_Absyn_Tqual*tqopt=_tmp37F;void**ztopt=_tmp380;
# 1632
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1634
Cyc_Tcexp_check_consume(body->loc,t,body);
if(te->le == 0){
# 1637
if(!Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp382=({struct Cyc_Warn_String_Warn_Warg_struct _tmp620;_tmp620.tag=0U,({struct _fat_ptr _tmp84A=({const char*_tmp383="bound is not constant";_tag_fat(_tmp383,sizeof(char),22U);});_tmp620.f1=_tmp84A;});_tmp620;});void*_tmp381[1U];_tmp381[0]=& _tmp382;({unsigned _tmp84B=bound->loc;Cyc_Warn_err2(_tmp84B,_tag_fat(_tmp381,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp385=({struct Cyc_Warn_String_Warn_Warg_struct _tmp621;_tmp621.tag=0U,({struct _fat_ptr _tmp84C=({const char*_tmp386="body is not constant";_tag_fat(_tmp386,sizeof(char),21U);});_tmp621.f1=_tmp84C;});_tmp621;});void*_tmp384[1U];_tmp384[0]=& _tmp385;({unsigned _tmp84D=body->loc;Cyc_Warn_err2(_tmp84D,_tag_fat(_tmp384,sizeof(void*),1U));});});}
# 1642
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1644
struct Cyc_Absyn_Exp*_tmp387=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp387;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1648
Cyc_Tcexp_check_consume(body->loc,(void*)_check_null(body->topt),body);
# 1650
return({void*_tmp850=t;struct Cyc_Absyn_Tqual _tmp84F=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp84E=bound;Cyc_Absyn_array_type(_tmp850,_tmp84F,_tmp84E,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1655
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1658
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp388=Cyc_Absyn_compress((void*)_check_null(bound->topt));void*_stmttmp37=_tmp388;void*_tmp389=_stmttmp37;void*_tmp38A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp389)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp389)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp389)->f2 != 0){_LL1: _tmp38A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp389)->f2)->hd;_LL2: {void*t=_tmp38A;
# 1663
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp38B=({void*_tmp851=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp851,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp38B;
b->topt=bound->topt;
bound=b;}
# 1668
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1670
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp623;_tmp623.tag=0U,({struct _fat_ptr _tmp852=({const char*_tmp38F="expecting unsigned int, found ";_tag_fat(_tmp38F,sizeof(char),31U);});_tmp623.f1=_tmp852;});_tmp623;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp38E=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp622;_tmp622.tag=3U,_tmp622.f1=(void*)bound->topt;_tmp622;});void*_tmp38C[2U];_tmp38C[0]=& _tmp38D,_tmp38C[1]=& _tmp38E;({unsigned _tmp853=bound->loc;Cyc_Warn_err2(_tmp853,_tag_fat(_tmp38C,sizeof(void*),2U));});});}_LL0:;}{
# 1673
struct _tuple19 _tmp390=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp38=_tmp390;struct _tuple19 _tmp391=_stmttmp38;void**_tmp394;struct Cyc_Absyn_Tqual*_tmp393;void**_tmp392;_LL6: _tmp392=_tmp391.f1;_tmp393=_tmp391.f2;_tmp394=_tmp391.f3;_LL7: {void**topt2=_tmp392;struct Cyc_Absyn_Tqual*tqopt=_tmp393;void**ztopt=_tmp394;
# 1676
({unsigned _tmp856=loc;struct Cyc_Tcenv_Tenv*_tmp855=te;struct Cyc_List_List*_tmp854=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp856,_tmp855,_tmp854,& Cyc_Kinds_tmk,1,1,t);});
if(topt2 != 0)
Cyc_Unify_unify(*topt2,t);
# 1680
if(te->le == 0 && !Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp396=({struct Cyc_Warn_String_Warn_Warg_struct _tmp624;_tmp624.tag=0U,({struct _fat_ptr _tmp857=({const char*_tmp397="bound is not constant";_tag_fat(_tmp397,sizeof(char),22U);});_tmp624.f1=_tmp857;});_tmp624;});void*_tmp395[1U];_tmp395[0]=& _tmp396;({unsigned _tmp858=bound->loc;Cyc_Warn_err2(_tmp858,_tag_fat(_tmp395,sizeof(void*),1U));});});
# 1684
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp399=({struct Cyc_Warn_String_Warn_Warg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp859=({const char*_tmp39A="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp39A,sizeof(char),74U);});_tmp625.f1=_tmp859;});_tmp625;});void*_tmp398[1U];_tmp398[0]=& _tmp399;({unsigned _tmp85A=loc;Cyc_Warn_err2(_tmp85A,_tag_fat(_tmp398,sizeof(void*),1U));});});
# 1688
return({void*_tmp85D=t;struct Cyc_Absyn_Tqual _tmp85C=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp85B=bound;Cyc_Absyn_array_type(_tmp85D,_tmp85C,_tmp85B,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1692
static int Cyc_Tcexp_ensure_typqual_nothrow(void*t){
void*_tmp39B=t;struct Cyc_Absyn_PtrInfo*_tmp39C;struct Cyc_Absyn_FnInfo*_tmp39D;switch(*((int*)_tmp39B)){case 5U: _LL1: _tmp39D=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp39B)->f1;_LL2: {struct Cyc_Absyn_FnInfo*i=_tmp39D;
# 1695
return((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Atts_attribute_cmp,i->attributes,(void*)& Cyc_Absyn_No_throw_att_val);}case 3U: _LL3: _tmp39C=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39B)->f1;_LL4: {struct Cyc_Absyn_PtrInfo*pi=_tmp39C;
# 1699
return Cyc_Tcexp_ensure_typqual_nothrow(pi->elt_type);}default: _LL5: _LL6:
# 1701
 return 1;}_LL0:;}
# 1705
static int Cyc_Tcexp_ensure_nothrow_initializer(struct Cyc_Absyn_Exp*e){
void*_tmp39E=e->r;void*_stmttmp39=_tmp39E;void*_tmp39F=_stmttmp39;struct Cyc_Absyn_Exp*_tmp3A0;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp39F)->tag == 14U){_LL1: _tmp3A0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp39F)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3A0;
return Cyc_Tcexp_ensure_nothrow_initializer(e1);}}else{_LL3: _LL4:
 return Cyc_Tcexp_ensure_typqual_nothrow((void*)_check_null(e->topt));}_LL0:;}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1722 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1726
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp3A1=_cycalloc(sizeof(*_tmp3A1));*_tmp3A1=ad;_tmp3A1;});}else{
# 1732
{struct _handler_cons _tmp3A2;_push_handler(& _tmp3A2);{int _tmp3A4=0;if(setjmp(_tmp3A2.handler))_tmp3A4=1;if(!_tmp3A4){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1732
;_pop_handler();}else{void*_tmp3A3=(void*)Cyc_Core_get_exn_thrown();void*_tmp3A5=_tmp3A3;void*_tmp3A6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3A5)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1735
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp627;_tmp627.tag=0U,({struct _fat_ptr _tmp85E=({const char*_tmp3AA="unbound struct/union name ";_tag_fat(_tmp3AA,sizeof(char),27U);});_tmp627.f1=_tmp85E;});_tmp627;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3A9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp626;_tmp626.tag=1U,_tmp626.f1=*tn;_tmp626;});void*_tmp3A7[2U];_tmp3A7[0]=& _tmp3A8,_tmp3A7[1]=& _tmp3A9;({unsigned _tmp85F=loc;Cyc_Warn_err2(_tmp85F,_tag_fat(_tmp3A7,sizeof(void*),2U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp3A6=_tmp3A5;_LL4: {void*exn=_tmp3A6;(int)_rethrow(exn);}}_LL0:;}}}
# 1738
*ad_opt=ad;
*tn=ad->name;}
# 1741
if(ad->impl == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp629;_tmp629.tag=0U,({struct _fat_ptr _tmp860=({const char*_tmp3AE="can't construct abstract ";_tag_fat(_tmp3AE,sizeof(char),26U);});_tmp629.f1=_tmp860;});_tmp629;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3AD=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp628;_tmp628.tag=6U,_tmp628.f1=ad;_tmp628;});void*_tmp3AB[2U];_tmp3AB[0]=& _tmp3AC,_tmp3AB[1]=& _tmp3AD;({struct Cyc_Tcenv_Tenv*_tmp862=te;unsigned _tmp861=loc;Cyc_Tcexp_expr_err(_tmp862,_tmp861,0,_tag_fat(_tmp3AB,sizeof(void*),2U));});});{
# 1748
struct Cyc_Tcenv_Tenv*_tmp3AF=(int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNew?Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te): te;struct Cyc_Tcenv_Tenv*te2=_tmp3AF;
# 1754
struct _tuple13 _tmp3B0=({struct _tuple13 _tmp636;({struct Cyc_List_List*_tmp863=Cyc_Tcenv_lookup_type_vars(te2);_tmp636.f1=_tmp863;}),_tmp636.f2=Cyc_Core_heap_region;_tmp636;});struct _tuple13 env=_tmp3B0;
struct Cyc_List_List*_tmp3B1=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp3B1;
struct Cyc_List_List*_tmp3B2=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp3B2;
struct Cyc_List_List*_tmp3B3=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp3B3;
struct Cyc_List_List*_tmp3B4=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp3B4;
struct Cyc_List_List*_tmp3B5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp3B5;
void*res_typ;
# 1765
if(topt != 0){
void*_tmp3B6=Cyc_Absyn_compress(*topt);void*_stmttmp3A=_tmp3B6;void*_tmp3B7=_stmttmp3A;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_Aggrdecl**_tmp3B8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp3B8=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->f1)->f1).KnownAggr).val;_tmp3B9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp3B8;struct Cyc_List_List*all_typs2=_tmp3B9;
# 1768
if(*adptr2 != *adptr)
goto _LL9;
{struct Cyc_List_List*_tmp3BA=all_typs;struct Cyc_List_List*ats=_tmp3BA;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1774
res_typ=*topt;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 res_typ=({union Cyc_Absyn_AggrInfo _tmp864=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp864,all_typs);});}_LL5:;}else{
# 1779
res_typ=({union Cyc_Absyn_AggrInfo _tmp865=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp865,all_typs);});}{
# 1782
struct Cyc_List_List*_tmp3BB=*ts;struct Cyc_List_List*user_ex_ts=_tmp3BB;
struct Cyc_List_List*_tmp3BC=exist_typs;struct Cyc_List_List*ex_ts=_tmp3BC;
while(user_ex_ts != 0 && ex_ts != 0){
# 1786
({unsigned _tmp868=loc;struct Cyc_Tcenv_Tenv*_tmp867=te2;struct Cyc_List_List*_tmp866=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp868,_tmp867,_tmp866,& Cyc_Kinds_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1792
if(user_ex_ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62A;_tmp62A.tag=0U,({struct _fat_ptr _tmp869=({const char*_tmp3BF="too many explicit witness types";_tag_fat(_tmp3BF,sizeof(char),32U);});_tmp62A.f1=_tmp869;});_tmp62A;});void*_tmp3BD[1U];_tmp3BD[0]=& _tmp3BE;({unsigned _tmp86A=loc;Cyc_Warn_err2(_tmp86A,_tag_fat(_tmp3BD,sizeof(void*),1U));});});
# 1795
*ts=exist_typs;{
# 1798
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3C0=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3B=_tmp3C0;struct _tuple20*_tmp3C1=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Aggrfield*_tmp3C2;_LLB: _tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp3C2;struct Cyc_Absyn_Exp*field_exp=_tmp3C3;
void*_tmp3C4=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp3C4;
void*_tmp3C5=({struct Cyc_Tcenv_Tenv*_tmp86B=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp86B,& inst_fieldtyp,field_exp);});void*ftyp=_tmp3C5;
# 1806
if(!({struct Cyc_RgnOrder_RgnPO*_tmp86D=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp86C=field_exp;Cyc_Tcutil_coerce_arg(_tmp86D,_tmp86C,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp632;_tmp632.tag=0U,({struct _fat_ptr _tmp86E=({const char*_tmp3D2="field ";_tag_fat(_tmp3D2,sizeof(char),7U);});_tmp632.f1=_tmp86E;});_tmp632;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp631;_tmp631.tag=0U,_tmp631.f1=*field->name;_tmp631;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp630;_tmp630.tag=0U,({struct _fat_ptr _tmp86F=({const char*_tmp3D1=" of ";_tag_fat(_tmp3D1,sizeof(char),5U);});_tmp630.f1=_tmp86F;});_tmp630;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3CA=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp62F;_tmp62F.tag=6U,_tmp62F.f1=ad;_tmp62F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62E;_tmp62E.tag=0U,({
struct _fat_ptr _tmp870=({const char*_tmp3D0=" expects type ";_tag_fat(_tmp3D0,sizeof(char),15U);});_tmp62E.f1=_tmp870;});_tmp62E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3CC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62D;_tmp62D.tag=2U,_tmp62D.f1=(void*)inst_fieldtyp;_tmp62D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62C;_tmp62C.tag=0U,({struct _fat_ptr _tmp871=({const char*_tmp3CF=" != ";_tag_fat(_tmp3CF,sizeof(char),5U);});_tmp62C.f1=_tmp871;});_tmp62C;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3CE=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp62B;_tmp62B.tag=3U,_tmp62B.f1=(void*)field_exp->topt;_tmp62B;});void*_tmp3C6[8U];_tmp3C6[0]=& _tmp3C7,_tmp3C6[1]=& _tmp3C8,_tmp3C6[2]=& _tmp3C9,_tmp3C6[3]=& _tmp3CA,_tmp3C6[4]=& _tmp3CB,_tmp3C6[5]=& _tmp3CC,_tmp3C6[6]=& _tmp3CD,_tmp3C6[7]=& _tmp3CE;({unsigned _tmp872=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp872,_tag_fat(_tmp3C6,sizeof(void*),8U));});});{
# 1810
int nothrow_fld=((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Atts_attribute_cmp,field->attributes,(void*)& Cyc_Absyn_No_throw_att_val);
# 1813
if(nothrow_fld && !Cyc_Tcexp_ensure_nothrow_initializer(field_exp))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp873=({const char*_tmp3D8="field ";_tag_fat(_tmp3D8,sizeof(char),7U);});_tmp635.f1=_tmp873;});_tmp635;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp634;_tmp634.tag=0U,_tmp634.f1=*field->name;_tmp634;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,({struct _fat_ptr _tmp874=({const char*_tmp3D7=" can only hold a no_throw function";_tag_fat(_tmp3D7,sizeof(char),35U);});_tmp633.f1=_tmp874;});_tmp633;});void*_tmp3D3[3U];_tmp3D3[0]=& _tmp3D4,_tmp3D3[1]=& _tmp3D5,_tmp3D3[2]=& _tmp3D6;({unsigned _tmp875=field_exp->loc;Cyc_Warn_err2(_tmp875,_tag_fat(_tmp3D3,sizeof(void*),3U));});});}}}{
# 1817
struct Cyc_List_List*_tmp3D9=0;struct Cyc_List_List*rpo_inst=_tmp3D9;
{struct Cyc_List_List*_tmp3DA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3DA;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));({struct _tuple0*_tmp878=({struct _tuple0*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));({void*_tmp877=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3DB->f1=_tmp877;}),({
void*_tmp876=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3DB->f2=_tmp876;});_tmp3DB;});
# 1819
_tmp3DC->hd=_tmp878;}),_tmp3DC->tl=rpo_inst;_tmp3DC;});}}
# 1822
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}
# 1828
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1830
{void*_tmp3DD=Cyc_Absyn_compress(ts);void*_stmttmp3C=_tmp3DD;void*_tmp3DE=_stmttmp3C;struct Cyc_List_List*_tmp3E0;enum Cyc_Absyn_AggrKind _tmp3DF;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3DE)->tag == 7U){_LL1: _tmp3DF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3DE)->f1;_tmp3E0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3DE)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp3DF;struct Cyc_List_List*fs=_tmp3E0;
# 1832
if((int)k == (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp637;_tmp637.tag=0U,({struct _fat_ptr _tmp879=({const char*_tmp3E3="expecting struct but found union";_tag_fat(_tmp3E3,sizeof(char),33U);});_tmp637.f1=_tmp879;});_tmp637;});void*_tmp3E1[1U];_tmp3E1[0]=& _tmp3E2;({unsigned _tmp87A=loc;Cyc_Warn_err2(_tmp87A,_tag_fat(_tmp3E1,sizeof(void*),1U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3E4=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3D=_tmp3E4;struct _tuple20*_tmp3E5=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_Absyn_Aggrfield*_tmp3E6;_LL6: _tmp3E6=_tmp3E5->f1;_tmp3E7=_tmp3E5->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp3E6;struct Cyc_Absyn_Exp*field_exp=_tmp3E7;
({struct Cyc_Tcenv_Tenv*_tmp87C=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp87B=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp87C,_tmp87B,field_exp);});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp87E=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp87D=field_exp;Cyc_Tcutil_coerce_arg(_tmp87E,_tmp87D,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D;_tmp63D.tag=0U,({struct _fat_ptr _tmp87F=({const char*_tmp3F1="field ";_tag_fat(_tmp3F1,sizeof(char),7U);});_tmp63D.f1=_tmp87F;});_tmp63D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63C;_tmp63C.tag=0U,_tmp63C.f1=*field->name;_tmp63C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63B;_tmp63B.tag=0U,({
struct _fat_ptr _tmp880=({const char*_tmp3F0=" of struct expects type ";_tag_fat(_tmp3F0,sizeof(char),25U);});_tmp63B.f1=_tmp880;});_tmp63B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3EC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63A;_tmp63A.tag=2U,_tmp63A.f1=(void*)field->type;_tmp63A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp639;_tmp639.tag=0U,({struct _fat_ptr _tmp881=({const char*_tmp3EF=" != ";_tag_fat(_tmp3EF,sizeof(char),5U);});_tmp639.f1=_tmp881;});_tmp639;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3EE=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp638;_tmp638.tag=3U,_tmp638.f1=(void*)field_exp->topt;_tmp638;});void*_tmp3E8[6U];_tmp3E8[0]=& _tmp3E9,_tmp3E8[1]=& _tmp3EA,_tmp3E8[2]=& _tmp3EB,_tmp3E8[3]=& _tmp3EC,_tmp3E8[4]=& _tmp3ED,_tmp3E8[5]=& _tmp3EE;({unsigned _tmp882=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp882,_tag_fat(_tmp3E8,sizeof(void*),6U));});});}}
# 1845
goto _LL0;}}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63E;_tmp63E.tag=0U,({struct _fat_ptr _tmp883=({const char*_tmp3F4="tcAnonStruct: wrong type";_tag_fat(_tmp3F4,sizeof(char),25U);});_tmp63E.f1=_tmp883;});_tmp63E;});void*_tmp3F2[1U];_tmp3F2[0]=& _tmp3F3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3F2,sizeof(void*),1U));});}_LL0:;}
# 1848
return ts;}
# 1852
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1855
struct _tuple13 _tmp3F5=({struct _tuple13 _tmp649;({struct Cyc_List_List*_tmp884=Cyc_Tcenv_lookup_type_vars(te);_tmp649.f1=_tmp884;}),_tmp649.f2=Cyc_Core_heap_region;_tmp649;});struct _tuple13 env=_tmp3F5;
struct Cyc_List_List*_tmp3F6=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3F6;
struct Cyc_List_List*_tmp3F7=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp3F7;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp885=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp885,all_typs);});
# 1860
if(topt != 0){
void*_tmp3F8=Cyc_Absyn_compress(*topt);void*_stmttmp3E=_tmp3F8;void*_tmp3F9=_stmttmp3E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F9)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F9)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1866
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp887=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp886=e;Cyc_Tcutil_coerce_arg(_tmp887,_tmp886,t,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0U,({struct _fat_ptr _tmp888=({const char*_tmp404="datatype constructor ";_tag_fat(_tmp404,sizeof(char),22U);});_tmp644.f1=_tmp888;});_tmp644;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3FC=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp643;_tmp643.tag=1U,_tmp643.f1=tuf->name;_tmp643;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3FD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp642;_tmp642.tag=0U,({
struct _fat_ptr _tmp889=({const char*_tmp403=" expects argument of type ";_tag_fat(_tmp403,sizeof(char),27U);});_tmp642.f1=_tmp889;});_tmp642;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3FE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp641;_tmp641.tag=2U,_tmp641.f1=(void*)t;_tmp641;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp640;_tmp640.tag=0U,({struct _fat_ptr _tmp88A=({const char*_tmp402=" but this argument has type ";_tag_fat(_tmp402,sizeof(char),29U);});_tmp640.f1=_tmp88A;});_tmp640;});struct Cyc_Warn_String_Warn_Warg_struct _tmp400=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63F;_tmp63F.tag=0U,
e->topt == 0?({struct _fat_ptr _tmp88C=(struct _fat_ptr)({const char*_tmp401="?";_tag_fat(_tmp401,sizeof(char),2U);});_tmp63F.f1=_tmp88C;}):({struct _fat_ptr _tmp88B=Cyc_Absynpp_typ2string((void*)_check_null(e->topt));_tmp63F.f1=_tmp88B;});_tmp63F;});void*_tmp3FA[6U];_tmp3FA[0]=& _tmp3FB,_tmp3FA[1]=& _tmp3FC,_tmp3FA[2]=& _tmp3FD,_tmp3FA[3]=& _tmp3FE,_tmp3FA[4]=& _tmp3FF,_tmp3FA[5]=& _tmp400;({unsigned _tmp88D=e->loc;Cyc_Warn_err2(_tmp88D,_tag_fat(_tmp3FA,sizeof(void*),6U));});});
Cyc_Unify_explain_failure();}}
# 1879
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp406=({struct Cyc_Warn_String_Warn_Warg_struct _tmp646;_tmp646.tag=0U,({
struct _fat_ptr _tmp88E=({const char*_tmp408="too many arguments for datatype constructor ";_tag_fat(_tmp408,sizeof(char),45U);});_tmp646.f1=_tmp88E;});_tmp646;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp407=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp645;_tmp645.tag=1U,_tmp645.f1=tuf->name;_tmp645;});void*_tmp405[2U];_tmp405[0]=& _tmp406,_tmp405[1]=& _tmp407;({struct Cyc_Tcenv_Tenv*_tmp891=te;unsigned _tmp890=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp88F=topt;Cyc_Tcexp_expr_err(_tmp891,_tmp890,_tmp88F,_tag_fat(_tmp405,sizeof(void*),2U));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp40A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp648;_tmp648.tag=0U,({
struct _fat_ptr _tmp892=({const char*_tmp40C="too few arguments for datatype constructor ";_tag_fat(_tmp40C,sizeof(char),44U);});_tmp648.f1=_tmp892;});_tmp648;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp40B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp647;_tmp647.tag=1U,_tmp647.f1=tuf->name;_tmp647;});void*_tmp409[2U];_tmp409[0]=& _tmp40A,_tmp409[1]=& _tmp40B;({struct Cyc_Tcenv_Tenv*_tmp895=te;unsigned _tmp894=loc;void**_tmp893=topt;Cyc_Tcexp_expr_err(_tmp895,_tmp894,_tmp893,_tag_fat(_tmp409,sizeof(void*),2U));});});
return res;}}
# 1889
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1891
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1894
if(topt == 0)
return 0;{
void*_tmp40D=Cyc_Absyn_compress(*topt);void*_stmttmp3F=_tmp40D;void*_tmp40E=_stmttmp3F;void*_tmp40F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->tag == 3U){_LL1: _tmp40F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->f1).elt_type;_LL2: {void*elt_typ=_tmp40F;
# 1898
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1904
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1906
enum Cyc_Absyn_AliasQual _tmp410=(Cyc_Tcutil_type_kind(Cyc_Absyn_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp40=_tmp410;enum Cyc_Absyn_AliasQual _tmp411=_stmttmp40;if(_tmp411 == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 1912
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 1916
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 1920
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp412=Cyc_Absyn_compress(handle_type);void*_stmttmp41=_tmp412;void*_tmp413=_stmttmp41;void*_tmp414;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp413)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp413)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp413)->f2 != 0){_LL1: _tmp414=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp413)->f2)->hd;_LL2: {void*r=_tmp414;
# 1925
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1929
({struct Cyc_Warn_String_Warn_Warg_struct _tmp416=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64B;_tmp64B.tag=0U,({struct _fat_ptr _tmp896=({const char*_tmp418="expecting region_t type but found ";_tag_fat(_tmp418,sizeof(char),35U);});_tmp64B.f1=_tmp896;});_tmp64B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp417=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64A;_tmp64A.tag=2U,_tmp64A.f1=(void*)handle_type;_tmp64A;});void*_tmp415[2U];_tmp415[0]=& _tmp416,_tmp415[1]=& _tmp417;({unsigned _tmp897=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;Cyc_Warn_err2(_tmp897,_tag_fat(_tmp415,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}else{
# 1935
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_uniquergn_exp();*ropt=_tmp898;});}}}
# 1942
({struct Cyc_Tcenv_Tenv*_tmp899=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp899,& Cyc_Absyn_uint_type,*e);});{
# 1951 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp41A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64C;_tmp64C.tag=0U,({struct _fat_ptr _tmp89A=({const char*_tmp41B="calloc with empty type";_tag_fat(_tmp41B,sizeof(char),23U);});_tmp64C.f1=_tmp89A;});_tmp64C;});void*_tmp419[1U];_tmp419[0]=& _tmp41A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp419,sizeof(void*),1U));});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp89D=loc;struct Cyc_Tcenv_Tenv*_tmp89C=te;struct Cyc_List_List*_tmp89B=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp89D,_tmp89C,_tmp89B,& Cyc_Kinds_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp41D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64F;_tmp64F.tag=0U,({struct _fat_ptr _tmp89E=({const char*_tmp421="calloc cannot be used with type ";_tag_fat(_tmp421,sizeof(char),33U);});_tmp64F.f1=_tmp89E;});_tmp64F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp41E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64E;_tmp64E.tag=2U,_tmp64E.f1=(void*)elt_type;_tmp64E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp41F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64D;_tmp64D.tag=0U,({
struct _fat_ptr _tmp89F=({const char*_tmp420="\n\t(type needs initialization)";_tag_fat(_tmp420,sizeof(char),30U);});_tmp64D.f1=_tmp89F;});_tmp64D;});void*_tmp41C[3U];_tmp41C[0]=& _tmp41D,_tmp41C[1]=& _tmp41E,_tmp41C[2]=& _tmp41F;({unsigned _tmp8A0=loc;Cyc_Warn_err2(_tmp8A0,_tag_fat(_tmp41C,sizeof(void*),3U));});});
num_elts=*e;
one_elt=0;}else{
# 1966
void*er=(*e)->r;
retry_sizeof: {
void*_tmp422=er;struct Cyc_Absyn_Exp*_tmp424;struct Cyc_Absyn_Exp*_tmp423;void*_tmp425;switch(*((int*)_tmp422)){case 17U: _LL6: _tmp425=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp422)->f1;_LL7: {void*t2=_tmp425;
# 1970
elt_type=t2;
({void**_tmp8A1=({void**_tmp426=_cycalloc(sizeof(*_tmp426));*_tmp426=elt_type;_tmp426;});*t=_tmp8A1;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp422)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp422)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp422)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp422)->f2)->tl)->tl == 0){_LL8: _tmp423=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp422)->f2)->hd;_tmp424=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp422)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp423;struct Cyc_Absyn_Exp*e2=_tmp424;
# 1977
{struct _tuple0 _tmp427=({struct _tuple0 _tmp653;_tmp653.f1=e1->r,_tmp653.f2=e2->r;_tmp653;});struct _tuple0 _stmttmp42=_tmp427;struct _tuple0 _tmp428=_stmttmp42;void*_tmp429;void*_tmp42A;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp428.f1)->tag == 17U){_LLD: _tmp42A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp428.f1)->f1;_LLE: {void*t1=_tmp42A;
# 1979
e1=e2;
_tmp429=t1;goto _LL10;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp428.f2)->tag == 17U){_LLF: _tmp429=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp428.f2)->f1;_LL10: {void*t2=_tmp429;
# 1983
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 1986
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp42C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp652;_tmp652.tag=0U,({struct _fat_ptr _tmp8A2=({const char*_tmp430="malloc cannot be used with type ";_tag_fat(_tmp430,sizeof(char),33U);});_tmp652.f1=_tmp8A2;});_tmp652;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp42D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp651;_tmp651.tag=2U,_tmp651.f1=(void*)t2;_tmp651;});struct Cyc_Warn_String_Warn_Warg_struct _tmp42E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp650;_tmp650.tag=0U,({
struct _fat_ptr _tmp8A3=({const char*_tmp42F="\n\t(type needs initialization)";_tag_fat(_tmp42F,sizeof(char),30U);});_tmp650.f1=_tmp8A3;});_tmp650;});void*_tmp42B[3U];_tmp42B[0]=& _tmp42C,_tmp42B[1]=& _tmp42D,_tmp42B[2]=& _tmp42E;({unsigned _tmp8A4=loc;Cyc_Warn_err2(_tmp8A4,_tag_fat(_tmp42B,sizeof(void*),3U));});});else{
# 1990
*is_calloc=1;}}
# 1992
elt_type=t2;
({void**_tmp8A5=({void**_tmp431=_cycalloc(sizeof(*_tmp431));*_tmp431=elt_type;_tmp431;});*t=_tmp8A5;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 1999
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2001
 No_sizeof: {
# 2004
struct Cyc_Absyn_Exp*_tmp432=*e;struct Cyc_Absyn_Exp*real_e=_tmp432;
{void*_tmp433=real_e->r;void*_stmttmp43=_tmp433;void*_tmp434=_stmttmp43;struct Cyc_Absyn_Exp*_tmp435;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp434)->tag == 14U){_LL14: _tmp435=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp434)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp435;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2009
{void*_tmp436=Cyc_Absyn_compress((void*)_check_null(real_e->topt));void*_stmttmp44=_tmp436;void*_tmp437=_stmttmp44;void*_tmp438;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f2 != 0){_LL19: _tmp438=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f2)->hd;_LL1A: {void*tagt=_tmp438;
# 2011
{void*_tmp439=Cyc_Absyn_compress(tagt);void*_stmttmp45=_tmp439;void*_tmp43A=_stmttmp45;struct Cyc_Absyn_Exp*_tmp43B;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43A)->tag == 9U){_LL1E: _tmp43B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43A)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp43B;
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2015
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2018
elt_type=Cyc_Absyn_char_type;
({void**_tmp8A6=({void**_tmp43C=_cycalloc(sizeof(*_tmp43C));*_tmp43C=elt_type;_tmp43C;});*t=_tmp8A6;});
num_elts=*e;
one_elt=0;}
# 2023
goto _LL5;}_LL5:;}}
# 2027
*is_fat=!one_elt;
# 2030
{void*_tmp43D=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp43E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43D)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp43E=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43D)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp43E;
# 2032
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp440=({struct Cyc_Warn_String_Warn_Warg_struct _tmp654;_tmp654.tag=0U,({struct _fat_ptr _tmp8A7=({const char*_tmp441="malloc with existential types not yet implemented";_tag_fat(_tmp441,sizeof(char),50U);});_tmp654.f1=_tmp8A7;});_tmp654;});void*_tmp43F[1U];_tmp43F[0]=& _tmp440;({unsigned _tmp8A8=loc;Cyc_Warn_err2(_tmp8A8,_tag_fat(_tmp43F,sizeof(void*),1U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2039
void*(*_tmp442)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term,void*rel)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term,void*rel)=_tmp442;
void*_tmp443=Cyc_Absyn_false_type;void*zero_term=_tmp443;
if(topt != 0){
void*_tmp444=Cyc_Absyn_compress(*topt);void*_stmttmp46=_tmp444;void*_tmp445=_stmttmp46;void*_tmp449;void*_tmp448;void*_tmp447;void*_tmp446;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->tag == 3U){_LL28: _tmp446=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).nullable;_tmp447=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).bounds;_tmp448=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).zero_term;_tmp449=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).released;_LL29: {void*n=_tmp446;void*b=_tmp447;void*zt=_tmp448;void*rel=_tmp449;
# 2044
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,rel))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp655;_tmp655.tag=0U,({struct _fat_ptr _tmp8A9=({const char*_tmp44C="cannot malloc() an autoreleased pointer";_tag_fat(_tmp44C,sizeof(char),40U);});_tmp655.f1=_tmp8A9;});_tmp655;});void*_tmp44A[1U];_tmp44A[0]=& _tmp44B;({unsigned _tmp8AA=loc;Cyc_Warn_err2(_tmp8AA,_tag_fat(_tmp44A,sizeof(void*),1U));});});
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2051
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp656;_tmp656.tag=0U,({struct _fat_ptr _tmp8AB=({const char*_tmp44F="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp44F,sizeof(char),55U);});_tmp656.f1=_tmp8AB;});_tmp656;});void*_tmp44D[1U];_tmp44D[0]=& _tmp44E;({unsigned _tmp8AC=loc;Cyc_Warn_warn2(_tmp8AC,_tag_fat(_tmp44D,sizeof(void*),1U));});});
*is_calloc=1;}{
# 2057
struct Cyc_Absyn_Exp*_tmp450=({void*_tmp8AD=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8AD,b);});struct Cyc_Absyn_Exp*eopt=_tmp450;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp451=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp451;
int _tmp452=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp452;
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp8B2=elt_type;void*_tmp8B1=rgn;struct Cyc_Absyn_Tqual _tmp8B0=Cyc_Absyn_empty_tqual(0U);void*_tmp8AF=b;void*_tmp8AE=zero_term;Cyc_Absyn_atb_type(_tmp8B2,_tmp8B1,_tmp8B0,_tmp8AF,_tmp8AE,Cyc_Absyn_false_type);});}{
# 2065
void*_tmp453=Cyc_Absyn_compress((void*)_check_null(num_elts->topt));void*_stmttmp47=_tmp453;void*_tmp454=_stmttmp47;void*_tmp455;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp454)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp454)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp454)->f2 != 0){_LL2D: _tmp455=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp454)->f2)->hd;_LL2E: {void*tagtyp=_tmp455;
# 2067
struct Cyc_Absyn_Exp*_tmp456=({void*_tmp8B3=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp8B3,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp456;
# 2069
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp8B8=elt_type;void*_tmp8B7=rgn;struct Cyc_Absyn_Tqual _tmp8B6=Cyc_Absyn_empty_tqual(0U);void*_tmp8B5=b;void*_tmp8B4=zero_term;Cyc_Absyn_atb_type(_tmp8B8,_tmp8B7,_tmp8B6,_tmp8B5,_tmp8B4,Cyc_Absyn_false_type);});}
# 2073
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2077
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2080
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp8BD)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term,void*rel)=ptr_maker;void*_tmp8BC=elt_type;void*_tmp8BB=rgn;struct Cyc_Absyn_Tqual _tmp8BA=Cyc_Absyn_empty_tqual(0U);void*_tmp8B9=zero_term;_tmp8BD(_tmp8BC,_tmp8BB,_tmp8BA,_tmp8B9,Cyc_Absyn_false_type);});}}}
# 2085
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2087
struct Cyc_Tcenv_Tenv*_tmp457=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp457;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp458=(void*)_check_null(e1->topt);void*t1=_tmp458;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2094
{void*_tmp459=Cyc_Absyn_compress(t1);void*_stmttmp48=_tmp459;void*_tmp45A=_stmttmp48;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp45A)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp45C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp657;_tmp657.tag=0U,({struct _fat_ptr _tmp8BE=({const char*_tmp45D="cannot assign to an array";_tag_fat(_tmp45D,sizeof(char),26U);});_tmp657.f1=_tmp8BE;});_tmp657;});void*_tmp45B[1U];_tmp45B[0]=& _tmp45C;({unsigned _tmp8BF=loc;Cyc_Warn_err2(_tmp8BF,_tag_fat(_tmp45B,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2099
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp45F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp658;_tmp658.tag=0U,({struct _fat_ptr _tmp8C0=({const char*_tmp460="swap not allowed for non-pointer or non-word-sized types";_tag_fat(_tmp460,sizeof(char),57U);});_tmp658.f1=_tmp8C0;});_tmp658;});void*_tmp45E[1U];_tmp45E[0]=& _tmp45F;({unsigned _tmp8C1=loc;Cyc_Warn_err2(_tmp8C1,_tag_fat(_tmp45E,sizeof(void*),1U));});});
# 2103
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp462=({struct Cyc_Warn_String_Warn_Warg_struct _tmp659;_tmp659.tag=0U,({struct _fat_ptr _tmp8C2=({const char*_tmp463="swap non-lvalue";_tag_fat(_tmp463,sizeof(char),16U);});_tmp659.f1=_tmp8C2;});_tmp659;});void*_tmp461[1U];_tmp461[0]=& _tmp462;({struct Cyc_Tcenv_Tenv*_tmp8C5=te;unsigned _tmp8C4=e1->loc;void**_tmp8C3=topt;Cyc_Tcexp_expr_err(_tmp8C5,_tmp8C4,_tmp8C3,_tag_fat(_tmp461,sizeof(void*),1U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp465=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65A;_tmp65A.tag=0U,({struct _fat_ptr _tmp8C6=({const char*_tmp466="swap non-lvalue";_tag_fat(_tmp466,sizeof(char),16U);});_tmp65A.f1=_tmp8C6;});_tmp65A;});void*_tmp464[1U];_tmp464[0]=& _tmp465;({struct Cyc_Tcenv_Tenv*_tmp8C9=te;unsigned _tmp8C8=e2->loc;void**_tmp8C7=topt;Cyc_Tcexp_expr_err(_tmp8C9,_tmp8C8,_tmp8C7,_tag_fat(_tmp464,sizeof(void*),1U));});});
# 2108
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp468=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65E;_tmp65E.tag=0U,({struct _fat_ptr _tmp8CA=({const char*_tmp46D="type mismatch: ";_tag_fat(_tmp46D,sizeof(char),16U);});_tmp65E.f1=_tmp8CA;});_tmp65E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp469=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65D;_tmp65D.tag=2U,_tmp65D.f1=(void*)t1;_tmp65D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp46A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65C;_tmp65C.tag=0U,({struct _fat_ptr _tmp8CB=({const char*_tmp46C=" != ";_tag_fat(_tmp46C,sizeof(char),5U);});_tmp65C.f1=_tmp8CB;});_tmp65C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp46B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65B;_tmp65B.tag=2U,_tmp65B.f1=(void*)t2;_tmp65B;});void*_tmp467[4U];_tmp467[0]=& _tmp468,_tmp467[1]=& _tmp469,_tmp467[2]=& _tmp46A,_tmp467[3]=& _tmp46B;({struct Cyc_Tcenv_Tenv*_tmp8CE=te;unsigned _tmp8CD=loc;void**_tmp8CC=topt;Cyc_Tcexp_expr_err(_tmp8CE,_tmp8CD,_tmp8CC,_tag_fat(_tmp467,sizeof(void*),4U));});});
return Cyc_Absyn_void_type;}}}
# 2116
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp8CF=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp8CF,s,1);});
# 2119
while(1){
void*_tmp46E=s->r;void*_stmttmp49=_tmp46E;void*_tmp46F=_stmttmp49;struct Cyc_Absyn_Stmt*_tmp470;struct Cyc_Absyn_Stmt*_tmp471;struct Cyc_Absyn_Exp*_tmp472;switch(*((int*)_tmp46F)){case 1U: _LL1: _tmp472=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp46F)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp472;
# 2123
void*_tmp473=(void*)_check_null(e->topt);void*t=_tmp473;
if(!({void*_tmp8D0=t;Cyc_Unify_unify(_tmp8D0,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp475=({struct Cyc_Warn_String_Warn_Warg_struct _tmp660;_tmp660.tag=0U,({struct _fat_ptr _tmp8D1=({const char*_tmp477="statement expression returns type ";_tag_fat(_tmp477,sizeof(char),35U);});_tmp660.f1=_tmp8D1;});_tmp660;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp476=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65F;_tmp65F.tag=2U,_tmp65F.f1=(void*)t;_tmp65F;});void*_tmp474[2U];_tmp474[0]=& _tmp475,_tmp474[1]=& _tmp476;({unsigned _tmp8D2=loc;Cyc_Tcexp_err_and_explain(_tmp8D2,_tag_fat(_tmp474,sizeof(void*),2U));});});
return t;}case 2U: _LL3: _tmp471=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp46F)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp471;
s=s2;continue;}case 12U: _LL5: _tmp470=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp46F)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp470;
s=s1;continue;}default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp479=({struct Cyc_Warn_String_Warn_Warg_struct _tmp661;_tmp661.tag=0U,({
struct _fat_ptr _tmp8D3=({const char*_tmp47A="statement expression must end with expression";_tag_fat(_tmp47A,sizeof(char),46U);});_tmp661.f1=_tmp8D3;});_tmp661;});void*_tmp478[1U];_tmp478[0]=& _tmp479;({struct Cyc_Tcenv_Tenv*_tmp8D6=te;unsigned _tmp8D5=loc;void**_tmp8D4=topt;Cyc_Tcexp_expr_err(_tmp8D6,_tmp8D5,_tmp8D4,_tag_fat(_tmp478,sizeof(void*),1U));});});}_LL0:;}}
# 2134
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Absyn_compress(({struct Cyc_Tcenv_Tenv*_tmp8D7=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp8D7,0,e);}));
{void*_tmp47B=t;struct Cyc_Absyn_Aggrdecl*_tmp47C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47B)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47B)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp47C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47B)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp47C;
# 2138
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp47E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp663;_tmp663.tag=0U,({struct _fat_ptr _tmp8D8=({const char*_tmp480="expecting @tagged union but found ";_tag_fat(_tmp480,sizeof(char),35U);});_tmp663.f1=_tmp8D8;});_tmp663;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp47F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp662;_tmp662.tag=2U,_tmp662.f1=(void*)t;_tmp662;});void*_tmp47D[2U];_tmp47D[0]=& _tmp47E,_tmp47D[1]=& _tmp47F;({unsigned _tmp8D9=loc;Cyc_Warn_err2(_tmp8D9,_tag_fat(_tmp47D,sizeof(void*),2U));});});goto _LL0;}_LL0:;}
# 2143
return Cyc_Absyn_uint_type;}
# 2146
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
({struct Cyc_Tcenv_Tenv*_tmp8DB=te;struct Cyc_Absyn_Exp*_tmp8DA=e;Cyc_Tcexp_tcTest(_tmp8DB,_tmp8DA,({const char*_tmp481="@assert";_tag_fat(_tmp481,sizeof(char),8U);}));});
if(!Cyc_Absyn_no_side_effects_exp(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp483=({struct Cyc_Warn_String_Warn_Warg_struct _tmp664;_tmp664.tag=0U,({struct _fat_ptr _tmp8DC=({const char*_tmp484="@assert expression may have side effects";_tag_fat(_tmp484,sizeof(char),41U);});_tmp664.f1=_tmp8DC;});_tmp664;});void*_tmp482[1U];_tmp482[0]=& _tmp483;({unsigned _tmp8DD=loc;Cyc_Warn_err2(_tmp8DD,_tag_fat(_tmp482,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;}
# 2154
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2158
void*rgn=Cyc_Absyn_heap_rgn_type;
te=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2163
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp485=Cyc_Absyn_compress(handle_type);void*_stmttmp4A=_tmp485;void*_tmp486=_stmttmp4A;void*_tmp487;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp486)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp486)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp486)->f2 != 0){_LL1: _tmp487=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp486)->f2)->hd;_LL2: {void*r=_tmp487;
# 2168
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2172
({struct Cyc_Warn_String_Warn_Warg_struct _tmp489=({struct Cyc_Warn_String_Warn_Warg_struct _tmp666;_tmp666.tag=0U,({struct _fat_ptr _tmp8DE=({const char*_tmp48B="expecting region_t type but found ";_tag_fat(_tmp48B,sizeof(char),35U);});_tmp666.f1=_tmp8DE;});_tmp666;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp48A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp665;_tmp665.tag=2U,_tmp665.f1=(void*)handle_type;_tmp665;});void*_tmp488[2U];_tmp488[0]=& _tmp489,_tmp488[1]=& _tmp48A;({unsigned _tmp8DF=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;Cyc_Warn_err2(_tmp8DF,_tag_fat(_tmp488,sizeof(void*),2U));});});
# 2174
goto _LL0;}_LL0:;}else{
# 2176
if(topt != 0){
# 2179
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)
({struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp8E0;});}}}{
# 2187
void*_tmp48C=e1->r;void*_stmttmp4B=_tmp48C;void*_tmp48D=_stmttmp4B;struct Cyc_List_List*_tmp48E;struct Cyc_List_List*_tmp490;struct Cyc_Core_Opt*_tmp48F;switch(*((int*)_tmp48D)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2192
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp492=({struct Cyc_Warn_String_Warn_Warg_struct _tmp667;_tmp667.tag=0U,({struct _fat_ptr _tmp8E1=({const char*_tmp493="tcNew: comprehension returned non-array type";_tag_fat(_tmp493,sizeof(char),45U);});_tmp667.f1=_tmp8E1;});_tmp667;});void*_tmp491[1U];_tmp491[0]=& _tmp492;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp491,sizeof(void*),1U));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8E4=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8E3=loc;void*_tmp8E2=res_typ;Cyc_Tcutil_silent_castable(_tmp8E4,_tmp8E3,_tmp8E2,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2203
return res_typ;}case 36U: _LLA: _tmp48F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp48D)->f1;_tmp490=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp48D)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp48F;struct Cyc_List_List*des=_tmp490;
# 2205
({void*_tmp8E5=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->tag=26U,_tmp494->f1=des;_tmp494;});e1->r=_tmp8E5;});
_tmp48E=des;goto _LLD;}case 26U: _LLC: _tmp48E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp48D)->f1;_LLD: {struct Cyc_List_List*des=_tmp48E;
# 2208
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp495=Cyc_Absyn_compress(*topt);void*_stmttmp4C=_tmp495;void*_tmp496=_stmttmp4C;void*_tmp498;void**_tmp497;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp496)->tag == 3U){_LL15: _tmp497=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp496)->f1).elt_type;_tmp498=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp496)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp497;void*zt=_tmp498;
# 2214
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2219
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp49A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp668;_tmp668.tag=0U,({struct _fat_ptr _tmp8E6=({const char*_tmp49B="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp49B,sizeof(char),50U);});_tmp668.f1=_tmp8E6;});_tmp668;});void*_tmp499[1U];_tmp499[0]=& _tmp49A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp499,sizeof(void*),1U));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2228
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8E9=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8E8=loc;void*_tmp8E7=res_typ;Cyc_Tcutil_silent_castable(_tmp8E9,_tmp8E8,_tmp8E7,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2234
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp48D)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2239
void*_tmp49C=({void*_tmp8EE=Cyc_Absyn_char_type;void*_tmp8ED=rgn;struct Cyc_Absyn_Tqual _tmp8EC=Cyc_Absyn_const_tqual(0U);void*_tmp8EB=Cyc_Absyn_fat_bound_type;void*_tmp8EA=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_tmp8EE,_tmp8ED,_tmp8EC,_tmp8EB,_tmp8EA,Cyc_Absyn_false_type);});void*topt2=_tmp49C;
# 2241
void*_tmp49D=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp49D;
return({void*_tmp8F3=t;void*_tmp8F2=rgn;struct Cyc_Absyn_Tqual _tmp8F1=Cyc_Absyn_empty_tqual(0U);void*_tmp8F0=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2242
void*_tmp8EF=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_tmp8F3,_tmp8F2,_tmp8F1,_tmp8F0,_tmp8EF,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2246
void*_tmp49E=({void*_tmp8F8=Cyc_Absyn_wchar_type();void*_tmp8F7=rgn;struct Cyc_Absyn_Tqual _tmp8F6=Cyc_Absyn_const_tqual(0U);void*_tmp8F5=Cyc_Absyn_fat_bound_type;void*_tmp8F4=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_tmp8F8,_tmp8F7,_tmp8F6,_tmp8F5,_tmp8F4,Cyc_Absyn_false_type);});void*topt2=_tmp49E;
# 2248
void*_tmp49F=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp49F;
return({void*_tmp8FD=t;void*_tmp8FC=rgn;struct Cyc_Absyn_Tqual _tmp8FB=Cyc_Absyn_empty_tqual(0U);void*_tmp8FA=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2249
void*_tmp8F9=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_tmp8FD,_tmp8FC,_tmp8FB,_tmp8FA,_tmp8F9,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2255
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp4A0=Cyc_Absyn_compress(*topt);void*_stmttmp4D=_tmp4A0;void*_tmp4A1=_stmttmp4D;struct Cyc_Absyn_Tqual _tmp4A3;void**_tmp4A2;switch(*((int*)_tmp4A1)){case 3U: _LL1A: _tmp4A2=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->f1).elt_type;_tmp4A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->f1).elt_tq;_LL1B: {void**elttype=_tmp4A2;struct Cyc_Absyn_Tqual tq=_tmp4A3;
# 2261
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A1)->f1)->tag == 18U){_LL1C: _LL1D:
# 2265
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2271
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2273
Cyc_Tcexp_check_consume(e1->loc,telt,e1);{
struct Cyc_Absyn_PtrInfo _tmp4A4=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp4A4;
pi.elt_type=telt;
(pi.ptr_atts).rgn=rgn;
({void*_tmp8FE=Cyc_Absyn_bounds_one();(pi.ptr_atts).bounds=_tmp8FE;});
(pi.ptr_atts).zero_term=Cyc_Absyn_false_type;
(pi.ptr_atts).released=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp901=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp900=loc;void*_tmp8FF=res_typ;Cyc_Tcutil_silent_castable(_tmp901,_tmp900,_tmp8FF,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}
# 2287
return res_typ;}}}}}_LL5:;}}
# 2293
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp903=t=({void*_tmp902=t;Cyc_Tcutil_promote_array(_tmp902,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp903;});
return t;}
# 2303
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2307
Cyc_Tcexp_check_consume(e->loc,t,e);{
void*_tmp4A5=e->r;void*_stmttmp4E=_tmp4A5;void*_tmp4A6=_stmttmp4E;switch(*((int*)_tmp4A6)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4A6)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2315
 if(Cyc_Tcutil_is_array_type(t))
({void*_tmp905=t=({void*_tmp904=t;Cyc_Tcutil_promote_array(_tmp904,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp905;});
return t;}_LL0:;}}
# 2328 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*t=({void*_tmp906=Cyc_Absyn_compress(Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0));e->topt=_tmp906;});
# 2332
{void*_tmp4A7=e->r;void*_stmttmp4F=_tmp4A7;void*_tmp4A8=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp4A9;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4A8)->tag == 12U){_LL1: _tmp4A9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4A8)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp4A9;
return t;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2337
void*_tmp4AA=t;void*_tmp4AB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AA)->tag == 3U){_LL6: _tmp4AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AA)->f1).elt_type;_LL7: {void*t2=_tmp4AB;
# 2339
void*_tmp4AC=Cyc_Absyn_compress(t2);void*_stmttmp50=_tmp4AC;void*_tmp4AD=_stmttmp50;struct Cyc_Absyn_FnInfo _tmp4AE;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4AD)->tag == 5U){_LLB: _tmp4AE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4AD)->f1;_LLC: {struct Cyc_Absyn_FnInfo info=_tmp4AE;
# 2341
struct _tuple13 _tmp4AF=({struct _tuple13 _tmp669;({struct Cyc_List_List*_tmp907=Cyc_Tcenv_lookup_type_vars(te);_tmp669.f1=_tmp907;}),_tmp669.f2=Cyc_Core_heap_region;_tmp669;});struct _tuple13 env=_tmp4AF;
struct Cyc_List_List*_tmp4B0=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);struct Cyc_List_List*inst=_tmp4B0;
struct Cyc_List_List*_tmp4B1=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*ts=_tmp4B1;
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp4B2;
({void*_tmp908=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3->tag=13U,_tmp4B3->f1=inner,_tmp4B3->f2=ts;_tmp4B3;});e->r=_tmp908;});
({void*_tmp909=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_tmp909;});
return(void*)_check_null(e->topt);}}else{_LLD: _LLE:
 return t;}_LLA:;}}else{_LL8: _LL9:
# 2350
 return t;}_LL5:;}}}
# 2359
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2361
struct Cyc_List_List*_tmp4B4=0;struct Cyc_List_List*decls=_tmp4B4;
# 2363
{void*_tmp4B5=fn_exp->r;void*_stmttmp51=_tmp4B5;void*_tmp4B6=_stmttmp51;struct Cyc_List_List*_tmp4B7;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B6)->tag == 10U){_LL1: _tmp4B7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B6)->f2;_LL2: {struct Cyc_List_List*es=_tmp4B7;
# 2365
{void*_tmp4B8=e->r;void*_stmttmp52=_tmp4B8;void*_tmp4B9=_stmttmp52;struct Cyc_List_List*_tmp4BA;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B9)->tag == 10U){_LL6: _tmp4BA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LL7: {struct Cyc_List_List*es2=_tmp4BA;
# 2367
struct Cyc_List_List*_tmp4BB=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp4BB;
int _tmp4BC=0;int arg_cnt=_tmp4BC;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66E;_tmp66E.tag=0U,({struct _fat_ptr _tmp90A=({const char*_tmp4C5="bad count ";_tag_fat(_tmp4C5,sizeof(char),11U);});_tmp66E.f1=_tmp90A;});_tmp66E;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp66D;_tmp66D.tag=12U,_tmp66D.f1=arg_cnt;_tmp66D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66C;_tmp66C.tag=0U,({struct _fat_ptr _tmp90B=({const char*_tmp4C4="/";_tag_fat(_tmp4C4,sizeof(char),2U);});_tmp66C.f1=_tmp90B;});_tmp66C;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4C1=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp66B;_tmp66B.tag=12U,_tmp66B.f1=(int)arg_exps->hd;_tmp66B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66A;_tmp66A.tag=0U,({
struct _fat_ptr _tmp90C=({const char*_tmp4C3=" for alias coercion!";_tag_fat(_tmp4C3,sizeof(char),21U);});_tmp66A.f1=_tmp90C;});_tmp66A;});void*_tmp4BD[5U];_tmp4BD[0]=& _tmp4BE,_tmp4BD[1]=& _tmp4BF,_tmp4BD[2]=& _tmp4C0,_tmp4BD[3]=& _tmp4C1,_tmp4BD[4]=& _tmp4C2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4BD,sizeof(void*),5U));});
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2379
struct _tuple12 _tmp4C6=({struct Cyc_Absyn_Exp*_tmp90D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp90D,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp53=_tmp4C6;struct _tuple12 _tmp4C7=_stmttmp53;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Decl*_tmp4C8;_LLB: _tmp4C8=_tmp4C7.f1;_tmp4C9=_tmp4C7.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp4C8;struct Cyc_Absyn_Exp*ve=_tmp4C9;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->hd=d,_tmp4CA->tl=decls;_tmp4CA;});
arg_exps=arg_exps->tl;}}}
# 2384
goto _LL5;}}else{_LL8: _LL9:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66F;_tmp66F.tag=0U,({struct _fat_ptr _tmp90E=({const char*_tmp4CD="not a function call!";_tag_fat(_tmp4CD,sizeof(char),21U);});_tmp66F.f1=_tmp90E;});_tmp66F;});void*_tmp4CB[1U];_tmp4CB[0]=& _tmp4CC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4CB,sizeof(void*),1U));});}_LL5:;}
# 2387
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp670;_tmp670.tag=0U,({struct _fat_ptr _tmp90F=({const char*_tmp4D0="not a function call!";_tag_fat(_tmp4D0,sizeof(char),21U);});_tmp670.f1=_tmp90F;});_tmp670;});void*_tmp4CE[1U];_tmp4CE[0]=& _tmp4CF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4CE,sizeof(void*),1U));});}_LL0:;}
# 2391
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp4D1=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp4D1;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp912=({struct Cyc_Absyn_Decl*_tmp911=d;struct Cyc_Absyn_Stmt*_tmp910=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp911,_tmp910,e->loc);});Cyc_Absyn_stmt_exp(_tmp912,e->loc);});
decls=decls->tl;}
# 2397
e->topt=0;
e->r=fn_exp->r;}
# 2402
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2406
{void*_tmp4D2=e->r;void*_stmttmp54=_tmp4D2;void*_tmp4D3=_stmttmp54;void*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;struct _fat_ptr*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;int*_tmp4DF;struct Cyc_Absyn_Exp**_tmp4DE;void***_tmp4DD;struct Cyc_Absyn_Exp**_tmp4DC;int*_tmp4DB;struct Cyc_Absyn_Enumfield*_tmp4E1;void*_tmp4E0;struct Cyc_Absyn_Enumfield*_tmp4E3;struct Cyc_Absyn_Enumdecl*_tmp4E2;struct Cyc_Absyn_Datatypefield*_tmp4E6;struct Cyc_Absyn_Datatypedecl*_tmp4E5;struct Cyc_List_List*_tmp4E4;struct Cyc_List_List*_tmp4E8;void*_tmp4E7;struct Cyc_Absyn_Aggrdecl**_tmp4EC;struct Cyc_List_List*_tmp4EB;struct Cyc_List_List**_tmp4EA;struct _tuple1**_tmp4E9;int*_tmp4EF;void*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;int*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Vardecl*_tmp4F0;struct Cyc_Absyn_Stmt*_tmp4F4;struct Cyc_List_List*_tmp4F5;struct Cyc_List_List*_tmp4F7;struct _tuple9*_tmp4F6;struct Cyc_List_List*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4F9;int*_tmp4FE;int*_tmp4FD;struct _fat_ptr*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FB;int*_tmp502;int*_tmp501;struct _fat_ptr*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_List_List*_tmp505;void*_tmp504;void*_tmp506;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp**_tmp508;struct Cyc_Absyn_Exp*_tmp50A;enum Cyc_Absyn_Coercion*_tmp50D;struct Cyc_Absyn_Exp*_tmp50C;void*_tmp50B;struct Cyc_List_List*_tmp50F;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Core_Opt*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;enum Cyc_Absyn_Incrementor _tmp51E;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_List_List*_tmp520;enum Cyc_Absyn_Primop _tmp51F;void**_tmp521;union Cyc_Absyn_Cnst*_tmp522;struct Cyc_List_List*_tmp524;struct Cyc_Core_Opt*_tmp523;struct Cyc_Absyn_VarargCallInfo**_tmp527;struct Cyc_List_List*_tmp526;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp528;switch(*((int*)_tmp4D3)){case 12U: _LL1: _tmp528=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp528;
# 2411
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=(void*)_check_null(e2->topt);
return;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D3)->f4 == 0){_LL3: _LL4:
# 2415
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp671;_tmp671.tag=0U,({struct _fat_ptr _tmp913=({const char*_tmp52B="unresolved function: tcExpNoInst";_tag_fat(_tmp52B,sizeof(char),33U);});_tmp671.f1=_tmp913;});_tmp671;});void*_tmp529[1U];_tmp529[0]=& _tmp52A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp529,sizeof(void*),1U));});}else{_LL1B: _tmp525=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp526=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp527=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp525;struct Cyc_List_List*es=_tmp526;struct Cyc_Absyn_VarargCallInfo**vci=_tmp527;
# 2438
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp52C;_push_handler(& _tmp52C);{int _tmp52E=0;if(setjmp(_tmp52C.handler))_tmp52E=1;if(!_tmp52E){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp52D=(void*)Cyc_Core_get_exn_thrown();void*_tmp52F=_tmp52D;void*_tmp530;if(((struct Cyc_Core_Failure_exn_struct*)_tmp52F)->tag == Cyc_Core_Failure){_LL5A: _LL5B:
# 2443
 ok=0;
fn_exp=e;
goto _LL59;}else{_LL5C: _tmp530=_tmp52F;_LL5D: {void*exn=_tmp530;(int)_rethrow(exn);}}_LL59:;}}}
# 2447
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2454
goto _LL0;}}case 36U: _LL5: _tmp523=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp524=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp523;struct Cyc_List_List*des=_tmp524;
# 2419
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp522=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp522;
# 2423
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp521=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LLA: {void**b=_tmp521;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp51F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp520=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp51F;struct Cyc_List_List*es=_tmp520;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp51D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp51E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp51D;enum Cyc_Absyn_Incrementor i=_tmp51E;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp51A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp51B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp51C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp51A;struct Cyc_Core_Opt*popt=_tmp51B;struct Cyc_Absyn_Exp*e2=_tmp51C;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp517=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp518=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp519=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp517;struct Cyc_Absyn_Exp*e2=_tmp518;struct Cyc_Absyn_Exp*e3=_tmp519;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp515=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp516=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp515;struct Cyc_Absyn_Exp*e2=_tmp516;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp513=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp514=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp513;struct Cyc_Absyn_Exp*e2=_tmp514;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp511=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp512=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp511;struct Cyc_Absyn_Exp*e2=_tmp512;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp510=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp510;
# 2455
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp50E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp50F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp50E;struct Cyc_List_List*ts=_tmp50F;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp50B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp50C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp50D=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D3)->f4;_LL22: {void*t1=_tmp50B;struct Cyc_Absyn_Exp*e1=_tmp50C;enum Cyc_Absyn_Coercion*c=_tmp50D;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp50A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp50A;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp508=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp509=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp508;struct Cyc_Absyn_Exp*e1=_tmp509;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp507=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp507;
_tmp506=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17U: _LL29: _tmp506=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL2A: {void*t1=_tmp506;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp504=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp505=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL2C: {void*t1=_tmp504;struct Cyc_List_List*l=_tmp505;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp503=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp503;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp4FF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp500=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp501=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_tmp502=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D3)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp4FF;struct _fat_ptr*f=_tmp500;int*is_tagged=_tmp501;int*is_read=_tmp502;
# 2465
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp4FB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4FC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp4FD=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_tmp4FE=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D3)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp4FB;struct _fat_ptr*f=_tmp4FC;int*is_tagged=_tmp4FD;int*is_read=_tmp4FE;
# 2467
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp4F9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4FA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp4F9;struct Cyc_Absyn_Exp*e2=_tmp4FA;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp4F8=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL36: {struct Cyc_List_List*es=_tmp4F8;
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp4F6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4F7=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL38: {struct _tuple9*t1=_tmp4F6;struct Cyc_List_List*des=_tmp4F7;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp4F5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL3A: {struct Cyc_List_List*des=_tmp4F5;
# 2474
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp531=Cyc_Absyn_compress(*topt);void*_stmttmp55=_tmp531;void*_tmp532=_stmttmp55;void*_tmp535;struct Cyc_Absyn_Tqual*_tmp534;void**_tmp533;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->tag == 4U){_LL5F: _tmp533=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).elt_type;_tmp534=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).tq;_tmp535=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).zero_term;_LL60: {void**et=_tmp533;struct Cyc_Absyn_Tqual*etq=_tmp534;void*zt=_tmp535;
# 2480
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}
# 2486
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp4F4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp4F4;
# 2488
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp4F0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4F1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp4F2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_tmp4F3=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D3)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp4F0;struct Cyc_Absyn_Exp*e1=_tmp4F1;struct Cyc_Absyn_Exp*e2=_tmp4F2;int*iszeroterm=_tmp4F3;
# 2490
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp4ED=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4EE=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp4EF=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp4ED;void*t1=_tmp4EE;int*iszeroterm=_tmp4EF;
# 2492
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp4E9=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4EA=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp4EB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_tmp4EC=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D3)->f4;_LL42: {struct _tuple1**tn=_tmp4E9;struct Cyc_List_List**ts=_tmp4EA;struct Cyc_List_List*args=_tmp4EB;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4EC;
# 2494
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp4E7=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4E8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL44: {void*ts=_tmp4E7;struct Cyc_List_List*args=_tmp4E8;
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp4E4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4E5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_tmp4E6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D3)->f3;_LL46: {struct Cyc_List_List*es=_tmp4E4;struct Cyc_Absyn_Datatypedecl*tud=_tmp4E5;struct Cyc_Absyn_Datatypefield*tuf=_tmp4E6;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp4E2=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4E3=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp4E2;struct Cyc_Absyn_Enumfield*ef=_tmp4E3;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp4E0=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4E1=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL4A: {void*t2=_tmp4E0;struct Cyc_Absyn_Enumfield*ef=_tmp4E1;
t=t2;goto _LL0;}case 34U: _LL4B: _tmp4DB=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1).is_calloc;_tmp4DC=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1).rgn;_tmp4DD=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1).elt_type;_tmp4DE=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1).num_elts;_tmp4DF=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1).fat_result;_LL4C: {int*is_calloc=_tmp4DB;struct Cyc_Absyn_Exp**ropt=_tmp4DC;void***t2=_tmp4DD;struct Cyc_Absyn_Exp**e2=_tmp4DE;int*isfat=_tmp4DF;
# 2500
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp4D9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4DA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp4D9;struct Cyc_Absyn_Exp*e2=_tmp4DA;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp4D7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_tmp4D8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4D3)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp4D7;struct _fat_ptr*f=_tmp4D8;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp4D6=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp4D6;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 42U: _LL53: _tmp4D5=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL54: {struct Cyc_Absyn_Exp*e=_tmp4D5;
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 39U: _LL55: _tmp4D4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4D3)->f1;_LL56: {void*t2=_tmp4D4;
# 2506
if(!te->allow_valueof)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp537=({struct Cyc_Warn_String_Warn_Warg_struct _tmp672;_tmp672.tag=0U,({struct _fat_ptr _tmp914=({const char*_tmp538="valueof(-) can only occur within types";_tag_fat(_tmp538,sizeof(char),39U);});_tmp672.f1=_tmp914;});_tmp672;});void*_tmp536[1U];_tmp536[0]=& _tmp537;({unsigned _tmp915=e->loc;Cyc_Warn_err2(_tmp915,_tag_fat(_tmp536,sizeof(void*),1U));});});
# 2514
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL57: _LL58:
# 2518
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 2520
e->topt=t;}

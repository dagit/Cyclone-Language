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
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 246
extern struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*,int);
# 265
extern void*Cyc_List_find_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 297
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 304
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 857 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 864
void*Cyc_Absyn_compress(void*);
# 877
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 879
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 882
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 884
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 886
extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_wchar_type (void);
void*Cyc_Absyn_gen_float_type(unsigned);
# 889
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 893
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 895
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*,struct Cyc_Absyn_Enumdecl*);
# 912
void*Cyc_Absyn_exn_type (void);
# 920
extern void*Cyc_Absyn_fat_bound_type;
# 922
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 924
void*Cyc_Absyn_bounds_one (void);
# 926
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 930
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 932
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 934
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 938
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 946
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 950
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 954
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 956
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 966
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void);
# 1038
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1049
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1100
void*Cyc_Absyn_pointer_expand(void*,int);
# 1102
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1104
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 1107
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1109
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
# 78
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
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
int Cyc_Tcutil_rgn_of_pointer(void*,void**);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 78
int Cyc_Tcutil_is_zero_pointer_type_elt(void*,void**);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*);
# 106
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*,void*,struct Cyc_List_List*);
# 110
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 112
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 117
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple12{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 120
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*,void*);
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
void Cyc_Tcutil_check_bound(unsigned,unsigned,void*,int);
# 166
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 177
int Cyc_Tcutil_is_noalias_region(void*,int);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*,int);
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
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 231
int Cyc_Tcutil_zeroable_type(void*);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
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
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 76 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*,int);
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
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 146
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 151
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_List_List*);
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 33
void*Cyc_CurRgn_instantiate(void*,void*);struct _tuple16{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 53
extern int Cyc_Evexp_okay_szofarg(void*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned);
# 34
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr);
void Cyc_Tcexp_check_consume(unsigned,void*,struct Cyc_Absyn_Exp*);
# 32
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 79
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C1;_tmp4C1.tag=0,({struct _fat_ptr _tmp5FB=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp4C1.f1=_tmp5FB;});_tmp4C1;});void*_tmp0[1];_tmp0[0]=& _tmp1;({unsigned _tmp5FC=loc;Cyc_Warn_err2(_tmp5FC,_tag_fat(_tmp0,sizeof(void*),1));});});}
# 66
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp3;
void*_tmp4=Cyc_Absyn_new_evar(& Cyc_Kinds_tako,({struct Cyc_Core_Opt*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->v=tenv_tvs;_tmpB;}));void*t2=_tmp4;
void*_tmp5=Cyc_Absyn_new_evar(& Cyc_Kinds_trko,({struct Cyc_Core_Opt*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->v=tenv_tvs;_tmpA;}));void*rt=_tmp5;
void*_tmp6=Cyc_Tcutil_any_bool(tenv_tvs);void*nbl=_tmp6;
void*_tmp7=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp7;
void*_tmp8=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp8;
void*_tmp9=Cyc_Tcutil_any_bool(tenv_tvs);void*rel=_tmp9;
return({struct Cyc_Absyn_PtrInfo _tmp4C2;_tmp4C2.elt_type=t2,({struct Cyc_Absyn_Tqual _tmp5FD=Cyc_Absyn_empty_tqual(0U);_tmp4C2.elt_tq=_tmp5FD;}),(_tmp4C2.ptr_atts).rgn=rt,(_tmp4C2.ptr_atts).nullable=nbl,(_tmp4C2.ptr_atts).bounds=b,(_tmp4C2.ptr_atts).zero_term=zt,(_tmp4C2.ptr_atts).ptrloc=0,(_tmp4C2.ptr_atts).autoreleased=rel;_tmp4C2;});}
# 77
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 81
if(topt == 0){
# 83
({void*_tmp5FE=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->tag=26,_tmpC->f1=des;_tmpC;});e->r=_tmp5FE;});
return;}{
# 86
void*t=*topt;
void*_tmpD=Cyc_Absyn_compress(t);void*_stmttmp0=_tmpD;void*_tmpE=_stmttmp0;struct Cyc_Absyn_Tqual _tmp10;void*_tmpF;union Cyc_Absyn_AggrInfo _tmp11;switch(*((int*)_tmpE)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->f1)== 20){_tmp11=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp11;
# 89
{union Cyc_Absyn_AggrInfo _tmp12=info;void*_tmp13;if((_tmp12.UnknownAggr).tag == 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C3;_tmp4C3.tag=0,({struct _fat_ptr _tmp5FF=({const char*_tmp16="struct type improperly set";_tag_fat(_tmp16,sizeof(char),27U);});_tmp4C3.f1=_tmp5FF;});_tmp4C3;});void*_tmp14[1];_tmp14[0]=& _tmp15;({int(*_tmp601)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;});unsigned _tmp600=loc;_tmp601(_tmp600,_tag_fat(_tmp14,sizeof(void*),1));});});else{_tmp13=*(_tmp12.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp13;
({void*_tmp602=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=29,_tmp17->f1=ad->name,_tmp17->f2=0,_tmp17->f3=des,_tmp17->f4=ad;_tmp17;});e->r=_tmp602;});}};}
# 93
goto _LL0;}}else{goto _LL7;}case 4: _tmpF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE)->f1).elt_type;_tmp10=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE)->f1).tq;{void*at=_tmpF;struct Cyc_Absyn_Tqual aq=_tmp10;
({void*_tmp603=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=26,_tmp18->f1=des;_tmp18;});e->r=_tmp603;});goto _LL0;}case 7:
({void*_tmp604=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=30,_tmp19->f1=t,_tmp19->f2=des;_tmp19;});e->r=_tmp604;});goto _LL0;default: _LL7:
({void*_tmp605=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->tag=26,_tmp1A->f1=des;_tmp1A;});e->r=_tmp605;});goto _LL0;}_LL0:;}}
# 103
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 107
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp1B=e->r;void*_stmttmp1=_tmp1B;void*_tmp1C=_stmttmp1;if(*((int*)_tmp1C)== 4){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1C)->f2 == 0){
# 110
if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C4;_tmp4C4.tag=0,({struct _fat_ptr _tmp606=({const char*_tmp1F="assignment in test";_tag_fat(_tmp1F,sizeof(char),19U);});_tmp4C4.f1=_tmp606;});_tmp4C4;});void*_tmp1D[1];_tmp1D[0]=& _tmp1E;({unsigned _tmp607=e->loc;Cyc_Warn_warn2(_tmp607,_tag_fat(_tmp1D,sizeof(void*),1));});});
goto _LL0;}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 118
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C9;_tmp4C9.tag=0,({struct _fat_ptr _tmp608=({const char*_tmp28="test of ";_tag_fat(_tmp28,sizeof(char),9U);});_tmp4C9.f1=_tmp608;});_tmp4C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C8;_tmp4C8.tag=0,_tmp4C8.f1=msg_part;_tmp4C8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp23=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C7;_tmp4C7.tag=0,({struct _fat_ptr _tmp609=({const char*_tmp27=" has type ";_tag_fat(_tmp27,sizeof(char),11U);});_tmp4C7.f1=_tmp609;});_tmp4C7;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp24=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp4C6;_tmp4C6.tag=3,_tmp4C6.f1=(void*)e->topt;_tmp4C6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C5;_tmp4C5.tag=0,({
struct _fat_ptr _tmp60A=({const char*_tmp26=" instead of integral or pointer type";_tag_fat(_tmp26,sizeof(char),37U);});_tmp4C5.f1=_tmp60A;});_tmp4C5;});void*_tmp20[5];_tmp20[0]=& _tmp21,_tmp20[1]=& _tmp22,_tmp20[2]=& _tmp23,_tmp20[3]=& _tmp24,_tmp20[4]=& _tmp25;({unsigned _tmp60B=e->loc;Cyc_Warn_err2(_tmp60B,_tag_fat(_tmp20,sizeof(void*),5));});});}
# 140 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 145
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
union Cyc_Absyn_Cnst _tmp29=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _stmttmp2=_tmp29;union Cyc_Absyn_Cnst _tmp2A=_stmttmp2;struct _fat_ptr _tmp2B;enum Cyc_Absyn_Sign _tmp2C;int _tmp2D;switch((_tmp2A.String_c).tag){case 2: switch((int)((_tmp2A.Char_c).val).f1){case Cyc_Absyn_Signed:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Unsigned:
 return Cyc_Absyn_uchar_type;default:
 return Cyc_Absyn_char_type;}case 3:
 return Cyc_Absyn_wchar_type();case 4: if(((_tmp2A.Short_c).val).f1 == Cyc_Absyn_Unsigned)
return Cyc_Absyn_ushort_type;else{
return Cyc_Absyn_sshort_type;}case 6: if(((_tmp2A.LongLong_c).val).f1 == Cyc_Absyn_Unsigned)
return Cyc_Absyn_ulonglong_type;else{
return Cyc_Absyn_slonglong_type;}case 7: _tmp2D=((_tmp2A.Float_c).val).f2;{int i=_tmp2D;
# 160
if(topt == 0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_tmp2E=Cyc_Absyn_compress(*topt);void*_stmttmp3=_tmp2E;void*_tmp2F=_stmttmp3;int _tmp30;if(*((int*)_tmp2F)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f1)== 2){_tmp30=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f1)->f1;{int i=_tmp30;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E:
 return Cyc_Absyn_gen_float_type((unsigned)i);};}}case 5: _tmp2C=((_tmp2A.Int_c).val).f1;_tmp2D=((_tmp2A.Int_c).val).f2;{enum Cyc_Absyn_Sign csn=_tmp2C;int i=_tmp2D;
# 170
if(topt == 0)
return(int)csn == 1?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;{
void*_tmp31=Cyc_Absyn_compress(*topt);void*_stmttmp4=_tmp31;void*_tmp32=_stmttmp4;void*_tmp33;enum Cyc_Absyn_Sign _tmp34;switch(*((int*)_tmp32)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32)->f1)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32)->f1)->f2){case Cyc_Absyn_Char_sz: _tmp34=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32)->f1)->f1;{enum Cyc_Absyn_Sign sn=_tmp34;
# 174
({union Cyc_Absyn_Cnst _tmp60C=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp60C;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _tmp34=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32)->f1)->f1;{enum Cyc_Absyn_Sign sn=_tmp34;
# 177
({union Cyc_Absyn_Cnst _tmp60D=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp60D;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _tmp34=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32)->f1)->f1;{enum Cyc_Absyn_Sign sn=_tmp34;
_tmp34=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _tmp34=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp34;
# 183
({union Cyc_Absyn_Cnst _tmp60E=Cyc_Absyn_Int_c(sn,i);*c=_tmp60E;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32)->f2 != 0){_tmp33=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32)->f2)->hd;{void*t1=_tmp33;
# 192
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=9,({struct Cyc_Absyn_Exp*_tmp60F=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp35->f1=_tmp60F;});_tmp35;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3: if(i == 0){
# 187
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp610=({union Cyc_Absyn_Cnst _tmp4CA;(_tmp4CA.Null_c).tag=1U,(_tmp4CA.Null_c).val=0;_tmp4CA;});*c=_tmp610;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}else{goto _LL2D;}default: _LL2D:
# 194
 return(int)csn == 1?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;};}}case 1:
# 197
 if(topt != 0){
void*_tmp36=Cyc_Absyn_compress(*topt);void*_stmttmp5=_tmp36;void*_tmp37=_stmttmp5;void*_tmp38;if(*((int*)_tmp37)== 3){_tmp38=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37)->f1).ptr_atts).nullable;{void*nbl=_tmp38;
# 200
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CC;_tmp4CC.tag=0,({struct _fat_ptr _tmp611=({const char*_tmp3C="Used NULL when expecting a value of type ";_tag_fat(_tmp3C,sizeof(char),42U);});_tmp4CC.f1=_tmp611;});_tmp4CC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4CB;_tmp4CB.tag=2,_tmp4CB.f1=(void*)*topt;_tmp4CB;});void*_tmp39[2];_tmp39[0]=& _tmp3A,_tmp39[1]=& _tmp3B;({unsigned _tmp612=e->loc;Cyc_Warn_err2(_tmp612,_tag_fat(_tmp39,sizeof(void*),2));});});
return*topt;}}else{
goto _LL2F;}_LL2F:;}{
# 205
struct Cyc_Absyn_PtrInfo _tmp3D=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp3D;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
return Cyc_Absyn_pointer_type(pi);}case 8: _tmp2B=(_tmp2A.String_c).val;{struct _fat_ptr s=_tmp2B;
# 210
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp2B=s;goto _LL1A;}default: _tmp2B=(_tmp2A.Wstring_c).val;_LL1A: {struct _fat_ptr s=_tmp2B;
# 213
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 217
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp613=Cyc_Absyn_Int_c(1U,string_numelts);Cyc_Absyn_const_exp(_tmp613,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 222
void*_tmp3E=({void*_tmp618=string_elt_typ;void*_tmp617=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp616=Cyc_Absyn_const_tqual(0U);void*_tmp615=
Cyc_Absyn_thin_bounds_exp(elen);
# 222
void*_tmp614=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_tmp618,_tmp617,_tmp616,_tmp615,_tmp614,Cyc_Absyn_false_type);});void*t=_tmp3E;
# 224
if(topt == 0)
return t;{
void*_tmp3F=Cyc_Absyn_compress(*topt);void*_stmttmp6=_tmp3F;void*_tmp40=_stmttmp6;struct Cyc_Absyn_Tqual _tmp41;switch(*((int*)_tmp40)){case 4: _tmp41=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40)->f1).tq;{struct Cyc_Absyn_Tqual tq=_tmp41;
# 230
return({void*_tmp61B=string_elt_typ;struct Cyc_Absyn_Tqual _tmp61A=tq;struct Cyc_Absyn_Exp*_tmp619=elen;Cyc_Absyn_array_type(_tmp61B,_tmp61A,_tmp619,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3:
# 235
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp61E=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp61D=loc;void*_tmp61C=t;Cyc_Tcutil_silent_castable(_tmp61E,_tmp61D,_tmp61C,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
return*topt;}
# 240
return t;default:
 return t;};}}}}};}
# 247
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp42=*((void**)_check_null(b));void*_stmttmp7=_tmp42;void*_tmp43=_stmttmp7;void*_tmp44;switch(*((int*)_tmp43)){case 0: _tmp44=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp43)->f1;{struct _tuple1*q=_tmp44;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp46=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CD;_tmp4CD.tag=0,({struct _fat_ptr _tmp61F=({const char*_tmp47="unresolved binding in tcVar";_tag_fat(_tmp47,sizeof(char),28U);});_tmp4CD.f1=_tmp61F;});_tmp4CD;});void*_tmp45[1];_tmp45[0]=& _tmp46;({int(*_tmp621)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;});unsigned _tmp620=loc;_tmp621(_tmp620,_tag_fat(_tmp45,sizeof(void*),1));});});}case 1: _tmp44=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp43)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp44;
# 253
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2: _tmp44=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp43)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp44;
# 260
if(fd->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5: _tmp44=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp43)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp44;
_tmp44=vd;goto _LLA;}case 4: _tmp44=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp43)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp44;
_tmp44=vd;goto _LLC;}default: _tmp44=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp43)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp44;
# 266
if(te->allow_valueof){
void*_tmp48=Cyc_Absyn_compress(vd->type);void*_stmttmp8=_tmp48;void*_tmp49=_stmttmp8;void*_tmp4A;if(*((int*)_tmp49)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f2 != 0){_tmp4A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f2)->hd;{void*i=_tmp4A;
({void*_tmp622=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=39,_tmp4B->f1=i;_tmp4B;});e->r=_tmp622;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10:
 goto _LLD;}_LLD:;}
# 271
return vd->type;}};}
# 275
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 282
void*_tmp4C=fmt->r;void*_stmttmp9=_tmp4C;void*_tmp4D=_stmttmp9;struct _fat_ptr _tmp4E;switch(*((int*)_tmp4D)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4D)->f1).String_c).tag == 8){_tmp4E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4D)->f1).String_c).val;{struct _fat_ptr s=_tmp4E;
_tmp4E=s;goto _LL4;}}else{goto _LL5;}case 14: if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D)->f2)->r)== 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D)->f2)->r)->f1).String_c).tag == 8){_tmp4E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D)->f2)->r)->f1).String_c).val;_LL4: {struct _fat_ptr s=_tmp4E;
# 285
struct Cyc_List_List*_tmp4F=type_getter(te,(struct _fat_ptr)s,isCproto,fmt->loc);struct Cyc_List_List*desc_types=_tmp4F;
if(opt_args == 0)
return;{
struct Cyc_List_List*_tmp50=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp50;
# 290
for(1;desc_types != 0 && args != 0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp624=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp623=e;Cyc_Tcutil_coerce_arg(_tmp624,_tmp623,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D1;_tmp4D1.tag=0,({struct _fat_ptr _tmp625=({const char*_tmp57="descriptor has type ";_tag_fat(_tmp57,sizeof(char),21U);});_tmp4D1.f1=_tmp625;});_tmp4D1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4D0;_tmp4D0.tag=2,_tmp4D0.f1=(void*)t;_tmp4D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp54=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CF;_tmp4CF.tag=0,({
struct _fat_ptr _tmp626=({const char*_tmp56=" but argument has type ";_tag_fat(_tmp56,sizeof(char),24U);});_tmp4CF.f1=_tmp626;});_tmp4CF;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp55=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp4CE;_tmp4CE.tag=3,_tmp4CE.f1=(void*)e->topt;_tmp4CE;});void*_tmp51[4];_tmp51[0]=& _tmp52,_tmp51[1]=& _tmp53,_tmp51[2]=& _tmp54,_tmp51[3]=& _tmp55;({unsigned _tmp627=e->loc;Cyc_Tcexp_err_and_explain(_tmp627,_tag_fat(_tmp51,sizeof(void*),4));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp628=({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->hd=(void*)arg_cnt,_tmp58->tl=*alias_arg_exps;_tmp58;});*alias_arg_exps=_tmp628;});
Cyc_Tcexp_check_consume(e->loc,t,e);}
# 304
if(desc_types != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D2;_tmp4D2.tag=0,({struct _fat_ptr _tmp629=({const char*_tmp5B="too few arguments";_tag_fat(_tmp5B,sizeof(char),18U);});_tmp4D2.f1=_tmp629;});_tmp4D2;});void*_tmp59[1];_tmp59[0]=& _tmp5A;({unsigned _tmp62A=fmt->loc;Cyc_Warn_err2(_tmp62A,_tag_fat(_tmp59,sizeof(void*),1));});});
if(args != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D3;_tmp4D3.tag=0,({struct _fat_ptr _tmp62B=({const char*_tmp5E="too many arguments";_tag_fat(_tmp5E,sizeof(char),19U);});_tmp4D3.f1=_tmp62B;});_tmp4D3;});void*_tmp5C[1];_tmp5C[0]=& _tmp5D;({unsigned _tmp62C=((struct Cyc_Absyn_Exp*)args->hd)->loc;Cyc_Warn_err2(_tmp62C,_tag_fat(_tmp5C,sizeof(void*),1));});});
# 309
for(1;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}
# 312
return;}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
# 316
 if(opt_args == 0)
return;
{struct Cyc_List_List*_tmp5F=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp5F;for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)args->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt),(struct Cyc_Absyn_Exp*)args->hd);}}
# 322
return;};}
# 326
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 328
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp60=p;switch((int)_tmp60){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Minus: _LL4:
# 332
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D5;_tmp4D5.tag=0,({struct _fat_ptr _tmp62D=({const char*_tmp64="expecting numeric type but found ";_tag_fat(_tmp64,sizeof(char),34U);});_tmp4D5.f1=_tmp62D;});_tmp4D5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4D4;_tmp4D4.tag=2,_tmp4D4.f1=(void*)t;_tmp4D4;});void*_tmp61[2];_tmp61[0]=& _tmp62,_tmp61[1]=& _tmp63;({unsigned _tmp62E=loc;Cyc_Warn_err2(_tmp62E,_tag_fat(_tmp61,sizeof(void*),2));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not:
# 336
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp66=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D7;_tmp4D7.tag=0,({struct _fat_ptr _tmp62F=({const char*_tmp68="expecting integral or * type but found ";_tag_fat(_tmp68,sizeof(char),40U);});_tmp4D7.f1=_tmp62F;});_tmp4D7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp67=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4D6;_tmp4D6.tag=2,_tmp4D6.f1=(void*)t;_tmp4D6;});void*_tmp65[2];_tmp65[0]=& _tmp66,_tmp65[1]=& _tmp67;({unsigned _tmp630=loc;Cyc_Warn_err2(_tmp630,_tag_fat(_tmp65,sizeof(void*),2));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot:
# 341
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D9;_tmp4D9.tag=0,({struct _fat_ptr _tmp631=({const char*_tmp6C="expecting integral type but found ";_tag_fat(_tmp6C,sizeof(char),35U);});_tmp4D9.f1=_tmp631;});_tmp4D9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4D8;_tmp4D8.tag=2,_tmp4D8.f1=(void*)t;_tmp4D8;});void*_tmp69[2];_tmp69[0]=& _tmp6A,_tmp69[1]=& _tmp6B;({unsigned _tmp632=loc;Cyc_Warn_err2(_tmp632,_tag_fat(_tmp69,sizeof(void*),2));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts:
# 345
{void*_tmp6D=t;void*_tmp6E;if(*((int*)_tmp6D)== 3){_tmp6E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).bounds;{void*b=_tmp6E;
# 347
struct Cyc_Absyn_Exp*_tmp6F=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp6F;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp71=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DA;_tmp4DA.tag=0,({struct _fat_ptr _tmp633=({const char*_tmp72="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp72,sizeof(char),53U);});_tmp4DA.f1=_tmp633;});_tmp4DA;});void*_tmp70[1];_tmp70[0]=& _tmp71;({unsigned _tmp634=loc;Cyc_Warn_err2(_tmp634,_tag_fat(_tmp70,sizeof(void*),1));});});
goto _LLD;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DC;_tmp4DC.tag=0,({struct _fat_ptr _tmp635=({const char*_tmp76="numelts requires pointer type, not ";_tag_fat(_tmp76,sizeof(char),36U);});_tmp4DC.f1=_tmp635;});_tmp4DC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp75=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4DB;_tmp4DB.tag=2,_tmp4DB.f1=(void*)t;_tmp4DB;});void*_tmp73[2];_tmp73[0]=& _tmp74,_tmp73[1]=& _tmp75;({unsigned _tmp636=loc;Cyc_Warn_err2(_tmp636,_tag_fat(_tmp73,sizeof(void*),2));});});}_LLD:;}
# 353
return Cyc_Absyn_uint_type;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp78=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DD;_tmp4DD.tag=0,({struct _fat_ptr _tmp637=({const char*_tmp79="Non-unary primop";_tag_fat(_tmp79,sizeof(char),17U);});_tmp4DD.f1=_tmp637;});_tmp4DD;});void*_tmp77[1];_tmp77[0]=& _tmp78;({int(*_tmp639)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;});unsigned _tmp638=loc;_tmp639(_tmp638,_tag_fat(_tmp77,sizeof(void*),1));});});};}
# 360
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp7A=(void*)_check_null(e1->topt);void*t1=_tmp7A;
void*_tmp7B=(void*)_check_null(e2->topt);void*t2=_tmp7B;
void*_tmp7C=Cyc_Tcutil_max_arithmetic_type(t1,t2);void*new_typ=_tmp7C;
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,1U);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,1U);
return new_typ;}
# 370
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 373
if(!checker(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E0;_tmp4E0.tag=0,({struct _fat_ptr _tmp63A=({const char*_tmp82="type ";_tag_fat(_tmp82,sizeof(char),6U);});_tmp4E0.f1=_tmp63A;});_tmp4E0;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp4DF;_tmp4DF.tag=3,_tmp4DF.f1=(void*)e1->topt;_tmp4DF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp80=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DE;_tmp4DE.tag=0,({struct _fat_ptr _tmp63B=({const char*_tmp81=" cannot be used here";_tag_fat(_tmp81,sizeof(char),21U);});_tmp4DE.f1=_tmp63B;});_tmp4DE;});void*_tmp7D[3];_tmp7D[0]=& _tmp7E,_tmp7D[1]=& _tmp7F,_tmp7D[2]=& _tmp80;({struct Cyc_Tcenv_Tenv*_tmp63D=te;unsigned _tmp63C=e1->loc;Cyc_Tcexp_expr_err(_tmp63D,_tmp63C,0,_tag_fat(_tmp7D,sizeof(void*),3));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E3;_tmp4E3.tag=0,({struct _fat_ptr _tmp63E=({const char*_tmp88="type ";_tag_fat(_tmp88,sizeof(char),6U);});_tmp4E3.f1=_tmp63E;});_tmp4E3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp85=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp4E2;_tmp4E2.tag=3,_tmp4E2.f1=(void*)e2->topt;_tmp4E2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp86=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E1;_tmp4E1.tag=0,({struct _fat_ptr _tmp63F=({const char*_tmp87=" cannot be used here";_tag_fat(_tmp87,sizeof(char),21U);});_tmp4E1.f1=_tmp63F;});_tmp4E1;});void*_tmp83[3];_tmp83[0]=& _tmp84,_tmp83[1]=& _tmp85,_tmp83[2]=& _tmp86;({struct Cyc_Tcenv_Tenv*_tmp641=te;unsigned _tmp640=e2->loc;Cyc_Tcexp_expr_err(_tmp641,_tmp640,0,_tag_fat(_tmp83,sizeof(void*),3));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 380
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
void*_tmp89=t1;void*_tmp90;void*_tmp8F;void*_tmp8E;void*_tmp8D;void*_tmp8C;struct Cyc_Absyn_Tqual _tmp8B;void*_tmp8A;if(*((int*)_tmp89)== 3){_tmp8A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89)->f1).elt_type;_tmp8B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89)->f1).elt_tq;_tmp8C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89)->f1).ptr_atts).rgn;_tmp8D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89)->f1).ptr_atts).nullable;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89)->f1).ptr_atts).bounds;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89)->f1).ptr_atts).zero_term;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89)->f1).ptr_atts).autoreleased;{void*et=_tmp8A;struct Cyc_Absyn_Tqual tq=_tmp8B;void*r=_tmp8C;void*n=_tmp8D;void*b=_tmp8E;void*zt=_tmp8F;void*rel=_tmp90;
# 385
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp92=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E4;_tmp4E4.tag=0,({struct _fat_ptr _tmp642=({const char*_tmp93="can't do arithmetic on abstract pointer type";_tag_fat(_tmp93,sizeof(char),45U);});_tmp4E4.f1=_tmp642;});_tmp4E4;});void*_tmp91[1];_tmp91[0]=& _tmp92;({unsigned _tmp643=e1->loc;Cyc_Warn_err2(_tmp643,_tag_fat(_tmp91,sizeof(void*),1));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp95=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E5;_tmp4E5.tag=0,({struct _fat_ptr _tmp644=({const char*_tmp96="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmp96,sizeof(char),49U);});_tmp4E5.f1=_tmp644;});_tmp4E5;});void*_tmp94[1];_tmp94[0]=& _tmp95;({unsigned _tmp645=e1->loc;Cyc_Warn_err2(_tmp645,_tag_fat(_tmp94,sizeof(void*),1));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp98=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E7;_tmp4E7.tag=0,({struct _fat_ptr _tmp646=({const char*_tmp9A="expecting int but found ";_tag_fat(_tmp9A,sizeof(char),25U);});_tmp4E7.f1=_tmp646;});_tmp4E7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp99=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4E6;_tmp4E6.tag=2,_tmp4E6.f1=(void*)t2;_tmp4E6;});void*_tmp97[2];_tmp97[0]=& _tmp98,_tmp97[1]=& _tmp99;({unsigned _tmp647=e2->loc;Cyc_Warn_err2(_tmp647,_tag_fat(_tmp97,sizeof(void*),2));});});{
struct Cyc_Absyn_Exp*_tmp9B=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp9B;
if(eopt == 0)
return t1;
# 396
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmp9C=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmp9C;struct _tuple16 _tmp9D=_stmttmpA;int _tmp9F;unsigned _tmp9E;_tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;{unsigned i=_tmp9E;int known=_tmp9F;
if(known && i == 1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E8;_tmp4E8.tag=0,({struct _fat_ptr _tmp648=({const char*_tmpA2="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpA2,sizeof(char),70U);});_tmp4E8.f1=_tmp648;});_tmp4E8;});void*_tmpA0[1];_tmpA0[0]=& _tmpA1;({unsigned _tmp649=e1->loc;Cyc_Warn_warn2(_tmp649,_tag_fat(_tmpA0,sizeof(void*),1));});});}}{
# 407
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA3=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=3,(_tmpA4->f1).elt_type=et,(_tmpA4->f1).elt_tq=tq,((_tmpA4->f1).ptr_atts).rgn=r,((_tmpA4->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpA4->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpA4->f1).ptr_atts).zero_term=zt,((_tmpA4->f1).ptr_atts).ptrloc=0,((_tmpA4->f1).ptr_atts).autoreleased=rel;_tmpA4;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpA3;
# 410
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,3U);
return(void*)new_t1;}}}}else{
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);};}
# 415
static void*Cyc_Tcexp_relqual(void*t){
void*_tmpA5=t;void*_tmpA6;if(*((int*)_tmpA5)== 3){_tmpA6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).autoreleased;{void*rel=_tmpA6;
# 418
return rel;}}else{
return Cyc_Absyn_false_type;};}
# 424
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4EE;_tmp4EE.tag=0,({struct _fat_ptr _tmp64A=({const char*_tmpB1="pointer arithmetic on values of different ";_tag_fat(_tmpB1,sizeof(char),43U);});_tmp4EE.f1=_tmp64A;});_tmp4EE;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4ED;_tmp4ED.tag=0,({
struct _fat_ptr _tmp64B=({const char*_tmpB0="types (";_tag_fat(_tmpB0,sizeof(char),8U);});_tmp4ED.f1=_tmp64B;});_tmp4ED;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4EC;_tmp4EC.tag=2,_tmp4EC.f1=(void*)t1;_tmp4EC;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4EB;_tmp4EB.tag=0,({struct _fat_ptr _tmp64C=({const char*_tmpAF=" != ";_tag_fat(_tmpAF,sizeof(char),5U);});_tmp4EB.f1=_tmp64C;});_tmp4EB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpAC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4EA;_tmp4EA.tag=2,_tmp4EA.f1=(void*)t2;_tmp4EA;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E9;_tmp4E9.tag=0,({struct _fat_ptr _tmp64D=({const char*_tmpAE=")";_tag_fat(_tmpAE,sizeof(char),2U);});_tmp4E9.f1=_tmp64D;});_tmp4E9;});void*_tmpA7[6];_tmpA7[0]=& _tmpA8,_tmpA7[1]=& _tmpA9,_tmpA7[2]=& _tmpAA,_tmpA7[3]=& _tmpAB,_tmpA7[4]=& _tmpAC,_tmpA7[5]=& _tmpAD;({unsigned _tmp64E=e1->loc;Cyc_Tcexp_err_and_explain(_tmp64E,_tag_fat(_tmpA7,sizeof(void*),6));});});
return Cyc_Absyn_sint_type;}
# 436
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp64F=t1_elt;Cyc_Unify_unify(_tmp64F,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F4;_tmp4F4.tag=0,({struct _fat_ptr _tmp650=({const char*_tmpBC="pointer arithmetic on values of different ";_tag_fat(_tmpBC,sizeof(char),43U);});_tmp4F4.f1=_tmp650;});_tmp4F4;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F3;_tmp4F3.tag=0,({
struct _fat_ptr _tmp651=({const char*_tmpBB="types(";_tag_fat(_tmpBB,sizeof(char),7U);});_tmp4F3.f1=_tmp651;});_tmp4F3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4F2;_tmp4F2.tag=2,_tmp4F2.f1=(void*)t1;_tmp4F2;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F1;_tmp4F1.tag=0,({struct _fat_ptr _tmp652=({const char*_tmpBA=" != ";_tag_fat(_tmpBA,sizeof(char),5U);});_tmp4F1.f1=_tmp652;});_tmp4F1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4F0;_tmp4F0.tag=2,_tmp4F0.f1=(void*)t2;_tmp4F0;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4EF;_tmp4EF.tag=0,({struct _fat_ptr _tmp653=({const char*_tmpB9=")";_tag_fat(_tmpB9,sizeof(char),2U);});_tmp4EF.f1=_tmp653;});_tmp4EF;});void*_tmpB2[6];_tmpB2[0]=& _tmpB3,_tmpB2[1]=& _tmpB4,_tmpB2[2]=& _tmpB5,_tmpB2[3]=& _tmpB6,_tmpB2[4]=& _tmpB7,_tmpB2[5]=& _tmpB8;({unsigned _tmp654=e1->loc;Cyc_Tcexp_err_and_explain(_tmp654,_tag_fat(_tmpB2,sizeof(void*),6));});});
# 441
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F5;_tmp4F5.tag=0,({struct _fat_ptr _tmp655=({const char*_tmpBF="coercing fat pointer to thin pointer for subtraction";_tag_fat(_tmpBF,sizeof(char),53U);});_tmp4F5.f1=_tmp655;});_tmp4F5;});void*_tmpBD[1];_tmpBD[0]=& _tmpBE;({unsigned _tmp656=e1->loc;Cyc_Warn_warn2(_tmp656,_tag_fat(_tmpBD,sizeof(void*),1));});});
({struct Cyc_Absyn_Exp*_tmp65B=e1;Cyc_Tcutil_unchecked_cast(_tmp65B,({void*_tmp65A=t1_elt;void*_tmp659=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp658=
Cyc_Absyn_empty_tqual(0U);
# 442
void*_tmp657=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_tmp65A,_tmp659,_tmp658,_tmp657,
Cyc_Tcexp_relqual(t1));}),3U);});
# 445
return Cyc_Absyn_sint_type;}
# 447
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F6;_tmp4F6.tag=0,({struct _fat_ptr _tmp65C=({const char*_tmpC2="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpC2,sizeof(char),50U);});_tmp4F6.f1=_tmp65C;});_tmp4F6;});void*_tmpC0[1];_tmpC0[0]=& _tmpC1;({unsigned _tmp65D=e1->loc;Cyc_Warn_err2(_tmp65D,_tag_fat(_tmpC0,sizeof(void*),1));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F7;_tmp4F7.tag=0,({struct _fat_ptr _tmp65E=({const char*_tmpC5="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpC5,sizeof(char),54U);});_tmp4F7.f1=_tmp65E;});_tmp4F7;});void*_tmpC3[1];_tmpC3[0]=& _tmpC4;({unsigned _tmp65F=e1->loc;Cyc_Warn_err2(_tmp65F,_tag_fat(_tmpC3,sizeof(void*),1));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FB;_tmp4FB.tag=0,({struct _fat_ptr _tmp660=({const char*_tmpCC="expecting ";_tag_fat(_tmpCC,sizeof(char),11U);});_tmp4FB.f1=_tmp660;});_tmp4FB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpC8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4FA;_tmp4FA.tag=2,_tmp4FA.f1=(void*)t1;_tmp4FA;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F9;_tmp4F9.tag=0,({struct _fat_ptr _tmp661=({const char*_tmpCB=" or int but found ";_tag_fat(_tmpCB,sizeof(char),19U);});_tmp4F9.f1=_tmp661;});_tmp4F9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4F8;_tmp4F8.tag=2,_tmp4F8.f1=(void*)t2;_tmp4F8;});void*_tmpC6[4];_tmpC6[0]=& _tmpC7,_tmpC6[1]=& _tmpC8,_tmpC6[2]=& _tmpC9,_tmpC6[3]=& _tmpCA;({unsigned _tmp662=e2->loc;Cyc_Tcexp_err_and_explain(_tmp662,_tag_fat(_tmpC6,sizeof(void*),4));});});
return t1;}
# 456
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp663=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp663,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FC;_tmp4FC.tag=0,({struct _fat_ptr _tmp664=({const char*_tmpCF="coercing fat pointer to thin pointer for subtraction";_tag_fat(_tmpCF,sizeof(char),53U);});_tmp4FC.f1=_tmp664;});_tmp4FC;});void*_tmpCD[1];_tmpCD[0]=& _tmpCE;({unsigned _tmp665=e1->loc;Cyc_Warn_warn2(_tmp665,_tag_fat(_tmpCD,sizeof(void*),1));});});
({struct Cyc_Absyn_Exp*_tmp66A=e2;Cyc_Tcutil_unchecked_cast(_tmp66A,({void*_tmp669=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp668=
Cyc_Tcutil_pointer_region(t2);
# 461
struct Cyc_Absyn_Tqual _tmp667=
# 463
Cyc_Absyn_empty_tqual(0U);
# 461
void*_tmp666=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_tmp669,_tmp668,_tmp667,_tmp666,
# 463
Cyc_Tcexp_relqual(t2));}),3U);});}
# 466
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FD;_tmp4FD.tag=0,({struct _fat_ptr _tmp66B=({const char*_tmpD2="thin pointer subtraction!";_tag_fat(_tmpD2,sizeof(char),26U);});_tmp4FD.f1=_tmp66B;});_tmp4FD;});void*_tmpD0[1];_tmpD0[0]=& _tmpD1;({unsigned _tmp66C=e1->loc;Cyc_Warn_warn2(_tmp66C,_tag_fat(_tmpD0,sizeof(void*),1));});});
return Cyc_Absyn_sint_type;}
# 469
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FE;_tmp4FE.tag=0,({struct _fat_ptr _tmp66D=({const char*_tmpD5="coercing thin pointer to integer for subtraction";_tag_fat(_tmpD5,sizeof(char),49U);});_tmp4FE.f1=_tmp66D;});_tmp4FE;});void*_tmpD3[1];_tmpD3[0]=& _tmpD4;({unsigned _tmp66E=e1->loc;Cyc_Warn_warn2(_tmp66E,_tag_fat(_tmpD3,sizeof(void*),1));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,3U);}
# 472
if(Cyc_Tcutil_is_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF;_tmp4FF.tag=0,({struct _fat_ptr _tmp66F=({const char*_tmpD8="coercing pointer to integer for subtraction";_tag_fat(_tmpD8,sizeof(char),44U);});_tmp4FF.f1=_tmp66F;});_tmp4FF;});void*_tmpD6[1];_tmpD6[0]=& _tmpD7;({unsigned _tmp670=e1->loc;Cyc_Warn_warn2(_tmp670,_tag_fat(_tmpD6,sizeof(void*),1));});});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,3U);}
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
if((int)(Cyc_Tcutil_type_kind(t1))->kind == 2 ||({
void*_tmp671=t1;Cyc_Unify_unify(_tmp671,Cyc_Absyn_new_evar(& Cyc_Kinds_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 493
if(({struct Cyc_RgnOrder_RgnPO*_tmp674=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp673=loc;void*_tmp672=t2;Cyc_Tcutil_silent_castable(_tmp674,_tmp673,_tmp672,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,3U);
return Cyc_Absyn_sint_type;}
# 497
if(({struct Cyc_RgnOrder_RgnPO*_tmp677=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp676=loc;void*_tmp675=t1;Cyc_Tcutil_silent_castable(_tmp677,_tmp676,_tmp675,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,3U);
return Cyc_Absyn_sint_type;}
# 501
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 506
{struct _tuple0 _tmpD9=({struct _tuple0 _tmp500;({void*_tmp679=Cyc_Absyn_compress(t1);_tmp500.f1=_tmp679;}),({void*_tmp678=Cyc_Absyn_compress(t2);_tmp500.f2=_tmp678;});_tmp500;});struct _tuple0 _stmttmpB=_tmpD9;struct _tuple0 _tmpDA=_stmttmpB;void*_tmpDC;void*_tmpDB;switch(*((int*)_tmpDA.f1)){case 3: if(*((int*)_tmpDA.f2)== 3){_tmpDB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDA.f1)->f1).elt_type;_tmpDC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDA.f2)->f1).elt_type;{void*t1a=_tmpDB;void*t2a=_tmpDC;
# 508
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpDA.f1)->f1)== 3){if(*((int*)_tmpDA.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpDA.f2)->f1)== 3)
# 512
return Cyc_Absyn_sint_type;else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
 goto _LL0;}_LL0:;}
# 516
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp504;_tmp504.tag=0,({struct _fat_ptr _tmp67A=({const char*_tmpE3="comparison not allowed between ";_tag_fat(_tmpE3,sizeof(char),32U);});_tmp504.f1=_tmp67A;});_tmp504;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpDF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp503;_tmp503.tag=2,_tmp503.f1=(void*)t1;_tmp503;});struct Cyc_Warn_String_Warn_Warg_struct _tmpE0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp502;_tmp502.tag=0,({struct _fat_ptr _tmp67B=({const char*_tmpE2=" and ";_tag_fat(_tmpE2,sizeof(char),6U);});_tmp502.f1=_tmp67B;});_tmp502;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp501;_tmp501.tag=2,_tmp501.f1=(void*)t2;_tmp501;});void*_tmpDD[4];_tmpDD[0]=& _tmpDE,_tmpDD[1]=& _tmpDF,_tmpDD[2]=& _tmpE0,_tmpDD[3]=& _tmpE1;({unsigned _tmp67C=loc;Cyc_Tcexp_err_and_explain(_tmp67C,_tag_fat(_tmpDD,sizeof(void*),4));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 522
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 524
enum Cyc_Absyn_Primop _tmpE4=p;switch((int)_tmpE4){case Cyc_Absyn_Plus:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times:
# 528
 goto _LL8;case Cyc_Absyn_Div: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod:
# 531
 goto _LLC;case Cyc_Absyn_Bitand: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
# 540
 goto _LL18;case Cyc_Absyn_Neq: _LL18:
 goto _LL1A;case Cyc_Absyn_Gt: _LL1A:
 goto _LL1C;case Cyc_Absyn_Lt: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL20:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);default:
# 547
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp505;_tmp505.tag=0,({struct _fat_ptr _tmp67D=({const char*_tmpE7="bad binary primop";_tag_fat(_tmpE7,sizeof(char),18U);});_tmp505.f1=_tmp67D;});_tmp505;});void*_tmpE5[1];_tmpE5[0]=& _tmpE6;({int(*_tmp67E)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp67E(_tag_fat(_tmpE5,sizeof(void*),1));});});};}
# 551
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 559
if((int)p == 2 &&({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(es)== 1){
struct Cyc_Absyn_Exp*_tmpE8=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpE8;
void*_tmpE9=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpE9;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp507;_tmp507.tag=0,({struct _fat_ptr _tmp67F=({const char*_tmpED="expecting numeric type but found ";_tag_fat(_tmpED,sizeof(char),34U);});_tmp507.f1=_tmp67F;});_tmp507;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpEC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp506;_tmp506.tag=2,_tmp506.f1=(void*)t;_tmp506;});void*_tmpEA[2];_tmpEA[0]=& _tmpEB,_tmpEA[1]=& _tmpEC;({unsigned _tmp680=e->loc;Cyc_Warn_err2(_tmp680,_tag_fat(_tmpEA,sizeof(void*),2));});});
return t;}
# 566
{struct Cyc_List_List*_tmpEE=es;struct Cyc_List_List*es2=_tmpEE;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _tmpEF=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(es);int _stmttmpC=_tmpEF;int _tmpF0=_stmttmpC;switch((int)_tmpF0){case 0:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp508;_tmp508.tag=0,({struct _fat_ptr _tmp681=({const char*_tmpF3="primitive operator has 0 arguments";_tag_fat(_tmpF3,sizeof(char),35U);});_tmp508.f1=_tmp681;});_tmp508;});void*_tmpF1[1];_tmpF1[0]=& _tmpF2;({struct Cyc_Tcenv_Tenv*_tmp684=te;unsigned _tmp683=loc;void**_tmp682=topt;Cyc_Tcexp_expr_err(_tmp684,_tmp683,_tmp682,_tag_fat(_tmpF1,sizeof(void*),1));});});case 1:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);case 2:
 return({struct Cyc_Tcenv_Tenv*_tmp689=te;unsigned _tmp688=loc;void**_tmp687=topt;enum Cyc_Absyn_Primop _tmp686=p;struct Cyc_Absyn_Exp*_tmp685=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp689,_tmp688,_tmp687,_tmp686,_tmp685,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});default:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp509;_tmp509.tag=0,({struct _fat_ptr _tmp68A=({const char*_tmpF6="primitive operator has > 2 arguments";_tag_fat(_tmpF6,sizeof(char),37U);});_tmp509.f1=_tmp68A;});_tmp509;});void*_tmpF4[1];_tmpF4[0]=& _tmpF5;({struct Cyc_Tcenv_Tenv*_tmp68D=te;unsigned _tmp68C=loc;void**_tmp68B=topt;Cyc_Tcexp_expr_err(_tmp68D,_tmp68C,_tmp68B,_tag_fat(_tmpF4,sizeof(void*),1));});});};}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 576
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
t=Cyc_Absyn_compress(t);{
void*_tmpF7=t;struct Cyc_Absyn_Tqual _tmpF9;void*_tmpF8;switch(*((int*)_tmpF7)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)){case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownAggr).tag == 2){_tmpF8=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmpF8;
# 580
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpFB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50B;_tmp50B.tag=0,({struct _fat_ptr _tmp68E=({const char*_tmpFD="attempt to write abstract ";_tag_fat(_tmpFD,sizeof(char),27U);});_tmp50B.f1=_tmp68E;});_tmp50B;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpFC=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp50A;_tmp50A.tag=6,_tmp50A.f1=ad;_tmp50A;});void*_tmpFA[2];_tmpFA[0]=& _tmpFB,_tmpFA[1]=& _tmpFC;({unsigned _tmp68F=loc;Cyc_Warn_err2(_tmp68F,_tag_fat(_tmpFA,sizeof(void*),2));});});
return 0;}
# 584
_tmpF8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownDatatypefield).tag == 2){_tmpF8=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypefield*df=_tmpF8;
# 596
{struct Cyc_List_List*_tmp103=df->typs;struct Cyc_List_List*fs=_tmp103;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp104=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmp104;struct _tuple17*_tmp105=_stmttmpD;void*_tmp107;struct Cyc_Absyn_Tqual _tmp106;_tmp106=_tmp105->f1;_tmp107=_tmp105->f2;{struct Cyc_Absyn_Tqual tq=_tmp106;void*t=_tmp107;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp109=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50E;_tmp50E.tag=0,({struct _fat_ptr _tmp690=({const char*_tmp10D="attempt to overwrite a datatype field (";_tag_fat(_tmp10D,sizeof(char),40U);});_tmp50E.f1=_tmp690;});_tmp50E;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp50D;_tmp50D.tag=1,_tmp50D.f1=df->name;_tmp50D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50C;_tmp50C.tag=0,({
struct _fat_ptr _tmp691=({const char*_tmp10C=") with a const member";_tag_fat(_tmp10C,sizeof(char),22U);});_tmp50C.f1=_tmp691;});_tmp50C;});void*_tmp108[3];_tmp108[0]=& _tmp109,_tmp108[1]=& _tmp10A,_tmp108[2]=& _tmp10B;({unsigned _tmp692=loc;Cyc_Warn_err2(_tmp692,_tag_fat(_tmp108,sizeof(void*),3));});});
return 0;}
# 603
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 605
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7: _tmpF8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF7)->f2;_LL4: {struct Cyc_List_List*fs=_tmpF8;
# 586
for(1;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpFE=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmpFE;
if((f->tq).real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp100=({struct Cyc_Warn_String_Warn_Warg_struct _tmp510;_tmp510.tag=0,({struct _fat_ptr _tmp693=({const char*_tmp102="attempt to overwrite an aggregate with const member ";_tag_fat(_tmp102,sizeof(char),53U);});_tmp510.f1=_tmp693;});_tmp510;});struct Cyc_Warn_String_Warn_Warg_struct _tmp101=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50F;_tmp50F.tag=0,_tmp50F.f1=*f->name;_tmp50F;});void*_tmpFF[2];_tmpFF[0]=& _tmp100,_tmpFF[1]=& _tmp101;({unsigned _tmp694=loc;Cyc_Warn_err2(_tmp694,_tag_fat(_tmpFF,sizeof(void*),2));});});
return 0;}
# 592
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 594
return 1;}case 4: _tmpF8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).elt_type;_tmpF9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).tq;{void*elt_type=_tmpF8;struct Cyc_Absyn_Tqual tq=_tmpF9;
# 607
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp511;_tmp511.tag=0,({struct _fat_ptr _tmp695=({const char*_tmp110="attempt to overwrite a const array";_tag_fat(_tmp110,sizeof(char),35U);});_tmp511.f1=_tmp695;});_tmp511;});void*_tmp10E[1];_tmp10E[0]=& _tmp10F;({unsigned _tmp696=loc;Cyc_Warn_err2(_tmp696,_tag_fat(_tmp10E,sizeof(void*),1));});});
return 0;}
# 611
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6: _tmpF8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF7)->f1;{struct Cyc_List_List*fs=_tmpF8;
# 613
for(1;fs != 0;fs=fs->tl){
struct _tuple17*_tmp111=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp111;struct _tuple17*_tmp112=_stmttmpE;void*_tmp114;struct Cyc_Absyn_Tqual _tmp113;_tmp113=_tmp112->f1;_tmp114=_tmp112->f2;{struct Cyc_Absyn_Tqual tq=_tmp113;void*t=_tmp114;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp116=({struct Cyc_Warn_String_Warn_Warg_struct _tmp512;_tmp512.tag=0,({struct _fat_ptr _tmp697=({const char*_tmp117="attempt to overwrite a tuple field with a const member";_tag_fat(_tmp117,sizeof(char),55U);});_tmp512.f1=_tmp697;});_tmp512;});void*_tmp115[1];_tmp115[0]=& _tmp116;({unsigned _tmp698=loc;Cyc_Warn_err2(_tmp698,_tag_fat(_tmp115,sizeof(void*),1));});});
return 0;}
# 619
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}
# 621
return 1;}default: _LLB:
 return 1;};}}
# 629
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 632
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp118=e->r;void*_stmttmpF=_tmp118;void*_tmp119=_stmttmpF;void*_tmp11B;void*_tmp11A;switch(*((int*)_tmp119)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)){case 3: _tmp11A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp11A;
_tmp11A=vd;goto _LL4;}case 4: _tmp11A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp11A;
_tmp11A=vd;goto _LL6;}case 5: _tmp11A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp11A;
_tmp11A=vd;goto _LL8;}case 1: _tmp11A=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp11A;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23: _tmp11A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 639
{void*_tmp11C=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp11C;void*_tmp11D=_stmttmp10;void*_tmp11E;struct Cyc_Absyn_Tqual _tmp11F;switch(*((int*)_tmp11D)){case 3: _tmp11F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11D)->f1).elt_tq;{struct Cyc_Absyn_Tqual tq=_tmp11F;
_tmp11F=tq;goto _LL1B;}case 4: _tmp11F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11D)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp11F;
if(!tq.real_const)return;goto _LL17;}case 6: _tmp11E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp11D)->f1;{struct Cyc_List_List*ts=_tmp11E;
# 643
struct _tuple16 _tmp120=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp11=_tmp120;struct _tuple16 _tmp121=_stmttmp11;int _tmp123;unsigned _tmp122;_tmp122=_tmp121.f1;_tmp123=_tmp121.f2;{unsigned i=_tmp122;int known=_tmp123;
if(!known){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp125=({struct Cyc_Warn_String_Warn_Warg_struct _tmp513;_tmp513.tag=0,({struct _fat_ptr _tmp699=({const char*_tmp126="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp126,sizeof(char),47U);});_tmp513.f1=_tmp699;});_tmp513;});void*_tmp124[1];_tmp124[0]=& _tmp125;({unsigned _tmp69A=e->loc;Cyc_Warn_err2(_tmp69A,_tag_fat(_tmp124,sizeof(void*),1));});});
return;}
# 648
{struct _handler_cons _tmp127;_push_handler(& _tmp127);{int _tmp129=0;if(setjmp(_tmp127.handler))_tmp129=1;if(!_tmp129){
{struct _tuple17*_tmp12A=({(struct _tuple17*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(ts,(int)i);struct _tuple17*_stmttmp12=_tmp12A;struct _tuple17*_tmp12B=_stmttmp12;struct Cyc_Absyn_Tqual _tmp12C;_tmp12C=_tmp12B->f1;{struct Cyc_Absyn_Tqual tq=_tmp12C;
if(!tq.real_const){_npop_handler(0);return;}}}
# 649
;_pop_handler();}else{void*_tmp128=(void*)Cyc_Core_get_exn_thrown();void*_tmp12D=_tmp128;void*_tmp12E;if(((struct Cyc_List_Nth_exn_struct*)_tmp12D)->tag == Cyc_List_Nth)
# 651
return;else{_tmp12E=_tmp12D;{void*exn=_tmp12E;(int)_rethrow(exn);}};}}}
goto _LL17;}}default:
 goto _LL17;}_LL17:;}
# 655
goto _LL0;}case 21: _tmp11A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct _fat_ptr*f=_tmp11B;
# 657
{void*_tmp12F=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp12F;void*_tmp130=_stmttmp13;void*_tmp131;switch(*((int*)_tmp130)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1)->f1).KnownAggr).tag == 2){_tmp131=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl**adp=_tmp131;
# 659
struct Cyc_Absyn_Aggrfield*sf=adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7: _tmp131=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp130)->f2;{struct Cyc_List_List*fs=_tmp131;
# 663
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30:
 goto _LL2B;}_LL2B:;}
# 668
goto _LL0;}case 22: _tmp11A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct _fat_ptr*f=_tmp11B;
# 670
{void*_tmp132=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp132;void*_tmp133=_stmttmp14;struct Cyc_Absyn_Tqual _tmp135;void*_tmp134;if(*((int*)_tmp133)== 3){_tmp134=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_type;_tmp135=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_tq;{void*elt_typ=_tmp134;struct Cyc_Absyn_Tqual tq=_tmp135;
# 672
if(!tq.real_const){
void*_tmp136=Cyc_Absyn_compress(elt_typ);void*_stmttmp15=_tmp136;void*_tmp137=_stmttmp15;void*_tmp138;switch(*((int*)_tmp137)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)->f1).KnownAggr).tag == 2){_tmp138=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl**adp=_tmp138;
# 675
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7: _tmp138=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp137)->f2;{struct Cyc_List_List*fs=_tmp138;
# 680
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C:
 goto _LL37;}_LL37:;}
# 686
goto _LL32;}}else{
goto _LL32;}_LL32:;}
# 689
goto _LL0;}case 20: _tmp11A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 691
{void*_tmp139=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp139;void*_tmp13A=_stmttmp16;struct Cyc_Absyn_Tqual _tmp13B;switch(*((int*)_tmp13A)){case 3: _tmp13B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13A)->f1).elt_tq;{struct Cyc_Absyn_Tqual tq=_tmp13B;
_tmp13B=tq;goto _LL42;}case 4: _tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13A)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp13B;
if(!tq.real_const)return;goto _LL3E;}default:
 goto _LL3E;}_LL3E:;}
# 696
goto _LL0;}case 12: _tmp11A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
_tmp11A=e1;goto _LL14;}case 13: _tmp11A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp11A;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15:
 goto _LL0;}_LL0:;}
# 701
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp515;_tmp515.tag=0,({struct _fat_ptr _tmp69B=({const char*_tmp13F="attempt to write a const location: ";_tag_fat(_tmp13F,sizeof(char),36U);});_tmp515.f1=_tmp69B;});_tmp515;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp13E=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp514;_tmp514.tag=4,_tmp514.f1=e;_tmp514;});void*_tmp13C[2];_tmp13C[0]=& _tmp13D,_tmp13C[1]=& _tmp13E;({unsigned _tmp69C=e->loc;Cyc_Warn_err2(_tmp69C,_tag_fat(_tmp13C,sizeof(void*),2));});});}
# 704
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 707
({struct Cyc_Tcenv_Tenv*_tmp69D=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp69D,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp141=({struct Cyc_Warn_String_Warn_Warg_struct _tmp516;_tmp516.tag=0,({struct _fat_ptr _tmp69E=({const char*_tmp142="increment/decrement of non-lvalue";_tag_fat(_tmp142,sizeof(char),34U);});_tmp516.f1=_tmp69E;});_tmp516;});void*_tmp140[1];_tmp140[0]=& _tmp141;({struct Cyc_Tcenv_Tenv*_tmp6A1=te;unsigned _tmp6A0=loc;void**_tmp69F=topt;Cyc_Tcexp_expr_err(_tmp6A1,_tmp6A0,_tmp69F,_tag_fat(_tmp140,sizeof(void*),1));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 713
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == 0 ||(int)i == 1)){
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Kinds_tmk))
({void*_tmp143=0U;({unsigned _tmp6A3=e->loc;struct _fat_ptr _tmp6A2=({const char*_tmp144="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp144,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6A3,_tmp6A2,_tag_fat(_tmp143,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp145=0U;({unsigned _tmp6A5=e->loc;struct _fat_ptr _tmp6A4=({const char*_tmp146="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp146,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp6A5,_tmp6A4,_tag_fat(_tmp145,sizeof(void*),0));});});}else{
# 722
({struct Cyc_Warn_String_Warn_Warg_struct _tmp148=({struct Cyc_Warn_String_Warn_Warg_struct _tmp518;_tmp518.tag=0,({struct _fat_ptr _tmp6A6=({const char*_tmp14A="expecting arithmetic or ? type but found ";_tag_fat(_tmp14A,sizeof(char),42U);});_tmp518.f1=_tmp6A6;});_tmp518;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp149=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp517;_tmp517.tag=2,_tmp517.f1=(void*)t;_tmp517;});void*_tmp147[2];_tmp147[0]=& _tmp148,_tmp147[1]=& _tmp149;({unsigned _tmp6A7=e->loc;Cyc_Warn_err2(_tmp6A7,_tag_fat(_tmp147,sizeof(void*),2));});});}}
# 724
return t;}}
# 728
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 730
({struct Cyc_Tcenv_Tenv*_tmp6A9=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp6A8=e1;Cyc_Tcexp_tcTest(_tmp6A9,_tmp6A8,({const char*_tmp14B="conditional expression";_tag_fat(_tmp14B,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*_tmp14C=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_tako:& Cyc_Kinds_tmko;struct Cyc_Core_Opt*ko=_tmp14C;
void*_tmp14D=({struct Cyc_Core_Opt*_tmp6AA=ko;Cyc_Absyn_new_evar(_tmp6AA,Cyc_Tcenv_lookup_opt_type_vars(te));});void*t=_tmp14D;
struct Cyc_List_List _tmp14E=({struct Cyc_List_List _tmp51E;_tmp51E.hd=e3,_tmp51E.tl=0;_tmp51E;});struct Cyc_List_List l1=_tmp14E;
struct Cyc_List_List _tmp14F=({struct Cyc_List_List _tmp51D;_tmp51D.hd=e2,_tmp51D.tl=& l1;_tmp51D;});struct Cyc_List_List l2=_tmp14F;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6AB=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp6AB,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp151=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51C;_tmp51C.tag=0,({struct _fat_ptr _tmp6AC=({const char*_tmp156="conditional clause types do not match: ";_tag_fat(_tmp156,sizeof(char),40U);});_tmp51C.f1=_tmp6AC;});_tmp51C;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp152=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp51B;_tmp51B.tag=3,_tmp51B.f1=(void*)e2->topt;_tmp51B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp153=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51A;_tmp51A.tag=0,({
struct _fat_ptr _tmp6AD=({const char*_tmp155=" != ";_tag_fat(_tmp155,sizeof(char),5U);});_tmp51A.f1=_tmp6AD;});_tmp51A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp154=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp519;_tmp519.tag=3,_tmp519.f1=(void*)e3->topt;_tmp519;});void*_tmp150[4];_tmp150[0]=& _tmp151,_tmp150[1]=& _tmp152,_tmp150[2]=& _tmp153,_tmp150[3]=& _tmp154;({unsigned _tmp6AE=loc;Cyc_Tcexp_err_and_explain(_tmp6AE,_tag_fat(_tmp150,sizeof(void*),4));});});
return t;}}
# 744
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 746
({struct Cyc_Tcenv_Tenv*_tmp6B0=te;struct Cyc_Absyn_Exp*_tmp6AF=e1;Cyc_Tcexp_tcTest(_tmp6B0,_tmp6AF,({const char*_tmp157="logical-and expression";_tag_fat(_tmp157,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6B2=te;struct Cyc_Absyn_Exp*_tmp6B1=e2;Cyc_Tcexp_tcTest(_tmp6B2,_tmp6B1,({const char*_tmp158="logical-and expression";_tag_fat(_tmp158,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 750
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 752
({struct Cyc_Tcenv_Tenv*_tmp6B4=te;struct Cyc_Absyn_Exp*_tmp6B3=e1;Cyc_Tcexp_tcTest(_tmp6B4,_tmp6B3,({const char*_tmp159="logical-or expression";_tag_fat(_tmp159,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6B6=te;struct Cyc_Absyn_Exp*_tmp6B5=e2;Cyc_Tcexp_tcTest(_tmp6B6,_tmp6B5,({const char*_tmp15A="logical-or expression";_tag_fat(_tmp15A,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 758
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 765
({struct Cyc_Tcenv_Tenv*_tmp6B7=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp6B7,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 770
{void*_tmp15B=Cyc_Absyn_compress(t1);void*_stmttmp17=_tmp15B;void*_tmp15C=_stmttmp17;if(*((int*)_tmp15C)== 4){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51F;_tmp51F.tag=0,({struct _fat_ptr _tmp6B8=({const char*_tmp15F="cannot assign to an array";_tag_fat(_tmp15F,sizeof(char),26U);});_tmp51F.f1=_tmp6B8;});_tmp51F;});void*_tmp15D[1];_tmp15D[0]=& _tmp15E;({unsigned _tmp6B9=loc;Cyc_Warn_err2(_tmp6B9,_tag_fat(_tmp15D,sizeof(void*),1));});});goto _LL0;}else{
goto _LL0;}_LL0:;}
# 775
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp161=({struct Cyc_Warn_String_Warn_Warg_struct _tmp520;_tmp520.tag=0,({struct _fat_ptr _tmp6BA=({const char*_tmp162="type is abstract (can't determine size)";_tag_fat(_tmp162,sizeof(char),40U);});_tmp520.f1=_tmp6BA;});_tmp520;});void*_tmp160[1];_tmp160[0]=& _tmp161;({unsigned _tmp6BB=loc;Cyc_Warn_err2(_tmp6BB,_tag_fat(_tmp160,sizeof(void*),1));});});
# 779
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp164=({struct Cyc_Warn_String_Warn_Warg_struct _tmp521;_tmp521.tag=0,({struct _fat_ptr _tmp6BC=({const char*_tmp165="assignment to non-lvalue";_tag_fat(_tmp165,sizeof(char),25U);});_tmp521.f1=_tmp6BC;});_tmp521;});void*_tmp163[1];_tmp163[0]=& _tmp164;({struct Cyc_Tcenv_Tenv*_tmp6BF=te;unsigned _tmp6BE=loc;void**_tmp6BD=topt;Cyc_Tcexp_expr_err(_tmp6BF,_tmp6BE,_tmp6BD,_tag_fat(_tmp163,sizeof(void*),1));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
Cyc_Tcexp_check_consume(e2->loc,t2,e2);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6C1=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6C0=e2;Cyc_Tcutil_coerce_assign(_tmp6C1,_tmp6C0,t1);})){
void*_tmp166=({struct Cyc_Warn_String_Warn_Warg_struct _tmp168=({struct Cyc_Warn_String_Warn_Warg_struct _tmp525;_tmp525.tag=0,({struct _fat_ptr _tmp6C2=({const char*_tmp16D="type mismatch: ";_tag_fat(_tmp16D,sizeof(char),16U);});_tmp525.f1=_tmp6C2;});_tmp525;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp169=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp524;_tmp524.tag=2,_tmp524.f1=(void*)t1;_tmp524;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp523;_tmp523.tag=0,({struct _fat_ptr _tmp6C3=({const char*_tmp16C=" != ";_tag_fat(_tmp16C,sizeof(char),5U);});_tmp523.f1=_tmp6C3;});_tmp523;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp522;_tmp522.tag=2,_tmp522.f1=(void*)t2;_tmp522;});void*_tmp167[4];_tmp167[0]=& _tmp168,_tmp167[1]=& _tmp169,_tmp167[2]=& _tmp16A,_tmp167[3]=& _tmp16B;({struct Cyc_Tcenv_Tenv*_tmp6C6=te;unsigned _tmp6C5=loc;void**_tmp6C4=topt;Cyc_Tcexp_expr_err(_tmp6C6,_tmp6C5,_tmp6C4,_tag_fat(_tmp167,sizeof(void*),4));});});void*result=_tmp166;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}
# 790
return t1;}{
# 792
struct Cyc_Absyn_Exp*_tmp16E=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp16E;
void*_tmp16F=Cyc_Tcexp_tcBinPrimop(te,loc,0,(enum Cyc_Absyn_Primop)po->v,e1copy,e2);void*t_result=_tmp16F;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
return t1;
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp170=({struct Cyc_Warn_String_Warn_Warg_struct _tmp172=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52A;_tmp52A.tag=0,({
struct _fat_ptr _tmp6C7=({const char*_tmp179="Cannot use this operator in an assignment when ";_tag_fat(_tmp179,sizeof(char),48U);});_tmp52A.f1=_tmp6C7;});_tmp52A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp173=({struct Cyc_Warn_String_Warn_Warg_struct _tmp529;_tmp529.tag=0,({
struct _fat_ptr _tmp6C8=({const char*_tmp178="the arguments have types ";_tag_fat(_tmp178,sizeof(char),26U);});_tmp529.f1=_tmp6C8;});_tmp529;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp174=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp528;_tmp528.tag=2,_tmp528.f1=(void*)t1;_tmp528;});struct Cyc_Warn_String_Warn_Warg_struct _tmp175=({struct Cyc_Warn_String_Warn_Warg_struct _tmp527;_tmp527.tag=0,({struct _fat_ptr _tmp6C9=({const char*_tmp177=" and ";_tag_fat(_tmp177,sizeof(char),6U);});_tmp527.f1=_tmp6C9;});_tmp527;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp176=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp526;_tmp526.tag=2,_tmp526.f1=(void*)t2;_tmp526;});void*_tmp171[5];_tmp171[0]=& _tmp172,_tmp171[1]=& _tmp173,_tmp171[2]=& _tmp174,_tmp171[3]=& _tmp175,_tmp171[4]=& _tmp176;({struct Cyc_Tcenv_Tenv*_tmp6CC=te;unsigned _tmp6CB=loc;void**_tmp6CA=topt;Cyc_Tcexp_expr_err(_tmp6CC,_tmp6CB,_tmp6CA,_tag_fat(_tmp171,sizeof(void*),5));});});
# 797
void*result=_tmp170;
# 800
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}
# 804
return t_result;}}}}
# 808
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp6CD=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp6CD,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp6CF=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp6CE=topt;Cyc_Tcexp_tcExp(_tmp6CF,_tmp6CE,e2);});
return(void*)_check_null(e2->topt);}
# 815
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 818
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 821
{void*_tmp17A=Cyc_Absyn_compress(t1);void*_stmttmp18=_tmp17A;void*_tmp17B=_stmttmp18;if(*((int*)_tmp17B)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17B)->f1)){case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17B)->f1)->f1 == 0){
# 823
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,1U);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17B)->f1)->f2){case Cyc_Absyn_Char_sz:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL6:
# 826
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,1U);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;}
# 830
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp17C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp17C;struct Cyc_Absyn_Datatypefield _tmp17D=_stmttmp19;enum Cyc_Absyn_Scope _tmp181;unsigned _tmp180;void*_tmp17F;void*_tmp17E;_tmp17E=_tmp17D.name;_tmp17F=_tmp17D.typs;_tmp180=_tmp17D.loc;_tmp181=_tmp17D.sc;{struct _tuple1*n=_tmp17E;struct Cyc_List_List*typs=_tmp17F;unsigned loc=_tmp180;enum Cyc_Absyn_Scope sc=_tmp181;
# 833
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 836
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 840
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp182=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp182;struct Cyc_Absyn_Datatypefield _tmp183=_stmttmp1A;enum Cyc_Absyn_Scope _tmp187;unsigned _tmp186;void*_tmp185;void*_tmp184;_tmp184=_tmp183.name;_tmp185=_tmp183.typs;_tmp186=_tmp183.loc;_tmp187=_tmp183.sc;{struct _tuple1*n=_tmp184;struct Cyc_List_List*typs=_tmp185;unsigned loc=_tmp186;enum Cyc_Absyn_Scope sc=_tmp187;
# 843
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 847
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp6D1=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6D0=e;Cyc_Tcutil_coerce_arg(_tmp6D1,_tmp6D0,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 851
({struct Cyc_Warn_String_Warn_Warg_struct _tmp189=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52E;_tmp52E.tag=0,({struct _fat_ptr _tmp6D2=({const char*_tmp18E="can't find a field in ";_tag_fat(_tmp18E,sizeof(char),23U);});_tmp52E.f1=_tmp6D2;});_tmp52E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp18A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52D;_tmp52D.tag=2,_tmp52D.f1=(void*)tu;_tmp52D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52C;_tmp52C.tag=0,({struct _fat_ptr _tmp6D3=({const char*_tmp18D=" to inject value of type ";_tag_fat(_tmp18D,sizeof(char),26U);});_tmp52C.f1=_tmp6D3;});_tmp52C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp18C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52B;_tmp52B.tag=2,_tmp52B.f1=(void*)t1;_tmp52B;});void*_tmp188[4];_tmp188[0]=& _tmp189,_tmp188[1]=& _tmp18A,_tmp188[2]=& _tmp18B,_tmp188[3]=& _tmp18C;({unsigned _tmp6D4=e->loc;Cyc_Warn_err2(_tmp6D4,_tag_fat(_tmp188,sizeof(void*),4));});});
return 0;}
# 856
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 862
struct Cyc_List_List*_tmp18F=args;struct Cyc_List_List*es=_tmp18F;
int _tmp190=0;int arg_cnt=_tmp190;
struct Cyc_Tcenv_Tenv*_tmp191=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp191;
struct Cyc_Tcenv_Tenv*_tmp192=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp192;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
# 871
void*_tmp193=t;void*_tmp199;void*_tmp198;void*_tmp197;void*_tmp196;struct Cyc_Absyn_Tqual _tmp195;void*_tmp194;if(*((int*)_tmp193)== 3){_tmp194=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp193)->f1).elt_type;_tmp195=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp193)->f1).elt_tq;_tmp196=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp193)->f1).ptr_atts).rgn;_tmp197=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp193)->f1).ptr_atts).nullable;_tmp198=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp193)->f1).ptr_atts).bounds;_tmp199=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp193)->f1).ptr_atts).zero_term;{void*t1=_tmp194;struct Cyc_Absyn_Tqual tq=_tmp195;void*rgn=_tmp196;void*x=_tmp197;void*b=_tmp198;void*zt=_tmp199;
# 876
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 878
Cyc_Tcutil_check_nonzero_bound(loc,b);
# 881
t1=({void*_tmp6D5=Cyc_Absyn_compress(t1);Cyc_CurRgn_instantiate(_tmp6D5,Cyc_Tcenv_curr_lifo_rgn(te));});{
void*_tmp19A=t1;void*_tmp1A8;void*_tmp1A7;void*_tmp1A6;void*_tmp1A5;void*_tmp1A4;void*_tmp1A3;void*_tmp1A2;void*_tmp1A1;int _tmp1A0;void*_tmp19F;void*_tmp19E;struct Cyc_Absyn_Tqual _tmp19D;void*_tmp19C;void*_tmp19B;if(*((int*)_tmp19A)== 5){_tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).tvars;_tmp19C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).effect;_tmp19D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).ret_tqual;_tmp19E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).ret_type;_tmp19F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).args;_tmp1A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).c_varargs;_tmp1A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).cyc_varargs;_tmp1A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).rgn_po;_tmp1A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).attributes;_tmp1A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).requires_clause;_tmp1A5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).requires_relns;_tmp1A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).ensures_clause;_tmp1A7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).ensures_relns;_tmp1A8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19A)->f1).return_value;{struct Cyc_List_List*tvars=_tmp19B;void*eff=_tmp19C;struct Cyc_Absyn_Tqual res_tq=_tmp19D;void*res_typ=_tmp19E;struct Cyc_List_List*args_info=_tmp19F;int c_vararg=_tmp1A0;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp1A1;struct Cyc_List_List*rgn_po=_tmp1A2;struct Cyc_List_List*atts=_tmp1A3;struct Cyc_Absyn_Exp*req=_tmp1A4;struct Cyc_List_List*req_relns=_tmp1A5;struct Cyc_Absyn_Exp*ens=_tmp1A6;struct Cyc_List_List*ens_relns=_tmp1A7;struct Cyc_Absyn_Vardecl*ret_var=_tmp1A8;
# 886
if(tvars != 0 || rgn_po != 0)
({void*_tmp1A9=0U;({unsigned _tmp6D7=e->loc;struct _fat_ptr _tmp6D6=({const char*_tmp1AA="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1AA,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp6D7,_tmp6D6,_tag_fat(_tmp1A9,sizeof(void*),0));});});
# 890
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 892
while(es != 0 && args_info != 0){
# 894
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6D9=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6D8=e1;Cyc_Tcutil_coerce_arg(_tmp6D9,_tmp6D8,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1BC="actual argument has type ";_tag_fat(_tmp1BC,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1BB=" but formal has type ";_tag_fat(_tmp1BB,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp6DC=({unsigned long _tmp6DB=({unsigned long _tmp6DA=Cyc_strlen((struct _fat_ptr)s0);_tmp6DA + Cyc_strlen((struct _fat_ptr)s1);});_tmp6DB + Cyc_strlen((struct _fat_ptr)s2);});_tmp6DC + Cyc_strlen((struct _fat_ptr)s3);})>= 70U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp535;_tmp535.tag=0,_tmp535.f1=s0;_tmp535;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp534;_tmp534.tag=0,({struct _fat_ptr _tmp6DD=({const char*_tmp1B5="\n\t";_tag_fat(_tmp1B5,sizeof(char),3U);});_tmp534.f1=_tmp6DD;});_tmp534;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp533;_tmp533.tag=0,_tmp533.f1=s1;_tmp533;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp532;_tmp532.tag=0,({struct _fat_ptr _tmp6DE=({const char*_tmp1B4="\n";_tag_fat(_tmp1B4,sizeof(char),2U);});_tmp532.f1=_tmp6DE;});_tmp532;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp531;_tmp531.tag=0,_tmp531.f1=s2;_tmp531;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp530;_tmp530.tag=0,({struct _fat_ptr _tmp6DF=({const char*_tmp1B3="\n\t";_tag_fat(_tmp1B3,sizeof(char),3U);});_tmp530.f1=_tmp6DF;});_tmp530;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52F;_tmp52F.tag=0,_tmp52F.f1=s3;_tmp52F;});void*_tmp1AB[7];_tmp1AB[0]=& _tmp1AC,_tmp1AB[1]=& _tmp1AD,_tmp1AB[2]=& _tmp1AE,_tmp1AB[3]=& _tmp1AF,_tmp1AB[4]=& _tmp1B0,_tmp1AB[5]=& _tmp1B1,_tmp1AB[6]=& _tmp1B2;({unsigned _tmp6E0=e1->loc;Cyc_Warn_err2(_tmp6E0,_tag_fat(_tmp1AB,sizeof(void*),7));});});else{
# 906
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp539;_tmp539.tag=0,_tmp539.f1=s0;_tmp539;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp538;_tmp538.tag=0,_tmp538.f1=s1;_tmp538;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp537;_tmp537.tag=0,_tmp537.f1=s2;_tmp537;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp536;_tmp536.tag=0,_tmp536.f1=s3;_tmp536;});void*_tmp1B6[4];_tmp1B6[0]=& _tmp1B7,_tmp1B6[1]=& _tmp1B8,_tmp1B6[2]=& _tmp1B9,_tmp1B6[3]=& _tmp1BA;({unsigned _tmp6E1=e1->loc;Cyc_Warn_err2(_tmp6E1,_tag_fat(_tmp1B6,sizeof(void*),4));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 911
if(alias_coercion)
({struct Cyc_List_List*_tmp6E2=({struct Cyc_List_List*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->hd=(void*)arg_cnt,_tmp1BD->tl=*alias_arg_exps;_tmp1BD;});*alias_arg_exps=_tmp6E2;});
Cyc_Tcexp_check_consume(e1->loc,t2,e1);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 921
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1BE=(void*)a->hd;void*_stmttmp1B=_tmp1BE;void*_tmp1BF=_stmttmp1B;int _tmp1C2;int _tmp1C1;enum Cyc_Absyn_Format_Type _tmp1C0;if(*((int*)_tmp1BF)== 19){_tmp1C0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1BF)->f1;_tmp1C1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1BF)->f2;_tmp1C2=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1BF)->f3;{enum Cyc_Absyn_Format_Type ft=_tmp1C0;int fmt_arg_pos=_tmp1C1;int arg_start_pos=_tmp1C2;
# 925
{struct _handler_cons _tmp1C3;_push_handler(& _tmp1C3);{int _tmp1C5=0;if(setjmp(_tmp1C3.handler))_tmp1C5=1;if(!_tmp1C5){
# 927
{struct Cyc_Absyn_Exp*_tmp1C6=({(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1C6;
# 929
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 933
fmt_args=({struct Cyc_Core_Opt*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({struct Cyc_List_List*_tmp6E3=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,int))Cyc_List_nth_tail;})(args,arg_start_pos - 1);_tmp1C7->v=_tmp6E3;});_tmp1C7;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1C8=ft;switch((int)_tmp1C8){case Cyc_Absyn_Printf_ft:
# 937
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 940
goto _LLF;case Cyc_Absyn_Scanf_ft:
 goto _LL15;default: _LL15:
# 943
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 946
goto _LLF;}_LLF:;}}
# 927
;_pop_handler();}else{void*_tmp1C4=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C9=_tmp1C4;void*_tmp1CA;if(((struct Cyc_List_Nth_exn_struct*)_tmp1C9)->tag == Cyc_List_Nth){
# 948
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53A;_tmp53A.tag=0,({struct _fat_ptr _tmp6E4=({const char*_tmp1CD="bad format arguments";_tag_fat(_tmp1CD,sizeof(char),21U);});_tmp53A.f1=_tmp6E4;});_tmp53A;});void*_tmp1CB[1];_tmp1CB[0]=& _tmp1CC;({unsigned _tmp6E5=loc;Cyc_Warn_err2(_tmp6E5,_tag_fat(_tmp1CB,sizeof(void*),1));});});goto _LL16;}else{_tmp1CA=_tmp1C9;{void*exn=_tmp1CA;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{
goto _LLA;}_LLA:;}}
# 953
if(args_info != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B;_tmp53B.tag=0,({struct _fat_ptr _tmp6E6=({const char*_tmp1D0="too few arguments for function";_tag_fat(_tmp1D0,sizeof(char),31U);});_tmp53B.f1=_tmp6E6;});_tmp53B;});void*_tmp1CE[1];_tmp1CE[0]=& _tmp1CF;({unsigned _tmp6E7=loc;Cyc_Warn_err2(_tmp6E7,_tag_fat(_tmp1CE,sizeof(void*),1));});});else{
# 956
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(1;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53C;_tmp53C.tag=0,({struct _fat_ptr _tmp6E8=({const char*_tmp1D3="too many arguments for function";_tag_fat(_tmp1D3,sizeof(char),32U);});_tmp53C.f1=_tmp6E8;});_tmp53C;});void*_tmp1D1[1];_tmp1D1[0]=& _tmp1D2;({unsigned _tmp6E9=loc;Cyc_Warn_err2(_tmp6E9,_tag_fat(_tmp1D1,sizeof(void*),1));});});else{
# 963
struct Cyc_Absyn_VarargInfo _tmp1D4=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1C=_tmp1D4;struct Cyc_Absyn_VarargInfo _tmp1D5=_stmttmp1C;int _tmp1D7;void*_tmp1D6;_tmp1D6=_tmp1D5.type;_tmp1D7=_tmp1D5.inject;{void*vt=_tmp1D6;int inject=_tmp1D7;
struct Cyc_Absyn_VarargCallInfo*_tmp1D8=({struct Cyc_Absyn_VarargCallInfo*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->num_varargs=0,_tmp1F0->injectors=0,_tmp1F0->vai=cyc_vararg;_tmp1F0;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1D8;
# 967
*vararg_call_info=vci;
# 969
if(!inject)
# 971
for(1;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6EB=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6EA=e1;Cyc_Tcutil_coerce_arg(_tmp6EB,_tmp6EA,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp540;_tmp540.tag=0,({struct _fat_ptr _tmp6EC=({const char*_tmp1DF="vararg requires type ";_tag_fat(_tmp1DF,sizeof(char),22U);});_tmp540.f1=_tmp6EC;});_tmp540;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1DB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53F;_tmp53F.tag=2,_tmp53F.f1=(void*)vt;_tmp53F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53E;_tmp53E.tag=0,({
struct _fat_ptr _tmp6ED=({const char*_tmp1DE=" but argument has type ";_tag_fat(_tmp1DE,sizeof(char),24U);});_tmp53E.f1=_tmp6ED;});_tmp53E;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1DD=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp53D;_tmp53D.tag=3,_tmp53D.f1=(void*)e1->topt;_tmp53D;});void*_tmp1D9[4];_tmp1D9[0]=& _tmp1DA,_tmp1D9[1]=& _tmp1DB,_tmp1D9[2]=& _tmp1DC,_tmp1D9[3]=& _tmp1DD;({unsigned _tmp6EE=loc;Cyc_Tcexp_err_and_explain(_tmp6EE,_tag_fat(_tmp1D9,sizeof(void*),4));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp6EF=({struct Cyc_List_List*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->hd=(void*)arg_cnt,_tmp1E0->tl=*alias_arg_exps;_tmp1E0;});*alias_arg_exps=_tmp6EF;});
Cyc_Tcexp_check_consume(e1->loc,vt,e1);}else{
# 986
void*_tmp1E1=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1D=_tmp1E1;void*_tmp1E2=_stmttmp1D;void*_tmp1E4;void*_tmp1E3;if(*((int*)_tmp1E2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E2)->f1)== 18){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E2)->f1)->f1).KnownDatatype).tag == 2){_tmp1E3=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E2)->f1)->f1).KnownDatatype).val;_tmp1E4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E2)->f2;{struct Cyc_Absyn_Datatypedecl*td=_tmp1E3;struct Cyc_List_List*targs=_tmp1E4;
# 990
struct Cyc_Absyn_Datatypedecl*_tmp1E5=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp1E5;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp542;_tmp542.tag=0,({struct _fat_ptr _tmp6F0=({const char*_tmp1E9="can't inject into abstract ";_tag_fat(_tmp1E9,sizeof(char),28U);});_tmp542.f1=_tmp6F0;});_tmp542;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp541;_tmp541.tag=2,_tmp541.f1=(void*)vt;_tmp541;});void*_tmp1E6[2];_tmp1E6[0]=& _tmp1E7,_tmp1E6[1]=& _tmp1E8;({unsigned _tmp6F1=loc;Cyc_Warn_err2(_tmp6F1,_tag_fat(_tmp1E6,sizeof(void*),2));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 1000
({void*_tmp6F2=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp6F2,Cyc_Tcenv_curr_rgn(te));});{
# 1002
struct Cyc_List_List*_tmp1EA=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;})(td->tvs,targs);struct Cyc_List_List*inst=_tmp1EA;
for(1;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1007
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_check_consume(e1->loc,(void*)_check_null(e1->topt),e1);}{
# 1011
struct Cyc_Absyn_Datatypefield*_tmp1EB=({struct Cyc_Tcenv_Tenv*_tmp6F6=te;struct Cyc_Absyn_Exp*_tmp6F5=e1;void*_tmp6F4=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp6F3=inst;Cyc_Tcexp_tcInjection(_tmp6F6,_tmp6F5,_tmp6F4,_tmp6F3,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp1EB;
if(f != 0)
({struct Cyc_List_List*_tmp6F9=({
struct Cyc_List_List*(*_tmp6F8)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp6F7=vci->injectors;_tmp6F8(_tmp6F7,({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=f,_tmp1EC->tl=0;_tmp1EC;}));});
# 1013
vci->injectors=_tmp6F9;});}}}
# 1016
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp543;_tmp543.tag=0,({struct _fat_ptr _tmp6FA=({const char*_tmp1EF="bad inject vararg type";_tag_fat(_tmp1EF,sizeof(char),23U);});_tmp543.f1=_tmp6FA;});_tmp543;});void*_tmp1ED[1];_tmp1ED[0]=& _tmp1EE;({unsigned _tmp6FB=loc;Cyc_Warn_err2(_tmp6FB,_tag_fat(_tmp1ED,sizeof(void*),1));});});goto _LL1E;}_LL1E:;}}}}}}
# 1022
if(*alias_arg_exps == 0)
# 1031 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1033
return res_typ;}}}else{
# 1035
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp545;_tmp545.tag=0,({struct _fat_ptr _tmp6FC=({const char*_tmp1F4="expected pointer to function but found ";_tag_fat(_tmp1F4,sizeof(char),40U);});_tmp545.f1=_tmp6FC;});_tmp545;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp544;_tmp544.tag=2,_tmp544.f1=(void*)t;_tmp544;});void*_tmp1F1[2];_tmp1F1[0]=& _tmp1F2,_tmp1F1[1]=& _tmp1F3;({struct Cyc_Tcenv_Tenv*_tmp6FF=te;unsigned _tmp6FE=loc;void**_tmp6FD=topt;Cyc_Tcexp_expr_err(_tmp6FF,_tmp6FE,_tmp6FD,_tag_fat(_tmp1F1,sizeof(void*),2));});});};}}}else{
# 1038
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp547;_tmp547.tag=0,({struct _fat_ptr _tmp700=({const char*_tmp1F8="expected pointer to function but found ";_tag_fat(_tmp1F8,sizeof(char),40U);});_tmp547.f1=_tmp700;});_tmp547;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp546;_tmp546.tag=2,_tmp546.f1=(void*)t;_tmp546;});void*_tmp1F5[2];_tmp1F5[0]=& _tmp1F6,_tmp1F5[1]=& _tmp1F7;({struct Cyc_Tcenv_Tenv*_tmp703=te;unsigned _tmp702=loc;void**_tmp701=topt;Cyc_Tcexp_expr_err(_tmp703,_tmp702,_tmp701,_tag_fat(_tmp1F5,sizeof(void*),2));});});};}}}
# 1042
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1F9=Cyc_Absyn_exn_type();void*exception_type=_tmp1F9;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp705=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp704=e;Cyc_Tcutil_coerce_arg(_tmp705,_tmp704,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54B;_tmp54B.tag=0,({struct _fat_ptr _tmp706=({const char*_tmp200="expected ";_tag_fat(_tmp200,sizeof(char),10U);});_tmp54B.f1=_tmp706;});_tmp54B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1FC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54A;_tmp54A.tag=2,_tmp54A.f1=(void*)exception_type;_tmp54A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1FD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp549;_tmp549.tag=0,({struct _fat_ptr _tmp707=({const char*_tmp1FF=" but found ";_tag_fat(_tmp1FF,sizeof(char),12U);});_tmp549.f1=_tmp707;});_tmp549;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1FE=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp548;_tmp548.tag=3,_tmp548.f1=(void*)e->topt;_tmp548;});void*_tmp1FA[4];_tmp1FA[0]=& _tmp1FB,_tmp1FA[1]=& _tmp1FC,_tmp1FA[2]=& _tmp1FD,_tmp1FA[3]=& _tmp1FE;({unsigned _tmp708=loc;Cyc_Warn_err2(_tmp708,_tag_fat(_tmp1FA,sizeof(void*),4));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1053
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1055
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
{void*_tmp201=t1;struct Cyc_Absyn_PtrAtts _tmp204;struct Cyc_Absyn_Tqual _tmp203;void*_tmp202;if(*((int*)_tmp201)== 3){_tmp202=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).elt_type;_tmp203=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).elt_tq;_tmp204=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).ptr_atts;{void*t=_tmp202;struct Cyc_Absyn_Tqual tq=_tmp203;struct Cyc_Absyn_PtrAtts atts=_tmp204;
# 1058
{void*_tmp205=Cyc_Absyn_compress(t);void*_stmttmp1E=_tmp205;void*_tmp206=_stmttmp1E;struct Cyc_Absyn_FnInfo _tmp207;if(*((int*)_tmp206)== 5){_tmp207=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp206)->f1;{struct Cyc_Absyn_FnInfo info=_tmp207;
# 1060
struct Cyc_List_List*_tmp208=info.tvars;struct Cyc_List_List*tvars=_tmp208;
struct Cyc_List_List*instantiation=0;
# 1063
for(1;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Kinds_bk);
({unsigned _tmp70C=loc;struct Cyc_Tcenv_Tenv*_tmp70B=te;struct Cyc_List_List*_tmp70A=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp709=k;Cyc_Tctyp_check_type(_tmp70C,_tmp70B,_tmp70A,_tmp709,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp20A=_cycalloc(sizeof(*_tmp20A));({struct _tuple14*_tmp70D=({struct _tuple14*_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp209->f2=(void*)ts->hd;_tmp209;});_tmp20A->hd=_tmp70D;}),_tmp20A->tl=instantiation;_tmp20A;});}
# 1069
info.tvars=tvars;
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp20C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54C;_tmp54C.tag=0,({struct _fat_ptr _tmp70E=({const char*_tmp20D="too many type variables in instantiation";_tag_fat(_tmp20D,sizeof(char),41U);});_tmp54C.f1=_tmp70E;});_tmp54C;});void*_tmp20B[1];_tmp20B[0]=& _tmp20C;({struct Cyc_Tcenv_Tenv*_tmp711=te;unsigned _tmp710=loc;void**_tmp70F=topt;Cyc_Tcexp_expr_err(_tmp711,_tmp710,_tmp70F,_tag_fat(_tmp20B,sizeof(void*),1));});});
# 1075
if(tvars == 0){
({struct Cyc_List_List*_tmp712=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,info.rgn_po);info.rgn_po=_tmp712;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,info.rgn_po);
info.rgn_po=0;}{
# 1080
void*new_fn_typ=({struct Cyc_List_List*_tmp713=instantiation;Cyc_Tcutil_substitute(_tmp713,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->tag=5,_tmp20E->f1=info;_tmp20E;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp54D;_tmp54D.elt_type=new_fn_typ,_tmp54D.elt_tq=tq,_tmp54D.ptr_atts=atts;_tmp54D;}));}}}else{
goto _LL5;}_LL5:;}
# 1084
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 1087
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp210=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54F;_tmp54F.tag=0,({struct _fat_ptr _tmp714=({const char*_tmp212="expecting polymorphic type but found ";_tag_fat(_tmp212,sizeof(char),38U);});_tmp54F.f1=_tmp714;});_tmp54F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp211=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54E;_tmp54E.tag=2,_tmp54E.f1=(void*)t1;_tmp54E;});void*_tmp20F[2];_tmp20F[0]=& _tmp210,_tmp20F[1]=& _tmp211;({struct Cyc_Tcenv_Tenv*_tmp717=te;unsigned _tmp716=loc;void**_tmp715=topt;Cyc_Tcexp_expr_err(_tmp717,_tmp716,_tmp715,_tag_fat(_tmp20F,sizeof(void*),2));});});}
# 1091
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1093
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1095
({void*_tmp718=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp718;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1100
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1102
({unsigned _tmp71C=loc;struct Cyc_Tcenv_Tenv*_tmp71B=te;struct Cyc_List_List*_tmp71A=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp719=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_tak:& Cyc_Kinds_tmk;
# 1102
Cyc_Tctyp_check_type(_tmp71C,_tmp71B,_tmp71A,_tmp719,1,0,t);});
# 1104
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp71F=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp71E=loc;void*_tmp71D=t2;Cyc_Tcutil_silent_castable(_tmp71F,_tmp71E,_tmp71D,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=1U;else{
# 1110
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp722=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp721=loc;void*_tmp720=t2;Cyc_Tcutil_castable(_tmp722,_tmp721,_tmp720,t);});
if((int)crc != 0)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=1U;else{
# 1117
Cyc_Unify_unify(t2,t);{
void*_tmp213=({struct Cyc_Warn_String_Warn_Warg_struct _tmp215=({struct Cyc_Warn_String_Warn_Warg_struct _tmp553;_tmp553.tag=0,({struct _fat_ptr _tmp723=({const char*_tmp21A="cannot cast ";_tag_fat(_tmp21A,sizeof(char),13U);});_tmp553.f1=_tmp723;});_tmp553;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp216=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp552;_tmp552.tag=2,_tmp552.f1=(void*)t2;_tmp552;});struct Cyc_Warn_String_Warn_Warg_struct _tmp217=({struct Cyc_Warn_String_Warn_Warg_struct _tmp551;_tmp551.tag=0,({struct _fat_ptr _tmp724=({const char*_tmp219=" to ";_tag_fat(_tmp219,sizeof(char),5U);});_tmp551.f1=_tmp724;});_tmp551;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp218=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp550;_tmp550.tag=2,_tmp550.f1=(void*)t;_tmp550;});void*_tmp214[4];_tmp214[0]=& _tmp215,_tmp214[1]=& _tmp216,_tmp214[2]=& _tmp217,_tmp214[3]=& _tmp218;({struct Cyc_Tcenv_Tenv*_tmp726=te;unsigned _tmp725=loc;Cyc_Tcexp_expr_err(_tmp726,_tmp725,& t,_tag_fat(_tmp214,sizeof(void*),4));});});void*result=_tmp213;
Cyc_Unify_explain_failure();
return result;}}}}{
# 1126
struct _tuple0 _tmp21B=({struct _tuple0 _tmp554;_tmp554.f1=e->r,({void*_tmp727=Cyc_Absyn_compress(t);_tmp554.f2=_tmp727;});_tmp554;});struct _tuple0 _stmttmp1F=_tmp21B;struct _tuple0 _tmp21C=_stmttmp1F;void*_tmp221;void*_tmp220;void*_tmp21F;void*_tmp21E;int _tmp21D;if(*((int*)_tmp21C.f1)== 34){if(*((int*)_tmp21C.f2)== 3){_tmp21D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21C.f1)->f1).fat_result;_tmp21E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).nullable;_tmp21F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).bounds;_tmp220=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).zero_term;_tmp221=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).autoreleased;{int fat_result=_tmp21D;void*nbl=_tmp21E;void*bds=_tmp21F;void*zt=_tmp220;void*rel=_tmp221;
# 1130
if(((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl))&& !
Cyc_Tcutil_force_type2bool(0,rel)){
struct Cyc_Absyn_Exp*_tmp222=({void*_tmp728=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp728,bds);});struct Cyc_Absyn_Exp*eopt=_tmp222;
if(eopt != 0){
if((Cyc_Evexp_eval_const_uint_exp(e)).f1 == 1U)
({void*_tmp223=0U;({unsigned _tmp72A=loc;struct _fat_ptr _tmp729=({const char*_tmp224="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp224,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp72A,_tmp729,_tag_fat(_tmp223,sizeof(void*),0));});});}}
# 1137
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;};}}}
# 1143
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp225=0;void**topt2=_tmp225;
struct Cyc_Absyn_Tqual _tmp226=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp226;
int _tmp227=0;int nullable=_tmp227;
if(topt != 0){
void*_tmp228=Cyc_Absyn_compress(*topt);void*_stmttmp20=_tmp228;void*_tmp229=_stmttmp20;void*_tmp22C;struct Cyc_Absyn_Tqual _tmp22B;void*_tmp22A;if(*((int*)_tmp229)== 3){_tmp22A=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229)->f1).elt_type;_tmp22B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229)->f1).elt_tq;_tmp22C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229)->f1).ptr_atts).nullable;{void**elttype=(void**)_tmp22A;struct Cyc_Absyn_Tqual tq=_tmp22B;void*n=_tmp22C;
# 1150
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 1164
({struct Cyc_Tcenv_Tenv*_tmp72C=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp72B=topt2;Cyc_Tcexp_tcExpNoInst(_tmp72C,_tmp72B,e);});
# 1166
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp555;_tmp555.tag=0,({struct _fat_ptr _tmp72D=({const char*_tmp22F="cannot take the address of an alias-free path";_tag_fat(_tmp22F,sizeof(char),46U);});_tmp555.f1=_tmp72D;});_tmp555;});void*_tmp22D[1];_tmp22D[0]=& _tmp22E;({unsigned _tmp72E=e->loc;Cyc_Warn_err2(_tmp72E,_tag_fat(_tmp22D,sizeof(void*),1));});});
# 1171
{void*_tmp230=e->r;void*_stmttmp21=_tmp230;void*_tmp231=_stmttmp21;void*_tmp233;void*_tmp232;if(*((int*)_tmp231)== 23){_tmp232=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp231)->f1;_tmp233=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp231)->f2;{struct Cyc_Absyn_Exp*e1=_tmp232;struct Cyc_Absyn_Exp*e2=_tmp233;
# 1173
{void*_tmp234=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp22=_tmp234;void*_tmp235=_stmttmp22;if(*((int*)_tmp235)== 6)
goto _LLA;else{
# 1178
({void*_tmp72F=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp72F;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1181
goto _LL5;}}else{
goto _LL5;}_LL5:;}
# 1186
{void*_tmp236=e->r;void*_stmttmp23=_tmp236;void*_tmp237=_stmttmp23;switch(*((int*)_tmp237)){case 21: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp237)->f3 == 1)
goto _LL13;else{goto _LL14;}case 22: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp237)->f3 == 1){_LL13:
# 1189
({struct Cyc_Warn_String_Warn_Warg_struct _tmp239=({struct Cyc_Warn_String_Warn_Warg_struct _tmp556;_tmp556.tag=0,({struct _fat_ptr _tmp730=({const char*_tmp23A="cannot take the address of a @tagged union member";_tag_fat(_tmp23A,sizeof(char),50U);});_tmp556.f1=_tmp730;});_tmp556;});void*_tmp238[1];_tmp238[0]=& _tmp239;({unsigned _tmp731=e->loc;Cyc_Warn_err2(_tmp731,_tag_fat(_tmp238,sizeof(void*),1));});});
goto _LLF;}else{goto _LL14;}default: _LL14:
 goto _LLF;}_LLF:;}{
# 1195
struct _tuple15 _tmp23B=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp24=_tmp23B;struct _tuple15 _tmp23C=_stmttmp24;void*_tmp23E;int _tmp23D;_tmp23D=_tmp23C.f1;_tmp23E=_tmp23C.f2;{int is_const=_tmp23D;void*rgn=_tmp23E;
# 1197
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp240=({struct Cyc_Warn_String_Warn_Warg_struct _tmp557;_tmp557.tag=0,({struct _fat_ptr _tmp732=({const char*_tmp241="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp241,sizeof(char),60U);});_tmp557.f1=_tmp732;});_tmp557;});void*_tmp23F[1];_tmp23F[0]=& _tmp240;({unsigned _tmp733=e->loc;Cyc_Warn_err2(_tmp733,_tag_fat(_tmp23F,sizeof(void*),1));});});{
# 1200
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _tmp734=tq.real_const=1;tq.print_const=_tmp734;});
# 1204
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}}}}
# 1208
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1211
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _tmp737=loc;struct Cyc_Tcenv_Tenv*_tmp736=te;struct Cyc_List_List*_tmp735=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp737,_tmp736,_tmp735,& Cyc_Kinds_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp243=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55A;_tmp55A.tag=0,({struct _fat_ptr _tmp738=({const char*_tmp247="sizeof applied to type ";_tag_fat(_tmp247,sizeof(char),24U);});_tmp55A.f1=_tmp738;});_tmp55A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp244=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp559;_tmp559.tag=2,_tmp559.f1=(void*)t;_tmp559;});struct Cyc_Warn_String_Warn_Warg_struct _tmp245=({struct Cyc_Warn_String_Warn_Warg_struct _tmp558;_tmp558.tag=0,({struct _fat_ptr _tmp739=({const char*_tmp246=" which has unknown size here";_tag_fat(_tmp246,sizeof(char),29U);});_tmp558.f1=_tmp739;});_tmp558;});void*_tmp242[3];_tmp242[0]=& _tmp243,_tmp242[1]=& _tmp244,_tmp242[2]=& _tmp245;({unsigned _tmp73A=loc;Cyc_Warn_err2(_tmp73A,_tag_fat(_tmp242,sizeof(void*),3));});});
if(topt == 0)
return Cyc_Absyn_uint_type;{
void*_tmp248=Cyc_Absyn_compress(*topt);void*_stmttmp25=_tmp248;void*_tmp249=_stmttmp25;void*_tmp24A;if(*((int*)_tmp249)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f2 != 0){_tmp24A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f2)->hd;{void*tt=_tmp24A;
# 1221
if(({void*_tmp73C=tt;Cyc_Unify_unify(_tmp73C,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B->tag=9,({struct Cyc_Absyn_Exp*_tmp73B=Cyc_Absyn_sizeoftype_exp(t,0U);_tmp24B->f1=_tmp73B;});_tmp24B;}));}))
return Cyc_Absyn_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return Cyc_Absyn_uint_type;};}}
# 1228
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)
return sf->type;
return 0;}
# 1236
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1238
({unsigned _tmp73F=loc;struct Cyc_Tcenv_Tenv*_tmp73E=te;struct Cyc_List_List*_tmp73D=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp73F,_tmp73E,_tmp73D,& Cyc_Kinds_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp24C=fs;struct Cyc_List_List*l=_tmp24C;
void*_tmp24D=t_orig;void*t=_tmp24D;
for(1;l != 0;l=l->tl){
void*_tmp24E=(void*)l->hd;void*n=_tmp24E;
void*_tmp24F=n;unsigned _tmp250;void*_tmp251;if(*((int*)_tmp24F)== 0){_tmp251=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24F)->f1;{struct _fat_ptr*n=_tmp251;
# 1246
int bad_type=1;
{void*_tmp252=Cyc_Absyn_compress(t);void*_stmttmp26=_tmp252;void*_tmp253=_stmttmp26;void*_tmp254;switch(*((int*)_tmp253)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp253)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp253)->f1)->f1).KnownAggr).tag == 2){_tmp254=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp253)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl**adp=_tmp254;
# 1249
if((*adp)->impl == 0)
goto _LL5;
_tmp254=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL9;}}else{goto _LLA;}}else{goto _LLA;}case 7: _tmp254=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp253)->f2;_LL9: {struct Cyc_List_List*fields=_tmp254;
# 1253
void*t2=({(void*(*)(void*(*)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*,struct Cyc_List_List*))Cyc_List_find_c;})(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp256=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55C;_tmp55C.tag=0,({struct _fat_ptr _tmp740=({const char*_tmp258="no field of struct/union has name %s";_tag_fat(_tmp258,sizeof(char),37U);});_tmp55C.f1=_tmp740;});_tmp55C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp257=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55B;_tmp55B.tag=0,_tmp55B.f1=*n;_tmp55B;});void*_tmp255[2];_tmp255[0]=& _tmp256,_tmp255[1]=& _tmp257;({unsigned _tmp741=loc;Cyc_Warn_err2(_tmp741,_tag_fat(_tmp255,sizeof(void*),2));});});else{
# 1257
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA:
 goto _LL5;}_LL5:;}
# 1262
if(bad_type){
if(l == fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp25A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55E;_tmp55E.tag=2,_tmp55E.f1=(void*)t;_tmp55E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55D;_tmp55D.tag=0,({struct _fat_ptr _tmp742=({const char*_tmp25C=" is not a known struct/union type";_tag_fat(_tmp25C,sizeof(char),34U);});_tmp55D.f1=_tmp742;});_tmp55D;});void*_tmp259[2];_tmp259[0]=& _tmp25A,_tmp259[1]=& _tmp25B;({unsigned _tmp743=loc;Cyc_Warn_err2(_tmp743,_tag_fat(_tmp259,sizeof(void*),2));});});else{
# 1266
struct _fat_ptr _tmp25D=({struct Cyc_String_pa_PrintArg_struct _tmp273=({struct Cyc_String_pa_PrintArg_struct _tmp567;_tmp567.tag=0,({struct _fat_ptr _tmp744=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp567.f1=_tmp744;});_tmp567;});void*_tmp271[1];_tmp271[0]=& _tmp273;({struct _fat_ptr _tmp745=({const char*_tmp272="(%s)";_tag_fat(_tmp272,sizeof(char),5U);});Cyc_aprintf(_tmp745,_tag_fat(_tmp271,sizeof(void*),1));});});struct _fat_ptr s=_tmp25D;
{struct Cyc_List_List*x=fs;for(0;x != l;x=x->tl){
void*_tmp25E=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp27=_tmp25E;void*_tmp25F=_stmttmp27;unsigned _tmp260;void*_tmp261;if(*((int*)_tmp25F)== 0){_tmp261=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->f1;{struct _fat_ptr*n=_tmp261;
s=({struct Cyc_String_pa_PrintArg_struct _tmp264=({struct Cyc_String_pa_PrintArg_struct _tmp560;_tmp560.tag=0,_tmp560.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp560;});struct Cyc_String_pa_PrintArg_struct _tmp265=({struct Cyc_String_pa_PrintArg_struct _tmp55F;_tmp55F.tag=0,_tmp55F.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp55F;});void*_tmp262[2];_tmp262[0]=& _tmp264,_tmp262[1]=& _tmp265;({struct _fat_ptr _tmp746=({const char*_tmp263="%s.%s";_tag_fat(_tmp263,sizeof(char),6U);});Cyc_aprintf(_tmp746,_tag_fat(_tmp262,sizeof(void*),2));});});goto _LLC;}}else{_tmp260=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp25F)->f1;{unsigned n=_tmp260;
s=({struct Cyc_String_pa_PrintArg_struct _tmp268=({struct Cyc_String_pa_PrintArg_struct _tmp562;_tmp562.tag=0,_tmp562.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp562;});struct Cyc_Int_pa_PrintArg_struct _tmp269=({struct Cyc_Int_pa_PrintArg_struct _tmp561;_tmp561.tag=1,_tmp561.f1=(unsigned long)((int)n);_tmp561;});void*_tmp266[2];_tmp266[0]=& _tmp268,_tmp266[1]=& _tmp269;({struct _fat_ptr _tmp747=({const char*_tmp267="%s.%d";_tag_fat(_tmp267,sizeof(char),6U);});Cyc_aprintf(_tmp747,_tag_fat(_tmp266,sizeof(void*),2));});});goto _LLC;}}_LLC:;}}
# 1272
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp566;_tmp566.tag=0,_tmp566.f1=(struct _fat_ptr)s;_tmp566;});struct Cyc_Warn_String_Warn_Warg_struct _tmp26C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp565;_tmp565.tag=0,({struct _fat_ptr _tmp748=({const char*_tmp270=" == ";_tag_fat(_tmp270,sizeof(char),5U);});_tmp565.f1=_tmp748;});_tmp565;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp26D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp564;_tmp564.tag=2,_tmp564.f1=(void*)t;_tmp564;});struct Cyc_Warn_String_Warn_Warg_struct _tmp26E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp563;_tmp563.tag=0,({struct _fat_ptr _tmp749=({const char*_tmp26F=" is not a struct/union type";_tag_fat(_tmp26F,sizeof(char),28U);});_tmp563.f1=_tmp749;});_tmp563;});void*_tmp26A[4];_tmp26A[0]=& _tmp26B,_tmp26A[1]=& _tmp26C,_tmp26A[2]=& _tmp26D,_tmp26A[3]=& _tmp26E;({unsigned _tmp74A=loc;Cyc_Warn_err2(_tmp74A,_tag_fat(_tmp26A,sizeof(void*),4));});});}}
# 1275
goto _LL0;}}else{_tmp250=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp24F)->f1;{unsigned n=_tmp250;
# 1277
int bad_type=1;
{void*_tmp274=Cyc_Absyn_compress(t);void*_stmttmp28=_tmp274;void*_tmp275=_stmttmp28;void*_tmp276;switch(*((int*)_tmp275)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)){case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownAggr).tag == 2){_tmp276=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl**adp=_tmp276;
# 1280
if((*adp)->impl == 0)
goto _LL11;
_tmp276=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownDatatypefield).tag == 2){_tmp276=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypefield*tuf=_tmp276;
# 1299
if(({unsigned _tmp74B=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tuf->typs);_tmp74B < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp287=({struct Cyc_Int_pa_PrintArg_struct _tmp569;_tmp569.tag=1,({
unsigned long _tmp74C=(unsigned long)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tuf->typs);_tmp569.f1=_tmp74C;});_tmp569;});struct Cyc_Int_pa_PrintArg_struct _tmp288=({struct Cyc_Int_pa_PrintArg_struct _tmp568;_tmp568.tag=1,_tmp568.f1=(unsigned long)((int)n);_tmp568;});void*_tmp285[2];_tmp285[0]=& _tmp287,_tmp285[1]=& _tmp288;({unsigned _tmp74E=loc;struct _fat_ptr _tmp74D=({const char*_tmp286="datatype field has too few components: %d < %d";_tag_fat(_tmp286,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp74E,_tmp74D,_tag_fat(_tmp285,sizeof(void*),2));});});else{
# 1303
if(n != 0U)
t=(*({(struct _tuple17*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(tuf->typs,(int)(n - 1U))).f2;else{
if(l->tl != 0)
({void*_tmp289=0U;({unsigned _tmp750=loc;struct _fat_ptr _tmp74F=({const char*_tmp28A="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp28A,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp750,_tmp74F,_tag_fat(_tmp289,sizeof(void*),0));});});}}
# 1308
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7: _tmp276=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp275)->f2;_LL15: {struct Cyc_List_List*fields=_tmp276;
# 1284
if(({unsigned _tmp751=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(fields);_tmp751 <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp278=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56D;_tmp56D.tag=0,({struct _fat_ptr _tmp752=({const char*_tmp27D="struct/union has too few components: ";_tag_fat(_tmp27D,sizeof(char),38U);});_tmp56D.f1=_tmp752;});_tmp56D;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp279=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp56C;_tmp56C.tag=12,({
int _tmp753=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(fields);_tmp56C.f1=_tmp753;});_tmp56C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56B;_tmp56B.tag=0,({struct _fat_ptr _tmp754=({const char*_tmp27C=" <= ";_tag_fat(_tmp27C,sizeof(char),5U);});_tmp56B.f1=_tmp754;});_tmp56B;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp27B=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp56A;_tmp56A.tag=12,_tmp56A.f1=(int)n;_tmp56A;});void*_tmp277[4];_tmp277[0]=& _tmp278,_tmp277[1]=& _tmp279,_tmp277[2]=& _tmp27A,_tmp277[3]=& _tmp27B;({unsigned _tmp755=loc;Cyc_Warn_err2(_tmp755,_tag_fat(_tmp277,sizeof(void*),4));});});else{
# 1288
t=(({(struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6: _tmp276=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp275)->f1;{struct Cyc_List_List*l=_tmp276;
# 1292
if(({unsigned _tmp756=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(l);_tmp756 <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp571;_tmp571.tag=0,({struct _fat_ptr _tmp757=({const char*_tmp284="tuple has too few components: ";_tag_fat(_tmp284,sizeof(char),31U);});_tmp571.f1=_tmp757;});_tmp571;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp280=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp570;_tmp570.tag=12,({int _tmp758=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(l);_tmp570.f1=_tmp758;});_tmp570;});struct Cyc_Warn_String_Warn_Warg_struct _tmp281=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56F;_tmp56F.tag=0,({struct _fat_ptr _tmp759=({const char*_tmp283=" <= ";_tag_fat(_tmp283,sizeof(char),5U);});_tmp56F.f1=_tmp759;});_tmp56F;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp282=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp56E;_tmp56E.tag=12,_tmp56E.f1=(int)n;_tmp56E;});void*_tmp27E[4];_tmp27E[0]=& _tmp27F,_tmp27E[1]=& _tmp280,_tmp27E[2]=& _tmp281,_tmp27E[3]=& _tmp282;({unsigned _tmp75A=loc;Cyc_Warn_err2(_tmp75A,_tag_fat(_tmp27E,sizeof(void*),4));});});else{
# 1295
t=(*({(struct _tuple17*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A:
# 1310
 goto _LL11;}_LL11:;}
# 1312
if(bad_type)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp573;_tmp573.tag=2,_tmp573.f1=(void*)t;_tmp573;});struct Cyc_Warn_String_Warn_Warg_struct _tmp28D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp572;_tmp572.tag=0,({struct _fat_ptr _tmp75B=({const char*_tmp28E=" is not a known type";_tag_fat(_tmp28E,sizeof(char),21U);});_tmp572.f1=_tmp75B;});_tmp572;});void*_tmp28B[2];_tmp28B[0]=& _tmp28C,_tmp28B[1]=& _tmp28D;({unsigned _tmp75C=loc;Cyc_Warn_err2(_tmp75C,_tag_fat(_tmp28B,sizeof(void*),2));});});
goto _LL0;}}_LL0:;}
# 1317
return Cyc_Absyn_uint_type;}}
# 1321
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
{void*_tmp28F=t;if(*((int*)_tmp28F)== 1){
# 1327
struct Cyc_Absyn_PtrInfo _tmp290=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp290;
({void*_tmp75E=Cyc_Absyn_new_evar(& Cyc_Kinds_ako,({struct Cyc_Core_Opt*_tmp291=_cycalloc(sizeof(*_tmp291));({struct Cyc_List_List*_tmp75D=Cyc_Tcenv_lookup_type_vars(te);_tmp291->v=_tmp75D;});_tmp291;}));pi.elt_type=_tmp75E;});{
void*_tmp292=Cyc_Absyn_pointer_type(pi);void*new_typ=_tmp292;
Cyc_Unify_unify(t,new_typ);
t=Cyc_Absyn_compress(t);
goto _LL0;}}else{
goto _LL0;}_LL0:;}{
# 1335
void*_tmp293=t;void*_tmp297;void*_tmp296;void*_tmp295;void*_tmp294;if(*((int*)_tmp293)== 3){_tmp294=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp293)->f1).elt_type;_tmp295=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp293)->f1).ptr_atts).rgn;_tmp296=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp293)->f1).ptr_atts).bounds;_tmp297=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp293)->f1).ptr_atts).zero_term;{void*t2=_tmp294;void*rt=_tmp295;void*b=_tmp296;void*zt=_tmp297;
# 1337
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Kinds_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp298=Cyc_Absyn_compress(t2);void*_stmttmp29=_tmp298;void*_tmp299=_stmttmp29;if(*((int*)_tmp299)== 5){
# 1342
if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp574;_tmp574.tag=0,({struct _fat_ptr _tmp75F=({const char*_tmp29C="unnecessary dereference for function type";_tag_fat(_tmp29C,sizeof(char),42U);});_tmp574.f1=_tmp75F;});_tmp574;});void*_tmp29A[1];_tmp29A[0]=& _tmp29B;({unsigned _tmp760=loc;Cyc_Warn_warn2(_tmp760,_tag_fat(_tmp29A,sizeof(void*),1));});});
return t;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp575;_tmp575.tag=0,({struct _fat_ptr _tmp761=({const char*_tmp29F="cannot dereference abstract pointer type";_tag_fat(_tmp29F,sizeof(char),41U);});_tmp575.f1=_tmp761;});_tmp575;});void*_tmp29D[1];_tmp29D[0]=& _tmp29E;({unsigned _tmp762=loc;Cyc_Warn_err2(_tmp762,_tag_fat(_tmp29D,sizeof(void*),1));});});};}
# 1347
return t2;}}else{
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp577;_tmp577.tag=0,({struct _fat_ptr _tmp763=({const char*_tmp2A3="expecting pointer type but found ";_tag_fat(_tmp2A3,sizeof(char),34U);});_tmp577.f1=_tmp763;});_tmp577;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp576;_tmp576.tag=2,_tmp576.f1=(void*)t;_tmp576;});void*_tmp2A0[2];_tmp2A0[0]=& _tmp2A1,_tmp2A0[1]=& _tmp2A2;({struct Cyc_Tcenv_Tenv*_tmp766=te;unsigned _tmp765=loc;void**_tmp764=topt;Cyc_Tcexp_expr_err(_tmp766,_tmp765,_tmp764,_tag_fat(_tmp2A0,sizeof(void*),2));});});};}}}
# 1352
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1356
({int _tmp767=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp767;});{
void*_tmp2A4=Cyc_Absyn_compress(aggr_type);void*_stmttmp2A=_tmp2A4;void*_tmp2A5=_stmttmp2A;enum Cyc_Absyn_AggrKind _tmp2A6;void*_tmp2A8;void*_tmp2A7;switch(*((int*)_tmp2A5)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f1)->f1).KnownAggr).tag == 2){_tmp2A7=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f1)->f1).KnownAggr).val;_tmp2A8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_tmp2A7;struct Cyc_List_List*ts=_tmp2A8;
# 1359
struct Cyc_Absyn_Aggrfield*_tmp2A9=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2A9;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2AB=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp57B;_tmp57B.tag=6,_tmp57B.f1=ad;_tmp57B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57A;_tmp57A.tag=0,({struct _fat_ptr _tmp768=({const char*_tmp2B0=" has no ";_tag_fat(_tmp2B0,sizeof(char),9U);});_tmp57A.f1=_tmp768;});_tmp57A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp579;_tmp579.tag=0,_tmp579.f1=*f;_tmp579;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp578;_tmp578.tag=0,({struct _fat_ptr _tmp769=({const char*_tmp2AF=" member";_tag_fat(_tmp2AF,sizeof(char),8U);});_tmp578.f1=_tmp769;});_tmp578;});void*_tmp2AA[4];_tmp2AA[0]=& _tmp2AB,_tmp2AA[1]=& _tmp2AC,_tmp2AA[2]=& _tmp2AD,_tmp2AA[3]=& _tmp2AE;({struct Cyc_Tcenv_Tenv*_tmp76C=te;unsigned _tmp76B=loc;void**_tmp76A=topt;Cyc_Tcexp_expr_err(_tmp76C,_tmp76B,_tmp76A,_tag_fat(_tmp2AA,sizeof(void*),4));});});
# 1363
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp76D=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;})(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp76D,finfo->type);});{
struct Cyc_Absyn_Kind*_tmp2B1=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*t2_kind=_tmp2B1;
# 1371
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2B2=Cyc_Absyn_compress(t2);void*_stmttmp2B=_tmp2B2;void*_tmp2B3=_stmttmp2B;if(*((int*)_tmp2B3)== 4)
goto _LL7;else{
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57E;_tmp57E.tag=0,({struct _fat_ptr _tmp76E=({const char*_tmp2B9="cannot get member ";_tag_fat(_tmp2B9,sizeof(char),19U);});_tmp57E.f1=_tmp76E;});_tmp57E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57D;_tmp57D.tag=0,_tmp57D.f1=*f;_tmp57D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57C;_tmp57C.tag=0,({
struct _fat_ptr _tmp76F=({const char*_tmp2B8=" since its type is abstract";_tag_fat(_tmp2B8,sizeof(char),28U);});_tmp57C.f1=_tmp76F;});_tmp57C;});void*_tmp2B4[3];_tmp2B4[0]=& _tmp2B5,_tmp2B4[1]=& _tmp2B6,_tmp2B4[2]=& _tmp2B7;({struct Cyc_Tcenv_Tenv*_tmp772=te;unsigned _tmp771=loc;void**_tmp770=topt;Cyc_Tcexp_expr_err(_tmp772,_tmp771,_tmp770,_tag_fat(_tmp2B4,sizeof(void*),3));});});}_LL7:;}
# 1379
if(((((int)ad->kind == 1 && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0,({struct _fat_ptr _tmp773=({const char*_tmp2BF="cannot read union member ";_tag_fat(_tmp2BF,sizeof(char),26U);});_tmp581.f1=_tmp773;});_tmp581;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp580;_tmp580.tag=0,_tmp580.f1=*f;_tmp580;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57F;_tmp57F.tag=0,({
struct _fat_ptr _tmp774=({const char*_tmp2BE=" since it is not `bits-only'";_tag_fat(_tmp2BE,sizeof(char),29U);});_tmp57F.f1=_tmp774;});_tmp57F;});void*_tmp2BA[3];_tmp2BA[0]=& _tmp2BB,_tmp2BA[1]=& _tmp2BC,_tmp2BA[2]=& _tmp2BD;({struct Cyc_Tcenv_Tenv*_tmp777=te;unsigned _tmp776=loc;void**_tmp775=topt;Cyc_Tcexp_expr_err(_tmp777,_tmp776,_tmp775,_tag_fat(_tmp2BA,sizeof(void*),3));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1385
if(!({void*_tmp778=t2;Cyc_Unify_unify(_tmp778,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp584;_tmp584.tag=0,({struct _fat_ptr _tmp779=({const char*_tmp2C5="must use pattern-matching to access field ";_tag_fat(_tmp2C5,sizeof(char),43U);});_tmp584.f1=_tmp779;});_tmp584;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp583;_tmp583.tag=0,_tmp583.f1=*f;_tmp583;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp582;_tmp582.tag=0,({struct _fat_ptr _tmp77A=({const char*_tmp2C4="\n\tdue to existential type variables.";_tag_fat(_tmp2C4,sizeof(char),37U);});_tmp582.f1=_tmp77A;});_tmp582;});void*_tmp2C0[3];_tmp2C0[0]=& _tmp2C1,_tmp2C0[1]=& _tmp2C2,_tmp2C0[2]=& _tmp2C3;({struct Cyc_Tcenv_Tenv*_tmp77D=te;unsigned _tmp77C=loc;void**_tmp77B=topt;Cyc_Tcexp_expr_err(_tmp77D,_tmp77C,_tmp77B,_tag_fat(_tmp2C0,sizeof(void*),3));});});}
# 1388
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 7: _tmp2A6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A5)->f1;_tmp2A7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A5)->f2;{enum Cyc_Absyn_AggrKind k=_tmp2A6;struct Cyc_List_List*fs=_tmp2A7;
# 1390
struct Cyc_Absyn_Aggrfield*_tmp2C6=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2C6;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp587;_tmp587.tag=0,({struct _fat_ptr _tmp77E=({const char*_tmp2CC="type has no ";_tag_fat(_tmp2CC,sizeof(char),13U);});_tmp587.f1=_tmp77E;});_tmp587;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp586;_tmp586.tag=0,_tmp586.f1=*f;_tmp586;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp585;_tmp585.tag=0,({struct _fat_ptr _tmp77F=({const char*_tmp2CB=" member";_tag_fat(_tmp2CB,sizeof(char),8U);});_tmp585.f1=_tmp77F;});_tmp585;});void*_tmp2C7[3];_tmp2C7[0]=& _tmp2C8,_tmp2C7[1]=& _tmp2C9,_tmp2C7[2]=& _tmp2CA;({struct Cyc_Tcenv_Tenv*_tmp782=te;unsigned _tmp781=loc;void**_tmp780=topt;Cyc_Tcexp_expr_err(_tmp782,_tmp781,_tmp780,_tag_fat(_tmp2C7,sizeof(void*),3));});});
# 1395
if((((int)k == 1 && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1397
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58A;_tmp58A.tag=0,({struct _fat_ptr _tmp783=({const char*_tmp2D2="cannot read union member ";_tag_fat(_tmp2D2,sizeof(char),26U);});_tmp58A.f1=_tmp783;});_tmp58A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp589;_tmp589.tag=0,_tmp589.f1=*f;_tmp589;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp588;_tmp588.tag=0,({struct _fat_ptr _tmp784=({const char*_tmp2D1=" since it is not `bits-only'";_tag_fat(_tmp2D1,sizeof(char),29U);});_tmp588.f1=_tmp784;});_tmp588;});void*_tmp2CD[3];_tmp2CD[0]=& _tmp2CE,_tmp2CD[1]=& _tmp2CF,_tmp2CD[2]=& _tmp2D0;({struct Cyc_Tcenv_Tenv*_tmp787=te;unsigned _tmp786=loc;void**_tmp785=topt;Cyc_Tcexp_expr_err(_tmp787,_tmp786,_tmp785,_tag_fat(_tmp2CD,sizeof(void*),3));});});
return finfo->type;}default: _LL5:
# 1400
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58C;_tmp58C.tag=0,({struct _fat_ptr _tmp788=({const char*_tmp2D6="expecting struct or union, found ";_tag_fat(_tmp2D6,sizeof(char),34U);});_tmp58C.f1=_tmp788;});_tmp58C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2D5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58B;_tmp58B.tag=2,_tmp58B.f1=(void*)aggr_type;_tmp58B;});void*_tmp2D3[2];_tmp2D3[0]=& _tmp2D4,_tmp2D3[1]=& _tmp2D5;({struct Cyc_Tcenv_Tenv*_tmp78B=te;unsigned _tmp78A=loc;void**_tmp789=topt;Cyc_Tcexp_expr_err(_tmp78B,_tmp78A,_tmp789,_tag_fat(_tmp2D3,sizeof(void*),2));});});};}}
# 1405
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1409
({struct Cyc_Tcenv_Tenv*_tmp78C=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp78C,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,(void*)_check_null(e->topt),f,is_tagged,is_read);}
# 1414
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1418
void*t2=({struct Cyc_Tcenv_Tenv*_tmp78E=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _tmp78D=loc;Cyc_Tcexp_tcDeref(_tmp78E,_tmp78D,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1423
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1425
struct Cyc_Tcenv_Tenv*_tmp2D7=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp2D7;
({struct Cyc_Tcenv_Tenv*_tmp78F=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp78F,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp790=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp790,0,e2);});{
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58E;_tmp58E.tag=0,({struct _fat_ptr _tmp791=({const char*_tmp2DB="expecting int subscript, found ";_tag_fat(_tmp2DB,sizeof(char),32U);});_tmp58E.f1=_tmp791;});_tmp58E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58D;_tmp58D.tag=2,_tmp58D.f1=(void*)t2;_tmp58D;});void*_tmp2D8[2];_tmp2D8[0]=& _tmp2D9,_tmp2D8[1]=& _tmp2DA;({struct Cyc_Tcenv_Tenv*_tmp794=te;unsigned _tmp793=e2->loc;void**_tmp792=topt;Cyc_Tcexp_expr_err(_tmp794,_tmp793,_tmp792,_tag_fat(_tmp2D8,sizeof(void*),2));});});{
# 1434
void*_tmp2DC=t1;void*_tmp2E1;void*_tmp2E0;void*_tmp2DF;struct Cyc_Absyn_Tqual _tmp2DE;void*_tmp2DD;switch(*((int*)_tmp2DC)){case 3: _tmp2DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).elt_type;_tmp2DE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).elt_tq;_tmp2DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).ptr_atts).rgn;_tmp2E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).ptr_atts).bounds;_tmp2E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).ptr_atts).zero_term;{void*t=_tmp2DD;struct Cyc_Absyn_Tqual tq=_tmp2DE;void*rt=_tmp2DF;void*b=_tmp2E0;void*zt=_tmp2E1;
# 1438
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp2E2=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp2E2;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp2E3=eopt;struct Cyc_Absyn_Exp*e3=_tmp2E3;
struct _tuple16 _tmp2E4=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2C=_tmp2E4;struct _tuple16 _tmp2E5=_stmttmp2C;int _tmp2E7;unsigned _tmp2E6;_tmp2E6=_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;{unsigned i=_tmp2E6;int known_i=_tmp2E7;
if(known_i && i == 1U)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp2E8=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2D=_tmp2E8;struct _tuple16 _tmp2E9=_stmttmp2D;int _tmp2EB;unsigned _tmp2EA;_tmp2EA=_tmp2E9.f1;_tmp2EB=_tmp2E9.f2;{unsigned j=_tmp2EA;int known_j=_tmp2EB;
if(known_j){
struct _tuple16 _tmp2EC=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2E=_tmp2EC;struct _tuple16 _tmp2ED=_stmttmp2E;int _tmp2EF;unsigned _tmp2EE;_tmp2EE=_tmp2ED.f1;_tmp2EF=_tmp2ED.f2;{unsigned j=_tmp2EE;int knownj=_tmp2EF;
if((known_i && j > i)&& i != 1U)
({void*_tmp2F0=0U;({unsigned _tmp796=loc;struct _fat_ptr _tmp795=({const char*_tmp2F1="subscript will fail at run-time";_tag_fat(_tmp2F1,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp796,_tmp795,_tag_fat(_tmp2F0,sizeof(void*),0));});});}}}}}}
# 1454
if(emit_warning)
({void*_tmp2F2=0U;({unsigned _tmp798=e2->loc;struct _fat_ptr _tmp797=({const char*_tmp2F3="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp2F3,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp798,_tmp797,_tag_fat(_tmp2F2,sizeof(void*),0));});});}else{
# 1458
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp2F4=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2F=_tmp2F4;struct _tuple16 _tmp2F5=_stmttmp2F;int _tmp2F7;unsigned _tmp2F6;_tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;{unsigned i=_tmp2F6;int known=_tmp2F7;
if(known)
# 1463
({unsigned _tmp79B=loc;unsigned _tmp79A=i;void*_tmp799=b;Cyc_Tcutil_check_bound(_tmp79B,_tmp79A,_tmp799,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1466
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp2F8=0U;({unsigned _tmp79D=e1->loc;struct _fat_ptr _tmp79C=({const char*_tmp2F9="subscript applied to pointer to one object";_tag_fat(_tmp2F9,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp79D,_tmp79C,_tag_fat(_tmp2F8,sizeof(void*),0));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1471
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58F;_tmp58F.tag=0,({struct _fat_ptr _tmp79E=({const char*_tmp2FC="can't subscript an abstract pointer";_tag_fat(_tmp2FC,sizeof(char),36U);});_tmp58F.f1=_tmp79E;});_tmp58F;});void*_tmp2FA[1];_tmp2FA[0]=& _tmp2FB;({unsigned _tmp79F=e1->loc;Cyc_Warn_err2(_tmp79F,_tag_fat(_tmp2FA,sizeof(void*),1));});});
return t;}case 6: _tmp2DD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2DC)->f1;{struct Cyc_List_List*ts=_tmp2DD;
# 1476
struct _tuple16 _tmp2FD=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp30=_tmp2FD;struct _tuple16 _tmp2FE=_stmttmp30;int _tmp300;unsigned _tmp2FF;_tmp2FF=_tmp2FE.f1;_tmp300=_tmp2FE.f2;{unsigned i=_tmp2FF;int known=_tmp300;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp302=({struct Cyc_Warn_String_Warn_Warg_struct _tmp590;_tmp590.tag=0,({
struct _fat_ptr _tmp7A0=({const char*_tmp303="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp303,sizeof(char),47U);});_tmp590.f1=_tmp7A0;});_tmp590;});void*_tmp301[1];_tmp301[0]=& _tmp302;({struct Cyc_Tcenv_Tenv*_tmp7A2=te;unsigned _tmp7A1=loc;Cyc_Tcexp_expr_err(_tmp7A2,_tmp7A1,0,_tag_fat(_tmp301,sizeof(void*),1));});});{
struct _handler_cons _tmp304;_push_handler(& _tmp304);{int _tmp306=0;if(setjmp(_tmp304.handler))_tmp306=1;if(!_tmp306){{void*_tmp307=(*({(struct _tuple17*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(ts,(int)i)).f2;_npop_handler(0);return _tmp307;};_pop_handler();}else{void*_tmp305=(void*)Cyc_Core_get_exn_thrown();void*_tmp308=_tmp305;void*_tmp309;if(((struct Cyc_List_Nth_exn_struct*)_tmp308)->tag == Cyc_List_Nth)
# 1482
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp30B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp595;_tmp595.tag=0,({struct _fat_ptr _tmp7A3=({const char*_tmp312="index is ";_tag_fat(_tmp312,sizeof(char),10U);});_tmp595.f1=_tmp7A3;});_tmp595;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp30C=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp594;_tmp594.tag=12,_tmp594.f1=(int)i;_tmp594;});struct Cyc_Warn_String_Warn_Warg_struct _tmp30D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp593;_tmp593.tag=0,({struct _fat_ptr _tmp7A4=({const char*_tmp311=" but tuple has only ";_tag_fat(_tmp311,sizeof(char),21U);});_tmp593.f1=_tmp7A4;});_tmp593;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp30E=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp592;_tmp592.tag=12,({
int _tmp7A5=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ts);_tmp592.f1=_tmp7A5;});_tmp592;});struct Cyc_Warn_String_Warn_Warg_struct _tmp30F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp591;_tmp591.tag=0,({struct _fat_ptr _tmp7A6=({const char*_tmp310=" fields";_tag_fat(_tmp310,sizeof(char),8U);});_tmp591.f1=_tmp7A6;});_tmp591;});void*_tmp30A[5];_tmp30A[0]=& _tmp30B,_tmp30A[1]=& _tmp30C,_tmp30A[2]=& _tmp30D,_tmp30A[3]=& _tmp30E,_tmp30A[4]=& _tmp30F;({struct Cyc_Tcenv_Tenv*_tmp7A8=te;unsigned _tmp7A7=loc;Cyc_Tcexp_expr_err(_tmp7A8,_tmp7A7,0,_tag_fat(_tmp30A,sizeof(void*),5));});});else{_tmp309=_tmp308;{void*exn=_tmp309;(int)_rethrow(exn);}};}}}}}default:
# 1485
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp314=({struct Cyc_Warn_String_Warn_Warg_struct _tmp597;_tmp597.tag=0,({struct _fat_ptr _tmp7A9=({const char*_tmp316="subscript applied to ";_tag_fat(_tmp316,sizeof(char),22U);});_tmp597.f1=_tmp7A9;});_tmp597;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp315=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp596;_tmp596.tag=2,_tmp596.f1=(void*)t1;_tmp596;});void*_tmp313[2];_tmp313[0]=& _tmp314,_tmp313[1]=& _tmp315;({struct Cyc_Tcenv_Tenv*_tmp7AC=te;unsigned _tmp7AB=loc;void**_tmp7AA=topt;Cyc_Tcexp_expr_err(_tmp7AC,_tmp7AB,_tmp7AA,_tag_fat(_tmp313,sizeof(void*),2));});});};}}}
# 1490
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp317=Cyc_Absyn_compress(*topt);void*_stmttmp31=_tmp317;void*_tmp318=_stmttmp31;void*_tmp319;if(*((int*)_tmp318)== 6){_tmp319=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp318)->f1;{struct Cyc_List_List*ts=_tmp319;
# 1497
if(({int _tmp7AD=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ts);_tmp7AD != ({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(es);}))
goto _LL0;
for(1;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp31A=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp31A;
({struct Cyc_Tcenv_Tenv*_tmp7AE=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7AE,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1504
({struct Cyc_RgnOrder_RgnPO*_tmp7B0=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7AF=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp7B0,_tmp7AF,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp31C=_cycalloc(sizeof(*_tmp31C));({struct _tuple17*_tmp7B1=({struct _tuple17*_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp31B->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp31B;});_tmp31C->hd=_tmp7B1;}),_tmp31C->tl=fields;_tmp31C;});}
# 1507
done=1;
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 1511
if(!done)
for(1;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp7B2=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7B2,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp31E=_cycalloc(sizeof(*_tmp31E));({struct _tuple17*_tmp7B4=({struct _tuple17*_tmp31D=_cycalloc(sizeof(*_tmp31D));({struct Cyc_Absyn_Tqual _tmp7B3=Cyc_Absyn_empty_tqual(0U);_tmp31D->f1=_tmp7B3;}),_tmp31D->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp31D;});_tmp31E->hd=_tmp7B4;}),_tmp31E->tl=fields;_tmp31E;});}
# 1516
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->tag=6,({struct Cyc_List_List*_tmp7B5=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fields);_tmp31F->f1=_tmp7B5;});_tmp31F;});}
# 1520
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1525
struct _tuple9*_tmp320=targ;void*_tmp321;_tmp321=_tmp320->f3;{void*t=_tmp321;
({unsigned _tmp7B9=loc;struct Cyc_Tcenv_Tenv*_tmp7B8=te;struct Cyc_List_List*_tmp7B7=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp7B6=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_tak:& Cyc_Kinds_tmk;
# 1526
Cyc_Tctyp_check_type(_tmp7B9,_tmp7B8,_tmp7B7,_tmp7B6,1,0,t);});
# 1528
({void*_tmp7BA=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->tag=36,_tmp322->f1=0,_tmp322->f2=des;_tmp322;});orig_exp->r=_tmp7BA;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1541 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1544
void*res_t2;
int _tmp323=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(des);int num_es=_tmp323;
struct Cyc_List_List*es=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple18*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd;}),des);
void*res=Cyc_Absyn_new_evar(& Cyc_Kinds_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp324=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342->tag=0,({union Cyc_Absyn_Cnst _tmp7BB=Cyc_Absyn_Int_c(1U,num_es);_tmp342->f1=_tmp7BB;});_tmp342;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp324;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1552
if(zero_term){
struct Cyc_Absyn_Exp*_tmp325=({(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp325;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp326=0U;({unsigned _tmp7BD=e->loc;struct _fat_ptr _tmp7BC=({const char*_tmp327="zero-terminated array doesn't end with zero.";_tag_fat(_tmp327,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp7BD,_tmp7BC,_tag_fat(_tmp326,sizeof(void*),0));});});}
# 1557
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp7C0=res;struct Cyc_Absyn_Tqual _tmp7BF=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1558
struct Cyc_Absyn_Exp*_tmp7BE=sz_exp;Cyc_Absyn_array_type(_tmp7C0,_tmp7BF,_tmp7BE,
# 1560
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1562
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1565
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7C2=Cyc_Tcenv_curr_rgnpo(te);void*_tmp7C1=res;Cyc_Tcutil_coerce_list(_tmp7C2,_tmp7C1,es);}))
# 1567
({struct Cyc_Warn_String_Warn_Warg_struct _tmp329=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59A;_tmp59A.tag=0,({struct _fat_ptr _tmp7C3=({const char*_tmp32D="elements of array do not all have the same type (";_tag_fat(_tmp32D,sizeof(char),50U);});_tmp59A.f1=_tmp7C3;});_tmp59A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp32A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp599;_tmp599.tag=2,_tmp599.f1=(void*)res;_tmp599;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp598;_tmp598.tag=0,({
struct _fat_ptr _tmp7C4=({const char*_tmp32C=")";_tag_fat(_tmp32C,sizeof(char),2U);});_tmp598.f1=_tmp7C4;});_tmp598;});void*_tmp328[3];_tmp328[0]=& _tmp329,_tmp328[1]=& _tmp32A,_tmp328[2]=& _tmp32B;({unsigned _tmp7C5=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;Cyc_Warn_err2(_tmp7C5,_tag_fat(_tmp328,sizeof(void*),3));});});
# 1570
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
void*_tmp32E=(void*)ds->hd;void*_stmttmp32=_tmp32E;void*_tmp32F=_stmttmp32;void*_tmp330;if(*((int*)_tmp32F)== 1){
# 1575
({struct Cyc_Warn_String_Warn_Warg_struct _tmp332=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59B;_tmp59B.tag=0,({struct _fat_ptr _tmp7C6=({const char*_tmp333="only array index designators are supported";_tag_fat(_tmp333,sizeof(char),43U);});_tmp59B.f1=_tmp7C6;});_tmp59B;});void*_tmp331[1];_tmp331[0]=& _tmp332;({unsigned _tmp7C7=loc;Cyc_Warn_err2(_tmp7C7,_tag_fat(_tmp331,sizeof(void*),1));});});goto _LL0;}else{_tmp330=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp32F)->f1;{struct Cyc_Absyn_Exp*e=_tmp330;
# 1577
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp334=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp33=_tmp334;struct _tuple16 _tmp335=_stmttmp33;int _tmp337;unsigned _tmp336;_tmp336=_tmp335.f1;_tmp337=_tmp335.f2;{unsigned i=_tmp336;int known=_tmp337;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp339=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59C;_tmp59C.tag=0,({struct _fat_ptr _tmp7C8=({const char*_tmp33A="index designator cannot use sizeof or offsetof";_tag_fat(_tmp33A,sizeof(char),47U);});_tmp59C.f1=_tmp7C8;});_tmp59C;});void*_tmp338[1];_tmp338[0]=& _tmp339;({unsigned _tmp7C9=e->loc;Cyc_Warn_err2(_tmp7C9,_tag_fat(_tmp338,sizeof(void*),1));});});else{
if(i != (unsigned)offset)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp33C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=0,({struct _fat_ptr _tmp7CA=({const char*_tmp341="expecting index designator ";_tag_fat(_tmp341,sizeof(char),28U);});_tmp5A0.f1=_tmp7CA;});_tmp5A0;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp33D=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp59F;_tmp59F.tag=12,_tmp59F.f1=offset;_tmp59F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp33E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59E;_tmp59E.tag=0,({struct _fat_ptr _tmp7CB=({const char*_tmp340=" but found ";_tag_fat(_tmp340,sizeof(char),12U);});_tmp59E.f1=_tmp7CB;});_tmp59E;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp33F=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp59D;_tmp59D.tag=12,_tmp59D.f1=(int)i;_tmp59D;});void*_tmp33B[4];_tmp33B[0]=& _tmp33C,_tmp33B[1]=& _tmp33D,_tmp33B[2]=& _tmp33E,_tmp33B[3]=& _tmp33F;({unsigned _tmp7CC=e->loc;Cyc_Warn_err2(_tmp7CC,_tag_fat(_tmp33B,sizeof(void*),4));});});}
goto _LL0;}}}}_LL0:;}}}
# 1587
return res_t2;}struct _tuple19{void**f1;struct Cyc_Absyn_Tqual*f2;void**f3;};
# 1592
static struct _tuple19 Cyc_Tcexp_ptrEltAtts(void**topt){
if(!((unsigned)topt))
return({struct _tuple19 _tmp5A1;_tmp5A1.f1=0,_tmp5A1.f2=0,_tmp5A1.f3=0;_tmp5A1;});{
void*_tmp343=Cyc_Absyn_compress(*topt);void*_stmttmp34=_tmp343;void*_tmp344=_stmttmp34;struct Cyc_Absyn_ArrayInfo _tmp345;struct Cyc_Absyn_PtrInfo _tmp346;switch(*((int*)_tmp344)){case 3: _tmp346=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp344)->f1;{struct Cyc_Absyn_PtrInfo x=_tmp346;
# 1597
return({struct _tuple19 _tmp5A2;({void**_tmp7CF=({void**_tmp347=_cycalloc(sizeof(*_tmp347));*_tmp347=x.elt_type;_tmp347;});_tmp5A2.f1=_tmp7CF;}),({struct Cyc_Absyn_Tqual*_tmp7CE=({struct Cyc_Absyn_Tqual*_tmp348=_cycalloc(sizeof(*_tmp348));*_tmp348=x.elt_tq;_tmp348;});_tmp5A2.f2=_tmp7CE;}),({void**_tmp7CD=({void**_tmp349=_cycalloc(sizeof(*_tmp349));*_tmp349=(x.ptr_atts).zero_term;_tmp349;});_tmp5A2.f3=_tmp7CD;});_tmp5A2;});}case 4: _tmp345=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp344)->f1;{struct Cyc_Absyn_ArrayInfo x=_tmp345;
return({struct _tuple19 _tmp5A3;({void**_tmp7D2=({void**_tmp34A=_cycalloc(sizeof(*_tmp34A));*_tmp34A=x.elt_type;_tmp34A;});_tmp5A3.f1=_tmp7D2;}),({struct Cyc_Absyn_Tqual*_tmp7D1=({struct Cyc_Absyn_Tqual*_tmp34B=_cycalloc(sizeof(*_tmp34B));*_tmp34B=x.tq;_tmp34B;});_tmp5A3.f2=_tmp7D1;}),({void**_tmp7D0=({void**_tmp34C=_cycalloc(sizeof(*_tmp34C));*_tmp34C=x.zero_term;_tmp34C;});_tmp5A3.f3=_tmp7D0;});_tmp5A3;});}default:
 return({struct _tuple19 _tmp5A4;_tmp5A4.f1=0,_tmp5A4.f2=0,_tmp5A4.f3=0;_tmp5A4;});};}}
# 1604
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1607
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp34D=Cyc_Absyn_compress((void*)_check_null(bound->topt));void*_stmttmp35=_tmp34D;void*_tmp34E=_stmttmp35;void*_tmp34F;if(*((int*)_tmp34E)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E)->f2 != 0){_tmp34F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E)->f2)->hd;{void*t=_tmp34F;
# 1612
if((int)Cyc_Tcenv_new_status(te)== 2){
struct Cyc_Absyn_Exp*_tmp350=({void*_tmp7D3=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7D3,Cyc_Absyn_valueof_exp(t,0U),0,1U,0U);});struct Cyc_Absyn_Exp*b=_tmp350;
b->topt=bound->topt;
bound=b;}
# 1617
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1619
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp352=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=0,({struct _fat_ptr _tmp7D4=({const char*_tmp354="expecting unsigned int, found ";_tag_fat(_tmp354,sizeof(char),31U);});_tmp5A6.f1=_tmp7D4;});_tmp5A6;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp353=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=3,_tmp5A5.f1=(void*)bound->topt;_tmp5A5;});void*_tmp351[2];_tmp351[0]=& _tmp352,_tmp351[1]=& _tmp353;({unsigned _tmp7D5=bound->loc;Cyc_Warn_err2(_tmp7D5,_tag_fat(_tmp351,sizeof(void*),2));});});}_LL0:;}
# 1623
if(!(vd->tq).real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp356=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=0,({struct _fat_ptr _tmp7D6=({const char*_tmp357="comprehension index variable is not declared const!";_tag_fat(_tmp357,sizeof(char),52U);});_tmp5A7.f1=_tmp7D6;});_tmp5A7;});void*_tmp355[1];_tmp355[0]=& _tmp356;({int(*_tmp7D7)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp7D7(_tag_fat(_tmp355,sizeof(void*),1));});});
# 1626
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
# 1629
struct _tuple19 _tmp358=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp36=_tmp358;struct _tuple19 _tmp359=_stmttmp36;void*_tmp35C;void*_tmp35B;void*_tmp35A;_tmp35A=_tmp359.f1;_tmp35B=_tmp359.f2;_tmp35C=_tmp359.f3;{void**topt2=_tmp35A;struct Cyc_Absyn_Tqual*tqopt=_tmp35B;void**ztopt=_tmp35C;
# 1632
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1634
Cyc_Tcexp_check_consume(body->loc,t,body);
if(te->le == 0){
# 1637
if(!Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp35E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=0,({struct _fat_ptr _tmp7D8=({const char*_tmp35F="bound is not constant";_tag_fat(_tmp35F,sizeof(char),22U);});_tmp5A8.f1=_tmp7D8;});_tmp5A8;});void*_tmp35D[1];_tmp35D[0]=& _tmp35E;({unsigned _tmp7D9=bound->loc;Cyc_Warn_err2(_tmp7D9,_tag_fat(_tmp35D,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_const_exp(body))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp361=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=0,({struct _fat_ptr _tmp7DA=({const char*_tmp362="body is not constant";_tag_fat(_tmp362,sizeof(char),21U);});_tmp5A9.f1=_tmp7DA;});_tmp5A9;});void*_tmp360[1];_tmp360[0]=& _tmp361;({unsigned _tmp7DB=body->loc;Cyc_Warn_err2(_tmp7DB,_tag_fat(_tmp360,sizeof(void*),1));});});}
# 1642
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1644
struct Cyc_Absyn_Exp*_tmp363=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp363;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1648
Cyc_Tcexp_check_consume(body->loc,(void*)_check_null(body->topt),body);
# 1650
return({void*_tmp7DE=t;struct Cyc_Absyn_Tqual _tmp7DD=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp7DC=bound;Cyc_Absyn_array_type(_tmp7DE,_tmp7DD,_tmp7DC,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1655
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1658
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp364=Cyc_Absyn_compress((void*)_check_null(bound->topt));void*_stmttmp37=_tmp364;void*_tmp365=_stmttmp37;void*_tmp366;if(*((int*)_tmp365)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f2 != 0){_tmp366=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f2)->hd;{void*t=_tmp366;
# 1663
if((int)Cyc_Tcenv_new_status(te)== 2){
struct Cyc_Absyn_Exp*_tmp367=({void*_tmp7DF=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7DF,Cyc_Absyn_valueof_exp(t,0U),0,1U,0U);});struct Cyc_Absyn_Exp*b=_tmp367;
b->topt=bound->topt;
bound=b;}
# 1668
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1670
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp369=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AB;_tmp5AB.tag=0,({struct _fat_ptr _tmp7E0=({const char*_tmp36B="expecting unsigned int, found ";_tag_fat(_tmp36B,sizeof(char),31U);});_tmp5AB.f1=_tmp7E0;});_tmp5AB;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp36A=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=3,_tmp5AA.f1=(void*)bound->topt;_tmp5AA;});void*_tmp368[2];_tmp368[0]=& _tmp369,_tmp368[1]=& _tmp36A;({unsigned _tmp7E1=bound->loc;Cyc_Warn_err2(_tmp7E1,_tag_fat(_tmp368,sizeof(void*),2));});});}_LL0:;}{
# 1673
struct _tuple19 _tmp36C=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp38=_tmp36C;struct _tuple19 _tmp36D=_stmttmp38;void*_tmp370;void*_tmp36F;void*_tmp36E;_tmp36E=_tmp36D.f1;_tmp36F=_tmp36D.f2;_tmp370=_tmp36D.f3;{void**topt2=_tmp36E;struct Cyc_Absyn_Tqual*tqopt=_tmp36F;void**ztopt=_tmp370;
# 1676
({unsigned _tmp7E4=loc;struct Cyc_Tcenv_Tenv*_tmp7E3=te;struct Cyc_List_List*_tmp7E2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7E4,_tmp7E3,_tmp7E2,& Cyc_Kinds_tmk,1,1,t);});
if(topt2 != 0)
Cyc_Unify_unify(*topt2,t);
# 1680
if(te->le == 0 && !Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp372=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AC;_tmp5AC.tag=0,({struct _fat_ptr _tmp7E5=({const char*_tmp373="bound is not constant";_tag_fat(_tmp373,sizeof(char),22U);});_tmp5AC.f1=_tmp7E5;});_tmp5AC;});void*_tmp371[1];_tmp371[0]=& _tmp372;({unsigned _tmp7E6=bound->loc;Cyc_Warn_err2(_tmp7E6,_tag_fat(_tmp371,sizeof(void*),1));});});
# 1684
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp375=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AD;_tmp5AD.tag=0,({struct _fat_ptr _tmp7E7=({const char*_tmp376="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp376,sizeof(char),74U);});_tmp5AD.f1=_tmp7E7;});_tmp5AD;});void*_tmp374[1];_tmp374[0]=& _tmp375;({unsigned _tmp7E8=loc;Cyc_Warn_err2(_tmp7E8,_tag_fat(_tmp374,sizeof(void*),1));});});
# 1688
return({void*_tmp7EB=t;struct Cyc_Absyn_Tqual _tmp7EA=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp7E9=bound;Cyc_Absyn_array_type(_tmp7EB,_tmp7EA,_tmp7E9,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1692
static int Cyc_Tcexp_ensure_typqual_nothrow(void*t){
void*_tmp377=t;void*_tmp378;switch(*((int*)_tmp377)){case 5: _tmp378=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp377)->f1;{struct Cyc_Absyn_FnInfo*i=_tmp378;
# 1695
return({(int(*)(int(*)(void*,void*),struct Cyc_List_List*,void*))Cyc_List_mem;})(Cyc_Atts_attribute_cmp,i->attributes,(void*)& Cyc_Atts_No_throw_att_val);}case 3: _tmp378=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377)->f1;{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_tmp378;
# 1699
return Cyc_Tcexp_ensure_typqual_nothrow(pi->elt_type);}default:
# 1701
 return 1;};}
# 1705
static int Cyc_Tcexp_ensure_nothrow_initializer(struct Cyc_Absyn_Exp*e){
void*_tmp379=e->r;void*_stmttmp39=_tmp379;void*_tmp37A=_stmttmp39;void*_tmp37B;if(*((int*)_tmp37A)== 14){_tmp37B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp37A)->f2;{struct Cyc_Absyn_Exp*e1=_tmp37B;
return Cyc_Tcexp_ensure_nothrow_initializer(e1);}}else{
return Cyc_Tcexp_ensure_typqual_nothrow((void*)_check_null(e->topt));};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1722 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1726
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp37C=_cycalloc(sizeof(*_tmp37C));*_tmp37C=ad;_tmp37C;});}else{
# 1732
{struct _handler_cons _tmp37D;_push_handler(& _tmp37D);{int _tmp37F=0;if(setjmp(_tmp37D.handler))_tmp37F=1;if(!_tmp37F){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1732
;_pop_handler();}else{void*_tmp37E=(void*)Cyc_Core_get_exn_thrown();void*_tmp380=_tmp37E;void*_tmp381;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp380)->tag == Cyc_Dict_Absent){
# 1735
({struct Cyc_Warn_String_Warn_Warg_struct _tmp383=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AF;_tmp5AF.tag=0,({struct _fat_ptr _tmp7EC=({const char*_tmp385="unbound struct/union name ";_tag_fat(_tmp385,sizeof(char),27U);});_tmp5AF.f1=_tmp7EC;});_tmp5AF;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp384=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5AE;_tmp5AE.tag=1,_tmp5AE.f1=*tn;_tmp5AE;});void*_tmp382[2];_tmp382[0]=& _tmp383,_tmp382[1]=& _tmp384;({unsigned _tmp7ED=loc;Cyc_Warn_err2(_tmp7ED,_tag_fat(_tmp382,sizeof(void*),2));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_tmp381=_tmp380;{void*exn=_tmp381;(int)_rethrow(exn);}};}}}
# 1738
*ad_opt=ad;
*tn=ad->name;}
# 1741
if(ad->impl == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp387=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B1;_tmp5B1.tag=0,({struct _fat_ptr _tmp7EE=({const char*_tmp389="can't construct abstract ";_tag_fat(_tmp389,sizeof(char),26U);});_tmp5B1.f1=_tmp7EE;});_tmp5B1;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp388=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5B0;_tmp5B0.tag=6,_tmp5B0.f1=ad;_tmp5B0;});void*_tmp386[2];_tmp386[0]=& _tmp387,_tmp386[1]=& _tmp388;({struct Cyc_Tcenv_Tenv*_tmp7F0=te;unsigned _tmp7EF=loc;Cyc_Tcexp_expr_err(_tmp7F0,_tmp7EF,0,_tag_fat(_tmp386,sizeof(void*),2));});});{
# 1748
struct Cyc_Tcenv_Tenv*_tmp38A=(int)Cyc_Tcenv_new_status(te)== 1?Cyc_Tcenv_set_new_status(2U,te): te;struct Cyc_Tcenv_Tenv*te2=_tmp38A;
# 1754
struct _tuple13 _tmp38B=({struct _tuple13 _tmp5BE;({struct Cyc_List_List*_tmp7F1=Cyc_Tcenv_lookup_type_vars(te2);_tmp5BE.f1=_tmp7F1;}),_tmp5BE.f2=Cyc_Core_heap_region;_tmp5BE;});struct _tuple13 env=_tmp38B;
struct Cyc_List_List*_tmp38C=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp38C;
struct Cyc_List_List*_tmp38D=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp38D;
struct Cyc_List_List*_tmp38E=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),all_inst);struct Cyc_List_List*all_typs=_tmp38E;
struct Cyc_List_List*_tmp38F=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),exist_inst);struct Cyc_List_List*exist_typs=_tmp38F;
struct Cyc_List_List*_tmp390=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp390;
void*res_typ;
# 1765
if(topt != 0){
void*_tmp391=Cyc_Absyn_compress(*topt);void*_stmttmp3A=_tmp391;void*_tmp392=_stmttmp3A;void*_tmp394;void*_tmp393;if(*((int*)_tmp392)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f1)->f1).KnownAggr).tag == 2){_tmp393=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f1)->f1).KnownAggr).val;_tmp394=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f2;{struct Cyc_Absyn_Aggrdecl**adptr2=_tmp393;struct Cyc_List_List*all_typs2=_tmp394;
# 1768
if(*adptr2 != *adptr)
goto _LL9;
{struct Cyc_List_List*_tmp395=all_typs;struct Cyc_List_List*ats=_tmp395;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1774
res_typ=*topt;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 res_typ=({union Cyc_Absyn_AggrInfo _tmp7F2=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp7F2,all_typs);});}_LL5:;}else{
# 1779
res_typ=({union Cyc_Absyn_AggrInfo _tmp7F3=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp7F3,all_typs);});}{
# 1782
struct Cyc_List_List*_tmp396=*ts;struct Cyc_List_List*user_ex_ts=_tmp396;
struct Cyc_List_List*_tmp397=exist_typs;struct Cyc_List_List*ex_ts=_tmp397;
while(user_ex_ts != 0 && ex_ts != 0){
# 1786
({unsigned _tmp7F6=loc;struct Cyc_Tcenv_Tenv*_tmp7F5=te2;struct Cyc_List_List*_tmp7F4=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp7F6,_tmp7F5,_tmp7F4,& Cyc_Kinds_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1792
if(user_ex_ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp399=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B2;_tmp5B2.tag=0,({struct _fat_ptr _tmp7F7=({const char*_tmp39A="too many explicit witness types";_tag_fat(_tmp39A,sizeof(char),32U);});_tmp5B2.f1=_tmp7F7;});_tmp5B2;});void*_tmp398[1];_tmp398[0]=& _tmp399;({unsigned _tmp7F8=loc;Cyc_Warn_err2(_tmp7F8,_tag_fat(_tmp398,sizeof(void*),1));});});
# 1795
*ts=exist_typs;{
# 1798
struct Cyc_List_List*fields=
({(struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators;})(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(1;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp39B=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3B=_tmp39B;struct _tuple20*_tmp39C=_stmttmp3B;void*_tmp39E;void*_tmp39D;_tmp39D=_tmp39C->f1;_tmp39E=_tmp39C->f2;{struct Cyc_Absyn_Aggrfield*field=_tmp39D;struct Cyc_Absyn_Exp*field_exp=_tmp39E;
void*_tmp39F=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp39F;
void*_tmp3A0=({struct Cyc_Tcenv_Tenv*_tmp7F9=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp7F9,& inst_fieldtyp,field_exp);});void*ftyp=_tmp3A0;
# 1806
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7FB=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp7FA=field_exp;Cyc_Tcutil_coerce_arg(_tmp7FB,_tmp7FA,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BA;_tmp5BA.tag=0,({struct _fat_ptr _tmp7FC=({const char*_tmp3AD="field ";_tag_fat(_tmp3AD,sizeof(char),7U);});_tmp5BA.f1=_tmp7FC;});_tmp5BA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B9;_tmp5B9.tag=0,_tmp5B9.f1=*field->name;_tmp5B9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3A4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B8;_tmp5B8.tag=0,({struct _fat_ptr _tmp7FD=({const char*_tmp3AC=" of ";_tag_fat(_tmp3AC,sizeof(char),5U);});_tmp5B8.f1=_tmp7FD;});_tmp5B8;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3A5=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5B7;_tmp5B7.tag=6,_tmp5B7.f1=ad;_tmp5B7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B6;_tmp5B6.tag=0,({
struct _fat_ptr _tmp7FE=({const char*_tmp3AB=" expects type ";_tag_fat(_tmp3AB,sizeof(char),15U);});_tmp5B6.f1=_tmp7FE;});_tmp5B6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3A7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B5;_tmp5B5.tag=2,_tmp5B5.f1=(void*)inst_fieldtyp;_tmp5B5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B4;_tmp5B4.tag=0,({struct _fat_ptr _tmp7FF=({const char*_tmp3AA=" != ";_tag_fat(_tmp3AA,sizeof(char),5U);});_tmp5B4.f1=_tmp7FF;});_tmp5B4;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3A9=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5B3;_tmp5B3.tag=3,_tmp5B3.f1=(void*)field_exp->topt;_tmp5B3;});void*_tmp3A1[8];_tmp3A1[0]=& _tmp3A2,_tmp3A1[1]=& _tmp3A3,_tmp3A1[2]=& _tmp3A4,_tmp3A1[3]=& _tmp3A5,_tmp3A1[4]=& _tmp3A6,_tmp3A1[5]=& _tmp3A7,_tmp3A1[6]=& _tmp3A8,_tmp3A1[7]=& _tmp3A9;({unsigned _tmp800=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp800,_tag_fat(_tmp3A1,sizeof(void*),8));});});{
# 1810
int nothrow_fld=({(int(*)(int(*)(void*,void*),struct Cyc_List_List*,void*))Cyc_List_mem;})(Cyc_Atts_attribute_cmp,field->attributes,(void*)& Cyc_Atts_No_throw_att_val);
# 1813
if(nothrow_fld && !Cyc_Tcexp_ensure_nothrow_initializer(field_exp))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BD;_tmp5BD.tag=0,({struct _fat_ptr _tmp801=({const char*_tmp3B3="field ";_tag_fat(_tmp3B3,sizeof(char),7U);});_tmp5BD.f1=_tmp801;});_tmp5BD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BC;_tmp5BC.tag=0,_tmp5BC.f1=*field->name;_tmp5BC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BB;_tmp5BB.tag=0,({struct _fat_ptr _tmp802=({const char*_tmp3B2=" can only hold a no_throw function";_tag_fat(_tmp3B2,sizeof(char),35U);});_tmp5BB.f1=_tmp802;});_tmp5BB;});void*_tmp3AE[3];_tmp3AE[0]=& _tmp3AF,_tmp3AE[1]=& _tmp3B0,_tmp3AE[2]=& _tmp3B1;({unsigned _tmp803=field_exp->loc;Cyc_Warn_err2(_tmp803,_tag_fat(_tmp3AE,sizeof(void*),3));});});}}}{
# 1817
struct Cyc_List_List*_tmp3B4=0;struct Cyc_List_List*rpo_inst=_tmp3B4;
{struct Cyc_List_List*_tmp3B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3B5;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({struct _tuple0*_tmp806=({struct _tuple0*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));({void*_tmp805=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3B6->f1=_tmp805;}),({
void*_tmp804=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3B6->f2=_tmp804;});_tmp3B6;});
# 1819
_tmp3B7->hd=_tmp806;}),_tmp3B7->tl=rpo_inst;_tmp3B7;});}}
# 1822
rpo_inst=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}
# 1828
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1830
{void*_tmp3B8=Cyc_Absyn_compress(ts);void*_stmttmp3C=_tmp3B8;void*_tmp3B9=_stmttmp3C;void*_tmp3BB;enum Cyc_Absyn_AggrKind _tmp3BA;if(*((int*)_tmp3B9)== 7){_tmp3BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B9)->f1;_tmp3BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B9)->f2;{enum Cyc_Absyn_AggrKind k=_tmp3BA;struct Cyc_List_List*fs=_tmp3BB;
# 1832
if((int)k == 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BF;_tmp5BF.tag=0,({struct _fat_ptr _tmp807=({const char*_tmp3BE="expecting struct but found union";_tag_fat(_tmp3BE,sizeof(char),33U);});_tmp5BF.f1=_tmp807;});_tmp5BF;});void*_tmp3BC[1];_tmp3BC[0]=& _tmp3BD;({unsigned _tmp808=loc;Cyc_Warn_err2(_tmp808,_tag_fat(_tmp3BC,sizeof(void*),1));});});{
struct Cyc_List_List*fields=
({(struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators;})(Cyc_Core_heap_region,loc,args,0U,fs);
for(1;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3BF=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3D=_tmp3BF;struct _tuple20*_tmp3C0=_stmttmp3D;void*_tmp3C2;void*_tmp3C1;_tmp3C1=_tmp3C0->f1;_tmp3C2=_tmp3C0->f2;{struct Cyc_Absyn_Aggrfield*field=_tmp3C1;struct Cyc_Absyn_Exp*field_exp=_tmp3C2;
({struct Cyc_Tcenv_Tenv*_tmp80A=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp809=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp80A,_tmp809,field_exp);});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp80C=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp80B=field_exp;Cyc_Tcutil_coerce_arg(_tmp80C,_tmp80B,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C5;_tmp5C5.tag=0,({struct _fat_ptr _tmp80D=({const char*_tmp3CC="field ";_tag_fat(_tmp3CC,sizeof(char),7U);});_tmp5C5.f1=_tmp80D;});_tmp5C5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C4;_tmp5C4.tag=0,_tmp5C4.f1=*field->name;_tmp5C4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3;_tmp5C3.tag=0,({
struct _fat_ptr _tmp80E=({const char*_tmp3CB=" of struct expects type ";_tag_fat(_tmp3CB,sizeof(char),25U);});_tmp5C3.f1=_tmp80E;});_tmp5C3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3C7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C2;_tmp5C2.tag=2,_tmp5C2.f1=(void*)field->type;_tmp5C2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C1;_tmp5C1.tag=0,({struct _fat_ptr _tmp80F=({const char*_tmp3CA=" != ";_tag_fat(_tmp3CA,sizeof(char),5U);});_tmp5C1.f1=_tmp80F;});_tmp5C1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3C9=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5C0;_tmp5C0.tag=3,_tmp5C0.f1=(void*)field_exp->topt;_tmp5C0;});void*_tmp3C3[6];_tmp3C3[0]=& _tmp3C4,_tmp3C3[1]=& _tmp3C5,_tmp3C3[2]=& _tmp3C6,_tmp3C3[3]=& _tmp3C7,_tmp3C3[4]=& _tmp3C8,_tmp3C3[5]=& _tmp3C9;({unsigned _tmp810=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp810,_tag_fat(_tmp3C3,sizeof(void*),6));});});}}
# 1845
goto _LL0;}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C6;_tmp5C6.tag=0,({struct _fat_ptr _tmp811=({const char*_tmp3CF="tcAnonStruct: wrong type";_tag_fat(_tmp3CF,sizeof(char),25U);});_tmp5C6.f1=_tmp811;});_tmp5C6;});void*_tmp3CD[1];_tmp3CD[0]=& _tmp3CE;({int(*_tmp812)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp812(_tag_fat(_tmp3CD,sizeof(void*),1));});});}_LL0:;}
# 1848
return ts;}
# 1852
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1855
struct _tuple13 _tmp3D0=({struct _tuple13 _tmp5D1;({struct Cyc_List_List*_tmp813=Cyc_Tcenv_lookup_type_vars(te);_tmp5D1.f1=_tmp813;}),_tmp5D1.f2=Cyc_Core_heap_region;_tmp5D1;});struct _tuple13 env=_tmp3D0;
struct Cyc_List_List*_tmp3D1=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3D1;
struct Cyc_List_List*_tmp3D2=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),inst);struct Cyc_List_List*all_typs=_tmp3D2;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp814=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp814,all_typs);});
# 1860
if(topt != 0){
void*_tmp3D3=Cyc_Absyn_compress(*topt);void*_stmttmp3E=_tmp3D3;void*_tmp3D4=_stmttmp3E;if(*((int*)_tmp3D4)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D4)->f1)== 19){
Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 1866
struct Cyc_List_List*ts=tuf->typs;
for(1;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp816=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp815=e;Cyc_Tcutil_coerce_arg(_tmp816,_tmp815,t,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CC;_tmp5CC.tag=0,({struct _fat_ptr _tmp817=({const char*_tmp3DF="datatype constructor ";_tag_fat(_tmp3DF,sizeof(char),22U);});_tmp5CC.f1=_tmp817;});_tmp5CC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3D7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=1,_tmp5CB.f1=tuf->name;_tmp5CB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=0,({
struct _fat_ptr _tmp818=({const char*_tmp3DE=" expects argument of type ";_tag_fat(_tmp3DE,sizeof(char),27U);});_tmp5CA.f1=_tmp818;});_tmp5CA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3D9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=2,_tmp5C9.f1=(void*)t;_tmp5C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C8;_tmp5C8.tag=0,({struct _fat_ptr _tmp819=({const char*_tmp3DD=" but this argument has type ";_tag_fat(_tmp3DD,sizeof(char),29U);});_tmp5C8.f1=_tmp819;});_tmp5C8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C7;_tmp5C7.tag=0,
e->topt == 0?({struct _fat_ptr _tmp81B=(struct _fat_ptr)({const char*_tmp3DC="?";_tag_fat(_tmp3DC,sizeof(char),2U);});_tmp5C7.f1=_tmp81B;}):({struct _fat_ptr _tmp81A=Cyc_Absynpp_typ2string((void*)_check_null(e->topt));_tmp5C7.f1=_tmp81A;});_tmp5C7;});void*_tmp3D5[6];_tmp3D5[0]=& _tmp3D6,_tmp3D5[1]=& _tmp3D7,_tmp3D5[2]=& _tmp3D8,_tmp3D5[3]=& _tmp3D9,_tmp3D5[4]=& _tmp3DA,_tmp3D5[5]=& _tmp3DB;({unsigned _tmp81C=e->loc;Cyc_Warn_err2(_tmp81C,_tag_fat(_tmp3D5,sizeof(void*),6));});});
Cyc_Unify_explain_failure();}}
# 1879
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CE;_tmp5CE.tag=0,({
struct _fat_ptr _tmp81D=({const char*_tmp3E3="too many arguments for datatype constructor ";_tag_fat(_tmp3E3,sizeof(char),45U);});_tmp5CE.f1=_tmp81D;});_tmp5CE;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3E2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5CD;_tmp5CD.tag=1,_tmp5CD.f1=tuf->name;_tmp5CD;});void*_tmp3E0[2];_tmp3E0[0]=& _tmp3E1,_tmp3E0[1]=& _tmp3E2;({struct Cyc_Tcenv_Tenv*_tmp820=te;unsigned _tmp81F=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp81E=topt;Cyc_Tcexp_expr_err(_tmp820,_tmp81F,_tmp81E,_tag_fat(_tmp3E0,sizeof(void*),2));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D0;_tmp5D0.tag=0,({
struct _fat_ptr _tmp821=({const char*_tmp3E7="too few arguments for datatype constructor ";_tag_fat(_tmp3E7,sizeof(char),44U);});_tmp5D0.f1=_tmp821;});_tmp5D0;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3E6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5CF;_tmp5CF.tag=1,_tmp5CF.f1=tuf->name;_tmp5CF;});void*_tmp3E4[2];_tmp3E4[0]=& _tmp3E5,_tmp3E4[1]=& _tmp3E6;({struct Cyc_Tcenv_Tenv*_tmp824=te;unsigned _tmp823=loc;void**_tmp822=topt;Cyc_Tcexp_expr_err(_tmp824,_tmp823,_tmp822,_tag_fat(_tmp3E4,sizeof(void*),2));});});
return res;}}
# 1889
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1891
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1894
if(topt == 0)
return 0;{
void*_tmp3E8=Cyc_Absyn_compress(*topt);void*_stmttmp3F=_tmp3E8;void*_tmp3E9=_stmttmp3F;void*_tmp3EA;if(*((int*)_tmp3E9)== 3){_tmp3EA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9)->f1).elt_type;{void*elt_typ=_tmp3EA;
# 1898
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{
return 0;};}}
# 1904
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1906
enum Cyc_Absyn_AliasQual _tmp3EB=(Cyc_Tcutil_type_kind(Cyc_Absyn_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp40=_tmp3EB;enum Cyc_Absyn_AliasQual _tmp3EC=_stmttmp40;if(_tmp3EC == Cyc_Absyn_Unique)
return Cyc_Absyn_unique_rgn_type;else{
return Cyc_Absyn_heap_rgn_type;};}
# 1912
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 1916
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 1920
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3ED=Cyc_Absyn_compress(handle_type);void*_stmttmp41=_tmp3ED;void*_tmp3EE=_stmttmp41;void*_tmp3EF;if(*((int*)_tmp3EE)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EE)->f1)== 3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EE)->f2 != 0){_tmp3EF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EE)->f2)->hd;{void*r=_tmp3EF;
# 1925
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1929
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=0,({struct _fat_ptr _tmp825=({const char*_tmp3F3="expecting region_t type but found ";_tag_fat(_tmp3F3,sizeof(char),35U);});_tmp5D3.f1=_tmp825;});_tmp5D3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D2;_tmp5D2.tag=2,_tmp5D2.f1=(void*)handle_type;_tmp5D2;});void*_tmp3F0[2];_tmp3F0[0]=& _tmp3F1,_tmp3F0[1]=& _tmp3F2;({unsigned _tmp826=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;Cyc_Warn_err2(_tmp826,_tag_fat(_tmp3F0,sizeof(void*),2));});});
goto _LL0;}_LL0:;}else{
# 1935
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp827=Cyc_Absyn_uniquergn_exp();*ropt=_tmp827;});}}}
# 1942
({struct Cyc_Tcenv_Tenv*_tmp828=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp828,& Cyc_Absyn_uint_type,*e);});{
# 1951 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=0,({struct _fat_ptr _tmp829=({const char*_tmp3F6="calloc with empty type";_tag_fat(_tmp3F6,sizeof(char),23U);});_tmp5D4.f1=_tmp829;});_tmp5D4;});void*_tmp3F4[1];_tmp3F4[0]=& _tmp3F5;({int(*_tmp82A)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp82A(_tag_fat(_tmp3F4,sizeof(void*),1));});});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp82D=loc;struct Cyc_Tcenv_Tenv*_tmp82C=te;struct Cyc_List_List*_tmp82B=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp82D,_tmp82C,_tmp82B,& Cyc_Kinds_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D7;_tmp5D7.tag=0,({struct _fat_ptr _tmp82E=({const char*_tmp3FC="calloc cannot be used with type ";_tag_fat(_tmp3FC,sizeof(char),33U);});_tmp5D7.f1=_tmp82E;});_tmp5D7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D6;_tmp5D6.tag=2,_tmp5D6.f1=(void*)elt_type;_tmp5D6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D5;_tmp5D5.tag=0,({
struct _fat_ptr _tmp82F=({const char*_tmp3FB="\n\t(type needs initialization)";_tag_fat(_tmp3FB,sizeof(char),30U);});_tmp5D5.f1=_tmp82F;});_tmp5D5;});void*_tmp3F7[3];_tmp3F7[0]=& _tmp3F8,_tmp3F7[1]=& _tmp3F9,_tmp3F7[2]=& _tmp3FA;({unsigned _tmp830=loc;Cyc_Warn_err2(_tmp830,_tag_fat(_tmp3F7,sizeof(void*),3));});});
num_elts=*e;
one_elt=0;}else{
# 1966
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3FD=er;void*_tmp3FF;void*_tmp3FE;switch(*((int*)_tmp3FD)){case 17: _tmp3FE=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;{void*t2=_tmp3FE;
# 1970
elt_type=t2;
({void**_tmp831=({void**_tmp400=_cycalloc(sizeof(*_tmp400));*_tmp400=elt_type;_tmp400;});*t=_tmp831;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2)->tl)->tl == 0){_tmp3FE=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2)->hd;_tmp3FF=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2)->tl)->hd;{struct Cyc_Absyn_Exp*e1=_tmp3FE;struct Cyc_Absyn_Exp*e2=_tmp3FF;
# 1977
{struct _tuple0 _tmp401=({struct _tuple0 _tmp5DB;_tmp5DB.f1=e1->r,_tmp5DB.f2=e2->r;_tmp5DB;});struct _tuple0 _stmttmp42=_tmp401;struct _tuple0 _tmp402=_stmttmp42;void*_tmp403;if(*((int*)_tmp402.f1)== 17){_tmp403=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp402.f1)->f1;{void*t1=_tmp403;
# 1979
e1=e2;
_tmp403=t1;goto _LL10;}}else{if(*((int*)_tmp402.f2)== 17){_tmp403=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp402.f2)->f1;_LL10: {void*t2=_tmp403;
# 1983
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 1986
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp405=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DA;_tmp5DA.tag=0,({struct _fat_ptr _tmp832=({const char*_tmp409="malloc cannot be used with type ";_tag_fat(_tmp409,sizeof(char),33U);});_tmp5DA.f1=_tmp832;});_tmp5DA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp406=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D9;_tmp5D9.tag=2,_tmp5D9.f1=(void*)t2;_tmp5D9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp407=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D8;_tmp5D8.tag=0,({
struct _fat_ptr _tmp833=({const char*_tmp408="\n\t(type needs initialization)";_tag_fat(_tmp408,sizeof(char),30U);});_tmp5D8.f1=_tmp833;});_tmp5D8;});void*_tmp404[3];_tmp404[0]=& _tmp405,_tmp404[1]=& _tmp406,_tmp404[2]=& _tmp407;({unsigned _tmp834=loc;Cyc_Warn_err2(_tmp834,_tag_fat(_tmp404,sizeof(void*),3));});});else{
# 1990
*is_calloc=1;}}
# 1992
elt_type=t2;
({void**_tmp835=({void**_tmp40A=_cycalloc(sizeof(*_tmp40A));*_tmp40A=elt_type;_tmp40A;});*t=_tmp835;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{
goto No_sizeof;}}_LLC:;}
# 1999
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA:
# 2001
 No_sizeof: {
# 2004
struct Cyc_Absyn_Exp*_tmp40B=*e;struct Cyc_Absyn_Exp*real_e=_tmp40B;
{void*_tmp40C=real_e->r;void*_stmttmp43=_tmp40C;void*_tmp40D=_stmttmp43;void*_tmp40E;if(*((int*)_tmp40D)== 14){_tmp40E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40D)->f2;{struct Cyc_Absyn_Exp*e=_tmp40E;
real_e=e;goto _LL13;}}else{
goto _LL13;}_LL13:;}
# 2009
{void*_tmp40F=Cyc_Absyn_compress((void*)_check_null(real_e->topt));void*_stmttmp44=_tmp40F;void*_tmp410=_stmttmp44;void*_tmp411;if(*((int*)_tmp410)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f2 != 0){_tmp411=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f2)->hd;{void*tagt=_tmp411;
# 2011
{void*_tmp412=Cyc_Absyn_compress(tagt);void*_stmttmp45=_tmp412;void*_tmp413=_stmttmp45;void*_tmp414;if(*((int*)_tmp413)== 9){_tmp414=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp413)->f1;{struct Cyc_Absyn_Exp*vexp=_tmp414;
er=vexp->r;goto retry_sizeof;}}else{
goto _LL1D;}_LL1D:;}
# 2015
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B:
 goto _LL18;}_LL18:;}
# 2018
elt_type=Cyc_Absyn_char_type;
({void**_tmp836=({void**_tmp415=_cycalloc(sizeof(*_tmp415));*_tmp415=elt_type;_tmp415;});*t=_tmp836;});
num_elts=*e;
one_elt=0;}
# 2023
goto _LL5;}_LL5:;}}
# 2027
*is_fat=!one_elt;
# 2030
{void*_tmp416=elt_type;void*_tmp417;if(*((int*)_tmp416)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp416)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp416)->f1)->f1).KnownAggr).tag == 2){_tmp417=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp416)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp417;
# 2032
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp419=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=0,({struct _fat_ptr _tmp837=({const char*_tmp41A="malloc with existential types not yet implemented";_tag_fat(_tmp41A,sizeof(char),50U);});_tmp5DC.f1=_tmp837;});_tmp5DC;});void*_tmp418[1];_tmp418[0]=& _tmp419;({unsigned _tmp838=loc;Cyc_Warn_err2(_tmp838,_tag_fat(_tmp418,sizeof(void*),1));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25:
 goto _LL22;}_LL22:;}{
# 2039
void*(*_tmp41B)(void*,void*,struct Cyc_Absyn_Tqual,void*,void*)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*,void*,struct Cyc_Absyn_Tqual,void*,void*)=_tmp41B;
void*_tmp41C=Cyc_Absyn_false_type;void*zero_term=_tmp41C;
if(topt != 0){
void*_tmp41D=Cyc_Absyn_compress(*topt);void*_stmttmp46=_tmp41D;void*_tmp41E=_stmttmp46;void*_tmp422;void*_tmp421;void*_tmp420;void*_tmp41F;if(*((int*)_tmp41E)== 3){_tmp41F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).ptr_atts).nullable;_tmp420=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).ptr_atts).bounds;_tmp421=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).ptr_atts).zero_term;_tmp422=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).ptr_atts).autoreleased;{void*n=_tmp41F;void*b=_tmp420;void*zt=_tmp421;void*rel=_tmp422;
# 2044
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,rel))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp424=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DD;_tmp5DD.tag=0,({struct _fat_ptr _tmp839=({const char*_tmp425="cannot malloc() an autoreleased pointer";_tag_fat(_tmp425,sizeof(char),40U);});_tmp5DD.f1=_tmp839;});_tmp5DD;});void*_tmp423[1];_tmp423[0]=& _tmp424;({unsigned _tmp83A=loc;Cyc_Warn_err2(_tmp83A,_tag_fat(_tmp423,sizeof(void*),1));});});
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2051
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp427=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DE;_tmp5DE.tag=0,({struct _fat_ptr _tmp83B=({const char*_tmp428="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp428,sizeof(char),55U);});_tmp5DE.f1=_tmp83B;});_tmp5DE;});void*_tmp426[1];_tmp426[0]=& _tmp427;({unsigned _tmp83C=loc;Cyc_Warn_warn2(_tmp83C,_tag_fat(_tmp426,sizeof(void*),1));});});
*is_calloc=1;}{
# 2057
struct Cyc_Absyn_Exp*_tmp429=({void*_tmp83D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp83D,b);});struct Cyc_Absyn_Exp*eopt=_tmp429;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp42A=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp42A;
int _tmp42B=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp42B;
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp842=elt_type;void*_tmp841=rgn;struct Cyc_Absyn_Tqual _tmp840=Cyc_Absyn_empty_tqual(0U);void*_tmp83F=b;void*_tmp83E=zero_term;Cyc_Absyn_atb_type(_tmp842,_tmp841,_tmp840,_tmp83F,_tmp83E,Cyc_Absyn_false_type);});}{
# 2065
void*_tmp42C=Cyc_Absyn_compress((void*)_check_null(num_elts->topt));void*_stmttmp47=_tmp42C;void*_tmp42D=_stmttmp47;void*_tmp42E;if(*((int*)_tmp42D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42D)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42D)->f2 != 0){_tmp42E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42D)->f2)->hd;{void*tagtyp=_tmp42E;
# 2067
struct Cyc_Absyn_Exp*_tmp42F=({void*_tmp843=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp843,Cyc_Absyn_valueof_exp(tagtyp,0U),0,1U,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp42F;
# 2069
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp848=elt_type;void*_tmp847=rgn;struct Cyc_Absyn_Tqual _tmp846=Cyc_Absyn_empty_tqual(0U);void*_tmp845=b;void*_tmp844=zero_term;Cyc_Absyn_atb_type(_tmp848,_tmp847,_tmp846,_tmp845,_tmp844,Cyc_Absyn_false_type);});}
# 2073
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F:
 goto _LL2C;}_LL2C:;}}
# 2077
goto _LL27;}}}else{
goto _LL27;}_LL27:;}
# 2080
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp84D)(void*,void*,struct Cyc_Absyn_Tqual,void*,void*)=ptr_maker;void*_tmp84C=elt_type;void*_tmp84B=rgn;struct Cyc_Absyn_Tqual _tmp84A=Cyc_Absyn_empty_tqual(0U);void*_tmp849=zero_term;_tmp84D(_tmp84C,_tmp84B,_tmp84A,_tmp849,Cyc_Absyn_false_type);});}}}
# 2085
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2087
struct Cyc_Tcenv_Tenv*_tmp430=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp430;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp431=(void*)_check_null(e1->topt);void*t1=_tmp431;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2094
{void*_tmp432=Cyc_Absyn_compress(t1);void*_stmttmp48=_tmp432;void*_tmp433=_stmttmp48;if(*((int*)_tmp433)== 4){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp435=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DF;_tmp5DF.tag=0,({struct _fat_ptr _tmp84E=({const char*_tmp436="cannot assign to an array";_tag_fat(_tmp436,sizeof(char),26U);});_tmp5DF.f1=_tmp84E;});_tmp5DF;});void*_tmp434[1];_tmp434[0]=& _tmp435;({unsigned _tmp84F=loc;Cyc_Warn_err2(_tmp84F,_tag_fat(_tmp434,sizeof(void*),1));});});goto _LL0;}else{
goto _LL0;}_LL0:;}
# 2099
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp438=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E0;_tmp5E0.tag=0,({struct _fat_ptr _tmp850=({const char*_tmp439="swap not allowed for non-pointer or non-word-sized types";_tag_fat(_tmp439,sizeof(char),57U);});_tmp5E0.f1=_tmp850;});_tmp5E0;});void*_tmp437[1];_tmp437[0]=& _tmp438;({unsigned _tmp851=loc;Cyc_Warn_err2(_tmp851,_tag_fat(_tmp437,sizeof(void*),1));});});
# 2103
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp43B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E1;_tmp5E1.tag=0,({struct _fat_ptr _tmp852=({const char*_tmp43C="swap non-lvalue";_tag_fat(_tmp43C,sizeof(char),16U);});_tmp5E1.f1=_tmp852;});_tmp5E1;});void*_tmp43A[1];_tmp43A[0]=& _tmp43B;({struct Cyc_Tcenv_Tenv*_tmp855=te;unsigned _tmp854=e1->loc;void**_tmp853=topt;Cyc_Tcexp_expr_err(_tmp855,_tmp854,_tmp853,_tag_fat(_tmp43A,sizeof(void*),1));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp43E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E2;_tmp5E2.tag=0,({struct _fat_ptr _tmp856=({const char*_tmp43F="swap non-lvalue";_tag_fat(_tmp43F,sizeof(char),16U);});_tmp5E2.f1=_tmp856;});_tmp5E2;});void*_tmp43D[1];_tmp43D[0]=& _tmp43E;({struct Cyc_Tcenv_Tenv*_tmp859=te;unsigned _tmp858=e2->loc;void**_tmp857=topt;Cyc_Tcexp_expr_err(_tmp859,_tmp858,_tmp857,_tag_fat(_tmp43D,sizeof(void*),1));});});
# 2108
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp441=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=0,({struct _fat_ptr _tmp85A=({const char*_tmp446="type mismatch: ";_tag_fat(_tmp446,sizeof(char),16U);});_tmp5E6.f1=_tmp85A;});_tmp5E6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp442=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=2,_tmp5E5.f1=(void*)t1;_tmp5E5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp443=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=0,({struct _fat_ptr _tmp85B=({const char*_tmp445=" != ";_tag_fat(_tmp445,sizeof(char),5U);});_tmp5E4.f1=_tmp85B;});_tmp5E4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp444=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E3;_tmp5E3.tag=2,_tmp5E3.f1=(void*)t2;_tmp5E3;});void*_tmp440[4];_tmp440[0]=& _tmp441,_tmp440[1]=& _tmp442,_tmp440[2]=& _tmp443,_tmp440[3]=& _tmp444;({struct Cyc_Tcenv_Tenv*_tmp85E=te;unsigned _tmp85D=loc;void**_tmp85C=topt;Cyc_Tcexp_expr_err(_tmp85E,_tmp85D,_tmp85C,_tag_fat(_tmp440,sizeof(void*),4));});});
return Cyc_Absyn_void_type;}}}
# 2116
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp85F=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp85F,s,1);});
# 2119
while(1){
void*_tmp447=s->r;void*_stmttmp49=_tmp447;void*_tmp448=_stmttmp49;void*_tmp449;switch(*((int*)_tmp448)){case 1: _tmp449=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp448)->f1;{struct Cyc_Absyn_Exp*e=_tmp449;
# 2123
void*_tmp44A=(void*)_check_null(e->topt);void*t=_tmp44A;
if(!({void*_tmp860=t;Cyc_Unify_unify(_tmp860,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=0,({struct _fat_ptr _tmp861=({const char*_tmp44E="statement expression returns type ";_tag_fat(_tmp44E,sizeof(char),35U);});_tmp5E8.f1=_tmp861;});_tmp5E8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp44D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=2,_tmp5E7.f1=(void*)t;_tmp5E7;});void*_tmp44B[2];_tmp44B[0]=& _tmp44C,_tmp44B[1]=& _tmp44D;({unsigned _tmp862=loc;Cyc_Tcexp_err_and_explain(_tmp862,_tag_fat(_tmp44B,sizeof(void*),2));});});
return t;}case 2: _tmp449=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp448)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp449;
s=s2;continue;}case 12: _tmp449=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp448)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp449;
s=s1;continue;}default:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp450=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=0,({
struct _fat_ptr _tmp863=({const char*_tmp451="statement expression must end with expression";_tag_fat(_tmp451,sizeof(char),46U);});_tmp5E9.f1=_tmp863;});_tmp5E9;});void*_tmp44F[1];_tmp44F[0]=& _tmp450;({struct Cyc_Tcenv_Tenv*_tmp866=te;unsigned _tmp865=loc;void**_tmp864=topt;Cyc_Tcexp_expr_err(_tmp866,_tmp865,_tmp864,_tag_fat(_tmp44F,sizeof(void*),1));});});};}}
# 2134
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Absyn_compress(({struct Cyc_Tcenv_Tenv*_tmp867=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp867,0,e);}));
{void*_tmp452=t;void*_tmp453;if(*((int*)_tmp452)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp452)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp452)->f1)->f1).KnownAggr).tag == 2){_tmp453=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp452)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp453;
# 2138
if(((int)ad->kind == 1 && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp455=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=0,({struct _fat_ptr _tmp868=({const char*_tmp457="expecting @tagged union but found ";_tag_fat(_tmp457,sizeof(char),35U);});_tmp5EB.f1=_tmp868;});_tmp5EB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp456=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=2,_tmp5EA.f1=(void*)t;_tmp5EA;});void*_tmp454[2];_tmp454[0]=& _tmp455,_tmp454[1]=& _tmp456;({unsigned _tmp869=loc;Cyc_Warn_err2(_tmp869,_tag_fat(_tmp454,sizeof(void*),2));});});goto _LL0;}_LL0:;}
# 2143
return Cyc_Absyn_uint_type;}
# 2146
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
({struct Cyc_Tcenv_Tenv*_tmp86B=te;struct Cyc_Absyn_Exp*_tmp86A=e;Cyc_Tcexp_tcTest(_tmp86B,_tmp86A,({const char*_tmp458="@assert";_tag_fat(_tmp458,sizeof(char),8U);}));});
if(!Cyc_Absyn_no_side_effects_exp(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp45A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=0,({struct _fat_ptr _tmp86C=({const char*_tmp45B="@assert expression may have side effects";_tag_fat(_tmp45B,sizeof(char),41U);});_tmp5EC.f1=_tmp86C;});_tmp5EC;});void*_tmp459[1];_tmp459[0]=& _tmp45A;({unsigned _tmp86D=loc;Cyc_Warn_err2(_tmp86D,_tag_fat(_tmp459,sizeof(void*),1));});});
return Cyc_Absyn_sint_type;}
# 2154
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2158
void*rgn=Cyc_Absyn_heap_rgn_type;
te=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(1U,te));
if(*rgn_handle != 0){
# 2163
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp45C=Cyc_Absyn_compress(handle_type);void*_stmttmp4A=_tmp45C;void*_tmp45D=_stmttmp4A;void*_tmp45E;if(*((int*)_tmp45D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45D)->f1)== 3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45D)->f2 != 0){_tmp45E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45D)->f2)->hd;{void*r=_tmp45E;
# 2168
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 2172
({struct Cyc_Warn_String_Warn_Warg_struct _tmp460=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=0,({struct _fat_ptr _tmp86E=({const char*_tmp462="expecting region_t type but found ";_tag_fat(_tmp462,sizeof(char),35U);});_tmp5EE.f1=_tmp86E;});_tmp5EE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp461=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5ED;_tmp5ED.tag=2,_tmp5ED.f1=(void*)handle_type;_tmp5ED;});void*_tmp45F[2];_tmp45F[0]=& _tmp460,_tmp45F[1]=& _tmp461;({unsigned _tmp86F=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;Cyc_Warn_err2(_tmp86F,_tag_fat(_tmp45F,sizeof(void*),2));});});
# 2174
goto _LL0;}_LL0:;}else{
# 2176
if(topt != 0){
# 2179
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)
({struct Cyc_Absyn_Exp*_tmp870=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp870;});}}}{
# 2187
void*_tmp463=e1->r;void*_stmttmp4B=_tmp463;void*_tmp464=_stmttmp4B;void*_tmp466;void*_tmp465;switch(*((int*)_tmp464)){case 27:
 goto _LL9;case 28: _LL9: {
# 2192
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp468=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=0,({struct _fat_ptr _tmp871=({const char*_tmp469="tcNew: comprehension returned non-array type";_tag_fat(_tmp469,sizeof(char),45U);});_tmp5EF.f1=_tmp871;});_tmp5EF;});void*_tmp467[1];_tmp467[0]=& _tmp468;({int(*_tmp872)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp872(_tag_fat(_tmp467,sizeof(void*),1));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp875=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp874=loc;void*_tmp873=res_typ;Cyc_Tcutil_silent_castable(_tmp875,_tmp874,_tmp873,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
res_typ=*topt;}}
# 2203
return res_typ;}case 36: _tmp465=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp464)->f1;_tmp466=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp464)->f2;{struct Cyc_Core_Opt*nopt=_tmp465;struct Cyc_List_List*des=_tmp466;
# 2205
({void*_tmp876=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=26,_tmp46A->f1=des;_tmp46A;});e1->r=_tmp876;});
_tmp465=des;goto _LLD;}case 26: _tmp465=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp464)->f1;_LLD: {struct Cyc_List_List*des=_tmp465;
# 2208
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp46B=Cyc_Absyn_compress(*topt);void*_stmttmp4C=_tmp46B;void*_tmp46C=_stmttmp4C;void*_tmp46E;void*_tmp46D;if(*((int*)_tmp46C)== 3){_tmp46D=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46C)->f1).elt_type;_tmp46E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46C)->f1).ptr_atts).zero_term;{void**elt_typ=(void**)_tmp46D;void*zt=_tmp46E;
# 2214
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{
goto _LL14;}_LL14:;}{
# 2219
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp470=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0,({struct _fat_ptr _tmp877=({const char*_tmp471="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp471,sizeof(char),50U);});_tmp5F0.f1=_tmp877;});_tmp5F0;});void*_tmp46F[1];_tmp46F[0]=& _tmp470;({int(*_tmp878)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp878(_tag_fat(_tmp46F,sizeof(void*),1));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2228
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp87B=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp87A=loc;void*_tmp879=res_typ;Cyc_Tcutil_silent_castable(_tmp87B,_tmp87A,_tmp879,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
res_typ=*topt;}}
# 2234
return res_typ;}}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp464)->f1).Wstring_c).tag){case 8:  {
# 2239
void*_tmp472=({void*_tmp880=Cyc_Absyn_char_type;void*_tmp87F=rgn;struct Cyc_Absyn_Tqual _tmp87E=Cyc_Absyn_const_tqual(0U);void*_tmp87D=Cyc_Absyn_fat_bound_type;void*_tmp87C=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_tmp880,_tmp87F,_tmp87E,_tmp87D,_tmp87C,Cyc_Absyn_false_type);});void*topt2=_tmp472;
# 2241
void*_tmp473=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp473;
return({void*_tmp885=t;void*_tmp884=rgn;struct Cyc_Absyn_Tqual _tmp883=Cyc_Absyn_empty_tqual(0U);void*_tmp882=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2242
void*_tmp881=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_tmp885,_tmp884,_tmp883,_tmp882,_tmp881,Cyc_Absyn_false_type);});}case 9:  {
# 2246
void*_tmp474=({void*_tmp88A=Cyc_Absyn_wchar_type();void*_tmp889=rgn;struct Cyc_Absyn_Tqual _tmp888=Cyc_Absyn_const_tqual(0U);void*_tmp887=Cyc_Absyn_fat_bound_type;void*_tmp886=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_tmp88A,_tmp889,_tmp888,_tmp887,_tmp886,Cyc_Absyn_false_type);});void*topt2=_tmp474;
# 2248
void*_tmp475=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp475;
return({void*_tmp88F=t;void*_tmp88E=rgn;struct Cyc_Absyn_Tqual _tmp88D=Cyc_Absyn_empty_tqual(0U);void*_tmp88C=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2249
void*_tmp88B=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_tmp88F,_tmp88E,_tmp88D,_tmp88C,_tmp88B,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: {
# 2256
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp476=Cyc_Absyn_compress(*topt);void*_stmttmp4D=_tmp476;void*_tmp477=_stmttmp4D;struct Cyc_Absyn_Tqual _tmp479;void*_tmp478;switch(*((int*)_tmp477)){case 3: _tmp478=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp477)->f1).elt_type;_tmp479=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp477)->f1).elt_tq;{void**elttype=(void**)_tmp478;struct Cyc_Absyn_Tqual tq=_tmp479;
# 2261
topt2=elttype;goto _LL19;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp477)->f1)== 18){
# 2265
bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E:
 goto _LL19;}_LL19:;}{
# 2271
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2273
Cyc_Tcexp_check_consume(e1->loc,telt,e1);{
struct Cyc_Absyn_PtrInfo _tmp47A=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp47A;
pi.elt_type=telt;
(pi.ptr_atts).rgn=rgn;
({void*_tmp890=Cyc_Absyn_bounds_one();(pi.ptr_atts).bounds=_tmp890;});
(pi.ptr_atts).zero_term=Cyc_Absyn_false_type;
(pi.ptr_atts).autoreleased=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp893=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp892=loc;void*_tmp891=res_typ;Cyc_Tcutil_silent_castable(_tmp893,_tmp892,_tmp891,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
res_typ=*topt;}
# 2287
return res_typ;}}}}};}}
# 2293
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp895=t=({void*_tmp894=t;Cyc_Tcutil_promote_array(_tmp894,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp895;});
return t;}
# 2303
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2307
Cyc_Tcexp_check_consume(e->loc,t,e);{
void*_tmp47B=e->r;void*_stmttmp4E=_tmp47B;void*_tmp47C=_stmttmp4E;switch(*((int*)_tmp47C)){case 26:
 goto _LL4;case 27: _LL4:
 goto _LL6;case 28: _LL6:
 goto _LL8;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp47C)->f1).String_c).tag){case 9: _LL8:
 goto _LLA;case 8: _LLA:
 return t;default: goto _LLB;}default: _LLB:
# 2315
 if(Cyc_Tcutil_is_array_type(t))
({void*_tmp897=t=({void*_tmp896=t;Cyc_Tcutil_promote_array(_tmp896,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp897;});
return t;};}}
# 2328 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*t=({void*_tmp898=Cyc_Absyn_compress(Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0));e->topt=_tmp898;});
# 2332
{void*_tmp47D=e->r;void*_stmttmp4F=_tmp47D;void*_tmp47E=_stmttmp4F;void*_tmp47F;if(*((int*)_tmp47E)== 12){_tmp47F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp47E)->f1;{struct Cyc_Absyn_Exp*e2=_tmp47F;
return t;}}else{
goto _LL0;}_LL0:;}{
# 2337
void*_tmp480=t;void*_tmp481;if(*((int*)_tmp480)== 3){_tmp481=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480)->f1).elt_type;{void*t2=_tmp481;
# 2339
void*_tmp482=Cyc_Absyn_compress(t2);void*_stmttmp50=_tmp482;void*_tmp483=_stmttmp50;struct Cyc_Absyn_FnInfo _tmp484;if(*((int*)_tmp483)== 5){_tmp484=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp483)->f1;{struct Cyc_Absyn_FnInfo info=_tmp484;
# 2341
struct _tuple13 _tmp485=({struct _tuple13 _tmp5F1;({struct Cyc_List_List*_tmp899=Cyc_Tcenv_lookup_type_vars(te);_tmp5F1.f1=_tmp899;}),_tmp5F1.f2=Cyc_Core_heap_region;_tmp5F1;});struct _tuple13 env=_tmp485;
struct Cyc_List_List*_tmp486=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);struct Cyc_List_List*inst=_tmp486;
struct Cyc_List_List*_tmp487=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),inst);struct Cyc_List_List*ts=_tmp487;
struct Cyc_Absyn_Exp*_tmp488=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp488;
({void*_tmp89A=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->tag=13,_tmp489->f1=inner,_tmp489->f2=ts;_tmp489;});e->r=_tmp89A;});
({void*_tmp89B=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_tmp89B;});
return(void*)_check_null(e->topt);}}else{
return t;};}}else{
# 2350
return t;};}}}
# 2359
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2361
struct Cyc_List_List*_tmp48A=0;struct Cyc_List_List*decls=_tmp48A;
# 2363
{void*_tmp48B=fn_exp->r;void*_stmttmp51=_tmp48B;void*_tmp48C=_stmttmp51;void*_tmp48D;if(*((int*)_tmp48C)== 10){_tmp48D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp48C)->f2;{struct Cyc_List_List*es=_tmp48D;
# 2365
{void*_tmp48E=e->r;void*_stmttmp52=_tmp48E;void*_tmp48F=_stmttmp52;void*_tmp490;if(*((int*)_tmp48F)== 10){_tmp490=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp48F)->f2;{struct Cyc_List_List*es2=_tmp490;
# 2367
struct Cyc_List_List*_tmp491=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp491;
int _tmp492=0;int arg_cnt=_tmp492;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp494=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F6;_tmp5F6.tag=0,({struct _fat_ptr _tmp89C=({const char*_tmp49B="bad count ";_tag_fat(_tmp49B,sizeof(char),11U);});_tmp5F6.f1=_tmp89C;});_tmp5F6;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp495=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=12,_tmp5F5.f1=arg_cnt;_tmp5F5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp496=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=0,({struct _fat_ptr _tmp89D=({const char*_tmp49A="/";_tag_fat(_tmp49A,sizeof(char),2U);});_tmp5F4.f1=_tmp89D;});_tmp5F4;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp497=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=12,_tmp5F3.f1=(int)arg_exps->hd;_tmp5F3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp498=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0,({
struct _fat_ptr _tmp89E=({const char*_tmp499=" for alias coercion!";_tag_fat(_tmp499,sizeof(char),21U);});_tmp5F2.f1=_tmp89E;});_tmp5F2;});void*_tmp493[5];_tmp493[0]=& _tmp494,_tmp493[1]=& _tmp495,_tmp493[2]=& _tmp496,_tmp493[3]=& _tmp497,_tmp493[4]=& _tmp498;({int(*_tmp89F)(struct _fat_ptr)=({
# 2372
(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp89F(_tag_fat(_tmp493,sizeof(void*),5));});});
# 2374
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2379
struct _tuple12 _tmp49C=({struct Cyc_Absyn_Exp*_tmp8A0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp8A0,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp53=_tmp49C;struct _tuple12 _tmp49D=_stmttmp53;void*_tmp49F;void*_tmp49E;_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;{struct Cyc_Absyn_Decl*d=_tmp49E;struct Cyc_Absyn_Exp*ve=_tmp49F;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->hd=d,_tmp4A0->tl=decls;_tmp4A0;});
arg_exps=arg_exps->tl;}}}
# 2384
goto _LL5;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F7;_tmp5F7.tag=0,({struct _fat_ptr _tmp8A1=({const char*_tmp4A3="not a function call!";_tag_fat(_tmp4A3,sizeof(char),21U);});_tmp5F7.f1=_tmp8A1;});_tmp5F7;});void*_tmp4A1[1];_tmp4A1[0]=& _tmp4A2;({int(*_tmp8A2)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp8A2(_tag_fat(_tmp4A1,sizeof(void*),1));});});}_LL5:;}
# 2387
goto _LL0;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F8;_tmp5F8.tag=0,({struct _fat_ptr _tmp8A3=({const char*_tmp4A6="not a function call!";_tag_fat(_tmp4A6,sizeof(char),21U);});_tmp5F8.f1=_tmp8A3;});_tmp5F8;});void*_tmp4A4[1];_tmp4A4[0]=& _tmp4A5;({int(*_tmp8A4)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp8A4(_tag_fat(_tmp4A4,sizeof(void*),1));});});}_LL0:;}
# 2391
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp4A7=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp4A7;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp8A7=({struct Cyc_Absyn_Decl*_tmp8A6=d;struct Cyc_Absyn_Stmt*_tmp8A5=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp8A6,_tmp8A5,e->loc);});Cyc_Absyn_stmt_exp(_tmp8A7,e->loc);});
decls=decls->tl;}
# 2397
e->topt=0;
e->r=fn_exp->r;}
# 2402
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2406
{void*_tmp4A8=e->r;void*_stmttmp54=_tmp4A8;void*_tmp4A9=_stmttmp54;void*_tmp4AE;void*_tmp4AD;enum Cyc_Absyn_Incrementor _tmp4AF;enum Cyc_Absyn_Primop _tmp4B0;void*_tmp4AC;void*_tmp4AB;void*_tmp4AA;switch(*((int*)_tmp4A9)){case 12: _tmp4AA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_Absyn_Exp*e2=_tmp4AA;
# 2411
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=(void*)_check_null(e2->topt);
return;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4A9)->f4 == 0)
# 2415
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F9;_tmp5F9.tag=0,({struct _fat_ptr _tmp8A8=({const char*_tmp4B3="unresolved function: tcExpNoInst";_tag_fat(_tmp4B3,sizeof(char),33U);});_tmp5F9.f1=_tmp8A8;});_tmp5F9;});void*_tmp4B1[1];_tmp4B1[0]=& _tmp4B2;({int(*_tmp8A9)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp8A9(_tag_fat(_tmp4B1,sizeof(void*),1));});});else{_tmp4AA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct Cyc_List_List*es=_tmp4AB;struct Cyc_Absyn_VarargCallInfo**vci=_tmp4AC;
# 2438
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp4B4;_push_handler(& _tmp4B4);{int _tmp4B6=0;if(setjmp(_tmp4B4.handler))_tmp4B6=1;if(!_tmp4B6){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp4B5=(void*)Cyc_Core_get_exn_thrown();void*_tmp4B7=_tmp4B5;void*_tmp4B8;if(((struct Cyc_Core_Failure_exn_struct*)_tmp4B7)->tag == Cyc_Core_Failure){
# 2443
ok=0;
fn_exp=e;
goto _LL59;}else{_tmp4B8=_tmp4B7;{void*exn=_tmp4B8;(int)_rethrow(exn);}}_LL59:;}}}
# 2447
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2454
goto _LL0;}}case 36: _tmp4AA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Core_Opt*nopt=_tmp4AA;struct Cyc_List_List*des=_tmp4AB;
# 2419
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0: _tmp4AA=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{union Cyc_Absyn_Cnst*c=_tmp4AA;
# 2423
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1: _tmp4AA=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{void**b=_tmp4AA;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3: _tmp4B0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{enum Cyc_Absyn_Primop p=_tmp4B0;struct Cyc_List_List*es=_tmp4AA;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5: _tmp4AA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AF=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp4AA;enum Cyc_Absyn_Incrementor i=_tmp4AF;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4: _tmp4AA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct Cyc_Core_Opt*popt=_tmp4AB;struct Cyc_Absyn_Exp*e2=_tmp4AC;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6: _tmp4AA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct Cyc_Absyn_Exp*e2=_tmp4AB;struct Cyc_Absyn_Exp*e3=_tmp4AC;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7: _tmp4AA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct Cyc_Absyn_Exp*e2=_tmp4AB;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8: _tmp4AA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct Cyc_Absyn_Exp*e2=_tmp4AB;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9: _tmp4AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct Cyc_Absyn_Exp*e2=_tmp4AB;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11: _tmp4AA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp4AA;
# 2455
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13: _tmp4AA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp*e2=_tmp4AA;struct Cyc_List_List*ts=_tmp4AB;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14: _tmp4AA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A9)->f4;{void*t1=_tmp4AA;struct Cyc_Absyn_Exp*e1=_tmp4AB;enum Cyc_Absyn_Coercion*c=_tmp4AC;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15: _tmp4AA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp4AA;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16: _tmp4AA=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp**rgn_handle=_tmp4AA;struct Cyc_Absyn_Exp*e1=_tmp4AB;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18: _tmp4AA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp4AA;
_tmp4AA=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17: _tmp4AA=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_LL2A: {void*t1=_tmp4AA;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19: _tmp4AA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{void*t1=_tmp4AA;struct Cyc_List_List*l=_tmp4AB;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20: _tmp4AA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp4AA;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21: _tmp4AA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;_tmp4AD=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4A9)->f4;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct _fat_ptr*f=_tmp4AB;int*is_tagged=_tmp4AC;int*is_read=_tmp4AD;
# 2465
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22: _tmp4AA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;_tmp4AD=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4A9)->f4;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct _fat_ptr*f=_tmp4AB;int*is_tagged=_tmp4AC;int*is_read=_tmp4AD;
# 2467
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23: _tmp4AA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct Cyc_Absyn_Exp*e2=_tmp4AB;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24: _tmp4AA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_List_List*es=_tmp4AA;
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25: _tmp4AA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct _tuple9*t1=_tmp4AA;struct Cyc_List_List*des=_tmp4AB;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26: _tmp4AA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_List_List*des=_tmp4AA;
# 2474
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4B9=Cyc_Absyn_compress(*topt);void*_stmttmp55=_tmp4B9;void*_tmp4BA=_stmttmp55;void*_tmp4BD;void*_tmp4BC;void*_tmp4BB;if(*((int*)_tmp4BA)== 4){_tmp4BB=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BA)->f1).elt_type;_tmp4BC=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BA)->f1).tq;_tmp4BD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BA)->f1).zero_term;{void**et=(void**)_tmp4BB;struct Cyc_Absyn_Tqual*etq=(struct Cyc_Absyn_Tqual*)_tmp4BC;void*zt=_tmp4BD;
# 2480
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{
goto _LL5E;}_LL5E:;}
# 2486
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37: _tmp4AA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_Absyn_Stmt*s=_tmp4AA;
# 2488
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27: _tmp4AA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;_tmp4AD=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4A9)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmp4AA;struct Cyc_Absyn_Exp*e1=_tmp4AB;struct Cyc_Absyn_Exp*e2=_tmp4AC;int*iszeroterm=_tmp4AD;
# 2490
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28: _tmp4AA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp4AA;void*t1=_tmp4AB;int*iszeroterm=_tmp4AC;
# 2492
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29: _tmp4AA=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;_tmp4AD=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4A9)->f4;{struct _tuple1**tn=_tmp4AA;struct Cyc_List_List**ts=_tmp4AB;struct Cyc_List_List*args=_tmp4AC;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4AD;
# 2494
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30: _tmp4AA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{void*ts=_tmp4AA;struct Cyc_List_List*args=_tmp4AB;
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31: _tmp4AA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_tmp4AC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4A9)->f3;{struct Cyc_List_List*es=_tmp4AA;struct Cyc_Absyn_Datatypedecl*tud=_tmp4AB;struct Cyc_Absyn_Datatypefield*tuf=_tmp4AC;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32: _tmp4AA=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp4AA;struct Cyc_Absyn_Enumfield*ef=_tmp4AB;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33: _tmp4AA=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{void*t2=_tmp4AA;struct Cyc_Absyn_Enumfield*ef=_tmp4AB;
t=t2;goto _LL0;}case 34: _tmp4AA=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1).is_calloc;_tmp4AB=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1).rgn;_tmp4AC=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1).elt_type;_tmp4AD=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1).num_elts;_tmp4AE=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1).fat_result;{int*is_calloc=_tmp4AA;struct Cyc_Absyn_Exp**ropt=_tmp4AB;void***t2=(void***)_tmp4AC;struct Cyc_Absyn_Exp**e2=(struct Cyc_Absyn_Exp**)_tmp4AD;int*isfat=(int*)_tmp4AE;
# 2500
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35: _tmp4AA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp4AA;struct Cyc_Absyn_Exp*e2=_tmp4AB;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38: _tmp4AA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;_tmp4AB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;{struct Cyc_Absyn_Exp*e=_tmp4AA;struct _fat_ptr*f=_tmp4AB;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41: _tmp4AA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp4AA;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 42: _tmp4AA=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{struct Cyc_Absyn_Exp*e=_tmp4AA;
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 39: _tmp4AA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4A9)->f1;{void*t2=_tmp4AA;
# 2506
if(!te->allow_valueof)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=0,({struct _fat_ptr _tmp8AA=({const char*_tmp4C0="valueof(-) can only occur within types";_tag_fat(_tmp4C0,sizeof(char),39U);});_tmp5FA.f1=_tmp8AA;});_tmp5FA;});void*_tmp4BE[1];_tmp4BE[0]=& _tmp4BF;({unsigned _tmp8AB=e->loc;Cyc_Warn_err2(_tmp8AB,_tag_fat(_tmp4BE,sizeof(void*),1));});});
# 2514
t=Cyc_Absyn_sint_type;
goto _LL0;}default:
# 2518
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 2520
e->topt=t;}

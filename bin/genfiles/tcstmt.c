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
# 108 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 110
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
extern int Cyc_List_forall(int(*)(void*),struct Cyc_List_List*);
# 258
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);struct Cyc_Position_Error;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 303
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 663 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 851
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 854
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 857
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 864
void*Cyc_Absyn_compress(void*);
# 877
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 889
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 893
extern void*Cyc_Absyn_false_type;
# 895
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_rgn_handle_type(void*);
# 912
void*Cyc_Absyn_exn_type (void);
# 934
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 951
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1036
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1049
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1058
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1063
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1067
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple2*,void*,struct Cyc_Absyn_Exp*);
# 1104
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 36 "tcutil.h"
int Cyc_Tcutil_is_void_type(void*);
# 38
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 106
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 150
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 214
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 30
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
# 40
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 62
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 70
void Cyc_Atts_check_fndecl_atts(unsigned,struct Cyc_List_List*,int);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 71 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned,struct Cyc_Tcenv_Fenv*,struct Cyc_Absyn_Fndecl*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 97
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 101
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 103
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Switch_clause*);
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
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 142
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int,int);
# 153
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
# 35 "tctyp.h"
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr);
void Cyc_Tcexp_check_consume(unsigned,void*,struct Cyc_Absyn_Exp*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 53 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,struct Cyc_List_List*);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 105
void Cyc_Tcpat_check_switch_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 107
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**);
# 109
void Cyc_Tcpat_check_catch_overlap(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*);
# 42 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;void*_tmp2;if(*((int*)_tmp1)== 5)switch((int)((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2;
({void*_tmp149=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=5,_tmp3->f1=e1,_tmp3->f2=0U;_tmp3;});e->r=_tmp149;});goto _LL0;}case Cyc_Absyn_PostDec: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2;
({void*_tmp14A=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=5,_tmp4->f1=e1,_tmp4->f2=2U;_tmp4;});e->r=_tmp14A;});goto _LL0;}default: goto _LL5;}else{_LL5:
 goto _LL0;}_LL0:;}
# 50
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var (void){
static int counter=0;
return({struct _tuple2*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp14D=({struct _fat_ptr*_tmp8=_cycalloc(sizeof(*_tmp8));({struct _fat_ptr _tmp14C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp7=({struct Cyc_Int_pa_PrintArg_struct _tmp11A;_tmp11A.tag=1,_tmp11A.f1=(unsigned)counter ++;_tmp11A;});void*_tmp5[1];_tmp5[0]=& _tmp7;({struct _fat_ptr _tmp14B=({const char*_tmp6="_stmttmp%X";_tag_fat(_tmp6,sizeof(char),11U);});Cyc_aprintf(_tmp14B,_tag_fat(_tmp5,sizeof(void*),1));});});*_tmp8=_tmp14C;});_tmp8;});_tmp9->f2=_tmp14D;});_tmp9;});}
# 57
static int Cyc_Tcstmt_make_var_exp(struct Cyc_Absyn_Exp**eptr,struct Cyc_Absyn_Stmt*s0){
struct Cyc_Absyn_Exp*_tmpA=*eptr;struct Cyc_Absyn_Exp*e2=_tmpA;
int done=0;
while(!done){
void*_tmpB=e2->r;void*_stmttmp1=_tmpB;void*_tmpC=_stmttmp1;void*_tmpD;switch(*((int*)_tmpC)){case 1:
 return 0;case 12: _tmpD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpC)->f1;{struct Cyc_Absyn_Exp*e3=_tmpD;
_tmpD=e3;goto _LL6;}case 13: _tmpD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL6: {struct Cyc_Absyn_Exp*e3=_tmpD;
e2=e3;continue;}default:
 done=1;};}{
# 67
struct Cyc_Absyn_Exp*_tmpE=*eptr;struct Cyc_Absyn_Exp*e=_tmpE;
struct Cyc_Absyn_Vardecl*_tmpF=({struct _tuple2*_tmp14E=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp14E,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmpF;
struct Cyc_Absyn_Decl*_tmp10=({struct Cyc_Absyn_Pat*_tmp152=({void*_tmp150=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->tag=1,_tmp12->f1=vd,({struct Cyc_Absyn_Pat*_tmp14F=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp12->f2=_tmp14F;});_tmp12;});Cyc_Absyn_new_pat(_tmp150,e->loc);});struct Cyc_Absyn_Exp*_tmp151=e;Cyc_Absyn_let_decl(_tmp152,_tmp151,s0->loc);});struct Cyc_Absyn_Decl*d=_tmp10;
# 71
({void*_tmp155=({struct Cyc_Absyn_Decl*_tmp154=d;struct Cyc_Absyn_Stmt*_tmp153=Cyc_Absyn_new_stmt(s0->r,s0->loc);Cyc_Absyn_decl_stmt(_tmp154,_tmp153,s0->loc);})->r;s0->r=_tmp155;});
({struct Cyc_Absyn_Exp*_tmp157=({void*_tmp156=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=4,_tmp11->f1=vd;_tmp11;});Cyc_Absyn_varb_exp(_tmp156,e->loc);});*eptr=_tmp157;});
return 1;}}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 77
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 79
struct Cyc_Tcpat_TcPatResult _tmp13=pat_res;void*_tmp15;void*_tmp14;_tmp14=_tmp13.tvars_and_bounds_opt;_tmp15=_tmp13.patvars;{struct _tuple1*p=_tmp14;struct Cyc_List_List*vse=_tmp15;
struct Cyc_List_List*_tmp16=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*))Cyc_Core_fst,vse);struct Cyc_List_List*vs=_tmp16;
struct Cyc_List_List*_tmp17=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*p).f1);struct Cyc_List_List*tvs=_tmp17;
struct Cyc_List_List*_tmp18=p == 0?0:(*p).f2;struct Cyc_List_List*bds=_tmp18;
struct Cyc_List_List*_tmp19=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_filter)((int(*)(struct _tuple15*))Cyc_Core_snd,(*p).f1));struct Cyc_List_List*rgns=_tmp19;
te=Cyc_Tcenv_add_type_vars(loc,te,tvs);
for(1;rgns != 0;rgns=rgns->tl){
te=({struct Cyc_Tcenv_Tenv*_tmp158=te;Cyc_Tcenv_add_region(_tmp158,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->tag=2,_tmp1A->f1=(struct Cyc_Absyn_Tvar*)rgns->hd;_tmp1A;}),0,0);});}
te=Cyc_Tcenv_new_outlives_constraints(te,bds,loc);
if(new_block)
te=Cyc_Tcenv_new_block(loc,te);{
void*_tmp1B=Cyc_Tcenv_curr_rgn(te);void*ropt=_tmp1B;
for(1;vs != 0;vs=vs->tl){
if((struct Cyc_Absyn_Vardecl**)vs->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)vs->hd)))->rgn=ropt;}
# 95
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp15A=te;struct Cyc_Absyn_Exp*_tmp159=where_opt;Cyc_Tcexp_tcTest(_tmp15A,_tmp159,({const char*_tmp1C="switch clause guard";_tag_fat(_tmp1C,sizeof(char),20U);}));});
if(!Cyc_Absyn_no_side_effects_exp(where_opt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp11B;_tmp11B.tag=0,({struct _fat_ptr _tmp15B=({const char*_tmp1F="cannot show &&-clause in pattern has no effects";_tag_fat(_tmp1F,sizeof(char),48U);});_tmp11B.f1=_tmp15B;});_tmp11B;});void*_tmp1D[1];_tmp1D[0]=& _tmp1E;({unsigned _tmp15C=where_opt->loc;Cyc_Warn_err2(_tmp15C,_tag_fat(_tmp1D,sizeof(void*),1));});});}
# 100
Cyc_Tcstmt_tcStmt(te,s,0);}}}
# 103
static int Cyc_Tcstmt_is_case_default(struct Cyc_Absyn_Switch_clause*c){
void*_tmp20=(c->pattern)->r;void*_stmttmp2=_tmp20;void*_tmp21=_stmttmp2;if(*((int*)_tmp21)== 0)
return 1;else{
return 0;};}
# 118 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 120
void*_tmp22=s0->r;void*_stmttmp3=_tmp22;void*_tmp23=_stmttmp3;void*_tmp29;void*_tmp28;void*_tmp27;void*_tmp26;void*_tmp25;void*_tmp24;switch(*((int*)_tmp23)){case 0:
# 122
 return;case 1: _tmp24=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;{struct Cyc_Absyn_Exp*e=_tmp24;
# 125
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(e);
return;}case 2: _tmp24=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp23)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp24;struct Cyc_Absyn_Stmt*s2=_tmp25;
# 131
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3: _tmp24=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp24;
# 136
void*t=Cyc_Tcenv_return_typ(te);
if(eopt == 0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp11D;_tmp11D.tag=0,({struct _fat_ptr _tmp15D=({const char*_tmp2D="should return a value of type ";_tag_fat(_tmp2D,sizeof(char),31U);});_tmp11D.f1=_tmp15D;});_tmp11D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp11C;_tmp11C.tag=2,_tmp11C.f1=(void*)t;_tmp11C;});void*_tmp2A[2];_tmp2A[0]=& _tmp2B,_tmp2A[1]=& _tmp2C;({unsigned _tmp15E=s0->loc;Cyc_Warn_warn2(_tmp15E,_tag_fat(_tmp2A,sizeof(void*),2));});});else{
# 142
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp11F;_tmp11F.tag=0,({struct _fat_ptr _tmp15F=({const char*_tmp31="must return a value of type ";_tag_fat(_tmp31,sizeof(char),29U);});_tmp11F.f1=_tmp15F;});_tmp11F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp30=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp11E;_tmp11E.tag=2,_tmp11E.f1=(void*)t;_tmp11E;});void*_tmp2E[2];_tmp2E[0]=& _tmp2F,_tmp2E[1]=& _tmp30;({unsigned _tmp160=s0->loc;Cyc_Warn_err2(_tmp160,_tag_fat(_tmp2E,sizeof(void*),2));});});}}
# 144
return;}
# 147
Cyc_Tcexp_tcExp(te,& t,eopt);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp162=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp161=eopt;Cyc_Tcutil_coerce_arg(_tmp162,_tmp161,t,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp33=({struct Cyc_Warn_String_Warn_Warg_struct _tmp123;_tmp123.tag=0,({struct _fat_ptr _tmp163=({const char*_tmp38="returns value of type ";_tag_fat(_tmp38,sizeof(char),23U);});_tmp123.f1=_tmp163;});_tmp123;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp34=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp122;_tmp122.tag=3,_tmp122.f1=(void*)eopt->topt;_tmp122;});struct Cyc_Warn_String_Warn_Warg_struct _tmp35=({struct Cyc_Warn_String_Warn_Warg_struct _tmp121;_tmp121.tag=0,({struct _fat_ptr _tmp164=({const char*_tmp37=" but requires ";_tag_fat(_tmp37,sizeof(char),15U);});_tmp121.f1=_tmp164;});_tmp121;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp36=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp120;_tmp120.tag=2,_tmp120.f1=(void*)t;_tmp120;});void*_tmp32[4];_tmp32[0]=& _tmp33,_tmp32[1]=& _tmp34,_tmp32[2]=& _tmp35,_tmp32[3]=& _tmp36;({unsigned _tmp165=s0->loc;Cyc_Warn_err2(_tmp165,_tag_fat(_tmp32,sizeof(void*),4));});});
Cyc_Unify_explain_failure();}
# 152
Cyc_Tcexp_check_consume(eopt->loc,t,eopt);
return;}case 4: _tmp24=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp23)->f2;_tmp26=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp23)->f3;{struct Cyc_Absyn_Exp*e=_tmp24;struct Cyc_Absyn_Stmt*s1=_tmp25;struct Cyc_Absyn_Stmt*s2=_tmp26;
# 156
({struct Cyc_Tcenv_Tenv*_tmp167=te;struct Cyc_Absyn_Exp*_tmp166=e;Cyc_Tcexp_tcTest(_tmp167,_tmp166,({const char*_tmp39="if statement";_tag_fat(_tmp39,sizeof(char),13U);}));});
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5: _tmp24=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp23)->f1).f1;_tmp25=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp23)->f1).f2;_tmp26=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp23)->f2;{struct Cyc_Absyn_Exp*e=_tmp24;struct Cyc_Absyn_Stmt*cont_s=_tmp25;struct Cyc_Absyn_Stmt*s=_tmp26;
# 162
({struct Cyc_Tcenv_Tenv*_tmp169=te;struct Cyc_Absyn_Exp*_tmp168=e;Cyc_Tcexp_tcTest(_tmp169,_tmp168,({const char*_tmp3A="while loop";_tag_fat(_tmp3A,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9: _tmp24=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp23)->f2).f1;_tmp26=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp23)->f2).f2;_tmp27=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp23)->f3).f1;_tmp28=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp23)->f3).f2;_tmp29=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp23)->f4;{struct Cyc_Absyn_Exp*e1=_tmp24;struct Cyc_Absyn_Exp*e2=_tmp25;struct Cyc_Absyn_Stmt*guard_s=_tmp26;struct Cyc_Absyn_Exp*e3=_tmp27;struct Cyc_Absyn_Stmt*cont_s=_tmp28;struct Cyc_Absyn_Stmt*s=_tmp29;
# 167
Cyc_Tcexp_tcExp(te,0,e1);
({struct Cyc_Tcenv_Tenv*_tmp16B=te;struct Cyc_Absyn_Exp*_tmp16A=e2;Cyc_Tcexp_tcTest(_tmp16B,_tmp16A,({const char*_tmp3B="for loop";_tag_fat(_tmp3B,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14: _tmp24=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp23)->f2).f1;_tmp26=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp23)->f2).f2;{struct Cyc_Absyn_Stmt*s=_tmp24;struct Cyc_Absyn_Exp*e=_tmp25;struct Cyc_Absyn_Stmt*cont_s=_tmp26;
# 175
Cyc_Tcstmt_tcStmt(te,s,1);
({struct Cyc_Tcenv_Tenv*_tmp16D=te;struct Cyc_Absyn_Exp*_tmp16C=e;Cyc_Tcexp_tcTest(_tmp16D,_tmp16C,({const char*_tmp3C="do loop";_tag_fat(_tmp3C,sizeof(char),8U);}));});
return;}case 6:
# 179
 goto _LL14;case 7: _LL14:
 goto _LL16;case 8: _LL16:
 return;case 13: _tmp24=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp23)->f2;{struct _fat_ptr*l=_tmp24;struct Cyc_Absyn_Stmt*s=_tmp25;
# 186
struct Cyc_Absyn_Tvar*_tmp3D=({struct Cyc_Absyn_Tvar*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _fat_ptr*_tmp172=({struct _fat_ptr*_tmp41=_cycalloc(sizeof(*_tmp41));({struct _fat_ptr _tmp171=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp40=({struct Cyc_String_pa_PrintArg_struct _tmp124;_tmp124.tag=0,_tmp124.f1=(struct _fat_ptr)((struct _fat_ptr)*l);_tmp124;});void*_tmp3E[1];_tmp3E[0]=& _tmp40;({struct _fat_ptr _tmp170=({const char*_tmp3F="`%s";_tag_fat(_tmp3F,sizeof(char),4U);});Cyc_aprintf(_tmp170,_tag_fat(_tmp3E,sizeof(void*),1));});});*_tmp41=_tmp171;});_tmp41;});_tmp42->name=_tmp172;}),({
int _tmp16F=Cyc_Tcutil_new_tvar_id();_tmp42->identity=_tmp16F;}),({void*_tmp16E=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp42->kind=_tmp16E;});_tmp42;});
# 186
struct Cyc_Absyn_Tvar*tv=_tmp3D;
# 188
({struct Cyc_Tcenv_Tenv*_tmp173=Cyc_Tcenv_new_named_block(s0->loc,te,tv);Cyc_Tcstmt_tcStmt(_tmp173,s,0);});
return;}case 11: _tmp24=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp23)->f2;{struct Cyc_List_List*es=_tmp24;struct Cyc_Absyn_Switch_clause***clauseopt=_tmp25;
# 192
const struct _tuple14*_tmp43=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);const struct _tuple14*trip_opt=_tmp43;
if(trip_opt == (const struct _tuple14*)0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp45=({struct Cyc_Warn_String_Warn_Warg_struct _tmp125;_tmp125.tag=0,({struct _fat_ptr _tmp174=({const char*_tmp46="fallthru not in a non-last case";_tag_fat(_tmp46,sizeof(char),32U);});_tmp125.f1=_tmp174;});_tmp125;});void*_tmp44[1];_tmp44[0]=& _tmp45;({unsigned _tmp175=s0->loc;Cyc_Warn_err2(_tmp175,_tag_fat(_tmp44,sizeof(void*),1));});});
return;}{
# 197
struct _tuple14 _tmp47=*trip_opt;struct _tuple14 _stmttmp4=_tmp47;struct _tuple14 _tmp48=_stmttmp4;void*_tmp4A;void*_tmp49;_tmp49=_tmp48.f2;_tmp4A=_tmp48.f3;{struct Cyc_List_List*tvs=_tmp49;struct Cyc_List_List*ts_orig=_tmp4A;
struct Cyc_List_List*_tmp4B=({struct Cyc_List_List*_tmp176=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp176,tvs);});struct Cyc_List_List*instantiation=_tmp4B;
struct Cyc_List_List*_tmp4C=((struct Cyc_List_List*(*)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,ts_orig);struct Cyc_List_List*ts=_tmp4C;
for(1;ts != 0 && es != 0;(ts=ts->tl,es=es->tl)){
# 202
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp178=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp177=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp178,_tmp177,(void*)ts->hd,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp129;_tmp129.tag=0,({struct _fat_ptr _tmp179=({const char*_tmp53="fallthru argument has type ";_tag_fat(_tmp53,sizeof(char),28U);});_tmp129.f1=_tmp179;});_tmp129;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp4F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp128;_tmp128.tag=3,_tmp128.f1=(void*)((struct Cyc_Absyn_Exp*)es->hd)->topt;_tmp128;});struct Cyc_Warn_String_Warn_Warg_struct _tmp50=({struct Cyc_Warn_String_Warn_Warg_struct _tmp127;_tmp127.tag=0,({
struct _fat_ptr _tmp17A=({const char*_tmp52="but pattern variable has type ";_tag_fat(_tmp52,sizeof(char),31U);});_tmp127.f1=_tmp17A;});_tmp127;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp51=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp126;_tmp126.tag=2,_tmp126.f1=(void*)ts->hd;_tmp126;});void*_tmp4D[4];_tmp4D[0]=& _tmp4E,_tmp4D[1]=& _tmp4F,_tmp4D[2]=& _tmp50,_tmp4D[3]=& _tmp51;({unsigned _tmp17B=s0->loc;Cyc_Warn_err2(_tmp17B,_tag_fat(_tmp4D,sizeof(void*),4));});});
Cyc_Unify_explain_failure();}
# 209
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)ts->hd,(struct Cyc_Absyn_Exp*)es->hd);}
# 211
if(es != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp55=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12A;_tmp12A.tag=0,({struct _fat_ptr _tmp17C=({const char*_tmp56="too many arguments in explicit fallthru";_tag_fat(_tmp56,sizeof(char),40U);});_tmp12A.f1=_tmp17C;});_tmp12A;});void*_tmp54[1];_tmp54[0]=& _tmp55;({unsigned _tmp17D=s0->loc;Cyc_Warn_err2(_tmp17D,_tag_fat(_tmp54,sizeof(void*),1));});});
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp58=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12B;_tmp12B.tag=0,({struct _fat_ptr _tmp17E=({const char*_tmp59="too few arguments in explicit fallthru";_tag_fat(_tmp59,sizeof(char),39U);});_tmp12B.f1=_tmp17E;});_tmp12B;});void*_tmp57[1];_tmp57[0]=& _tmp58;({unsigned _tmp17F=s0->loc;Cyc_Warn_err2(_tmp17F,_tag_fat(_tmp57,sizeof(void*),1));});});
return;}}}case 10: _tmp24=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp23)->f2;_tmp26=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp23)->f3;{struct Cyc_Absyn_Exp**exp=_tmp24;struct Cyc_List_List*scs0=_tmp25;void**dtp=_tmp26;
# 219
if(Cyc_Tcstmt_make_var_exp(exp,s0)){
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 224
struct Cyc_Absyn_Exp*_tmp5A=*exp;struct Cyc_Absyn_Exp*e=_tmp5A;
void*_tmp5B=Cyc_Tcexp_tcExp(te,0,e);void*t=_tmp5B;
# 227
Cyc_Tcexp_check_consume(e->loc,t,e);{
# 231
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp5C=scs0;struct Cyc_List_List*scs=_tmp5C;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& t,e);}else{
# 239
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 241
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct Cyc_Tcpat_TcPatResult _tmp180=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& t,e);*_tmp5D=_tmp180;});_tmp5D;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 245
te=Cyc_Tcenv_clear_fallthru(te);else{
# 247
struct Cyc_List_List*_tmp5E=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp5E;
struct Cyc_List_List*_tmp5F=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 248
struct Cyc_List_List*new_tvs=_tmp5F;
# 250
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 253
te=Cyc_Tcenv_clear_fallthru(te);}{
# 255
struct Cyc_Absyn_Pat*_tmp60=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmp60;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12F;_tmp12F.tag=0,({
struct _fat_ptr _tmp181=({const char*_tmp67="switch on type ";_tag_fat(_tmp67,sizeof(char),16U);});_tmp12F.f1=_tmp181;});_tmp12F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp12E;_tmp12E.tag=2,_tmp12E.f1=(void*)t;_tmp12E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12D;_tmp12D.tag=0,({struct _fat_ptr _tmp182=({const char*_tmp66=" but case expects type ";_tag_fat(_tmp66,sizeof(char),24U);});_tmp12D.f1=_tmp182;});_tmp12D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp65=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp12C;_tmp12C.tag=3,_tmp12C.f1=(void*)p->topt;_tmp12C;});void*_tmp61[4];_tmp61[0]=& _tmp62,_tmp61[1]=& _tmp63,_tmp61[2]=& _tmp64,_tmp61[3]=& _tmp65;({unsigned _tmp183=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp183,_tag_fat(_tmp61,sizeof(void*),4));});});
Cyc_Unify_explain_failure();}else{
# 262
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 264
({struct Cyc_Core_Opt*_tmp184=({struct Cyc_Core_Opt*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->v=this_pattern.patvars;_tmp68;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp184;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 268
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,scs0,dtp);
return;}}}case 15: _tmp24=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp23)->f2;_tmp26=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp23)->f3;{struct Cyc_Absyn_Stmt*s=_tmp24;struct Cyc_List_List**scs0=_tmp25;void**dtp=(void**)_tmp26;
# 276
({void*_tmp188=({void*_tmp187=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=2,({struct Cyc_Absyn_Stmt*_tmp186=Cyc_Absyn_new_stmt(s->r,s->loc);_tmp69->f1=_tmp186;}),({struct Cyc_Absyn_Stmt*_tmp185=Cyc_Absyn_skip_stmt(s->loc);_tmp69->f2=_tmp185;});_tmp69;});Cyc_Absyn_new_stmt(_tmp187,s->loc);})->r;s->r=_tmp188;});
# 278
Cyc_Tcstmt_tcStmt(te,s,1);
# 280
if(!((int(*)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))Cyc_List_exists)(Cyc_Tcstmt_is_case_default,*scs0)){
# 282
struct _tuple2*def_v=({struct _tuple2*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp18A=({struct _fat_ptr*_tmp74=_cycalloc(sizeof(*_tmp74));({struct _fat_ptr _tmp189=({const char*_tmp73="exn";_tag_fat(_tmp73,sizeof(char),4U);});*_tmp74=_tmp189;});_tmp74;});_tmp75->f2=_tmp18A;});_tmp75;});
struct Cyc_Absyn_Vardecl*_tmp6A=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmp6A;
struct Cyc_Absyn_Pat*_tmp6B=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=1,_tmp72->f1=vd,({struct Cyc_Absyn_Pat*_tmp18B=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp72->f2=_tmp18B;});_tmp72;}),0U);struct Cyc_Absyn_Pat*def_pat=_tmp6B;
struct Cyc_Absyn_Stmt*_tmp6C=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=11,({struct Cyc_Absyn_Exp*_tmp18C=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=4,_tmp70->f1=vd;_tmp70;}),0U);_tmp71->f1=_tmp18C;}),_tmp71->f2=1;_tmp71;}),0U),0U);
# 285
struct Cyc_Absyn_Stmt*def_stmt=_tmp6C;
# 287
struct Cyc_Absyn_Switch_clause*_tmp6D=({struct Cyc_Absyn_Switch_clause*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->pattern=def_pat,_tmp6F->pat_vars=0,_tmp6F->where_clause=0,_tmp6F->body=def_stmt,_tmp6F->loc=0U;_tmp6F;});struct Cyc_Absyn_Switch_clause*def_clause=_tmp6D;
({struct Cyc_List_List*_tmp18E=({struct Cyc_List_List*_tmp18D=*scs0;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp18D,({struct Cyc_Absyn_Switch_clause*_tmp6E[1];_tmp6E[0]=def_clause;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp6E,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));});*scs0=_tmp18E;});}{
# 294
void*_tmp76=Cyc_Absyn_exn_type();void*exception_type=_tmp76;
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp77=*scs0;struct Cyc_List_List*scs=_tmp77;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& exception_type,0);}else{
# 303
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 305
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp78=_cycalloc(sizeof(*_tmp78));({struct Cyc_Tcpat_TcPatResult _tmp18F=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& exception_type,0);*_tmp78=_tmp18F;});_tmp78;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 309
te=Cyc_Tcenv_clear_fallthru(te);else{
# 311
struct Cyc_List_List*_tmp79=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp79;
struct Cyc_List_List*_tmp7A=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 312
struct Cyc_List_List*new_tvs=_tmp7A;
# 314
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 317
te=Cyc_Tcenv_clear_fallthru(te);}{
# 319
struct Cyc_Absyn_Pat*_tmp7B=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmp7B;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),exception_type)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp133;_tmp133.tag=0,({struct _fat_ptr _tmp190=({const char*_tmp82="switch on type ";_tag_fat(_tmp82,sizeof(char),16U);});_tmp133.f1=_tmp190;});_tmp133;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp132;_tmp132.tag=2,_tmp132.f1=(void*)exception_type;_tmp132;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp131;_tmp131.tag=0,({
struct _fat_ptr _tmp191=({const char*_tmp81=" but case expects type ";_tag_fat(_tmp81,sizeof(char),24U);});_tmp131.f1=_tmp191;});_tmp131;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp80=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp130;_tmp130.tag=3,_tmp130.f1=(void*)p->topt;_tmp130;});void*_tmp7C[4];_tmp7C[0]=& _tmp7D,_tmp7C[1]=& _tmp7E,_tmp7C[2]=& _tmp7F,_tmp7C[3]=& _tmp80;({unsigned _tmp192=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp192,_tag_fat(_tmp7C,sizeof(void*),4));});});
Cyc_Unify_explain_failure();}else{
# 326
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 328
({struct Cyc_Core_Opt*_tmp193=({struct Cyc_Core_Opt*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->v=this_pattern.patvars;_tmp83;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp193;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 332
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*scs0,dtp);
return;}}default: _tmp24=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp23)->f1;_tmp25=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp23)->f2;{struct Cyc_Absyn_Decl*d=_tmp24;struct Cyc_Absyn_Stmt*s=_tmp25;
# 336
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmp84=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;struct Cyc_Tcenv_Tenv*te=_tmp84;
{void*_tmp85=d->r;void*_stmttmp5=_tmp85;void*_tmp86=_stmttmp5;void*_tmp8A;void*_tmp89;void*_tmp88;void*_tmp87;switch(*((int*)_tmp86)){case 0: _tmp87=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp86)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp87;
# 340
struct Cyc_Absyn_Vardecl*_tmp8B=vd;void*_tmp90;void*_tmp8F;void*_tmp8E;struct Cyc_Absyn_Tqual _tmp8D;enum Cyc_Absyn_Scope _tmp8C;_tmp8C=_tmp8B->sc;_tmp8D=_tmp8B->tq;_tmp8E=_tmp8B->type;_tmp8F=_tmp8B->initializer;_tmp90=(void**)& _tmp8B->rgn;{enum Cyc_Absyn_Scope sc=_tmp8C;struct Cyc_Absyn_Tqual tq=_tmp8D;void*t=_tmp8E;struct Cyc_Absyn_Exp*initializer=_tmp8F;void**rgn_ptr=(void**)_tmp90;
void*_tmp91=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp91;
int is_local;
{enum Cyc_Absyn_Scope _tmp92=sc;switch((int)_tmp92){case Cyc_Absyn_Static:
 goto _LL4E;case Cyc_Absyn_Extern: _LL4E:
 goto _LL50;case Cyc_Absyn_ExternC: _LL50:
 vd->escapes=1;is_local=0;goto _LL4A;case Cyc_Absyn_Abstract:
# 348
({struct Cyc_Warn_String_Warn_Warg_struct _tmp94=({struct Cyc_Warn_String_Warn_Warg_struct _tmp134;_tmp134.tag=0,({struct _fat_ptr _tmp194=({const char*_tmp95="bad abstract scope for local variable";_tag_fat(_tmp95,sizeof(char),38U);});_tmp134.f1=_tmp194;});_tmp134;});void*_tmp93[1];_tmp93[0]=& _tmp94;({unsigned _tmp195=d->loc;Cyc_Warn_err2(_tmp195,_tag_fat(_tmp93,sizeof(void*),1));});});
goto _LL54;case Cyc_Absyn_Register: _LL54:
 goto _LL56;case Cyc_Absyn_Public: _LL56:
 goto _LL58;default: _LL58:
 is_local=1;goto _LL4A;}_LL4A:;}
# 355
*rgn_ptr=is_local?curr_bl: Cyc_Absyn_heap_rgn_type;
{void*_tmp96=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp96;void*_tmp97=_stmttmp6;struct Cyc_Absyn_ArrayInfo _tmp98;switch(*((int*)_tmp97)){case 5: if(is_local){
# 359
vd->escapes=1;
sc=3U;
is_local=0;
goto _LL59;}else{goto _LL5E;}case 4: _tmp98=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97)->f1;{struct Cyc_Absyn_ArrayInfo ai=_tmp98;
# 365
if(vd->initializer == 0 || ai.num_elts != 0)
goto _LL59;{
struct Cyc_Absyn_Exp*len;
{void*_tmp99=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmp7=_tmp99;void*_tmp9A=_stmttmp7;void*_tmp9B;struct _fat_ptr _tmp9C;switch(*((int*)_tmp9A)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp9A)->f1).Wstring_c).tag){case 8: _tmp9C=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp9A)->f1).String_c).val;{struct _fat_ptr s=_tmp9C;
len=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);goto _LL60;}case 9: _tmp9C=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp9A)->f1).Wstring_c).val;{struct _fat_ptr s=_tmp9C;
len=Cyc_Absyn_uint_exp(1U,0U);goto _LL60;}default: goto _LL6D;}case 27: _tmp9B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9A)->f2;{struct Cyc_Absyn_Exp*e=_tmp9B;
_tmp9B=e;goto _LL68;}case 28: _tmp9B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9A)->f1;_LL68: {struct Cyc_Absyn_Exp*e=_tmp9B;
len=e;goto _LL60;}case 36: _tmp9B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp9A)->f2;{struct Cyc_List_List*es=_tmp9B;
_tmp9B=es;goto _LL6C;}case 26: _tmp9B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9A)->f1;_LL6C: {struct Cyc_List_List*es=_tmp9B;
len=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*))Cyc_List_length)(es),0U);goto _LL60;}default: _LL6D:
 len=0;goto _LL60;}_LL60:;}
# 377
ai.num_elts=len;
t=({void*_tmp196=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=4,_tmp9D->f1=ai;_tmp9D;});vd->type=_tmp196;});
goto _LL59;}}default: _LL5E:
 goto _LL59;}_LL59:;}{
# 385
struct Cyc_List_List*_tmp9E=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*bound_vars=_tmp9E;
int _tmp9F=is_local;int allow_evars=_tmp9F;
Cyc_Tctyp_check_type(s0->loc,te,bound_vars,& Cyc_Kinds_tmk,allow_evars,1,t);
({int _tmp197=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp197;});
# 390
if((int)sc == 3 ||(int)sc == 4)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp135;_tmp135.tag=0,({struct _fat_ptr _tmp198=({const char*_tmpA2="extern declarations are not yet supported within functions";_tag_fat(_tmpA2,sizeof(char),59U);});_tmp135.f1=_tmp198;});_tmp135;});void*_tmpA0[1];_tmpA0[0]=& _tmpA1;({unsigned _tmp199=d->loc;Cyc_Warn_err2(_tmp199,_tag_fat(_tmpA0,sizeof(void*),1));});});
if(initializer != 0){
Cyc_Tcexp_tcExpInitializer(te,& t,initializer);
# 395
if(!is_local && !Cyc_Tcutil_is_const_exp(initializer))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp136;_tmp136.tag=0,({struct _fat_ptr _tmp19A=({const char*_tmpA5="initializer for static variable needs to be a constant expression";_tag_fat(_tmpA5,sizeof(char),66U);});_tmp136.f1=_tmp19A;});_tmp136;});void*_tmpA3[1];_tmpA3[0]=& _tmpA4;({unsigned _tmp19B=d->loc;Cyc_Warn_err2(_tmp19B,_tag_fat(_tmpA3,sizeof(void*),1));});});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp19D=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp19C=initializer;Cyc_Tcutil_coerce_assign(_tmp19D,_tmp19C,t);})){
({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmpA7=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp13B;_tmp13B.tag=11,_tmp13B.f1=vd;_tmp13B;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13A;_tmp13A.tag=0,({struct _fat_ptr _tmp19E=({const char*_tmpAD=" was declared with type ";_tag_fat(_tmpAD,sizeof(char),25U);});_tmp13A.f1=_tmp19E;});_tmp13A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp139;_tmp139.tag=2,_tmp139.f1=(void*)t;_tmp139;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp138;_tmp138.tag=0,({
struct _fat_ptr _tmp19F=({const char*_tmpAC=" but initialized with type ";_tag_fat(_tmpAC,sizeof(char),28U);});_tmp138.f1=_tmp19F;});_tmp138;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmpAB=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp137;_tmp137.tag=3,_tmp137.f1=(void*)initializer->topt;_tmp137;});void*_tmpA6[5];_tmpA6[0]=& _tmpA7,_tmpA6[1]=& _tmpA8,_tmpA6[2]=& _tmpA9,_tmpA6[3]=& _tmpAA,_tmpA6[4]=& _tmpAB;({unsigned _tmp1A0=d->loc;Cyc_Warn_err2(_tmp1A0,_tag_fat(_tmpA6,sizeof(void*),5));});});
Cyc_Unify_unify(t,(void*)_check_null(initializer->topt));
Cyc_Unify_explain_failure();}}
# 404
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}case 2: _tmp87=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp86)->f1;_tmp88=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp86)->f2;_tmp89=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp86)->f3;_tmp8A=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp86)->f4;{struct Cyc_Absyn_Pat*p=_tmp87;struct Cyc_Core_Opt**vds=_tmp88;struct Cyc_Absyn_Exp**exp=_tmp89;void**dtp=(void**)_tmp8A;
# 409
{void*_tmpAE=p->r;void*_stmttmp8=_tmpAE;void*_tmpAF=_stmttmp8;switch(*((int*)_tmpAF)){case 1:
 goto _LL73;case 2: _LL73:
 goto _LL75;case 15: _LL75:
 goto _LL6F;default:
# 414
 if(Cyc_Tcstmt_make_var_exp(exp,s0)){
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}}_LL6F:;}{
# 420
struct Cyc_Absyn_Exp*_tmpB0=*exp;struct Cyc_Absyn_Exp*e=_tmpB0;
Cyc_Tcexp_tcExpInitializer(te,0,e);{
# 423
void*pat_type=(void*)_check_null(e->topt);
# 425
struct Cyc_Tcpat_TcPatResult _tmpB1=Cyc_Tcpat_tcPat(te,p,& pat_type,e);struct Cyc_Tcpat_TcPatResult pat_res=_tmpB1;
({struct Cyc_Core_Opt*_tmp1A1=({struct Cyc_Core_Opt*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=pat_res.patvars;_tmpB2;});*vds=_tmp1A1;});{
void*_tmpB3=(void*)_check_null(p->topt);void*pt=_tmpB3;
void*_tmpB4=(void*)_check_null(e->topt);void*et=_tmpB4;
if(!Cyc_Unify_unify(pt,et)&& !({struct Cyc_RgnOrder_RgnPO*_tmp1A3=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1A2=e;Cyc_Tcutil_coerce_assign(_tmp1A3,_tmp1A2,pt);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13F;_tmp13F.tag=0,({
struct _fat_ptr _tmp1A4=({const char*_tmpBB="pattern type ";_tag_fat(_tmpBB,sizeof(char),14U);});_tmp13F.f1=_tmp1A4;});_tmp13F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp13E;_tmp13E.tag=2,_tmp13E.f1=(void*)pt;_tmp13E;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13D;_tmp13D.tag=0,({struct _fat_ptr _tmp1A5=({const char*_tmpBA=" does not match definition type ";_tag_fat(_tmpBA,sizeof(char),33U);});_tmp13D.f1=_tmp1A5;});_tmp13D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp13C;_tmp13C.tag=2,_tmp13C.f1=(void*)et;_tmp13C;});void*_tmpB5[4];_tmpB5[0]=& _tmpB6,_tmpB5[1]=& _tmpB7,_tmpB5[2]=& _tmpB8,_tmpB5[3]=& _tmpB9;({unsigned _tmp1A6=d->loc;Cyc_Warn_err2(_tmp1A6,_tag_fat(_tmpB5,sizeof(void*),4));});});
Cyc_Unify_unify(pt,et);
Cyc_Unify_explain_failure();}else{
# 436
Cyc_Tcpat_check_pat_regions(te,p,pat_res.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(p->loc,te,p,dtp);
Cyc_Tcstmt_pattern_synth(s0->loc,te,pat_res,s,0,0);
return;}}}}case 3: _tmp87=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp86)->f1;{struct Cyc_List_List*vds=_tmp87;
# 442
void*_tmpBC=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpBC;
for(1;vds != 0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=curr_bl;
({unsigned _tmp1A9=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1A8=te;struct Cyc_List_List*_tmp1A7=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1A9,_tmp1A8,_tmp1A7,& Cyc_Kinds_tmk,1,1,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);});}
# 448
Cyc_Tcstmt_tcStmt(te,s,0);
return;}case 4: _tmp87=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp86)->f1;_tmp88=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp86)->f2;_tmp89=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp86)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp87;struct Cyc_Absyn_Vardecl*vd=_tmp88;struct Cyc_Absyn_Exp*open_exp_opt=_tmp89;
# 454
({void*_tmp1AD=({void*_tmp1AC=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->tag=2,({struct Cyc_Absyn_Stmt*_tmp1AB=Cyc_Absyn_new_stmt(s->r,s->loc);_tmpBD->f1=_tmp1AB;}),({struct Cyc_Absyn_Stmt*_tmp1AA=Cyc_Absyn_skip_stmt(s->loc);_tmpBD->f2=_tmp1AA;});_tmpBD;});Cyc_Absyn_new_stmt(_tmp1AC,s->loc);})->r;s->r=_tmp1AD;});{
# 456
struct Cyc_Tcenv_Tenv*_tmpBE=te;struct Cyc_Tcenv_Tenv*te2=_tmpBE;
struct Cyc_Absyn_Vardecl*_tmpBF=vd;void*_tmpC1;void*_tmpC0;_tmpC0=(void**)& _tmpBF->type;_tmpC1=(void**)& _tmpBF->rgn;{void**t=(void**)_tmpC0;void**rgn_ptr=(void**)_tmpC1;
void*_tmpC2=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpC2;
*rgn_ptr=curr_bl;{
void*rgn_typ;
if((unsigned)open_exp_opt){
# 464
struct _tuple2*_tmpC3=({struct _tuple2*_tmpDA=_cycalloc(sizeof(*_tmpDA));({union Cyc_Absyn_Nmspace _tmp1B2=Cyc_Absyn_Abs_n(({struct _fat_ptr*_tmpD5[1];({struct _fat_ptr*_tmp1B1=({struct _fat_ptr*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct _fat_ptr _tmp1B0=({const char*_tmpD6="Core";_tag_fat(_tmpD6,sizeof(char),5U);});*_tmpD7=_tmp1B0;});_tmpD7;});_tmpD5[0]=_tmp1B1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpD5,sizeof(struct _fat_ptr*),1));}),0);_tmpDA->f1=_tmp1B2;}),({struct _fat_ptr*_tmp1AF=({struct _fat_ptr*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct _fat_ptr _tmp1AE=({const char*_tmpD8="DynamicRegion";_tag_fat(_tmpD8,sizeof(char),14U);});*_tmpD9=_tmp1AE;});_tmpD9;});_tmpDA->f2=_tmp1AF;});_tmpDA;});struct _tuple2*drname=_tmpC3;
void*_tmpC4=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->v=& Cyc_Kinds_trk;_tmpD4;}),0);void*outer_rgn=_tmpC4;
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->v=& Cyc_Kinds_rk;_tmpC5;}),0);{
void*_tmpC6=({union Cyc_Absyn_AggrInfo _tmp1B3=Cyc_Absyn_UnknownAggr(0U,drname,0);Cyc_Absyn_aggr_type(_tmp1B3,({void*_tmpD3[1];_tmpD3[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpD3,sizeof(void*),1));}));});void*dr_type=_tmpC6;
void*_tmpC7=({void*_tmp1B7=dr_type;void*_tmp1B6=outer_rgn;struct Cyc_Absyn_Tqual _tmp1B5=Cyc_Absyn_empty_tqual(0U);void*_tmp1B4=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp1B7,_tmp1B6,_tmp1B5,_tmp1B4,Cyc_Absyn_false_type);});void*exp_type=_tmpC7;
({unsigned _tmp1BA=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1B9=te;struct Cyc_List_List*_tmp1B8=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1BA,_tmp1B9,_tmp1B8,& Cyc_Kinds_tmk,1,0,exp_type);});{
# 471
void*_tmpC8=Cyc_Tcexp_tcExp(te,& exp_type,open_exp_opt);void*key_typ=_tmpC8;
if(!Cyc_Unify_unify(exp_type,key_typ)&& !({
struct Cyc_RgnOrder_RgnPO*_tmp1BC=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1BB=open_exp_opt;Cyc_Tcutil_coerce_assign(_tmp1BC,_tmp1BB,exp_type);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp143;_tmp143.tag=0,({struct _fat_ptr _tmp1BD=({const char*_tmpCF="expected ";_tag_fat(_tmpCF,sizeof(char),10U);});_tmp143.f1=_tmp1BD;});_tmp143;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp142;_tmp142.tag=2,_tmp142.f1=(void*)exp_type;_tmp142;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp141;_tmp141.tag=0,({struct _fat_ptr _tmp1BE=({const char*_tmpCE=" but found ";_tag_fat(_tmpCE,sizeof(char),12U);});_tmp141.f1=_tmp1BE;});_tmp141;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp140;_tmp140.tag=2,_tmp140.f1=(void*)key_typ;_tmp140;});void*_tmpC9[4];_tmpC9[0]=& _tmpCA,_tmpC9[1]=& _tmpCB,_tmpC9[2]=& _tmpCC,_tmpC9[3]=& _tmpCD;({unsigned _tmp1BF=s0->loc;Cyc_Warn_err2(_tmp1BF,_tag_fat(_tmpC9,sizeof(void*),4));});});
if(!Cyc_Unify_unify(outer_rgn,Cyc_Absyn_unique_rgn_type)&& !
Cyc_Unify_unify(outer_rgn,Cyc_Absyn_refcnt_rgn_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp144;_tmp144.tag=0,({struct _fat_ptr _tmp1C0=({const char*_tmpD2="open is only allowed on unique or reference-counted keys";_tag_fat(_tmpD2,sizeof(char),57U);});_tmp144.f1=_tmp1C0;});_tmp144;});void*_tmpD0[1];_tmpD0[0]=& _tmpD1;({unsigned _tmp1C1=s0->loc;Cyc_Warn_err2(_tmp1C1,_tag_fat(_tmpD0,sizeof(void*),1));});});
({void*_tmp1C2=Cyc_Absyn_rgn_handle_type(rgn_typ);*t=_tmp1C2;});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1,0);}}}else{
# 481
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->tag=2,_tmpDB->f1=tv;_tmpDB;});
te2=({unsigned _tmp1C4=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1C3=te2;Cyc_Tcenv_add_type_vars(_tmp1C4,_tmp1C3,({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=tv,_tmpDC->tl=0;_tmpDC;}));});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0,1);}
# 485
({unsigned _tmp1C7=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1C6=te;struct Cyc_List_List*_tmp1C5=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp1C7,_tmp1C6,_tmp1C5,& Cyc_Kinds_bk,1,0,*t);});
if(!({void*_tmp1C8=*t;Cyc_Unify_unify(_tmp1C8,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({void*_tmpDD=0U;({struct _fat_ptr _tmp1C9=({const char*_tmpDE="region stmt: type of region handle is wrong!";_tag_fat(_tmpDE,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp1C9,_tag_fat(_tmpDD,sizeof(void*),0));});});
# 489
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1: _tmp87=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp86)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp87;
# 493
if((int)fd->sc != 2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp145;_tmp145.tag=0,({struct _fat_ptr _tmp1CA=({const char*_tmpE1="bad storage class for inner function";_tag_fat(_tmpE1,sizeof(char),37U);});_tmp145.f1=_tmp1CA;});_tmp145;});void*_tmpDF[1];_tmpDF[0]=& _tmpE0;({unsigned _tmp1CB=d->loc;Cyc_Warn_err2(_tmp1CB,_tag_fat(_tmpDF,sizeof(void*),1));});});
Cyc_Atts_check_fndecl_atts(d->loc,(fd->i).attributes,1);
Cyc_Tctyp_check_fndecl_valid_type(d->loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
({struct Cyc_List_List*_tmp1CC=Cyc_Atts_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp1CC;});{
void*_tmpE2=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpE2;
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->sc=fd->sc,_tmpE3->name=fd->name,_tmpE3->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1D2=Cyc_Absyn_const_tqual(0U);_tmpE3->tq=_tmp1D2;}),({
void*_tmp1D1=({void*_tmp1D0=t;void*_tmp1CF=curr_bl;struct Cyc_Absyn_Tqual _tmp1CE=Cyc_Absyn_empty_tqual(0U);void*_tmp1CD=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp1D0,_tmp1CF,_tmp1CE,_tmp1CD,Cyc_Absyn_false_type);});_tmpE3->type=_tmp1D1;}),_tmpE3->initializer=0,_tmpE3->rgn=curr_bl,_tmpE3->attributes=0,_tmpE3->escapes=0,_tmpE3->is_proto=fd->body != 0;_tmpE3;});
# 505
fd->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(te->le);
({struct Cyc_Tcenv_Fenv*_tmp1D3=Cyc_Tcenv_nested_fenv(d->loc,old_fenv,fd);te->le=_tmp1D3;});
Cyc_Tcstmt_tcStmt(te,fd->body,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
te->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9: _tmp87=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp86)->f1;_tmp88=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp86)->f2;{struct _fat_ptr*n=_tmp87;struct Cyc_List_List*tds=_tmp88;
unimp_msg_part=({const char*_tmpE4="namespace";_tag_fat(_tmpE4,sizeof(char),10U);});goto _LL24;}case 10: _tmp87=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp86)->f1;_tmp88=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp86)->f2;{struct _tuple2*q=_tmp87;struct Cyc_List_List*tds=_tmp88;
unimp_msg_part=({const char*_tmpE5="using";_tag_fat(_tmpE5,sizeof(char),6U);});goto _LL24;}case 5:
 unimp_msg_part=({const char*_tmpE6="type";_tag_fat(_tmpE6,sizeof(char),5U);});goto _LL24;case 6:
 unimp_msg_part=({const char*_tmpE7="datatype";_tag_fat(_tmpE7,sizeof(char),9U);});goto _LL24;case 7:
 unimp_msg_part=({const char*_tmpE8="enum";_tag_fat(_tmpE8,sizeof(char),5U);});goto _LL24;case 8:
 unimp_msg_part=({const char*_tmpE9="typedef";_tag_fat(_tmpE9,sizeof(char),8U);});goto _LL24;case 11:
 unimp_msg_part=({const char*_tmpEA="extern \"C\"";_tag_fat(_tmpEA,sizeof(char),11U);});goto _LL24;case 12:
 unimp_msg_part=({const char*_tmpEB="extern \"C include\"";_tag_fat(_tmpEB,sizeof(char),19U);});goto _LL24;case 13:
 unimp_msg_part=({const char*_tmpEC="__cyclone_port_on__";_tag_fat(_tmpEC,sizeof(char),20U);});goto _LL24;case 14:
 unimp_msg_part=({const char*_tmpED="__cyclone_port_off__";_tag_fat(_tmpED,sizeof(char),21U);});goto _LL24;case 15:
 unimp_msg_part=({const char*_tmpEE="__tempest_on__";_tag_fat(_tmpEE,sizeof(char),15U);});goto _LL24;default:
 unimp_msg_part=({const char*_tmpEF="__tempest_off__";_tag_fat(_tmpEF,sizeof(char),16U);});goto _LL24;}_LL24:;}
# 527
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp148;_tmp148.tag=0,({struct _fat_ptr _tmp1D4=({const char*_tmpF5="tcStmt: nested ";_tag_fat(_tmpF5,sizeof(char),16U);});_tmp148.f1=_tmp1D4;});_tmp148;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp147;_tmp147.tag=0,_tmp147.f1=unimp_msg_part;_tmp147;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp146;_tmp146.tag=0,({struct _fat_ptr _tmp1D5=({const char*_tmpF4=" declarations not allowed";_tag_fat(_tmpF4,sizeof(char),26U);});_tmp146.f1=_tmp1D5;});_tmp146;});void*_tmpF0[3];_tmpF0[0]=& _tmpF1,_tmpF0[1]=& _tmpF2,_tmpF0[2]=& _tmpF3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpF0,sizeof(void*),3));});}};}
# 539 "tcstmt.cyc"
static int Cyc_Tcstmt_check_catchall_handler(struct Cyc_Absyn_Stmt*b){
void*_tmpF6=b->r;void*_stmttmp9=_tmpF6;void*_tmpF7=_stmttmp9;void*_tmpF8;if(*((int*)_tmpF7)== 15){_tmpF8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmpF7)->f2;{struct Cyc_List_List*case_list=_tmpF8;
# 542
int found_wild=0;
while((unsigned)case_list){
struct Cyc_Absyn_Switch_clause*_tmpF9=(struct Cyc_Absyn_Switch_clause*)case_list->hd;struct Cyc_Absyn_Switch_clause*clause=_tmpF9;
{void*_tmpFA=(clause->pattern)->r;void*_stmttmpA=_tmpFA;void*_tmpFB=_stmttmpA;if(*((int*)_tmpFB)== 0){
# 547
found_wild=1;
goto _LL5;}else{
# 550
goto _LL5;}_LL5:;}
# 552
if(!Cyc_Tcstmt_ensure_no_throw_stmt(clause->body))
return 0;
case_list=case_list->tl;}
# 556
return found_wild;}}else{
# 558
return 0;};}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 562
static int Cyc_Tcstmt_ensure_no_throw_exp(struct Cyc_Absyn_Exp*e){
void*_tmpFC=e->r;void*_stmttmpB=_tmpFC;void*_tmpFD=_stmttmpB;enum Cyc_Absyn_Coercion _tmp101;int _tmp100;void*_tmp102;void*_tmpFF;void*_tmpFE;switch(*((int*)_tmpFD)){case 0:
# 565
 return 1;case 3: _tmpFE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;{struct Cyc_List_List*es=_tmpFE;
# 568
return((int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall)(Cyc_Tcstmt_ensure_no_throw_exp,es);}case 7: _tmpFE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmpFF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;{struct Cyc_Absyn_Exp*e1=_tmpFE;struct Cyc_Absyn_Exp*e2=_tmpFF;
_tmpFE=e1;_tmpFF=e2;goto _LL8;}case 8: _tmpFE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmpFF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmpFE;struct Cyc_Absyn_Exp*e2=_tmpFF;
_tmpFE=e1;_tmpFF=e2;goto _LLA;}case 35: _tmpFE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmpFF=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmpFE;struct Cyc_Absyn_Exp*e2=_tmpFF;
_tmpFE=e1;_tmpFF=e2;goto _LLC;}case 9: _tmpFE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmpFF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmpFE;struct Cyc_Absyn_Exp*e2=_tmpFF;
# 573
return Cyc_Tcstmt_ensure_no_throw_exp(e1)&& Cyc_Tcstmt_ensure_no_throw_exp(e2);}case 10: _tmpFE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmpFF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_tmp102=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpFD)->f3;_tmp100=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpFD)->f4;{struct Cyc_Absyn_Exp*fname=_tmpFE;struct Cyc_List_List*args=_tmpFF;struct Cyc_Absyn_VarargCallInfo*vargs=_tmp102;int res=_tmp100;
# 575
if(!res)
return 0;{
int fname_ok=0;
{void*_tmp103=fname->r;void*_stmttmpC=_tmp103;void*_tmp104=_stmttmpC;void*_tmp106;void*_tmp105;switch(*((int*)_tmp104)){case 13: _tmp105=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_tmp106=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp104)->f2;{struct Cyc_Absyn_Exp*e1=_tmp105;struct Cyc_List_List*ts=_tmp106;
# 580
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(e1);
goto _LL1D;}case 1: _tmp105=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1;{void*b=_tmp105;
# 583
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(fname);
goto _LL1D;}default:
# 586
 goto _LL1D;}_LL1D:;}
# 588
return fname_ok &&((int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall)(Cyc_Tcstmt_ensure_no_throw_exp,args);}}case 1: _tmpFE=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;{void*b=_tmpFE;
# 590
void*_tmp107=b;void*_tmp108;switch(*((int*)_tmp107)){case 2: _tmp108=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp107)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp108;
# 592
void*_tmp109=fd->cached_type;void*_stmttmpD=_tmp109;void*_tmp10A=_stmttmpD;struct Cyc_Absyn_FnInfo _tmp10B;if(_tmp10A != 0){if(*((int*)_tmp10A)== 5){_tmp10B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp10A)->f1;{struct Cyc_Absyn_FnInfo fi=_tmp10B;
# 594
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,void*))Cyc_List_mem)(Cyc_Atts_attribute_cmp,fi.attributes,(void*)& Cyc_Atts_No_throw_att_val);}}else{
# 602
return 0;}}else{
# 598
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,void*))Cyc_List_mem)(Cyc_Atts_attribute_cmp,(fd->i).attributes,(void*)& Cyc_Atts_No_throw_att_val);};}case 1: _tmp108=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp107)->f1;{struct Cyc_Absyn_Vardecl*fvard=_tmp108;
# 606
void*_tmp10C=fvard->type;void*_stmttmpE=_tmp10C;void*_tmp10D=_stmttmpE;struct Cyc_Absyn_FnInfo _tmp10E;if(*((int*)_tmp10D)== 5){_tmp10E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp10D)->f1;{struct Cyc_Absyn_FnInfo fi=_tmp10E;
# 608
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,void*))Cyc_List_mem)(Cyc_Atts_attribute_cmp,fi.attributes,(void*)& Cyc_Atts_No_throw_att_val)||
# 611
((int(*)(int(*)(void*,void*),struct Cyc_List_List*,void*))Cyc_List_mem)(Cyc_Atts_attribute_cmp,fvard->attributes,(void*)& Cyc_Atts_No_throw_att_val);}}else{
# 615
return 0;};}case 3:
# 618
 goto _LL2C;case 4: _LL2C:
# 620
 return 1;default:
# 622
 return 0;};}case 15: _tmpFE=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;{struct Cyc_Absyn_Exp*e1=_tmpFE;
# 625
return Cyc_Tcstmt_ensure_no_throw_exp(e1);}case 29: _tmpFE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpFD)->f3;{struct Cyc_List_List*exps=_tmpFE;
# 627
return((int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall)(Cyc_Tcstmt_ensure_no_throw_exp,
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,exps));}case 14: _tmpFE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmpFF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_tmp100=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFD)->f3;_tmp101=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFD)->f4;{void*ct=_tmpFE;struct Cyc_Absyn_Exp*e1=_tmpFF;int b=_tmp100;enum Cyc_Absyn_Coercion c=_tmp101;
# 631
return !b && Cyc_Tcstmt_ensure_no_throw_exp(e1);}case 20: _tmpFE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;{struct Cyc_Absyn_Exp*e1=_tmpFE;
_tmpFE=e1;goto _LL1A;}case 22: _tmpFE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpFE;
# 634
void*_tmp10F=e1->topt;void*_stmttmpF=_tmp10F;void*_tmp110=_stmttmpF;void*_tmp111;if(_tmp110 != 0){if(*((int*)_tmp110)== 3){_tmp111=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp110)->f1;{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_tmp111;
# 636
return !((unsigned)(pi->ptr_atts).nullable);}}else{goto _LL3E;}}else{_LL3E:
# 638
 return 0;};}default:
# 642
 return 0;};}
# 655 "tcstmt.cyc"
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp112=s->r;void*_stmttmp10=_tmp112;void*_tmp113=_stmttmp10;void*_tmp116;void*_tmp115;void*_tmp114;switch(*((int*)_tmp113)){case 6:
 goto _LL4;case 7: _LL4:
# 659
 return 1;case 15:
# 661
 return Cyc_Tcstmt_check_catchall_handler(s);case 2: _tmp114=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp113)->f1;_tmp115=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp113)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp114;struct Cyc_Absyn_Stmt*s2=_tmp115;
# 663
return Cyc_Tcstmt_ensure_no_throw_stmt(s1)&& Cyc_Tcstmt_ensure_no_throw_stmt(s2);}case 1: _tmp114=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp113)->f1;{struct Cyc_Absyn_Exp*e=_tmp114;
# 665
return Cyc_Tcstmt_ensure_no_throw_exp(e);}case 3: _tmp114=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp113)->f1;{struct Cyc_Absyn_Exp*e=_tmp114;
# 667
if((unsigned)e)
return Cyc_Tcstmt_ensure_no_throw_exp(e);
# 670
return 1;}case 12: _tmp114=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp113)->f1;_tmp115=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp113)->f2;{struct Cyc_Absyn_Decl*d=_tmp114;struct Cyc_Absyn_Stmt*s2=_tmp115;
# 672
int init_okay=0;
{void*_tmp117=d->r;void*_stmttmp11=_tmp117;void*_tmp118=_stmttmp11;void*_tmp119;switch(*((int*)_tmp118)){case 0: _tmp119=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp118)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp119;
# 675
init_okay=(unsigned)vd->initializer?Cyc_Tcstmt_ensure_no_throw_exp((struct Cyc_Absyn_Exp*)_check_null(vd->initializer)): 1;
goto _LL15;}case 2: _tmp119=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp118)->f3;{struct Cyc_Absyn_Exp*i=_tmp119;
# 678
init_okay=Cyc_Tcstmt_ensure_no_throw_exp(i);
goto _LL15;}default:
# 681
 return 0;}_LL15:;}
# 683
return init_okay && Cyc_Tcstmt_ensure_no_throw_stmt(s2);}case 13: _tmp114=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp113)->f1;_tmp115=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp113)->f2;{struct _fat_ptr*l=_tmp114;struct Cyc_Absyn_Stmt*s1=_tmp115;
# 685
return Cyc_Tcstmt_ensure_no_throw_stmt(s1);}case 4: _tmp114=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp113)->f1;_tmp115=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp113)->f2;_tmp116=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp113)->f3;{struct Cyc_Absyn_Exp*e1=_tmp114;struct Cyc_Absyn_Stmt*s1=_tmp115;struct Cyc_Absyn_Stmt*s2=_tmp116;
# 687
return(Cyc_Tcstmt_ensure_no_throw_exp(e1)&& Cyc_Tcstmt_ensure_no_throw_stmt(s1))&& Cyc_Tcstmt_ensure_no_throw_stmt(s2);}default:
# 689
 return 0;};}

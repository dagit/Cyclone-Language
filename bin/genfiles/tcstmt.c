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

/* Pools */
struct _pool; // defined in runtime_memory.c
struct _PoolHandle {
  struct _RuntimeStack s;
  struct _pool *p;
};
struct _PoolHandle _new_pool(void);
void _free_pool(struct _PoolHandle *h);
void _push_pool(struct _PoolHandle * r);
void _pop_pool(void);

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
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
extern int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 258
extern int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_Position_Error;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 312
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 415 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 514
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 532
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 700 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 896
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 903
void*Cyc_Absyn_compress(void*);
# 917
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 929
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 933
extern void*Cyc_Absyn_false_type;
# 935
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_rgn_handle_type(void*);
# 952
void*Cyc_Absyn_exn_type (void);
# 974
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term,void*rel);
# 991
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1088
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1093
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1098
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple2*,void*,struct Cyc_Absyn_Exp*init);
# 1138
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
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
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
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
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
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
# 35 "attributes.h"
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 43
void Cyc_Atts_check_fndecl_atts(unsigned,struct Cyc_List_List*,int is_defn);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
# 141
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 143
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened,int lifo);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pool(struct Cyc_Tcenv_Tenv*te,void*poolrgn);
# 155
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
void Cyc_Tcexp_check_consume(unsigned,void*,struct Cyc_Absyn_Exp*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 53 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 105
void Cyc_Tcpat_check_switch_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 107
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**);
# 109
void Cyc_Tcpat_check_catch_overlap(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 41 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;struct Cyc_Absyn_Exp*_tmp3;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3;
({void*_tmp159=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=5U,_tmp4->f1=e1,_tmp4->f2=Cyc_Absyn_PreInc;_tmp4;});e->r=_tmp159;});goto _LL0;}case Cyc_Absyn_PostDec: _LL3: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2;
({void*_tmp15A=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=5U,_tmp5->f1=e1,_tmp5->f2=Cyc_Absyn_PreDec;_tmp5;});e->r=_tmp15A;});goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 49
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var (void){
static int counter=0;
return({struct _tuple2*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp15D=({struct _fat_ptr*_tmp9=_cycalloc(sizeof(*_tmp9));({struct _fat_ptr _tmp15C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp8=({struct Cyc_Int_pa_PrintArg_struct _tmp12A;_tmp12A.tag=1U,_tmp12A.f1=(unsigned)counter ++;_tmp12A;});void*_tmp6[1U];_tmp6[0]=& _tmp8;({struct _fat_ptr _tmp15B=({const char*_tmp7="_stmttmp%X";_tag_fat(_tmp7,sizeof(char),11U);});Cyc_aprintf(_tmp15B,_tag_fat(_tmp6,sizeof(void*),1U));});});*_tmp9=_tmp15C;});_tmp9;});_tmpA->f2=_tmp15D;});_tmpA;});}
# 56
static int Cyc_Tcstmt_make_var_exp(struct Cyc_Absyn_Exp**eptr,struct Cyc_Absyn_Stmt*s0){
struct Cyc_Absyn_Exp*_tmpB=*eptr;struct Cyc_Absyn_Exp*e2=_tmpB;
int done=0;
while(!done){
void*_tmpC=e2->r;void*_stmttmp1=_tmpC;void*_tmpD=_stmttmp1;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpF;switch(*((int*)_tmpD)){case 1U: _LL1: _LL2:
 return 0;case 12U: _LL3: _tmpF=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD)->f1;_LL4: {struct Cyc_Absyn_Exp*e3=_tmpF;
_tmpE=e3;goto _LL6;}case 13U: _LL5: _tmpE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD)->f1;_LL6: {struct Cyc_Absyn_Exp*e3=_tmpE;
e2=e3;continue;}default: _LL7: _LL8:
 done=1;}_LL0:;}{
# 66
struct Cyc_Absyn_Exp*_tmp10=*eptr;struct Cyc_Absyn_Exp*e=_tmp10;
struct Cyc_Absyn_Vardecl*_tmp11=({struct _tuple2*_tmp15E=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp15E,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp11;
struct Cyc_Absyn_Decl*_tmp12=({struct Cyc_Absyn_Pat*_tmp162=({void*_tmp160=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=1U,_tmp14->f1=vd,({struct Cyc_Absyn_Pat*_tmp15F=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp14->f2=_tmp15F;});_tmp14;});Cyc_Absyn_new_pat(_tmp160,e->loc);});struct Cyc_Absyn_Exp*_tmp161=e;Cyc_Absyn_let_decl(_tmp162,_tmp161,s0->loc);});struct Cyc_Absyn_Decl*d=_tmp12;
# 70
({void*_tmp165=({struct Cyc_Absyn_Decl*_tmp164=d;struct Cyc_Absyn_Stmt*_tmp163=Cyc_Absyn_new_stmt(s0->r,s0->loc);Cyc_Absyn_decl_stmt(_tmp164,_tmp163,s0->loc);})->r;s0->r=_tmp165;});
({struct Cyc_Absyn_Exp*_tmp167=({void*_tmp166=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->tag=4U,_tmp13->f1=vd;_tmp13;});Cyc_Absyn_varb_exp(_tmp166,e->loc);});*eptr=_tmp167;});
return 1;}}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 76
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 78
struct Cyc_Tcpat_TcPatResult _tmp15=pat_res;struct Cyc_List_List*_tmp17;struct _tuple1*_tmp16;_LL1: _tmp16=_tmp15.tvars_and_bounds_opt;_tmp17=_tmp15.patvars;_LL2: {struct _tuple1*p=_tmp16;struct Cyc_List_List*vse=_tmp17;
struct Cyc_List_List*_tmp18=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*))Cyc_Core_fst,vse);struct Cyc_List_List*vs=_tmp18;
struct Cyc_List_List*_tmp19=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*p).f1);struct Cyc_List_List*tvs=_tmp19;
struct Cyc_List_List*_tmp1A=p == 0?0:(*p).f2;struct Cyc_List_List*bds=_tmp1A;
struct Cyc_List_List*_tmp1B=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple15*))Cyc_Core_snd,(*p).f1));struct Cyc_List_List*rgns=_tmp1B;
te=Cyc_Tcenv_add_type_vars(loc,te,tvs);
for(0;rgns != 0;rgns=rgns->tl){
te=({struct Cyc_Tcenv_Tenv*_tmp168=te;Cyc_Tcenv_add_region(_tmp168,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=2U,_tmp1C->f1=(struct Cyc_Absyn_Tvar*)rgns->hd;_tmp1C;}),0,0);});}
te=Cyc_Tcenv_new_outlives_constraints(te,bds,loc);
if(new_block)
te=Cyc_Tcenv_new_block(loc,te);{
void*_tmp1D=Cyc_Tcenv_curr_rgn(te);void*ropt=_tmp1D;
for(0;vs != 0;vs=vs->tl){
if((struct Cyc_Absyn_Vardecl**)vs->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)vs->hd)))->rgn=ropt;}
# 94
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp16A=te;struct Cyc_Absyn_Exp*_tmp169=where_opt;Cyc_Tcexp_tcTest(_tmp16A,_tmp169,({const char*_tmp1E="switch clause guard";_tag_fat(_tmp1E,sizeof(char),20U);}));});
if(!Cyc_Absyn_no_side_effects_exp(where_opt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12B;_tmp12B.tag=0U,({struct _fat_ptr _tmp16B=({const char*_tmp21="cannot show &&-clause in pattern has no effects";_tag_fat(_tmp21,sizeof(char),48U);});_tmp12B.f1=_tmp16B;});_tmp12B;});void*_tmp1F[1U];_tmp1F[0]=& _tmp20;({unsigned _tmp16C=where_opt->loc;Cyc_Warn_err2(_tmp16C,_tag_fat(_tmp1F,sizeof(void*),1U));});});}
# 99
Cyc_Tcstmt_tcStmt(te,s,0);}}}
# 102
static int Cyc_Tcstmt_is_case_default(struct Cyc_Absyn_Switch_clause*c){
void*_tmp22=(c->pattern)->r;void*_stmttmp2=_tmp22;void*_tmp23=_stmttmp2;if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp23)->tag == 0U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 117 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 119
void*_tmp24=s0->r;void*_stmttmp3=_tmp24;void*_tmp25=_stmttmp3;struct Cyc_Absyn_Stmt*_tmp27;struct Cyc_Absyn_Decl*_tmp26;void**_tmp2A;struct Cyc_List_List**_tmp29;struct Cyc_Absyn_Stmt*_tmp28;void**_tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_Absyn_Exp**_tmp2B;struct Cyc_Absyn_Switch_clause***_tmp2F;struct Cyc_List_List*_tmp2E;struct Cyc_Absyn_Stmt*_tmp31;struct _fat_ptr*_tmp30;struct Cyc_Absyn_Stmt*_tmp34;struct Cyc_Absyn_Exp*_tmp33;struct Cyc_Absyn_Stmt*_tmp32;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Stmt*_tmp37;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Stmt*_tmp40;struct Cyc_Absyn_Stmt*_tmp3F;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Stmt*_tmp43;struct Cyc_Absyn_Stmt*_tmp42;struct Cyc_Absyn_Exp*_tmp44;switch(*((int*)_tmp25)){case 0U: _LL1: _LL2:
# 121
 return;case 1U: _LL3: _tmp44=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp44;
# 124
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(e);
return;}case 2U: _LL5: _tmp42=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp43=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp25)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp42;struct Cyc_Absyn_Stmt*s2=_tmp43;
# 130
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3U: _LL7: _tmp41=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp41;
# 135
void*t=Cyc_Tcenv_return_typ(te);
if(eopt == 0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp46=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12D;_tmp12D.tag=0U,({struct _fat_ptr _tmp16D=({const char*_tmp48="should return a value of type ";_tag_fat(_tmp48,sizeof(char),31U);});_tmp12D.f1=_tmp16D;});_tmp12D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp47=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp12C;_tmp12C.tag=2U,_tmp12C.f1=(void*)t;_tmp12C;});void*_tmp45[2U];_tmp45[0]=& _tmp46,_tmp45[1]=& _tmp47;({unsigned _tmp16E=s0->loc;Cyc_Warn_warn2(_tmp16E,_tag_fat(_tmp45,sizeof(void*),2U));});});else{
# 141
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12F;_tmp12F.tag=0U,({struct _fat_ptr _tmp16F=({const char*_tmp4C="must return a value of type ";_tag_fat(_tmp4C,sizeof(char),29U);});_tmp12F.f1=_tmp16F;});_tmp12F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp12E;_tmp12E.tag=2U,_tmp12E.f1=(void*)t;_tmp12E;});void*_tmp49[2U];_tmp49[0]=& _tmp4A,_tmp49[1]=& _tmp4B;({unsigned _tmp170=s0->loc;Cyc_Warn_err2(_tmp170,_tag_fat(_tmp49,sizeof(void*),2U));});});}}
# 143
return;}
# 146
Cyc_Tcexp_tcExp(te,& t,eopt);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp172=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp171=eopt;Cyc_Tcutil_coerce_arg(_tmp172,_tmp171,t,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp133;_tmp133.tag=0U,({struct _fat_ptr _tmp173=({const char*_tmp53="returns value of type ";_tag_fat(_tmp53,sizeof(char),23U);});_tmp133.f1=_tmp173;});_tmp133;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp4F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp132;_tmp132.tag=3U,_tmp132.f1=(void*)eopt->topt;_tmp132;});struct Cyc_Warn_String_Warn_Warg_struct _tmp50=({struct Cyc_Warn_String_Warn_Warg_struct _tmp131;_tmp131.tag=0U,({struct _fat_ptr _tmp174=({const char*_tmp52=" but requires ";_tag_fat(_tmp52,sizeof(char),15U);});_tmp131.f1=_tmp174;});_tmp131;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp51=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp130;_tmp130.tag=2U,_tmp130.f1=(void*)t;_tmp130;});void*_tmp4D[4U];_tmp4D[0]=& _tmp4E,_tmp4D[1]=& _tmp4F,_tmp4D[2]=& _tmp50,_tmp4D[3]=& _tmp51;({unsigned _tmp175=s0->loc;Cyc_Warn_err2(_tmp175,_tag_fat(_tmp4D,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}
# 151
Cyc_Tcexp_check_consume(eopt->loc,t,eopt);
return;}case 4U: _LL9: _tmp3E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp3F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp25)->f2;_tmp40=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp25)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp3E;struct Cyc_Absyn_Stmt*s1=_tmp3F;struct Cyc_Absyn_Stmt*s2=_tmp40;
# 155
({struct Cyc_Tcenv_Tenv*_tmp177=te;struct Cyc_Absyn_Exp*_tmp176=e;Cyc_Tcexp_tcTest(_tmp177,_tmp176,({const char*_tmp54="if statement";_tag_fat(_tmp54,sizeof(char),13U);}));});
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5U: _LLB: _tmp3B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp25)->f1).f1;_tmp3C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp25)->f1).f2;_tmp3D=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp25)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp3B;struct Cyc_Absyn_Stmt*cont_s=_tmp3C;struct Cyc_Absyn_Stmt*s=_tmp3D;
# 161
({struct Cyc_Tcenv_Tenv*_tmp179=te;struct Cyc_Absyn_Exp*_tmp178=e;Cyc_Tcexp_tcTest(_tmp179,_tmp178,({const char*_tmp55="while loop";_tag_fat(_tmp55,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9U: _LLD: _tmp35=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp36=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp25)->f2).f1;_tmp37=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp25)->f2).f2;_tmp38=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp25)->f3).f1;_tmp39=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp25)->f3).f2;_tmp3A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp25)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp35;struct Cyc_Absyn_Exp*e2=_tmp36;struct Cyc_Absyn_Stmt*guard_s=_tmp37;struct Cyc_Absyn_Exp*e3=_tmp38;struct Cyc_Absyn_Stmt*cont_s=_tmp39;struct Cyc_Absyn_Stmt*s=_tmp3A;
# 166
Cyc_Tcexp_tcExp(te,0,e1);
({struct Cyc_Tcenv_Tenv*_tmp17B=te;struct Cyc_Absyn_Exp*_tmp17A=e2;Cyc_Tcexp_tcTest(_tmp17B,_tmp17A,({const char*_tmp56="for loop";_tag_fat(_tmp56,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14U: _LLF: _tmp32=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp33=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp25)->f2).f1;_tmp34=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp25)->f2).f2;_LL10: {struct Cyc_Absyn_Stmt*s=_tmp32;struct Cyc_Absyn_Exp*e=_tmp33;struct Cyc_Absyn_Stmt*cont_s=_tmp34;
# 174
Cyc_Tcstmt_tcStmt(te,s,1);
({struct Cyc_Tcenv_Tenv*_tmp17D=te;struct Cyc_Absyn_Exp*_tmp17C=e;Cyc_Tcexp_tcTest(_tmp17D,_tmp17C,({const char*_tmp57="do loop";_tag_fat(_tmp57,sizeof(char),8U);}));});
return;}case 6U: _LL11: _LL12:
# 178
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 13U: _LL17: _tmp30=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp31=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp25)->f2;_LL18: {struct _fat_ptr*l=_tmp30;struct Cyc_Absyn_Stmt*s=_tmp31;
# 185
struct Cyc_Absyn_Tvar*_tmp58=({struct Cyc_Absyn_Tvar*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct _fat_ptr*_tmp182=({struct _fat_ptr*_tmp5C=_cycalloc(sizeof(*_tmp5C));({struct _fat_ptr _tmp181=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5B=({struct Cyc_String_pa_PrintArg_struct _tmp134;_tmp134.tag=0U,_tmp134.f1=(struct _fat_ptr)((struct _fat_ptr)*l);_tmp134;});void*_tmp59[1U];_tmp59[0]=& _tmp5B;({struct _fat_ptr _tmp180=({const char*_tmp5A="`%s";_tag_fat(_tmp5A,sizeof(char),4U);});Cyc_aprintf(_tmp180,_tag_fat(_tmp59,sizeof(void*),1U));});});*_tmp5C=_tmp181;});_tmp5C;});_tmp5D->name=_tmp182;}),({
int _tmp17F=Cyc_Tcutil_new_tvar_id();_tmp5D->identity=_tmp17F;}),({void*_tmp17E=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp5D->kind=_tmp17E;});_tmp5D;});
# 185
struct Cyc_Absyn_Tvar*tv=_tmp58;
# 187
({struct Cyc_Tcenv_Tenv*_tmp183=Cyc_Tcenv_new_named_block(s0->loc,te,tv);Cyc_Tcstmt_tcStmt(_tmp183,s,0);});
return;}case 11U: _LL19: _tmp2E=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp2F=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp25)->f2;_LL1A: {struct Cyc_List_List*es=_tmp2E;struct Cyc_Absyn_Switch_clause***clauseopt=_tmp2F;
# 191
const struct _tuple14*_tmp5E=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);const struct _tuple14*trip_opt=_tmp5E;
if(trip_opt == (const struct _tuple14*)0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp60=({struct Cyc_Warn_String_Warn_Warg_struct _tmp135;_tmp135.tag=0U,({struct _fat_ptr _tmp184=({const char*_tmp61="fallthru not in a non-last case";_tag_fat(_tmp61,sizeof(char),32U);});_tmp135.f1=_tmp184;});_tmp135;});void*_tmp5F[1U];_tmp5F[0]=& _tmp60;({unsigned _tmp185=s0->loc;Cyc_Warn_err2(_tmp185,_tag_fat(_tmp5F,sizeof(void*),1U));});});
return;}{
# 196
struct _tuple14 _tmp62=*trip_opt;struct _tuple14 _stmttmp4=_tmp62;struct _tuple14 _tmp63=_stmttmp4;struct Cyc_List_List*_tmp65;struct Cyc_List_List*_tmp64;_LL22: _tmp64=_tmp63.f2;_tmp65=_tmp63.f3;_LL23: {struct Cyc_List_List*tvs=_tmp64;struct Cyc_List_List*ts_orig=_tmp65;
struct Cyc_List_List*_tmp66=({struct Cyc_List_List*_tmp186=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp186,tvs);});struct Cyc_List_List*instantiation=_tmp66;
struct Cyc_List_List*_tmp67=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,ts_orig);struct Cyc_List_List*ts=_tmp67;
for(0;ts != 0 && es != 0;(ts=ts->tl,es=es->tl)){
# 201
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp188=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp187=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp188,_tmp187,(void*)ts->hd,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp69=({struct Cyc_Warn_String_Warn_Warg_struct _tmp139;_tmp139.tag=0U,({struct _fat_ptr _tmp189=({const char*_tmp6E="fallthru argument has type ";_tag_fat(_tmp6E,sizeof(char),28U);});_tmp139.f1=_tmp189;});_tmp139;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp138;_tmp138.tag=3U,_tmp138.f1=(void*)((struct Cyc_Absyn_Exp*)es->hd)->topt;_tmp138;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp137;_tmp137.tag=0U,({
struct _fat_ptr _tmp18A=({const char*_tmp6D="but pattern variable has type ";_tag_fat(_tmp6D,sizeof(char),31U);});_tmp137.f1=_tmp18A;});_tmp137;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp136;_tmp136.tag=2U,_tmp136.f1=(void*)ts->hd;_tmp136;});void*_tmp68[4U];_tmp68[0]=& _tmp69,_tmp68[1]=& _tmp6A,_tmp68[2]=& _tmp6B,_tmp68[3]=& _tmp6C;({unsigned _tmp18B=s0->loc;Cyc_Warn_err2(_tmp18B,_tag_fat(_tmp68,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}
# 208
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)ts->hd,(struct Cyc_Absyn_Exp*)es->hd);}
# 210
if(es != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13A;_tmp13A.tag=0U,({struct _fat_ptr _tmp18C=({const char*_tmp71="too many arguments in explicit fallthru";_tag_fat(_tmp71,sizeof(char),40U);});_tmp13A.f1=_tmp18C;});_tmp13A;});void*_tmp6F[1U];_tmp6F[0]=& _tmp70;({unsigned _tmp18D=s0->loc;Cyc_Warn_err2(_tmp18D,_tag_fat(_tmp6F,sizeof(void*),1U));});});
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp73=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13B;_tmp13B.tag=0U,({struct _fat_ptr _tmp18E=({const char*_tmp74="too few arguments in explicit fallthru";_tag_fat(_tmp74,sizeof(char),39U);});_tmp13B.f1=_tmp18E;});_tmp13B;});void*_tmp72[1U];_tmp72[0]=& _tmp73;({unsigned _tmp18F=s0->loc;Cyc_Warn_err2(_tmp18F,_tag_fat(_tmp72,sizeof(void*),1U));});});
return;}}}case 10U: _LL1B: _tmp2B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp2C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp25)->f2;_tmp2D=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp25)->f3;_LL1C: {struct Cyc_Absyn_Exp**exp=_tmp2B;struct Cyc_List_List*scs0=_tmp2C;void**dtp=_tmp2D;
# 218
if(Cyc_Tcstmt_make_var_exp(exp,s0)){
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 223
struct Cyc_Absyn_Exp*_tmp75=*exp;struct Cyc_Absyn_Exp*e=_tmp75;
void*_tmp76=Cyc_Tcexp_tcExp(te,0,e);void*t=_tmp76;
# 226
Cyc_Tcexp_check_consume(e->loc,t,e);{
# 230
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp77=scs0;struct Cyc_List_List*scs=_tmp77;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& t,e);}else{
# 238
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 240
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp78=_cycalloc(sizeof(*_tmp78));({struct Cyc_Tcpat_TcPatResult _tmp190=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& t,e);*_tmp78=_tmp190;});_tmp78;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 244
te=Cyc_Tcenv_clear_fallthru(te);else{
# 246
struct Cyc_List_List*_tmp79=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp79;
struct Cyc_List_List*_tmp7A=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 247
struct Cyc_List_List*new_tvs=_tmp7A;
# 249
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 252
te=Cyc_Tcenv_clear_fallthru(te);}{
# 254
struct Cyc_Absyn_Pat*_tmp7B=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmp7B;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13F;_tmp13F.tag=0U,({
struct _fat_ptr _tmp191=({const char*_tmp82="switch on type ";_tag_fat(_tmp82,sizeof(char),16U);});_tmp13F.f1=_tmp191;});_tmp13F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp13E;_tmp13E.tag=2U,_tmp13E.f1=(void*)t;_tmp13E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13D;_tmp13D.tag=0U,({struct _fat_ptr _tmp192=({const char*_tmp81=" but case expects type ";_tag_fat(_tmp81,sizeof(char),24U);});_tmp13D.f1=_tmp192;});_tmp13D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp80=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp13C;_tmp13C.tag=3U,_tmp13C.f1=(void*)p->topt;_tmp13C;});void*_tmp7C[4U];_tmp7C[0]=& _tmp7D,_tmp7C[1]=& _tmp7E,_tmp7C[2]=& _tmp7F,_tmp7C[3]=& _tmp80;({unsigned _tmp193=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp193,_tag_fat(_tmp7C,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}else{
# 261
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 263
({struct Cyc_Core_Opt*_tmp194=({struct Cyc_Core_Opt*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->v=this_pattern.patvars;_tmp83;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp194;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 267
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,scs0,dtp);
return;}}}case 15U: _LL1D: _tmp28=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp29=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp25)->f2;_tmp2A=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp25)->f3;_LL1E: {struct Cyc_Absyn_Stmt*s=_tmp28;struct Cyc_List_List**scs0=_tmp29;void**dtp=_tmp2A;
# 275
({void*_tmp198=({void*_tmp197=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=2U,({struct Cyc_Absyn_Stmt*_tmp196=Cyc_Absyn_new_stmt(s->r,s->loc);_tmp84->f1=_tmp196;}),({struct Cyc_Absyn_Stmt*_tmp195=Cyc_Absyn_skip_stmt(s->loc);_tmp84->f2=_tmp195;});_tmp84;});Cyc_Absyn_new_stmt(_tmp197,s->loc);})->r;s->r=_tmp198;});
# 277
Cyc_Tcstmt_tcStmt(te,s,1);
# 279
if(!((int(*)(int(*pred)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Tcstmt_is_case_default,*scs0)){
# 281
struct _tuple2*def_v=({struct _tuple2*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp19A=({struct _fat_ptr*_tmp8F=_cycalloc(sizeof(*_tmp8F));({struct _fat_ptr _tmp199=({const char*_tmp8E="exn";_tag_fat(_tmp8E,sizeof(char),4U);});*_tmp8F=_tmp199;});_tmp8F;});_tmp90->f2=_tmp19A;});_tmp90;});
struct Cyc_Absyn_Vardecl*_tmp85=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmp85;
struct Cyc_Absyn_Pat*_tmp86=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=1U,_tmp8D->f1=vd,({struct Cyc_Absyn_Pat*_tmp19B=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp8D->f2=_tmp19B;});_tmp8D;}),0U);struct Cyc_Absyn_Pat*def_pat=_tmp86;
struct Cyc_Absyn_Stmt*_tmp87=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=11U,({struct Cyc_Absyn_Exp*_tmp19C=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=4U,_tmp8B->f1=vd;_tmp8B;}),0U);_tmp8C->f1=_tmp19C;}),_tmp8C->f2=1;_tmp8C;}),0U),0U);
# 284
struct Cyc_Absyn_Stmt*def_stmt=_tmp87;
# 286
struct Cyc_Absyn_Switch_clause*_tmp88=({struct Cyc_Absyn_Switch_clause*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->pattern=def_pat,_tmp8A->pat_vars=0,_tmp8A->where_clause=0,_tmp8A->body=def_stmt,_tmp8A->loc=0U;_tmp8A;});struct Cyc_Absyn_Switch_clause*def_clause=_tmp88;
({struct Cyc_List_List*_tmp19E=({struct Cyc_List_List*_tmp19D=*scs0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp19D,({struct Cyc_Absyn_Switch_clause*_tmp89[1U];_tmp89[0]=def_clause;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp89,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*scs0=_tmp19E;});}{
# 293
void*_tmp91=Cyc_Absyn_exn_type();void*exception_type=_tmp91;
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp92=*scs0;struct Cyc_List_List*scs=_tmp92;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& exception_type,0);}else{
# 302
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 304
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp93=_cycalloc(sizeof(*_tmp93));({struct Cyc_Tcpat_TcPatResult _tmp19F=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& exception_type,0);*_tmp93=_tmp19F;});_tmp93;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 308
te=Cyc_Tcenv_clear_fallthru(te);else{
# 310
struct Cyc_List_List*_tmp94=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp94;
struct Cyc_List_List*_tmp95=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 311
struct Cyc_List_List*new_tvs=_tmp95;
# 313
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 316
te=Cyc_Tcenv_clear_fallthru(te);}{
# 318
struct Cyc_Absyn_Pat*_tmp96=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmp96;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),exception_type)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp98=({struct Cyc_Warn_String_Warn_Warg_struct _tmp143;_tmp143.tag=0U,({struct _fat_ptr _tmp1A0=({const char*_tmp9D="switch on type ";_tag_fat(_tmp9D,sizeof(char),16U);});_tmp143.f1=_tmp1A0;});_tmp143;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp99=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp142;_tmp142.tag=2U,_tmp142.f1=(void*)exception_type;_tmp142;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp141;_tmp141.tag=0U,({
struct _fat_ptr _tmp1A1=({const char*_tmp9C=" but case expects type ";_tag_fat(_tmp9C,sizeof(char),24U);});_tmp141.f1=_tmp1A1;});_tmp141;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp140;_tmp140.tag=3U,_tmp140.f1=(void*)p->topt;_tmp140;});void*_tmp97[4U];_tmp97[0]=& _tmp98,_tmp97[1]=& _tmp99,_tmp97[2]=& _tmp9A,_tmp97[3]=& _tmp9B;({unsigned _tmp1A2=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp1A2,_tag_fat(_tmp97,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}else{
# 325
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 327
({struct Cyc_Core_Opt*_tmp1A3=({struct Cyc_Core_Opt*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->v=this_pattern.patvars;_tmp9E;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp1A3;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 331
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*scs0,dtp);
return;}}default: _LL1F: _tmp26=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp25)->f1;_tmp27=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp25)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp26;struct Cyc_Absyn_Stmt*s=_tmp27;
# 335
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmp9F=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;struct Cyc_Tcenv_Tenv*te=_tmp9F;
{void*_tmpA0=d->r;void*_stmttmp5=_tmpA0;void*_tmpA1=_stmttmp5;struct Cyc_List_List*_tmpA3;struct _tuple2*_tmpA2;struct Cyc_List_List*_tmpA5;struct _fat_ptr*_tmpA4;struct Cyc_Absyn_Fndecl*_tmpA6;struct Cyc_Absyn_Tvar*_tmpA7;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Vardecl*_tmpA9;struct Cyc_Absyn_Tvar*_tmpA8;struct Cyc_List_List*_tmpAB;void**_tmpAF;struct Cyc_Absyn_Exp**_tmpAE;struct Cyc_Core_Opt**_tmpAD;struct Cyc_Absyn_Pat*_tmpAC;struct Cyc_Absyn_Vardecl*_tmpB0;switch(*((int*)_tmpA1)){case 0U: _LL25: _tmpB0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL26: {struct Cyc_Absyn_Vardecl*vd=_tmpB0;
# 339
struct Cyc_Absyn_Vardecl*_tmpB1=vd;void**_tmpB6;struct Cyc_Absyn_Exp*_tmpB5;void*_tmpB4;struct Cyc_Absyn_Tqual _tmpB3;enum Cyc_Absyn_Scope _tmpB2;_LL4A: _tmpB2=_tmpB1->sc;_tmpB3=_tmpB1->tq;_tmpB4=_tmpB1->type;_tmpB5=_tmpB1->initializer;_tmpB6=(void**)& _tmpB1->rgn;_LL4B: {enum Cyc_Absyn_Scope sc=_tmpB2;struct Cyc_Absyn_Tqual tq=_tmpB3;void*t=_tmpB4;struct Cyc_Absyn_Exp*initializer=_tmpB5;void**rgn_ptr=_tmpB6;
void*_tmpB7=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpB7;
int is_local;
{enum Cyc_Absyn_Scope _tmpB8=sc;switch(_tmpB8){case Cyc_Absyn_Static: _LL4D: _LL4E:
 goto _LL50;case Cyc_Absyn_Extern: _LL4F: _LL50:
 goto _LL52;case Cyc_Absyn_ExternC: _LL51: _LL52:
 vd->escapes=1;is_local=0;goto _LL4C;case Cyc_Absyn_Abstract: _LL53: _LL54:
# 347
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp144;_tmp144.tag=0U,({struct _fat_ptr _tmp1A4=({const char*_tmpBB="bad abstract scope for local variable";_tag_fat(_tmpBB,sizeof(char),38U);});_tmp144.f1=_tmp1A4;});_tmp144;});void*_tmpB9[1U];_tmpB9[0]=& _tmpBA;({unsigned _tmp1A5=d->loc;Cyc_Warn_err2(_tmp1A5,_tag_fat(_tmpB9,sizeof(void*),1U));});});
goto _LL56;case Cyc_Absyn_Register: _LL55: _LL56:
 goto _LL58;case Cyc_Absyn_Public: _LL57: _LL58:
 goto _LL5A;default: _LL59: _LL5A:
 is_local=1;goto _LL4C;}_LL4C:;}
# 354
*rgn_ptr=is_local?curr_bl: Cyc_Absyn_heap_rgn_type;
{void*_tmpBC=Cyc_Absyn_compress(t);void*_stmttmp6=_tmpBC;void*_tmpBD=_stmttmp6;struct Cyc_Absyn_ArrayInfo _tmpBE;switch(*((int*)_tmpBD)){case 5U: _LL5C: if(is_local){_LL5D:
# 358
 vd->escapes=1;
sc=3U;
is_local=0;
goto _LL5B;}else{goto _LL60;}case 4U: _LL5E: _tmpBE=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBD)->f1;_LL5F: {struct Cyc_Absyn_ArrayInfo ai=_tmpBE;
# 364
if(vd->initializer == 0 || ai.num_elts != 0)
goto _LL5B;{
struct Cyc_Absyn_Exp*len;
{void*_tmpBF=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmp7=_tmpBF;void*_tmpC0=_stmttmp7;struct Cyc_List_List*_tmpC1;struct Cyc_List_List*_tmpC2;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct _fat_ptr _tmpC5;struct _fat_ptr _tmpC6;switch(*((int*)_tmpC0)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC0)->f1).Wstring_c).tag){case 8U: _LL63: _tmpC6=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC0)->f1).String_c).val;_LL64: {struct _fat_ptr s=_tmpC6;
len=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);goto _LL62;}case 9U: _LL65: _tmpC5=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC0)->f1).Wstring_c).val;_LL66: {struct _fat_ptr s=_tmpC5;
len=Cyc_Absyn_uint_exp(1U,0U);goto _LL62;}default: goto _LL6F;}case 27U: _LL67: _tmpC4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpC0)->f2;_LL68: {struct Cyc_Absyn_Exp*e=_tmpC4;
_tmpC3=e;goto _LL6A;}case 28U: _LL69: _tmpC3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpC0)->f1;_LL6A: {struct Cyc_Absyn_Exp*e=_tmpC3;
len=e;goto _LL62;}case 36U: _LL6B: _tmpC2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC0)->f2;_LL6C: {struct Cyc_List_List*es=_tmpC2;
_tmpC1=es;goto _LL6E;}case 26U: _LL6D: _tmpC1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpC0)->f1;_LL6E: {struct Cyc_List_List*es=_tmpC1;
len=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);goto _LL62;}default: _LL6F: _LL70:
 len=0;goto _LL62;}_LL62:;}
# 376
ai.num_elts=len;
t=({void*_tmp1A6=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->tag=4U,_tmpC7->f1=ai;_tmpC7;});vd->type=_tmp1A6;});
goto _LL5B;}}default: _LL60: _LL61:
 goto _LL5B;}_LL5B:;}{
# 384
struct Cyc_List_List*_tmpC8=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*bound_vars=_tmpC8;
int _tmpC9=is_local;int allow_evars=_tmpC9;
Cyc_Tctyp_check_type(s0->loc,te,bound_vars,& Cyc_Kinds_tmk,allow_evars,1,t);
({int _tmp1A7=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp1A7;});
# 389
if((int)sc == (int)3U ||(int)sc == (int)4U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp145;_tmp145.tag=0U,({struct _fat_ptr _tmp1A8=({const char*_tmpCC="extern declarations are not yet supported within functions";_tag_fat(_tmpCC,sizeof(char),59U);});_tmp145.f1=_tmp1A8;});_tmp145;});void*_tmpCA[1U];_tmpCA[0]=& _tmpCB;({unsigned _tmp1A9=d->loc;Cyc_Warn_err2(_tmp1A9,_tag_fat(_tmpCA,sizeof(void*),1U));});});
if(initializer != 0){
Cyc_Tcexp_tcExpInitializer(te,& t,initializer);
# 394
if(!is_local && !Cyc_Tcutil_is_const_exp(initializer))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp146;_tmp146.tag=0U,({struct _fat_ptr _tmp1AA=({const char*_tmpCF="initializer for static variable needs to be a constant expression";_tag_fat(_tmpCF,sizeof(char),66U);});_tmp146.f1=_tmp1AA;});_tmp146;});void*_tmpCD[1U];_tmpCD[0]=& _tmpCE;({unsigned _tmp1AB=d->loc;Cyc_Warn_err2(_tmp1AB,_tag_fat(_tmpCD,sizeof(void*),1U));});});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp1AD=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1AC=initializer;Cyc_Tcutil_coerce_assign(_tmp1AD,_tmp1AC,t);})){
({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmpD1=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp14B;_tmp14B.tag=11U,_tmp14B.f1=vd;_tmp14B;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14A;_tmp14A.tag=0U,({struct _fat_ptr _tmp1AE=({const char*_tmpD7=" was declared with type ";_tag_fat(_tmpD7,sizeof(char),25U);});_tmp14A.f1=_tmp1AE;});_tmp14A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp149;_tmp149.tag=2U,_tmp149.f1=(void*)t;_tmp149;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp148;_tmp148.tag=0U,({
struct _fat_ptr _tmp1AF=({const char*_tmpD6=" but initialized with type ";_tag_fat(_tmpD6,sizeof(char),28U);});_tmp148.f1=_tmp1AF;});_tmp148;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp147;_tmp147.tag=3U,_tmp147.f1=(void*)initializer->topt;_tmp147;});void*_tmpD0[5U];_tmpD0[0]=& _tmpD1,_tmpD0[1]=& _tmpD2,_tmpD0[2]=& _tmpD3,_tmpD0[3]=& _tmpD4,_tmpD0[4]=& _tmpD5;({unsigned _tmp1B0=d->loc;Cyc_Warn_err2(_tmp1B0,_tag_fat(_tmpD0,sizeof(void*),5U));});});
Cyc_Unify_unify(t,(void*)_check_null(initializer->topt));
Cyc_Unify_explain_failure();}}
# 403
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}case 2U: _LL27: _tmpAC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpAD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_tmpAE=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f3;_tmpAF=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f4;_LL28: {struct Cyc_Absyn_Pat*p=_tmpAC;struct Cyc_Core_Opt**vds=_tmpAD;struct Cyc_Absyn_Exp**exp=_tmpAE;void**dtp=_tmpAF;
# 408
{void*_tmpD8=p->r;void*_stmttmp8=_tmpD8;void*_tmpD9=_stmttmp8;switch(*((int*)_tmpD9)){case 1U: _LL72: _LL73:
 goto _LL75;case 2U: _LL74: _LL75:
 goto _LL77;case 15U: _LL76: _LL77:
 goto _LL71;default: _LL78: _LL79:
# 413
 if(Cyc_Tcstmt_make_var_exp(exp,s0)){
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}}_LL71:;}{
# 419
struct Cyc_Absyn_Exp*_tmpDA=*exp;struct Cyc_Absyn_Exp*e=_tmpDA;
Cyc_Tcexp_tcExpInitializer(te,0,e);{
# 422
void*pat_type=(void*)_check_null(e->topt);
# 424
struct Cyc_Tcpat_TcPatResult _tmpDB=Cyc_Tcpat_tcPat(te,p,& pat_type,e);struct Cyc_Tcpat_TcPatResult pat_res=_tmpDB;
({struct Cyc_Core_Opt*_tmp1B1=({struct Cyc_Core_Opt*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->v=pat_res.patvars;_tmpDC;});*vds=_tmp1B1;});{
void*_tmpDD=(void*)_check_null(p->topt);void*pt=_tmpDD;
void*_tmpDE=(void*)_check_null(e->topt);void*et=_tmpDE;
if(!Cyc_Unify_unify(pt,et)&& !({struct Cyc_RgnOrder_RgnPO*_tmp1B3=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1B2=e;Cyc_Tcutil_coerce_assign(_tmp1B3,_tmp1B2,pt);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14F;_tmp14F.tag=0U,({
struct _fat_ptr _tmp1B4=({const char*_tmpE5="pattern type ";_tag_fat(_tmpE5,sizeof(char),14U);});_tmp14F.f1=_tmp1B4;});_tmp14F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14E;_tmp14E.tag=2U,_tmp14E.f1=(void*)pt;_tmp14E;});struct Cyc_Warn_String_Warn_Warg_struct _tmpE2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14D;_tmp14D.tag=0U,({struct _fat_ptr _tmp1B5=({const char*_tmpE4=" does not match definition type ";_tag_fat(_tmpE4,sizeof(char),33U);});_tmp14D.f1=_tmp1B5;});_tmp14D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14C;_tmp14C.tag=2U,_tmp14C.f1=(void*)et;_tmp14C;});void*_tmpDF[4U];_tmpDF[0]=& _tmpE0,_tmpDF[1]=& _tmpE1,_tmpDF[2]=& _tmpE2,_tmpDF[3]=& _tmpE3;({unsigned _tmp1B6=d->loc;Cyc_Warn_err2(_tmp1B6,_tag_fat(_tmpDF,sizeof(void*),4U));});});
Cyc_Unify_unify(pt,et);
Cyc_Unify_explain_failure();}else{
# 435
Cyc_Tcpat_check_pat_regions(te,p,pat_res.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(p->loc,te,p,dtp);
Cyc_Tcstmt_pattern_synth(s0->loc,te,pat_res,s,0,0);
return;}}}}case 3U: _LL29: _tmpAB=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL2A: {struct Cyc_List_List*vds=_tmpAB;
# 441
void*_tmpE6=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpE6;
for(0;vds != 0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=curr_bl;
({unsigned _tmp1B9=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1B8=te;struct Cyc_List_List*_tmp1B7=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1B9,_tmp1B8,_tmp1B7,& Cyc_Kinds_tmk,1,1,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);});}
# 447
Cyc_Tcstmt_tcStmt(te,s,0);
return;}case 4U: _LL2B: _tmpA8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpA9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_tmpAA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f3;_LL2C: {struct Cyc_Absyn_Tvar*tv=_tmpA8;struct Cyc_Absyn_Vardecl*vd=_tmpA9;struct Cyc_Absyn_Exp*open_exp_opt=_tmpAA;
# 453
({void*_tmp1BD=({void*_tmp1BC=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1BB=Cyc_Absyn_new_stmt(s->r,s->loc);_tmpE7->f1=_tmp1BB;}),({struct Cyc_Absyn_Stmt*_tmp1BA=Cyc_Absyn_skip_stmt(s->loc);_tmpE7->f2=_tmp1BA;});_tmpE7;});Cyc_Absyn_new_stmt(_tmp1BC,s->loc);})->r;s->r=_tmp1BD;});{
# 455
struct Cyc_Tcenv_Tenv*_tmpE8=te;struct Cyc_Tcenv_Tenv*te2=_tmpE8;
struct Cyc_Absyn_Vardecl*_tmpE9=vd;void**_tmpEB;void**_tmpEA;_LL7B: _tmpEA=(void**)& _tmpE9->type;_tmpEB=(void**)& _tmpE9->rgn;_LL7C: {void**t=_tmpEA;void**rgn_ptr=_tmpEB;
void*_tmpEC=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpEC;
*rgn_ptr=curr_bl;{
void*rgn_typ;
if((unsigned)open_exp_opt){
# 463
struct _tuple2*_tmpED=({struct _tuple2*_tmp104=_cycalloc(sizeof(*_tmp104));({union Cyc_Absyn_Nmspace _tmp1C2=Cyc_Absyn_Abs_n(({struct _fat_ptr*_tmpFF[1U];({struct _fat_ptr*_tmp1C1=({struct _fat_ptr*_tmp101=_cycalloc(sizeof(*_tmp101));({struct _fat_ptr _tmp1C0=({const char*_tmp100="Core";_tag_fat(_tmp100,sizeof(char),5U);});*_tmp101=_tmp1C0;});_tmp101;});_tmpFF[0]=_tmp1C1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpFF,sizeof(struct _fat_ptr*),1U));}),0);_tmp104->f1=_tmp1C2;}),({struct _fat_ptr*_tmp1BF=({struct _fat_ptr*_tmp103=_cycalloc(sizeof(*_tmp103));({struct _fat_ptr _tmp1BE=({const char*_tmp102="DynamicRegion";_tag_fat(_tmp102,sizeof(char),14U);});*_tmp103=_tmp1BE;});_tmp103;});_tmp104->f2=_tmp1BF;});_tmp104;});struct _tuple2*drname=_tmpED;
void*_tmpEE=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->v=& Cyc_Kinds_trk;_tmpFE;}),0);void*outer_rgn=_tmpEE;
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->v=& Cyc_Kinds_rk;_tmpEF;}),0);{
void*_tmpF0=({union Cyc_Absyn_AggrInfo _tmp1C3=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);Cyc_Absyn_aggr_type(_tmp1C3,({void*_tmpFD[1U];_tmpFD[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpFD,sizeof(void*),1U));}));});void*dr_type=_tmpF0;
void*_tmpF1=({void*_tmp1C7=dr_type;void*_tmp1C6=outer_rgn;struct Cyc_Absyn_Tqual _tmp1C5=Cyc_Absyn_empty_tqual(0U);void*_tmp1C4=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp1C7,_tmp1C6,_tmp1C5,_tmp1C4,Cyc_Absyn_false_type);});void*exp_type=_tmpF1;
({unsigned _tmp1CA=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1C9=te;struct Cyc_List_List*_tmp1C8=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1CA,_tmp1C9,_tmp1C8,& Cyc_Kinds_tmk,1,0,exp_type);});{
# 470
void*_tmpF2=Cyc_Tcexp_tcExp(te,& exp_type,open_exp_opt);void*key_typ=_tmpF2;
if(!Cyc_Unify_unify(exp_type,key_typ)&& !({
struct Cyc_RgnOrder_RgnPO*_tmp1CC=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1CB=open_exp_opt;Cyc_Tcutil_coerce_assign(_tmp1CC,_tmp1CB,exp_type);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp153;_tmp153.tag=0U,({struct _fat_ptr _tmp1CD=({const char*_tmpF9="expected ";_tag_fat(_tmpF9,sizeof(char),10U);});_tmp153.f1=_tmp1CD;});_tmp153;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp152;_tmp152.tag=2U,_tmp152.f1=(void*)exp_type;_tmp152;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp151;_tmp151.tag=0U,({struct _fat_ptr _tmp1CE=({const char*_tmpF8=" but found ";_tag_fat(_tmpF8,sizeof(char),12U);});_tmp151.f1=_tmp1CE;});_tmp151;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp150;_tmp150.tag=2U,_tmp150.f1=(void*)key_typ;_tmp150;});void*_tmpF3[4U];_tmpF3[0]=& _tmpF4,_tmpF3[1]=& _tmpF5,_tmpF3[2]=& _tmpF6,_tmpF3[3]=& _tmpF7;({unsigned _tmp1CF=s0->loc;Cyc_Warn_err2(_tmp1CF,_tag_fat(_tmpF3,sizeof(void*),4U));});});
if(!Cyc_Unify_unify(outer_rgn,Cyc_Absyn_unique_rgn_type)&& !
Cyc_Unify_unify(outer_rgn,Cyc_Absyn_refcnt_rgn_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpFB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp154;_tmp154.tag=0U,({struct _fat_ptr _tmp1D0=({const char*_tmpFC="open is only allowed on unique or reference-counted keys";_tag_fat(_tmpFC,sizeof(char),57U);});_tmp154.f1=_tmp1D0;});_tmp154;});void*_tmpFA[1U];_tmpFA[0]=& _tmpFB;({unsigned _tmp1D1=s0->loc;Cyc_Warn_err2(_tmp1D1,_tag_fat(_tmpFA,sizeof(void*),1U));});});
({void*_tmp1D2=Cyc_Absyn_rgn_handle_type(rgn_typ);*t=_tmp1D2;});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1,0);}}}else{
# 480
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=2U,_tmp105->f1=tv;_tmp105;});
te2=({unsigned _tmp1D4=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1D3=te2;Cyc_Tcenv_add_type_vars(_tmp1D4,_tmp1D3,({struct Cyc_List_List*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->hd=tv,_tmp106->tl=0;_tmp106;}));});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0,1);}
# 484
({unsigned _tmp1D7=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1D6=te;struct Cyc_List_List*_tmp1D5=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp1D7,_tmp1D6,_tmp1D5,& Cyc_Kinds_bk,1,0,*t);});
if(!({void*_tmp1D8=*t;Cyc_Unify_unify(_tmp1D8,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({void*_tmp107=0U;({struct _fat_ptr _tmp1D9=({const char*_tmp108="region stmt: type of region handle is wrong!";_tag_fat(_tmp108,sizeof(char),45U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1D9,_tag_fat(_tmp107,sizeof(void*),0U));});});
# 488
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 5U: _LL2D: _tmpA7=((struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL2E: {struct Cyc_Absyn_Tvar*tv=_tmpA7;
# 494
({void*_tmp1DD=({void*_tmp1DC=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1DB=Cyc_Absyn_new_stmt(s->r,s->loc);_tmp109->f1=_tmp1DB;}),({struct Cyc_Absyn_Stmt*_tmp1DA=Cyc_Absyn_skip_stmt(s->loc);_tmp109->f2=_tmp1DA;});_tmp109;});Cyc_Absyn_new_stmt(_tmp1DC,s->loc);})->r;s->r=_tmp1DD;});{
# 496
struct Cyc_Tcenv_Tenv*_tmp10A=te;struct Cyc_Tcenv_Tenv*te2=_tmp10A;
void*pool_rgn_typ;
pool_rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->tag=2U,_tmp10B->f1=tv;_tmp10B;});
te2=({unsigned _tmp1DF=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1DE=te2;Cyc_Tcenv_add_type_vars(_tmp1DF,_tmp1DE,({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=tv,_tmp10C->tl=0;_tmp10C;}));});
te2=Cyc_Tcenv_add_pool(te2,pool_rgn_typ);
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}case 1U: _LL2F: _tmpA6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL30: {struct Cyc_Absyn_Fndecl*fd=_tmpA6;
# 505
if((int)fd->sc != (int)Cyc_Absyn_Public)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp155;_tmp155.tag=0U,({struct _fat_ptr _tmp1E0=({const char*_tmp10F="bad storage class for inner function";_tag_fat(_tmp10F,sizeof(char),37U);});_tmp155.f1=_tmp1E0;});_tmp155;});void*_tmp10D[1U];_tmp10D[0]=& _tmp10E;({unsigned _tmp1E1=d->loc;Cyc_Warn_err2(_tmp1E1,_tag_fat(_tmp10D,sizeof(void*),1U));});});
Cyc_Atts_check_fndecl_atts(d->loc,(fd->i).attributes,1);
Cyc_Tctyp_check_fndecl_valid_type(d->loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
({struct Cyc_List_List*_tmp1E2=Cyc_Atts_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp1E2;});{
void*_tmp110=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp110;
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->sc=fd->sc,_tmp111->name=fd->name,_tmp111->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1E8=Cyc_Absyn_const_tqual(0U);_tmp111->tq=_tmp1E8;}),({
void*_tmp1E7=({void*_tmp1E6=t;void*_tmp1E5=curr_bl;struct Cyc_Absyn_Tqual _tmp1E4=Cyc_Absyn_empty_tqual(0U);void*_tmp1E3=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp1E6,_tmp1E5,_tmp1E4,_tmp1E3,Cyc_Absyn_false_type);});_tmp111->type=_tmp1E7;}),_tmp111->initializer=0,_tmp111->rgn=curr_bl,_tmp111->attributes=0,_tmp111->escapes=0,_tmp111->is_proto=fd->body != 0;_tmp111;});
# 517
fd->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(te->le);
({struct Cyc_Tcenv_Fenv*_tmp1E9=Cyc_Tcenv_nested_fenv(d->loc,old_fenv,fd);te->le=_tmp1E9;});
Cyc_Tcstmt_tcStmt(te,fd->body,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
te->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 10U: _LL31: _tmpA4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpA5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_LL32: {struct _fat_ptr*n=_tmpA4;struct Cyc_List_List*tds=_tmpA5;
unimp_msg_part=({const char*_tmp112="namespace";_tag_fat(_tmp112,sizeof(char),10U);});goto _LL24;}case 11U: _LL33: _tmpA2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpA3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_LL34: {struct _tuple2*q=_tmpA2;struct Cyc_List_List*tds=_tmpA3;
unimp_msg_part=({const char*_tmp113="using";_tag_fat(_tmp113,sizeof(char),6U);});goto _LL24;}case 6U: _LL35: _LL36:
 unimp_msg_part=({const char*_tmp114="type";_tag_fat(_tmp114,sizeof(char),5U);});goto _LL24;case 7U: _LL37: _LL38:
 unimp_msg_part=({const char*_tmp115="datatype";_tag_fat(_tmp115,sizeof(char),9U);});goto _LL24;case 8U: _LL39: _LL3A:
 unimp_msg_part=({const char*_tmp116="enum";_tag_fat(_tmp116,sizeof(char),5U);});goto _LL24;case 9U: _LL3B: _LL3C:
 unimp_msg_part=({const char*_tmp117="typedef";_tag_fat(_tmp117,sizeof(char),8U);});goto _LL24;case 12U: _LL3D: _LL3E:
 unimp_msg_part=({const char*_tmp118="extern \"C\"";_tag_fat(_tmp118,sizeof(char),11U);});goto _LL24;case 13U: _LL3F: _LL40:
 unimp_msg_part=({const char*_tmp119="extern \"C include\"";_tag_fat(_tmp119,sizeof(char),19U);});goto _LL24;case 14U: _LL41: _LL42:
 unimp_msg_part=({const char*_tmp11A="__cyclone_port_on__";_tag_fat(_tmp11A,sizeof(char),20U);});goto _LL24;case 15U: _LL43: _LL44:
 unimp_msg_part=({const char*_tmp11B="__cyclone_port_off__";_tag_fat(_tmp11B,sizeof(char),21U);});goto _LL24;case 16U: _LL45: _LL46:
 unimp_msg_part=({const char*_tmp11C="__tempest_on__";_tag_fat(_tmp11C,sizeof(char),15U);});goto _LL24;default: _LL47: _LL48:
 unimp_msg_part=({const char*_tmp11D="__tempest_off__";_tag_fat(_tmp11D,sizeof(char),16U);});goto _LL24;}_LL24:;}
# 539
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp158;_tmp158.tag=0U,({struct _fat_ptr _tmp1EA=({const char*_tmp123="tcStmt: nested ";_tag_fat(_tmp123,sizeof(char),16U);});_tmp158.f1=_tmp1EA;});_tmp158;});struct Cyc_Warn_String_Warn_Warg_struct _tmp120=({struct Cyc_Warn_String_Warn_Warg_struct _tmp157;_tmp157.tag=0U,_tmp157.f1=unimp_msg_part;_tmp157;});struct Cyc_Warn_String_Warn_Warg_struct _tmp121=({struct Cyc_Warn_String_Warn_Warg_struct _tmp156;_tmp156.tag=0U,({struct _fat_ptr _tmp1EB=({const char*_tmp122=" declarations not allowed";_tag_fat(_tmp122,sizeof(char),26U);});_tmp156.f1=_tmp1EB;});_tmp156;});void*_tmp11E[3U];_tmp11E[0]=& _tmp11F,_tmp11E[1]=& _tmp120,_tmp11E[2]=& _tmp121;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp11E,sizeof(void*),3U));});}}_LL0:;}
# 546
static int Cyc_Tcstmt_ensures_no_throws_exp(struct Cyc_Absyn_Exp*e){
void*_tmp124=e->r;void*_stmttmp9=_tmp124;void*_tmp125=_stmttmp9;struct Cyc_List_List*_tmp126;switch(*((int*)_tmp125)){case 0U: _LL1: _LL2:
 return 1;case 3U: _LL3: _tmp126=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp125)->f2;_LL4: {struct Cyc_List_List*es=_tmp126;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_ensures_no_throws_exp,es);}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 553
int Cyc_Tcstmt_ensure_no_throws(struct Cyc_Absyn_Stmt*s){
void*_tmp127=s->r;void*_stmttmpA=_tmp127;void*_tmp128=_stmttmpA;struct Cyc_Absyn_Exp*_tmp129;if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp128)->tag == 3U){_LL1: _tmp129=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp128)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp129;
return e == 0?1: Cyc_Tcstmt_ensures_no_throws_exp(e);}}else{_LL3: _LL4:
 return 0;}_LL0:;}

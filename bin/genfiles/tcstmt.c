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
# 108 "core.h"
extern void*Cyc_Core_fst(struct _tuple0*);
# 111
extern void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 173
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
# 258
extern int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_Position_Error;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 311
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 513
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 531
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 699 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 894
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 904
void*Cyc_Absyn_compress(void*);
# 918
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 930
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 934
extern void*Cyc_Absyn_false_type;
# 936
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_rgn_handle_type(void*);
# 953
void*Cyc_Absyn_exn_type (void);
# 975
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 992
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1067
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1089
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1094
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1098
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple2*,void*,struct Cyc_Absyn_Exp*init);
# 1138
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 36
int Cyc_Tcutil_is_void_type(void*);
# 38
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 111
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 128
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 130
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 140
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 167
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 178
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 196
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 200
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 252
int Cyc_Tcutil_new_tvar_id (void);
# 260
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 264
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 274
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 35 "attributes.h"
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 142
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened,int lifo);
# 153
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
({void*_tmp15D=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=5U,_tmp4->f1=e1,_tmp4->f2=Cyc_Absyn_PreInc;_tmp4;});e->r=_tmp15D;});goto _LL0;}case Cyc_Absyn_PostDec: _LL3: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2;
({void*_tmp15E=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=5U,_tmp5->f1=e1,_tmp5->f2=Cyc_Absyn_PreDec;_tmp5;});e->r=_tmp15E;});goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 50
static int Cyc_Tcstmt_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp6=e->r;void*_stmttmp1=_tmp6;void*_tmp7=_stmttmp1;struct Cyc_Absyn_Exp*_tmp8;struct Cyc_Absyn_Exp*_tmp9;switch(*((int*)_tmp7)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL4: {struct Cyc_Absyn_Exp*e2=_tmp9;
_tmp8=e2;goto _LL6;}case 13U: _LL5: _tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL6: {struct Cyc_Absyn_Exp*e2=_tmp8;
e=e2;continue;}default: _LL7: _LL8:
# 57
 return 0;}_LL0:;}}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 63
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 65
struct Cyc_Tcpat_TcPatResult _tmpA=pat_res;struct Cyc_List_List*_tmpC;struct _tuple1*_tmpB;_LL1: _tmpB=_tmpA.tvars_and_bounds_opt;_tmpC=_tmpA.patvars;_LL2: {struct _tuple1*p=_tmpB;struct Cyc_List_List*vse=_tmpC;
struct Cyc_List_List*_tmpD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*))Cyc_Core_fst,vse);struct Cyc_List_List*vs=_tmpD;
struct Cyc_List_List*_tmpE=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*p).f1);struct Cyc_List_List*tvs=_tmpE;
struct Cyc_List_List*_tmpF=p == 0?0:(*p).f2;struct Cyc_List_List*bds=_tmpF;
struct Cyc_List_List*_tmp10=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple15*))Cyc_Core_snd,(*p).f1));struct Cyc_List_List*rgns=_tmp10;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,tvs);
for(0;rgns != 0;rgns=rgns->tl){
te2=({struct Cyc_Tcenv_Tenv*_tmp15F=te2;Cyc_Tcenv_add_region(_tmp15F,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=2U,_tmp11->f1=(struct Cyc_Absyn_Tvar*)rgns->hd;_tmp11;}),0,0);});}
te2=Cyc_Tcenv_new_outlives_constraints(te2,bds,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(loc,te2);{
void*_tmp12=Cyc_Tcenv_curr_rgn(te2);void*ropt=_tmp12;
{struct Cyc_List_List*_tmp13=vs;struct Cyc_List_List*vs2=_tmp13;for(0;vs2 != 0;vs2=vs2->tl){
if((struct Cyc_Absyn_Vardecl**)vs2->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)vs2->hd)))->rgn=ropt;}}
# 81
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp161=te2;struct Cyc_Absyn_Exp*_tmp160=where_opt;Cyc_Tcexp_tcTest(_tmp161,_tmp160,({const char*_tmp14="switch clause guard";_tag_fat(_tmp14,sizeof(char),20U);}));});
if(!Cyc_Absyn_no_side_effects_exp(where_opt))
({void*_tmp15=0U;({unsigned _tmp163=where_opt->loc;struct _fat_ptr _tmp162=({const char*_tmp16="cannot show &&-clause in pattern has no effects";_tag_fat(_tmp16,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp163,_tmp162,_tag_fat(_tmp15,sizeof(void*),0U));});});}
# 86
Cyc_Tcstmt_tcStmt(te2,s,0);}}}
# 89
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var (void){
static int counter=0;
struct _tuple2*res=({struct _tuple2*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp166=({struct _fat_ptr*_tmp1A=_cycalloc(sizeof(*_tmp1A));({struct _fat_ptr _tmp165=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp19=({struct Cyc_Int_pa_PrintArg_struct _tmp12F;_tmp12F.tag=1U,_tmp12F.f1=(unsigned)counter ++;_tmp12F;});void*_tmp17[1U];_tmp17[0]=& _tmp19;({struct _fat_ptr _tmp164=({const char*_tmp18="_stmttmp%X";_tag_fat(_tmp18,sizeof(char),11U);});Cyc_aprintf(_tmp164,_tag_fat(_tmp17,sizeof(void*),1U));});});*_tmp1A=_tmp165;});_tmp1A;});_tmp1B->f2=_tmp166;});_tmp1B;});
return res;}
# 95
static int Cyc_Tcstmt_is_case_default(struct Cyc_Absyn_Switch_clause*c){
void*_tmp1C=(c->pattern)->r;void*_stmttmp2=_tmp1C;void*_tmp1D=_stmttmp2;if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1D)->tag == 0U){_LL1: _LL2:
# 98
 return 1;}else{_LL3: _LL4:
# 100
 return 0;}_LL0:;}
# 112 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 114
void*_tmp1E=s0->r;void*_stmttmp3=_tmp1E;void*_tmp1F=_stmttmp3;struct Cyc_Absyn_Stmt*_tmp21;struct Cyc_Absyn_Decl*_tmp20;void**_tmp24;struct Cyc_List_List**_tmp23;struct Cyc_Absyn_Stmt*_tmp22;void**_tmp27;struct Cyc_List_List*_tmp26;struct Cyc_Absyn_Exp**_tmp25;struct Cyc_Absyn_Switch_clause***_tmp29;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Stmt*_tmp2B;struct _fat_ptr*_tmp2A;struct Cyc_Absyn_Stmt*_tmp2E;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Stmt*_tmp2C;struct Cyc_Absyn_Stmt*_tmp34;struct Cyc_Absyn_Stmt*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Stmt*_tmp31;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_Absyn_Stmt*_tmp37;struct Cyc_Absyn_Stmt*_tmp36;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_Absyn_Exp*_tmp3E;switch(*((int*)_tmp1F)){case 0U: _LL1: _LL2:
# 116
 return;case 1U: _LL3: _tmp3E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp3E;
# 119
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(e);
return;}case 2U: _LL5: _tmp3C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp3D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp3C;struct Cyc_Absyn_Stmt*s2=_tmp3D;
# 125
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3U: _LL7: _tmp3B=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp3B;
# 130
void*t=Cyc_Tcenv_return_typ(te);
if(eopt == 0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40=({struct Cyc_Warn_String_Warn_Warg_struct _tmp131;_tmp131.tag=0U,({struct _fat_ptr _tmp167=({const char*_tmp42="should return a value of type ";_tag_fat(_tmp42,sizeof(char),31U);});_tmp131.f1=_tmp167;});_tmp131;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp41=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp130;_tmp130.tag=2U,_tmp130.f1=(void*)t;_tmp130;});void*_tmp3F[2U];_tmp3F[0]=& _tmp40,_tmp3F[1]=& _tmp41;({unsigned _tmp168=s0->loc;Cyc_Warn_warn2(_tmp168,_tag_fat(_tmp3F,sizeof(void*),2U));});});else{
# 136
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44=({struct Cyc_Warn_String_Warn_Warg_struct _tmp133;_tmp133.tag=0U,({struct _fat_ptr _tmp169=({const char*_tmp46="must return a value of type ";_tag_fat(_tmp46,sizeof(char),29U);});_tmp133.f1=_tmp169;});_tmp133;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp45=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp132;_tmp132.tag=2U,_tmp132.f1=(void*)t;_tmp132;});void*_tmp43[2U];_tmp43[0]=& _tmp44,_tmp43[1]=& _tmp45;({unsigned _tmp16A=s0->loc;Cyc_Warn_err2(_tmp16A,_tag_fat(_tmp43,sizeof(void*),2U));});});}}
# 138
return;}
# 141
Cyc_Tcexp_tcExp(te,& t,eopt);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp16C=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp16B=eopt;Cyc_Tcutil_coerce_arg(_tmp16C,_tmp16B,t,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp48=({struct Cyc_Warn_String_Warn_Warg_struct _tmp137;_tmp137.tag=0U,({struct _fat_ptr _tmp16D=({const char*_tmp4D="returns value of type ";_tag_fat(_tmp4D,sizeof(char),23U);});_tmp137.f1=_tmp16D;});_tmp137;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp49=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp136;_tmp136.tag=3U,_tmp136.f1=(void*)eopt->topt;_tmp136;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp135;_tmp135.tag=0U,({struct _fat_ptr _tmp16E=({const char*_tmp4C=" but requires ";_tag_fat(_tmp4C,sizeof(char),15U);});_tmp135.f1=_tmp16E;});_tmp135;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp134;_tmp134.tag=2U,_tmp134.f1=(void*)t;_tmp134;});void*_tmp47[4U];_tmp47[0]=& _tmp48,_tmp47[1]=& _tmp49,_tmp47[2]=& _tmp4A,_tmp47[3]=& _tmp4B;({unsigned _tmp16F=s0->loc;Cyc_Warn_err2(_tmp16F,_tag_fat(_tmp47,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}
# 146
Cyc_Tcexp_check_consume(eopt->loc,t,eopt);
return;}case 4U: _LL9: _tmp38=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp39=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2;_tmp3A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1F)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp38;struct Cyc_Absyn_Stmt*s1=_tmp39;struct Cyc_Absyn_Stmt*s2=_tmp3A;
# 150
({struct Cyc_Tcenv_Tenv*_tmp171=te;struct Cyc_Absyn_Exp*_tmp170=e;Cyc_Tcexp_tcTest(_tmp171,_tmp170,({const char*_tmp4E="if statement";_tag_fat(_tmp4E,sizeof(char),13U);}));});
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5U: _LLB: _tmp35=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1).f1;_tmp36=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1).f2;_tmp37=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp35;struct Cyc_Absyn_Stmt*cont_s=_tmp36;struct Cyc_Absyn_Stmt*s=_tmp37;
# 156
({struct Cyc_Tcenv_Tenv*_tmp173=te;struct Cyc_Absyn_Exp*_tmp172=e;Cyc_Tcexp_tcTest(_tmp173,_tmp172,({const char*_tmp4F="while loop";_tag_fat(_tmp4F,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9U: _LLD: _tmp2F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp30=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2).f1;_tmp31=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2).f2;_tmp32=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1F)->f3).f1;_tmp33=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1F)->f3).f2;_tmp34=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1F)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp2F;struct Cyc_Absyn_Exp*e2=_tmp30;struct Cyc_Absyn_Stmt*guard_s=_tmp31;struct Cyc_Absyn_Exp*e3=_tmp32;struct Cyc_Absyn_Stmt*cont_s=_tmp33;struct Cyc_Absyn_Stmt*s=_tmp34;
# 161
Cyc_Tcexp_tcExp(te,0,e1);
({struct Cyc_Tcenv_Tenv*_tmp175=te;struct Cyc_Absyn_Exp*_tmp174=e2;Cyc_Tcexp_tcTest(_tmp175,_tmp174,({const char*_tmp50="for loop";_tag_fat(_tmp50,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14U: _LLF: _tmp2C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp2D=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2).f1;_tmp2E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2).f2;_LL10: {struct Cyc_Absyn_Stmt*s=_tmp2C;struct Cyc_Absyn_Exp*e=_tmp2D;struct Cyc_Absyn_Stmt*cont_s=_tmp2E;
# 169
Cyc_Tcstmt_tcStmt(te,s,1);
({struct Cyc_Tcenv_Tenv*_tmp177=te;struct Cyc_Absyn_Exp*_tmp176=e;Cyc_Tcexp_tcTest(_tmp177,_tmp176,({const char*_tmp51="do loop";_tag_fat(_tmp51,sizeof(char),8U);}));});
return;}case 6U: _LL11: _LL12:
# 173
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 13U: _LL17: _tmp2A=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp2B=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2;_LL18: {struct _fat_ptr*l=_tmp2A;struct Cyc_Absyn_Stmt*s=_tmp2B;
# 180
struct Cyc_Absyn_Tvar*_tmp52=({struct Cyc_Absyn_Tvar*_tmp57=_cycalloc(sizeof(*_tmp57));({struct _fat_ptr*_tmp17C=({struct _fat_ptr*_tmp56=_cycalloc(sizeof(*_tmp56));({struct _fat_ptr _tmp17B=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp138;_tmp138.tag=0U,_tmp138.f1=(struct _fat_ptr)((struct _fat_ptr)*l);_tmp138;});void*_tmp53[1U];_tmp53[0]=& _tmp55;({struct _fat_ptr _tmp17A=({const char*_tmp54="`%s";_tag_fat(_tmp54,sizeof(char),4U);});Cyc_aprintf(_tmp17A,_tag_fat(_tmp53,sizeof(void*),1U));});});*_tmp56=_tmp17B;});_tmp56;});_tmp57->name=_tmp17C;}),({
int _tmp179=Cyc_Tcutil_new_tvar_id();_tmp57->identity=_tmp179;}),({void*_tmp178=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp57->kind=_tmp178;});_tmp57;});
# 180
struct Cyc_Absyn_Tvar*tv=_tmp52;
# 182
({struct Cyc_Tcenv_Tenv*_tmp17D=Cyc_Tcenv_new_named_block(s0->loc,te,tv);Cyc_Tcstmt_tcStmt(_tmp17D,s,0);});
return;}case 11U: _LL19: _tmp28=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp29=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2;_LL1A: {struct Cyc_List_List*es=_tmp28;struct Cyc_Absyn_Switch_clause***clauseopt=_tmp29;
# 186
const struct _tuple14*_tmp58=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);const struct _tuple14*trip_opt=_tmp58;
if(trip_opt == (const struct _tuple14*)0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp139;_tmp139.tag=0U,({struct _fat_ptr _tmp17E=({const char*_tmp5B="fallthru not in a non-last case";_tag_fat(_tmp5B,sizeof(char),32U);});_tmp139.f1=_tmp17E;});_tmp139;});void*_tmp59[1U];_tmp59[0]=& _tmp5A;({unsigned _tmp17F=s0->loc;Cyc_Warn_err2(_tmp17F,_tag_fat(_tmp59,sizeof(void*),1U));});});
return;}{
# 191
struct _tuple14 _tmp5C=*trip_opt;struct _tuple14 _stmttmp4=_tmp5C;struct _tuple14 _tmp5D=_stmttmp4;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*_tmp5E;_LL22: _tmp5E=_tmp5D.f2;_tmp5F=_tmp5D.f3;_LL23: {struct Cyc_List_List*tvs=_tmp5E;struct Cyc_List_List*ts_orig=_tmp5F;
struct Cyc_List_List*_tmp60=({struct Cyc_List_List*_tmp180=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp180,tvs);});struct Cyc_List_List*instantiation=_tmp60;
struct Cyc_List_List*_tmp61=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,ts_orig);struct Cyc_List_List*ts=_tmp61;
for(0;ts != 0 && es != 0;(ts=ts->tl,es=es->tl)){
# 196
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp182=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp181=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp182,_tmp181,(void*)ts->hd,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp63=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13D;_tmp13D.tag=0U,({struct _fat_ptr _tmp183=({const char*_tmp68="fallthru argument has type ";_tag_fat(_tmp68,sizeof(char),28U);});_tmp13D.f1=_tmp183;});_tmp13D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp64=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp13C;_tmp13C.tag=3U,_tmp13C.f1=(void*)((struct Cyc_Absyn_Exp*)es->hd)->topt;_tmp13C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp65=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13B;_tmp13B.tag=0U,({
struct _fat_ptr _tmp184=({const char*_tmp67="but pattern variable has type ";_tag_fat(_tmp67,sizeof(char),31U);});_tmp13B.f1=_tmp184;});_tmp13B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp66=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp13A;_tmp13A.tag=2U,_tmp13A.f1=(void*)ts->hd;_tmp13A;});void*_tmp62[4U];_tmp62[0]=& _tmp63,_tmp62[1]=& _tmp64,_tmp62[2]=& _tmp65,_tmp62[3]=& _tmp66;({unsigned _tmp185=s0->loc;Cyc_Warn_err2(_tmp185,_tag_fat(_tmp62,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}
# 203
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)ts->hd,(struct Cyc_Absyn_Exp*)es->hd);}
# 205
if(es != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13E;_tmp13E.tag=0U,({struct _fat_ptr _tmp186=({const char*_tmp6B="too many arguments in explicit fallthru";_tag_fat(_tmp6B,sizeof(char),40U);});_tmp13E.f1=_tmp186;});_tmp13E;});void*_tmp69[1U];_tmp69[0]=& _tmp6A;({unsigned _tmp187=s0->loc;Cyc_Warn_err2(_tmp187,_tag_fat(_tmp69,sizeof(void*),1U));});});
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp13F;_tmp13F.tag=0U,({struct _fat_ptr _tmp188=({const char*_tmp6E="too few arguments in explicit fallthru";_tag_fat(_tmp6E,sizeof(char),39U);});_tmp13F.f1=_tmp188;});_tmp13F;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6D;({unsigned _tmp189=s0->loc;Cyc_Warn_err2(_tmp189,_tag_fat(_tmp6C,sizeof(void*),1U));});});
return;}}}case 10U: _LL1B: _tmp25=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp26=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2;_tmp27=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1F)->f3;_LL1C: {struct Cyc_Absyn_Exp**exp=_tmp25;struct Cyc_List_List*scs0=_tmp26;void**dtp=_tmp27;
# 214
struct Cyc_Absyn_Exp*_tmp6F=*exp;struct Cyc_Absyn_Exp*e=_tmp6F;
if(!Cyc_Tcstmt_is_var_exp(e)){
struct Cyc_Absyn_Vardecl*_tmp70=({struct _tuple2*_tmp18A=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp18A,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp70;
struct Cyc_Absyn_Stmt*_tmp71=Cyc_Absyn_new_stmt(s0->r,s0->loc);struct Cyc_Absyn_Stmt*s1=_tmp71;
struct Cyc_Absyn_Decl*_tmp72=({struct Cyc_Absyn_Pat*_tmp18E=({void*_tmp18C=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=1U,_tmp75->f1=vd,({struct Cyc_Absyn_Pat*_tmp18B=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp75->f2=_tmp18B;});_tmp75;});Cyc_Absyn_new_pat(_tmp18C,e->loc);});struct Cyc_Absyn_Exp*_tmp18D=e;Cyc_Absyn_let_decl(_tmp18E,_tmp18D,s0->loc);});struct Cyc_Absyn_Decl*d=_tmp72;
# 221
struct Cyc_Absyn_Stmt*_tmp73=Cyc_Absyn_decl_stmt(d,s1,s0->loc);struct Cyc_Absyn_Stmt*s2=_tmp73;
s0->r=s2->r;
({struct Cyc_Absyn_Exp*_tmp190=({void*_tmp18F=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=4U,_tmp74->f1=vd;_tmp74;});Cyc_Absyn_varb_exp(_tmp18F,e->loc);});*exp=_tmp190;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 228
void*_tmp76=Cyc_Tcexp_tcExp(te,0,e);void*t=_tmp76;
# 230
Cyc_Tcexp_check_consume(e->loc,t,e);{
# 234
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp77=scs0;struct Cyc_List_List*scs=_tmp77;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& t,e);}else{
# 242
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 244
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp78=_cycalloc(sizeof(*_tmp78));({struct Cyc_Tcpat_TcPatResult _tmp191=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& t,e);*_tmp78=_tmp191;});_tmp78;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 248
te=Cyc_Tcenv_clear_fallthru(te);else{
# 250
struct Cyc_List_List*_tmp79=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp79;
struct Cyc_List_List*_tmp7A=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 251
struct Cyc_List_List*new_tvs=_tmp7A;
# 253
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 256
te=Cyc_Tcenv_clear_fallthru(te);}{
# 258
struct Cyc_Absyn_Pat*_tmp7B=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmp7B;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp143;_tmp143.tag=0U,({
struct _fat_ptr _tmp192=({const char*_tmp82="switch on type ";_tag_fat(_tmp82,sizeof(char),16U);});_tmp143.f1=_tmp192;});_tmp143;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp142;_tmp142.tag=2U,_tmp142.f1=(void*)t;_tmp142;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp141;_tmp141.tag=0U,({struct _fat_ptr _tmp193=({const char*_tmp81=" but case expects type ";_tag_fat(_tmp81,sizeof(char),24U);});_tmp141.f1=_tmp193;});_tmp141;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp80=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp140;_tmp140.tag=3U,_tmp140.f1=(void*)p->topt;_tmp140;});void*_tmp7C[4U];_tmp7C[0]=& _tmp7D,_tmp7C[1]=& _tmp7E,_tmp7C[2]=& _tmp7F,_tmp7C[3]=& _tmp80;({unsigned _tmp194=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp194,_tag_fat(_tmp7C,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}else{
# 265
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 267
({struct Cyc_Core_Opt*_tmp195=({struct Cyc_Core_Opt*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->v=this_pattern.patvars;_tmp83;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp195;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 271
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,scs0,dtp);
return;}}}case 15U: _LL1D: _tmp22=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp23=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2;_tmp24=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1F)->f3;_LL1E: {struct Cyc_Absyn_Stmt*s=_tmp22;struct Cyc_List_List**scs0=_tmp23;void**dtp=_tmp24;
# 279
({void*_tmp199=({void*_tmp198=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=2U,({struct Cyc_Absyn_Stmt*_tmp197=Cyc_Absyn_new_stmt(s->r,s->loc);_tmp84->f1=_tmp197;}),({struct Cyc_Absyn_Stmt*_tmp196=Cyc_Absyn_skip_stmt(s->loc);_tmp84->f2=_tmp196;});_tmp84;});Cyc_Absyn_new_stmt(_tmp198,s->loc);})->r;s->r=_tmp199;});
# 281
Cyc_Tcstmt_tcStmt(te,s,1);
# 283
if(!((int(*)(int(*pred)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Tcstmt_is_case_default,*scs0)){
# 285
struct _tuple2*def_v=({struct _tuple2*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp19B=({struct _fat_ptr*_tmp8F=_cycalloc(sizeof(*_tmp8F));({struct _fat_ptr _tmp19A=({const char*_tmp8E="exn";_tag_fat(_tmp8E,sizeof(char),4U);});*_tmp8F=_tmp19A;});_tmp8F;});_tmp90->f2=_tmp19B;});_tmp90;});
struct Cyc_Absyn_Vardecl*_tmp85=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmp85;
struct Cyc_Absyn_Pat*_tmp86=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=1U,_tmp8D->f1=vd,({struct Cyc_Absyn_Pat*_tmp19C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp8D->f2=_tmp19C;});_tmp8D;}),0U);struct Cyc_Absyn_Pat*def_pat=_tmp86;
struct Cyc_Absyn_Stmt*_tmp87=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=11U,({struct Cyc_Absyn_Exp*_tmp19D=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=4U,_tmp8B->f1=vd;_tmp8B;}),0U);_tmp8C->f1=_tmp19D;}),_tmp8C->f2=1;_tmp8C;}),0U),0U);
# 288
struct Cyc_Absyn_Stmt*def_stmt=_tmp87;
# 290
struct Cyc_Absyn_Switch_clause*_tmp88=({struct Cyc_Absyn_Switch_clause*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->pattern=def_pat,_tmp8A->pat_vars=0,_tmp8A->where_clause=0,_tmp8A->body=def_stmt,_tmp8A->loc=0U;_tmp8A;});struct Cyc_Absyn_Switch_clause*def_clause=_tmp88;
({struct Cyc_List_List*_tmp19F=({struct Cyc_List_List*_tmp19E=*scs0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp19E,({struct Cyc_Absyn_Switch_clause*_tmp89[1U];_tmp89[0]=def_clause;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp89,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*scs0=_tmp19F;});}{
# 297
void*_tmp91=Cyc_Absyn_exn_type();void*exception_type=_tmp91;
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp92=*scs0;struct Cyc_List_List*scs=_tmp92;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& exception_type,0);}else{
# 306
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 308
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp93=_cycalloc(sizeof(*_tmp93));({struct Cyc_Tcpat_TcPatResult _tmp1A0=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& exception_type,0);*_tmp93=_tmp1A0;});_tmp93;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 312
te=Cyc_Tcenv_clear_fallthru(te);else{
# 314
struct Cyc_List_List*_tmp94=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp94;
struct Cyc_List_List*_tmp95=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 315
struct Cyc_List_List*new_tvs=_tmp95;
# 317
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 320
te=Cyc_Tcenv_clear_fallthru(te);}{
# 322
struct Cyc_Absyn_Pat*_tmp96=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmp96;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),exception_type)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp98=({struct Cyc_Warn_String_Warn_Warg_struct _tmp147;_tmp147.tag=0U,({struct _fat_ptr _tmp1A1=({const char*_tmp9D="switch on type ";_tag_fat(_tmp9D,sizeof(char),16U);});_tmp147.f1=_tmp1A1;});_tmp147;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp99=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp146;_tmp146.tag=2U,_tmp146.f1=(void*)exception_type;_tmp146;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp145;_tmp145.tag=0U,({
struct _fat_ptr _tmp1A2=({const char*_tmp9C=" but case expects type ";_tag_fat(_tmp9C,sizeof(char),24U);});_tmp145.f1=_tmp1A2;});_tmp145;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp144;_tmp144.tag=3U,_tmp144.f1=(void*)p->topt;_tmp144;});void*_tmp97[4U];_tmp97[0]=& _tmp98,_tmp97[1]=& _tmp99,_tmp97[2]=& _tmp9A,_tmp97[3]=& _tmp9B;({unsigned _tmp1A3=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp1A3,_tag_fat(_tmp97,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}else{
# 329
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 331
({struct Cyc_Core_Opt*_tmp1A4=({struct Cyc_Core_Opt*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->v=this_pattern.patvars;_tmp9E;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp1A4;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 335
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*scs0,dtp);
return;}}default: _LL1F: _tmp20=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1F)->f1;_tmp21=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1F)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp20;struct Cyc_Absyn_Stmt*s=_tmp21;
# 339
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmp9F=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;struct Cyc_Tcenv_Tenv*te=_tmp9F;
{void*_tmpA0=d->r;void*_stmttmp5=_tmpA0;void*_tmpA1=_stmttmp5;struct Cyc_List_List*_tmpA3;struct _tuple2*_tmpA2;struct Cyc_List_List*_tmpA5;struct _fat_ptr*_tmpA4;struct Cyc_Absyn_Fndecl*_tmpA6;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Vardecl*_tmpA8;struct Cyc_Absyn_Tvar*_tmpA7;struct Cyc_List_List*_tmpAA;void**_tmpAE;struct Cyc_Absyn_Exp**_tmpAD;struct Cyc_Core_Opt**_tmpAC;struct Cyc_Absyn_Pat*_tmpAB;struct Cyc_Absyn_Vardecl*_tmpAF;switch(*((int*)_tmpA1)){case 0U: _LL25: _tmpAF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL26: {struct Cyc_Absyn_Vardecl*vd=_tmpAF;
# 343
struct Cyc_Absyn_Vardecl*_tmpB0=vd;void**_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;void*_tmpB3;struct Cyc_Absyn_Tqual _tmpB2;enum Cyc_Absyn_Scope _tmpB1;_LL48: _tmpB1=_tmpB0->sc;_tmpB2=_tmpB0->tq;_tmpB3=_tmpB0->type;_tmpB4=_tmpB0->initializer;_tmpB5=(void**)& _tmpB0->rgn;_LL49: {enum Cyc_Absyn_Scope sc=_tmpB1;struct Cyc_Absyn_Tqual tq=_tmpB2;void*t=_tmpB3;struct Cyc_Absyn_Exp*initializer=_tmpB4;void**rgn_ptr=_tmpB5;
void*_tmpB6=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpB6;
int is_local;
{enum Cyc_Absyn_Scope _tmpB7=sc;switch(_tmpB7){case Cyc_Absyn_Static: _LL4B: _LL4C:
 goto _LL4E;case Cyc_Absyn_Extern: _LL4D: _LL4E:
 goto _LL50;case Cyc_Absyn_ExternC: _LL4F: _LL50:
 vd->escapes=1;is_local=0;goto _LL4A;case Cyc_Absyn_Abstract: _LL51: _LL52:
# 351
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp148;_tmp148.tag=0U,({struct _fat_ptr _tmp1A5=({const char*_tmpBA="bad abstract scope for local variable";_tag_fat(_tmpBA,sizeof(char),38U);});_tmp148.f1=_tmp1A5;});_tmp148;});void*_tmpB8[1U];_tmpB8[0]=& _tmpB9;({unsigned _tmp1A6=d->loc;Cyc_Warn_err2(_tmp1A6,_tag_fat(_tmpB8,sizeof(void*),1U));});});
goto _LL54;case Cyc_Absyn_Register: _LL53: _LL54:
 goto _LL56;case Cyc_Absyn_Public: _LL55: _LL56:
 goto _LL58;default: _LL57: _LL58:
 is_local=1;goto _LL4A;}_LL4A:;}
# 358
*rgn_ptr=is_local?curr_bl: Cyc_Absyn_heap_rgn_type;
{void*_tmpBB=Cyc_Absyn_compress(t);void*_stmttmp6=_tmpBB;void*_tmpBC=_stmttmp6;struct Cyc_Absyn_ArrayInfo _tmpBD;switch(*((int*)_tmpBC)){case 5U: _LL5A: if(is_local){_LL5B:
# 362
 vd->escapes=1;
sc=3U;
is_local=0;
goto _LL59;}else{goto _LL5E;}case 4U: _LL5C: _tmpBD=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBC)->f1;_LL5D: {struct Cyc_Absyn_ArrayInfo ai=_tmpBD;
# 368
if(vd->initializer == 0 || ai.num_elts != 0)
goto _LL59;{
struct Cyc_Absyn_Exp*len;
{void*_tmpBE=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmp7=_tmpBE;void*_tmpBF=_stmttmp7;struct Cyc_List_List*_tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC3;struct _fat_ptr _tmpC4;struct _fat_ptr _tmpC5;switch(*((int*)_tmpBF)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBF)->f1).Wstring_c).tag){case 8U: _LL61: _tmpC5=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBF)->f1).String_c).val;_LL62: {struct _fat_ptr s=_tmpC5;
len=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);goto _LL60;}case 9U: _LL63: _tmpC4=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBF)->f1).Wstring_c).val;_LL64: {struct _fat_ptr s=_tmpC4;
len=Cyc_Absyn_uint_exp(1U,0U);goto _LL60;}default: goto _LL6D;}case 27U: _LL65: _tmpC3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBF)->f2;_LL66: {struct Cyc_Absyn_Exp*e=_tmpC3;
_tmpC2=e;goto _LL68;}case 28U: _LL67: _tmpC2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBF)->f1;_LL68: {struct Cyc_Absyn_Exp*e=_tmpC2;
len=e;goto _LL60;}case 36U: _LL69: _tmpC1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBF)->f2;_LL6A: {struct Cyc_List_List*es=_tmpC1;
_tmpC0=es;goto _LL6C;}case 26U: _LL6B: _tmpC0=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpBF)->f1;_LL6C: {struct Cyc_List_List*es=_tmpC0;
len=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);goto _LL60;}default: _LL6D: _LL6E:
 len=0;goto _LL60;}_LL60:;}
# 380
ai.num_elts=len;
t=({void*_tmp1A7=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=4U,_tmpC6->f1=ai;_tmpC6;});vd->type=_tmp1A7;});
goto _LL59;}}default: _LL5E: _LL5F:
 goto _LL59;}_LL59:;}{
# 388
struct Cyc_List_List*_tmpC7=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*bound_vars=_tmpC7;
int _tmpC8=is_local;int allow_evars=_tmpC8;
Cyc_Tctyp_check_type(s0->loc,te,bound_vars,& Cyc_Tcutil_tmk,allow_evars,1,t);
({int _tmp1A8=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp1A8;});
# 393
if((int)sc == (int)3U ||(int)sc == (int)4U)
({void*_tmpC9=0U;({unsigned _tmp1AA=d->loc;struct _fat_ptr _tmp1A9=({const char*_tmpCA="extern declarations are not yet supported within functions";_tag_fat(_tmpCA,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1AA,_tmp1A9,_tag_fat(_tmpC9,sizeof(void*),0U));});});
if(initializer != 0){
Cyc_Tcexp_tcExpInitializer(te,& t,initializer);
# 398
if(!is_local && !Cyc_Tcutil_is_const_exp(initializer))
({void*_tmpCB=0U;({unsigned _tmp1AC=d->loc;struct _fat_ptr _tmp1AB=({const char*_tmpCC="initializer for static variable needs to be a constant expression";_tag_fat(_tmpCC,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1AC,_tmp1AB,_tag_fat(_tmpCB,sizeof(void*),0U));});});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp1AE=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1AD=initializer;Cyc_Tcutil_coerce_assign(_tmp1AE,_tmp1AD,t);})){
({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp14D;_tmp14D.tag=11U,_tmp14D.f1=vd;_tmp14D;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14C;_tmp14C.tag=0U,({struct _fat_ptr _tmp1AF=({const char*_tmpD4=" was declared with type ";_tag_fat(_tmpD4,sizeof(char),25U);});_tmp14C.f1=_tmp1AF;});_tmp14C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14B;_tmp14B.tag=2U,_tmp14B.f1=(void*)t;_tmp14B;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14A;_tmp14A.tag=0U,({
struct _fat_ptr _tmp1B0=({const char*_tmpD3=" but initialized with type ";_tag_fat(_tmpD3,sizeof(char),28U);});_tmp14A.f1=_tmp1B0;});_tmp14A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmpD2=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp149;_tmp149.tag=3U,_tmp149.f1=(void*)initializer->topt;_tmp149;});void*_tmpCD[5U];_tmpCD[0]=& _tmpCE,_tmpCD[1]=& _tmpCF,_tmpCD[2]=& _tmpD0,_tmpCD[3]=& _tmpD1,_tmpCD[4]=& _tmpD2;({unsigned _tmp1B1=d->loc;Cyc_Warn_err2(_tmp1B1,_tag_fat(_tmpCD,sizeof(void*),5U));});});
Cyc_Unify_unify(t,(void*)_check_null(initializer->topt));
Cyc_Unify_explain_failure();}}
# 407
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}case 2U: _LL27: _tmpAB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpAC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_tmpAD=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f3;_tmpAE=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f4;_LL28: {struct Cyc_Absyn_Pat*p=_tmpAB;struct Cyc_Core_Opt**vds=_tmpAC;struct Cyc_Absyn_Exp**exp=_tmpAD;void**dtp=_tmpAE;
# 414
struct Cyc_Absyn_Exp*_tmpD5=*exp;struct Cyc_Absyn_Exp*e=_tmpD5;
{void*_tmpD6=p->r;void*_stmttmp8=_tmpD6;void*_tmpD7=_stmttmp8;switch(*((int*)_tmpD7)){case 1U: _LL70: _LL71:
 goto _LL73;case 2U: _LL72: _LL73:
 goto _LL75;case 15U: _LL74: _LL75:
 goto _LL6F;default: _LL76: _LL77:
# 420
 if(!Cyc_Tcstmt_is_var_exp(e)){
struct Cyc_Absyn_Vardecl*_tmpD8=({struct _tuple2*_tmp1B2=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1B2,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmpD8;
struct Cyc_Absyn_Decl*_tmpD9=({struct Cyc_Absyn_Pat*_tmp1B6=({void*_tmp1B4=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->tag=1U,_tmpDB->f1=vd,({struct Cyc_Absyn_Pat*_tmp1B3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmpDB->f2=_tmp1B3;});_tmpDB;});Cyc_Absyn_new_pat(_tmp1B4,e->loc);});struct Cyc_Absyn_Exp*_tmp1B5=e;Cyc_Absyn_let_decl(_tmp1B6,_tmp1B5,s0->loc);});struct Cyc_Absyn_Decl*d=_tmpD9;
# 425
({void*_tmp1B9=({struct Cyc_Absyn_Decl*_tmp1B8=d;struct Cyc_Absyn_Stmt*_tmp1B7=Cyc_Absyn_new_stmt(s0->r,s0->loc);Cyc_Absyn_decl_stmt(_tmp1B8,_tmp1B7,s0->loc);})->r;s0->r=_tmp1B9;});
({struct Cyc_Absyn_Exp*_tmp1BB=({void*_tmp1BA=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->tag=4U,_tmpDA->f1=vd;_tmpDA;});Cyc_Absyn_varb_exp(_tmp1BA,e->loc);});*exp=_tmp1BB;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}}_LL6F:;}
# 431
Cyc_Tcexp_tcExpInitializer(te,0,e);{
# 433
void*pat_type=(void*)_check_null(e->topt);
# 435
struct Cyc_Tcpat_TcPatResult _tmpDC=Cyc_Tcpat_tcPat(te,p,& pat_type,e);struct Cyc_Tcpat_TcPatResult pat_res=_tmpDC;
({struct Cyc_Core_Opt*_tmp1BC=({struct Cyc_Core_Opt*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->v=pat_res.patvars;_tmpDD;});*vds=_tmp1BC;});{
void*_tmpDE=(void*)_check_null(p->topt);void*pt=_tmpDE;
void*_tmpDF=(void*)_check_null(e->topt);void*et=_tmpDF;
if(!Cyc_Unify_unify(pt,et)&& !({struct Cyc_RgnOrder_RgnPO*_tmp1BE=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1BD=e;Cyc_Tcutil_coerce_assign(_tmp1BE,_tmp1BD,pt);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp151;_tmp151.tag=0U,({
struct _fat_ptr _tmp1BF=({const char*_tmpE6="pattern type ";_tag_fat(_tmpE6,sizeof(char),14U);});_tmp151.f1=_tmp1BF;});_tmp151;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp150;_tmp150.tag=2U,_tmp150.f1=(void*)pt;_tmp150;});struct Cyc_Warn_String_Warn_Warg_struct _tmpE3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14F;_tmp14F.tag=0U,({struct _fat_ptr _tmp1C0=({const char*_tmpE5=" does not match definition type ";_tag_fat(_tmpE5,sizeof(char),33U);});_tmp14F.f1=_tmp1C0;});_tmp14F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14E;_tmp14E.tag=2U,_tmp14E.f1=(void*)et;_tmp14E;});void*_tmpE0[4U];_tmpE0[0]=& _tmpE1,_tmpE0[1]=& _tmpE2,_tmpE0[2]=& _tmpE3,_tmpE0[3]=& _tmpE4;({unsigned _tmp1C1=d->loc;Cyc_Warn_err2(_tmp1C1,_tag_fat(_tmpE0,sizeof(void*),4U));});});
Cyc_Unify_unify(pt,et);
Cyc_Unify_explain_failure();}else{
# 446
Cyc_Tcpat_check_pat_regions(te,p,pat_res.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(p->loc,te,p,dtp);
Cyc_Tcstmt_pattern_synth(s0->loc,te,pat_res,s,0,0);
return;}}}case 3U: _LL29: _tmpAA=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL2A: {struct Cyc_List_List*vds=_tmpAA;
# 452
void*_tmpE7=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpE7;
for(0;vds != 0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=curr_bl;
({unsigned _tmp1C4=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1C3=te;struct Cyc_List_List*_tmp1C2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1C4,_tmp1C3,_tmp1C2,& Cyc_Tcutil_tmk,1,1,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);});}
# 457
Cyc_Tcstmt_tcStmt(te,s,0);
return;}case 4U: _LL2B: _tmpA7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpA8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_tmpA9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f3;_LL2C: {struct Cyc_Absyn_Tvar*tv=_tmpA7;struct Cyc_Absyn_Vardecl*vd=_tmpA8;struct Cyc_Absyn_Exp*open_exp_opt=_tmpA9;
# 463
({void*_tmp1C8=({void*_tmp1C7=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1C6=Cyc_Absyn_new_stmt(s->r,s->loc);_tmpE8->f1=_tmp1C6;}),({struct Cyc_Absyn_Stmt*_tmp1C5=Cyc_Absyn_skip_stmt(s->loc);_tmpE8->f2=_tmp1C5;});_tmpE8;});Cyc_Absyn_new_stmt(_tmp1C7,s->loc);})->r;s->r=_tmp1C8;});{
# 465
struct Cyc_Tcenv_Tenv*_tmpE9=te;struct Cyc_Tcenv_Tenv*te2=_tmpE9;
struct Cyc_Absyn_Vardecl*_tmpEA=vd;void**_tmpEC;void**_tmpEB;_LL79: _tmpEB=(void**)& _tmpEA->type;_tmpEC=(void**)& _tmpEA->rgn;_LL7A: {void**t=_tmpEB;void**rgn_ptr=_tmpEC;
void*_tmpED=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpED;
*rgn_ptr=curr_bl;{
void*rgn_typ;
if((unsigned)open_exp_opt){
# 473
struct _tuple2*drname=({struct _tuple2*_tmp103=_cycalloc(sizeof(*_tmp103));((_tmp103->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp1CD=({struct _fat_ptr*_tmpFE[1U];({struct _fat_ptr*_tmp1CC=({struct _fat_ptr*_tmp100=_cycalloc(sizeof(*_tmp100));({struct _fat_ptr _tmp1CB=({const char*_tmpFF="Core";_tag_fat(_tmpFF,sizeof(char),5U);});*_tmp100=_tmp1CB;});_tmp100;});_tmpFE[0]=_tmp1CC;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpFE,sizeof(struct _fat_ptr*),1U));});((_tmp103->f1).Abs_n).val=_tmp1CD;}),({
struct _fat_ptr*_tmp1CA=({struct _fat_ptr*_tmp102=_cycalloc(sizeof(*_tmp102));({struct _fat_ptr _tmp1C9=({const char*_tmp101="DynamicRegion";_tag_fat(_tmp101,sizeof(char),14U);});*_tmp102=_tmp1C9;});_tmp102;});_tmp103->f2=_tmp1CA;});_tmp103;});
void*_tmpEE=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->v=& Cyc_Tcutil_trk;_tmpFD;}),0);void*outer_rgn=_tmpEE;
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->v=& Cyc_Tcutil_rk;_tmpEF;}),0);{
void*_tmpF0=({
union Cyc_Absyn_AggrInfo _tmp1CE=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);Cyc_Absyn_aggr_type(_tmp1CE,({void*_tmpFC[1U];_tmpFC[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpFC,sizeof(void*),1U));}));});
# 477
void*dr_type=_tmpF0;
# 479
void*_tmpF1=({void*_tmp1D1=dr_type;void*_tmp1D0=outer_rgn;struct Cyc_Absyn_Tqual _tmp1CF=
Cyc_Absyn_empty_tqual(0U);
# 479
Cyc_Absyn_at_type(_tmp1D1,_tmp1D0,_tmp1CF,Cyc_Absyn_false_type);});void*exp_type=_tmpF1;
# 481
({unsigned _tmp1D4=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1D3=te;struct Cyc_List_List*_tmp1D2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1D4,_tmp1D3,_tmp1D2,& Cyc_Tcutil_tmk,1,0,exp_type);});{
void*_tmpF2=Cyc_Tcexp_tcExp(te,& exp_type,open_exp_opt);void*key_typ=_tmpF2;
if(!Cyc_Unify_unify(exp_type,key_typ)&& !({
struct Cyc_RgnOrder_RgnPO*_tmp1D6=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1D5=open_exp_opt;Cyc_Tcutil_coerce_assign(_tmp1D6,_tmp1D5,exp_type);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp155;_tmp155.tag=0U,({struct _fat_ptr _tmp1D7=({const char*_tmpF9="expected ";_tag_fat(_tmpF9,sizeof(char),10U);});_tmp155.f1=_tmp1D7;});_tmp155;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp154;_tmp154.tag=2U,_tmp154.f1=(void*)exp_type;_tmp154;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp153;_tmp153.tag=0U,({struct _fat_ptr _tmp1D8=({const char*_tmpF8=" but found ";_tag_fat(_tmpF8,sizeof(char),12U);});_tmp153.f1=_tmp1D8;});_tmp153;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp152;_tmp152.tag=2U,_tmp152.f1=(void*)key_typ;_tmp152;});void*_tmpF3[4U];_tmpF3[0]=& _tmpF4,_tmpF3[1]=& _tmpF5,_tmpF3[2]=& _tmpF6,_tmpF3[3]=& _tmpF7;({unsigned _tmp1D9=s0->loc;Cyc_Warn_err2(_tmp1D9,_tag_fat(_tmpF3,sizeof(void*),4U));});});
if(!Cyc_Unify_unify(outer_rgn,Cyc_Absyn_unique_rgn_type)&& !
Cyc_Unify_unify(outer_rgn,Cyc_Absyn_refcnt_rgn_type))
({void*_tmpFA=0U;({unsigned _tmp1DB=s0->loc;struct _fat_ptr _tmp1DA=({const char*_tmpFB="open is only allowed on unique or reference-counted keys";_tag_fat(_tmpFB,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp1DB,_tmp1DA,_tag_fat(_tmpFA,sizeof(void*),0U));});});
({void*_tmp1DC=Cyc_Absyn_rgn_handle_type(rgn_typ);*t=_tmp1DC;});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1,0);}}}else{
# 492
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=2U,_tmp104->f1=tv;_tmp104;});
te2=({unsigned _tmp1DE=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1DD=te2;Cyc_Tcenv_add_type_vars(_tmp1DE,_tmp1DD,({struct Cyc_List_List*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->hd=tv,_tmp105->tl=0;_tmp105;}));});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0,1);}
# 496
({unsigned _tmp1E1=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1E0=te;struct Cyc_List_List*_tmp1DF=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp1E1,_tmp1E0,_tmp1DF,& Cyc_Tcutil_bk,1,0,*t);});
if(!({void*_tmp1E2=*t;Cyc_Unify_unify(_tmp1E2,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({void*_tmp106=0U;({struct _fat_ptr _tmp1E3=({const char*_tmp107="region stmt: type of region handle is wrong!";_tag_fat(_tmp107,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp1E3,_tag_fat(_tmp106,sizeof(void*),0U));});});
# 500
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1U: _LL2D: _tmpA6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL2E: {struct Cyc_Absyn_Fndecl*fd=_tmpA6;
# 505
if((int)fd->sc != (int)Cyc_Absyn_Public)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp109=({struct Cyc_Warn_String_Warn_Warg_struct _tmp156;_tmp156.tag=0U,({struct _fat_ptr _tmp1E4=({const char*_tmp10A="bad storage class for inner function";_tag_fat(_tmp10A,sizeof(char),37U);});_tmp156.f1=_tmp1E4;});_tmp156;});void*_tmp108[1U];_tmp108[0]=& _tmp109;({unsigned _tmp1E5=d->loc;Cyc_Warn_err2(_tmp1E5,_tag_fat(_tmp108,sizeof(void*),1U));});});
# 508
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;(unsigned)atts;atts=atts->tl){
void*_tmp10B=(void*)atts->hd;void*_stmttmp9=_tmp10B;void*_tmp10C=_stmttmp9;switch(*((int*)_tmp10C)){case 7U: _LL7C: _LL7D:
 goto _LL7F;case 6U: _LL7E: _LL7F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp159;_tmp159.tag=0U,({struct _fat_ptr _tmp1E6=({const char*_tmp112="bad attribute ";_tag_fat(_tmp112,sizeof(char),15U);});_tmp159.f1=_tmp1E6;});_tmp159;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp158;_tmp158.tag=10U,_tmp158.f1=(void*)atts->hd;_tmp158;});struct Cyc_Warn_String_Warn_Warg_struct _tmp110=({struct Cyc_Warn_String_Warn_Warg_struct _tmp157;_tmp157.tag=0U,({struct _fat_ptr _tmp1E7=({const char*_tmp111=" for function";_tag_fat(_tmp111,sizeof(char),14U);});_tmp157.f1=_tmp1E7;});_tmp157;});void*_tmp10D[3U];_tmp10D[0]=& _tmp10E,_tmp10D[1]=& _tmp10F,_tmp10D[2]=& _tmp110;({unsigned _tmp1E8=d->loc;Cyc_Warn_err2(_tmp1E8,_tag_fat(_tmp10D,sizeof(void*),3U));});});
goto _LL7B;default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}}
# 515
Cyc_Tctyp_check_fndecl_valid_type(d->loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
({struct Cyc_List_List*_tmp1E9=Cyc_Atts_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp1E9;});{
void*_tmp113=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp113;
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->sc=fd->sc,_tmp114->name=fd->name,_tmp114->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1EE=Cyc_Absyn_const_tqual(0U);_tmp114->tq=_tmp1EE;}),({
void*_tmp1ED=({void*_tmp1EC=t;void*_tmp1EB=curr_bl;struct Cyc_Absyn_Tqual _tmp1EA=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp1EC,_tmp1EB,_tmp1EA,Cyc_Absyn_false_type);});_tmp114->type=_tmp1ED;}),_tmp114->initializer=0,_tmp114->rgn=curr_bl,_tmp114->attributes=0,_tmp114->escapes=0,_tmp114->is_proto=fd->body != 0;_tmp114;});
# 524
fd->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(te->le);
({struct Cyc_Tcenv_Fenv*_tmp1EF=Cyc_Tcenv_nested_fenv(d->loc,old_fenv,fd);te->le=_tmp1EF;});
Cyc_Tcstmt_tcStmt(te,fd->body,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
te->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9U: _LL2F: _tmpA4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpA5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_LL30: {struct _fat_ptr*n=_tmpA4;struct Cyc_List_List*tds=_tmpA5;
unimp_msg_part=({const char*_tmp115="namespace";_tag_fat(_tmp115,sizeof(char),10U);});goto _LL24;}case 10U: _LL31: _tmpA2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpA3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_LL32: {struct _tuple2*q=_tmpA2;struct Cyc_List_List*tds=_tmpA3;
unimp_msg_part=({const char*_tmp116="using";_tag_fat(_tmp116,sizeof(char),6U);});goto _LL24;}case 5U: _LL33: _LL34:
 unimp_msg_part=({const char*_tmp117="type";_tag_fat(_tmp117,sizeof(char),5U);});goto _LL24;case 6U: _LL35: _LL36:
 unimp_msg_part=({const char*_tmp118="datatype";_tag_fat(_tmp118,sizeof(char),9U);});goto _LL24;case 7U: _LL37: _LL38:
 unimp_msg_part=({const char*_tmp119="enum";_tag_fat(_tmp119,sizeof(char),5U);});goto _LL24;case 8U: _LL39: _LL3A:
 unimp_msg_part=({const char*_tmp11A="typedef";_tag_fat(_tmp11A,sizeof(char),8U);});goto _LL24;case 11U: _LL3B: _LL3C:
 unimp_msg_part=({const char*_tmp11B="extern \"C\"";_tag_fat(_tmp11B,sizeof(char),11U);});goto _LL24;case 12U: _LL3D: _LL3E:
 unimp_msg_part=({const char*_tmp11C="extern \"C include\"";_tag_fat(_tmp11C,sizeof(char),19U);});goto _LL24;case 13U: _LL3F: _LL40:
 unimp_msg_part=({const char*_tmp11D="__cyclone_port_on__";_tag_fat(_tmp11D,sizeof(char),20U);});goto _LL24;case 14U: _LL41: _LL42:
 unimp_msg_part=({const char*_tmp11E="__cyclone_port_off__";_tag_fat(_tmp11E,sizeof(char),21U);});goto _LL24;case 15U: _LL43: _LL44:
 unimp_msg_part=({const char*_tmp11F="__tempest_on__";_tag_fat(_tmp11F,sizeof(char),15U);});goto _LL24;default: _LL45: _LL46:
 unimp_msg_part=({const char*_tmp120="__tempest_off__";_tag_fat(_tmp120,sizeof(char),16U);});goto _LL24;}_LL24:;}
# 546
({struct Cyc_Warn_String_Warn_Warg_struct _tmp122=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15C;_tmp15C.tag=0U,({struct _fat_ptr _tmp1F0=({const char*_tmp126="tcStmt: nested ";_tag_fat(_tmp126,sizeof(char),16U);});_tmp15C.f1=_tmp1F0;});_tmp15C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp123=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15B;_tmp15B.tag=0U,_tmp15B.f1=unimp_msg_part;_tmp15B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp124=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15A;_tmp15A.tag=0U,({struct _fat_ptr _tmp1F1=({const char*_tmp125=" declarations not allowed";_tag_fat(_tmp125,sizeof(char),26U);});_tmp15A.f1=_tmp1F1;});_tmp15A;});void*_tmp121[3U];_tmp121[0]=& _tmp122,_tmp121[1]=& _tmp123,_tmp121[2]=& _tmp124;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp121,sizeof(void*),3U));});}}_LL0:;}
# 553
int Cyc_Tcstmt_ensure_no_throws(struct Cyc_Absyn_Stmt*s){
void*_tmp127=s->r;void*_stmttmpA=_tmp127;void*_tmp128=_stmttmpA;struct Cyc_Absyn_Exp*_tmp129;if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp128)->tag == 3U){_LL1: _tmp129=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp128)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp129;
# 556
if((unsigned)e){
void*_tmp12A=e->r;void*_stmttmpB=_tmp12A;void*_tmp12B=_stmttmpB;struct Cyc_List_List*_tmp12C;switch(*((int*)_tmp12B)){case 0U: _LL6: _LL7:
# 559
 return 1;case 3U: _LL8: _tmp12C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12B)->f2;_LL9: {struct Cyc_List_List*es=_tmp12C;
# 561
while((unsigned)es){
{void*_tmp12D=((struct Cyc_Absyn_Exp*)es->hd)->r;void*_stmttmpC=_tmp12D;void*_tmp12E=_stmttmpC;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12E)->tag == 0U){_LLD: _LLE:
# 564
 goto _LLC;}else{_LLF: _LL10:
# 566
 return 0;}_LLC:;}
# 568
es=es->tl;}
# 570
return 1;}default: _LLA: _LLB:
# 572
 return 0;}_LL5:;}else{
# 576
return 1;}}}else{_LL3: _LL4:
# 579
 return 0;}_LL0:;}

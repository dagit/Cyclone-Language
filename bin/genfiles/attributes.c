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
 struct Cyc_Core_Opt{void*v;};
# 119 "core.h"
extern int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "/tmp/cyclone/include/list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
extern int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 49 "/tmp/cyclone/include/string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 900 "/tmp/cyclone/src/absyn.h"
void*Cyc_Absyn_compress(void*);
# 46 "/tmp/cyclone/src/evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "/tmp/cyclone/src/relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 53 "/tmp/cyclone/src/tcutil.h"
int Cyc_Tcutil_is_bound_one(void*);
# 230
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 285
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 35 "/tmp/cyclone/src/warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 25 "/tmp/cyclone/src/attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 41
int Cyc_Atts_fntype_att(void*);
# 50
int Cyc_Atts_equal_att(void*,void*);
# 52
int Cyc_Atts_attribute_cmp(void*,void*);
# 30 "/tmp/cyclone/src/attributes.cyc"
int Cyc_Atts_is_noreturn_fn_type(void*t){
void*_tmp0=Cyc_Absyn_compress(t);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_List_List*_tmp2;void*_tmp3;switch(*((int*)_tmp1)){case 3U: _LL1: _tmp3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1)->f1).elt_type;_LL2: {void*elt=_tmp3;
return Cyc_Atts_is_noreturn_fn_type(elt);}case 5U: _LL3: _tmp2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1)->f1).attributes;_LL4: {struct Cyc_List_List*atts=_tmp2;
# 34
for(0;atts != 0;atts=atts->tl){
void*_tmp4=(void*)atts->hd;void*_stmttmp1=_tmp4;void*_tmp5=_stmttmp1;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp5)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 39
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 44
int Cyc_Atts_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a2))
return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a1))
return 0;}}
return 1;}
# 54
static int Cyc_Atts_is_regparm0_att(void*a){
void*_tmp6=a;if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp6)->tag == 0U){if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp6)->f1 == 0){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 61
int Cyc_Atts_equiv_fn_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a2)&& !Cyc_Atts_is_regparm0_att((void*)a->hd))
return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a1)&& !Cyc_Atts_is_regparm0_att((void*)a->hd))
return 0;}}
return 1;}
# 73
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp7=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp7;void*_tmp8=_stmttmp2;struct Cyc_List_List**_tmp9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8)->tag == 5U){_LL1: _tmp9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8)->f1).attributes;_LL2: {struct Cyc_List_List**fnatts=_tmp9;
# 76
struct Cyc_List_List*_tmpA=0;struct Cyc_List_List*res_atts=_tmpA;
for(0;atts != 0;atts=atts->tl){
if(!Cyc_Atts_fntype_att((void*)atts->hd))
res_atts=({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->hd=(void*)atts->hd,_tmpB->tl=res_atts;_tmpB;});else{
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Atts_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_tmpA7=({struct Cyc_List_List*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->hd=(void*)atts->hd,_tmpC->tl=*fnatts;_tmpC;});*fnatts=_tmpA7;});}}
return res_atts;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp86;_tmp86.tag=0U,({struct _fat_ptr _tmpA8=({const char*_tmpF="transfer_fn_type_atts";_tag_fat(_tmpF,sizeof(char),22U);});_tmp86.f1=_tmpA8;});_tmp86;});void*_tmpD[1U];_tmpD[0]=& _tmpE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpD,sizeof(void*),1U));});}_LL0:;}
# 87
static int Cyc_Atts_fnTypeAttsRangeOK(unsigned loc,int i,int nargs,void*att){
if(i < 1 || i > nargs){
({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp11=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp88;_tmp88.tag=10U,_tmp88.f1=(void*)att;_tmp88;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp87;_tmp87.tag=0U,({struct _fat_ptr _tmpA9=({const char*_tmp13=" has an out-of-range index";_tag_fat(_tmp13,sizeof(char),27U);});_tmp87.f1=_tmpA9;});_tmp87;});void*_tmp10[2U];_tmp10[0]=& _tmp11,_tmp10[1]=& _tmp12;({unsigned _tmpAA=loc;Cyc_Warn_err2(_tmpAA,_tag_fat(_tmp10,sizeof(void*),2U));});});
return 0;}
# 92
return 1;}struct _tuple11{struct Cyc_List_List*f1;struct _fat_ptr f2;};
# 94
static void Cyc_Atts_fnTypeAttsOverlap(unsigned loc,int i,struct _tuple11 lst1,struct _tuple11 lst2){
# 97
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0U,_tmp8B.f1=(struct _fat_ptr)((struct _fat_ptr)lst1.f2);_tmp8B;});struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp8A;_tmp8A.tag=0U,_tmp8A.f1=(struct _fat_ptr)((struct _fat_ptr)lst2.f2);_tmp8A;});struct Cyc_Int_pa_PrintArg_struct _tmp18=({struct Cyc_Int_pa_PrintArg_struct _tmp89;_tmp89.tag=1U,_tmp89.f1=(unsigned long)i;_tmp89;});void*_tmp14[3U];_tmp14[0]=& _tmp16,_tmp14[1]=& _tmp17,_tmp14[2]=& _tmp18;({unsigned _tmpAC=loc;struct _fat_ptr _tmpAB=({const char*_tmp15="incompatible %s() and %s() attributes on parameter %d";_tag_fat(_tmp15,sizeof(char),54U);});Cyc_Warn_err(_tmpAC,_tmpAB,_tag_fat(_tmp14,sizeof(void*),3U));});});}
# 101
void Cyc_Atts_fnTypeAttsOK(unsigned loc,void*t){
struct _tuple11 init_params=({struct _tuple11 _tmp99;_tmp99.f1=0,({struct _fat_ptr _tmpAD=({const char*_tmp4C="initializes";_tag_fat(_tmp4C,sizeof(char),12U);});_tmp99.f2=_tmpAD;});_tmp99;});
struct _tuple11 nolive_unique_params=({struct _tuple11 _tmp98;_tmp98.f1=0,({struct _fat_ptr _tmpAE=({const char*_tmp4B="noliveunique";_tag_fat(_tmp4B,sizeof(char),13U);});_tmp98.f2=_tmpAE;});_tmp98;});
struct _tuple11 consume_params=({struct _tuple11 _tmp97;_tmp97.f1=0,({struct _fat_ptr _tmpAF=({const char*_tmp4A="consume";_tag_fat(_tmp4A,sizeof(char),8U);});_tmp97.f2=_tmpAF;});_tmp97;});
void*_tmp19=Cyc_Absyn_compress(t);void*_stmttmp3=_tmp19;void*_tmp1A=_stmttmp3;struct Cyc_List_List*_tmp1C;struct Cyc_List_List*_tmp1B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A)->tag == 5U){_LL1: _tmp1B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A)->f1).attributes;_tmp1C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A)->f1).args;_LL2: {struct Cyc_List_List*atts=_tmp1B;struct Cyc_List_List*args=_tmp1C;
# 107
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args);
for(0;atts != 0;atts=atts->tl){
void*_tmp1D=(void*)atts->hd;void*_stmttmp4=_tmp1D;void*_tmp1E=_stmttmp4;int _tmp1F;int _tmp20;int _tmp21;switch(*((int*)_tmp1E)){case 20U: _LL6: _tmp21=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1E)->f1;_LL7: {int i=_tmp21;
# 111
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))
goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,init_params,nolive_unique_params);
Cyc_Atts_fnTypeAttsOverlap(loc,i,init_params,consume_params);{
struct _fat_ptr s=({const char*_tmp38="initializes attribute allowed only on";_tag_fat(_tmp38,sizeof(char),38U);});
{void*_tmp22=Cyc_Absyn_compress((*((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1)).f3);void*_stmttmp5=_tmp22;void*_tmp23=_stmttmp5;void*_tmp26;void*_tmp25;void*_tmp24;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->tag == 3U){_LLF: _tmp24=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).nullable;_tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).bounds;_tmp26=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).zero_term;_LL10: {void*nullable=_tmp24;void*bd=_tmp25;void*zt=_tmp26;
# 118
if(Cyc_Tcutil_force_type2bool(0,nullable))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28=({struct Cyc_Warn_String_Warn_Warg_struct _tmp8D;_tmp8D.tag=0U,_tmp8D.f1=s;_tmp8D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmp8C;_tmp8C.tag=0U,({struct _fat_ptr _tmpB0=({const char*_tmp2A=" non-null pointers";_tag_fat(_tmp2A,sizeof(char),19U);});_tmp8C.f1=_tmpB0;});_tmp8C;});void*_tmp27[2U];_tmp27[0]=& _tmp28,_tmp27[1]=& _tmp29;({unsigned _tmpB1=loc;Cyc_Warn_err2(_tmpB1,_tag_fat(_tmp27,sizeof(void*),2U));});});
if(!Cyc_Tcutil_is_bound_one(bd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp8F;_tmp8F.tag=0U,_tmp8F.f1=s;_tmp8F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp8E;_tmp8E.tag=0U,({struct _fat_ptr _tmpB2=({const char*_tmp2E=" pointers of size 1";_tag_fat(_tmp2E,sizeof(char),20U);});_tmp8E.f1=_tmpB2;});_tmp8E;});void*_tmp2B[2U];_tmp2B[0]=& _tmp2C,_tmp2B[1]=& _tmp2D;({unsigned _tmpB3=loc;Cyc_Warn_err2(_tmpB3,_tag_fat(_tmp2B,sizeof(void*),2U));});});
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp30=({struct Cyc_Warn_String_Warn_Warg_struct _tmp91;_tmp91.tag=0U,_tmp91.f1=s;_tmp91;});struct Cyc_Warn_String_Warn_Warg_struct _tmp31=({struct Cyc_Warn_String_Warn_Warg_struct _tmp90;_tmp90.tag=0U,({struct _fat_ptr _tmpB4=({const char*_tmp32=" pointers to non-zero-terminated arrays";_tag_fat(_tmp32,sizeof(char),40U);});_tmp90.f1=_tmpB4;});_tmp90;});void*_tmp2F[2U];_tmp2F[0]=& _tmp30,_tmp2F[1]=& _tmp31;({unsigned _tmpB5=loc;Cyc_Warn_err2(_tmpB5,_tag_fat(_tmp2F,sizeof(void*),2U));});});
goto _LLE;}}else{_LL11: _LL12:
# 126
({struct Cyc_Warn_String_Warn_Warg_struct _tmp34=({struct Cyc_Warn_String_Warn_Warg_struct _tmp93;_tmp93.tag=0U,_tmp93.f1=s;_tmp93;});struct Cyc_Warn_String_Warn_Warg_struct _tmp35=({struct Cyc_Warn_String_Warn_Warg_struct _tmp92;_tmp92.tag=0U,({struct _fat_ptr _tmpB6=({const char*_tmp36=" pointers";_tag_fat(_tmp36,sizeof(char),10U);});_tmp92.f1=_tmpB6;});_tmp92;});void*_tmp33[2U];_tmp33[0]=& _tmp34,_tmp33[1]=& _tmp35;({unsigned _tmpB7=loc;Cyc_Warn_err2(_tmpB7,_tag_fat(_tmp33,sizeof(void*),2U));});});}_LLE:;}
# 128
({struct Cyc_List_List*_tmpB8=({struct Cyc_List_List*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->hd=(void*)i,_tmp37->tl=init_params.f1;_tmp37;});init_params.f1=_tmpB8;});
goto _LL5;}}case 21U: _LL8: _tmp20=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1E)->f1;_LL9: {int i=_tmp20;
# 131
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,nolive_unique_params,init_params);{
# 134
struct _tuple8*_tmp39=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp6=_tmp39;struct _tuple8*_tmp3A=_stmttmp6;void*_tmp3B;_LL14: _tmp3B=_tmp3A->f3;_LL15: {void*t=_tmp3B;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp94;_tmp94.tag=0U,({struct _fat_ptr _tmpB9=({const char*_tmp3E="noliveunique allowed only on unique pointers";_tag_fat(_tmp3E,sizeof(char),45U);});_tmp94.f1=_tmpB9;});_tmp94;});void*_tmp3C[1U];_tmp3C[0]=& _tmp3D;({unsigned _tmpBA=loc;Cyc_Warn_err2(_tmpBA,_tag_fat(_tmp3C,sizeof(void*),1U));});});
({struct Cyc_List_List*_tmpBB=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->hd=(void*)i,_tmp3F->tl=nolive_unique_params.f1;_tmp3F;});nolive_unique_params.f1=_tmpBB;});
goto _LL5;}}}case 22U: _LLA: _tmp1F=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp1E)->f1;_LLB: {int i=_tmp1F;
# 140
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,consume_params,init_params);{
# 143
struct _tuple8*_tmp40=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp7=_tmp40;struct _tuple8*_tmp41=_stmttmp7;void*_tmp42;_LL17: _tmp42=_tmp41->f3;_LL18: {void*t=_tmp42;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44=({struct Cyc_Warn_String_Warn_Warg_struct _tmp95;_tmp95.tag=0U,({struct _fat_ptr _tmpBC=({const char*_tmp45="consume allowed only on unique pointers";_tag_fat(_tmp45,sizeof(char),40U);});_tmp95.f1=_tmpBC;});_tmp95;});void*_tmp43[1U];_tmp43[0]=& _tmp44;({unsigned _tmpBD=loc;Cyc_Warn_err2(_tmpBD,_tag_fat(_tmp43,sizeof(void*),1U));});});
({struct Cyc_List_List*_tmpBE=({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->hd=(void*)i,_tmp46->tl=consume_params.f1;_tmp46;});consume_params.f1=_tmpBE;});
goto _LL5;}}}default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 151
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp48=({struct Cyc_Warn_String_Warn_Warg_struct _tmp96;_tmp96.tag=0U,({struct _fat_ptr _tmpBF=({const char*_tmp49="fnTypeAttsOK: not a function type";_tag_fat(_tmp49,sizeof(char),34U);});_tmp96.f1=_tmpBF;});_tmp96;});void*_tmp47[1U];_tmp47[0]=& _tmp48;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp47,sizeof(void*),1U));});}_LL0:;}
# 157
int Cyc_Atts_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp4D=(void*)t->hd;void*_stmttmp8=_tmp4D;void*_tmp4E=_stmttmp8;switch(*((int*)_tmp4E)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 165
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a2->hd,a1))
return 0;}
return 1;}
# 171
static int Cyc_Atts_attribute_case_number(void*att){
void*_tmp4F=att;switch(*((int*)_tmp4F)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: _LLF: _LL10:
 return 7;case 8U: _LL11: _LL12:
 return 8;case 9U: _LL13: _LL14:
 return 9;case 10U: _LL15: _LL16:
 return 10;case 11U: _LL17: _LL18:
 return 11;case 12U: _LL19: _LL1A:
 return 12;case 13U: _LL1B: _LL1C:
 return 13;case 14U: _LL1D: _LL1E:
 return 14;case 15U: _LL1F: _LL20:
 return 15;case 16U: _LL21: _LL22:
 return 16;case 17U: _LL23: _LL24:
 return 17;case 18U: _LL25: _LL26:
 return 18;case 19U: _LL27: _LL28:
 return 19;case 20U: _LL29: _LL2A:
 return 20;case 21U: _LL2B: _LL2C:
 return 21;case 22U: _LL2D: _LL2E:
 return 22;case 23U: _LL2F: _LL30:
 return 23;case 24U: _LL31: _LL32:
 return 24;case 25U: _LL33: _LL34:
 return 25;default: _LL35: _LL36:
 return 26;}_LL0:;}struct _tuple12{void*f1;void*f2;};
# 202
int Cyc_Atts_attribute_cmp(void*att1,void*att2){
struct _tuple12 _tmp50=({struct _tuple12 _tmp9A;_tmp9A.f1=att1,_tmp9A.f2=att2;_tmp9A;});struct _tuple12 _stmttmp9=_tmp50;struct _tuple12 _tmp51=_stmttmp9;int _tmp57;int _tmp56;enum Cyc_Absyn_Format_Type _tmp55;int _tmp54;int _tmp53;enum Cyc_Absyn_Format_Type _tmp52;struct _fat_ptr _tmp59;struct _fat_ptr _tmp58;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Exp*_tmp5A;int _tmp5D;int _tmp5C;int _tmp5F;int _tmp5E;switch(*((int*)_tmp51.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp51.f2)->tag == 0U){_LL1: _tmp5E=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp51.f1)->f1;_tmp5F=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp51.f2)->f1;_LL2: {int i1=_tmp5E;int i2=_tmp5F;
_tmp5C=i1;_tmp5D=i2;goto _LL4;}}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp51.f2)->tag == 20U){_LL3: _tmp5C=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp51.f1)->f1;_tmp5D=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp51.f2)->f1;_LL4: {int i1=_tmp5C;int i2=_tmp5D;
return Cyc_Core_intcmp(i1,i2);}}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp51.f2)->tag == 6U){_LL5: _tmp5A=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp51.f1)->f1;_tmp5B=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp51.f2)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp5A;struct Cyc_Absyn_Exp*e2=_tmp5B;
# 207
if(e1 == e2)return 0;
if(e1 == 0)return - 1;
if(e2 == 0)return 1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp51.f2)->tag == 8U){_LL7: _tmp58=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp51.f1)->f1;_tmp59=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp51.f2)->f1;_LL8: {struct _fat_ptr s1=_tmp58;struct _fat_ptr s2=_tmp59;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp51.f2)->tag == 19U){_LL9: _tmp52=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp51.f1)->f1;_tmp53=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp51.f1)->f2;_tmp54=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp51.f1)->f3;_tmp55=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp51.f2)->f1;_tmp56=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp51.f2)->f2;_tmp57=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp51.f2)->f3;_LLA: {enum Cyc_Absyn_Format_Type ft1=_tmp52;int i1=_tmp53;int j1=_tmp54;enum Cyc_Absyn_Format_Type ft2=_tmp55;int i2=_tmp56;int j2=_tmp57;
# 213
int _tmp60=Cyc_Core_intcmp((int)((unsigned)ft1),(int)((unsigned)ft2));int ftc=_tmp60;
if(ftc != 0)return ftc;{
int _tmp61=Cyc_Core_intcmp(i1,i2);int ic=_tmp61;
if(ic != 0)return ic;
return Cyc_Core_intcmp(j1,j2);}}}else{goto _LLB;}default: _LLB: _LLC:
# 219
 return({int _tmpC0=Cyc_Atts_attribute_case_number(att1);Cyc_Core_intcmp(_tmpC0,
Cyc_Atts_attribute_case_number(att2));});}_LL0:;}
# 223
int Cyc_Atts_equal_att(void*a1,void*a2){
return Cyc_Atts_attribute_cmp(a1,a2)== 0;}
# 228
int Cyc_Atts_fntype_att(void*a){
void*_tmp62=a;int _tmp63;switch(*((int*)_tmp62)){case 0U: _LL1: _tmp63=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp62)->f1;_LL2: {int i=_tmp63;
goto _LL4;}case 3U: _LL3: _LL4:
 goto _LL6;case 1U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 23U: _LLB: _LLC:
 goto _LLE;case 26U: _LLD: _LLE:
 goto _LL10;case 19U: _LLF: _LL10:
 goto _LL12;case 5U: _LL11: _LL12:
 goto _LL14;case 21U: _LL13: _LL14:
 goto _LL16;case 20U: _LL15: _LL16:
 goto _LL18;case 22U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 246
void Cyc_Atts_check_fndecl_atts(unsigned loc,struct Cyc_List_List*atts,int is_defn){
for(0;atts != 0;atts=atts->tl){
void*_tmp64=(void*)atts->hd;void*_stmttmpA=_tmp64;void*_tmp65=_stmttmpA;switch(*((int*)_tmp65)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
# 251
({struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp9D;_tmp9D.tag=0U,({struct _fat_ptr _tmpC1=({const char*_tmp6B="bad attribute ";_tag_fat(_tmp6B,sizeof(char),15U);});_tmp9D.f1=_tmpC1;});_tmp9D;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp68=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp9C;_tmp9C.tag=10U,_tmp9C.f1=(void*)atts->hd;_tmp9C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp69=({struct Cyc_Warn_String_Warn_Warg_struct _tmp9B;_tmp9B.tag=0U,({struct _fat_ptr _tmpC2=({const char*_tmp6A=" in function declaration";_tag_fat(_tmp6A,sizeof(char),25U);});_tmp9B.f1=_tmpC2;});_tmp9B;});void*_tmp66[3U];_tmp66[0]=& _tmp67,_tmp66[1]=& _tmp68,_tmp66[2]=& _tmp69;({unsigned _tmpC3=loc;Cyc_Warn_err2(_tmpC3,_tag_fat(_tmp66,sizeof(void*),3U));});});
return;default: _LL5: _LL6:
# 254
 if(!is_defn && Cyc_Atts_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp9E;_tmp9E.tag=0U,({struct _fat_ptr _tmpC4=({const char*_tmp6E="function-type att in function variable declaration";_tag_fat(_tmp6E,sizeof(char),51U);});_tmp9E.f1=_tmpC4;});_tmp9E;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp6C,sizeof(void*),1U));});}_LL0:;}}
# 259
void Cyc_Atts_check_variable_atts(unsigned loc,struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp6F=(void*)atts->hd;void*_stmttmpB=_tmp6F;void*_tmp70=_stmttmpB;switch(*((int*)_tmp70)){case 6U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
# 265
 goto _LL6;case 9U: _LL5: _LL6:
 goto _LL8;case 10U: _LL7: _LL8:
 goto _LLA;case 11U: _LL9: _LLA:
 goto _LLC;case 12U: _LLB: _LLC:
 goto _LLE;case 13U: _LLD: _LLE:
 goto _LL10;case 14U: _LLF: _LL10:
 continue;default: _LL11: _LL12:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp72=({struct Cyc_Warn_String_Warn_Warg_struct _tmpA2;_tmpA2.tag=0U,({struct _fat_ptr _tmpC5=({const char*_tmp77="bad attribute ";_tag_fat(_tmp77,sizeof(char),15U);});_tmpA2.f1=_tmpC5;});_tmpA2;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp73=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpA1;_tmpA1.tag=10U,_tmpA1.f1=(void*)atts->hd;_tmpA1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmpA0;_tmpA0.tag=0U,({struct _fat_ptr _tmpC6=({const char*_tmp76=" for variable ";_tag_fat(_tmp76,sizeof(char),15U);});_tmpA0.f1=_tmpC6;});_tmpA0;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp75=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp9F;_tmp9F.tag=11U,_tmp9F.f1=vd;_tmp9F;});void*_tmp71[4U];_tmp71[0]=& _tmp72,_tmp71[1]=& _tmp73,_tmp71[2]=& _tmp74,_tmp71[3]=& _tmp75;({unsigned _tmpC7=loc;Cyc_Warn_err2(_tmpC7,_tag_fat(_tmp71,sizeof(void*),4U));});});}_LL0:;}}
# 276
void Cyc_Atts_check_field_atts(unsigned loc,struct _fat_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp78=(void*)atts->hd;void*_stmttmpC=_tmp78;void*_tmp79=_stmttmpC;switch(*((int*)_tmp79)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmpA6;_tmpA6.tag=0U,({struct _fat_ptr _tmpC8=({const char*_tmp80="bad attribute ";_tag_fat(_tmp80,sizeof(char),15U);});_tmpA6.f1=_tmpC8;});_tmpA6;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp7C=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpA5;_tmpA5.tag=10U,_tmpA5.f1=(void*)atts->hd;_tmpA5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7D=({struct Cyc_Warn_String_Warn_Warg_struct _tmpA4;_tmpA4.tag=0U,({struct _fat_ptr _tmpC9=({const char*_tmp7F=" on ";_tag_fat(_tmp7F,sizeof(char),5U);});_tmpA4.f1=_tmpC9;});_tmpA4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmpA3;_tmpA3.tag=0U,_tmpA3.f1=*fn;_tmpA3;});void*_tmp7A[4U];_tmp7A[0]=& _tmp7B,_tmp7A[1]=& _tmp7C,_tmp7A[2]=& _tmp7D,_tmp7A[3]=& _tmp7E;({unsigned _tmpCA=loc;Cyc_Warn_err2(_tmpCA,_tag_fat(_tmp7A,sizeof(void*),4U));});});}_LL0:;}}
# 286
struct Cyc_List_List*Cyc_Atts_merge_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
struct Cyc_List_List*x=0;
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a2))
x=({struct Cyc_List_List*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->hd=(void*)a->hd,_tmp81->tl=x;_tmp81;});}}
return({struct Cyc_List_List*_tmpCB=x;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpCB,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(a2));});}
# 294
struct Cyc_List_List*Cyc_Atts_atts2c(struct Cyc_List_List*atts){
# 297
struct Cyc_List_List*_tmp82=0;struct Cyc_List_List*ans=_tmp82;
for(0;atts != 0;atts=atts->tl){
void*_tmp83=(void*)atts->hd;void*_stmttmpD=_tmp83;void*_tmp84=_stmttmpD;switch(*((int*)_tmp84)){case 4U: _LL1: _LL2:
 goto _LL4;case 5U: _LL3: _LL4:
 goto _LL6;case 19U: _LL5: _LL6:
 continue;case 22U: _LL7: _LL8:
 continue;case 21U: _LL9: _LLA:
 continue;case 20U: _LLB: _LLC:
 continue;default: _LLD: _LLE:
 ans=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=(void*)atts->hd,_tmp85->tl=ans;_tmp85;});goto _LL0;}_LL0:;}
# 308
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}

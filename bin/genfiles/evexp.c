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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 844 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 864
void*Cyc_Absyn_compress(void*);
# 882
extern void*Cyc_Absyn_uint_type;
# 954
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 956
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 967
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1127
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);
# 41 "evexp.h"
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 46
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 50
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 53
int Cyc_Evexp_okay_szofarg(void*);struct _tuple12{union Cyc_Absyn_Cnst f1;int f2;};
# 33 "evexp.cyc"
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 35
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;short _tmp2;char _tmp3;enum Cyc_Absyn_Sign _tmp1;switch((_tmp0.Short_c).tag){case 2: _tmp1=((_tmp0.Char_c).val).f1;_tmp3=((_tmp0.Char_c).val).f2;{enum Cyc_Absyn_Sign sn=_tmp1;char c=_tmp3;
return Cyc_Absyn_Int_c(sn,(int)c);}case 4: _tmp1=((_tmp0.Short_c).val).f1;_tmp2=((_tmp0.Short_c).val).f2;{enum Cyc_Absyn_Sign sn=_tmp1;short s=_tmp2;
# 39
return Cyc_Absyn_Int_c(sn,(int)s);}default:
 return cn;};}
# 44
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmp4=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp0=_tmp4;struct _tuple12 _tmp5=_stmttmp0;int _tmp7;union Cyc_Absyn_Cnst _tmp6;_tmp6=_tmp5.f1;_tmp7=_tmp5.f2;{union Cyc_Absyn_Cnst cn=_tmp6;int known=_tmp7;
if(!known)
return({struct _tuple11 _tmpF8;_tmpF8.f1=0U,_tmpF8.f2=0;_tmpF8;});{
union Cyc_Absyn_Cnst _tmp8=Cyc_Evexp_promote_const(cn);union Cyc_Absyn_Cnst _stmttmp1=_tmp8;union Cyc_Absyn_Cnst _tmp9=_stmttmp1;long long _tmpA;int _tmpB;switch((_tmp9.Null_c).tag){case 5: _tmpB=((_tmp9.Int_c).val).f2;{int i=_tmpB;
return({struct _tuple11 _tmpF9;_tmpF9.f1=(unsigned)i,_tmpF9.f2=1;_tmpF9;});}case 6: _tmpA=((_tmp9.LongLong_c).val).f2;{long long x=_tmpA;
# 52
unsigned long long y=(unsigned long long)x >> (unsigned long long)32;
if(y != 4294967295U && y != 0U)
return({struct _tuple11 _tmpFA;_tmpFA.f1=0U,_tmpFA.f2=0;_tmpFA;});
return({struct _tuple11 _tmpFB;_tmpFB.f1=(unsigned)x,_tmpFB.f2=1;_tmpFB;});}case 7:
 return({struct _tuple11 _tmpFC;_tmpFC.f1=0U,_tmpFC.f2=0;_tmpFC;});case 1:
 return({struct _tuple11 _tmpFD;_tmpFD.f1=0U,_tmpFD.f2=1;_tmpFD;});default:
 return({struct _tuple11 _tmpFE;_tmpFE.f1=0U,_tmpFE.f2=0;_tmpFE;});};}}}struct _tuple13{int f1;int f2;};
# 62
static struct _tuple13 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmpC=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp2=_tmpC;struct _tuple12 _tmpD=_stmttmp2;int _tmpF;union Cyc_Absyn_Cnst _tmpE;_tmpE=_tmpD.f1;_tmpF=_tmpD.f2;{union Cyc_Absyn_Cnst cn=_tmpE;int known=_tmpF;
if(!known)
return({struct _tuple13 _tmpFF;_tmpFF.f1=0,_tmpFF.f2=0;_tmpFF;});{
union Cyc_Absyn_Cnst _tmp10=Cyc_Evexp_promote_const(cn);union Cyc_Absyn_Cnst _stmttmp3=_tmp10;union Cyc_Absyn_Cnst _tmp11=_stmttmp3;long long _tmp12;int _tmp13;switch((_tmp11.Float_c).tag){case 5: _tmp13=((_tmp11.Int_c).val).f2;{int b=_tmp13;
return({struct _tuple13 _tmp100;_tmp100.f1=b != 0,_tmp100.f2=1;_tmp100;});}case 6: _tmp12=((_tmp11.LongLong_c).val).f2;{long long b=_tmp12;
return({struct _tuple13 _tmp101;_tmp101.f1=b != (long long)0,_tmp101.f2=1;_tmp101;});}case 1:
 return({struct _tuple13 _tmp102;_tmp102.f1=0,_tmp102.f2=0;_tmp102;});case 7:
 return({struct _tuple13 _tmp103;_tmp103.f1=0,_tmp103.f2=1;_tmp103;});default:
 return({struct _tuple13 _tmp104;_tmp104.f1=0,_tmp104.f2=0;_tmp104;});};}}}struct _tuple14{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 75
static struct _tuple12 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmp14=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp4=_tmp14;struct _tuple12 _tmp15=_stmttmp4;int _tmp17;union Cyc_Absyn_Cnst _tmp16;_tmp16=_tmp15.f1;_tmp17=_tmp15.f2;{union Cyc_Absyn_Cnst cn=_tmp16;int known=_tmp17;
if(!known)
return({struct _tuple12 _tmp105;_tmp105.f1=cn,_tmp105.f2=0;_tmp105;});
{struct _tuple14 _tmp18=({struct _tuple14 _tmp107;_tmp107.f1=p,_tmp107.f2=cn;_tmp107;});struct _tuple14 _stmttmp5=_tmp18;struct _tuple14 _tmp19=_stmttmp5;long long _tmp1A;char _tmp1B;short _tmp1C;int _tmp1D;enum Cyc_Absyn_Sign _tmp1E;switch((int)_tmp19.f1){case Cyc_Absyn_Plus:
 goto _LL3;case Cyc_Absyn_Minus: switch(((_tmp19.f2).LongLong_c).tag){case 5: _tmp1E=(((_tmp19.f2).Int_c).val).f1;_tmp1D=(((_tmp19.f2).Int_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp1E;int i=_tmp1D;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 4: _tmp1E=(((_tmp19.f2).Short_c).val).f1;_tmp1C=(((_tmp19.f2).Short_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp1E;short i=_tmp1C;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 2: _tmp1E=(((_tmp19.f2).Char_c).val).f1;_tmp1B=(((_tmp19.f2).Char_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp1E;char i=_tmp1B;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 6: _tmp1E=(((_tmp19.f2).LongLong_c).val).f1;_tmp1A=(((_tmp19.f2).LongLong_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp1E;long long i=_tmp1A;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,- i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Bitnot: switch(((_tmp19.f2).LongLong_c).tag){case 5: _tmp1E=(((_tmp19.f2).Int_c).val).f1;_tmp1D=(((_tmp19.f2).Int_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp1E;int i=_tmp1D;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 4: _tmp1E=(((_tmp19.f2).Short_c).val).f1;_tmp1C=(((_tmp19.f2).Short_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp1E;short i=_tmp1C;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 2: _tmp1E=(((_tmp19.f2).Char_c).val).f1;_tmp1B=(((_tmp19.f2).Char_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp1E;char i=_tmp1B;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 6: _tmp1E=(((_tmp19.f2).LongLong_c).val).f1;_tmp1A=(((_tmp19.f2).LongLong_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp1E;long long i=_tmp1A;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Not: switch(((_tmp19.f2).LongLong_c).tag){case 5: _tmp1D=(((_tmp19.f2).Int_c).val).f2;{int i=_tmp1D;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i == 0?1: 0);goto _LL3;}case 4: _tmp1C=(((_tmp19.f2).Short_c).val).f2;{short i=_tmp1C;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,(int)i == 0?1: 0);goto _LL3;}case 2: _tmp1B=(((_tmp19.f2).Char_c).val).f2;{char i=_tmp1B;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,(int)i == 0?'\001':'\000');goto _LL3;}case 1:
 cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL3;case 6: _tmp1A=(((_tmp19.f2).LongLong_c).val).f2;{long long i=_tmp1A;
# 94
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,(long long)(i == 0?1: 0));goto _LL3;}default: goto _LL20;}default: _LL20:
 return({struct _tuple12 _tmp106;_tmp106.f1=cn,_tmp106.f2=0;_tmp106;});}_LL3:;}
# 97
return({struct _tuple12 _tmp108;_tmp108.f1=cn,_tmp108.f2=1;_tmp108;});}}struct _tuple15{enum Cyc_Absyn_Primop f1;int f2;};
# 101
static struct _tuple12 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple12 _tmp1F=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp6=_tmp1F;struct _tuple12 _tmp20=_stmttmp6;int _tmp22;union Cyc_Absyn_Cnst _tmp21;_tmp21=_tmp20.f1;_tmp22=_tmp20.f2;{union Cyc_Absyn_Cnst cn1=_tmp21;int known1=_tmp22;
struct _tuple12 _tmp23=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp7=_tmp23;struct _tuple12 _tmp24=_stmttmp7;int _tmp26;union Cyc_Absyn_Cnst _tmp25;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;{union Cyc_Absyn_Cnst cn2=_tmp25;int known2=_tmp26;
if(!known1 || !known2)
return({struct _tuple12 _tmp109;_tmp109.f1=cn1,_tmp109.f2=0;_tmp109;});
cn1=Cyc_Evexp_promote_const(cn1);
cn2=Cyc_Evexp_promote_const(cn2);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp27=cn1;int _tmp29;enum Cyc_Absyn_Sign _tmp28;if((_tmp27.Int_c).tag == 5){_tmp28=((_tmp27.Int_c).val).f1;_tmp29=((_tmp27.Int_c).val).f2;{enum Cyc_Absyn_Sign x=_tmp28;int y=_tmp29;
s1=x;i1=y;goto _LL6;}}else{
return({struct _tuple12 _tmp10A;_tmp10A.f1=cn1,_tmp10A.f2=0;_tmp10A;});}_LL6:;}
# 114
{union Cyc_Absyn_Cnst _tmp2A=cn2;int _tmp2C;enum Cyc_Absyn_Sign _tmp2B;if((_tmp2A.Int_c).tag == 5){_tmp2B=((_tmp2A.Int_c).val).f1;_tmp2C=((_tmp2A.Int_c).val).f2;{enum Cyc_Absyn_Sign x=_tmp2B;int y=_tmp2C;
s2=x;i2=y;goto _LLB;}}else{
return({struct _tuple12 _tmp10B;_tmp10B.f1=cn1,_tmp10B.f2=0;_tmp10B;});}_LLB:;}
# 118
{enum Cyc_Absyn_Primop _tmp2D=p;switch((int)_tmp2D){case Cyc_Absyn_Div:
 goto _LL14;case Cyc_Absyn_Mod: _LL14:
# 121
 if(i2 != 0)
goto _LL10;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp10C;_tmp10C.tag=0,({struct _fat_ptr _tmp134=({const char*_tmp30="division by zero in constant expression";_tag_fat(_tmp30,sizeof(char),40U);});_tmp10C.f1=_tmp134;});_tmp10C;});void*_tmp2E[1];_tmp2E[0]=& _tmp2F;({unsigned _tmp135=e2->loc;Cyc_Warn_err2(_tmp135,_tag_fat(_tmp2E,sizeof(void*),1));});});
return({struct _tuple12 _tmp10D;_tmp10D.f1=cn1,_tmp10D.f2=1;_tmp10D;});default:
 goto _LL10;}_LL10:;}{
# 127
int has_u_arg=(int)s1 == 1 ||(int)s2 == (int)Cyc_Absyn_Unsigned;
unsigned u1=(unsigned)i1;
unsigned u2=(unsigned)i2;
int i3=0;
unsigned u3=0U;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple15 _tmp31=({struct _tuple15 _tmp110;_tmp110.f1=p,_tmp110.f2=has_u_arg;_tmp110;});struct _tuple15 _stmttmp8=_tmp31;struct _tuple15 _tmp32=_stmttmp8;switch((int)_tmp32.f1){case Cyc_Absyn_Plus: switch((int)_tmp32.f2){case 0:
 i3=i1 + i2;use_i3=1;goto _LL17;case 1:
# 142
 u3=u1 + u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Times: switch((int)_tmp32.f2){case 0:
# 138
 i3=i1 * i2;use_i3=1;goto _LL17;case 1:
# 143
 u3=u1 * u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Minus: switch((int)_tmp32.f2){case 0:
# 139
 i3=i1 - i2;use_i3=1;goto _LL17;case 1:
# 144
 u3=u1 - u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Div: switch((int)_tmp32.f2){case 0:
# 140
 i3=i1 / i2;use_i3=1;goto _LL17;case 1:
# 145
 u3=u1 / u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Mod: switch((int)_tmp32.f2){case 0:
# 141
 i3=i1 % i2;use_i3=1;goto _LL17;case 1:
# 146
 u3=u1 % u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Eq:
 b3=i1 == i2;use_b3=1;goto _LL17;case Cyc_Absyn_Neq:
 b3=i1 != i2;use_b3=1;goto _LL17;case Cyc_Absyn_Gt: switch((int)_tmp32.f2){case 0:
 b3=i1 > i2;use_b3=1;goto _LL17;case 1:
# 153
 b3=u1 > u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lt: switch((int)_tmp32.f2){case 0:
# 150
 b3=i1 < i2;use_b3=1;goto _LL17;case 1:
# 154
 b3=u1 < u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Gte: switch((int)_tmp32.f2){case 0:
# 151
 b3=i1 >= i2;use_b3=1;goto _LL17;case 1:
# 155
 b3=u1 >= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lte: switch((int)_tmp32.f2){case 0:
# 152
 b3=i1 <= i2;use_b3=1;goto _LL17;case 1:
# 156
 b3=u1 <= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Bitand:
 u3=u1 & u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitor:
 u3=u1 | u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitxor:
 u3=u1 ^ u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlshift:
 u3=u1 << u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlrshift:
 u3=u1 >> u2;use_u3=1;goto _LL17;default: _LL4A:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp34=({struct Cyc_Warn_String_Warn_Warg_struct _tmp10E;_tmp10E.tag=0,({struct _fat_ptr _tmp136=({const char*_tmp35="bad constant expression";_tag_fat(_tmp35,sizeof(char),24U);});_tmp10E.f1=_tmp136;});_tmp10E;});void*_tmp33[1];_tmp33[0]=& _tmp34;({unsigned _tmp137=e1->loc;Cyc_Warn_err2(_tmp137,_tag_fat(_tmp33,sizeof(void*),1));});});return({struct _tuple12 _tmp10F;_tmp10F.f1=cn1,_tmp10F.f2=1;_tmp10F;});}_LL17:;}
# 164
if(use_i3)return({struct _tuple12 _tmp111;({union Cyc_Absyn_Cnst _tmp138=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3);_tmp111.f1=_tmp138;}),_tmp111.f2=1;_tmp111;});
if(use_u3)return({struct _tuple12 _tmp112;({union Cyc_Absyn_Cnst _tmp139=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3);_tmp112.f1=_tmp139;}),_tmp112.f2=1;_tmp112;});
if(use_b3)return({struct _tuple12 _tmp113;({union Cyc_Absyn_Cnst _tmp13A=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0);_tmp113.f1=_tmp13A;}),_tmp113.f2=1;_tmp113;});
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37=({struct Cyc_Warn_String_Warn_Warg_struct _tmp114;_tmp114.tag=0,({struct _fat_ptr _tmp13B=({const char*_tmp38="Evexp::eval_const_binop";_tag_fat(_tmp38,sizeof(char),24U);});_tmp114.f1=_tmp13B;});_tmp114;});void*_tmp36[1];_tmp36[0]=& _tmp37;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp36,sizeof(void*),1));});}}}}}struct _tuple16{void*f1;union Cyc_Absyn_Cnst f2;};
# 172
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 ans;
{void*_tmp39=e->r;void*_stmttmp9=_tmp39;void*_tmp3A=_stmttmp9;enum Cyc_Absyn_Primop _tmp3D;void*_tmp3E;void*_tmp3C;void*_tmp3B;union Cyc_Absyn_Cnst _tmp3F;switch(*((int*)_tmp3A)){case 1:
# 176
 return({struct _tuple12 _tmp115;({union Cyc_Absyn_Cnst _tmp13C=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp115.f1=_tmp13C;}),_tmp115.f2=0;_tmp115;});case 0: _tmp3F=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3A)->f1;{union Cyc_Absyn_Cnst c=_tmp3F;
return({struct _tuple12 _tmp116;_tmp116.f1=c,_tmp116.f2=1;_tmp116;});}case 2:
 return({struct _tuple12 _tmp117;({union Cyc_Absyn_Cnst _tmp13D=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp117.f1=_tmp13D;}),_tmp117.f2=1;_tmp117;});case 6: _tmp3B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3A)->f1;_tmp3C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3A)->f2;_tmp3E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3A)->f3;{struct Cyc_Absyn_Exp*e1=_tmp3B;struct Cyc_Absyn_Exp*e2=_tmp3C;struct Cyc_Absyn_Exp*e3=_tmp3E;
# 180
struct _tuple13 _tmp40=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpA=_tmp40;struct _tuple13 _tmp41=_stmttmpA;int _tmp43;int _tmp42;_tmp42=_tmp41.f1;_tmp43=_tmp41.f2;{int bool1=_tmp42;int known1=_tmp43;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
Cyc_Evexp_eval_const_exp(e3);
return({struct _tuple12 _tmp118;({union Cyc_Absyn_Cnst _tmp13E=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp118.f1=_tmp13E;}),_tmp118.f2=0;_tmp118;});}
# 186
ans=bool1?Cyc_Evexp_eval_const_exp(e2): Cyc_Evexp_eval_const_exp(e3);
goto _LL0;}}case 7: _tmp3B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3A)->f1;_tmp3C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3A)->f2;{struct Cyc_Absyn_Exp*e1=_tmp3B;struct Cyc_Absyn_Exp*e2=_tmp3C;
# 189
struct _tuple13 _tmp44=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpB=_tmp44;struct _tuple13 _tmp45=_stmttmpB;int _tmp47;int _tmp46;_tmp46=_tmp45.f1;_tmp47=_tmp45.f2;{int bool1=_tmp46;int known1=_tmp47;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple12 _tmp119;({union Cyc_Absyn_Cnst _tmp13F=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp119.f1=_tmp13F;}),_tmp119.f2=0;_tmp119;});}
# 194
ans=bool1?Cyc_Evexp_eval_const_exp(e2):({struct _tuple12 _tmp11A;({union Cyc_Absyn_Cnst _tmp140=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp11A.f1=_tmp140;}),_tmp11A.f2=1;_tmp11A;});
goto _LL0;}}case 8: _tmp3B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3A)->f1;_tmp3C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3A)->f2;{struct Cyc_Absyn_Exp*e1=_tmp3B;struct Cyc_Absyn_Exp*e2=_tmp3C;
# 197
struct _tuple13 _tmp48=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpC=_tmp48;struct _tuple13 _tmp49=_stmttmpC;int _tmp4B;int _tmp4A;_tmp4A=_tmp49.f1;_tmp4B=_tmp49.f2;{int bool1=_tmp4A;int known1=_tmp4B;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple12 _tmp11B;({union Cyc_Absyn_Cnst _tmp141=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp11B.f1=_tmp141;}),_tmp11B.f2=0;_tmp11B;});}
# 202
ans=bool1?({struct _tuple12 _tmp11C;({union Cyc_Absyn_Cnst _tmp142=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);_tmp11C.f1=_tmp142;}),_tmp11C.f2=1;_tmp11C;}): Cyc_Evexp_eval_const_exp(e2);
goto _LL0;}}case 3: _tmp3D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3A)->f1;_tmp3B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3A)->f2;{enum Cyc_Absyn_Primop p=_tmp3D;struct Cyc_List_List*es=_tmp3B;
# 205
if(es == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp11D;_tmp11D.tag=0,({struct _fat_ptr _tmp143=({const char*_tmp4E="bad static expression (no args to primop)";_tag_fat(_tmp4E,sizeof(char),42U);});_tmp11D.f1=_tmp143;});_tmp11D;});void*_tmp4C[1];_tmp4C[0]=& _tmp4D;({unsigned _tmp144=e->loc;Cyc_Warn_err2(_tmp144,_tag_fat(_tmp4C,sizeof(void*),1));});});
return({struct _tuple12 _tmp11E;({union Cyc_Absyn_Cnst _tmp145=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp11E.f1=_tmp145;}),_tmp11E.f2=1;_tmp11E;});}else{
if(es->tl == 0)
ans=Cyc_Evexp_eval_const_unprimop(p,(struct Cyc_Absyn_Exp*)es->hd);else{
if(((struct Cyc_List_List*)_check_null(es->tl))->tl == 0)
ans=Cyc_Evexp_eval_const_binprimop(p,(struct Cyc_Absyn_Exp*)es->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);else{
# 213
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50=({struct Cyc_Warn_String_Warn_Warg_struct _tmp11F;_tmp11F.tag=0,({struct _fat_ptr _tmp146=({const char*_tmp51="bad static expression (too many args to primop)";_tag_fat(_tmp51,sizeof(char),48U);});_tmp11F.f1=_tmp146;});_tmp11F;});void*_tmp4F[1];_tmp4F[0]=& _tmp50;({unsigned _tmp147=e->loc;Cyc_Warn_err2(_tmp147,_tag_fat(_tmp4F,sizeof(void*),1));});});
return({struct _tuple12 _tmp120;({union Cyc_Absyn_Cnst _tmp148=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp120.f1=_tmp148;}),_tmp120.f2=1;_tmp120;});}}}
# 216
goto _LL0;}case 39: _tmp3B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3A)->f1;{void*t=_tmp3B;
# 219
{void*_tmp52=Cyc_Absyn_compress(t);void*_stmttmpD=_tmp52;void*_tmp53=_stmttmpD;void*_tmp54;if(*((int*)_tmp53)== 9){_tmp54=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp53)->f1;{struct Cyc_Absyn_Exp*e2=_tmp54;
# 221
e->r=e2->r;
return Cyc_Evexp_eval_const_exp(e2);}}else{
goto _LL28;}_LL28:;}
# 225
goto _LL12;}case 17: _LL12:
 goto _LL14;case 18: _LL14:
 goto _LL16;case 19: _LL16:
 ans=({struct _tuple12 _tmp121;({union Cyc_Absyn_Cnst _tmp149=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp121.f1=_tmp149;}),_tmp121.f2=0;_tmp121;});goto _LL0;case 14: _tmp3B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3A)->f1;_tmp3C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3A)->f2;{void*t=_tmp3B;struct Cyc_Absyn_Exp*e2=_tmp3C;
# 231
ans=Cyc_Evexp_eval_const_exp(e2);
if(ans.f2){
struct _tuple16 _tmp55=({struct _tuple16 _tmp124;({void*_tmp14A=Cyc_Absyn_compress(t);_tmp124.f1=_tmp14A;}),_tmp124.f2=ans.f1;_tmp124;});struct _tuple16 _stmttmpE=_tmp55;struct _tuple16 _tmp56=_stmttmpE;void*_tmp57;int _tmp59;short _tmp5A;char _tmp5B;enum Cyc_Absyn_Sign _tmp5D;enum Cyc_Absyn_Size_of _tmp5C;enum Cyc_Absyn_Sign _tmp58;if(*((int*)_tmp56.f1)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f1)){case 1: switch(((_tmp56.f2).Int_c).tag){case 2: _tmp58=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f1)->f1;_tmp5C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f1)->f2;_tmp5D=(((_tmp56.f2).Char_c).val).f1;_tmp5B=(((_tmp56.f2).Char_c).val).f2;{enum Cyc_Absyn_Sign sn=_tmp58;enum Cyc_Absyn_Size_of sz=_tmp5C;enum Cyc_Absyn_Sign sn2=_tmp5D;char x=_tmp5B;
# 236
_tmp58=sn;_tmp5C=sz;_tmp5D=sn2;_tmp5A=(short)x;goto _LL31;}case 4: _tmp58=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f1)->f1;_tmp5C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f1)->f2;_tmp5D=(((_tmp56.f2).Short_c).val).f1;_tmp5A=(((_tmp56.f2).Short_c).val).f2;_LL31: {enum Cyc_Absyn_Sign sn=_tmp58;enum Cyc_Absyn_Size_of sz=_tmp5C;enum Cyc_Absyn_Sign sn2=_tmp5D;short x=_tmp5A;
# 238
_tmp58=sn;_tmp5C=sz;_tmp5D=sn2;_tmp59=(int)x;goto _LL33;}case 5: _tmp58=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f1)->f1;_tmp5C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f1)->f2;_tmp5D=(((_tmp56.f2).Int_c).val).f1;_tmp59=(((_tmp56.f2).Int_c).val).f2;_LL33: {enum Cyc_Absyn_Sign sn=_tmp58;enum Cyc_Absyn_Size_of sz=_tmp5C;enum Cyc_Absyn_Sign sn2=_tmp5D;int x=_tmp59;
# 240
if((int)sn != (int)sn2)
({union Cyc_Absyn_Cnst _tmp14B=Cyc_Absyn_Int_c(sn,x);ans.f1=_tmp14B;});
goto _LL2D;}default: goto _LL3A;}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f2 != 0)switch(((_tmp56.f2).Int_c).tag){case 2: _tmp57=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f2)->hd;_tmp58=(((_tmp56.f2).Char_c).val).f1;_tmp5B=(((_tmp56.f2).Char_c).val).f2;{void*it=_tmp57;enum Cyc_Absyn_Sign sn2=_tmp58;char x=_tmp5B;
# 245
_tmp57=it;_tmp58=sn2;_tmp5A=(short)x;goto _LL37;}case 4: _tmp57=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f2)->hd;_tmp58=(((_tmp56.f2).Short_c).val).f1;_tmp5A=(((_tmp56.f2).Short_c).val).f2;_LL37: {void*it=_tmp57;enum Cyc_Absyn_Sign sn2=_tmp58;short x=_tmp5A;
# 247
_tmp57=it;_tmp58=sn2;_tmp59=(int)x;goto _LL39;}case 5: _tmp57=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56.f1)->f2)->hd;_tmp58=(((_tmp56.f2).Int_c).val).f1;_tmp59=(((_tmp56.f2).Int_c).val).f2;_LL39: {void*it=_tmp57;enum Cyc_Absyn_Sign sn2=_tmp58;int x=_tmp59;
# 249
if(x < 0)
return({struct _tuple12 _tmp122;({union Cyc_Absyn_Cnst _tmp14C=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp122.f1=_tmp14C;}),_tmp122.f2=0;_tmp122;});
({void*_tmp14E=it;Cyc_Unify_unify(_tmp14E,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=9,({struct Cyc_Absyn_Exp*_tmp14D=Cyc_Absyn_const_exp(ans.f1,0U);_tmp5E->f1=_tmp14D;});_tmp5E;}));});
({union Cyc_Absyn_Cnst _tmp14F=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,x);ans.f1=_tmp14F;});
goto _LL2D;}default: goto _LL3A;}else{goto _LL3A;}default: goto _LL3A;}else{_LL3A:
 return({struct _tuple12 _tmp123;({union Cyc_Absyn_Cnst _tmp150=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp123.f1=_tmp150;}),_tmp123.f2=0;_tmp123;});}_LL2D:;}
# 256
goto _LL0;}case 33: _tmp3B=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp3A)->f2;{struct Cyc_Absyn_Enumfield*ef=_tmp3B;
_tmp3B=ef;goto _LL1C;}case 32: _tmp3B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp3A)->f2;_LL1C: {struct Cyc_Absyn_Enumfield*ef=_tmp3B;
ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));goto _LL0;}default:
 return({struct _tuple12 _tmp125;({union Cyc_Absyn_Cnst _tmp151=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp125.f1=_tmp151;}),_tmp125.f2=0;_tmp125;});}_LL0:;}
# 261
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmp5F=ans.f1;{union Cyc_Absyn_Cnst c=_tmp5F;
({void*_tmp152=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=0,_tmp60->f1=c;_tmp60;});e->r=_tmp152;});}}
# 266
return ans;}
# 269
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmp61=e->r;void*_stmttmpF=_tmp61;void*_tmp62=_stmttmpF;enum Cyc_Absyn_Primop _tmp64;void*_tmp66;void*_tmp65;void*_tmp63;switch(*((int*)_tmp62)){case 33:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 17: _LL6:
 goto _LL8;case 18: _LL8:
 goto _LLA;case 19: _LLA:
 goto _LLC;case 0: _LLC:
 return 1;case 6: _tmp63=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62)->f1;_tmp65=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62)->f2;_tmp66=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62)->f3;{struct Cyc_Absyn_Exp*e1=_tmp63;struct Cyc_Absyn_Exp*e2=_tmp65;struct Cyc_Absyn_Exp*e3=_tmp66;
# 278
return(Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2))&& Cyc_Evexp_c_can_eval(e3);}case 7: _tmp63=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62)->f1;_tmp65=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62)->f2;{struct Cyc_Absyn_Exp*e1=_tmp63;struct Cyc_Absyn_Exp*e2=_tmp65;
_tmp63=e1;_tmp65=e2;goto _LL12;}case 8: _tmp63=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62)->f1;_tmp65=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp63;struct Cyc_Absyn_Exp*e2=_tmp65;
return Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2);}case 3: _tmp64=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62)->f1;_tmp63=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62)->f2;{enum Cyc_Absyn_Primop p=_tmp64;struct Cyc_List_List*es=_tmp63;
# 282
for(1;es != 0;es=es->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}case 39:
 return 0;case 14: _tmp63=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62)->f2;{struct Cyc_Absyn_Exp*e=_tmp63;
return Cyc_Evexp_c_can_eval(e);}default:
 return 0;};}
# 293
static struct Cyc_Absyn_Exp*Cyc_Evexp_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp67=e->r;void*_stmttmp10=_tmp67;void*_tmp68=_stmttmp10;void*_tmp6A;void*_tmp69;if(*((int*)_tmp68)== 14){_tmp69=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp68)->f1;_tmp6A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp68)->f2;{void*t=_tmp69;struct Cyc_Absyn_Exp*e2=_tmp6A;
# 296
if(Cyc_Unify_unify(t,Cyc_Absyn_uint_type)){
void*_tmp6B=e2->r;void*_stmttmp11=_tmp6B;void*_tmp6C=_stmttmp11;if(*((int*)_tmp6C)== 39)
return e2;else{
goto _LL5;}_LL5:;}
# 301
return e;}}else{
return e;};}
# 306
static unsigned long long Cyc_Evexp_integral_const_to_long_long(union Cyc_Absyn_Cnst c){
unsigned long long res;
{union Cyc_Absyn_Cnst _tmp6D=c;long long _tmp6E;int _tmp6F;short _tmp70;char _tmp71;switch((_tmp6D.LongLong_c).tag){case 2: _tmp71=((_tmp6D.Char_c).val).f2;{char c=_tmp71;
res=(unsigned long long)c;goto _LL0;}case 4: _tmp70=((_tmp6D.Short_c).val).f2;{short s=_tmp70;
res=(unsigned long long)s;goto _LL0;}case 5: _tmp6F=((_tmp6D.Int_c).val).f2;{int i=_tmp6F;
res=(unsigned long long)i;goto _LL0;}case 6: _tmp6E=((_tmp6D.LongLong_c).val).f2;{long long x=_tmp6E;
res=(unsigned long long)x;goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp73=({struct Cyc_Warn_String_Warn_Warg_struct _tmp126;_tmp126.tag=0,({struct _fat_ptr _tmp153=({const char*_tmp74="Evexp::integral_const_to_long_long";_tag_fat(_tmp74,sizeof(char),35U);});_tmp126.f1=_tmp153;});_tmp126;});void*_tmp72[1];_tmp72[0]=& _tmp73;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp72,sizeof(void*),1));});}_LL0:;}
# 315
return res;}struct _tuple17{union Cyc_Absyn_Cnst f1;union Cyc_Absyn_Cnst f2;};
# 318
static int Cyc_Evexp_cmp_cnst(union Cyc_Absyn_Cnst c1,union Cyc_Absyn_Cnst c2){
struct _tuple17 _tmp75=({struct _tuple17 _tmp127;_tmp127.f1=c1,_tmp127.f2=c2;_tmp127;});struct _tuple17 _stmttmp12=_tmp75;struct _tuple17 _tmp76=_stmttmp12;int _tmp7A;int _tmp78;struct _fat_ptr _tmp79;struct _fat_ptr _tmp77;if(((_tmp76.f1).Null_c).tag == 1){if(((_tmp76.f2).Null_c).tag == 1)
return 0;else{
return -1;}}else{if(((_tmp76.f2).Null_c).tag == 1)
return 1;else{if(((_tmp76.f1).Wchar_c).tag == 3){if(((_tmp76.f2).Wchar_c).tag == 3){_tmp77=((_tmp76.f1).Wchar_c).val;_tmp79=((_tmp76.f2).Wchar_c).val;{struct _fat_ptr s1=_tmp77;struct _fat_ptr s2=_tmp79;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{
return -1;}}else{if(((_tmp76.f2).Wchar_c).tag == 3)
return 1;else{if(((_tmp76.f1).String_c).tag == 8){if(((_tmp76.f2).String_c).tag == 8){_tmp77=((_tmp76.f1).String_c).val;_tmp79=((_tmp76.f2).String_c).val;{struct _fat_ptr s1=_tmp77;struct _fat_ptr s2=_tmp79;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{
return -1;}}else{if(((_tmp76.f2).String_c).tag == 8)
return 1;else{if(((_tmp76.f1).Wstring_c).tag == 9){if(((_tmp76.f2).Wstring_c).tag == 9){_tmp77=((_tmp76.f1).Wstring_c).val;_tmp79=((_tmp76.f2).Wstring_c).val;{struct _fat_ptr s1=_tmp77;struct _fat_ptr s2=_tmp79;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_tmp77=((_tmp76.f1).Wstring_c).val;{struct _fat_ptr s1=_tmp77;
return -1;}}}else{if(((_tmp76.f2).Wstring_c).tag == 9){_tmp77=((_tmp76.f2).Wstring_c).val;{struct _fat_ptr s1=_tmp77;
return 1;}}else{if(((_tmp76.f1).Float_c).tag == 7){if(((_tmp76.f2).Float_c).tag == 7){_tmp77=(((_tmp76.f1).Float_c).val).f1;_tmp78=(((_tmp76.f1).Float_c).val).f2;_tmp79=(((_tmp76.f2).Float_c).val).f1;_tmp7A=(((_tmp76.f2).Float_c).val).f2;{struct _fat_ptr s1=_tmp77;int i1=_tmp78;struct _fat_ptr s2=_tmp79;int i2=_tmp7A;
# 333
if(i1 != i2)return i1 - i2;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{
return -1;}}else{if(((_tmp76.f2).Float_c).tag == 7)
return 1;else{
# 338
unsigned long long i1=Cyc_Evexp_integral_const_to_long_long(c1);
unsigned long long i2=Cyc_Evexp_integral_const_to_long_long(c2);
long long diff=(long long)(i1 - i2);
return diff == 0?0:(diff < 0?-1: 1);}}}}}}}}}};}struct _tuple18{void*f1;void*f2;};
# 351
static int Cyc_Evexp_designator_cmp(void*d1,void*d2){
struct _tuple18 _tmp7B=({struct _tuple18 _tmp128;_tmp128.f1=d1,_tmp128.f2=d2;_tmp128;});struct _tuple18 _stmttmp13=_tmp7B;struct _tuple18 _tmp7C=_stmttmp13;void*_tmp7E;void*_tmp7D;if(*((int*)_tmp7C.f1)== 0){if(*((int*)_tmp7C.f2)== 0){_tmp7D=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp7C.f1)->f1;_tmp7E=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp7C.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp7D;struct Cyc_Absyn_Exp*e2=_tmp7E;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{_tmp7D=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp7C.f1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp7D;
return -1;}}}else{if(*((int*)_tmp7C.f2)== 0){_tmp7D=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp7C.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp7D;
return 1;}}else{_tmp7D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7C.f1)->f1;_tmp7E=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7C.f2)->f1;{struct _fat_ptr*v1=_tmp7D;struct _fat_ptr*v2=_tmp7E;
return Cyc_strcmp((struct _fat_ptr)*v1,(struct _fat_ptr)*v2);}}};}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 360
static int Cyc_Evexp_designator_exp_cmp(struct _tuple19*d1,struct _tuple19*d2){
# 362
struct _tuple19 _tmp7F=*d1;struct _tuple19 _stmttmp14=_tmp7F;struct _tuple19 _tmp80=_stmttmp14;void*_tmp82;void*_tmp81;_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{struct Cyc_List_List*des1=_tmp81;struct Cyc_Absyn_Exp*e1=_tmp82;
struct _tuple19 _tmp83=*d2;struct _tuple19 _stmttmp15=_tmp83;struct _tuple19 _tmp84=_stmttmp15;void*_tmp86;void*_tmp85;_tmp85=_tmp84.f1;_tmp86=_tmp84.f2;{struct Cyc_List_List*des2=_tmp85;struct Cyc_Absyn_Exp*e2=_tmp86;
int _tmp87=Cyc_Evexp_exp_cmp(e1,e2);int c1=_tmp87;
if(c1 != 0)return c1;
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Evexp_designator_cmp,des1,des2);}}}
# 369
static struct Cyc_Absyn_Vardecl*Cyc_Evexp_binding2vardecl(void*b){
void*_tmp88=b;void*_tmp89;switch(*((int*)_tmp88)){case 0:
 return 0;case 1: _tmp89=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp88)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp89;
return vd;}case 2: _tmp89=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp88)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp89;
return fd->fn_vardecl;}case 3: _tmp89=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp88)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp89;
return vd;}case 4: _tmp89=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp88)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp89;
return vd;}default: _tmp89=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp88)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp89;
return vd;}};}struct _tuple20{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};
# 380
static int Cyc_Evexp_cmp_binding(void*b1,void*b2){
struct Cyc_Absyn_Vardecl*_tmp8A=Cyc_Evexp_binding2vardecl(b1);struct Cyc_Absyn_Vardecl*vdopt1=_tmp8A;
struct Cyc_Absyn_Vardecl*_tmp8B=Cyc_Evexp_binding2vardecl(b2);struct Cyc_Absyn_Vardecl*vdopt2=_tmp8B;
struct _tuple20 _tmp8C=({struct _tuple20 _tmp129;_tmp129.f1=vdopt1,_tmp129.f2=vdopt2;_tmp129;});struct _tuple20 _stmttmp16=_tmp8C;struct _tuple20 _tmp8D=_stmttmp16;if(_tmp8D.f1 == 0){if(_tmp8D.f2 == 0)
return({struct _tuple0*_tmp154=Cyc_Absyn_binding2qvar(b1);Cyc_Absyn_qvar_cmp(_tmp154,Cyc_Absyn_binding2qvar(b2));});else{
return -1;}}else{if(_tmp8D.f2 == 0)
return 1;else{
return(int)vdopt1 - (int)vdopt2;}};}
# 393
static unsigned Cyc_Evexp_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmp8E=e->r;void*_stmttmp17=_tmp8E;void*_tmp8F=_stmttmp17;switch(*((int*)_tmp8F)){case 0:
 return 0U;case 1:
 return 1U;case 2:
 return 2U;case 3:
 return 3U;case 4:
 return 4U;case 5:
 return 5U;case 6:
 return 6U;case 7:
 return 7U;case 8:
 return 8U;case 9:
 return 9U;case 10:
 return 10U;case 11:
 return 11U;case 12:
 return 12U;case 13:
 return 13U;case 14:
 return 14U;case 15:
 return 15U;case 16:
 return 16U;case 17:
 return 17U;case 18:
 return 18U;case 19:
 return 19U;case 20:
 return 20U;case 21:
 return 21U;case 22:
 return 22U;case 23:
 return 23U;case 24:
 return 24U;case 25:
 return 25U;case 26:
 return 26U;case 27:
 return 27U;case 28:
 return 28U;case 29:
 return 29U;case 30:
 return 30U;case 31:
 return 31U;case 32:
 return 32U;case 33:
 return 33U;case 34:
 return 34U;case 35:
 return 35U;case 36:
 return 36U;case 37:
 return 37U;case 38:
 return 38U;case 39:
 return 39U;case 40:
 return 40U;case 41:
 return 41U;default:
 return 42U;};}
# 443
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple12 _tmp90=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp18=_tmp90;struct _tuple12 _tmp91=_stmttmp18;int _tmp93;union Cyc_Absyn_Cnst _tmp92;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{union Cyc_Absyn_Cnst c1=_tmp92;int known1=_tmp93;
struct _tuple12 _tmp94=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp19=_tmp94;struct _tuple12 _tmp95=_stmttmp19;int _tmp97;union Cyc_Absyn_Cnst _tmp96;_tmp96=_tmp95.f1;_tmp97=_tmp95.f2;{union Cyc_Absyn_Cnst c2=_tmp96;int known2=_tmp97;
if(known1 && known2)return Cyc_Evexp_cmp_cnst(c1,c2);{
unsigned _tmp98=Cyc_Evexp_exp_case_number(e1);unsigned e1case=_tmp98;
unsigned _tmp99=Cyc_Evexp_exp_case_number(e2);unsigned e2case=_tmp99;
if(e1case != e2case)
return(int)(e1case - e2case);{
struct _tuple18 _tmp9A=({struct _tuple18 _tmp130;_tmp130.f1=e1->r,_tmp130.f2=e2->r;_tmp130;});struct _tuple18 _stmttmp1A=_tmp9A;struct _tuple18 _tmp9B=_stmttmp1A;int _tmpA3;int _tmpA2;enum Cyc_Absyn_Primop _tmpA5;enum Cyc_Absyn_Primop _tmpA4;void*_tmpA1;void*_tmpA0;void*_tmp9F;void*_tmp9E;struct _fat_ptr _tmpA7;struct _fat_ptr _tmpA6;void*_tmp9D;void*_tmp9C;union Cyc_Absyn_Cnst _tmpA9;union Cyc_Absyn_Cnst _tmpA8;switch(*((int*)_tmp9B.f1)){case 0: if(*((int*)_tmp9B.f2)== 0){_tmpA8=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmpA9=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{union Cyc_Absyn_Cnst c1=_tmpA8;union Cyc_Absyn_Cnst c2=_tmpA9;
# 453
return Cyc_Evexp_cmp_cnst(c1,c2);}}else{goto _LL3F;}case 1: if(*((int*)_tmp9B.f2)== 1){_tmp9C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{void*b1=_tmp9C;void*b2=_tmp9D;
return Cyc_Evexp_cmp_binding(b1,b2);}}else{goto _LL3F;}case 2: if(*((int*)_tmp9B.f2)== 2){_tmpA6=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmpA7=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{struct _fat_ptr s1=_tmpA6;struct _fat_ptr s2=_tmpA7;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LL3F;}case 6: if(*((int*)_tmp9B.f2)== 6){_tmp9C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f3;_tmp9F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmpA0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;_tmpA1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f3;{struct Cyc_Absyn_Exp*e11=_tmp9C;struct Cyc_Absyn_Exp*e12=_tmp9D;struct Cyc_Absyn_Exp*e13=_tmp9E;struct Cyc_Absyn_Exp*e21=_tmp9F;struct Cyc_Absyn_Exp*e22=_tmpA0;struct Cyc_Absyn_Exp*e23=_tmpA1;
# 459
int _tmpAA=Cyc_Evexp_exp_cmp(e11,e21);int c1=_tmpAA;
if(c1 != 0)return c1;
c1=Cyc_Evexp_exp_cmp(e12,e22);
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e13,e23);}}else{goto _LL3F;}case 27: if(*((int*)_tmp9B.f2)== 27){_tmp9C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f3;_tmp9F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmpA0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;_tmpA1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f3;{struct Cyc_Absyn_Vardecl*vd1=_tmp9C;struct Cyc_Absyn_Exp*e11=_tmp9D;struct Cyc_Absyn_Exp*e12=_tmp9E;struct Cyc_Absyn_Vardecl*vd2=_tmp9F;struct Cyc_Absyn_Exp*e21=_tmpA0;struct Cyc_Absyn_Exp*e22=_tmpA1;
# 465
if(vd1 != vd2)return(int)vd1 - (int)vd2;
_tmp9C=e11;_tmp9D=e12;_tmp9E=e21;_tmp9F=e22;goto _LL12;}}else{goto _LL3F;}case 7: if(*((int*)_tmp9B.f2)== 7){_tmp9C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;_LL12: {struct Cyc_Absyn_Exp*e11=_tmp9C;struct Cyc_Absyn_Exp*e12=_tmp9D;struct Cyc_Absyn_Exp*e21=_tmp9E;struct Cyc_Absyn_Exp*e22=_tmp9F;
_tmp9C=e11;_tmp9D=e12;_tmp9E=e21;_tmp9F=e22;goto _LL14;}}else{goto _LL3F;}case 8: if(*((int*)_tmp9B.f2)== 8){_tmp9C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;_LL14: {struct Cyc_Absyn_Exp*e11=_tmp9C;struct Cyc_Absyn_Exp*e12=_tmp9D;struct Cyc_Absyn_Exp*e21=_tmp9E;struct Cyc_Absyn_Exp*e22=_tmp9F;
_tmp9C=e11;_tmp9D=e12;_tmp9E=e21;_tmp9F=e22;goto _LL16;}}else{goto _LL3F;}case 9: if(*((int*)_tmp9B.f2)== 9){_tmp9C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;_LL16: {struct Cyc_Absyn_Exp*e11=_tmp9C;struct Cyc_Absyn_Exp*e12=_tmp9D;struct Cyc_Absyn_Exp*e21=_tmp9E;struct Cyc_Absyn_Exp*e22=_tmp9F;
# 470
int _tmpAB=Cyc_Evexp_exp_cmp(e11,e21);int c1=_tmpAB;
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e12,e22);}}else{goto _LL3F;}case 3: if(*((int*)_tmp9B.f2)== 3){_tmpA4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmpA5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;{enum Cyc_Absyn_Primop p1=_tmpA4;struct Cyc_List_List*es1=_tmp9C;enum Cyc_Absyn_Primop p2=_tmpA5;struct Cyc_List_List*es2=_tmp9D;
# 474
int _tmpAC=(int)p1 - (int)p2;int c1=_tmpAC;
if(c1 != 0)return c1;
for(1;es1 != 0 && es2 != 0;(es1=es1->tl,es2=es2->tl)){
int _tmpAD=Cyc_Evexp_exp_cmp((struct Cyc_Absyn_Exp*)es1->hd,(struct Cyc_Absyn_Exp*)es2->hd);int c2=_tmpAD;
if(c2 != 0)
return c2;}
# 481
return 0;}}else{goto _LL3F;}case 17: switch(*((int*)_tmp9B.f2)){case 17: _tmp9C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{void*t1=_tmp9C;void*t2=_tmp9D;
# 483
return Cyc_Tcutil_typecmp(t1,t2);}case 18: _tmp9C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{void*t1=_tmp9C;struct Cyc_Absyn_Exp*e2a=_tmp9D;
# 485
void*_tmpAE=e2a->topt;void*e2atopt=_tmpAE;
if(e2atopt == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12A;_tmp12A.tag=0,({struct _fat_ptr _tmp155=({const char*_tmpB1="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmpB1,sizeof(char),51U);});_tmp12A.f1=_tmp155;});_tmp12A;});void*_tmpAF[1];_tmpAF[0]=& _tmpB0;({unsigned _tmp156=e2->loc;Cyc_Warn_err2(_tmp156,_tag_fat(_tmpAF,sizeof(void*),1));});});
return 0;}
# 490
return Cyc_Tcutil_typecmp(t1,e2atopt);}default: goto _LL3F;}case 18: switch(*((int*)_tmp9B.f2)){case 17: _tmp9C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{struct Cyc_Absyn_Exp*e1a=_tmp9C;void*t2=_tmp9D;
# 492
void*_tmpB2=e1a->topt;void*e1atopt=_tmpB2;
if(e1atopt == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12B;_tmp12B.tag=0,({struct _fat_ptr _tmp157=({const char*_tmpB5="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmpB5,sizeof(char),51U);});_tmp12B.f1=_tmp157;});_tmp12B;});void*_tmpB3[1];_tmpB3[0]=& _tmpB4;({unsigned _tmp158=e1->loc;Cyc_Warn_err2(_tmp158,_tag_fat(_tmpB3,sizeof(void*),1));});});
return 0;}
# 497
return Cyc_Tcutil_typecmp(e1atopt,t2);}case 18: _tmp9C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{struct Cyc_Absyn_Exp*e1a=_tmp9C;struct Cyc_Absyn_Exp*e2a=_tmp9D;
# 499
void*_tmpB6=e1a->topt;void*e1atopt=_tmpB6;
void*_tmpB7=e2a->topt;void*e2atopt=_tmpB7;
if(e1atopt == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12C;_tmp12C.tag=0,({struct _fat_ptr _tmp159=({const char*_tmpBA="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmpBA,sizeof(char),51U);});_tmp12C.f1=_tmp159;});_tmp12C;});void*_tmpB8[1];_tmpB8[0]=& _tmpB9;({unsigned _tmp15A=e1->loc;Cyc_Warn_err2(_tmp15A,_tag_fat(_tmpB8,sizeof(void*),1));});});
return 0;}else{
if(e2atopt == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12D;_tmp12D.tag=0,({struct _fat_ptr _tmp15B=({const char*_tmpBD="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmpBD,sizeof(char),51U);});_tmp12D.f1=_tmp15B;});_tmp12D;});void*_tmpBB[1];_tmpBB[0]=& _tmpBC;({unsigned _tmp15C=e2->loc;Cyc_Warn_err2(_tmp15C,_tag_fat(_tmpBB,sizeof(void*),1));});});
return 0;}}
# 508
return Cyc_Tcutil_typecmp(e1atopt,e2atopt);}default: goto _LL3F;}case 19: if(*((int*)_tmp9B.f2)== 19){_tmp9C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9F=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;{void*t1=_tmp9C;struct Cyc_List_List*f1=_tmp9D;void*t2=_tmp9E;struct Cyc_List_List*f2=_tmp9F;
# 510
int _tmpBE=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmpBE;
if(c1 != 0)return c1;{
int _tmpBF=((int(*)(struct Cyc_List_List*))Cyc_List_length)(f1);int l1=_tmpBF;
int _tmpC0=((int(*)(struct Cyc_List_List*))Cyc_List_length)(f2);int l2=_tmpC0;
if(l1 < l2)return -1;
if(l2 < l1)return 1;
for(1;f1 != 0 && f2 != 0;(f1=f1->tl,f2=f2->tl)){
struct _tuple18 _tmpC1=({struct _tuple18 _tmp12E;_tmp12E.f1=(void*)f1->hd,_tmp12E.f2=(void*)f2->hd;_tmp12E;});struct _tuple18 _stmttmp1B=_tmpC1;struct _tuple18 _tmpC2=_stmttmp1B;unsigned _tmpC4;unsigned _tmpC3;void*_tmpC6;void*_tmpC5;if(*((int*)_tmpC2.f1)== 0){if(*((int*)_tmpC2.f2)== 1)
return -1;else{_tmpC5=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpC2.f1)->f1;_tmpC6=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpC2.f2)->f1;{struct _fat_ptr*fn1=_tmpC5;struct _fat_ptr*fn2=_tmpC6;
# 521
int c=Cyc_strptrcmp(fn1,fn2);
if(c != 0)return c;
goto _LL41;}}}else{if(*((int*)_tmpC2.f2)== 0)
# 519
return 1;else{_tmpC3=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpC2.f1)->f1;_tmpC4=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpC2.f2)->f1;{unsigned i1=_tmpC3;unsigned i2=_tmpC4;
# 525
int c=(int)(i1 - i2);
if(c != 0)return c;
goto _LL41;}}}_LL41:;}
# 529
return 0;}}}else{goto _LL3F;}case 14: if(*((int*)_tmp9B.f2)== 14){_tmp9C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;{void*t1=_tmp9C;struct Cyc_Absyn_Exp*e1a=_tmp9D;void*t2=_tmp9E;struct Cyc_Absyn_Exp*e2a=_tmp9F;
# 531
int _tmpC7=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmpC7;
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e1a,e2a);}}else{goto _LL3F;}case 39: if(*((int*)_tmp9B.f2)== 39){_tmp9C=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{void*t1=_tmp9C;void*t2=_tmp9D;
# 535
if(Cyc_Unify_unify(t1,t2))return 0;
return Cyc_Tcutil_typecmp(t1,t2);}}else{goto _LL3F;}case 41: if(*((int*)_tmp9B.f2)== 41){_tmp9C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp9C;struct Cyc_Absyn_Exp*e2=_tmp9D;
_tmp9C=e1;_tmp9D=e2;goto _LL2A;}}else{goto _LL3F;}case 12: if(*((int*)_tmp9B.f2)== 12){_tmp9C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp9C;struct Cyc_Absyn_Exp*e2=_tmp9D;
# 539
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 13: if(*((int*)_tmp9B.f2)== 13){_tmp9C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;{struct Cyc_Absyn_Exp*e1=_tmp9C;struct Cyc_List_List*t1=_tmp9D;struct Cyc_Absyn_Exp*e2=_tmp9E;struct Cyc_List_List*t2=_tmp9F;
# 541
int _tmpC8=Cyc_Evexp_exp_cmp(e1,e2);int c1=_tmpC8;
if(c1 != 0)return c1;
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,t1,t2);}}else{goto _LL3F;}case 15: if(*((int*)_tmp9B.f2)== 15){_tmp9C=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp9C;struct Cyc_Absyn_Exp*e2=_tmp9D;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 28: if(*((int*)_tmp9B.f2)== 28){_tmp9C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmpA2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f3;_tmp9E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9F=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;_tmpA3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f3;{struct Cyc_Absyn_Exp*e1=_tmp9C;void*t1=_tmp9D;int b1=_tmpA2;struct Cyc_Absyn_Exp*e2=_tmp9E;void*t2=_tmp9F;int b2=_tmpA3;
# 546
int _tmpC9=b1 - b2;int c1=_tmpC9;
if(c1 != 0)return c1;
c1=Cyc_Tcutil_typecmp(t1,t2);
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 30: if(*((int*)_tmp9B.f2)== 30){_tmp9C=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmp9F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;{void*t1=_tmp9C;struct Cyc_List_List*des1=_tmp9D;void*t2=_tmp9E;struct Cyc_List_List*des2=_tmp9F;
# 552
int _tmpCA=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmpCA;
if(c1 != 0)return c1;
_tmp9C=des1;_tmp9D=des2;goto _LL34;}}else{goto _LL3F;}case 26: if(*((int*)_tmp9B.f2)== 26){_tmp9C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_LL34: {struct Cyc_List_List*des1=_tmp9C;struct Cyc_List_List*des2=_tmp9D;
# 556
return((int(*)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Evexp_designator_exp_cmp,des1,des2);}}else{goto _LL3F;}case 29: if(*((int*)_tmp9B.f2)== 29){_tmp9C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f3;_tmp9E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f4;_tmp9F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;_tmpA0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f3;_tmpA1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f4;{struct Cyc_List_List*ts1=_tmp9C;struct Cyc_List_List*des1=_tmp9D;struct Cyc_Absyn_Aggrdecl*ad1=_tmp9E;struct Cyc_List_List*ts2=_tmp9F;struct Cyc_List_List*des2=_tmpA0;struct Cyc_Absyn_Aggrdecl*ad2=_tmpA1;
# 558
int _tmpCB=(int)ad1 - (int)ad2;int c1=_tmpCB;
if(c1 != 0)return c1;
c1=((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);
if(c1 != 0)return c1;
return((int(*)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Evexp_designator_exp_cmp,des1,des1);}}else{goto _LL3F;}case 24: if(*((int*)_tmp9B.f2)== 24){_tmp9C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;{struct Cyc_List_List*es1=_tmp9C;struct Cyc_List_List*es2=_tmp9D;
# 564
return((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Evexp_exp_cmp,es1,es2);}}else{goto _LL3F;}case 31: if(*((int*)_tmp9B.f2)== 31){_tmp9C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f1;_tmp9D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f2;_tmp9E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9B.f1)->f3;_tmp9F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f1;_tmpA0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f2;_tmpA1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9B.f2)->f3;{struct Cyc_List_List*es1=_tmp9C;struct Cyc_Absyn_Datatypedecl*dd1=_tmp9D;struct Cyc_Absyn_Datatypefield*df1=_tmp9E;struct Cyc_List_List*es2=_tmp9F;struct Cyc_Absyn_Datatypedecl*dd2=_tmpA0;struct Cyc_Absyn_Datatypefield*df2=_tmpA1;
# 566
int _tmpCC=(int)dd1 - (int)dd2;int c1=_tmpCC;
if(c1 != 0)return c1;
c1=(int)df1 - (int)df2;
if(c1 != 0)return c1;
return((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Evexp_exp_cmp,es1,es2);}}else{goto _LL3F;}case 32: if(*((int*)_tmp9B.f2)== 32)
# 572
goto _LL3E;else{goto _LL3F;}case 33: if(*((int*)_tmp9B.f2)== 33){_LL3E:
 goto _LL40;}else{goto _LL3F;}default: _LL3F: _LL40:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12F;_tmp12F.tag=0,({struct _fat_ptr _tmp15D=({const char*_tmpCF="Evexp::exp_cmp, unexpected case";_tag_fat(_tmpCF,sizeof(char),32U);});_tmp12F.f1=_tmp15D;});_tmp12F;});void*_tmpCD[1];_tmpCD[0]=& _tmpCE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpCD,sizeof(void*),1));});};}}}}}
# 578
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 580
e1=Cyc_Evexp_strip_cast(e1);
e2=Cyc_Evexp_strip_cast(e2);{
struct _tuple12 _tmpD0=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp1C=_tmpD0;struct _tuple12 _tmpD1=_stmttmp1C;int _tmpD3;union Cyc_Absyn_Cnst _tmpD2;_tmpD2=_tmpD1.f1;_tmpD3=_tmpD1.f2;{union Cyc_Absyn_Cnst c1=_tmpD2;int known1=_tmpD3;
struct _tuple12 _tmpD4=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp1D=_tmpD4;struct _tuple12 _tmpD5=_stmttmp1D;int _tmpD7;union Cyc_Absyn_Cnst _tmpD6;_tmpD6=_tmpD5.f1;_tmpD7=_tmpD5.f2;{union Cyc_Absyn_Cnst c2=_tmpD6;int known2=_tmpD7;
if(known1 && known2){
struct _tuple11 _tmpD8=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple11 _stmttmp1E=_tmpD8;struct _tuple11 _tmpD9=_stmttmp1E;unsigned _tmpDA;_tmpDA=_tmpD9.f1;{unsigned i1=_tmpDA;
struct _tuple11 _tmpDB=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple11 _stmttmp1F=_tmpDB;struct _tuple11 _tmpDC=_stmttmp1F;unsigned _tmpDD;_tmpDD=_tmpDC.f1;{unsigned i2=_tmpDD;
return(int)(i1 - i2);}}}
# 589
return Cyc_Evexp_exp_cmp(e1,e2);}}}}
# 592
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_uint_exp_cmp(e1,e2)== 0;}
# 595
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmpDE=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple11 _stmttmp20=_tmpDE;struct _tuple11 _tmpDF=_stmttmp20;int _tmpE1;unsigned _tmpE0;_tmpE0=_tmpDF.f1;_tmpE1=_tmpDF.f2;{unsigned i1=_tmpE0;int known1=_tmpE1;
struct _tuple11 _tmpE2=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple11 _stmttmp21=_tmpE2;struct _tuple11 _tmpE3=_stmttmp21;int _tmpE5;unsigned _tmpE4;_tmpE4=_tmpE3.f1;_tmpE5=_tmpE3.f2;{unsigned i2=_tmpE4;int known2=_tmpE5;
if(known1 && known2)
return i1 <= i2;
return Cyc_Evexp_same_uint_const_exp(e1,e2);}}}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 605
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmpE6=Cyc_Absyn_compress(t);void*_stmttmp22=_tmpE6;void*_tmpE7=_stmttmp22;void*_tmpE9;void*_tmpE8;switch(*((int*)_tmpE7)){case 0: _tmpE8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE7)->f1;_tmpE9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE7)->f2;{void*c=_tmpE8;struct Cyc_List_List*ts=_tmpE9;
# 608
void*_tmpEA=c;void*_tmpEB;switch(*((int*)_tmpEA)){case 19:
 return 0;case 4:
 goto _LL21;case 1: _LL21:
 goto _LL23;case 2: _LL23:
 goto _LL25;case 17: _LL25:
 goto _LL27;case 15: _LL27:
 goto _LL29;case 3: _LL29:
 goto _LL2B;case 18: _LL2B:
 goto _LL2D;case 16: _LL2D:
 return 1;case 0:
# 619
 goto _LL31;case 5: _LL31:
 goto _LL33;case 7: _LL33:
 goto _LL35;case 6: _LL35:
 goto _LL37;case 9: _LL37:
 goto _LL39;case 8: _LL39:
 goto _LL3B;case 11: _LL3B:
 goto _LL3D;case 12: _LL3D:
 goto _LL3F;case 13: _LL3F:
 goto _LL41;case 14: _LL41:
 goto _LL43;case 10: _LL43:
 return 0;default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpEA)->f1).UnknownAggr).tag == 1)
# 631
({struct Cyc_Warn_String_Warn_Warg_struct _tmpED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp131;_tmp131.tag=0,({struct _fat_ptr _tmp15E=({const char*_tmpEE="szof on unchecked StructType";_tag_fat(_tmpEE,sizeof(char),29U);});_tmp131.f1=_tmp15E;});_tmp131;});void*_tmpEC[1];_tmpEC[0]=& _tmpED;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpEC,sizeof(void*),1));});else{_tmpEB=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpEA)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmpEB;
# 633
if(ad->impl == 0)return 0;{
struct _RegionHandle _tmpEF=_new_region("temp");struct _RegionHandle*temp=& _tmpEF;_push_region(temp);
{struct Cyc_List_List*_tmpF0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(temp,temp,ad->tvs,ts);struct Cyc_List_List*inst=_tmpF0;
{struct Cyc_List_List*_tmpF1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmpF1;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmpF2=0;_npop_handler(0);return _tmpF2;}}}{
int _tmpF3=1;_npop_handler(0);return _tmpF3;}}
# 635
;_pop_region();}}}};}case 2: _tmpE8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpE7)->f1;{struct Cyc_Absyn_Tvar*tv=_tmpE8;
# 641
return(int)(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk))->kind == (int)Cyc_Absyn_BoxKind;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpE7)->f1 != 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpE7)->f1)->v)->kind == Cyc_Absyn_BoxKind)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}case 6: _tmpE8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE7)->f1;{struct Cyc_List_List*tqs=_tmpE8;
# 646
for(1;tqs != 0;tqs=tqs->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple21*)tqs->hd)).f2))
return 0;}
return 1;}case 3:
# 652
 return 1;case 4: _tmpE8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE7)->f1).elt_type;_tmpE9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE7)->f1).num_elts;{void*t2=_tmpE8;struct Cyc_Absyn_Exp*e=_tmpE9;
# 655
return e != 0;}case 5:
 return 0;case 7: _tmpE8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE7)->f2;{struct Cyc_List_List*fs=_tmpE8;
# 658
for(1;fs != 0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}case 10:
 goto _LL16;case 9: _LL16:
 goto _LL18;case 11: _LL18:
 return 0;default: _tmpE8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpE7)->f3;{struct Cyc_Absyn_Typedefdecl*td=_tmpE8;
# 667
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp133;_tmp133.tag=0,({struct _fat_ptr _tmp15F=({const char*_tmpF7="szof typedeftype ";_tag_fat(_tmpF7,sizeof(char),18U);});_tmp133.f1=_tmp15F;});_tmp133;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp132;_tmp132.tag=2,_tmp132.f1=(void*)t;_tmp132;});void*_tmpF4[2];_tmpF4[0]=& _tmpF5,_tmpF4[1]=& _tmpF6;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpF4,sizeof(void*),2));});
return(int)((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind == (int)Cyc_Absyn_BoxKind;}};}

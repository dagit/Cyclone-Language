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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 383
extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 493
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 495
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 882 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 900
void*Cyc_Absyn_compress(void*);
# 919
extern void*Cyc_Absyn_uint_type;
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1157
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 104 "tcutil.h"
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
# 130
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 175
int Cyc_Tcutil_typecmp(void*,void*);
# 181
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 46
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 50
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 53
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple12{union Cyc_Absyn_Cnst f1;int f2;};
# 32 "evexp.cyc"
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 34
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;short _tmp2;enum Cyc_Absyn_Sign _tmp1;char _tmp4;enum Cyc_Absyn_Sign _tmp3;switch((_tmp0.Short_c).tag){case 2U: _LL1: _tmp3=((_tmp0.Char_c).val).f1;_tmp4=((_tmp0.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sn=_tmp3;char c=_tmp4;
return Cyc_Absyn_Int_c(sn,(int)c);}case 4U: _LL3: _tmp1=((_tmp0.Short_c).val).f1;_tmp2=((_tmp0.Short_c).val).f2;_LL4: {enum Cyc_Absyn_Sign sn=_tmp1;short s=_tmp2;
# 38
return Cyc_Absyn_Int_c(sn,(int)s);}default: _LL5: _LL6:
 return cn;}_LL0:;}
# 43
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmp5=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp0=_tmp5;struct _tuple12 _tmp6=_stmttmp0;int _tmp8;union Cyc_Absyn_Cnst _tmp7;_LL1: _tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmp7;int known=_tmp8;
if(!known)
return({struct _tuple11 _tmp186;_tmp186.f1=0U,_tmp186.f2=0;_tmp186;});{
union Cyc_Absyn_Cnst _tmp9=Cyc_Evexp_promote_const(cn);union Cyc_Absyn_Cnst _stmttmp1=_tmp9;union Cyc_Absyn_Cnst _tmpA=_stmttmp1;long long _tmpB;int _tmpC;switch((_tmpA.Null_c).tag){case 5U: _LL4: _tmpC=((_tmpA.Int_c).val).f2;_LL5: {int i=_tmpC;
return({struct _tuple11 _tmp187;_tmp187.f1=(unsigned)i,_tmp187.f2=1;_tmp187;});}case 6U: _LL6: _tmpB=((_tmpA.LongLong_c).val).f2;_LL7: {long long x=_tmpB;
# 51
unsigned long long y=(unsigned long long)x >> (unsigned long long)32;
if(y != (unsigned long long)-1 && y != (unsigned long long)0)
return({struct _tuple11 _tmp188;_tmp188.f1=0U,_tmp188.f2=0;_tmp188;});
return({struct _tuple11 _tmp189;_tmp189.f1=(unsigned)x,_tmp189.f2=1;_tmp189;});}case 7U: _LL8: _LL9:
 return({struct _tuple11 _tmp18A;_tmp18A.f1=0U,_tmp18A.f2=0;_tmp18A;});case 1U: _LLA: _LLB:
 return({struct _tuple11 _tmp18B;_tmp18B.f1=0U,_tmp18B.f2=1;_tmp18B;});default: _LLC: _LLD:
 return({struct _tuple11 _tmp18C;_tmp18C.f1=0U,_tmp18C.f2=0;_tmp18C;});}_LL3:;}}}struct _tuple13{int f1;int f2;};
# 61
static struct _tuple13 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmpD=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp2=_tmpD;struct _tuple12 _tmpE=_stmttmp2;int _tmp10;union Cyc_Absyn_Cnst _tmpF;_LL1: _tmpF=_tmpE.f1;_tmp10=_tmpE.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmpF;int known=_tmp10;
if(!known)
return({struct _tuple13 _tmp18D;_tmp18D.f1=0,_tmp18D.f2=0;_tmp18D;});{
union Cyc_Absyn_Cnst _tmp11=Cyc_Evexp_promote_const(cn);union Cyc_Absyn_Cnst _stmttmp3=_tmp11;union Cyc_Absyn_Cnst _tmp12=_stmttmp3;long long _tmp13;int _tmp14;switch((_tmp12.Float_c).tag){case 5U: _LL4: _tmp14=((_tmp12.Int_c).val).f2;_LL5: {int b=_tmp14;
return({struct _tuple13 _tmp18E;_tmp18E.f1=b != 0,_tmp18E.f2=1;_tmp18E;});}case 6U: _LL6: _tmp13=((_tmp12.LongLong_c).val).f2;_LL7: {long long b=_tmp13;
return({struct _tuple13 _tmp18F;_tmp18F.f1=b != (long long)0,_tmp18F.f2=1;_tmp18F;});}case 1U: _LL8: _LL9:
 return({struct _tuple13 _tmp190;_tmp190.f1=0,_tmp190.f2=0;_tmp190;});case 7U: _LLA: _LLB:
 return({struct _tuple13 _tmp191;_tmp191.f1=0,_tmp191.f2=1;_tmp191;});default: _LLC: _LLD:
 return({struct _tuple13 _tmp192;_tmp192.f1=0,_tmp192.f2=0;_tmp192;});}_LL3:;}}}struct _tuple14{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 74
static struct _tuple12 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmp15=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp4=_tmp15;struct _tuple12 _tmp16=_stmttmp4;int _tmp18;union Cyc_Absyn_Cnst _tmp17;_LL1: _tmp17=_tmp16.f1;_tmp18=_tmp16.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmp17;int known=_tmp18;
if(!known)
return({struct _tuple12 _tmp193;_tmp193.f1=cn,_tmp193.f2=0;_tmp193;});
{struct _tuple14 _tmp19=({struct _tuple14 _tmp195;_tmp195.f1=p,_tmp195.f2=cn;_tmp195;});struct _tuple14 _stmttmp5=_tmp19;struct _tuple14 _tmp1A=_stmttmp5;long long _tmp1B;char _tmp1C;short _tmp1D;int _tmp1E;long long _tmp20;enum Cyc_Absyn_Sign _tmp1F;char _tmp22;enum Cyc_Absyn_Sign _tmp21;short _tmp24;enum Cyc_Absyn_Sign _tmp23;int _tmp26;enum Cyc_Absyn_Sign _tmp25;long long _tmp28;enum Cyc_Absyn_Sign _tmp27;char _tmp2A;enum Cyc_Absyn_Sign _tmp29;short _tmp2C;enum Cyc_Absyn_Sign _tmp2B;int _tmp2E;enum Cyc_Absyn_Sign _tmp2D;switch(_tmp1A.f1){case Cyc_Absyn_Plus: _LL4: _LL5:
 goto _LL3;case Cyc_Absyn_Minus: switch(((_tmp1A.f2).LongLong_c).tag){case 5U: _LL6: _tmp2D=(((_tmp1A.f2).Int_c).val).f1;_tmp2E=(((_tmp1A.f2).Int_c).val).f2;_LL7: {enum Cyc_Absyn_Sign s=_tmp2D;int i=_tmp2E;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 4U: _LL8: _tmp2B=(((_tmp1A.f2).Short_c).val).f1;_tmp2C=(((_tmp1A.f2).Short_c).val).f2;_LL9: {enum Cyc_Absyn_Sign s=_tmp2B;short i=_tmp2C;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 2U: _LLA: _tmp29=(((_tmp1A.f2).Char_c).val).f1;_tmp2A=(((_tmp1A.f2).Char_c).val).f2;_LLB: {enum Cyc_Absyn_Sign s=_tmp29;char i=_tmp2A;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 6U: _LLC: _tmp27=(((_tmp1A.f2).LongLong_c).val).f1;_tmp28=(((_tmp1A.f2).LongLong_c).val).f2;_LLD: {enum Cyc_Absyn_Sign s=_tmp27;long long i=_tmp28;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,- i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Bitnot: switch(((_tmp1A.f2).LongLong_c).tag){case 5U: _LLE: _tmp25=(((_tmp1A.f2).Int_c).val).f1;_tmp26=(((_tmp1A.f2).Int_c).val).f2;_LLF: {enum Cyc_Absyn_Sign s=_tmp25;int i=_tmp26;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 4U: _LL10: _tmp23=(((_tmp1A.f2).Short_c).val).f1;_tmp24=(((_tmp1A.f2).Short_c).val).f2;_LL11: {enum Cyc_Absyn_Sign s=_tmp23;short i=_tmp24;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 2U: _LL12: _tmp21=(((_tmp1A.f2).Char_c).val).f1;_tmp22=(((_tmp1A.f2).Char_c).val).f2;_LL13: {enum Cyc_Absyn_Sign s=_tmp21;char i=_tmp22;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 6U: _LL14: _tmp1F=(((_tmp1A.f2).LongLong_c).val).f1;_tmp20=(((_tmp1A.f2).LongLong_c).val).f2;_LL15: {enum Cyc_Absyn_Sign s=_tmp1F;long long i=_tmp20;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Not: switch(((_tmp1A.f2).LongLong_c).tag){case 5U: _LL16: _tmp1E=(((_tmp1A.f2).Int_c).val).f2;_LL17: {int i=_tmp1E;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i == 0?1: 0);goto _LL3;}case 4U: _LL18: _tmp1D=(((_tmp1A.f2).Short_c).val).f2;_LL19: {short i=_tmp1D;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,(int)i == 0?1: 0);goto _LL3;}case 2U: _LL1A: _tmp1C=(((_tmp1A.f2).Char_c).val).f2;_LL1B: {char i=_tmp1C;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,(int)i == 0?'\001':'\000');goto _LL3;}case 1U: _LL1C: _LL1D:
 cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL3;case 6U: _LL1E: _tmp1B=(((_tmp1A.f2).LongLong_c).val).f2;_LL1F: {long long i=_tmp1B;
# 93
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,(long long)(i == (long long)0?1: 0));goto _LL3;}default: goto _LL20;}default: _LL20: _LL21:
 return({struct _tuple12 _tmp194;_tmp194.f1=cn,_tmp194.f2=0;_tmp194;});}_LL3:;}
# 96
return({struct _tuple12 _tmp196;_tmp196.f1=cn,_tmp196.f2=1;_tmp196;});}}struct _tuple15{enum Cyc_Absyn_Primop f1;int f2;};
# 100
static struct _tuple12 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple12 _tmp2F=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp6=_tmp2F;struct _tuple12 _tmp30=_stmttmp6;int _tmp32;union Cyc_Absyn_Cnst _tmp31;_LL1: _tmp31=_tmp30.f1;_tmp32=_tmp30.f2;_LL2: {union Cyc_Absyn_Cnst cn1=_tmp31;int known1=_tmp32;
struct _tuple12 _tmp33=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp7=_tmp33;struct _tuple12 _tmp34=_stmttmp7;int _tmp36;union Cyc_Absyn_Cnst _tmp35;_LL4: _tmp35=_tmp34.f1;_tmp36=_tmp34.f2;_LL5: {union Cyc_Absyn_Cnst cn2=_tmp35;int known2=_tmp36;
if(!known1 || !known2)
return({struct _tuple12 _tmp197;_tmp197.f1=cn1,_tmp197.f2=0;_tmp197;});
cn1=Cyc_Evexp_promote_const(cn1);
cn2=Cyc_Evexp_promote_const(cn2);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp37=cn1;int _tmp39;enum Cyc_Absyn_Sign _tmp38;if((_tmp37.Int_c).tag == 5){_LL7: _tmp38=((_tmp37.Int_c).val).f1;_tmp39=((_tmp37.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign x=_tmp38;int y=_tmp39;
s1=x;i1=y;goto _LL6;}}else{_LL9: _LLA:
 return({struct _tuple12 _tmp198;_tmp198.f1=cn1,_tmp198.f2=0;_tmp198;});}_LL6:;}
# 113
{union Cyc_Absyn_Cnst _tmp3A=cn2;int _tmp3C;enum Cyc_Absyn_Sign _tmp3B;if((_tmp3A.Int_c).tag == 5){_LLC: _tmp3B=((_tmp3A.Int_c).val).f1;_tmp3C=((_tmp3A.Int_c).val).f2;_LLD: {enum Cyc_Absyn_Sign x=_tmp3B;int y=_tmp3C;
s2=x;i2=y;goto _LLB;}}else{_LLE: _LLF:
 return({struct _tuple12 _tmp199;_tmp199.f1=cn1,_tmp199.f2=0;_tmp199;});}_LLB:;}
# 117
{enum Cyc_Absyn_Primop _tmp3D=p;switch(_tmp3D){case Cyc_Absyn_Div: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Mod: _LL13: _LL14:
# 120
 if(i2 != 0)
goto _LL10;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp19A;_tmp19A.tag=0U,({struct _fat_ptr _tmp1C2=({const char*_tmp40="division by zero in constant expression";_tag_fat(_tmp40,sizeof(char),40U);});_tmp19A.f1=_tmp1C2;});_tmp19A;});void*_tmp3E[1U];_tmp3E[0]=& _tmp3F;({unsigned _tmp1C3=e2->loc;Cyc_Warn_err2(_tmp1C3,_tag_fat(_tmp3E,sizeof(void*),1U));});});
return({struct _tuple12 _tmp19B;_tmp19B.f1=cn1,_tmp19B.f2=1;_tmp19B;});default: _LL15: _LL16:
 goto _LL10;}_LL10:;}{
# 126
int has_u_arg=(int)s1 == (int)1U ||(int)s2 == (int)Cyc_Absyn_Unsigned;
unsigned u1=(unsigned)i1;
unsigned u2=(unsigned)i2;
int i3=0;
unsigned u3=0U;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple15 _tmp41=({struct _tuple15 _tmp19E;_tmp19E.f1=p,_tmp19E.f2=has_u_arg;_tmp19E;});struct _tuple15 _stmttmp8=_tmp41;struct _tuple15 _tmp42=_stmttmp8;switch(_tmp42.f1){case Cyc_Absyn_Plus: switch(_tmp42.f2){case 0U: _LL18: _LL19:
 i3=i1 + i2;use_i3=1;goto _LL17;case 1U: _LL22: _LL23:
# 141
 u3=u1 + u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Times: switch(_tmp42.f2){case 0U: _LL1A: _LL1B:
# 137
 i3=i1 * i2;use_i3=1;goto _LL17;case 1U: _LL24: _LL25:
# 142
 u3=u1 * u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Minus: switch(_tmp42.f2){case 0U: _LL1C: _LL1D:
# 138
 i3=i1 - i2;use_i3=1;goto _LL17;case 1U: _LL26: _LL27:
# 143
 u3=u1 - u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Div: switch(_tmp42.f2){case 0U: _LL1E: _LL1F:
# 139
 i3=i1 / i2;use_i3=1;goto _LL17;case 1U: _LL28: _LL29:
# 144
 u3=u1 / u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Mod: switch(_tmp42.f2){case 0U: _LL20: _LL21:
# 140
 i3=i1 % i2;use_i3=1;goto _LL17;case 1U: _LL2A: _LL2B:
# 145
 u3=u1 % u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Eq: _LL2C: _LL2D:
 b3=i1 == i2;use_b3=1;goto _LL17;case Cyc_Absyn_Neq: _LL2E: _LL2F:
 b3=i1 != i2;use_b3=1;goto _LL17;case Cyc_Absyn_Gt: switch(_tmp42.f2){case 0U: _LL30: _LL31:
 b3=i1 > i2;use_b3=1;goto _LL17;case 1U: _LL38: _LL39:
# 152
 b3=u1 > u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lt: switch(_tmp42.f2){case 0U: _LL32: _LL33:
# 149
 b3=i1 < i2;use_b3=1;goto _LL17;case 1U: _LL3A: _LL3B:
# 153
 b3=u1 < u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Gte: switch(_tmp42.f2){case 0U: _LL34: _LL35:
# 150
 b3=i1 >= i2;use_b3=1;goto _LL17;case 1U: _LL3C: _LL3D:
# 154
 b3=u1 >= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lte: switch(_tmp42.f2){case 0U: _LL36: _LL37:
# 151
 b3=i1 <= i2;use_b3=1;goto _LL17;case 1U: _LL3E: _LL3F:
# 155
 b3=u1 <= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Bitand: _LL40: _LL41:
 u3=u1 & u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitor: _LL42: _LL43:
 u3=u1 | u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitxor: _LL44: _LL45:
 u3=u1 ^ u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlshift: _LL46: _LL47:
 u3=u1 << u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlrshift: _LL48: _LL49:
 u3=u1 >> u2;use_u3=1;goto _LL17;default: _LL4A: _LL4B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44=({struct Cyc_Warn_String_Warn_Warg_struct _tmp19C;_tmp19C.tag=0U,({struct _fat_ptr _tmp1C4=({const char*_tmp45="bad constant expression";_tag_fat(_tmp45,sizeof(char),24U);});_tmp19C.f1=_tmp1C4;});_tmp19C;});void*_tmp43[1U];_tmp43[0]=& _tmp44;({unsigned _tmp1C5=e1->loc;Cyc_Warn_err2(_tmp1C5,_tag_fat(_tmp43,sizeof(void*),1U));});});return({struct _tuple12 _tmp19D;_tmp19D.f1=cn1,_tmp19D.f2=1;_tmp19D;});}_LL17:;}
# 163
if(use_i3)return({struct _tuple12 _tmp19F;({union Cyc_Absyn_Cnst _tmp1C6=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3);_tmp19F.f1=_tmp1C6;}),_tmp19F.f2=1;_tmp19F;});
if(use_u3)return({struct _tuple12 _tmp1A0;({union Cyc_Absyn_Cnst _tmp1C7=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3);_tmp1A0.f1=_tmp1C7;}),_tmp1A0.f2=1;_tmp1A0;});
if(use_b3)return({struct _tuple12 _tmp1A1;({union Cyc_Absyn_Cnst _tmp1C8=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0);_tmp1A1.f1=_tmp1C8;}),_tmp1A1.f2=1;_tmp1A1;});
({struct Cyc_Warn_String_Warn_Warg_struct _tmp47=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A2;_tmp1A2.tag=0U,({struct _fat_ptr _tmp1C9=({const char*_tmp48="Evexp::eval_const_binop";_tag_fat(_tmp48,sizeof(char),24U);});_tmp1A2.f1=_tmp1C9;});_tmp1A2;});void*_tmp46[1U];_tmp46[0]=& _tmp47;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp46,sizeof(void*),1U));});}}}}}struct _tuple16{void*f1;union Cyc_Absyn_Cnst f2;};
# 171
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 ans;
{void*_tmp49=e->r;void*_stmttmp9=_tmp49;void*_tmp4A=_stmttmp9;struct Cyc_Absyn_Enumfield*_tmp4B;struct Cyc_Absyn_Enumfield*_tmp4C;struct Cyc_Absyn_Exp*_tmp4E;void*_tmp4D;void*_tmp4F;struct Cyc_List_List*_tmp51;enum Cyc_Absyn_Primop _tmp50;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Exp*_tmp56;union Cyc_Absyn_Cnst _tmp59;switch(*((int*)_tmp4A)){case 1U: _LL1: _LL2:
# 175
 return({struct _tuple12 _tmp1A3;({union Cyc_Absyn_Cnst _tmp1CA=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A3.f1=_tmp1CA;}),_tmp1A3.f2=0;_tmp1A3;});case 0U: _LL3: _tmp59=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4A)->f1;_LL4: {union Cyc_Absyn_Cnst c=_tmp59;
return({struct _tuple12 _tmp1A4;_tmp1A4.f1=c,_tmp1A4.f2=1;_tmp1A4;});}case 2U: _LL5: _LL6:
 return({struct _tuple12 _tmp1A5;({union Cyc_Absyn_Cnst _tmp1CB=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A5.f1=_tmp1CB;}),_tmp1A5.f2=1;_tmp1A5;});case 6U: _LL7: _tmp56=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4A)->f1;_tmp57=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4A)->f2;_tmp58=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4A)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp56;struct Cyc_Absyn_Exp*e2=_tmp57;struct Cyc_Absyn_Exp*e3=_tmp58;
# 179
struct _tuple13 _tmp5A=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpA=_tmp5A;struct _tuple13 _tmp5B=_stmttmpA;int _tmp5D;int _tmp5C;_LL20: _tmp5C=_tmp5B.f1;_tmp5D=_tmp5B.f2;_LL21: {int bool1=_tmp5C;int known1=_tmp5D;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
Cyc_Evexp_eval_const_exp(e3);
return({struct _tuple12 _tmp1A6;({union Cyc_Absyn_Cnst _tmp1CC=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A6.f1=_tmp1CC;}),_tmp1A6.f2=0;_tmp1A6;});}
# 185
ans=bool1?Cyc_Evexp_eval_const_exp(e2): Cyc_Evexp_eval_const_exp(e3);
goto _LL0;}}case 7U: _LL9: _tmp54=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4A)->f1;_tmp55=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4A)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp54;struct Cyc_Absyn_Exp*e2=_tmp55;
# 188
struct _tuple13 _tmp5E=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpB=_tmp5E;struct _tuple13 _tmp5F=_stmttmpB;int _tmp61;int _tmp60;_LL23: _tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;_LL24: {int bool1=_tmp60;int known1=_tmp61;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple12 _tmp1A7;({union Cyc_Absyn_Cnst _tmp1CD=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A7.f1=_tmp1CD;}),_tmp1A7.f2=0;_tmp1A7;});}
# 193
ans=bool1?Cyc_Evexp_eval_const_exp(e2):({struct _tuple12 _tmp1A8;({union Cyc_Absyn_Cnst _tmp1CE=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A8.f1=_tmp1CE;}),_tmp1A8.f2=1;_tmp1A8;});
goto _LL0;}}case 8U: _LLB: _tmp52=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4A)->f1;_tmp53=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4A)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp52;struct Cyc_Absyn_Exp*e2=_tmp53;
# 196
struct _tuple13 _tmp62=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpC=_tmp62;struct _tuple13 _tmp63=_stmttmpC;int _tmp65;int _tmp64;_LL26: _tmp64=_tmp63.f1;_tmp65=_tmp63.f2;_LL27: {int bool1=_tmp64;int known1=_tmp65;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple12 _tmp1A9;({union Cyc_Absyn_Cnst _tmp1CF=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A9.f1=_tmp1CF;}),_tmp1A9.f2=0;_tmp1A9;});}
# 201
ans=bool1?({struct _tuple12 _tmp1AA;({union Cyc_Absyn_Cnst _tmp1D0=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);_tmp1AA.f1=_tmp1D0;}),_tmp1AA.f2=1;_tmp1AA;}): Cyc_Evexp_eval_const_exp(e2);
goto _LL0;}}case 3U: _LLD: _tmp50=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A)->f1;_tmp51=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp50;struct Cyc_List_List*es=_tmp51;
# 204
if(es == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AB;_tmp1AB.tag=0U,({struct _fat_ptr _tmp1D1=({const char*_tmp68="bad static expression (no args to primop)";_tag_fat(_tmp68,sizeof(char),42U);});_tmp1AB.f1=_tmp1D1;});_tmp1AB;});void*_tmp66[1U];_tmp66[0]=& _tmp67;({unsigned _tmp1D2=e->loc;Cyc_Warn_err2(_tmp1D2,_tag_fat(_tmp66,sizeof(void*),1U));});});
return({struct _tuple12 _tmp1AC;({union Cyc_Absyn_Cnst _tmp1D3=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1AC.f1=_tmp1D3;}),_tmp1AC.f2=1;_tmp1AC;});}else{
if(es->tl == 0)
ans=Cyc_Evexp_eval_const_unprimop(p,(struct Cyc_Absyn_Exp*)es->hd);else{
if(((struct Cyc_List_List*)_check_null(es->tl))->tl == 0)
ans=Cyc_Evexp_eval_const_binprimop(p,(struct Cyc_Absyn_Exp*)es->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);else{
# 212
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AD;_tmp1AD.tag=0U,({struct _fat_ptr _tmp1D4=({const char*_tmp6B="bad static expression (too many args to primop)";_tag_fat(_tmp6B,sizeof(char),48U);});_tmp1AD.f1=_tmp1D4;});_tmp1AD;});void*_tmp69[1U];_tmp69[0]=& _tmp6A;({unsigned _tmp1D5=e->loc;Cyc_Warn_err2(_tmp1D5,_tag_fat(_tmp69,sizeof(void*),1U));});});
return({struct _tuple12 _tmp1AE;({union Cyc_Absyn_Cnst _tmp1D6=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1AE.f1=_tmp1D6;}),_tmp1AE.f2=1;_tmp1AE;});}}}
# 215
goto _LL0;}case 39U: _LLF: _tmp4F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4A)->f1;_LL10: {void*t=_tmp4F;
# 218
{void*_tmp6C=Cyc_Absyn_compress(t);void*_stmttmpD=_tmp6C;void*_tmp6D=_stmttmpD;struct Cyc_Absyn_Exp*_tmp6E;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->tag == 9U){_LL29: _tmp6E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL2A: {struct Cyc_Absyn_Exp*e2=_tmp6E;
# 220
e->r=e2->r;
return Cyc_Evexp_eval_const_exp(e2);}}else{_LL2B: _LL2C:
 goto _LL28;}_LL28:;}
# 224
goto _LL12;}case 17U: _LL11: _LL12:
 goto _LL14;case 18U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 ans=({struct _tuple12 _tmp1AF;({union Cyc_Absyn_Cnst _tmp1D7=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1AF.f1=_tmp1D7;}),_tmp1AF.f2=0;_tmp1AF;});goto _LL0;case 14U: _LL17: _tmp4D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A)->f1;_tmp4E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A)->f2;_LL18: {void*t=_tmp4D;struct Cyc_Absyn_Exp*e2=_tmp4E;
# 230
ans=Cyc_Evexp_eval_const_exp(e2);
if(ans.f2){
struct _tuple16 _tmp6F=({struct _tuple16 _tmp1B2;({void*_tmp1D8=Cyc_Absyn_compress(t);_tmp1B2.f1=_tmp1D8;}),_tmp1B2.f2=ans.f1;_tmp1B2;});struct _tuple16 _stmttmpE=_tmp6F;struct _tuple16 _tmp70=_stmttmpE;int _tmp73;enum Cyc_Absyn_Sign _tmp72;void*_tmp71;short _tmp76;enum Cyc_Absyn_Sign _tmp75;void*_tmp74;char _tmp79;enum Cyc_Absyn_Sign _tmp78;void*_tmp77;int _tmp7D;enum Cyc_Absyn_Sign _tmp7C;enum Cyc_Absyn_Size_of _tmp7B;enum Cyc_Absyn_Sign _tmp7A;short _tmp81;enum Cyc_Absyn_Sign _tmp80;enum Cyc_Absyn_Size_of _tmp7F;enum Cyc_Absyn_Sign _tmp7E;char _tmp85;enum Cyc_Absyn_Sign _tmp84;enum Cyc_Absyn_Size_of _tmp83;enum Cyc_Absyn_Sign _tmp82;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)){case 1U: switch(((_tmp70.f2).Int_c).tag){case 2U: _LL2E: _tmp82=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f1;_tmp83=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f2;_tmp84=(((_tmp70.f2).Char_c).val).f1;_tmp85=(((_tmp70.f2).Char_c).val).f2;_LL2F: {enum Cyc_Absyn_Sign sn=_tmp82;enum Cyc_Absyn_Size_of sz=_tmp83;enum Cyc_Absyn_Sign sn2=_tmp84;char x=_tmp85;
# 235
_tmp7E=sn;_tmp7F=sz;_tmp80=sn2;_tmp81=(short)x;goto _LL31;}case 4U: _LL30: _tmp7E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f1;_tmp7F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f2;_tmp80=(((_tmp70.f2).Short_c).val).f1;_tmp81=(((_tmp70.f2).Short_c).val).f2;_LL31: {enum Cyc_Absyn_Sign sn=_tmp7E;enum Cyc_Absyn_Size_of sz=_tmp7F;enum Cyc_Absyn_Sign sn2=_tmp80;short x=_tmp81;
# 237
_tmp7A=sn;_tmp7B=sz;_tmp7C=sn2;_tmp7D=(int)x;goto _LL33;}case 5U: _LL32: _tmp7A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f1;_tmp7B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f2;_tmp7C=(((_tmp70.f2).Int_c).val).f1;_tmp7D=(((_tmp70.f2).Int_c).val).f2;_LL33: {enum Cyc_Absyn_Sign sn=_tmp7A;enum Cyc_Absyn_Size_of sz=_tmp7B;enum Cyc_Absyn_Sign sn2=_tmp7C;int x=_tmp7D;
# 239
if((int)sn != (int)sn2)
({union Cyc_Absyn_Cnst _tmp1D9=Cyc_Absyn_Int_c(sn,x);ans.f1=_tmp1D9;});
goto _LL2D;}default: goto _LL3A;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f2 != 0)switch(((_tmp70.f2).Int_c).tag){case 2U: _LL34: _tmp77=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f2)->hd;_tmp78=(((_tmp70.f2).Char_c).val).f1;_tmp79=(((_tmp70.f2).Char_c).val).f2;_LL35: {void*it=_tmp77;enum Cyc_Absyn_Sign sn2=_tmp78;char x=_tmp79;
# 244
_tmp74=it;_tmp75=sn2;_tmp76=(short)x;goto _LL37;}case 4U: _LL36: _tmp74=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f2)->hd;_tmp75=(((_tmp70.f2).Short_c).val).f1;_tmp76=(((_tmp70.f2).Short_c).val).f2;_LL37: {void*it=_tmp74;enum Cyc_Absyn_Sign sn2=_tmp75;short x=_tmp76;
# 246
_tmp71=it;_tmp72=sn2;_tmp73=(int)x;goto _LL39;}case 5U: _LL38: _tmp71=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f2)->hd;_tmp72=(((_tmp70.f2).Int_c).val).f1;_tmp73=(((_tmp70.f2).Int_c).val).f2;_LL39: {void*it=_tmp71;enum Cyc_Absyn_Sign sn2=_tmp72;int x=_tmp73;
# 248
if(x < 0)
return({struct _tuple12 _tmp1B0;({union Cyc_Absyn_Cnst _tmp1DA=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1B0.f1=_tmp1DA;}),_tmp1B0.f2=0;_tmp1B0;});
({void*_tmp1DC=it;Cyc_Unify_unify(_tmp1DC,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=9U,({struct Cyc_Absyn_Exp*_tmp1DB=Cyc_Absyn_const_exp(ans.f1,0U);_tmp86->f1=_tmp1DB;});_tmp86;}));});
({union Cyc_Absyn_Cnst _tmp1DD=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,x);ans.f1=_tmp1DD;});
goto _LL2D;}default: goto _LL3A;}else{goto _LL3A;}default: goto _LL3A;}else{_LL3A: _LL3B:
 return({struct _tuple12 _tmp1B1;({union Cyc_Absyn_Cnst _tmp1DE=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1B1.f1=_tmp1DE;}),_tmp1B1.f2=0;_tmp1B1;});}_LL2D:;}
# 255
goto _LL0;}case 33U: _LL19: _tmp4C=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4A)->f2;_LL1A: {struct Cyc_Absyn_Enumfield*ef=_tmp4C;
_tmp4B=ef;goto _LL1C;}case 32U: _LL1B: _tmp4B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4A)->f2;_LL1C: {struct Cyc_Absyn_Enumfield*ef=_tmp4B;
ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));goto _LL0;}default: _LL1D: _LL1E:
 return({struct _tuple12 _tmp1B3;({union Cyc_Absyn_Cnst _tmp1DF=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1B3.f1=_tmp1DF;}),_tmp1B3.f2=0;_tmp1B3;});}_LL0:;}
# 260
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmp87=ans.f1;{union Cyc_Absyn_Cnst c=_tmp87;
({void*_tmp1E0=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=0U,_tmp88->f1=c;_tmp88;});e->r=_tmp1E0;});}}
# 265
return ans;}
# 268
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmp89=e->r;void*_stmttmpF=_tmp89;void*_tmp8A=_stmttmpF;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_List_List*_tmp8D;enum Cyc_Absyn_Primop _tmp8C;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp92;switch(*((int*)_tmp8A)){case 33U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 0U: _LLB: _LLC:
 return 1;case 6U: _LLD: _tmp92=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A)->f1;_tmp93=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_tmp94=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp92;struct Cyc_Absyn_Exp*e2=_tmp93;struct Cyc_Absyn_Exp*e3=_tmp94;
# 277
return(Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2))&& Cyc_Evexp_c_can_eval(e3);}case 7U: _LLF: _tmp90=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8A)->f1;_tmp91=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp90;struct Cyc_Absyn_Exp*e2=_tmp91;
_tmp8E=e1;_tmp8F=e2;goto _LL12;}case 8U: _LL11: _tmp8E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8A)->f1;_tmp8F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp8E;struct Cyc_Absyn_Exp*e2=_tmp8F;
return Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2);}case 3U: _LL13: _tmp8C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8A)->f1;_tmp8D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL14: {enum Cyc_Absyn_Primop p=_tmp8C;struct Cyc_List_List*es=_tmp8D;
# 281
for(0;es != 0;es=es->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}case 39U: _LL15: _LL16:
 return 0;case 14U: _LL17: _tmp8B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL18: {struct Cyc_Absyn_Exp*e=_tmp8B;
return Cyc_Evexp_c_can_eval(e);}default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 292
static struct Cyc_Absyn_Exp*Cyc_Evexp_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp95=e->r;void*_stmttmp10=_tmp95;void*_tmp96=_stmttmp10;struct Cyc_Absyn_Exp*_tmp98;void*_tmp97;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96)->tag == 14U){_LL1: _tmp97=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96)->f1;_tmp98=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL2: {void*t=_tmp97;struct Cyc_Absyn_Exp*e2=_tmp98;
# 295
if(Cyc_Unify_unify(t,Cyc_Absyn_uint_type)){
void*_tmp99=e2->r;void*_stmttmp11=_tmp99;void*_tmp9A=_stmttmp11;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp9A)->tag == 39U){_LL6: _LL7:
 return e2;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 300
return e;}}else{_LL3: _LL4:
 return e;}_LL0:;}
# 305
static unsigned long long Cyc_Evexp_integral_const_to_long_long(union Cyc_Absyn_Cnst c){
unsigned long long res;
{union Cyc_Absyn_Cnst _tmp9B=c;long long _tmp9C;int _tmp9D;short _tmp9E;char _tmp9F;switch((_tmp9B.LongLong_c).tag){case 2U: _LL1: _tmp9F=((_tmp9B.Char_c).val).f2;_LL2: {char c=_tmp9F;
res=(unsigned long long)c;goto _LL0;}case 4U: _LL3: _tmp9E=((_tmp9B.Short_c).val).f2;_LL4: {short s=_tmp9E;
res=(unsigned long long)s;goto _LL0;}case 5U: _LL5: _tmp9D=((_tmp9B.Int_c).val).f2;_LL6: {int i=_tmp9D;
res=(unsigned long long)i;goto _LL0;}case 6U: _LL7: _tmp9C=((_tmp9B.LongLong_c).val).f2;_LL8: {long long x=_tmp9C;
res=(unsigned long long)x;goto _LL0;}default: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B4;_tmp1B4.tag=0U,({struct _fat_ptr _tmp1E1=({const char*_tmpA2="Evexp::integral_const_to_long_long";_tag_fat(_tmpA2,sizeof(char),35U);});_tmp1B4.f1=_tmp1E1;});_tmp1B4;});void*_tmpA0[1U];_tmpA0[0]=& _tmpA1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpA0,sizeof(void*),1U));});}_LL0:;}
# 314
return res;}struct _tuple17{union Cyc_Absyn_Cnst f1;union Cyc_Absyn_Cnst f2;};
# 317
static int Cyc_Evexp_cmp_cnst(union Cyc_Absyn_Cnst c1,union Cyc_Absyn_Cnst c2){
struct _tuple17 _tmpA3=({struct _tuple17 _tmp1B5;_tmp1B5.f1=c1,_tmp1B5.f2=c2;_tmp1B5;});struct _tuple17 _stmttmp12=_tmpA3;struct _tuple17 _tmpA4=_stmttmp12;int _tmpA8;struct _fat_ptr _tmpA7;int _tmpA6;struct _fat_ptr _tmpA5;struct _fat_ptr _tmpA9;struct _fat_ptr _tmpAA;struct _fat_ptr _tmpAC;struct _fat_ptr _tmpAB;struct _fat_ptr _tmpAE;struct _fat_ptr _tmpAD;struct _fat_ptr _tmpB0;struct _fat_ptr _tmpAF;if(((_tmpA4.f1).Null_c).tag == 1){if(((_tmpA4.f2).Null_c).tag == 1){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((_tmpA4.f2).Null_c).tag == 1){_LL5: _LL6:
 return 1;}else{if(((_tmpA4.f1).Wchar_c).tag == 3){if(((_tmpA4.f2).Wchar_c).tag == 3){_LL7: _tmpAF=((_tmpA4.f1).Wchar_c).val;_tmpB0=((_tmpA4.f2).Wchar_c).val;_LL8: {struct _fat_ptr s1=_tmpAF;struct _fat_ptr s2=_tmpB0;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_LL9: _LLA:
 return - 1;}}else{if(((_tmpA4.f2).Wchar_c).tag == 3){_LLB: _LLC:
 return 1;}else{if(((_tmpA4.f1).String_c).tag == 8){if(((_tmpA4.f2).String_c).tag == 8){_LLD: _tmpAD=((_tmpA4.f1).String_c).val;_tmpAE=((_tmpA4.f2).String_c).val;_LLE: {struct _fat_ptr s1=_tmpAD;struct _fat_ptr s2=_tmpAE;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_LLF: _LL10:
 return - 1;}}else{if(((_tmpA4.f2).String_c).tag == 8){_LL11: _LL12:
 return 1;}else{if(((_tmpA4.f1).Wstring_c).tag == 9){if(((_tmpA4.f2).Wstring_c).tag == 9){_LL13: _tmpAB=((_tmpA4.f1).Wstring_c).val;_tmpAC=((_tmpA4.f2).Wstring_c).val;_LL14: {struct _fat_ptr s1=_tmpAB;struct _fat_ptr s2=_tmpAC;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_LL15: _tmpAA=((_tmpA4.f1).Wstring_c).val;_LL16: {struct _fat_ptr s1=_tmpAA;
return - 1;}}}else{if(((_tmpA4.f2).Wstring_c).tag == 9){_LL17: _tmpA9=((_tmpA4.f2).Wstring_c).val;_LL18: {struct _fat_ptr s1=_tmpA9;
return 1;}}else{if(((_tmpA4.f1).Float_c).tag == 7){if(((_tmpA4.f2).Float_c).tag == 7){_LL19: _tmpA5=(((_tmpA4.f1).Float_c).val).f1;_tmpA6=(((_tmpA4.f1).Float_c).val).f2;_tmpA7=(((_tmpA4.f2).Float_c).val).f1;_tmpA8=(((_tmpA4.f2).Float_c).val).f2;_LL1A: {struct _fat_ptr s1=_tmpA5;int i1=_tmpA6;struct _fat_ptr s2=_tmpA7;int i2=_tmpA8;
# 332
if(i1 != i2)return i1 - i2;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_LL1B: _LL1C:
 return - 1;}}else{if(((_tmpA4.f2).Float_c).tag == 7){_LL1D: _LL1E:
 return 1;}else{_LL1F: _LL20: {
# 337
unsigned long long i1=Cyc_Evexp_integral_const_to_long_long(c1);
unsigned long long i2=Cyc_Evexp_integral_const_to_long_long(c2);
long long diff=(long long)(i1 - i2);
return diff == (long long)0?0:(diff < (long long)0?- 1: 1);}}}}}}}}}}}_LL0:;}struct _tuple18{void*f1;void*f2;};
# 350
static int Cyc_Evexp_designator_cmp(void*d1,void*d2){
struct _tuple18 _tmpB1=({struct _tuple18 _tmp1B6;_tmp1B6.f1=d1,_tmp1B6.f2=d2;_tmp1B6;});struct _tuple18 _stmttmp13=_tmpB1;struct _tuple18 _tmpB2=_stmttmp13;struct _fat_ptr*_tmpB4;struct _fat_ptr*_tmpB3;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Exp*_tmpB7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2.f1)->tag == 0U){if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2.f2)->tag == 0U){_LL1: _tmpB7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2.f1)->f1;_tmpB8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2.f2)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmpB7;struct Cyc_Absyn_Exp*e2=_tmpB8;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{_LL3: _tmpB6=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2.f1)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmpB6;
return - 1;}}}else{if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2.f2)->tag == 0U){_LL5: _tmpB5=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2.f2)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmpB5;
return 1;}}else{_LL7: _tmpB3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB2.f1)->f1;_tmpB4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB2.f2)->f1;_LL8: {struct _fat_ptr*v1=_tmpB3;struct _fat_ptr*v2=_tmpB4;
return Cyc_strcmp((struct _fat_ptr)*v1,(struct _fat_ptr)*v2);}}}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 359
static int Cyc_Evexp_designator_exp_cmp(struct _tuple19*d1,struct _tuple19*d2){
# 361
struct _tuple19 _tmpB9=*d1;struct _tuple19 _stmttmp14=_tmpB9;struct _tuple19 _tmpBA=_stmttmp14;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_List_List*_tmpBB;_LL1: _tmpBB=_tmpBA.f1;_tmpBC=_tmpBA.f2;_LL2: {struct Cyc_List_List*des1=_tmpBB;struct Cyc_Absyn_Exp*e1=_tmpBC;
struct _tuple19 _tmpBD=*d2;struct _tuple19 _stmttmp15=_tmpBD;struct _tuple19 _tmpBE=_stmttmp15;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_List_List*_tmpBF;_LL4: _tmpBF=_tmpBE.f1;_tmpC0=_tmpBE.f2;_LL5: {struct Cyc_List_List*des2=_tmpBF;struct Cyc_Absyn_Exp*e2=_tmpC0;
int _tmpC1=Cyc_Evexp_exp_cmp(e1,e2);int c1=_tmpC1;
if(c1 != 0)return c1;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_designator_cmp,des1,des2);}}}
# 368
static struct Cyc_Absyn_Vardecl*Cyc_Evexp_binding2vardecl(void*b){
void*_tmpC2=b;struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC4;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Fndecl*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;switch(*((int*)_tmpC2)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _tmpC7=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC2)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpC7;
return vd;}case 2U: _LL5: _tmpC6=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC2)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmpC6;
return fd->fn_vardecl;}case 3U: _LL7: _tmpC5=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC2)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpC5;
return vd;}case 4U: _LL9: _tmpC4=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC2)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpC4;
return vd;}default: _LLB: _tmpC3=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC2)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmpC3;
return vd;}}_LL0:;}struct _tuple20{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};
# 379
static int Cyc_Evexp_cmp_binding(void*b1,void*b2){
struct Cyc_Absyn_Vardecl*_tmpC8=Cyc_Evexp_binding2vardecl(b1);struct Cyc_Absyn_Vardecl*vdopt1=_tmpC8;
struct Cyc_Absyn_Vardecl*_tmpC9=Cyc_Evexp_binding2vardecl(b2);struct Cyc_Absyn_Vardecl*vdopt2=_tmpC9;
struct _tuple20 _tmpCA=({struct _tuple20 _tmp1B7;_tmp1B7.f1=vdopt1,_tmp1B7.f2=vdopt2;_tmp1B7;});struct _tuple20 _stmttmp16=_tmpCA;struct _tuple20 _tmpCB=_stmttmp16;if(_tmpCB.f1 == 0){if(_tmpCB.f2 == 0){_LL1: _LL2:
 return({struct _tuple0*_tmp1E2=Cyc_Absyn_binding2qvar(b1);Cyc_Absyn_qvar_cmp(_tmp1E2,Cyc_Absyn_binding2qvar(b2));});}else{_LL3: _LL4:
 return - 1;}}else{if(_tmpCB.f2 == 0){_LL5: _LL6:
 return 1;}else{_LL7: _LL8:
 return(int)vdopt1 - (int)vdopt2;}}_LL0:;}
# 392
static unsigned Cyc_Evexp_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmpCC=e->r;void*_stmttmp17=_tmpCC;void*_tmpCD=_stmttmp17;switch(*((int*)_tmpCD)){case 0U: _LL1: _LL2:
 return 0U;case 1U: _LL3: _LL4:
 return 1U;case 2U: _LL5: _LL6:
 return 2U;case 3U: _LL7: _LL8:
 return 3U;case 4U: _LL9: _LLA:
 return 4U;case 5U: _LLB: _LLC:
 return 5U;case 6U: _LLD: _LLE:
 return 6U;case 7U: _LLF: _LL10:
 return 7U;case 8U: _LL11: _LL12:
 return 8U;case 9U: _LL13: _LL14:
 return 9U;case 10U: _LL15: _LL16:
 return 10U;case 11U: _LL17: _LL18:
 return 11U;case 12U: _LL19: _LL1A:
 return 12U;case 13U: _LL1B: _LL1C:
 return 13U;case 14U: _LL1D: _LL1E:
 return 14U;case 15U: _LL1F: _LL20:
 return 15U;case 16U: _LL21: _LL22:
 return 16U;case 17U: _LL23: _LL24:
 return 17U;case 18U: _LL25: _LL26:
 return 18U;case 19U: _LL27: _LL28:
 return 19U;case 20U: _LL29: _LL2A:
 return 20U;case 21U: _LL2B: _LL2C:
 return 21U;case 22U: _LL2D: _LL2E:
 return 22U;case 23U: _LL2F: _LL30:
 return 23U;case 24U: _LL31: _LL32:
 return 24U;case 25U: _LL33: _LL34:
 return 25U;case 26U: _LL35: _LL36:
 return 26U;case 27U: _LL37: _LL38:
 return 27U;case 28U: _LL39: _LL3A:
 return 28U;case 29U: _LL3B: _LL3C:
 return 29U;case 30U: _LL3D: _LL3E:
 return 30U;case 31U: _LL3F: _LL40:
 return 31U;case 32U: _LL41: _LL42:
 return 32U;case 33U: _LL43: _LL44:
 return 33U;case 34U: _LL45: _LL46:
 return 34U;case 35U: _LL47: _LL48:
 return 35U;case 36U: _LL49: _LL4A:
 return 36U;case 37U: _LL4B: _LL4C:
 return 37U;case 38U: _LL4D: _LL4E:
 return 38U;case 39U: _LL4F: _LL50:
 return 39U;case 40U: _LL51: _LL52:
 return 40U;case 41U: _LL53: _LL54:
 return 41U;default: _LL55: _LL56:
 return 42U;}_LL0:;}
# 442
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple12 _tmpCE=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp18=_tmpCE;struct _tuple12 _tmpCF=_stmttmp18;int _tmpD1;union Cyc_Absyn_Cnst _tmpD0;_LL1: _tmpD0=_tmpCF.f1;_tmpD1=_tmpCF.f2;_LL2: {union Cyc_Absyn_Cnst c1=_tmpD0;int known1=_tmpD1;
struct _tuple12 _tmpD2=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp19=_tmpD2;struct _tuple12 _tmpD3=_stmttmp19;int _tmpD5;union Cyc_Absyn_Cnst _tmpD4;_LL4: _tmpD4=_tmpD3.f1;_tmpD5=_tmpD3.f2;_LL5: {union Cyc_Absyn_Cnst c2=_tmpD4;int known2=_tmpD5;
if(known1 && known2)return Cyc_Evexp_cmp_cnst(c1,c2);{
unsigned _tmpD6=Cyc_Evexp_exp_case_number(e1);unsigned e1case=_tmpD6;
unsigned _tmpD7=Cyc_Evexp_exp_case_number(e2);unsigned e2case=_tmpD7;
if(e1case != e2case)
return(int)(e1case - e2case);{
struct _tuple18 _tmpD8=({struct _tuple18 _tmp1BE;_tmp1BE.f1=e1->r,_tmp1BE.f2=e2->r;_tmp1BE;});struct _tuple18 _stmttmp1A=_tmpD8;struct _tuple18 _tmpD9=_stmttmp1A;struct Cyc_Absyn_Datatypefield*_tmpDF;struct Cyc_Absyn_Datatypedecl*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Datatypefield*_tmpDC;struct Cyc_Absyn_Datatypedecl*_tmpDB;struct Cyc_List_List*_tmpDA;struct Cyc_List_List*_tmpE1;struct Cyc_List_List*_tmpE0;struct Cyc_Absyn_Aggrdecl*_tmpE7;struct Cyc_List_List*_tmpE6;struct Cyc_List_List*_tmpE5;struct Cyc_Absyn_Aggrdecl*_tmpE4;struct Cyc_List_List*_tmpE3;struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*_tmpE8;struct Cyc_List_List*_tmpED;void*_tmpEC;struct Cyc_List_List*_tmpEB;void*_tmpEA;int _tmpF3;void*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;int _tmpF0;void*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_List_List*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFF;void*_tmpFE;struct Cyc_Absyn_Exp*_tmp103;void*_tmp102;struct Cyc_Absyn_Exp*_tmp101;void*_tmp100;struct Cyc_List_List*_tmp107;void*_tmp106;struct Cyc_List_List*_tmp105;void*_tmp104;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;void*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10D;void*_tmp10C;void*_tmp10F;void*_tmp10E;struct Cyc_List_List*_tmp113;enum Cyc_Absyn_Primop _tmp112;struct Cyc_List_List*_tmp111;enum Cyc_Absyn_Primop _tmp110;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Vardecl*_tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Vardecl*_tmp120;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct _fat_ptr _tmp12D;struct _fat_ptr _tmp12C;void*_tmp12F;void*_tmp12E;union Cyc_Absyn_Cnst _tmp131;union Cyc_Absyn_Cnst _tmp130;switch(*((int*)_tmpD9.f1)){case 0U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 0U){_LL7: _tmp130=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp131=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL8: {union Cyc_Absyn_Cnst c1=_tmp130;union Cyc_Absyn_Cnst c2=_tmp131;
# 452
return Cyc_Evexp_cmp_cnst(c1,c2);}}else{goto _LL3F;}case 1U: if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 1U){_LL9: _tmp12E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp12F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LLA: {void*b1=_tmp12E;void*b2=_tmp12F;
return Cyc_Evexp_cmp_binding(b1,b2);}}else{goto _LL3F;}case 2U: if(((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 2U){_LLB: _tmp12C=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp12D=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LLC: {struct _fat_ptr s1=_tmp12C;struct _fat_ptr s2=_tmp12D;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LL3F;}case 6U: if(((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 6U){_LLD: _tmp126=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp127=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmp128=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f3;_tmp129=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmp12A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_tmp12B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f3;_LLE: {struct Cyc_Absyn_Exp*e11=_tmp126;struct Cyc_Absyn_Exp*e12=_tmp127;struct Cyc_Absyn_Exp*e13=_tmp128;struct Cyc_Absyn_Exp*e21=_tmp129;struct Cyc_Absyn_Exp*e22=_tmp12A;struct Cyc_Absyn_Exp*e23=_tmp12B;
# 458
int _tmp132=Cyc_Evexp_exp_cmp(e11,e21);int c1=_tmp132;
if(c1 != 0)return c1;
c1=Cyc_Evexp_exp_cmp(e12,e22);
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e13,e23);}}else{goto _LL3F;}case 27U: if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 27U){_LLF: _tmp120=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp121=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmp122=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f3;_tmp123=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmp124=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_tmp125=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f3;_LL10: {struct Cyc_Absyn_Vardecl*vd1=_tmp120;struct Cyc_Absyn_Exp*e11=_tmp121;struct Cyc_Absyn_Exp*e12=_tmp122;struct Cyc_Absyn_Vardecl*vd2=_tmp123;struct Cyc_Absyn_Exp*e21=_tmp124;struct Cyc_Absyn_Exp*e22=_tmp125;
# 464
if(vd1 != vd2)return(int)vd1 - (int)vd2;
_tmp11C=e11;_tmp11D=e12;_tmp11E=e21;_tmp11F=e22;goto _LL12;}}else{goto _LL3F;}case 7U: if(((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 7U){_LL11: _tmp11C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp11D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmp11E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmp11F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_LL12: {struct Cyc_Absyn_Exp*e11=_tmp11C;struct Cyc_Absyn_Exp*e12=_tmp11D;struct Cyc_Absyn_Exp*e21=_tmp11E;struct Cyc_Absyn_Exp*e22=_tmp11F;
_tmp118=e11;_tmp119=e12;_tmp11A=e21;_tmp11B=e22;goto _LL14;}}else{goto _LL3F;}case 8U: if(((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 8U){_LL13: _tmp118=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp119=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmp11A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmp11B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_LL14: {struct Cyc_Absyn_Exp*e11=_tmp118;struct Cyc_Absyn_Exp*e12=_tmp119;struct Cyc_Absyn_Exp*e21=_tmp11A;struct Cyc_Absyn_Exp*e22=_tmp11B;
_tmp114=e11;_tmp115=e12;_tmp116=e21;_tmp117=e22;goto _LL16;}}else{goto _LL3F;}case 9U: if(((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 9U){_LL15: _tmp114=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp115=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmp116=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmp117=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_LL16: {struct Cyc_Absyn_Exp*e11=_tmp114;struct Cyc_Absyn_Exp*e12=_tmp115;struct Cyc_Absyn_Exp*e21=_tmp116;struct Cyc_Absyn_Exp*e22=_tmp117;
# 469
int _tmp133=Cyc_Evexp_exp_cmp(e11,e21);int c1=_tmp133;
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e12,e22);}}else{goto _LL3F;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 3U){_LL17: _tmp110=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp111=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmp112=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmp113=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_LL18: {enum Cyc_Absyn_Primop p1=_tmp110;struct Cyc_List_List*es1=_tmp111;enum Cyc_Absyn_Primop p2=_tmp112;struct Cyc_List_List*es2=_tmp113;
# 473
int _tmp134=(int)p1 - (int)p2;int c1=_tmp134;
if(c1 != 0)return c1;
for(0;es1 != 0 && es2 != 0;(es1=es1->tl,es2=es2->tl)){
int _tmp135=Cyc_Evexp_exp_cmp((struct Cyc_Absyn_Exp*)es1->hd,(struct Cyc_Absyn_Exp*)es2->hd);int c2=_tmp135;
if(c2 != 0)
return c2;}
# 480
return 0;}}else{goto _LL3F;}case 17U: switch(*((int*)_tmpD9.f2)){case 17U: _LL19: _tmp10E=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp10F=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL1A: {void*t1=_tmp10E;void*t2=_tmp10F;
# 482
return Cyc_Tcutil_typecmp(t1,t2);}case 18U: _LL1B: _tmp10C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp10D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL1C: {void*t1=_tmp10C;struct Cyc_Absyn_Exp*e2a=_tmp10D;
# 484
void*_tmp136=e2a->topt;void*e2atopt=_tmp136;
if(e2atopt == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp138=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B8;_tmp1B8.tag=0U,({struct _fat_ptr _tmp1E3=({const char*_tmp139="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmp139,sizeof(char),51U);});_tmp1B8.f1=_tmp1E3;});_tmp1B8;});void*_tmp137[1U];_tmp137[0]=& _tmp138;({unsigned _tmp1E4=e2->loc;Cyc_Warn_err2(_tmp1E4,_tag_fat(_tmp137,sizeof(void*),1U));});});
return 0;}
# 489
return Cyc_Tcutil_typecmp(t1,e2atopt);}default: goto _LL3F;}case 18U: switch(*((int*)_tmpD9.f2)){case 17U: _LL1D: _tmp10A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp10B=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1a=_tmp10A;void*t2=_tmp10B;
# 491
void*_tmp13A=e1a->topt;void*e1atopt=_tmp13A;
if(e1atopt == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B9;_tmp1B9.tag=0U,({struct _fat_ptr _tmp1E5=({const char*_tmp13D="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmp13D,sizeof(char),51U);});_tmp1B9.f1=_tmp1E5;});_tmp1B9;});void*_tmp13B[1U];_tmp13B[0]=& _tmp13C;({unsigned _tmp1E6=e1->loc;Cyc_Warn_err2(_tmp1E6,_tag_fat(_tmp13B,sizeof(void*),1U));});});
return 0;}
# 496
return Cyc_Tcutil_typecmp(e1atopt,t2);}case 18U: _LL1F: _tmp108=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp109=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL20: {struct Cyc_Absyn_Exp*e1a=_tmp108;struct Cyc_Absyn_Exp*e2a=_tmp109;
# 498
void*_tmp13E=e1a->topt;void*e1atopt=_tmp13E;
void*_tmp13F=e2a->topt;void*e2atopt=_tmp13F;
if(e1atopt == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp141=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BA;_tmp1BA.tag=0U,({struct _fat_ptr _tmp1E7=({const char*_tmp142="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmp142,sizeof(char),51U);});_tmp1BA.f1=_tmp1E7;});_tmp1BA;});void*_tmp140[1U];_tmp140[0]=& _tmp141;({unsigned _tmp1E8=e1->loc;Cyc_Warn_err2(_tmp1E8,_tag_fat(_tmp140,sizeof(void*),1U));});});
return 0;}else{
if(e2atopt == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp144=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BB;_tmp1BB.tag=0U,({struct _fat_ptr _tmp1E9=({const char*_tmp145="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmp145,sizeof(char),51U);});_tmp1BB.f1=_tmp1E9;});_tmp1BB;});void*_tmp143[1U];_tmp143[0]=& _tmp144;({unsigned _tmp1EA=e2->loc;Cyc_Warn_err2(_tmp1EA,_tag_fat(_tmp143,sizeof(void*),1U));});});
return 0;}}
# 507
return Cyc_Tcutil_typecmp(e1atopt,e2atopt);}default: goto _LL3F;}case 19U: if(((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 19U){_LL21: _tmp104=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp105=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmp106=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmp107=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_LL22: {void*t1=_tmp104;struct Cyc_List_List*f1=_tmp105;void*t2=_tmp106;struct Cyc_List_List*f2=_tmp107;
# 509
int _tmp146=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmp146;
if(c1 != 0)return c1;{
int _tmp147=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(f1);int l1=_tmp147;
int _tmp148=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(f2);int l2=_tmp148;
if(l1 < l2)return - 1;
if(l2 < l1)return 1;
for(0;f1 != 0 && f2 != 0;(f1=f1->tl,f2=f2->tl)){
struct _tuple18 _tmp149=({struct _tuple18 _tmp1BC;_tmp1BC.f1=(void*)f1->hd,_tmp1BC.f2=(void*)f2->hd;_tmp1BC;});struct _tuple18 _stmttmp1B=_tmp149;struct _tuple18 _tmp14A=_stmttmp1B;unsigned _tmp14C;unsigned _tmp14B;struct _fat_ptr*_tmp14E;struct _fat_ptr*_tmp14D;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp14A.f1)->tag == 0U){if(((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp14A.f2)->tag == 1U){_LL42: _LL43:
 return - 1;}else{_LL46: _tmp14D=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp14A.f1)->f1;_tmp14E=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp14A.f2)->f1;_LL47: {struct _fat_ptr*fn1=_tmp14D;struct _fat_ptr*fn2=_tmp14E;
# 520
int c=Cyc_strptrcmp(fn1,fn2);
if(c != 0)return c;
goto _LL41;}}}else{if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp14A.f2)->tag == 0U){_LL44: _LL45:
# 518
 return 1;}else{_LL48: _tmp14B=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp14A.f1)->f1;_tmp14C=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp14A.f2)->f1;_LL49: {unsigned i1=_tmp14B;unsigned i2=_tmp14C;
# 524
int c=(int)(i1 - i2);
if(c != 0)return c;
goto _LL41;}}}_LL41:;}
# 528
return 0;}}}else{goto _LL3F;}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 14U){_LL23: _tmp100=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmp101=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmp102=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmp103=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_LL24: {void*t1=_tmp100;struct Cyc_Absyn_Exp*e1a=_tmp101;void*t2=_tmp102;struct Cyc_Absyn_Exp*e2a=_tmp103;
# 530
int _tmp14F=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmp14F;
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e1a,e2a);}}else{goto _LL3F;}case 39U: if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 39U){_LL25: _tmpFE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpFF=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL26: {void*t1=_tmpFE;void*t2=_tmpFF;
# 534
if(Cyc_Unify_unify(t1,t2))return 0;
return Cyc_Tcutil_typecmp(t1,t2);}}else{goto _LL3F;}case 41U: if(((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 41U){_LL27: _tmpFC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpFD=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpFC;struct Cyc_Absyn_Exp*e2=_tmpFD;
_tmpFA=e1;_tmpFB=e2;goto _LL2A;}}else{goto _LL3F;}case 12U: if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 12U){_LL29: _tmpFA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpFB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_Absyn_Exp*e2=_tmpFB;
# 538
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 13U: if(((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 13U){_LL2B: _tmpF6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpF7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmpF8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmpF9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpF6;struct Cyc_List_List*t1=_tmpF7;struct Cyc_Absyn_Exp*e2=_tmpF8;struct Cyc_List_List*t2=_tmpF9;
# 540
int _tmp150=Cyc_Evexp_exp_cmp(e1,e2);int c1=_tmp150;
if(c1 != 0)return c1;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,t1,t2);}}else{goto _LL3F;}case 15U: if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 15U){_LL2D: _tmpF4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpF5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpF4;struct Cyc_Absyn_Exp*e2=_tmpF5;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 28U: if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 28U){_LL2F: _tmpEE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpEF=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmpF0=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f3;_tmpF1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmpF2=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_tmpF3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f3;_LL30: {struct Cyc_Absyn_Exp*e1=_tmpEE;void*t1=_tmpEF;int b1=_tmpF0;struct Cyc_Absyn_Exp*e2=_tmpF1;void*t2=_tmpF2;int b2=_tmpF3;
# 545
int _tmp151=b1 - b2;int c1=_tmp151;
if(c1 != 0)return c1;
c1=Cyc_Tcutil_typecmp(t1,t2);
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 30U: if(((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 30U){_LL31: _tmpEA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpEB=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmpEC=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmpED=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_LL32: {void*t1=_tmpEA;struct Cyc_List_List*des1=_tmpEB;void*t2=_tmpEC;struct Cyc_List_List*des2=_tmpED;
# 551
int _tmp152=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmp152;
if(c1 != 0)return c1;
_tmpE8=des1;_tmpE9=des2;goto _LL34;}}else{goto _LL3F;}case 26U: if(((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 26U){_LL33: _tmpE8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpE9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL34: {struct Cyc_List_List*des1=_tmpE8;struct Cyc_List_List*des2=_tmpE9;
# 555
return((int(*)(int(*cmp)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_designator_exp_cmp,des1,des2);}}else{goto _LL3F;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 29U){_LL35: _tmpE2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmpE3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f3;_tmpE4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f4;_tmpE5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_tmpE6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f3;_tmpE7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f4;_LL36: {struct Cyc_List_List*ts1=_tmpE2;struct Cyc_List_List*des1=_tmpE3;struct Cyc_Absyn_Aggrdecl*ad1=_tmpE4;struct Cyc_List_List*ts2=_tmpE5;struct Cyc_List_List*des2=_tmpE6;struct Cyc_Absyn_Aggrdecl*ad2=_tmpE7;
# 557
int _tmp153=(int)ad1 - (int)ad2;int c1=_tmp153;
if(c1 != 0)return c1;
c1=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);
if(c1 != 0)return c1;
return((int(*)(int(*cmp)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_designator_exp_cmp,des1,des1);}}else{goto _LL3F;}case 24U: if(((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 24U){_LL37: _tmpE0=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpE1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_LL38: {struct Cyc_List_List*es1=_tmpE0;struct Cyc_List_List*es2=_tmpE1;
# 563
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_exp_cmp,es1,es2);}}else{goto _LL3F;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 31U){_LL39: _tmpDA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f1;_tmpDB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f2;_tmpDC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9.f1)->f3;_tmpDD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f1;_tmpDE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f2;_tmpDF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->f3;_LL3A: {struct Cyc_List_List*es1=_tmpDA;struct Cyc_Absyn_Datatypedecl*dd1=_tmpDB;struct Cyc_Absyn_Datatypefield*df1=_tmpDC;struct Cyc_List_List*es2=_tmpDD;struct Cyc_Absyn_Datatypedecl*dd2=_tmpDE;struct Cyc_Absyn_Datatypefield*df2=_tmpDF;
# 565
int _tmp154=(int)dd1 - (int)dd2;int c1=_tmp154;
if(c1 != 0)return c1;
c1=(int)df1 - (int)df2;
if(c1 != 0)return c1;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_exp_cmp,es1,es2);}}else{goto _LL3F;}case 32U: if(((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 32U){_LL3B: _LL3C:
# 571
 goto _LL3E;}else{goto _LL3F;}case 33U: if(((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD9.f2)->tag == 33U){_LL3D: _LL3E:
 goto _LL40;}else{goto _LL3F;}default: _LL3F: _LL40:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp156=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BD;_tmp1BD.tag=0U,({struct _fat_ptr _tmp1EB=({const char*_tmp157="Evexp::exp_cmp, unexpected case";_tag_fat(_tmp157,sizeof(char),32U);});_tmp1BD.f1=_tmp1EB;});_tmp1BD;});void*_tmp155[1U];_tmp155[0]=& _tmp156;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp155,sizeof(void*),1U));});}_LL6:;}}}}}
# 577
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 579
e1=Cyc_Evexp_strip_cast(e1);
e2=Cyc_Evexp_strip_cast(e2);{
struct _tuple12 _tmp158=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp1C=_tmp158;struct _tuple12 _tmp159=_stmttmp1C;int _tmp15B;union Cyc_Absyn_Cnst _tmp15A;_LL1: _tmp15A=_tmp159.f1;_tmp15B=_tmp159.f2;_LL2: {union Cyc_Absyn_Cnst c1=_tmp15A;int known1=_tmp15B;
struct _tuple12 _tmp15C=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp1D=_tmp15C;struct _tuple12 _tmp15D=_stmttmp1D;int _tmp15F;union Cyc_Absyn_Cnst _tmp15E;_LL4: _tmp15E=_tmp15D.f1;_tmp15F=_tmp15D.f2;_LL5: {union Cyc_Absyn_Cnst c2=_tmp15E;int known2=_tmp15F;
if(known1 && known2){
struct _tuple11 _tmp160=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple11 _stmttmp1E=_tmp160;struct _tuple11 _tmp161=_stmttmp1E;unsigned _tmp162;_LL7: _tmp162=_tmp161.f1;_LL8: {unsigned i1=_tmp162;
struct _tuple11 _tmp163=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple11 _stmttmp1F=_tmp163;struct _tuple11 _tmp164=_stmttmp1F;unsigned _tmp165;_LLA: _tmp165=_tmp164.f1;_LLB: {unsigned i2=_tmp165;
return(int)(i1 - i2);}}}
# 588
return Cyc_Evexp_exp_cmp(e1,e2);}}}}
# 591
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_uint_exp_cmp(e1,e2)== 0;}
# 594
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmp166=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple11 _stmttmp20=_tmp166;struct _tuple11 _tmp167=_stmttmp20;int _tmp169;unsigned _tmp168;_LL1: _tmp168=_tmp167.f1;_tmp169=_tmp167.f2;_LL2: {unsigned i1=_tmp168;int known1=_tmp169;
struct _tuple11 _tmp16A=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple11 _stmttmp21=_tmp16A;struct _tuple11 _tmp16B=_stmttmp21;int _tmp16D;unsigned _tmp16C;_LL4: _tmp16C=_tmp16B.f1;_tmp16D=_tmp16B.f2;_LL5: {unsigned i2=_tmp16C;int known2=_tmp16D;
if(known1 && known2)
return i1 <= i2;
return Cyc_Evexp_same_uint_const_exp(e1,e2);}}}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 604
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp16E=Cyc_Absyn_compress(t);void*_stmttmp22=_tmp16E;void*_tmp16F=_stmttmp22;struct Cyc_Absyn_Typedefdecl*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_Absyn_Exp*_tmp173;void*_tmp172;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Tvar*_tmp175;struct Cyc_List_List*_tmp177;void*_tmp176;switch(*((int*)_tmp16F)){case 0U: _LL1: _tmp176=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1;_tmp177=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2;_LL2: {void*c=_tmp176;struct Cyc_List_List*ts=_tmp177;
# 607
void*_tmp178=c;struct Cyc_Absyn_Aggrdecl*_tmp179;switch(*((int*)_tmp178)){case 19U: _LL1C: _LL1D:
 return 0;case 4U: _LL1E: _LL1F:
 goto _LL21;case 1U: _LL20: _LL21:
 goto _LL23;case 2U: _LL22: _LL23:
 goto _LL25;case 17U: _LL24: _LL25:
 goto _LL27;case 15U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 goto _LL2B;case 18U: _LL2A: _LL2B:
 goto _LL2D;case 16U: _LL2C: _LL2D:
 return 1;case 0U: _LL2E: _LL2F:
# 618
 goto _LL31;case 5U: _LL30: _LL31:
 goto _LL33;case 7U: _LL32: _LL33:
 goto _LL35;case 6U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 8U: _LL38: _LL39:
 goto _LL3B;case 11U: _LL3A: _LL3B:
 goto _LL3D;case 12U: _LL3C: _LL3D:
 goto _LL3F;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 goto _LL43;case 10U: _LL42: _LL43:
 return 0;default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp178)->f1).UnknownAggr).tag == 1){_LL44: _LL45:
# 630
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BF;_tmp1BF.tag=0U,({struct _fat_ptr _tmp1EC=({const char*_tmp17C="szof on unchecked StructType";_tag_fat(_tmp17C,sizeof(char),29U);});_tmp1BF.f1=_tmp1EC;});_tmp1BF;});void*_tmp17A[1U];_tmp17A[0]=& _tmp17B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp17A,sizeof(void*),1U));});}else{_LL46: _tmp179=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp178)->f1).KnownAggr).val;_LL47: {struct Cyc_Absyn_Aggrdecl*ad=_tmp179;
# 632
if(ad->impl == 0)return 0;{
struct _RegionHandle _tmp17D=_new_region("temp");struct _RegionHandle*temp=& _tmp17D;_push_region(temp);
{struct Cyc_List_List*_tmp17E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,ts);struct Cyc_List_List*inst=_tmp17E;
{struct Cyc_List_List*_tmp17F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp17F;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp180=0;_npop_handler(0U);return _tmp180;}}}{
int _tmp181=1;_npop_handler(0U);return _tmp181;}}
# 634
;_pop_region();}}}}_LL1B:;}case 2U: _LL3: _tmp175=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp16F)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp175;
# 640
return(int)(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_BoxKind;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16F)->f1 != 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16F)->f1)->v)->kind == Cyc_Absyn_BoxKind){_LL5: _LL6:
 return 1;}else{goto _LL7;}}else{_LL7: _LL8:
 return 0;}case 6U: _LL9: _tmp174=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp16F)->f1;_LLA: {struct Cyc_List_List*tqs=_tmp174;
# 645
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple21*)tqs->hd)).f2))
return 0;}
return 1;}case 3U: _LLB: _LLC:
# 651
 return 1;case 4U: _LLD: _tmp172=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16F)->f1).elt_type;_tmp173=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16F)->f1).num_elts;_LLE: {void*t2=_tmp172;struct Cyc_Absyn_Exp*e=_tmp173;
# 654
return e != 0;}case 5U: _LLF: _LL10:
 return 0;case 7U: _LL11: _tmp171=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp16F)->f2;_LL12: {struct Cyc_List_List*fs=_tmp171;
# 657
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}case 10U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 return 0;default: _LL19: _tmp170=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16F)->f3;_LL1A: {struct Cyc_Absyn_Typedefdecl*td=_tmp170;
# 666
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp183=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C1;_tmp1C1.tag=0U,({struct _fat_ptr _tmp1ED=({const char*_tmp185="szof typedeftype ";_tag_fat(_tmp185,sizeof(char),18U);});_tmp1C1.f1=_tmp1ED;});_tmp1C1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp184=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1C0;_tmp1C0.tag=2U,_tmp1C0.f1=(void*)t;_tmp1C0;});void*_tmp182[2U];_tmp182[0]=& _tmp183,_tmp182[1]=& _tmp184;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp182,sizeof(void*),2U));});
return(int)((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind == (int)Cyc_Absyn_BoxKind;}}_LL0:;}

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
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 964 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 966
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1038
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1050
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 210 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);struct Cyc_Hashtable_Table;
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 41
struct _tuple1*Cyc_Toc_temp_var (void);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 32 "toseqc.cyc"
enum Cyc_Toseqc_SideEffect{Cyc_Toseqc_Const =0U,Cyc_Toseqc_NoEffect =1U,Cyc_Toseqc_ExnEffect =2U,Cyc_Toseqc_AnyEffect =3U};struct _tuple12{enum Cyc_Toseqc_SideEffect f1;enum Cyc_Toseqc_SideEffect f2;};
# 34
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_join_side_effect(enum Cyc_Toseqc_SideEffect e1,enum Cyc_Toseqc_SideEffect e2){
struct _tuple12 _tmp0=({struct _tuple12 _tmp7E;_tmp7E.f1=e1,_tmp7E.f2=e2;_tmp7E;});struct _tuple12 _stmttmp0=_tmp0;struct _tuple12 _tmp1=_stmttmp0;if(_tmp1.f1 == Cyc_Toseqc_AnyEffect)
goto _LL4;else{if(_tmp1.f2 == Cyc_Toseqc_AnyEffect){_LL4:
 return Cyc_Toseqc_AnyEffect;}else{if(_tmp1.f1 == Cyc_Toseqc_ExnEffect){if(_tmp1.f2 == Cyc_Toseqc_ExnEffect)
# 39
return Cyc_Toseqc_AnyEffect;else{
goto _LLA;}}else{if(_tmp1.f2 == Cyc_Toseqc_ExnEffect){_LLA:
 return Cyc_Toseqc_ExnEffect;}else{if(_tmp1.f1 == Cyc_Toseqc_NoEffect)
goto _LLE;else{if(_tmp1.f2 == Cyc_Toseqc_NoEffect){_LLE:
 return Cyc_Toseqc_NoEffect;}else{if(_tmp1.f1 == Cyc_Toseqc_Const){if(_tmp1.f2 == Cyc_Toseqc_Const)
return Cyc_Toseqc_Const;else{goto _LL11;}}else{_LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D;_tmp7D.tag=0,({struct _fat_ptr _tmp8D=({const char*_tmp4="join_side_effect";_tag_fat(_tmp4,sizeof(char),17U);});_tmp7D.f1=_tmp8D;});_tmp7D;});void*_tmp2[1];_tmp2[0]=& _tmp3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2,sizeof(void*),1));});}}}}}}};}static char _tmp5[14U]="_get_fat_size";static char _tmp6[24U]="_get_zero_arr_size_char";static char _tmp7[25U]="_get_zero_arr_size_short";static char _tmp8[23U]="_get_zero_arr_size_int";static char _tmp9[25U]="_get_zero_arr_size_float";static char _tmpA[26U]="_get_zero_arr_size_double";static char _tmpB[30U]="_get_zero_arr_size_longdouble";static char _tmpC[28U]="_get_zero_arr_size_voidstar";
# 50
static struct _fat_ptr Cyc_Toseqc_pure_funs[8U]={{_tmp5,_tmp5,_tmp5 + 14U},{_tmp6,_tmp6,_tmp6 + 24U},{_tmp7,_tmp7,_tmp7 + 25U},{_tmp8,_tmp8,_tmp8 + 23U},{_tmp9,_tmp9,_tmp9 + 25U},{_tmpA,_tmpA,_tmpA + 26U},{_tmpB,_tmpB,_tmpB + 30U},{_tmpC,_tmpC,_tmpC + 28U}};static char _tmpD[12U]="_check_null";static char _tmpE[28U]="_check_known_subscript_null";static char _tmpF[31U]="_check_known_subscript_notnull";static char _tmp10[21U]="_check_fat_subscript";static char _tmp11[15U]="_untag_fat_ptr";static char _tmp12[15U]="_region_malloc";
# 61
static struct _fat_ptr Cyc_Toseqc_exn_effect_funs[6U]={{_tmpD,_tmpD,_tmpD + 12U},{_tmpE,_tmpE,_tmpE + 28U},{_tmpF,_tmpF,_tmpF + 31U},{_tmp10,_tmp10,_tmp10 + 21U},{_tmp11,_tmp11,_tmp11 + 15U},{_tmp12,_tmp12,_tmp12 + 15U}};
# 69
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_fun_effect(struct _tuple1*qv){
struct _tuple1*_tmp13=qv;void*_tmp14;if(((((struct _tuple1*)_tmp13)->f1).Loc_n).tag == 4){_tmp14=_tmp13->f2;{struct _fat_ptr*fn=_tmp14;
# 72
{int i=0;for(0;(unsigned)i < 8U;++ i){
if(!Cyc_strcmp((struct _fat_ptr)Cyc_Toseqc_pure_funs[i],(struct _fat_ptr)*fn))
return Cyc_Toseqc_NoEffect;}}
{int i=0;for(0;(unsigned)i < 6U;++ i){
if(!Cyc_strcmp((struct _fat_ptr)Cyc_Toseqc_exn_effect_funs[i],(struct _fat_ptr)*fn))
return Cyc_Toseqc_ExnEffect;}}
return Cyc_Toseqc_AnyEffect;}}else{
return Cyc_Toseqc_AnyEffect;};}
# 83
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exps_effect(struct Cyc_List_List*es){
enum Cyc_Toseqc_SideEffect res=0U;
for(1;es != 0 &&(int)res != (int)3U;es=es->tl){
res=({enum Cyc_Toseqc_SideEffect _tmp8E=Cyc_Toseqc_exp_effect((struct Cyc_Absyn_Exp*)es->hd);Cyc_Toseqc_join_side_effect(_tmp8E,res);});}
return res;}
# 92
static void*Cyc_Toseqc_unexpected_form(struct Cyc_Absyn_Exp*e){
struct _fat_ptr bad_form;
{void*_tmp15=e->r;void*_stmttmp1=_tmp15;void*_tmp16=_stmttmp1;switch(*((int*)_tmp16)){case 2:
 bad_form=({const char*_tmp17="Pragma_e";_tag_fat(_tmp17,sizeof(char),9U);});goto _LL0;case 11:
 bad_form=({const char*_tmp18="Throw_e";_tag_fat(_tmp18,sizeof(char),8U);});goto _LL0;case 16:
 bad_form=({const char*_tmp19="New_e";_tag_fat(_tmp19,sizeof(char),6U);});goto _LL0;case 24:
 bad_form=({const char*_tmp1A="Tuple_e";_tag_fat(_tmp1A,sizeof(char),8U);});goto _LL0;case 25:
 bad_form=({const char*_tmp1B="CompoundLit_e";_tag_fat(_tmp1B,sizeof(char),14U);});goto _LL0;case 26:
 bad_form=({const char*_tmp1C="Array_e";_tag_fat(_tmp1C,sizeof(char),8U);});goto _LL0;case 27:
 bad_form=({const char*_tmp1D="Comprehension_e";_tag_fat(_tmp1D,sizeof(char),16U);});goto _LL0;case 28:
 bad_form=({const char*_tmp1E="ComprehensionNoinit_e";_tag_fat(_tmp1E,sizeof(char),22U);});goto _LL0;case 29:
 bad_form=({const char*_tmp1F="Aggregate_e";_tag_fat(_tmp1F,sizeof(char),12U);});goto _LL0;case 30:
 bad_form=({const char*_tmp20="AnonStruct_e";_tag_fat(_tmp20,sizeof(char),13U);});goto _LL0;case 31:
 bad_form=({const char*_tmp21="Datatype_e";_tag_fat(_tmp21,sizeof(char),11U);});goto _LL0;case 34:
 bad_form=({const char*_tmp22="Malloc_e";_tag_fat(_tmp22,sizeof(char),9U);});goto _LL0;case 35:
 bad_form=({const char*_tmp23="Swap_e";_tag_fat(_tmp23,sizeof(char),7U);});goto _LL0;case 38:
 bad_form=({const char*_tmp24="Tagcheck_e";_tag_fat(_tmp24,sizeof(char),11U);});goto _LL0;case 39:
 bad_form=({const char*_tmp25="Valueof_e";_tag_fat(_tmp25,sizeof(char),10U);});goto _LL0;case 42:
 bad_form=({const char*_tmp26="Assert_e";_tag_fat(_tmp26,sizeof(char),9U);});goto _LL0;default:
 bad_form=({const char*_tmp27="huh??? Unexpected-form";_tag_fat(_tmp27,sizeof(char),23U);});goto _LL0;}_LL0:;}
# 113
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmp83;_tmp83.tag=0,({struct _fat_ptr _tmp8F=({const char*_tmp30="bad exp form ";_tag_fat(_tmp30,sizeof(char),14U);});_tmp83.f1=_tmp8F;});_tmp83;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp82;_tmp82.tag=0,_tmp82.f1=bad_form;_tmp82;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp81;_tmp81.tag=0,({struct _fat_ptr _tmp90=({const char*_tmp2F=" (exp=|";_tag_fat(_tmp2F,sizeof(char),8U);});_tmp81.f1=_tmp90;});_tmp81;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2C=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp80;_tmp80.tag=4,_tmp80.f1=e;_tmp80;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F;_tmp7F.tag=0,({struct _fat_ptr _tmp91=({const char*_tmp2E="|) after xlation to C";_tag_fat(_tmp2E,sizeof(char),22U);});_tmp7F.f1=_tmp91;});_tmp7F;});void*_tmp28[5];_tmp28[0]=& _tmp29,_tmp28[1]=& _tmp2A,_tmp28[2]=& _tmp2B,_tmp28[3]=& _tmp2C,_tmp28[4]=& _tmp2D;Cyc_Warn_impos2(_tag_fat(_tmp28,sizeof(void*),5));});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 117
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e){
struct _fat_ptr bad_form;
void*_tmp31=e->r;void*_stmttmp2=_tmp31;void*_tmp32=_stmttmp2;void*_tmp35;void*_tmp34;void*_tmp33;switch(*((int*)_tmp32)){case 0:
 goto _LL4;case 18: _LL4:
 goto _LL6;case 17: _LL6:
 goto _LL8;case 19: _LL8:
 goto _LLA;case 33: _LLA:
 goto _LLC;case 32: _LLC:
 return Cyc_Toseqc_Const;case 1:
 return Cyc_Toseqc_NoEffect;case 4:
# 128
 goto _LL12;case 40: _LL12:
 return Cyc_Toseqc_AnyEffect;case 10: _tmp33=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp32)->f1;{struct Cyc_Absyn_Exp*e=_tmp33;
# 132
void*_tmp36=e->r;void*_stmttmp3=_tmp36;void*_tmp37=_stmttmp3;void*_tmp38;if(*((int*)_tmp37)== 1)switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37)->f1)){case 2: _tmp38=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37)->f1)->f1;{struct Cyc_Absyn_Fndecl*d=_tmp38;
return Cyc_Toseqc_fun_effect(d->name);}case 1: _tmp38=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp38;
return Cyc_Toseqc_fun_effect(vd->name);}case 0: _tmp38=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37)->f1)->f1;{struct _tuple1*v=_tmp38;
return Cyc_Toseqc_fun_effect(v);}default: goto _LL40;}else{_LL40:
 return Cyc_Toseqc_AnyEffect;};}case 36: _tmp33=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp32)->f2;{struct Cyc_List_List*dles=_tmp33;
# 139
_tmp33=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple13*))Cyc_Core_snd,dles);goto _LL18;}case 3: _tmp33=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp32)->f2;_LL18: {struct Cyc_List_List*es=_tmp33;
return Cyc_Toseqc_exps_effect(es);}case 6: _tmp33=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_tmp34=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp32)->f2;_tmp35=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp32)->f3;{struct Cyc_Absyn_Exp*e1=_tmp33;struct Cyc_Absyn_Exp*e2=_tmp34;struct Cyc_Absyn_Exp*e3=_tmp35;
# 142
return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp39[3];_tmp39[0]=e1,_tmp39[1]=e2,_tmp39[2]=e3;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp39,sizeof(struct Cyc_Absyn_Exp*),3));}));}case 23: _tmp33=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_tmp34=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp32)->f2;{struct Cyc_Absyn_Exp*e1=_tmp33;struct Cyc_Absyn_Exp*e2=_tmp34;
# 144
_tmp33=e1;_tmp34=e2;goto _LL1E;}case 7: _tmp33=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_tmp34=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp32)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp33;struct Cyc_Absyn_Exp*e2=_tmp34;
_tmp33=e1;_tmp34=e2;goto _LL20;}case 8: _tmp33=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_tmp34=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp32)->f2;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp33;struct Cyc_Absyn_Exp*e2=_tmp34;
_tmp33=e1;_tmp34=e2;goto _LL22;}case 9: _tmp33=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_tmp34=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp32)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp33;struct Cyc_Absyn_Exp*e2=_tmp34;
return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp3A[2];_tmp3A[0]=e1,_tmp3A[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3A,sizeof(struct Cyc_Absyn_Exp*),2));}));}case 41: _tmp33=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp32)->f1;{struct Cyc_Absyn_Exp*e1=_tmp33;
# 149
_tmp33=e1;goto _LL26;}case 12: _tmp33=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp33;
_tmp33=e1;goto _LL28;}case 13: _tmp33=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp33;
_tmp33=e1;goto _LL2A;}case 14: _tmp33=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp32)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp33;
_tmp33=e1;goto _LL2C;}case 15: _tmp33=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp33;
_tmp33=e1;goto _LL2E;}case 20: _tmp33=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp33;
_tmp33=e1;goto _LL30;}case 21: _tmp33=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp33;
_tmp33=e1;goto _LL32;}case 22: _tmp33=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp33;
_tmp33=e1;goto _LL34;}case 5: _tmp33=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp32)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp33;
return Cyc_Toseqc_exp_effect(e1);}case 37: _tmp33=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp32)->f1;{struct Cyc_Absyn_Stmt*s=_tmp33;
# 159
return Cyc_Toseqc_stmt_effect(s);}default:
# 161
((int(*)(struct Cyc_Absyn_Exp*))Cyc_Toseqc_unexpected_form)(e);};}
# 164
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s){
enum Cyc_Toseqc_SideEffect res=0U;
while(1){
void*_tmp3B=s->r;void*_stmttmp4=_tmp3B;void*_tmp3C=_stmttmp4;void*_tmp3E;void*_tmp3D;switch(*((int*)_tmp3C)){case 0:
 return res;case 1: _tmp3D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C)->f1;{struct Cyc_Absyn_Exp*e=_tmp3D;
return({enum Cyc_Toseqc_SideEffect _tmp92=res;Cyc_Toseqc_join_side_effect(_tmp92,Cyc_Toseqc_exp_effect(e));});}case 13: _tmp3D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3C)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp3D;
# 171
s=s2;continue;}case 2: _tmp3D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C)->f1;_tmp3E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp3D;struct Cyc_Absyn_Stmt*s2=_tmp3E;
# 173
res=({enum Cyc_Toseqc_SideEffect _tmp93=res;Cyc_Toseqc_join_side_effect(_tmp93,Cyc_Toseqc_stmt_effect(s1));});
s=s2;
continue;}case 6:
# 177
 goto _LLC;case 7: _LLC:
 goto _LLE;case 8: _LLE:
 goto _LL10;case 3: _LL10:
 goto _LL12;case 4: _LL12:
 goto _LL14;case 5: _LL14:
 goto _LL16;case 9: _LL16:
 goto _LL18;case 14: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 10: _LL1C:
 return Cyc_Toseqc_AnyEffect;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40=({struct Cyc_Warn_String_Warn_Warg_struct _tmp84;_tmp84.tag=0,({struct _fat_ptr _tmp94=({const char*_tmp41="bad stmt after xlation to C";_tag_fat(_tmp41,sizeof(char),28U);});_tmp84.f1=_tmp94;});_tmp84;});void*_tmp3F[1];_tmp3F[0]=& _tmp40;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3F,sizeof(void*),1));});};}}
# 193
static int Cyc_Toseqc_is_toc_var(struct Cyc_Absyn_Exp*e){
void*_tmp42=e->r;void*_stmttmp5=_tmp42;void*_tmp43=_stmttmp5;void*_tmp44;if(*((int*)_tmp43)== 1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp43)->f1)== 0){_tmp44=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp43)->f1)->f1;{struct _tuple1*v=_tmp44;
return 1;}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 200
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*);
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*);
# 218 "toseqc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toseqc_exps_to_seqc(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
# 220
((void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_iter)(Cyc_Toseqc_exp_to_seqc,es);
# 223
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(es)<= 1 ||(int)Cyc_Toseqc_exps_effect(es)!= (int)Cyc_Toseqc_AnyEffect)return 0;{
# 227
struct Cyc_Absyn_Stmt*stmt=({struct Cyc_Absyn_Exp*_tmp95=Cyc_Absyn_copy_exp(e);Cyc_Absyn_exp_stmt(_tmp95,e->loc);});
struct Cyc_Absyn_Stmt*laststmt=stmt;
int did_skip=0;
int did_convert=0;
{struct Cyc_List_List*x=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(es);for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Exp*_tmp45=(struct Cyc_Absyn_Exp*)x->hd;struct Cyc_Absyn_Exp*e1=_tmp45;
# 235
if(Cyc_Tcutil_is_const_exp(e1)|| Cyc_Toseqc_is_toc_var(e1))continue;
# 240
if(!did_skip){
did_skip=1;
continue;}
# 244
did_convert=1;{
# 247
struct _tuple1*_tmp46=Cyc_Toc_temp_var();struct _tuple1*v=_tmp46;
struct Cyc_Absyn_Exp*_tmp47=Cyc_Absyn_var_exp(v,e1->loc);struct Cyc_Absyn_Exp*ve=_tmp47;
# 251
struct Cyc_Absyn_Exp*_tmp48=Cyc_Absyn_new_exp(e1->r,e1->loc);struct Cyc_Absyn_Exp*e2=_tmp48;
e2->annot=e1->annot;
e2->topt=e1->topt;{
# 255
void*t=e2->topt == 0?(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=11,_tmp49->f1=e2;_tmp49;}): Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
stmt=Cyc_Absyn_declare_stmt(v,t,e2,stmt,e->loc);
# 259
e1->r=ve->r;}}}}
# 263
if(did_convert){
({void*_tmp96=(Cyc_Absyn_stmt_exp(stmt,e->loc))->r;e->r=_tmp96;});
return laststmt;}
# 267
return 0;}}
# 270
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*e){
void*_tmp4A=e->r;void*_stmttmp6=_tmp4A;void*_tmp4B=_stmttmp6;void*_tmp4E;void*_tmp4D;void*_tmp4C;switch(*((int*)_tmp4B)){case 0:
 goto _LL4;case 1: _LL4:
 return;case 10: _tmp4C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_tmp4D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;{struct Cyc_Absyn_Exp*e1=_tmp4C;struct Cyc_List_List*es=_tmp4D;
# 275
_tmp4C=({struct Cyc_List_List*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->hd=e1,_tmp4F->tl=es;_tmp4F;});goto _LL8;}case 3: _tmp4C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;_LL8: {struct Cyc_List_List*es=_tmp4C;
Cyc_Toseqc_exps_to_seqc(e,es);return;}case 23: _tmp4C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_tmp4D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;{struct Cyc_Absyn_Exp*e1=_tmp4C;struct Cyc_Absyn_Exp*e2=_tmp4D;
({struct Cyc_Absyn_Exp*_tmp97=e;Cyc_Toseqc_exps_to_seqc(_tmp97,({struct Cyc_Absyn_Exp*_tmp50[2];_tmp50[0]=e1,_tmp50[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp50,sizeof(struct Cyc_Absyn_Exp*),2));}));});return;}case 4: _tmp4C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_tmp4D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;_tmp4E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B)->f3;{struct Cyc_Absyn_Exp*e1=_tmp4C;struct Cyc_Core_Opt*p=_tmp4D;struct Cyc_Absyn_Exp*e2=_tmp4E;
# 280
if(p == 0){
void*_tmp51=e1->r;void*_stmttmp7=_tmp51;void*_tmp52=_stmttmp7;if(*((int*)_tmp52)== 1){
Cyc_Toseqc_exp_to_seqc(e2);return;}else{
goto _LL39;}_LL39:;}
# 286
({struct Cyc_Absyn_Exp*_tmp98=e;Cyc_Toseqc_exps_to_seqc(_tmp98,({struct Cyc_Absyn_Exp*_tmp53[2];_tmp53[0]=e2,_tmp53[1]=e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp53,sizeof(struct Cyc_Absyn_Exp*),2));}));});
return;}case 6: _tmp4C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_tmp4D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;_tmp4E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B)->f3;{struct Cyc_Absyn_Exp*e1=_tmp4C;struct Cyc_Absyn_Exp*e2=_tmp4D;struct Cyc_Absyn_Exp*e3=_tmp4E;
# 289
Cyc_Toseqc_exp_to_seqc(e3);_tmp4C=e1;_tmp4D=e2;goto _LL10;}case 7: _tmp4C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_tmp4D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp4C;struct Cyc_Absyn_Exp*e2=_tmp4D;
_tmp4C=e1;_tmp4D=e2;goto _LL12;}case 8: _tmp4C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_tmp4D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp4C;struct Cyc_Absyn_Exp*e2=_tmp4D;
_tmp4C=e1;_tmp4D=e2;goto _LL14;}case 9: _tmp4C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_tmp4D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp4C;struct Cyc_Absyn_Exp*e2=_tmp4D;
Cyc_Toseqc_exp_to_seqc(e2);_tmp4C=e1;goto _LL16;}case 41: _tmp4C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp4C;
# 294
_tmp4C=e1;goto _LL18;}case 12: _tmp4C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp4C;
_tmp4C=e1;goto _LL1A;}case 13: _tmp4C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp4C;
_tmp4C=e1;goto _LL1C;}case 14: _tmp4C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp4C;
_tmp4C=e1;goto _LL1E;}case 15: _tmp4C=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp4C;
_tmp4C=e1;goto _LL20;}case 18: _tmp4C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp4C;
_tmp4C=e1;goto _LL22;}case 20: _tmp4C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp4C;
_tmp4C=e1;goto _LL24;}case 21: _tmp4C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp4C;
_tmp4C=e1;goto _LL26;}case 22: _tmp4C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp4C;
_tmp4C=e1;goto _LL28;}case 5: _tmp4C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp4C;
Cyc_Toseqc_exp_to_seqc(e1);return;}case 17:
# 305
 goto _LL2C;case 19: _LL2C:
 goto _LL2E;case 33: _LL2E:
 goto _LL30;case 32: _LL30:
 return;case 36: _tmp4C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4B)->f2;{struct Cyc_List_List*dles=_tmp4C;
# 310
({struct Cyc_Absyn_Exp*_tmp99=e;Cyc_Toseqc_exps_to_seqc(_tmp99,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple13*))Cyc_Core_snd,dles));});return;}case 37: _tmp4C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4B)->f1;{struct Cyc_Absyn_Stmt*s=_tmp4C;
# 312
Cyc_Toseqc_stmt_to_seqc(s);return;}case 40:
# 314
 return;default:
((int(*)(struct Cyc_Absyn_Exp*))Cyc_Toseqc_unexpected_form)(e);};}
# 319
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s){
# 321
while(1){
void*_tmp54=s->r;void*_stmttmp8=_tmp54;void*_tmp55=_stmttmp8;void*_tmp59;void*_tmp58;void*_tmp57;void*_tmp56;switch(*((int*)_tmp55)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 13: _tmp56=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp56;
s=s2;continue;}case 3: _tmp56=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp56;
# 329
if(eopt == 0)
return;
_tmp56=eopt;goto _LLE;}case 1: _tmp56=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp56;
# 333
Cyc_Toseqc_exp_to_seqc(e);
# 341
return;}case 4: _tmp56=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp57=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_tmp58=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp55)->f3;{struct Cyc_Absyn_Exp*e=_tmp56;struct Cyc_Absyn_Stmt*s1=_tmp57;struct Cyc_Absyn_Stmt*s2=_tmp58;
# 343
Cyc_Toseqc_exp_to_seqc(e);
_tmp56=s1;_tmp57=s2;goto _LL12;}case 2: _tmp56=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp57=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_LL12: {struct Cyc_Absyn_Stmt*s1=_tmp56;struct Cyc_Absyn_Stmt*s2=_tmp57;
# 346
Cyc_Toseqc_stmt_to_seqc(s1);
s=s2;
continue;}case 14: _tmp56=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp57=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp55)->f2).f1;{struct Cyc_Absyn_Stmt*s2=_tmp56;struct Cyc_Absyn_Exp*e=_tmp57;
_tmp56=e;_tmp57=s2;goto _LL16;}case 5: _tmp56=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp55)->f1).f1;_tmp57=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp56;struct Cyc_Absyn_Stmt*s2=_tmp57;
# 351
Cyc_Toseqc_exp_to_seqc(e);
s=s2;
continue;}case 9: _tmp56=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp57=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp55)->f2).f1;_tmp58=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp55)->f3).f1;_tmp59=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp55)->f4;{struct Cyc_Absyn_Exp*e1=_tmp56;struct Cyc_Absyn_Exp*e2=_tmp57;struct Cyc_Absyn_Exp*e3=_tmp58;struct Cyc_Absyn_Stmt*s2=_tmp59;
# 355
Cyc_Toseqc_exp_to_seqc(e1);
Cyc_Toseqc_exp_to_seqc(e2);
Cyc_Toseqc_exp_to_seqc(e3);
s=s2;
continue;}case 10: _tmp56=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp57=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;{struct Cyc_Absyn_Exp*e=_tmp56;struct Cyc_List_List*scs=_tmp57;
# 363
Cyc_Toseqc_exp_to_seqc(e);
for(1;scs != 0;scs=scs->tl){
Cyc_Toseqc_stmt_to_seqc(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _tmp56=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp57=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;{struct Cyc_Absyn_Decl*d=_tmp56;struct Cyc_Absyn_Stmt*s2=_tmp57;
# 368
{void*_tmp5A=d->r;void*_stmttmp9=_tmp5A;void*_tmp5B=_stmttmp9;void*_tmp5C;if(*((int*)_tmp5B)== 0){_tmp5C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5B)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp5C;
# 370
if(vd->initializer != 0){
# 373
void*_tmp5D=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmpA=_tmp5D;void*_tmp5E=_stmttmpA;void*_tmp5F;if(*((int*)_tmp5E)== 36){_tmp5F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5E)->f2;{struct Cyc_List_List*dles=_tmp5F;
# 386 "toseqc.cyc"
struct Cyc_Absyn_Stmt*_tmp60=({struct Cyc_Absyn_Exp*_tmp9A=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);Cyc_Toseqc_exps_to_seqc(_tmp9A,
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple13*))Cyc_Core_snd,dles));});
# 386
struct Cyc_Absyn_Stmt*laststmt=_tmp60;
# 389
if(laststmt == 0)goto _LL24;
# 395
{void*_tmp61=laststmt->r;void*_stmttmpB=_tmp61;void*_tmp62=_stmttmpB;void*_tmp63;if(*((int*)_tmp62)== 1){_tmp63=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp62)->f1;{struct Cyc_Absyn_Exp*initexp=_tmp63;
# 397
{void*_tmp64=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmpC=_tmp64;void*_tmp65=_stmttmpC;void*_tmp66;if(*((int*)_tmp65)== 37){_tmp66=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp65)->f1;{struct Cyc_Absyn_Stmt*s3=_tmp66;
# 399
vd->initializer=initexp;
laststmt->r=s->r;
s->r=s3->r;
goto _LL2E;}}else{
# 404
({struct Cyc_Warn_String_Warn_Warg_struct _tmp68=({struct Cyc_Warn_String_Warn_Warg_struct _tmp87;_tmp87.tag=0,({struct _fat_ptr _tmp9B=({const char*_tmp6C="exps_to_seqc updated to non-stmt-exp |";_tag_fat(_tmp6C,sizeof(char),39U);});_tmp87.f1=_tmp9B;});_tmp87;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp69=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp86;_tmp86.tag=4,_tmp86.f1=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);_tmp86;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp85;_tmp85.tag=0,({
struct _fat_ptr _tmp9C=({const char*_tmp6B="|";_tag_fat(_tmp6B,sizeof(char),2U);});_tmp85.f1=_tmp9C;});_tmp85;});void*_tmp67[3];_tmp67[0]=& _tmp68,_tmp67[1]=& _tmp69,_tmp67[2]=& _tmp6A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp67,sizeof(void*),3));});}_LL2E:;}
# 407
goto _LL29;}}else{
# 409
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp8A;_tmp8A.tag=0,({struct _fat_ptr _tmp9D=({const char*_tmp72="exps_to_seqc returned non-exp-stmt |";_tag_fat(_tmp72,sizeof(char),37U);});_tmp8A.f1=_tmp9D;});_tmp8A;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp6F=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp89;_tmp89.tag=5,_tmp89.f1=laststmt;_tmp89;});struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp88;_tmp88.tag=0,({
struct _fat_ptr _tmp9E=({const char*_tmp71="|";_tag_fat(_tmp71,sizeof(char),2U);});_tmp88.f1=_tmp9E;});_tmp88;});void*_tmp6D[3];_tmp6D[0]=& _tmp6E,_tmp6D[1]=& _tmp6F,_tmp6D[2]=& _tmp70;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp6D,sizeof(void*),3));});}_LL29:;}
# 412
goto _LL24;}}else{
# 415
Cyc_Toseqc_exp_to_seqc((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL24;}_LL24:;}
# 418
s=s2;
continue;}}else{
goto _LL1F;}_LL1F:;}
# 422
goto _LL1E;}default: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmp8B;_tmp8B.tag=0,({struct _fat_ptr _tmp9F=({const char*_tmp75="bad stmt after xlation to C";_tag_fat(_tmp75,sizeof(char),28U);});_tmp8B.f1=_tmp9F;});_tmp8B;});void*_tmp73[1];_tmp73[0]=& _tmp74;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp73,sizeof(void*),1));});};}}
# 427
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*ds){
{struct Cyc_List_List*_tmp76=ds;struct Cyc_List_List*old_ds=_tmp76;for(0;old_ds != 0;old_ds=old_ds->tl){
void*_tmp77=((struct Cyc_Absyn_Decl*)old_ds->hd)->r;void*_stmttmpD=_tmp77;void*_tmp78=_stmttmpD;void*_tmp79;switch(*((int*)_tmp78)){case 1: _tmp79=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp78)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp79;
# 431
if((int)fd->orig_scope != (int)Cyc_Absyn_ExternC)
Cyc_Toseqc_stmt_to_seqc(fd->body);
goto _LL0;}case 0:
 goto _LL6;case 5: _LL6:
 goto _LL8;case 7: _LL8:
 goto _LLA;case 8: _LLA:
 goto _LL0;case 6:
 goto _LLE;case 2: _LLE:
 goto _LL10;case 3: _LL10:
 goto _LL12;case 4: _LL12:
 goto _LL14;case 9: _LL14:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp8C;_tmp8C.tag=0,({struct _fat_ptr _tmpA0=({const char*_tmp7C="unexpected toplevel decl in toseqc";_tag_fat(_tmp7C,sizeof(char),35U);});_tmp8C.f1=_tmpA0;});_tmp8C;});void*_tmp7A[1];_tmp7A[0]=& _tmp7B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp7A,sizeof(void*),1));});}_LL0:;}}
# 451
return ds;}

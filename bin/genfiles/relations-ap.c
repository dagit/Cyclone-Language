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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 86 "list.h"
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 864 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 895
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 920
extern void*Cyc_Absyn_fat_bound_type;
# 924
void*Cyc_Absyn_bounds_one (void);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RType(void*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple11{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 64
struct _tuple11 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*);
# 68
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation);
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*,union Cyc_Relations_RelnOp*);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*,union Cyc_Relations_RelnOp,enum Cyc_Relations_Relation,union Cyc_Relations_RelnOp,struct Cyc_List_List*);
# 100
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 106
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
# 120
struct _fat_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*);
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp);
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc_RgnOrder_RgnPO;
# 40 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 221
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 33 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 8 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_one;
# 10
extern struct Cyc_AP_T*Cyc_AP_fromint(long);
# 14
extern struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*);
# 16
extern struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*,struct Cyc_AP_T*);
extern struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*,struct Cyc_AP_T*);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned val;};struct _union_Node_NParamNumelts{int tag;unsigned val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 61 "pratt-ap.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;
# 63
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NType(void*);
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned);
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned);
union Cyc_Pratt_Node Cyc_Pratt_NReturn (void);struct Cyc_Pratt_Graph;
# 76
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph (void);
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*);
# 81
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*,union Cyc_Pratt_Node,union Cyc_Pratt_Node,struct Cyc_AP_T*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 64 "string.h"
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 35 "relations-ap.cyc"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned i){return({union Cyc_Relations_RelnOp _tmpB9;(_tmpB9.RConst).tag=1U,(_tmpB9.RConst).val=i;_tmpB9;});}
union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*v){return({union Cyc_Relations_RelnOp _tmpBA;(_tmpBA.RVar).tag=2U,(_tmpBA.RVar).val=v;_tmpBA;});}
union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*v){return({union Cyc_Relations_RelnOp _tmpBB;(_tmpBB.RNumelts).tag=3U,(_tmpBB.RNumelts).val=v;_tmpBB;});}
union Cyc_Relations_RelnOp Cyc_Relations_RType(void*t){return({union Cyc_Relations_RelnOp _tmpBC;(_tmpBC.RType).tag=4U,({void*_tmpD1=Cyc_Absyn_compress(t);(_tmpBC.RType).val=_tmpD1;});_tmpBC;});}
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned i){return({union Cyc_Relations_RelnOp _tmpBD;(_tmpBD.RParam).tag=5U,(_tmpBD.RParam).val=i;_tmpBD;});}
union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned i){return({union Cyc_Relations_RelnOp _tmpBE;(_tmpBE.RParamNumelts).tag=6U,(_tmpBE.RParamNumelts).val=i;_tmpBE;});}
union Cyc_Relations_RelnOp Cyc_Relations_RReturn (void){return({union Cyc_Relations_RelnOp _tmpBF;(_tmpBF.RReturn).tag=7U,(_tmpBF.RReturn).val=0U;_tmpBF;});}struct _tuple13{union Cyc_Relations_RelnOp f1;union Cyc_Relations_RelnOp f2;};
# 43
static int Cyc_Relations_same_relop(union Cyc_Relations_RelnOp r1,union Cyc_Relations_RelnOp r2){
struct _tuple13 _tmp0=({struct _tuple13 _tmpC0;_tmpC0.f1=r1,_tmpC0.f2=r2;_tmpC0;});struct _tuple13 _stmttmp0=_tmp0;struct _tuple13 _tmp1=_stmttmp0;void*_tmp5;void*_tmp4;unsigned _tmp3;unsigned _tmp2;switch(((_tmp1.f1).RParamNumelts).tag){case 1: if(((_tmp1.f2).RConst).tag == 1){_tmp2=((_tmp1.f1).RConst).val;_tmp3=((_tmp1.f2).RConst).val;{unsigned c1=_tmp2;unsigned c2=_tmp3;
return c1 == c2;}}else{goto _LLF;}case 2: if(((_tmp1.f2).RVar).tag == 2){_tmp4=((_tmp1.f1).RVar).val;_tmp5=((_tmp1.f2).RVar).val;{struct Cyc_Absyn_Vardecl*x1=_tmp4;struct Cyc_Absyn_Vardecl*x2=_tmp5;
return x1 == x2;}}else{goto _LLF;}case 3: if(((_tmp1.f2).RNumelts).tag == 3){_tmp4=((_tmp1.f1).RNumelts).val;_tmp5=((_tmp1.f2).RNumelts).val;{struct Cyc_Absyn_Vardecl*x1=_tmp4;struct Cyc_Absyn_Vardecl*x2=_tmp5;
return x1 == x2;}}else{goto _LLF;}case 4: if(((_tmp1.f2).RType).tag == 4){_tmp4=((_tmp1.f1).RType).val;_tmp5=((_tmp1.f2).RType).val;{void*t1=_tmp4;void*t2=_tmp5;
return Cyc_Unify_unify(t1,t2);}}else{goto _LLF;}case 5: if(((_tmp1.f2).RParam).tag == 5){_tmp2=((_tmp1.f1).RParam).val;_tmp3=((_tmp1.f2).RParam).val;{unsigned i=_tmp2;unsigned j=_tmp3;
return i == j;}}else{goto _LLF;}case 6: if(((_tmp1.f2).RParamNumelts).tag == 6){_tmp2=((_tmp1.f1).RParamNumelts).val;_tmp3=((_tmp1.f2).RParamNumelts).val;{unsigned i=_tmp2;unsigned j=_tmp3;
return i == j;}}else{goto _LLF;}default: if(((_tmp1.f2).RReturn).tag == 7)
return 1;else{_LLF:
 return 0;}};}
# 56
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation relation,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns){
# 60
{struct Cyc_List_List*_tmp6=relns;struct Cyc_List_List*rs=_tmp6;for(0;rs != 0;rs=rs->tl){
struct Cyc_Relations_Reln*_tmp7=(struct Cyc_Relations_Reln*)rs->hd;struct Cyc_Relations_Reln*r=_tmp7;
if((Cyc_Relations_same_relop(r->rop1,rop1)&&(int)r->relation == (int)relation)&&
 Cyc_Relations_same_relop(r->rop2,rop2))return relns;}}
# 65
return({struct Cyc_List_List*_tmp9=_region_malloc(rgn,sizeof(*_tmp9));({struct Cyc_Relations_Reln*_tmpD2=({struct Cyc_Relations_Reln*_tmp8=_region_malloc(rgn,sizeof(*_tmp8));_tmp8->rop1=rop1,_tmp8->relation=relation,_tmp8->rop2=rop2;_tmp8;});_tmp9->hd=_tmpD2;}),_tmp9->tl=relns;_tmp9;});}
# 68
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns){
# 71
union Cyc_Relations_RelnOp rop_old=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop_new=Cyc_Relations_RConst(0U);
if(!Cyc_Relations_exp2relnop(e_old,& rop_old))return relns;
if(!Cyc_Relations_exp2relnop(e_new,& rop_new)){
({void*_tmpA=0U;({unsigned _tmpD4=e_new->loc;struct _fat_ptr _tmpD3=({const char*_tmpB="Could not construct relation";_tag_fat(_tmpB,sizeof(char),29U);});Cyc_Warn_warn(_tmpD4,_tmpD3,_tag_fat(_tmpA,sizeof(void*),0));});});
return relns;}
# 78
{struct Cyc_List_List*_tmpC=relns;struct Cyc_List_List*rs=_tmpC;for(0;rs != 0;rs=rs->tl){
struct Cyc_Relations_Reln*_tmpD=(struct Cyc_Relations_Reln*)rs->hd;struct Cyc_Relations_Reln*r=_tmpD;
union Cyc_Relations_RelnOp _tmpE=r->rop1;union Cyc_Relations_RelnOp rop1=_tmpE;
union Cyc_Relations_RelnOp _tmpF=r->rop2;union Cyc_Relations_RelnOp rop2=_tmpF;
int addIt=0;
if(Cyc_Relations_same_relop(rop1,rop_old)){
addIt=1;
rop1=rop_new;}
# 87
if(Cyc_Relations_same_relop(rop2,rop_old)){
addIt=1;
rop2=rop_new;}
# 91
if(addIt)
relns=({struct Cyc_List_List*_tmp11=_region_malloc(rgn,sizeof(*_tmp11));({struct Cyc_Relations_Reln*_tmpD5=({struct Cyc_Relations_Reln*_tmp10=_region_malloc(rgn,sizeof(*_tmp10));_tmp10->rop1=rop1,_tmp10->relation=r->relation,_tmp10->rop2=rop2;_tmp10;});_tmp11->hd=_tmpD5;}),_tmp11->tl=relns;_tmp11;});}}
# 94
return relns;}
# 97
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 101
for(1;r1s != 0;r1s=r1s->tl){
struct Cyc_Relations_Reln*_tmp12=(struct Cyc_Relations_Reln*)r1s->hd;struct Cyc_Relations_Reln*r1=_tmp12;
int found=0;
{struct Cyc_List_List*_tmp13=r2s;struct Cyc_List_List*r2s=_tmp13;for(0;r2s != 0;r2s=r2s->tl){
struct Cyc_Relations_Reln*_tmp14=(struct Cyc_Relations_Reln*)r2s->hd;struct Cyc_Relations_Reln*r2=_tmp14;
if(r1 == r2 ||(Cyc_Relations_same_relop(r1->rop1,r2->rop1)&&(int)r1->relation == (int)r2->relation)&&
# 108
 Cyc_Relations_same_relop(r1->rop2,r2->rop2)){
found=1;
break;}}}
# 113
if(!found)
return 0;}
# 116
return 1;}
# 119
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp15=r1s;struct Cyc_List_List*r1s=_tmp15;for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_Relations_Reln*_tmp16=(struct Cyc_Relations_Reln*)r1s->hd;struct Cyc_Relations_Reln*r1=_tmp16;
int found=0;
{struct Cyc_List_List*_tmp17=r2s;struct Cyc_List_List*r2s=_tmp17;for(0;r2s != 0;r2s=r2s->tl){
struct Cyc_Relations_Reln*_tmp18=(struct Cyc_Relations_Reln*)r2s->hd;struct Cyc_Relations_Reln*r2=_tmp18;
if(r1 == r2 ||(Cyc_Relations_same_relop(r1->rop1,r2->rop1)&&(int)r1->relation == (int)r2->relation)&&
# 130
 Cyc_Relations_same_relop(r1->rop2,r2->rop2)){
res=({struct Cyc_List_List*_tmp19=_region_malloc(r,sizeof(*_tmp19));_tmp19->hd=r1,_tmp19->tl=res;_tmp19;});
found=1;
break;}}}
# 136
if(!found)diff=1;}}
# 138
if(!diff)res=r1s;
return res;}}
# 142
static int Cyc_Relations_rop_contains_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v){
union Cyc_Relations_RelnOp _tmp1A=r;void*_tmp1B;switch((_tmp1A.RReturn).tag){case 2: _tmp1B=(_tmp1A.RVar).val;{struct Cyc_Absyn_Vardecl*v2=_tmp1B;
return v == v2;}case 3: _tmp1B=(_tmp1A.RNumelts).val;{struct Cyc_Absyn_Vardecl*v2=_tmp1B;
return v == v2;}case 4:
 goto _LL8;case 5: _LL8:
 goto _LLA;case 6: _LLA:
 goto _LLC;case 7: _LLC:
 goto _LLE;default: _LLE:
 return 0;};}
# 154
union Cyc_Relations_RelnOp Cyc_Relations_subst_rop_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v,union Cyc_Relations_RelnOp new_rop){
union Cyc_Relations_RelnOp _tmp1C=r;void*_tmp1D;switch((_tmp1C.RReturn).tag){case 2: _tmp1D=(_tmp1C.RVar).val;{struct Cyc_Absyn_Vardecl*v2=_tmp1D;
return v == v2?new_rop: r;}case 3: _tmp1D=(_tmp1C.RNumelts).val;{struct Cyc_Absyn_Vardecl*v2=_tmp1D;
return v == v2?new_rop: r;}case 4:
 goto _LL8;case 5: _LL8:
 goto _LLA;case 6: _LLA:
 goto _LLC;case 7: _LLC:
 goto _LLE;default: _LLE:
 return r;};}
# 166
struct Cyc_Relations_Reln*Cyc_Relations_substitute_rop_var(struct _RegionHandle*rgn,struct Cyc_Relations_Reln*reln,struct Cyc_Absyn_Vardecl*v,union Cyc_Relations_RelnOp new_rop){
# 168
return({struct Cyc_Relations_Reln*_tmp1E=_region_malloc(rgn,sizeof(*_tmp1E));({union Cyc_Relations_RelnOp _tmpD7=Cyc_Relations_subst_rop_var(reln->rop1,v,new_rop);_tmp1E->rop1=_tmpD7;}),_tmp1E->relation=reln->relation,({
union Cyc_Relations_RelnOp _tmpD6=Cyc_Relations_subst_rop_var(reln->rop2,v,new_rop);_tmp1E->rop2=_tmpD6;});_tmp1E;});}
# 171
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found && p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp1F=(struct Cyc_Relations_Reln*)p->hd;struct Cyc_Relations_Reln*r=_tmp1F;
if(Cyc_Relations_rop_contains_var(r->rop1,v)|| Cyc_Relations_rop_contains_var(r->rop2,v)){
found=1;
break;}}
# 181
if(!found)return rs;{
# 183
struct Cyc_Absyn_Tvar*_tmp20=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));struct Cyc_Absyn_Tvar*tv=_tmp20;
union Cyc_Relations_RelnOp _tmp21=Cyc_Relations_RType(Cyc_Absyn_var_type(tv));union Cyc_Relations_RelnOp new_rop=_tmp21;
struct Cyc_List_List*_tmp22=0;struct Cyc_List_List*new_rs=_tmp22;
for(p=rs;p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp23=(struct Cyc_Relations_Reln*)p->hd;struct Cyc_Relations_Reln*r=_tmp23;
if(Cyc_Relations_rop_contains_var(r->rop1,v)|| Cyc_Relations_rop_contains_var(r->rop2,v))
new_rs=({struct Cyc_List_List*_tmp24=_region_malloc(rgn,sizeof(*_tmp24));({struct Cyc_Relations_Reln*_tmpD8=Cyc_Relations_substitute_rop_var(rgn,r,v,new_rop);_tmp24->hd=_tmpD8;}),_tmp24->tl=new_rs;_tmp24;});else{
# 191
new_rs=({struct Cyc_List_List*_tmp25=_region_malloc(rgn,sizeof(*_tmp25));_tmp25->hd=r,_tmp25->tl=new_rs;_tmp25;});}}
# 194
return new_rs;}}
# 200
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p){
# 203
RETRY:
 if(e->topt != 0){
void*_tmp26=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp1=_tmp26;void*_tmp27=_stmttmp1;void*_tmp28;switch(*((int*)_tmp27)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27)->f2 != 0){_tmp28=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27)->f2)->hd;{void*t=_tmp28;
# 207
({union Cyc_Relations_RelnOp _tmpD9=Cyc_Relations_RType(t);*p=_tmpD9;});return 1;
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}case 9: _tmp28=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp27)->f1;{struct Cyc_Absyn_Exp*type_exp=_tmp28;
# 210
e=type_exp;
goto _LL0;}default: _LL5:
 goto _LL0;}_LL0:;}
# 215
{void*_tmp29=e->r;void*_stmttmp2=_tmp29;void*_tmp2A=_stmttmp2;int _tmp2C;void*_tmp2B;switch(*((int*)_tmp2A)){case 14: _tmp2B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2A)->f2;{struct Cyc_Absyn_Exp*e=_tmp2B;
return Cyc_Relations_exp2relnop(e,p);}case 1: _tmp2B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2A)->f1;{void*b=_tmp2B;
# 218
struct Cyc_Absyn_Vardecl*_tmp2D=Cyc_Tcutil_nonesc_vardecl(b);struct Cyc_Absyn_Vardecl*v=_tmp2D;
if(v != 0){
({union Cyc_Relations_RelnOp _tmpDA=Cyc_Relations_RVar(v);*p=_tmpDA;});
return 1;}
# 223
goto _LL7;}case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2A)->f1).Int_c).tag == 5){_tmp2C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2A)->f1).Int_c).val).f2;{int i=_tmp2C;
# 225
({union Cyc_Relations_RelnOp _tmpDB=Cyc_Relations_RConst((unsigned)i);*p=_tmpDB;});
return 1;}}else{goto _LL12;}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2A)->f1 == Cyc_Absyn_Numelts){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2A)->f2 != 0){if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2A)->f2)->hd)->r)== 1){_tmp2B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2A)->f2)->hd)->r)->f1;{void*b=_tmp2B;
# 228
struct Cyc_Absyn_Vardecl*_tmp2E=Cyc_Tcutil_nonesc_vardecl(b);struct Cyc_Absyn_Vardecl*v=_tmp2E;
if(v != 0){
({union Cyc_Relations_RelnOp _tmpDC=Cyc_Relations_RNumelts(v);*p=_tmpDC;});
return 1;}
# 233
goto _LL7;}}else{goto _LL12;}}else{goto _LL12;}}else{goto _LL12;}case 39: _tmp2B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2A)->f1;{void*t=_tmp2B;
# 235
{void*_tmp2F=Cyc_Absyn_compress(t);void*_stmttmp3=_tmp2F;void*_tmp30=_stmttmp3;void*_tmp31;if(*((int*)_tmp30)== 9){_tmp31=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp30)->f1;{struct Cyc_Absyn_Exp*type_exp=_tmp31;
# 237
e=type_exp;
goto RETRY;}}else{
# 240
({union Cyc_Relations_RelnOp _tmpDD=Cyc_Relations_RType(t);*p=_tmpDD;});
return 1;};}
# 243
goto _LL7;}default: _LL12:
 goto _LL7;}_LL7:;}
# 247
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple12 _tmp32=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp4=_tmp32;struct _tuple12 _tmp33=_stmttmp4;int _tmp35;unsigned _tmp34;_tmp34=_tmp33.f1;_tmp35=_tmp33.f2;{unsigned t1=_tmp34;int known=_tmp35;
if(known){
({union Cyc_Relations_RelnOp _tmpDE=Cyc_Relations_RConst(t1);*p=_tmpDE;});
return 1;}}}
# 254
return 0;}
# 257
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_tmp36=e->r;void*_stmttmp5=_tmp36;void*_tmp37=_stmttmp5;void*_tmp38;if(*((int*)_tmp37)== 1){_tmp38=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37)->f1;{void*b=_tmp38;
# 260
struct Cyc_Absyn_Vardecl*_tmp39=Cyc_Tcutil_nonesc_vardecl(b);struct Cyc_Absyn_Vardecl*v=_tmp39;
if(v != 0)
return Cyc_Relations_reln_kill_var(rgn,r,v);else{
return r;}}}else{
return r;};}
# 268
struct Cyc_Relations_Reln*Cyc_Relations_copy_reln(struct _RegionHandle*r2,struct Cyc_Relations_Reln*r){
return({struct Cyc_Relations_Reln*_tmp3A=_region_malloc(r2,sizeof(*_tmp3A));*_tmp3A=*r;_tmp3A;});}
# 272
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;})(r2,Cyc_Relations_copy_reln,r2,relns);}
# 276
int Cyc_Relations_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(1;r1 != 0 && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_Relations_Reln*_tmp3B=(struct Cyc_Relations_Reln*)r1->hd;struct Cyc_Relations_Reln*x1=_tmp3B;
struct Cyc_Relations_Reln*_tmp3C=(struct Cyc_Relations_Reln*)r2->hd;struct Cyc_Relations_Reln*x2=_tmp3C;
if((!Cyc_Relations_same_relop(x1->rop1,x2->rop1)||(int)x1->relation != (int)x2->relation)|| !
# 282
Cyc_Relations_same_relop(x1->rop2,x2->rop2))return 0;}
# 284
if(r1 != 0 || r2 != 0)return 0;else{
return 1;}}
# 288
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 292
if(v->escapes)return r;
# 295
r=Cyc_Relations_reln_kill_var(rgn,r,v);
# 300
{void*_tmp3D=e->r;void*_stmttmp6=_tmp3D;void*_tmp3E=_stmttmp6;void*_tmp3F;if(*((int*)_tmp3E)== 34){if((((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp3E)->f1).fat_result == 1){_tmp3F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp3E)->f1).num_elts;{struct Cyc_Absyn_Exp*e2=_tmp3F;
# 303
malloc_loop: {
void*_tmp40=e2->r;void*_stmttmp7=_tmp40;void*_tmp41=_stmttmp7;void*_tmp42;switch(*((int*)_tmp41)){case 14: _tmp42=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp41)->f2;{struct Cyc_Absyn_Exp*e3=_tmp42;
e2=e3;goto malloc_loop;}case 1: _tmp42=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp41)->f1;{void*b=_tmp42;
# 307
struct Cyc_Absyn_Vardecl*_tmp43=Cyc_Tcutil_nonesc_vardecl(b);struct Cyc_Absyn_Vardecl*vd=_tmp43;
if(vd != 0)
return({struct _RegionHandle*_tmpE1=rgn;union Cyc_Relations_RelnOp _tmpE0=Cyc_Relations_RVar(vd);union Cyc_Relations_RelnOp _tmpDF=Cyc_Relations_RNumelts(v);Cyc_Relations_add_relation(_tmpE1,_tmpE0,0U,_tmpDF,r);});else{
return r;}}default:
 return r;};}}}else{goto _LL3;}}else{_LL3:
# 314
 goto _LL0;}_LL0:;}
# 317
if(!Cyc_Tcutil_is_integral_type(v->type))return r;{
# 319
union Cyc_Relations_RelnOp eop=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e,& eop))
return({struct _RegionHandle*_tmpE4=rgn;union Cyc_Relations_RelnOp _tmpE3=Cyc_Relations_RVar(v);union Cyc_Relations_RelnOp _tmpE2=eop;Cyc_Relations_add_relation(_tmpE4,_tmpE3,0U,_tmpE2,r);});
# 323
return r;}}
# 326
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp44=e1->r;void*_stmttmp8=_tmp44;void*_tmp45=_stmttmp8;void*_tmp46;if(*((int*)_tmp45)== 1){_tmp46=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp45)->f1;{void*b=_tmp46;
# 329
struct Cyc_Absyn_Vardecl*_tmp47=Cyc_Tcutil_nonesc_vardecl(b);struct Cyc_Absyn_Vardecl*v=_tmp47;
if(v != 0)
return Cyc_Relations_reln_assign_var(rgn,r,v,e2);else{
return r;}}}else{
return r;};}
# 338
static union Cyc_Pratt_Node Cyc_Relations_rop2node(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp48=r;unsigned _tmp49;void*_tmp4A;switch((_tmp48.RReturn).tag){case 2: _tmp4A=(_tmp48.RVar).val;{struct Cyc_Absyn_Vardecl*x=_tmp4A;
return Cyc_Pratt_NVar(x);}case 3: _tmp4A=(_tmp48.RNumelts).val;{struct Cyc_Absyn_Vardecl*x=_tmp4A;
return Cyc_Pratt_NNumelts(x);}case 4: _tmp4A=(_tmp48.RType).val;{void*x=_tmp4A;
return Cyc_Pratt_NType(x);}case 5: _tmp49=(_tmp48.RParam).val;{unsigned i=_tmp49;
return Cyc_Pratt_NParam(i);}case 6: _tmp49=(_tmp48.RParamNumelts).val;{unsigned i=_tmp49;
return Cyc_Pratt_NParamNumelts(i);}case 7:
 return Cyc_Pratt_NReturn();default:
({void*_tmp4B=0U;({int(*_tmpE6)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmpE5=({const char*_tmp4C="rop2node";_tag_fat(_tmp4C,sizeof(char),9U);});_tmpE6(_tmpE5,_tag_fat(_tmp4B,sizeof(void*),0));});});};}
# 350
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmp4D=r;switch((int)_tmp4D){case Cyc_Relations_Req:
 return({const char*_tmp4E="==";_tag_fat(_tmp4E,sizeof(char),3U);});case Cyc_Relations_Rneq:
 return({const char*_tmp4F="!=";_tag_fat(_tmp4F,sizeof(char),3U);});case Cyc_Relations_Rlt:
 return({const char*_tmp50="<";_tag_fat(_tmp50,sizeof(char),2U);});case Cyc_Relations_Rlte:
 goto _LLA;default: _LLA: return({const char*_tmp51="<=";_tag_fat(_tmp51,sizeof(char),3U);});};}
# 359
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp52=r;void*_tmp54;unsigned _tmp53;switch((_tmp52.RParamNumelts).tag){case 1: _tmp53=(_tmp52.RConst).val;{unsigned c=_tmp53;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp57=({struct Cyc_Int_pa_PrintArg_struct _tmpC1;_tmpC1.tag=1,_tmpC1.f1=(unsigned long)((int)c);_tmpC1;});void*_tmp55[1];_tmp55[0]=& _tmp57;({struct _fat_ptr _tmpE7=({const char*_tmp56="%d";_tag_fat(_tmp56,sizeof(char),3U);});Cyc_aprintf(_tmpE7,_tag_fat(_tmp55,sizeof(void*),1));});});}case 2: _tmp54=(_tmp52.RVar).val;{struct Cyc_Absyn_Vardecl*vd=_tmp54;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmpC2;_tmpC2.tag=0,({struct _fat_ptr _tmpE8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmpC2.f1=_tmpE8;});_tmpC2;});void*_tmp58[1];_tmp58[0]=& _tmp5A;({struct _fat_ptr _tmpE9=({const char*_tmp59="%s";_tag_fat(_tmp59,sizeof(char),3U);});Cyc_aprintf(_tmpE9,_tag_fat(_tmp58,sizeof(void*),1));});});}case 4: _tmp54=(_tmp52.RType).val;{void*t=_tmp54;
return Cyc_Absynpp_typ2string(t);}case 3: _tmp54=(_tmp52.RNumelts).val;{struct Cyc_Absyn_Vardecl*vd=_tmp54;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmpC3;_tmpC3.tag=0,({struct _fat_ptr _tmpEA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmpC3.f1=_tmpEA;});_tmpC3;});void*_tmp5B[1];_tmp5B[0]=& _tmp5D;({struct _fat_ptr _tmpEB=({const char*_tmp5C="numelts(%s)";_tag_fat(_tmp5C,sizeof(char),12U);});Cyc_aprintf(_tmpEB,_tag_fat(_tmp5B,sizeof(void*),1));});});}case 5: _tmp53=(_tmp52.RParam).val;{unsigned i=_tmp53;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp60=({struct Cyc_Int_pa_PrintArg_struct _tmpC4;_tmpC4.tag=1,_tmpC4.f1=(unsigned long)((int)i);_tmpC4;});void*_tmp5E[1];_tmp5E[0]=& _tmp60;({struct _fat_ptr _tmpEC=({const char*_tmp5F="param(%d)";_tag_fat(_tmp5F,sizeof(char),10U);});Cyc_aprintf(_tmpEC,_tag_fat(_tmp5E,sizeof(void*),1));});});}case 6: _tmp53=(_tmp52.RParamNumelts).val;{unsigned i=_tmp53;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp63=({struct Cyc_Int_pa_PrintArg_struct _tmpC5;_tmpC5.tag=1,_tmpC5.f1=(unsigned long)((int)i);_tmpC5;});void*_tmp61[1];_tmp61[0]=& _tmp63;({struct _fat_ptr _tmpED=({const char*_tmp62="numelts(param(%d))";_tag_fat(_tmp62,sizeof(char),19U);});Cyc_aprintf(_tmpED,_tag_fat(_tmp61,sizeof(void*),1));});});}default:
 return(struct _fat_ptr)({void*_tmp64=0U;({struct _fat_ptr _tmpEE=({const char*_tmp65="return_value";_tag_fat(_tmp65,sizeof(char),13U);});Cyc_aprintf(_tmpEE,_tag_fat(_tmp64,sizeof(void*),0));});});};}
# 371
struct _fat_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r){
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmpC8;_tmpC8.tag=0,({struct _fat_ptr _tmpEF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_rop2string(r->rop1));_tmpC8.f1=_tmpEF;});_tmpC8;});struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmpC7;_tmpC7.tag=0,({struct _fat_ptr _tmpF0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_relation2string(r->relation));_tmpC7.f1=_tmpF0;});_tmpC7;});struct Cyc_String_pa_PrintArg_struct _tmp6A=({struct Cyc_String_pa_PrintArg_struct _tmpC6;_tmpC6.tag=0,({
struct _fat_ptr _tmpF1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_rop2string(r->rop2));_tmpC6.f1=_tmpF1;});_tmpC6;});void*_tmp66[3];_tmp66[0]=& _tmp68,_tmp66[1]=& _tmp69,_tmp66[2]=& _tmp6A;({struct _fat_ptr _tmpF2=({const char*_tmp67="%s %s %s";_tag_fat(_tmp67,sizeof(char),9U);});Cyc_aprintf(_tmpF2,_tag_fat(_tmp66,sizeof(void*),3));});});}
# 376
struct _fat_ptr Cyc_Relations_relns2string(struct Cyc_List_List*rs){
if(rs == 0)return({const char*_tmp6B="true";_tag_fat(_tmp6B,sizeof(char),5U);});{
struct Cyc_List_List*ss=0;
for(1;rs != 0;rs=rs->tl){
ss=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct _fat_ptr*_tmpF4=({struct _fat_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct _fat_ptr _tmpF3=Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)rs->hd);*_tmp6C=_tmpF3;});_tmp6C;});_tmp6D->hd=_tmpF4;}),_tmp6D->tl=ss;_tmp6D;});
if(rs->tl != 0)ss=({struct Cyc_List_List*_tmp70=_cycalloc(sizeof(*_tmp70));({struct _fat_ptr*_tmpF6=({struct _fat_ptr*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct _fat_ptr _tmpF5=({const char*_tmp6E=" && ";_tag_fat(_tmp6E,sizeof(char),5U);});*_tmp6F=_tmpF5;});_tmp6F;});_tmp70->hd=_tmpF6;}),_tmp70->tl=ss;_tmp70;});}
# 383
return(struct _fat_ptr)Cyc_strconcat_l(ss);}}
# 386
void Cyc_Relations_print_relns(struct Cyc___cycFILE*f,struct Cyc_List_List*r){
for(1;r != 0;r=r->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmpC9;_tmpC9.tag=0,({struct _fat_ptr _tmpF7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)r->hd));_tmpC9.f1=_tmpF7;});_tmpC9;});void*_tmp71[1];_tmp71[0]=& _tmp73;({struct Cyc___cycFILE*_tmpF9=f;struct _fat_ptr _tmpF8=({const char*_tmp72="%s";_tag_fat(_tmp72,sizeof(char),3U);});Cyc_fprintf(_tmpF9,_tmpF8,_tag_fat(_tmp71,sizeof(void*),1));});});
if(r->tl != 0)({void*_tmp74=0U;({struct Cyc___cycFILE*_tmpFB=f;struct _fat_ptr _tmpFA=({const char*_tmp75=",";_tag_fat(_tmp75,sizeof(char),2U);});Cyc_fprintf(_tmpFB,_tmpFA,_tag_fat(_tmp74,sizeof(void*),0));});});}}
# 393
static union Cyc_Relations_RelnOp Cyc_Relations_reduce_relnop(union Cyc_Relations_RelnOp rop){
{union Cyc_Relations_RelnOp _tmp76=rop;void*_tmp77;switch((_tmp76.RNumelts).tag){case 4: _tmp77=(_tmp76.RType).val;{void*t=_tmp77;
# 396
{void*_tmp78=Cyc_Absyn_compress(t);void*_stmttmp9=_tmp78;void*_tmp79=_stmttmp9;void*_tmp7A;if(*((int*)_tmp79)== 9){_tmp7A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp79)->f1;{struct Cyc_Absyn_Exp*e=_tmp7A;
# 398
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(e,& rop2))return rop2;
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 403
goto _LL0;}case 3: _tmp77=(_tmp76.RNumelts).val;{struct Cyc_Absyn_Vardecl*v=_tmp77;
# 405
{void*_tmp7B=Cyc_Absyn_compress(v->type);void*_stmttmpA=_tmp7B;void*_tmp7C=_stmttmpA;void*_tmp7D;if(*((int*)_tmp7C)== 3){_tmp7D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C)->f1).ptr_atts).bounds;{void*b=_tmp7D;
# 409
if(({void*_tmpFC=b;_tmpFC == Cyc_Absyn_bounds_one();}))goto _LLC;{
struct Cyc_Absyn_Exp*_tmp7E=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp7E;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp7F=eopt;struct Cyc_Absyn_Exp*e=_tmp7F;
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(e,& rop2))return rop2;}
# 416
goto _LLC;}}}else{
goto _LLC;}_LLC:;}
# 419
goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 422
return rop;}
# 427
static int Cyc_Relations_consist_relations(struct Cyc_Pratt_Graph*G,struct Cyc_List_List*rlns){
for(1;(unsigned)G && rlns != 0;rlns=rlns->tl){
struct Cyc_Relations_Reln*_tmp80=(struct Cyc_Relations_Reln*)rlns->hd;struct Cyc_Relations_Reln*reln=_tmp80;
({union Cyc_Relations_RelnOp _tmpFD=Cyc_Relations_reduce_relnop(reln->rop1);reln->rop1=_tmpFD;});
({union Cyc_Relations_RelnOp _tmpFE=Cyc_Relations_reduce_relnop(reln->rop2);reln->rop2=_tmpFE;});{
# 437
struct Cyc_Relations_Reln*_tmp81=reln;union Cyc_Relations_RelnOp _tmp84;union Cyc_Relations_RelnOp _tmp82;unsigned _tmp86;enum Cyc_Relations_Relation _tmp83;unsigned _tmp85;if(((((struct Cyc_Relations_Reln*)_tmp81)->rop1).RConst).tag == 1){if(((((struct Cyc_Relations_Reln*)_tmp81)->rop2).RConst).tag == 1){_tmp85=((_tmp81->rop1).RConst).val;_tmp83=_tmp81->relation;_tmp86=((_tmp81->rop2).RConst).val;{unsigned i=_tmp85;enum Cyc_Relations_Relation reln=_tmp83;unsigned j=_tmp86;
# 439
{enum Cyc_Relations_Relation _tmp87=reln;switch((int)_tmp87){case Cyc_Relations_Req:
 if(i != j)return 0;goto _LLF;case Cyc_Relations_Rneq:
 if(i == j)return 0;goto _LLF;case Cyc_Relations_Rlt:
 if(i >= j)return 0;goto _LLF;case Cyc_Relations_Rlte:
 goto _LL19;default: _LL19: if(i > j)return 0;goto _LLF;}_LLF:;}
# 445
goto _LL0;}}else{if(((struct Cyc_Relations_Reln*)_tmp81)->relation == Cyc_Relations_Rneq){_tmp85=((_tmp81->rop1).RConst).val;_tmp82=_tmp81->rop2;_LL6: {unsigned i=_tmp85;union Cyc_Relations_RelnOp rop=_tmp82;
# 452
union Cyc_Pratt_Node _tmp88=Cyc_Relations_rop2node(rop);union Cyc_Pratt_Node n=_tmp88;
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=({struct Cyc_Pratt_Graph*_tmp102=G2;union Cyc_Pratt_Node _tmp101=n;union Cyc_Pratt_Node _tmp100=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp102,_tmp101,_tmp100,({struct Cyc_AP_T*_tmpFF=Cyc_AP_fromint((long)i);Cyc_AP_sub(_tmpFF,Cyc_AP_one);}));});
G=({struct Cyc_Pratt_Graph*_tmp106=G;union Cyc_Pratt_Node _tmp105=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp104=n;Cyc_Pratt_add_edge(_tmp106,_tmp105,_tmp104,Cyc_AP_neg(({struct Cyc_AP_T*_tmp103=Cyc_AP_fromint((long)i);Cyc_AP_add(_tmp103,Cyc_AP_one);})));});
# 462
if(G != 0 && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl)||
 Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}else{_tmp85=((_tmp81->rop1).RConst).val;_tmp83=_tmp81->relation;_tmp82=_tmp81->rop2;{unsigned i=_tmp85;enum Cyc_Relations_Relation reln=_tmp83;union Cyc_Relations_RelnOp rop=_tmp82;
# 496
union Cyc_Pratt_Node _tmp8C=Cyc_Relations_rop2node(rop);union Cyc_Pratt_Node n=_tmp8C;
if((int)reln == 3)i=i + 1U;
G=({struct Cyc_Pratt_Graph*_tmp109=G;union Cyc_Pratt_Node _tmp108=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp107=n;Cyc_Pratt_add_edge(_tmp109,_tmp108,_tmp107,Cyc_AP_neg(Cyc_AP_fromint((long)i)));});
# 501
if((unsigned)G &&(int)reln == 0)
# 506
G=({struct Cyc_Pratt_Graph*_tmp10C=G;union Cyc_Pratt_Node _tmp10B=n;union Cyc_Pratt_Node _tmp10A=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp10C,_tmp10B,_tmp10A,Cyc_AP_fromint((long)i));});
# 513
goto _LL0;}}}}else{if(((struct Cyc_Relations_Reln*)_tmp81)->relation == Cyc_Relations_Rneq){if(((((struct Cyc_Relations_Reln*)_tmp81)->rop2).RConst).tag == 1){_tmp82=_tmp81->rop1;_tmp85=((_tmp81->rop2).RConst).val;{union Cyc_Relations_RelnOp rop=_tmp82;unsigned i=_tmp85;
# 450
_tmp85=i;_tmp82=rop;goto _LL6;}}else{_tmp82=_tmp81->rop1;_tmp84=_tmp81->rop2;{union Cyc_Relations_RelnOp rop1=_tmp82;union Cyc_Relations_RelnOp rop2=_tmp84;
# 469
union Cyc_Pratt_Node _tmp89=Cyc_Relations_rop2node(rop1);union Cyc_Pratt_Node n1=_tmp89;
union Cyc_Pratt_Node _tmp8A=Cyc_Relations_rop2node(rop2);union Cyc_Pratt_Node n2=_tmp8A;
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=({struct Cyc_Pratt_Graph*_tmp10F=G2;union Cyc_Pratt_Node _tmp10E=n1;union Cyc_Pratt_Node _tmp10D=n2;Cyc_Pratt_add_edge(_tmp10F,_tmp10E,_tmp10D,Cyc_AP_neg(Cyc_AP_one));});
G=({struct Cyc_Pratt_Graph*_tmp112=G;union Cyc_Pratt_Node _tmp111=n2;union Cyc_Pratt_Node _tmp110=n1;Cyc_Pratt_add_edge(_tmp112,_tmp111,_tmp110,Cyc_AP_neg(Cyc_AP_one));});
if(G != 0 && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl)||
 Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}}else{if(((((struct Cyc_Relations_Reln*)_tmp81)->rop2).RConst).tag == 1){_tmp82=_tmp81->rop1;_tmp83=_tmp81->relation;_tmp85=((_tmp81->rop2).RConst).val;{union Cyc_Relations_RelnOp rop=_tmp82;enum Cyc_Relations_Relation reln=_tmp83;unsigned i=_tmp85;
# 483
union Cyc_Pratt_Node _tmp8B=Cyc_Relations_rop2node(rop);union Cyc_Pratt_Node n=_tmp8B;
if((int)reln == 3)i=i - 1U;
G=({struct Cyc_Pratt_Graph*_tmp115=G;union Cyc_Pratt_Node _tmp114=n;union Cyc_Pratt_Node _tmp113=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp115,_tmp114,_tmp113,Cyc_AP_fromint((long)i));});
# 487
if((unsigned)G &&(int)reln == 0)
G=({struct Cyc_Pratt_Graph*_tmp118=G;union Cyc_Pratt_Node _tmp117=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp116=n;Cyc_Pratt_add_edge(_tmp118,_tmp117,_tmp116,Cyc_AP_neg(Cyc_AP_fromint((long)i)));});
goto _LL0;}}else{_tmp82=_tmp81->rop1;_tmp83=_tmp81->relation;_tmp84=_tmp81->rop2;{union Cyc_Relations_RelnOp rop1=_tmp82;enum Cyc_Relations_Relation reln=_tmp83;union Cyc_Relations_RelnOp rop2=_tmp84;
# 516
union Cyc_Pratt_Node _tmp8D=Cyc_Relations_rop2node(rop1);union Cyc_Pratt_Node n1=_tmp8D;
union Cyc_Pratt_Node _tmp8E=Cyc_Relations_rop2node(rop2);union Cyc_Pratt_Node n2=_tmp8E;
int i=(int)reln == 3?-1: 0;
# 521
G=({struct Cyc_Pratt_Graph*_tmp11B=G;union Cyc_Pratt_Node _tmp11A=n1;union Cyc_Pratt_Node _tmp119=n2;Cyc_Pratt_add_edge(_tmp11B,_tmp11A,_tmp119,Cyc_AP_fromint(i));});
# 523
if((unsigned)G &&(int)reln == 0)
G=({struct Cyc_Pratt_Graph*_tmp11E=G;union Cyc_Pratt_Node _tmp11D=n2;union Cyc_Pratt_Node _tmp11C=n1;Cyc_Pratt_add_edge(_tmp11E,_tmp11D,_tmp11C,Cyc_AP_fromint(i));});
goto _LL0;}}}}_LL0:;}}
# 528
if(G != 0)return 1;else{return 0;}}
# 536
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns){
# 542
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
return Cyc_Relations_consist_relations(G,rlns);}
# 548
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(1;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp8F=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);struct Cyc_Relations_Reln*r=_tmp8F;
struct Cyc_List_List*_tmp90=({struct Cyc_List_List*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->hd=r,_tmp91->tl=r1;_tmp91;});struct Cyc_List_List*relns=_tmp90;
if(Cyc_Relations_consistent_relations(relns))return 0;}
# 554
return 1;}
# 560
struct _tuple11 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2){
# 562
enum Cyc_Absyn_Primop _tmp92=p;switch((int)_tmp92){case Cyc_Absyn_Eq:
 return({struct _tuple11 _tmpCA;_tmpCA.f1=e1,_tmpCA.f2=0U,_tmpCA.f3=e2;_tmpCA;});case Cyc_Absyn_Neq:
 return({struct _tuple11 _tmpCB;_tmpCB.f1=e1,_tmpCB.f2=1U,_tmpCB.f3=e2;_tmpCB;});case Cyc_Absyn_Lt:
 return({struct _tuple11 _tmpCC;_tmpCC.f1=e1,_tmpCC.f2=3U,_tmpCC.f3=e2;_tmpCC;});case Cyc_Absyn_Lte:
 return({struct _tuple11 _tmpCD;_tmpCD.f1=e1,_tmpCD.f2=2U,_tmpCD.f3=e2;_tmpCD;});case Cyc_Absyn_Gt:
 return({struct _tuple11 _tmpCE;_tmpCE.f1=e2,_tmpCE.f2=3U,_tmpCE.f3=e1;_tmpCE;});case Cyc_Absyn_Gte:
 return({struct _tuple11 _tmpCF;_tmpCF.f1=e2,_tmpCF.f2=2U,_tmpCF.f3=e1;_tmpCF;});default:
({void*_tmp93=0U;({int(*_tmp120)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp11F=({const char*_tmp94="primop2relation";_tag_fat(_tmp94,sizeof(char),16U);});_tmp120(_tmp11F,_tag_fat(_tmp93,sizeof(void*),0));});});};}
# 574
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmp95=r;switch((int)_tmp95){case Cyc_Relations_Req:
 return 1U;case Cyc_Relations_Rneq:
 return 0U;case Cyc_Relations_Rlt:
 return 2U;case Cyc_Relations_Rlte:
 goto _LLA;default: _LLA: return 3U;};}
# 583
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*r,struct Cyc_Relations_Reln*rln){
return({struct Cyc_Relations_Reln*_tmp96=_region_malloc(r,sizeof(*_tmp96));_tmp96->rop1=rln->rop2,({enum Cyc_Relations_Relation _tmp121=Cyc_Relations_flip_relation(rln->relation);_tmp96->relation=_tmp121;}),_tmp96->rop2=rln->rop1;_tmp96;});}
# 594
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 596
{void*_tmp97=e->r;void*_stmttmpB=_tmp97;void*_tmp98=_stmttmpB;enum Cyc_Absyn_Primop _tmp99;void*_tmp9B;void*_tmp9A;switch(*((int*)_tmp98)){case 14: _tmp9A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_tmp9B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp98)->f2;{void*t=_tmp9A;struct Cyc_Absyn_Exp*e=_tmp9B;
# 599
{void*_tmp9C=Cyc_Absyn_compress(t);void*_stmttmpC=_tmp9C;void*_tmp9D=_stmttmpC;if(*((int*)_tmp9D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9D)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9D)->f1)->f2 == Cyc_Absyn_Int_sz)
return Cyc_Relations_exp2relns(r,e);else{goto _LLE;}}else{goto _LLE;}}else{_LLE:
 goto _LLB;}_LLB:;}
# 603
goto _LL0;}case 7: _tmp9A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_tmp9B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp98)->f2;{struct Cyc_Absyn_Exp*e1=_tmp9A;struct Cyc_Absyn_Exp*e2=_tmp9B;
return({struct Cyc_List_List*(*_tmp124)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rappend;});struct _RegionHandle*_tmp123=r;struct Cyc_List_List*_tmp122=Cyc_Relations_exp2relns(r,e1);_tmp124(_tmp123,_tmp122,Cyc_Relations_exp2relns(r,e2));});}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2 != 0){if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd)->r)== 3){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd)->r)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd)->r)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd)->r)->f2)->tl)->tl == 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl == 0){_tmp99=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd)->r)->f1;_tmp9A=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd)->r)->f2)->hd;_tmp9B=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd)->r)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmp99;struct Cyc_Absyn_Exp*e1=_tmp9A;struct Cyc_Absyn_Exp*e2=_tmp9B;
# 606
{enum Cyc_Absyn_Primop _tmp9E=p;switch((int)_tmp9E){case Cyc_Absyn_Eq:
 goto _LL14;case Cyc_Absyn_Neq: _LL14: goto _LL16;case Cyc_Absyn_Lt: _LL16: goto _LL18;case Cyc_Absyn_Lte: _LL18: goto _LL1A;case Cyc_Absyn_Gt: _LL1A: goto _LL1C;case Cyc_Absyn_Gte: _LL1C: {
struct _tuple11 _tmp9F=Cyc_Relations_primop2relation(e1,p,e2);struct _tuple11 _stmttmpD=_tmp9F;struct _tuple11 _tmpA0=_stmttmpD;void*_tmpA3;enum Cyc_Relations_Relation _tmpA2;void*_tmpA1;_tmpA1=_tmpA0.f1;_tmpA2=_tmpA0.f2;_tmpA3=_tmpA0.f3;{struct Cyc_Absyn_Exp*e1=_tmpA1;enum Cyc_Relations_Relation rln=_tmpA2;struct Cyc_Absyn_Exp*e2=_tmpA3;
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e1,& rop1)&& Cyc_Relations_exp2relnop(e2,& rop2))
return({struct Cyc_List_List*_tmpA5=_region_malloc(r,sizeof(*_tmpA5));({struct Cyc_Relations_Reln*_tmp126=({struct Cyc_Relations_Reln*_tmpA4=_region_malloc(r,sizeof(*_tmpA4));_tmpA4->rop1=rop2,({enum Cyc_Relations_Relation _tmp125=Cyc_Relations_flip_relation(rln);_tmpA4->relation=_tmp125;}),_tmpA4->rop2=rop1;_tmpA4;});_tmpA5->hd=_tmp126;}),_tmpA5->tl=0;_tmpA5;});
goto _LL10;}}default:
 goto _LL10;}_LL10:;}
# 616
goto _LL0;}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->tl == 0){_LL7: _tmp99=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_tmp9A=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd;_tmp9B=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmp99;struct Cyc_Absyn_Exp*e1=_tmp9A;struct Cyc_Absyn_Exp*e2=_tmp9B;
# 618
{enum Cyc_Absyn_Primop _tmpA6=p;switch((int)_tmpA6){case Cyc_Absyn_Eq:
 goto _LL26;case Cyc_Absyn_Neq: _LL26: goto _LL28;case Cyc_Absyn_Lt: _LL28: goto _LL2A;case Cyc_Absyn_Lte: _LL2A: goto _LL2C;case Cyc_Absyn_Gt: _LL2C: goto _LL2E;case Cyc_Absyn_Gte: _LL2E: {
struct _tuple11 _tmpA7=Cyc_Relations_primop2relation(e1,p,e2);struct _tuple11 _stmttmpE=_tmpA7;struct _tuple11 _tmpA8=_stmttmpE;void*_tmpAB;enum Cyc_Relations_Relation _tmpAA;void*_tmpA9;_tmpA9=_tmpA8.f1;_tmpAA=_tmpA8.f2;_tmpAB=_tmpA8.f3;{struct Cyc_Absyn_Exp*e1=_tmpA9;enum Cyc_Relations_Relation rln=_tmpAA;struct Cyc_Absyn_Exp*e2=_tmpAB;
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e1,& rop1)&& Cyc_Relations_exp2relnop(e2,& rop2))
return({struct Cyc_List_List*_tmpAD=_region_malloc(r,sizeof(*_tmpAD));({struct Cyc_Relations_Reln*_tmp127=({struct Cyc_Relations_Reln*_tmpAC=_region_malloc(r,sizeof(*_tmpAC));_tmpAC->rop1=rop1,_tmpAC->relation=rln,_tmpAC->rop2=rop2;_tmpAC;});_tmpAD->hd=_tmp127;}),_tmpAD->tl=0;_tmpAD;});
goto _LL22;}}default:
 goto _LL22;}_LL22:;}
# 628
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}}default: _LL9:
 goto _LL0;}_LL0:;}
# 632
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple12 _tmpAE=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmpF=_tmpAE;struct _tuple12 _tmpAF=_stmttmpF;int _tmpB1;unsigned _tmpB0;_tmpB0=_tmpAF.f1;_tmpB1=_tmpAF.f2;{unsigned i=_tmpB0;int known=_tmpB1;
if(known){
if((int)i)return 0;else{
# 637
return({struct Cyc_List_List*_tmpB3=_region_malloc(r,sizeof(*_tmpB3));({struct Cyc_Relations_Reln*_tmp12A=({struct Cyc_Relations_Reln*_tmpB2=_region_malloc(r,sizeof(*_tmpB2));({union Cyc_Relations_RelnOp _tmp129=Cyc_Relations_RConst(0U);_tmpB2->rop1=_tmp129;}),_tmpB2->relation=3U,({union Cyc_Relations_RelnOp _tmp128=Cyc_Relations_RConst(0U);_tmpB2->rop2=_tmp128;});_tmpB2;});_tmpB3->hd=_tmp12A;}),_tmpB3->tl=0;_tmpB3;});}}
# 639
({struct Cyc_String_pa_PrintArg_struct _tmpB6=({struct Cyc_String_pa_PrintArg_struct _tmpD0;_tmpD0.tag=0,({
struct _fat_ptr _tmp12B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmpD0.f1=_tmp12B;});_tmpD0;});void*_tmpB4[1];_tmpB4[0]=& _tmpB6;({unsigned _tmp12D=e->loc;struct _fat_ptr _tmp12C=({const char*_tmpB5="unable to convert `%s' to static relation";_tag_fat(_tmpB5,sizeof(char),42U);});Cyc_Warn_err(_tmp12D,_tmp12C,_tag_fat(_tmpB4,sizeof(void*),1));});});}}
# 643
return({struct Cyc_List_List*_tmpB8=_region_malloc(r,sizeof(*_tmpB8));({struct Cyc_Relations_Reln*_tmp130=({struct Cyc_Relations_Reln*_tmpB7=_region_malloc(r,sizeof(*_tmpB7));({union Cyc_Relations_RelnOp _tmp12F=Cyc_Relations_RConst(0U);_tmpB7->rop1=_tmp12F;}),_tmpB7->relation=3U,({union Cyc_Relations_RelnOp _tmp12E=Cyc_Relations_RConst(0U);_tmpB7->rop2=_tmp12E;});_tmpB7;});_tmpB8->hd=_tmp130;}),_tmpB8->tl=0;_tmpB8;});}

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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172 "list.h"
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 50 "string.h"
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 66
struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 289
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 302
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 691 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 874
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 911
extern void*Cyc_Absyn_heap_rgn_type;
# 915
extern void*Cyc_Absyn_false_type;
# 917
extern void*Cyc_Absyn_void_type;
# 933
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud();
# 962
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);
# 1068
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1077
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};
# 58
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 60
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 29 "binding.h"
extern int Cyc_Binding_warn_override;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 34 "cifc.h"
void Cyc_Cifc_user_overrides(unsigned,struct Cyc_List_List*ds,struct Cyc_List_List*ovrs);char Cyc_Binding_BindingError[13U]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 44 "binding.cyc"
int Cyc_Binding_warn_override=0;
# 58
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);void*data=_tmp0;
return({struct Cyc_Binding_NSCtxt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->curr_ns=0,({
struct Cyc_List_List*_tmp2F4=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({void*_tmp2F3=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0U,_tmp1->f1=0;_tmp1;});_tmp2->hd=_tmp2F3;}),_tmp2->tl=0;_tmp2;});_tmp3->availables=_tmp2F4;}),({
struct Cyc_Dict_Dict _tmp2F2=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,data);_tmp3->ns_data=_tmp2F2;});_tmp3;});}
# 64
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp4=abs_ns;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp6;switch((_tmp4.Abs_n).tag){case 3U: _LL1: _tmp6=(_tmp4.C_n).val;_LL2: {struct Cyc_List_List*vs=_tmp6;
_tmp5=vs;goto _LL4;}case 2U: _LL3: _tmp5=(_tmp4.Abs_n).val;_LL4: {struct Cyc_List_List*vs=_tmp5;
return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,vs);}default: _LL5: _LL6:
({void*_tmp7=0U;({struct _fat_ptr _tmp2F5=({const char*_tmp8="Binding:get_ns_data: relative ns";_tag_fat(_tmp8,sizeof(char),33U);});Cyc_Warn_impos(_tmp2F5,_tag_fat(_tmp7,sizeof(void*),0U));});});}_LL0:;}
# 82 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 85
struct Cyc_List_List*_tmp9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);struct Cyc_List_List*fullname=_tmp9;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,fullname))
return fullname;
{struct Cyc_List_List*_tmpA=ctxt->availables;struct Cyc_List_List*as=_tmpA;for(0;as != 0;as=as->tl){
void*_tmpB=(void*)as->hd;void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->tag == 1U){_LL1: _tmpE=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->f1;_LL2: {struct Cyc_List_List*ns=_tmpE;
_tmpD=ns;goto _LL4;}}else{_LL3: _tmpD=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpC)->f1;_LL4: {struct Cyc_List_List*ns=_tmpD;
# 92
struct Cyc_List_List*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ns,rel_ns);struct Cyc_List_List*fullname=_tmpF;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,fullname))
return fullname;
goto _LL0;}}_LL0:;}}
# 97
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3;_tmp2C3.tag=0U,({struct _fat_ptr _tmp2F6=({const char*_tmp16="namespace ";_tag_fat(_tmp16,sizeof(char),11U);});_tmp2C3.f1=_tmp2F6;});_tmp2C3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2;_tmp2C2.tag=0U,({struct _fat_ptr _tmp2F8=(struct _fat_ptr)({struct Cyc_List_List*_tmp2F7=rel_ns;Cyc_str_sepstr(_tmp2F7,({const char*_tmp15="::";_tag_fat(_tmp15,sizeof(char),3U);}));});_tmp2C2.f1=_tmp2F8;});_tmp2C2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1;_tmp2C1.tag=0U,({struct _fat_ptr _tmp2F9=({const char*_tmp14=" not found";_tag_fat(_tmp14,sizeof(char),11U);});_tmp2C1.f1=_tmp2F9;});_tmp2C1;});void*_tmp10[3U];_tmp10[0]=& _tmp11,_tmp10[1]=& _tmp12,_tmp10[2]=& _tmp13;({unsigned _tmp2FA=loc;Cyc_Warn_err2(_tmp2FA,_tag_fat(_tmp10,sizeof(void*),3U));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=Cyc_Binding_BindingError;_tmp17;}));}
# 102
void*Cyc_Binding_resolve_lookup(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _fat_ptr*)){
struct _tuple0*_tmp18=qv;struct _fat_ptr*_tmp1A;union Cyc_Absyn_Nmspace _tmp19;_LL1: _tmp19=_tmp18->f1;_tmp1A=_tmp18->f2;_LL2: {union Cyc_Absyn_Nmspace ns=_tmp19;struct _fat_ptr*v=_tmp1A;
union Cyc_Absyn_Nmspace _tmp1B=ns;struct Cyc_List_List*_tmp1C;struct Cyc_List_List*_tmp1D;struct Cyc_List_List*_tmp1E;switch((_tmp1B.Abs_n).tag){case 1U: if((_tmp1B.Rel_n).val == 0){_LL4: _LL5:
# 106
{struct _handler_cons _tmp1F;_push_handler(& _tmp1F);{int _tmp21=0;if(setjmp(_tmp1F.handler))_tmp21=1;if(!_tmp21){{void*_tmp22=({void*(*_tmp2FC)(void*,struct _fat_ptr*)=lookup;void*_tmp2FB=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp2FC(_tmp2FB,v);});_npop_handler(0U);return _tmp22;};_pop_handler();}else{void*_tmp20=(void*)Cyc_Core_get_exn_thrown();void*_tmp23=_tmp20;void*_tmp24;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp23)->tag == Cyc_Dict_Absent){_LLF: _LL10:
 goto _LLE;}else{_LL11: _tmp24=_tmp23;_LL12: {void*exn=_tmp24;(int)_rethrow(exn);}}_LLE:;}}}
{struct Cyc_List_List*_tmp25=ctxt->availables;struct Cyc_List_List*as=_tmp25;for(0;as != 0;as=as->tl){
void*_tmp26=(void*)as->hd;void*_stmttmp1=_tmp26;void*_tmp27=_stmttmp1;struct Cyc_List_List*_tmp28;struct Cyc_List_List*_tmp29;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp27)->tag == 1U){_LL14: _tmp29=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp27)->f1;_LL15: {struct Cyc_List_List*ns=_tmp29;
_tmp28=ns;goto _LL17;}}else{_LL16: _tmp28=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp27)->f1;_LL17: {struct Cyc_List_List*ns=_tmp28;
# 112
{struct _handler_cons _tmp2A;_push_handler(& _tmp2A);{int _tmp2C=0;if(setjmp(_tmp2A.handler))_tmp2C=1;if(!_tmp2C){{void*_tmp2D=({void*(*_tmp2FE)(void*,struct _fat_ptr*)=lookup;void*_tmp2FD=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ns);_tmp2FE(_tmp2FD,v);});_npop_handler(0U);return _tmp2D;};_pop_handler();}else{void*_tmp2B=(void*)Cyc_Core_get_exn_thrown();void*_tmp2E=_tmp2B;void*_tmp2F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2E)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
 goto _LL18;}else{_LL1B: _tmp2F=_tmp2E;_LL1C: {void*exn=_tmp2F;(int)_rethrow(exn);}}_LL18:;}}}
goto _LL13;}}_LL13:;}}
# 116
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=Cyc_Binding_BindingError;_tmp30;}));}else{_LL6: _tmp1E=(_tmp1B.Rel_n).val;_LL7: {struct Cyc_List_List*ns=_tmp1E;
# 118
struct _handler_cons _tmp31;_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!_tmp33){
{struct Cyc_List_List*_tmp34=Cyc_Binding_resolve_rel_ns(loc,ctxt,ns);struct Cyc_List_List*abs_ns=_tmp34;
void*_tmp35=({void*(*_tmp300)(void*,struct _fat_ptr*)=lookup;void*_tmp2FF=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,abs_ns);_tmp300(_tmp2FF,v);});_npop_handler(0U);return _tmp35;}
# 119
;_pop_handler();}else{void*_tmp32=(void*)Cyc_Core_get_exn_thrown();void*_tmp36=_tmp32;void*_tmp37;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp36)->tag == Cyc_Dict_Absent){_LL1E: _LL1F:
# 121
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->tag=Cyc_Binding_BindingError;_tmp38;}));}else{_LL20: _tmp37=_tmp36;_LL21: {void*exn=_tmp37;(int)_rethrow(exn);}}_LL1D:;}}}}case 3U: _LL8: _tmp1D=(_tmp1B.C_n).val;_LL9: {struct Cyc_List_List*ns=_tmp1D;
_tmp1C=ns;goto _LLB;}case 2U: _LLA: _tmp1C=(_tmp1B.Abs_n).val;_LLB: {struct Cyc_List_List*ns=_tmp1C;
# 124
struct _handler_cons _tmp39;_push_handler(& _tmp39);{int _tmp3B=0;if(setjmp(_tmp39.handler))_tmp3B=1;if(!_tmp3B){{void*_tmp3C=({void*(*_tmp302)(void*,struct _fat_ptr*)=lookup;void*_tmp301=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ns);_tmp302(_tmp301,v);});_npop_handler(0U);return _tmp3C;};_pop_handler();}else{void*_tmp3A=(void*)Cyc_Core_get_exn_thrown();void*_tmp3D=_tmp3A;void*_tmp3E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3D)->tag == Cyc_Dict_Absent){_LL23: _LL24:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->tag=Cyc_Binding_BindingError;_tmp3F;}));}else{_LL25: _tmp3E=_tmp3D;_LL26: {void*exn=_tmp3E;(int)_rethrow(exn);}}_LL22:;}}}default: _LLC: _LLD:
({void*_tmp40=0U;({struct _fat_ptr _tmp303=({const char*_tmp41="lookup local in global";_tag_fat(_tmp41,sizeof(char),23U);});Cyc_Warn_impos(_tmp303,_tag_fat(_tmp40,sizeof(void*),0U));});});}_LL3:;}}
# 130
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp42=ctxt->curr_ns;struct Cyc_List_List*ns=_tmp42;
struct Cyc_List_List*_tmp43=({struct Cyc_List_List*_tmp304=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp304,({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->hd=subname,_tmp46->tl=0;_tmp46;}));});struct Cyc_List_List*ns2=_tmp43;
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,ns2))
({struct Cyc_Dict_Dict _tmp307=({struct Cyc_Dict_Dict _tmp306=ctxt->ns_data;struct Cyc_List_List*_tmp305=ns2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(_tmp306,_tmp305,mkdata(env));});ctxt->ns_data=_tmp307;});
ctxt->curr_ns=ns2;
({struct Cyc_List_List*_tmp309=({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));({void*_tmp308=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=0U,_tmp44->f1=ns2;_tmp44;});_tmp45->hd=_tmp308;}),_tmp45->tl=ctxt->availables;_tmp45;});ctxt->availables=_tmp309;});}
# 138
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp47=0U;({struct _fat_ptr _tmp30A=({const char*_tmp48="leaving topmost namespace";_tag_fat(_tmp48,sizeof(char),26U);});Cyc_Warn_impos(_tmp30A,_tag_fat(_tmp47,sizeof(void*),0U));});});{
void*_tmp49=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_stmttmp2=_tmp49;void*_tmp4A=_stmttmp2;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp4A)->tag == 0U){_LL1: _LL2:
# 143
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp30B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp30B;});
goto _LL0;}else{_LL3: _LL4:
# 147
({void*_tmp4B=0U;({struct _fat_ptr _tmp30C=({const char*_tmp4C="leaving using as namespace";_tag_fat(_tmp4C,sizeof(char),27U);});Cyc_Warn_impos(_tmp30C,_tag_fat(_tmp4B,sizeof(void*),0U));});});}_LL0:;}}
# 150
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp4D=usename;struct _fat_ptr*_tmp4F;union Cyc_Absyn_Nmspace _tmp4E;_LL1: _tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;_LL2: {union Cyc_Absyn_Nmspace nsl=_tmp4E;struct _fat_ptr*v=_tmp4F;
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp50=nsl;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;switch((_tmp50.Loc_n).tag){case 1U: _LL4: _tmp52=(_tmp50.Rel_n).val;_LL5: {struct Cyc_List_List*vs=_tmp52;
# 155
ns=({unsigned _tmp30F=loc;struct Cyc_Binding_NSCtxt*_tmp30E=ctxt;Cyc_Binding_resolve_rel_ns(_tmp30F,_tmp30E,({struct Cyc_List_List*_tmp30D=vs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30D,({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=v,_tmp53->tl=0;_tmp53;}));}));});{
struct Cyc_List_List*_tmp54=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);struct Cyc_List_List*abs_vs=_tmp54;
({union Cyc_Absyn_Nmspace _tmp310=Cyc_Absyn_Abs_n(abs_vs,0);(*usename).f1=_tmp310;});
goto _LL3;}}case 2U: _LL6: _tmp51=(_tmp50.Abs_n).val;_LL7: {struct Cyc_List_List*vs=_tmp51;
# 160
ns=({struct Cyc_List_List*_tmp311=vs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp311,({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=v,_tmp55->tl=0;_tmp55;}));});
goto _LL3;}case 4U: _LL8: _LL9:
({void*_tmp56=0U;({struct _fat_ptr _tmp312=({const char*_tmp57="enter_using, Loc_n";_tag_fat(_tmp57,sizeof(char),19U);});Cyc_Warn_impos(_tmp312,_tag_fat(_tmp56,sizeof(void*),0U));});});default: _LLA: _LLB:
({void*_tmp58=0U;({struct _fat_ptr _tmp313=({const char*_tmp59="enter_using, C_n";_tag_fat(_tmp59,sizeof(char),17U);});Cyc_Warn_impos(_tmp313,_tag_fat(_tmp58,sizeof(void*),0U));});});}_LL3:;}
# 165
({struct Cyc_List_List*_tmp315=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));({void*_tmp314=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=1U,_tmp5A->f1=ns;_tmp5A;});_tmp5B->hd=_tmp314;}),_tmp5B->tl=ctxt->availables;_tmp5B;});ctxt->availables=_tmp315;});
return ns;}}
# 168
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp5C=0U;({struct _fat_ptr _tmp316=({const char*_tmp5D="leaving topmost namespace as a using";_tag_fat(_tmp5D,sizeof(char),37U);});Cyc_Warn_impos(_tmp316,_tag_fat(_tmp5C,sizeof(void*),0U));});});{
void*_tmp5E=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_stmttmp3=_tmp5E;void*_tmp5F=_stmttmp3;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp5F)->tag == 1U){_LL1: _LL2:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL0;}else{_LL3: _LL4:
({void*_tmp60=0U;({struct _fat_ptr _tmp317=({const char*_tmp61="leaving namespace as using";_tag_fat(_tmp61,sizeof(char),27U);});Cyc_Warn_impos(_tmp317,_tag_fat(_tmp60,sizeof(void*),0U));});});}_LL0:;}}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 212 "binding.cyc"
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 215
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 218
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp62=_cycalloc(sizeof(*_tmp62));({struct Cyc_Dict_Dict _tmp31C=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->aggrdecls=_tmp31C;}),({
struct Cyc_Dict_Dict _tmp31B=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->datatypedecls=_tmp31B;}),({
struct Cyc_Dict_Dict _tmp31A=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->enumdecls=_tmp31A;}),({
struct Cyc_Dict_Dict _tmp319=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->typedefs=_tmp319;}),({
struct Cyc_Dict_Dict _tmp318=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->ordinaries=_tmp318;});_tmp62;});}
# 226
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->aggrdecls,v);}
# 229
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->datatypedecls,v);}
# 232
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->enumdecls,v);}
# 235
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->typedefs,v);}
# 238
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->ordinaries,v);}
# 241
static void*Cyc_Binding_lookup_ordinary(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
struct _tuple0*_tmp63=qv;struct _fat_ptr*_tmp65;union Cyc_Absyn_Nmspace _tmp64;_LL1: _tmp64=_tmp63->f1;_tmp65=_tmp63->f2;_LL2: {union Cyc_Absyn_Nmspace nsl=_tmp64;struct _fat_ptr*v=_tmp65;
union Cyc_Absyn_Nmspace _tmp66=nsl;switch((_tmp66.Rel_n).tag){case 4U: _LL4: _LL5:
# 245
 if(Cyc_Binding_at_toplevel(env)|| !((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=Cyc_Binding_BindingError;_tmp67;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v);case 1U: if((_tmp66.Rel_n).val == 0){_LL6: _LL7:
# 249
 if(!Cyc_Binding_at_toplevel(env)&&((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v))
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 253
 return((void*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL3:;}}
# 257
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*,struct Cyc_Absyn_Decl*);
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*,struct Cyc_Absyn_Pat*);
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);
void Cyc_Binding_resolve_type(unsigned,struct Cyc_Binding_Env*,void*);
void Cyc_Binding_resolve_rgnpo(unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_aggrfields(unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo i);
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int,unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*);
# 272
void Cyc_Binding_absolutize_decl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp68=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp4=_tmp68;union Cyc_Absyn_Nmspace _tmp69=_stmttmp4;switch((_tmp69.Abs_n).tag){case 1U: if((_tmp69.Rel_n).val == 0){_LL1: _LL2:
# 275
 if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _tmp31E=({struct Cyc_List_List*_tmp31D=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp31D,Cyc_Binding_in_cinclude(env)||(int)sc == (int)Cyc_Absyn_ExternC);});(*qv).f1=_tmp31E;});else{
# 278
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL0;}else{goto _LL7;}case 4U: _LL3: _LL4:
 goto _LL0;case 2U: _LL5: _LL6:
# 282
 if(!Cyc_Binding_at_toplevel(env))
goto _LL8;
goto _LL0;default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C6;_tmp2C6.tag=0U,({struct _fat_ptr _tmp31F=({const char*_tmp6F="qualified names in declarations unimplemented (";_tag_fat(_tmp6F,sizeof(char),48U);});_tmp2C6.f1=_tmp31F;});_tmp2C6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2C5;_tmp2C5.tag=3U,_tmp2C5.f1=qv;_tmp2C5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C4;_tmp2C4.tag=0U,({struct _fat_ptr _tmp320=({const char*_tmp6E=")";_tag_fat(_tmp6E,sizeof(char),2U);});_tmp2C4.f1=_tmp320;});_tmp2C4;});void*_tmp6A[3U];_tmp6A[0]=& _tmp6B,_tmp6A[1]=& _tmp6C,_tmp6A[2]=& _tmp6D;({unsigned _tmp321=loc;Cyc_Warn_err2(_tmp321,_tag_fat(_tmp6A,sizeof(void*),3U));});});}_LL0:;}
# 291
void Cyc_Binding_absolutize_topdecl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp70=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp70;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=old_locals;}
# 298
void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _fat_ptr hides_what;
struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){
{void*_tmp74=Cyc_Binding_lookup_ordinary(loc,env,q);void*_stmttmp5=_tmp74;void*_tmp75=_stmttmp5;void*_tmp76;switch(*((int*)_tmp75)){case 0U: _LL1: _tmp76=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp75)->f1;_LL2: {void*b=_tmp76;
# 303
if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0U);return;}
{void*_tmp77=b;switch(*((int*)_tmp77)){case 1U: _LLC: _LLD:
 hides_what=({const char*_tmp78="global variable";_tag_fat(_tmp78,sizeof(char),16U);});goto _LLB;case 4U: _LLE: _LLF:
 hides_what=({const char*_tmp79="local variable";_tag_fat(_tmp79,sizeof(char),15U);});goto _LLB;case 3U: _LL10: _LL11:
 hides_what=({const char*_tmp7A="parameter";_tag_fat(_tmp7A,sizeof(char),10U);});goto _LLB;case 5U: _LL12: _LL13:
 hides_what=({const char*_tmp7B="pattern variable";_tag_fat(_tmp7B,sizeof(char),17U);});goto _LLB;case 2U: _LL14: _LL15:
 hides_what=({const char*_tmp7C="function";_tag_fat(_tmp7C,sizeof(char),9U);});goto _LLB;default: _LL16: _LL17:
({void*_tmp7D=0U;({struct _fat_ptr _tmp322=({const char*_tmp7E="shadowing free variable!";_tag_fat(_tmp7E,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp322,_tag_fat(_tmp7D,sizeof(void*),0U));});});}_LLB:;}
# 313
goto _LL0;}case 1U: _LL3: _LL4:
 hides_what=({const char*_tmp7F="struct constructor";_tag_fat(_tmp7F,sizeof(char),19U);});goto _LL0;case 2U: _LL5: _LL6:
 hides_what=({const char*_tmp80="datatype constructor";_tag_fat(_tmp80,sizeof(char),21U);});goto _LL0;case 3U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 hides_what=({const char*_tmp81="enum tag";_tag_fat(_tmp81,sizeof(char),9U);});goto _LL0;}_LL0:;}
# 319
({struct Cyc_Warn_String_Warn_Warg_struct _tmp83=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8;_tmp2C8.tag=0U,({struct _fat_ptr _tmp323=({const char*_tmp85="declaration hides ";_tag_fat(_tmp85,sizeof(char),19U);});_tmp2C8.f1=_tmp323;});_tmp2C8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C7;_tmp2C7.tag=0U,_tmp2C7.f1=hides_what;_tmp2C7;});void*_tmp82[2U];_tmp82[0]=& _tmp83,_tmp82[1]=& _tmp84;({unsigned _tmp324=loc;Cyc_Warn_warn2(_tmp324,_tag_fat(_tmp82,sizeof(void*),2U));});});
_npop_handler(0U);return;
# 301
;_pop_handler();}else{void*_tmp72=(void*)Cyc_Core_get_exn_thrown();void*_tmp86=_tmp72;void*_tmp87;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp86)->tag == Cyc_Binding_BindingError){_LL19: _LL1A:
# 321
 return;}else{_LL1B: _tmp87=_tmp86;_LL1C: {void*exn=_tmp87;(int)_rethrow(exn);}}_LL18:;}}}
# 324
void Cyc_Binding_resolve_and_add_vardecl(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp88=vd->name;struct _tuple0*_stmttmp6=_tmp88;struct _tuple0*_tmp89=_stmttmp6;struct _fat_ptr*_tmp8B;union Cyc_Absyn_Nmspace _tmp8A;_LL1: _tmp8A=_tmp89->f1;_tmp8B=_tmp89->f2;_LL2: {union Cyc_Absyn_Nmspace decl_ns=_tmp8A;struct _fat_ptr*decl_name=_tmp8B;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp328=({struct Cyc_Dict_Dict _tmp327=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp326=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp327,_tmp326,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D->tag=0U,({void*_tmp325=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=4U,_tmp8C->f1=vd;_tmp8C;});_tmp8D->f1=_tmp325;});_tmp8D;}));});
# 331
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp328;});else{
# 334
struct Cyc_Binding_ResolveNSEnv*_tmp8E=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp8E;
({struct Cyc_Dict_Dict _tmp32C=({struct Cyc_Dict_Dict _tmp32B=decl_ns_data->ordinaries;struct _fat_ptr*_tmp32A=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp32B,_tmp32A,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp90=_cycalloc(sizeof(*_tmp90));
# 337
_tmp90->tag=0U,({void*_tmp329=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=1U,_tmp8F->f1=vd;_tmp8F;});_tmp90->f1=_tmp329;});_tmp90;}));});
# 335
decl_ns_data->ordinaries=_tmp32C;});}}}}
# 341
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp91=s->r;void*_stmttmp7=_tmp91;void*_tmp92=_stmttmp7;struct Cyc_List_List*_tmp94;struct Cyc_Absyn_Stmt*_tmp93;struct Cyc_List_List*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Stmt*_tmp98;struct Cyc_Absyn_Decl*_tmp97;struct Cyc_Absyn_Stmt*_tmp99;struct Cyc_List_List*_tmp9A;struct Cyc_Absyn_Stmt*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Stmt*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Stmt*_tmpA1;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Stmt*_tmpA8;struct Cyc_Absyn_Stmt*_tmpA7;struct Cyc_Absyn_Exp*_tmpA9;switch(*((int*)_tmp92)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmpA9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmpA9;
Cyc_Binding_resolve_exp(env,e);return;}case 2U: _LL5: _tmpA7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmpA7;struct Cyc_Absyn_Stmt*s2=_tmpA8;
Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_stmt(env,s2);return;}case 3U: _LL7: _tmpA6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmpA6;
if((unsigned)eopt)Cyc_Binding_resolve_exp(env,eopt);return;}case 4U: _LL9: _tmpA3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_tmpA5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmpA3;struct Cyc_Absyn_Stmt*s1=_tmpA4;struct Cyc_Absyn_Stmt*s2=_tmpA5;
# 348
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_stmt(env,s2);return;}case 14U: _LLB: _tmpA1=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA2=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp92)->f2).f1;_LLC: {struct Cyc_Absyn_Stmt*s1=_tmpA1;struct Cyc_Absyn_Exp*e=_tmpA2;
_tmp9F=e;_tmpA0=s1;goto _LLE;}case 5U: _LLD: _tmp9F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp92)->f1).f1;_tmpA0=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp9F;struct Cyc_Absyn_Stmt*s1=_tmpA0;
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_stmt(env,s1);return;}case 9U: _LLF: _tmp9B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp9C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f2).f1;_tmp9D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f3).f1;_tmp9E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f4;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp9B;struct Cyc_Absyn_Exp*e2=_tmp9C;struct Cyc_Absyn_Exp*e3=_tmp9D;struct Cyc_Absyn_Stmt*s1=_tmp9E;
# 352
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);Cyc_Binding_resolve_exp(env,e3);
Cyc_Binding_resolve_stmt(env,s1);
return;}case 6U: _LL11: _LL12:
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp9A=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL18: {struct Cyc_List_List*es=_tmp9A;
# 359
for(0;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 13U: _LL19: _tmp99=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1A: {struct Cyc_Absyn_Stmt*s1=_tmp99;
Cyc_Binding_resolve_stmt(env,s1);return;}case 12U: _LL1B: _tmp97=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp98=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp97;struct Cyc_Absyn_Stmt*s1=_tmp98;
# 364
struct Cyc_Dict_Dict _tmpAA=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmpAA;
Cyc_Binding_resolve_decl(env,d);
Cyc_Binding_resolve_stmt(env,s1);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;
return;}case 10U: _LL1D: _tmp95=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp96=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp95;struct Cyc_List_List*scs=_tmp96;
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_scs(env,scs);return;}default: _LL1F: _tmp93=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp94=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp93;struct Cyc_List_List*scs=_tmp94;
Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_scs(env,scs);return;}}_LL0:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 373
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpAB=e->r;void*_stmttmp8=_tmpAB;void*_tmpAC=_stmttmp8;struct Cyc_List_List*_tmpAE;void*_tmpAD;struct Cyc_Absyn_Datatypefield*_tmpB1;struct Cyc_Absyn_Datatypedecl*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Enumfield*_tmpB3;struct Cyc_Absyn_Enumdecl*_tmpB2;struct Cyc_Absyn_Enumfield*_tmpB5;void*_tmpB4;struct Cyc_List_List*_tmpB6;struct Cyc_List_List*_tmpB8;void*_tmpB7;void*_tmpB9;void*_tmpBA;void*_tmpBB;struct Cyc_Absyn_Stmt*_tmpBC;struct Cyc_Absyn_Exp*_tmpBE;void*_tmpBD;struct Cyc_List_List*_tmpC0;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC8;void**_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_List_List*_tmpDF;struct Cyc_List_List*_tmpE0;void*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_List_List*_tmpE3;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Vardecl*_tmpE4;struct Cyc_Absyn_Aggrdecl**_tmpEA;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*_tmpE8;struct _tuple0**_tmpE7;int*_tmpED;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;void**_tmpEE;switch(*((int*)_tmpAC)){case 1U: _LL1: _tmpEE=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL2: {void**b=_tmpEE;
# 376
void*_tmpEF=*b;void*_stmttmp9=_tmpEF;void*_tmpF0=_stmttmp9;struct _tuple0*_tmpF1;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF0)->tag == 0U){_LL56: _tmpF1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF0)->f1;_LL57: {struct _tuple0*q=_tmpF1;
# 378
struct _handler_cons _tmpF2;_push_handler(& _tmpF2);{int _tmpF4=0;if(setjmp(_tmpF2.handler))_tmpF4=1;if(!_tmpF4){{void*_tmpF5=Cyc_Binding_lookup_ordinary(e->loc,env,q);void*_stmttmpA=_tmpF5;void*_tmpF6=_stmttmpA;struct Cyc_Absyn_Aggrdecl*_tmpF7;struct Cyc_Absyn_Enumfield*_tmpF9;void*_tmpF8;struct Cyc_Absyn_Enumfield*_tmpFB;struct Cyc_Absyn_Enumdecl*_tmpFA;struct Cyc_Absyn_Datatypefield*_tmpFD;struct Cyc_Absyn_Datatypedecl*_tmpFC;void*_tmpFE;switch(*((int*)_tmpF6)){case 0U: _LL5B: _tmpFE=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpF6)->f1;_LL5C: {void*bnd=_tmpFE;
*b=bnd;_npop_handler(0U);return;}case 2U: _LL5D: _tmpFC=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpF6)->f1;_tmpFD=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpF6)->f2;_LL5E: {struct Cyc_Absyn_Datatypedecl*tud=_tmpFC;struct Cyc_Absyn_Datatypefield*tuf=_tmpFD;
({void*_tmp32D=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->tag=31U,_tmpFF->f1=0,_tmpFF->f2=tud,_tmpFF->f3=tuf;_tmpFF;});e->r=_tmp32D;});_npop_handler(0U);return;}case 3U: _LL5F: _tmpFA=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpF6)->f1;_tmpFB=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpF6)->f2;_LL60: {struct Cyc_Absyn_Enumdecl*ed=_tmpFA;struct Cyc_Absyn_Enumfield*ef=_tmpFB;
({void*_tmp32E=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->tag=32U,_tmp100->f1=ed,_tmp100->f2=ef;_tmp100;});e->r=_tmp32E;});_npop_handler(0U);return;}case 4U: _LL61: _tmpF8=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpF6)->f1;_tmpF9=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpF6)->f2;_LL62: {void*t=_tmpF8;struct Cyc_Absyn_Enumfield*ef=_tmpF9;
({void*_tmp32F=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=33U,_tmp101->f1=t,_tmp101->f2=ef;_tmp101;});e->r=_tmp32F;});_npop_handler(0U);return;}default: _LL63: _tmpF7=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpF6)->f1;_LL64: {struct Cyc_Absyn_Aggrdecl*ad=_tmpF7;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp103=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA;_tmp2CA.tag=0U,({struct _fat_ptr _tmp330=({const char*_tmp105="bad occurrence of type name ";_tag_fat(_tmp105,sizeof(char),29U);});_tmp2CA.f1=_tmp330;});_tmp2CA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp104=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2C9;_tmp2C9.tag=3U,_tmp2C9.f1=q;_tmp2C9;});void*_tmp102[2U];_tmp102[0]=& _tmp103,_tmp102[1]=& _tmp104;({unsigned _tmp331=e->loc;Cyc_Warn_err2(_tmp331,_tag_fat(_tmp102,sizeof(void*),2U));});});_npop_handler(0U);return;}}_LL5A:;}
# 378
;_pop_handler();}else{void*_tmpF3=(void*)Cyc_Core_get_exn_thrown();void*_tmp106=_tmpF3;void*_tmp107;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp106)->tag == Cyc_Binding_BindingError){_LL66: _LL67:
# 386
({struct Cyc_Warn_String_Warn_Warg_struct _tmp109=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC;_tmp2CC.tag=0U,({struct _fat_ptr _tmp332=({const char*_tmp10B="undeclared identifier ";_tag_fat(_tmp10B,sizeof(char),23U);});_tmp2CC.f1=_tmp332;});_tmp2CC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2CB;_tmp2CB.tag=3U,_tmp2CB.f1=q;_tmp2CB;});void*_tmp108[2U];_tmp108[0]=& _tmp109,_tmp108[1]=& _tmp10A;({unsigned _tmp333=e->loc;Cyc_Warn_err2(_tmp333,_tag_fat(_tmp108,sizeof(void*),2U));});});return;}else{_LL68: _tmp107=_tmp106;_LL69: {void*exn=_tmp107;(int)_rethrow(exn);}}_LL65:;}}}}else{_LL58: _LL59:
# 388
 return;}_LL55:;}case 10U: _LL3: _tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpEC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpED=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmpEB;struct Cyc_List_List*es=_tmpEC;int*b=_tmpED;
# 391
*b=1;
# 393
{struct Cyc_List_List*_tmp10C=es;struct Cyc_List_List*es2=_tmp10C;for(0;es2 != 0;es2=es2->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es2->hd);}}{
void*_tmp10D=e1->r;void*_stmttmpB=_tmp10D;void*_tmp10E=_stmttmpB;void**_tmp10F;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10E)->tag == 1U){_LL6B: _tmp10F=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL6C: {void**b=_tmp10F;
# 397
void*_tmp110=*b;void*_stmttmpC=_tmp110;void*_tmp111=_stmttmpC;struct _tuple0*_tmp112;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp111)->tag == 0U){_LL70: _tmp112=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp111)->f1;_LL71: {struct _tuple0*q=_tmp112;
# 399
struct _handler_cons _tmp113;_push_handler(& _tmp113);{int _tmp115=0;if(setjmp(_tmp113.handler))_tmp115=1;if(!_tmp115){{void*_tmp116=Cyc_Binding_lookup_ordinary(e1->loc,env,q);void*_stmttmpD=_tmp116;void*_tmp117=_stmttmpD;struct Cyc_Absyn_Datatypefield*_tmp119;struct Cyc_Absyn_Datatypedecl*_tmp118;struct Cyc_Absyn_Aggrdecl*_tmp11A;void*_tmp11B;switch(*((int*)_tmp117)){case 0U: _LL75: _tmp11B=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp117)->f1;_LL76: {void*bnd=_tmp11B;
# 401
*b=bnd;_npop_handler(0U);return;}case 1U: _LL77: _tmp11A=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp117)->f1;_LL78: {struct Cyc_Absyn_Aggrdecl*ad=_tmp11A;
# 403
struct Cyc_List_List*_tmp11C=0;struct Cyc_List_List*dles=_tmp11C;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*_tmp11E));({struct _tuple10*_tmp334=({struct _tuple10*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->f1=0,_tmp11D->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp11D;});_tmp11E->hd=_tmp334;}),_tmp11E->tl=dles;_tmp11E;});}
({void*_tmp336=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=29U,_tmp11F->f1=ad->name,_tmp11F->f2=0,({struct Cyc_List_List*_tmp335=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp11F->f3=_tmp335;}),_tmp11F->f4=ad;_tmp11F;});e->r=_tmp336;});
_npop_handler(0U);return;}case 2U: _LL79: _tmp118=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp117)->f1;_tmp119=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp117)->f2;_LL7A: {struct Cyc_Absyn_Datatypedecl*tud=_tmp118;struct Cyc_Absyn_Datatypefield*tuf=_tmp119;
# 409
if(tuf->typs == 0)
# 411
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp121=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2CE;_tmp2CE.tag=3U,_tmp2CE.f1=tuf->name;_tmp2CE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp122=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD;_tmp2CD.tag=0U,({struct _fat_ptr _tmp337=({const char*_tmp123=" is a constant, not a function";_tag_fat(_tmp123,sizeof(char),31U);});_tmp2CD.f1=_tmp337;});_tmp2CD;});void*_tmp120[2U];_tmp120[0]=& _tmp121,_tmp120[1]=& _tmp122;({unsigned _tmp338=e->loc;Cyc_Warn_err2(_tmp338,_tag_fat(_tmp120,sizeof(void*),2U));});});
({void*_tmp339=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=31U,_tmp124->f1=es,_tmp124->f2=tud,_tmp124->f3=tuf;_tmp124;});e->r=_tmp339;});
_npop_handler(0U);return;}case 4U: _LL7B: _LL7C:
 goto _LL7E;default: _LL7D: _LL7E:
# 416
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp126=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D0;_tmp2D0.tag=3U,_tmp2D0.f1=q;_tmp2D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp127=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF;_tmp2CF.tag=0U,({struct _fat_ptr _tmp33A=({const char*_tmp128=" is an enum constructor, not a function";_tag_fat(_tmp128,sizeof(char),40U);});_tmp2CF.f1=_tmp33A;});_tmp2CF;});void*_tmp125[2U];_tmp125[0]=& _tmp126,_tmp125[1]=& _tmp127;({unsigned _tmp33B=e->loc;Cyc_Warn_err2(_tmp33B,_tag_fat(_tmp125,sizeof(void*),2U));});});_npop_handler(0U);return;}_LL74:;}
# 399
;_pop_handler();}else{void*_tmp114=(void*)Cyc_Core_get_exn_thrown();void*_tmp129=_tmp114;void*_tmp12A;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp129)->tag == Cyc_Binding_BindingError){_LL80: _LL81:
# 418
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D2;_tmp2D2.tag=0U,({struct _fat_ptr _tmp33C=({const char*_tmp12E="undeclared identifier ";_tag_fat(_tmp12E,sizeof(char),23U);});_tmp2D2.f1=_tmp33C;});_tmp2D2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp12D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D1;_tmp2D1.tag=3U,_tmp2D1.f1=q;_tmp2D1;});void*_tmp12B[2U];_tmp12B[0]=& _tmp12C,_tmp12B[1]=& _tmp12D;({unsigned _tmp33D=e->loc;Cyc_Warn_err2(_tmp33D,_tag_fat(_tmp12B,sizeof(void*),2U));});});return;}else{_LL82: _tmp12A=_tmp129;_LL83: {void*exn=_tmp12A;(int)_rethrow(exn);}}_LL7F:;}}}}else{_LL72: _LL73:
# 420
 return;}_LL6F:;}}else{_LL6D: _LL6E:
# 422
 Cyc_Binding_resolve_exp(env,e1);return;}_LL6A:;}}case 29U: _LL5: _tmpE7=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpE8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpE9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpEA=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL6: {struct _tuple0**tn=_tmpE7;struct Cyc_List_List*ts=_tmpE8;struct Cyc_List_List*dles=_tmpE9;struct Cyc_Absyn_Aggrdecl**adopt=_tmpEA;
# 426
for(0;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)dles->hd)).f2);}{
struct _handler_cons _tmp12F;_push_handler(& _tmp12F);{int _tmp131=0;if(setjmp(_tmp12F.handler))_tmp131=1;if(!_tmp131){
({struct Cyc_Absyn_Aggrdecl*_tmp33E=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*tn,Cyc_Binding_lookup_aggrdecl);*adopt=_tmp33E;});
*tn=((struct Cyc_Absyn_Aggrdecl*)_check_null(*adopt))->name;
_npop_handler(0U);return;
# 429
;_pop_handler();}else{void*_tmp130=(void*)Cyc_Core_get_exn_thrown();void*_tmp132=_tmp130;void*_tmp133;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp132)->tag == Cyc_Binding_BindingError){_LL85: _LL86:
# 433
({struct Cyc_Warn_String_Warn_Warg_struct _tmp135=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4;_tmp2D4.tag=0U,({struct _fat_ptr _tmp33F=({const char*_tmp137="unbound struct/union name ";_tag_fat(_tmp137,sizeof(char),27U);});_tmp2D4.f1=_tmp33F;});_tmp2D4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp136=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D3;_tmp2D3.tag=3U,_tmp2D3.f1=*tn;_tmp2D3;});void*_tmp134[2U];_tmp134[0]=& _tmp135,_tmp134[1]=& _tmp136;({unsigned _tmp340=e->loc;Cyc_Warn_err2(_tmp340,_tag_fat(_tmp134,sizeof(void*),2U));});});
return;}else{_LL87: _tmp133=_tmp132;_LL88: {void*exn=_tmp133;(int)_rethrow(exn);}}_LL84:;}}}}case 27U: _LL7: _tmpE4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpE5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpE6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpE4;struct Cyc_Absyn_Exp*e1=_tmpE5;struct Cyc_Absyn_Exp*e2=_tmpE6;
# 438
Cyc_Binding_resolve_exp(env,e1);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp342=({struct Cyc_Dict_Dict*_tmp138=_cycalloc(sizeof(*_tmp138));({struct Cyc_Dict_Dict _tmp341=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp138=_tmp341;});_tmp138;});env->local_vars=_tmp342;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
env->local_vars=0;
return;}{
# 446
struct Cyc_Dict_Dict _tmp139=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmp139;
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;
return;}}case 36U: _LL9: _tmpE3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLA: {struct Cyc_List_List*dles=_tmpE3;
# 454
for(0;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)dles->hd)).f2);}
return;}case 28U: _LLB: _tmpE1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpE2=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmpE1;void*t=_tmpE2;
# 460
Cyc_Binding_resolve_exp(env,e1);
Cyc_Binding_resolve_type(e->loc,env,t);
return;}case 2U: _LLD: _LLE:
# 464
 goto _LL10;case 0U: _LLF: _LL10:
 return;case 24U: _LL11: _tmpE0=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL12: {struct Cyc_List_List*es=_tmpE0;
# 467
_tmpDF=es;goto _LL14;}case 3U: _LL13: _tmpDF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL14: {struct Cyc_List_List*es=_tmpDF;
# 469
for(0;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 41U: _LL15: _tmpDE=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpDE;
# 473
_tmpDD=e1;goto _LL18;}case 38U: _LL17: _tmpDD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpDD;
_tmpDC=e1;goto _LL1A;}case 12U: _LL19: _tmpDC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpDC;
_tmpDB=e1;goto _LL1C;}case 18U: _LL1B: _tmpDB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpDB;
_tmpDA=e1;goto _LL1E;}case 11U: _LL1D: _tmpDA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpDA;
_tmpD9=e1;goto _LL20;}case 5U: _LL1F: _tmpD9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmpD9;
_tmpD8=e1;goto _LL22;}case 22U: _LL21: _tmpD8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmpD8;
_tmpD7=e1;goto _LL24;}case 21U: _LL23: _tmpD7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpD7;
_tmpD6=e1;goto _LL26;}case 15U: _LL25: _tmpD6=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmpD6;
_tmpD5=e1;goto _LL28;}case 20U: _LL27: _tmpD5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpD5;
Cyc_Binding_resolve_exp(env,e1);return;}case 35U: _LL29: _tmpD3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpD3;struct Cyc_Absyn_Exp*e2=_tmpD4;
# 484
_tmpD1=e1;_tmpD2=e2;goto _LL2C;}case 9U: _LL2B: _tmpD1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpD1;struct Cyc_Absyn_Exp*e2=_tmpD2;
_tmpCF=e1;_tmpD0=e2;goto _LL2E;}case 4U: _LL2D: _tmpCF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpCF;struct Cyc_Absyn_Exp*e2=_tmpD0;
_tmpCD=e1;_tmpCE=e2;goto _LL30;}case 23U: _LL2F: _tmpCD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmpCD;struct Cyc_Absyn_Exp*e2=_tmpCE;
_tmpCB=e1;_tmpCC=e2;goto _LL32;}case 7U: _LL31: _tmpCB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpCB;struct Cyc_Absyn_Exp*e2=_tmpCC;
_tmpC9=e1;_tmpCA=e2;goto _LL34;}case 8U: _LL33: _tmpC9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpC9;struct Cyc_Absyn_Exp*e2=_tmpCA;
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);return;}case 34U: _LL35: _tmpC6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).rgn;_tmpC7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).elt_type;_tmpC8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).num_elts;_LL36: {struct Cyc_Absyn_Exp*eo=_tmpC6;void**to=_tmpC7;struct Cyc_Absyn_Exp*e1=_tmpC8;
# 492
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
if(to != 0)Cyc_Binding_resolve_type(e->loc,env,*to);
Cyc_Binding_resolve_exp(env,e1);
return;}case 16U: _LL37: _tmpC4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL38: {struct Cyc_Absyn_Exp*eo=_tmpC4;struct Cyc_Absyn_Exp*e2=_tmpC5;
# 498
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
Cyc_Binding_resolve_exp(env,e2);
return;}case 6U: _LL39: _tmpC1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpC3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmpC1;struct Cyc_Absyn_Exp*e2=_tmpC2;struct Cyc_Absyn_Exp*e3=_tmpC3;
# 503
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);Cyc_Binding_resolve_exp(env,e3);return;}case 13U: _LL3B: _tmpBF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC0=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmpBF;struct Cyc_List_List*ts=_tmpC0;
# 506
Cyc_Binding_resolve_exp(env,e1);
for(0;ts != 0;ts=ts->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)ts->hd);}
return;}case 14U: _LL3D: _tmpBD=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpBE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL3E: {void*t=_tmpBD;struct Cyc_Absyn_Exp*e1=_tmpBE;
# 512
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_type(e->loc,env,t);return;}case 37U: _LL3F: _tmpBC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL40: {struct Cyc_Absyn_Stmt*s=_tmpBC;
# 514
Cyc_Binding_resolve_stmt(env,s);return;}case 39U: _LL41: _tmpBB=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL42: {void*t=_tmpBB;
# 516
_tmpBA=t;goto _LL44;}case 19U: _LL43: _tmpBA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL44: {void*t=_tmpBA;
_tmpB9=t;goto _LL46;}case 17U: _LL45: _tmpB9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL46: {void*t=_tmpB9;
Cyc_Binding_resolve_type(e->loc,env,t);return;}case 25U: _LL47: _tmpB7=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1)->f3;_tmpB8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL48: {void*t=_tmpB7;struct Cyc_List_List*dles=_tmpB8;
# 521
Cyc_Binding_resolve_type(e->loc,env,t);
_tmpB6=dles;goto _LL4A;}case 26U: _LL49: _tmpB6=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL4A: {struct Cyc_List_List*dles=_tmpB6;
# 525
for(0;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)dles->hd)).f2);}
return;}case 33U: _LL4B: _tmpB4=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB5=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4C: {void*t=_tmpB4;struct Cyc_Absyn_Enumfield*ef=_tmpB5;
# 530
Cyc_Binding_resolve_type(e->loc,env,t);return;}case 32U: _LL4D: _tmpB2=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB3=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4E: {struct Cyc_Absyn_Enumdecl*ed=_tmpB2;struct Cyc_Absyn_Enumfield*ef=_tmpB3;
return;}case 31U: _LL4F: _tmpAF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpB1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL50: {struct Cyc_List_List*es=_tmpAF;struct Cyc_Absyn_Datatypedecl*tud=_tmpB0;struct Cyc_Absyn_Datatypefield*tuf=_tmpB1;
# 533
for(0;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 30U: _LL51: _tmpAD=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpAE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL52: {void*t=_tmpAD;struct Cyc_List_List*dles=_tmpAE;
# 537
Cyc_Binding_resolve_type(e->loc,env,t);
for(0;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)dles->hd)).f2);}
return;}default: _LL53: _LL54:
# 543
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5;_tmp2D5.tag=0U,({struct _fat_ptr _tmp343=({const char*_tmp13C="asm expressions cannot occur within Cyclone code";_tag_fat(_tmp13C,sizeof(char),49U);});_tmp2D5.f1=_tmp343;});_tmp2D5;});void*_tmp13A[1U];_tmp13A[0]=& _tmp13B;({unsigned _tmp344=e->loc;Cyc_Warn_err2(_tmp344,_tag_fat(_tmp13A,sizeof(void*),1U));});});}_LL0:;}
# 547
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp13D=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmp13D;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp13E=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpE=_tmp13E;struct Cyc_Absyn_Switch_clause*_tmp13F=_stmttmpE;struct Cyc_Absyn_Stmt*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Pat*_tmp140;_LL1: _tmp140=_tmp13F->pattern;_tmp141=_tmp13F->where_clause;_tmp142=_tmp13F->body;_LL2: {struct Cyc_Absyn_Pat*pattern=_tmp140;struct Cyc_Absyn_Exp*where_clause=_tmp141;struct Cyc_Absyn_Stmt*body=_tmp142;
Cyc_Binding_resolve_pat(env,pattern);
if(where_clause != 0)
Cyc_Binding_resolve_exp(env,where_clause);
Cyc_Binding_resolve_stmt(env,body);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;}}
# 557
return;}
# 559
void Cyc_Binding_resolve_aggrfields(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp143=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmpF=_tmp143;struct Cyc_Absyn_Aggrfield*_tmp144=_stmttmpF;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_List_List*_tmp149;struct Cyc_Absyn_Exp*_tmp148;void*_tmp147;struct Cyc_Absyn_Tqual _tmp146;struct _fat_ptr*_tmp145;_LL1: _tmp145=_tmp144->name;_tmp146=_tmp144->tq;_tmp147=_tmp144->type;_tmp148=_tmp144->width;_tmp149=_tmp144->attributes;_tmp14A=_tmp144->requires_clause;_LL2: {struct _fat_ptr*fn=_tmp145;struct Cyc_Absyn_Tqual tq=_tmp146;void*t=_tmp147;struct Cyc_Absyn_Exp*width=_tmp148;struct Cyc_List_List*atts=_tmp149;struct Cyc_Absyn_Exp*requires_clause=_tmp14A;
Cyc_Binding_resolve_type(loc,env,t);
if(width != 0)
Cyc_Binding_resolve_exp(env,width);
if(requires_clause != 0)
Cyc_Binding_resolve_exp(env,requires_clause);}}
# 568
return;}
# 571
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int need_va_name,unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 577
struct Cyc_List_List*_tmp14B=0;struct Cyc_List_List*vds=_tmp14B;
for(0;args != 0;args=args->tl){
struct _tuple8*_tmp14C=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp10=_tmp14C;struct _tuple8*_tmp14D=_stmttmp10;void*_tmp150;struct Cyc_Absyn_Tqual _tmp14F;struct _fat_ptr*_tmp14E;_LL1: _tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;_tmp150=_tmp14D->f3;_LL2: {struct _fat_ptr*a=_tmp14E;struct Cyc_Absyn_Tqual tq=_tmp14F;void*t=_tmp150;
if(a == 0)
continue;{
struct Cyc_Absyn_Vardecl*_tmp151=({struct _tuple0*_tmp345=({struct _tuple0*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->f1=Cyc_Absyn_Loc_n,_tmp153->f2=a;_tmp153;});Cyc_Absyn_new_vardecl(0U,_tmp345,t,0);});struct Cyc_Absyn_Vardecl*vd=_tmp151;
vd->tq=tq;
vds=({struct Cyc_List_List*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->hd=vd,_tmp152->tl=vds;_tmp152;});}}}
# 586
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp154=*vai;struct Cyc_Absyn_VarargInfo _stmttmp11=_tmp154;struct Cyc_Absyn_VarargInfo _tmp155=_stmttmp11;int _tmp159;void*_tmp158;struct Cyc_Absyn_Tqual _tmp157;struct _fat_ptr*_tmp156;_LL4: _tmp156=_tmp155.name;_tmp157=_tmp155.tq;_tmp158=_tmp155.type;_tmp159=_tmp155.inject;_LL5: {struct _fat_ptr*v=_tmp156;struct Cyc_Absyn_Tqual tq=_tmp157;void*t=_tmp158;int i=_tmp159;
if(v == 0){
if(need_va_name)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6;_tmp2D6.tag=0U,({struct _fat_ptr _tmp346=({const char*_tmp15C="missing name for vararg";_tag_fat(_tmp15C,sizeof(char),24U);});_tmp2D6.f1=_tmp346;});_tmp2D6;});void*_tmp15A[1U];_tmp15A[0]=& _tmp15B;({unsigned _tmp347=loc;Cyc_Warn_err2(_tmp347,_tag_fat(_tmp15A,sizeof(void*),1U));});});}else{
# 593
void*_tmp15D=Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);void*typ=_tmp15D;
struct Cyc_Absyn_Vardecl*_tmp15E=({struct _tuple0*_tmp348=({struct _tuple0*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->f1=Cyc_Absyn_Loc_n,_tmp160->f2=v;_tmp160;});Cyc_Absyn_new_vardecl(0U,_tmp348,typ,0);});struct Cyc_Absyn_Vardecl*vd=_tmp15E;
vds=({struct Cyc_List_List*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->hd=vd,_tmp15F->tl=vds;_tmp15F;});}}}
# 598
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
return vds;}
# 602
void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo i){
if(i.effect != 0)
Cyc_Binding_resolve_type(loc,env,i.effect);
Cyc_Binding_resolve_type(loc,env,i.ret_type);
{struct Cyc_List_List*_tmp161=i.args;struct Cyc_List_List*args=_tmp161;for(0;args != 0;args=args->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple8*)args->hd)).f3);}}
if(i.cyc_varargs != 0)
Cyc_Binding_resolve_type(loc,env,(i.cyc_varargs)->type);
Cyc_Binding_resolve_rgnpo(loc,env,i.rgn_po);
if(i.requires_clause != 0)
Cyc_Binding_resolve_exp(env,i.requires_clause);
if(i.ensures_clause != 0){
# 615
struct Cyc_Dict_Dict _tmp162=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict locs=_tmp162;
struct _fat_ptr*_tmp163=({struct _fat_ptr*_tmp169=_cycalloc(sizeof(*_tmp169));({struct _fat_ptr _tmp349=({const char*_tmp168="return_value";_tag_fat(_tmp168,sizeof(char),13U);});*_tmp169=_tmp349;});_tmp169;});struct _fat_ptr*v=_tmp163;
struct Cyc_Absyn_Vardecl*_tmp164=({struct _tuple0*_tmp34A=({struct _tuple0*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->f1=Cyc_Absyn_Loc_n,_tmp167->f2=v;_tmp167;});Cyc_Absyn_new_vardecl(0U,_tmp34A,i.ret_type,0);});struct Cyc_Absyn_Vardecl*vd=_tmp164;
({struct Cyc_Dict_Dict _tmp34E=({struct Cyc_Dict_Dict _tmp34D=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp34C=v;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp34D,_tmp34C,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp166=_cycalloc(sizeof(*_tmp166));
_tmp166->tag=0U,({void*_tmp34B=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=4U,_tmp165->f1=vd;_tmp165;});_tmp166->f1=_tmp34B;});_tmp166;}));});
# 618
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp34E;});
# 620
Cyc_Binding_resolve_exp(env,i.ensures_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=locs;}}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 630
void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp16A=t;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_FnInfo _tmp16D;struct Cyc_Absyn_Exp*_tmp16F;void*_tmp16E;struct Cyc_List_List*_tmp170;void*_tmp173;void*_tmp172;void*_tmp171;void*_tmp174;struct Cyc_List_List*_tmp175;void***_tmp177;struct Cyc_Absyn_TypeDecl*_tmp176;struct Cyc_Absyn_Typedefdecl**_tmp17A;struct Cyc_List_List*_tmp179;struct _tuple0**_tmp178;struct Cyc_List_List*_tmp17C;void*_tmp17B;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Enumdecl*_tmp17F;struct _tuple0**_tmp17E;struct Cyc_List_List*_tmp181;union Cyc_Absyn_AggrInfo*_tmp180;struct Cyc_List_List*_tmp183;union Cyc_Absyn_DatatypeFieldInfo*_tmp182;struct Cyc_List_List*_tmp185;union Cyc_Absyn_DatatypeInfo*_tmp184;switch(*((int*)_tmp16A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)){case 18U: _LL1: _tmp184=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_tmp185=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2;_LL2: {union Cyc_Absyn_DatatypeInfo*info=_tmp184;struct Cyc_List_List*targs=_tmp185;
# 633
for(0;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeInfo _tmp186=*info;union Cyc_Absyn_DatatypeInfo _stmttmp12=_tmp186;union Cyc_Absyn_DatatypeInfo _tmp187=_stmttmp12;int _tmp189;struct _tuple0*_tmp188;if((_tmp187.UnknownDatatype).tag == 1){_LL26: _tmp188=((_tmp187.UnknownDatatype).val).name;_tmp189=((_tmp187.UnknownDatatype).val).is_extensible;_LL27: {struct _tuple0*qv=_tmp188;int b=_tmp189;
# 637
struct _handler_cons _tmp18A;_push_handler(& _tmp18A);{int _tmp18C=0;if(setjmp(_tmp18A.handler))_tmp18C=1;if(!_tmp18C){
{struct Cyc_Absyn_Datatypedecl*_tmp18D=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_datatypedecl))->hd;struct Cyc_Absyn_Datatypedecl*tud=_tmp18D;
({union Cyc_Absyn_DatatypeInfo _tmp34F=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp2D7;_tmp2D7.name=tud->name,_tmp2D7.is_extensible=b;_tmp2D7;}));*info=_tmp34F;});
_npop_handler(0U);return;}
# 638
;_pop_handler();}else{void*_tmp18B=(void*)Cyc_Core_get_exn_thrown();void*_tmp18E=_tmp18B;void*_tmp18F;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp18E)->tag == Cyc_Binding_BindingError){_LL2B: _LL2C:
# 642
 Cyc_Binding_absolutize_topdecl(loc,env,qv,Cyc_Absyn_Public);return;}else{_LL2D: _tmp18F=_tmp18E;_LL2E: {void*exn=_tmp18F;(int)_rethrow(exn);}}_LL2A:;}}}}else{_LL28: _LL29:
 return;}_LL25:;}}case 19U: _LL3: _tmp182=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_tmp183=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2;_LL4: {union Cyc_Absyn_DatatypeFieldInfo*info=_tmp182;struct Cyc_List_List*targs=_tmp183;
# 646
for(0;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeFieldInfo _tmp190=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmp13=_tmp190;union Cyc_Absyn_DatatypeFieldInfo _tmp191=_stmttmp13;int _tmp194;struct _tuple0*_tmp193;struct _tuple0*_tmp192;if((_tmp191.UnknownDatatypefield).tag == 1){_LL30: _tmp192=((_tmp191.UnknownDatatypefield).val).datatype_name;_tmp193=((_tmp191.UnknownDatatypefield).val).field_name;_tmp194=((_tmp191.UnknownDatatypefield).val).is_extensible;_LL31: {struct _tuple0*qvd=_tmp192;struct _tuple0*qvf=_tmp193;int b=_tmp194;
# 652
{struct _handler_cons _tmp195;_push_handler(& _tmp195);{int _tmp197=0;if(setjmp(_tmp195.handler))_tmp197=1;if(!_tmp197){
{void*_tmp198=Cyc_Binding_lookup_ordinary(loc,env,qvf);void*_stmttmp14=_tmp198;void*_tmp199=_stmttmp14;struct Cyc_Absyn_Datatypefield*_tmp19B;struct Cyc_Absyn_Datatypedecl*_tmp19A;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp199)->tag == 2U){_LL35: _tmp19A=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp199)->f1;_tmp19B=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp199)->f2;_LL36: {struct Cyc_Absyn_Datatypedecl*tud=_tmp19A;struct Cyc_Absyn_Datatypefield*tuf=_tmp19B;
# 655
struct Cyc_Absyn_Datatypedecl*_tmp19C=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qvd,Cyc_Binding_lookup_datatypedecl))->hd;struct Cyc_Absyn_Datatypedecl*tud2=_tmp19C;
if(Cyc_Absyn_qvar_cmp(tud->name,tud2->name)!= 0){
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2DD;_tmp2DD.tag=3U,_tmp2DD.f1=tuf->name;_tmp2DD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp19F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC;_tmp2DC.tag=0U,({struct _fat_ptr _tmp350=({const char*_tmp1A6=" is a variant of ";_tag_fat(_tmp1A6,sizeof(char),18U);});_tmp2DC.f1=_tmp350;});_tmp2DC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A0=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2DB;_tmp2DB.tag=3U,_tmp2DB.f1=tud2->name;_tmp2DB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA;_tmp2DA.tag=0U,({struct _fat_ptr _tmp351=({const char*_tmp1A5=" not ";_tag_fat(_tmp1A5,sizeof(char),6U);});_tmp2DA.f1=_tmp351;});_tmp2DA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D9;_tmp2D9.tag=3U,_tmp2D9.f1=tud->name;_tmp2D9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D8;_tmp2D8.tag=0U,({
struct _fat_ptr _tmp352=({const char*_tmp1A4=" (shadowing not yet implemented properly)";_tag_fat(_tmp1A4,sizeof(char),42U);});_tmp2D8.f1=_tmp352;});_tmp2D8;});void*_tmp19D[6U];_tmp19D[0]=& _tmp19E,_tmp19D[1]=& _tmp19F,_tmp19D[2]=& _tmp1A0,_tmp19D[3]=& _tmp1A1,_tmp19D[4]=& _tmp1A2,_tmp19D[5]=& _tmp1A3;({unsigned _tmp353=loc;Cyc_Warn_err2(_tmp353,_tag_fat(_tmp19D,sizeof(void*),6U));});});
_npop_handler(0U);return;}
# 661
({union Cyc_Absyn_DatatypeFieldInfo _tmp354=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp2DE;_tmp2DE.datatype_name=tud->name,_tmp2DE.field_name=tuf->name,_tmp2DE.is_extensible=b;_tmp2DE;}));*info=_tmp354;});
# 663
_npop_handler(0U);return;}}else{_LL37: _LL38:
 goto _LL34;}_LL34:;}
# 653
;_pop_handler();}else{void*_tmp196=(void*)Cyc_Core_get_exn_thrown();void*_tmp1A7=_tmp196;void*_tmp1A8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1A7)->tag == Cyc_Binding_BindingError){_LL3A: _LL3B:
# 666
 goto _LL39;}else{_LL3C: _tmp1A8=_tmp1A7;_LL3D: {void*exn=_tmp1A8;(int)_rethrow(exn);}}_LL39:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1AA=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E0;_tmp2E0.tag=3U,_tmp2E0.f1=qvf;_tmp2E0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF;_tmp2DF.tag=0U,({struct _fat_ptr _tmp355=({const char*_tmp1AC=" is not a datatype field";_tag_fat(_tmp1AC,sizeof(char),25U);});_tmp2DF.f1=_tmp355;});_tmp2DF;});void*_tmp1A9[2U];_tmp1A9[0]=& _tmp1AA,_tmp1A9[1]=& _tmp1AB;({unsigned _tmp356=loc;Cyc_Warn_err2(_tmp356,_tag_fat(_tmp1A9,sizeof(void*),2U));});});return;}}else{_LL32: _LL33:
 return;}_LL2F:;}}case 20U: _LL5: _tmp180=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_tmp181=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2;_LL6: {union Cyc_Absyn_AggrInfo*info=_tmp180;struct Cyc_List_List*targs=_tmp181;
# 672
for(0;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_AggrInfo _tmp1AD=*info;union Cyc_Absyn_AggrInfo _stmttmp15=_tmp1AD;union Cyc_Absyn_AggrInfo _tmp1AE=_stmttmp15;struct Cyc_Core_Opt*_tmp1B1;struct _tuple0*_tmp1B0;enum Cyc_Absyn_AggrKind _tmp1AF;if((_tmp1AE.UnknownAggr).tag == 1){_LL3F: _tmp1AF=((_tmp1AE.UnknownAggr).val).f1;_tmp1B0=((_tmp1AE.UnknownAggr).val).f2;_tmp1B1=((_tmp1AE.UnknownAggr).val).f3;_LL40: {enum Cyc_Absyn_AggrKind ak=_tmp1AF;struct _tuple0*qv=_tmp1B0;struct Cyc_Core_Opt*bo=_tmp1B1;
# 676
struct _handler_cons _tmp1B2;_push_handler(& _tmp1B2);{int _tmp1B4=0;if(setjmp(_tmp1B2.handler))_tmp1B4=1;if(!_tmp1B4){
{struct Cyc_Absyn_Aggrdecl*_tmp1B5=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_aggrdecl);struct Cyc_Absyn_Aggrdecl*ad=_tmp1B5;
if((int)ad->kind != (int)ak)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E1;_tmp2E1.tag=0U,({struct _fat_ptr _tmp357=({const char*_tmp1B8="struct vs. union mismatch with earlier declaration";_tag_fat(_tmp1B8,sizeof(char),51U);});_tmp2E1.f1=_tmp357;});_tmp2E1;});void*_tmp1B6[1U];_tmp1B6[0]=& _tmp1B7;({unsigned _tmp358=loc;Cyc_Warn_err2(_tmp358,_tag_fat(_tmp1B6,sizeof(void*),1U));});});
if((((int)ak == (int)1U && bo != 0)&& ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged != (int)bo->v)
# 682
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2;_tmp2E2.tag=0U,({struct _fat_ptr _tmp359=({const char*_tmp1BB="@tagged mismatch with earlier declaration";_tag_fat(_tmp1BB,sizeof(char),42U);});_tmp2E2.f1=_tmp359;});_tmp2E2;});void*_tmp1B9[1U];_tmp1B9[0]=& _tmp1BA;({unsigned _tmp35A=loc;Cyc_Warn_err2(_tmp35A,_tag_fat(_tmp1B9,sizeof(void*),1U));});});
({union Cyc_Absyn_AggrInfo _tmp35B=Cyc_Absyn_UnknownAggr(ak,ad->name,bo);*info=_tmp35B;});
_npop_handler(0U);return;}
# 677
;_pop_handler();}else{void*_tmp1B3=(void*)Cyc_Core_get_exn_thrown();void*_tmp1BC=_tmp1B3;void*_tmp1BD;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1BC)->tag == Cyc_Binding_BindingError){_LL44: _LL45:
# 686
 Cyc_Binding_absolutize_topdecl(loc,env,qv,Cyc_Absyn_Public);return;}else{_LL46: _tmp1BD=_tmp1BC;_LL47: {void*exn=_tmp1BD;(int)_rethrow(exn);}}_LL43:;}}}}else{_LL41: _LL42:
 return;}_LL3E:;}}case 15U: _LL7: _tmp17E=(struct _tuple0**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_tmp17F=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f2;_LL8: {struct _tuple0**qv=_tmp17E;struct Cyc_Absyn_Enumdecl*edo=_tmp17F;
# 690
if(edo != 0)
return;{
struct _handler_cons _tmp1BE;_push_handler(& _tmp1BE);{int _tmp1C0=0;if(setjmp(_tmp1BE.handler))_tmp1C0=1;if(!_tmp1C0){
{struct Cyc_Absyn_Enumdecl*_tmp1C1=((struct Cyc_Absyn_Enumdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*qv,Cyc_Binding_lookup_enumdecl);struct Cyc_Absyn_Enumdecl*ed=_tmp1C1;
*qv=ed->name;
_npop_handler(0U);return;}
# 693
;_pop_handler();}else{void*_tmp1BF=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C2=_tmp1BF;void*_tmp1C3;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1C2)->tag == Cyc_Binding_BindingError){_LL49: _LL4A:
# 697
 Cyc_Binding_absolutize_topdecl(loc,env,*qv,Cyc_Absyn_Public);return;}else{_LL4B: _tmp1C3=_tmp1C2;_LL4C: {void*exn=_tmp1C3;(int)_rethrow(exn);}}_LL48:;}}}}case 16U: _LLF: _tmp17D=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_LL10: {struct Cyc_List_List*fs=_tmp17D;
# 735
struct Cyc_Binding_ResolveNSEnv*_tmp1DA=({struct Cyc_Binding_NSCtxt*_tmp35D=env->ns;((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(_tmp35D,({struct Cyc_List_List*_tmp35C=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp35C,
Cyc_Binding_in_cinclude(env));}));});
# 735
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp1DA;
# 737
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1DB=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp1DB;
if(f->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
({struct Cyc_Dict_Dict _tmp360=({struct Cyc_Dict_Dict _tmp35F=decl_ns_data->ordinaries;struct _fat_ptr*_tmp35E=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp35F,_tmp35E,(void*)({struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->tag=4U,_tmp1DC->f1=t,_tmp1DC->f2=f;_tmp1DC;}));});decl_ns_data->ordinaries=_tmp360;});}
# 743
return;}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2 == 0){_LL15: _LL16:
# 747
 return;}else{_LL17: _tmp17B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1;_tmp17C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2;_LL18: {void*c=_tmp17B;struct Cyc_List_List*ts=_tmp17C;
# 749
for(0;ts != 0;ts=ts->tl){
Cyc_Binding_resolve_type(loc,env,(void*)ts->hd);}
return;}}}case 8U: _LL9: _tmp178=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16A)->f1;_tmp179=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16A)->f2;_tmp17A=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16A)->f3;_LLA: {struct _tuple0**tdn=_tmp178;struct Cyc_List_List*targs=_tmp179;struct Cyc_Absyn_Typedefdecl**tdo=_tmp17A;
# 700
for(0;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}
{struct _handler_cons _tmp1C4;_push_handler(& _tmp1C4);{int _tmp1C6=0;if(setjmp(_tmp1C4.handler))_tmp1C6=1;if(!_tmp1C6){
{struct Cyc_Absyn_Typedefdecl*_tmp1C7=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*tdn,Cyc_Binding_lookup_typedefdecl);struct Cyc_Absyn_Typedefdecl*td=_tmp1C7;
# 705
*tdn=td->name;
_npop_handler(0U);return;}
# 703
;_pop_handler();}else{void*_tmp1C5=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C8=_tmp1C5;void*_tmp1C9;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1C8)->tag == Cyc_Binding_BindingError){_LL4E: _LL4F:
# 707
 goto _LL4D;}else{_LL50: _tmp1C9=_tmp1C8;_LL51: {void*exn=_tmp1C9;(int)_rethrow(exn);}}_LL4D:;}}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4;_tmp2E4.tag=0U,({struct _fat_ptr _tmp361=({const char*_tmp1CD="unbound typdef name ";_tag_fat(_tmp1CD,sizeof(char),21U);});_tmp2E4.f1=_tmp361;});_tmp2E4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1CC=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E3;_tmp2E3.tag=3U,_tmp2E3.f1=*tdn;_tmp2E3;});void*_tmp1CA[2U];_tmp1CA[0]=& _tmp1CB,_tmp1CA[1]=& _tmp1CC;({unsigned _tmp362=loc;Cyc_Warn_err2(_tmp362,_tag_fat(_tmp1CA,sizeof(void*),2U));});});
return;}case 10U: _LLB: _tmp176=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16A)->f1;_tmp177=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16A)->f2;_LLC: {struct Cyc_Absyn_TypeDecl*td=_tmp176;void***to=_tmp177;
# 713
struct Cyc_Dict_Dict*_tmp1CE=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp1CE;
env->local_vars=0;
{void*_tmp1CF=td->r;void*_stmttmp16=_tmp1CF;void*_tmp1D0=_stmttmp16;struct Cyc_Absyn_Datatypedecl*_tmp1D1;struct Cyc_Absyn_Enumdecl*_tmp1D2;struct Cyc_Absyn_Aggrdecl*_tmp1D3;switch(*((int*)_tmp1D0)){case 0U: _LL53: _tmp1D3=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1D0)->f1;_LL54: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1D3;
# 717
({struct Cyc_Binding_Env*_tmp364=env;Cyc_Binding_resolve_decl(_tmp364,({struct Cyc_Absyn_Decl*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));({void*_tmp363=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->tag=5U,_tmp1D4->f1=ad;_tmp1D4;});_tmp1D5->r=_tmp363;}),_tmp1D5->loc=td->loc;_tmp1D5;}));});goto _LL52;}case 1U: _LL55: _tmp1D2=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1D0)->f1;_LL56: {struct Cyc_Absyn_Enumdecl*ed=_tmp1D2;
# 719
({struct Cyc_Binding_Env*_tmp366=env;Cyc_Binding_resolve_decl(_tmp366,({struct Cyc_Absyn_Decl*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));({void*_tmp365=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->tag=7U,_tmp1D6->f1=ed;_tmp1D6;});_tmp1D7->r=_tmp365;}),_tmp1D7->loc=td->loc;_tmp1D7;}));});goto _LL52;}default: _LL57: _tmp1D1=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1D0)->f1;_LL58: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1D1;
# 721
({struct Cyc_Binding_Env*_tmp368=env;Cyc_Binding_resolve_decl(_tmp368,({struct Cyc_Absyn_Decl*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));({void*_tmp367=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->tag=6U,_tmp1D8->f1=tud;_tmp1D8;});_tmp1D9->r=_tmp367;}),_tmp1D9->loc=td->loc;_tmp1D9;}));});goto _LL52;}}_LL52:;}
# 723
env->local_vars=old_locals;
return;}case 7U: _LLD: _tmp175=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp16A)->f2;_LLE: {struct Cyc_List_List*fs=_tmp175;
# 730
Cyc_Binding_resolve_aggrfields(loc,env,fs);
return;}case 1U: _LL11: _tmp174=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16A)->f2;_LL12: {void*to=_tmp174;
# 744
if(to != 0)Cyc_Binding_resolve_type(loc,env,to);return;}case 2U: _LL13: _LL14:
# 746
 goto _LL16;case 3U: _LL19: _tmp171=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).elt_type;_tmp172=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).rgn;_tmp173=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).bounds;_LL1A: {void*t1=_tmp171;void*t2=_tmp172;void*bds=_tmp173;
# 754
Cyc_Binding_resolve_type(loc,env,t1);
Cyc_Binding_resolve_type(loc,env,t2);
Cyc_Binding_resolve_type(loc,env,bds);
return;}case 6U: _LL1B: _tmp170=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp16A)->f1;_LL1C: {struct Cyc_List_List*tqts=_tmp170;
# 760
for(0;tqts != 0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)tqts->hd)).f2);}
return;}case 4U: _LL1D: _tmp16E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16A)->f1).elt_type;_tmp16F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16A)->f1).num_elts;_LL1E: {void*t1=_tmp16E;struct Cyc_Absyn_Exp*eo=_tmp16F;
# 764
Cyc_Binding_resolve_type(loc,env,t1);
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
return;}case 5U: _LL1F: _tmp16D=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16A)->f1;_LL20: {struct Cyc_Absyn_FnInfo i=_tmp16D;
# 771
struct Cyc_List_List*_tmp1DD=Cyc_Binding_get_fun_vardecls(0,loc,env,i.args,i.cyc_varargs);struct Cyc_List_List*vds=_tmp1DD;
# 773
struct Cyc_Dict_Dict*_tmp1DE=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp1DE;
if(old_locals != 0)
({struct Cyc_Dict_Dict*_tmp369=({struct Cyc_Dict_Dict*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));*_tmp1DF=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1DF;});env->local_vars=_tmp369;});else{
# 777
({struct Cyc_Dict_Dict*_tmp36B=({struct Cyc_Dict_Dict*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));({struct Cyc_Dict_Dict _tmp36A=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp1E0=_tmp36A;});_tmp1E0;});env->local_vars=_tmp36B;});}
{struct Cyc_List_List*_tmp1E1=vds;struct Cyc_List_List*vds1=_tmp1E1;for(0;vds1 != 0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _tmp36F=({struct Cyc_Dict_Dict _tmp36E=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp36D=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp36E,_tmp36D,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));
_tmp1E3->tag=0U,({void*_tmp36C=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2->tag=3U,_tmp1E2->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_tmp1E2;});_tmp1E3->f1=_tmp36C;});_tmp1E3;}));});
# 779
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp36F;});}}
# 782
Cyc_Binding_resolve_function_stuff(loc,env,i);
env->local_vars=old_locals;
return;}case 9U: _LL21: _tmp16C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp16A)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp16C;
# 786
_tmp16B=e;goto _LL24;}default: _LL23: _tmp16B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp16A)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp16B;
Cyc_Binding_resolve_exp(env,e);return;}}_LL0:;}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 791
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1E4=p;unsigned _tmp1E6;void*_tmp1E5;_LL1: _tmp1E5=_tmp1E4->r;_tmp1E6=_tmp1E4->loc;_LL2: {void*r=_tmp1E5;unsigned loc=_tmp1E6;
void*_tmp1E7=r;struct Cyc_List_List*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Pat*_tmp1EB;struct Cyc_Absyn_Vardecl*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1ED;struct Cyc_Absyn_Pat*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1EE;struct Cyc_Absyn_Pat*_tmp1F1;struct Cyc_Absyn_Vardecl*_tmp1F0;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F3;int _tmp1F7;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F5;struct _tuple0*_tmp1F4;int _tmp1FA;struct Cyc_List_List*_tmp1F9;struct _tuple0*_tmp1F8;struct _tuple0*_tmp1FB;switch(*((int*)_tmp1E7)){case 15U: _LL4: _tmp1FB=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1;_LL5: {struct _tuple0*qv=_tmp1FB;
# 795
{struct _handler_cons _tmp1FC;_push_handler(& _tmp1FC);{int _tmp1FE=0;if(setjmp(_tmp1FC.handler))_tmp1FE=1;if(!_tmp1FE){{void*_tmp1FF=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_stmttmp17=_tmp1FF;void*_tmp200=_stmttmp17;struct Cyc_Absyn_Enumfield*_tmp202;void*_tmp201;struct Cyc_Absyn_Enumfield*_tmp204;struct Cyc_Absyn_Enumdecl*_tmp203;struct Cyc_Absyn_Datatypefield*_tmp206;struct Cyc_Absyn_Datatypedecl*_tmp205;switch(*((int*)_tmp200)){case 0U: _LL2D: _LL2E:
 goto _LL2C;case 1U: _LL2F: _LL30:
# 798
({struct Cyc_Warn_String_Warn_Warg_struct _tmp208=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5;_tmp2E5.tag=0U,({struct _fat_ptr _tmp370=({const char*_tmp209="struct tag used without arguments in pattern";_tag_fat(_tmp209,sizeof(char),45U);});_tmp2E5.f1=_tmp370;});_tmp2E5;});void*_tmp207[1U];_tmp207[0]=& _tmp208;({unsigned _tmp371=loc;Cyc_Warn_err2(_tmp371,_tag_fat(_tmp207,sizeof(void*),1U));});});_npop_handler(0U);return;case 2U: _LL31: _tmp205=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp200)->f1;_tmp206=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp200)->f2;_LL32: {struct Cyc_Absyn_Datatypedecl*tud=_tmp205;struct Cyc_Absyn_Datatypefield*tuf=_tmp206;
# 800
({void*_tmp372=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->tag=8U,_tmp20A->f1=tud,_tmp20A->f2=tuf,_tmp20A->f3=0,_tmp20A->f4=0;_tmp20A;});p->r=_tmp372;});_npop_handler(0U);return;}case 3U: _LL33: _tmp203=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp200)->f1;_tmp204=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp200)->f2;_LL34: {struct Cyc_Absyn_Enumdecl*ed=_tmp203;struct Cyc_Absyn_Enumfield*ef=_tmp204;
# 802
({void*_tmp373=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B->tag=13U,_tmp20B->f1=ed,_tmp20B->f2=ef;_tmp20B;});p->r=_tmp373;});_npop_handler(0U);return;}default: _LL35: _tmp201=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp200)->f1;_tmp202=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp200)->f2;_LL36: {void*t=_tmp201;struct Cyc_Absyn_Enumfield*ef=_tmp202;
# 804
({void*_tmp374=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->tag=14U,_tmp20C->f1=t,_tmp20C->f2=ef;_tmp20C;});p->r=_tmp374;});_npop_handler(0U);return;}}_LL2C:;}
# 795
;_pop_handler();}else{void*_tmp1FD=(void*)Cyc_Core_get_exn_thrown();void*_tmp20D=_tmp1FD;void*_tmp20E;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp20D)->tag == Cyc_Binding_BindingError){_LL38: _LL39:
# 805
 goto _LL37;}else{_LL3A: _tmp20E=_tmp20D;_LL3B: {void*exn=_tmp20E;(int)_rethrow(exn);}}_LL37:;}}}{
struct Cyc_Absyn_Vardecl*_tmp20F=Cyc_Absyn_new_vardecl(0U,qv,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmp20F;
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
({void*_tmp376=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->tag=1U,_tmp210->f1=vd,({struct Cyc_Absyn_Pat*_tmp375=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp210->f2=_tmp375;});_tmp210;});p->r=_tmp376;});
return;}}case 16U: _LL6: _tmp1F8=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1;_tmp1F9=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E7)->f2;_tmp1FA=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E7)->f3;_LL7: {struct _tuple0*qv=_tmp1F8;struct Cyc_List_List*ps=_tmp1F9;int dots=_tmp1FA;
# 812
{struct Cyc_List_List*_tmp211=ps;struct Cyc_List_List*ps2=_tmp211;for(0;ps2 != 0;ps2=ps2->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps2->hd);}}
{struct _handler_cons _tmp212;_push_handler(& _tmp212);{int _tmp214=0;if(setjmp(_tmp212.handler))_tmp214=1;if(!_tmp214){{void*_tmp215=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_stmttmp18=_tmp215;void*_tmp216=_stmttmp18;struct Cyc_Absyn_Datatypefield*_tmp218;struct Cyc_Absyn_Datatypedecl*_tmp217;struct Cyc_Absyn_Aggrdecl*_tmp219;switch(*((int*)_tmp216)){case 0U: _LL3D: _LL3E:
 goto _LL3C;case 1U: _LL3F: _tmp219=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp216)->f1;_LL40: {struct Cyc_Absyn_Aggrdecl*ad=_tmp219;
# 817
struct Cyc_List_List*_tmp21A=0;struct Cyc_List_List*new_ps=_tmp21A;
for(0;ps != 0;ps=ps->tl){
new_ps=({struct Cyc_List_List*_tmp21C=_cycalloc(sizeof(*_tmp21C));({struct _tuple12*_tmp377=({struct _tuple12*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->f1=0,_tmp21B->f2=(struct Cyc_Absyn_Pat*)ps->hd;_tmp21B;});_tmp21C->hd=_tmp377;}),_tmp21C->tl=new_ps;_tmp21C;});}
({void*_tmp37B=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp37A=({union Cyc_Absyn_AggrInfo*_tmp21E=_cycalloc(sizeof(*_tmp21E));({union Cyc_Absyn_AggrInfo _tmp379=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp21D=_cycalloc(sizeof(*_tmp21D));*_tmp21D=ad;_tmp21D;}));*_tmp21E=_tmp379;});_tmp21E;});_tmp21F->f1=_tmp37A;}),_tmp21F->f2=0,({struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_ps);_tmp21F->f3=_tmp378;}),_tmp21F->f4=dots;_tmp21F;});p->r=_tmp37B;});
_npop_handler(0U);return;}case 2U: _LL41: _tmp217=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp216)->f1;_tmp218=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp216)->f2;_LL42: {struct Cyc_Absyn_Datatypedecl*tud=_tmp217;struct Cyc_Absyn_Datatypefield*tuf=_tmp218;
# 823
({void*_tmp37C=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220->tag=8U,_tmp220->f1=tud,_tmp220->f2=tuf,_tmp220->f3=ps,_tmp220->f4=dots;_tmp220;});p->r=_tmp37C;});_npop_handler(0U);return;}case 3U: _LL43: _LL44:
 goto _LL46;default: _LL45: _LL46:
# 826
({struct Cyc_Warn_String_Warn_Warg_struct _tmp222=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6;_tmp2E6.tag=0U,({struct _fat_ptr _tmp37D=({const char*_tmp223="enum tag used with arguments in pattern";_tag_fat(_tmp223,sizeof(char),40U);});_tmp2E6.f1=_tmp37D;});_tmp2E6;});void*_tmp221[1U];_tmp221[0]=& _tmp222;({unsigned _tmp37E=loc;Cyc_Warn_err2(_tmp37E,_tag_fat(_tmp221,sizeof(void*),1U));});});_npop_handler(0U);return;}_LL3C:;}
# 814
;_pop_handler();}else{void*_tmp213=(void*)Cyc_Core_get_exn_thrown();void*_tmp224=_tmp213;void*_tmp225;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp224)->tag == Cyc_Binding_BindingError){_LL48: _LL49:
# 827
 goto _LL47;}else{_LL4A: _tmp225=_tmp224;_LL4B: {void*exn=_tmp225;(int)_rethrow(exn);}}_LL47:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp227=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E8;_tmp2E8.tag=3U,_tmp2E8.f1=qv;_tmp2E8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp228=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7;_tmp2E7.tag=0U,({struct _fat_ptr _tmp37F=({const char*_tmp229=" is not a constructor in pattern";_tag_fat(_tmp229,sizeof(char),33U);});_tmp2E7.f1=_tmp37F;});_tmp2E7;});void*_tmp226[2U];_tmp226[0]=& _tmp227,_tmp226[1]=& _tmp228;({unsigned _tmp380=loc;Cyc_Warn_err2(_tmp380,_tag_fat(_tmp226,sizeof(void*),2U));});});return;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1)->UnknownAggr).tag == 1){_LL8: _tmp1F4=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1)->UnknownAggr).val).f2;_tmp1F5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E7)->f2;_tmp1F6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E7)->f3;_tmp1F7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E7)->f4;_LL9: {struct _tuple0*n=_tmp1F4;struct Cyc_List_List*exist_ts=_tmp1F5;struct Cyc_List_List*dps=_tmp1F6;int dots=_tmp1F7;
# 831
{struct Cyc_List_List*_tmp22A=dps;struct Cyc_List_List*dps2=_tmp22A;for(0;dps2 != 0;dps2=dps2->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple12*)dps2->hd)).f2);}}
{struct _handler_cons _tmp22B;_push_handler(& _tmp22B);{int _tmp22D=0;if(setjmp(_tmp22B.handler))_tmp22D=1;if(!_tmp22D){
{struct Cyc_Absyn_Aggrdecl*_tmp22E=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,n,Cyc_Binding_lookup_aggrdecl);struct Cyc_Absyn_Aggrdecl*ad=_tmp22E;
({void*_tmp383=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp382=({union Cyc_Absyn_AggrInfo*_tmp230=_cycalloc(sizeof(*_tmp230));({union Cyc_Absyn_AggrInfo _tmp381=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp22F=_cycalloc(sizeof(*_tmp22F));*_tmp22F=ad;_tmp22F;}));*_tmp230=_tmp381;});_tmp230;});_tmp231->f1=_tmp382;}),_tmp231->f2=exist_ts,_tmp231->f3=dps,_tmp231->f4=dots;_tmp231;});p->r=_tmp383;});}
# 834
;_pop_handler();}else{void*_tmp22C=(void*)Cyc_Core_get_exn_thrown();void*_tmp232=_tmp22C;void*_tmp233;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp232)->tag == Cyc_Binding_BindingError){_LL4D: _LL4E:
# 837
({struct Cyc_Warn_String_Warn_Warg_struct _tmp235=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9;_tmp2E9.tag=0U,({struct _fat_ptr _tmp384=({const char*_tmp236="non-aggregate name has designator patterns";_tag_fat(_tmp236,sizeof(char),43U);});_tmp2E9.f1=_tmp384;});_tmp2E9;});void*_tmp234[1U];_tmp234[0]=& _tmp235;({unsigned _tmp385=loc;Cyc_Warn_err2(_tmp385,_tag_fat(_tmp234,sizeof(void*),1U));});});return;}else{_LL4F: _tmp233=_tmp232;_LL50: {void*exn=_tmp233;(int)_rethrow(exn);}}_LL4C:;}}}
# 839
return;}}else{_LL24: _tmp1F3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E7)->f3;_LL25: {struct Cyc_List_List*dps=_tmp1F3;
# 868
for(0;dps != 0;dps=dps->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple12*)dps->hd)).f2);}
return;}}}else{_LL22: _tmp1F2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E7)->f3;_LL23: {struct Cyc_List_List*dps=_tmp1F2;
# 866
_tmp1F3=dps;goto _LL25;}}case 0U: _LLA: _LLB:
# 841
 return;case 3U: _LLC: _tmp1F0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1;_tmp1F1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E7)->f2;_LLD: {struct Cyc_Absyn_Vardecl*vd=_tmp1F0;struct Cyc_Absyn_Pat*p2=_tmp1F1;
# 843
_tmp1EE=vd;_tmp1EF=p2;goto _LLF;}case 1U: _LLE: _tmp1EE=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1;_tmp1EF=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1E7)->f2;_LLF: {struct Cyc_Absyn_Vardecl*vd=_tmp1EE;struct Cyc_Absyn_Pat*p2=_tmp1EF;
# 845
Cyc_Binding_resolve_pat(env,p2);
_tmp1ED=vd;goto _LL11;}case 4U: _LL10: _tmp1ED=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1E7)->f2;_LL11: {struct Cyc_Absyn_Vardecl*vd=_tmp1ED;
_tmp1EC=vd;goto _LL13;}case 2U: _LL12: _tmp1EC=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1E7)->f2;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp1EC;
# 849
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL3;}case 6U: _LL14: _tmp1EB=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1;_LL15: {struct Cyc_Absyn_Pat*p2=_tmp1EB;
# 851
Cyc_Binding_resolve_pat(env,p2);return;}case 5U: _LL16: _tmp1EA=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1;_LL17: {struct Cyc_List_List*ps=_tmp1EA;
# 853
for(0;ps != 0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
goto _LL3;}case 17U: _LL18: _tmp1E9=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1E7)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp1E9;
# 857
Cyc_Binding_resolve_exp(env,e);return;}case 9U: _LL1A: _LL1B:
# 859
 goto _LL1D;case 10U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
 return;case 8U: _LL26: _tmp1E8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E7)->f3;_LL27: {struct Cyc_List_List*ps=_tmp1E8;
# 872
for(0;ps != 0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 13U: _LL28: _LL29:
 goto _LL2B;default: _LL2A: _LL2B:
 return;}_LL3:;}}struct _tuple13{void*f1;void*f2;};
# 880
void Cyc_Binding_resolve_rgnpo(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)po->hd)).f2);}}struct _tuple14{unsigned f1;struct _tuple0*f2;int f3;};
# 894 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 897
unsigned loc=d->loc;
void*_tmp237=d->r;void*_stmttmp19=_tmp237;void*_tmp238=_stmttmp19;struct Cyc_List_List*_tmp23B;struct Cyc_List_List*_tmp23A;struct Cyc_List_List*_tmp239;struct Cyc_List_List*_tmp23C;struct Cyc_List_List*_tmp23E;struct _tuple0*_tmp23D;struct Cyc_List_List*_tmp240;struct _fat_ptr*_tmp23F;struct Cyc_Absyn_Datatypedecl*_tmp241;struct Cyc_Absyn_Enumdecl*_tmp242;struct Cyc_Absyn_Aggrdecl*_tmp243;struct Cyc_Absyn_Typedefdecl*_tmp244;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Vardecl*_tmp246;struct Cyc_Absyn_Tvar*_tmp245;struct Cyc_List_List*_tmp248;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_Absyn_Pat*_tmp249;struct Cyc_Absyn_Fndecl*_tmp24B;struct Cyc_Absyn_Vardecl*_tmp24C;switch(*((int*)_tmp238)){case 0U: _LL1: _tmp24C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp24C;
# 901
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
# 906
if(vd->initializer != 0 && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1U: _LL3: _tmp24B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp24B;
# 911
Cyc_Binding_absolutize_decl(loc,env,fd->name,fd->sc);{
struct _tuple0*_tmp24D=fd->name;struct _tuple0*_stmttmp1A=_tmp24D;struct _tuple0*_tmp24E=_stmttmp1A;struct _fat_ptr*_tmp250;union Cyc_Absyn_Nmspace _tmp24F;_LL24: _tmp24F=_tmp24E->f1;_tmp250=_tmp24E->f2;_LL25: {union Cyc_Absyn_Nmspace decl_ns=_tmp24F;struct _fat_ptr*decl_name=_tmp250;
# 914
struct Cyc_List_List*_tmp251=Cyc_Binding_get_fun_vardecls(1,loc,env,(fd->i).args,(fd->i).cyc_varargs);struct Cyc_List_List*vds=_tmp251;
({struct Cyc_Core_Opt*_tmp386=({struct Cyc_Core_Opt*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252->v=vds;_tmp252;});fd->param_vardecls=_tmp386;});
# 917
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,fd->name);{
# 920
struct Cyc_Dict_Dict*_tmp253=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp253;
if(old_locals != 0)
({struct Cyc_Dict_Dict*_tmp387=({struct Cyc_Dict_Dict*_tmp254=_cycalloc(sizeof(*_tmp254));*_tmp254=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp254;});env->local_vars=_tmp387;});else{
# 924
({struct Cyc_Dict_Dict*_tmp389=({struct Cyc_Dict_Dict*_tmp255=_cycalloc(sizeof(*_tmp255));({struct Cyc_Dict_Dict _tmp388=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp255=_tmp388;});_tmp255;});env->local_vars=_tmp389;});}
{struct Cyc_List_List*_tmp256=vds;struct Cyc_List_List*vds1=_tmp256;for(0;vds1 != 0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _tmp38D=({struct Cyc_Dict_Dict _tmp38C=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp38B=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp38C,_tmp38B,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp258=_cycalloc(sizeof(*_tmp258));
_tmp258->tag=0U,({void*_tmp38A=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->tag=3U,_tmp257->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_tmp257;});_tmp258->f1=_tmp38A;});_tmp258;}));});
# 926
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp38D;});}}
# 929
Cyc_Binding_resolve_function_stuff(loc,env,fd->i);
# 931
if(old_locals != 0){
# 933
({struct Cyc_Dict_Dict _tmp391=({struct Cyc_Dict_Dict _tmp390=*old_locals;struct _fat_ptr*_tmp38F=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp390,_tmp38F,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));
_tmp25A->tag=0U,({void*_tmp38E=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->tag=2U,_tmp259->f1=fd;_tmp259;});_tmp25A->f1=_tmp38E;});_tmp25A;}));});
# 933
*old_locals=_tmp391;});
# 935
({struct Cyc_Dict_Dict _tmp395=({struct Cyc_Dict_Dict _tmp394=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp393=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp394,_tmp393,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp25C=_cycalloc(sizeof(*_tmp25C));
_tmp25C->tag=0U,({void*_tmp392=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B->tag=2U,_tmp25B->f1=fd;_tmp25B;});_tmp25C->f1=_tmp392;});_tmp25C;}));});
# 935
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp395;});}else{
# 938
struct Cyc_Binding_ResolveNSEnv*_tmp25D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp25D;
({struct Cyc_Dict_Dict _tmp399=({struct Cyc_Dict_Dict _tmp398=decl_ns_data->ordinaries;struct _fat_ptr*_tmp397=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp398,_tmp397,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp25F=_cycalloc(sizeof(*_tmp25F));
# 941
_tmp25F->tag=0U,({void*_tmp396=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->tag=2U,_tmp25E->f1=fd;_tmp25E;});_tmp25F->f1=_tmp396;});_tmp25F;}));});
# 939
decl_ns_data->ordinaries=_tmp399;});}
# 944
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,fd->body);
# 947
env->local_vars=old_locals;
goto _LL0;}}}}case 2U: _LL5: _tmp249=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp24A=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp238)->f3;_LL6: {struct Cyc_Absyn_Pat*p=_tmp249;struct Cyc_Absyn_Exp*exp=_tmp24A;
# 951
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp261=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EA;_tmp2EA.tag=0U,({struct _fat_ptr _tmp39A=({const char*_tmp262="let not allowed at top-level";_tag_fat(_tmp262,sizeof(char),29U);});_tmp2EA.f1=_tmp39A;});_tmp2EA;});void*_tmp260[1U];_tmp260[0]=& _tmp261;({unsigned _tmp39B=loc;Cyc_Warn_err2(_tmp39B,_tag_fat(_tmp260,sizeof(void*),1U));});});
goto _LL0;}
# 955
Cyc_Binding_resolve_exp(env,exp);
Cyc_Binding_resolve_pat(env,p);
goto _LL0;}case 3U: _LL7: _tmp248=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL8: {struct Cyc_List_List*vds=_tmp248;
# 960
for(0;vds != 0;vds=vds->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)vds->hd);}
goto _LL0;}case 4U: _LL9: _tmp245=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp246=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp238)->f2;_tmp247=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp238)->f3;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp245;struct Cyc_Absyn_Vardecl*vd=_tmp246;struct Cyc_Absyn_Exp*open_exp_opt=_tmp247;
# 965
if(open_exp_opt != 0)
Cyc_Binding_resolve_exp(env,open_exp_opt);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
goto _LL0;}case 8U: _LLB: _tmp244=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp244;
# 971
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp264=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB;_tmp2EB.tag=0U,({struct _fat_ptr _tmp39C=({const char*_tmp265="nested type definitions are not yet supported";_tag_fat(_tmp265,sizeof(char),46U);});_tmp2EB.f1=_tmp39C;});_tmp2EB;});void*_tmp263[1U];_tmp263[0]=& _tmp264;({unsigned _tmp39D=loc;Cyc_Warn_err2(_tmp39D,_tag_fat(_tmp263,sizeof(void*),1U));});});
goto _LL0;}
# 975
Cyc_Binding_absolutize_decl(loc,env,td->name,td->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp266=td->name;struct _tuple0*_stmttmp1B=_tmp266;struct _tuple0*_tmp267=_stmttmp1B;struct _fat_ptr*_tmp269;union Cyc_Absyn_Nmspace _tmp268;_LL27: _tmp268=_tmp267->f1;_tmp269=_tmp267->f2;_LL28: {union Cyc_Absyn_Nmspace decl_ns=_tmp268;struct _fat_ptr*decl_name=_tmp269;
# 978
struct Cyc_Binding_ResolveNSEnv*_tmp26A=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp26A;
if(td->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(td->defn));
# 982
({struct Cyc_Dict_Dict _tmp39E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(decl_ns_data->typedefs,decl_name,td);decl_ns_data->typedefs=_tmp39E;});
# 984
goto _LL0;}}}case 5U: _LLD: _tmp243=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmp243;
# 987
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC;_tmp2EC.tag=0U,({struct _fat_ptr _tmp39F=({const char*_tmp26D="nested type definitions are not yet supported";_tag_fat(_tmp26D,sizeof(char),46U);});_tmp2EC.f1=_tmp39F;});_tmp2EC;});void*_tmp26B[1U];_tmp26B[0]=& _tmp26C;({unsigned _tmp3A0=loc;Cyc_Warn_err2(_tmp3A0,_tag_fat(_tmp26B,sizeof(void*),1U));});});
goto _LL0;}
# 991
Cyc_Binding_absolutize_decl(loc,env,ad->name,ad->sc);{
struct _tuple0*_tmp26E=ad->name;struct _tuple0*_stmttmp1C=_tmp26E;struct _tuple0*_tmp26F=_stmttmp1C;struct _fat_ptr*_tmp271;union Cyc_Absyn_Nmspace _tmp270;_LL2A: _tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;_LL2B: {union Cyc_Absyn_Nmspace decl_ns=_tmp270;struct _fat_ptr*decl_name=_tmp271;
# 994
struct Cyc_Binding_ResolveNSEnv*_tmp272=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp272;
# 996
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(decl_ns_data->aggrdecls,decl_name)&& ad->impl == 0)
goto _LL0;
# 999
({struct Cyc_Dict_Dict _tmp3A1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(decl_ns_data->aggrdecls,decl_name,ad);decl_ns_data->aggrdecls=_tmp3A1;});
# 1001
({struct Cyc_Dict_Dict _tmp3A4=({struct Cyc_Dict_Dict _tmp3A3=decl_ns_data->ordinaries;struct _fat_ptr*_tmp3A2=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3A3,_tmp3A2,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp273=_cycalloc(sizeof(*_tmp273));
_tmp273->tag=1U,_tmp273->f1=ad;_tmp273;}));});
# 1001
decl_ns_data->ordinaries=_tmp3A4;});
# 1003
if(ad->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp274=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl);struct Cyc_Absyn_AggrdeclImpl*_stmttmp1D=_tmp274;struct Cyc_Absyn_AggrdeclImpl*_tmp275=_stmttmp1D;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp276;_LL2D: _tmp276=_tmp275->rgn_po;_tmp277=_tmp275->fields;_LL2E: {struct Cyc_List_List*rpo=_tmp276;struct Cyc_List_List*fs=_tmp277;
Cyc_Binding_resolve_rgnpo(loc,env,rpo);
Cyc_Binding_resolve_aggrfields(loc,env,fs);}}
# 1008
goto _LL0;}}}case 7U: _LLF: _tmp242=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp242;
# 1011
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp279=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED;_tmp2ED.tag=0U,({struct _fat_ptr _tmp3A5=({const char*_tmp27A="nested type definitions are not yet supported";_tag_fat(_tmp27A,sizeof(char),46U);});_tmp2ED.f1=_tmp3A5;});_tmp2ED;});void*_tmp278[1U];_tmp278[0]=& _tmp279;({unsigned _tmp3A6=loc;Cyc_Warn_err2(_tmp3A6,_tag_fat(_tmp278,sizeof(void*),1U));});});
goto _LL0;}
# 1015
Cyc_Binding_absolutize_decl(loc,env,ed->name,ed->sc);{
struct _tuple0*_tmp27B=ed->name;struct _tuple0*_stmttmp1E=_tmp27B;struct _tuple0*_tmp27C=_stmttmp1E;struct _fat_ptr*_tmp27E;union Cyc_Absyn_Nmspace _tmp27D;_LL30: _tmp27D=_tmp27C->f1;_tmp27E=_tmp27C->f2;_LL31: {union Cyc_Absyn_Nmspace decl_ns=_tmp27D;struct _fat_ptr*decl_name=_tmp27E;
# 1018
struct Cyc_Binding_ResolveNSEnv*_tmp27F=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp27F;
# 1020
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(decl_ns_data->enumdecls,decl_name)&& ed->fields == 0)
goto _LL0;
# 1024
({struct Cyc_Dict_Dict _tmp3A7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(decl_ns_data->enumdecls,decl_name,ed);decl_ns_data->enumdecls=_tmp3A7;});
# 1026
if(ed->fields != 0){
# 1028
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp280=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp280;
Cyc_Binding_absolutize_decl(f->loc,env,f->name,ed->sc);
if(f->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
({struct Cyc_Dict_Dict _tmp3AA=({struct Cyc_Dict_Dict _tmp3A9=decl_ns_data->ordinaries;struct _fat_ptr*_tmp3A8=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3A9,_tmp3A8,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp281=_cycalloc(sizeof(*_tmp281));
# 1035
_tmp281->tag=3U,_tmp281->f1=ed,_tmp281->f2=f;_tmp281;}));});
# 1033
decl_ns_data->ordinaries=_tmp3AA;});}}
# 1037
goto _LL0;}}}case 6U: _LL11: _tmp241=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL12: {struct Cyc_Absyn_Datatypedecl*tud=_tmp241;
# 1041
{struct _handler_cons _tmp282;_push_handler(& _tmp282);{int _tmp284=0;if(setjmp(_tmp282.handler))_tmp284=1;if(!_tmp284){
{struct Cyc_List_List*_tmp285=((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,tud->name,Cyc_Binding_lookup_datatypedecl);struct Cyc_List_List*decls=_tmp285;
struct Cyc_Absyn_Datatypedecl*_tmp286=(struct Cyc_Absyn_Datatypedecl*)decls->hd;struct Cyc_Absyn_Datatypedecl*last_decl=_tmp286;
if(!last_decl->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287->tag=Cyc_Binding_BindingError;_tmp287;}));
tud->name=last_decl->name;
tud->is_extensible=1;{
struct _tuple0*_tmp288=tud->name;struct _tuple0*_stmttmp1F=_tmp288;struct _tuple0*_tmp289=_stmttmp1F;struct _fat_ptr*_tmp28B;union Cyc_Absyn_Nmspace _tmp28A;_LL33: _tmp28A=_tmp289->f1;_tmp28B=_tmp289->f2;_LL34: {union Cyc_Absyn_Nmspace decl_ns=_tmp28A;struct _fat_ptr*decl_name=_tmp28B;
struct Cyc_Binding_ResolveNSEnv*_tmp28C=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp28C;
({struct Cyc_Dict_Dict _tmp3AD=({struct Cyc_Dict_Dict _tmp3AC=decl_ns_data->datatypedecls;struct _fat_ptr*_tmp3AB=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp3AC,_tmp3AB,({struct Cyc_List_List*_tmp28D=_cycalloc(sizeof(*_tmp28D));
_tmp28D->hd=tud,_tmp28D->tl=decls;_tmp28D;}));});
# 1050
decl_ns_data->datatypedecls=_tmp3AD;});}}}
# 1042
;_pop_handler();}else{void*_tmp283=(void*)Cyc_Core_get_exn_thrown();void*_tmp28E=_tmp283;void*_tmp28F;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp28E)->tag == Cyc_Binding_BindingError){_LL36: _LL37:
# 1054
 Cyc_Binding_absolutize_topdecl(loc,env,tud->name,tud->sc);{
struct _tuple0*_tmp290=tud->name;struct _tuple0*_stmttmp20=_tmp290;struct _tuple0*_tmp291=_stmttmp20;struct _fat_ptr*_tmp293;union Cyc_Absyn_Nmspace _tmp292;_LL3B: _tmp292=_tmp291->f1;_tmp293=_tmp291->f2;_LL3C: {union Cyc_Absyn_Nmspace decl_ns=_tmp292;struct _fat_ptr*decl_name=_tmp293;
struct Cyc_Binding_ResolveNSEnv*_tmp294=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp294;
# 1058
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(decl_ns_data->datatypedecls,decl_name)&& tud->fields == 0)
# 1060
goto _LL35;
({struct Cyc_Dict_Dict _tmp3B0=({struct Cyc_Dict_Dict _tmp3AF=decl_ns_data->datatypedecls;struct _fat_ptr*_tmp3AE=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp3AF,_tmp3AE,({struct Cyc_List_List*_tmp295=_cycalloc(sizeof(*_tmp295));
_tmp295->hd=tud,_tmp295->tl=0;_tmp295;}));});
# 1061
decl_ns_data->datatypedecls=_tmp3B0;});
# 1063
goto _LL35;}}}else{_LL38: _tmp28F=_tmp28E;_LL39: {void*exn=_tmp28F;(int)_rethrow(exn);}}_LL35:;}}}{
# 1066
struct _tuple0*_tmp296=tud->name;struct _tuple0*_stmttmp21=_tmp296;struct _tuple0*_tmp297=_stmttmp21;struct _fat_ptr*_tmp299;union Cyc_Absyn_Nmspace _tmp298;_LL3E: _tmp298=_tmp297->f1;_tmp299=_tmp297->f2;_LL3F: {union Cyc_Absyn_Nmspace decl_ns=_tmp298;struct _fat_ptr*decl_name=_tmp299;
if(tud->fields != 0){
struct Cyc_List_List*_tmp29A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmp29A;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp29B=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp29B;
{struct Cyc_List_List*_tmp29C=f->typs;struct Cyc_List_List*tqts=_tmp29C;for(0;tqts != 0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)tqts->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp29D=(*f->name).f1;union Cyc_Absyn_Nmspace _stmttmp22=_tmp29D;union Cyc_Absyn_Nmspace _tmp29E=_stmttmp22;switch((_tmp29E.Abs_n).tag){case 1U: if((_tmp29E.Rel_n).val == 0){_LL41: _LL42:
# 1074
 if(tud->is_extensible)
({union Cyc_Absyn_Nmspace _tmp3B1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*f->name).f1=_tmp3B1;});else{
# 1077
(*f->name).f1=(*tud->name).f1;}
goto _LL40;}else{_LL43: _LL44:
# 1080
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE;_tmp2EE.tag=0U,({struct _fat_ptr _tmp3B2=({const char*_tmp2A1="qualified datatype field declaratations not allowed";_tag_fat(_tmp2A1,sizeof(char),52U);});_tmp2EE.f1=_tmp3B2;});_tmp2EE;});void*_tmp29F[1U];_tmp29F[0]=& _tmp2A0;({unsigned _tmp3B3=f->loc;Cyc_Warn_err2(_tmp3B3,_tag_fat(_tmp29F,sizeof(void*),1U));});});
return;}case 2U: _LL45: _LL46:
 goto _LL40;default: _LL47: _LL48:
({void*_tmp2A2=0U;({struct _fat_ptr _tmp3B4=({const char*_tmp2A3="datatype field Loc_n or C_n";_tag_fat(_tmp2A3,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp3B4,_tag_fat(_tmp2A2,sizeof(void*),0U));});});}_LL40:;}{
# 1085
struct Cyc_Binding_ResolveNSEnv*_tmp2A4=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*f->name).f1);struct Cyc_Binding_ResolveNSEnv*ns_data=_tmp2A4;
({struct Cyc_Dict_Dict _tmp3B7=({struct Cyc_Dict_Dict _tmp3B6=ns_data->ordinaries;struct _fat_ptr*_tmp3B5=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3B6,_tmp3B5,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));
# 1088
_tmp2A5->tag=2U,_tmp2A5->f1=tud,_tmp2A5->f2=f;_tmp2A5;}));});
# 1086
ns_data->ordinaries=_tmp3B7;});}}}
# 1090
goto _LL0;}}}case 9U: _LL13: _tmp23F=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp240=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp238)->f2;_LL14: {struct _fat_ptr*v=_tmp23F;struct Cyc_List_List*ds2=_tmp240;
# 1093
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,v,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,ds2);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL0;}case 10U: _LL15: _tmp23D=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp23E=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp238)->f2;_LL16: {struct _tuple0*qv=_tmp23D;struct Cyc_List_List*ds2=_tmp23E;
# 1098
((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,qv);
Cyc_Binding_resolve_decls(env,ds2);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL0;}case 11U: _LL17: _tmp23C=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL18: {struct Cyc_List_List*ds2=_tmp23C;
# 1104
int _tmp2A6=env->in_cinclude;int old=_tmp2A6;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
env->in_cinclude=old;
goto _LL0;}case 12U: _LL19: _tmp239=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp23A=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp238)->f2;_tmp23B=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp238)->f3;_LL1A: {struct Cyc_List_List*ds2=_tmp239;struct Cyc_List_List*ovrs=_tmp23A;struct Cyc_List_List*exports=_tmp23B;
# 1115
{struct Cyc_List_List*_tmp2A7=exports;struct Cyc_List_List*exs=_tmp2A7;for(0;exs != 0;exs=exs->tl){
struct _tuple14*_tmp2A8=(struct _tuple14*)exs->hd;struct _tuple14*_stmttmp23=_tmp2A8;struct _tuple14*_tmp2A9=_stmttmp23;struct _tuple0*_tmp2AB;unsigned _tmp2AA;_LL4A: _tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2A9->f2;_LL4B: {unsigned loc=_tmp2AA;struct _tuple0*qv=_tmp2AB;
Cyc_Binding_absolutize_decl(loc,env,qv,Cyc_Absyn_ExternC);}}}
# 1123
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF;_tmp2EF.tag=0U,({struct _fat_ptr _tmp3B8=({const char*_tmp2AE="extern \"C include\" not at toplevel";_tag_fat(_tmp2AE,sizeof(char),35U);});_tmp2EF.f1=_tmp3B8;});_tmp2EF;});void*_tmp2AC[1U];_tmp2AC[0]=& _tmp2AD;({unsigned _tmp3B9=loc;Cyc_Warn_err2(_tmp3B9,_tag_fat(_tmp2AC,sizeof(void*),1U));});});
goto _LL0;}{
# 1127
struct Cyc_Binding_ResolveNSEnv*_tmp2AF=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);struct Cyc_Binding_ResolveNSEnv*data=_tmp2AF;
struct Cyc_Dict_Dict _tmp2B0=data->ordinaries;struct Cyc_Dict_Dict old_dict=_tmp2B0;
int _tmp2B1=env->in_cinclude;int old=_tmp2B1;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
Cyc_Binding_resolve_decls(env,ovrs);{
struct Cyc_Dict_Dict _tmp2B2=data->ordinaries;struct Cyc_Dict_Dict new_dict=_tmp2B2;
struct Cyc_Dict_Dict _tmp2B3=old_dict;struct Cyc_Dict_Dict out_dict=_tmp2B3;
{struct Cyc_List_List*_tmp2B4=exports;struct Cyc_List_List*exs=_tmp2B4;for(0;exs != 0;exs=exs->tl){
struct _tuple14*_tmp2B5=(struct _tuple14*)exs->hd;struct _tuple14*_stmttmp24=_tmp2B5;struct _tuple14*_tmp2B6=_stmttmp24;struct _fat_ptr*_tmp2B8;unsigned _tmp2B7;_LL4D: _tmp2B7=_tmp2B6->f1;_tmp2B8=(_tmp2B6->f2)->f2;_LL4E: {unsigned loc=_tmp2B7;struct _fat_ptr*v=_tmp2B8;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(new_dict,v)||
((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(old_dict,v)&&({
void*_tmp3BA=((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(old_dict,v);_tmp3BA == ((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(new_dict,v);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1;_tmp2F1.tag=0U,_tmp2F1.f1=*v;_tmp2F1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F0;_tmp2F0.tag=0U,({struct _fat_ptr _tmp3BB=({const char*_tmp2BC=" is exported but not defined";_tag_fat(_tmp2BC,sizeof(char),29U);});_tmp2F0.f1=_tmp3BB;});_tmp2F0;});void*_tmp2B9[2U];_tmp2B9[0]=& _tmp2BA,_tmp2B9[1]=& _tmp2BB;({unsigned _tmp3BC=loc;Cyc_Warn_err2(_tmp3BC,_tag_fat(_tmp2B9,sizeof(void*),2U));});});
out_dict=({struct Cyc_Dict_Dict _tmp3BE=out_dict;struct _fat_ptr*_tmp3BD=v;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3BE,_tmp3BD,((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(new_dict,v));});}}}
# 1143
data->ordinaries=out_dict;
env->in_cinclude=old;
# 1147
Cyc_Cifc_user_overrides(loc,ds2,ovrs);
goto _LL0;}}}case 13U: _LL1B: _LL1C:
# 1150
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 1157
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1162
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp2BD=({struct Cyc_Binding_Env*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->in_cinclude=0,({struct Cyc_Binding_NSCtxt*_tmp3BF=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2C0->ns=_tmp3BF;}),_tmp2C0->local_vars=0;_tmp2C0;});struct Cyc_Binding_Env*env=_tmp2BD;
({struct Cyc_Binding_Env*_tmp3C2=env;Cyc_Binding_resolve_decl(_tmp3C2,({struct Cyc_Absyn_Decl*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));({void*_tmp3C1=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp3C0=Cyc_Absyn_exn_tud();_tmp2BE->f1=_tmp3C0;});_tmp2BE;});_tmp2BF->r=_tmp3C1;}),_tmp2BF->loc=0U;_tmp2BF;}));});
Cyc_Binding_resolve_decls(env,tds);}

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
void*_tmp0=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->curr_ns=0,({
struct Cyc_List_List*_tmp2F4=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({void*_tmp2F3=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0U,_tmp1->f1=0;_tmp1;});_tmp2->hd=_tmp2F3;}),_tmp2->tl=0;_tmp2;});_tmp3->availables=_tmp2F4;}),({
struct Cyc_Dict_Dict _tmp2F2=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,_tmp0);_tmp3->ns_data=_tmp2F2;});_tmp3;});}
# 64
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp4=abs_ns;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;switch((_tmp4.Abs_n).tag){case 3U: _LL1: _tmp7=(_tmp4.C_n).val;_LL2:
 _tmp8=_tmp7;goto _LL4;case 2U: _LL3: _tmp8=(_tmp4.Abs_n).val;_LL4:
 return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp8);default: _LL5: _LL6:
({void*_tmp5=0U;({struct _fat_ptr _tmp2F5=({const char*_tmp6="Binding:get_ns_data: relative ns";_tag_fat(_tmp6,sizeof(char),33U);});Cyc_Warn_impos(_tmp2F5,_tag_fat(_tmp5,sizeof(void*),0U));});});}_LL0:;}
# 82 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 85
struct Cyc_List_List*_tmp9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp9))
return _tmp9;
{struct Cyc_List_List*_tmpA=ctxt->availables;for(0;_tmpA != 0;_tmpA=_tmpA->tl){
void*_tmpB=(void*)_tmpA->hd;void*_tmpC=_tmpB;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmpE;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->tag == 1U){_LL1: _tmpE=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->f1;_LL2:
 _tmpF=_tmpE;goto _LL4;}else{_LL3: _tmpF=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpC)->f1;_LL4: {
# 92
struct Cyc_List_List*_tmpD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpF,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpD))
return _tmpD;
goto _LL0;}}_LL0:;}}
# 97
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3;_tmp2C3.tag=0U,({struct _fat_ptr _tmp2F6=({const char*_tmp16="namespace ";_tag_fat(_tmp16,sizeof(char),11U);});_tmp2C3.f1=_tmp2F6;});_tmp2C3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2;_tmp2C2.tag=0U,({struct _fat_ptr _tmp2F8=(struct _fat_ptr)({struct Cyc_List_List*_tmp2F7=rel_ns;Cyc_str_sepstr(_tmp2F7,({const char*_tmp15="::";_tag_fat(_tmp15,sizeof(char),3U);}));});_tmp2C2.f1=_tmp2F8;});_tmp2C2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1;_tmp2C1.tag=0U,({struct _fat_ptr _tmp2F9=({const char*_tmp14=" not found";_tag_fat(_tmp14,sizeof(char),11U);});_tmp2C1.f1=_tmp2F9;});_tmp2C1;});void*_tmp10[3U];_tmp10[0]=& _tmp11,_tmp10[1]=& _tmp12,_tmp10[2]=& _tmp13;({unsigned _tmp2FA=loc;Cyc_Warn_err2(_tmp2FA,_tag_fat(_tmp10,sizeof(void*),3U));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=Cyc_Binding_BindingError;_tmp17;}));}
# 102
void*Cyc_Binding_resolve_lookup(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _fat_ptr*)){
struct _tuple0*_tmp18=qv;union Cyc_Absyn_Nmspace _tmp41;struct _fat_ptr*_tmp40;_LL1: _tmp41=_tmp18->f1;_tmp40=_tmp18->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp19=_tmp41;struct Cyc_List_List*_tmp3F;struct Cyc_List_List*_tmp3E;struct Cyc_List_List*_tmp3D;switch((_tmp19.Abs_n).tag){case 1U: if((_tmp19.Rel_n).val == 0){_LL4: _LL5:
# 106
{struct _handler_cons _tmp1A;_push_handler(& _tmp1A);{int _tmp1C=0;if(setjmp(_tmp1A.handler))_tmp1C=1;if(!_tmp1C){{void*_tmp1D=({void*(*_tmp2FC)(void*,struct _fat_ptr*)=lookup;void*_tmp2FB=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp2FC(_tmp2FB,_tmp40);});_npop_handler(0U);return _tmp1D;};_pop_handler();}else{void*_tmp1B=(void*)Cyc_Core_get_exn_thrown();void*_tmp1E=_tmp1B;void*_tmp1F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1E)->tag == Cyc_Dict_Absent){_LLF: _LL10:
 goto _LLE;}else{_LL11: _tmp1F=_tmp1E;_LL12:(int)_rethrow(_tmp1F);}_LLE:;}};}
{struct Cyc_List_List*_tmp20=ctxt->availables;for(0;_tmp20 != 0;_tmp20=_tmp20->tl){
void*_tmp21=(void*)_tmp20->hd;void*_tmp22=_tmp21;struct Cyc_List_List*_tmp2A;struct Cyc_List_List*_tmp29;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp22)->tag == 1U){_LL14: _tmp29=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp22)->f1;_LL15:
 _tmp2A=_tmp29;goto _LL17;}else{_LL16: _tmp2A=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp22)->f1;_LL17:
# 112
{struct _handler_cons _tmp23;_push_handler(& _tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))_tmp25=1;if(!_tmp25){{void*_tmp26=({void*(*_tmp2FE)(void*,struct _fat_ptr*)=lookup;void*_tmp2FD=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2A);_tmp2FE(_tmp2FD,_tmp40);});_npop_handler(0U);return _tmp26;};_pop_handler();}else{void*_tmp24=(void*)Cyc_Core_get_exn_thrown();void*_tmp27=_tmp24;void*_tmp28;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp27)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
 goto _LL18;}else{_LL1B: _tmp28=_tmp27;_LL1C:(int)_rethrow(_tmp28);}_LL18:;}};}
goto _LL13;}_LL13:;}}
# 116
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=Cyc_Binding_BindingError;_tmp2B;}));}else{_LL6: _tmp3D=(_tmp19.Rel_n).val;_LL7: {
# 118
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct Cyc_List_List*_tmp2F=Cyc_Binding_resolve_rel_ns(loc,ctxt,_tmp3D);
void*_tmp30=({void*(*_tmp300)(void*,struct _fat_ptr*)=lookup;void*_tmp2FF=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2F);_tmp300(_tmp2FF,_tmp40);});_npop_handler(0U);return _tmp30;}
# 119
;_pop_handler();}else{void*_tmp2D=(void*)Cyc_Core_get_exn_thrown();void*_tmp31=_tmp2D;void*_tmp33;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp31)->tag == Cyc_Dict_Absent){_LL1E: _LL1F:
# 121
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->tag=Cyc_Binding_BindingError;_tmp32;}));}else{_LL20: _tmp33=_tmp31;_LL21:(int)_rethrow(_tmp33);}_LL1D:;}};}}case 3U: _LL8: _tmp3E=(_tmp19.C_n).val;_LL9:
 _tmp3F=_tmp3E;goto _LLB;case 2U: _LLA: _tmp3F=(_tmp19.Abs_n).val;_LLB: {
# 124
struct _handler_cons _tmp34;_push_handler(& _tmp34);{int _tmp36=0;if(setjmp(_tmp34.handler))_tmp36=1;if(!_tmp36){{void*_tmp37=({void*(*_tmp302)(void*,struct _fat_ptr*)=lookup;void*_tmp301=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp3F);_tmp302(_tmp301,_tmp40);});_npop_handler(0U);return _tmp37;};_pop_handler();}else{void*_tmp35=(void*)Cyc_Core_get_exn_thrown();void*_tmp38=_tmp35;void*_tmp3A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp38)->tag == Cyc_Dict_Absent){_LL23: _LL24:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=Cyc_Binding_BindingError;_tmp39;}));}else{_LL25: _tmp3A=_tmp38;_LL26:(int)_rethrow(_tmp3A);}_LL22:;}};}default: _LLC: _LLD:
({void*_tmp3B=0U;({struct _fat_ptr _tmp303=({const char*_tmp3C="lookup local in global";_tag_fat(_tmp3C,sizeof(char),23U);});Cyc_Warn_impos(_tmp303,_tag_fat(_tmp3B,sizeof(void*),0U));});});}_LL3:;};}
# 130
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp42=ctxt->curr_ns;
struct Cyc_List_List*_tmp43=({struct Cyc_List_List*_tmp304=_tmp42;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp304,({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->hd=subname,_tmp46->tl=0;_tmp46;}));});
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp43))
({struct Cyc_Dict_Dict _tmp307=({struct Cyc_Dict_Dict _tmp306=ctxt->ns_data;struct Cyc_List_List*_tmp305=_tmp43;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(_tmp306,_tmp305,mkdata(env));});ctxt->ns_data=_tmp307;});
ctxt->curr_ns=_tmp43;
({struct Cyc_List_List*_tmp309=({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));({void*_tmp308=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=0U,_tmp44->f1=_tmp43;_tmp44;});_tmp45->hd=_tmp308;}),_tmp45->tl=ctxt->availables;_tmp45;});ctxt->availables=_tmp309;});}
# 138
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp47=0U;({struct _fat_ptr _tmp30A=({const char*_tmp48="leaving topmost namespace";_tag_fat(_tmp48,sizeof(char),26U);});Cyc_Warn_impos(_tmp30A,_tag_fat(_tmp47,sizeof(void*),0U));});});{
void*_tmp49=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp4A=_tmp49;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp4A)->tag == 0U){_LL1: _LL2:
# 143
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp30B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp30B;});
goto _LL0;}else{_LL3: _LL4:
# 147
({void*_tmp4B=0U;({struct _fat_ptr _tmp30C=({const char*_tmp4C="leaving using as namespace";_tag_fat(_tmp4C,sizeof(char),27U);});Cyc_Warn_impos(_tmp30C,_tag_fat(_tmp4B,sizeof(void*),0U));});});}_LL0:;};}
# 150
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp4D=usename;union Cyc_Absyn_Nmspace _tmp5B;struct _fat_ptr*_tmp5A;_LL1: _tmp5B=_tmp4D->f1;_tmp5A=_tmp4D->f2;_LL2:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp4E=_tmp5B;struct Cyc_List_List*_tmp57;struct Cyc_List_List*_tmp56;switch((_tmp4E.Loc_n).tag){case 1U: _LL4: _tmp56=(_tmp4E.Rel_n).val;_LL5:
# 155
 ns=({unsigned _tmp30F=loc;struct Cyc_Binding_NSCtxt*_tmp30E=ctxt;Cyc_Binding_resolve_rel_ns(_tmp30F,_tmp30E,({struct Cyc_List_List*_tmp30D=_tmp56;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30D,({struct Cyc_List_List*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->hd=_tmp5A,_tmp4F->tl=0;_tmp4F;}));}));});{
struct Cyc_List_List*_tmp50=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);
({union Cyc_Absyn_Nmspace _tmp310=Cyc_Absyn_Abs_n(_tmp50,0);(*usename).f1=_tmp310;});
goto _LL3;};case 2U: _LL6: _tmp57=(_tmp4E.Abs_n).val;_LL7:
# 160
 ns=({struct Cyc_List_List*_tmp311=_tmp57;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp311,({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->hd=_tmp5A,_tmp51->tl=0;_tmp51;}));});
goto _LL3;case 4U: _LL8: _LL9:
({void*_tmp52=0U;({struct _fat_ptr _tmp312=({const char*_tmp53="enter_using, Loc_n";_tag_fat(_tmp53,sizeof(char),19U);});Cyc_Warn_impos(_tmp312,_tag_fat(_tmp52,sizeof(void*),0U));});});default: _LLA: _LLB:
({void*_tmp54=0U;({struct _fat_ptr _tmp313=({const char*_tmp55="enter_using, C_n";_tag_fat(_tmp55,sizeof(char),17U);});Cyc_Warn_impos(_tmp313,_tag_fat(_tmp54,sizeof(void*),0U));});});}_LL3:;}
# 165
({struct Cyc_List_List*_tmp315=({struct Cyc_List_List*_tmp59=_cycalloc(sizeof(*_tmp59));({void*_tmp314=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=1U,_tmp58->f1=ns;_tmp58;});_tmp59->hd=_tmp314;}),_tmp59->tl=ctxt->availables;_tmp59;});ctxt->availables=_tmp315;});
return ns;};}
# 168
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp5C=0U;({struct _fat_ptr _tmp316=({const char*_tmp5D="leaving topmost namespace as a using";_tag_fat(_tmp5D,sizeof(char),37U);});Cyc_Warn_impos(_tmp316,_tag_fat(_tmp5C,sizeof(void*),0U));});});{
void*_tmp5E=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp5F=_tmp5E;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp5F)->tag == 1U){_LL1: _LL2:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL0;}else{_LL3: _LL4:
({void*_tmp60=0U;({struct _fat_ptr _tmp317=({const char*_tmp61="leaving namespace as using";_tag_fat(_tmp61,sizeof(char),27U);});Cyc_Warn_impos(_tmp317,_tag_fat(_tmp60,sizeof(void*),0U));});});}_LL0:;};}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
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
struct _tuple0*_tmp63=qv;union Cyc_Absyn_Nmspace _tmp67;struct _fat_ptr*_tmp66;_LL1: _tmp67=_tmp63->f1;_tmp66=_tmp63->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp64=_tmp67;switch((_tmp64.Rel_n).tag){case 4U: _LL4: _LL5:
# 245
 if(Cyc_Binding_at_toplevel(env)|| !((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp66))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=Cyc_Binding_BindingError;_tmp65;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp66);case 1U: if((_tmp64.Rel_n).val == 0){_LL6: _LL7:
# 249
 if(!Cyc_Binding_at_toplevel(env)&&((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp66))
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp66);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 253
 return((void*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL3:;};}
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
union Cyc_Absyn_Nmspace _tmp68=(*qv).f1;union Cyc_Absyn_Nmspace _tmp69=_tmp68;switch((_tmp69.Abs_n).tag){case 1U: if((_tmp69.Rel_n).val == 0){_LL1: _LL2:
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
struct Cyc_Dict_Dict*_tmp70=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp70;}
# 298
void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _fat_ptr hides_what;
struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){
{void*_tmp74=Cyc_Binding_lookup_ordinary(loc,env,q);void*_tmp75=_tmp74;void*_tmp81;switch(*((int*)_tmp75)){case 0U: _LL1: _tmp81=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp75)->f1;_LL2:
# 303
 if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0U);return;}
{void*_tmp76=_tmp81;switch(*((int*)_tmp76)){case 1U: _LLC: _LLD:
 hides_what=({const char*_tmp77="global variable";_tag_fat(_tmp77,sizeof(char),16U);});goto _LLB;case 4U: _LLE: _LLF:
 hides_what=({const char*_tmp78="local variable";_tag_fat(_tmp78,sizeof(char),15U);});goto _LLB;case 3U: _LL10: _LL11:
 hides_what=({const char*_tmp79="parameter";_tag_fat(_tmp79,sizeof(char),10U);});goto _LLB;case 5U: _LL12: _LL13:
 hides_what=({const char*_tmp7A="pattern variable";_tag_fat(_tmp7A,sizeof(char),17U);});goto _LLB;case 2U: _LL14: _LL15:
 hides_what=({const char*_tmp7B="function";_tag_fat(_tmp7B,sizeof(char),9U);});goto _LLB;default: _LL16: _LL17:
({void*_tmp7C=0U;({struct _fat_ptr _tmp322=({const char*_tmp7D="shadowing free variable!";_tag_fat(_tmp7D,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp322,_tag_fat(_tmp7C,sizeof(void*),0U));});});}_LLB:;}
# 313
goto _LL0;case 1U: _LL3: _LL4:
 hides_what=({const char*_tmp7E="struct constructor";_tag_fat(_tmp7E,sizeof(char),19U);});goto _LL0;case 2U: _LL5: _LL6:
 hides_what=({const char*_tmp7F="datatype constructor";_tag_fat(_tmp7F,sizeof(char),21U);});goto _LL0;case 3U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 hides_what=({const char*_tmp80="enum tag";_tag_fat(_tmp80,sizeof(char),9U);});goto _LL0;}_LL0:;}
# 319
({struct Cyc_Warn_String_Warn_Warg_struct _tmp83=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8;_tmp2C8.tag=0U,({struct _fat_ptr _tmp323=({const char*_tmp85="declaration hides ";_tag_fat(_tmp85,sizeof(char),19U);});_tmp2C8.f1=_tmp323;});_tmp2C8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C7;_tmp2C7.tag=0U,_tmp2C7.f1=hides_what;_tmp2C7;});void*_tmp82[2U];_tmp82[0]=& _tmp83,_tmp82[1]=& _tmp84;({unsigned _tmp324=loc;Cyc_Warn_warn2(_tmp324,_tag_fat(_tmp82,sizeof(void*),2U));});});
_npop_handler(0U);return;
# 301
;_pop_handler();}else{void*_tmp72=(void*)Cyc_Core_get_exn_thrown();void*_tmp86=_tmp72;void*_tmp87;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp86)->tag == Cyc_Binding_BindingError){_LL19: _LL1A:
# 321
 return;}else{_LL1B: _tmp87=_tmp86;_LL1C:(int)_rethrow(_tmp87);}_LL18:;}};}
# 324
void Cyc_Binding_resolve_and_add_vardecl(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp88=vd->name;struct _tuple0*_tmp89=_tmp88;union Cyc_Absyn_Nmspace _tmp90;struct _fat_ptr*_tmp8F;_LL1: _tmp90=_tmp89->f1;_tmp8F=_tmp89->f2;_LL2:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp328=({struct Cyc_Dict_Dict _tmp327=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp326=_tmp8F;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp327,_tmp326,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B->tag=0U,({void*_tmp325=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=4U,_tmp8A->f1=vd;_tmp8A;});_tmp8B->f1=_tmp325;});_tmp8B;}));});
# 331
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp328;});else{
# 334
struct Cyc_Binding_ResolveNSEnv*_tmp8C=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp90);
({struct Cyc_Dict_Dict _tmp32C=({struct Cyc_Dict_Dict _tmp32B=_tmp8C->ordinaries;struct _fat_ptr*_tmp32A=_tmp8F;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp32B,_tmp32A,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));
# 337
_tmp8E->tag=0U,({void*_tmp329=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=1U,_tmp8D->f1=vd;_tmp8D;});_tmp8E->f1=_tmp329;});_tmp8E;}));});
# 335
_tmp8C->ordinaries=_tmp32C;});}};}
# 341
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp91=s->r;void*_tmp92=_tmp91;struct Cyc_Absyn_Stmt*_tmpAA;struct Cyc_List_List*_tmpA9;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_List_List*_tmpA7;struct Cyc_Absyn_Decl*_tmpA6;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_List_List*_tmpA3;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Stmt*_tmp9F;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Stmt*_tmp9D;struct Cyc_Absyn_Stmt*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Stmt*_tmp99;struct Cyc_Absyn_Stmt*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Stmt*_tmp96;struct Cyc_Absyn_Stmt*_tmp95;struct Cyc_Absyn_Exp*_tmp94;switch(*((int*)_tmp92)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp94=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL4:
 Cyc_Binding_resolve_exp(env,_tmp94);return;case 2U: _LL5: _tmp96=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp95=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL6:
 Cyc_Binding_resolve_stmt(env,_tmp96);Cyc_Binding_resolve_stmt(env,_tmp95);return;case 3U: _LL7: _tmp97=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL8:
 if((unsigned)_tmp97)Cyc_Binding_resolve_exp(env,_tmp97);return;case 4U: _LL9: _tmp9A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp99=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_tmp98=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f3;_LLA:
# 348
 Cyc_Binding_resolve_exp(env,_tmp9A);Cyc_Binding_resolve_stmt(env,_tmp99);Cyc_Binding_resolve_stmt(env,_tmp98);return;case 14U: _LLB: _tmp9C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp9B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp92)->f2).f1;_LLC:
 _tmp9E=_tmp9B;_tmp9D=_tmp9C;goto _LLE;case 5U: _LLD: _tmp9E=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp92)->f1).f1;_tmp9D=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LLE:
 Cyc_Binding_resolve_exp(env,_tmp9E);Cyc_Binding_resolve_stmt(env,_tmp9D);return;case 9U: _LLF: _tmpA2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f2).f1;_tmpA0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f3).f1;_tmp9F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f4;_LL10:
# 352
 Cyc_Binding_resolve_exp(env,_tmpA2);Cyc_Binding_resolve_exp(env,_tmpA1);Cyc_Binding_resolve_exp(env,_tmpA0);
Cyc_Binding_resolve_stmt(env,_tmp9F);
return;case 6U: _LL11: _LL12:
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmpA3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL18:
# 359
 for(0;_tmpA3 != 0;_tmpA3=_tmpA3->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpA3->hd);}
return;case 13U: _LL19: _tmpA4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1A:
 Cyc_Binding_resolve_stmt(env,_tmpA4);return;case 12U: _LL1B: _tmpA6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1C: {
# 364
struct Cyc_Dict_Dict _tmp93=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmpA6);
Cyc_Binding_resolve_stmt(env,_tmpA5);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp93;
return;}case 10U: _LL1D: _tmpA8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA7=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1E:
 Cyc_Binding_resolve_exp(env,_tmpA8);Cyc_Binding_resolve_scs(env,_tmpA7);return;default: _LL1F: _tmpAA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL20:
 Cyc_Binding_resolve_stmt(env,_tmpAA);Cyc_Binding_resolve_scs(env,_tmpA9);return;}_LL0:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 373
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpAB=e->r;void*_tmpAC=_tmpAB;void*_tmp13C;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_Datatypedecl*_tmp139;struct Cyc_Absyn_Datatypefield*_tmp138;struct Cyc_Absyn_Enumdecl*_tmp137;struct Cyc_Absyn_Enumfield*_tmp136;void*_tmp135;struct Cyc_Absyn_Enumfield*_tmp134;struct Cyc_List_List*_tmp133;void*_tmp132;struct Cyc_List_List*_tmp131;void*_tmp130;void*_tmp12F;void*_tmp12E;struct Cyc_Absyn_Stmt*_tmp12D;void*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_List_List*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;void**_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_List_List*_tmp10A;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_Exp*_tmp108;void*_tmp107;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Vardecl*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct _tuple0**_tmp102;struct Cyc_List_List*_tmp101;struct Cyc_List_List*_tmp100;struct Cyc_Absyn_Aggrdecl**_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_List_List*_tmpFD;int*_tmpFC;void**_tmpFB;switch(*((int*)_tmpAC)){case 1U: _LL1: _tmpFB=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL2: {
# 376
void*_tmpAD=*_tmpFB;void*_tmpAE=_tmpAD;struct _tuple0*_tmpC9;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpAE)->tag == 0U){_LL56: _tmpC9=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpAE)->f1;_LL57: {
# 378
struct _handler_cons _tmpAF;_push_handler(& _tmpAF);{int _tmpB1=0;if(setjmp(_tmpAF.handler))_tmpB1=1;if(!_tmpB1){{void*_tmpB2=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpC9);void*_tmpB3=_tmpB2;struct Cyc_Absyn_Aggrdecl*_tmpC2;void*_tmpC1;struct Cyc_Absyn_Enumfield*_tmpC0;struct Cyc_Absyn_Enumdecl*_tmpBF;struct Cyc_Absyn_Enumfield*_tmpBE;struct Cyc_Absyn_Datatypedecl*_tmpBD;struct Cyc_Absyn_Datatypefield*_tmpBC;void*_tmpBB;switch(*((int*)_tmpB3)){case 0U: _LL5B: _tmpBB=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpB3)->f1;_LL5C:
*_tmpFB=_tmpBB;_npop_handler(0U);return;case 2U: _LL5D: _tmpBD=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpB3)->f1;_tmpBC=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpB3)->f2;_LL5E:
({void*_tmp32D=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->tag=31U,_tmpB4->f1=0,_tmpB4->f2=_tmpBD,_tmpB4->f3=_tmpBC;_tmpB4;});e->r=_tmp32D;});_npop_handler(0U);return;case 3U: _LL5F: _tmpBF=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpB3)->f1;_tmpBE=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpB3)->f2;_LL60:
({void*_tmp32E=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->tag=32U,_tmpB5->f1=_tmpBF,_tmpB5->f2=_tmpBE;_tmpB5;});e->r=_tmp32E;});_npop_handler(0U);return;case 4U: _LL61: _tmpC1=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpB3)->f1;_tmpC0=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpB3)->f2;_LL62:
({void*_tmp32F=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->tag=33U,_tmpB6->f1=_tmpC1,_tmpB6->f2=_tmpC0;_tmpB6;});e->r=_tmp32F;});_npop_handler(0U);return;default: _LL63: _tmpC2=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpB3)->f1;_LL64:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA;_tmp2CA.tag=0U,({struct _fat_ptr _tmp330=({const char*_tmpBA="bad occurrence of type name ";_tag_fat(_tmpBA,sizeof(char),29U);});_tmp2CA.f1=_tmp330;});_tmp2CA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2C9;_tmp2C9.tag=3U,_tmp2C9.f1=_tmpC9;_tmp2C9;});void*_tmpB7[2U];_tmpB7[0]=& _tmpB8,_tmpB7[1]=& _tmpB9;({unsigned _tmp331=e->loc;Cyc_Warn_err2(_tmp331,_tag_fat(_tmpB7,sizeof(void*),2U));});});_npop_handler(0U);return;}_LL5A:;}
# 378
;_pop_handler();}else{void*_tmpB0=(void*)Cyc_Core_get_exn_thrown();void*_tmpC3=_tmpB0;void*_tmpC8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpC3)->tag == Cyc_Binding_BindingError){_LL66: _LL67:
# 386
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC;_tmp2CC.tag=0U,({struct _fat_ptr _tmp332=({const char*_tmpC7="undeclared identifier ";_tag_fat(_tmpC7,sizeof(char),23U);});_tmp2CC.f1=_tmp332;});_tmp2CC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpC6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2CB;_tmp2CB.tag=3U,_tmp2CB.f1=_tmpC9;_tmp2CB;});void*_tmpC4[2U];_tmpC4[0]=& _tmpC5,_tmpC4[1]=& _tmpC6;({unsigned _tmp333=e->loc;Cyc_Warn_err2(_tmp333,_tag_fat(_tmpC4,sizeof(void*),2U));});});return;}else{_LL68: _tmpC8=_tmpC3;_LL69:(int)_rethrow(_tmpC8);}_LL65:;}};}}else{_LL58: _LL59:
# 388
 return;}_LL55:;}case 10U: _LL3: _tmpFE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpFD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpFC=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL4:
# 391
*_tmpFC=1;
# 393
{struct Cyc_List_List*_tmpCA=_tmpFD;for(0;_tmpCA != 0;_tmpCA=_tmpCA->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpCA->hd);}}{
void*_tmpCB=_tmpFE->r;void*_tmpCC=_tmpCB;void**_tmpEC;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCC)->tag == 1U){_LL6B: _tmpEC=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCC)->f1;_LL6C: {
# 397
void*_tmpCD=*_tmpEC;void*_tmpCE=_tmpCD;struct _tuple0*_tmpEB;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpCE)->tag == 0U){_LL70: _tmpEB=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpCE)->f1;_LL71: {
# 399
struct _handler_cons _tmpCF;_push_handler(& _tmpCF);{int _tmpD1=0;if(setjmp(_tmpCF.handler))_tmpD1=1;if(!_tmpD1){{void*_tmpD2=Cyc_Binding_lookup_ordinary(_tmpFE->loc,env,_tmpEB);void*_tmpD3=_tmpD2;struct Cyc_Absyn_Datatypedecl*_tmpE4;struct Cyc_Absyn_Datatypefield*_tmpE3;struct Cyc_Absyn_Aggrdecl*_tmpE2;void*_tmpE1;switch(*((int*)_tmpD3)){case 0U: _LL75: _tmpE1=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpD3)->f1;_LL76:
# 401
*_tmpEC=_tmpE1;_npop_handler(0U);return;case 1U: _LL77: _tmpE2=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpD3)->f1;_LL78: {
# 403
struct Cyc_List_List*_tmpD4=0;
for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){
_tmpD4=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct _tuple10*_tmp334=({struct _tuple10*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->f1=0,_tmpD5->f2=(struct Cyc_Absyn_Exp*)_tmpFD->hd;_tmpD5;});_tmpD6->hd=_tmp334;}),_tmpD6->tl=_tmpD4;_tmpD6;});}
({void*_tmp336=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->tag=29U,_tmpD7->f1=_tmpE2->name,_tmpD7->f2=0,({struct Cyc_List_List*_tmp335=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD4);_tmpD7->f3=_tmp335;}),_tmpD7->f4=_tmpE2;_tmpD7;});e->r=_tmp336;});
_npop_handler(0U);return;}case 2U: _LL79: _tmpE4=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpD3)->f1;_tmpE3=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpD3)->f2;_LL7A:
# 409
 if(_tmpE3->typs == 0)
# 411
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpD9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2CE;_tmp2CE.tag=3U,_tmp2CE.f1=_tmpE3->name;_tmp2CE;});struct Cyc_Warn_String_Warn_Warg_struct _tmpDA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD;_tmp2CD.tag=0U,({struct _fat_ptr _tmp337=({const char*_tmpDB=" is a constant, not a function";_tag_fat(_tmpDB,sizeof(char),31U);});_tmp2CD.f1=_tmp337;});_tmp2CD;});void*_tmpD8[2U];_tmpD8[0]=& _tmpD9,_tmpD8[1]=& _tmpDA;({unsigned _tmp338=e->loc;Cyc_Warn_err2(_tmp338,_tag_fat(_tmpD8,sizeof(void*),2U));});});
({void*_tmp339=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->tag=31U,_tmpDC->f1=_tmpFD,_tmpDC->f2=_tmpE4,_tmpDC->f3=_tmpE3;_tmpDC;});e->r=_tmp339;});
_npop_handler(0U);return;case 4U: _LL7B: _LL7C:
 goto _LL7E;default: _LL7D: _LL7E:
# 416
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D0;_tmp2D0.tag=3U,_tmp2D0.f1=_tmpEB;_tmp2D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmpDF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF;_tmp2CF.tag=0U,({struct _fat_ptr _tmp33A=({const char*_tmpE0=" is an enum constructor, not a function";_tag_fat(_tmpE0,sizeof(char),40U);});_tmp2CF.f1=_tmp33A;});_tmp2CF;});void*_tmpDD[2U];_tmpDD[0]=& _tmpDE,_tmpDD[1]=& _tmpDF;({unsigned _tmp33B=e->loc;Cyc_Warn_err2(_tmp33B,_tag_fat(_tmpDD,sizeof(void*),2U));});});_npop_handler(0U);return;}_LL74:;}
# 399
;_pop_handler();}else{void*_tmpD0=(void*)Cyc_Core_get_exn_thrown();void*_tmpE5=_tmpD0;void*_tmpEA;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpE5)->tag == Cyc_Binding_BindingError){_LL80: _LL81:
# 418
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D2;_tmp2D2.tag=0U,({struct _fat_ptr _tmp33C=({const char*_tmpE9="undeclared identifier ";_tag_fat(_tmpE9,sizeof(char),23U);});_tmp2D2.f1=_tmp33C;});_tmp2D2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpE8=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D1;_tmp2D1.tag=3U,_tmp2D1.f1=_tmpEB;_tmp2D1;});void*_tmpE6[2U];_tmpE6[0]=& _tmpE7,_tmpE6[1]=& _tmpE8;({unsigned _tmp33D=e->loc;Cyc_Warn_err2(_tmp33D,_tag_fat(_tmpE6,sizeof(void*),2U));});});return;}else{_LL82: _tmpEA=_tmpE5;_LL83:(int)_rethrow(_tmpEA);}_LL7F:;}};}}else{_LL72: _LL73:
# 420
 return;}_LL6F:;}}else{_LL6D: _LL6E:
# 422
 Cyc_Binding_resolve_exp(env,_tmpFE);return;}_LL6A:;};case 29U: _LL5: _tmp102=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp101=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmp100=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpFF=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL6:
# 426
 for(0;_tmp100 != 0;_tmp100=_tmp100->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp100->hd)).f2);}{
struct _handler_cons _tmpED;_push_handler(& _tmpED);{int _tmpEF=0;if(setjmp(_tmpED.handler))_tmpEF=1;if(!_tmpEF){
({struct Cyc_Absyn_Aggrdecl*_tmp33E=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmp102,Cyc_Binding_lookup_aggrdecl);*_tmpFF=_tmp33E;});
*_tmp102=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmpFF))->name;
_npop_handler(0U);return;
# 429
;_pop_handler();}else{void*_tmpEE=(void*)Cyc_Core_get_exn_thrown();void*_tmpF0=_tmpEE;void*_tmpF5;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpF0)->tag == Cyc_Binding_BindingError){_LL85: _LL86:
# 433
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4;_tmp2D4.tag=0U,({struct _fat_ptr _tmp33F=({const char*_tmpF4="unbound struct/union name ";_tag_fat(_tmpF4,sizeof(char),27U);});_tmp2D4.f1=_tmp33F;});_tmp2D4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D3;_tmp2D3.tag=3U,_tmp2D3.f1=*_tmp102;_tmp2D3;});void*_tmpF1[2U];_tmpF1[0]=& _tmpF2,_tmpF1[1]=& _tmpF3;({unsigned _tmp340=e->loc;Cyc_Warn_err2(_tmp340,_tag_fat(_tmpF1,sizeof(void*),2U));});});
return;}else{_LL87: _tmpF5=_tmpF0;_LL88:(int)_rethrow(_tmpF5);}_LL84:;}};};case 27U: _LL7: _tmp105=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp104=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmp103=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL8:
# 438
 Cyc_Binding_resolve_exp(env,_tmp104);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp342=({struct Cyc_Dict_Dict*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct Cyc_Dict_Dict _tmp341=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmpF6=_tmp341;});_tmpF6;});env->local_vars=_tmp342;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmp105);
Cyc_Binding_resolve_exp(env,_tmp103);
env->local_vars=0;
return;}{
# 446
struct Cyc_Dict_Dict _tmpF7=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmp105);
Cyc_Binding_resolve_exp(env,_tmp103);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpF7;
return;};case 36U: _LL9: _tmp106=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLA:
# 454
 for(0;_tmp106 != 0;_tmp106=_tmp106->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp106->hd)).f2);}
return;case 28U: _LLB: _tmp108=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp107=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLC:
# 460
 Cyc_Binding_resolve_exp(env,_tmp108);
Cyc_Binding_resolve_type(e->loc,env,_tmp107);
return;case 2U: _LLD: _LLE:
# 464
 goto _LL10;case 0U: _LLF: _LL10:
 return;case 24U: _LL11: _tmp109=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL12:
# 467
 _tmp10A=_tmp109;goto _LL14;case 3U: _LL13: _tmp10A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL14:
# 469
 for(0;_tmp10A != 0;_tmp10A=_tmp10A->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp10A->hd);}
return;case 41U: _LL15: _tmp10B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL16:
# 473
 _tmp10C=_tmp10B;goto _LL18;case 38U: _LL17: _tmp10C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL18:
 _tmp10D=_tmp10C;goto _LL1A;case 12U: _LL19: _tmp10D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1A:
 _tmp10E=_tmp10D;goto _LL1C;case 18U: _LL1B: _tmp10E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1C:
 _tmp10F=_tmp10E;goto _LL1E;case 11U: _LL1D: _tmp10F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1E:
 _tmp110=_tmp10F;goto _LL20;case 5U: _LL1F: _tmp110=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL20:
 _tmp111=_tmp110;goto _LL22;case 22U: _LL21: _tmp111=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL22:
 _tmp112=_tmp111;goto _LL24;case 21U: _LL23: _tmp112=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL24:
 _tmp113=_tmp112;goto _LL26;case 15U: _LL25: _tmp113=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL26:
 _tmp114=_tmp113;goto _LL28;case 20U: _LL27: _tmp114=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL28:
 Cyc_Binding_resolve_exp(env,_tmp114);return;case 35U: _LL29: _tmp116=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp115=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2A:
# 484
 _tmp118=_tmp116;_tmp117=_tmp115;goto _LL2C;case 9U: _LL2B: _tmp118=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp117=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2C:
 _tmp11A=_tmp118;_tmp119=_tmp117;goto _LL2E;case 4U: _LL2D: _tmp11A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp119=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL2E:
 _tmp11C=_tmp11A;_tmp11B=_tmp119;goto _LL30;case 23U: _LL2F: _tmp11C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp11B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL30:
 _tmp11E=_tmp11C;_tmp11D=_tmp11B;goto _LL32;case 7U: _LL31: _tmp11E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp11D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL32:
 _tmp120=_tmp11E;_tmp11F=_tmp11D;goto _LL34;case 8U: _LL33: _tmp120=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp11F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL34:
 Cyc_Binding_resolve_exp(env,_tmp120);Cyc_Binding_resolve_exp(env,_tmp11F);return;case 34U: _LL35: _tmp123=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).rgn;_tmp122=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).elt_type;_tmp121=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).num_elts;_LL36:
# 492
 if(_tmp123 != 0)Cyc_Binding_resolve_exp(env,_tmp123);
if(_tmp122 != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp122);
Cyc_Binding_resolve_exp(env,_tmp121);
return;case 16U: _LL37: _tmp125=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp124=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL38:
# 498
 if(_tmp125 != 0)Cyc_Binding_resolve_exp(env,_tmp125);
Cyc_Binding_resolve_exp(env,_tmp124);
return;case 6U: _LL39: _tmp128=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp127=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmp126=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL3A:
# 503
 Cyc_Binding_resolve_exp(env,_tmp128);Cyc_Binding_resolve_exp(env,_tmp127);Cyc_Binding_resolve_exp(env,_tmp126);return;case 13U: _LL3B: _tmp12A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp129=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL3C:
# 506
 Cyc_Binding_resolve_exp(env,_tmp12A);
for(0;_tmp129 != 0;_tmp129=_tmp129->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp129->hd);}
return;case 14U: _LL3D: _tmp12C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp12B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL3E:
# 512
 Cyc_Binding_resolve_exp(env,_tmp12B);Cyc_Binding_resolve_type(e->loc,env,_tmp12C);return;case 37U: _LL3F: _tmp12D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL40:
# 514
 Cyc_Binding_resolve_stmt(env,_tmp12D);return;case 39U: _LL41: _tmp12E=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL42:
# 516
 _tmp12F=_tmp12E;goto _LL44;case 19U: _LL43: _tmp12F=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL44:
 _tmp130=_tmp12F;goto _LL46;case 17U: _LL45: _tmp130=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL46:
 Cyc_Binding_resolve_type(e->loc,env,_tmp130);return;case 25U: _LL47: _tmp132=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1)->f3;_tmp131=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL48:
# 521
 Cyc_Binding_resolve_type(e->loc,env,_tmp132);
_tmp133=_tmp131;goto _LL4A;case 26U: _LL49: _tmp133=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL4A:
# 525
 for(0;_tmp133 != 0;_tmp133=_tmp133->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp133->hd)).f2);}
return;case 33U: _LL4B: _tmp135=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp134=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4C:
# 530
 Cyc_Binding_resolve_type(e->loc,env,_tmp135);return;case 32U: _LL4D: _tmp137=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp136=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4E:
 return;case 31U: _LL4F: _tmp13A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp139=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmp138=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL50:
# 533
 for(0;_tmp13A != 0;_tmp13A=_tmp13A->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp13A->hd);}
return;case 30U: _LL51: _tmp13C=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp13B=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL52:
# 537
 Cyc_Binding_resolve_type(e->loc,env,_tmp13C);
for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp13B->hd)).f2);}
return;default: _LL53: _LL54:
# 543
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5;_tmp2D5.tag=0U,({struct _fat_ptr _tmp343=({const char*_tmpFA="asm expressions cannot occur within Cyclone code";_tag_fat(_tmpFA,sizeof(char),49U);});_tmp2D5.f1=_tmp343;});_tmp2D5;});void*_tmpF8[1U];_tmpF8[0]=& _tmpF9;({unsigned _tmp344=e->loc;Cyc_Warn_err2(_tmp344,_tag_fat(_tmpF8,sizeof(void*),1U));});});}_LL0:;}
# 547
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp13D=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp13E=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp13F=_tmp13E;struct Cyc_Absyn_Pat*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Stmt*_tmp140;_LL1: _tmp142=_tmp13F->pattern;_tmp141=_tmp13F->where_clause;_tmp140=_tmp13F->body;_LL2:;
Cyc_Binding_resolve_pat(env,_tmp142);
if(_tmp141 != 0)
Cyc_Binding_resolve_exp(env,_tmp141);
Cyc_Binding_resolve_stmt(env,_tmp140);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp13D;}
# 557
return;}
# 559
void Cyc_Binding_resolve_aggrfields(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp143=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp144=_tmp143;struct _fat_ptr*_tmp14A;struct Cyc_Absyn_Tqual _tmp149;void*_tmp148;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_Absyn_Exp*_tmp145;_LL1: _tmp14A=_tmp144->name;_tmp149=_tmp144->tq;_tmp148=_tmp144->type;_tmp147=_tmp144->width;_tmp146=_tmp144->attributes;_tmp145=_tmp144->requires_clause;_LL2:;
Cyc_Binding_resolve_type(loc,env,_tmp148);
if(_tmp147 != 0)
Cyc_Binding_resolve_exp(env,_tmp147);
if(_tmp145 != 0)
Cyc_Binding_resolve_exp(env,_tmp145);}
# 568
return;}
# 571
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int need_va_name,unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 577
struct Cyc_List_List*_tmp14B=0;
for(0;args != 0;args=args->tl){
struct _tuple8*_tmp14C=(struct _tuple8*)args->hd;struct _tuple8*_tmp14D=_tmp14C;struct _fat_ptr*_tmp153;struct Cyc_Absyn_Tqual _tmp152;void*_tmp151;_LL1: _tmp153=_tmp14D->f1;_tmp152=_tmp14D->f2;_tmp151=_tmp14D->f3;_LL2:;
if(_tmp153 == 0)
continue;{
struct Cyc_Absyn_Vardecl*_tmp14E=({struct _tuple0*_tmp345=({struct _tuple0*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->f1=Cyc_Absyn_Loc_n,_tmp150->f2=_tmp153;_tmp150;});Cyc_Absyn_new_vardecl(0U,_tmp345,_tmp151,0);});
_tmp14E->tq=_tmp152;
_tmp14B=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->hd=_tmp14E,_tmp14F->tl=_tmp14B;_tmp14F;});};}
# 586
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp154=*vai;struct Cyc_Absyn_VarargInfo _tmp155=_tmp154;struct _fat_ptr*_tmp160;struct Cyc_Absyn_Tqual _tmp15F;void*_tmp15E;int _tmp15D;_LL4: _tmp160=_tmp155.name;_tmp15F=_tmp155.tq;_tmp15E=_tmp155.type;_tmp15D=_tmp155.inject;_LL5:;
if(_tmp160 == 0){
if(need_va_name)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp157=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6;_tmp2D6.tag=0U,({struct _fat_ptr _tmp346=({const char*_tmp158="missing name for vararg";_tag_fat(_tmp158,sizeof(char),24U);});_tmp2D6.f1=_tmp346;});_tmp2D6;});void*_tmp156[1U];_tmp156[0]=& _tmp157;({unsigned _tmp347=loc;Cyc_Warn_err2(_tmp347,_tag_fat(_tmp156,sizeof(void*),1U));});});}else{
# 593
void*_tmp159=Cyc_Absyn_fatptr_type(_tmp15E,Cyc_Absyn_heap_rgn_type,_tmp15F,Cyc_Absyn_false_type);
struct Cyc_Absyn_Vardecl*_tmp15A=({struct _tuple0*_tmp348=({struct _tuple0*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->f1=Cyc_Absyn_Loc_n,_tmp15C->f2=_tmp160;_tmp15C;});Cyc_Absyn_new_vardecl(0U,_tmp348,_tmp159,0);});
_tmp14B=({struct Cyc_List_List*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B->hd=_tmp15A,_tmp15B->tl=_tmp14B;_tmp15B;});}}
# 598
_tmp14B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14B);
return _tmp14B;}
# 602
void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo i){
if(i.effect != 0)
Cyc_Binding_resolve_type(loc,env,i.effect);
Cyc_Binding_resolve_type(loc,env,i.ret_type);
{struct Cyc_List_List*_tmp161=i.args;for(0;_tmp161 != 0;_tmp161=_tmp161->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple8*)_tmp161->hd)).f3);}}
if(i.cyc_varargs != 0)
Cyc_Binding_resolve_type(loc,env,(i.cyc_varargs)->type);
Cyc_Binding_resolve_rgnpo(loc,env,i.rgn_po);
if(i.requires_clause != 0)
Cyc_Binding_resolve_exp(env,i.requires_clause);
if(i.ensures_clause != 0){
# 615
struct Cyc_Dict_Dict _tmp162=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
struct _fat_ptr*_tmp163=({struct _fat_ptr*_tmp169=_cycalloc(sizeof(*_tmp169));({struct _fat_ptr _tmp349=({const char*_tmp168="return_value";_tag_fat(_tmp168,sizeof(char),13U);});*_tmp169=_tmp349;});_tmp169;});
struct Cyc_Absyn_Vardecl*_tmp164=({struct _tuple0*_tmp34A=({struct _tuple0*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->f1=Cyc_Absyn_Loc_n,_tmp167->f2=_tmp163;_tmp167;});Cyc_Absyn_new_vardecl(0U,_tmp34A,i.ret_type,0);});
({struct Cyc_Dict_Dict _tmp34E=({struct Cyc_Dict_Dict _tmp34D=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp34C=_tmp163;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp34D,_tmp34C,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp166=_cycalloc(sizeof(*_tmp166));
_tmp166->tag=0U,({void*_tmp34B=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=4U,_tmp165->f1=_tmp164;_tmp165;});_tmp166->f1=_tmp34B;});_tmp166;}));});
# 618
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp34E;});
# 620
Cyc_Binding_resolve_exp(env,i.ensures_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp162;}}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 630
void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp16A=t;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_Absyn_FnInfo _tmp1E1;void*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_List_List*_tmp1DE;void*_tmp1DD;void*_tmp1DC;void*_tmp1DB;void*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_TypeDecl*_tmp1D8;void***_tmp1D7;struct _tuple0**_tmp1D6;struct Cyc_List_List*_tmp1D5;struct Cyc_Absyn_Typedefdecl**_tmp1D4;void*_tmp1D3;struct Cyc_List_List*_tmp1D2;struct Cyc_List_List*_tmp1D1;struct _tuple0**_tmp1D0;struct Cyc_Absyn_Enumdecl*_tmp1CF;union Cyc_Absyn_AggrInfo*_tmp1CE;struct Cyc_List_List*_tmp1CD;union Cyc_Absyn_DatatypeFieldInfo*_tmp1CC;struct Cyc_List_List*_tmp1CB;union Cyc_Absyn_DatatypeInfo*_tmp1CA;struct Cyc_List_List*_tmp1C9;switch(*((int*)_tmp16A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)){case 18U: _LL1: _tmp1CA=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_tmp1C9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2;_LL2:
# 633
 for(0;_tmp1C9 != 0;_tmp1C9=_tmp1C9->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1C9->hd);}{
union Cyc_Absyn_DatatypeInfo _tmp16B=*_tmp1CA;union Cyc_Absyn_DatatypeInfo _tmp16C=_tmp16B;struct _tuple0*_tmp174;int _tmp173;if((_tmp16C.UnknownDatatype).tag == 1){_LL26: _tmp174=((_tmp16C.UnknownDatatype).val).name;_tmp173=((_tmp16C.UnknownDatatype).val).is_extensible;_LL27: {
# 637
struct _handler_cons _tmp16D;_push_handler(& _tmp16D);{int _tmp16F=0;if(setjmp(_tmp16D.handler))_tmp16F=1;if(!_tmp16F){
{struct Cyc_Absyn_Datatypedecl*_tmp170=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp174,Cyc_Binding_lookup_datatypedecl))->hd;
({union Cyc_Absyn_DatatypeInfo _tmp34F=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp2D7;_tmp2D7.name=_tmp170->name,_tmp2D7.is_extensible=_tmp173;_tmp2D7;}));*_tmp1CA=_tmp34F;});
_npop_handler(0U);return;}
# 638
;_pop_handler();}else{void*_tmp16E=(void*)Cyc_Core_get_exn_thrown();void*_tmp171=_tmp16E;void*_tmp172;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp171)->tag == Cyc_Binding_BindingError){_LL2B: _LL2C:
# 642
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp174,Cyc_Absyn_Public);return;}else{_LL2D: _tmp172=_tmp171;_LL2E:(int)_rethrow(_tmp172);}_LL2A:;}};}}else{_LL28: _LL29:
 return;}_LL25:;};case 19U: _LL3: _tmp1CC=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_tmp1CB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2;_LL4:
# 646
 for(0;_tmp1CB != 0;_tmp1CB=_tmp1CB->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CB->hd);}{
union Cyc_Absyn_DatatypeFieldInfo _tmp175=*_tmp1CC;union Cyc_Absyn_DatatypeFieldInfo _tmp176=_tmp175;struct _tuple0*_tmp191;struct _tuple0*_tmp190;int _tmp18F;if((_tmp176.UnknownDatatypefield).tag == 1){_LL30: _tmp191=((_tmp176.UnknownDatatypefield).val).datatype_name;_tmp190=((_tmp176.UnknownDatatypefield).val).field_name;_tmp18F=((_tmp176.UnknownDatatypefield).val).is_extensible;_LL31:
# 652
{struct _handler_cons _tmp177;_push_handler(& _tmp177);{int _tmp179=0;if(setjmp(_tmp177.handler))_tmp179=1;if(!_tmp179){
{void*_tmp17A=Cyc_Binding_lookup_ordinary(loc,env,_tmp190);void*_tmp17B=_tmp17A;struct Cyc_Absyn_Datatypedecl*_tmp188;struct Cyc_Absyn_Datatypefield*_tmp187;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17B)->tag == 2U){_LL35: _tmp188=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17B)->f1;_tmp187=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp17B)->f2;_LL36: {
# 655
struct Cyc_Absyn_Datatypedecl*_tmp17C=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp191,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp188->name,_tmp17C->name)!= 0){
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp17E=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2DD;_tmp2DD.tag=3U,_tmp2DD.f1=_tmp187->name;_tmp2DD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC;_tmp2DC.tag=0U,({struct _fat_ptr _tmp350=({const char*_tmp186=" is a variant of ";_tag_fat(_tmp186,sizeof(char),18U);});_tmp2DC.f1=_tmp350;});_tmp2DC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp180=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2DB;_tmp2DB.tag=3U,_tmp2DB.f1=_tmp17C->name;_tmp2DB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp181=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA;_tmp2DA.tag=0U,({struct _fat_ptr _tmp351=({const char*_tmp185=" not ";_tag_fat(_tmp185,sizeof(char),6U);});_tmp2DA.f1=_tmp351;});_tmp2DA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp182=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D9;_tmp2D9.tag=3U,_tmp2D9.f1=_tmp188->name;_tmp2D9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp183=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D8;_tmp2D8.tag=0U,({
struct _fat_ptr _tmp352=({const char*_tmp184=" (shadowing not yet implemented properly)";_tag_fat(_tmp184,sizeof(char),42U);});_tmp2D8.f1=_tmp352;});_tmp2D8;});void*_tmp17D[6U];_tmp17D[0]=& _tmp17E,_tmp17D[1]=& _tmp17F,_tmp17D[2]=& _tmp180,_tmp17D[3]=& _tmp181,_tmp17D[4]=& _tmp182,_tmp17D[5]=& _tmp183;({unsigned _tmp353=loc;Cyc_Warn_err2(_tmp353,_tag_fat(_tmp17D,sizeof(void*),6U));});});
_npop_handler(0U);return;}
# 661
({union Cyc_Absyn_DatatypeFieldInfo _tmp354=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp2DE;_tmp2DE.datatype_name=_tmp188->name,_tmp2DE.field_name=_tmp187->name,_tmp2DE.is_extensible=_tmp18F;_tmp2DE;}));*_tmp1CC=_tmp354;});
# 663
_npop_handler(0U);return;}}else{_LL37: _LL38:
 goto _LL34;}_LL34:;}
# 653
;_pop_handler();}else{void*_tmp178=(void*)Cyc_Core_get_exn_thrown();void*_tmp189=_tmp178;void*_tmp18A;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp189)->tag == Cyc_Binding_BindingError){_LL3A: _LL3B:
# 666
 goto _LL39;}else{_LL3C: _tmp18A=_tmp189;_LL3D:(int)_rethrow(_tmp18A);}_LL39:;}};}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp18C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E0;_tmp2E0.tag=3U,_tmp2E0.f1=_tmp190;_tmp2E0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF;_tmp2DF.tag=0U,({struct _fat_ptr _tmp355=({const char*_tmp18E=" is not a datatype field";_tag_fat(_tmp18E,sizeof(char),25U);});_tmp2DF.f1=_tmp355;});_tmp2DF;});void*_tmp18B[2U];_tmp18B[0]=& _tmp18C,_tmp18B[1]=& _tmp18D;({unsigned _tmp356=loc;Cyc_Warn_err2(_tmp356,_tag_fat(_tmp18B,sizeof(void*),2U));});});return;}else{_LL32: _LL33:
 return;}_LL2F:;};case 20U: _LL5: _tmp1CE=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_tmp1CD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2;_LL6:
# 672
 for(0;_tmp1CD != 0;_tmp1CD=_tmp1CD->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CD->hd);}{
union Cyc_Absyn_AggrInfo _tmp192=*_tmp1CE;union Cyc_Absyn_AggrInfo _tmp193=_tmp192;enum Cyc_Absyn_AggrKind _tmp1A2;struct _tuple0*_tmp1A1;struct Cyc_Core_Opt*_tmp1A0;if((_tmp193.UnknownAggr).tag == 1){_LL3F: _tmp1A2=((_tmp193.UnknownAggr).val).f1;_tmp1A1=((_tmp193.UnknownAggr).val).f2;_tmp1A0=((_tmp193.UnknownAggr).val).f3;_LL40: {
# 676
struct _handler_cons _tmp194;_push_handler(& _tmp194);{int _tmp196=0;if(setjmp(_tmp194.handler))_tmp196=1;if(!_tmp196){
{struct Cyc_Absyn_Aggrdecl*_tmp197=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp1A1,Cyc_Binding_lookup_aggrdecl);
if((int)_tmp197->kind != (int)_tmp1A2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp199=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E1;_tmp2E1.tag=0U,({struct _fat_ptr _tmp357=({const char*_tmp19A="struct vs. union mismatch with earlier declaration";_tag_fat(_tmp19A,sizeof(char),51U);});_tmp2E1.f1=_tmp357;});_tmp2E1;});void*_tmp198[1U];_tmp198[0]=& _tmp199;({unsigned _tmp358=loc;Cyc_Warn_err2(_tmp358,_tag_fat(_tmp198,sizeof(void*),1U));});});
if((((int)_tmp1A2 == (int)1U && _tmp1A0 != 0)&& _tmp197->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp197->impl))->tagged != (int)_tmp1A0->v)
# 682
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2;_tmp2E2.tag=0U,({struct _fat_ptr _tmp359=({const char*_tmp19D="@tagged mismatch with earlier declaration";_tag_fat(_tmp19D,sizeof(char),42U);});_tmp2E2.f1=_tmp359;});_tmp2E2;});void*_tmp19B[1U];_tmp19B[0]=& _tmp19C;({unsigned _tmp35A=loc;Cyc_Warn_err2(_tmp35A,_tag_fat(_tmp19B,sizeof(void*),1U));});});
({union Cyc_Absyn_AggrInfo _tmp35B=Cyc_Absyn_UnknownAggr(_tmp1A2,_tmp197->name,_tmp1A0);*_tmp1CE=_tmp35B;});
_npop_handler(0U);return;}
# 677
;_pop_handler();}else{void*_tmp195=(void*)Cyc_Core_get_exn_thrown();void*_tmp19E=_tmp195;void*_tmp19F;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp19E)->tag == Cyc_Binding_BindingError){_LL44: _LL45:
# 686
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp1A1,Cyc_Absyn_Public);return;}else{_LL46: _tmp19F=_tmp19E;_LL47:(int)_rethrow(_tmp19F);}_LL43:;}};}}else{_LL41: _LL42:
 return;}_LL3E:;};case 15U: _LL7: _tmp1D0=(struct _tuple0**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_tmp1CF=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f2;_LL8:
# 690
 if(_tmp1CF != 0)
return;{
struct _handler_cons _tmp1A3;_push_handler(& _tmp1A3);{int _tmp1A5=0;if(setjmp(_tmp1A3.handler))_tmp1A5=1;if(!_tmp1A5){
{struct Cyc_Absyn_Enumdecl*_tmp1A6=((struct Cyc_Absyn_Enumdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1D0,Cyc_Binding_lookup_enumdecl);
*_tmp1D0=_tmp1A6->name;
_npop_handler(0U);return;}
# 693
;_pop_handler();}else{void*_tmp1A4=(void*)Cyc_Core_get_exn_thrown();void*_tmp1A7=_tmp1A4;void*_tmp1A8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1A7)->tag == Cyc_Binding_BindingError){_LL49: _LL4A:
# 697
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp1D0,Cyc_Absyn_Public);return;}else{_LL4B: _tmp1A8=_tmp1A7;_LL4C:(int)_rethrow(_tmp1A8);}_LL48:;}};};case 16U: _LLF: _tmp1D1=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1)->f1;_LL10: {
# 735
struct Cyc_Binding_ResolveNSEnv*_tmp1BF=({struct Cyc_Binding_NSCtxt*_tmp35D=env->ns;((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(_tmp35D,({struct Cyc_List_List*_tmp35C=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp35C,
Cyc_Binding_in_cinclude(env));}));});
for(0;_tmp1D1 != 0;_tmp1D1=_tmp1D1->tl){
struct Cyc_Absyn_Enumfield*_tmp1C0=(struct Cyc_Absyn_Enumfield*)_tmp1D1->hd;
if(_tmp1C0->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp1C0->tag));
({struct Cyc_Dict_Dict _tmp360=({struct Cyc_Dict_Dict _tmp35F=_tmp1BF->ordinaries;struct _fat_ptr*_tmp35E=(*_tmp1C0->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp35F,_tmp35E,(void*)({struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->tag=4U,_tmp1C1->f1=t,_tmp1C1->f2=_tmp1C0;_tmp1C1;}));});_tmp1BF->ordinaries=_tmp360;});}
# 743
return;}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2 == 0){_LL15: _LL16:
# 747
 return;}else{_LL17: _tmp1D3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f1;_tmp1D2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16A)->f2;_LL18:
# 749
 for(0;_tmp1D2 != 0;_tmp1D2=_tmp1D2->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1D2->hd);}
return;}}case 8U: _LL9: _tmp1D6=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16A)->f1;_tmp1D5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16A)->f2;_tmp1D4=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16A)->f3;_LLA:
# 700
 for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1D5->hd);}
{struct _handler_cons _tmp1A9;_push_handler(& _tmp1A9);{int _tmp1AB=0;if(setjmp(_tmp1A9.handler))_tmp1AB=1;if(!_tmp1AB){
{struct Cyc_Absyn_Typedefdecl*_tmp1AC=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1D6,Cyc_Binding_lookup_typedefdecl);
# 705
*_tmp1D6=_tmp1AC->name;
_npop_handler(0U);return;}
# 703
;_pop_handler();}else{void*_tmp1AA=(void*)Cyc_Core_get_exn_thrown();void*_tmp1AD=_tmp1AA;void*_tmp1AE;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1AD)->tag == Cyc_Binding_BindingError){_LL4E: _LL4F:
# 707
 goto _LL4D;}else{_LL50: _tmp1AE=_tmp1AD;_LL51:(int)_rethrow(_tmp1AE);}_LL4D:;}};}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4;_tmp2E4.tag=0U,({struct _fat_ptr _tmp361=({const char*_tmp1B2="unbound typdef name ";_tag_fat(_tmp1B2,sizeof(char),21U);});_tmp2E4.f1=_tmp361;});_tmp2E4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1B1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E3;_tmp2E3.tag=3U,_tmp2E3.f1=*_tmp1D6;_tmp2E3;});void*_tmp1AF[2U];_tmp1AF[0]=& _tmp1B0,_tmp1AF[1]=& _tmp1B1;({unsigned _tmp362=loc;Cyc_Warn_err2(_tmp362,_tag_fat(_tmp1AF,sizeof(void*),2U));});});
return;case 10U: _LLB: _tmp1D8=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16A)->f1;_tmp1D7=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16A)->f2;_LLC: {
# 713
struct Cyc_Dict_Dict*_tmp1B3=env->local_vars;
env->local_vars=0;
{void*_tmp1B4=_tmp1D8->r;void*_tmp1B5=_tmp1B4;struct Cyc_Absyn_Datatypedecl*_tmp1BE;struct Cyc_Absyn_Enumdecl*_tmp1BD;struct Cyc_Absyn_Aggrdecl*_tmp1BC;switch(*((int*)_tmp1B5)){case 0U: _LL53: _tmp1BC=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1B5)->f1;_LL54:
# 717
({struct Cyc_Binding_Env*_tmp364=env;Cyc_Binding_resolve_decl(_tmp364,({struct Cyc_Absyn_Decl*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));({void*_tmp363=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->tag=5U,_tmp1B6->f1=_tmp1BC;_tmp1B6;});_tmp1B7->r=_tmp363;}),_tmp1B7->loc=_tmp1D8->loc;_tmp1B7;}));});goto _LL52;case 1U: _LL55: _tmp1BD=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1B5)->f1;_LL56:
# 719
({struct Cyc_Binding_Env*_tmp366=env;Cyc_Binding_resolve_decl(_tmp366,({struct Cyc_Absyn_Decl*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({void*_tmp365=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->tag=7U,_tmp1B8->f1=_tmp1BD;_tmp1B8;});_tmp1B9->r=_tmp365;}),_tmp1B9->loc=_tmp1D8->loc;_tmp1B9;}));});goto _LL52;default: _LL57: _tmp1BE=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1B5)->f1;_LL58:
# 721
({struct Cyc_Binding_Env*_tmp368=env;Cyc_Binding_resolve_decl(_tmp368,({struct Cyc_Absyn_Decl*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));({void*_tmp367=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->tag=6U,_tmp1BA->f1=_tmp1BE;_tmp1BA;});_tmp1BB->r=_tmp367;}),_tmp1BB->loc=_tmp1D8->loc;_tmp1BB;}));});goto _LL52;}_LL52:;}
# 723
env->local_vars=_tmp1B3;
return;}case 7U: _LLD: _tmp1D9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp16A)->f2;_LLE:
# 730
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp1D9);
return;case 1U: _LL11: _tmp1DA=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16A)->f2;_LL12:
# 744
 if(_tmp1DA != 0)Cyc_Binding_resolve_type(loc,env,_tmp1DA);return;case 2U: _LL13: _LL14:
# 746
 goto _LL16;case 3U: _LL19: _tmp1DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).elt_type;_tmp1DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).rgn;_tmp1DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).bounds;_LL1A:
# 754
 Cyc_Binding_resolve_type(loc,env,_tmp1DD);
Cyc_Binding_resolve_type(loc,env,_tmp1DC);
Cyc_Binding_resolve_type(loc,env,_tmp1DB);
return;case 6U: _LL1B: _tmp1DE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp16A)->f1;_LL1C:
# 760
 for(0;_tmp1DE != 0;_tmp1DE=_tmp1DE->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)_tmp1DE->hd)).f2);}
return;case 4U: _LL1D: _tmp1E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16A)->f1).elt_type;_tmp1DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16A)->f1).num_elts;_LL1E:
# 764
 Cyc_Binding_resolve_type(loc,env,_tmp1E0);
if(_tmp1DF != 0)Cyc_Binding_resolve_exp(env,_tmp1DF);
return;case 5U: _LL1F: _tmp1E1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16A)->f1;_LL20: {
# 771
struct Cyc_List_List*_tmp1C2=Cyc_Binding_get_fun_vardecls(0,loc,env,_tmp1E1.args,_tmp1E1.cyc_varargs);
# 773
struct Cyc_Dict_Dict*_tmp1C3=env->local_vars;
if(_tmp1C3 != 0)
({struct Cyc_Dict_Dict*_tmp369=({struct Cyc_Dict_Dict*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));*_tmp1C4=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1C4;});env->local_vars=_tmp369;});else{
# 777
({struct Cyc_Dict_Dict*_tmp36B=({struct Cyc_Dict_Dict*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));({struct Cyc_Dict_Dict _tmp36A=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp1C5=_tmp36A;});_tmp1C5;});env->local_vars=_tmp36B;});}
{struct Cyc_List_List*_tmp1C6=_tmp1C2;for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){
({struct Cyc_Dict_Dict _tmp36F=({struct Cyc_Dict_Dict _tmp36E=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp36D=(*((struct Cyc_Absyn_Vardecl*)_tmp1C6->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp36E,_tmp36D,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));
_tmp1C8->tag=0U,({void*_tmp36C=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->tag=3U,_tmp1C7->f1=(struct Cyc_Absyn_Vardecl*)_tmp1C6->hd;_tmp1C7;});_tmp1C8->f1=_tmp36C;});_tmp1C8;}));});
# 779
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp36F;});}}
# 782
Cyc_Binding_resolve_function_stuff(loc,env,_tmp1E1);
env->local_vars=_tmp1C3;
return;}case 9U: _LL21: _tmp1E2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp16A)->f1;_LL22:
# 786
 _tmp1E3=_tmp1E2;goto _LL24;default: _LL23: _tmp1E3=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp16A)->f1;_LL24:
 Cyc_Binding_resolve_exp(env,_tmp1E3);return;}_LL0:;}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 791
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1E4=p;void*_tmp236;unsigned _tmp235;_LL1: _tmp236=_tmp1E4->r;_tmp235=_tmp1E4->loc;_LL2:;{
void*_tmp1E5=_tmp236;struct Cyc_List_List*_tmp234;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_List_List*_tmp232;struct Cyc_Absyn_Pat*_tmp231;struct Cyc_Absyn_Vardecl*_tmp230;struct Cyc_Absyn_Vardecl*_tmp22F;struct Cyc_Absyn_Vardecl*_tmp22E;struct Cyc_Absyn_Pat*_tmp22D;struct Cyc_Absyn_Vardecl*_tmp22C;struct Cyc_Absyn_Pat*_tmp22B;struct Cyc_List_List*_tmp22A;struct Cyc_List_List*_tmp229;struct _tuple0*_tmp228;struct Cyc_List_List*_tmp227;struct Cyc_List_List*_tmp226;int _tmp225;struct _tuple0*_tmp224;struct Cyc_List_List*_tmp223;int _tmp222;struct _tuple0*_tmp221;switch(*((int*)_tmp1E5)){case 15U: _LL4: _tmp221=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1;_LL5:
# 795
{struct _handler_cons _tmp1E6;_push_handler(& _tmp1E6);{int _tmp1E8=0;if(setjmp(_tmp1E6.handler))_tmp1E8=1;if(!_tmp1E8){{void*_tmp1E9=Cyc_Binding_lookup_ordinary(_tmp235,env,_tmp221);void*_tmp1EA=_tmp1E9;void*_tmp1F6;struct Cyc_Absyn_Enumfield*_tmp1F5;struct Cyc_Absyn_Enumdecl*_tmp1F4;struct Cyc_Absyn_Enumfield*_tmp1F3;struct Cyc_Absyn_Datatypedecl*_tmp1F2;struct Cyc_Absyn_Datatypefield*_tmp1F1;switch(*((int*)_tmp1EA)){case 0U: _LL2D: _LL2E:
 goto _LL2C;case 1U: _LL2F: _LL30:
# 798
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5;_tmp2E5.tag=0U,({struct _fat_ptr _tmp370=({const char*_tmp1ED="struct tag used without arguments in pattern";_tag_fat(_tmp1ED,sizeof(char),45U);});_tmp2E5.f1=_tmp370;});_tmp2E5;});void*_tmp1EB[1U];_tmp1EB[0]=& _tmp1EC;({unsigned _tmp371=_tmp235;Cyc_Warn_err2(_tmp371,_tag_fat(_tmp1EB,sizeof(void*),1U));});});_npop_handler(0U);return;case 2U: _LL31: _tmp1F2=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1EA)->f1;_tmp1F1=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1EA)->f2;_LL32:
# 800
({void*_tmp372=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->tag=8U,_tmp1EE->f1=_tmp1F2,_tmp1EE->f2=_tmp1F1,_tmp1EE->f3=0,_tmp1EE->f4=0;_tmp1EE;});p->r=_tmp372;});_npop_handler(0U);return;case 3U: _LL33: _tmp1F4=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1EA)->f1;_tmp1F3=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1EA)->f2;_LL34:
# 802
({void*_tmp373=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->tag=13U,_tmp1EF->f1=_tmp1F4,_tmp1EF->f2=_tmp1F3;_tmp1EF;});p->r=_tmp373;});_npop_handler(0U);return;default: _LL35: _tmp1F6=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1EA)->f1;_tmp1F5=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1EA)->f2;_LL36:
# 804
({void*_tmp374=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->tag=14U,_tmp1F0->f1=_tmp1F6,_tmp1F0->f2=_tmp1F5;_tmp1F0;});p->r=_tmp374;});_npop_handler(0U);return;}_LL2C:;}
# 795
;_pop_handler();}else{void*_tmp1E7=(void*)Cyc_Core_get_exn_thrown();void*_tmp1F7=_tmp1E7;void*_tmp1F8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1F7)->tag == Cyc_Binding_BindingError){_LL38: _LL39:
# 805
 goto _LL37;}else{_LL3A: _tmp1F8=_tmp1F7;_LL3B:(int)_rethrow(_tmp1F8);}_LL37:;}};}{
struct Cyc_Absyn_Vardecl*_tmp1F9=Cyc_Absyn_new_vardecl(0U,_tmp221,Cyc_Absyn_void_type,0);
Cyc_Binding_resolve_and_add_vardecl(_tmp235,env,_tmp1F9);
({void*_tmp376=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->tag=1U,_tmp1FA->f1=_tmp1F9,({struct Cyc_Absyn_Pat*_tmp375=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp1FA->f2=_tmp375;});_tmp1FA;});p->r=_tmp376;});
return;};case 16U: _LL6: _tmp224=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1;_tmp223=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E5)->f2;_tmp222=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E5)->f3;_LL7:
# 812
{struct Cyc_List_List*_tmp1FB=_tmp223;for(0;_tmp1FB != 0;_tmp1FB=_tmp1FB->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp1FB->hd);}}
{struct _handler_cons _tmp1FC;_push_handler(& _tmp1FC);{int _tmp1FE=0;if(setjmp(_tmp1FC.handler))_tmp1FE=1;if(!_tmp1FE){{void*_tmp1FF=Cyc_Binding_lookup_ordinary(_tmp235,env,_tmp224);void*_tmp200=_tmp1FF;struct Cyc_Absyn_Datatypedecl*_tmp20D;struct Cyc_Absyn_Datatypefield*_tmp20C;struct Cyc_Absyn_Aggrdecl*_tmp20B;switch(*((int*)_tmp200)){case 0U: _LL3D: _LL3E:
 goto _LL3C;case 1U: _LL3F: _tmp20B=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp200)->f1;_LL40: {
# 817
struct Cyc_List_List*_tmp201=0;
for(0;_tmp223 != 0;_tmp223=_tmp223->tl){
_tmp201=({struct Cyc_List_List*_tmp203=_cycalloc(sizeof(*_tmp203));({struct _tuple12*_tmp377=({struct _tuple12*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->f1=0,_tmp202->f2=(struct Cyc_Absyn_Pat*)_tmp223->hd;_tmp202;});_tmp203->hd=_tmp377;}),_tmp203->tl=_tmp201;_tmp203;});}
({void*_tmp37B=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp37A=({union Cyc_Absyn_AggrInfo*_tmp205=_cycalloc(sizeof(*_tmp205));({union Cyc_Absyn_AggrInfo _tmp379=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp204=_cycalloc(sizeof(*_tmp204));*_tmp204=_tmp20B;_tmp204;}));*_tmp205=_tmp379;});_tmp205;});_tmp206->f1=_tmp37A;}),_tmp206->f2=0,({struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp201);_tmp206->f3=_tmp378;}),_tmp206->f4=_tmp222;_tmp206;});p->r=_tmp37B;});
_npop_handler(0U);return;}case 2U: _LL41: _tmp20D=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp200)->f1;_tmp20C=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp200)->f2;_LL42:
# 823
({void*_tmp37C=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->tag=8U,_tmp207->f1=_tmp20D,_tmp207->f2=_tmp20C,_tmp207->f3=_tmp223,_tmp207->f4=_tmp222;_tmp207;});p->r=_tmp37C;});_npop_handler(0U);return;case 3U: _LL43: _LL44:
 goto _LL46;default: _LL45: _LL46:
# 826
({struct Cyc_Warn_String_Warn_Warg_struct _tmp209=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6;_tmp2E6.tag=0U,({struct _fat_ptr _tmp37D=({const char*_tmp20A="enum tag used with arguments in pattern";_tag_fat(_tmp20A,sizeof(char),40U);});_tmp2E6.f1=_tmp37D;});_tmp2E6;});void*_tmp208[1U];_tmp208[0]=& _tmp209;({unsigned _tmp37E=_tmp235;Cyc_Warn_err2(_tmp37E,_tag_fat(_tmp208,sizeof(void*),1U));});});_npop_handler(0U);return;}_LL3C:;}
# 814
;_pop_handler();}else{void*_tmp1FD=(void*)Cyc_Core_get_exn_thrown();void*_tmp20E=_tmp1FD;void*_tmp20F;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp20E)->tag == Cyc_Binding_BindingError){_LL48: _LL49:
# 827
 goto _LL47;}else{_LL4A: _tmp20F=_tmp20E;_LL4B:(int)_rethrow(_tmp20F);}_LL47:;}};}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp211=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E8;_tmp2E8.tag=3U,_tmp2E8.f1=_tmp224;_tmp2E8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp212=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7;_tmp2E7.tag=0U,({struct _fat_ptr _tmp37F=({const char*_tmp213=" is not a constructor in pattern";_tag_fat(_tmp213,sizeof(char),33U);});_tmp2E7.f1=_tmp37F;});_tmp2E7;});void*_tmp210[2U];_tmp210[0]=& _tmp211,_tmp210[1]=& _tmp212;({unsigned _tmp380=_tmp235;Cyc_Warn_err2(_tmp380,_tag_fat(_tmp210,sizeof(void*),2U));});});return;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1)->UnknownAggr).tag == 1){_LL8: _tmp228=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1)->UnknownAggr).val).f2;_tmp227=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E5)->f2;_tmp226=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E5)->f3;_tmp225=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E5)->f4;_LL9:
# 831
{struct Cyc_List_List*_tmp214=_tmp226;for(0;_tmp214 != 0;_tmp214=_tmp214->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple12*)_tmp214->hd)).f2);}}
{struct _handler_cons _tmp215;_push_handler(& _tmp215);{int _tmp217=0;if(setjmp(_tmp215.handler))_tmp217=1;if(!_tmp217){
{struct Cyc_Absyn_Aggrdecl*_tmp218=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(_tmp235,env->ns,_tmp228,Cyc_Binding_lookup_aggrdecl);
({void*_tmp383=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp382=({union Cyc_Absyn_AggrInfo*_tmp21A=_cycalloc(sizeof(*_tmp21A));({union Cyc_Absyn_AggrInfo _tmp381=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp219=_cycalloc(sizeof(*_tmp219));*_tmp219=_tmp218;_tmp219;}));*_tmp21A=_tmp381;});_tmp21A;});_tmp21B->f1=_tmp382;}),_tmp21B->f2=_tmp227,_tmp21B->f3=_tmp226,_tmp21B->f4=_tmp225;_tmp21B;});p->r=_tmp383;});}
# 834
;_pop_handler();}else{void*_tmp216=(void*)Cyc_Core_get_exn_thrown();void*_tmp21C=_tmp216;void*_tmp220;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp21C)->tag == Cyc_Binding_BindingError){_LL4D: _LL4E:
# 837
({struct Cyc_Warn_String_Warn_Warg_struct _tmp21E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9;_tmp2E9.tag=0U,({struct _fat_ptr _tmp384=({const char*_tmp21F="non-aggregate name has designator patterns";_tag_fat(_tmp21F,sizeof(char),43U);});_tmp2E9.f1=_tmp384;});_tmp2E9;});void*_tmp21D[1U];_tmp21D[0]=& _tmp21E;({unsigned _tmp385=_tmp235;Cyc_Warn_err2(_tmp385,_tag_fat(_tmp21D,sizeof(void*),1U));});});return;}else{_LL4F: _tmp220=_tmp21C;_LL50:(int)_rethrow(_tmp220);}_LL4C:;}};}
# 839
return;}else{_LL24: _tmp229=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E5)->f3;_LL25:
# 868
 for(0;_tmp229 != 0;_tmp229=_tmp229->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple12*)_tmp229->hd)).f2);}
return;}}else{_LL22: _tmp22A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E5)->f3;_LL23:
# 866
 _tmp229=_tmp22A;goto _LL25;}case 0U: _LLA: _LLB:
# 841
 return;case 3U: _LLC: _tmp22C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1;_tmp22B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E5)->f2;_LLD:
# 843
 _tmp22E=_tmp22C;_tmp22D=_tmp22B;goto _LLF;case 1U: _LLE: _tmp22E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1;_tmp22D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1E5)->f2;_LLF:
# 845
 Cyc_Binding_resolve_pat(env,_tmp22D);
_tmp22F=_tmp22E;goto _LL11;case 4U: _LL10: _tmp22F=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1E5)->f2;_LL11:
 _tmp230=_tmp22F;goto _LL13;case 2U: _LL12: _tmp230=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1E5)->f2;_LL13:
# 849
 Cyc_Binding_resolve_and_add_vardecl(_tmp235,env,_tmp230);goto _LL3;case 6U: _LL14: _tmp231=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1;_LL15:
# 851
 Cyc_Binding_resolve_pat(env,_tmp231);return;case 5U: _LL16: _tmp232=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1;_LL17:
# 853
 for(0;_tmp232 != 0;_tmp232=_tmp232->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp232->hd);}
goto _LL3;case 17U: _LL18: _tmp233=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1E5)->f1;_LL19:
# 857
 Cyc_Binding_resolve_exp(env,_tmp233);return;case 9U: _LL1A: _LL1B:
# 859
 goto _LL1D;case 10U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
 return;case 8U: _LL26: _tmp234=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E5)->f3;_LL27:
# 872
 for(0;_tmp234 != 0;_tmp234=_tmp234->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp234->hd);}
return;case 13U: _LL28: _LL29:
 goto _LL2B;default: _LL2A: _LL2B:
 return;}_LL3:;};}struct _tuple13{void*f1;void*f2;};
# 880
void Cyc_Binding_resolve_rgnpo(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)po->hd)).f2);}}struct _tuple14{unsigned f1;struct _tuple0*f2;int f3;};
# 894 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 897
unsigned loc=d->loc;
void*_tmp237=d->r;void*_tmp238=_tmp237;struct Cyc_List_List*_tmp2BC;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BA;struct Cyc_List_List*_tmp2B9;struct _tuple0*_tmp2B8;struct Cyc_List_List*_tmp2B7;struct _fat_ptr*_tmp2B6;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_Datatypedecl*_tmp2B4;struct Cyc_Absyn_Enumdecl*_tmp2B3;struct Cyc_Absyn_Aggrdecl*_tmp2B2;struct Cyc_Absyn_Typedefdecl*_tmp2B1;struct Cyc_Absyn_Tvar*_tmp2B0;struct Cyc_Absyn_Vardecl*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_List_List*_tmp2AD;struct Cyc_Absyn_Pat*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Fndecl*_tmp2AA;struct Cyc_Absyn_Vardecl*_tmp2A9;switch(*((int*)_tmp238)){case 0U: _LL1: _tmp2A9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL2:
# 901
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2A9);
# 906
if(_tmp2A9->initializer != 0 && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp2A9->initializer));
goto _LL0;case 1U: _LL3: _tmp2AA=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL4:
# 911
 Cyc_Binding_absolutize_decl(loc,env,_tmp2AA->name,_tmp2AA->sc);{
struct _tuple0*_tmp239=_tmp2AA->name;struct _tuple0*_tmp23A=_tmp239;union Cyc_Absyn_Nmspace _tmp24B;struct _fat_ptr*_tmp24A;_LL24: _tmp24B=_tmp23A->f1;_tmp24A=_tmp23A->f2;_LL25:;{
# 914
struct Cyc_List_List*_tmp23B=Cyc_Binding_get_fun_vardecls(1,loc,env,(_tmp2AA->i).args,(_tmp2AA->i).cyc_varargs);
({struct Cyc_Core_Opt*_tmp386=({struct Cyc_Core_Opt*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C->v=_tmp23B;_tmp23C;});_tmp2AA->param_vardecls=_tmp386;});
# 917
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,_tmp2AA->name);{
# 920
struct Cyc_Dict_Dict*_tmp23D=env->local_vars;
if(_tmp23D != 0)
({struct Cyc_Dict_Dict*_tmp387=({struct Cyc_Dict_Dict*_tmp23E=_cycalloc(sizeof(*_tmp23E));*_tmp23E=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp23E;});env->local_vars=_tmp387;});else{
# 924
({struct Cyc_Dict_Dict*_tmp389=({struct Cyc_Dict_Dict*_tmp23F=_cycalloc(sizeof(*_tmp23F));({struct Cyc_Dict_Dict _tmp388=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp23F=_tmp388;});_tmp23F;});env->local_vars=_tmp389;});}
{struct Cyc_List_List*_tmp240=_tmp23B;for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
({struct Cyc_Dict_Dict _tmp38D=({struct Cyc_Dict_Dict _tmp38C=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp38B=(*((struct Cyc_Absyn_Vardecl*)_tmp240->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp38C,_tmp38B,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp242=_cycalloc(sizeof(*_tmp242));
_tmp242->tag=0U,({void*_tmp38A=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241->tag=3U,_tmp241->f1=(struct Cyc_Absyn_Vardecl*)_tmp240->hd;_tmp241;});_tmp242->f1=_tmp38A;});_tmp242;}));});
# 926
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp38D;});}}
# 929
Cyc_Binding_resolve_function_stuff(loc,env,_tmp2AA->i);
# 931
if(_tmp23D != 0){
# 933
({struct Cyc_Dict_Dict _tmp391=({struct Cyc_Dict_Dict _tmp390=*_tmp23D;struct _fat_ptr*_tmp38F=_tmp24A;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp390,_tmp38F,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp244=_cycalloc(sizeof(*_tmp244));
_tmp244->tag=0U,({void*_tmp38E=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243->tag=2U,_tmp243->f1=_tmp2AA;_tmp243;});_tmp244->f1=_tmp38E;});_tmp244;}));});
# 933
*_tmp23D=_tmp391;});
# 935
({struct Cyc_Dict_Dict _tmp395=({struct Cyc_Dict_Dict _tmp394=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp393=_tmp24A;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp394,_tmp393,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp246=_cycalloc(sizeof(*_tmp246));
_tmp246->tag=0U,({void*_tmp392=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->tag=2U,_tmp245->f1=_tmp2AA;_tmp245;});_tmp246->f1=_tmp392;});_tmp246;}));});
# 935
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp395;});}else{
# 938
struct Cyc_Binding_ResolveNSEnv*_tmp247=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp24B);
({struct Cyc_Dict_Dict _tmp399=({struct Cyc_Dict_Dict _tmp398=_tmp247->ordinaries;struct _fat_ptr*_tmp397=_tmp24A;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp398,_tmp397,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp249=_cycalloc(sizeof(*_tmp249));
# 941
_tmp249->tag=0U,({void*_tmp396=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->tag=2U,_tmp248->f1=_tmp2AA;_tmp248;});_tmp249->f1=_tmp396;});_tmp249;}));});
# 939
_tmp247->ordinaries=_tmp399;});}
# 944
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp2AA->body);
# 947
env->local_vars=_tmp23D;
goto _LL0;};};};case 2U: _LL5: _tmp2AC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp2AB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp238)->f3;_LL6:
# 951
 if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EA;_tmp2EA.tag=0U,({struct _fat_ptr _tmp39A=({const char*_tmp24E="let not allowed at top-level";_tag_fat(_tmp24E,sizeof(char),29U);});_tmp2EA.f1=_tmp39A;});_tmp2EA;});void*_tmp24C[1U];_tmp24C[0]=& _tmp24D;({unsigned _tmp39B=loc;Cyc_Warn_err2(_tmp39B,_tag_fat(_tmp24C,sizeof(void*),1U));});});
goto _LL0;}
# 955
Cyc_Binding_resolve_exp(env,_tmp2AB);
Cyc_Binding_resolve_pat(env,_tmp2AC);
goto _LL0;case 3U: _LL7: _tmp2AD=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL8:
# 960
 for(0;_tmp2AD != 0;_tmp2AD=_tmp2AD->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp2AD->hd);}
goto _LL0;case 4U: _LL9: _tmp2B0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp2AF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp238)->f2;_tmp2AE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp238)->f3;_LLA:
# 965
 if(_tmp2AE != 0)
Cyc_Binding_resolve_exp(env,_tmp2AE);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp2AF);
goto _LL0;case 8U: _LLB: _tmp2B1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LLC:
# 971
 if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp250=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB;_tmp2EB.tag=0U,({struct _fat_ptr _tmp39C=({const char*_tmp251="nested type definitions are not yet supported";_tag_fat(_tmp251,sizeof(char),46U);});_tmp2EB.f1=_tmp39C;});_tmp2EB;});void*_tmp24F[1U];_tmp24F[0]=& _tmp250;({unsigned _tmp39D=loc;Cyc_Warn_err2(_tmp39D,_tag_fat(_tmp24F,sizeof(void*),1U));});});
goto _LL0;}
# 975
Cyc_Binding_absolutize_decl(loc,env,_tmp2B1->name,_tmp2B1->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp252=_tmp2B1->name;struct _tuple0*_tmp253=_tmp252;union Cyc_Absyn_Nmspace _tmp256;struct _fat_ptr*_tmp255;_LL27: _tmp256=_tmp253->f1;_tmp255=_tmp253->f2;_LL28:;{
# 978
struct Cyc_Binding_ResolveNSEnv*_tmp254=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp256);
if(_tmp2B1->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp2B1->defn));
# 982
({struct Cyc_Dict_Dict _tmp39E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp254->typedefs,_tmp255,_tmp2B1);_tmp254->typedefs=_tmp39E;});
# 984
goto _LL0;};};case 5U: _LLD: _tmp2B2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LLE:
# 987
 if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp258=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC;_tmp2EC.tag=0U,({struct _fat_ptr _tmp39F=({const char*_tmp259="nested type definitions are not yet supported";_tag_fat(_tmp259,sizeof(char),46U);});_tmp2EC.f1=_tmp39F;});_tmp2EC;});void*_tmp257[1U];_tmp257[0]=& _tmp258;({unsigned _tmp3A0=loc;Cyc_Warn_err2(_tmp3A0,_tag_fat(_tmp257,sizeof(void*),1U));});});
goto _LL0;}
# 991
Cyc_Binding_absolutize_decl(loc,env,_tmp2B2->name,_tmp2B2->sc);{
struct _tuple0*_tmp25A=_tmp2B2->name;struct _tuple0*_tmp25B=_tmp25A;union Cyc_Absyn_Nmspace _tmp263;struct _fat_ptr*_tmp262;_LL2A: _tmp263=_tmp25B->f1;_tmp262=_tmp25B->f2;_LL2B:;{
# 994
struct Cyc_Binding_ResolveNSEnv*_tmp25C=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp263);
# 996
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(_tmp25C->aggrdecls,_tmp262)&& _tmp2B2->impl == 0)
goto _LL0;
# 999
({struct Cyc_Dict_Dict _tmp3A1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp25C->aggrdecls,_tmp262,_tmp2B2);_tmp25C->aggrdecls=_tmp3A1;});
# 1001
({struct Cyc_Dict_Dict _tmp3A4=({struct Cyc_Dict_Dict _tmp3A3=_tmp25C->ordinaries;struct _fat_ptr*_tmp3A2=_tmp262;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3A3,_tmp3A2,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));
_tmp25D->tag=1U,_tmp25D->f1=_tmp2B2;_tmp25D;}));});
# 1001
_tmp25C->ordinaries=_tmp3A4;});
# 1003
if(_tmp2B2->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp25E=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B2->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp25F=_tmp25E;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp260;_LL2D: _tmp261=_tmp25F->rgn_po;_tmp260=_tmp25F->fields;_LL2E:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp261);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp260);}
# 1008
goto _LL0;};};case 7U: _LLF: _tmp2B3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL10:
# 1011
 if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp265=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED;_tmp2ED.tag=0U,({struct _fat_ptr _tmp3A5=({const char*_tmp266="nested type definitions are not yet supported";_tag_fat(_tmp266,sizeof(char),46U);});_tmp2ED.f1=_tmp3A5;});_tmp2ED;});void*_tmp264[1U];_tmp264[0]=& _tmp265;({unsigned _tmp3A6=loc;Cyc_Warn_err2(_tmp3A6,_tag_fat(_tmp264,sizeof(void*),1U));});});
goto _LL0;}
# 1015
Cyc_Binding_absolutize_decl(loc,env,_tmp2B3->name,_tmp2B3->sc);{
struct _tuple0*_tmp267=_tmp2B3->name;struct _tuple0*_tmp268=_tmp267;union Cyc_Absyn_Nmspace _tmp26D;struct _fat_ptr*_tmp26C;_LL30: _tmp26D=_tmp268->f1;_tmp26C=_tmp268->f2;_LL31:;{
# 1018
struct Cyc_Binding_ResolveNSEnv*_tmp269=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp26D);
# 1020
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(_tmp269->enumdecls,_tmp26C)&& _tmp2B3->fields == 0)
goto _LL0;
# 1024
({struct Cyc_Dict_Dict _tmp3A7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp269->enumdecls,_tmp26C,_tmp2B3);_tmp269->enumdecls=_tmp3A7;});
# 1026
if(_tmp2B3->fields != 0){
# 1028
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B3->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp26A=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp26A->loc,env,_tmp26A->name,_tmp2B3->sc);
if(_tmp26A->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp26A->tag));
({struct Cyc_Dict_Dict _tmp3AA=({struct Cyc_Dict_Dict _tmp3A9=_tmp269->ordinaries;struct _fat_ptr*_tmp3A8=(*_tmp26A->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3A9,_tmp3A8,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));
# 1035
_tmp26B->tag=3U,_tmp26B->f1=_tmp2B3,_tmp26B->f2=_tmp26A;_tmp26B;}));});
# 1033
_tmp269->ordinaries=_tmp3AA;});}}
# 1037
goto _LL0;};};case 6U: _LL11: _tmp2B4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL12:
# 1041
{struct _handler_cons _tmp26E;_push_handler(& _tmp26E);{int _tmp270=0;if(setjmp(_tmp26E.handler))_tmp270=1;if(!_tmp270){
{struct Cyc_List_List*_tmp271=((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp2B4->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp272=(struct Cyc_Absyn_Datatypedecl*)_tmp271->hd;
if(!_tmp272->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->tag=Cyc_Binding_BindingError;_tmp273;}));
_tmp2B4->name=_tmp272->name;
_tmp2B4->is_extensible=1;{
struct _tuple0*_tmp274=_tmp2B4->name;struct _tuple0*_tmp275=_tmp274;union Cyc_Absyn_Nmspace _tmp279;struct _fat_ptr*_tmp278;_LL33: _tmp279=_tmp275->f1;_tmp278=_tmp275->f2;_LL34:;{
struct Cyc_Binding_ResolveNSEnv*_tmp276=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp279);
({struct Cyc_Dict_Dict _tmp3AD=({struct Cyc_Dict_Dict _tmp3AC=_tmp276->datatypedecls;struct _fat_ptr*_tmp3AB=_tmp278;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp3AC,_tmp3AB,({struct Cyc_List_List*_tmp277=_cycalloc(sizeof(*_tmp277));
_tmp277->hd=_tmp2B4,_tmp277->tl=_tmp271;_tmp277;}));});
# 1050
_tmp276->datatypedecls=_tmp3AD;});};};}
# 1042
;_pop_handler();}else{void*_tmp26F=(void*)Cyc_Core_get_exn_thrown();void*_tmp27A=_tmp26F;void*_tmp281;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp27A)->tag == Cyc_Binding_BindingError){_LL36: _LL37:
# 1054
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp2B4->name,_tmp2B4->sc);{
struct _tuple0*_tmp27B=_tmp2B4->name;struct _tuple0*_tmp27C=_tmp27B;union Cyc_Absyn_Nmspace _tmp280;struct _fat_ptr*_tmp27F;_LL3B: _tmp280=_tmp27C->f1;_tmp27F=_tmp27C->f2;_LL3C:;{
struct Cyc_Binding_ResolveNSEnv*_tmp27D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp280);
# 1058
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(_tmp27D->datatypedecls,_tmp27F)&& _tmp2B4->fields == 0)
# 1060
goto _LL35;
({struct Cyc_Dict_Dict _tmp3B0=({struct Cyc_Dict_Dict _tmp3AF=_tmp27D->datatypedecls;struct _fat_ptr*_tmp3AE=_tmp27F;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp3AF,_tmp3AE,({struct Cyc_List_List*_tmp27E=_cycalloc(sizeof(*_tmp27E));
_tmp27E->hd=_tmp2B4,_tmp27E->tl=0;_tmp27E;}));});
# 1061
_tmp27D->datatypedecls=_tmp3B0;});
# 1063
goto _LL35;};};}else{_LL38: _tmp281=_tmp27A;_LL39:(int)_rethrow(_tmp281);}_LL35:;}};}{
# 1066
struct _tuple0*_tmp282=_tmp2B4->name;struct _tuple0*_tmp283=_tmp282;union Cyc_Absyn_Nmspace _tmp291;struct _fat_ptr*_tmp290;_LL3E: _tmp291=_tmp283->f1;_tmp290=_tmp283->f2;_LL3F:;
if(_tmp2B4->fields != 0){
struct Cyc_List_List*_tmp284=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B4->fields))->v;for(0;_tmp284 != 0;_tmp284=_tmp284->tl){
struct Cyc_Absyn_Datatypefield*_tmp285=(struct Cyc_Absyn_Datatypefield*)_tmp284->hd;
{struct Cyc_List_List*_tmp286=_tmp285->typs;for(0;_tmp286 != 0;_tmp286=_tmp286->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)_tmp286->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp287=(*_tmp285->name).f1;union Cyc_Absyn_Nmspace _tmp288=_tmp287;switch((_tmp288.Abs_n).tag){case 1U: if((_tmp288.Rel_n).val == 0){_LL41: _LL42:
# 1074
 if(_tmp2B4->is_extensible)
({union Cyc_Absyn_Nmspace _tmp3B1=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*_tmp285->name).f1=_tmp3B1;});else{
# 1077
(*_tmp285->name).f1=(*_tmp2B4->name).f1;}
goto _LL40;}else{_LL43: _LL44:
# 1080
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE;_tmp2EE.tag=0U,({struct _fat_ptr _tmp3B2=({const char*_tmp28B="qualified datatype field declaratations not allowed";_tag_fat(_tmp28B,sizeof(char),52U);});_tmp2EE.f1=_tmp3B2;});_tmp2EE;});void*_tmp289[1U];_tmp289[0]=& _tmp28A;({unsigned _tmp3B3=_tmp285->loc;Cyc_Warn_err2(_tmp3B3,_tag_fat(_tmp289,sizeof(void*),1U));});});
return;}case 2U: _LL45: _LL46:
 goto _LL40;default: _LL47: _LL48:
({void*_tmp28C=0U;({struct _fat_ptr _tmp3B4=({const char*_tmp28D="datatype field Loc_n or C_n";_tag_fat(_tmp28D,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp3B4,_tag_fat(_tmp28C,sizeof(void*),0U));});});}_LL40:;}{
# 1085
struct Cyc_Binding_ResolveNSEnv*_tmp28E=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp285->name).f1);
({struct Cyc_Dict_Dict _tmp3B7=({struct Cyc_Dict_Dict _tmp3B6=_tmp28E->ordinaries;struct _fat_ptr*_tmp3B5=(*_tmp285->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3B6,_tmp3B5,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));
# 1088
_tmp28F->tag=2U,_tmp28F->f1=_tmp2B4,_tmp28F->f2=_tmp285;_tmp28F;}));});
# 1086
_tmp28E->ordinaries=_tmp3B7;});};}}
# 1090
goto _LL0;};case 9U: _LL13: _tmp2B6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp2B5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp238)->f2;_LL14:
# 1093
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp2B6,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp2B5);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL0;case 10U: _LL15: _tmp2B8=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp2B7=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp238)->f2;_LL16:
# 1098
((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp2B8);
Cyc_Binding_resolve_decls(env,_tmp2B7);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL0;case 11U: _LL17: _tmp2B9=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_LL18: {
# 1104
int _tmp292=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2B9);
env->in_cinclude=_tmp292;
goto _LL0;}case 12U: _LL19: _tmp2BC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp238)->f1;_tmp2BB=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp238)->f2;_tmp2BA=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp238)->f3;_LL1A:
# 1115
{struct Cyc_List_List*_tmp293=_tmp2BA;for(0;_tmp293 != 0;_tmp293=_tmp293->tl){
struct _tuple14*_tmp294=(struct _tuple14*)_tmp293->hd;struct _tuple14*_tmp295=_tmp294;unsigned _tmp297;struct _tuple0*_tmp296;_LL4A: _tmp297=_tmp295->f1;_tmp296=_tmp295->f2;_LL4B:;
Cyc_Binding_absolutize_decl(_tmp297,env,_tmp296,Cyc_Absyn_ExternC);}}
# 1123
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp299=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF;_tmp2EF.tag=0U,({struct _fat_ptr _tmp3B8=({const char*_tmp29A="extern \"C include\" not at toplevel";_tag_fat(_tmp29A,sizeof(char),35U);});_tmp2EF.f1=_tmp3B8;});_tmp2EF;});void*_tmp298[1U];_tmp298[0]=& _tmp299;({unsigned _tmp3B9=loc;Cyc_Warn_err2(_tmp3B9,_tag_fat(_tmp298,sizeof(void*),1U));});});
goto _LL0;}{
# 1127
struct Cyc_Binding_ResolveNSEnv*_tmp29B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp29C=_tmp29B->ordinaries;
int _tmp29D=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp2BC);
Cyc_Binding_resolve_decls(env,_tmp2BB);{
struct Cyc_Dict_Dict _tmp29E=_tmp29B->ordinaries;
struct Cyc_Dict_Dict _tmp29F=_tmp29C;
{struct Cyc_List_List*_tmp2A0=_tmp2BA;for(0;_tmp2A0 != 0;_tmp2A0=_tmp2A0->tl){
struct _tuple14*_tmp2A1=(struct _tuple14*)_tmp2A0->hd;struct _tuple14*_tmp2A2=_tmp2A1;unsigned _tmp2A8;struct _fat_ptr*_tmp2A7;_LL4D: _tmp2A8=_tmp2A2->f1;_tmp2A7=(_tmp2A2->f2)->f2;_LL4E:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(_tmp29E,_tmp2A7)||
((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(_tmp29C,_tmp2A7)&&({
void*_tmp3BA=((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(_tmp29C,_tmp2A7);_tmp3BA == ((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(_tmp29E,_tmp2A7);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1;_tmp2F1.tag=0U,_tmp2F1.f1=*_tmp2A7;_tmp2F1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F0;_tmp2F0.tag=0U,({struct _fat_ptr _tmp3BB=({const char*_tmp2A6=" is exported but not defined";_tag_fat(_tmp2A6,sizeof(char),29U);});_tmp2F0.f1=_tmp3BB;});_tmp2F0;});void*_tmp2A3[2U];_tmp2A3[0]=& _tmp2A4,_tmp2A3[1]=& _tmp2A5;({unsigned _tmp3BC=_tmp2A8;Cyc_Warn_err2(_tmp3BC,_tag_fat(_tmp2A3,sizeof(void*),2U));});});
_tmp29F=({struct Cyc_Dict_Dict _tmp3BE=_tmp29F;struct _fat_ptr*_tmp3BD=_tmp2A7;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3BE,_tmp3BD,((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(_tmp29E,_tmp2A7));});}}
# 1143
_tmp29B->ordinaries=_tmp29F;
env->in_cinclude=_tmp29D;
# 1147
Cyc_Cifc_user_overrides(loc,_tmp2BC,_tmp2BB);
goto _LL0;};};case 13U: _LL1B: _LL1C:
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
struct Cyc_Binding_Env*_tmp2BD=({struct Cyc_Binding_Env*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->in_cinclude=0,({struct Cyc_Binding_NSCtxt*_tmp3BF=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2C0->ns=_tmp3BF;}),_tmp2C0->local_vars=0;_tmp2C0;});
({struct Cyc_Binding_Env*_tmp3C2=_tmp2BD;Cyc_Binding_resolve_decl(_tmp3C2,({struct Cyc_Absyn_Decl*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));({void*_tmp3C1=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp3C0=Cyc_Absyn_exn_tud();_tmp2BE->f1=_tmp3C0;});_tmp2BE;});_tmp2BF->r=_tmp3C1;}),_tmp2BF->loc=0U;_tmp2BF;}));});
Cyc_Binding_resolve_decls(_tmp2BD,tds);}

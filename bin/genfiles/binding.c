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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172 "list.h"
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 101
extern struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*)(void*,void*),void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 149 "dict.h"
extern void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);
# 257
extern struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 283
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 296
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 303
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 663 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 844
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 846
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 851
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 854
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 889
extern void*Cyc_Absyn_heap_rgn_type;
# 893
extern void*Cyc_Absyn_false_type;
# 895
extern void*Cyc_Absyn_void_type;
# 911
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 938
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1058
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1067
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 37 "flags.h"
extern int Cyc_Flags_warn_override;
# 78
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 45 "binding.h"
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,void*,void*(*)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};char Cyc_Binding_BindingError[13U]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 57 "binding.cyc"
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);void*data=_tmp0;
return({struct Cyc_Binding_NSCtxt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->curr_ns=0,({
struct Cyc_List_List*_tmp26C=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({void*_tmp26B=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0,_tmp1->f1=0;_tmp1;});_tmp2->hd=_tmp26B;}),_tmp2->tl=0;_tmp2;});_tmp3->availables=_tmp26C;}),({
struct Cyc_Dict_Dict _tmp26A=((struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*,void*))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,data);_tmp3->ns_data=_tmp26A;});_tmp3;});}
# 63
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp4=abs_ns;void*_tmp5;switch((_tmp4.Abs_n).tag){case 3: _tmp5=(_tmp4.C_n).val;{struct Cyc_List_List*vs=_tmp5;
_tmp5=vs;goto _LL4;}case 2: _tmp5=(_tmp4.Abs_n).val;_LL4: {struct Cyc_List_List*vs=_tmp5;
return((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,vs);}default:
({void*_tmp6=0U;({struct _fat_ptr _tmp26D=({const char*_tmp7="Binding:get_ns_data: relative ns";_tag_fat(_tmp7,sizeof(char),33U);});Cyc_Warn_impos(_tmp26D,_tag_fat(_tmp6,sizeof(void*),0));});});};}
# 81 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 84
struct Cyc_List_List*_tmp8=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ctxt->curr_ns,rel_ns);struct Cyc_List_List*fullname=_tmp8;
if(((int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member)(ctxt->ns_data,fullname))
return fullname;
{struct Cyc_List_List*_tmp9=ctxt->availables;struct Cyc_List_List*as=_tmp9;for(0;as != 0;as=as->tl){
void*_tmpA=(void*)as->hd;void*_stmttmp0=_tmpA;void*_tmpB=_stmttmp0;void*_tmpC;if(*((int*)_tmpB)== 1){_tmpC=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpB)->f1;{struct Cyc_List_List*ns=_tmpC;
_tmpC=ns;goto _LL4;}}else{_tmpC=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpB)->f1;_LL4: {struct Cyc_List_List*ns=_tmpC;
# 91
struct Cyc_List_List*_tmpD=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ns,rel_ns);struct Cyc_List_List*fullname=_tmpD;
if(((int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member)(ctxt->ns_data,fullname))
return fullname;
goto _LL0;}}_LL0:;}}
# 96
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23C;_tmp23C.tag=0,({struct _fat_ptr _tmp26E=({const char*_tmp14="namespace ";_tag_fat(_tmp14,sizeof(char),11U);});_tmp23C.f1=_tmp26E;});_tmp23C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23B;_tmp23B.tag=0,({struct _fat_ptr _tmp270=(struct _fat_ptr)({struct Cyc_List_List*_tmp26F=rel_ns;Cyc_str_sepstr(_tmp26F,({const char*_tmp13="::";_tag_fat(_tmp13,sizeof(char),3U);}));});_tmp23B.f1=_tmp270;});_tmp23B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23A;_tmp23A.tag=0,({struct _fat_ptr _tmp271=({const char*_tmp12=" not found";_tag_fat(_tmp12,sizeof(char),11U);});_tmp23A.f1=_tmp271;});_tmp23A;});void*_tmpE[3];_tmpE[0]=& _tmpF,_tmpE[1]=& _tmp10,_tmpE[2]=& _tmp11;({unsigned _tmp272=loc;Cyc_Warn_err2(_tmp272,_tag_fat(_tmpE,sizeof(void*),3));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=Cyc_Binding_BindingError;_tmp15;}));}
# 101
void*Cyc_Binding_resolve_lookup(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _fat_ptr*)){
struct _tuple0*_tmp16=qv;void*_tmp18;union Cyc_Absyn_Nmspace _tmp17;_tmp17=_tmp16->f1;_tmp18=_tmp16->f2;{union Cyc_Absyn_Nmspace ns=_tmp17;struct _fat_ptr*v=_tmp18;
union Cyc_Absyn_Nmspace _tmp19=ns;void*_tmp1A;switch((_tmp19.Abs_n).tag){case 1: if((_tmp19.Rel_n).val == 0){
# 105
{struct _handler_cons _tmp1B;_push_handler(& _tmp1B);{int _tmp1D=0;if(setjmp(_tmp1B.handler))_tmp1D=1;if(!_tmp1D){{void*_tmp1E=({void*(*_tmp274)(void*,struct _fat_ptr*)=lookup;void*_tmp273=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp274(_tmp273,v);});_npop_handler(0);return _tmp1E;};_pop_handler();}else{void*_tmp1C=(void*)Cyc_Core_get_exn_thrown();void*_tmp1F=_tmp1C;void*_tmp20;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1F)->tag == Cyc_Dict_Absent)
goto _LLE;else{_tmp20=_tmp1F;{void*exn=_tmp20;(int)_rethrow(exn);}}_LLE:;}}}
{struct Cyc_List_List*_tmp21=ctxt->availables;struct Cyc_List_List*as=_tmp21;for(0;as != 0;as=as->tl){
void*_tmp22=(void*)as->hd;void*_stmttmp1=_tmp22;void*_tmp23=_stmttmp1;void*_tmp24;if(*((int*)_tmp23)== 1){_tmp24=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp23)->f1;{struct Cyc_List_List*ns=_tmp24;
_tmp24=ns;goto _LL17;}}else{_tmp24=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp23)->f1;_LL17: {struct Cyc_List_List*ns=_tmp24;
# 111
{struct _handler_cons _tmp25;_push_handler(& _tmp25);{int _tmp27=0;if(setjmp(_tmp25.handler))_tmp27=1;if(!_tmp27){{void*_tmp28=({void*(*_tmp276)(void*,struct _fat_ptr*)=lookup;void*_tmp275=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,ns);_tmp276(_tmp275,v);});_npop_handler(0);return _tmp28;};_pop_handler();}else{void*_tmp26=(void*)Cyc_Core_get_exn_thrown();void*_tmp29=_tmp26;void*_tmp2A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp29)->tag == Cyc_Dict_Absent)
goto _LL18;else{_tmp2A=_tmp29;{void*exn=_tmp2A;(int)_rethrow(exn);}}_LL18:;}}}
goto _LL13;}}_LL13:;}}
# 115
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=Cyc_Binding_BindingError;_tmp2B;}));}else{_tmp1A=(_tmp19.Rel_n).val;{struct Cyc_List_List*ns=_tmp1A;
# 117
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct Cyc_List_List*_tmp2F=Cyc_Binding_resolve_rel_ns(loc,ctxt,ns);struct Cyc_List_List*abs_ns=_tmp2F;
void*_tmp30=({void*(*_tmp278)(void*,struct _fat_ptr*)=lookup;void*_tmp277=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,abs_ns);_tmp278(_tmp277,v);});_npop_handler(0);return _tmp30;}
# 118
;_pop_handler();}else{void*_tmp2D=(void*)Cyc_Core_get_exn_thrown();void*_tmp31=_tmp2D;void*_tmp32;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp31)->tag == Cyc_Dict_Absent)
# 120
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=Cyc_Binding_BindingError;_tmp33;}));else{_tmp32=_tmp31;{void*exn=_tmp32;(int)_rethrow(exn);}};}}}}case 3: _tmp1A=(_tmp19.C_n).val;{struct Cyc_List_List*ns=_tmp1A;
_tmp1A=ns;goto _LLB;}case 2: _tmp1A=(_tmp19.Abs_n).val;_LLB: {struct Cyc_List_List*ns=_tmp1A;
# 123
struct _handler_cons _tmp34;_push_handler(& _tmp34);{int _tmp36=0;if(setjmp(_tmp34.handler))_tmp36=1;if(!_tmp36){{void*_tmp37=({void*(*_tmp27A)(void*,struct _fat_ptr*)=lookup;void*_tmp279=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,ns);_tmp27A(_tmp279,v);});_npop_handler(0);return _tmp37;};_pop_handler();}else{void*_tmp35=(void*)Cyc_Core_get_exn_thrown();void*_tmp38=_tmp35;void*_tmp39;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp38)->tag == Cyc_Dict_Absent)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->tag=Cyc_Binding_BindingError;_tmp3A;}));else{_tmp39=_tmp38;{void*exn=_tmp39;(int)_rethrow(exn);}};}}}default:
({void*_tmp3B=0U;({struct _fat_ptr _tmp27B=({const char*_tmp3C="lookup local in global";_tag_fat(_tmp3C,sizeof(char),23U);});Cyc_Warn_impos(_tmp27B,_tag_fat(_tmp3B,sizeof(void*),0));});});};}}
# 129
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp3D=ctxt->curr_ns;struct Cyc_List_List*ns=_tmp3D;
struct Cyc_List_List*_tmp3E=({struct Cyc_List_List*_tmp27C=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp27C,({struct Cyc_List_List*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->hd=subname,_tmp41->tl=0;_tmp41;}));});struct Cyc_List_List*ns2=_tmp3E;
if(!((int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member)(ctxt->ns_data,ns2))
({struct Cyc_Dict_Dict _tmp27F=({struct Cyc_Dict_Dict _tmp27E=ctxt->ns_data;struct Cyc_List_List*_tmp27D=ns2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*,void*))Cyc_Dict_insert)(_tmp27E,_tmp27D,mkdata(env));});ctxt->ns_data=_tmp27F;});
ctxt->curr_ns=ns2;
({struct Cyc_List_List*_tmp281=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));({void*_tmp280=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->tag=0,_tmp3F->f1=ns2;_tmp3F;});_tmp40->hd=_tmp280;}),_tmp40->tl=ctxt->availables;_tmp40;});ctxt->availables=_tmp281;});}
# 137
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp42=0U;({struct _fat_ptr _tmp282=({const char*_tmp43="leaving topmost namespace";_tag_fat(_tmp43,sizeof(char),26U);});Cyc_Warn_impos(_tmp282,_tag_fat(_tmp42,sizeof(void*),0));});});{
void*_tmp44=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_stmttmp2=_tmp44;void*_tmp45=_stmttmp2;if(*((int*)_tmp45)== 0){
# 142
ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp283=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp283;});
goto _LL0;}else{
# 146
({void*_tmp46=0U;({struct _fat_ptr _tmp284=({const char*_tmp47="leaving using as namespace";_tag_fat(_tmp47,sizeof(char),27U);});Cyc_Warn_impos(_tmp284,_tag_fat(_tmp46,sizeof(void*),0));});});}_LL0:;}}
# 149
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp48=usename;void*_tmp4A;union Cyc_Absyn_Nmspace _tmp49;_tmp49=_tmp48->f1;_tmp4A=_tmp48->f2;{union Cyc_Absyn_Nmspace nsl=_tmp49;struct _fat_ptr*v=_tmp4A;
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp4B=nsl;void*_tmp4C;switch((_tmp4B.Loc_n).tag){case 1: _tmp4C=(_tmp4B.Rel_n).val;{struct Cyc_List_List*vs=_tmp4C;
# 154
ns=({unsigned _tmp287=loc;struct Cyc_Binding_NSCtxt*_tmp286=ctxt;Cyc_Binding_resolve_rel_ns(_tmp287,_tmp286,({struct Cyc_List_List*_tmp285=vs;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp285,({struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->hd=v,_tmp4D->tl=0;_tmp4D;}));}));});{
struct Cyc_List_List*_tmp4E=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(ns)))->tl);struct Cyc_List_List*abs_vs=_tmp4E;
({union Cyc_Absyn_Nmspace _tmp288=Cyc_Absyn_Abs_n(abs_vs,0);(*usename).f1=_tmp288;});
goto _LL3;}}case 2: _tmp4C=(_tmp4B.Abs_n).val;{struct Cyc_List_List*vs=_tmp4C;
# 159
ns=({struct Cyc_List_List*_tmp289=vs;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp289,({struct Cyc_List_List*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->hd=v,_tmp4F->tl=0;_tmp4F;}));});
goto _LL3;}case 4:
({void*_tmp50=0U;({struct _fat_ptr _tmp28A=({const char*_tmp51="enter_using, Loc_n";_tag_fat(_tmp51,sizeof(char),19U);});Cyc_Warn_impos(_tmp28A,_tag_fat(_tmp50,sizeof(void*),0));});});default:
({void*_tmp52=0U;({struct _fat_ptr _tmp28B=({const char*_tmp53="enter_using, C_n";_tag_fat(_tmp53,sizeof(char),17U);});Cyc_Warn_impos(_tmp28B,_tag_fat(_tmp52,sizeof(void*),0));});});}_LL3:;}
# 164
({struct Cyc_List_List*_tmp28D=({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));({void*_tmp28C=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=1,_tmp54->f1=ns;_tmp54;});_tmp55->hd=_tmp28C;}),_tmp55->tl=ctxt->availables;_tmp55;});ctxt->availables=_tmp28D;});
return ns;}}
# 167
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp56=0U;({struct _fat_ptr _tmp28E=({const char*_tmp57="leaving topmost namespace as a using";_tag_fat(_tmp57,sizeof(char),37U);});Cyc_Warn_impos(_tmp28E,_tag_fat(_tmp56,sizeof(void*),0));});});{
void*_tmp58=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_stmttmp3=_tmp58;void*_tmp59=_stmttmp3;if(*((int*)_tmp59)== 1){
ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL0;}else{
({void*_tmp5A=0U;({struct _fat_ptr _tmp28F=({const char*_tmp5B="leaving namespace as using";_tag_fat(_tmp5B,sizeof(char),27U);});Cyc_Warn_impos(_tmp28F,_tag_fat(_tmp5A,sizeof(void*),0));});});}_LL0:;}}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;int in_cyc_override;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 212 "binding.cyc"
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 215
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 218
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp5C=_cycalloc(sizeof(*_tmp5C));({struct Cyc_Dict_Dict _tmp294=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5C->aggrdecls=_tmp294;}),({
struct Cyc_Dict_Dict _tmp293=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5C->datatypedecls=_tmp293;}),({
struct Cyc_Dict_Dict _tmp292=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5C->enumdecls=_tmp292;}),({
struct Cyc_Dict_Dict _tmp291=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5C->typedefs=_tmp291;}),({
struct Cyc_Dict_Dict _tmp290=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5C->ordinaries=_tmp290;});_tmp5C;});}
# 226
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(renv->aggrdecls,v);}
# 229
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(renv->datatypedecls,v);}
# 232
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(renv->enumdecls,v);}
# 235
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(renv->typedefs,v);}
# 238
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(renv->ordinaries,v);}
# 241
static void*Cyc_Binding_lookup_ordinary(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
struct _tuple0*_tmp5D=qv;void*_tmp5F;union Cyc_Absyn_Nmspace _tmp5E;_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;{union Cyc_Absyn_Nmspace nsl=_tmp5E;struct _fat_ptr*v=_tmp5F;
union Cyc_Absyn_Nmspace _tmp60=nsl;switch((_tmp60.Rel_n).tag){case 4:
# 245
 if(Cyc_Binding_at_toplevel(env)|| !((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=Cyc_Binding_BindingError;_tmp61;}));
return((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v);case 1: if((_tmp60.Rel_n).val == 0){
# 249
if(!Cyc_Binding_at_toplevel(env)&&((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v))
return((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 253
 return((void*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,void*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);};}}
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
static void Cyc_Binding_resolve_function_stuff(unsigned,struct Cyc_Binding_Env*,struct Cyc_Absyn_FnInfo*);
void Cyc_Binding_resolve_asm_iolist(struct Cyc_Binding_Env*,struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int,unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*);
# 273
void Cyc_Binding_absolutize_decl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp62=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp4=_tmp62;union Cyc_Absyn_Nmspace _tmp63=_stmttmp4;switch((_tmp63.Abs_n).tag){case 1: if((_tmp63.Rel_n).val == 0){
# 276
if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _tmp296=({struct Cyc_List_List*_tmp295=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp295,Cyc_Binding_in_cinclude(env)||(int)sc == (int)Cyc_Absyn_ExternC);});(*qv).f1=_tmp296;});else{
# 279
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL0;}else{goto _LL7;}case 4:
 goto _LL0;case 2:
# 283
 if(!Cyc_Binding_at_toplevel(env))
goto _LL8;
goto _LL0;default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp65=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23F;_tmp23F.tag=0,({struct _fat_ptr _tmp297=({const char*_tmp69="qualified names in declarations unimplemented (";_tag_fat(_tmp69,sizeof(char),48U);});_tmp23F.f1=_tmp297;});_tmp23F;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp66=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp23E;_tmp23E.tag=1,_tmp23E.f1=qv;_tmp23E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23D;_tmp23D.tag=0,({struct _fat_ptr _tmp298=({const char*_tmp68=")";_tag_fat(_tmp68,sizeof(char),2U);});_tmp23D.f1=_tmp298;});_tmp23D;});void*_tmp64[3];_tmp64[0]=& _tmp65,_tmp64[1]=& _tmp66,_tmp64[2]=& _tmp67;({unsigned _tmp299=loc;Cyc_Warn_err2(_tmp299,_tag_fat(_tmp64,sizeof(void*),3));});});}_LL0:;}
# 292
void Cyc_Binding_absolutize_topdecl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp6A=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp6A;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=old_locals;}
# 299
void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _fat_ptr hides_what;
struct _handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler))_tmp6D=1;if(!_tmp6D){
{void*_tmp6E=Cyc_Binding_lookup_ordinary(loc,env,q);void*_stmttmp5=_tmp6E;void*_tmp6F=_stmttmp5;void*_tmp70;switch(*((int*)_tmp6F)){case 0: _tmp70=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp6F)->f1;{void*b=_tmp70;
# 304
if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0);return;}
{void*_tmp71=b;switch(*((int*)_tmp71)){case 1:
 hides_what=({const char*_tmp72="global variable";_tag_fat(_tmp72,sizeof(char),16U);});goto _LLB;case 4:
 hides_what=({const char*_tmp73="local variable";_tag_fat(_tmp73,sizeof(char),15U);});goto _LLB;case 3:
 hides_what=({const char*_tmp74="parameter";_tag_fat(_tmp74,sizeof(char),10U);});goto _LLB;case 5:
 hides_what=({const char*_tmp75="pattern variable";_tag_fat(_tmp75,sizeof(char),17U);});goto _LLB;case 2:
 hides_what=({const char*_tmp76="function";_tag_fat(_tmp76,sizeof(char),9U);});goto _LLB;default:
({void*_tmp77=0U;({struct _fat_ptr _tmp29A=({const char*_tmp78="shadowing free variable!";_tag_fat(_tmp78,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp29A,_tag_fat(_tmp77,sizeof(void*),0));});});}_LLB:;}
# 314
goto _LL0;}case 1:
 hides_what=({const char*_tmp79="struct constructor";_tag_fat(_tmp79,sizeof(char),19U);});goto _LL0;case 2:
 hides_what=({const char*_tmp7A="datatype constructor";_tag_fat(_tmp7A,sizeof(char),21U);});goto _LL0;case 3:
 goto _LLA;default: _LLA:
 hides_what=({const char*_tmp7B="enum tag";_tag_fat(_tmp7B,sizeof(char),9U);});goto _LL0;}_LL0:;}
# 320
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp241;_tmp241.tag=0,({struct _fat_ptr _tmp29B=({const char*_tmp7F="declaration hides ";_tag_fat(_tmp7F,sizeof(char),19U);});_tmp241.f1=_tmp29B;});_tmp241;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp240;_tmp240.tag=0,_tmp240.f1=hides_what;_tmp240;});void*_tmp7C[2];_tmp7C[0]=& _tmp7D,_tmp7C[1]=& _tmp7E;({unsigned _tmp29C=loc;Cyc_Warn_warn2(_tmp29C,_tag_fat(_tmp7C,sizeof(void*),2));});});
_npop_handler(0);return;
# 302
;_pop_handler();}else{void*_tmp6C=(void*)Cyc_Core_get_exn_thrown();void*_tmp80=_tmp6C;void*_tmp81;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp80)->tag == Cyc_Binding_BindingError)
# 322
return;else{_tmp81=_tmp80;{void*exn=_tmp81;(int)_rethrow(exn);}};}}}
# 325
void Cyc_Binding_resolve_and_add_vardecl(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp82=vd->name;struct _tuple0*_stmttmp6=_tmp82;struct _tuple0*_tmp83=_stmttmp6;void*_tmp85;union Cyc_Absyn_Nmspace _tmp84;_tmp84=_tmp83->f1;_tmp85=_tmp83->f2;{union Cyc_Absyn_Nmspace decl_ns=_tmp84;struct _fat_ptr*decl_name=_tmp85;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Flags_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp2A0=({struct Cyc_Dict_Dict _tmp29F=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp29E=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp29F,_tmp29E,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp87=_cycalloc(sizeof(*_tmp87));
_tmp87->tag=0,({void*_tmp29D=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=4,_tmp86->f1=vd;_tmp86;});_tmp87->f1=_tmp29D;});_tmp87;}));});
# 332
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp2A0;});else{
# 334
if(!env->in_cyc_override){
struct Cyc_Binding_ResolveNSEnv*_tmp88=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp88;
({struct Cyc_Dict_Dict _tmp2A4=({struct Cyc_Dict_Dict _tmp2A3=decl_ns_data->ordinaries;struct _fat_ptr*_tmp2A2=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp2A3,_tmp2A2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));
# 338
_tmp8A->tag=0,({void*_tmp2A1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=1,_tmp89->f1=vd;_tmp89;});_tmp8A->f1=_tmp2A1;});_tmp8A;}));});
# 336
decl_ns_data->ordinaries=_tmp2A4;});}}}}}
# 342
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp8B=s->r;void*_stmttmp7=_tmp8B;void*_tmp8C=_stmttmp7;void*_tmp90;void*_tmp8F;void*_tmp8E;void*_tmp8D;switch(*((int*)_tmp8C)){case 0:
 return;case 1: _tmp8D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;{struct Cyc_Absyn_Exp*e=_tmp8D;
Cyc_Binding_resolve_exp(env,e);return;}case 2: _tmp8D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;_tmp8E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp8D;struct Cyc_Absyn_Stmt*s2=_tmp8E;
Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_stmt(env,s2);return;}case 3: _tmp8D=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp8D;
if((unsigned)eopt)Cyc_Binding_resolve_exp(env,eopt);return;}case 4: _tmp8D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;_tmp8E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2;_tmp8F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8C)->f3;{struct Cyc_Absyn_Exp*e=_tmp8D;struct Cyc_Absyn_Stmt*s1=_tmp8E;struct Cyc_Absyn_Stmt*s2=_tmp8F;
# 349
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_stmt(env,s2);return;}case 14: _tmp8D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;_tmp8E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2).f1;{struct Cyc_Absyn_Stmt*s1=_tmp8D;struct Cyc_Absyn_Exp*e=_tmp8E;
_tmp8D=e;_tmp8E=s1;goto _LLE;}case 5: _tmp8D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1).f1;_tmp8E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp8D;struct Cyc_Absyn_Stmt*s1=_tmp8E;
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_stmt(env,s1);return;}case 9: _tmp8D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;_tmp8E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2).f1;_tmp8F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8C)->f3).f1;_tmp90=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8C)->f4;{struct Cyc_Absyn_Exp*e1=_tmp8D;struct Cyc_Absyn_Exp*e2=_tmp8E;struct Cyc_Absyn_Exp*e3=_tmp8F;struct Cyc_Absyn_Stmt*s1=_tmp90;
# 353
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);Cyc_Binding_resolve_exp(env,e3);
Cyc_Binding_resolve_stmt(env,s1);
return;}case 6:
 goto _LL14;case 7: _LL14:
 goto _LL16;case 8: _LL16:
 return;case 11: _tmp8D=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;{struct Cyc_List_List*es=_tmp8D;
# 360
for(1;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 13: _tmp8D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp8D;
Cyc_Binding_resolve_stmt(env,s1);return;}case 12: _tmp8D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;_tmp8E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2;{struct Cyc_Absyn_Decl*d=_tmp8D;struct Cyc_Absyn_Stmt*s1=_tmp8E;
# 365
struct Cyc_Dict_Dict _tmp91=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmp91;
Cyc_Binding_resolve_decl(env,d);
Cyc_Binding_resolve_stmt(env,s1);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;
return;}case 10: _tmp8D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;_tmp8E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2;{struct Cyc_Absyn_Exp*e=_tmp8D;struct Cyc_List_List*scs=_tmp8E;
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_scs(env,scs);return;}default: _tmp8D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp8C)->f1;_tmp8E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp8C)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp8D;struct Cyc_List_List*scs=_tmp8E;
Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_scs(env,scs);return;}};}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 374
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_stmttmp8=_tmp92;void*_tmp93=_stmttmp8;struct _fat_ptr _tmp95;int _tmp94;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;switch(*((int*)_tmp93)){case 1: _tmp96=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp93)->f1;{void**b=_tmp96;
# 377
void*_tmp9A=*b;void*_stmttmp9=_tmp9A;void*_tmp9B=_stmttmp9;void*_tmp9C;if(*((int*)_tmp9B)== 0){_tmp9C=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp9B)->f1;{struct _tuple0*q=_tmp9C;
# 379
struct _handler_cons _tmp9D;_push_handler(& _tmp9D);{int _tmp9F=0;if(setjmp(_tmp9D.handler))_tmp9F=1;if(!_tmp9F){{void*_tmpA0=Cyc_Binding_lookup_ordinary(e->loc,env,q);void*_stmttmpA=_tmpA0;void*_tmpA1=_stmttmpA;void*_tmpA3;void*_tmpA2;switch(*((int*)_tmpA1)){case 0: _tmpA2=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpA1)->f1;{void*bnd=_tmpA2;
*b=bnd;_npop_handler(0);return;}case 2: _tmpA2=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpA1)->f1;_tmpA3=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpA1)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmpA2;struct Cyc_Absyn_Datatypefield*tuf=_tmpA3;
({void*_tmp2A5=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=31,_tmpA4->f1=0,_tmpA4->f2=tud,_tmpA4->f3=tuf;_tmpA4;});e->r=_tmp2A5;});_npop_handler(0);return;}case 3: _tmpA2=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpA1)->f1;_tmpA3=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpA1)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmpA2;struct Cyc_Absyn_Enumfield*ef=_tmpA3;
({void*_tmp2A6=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=32,_tmpA5->f1=ed,_tmpA5->f2=ef;_tmpA5;});e->r=_tmp2A6;});_npop_handler(0);return;}case 4: _tmpA2=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpA1)->f1;_tmpA3=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpA1)->f2;{void*t=_tmpA2;struct Cyc_Absyn_Enumfield*ef=_tmpA3;
({void*_tmp2A7=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->tag=33,_tmpA6->f1=t,_tmpA6->f2=ef;_tmpA6;});e->r=_tmp2A7;});_npop_handler(0);return;}default: _tmpA2=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpA1)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpA2;
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp243;_tmp243.tag=0,({struct _fat_ptr _tmp2A8=({const char*_tmpAA="bad occurrence of type name ";_tag_fat(_tmpAA,sizeof(char),29U);});_tmp243.f1=_tmp2A8;});_tmp243;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpA9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp242;_tmp242.tag=1,_tmp242.f1=q;_tmp242;});void*_tmpA7[2];_tmpA7[0]=& _tmpA8,_tmpA7[1]=& _tmpA9;({unsigned _tmp2A9=e->loc;Cyc_Warn_err2(_tmp2A9,_tag_fat(_tmpA7,sizeof(void*),2));});});_npop_handler(0);return;}};}
# 379
;_pop_handler();}else{void*_tmp9E=(void*)Cyc_Core_get_exn_thrown();void*_tmpAB=_tmp9E;void*_tmpAC;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpAB)->tag == Cyc_Binding_BindingError){
# 387
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp245;_tmp245.tag=0,({struct _fat_ptr _tmp2AA=({const char*_tmpB0="undeclared identifier ";_tag_fat(_tmpB0,sizeof(char),23U);});_tmp245.f1=_tmp2AA;});_tmp245;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp244;_tmp244.tag=1,_tmp244.f1=q;_tmp244;});void*_tmpAD[2];_tmpAD[0]=& _tmpAE,_tmpAD[1]=& _tmpAF;({unsigned _tmp2AB=e->loc;Cyc_Warn_err2(_tmp2AB,_tag_fat(_tmpAD,sizeof(void*),2));});});return;}else{_tmpAC=_tmpAB;{void*exn=_tmpAC;(int)_rethrow(exn);}};}}}}else{
# 389
return;};}case 10: _tmp96=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_tmp98=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp93)->f4;{struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_List_List*es=_tmp97;int*b=_tmp98;
# 392
*b=1;
# 394
{struct Cyc_List_List*_tmpB1=es;struct Cyc_List_List*es2=_tmpB1;for(0;es2 != 0;es2=es2->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es2->hd);}}{
void*_tmpB2=e1->r;void*_stmttmpB=_tmpB2;void*_tmpB3=_stmttmpB;void*_tmpB4;if(*((int*)_tmpB3)== 1){_tmpB4=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;{void**b=(void**)_tmpB4;
# 398
void*_tmpB5=*b;void*_stmttmpC=_tmpB5;void*_tmpB6=_stmttmpC;void*_tmpB7;if(*((int*)_tmpB6)== 0){_tmpB7=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpB6)->f1;{struct _tuple0*q=_tmpB7;
# 400
struct _handler_cons _tmpB8;_push_handler(& _tmpB8);{int _tmpBA=0;if(setjmp(_tmpB8.handler))_tmpBA=1;if(!_tmpBA){{void*_tmpBB=Cyc_Binding_lookup_ordinary(e1->loc,env,q);void*_stmttmpD=_tmpBB;void*_tmpBC=_stmttmpD;void*_tmpBE;void*_tmpBD;switch(*((int*)_tmpBC)){case 0: _tmpBD=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpBC)->f1;{void*bnd=_tmpBD;
# 402
*b=bnd;_npop_handler(0);return;}case 1: _tmpBD=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpBC)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpBD;
# 404
struct Cyc_List_List*_tmpBF=0;struct Cyc_List_List*dles=_tmpBF;
for(1;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmpC1=_cycalloc(sizeof(*_tmpC1));({struct _tuple11*_tmp2AC=({struct _tuple11*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->f1=0,_tmpC0->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpC0;});_tmpC1->hd=_tmp2AC;}),_tmpC1->tl=dles;_tmpC1;});}
({void*_tmp2AE=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->tag=29,_tmpC2->f1=ad->name,_tmpC2->f2=0,({struct Cyc_List_List*_tmp2AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles);_tmpC2->f3=_tmp2AD;}),_tmpC2->f4=ad;_tmpC2;});e->r=_tmp2AE;});
_npop_handler(0);return;}case 2: _tmpBD=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpBC)->f1;_tmpBE=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpBC)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmpBD;struct Cyc_Absyn_Datatypefield*tuf=_tmpBE;
# 410
if(tuf->typs == 0)
# 412
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpC4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp247;_tmp247.tag=1,_tmp247.f1=tuf->name;_tmp247;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp246;_tmp246.tag=0,({struct _fat_ptr _tmp2AF=({const char*_tmpC6=" is a constant, not a function";_tag_fat(_tmpC6,sizeof(char),31U);});_tmp246.f1=_tmp2AF;});_tmp246;});void*_tmpC3[2];_tmpC3[0]=& _tmpC4,_tmpC3[1]=& _tmpC5;({unsigned _tmp2B0=e->loc;Cyc_Warn_err2(_tmp2B0,_tag_fat(_tmpC3,sizeof(void*),2));});});
({void*_tmp2B1=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->tag=31,_tmpC7->f1=es,_tmpC7->f2=tud,_tmpC7->f3=tuf;_tmpC7;});e->r=_tmp2B1;});
_npop_handler(0);return;}case 4:
 goto _LL80;default: _LL80:
# 417
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp249;_tmp249.tag=1,_tmp249.f1=q;_tmp249;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp248;_tmp248.tag=0,({struct _fat_ptr _tmp2B2=({const char*_tmpCB=" is an enum constructor, not a function";_tag_fat(_tmpCB,sizeof(char),40U);});_tmp248.f1=_tmp2B2;});_tmp248;});void*_tmpC8[2];_tmpC8[0]=& _tmpC9,_tmpC8[1]=& _tmpCA;({unsigned _tmp2B3=e->loc;Cyc_Warn_err2(_tmp2B3,_tag_fat(_tmpC8,sizeof(void*),2));});});_npop_handler(0);return;};}
# 400
;_pop_handler();}else{void*_tmpB9=(void*)Cyc_Core_get_exn_thrown();void*_tmpCC=_tmpB9;void*_tmpCD;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpCC)->tag == Cyc_Binding_BindingError){
# 419
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24B;_tmp24B.tag=0,({struct _fat_ptr _tmp2B4=({const char*_tmpD1="undeclared identifier ";_tag_fat(_tmpD1,sizeof(char),23U);});_tmp24B.f1=_tmp2B4;});_tmp24B;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpD0=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp24A;_tmp24A.tag=1,_tmp24A.f1=q;_tmp24A;});void*_tmpCE[2];_tmpCE[0]=& _tmpCF,_tmpCE[1]=& _tmpD0;({unsigned _tmp2B5=e->loc;Cyc_Warn_err2(_tmp2B5,_tag_fat(_tmpCE,sizeof(void*),2));});});return;}else{_tmpCD=_tmpCC;{void*exn=_tmpCD;(int)_rethrow(exn);}};}}}}else{
# 421
return;};}}else{
# 423
Cyc_Binding_resolve_exp(env,e1);return;};}}case 29: _tmp96=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_tmp98=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_tmp99=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f4;{struct _tuple0**tn=_tmp96;struct Cyc_List_List*ts=_tmp97;struct Cyc_List_List*dles=_tmp98;struct Cyc_Absyn_Aggrdecl**adopt=(struct Cyc_Absyn_Aggrdecl**)_tmp99;
# 427
for(1;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}{
struct _handler_cons _tmpD2;_push_handler(& _tmpD2);{int _tmpD4=0;if(setjmp(_tmpD2.handler))_tmpD4=1;if(!_tmpD4){
({struct Cyc_Absyn_Aggrdecl*_tmp2B6=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*tn,Cyc_Binding_lookup_aggrdecl);*adopt=_tmp2B6;});
*tn=((struct Cyc_Absyn_Aggrdecl*)_check_null(*adopt))->name;
_npop_handler(0);return;
# 430
;_pop_handler();}else{void*_tmpD3=(void*)Cyc_Core_get_exn_thrown();void*_tmpD5=_tmpD3;void*_tmpD6;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpD5)->tag == Cyc_Binding_BindingError){
# 434
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24D;_tmp24D.tag=0,({struct _fat_ptr _tmp2B7=({const char*_tmpDA="unbound struct/union name ";_tag_fat(_tmpDA,sizeof(char),27U);});_tmp24D.f1=_tmp2B7;});_tmp24D;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpD9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp24C;_tmp24C.tag=1,_tmp24C.f1=*tn;_tmp24C;});void*_tmpD7[2];_tmpD7[0]=& _tmpD8,_tmpD7[1]=& _tmpD9;({unsigned _tmp2B8=e->loc;Cyc_Warn_err2(_tmp2B8,_tag_fat(_tmpD7,sizeof(void*),2));});});
return;}else{_tmpD6=_tmpD5;{void*exn=_tmpD6;(int)_rethrow(exn);}};}}}}case 27: _tmp96=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_tmp98=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp93)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp96;struct Cyc_Absyn_Exp*e1=_tmp97;struct Cyc_Absyn_Exp*e2=_tmp98;
# 439
Cyc_Binding_resolve_exp(env,e1);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp2BA=({struct Cyc_Dict_Dict*_tmpDB=_cycalloc(sizeof(*_tmpDB));({struct Cyc_Dict_Dict _tmp2B9=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmpDB=_tmp2B9;});_tmpDB;});env->local_vars=_tmp2BA;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
env->local_vars=0;
return;}{
# 447
struct Cyc_Dict_Dict _tmpDC=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmpDC;
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;
return;}}case 36: _tmp96=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{struct Cyc_List_List*dles=_tmp96;
# 455
for(1;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}case 28: _tmp96=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{struct Cyc_Absyn_Exp*e1=_tmp96;void*t=_tmp97;
# 461
Cyc_Binding_resolve_exp(env,e1);
Cyc_Binding_resolve_type(e->loc,env,t);
return;}case 2:
# 465
 goto _LL10;case 0: _LL10:
 return;case 24: _tmp96=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;{struct Cyc_List_List*es=_tmp96;
# 468
_tmp96=es;goto _LL14;}case 3: _tmp96=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL14: {struct Cyc_List_List*es=_tmp96;
# 470
for(1;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 42: _tmp96=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp93)->f1;{struct Cyc_Absyn_Exp*e1=_tmp96;
# 474
_tmp96=e1;goto _LL18;}case 41: _tmp96=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL1A;}case 38: _tmp96=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL1C;}case 12: _tmp96=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL1E;}case 18: _tmp96=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL20;}case 11: _tmp96=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL22;}case 5: _tmp96=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL24;}case 22: _tmp96=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL26;}case 21: _tmp96=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL28;}case 15: _tmp96=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp96;
_tmp96=e1;goto _LL2A;}case 20: _tmp96=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp96;
Cyc_Binding_resolve_exp(env,e1);return;}case 35: _tmp96=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;
# 486
_tmp96=e1;_tmp97=e2;goto _LL2E;}case 9: _tmp96=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;
_tmp96=e1;_tmp97=e2;goto _LL30;}case 4: _tmp96=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;
_tmp96=e1;_tmp97=e2;goto _LL32;}case 23: _tmp96=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;
_tmp96=e1;_tmp97=e2;goto _LL34;}case 7: _tmp96=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;
_tmp96=e1;_tmp97=e2;goto _LL36;}case 8: _tmp96=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);return;}case 34: _tmp96=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp93)->f1).rgn;_tmp97=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp93)->f1).elt_type;_tmp98=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp93)->f1).num_elts;{struct Cyc_Absyn_Exp*eo=_tmp96;void**to=_tmp97;struct Cyc_Absyn_Exp*e1=_tmp98;
# 494
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
if(to != 0)Cyc_Binding_resolve_type(e->loc,env,*to);
Cyc_Binding_resolve_exp(env,e1);
return;}case 16: _tmp96=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{struct Cyc_Absyn_Exp*eo=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;
# 500
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
Cyc_Binding_resolve_exp(env,e2);
return;}case 6: _tmp96=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_tmp98=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp93)->f3;{struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;struct Cyc_Absyn_Exp*e3=_tmp98;
# 505
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);Cyc_Binding_resolve_exp(env,e3);return;}case 13: _tmp96=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_List_List*ts=_tmp97;
# 508
Cyc_Binding_resolve_exp(env,e1);
for(1;ts != 0;ts=ts->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)ts->hd);}
return;}case 14: _tmp96=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{void*t=_tmp96;struct Cyc_Absyn_Exp*e1=_tmp97;
# 514
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_type(e->loc,env,t);return;}case 37: _tmp96=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp93)->f1;{struct Cyc_Absyn_Stmt*s=_tmp96;
# 516
Cyc_Binding_resolve_stmt(env,s);return;}case 39: _tmp96=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp93)->f1;{void*t=_tmp96;
# 518
_tmp96=t;goto _LL46;}case 19: _tmp96=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL46: {void*t=_tmp96;
_tmp96=t;goto _LL48;}case 17: _tmp96=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL48: {void*t=_tmp96;
Cyc_Binding_resolve_type(e->loc,env,t);return;}case 25: _tmp96=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f1)->f3;_tmp97=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{void*t=_tmp96;struct Cyc_List_List*dles=_tmp97;
# 523
Cyc_Binding_resolve_type(e->loc,env,t);
_tmp96=dles;goto _LL4C;}case 26: _tmp96=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL4C: {struct Cyc_List_List*dles=_tmp96;
# 527
for(1;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}case 33: _tmp96=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{void*t=_tmp96;struct Cyc_Absyn_Enumfield*ef=_tmp97;
# 532
Cyc_Binding_resolve_type(e->loc,env,t);return;}case 32: _tmp96=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp96;struct Cyc_Absyn_Enumfield*ef=_tmp97;
return;}case 31: _tmp96=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_tmp98=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp93)->f3;{struct Cyc_List_List*es=_tmp96;struct Cyc_Absyn_Datatypedecl*tud=_tmp97;struct Cyc_Absyn_Datatypefield*tuf=_tmp98;
# 535
for(1;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 30: _tmp96=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp97=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp93)->f2;{void*t=_tmp96;struct Cyc_List_List*dles=_tmp97;
# 539
Cyc_Binding_resolve_type(e->loc,env,t);
for(1;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}default: _tmp94=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_tmp95=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_tmp96=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_tmp97=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp93)->f4;_tmp98=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp93)->f5;{int v=_tmp94;struct _fat_ptr t=_tmp95;struct Cyc_List_List*o=_tmp96;struct Cyc_List_List*i=_tmp97;struct Cyc_List_List*c=_tmp98;
# 545
Cyc_Binding_resolve_asm_iolist(env,o);
Cyc_Binding_resolve_asm_iolist(env,i);
goto _LL0;}}_LL0:;}struct _tuple12{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 551
void Cyc_Binding_resolve_asm_iolist(struct Cyc_Binding_Env*env,struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple12*_tmpDD=(struct _tuple12*)l->hd;struct _tuple12*_stmttmpE=_tmpDD;struct _tuple12*_tmpDE=_stmttmpE;void*_tmpDF;_tmpDF=_tmpDE->f2;{struct Cyc_Absyn_Exp*e=_tmpDF;
Cyc_Binding_resolve_exp(env,e);
l=l->tl;}}}
# 559
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmpE0=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmpE0;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmpE1=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpF=_tmpE1;struct Cyc_Absyn_Switch_clause*_tmpE2=_stmttmpF;void*_tmpE5;void*_tmpE4;void*_tmpE3;_tmpE3=_tmpE2->pattern;_tmpE4=_tmpE2->where_clause;_tmpE5=_tmpE2->body;{struct Cyc_Absyn_Pat*pattern=_tmpE3;struct Cyc_Absyn_Exp*where_clause=_tmpE4;struct Cyc_Absyn_Stmt*body=_tmpE5;
Cyc_Binding_resolve_pat(env,pattern);
if(where_clause != 0)
Cyc_Binding_resolve_exp(env,where_clause);
Cyc_Binding_resolve_stmt(env,body);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;}}
# 569
return;}
# 571
void Cyc_Binding_resolve_aggrfields(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(1;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpE6=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp10=_tmpE6;struct Cyc_Absyn_Aggrfield*_tmpE7=_stmttmp10;void*_tmpED;void*_tmpEC;void*_tmpEB;void*_tmpEA;struct Cyc_Absyn_Tqual _tmpE9;void*_tmpE8;_tmpE8=_tmpE7->name;_tmpE9=_tmpE7->tq;_tmpEA=_tmpE7->type;_tmpEB=_tmpE7->width;_tmpEC=_tmpE7->attributes;_tmpED=_tmpE7->requires_clause;{struct _fat_ptr*fn=_tmpE8;struct Cyc_Absyn_Tqual tq=_tmpE9;void*t=_tmpEA;struct Cyc_Absyn_Exp*width=_tmpEB;struct Cyc_List_List*atts=_tmpEC;struct Cyc_Absyn_Exp*requires_clause=_tmpED;
Cyc_Binding_resolve_type(loc,env,t);
if(width != 0)
Cyc_Binding_resolve_exp(env,width);
if(requires_clause != 0)
Cyc_Binding_resolve_exp(env,requires_clause);}}
# 580
return;}
# 583
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int need_va_name,unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 589
struct Cyc_List_List*_tmpEE=0;struct Cyc_List_List*vds=_tmpEE;
for(1;args != 0;args=args->tl){
struct _tuple8*_tmpEF=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp11=_tmpEF;struct _tuple8*_tmpF0=_stmttmp11;void*_tmpF3;struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF1;_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;_tmpF3=_tmpF0->f3;{struct _fat_ptr*a=_tmpF1;struct Cyc_Absyn_Tqual tq=_tmpF2;void*t=_tmpF3;
if(a == 0)
continue;{
struct Cyc_Absyn_Vardecl*_tmpF4=({struct _tuple0*_tmp2BB=({struct _tuple0*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->f1=Cyc_Absyn_Loc_n,_tmpF6->f2=a;_tmpF6;});Cyc_Absyn_new_vardecl(0U,_tmp2BB,t,0);});struct Cyc_Absyn_Vardecl*vd=_tmpF4;
vd->tq=tq;
vds=({struct Cyc_List_List*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=vd,_tmpF5->tl=vds;_tmpF5;});}}}
# 598
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmpF7=*vai;struct Cyc_Absyn_VarargInfo _stmttmp12=_tmpF7;struct Cyc_Absyn_VarargInfo _tmpF8=_stmttmp12;int _tmpFC;void*_tmpFB;struct Cyc_Absyn_Tqual _tmpFA;void*_tmpF9;_tmpF9=_tmpF8.name;_tmpFA=_tmpF8.tq;_tmpFB=_tmpF8.type;_tmpFC=_tmpF8.inject;{struct _fat_ptr*v=_tmpF9;struct Cyc_Absyn_Tqual tq=_tmpFA;void*t=_tmpFB;int i=_tmpFC;
if(v == 0){
if(need_va_name)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpFE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24E;_tmp24E.tag=0,({struct _fat_ptr _tmp2BC=({const char*_tmpFF="missing name for vararg";_tag_fat(_tmpFF,sizeof(char),24U);});_tmp24E.f1=_tmp2BC;});_tmp24E;});void*_tmpFD[1];_tmpFD[0]=& _tmpFE;({unsigned _tmp2BD=loc;Cyc_Warn_err2(_tmp2BD,_tag_fat(_tmpFD,sizeof(void*),1));});});}else{
# 605
void*_tmp100=Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);void*typ=_tmp100;
struct Cyc_Absyn_Vardecl*_tmp101=({struct _tuple0*_tmp2BE=({struct _tuple0*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->f1=Cyc_Absyn_Loc_n,_tmp103->f2=v;_tmp103;});Cyc_Absyn_new_vardecl(0U,_tmp2BE,typ,0);});struct Cyc_Absyn_Vardecl*vd=_tmp101;
vds=({struct Cyc_List_List*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->hd=vd,_tmp102->tl=vds;_tmp102;});}}}
# 610
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(vds);
return vds;}
# 614
static void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo*i){
if(i->effect != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(i->effect));
Cyc_Binding_resolve_type(loc,env,i->ret_type);
{struct Cyc_List_List*_tmp104=i->args;struct Cyc_List_List*args=_tmp104;for(0;args != 0;args=args->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple8*)args->hd)).f3);}}
if(i->cyc_varargs != 0)
Cyc_Binding_resolve_type(loc,env,((struct Cyc_Absyn_VarargInfo*)_check_null(i->cyc_varargs))->type);
Cyc_Binding_resolve_rgnpo(loc,env,i->rgn_po);
if(i->requires_clause != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(i->requires_clause));
if(i->ensures_clause != 0){
# 627
struct Cyc_Dict_Dict _tmp105=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict locs=_tmp105;
struct _fat_ptr*_tmp106=({struct _fat_ptr*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct _fat_ptr _tmp2BF=({const char*_tmp10B="return_value";_tag_fat(_tmp10B,sizeof(char),13U);});*_tmp10C=_tmp2BF;});_tmp10C;});struct _fat_ptr*v=_tmp106;
struct Cyc_Absyn_Vardecl*_tmp107=({struct _tuple0*_tmp2C0=({struct _tuple0*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->f1=Cyc_Absyn_Loc_n,_tmp10A->f2=v;_tmp10A;});Cyc_Absyn_new_vardecl(0U,_tmp2C0,i->ret_type,0);});struct Cyc_Absyn_Vardecl*vd=_tmp107;
i->return_value=vd;
({struct Cyc_Dict_Dict _tmp2C4=({struct Cyc_Dict_Dict _tmp2C3=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp2C2=v;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp2C3,_tmp2C2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp109=_cycalloc(sizeof(*_tmp109));
_tmp109->tag=0,({void*_tmp2C1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->tag=4,_tmp108->f1=vd;_tmp108;});_tmp109->f1=_tmp2C1;});_tmp109;}));});
# 631
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp2C4;});
# 633
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(i->ensures_clause));
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=locs;}}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
# 643
void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp10D=t;void*_tmp110;void*_tmp10F;void*_tmp10E;switch(*((int*)_tmp10D)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)){case 18: _tmp10E=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;_tmp10F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2;{union Cyc_Absyn_DatatypeInfo*info=_tmp10E;struct Cyc_List_List*targs=_tmp10F;
# 646
for(1;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeInfo _tmp111=*info;union Cyc_Absyn_DatatypeInfo _stmttmp13=_tmp111;union Cyc_Absyn_DatatypeInfo _tmp112=_stmttmp13;int _tmp114;void*_tmp113;if((_tmp112.UnknownDatatype).tag == 1){_tmp113=((_tmp112.UnknownDatatype).val).name;_tmp114=((_tmp112.UnknownDatatype).val).is_extensible;{struct _tuple0*qv=_tmp113;int b=_tmp114;
# 650
struct _handler_cons _tmp115;_push_handler(& _tmp115);{int _tmp117=0;if(setjmp(_tmp115.handler))_tmp117=1;if(!_tmp117){
{struct Cyc_Absyn_Datatypedecl*_tmp118=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_datatypedecl))->hd;struct Cyc_Absyn_Datatypedecl*tud=_tmp118;
({union Cyc_Absyn_DatatypeInfo _tmp2C5=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp24F;_tmp24F.name=tud->name,_tmp24F.is_extensible=b;_tmp24F;}));*info=_tmp2C5;});
_npop_handler(0);return;}
# 651
;_pop_handler();}else{void*_tmp116=(void*)Cyc_Core_get_exn_thrown();void*_tmp119=_tmp116;void*_tmp11A;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp119)->tag == Cyc_Binding_BindingError){
# 655
Cyc_Binding_absolutize_topdecl(loc,env,qv,Cyc_Absyn_Public);return;}else{_tmp11A=_tmp119;{void*exn=_tmp11A;(int)_rethrow(exn);}};}}}}else{
return;};}}case 19: _tmp10E=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;_tmp10F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2;{union Cyc_Absyn_DatatypeFieldInfo*info=_tmp10E;struct Cyc_List_List*targs=_tmp10F;
# 659
for(1;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeFieldInfo _tmp11B=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmp14=_tmp11B;union Cyc_Absyn_DatatypeFieldInfo _tmp11C=_stmttmp14;int _tmp11F;void*_tmp11E;void*_tmp11D;if((_tmp11C.UnknownDatatypefield).tag == 1){_tmp11D=((_tmp11C.UnknownDatatypefield).val).datatype_name;_tmp11E=((_tmp11C.UnknownDatatypefield).val).field_name;_tmp11F=((_tmp11C.UnknownDatatypefield).val).is_extensible;{struct _tuple0*qvd=_tmp11D;struct _tuple0*qvf=_tmp11E;int b=_tmp11F;
# 665
{struct _handler_cons _tmp120;_push_handler(& _tmp120);{int _tmp122=0;if(setjmp(_tmp120.handler))_tmp122=1;if(!_tmp122){
{void*_tmp123=Cyc_Binding_lookup_ordinary(loc,env,qvf);void*_stmttmp15=_tmp123;void*_tmp124=_stmttmp15;void*_tmp126;void*_tmp125;if(*((int*)_tmp124)== 2){_tmp125=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp124)->f1;_tmp126=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp124)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp125;struct Cyc_Absyn_Datatypefield*tuf=_tmp126;
# 668
struct Cyc_Absyn_Datatypedecl*_tmp127=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qvd,Cyc_Binding_lookup_datatypedecl))->hd;struct Cyc_Absyn_Datatypedecl*tud2=_tmp127;
if(Cyc_Absyn_qvar_cmp(tud->name,tud2->name)!= 0){
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp129=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp255;_tmp255.tag=1,_tmp255.f1=tuf->name;_tmp255;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp254;_tmp254.tag=0,({struct _fat_ptr _tmp2C6=({const char*_tmp131=" is a variant of ";_tag_fat(_tmp131,sizeof(char),18U);});_tmp254.f1=_tmp2C6;});_tmp254;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp12B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp253;_tmp253.tag=1,_tmp253.f1=tud2->name;_tmp253;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp252;_tmp252.tag=0,({struct _fat_ptr _tmp2C7=({const char*_tmp130=" not ";_tag_fat(_tmp130,sizeof(char),6U);});_tmp252.f1=_tmp2C7;});_tmp252;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp12D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp251;_tmp251.tag=1,_tmp251.f1=tud->name;_tmp251;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp250;_tmp250.tag=0,({
struct _fat_ptr _tmp2C8=({const char*_tmp12F=" (shadowing not yet implemented properly)";_tag_fat(_tmp12F,sizeof(char),42U);});_tmp250.f1=_tmp2C8;});_tmp250;});void*_tmp128[6];_tmp128[0]=& _tmp129,_tmp128[1]=& _tmp12A,_tmp128[2]=& _tmp12B,_tmp128[3]=& _tmp12C,_tmp128[4]=& _tmp12D,_tmp128[5]=& _tmp12E;({unsigned _tmp2C9=loc;Cyc_Warn_err2(_tmp2C9,_tag_fat(_tmp128,sizeof(void*),6));});});
_npop_handler(0);return;}
# 674
({union Cyc_Absyn_DatatypeFieldInfo _tmp2CA=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp256;_tmp256.datatype_name=tud->name,_tmp256.field_name=tuf->name,_tmp256.is_extensible=b;_tmp256;}));*info=_tmp2CA;});
# 676
_npop_handler(0);return;}}else{
goto _LL34;}_LL34:;}
# 666
;_pop_handler();}else{void*_tmp121=(void*)Cyc_Core_get_exn_thrown();void*_tmp132=_tmp121;void*_tmp133;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp132)->tag == Cyc_Binding_BindingError)
# 679
goto _LL39;else{_tmp133=_tmp132;{void*exn=_tmp133;(int)_rethrow(exn);}}_LL39:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp135=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp258;_tmp258.tag=1,_tmp258.f1=qvf;_tmp258;});struct Cyc_Warn_String_Warn_Warg_struct _tmp136=({struct Cyc_Warn_String_Warn_Warg_struct _tmp257;_tmp257.tag=0,({struct _fat_ptr _tmp2CB=({const char*_tmp137=" is not a datatype field";_tag_fat(_tmp137,sizeof(char),25U);});_tmp257.f1=_tmp2CB;});_tmp257;});void*_tmp134[2];_tmp134[0]=& _tmp135,_tmp134[1]=& _tmp136;({unsigned _tmp2CC=loc;Cyc_Warn_err2(_tmp2CC,_tag_fat(_tmp134,sizeof(void*),2));});});return;}}else{
return;};}}case 20: _tmp10E=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;_tmp10F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2;{union Cyc_Absyn_AggrInfo*info=_tmp10E;struct Cyc_List_List*targs=_tmp10F;
# 685
for(1;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_AggrInfo _tmp138=*info;union Cyc_Absyn_AggrInfo _stmttmp16=_tmp138;union Cyc_Absyn_AggrInfo _tmp139=_stmttmp16;void*_tmp13C;void*_tmp13B;enum Cyc_Absyn_AggrKind _tmp13A;if((_tmp139.UnknownAggr).tag == 1){_tmp13A=((_tmp139.UnknownAggr).val).f1;_tmp13B=((_tmp139.UnknownAggr).val).f2;_tmp13C=((_tmp139.UnknownAggr).val).f3;{enum Cyc_Absyn_AggrKind ak=_tmp13A;struct _tuple0*qv=_tmp13B;struct Cyc_Core_Opt*bo=_tmp13C;
# 689
struct _handler_cons _tmp13D;_push_handler(& _tmp13D);{int _tmp13F=0;if(setjmp(_tmp13D.handler))_tmp13F=1;if(!_tmp13F){
{struct Cyc_Absyn_Aggrdecl*_tmp140=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_aggrdecl);struct Cyc_Absyn_Aggrdecl*ad=_tmp140;
if((int)ad->kind != (int)ak)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp142=({struct Cyc_Warn_String_Warn_Warg_struct _tmp259;_tmp259.tag=0,({struct _fat_ptr _tmp2CD=({const char*_tmp143="struct vs. union mismatch with earlier declaration";_tag_fat(_tmp143,sizeof(char),51U);});_tmp259.f1=_tmp2CD;});_tmp259;});void*_tmp141[1];_tmp141[0]=& _tmp142;({unsigned _tmp2CE=loc;Cyc_Warn_err2(_tmp2CE,_tag_fat(_tmp141,sizeof(void*),1));});});
if((((int)ak == (int)1U && bo != 0)&& ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged != (int)bo->v)
# 695
({struct Cyc_Warn_String_Warn_Warg_struct _tmp145=({struct Cyc_Warn_String_Warn_Warg_struct _tmp25A;_tmp25A.tag=0,({struct _fat_ptr _tmp2CF=({const char*_tmp146="@tagged mismatch with earlier declaration";_tag_fat(_tmp146,sizeof(char),42U);});_tmp25A.f1=_tmp2CF;});_tmp25A;});void*_tmp144[1];_tmp144[0]=& _tmp145;({unsigned _tmp2D0=loc;Cyc_Warn_err2(_tmp2D0,_tag_fat(_tmp144,sizeof(void*),1));});});
({union Cyc_Absyn_AggrInfo _tmp2D1=Cyc_Absyn_UnknownAggr(ak,ad->name,bo);*info=_tmp2D1;});
_npop_handler(0);return;}
# 690
;_pop_handler();}else{void*_tmp13E=(void*)Cyc_Core_get_exn_thrown();void*_tmp147=_tmp13E;void*_tmp148;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp147)->tag == Cyc_Binding_BindingError){
# 699
Cyc_Binding_absolutize_topdecl(loc,env,qv,Cyc_Absyn_Public);return;}else{_tmp148=_tmp147;{void*exn=_tmp148;(int)_rethrow(exn);}};}}}}else{
return;};}}case 15: _tmp10E=(struct _tuple0**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;_tmp10F=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f2;{struct _tuple0**qv=_tmp10E;struct Cyc_Absyn_Enumdecl*edo=_tmp10F;
# 703
if(edo != 0)
return;{
struct _handler_cons _tmp149;_push_handler(& _tmp149);{int _tmp14B=0;if(setjmp(_tmp149.handler))_tmp14B=1;if(!_tmp14B){
{struct Cyc_Absyn_Enumdecl*_tmp14C=((struct Cyc_Absyn_Enumdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*qv,Cyc_Binding_lookup_enumdecl);struct Cyc_Absyn_Enumdecl*ed=_tmp14C;
*qv=ed->name;
_npop_handler(0);return;}
# 706
;_pop_handler();}else{void*_tmp14A=(void*)Cyc_Core_get_exn_thrown();void*_tmp14D=_tmp14A;void*_tmp14E;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp14D)->tag == Cyc_Binding_BindingError){
# 710
Cyc_Binding_absolutize_topdecl(loc,env,*qv,Cyc_Absyn_Public);return;}else{_tmp14E=_tmp14D;{void*exn=_tmp14E;(int)_rethrow(exn);}};}}}}case 16: _tmp10E=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;{struct Cyc_List_List*fs=_tmp10E;
# 748
struct Cyc_Binding_ResolveNSEnv*_tmp163=({struct Cyc_Binding_NSCtxt*_tmp2D3=env->ns;((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(_tmp2D3,({struct Cyc_List_List*_tmp2D2=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp2D2,
Cyc_Binding_in_cinclude(env));}));});
# 748
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp163;
# 750
for(1;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp164=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp164;
if(f->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
({struct Cyc_Dict_Dict _tmp2D6=({struct Cyc_Dict_Dict _tmp2D5=decl_ns_data->ordinaries;struct _fat_ptr*_tmp2D4=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp2D5,_tmp2D4,(void*)({struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=4,_tmp165->f1=t,_tmp165->f2=f;_tmp165;}));});decl_ns_data->ordinaries=_tmp2D6;});}
# 756
return;}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2 == 0){_LL16:
# 760
 return;}else{_tmp10E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1;_tmp10F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2;{void*c=_tmp10E;struct Cyc_List_List*ts=_tmp10F;
# 762
for(1;ts != 0;ts=ts->tl){
Cyc_Binding_resolve_type(loc,env,(void*)ts->hd);}
return;}}}case 8: _tmp10E=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp10D)->f1;_tmp10F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp10D)->f2;_tmp110=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp10D)->f3;{struct _tuple0**tdn=_tmp10E;struct Cyc_List_List*targs=_tmp10F;struct Cyc_Absyn_Typedefdecl**tdo=_tmp110;
# 713
for(1;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}
{struct _handler_cons _tmp14F;_push_handler(& _tmp14F);{int _tmp151=0;if(setjmp(_tmp14F.handler))_tmp151=1;if(!_tmp151){
{struct Cyc_Absyn_Typedefdecl*_tmp152=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*tdn,Cyc_Binding_lookup_typedefdecl);struct Cyc_Absyn_Typedefdecl*td=_tmp152;
# 718
*tdn=td->name;
_npop_handler(0);return;}
# 716
;_pop_handler();}else{void*_tmp150=(void*)Cyc_Core_get_exn_thrown();void*_tmp153=_tmp150;void*_tmp154;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp153)->tag == Cyc_Binding_BindingError)
# 720
goto _LL4D;else{_tmp154=_tmp153;{void*exn=_tmp154;(int)_rethrow(exn);}}_LL4D:;}}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp156=({struct Cyc_Warn_String_Warn_Warg_struct _tmp25C;_tmp25C.tag=0,({struct _fat_ptr _tmp2D7=({const char*_tmp158="unbound typdef name ";_tag_fat(_tmp158,sizeof(char),21U);});_tmp25C.f1=_tmp2D7;});_tmp25C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp157=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp25B;_tmp25B.tag=1,_tmp25B.f1=*tdn;_tmp25B;});void*_tmp155[2];_tmp155[0]=& _tmp156,_tmp155[1]=& _tmp157;({unsigned _tmp2D8=loc;Cyc_Warn_err2(_tmp2D8,_tag_fat(_tmp155,sizeof(void*),2));});});
return;}case 10: _tmp10E=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1;_tmp10F=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f2;{struct Cyc_Absyn_TypeDecl*td=_tmp10E;void***to=_tmp10F;
# 726
struct Cyc_Dict_Dict*_tmp159=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp159;
env->local_vars=0;
{void*_tmp15A=td->r;void*_stmttmp17=_tmp15A;void*_tmp15B=_stmttmp17;void*_tmp15C;switch(*((int*)_tmp15B)){case 0: _tmp15C=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp15B)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp15C;
# 730
({struct Cyc_Binding_Env*_tmp2DA=env;Cyc_Binding_resolve_decl(_tmp2DA,({struct Cyc_Absyn_Decl*_tmp15E=_cycalloc(sizeof(*_tmp15E));({void*_tmp2D9=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->tag=5,_tmp15D->f1=ad;_tmp15D;});_tmp15E->r=_tmp2D9;}),_tmp15E->loc=td->loc;_tmp15E;}));});goto _LL52;}case 1: _tmp15C=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp15B)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp15C;
# 732
({struct Cyc_Binding_Env*_tmp2DC=env;Cyc_Binding_resolve_decl(_tmp2DC,({struct Cyc_Absyn_Decl*_tmp160=_cycalloc(sizeof(*_tmp160));({void*_tmp2DB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->tag=7,_tmp15F->f1=ed;_tmp15F;});_tmp160->r=_tmp2DB;}),_tmp160->loc=td->loc;_tmp160;}));});goto _LL52;}default: _tmp15C=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp15B)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_tmp15C;
# 734
({struct Cyc_Binding_Env*_tmp2DE=env;Cyc_Binding_resolve_decl(_tmp2DE,({struct Cyc_Absyn_Decl*_tmp162=_cycalloc(sizeof(*_tmp162));({void*_tmp2DD=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->tag=6,_tmp161->f1=tud;_tmp161;});_tmp162->r=_tmp2DD;}),_tmp162->loc=td->loc;_tmp162;}));});goto _LL52;}}_LL52:;}
# 736
env->local_vars=old_locals;
return;}case 7: _tmp10E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10D)->f2;{struct Cyc_List_List*fs=_tmp10E;
# 743
Cyc_Binding_resolve_aggrfields(loc,env,fs);
return;}case 1: _tmp10E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp10D)->f2;{void*to=_tmp10E;
# 757
if(to != 0)Cyc_Binding_resolve_type(loc,env,to);return;}case 2:
# 759
 goto _LL16;case 3: _tmp10E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10D)->f1).elt_type;_tmp10F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10D)->f1).ptr_atts).rgn;_tmp110=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10D)->f1).ptr_atts).bounds;{void*t1=_tmp10E;void*t2=_tmp10F;void*bds=_tmp110;
# 767
Cyc_Binding_resolve_type(loc,env,t1);
Cyc_Binding_resolve_type(loc,env,t2);
Cyc_Binding_resolve_type(loc,env,bds);
return;}case 6: _tmp10E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp10D)->f1;{struct Cyc_List_List*tqts=_tmp10E;
# 773
for(1;tqts != 0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)tqts->hd)).f2);}
return;}case 4: _tmp10E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp10D)->f1).elt_type;_tmp10F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp10D)->f1).num_elts;{void*t1=_tmp10E;struct Cyc_Absyn_Exp*eo=_tmp10F;
# 777
Cyc_Binding_resolve_type(loc,env,t1);
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
return;}case 5: _tmp10E=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp10D)->f1;{struct Cyc_Absyn_FnInfo*i=_tmp10E;
# 784
struct Cyc_List_List*_tmp166=Cyc_Binding_get_fun_vardecls(0,loc,env,i->args,i->cyc_varargs);struct Cyc_List_List*vds=_tmp166;
# 786
struct Cyc_Dict_Dict*_tmp167=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp167;
if(old_locals != 0)
({struct Cyc_Dict_Dict*_tmp2DF=({struct Cyc_Dict_Dict*_tmp168=_cycalloc(sizeof(*_tmp168));*_tmp168=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp168;});env->local_vars=_tmp2DF;});else{
# 790
({struct Cyc_Dict_Dict*_tmp2E1=({struct Cyc_Dict_Dict*_tmp169=_cycalloc(sizeof(*_tmp169));({struct Cyc_Dict_Dict _tmp2E0=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp169=_tmp2E0;});_tmp169;});env->local_vars=_tmp2E1;});}
{struct Cyc_List_List*_tmp16A=vds;struct Cyc_List_List*vds1=_tmp16A;for(0;vds1 != 0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _tmp2E5=({struct Cyc_Dict_Dict _tmp2E4=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp2E3=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp2E4,_tmp2E3,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));
_tmp16C->tag=0,({void*_tmp2E2=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=3,_tmp16B->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_tmp16B;});_tmp16C->f1=_tmp2E2;});_tmp16C;}));});
# 792
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp2E5;});}}
# 795
Cyc_Binding_resolve_function_stuff(loc,env,i);
env->local_vars=old_locals;
return;}case 9: _tmp10E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp10D)->f1;{struct Cyc_Absyn_Exp*e=_tmp10E;
# 799
_tmp10E=e;goto _LL24;}default: _tmp10E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp10D)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp10E;
Cyc_Binding_resolve_exp(env,e);return;}};}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 804
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp16D=p;unsigned _tmp16F;void*_tmp16E;_tmp16E=_tmp16D->r;_tmp16F=_tmp16D->loc;{void*r=_tmp16E;unsigned loc=_tmp16F;
void*_tmp170=r;void*_tmp173;int _tmp174;void*_tmp172;void*_tmp171;switch(*((int*)_tmp170)){case 15: _tmp171=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp170)->f1;{struct _tuple0*qv=_tmp171;
# 808
{struct _handler_cons _tmp175;_push_handler(& _tmp175);{int _tmp177=0;if(setjmp(_tmp175.handler))_tmp177=1;if(!_tmp177){{void*_tmp178=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_stmttmp18=_tmp178;void*_tmp179=_stmttmp18;void*_tmp17B;void*_tmp17A;switch(*((int*)_tmp179)){case 0:
 goto _LL2C;case 1:
# 811
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp25D;_tmp25D.tag=0,({struct _fat_ptr _tmp2E6=({const char*_tmp17E="struct tag used without arguments in pattern";_tag_fat(_tmp17E,sizeof(char),45U);});_tmp25D.f1=_tmp2E6;});_tmp25D;});void*_tmp17C[1];_tmp17C[0]=& _tmp17D;({unsigned _tmp2E7=loc;Cyc_Warn_err2(_tmp2E7,_tag_fat(_tmp17C,sizeof(void*),1));});});_npop_handler(0);return;case 2: _tmp17A=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp179)->f1;_tmp17B=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp179)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp17A;struct Cyc_Absyn_Datatypefield*tuf=_tmp17B;
# 813
({void*_tmp2E8=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->tag=8,_tmp17F->f1=tud,_tmp17F->f2=tuf,_tmp17F->f3=0,_tmp17F->f4=0;_tmp17F;});p->r=_tmp2E8;});_npop_handler(0);return;}case 3: _tmp17A=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp179)->f1;_tmp17B=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp179)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp17A;struct Cyc_Absyn_Enumfield*ef=_tmp17B;
# 815
({void*_tmp2E9=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->tag=13,_tmp180->f1=ed,_tmp180->f2=ef;_tmp180;});p->r=_tmp2E9;});_npop_handler(0);return;}default: _tmp17A=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp179)->f1;_tmp17B=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp179)->f2;{void*t=_tmp17A;struct Cyc_Absyn_Enumfield*ef=_tmp17B;
# 817
({void*_tmp2EA=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->tag=14,_tmp181->f1=t,_tmp181->f2=ef;_tmp181;});p->r=_tmp2EA;});_npop_handler(0);return;}}_LL2C:;}
# 808
;_pop_handler();}else{void*_tmp176=(void*)Cyc_Core_get_exn_thrown();void*_tmp182=_tmp176;void*_tmp183;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp182)->tag == Cyc_Binding_BindingError)
# 818
goto _LL37;else{_tmp183=_tmp182;{void*exn=_tmp183;(int)_rethrow(exn);}}_LL37:;}}}{
struct Cyc_Absyn_Vardecl*_tmp184=Cyc_Absyn_new_vardecl(0U,qv,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmp184;
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
({void*_tmp2EC=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->tag=1,_tmp185->f1=vd,({struct Cyc_Absyn_Pat*_tmp2EB=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp185->f2=_tmp2EB;});_tmp185;});p->r=_tmp2EC;});
return;}}case 16: _tmp171=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp170)->f1;_tmp172=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp170)->f2;_tmp174=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp170)->f3;{struct _tuple0*qv=_tmp171;struct Cyc_List_List*ps=_tmp172;int dots=_tmp174;
# 825
{struct Cyc_List_List*_tmp186=ps;struct Cyc_List_List*ps2=_tmp186;for(0;ps2 != 0;ps2=ps2->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps2->hd);}}
{struct _handler_cons _tmp187;_push_handler(& _tmp187);{int _tmp189=0;if(setjmp(_tmp187.handler))_tmp189=1;if(!_tmp189){{void*_tmp18A=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_stmttmp19=_tmp18A;void*_tmp18B=_stmttmp19;void*_tmp18D;void*_tmp18C;switch(*((int*)_tmp18B)){case 0:
 goto _LL3C;case 1: _tmp18C=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp18B)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp18C;
# 830
struct Cyc_List_List*_tmp18E=0;struct Cyc_List_List*new_ps=_tmp18E;
for(1;ps != 0;ps=ps->tl){
new_ps=({struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));({struct _tuple14*_tmp2ED=({struct _tuple14*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->f1=0,_tmp18F->f2=(struct Cyc_Absyn_Pat*)ps->hd;_tmp18F;});_tmp190->hd=_tmp2ED;}),_tmp190->tl=new_ps;_tmp190;});}
({void*_tmp2F1=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->tag=7,({union Cyc_Absyn_AggrInfo*_tmp2F0=({union Cyc_Absyn_AggrInfo*_tmp192=_cycalloc(sizeof(*_tmp192));({union Cyc_Absyn_AggrInfo _tmp2EF=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp191=_cycalloc(sizeof(*_tmp191));*_tmp191=ad;_tmp191;}));*_tmp192=_tmp2EF;});_tmp192;});_tmp193->f1=_tmp2F0;}),_tmp193->f2=0,({struct Cyc_List_List*_tmp2EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(new_ps);_tmp193->f3=_tmp2EE;}),_tmp193->f4=dots;_tmp193;});p->r=_tmp2F1;});
_npop_handler(0);return;}case 2: _tmp18C=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp18B)->f1;_tmp18D=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp18B)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp18C;struct Cyc_Absyn_Datatypefield*tuf=_tmp18D;
# 836
({void*_tmp2F2=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->tag=8,_tmp194->f1=tud,_tmp194->f2=tuf,_tmp194->f3=ps,_tmp194->f4=dots;_tmp194;});p->r=_tmp2F2;});_npop_handler(0);return;}case 3:
 goto _LL46;default: _LL46:
# 839
({struct Cyc_Warn_String_Warn_Warg_struct _tmp196=({struct Cyc_Warn_String_Warn_Warg_struct _tmp25E;_tmp25E.tag=0,({struct _fat_ptr _tmp2F3=({const char*_tmp197="enum tag used with arguments in pattern";_tag_fat(_tmp197,sizeof(char),40U);});_tmp25E.f1=_tmp2F3;});_tmp25E;});void*_tmp195[1];_tmp195[0]=& _tmp196;({unsigned _tmp2F4=loc;Cyc_Warn_err2(_tmp2F4,_tag_fat(_tmp195,sizeof(void*),1));});});_npop_handler(0);return;}_LL3C:;}
# 827
;_pop_handler();}else{void*_tmp188=(void*)Cyc_Core_get_exn_thrown();void*_tmp198=_tmp188;void*_tmp199;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp198)->tag == Cyc_Binding_BindingError)
# 840
goto _LL47;else{_tmp199=_tmp198;{void*exn=_tmp199;(int)_rethrow(exn);}}_LL47:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp19B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp260;_tmp260.tag=1,_tmp260.f1=qv;_tmp260;});struct Cyc_Warn_String_Warn_Warg_struct _tmp19C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp25F;_tmp25F.tag=0,({struct _fat_ptr _tmp2F5=({const char*_tmp19D=" is not a constructor in pattern";_tag_fat(_tmp19D,sizeof(char),33U);});_tmp25F.f1=_tmp2F5;});_tmp25F;});void*_tmp19A[2];_tmp19A[0]=& _tmp19B,_tmp19A[1]=& _tmp19C;({unsigned _tmp2F6=loc;Cyc_Warn_err2(_tmp2F6,_tag_fat(_tmp19A,sizeof(void*),2));});});return;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp170)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp170)->f1)->UnknownAggr).tag == 1){_tmp171=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp170)->f1)->UnknownAggr).val).f2;_tmp172=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp170)->f2;_tmp173=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp170)->f3;_tmp174=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp170)->f4;{struct _tuple0*n=_tmp171;struct Cyc_List_List*exist_ts=_tmp172;struct Cyc_List_List*dps=_tmp173;int dots=_tmp174;
# 844
{struct Cyc_List_List*_tmp19E=dps;struct Cyc_List_List*dps2=_tmp19E;for(0;dps2 != 0;dps2=dps2->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple14*)dps2->hd)).f2);}}
{struct _handler_cons _tmp19F;_push_handler(& _tmp19F);{int _tmp1A1=0;if(setjmp(_tmp19F.handler))_tmp1A1=1;if(!_tmp1A1){
{struct Cyc_Absyn_Aggrdecl*_tmp1A2=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,n,Cyc_Binding_lookup_aggrdecl);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A2;
({void*_tmp2F9=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->tag=7,({union Cyc_Absyn_AggrInfo*_tmp2F8=({union Cyc_Absyn_AggrInfo*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));({union Cyc_Absyn_AggrInfo _tmp2F7=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp1A3=_cycalloc(sizeof(*_tmp1A3));*_tmp1A3=ad;_tmp1A3;}));*_tmp1A4=_tmp2F7;});_tmp1A4;});_tmp1A5->f1=_tmp2F8;}),_tmp1A5->f2=exist_ts,_tmp1A5->f3=dps,_tmp1A5->f4=dots;_tmp1A5;});p->r=_tmp2F9;});}
# 847
;_pop_handler();}else{void*_tmp1A0=(void*)Cyc_Core_get_exn_thrown();void*_tmp1A6=_tmp1A0;void*_tmp1A7;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1A6)->tag == Cyc_Binding_BindingError){
# 850
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp261;_tmp261.tag=0,({struct _fat_ptr _tmp2FA=({const char*_tmp1AA="non-aggregate name has designator patterns";_tag_fat(_tmp1AA,sizeof(char),43U);});_tmp261.f1=_tmp2FA;});_tmp261;});void*_tmp1A8[1];_tmp1A8[0]=& _tmp1A9;({unsigned _tmp2FB=loc;Cyc_Warn_err2(_tmp2FB,_tag_fat(_tmp1A8,sizeof(void*),1));});});return;}else{_tmp1A7=_tmp1A6;{void*exn=_tmp1A7;(int)_rethrow(exn);}};}}}
# 852
return;}}else{_tmp171=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp170)->f3;_LL25: {struct Cyc_List_List*dps=_tmp171;
# 881
for(1;dps != 0;dps=dps->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple14*)dps->hd)).f2);}
return;}}}else{_tmp171=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp170)->f3;{struct Cyc_List_List*dps=_tmp171;
# 879
_tmp171=dps;goto _LL25;}}case 0:
# 854
 return;case 3: _tmp171=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp170)->f1;_tmp172=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp170)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp171;struct Cyc_Absyn_Pat*p2=_tmp172;
# 856
_tmp171=vd;_tmp172=p2;goto _LLF;}case 1: _tmp171=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp170)->f1;_tmp172=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp170)->f2;_LLF: {struct Cyc_Absyn_Vardecl*vd=_tmp171;struct Cyc_Absyn_Pat*p2=_tmp172;
# 858
Cyc_Binding_resolve_pat(env,p2);
_tmp171=vd;goto _LL11;}case 4: _tmp171=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp170)->f2;_LL11: {struct Cyc_Absyn_Vardecl*vd=_tmp171;
_tmp171=vd;goto _LL13;}case 2: _tmp171=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp170)->f2;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp171;
# 862
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL3;}case 6: _tmp171=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp170)->f1;{struct Cyc_Absyn_Pat*p2=_tmp171;
# 864
Cyc_Binding_resolve_pat(env,p2);return;}case 5: _tmp171=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp170)->f1;{struct Cyc_List_List*ps=_tmp171;
# 866
for(1;ps != 0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
goto _LL3;}case 17: _tmp171=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp170)->f1;{struct Cyc_Absyn_Exp*e=_tmp171;
# 870
Cyc_Binding_resolve_exp(env,e);return;}case 9:
# 872
 goto _LL1D;case 10: _LL1D:
 goto _LL1F;case 11: _LL1F:
 goto _LL21;case 12: _LL21:
 return;case 8: _tmp171=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp170)->f3;{struct Cyc_List_List*ps=_tmp171;
# 885
for(1;ps != 0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 13:
 goto _LL2B;default: _LL2B:
 return;}_LL3:;}}struct _tuple15{void*f1;void*f2;};
# 893
void Cyc_Binding_resolve_rgnpo(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(1;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple15*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple15*)po->hd)).f2);}}struct _tuple16{struct Cyc_List_List**f1;struct Cyc_Dict_Dict*f2;struct Cyc_Binding_Env*f3;struct _tuple10*f4;};struct _tuple17{unsigned f1;struct _tuple0*f2;int f3;};
# 902
void Cyc_Binding_export_all_symbols(struct _tuple16*cenv,struct _fat_ptr*name,void*res){
# 904
struct _tuple16*_tmp1AB=cenv;void*_tmp1B0;unsigned _tmp1AF;void*_tmp1AE;void*_tmp1AD;void*_tmp1AC;_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;_tmp1AE=_tmp1AB->f3;_tmp1AF=(_tmp1AB->f4)->f1;_tmp1B0=(_tmp1AB->f4)->f2;{struct Cyc_List_List**exlist_ptr=_tmp1AC;struct Cyc_Dict_Dict*out_dict=_tmp1AD;struct Cyc_Binding_Env*env=_tmp1AE;unsigned wcloc=_tmp1AF;struct Cyc_List_List*hidelist=_tmp1B0;
void*_tmp1B1=res;void*_tmp1B3;void*_tmp1B2;switch(*((int*)_tmp1B1)){case 0: _tmp1B2=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp1B1)->f1;{void*bnd=_tmp1B2;
# 907
struct _tuple0*qv=({struct _tuple0*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));({union Cyc_Absyn_Nmspace _tmp2FC=Cyc_Absyn_Rel_n(0);_tmp1B7->f1=_tmp2FC;}),_tmp1B7->f2=name;_tmp1B7;});
if(((int(*)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,hidelist,qv))
# 910
return;
# 912
Cyc_Binding_absolutize_decl(wcloc,env,qv,Cyc_Absyn_ExternC);{
struct _tuple17*_tmp1B4=({struct _tuple17*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->f1=wcloc,_tmp1B6->f2=qv,_tmp1B6->f3=0;_tmp1B6;});struct _tuple17*ex_sym=_tmp1B4;
({struct Cyc_List_List*_tmp2FD=({struct Cyc_List_List*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->hd=ex_sym,_tmp1B5->tl=*exlist_ptr;_tmp1B5;});*exlist_ptr=_tmp2FD;});
({struct Cyc_Dict_Dict _tmp2FE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(*out_dict,name,res);*out_dict=_tmp2FE;});
goto _LL3;}}case 3: _tmp1B2=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1B1)->f1;_tmp1B3=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1B1)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp1B2;struct Cyc_Absyn_Enumfield*ef=_tmp1B3;
# 918
({struct Cyc_Dict_Dict _tmp2FF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(*out_dict,name,res);*out_dict=_tmp2FF;});
goto _LL3;}case 1: _tmp1B2=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp1B1)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp1B2;
# 922
({struct Cyc_Dict_Dict _tmp300=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(*out_dict,name,res);*out_dict=_tmp300;});
goto _LL3;}case 4: _tmp1B2=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1B1)->f1;_tmp1B3=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1B1)->f2;{void*t=_tmp1B2;struct Cyc_Absyn_Enumfield*ef=_tmp1B3;
# 925
goto _LL3;}default: _tmp1B2=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1B1)->f1;_tmp1B3=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1B1)->f2;{struct Cyc_Absyn_Datatypedecl*dd=_tmp1B2;struct Cyc_Absyn_Datatypefield*df=_tmp1B3;
# 927
({void*_tmp1B8=0U;({unsigned _tmp302=wcloc;struct _fat_ptr _tmp301=({const char*_tmp1B9="Unexpected binding from extern C Include";_tag_fat(_tmp1B9,sizeof(char),41U);});Cyc_Warn_err(_tmp302,_tmp301,_tag_fat(_tmp1B8,sizeof(void*),0));});});
goto _LL3;}}_LL3:;}}
# 940 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 943
unsigned loc=d->loc;
void*_tmp1BA=d->r;void*_stmttmp1A=_tmp1BA;void*_tmp1BB=_stmttmp1A;void*_tmp1BF;void*_tmp1BE;void*_tmp1BD;void*_tmp1BC;switch(*((int*)_tmp1BB)){case 0: _tmp1BC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp1BC;
# 947
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
# 952
if(vd->initializer != 0 && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1: _tmp1BC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp1BC;
# 957
if(env->in_cyc_override)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp262;_tmp262.tag=0,({struct _fat_ptr _tmp303=({const char*_tmp1C2="Function declaration not permitted within cyclone_override block";_tag_fat(_tmp1C2,sizeof(char),65U);});_tmp262.f1=_tmp303;});_tmp262;});void*_tmp1C0[1];_tmp1C0[0]=& _tmp1C1;({unsigned _tmp304=loc;Cyc_Warn_err2(_tmp304,_tag_fat(_tmp1C0,sizeof(void*),1));});});
Cyc_Binding_absolutize_decl(loc,env,fd->name,fd->sc);{
struct _tuple0*_tmp1C3=fd->name;struct _tuple0*_stmttmp1B=_tmp1C3;struct _tuple0*_tmp1C4=_stmttmp1B;void*_tmp1C6;union Cyc_Absyn_Nmspace _tmp1C5;_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;{union Cyc_Absyn_Nmspace decl_ns=_tmp1C5;struct _fat_ptr*decl_name=_tmp1C6;
struct Cyc_List_List*_tmp1C7=(fd->i).args;struct Cyc_List_List*argit=_tmp1C7;
struct Cyc_List_List*_tmp1C8=Cyc_Binding_get_fun_vardecls(1,loc,env,(fd->i).args,(fd->i).cyc_varargs);struct Cyc_List_List*vds=_tmp1C8;
({struct Cyc_Core_Opt*_tmp305=({struct Cyc_Core_Opt*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->v=vds;_tmp1C9;});fd->param_vardecls=_tmp305;});
# 965
if(Cyc_Flags_warn_override)
Cyc_Binding_check_warn_override(loc,env,fd->name);{
# 968
struct Cyc_Dict_Dict*_tmp1CA=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp1CA;
if(old_locals != 0)
({struct Cyc_Dict_Dict*_tmp306=({struct Cyc_Dict_Dict*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));*_tmp1CB=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1CB;});env->local_vars=_tmp306;});else{
# 972
({struct Cyc_Dict_Dict*_tmp308=({struct Cyc_Dict_Dict*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));({struct Cyc_Dict_Dict _tmp307=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp1CC=_tmp307;});_tmp1CC;});env->local_vars=_tmp308;});}
{struct Cyc_List_List*_tmp1CD=vds;struct Cyc_List_List*vds1=_tmp1CD;for(0;vds1 != 0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _tmp30C=({struct Cyc_Dict_Dict _tmp30B=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp30A=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp30B,_tmp30A,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));
_tmp1CF->tag=0,({void*_tmp309=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->tag=3,_tmp1CE->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_tmp1CE;});_tmp1CF->f1=_tmp309;});_tmp1CF;}));});
# 974
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp30C;});}}
# 977
Cyc_Binding_resolve_function_stuff(loc,env,& fd->i);
# 979
if(old_locals != 0){
# 981
({struct Cyc_Dict_Dict _tmp310=({struct Cyc_Dict_Dict _tmp30F=*old_locals;struct _fat_ptr*_tmp30E=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp30F,_tmp30E,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));
_tmp1D1->tag=0,({void*_tmp30D=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->tag=2,_tmp1D0->f1=fd;_tmp1D0;});_tmp1D1->f1=_tmp30D;});_tmp1D1;}));});
# 981
*old_locals=_tmp310;});
# 983
({struct Cyc_Dict_Dict _tmp314=({struct Cyc_Dict_Dict _tmp313=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp312=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp313,_tmp312,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));
_tmp1D3->tag=0,({void*_tmp311=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->tag=2,_tmp1D2->f1=fd;_tmp1D2;});_tmp1D3->f1=_tmp311;});_tmp1D3;}));});
# 983
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp314;});}else{
# 986
struct Cyc_Binding_ResolveNSEnv*_tmp1D4=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp1D4;
({struct Cyc_Dict_Dict _tmp318=({struct Cyc_Dict_Dict _tmp317=decl_ns_data->ordinaries;struct _fat_ptr*_tmp316=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp317,_tmp316,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));
# 989
_tmp1D6->tag=0,({void*_tmp315=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->tag=2,_tmp1D5->f1=fd;_tmp1D5;});_tmp1D6->f1=_tmp315;});_tmp1D6;}));});
# 987
decl_ns_data->ordinaries=_tmp318;});}
# 992
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,fd->body);
# 995
env->local_vars=old_locals;
goto _LL0;}}}}case 2: _tmp1BC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_tmp1BD=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp1BB)->f3;{struct Cyc_Absyn_Pat*p=_tmp1BC;struct Cyc_Absyn_Exp*exp=_tmp1BD;
# 999
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp263;_tmp263.tag=0,({struct _fat_ptr _tmp319=({const char*_tmp1D9="let not allowed at top-level";_tag_fat(_tmp1D9,sizeof(char),29U);});_tmp263.f1=_tmp319;});_tmp263;});void*_tmp1D7[1];_tmp1D7[0]=& _tmp1D8;({unsigned _tmp31A=loc;Cyc_Warn_err2(_tmp31A,_tag_fat(_tmp1D7,sizeof(void*),1));});});
goto _LL0;}
# 1003
Cyc_Binding_resolve_exp(env,exp);
Cyc_Binding_resolve_pat(env,p);
goto _LL0;}case 3: _tmp1BC=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;{struct Cyc_List_List*vds=_tmp1BC;
# 1008
for(1;vds != 0;vds=vds->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)vds->hd);}
goto _LL0;}case 4: _tmp1BC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_tmp1BD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1BB)->f2;_tmp1BE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1BB)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp1BC;struct Cyc_Absyn_Vardecl*vd=_tmp1BD;struct Cyc_Absyn_Exp*open_exp_opt=_tmp1BE;
# 1013
if(open_exp_opt != 0)
Cyc_Binding_resolve_exp(env,open_exp_opt);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
goto _LL0;}case 8: _tmp1BC=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp1BC;
# 1019
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp264;_tmp264.tag=0,({struct _fat_ptr _tmp31B=({const char*_tmp1DC="nested type definitions are not yet supported";_tag_fat(_tmp1DC,sizeof(char),46U);});_tmp264.f1=_tmp31B;});_tmp264;});void*_tmp1DA[1];_tmp1DA[0]=& _tmp1DB;({unsigned _tmp31C=loc;Cyc_Warn_err2(_tmp31C,_tag_fat(_tmp1DA,sizeof(void*),1));});});
goto _LL0;}
# 1023
Cyc_Binding_absolutize_decl(loc,env,td->name,td->extern_c?4U: 2U);{
struct _tuple0*_tmp1DD=td->name;struct _tuple0*_stmttmp1C=_tmp1DD;struct _tuple0*_tmp1DE=_stmttmp1C;void*_tmp1E0;union Cyc_Absyn_Nmspace _tmp1DF;_tmp1DF=_tmp1DE->f1;_tmp1E0=_tmp1DE->f2;{union Cyc_Absyn_Nmspace decl_ns=_tmp1DF;struct _fat_ptr*decl_name=_tmp1E0;
# 1026
struct Cyc_Binding_ResolveNSEnv*_tmp1E1=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp1E1;
if(td->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(td->defn));
# 1030
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _tmp31D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*))Cyc_Dict_insert)(decl_ns_data->typedefs,decl_name,td);decl_ns_data->typedefs=_tmp31D;});
# 1034
goto _LL0;}}}case 5: _tmp1BC=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp1BC;
# 1037
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp265;_tmp265.tag=0,({struct _fat_ptr _tmp31E=({const char*_tmp1E4="nested type definitions are not yet supported";_tag_fat(_tmp1E4,sizeof(char),46U);});_tmp265.f1=_tmp31E;});_tmp265;});void*_tmp1E2[1];_tmp1E2[0]=& _tmp1E3;({unsigned _tmp31F=loc;Cyc_Warn_err2(_tmp31F,_tag_fat(_tmp1E2,sizeof(void*),1));});});
goto _LL0;}
# 1041
Cyc_Binding_absolutize_decl(loc,env,ad->name,ad->sc);{
struct _tuple0*_tmp1E5=ad->name;struct _tuple0*_stmttmp1D=_tmp1E5;struct _tuple0*_tmp1E6=_stmttmp1D;void*_tmp1E8;union Cyc_Absyn_Nmspace _tmp1E7;_tmp1E7=_tmp1E6->f1;_tmp1E8=_tmp1E6->f2;{union Cyc_Absyn_Nmspace decl_ns=_tmp1E7;struct _fat_ptr*decl_name=_tmp1E8;
# 1044
struct Cyc_Binding_ResolveNSEnv*_tmp1E9=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp1E9;
# 1046
if(((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(decl_ns_data->aggrdecls,decl_name)&& ad->impl == 0)
goto _LL0;
# 1049
if(!env->in_cyc_override){
({struct Cyc_Dict_Dict _tmp320=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert)(decl_ns_data->aggrdecls,decl_name,ad);decl_ns_data->aggrdecls=_tmp320;});
# 1052
({struct Cyc_Dict_Dict _tmp323=({struct Cyc_Dict_Dict _tmp322=decl_ns_data->ordinaries;struct _fat_ptr*_tmp321=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp322,_tmp321,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));
_tmp1EA->tag=1,_tmp1EA->f1=ad;_tmp1EA;}));});
# 1052
decl_ns_data->ordinaries=_tmp323;});}
# 1055
if(ad->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp1EB=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl);struct Cyc_Absyn_AggrdeclImpl*_stmttmp1E=_tmp1EB;struct Cyc_Absyn_AggrdeclImpl*_tmp1EC=_stmttmp1E;void*_tmp1EE;void*_tmp1ED;_tmp1ED=_tmp1EC->rgn_po;_tmp1EE=_tmp1EC->fields;{struct Cyc_List_List*rpo=_tmp1ED;struct Cyc_List_List*fs=_tmp1EE;
Cyc_Binding_resolve_rgnpo(loc,env,rpo);
Cyc_Binding_resolve_aggrfields(loc,env,fs);}}
# 1060
goto _LL0;}}}case 7: _tmp1BC=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp1BC;
# 1063
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp266;_tmp266.tag=0,({struct _fat_ptr _tmp324=({const char*_tmp1F1="nested type definitions are not yet supported";_tag_fat(_tmp1F1,sizeof(char),46U);});_tmp266.f1=_tmp324;});_tmp266;});void*_tmp1EF[1];_tmp1EF[0]=& _tmp1F0;({unsigned _tmp325=loc;Cyc_Warn_err2(_tmp325,_tag_fat(_tmp1EF,sizeof(void*),1));});});
goto _LL0;}
# 1067
Cyc_Binding_absolutize_decl(loc,env,ed->name,ed->sc);{
struct _tuple0*_tmp1F2=ed->name;struct _tuple0*_stmttmp1F=_tmp1F2;struct _tuple0*_tmp1F3=_stmttmp1F;void*_tmp1F5;union Cyc_Absyn_Nmspace _tmp1F4;_tmp1F4=_tmp1F3->f1;_tmp1F5=_tmp1F3->f2;{union Cyc_Absyn_Nmspace decl_ns=_tmp1F4;struct _fat_ptr*decl_name=_tmp1F5;
# 1070
struct Cyc_Binding_ResolveNSEnv*_tmp1F6=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp1F6;
# 1072
if(((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(decl_ns_data->enumdecls,decl_name)&& ed->fields == 0)
goto _LL0;
# 1076
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _tmp326=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert)(decl_ns_data->enumdecls,decl_name,ed);decl_ns_data->enumdecls=_tmp326;});
# 1079
if(ed->fields != 0){
# 1081
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1F7=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp1F7;
Cyc_Binding_absolutize_decl(f->loc,env,f->name,ed->sc);
if(f->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _tmp329=({struct Cyc_Dict_Dict _tmp328=decl_ns_data->ordinaries;struct _fat_ptr*_tmp327=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp328,_tmp327,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));
# 1089
_tmp1F8->tag=3,_tmp1F8->f1=ed,_tmp1F8->f2=f;_tmp1F8;}));});
# 1087
decl_ns_data->ordinaries=_tmp329;});}}
# 1091
goto _LL0;}}}case 6: _tmp1BC=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_tmp1BC;
# 1095
{struct _handler_cons _tmp1F9;_push_handler(& _tmp1F9);{int _tmp1FB=0;if(setjmp(_tmp1F9.handler))_tmp1FB=1;if(!_tmp1FB){
{struct Cyc_List_List*_tmp1FC=((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,tud->name,Cyc_Binding_lookup_datatypedecl);struct Cyc_List_List*decls=_tmp1FC;
struct Cyc_Absyn_Datatypedecl*_tmp1FD=(struct Cyc_Absyn_Datatypedecl*)decls->hd;struct Cyc_Absyn_Datatypedecl*last_decl=_tmp1FD;
if(!last_decl->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->tag=Cyc_Binding_BindingError;_tmp1FE;}));
tud->name=last_decl->name;
tud->is_extensible=1;{
struct _tuple0*_tmp1FF=tud->name;struct _tuple0*_stmttmp20=_tmp1FF;struct _tuple0*_tmp200=_stmttmp20;void*_tmp202;union Cyc_Absyn_Nmspace _tmp201;_tmp201=_tmp200->f1;_tmp202=_tmp200->f2;{union Cyc_Absyn_Nmspace decl_ns=_tmp201;struct _fat_ptr*decl_name=_tmp202;
struct Cyc_Binding_ResolveNSEnv*_tmp203=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp203;
({struct Cyc_Dict_Dict _tmp32C=({struct Cyc_Dict_Dict _tmp32B=decl_ns_data->datatypedecls;struct _fat_ptr*_tmp32A=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))Cyc_Dict_insert)(_tmp32B,_tmp32A,({struct Cyc_List_List*_tmp204=_cycalloc(sizeof(*_tmp204));
_tmp204->hd=tud,_tmp204->tl=decls;_tmp204;}));});
# 1104
decl_ns_data->datatypedecls=_tmp32C;});}}}
# 1096
;_pop_handler();}else{void*_tmp1FA=(void*)Cyc_Core_get_exn_thrown();void*_tmp205=_tmp1FA;void*_tmp206;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp205)->tag == Cyc_Binding_BindingError){
# 1108
Cyc_Binding_absolutize_topdecl(loc,env,tud->name,tud->sc);{
struct _tuple0*_tmp207=tud->name;struct _tuple0*_stmttmp21=_tmp207;struct _tuple0*_tmp208=_stmttmp21;void*_tmp20A;union Cyc_Absyn_Nmspace _tmp209;_tmp209=_tmp208->f1;_tmp20A=_tmp208->f2;{union Cyc_Absyn_Nmspace decl_ns=_tmp209;struct _fat_ptr*decl_name=_tmp20A;
struct Cyc_Binding_ResolveNSEnv*_tmp20B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp20B;
# 1112
if(((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(decl_ns_data->datatypedecls,decl_name)&& tud->fields == 0)
# 1114
goto _LL35;
({struct Cyc_Dict_Dict _tmp32F=({struct Cyc_Dict_Dict _tmp32E=decl_ns_data->datatypedecls;struct _fat_ptr*_tmp32D=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))Cyc_Dict_insert)(_tmp32E,_tmp32D,({struct Cyc_List_List*_tmp20C=_cycalloc(sizeof(*_tmp20C));
_tmp20C->hd=tud,_tmp20C->tl=0;_tmp20C;}));});
# 1115
decl_ns_data->datatypedecls=_tmp32F;});
# 1117
goto _LL35;}}}else{_tmp206=_tmp205;{void*exn=_tmp206;(int)_rethrow(exn);}}_LL35:;}}}{
# 1120
struct _tuple0*_tmp20D=tud->name;struct _tuple0*_stmttmp22=_tmp20D;struct _tuple0*_tmp20E=_stmttmp22;void*_tmp210;union Cyc_Absyn_Nmspace _tmp20F;_tmp20F=_tmp20E->f1;_tmp210=_tmp20E->f2;{union Cyc_Absyn_Nmspace decl_ns=_tmp20F;struct _fat_ptr*decl_name=_tmp210;
if(tud->fields != 0){
struct Cyc_List_List*_tmp211=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmp211;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp212=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp212;
{struct Cyc_List_List*_tmp213=f->typs;struct Cyc_List_List*tqts=_tmp213;for(0;tqts != 0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)tqts->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp214=(*f->name).f1;union Cyc_Absyn_Nmspace _stmttmp23=_tmp214;union Cyc_Absyn_Nmspace _tmp215=_stmttmp23;switch((_tmp215.Abs_n).tag){case 1: if((_tmp215.Rel_n).val == 0){
# 1128
if(tud->is_extensible)
({union Cyc_Absyn_Nmspace _tmp330=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*f->name).f1=_tmp330;});else{
# 1131
(*f->name).f1=(*tud->name).f1;}
goto _LL40;}else{
# 1134
({struct Cyc_Warn_String_Warn_Warg_struct _tmp217=({struct Cyc_Warn_String_Warn_Warg_struct _tmp267;_tmp267.tag=0,({struct _fat_ptr _tmp331=({const char*_tmp218="qualified datatype field declarations not allowed";_tag_fat(_tmp218,sizeof(char),50U);});_tmp267.f1=_tmp331;});_tmp267;});void*_tmp216[1];_tmp216[0]=& _tmp217;({unsigned _tmp332=f->loc;Cyc_Warn_err2(_tmp332,_tag_fat(_tmp216,sizeof(void*),1));});});
return;}case 2:
 goto _LL40;default:
({void*_tmp219=0U;({struct _fat_ptr _tmp333=({const char*_tmp21A="datatype field Loc_n or C_n";_tag_fat(_tmp21A,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp333,_tag_fat(_tmp219,sizeof(void*),0));});});}_LL40:;}{
# 1139
struct Cyc_Binding_ResolveNSEnv*_tmp21B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,(*f->name).f1);struct Cyc_Binding_ResolveNSEnv*ns_data=_tmp21B;
({struct Cyc_Dict_Dict _tmp336=({struct Cyc_Dict_Dict _tmp335=ns_data->ordinaries;struct _fat_ptr*_tmp334=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp335,_tmp334,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp21C=_cycalloc(sizeof(*_tmp21C));
# 1142
_tmp21C->tag=2,_tmp21C->f1=tud,_tmp21C->f2=f;_tmp21C;}));});
# 1140
ns_data->ordinaries=_tmp336;});}}}
# 1144
goto _LL0;}}}case 9: _tmp1BC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_tmp1BD=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BB)->f2;{struct _fat_ptr*v=_tmp1BC;struct Cyc_List_List*ds2=_tmp1BD;
# 1147
((void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_enter_ns)(env->ns,v,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,ds2);
((void(*)(struct Cyc_Binding_NSCtxt*))Cyc_Binding_leave_ns)(env->ns);
goto _LL0;}case 10: _tmp1BC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_tmp1BD=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BB)->f2;{struct _tuple0*qv=_tmp1BC;struct Cyc_List_List*ds2=_tmp1BD;
# 1152
((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*))Cyc_Binding_enter_using)(d->loc,env->ns,qv);
Cyc_Binding_resolve_decls(env,ds2);
((void(*)(struct Cyc_Binding_NSCtxt*))Cyc_Binding_leave_using)(env->ns);
goto _LL0;}case 11: _tmp1BC=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;{struct Cyc_List_List*ds2=_tmp1BC;
# 1158
int _tmp21D=env->in_cinclude;int old=_tmp21D;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
env->in_cinclude=old;
goto _LL0;}case 12: _tmp1BC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_tmp1BD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BB)->f2;_tmp1BE=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BB)->f3;_tmp1BF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BB)->f4;{struct Cyc_List_List*ds2=_tmp1BC;struct Cyc_List_List*ovrs=_tmp1BD;struct Cyc_List_List**exports=(struct Cyc_List_List**)_tmp1BE;struct _tuple10*hides=_tmp1BF;
# 1169
{struct Cyc_List_List*_tmp21E=*exports;struct Cyc_List_List*exs=_tmp21E;for(0;exs != 0;exs=exs->tl){
struct _tuple17*_tmp21F=(struct _tuple17*)exs->hd;struct _tuple17*_stmttmp24=_tmp21F;struct _tuple17*_tmp220=_stmttmp24;void*_tmp222;unsigned _tmp221;_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;{unsigned loc=_tmp221;struct _tuple0*qv=_tmp222;
Cyc_Binding_absolutize_decl(loc,env,qv,Cyc_Absyn_ExternC);}}}
# 1177
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp224=({struct Cyc_Warn_String_Warn_Warg_struct _tmp268;_tmp268.tag=0,({struct _fat_ptr _tmp337=({const char*_tmp225="extern \"C include\" not at toplevel";_tag_fat(_tmp225,sizeof(char),35U);});_tmp268.f1=_tmp337;});_tmp268;});void*_tmp223[1];_tmp223[0]=& _tmp224;({unsigned _tmp338=loc;Cyc_Warn_err2(_tmp338,_tag_fat(_tmp223,sizeof(void*),1));});});
goto _LL0;}{
# 1181
struct Cyc_Binding_ResolveNSEnv*_tmp226=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);struct Cyc_Binding_ResolveNSEnv*data=_tmp226;
struct Cyc_Dict_Dict _tmp227=data->ordinaries;struct Cyc_Dict_Dict old_dict=_tmp227;
int _tmp228=env->in_cinclude;int old=_tmp228;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
env->in_cyc_override=1;
Cyc_Binding_resolve_decls(env,ovrs);
env->in_cyc_override=0;{
struct Cyc_Dict_Dict _tmp229=data->ordinaries;struct Cyc_Dict_Dict new_dict=_tmp229;
struct Cyc_Dict_Dict _tmp22A=old_dict;struct Cyc_Dict_Dict out_dict=_tmp22A;
if((*hides).f1 > (unsigned)0){
if((unsigned)*exports)
({void*_tmp22B=0U;({unsigned _tmp33A=(*hides).f1;struct _fat_ptr _tmp339=({const char*_tmp22C="export wildcard expects empty export list";_tag_fat(_tmp22C,sizeof(char),42U);});Cyc_Warn_err(_tmp33A,_tmp339,_tag_fat(_tmp22B,sizeof(void*),0));});});
env->in_cinclude=old;
({struct _tuple16*_tmp33B=({struct _tuple16*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->f1=exports,_tmp22D->f2=& out_dict,_tmp22D->f3=env,_tmp22D->f4=hides;_tmp22D;});((void(*)(void(*)(struct _tuple16*,struct _fat_ptr*,void*),struct _tuple16*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Binding_export_all_symbols,_tmp33B,
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_difference)(new_dict,old_dict));});}else{
# 1199
struct Cyc_List_List*_tmp22E=*exports;struct Cyc_List_List*exs=_tmp22E;for(0;exs != 0;exs=exs->tl){
struct _tuple17*_tmp22F=(struct _tuple17*)exs->hd;struct _tuple17*_stmttmp25=_tmp22F;struct _tuple17*_tmp230=_stmttmp25;void*_tmp232;unsigned _tmp231;_tmp231=_tmp230->f1;_tmp232=(_tmp230->f2)->f2;{unsigned loc=_tmp231;struct _fat_ptr*v=_tmp232;
if(!((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(new_dict,v)||
((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(old_dict,v)&&({
void*_tmp33C=((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(old_dict,v);_tmp33C == ((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(new_dict,v);}))
({struct Cyc_String_pa_PrintArg_struct _tmp235=({struct Cyc_String_pa_PrintArg_struct _tmp269;_tmp269.tag=0,_tmp269.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp269;});void*_tmp233[1];_tmp233[0]=& _tmp235;({unsigned _tmp33E=loc;struct _fat_ptr _tmp33D=({const char*_tmp234="%s is exported but not defined";_tag_fat(_tmp234,sizeof(char),31U);});Cyc_Warn_err(_tmp33E,_tmp33D,_tag_fat(_tmp233,sizeof(void*),1));});});
out_dict=({struct Cyc_Dict_Dict _tmp340=out_dict;struct _fat_ptr*_tmp33F=v;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp340,_tmp33F,((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(new_dict,v));});}}}
# 1208
data->ordinaries=out_dict;
env->in_cinclude=old;
# 1213
goto _LL0;}}}case 13:
# 1215
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
 goto _LL0;}_LL0:;}
# 1222
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(1;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1227
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp236=({struct Cyc_Binding_Env*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->in_cinclude=0,_tmp239->in_cyc_override=0,({struct Cyc_Binding_NSCtxt*_tmp341=((struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp239->ns=_tmp341;}),_tmp239->local_vars=0;_tmp239;});struct Cyc_Binding_Env*env=_tmp236;
({struct Cyc_Binding_Env*_tmp344=env;Cyc_Binding_resolve_decl(_tmp344,({struct Cyc_Absyn_Decl*_tmp238=_cycalloc(sizeof(*_tmp238));({void*_tmp343=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237->tag=6,({struct Cyc_Absyn_Datatypedecl*_tmp342=Cyc_Absyn_exn_tud();_tmp237->f1=_tmp342;});_tmp237;});_tmp238->r=_tmp343;}),_tmp238->loc=0U;_tmp238;}));});
Cyc_Binding_resolve_decls(env,tds);}

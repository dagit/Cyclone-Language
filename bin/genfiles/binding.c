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
extern struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 90 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 153
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 174
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 179
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 184
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 287
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 300
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 307
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 410 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 480
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 673 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 859
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 861
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 901
extern void*Cyc_Absyn_heap_rgn_type;
# 905
extern void*Cyc_Absyn_false_type;
# 907
extern void*Cyc_Absyn_void_type;
# 923
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 950
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1060
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1069
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
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
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
struct Cyc_List_List*_tmp315=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({void*_tmp314=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0,_tmp1->f1=0;_tmp1;});_tmp2->hd=_tmp314;}),_tmp2->tl=0;_tmp2;});_tmp3->availables=_tmp315;}),({
struct Cyc_Dict_Dict _tmp313=((struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*,void*))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,data);_tmp3->ns_data=_tmp313;});_tmp3;});}
# 63
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp4=abs_ns;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp6;switch((_tmp4.Abs_n).tag){case 3: _LL1: _tmp6=(_tmp4.C_n).val;_LL2: {struct Cyc_List_List*vs=_tmp6;
_tmp5=vs;goto _LL4;}case 2: _LL3: _tmp5=(_tmp4.Abs_n).val;_LL4: {struct Cyc_List_List*vs=_tmp5;
return((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,vs);}default: _LL5: _LL6:
({void*_tmp7=0U;({struct _fat_ptr _tmp316=({const char*_tmp8="Binding:get_ns_data: relative ns";_tag_fat(_tmp8,sizeof(char),33U);});Cyc_Warn_impos(_tmp316,_tag_fat(_tmp7,sizeof(void*),0));});});}_LL0:;}
# 81 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 84
struct Cyc_List_List*_tmp9=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ctxt->curr_ns,rel_ns);struct Cyc_List_List*fullname=_tmp9;
if(((int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member)(ctxt->ns_data,fullname))
return fullname;
{struct Cyc_List_List*_tmpA=ctxt->availables;struct Cyc_List_List*as=_tmpA;for(0;as != 0;as=as->tl){
void*_tmpB=(void*)as->hd;void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;if(*((int*)_tmpC)== 1){_LL1: _tmpE=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->f1;_LL2: {struct Cyc_List_List*ns=_tmpE;
_tmpD=ns;goto _LL4;}}else{_LL3: _tmpD=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpC)->f1;_LL4: {struct Cyc_List_List*ns=_tmpD;
# 91
struct Cyc_List_List*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ns,rel_ns);struct Cyc_List_List*fullname=_tmpF;
if(((int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member)(ctxt->ns_data,fullname))
return fullname;
goto _LL0;}}_LL0:;}}
# 96
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5;_tmp2E5.tag=0,({struct _fat_ptr _tmp317=({const char*_tmp16="namespace ";_tag_fat(_tmp16,sizeof(char),11U);});_tmp2E5.f1=_tmp317;});_tmp2E5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4;_tmp2E4.tag=0,({struct _fat_ptr _tmp319=(struct _fat_ptr)({struct Cyc_List_List*_tmp318=rel_ns;Cyc_str_sepstr(_tmp318,({const char*_tmp15="::";_tag_fat(_tmp15,sizeof(char),3U);}));});_tmp2E4.f1=_tmp319;});_tmp2E4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3;_tmp2E3.tag=0,({struct _fat_ptr _tmp31A=({const char*_tmp14=" not found";_tag_fat(_tmp14,sizeof(char),11U);});_tmp2E3.f1=_tmp31A;});_tmp2E3;});void*_tmp10[3];_tmp10[0]=& _tmp11,_tmp10[1]=& _tmp12,_tmp10[2]=& _tmp13;({unsigned _tmp31B=loc;Cyc_Warn_err2(_tmp31B,_tag_fat(_tmp10,sizeof(void*),3));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=Cyc_Binding_BindingError;_tmp17;}));}
# 101
void*Cyc_Binding_resolve_lookup(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _fat_ptr*)){
struct _tuple0*_tmp18=qv;struct _fat_ptr*_tmp1A;union Cyc_Absyn_Nmspace _tmp19;_LL1: _tmp19=_tmp18->f1;_tmp1A=_tmp18->f2;_LL2: {union Cyc_Absyn_Nmspace ns=_tmp19;struct _fat_ptr*v=_tmp1A;
union Cyc_Absyn_Nmspace _tmp1B=ns;struct Cyc_List_List*_tmp1C;struct Cyc_List_List*_tmp1D;struct Cyc_List_List*_tmp1E;switch((_tmp1B.Abs_n).tag){case 1: if((_tmp1B.Rel_n).val == 0){_LL4: _LL5:
# 105
{struct _handler_cons _tmp1F;_push_handler(& _tmp1F);{int _tmp21=0;if(setjmp(_tmp1F.handler))_tmp21=1;if(!_tmp21){{void*_tmp22=({void*(*_tmp31D)(void*,struct _fat_ptr*)=lookup;void*_tmp31C=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp31D(_tmp31C,v);});_npop_handler(0);return _tmp22;};_pop_handler();}else{void*_tmp20=(void*)Cyc_Core_get_exn_thrown();void*_tmp23=_tmp20;void*_tmp24;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp23)->tag == Cyc_Dict_Absent){_LLF: _LL10:
 goto _LLE;}else{_LL11: _tmp24=_tmp23;_LL12: {void*exn=_tmp24;(int)_rethrow(exn);}}_LLE:;}}}
{struct Cyc_List_List*_tmp25=ctxt->availables;struct Cyc_List_List*as=_tmp25;for(0;as != 0;as=as->tl){
void*_tmp26=(void*)as->hd;void*_stmttmp1=_tmp26;void*_tmp27=_stmttmp1;struct Cyc_List_List*_tmp28;struct Cyc_List_List*_tmp29;if(*((int*)_tmp27)== 1){_LL14: _tmp29=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp27)->f1;_LL15: {struct Cyc_List_List*ns=_tmp29;
_tmp28=ns;goto _LL17;}}else{_LL16: _tmp28=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp27)->f1;_LL17: {struct Cyc_List_List*ns=_tmp28;
# 111
{struct _handler_cons _tmp2A;_push_handler(& _tmp2A);{int _tmp2C=0;if(setjmp(_tmp2A.handler))_tmp2C=1;if(!_tmp2C){{void*_tmp2D=({void*(*_tmp31F)(void*,struct _fat_ptr*)=lookup;void*_tmp31E=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,ns);_tmp31F(_tmp31E,v);});_npop_handler(0);return _tmp2D;};_pop_handler();}else{void*_tmp2B=(void*)Cyc_Core_get_exn_thrown();void*_tmp2E=_tmp2B;void*_tmp2F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2E)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
 goto _LL18;}else{_LL1B: _tmp2F=_tmp2E;_LL1C: {void*exn=_tmp2F;(int)_rethrow(exn);}}_LL18:;}}}
goto _LL13;}}_LL13:;}}
# 115
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=Cyc_Binding_BindingError;_tmp30;}));}else{_LL6: _tmp1E=(_tmp1B.Rel_n).val;_LL7: {struct Cyc_List_List*ns=_tmp1E;
# 117
struct _handler_cons _tmp31;_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!_tmp33){
{struct Cyc_List_List*_tmp34=Cyc_Binding_resolve_rel_ns(loc,ctxt,ns);struct Cyc_List_List*abs_ns=_tmp34;
void*_tmp35=({void*(*_tmp321)(void*,struct _fat_ptr*)=lookup;void*_tmp320=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,abs_ns);_tmp321(_tmp320,v);});_npop_handler(0);return _tmp35;}
# 118
;_pop_handler();}else{void*_tmp32=(void*)Cyc_Core_get_exn_thrown();void*_tmp36=_tmp32;void*_tmp37;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp36)->tag == Cyc_Dict_Absent){_LL1E: _LL1F:
# 120
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->tag=Cyc_Binding_BindingError;_tmp38;}));}else{_LL20: _tmp37=_tmp36;_LL21: {void*exn=_tmp37;(int)_rethrow(exn);}}_LL1D:;}}}}case 3: _LL8: _tmp1D=(_tmp1B.C_n).val;_LL9: {struct Cyc_List_List*ns=_tmp1D;
_tmp1C=ns;goto _LLB;}case 2: _LLA: _tmp1C=(_tmp1B.Abs_n).val;_LLB: {struct Cyc_List_List*ns=_tmp1C;
# 123
struct _handler_cons _tmp39;_push_handler(& _tmp39);{int _tmp3B=0;if(setjmp(_tmp39.handler))_tmp3B=1;if(!_tmp3B){{void*_tmp3C=({void*(*_tmp323)(void*,struct _fat_ptr*)=lookup;void*_tmp322=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)(ctxt->ns_data,ns);_tmp323(_tmp322,v);});_npop_handler(0);return _tmp3C;};_pop_handler();}else{void*_tmp3A=(void*)Cyc_Core_get_exn_thrown();void*_tmp3D=_tmp3A;void*_tmp3E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3D)->tag == Cyc_Dict_Absent){_LL23: _LL24:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->tag=Cyc_Binding_BindingError;_tmp3F;}));}else{_LL25: _tmp3E=_tmp3D;_LL26: {void*exn=_tmp3E;(int)_rethrow(exn);}}_LL22:;}}}default: _LLC: _LLD:
({void*_tmp40=0U;({struct _fat_ptr _tmp324=({const char*_tmp41="lookup local in global";_tag_fat(_tmp41,sizeof(char),23U);});Cyc_Warn_impos(_tmp324,_tag_fat(_tmp40,sizeof(void*),0));});});}_LL3:;}}
# 129
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp42=ctxt->curr_ns;struct Cyc_List_List*ns=_tmp42;
struct Cyc_List_List*_tmp43=({struct Cyc_List_List*_tmp325=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp325,({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->hd=subname,_tmp46->tl=0;_tmp46;}));});struct Cyc_List_List*ns2=_tmp43;
if(!((int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member)(ctxt->ns_data,ns2))
({struct Cyc_Dict_Dict _tmp328=({struct Cyc_Dict_Dict _tmp327=ctxt->ns_data;struct Cyc_List_List*_tmp326=ns2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*,void*))Cyc_Dict_insert)(_tmp327,_tmp326,mkdata(env));});ctxt->ns_data=_tmp328;});
ctxt->curr_ns=ns2;
({struct Cyc_List_List*_tmp32A=({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));({void*_tmp329=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=0,_tmp44->f1=ns2;_tmp44;});_tmp45->hd=_tmp329;}),_tmp45->tl=ctxt->availables;_tmp45;});ctxt->availables=_tmp32A;});}
# 137
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp47=0U;({struct _fat_ptr _tmp32B=({const char*_tmp48="leaving topmost namespace";_tag_fat(_tmp48,sizeof(char),26U);});Cyc_Warn_impos(_tmp32B,_tag_fat(_tmp47,sizeof(void*),0));});});{
void*_tmp49=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_stmttmp2=_tmp49;void*_tmp4A=_stmttmp2;if(*((int*)_tmp4A)== 0){_LL1: _LL2:
# 142
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp32C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp32C;});
goto _LL0;}else{_LL3: _LL4:
# 146
({void*_tmp4B=0U;({struct _fat_ptr _tmp32D=({const char*_tmp4C="leaving using as namespace";_tag_fat(_tmp4C,sizeof(char),27U);});Cyc_Warn_impos(_tmp32D,_tag_fat(_tmp4B,sizeof(void*),0));});});}_LL0:;}}
# 149
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp4D=usename;struct _fat_ptr*_tmp4F;union Cyc_Absyn_Nmspace _tmp4E;_LL1: _tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;_LL2: {union Cyc_Absyn_Nmspace nsl=_tmp4E;struct _fat_ptr*v=_tmp4F;
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp50=nsl;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;switch((_tmp50.Loc_n).tag){case 1: _LL4: _tmp52=(_tmp50.Rel_n).val;_LL5: {struct Cyc_List_List*vs=_tmp52;
# 154
ns=({unsigned _tmp330=loc;struct Cyc_Binding_NSCtxt*_tmp32F=ctxt;Cyc_Binding_resolve_rel_ns(_tmp330,_tmp32F,({struct Cyc_List_List*_tmp32E=vs;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp32E,({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=v,_tmp53->tl=0;_tmp53;}));}));});{
struct Cyc_List_List*_tmp54=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(ns)))->tl);struct Cyc_List_List*abs_vs=_tmp54;
({union Cyc_Absyn_Nmspace _tmp331=Cyc_Absyn_Abs_n(abs_vs,0);(*usename).f1=_tmp331;});
goto _LL3;}}case 2: _LL6: _tmp51=(_tmp50.Abs_n).val;_LL7: {struct Cyc_List_List*vs=_tmp51;
# 159
ns=({struct Cyc_List_List*_tmp332=vs;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp332,({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=v,_tmp55->tl=0;_tmp55;}));});
goto _LL3;}case 4: _LL8: _LL9:
({void*_tmp56=0U;({struct _fat_ptr _tmp333=({const char*_tmp57="enter_using, Loc_n";_tag_fat(_tmp57,sizeof(char),19U);});Cyc_Warn_impos(_tmp333,_tag_fat(_tmp56,sizeof(void*),0));});});default: _LLA: _LLB:
({void*_tmp58=0U;({struct _fat_ptr _tmp334=({const char*_tmp59="enter_using, C_n";_tag_fat(_tmp59,sizeof(char),17U);});Cyc_Warn_impos(_tmp334,_tag_fat(_tmp58,sizeof(void*),0));});});}_LL3:;}
# 164
({struct Cyc_List_List*_tmp336=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));({void*_tmp335=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=1,_tmp5A->f1=ns;_tmp5A;});_tmp5B->hd=_tmp335;}),_tmp5B->tl=ctxt->availables;_tmp5B;});ctxt->availables=_tmp336;});
return ns;}}
# 167
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp5C=0U;({struct _fat_ptr _tmp337=({const char*_tmp5D="leaving topmost namespace as a using";_tag_fat(_tmp5D,sizeof(char),37U);});Cyc_Warn_impos(_tmp337,_tag_fat(_tmp5C,sizeof(void*),0));});});{
void*_tmp5E=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_stmttmp3=_tmp5E;void*_tmp5F=_stmttmp3;if(*((int*)_tmp5F)== 1){_LL1: _LL2:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL0;}else{_LL3: _LL4:
({void*_tmp60=0U;({struct _fat_ptr _tmp338=({const char*_tmp61="leaving namespace as using";_tag_fat(_tmp61,sizeof(char),27U);});Cyc_Warn_impos(_tmp338,_tag_fat(_tmp60,sizeof(void*),0));});});}_LL0:;}}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;int in_cyc_override;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 212 "binding.cyc"
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 215
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 218
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp62=_cycalloc(sizeof(*_tmp62));({struct Cyc_Dict_Dict _tmp33D=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->aggrdecls=_tmp33D;}),({
struct Cyc_Dict_Dict _tmp33C=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->datatypedecls=_tmp33C;}),({
struct Cyc_Dict_Dict _tmp33B=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->enumdecls=_tmp33B;}),({
struct Cyc_Dict_Dict _tmp33A=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->typedefs=_tmp33A;}),({
struct Cyc_Dict_Dict _tmp339=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->ordinaries=_tmp339;});_tmp62;});}
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
struct _tuple0*_tmp63=qv;struct _fat_ptr*_tmp65;union Cyc_Absyn_Nmspace _tmp64;_LL1: _tmp64=_tmp63->f1;_tmp65=_tmp63->f2;_LL2: {union Cyc_Absyn_Nmspace nsl=_tmp64;struct _fat_ptr*v=_tmp65;
union Cyc_Absyn_Nmspace _tmp66=nsl;switch((_tmp66.Rel_n).tag){case 4: _LL4: _LL5:
# 245
 if(Cyc_Binding_at_toplevel(env)|| !((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=Cyc_Binding_BindingError;_tmp67;}));
return((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v);case 1: if((_tmp66.Rel_n).val == 0){_LL6: _LL7:
# 249
 if(!Cyc_Binding_at_toplevel(env)&&((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v))
return((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 253
 return((void*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,void*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL3:;}}
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
union Cyc_Absyn_Nmspace _tmp68=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp4=_tmp68;union Cyc_Absyn_Nmspace _tmp69=_stmttmp4;switch((_tmp69.Abs_n).tag){case 1: if((_tmp69.Rel_n).val == 0){_LL1: _LL2:
# 276
 if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _tmp33F=({struct Cyc_List_List*_tmp33E=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp33E,Cyc_Binding_in_cinclude(env)||(int)sc == (int)Cyc_Absyn_ExternC);});(*qv).f1=_tmp33F;});else{
# 279
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL0;}else{goto _LL7;}case 4: _LL3: _LL4:
 goto _LL0;case 2: _LL5: _LL6:
# 283
 if(!Cyc_Binding_at_toplevel(env))
goto _LL8;
goto _LL0;default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E8;_tmp2E8.tag=0,({struct _fat_ptr _tmp340=({const char*_tmp6F="qualified names in declarations unimplemented (";_tag_fat(_tmp6F,sizeof(char),48U);});_tmp2E8.f1=_tmp340;});_tmp2E8;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E7;_tmp2E7.tag=1,_tmp2E7.f1=qv;_tmp2E7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6;_tmp2E6.tag=0,({struct _fat_ptr _tmp341=({const char*_tmp6E=")";_tag_fat(_tmp6E,sizeof(char),2U);});_tmp2E6.f1=_tmp341;});_tmp2E6;});void*_tmp6A[3];_tmp6A[0]=& _tmp6B,_tmp6A[1]=& _tmp6C,_tmp6A[2]=& _tmp6D;({unsigned _tmp342=loc;Cyc_Warn_err2(_tmp342,_tag_fat(_tmp6A,sizeof(void*),3));});});}_LL0:;}
# 292
void Cyc_Binding_absolutize_topdecl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp70=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp70;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=old_locals;}
# 299
void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _fat_ptr hides_what;
struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){
{void*_tmp74=Cyc_Binding_lookup_ordinary(loc,env,q);void*_stmttmp5=_tmp74;void*_tmp75=_stmttmp5;void*_tmp76;switch(*((int*)_tmp75)){case 0: _LL1: _tmp76=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp75)->f1;_LL2: {void*b=_tmp76;
# 304
if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0);return;}
{void*_tmp77=b;switch(*((int*)_tmp77)){case 1: _LLC: _LLD:
 hides_what=({const char*_tmp78="global variable";_tag_fat(_tmp78,sizeof(char),16U);});goto _LLB;case 4: _LLE: _LLF:
 hides_what=({const char*_tmp79="local variable";_tag_fat(_tmp79,sizeof(char),15U);});goto _LLB;case 3: _LL10: _LL11:
 hides_what=({const char*_tmp7A="parameter";_tag_fat(_tmp7A,sizeof(char),10U);});goto _LLB;case 5: _LL12: _LL13:
 hides_what=({const char*_tmp7B="pattern variable";_tag_fat(_tmp7B,sizeof(char),17U);});goto _LLB;case 2: _LL14: _LL15:
 hides_what=({const char*_tmp7C="function";_tag_fat(_tmp7C,sizeof(char),9U);});goto _LLB;default: _LL16: _LL17:
({void*_tmp7D=0U;({struct _fat_ptr _tmp343=({const char*_tmp7E="shadowing free variable!";_tag_fat(_tmp7E,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp343,_tag_fat(_tmp7D,sizeof(void*),0));});});}_LLB:;}
# 314
goto _LL0;}case 1: _LL3: _LL4:
 hides_what=({const char*_tmp7F="struct constructor";_tag_fat(_tmp7F,sizeof(char),19U);});goto _LL0;case 2: _LL5: _LL6:
 hides_what=({const char*_tmp80="datatype constructor";_tag_fat(_tmp80,sizeof(char),21U);});goto _LL0;case 3: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 hides_what=({const char*_tmp81="enum tag";_tag_fat(_tmp81,sizeof(char),9U);});goto _LL0;}_LL0:;}
# 320
({struct Cyc_Warn_String_Warn_Warg_struct _tmp83=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EA;_tmp2EA.tag=0,({struct _fat_ptr _tmp344=({const char*_tmp85="declaration hides ";_tag_fat(_tmp85,sizeof(char),19U);});_tmp2EA.f1=_tmp344;});_tmp2EA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9;_tmp2E9.tag=0,_tmp2E9.f1=hides_what;_tmp2E9;});void*_tmp82[2];_tmp82[0]=& _tmp83,_tmp82[1]=& _tmp84;({unsigned _tmp345=loc;Cyc_Warn_warn2(_tmp345,_tag_fat(_tmp82,sizeof(void*),2));});});
_npop_handler(0);return;
# 302
;_pop_handler();}else{void*_tmp72=(void*)Cyc_Core_get_exn_thrown();void*_tmp86=_tmp72;void*_tmp87;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp86)->tag == Cyc_Binding_BindingError){_LL19: _LL1A:
# 322
 return;}else{_LL1B: _tmp87=_tmp86;_LL1C: {void*exn=_tmp87;(int)_rethrow(exn);}}_LL18:;}}}
# 325
void Cyc_Binding_resolve_and_add_vardecl(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp88=vd->name;struct _tuple0*_stmttmp6=_tmp88;struct _tuple0*_tmp89=_stmttmp6;struct _fat_ptr*_tmp8B;union Cyc_Absyn_Nmspace _tmp8A;_LL1: _tmp8A=_tmp89->f1;_tmp8B=_tmp89->f2;_LL2: {union Cyc_Absyn_Nmspace decl_ns=_tmp8A;struct _fat_ptr*decl_name=_tmp8B;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Flags_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp349=({struct Cyc_Dict_Dict _tmp348=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp347=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp348,_tmp347,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D->tag=0,({void*_tmp346=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=4,_tmp8C->f1=vd;_tmp8C;});_tmp8D->f1=_tmp346;});_tmp8D;}));});
# 332
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp349;});else{
# 334
if(!env->in_cyc_override){
struct Cyc_Binding_ResolveNSEnv*_tmp8E=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp8E;
({struct Cyc_Dict_Dict _tmp34D=({struct Cyc_Dict_Dict _tmp34C=decl_ns_data->ordinaries;struct _fat_ptr*_tmp34B=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp34C,_tmp34B,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp90=_cycalloc(sizeof(*_tmp90));
# 338
_tmp90->tag=0,({void*_tmp34A=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=1,_tmp8F->f1=vd;_tmp8F;});_tmp90->f1=_tmp34A;});_tmp90;}));});
# 336
decl_ns_data->ordinaries=_tmp34D;});}}}}}
# 342
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp91=s->r;void*_stmttmp7=_tmp91;void*_tmp92=_stmttmp7;struct Cyc_List_List*_tmp94;struct Cyc_Absyn_Stmt*_tmp93;struct Cyc_List_List*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Stmt*_tmp98;struct Cyc_Absyn_Decl*_tmp97;struct Cyc_Absyn_Stmt*_tmp99;struct Cyc_List_List*_tmp9A;struct Cyc_Absyn_Stmt*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Stmt*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Stmt*_tmpA1;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Stmt*_tmpA8;struct Cyc_Absyn_Stmt*_tmpA7;struct Cyc_Absyn_Exp*_tmpA9;switch(*((int*)_tmp92)){case 0: _LL1: _LL2:
 return;case 1: _LL3: _tmpA9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmpA9;
Cyc_Binding_resolve_exp(env,e);return;}case 2: _LL5: _tmpA7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmpA7;struct Cyc_Absyn_Stmt*s2=_tmpA8;
Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_stmt(env,s2);return;}case 3: _LL7: _tmpA6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmpA6;
if((unsigned)eopt)Cyc_Binding_resolve_exp(env,eopt);return;}case 4: _LL9: _tmpA3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_tmpA5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmpA3;struct Cyc_Absyn_Stmt*s1=_tmpA4;struct Cyc_Absyn_Stmt*s2=_tmpA5;
# 349
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_stmt(env,s2);return;}case 14: _LLB: _tmpA1=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA2=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp92)->f2).f1;_LLC: {struct Cyc_Absyn_Stmt*s1=_tmpA1;struct Cyc_Absyn_Exp*e=_tmpA2;
_tmp9F=e;_tmpA0=s1;goto _LLE;}case 5: _LLD: _tmp9F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp92)->f1).f1;_tmpA0=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp9F;struct Cyc_Absyn_Stmt*s1=_tmpA0;
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_stmt(env,s1);return;}case 9: _LLF: _tmp9B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp9C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f2).f1;_tmp9D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f3).f1;_tmp9E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f4;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp9B;struct Cyc_Absyn_Exp*e2=_tmp9C;struct Cyc_Absyn_Exp*e3=_tmp9D;struct Cyc_Absyn_Stmt*s1=_tmp9E;
# 353
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);Cyc_Binding_resolve_exp(env,e3);
Cyc_Binding_resolve_stmt(env,s1);
return;}case 6: _LL11: _LL12:
 goto _LL14;case 7: _LL13: _LL14:
 goto _LL16;case 8: _LL15: _LL16:
 return;case 11: _LL17: _tmp9A=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL18: {struct Cyc_List_List*es=_tmp9A;
# 360
for(1;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 13: _LL19: _tmp99=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1A: {struct Cyc_Absyn_Stmt*s1=_tmp99;
Cyc_Binding_resolve_stmt(env,s1);return;}case 12: _LL1B: _tmp97=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp98=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp97;struct Cyc_Absyn_Stmt*s1=_tmp98;
# 365
struct Cyc_Dict_Dict _tmpAA=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmpAA;
Cyc_Binding_resolve_decl(env,d);
Cyc_Binding_resolve_stmt(env,s1);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;
return;}case 10: _LL1D: _tmp95=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp96=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp95;struct Cyc_List_List*scs=_tmp96;
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_scs(env,scs);return;}default: _LL1F: _tmp93=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp94=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp93;struct Cyc_List_List*scs=_tmp94;
Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_scs(env,scs);return;}}_LL0:;}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 374
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpAB=e->r;void*_stmttmp8=_tmpAB;void*_tmpAC=_stmttmp8;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct _fat_ptr _tmpAE;int _tmpAD;struct Cyc_List_List*_tmpB3;void*_tmpB2;struct Cyc_Absyn_Datatypefield*_tmpB6;struct Cyc_Absyn_Datatypedecl*_tmpB5;struct Cyc_List_List*_tmpB4;struct Cyc_Absyn_Enumfield*_tmpB8;struct Cyc_Absyn_Enumdecl*_tmpB7;struct Cyc_Absyn_Enumfield*_tmpBA;void*_tmpB9;struct Cyc_List_List*_tmpBB;struct Cyc_List_List*_tmpBD;void*_tmpBC;void*_tmpBE;void*_tmpBF;void*_tmpC0;struct Cyc_Absyn_Stmt*_tmpC1;struct Cyc_Absyn_Exp*_tmpC3;void*_tmpC2;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCD;void**_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*_tmpE6;void*_tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_List_List*_tmpE9;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Vardecl*_tmpEA;struct Cyc_Absyn_Aggrdecl**_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*_tmpEE;struct _tuple0**_tmpED;int*_tmpF3;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;void**_tmpF4;switch(*((int*)_tmpAC)){case 1: _LL1: _tmpF4=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL2: {void**b=_tmpF4;
# 377
void*_tmpF5=*b;void*_stmttmp9=_tmpF5;void*_tmpF6=_stmttmp9;struct _tuple0*_tmpF7;if(*((int*)_tmpF6)== 0){_LL58: _tmpF7=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF6)->f1;_LL59: {struct _tuple0*q=_tmpF7;
# 379
struct _handler_cons _tmpF8;_push_handler(& _tmpF8);{int _tmpFA=0;if(setjmp(_tmpF8.handler))_tmpFA=1;if(!_tmpFA){{void*_tmpFB=Cyc_Binding_lookup_ordinary(e->loc,env,q);void*_stmttmpA=_tmpFB;void*_tmpFC=_stmttmpA;struct Cyc_Absyn_Aggrdecl*_tmpFD;struct Cyc_Absyn_Enumfield*_tmpFF;void*_tmpFE;struct Cyc_Absyn_Enumfield*_tmp101;struct Cyc_Absyn_Enumdecl*_tmp100;struct Cyc_Absyn_Datatypefield*_tmp103;struct Cyc_Absyn_Datatypedecl*_tmp102;void*_tmp104;switch(*((int*)_tmpFC)){case 0: _LL5D: _tmp104=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpFC)->f1;_LL5E: {void*bnd=_tmp104;
*b=bnd;_npop_handler(0);return;}case 2: _LL5F: _tmp102=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpFC)->f1;_tmp103=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpFC)->f2;_LL60: {struct Cyc_Absyn_Datatypedecl*tud=_tmp102;struct Cyc_Absyn_Datatypefield*tuf=_tmp103;
({void*_tmp34E=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=31,_tmp105->f1=0,_tmp105->f2=tud,_tmp105->f3=tuf;_tmp105;});e->r=_tmp34E;});_npop_handler(0);return;}case 3: _LL61: _tmp100=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpFC)->f1;_tmp101=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpFC)->f2;_LL62: {struct Cyc_Absyn_Enumdecl*ed=_tmp100;struct Cyc_Absyn_Enumfield*ef=_tmp101;
({void*_tmp34F=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=32,_tmp106->f1=ed,_tmp106->f2=ef;_tmp106;});e->r=_tmp34F;});_npop_handler(0);return;}case 4: _LL63: _tmpFE=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpFC)->f1;_tmpFF=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpFC)->f2;_LL64: {void*t=_tmpFE;struct Cyc_Absyn_Enumfield*ef=_tmpFF;
({void*_tmp350=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->tag=33,_tmp107->f1=t,_tmp107->f2=ef;_tmp107;});e->r=_tmp350;});_npop_handler(0);return;}default: _LL65: _tmpFD=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpFC)->f1;_LL66: {struct Cyc_Absyn_Aggrdecl*ad=_tmpFD;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp109=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC;_tmp2EC.tag=0,({struct _fat_ptr _tmp351=({const char*_tmp10B="bad occurrence of type name ";_tag_fat(_tmp10B,sizeof(char),29U);});_tmp2EC.f1=_tmp351;});_tmp2EC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2EB;_tmp2EB.tag=1,_tmp2EB.f1=q;_tmp2EB;});void*_tmp108[2];_tmp108[0]=& _tmp109,_tmp108[1]=& _tmp10A;({unsigned _tmp352=e->loc;Cyc_Warn_err2(_tmp352,_tag_fat(_tmp108,sizeof(void*),2));});});_npop_handler(0);return;}}_LL5C:;}
# 379
;_pop_handler();}else{void*_tmpF9=(void*)Cyc_Core_get_exn_thrown();void*_tmp10C=_tmpF9;void*_tmp10D;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp10C)->tag == Cyc_Binding_BindingError){_LL68: _LL69:
# 387
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE;_tmp2EE.tag=0,({struct _fat_ptr _tmp353=({const char*_tmp111="undeclared identifier ";_tag_fat(_tmp111,sizeof(char),23U);});_tmp2EE.f1=_tmp353;});_tmp2EE;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp110=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2ED;_tmp2ED.tag=1,_tmp2ED.f1=q;_tmp2ED;});void*_tmp10E[2];_tmp10E[0]=& _tmp10F,_tmp10E[1]=& _tmp110;({unsigned _tmp354=e->loc;Cyc_Warn_err2(_tmp354,_tag_fat(_tmp10E,sizeof(void*),2));});});return;}else{_LL6A: _tmp10D=_tmp10C;_LL6B: {void*exn=_tmp10D;(int)_rethrow(exn);}}_LL67:;}}}}else{_LL5A: _LL5B:
# 389
 return;}_LL57:;}case 10: _LL3: _tmpF1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpF2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpF3=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmpF1;struct Cyc_List_List*es=_tmpF2;int*b=_tmpF3;
# 392
*b=1;
# 394
{struct Cyc_List_List*_tmp112=es;struct Cyc_List_List*es2=_tmp112;for(0;es2 != 0;es2=es2->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es2->hd);}}{
void*_tmp113=e1->r;void*_stmttmpB=_tmp113;void*_tmp114=_stmttmpB;void**_tmp115;if(*((int*)_tmp114)== 1){_LL6D: _tmp115=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp114)->f1;_LL6E: {void**b=_tmp115;
# 398
void*_tmp116=*b;void*_stmttmpC=_tmp116;void*_tmp117=_stmttmpC;struct _tuple0*_tmp118;if(*((int*)_tmp117)== 0){_LL72: _tmp118=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp117)->f1;_LL73: {struct _tuple0*q=_tmp118;
# 400
struct _handler_cons _tmp119;_push_handler(& _tmp119);{int _tmp11B=0;if(setjmp(_tmp119.handler))_tmp11B=1;if(!_tmp11B){{void*_tmp11C=Cyc_Binding_lookup_ordinary(e1->loc,env,q);void*_stmttmpD=_tmp11C;void*_tmp11D=_stmttmpD;struct Cyc_Absyn_Datatypefield*_tmp11F;struct Cyc_Absyn_Datatypedecl*_tmp11E;struct Cyc_Absyn_Aggrdecl*_tmp120;void*_tmp121;switch(*((int*)_tmp11D)){case 0: _LL77: _tmp121=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp11D)->f1;_LL78: {void*bnd=_tmp121;
# 402
*b=bnd;_npop_handler(0);return;}case 1: _LL79: _tmp120=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp11D)->f1;_LL7A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp120;
# 404
struct Cyc_List_List*_tmp122=0;struct Cyc_List_List*dles=_tmp122;
for(1;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _tuple11*_tmp355=({struct _tuple11*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->f1=0,_tmp123->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp123;});_tmp124->hd=_tmp355;}),_tmp124->tl=dles;_tmp124;});}
({void*_tmp357=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=29,_tmp125->f1=ad->name,_tmp125->f2=0,({struct Cyc_List_List*_tmp356=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles);_tmp125->f3=_tmp356;}),_tmp125->f4=ad;_tmp125;});e->r=_tmp357;});
_npop_handler(0);return;}case 2: _LL7B: _tmp11E=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp11D)->f1;_tmp11F=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp11D)->f2;_LL7C: {struct Cyc_Absyn_Datatypedecl*tud=_tmp11E;struct Cyc_Absyn_Datatypefield*tuf=_tmp11F;
# 410
if(tuf->typs == 0)
# 412
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp127=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F0;_tmp2F0.tag=1,_tmp2F0.f1=tuf->name;_tmp2F0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp128=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF;_tmp2EF.tag=0,({struct _fat_ptr _tmp358=({const char*_tmp129=" is a constant, not a function";_tag_fat(_tmp129,sizeof(char),31U);});_tmp2EF.f1=_tmp358;});_tmp2EF;});void*_tmp126[2];_tmp126[0]=& _tmp127,_tmp126[1]=& _tmp128;({unsigned _tmp359=e->loc;Cyc_Warn_err2(_tmp359,_tag_fat(_tmp126,sizeof(void*),2));});});
({void*_tmp35A=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=31,_tmp12A->f1=es,_tmp12A->f2=tud,_tmp12A->f3=tuf;_tmp12A;});e->r=_tmp35A;});
_npop_handler(0);return;}case 4: _LL7D: _LL7E:
 goto _LL80;default: _LL7F: _LL80:
# 417
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp12C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F2;_tmp2F2.tag=1,_tmp2F2.f1=q;_tmp2F2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1;_tmp2F1.tag=0,({struct _fat_ptr _tmp35B=({const char*_tmp12E=" is an enum constructor, not a function";_tag_fat(_tmp12E,sizeof(char),40U);});_tmp2F1.f1=_tmp35B;});_tmp2F1;});void*_tmp12B[2];_tmp12B[0]=& _tmp12C,_tmp12B[1]=& _tmp12D;({unsigned _tmp35C=e->loc;Cyc_Warn_err2(_tmp35C,_tag_fat(_tmp12B,sizeof(void*),2));});});_npop_handler(0);return;}_LL76:;}
# 400
;_pop_handler();}else{void*_tmp11A=(void*)Cyc_Core_get_exn_thrown();void*_tmp12F=_tmp11A;void*_tmp130;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp12F)->tag == Cyc_Binding_BindingError){_LL82: _LL83:
# 419
({struct Cyc_Warn_String_Warn_Warg_struct _tmp132=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F4;_tmp2F4.tag=0,({struct _fat_ptr _tmp35D=({const char*_tmp134="undeclared identifier ";_tag_fat(_tmp134,sizeof(char),23U);});_tmp2F4.f1=_tmp35D;});_tmp2F4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp133=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F3;_tmp2F3.tag=1,_tmp2F3.f1=q;_tmp2F3;});void*_tmp131[2];_tmp131[0]=& _tmp132,_tmp131[1]=& _tmp133;({unsigned _tmp35E=e->loc;Cyc_Warn_err2(_tmp35E,_tag_fat(_tmp131,sizeof(void*),2));});});return;}else{_LL84: _tmp130=_tmp12F;_LL85: {void*exn=_tmp130;(int)_rethrow(exn);}}_LL81:;}}}}else{_LL74: _LL75:
# 421
 return;}_LL71:;}}else{_LL6F: _LL70:
# 423
 Cyc_Binding_resolve_exp(env,e1);return;}_LL6C:;}}case 29: _LL5: _tmpED=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpEE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpEF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpF0=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL6: {struct _tuple0**tn=_tmpED;struct Cyc_List_List*ts=_tmpEE;struct Cyc_List_List*dles=_tmpEF;struct Cyc_Absyn_Aggrdecl**adopt=_tmpF0;
# 427
for(1;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}{
struct _handler_cons _tmp135;_push_handler(& _tmp135);{int _tmp137=0;if(setjmp(_tmp135.handler))_tmp137=1;if(!_tmp137){
({struct Cyc_Absyn_Aggrdecl*_tmp35F=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*tn,Cyc_Binding_lookup_aggrdecl);*adopt=_tmp35F;});
*tn=((struct Cyc_Absyn_Aggrdecl*)_check_null(*adopt))->name;
_npop_handler(0);return;
# 430
;_pop_handler();}else{void*_tmp136=(void*)Cyc_Core_get_exn_thrown();void*_tmp138=_tmp136;void*_tmp139;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp138)->tag == Cyc_Binding_BindingError){_LL87: _LL88:
# 434
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F6;_tmp2F6.tag=0,({struct _fat_ptr _tmp360=({const char*_tmp13D="unbound struct/union name ";_tag_fat(_tmp13D,sizeof(char),27U);});_tmp2F6.f1=_tmp360;});_tmp2F6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp13C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F5;_tmp2F5.tag=1,_tmp2F5.f1=*tn;_tmp2F5;});void*_tmp13A[2];_tmp13A[0]=& _tmp13B,_tmp13A[1]=& _tmp13C;({unsigned _tmp361=e->loc;Cyc_Warn_err2(_tmp361,_tag_fat(_tmp13A,sizeof(void*),2));});});
return;}else{_LL89: _tmp139=_tmp138;_LL8A: {void*exn=_tmp139;(int)_rethrow(exn);}}_LL86:;}}}}case 27: _LL7: _tmpEA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpEB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpEC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpEA;struct Cyc_Absyn_Exp*e1=_tmpEB;struct Cyc_Absyn_Exp*e2=_tmpEC;
# 439
Cyc_Binding_resolve_exp(env,e1);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp363=({struct Cyc_Dict_Dict*_tmp13E=_cycalloc(sizeof(*_tmp13E));({struct Cyc_Dict_Dict _tmp362=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp13E=_tmp362;});_tmp13E;});env->local_vars=_tmp363;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
env->local_vars=0;
return;}{
# 447
struct Cyc_Dict_Dict _tmp13F=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmp13F;
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;
return;}}case 36: _LL9: _tmpE9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLA: {struct Cyc_List_List*dles=_tmpE9;
# 455
for(1;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}case 28: _LLB: _tmpE7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpE8=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmpE7;void*t=_tmpE8;
# 461
Cyc_Binding_resolve_exp(env,e1);
Cyc_Binding_resolve_type(e->loc,env,t);
return;}case 2: _LLD: _LLE:
# 465
 goto _LL10;case 0: _LLF: _LL10:
 return;case 24: _LL11: _tmpE6=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL12: {struct Cyc_List_List*es=_tmpE6;
# 468
_tmpE5=es;goto _LL14;}case 3: _LL13: _tmpE5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL14: {struct Cyc_List_List*es=_tmpE5;
# 470
for(1;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 42: _LL15: _tmpE4=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpE4;
# 474
_tmpE3=e1;goto _LL18;}case 41: _LL17: _tmpE3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpE3;
_tmpE2=e1;goto _LL1A;}case 38: _LL19: _tmpE2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpE2;
_tmpE1=e1;goto _LL1C;}case 12: _LL1B: _tmpE1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpE1;
_tmpE0=e1;goto _LL1E;}case 18: _LL1D: _tmpE0=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpE0;
_tmpDF=e1;goto _LL20;}case 11: _LL1F: _tmpDF=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmpDF;
_tmpDE=e1;goto _LL22;}case 5: _LL21: _tmpDE=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmpDE;
_tmpDD=e1;goto _LL24;}case 22: _LL23: _tmpDD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpDD;
_tmpDC=e1;goto _LL26;}case 21: _LL25: _tmpDC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmpDC;
_tmpDB=e1;goto _LL28;}case 15: _LL27: _tmpDB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpDB;
_tmpDA=e1;goto _LL2A;}case 20: _LL29: _tmpDA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpDA;
Cyc_Binding_resolve_exp(env,e1);return;}case 35: _LL2B: _tmpD8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpD8;struct Cyc_Absyn_Exp*e2=_tmpD9;
# 486
_tmpD6=e1;_tmpD7=e2;goto _LL2E;}case 9: _LL2D: _tmpD6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpD6;struct Cyc_Absyn_Exp*e2=_tmpD7;
_tmpD4=e1;_tmpD5=e2;goto _LL30;}case 4: _LL2F: _tmpD4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL30: {struct Cyc_Absyn_Exp*e1=_tmpD4;struct Cyc_Absyn_Exp*e2=_tmpD5;
_tmpD2=e1;_tmpD3=e2;goto _LL32;}case 23: _LL31: _tmpD2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpD2;struct Cyc_Absyn_Exp*e2=_tmpD3;
_tmpD0=e1;_tmpD1=e2;goto _LL34;}case 7: _LL33: _tmpD0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpD0;struct Cyc_Absyn_Exp*e2=_tmpD1;
_tmpCE=e1;_tmpCF=e2;goto _LL36;}case 8: _LL35: _tmpCE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpCE;struct Cyc_Absyn_Exp*e2=_tmpCF;
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);return;}case 34: _LL37: _tmpCB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).rgn;_tmpCC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).elt_type;_tmpCD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).num_elts;_LL38: {struct Cyc_Absyn_Exp*eo=_tmpCB;void**to=_tmpCC;struct Cyc_Absyn_Exp*e1=_tmpCD;
# 494
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
if(to != 0)Cyc_Binding_resolve_type(e->loc,env,*to);
Cyc_Binding_resolve_exp(env,e1);
return;}case 16: _LL39: _tmpC9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL3A: {struct Cyc_Absyn_Exp*eo=_tmpC9;struct Cyc_Absyn_Exp*e2=_tmpCA;
# 500
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
Cyc_Binding_resolve_exp(env,e2);
return;}case 6: _LL3B: _tmpC6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpC8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmpC6;struct Cyc_Absyn_Exp*e2=_tmpC7;struct Cyc_Absyn_Exp*e3=_tmpC8;
# 505
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);Cyc_Binding_resolve_exp(env,e3);return;}case 13: _LL3D: _tmpC4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmpC4;struct Cyc_List_List*ts=_tmpC5;
# 508
Cyc_Binding_resolve_exp(env,e1);
for(1;ts != 0;ts=ts->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)ts->hd);}
return;}case 14: _LL3F: _tmpC2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL40: {void*t=_tmpC2;struct Cyc_Absyn_Exp*e1=_tmpC3;
# 514
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_type(e->loc,env,t);return;}case 37: _LL41: _tmpC1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL42: {struct Cyc_Absyn_Stmt*s=_tmpC1;
# 516
Cyc_Binding_resolve_stmt(env,s);return;}case 39: _LL43: _tmpC0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL44: {void*t=_tmpC0;
# 518
_tmpBF=t;goto _LL46;}case 19: _LL45: _tmpBF=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL46: {void*t=_tmpBF;
_tmpBE=t;goto _LL48;}case 17: _LL47: _tmpBE=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL48: {void*t=_tmpBE;
Cyc_Binding_resolve_type(e->loc,env,t);return;}case 25: _LL49: _tmpBC=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1)->f3;_tmpBD=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4A: {void*t=_tmpBC;struct Cyc_List_List*dles=_tmpBD;
# 523
Cyc_Binding_resolve_type(e->loc,env,t);
_tmpBB=dles;goto _LL4C;}case 26: _LL4B: _tmpBB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL4C: {struct Cyc_List_List*dles=_tmpBB;
# 527
for(1;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}case 33: _LL4D: _tmpB9=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpBA=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4E: {void*t=_tmpB9;struct Cyc_Absyn_Enumfield*ef=_tmpBA;
# 532
Cyc_Binding_resolve_type(e->loc,env,t);return;}case 32: _LL4F: _tmpB7=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL50: {struct Cyc_Absyn_Enumdecl*ed=_tmpB7;struct Cyc_Absyn_Enumfield*ef=_tmpB8;
return;}case 31: _LL51: _tmpB4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpB6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL52: {struct Cyc_List_List*es=_tmpB4;struct Cyc_Absyn_Datatypedecl*tud=_tmpB5;struct Cyc_Absyn_Datatypefield*tuf=_tmpB6;
# 535
for(1;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 30: _LL53: _tmpB2=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL54: {void*t=_tmpB2;struct Cyc_List_List*dles=_tmpB3;
# 539
Cyc_Binding_resolve_type(e->loc,env,t);
for(1;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}default: _LL55: _tmpAD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpAE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpAF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpB0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_tmpB1=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f5;_LL56: {int v=_tmpAD;struct _fat_ptr t=_tmpAE;struct Cyc_List_List*o=_tmpAF;struct Cyc_List_List*i=_tmpB0;struct Cyc_List_List*c=_tmpB1;
# 545
Cyc_Binding_resolve_asm_iolist(env,o);
Cyc_Binding_resolve_asm_iolist(env,i);
goto _LL0;}}_LL0:;}struct _tuple12{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 551
void Cyc_Binding_resolve_asm_iolist(struct Cyc_Binding_Env*env,struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple12*_tmp140=(struct _tuple12*)l->hd;struct _tuple12*_stmttmpE=_tmp140;struct _tuple12*_tmp141=_stmttmpE;struct Cyc_Absyn_Exp*_tmp142;_LL1: _tmp142=_tmp141->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp142;
Cyc_Binding_resolve_exp(env,e);
l=l->tl;}}}
# 559
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp143=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmp143;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp144=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpF=_tmp144;struct Cyc_Absyn_Switch_clause*_tmp145=_stmttmpF;struct Cyc_Absyn_Stmt*_tmp148;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Pat*_tmp146;_LL1: _tmp146=_tmp145->pattern;_tmp147=_tmp145->where_clause;_tmp148=_tmp145->body;_LL2: {struct Cyc_Absyn_Pat*pattern=_tmp146;struct Cyc_Absyn_Exp*where_clause=_tmp147;struct Cyc_Absyn_Stmt*body=_tmp148;
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
struct Cyc_Absyn_Aggrfield*_tmp149=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp10=_tmp149;struct Cyc_Absyn_Aggrfield*_tmp14A=_stmttmp10;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_List_List*_tmp14F;struct Cyc_Absyn_Exp*_tmp14E;void*_tmp14D;struct Cyc_Absyn_Tqual _tmp14C;struct _fat_ptr*_tmp14B;_LL1: _tmp14B=_tmp14A->name;_tmp14C=_tmp14A->tq;_tmp14D=_tmp14A->type;_tmp14E=_tmp14A->width;_tmp14F=_tmp14A->attributes;_tmp150=_tmp14A->requires_clause;_LL2: {struct _fat_ptr*fn=_tmp14B;struct Cyc_Absyn_Tqual tq=_tmp14C;void*t=_tmp14D;struct Cyc_Absyn_Exp*width=_tmp14E;struct Cyc_List_List*atts=_tmp14F;struct Cyc_Absyn_Exp*requires_clause=_tmp150;
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
struct Cyc_List_List*_tmp151=0;struct Cyc_List_List*vds=_tmp151;
for(1;args != 0;args=args->tl){
struct _tuple8*_tmp152=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp11=_tmp152;struct _tuple8*_tmp153=_stmttmp11;void*_tmp156;struct Cyc_Absyn_Tqual _tmp155;struct _fat_ptr*_tmp154;_LL1: _tmp154=_tmp153->f1;_tmp155=_tmp153->f2;_tmp156=_tmp153->f3;_LL2: {struct _fat_ptr*a=_tmp154;struct Cyc_Absyn_Tqual tq=_tmp155;void*t=_tmp156;
if(a == 0)
continue;{
struct Cyc_Absyn_Vardecl*_tmp157=({struct _tuple0*_tmp364=({struct _tuple0*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->f1=Cyc_Absyn_Loc_n,_tmp159->f2=a;_tmp159;});Cyc_Absyn_new_vardecl(0U,_tmp364,t,0);});struct Cyc_Absyn_Vardecl*vd=_tmp157;
vd->tq=tq;
vds=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->hd=vd,_tmp158->tl=vds;_tmp158;});}}}
# 598
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp15A=*vai;struct Cyc_Absyn_VarargInfo _stmttmp12=_tmp15A;struct Cyc_Absyn_VarargInfo _tmp15B=_stmttmp12;int _tmp15F;void*_tmp15E;struct Cyc_Absyn_Tqual _tmp15D;struct _fat_ptr*_tmp15C;_LL4: _tmp15C=_tmp15B.name;_tmp15D=_tmp15B.tq;_tmp15E=_tmp15B.type;_tmp15F=_tmp15B.inject;_LL5: {struct _fat_ptr*v=_tmp15C;struct Cyc_Absyn_Tqual tq=_tmp15D;void*t=_tmp15E;int i=_tmp15F;
if(v == 0){
if(need_va_name)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp161=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F7;_tmp2F7.tag=0,({struct _fat_ptr _tmp365=({const char*_tmp162="missing name for vararg";_tag_fat(_tmp162,sizeof(char),24U);});_tmp2F7.f1=_tmp365;});_tmp2F7;});void*_tmp160[1];_tmp160[0]=& _tmp161;({unsigned _tmp366=loc;Cyc_Warn_err2(_tmp366,_tag_fat(_tmp160,sizeof(void*),1));});});}else{
# 605
void*_tmp163=Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);void*typ=_tmp163;
struct Cyc_Absyn_Vardecl*_tmp164=({struct _tuple0*_tmp367=({struct _tuple0*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->f1=Cyc_Absyn_Loc_n,_tmp166->f2=v;_tmp166;});Cyc_Absyn_new_vardecl(0U,_tmp367,typ,0);});struct Cyc_Absyn_Vardecl*vd=_tmp164;
vds=({struct Cyc_List_List*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->hd=vd,_tmp165->tl=vds;_tmp165;});}}}
# 610
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(vds);
return vds;}
# 614
static void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo*i){
if(i->effect != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(i->effect));
Cyc_Binding_resolve_type(loc,env,i->ret_type);
{struct Cyc_List_List*_tmp167=i->args;struct Cyc_List_List*args=_tmp167;for(0;args != 0;args=args->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple8*)args->hd)).f3);}}
if(i->cyc_varargs != 0)
Cyc_Binding_resolve_type(loc,env,((struct Cyc_Absyn_VarargInfo*)_check_null(i->cyc_varargs))->type);
Cyc_Binding_resolve_rgnpo(loc,env,i->rgn_po);
if(i->requires_clause != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(i->requires_clause));
if(i->ensures_clause != 0){
# 627
struct Cyc_Dict_Dict _tmp168=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict locs=_tmp168;
struct _fat_ptr*_tmp169=({struct _fat_ptr*_tmp16F=_cycalloc(sizeof(*_tmp16F));({struct _fat_ptr _tmp368=({const char*_tmp16E="return_value";_tag_fat(_tmp16E,sizeof(char),13U);});*_tmp16F=_tmp368;});_tmp16F;});struct _fat_ptr*v=_tmp169;
struct Cyc_Absyn_Vardecl*_tmp16A=({struct _tuple0*_tmp369=({struct _tuple0*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->f1=Cyc_Absyn_Loc_n,_tmp16D->f2=v;_tmp16D;});Cyc_Absyn_new_vardecl(0U,_tmp369,i->ret_type,0);});struct Cyc_Absyn_Vardecl*vd=_tmp16A;
i->return_value=vd;
({struct Cyc_Dict_Dict _tmp36D=({struct Cyc_Dict_Dict _tmp36C=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp36B=v;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp36C,_tmp36B,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));
_tmp16C->tag=0,({void*_tmp36A=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=4,_tmp16B->f1=vd;_tmp16B;});_tmp16C->f1=_tmp36A;});_tmp16C;}));});
# 631
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp36D;});
# 633
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(i->ensures_clause));
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=locs;}}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
# 643
void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp170=t;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_FnInfo*_tmp173;struct Cyc_Absyn_Exp*_tmp175;void*_tmp174;struct Cyc_List_List*_tmp176;void*_tmp179;void*_tmp178;void*_tmp177;void*_tmp17A;struct Cyc_List_List*_tmp17B;void***_tmp17D;struct Cyc_Absyn_TypeDecl*_tmp17C;struct Cyc_Absyn_Typedefdecl**_tmp180;struct Cyc_List_List*_tmp17F;struct _tuple0**_tmp17E;struct Cyc_List_List*_tmp182;void*_tmp181;struct Cyc_List_List*_tmp183;struct Cyc_Absyn_Enumdecl*_tmp185;struct _tuple0**_tmp184;struct Cyc_List_List*_tmp187;union Cyc_Absyn_AggrInfo*_tmp186;struct Cyc_List_List*_tmp189;union Cyc_Absyn_DatatypeFieldInfo*_tmp188;struct Cyc_List_List*_tmp18B;union Cyc_Absyn_DatatypeInfo*_tmp18A;switch(*((int*)_tmp170)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)){case 18: _LL1: _tmp18A=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)->f1;_tmp18B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f2;_LL2: {union Cyc_Absyn_DatatypeInfo*info=_tmp18A;struct Cyc_List_List*targs=_tmp18B;
# 646
for(1;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeInfo _tmp18C=*info;union Cyc_Absyn_DatatypeInfo _stmttmp13=_tmp18C;union Cyc_Absyn_DatatypeInfo _tmp18D=_stmttmp13;int _tmp18F;struct _tuple0*_tmp18E;if((_tmp18D.UnknownDatatype).tag == 1){_LL26: _tmp18E=((_tmp18D.UnknownDatatype).val).name;_tmp18F=((_tmp18D.UnknownDatatype).val).is_extensible;_LL27: {struct _tuple0*qv=_tmp18E;int b=_tmp18F;
# 650
struct _handler_cons _tmp190;_push_handler(& _tmp190);{int _tmp192=0;if(setjmp(_tmp190.handler))_tmp192=1;if(!_tmp192){
{struct Cyc_Absyn_Datatypedecl*_tmp193=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_datatypedecl))->hd;struct Cyc_Absyn_Datatypedecl*tud=_tmp193;
({union Cyc_Absyn_DatatypeInfo _tmp36E=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp2F8;_tmp2F8.name=tud->name,_tmp2F8.is_extensible=b;_tmp2F8;}));*info=_tmp36E;});
_npop_handler(0);return;}
# 651
;_pop_handler();}else{void*_tmp191=(void*)Cyc_Core_get_exn_thrown();void*_tmp194=_tmp191;void*_tmp195;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp194)->tag == Cyc_Binding_BindingError){_LL2B: _LL2C:
# 655
 Cyc_Binding_absolutize_topdecl(loc,env,qv,Cyc_Absyn_Public);return;}else{_LL2D: _tmp195=_tmp194;_LL2E: {void*exn=_tmp195;(int)_rethrow(exn);}}_LL2A:;}}}}else{_LL28: _LL29:
 return;}_LL25:;}}case 19: _LL3: _tmp188=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)->f1;_tmp189=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f2;_LL4: {union Cyc_Absyn_DatatypeFieldInfo*info=_tmp188;struct Cyc_List_List*targs=_tmp189;
# 659
for(1;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeFieldInfo _tmp196=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmp14=_tmp196;union Cyc_Absyn_DatatypeFieldInfo _tmp197=_stmttmp14;int _tmp19A;struct _tuple0*_tmp199;struct _tuple0*_tmp198;if((_tmp197.UnknownDatatypefield).tag == 1){_LL30: _tmp198=((_tmp197.UnknownDatatypefield).val).datatype_name;_tmp199=((_tmp197.UnknownDatatypefield).val).field_name;_tmp19A=((_tmp197.UnknownDatatypefield).val).is_extensible;_LL31: {struct _tuple0*qvd=_tmp198;struct _tuple0*qvf=_tmp199;int b=_tmp19A;
# 665
{struct _handler_cons _tmp19B;_push_handler(& _tmp19B);{int _tmp19D=0;if(setjmp(_tmp19B.handler))_tmp19D=1;if(!_tmp19D){
{void*_tmp19E=Cyc_Binding_lookup_ordinary(loc,env,qvf);void*_stmttmp15=_tmp19E;void*_tmp19F=_stmttmp15;struct Cyc_Absyn_Datatypefield*_tmp1A1;struct Cyc_Absyn_Datatypedecl*_tmp1A0;if(*((int*)_tmp19F)== 2){_LL35: _tmp1A0=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp19F)->f1;_tmp1A1=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp19F)->f2;_LL36: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1A0;struct Cyc_Absyn_Datatypefield*tuf=_tmp1A1;
# 668
struct Cyc_Absyn_Datatypedecl*_tmp1A2=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qvd,Cyc_Binding_lookup_datatypedecl))->hd;struct Cyc_Absyn_Datatypedecl*tud2=_tmp1A2;
if(Cyc_Absyn_qvar_cmp(tud->name,tud2->name)!= 0){
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FE;_tmp2FE.tag=1,_tmp2FE.f1=tuf->name;_tmp2FE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FD;_tmp2FD.tag=0,({struct _fat_ptr _tmp36F=({const char*_tmp1AC=" is a variant of ";_tag_fat(_tmp1AC,sizeof(char),18U);});_tmp2FD.f1=_tmp36F;});_tmp2FD;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FC;_tmp2FC.tag=1,_tmp2FC.f1=tud2->name;_tmp2FC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FB;_tmp2FB.tag=0,({struct _fat_ptr _tmp370=({const char*_tmp1AB=" not ";_tag_fat(_tmp1AB,sizeof(char),6U);});_tmp2FB.f1=_tmp370;});_tmp2FB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A8=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FA;_tmp2FA.tag=1,_tmp2FA.f1=tud->name;_tmp2FA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F9;_tmp2F9.tag=0,({
struct _fat_ptr _tmp371=({const char*_tmp1AA=" (shadowing not yet implemented properly)";_tag_fat(_tmp1AA,sizeof(char),42U);});_tmp2F9.f1=_tmp371;});_tmp2F9;});void*_tmp1A3[6];_tmp1A3[0]=& _tmp1A4,_tmp1A3[1]=& _tmp1A5,_tmp1A3[2]=& _tmp1A6,_tmp1A3[3]=& _tmp1A7,_tmp1A3[4]=& _tmp1A8,_tmp1A3[5]=& _tmp1A9;({unsigned _tmp372=loc;Cyc_Warn_err2(_tmp372,_tag_fat(_tmp1A3,sizeof(void*),6));});});
_npop_handler(0);return;}
# 674
({union Cyc_Absyn_DatatypeFieldInfo _tmp373=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp2FF;_tmp2FF.datatype_name=tud->name,_tmp2FF.field_name=tuf->name,_tmp2FF.is_extensible=b;_tmp2FF;}));*info=_tmp373;});
# 676
_npop_handler(0);return;}}else{_LL37: _LL38:
 goto _LL34;}_LL34:;}
# 666
;_pop_handler();}else{void*_tmp19C=(void*)Cyc_Core_get_exn_thrown();void*_tmp1AD=_tmp19C;void*_tmp1AE;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1AD)->tag == Cyc_Binding_BindingError){_LL3A: _LL3B:
# 679
 goto _LL39;}else{_LL3C: _tmp1AE=_tmp1AD;_LL3D: {void*exn=_tmp1AE;(int)_rethrow(exn);}}_LL39:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp301;_tmp301.tag=1,_tmp301.f1=qvf;_tmp301;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp300;_tmp300.tag=0,({struct _fat_ptr _tmp374=({const char*_tmp1B2=" is not a datatype field";_tag_fat(_tmp1B2,sizeof(char),25U);});_tmp300.f1=_tmp374;});_tmp300;});void*_tmp1AF[2];_tmp1AF[0]=& _tmp1B0,_tmp1AF[1]=& _tmp1B1;({unsigned _tmp375=loc;Cyc_Warn_err2(_tmp375,_tag_fat(_tmp1AF,sizeof(void*),2));});});return;}}else{_LL32: _LL33:
 return;}_LL2F:;}}case 20: _LL5: _tmp186=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)->f1;_tmp187=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f2;_LL6: {union Cyc_Absyn_AggrInfo*info=_tmp186;struct Cyc_List_List*targs=_tmp187;
# 685
for(1;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_AggrInfo _tmp1B3=*info;union Cyc_Absyn_AggrInfo _stmttmp16=_tmp1B3;union Cyc_Absyn_AggrInfo _tmp1B4=_stmttmp16;struct Cyc_Core_Opt*_tmp1B7;struct _tuple0*_tmp1B6;enum Cyc_Absyn_AggrKind _tmp1B5;if((_tmp1B4.UnknownAggr).tag == 1){_LL3F: _tmp1B5=((_tmp1B4.UnknownAggr).val).f1;_tmp1B6=((_tmp1B4.UnknownAggr).val).f2;_tmp1B7=((_tmp1B4.UnknownAggr).val).f3;_LL40: {enum Cyc_Absyn_AggrKind ak=_tmp1B5;struct _tuple0*qv=_tmp1B6;struct Cyc_Core_Opt*bo=_tmp1B7;
# 689
struct _handler_cons _tmp1B8;_push_handler(& _tmp1B8);{int _tmp1BA=0;if(setjmp(_tmp1B8.handler))_tmp1BA=1;if(!_tmp1BA){
{struct Cyc_Absyn_Aggrdecl*_tmp1BB=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_aggrdecl);struct Cyc_Absyn_Aggrdecl*ad=_tmp1BB;
if((int)ad->kind != (int)ak)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp302;_tmp302.tag=0,({struct _fat_ptr _tmp376=({const char*_tmp1BE="struct vs. union mismatch with earlier declaration";_tag_fat(_tmp1BE,sizeof(char),51U);});_tmp302.f1=_tmp376;});_tmp302;});void*_tmp1BC[1];_tmp1BC[0]=& _tmp1BD;({unsigned _tmp377=loc;Cyc_Warn_err2(_tmp377,_tag_fat(_tmp1BC,sizeof(void*),1));});});
if((((int)ak == (int)1U && bo != 0)&& ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged != (int)bo->v)
# 695
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp303;_tmp303.tag=0,({struct _fat_ptr _tmp378=({const char*_tmp1C1="@tagged mismatch with earlier declaration";_tag_fat(_tmp1C1,sizeof(char),42U);});_tmp303.f1=_tmp378;});_tmp303;});void*_tmp1BF[1];_tmp1BF[0]=& _tmp1C0;({unsigned _tmp379=loc;Cyc_Warn_err2(_tmp379,_tag_fat(_tmp1BF,sizeof(void*),1));});});
({union Cyc_Absyn_AggrInfo _tmp37A=Cyc_Absyn_UnknownAggr(ak,ad->name,bo);*info=_tmp37A;});
_npop_handler(0);return;}
# 690
;_pop_handler();}else{void*_tmp1B9=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C2=_tmp1B9;void*_tmp1C3;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1C2)->tag == Cyc_Binding_BindingError){_LL44: _LL45:
# 699
 Cyc_Binding_absolutize_topdecl(loc,env,qv,Cyc_Absyn_Public);return;}else{_LL46: _tmp1C3=_tmp1C2;_LL47: {void*exn=_tmp1C3;(int)_rethrow(exn);}}_LL43:;}}}}else{_LL41: _LL42:
 return;}_LL3E:;}}case 15: _LL7: _tmp184=(struct _tuple0**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)->f1;_tmp185=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)->f2;_LL8: {struct _tuple0**qv=_tmp184;struct Cyc_Absyn_Enumdecl*edo=_tmp185;
# 703
if(edo != 0)
return;{
struct _handler_cons _tmp1C4;_push_handler(& _tmp1C4);{int _tmp1C6=0;if(setjmp(_tmp1C4.handler))_tmp1C6=1;if(!_tmp1C6){
{struct Cyc_Absyn_Enumdecl*_tmp1C7=((struct Cyc_Absyn_Enumdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*qv,Cyc_Binding_lookup_enumdecl);struct Cyc_Absyn_Enumdecl*ed=_tmp1C7;
*qv=ed->name;
_npop_handler(0);return;}
# 706
;_pop_handler();}else{void*_tmp1C5=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C8=_tmp1C5;void*_tmp1C9;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1C8)->tag == Cyc_Binding_BindingError){_LL49: _LL4A:
# 710
 Cyc_Binding_absolutize_topdecl(loc,env,*qv,Cyc_Absyn_Public);return;}else{_LL4B: _tmp1C9=_tmp1C8;_LL4C: {void*exn=_tmp1C9;(int)_rethrow(exn);}}_LL48:;}}}}case 16: _LLF: _tmp183=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)->f1;_LL10: {struct Cyc_List_List*fs=_tmp183;
# 748
struct Cyc_Binding_ResolveNSEnv*_tmp1E0=({struct Cyc_Binding_NSCtxt*_tmp37C=env->ns;((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(_tmp37C,({struct Cyc_List_List*_tmp37B=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp37B,
Cyc_Binding_in_cinclude(env));}));});
# 748
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp1E0;
# 750
for(1;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1E1=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp1E1;
if(f->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
({struct Cyc_Dict_Dict _tmp37F=({struct Cyc_Dict_Dict _tmp37E=decl_ns_data->ordinaries;struct _fat_ptr*_tmp37D=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp37E,_tmp37D,(void*)({struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2->tag=4,_tmp1E2->f1=t,_tmp1E2->f2=f;_tmp1E2;}));});decl_ns_data->ordinaries=_tmp37F;});}
# 756
return;}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f2 == 0){_LL15: _LL16:
# 760
 return;}else{_LL17: _tmp181=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1;_tmp182=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f2;_LL18: {void*c=_tmp181;struct Cyc_List_List*ts=_tmp182;
# 762
for(1;ts != 0;ts=ts->tl){
Cyc_Binding_resolve_type(loc,env,(void*)ts->hd);}
return;}}}case 8: _LL9: _tmp17E=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp170)->f1;_tmp17F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp170)->f2;_tmp180=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp170)->f3;_LLA: {struct _tuple0**tdn=_tmp17E;struct Cyc_List_List*targs=_tmp17F;struct Cyc_Absyn_Typedefdecl**tdo=_tmp180;
# 713
for(1;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}
{struct _handler_cons _tmp1CA;_push_handler(& _tmp1CA);{int _tmp1CC=0;if(setjmp(_tmp1CA.handler))_tmp1CC=1;if(!_tmp1CC){
{struct Cyc_Absyn_Typedefdecl*_tmp1CD=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*tdn,Cyc_Binding_lookup_typedefdecl);struct Cyc_Absyn_Typedefdecl*td=_tmp1CD;
# 718
*tdn=td->name;
_npop_handler(0);return;}
# 716
;_pop_handler();}else{void*_tmp1CB=(void*)Cyc_Core_get_exn_thrown();void*_tmp1CE=_tmp1CB;void*_tmp1CF;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1CE)->tag == Cyc_Binding_BindingError){_LL4E: _LL4F:
# 720
 goto _LL4D;}else{_LL50: _tmp1CF=_tmp1CE;_LL51: {void*exn=_tmp1CF;(int)_rethrow(exn);}}_LL4D:;}}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp305;_tmp305.tag=0,({struct _fat_ptr _tmp380=({const char*_tmp1D3="unbound typdef name ";_tag_fat(_tmp1D3,sizeof(char),21U);});_tmp305.f1=_tmp380;});_tmp305;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1D2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp304;_tmp304.tag=1,_tmp304.f1=*tdn;_tmp304;});void*_tmp1D0[2];_tmp1D0[0]=& _tmp1D1,_tmp1D0[1]=& _tmp1D2;({unsigned _tmp381=loc;Cyc_Warn_err2(_tmp381,_tag_fat(_tmp1D0,sizeof(void*),2));});});
return;}case 10: _LLB: _tmp17C=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp170)->f1;_tmp17D=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp170)->f2;_LLC: {struct Cyc_Absyn_TypeDecl*td=_tmp17C;void***to=_tmp17D;
# 726
struct Cyc_Dict_Dict*_tmp1D4=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp1D4;
env->local_vars=0;
{void*_tmp1D5=td->r;void*_stmttmp17=_tmp1D5;void*_tmp1D6=_stmttmp17;struct Cyc_Absyn_Datatypedecl*_tmp1D7;struct Cyc_Absyn_Enumdecl*_tmp1D8;struct Cyc_Absyn_Aggrdecl*_tmp1D9;switch(*((int*)_tmp1D6)){case 0: _LL53: _tmp1D9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1D6)->f1;_LL54: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1D9;
# 730
({struct Cyc_Binding_Env*_tmp383=env;Cyc_Binding_resolve_decl(_tmp383,({struct Cyc_Absyn_Decl*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));({void*_tmp382=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->tag=5,_tmp1DA->f1=ad;_tmp1DA;});_tmp1DB->r=_tmp382;}),_tmp1DB->loc=td->loc;_tmp1DB;}));});goto _LL52;}case 1: _LL55: _tmp1D8=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1D6)->f1;_LL56: {struct Cyc_Absyn_Enumdecl*ed=_tmp1D8;
# 732
({struct Cyc_Binding_Env*_tmp385=env;Cyc_Binding_resolve_decl(_tmp385,({struct Cyc_Absyn_Decl*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));({void*_tmp384=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->tag=7,_tmp1DC->f1=ed;_tmp1DC;});_tmp1DD->r=_tmp384;}),_tmp1DD->loc=td->loc;_tmp1DD;}));});goto _LL52;}default: _LL57: _tmp1D7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1D6)->f1;_LL58: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1D7;
# 734
({struct Cyc_Binding_Env*_tmp387=env;Cyc_Binding_resolve_decl(_tmp387,({struct Cyc_Absyn_Decl*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));({void*_tmp386=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->tag=6,_tmp1DE->f1=tud;_tmp1DE;});_tmp1DF->r=_tmp386;}),_tmp1DF->loc=td->loc;_tmp1DF;}));});goto _LL52;}}_LL52:;}
# 736
env->local_vars=old_locals;
return;}case 7: _LLD: _tmp17B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp170)->f2;_LLE: {struct Cyc_List_List*fs=_tmp17B;
# 743
Cyc_Binding_resolve_aggrfields(loc,env,fs);
return;}case 1: _LL11: _tmp17A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp170)->f2;_LL12: {void*to=_tmp17A;
# 757
if(to != 0)Cyc_Binding_resolve_type(loc,env,to);return;}case 2: _LL13: _LL14:
# 759
 goto _LL16;case 3: _LL19: _tmp177=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).elt_type;_tmp178=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).ptr_atts).rgn;_tmp179=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).ptr_atts).bounds;_LL1A: {void*t1=_tmp177;void*t2=_tmp178;void*bds=_tmp179;
# 767
Cyc_Binding_resolve_type(loc,env,t1);
Cyc_Binding_resolve_type(loc,env,t2);
Cyc_Binding_resolve_type(loc,env,bds);
return;}case 6: _LL1B: _tmp176=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp170)->f1;_LL1C: {struct Cyc_List_List*tqts=_tmp176;
# 773
for(1;tqts != 0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)tqts->hd)).f2);}
return;}case 4: _LL1D: _tmp174=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp170)->f1).elt_type;_tmp175=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp170)->f1).num_elts;_LL1E: {void*t1=_tmp174;struct Cyc_Absyn_Exp*eo=_tmp175;
# 777
Cyc_Binding_resolve_type(loc,env,t1);
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
return;}case 5: _LL1F: _tmp173=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp170)->f1;_LL20: {struct Cyc_Absyn_FnInfo*i=_tmp173;
# 784
struct Cyc_List_List*_tmp1E3=Cyc_Binding_get_fun_vardecls(0,loc,env,i->args,i->cyc_varargs);struct Cyc_List_List*vds=_tmp1E3;
# 786
struct Cyc_Dict_Dict*_tmp1E4=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp1E4;
if(old_locals != 0)
({struct Cyc_Dict_Dict*_tmp388=({struct Cyc_Dict_Dict*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));*_tmp1E5=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1E5;});env->local_vars=_tmp388;});else{
# 790
({struct Cyc_Dict_Dict*_tmp38A=({struct Cyc_Dict_Dict*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));({struct Cyc_Dict_Dict _tmp389=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp1E6=_tmp389;});_tmp1E6;});env->local_vars=_tmp38A;});}
{struct Cyc_List_List*_tmp1E7=vds;struct Cyc_List_List*vds1=_tmp1E7;for(0;vds1 != 0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _tmp38E=({struct Cyc_Dict_Dict _tmp38D=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp38C=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp38D,_tmp38C,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));
_tmp1E9->tag=0,({void*_tmp38B=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->tag=3,_tmp1E8->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_tmp1E8;});_tmp1E9->f1=_tmp38B;});_tmp1E9;}));});
# 792
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp38E;});}}
# 795
Cyc_Binding_resolve_function_stuff(loc,env,i);
env->local_vars=old_locals;
return;}case 9: _LL21: _tmp172=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp170)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp172;
# 799
_tmp171=e;goto _LL24;}default: _LL23: _tmp171=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp170)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp171;
Cyc_Binding_resolve_exp(env,e);return;}}_LL0:;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 804
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1EA=p;unsigned _tmp1EC;void*_tmp1EB;_LL1: _tmp1EB=_tmp1EA->r;_tmp1EC=_tmp1EA->loc;_LL2: {void*r=_tmp1EB;unsigned loc=_tmp1EC;
void*_tmp1ED=r;struct Cyc_List_List*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_Absyn_Pat*_tmp1F1;struct Cyc_Absyn_Vardecl*_tmp1F2;struct Cyc_Absyn_Vardecl*_tmp1F3;struct Cyc_Absyn_Pat*_tmp1F5;struct Cyc_Absyn_Vardecl*_tmp1F4;struct Cyc_Absyn_Pat*_tmp1F7;struct Cyc_Absyn_Vardecl*_tmp1F6;struct Cyc_List_List*_tmp1F8;struct Cyc_List_List*_tmp1F9;int _tmp1FD;struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FB;struct _tuple0*_tmp1FA;int _tmp200;struct Cyc_List_List*_tmp1FF;struct _tuple0*_tmp1FE;struct _tuple0*_tmp201;switch(*((int*)_tmp1ED)){case 15: _LL4: _tmp201=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL5: {struct _tuple0*qv=_tmp201;
# 808
{struct _handler_cons _tmp202;_push_handler(& _tmp202);{int _tmp204=0;if(setjmp(_tmp202.handler))_tmp204=1;if(!_tmp204){{void*_tmp205=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_stmttmp18=_tmp205;void*_tmp206=_stmttmp18;struct Cyc_Absyn_Enumfield*_tmp208;void*_tmp207;struct Cyc_Absyn_Enumfield*_tmp20A;struct Cyc_Absyn_Enumdecl*_tmp209;struct Cyc_Absyn_Datatypefield*_tmp20C;struct Cyc_Absyn_Datatypedecl*_tmp20B;switch(*((int*)_tmp206)){case 0: _LL2D: _LL2E:
 goto _LL2C;case 1: _LL2F: _LL30:
# 811
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp306;_tmp306.tag=0,({struct _fat_ptr _tmp38F=({const char*_tmp20F="struct tag used without arguments in pattern";_tag_fat(_tmp20F,sizeof(char),45U);});_tmp306.f1=_tmp38F;});_tmp306;});void*_tmp20D[1];_tmp20D[0]=& _tmp20E;({unsigned _tmp390=loc;Cyc_Warn_err2(_tmp390,_tag_fat(_tmp20D,sizeof(void*),1));});});_npop_handler(0);return;case 2: _LL31: _tmp20B=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp206)->f1;_tmp20C=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp206)->f2;_LL32: {struct Cyc_Absyn_Datatypedecl*tud=_tmp20B;struct Cyc_Absyn_Datatypefield*tuf=_tmp20C;
# 813
({void*_tmp391=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->tag=8,_tmp210->f1=tud,_tmp210->f2=tuf,_tmp210->f3=0,_tmp210->f4=0;_tmp210;});p->r=_tmp391;});_npop_handler(0);return;}case 3: _LL33: _tmp209=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp206)->f1;_tmp20A=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp206)->f2;_LL34: {struct Cyc_Absyn_Enumdecl*ed=_tmp209;struct Cyc_Absyn_Enumfield*ef=_tmp20A;
# 815
({void*_tmp392=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->tag=13,_tmp211->f1=ed,_tmp211->f2=ef;_tmp211;});p->r=_tmp392;});_npop_handler(0);return;}default: _LL35: _tmp207=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp206)->f1;_tmp208=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp206)->f2;_LL36: {void*t=_tmp207;struct Cyc_Absyn_Enumfield*ef=_tmp208;
# 817
({void*_tmp393=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->tag=14,_tmp212->f1=t,_tmp212->f2=ef;_tmp212;});p->r=_tmp393;});_npop_handler(0);return;}}_LL2C:;}
# 808
;_pop_handler();}else{void*_tmp203=(void*)Cyc_Core_get_exn_thrown();void*_tmp213=_tmp203;void*_tmp214;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp213)->tag == Cyc_Binding_BindingError){_LL38: _LL39:
# 818
 goto _LL37;}else{_LL3A: _tmp214=_tmp213;_LL3B: {void*exn=_tmp214;(int)_rethrow(exn);}}_LL37:;}}}{
struct Cyc_Absyn_Vardecl*_tmp215=Cyc_Absyn_new_vardecl(0U,qv,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmp215;
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
({void*_tmp395=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->tag=1,_tmp216->f1=vd,({struct Cyc_Absyn_Pat*_tmp394=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp216->f2=_tmp394;});_tmp216;});p->r=_tmp395;});
return;}}case 16: _LL6: _tmp1FE=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp1FF=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_tmp200=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL7: {struct _tuple0*qv=_tmp1FE;struct Cyc_List_List*ps=_tmp1FF;int dots=_tmp200;
# 825
{struct Cyc_List_List*_tmp217=ps;struct Cyc_List_List*ps2=_tmp217;for(0;ps2 != 0;ps2=ps2->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps2->hd);}}
{struct _handler_cons _tmp218;_push_handler(& _tmp218);{int _tmp21A=0;if(setjmp(_tmp218.handler))_tmp21A=1;if(!_tmp21A){{void*_tmp21B=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_stmttmp19=_tmp21B;void*_tmp21C=_stmttmp19;struct Cyc_Absyn_Datatypefield*_tmp21E;struct Cyc_Absyn_Datatypedecl*_tmp21D;struct Cyc_Absyn_Aggrdecl*_tmp21F;switch(*((int*)_tmp21C)){case 0: _LL3D: _LL3E:
 goto _LL3C;case 1: _LL3F: _tmp21F=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp21C)->f1;_LL40: {struct Cyc_Absyn_Aggrdecl*ad=_tmp21F;
# 830
struct Cyc_List_List*_tmp220=0;struct Cyc_List_List*new_ps=_tmp220;
for(1;ps != 0;ps=ps->tl){
new_ps=({struct Cyc_List_List*_tmp222=_cycalloc(sizeof(*_tmp222));({struct _tuple14*_tmp396=({struct _tuple14*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->f1=0,_tmp221->f2=(struct Cyc_Absyn_Pat*)ps->hd;_tmp221;});_tmp222->hd=_tmp396;}),_tmp222->tl=new_ps;_tmp222;});}
({void*_tmp39A=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->tag=7,({union Cyc_Absyn_AggrInfo*_tmp399=({union Cyc_Absyn_AggrInfo*_tmp224=_cycalloc(sizeof(*_tmp224));({union Cyc_Absyn_AggrInfo _tmp398=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp223=_cycalloc(sizeof(*_tmp223));*_tmp223=ad;_tmp223;}));*_tmp224=_tmp398;});_tmp224;});_tmp225->f1=_tmp399;}),_tmp225->f2=0,({struct Cyc_List_List*_tmp397=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(new_ps);_tmp225->f3=_tmp397;}),_tmp225->f4=dots;_tmp225;});p->r=_tmp39A;});
_npop_handler(0);return;}case 2: _LL41: _tmp21D=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp21C)->f1;_tmp21E=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp21C)->f2;_LL42: {struct Cyc_Absyn_Datatypedecl*tud=_tmp21D;struct Cyc_Absyn_Datatypefield*tuf=_tmp21E;
# 836
({void*_tmp39B=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp226=_cycalloc(sizeof(*_tmp226));_tmp226->tag=8,_tmp226->f1=tud,_tmp226->f2=tuf,_tmp226->f3=ps,_tmp226->f4=dots;_tmp226;});p->r=_tmp39B;});_npop_handler(0);return;}case 3: _LL43: _LL44:
 goto _LL46;default: _LL45: _LL46:
# 839
({struct Cyc_Warn_String_Warn_Warg_struct _tmp228=({struct Cyc_Warn_String_Warn_Warg_struct _tmp307;_tmp307.tag=0,({struct _fat_ptr _tmp39C=({const char*_tmp229="enum tag used with arguments in pattern";_tag_fat(_tmp229,sizeof(char),40U);});_tmp307.f1=_tmp39C;});_tmp307;});void*_tmp227[1];_tmp227[0]=& _tmp228;({unsigned _tmp39D=loc;Cyc_Warn_err2(_tmp39D,_tag_fat(_tmp227,sizeof(void*),1));});});_npop_handler(0);return;}_LL3C:;}
# 827
;_pop_handler();}else{void*_tmp219=(void*)Cyc_Core_get_exn_thrown();void*_tmp22A=_tmp219;void*_tmp22B;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp22A)->tag == Cyc_Binding_BindingError){_LL48: _LL49:
# 840
 goto _LL47;}else{_LL4A: _tmp22B=_tmp22A;_LL4B: {void*exn=_tmp22B;(int)_rethrow(exn);}}_LL47:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp22D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp309;_tmp309.tag=1,_tmp309.f1=qv;_tmp309;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp308;_tmp308.tag=0,({struct _fat_ptr _tmp39E=({const char*_tmp22F=" is not a constructor in pattern";_tag_fat(_tmp22F,sizeof(char),33U);});_tmp308.f1=_tmp39E;});_tmp308;});void*_tmp22C[2];_tmp22C[0]=& _tmp22D,_tmp22C[1]=& _tmp22E;({unsigned _tmp39F=loc;Cyc_Warn_err2(_tmp39F,_tag_fat(_tmp22C,sizeof(void*),2));});});return;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1)->UnknownAggr).tag == 1){_LL8: _tmp1FA=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1)->UnknownAggr).val).f2;_tmp1FB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_tmp1FC=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_tmp1FD=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f4;_LL9: {struct _tuple0*n=_tmp1FA;struct Cyc_List_List*exist_ts=_tmp1FB;struct Cyc_List_List*dps=_tmp1FC;int dots=_tmp1FD;
# 844
{struct Cyc_List_List*_tmp230=dps;struct Cyc_List_List*dps2=_tmp230;for(0;dps2 != 0;dps2=dps2->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple14*)dps2->hd)).f2);}}
{struct _handler_cons _tmp231;_push_handler(& _tmp231);{int _tmp233=0;if(setjmp(_tmp231.handler))_tmp233=1;if(!_tmp233){
{struct Cyc_Absyn_Aggrdecl*_tmp234=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,n,Cyc_Binding_lookup_aggrdecl);struct Cyc_Absyn_Aggrdecl*ad=_tmp234;
({void*_tmp3A2=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237->tag=7,({union Cyc_Absyn_AggrInfo*_tmp3A1=({union Cyc_Absyn_AggrInfo*_tmp236=_cycalloc(sizeof(*_tmp236));({union Cyc_Absyn_AggrInfo _tmp3A0=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp235=_cycalloc(sizeof(*_tmp235));*_tmp235=ad;_tmp235;}));*_tmp236=_tmp3A0;});_tmp236;});_tmp237->f1=_tmp3A1;}),_tmp237->f2=exist_ts,_tmp237->f3=dps,_tmp237->f4=dots;_tmp237;});p->r=_tmp3A2;});}
# 847
;_pop_handler();}else{void*_tmp232=(void*)Cyc_Core_get_exn_thrown();void*_tmp238=_tmp232;void*_tmp239;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp238)->tag == Cyc_Binding_BindingError){_LL4D: _LL4E:
# 850
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30A;_tmp30A.tag=0,({struct _fat_ptr _tmp3A3=({const char*_tmp23C="non-aggregate name has designator patterns";_tag_fat(_tmp23C,sizeof(char),43U);});_tmp30A.f1=_tmp3A3;});_tmp30A;});void*_tmp23A[1];_tmp23A[0]=& _tmp23B;({unsigned _tmp3A4=loc;Cyc_Warn_err2(_tmp3A4,_tag_fat(_tmp23A,sizeof(void*),1));});});return;}else{_LL4F: _tmp239=_tmp238;_LL50: {void*exn=_tmp239;(int)_rethrow(exn);}}_LL4C:;}}}
# 852
return;}}else{_LL24: _tmp1F9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL25: {struct Cyc_List_List*dps=_tmp1F9;
# 881
for(1;dps != 0;dps=dps->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple14*)dps->hd)).f2);}
return;}}}else{_LL22: _tmp1F8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL23: {struct Cyc_List_List*dps=_tmp1F8;
# 879
_tmp1F9=dps;goto _LL25;}}case 0: _LLA: _LLB:
# 854
 return;case 3: _LLC: _tmp1F6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp1F7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LLD: {struct Cyc_Absyn_Vardecl*vd=_tmp1F6;struct Cyc_Absyn_Pat*p2=_tmp1F7;
# 856
_tmp1F4=vd;_tmp1F5=p2;goto _LLF;}case 1: _LLE: _tmp1F4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp1F5=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LLF: {struct Cyc_Absyn_Vardecl*vd=_tmp1F4;struct Cyc_Absyn_Pat*p2=_tmp1F5;
# 858
Cyc_Binding_resolve_pat(env,p2);
_tmp1F3=vd;goto _LL11;}case 4: _LL10: _tmp1F3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL11: {struct Cyc_Absyn_Vardecl*vd=_tmp1F3;
_tmp1F2=vd;goto _LL13;}case 2: _LL12: _tmp1F2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp1F2;
# 862
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL3;}case 6: _LL14: _tmp1F1=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL15: {struct Cyc_Absyn_Pat*p2=_tmp1F1;
# 864
Cyc_Binding_resolve_pat(env,p2);return;}case 5: _LL16: _tmp1F0=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL17: {struct Cyc_List_List*ps=_tmp1F0;
# 866
for(1;ps != 0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
goto _LL3;}case 17: _LL18: _tmp1EF=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp1EF;
# 870
Cyc_Binding_resolve_exp(env,e);return;}case 9: _LL1A: _LL1B:
# 872
 goto _LL1D;case 10: _LL1C: _LL1D:
 goto _LL1F;case 11: _LL1E: _LL1F:
 goto _LL21;case 12: _LL20: _LL21:
 return;case 8: _LL26: _tmp1EE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL27: {struct Cyc_List_List*ps=_tmp1EE;
# 885
for(1;ps != 0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 13: _LL28: _LL29:
 goto _LL2B;default: _LL2A: _LL2B:
 return;}_LL3:;}}struct _tuple15{void*f1;void*f2;};
# 893
void Cyc_Binding_resolve_rgnpo(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(1;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple15*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple15*)po->hd)).f2);}}struct _tuple16{struct Cyc_List_List**f1;struct Cyc_Dict_Dict*f2;struct Cyc_Binding_Env*f3;struct _tuple10*f4;};struct _tuple17{unsigned f1;struct _tuple0*f2;int f3;};
# 902
void Cyc_Binding_export_all_symbols(struct _tuple16*cenv,struct _fat_ptr*name,void*res){
# 904
struct _tuple16*_tmp23D=cenv;struct Cyc_List_List*_tmp242;unsigned _tmp241;struct Cyc_Binding_Env*_tmp240;struct Cyc_Dict_Dict*_tmp23F;struct Cyc_List_List**_tmp23E;_LL1: _tmp23E=_tmp23D->f1;_tmp23F=_tmp23D->f2;_tmp240=_tmp23D->f3;_tmp241=(_tmp23D->f4)->f1;_tmp242=(_tmp23D->f4)->f2;_LL2: {struct Cyc_List_List**exlist_ptr=_tmp23E;struct Cyc_Dict_Dict*out_dict=_tmp23F;struct Cyc_Binding_Env*env=_tmp240;unsigned wcloc=_tmp241;struct Cyc_List_List*hidelist=_tmp242;
void*_tmp243=res;struct Cyc_Absyn_Datatypefield*_tmp245;struct Cyc_Absyn_Datatypedecl*_tmp244;struct Cyc_Absyn_Enumfield*_tmp247;void*_tmp246;struct Cyc_Absyn_Aggrdecl*_tmp248;struct Cyc_Absyn_Enumfield*_tmp24A;struct Cyc_Absyn_Enumdecl*_tmp249;void*_tmp24B;switch(*((int*)_tmp243)){case 0: _LL4: _tmp24B=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp243)->f1;_LL5: {void*bnd=_tmp24B;
# 907
struct _tuple0*qv=({struct _tuple0*_tmp24F=_cycalloc(sizeof(*_tmp24F));({union Cyc_Absyn_Nmspace _tmp3A5=Cyc_Absyn_Rel_n(0);_tmp24F->f1=_tmp3A5;}),_tmp24F->f2=name;_tmp24F;});
if(((int(*)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,hidelist,qv))
# 910
return;
# 912
Cyc_Binding_absolutize_decl(wcloc,env,qv,Cyc_Absyn_ExternC);{
struct _tuple17*_tmp24C=({struct _tuple17*_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E->f1=wcloc,_tmp24E->f2=qv,_tmp24E->f3=0;_tmp24E;});struct _tuple17*ex_sym=_tmp24C;
({struct Cyc_List_List*_tmp3A6=({struct Cyc_List_List*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->hd=ex_sym,_tmp24D->tl=*exlist_ptr;_tmp24D;});*exlist_ptr=_tmp3A6;});
({struct Cyc_Dict_Dict _tmp3A7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(*out_dict,name,res);*out_dict=_tmp3A7;});
goto _LL3;}}case 3: _LL6: _tmp249=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp243)->f1;_tmp24A=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp243)->f2;_LL7: {struct Cyc_Absyn_Enumdecl*ed=_tmp249;struct Cyc_Absyn_Enumfield*ef=_tmp24A;
# 918
({struct Cyc_Dict_Dict _tmp3A8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(*out_dict,name,res);*out_dict=_tmp3A8;});
goto _LL3;}case 1: _LL8: _tmp248=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp243)->f1;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmp248;
# 922
({struct Cyc_Dict_Dict _tmp3A9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(*out_dict,name,res);*out_dict=_tmp3A9;});
goto _LL3;}case 4: _LLA: _tmp246=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp243)->f1;_tmp247=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp243)->f2;_LLB: {void*t=_tmp246;struct Cyc_Absyn_Enumfield*ef=_tmp247;
# 925
goto _LL3;}default: _LLC: _tmp244=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp243)->f1;_tmp245=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp243)->f2;_LLD: {struct Cyc_Absyn_Datatypedecl*dd=_tmp244;struct Cyc_Absyn_Datatypefield*df=_tmp245;
# 927
({void*_tmp250=0U;({unsigned _tmp3AB=wcloc;struct _fat_ptr _tmp3AA=({const char*_tmp251="Unexpected binding from extern C Include";_tag_fat(_tmp251,sizeof(char),41U);});Cyc_Warn_err(_tmp3AB,_tmp3AA,_tag_fat(_tmp250,sizeof(void*),0));});});
goto _LL3;}}_LL3:;}}
# 940 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 943
unsigned loc=d->loc;
void*_tmp252=d->r;void*_stmttmp1A=_tmp252;void*_tmp253=_stmttmp1A;struct _tuple10*_tmp257;struct Cyc_List_List**_tmp256;struct Cyc_List_List*_tmp255;struct Cyc_List_List*_tmp254;struct Cyc_List_List*_tmp258;struct Cyc_List_List*_tmp25A;struct _tuple0*_tmp259;struct Cyc_List_List*_tmp25C;struct _fat_ptr*_tmp25B;struct Cyc_Absyn_Datatypedecl*_tmp25D;struct Cyc_Absyn_Enumdecl*_tmp25E;struct Cyc_Absyn_Aggrdecl*_tmp25F;struct Cyc_Absyn_Typedefdecl*_tmp260;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Vardecl*_tmp262;struct Cyc_Absyn_Tvar*_tmp261;struct Cyc_List_List*_tmp264;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Pat*_tmp265;struct Cyc_Absyn_Fndecl*_tmp267;struct Cyc_Absyn_Vardecl*_tmp268;switch(*((int*)_tmp253)){case 0: _LL1: _tmp268=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp268;
# 947
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
# 952
if(vd->initializer != 0 && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1: _LL3: _tmp267=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp267;
# 957
if(env->in_cyc_override)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30B;_tmp30B.tag=0,({struct _fat_ptr _tmp3AC=({const char*_tmp26B="Function declaration not permitted within cyclone_override block";_tag_fat(_tmp26B,sizeof(char),65U);});_tmp30B.f1=_tmp3AC;});_tmp30B;});void*_tmp269[1];_tmp269[0]=& _tmp26A;({unsigned _tmp3AD=loc;Cyc_Warn_err2(_tmp3AD,_tag_fat(_tmp269,sizeof(void*),1));});});
Cyc_Binding_absolutize_decl(loc,env,fd->name,fd->sc);{
struct _tuple0*_tmp26C=fd->name;struct _tuple0*_stmttmp1B=_tmp26C;struct _tuple0*_tmp26D=_stmttmp1B;struct _fat_ptr*_tmp26F;union Cyc_Absyn_Nmspace _tmp26E;_LL24: _tmp26E=_tmp26D->f1;_tmp26F=_tmp26D->f2;_LL25: {union Cyc_Absyn_Nmspace decl_ns=_tmp26E;struct _fat_ptr*decl_name=_tmp26F;
struct Cyc_List_List*_tmp270=(fd->i).args;struct Cyc_List_List*argit=_tmp270;
struct Cyc_List_List*_tmp271=Cyc_Binding_get_fun_vardecls(1,loc,env,(fd->i).args,(fd->i).cyc_varargs);struct Cyc_List_List*vds=_tmp271;
({struct Cyc_Core_Opt*_tmp3AE=({struct Cyc_Core_Opt*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->v=vds;_tmp272;});fd->param_vardecls=_tmp3AE;});
# 965
if(Cyc_Flags_warn_override)
Cyc_Binding_check_warn_override(loc,env,fd->name);{
# 968
struct Cyc_Dict_Dict*_tmp273=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp273;
if(old_locals != 0)
({struct Cyc_Dict_Dict*_tmp3AF=({struct Cyc_Dict_Dict*_tmp274=_cycalloc(sizeof(*_tmp274));*_tmp274=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp274;});env->local_vars=_tmp3AF;});else{
# 972
({struct Cyc_Dict_Dict*_tmp3B1=({struct Cyc_Dict_Dict*_tmp275=_cycalloc(sizeof(*_tmp275));({struct Cyc_Dict_Dict _tmp3B0=((struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp275=_tmp3B0;});_tmp275;});env->local_vars=_tmp3B1;});}
{struct Cyc_List_List*_tmp276=vds;struct Cyc_List_List*vds1=_tmp276;for(0;vds1 != 0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _tmp3B5=({struct Cyc_Dict_Dict _tmp3B4=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp3B3=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp3B4,_tmp3B3,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp278=_cycalloc(sizeof(*_tmp278));
_tmp278->tag=0,({void*_tmp3B2=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->tag=3,_tmp277->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_tmp277;});_tmp278->f1=_tmp3B2;});_tmp278;}));});
# 974
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp3B5;});}}
# 977
Cyc_Binding_resolve_function_stuff(loc,env,& fd->i);
# 979
if(old_locals != 0){
# 981
({struct Cyc_Dict_Dict _tmp3B9=({struct Cyc_Dict_Dict _tmp3B8=*old_locals;struct _fat_ptr*_tmp3B7=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp3B8,_tmp3B7,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));
_tmp27A->tag=0,({void*_tmp3B6=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->tag=2,_tmp279->f1=fd;_tmp279;});_tmp27A->f1=_tmp3B6;});_tmp27A;}));});
# 981
*old_locals=_tmp3B9;});
# 983
({struct Cyc_Dict_Dict _tmp3BD=({struct Cyc_Dict_Dict _tmp3BC=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp3BB=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp3BC,_tmp3BB,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));
_tmp27C->tag=0,({void*_tmp3BA=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->tag=2,_tmp27B->f1=fd;_tmp27B;});_tmp27C->f1=_tmp3BA;});_tmp27C;}));});
# 983
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp3BD;});}else{
# 986
struct Cyc_Binding_ResolveNSEnv*_tmp27D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp27D;
({struct Cyc_Dict_Dict _tmp3C1=({struct Cyc_Dict_Dict _tmp3C0=decl_ns_data->ordinaries;struct _fat_ptr*_tmp3BF=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp3C0,_tmp3BF,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp27F=_cycalloc(sizeof(*_tmp27F));
# 989
_tmp27F->tag=0,({void*_tmp3BE=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->tag=2,_tmp27E->f1=fd;_tmp27E;});_tmp27F->f1=_tmp3BE;});_tmp27F;}));});
# 987
decl_ns_data->ordinaries=_tmp3C1;});}
# 992
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,fd->body);
# 995
env->local_vars=old_locals;
goto _LL0;}}}}case 2: _LL5: _tmp265=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_tmp266=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp253)->f3;_LL6: {struct Cyc_Absyn_Pat*p=_tmp265;struct Cyc_Absyn_Exp*exp=_tmp266;
# 999
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp281=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30C;_tmp30C.tag=0,({struct _fat_ptr _tmp3C2=({const char*_tmp282="let not allowed at top-level";_tag_fat(_tmp282,sizeof(char),29U);});_tmp30C.f1=_tmp3C2;});_tmp30C;});void*_tmp280[1];_tmp280[0]=& _tmp281;({unsigned _tmp3C3=loc;Cyc_Warn_err2(_tmp3C3,_tag_fat(_tmp280,sizeof(void*),1));});});
goto _LL0;}
# 1003
Cyc_Binding_resolve_exp(env,exp);
Cyc_Binding_resolve_pat(env,p);
goto _LL0;}case 3: _LL7: _tmp264=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_LL8: {struct Cyc_List_List*vds=_tmp264;
# 1008
for(1;vds != 0;vds=vds->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)vds->hd);}
goto _LL0;}case 4: _LL9: _tmp261=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_tmp262=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp253)->f2;_tmp263=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp253)->f3;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp261;struct Cyc_Absyn_Vardecl*vd=_tmp262;struct Cyc_Absyn_Exp*open_exp_opt=_tmp263;
# 1013
if(open_exp_opt != 0)
Cyc_Binding_resolve_exp(env,open_exp_opt);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
goto _LL0;}case 8: _LLB: _tmp260=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp260;
# 1019
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp284=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30D;_tmp30D.tag=0,({struct _fat_ptr _tmp3C4=({const char*_tmp285="nested type definitions are not yet supported";_tag_fat(_tmp285,sizeof(char),46U);});_tmp30D.f1=_tmp3C4;});_tmp30D;});void*_tmp283[1];_tmp283[0]=& _tmp284;({unsigned _tmp3C5=loc;Cyc_Warn_err2(_tmp3C5,_tag_fat(_tmp283,sizeof(void*),1));});});
goto _LL0;}
# 1023
Cyc_Binding_absolutize_decl(loc,env,td->name,td->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp286=td->name;struct _tuple0*_stmttmp1C=_tmp286;struct _tuple0*_tmp287=_stmttmp1C;struct _fat_ptr*_tmp289;union Cyc_Absyn_Nmspace _tmp288;_LL27: _tmp288=_tmp287->f1;_tmp289=_tmp287->f2;_LL28: {union Cyc_Absyn_Nmspace decl_ns=_tmp288;struct _fat_ptr*decl_name=_tmp289;
# 1026
struct Cyc_Binding_ResolveNSEnv*_tmp28A=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp28A;
if(td->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(td->defn));
# 1030
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _tmp3C6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*))Cyc_Dict_insert)(decl_ns_data->typedefs,decl_name,td);decl_ns_data->typedefs=_tmp3C6;});
# 1034
goto _LL0;}}}case 5: _LLD: _tmp25F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmp25F;
# 1037
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30E;_tmp30E.tag=0,({struct _fat_ptr _tmp3C7=({const char*_tmp28D="nested type definitions are not yet supported";_tag_fat(_tmp28D,sizeof(char),46U);});_tmp30E.f1=_tmp3C7;});_tmp30E;});void*_tmp28B[1];_tmp28B[0]=& _tmp28C;({unsigned _tmp3C8=loc;Cyc_Warn_err2(_tmp3C8,_tag_fat(_tmp28B,sizeof(void*),1));});});
goto _LL0;}
# 1041
Cyc_Binding_absolutize_decl(loc,env,ad->name,ad->sc);{
struct _tuple0*_tmp28E=ad->name;struct _tuple0*_stmttmp1D=_tmp28E;struct _tuple0*_tmp28F=_stmttmp1D;struct _fat_ptr*_tmp291;union Cyc_Absyn_Nmspace _tmp290;_LL2A: _tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;_LL2B: {union Cyc_Absyn_Nmspace decl_ns=_tmp290;struct _fat_ptr*decl_name=_tmp291;
# 1044
struct Cyc_Binding_ResolveNSEnv*_tmp292=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp292;
# 1046
if(((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(decl_ns_data->aggrdecls,decl_name)&& ad->impl == 0)
goto _LL0;
# 1049
if(!env->in_cyc_override){
({struct Cyc_Dict_Dict _tmp3C9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert)(decl_ns_data->aggrdecls,decl_name,ad);decl_ns_data->aggrdecls=_tmp3C9;});
# 1052
({struct Cyc_Dict_Dict _tmp3CC=({struct Cyc_Dict_Dict _tmp3CB=decl_ns_data->ordinaries;struct _fat_ptr*_tmp3CA=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp3CB,_tmp3CA,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp293=_cycalloc(sizeof(*_tmp293));
_tmp293->tag=1,_tmp293->f1=ad;_tmp293;}));});
# 1052
decl_ns_data->ordinaries=_tmp3CC;});}
# 1055
if(ad->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp294=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl);struct Cyc_Absyn_AggrdeclImpl*_stmttmp1E=_tmp294;struct Cyc_Absyn_AggrdeclImpl*_tmp295=_stmttmp1E;struct Cyc_List_List*_tmp297;struct Cyc_List_List*_tmp296;_LL2D: _tmp296=_tmp295->rgn_po;_tmp297=_tmp295->fields;_LL2E: {struct Cyc_List_List*rpo=_tmp296;struct Cyc_List_List*fs=_tmp297;
Cyc_Binding_resolve_rgnpo(loc,env,rpo);
Cyc_Binding_resolve_aggrfields(loc,env,fs);}}
# 1060
goto _LL0;}}}case 7: _LLF: _tmp25E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp25E;
# 1063
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp299=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30F;_tmp30F.tag=0,({struct _fat_ptr _tmp3CD=({const char*_tmp29A="nested type definitions are not yet supported";_tag_fat(_tmp29A,sizeof(char),46U);});_tmp30F.f1=_tmp3CD;});_tmp30F;});void*_tmp298[1];_tmp298[0]=& _tmp299;({unsigned _tmp3CE=loc;Cyc_Warn_err2(_tmp3CE,_tag_fat(_tmp298,sizeof(void*),1));});});
goto _LL0;}
# 1067
Cyc_Binding_absolutize_decl(loc,env,ed->name,ed->sc);{
struct _tuple0*_tmp29B=ed->name;struct _tuple0*_stmttmp1F=_tmp29B;struct _tuple0*_tmp29C=_stmttmp1F;struct _fat_ptr*_tmp29E;union Cyc_Absyn_Nmspace _tmp29D;_LL30: _tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;_LL31: {union Cyc_Absyn_Nmspace decl_ns=_tmp29D;struct _fat_ptr*decl_name=_tmp29E;
# 1070
struct Cyc_Binding_ResolveNSEnv*_tmp29F=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp29F;
# 1072
if(((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(decl_ns_data->enumdecls,decl_name)&& ed->fields == 0)
goto _LL0;
# 1076
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _tmp3CF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert)(decl_ns_data->enumdecls,decl_name,ed);decl_ns_data->enumdecls=_tmp3CF;});
# 1079
if(ed->fields != 0){
# 1081
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp2A0=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp2A0;
Cyc_Binding_absolutize_decl(f->loc,env,f->name,ed->sc);
if(f->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _tmp3D2=({struct Cyc_Dict_Dict _tmp3D1=decl_ns_data->ordinaries;struct _fat_ptr*_tmp3D0=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp3D1,_tmp3D0,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));
# 1089
_tmp2A1->tag=3,_tmp2A1->f1=ed,_tmp2A1->f2=f;_tmp2A1;}));});
# 1087
decl_ns_data->ordinaries=_tmp3D2;});}}
# 1091
goto _LL0;}}}case 6: _LL11: _tmp25D=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_LL12: {struct Cyc_Absyn_Datatypedecl*tud=_tmp25D;
# 1095
{struct _handler_cons _tmp2A2;_push_handler(& _tmp2A2);{int _tmp2A4=0;if(setjmp(_tmp2A2.handler))_tmp2A4=1;if(!_tmp2A4){
{struct Cyc_List_List*_tmp2A5=((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,tud->name,Cyc_Binding_lookup_datatypedecl);struct Cyc_List_List*decls=_tmp2A5;
struct Cyc_Absyn_Datatypedecl*_tmp2A6=(struct Cyc_Absyn_Datatypedecl*)decls->hd;struct Cyc_Absyn_Datatypedecl*last_decl=_tmp2A6;
if(!last_decl->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->tag=Cyc_Binding_BindingError;_tmp2A7;}));
tud->name=last_decl->name;
tud->is_extensible=1;{
struct _tuple0*_tmp2A8=tud->name;struct _tuple0*_stmttmp20=_tmp2A8;struct _tuple0*_tmp2A9=_stmttmp20;struct _fat_ptr*_tmp2AB;union Cyc_Absyn_Nmspace _tmp2AA;_LL33: _tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2A9->f2;_LL34: {union Cyc_Absyn_Nmspace decl_ns=_tmp2AA;struct _fat_ptr*decl_name=_tmp2AB;
struct Cyc_Binding_ResolveNSEnv*_tmp2AC=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp2AC;
({struct Cyc_Dict_Dict _tmp3D5=({struct Cyc_Dict_Dict _tmp3D4=decl_ns_data->datatypedecls;struct _fat_ptr*_tmp3D3=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))Cyc_Dict_insert)(_tmp3D4,_tmp3D3,({struct Cyc_List_List*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));
_tmp2AD->hd=tud,_tmp2AD->tl=decls;_tmp2AD;}));});
# 1104
decl_ns_data->datatypedecls=_tmp3D5;});}}}
# 1096
;_pop_handler();}else{void*_tmp2A3=(void*)Cyc_Core_get_exn_thrown();void*_tmp2AE=_tmp2A3;void*_tmp2AF;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp2AE)->tag == Cyc_Binding_BindingError){_LL36: _LL37:
# 1108
 Cyc_Binding_absolutize_topdecl(loc,env,tud->name,tud->sc);{
struct _tuple0*_tmp2B0=tud->name;struct _tuple0*_stmttmp21=_tmp2B0;struct _tuple0*_tmp2B1=_stmttmp21;struct _fat_ptr*_tmp2B3;union Cyc_Absyn_Nmspace _tmp2B2;_LL3B: _tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B1->f2;_LL3C: {union Cyc_Absyn_Nmspace decl_ns=_tmp2B2;struct _fat_ptr*decl_name=_tmp2B3;
struct Cyc_Binding_ResolveNSEnv*_tmp2B4=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp2B4;
# 1112
if(((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(decl_ns_data->datatypedecls,decl_name)&& tud->fields == 0)
# 1114
goto _LL35;
({struct Cyc_Dict_Dict _tmp3D8=({struct Cyc_Dict_Dict _tmp3D7=decl_ns_data->datatypedecls;struct _fat_ptr*_tmp3D6=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))Cyc_Dict_insert)(_tmp3D7,_tmp3D6,({struct Cyc_List_List*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));
_tmp2B5->hd=tud,_tmp2B5->tl=0;_tmp2B5;}));});
# 1115
decl_ns_data->datatypedecls=_tmp3D8;});
# 1117
goto _LL35;}}}else{_LL38: _tmp2AF=_tmp2AE;_LL39: {void*exn=_tmp2AF;(int)_rethrow(exn);}}_LL35:;}}}{
# 1120
struct _tuple0*_tmp2B6=tud->name;struct _tuple0*_stmttmp22=_tmp2B6;struct _tuple0*_tmp2B7=_stmttmp22;struct _fat_ptr*_tmp2B9;union Cyc_Absyn_Nmspace _tmp2B8;_LL3E: _tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B7->f2;_LL3F: {union Cyc_Absyn_Nmspace decl_ns=_tmp2B8;struct _fat_ptr*decl_name=_tmp2B9;
if(tud->fields != 0){
struct Cyc_List_List*_tmp2BA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmp2BA;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp2BB=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp2BB;
{struct Cyc_List_List*_tmp2BC=f->typs;struct Cyc_List_List*tqts=_tmp2BC;for(0;tqts != 0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)tqts->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp2BD=(*f->name).f1;union Cyc_Absyn_Nmspace _stmttmp23=_tmp2BD;union Cyc_Absyn_Nmspace _tmp2BE=_stmttmp23;switch((_tmp2BE.Abs_n).tag){case 1: if((_tmp2BE.Rel_n).val == 0){_LL41: _LL42:
# 1128
 if(tud->is_extensible)
({union Cyc_Absyn_Nmspace _tmp3D9=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*f->name).f1=_tmp3D9;});else{
# 1131
(*f->name).f1=(*tud->name).f1;}
goto _LL40;}else{_LL43: _LL44:
# 1134
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp310;_tmp310.tag=0,({struct _fat_ptr _tmp3DA=({const char*_tmp2C1="qualified datatype field declarations not allowed";_tag_fat(_tmp2C1,sizeof(char),50U);});_tmp310.f1=_tmp3DA;});_tmp310;});void*_tmp2BF[1];_tmp2BF[0]=& _tmp2C0;({unsigned _tmp3DB=f->loc;Cyc_Warn_err2(_tmp3DB,_tag_fat(_tmp2BF,sizeof(void*),1));});});
return;}case 2: _LL45: _LL46:
 goto _LL40;default: _LL47: _LL48:
({void*_tmp2C2=0U;({struct _fat_ptr _tmp3DC=({const char*_tmp2C3="datatype field Loc_n or C_n";_tag_fat(_tmp2C3,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp3DC,_tag_fat(_tmp2C2,sizeof(void*),0));});});}_LL40:;}{
# 1139
struct Cyc_Binding_ResolveNSEnv*_tmp2C4=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data)(env->ns,(*f->name).f1);struct Cyc_Binding_ResolveNSEnv*ns_data=_tmp2C4;
({struct Cyc_Dict_Dict _tmp3DF=({struct Cyc_Dict_Dict _tmp3DE=ns_data->ordinaries;struct _fat_ptr*_tmp3DD=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp3DE,_tmp3DD,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));
# 1142
_tmp2C5->tag=2,_tmp2C5->f1=tud,_tmp2C5->f2=f;_tmp2C5;}));});
# 1140
ns_data->ordinaries=_tmp3DF;});}}}
# 1144
goto _LL0;}}}case 9: _LL13: _tmp25B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_tmp25C=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp253)->f2;_LL14: {struct _fat_ptr*v=_tmp25B;struct Cyc_List_List*ds2=_tmp25C;
# 1147
((void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_enter_ns)(env->ns,v,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,ds2);
((void(*)(struct Cyc_Binding_NSCtxt*))Cyc_Binding_leave_ns)(env->ns);
goto _LL0;}case 10: _LL15: _tmp259=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_tmp25A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp253)->f2;_LL16: {struct _tuple0*qv=_tmp259;struct Cyc_List_List*ds2=_tmp25A;
# 1152
((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*))Cyc_Binding_enter_using)(d->loc,env->ns,qv);
Cyc_Binding_resolve_decls(env,ds2);
((void(*)(struct Cyc_Binding_NSCtxt*))Cyc_Binding_leave_using)(env->ns);
goto _LL0;}case 11: _LL17: _tmp258=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_LL18: {struct Cyc_List_List*ds2=_tmp258;
# 1158
int _tmp2C6=env->in_cinclude;int old=_tmp2C6;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
env->in_cinclude=old;
goto _LL0;}case 12: _LL19: _tmp254=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp253)->f1;_tmp255=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp253)->f2;_tmp256=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp253)->f3;_tmp257=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp253)->f4;_LL1A: {struct Cyc_List_List*ds2=_tmp254;struct Cyc_List_List*ovrs=_tmp255;struct Cyc_List_List**exports=_tmp256;struct _tuple10*hides=_tmp257;
# 1169
{struct Cyc_List_List*_tmp2C7=*exports;struct Cyc_List_List*exs=_tmp2C7;for(0;exs != 0;exs=exs->tl){
struct _tuple17*_tmp2C8=(struct _tuple17*)exs->hd;struct _tuple17*_stmttmp24=_tmp2C8;struct _tuple17*_tmp2C9=_stmttmp24;struct _tuple0*_tmp2CB;unsigned _tmp2CA;_LL4A: _tmp2CA=_tmp2C9->f1;_tmp2CB=_tmp2C9->f2;_LL4B: {unsigned loc=_tmp2CA;struct _tuple0*qv=_tmp2CB;
Cyc_Binding_absolutize_decl(loc,env,qv,Cyc_Absyn_ExternC);}}}
# 1177
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp311;_tmp311.tag=0,({struct _fat_ptr _tmp3E0=({const char*_tmp2CE="extern \"C include\" not at toplevel";_tag_fat(_tmp2CE,sizeof(char),35U);});_tmp311.f1=_tmp3E0;});_tmp311;});void*_tmp2CC[1];_tmp2CC[0]=& _tmp2CD;({unsigned _tmp3E1=loc;Cyc_Warn_err2(_tmp3E1,_tag_fat(_tmp2CC,sizeof(void*),1));});});
goto _LL0;}{
# 1181
struct Cyc_Binding_ResolveNSEnv*_tmp2CF=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);struct Cyc_Binding_ResolveNSEnv*data=_tmp2CF;
struct Cyc_Dict_Dict _tmp2D0=data->ordinaries;struct Cyc_Dict_Dict old_dict=_tmp2D0;
int _tmp2D1=env->in_cinclude;int old=_tmp2D1;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
env->in_cyc_override=1;
Cyc_Binding_resolve_decls(env,ovrs);
env->in_cyc_override=0;{
struct Cyc_Dict_Dict _tmp2D2=data->ordinaries;struct Cyc_Dict_Dict new_dict=_tmp2D2;
struct Cyc_Dict_Dict _tmp2D3=old_dict;struct Cyc_Dict_Dict out_dict=_tmp2D3;
if((*hides).f1 > (unsigned)0){
if((unsigned)*exports)
({void*_tmp2D4=0U;({unsigned _tmp3E3=(*hides).f1;struct _fat_ptr _tmp3E2=({const char*_tmp2D5="export wildcard expects empty export list";_tag_fat(_tmp2D5,sizeof(char),42U);});Cyc_Warn_err(_tmp3E3,_tmp3E2,_tag_fat(_tmp2D4,sizeof(void*),0));});});
env->in_cinclude=old;
({struct _tuple16*_tmp3E4=({struct _tuple16*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->f1=exports,_tmp2D6->f2=& out_dict,_tmp2D6->f3=env,_tmp2D6->f4=hides;_tmp2D6;});((void(*)(void(*)(struct _tuple16*,struct _fat_ptr*,void*),struct _tuple16*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Binding_export_all_symbols,_tmp3E4,
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_difference)(new_dict,old_dict));});}else{
# 1199
struct Cyc_List_List*_tmp2D7=*exports;struct Cyc_List_List*exs=_tmp2D7;for(0;exs != 0;exs=exs->tl){
struct _tuple17*_tmp2D8=(struct _tuple17*)exs->hd;struct _tuple17*_stmttmp25=_tmp2D8;struct _tuple17*_tmp2D9=_stmttmp25;struct _fat_ptr*_tmp2DB;unsigned _tmp2DA;_LL4D: _tmp2DA=_tmp2D9->f1;_tmp2DB=(_tmp2D9->f2)->f2;_LL4E: {unsigned loc=_tmp2DA;struct _fat_ptr*v=_tmp2DB;
if(!((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(new_dict,v)||
((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(old_dict,v)&&({
void*_tmp3E5=((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(old_dict,v);_tmp3E5 == ((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(new_dict,v);}))
({struct Cyc_String_pa_PrintArg_struct _tmp2DE=({struct Cyc_String_pa_PrintArg_struct _tmp312;_tmp312.tag=0,_tmp312.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp312;});void*_tmp2DC[1];_tmp2DC[0]=& _tmp2DE;({unsigned _tmp3E7=loc;struct _fat_ptr _tmp3E6=({const char*_tmp2DD="%s is exported but not defined";_tag_fat(_tmp2DD,sizeof(char),31U);});Cyc_Warn_err(_tmp3E7,_tmp3E6,_tag_fat(_tmp2DC,sizeof(void*),1));});});
out_dict=({struct Cyc_Dict_Dict _tmp3E9=out_dict;struct _fat_ptr*_tmp3E8=v;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert)(_tmp3E9,_tmp3E8,((void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(new_dict,v));});}}}
# 1208
data->ordinaries=out_dict;
env->in_cinclude=old;
# 1213
goto _LL0;}}}case 13: _LL1B: _LL1C:
# 1215
 goto _LL1E;case 14: _LL1D: _LL1E:
 goto _LL20;case 15: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 1222
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(1;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1227
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp2DF=({struct Cyc_Binding_Env*_tmp2E2=_cycalloc(sizeof(*_tmp2E2));_tmp2E2->in_cinclude=0,_tmp2E2->in_cyc_override=0,({struct Cyc_Binding_NSCtxt*_tmp3EA=((struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2E2->ns=_tmp3EA;}),_tmp2E2->local_vars=0;_tmp2E2;});struct Cyc_Binding_Env*env=_tmp2DF;
({struct Cyc_Binding_Env*_tmp3ED=env;Cyc_Binding_resolve_decl(_tmp3ED,({struct Cyc_Absyn_Decl*_tmp2E1=_cycalloc(sizeof(*_tmp2E1));({void*_tmp3EC=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2E0=_cycalloc(sizeof(*_tmp2E0));_tmp2E0->tag=6,({struct Cyc_Absyn_Datatypedecl*_tmp3EB=Cyc_Absyn_exn_tud();_tmp2E0->f1=_tmp3EB;});_tmp2E0;});_tmp2E1->r=_tmp3EC;}),_tmp2E1->loc=0U;_tmp2E1;}));});
Cyc_Binding_resolve_decls(env,tds);}

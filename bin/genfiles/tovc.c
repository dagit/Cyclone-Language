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
# 72 "list.h"
extern struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 69
extern struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*,struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
extern struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*,struct Cyc_Set_Set*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 277 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
# 857
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 864
void*Cyc_Absyn_compress(void*);
# 895
extern void*Cyc_Absyn_void_type;
# 910
extern struct _tuple0*Cyc_Absyn_exn_name;
# 936
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 970
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1036
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
# 1038
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1049
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1062
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1127
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 70 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 146 "tcutil.h"
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_Hashtable_Table;
# 41 "toc.h"
struct _tuple0*Cyc_Toc_temp_var (void);
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
# 73 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 101
static struct Cyc_Absyn_Exp*Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;void*_tmp4;int _tmp6;int _tmp5;void*_tmp3;void*_tmp2;switch(*((int*)_tmp1)){case 1: _tmp2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{void*b=_tmp2;
# 105
struct _tuple0*_tmp7=Cyc_Absyn_binding2qvar(b);struct _tuple0*_stmttmp1=_tmp7;struct _tuple0*_tmp8=_stmttmp1;void*_tmp9;if(((((struct _tuple0*)_tmp8)->f1).Loc_n).tag == 4){_tmp9=_tmp8->f2;{struct _fat_ptr*v=_tmp9;
# 109
if(((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(env.varmap,v)){
({void*_tmpCD=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(env.varmap,v))->r;e->r=_tmpCD;});
continue;}
# 114
{int i=0;for(0;(unsigned)i < _get_fat_size(Cyc_Toc_globals,sizeof(struct _fat_ptr*));++ i){
if(Cyc_strptrcmp(v,*((struct _fat_ptr**)_check_fat_subscript(Cyc_Toc_globals,sizeof(struct _fat_ptr*),i)))== 0)
return 0;}}
# 118
for(1;env.boundvars != 0;env.boundvars=(env.boundvars)->tl){
if(Cyc_strptrcmp(v,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)
return 0;}
# 122
for(1;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmpA=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;struct Cyc_Absyn_Vardecl*vd=_tmpA;
if(Cyc_strptrcmp(v,(*vd->name).f2)== 0){
({struct Cyc_Set_Set*_tmpCE=((struct Cyc_Set_Set*(*)(struct _RegionHandle*,struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))Cyc_Set_rinsert)(env.rgn,*env.freevars,vd);*env.freevars=_tmpCE;});{
# 127
void*_tmpB=vd->type;void*_stmttmp2=_tmpB;void*_tmpC=_stmttmp2;if(*((int*)_tmpC)== 4)
return 0;else{
return Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(vd->name,0U),0U);};}}}
# 136
for(1;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
if(Cyc_strptrcmp(v,(*((struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd)->name).f2)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC1;_tmpC1.tag=0,({struct _fat_ptr _tmpCF=({const char*_tmp12="unbound variable ";_tag_fat(_tmp12,sizeof(char),18U);});_tmpC1.f1=_tmpCF;});_tmpC1;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC0;_tmpC0.tag=0,_tmpC0.f1=*v;_tmpC0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _tmpBF;_tmpBF.tag=0,({struct _fat_ptr _tmpD0=({const char*_tmp11=" found in box_free_vars_exp";_tag_fat(_tmp11,sizeof(char),28U);});_tmpBF.f1=_tmpD0;});_tmpBF;});void*_tmpD[3];_tmpD[0]=& _tmpE,_tmpD[1]=& _tmpF,_tmpD[2]=& _tmp10;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpD,sizeof(void*),3));});}
# 143
return 0;}}else{
return 0;};}case 23: _tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2;struct Cyc_Absyn_Exp*e2=_tmp3;
# 150
struct Cyc_Absyn_Exp*_tmp13=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp13;
struct Cyc_Absyn_Exp*_tmp14=Cyc_Tovc_box_free_vars_exp(env,e2);struct Cyc_Absyn_Exp*e2b=_tmp14;
if(e1b != 0){
if(e2b != 0){
struct Cyc_Absyn_Exp*_tmp15=Cyc_Absyn_subscript_exp(e1b,e2b,e->loc);struct Cyc_Absyn_Exp*eb=_tmp15;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}else{
# 160
struct Cyc_Absyn_Exp*_tmp16=Cyc_Absyn_subscript_exp(e1b,e2,e->loc);struct Cyc_Absyn_Exp*eb=_tmp16;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}}else{
# 166
if(e2b != 0){
struct Cyc_Absyn_Exp*_tmp17=Cyc_Absyn_subscript_exp(e1,e2b,e->loc);struct Cyc_Absyn_Exp*eb=_tmp17;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}}
# 172
return 0;}case 15: _tmp2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2;
# 175
struct Cyc_Absyn_Exp*_tmp18=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp18;
if(e1b != 0){
struct Cyc_Absyn_Exp*_tmp19=Cyc_Absyn_address_exp(e1b,e->loc);struct Cyc_Absyn_Exp*eb=_tmp19;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 182
return 0;}case 20: _tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2;
# 185
struct Cyc_Absyn_Exp*_tmp1A=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp1A;
if(e1b != 0){
struct Cyc_Absyn_Exp*_tmp1B=Cyc_Absyn_deref_exp(e1b,e->loc);struct Cyc_Absyn_Exp*eb=_tmp1B;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 192
return 0;}case 21: _tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2;struct _fat_ptr*fn=_tmp3;int ist=_tmp5;int isr=_tmp6;
# 195
struct Cyc_Absyn_Exp*_tmp1C=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp1C;
if(e1b != 0){
struct Cyc_Absyn_Exp*_tmp1D=({void*_tmpD1=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->tag=21,_tmp1E->f1=e1b,_tmp1E->f2=fn,_tmp1E->f3=ist,_tmp1E->f4=isr;_tmp1E;});Cyc_Absyn_new_exp(_tmpD1,e->loc);});struct Cyc_Absyn_Exp*eb=_tmp1D;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 202
return 0;}case 22: _tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2;struct _fat_ptr*fn=_tmp3;int ist=_tmp5;int isr=_tmp6;
# 205
struct Cyc_Absyn_Exp*_tmp1F=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp1F;
if(e1b != 0){
struct Cyc_Absyn_Exp*_tmp20=({void*_tmpD2=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=22,_tmp21->f1=e1b,_tmp21->f2=fn,_tmp21->f3=ist,_tmp21->f4=isr;_tmp21;});Cyc_Absyn_new_exp(_tmpD2,e->loc);});struct Cyc_Absyn_Exp*eb=_tmp20;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 212
return 0;}case 0:
# 215
 return 0;case 10: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f2;{struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_List_List*es=_tmp3;
# 217
{struct Cyc_Absyn_Exp*_tmp22=Cyc_Tovc_box_free_vars_exp(env,*e1);struct Cyc_Absyn_Exp*__ei=_tmp22;if(__ei != 0)*e1=__ei;}
_tmp2=es;goto _LL12;}case 3: _tmp2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL12: {struct Cyc_List_List*es=_tmp2;
# 220
for(1;es != 0;es=es->tl){
struct Cyc_Absyn_Exp*_tmp23=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)es->hd);struct Cyc_Absyn_Exp*__ei=_tmp23;if(__ei != 0)es->hd=(void*)__ei;}
# 223
return 0;}case 6: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f3;{struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=_tmp3;struct Cyc_Absyn_Exp**e3=(struct Cyc_Absyn_Exp**)_tmp4;
# 226
{struct Cyc_Absyn_Exp*_tmp24=Cyc_Tovc_box_free_vars_exp(env,*e3);struct Cyc_Absyn_Exp*__ei=_tmp24;if(__ei != 0)*e3=__ei;}
_tmp2=e1;_tmp3=e2;goto _LL16;}case 7: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL16: {struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=_tmp3;
_tmp2=e1;_tmp3=e2;goto _LL18;}case 8: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL18: {struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=_tmp3;
_tmp2=e1;_tmp3=e2;goto _LL1A;}case 9: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL1A: {struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=_tmp3;
_tmp2=e1;_tmp3=e2;goto _LL1C;}case 4: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL1C: {struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=(struct Cyc_Absyn_Exp**)_tmp3;
# 232
{struct Cyc_Absyn_Exp*_tmp25=Cyc_Tovc_box_free_vars_exp(env,*e2);struct Cyc_Absyn_Exp*__ei=_tmp25;if(__ei != 0)*e2=__ei;}
_tmp2=e1;goto _LL1E;}case 12: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL1E: {struct Cyc_Absyn_Exp**e1=_tmp2;
# 235
_tmp2=e1;goto _LL20;}case 13: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL20: {struct Cyc_Absyn_Exp**e1=_tmp2;
_tmp2=e1;goto _LL22;}case 14: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL22: {struct Cyc_Absyn_Exp**e1=_tmp2;
_tmp2=e1;goto _LL24;}case 18: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL24: {struct Cyc_Absyn_Exp**e1=_tmp2;
_tmp2=e1;goto _LL26;}case 5: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL26: {struct Cyc_Absyn_Exp**e1=_tmp2;
# 240
{struct Cyc_Absyn_Exp*_tmp26=Cyc_Tovc_box_free_vars_exp(env,*e1);struct Cyc_Absyn_Exp*__ei=_tmp26;if(__ei != 0)*e1=__ei;}
return 0;}case 17:
# 243
 goto _LL2A;case 19: _LL2A:
 goto _LL2C;case 32: _LL2C:
 return 0;case 36: _tmp2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1)->f2;{struct Cyc_List_List*dles=_tmp2;
# 248
for(1;dles != 0;dles=dles->tl){
struct Cyc_Absyn_Exp*_tmp27=Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple11*)dles->hd)).f2);struct Cyc_Absyn_Exp*__ei=_tmp27;if(__ei != 0)(*((struct _tuple11*)dles->hd)).f2=__ei;}
# 251
return 0;}default:
# 253
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC2;_tmpC2.tag=0,({struct _fat_ptr _tmpD3=({const char*_tmp2A="bad exp form in Tocv::box_free_vars_exp";_tag_fat(_tmp2A,sizeof(char),40U);});_tmpC2.f1=_tmpD3;});_tmpC2;});void*_tmp28[1];_tmp28[0]=& _tmp29;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp28,sizeof(void*),1));});};}}
# 257
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 261
while(1){
void*_tmp2B=s->r;void*_stmttmp3=_tmp2B;void*_tmp2C=_stmttmp3;void*_tmp30;void*_tmp2F;void*_tmp2E;void*_tmp2D;switch(*((int*)_tmp2C)){case 0:
 return;case 6:
# 265
({void*_tmp31=0U;({unsigned _tmpD5=s->loc;struct _fat_ptr _tmpD4=({const char*_tmp32="break in eliminated statement expression";_tag_fat(_tmp32,sizeof(char),41U);});Cyc_Warn_warn(_tmpD5,_tmpD4,_tag_fat(_tmp31,sizeof(void*),0));});});
return;case 7:
# 268
({void*_tmp33=0U;({unsigned _tmpD7=s->loc;struct _fat_ptr _tmpD6=({const char*_tmp34="continue in eliminated statement expression";_tag_fat(_tmp34,sizeof(char),44U);});Cyc_Warn_warn(_tmpD7,_tmpD6,_tag_fat(_tmp33,sizeof(void*),0));});});
# 271
return;case 8:
# 273
({void*_tmp35=0U;({unsigned _tmpD9=s->loc;struct _fat_ptr _tmpD8=({const char*_tmp36="goto in eliminated statement expression";_tag_fat(_tmp36,sizeof(char),40U);});Cyc_Warn_warn(_tmpD9,_tmpD8,_tag_fat(_tmp35,sizeof(void*),0));});});
return;case 13: _tmp2D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp2D;
# 276
s=s2;
continue;}case 3: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;{struct Cyc_Absyn_Exp**eopt=_tmp2D;
# 279
({void*_tmp37=0U;({unsigned _tmpDB=s->loc;struct _fat_ptr _tmpDA=({const char*_tmp38="return in eliminated statement expression";_tag_fat(_tmp38,sizeof(char),42U);});Cyc_Warn_warn(_tmpDB,_tmpDA,_tag_fat(_tmp37,sizeof(void*),0));});});
# 281
if(*eopt != 0){
struct Cyc_Absyn_Exp*_tmp39=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(*eopt));struct Cyc_Absyn_Exp*ei=_tmp39;
if(ei != 0)*eopt=ei;}
# 285
return;}case 1: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;{struct Cyc_Absyn_Exp**e=_tmp2D;
# 287
{struct Cyc_Absyn_Exp*_tmp3A=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp3A;if(__ei != 0)*e=__ei;}
return;}case 2: _tmp2D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp2D;struct Cyc_Absyn_Stmt*s2=_tmp2E;
# 290
Cyc_Tovc_box_free_vars_stmt(env,s1);
s=s2;
continue;}case 4: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;_tmp2F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2C)->f3;{struct Cyc_Absyn_Exp**e=_tmp2D;struct Cyc_Absyn_Stmt*s1=_tmp2E;struct Cyc_Absyn_Stmt*s2=_tmp2F;
# 294
{struct Cyc_Absyn_Exp*_tmp3B=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp3B;if(__ei != 0)*e=__ei;}
Cyc_Tovc_box_free_vars_stmt(env,s1);
s=s2;
continue;}case 5: _tmp2D=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1).f1;_tmp2E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Exp**e=_tmp2D;struct Cyc_Absyn_Stmt*s2=_tmp2E;
# 299
{struct Cyc_Absyn_Exp*_tmp3C=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp3C;if(__ei != 0)*e=__ei;}
s=s2;
continue;}case 9: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2).f1;_tmp2F=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2C)->f3).f1;_tmp30=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2C)->f4;{struct Cyc_Absyn_Exp**e1=_tmp2D;struct Cyc_Absyn_Exp**e2=_tmp2E;struct Cyc_Absyn_Exp**e3=(struct Cyc_Absyn_Exp**)_tmp2F;struct Cyc_Absyn_Stmt*s2=_tmp30;
# 303
{struct Cyc_Absyn_Exp*_tmp3D=Cyc_Tovc_box_free_vars_exp(env,*e1);struct Cyc_Absyn_Exp*__ei=_tmp3D;if(__ei != 0)*e1=__ei;}
{struct Cyc_Absyn_Exp*_tmp3E=Cyc_Tovc_box_free_vars_exp(env,*e2);struct Cyc_Absyn_Exp*__ei=_tmp3E;if(__ei != 0)*e2=__ei;}
{struct Cyc_Absyn_Exp*_tmp3F=Cyc_Tovc_box_free_vars_exp(env,*e3);struct Cyc_Absyn_Exp*__ei=_tmp3F;if(__ei != 0)*e3=__ei;}
s=s2;
continue;}case 14: _tmp2D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2).f1;{struct Cyc_Absyn_Stmt*s2=_tmp2D;struct Cyc_Absyn_Exp**e=_tmp2E;
# 309
{struct Cyc_Absyn_Exp*_tmp40=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp40;if(__ei != 0)*e=__ei;}
s=s2;
continue;}case 10: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Exp**e=_tmp2D;struct Cyc_List_List*scs=_tmp2E;
# 314
{struct Cyc_Absyn_Exp*_tmp41=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp41;if(__ei != 0)*e=__ei;}
for(1;scs != 0;scs=scs->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _tmp2D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Decl*d=_tmp2D;struct Cyc_Absyn_Stmt*s2=_tmp2E;
# 319
{void*_tmp42=d->r;void*_stmttmp4=_tmp42;void*_tmp43=_stmttmp4;void*_tmp44;if(*((int*)_tmp43)== 0){_tmp44=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp43)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp44;
# 321
({struct Cyc_List_List*_tmpDC=({struct Cyc_List_List*_tmp45=_region_malloc(env.rgn,sizeof(*_tmp45));_tmp45->hd=vd,_tmp45->tl=env.boundvars;_tmp45;});env.boundvars=_tmpDC;});
({struct Cyc_Dict_Dict _tmpDD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_rdelete_same)(env.varmap,(*vd->name).f2);env.varmap=_tmpDD;});
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*_tmp46=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));struct Cyc_Absyn_Exp*ei=_tmp46;
if(ei != 0)vd->initializer=ei;}
# 327
s=s2;
continue;}}else{
goto _LL1F;}_LL1F:;}
# 331
goto _LL1E;}default: _LL1E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmpDE=({const char*_tmp47="bad stmt after xlation to C";_tag_fat(_tmp47,sizeof(char),28U);});_tmp48->f1=_tmpDE;});_tmp48;}));};}}
# 342
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 346
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 349
struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp49=({struct Cyc_Tovc_BoxingEnv _tmpC3;_tmpC3.rgn=env.rgn,_tmpC3.all_locals=env.all_locals,_tmpC3.varmap=env.varmap,_tmpC3.boundvars=0,_tmpC3.freevars=& freevars;_tmpC3;});struct Cyc_Tovc_BoxingEnv boxenv=_tmp49;
Cyc_Tovc_box_free_vars_stmt(boxenv,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 355
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*,struct Cyc_Set_Set*))Cyc_Set_make_iter)(Cyc_Core_heap_region,freevars);
# 357
struct Cyc_Absyn_Vardecl*_tmp4A=({struct Cyc_Absyn_Vardecl*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->sc=Cyc_Absyn_Public,_tmp57->name=Cyc_Absyn_exn_name,_tmp57->varloc=0U,({struct Cyc_Absyn_Tqual _tmpDF=Cyc_Absyn_empty_tqual(0U);_tmp57->tq=_tmpDF;}),_tmp57->type=Cyc_Absyn_void_type,_tmp57->initializer=0,_tmp57->rgn=0,_tmp57->attributes=0,_tmp57->escapes=0,_tmp57->is_proto=0;_tmp57;});struct Cyc_Absyn_Vardecl*vd=_tmp4A;
# 360
while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,& vd)){
void*_tmp4B=vd->type;void*_stmttmp5=_tmp4B;void*_tmp4C=_stmttmp5;void*_tmp4D;if(*((int*)_tmp4C)== 4){_tmp4D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;{void*elt_type=_tmp4D;
# 363
params=({struct Cyc_List_List*_tmp4F=_cycalloc(sizeof(*_tmp4F));({struct _tuple8*_tmpE2=({struct _tuple8*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=(*vd->name).f2,_tmp4E->f2=vd->tq,({
void*_tmpE1=({void*_tmpE0=elt_type;Cyc_Absyn_cstar_type(_tmpE0,Cyc_Absyn_empty_tqual(0U));});_tmp4E->f3=_tmpE1;});_tmp4E;});
# 363
_tmp4F->hd=_tmpE2;}),_tmp4F->tl=params;_tmp4F;});
# 366
args=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));({struct Cyc_Absyn_Exp*_tmpE3=Cyc_Absyn_var_exp(vd->name,0U);_tmp50->hd=_tmpE3;}),_tmp50->tl=args;_tmp50;});
goto _LL0;}}else{
# 369
params=({struct Cyc_List_List*_tmp52=_cycalloc(sizeof(*_tmp52));({struct _tuple8*_tmpE6=({struct _tuple8*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->f1=(*vd->name).f2,_tmp51->f2=vd->tq,({
void*_tmpE5=({void*_tmpE4=vd->type;Cyc_Absyn_cstar_type(_tmpE4,Cyc_Absyn_empty_tqual(0U));});_tmp51->f3=_tmpE5;});_tmp51;});
# 369
_tmp52->hd=_tmpE6;}),_tmp52->tl=params;_tmp52;});
# 372
args=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));({struct Cyc_Absyn_Exp*_tmpE7=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(vd->name,0U),0U);_tmp53->hd=_tmpE7;}),_tmp53->tl=args;_tmp53;});
goto _LL0;}_LL0:;}{
# 375
struct _tuple0*funname=Cyc_Toc_temp_var();
({struct Cyc_List_List*_tmpEB=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));
({struct Cyc_Absyn_Decl*_tmpEA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=1,({struct Cyc_Absyn_Fndecl*_tmpE9=({struct Cyc_Absyn_Fndecl*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->sc=Cyc_Absyn_Static,_tmp54->is_inline=0,_tmp54->name=funname,_tmp54->body=s,
(_tmp54->i).tvars=0,(_tmp54->i).effect=0,({struct Cyc_Absyn_Tqual _tmpE8=Cyc_Absyn_empty_tqual(0U);(_tmp54->i).ret_tqual=_tmpE8;}),(_tmp54->i).ret_type=rettype,(_tmp54->i).args=params,(_tmp54->i).c_varargs=0,(_tmp54->i).cyc_varargs=0,(_tmp54->i).rgn_po=0,(_tmp54->i).attributes=0,(_tmp54->i).requires_clause=0,(_tmp54->i).requires_relns=0,(_tmp54->i).ensures_clause=0,(_tmp54->i).ensures_relns=0,(_tmp54->i).return_value=0,_tmp54->cached_type=0,_tmp54->param_vardecls=0,_tmp54->fn_vardecl=0,_tmp54->orig_scope=Cyc_Absyn_Static;_tmp54;});
# 377
_tmp55->f1=_tmpE9;});_tmp55;}),0U);_tmp56->hd=_tmpEA;}),_tmp56->tl=*env.gen_ds;_tmp56;});
# 376
*env.gen_ds=_tmpEB;});
# 384
return({struct Cyc_Absyn_Exp*_tmpEC=Cyc_Absyn_var_exp(funname,0U);Cyc_Absyn_fncall_exp(_tmpEC,args,0U);});}}}
# 387
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_tmp58=e->r;void*_stmttmp6=_tmp58;void*_tmp59=_stmttmp6;void*_tmp5C;void*_tmp5B;void*_tmp5A;switch(*((int*)_tmp59)){case 1: _tmp5A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59)->f1;{void*b=_tmp5A;
# 390
struct _tuple0*_tmp5D=Cyc_Absyn_binding2qvar(b);struct _tuple0*_stmttmp7=_tmp5D;struct _tuple0*_tmp5E=_stmttmp7;void*_tmp5F;if(((((struct _tuple0*)_tmp5E)->f1).Loc_n).tag == 4){_tmp5F=_tmp5E->f2;{struct _fat_ptr*v=_tmp5F;
# 392
if(((int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member)(varmap,v))
({void*_tmpED=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup)(varmap,v))->r;e->r=_tmpED;});
return;}}else{
return;};}case 0:
# 399
 return;case 10: _tmp5A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_tmp5B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp59)->f2;{struct Cyc_Absyn_Exp*e1=_tmp5A;struct Cyc_List_List*es=_tmp5B;
# 401
Cyc_Tovc_apply_varmap(varmap,e1);_tmp5A=es;goto _LL8;}case 3: _tmp5A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59)->f2;_LL8: {struct Cyc_List_List*es=_tmp5A;
# 403
for(1;es != 0;es=es->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 6: _tmp5A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_tmp5B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59)->f2;_tmp5C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59)->f3;{struct Cyc_Absyn_Exp*e1=_tmp5A;struct Cyc_Absyn_Exp*e2=_tmp5B;struct Cyc_Absyn_Exp*e3=_tmp5C;
# 408
Cyc_Tovc_apply_varmap(varmap,e3);_tmp5A=e1;_tmp5B=e2;goto _LLC;}case 7: _tmp5A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_tmp5B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp59)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp5A;struct Cyc_Absyn_Exp*e2=_tmp5B;
_tmp5A=e1;_tmp5B=e2;goto _LLE;}case 8: _tmp5A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_tmp5B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp59)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp5A;struct Cyc_Absyn_Exp*e2=_tmp5B;
_tmp5A=e1;_tmp5B=e2;goto _LL10;}case 23: _tmp5A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_tmp5B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp59)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp5A;struct Cyc_Absyn_Exp*e2=_tmp5B;
_tmp5A=e1;_tmp5B=e2;goto _LL12;}case 9: _tmp5A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_tmp5B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5A;struct Cyc_Absyn_Exp*e2=_tmp5B;
_tmp5A=e1;_tmp5B=e2;goto _LL14;}case 4: _tmp5A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_tmp5B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp59)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5A;struct Cyc_Absyn_Exp*e2=_tmp5B;
# 414
Cyc_Tovc_apply_varmap(varmap,e2);_tmp5A=e1;goto _LL16;}case 12: _tmp5A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp5A;
# 416
_tmp5A=e1;goto _LL18;}case 13: _tmp5A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp5A;
_tmp5A=e1;goto _LL1A;}case 14: _tmp5A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp5A;
_tmp5A=e1;goto _LL1C;}case 15: _tmp5A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp5A;
_tmp5A=e1;goto _LL1E;}case 18: _tmp5A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp5A;
_tmp5A=e1;goto _LL20;}case 20: _tmp5A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp5A;
_tmp5A=e1;goto _LL22;}case 21: _tmp5A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp5A;
_tmp5A=e1;goto _LL24;}case 22: _tmp5A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp5A;
_tmp5A=e1;goto _LL26;}case 5: _tmp5A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp5A;
# 425
Cyc_Tovc_apply_varmap(varmap,e1);
return;}case 17:
# 428
 goto _LL2A;case 19: _LL2A:
 goto _LL2C;case 32: _LL2C:
 return;case 36: _tmp5A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp59)->f2;{struct Cyc_List_List*dles=_tmp5A;
# 433
for(1;dles != 0;dles=dles->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple11*)dles->hd)).f2);}
return;}default:
# 437
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmpEE=({const char*_tmp60="bad exp form in Tocv::apply_varmap";_tag_fat(_tmp60,sizeof(char),35U);});_tmp61->f1=_tmpEE;});_tmp61;}));};}
# 441
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_tmp62=s->r;void*_stmttmp8=_tmp62;void*_tmp63=_stmttmp8;void*_tmp66;void*_tmp65;void*_tmp64;switch(*((int*)_tmp63)){case 0:
 return Cyc_Absyn_true_exp(0U);case 1: _tmp64=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp63)->f1;{struct Cyc_Absyn_Exp*e=_tmp64;
# 445
Cyc_Tovc_apply_varmap(env.varmap,e);
return e;}case 2: _tmp64=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp63)->f1;_tmp65=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp63)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp64;struct Cyc_Absyn_Stmt*s2=_tmp65;
# 449
struct Cyc_Absyn_Exp*_tmp67=Cyc_Tovc_stmt_to_exp(env,s1);struct Cyc_Absyn_Exp*s1_new=_tmp67;
struct Cyc_Absyn_Exp*_tmp68=Cyc_Tovc_stmt_to_exp(env,s2);struct Cyc_Absyn_Exp*s2_new=_tmp68;
return Cyc_Absyn_seq_exp(s1_new,s2_new,0U);}case 4: _tmp64=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp63)->f1;_tmp65=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp63)->f2;_tmp66=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp63)->f3;{struct Cyc_Absyn_Exp*e=_tmp64;struct Cyc_Absyn_Stmt*s1=_tmp65;struct Cyc_Absyn_Stmt*s2=_tmp66;
# 453
Cyc_Tovc_apply_varmap(env.varmap,e);{
# 455
struct Cyc_Absyn_Exp*_tmp69=Cyc_Tovc_stmt_to_exp(env,s1);struct Cyc_Absyn_Exp*s1_new=_tmp69;
struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tovc_stmt_to_exp(env,s2);struct Cyc_Absyn_Exp*s2_new=_tmp6A;
return Cyc_Absyn_conditional_exp(e,s1_new,s2_new,0U);}}case 12: _tmp64=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp63)->f1;_tmp65=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp63)->f2;{struct Cyc_Absyn_Decl*d=_tmp64;struct Cyc_Absyn_Stmt*s2=_tmp65;
# 459
void*_tmp6B=d->r;void*_stmttmp9=_tmp6B;void*_tmp6C=_stmttmp9;void*_tmp6D;if(*((int*)_tmp6C)== 0){_tmp6D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6C)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp6D;
# 462
struct _fat_ptr*_tmp6E=(*vd->name).f2;struct _fat_ptr*oldname=_tmp6E;
struct _tuple0*_tmp6F=Cyc_Toc_temp_var();struct _tuple0*newqvar=_tmp6F;
vd->name=newqvar;
({struct Cyc_Dict_Dict _tmpF1=({struct Cyc_Dict_Dict _tmpF0=env.varmap;struct _fat_ptr*_tmpEF=oldname;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Exp*))Cyc_Dict_insert)(_tmpF0,_tmpEF,Cyc_Absyn_var_exp(newqvar,0U));});env.varmap=_tmpF1;});{
struct Cyc_List_List*_tmp70=0;struct Cyc_List_List*unresolvedmem_inits=_tmp70;
if(vd->initializer != 0){
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));{
void*_tmp71=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmpA=_tmp71;void*_tmp72=_stmttmpA;void*_tmp73;if(*((int*)_tmp72)== 36){_tmp73=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp72)->f2;{struct Cyc_List_List*dles=_tmp73;
# 481 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmp74=Cyc_Absyn_var_exp(vd->name,0U);struct Cyc_Absyn_Exp*var=_tmp74;
{unsigned i=0U;for(0;dles != 0;(dles=dles->tl,++ i)){
unresolvedmem_inits=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(*_tmp75));({struct Cyc_Absyn_Exp*_tmpF4=({struct Cyc_Absyn_Exp*_tmpF3=({struct Cyc_Absyn_Exp*_tmpF2=var;Cyc_Absyn_subscript_exp(_tmpF2,Cyc_Absyn_uint_exp(i,0U),0U);});Cyc_Absyn_assign_exp(_tmpF3,(*((struct _tuple11*)dles->hd)).f2,0U);});_tmp75->hd=_tmpF4;}),_tmp75->tl=unresolvedmem_inits;_tmp75;});}}
# 487
vd->initializer=0;
goto _LL12;}}else{
goto _LL12;}_LL12:;}}
# 492
({struct Cyc_List_List*_tmpF5=({struct Cyc_List_List*_tmp76=_region_malloc(env.rgn,sizeof(*_tmp76));_tmp76->hd=vd,_tmp76->tl=env.all_locals;_tmp76;});env.all_locals=_tmpF5;});
# 494
({void*_tmpF7=({struct Cyc_Absyn_Decl*_tmpF6=d;Cyc_Absyn_decl_stmt(_tmpF6,Cyc_Absyn_new_stmt((env.encloser)->r,0U),0U);})->r;(env.encloser)->r=_tmpF7;});{
struct Cyc_Absyn_Exp*_tmp77=Cyc_Tovc_stmt_to_exp(env,s2);struct Cyc_Absyn_Exp*e=_tmp77;
if(vd->initializer != 0){
e=({struct Cyc_Absyn_Exp*_tmpF9=({struct Cyc_Absyn_Exp*_tmpF8=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_assign_exp(_tmpF8,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),0U);});Cyc_Absyn_seq_exp(_tmpF9,e,0U);});
# 499
vd->initializer=0;}
# 501
for(1;unresolvedmem_inits != 0;unresolvedmem_inits=unresolvedmem_inits->tl){
e=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)unresolvedmem_inits->hd,e,0U);}
return e;}}}}else{
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmpFA=({const char*_tmp78="bad local decl in Tovc::stmt_to_exp";_tag_fat(_tmp78,sizeof(char),36U);});_tmp79->f1=_tmpFA;});_tmp79;}));};}default:
# 507
 return Cyc_Tovc_stmt_to_fun(env,s,Cyc_Absyn_void_type);};}
# 511
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv,struct Cyc_Absyn_Stmt*);
# 513
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
struct _fat_ptr bad_form=({const char*_tmp98="";_tag_fat(_tmp98,sizeof(char),1U);});
{void*_tmp7A=e->r;void*_stmttmpB=_tmp7A;void*_tmp7B=_stmttmpB;void*_tmp7E;void*_tmp7D;void*_tmp7C;switch(*((int*)_tmp7B)){case 0:
 return;case 1:
 return;case 10: _tmp7C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;{struct Cyc_Absyn_Exp*e1=_tmp7C;struct Cyc_List_List*es=_tmp7D;
# 520
Cyc_Tovc_exp_to_vc(env,e1);_tmp7C=es;goto _LL8;}case 3: _tmp7C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;_LL8: {struct Cyc_List_List*es=_tmp7C;
# 522
for(1;es != 0;es=es->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 6: _tmp7C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;_tmp7E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7B)->f3;{struct Cyc_Absyn_Exp*e1=_tmp7C;struct Cyc_Absyn_Exp*e2=_tmp7D;struct Cyc_Absyn_Exp*e3=_tmp7E;
# 527
Cyc_Tovc_exp_to_vc(env,e3);_tmp7C=e1;_tmp7D=e2;goto _LLC;}case 7: _tmp7C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp7C;struct Cyc_Absyn_Exp*e2=_tmp7D;
_tmp7C=e1;_tmp7D=e2;goto _LLE;}case 8: _tmp7C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp7C;struct Cyc_Absyn_Exp*e2=_tmp7D;
_tmp7C=e1;_tmp7D=e2;goto _LL10;}case 23: _tmp7C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp7C;struct Cyc_Absyn_Exp*e2=_tmp7D;
_tmp7C=e1;_tmp7D=e2;goto _LL12;}case 9: _tmp7C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp7C;struct Cyc_Absyn_Exp*e2=_tmp7D;
_tmp7C=e1;_tmp7D=e2;goto _LL14;}case 4: _tmp7C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7B)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp7C;struct Cyc_Absyn_Exp*e2=_tmp7D;
# 533
Cyc_Tovc_exp_to_vc(env,e2);_tmp7C=e1;goto _LL16;}case 41: _tmp7C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp7C;
# 535
_tmp7C=e1;goto _LL18;}case 12: _tmp7C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7C=e1;goto _LL1A;}case 13: _tmp7C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7C=e1;goto _LL1C;}case 14: _tmp7C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7C=e1;goto _LL1E;}case 15: _tmp7C=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7C=e1;goto _LL20;}case 18: _tmp7C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7C=e1;goto _LL22;}case 20: _tmp7C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7C=e1;goto _LL24;}case 21: _tmp7C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7C=e1;goto _LL26;}case 22: _tmp7C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7C=e1;goto _LL28;}case 5: _tmp7C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp7C;
# 545
Cyc_Tovc_exp_to_vc(env,e1);
return;}case 17:
# 548
 goto _LL2C;case 19: _LL2C:
 goto _LL2E;case 33: _LL2E:
 goto _LL30;case 32: _LL30:
 return;case 36: _tmp7C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp7B)->f2;{struct Cyc_List_List*dles=_tmp7C;
# 557
for(1;dles != 0;dles=dles->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}case 37: _tmp7C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;{struct Cyc_Absyn_Stmt*s=_tmp7C;
# 565
Cyc_Tovc_stmt_to_vc(env,s);
# 567
({void*_tmpFB=(Cyc_Tovc_stmt_to_exp(env,s))->r;e->r=_tmpFB;});
return;}case 40:
# 570
 return;case 42:
# 572
 bad_form=({const char*_tmp7F="Assert_e";_tag_fat(_tmp7F,sizeof(char),9U);});goto _LL0;case 2:
 bad_form=({const char*_tmp80="Pragma_e";_tag_fat(_tmp80,sizeof(char),9U);});goto _LL0;case 11:
 bad_form=({const char*_tmp81="Throw_e";_tag_fat(_tmp81,sizeof(char),8U);});goto _LL0;case 16:
 bad_form=({const char*_tmp82="New_e";_tag_fat(_tmp82,sizeof(char),6U);});goto _LL0;case 24:
 bad_form=({const char*_tmp83="Tuple_e";_tag_fat(_tmp83,sizeof(char),8U);});goto _LL0;case 25:
 bad_form=({const char*_tmp84="CompoundLit_e";_tag_fat(_tmp84,sizeof(char),14U);});goto _LL0;case 26:
 bad_form=({const char*_tmp85="Array_e";_tag_fat(_tmp85,sizeof(char),8U);});goto _LL0;case 27:
 bad_form=({const char*_tmp86="Comprehension_e";_tag_fat(_tmp86,sizeof(char),16U);});goto _LL0;case 28:
 bad_form=({const char*_tmp87="ComprehensionNoinit_e";_tag_fat(_tmp87,sizeof(char),22U);});goto _LL0;case 29:
 bad_form=({const char*_tmp88="Aggregate_e";_tag_fat(_tmp88,sizeof(char),12U);});goto _LL0;case 30:
 bad_form=({const char*_tmp89="AnonStruct_e";_tag_fat(_tmp89,sizeof(char),13U);});goto _LL0;case 31:
# 584
 bad_form=({const char*_tmp8A="Datatype_e";_tag_fat(_tmp8A,sizeof(char),11U);});goto _LL0;case 34:
 bad_form=({const char*_tmp8B="Malloc_e";_tag_fat(_tmp8B,sizeof(char),9U);});goto _LL0;case 35:
 bad_form=({const char*_tmp8C="Swap_e";_tag_fat(_tmp8C,sizeof(char),7U);});goto _LL0;case 38:
 bad_form=({const char*_tmp8D="Tagcheck_e";_tag_fat(_tmp8D,sizeof(char),11U);});goto _LL0;default:
 bad_form=({const char*_tmp8E="Valueof_e";_tag_fat(_tmp8E,sizeof(char),10U);});goto _LL0;}_LL0:;}
# 590
({struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC8;_tmpC8.tag=0,({struct _fat_ptr _tmpFC=({const char*_tmp97="bad exp form ";_tag_fat(_tmp97,sizeof(char),14U);});_tmpC8.f1=_tmpFC;});_tmpC8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp91=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC7;_tmpC7.tag=0,_tmpC7.f1=bad_form;_tmpC7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp92=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC6;_tmpC6.tag=0,({struct _fat_ptr _tmpFD=({const char*_tmp96="(exp=|";_tag_fat(_tmp96,sizeof(char),7U);});_tmpC6.f1=_tmpFD;});_tmpC6;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp93=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmpC5;_tmpC5.tag=4,_tmpC5.f1=e;_tmpC5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp94=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC4;_tmpC4.tag=0,({struct _fat_ptr _tmpFE=({const char*_tmp95="|) after xlation to C";_tag_fat(_tmp95,sizeof(char),22U);});_tmpC4.f1=_tmpFE;});_tmpC4;});void*_tmp8F[5];_tmp8F[0]=& _tmp90,_tmp8F[1]=& _tmp91,_tmp8F[2]=& _tmp92,_tmp8F[3]=& _tmp93,_tmp8F[4]=& _tmp94;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp8F,sizeof(void*),5));});}
# 593
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 595
while(1){
env.encloser=s;{
void*_tmp99=s->r;void*_stmttmpC=_tmp99;void*_tmp9A=_stmttmpC;void*_tmp9E;void*_tmp9D;void*_tmp9C;void*_tmp9B;switch(*((int*)_tmp9A)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 13: _tmp9B=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9A)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp9B;
# 603
s=s2;
continue;}case 3: _tmp9B=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp9B;
# 606
if(eopt == 0)
return;
_tmp9B=eopt;goto _LLE;}case 1: _tmp9B=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp9B;
# 610
Cyc_Tovc_exp_to_vc(env,e);
return;}case 2: _tmp9B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1;_tmp9C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9A)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp9B;struct Cyc_Absyn_Stmt*s2=_tmp9C;
# 613
Cyc_Tovc_stmt_to_vc(env,s1);
s=s2;
continue;}case 4: _tmp9B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1;_tmp9C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9A)->f2;_tmp9D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9A)->f3;{struct Cyc_Absyn_Exp*e=_tmp9B;struct Cyc_Absyn_Stmt*s1=_tmp9C;struct Cyc_Absyn_Stmt*s2=_tmp9D;
# 617
Cyc_Tovc_exp_to_vc(env,e);
Cyc_Tovc_stmt_to_vc(env,s1);
s=s2;
continue;}case 5: _tmp9B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1).f1;_tmp9C=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9A)->f2;{struct Cyc_Absyn_Exp*e=_tmp9B;struct Cyc_Absyn_Stmt*s2=_tmp9C;
# 622
Cyc_Tovc_exp_to_vc(env,e);
s=s2;
continue;}case 9: _tmp9B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1;_tmp9C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9A)->f2).f1;_tmp9D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9A)->f3).f1;_tmp9E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9A)->f4;{struct Cyc_Absyn_Exp*e1=_tmp9B;struct Cyc_Absyn_Exp*e2=_tmp9C;struct Cyc_Absyn_Exp*e3=_tmp9D;struct Cyc_Absyn_Stmt*s2=_tmp9E;
# 626
Cyc_Tovc_exp_to_vc(env,e1);
Cyc_Tovc_exp_to_vc(env,e2);
Cyc_Tovc_exp_to_vc(env,e3);
s=s2;
continue;}case 14: _tmp9B=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1;_tmp9C=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9A)->f2).f1;{struct Cyc_Absyn_Stmt*s2=_tmp9B;struct Cyc_Absyn_Exp*e=_tmp9C;
# 632
Cyc_Tovc_exp_to_vc(env,e);
s=s2;
continue;}case 10: _tmp9B=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1;_tmp9C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9A)->f2;{struct Cyc_Absyn_Exp*e=_tmp9B;struct Cyc_List_List*scs=_tmp9C;
# 638
Cyc_Tovc_exp_to_vc(env,e);
for(1;scs != 0;scs=scs->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _tmp9B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9A)->f1;_tmp9C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9A)->f2;{struct Cyc_Absyn_Decl*d=_tmp9B;struct Cyc_Absyn_Stmt*s2=_tmp9C;
# 643
{void*_tmp9F=d->r;void*_stmttmpD=_tmp9F;void*_tmpA0=_stmttmpD;void*_tmpA1;if(*((int*)_tmpA0)== 0){_tmpA1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA0)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpA1;
# 645
if(Cyc_Tovc_elim_array_initializers){
# 650
void*_tmpA2=Cyc_Absyn_compress(vd->type);void*_stmttmpE=_tmpA2;void*_tmpA3=_stmttmpE;if(*((int*)_tmpA3)== 4){
# 652
if(vd->initializer != 0){
void*_tmpA4=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmpF=_tmpA4;void*_tmpA5=_stmttmpF;void*_tmpA6;if(*((int*)_tmpA5)== 36){_tmpA6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;{struct Cyc_List_List*dles=_tmpA6;
# 655
vd->initializer=0;{
# 657
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(vd->name,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(s2->r,0U);
{unsigned i=0U;for(0;dles != 0;(dles=dles->tl,++ i)){
s3=({struct Cyc_Absyn_Stmt*_tmp101=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp100=({struct Cyc_Absyn_Exp*_tmpFF=arr;Cyc_Absyn_subscript_exp(_tmpFF,
Cyc_Absyn_uint_exp(i,0U),0U);});
# 660
Cyc_Absyn_assign_exp(_tmp100,(*((struct _tuple11*)dles->hd)).f2,0U);}),0U);Cyc_Absyn_seq_stmt(_tmp101,s3,0U);});}}
# 665
s2->r=s3->r;
goto _LL29;}}}else{
# 668
struct _fat_ptr _tmpA7=({struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmpC9;_tmpC9.tag=0,({
struct _fat_ptr _tmp102=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmpC9.f1=_tmp102;});_tmpC9;});void*_tmpA9[1];_tmpA9[0]=& _tmpAB;({struct _fat_ptr _tmp103=({const char*_tmpAA="|%s|: array type bad initializer";_tag_fat(_tmpAA,sizeof(char),33U);});Cyc_aprintf(_tmp103,_tag_fat(_tmpA9,sizeof(void*),1));});});
# 668
struct _fat_ptr msg=_tmpA7;
# 670
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=Cyc_Core_Impossible,_tmpA8->f1=(struct _fat_ptr)msg;_tmpA8;}));}_LL29:;}
# 672
goto _LL24;}else{
goto _LL24;}_LL24:;}
# 676
if(vd->initializer != 0)
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
({struct Cyc_List_List*_tmp104=({struct Cyc_List_List*_tmpAC=_region_malloc(env.rgn,sizeof(*_tmpAC));_tmpAC->hd=vd,_tmpAC->tl=env.all_locals;_tmpAC;});env.all_locals=_tmp104;});
s=s2;
continue;}}else{
goto _LL1F;}_LL1F:;}
# 683
goto _LL1E;}default: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCA;_tmpCA.tag=0,({struct _fat_ptr _tmp105=({const char*_tmpAF="bad stmt after xlation to C";_tag_fat(_tmpAF,sizeof(char),28U);});_tmpCA.f1=_tmp105;});_tmpCA;});void*_tmpAD[1];_tmpAD[0]=& _tmpAE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpAD,sizeof(void*),1));});};}}}
# 689
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*_tmpB0=0;struct Cyc_List_List*new_ds=_tmpB0;
for(1;old_ds != 0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*_tmpB1=(struct Cyc_Absyn_Decl*)old_ds->hd;struct Cyc_Absyn_Decl*next_d=_tmpB1;
void*_tmpB2=next_d->r;void*_stmttmp10=_tmpB2;void*_tmpB3=_stmttmp10;void*_tmpB4;switch(*((int*)_tmpB3)){case 1: _tmpB4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpB3)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpB4;
# 698
fd->cached_type=0;
# 704
if(fd->param_vardecls != 0){
struct _RegionHandle _tmpB5=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB5;_push_region(rgn);
{struct Cyc_List_List**_tmpB6=({struct Cyc_List_List**_tmpBA=_region_malloc(rgn,sizeof(*_tmpBA));*_tmpBA=0;_tmpBA;});struct Cyc_List_List**gen_ds=_tmpB6;
({struct Cyc_Tovc_ToExpEnv _tmp108=({struct Cyc_Tovc_ToExpEnv _tmpCB;_tmpCB.rgn=rgn,({
struct Cyc_List_List*_tmp107=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v);_tmpCB.all_locals=_tmp107;}),({
struct Cyc_Dict_Dict _tmp106=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp);_tmpCB.varmap=_tmp106;}),_tmpCB.encloser=fd->body,_tmpCB.gen_ds=gen_ds;_tmpCB;});
# 707
Cyc_Tovc_stmt_to_vc(_tmp108,fd->body);});
# 713
if(*gen_ds != 0){
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->sc=fd->sc,_tmpB9->name=fd->name,_tmpB9->varloc=0U,({struct Cyc_Absyn_Tqual _tmp10A=Cyc_Absyn_const_tqual(0U);_tmpB9->tq=_tmp10A;}),({
void*_tmp109=Cyc_Tcutil_fndecl2type(fd);_tmpB9->type=_tmp109;}),_tmpB9->initializer=0,_tmpB9->rgn=0,_tmpB9->attributes=(fd->i).attributes,_tmpB9->escapes=0,_tmpB9->is_proto=fd->body != 0;_tmpB9;});
# 717
new_ds=({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct Cyc_Absyn_Decl*_tmp10B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->tag=0,_tmpB7->f1=vd;_tmpB7;}),0U);_tmpB8->hd=_tmp10B;}),_tmpB8->tl=new_ds;_tmpB8;});
new_ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(*gen_ds,new_ds);}}
# 706
;_pop_region();}
# 721
goto _LL4;}case 0: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 5: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 new_ds=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->hd=next_d,_tmpBB->tl=new_ds;_tmpBB;});goto _LL0;case 9:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
# 738
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBD=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCC;_tmpCC.tag=0,({struct _fat_ptr _tmp10C=({const char*_tmpBE="nested translation unit after translation to C";_tag_fat(_tmpBE,sizeof(char),47U);});_tmpCC.f1=_tmp10C;});_tmpCC;});void*_tmpBC[1];_tmpBC[0]=& _tmpBD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpBC,sizeof(void*),1));});}_LL0:;}
# 741
return((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(new_ds);}

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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 111 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 126
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 275 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 37 "position.h"
struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 931 "absyn.h"
extern void*Cyc_Absyn_void_type;
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
# 1062
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple2*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1086
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1090
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple2*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc);
# 1104
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple2*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc);
# 1107
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple2*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc);
# 1110
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple2*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc);
# 1113
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple2*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc);
# 1118
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple2*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc);
# 1124
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1136
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1138
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1141
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1143
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);
# 1163
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1165
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1167
int Cyc_Absyn_is_require_union_type(void*);struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 36
int Cyc_Tcutil_is_void_type(void*);
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
int Cyc_Tcutil_is_signed_type(void*);
int Cyc_Tcutil_is_function_type(void*);
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 49
int Cyc_Tcutil_is_fat_ptr(void*);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
int Cyc_Tcutil_is_nullable_pointer_type(void*);
int Cyc_Tcutil_is_bound_one(void*);
# 54
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 57
int Cyc_Tcutil_is_bits_only_type(void*);
# 59
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 64
void*Cyc_Tcutil_pointer_elt_type(void*);
# 66
void*Cyc_Tcutil_pointer_region(void*);
# 69
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 75
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 77
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 79
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 81
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type);
# 84
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 87
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 94
void*Cyc_Tcutil_copy_type(void*);
# 97
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 104
void*Cyc_Tcutil_fd_type(struct Cyc_Absyn_Fndecl*);
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_use(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 117
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned,void*,void*);
# 119
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned,void*,void*);
# 121
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple12{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 125
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 172
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 176
struct _tuple13 Cyc_Tcutil_swap_kind(void*,void*);
# 181
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_Absyn_Exp*);
# 183
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 187
void Cyc_Tcutil_explain_failure();
# 189
int Cyc_Tcutil_unify(void*,void*);
# 191
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 194
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 196
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 202
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 205
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 209
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 211
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 215
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple14{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple14*,struct Cyc_Absyn_Tvar*);
# 221
void Cyc_Tcutil_check_bitfield(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn);
# 224
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 231
void Cyc_Tcutil_check_nonzero_bound(unsigned,void*);
# 233
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 235
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 242
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**ptr_type,int*is_fat,void**elt_type);
# 246
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 249
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 254
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 259
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple15{int f1;void*f2;};
# 263
struct _tuple15 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*e);
# 266
void*Cyc_Tcutil_normalize_effect(void*e);
# 269
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 271
int Cyc_Tcutil_new_tvar_id();
# 273
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 276
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 278
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 281
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 284
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 287
void*Cyc_Tcutil_snd_tqt(struct _tuple16*);
# 291
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 295
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 298
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 302
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 305
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 309
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 312
int Cyc_Tcutil_zeroable_type(void*);
# 316
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 319
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**);
# 321
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**);
# 324
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 327
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct _tuple17{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple17 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple18{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 64
struct _tuple18 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 68
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 100
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 103
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 108
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 112
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 118
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 121
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _fat_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 153
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env();
# 155
int Cyc_CfFlowInfo_get_field_index(void*t,struct _fat_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f);
# 158
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 161
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple20{void*f1;struct Cyc_List_List*f2;};
struct _tuple20 Cyc_CfFlowInfo_unname_rval(void*rv);
# 168
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 171
int Cyc_CfFlowInfo_is_init_pointer(void*r);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 181
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 196 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 201
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple21 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple21 pr1,struct _tuple21 pr2);
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 28
void Cyc_Warn_warn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 30
void Cyc_Warn_flush_warnings();
# 32
void Cyc_Warn_verr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 34
void Cyc_Warn_err(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);
# 39
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap);
# 42
void*Cyc_Warn_vimpos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 45
void*Cyc_Warn_impos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 51 "new_control_flow.cyc"
static int Cyc_NewControlFlow_strcmp(struct _fat_ptr s1,struct _fat_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned sz1=_get_fat_size(s1,sizeof(char));
unsigned sz2=_get_fat_size(s2,sizeof(char));
unsigned minsz=sz1 < sz2?sz1: sz2;
# 59
while((unsigned)i < minsz){
char c1=*((const char*)_check_fat_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_fat_subscript(s2,sizeof(char),i));
if((int)c1 == (int)'\000'){
if((int)c2 == (int)'\000')return 0;else{
return - 1;}}else{
if((int)c2 == (int)'\000')return 1;else{
# 67
int diff=(int)c1 - (int)c2;
if(diff != 0)return diff;}}
# 70
++ i;}
# 72
if(sz1 == sz2)return 0;
if(minsz < sz2){
if((int)*((const char*)_check_fat_subscript(s2,sizeof(char),i))== (int)'\000')return 0;else{
return - 1;}}else{
# 77
if((int)*((const char*)_check_fat_subscript(s1,sizeof(char),i))== (int)'\000')return 0;else{
return 1;}}};}
# 83
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};struct _tuple22{void*f1;int f2;};
# 125 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple22*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple23 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple21 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple24{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple24 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*,struct Cyc_CfFlowInfo_Place*,void*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 135
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_tmp1=_tmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp3;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp3=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2:
 return _tmp3;}else{_LL3: _LL4:
({void*_tmp639=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_NewControlFlow_CFAnnot,(_tmp2->f1).visited=0;_tmp2;});s->annot=_tmp639;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp4=_cycalloc(sizeof(*_tmp4));({union Cyc_CfFlowInfo_FlowInfo _tmp63A=Cyc_CfFlowInfo_BottomFL();*_tmp4=_tmp63A;});_tmp4;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 149
return*sflow;}
# 152
static struct Cyc_List_List*Cyc_NewControlFlow_flowrelns(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp5=f;struct Cyc_List_List*_tmp6;if((_tmp5.BottomFL).tag == 1){_LL1: _LL2:
 return 0;}else{_LL3: _tmp6=((_tmp5.ReachableFL).val).f2;_LL4:
 return _tmp6;}_LL0:;}struct _tuple25{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 159
static struct _tuple25 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 163
({union Cyc_CfFlowInfo_FlowInfo _tmp63B=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp63B;});
# 169
_tmp7->visited=env->iteration_num;
return({struct _tuple25 _tmp597;_tmp597.f1=_tmp7,_tmp597.f2=_tmp8;_tmp597;});}
# 179
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 187
({union Cyc_CfFlowInfo_FlowInfo _tmp63C=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp63C;});}struct _tuple26{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned f2;struct Cyc_Dict_Dict f3;};
# 194
static void Cyc_NewControlFlow_check_unique_root(struct _tuple26*ckenv,void*root,void*rval){
# 196
struct _tuple26*_tmp9=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp13;unsigned _tmp12;struct Cyc_Dict_Dict _tmp11;_LL1: _tmp13=_tmp9->f1;_tmp12=_tmp9->f2;_tmp11=_tmp9->f3;_LL2:;{
void*_tmpA=root;struct Cyc_Absyn_Vardecl*_tmp10;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpA)->tag == 0U){_LL4: _tmp10=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpA)->f1;_LL5:
# 199
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp11,root,& rval)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp10->type)){
retry: {void*_tmpB=rval;void*_tmpF;switch(*((int*)_tmpB)){case 8U: _LL9: _tmpF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB)->f2;_LLA:
 rval=_tmpF;goto retry;case 7U: _LLB: _LLC:
 goto _LLE;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB)->f1 == Cyc_CfFlowInfo_NoneIL){_LLD: _LLE:
 goto _LL10;}else{goto _LL11;}case 0U: _LLF: _LL10:
 goto _LL8;default: _LL11: _LL12:
# 208
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp598;_tmp598.tag=0U,({struct _fat_ptr _tmp63D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp10->name));_tmp598.f1=_tmp63D;});_tmp598;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({unsigned _tmp63F=_tmp12;struct _fat_ptr _tmp63E=({const char*_tmpD="alias-free pointer(s) reachable from %s may become unreachable.";_tag_fat(_tmpD,sizeof(char),64U);});Cyc_Warn_warn(_tmp63F,_tmp63E,_tag_fat(_tmpC,sizeof(void*),1U));});});
goto _LL8;}_LL8:;}}
# 212
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 220
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp14=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp15=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp16=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*_tmp15);
# 226
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple24 _tmp17=({struct _tuple24 _tmp59A;_tmp59A.f1=flow,_tmp59A.f2=_tmp16;_tmp59A;});struct _tuple24 _tmp18=_tmp17;struct Cyc_Dict_Dict _tmp1B;struct Cyc_Dict_Dict _tmp1A;if(((_tmp18.f1).ReachableFL).tag == 2){if(((_tmp18.f2).ReachableFL).tag == 2){_LL1: _tmp1B=(((_tmp18.f1).ReachableFL).val).f1;_tmp1A=(((_tmp18.f2).ReachableFL).val).f1;_LL2: {
# 229
struct _tuple26 _tmp19=({struct _tuple26 _tmp599;_tmp599.f1=env,_tmp599.f2=s->loc,_tmp599.f3=_tmp1A;_tmp599;});
((void(*)(void(*f)(struct _tuple26*,void*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp19,_tmp1B);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 235
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp16,*_tmp15)){
# 242
*_tmp15=_tmp16;
# 246
if(_tmp14->visited == env->iteration_num)
# 248
env->iterate_again=1;}}
# 253
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned loc,int nameit){
# 257
union Cyc_CfFlowInfo_FlowInfo _tmp1C=inflow;struct Cyc_Dict_Dict _tmp22;struct Cyc_List_List*_tmp21;if((_tmp1C.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp22=((_tmp1C.ReachableFL).val).f1;_tmp21=((_tmp1C.ReachableFL).val).f2;_LL4:
# 260
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp1D=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=0U,_tmp20->f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp20;});
void*_tmp1E=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
_tmp1E=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=8U,_tmp1F->f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp1F->f2=_tmp1E;_tmp1F;});
# 267
_tmp22=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp22,(void*)_tmp1D,_tmp1E);}
# 269
return Cyc_CfFlowInfo_ReachableFL(_tmp22,_tmp21);}_LL0:;}
# 273
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 280
for(0;(unsigned)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp23=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->hd=_tmp23,_tmp24->tl=assume;_tmp24;})))
return 0;}
# 285
return 1;}
# 288
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp25=e->r;void*_tmp26=_tmp25;struct Cyc_Absyn_Exp*_tmp27;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->tag == 14U){_LL1: _tmp27=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->f2;_LL2:
 return _tmp27;}else{_LL3: _LL4:
 return e;}_LL0:;}
# 295
static void Cyc_NewControlFlow_check_union_requires(unsigned loc,void*t,struct _fat_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 297
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;struct Cyc_List_List*_tmp46;if((_tmp28.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp46=((_tmp28.ReachableFL).val).f2;_LL4:
# 300
{void*_tmp29=Cyc_Tcutil_compress(t);void*_tmp2A=_tmp29;struct Cyc_List_List*_tmp45;union Cyc_Absyn_AggrInfo _tmp44;struct Cyc_List_List*_tmp43;switch(*((int*)_tmp2A)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f1)->tag == 20U){_LL6: _tmp44=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f1)->f1;_tmp43=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f2;_LL7: {
# 302
struct Cyc_Absyn_Aggrdecl*_tmp2B=Cyc_Absyn_get_known_aggrdecl(_tmp44);
struct Cyc_Absyn_Aggrfield*_tmp2C=Cyc_Absyn_lookup_decl_field(_tmp2B,f);
struct Cyc_Absyn_Exp*_tmp2D=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp2C))->requires_clause;
if(_tmp2D != 0){
struct _RegionHandle _tmp2E=_new_region("temp");struct _RegionHandle*temp=& _tmp2E;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp2F=({struct _RegionHandle*_tmp641=temp;struct Cyc_List_List*_tmp640=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp2B->tvs,_tmp43);Cyc_Tcutil_rsubsexp(_tmp641,_tmp640,_tmp2D);});
# 309
if(!({struct Cyc_List_List*_tmp642=_tmp46;Cyc_NewControlFlow_relns_ok(_tmp642,Cyc_Relations_exp2relns(temp,_tmp2F));})){
({struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp59C;_tmp59C.tag=0U,({struct _fat_ptr _tmp643=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp2F)));_tmp59C.f1=_tmp643;});_tmp59C;});struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp59B;_tmp59B.tag=0U,_tmp59B.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp59B;});void*_tmp30[2U];_tmp30[0]=& _tmp32,_tmp30[1]=& _tmp33;({unsigned _tmp645=loc;struct _fat_ptr _tmp644=({const char*_tmp31="unable to prove %s, required to access %s";_tag_fat(_tmp31,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp645,_tmp644,_tag_fat(_tmp30,sizeof(void*),2U));});});
({void*_tmp34=0U;({struct Cyc___cycFILE*_tmp647=Cyc_stderr;struct _fat_ptr _tmp646=({const char*_tmp35="  [recorded facts on path: ";_tag_fat(_tmp35,sizeof(char),28U);});Cyc_fprintf(_tmp647,_tmp646,_tag_fat(_tmp34,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp46);
({void*_tmp36=0U;({struct Cyc___cycFILE*_tmp649=Cyc_stderr;struct _fat_ptr _tmp648=({const char*_tmp37="]\n";_tag_fat(_tmp37,sizeof(char),3U);});Cyc_fprintf(_tmp649,_tmp648,_tag_fat(_tmp36,sizeof(void*),0U));});});}}
# 307
;_pop_region(temp);}
# 316
goto _LL5;}}else{goto _LLA;}case 7U: _LL8: _tmp45=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A)->f2;_LL9: {
# 318
struct Cyc_Absyn_Aggrfield*_tmp38=Cyc_Absyn_lookup_field(_tmp45,f);
struct Cyc_Absyn_Exp*_tmp39=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp38))->requires_clause;
if(_tmp39 != 0){
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
if(!({struct Cyc_List_List*_tmp64A=_tmp46;Cyc_NewControlFlow_relns_ok(_tmp64A,Cyc_Relations_exp2relns(temp,_tmp39));})){
({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp59E;_tmp59E.tag=0U,({struct _fat_ptr _tmp64B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp39)));_tmp59E.f1=_tmp64B;});_tmp59E;});struct Cyc_String_pa_PrintArg_struct _tmp3E=({struct Cyc_String_pa_PrintArg_struct _tmp59D;_tmp59D.tag=0U,_tmp59D.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp59D;});void*_tmp3B[2U];_tmp3B[0]=& _tmp3D,_tmp3B[1]=& _tmp3E;({unsigned _tmp64D=loc;struct _fat_ptr _tmp64C=({const char*_tmp3C="unable to prove %s, required to access %s";_tag_fat(_tmp3C,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp64D,_tmp64C,_tag_fat(_tmp3B,sizeof(void*),2U));});});
({void*_tmp3F=0U;({struct Cyc___cycFILE*_tmp64F=Cyc_stderr;struct _fat_ptr _tmp64E=({const char*_tmp40="  [recorded facts on path: ";_tag_fat(_tmp40,sizeof(char),28U);});Cyc_fprintf(_tmp64F,_tmp64E,_tag_fat(_tmp3F,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp46);
({void*_tmp41=0U;({struct Cyc___cycFILE*_tmp651=Cyc_stderr;struct _fat_ptr _tmp650=({const char*_tmp42="]\n";_tag_fat(_tmp42,sizeof(char),3U);});Cyc_fprintf(_tmp651,_tmp650,_tag_fat(_tmp41,sizeof(void*),0U));});});}
# 322
;_pop_region(temp);}
# 329
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 332
goto _LL0;}_LL0:;}
# 336
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp47=inflow;struct Cyc_Dict_Dict _tmp4D;struct Cyc_List_List*_tmp4C;if((_tmp47.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp4D=((_tmp47.ReachableFL).val).f1;_tmp4C=((_tmp47.ReachableFL).val).f2;_LL4:
# 340
 if((int)Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D,r)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp48=0U;({unsigned _tmp653=loc;struct _fat_ptr _tmp652=({const char*_tmp49="expression may not be fully initialized";_tag_fat(_tmp49,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp653,_tmp652,_tag_fat(_tmp48,sizeof(void*),0U));});});{
struct Cyc_Dict_Dict _tmp4A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp4D,r);
if(_tmp4D.t == _tmp4A.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B=Cyc_CfFlowInfo_ReachableFL(_tmp4A,_tmp4C);
Cyc_NewControlFlow_update_tryflow(env,_tmp4B);
return _tmp4B;};};}_LL0:;}
# 350
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,void*r){
struct _tuple0 _tmp4E=({struct _tuple0 _tmp59F;({void*_tmp654=Cyc_Tcutil_compress(t);_tmp59F.f1=_tmp654;}),_tmp59F.f2=r;_tmp59F;});struct _tuple0 _tmp4F=_tmp4E;enum Cyc_Absyn_AggrKind _tmp60;struct Cyc_List_List*_tmp5F;struct _fat_ptr _tmp5E;struct Cyc_List_List*_tmp5D;struct _fat_ptr _tmp5C;union Cyc_Absyn_AggrInfo _tmp5B;struct Cyc_List_List*_tmp5A;struct _fat_ptr _tmp59;struct Cyc_Absyn_Datatypefield*_tmp58;struct _fat_ptr _tmp57;void*_tmp56;switch(*((int*)_tmp4F.f2)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
 return;}else{switch(*((int*)_tmp4F.f1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 20U: goto _LL13;default: goto _LL13;}case 6U: goto _LL13;case 7U: goto _LL13;case 3U: goto _LL11;default: goto _LL13;}}case 0U: _LL3: _LL4:
 return;case 7U: _LL5: _LL6:
 return;case 8U: _LL7: _tmp56=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL8:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp56);return;default: switch(*((int*)_tmp4F.f1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LL9: _tmp58=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp57=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLA:
# 357
 if(_tmp58->typs == 0)
return;
_tmp5D=_tmp58->typs;_tmp5C=_tmp57;goto _LLC;}else{goto _LL13;}}else{goto _LL13;}case 20U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLD: _tmp5B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1;_tmp5A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f2;_tmp59=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLE: {
# 368
struct Cyc_Absyn_Aggrdecl*_tmp50=Cyc_Absyn_get_known_aggrdecl(_tmp5B);
if(_tmp50->impl == 0)return;{
struct Cyc_List_List*_tmp51=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50->impl))->fields;
struct _RegionHandle _tmp52=_new_region("temp");struct _RegionHandle*temp=& _tmp52;_push_region(temp);
{struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp50->tvs,_tmp5A);
{int i=0;for(0;(unsigned)i < _get_fat_size(_tmp59,sizeof(void*));(i ++,_tmp51=_tmp51->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->type;
if(_tmp53 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp53,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp59.curr)[i]);}}
# 378
_npop_handler(0U);return;}
# 372
;_pop_region(temp);};}}else{goto _LL13;}default: goto _LL13;}case 6U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLB: _tmp5D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp5C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLC: {
# 361
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5D);
{int i=0;for(0;(unsigned)i < sz;(i ++,_tmp5D=_tmp5D->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp657=env;unsigned _tmp656=loc;void*_tmp655=(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp5D))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp657,_tmp656,_tmp655,*((void**)_check_fat_subscript(_tmp5C,sizeof(void*),i)));});}}
# 365
return;}}else{goto _LL13;}case 7U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLF: _tmp60=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp5F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f2;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL10:
# 381
{int i=0;for(0;(unsigned)i < _get_fat_size(_tmp5E,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((void**)_tmp5E.curr)[i]);}}
return;}else{goto _LL13;}case 3U: _LL11: _LL12:
# 385
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp54=0U;({unsigned _tmp659=loc;struct _fat_ptr _tmp658=({const char*_tmp55="argument may still contain alias-free pointers";_tag_fat(_tmp55,sizeof(char),47U);});Cyc_Warn_warn(_tmp659,_tmp658,_tag_fat(_tmp54,sizeof(void*),0U));});});
return;default: _LL13: _LL14:
 return;}}_LL0:;}
# 392
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp61=inflow;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6D;if((_tmp61.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp6E=((_tmp61.ReachableFL).val).f1;_tmp6D=((_tmp61.ReachableFL).val).f2;_LL4:
# 396
 if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp62=0U;({struct _fat_ptr _tmp65A=({const char*_tmp63="noliveunique attribute requires alias-free pointer";_tag_fat(_tmp63,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp65A,_tag_fat(_tmp62,sizeof(void*),0U));});});{
void*_tmp64=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp65=r;void*_tmp6A;struct Cyc_CfFlowInfo_Place*_tmp69;void*_tmp68;switch(*((int*)_tmp65)){case 8U: _LL6: _tmp68=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp65)->f2;_LL7:
 r=_tmp68;goto retry;case 4U: _LL8: _tmp69=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp65)->f1;_LL9:
 _tmp6A=Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp69);goto _LLB;case 5U: _LLA: _tmp6A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp65)->f1;_LLB:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp64,_tmp6A);goto _LL5;default: _LLC: _LLD:
# 404
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp64))
({void*_tmp66=0U;({unsigned _tmp65C=loc;struct _fat_ptr _tmp65B=({const char*_tmp67="argument may contain live alias-free pointers";_tag_fat(_tmp67,sizeof(char),46U);});Cyc_Warn_warn(_tmp65C,_tmp65B,_tag_fat(_tmp66,sizeof(void*),0U));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 408
struct Cyc_Dict_Dict _tmp6B=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp6E,r);
if(_tmp6E.t == _tmp6B.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp6C=Cyc_CfFlowInfo_ReachableFL(_tmp6B,_tmp6D);
Cyc_NewControlFlow_update_tryflow(env,_tmp6C);
return _tmp6C;};};};}_LL0:;}struct _tuple27{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 418
static struct _tuple27 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 423
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple27 _tmp5A0;_tmp5A0.f1=inflow,_tmp5A0.f2=0;_tmp5A0;});
do{
struct _tuple21 _tmp6F=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple21 _tmp70=_tmp6F;union Cyc_CfFlowInfo_FlowInfo _tmp73;void*_tmp72;_LL1: _tmp73=_tmp70.f1;_tmp72=_tmp70.f2;_LL2:;
inflow=_tmp73;
rvals=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=_tmp72,_tmp71->tl=rvals;_tmp71;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 434
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple27 _tmp5A1;_tmp5A1.f1=inflow,({struct Cyc_List_List*_tmp65D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp5A1.f2=_tmp65D;});_tmp5A1;});}
# 440
static struct _tuple21 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 442
struct _tuple27 _tmp74=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 442
struct _tuple27 _tmp75=_tmp74;union Cyc_CfFlowInfo_FlowInfo _tmp7B;struct Cyc_List_List*_tmp7A;_LL1: _tmp7B=_tmp75.f1;_tmp7A=_tmp75.f2;_LL2:;
# 444
{union Cyc_CfFlowInfo_FlowInfo _tmp76=_tmp7B;struct Cyc_Dict_Dict _tmp79;if((_tmp76.ReachableFL).tag == 2){_LL4: _tmp79=((_tmp76.ReachableFL).val).f1;_LL5:
# 446
 for(0;_tmp7A != 0;(_tmp7A=_tmp7A->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,_tmp79,(void*)_tmp7A->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp77=0U;({unsigned _tmp65F=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp65E=({const char*_tmp78="expression may not be initialized";_tag_fat(_tmp78,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp65F,_tmp65E,_tag_fat(_tmp77,sizeof(void*),0U));});});}
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 452
return({struct _tuple21 _tmp5A2;_tmp5A2.f1=_tmp7B,_tmp5A2.f2=(env->fenv)->unknown_all;_tmp5A2;});}
# 459
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 466
int needs_unconsume=0;
void*_tmp7C=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp7C,& needs_unconsume))
({void*_tmp7D=0U;({struct _fat_ptr _tmp660=({const char*_tmp7E="consume_zero_rval";_tag_fat(_tmp7E,sizeof(char),18U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp660,_tag_fat(_tmp7D,sizeof(void*),0U));});});else{
# 471
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 475
return new_rval;}}}
# 488 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 493
union Cyc_CfFlowInfo_FlowInfo _tmp7F=outflow;struct Cyc_Dict_Dict _tmp86;struct Cyc_List_List*_tmp85;if((_tmp7F.BottomFL).tag == 1){_LL1: _LL2:
 return outflow;}else{_LL3: _tmp86=((_tmp7F.ReachableFL).val).f1;_tmp85=((_tmp7F.ReachableFL).val).f2;_LL4: {
# 496
union Cyc_CfFlowInfo_AbsLVal _tmp80=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp81=_tmp80;struct Cyc_CfFlowInfo_Place*_tmp84;if((_tmp81.UnknownL).tag == 2){_LL6: _LL7:
# 500
 return outflow;}else{_LL8: _tmp84=(_tmp81.PlaceL).val;_LL9: {
# 504
void*nzval=(int)il == (int)1U?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(0;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=8U,_tmp82->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp82->f2=nzval;_tmp82;});}
# 508
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmp86,_tmp84,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmp83=({struct Cyc_Dict_Dict _tmp661=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp86,_tmp84,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp661,_tmp85);});
# 513
return _tmp83;};}}_LL5:;}}_LL0:;}
# 522
static struct _tuple24 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 524
union Cyc_CfFlowInfo_FlowInfo _tmp87=outflow;struct Cyc_Dict_Dict _tmp8E;struct Cyc_List_List*_tmp8D;if((_tmp87.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple24 _tmp5A3;_tmp5A3.f1=outflow,_tmp5A3.f2=outflow;_tmp5A3;});}else{_LL3: _tmp8E=((_tmp87.ReachableFL).val).f1;_tmp8D=((_tmp87.ReachableFL).val).f2;_LL4: {
# 527
union Cyc_CfFlowInfo_AbsLVal _tmp88=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp89=_tmp88;struct Cyc_CfFlowInfo_Place*_tmp8C;if((_tmp89.UnknownL).tag == 2){_LL6: _LL7:
 return({struct _tuple24 _tmp5A4;_tmp5A4.f1=outflow,_tmp5A4.f2=outflow;_tmp5A4;});}else{_LL8: _tmp8C=(_tmp89.PlaceL).val;_LL9: {
# 530
void*nzval=(int)il == (int)1U?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=8U,_tmp8A->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8A->f2=nzval;_tmp8A;});
zval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=8U,_tmp8B->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8B->f2=zval;_tmp8B;});}
# 536
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmp8E,_tmp8C,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmp8E,_tmp8C,e,zval);
return({struct _tuple24 _tmp5A5;({
union Cyc_CfFlowInfo_FlowInfo _tmp665=({struct Cyc_Dict_Dict _tmp664=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8E,_tmp8C,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp664,_tmp8D);});_tmp5A5.f1=_tmp665;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp663=({struct Cyc_Dict_Dict _tmp662=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8E,_tmp8C,zval);Cyc_CfFlowInfo_ReachableFL(_tmp662,_tmp8D);});_tmp5A5.f2=_tmp663;});_tmp5A5;});}}_LL5:;}}_LL0:;}
# 545
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 548
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
# 551
{void*_tmp8F=e->annot;void*_tmp90=_tmp8F;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp91;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp90)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp91=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp90)->f1;_LL2:
# 553
 if(possibly_null && Cyc_Relations_same_relns(relns,_tmp91))return;
goto _LL0;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp90)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp92=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp90)->f1;_LL4:
# 556
 if(!possibly_null && Cyc_Relations_same_relns(relns,_tmp92))return;
goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 560
if(possibly_null)
({void*_tmp666=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=Cyc_CfFlowInfo_UnknownZ,_tmp93->f1=relns;_tmp93;});e->annot=_tmp666;});else{
# 563
({void*_tmp667=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=Cyc_CfFlowInfo_NotZero,_tmp94->f1=relns;_tmp94;});e->annot=_tmp667;});}}
# 571
static struct _tuple21 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_Absyn_Exp*index){
# 575
void*_tmp95=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp96=_tmp95;void*_tmpB3;void*_tmpB2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96)->tag == 3U){_LL1: _tmpB3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96)->f1).elt_type;_tmpB2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96)->f1).ptr_atts).bounds;_LL2: {
# 577
union Cyc_CfFlowInfo_FlowInfo _tmp97=f;struct Cyc_Dict_Dict _tmpAF;struct Cyc_List_List*_tmpAE;if((_tmp97.BottomFL).tag == 1){_LL6: _LL7:
# 579
 return({struct _tuple21 _tmp5A6;_tmp5A6.f1=f,({void*_tmp668=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB3,0,(env->fenv)->unknown_all);_tmp5A6.f2=_tmp668;});_tmp5A6;});}else{_LL8: _tmpAF=((_tmp97.ReachableFL).val).f1;_tmpAE=((_tmp97.ReachableFL).val).f2;_LL9: {
# 581
struct _tuple20 _tmp98=Cyc_CfFlowInfo_unname_rval(r);struct _tuple20 _tmp99=_tmp98;void*_tmpAD;struct Cyc_List_List*_tmpAC;_LLB: _tmpAD=_tmp99.f1;_tmpAC=_tmp99.f2;_LLC:;
{void*_tmp9A=_tmpAD;enum Cyc_CfFlowInfo_InitLevel _tmpA1;void*_tmpA0;struct Cyc_CfFlowInfo_Place*_tmp9F;struct Cyc_Absyn_Vardecl*_tmp9E;void*_tmp9D;switch(*((int*)_tmp9A)){case 8U: _LLE: _tmp9E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9A)->f1;_tmp9D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9A)->f2;_LLF:
# 584
({void*_tmp9B=0U;({struct _fat_ptr _tmp669=({const char*_tmp9C="named location in anal_derefR";_tag_fat(_tmp9C,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp669,_tag_fat(_tmp9B,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 586
 Cyc_NewControlFlow_update_relns(e,0,_tmpAE);
goto _LLD;case 4U: _LL12: _tmp9F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp9A)->f1;_LL13:
# 589
 Cyc_NewControlFlow_update_relns(e,0,_tmpAE);
if(index == 0 && Cyc_Tcutil_is_bound_one(_tmpB2))
return({struct _tuple21 _tmp5A7;_tmp5A7.f1=f,({void*_tmp66A=Cyc_CfFlowInfo_lookup_place(_tmpAF,_tmp9F);_tmp5A7.f2=_tmp66A;});_tmp5A7;});
goto _LLD;case 5U: _LL14: _tmpA0=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp9A)->f1;_LL15:
# 595
 Cyc_NewControlFlow_update_relns(e,1,_tmpAE);
if(index == 0 && Cyc_Tcutil_is_bound_one(_tmpB2))
return({struct _tuple21 _tmp5A8;_tmp5A8.f1=f,_tmp5A8.f2=_tmpA0;_tmp5A8;});
goto _LLD;case 0U: _LL16: _LL17:
# 600
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple21 _tmp5A9;({union Cyc_CfFlowInfo_FlowInfo _tmp66C=Cyc_CfFlowInfo_BottomFL();_tmp5A9.f1=_tmp66C;}),({void*_tmp66B=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB3,0,(env->fenv)->unknown_all);_tmp5A9.f2=_tmp66B;});_tmp5A9;});case 2U: _LL18: _tmpA1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp9A)->f1;_LL19:
# 604
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpA1,_tmpAC);
goto _LL1B;default: _LL1A: _LL1B:
# 607
 Cyc_NewControlFlow_update_relns(e,1,_tmpAE);
goto _LLD;}_LLD:;}{
# 610
enum Cyc_CfFlowInfo_InitLevel _tmpA2=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpAF,_tmpAD);enum Cyc_CfFlowInfo_InitLevel _tmpA3=_tmpA2;if(_tmpA3 == Cyc_CfFlowInfo_NoneIL){_LL1D: _LL1E: {
# 612
struct _tuple20 _tmpA4=Cyc_CfFlowInfo_unname_rval(_tmpAD);struct _tuple20 _tmpA5=_tmpA4;void*_tmpAB;_LL22: _tmpAB=_tmpA5.f1;_LL23:;
{void*_tmpA6=_tmpAB;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA6)->tag == 7U){_LL25: _LL26:
# 615
({void*_tmpA7=0U;({unsigned _tmp66E=e->loc;struct _fat_ptr _tmp66D=({const char*_tmpA8="attempt to dereference a consumed alias-free pointer";_tag_fat(_tmpA8,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp66E,_tmp66D,_tag_fat(_tmpA7,sizeof(void*),0U));});});
goto _LL24;}else{_LL27: _LL28:
# 618
({void*_tmpA9=0U;({unsigned _tmp670=e->loc;struct _fat_ptr _tmp66F=({const char*_tmpAA="dereference of possibly uninitialized pointer";_tag_fat(_tmpAA,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp670,_tmp66F,_tag_fat(_tmpA9,sizeof(void*),0U));});});}_LL24:;}
# 620
goto _LL20;}}else{_LL1F: _LL20:
# 622
 return({struct _tuple21 _tmp5AA;_tmp5AA.f1=f,({void*_tmp671=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB3,0,(env->fenv)->unknown_all);_tmp5AA.f2=_tmp671;});_tmp5AA;});}_LL1C:;};}}_LL5:;}}else{_LL3: _LL4:
# 625
({void*_tmpB0=0U;({struct _fat_ptr _tmp672=({const char*_tmpB1="right deref of non-pointer-type";_tag_fat(_tmpB1,sizeof(char),32U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp672,_tag_fat(_tmpB0,sizeof(void*),0U));});});}_LL0:;}
# 632
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 634
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 637
{void*_tmpB4=e1->r;void*_tmpB5=_tmpB4;void*_tmpB7;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB5)->tag == 1U){_LL1: _tmpB7=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB5)->f1;_LL2: {
# 639
struct Cyc_Absyn_Vardecl*_tmpB6=Cyc_Tcutil_nonesc_vardecl(_tmpB7);
if(_tmpB6 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpB6);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);}
# 645
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 649
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);}
# 655
return relns;};}
# 663
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned loc,void*old_rval){
# 668
struct _tuple23 _tmpB8=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple23 _tmpB9=_tmpB8;union Cyc_CfFlowInfo_AbsLVal _tmpC8;_LL1: _tmpC8=_tmpB9.f2;_LL2:;
{struct _tuple23 _tmpBA=({struct _tuple23 _tmp5AC;_tmp5AC.f1=inflow,_tmp5AC.f2=_tmpC8;_tmp5AC;});struct _tuple23 _tmpBB=_tmpBA;struct Cyc_Dict_Dict _tmpC7;struct Cyc_List_List*_tmpC6;struct Cyc_CfFlowInfo_Place*_tmpC5;if(((_tmpBB.f1).ReachableFL).tag == 2){if(((_tmpBB.f2).PlaceL).tag == 1){_LL4: _tmpC7=(((_tmpBB.f1).ReachableFL).val).f1;_tmpC6=(((_tmpBB.f1).ReachableFL).val).f2;_tmpC5=((_tmpBB.f2).PlaceL).val;_LL5: {
# 671
void*_tmpBC=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 673
struct _tuple20 _tmpBD=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple20 _tmpBE=_tmpBD;void*_tmpC1;struct Cyc_List_List*_tmpC0;_LLB: _tmpC1=_tmpBE.f1;_tmpC0=_tmpBE.f2;_LLC:;
for(0;_tmpC0 != 0;_tmpC0=_tmpC0->tl){
_tmpBC=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->tag=8U,_tmpBF->f1=(struct Cyc_Absyn_Vardecl*)_tmpC0->hd,_tmpBF->f2=_tmpBC;_tmpBF;});}
# 678
_tmpC7=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpC7,_tmpC5,_tmpBC);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmpC7,_tmpC6);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 684
goto _LL3;}}else{_LL8: _LL9:
# 687
({struct Cyc_String_pa_PrintArg_struct _tmpC4=({struct Cyc_String_pa_PrintArg_struct _tmp5AB;_tmp5AB.tag=0U,({struct _fat_ptr _tmp673=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(exp));_tmp5AB.f1=_tmp673;});_tmp5AB;});void*_tmpC2[1U];_tmpC2[0]=& _tmpC4;({struct _fat_ptr _tmp674=({const char*_tmpC3="noconsume parameter %s must be l-values";_tag_fat(_tmpC3,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp674,_tag_fat(_tmpC2,sizeof(void*),1U));});});
goto _LL3;}}else{_LL6: _LL7:
# 685
 goto _LL3;}_LL3:;}
# 690
return inflow;}
# 695
static struct _tuple21 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 702
union Cyc_CfFlowInfo_FlowInfo _tmpC9=outflow;struct Cyc_Dict_Dict _tmpD4;struct Cyc_List_List*_tmpD3;if((_tmpC9.BottomFL).tag == 1){_LL1: _LL2:
# 704
 return({struct _tuple21 _tmp5AD;({union Cyc_CfFlowInfo_FlowInfo _tmp675=Cyc_CfFlowInfo_BottomFL();_tmp5AD.f1=_tmp675;}),_tmp5AD.f2=rval;_tmp5AD;});}else{_LL3: _tmpD4=((_tmpC9.ReachableFL).val).f1;_tmpD3=((_tmpC9.ReachableFL).val).f2;_LL4: {
# 706
union Cyc_CfFlowInfo_AbsLVal _tmpCA=lval;struct Cyc_CfFlowInfo_Place*_tmpD2;if((_tmpCA.PlaceL).tag == 1){_LL6: _tmpD2=(_tmpCA.PlaceL).val;_LL7: {
# 714
struct Cyc_Dict_Dict _tmpCB=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpD4,_tmpD2,rval);
_tmpD3=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,_tmpD3,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmpCB,_tmpD3);
if(Cyc_NewControlFlow_warn_lose_unique &&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 720
struct _tuple20 _tmpCC=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(_tmpD4,_tmpD2));struct _tuple20 _tmpCD=_tmpCC;void*_tmpD1;_LLB: _tmpD1=_tmpCD.f1;_LLC:;{
void*_tmpCE=_tmpD1;switch(*((int*)_tmpCE)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpCE)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE: _LLF:
 goto _LL11;}else{goto _LL14;}case 7U: _LL10: _LL11:
 goto _LL13;case 0U: _LL12: _LL13:
 goto _LLD;default: _LL14: _LL15:
# 726
({void*_tmpCF=0U;({unsigned _tmp677=lexp->loc;struct _fat_ptr _tmp676=({const char*_tmpD0="assignment may overwrite alias-free pointer(s)";_tag_fat(_tmpD0,sizeof(char),47U);});Cyc_Warn_warn(_tmp677,_tmp676,_tag_fat(_tmpCF,sizeof(void*),0U));});});
goto _LLD;}_LLD:;};}
# 731
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple21 _tmp5AE;_tmp5AE.f1=outflow,_tmp5AE.f2=rval;_tmp5AE;});}}else{_LL8: _LL9:
# 737
 return({struct _tuple21 _tmp5AF;({union Cyc_CfFlowInfo_FlowInfo _tmp678=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp5AF.f1=_tmp678;}),_tmp5AF.f2=rval;_tmp5AF;});}_LL5:;}}_LL0:;}
# 744
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 750
union Cyc_CfFlowInfo_FlowInfo _tmpD5=f;struct Cyc_Dict_Dict _tmpDA;struct Cyc_List_List*_tmpD9;if((_tmpD5.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmpDA=((_tmpD5.ReachableFL).val).f1;_tmpD9=((_tmpD5.ReachableFL).val).f2;_LL4:
# 755
 _tmpDA=({struct Cyc_CfFlowInfo_FlowEnv*_tmp67D=fenv;unsigned _tmp67C=loc;struct Cyc_Dict_Dict _tmp67B=_tmpDA;struct Cyc_CfFlowInfo_Place*_tmp67A=({struct Cyc_CfFlowInfo_Place*_tmpD7=_cycalloc(sizeof(*_tmpD7));
({void*_tmp679=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->tag=0U,_tmpD6->f1=vd;_tmpD6;});_tmpD7->root=_tmp679;}),_tmpD7->path=0;_tmpD7;});
# 755
Cyc_CfFlowInfo_assign_place(_tmp67D,_tmp67C,_tmp67B,_tmp67A,rval);});
# 758
_tmpD9=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,_tmpD9,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmpD8=Cyc_CfFlowInfo_ReachableFL(_tmpDA,_tmpD9);
Cyc_NewControlFlow_update_tryflow(env,_tmpD8);
# 763
return _tmpD8;};}_LL0:;}struct _tuple28{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 767
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned pat_loc,unsigned exp_loc){
# 774
if(vds == 0)return inflow;{
# 777
struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple28*)x->hd)).f1 == 0)es=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple28*)x->hd)).f2),_tmpDC->tl=es;_tmpDC;});}}
# 783
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmpDB,fenv->unknown_all,pat_loc,name_locs);
# 785
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 789
struct _tuple21 _tmpDD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple21 _tmpDE=_tmpDD;union Cyc_CfFlowInfo_FlowInfo _tmpE0;void*_tmpDF;_LL1: _tmpE0=_tmpDE.f1;_tmpDF=_tmpDE.f2;_LL2:;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmpE0,_tmpDF);}}{
# 797
struct Cyc_List_List*_tmpE1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmpE1 != 0;_tmpE1=_tmpE1->tl){
struct _tuple28*_tmpE2=(struct _tuple28*)_tmpE1->hd;struct _tuple28*_tmpE3=_tmpE2;struct Cyc_Absyn_Vardecl**_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;_LL4: _tmpFB=_tmpE3->f1;_tmpFA=_tmpE3->f2;_LL5:;
if(_tmpFB != 0 && _tmpFA != 0){
if(_tmpFA->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpE6=({struct Cyc_String_pa_PrintArg_struct _tmp5B0;_tmp5B0.tag=0U,({
struct _fat_ptr _tmp67E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(_tmpFA));_tmp5B0.f1=_tmp67E;});_tmp5B0;});void*_tmpE4[1U];_tmpE4[0]=& _tmpE6;({struct _fat_ptr _tmp67F=({const char*_tmpE5="oops! pattern init expr %s has no type!\n";_tag_fat(_tmpE5,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp67F,_tag_fat(_tmpE4,sizeof(void*),1U));});});{
# 811
struct Cyc_List_List l=({struct Cyc_List_List _tmp5B2;_tmp5B2.hd=_tmpFA,_tmp5B2.tl=0;_tmp5B2;});
union Cyc_CfFlowInfo_FlowInfo _tmpE7=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple23 _tmpE8=Cyc_NewControlFlow_anal_Lexp(env,_tmpE7,0,0,_tmpFA);struct _tuple23 _tmpE9=_tmpE8;union Cyc_CfFlowInfo_AbsLVal _tmpF9;_LL7: _tmpF9=_tmpE9.f2;_LL8:;{
struct _tuple21 _tmpEA=Cyc_NewControlFlow_anal_Rexp(env,1,_tmpE7,_tmpFA);struct _tuple21 _tmpEB=_tmpEA;union Cyc_CfFlowInfo_FlowInfo _tmpF8;void*_tmpF7;_LLA: _tmpF8=_tmpEB.f1;_tmpF7=_tmpEB.f2;_LLB:;{
union Cyc_CfFlowInfo_FlowInfo _tmpEC=_tmpF8;struct Cyc_Dict_Dict _tmpF6;struct Cyc_List_List*_tmpF5;if((_tmpEC.ReachableFL).tag == 2){_LLD: _tmpF6=((_tmpEC.ReachableFL).val).f1;_tmpF5=((_tmpEC.ReachableFL).val).f2;_LLE:
# 817
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmpED=_tmpF9;struct Cyc_CfFlowInfo_Place*_tmpF4;if((_tmpED.PlaceL).tag == 1){_LL12: _tmpF4=(_tmpED.PlaceL).val;_LL13:
# 820
 _tmpF7=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=8U,_tmpEE->f1=*_tmpFB,_tmpEE->f2=_tmpF7;_tmpEE;});{
# 823
void*_tmpEF=Cyc_CfFlowInfo_lookup_place(_tmpF6,_tmpF4);
_tmpEF=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->tag=8U,_tmpF0->f1=*_tmpFB,_tmpF0->f2=_tmpEF;_tmpF0;});
_tmpF6=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmpF6,_tmpF4,_tmpEF);
_tmpF8=Cyc_CfFlowInfo_ReachableFL(_tmpF6,_tmpF5);
goto _LL11;};}else{_LL14: _LL15:
# 830
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmpFA->topt))&& !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmpFB)->type))
({struct Cyc_String_pa_PrintArg_struct _tmpF3=({struct Cyc_String_pa_PrintArg_struct _tmp5B1;_tmp5B1.tag=0U,({
struct _fat_ptr _tmp680=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(_tmpFA));_tmp5B1.f1=_tmp680;});_tmp5B1;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({unsigned _tmp682=exp_loc;struct _fat_ptr _tmp681=({const char*_tmpF2="aliased pattern expression not an l-value: %s";_tag_fat(_tmpF2,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp682,_tmp681,_tag_fat(_tmpF1,sizeof(void*),1U));});});}_LL11:;}
# 840
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmpF8,*_tmpFB,_tmpFA,_tmpF7,pat_loc);
goto _LLC;}else{_LLF: _LL10:
# 843
 goto _LLC;}_LLC:;};};};}}
# 848
return inflow;};};}
# 851
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 853
void*_tmpFC=e->r;void*_tmpFD=_tmpFC;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;switch(*((int*)_tmpFD)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFD)->f1)){case 4U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: goto _LL11;}case 20U: _LL7: _tmp100=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LL8:
 _tmp101=_tmp100;goto _LLA;case 21U: _LL9: _tmp101=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LLA:
 _tmp102=_tmp101;goto _LLC;case 22U: _LLB: _tmp102=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LLC:
# 860
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp102,cast_ok);case 23U: _LLD: _tmp103=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LLE: {
# 862
void*_tmpFE=Cyc_Tcutil_compress((void*)_check_null(_tmp103->topt));void*_tmpFF=_tmpFE;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFF)->tag == 6U){_LL14: _LL15:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp103,cast_ok);}else{_LL16: _LL17:
 return 0;}_LL13:;}case 14U: _LLF: _tmp104=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_LL10:
# 867
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp104,cast_ok);else{
return 0;}default: _LL11: _LL12:
 return 0;}_LL0:;}
# 873
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp105=*rop;union Cyc_Relations_RelnOp _tmp106=_tmp105;unsigned _tmp10A;unsigned _tmp109;switch((_tmp106.RParamNumelts).tag){case 5U: _LL1: _tmp109=(_tmp106.RParam).val;_LL2: {
# 876
struct Cyc_Absyn_Exp*_tmp107=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp109);
return Cyc_Relations_exp2relnop(_tmp107,rop);}case 6U: _LL3: _tmp10A=(_tmp106.RParamNumelts).val;_LL4: {
# 879
struct Cyc_Absyn_Exp*_tmp108=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp10A);
return({struct Cyc_Absyn_Exp*_tmp683=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp108,0U);Cyc_Relations_exp2relnop(_tmp683,rop);});}default: _LL5: _LL6:
 return 1;}_LL0:;}
# 885
static struct _fat_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp10B=rop;unsigned _tmp10D;unsigned _tmp10C;switch((_tmp10B.RParamNumelts).tag){case 5U: _LL1: _tmp10C=(_tmp10B.RParam).val;_LL2:
# 888
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp10C));case 6U: _LL3: _tmp10D=(_tmp10B.RParamNumelts).val;_LL4:
# 890
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp10D));default: _LL5: _LL6:
 return Cyc_Relations_rop2string(rop);}_LL0:;}
# 895
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned loc){
# 898
union Cyc_CfFlowInfo_FlowInfo _tmp10E=inflow;struct Cyc_Dict_Dict _tmp118;struct Cyc_List_List*_tmp117;if((_tmp10E.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp118=((_tmp10E.ReachableFL).val).f1;_tmp117=((_tmp10E.ReachableFL).val).f2;_LL4:
# 901
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp10F=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp10F->rop1;
union Cyc_Relations_RelnOp rop2=_tmp10F->rop2;
enum Cyc_Relations_Relation _tmp110=Cyc_Relations_flip_relation(_tmp10F->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1)|| !Cyc_NewControlFlow_subst_param(exps,& rop2))||
 Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,_tmp110,rop1,_tmp117))){
struct _fat_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _fat_ptr s2=Cyc_Relations_relation2string(_tmp10F->relation);
struct _fat_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp113=({struct Cyc_String_pa_PrintArg_struct _tmp5B6;_tmp5B6.tag=0U,_tmp5B6.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp5B6;});struct Cyc_String_pa_PrintArg_struct _tmp114=({struct Cyc_String_pa_PrintArg_struct _tmp5B5;_tmp5B5.tag=0U,_tmp5B5.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp5B5;});struct Cyc_String_pa_PrintArg_struct _tmp115=({struct Cyc_String_pa_PrintArg_struct _tmp5B4;_tmp5B4.tag=0U,_tmp5B4.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp5B4;});struct Cyc_String_pa_PrintArg_struct _tmp116=({struct Cyc_String_pa_PrintArg_struct _tmp5B3;_tmp5B3.tag=0U,({
struct _fat_ptr _tmp684=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_relns2string(_tmp117));_tmp5B3.f1=_tmp684;});_tmp5B3;});void*_tmp111[4U];_tmp111[0]=& _tmp113,_tmp111[1]=& _tmp114,_tmp111[2]=& _tmp115,_tmp111[3]=& _tmp116;({unsigned _tmp686=loc;struct _fat_ptr _tmp685=({const char*_tmp112="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_fat(_tmp112,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp686,_tmp685,_tag_fat(_tmp111,sizeof(void*),4U));});});
break;}}
# 916
goto _LL0;}_LL0:;}struct _tuple29{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 921
static struct _tuple21 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 925
struct Cyc_CfFlowInfo_FlowEnv*_tmp119=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 938
{union Cyc_CfFlowInfo_FlowInfo _tmp11A=inflow;struct Cyc_Dict_Dict _tmp11C;struct Cyc_List_List*_tmp11B;if((_tmp11A.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple21 _tmp5B7;({union Cyc_CfFlowInfo_FlowInfo _tmp687=Cyc_CfFlowInfo_BottomFL();_tmp5B7.f1=_tmp687;}),_tmp5B7.f2=_tmp119->unknown_all;_tmp5B7;});}else{_LL3: _tmp11C=((_tmp11A.ReachableFL).val).f1;_tmp11B=((_tmp11A.ReachableFL).val).f2;_LL4:
 d=_tmp11C;relns=_tmp11B;}_LL0:;}
# 953 "new_control_flow.cyc"
if((copy_ctxt && Cyc_NewControlFlow_is_local_var_rooted_path(e,0))&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 974 "new_control_flow.cyc"
struct _tuple23 _tmp11D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple23 _tmp11E=_tmp11D;union Cyc_CfFlowInfo_FlowInfo _tmp12A;union Cyc_CfFlowInfo_AbsLVal _tmp129;_LL6: _tmp12A=_tmp11E.f1;_tmp129=_tmp11E.f2;_LL7:;{
struct _tuple23 _tmp11F=({struct _tuple23 _tmp5BB;_tmp5BB.f1=_tmp12A,_tmp5BB.f2=_tmp129;_tmp5BB;});struct _tuple23 _tmp120=_tmp11F;struct Cyc_Dict_Dict _tmp128;struct Cyc_List_List*_tmp127;struct Cyc_CfFlowInfo_Place*_tmp126;if(((_tmp120.f1).ReachableFL).tag == 2){if(((_tmp120.f2).PlaceL).tag == 1){_LL9: _tmp128=(((_tmp120.f1).ReachableFL).val).f1;_tmp127=(((_tmp120.f1).ReachableFL).val).f2;_tmp126=((_tmp120.f2).PlaceL).val;_LLA: {
# 977
void*_tmp121=Cyc_CfFlowInfo_lookup_place(_tmp128,_tmp126);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp121,& needs_unconsume)){
({void*_tmp122=0U;({unsigned _tmp689=e->loc;struct _fat_ptr _tmp688=({const char*_tmp123="expression attempts to copy a consumed alias-free value";_tag_fat(_tmp123,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp689,_tmp688,_tag_fat(_tmp122,sizeof(void*),0U));});});
return({struct _tuple21 _tmp5B8;({union Cyc_CfFlowInfo_FlowInfo _tmp68A=Cyc_CfFlowInfo_BottomFL();_tmp5B8.f1=_tmp68A;}),_tmp5B8.f2=_tmp119->unknown_all;_tmp5B8;});}else{
# 983
if(needs_unconsume)
# 985
return({struct _tuple21 _tmp5B9;_tmp5B9.f1=_tmp12A,({void*_tmp68B=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp119,_tmp121);_tmp5B9.f2=_tmp68B;});_tmp5B9;});else{
# 988
void*_tmp124=Cyc_CfFlowInfo_make_unique_consumed(_tmp119,(void*)_check_null(e->topt),e,env->iteration_num,_tmp121);
struct Cyc_Dict_Dict _tmp125=Cyc_CfFlowInfo_assign_place(_tmp119,e->loc,_tmp128,_tmp126,_tmp124);
# 1000
return({struct _tuple21 _tmp5BA;({union Cyc_CfFlowInfo_FlowInfo _tmp68C=Cyc_CfFlowInfo_ReachableFL(_tmp125,_tmp127);_tmp5BA.f1=_tmp68C;}),_tmp5BA.f2=_tmp121;_tmp5BA;});}}}}else{goto _LLB;}}else{_LLB: _LLC:
# 1004
 goto _LL8;}_LL8:;};}{
# 1007
void*_tmp12B=e->r;void*_tmp12C=_tmp12B;struct Cyc_Absyn_Stmt*_tmp2EE;struct Cyc_Absyn_Exp*_tmp2ED;void*_tmp2EC;int _tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2E9;struct Cyc_Absyn_Exp*_tmp2E8;int _tmp2E7;struct Cyc_List_List*_tmp2E6;struct Cyc_List_List*_tmp2E5;enum Cyc_Absyn_AggrKind _tmp2E4;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E2;struct Cyc_List_List*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D7;struct _fat_ptr*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;struct _fat_ptr*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D3;struct _fat_ptr*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;int _tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;void**_tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;int _tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_List_List*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;enum Cyc_Absyn_Primop _tmp2BC;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_Datatypedecl*_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2B8;struct Cyc_Absyn_Vardecl*_tmp2B7;struct Cyc_Absyn_Vardecl*_tmp2B6;struct _fat_ptr _tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;switch(*((int*)_tmp12C)){case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12C)->f4 == Cyc_Absyn_Null_to_NonNull){_LLE: _tmp2B0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LLF: {
# 1011
struct _tuple21 _tmp12D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp2B0);struct _tuple21 _tmp12E=_tmp12D;union Cyc_CfFlowInfo_FlowInfo _tmp134;void*_tmp133;_LL7F: _tmp134=_tmp12E.f1;_tmp133=_tmp12E.f2;_LL80:;{
struct _tuple21 _tmp12F=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp134,_tmp2B0,_tmp133,0);struct _tuple21 _tmp130=_tmp12F;union Cyc_CfFlowInfo_FlowInfo _tmp132;void*_tmp131;_LL82: _tmp132=_tmp130.f1;_tmp131=_tmp130.f2;_LL83:;
return({struct _tuple21 _tmp5BC;_tmp5BC.f1=_tmp132,_tmp5BC.f2=_tmp133;_tmp5BC;});};}}else{_LL10: _tmp2B1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL11:
# 1017
 _tmp2B2=_tmp2B1;goto _LL13;}case 12U: _LL12: _tmp2B2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL13:
 _tmp2B3=_tmp2B2;goto _LL15;case 41U: _LL14: _tmp2B3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL15:
 _tmp2B4=_tmp2B3;goto _LL17;case 13U: _LL16: _tmp2B4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL17:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp2B4);case 2U: _LL18: _tmp2B5=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL19:
# 1023
 if(!({struct _fat_ptr _tmp68D=(struct _fat_ptr)_tmp2B5;Cyc_NewControlFlow_strcmp(_tmp68D,({const char*_tmp135="print_flow";_tag_fat(_tmp135,sizeof(char),11U);}));})){
struct _fat_ptr _tmp136=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp139=({struct Cyc_String_pa_PrintArg_struct _tmp5BD;_tmp5BD.tag=0U,_tmp5BD.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp136);_tmp5BD;});void*_tmp137[1U];_tmp137[0]=& _tmp139;({struct Cyc___cycFILE*_tmp68F=Cyc_stderr;struct _fat_ptr _tmp68E=({const char*_tmp138="%s: current flow is\n";_tag_fat(_tmp138,sizeof(char),21U);});Cyc_fprintf(_tmp68F,_tmp68E,_tag_fat(_tmp137,sizeof(void*),1U));});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp13A=0U;({struct Cyc___cycFILE*_tmp691=Cyc_stderr;struct _fat_ptr _tmp690=({const char*_tmp13B="\n";_tag_fat(_tmp13B,sizeof(char),2U);});Cyc_fprintf(_tmp691,_tmp690,_tag_fat(_tmp13A,sizeof(void*),0U));});});}else{
# 1029
if(!({struct _fat_ptr _tmp692=(struct _fat_ptr)_tmp2B5;Cyc_NewControlFlow_strcmp(_tmp692,({const char*_tmp13C="debug_on";_tag_fat(_tmp13C,sizeof(char),9U);}));}));else{
# 1034
if(!({struct _fat_ptr _tmp693=(struct _fat_ptr)_tmp2B5;Cyc_NewControlFlow_strcmp(_tmp693,({const char*_tmp13D="debug_off";_tag_fat(_tmp13D,sizeof(char),10U);}));}));else{
# 1040
({struct Cyc_String_pa_PrintArg_struct _tmp140=({struct Cyc_String_pa_PrintArg_struct _tmp5BE;_tmp5BE.tag=0U,_tmp5BE.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp2B5);_tmp5BE;});void*_tmp13E[1U];_tmp13E[0]=& _tmp140;({unsigned _tmp695=e->loc;struct _fat_ptr _tmp694=({const char*_tmp13F="unknown pragma %s";_tag_fat(_tmp13F,sizeof(char),18U);});Cyc_Warn_warn(_tmp695,_tmp694,_tag_fat(_tmp13E,sizeof(void*),1U));});});}}}
return({struct _tuple21 _tmp5BF;_tmp5BF.f1=inflow,_tmp5BF.f2=_tmp119->zero;_tmp5BF;});case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).Wstring_c).tag){case 1U: _LL1A: _LL1B:
# 1043
 goto _LL1D;case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).Int_c).val).f2 == 0){_LL1C: _LL1D:
 return({struct _tuple21 _tmp5C0;_tmp5C0.f1=inflow,_tmp5C0.f2=_tmp119->zero;_tmp5C0;});}else{_LL1E: _LL1F:
 goto _LL21;}case 8U: _LL20: _LL21:
 goto _LL23;case 9U: _LL22: _LL23:
 goto _LL25;default: _LL28: _LL29:
# 1051
 goto _LL2B;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C)->f1)){case 2U: _LL24: _LL25:
# 1048
 return({struct _tuple21 _tmp5C1;_tmp5C1.f1=inflow,_tmp5C1.f2=_tmp119->notzeroall;_tmp5C1;});case 1U: _LL34: _LL35:
# 1059
 return({struct _tuple21 _tmp5C2;_tmp5C2.f1=inflow,({void*_tmp696=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),0,_tmp119->unknown_all);_tmp5C2.f2=_tmp696;});_tmp5C2;});case 4U: _LL36: _tmp2B6=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C)->f1)->f1;_LL37:
# 1064
 if((int)_tmp2B6->sc == (int)Cyc_Absyn_Static)
return({struct _tuple21 _tmp5C3;_tmp5C3.f1=inflow,({void*_tmp697=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),0,_tmp119->unknown_all);_tmp5C3.f2=_tmp697;});_tmp5C3;});
# 1067
_tmp2B7=_tmp2B6;goto _LL39;case 3U: _LL38: _tmp2B7=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C)->f1)->f1;_LL39:
 _tmp2B8=_tmp2B7;goto _LL3B;case 5U: _LL3A: _tmp2B8=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C)->f1)->f1;_LL3B: {
# 1071
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5C5;_tmp5C5.tag=0U,_tmp5C5.f1=_tmp2B8;_tmp5C5;});
return({struct _tuple21 _tmp5C4;_tmp5C4.f1=inflow,({void*_tmp699=({struct Cyc_Dict_Dict _tmp698=d;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp698,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp141=_cycalloc(sizeof(*_tmp141));*_tmp141=vdroot;_tmp141;}));});_tmp5C4.f2=_tmp699;});_tmp5C4;});}default: _LL74: _LL75:
# 1676
 goto _LL77;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12C)->f1 == 0){_LL26: _LL27:
# 1050
 goto _LL29;}else{_LL62: _tmp2BA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2B9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL63:
# 1538
 _tmp2E1=_tmp2BA;goto _LL65;}case 18U: _LL2A: _LL2B:
# 1052
 goto _LL2D;case 17U: _LL2C: _LL2D:
 goto _LL2F;case 19U: _LL2E: _LL2F:
 goto _LL31;case 33U: _LL30: _LL31:
 goto _LL33;case 32U: _LL32: _LL33:
 return({struct _tuple21 _tmp5C6;_tmp5C6.f1=inflow,_tmp5C6.f2=_tmp119->unknown_all;_tmp5C6;});case 3U: _LL3C: _tmp2BC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2BB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL3D: {
# 1078
struct _tuple21 _tmp142=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp2BB);struct _tuple21 _tmp143=_tmp142;union Cyc_CfFlowInfo_FlowInfo _tmp145;void*_tmp144;_LL85: _tmp145=_tmp143.f1;_tmp144=_tmp143.f2;_LL86:;
return({struct _tuple21 _tmp5C7;_tmp5C7.f1=_tmp145,_tmp5C7.f2=_tmp144;_tmp5C7;});}case 5U: _LL3E: _tmp2BD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL3F: {
# 1082
struct Cyc_List_List _tmp146=({struct Cyc_List_List _tmp5CB;_tmp5CB.hd=_tmp2BD,_tmp5CB.tl=0;_tmp5CB;});
struct _tuple21 _tmp147=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp146);struct _tuple21 _tmp148=_tmp147;union Cyc_CfFlowInfo_FlowInfo _tmp155;_LL88: _tmp155=_tmp148.f1;_LL89:;{
struct _tuple23 _tmp149=Cyc_NewControlFlow_anal_Lexp(env,_tmp155,0,0,_tmp2BD);struct _tuple23 _tmp14A=_tmp149;union Cyc_CfFlowInfo_AbsLVal _tmp154;_LL8B: _tmp154=_tmp14A.f2;_LL8C:;{
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(_tmp2BD->topt));
if(iszt){
# 1089
struct _tuple21 _tmp14B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2BD);struct _tuple21 _tmp14C=_tmp14B;union Cyc_CfFlowInfo_FlowInfo _tmp14E;void*_tmp14D;_LL8E: _tmp14E=_tmp14C.f1;_tmp14D=_tmp14C.f2;_LL8F:;
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp14E,_tmp2BD,_tmp14D,0);}
# 1092
{struct _tuple29 _tmp14F=({struct _tuple29 _tmp5C8;_tmp5C8.f1=_tmp154,_tmp5C8.f2=_tmp155;_tmp5C8;});struct _tuple29 _tmp150=_tmp14F;struct Cyc_CfFlowInfo_Place*_tmp153;struct Cyc_Dict_Dict _tmp152;struct Cyc_List_List*_tmp151;if(((_tmp150.f1).PlaceL).tag == 1){if(((_tmp150.f2).ReachableFL).tag == 2){_LL91: _tmp153=((_tmp150.f1).PlaceL).val;_tmp152=(((_tmp150.f2).ReachableFL).val).f1;_tmp151=(((_tmp150.f2).ReachableFL).val).f2;_LL92:
# 1094
 _tmp151=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp151,_tmp2BD);
_tmp155=({struct Cyc_Dict_Dict _tmp69A=Cyc_CfFlowInfo_assign_place(_tmp119,_tmp2BD->loc,_tmp152,_tmp153,_tmp119->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp69A,_tmp151);});
# 1099
goto _LL90;}else{goto _LL93;}}else{_LL93: _LL94:
 goto _LL90;}_LL90:;}
# 1103
if(iszt)
return({struct _tuple21 _tmp5C9;_tmp5C9.f1=_tmp155,_tmp5C9.f2=_tmp119->notzeroall;_tmp5C9;});else{
return({struct _tuple21 _tmp5CA;_tmp5CA.f1=_tmp155,_tmp5CA.f2=_tmp119->unknown_all;_tmp5CA;});}};};}case 4U: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f2 != 0){_LL40: _tmp2BF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2BE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_LL41:
# 1108
 if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp156=0U;({unsigned _tmp69C=e->loc;struct _fat_ptr _tmp69B=({const char*_tmp157="cannot track alias-free pointers through multiple assignments";_tag_fat(_tmp157,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp69C,_tmp69B,_tag_fat(_tmp156,sizeof(void*),0U));});});
return({struct _tuple21 _tmp5CC;({union Cyc_CfFlowInfo_FlowInfo _tmp69D=Cyc_CfFlowInfo_BottomFL();_tmp5CC.f1=_tmp69D;}),_tmp5CC.f2=_tmp119->unknown_all;_tmp5CC;});}{
# 1112
struct _tuple21 _tmp158=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2BE);struct _tuple21 _tmp159=_tmp158;union Cyc_CfFlowInfo_FlowInfo _tmp164;void*_tmp163;_LL96: _tmp164=_tmp159.f1;_tmp163=_tmp159.f2;_LL97:;{
struct _tuple23 _tmp15A=Cyc_NewControlFlow_anal_Lexp(env,_tmp164,0,0,_tmp2BF);struct _tuple23 _tmp15B=_tmp15A;union Cyc_CfFlowInfo_FlowInfo _tmp162;union Cyc_CfFlowInfo_AbsLVal _tmp161;_LL99: _tmp162=_tmp15B.f1;_tmp161=_tmp15B.f2;_LL9A:;
{union Cyc_CfFlowInfo_FlowInfo _tmp15C=_tmp162;struct Cyc_Dict_Dict _tmp160;struct Cyc_List_List*_tmp15F;if((_tmp15C.ReachableFL).tag == 2){_LL9C: _tmp160=((_tmp15C.ReachableFL).val).f1;_tmp15F=((_tmp15C.ReachableFL).val).f2;_LL9D:
# 1116
{union Cyc_CfFlowInfo_AbsLVal _tmp15D=_tmp161;struct Cyc_CfFlowInfo_Place*_tmp15E;if((_tmp15D.PlaceL).tag == 1){_LLA1: _tmp15E=(_tmp15D.PlaceL).val;_LLA2:
# 1120
 _tmp15F=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp15F,_tmp2BF);
_tmp160=Cyc_CfFlowInfo_assign_place(_tmp119,_tmp2BF->loc,_tmp160,_tmp15E,_tmp119->unknown_all);
# 1123
_tmp162=Cyc_CfFlowInfo_ReachableFL(_tmp160,_tmp15F);
# 1127
goto _LLA0;}else{_LLA3: _LLA4:
# 1130
 goto _LLA0;}_LLA0:;}
# 1132
goto _LL9B;}else{_LL9E: _LL9F:
 goto _LL9B;}_LL9B:;}
# 1135
return({struct _tuple21 _tmp5CD;_tmp5CD.f1=_tmp162,_tmp5CD.f2=_tmp119->unknown_all;_tmp5CD;});};};}else{_LL42: _tmp2C1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2C0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_LL43:
# 1138
 if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp165=0U;({unsigned _tmp69F=e->loc;struct _fat_ptr _tmp69E=({const char*_tmp166="cannot track alias-free pointers through multiple assignments";_tag_fat(_tmp166,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp69F,_tmp69E,_tag_fat(_tmp165,sizeof(void*),0U));});});
return({struct _tuple21 _tmp5CE;({union Cyc_CfFlowInfo_FlowInfo _tmp6A0=Cyc_CfFlowInfo_BottomFL();_tmp5CE.f1=_tmp6A0;}),_tmp5CE.f2=_tmp119->unknown_all;_tmp5CE;});}
# 1144
inflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp6A2=env;union Cyc_CfFlowInfo_FlowInfo _tmp6A1=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp6A2,_tmp6A1,({struct Cyc_Absyn_Exp*_tmp167[2U];_tmp167[0]=_tmp2C1,_tmp167[1]=_tmp2C0;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp167,sizeof(struct Cyc_Absyn_Exp*),2U));}));});{
# 1146
struct _tuple21 _tmp168=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C0);struct _tuple21 _tmp169=_tmp168;union Cyc_CfFlowInfo_FlowInfo _tmp173;void*_tmp172;_LLA6: _tmp173=_tmp169.f1;_tmp172=_tmp169.f2;_LLA7:;{
struct _tuple23 _tmp16A=Cyc_NewControlFlow_anal_Lexp(env,_tmp173,0,0,_tmp2C1);struct _tuple23 _tmp16B=_tmp16A;union Cyc_CfFlowInfo_FlowInfo _tmp171;union Cyc_CfFlowInfo_AbsLVal _tmp170;_LLA9: _tmp171=_tmp16B.f1;_tmp170=_tmp16B.f2;_LLAA:;{
struct _tuple21 _tmp16C=Cyc_NewControlFlow_do_assign(_tmp119,env,_tmp171,_tmp2C1,_tmp170,_tmp2C0,_tmp172,e->loc);struct _tuple21 _tmp16D=_tmp16C;union Cyc_CfFlowInfo_FlowInfo _tmp16F;void*_tmp16E;_LLAC: _tmp16F=_tmp16D.f1;_tmp16E=_tmp16D.f2;_LLAD:;
# 1152
return({struct _tuple21 _tmp5CF;_tmp5CF.f1=_tmp16F,_tmp5CF.f2=_tmp16E;_tmp5CF;});};};};}case 9U: _LL44: _tmp2C3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2C2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL45: {
# 1155
struct _tuple21 _tmp174=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2C3);struct _tuple21 _tmp175=_tmp174;union Cyc_CfFlowInfo_FlowInfo _tmp177;void*_tmp176;_LLAF: _tmp177=_tmp175.f1;_tmp176=_tmp175.f2;_LLB0:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp177,_tmp2C2);}case 11U: _LL46: _tmp2C4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL47: {
# 1159
struct _tuple21 _tmp178=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C4);struct _tuple21 _tmp179=_tmp178;union Cyc_CfFlowInfo_FlowInfo _tmp17B;void*_tmp17A;_LLB2: _tmp17B=_tmp179.f1;_tmp17A=_tmp179.f2;_LLB3:;
Cyc_NewControlFlow_use_Rval(env,_tmp2C4->loc,_tmp17B,_tmp17A);
return({struct _tuple21 _tmp5D0;({union Cyc_CfFlowInfo_FlowInfo _tmp6A4=Cyc_CfFlowInfo_BottomFL();_tmp5D0.f1=_tmp6A4;}),({void*_tmp6A3=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),0,_tmp119->unknown_all);_tmp5D0.f2=_tmp6A3;});_tmp5D0;});}case 10U: _LL48: _tmp2C6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2C5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL49: {
# 1164
struct Cyc_List_List*_tmp17C=_tmp2C5;
struct _tuple21 _tmp17D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2C6);struct _tuple21 _tmp17E=_tmp17D;union Cyc_CfFlowInfo_FlowInfo _tmp1AF;void*_tmp1AE;_LLB5: _tmp1AF=_tmp17E.f1;_tmp1AE=_tmp17E.f2;_LLB6:;
# 1167
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1AF,_tmp2C6,_tmp1AE,0);{
# 1170
struct _tuple27 _tmp17F=({struct Cyc_NewControlFlow_AnalEnv*_tmp6A6=env;union Cyc_CfFlowInfo_FlowInfo _tmp6A5=_tmp1AF;Cyc_NewControlFlow_anal_Rexps(_tmp6A6,_tmp6A5,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp2C5),1,1);});struct _tuple27 _tmp180=_tmp17F;union Cyc_CfFlowInfo_FlowInfo _tmp1AD;struct Cyc_List_List*_tmp1AC;_LLB8: _tmp1AD=_tmp180.f1;_tmp1AC=_tmp180.f2;_LLB9:;{
# 1172
union Cyc_CfFlowInfo_FlowInfo _tmp181=Cyc_NewControlFlow_use_Rval(env,_tmp2C6->loc,_tmp1AD,_tmp1AE);
# 1174
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*consume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp182=Cyc_Tcutil_compress((void*)_check_null(_tmp2C6->topt));void*_tmp183=_tmp182;void*_tmp196;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp183)->tag == 3U){_LLBB: _tmp196=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp183)->f1).elt_type;_LLBC:
# 1181
{void*_tmp184=Cyc_Tcutil_compress(_tmp196);void*_tmp185=_tmp184;struct Cyc_List_List*_tmp193;struct Cyc_List_List*_tmp192;struct Cyc_List_List*_tmp191;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->tag == 5U){_LLC0: _tmp193=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).attributes;_tmp192=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).requires_relns;_tmp191=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).ensures_relns;_LLC1:
# 1183
 requires=_tmp192;
ensures=_tmp191;
for(0;_tmp193 != 0;_tmp193=_tmp193->tl){
# 1187
void*_tmp186=(void*)_tmp193->hd;void*_tmp187=_tmp186;int _tmp18E;int _tmp18D;int _tmp18C;switch(*((int*)_tmp187)){case 20U: _LLC5: _tmp18C=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp187)->f1;_LLC6:
# 1189
 init_params=({struct Cyc_List_List*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->hd=(void*)_tmp18C,_tmp188->tl=init_params;_tmp188;});goto _LLC4;case 21U: _LLC7: _tmp18D=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp187)->f1;_LLC8:
# 1191
 nolive_unique_params=({struct Cyc_List_List*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->hd=(void*)_tmp18D,_tmp189->tl=nolive_unique_params;_tmp189;});
consume_params=({struct Cyc_List_List*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A->hd=(void*)_tmp18D,_tmp18A->tl=consume_params;_tmp18A;});
goto _LLC4;case 22U: _LLC9: _tmp18E=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp187)->f1;_LLCA:
# 1196
 consume_params=({struct Cyc_List_List*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->hd=(void*)_tmp18E,_tmp18B->tl=consume_params;_tmp18B;});
goto _LLC4;default: _LLCB: _LLCC:
 goto _LLC4;}_LLC4:;}
# 1200
goto _LLBF;}else{_LLC2: _LLC3:
({void*_tmp18F=0U;({struct _fat_ptr _tmp6A7=({const char*_tmp190="anal_Rexp: bad function type";_tag_fat(_tmp190,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6A7,_tag_fat(_tmp18F,sizeof(void*),0U));});});}_LLBF:;}
# 1203
goto _LLBA;}else{_LLBD: _LLBE:
({void*_tmp194=0U;({struct _fat_ptr _tmp6A8=({const char*_tmp195="anal_Rexp: bad function type";_tag_fat(_tmp195,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6A8,_tag_fat(_tmp194,sizeof(void*),0U));});});}_LLBA:;}
# 1206
{int i=1;for(0;_tmp1AC != 0;(_tmp1AC=_tmp1AC->tl,_tmp2C5=((struct Cyc_List_List*)_check_null(_tmp2C5))->tl,++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp197=_tmp1AD;struct Cyc_Dict_Dict _tmp1AB;if((_tmp197.BottomFL).tag == 1){_LLCE: _LLCF:
 goto _LLCD;}else{_LLD0: _tmp1AB=((_tmp197.ReachableFL).val).f1;_LLD1: {
# 1211
struct _tuple20 _tmp198=Cyc_CfFlowInfo_unname_rval((void*)_tmp1AC->hd);struct _tuple20 _tmp199=_tmp198;void*_tmp1AA;_LLD3: _tmp1AA=_tmp199.f1;_LLD4:;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1AB,(void*)_tmp1AC->hd)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer((void*)_tmp1AC->hd))
({void*_tmp19A=0U;({unsigned _tmp6AA=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C5))->hd)->loc;struct _fat_ptr _tmp6A9=({const char*_tmp19B="expression may not be initialized";_tag_fat(_tmp19B,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6AA,_tmp6A9,_tag_fat(_tmp19A,sizeof(void*),0U));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp19C=_tmp181;struct Cyc_Dict_Dict _tmp1A9;struct Cyc_List_List*_tmp1A8;if((_tmp19C.BottomFL).tag == 1){_LLD6: _LLD7:
 goto _LLD5;}else{_LLD8: _tmp1A9=((_tmp19C.ReachableFL).val).f1;_tmp1A8=((_tmp19C.ReachableFL).val).f2;_LLD9: {
# 1219
struct Cyc_Dict_Dict _tmp19D=Cyc_CfFlowInfo_escape_deref(_tmp119,_tmp1A9,(void*)_tmp1AC->hd);
{void*_tmp19E=(void*)_tmp1AC->hd;void*_tmp19F=_tmp19E;struct Cyc_CfFlowInfo_Place*_tmp1A7;switch(*((int*)_tmp19F)){case 4U: _LLDB: _tmp1A7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19F)->f1;_LLDC:
# 1222
{void*_tmp1A0=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C5))->hd)->topt));void*_tmp1A1=_tmp1A0;void*_tmp1A4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A1)->tag == 3U){_LLE2: _tmp1A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A1)->f1).elt_type;_LLE3:
# 1224
 _tmp19D=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6AE=_tmp119;unsigned _tmp6AD=((struct Cyc_Absyn_Exp*)_tmp2C5->hd)->loc;struct Cyc_Dict_Dict _tmp6AC=_tmp19D;struct Cyc_CfFlowInfo_Place*_tmp6AB=_tmp1A7;Cyc_CfFlowInfo_assign_place(_tmp6AE,_tmp6AD,_tmp6AC,_tmp6AB,
Cyc_CfFlowInfo_typ_to_absrval(_tmp119,_tmp1A4,0,_tmp119->esc_all));});
# 1228
goto _LLE1;}else{_LLE4: _LLE5:
({void*_tmp1A2=0U;({struct _fat_ptr _tmp6AF=({const char*_tmp1A3="anal_Rexp:bad type for initialized arg";_tag_fat(_tmp1A3,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6AF,_tag_fat(_tmp1A2,sizeof(void*),0U));});});}_LLE1:;}
# 1231
goto _LLDA;case 5U: _LLDD: _LLDE:
# 1233
({void*_tmp1A5=0U;({struct _fat_ptr _tmp6B0=({const char*_tmp1A6="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_fat(_tmp1A6,sizeof(char),68U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6B0,_tag_fat(_tmp1A5,sizeof(void*),0U));});});default: _LLDF: _LLE0:
# 1235
 goto _LLDA;}_LLDA:;}
# 1237
_tmp181=Cyc_CfFlowInfo_ReachableFL(_tmp19D,_tmp1A8);
goto _LLD5;}}_LLD5:;}
# 1240
goto _LLCD;}}_LLCD:;}else{
# 1243
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1248
_tmp181=({struct Cyc_NewControlFlow_AnalEnv*_tmp6B4=env;unsigned _tmp6B3=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C5))->hd)->loc;void*_tmp6B2=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2C5->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp6B1=_tmp181;Cyc_NewControlFlow_use_nounique_Rval(_tmp6B4,_tmp6B3,_tmp6B2,_tmp6B1,(void*)_tmp1AC->hd);});else{
# 1258
_tmp181=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C5))->hd)->loc,_tmp181,(void*)_tmp1AC->hd);
if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(consume_params,i)&&
 Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2C5->hd)->topt),0))
_tmp181=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp181,(struct Cyc_Absyn_Exp*)_tmp2C5->hd,((struct Cyc_Absyn_Exp*)_tmp2C5->hd)->loc,(void*)_tmp1AC->hd);}}}}
# 1265
Cyc_NewControlFlow_check_fn_requires(env,_tmp181,_tmp17C,requires,e->loc);
# 1268
if(Cyc_Tcutil_is_noreturn_fn_type((void*)_check_null(_tmp2C6->topt)))
return({struct _tuple21 _tmp5D1;({union Cyc_CfFlowInfo_FlowInfo _tmp6B6=Cyc_CfFlowInfo_BottomFL();_tmp5D1.f1=_tmp6B6;}),({void*_tmp6B5=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),0,_tmp119->unknown_all);_tmp5D1.f2=_tmp6B5;});_tmp5D1;});else{
# 1271
return({struct _tuple21 _tmp5D2;_tmp5D2.f1=_tmp181,({void*_tmp6B7=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),0,_tmp119->unknown_all);_tmp5D2.f2=_tmp6B7;});_tmp5D2;});}};};}case 34U: _LL4A: _tmp2CB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).is_calloc;_tmp2CA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).rgn;_tmp2C9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).elt_type;_tmp2C8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).num_elts;_tmp2C7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).fat_result;_LL4B: {
# 1274
void*place_val;
if(_tmp2C7)place_val=_tmp119->notzeroall;else{
if(_tmp2CB)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,*((void**)_check_null(_tmp2C9)),0,_tmp119->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,*((void**)_check_null(_tmp2C9)),0,_tmp119->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp2CA != 0){
struct _tuple27 _tmp1B0=({struct Cyc_NewControlFlow_AnalEnv*_tmp6B9=env;union Cyc_CfFlowInfo_FlowInfo _tmp6B8=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6B9,_tmp6B8,({struct Cyc_Absyn_Exp*_tmp1B4[2U];_tmp1B4[0]=_tmp2CA,_tmp1B4[1]=_tmp2C8;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B4,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple27 _tmp1B1=_tmp1B0;union Cyc_CfFlowInfo_FlowInfo _tmp1B3;struct Cyc_List_List*_tmp1B2;_LLE7: _tmp1B3=_tmp1B1.f1;_tmp1B2=_tmp1B1.f2;_LLE8:;
outflow=_tmp1B3;}else{
# 1283
struct _tuple21 _tmp1B5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C8);struct _tuple21 _tmp1B6=_tmp1B5;union Cyc_CfFlowInfo_FlowInfo _tmp1B7;_LLEA: _tmp1B7=_tmp1B6.f1;_LLEB:;
outflow=_tmp1B7;}
# 1288
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple21 _tmp5D3;_tmp5D3.f1=outflow,({void*_tmp6BA=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->tag=5U,_tmp1B8->f1=place_val;_tmp1B8;});_tmp5D3.f2=_tmp6BA;});_tmp5D3;});else{
# 1292
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->tag=1U,_tmp1BE->f1=_tmp2C8,_tmp1BE->f2=(void*)_check_null(e->topt);_tmp1BE;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->root=root,_tmp1BD->path=0;_tmp1BD;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->tag=4U,_tmp1BC->f1=place;_tmp1BC;});
union Cyc_CfFlowInfo_FlowInfo _tmp1B9=outflow;struct Cyc_Dict_Dict _tmp1BB;struct Cyc_List_List*_tmp1BA;if((_tmp1B9.BottomFL).tag == 1){_LLED: _LLEE:
 return({struct _tuple21 _tmp5D4;_tmp5D4.f1=outflow,_tmp5D4.f2=rval;_tmp5D4;});}else{_LLEF: _tmp1BB=((_tmp1B9.ReachableFL).val).f1;_tmp1BA=((_tmp1B9.ReachableFL).val).f2;_LLF0:
# 1298
 return({struct _tuple21 _tmp5D5;({union Cyc_CfFlowInfo_FlowInfo _tmp6BC=({struct Cyc_Dict_Dict _tmp6BB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1BB,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6BB,_tmp1BA);});_tmp5D5.f1=_tmp6BC;}),_tmp5D5.f2=rval;_tmp5D5;});}_LLEC:;}};}case 35U: _LL4C: _tmp2CD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2CC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL4D: {
# 1303
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1309
struct _tuple27 _tmp1BF=({struct Cyc_NewControlFlow_AnalEnv*_tmp6BE=env;union Cyc_CfFlowInfo_FlowInfo _tmp6BD=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6BE,_tmp6BD,({struct Cyc_Absyn_Exp*_tmp1DA[2U];_tmp1DA[0]=_tmp2CD,_tmp1DA[1]=_tmp2CC;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1DA,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,0);});struct _tuple27 _tmp1C0=_tmp1BF;union Cyc_CfFlowInfo_FlowInfo _tmp1D9;struct Cyc_List_List*_tmp1D8;_LLF2: _tmp1D9=_tmp1C0.f1;_tmp1D8=_tmp1C0.f2;_LLF3:;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1D8))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1D8->tl))->hd;
outflow=_tmp1D9;{
# 1315
void*t_ign1=Cyc_Absyn_void_type;void*t_ign2=Cyc_Absyn_void_type;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp2CD,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp1C1=right_rval;if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp1C1)->tag == 0U){_LLF5: _LLF6:
 goto _LLF4;}else{_LLF7: _LLF8:
({void*_tmp1C2=0U;({unsigned _tmp6C0=_tmp2CD->loc;struct _fat_ptr _tmp6BF=({const char*_tmp1C3="cannot swap value into zeroterm array not known to be 0";_tag_fat(_tmp1C3,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6C0,_tmp6BF,_tag_fat(_tmp1C2,sizeof(void*),0U));});});}_LLF4:;}else{
# 1326
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp2CC,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp1C4=left_rval;if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp1C4)->tag == 0U){_LLFA: _LLFB:
 goto _LLF9;}else{_LLFC: _LLFD:
({void*_tmp1C5=0U;({unsigned _tmp6C2=_tmp2CC->loc;struct _fat_ptr _tmp6C1=({const char*_tmp1C6="cannot swap value into zeroterm array not known to be 0";_tag_fat(_tmp1C6,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6C2,_tmp6C1,_tag_fat(_tmp1C5,sizeof(void*),0U));});});}_LLF9:;}}{
# 1334
struct _tuple23 _tmp1C7=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp2CD);struct _tuple23 _tmp1C8=_tmp1C7;union Cyc_CfFlowInfo_AbsLVal _tmp1D7;_LLFF: _tmp1D7=_tmp1C8.f2;_LL100:;{
struct _tuple23 _tmp1C9=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp2CC);struct _tuple23 _tmp1CA=_tmp1C9;union Cyc_CfFlowInfo_AbsLVal _tmp1D6;_LL102: _tmp1D6=_tmp1CA.f2;_LL103:;
{union Cyc_CfFlowInfo_FlowInfo _tmp1CB=outflow;struct Cyc_Dict_Dict _tmp1D5;struct Cyc_List_List*_tmp1D4;if((_tmp1CB.ReachableFL).tag == 2){_LL105: _tmp1D5=((_tmp1CB.ReachableFL).val).f1;_tmp1D4=((_tmp1CB.ReachableFL).val).f2;_LL106:
# 1338
{union Cyc_CfFlowInfo_AbsLVal _tmp1CC=_tmp1D7;struct Cyc_CfFlowInfo_Place*_tmp1CF;if((_tmp1CC.PlaceL).tag == 1){_LL10A: _tmp1CF=(_tmp1CC.PlaceL).val;_LL10B:
# 1340
 _tmp1D5=Cyc_CfFlowInfo_assign_place(_tmp119,_tmp2CD->loc,_tmp1D5,_tmp1CF,right_rval);
goto _LL109;}else{_LL10C: _LL10D:
# 1346
 if((int)Cyc_CfFlowInfo_initlevel(_tmp119,_tmp1D5,right_rval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp1CD=0U;({unsigned _tmp6C4=_tmp2CC->loc;struct _fat_ptr _tmp6C3=({const char*_tmp1CE="expression may not be fully initialized";_tag_fat(_tmp1CE,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6C4,_tmp6C3,_tag_fat(_tmp1CD,sizeof(void*),0U));});});
goto _LL109;}_LL109:;}
# 1350
{union Cyc_CfFlowInfo_AbsLVal _tmp1D0=_tmp1D6;struct Cyc_CfFlowInfo_Place*_tmp1D3;if((_tmp1D0.PlaceL).tag == 1){_LL10F: _tmp1D3=(_tmp1D0.PlaceL).val;_LL110:
# 1352
 _tmp1D5=Cyc_CfFlowInfo_assign_place(_tmp119,_tmp2CC->loc,_tmp1D5,_tmp1D3,left_rval);
goto _LL10E;}else{_LL111: _LL112:
# 1355
 if((int)Cyc_CfFlowInfo_initlevel(_tmp119,_tmp1D5,left_rval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp1D1=0U;({unsigned _tmp6C6=_tmp2CD->loc;struct _fat_ptr _tmp6C5=({const char*_tmp1D2="expression may not be fully initialized";_tag_fat(_tmp1D2,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6C6,_tmp6C5,_tag_fat(_tmp1D1,sizeof(void*),0U));});});
goto _LL10E;}_LL10E:;}
# 1360
_tmp1D4=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1D4,_tmp2CD);
_tmp1D4=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1D4,_tmp2CC);
# 1363
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp1D5,_tmp1D4);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL104;}else{_LL107: _LL108:
 goto _LL104;}_LL104:;}
# 1370
return({struct _tuple21 _tmp5D6;_tmp5D6.f1=outflow,_tmp5D6.f2=_tmp119->unknown_all;_tmp5D6;});};};};}case 16U: _LL4E: _tmp2CF=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2CE=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL4F: {
# 1373
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp2CF != 0){
struct _tuple27 _tmp1DB=({struct Cyc_NewControlFlow_AnalEnv*_tmp6C8=env;union Cyc_CfFlowInfo_FlowInfo _tmp6C7=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6C8,_tmp6C7,({struct Cyc_Absyn_Exp*_tmp1DF[2U];_tmp1DF[0]=_tmp2CF,_tmp1DF[1]=_tmp2CE;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1DF,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple27 _tmp1DC=_tmp1DB;union Cyc_CfFlowInfo_FlowInfo _tmp1DE;struct Cyc_List_List*_tmp1DD;_LL114: _tmp1DE=_tmp1DC.f1;_tmp1DD=_tmp1DC.f2;_LL115:;
outflow=_tmp1DE;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp1DD))->tl))->hd;}else{
# 1380
struct _tuple21 _tmp1E0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2CE);struct _tuple21 _tmp1E1=_tmp1E0;union Cyc_CfFlowInfo_FlowInfo _tmp1E3;void*_tmp1E2;_LL117: _tmp1E3=_tmp1E1.f1;_tmp1E2=_tmp1E1.f2;_LL118:;
outflow=_tmp1E3;
place_val=_tmp1E2;}
# 1385
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple21 _tmp5D7;_tmp5D7.f1=outflow,({void*_tmp6C9=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4->tag=5U,_tmp1E4->f1=place_val;_tmp1E4;});_tmp5D7.f2=_tmp6C9;});_tmp5D7;});else{
# 1389
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->tag=1U,_tmp1EA->f1=_tmp2CE,_tmp1EA->f2=(void*)_check_null(e->topt);_tmp1EA;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->root=root,_tmp1E9->path=0;_tmp1E9;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->tag=4U,_tmp1E8->f1=place;_tmp1E8;});
union Cyc_CfFlowInfo_FlowInfo _tmp1E5=outflow;struct Cyc_Dict_Dict _tmp1E7;struct Cyc_List_List*_tmp1E6;if((_tmp1E5.BottomFL).tag == 1){_LL11A: _LL11B:
 return({struct _tuple21 _tmp5D8;_tmp5D8.f1=outflow,_tmp5D8.f2=rval;_tmp5D8;});}else{_LL11C: _tmp1E7=((_tmp1E5.ReachableFL).val).f1;_tmp1E6=((_tmp1E5.ReachableFL).val).f2;_LL11D:
# 1395
 return({struct _tuple21 _tmp5D9;({union Cyc_CfFlowInfo_FlowInfo _tmp6CB=({struct Cyc_Dict_Dict _tmp6CA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1E7,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6CA,_tmp1E6);});_tmp5D9.f1=_tmp6CB;}),_tmp5D9.f2=rval;_tmp5D9;});}_LL119:;}}case 15U: _LL50: _tmp2D0=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL51: {
# 1400
struct _tuple23 _tmp1EB=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp2D0);struct _tuple23 _tmp1EC=_tmp1EB;union Cyc_CfFlowInfo_FlowInfo _tmp1F1;union Cyc_CfFlowInfo_AbsLVal _tmp1F0;_LL11F: _tmp1F1=_tmp1EC.f1;_tmp1F0=_tmp1EC.f2;_LL120:;{
union Cyc_CfFlowInfo_AbsLVal _tmp1ED=_tmp1F0;struct Cyc_CfFlowInfo_Place*_tmp1EF;if((_tmp1ED.UnknownL).tag == 2){_LL122: _LL123:
 return({struct _tuple21 _tmp5DA;_tmp5DA.f1=_tmp1F1,_tmp5DA.f2=_tmp119->notzeroall;_tmp5DA;});}else{_LL124: _tmp1EF=(_tmp1ED.PlaceL).val;_LL125:
 return({struct _tuple21 _tmp5DB;_tmp5DB.f1=_tmp1F1,({void*_tmp6CC=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->tag=4U,_tmp1EE->f1=_tmp1EF;_tmp1EE;});_tmp5DB.f2=_tmp6CC;});_tmp5DB;});}_LL121:;};}case 20U: _LL52: _tmp2D1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL53: {
# 1407
struct _tuple21 _tmp1F2=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D1);struct _tuple21 _tmp1F3=_tmp1F2;union Cyc_CfFlowInfo_FlowInfo _tmp1F5;void*_tmp1F4;_LL127: _tmp1F5=_tmp1F3.f1;_tmp1F4=_tmp1F3.f2;_LL128:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1F5,_tmp2D1,_tmp1F4,0);}case 21U: _LL54: _tmp2D3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2D2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL55: {
# 1411
struct _tuple21 _tmp1F6=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D3);struct _tuple21 _tmp1F7=_tmp1F6;union Cyc_CfFlowInfo_FlowInfo _tmp207;void*_tmp206;_LL12A: _tmp207=_tmp1F7.f1;_tmp206=_tmp1F7.f2;_LL12B:;{
void*_tmp1F8=(void*)_check_null(_tmp2D3->topt);
# 1414
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp1F8))
return({struct _tuple21 _tmp5DC;_tmp5DC.f1=_tmp207,({void*_tmp6CD=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),0,_tmp119->unknown_all);_tmp5DC.f2=_tmp6CD;});_tmp5DC;});
if(Cyc_Absyn_is_require_union_type(_tmp1F8))
Cyc_NewControlFlow_check_union_requires(_tmp2D3->loc,_tmp1F8,_tmp2D2,_tmp207);{
struct _tuple20 _tmp1F9=Cyc_CfFlowInfo_unname_rval(_tmp206);struct _tuple20 _tmp1FA=_tmp1F9;void*_tmp205;_LL12D: _tmp205=_tmp1FA.f1;_LL12E:;{
void*_tmp1FB=_tmp205;int _tmp204;int _tmp203;struct _fat_ptr _tmp202;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB)->tag == 6U){_LL130: _tmp204=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB)->f1).is_union;_tmp203=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB)->f1).fieldnum;_tmp202=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FB)->f2;_LL131: {
# 1421
int _tmp1FC=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp2D3->topt),_tmp2D2);
if((_tmp204 && _tmp203 != -1)&& _tmp203 != _tmp1FC)
return({struct _tuple21 _tmp5DD;_tmp5DD.f1=_tmp207,({void*_tmp6CE=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),1,_tmp119->unknown_none);_tmp5DD.f2=_tmp6CE;});_tmp5DD;});
return({struct _tuple21 _tmp5DE;_tmp5DE.f1=_tmp207,_tmp5DE.f2=*((void**)_check_fat_subscript(_tmp202,sizeof(void*),_tmp1FC));_tmp5DE;});}}else{_LL132: _LL133:
# 1426
({void*_tmp1FD=0U;({struct Cyc___cycFILE*_tmp6D0=Cyc_stderr;struct _fat_ptr _tmp6CF=({const char*_tmp1FE="the bad rexp is :";_tag_fat(_tmp1FE,sizeof(char),18U);});Cyc_fprintf(_tmp6D0,_tmp6CF,_tag_fat(_tmp1FD,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(_tmp205);
({struct Cyc_String_pa_PrintArg_struct _tmp201=({struct Cyc_String_pa_PrintArg_struct _tmp5DF;_tmp5DF.tag=0U,({struct _fat_ptr _tmp6D1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5DF.f1=_tmp6D1;});_tmp5DF;});void*_tmp1FF[1U];_tmp1FF[0]=& _tmp201;({struct _fat_ptr _tmp6D2=({const char*_tmp200="\nanal_Rexp: AggrMember: %s";_tag_fat(_tmp200,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6D2,_tag_fat(_tmp1FF,sizeof(void*),1U));});});}_LL12F:;};};};}case 38U: _LL56: _tmp2D5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2D4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL57: {
# 1434
struct _tuple21 _tmp208=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D5);struct _tuple21 _tmp209=_tmp208;union Cyc_CfFlowInfo_FlowInfo _tmp216;void*_tmp215;_LL135: _tmp216=_tmp209.f1;_tmp215=_tmp209.f2;_LL136:;
# 1436
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp2D5->topt)))
return({struct _tuple21 _tmp5E0;_tmp5E0.f1=_tmp216,_tmp5E0.f2=_tmp119->unknown_all;_tmp5E0;});{
struct _tuple20 _tmp20A=Cyc_CfFlowInfo_unname_rval(_tmp215);struct _tuple20 _tmp20B=_tmp20A;void*_tmp214;_LL138: _tmp214=_tmp20B.f1;_LL139:;{
void*_tmp20C=_tmp214;int _tmp213;int _tmp212;struct _fat_ptr _tmp211;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20C)->tag == 6U){_LL13B: _tmp213=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20C)->f1).is_union;_tmp212=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20C)->f1).fieldnum;_tmp211=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20C)->f2;_LL13C: {
# 1441
int _tmp20D=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp2D5->topt),_tmp2D4);
if(_tmp213 && _tmp212 != -1){
if(_tmp212 != _tmp20D)
return({struct _tuple21 _tmp5E1;_tmp5E1.f1=_tmp216,_tmp5E1.f2=_tmp119->zero;_tmp5E1;});else{
# 1446
return({struct _tuple21 _tmp5E2;_tmp5E2.f1=_tmp216,_tmp5E2.f2=_tmp119->notzeroall;_tmp5E2;});}}else{
# 1448
return({struct _tuple21 _tmp5E3;_tmp5E3.f1=_tmp216,_tmp5E3.f2=_tmp119->unknown_all;_tmp5E3;});}}}else{_LL13D: _LL13E:
# 1450
({struct Cyc_String_pa_PrintArg_struct _tmp210=({struct Cyc_String_pa_PrintArg_struct _tmp5E4;_tmp5E4.tag=0U,({struct _fat_ptr _tmp6D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5E4.f1=_tmp6D3;});_tmp5E4;});void*_tmp20E[1U];_tmp20E[0]=& _tmp210;({struct _fat_ptr _tmp6D4=({const char*_tmp20F="anal_Rexp: TagCheck_e: %s";_tag_fat(_tmp20F,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6D4,_tag_fat(_tmp20E,sizeof(void*),1U));});});}_LL13A:;};};}case 22U: _LL58: _tmp2D7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2D6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL59: {
# 1454
struct _tuple21 _tmp217=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D7);struct _tuple21 _tmp218=_tmp217;union Cyc_CfFlowInfo_FlowInfo _tmp22D;void*_tmp22C;_LL140: _tmp22D=_tmp218.f1;_tmp22C=_tmp218.f2;_LL141:;{
# 1457
struct _tuple21 _tmp219=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp22D,_tmp2D7,_tmp22C,0);struct _tuple21 _tmp21A=_tmp219;union Cyc_CfFlowInfo_FlowInfo _tmp22B;void*_tmp22A;_LL143: _tmp22B=_tmp21A.f1;_tmp22A=_tmp21A.f2;_LL144:;{
# 1460
void*_tmp21B=Cyc_Tcutil_compress((void*)_check_null(_tmp2D7->topt));void*_tmp21C=_tmp21B;void*_tmp229;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->tag == 3U){_LL146: _tmp229=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->f1).elt_type;_LL147:
# 1462
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp229))
# 1464
return({struct _tuple21 _tmp5E5;_tmp5E5.f1=_tmp22B,({void*_tmp6D5=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),0,_tmp119->unknown_all);_tmp5E5.f2=_tmp6D5;});_tmp5E5;});
# 1466
if(Cyc_Absyn_is_require_union_type(_tmp229))
Cyc_NewControlFlow_check_union_requires(_tmp2D7->loc,_tmp229,_tmp2D6,_tmp22D);{
# 1469
struct _tuple20 _tmp21D=Cyc_CfFlowInfo_unname_rval(_tmp22A);struct _tuple20 _tmp21E=_tmp21D;void*_tmp226;_LL14B: _tmp226=_tmp21E.f1;_LL14C:;{
void*_tmp21F=_tmp226;int _tmp225;int _tmp224;struct _fat_ptr _tmp223;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp21F)->tag == 6U){_LL14E: _tmp225=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp21F)->f1).is_union;_tmp224=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp21F)->f1).fieldnum;_tmp223=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp21F)->f2;_LL14F: {
# 1472
int _tmp220=Cyc_CfFlowInfo_get_field_index(_tmp229,_tmp2D6);
if((_tmp225 && _tmp224 != -1)&& _tmp224 != _tmp220)
return({struct _tuple21 _tmp5E6;_tmp5E6.f1=_tmp22B,({void*_tmp6D6=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),1,_tmp119->unknown_none);_tmp5E6.f2=_tmp6D6;});_tmp5E6;});
return({struct _tuple21 _tmp5E7;_tmp5E7.f1=_tmp22B,_tmp5E7.f2=*((void**)_check_fat_subscript(_tmp223,sizeof(void*),_tmp220));_tmp5E7;});}}else{_LL150: _LL151:
({void*_tmp221=0U;({struct _fat_ptr _tmp6D7=({const char*_tmp222="anal_Rexp: AggrArrow";_tag_fat(_tmp222,sizeof(char),21U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6D7,_tag_fat(_tmp221,sizeof(void*),0U));});});}_LL14D:;};};}else{_LL148: _LL149:
# 1478
({void*_tmp227=0U;({struct _fat_ptr _tmp6D8=({const char*_tmp228="anal_Rexp: AggrArrow ptr";_tag_fat(_tmp228,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6D8,_tag_fat(_tmp227,sizeof(void*),0U));});});}_LL145:;};};}case 6U: _LL5A: _tmp2DA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2D9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_tmp2D8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_LL5B: {
# 1482
struct _tuple24 _tmp22E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2DA);struct _tuple24 _tmp22F=_tmp22E;union Cyc_CfFlowInfo_FlowInfo _tmp233;union Cyc_CfFlowInfo_FlowInfo _tmp232;_LL153: _tmp233=_tmp22F.f1;_tmp232=_tmp22F.f2;_LL154:;{
struct _tuple21 _tmp230=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp233,_tmp2D9);
struct _tuple21 _tmp231=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp232,_tmp2D8);
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp119,_tmp230,_tmp231);};}case 7U: _LL5C: _tmp2DC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2DB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL5D: {
# 1488
struct _tuple24 _tmp234=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2DC);struct _tuple24 _tmp235=_tmp234;union Cyc_CfFlowInfo_FlowInfo _tmp23D;union Cyc_CfFlowInfo_FlowInfo _tmp23C;_LL156: _tmp23D=_tmp235.f1;_tmp23C=_tmp235.f2;_LL157:;{
struct _tuple21 _tmp236=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp23D,_tmp2DB);struct _tuple21 _tmp237=_tmp236;union Cyc_CfFlowInfo_FlowInfo _tmp23B;void*_tmp23A;_LL159: _tmp23B=_tmp237.f1;_tmp23A=_tmp237.f2;_LL15A:;{
struct _tuple21 _tmp238=({struct _tuple21 _tmp5E9;_tmp5E9.f1=_tmp23B,_tmp5E9.f2=_tmp23A;_tmp5E9;});
struct _tuple21 _tmp239=({struct _tuple21 _tmp5E8;_tmp5E8.f1=_tmp23C,_tmp5E8.f2=_tmp119->zero;_tmp5E8;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp119,_tmp238,_tmp239);};};}case 8U: _LL5E: _tmp2DE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2DD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL5F: {
# 1495
struct _tuple24 _tmp23E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2DE);struct _tuple24 _tmp23F=_tmp23E;union Cyc_CfFlowInfo_FlowInfo _tmp247;union Cyc_CfFlowInfo_FlowInfo _tmp246;_LL15C: _tmp247=_tmp23F.f1;_tmp246=_tmp23F.f2;_LL15D:;{
struct _tuple21 _tmp240=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp246,_tmp2DD);struct _tuple21 _tmp241=_tmp240;union Cyc_CfFlowInfo_FlowInfo _tmp245;void*_tmp244;_LL15F: _tmp245=_tmp241.f1;_tmp244=_tmp241.f2;_LL160:;{
struct _tuple21 _tmp242=({struct _tuple21 _tmp5EB;_tmp5EB.f1=_tmp245,_tmp5EB.f2=_tmp244;_tmp5EB;});
struct _tuple21 _tmp243=({struct _tuple21 _tmp5EA;_tmp5EA.f1=_tmp247,_tmp5EA.f2=_tmp119->notzeroall;_tmp5EA;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp119,_tmp242,_tmp243);};};}case 23U: _LL60: _tmp2E0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2DF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL61: {
# 1502
struct _tuple27 _tmp248=({struct Cyc_NewControlFlow_AnalEnv*_tmp6DA=env;union Cyc_CfFlowInfo_FlowInfo _tmp6D9=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6DA,_tmp6D9,({struct Cyc_Absyn_Exp*_tmp264[2U];_tmp264[0]=_tmp2E0,_tmp264[1]=_tmp2DF;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp264,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple27 _tmp249=_tmp248;union Cyc_CfFlowInfo_FlowInfo _tmp263;struct Cyc_List_List*_tmp262;_LL162: _tmp263=_tmp249.f1;_tmp262=_tmp249.f2;_LL163:;{
# 1506
union Cyc_CfFlowInfo_FlowInfo _tmp24A=_tmp263;
{union Cyc_CfFlowInfo_FlowInfo _tmp24B=_tmp263;struct Cyc_Dict_Dict _tmp250;struct Cyc_List_List*_tmp24F;if((_tmp24B.ReachableFL).tag == 2){_LL165: _tmp250=((_tmp24B.ReachableFL).val).f1;_tmp24F=((_tmp24B.ReachableFL).val).f2;_LL166:
# 1511
 if((int)Cyc_CfFlowInfo_initlevel(env->fenv,_tmp250,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp262))->tl))->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp24C=0U;({unsigned _tmp6DC=_tmp2DF->loc;struct _fat_ptr _tmp6DB=({const char*_tmp24D="expression may not be initialized";_tag_fat(_tmp24D,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6DC,_tmp6DB,_tag_fat(_tmp24C,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp24E=Cyc_NewControlFlow_add_subscript_reln(_tmp24F,_tmp2E0,_tmp2DF);
if(_tmp24F != _tmp24E)
_tmp24A=Cyc_CfFlowInfo_ReachableFL(_tmp250,_tmp24E);
goto _LL164;};}else{_LL167: _LL168:
 goto _LL164;}_LL164:;}{
# 1519
void*_tmp251=Cyc_Tcutil_compress((void*)_check_null(_tmp2E0->topt));void*_tmp252=_tmp251;struct Cyc_List_List*_tmp261;switch(*((int*)_tmp252)){case 6U: _LL16A: _tmp261=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp252)->f1;_LL16B: {
# 1521
struct _tuple20 _tmp253=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(_tmp262))->hd);struct _tuple20 _tmp254=_tmp253;void*_tmp259;_LL171: _tmp259=_tmp254.f1;_LL172:;{
void*_tmp255=_tmp259;struct _fat_ptr _tmp258;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp255)->tag == 6U){_LL174: _tmp258=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp255)->f2;_LL175: {
# 1524
unsigned i=(Cyc_Evexp_eval_const_uint_exp(_tmp2DF)).f1;
return({struct _tuple21 _tmp5EC;_tmp5EC.f1=_tmp24A,_tmp5EC.f2=*((void**)_check_fat_subscript(_tmp258,sizeof(void*),(int)i));_tmp5EC;});}}else{_LL176: _LL177:
({void*_tmp256=0U;({struct _fat_ptr _tmp6DD=({const char*_tmp257="anal_Rexp: Subscript";_tag_fat(_tmp257,sizeof(char),21U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6DD,_tag_fat(_tmp256,sizeof(void*),0U));});});}_LL173:;};}case 3U: _LL16C: _LL16D: {
# 1529
struct _tuple21 _tmp25A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp263,_tmp2E0,(void*)((struct Cyc_List_List*)_check_null(_tmp262))->hd,_tmp2DF);struct _tuple21 _tmp25B=_tmp25A;union Cyc_CfFlowInfo_FlowInfo _tmp25E;void*_tmp25D;_LL179: _tmp25E=_tmp25B.f1;_tmp25D=_tmp25B.f2;_LL17A:;{
union Cyc_CfFlowInfo_FlowInfo _tmp25C=_tmp25E;if((_tmp25C.BottomFL).tag == 1){_LL17C: _LL17D:
 return({struct _tuple21 _tmp5ED;_tmp5ED.f1=_tmp25E,_tmp5ED.f2=_tmp25D;_tmp5ED;});}else{_LL17E: _LL17F:
 return({struct _tuple21 _tmp5EE;_tmp5EE.f1=_tmp24A,_tmp5EE.f2=_tmp25D;_tmp5EE;});}_LL17B:;};}default: _LL16E: _LL16F:
# 1534
({void*_tmp25F=0U;({struct _fat_ptr _tmp6DE=({const char*_tmp260="anal_Rexp: Subscript -- bad type";_tag_fat(_tmp260,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6DE,_tag_fat(_tmp25F,sizeof(void*),0U));});});}_LL169:;};};}case 24U: _LL64: _tmp2E1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL65: {
# 1540
struct _tuple27 _tmp265=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp2E1,1,1);struct _tuple27 _tmp266=_tmp265;union Cyc_CfFlowInfo_FlowInfo _tmp26C;struct Cyc_List_List*_tmp26B;_LL181: _tmp26C=_tmp266.f1;_tmp26B=_tmp266.f2;_LL182:;{
struct _fat_ptr aggrdict=({unsigned _tmp26A=(unsigned)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E1);void**_tmp269=_cycalloc(_check_times(_tmp26A,sizeof(void*)));({{unsigned _tmp5F0=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E1);unsigned i;for(i=0;i < _tmp5F0;++ i){({
void*_tmp6DF=({void*_tmp268=(void*)((struct Cyc_List_List*)_check_null(_tmp26B))->hd;
_tmp26B=_tmp26B->tl;
_tmp268;});
# 1543
_tmp269[i]=_tmp6DF;});}}0;});_tag_fat(_tmp269,sizeof(void*),_tmp26A);});
# 1547
return({struct _tuple21 _tmp5EF;_tmp5EF.f1=_tmp26C,({void*_tmp6E0=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->tag=6U,(_tmp267->f1).is_union=0,(_tmp267->f1).fieldnum=- 1,_tmp267->f2=aggrdict;_tmp267;});_tmp5EF.f2=_tmp6E0;});_tmp5EF;});};}case 30U: _LL66: _tmp2E2=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL67: {
# 1550
struct Cyc_List_List*fs;
{void*_tmp26D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp26E=_tmp26D;struct Cyc_List_List*_tmp271;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp26E)->tag == 7U){_LL184: _tmp271=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp26E)->f2;_LL185:
 fs=_tmp271;goto _LL183;}else{_LL186: _LL187:
({void*_tmp26F=0U;({struct _fat_ptr _tmp6E1=({const char*_tmp270="anal_Rexp:anon struct has bad type";_tag_fat(_tmp270,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6E1,_tag_fat(_tmp26F,sizeof(void*),0U));});});}_LL183:;}
# 1555
_tmp2E5=_tmp2E2;_tmp2E4=0U;_tmp2E3=fs;goto _LL69;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f4)->impl != 0){_LL68: _tmp2E5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_tmp2E4=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f4)->kind;_tmp2E3=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f4)->impl)->fields;_LL69: {
# 1557
void*exp_type=(void*)_check_null(e->topt);
struct _tuple27 _tmp272=({struct Cyc_NewControlFlow_AnalEnv*_tmp6E3=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E2=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6E3,_tmp6E2,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple30*))Cyc_Core_snd,_tmp2E5),1,1);});struct _tuple27 _tmp273=_tmp272;union Cyc_CfFlowInfo_FlowInfo _tmp27D;struct Cyc_List_List*_tmp27C;_LL189: _tmp27D=_tmp273.f1;_tmp27C=_tmp273.f2;_LL18A:;{
struct _fat_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp119,_tmp2E3,(int)_tmp2E4 == (int)Cyc_Absyn_UnionA,_tmp119->unknown_all);
# 1562
int field_no=-1;
{int i=0;for(0;_tmp27C != 0;(_tmp27C=_tmp27C->tl,_tmp2E5=_tmp2E5->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp2E5))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp274=(void*)ds->hd;void*_tmp275=_tmp274;struct _fat_ptr*_tmp279;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp275)->tag == 0U){_LL18C: _LL18D:
({void*_tmp276=0U;({struct _fat_ptr _tmp6E4=({const char*_tmp277="anal_Rexp:Aggregate_e";_tag_fat(_tmp277,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6E4,_tag_fat(_tmp276,sizeof(void*),0U));});});}else{_LL18E: _tmp279=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp275)->f1;_LL18F:
# 1569
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp2E3,_tmp279);
*((void**)_check_fat_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp27C->hd;
# 1572
if((int)_tmp2E4 == (int)1U){
struct Cyc_Absyn_Exp*_tmp278=(*((struct _tuple30*)_tmp2E5->hd)).f2;
_tmp27D=Cyc_NewControlFlow_use_Rval(env,_tmp278->loc,_tmp27D,(void*)_tmp27C->hd);
# 1576
Cyc_NewControlFlow_check_union_requires(_tmp278->loc,exp_type,_tmp279,_tmp27D);}}_LL18B:;}}}{
# 1579
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp27A=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->tag=6U,(_tmp27B->f1).is_union=(int)_tmp2E4 == (int)Cyc_Absyn_UnionA,(_tmp27B->f1).fieldnum=field_no,_tmp27B->f2=aggrdict;_tmp27B;});
return({struct _tuple21 _tmp5F1;_tmp5F1.f1=_tmp27D,_tmp5F1.f2=(void*)_tmp27A;_tmp5F1;});};};}}else{goto _LL6A;}}else{_LL6A: _LL6B:
# 1583
({struct Cyc_String_pa_PrintArg_struct _tmp280=({struct Cyc_String_pa_PrintArg_struct _tmp5F2;_tmp5F2.tag=0U,({struct _fat_ptr _tmp6E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5F2.f1=_tmp6E5;});_tmp5F2;});void*_tmp27E[1U];_tmp27E[0]=& _tmp280;({struct _fat_ptr _tmp6E6=({const char*_tmp27F="anal_Rexp:missing aggrdeclimpl in %s";_tag_fat(_tmp27F,sizeof(char),37U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6E6,_tag_fat(_tmp27E,sizeof(void*),1U));});});}case 26U: _LL6C: _tmp2E6=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL6D: {
# 1585
struct Cyc_List_List*_tmp281=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple30*))Cyc_Core_snd,_tmp2E6);
struct _tuple27 _tmp282=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp281,1,1);struct _tuple27 _tmp283=_tmp282;union Cyc_CfFlowInfo_FlowInfo _tmp285;struct Cyc_List_List*_tmp284;_LL191: _tmp285=_tmp283.f1;_tmp284=_tmp283.f2;_LL192:;
for(0;_tmp284 != 0;(_tmp284=_tmp284->tl,_tmp281=_tmp281->tl)){
_tmp285=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp281))->hd)->loc,_tmp285,(void*)_tmp284->hd);}
# 1590
return({struct _tuple21 _tmp5F3;_tmp5F3.f1=_tmp285,({void*_tmp6E7=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,(void*)_check_null(e->topt),0,_tmp119->notzeroall);_tmp5F3.f2=_tmp6E7;});_tmp5F3;});}case 27U: _LL6E: _tmp2EA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2E9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_tmp2E8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_tmp2E7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C)->f4;_LL6F: {
# 1594
struct _tuple21 _tmp286=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2E9);struct _tuple21 _tmp287=_tmp286;union Cyc_CfFlowInfo_FlowInfo _tmp2A2;void*_tmp2A1;_LL194: _tmp2A2=_tmp287.f1;_tmp2A1=_tmp287.f2;_LL195:;{
union Cyc_CfFlowInfo_FlowInfo _tmp288=_tmp2A2;struct Cyc_Dict_Dict _tmp2A0;struct Cyc_List_List*_tmp29F;if((_tmp288.BottomFL).tag == 1){_LL197: _LL198:
 return({struct _tuple21 _tmp5F4;_tmp5F4.f1=_tmp2A2,_tmp5F4.f2=_tmp119->unknown_all;_tmp5F4;});}else{_LL199: _tmp2A0=((_tmp288.ReachableFL).val).f1;_tmp29F=((_tmp288.ReachableFL).val).f2;_LL19A:
# 1598
 if((int)Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2A0,_tmp2A1)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp289=0U;({unsigned _tmp6E9=_tmp2E9->loc;struct _fat_ptr _tmp6E8=({const char*_tmp28A="expression may not be initialized";_tag_fat(_tmp28A,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6E9,_tmp6E8,_tag_fat(_tmp289,sizeof(void*),0U));});});{
# 1602
struct Cyc_List_List*new_relns=_tmp29F;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp2EA);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp2E9,& n2))
new_relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,_tmp29F);
if(_tmp29F != new_relns)
_tmp2A2=Cyc_CfFlowInfo_ReachableFL(_tmp2A0,new_relns);{
# 1611
void*_tmp28B=_tmp2A1;switch(*((int*)_tmp28B)){case 0U: _LL19C: _LL19D:
 return({struct _tuple21 _tmp5F5;_tmp5F5.f1=_tmp2A2,_tmp5F5.f2=_tmp119->unknown_all;_tmp5F5;});case 1U: _LL19E: _LL19F:
 goto _LL1A1;case 4U: _LL1A0: _LL1A1: {
# 1615
struct Cyc_List_List _tmp28C=({struct Cyc_List_List _tmp5F8;_tmp5F8.hd=_tmp2EA,_tmp5F8.tl=0;_tmp5F8;});
_tmp2A2=Cyc_NewControlFlow_add_vars(_tmp119,_tmp2A2,& _tmp28C,_tmp119->unknown_all,e->loc,0);{
# 1618
struct _tuple21 _tmp28D=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp2A2,_tmp2E8);struct _tuple21 _tmp28E=_tmp28D;union Cyc_CfFlowInfo_FlowInfo _tmp294;void*_tmp293;_LL1A5: _tmp294=_tmp28E.f1;_tmp293=_tmp28E.f2;_LL1A6:;
{union Cyc_CfFlowInfo_FlowInfo _tmp28F=_tmp294;struct Cyc_Dict_Dict _tmp292;if((_tmp28F.BottomFL).tag == 1){_LL1A8: _LL1A9:
 return({struct _tuple21 _tmp5F6;_tmp5F6.f1=_tmp294,_tmp5F6.f2=_tmp119->unknown_all;_tmp5F6;});}else{_LL1AA: _tmp292=((_tmp28F.ReachableFL).val).f1;_LL1AB:
# 1622
 if((int)Cyc_CfFlowInfo_initlevel(_tmp119,_tmp292,_tmp293)!= (int)Cyc_CfFlowInfo_AllIL){
({void*_tmp290=0U;({unsigned _tmp6EB=_tmp2E9->loc;struct _fat_ptr _tmp6EA=({const char*_tmp291="expression may not be initialized";_tag_fat(_tmp291,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6EB,_tmp6EA,_tag_fat(_tmp290,sizeof(void*),0U));});});
return({struct _tuple21 _tmp5F7;({union Cyc_CfFlowInfo_FlowInfo _tmp6EC=Cyc_CfFlowInfo_BottomFL();_tmp5F7.f1=_tmp6EC;}),_tmp5F7.f2=_tmp119->unknown_all;_tmp5F7;});}}_LL1A7:;}
# 1627
_tmp2A2=_tmp294;
goto _LL1A3;};}default: _LL1A2: _LL1A3:
# 1630
 while(1){
struct Cyc_List_List _tmp295=({struct Cyc_List_List _tmp5FA;_tmp5FA.hd=_tmp2EA,_tmp5FA.tl=0;_tmp5FA;});
_tmp2A2=Cyc_NewControlFlow_add_vars(_tmp119,_tmp2A2,& _tmp295,_tmp119->unknown_all,e->loc,0);{
struct _tuple21 _tmp296=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp2A2,_tmp2E8);struct _tuple21 _tmp297=_tmp296;union Cyc_CfFlowInfo_FlowInfo _tmp29E;void*_tmp29D;_LL1AD: _tmp29E=_tmp297.f1;_tmp29D=_tmp297.f2;_LL1AE:;
{union Cyc_CfFlowInfo_FlowInfo _tmp298=_tmp29E;struct Cyc_Dict_Dict _tmp29B;if((_tmp298.BottomFL).tag == 1){_LL1B0: _LL1B1:
 goto _LL1AF;}else{_LL1B2: _tmp29B=((_tmp298.ReachableFL).val).f1;_LL1B3:
# 1637
 if((int)Cyc_CfFlowInfo_initlevel(_tmp119,_tmp29B,_tmp29D)!= (int)Cyc_CfFlowInfo_AllIL){
({void*_tmp299=0U;({unsigned _tmp6EE=_tmp2E9->loc;struct _fat_ptr _tmp6ED=({const char*_tmp29A="expression may not be initialized";_tag_fat(_tmp29A,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6EE,_tmp6ED,_tag_fat(_tmp299,sizeof(void*),0U));});});
return({struct _tuple21 _tmp5F9;({union Cyc_CfFlowInfo_FlowInfo _tmp6EF=Cyc_CfFlowInfo_BottomFL();_tmp5F9.f1=_tmp6EF;}),_tmp5F9.f2=_tmp119->unknown_all;_tmp5F9;});}}_LL1AF:;}{
# 1642
union Cyc_CfFlowInfo_FlowInfo _tmp29C=Cyc_CfFlowInfo_join_flow(_tmp119,_tmp2A2,_tmp29E);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp29C,_tmp2A2))
break;
_tmp2A2=_tmp29C;};};}
# 1647
return({struct _tuple21 _tmp5FB;_tmp5FB.f1=_tmp2A2,_tmp5FB.f2=_tmp119->notzeroall;_tmp5FB;});}_LL19B:;};};}_LL196:;};}case 28U: _LL70: _tmp2ED=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp2EC=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_tmp2EB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_LL71: {
# 1653
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->tag=1U,_tmp2AB->f1=_tmp2ED,_tmp2AB->f2=(void*)_check_null(e->topt);_tmp2AB;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->root=root,_tmp2AA->path=0;_tmp2AA;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->tag=4U,_tmp2A9->f1=place;_tmp2A9;});
void*place_val;
# 1661
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp119,_tmp2EC,0,_tmp119->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple21 _tmp2A3=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2ED);struct _tuple21 _tmp2A4=_tmp2A3;union Cyc_CfFlowInfo_FlowInfo _tmp2A8;_LL1B5: _tmp2A8=_tmp2A4.f1;_LL1B6:;
outflow=_tmp2A8;{
union Cyc_CfFlowInfo_FlowInfo _tmp2A5=outflow;struct Cyc_Dict_Dict _tmp2A7;struct Cyc_List_List*_tmp2A6;if((_tmp2A5.BottomFL).tag == 1){_LL1B8: _LL1B9:
 return({struct _tuple21 _tmp5FC;_tmp5FC.f1=outflow,_tmp5FC.f2=rval;_tmp5FC;});}else{_LL1BA: _tmp2A7=((_tmp2A5.ReachableFL).val).f1;_tmp2A6=((_tmp2A5.ReachableFL).val).f2;_LL1BB:
# 1668
 return({struct _tuple21 _tmp5FD;({union Cyc_CfFlowInfo_FlowInfo _tmp6F1=({struct Cyc_Dict_Dict _tmp6F0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A7,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6F0,_tmp2A6);});_tmp5FD.f1=_tmp6F1;}),_tmp5FD.f2=rval;_tmp5FD;});}_LL1B7:;};};}case 37U: _LL72: _tmp2EE=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL73: {
# 1672
struct _tuple22 _tmp2AC=({struct _tuple22 _tmp5FF;_tmp5FF.f1=(env->fenv)->unknown_all,_tmp5FF.f2=copy_ctxt;_tmp5FF;});
union Cyc_CfFlowInfo_FlowInfo _tmp2AD=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp2EE,& _tmp2AC);
return({struct _tuple21 _tmp5FE;_tmp5FE.f1=_tmp2AD,_tmp5FE.f2=_tmp2AC.f1;_tmp5FE;});}case 36U: _LL76: _LL77:
# 1677
 goto _LL79;case 25U: _LL78: _LL79:
 goto _LL7B;case 39U: _LL7A: _LL7B:
 goto _LL7D;default: _LL7C: _LL7D:
({void*_tmp2AE=0U;({struct _fat_ptr _tmp6F2=({const char*_tmp2AF="anal_Rexp, unexpected exp form";_tag_fat(_tmp2AF,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6F2,_tag_fat(_tmp2AE,sizeof(void*),0U));});});}_LLD:;};}
# 1692 "new_control_flow.cyc"
static struct _tuple23 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1701
struct Cyc_CfFlowInfo_FlowEnv*_tmp2EF=env->fenv;
void*_tmp2F0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2F1=_tmp2F0;void*_tmp315;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->tag == 3U){_LL1: _tmp315=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->f1).ptr_atts).bounds;_LL2: {
# 1704
union Cyc_CfFlowInfo_FlowInfo _tmp2F2=f;struct Cyc_Dict_Dict _tmp312;struct Cyc_List_List*_tmp311;if((_tmp2F2.BottomFL).tag == 1){_LL6: _LL7:
 return({struct _tuple23 _tmp600;_tmp600.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp6F3=Cyc_CfFlowInfo_UnknownL();_tmp600.f2=_tmp6F3;});_tmp600;});}else{_LL8: _tmp312=((_tmp2F2.ReachableFL).val).f1;_tmp311=((_tmp2F2.ReachableFL).val).f2;_LL9: {
# 1708
struct _tuple20 _tmp2F3=Cyc_CfFlowInfo_unname_rval(r);struct _tuple20 _tmp2F4=_tmp2F3;void*_tmp310;struct Cyc_List_List*_tmp30F;_LLB: _tmp310=_tmp2F4.f1;_tmp30F=_tmp2F4.f2;_LLC:;
retry: {void*_tmp2F5=_tmp310;void*_tmp306;enum Cyc_CfFlowInfo_InitLevel _tmp305;void*_tmp304;struct Cyc_List_List*_tmp303;void*_tmp302;switch(*((int*)_tmp2F5)){case 8U: _LLE: _LLF:
# 1711
({void*_tmp2F6=0U;({struct _fat_ptr _tmp6F4=({const char*_tmp2F7="named location in anal_derefL";_tag_fat(_tmp2F7,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp6F4,_tag_fat(_tmp2F6,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 1713
({void*_tmp6F5=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->tag=Cyc_CfFlowInfo_NotZero,_tmp2F8->f1=_tmp311;_tmp2F8;});e->annot=_tmp6F5;});goto _LLD;case 5U: _LL12: _tmp302=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f1;_LL13:
# 1716
 if(expanded_place_possibly_null)
# 1719
({void*_tmp6F6=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2F9->f1=_tmp311;_tmp2F9;});e->annot=_tmp6F6;});else{
# 1722
void*_tmp2FA=e->annot;void*_tmp2FB=_tmp2FA;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2FB)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1F: _LL20:
# 1726
({void*_tmp6F7=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2FC->f1=_tmp311;_tmp2FC;});e->annot=_tmp6F7;});
goto _LL1E;}else{_LL21: _LL22:
# 1729
({void*_tmp6F8=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->tag=Cyc_CfFlowInfo_NotZero,_tmp2FD->f1=_tmp311;_tmp2FD;});e->annot=_tmp6F8;});
goto _LL1E;}_LL1E:;}
# 1734
if(Cyc_Tcutil_is_bound_one(_tmp315))return({struct _tuple23 _tmp601;_tmp601.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp6F9=Cyc_CfFlowInfo_UnknownL();_tmp601.f2=_tmp6F9;});_tmp601;});
goto _LLD;case 4U: _LL14: _tmp304=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f1)->root;_tmp303=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f1)->path;_LL15:
# 1738
({void*_tmp6FA=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE->tag=Cyc_CfFlowInfo_NotZero,_tmp2FE->f1=_tmp311;_tmp2FE;});e->annot=_tmp6FA;});
if(Cyc_Tcutil_is_bound_one(_tmp315))
return({struct _tuple23 _tmp602;_tmp602.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp6FC=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->root=_tmp304,({struct Cyc_List_List*_tmp6FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp303,path);_tmp2FF->path=_tmp6FB;});_tmp2FF;}));_tmp602.f2=_tmp6FC;});_tmp602;});
goto _LLD;case 0U: _LL16: _LL17:
# 1743
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple23 _tmp603;({union Cyc_CfFlowInfo_FlowInfo _tmp6FE=Cyc_CfFlowInfo_BottomFL();_tmp603.f1=_tmp6FE;}),({union Cyc_CfFlowInfo_AbsLVal _tmp6FD=Cyc_CfFlowInfo_UnknownL();_tmp603.f2=_tmp6FD;});_tmp603;});case 2U: _LL18: _tmp305=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f1;_LL19:
# 1748
 if(Cyc_Tcutil_is_bound_one(_tmp315))
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp305,_tmp30F);
({void*_tmp6FF=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->tag=Cyc_CfFlowInfo_UnknownZ,_tmp300->f1=_tmp311;_tmp300;});e->annot=_tmp6FF;});
goto _LLD;case 7U: _LL1A: _tmp306=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2F5)->f3;_LL1B:
# 1753
 if(passthrough_consumes){
_tmp310=_tmp306;goto retry;}
# 1756
goto _LL1D;default: _LL1C: _LL1D:
# 1758
({void*_tmp700=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->tag=Cyc_CfFlowInfo_UnknownZ,_tmp301->f1=_tmp311;_tmp301;});e->annot=_tmp700;});
goto _LLD;}_LLD:;}
# 1761
if((int)Cyc_CfFlowInfo_initlevel(_tmp2EF,_tmp312,_tmp310)== (int)Cyc_CfFlowInfo_NoneIL){
struct _tuple20 _tmp307=Cyc_CfFlowInfo_unname_rval(_tmp310);struct _tuple20 _tmp308=_tmp307;void*_tmp30E;_LL24: _tmp30E=_tmp308.f1;_LL25:;{
void*_tmp309=_tmp30E;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp309)->tag == 7U){_LL27: _LL28:
# 1765
({void*_tmp30A=0U;({unsigned _tmp702=e->loc;struct _fat_ptr _tmp701=({const char*_tmp30B="attempt to dereference an alias-free that has already been copied";_tag_fat(_tmp30B,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp702,_tmp701,_tag_fat(_tmp30A,sizeof(void*),0U));});});
goto _LL26;}else{_LL29: _LL2A:
# 1768
({void*_tmp30C=0U;({unsigned _tmp704=e->loc;struct _fat_ptr _tmp703=({const char*_tmp30D="dereference of possibly uninitialized pointer";_tag_fat(_tmp30D,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp704,_tmp703,_tag_fat(_tmp30C,sizeof(void*),0U));});});
goto _LL26;}_LL26:;};}
# 1772
return({struct _tuple23 _tmp604;_tmp604.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp705=Cyc_CfFlowInfo_UnknownL();_tmp604.f2=_tmp705;});_tmp604;});}}_LL5:;}}else{_LL3: _LL4:
# 1774
({void*_tmp313=0U;({struct _fat_ptr _tmp706=({const char*_tmp314="left deref of non-pointer-type";_tag_fat(_tmp314,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp706,_tag_fat(_tmp313,sizeof(void*),0U));});});}_LL0:;}
# 1780
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp316=p;void*_tmp31B;struct Cyc_List_List*_tmp31A;_LL1: _tmp31B=_tmp316->root;_tmp31A=_tmp316->path;_LL2:;
path=({struct Cyc_List_List*_tmp318=_cycalloc(sizeof(*_tmp318));({void*_tmp707=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->tag=1U;_tmp317;});_tmp318->hd=_tmp707;}),_tmp318->tl=path;_tmp318;});
return({struct Cyc_CfFlowInfo_Place*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319->root=_tmp31B,({struct Cyc_List_List*_tmp708=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp31A,path);_tmp319->path=_tmp708;});_tmp319;});}
# 1790
static struct _tuple23 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1794
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp31C=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp31D=inflow;struct Cyc_Dict_Dict _tmp31F;struct Cyc_List_List*_tmp31E;if((_tmp31D.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple23 _tmp605;({union Cyc_CfFlowInfo_FlowInfo _tmp70A=Cyc_CfFlowInfo_BottomFL();_tmp605.f1=_tmp70A;}),({union Cyc_CfFlowInfo_AbsLVal _tmp709=Cyc_CfFlowInfo_UnknownL();_tmp605.f2=_tmp709;});_tmp605;});}else{_LL3: _tmp31F=((_tmp31D.ReachableFL).val).f1;_tmp31E=((_tmp31D.ReachableFL).val).f2;_LL4:
# 1799
 d=_tmp31F;}_LL0:;}{
# 1803
void*_tmp320=e->r;void*_tmp321=_tmp320;struct Cyc_Absyn_Exp*_tmp378;struct _fat_ptr*_tmp377;struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp373;struct _fat_ptr*_tmp372;struct Cyc_Absyn_Vardecl*_tmp371;struct Cyc_Absyn_Vardecl*_tmp370;struct Cyc_Absyn_Vardecl*_tmp36F;struct Cyc_Absyn_Vardecl*_tmp36E;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*_tmp36C;struct Cyc_Absyn_Exp*_tmp36B;switch(*((int*)_tmp321)){case 14U: _LL6: _tmp36B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL7:
 _tmp36C=_tmp36B;goto _LL9;case 12U: _LL8: _tmp36C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LL9:
 _tmp36D=_tmp36C;goto _LLB;case 13U: _LLA: _tmp36D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LLB:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp36D,path);case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)){case 4U: _LLC: _tmp36E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LLD:
# 1809
 if((int)_tmp36E->sc == (int)Cyc_Absyn_Static)
return({struct _tuple23 _tmp606;_tmp606.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp70B=Cyc_CfFlowInfo_UnknownL();_tmp606.f2=_tmp70B;});_tmp606;});
_tmp36F=_tmp36E;goto _LLF;case 3U: _LLE: _tmp36F=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LLF:
 _tmp370=_tmp36F;goto _LL11;case 5U: _LL10: _tmp370=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LL11:
# 1814
 return({struct _tuple23 _tmp607;_tmp607.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp70D=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp323=_cycalloc(sizeof(*_tmp323));({void*_tmp70C=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->tag=0U,_tmp322->f1=_tmp370;_tmp322;});_tmp323->root=_tmp70C;}),_tmp323->path=path;_tmp323;}));_tmp607.f2=_tmp70D;});_tmp607;});case 1U: _LL12: _tmp371=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LL13:
# 1816
 return({struct _tuple23 _tmp608;_tmp608.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp70E=Cyc_CfFlowInfo_UnknownL();_tmp608.f2=_tmp70E;});_tmp608;});default: goto _LL1C;}case 22U: _LL14: _tmp373=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp372=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL15:
# 1819
{void*_tmp324=Cyc_Tcutil_compress((void*)_check_null(_tmp373->topt));void*_tmp325=_tmp324;void*_tmp32A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325)->tag == 3U){_LL1F: _tmp32A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325)->f1).elt_type;_LL20:
# 1821
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp32A)){
Cyc_NewControlFlow_check_union_requires(_tmp373->loc,_tmp32A,_tmp372,inflow);
path=({struct Cyc_List_List*_tmp327=_cycalloc(sizeof(*_tmp327));({void*_tmp710=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->tag=0U,({int _tmp70F=Cyc_CfFlowInfo_get_field_index(_tmp32A,_tmp372);_tmp326->f1=_tmp70F;});_tmp326;});_tmp327->hd=_tmp710;}),_tmp327->tl=path;_tmp327;});}
# 1825
goto _LL1E;}else{_LL21: _LL22:
({void*_tmp328=0U;({struct _fat_ptr _tmp711=({const char*_tmp329="anal_Lexp: AggrArrow ptr";_tag_fat(_tmp329,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp711,_tag_fat(_tmp328,sizeof(void*),0U));});});}_LL1E:;}
# 1828
_tmp374=_tmp373;goto _LL17;case 20U: _LL16: _tmp374=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LL17:
# 1832
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp374->topt),1)){
# 1834
struct _tuple23 _tmp32B=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp374);struct _tuple23 _tmp32C=_tmp32B;union Cyc_CfFlowInfo_FlowInfo _tmp34B;union Cyc_CfFlowInfo_AbsLVal _tmp34A;_LL24: _tmp34B=_tmp32C.f1;_tmp34A=_tmp32C.f2;_LL25:;{
struct _tuple23 _tmp32D=({struct _tuple23 _tmp60C;_tmp60C.f1=_tmp34B,_tmp60C.f2=_tmp34A;_tmp60C;});struct _tuple23 _tmp32E=_tmp32D;struct Cyc_Dict_Dict _tmp349;struct Cyc_List_List*_tmp348;struct Cyc_CfFlowInfo_Place*_tmp347;if(((_tmp32E.f1).ReachableFL).tag == 2){if(((_tmp32E.f2).PlaceL).tag == 1){_LL27: _tmp349=(((_tmp32E.f1).ReachableFL).val).f1;_tmp348=(((_tmp32E.f1).ReachableFL).val).f2;_tmp347=((_tmp32E.f2).PlaceL).val;_LL28: {
# 1837
void*_tmp32F=Cyc_CfFlowInfo_lookup_place(_tmp349,_tmp347);
struct _tuple20 _tmp330=Cyc_CfFlowInfo_unname_rval(_tmp32F);struct _tuple20 _tmp331=_tmp330;void*_tmp346;struct Cyc_List_List*_tmp345;_LL2C: _tmp346=_tmp331.f1;_tmp345=_tmp331.f2;_LL2D:;
if(expand_unique){
int _tmp332=1;
void*_tmp333=_tmp346;struct Cyc_Absyn_Vardecl*_tmp340;void*_tmp33F;switch(*((int*)_tmp333)){case 8U: _LL2F: _tmp340=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp333)->f1;_tmp33F=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp333)->f2;_LL30:
# 1843
({void*_tmp334=0U;({struct _fat_ptr _tmp712=({const char*_tmp335="bad named location in anal_Lexp:deref";_tag_fat(_tmp335,sizeof(char),38U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp712,_tag_fat(_tmp334,sizeof(void*),0U));});});case 7U: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp333)->f3)){case 4U: _LL31: _LL32:
 goto _LL34;case 5U: _LL35: _LL36:
# 1847
 goto _LL38;default: goto _LL3B;}case 4U: _LL33: _LL34:
# 1846
 return Cyc_NewControlFlow_anal_derefL(env,_tmp34B,_tmp374,_tmp34B,_tmp32F,passthrough_consumes,0,path);case 5U: _LL37: _LL38: {
# 1851
struct _tuple23 _tmp336=Cyc_NewControlFlow_anal_derefL(env,_tmp34B,_tmp374,_tmp34B,_tmp32F,passthrough_consumes,0,path);struct _tuple23 _tmp337=_tmp336;union Cyc_CfFlowInfo_FlowInfo _tmp338;_LL3E: _tmp338=_tmp337.f1;_LL3F:;
return({struct _tuple23 _tmp609;_tmp609.f1=_tmp338,({union Cyc_CfFlowInfo_AbsLVal _tmp713=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp347,path));_tmp609.f2=_tmp713;});_tmp609;});}case 1U: _LL39: _LL3A:
# 1854
 _tmp332=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1861
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp31C,_tmp349,_tmp346);
void*leaf=(int)il == (int)1U?_tmp31C->unknown_all: _tmp31C->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->tag=5U,({void*_tmp716=({struct Cyc_CfFlowInfo_FlowEnv*_tmp715=_tmp31C;void*_tmp714=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp374->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp715,_tmp714,0,leaf);});_tmp33E->f1=_tmp716;});_tmp33E;});
for(0;_tmp345 != 0;_tmp345=_tmp345->tl){
res=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->tag=8U,_tmp339->f1=(struct Cyc_Absyn_Vardecl*)_tmp345->hd,_tmp339->f2=res;_tmp339;});}
_tmp349=Cyc_CfFlowInfo_assign_place(_tmp31C,e->loc,_tmp349,_tmp347,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp33A=Cyc_CfFlowInfo_ReachableFL(_tmp349,_tmp348);
# 1874
struct _tuple23 _tmp33B=Cyc_NewControlFlow_anal_derefL(env,_tmp33A,_tmp374,_tmp33A,res,passthrough_consumes,_tmp332,path);struct _tuple23 _tmp33C=_tmp33B;union Cyc_CfFlowInfo_FlowInfo _tmp33D;_LL41: _tmp33D=_tmp33C.f1;_LL42:;
# 1876
return({struct _tuple23 _tmp60A;_tmp60A.f1=_tmp33D,({union Cyc_CfFlowInfo_AbsLVal _tmp717=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp347,path));_tmp60A.f2=_tmp717;});_tmp60A;});};}}_LL2E:;}else{
# 1880
void*_tmp341=_tmp346;switch(*((int*)_tmp341)){case 7U: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp341)->f3)->tag == 5U){_LL44: _LL45:
 goto _LL47;}else{goto _LL48;}case 5U: _LL46: _LL47: {
# 1885
struct _tuple23 _tmp342=Cyc_NewControlFlow_anal_derefL(env,_tmp34B,_tmp374,_tmp34B,_tmp32F,passthrough_consumes,0,path);struct _tuple23 _tmp343=_tmp342;union Cyc_CfFlowInfo_FlowInfo _tmp344;_LL4B: _tmp344=_tmp343.f1;_LL4C:;
return({struct _tuple23 _tmp60B;_tmp60B.f1=_tmp344,({union Cyc_CfFlowInfo_AbsLVal _tmp718=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp347,path));_tmp60B.f2=_tmp718;});_tmp60B;});}default: _LL48: _LL49:
# 1889
 goto _LL43;}_LL43:;}
# 1892
goto _LL26;}}else{goto _LL29;}}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;};}{
# 1897
struct _tuple21 _tmp34C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp374);struct _tuple21 _tmp34D=_tmp34C;union Cyc_CfFlowInfo_FlowInfo _tmp34F;void*_tmp34E;_LL4E: _tmp34F=_tmp34D.f1;_tmp34E=_tmp34D.f2;_LL4F:;
# 1899
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp374,_tmp34F,_tmp34E,passthrough_consumes,0,path);};case 23U: _LL18: _tmp376=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp375=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL19: {
# 1902
void*_tmp350=Cyc_Tcutil_compress((void*)_check_null(_tmp376->topt));void*_tmp351=_tmp350;switch(*((int*)_tmp351)){case 6U: _LL51: _LL52: {
# 1904
unsigned _tmp352=(Cyc_Evexp_eval_const_uint_exp(_tmp375)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp71E=env;union Cyc_CfFlowInfo_FlowInfo _tmp71D=inflow;int _tmp71C=expand_unique;int _tmp71B=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp71A=_tmp376;Cyc_NewControlFlow_anal_Lexp_rec(_tmp71E,_tmp71D,_tmp71C,_tmp71B,_tmp71A,({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));
({void*_tmp719=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->tag=0U,_tmp353->f1=(int)_tmp352;_tmp353;});_tmp354->hd=_tmp719;}),_tmp354->tl=path;_tmp354;}));});}case 3U: _LL53: _LL54: {
# 1908
struct _tuple27 _tmp355=({struct Cyc_NewControlFlow_AnalEnv*_tmp720=env;union Cyc_CfFlowInfo_FlowInfo _tmp71F=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp720,_tmp71F,({struct Cyc_Absyn_Exp*_tmp365[2U];_tmp365[0]=_tmp376,_tmp365[1]=_tmp375;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp365,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple27 _tmp356=_tmp355;union Cyc_CfFlowInfo_FlowInfo _tmp364;struct Cyc_List_List*_tmp363;_LL58: _tmp364=_tmp356.f1;_tmp363=_tmp356.f2;_LL59:;{
union Cyc_CfFlowInfo_FlowInfo _tmp357=_tmp364;
{union Cyc_CfFlowInfo_FlowInfo _tmp358=_tmp364;struct Cyc_Dict_Dict _tmp35D;struct Cyc_List_List*_tmp35C;if((_tmp358.ReachableFL).tag == 2){_LL5B: _tmp35D=((_tmp358.ReachableFL).val).f1;_tmp35C=((_tmp358.ReachableFL).val).f2;_LL5C:
# 1912
 if((int)Cyc_CfFlowInfo_initlevel(_tmp31C,_tmp35D,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp363))->tl))->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp359=0U;({unsigned _tmp722=_tmp375->loc;struct _fat_ptr _tmp721=({const char*_tmp35A="expression may not be initialized";_tag_fat(_tmp35A,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp722,_tmp721,_tag_fat(_tmp359,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp35B=Cyc_NewControlFlow_add_subscript_reln(_tmp35C,_tmp376,_tmp375);
if(_tmp35C != _tmp35B)
_tmp357=Cyc_CfFlowInfo_ReachableFL(_tmp35D,_tmp35B);
goto _LL5A;};}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}{
# 1921
struct _tuple23 _tmp35E=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp376,_tmp364,(void*)((struct Cyc_List_List*)_check_null(_tmp363))->hd,passthrough_consumes,0,path);struct _tuple23 _tmp35F=_tmp35E;union Cyc_CfFlowInfo_FlowInfo _tmp362;union Cyc_CfFlowInfo_AbsLVal _tmp361;_LL60: _tmp362=_tmp35F.f1;_tmp361=_tmp35F.f2;_LL61:;{
union Cyc_CfFlowInfo_FlowInfo _tmp360=_tmp362;if((_tmp360.BottomFL).tag == 1){_LL63: _LL64:
 return({struct _tuple23 _tmp60D;_tmp60D.f1=_tmp362,_tmp60D.f2=_tmp361;_tmp60D;});}else{_LL65: _LL66:
 return({struct _tuple23 _tmp60E;_tmp60E.f1=_tmp357,_tmp60E.f2=_tmp361;_tmp60E;});}_LL62:;};};};}default: _LL55: _LL56:
# 1926
({void*_tmp366=0U;({struct _fat_ptr _tmp723=({const char*_tmp367="anal_Lexp: Subscript -- bad type";_tag_fat(_tmp367,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp723,_tag_fat(_tmp366,sizeof(void*),0U));});});}_LL50:;}case 21U: _LL1A: _tmp378=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp377=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL1B: {
# 1930
void*_tmp368=(void*)_check_null(_tmp378->topt);
if(Cyc_Absyn_is_require_union_type(_tmp368))
Cyc_NewControlFlow_check_union_requires(_tmp378->loc,_tmp368,_tmp377,inflow);
# 1934
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp368))
return({struct _tuple23 _tmp60F;_tmp60F.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp724=Cyc_CfFlowInfo_UnknownL();_tmp60F.f2=_tmp724;});_tmp60F;});
# 1937
return({struct Cyc_NewControlFlow_AnalEnv*_tmp72B=env;union Cyc_CfFlowInfo_FlowInfo _tmp72A=inflow;int _tmp729=expand_unique;int _tmp728=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp727=_tmp378;Cyc_NewControlFlow_anal_Lexp_rec(_tmp72B,_tmp72A,_tmp729,_tmp728,_tmp727,({struct Cyc_List_List*_tmp36A=_cycalloc(sizeof(*_tmp36A));
({void*_tmp726=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369->tag=0U,({int _tmp725=Cyc_CfFlowInfo_get_field_index(_tmp368,_tmp377);_tmp369->f1=_tmp725;});_tmp369;});_tmp36A->hd=_tmp726;}),_tmp36A->tl=path;_tmp36A;}));});}default: _LL1C: _LL1D:
# 1941
 return({struct _tuple23 _tmp610;({union Cyc_CfFlowInfo_FlowInfo _tmp72D=Cyc_CfFlowInfo_BottomFL();_tmp610.f1=_tmp72D;}),({union Cyc_CfFlowInfo_AbsLVal _tmp72C=Cyc_CfFlowInfo_UnknownL();_tmp610.f2=_tmp72C;});_tmp610;});}_LL5:;};}
# 1945
static struct _tuple23 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1949
struct _tuple23 _tmp379=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple23 _tmp37A=_tmp379;union Cyc_CfFlowInfo_FlowInfo _tmp37C;union Cyc_CfFlowInfo_AbsLVal _tmp37B;_LL1: _tmp37C=_tmp37A.f1;_tmp37B=_tmp37A.f2;_LL2:;
return({struct _tuple23 _tmp611;_tmp611.f1=_tmp37C,_tmp611.f2=_tmp37B;_tmp611;});}
# 1956
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1959
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 1963
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
# 1966
struct _tuple23 _tmp37D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple23 _tmp37E=_tmp37D;union Cyc_CfFlowInfo_FlowInfo _tmp37F;_LL1: _tmp37F=_tmp37E.f1;_LL2:;
inflow=_tmp37F;}}}
# 1971
return inflow;}struct _tuple31{enum Cyc_Absyn_Primop f1;void*f2;void*f3;};
# 1977
static struct _tuple24 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1979
struct Cyc_CfFlowInfo_FlowEnv*_tmp380=env->fenv;
# 1982
struct _tuple21 _tmp381=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);struct _tuple21 _tmp382=_tmp381;union Cyc_CfFlowInfo_FlowInfo _tmp3BF;void*_tmp3BE;_LL1: _tmp3BF=_tmp382.f1;_tmp3BE=_tmp382.f2;_LL2:;{
struct _tuple21 _tmp383=Cyc_NewControlFlow_anal_Rexp(env,0,_tmp3BF,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);struct _tuple21 _tmp384=_tmp383;union Cyc_CfFlowInfo_FlowInfo _tmp3BD;void*_tmp3BC;_LL4: _tmp3BD=_tmp384.f1;_tmp3BC=_tmp384.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp385=_tmp3BD;
# 1987
union Cyc_CfFlowInfo_FlowInfo _tmp386=_tmp385;struct Cyc_Dict_Dict _tmp3BB;struct Cyc_List_List*_tmp3BA;if((_tmp386.BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple24 _tmp612;_tmp612.f1=_tmp385,_tmp612.f2=_tmp385;_tmp612;});}else{_LL9: _tmp3BB=((_tmp386.ReachableFL).val).f1;_tmp3BA=((_tmp386.ReachableFL).val).f2;_LLA: {
# 1990
struct Cyc_Absyn_Exp*_tmp387=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*_tmp388=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3BB,_tmp3BE)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer(_tmp3BE))
({void*_tmp389=0U;({unsigned _tmp72F=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _fat_ptr _tmp72E=({const char*_tmp38A="expression may not be initialized";_tag_fat(_tmp38A,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp72F,_tmp72E,_tag_fat(_tmp389,sizeof(void*),0U));});});
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3BB,_tmp3BC)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer(_tmp3BE))
({void*_tmp38B=0U;({unsigned _tmp731=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _fat_ptr _tmp730=({const char*_tmp38C="expression may not be initialized";_tag_fat(_tmp38C,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp731,_tmp730,_tag_fat(_tmp38B,sizeof(void*),0U));});});{
# 1997
union Cyc_CfFlowInfo_FlowInfo _tmp38D=_tmp385;
union Cyc_CfFlowInfo_FlowInfo _tmp38E=_tmp385;
# 2002
if((int)p == (int)5U ||(int)p == (int)6U){
struct _tuple20 _tmp38F=Cyc_CfFlowInfo_unname_rval(_tmp3BE);struct _tuple20 _tmp390=_tmp38F;void*_tmp3A8;struct Cyc_List_List*_tmp3A7;_LLC: _tmp3A8=_tmp390.f1;_tmp3A7=_tmp390.f2;_LLD:;{
struct _tuple20 _tmp391=Cyc_CfFlowInfo_unname_rval(_tmp3BC);struct _tuple20 _tmp392=_tmp391;void*_tmp3A6;struct Cyc_List_List*_tmp3A5;_LLF: _tmp3A6=_tmp392.f1;_tmp3A5=_tmp392.f2;_LL10:;{
struct _tuple0 _tmp393=({struct _tuple0 _tmp613;_tmp613.f1=_tmp3A8,_tmp613.f2=_tmp3A6;_tmp613;});struct _tuple0 _tmp394=_tmp393;enum Cyc_CfFlowInfo_InitLevel _tmp3A4;enum Cyc_CfFlowInfo_InitLevel _tmp3A3;switch(*((int*)_tmp394.f1)){case 2U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp394.f2)->tag == 0U){_LL12: _tmp3A3=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp394.f1)->f1;_LL13: {
# 2007
struct _tuple24 _tmp395=Cyc_NewControlFlow_splitzero(env,inflow,_tmp385,_tmp387,_tmp3A3,_tmp3A7);struct _tuple24 _tmp396=_tmp395;union Cyc_CfFlowInfo_FlowInfo _tmp39B;union Cyc_CfFlowInfo_FlowInfo _tmp39A;_LL23: _tmp39B=_tmp396.f1;_tmp39A=_tmp396.f2;_LL24:;
{enum Cyc_Absyn_Primop _tmp397=p;switch(_tmp397){case Cyc_Absyn_Eq: _LL26: _LL27:
 _tmp38D=_tmp39A;_tmp38E=_tmp39B;goto _LL25;case Cyc_Absyn_Neq: _LL28: _LL29:
 _tmp38D=_tmp39B;_tmp38E=_tmp39A;goto _LL25;default: _LL2A: _LL2B:
({void*_tmp398=0U;({struct _fat_ptr _tmp732=({const char*_tmp399="anal_test, zero-split";_tag_fat(_tmp399,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp732,_tag_fat(_tmp398,sizeof(void*),0U));});});}_LL25:;}
# 2013
goto _LL11;}}else{goto _LL20;}case 0U: switch(*((int*)_tmp394.f2)){case 2U: _LL14: _tmp3A4=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp394.f2)->f1;_LL15: {
# 2015
struct _tuple24 _tmp39C=Cyc_NewControlFlow_splitzero(env,_tmp3BD,_tmp385,_tmp388,_tmp3A4,_tmp3A5);struct _tuple24 _tmp39D=_tmp39C;union Cyc_CfFlowInfo_FlowInfo _tmp3A2;union Cyc_CfFlowInfo_FlowInfo _tmp3A1;_LL2D: _tmp3A2=_tmp39D.f1;_tmp3A1=_tmp39D.f2;_LL2E:;
{enum Cyc_Absyn_Primop _tmp39E=p;switch(_tmp39E){case Cyc_Absyn_Eq: _LL30: _LL31:
 _tmp38D=_tmp3A1;_tmp38E=_tmp3A2;goto _LL2F;case Cyc_Absyn_Neq: _LL32: _LL33:
 _tmp38D=_tmp3A2;_tmp38E=_tmp3A1;goto _LL2F;default: _LL34: _LL35:
({void*_tmp39F=0U;({struct _fat_ptr _tmp733=({const char*_tmp3A0="anal_test, zero-split";_tag_fat(_tmp3A0,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp733,_tag_fat(_tmp39F,sizeof(void*),0U));});});}_LL2F:;}
# 2021
goto _LL11;}case 0U: _LL16: _LL17:
# 2023
 if((int)p == (int)5U)_tmp38E=Cyc_CfFlowInfo_BottomFL();else{
_tmp38D=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;case 1U: _LL18: _LL19:
 goto _LL1B;case 4U: _LL1A: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp394.f2)->tag == 0U){_LL1C: _LL1D:
 goto _LL1F;}else{goto _LL20;}case 4U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp394.f2)->tag == 0U){_LL1E: _LL1F:
# 2030
 if((int)p == (int)6U)_tmp38E=Cyc_CfFlowInfo_BottomFL();else{
_tmp38D=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;}else{goto _LL20;}default: _LL20: _LL21:
 goto _LL11;}_LL11:;};};}
# 2042
{struct _tuple31 _tmp3A9=({struct _tuple31 _tmp615;_tmp615.f1=p,({
void*_tmp735=Cyc_Tcutil_compress((void*)_check_null(_tmp387->topt));_tmp615.f2=_tmp735;}),({
void*_tmp734=Cyc_Tcutil_compress((void*)_check_null(_tmp388->topt));_tmp615.f3=_tmp734;});_tmp615;});
# 2042
struct _tuple31 _tmp3AA=_tmp3A9;switch(_tmp3AA.f1){case Cyc_Absyn_Eq: _LL37: _LL38:
# 2045
 goto _LL3A;case Cyc_Absyn_Neq: _LL39: _LL3A:
 goto _LL3C;default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3B: _LL3C:
 goto _LL3E;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{goto _LL43;}case 4U: goto _LL41;default: goto _LL43;}else{goto _LL43;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f2)->f1)->tag == 4U)goto _LL3F;else{goto _LL43;}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f2)->f1)->tag == 4U)goto _LL3F;else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->f1)->tag == 4U)goto _LL41;else{goto _LL43;}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f2)->f1)->tag == 4U){_LL3F: _LL40:
# 2049
 goto _LL42;}else{goto _LL43;}}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA.f3)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3D: _LL3E:
# 2048
 goto _LL40;}else{goto _LL43;}case 4U: _LL41: _LL42:
# 2050
 goto _LL36;default: goto _LL43;}else{_LL43: _LL44:
 return({struct _tuple24 _tmp614;_tmp614.f1=_tmp38D,_tmp614.f2=_tmp38E;_tmp614;});}}}_LL36:;}
# 2055
{enum Cyc_Absyn_Primop _tmp3AB=p;switch(_tmp3AB){case Cyc_Absyn_Eq: _LL46: _LL47:
 goto _LL49;case Cyc_Absyn_Neq: _LL48: _LL49: goto _LL4B;case Cyc_Absyn_Gt: _LL4A: _LL4B: goto _LL4D;case Cyc_Absyn_Gte: _LL4C: _LL4D: goto _LL4F;case Cyc_Absyn_Lt: _LL4E: _LL4F: goto _LL51;case Cyc_Absyn_Lte: _LL50: _LL51: goto _LL45;default: _LL52: _LL53:
 return({struct _tuple24 _tmp616;_tmp616.f1=_tmp38D,_tmp616.f2=_tmp38E;_tmp616;});}_LL45:;}{
# 2060
struct _RegionHandle*_tmp3AC=Cyc_Core_heap_region;
struct _tuple18 _tmp3AD=Cyc_Relations_primop2relation(_tmp387,p,_tmp388);struct _tuple18 _tmp3AE=_tmp3AD;struct Cyc_Absyn_Exp*_tmp3B9;enum Cyc_Relations_Relation _tmp3B8;struct Cyc_Absyn_Exp*_tmp3B7;_LL55: _tmp3B9=_tmp3AE.f1;_tmp3B8=_tmp3AE.f2;_tmp3B7=_tmp3AE.f3;_LL56:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2065
if(Cyc_Relations_exp2relnop(_tmp3B9,& n1)&& Cyc_Relations_exp2relnop(_tmp3B7,& n2)){
# 2067
struct Cyc_List_List*_tmp3AF=Cyc_Relations_add_relation(_tmp3AC,n1,_tmp3B8,n2,_tmp3BA);
# 2071
struct Cyc_List_List*_tmp3B0=({struct _RegionHandle*_tmp739=_tmp3AC;union Cyc_Relations_RelnOp _tmp738=n2;enum Cyc_Relations_Relation _tmp737=Cyc_Relations_flip_relation(_tmp3B8);union Cyc_Relations_RelnOp _tmp736=n1;Cyc_Relations_add_relation(_tmp739,_tmp738,_tmp737,_tmp736,_tmp3BA);});
struct _tuple24 _tmp3B1=({struct _tuple24 _tmp61B;_tmp61B.f1=_tmp38D,_tmp61B.f2=_tmp38E;_tmp61B;});struct _tuple24 _tmp3B2=_tmp3B1;struct Cyc_Dict_Dict _tmp3B6;struct Cyc_Dict_Dict _tmp3B5;struct Cyc_Dict_Dict _tmp3B4;struct Cyc_Dict_Dict _tmp3B3;if(((_tmp3B2.f1).ReachableFL).tag == 2){if(((_tmp3B2.f2).ReachableFL).tag == 2){_LL58: _tmp3B4=(((_tmp3B2.f1).ReachableFL).val).f1;_tmp3B3=(((_tmp3B2.f2).ReachableFL).val).f1;_LL59:
# 2074
 return({struct _tuple24 _tmp617;({union Cyc_CfFlowInfo_FlowInfo _tmp73B=Cyc_CfFlowInfo_ReachableFL(_tmp3B4,_tmp3AF);_tmp617.f1=_tmp73B;}),({union Cyc_CfFlowInfo_FlowInfo _tmp73A=Cyc_CfFlowInfo_ReachableFL(_tmp3B3,_tmp3B0);_tmp617.f2=_tmp73A;});_tmp617;});}else{_LL5C: _tmp3B5=(((_tmp3B2.f1).ReachableFL).val).f1;_LL5D:
# 2078
 return({struct _tuple24 _tmp618;({union Cyc_CfFlowInfo_FlowInfo _tmp73C=Cyc_CfFlowInfo_ReachableFL(_tmp3B5,_tmp3AF);_tmp618.f1=_tmp73C;}),_tmp618.f2=_tmp38E;_tmp618;});}}else{if(((_tmp3B2.f2).ReachableFL).tag == 2){_LL5A: _tmp3B6=(((_tmp3B2.f2).ReachableFL).val).f1;_LL5B:
# 2076
 return({struct _tuple24 _tmp619;_tmp619.f1=_tmp38D,({union Cyc_CfFlowInfo_FlowInfo _tmp73D=Cyc_CfFlowInfo_ReachableFL(_tmp3B6,_tmp3B0);_tmp619.f2=_tmp73D;});_tmp619;});}else{_LL5E: _LL5F:
# 2080
 return({struct _tuple24 _tmp61A;_tmp61A.f1=_tmp38D,_tmp61A.f2=_tmp38E;_tmp61A;});}}_LL57:;}else{
# 2083
return({struct _tuple24 _tmp61C;_tmp61C.f1=_tmp38D,_tmp61C.f2=_tmp38E;_tmp61C;});}};};};}}_LL6:;};};}
# 2089
static struct _tuple24 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C0=env->fenv;
void*_tmp3C1=e->r;void*_tmp3C2=_tmp3C1;enum Cyc_Absyn_Primop _tmp406;struct Cyc_List_List*_tmp405;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp402;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FB;switch(*((int*)_tmp3C2)){case 6U: _LL1: _tmp3FD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3FC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_tmp3FB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f3;_LL2: {
# 2093
struct _tuple24 _tmp3C3=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3FD);struct _tuple24 _tmp3C4=_tmp3C3;union Cyc_CfFlowInfo_FlowInfo _tmp3CE;union Cyc_CfFlowInfo_FlowInfo _tmp3CD;_LL10: _tmp3CE=_tmp3C4.f1;_tmp3CD=_tmp3C4.f2;_LL11:;{
struct _tuple24 _tmp3C5=Cyc_NewControlFlow_anal_test(env,_tmp3CE,_tmp3FC);struct _tuple24 _tmp3C6=_tmp3C5;union Cyc_CfFlowInfo_FlowInfo _tmp3CC;union Cyc_CfFlowInfo_FlowInfo _tmp3CB;_LL13: _tmp3CC=_tmp3C6.f1;_tmp3CB=_tmp3C6.f2;_LL14:;{
struct _tuple24 _tmp3C7=Cyc_NewControlFlow_anal_test(env,_tmp3CD,_tmp3FB);struct _tuple24 _tmp3C8=_tmp3C7;union Cyc_CfFlowInfo_FlowInfo _tmp3CA;union Cyc_CfFlowInfo_FlowInfo _tmp3C9;_LL16: _tmp3CA=_tmp3C8.f1;_tmp3C9=_tmp3C8.f2;_LL17:;
return({struct _tuple24 _tmp61D;({union Cyc_CfFlowInfo_FlowInfo _tmp73F=Cyc_CfFlowInfo_join_flow(_tmp3C0,_tmp3CC,_tmp3CA);_tmp61D.f1=_tmp73F;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp73E=Cyc_CfFlowInfo_join_flow(_tmp3C0,_tmp3CB,_tmp3C9);_tmp61D.f2=_tmp73E;});_tmp61D;});};};}case 7U: _LL3: _tmp3FF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3FE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL4: {
# 2099
struct _tuple24 _tmp3CF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3FF);struct _tuple24 _tmp3D0=_tmp3CF;union Cyc_CfFlowInfo_FlowInfo _tmp3D6;union Cyc_CfFlowInfo_FlowInfo _tmp3D5;_LL19: _tmp3D6=_tmp3D0.f1;_tmp3D5=_tmp3D0.f2;_LL1A:;{
struct _tuple24 _tmp3D1=Cyc_NewControlFlow_anal_test(env,_tmp3D6,_tmp3FE);struct _tuple24 _tmp3D2=_tmp3D1;union Cyc_CfFlowInfo_FlowInfo _tmp3D4;union Cyc_CfFlowInfo_FlowInfo _tmp3D3;_LL1C: _tmp3D4=_tmp3D2.f1;_tmp3D3=_tmp3D2.f2;_LL1D:;
return({struct _tuple24 _tmp61E;_tmp61E.f1=_tmp3D4,({union Cyc_CfFlowInfo_FlowInfo _tmp740=Cyc_CfFlowInfo_join_flow(_tmp3C0,_tmp3D5,_tmp3D3);_tmp61E.f2=_tmp740;});_tmp61E;});};}case 8U: _LL5: _tmp401=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp400=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL6: {
# 2103
struct _tuple24 _tmp3D7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp401);struct _tuple24 _tmp3D8=_tmp3D7;union Cyc_CfFlowInfo_FlowInfo _tmp3DE;union Cyc_CfFlowInfo_FlowInfo _tmp3DD;_LL1F: _tmp3DE=_tmp3D8.f1;_tmp3DD=_tmp3D8.f2;_LL20:;{
struct _tuple24 _tmp3D9=Cyc_NewControlFlow_anal_test(env,_tmp3DD,_tmp400);struct _tuple24 _tmp3DA=_tmp3D9;union Cyc_CfFlowInfo_FlowInfo _tmp3DC;union Cyc_CfFlowInfo_FlowInfo _tmp3DB;_LL22: _tmp3DC=_tmp3DA.f1;_tmp3DB=_tmp3DA.f2;_LL23:;
return({struct _tuple24 _tmp61F;({union Cyc_CfFlowInfo_FlowInfo _tmp741=Cyc_CfFlowInfo_join_flow(_tmp3C0,_tmp3DE,_tmp3DC);_tmp61F.f1=_tmp741;}),_tmp61F.f2=_tmp3DB;_tmp61F;});};}case 9U: _LL7: _tmp403=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp402=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL8: {
# 2107
struct _tuple21 _tmp3DF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp403);struct _tuple21 _tmp3E0=_tmp3DF;union Cyc_CfFlowInfo_FlowInfo _tmp3E2;void*_tmp3E1;_LL25: _tmp3E2=_tmp3E0.f1;_tmp3E1=_tmp3E0.f2;_LL26:;
return Cyc_NewControlFlow_anal_test(env,_tmp3E2,_tmp402);}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->tl == 0){_LL9: _tmp404=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->hd;_LLA: {
# 2110
struct _tuple24 _tmp3E3=Cyc_NewControlFlow_anal_test(env,inflow,_tmp404);struct _tuple24 _tmp3E4=_tmp3E3;union Cyc_CfFlowInfo_FlowInfo _tmp3E6;union Cyc_CfFlowInfo_FlowInfo _tmp3E5;_LL28: _tmp3E6=_tmp3E4.f1;_tmp3E5=_tmp3E4.f2;_LL29:;
return({struct _tuple24 _tmp620;_tmp620.f1=_tmp3E5,_tmp620.f2=_tmp3E6;_tmp620;});}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp406=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp405=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LLC:
# 2113
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp406,_tmp405);}default: _LLD: _LLE: {
# 2117
struct _tuple21 _tmp3E7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple21 _tmp3E8=_tmp3E7;union Cyc_CfFlowInfo_FlowInfo _tmp3FA;void*_tmp3F9;_LL2B: _tmp3FA=_tmp3E8.f1;_tmp3F9=_tmp3E8.f2;_LL2C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E9=_tmp3FA;struct Cyc_Dict_Dict _tmp3F8;if((_tmp3E9.BottomFL).tag == 1){_LL2E: _LL2F:
 return({struct _tuple24 _tmp621;_tmp621.f1=_tmp3FA,_tmp621.f2=_tmp3FA;_tmp621;});}else{_LL30: _tmp3F8=((_tmp3E9.ReachableFL).val).f1;_LL31: {
# 2121
struct _tuple20 _tmp3EA=Cyc_CfFlowInfo_unname_rval(_tmp3F9);struct _tuple20 _tmp3EB=_tmp3EA;void*_tmp3F7;struct Cyc_List_List*_tmp3F6;_LL33: _tmp3F7=_tmp3EB.f1;_tmp3F6=_tmp3EB.f2;_LL34:;{
void*_tmp3EC=_tmp3F7;enum Cyc_CfFlowInfo_InitLevel _tmp3F5;struct Cyc_Absyn_Vardecl*_tmp3F4;void*_tmp3F3;switch(*((int*)_tmp3EC)){case 8U: _LL36: _tmp3F4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f1;_tmp3F3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f2;_LL37:
# 2124
({void*_tmp3ED=0U;({struct _fat_ptr _tmp742=({const char*_tmp3EE="anal_test: bad namedlocation";_tag_fat(_tmp3EE,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp742,_tag_fat(_tmp3ED,sizeof(void*),0U));});});case 0U: _LL38: _LL39:
 return({struct _tuple24 _tmp622;({union Cyc_CfFlowInfo_FlowInfo _tmp743=Cyc_CfFlowInfo_BottomFL();_tmp622.f1=_tmp743;}),_tmp622.f2=_tmp3FA;_tmp622;});case 1U: _LL3A: _LL3B:
 goto _LL3D;case 5U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F:
 return({struct _tuple24 _tmp623;_tmp623.f1=_tmp3FA,({union Cyc_CfFlowInfo_FlowInfo _tmp744=Cyc_CfFlowInfo_BottomFL();_tmp623.f2=_tmp744;});_tmp623;});case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40: _LL41:
 goto _LL43;}else{_LL46: _tmp3F5=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f1;_LL47:
# 2134
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp3FA,e,_tmp3F5,_tmp3F6);}case 3U: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp3EC)->f1 == Cyc_CfFlowInfo_NoneIL){_LL42: _LL43:
# 2130
 goto _LL45;}else{_LL48: _LL49:
# 2135
 return({struct _tuple24 _tmp624;_tmp624.f1=_tmp3FA,_tmp624.f2=_tmp3FA;_tmp624;});}case 7U: _LL44: _LL45:
# 2132
({void*_tmp3EF=0U;({unsigned _tmp746=e->loc;struct _fat_ptr _tmp745=({const char*_tmp3F0="expression may not be initialized";_tag_fat(_tmp3F0,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp746,_tmp745,_tag_fat(_tmp3EF,sizeof(void*),0U));});});
return({struct _tuple24 _tmp625;({union Cyc_CfFlowInfo_FlowInfo _tmp748=Cyc_CfFlowInfo_BottomFL();_tmp625.f1=_tmp748;}),({union Cyc_CfFlowInfo_FlowInfo _tmp747=Cyc_CfFlowInfo_BottomFL();_tmp625.f2=_tmp747;});_tmp625;});default: _LL4A: _LL4B:
# 2136
({void*_tmp3F1=0U;({struct _fat_ptr _tmp749=({const char*_tmp3F2="anal_test";_tag_fat(_tmp3F2,sizeof(char),10U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp749,_tag_fat(_tmp3F1,sizeof(void*),0U));});});}_LL35:;};}}_LL2D:;};}}_LL0:;}struct _tuple32{unsigned f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2142
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple32*ckenv,void*root,void*rval){
# 2144
struct _tuple32*_tmp407=ckenv;unsigned _tmp413;struct Cyc_NewControlFlow_AnalEnv*_tmp412;struct Cyc_Dict_Dict _tmp411;_LL1: _tmp413=_tmp407->f1;_tmp412=_tmp407->f2;_tmp411=_tmp407->f3;_LL2:;{
void*_tmp408=root;struct Cyc_Absyn_Vardecl*_tmp410;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp408)->tag == 0U){_LL4: _tmp410=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp408)->f1;_LL5:
# 2147
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp410->type)){
struct _tuple20 _tmp409=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple20 _tmp40A=_tmp409;void*_tmp40F;_LL9: _tmp40F=_tmp40A.f1;_LLA:;{
void*_tmp40B=_tmp40F;switch(*((int*)_tmp40B)){case 7U: _LLC: _LLD:
 goto _LLF;case 0U: _LLE: _LLF:
 goto _LL11;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp40B)->f1 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12: _LL13:
# 2154
({struct Cyc_String_pa_PrintArg_struct _tmp40E=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({struct _fat_ptr _tmp74A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp410->name));_tmp626.f1=_tmp74A;});_tmp626;});void*_tmp40C[1U];_tmp40C[0]=& _tmp40E;({unsigned _tmp74C=_tmp413;struct _fat_ptr _tmp74B=({const char*_tmp40D="unique pointers reachable from %s may become unreachable";_tag_fat(_tmp40D,sizeof(char),57U);});Cyc_Warn_warn(_tmp74C,_tmp74B,_tag_fat(_tmp40C,sizeof(void*),1U));});});
goto _LLB;}_LLB:;};}
# 2158
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 2163
static void Cyc_NewControlFlow_check_init_params(unsigned loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp414=flow;struct Cyc_Dict_Dict _tmp419;if((_tmp414.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp419=((_tmp414.ReachableFL).val).f1;_LL4:
# 2167
{struct Cyc_List_List*_tmp415=env->param_roots;for(0;_tmp415 != 0;_tmp415=_tmp415->tl){
if((int)({struct Cyc_CfFlowInfo_FlowEnv*_tmp74E=env->fenv;struct Cyc_Dict_Dict _tmp74D=_tmp419;Cyc_CfFlowInfo_initlevel(_tmp74E,_tmp74D,Cyc_CfFlowInfo_lookup_place(_tmp419,(struct Cyc_CfFlowInfo_Place*)_tmp415->hd));})!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp416=0U;({unsigned _tmp750=loc;struct _fat_ptr _tmp74F=({const char*_tmp417="function may not initialize all the parameters with attribute 'initializes'";_tag_fat(_tmp417,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp750,_tmp74F,_tag_fat(_tmp416,sizeof(void*),0U));});});}}
# 2171
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple32 _tmp418=({struct _tuple32 _tmp627;_tmp627.f1=loc,_tmp627.f2=env,_tmp627.f3=_tmp419;_tmp627;});
((void(*)(void(*f)(struct _tuple32*,void*,void*),struct _tuple32*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp418,_tmp419);}
# 2175
return;}_LL0:;}
# 2184
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2186
struct Cyc_List_List*_tmp41A=0;
struct Cyc_List_List*_tmp41B=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple28*_tmp41C=(struct _tuple28*)x->hd;struct _tuple28*_tmp41D=_tmp41C;struct Cyc_Absyn_Vardecl**_tmp425;struct Cyc_Absyn_Exp*_tmp424;_LL1: _tmp425=_tmp41D->f1;_tmp424=_tmp41D->f2;_LL2:;
if((_tmp425 != 0 && _tmp424 != 0)&&
 Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp424->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp41E=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->tag=0U,_tmp423->f1=*_tmp425;_tmp423;});
struct Cyc_CfFlowInfo_Place*_tmp41F=({struct Cyc_CfFlowInfo_Place*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->root=(void*)_tmp41E,_tmp422->path=0;_tmp422;});
_tmp41A=({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->hd=_tmp41F,_tmp420->tl=_tmp41A;_tmp420;});
_tmp41B=({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->hd=_tmp424,_tmp421->tl=_tmp41B;_tmp421;});}}}
# 2198
return({struct _tuple1 _tmp628;_tmp628.f1=_tmp41A,_tmp628.f2=_tmp41B;_tmp628;});}
# 2204
static struct _tuple15 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2211
union Cyc_CfFlowInfo_FlowInfo _tmp426=flow;struct Cyc_Dict_Dict _tmp43A;if((_tmp426.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp427=0U;({struct _fat_ptr _tmp751=({const char*_tmp428="noconsume_place_ok: flow became Bottom!";_tag_fat(_tmp428,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp751,_tag_fat(_tmp427,sizeof(void*),0U));});});}else{_LL3: _tmp43A=((_tmp426.ReachableFL).val).f1;_LL4: {
# 2219
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_uint_exp(1U,0U);
int _tmp42A=0;
int _tmp42B=1;
void*_tmp42C=Cyc_CfFlowInfo_lookup_place(_tmp43A,place);
void*_tmp42D=_tmp42C;
# 2231
int varok=0;
{void*_tmp42E=_tmp42C;struct Cyc_Absyn_Vardecl*_tmp439;void*_tmp438;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42E)->tag == 8U){_LL6: _tmp439=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42E)->f1;_tmp438=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42E)->f2;_LL7:
# 2234
 if(vd == _tmp439){
_tmp42D=_tmp438;
# 2237
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2239
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp429,_tmp42B,_tmp42D,& _tmp42A)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp431=({struct Cyc_String_pa_PrintArg_struct _tmp629;_tmp629.tag=0U,({
# 2243
struct _fat_ptr _tmp752=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp629.f1=_tmp752;});_tmp629;});void*_tmp42F[1U];_tmp42F[0]=& _tmp431;({unsigned _tmp754=loc;struct _fat_ptr _tmp753=({const char*_tmp430="function consumes parameter %s which is does not have the 'consume' attribute";_tag_fat(_tmp430,sizeof(char),78U);});Cyc_CfFlowInfo_aerr(_tmp754,_tmp753,_tag_fat(_tmp42F,sizeof(void*),1U));});});}else{
# 2246
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,_tmp43A,_tmp42D)!= (int)Cyc_CfFlowInfo_AllIL && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp434=({struct Cyc_String_pa_PrintArg_struct _tmp62A;_tmp62A.tag=0U,({
# 2249
struct _fat_ptr _tmp755=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp62A.f1=_tmp755;});_tmp62A;});void*_tmp432[1U];_tmp432[0]=& _tmp434;({unsigned _tmp757=loc;struct _fat_ptr _tmp756=({const char*_tmp433="function consumes value pointed to by parameter %s, which does not have the 'consume' attribute";_tag_fat(_tmp433,sizeof(char),96U);});Cyc_CfFlowInfo_aerr(_tmp757,_tmp756,_tag_fat(_tmp432,sizeof(void*),1U));});});else{
# 2251
varok=1;}}}else{
# 2254
varok=1;}}else{
# 2257
goto _LL9;}
goto _LL5;}else{_LL8: _LL9:
# 2261
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp437=({struct Cyc_String_pa_PrintArg_struct _tmp62B;_tmp62B.tag=0U,({
# 2266
struct _fat_ptr _tmp758=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp62B.f1=_tmp758;});_tmp62B;});void*_tmp435[1U];_tmp435[0]=& _tmp437;({unsigned _tmp75A=loc;struct _fat_ptr _tmp759=({const char*_tmp436="function parameter %s without 'consume' attribute no longer set to its original value";_tag_fat(_tmp436,sizeof(char),86U);});Cyc_CfFlowInfo_aerr(_tmp75A,_tmp759,_tag_fat(_tmp435,sizeof(void*),1U));});});}
goto _LL5;}_LL5:;}
# 2273
return({struct _tuple15 _tmp62C;_tmp62C.f1=varok,_tmp62C.f2=_tmp42D;_tmp62C;});}}_LL0:;}
# 2279
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp43B=p;void*_tmp443;struct Cyc_List_List*_tmp442;_LL1: _tmp443=_tmp43B->root;_tmp442=_tmp43B->path;_LL2:;
if(_tmp442 != 0)
({void*_tmp43C=0U;({struct _fat_ptr _tmp75B=({const char*_tmp43D="unconsume_params: param to unconsume is non-variable";_tag_fat(_tmp43D,sizeof(char),53U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp75B,_tag_fat(_tmp43C,sizeof(void*),0U));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp43E=_tmp443;struct Cyc_Absyn_Vardecl*_tmp441;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp43E)->tag == 0U){_LL4: _tmp441=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp43E)->f1;_LL5:
 return _tmp441;}else{_LL6: _LL7:
({void*_tmp43F=0U;({struct _fat_ptr _tmp75C=({const char*_tmp440="unconsume_params: root is not a varroot";_tag_fat(_tmp440,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp75C,_tag_fat(_tmp43F,sizeof(void*),0U));});});}_LL3:;};}
# 2298 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2306
{union Cyc_CfFlowInfo_FlowInfo _tmp444=flow;struct Cyc_Dict_Dict _tmp45B;if((_tmp444.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _tmp45B=((_tmp444.ReachableFL).val).f1;_LL4: {
# 2313
struct _tuple23 _tmp445=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple23 _tmp446=_tmp445;union Cyc_CfFlowInfo_FlowInfo _tmp45A;union Cyc_CfFlowInfo_AbsLVal _tmp459;_LL6: _tmp45A=_tmp446.f1;_tmp459=_tmp446.f2;_LL7:;
# 2316
{union Cyc_CfFlowInfo_AbsLVal _tmp447=_tmp459;struct Cyc_CfFlowInfo_Place*_tmp458;if((_tmp447.PlaceL).tag == 1){_LL9: _tmp458=(_tmp447.PlaceL).val;_LLA: {
# 2320
void*_tmp448=Cyc_CfFlowInfo_lookup_place(_tmp45B,_tmp458);
{void*_tmp449=_tmp448;struct Cyc_Absyn_Vardecl*_tmp457;void*_tmp456;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp449)->tag == 8U){_LLE: _tmp457=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp449)->f1;_tmp456=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp449)->f2;_LLF: {
# 2326
void*new_rval;
if(_tmp457 == vd){
# 2330
if(varok){
# 2332
_tmp448=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp456);
# 2337
if(ropt != 0){
# 2343
struct _tuple21 _tmp44A=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp75E=env->fenv;struct _tuple21 _tmp75D=({struct _tuple21 _tmp62D;_tmp62D.f1=_tmp45A,_tmp62D.f2=_tmp448;_tmp62D;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp75E,_tmp75D,({struct _tuple21 _tmp62E;_tmp62E.f1=_tmp45A,_tmp62E.f2=ropt;_tmp62E;}));});
# 2343
struct _tuple21 _tmp44B=_tmp44A;union Cyc_CfFlowInfo_FlowInfo _tmp44D;void*_tmp44C;_LL13: _tmp44D=_tmp44B.f1;_tmp44C=_tmp44B.f2;_LL14:;
# 2347
_tmp45A=_tmp44D;new_rval=_tmp44C;}else{
# 2352
new_rval=_tmp448;}}else{
# 2355
new_rval=_tmp456;}
# 2357
{union Cyc_CfFlowInfo_FlowInfo _tmp44E=_tmp45A;struct Cyc_Dict_Dict _tmp452;struct Cyc_List_List*_tmp451;if((_tmp44E.ReachableFL).tag == 2){_LL16: _tmp452=((_tmp44E.ReachableFL).val).f1;_tmp451=((_tmp44E.ReachableFL).val).f2;_LL17:
# 2359
 flow=({struct Cyc_Dict_Dict _tmp75F=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp452,_tmp458,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp75F,_tmp451);});
# 2363
goto _LL15;}else{_LL18: _LL19:
# 2365
({void*_tmp44F=0U;({struct _fat_ptr _tmp760=({const char*_tmp450="unconsume_params: joined flow became bot!";_tag_fat(_tmp450,sizeof(char),42U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp760,_tag_fat(_tmp44F,sizeof(void*),0U));});});}_LL15:;}
# 2367
goto _LLD;}else{
# 2369
goto _LL11;}
goto _LLD;}}else{_LL10: _LL11:
# 2376
 if(ropt != 0 && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp455=({struct Cyc_String_pa_PrintArg_struct _tmp62F;_tmp62F.tag=0U,({
struct _fat_ptr _tmp761=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(unconsume_exp));_tmp62F.f1=_tmp761;});_tmp62F;});void*_tmp453[1U];_tmp453[0]=& _tmp455;({unsigned _tmp763=loc;struct _fat_ptr _tmp762=({const char*_tmp454="aliased expression %s was overwritten";_tag_fat(_tmp454,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp763,_tmp762,_tag_fat(_tmp453,sizeof(void*),1U));});});
# 2393 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2395
goto _LL8;}}else{_LLB: _LLC:
# 2401
 goto _LL8;}_LL8:;}
# 2403
goto _LL0;}}_LL0:;}
# 2405
return flow;}
# 2418 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2424
{union Cyc_CfFlowInfo_FlowInfo _tmp45C=flow;if((_tmp45C.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2428
int _tmp45D=unconsume_exps != 0;
struct Cyc_List_List*_tmp45E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(consumed_vals);
struct Cyc_List_List*_tmp45F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(unconsume_exps);
{struct Cyc_List_List*_tmp460=_tmp45E;for(0;_tmp460 != 0;(
_tmp460=_tmp460->tl,
_tmp45F != 0?_tmp45F=_tmp45F->tl: 0)){
# 2437
struct Cyc_Absyn_Vardecl*_tmp461=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp460->hd);
struct _tuple15 _tmp462=
is_region_open?({struct _tuple15 _tmp630;_tmp630.f1=1,_tmp630.f2=0;_tmp630;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp460->hd,_tmp45D,_tmp461,flow,loc);
# 2438
struct _tuple15 _tmp463=_tmp462;int _tmp465;void*_tmp464;_LL6: _tmp465=_tmp463.f1;_tmp464=_tmp463.f2;_LL7:;
# 2444
if(_tmp45D)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45F))->hd,_tmp461,_tmp465,_tmp464,flow,loc);}}
# 2447
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple33{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2451
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned exp_loc){
# 2453
struct Cyc_CfFlowInfo_FlowEnv*_tmp466=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp467=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp468=_tmp467;struct Cyc_Core_Opt*_tmp47C;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Stmt*_tmp47A;unsigned _tmp479;_LL1: _tmp47C=_tmp468->pat_vars;_tmp47B=_tmp468->where_clause;_tmp47A=_tmp468->body;_tmp479=_tmp468->loc;_LL2:;{
struct _tuple1 _tmp469=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp47C))->v);struct _tuple1 _tmp46A=_tmp469;struct Cyc_List_List*_tmp478;struct Cyc_List_List*_tmp477;_LL4: _tmp478=_tmp46A.f1;_tmp477=_tmp46A.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp47C->v,_tmp478 != 0,_tmp479,exp_loc);
# 2461
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp46B=env->unique_pat_vars;
# 2464
if(Cyc_Tcpat_has_vars(_tmp47C))
({struct Cyc_List_List*_tmp765=({struct Cyc_List_List*_tmp46D=_cycalloc(sizeof(*_tmp46D));
({struct _tuple33*_tmp764=({struct _tuple33*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->f1=0,_tmp46C->f2=_tmp47A,_tmp46C->f3=_tmp478,_tmp46C->f4=_tmp477;_tmp46C;});_tmp46D->hd=_tmp764;}),_tmp46D->tl=_tmp46B;_tmp46D;});
# 2465
env->unique_pat_vars=_tmp765;});
# 2468
if(_tmp47B != 0){
struct Cyc_Absyn_Exp*wexp=_tmp47B;
struct _tuple24 _tmp46E=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple24 _tmp46F=_tmp46E;union Cyc_CfFlowInfo_FlowInfo _tmp471;union Cyc_CfFlowInfo_FlowInfo _tmp470;_LL7: _tmp471=_tmp46F.f1;_tmp470=_tmp46F.f2;_LL8:;
inflow=_tmp470;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp471,_tmp47A,0);}else{
# 2474
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp47A,0);}
# 2476
env->unique_pat_vars=_tmp46B;{
union Cyc_CfFlowInfo_FlowInfo _tmp472=clause_outflow;if((_tmp472.BottomFL).tag == 1){_LLA: _LLB:
 goto _LL9;}else{_LLC: _LLD:
# 2481
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp478,_tmp477,0,clause_outflow,_tmp479);
# 2483
if(scs->tl == 0)
return clause_outflow;else{
# 2488
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp473=0U;({unsigned _tmp767=_tmp47A->loc;struct _fat_ptr _tmp766=({const char*_tmp474="switch clause may implicitly fallthru";_tag_fat(_tmp474,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp767,_tmp766,_tag_fat(_tmp473,sizeof(void*),0U));});});else{
# 2491
({void*_tmp475=0U;({unsigned _tmp769=_tmp47A->loc;struct _fat_ptr _tmp768=({const char*_tmp476="switch clause may implicitly fallthru";_tag_fat(_tmp476,sizeof(char),38U);});Cyc_Warn_warn(_tmp769,_tmp768,_tag_fat(_tmp475,sizeof(void*),0U));});});}
# 2493
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2495
goto _LL9;}_LL9:;};};};}
# 2498
return Cyc_CfFlowInfo_BottomFL();}struct _tuple34{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned f3;};
# 2503
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple34*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2505
struct _tuple34*_tmp47D=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp488;struct Cyc_Dict_Dict _tmp487;unsigned _tmp486;_LL1: _tmp488=_tmp47D->f1;_tmp487=_tmp47D->f2;_tmp486=_tmp47D->f3;_LL2:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2508
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp632;_tmp632.tag=0U,_tmp632.f1=vd;_tmp632;});
struct _tuple20 _tmp47E=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp76A=_tmp487;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp76A,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp485=_cycalloc(sizeof(*_tmp485));*_tmp485=vdroot;_tmp485;}));}));struct _tuple20 _tmp47F=_tmp47E;void*_tmp484;_LL4: _tmp484=_tmp47F.f1;_LL5:;{
void*_tmp480=_tmp484;switch(*((int*)_tmp480)){case 7U: _LL7: _LL8:
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp480)->f1 == Cyc_CfFlowInfo_NoneIL){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
# 2515
({struct Cyc_String_pa_PrintArg_struct _tmp483=({struct Cyc_String_pa_PrintArg_struct _tmp631;_tmp631.tag=0U,({struct _fat_ptr _tmp76B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp631.f1=_tmp76B;});_tmp631;});void*_tmp481[1U];_tmp481[0]=& _tmp483;({unsigned _tmp76D=_tmp486;struct _fat_ptr _tmp76C=({const char*_tmp482="unique pointers may still exist after variable %s goes out of scope";_tag_fat(_tmp482,sizeof(char),68U);});Cyc_Warn_warn(_tmp76D,_tmp76C,_tag_fat(_tmp481,sizeof(void*),1U));});});
# 2517
goto _LL6;}_LL6:;};}}
# 2522
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp489=inflow;struct Cyc_Dict_Dict _tmp495;if((_tmp489.ReachableFL).tag == 2){_LL1: _tmp495=((_tmp489.ReachableFL).val).f1;_LL2: {
# 2525
struct _tuple34 _tmp48A=({struct _tuple34 _tmp633;_tmp633.f1=env,_tmp633.f2=_tmp495,_tmp633.f3=decl->loc;_tmp633;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp48B=env->fenv;
{void*_tmp48C=decl->r;void*_tmp48D=_tmp48C;struct Cyc_List_List*_tmp494;struct Cyc_List_List*_tmp493;struct Cyc_Absyn_Vardecl*_tmp492;switch(*((int*)_tmp48D)){case 0U: _LL6: _tmp492=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp48D)->f1;_LL7:
# 2529
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp48A,_tmp492);
goto _LL5;case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp48D)->f2 != 0){_LL8: _tmp493=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp48D)->f2)->v;_LL9: {
# 2532
struct _tuple1 _tmp48E=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp493);struct _tuple1 _tmp48F=_tmp48E;struct Cyc_List_List*_tmp491;_LLF: _tmp491=_tmp48F.f1;_LL10:;{
struct Cyc_List_List*_tmp490=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)(_tmp491);
_tmp494=_tmp490;goto _LLB;};}}else{goto _LLC;}case 3U: _LLA: _tmp494=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp48D)->f1;_LLB:
# 2536
((void(*)(void(*f)(struct _tuple34*,struct Cyc_Absyn_Vardecl*),struct _tuple34*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp48A,_tmp494);
goto _LL5;default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 2540
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2543
return;}
# 2549
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2553
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple33*_tmp496=(struct _tuple33*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple33*_tmp497=_tmp496;int _tmp49B;struct Cyc_Absyn_Stmt*_tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_List_List*_tmp498;_LL1: _tmp49B=_tmp497->f1;_tmp49A=_tmp497->f2;_tmp499=_tmp497->f3;_tmp498=_tmp497->f4;_LL2:;
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp499,_tmp498,_tmp49B,inflow,dest->loc);}}
# 2558
return inflow;}
# 2564
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple22*rval_opt){
# 2566
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple25 _tmp49C=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple25 _tmp49D=_tmp49C;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp537;union Cyc_CfFlowInfo_FlowInfo*_tmp536;_LL1: _tmp537=_tmp49D.f1;_tmp536=_tmp49D.f2;_LL2:;
inflow=*_tmp536;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp49E=env->fenv;
# 2573
void*_tmp49F=s->r;void*_tmp4A0=_tmp49F;struct Cyc_Absyn_Stmt*_tmp535;struct Cyc_Absyn_Decl*_tmp534;struct Cyc_Absyn_Stmt*_tmp533;struct Cyc_Absyn_Vardecl*_tmp532;struct Cyc_Absyn_Exp*_tmp531;unsigned _tmp530;struct Cyc_Absyn_Stmt*_tmp52F;struct Cyc_List_List*_tmp52E;struct Cyc_Absyn_Exp*_tmp52D;unsigned _tmp52C;struct Cyc_Absyn_Stmt*_tmp52B;struct Cyc_Absyn_Stmt*_tmp52A;struct Cyc_List_List*_tmp529;void*_tmp528;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_List_List*_tmp526;void*_tmp525;struct Cyc_List_List*_tmp524;struct Cyc_Absyn_Switch_clause*_tmp523;struct Cyc_Absyn_Exp*_tmp522;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Stmt*_tmp520;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Stmt*_tmp51E;struct Cyc_Absyn_Stmt*_tmp51D;struct Cyc_Absyn_Stmt*_tmp51C;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Stmt*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Stmt*_tmp518;struct Cyc_Absyn_Stmt*_tmp517;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Stmt*_tmp515;struct Cyc_Absyn_Stmt*_tmp514;struct Cyc_Absyn_Stmt*_tmp513;struct Cyc_Absyn_Stmt*_tmp512;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp510;switch(*((int*)_tmp4A0)){case 0U: _LL4: _LL5:
 return inflow;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1 == 0){_LL6: _LL7:
# 2577
 if(env->noreturn)
({void*_tmp4A1=0U;({unsigned _tmp76F=s->loc;struct _fat_ptr _tmp76E=({const char*_tmp4A2="`noreturn' function might return";_tag_fat(_tmp4A2,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp76F,_tmp76E,_tag_fat(_tmp4A1,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL8: _tmp510=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_LL9:
# 2583
 if(env->noreturn)
({void*_tmp4A3=0U;({unsigned _tmp771=s->loc;struct _fat_ptr _tmp770=({const char*_tmp4A4="`noreturn' function might return";_tag_fat(_tmp4A4,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp771,_tmp770,_tag_fat(_tmp4A3,sizeof(void*),0U));});});{
struct _tuple21 _tmp4A5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp510));struct _tuple21 _tmp4A6=_tmp4A5;union Cyc_CfFlowInfo_FlowInfo _tmp4A8;void*_tmp4A7;_LL2D: _tmp4A8=_tmp4A6.f1;_tmp4A7=_tmp4A6.f2;_LL2E:;
_tmp4A8=Cyc_NewControlFlow_use_Rval(env,_tmp510->loc,_tmp4A8,_tmp4A7);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp4A8);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp4A8,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1U: _LLA: _tmp511=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_LLB: {
# 2592
struct _tuple22*_tmp4A9=rval_opt;void**_tmp4AF;int _tmp4AE;if(_tmp4A9 != 0){_LL30: _tmp4AF=(void**)& _tmp4A9->f1;_tmp4AE=_tmp4A9->f2;_LL31: {
# 2594
struct _tuple21 _tmp4AA=Cyc_NewControlFlow_anal_Rexp(env,_tmp4AE,inflow,_tmp511);struct _tuple21 _tmp4AB=_tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4AD;void*_tmp4AC;_LL35: _tmp4AD=_tmp4AB.f1;_tmp4AC=_tmp4AB.f2;_LL36:;
*_tmp4AF=_tmp4AC;
return _tmp4AD;}}else{_LL32: _LL33:
# 2598
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp511)).f1;}_LL2F:;}case 2U: _LLC: _tmp513=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp512=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LLD:
# 2602
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp774=env;union Cyc_CfFlowInfo_FlowInfo _tmp773=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp513,0);struct Cyc_Absyn_Stmt*_tmp772=_tmp512;Cyc_NewControlFlow_anal_stmt(_tmp774,_tmp773,_tmp772,rval_opt);});case 4U: _LLE: _tmp516=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp515=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_tmp514=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3;_LLF: {
# 2605
struct _tuple24 _tmp4B0=Cyc_NewControlFlow_anal_test(env,inflow,_tmp516);struct _tuple24 _tmp4B1=_tmp4B0;union Cyc_CfFlowInfo_FlowInfo _tmp4B5;union Cyc_CfFlowInfo_FlowInfo _tmp4B4;_LL38: _tmp4B5=_tmp4B1.f1;_tmp4B4=_tmp4B1.f2;_LL39:;{
# 2612
union Cyc_CfFlowInfo_FlowInfo _tmp4B2=Cyc_NewControlFlow_anal_stmt(env,_tmp4B4,_tmp514,0);
union Cyc_CfFlowInfo_FlowInfo _tmp4B3=Cyc_NewControlFlow_anal_stmt(env,_tmp4B5,_tmp515,0);
return Cyc_CfFlowInfo_join_flow(_tmp49E,_tmp4B3,_tmp4B2);};}case 5U: _LL10: _tmp519=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1).f1;_tmp518=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1).f2;_tmp517=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL11: {
# 2620
struct _tuple25 _tmp4B6=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp518);struct _tuple25 _tmp4B7=_tmp4B6;union Cyc_CfFlowInfo_FlowInfo*_tmp4BE;_LL3B: _tmp4BE=_tmp4B7.f2;_LL3C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B8=*_tmp4BE;
struct _tuple24 _tmp4B9=Cyc_NewControlFlow_anal_test(env,_tmp4B8,_tmp519);struct _tuple24 _tmp4BA=_tmp4B9;union Cyc_CfFlowInfo_FlowInfo _tmp4BD;union Cyc_CfFlowInfo_FlowInfo _tmp4BC;_LL3E: _tmp4BD=_tmp4BA.f1;_tmp4BC=_tmp4BA.f2;_LL3F:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4BB=Cyc_NewControlFlow_anal_stmt(env,_tmp4BD,_tmp517,0);
Cyc_NewControlFlow_update_flow(env,_tmp518,_tmp4BB);
return _tmp4BC;};};}case 14U: _LL12: _tmp51C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp51B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2).f1;_tmp51A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2).f2;_LL13: {
# 2630
union Cyc_CfFlowInfo_FlowInfo _tmp4BF=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp51C,0);
struct _tuple25 _tmp4C0=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4BF,_tmp51A);struct _tuple25 _tmp4C1=_tmp4C0;union Cyc_CfFlowInfo_FlowInfo*_tmp4C7;_LL41: _tmp4C7=_tmp4C1.f2;_LL42:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4C2=*_tmp4C7;
struct _tuple24 _tmp4C3=Cyc_NewControlFlow_anal_test(env,_tmp4C2,_tmp51B);struct _tuple24 _tmp4C4=_tmp4C3;union Cyc_CfFlowInfo_FlowInfo _tmp4C6;union Cyc_CfFlowInfo_FlowInfo _tmp4C5;_LL44: _tmp4C6=_tmp4C4.f1;_tmp4C5=_tmp4C4.f2;_LL45:;
Cyc_NewControlFlow_update_flow(env,_tmp51C,_tmp4C6);
return _tmp4C5;};}case 9U: _LL14: _tmp522=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp521=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2).f1;_tmp520=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2).f2;_tmp51F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3).f1;_tmp51E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3).f2;_tmp51D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f4;_LL15: {
# 2639
union Cyc_CfFlowInfo_FlowInfo _tmp4C8=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp522)).f1;
struct _tuple25 _tmp4C9=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4C8,_tmp520);struct _tuple25 _tmp4CA=_tmp4C9;union Cyc_CfFlowInfo_FlowInfo*_tmp4D6;_LL47: _tmp4D6=_tmp4CA.f2;_LL48:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4CB=*_tmp4D6;
struct _tuple24 _tmp4CC=Cyc_NewControlFlow_anal_test(env,_tmp4CB,_tmp521);struct _tuple24 _tmp4CD=_tmp4CC;union Cyc_CfFlowInfo_FlowInfo _tmp4D5;union Cyc_CfFlowInfo_FlowInfo _tmp4D4;_LL4A: _tmp4D5=_tmp4CD.f1;_tmp4D4=_tmp4CD.f2;_LL4B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4CE=Cyc_NewControlFlow_anal_stmt(env,_tmp4D5,_tmp51D,0);
struct _tuple25 _tmp4CF=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4CE,_tmp51E);struct _tuple25 _tmp4D0=_tmp4CF;union Cyc_CfFlowInfo_FlowInfo*_tmp4D3;_LL4D: _tmp4D3=_tmp4D0.f2;_LL4E:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4D1=*_tmp4D3;
union Cyc_CfFlowInfo_FlowInfo _tmp4D2=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp4D1,_tmp51F)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp520,_tmp4D2);
return _tmp4D4;};};};}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2 != 0){_LL16: _tmp524=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp523=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL17: {
# 2651
struct _tuple27 _tmp4D7=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp524,1,1);struct _tuple27 _tmp4D8=_tmp4D7;union Cyc_CfFlowInfo_FlowInfo _tmp4DF;struct Cyc_List_List*_tmp4DE;_LL50: _tmp4DF=_tmp4D8.f1;_tmp4DE=_tmp4D8.f2;_LL51:;
# 2653
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp523->body);{
# 2655
struct Cyc_List_List*_tmp4D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp523->pat_vars))->v)).f1);
_tmp4DF=Cyc_NewControlFlow_add_vars(_tmp49E,_tmp4DF,_tmp4D9,_tmp49E->unknown_all,s->loc,0);
# 2658
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp523->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple28*_tmp4DA=(struct _tuple28*)x->hd;struct _tuple28*_tmp4DB=_tmp4DA;struct Cyc_Absyn_Vardecl**_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;_LL53: _tmp4DD=_tmp4DB->f1;_tmp4DC=_tmp4DB->f2;_LL54:;
if(_tmp4DD != 0){
_tmp4DF=({struct Cyc_CfFlowInfo_FlowEnv*_tmp77A=_tmp49E;struct Cyc_NewControlFlow_AnalEnv*_tmp779=env;union Cyc_CfFlowInfo_FlowInfo _tmp778=_tmp4DF;struct Cyc_Absyn_Vardecl*_tmp777=*_tmp4DD;struct Cyc_Absyn_Exp*_tmp776=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp524))->hd;void*_tmp775=(void*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp77A,_tmp779,_tmp778,_tmp777,_tmp776,_tmp775,s->loc);});
_tmp4DE=_tmp4DE->tl;
_tmp524=_tmp524->tl;}}}
# 2666
Cyc_NewControlFlow_update_flow(env,_tmp523->body,_tmp4DF);
return Cyc_CfFlowInfo_BottomFL();};}}else{_LL2A: _LL2B:
# 2830
({void*_tmp50E=0U;({struct _fat_ptr _tmp77B=({const char*_tmp50F="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_fat(_tmp50F,sizeof(char),54U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp77B,_tag_fat(_tmp50E,sizeof(void*),0U));});});}case 6U: _LL18: _LL19:
# 2672
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp4E0=inflow;if((_tmp4E0.ReachableFL).tag == 2){_LL56: _LL57:
# 2675
 if(!Cyc_Tcutil_is_void_type(env->return_type)){
if(Cyc_Tcutil_is_any_float_type(env->return_type)||
 Cyc_Tcutil_is_any_int_type(env->return_type))
({void*_tmp4E1=0U;({unsigned _tmp77D=s->loc;struct _fat_ptr _tmp77C=({const char*_tmp4E2="break may cause function not to return a value";_tag_fat(_tmp4E2,sizeof(char),47U);});Cyc_Warn_warn(_tmp77D,_tmp77C,_tag_fat(_tmp4E1,sizeof(void*),0U));});});else{
# 2680
({void*_tmp4E3=0U;({unsigned _tmp77F=s->loc;struct _fat_ptr _tmp77E=({const char*_tmp4E4="break may cause function not to return a value";_tag_fat(_tmp4E4,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp77F,_tmp77E,_tag_fat(_tmp4E3,sizeof(void*),0U));});});}}
# 2682
goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 2685
if(env->noreturn)
({void*_tmp4E5=0U;({unsigned _tmp781=s->loc;struct _fat_ptr _tmp780=({const char*_tmp4E6="`noreturn' function might return";_tag_fat(_tmp4E6,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp781,_tmp780,_tag_fat(_tmp4E5,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2691
goto _LL1B;case 7U: _LL1A: _LL1B:
 goto _LL1D;case 8U: _LL1C: _LL1D: {
# 2695
struct Cyc_Absyn_Stmt*_tmp4E7=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp4E7 == 0)
({void*_tmp4E8=0U;({unsigned _tmp783=s->loc;struct _fat_ptr _tmp782=({const char*_tmp4E9="jump has no target (should be impossible)";_tag_fat(_tmp4E9,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp783,_tmp782,_tag_fat(_tmp4E8,sizeof(void*),0U));});});
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp4E7));
# 2700
Cyc_NewControlFlow_update_flow(env,_tmp4E7,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10U: _LL1E: _tmp527=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp526=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_tmp525=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3;_LL1F:
# 2706
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp526,_tmp527->loc);case 15U: _LL20: _tmp52A=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp529=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_tmp528=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f3;_LL21: {
# 2711
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp52A,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2719
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2722
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp529,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp4EA=scs_outflow;if((_tmp4EA.BottomFL).tag == 1){_LL5B: _LL5C:
 goto _LL5A;}else{_LL5D: _LL5E:
({void*_tmp4EB=0U;({unsigned _tmp785=s->loc;struct _fat_ptr _tmp784=({const char*_tmp4EC="last catch clause may implicitly fallthru";_tag_fat(_tmp4EC,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp785,_tmp784,_tag_fat(_tmp4EB,sizeof(void*),0U));});});}_LL5A:;}
# 2728
outflow=s1_outflow;
# 2730
return outflow;};};}case 12U: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)){case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f2 != 0){_LL22: _tmp52E=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f2)->v;_tmp52D=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f3;_tmp52C=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->loc;_tmp52B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL23: {
# 2740
struct _tuple1 _tmp4ED=Cyc_NewControlFlow_get_unconsume_pat_vars(_tmp52E);struct _tuple1 _tmp4EE=_tmp4ED;struct Cyc_List_List*_tmp4F3;struct Cyc_List_List*_tmp4F2;_LL60: _tmp4F3=_tmp4EE.f1;_tmp4F2=_tmp4EE.f2;_LL61:;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp49E,env,inflow,_tmp52E,_tmp4F3 != 0,_tmp52C,_tmp52D->loc);{
struct Cyc_List_List*_tmp4EF=env->unique_pat_vars;
# 2744
({struct Cyc_List_List*_tmp787=({struct Cyc_List_List*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));
({struct _tuple33*_tmp786=({struct _tuple33*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->f1=0,_tmp4F0->f2=s,_tmp4F0->f3=_tmp4F3,_tmp4F0->f4=_tmp4F2;_tmp4F0;});_tmp4F1->hd=_tmp786;}),_tmp4F1->tl=_tmp4EF;_tmp4F1;});
# 2744
env->unique_pat_vars=_tmp787;});
# 2749
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp52B,rval_opt);
env->unique_pat_vars=_tmp4EF;
# 2753
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp4F3,_tmp4F2,0,inflow,_tmp52C);
# 2757
return inflow;};}}else{goto _LL26;}case 4U: _LL24: _tmp532=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f2;_tmp531=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->r)->f3;_tmp530=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1)->loc;_tmp52F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;if(_tmp531 != 0){_LL25: {
# 2769
struct Cyc_List_List l=({struct Cyc_List_List _tmp635;_tmp635.hd=_tmp531,_tmp635.tl=0;_tmp635;});
union Cyc_CfFlowInfo_FlowInfo _tmp4F4=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple23 _tmp4F5=Cyc_NewControlFlow_anal_Lexp(env,_tmp4F4,0,0,_tmp531);struct _tuple23 _tmp4F6=_tmp4F5;union Cyc_CfFlowInfo_AbsLVal _tmp50D;_LL63: _tmp50D=_tmp4F6.f2;_LL64:;{
struct _tuple21 _tmp4F7=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp4F4,_tmp531);struct _tuple21 _tmp4F8=_tmp4F7;union Cyc_CfFlowInfo_FlowInfo _tmp50C;_LL66: _tmp50C=_tmp4F8.f1;_LL67:;{
struct Cyc_List_List*_tmp4F9=0;
struct Cyc_List_List*_tmp4FA=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp4FB=_tmp50C;struct Cyc_Dict_Dict _tmp507;struct Cyc_List_List*_tmp506;if((_tmp4FB.ReachableFL).tag == 2){_LL69: _tmp507=((_tmp4FB.ReachableFL).val).f1;_tmp506=((_tmp4FB.ReachableFL).val).f2;_LL6A:
# 2777
{union Cyc_CfFlowInfo_AbsLVal _tmp4FC=_tmp50D;struct Cyc_CfFlowInfo_Place*_tmp505;if((_tmp4FC.PlaceL).tag == 1){_LL6E: _tmp505=(_tmp4FC.PlaceL).val;_LL6F: {
# 2781
void*_tmp4FD=Cyc_CfFlowInfo_lookup_place(_tmp507,_tmp505);
_tmp4FD=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->tag=8U,_tmp4FE->f1=_tmp532,_tmp4FE->f2=_tmp4FD;_tmp4FE;});
_tmp507=Cyc_CfFlowInfo_assign_place(_tmp49E,_tmp531->loc,_tmp507,_tmp505,_tmp4FD);
_tmp50C=Cyc_CfFlowInfo_ReachableFL(_tmp507,_tmp506);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4FF=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->tag=0U,_tmp504->f1=_tmp532;_tmp504;});
struct Cyc_CfFlowInfo_Place*_tmp500=({struct Cyc_CfFlowInfo_Place*_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503->root=(void*)_tmp4FF,_tmp503->path=0;_tmp503;});
_tmp4F9=({struct Cyc_List_List*_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->hd=_tmp500,_tmp501->tl=_tmp4F9;_tmp501;});
_tmp4FA=({struct Cyc_List_List*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->hd=_tmp531,_tmp502->tl=_tmp4FA;_tmp502;});
goto _LL6D;};}}else{_LL70: _LL71:
# 2796
 goto _LL6D;}_LL6D:;}
# 2798
goto _LL68;}else{_LL6B: _LL6C:
# 2800
 goto _LL68;}_LL68:;}{
# 2803
struct Cyc_List_List _tmp508=({struct Cyc_List_List _tmp634;_tmp634.hd=_tmp532,_tmp634.tl=0;_tmp634;});
_tmp50C=Cyc_NewControlFlow_add_vars(_tmp49E,_tmp50C,& _tmp508,_tmp49E->unknown_all,_tmp530,0);{
# 2807
struct Cyc_List_List*_tmp509=env->unique_pat_vars;
({struct Cyc_List_List*_tmp789=({struct Cyc_List_List*_tmp50B=_cycalloc(sizeof(*_tmp50B));({struct _tuple33*_tmp788=({struct _tuple33*_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A->f1=1,_tmp50A->f2=s,_tmp50A->f3=_tmp4F9,_tmp50A->f4=_tmp4FA;_tmp50A;});_tmp50B->hd=_tmp788;}),_tmp50B->tl=_tmp509;_tmp50B;});env->unique_pat_vars=_tmp789;});
# 2812
_tmp50C=Cyc_NewControlFlow_anal_stmt(env,_tmp50C,_tmp52F,rval_opt);
env->unique_pat_vars=_tmp509;
# 2816
_tmp50C=Cyc_NewControlFlow_unconsume_params(env,_tmp4F9,_tmp4FA,1,_tmp50C,_tmp530);
# 2820
return _tmp50C;};};};};}}else{goto _LL26;}default: _LL26: _tmp534=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f1;_tmp533=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL27:
# 2823
 outflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp78C=env;union Cyc_CfFlowInfo_FlowInfo _tmp78B=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp534);struct Cyc_Absyn_Stmt*_tmp78A=_tmp533;Cyc_NewControlFlow_anal_stmt(_tmp78C,_tmp78B,_tmp78A,rval_opt);});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp534);
return outflow;}default: _LL28: _tmp535=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp4A0)->f2;_LL29:
# 2828
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp535,rval_opt);}_LL3:;};}
# 2834
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2838
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp538=env->fenv;
void*_tmp539=decl->r;void*_tmp53A=_tmp539;struct Cyc_Absyn_Tvar*_tmp54D;struct Cyc_Absyn_Vardecl*_tmp54C;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Fndecl*_tmp54A;struct Cyc_List_List*_tmp549;struct Cyc_Absyn_Vardecl*_tmp548;switch(*((int*)_tmp53A)){case 0U: _LL1: _tmp548=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp53A)->f1;_LL2:
# 2846
 if((int)_tmp548->sc == (int)Cyc_Absyn_Static)
return inflow;{
# 2851
struct Cyc_List_List _tmp53B=({struct Cyc_List_List _tmp636;_tmp636.hd=_tmp548,_tmp636.tl=0;_tmp636;});
inflow=Cyc_NewControlFlow_add_vars(_tmp538,inflow,& _tmp53B,_tmp538->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp53C=_tmp548->initializer;
if(_tmp53C == 0)
return inflow;{
struct _tuple21 _tmp53D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp53C);struct _tuple21 _tmp53E=_tmp53D;union Cyc_CfFlowInfo_FlowInfo _tmp540;void*_tmp53F;_LLC: _tmp540=_tmp53E.f1;_tmp53F=_tmp53E.f2;_LLD:;
return Cyc_NewControlFlow_do_initialize_var(_tmp538,env,_tmp540,_tmp548,_tmp53C,_tmp53F,decl->loc);};};};case 3U: _LL3: _tmp549=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp53A)->f1;_LL4:
# 2860
 return Cyc_NewControlFlow_add_vars(_tmp538,inflow,_tmp549,_tmp538->unknown_none,decl->loc,0);case 1U: _LL5: _tmp54A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp53A)->f1;_LL6:
# 2863
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp538,inflow,_tmp54A);{
struct Cyc_Absyn_Vardecl*_tmp541=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp54A->fn_vardecl);
# 2868
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp790=_tmp538;union Cyc_CfFlowInfo_FlowInfo _tmp78F=inflow;struct Cyc_List_List*_tmp78E=({struct Cyc_List_List*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->hd=_tmp541,_tmp542->tl=0;_tmp542;});void*_tmp78D=_tmp538->unknown_all;Cyc_NewControlFlow_add_vars(_tmp790,_tmp78F,_tmp78E,_tmp78D,decl->loc,0);});};case 4U: _LL7: _tmp54D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53A)->f1;_tmp54C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53A)->f2;_tmp54B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53A)->f3;_LL8:
# 2871
 if(_tmp54B != 0)
({void*_tmp543=0U;({struct _fat_ptr _tmp791=({const char*_tmp544="found open expression in declaration!";_tag_fat(_tmp544,sizeof(char),38U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp791,_tag_fat(_tmp543,sizeof(void*),0U));});});{
struct Cyc_List_List _tmp545=({struct Cyc_List_List _tmp637;_tmp637.hd=_tmp54C,_tmp637.tl=0;_tmp637;});
return Cyc_NewControlFlow_add_vars(_tmp538,inflow,& _tmp545,_tmp538->unknown_all,decl->loc,0);};default: _LL9: _LLA:
# 2877
({void*_tmp546=0U;({struct _fat_ptr _tmp792=({const char*_tmp547="anal_decl: unexpected decl variant";_tag_fat(_tmp547,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp792,_tag_fat(_tmp546,sizeof(void*),0U));});});}_LL0:;}
# 2885
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp54E;_push_handler(& _tmp54E);{int _tmp550=0;if(setjmp(_tmp54E.handler))_tmp550=1;if(!_tmp550){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp795=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp794=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp793=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp795,_tmp794,_tmp793,fd);});}
# 2887
;_pop_handler();}else{void*_tmp54F=(void*)Cyc_Core_get_exn_thrown();void*_tmp551=_tmp54F;void*_tmp553;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp551)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 2893
 if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->tag=Cyc_Dict_Absent;_tmp552;}));}}else{_LL3: _tmp553=_tmp551;_LL4:(int)_rethrow(_tmp553);}_LL0:;}};}
# 2899
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2904
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp554=r;unsigned _tmp556;if((_tmp554.RParam).tag == 5){_LL1: _tmp556=(_tmp554.RParam).val;_LL2: {
# 2907
struct Cyc_Absyn_Vardecl*_tmp555=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp556);
if(!_tmp555->escapes)
return Cyc_Relations_RVar(_tmp555);
return r;}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 2918
static struct Cyc_List_List*Cyc_NewControlFlow_get_noconsume_params(struct Cyc_List_List*param_vardecls,struct Cyc_List_List*atts){
# 2920
struct _RegionHandle _tmp557=_new_region("r");struct _RegionHandle*r=& _tmp557;_push_region(r);
{int _tmp558=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(param_vardecls);
struct _fat_ptr _tmp559=({unsigned _tmp566=(unsigned)_tmp558;int*_tmp565=({struct _RegionHandle*_tmp796=r;_region_malloc(_tmp796,_check_times(_tmp566,sizeof(int)));});({{unsigned _tmp638=(unsigned)_tmp558;unsigned i;for(i=0;i < _tmp638;++ i){_tmp565[i]=0;}}0;});_tag_fat(_tmp565,sizeof(int),_tmp566);});
for(0;atts != 0;atts=atts->tl){
void*_tmp55A=(void*)atts->hd;void*_tmp55B=_tmp55A;int _tmp55D;int _tmp55C;switch(*((int*)_tmp55B)){case 22U: _LL1: _tmp55C=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp55B)->f1;_LL2:
 _tmp55D=_tmp55C;goto _LL4;case 21U: _LL3: _tmp55D=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp55B)->f1;_LL4:
*((int*)_check_fat_subscript(_tmp559,sizeof(int),_tmp55D - 1))=1;goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 2930
struct Cyc_List_List*noconsume_roots=0;
{int i=0;for(0;param_vardecls != 0;(param_vardecls=param_vardecls->tl,i ++)){
struct Cyc_Absyn_Vardecl*_tmp55E=(struct Cyc_Absyn_Vardecl*)param_vardecls->hd;
if(Cyc_Tcutil_is_noalias_pointer(_tmp55E->type,0)&& !(*((int*)_check_fat_subscript(_tmp559,sizeof(int),i)))){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp55F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=0U,_tmp563->f1=_tmp55E;_tmp563;});
struct Cyc_CfFlowInfo_Place*_tmp560=({struct Cyc_CfFlowInfo_Place*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->root=(void*)_tmp55F,_tmp562->path=0;_tmp562;});
noconsume_roots=({struct Cyc_List_List*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->hd=_tmp560,_tmp561->tl=noconsume_roots;_tmp561;});}}}{
# 2939
struct Cyc_List_List*_tmp564=noconsume_roots;_npop_handler(0U);return _tmp564;};};}
# 2921
;_pop_region(r);}
# 2942
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2946
unsigned _tmp567=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp567,1);{
# 2951
struct Cyc_List_List*param_roots=0;
struct _tuple19 _tmp568=({union Cyc_CfFlowInfo_FlowInfo _tmp590=inflow;if((_tmp590.ReachableFL).tag != 2)_throw_match();(_tmp590.ReachableFL).val;});struct _tuple19 _tmp569=_tmp568;struct Cyc_Dict_Dict _tmp58F;struct Cyc_List_List*_tmp58E;_LL1: _tmp58F=_tmp569.f1;_tmp58E=_tmp569.f2;_LL2:;{
# 2955
struct Cyc_List_List*_tmp56A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp56B=(fd->i).requires_relns;for(0;_tmp56B != 0;_tmp56B=_tmp56B->tl){
struct Cyc_Relations_Reln*_tmp56C=(struct Cyc_Relations_Reln*)_tmp56B->hd;
_tmp58E=({struct _RegionHandle*_tmp79A=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp799=Cyc_NewControlFlow_translate_rop(_tmp56A,_tmp56C->rop1);enum Cyc_Relations_Relation _tmp798=_tmp56C->relation;union Cyc_Relations_RelnOp _tmp797=
Cyc_NewControlFlow_translate_rop(_tmp56A,_tmp56C->rop2);
# 2958
Cyc_Relations_add_relation(_tmp79A,_tmp799,_tmp798,_tmp797,_tmp58E);});}}{
# 2964
struct Cyc_List_List*atts;
{void*_tmp56D=Cyc_Tcutil_compress((void*)_check_null(fd->cached_type));void*_tmp56E=_tmp56D;struct Cyc_List_List*_tmp571;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp56E)->tag == 5U){_LL4: _tmp571=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp56E)->f1).attributes;_LL5:
 atts=_tmp571;goto _LL3;}else{_LL6: _LL7:
({void*_tmp56F=0U;({struct _fat_ptr _tmp79B=({const char*_tmp570="check_fun: non-function type cached with fndecl_t";_tag_fat(_tmp570,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp79B,_tag_fat(_tmp56F,sizeof(void*),0U));});});}_LL3:;}{
# 2969
struct Cyc_List_List*noconsume_roots=Cyc_NewControlFlow_get_noconsume_params((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,atts);
# 2972
for(0;atts != 0;atts=atts->tl){
void*_tmp572=(void*)atts->hd;void*_tmp573=_tmp572;int _tmp585;int _tmp584;switch(*((int*)_tmp573)){case 21U: _LL9: _tmp584=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp573)->f1;_LLA: {
# 2975
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp574=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp584 - 1);
void*t=Cyc_Tcutil_compress(_tmp574->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp575=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp79E=fenv;void*_tmp79D=elttype;struct Cyc_Absyn_Exp*_tmp79C=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp79E,_tmp79D,_tmp79C,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp576=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->tag=2U,_tmp57B->f1=_tmp584,_tmp57B->f2=t;_tmp57B;});
struct Cyc_CfFlowInfo_Place*_tmp577=({struct Cyc_CfFlowInfo_Place*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->root=(void*)_tmp576,_tmp57A->path=0;_tmp57A;});
_tmp58F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp58F,(void*)_tmp576,_tmp575);
_tmp58F=({struct Cyc_Dict_Dict _tmp7A0=_tmp58F;void*_tmp79F=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->tag=0U,_tmp578->f1=_tmp574;_tmp578;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7A0,_tmp79F,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->tag=4U,_tmp579->f1=_tmp577;_tmp579;}));});
goto _LL8;}case 20U: _LLB: _tmp585=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp573)->f1;_LLC: {
# 2988
struct Cyc_Absyn_Vardecl*_tmp57C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp585 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp57C->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp57D=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->tag=2U,_tmp583->f1=_tmp585,_tmp583->f2=elttype;_tmp583;});
struct Cyc_CfFlowInfo_Place*_tmp57E=({struct Cyc_CfFlowInfo_Place*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582->root=(void*)_tmp57D,_tmp582->path=0;_tmp582;});
_tmp58F=({struct Cyc_Dict_Dict _tmp7A2=_tmp58F;void*_tmp7A1=(void*)_tmp57D;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7A2,_tmp7A1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});
_tmp58F=({struct Cyc_Dict_Dict _tmp7A4=_tmp58F;void*_tmp7A3=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->tag=0U,_tmp57F->f1=_tmp57C;_tmp57F;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7A4,_tmp7A3,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->tag=4U,_tmp580->f1=_tmp57E;_tmp580;}));});
param_roots=({struct Cyc_List_List*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->hd=_tmp57E,_tmp581->tl=param_roots;_tmp581;});
goto _LL8;}default: _LLD: _LLE:
 goto _LL8;}_LL8:;}
# 2999
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp58F,_tmp58E);{
# 3001
int noreturn=Cyc_Tcutil_is_noreturn_fn_type(Cyc_Tcutil_fndecl2type(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp58D=_cycalloc(sizeof(*_tmp58D));
_tmp58D->all_tables=tables,({
struct Cyc_Hashtable_Table*_tmp7A6=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp58D->succ_table=_tmp7A6;}),({
struct Cyc_Hashtable_Table*_tmp7A5=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp58D->pat_pop_table=_tmp7A5;}),_tmp58D->fenv=fenv,_tmp58D->iterate_again=1,_tmp58D->iteration_num=0,_tmp58D->in_try=0,_tmp58D->tryflow=inflow,_tmp58D->noreturn=noreturn,_tmp58D->return_type=(fd->i).ret_type,_tmp58D->unique_pat_vars=0,_tmp58D->param_roots=param_roots,_tmp58D->noconsume_params=noconsume_roots,_tmp58D->flow_table=flow_table,_tmp58D->return_relns=(fd->i).ensures_relns;_tmp58D;});
# 3011
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3017
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3020
union Cyc_CfFlowInfo_FlowInfo _tmp586=outflow;if((_tmp586.BottomFL).tag == 1){_LL10: _LL11:
 goto _LLF;}else{_LL12: _LL13:
# 3023
 Cyc_NewControlFlow_check_init_params(_tmp567,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp567);
# 3027
if(noreturn)
({void*_tmp587=0U;({unsigned _tmp7A8=_tmp567;struct _fat_ptr _tmp7A7=({const char*_tmp588="`noreturn' function might (implicitly) return";_tag_fat(_tmp588,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp7A8,_tmp7A7,_tag_fat(_tmp587,sizeof(void*),0U));});});else{
if(!Cyc_Tcutil_is_void_type((fd->i).ret_type)){
if(Cyc_Tcutil_is_any_float_type((fd->i).ret_type)||
 Cyc_Tcutil_is_any_int_type((fd->i).ret_type))
({void*_tmp589=0U;({unsigned _tmp7AA=_tmp567;struct _fat_ptr _tmp7A9=({const char*_tmp58A="function may not return a value";_tag_fat(_tmp58A,sizeof(char),32U);});Cyc_Warn_warn(_tmp7AA,_tmp7A9,_tag_fat(_tmp589,sizeof(void*),0U));});});else{
# 3034
({void*_tmp58B=0U;({unsigned _tmp7AC=_tmp567;struct _fat_ptr _tmp7AB=({const char*_tmp58C="function may not return a value";_tag_fat(_tmp58C,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp7AC,_tmp7AB,_tag_fat(_tmp58B,sizeof(void*),0U));});});}goto _LLF;}}
# 3036
goto _LLF;}_LLF:;};};};};};};}
# 3040
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp591=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp592=_tmp591;struct Cyc_List_List*_tmp596;struct Cyc_List_List*_tmp595;struct Cyc_List_List*_tmp594;struct Cyc_Absyn_Fndecl*_tmp593;switch(*((int*)_tmp592)){case 1U: _LL1: _tmp593=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp592)->f1;_LL2:
 Cyc_NewControlFlow_check_fun(tables,_tmp593);goto _LL0;case 11U: _LL3: _tmp594=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp592)->f1;_LL4:
# 3046
 _tmp595=_tmp594;goto _LL6;case 10U: _LL5: _tmp595=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp592)->f2;_LL6:
 _tmp596=_tmp595;goto _LL8;case 9U: _LL7: _tmp596=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp592)->f2;_LL8:
 Cyc_NewControlFlow_cf_check(tables,_tmp596);goto _LL0;case 12U: _LL9: _LLA:
 goto _LL0;default: _LLB: _LLC:
 goto _LL0;}_LL0:;};}}

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
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 147 "dict.h"
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 212
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 222
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 283
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 50 "string.h"
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 869 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 919
extern void*Cyc_Absyn_void_type;
# 1142
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1144
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 114
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 116
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 118
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 57
int Cyc_Tcutil_is_bits_only_type(void*);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
# 107
void*Cyc_Tcutil_compress(void*);
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 187
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 193
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 236
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 305
int Cyc_Tcutil_force_type2bool(int desired,void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 33 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};char Cyc_CfFlowInfo_IsZero[7U]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8U]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9U]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 139 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 156
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f);
# 158
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 161
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
# 163
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 168
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 171
int Cyc_CfFlowInfo_is_init_pointer(void*r);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
# 180
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
# 201 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 38 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 41
Cyc_CfFlowInfo_anal_error=1;
Cyc_Warn_verr(loc,fmt,ap);}
# 46
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
return({union Cyc_CfFlowInfo_AbsLVal _tmp220;(_tmp220.PlaceL).tag=1U,(_tmp220.PlaceL).val=x;_tmp220;});}
# 50
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
return({union Cyc_CfFlowInfo_AbsLVal _tmp221;(_tmp221.UnknownL).tag=2U,(_tmp221.UnknownL).val=0;_tmp221;});}
# 53
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){return({union Cyc_CfFlowInfo_FlowInfo _tmp222;(_tmp222.BottomFL).tag=1U,(_tmp222.BottomFL).val=0;_tmp222;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp223;(_tmp223.ReachableFL).tag=2U,((_tmp223.ReachableFL).val).f1=fd,((_tmp223.ReachableFL).val).f2=r;_tmp223;});}
# 58
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0U,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp8=_cycalloc(sizeof(*_tmp8));
({void*_tmp25D=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->tag=0U;_tmp0;});_tmp8->zero=_tmp25D;}),({
void*_tmp25C=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=1U;_tmp1;});_tmp8->notzeroall=_tmp25C;}),({
void*_tmp25B=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=2U,_tmp2->f1=Cyc_CfFlowInfo_NoneIL;_tmp2;});_tmp8->unknown_none=_tmp25B;}),({
void*_tmp25A=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=2U,_tmp3->f1=Cyc_CfFlowInfo_AllIL;_tmp3;});_tmp8->unknown_all=_tmp25A;}),({
void*_tmp259=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=3U,_tmp4->f1=Cyc_CfFlowInfo_NoneIL;_tmp4;});_tmp8->esc_none=_tmp259;}),({
void*_tmp258=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=3U,_tmp5->f1=Cyc_CfFlowInfo_AllIL;_tmp5;});_tmp8->esc_all=_tmp258;}),({
struct Cyc_Dict_Dict _tmp257=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(Cyc_CfFlowInfo_root_cmp);_tmp8->mt_flowdict=_tmp257;}),({
struct Cyc_CfFlowInfo_Place*_tmp256=({struct Cyc_CfFlowInfo_Place*_tmp7=_cycalloc(sizeof(*_tmp7));({void*_tmp255=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=1U,_tmp6->f1=& dummy_exp,_tmp6->f2=Cyc_Absyn_void_type;_tmp6;});_tmp7->root=_tmp255;}),_tmp7->path=0;_tmp7;});_tmp8->dummy_place=_tmp256;});_tmp8;});}
# 74
struct _fat_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp9=*place;struct Cyc_CfFlowInfo_Place _stmttmp0=_tmp9;struct Cyc_CfFlowInfo_Place _tmpA=_stmttmp0;struct Cyc_List_List*_tmpC;void*_tmpB;_LL1: _tmpB=_tmpA.root;_tmpC=_tmpA.path;_LL2: {void*root=_tmpB;struct Cyc_List_List*fields=_tmpC;
void*_tmpD=root;struct Cyc_Absyn_Vardecl*_tmpE;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->tag == 0U){_LL4: _tmpE=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->f1;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmpE;
# 78
if(fields == 0)
return Cyc_Absynpp_qvar2string(vd->name);else{
# 81
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmp224;_tmp224.tag=0U,({struct _fat_ptr _tmp25E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp224.f1=_tmp25E;});_tmp224;});void*_tmpF[1U];_tmpF[0]=& _tmp11;({struct _fat_ptr _tmp25F=({const char*_tmp10="reachable from %s";_tag_fat(_tmp10,sizeof(char),18U);});Cyc_aprintf(_tmp25F,_tag_fat(_tmpF,sizeof(void*),1U));});});}}}else{_LL6: _LL7:
({void*_tmp12=0U;({struct _fat_ptr _tmp260=({const char*_tmp13="error locations not for VarRoots";_tag_fat(_tmp13,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp260,_tag_fat(_tmp12,sizeof(void*),0U));});});}_LL3:;}}
# 87
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f){
int n=0;
for(0;(unsigned)fs;fs=fs->tl){
struct _fat_ptr*_tmp14=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;struct _fat_ptr*f2=_tmp14;
if(Cyc_strptrcmp(f2,f)== 0)return n;
n=n + 1;}
# 94
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp225;_tmp225.tag=0U,_tmp225.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp225;});void*_tmp15[1U];_tmp15[0]=& _tmp17;({struct _fat_ptr _tmp261=({const char*_tmp16="get_field_index_fs failed: %s";_tag_fat(_tmp16,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp261,_tag_fat(_tmp15,sizeof(void*),1U));});});}
# 97
int Cyc_CfFlowInfo_get_field_index(void*t,struct _fat_ptr*f){
void*_tmp18=Cyc_Tcutil_compress(t);void*_stmttmp1=_tmp18;void*_tmp19=_stmttmp1;struct Cyc_List_List*_tmp1A;union Cyc_Absyn_AggrInfo _tmp1B;switch(*((int*)_tmp19)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19)->f1)->tag == 20U){_LL1: _tmp1B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp1B;
# 100
struct Cyc_List_List*_tmp1C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(info))->impl))->fields;struct Cyc_List_List*fs=_tmp1C;
_tmp1A=fs;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp19)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A;
# 103
return Cyc_CfFlowInfo_get_field_index_fs(fs,f);}default: _LL5: _LL6:
# 105
({struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0U,({struct _fat_ptr _tmp262=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp226.f1=_tmp262;});_tmp226;});void*_tmp1D[1U];_tmp1D[0]=& _tmp1F;({struct _fat_ptr _tmp263=({const char*_tmp1E="get_field_index failed: %s";_tag_fat(_tmp1E,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp263,_tag_fat(_tmp1D,sizeof(void*),1U));});});}_LL0:;}struct _tuple12{void*f1;void*f2;};
# 109
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple12 _tmp20=({struct _tuple12 _tmp227;_tmp227.f1=r1,_tmp227.f2=r2;_tmp227;});struct _tuple12 _stmttmp2=_tmp20;struct _tuple12 _tmp21=_stmttmp2;int _tmp23;int _tmp22;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Vardecl*_tmp27;struct Cyc_Absyn_Vardecl*_tmp26;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f1)->tag == 0U){if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f2)->tag == 0U){_LL1: _tmp26=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp27=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd1=_tmp26;struct Cyc_Absyn_Vardecl*vd2=_tmp27;
return(int)vd1 - (int)vd2;}}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f1)->tag == 1U){if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f2)->tag == 1U){_LL7: _tmp24=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp25=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp24;struct Cyc_Absyn_Exp*e2=_tmp25;
return(int)e1 - (int)e2;}}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{_LLD: _tmp22=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp23=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;_LLE: {int i1=_tmp22;int i2=_tmp23;
return i1 - i2;}}}}}_LL0:;}}
# 123
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){
struct _tuple12 _tmp28=({struct _tuple12 _tmp228;_tmp228.f1=p1,_tmp228.f2=p2;_tmp228;});struct _tuple12 _stmttmp3=_tmp28;struct _tuple12 _tmp29=_stmttmp3;int _tmp2B;int _tmp2A;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp29.f1)->tag == 0U){if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp29.f2)->tag == 0U){_LL1: _tmp2A=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp29.f1)->f1;_tmp2B=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp29.f2)->f1;_LL2: {int i1=_tmp2A;int i2=_tmp2B;
# 126
if(i1 == i2)return 0;
if(i1 < i2)return - 1;else{
return 1;}}}else{_LL7: _LL8:
# 131
 return 1;}}else{if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_tmp29.f2)->tag == 1U){_LL3: _LL4:
# 129
 return 0;}else{_LL5: _LL6:
 return - 1;}}_LL0:;}
# 135
static int Cyc_CfFlowInfo_path_cmp(struct Cyc_List_List*path1,struct Cyc_List_List*path2){
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_pathcon_cmp,path1,path2);}
# 139
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp2C=*p1;struct Cyc_CfFlowInfo_Place _stmttmp4=_tmp2C;struct Cyc_CfFlowInfo_Place _tmp2D=_stmttmp4;struct Cyc_List_List*_tmp2F;void*_tmp2E;_LL1: _tmp2E=_tmp2D.root;_tmp2F=_tmp2D.path;_LL2: {void*root1=_tmp2E;struct Cyc_List_List*path1=_tmp2F;
struct Cyc_CfFlowInfo_Place _tmp30=*p2;struct Cyc_CfFlowInfo_Place _stmttmp5=_tmp30;struct Cyc_CfFlowInfo_Place _tmp31=_stmttmp5;struct Cyc_List_List*_tmp33;void*_tmp32;_LL4: _tmp32=_tmp31.root;_tmp33=_tmp31.path;_LL5: {void*root2=_tmp32;struct Cyc_List_List*path2=_tmp33;
int i=Cyc_CfFlowInfo_root_cmp(root1,root2);
if(i != 0)
return i;
return Cyc_CfFlowInfo_path_cmp(path1,path2);}}}}
# 160 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);
# 165
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 171
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
struct _fat_ptr d=({unsigned _tmp39=sz;void**_tmp38=_cycalloc(_check_times(_tmp39,sizeof(void*)));({{unsigned _tmp229=sz;unsigned i;for(i=0;i < _tmp229;++ i){_tmp38[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp38,sizeof(void*),_tmp39);});
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp34=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_stmttmp6=_tmp34;struct Cyc_Absyn_Aggrfield*_tmp35=_stmttmp6;void*_tmp37;struct _fat_ptr*_tmp36;_LL1: _tmp36=_tmp35->name;_tmp37=_tmp35->type;_LL2: {struct _fat_ptr*n=_tmp36;void*t2=_tmp37;
if(_get_fat_size(*n,sizeof(char))!= (unsigned)1)
({void*_tmp264=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,t2,leafval);*((void**)_check_fat_subscript(d,sizeof(void*),i))=_tmp264;});}}}
# 179
return d;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 183
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple13*env,struct Cyc_Absyn_Aggrfield*f){
# 187
struct _tuple13*_tmp3A=env;struct Cyc_List_List*_tmp3C;struct _RegionHandle*_tmp3B;_LL1: _tmp3B=_tmp3A->f1;_tmp3C=_tmp3A->f2;_LL2: {struct _RegionHandle*t=_tmp3B;struct Cyc_List_List*inst=_tmp3C;
void*_tmp3D=Cyc_Tcutil_rsubstitute(t,inst,f->type);void*new_typ=_tmp3D;
struct Cyc_Absyn_Exp*_tmp3E=f->requires_clause;struct Cyc_Absyn_Exp*r=_tmp3E;
struct Cyc_Absyn_Exp*_tmp3F=r == 0?0: Cyc_Tcutil_rsubsexp(t,inst,r);struct Cyc_Absyn_Exp*new_r=_tmp3F;
return({struct Cyc_Absyn_Aggrfield*_tmp40=_region_malloc(t,sizeof(*_tmp40));_tmp40->name=f->name,_tmp40->tq=f->tq,_tmp40->type=new_typ,_tmp40->width=f->width,_tmp40->attributes=f->attributes,_tmp40->requires_clause=new_r;_tmp40;});}}struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 198
static struct _fat_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 204
struct _RegionHandle _tmp41=_new_region("temp");struct _RegionHandle*temp=& _tmp41;_push_region(temp);
# 208
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp42=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp42;
void*_tmp43=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;void*t=_tmp43;
{struct Cyc_Absyn_Kind*_tmp44=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp7=_tmp44;struct Cyc_Absyn_Kind*_tmp45=_stmttmp7;switch(((struct Cyc_Absyn_Kind*)_tmp45)->kind){case Cyc_Absyn_RgnKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_EffKind: _LL3: _LL4:
 continue;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 217
inst=({struct Cyc_List_List*_tmp47=_region_malloc(temp,sizeof(*_tmp47));({struct _tuple14*_tmp265=({struct _tuple14*_tmp46=_region_malloc(temp,sizeof(*_tmp46));_tmp46->f1=tv,_tmp46->f2=t;_tmp46;});_tmp47->hd=_tmp265;}),_tmp47->tl=inst;_tmp47;});}
# 219
if(inst != 0){
struct _tuple13 _tmp48=({struct _tuple13 _tmp22A;_tmp22A.f1=temp,({struct Cyc_List_List*_tmp266=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);_tmp22A.f2=_tmp266;});_tmp22A;});struct _tuple13 env=_tmp48;
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& env,fs);
struct _fat_ptr _tmp49=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0U);return _tmp49;}else{
# 224
struct _fat_ptr _tmp4A=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0U);return _tmp4A;}}
# 208
;_pop_region(temp);}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 228
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 234
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 236
{void*_tmp4B=Cyc_Tcutil_compress(t);void*_stmttmp8=_tmp4B;void*_tmp4C=_stmttmp8;void*_tmp4D;void*_tmp4F;void*_tmp4E;struct Cyc_List_List*_tmp51;enum Cyc_Absyn_AggrKind _tmp50;struct Cyc_List_List*_tmp52;void*_tmp53;struct Cyc_List_List*_tmp55;union Cyc_Absyn_AggrInfo _tmp54;struct Cyc_Absyn_Datatypefield*_tmp56;switch(*((int*)_tmp4C)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f1)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f1)->f1).KnownDatatypefield).tag == 2){_LL1: _tmp56=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f1)->f1).KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypefield*tuf=_tmp56;
# 238
if(tuf->typs == 0)
return leafval;
_tmp52=tuf->typs;goto _LL4;}}else{goto _LLF;}case 20U: _LL5: _tmp54=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f1)->f1;_tmp55=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f2;_LL6: {union Cyc_Absyn_AggrInfo info=_tmp54;struct Cyc_List_List*targs=_tmp55;
# 250
struct Cyc_Absyn_Aggrdecl*_tmp5A=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5A;
if(ad->impl == 0)
goto _LL0;{
struct Cyc_List_List*_tmp5B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp5B;
if(targs == 0){_tmp50=ad->kind;_tmp51=fields;goto _LL8;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=6U,(_tmp5C->f1).is_union=(int)ad->kind == (int)Cyc_Absyn_UnionA,(_tmp5C->f1).fieldnum=- 1,({
struct _fat_ptr _tmp267=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,ad->tvs,targs,fields,(int)ad->kind == (int)Cyc_Absyn_UnionA,leafval);_tmp5C->f2=_tmp267;});_tmp5C;});}}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f2 != 0){_LLB: _tmp53=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f2)->hd;_LLC: {void*t=_tmp53;
# 273
return leafval;}}else{goto _LLF;}default: goto _LLF;}case 6U: _LL3: _tmp52=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4C)->f1;_LL4: {struct Cyc_List_List*tqts=_tmp52;
# 242
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
struct _fat_ptr d=({unsigned _tmp59=sz;void**_tmp58=_cycalloc(_check_times(_tmp59,sizeof(void*)));({{unsigned _tmp22B=sz;unsigned i;for(i=0;i < _tmp22B;++ i){_tmp58[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp58,sizeof(void*),_tmp59);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp268=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2,leafval);*((void**)_check_fat_subscript(d,sizeof(void*),i))=_tmp268;});
tqts=tqts->tl;}}
# 248
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=6U,(_tmp57->f1).is_union=0,(_tmp57->f1).fieldnum=- 1,_tmp57->f2=d;_tmp57;});}case 7U: _LL7: _tmp50=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f1;_tmp51=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f2;_LL8: {enum Cyc_Absyn_AggrKind k=_tmp50;struct Cyc_List_List*fs=_tmp51;
# 265
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=6U,(_tmp5D->f1).is_union=(int)k == (int)Cyc_Absyn_UnionA,(_tmp5D->f1).fieldnum=- 1,({
struct _fat_ptr _tmp269=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)k == (int)Cyc_Absyn_UnionA,leafval);_tmp5D->f2=_tmp269;});_tmp5D;});}case 4U: _LL9: _tmp4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_tmp4F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zero_term;_LLA: {void*et=_tmp4E;void*zeroterm=_tmp4F;
# 268
if(Cyc_Tcutil_force_type2bool(0,zeroterm))
# 271
return(allow_zeroterm && !no_init_bits_only)&& Cyc_Tcutil_is_bits_only_type(et)?fenv->unknown_all: leafval;
goto _LL0;}case 3U: _LLD: _tmp4D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_LLE: {void*nbl=_tmp4D;
# 275
if(!Cyc_Tcutil_force_type2bool(0,nbl)){
void*_tmp5E=leafval;if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5E)->tag == 2U){if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f1 == Cyc_CfFlowInfo_AllIL){_LL12: _LL13:
 return fenv->notzeroall;}else{goto _LL14;}}else{_LL14: _LL15:
 goto _LL11;}_LL11:;}
# 280
goto _LL0;}default: _LLF: _LL10:
 goto _LL0;}_LL0:;}
# 284
return !no_init_bits_only && Cyc_Tcutil_is_bits_only_type(t)?fenv->unknown_all: leafval;}
# 287
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 293
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmp5F=r;void*_tmp60;struct _fat_ptr _tmp63;int _tmp62;int _tmp61;void*_tmp66;int _tmp65;struct Cyc_Absyn_Exp*_tmp64;switch(*((int*)_tmp5F)){case 7U: _LL1: _tmp64=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1;_tmp65=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f2;_tmp66=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f3;_LL2: {struct Cyc_Absyn_Exp*consumer=_tmp64;int iteration=_tmp65;void*r=_tmp66;
# 296
if(consumer == e && iteration == env_iteration){
*needs_unconsume=1;
return 0;}
# 300
return 1;}case 6U: _LL3: _tmp61=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1).is_union;_tmp62=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1).fieldnum;_tmp63=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f2;_LL4: {int is_union=_tmp61;int field_no=_tmp62;struct _fat_ptr d=_tmp63;
# 302
if(!is_union || field_no == -1){
unsigned _tmp67=_get_fat_size(d,sizeof(void*));unsigned sz=_tmp67;
{int i=0;for(0;(unsigned)i < sz;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)d.curr)[i],needs_unconsume))
return 1;}}
# 308
return 0;}else{
# 311
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_fat_subscript(d,sizeof(void*),field_no)),needs_unconsume);}}case 8U: _LL5: _tmp60=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f2;_LL6: {void*r=_tmp60;
# 313
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,r,needs_unconsume);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 320
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmp68=r;void*_tmp6A;struct Cyc_Absyn_Vardecl*_tmp69;struct _fat_ptr _tmp6C;struct Cyc_CfFlowInfo_UnionRInfo _tmp6B;void*_tmp6F;int _tmp6E;struct Cyc_Absyn_Exp*_tmp6D;switch(*((int*)_tmp68)){case 7U: _LL1: _tmp6D=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp68)->f1;_tmp6E=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp68)->f2;_tmp6F=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp68)->f3;_LL2: {struct Cyc_Absyn_Exp*consumer=_tmp6D;int iteration=_tmp6E;void*r=_tmp6F;
# 323
return r;}case 6U: _LL3: _tmp6B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp68)->f1;_tmp6C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp68)->f2;_LL4: {struct Cyc_CfFlowInfo_UnionRInfo uinfo=_tmp6B;struct _fat_ptr d=_tmp6C;
# 325
unsigned _tmp70=_get_fat_size(d,sizeof(void*));unsigned sz=_tmp70;
int change=0;
struct _fat_ptr d2=({unsigned _tmp73=sz;void**_tmp72=_cycalloc(_check_times(_tmp73,sizeof(void*)));({{unsigned _tmp22C=sz;unsigned i;for(i=0;i < _tmp22C;++ i){_tmp72[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmp72,sizeof(void*),_tmp73);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp26A=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)d.curr)[i]);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp26A;});
if(((void**)d2.curr)[i]!= ((void**)d.curr)[i])
change=1;}}
# 333
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=6U,_tmp71->f1=uinfo,_tmp71->f2=d2;_tmp71;});else{
return r;}}case 8U: _LL5: _tmp69=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp68)->f1;_tmp6A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp68)->f2;_LL6: {struct Cyc_Absyn_Vardecl*n=_tmp69;void*r2=_tmp6A;
# 337
void*_tmp74=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,r2);void*r3=_tmp74;
if(r3 != r2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=8U,_tmp75->f1=n,_tmp75->f2=r3;_tmp75;});else{
return r;}}default: _LL7: _LL8:
 return r;}_LL0:;}
# 347
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple12 _tmp76=({struct _tuple12 _tmp22F;({void*_tmp26B=Cyc_Tcutil_compress(t);_tmp22F.f1=_tmp26B;}),_tmp22F.f2=r;_tmp22F;});struct _tuple12 _stmttmp9=_tmp76;struct _tuple12 _tmp77=_stmttmp9;struct _fat_ptr _tmp7B;struct Cyc_CfFlowInfo_UnionRInfo _tmp7A;struct Cyc_List_List*_tmp79;enum Cyc_Absyn_AggrKind _tmp78;struct _fat_ptr _tmp7E;struct Cyc_CfFlowInfo_UnionRInfo _tmp7D;union Cyc_Absyn_AggrInfo _tmp7C;struct _fat_ptr _tmp81;struct Cyc_CfFlowInfo_UnionRInfo _tmp80;struct Cyc_List_List*_tmp7F;void*_tmp83;struct Cyc_Absyn_Vardecl*_tmp82;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->tag == 8U){_LL1: _tmp82=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f1;_tmp83=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f2;_LL2: {struct Cyc_Absyn_Vardecl*s=_tmp82;void*r2=_tmp83;
# 350
void*_tmp84=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,r2);void*r3=_tmp84;
if(r3 != r2)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=8U,_tmp85->f1=s,_tmp85->f2=r3;_tmp85;});else{
return r;}}}else{switch(*((int*)_tmp77.f1)){case 6U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->tag == 6U){_LL3: _tmp7F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp77.f1)->f1;_tmp80=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f1;_tmp81=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f2;_LL4: {struct Cyc_List_List*tqts=_tmp7F;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp80;struct _fat_ptr d=_tmp81;
# 354
unsigned _tmp86=_get_fat_size(d,sizeof(void*));unsigned sz=_tmp86;
struct _fat_ptr d2=({unsigned _tmp89=sz;void**_tmp88=_cycalloc(_check_times(_tmp89,sizeof(void*)));({{unsigned _tmp22D=sz;unsigned i;for(i=0;i < _tmp22D;++ i){_tmp88[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp88,sizeof(void*),_tmp89);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp26C=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2,consumer,iteration,((void**)d.curr)[i]);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp26C;});
tqts=tqts->tl;}}
# 360
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=6U,_tmp87->f1=b,_tmp87->f2=d2;_tmp87;});}}else{goto _LL9;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp77.f1)->f1)->tag == 20U){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->tag == 6U){_LL5: _tmp7C=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp77.f1)->f1)->f1;_tmp7D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f1;_tmp7E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f2;_LL6: {union Cyc_Absyn_AggrInfo info=_tmp7C;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp7D;struct _fat_ptr d=_tmp7E;
# 362
struct Cyc_Absyn_Aggrdecl*_tmp8A=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp8A;
if(ad->impl == 0)return r;
_tmp78=ad->kind;_tmp79=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp7A=b;_tmp7B=d;goto _LL8;}}else{goto _LL9;}}else{goto _LL9;}case 7U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->tag == 6U){_LL7: _tmp78=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp77.f1)->f1;_tmp79=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp77.f1)->f2;_tmp7A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f1;_tmp7B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f2;_LL8: {enum Cyc_Absyn_AggrKind k=_tmp78;struct Cyc_List_List*fs=_tmp79;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp7A;struct _fat_ptr d=_tmp7B;
# 366
struct _fat_ptr d2=({unsigned _tmp91=_get_fat_size(d,sizeof(void*));void**_tmp90=_cycalloc(_check_times(_tmp91,sizeof(void*)));({{unsigned _tmp22E=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp22E;++ i){_tmp90[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmp90,sizeof(void*),_tmp91);});
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp8B=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_stmttmpA=_tmp8B;struct Cyc_Absyn_Aggrfield*_tmp8C=_stmttmpA;void*_tmp8E;struct _fat_ptr*_tmp8D;_LLC: _tmp8D=_tmp8C->name;_tmp8E=_tmp8C->type;_LLD: {struct _fat_ptr*n=_tmp8D;void*t2=_tmp8E;
if(_get_fat_size(*n,sizeof(char))!= (unsigned)1)
({void*_tmp26D=Cyc_CfFlowInfo_make_unique_consumed(fenv,t2,consumer,iteration,*((void**)_check_fat_subscript(d,sizeof(void*),i)));*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp26D;});}}}
# 373
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=6U,_tmp8F->f1=b,_tmp8F->f2=d2;_tmp8F;});}}else{goto _LL9;}default: _LL9: _LLA:
# 375
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=7U,_tmp92->f1=consumer,_tmp92->f2=iteration,_tmp92->f3=r;_tmp92;});else{
return r;}}}_LL0:;}struct _tuple16{struct Cyc_CfFlowInfo_Place*f1;unsigned f2;};
# 381
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 383
struct _RegionHandle _tmp93=_new_region("r");struct _RegionHandle*r=& _tmp93;_push_region(r);
{struct _tuple16 elem=({struct _tuple16 _tmp230;_tmp230.f1=place,_tmp230.f2=0U;_tmp230;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple16*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp94=elem.f1;struct Cyc_CfFlowInfo_Place*place2=_tmp94;
struct Cyc_CfFlowInfo_Place _tmp95=*place;struct Cyc_CfFlowInfo_Place _stmttmpB=_tmp95;struct Cyc_CfFlowInfo_Place _tmp96=_stmttmpB;struct Cyc_List_List*_tmp98;void*_tmp97;_LL1: _tmp97=_tmp96.root;_tmp98=_tmp96.path;_LL2: {void*root1=_tmp97;struct Cyc_List_List*fs1=_tmp98;
struct Cyc_CfFlowInfo_Place _tmp99=*place2;struct Cyc_CfFlowInfo_Place _stmttmpC=_tmp99;struct Cyc_CfFlowInfo_Place _tmp9A=_stmttmpC;struct Cyc_List_List*_tmp9C;void*_tmp9B;_LL4: _tmp9B=_tmp9A.root;_tmp9C=_tmp9A.path;_LL5: {void*root2=_tmp9B;struct Cyc_List_List*fs2=_tmp9C;
if(Cyc_CfFlowInfo_root_cmp(root1,root2)!= 0)
continue;
for(0;fs1 != 0 && fs2 != 0;(fs1=fs1->tl,fs2=fs2->tl)){
if((void*)fs1->hd != (void*)fs2->hd)break;}
if(fs1 == 0){
int _tmp9D=1;_npop_handler(0U);return _tmp9D;}}}}{
# 397
int _tmp9E=0;_npop_handler(0U);return _tmp9E;}}
# 384
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct Cyc_List_List*places;};
# 407
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 410
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_tmp26E=({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->hd=place,_tmp9F->tl=pile->places;_tmp9F;});pile->places=_tmp26E;});}
# 413
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmpA0=r;struct _fat_ptr _tmpA2;struct Cyc_CfFlowInfo_UnionRInfo _tmpA1;struct Cyc_CfFlowInfo_Place*_tmpA3;void*_tmpA4;void*_tmpA5;switch(*((int*)_tmpA0)){case 7U: _LL1: _tmpA5=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f3;_LL2: {void*r=_tmpA5;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 8U: _LL3: _tmpA4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f2;_LL4: {void*r=_tmpA4;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 4U: _LL5: _tmpA3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f1;_LL6: {struct Cyc_CfFlowInfo_Place*p=_tmpA3;
Cyc_CfFlowInfo_add_place(pile,p);return;}case 6U: _LL7: _tmpA1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f1;_tmpA2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f2;_LL8: {struct Cyc_CfFlowInfo_UnionRInfo b=_tmpA1;struct _fat_ptr d=_tmpA2;
# 419
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)d.curr)[i]);}}
return;}default: _LL9: _LLA:
 return;}_LL0:;}
# 429
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){
void*_tmpA6=old_val;void*_tmpA8;struct Cyc_Absyn_Vardecl*_tmpA7;void*_tmpAB;int _tmpAA;struct Cyc_Absyn_Exp*_tmpA9;struct _fat_ptr _tmpAD;int _tmpAC;switch(*((int*)_tmpA6)){case 6U: _LL1: _tmpAC=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f1).is_union;_tmpAD=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f2;_LL2: {int is_union=_tmpAC;struct _fat_ptr d=_tmpAD;
# 432
struct _fat_ptr d2=({unsigned _tmpB0=_get_fat_size(d,sizeof(void*));void**_tmpAF=_cycalloc(_check_times(_tmpB0,sizeof(void*)));({{unsigned _tmp231=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp231;++ i){({
void*_tmp26F=Cyc_CfFlowInfo_insert_place_inner(new_val,((void**)d.curr)[(int)i]);_tmpAF[i]=_tmp26F;});}}0;});_tag_fat(_tmpAF,sizeof(void*),_tmpB0);});
# 436
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=6U,(_tmpAE->f1).is_union=is_union,(_tmpAE->f1).fieldnum=- 1,_tmpAE->f2=d2;_tmpAE;});}case 7U: _LL3: _tmpA9=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f1;_tmpAA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f2;_tmpAB=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpA9;int i=_tmpAA;void*rval=_tmpAB;
# 438
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=7U,_tmpB1->f1=e,_tmpB1->f2=i,({void*_tmp270=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_tmpB1->f3=_tmp270;});_tmpB1;});}case 8U: _LL5: _tmpA7=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f1;_tmpA8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f2;_LL6: {struct Cyc_Absyn_Vardecl*n=_tmpA7;void*rval=_tmpA8;
# 440
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=8U,_tmpB2->f1=n,({void*_tmp271=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_tmpB2->f2=_tmp271;});_tmpB2;});}default: _LL7: _LL8:
 return new_val;}_LL0:;}
# 447
static struct _fat_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _fat_ptr d,int n,void*rval){
void*_tmpB3=*((void**)_check_fat_subscript(d,sizeof(void*),n));void*old_rval=_tmpB3;
if(old_rval == rval)return d;{
struct _fat_ptr res=({unsigned _tmpB5=_get_fat_size(d,sizeof(void*));void**_tmpB4=_cycalloc(_check_times(_tmpB5,sizeof(void*)));({{unsigned _tmp232=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp232;++ i){_tmpB4[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmpB4,sizeof(void*),_tmpB5);});
*((void**)_check_fat_subscript(res,sizeof(void*),n))=rval;
return res;}}struct _tuple17{struct Cyc_List_List*f1;void*f2;};
# 460
static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*path,void*old_val,void*new_val){
# 462
if(path == 0)
return Cyc_CfFlowInfo_insert_place_inner(new_val,old_val);{
struct _tuple17 _tmpB6=({struct _tuple17 _tmp233;_tmp233.f1=path,_tmp233.f2=old_val;_tmp233;});struct _tuple17 _stmttmpD=_tmpB6;struct _tuple17 _tmpB7=_stmttmpD;void*_tmpB9;struct Cyc_Absyn_Vardecl*_tmpB8;void*_tmpBB;struct Cyc_List_List*_tmpBA;struct _fat_ptr _tmpBF;int _tmpBE;struct Cyc_List_List*_tmpBD;int _tmpBC;if(_tmpB7.f1 != 0){if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)((struct Cyc_List_List*)_tmpB7.f1)->hd)->tag == 0U)switch(*((int*)_tmpB7.f2)){case 6U: _LL1: _tmpBC=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmpB7.f1)->hd)->f1;_tmpBD=(_tmpB7.f1)->tl;_tmpBE=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f1).is_union;_tmpBF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f2;_LL2: {int i=_tmpBC;struct Cyc_List_List*tl=_tmpBD;int is_union=_tmpBE;struct _fat_ptr d=_tmpBF;
# 466
void*_tmpC0=Cyc_CfFlowInfo_insert_place_outer(tl,*((void**)_check_fat_subscript(d,sizeof(void*),i)),new_val);void*new_child=_tmpC0;
struct _fat_ptr _tmpC1=Cyc_CfFlowInfo_aggr_dict_insert(d,i,new_child);struct _fat_ptr new_d=_tmpC1;
if((void**)new_d.curr == (void**)d.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->tag=6U,(_tmpC2->f1).is_union=is_union,(_tmpC2->f1).fieldnum=- 1,_tmpC2->f2=new_d;_tmpC2;});}}case 8U: goto _LL5;default: goto _LL7;}else{switch(*((int*)_tmpB7.f2)){case 5U: _LL3: _tmpBA=(_tmpB7.f1)->tl;_tmpBB=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f1;_LL4: {struct Cyc_List_List*tl=_tmpBA;void*rval=_tmpBB;
# 471
void*_tmpC3=Cyc_CfFlowInfo_insert_place_outer(tl,rval,new_val);void*new_rval=_tmpC3;
if(new_rval == rval)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->tag=5U,_tmpC4->f1=new_rval;_tmpC4;});}}case 8U: goto _LL5;default: goto _LL7;}}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->tag == 8U){_LL5: _tmpB8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f1;_tmpB9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f2;_LL6: {struct Cyc_Absyn_Vardecl*n=_tmpB8;void*rval=_tmpB9;
# 475
void*_tmpC5=Cyc_CfFlowInfo_insert_place_outer(path,rval,new_val);void*new_rval=_tmpC5;
if(new_rval == rval)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=8U,_tmpC6->f1=n,_tmpC6->f2=new_rval;_tmpC6;});}}}else{_LL7: _LL8:
({void*_tmpC7=0U;({struct _fat_ptr _tmp272=({const char*_tmpC8="bad insert place";_tag_fat(_tmpC8,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp272,_tag_fat(_tmpC7,sizeof(void*),0U));});});}}_LL0:;}}
# 485
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmpC9=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;struct Cyc_CfFlowInfo_Place*place=_tmpC9;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;{
void*oldval;void*newval;
{struct _handler_cons _tmpCA;_push_handler(& _tmpCA);{int _tmpCC=0;if(setjmp(_tmpCA.handler))_tmpCC=1;if(!_tmpCC){oldval=Cyc_CfFlowInfo_lookup_place(d,place);;_pop_handler();}else{void*_tmpCB=(void*)Cyc_Core_get_exn_thrown();void*_tmpCD=_tmpCB;void*_tmpCE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpCD)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 continue;}else{_LL3: _tmpCE=_tmpCD;_LL4: {void*exn=_tmpCE;(int)_rethrow(exn);}}_LL0:;}}}
{enum Cyc_CfFlowInfo_InitLevel _tmpCF=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _stmttmpE=_tmpCF;enum Cyc_CfFlowInfo_InitLevel _tmpD0=_stmttmpE;if(_tmpD0 == Cyc_CfFlowInfo_AllIL){_LL6: _LL7:
 newval=fenv->esc_all;goto _LL5;}else{_LL8: _LL9:
 newval=fenv->esc_none;goto _LL5;}_LL5:;}
# 496
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmpD1=*place;struct Cyc_CfFlowInfo_Place _stmttmpF=_tmpD1;struct Cyc_CfFlowInfo_Place _tmpD2=_stmttmpF;struct Cyc_List_List*_tmpD4;void*_tmpD3;_LLB: _tmpD3=_tmpD2.root;_tmpD4=_tmpD2.path;_LLC: {void*root=_tmpD3;struct Cyc_List_List*path=_tmpD4;
d=({struct Cyc_Dict_Dict _tmp276=d;void*_tmp275=root;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp276,_tmp275,({
struct Cyc_List_List*_tmp274=path;void*_tmp273=
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,root);
# 499
Cyc_CfFlowInfo_insert_place_outer(_tmp274,_tmp273,newval);}));});}}}}
# 503
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 513
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmpD5=r;void*_tmpD6;enum Cyc_CfFlowInfo_InitLevel _tmpD7;enum Cyc_CfFlowInfo_InitLevel _tmpD8;switch(*((int*)_tmpD5)){case 2U: _LL1: _tmpD8=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD5)->f1;_LL2: {enum Cyc_CfFlowInfo_InitLevel il=_tmpD8;
return il;}case 3U: _LL3: _tmpD7=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmpD5)->f1;_LL4: {enum Cyc_CfFlowInfo_InitLevel il=_tmpD7;
return il;}case 0U: _LL5: _LL6:
 goto _LL8;case 1U: _LL7: _LL8:
 return Cyc_CfFlowInfo_AllIL;case 7U: _LL9: _tmpD6=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD5)->f3;_LLA: {void*r=_tmpD6;
return Cyc_CfFlowInfo_NoneIL;}default: _LLB: _LLC:
({void*_tmpD9=0U;({struct _fat_ptr _tmp277=({const char*_tmpDA="initlevel_approx";_tag_fat(_tmpDA,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp277,_tag_fat(_tmpD9,sizeof(void*),0U));});});}_LL0:;}
# 523
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 525
enum Cyc_CfFlowInfo_InitLevel this_ans;
if((int)acc == (int)0U)return Cyc_CfFlowInfo_NoneIL;
{void*_tmpDB=r;void*_tmpDC;struct Cyc_CfFlowInfo_Place*_tmpDD;struct _fat_ptr _tmpDE;struct _fat_ptr _tmpE1;int _tmpE0;int _tmpDF;void*_tmpE2;switch(*((int*)_tmpDB)){case 8U: _LL1: _tmpE2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f2;_LL2: {void*r=_tmpE2;
return Cyc_CfFlowInfo_initlevel_rec(env,r,acc);}case 6U: _LL3: _tmpDF=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f1).is_union;_tmpE0=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f1).fieldnum;_tmpE1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f2;if(_tmpDF){_LL4: {int iu=_tmpDF;int f=_tmpE0;struct _fat_ptr d=_tmpE1;
# 533
unsigned _tmpE3=_get_fat_size(d,sizeof(void*));unsigned sz=_tmpE3;
this_ans=0U;
if(f == -1){
int i=0;for(0;(unsigned)i < sz;++ i){
if((int)Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[i],Cyc_CfFlowInfo_AllIL)== (int)Cyc_CfFlowInfo_AllIL){
this_ans=1U;
break;}}}else{
# 543
if((int)Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_fat_subscript(d,sizeof(void*),f)),Cyc_CfFlowInfo_AllIL)== (int)Cyc_CfFlowInfo_AllIL)
this_ans=1U;}
goto _LL0;}}else{_LL5: _tmpDE=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f2;_LL6: {struct _fat_ptr d=_tmpDE;
# 547
unsigned _tmpE4=_get_fat_size(d,sizeof(void*));unsigned sz=_tmpE4;
this_ans=1U;
{int i=0;for(0;(unsigned)i < sz;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[i],this_ans);}}
goto _LL0;}}case 4U: _LL7: _tmpDD=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f1;_LL8: {struct Cyc_CfFlowInfo_Place*p=_tmpDD;
# 553
if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,p))
this_ans=1U;else{
# 556
({struct Cyc_List_List*_tmp278=({struct Cyc_List_List*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->hd=p,_tmpE5->tl=env->seen;_tmpE5;});env->seen=_tmp278;});
this_ans=({struct Cyc_CfFlowInfo_InitlevelEnv*_tmp279=env;Cyc_CfFlowInfo_initlevel_rec(_tmp279,Cyc_CfFlowInfo_lookup_place(env->d,p),Cyc_CfFlowInfo_AllIL);});
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;}
# 560
goto _LL0;}case 5U: _LL9: _tmpDC=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f1;_LLA: {void*r=_tmpDC;
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,r,Cyc_CfFlowInfo_AllIL);goto _LL0;}default: _LLB: _LLC:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);goto _LL0;}_LL0:;}
# 564
return this_ans;}
# 566
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmpE6=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp234;_tmp234.d=d,_tmp234.seen=0;_tmp234;});struct Cyc_CfFlowInfo_InitlevelEnv env=_tmpE6;
return Cyc_CfFlowInfo_initlevel_rec(& env,r,Cyc_CfFlowInfo_AllIL);}
# 571
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmpE7=*place;struct Cyc_CfFlowInfo_Place _stmttmp10=_tmpE7;struct Cyc_CfFlowInfo_Place _tmpE8=_stmttmp10;struct Cyc_List_List*_tmpEA;void*_tmpE9;_LL1: _tmpE9=_tmpE8.root;_tmpEA=_tmpE8.path;_LL2: {void*root=_tmpE9;struct Cyc_List_List*path=_tmpEA;
void*_tmpEB=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,root);void*ans=_tmpEB;
for(0;path != 0;path=path->tl){
retry: {struct _tuple12 _tmpEC=({struct _tuple12 _tmp235;_tmp235.f1=ans,_tmp235.f2=(void*)path->hd;_tmp235;});struct _tuple12 _stmttmp11=_tmpEC;struct _tuple12 _tmpED=_stmttmp11;void*_tmpEE;int _tmpF1;struct _fat_ptr _tmpF0;struct Cyc_CfFlowInfo_UnionRInfo _tmpEF;void*_tmpF2;void*_tmpF3;switch(*((int*)_tmpED.f1)){case 8U: _LL4: _tmpF3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpED.f1)->f2;_LL5: {void*rval=_tmpF3;
# 577
ans=rval;goto retry;}case 7U: _LL6: _tmpF2=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpED.f1)->f3;_LL7: {void*rval=_tmpF2;
# 580
ans=rval;goto retry;}case 6U: if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmpED.f2)->tag == 0U){_LL8: _tmpEF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpED.f1)->f1;_tmpF0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpED.f1)->f2;_tmpF1=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmpED.f2)->f1;_LL9: {struct Cyc_CfFlowInfo_UnionRInfo is_union=_tmpEF;struct _fat_ptr d2=_tmpF0;int fname=_tmpF1;
# 582
ans=*((void**)_check_fat_subscript(d2,sizeof(void*),fname));
goto _LL3;}}else{goto _LLC;}case 5U: if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_tmpED.f2)->tag == 1U){_LLA: _tmpEE=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpED.f1)->f1;_LLB: {void*rval=_tmpEE;
# 585
ans=rval;
goto _LL3;}}else{goto _LLC;}default: _LLC: _LLD:
# 593
({void*_tmpF4=0U;({struct _fat_ptr _tmp27A=({const char*_tmpF5="bad lookup_place";_tag_fat(_tmpF5,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp27A,_tag_fat(_tmpF4,sizeof(void*),0U));});});}_LL3:;}}
# 595
return ans;}}
# 598
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmpF6=rval;struct _fat_ptr _tmpF9;int _tmpF8;int _tmpF7;void*_tmpFA;void*_tmpFB;switch(*((int*)_tmpF6)){case 3U: _LL1: _LL2:
 return 0;case 7U: _LL3: _tmpFB=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f3;_LL4: {void*r=_tmpFB;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 8U: _LL5: _tmpFA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f2;_LL6: {void*r=_tmpFA;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 6U: _LL7: _tmpF7=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f1).is_union;_tmpF8=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f1).fieldnum;_tmpF9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f2;_LL8: {int is_union=_tmpF7;int field_no=_tmpF8;struct _fat_ptr d=_tmpF9;
# 604
if(is_union && field_no != -1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_fat_subscript(d,sizeof(void*),field_no)));else{
# 607
unsigned sz=_get_fat_size(d,sizeof(void*));
{int i=0;for(0;(unsigned)i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)d.curr)[i]))return 0;}}
return 1;}}default: _LL9: _LLA:
# 612
 return 1;}_LL0:;}
# 615
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 618
int Cyc_CfFlowInfo_is_init_pointer(void*rval){
void*_tmpFC=rval;void*_tmpFD;switch(*((int*)_tmpFC)){case 8U: _LL1: _tmpFD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFC)->f2;_LL2: {void*r=_tmpFD;
return Cyc_CfFlowInfo_is_init_pointer(r);}case 4U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 628
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle _tmpFE=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFE;_push_region(rgn);
{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->places=0;_tmp100;});
Cyc_CfFlowInfo_add_places(pile,r);{
struct Cyc_Dict_Dict _tmpFF=Cyc_CfFlowInfo_escape_these(fenv,pile,d);_npop_handler(0U);return _tmpFF;}}
# 630
;_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 642
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 647
struct _tuple12 _tmp101=({struct _tuple12 _tmp237;_tmp237.f1=oldval,_tmp237.f2=newval;_tmp237;});struct _tuple12 _stmttmp12=_tmp101;struct _tuple12 _tmp102=_stmttmp12;enum Cyc_CfFlowInfo_InitLevel _tmp103;struct _fat_ptr _tmp107;struct Cyc_CfFlowInfo_UnionRInfo _tmp106;struct _fat_ptr _tmp105;struct Cyc_CfFlowInfo_UnionRInfo _tmp104;struct Cyc_CfFlowInfo_Place*_tmp108;void*_tmp10A;struct Cyc_Absyn_Vardecl*_tmp109;void*_tmp10B;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f1)->tag == 8U){_LL1: _tmp10B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f1)->f2;_LL2: {void*r1=_tmp10B;
# 649
return Cyc_CfFlowInfo_assign_place_inner(env,r1,newval);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->tag == 8U){_LL3: _tmp109=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f1;_tmp10A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f2;_LL4: {struct Cyc_Absyn_Vardecl*n=_tmp109;void*r=_tmp10A;
# 651
void*_tmp10C=Cyc_CfFlowInfo_assign_place_inner(env,oldval,r);void*new_rval=_tmp10C;
if(new_rval == r)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->tag=8U,_tmp10D->f1=n,_tmp10D->f2=new_rval;_tmp10D;});}}}else{switch(*((int*)_tmp102.f1)){case 3U: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->tag == 4U){_LL5: _tmp108=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f1;_LL6: {struct Cyc_CfFlowInfo_Place*p=_tmp108;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL8;}}else{_LL7: _LL8:
# 656
 if((int)Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp10E=0U;({unsigned _tmp27C=env->loc;struct _fat_ptr _tmp27B=({const char*_tmp10F="assignment puts possibly-uninitialized data in an escaped location";_tag_fat(_tmp10F,sizeof(char),67U);});Cyc_CfFlowInfo_aerr(_tmp27C,_tmp27B,_tag_fat(_tmp10E,sizeof(void*),0U));});});
# 659
return(env->fenv)->esc_all;}case 6U: switch(*((int*)_tmp102.f2)){case 6U: _LL9: _tmp104=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102.f1)->f1;_tmp105=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102.f1)->f2;_tmp106=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f1;_tmp107=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f2;_LLA: {struct Cyc_CfFlowInfo_UnionRInfo is_union1=_tmp104;struct _fat_ptr d1=_tmp105;struct Cyc_CfFlowInfo_UnionRInfo is_union2=_tmp106;struct _fat_ptr d2=_tmp107;
# 661
struct _fat_ptr new_d=({unsigned _tmp112=
_get_fat_size(d1,sizeof(void*));void**_tmp111=_cycalloc(_check_times(_tmp112,sizeof(void*)));({{unsigned _tmp236=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _tmp236;++ i){({void*_tmp27D=Cyc_CfFlowInfo_assign_place_inner(env,((void**)d1.curr)[(int)i],*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));_tmp111[i]=_tmp27D;});}}0;});_tag_fat(_tmp111,sizeof(void*),_tmp112);});
# 665
int change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(*((void**)_check_fat_subscript(new_d,sizeof(void*),i))!= ((void**)d1.curr)[i]){
change=1;break;}}}
# 670
if(!change){
if(!is_union1.is_union)return oldval;
new_d=d1;}else{
# 675
change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(({void*_tmp27E=*((void**)_check_fat_subscript(new_d,sizeof(void*),i));_tmp27E != *((void**)_check_fat_subscript(d2,sizeof(void*),i));})){
change=1;break;}}}
# 680
if(!change){
if(!is_union1.is_union)return newval;
new_d=d2;}}
# 685
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->tag=6U,_tmp110->f1=is_union2,_tmp110->f2=new_d;_tmp110;});}case 3U: goto _LLB;default: goto _LLD;}default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->tag == 3U){_LLB: _tmp103=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f1;_LLC: {enum Cyc_CfFlowInfo_InitLevel il=_tmp103;
# 687
enum Cyc_CfFlowInfo_InitLevel _tmp113=il;if(_tmp113 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 return(env->fenv)->unknown_none;}else{_LL12: _LL13:
 return(env->fenv)->unknown_all;}_LLF:;}}else{_LLD: _LLE:
# 691
 return newval;}}}}_LL0:;}
# 697
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 701
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*path,int path_prefix,void*oldval,void*newval){
# 712
if(path == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple17 _tmp114=({struct _tuple17 _tmp238;_tmp238.f1=path,_tmp238.f2=oldval;_tmp238;});struct _tuple17 _stmttmp13=_tmp114;struct _tuple17 _tmp115=_stmttmp13;struct _fat_ptr _tmp11A;int _tmp119;int _tmp118;struct Cyc_List_List*_tmp117;int _tmp116;void*_tmp11C;struct Cyc_List_List*_tmp11B;void*_tmp11F;int _tmp11E;struct Cyc_Absyn_Exp*_tmp11D;void*_tmp121;struct Cyc_Absyn_Vardecl*_tmp120;switch(*((int*)_tmp115.f2)){case 8U: _LL1: _tmp120=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1;_tmp121=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f2;_LL2: {struct Cyc_Absyn_Vardecl*n=_tmp120;void*r=_tmp121;
# 715
void*_tmp122=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);void*new_r=_tmp122;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=8U,_tmp123->f1=n,_tmp123->f2=new_r;_tmp123;});}}case 7U: _LL3: _tmp11D=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1;_tmp11E=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f2;_tmp11F=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f3;_LL4: {struct Cyc_Absyn_Exp*x=_tmp11D;int y=_tmp11E;void*r=_tmp11F;
# 719
void*_tmp124=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);void*new_r=_tmp124;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=7U,_tmp125->f1=x,_tmp125->f2=y,_tmp125->f3=new_r;_tmp125;});}}default: if(_tmp115.f1 != 0){if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)((struct Cyc_List_List*)_tmp115.f1)->hd)->tag == 1U){if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->tag == 5U){_LL5: _tmp11B=(_tmp115.f1)->tl;_tmp11C=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1;_LL6: {struct Cyc_List_List*tl=_tmp11B;void*r=_tmp11C;
# 723
void*_tmp126=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,r,newval);void*new_r=_tmp126;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=5U,_tmp127->f1=new_r;_tmp127;});}}}else{goto _LL9;}}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->tag == 6U){_LL7: _tmp116=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmp115.f1)->hd)->f1;_tmp117=(_tmp115.f1)->tl;_tmp118=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1).is_union;_tmp119=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1).fieldnum;_tmp11A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f2;_LL8: {int fnum=_tmp116;struct Cyc_List_List*tl=_tmp117;int is_union=_tmp118;int fldnum=_tmp119;struct _fat_ptr d=_tmp11A;
# 727
void*_tmp128=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,*((void**)_check_fat_subscript(d,sizeof(void*),fnum)),newval);void*new_child=_tmp128;
# 729
struct _fat_ptr _tmp129=Cyc_CfFlowInfo_aggr_dict_insert(d,fnum,new_child);struct _fat_ptr new_child_agg=_tmp129;
if((void**)new_child_agg.curr == (void**)d.curr &&(!is_union || fldnum == fnum))return oldval;
d=new_child_agg;
# 734
if(is_union){
int changed=0;
int sz=(int)_get_fat_size(d,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != fnum){
struct _fat_ptr _tmp12A=({
struct _fat_ptr _tmp280=d;int _tmp27F=i;Cyc_CfFlowInfo_aggr_dict_insert(_tmp280,_tmp27F,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->unknown_all,*((void**)_check_fat_subscript(d,sizeof(void*),i))));});
# 739
struct _fat_ptr new_d=_tmp12A;
# 742
if((void**)new_d.curr != (void**)d.curr){
d=new_d;
changed=1;}}}}
# 750
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp12B=env->root_place;struct Cyc_CfFlowInfo_Place*_stmttmp14=_tmp12B;struct Cyc_CfFlowInfo_Place*_tmp12C=_stmttmp14;struct Cyc_List_List*_tmp12E;void*_tmp12D;_LLC: _tmp12D=_tmp12C->root;_tmp12E=_tmp12C->path;_LLD: {void*root=_tmp12D;struct Cyc_List_List*path=_tmp12E;
struct Cyc_List_List*_tmp12F=({int*_tmp281=({int*_tmp132=_cycalloc_atomic(sizeof(*_tmp132));*_tmp132=path_prefix;_tmp132;});((struct Cyc_List_List*(*)(int(*f)(int*,void*),int*env,struct Cyc_List_List*x))Cyc_List_filter_c)((int(*)(int*n,void*unused))Cyc_CfFlowInfo_nprefix,_tmp281,path);});struct Cyc_List_List*new_path=_tmp12F;
struct Cyc_CfFlowInfo_Place*_tmp130=({struct Cyc_CfFlowInfo_Place*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->root=root,_tmp131->path=new_path;_tmp131;});struct Cyc_CfFlowInfo_Place*curr_place=_tmp130;;}}}
# 756
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->tag=6U,(_tmp133->f1).is_union=is_union,(_tmp133->f1).fieldnum=fnum,_tmp133->f2=d;_tmp133;});}}else{goto _LL9;}}}else{_LL9: _LLA:
({void*_tmp134=0U;({struct _fat_ptr _tmp282=({const char*_tmp135="bad assign place";_tag_fat(_tmp135,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp282,_tag_fat(_tmp134,sizeof(void*),0U));});});}}_LL0:;}}
# 760
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 768
struct Cyc_CfFlowInfo_Place*_tmp136=place;struct Cyc_List_List*_tmp138;void*_tmp137;_LL1: _tmp137=_tmp136->root;_tmp138=_tmp136->path;_LL2: {void*root=_tmp137;struct Cyc_List_List*path=_tmp138;
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp239;_tmp239.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp283=({struct Cyc_CfFlowInfo_EscPile*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->places=0;_tmp139;});_tmp239.pile=_tmp283;}),_tmp239.d=d,_tmp239.loc=loc,_tmp239.root_place=place;_tmp239;});
void*newval=({struct Cyc_List_List*_tmp285=path;void*_tmp284=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,root);Cyc_CfFlowInfo_assign_place_outer(& env,_tmp285,0,_tmp284,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp287=fenv;struct Cyc_CfFlowInfo_EscPile*_tmp286=env.pile;Cyc_CfFlowInfo_escape_these(_tmp287,_tmp286,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,root,newval));});}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 783
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple18{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 791
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;
# 794
{struct _tuple12 _tmp13A=({struct _tuple12 _tmp23B;_tmp23B.f1=r1,_tmp23B.f2=r2;_tmp23B;});struct _tuple12 _stmttmp15=_tmp13A;struct _tuple12 _tmp13B=_stmttmp15;struct _fat_ptr _tmp141;int _tmp140;int _tmp13F;struct _fat_ptr _tmp13E;int _tmp13D;int _tmp13C;struct Cyc_CfFlowInfo_Place*_tmp142;void*_tmp143;void*_tmp145;void*_tmp144;void*_tmp146;struct Cyc_CfFlowInfo_Place*_tmp147;struct Cyc_CfFlowInfo_Place*_tmp148;struct Cyc_CfFlowInfo_Place*_tmp149;struct Cyc_CfFlowInfo_Place*_tmp14B;struct Cyc_CfFlowInfo_Place*_tmp14A;void*_tmp14E;int _tmp14D;struct Cyc_Absyn_Exp*_tmp14C;void*_tmp151;int _tmp150;struct Cyc_Absyn_Exp*_tmp14F;void*_tmp157;int _tmp156;struct Cyc_Absyn_Exp*_tmp155;void*_tmp154;int _tmp153;struct Cyc_Absyn_Exp*_tmp152;void*_tmp159;struct Cyc_Absyn_Vardecl*_tmp158;void*_tmp15B;struct Cyc_Absyn_Vardecl*_tmp15A;void*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp15E;void*_tmp15D;struct Cyc_Absyn_Vardecl*_tmp15C;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->tag == 8U){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 8U){_LL1: _tmp15C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp15D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_tmp15E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_tmp15F=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_LL2: {struct Cyc_Absyn_Vardecl*n1=_tmp15C;void*r1=_tmp15D;struct Cyc_Absyn_Vardecl*n2=_tmp15E;void*r2=_tmp15F;
# 797
if(n1 == n2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->tag=8U,_tmp160->f1=n1,({void*_tmp288=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp160->f2=_tmp288;});_tmp160;});else{
# 800
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{_LL3: _tmp15A=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp15B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_LL4: {struct Cyc_Absyn_Vardecl*n1=_tmp15A;void*r1=_tmp15B;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 8U){_LL5: _tmp158=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_tmp159=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_LL6: {struct Cyc_Absyn_Vardecl*n2=_tmp158;void*r2=_tmp159;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->tag == 7U){if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 7U){_LL7: _tmp152=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp153=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_tmp154=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f3;_tmp155=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_tmp156=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_tmp157=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp152;int i1=_tmp153;void*r1=_tmp154;struct Cyc_Absyn_Exp*e2=_tmp155;int i2=_tmp156;void*r2=_tmp157;
# 805
if(e1 == e2)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->tag=7U,_tmp161->f1=e1,i1 > i2?_tmp161->f2=i1:(_tmp161->f2=i2),({void*_tmp289=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp161->f3=_tmp289;});_tmp161;});
{void*_tmp162=r1;struct Cyc_CfFlowInfo_Place*_tmp163;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp162)->tag == 4U){_LL22: _tmp163=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp162)->f1;_LL23: {struct Cyc_CfFlowInfo_Place*p=_tmp163;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL21;}}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 811
{void*_tmp164=r2;struct Cyc_CfFlowInfo_Place*_tmp165;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp164)->tag == 4U){_LL27: _tmp165=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp164)->f1;_LL28: {struct Cyc_CfFlowInfo_Place*p=_tmp165;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL26;}}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;}
# 815
goto _LL0;}}else{_LL9: _tmp14F=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp150=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_tmp151=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp14F;int i1=_tmp150;void*r1=_tmp151;
# 817
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->tag=7U,_tmp166->f1=e1,_tmp166->f2=i1,({void*_tmp28A=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp166->f3=_tmp28A;});_tmp166;});}}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 7U){_LLB: _tmp14C=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_tmp14D=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_tmp14E=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f3;_LLC: {struct Cyc_Absyn_Exp*e2=_tmp14C;int i2=_tmp14D;void*r2=_tmp14E;
# 819
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->tag=7U,_tmp167->f1=e2,_tmp167->f2=i2,({void*_tmp28B=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp167->f3=_tmp28B;});_tmp167;});}}else{switch(*((int*)_tmp13B.f1)){case 4U: switch(*((int*)_tmp13B.f2)){case 4U: _LLD: _tmp14A=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp14B=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LLE: {struct Cyc_CfFlowInfo_Place*p1=_tmp14A;struct Cyc_CfFlowInfo_Place*p2=_tmp14B;
# 822
if(Cyc_CfFlowInfo_place_cmp(p1,p2)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,p1);
Cyc_CfFlowInfo_add_place(env->pile,p2);
goto _LL0;}case 1U: _LLF: _tmp149=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_LL10: {struct Cyc_CfFlowInfo_Place*p=_tmp149;
# 828
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _tmp168=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _stmttmp16=_tmp168;enum Cyc_CfFlowInfo_InitLevel _tmp169=_stmttmp16;if(_tmp169 == Cyc_CfFlowInfo_AllIL){_LL2C: _LL2D:
 return(env->fenv)->notzeroall;}else{_LL2E: _LL2F:
 return(env->fenv)->unknown_none;}_LL2B:;}}default: _LL19: _tmp148=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_LL1A: {struct Cyc_CfFlowInfo_Place*p=_tmp148;
# 854
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}case 1U: switch(*((int*)_tmp13B.f2)){case 4U: _LL11: _tmp147=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LL12: {struct Cyc_CfFlowInfo_Place*p=_tmp147;
# 834
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _tmp16A=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _stmttmp17=_tmp16A;enum Cyc_CfFlowInfo_InitLevel _tmp16B=_stmttmp17;if(_tmp16B == Cyc_CfFlowInfo_AllIL){_LL31: _LL32:
 return(env->fenv)->notzeroall;}else{_LL33: _LL34:
 return(env->fenv)->unknown_none;}_LL30:;}}case 5U: _LL17: _tmp146=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LL18: {void*r2=_tmp146;
# 849
enum Cyc_CfFlowInfo_InitLevel _tmp16F=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _stmttmp19=_tmp16F;enum Cyc_CfFlowInfo_InitLevel _tmp170=_stmttmp19;if(_tmp170 == Cyc_CfFlowInfo_AllIL){_LL3B: _LL3C:
 return(env->fenv)->notzeroall;}else{_LL3D: _LL3E:
 return(env->fenv)->unknown_none;}_LL3A:;}default: goto _LL1F;}case 5U: switch(*((int*)_tmp13B.f2)){case 5U: _LL13: _tmp144=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp145=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LL14: {void*r1=_tmp144;void*r2=_tmp145;
# 841
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->tag=5U,({void*_tmp28C=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp16C->f1=_tmp28C;});_tmp16C;});}case 1U: _LL15: _tmp143=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_LL16: {void*r1=_tmp143;
# 844
enum Cyc_CfFlowInfo_InitLevel _tmp16D=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _stmttmp18=_tmp16D;enum Cyc_CfFlowInfo_InitLevel _tmp16E=_stmttmp18;if(_tmp16E == Cyc_CfFlowInfo_AllIL){_LL36: _LL37:
 return(env->fenv)->notzeroall;}else{_LL38: _LL39:
 return(env->fenv)->unknown_none;}_LL35:;}case 4U: goto _LL1B;default: goto _LL1F;}default: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 4U){_LL1B: _tmp142=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LL1C: {struct Cyc_CfFlowInfo_Place*p=_tmp142;
# 855
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->tag == 6U){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 6U){_LL1D: _tmp13C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1).is_union;_tmp13D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1).fieldnum;_tmp13E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_tmp13F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1).is_union;_tmp140=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1).fieldnum;_tmp141=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_LL1E: {int is_union1=_tmp13C;int field_no1=_tmp13D;struct _fat_ptr d1=_tmp13E;int is_union2=_tmp13F;int field_no2=_tmp140;struct _fat_ptr d2=_tmp141;
# 858
struct _fat_ptr new_d=({unsigned _tmp173=
_get_fat_size(d1,sizeof(void*));void**_tmp172=_cycalloc(_check_times(_tmp173,sizeof(void*)));({{unsigned _tmp23A=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _tmp23A;++ i){({void*_tmp28D=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)d1.curr)[(int)i],*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));_tmp172[i]=_tmp28D;});}}0;});_tag_fat(_tmp172,sizeof(void*),_tmp173);});
# 863
int change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(*((void**)_check_fat_subscript(new_d,sizeof(void*),i))!= ((void**)d1.curr)[i]){
change=1;break;}}}
# 868
if(!change){
if(!is_union1)return r1;
new_d=d1;}else{
# 873
change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(({void*_tmp28E=*((void**)_check_fat_subscript(new_d,sizeof(void*),i));_tmp28E != *((void**)_check_fat_subscript(d2,sizeof(void*),i));})){
change=1;break;}}}
# 878
if(!change){
if(!is_union1)return r2;
new_d=d2;}}
# 883
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->tag=6U,(_tmp171->f1).is_union=is_union1,field_no1 == field_no2?(_tmp171->f1).fieldnum=field_no1:((_tmp171->f1).fieldnum=- 1),_tmp171->f2=new_d;_tmp171;});}}else{goto _LL1F;}}else{_LL1F: _LL20:
# 885
 goto _LL0;}}}}}}}_LL0:;}{
# 887
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple12 _tmp174=({struct _tuple12 _tmp23E;_tmp23E.f1=r1,_tmp23E.f2=r2;_tmp23E;});struct _tuple12 _stmttmp1A=_tmp174;struct _tuple12 _tmp175=_stmttmp1A;if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp175.f1)->tag == 3U){_LL40: _LL41:
 goto _LL43;}else{if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp175.f2)->tag == 3U){_LL42: _LL43: {
# 892
struct _tuple18 _tmp176=({struct _tuple18 _tmp23C;_tmp23C.f1=il1,_tmp23C.f2=il2;_tmp23C;});struct _tuple18 _stmttmp1B=_tmp176;struct _tuple18 _tmp177=_stmttmp1B;if(_tmp177.f2 == Cyc_CfFlowInfo_NoneIL){_LL47: _LL48:
 goto _LL4A;}else{if(_tmp177.f1 == Cyc_CfFlowInfo_NoneIL){_LL49: _LL4A: return(env->fenv)->esc_none;}else{_LL4B: _LL4C:
 return(env->fenv)->esc_all;}}_LL46:;}}else{_LL44: _LL45: {
# 897
struct _tuple18 _tmp178=({struct _tuple18 _tmp23D;_tmp23D.f1=il1,_tmp23D.f2=il2;_tmp23D;});struct _tuple18 _stmttmp1C=_tmp178;struct _tuple18 _tmp179=_stmttmp1C;if(_tmp179.f2 == Cyc_CfFlowInfo_NoneIL){_LL4E: _LL4F:
 goto _LL51;}else{if(_tmp179.f1 == Cyc_CfFlowInfo_NoneIL){_LL50: _LL51: return(env->fenv)->unknown_none;}else{_LL52: _LL53:
 return(env->fenv)->unknown_all;}}_LL4D:;}}}_LL3F:;}}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 904
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 906
struct _tuple19 _tmp17A=({struct _tuple19 _tmp240;_tmp240.f1=f1,_tmp240.f2=f2;_tmp240;});struct _tuple19 _stmttmp1D=_tmp17A;struct _tuple19 _tmp17B=_stmttmp1D;struct Cyc_List_List*_tmp17F;struct Cyc_Dict_Dict _tmp17E;struct Cyc_List_List*_tmp17D;struct Cyc_Dict_Dict _tmp17C;if(((_tmp17B.f1).BottomFL).tag == 1){_LL1: _LL2:
 return f2;}else{if(((_tmp17B.f2).BottomFL).tag == 1){_LL3: _LL4:
 return f1;}else{_LL5: _tmp17C=(((_tmp17B.f1).ReachableFL).val).f1;_tmp17D=(((_tmp17B.f1).ReachableFL).val).f2;_tmp17E=(((_tmp17B.f2).ReachableFL).val).f1;_tmp17F=(((_tmp17B.f2).ReachableFL).val).f2;_LL6: {struct Cyc_Dict_Dict d1=_tmp17C;struct Cyc_List_List*r1=_tmp17D;struct Cyc_Dict_Dict d2=_tmp17E;struct Cyc_List_List*r2=_tmp17F;
# 912
if(d1.t == d2.t && r1 == r2)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct Cyc_CfFlowInfo_JoinEnv _tmp180=({struct Cyc_CfFlowInfo_JoinEnv _tmp23F;_tmp23F.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp28F=({struct Cyc_CfFlowInfo_EscPile*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->places=0;_tmp183;});_tmp23F.pile=_tmp28F;}),_tmp23F.d1=d1,_tmp23F.d2=d2;_tmp23F;});struct Cyc_CfFlowInfo_JoinEnv env=_tmp180;
struct Cyc_Dict_Dict _tmp181=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& env,d1,d2);struct Cyc_Dict_Dict outdict=_tmp181;
struct Cyc_List_List*_tmp182=Cyc_Relations_join_relns(Cyc_Core_heap_region,r1,r2);struct Cyc_List_List*r=_tmp182;
return({struct Cyc_Dict_Dict _tmp290=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outdict);Cyc_CfFlowInfo_ReachableFL(_tmp290,r);});}}}}_LL0:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 922
struct _tuple20 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple20 pr1,struct _tuple20 pr2){
# 927
struct _tuple20 _tmp184=pr1;void*_tmp186;union Cyc_CfFlowInfo_FlowInfo _tmp185;_LL1: _tmp185=_tmp184.f1;_tmp186=_tmp184.f2;_LL2: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp185;void*r1=_tmp186;
struct _tuple20 _tmp187=pr2;void*_tmp189;union Cyc_CfFlowInfo_FlowInfo _tmp188;_LL4: _tmp188=_tmp187.f1;_tmp189=_tmp187.f2;_LL5: {union Cyc_CfFlowInfo_FlowInfo f2=_tmp188;void*r2=_tmp189;
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);
struct _tuple21 _tmp18A=({struct _tuple21 _tmp247;_tmp247.f1=f1,_tmp247.f2=f2,_tmp247.f3=outflow;_tmp247;});struct _tuple21 _stmttmp1E=_tmp18A;struct _tuple21 _tmp18B=_stmttmp1E;struct Cyc_List_List*_tmp18F;struct Cyc_Dict_Dict _tmp18E;struct Cyc_Dict_Dict _tmp18D;struct Cyc_Dict_Dict _tmp18C;if(((_tmp18B.f1).BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple20 _tmp241;_tmp241.f1=outflow,_tmp241.f2=r2;_tmp241;});}else{if(((_tmp18B.f2).BottomFL).tag == 1){_LL9: _LLA:
 return({struct _tuple20 _tmp242;_tmp242.f1=outflow,_tmp242.f2=r1;_tmp242;});}else{if(((_tmp18B.f3).ReachableFL).tag == 2){_LLB: _tmp18C=(((_tmp18B.f1).ReachableFL).val).f1;_tmp18D=(((_tmp18B.f2).ReachableFL).val).f1;_tmp18E=(((_tmp18B.f3).ReachableFL).val).f1;_tmp18F=(((_tmp18B.f3).ReachableFL).val).f2;_LLC: {struct Cyc_Dict_Dict d1=_tmp18C;struct Cyc_Dict_Dict d2=_tmp18D;struct Cyc_Dict_Dict outd=_tmp18E;struct Cyc_List_List*relns=_tmp18F;
# 935
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,r1,r2))return({struct _tuple20 _tmp243;_tmp243.f1=outflow,_tmp243.f2=r2;_tmp243;});
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,r2,r1))return({struct _tuple20 _tmp244;_tmp244.f1=outflow,_tmp244.f2=r1;_tmp244;});{
struct Cyc_CfFlowInfo_JoinEnv _tmp190=({struct Cyc_CfFlowInfo_JoinEnv _tmp246;_tmp246.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp291=({struct Cyc_CfFlowInfo_EscPile*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->places=0;_tmp192;});_tmp246.pile=_tmp291;}),_tmp246.d1=d1,_tmp246.d2=d2;_tmp246;});struct Cyc_CfFlowInfo_JoinEnv env=_tmp190;
void*_tmp191=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env,0,r1,r2);void*outr=_tmp191;
return({struct _tuple20 _tmp245;({union Cyc_CfFlowInfo_FlowInfo _tmp293=({struct Cyc_Dict_Dict _tmp292=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outd);Cyc_CfFlowInfo_ReachableFL(_tmp292,relns);});_tmp245.f1=_tmp293;}),_tmp245.f2=outr;_tmp245;});}}}else{_LLD: _LLE:
# 941
({void*_tmp193=0U;({struct _fat_ptr _tmp294=({const char*_tmp194="join_flow_and_rval: BottomFL outflow";_tag_fat(_tmp194,sizeof(char),37U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp294,_tag_fat(_tmp193,sizeof(void*),0U));});});}}}_LL6:;}}}
# 946
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 949
{struct _tuple12 _tmp195=({struct _tuple12 _tmp248;_tmp248.f1=r1,_tmp248.f2=r2;_tmp248;});struct _tuple12 _stmttmp1F=_tmp195;struct _tuple12 _tmp196=_stmttmp1F;struct _fat_ptr _tmp19C;int _tmp19B;int _tmp19A;struct _fat_ptr _tmp199;int _tmp198;int _tmp197;void*_tmp19E;void*_tmp19D;struct Cyc_CfFlowInfo_Place*_tmp1A0;struct Cyc_CfFlowInfo_Place*_tmp19F;void*_tmp1A4;struct Cyc_Absyn_Vardecl*_tmp1A3;void*_tmp1A2;struct Cyc_Absyn_Vardecl*_tmp1A1;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 8U){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 8U){_LL1: _tmp1A1=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1;_tmp1A2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f2;_tmp1A3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1;_tmp1A4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f2;_LL2: {struct Cyc_Absyn_Vardecl*n1=_tmp1A1;void*r1=_tmp1A2;struct Cyc_Absyn_Vardecl*n2=_tmp1A3;void*r2=_tmp1A4;
# 951
return n1 == n2 && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{_LL3: _LL4:
 goto _LL6;}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 8U){_LL5: _LL6:
 return 0;}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 4U){if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 4U){_LL7: _tmp19F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1;_tmp1A0=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1;_LL8: {struct Cyc_CfFlowInfo_Place*p1=_tmp19F;struct Cyc_CfFlowInfo_Place*p2=_tmp1A0;
return Cyc_CfFlowInfo_place_cmp(p1,p2)== 0;}}else{_LL9: _LLA:
 goto _LLC;}}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 4U){_LLB: _LLC:
 return 0;}else{if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 5U){if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 5U){_LLD: _tmp19D=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1;_tmp19E=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1;_LLE: {void*r1=_tmp19D;void*r2=_tmp19E;
return Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{_LLF: _LL10:
 goto _LL12;}}else{if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 5U){_LL11: _LL12:
 return 0;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 6U)switch(*((int*)_tmp196.f2)){case 6U: _LL13: _tmp197=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1).is_union;_tmp198=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1).fieldnum;_tmp199=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f2;_tmp19A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1).is_union;_tmp19B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1).fieldnum;_tmp19C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f2;_LL14: {int is_union1=_tmp197;int fld1=_tmp198;struct _fat_ptr d1=_tmp199;int is_union2=_tmp19A;int fld2=_tmp19B;struct _fat_ptr d2=_tmp19C;
# 962
if(is_union1 && fld1 != fld2)return 0;
if((void**)d1.curr == (void**)d2.curr)return 1;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)d1.curr)[i],*((void**)_check_fat_subscript(d2,sizeof(void*),i))))return 0;}}
return 1;}case 0U: goto _LL15;case 1U: goto _LL17;default: goto _LL1D;}else{switch(*((int*)_tmp196.f2)){case 0U: _LL15: _LL16:
 goto _LL18;case 1U: _LL17: _LL18:
 return 0;default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 3U){if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 3U){_LL19: _LL1A:
 goto _LL0;}else{_LL1B: _LL1C:
 return 0;}}else{_LL1D: _LL1E:
# 977
 goto _LL0;}}}}}}}}}_LL0:;}{
# 979
struct _tuple18 _tmp1A5=({struct _tuple18 _tmp249;({enum Cyc_CfFlowInfo_InitLevel _tmp296=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp249.f1=_tmp296;}),({enum Cyc_CfFlowInfo_InitLevel _tmp295=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp249.f2=_tmp295;});_tmp249;});struct _tuple18 _stmttmp20=_tmp1A5;struct _tuple18 _tmp1A6=_stmttmp20;if(_tmp1A6.f1 == Cyc_CfFlowInfo_AllIL){_LL20: _LL21:
 goto _LL23;}else{if(_tmp1A6.f2 == Cyc_CfFlowInfo_NoneIL){_LL22: _LL23:
 return 1;}else{_LL24: _LL25:
 return 0;}}_LL1F:;}}
# 992
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 994
struct _tuple19 _tmp1A7=({struct _tuple19 _tmp24A;_tmp24A.f1=f1,_tmp24A.f2=f2;_tmp24A;});struct _tuple19 _stmttmp21=_tmp1A7;struct _tuple19 _tmp1A8=_stmttmp21;struct Cyc_List_List*_tmp1AC;struct Cyc_Dict_Dict _tmp1AB;struct Cyc_List_List*_tmp1AA;struct Cyc_Dict_Dict _tmp1A9;if(((_tmp1A8.f1).BottomFL).tag == 1){_LL1: _LL2:
 return 1;}else{if(((_tmp1A8.f2).BottomFL).tag == 1){_LL3: _LL4:
 return 0;}else{_LL5: _tmp1A9=(((_tmp1A8.f1).ReachableFL).val).f1;_tmp1AA=(((_tmp1A8.f1).ReachableFL).val).f2;_tmp1AB=(((_tmp1A8.f2).ReachableFL).val).f1;_tmp1AC=(((_tmp1A8.f2).ReachableFL).val).f2;_LL6: {struct Cyc_Dict_Dict d1=_tmp1A9;struct Cyc_List_List*r1=_tmp1AA;struct Cyc_Dict_Dict d2=_tmp1AB;struct Cyc_List_List*r2=_tmp1AC;
# 998
if(d1.t == d2.t && r1 == r2)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,d1,d2)&&
 Cyc_Relations_relns_approx(r1,r2);}}}_LL0:;}struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 1004
struct _tuple22 Cyc_CfFlowInfo_unname_rval(void*rv){
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp1AD=rv;void*_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1AE;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AD)->tag == 8U){_LL1: _tmp1AE=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AD)->f1;_tmp1AF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AD)->f2;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1AE;void*rv2=_tmp1AF;
# 1010
names=({struct Cyc_List_List*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->hd=vd,_tmp1B0->tl=names;_tmp1B0;});rv=rv2;goto _LL0;}}else{_LL3: _LL4:
# 1012
 done=1;goto _LL0;}_LL0:;}
# 1015
return({struct _tuple22 _tmp24B;_tmp24B.f1=rv,_tmp24B.f2=names;_tmp24B;});}
# 1018
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp1B1=il;if(_tmp1B1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
({void*_tmp1B2=0U;({struct Cyc___cycFILE*_tmp298=Cyc_stderr;struct _fat_ptr _tmp297=({const char*_tmp1B3="uninitialized";_tag_fat(_tmp1B3,sizeof(char),14U);});Cyc_fprintf(_tmp298,_tmp297,_tag_fat(_tmp1B2,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
({void*_tmp1B4=0U;({struct Cyc___cycFILE*_tmp29A=Cyc_stderr;struct _fat_ptr _tmp299=({const char*_tmp1B5="all-initialized";_tag_fat(_tmp1B5,sizeof(char),16U);});Cyc_fprintf(_tmp29A,_tmp299,_tag_fat(_tmp1B4,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 1025
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp1B6=root;void*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Vardecl*_tmp1B9;switch(*((int*)_tmp1B6)){case 0U: _LL1: _tmp1B9=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp1B6)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1B9;
# 1028
({struct Cyc_String_pa_PrintArg_struct _tmp1BC=({struct Cyc_String_pa_PrintArg_struct _tmp24C;_tmp24C.tag=0U,({struct _fat_ptr _tmp29B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp24C.f1=_tmp29B;});_tmp24C;});void*_tmp1BA[1U];_tmp1BA[0]=& _tmp1BC;({struct Cyc___cycFILE*_tmp29D=Cyc_stderr;struct _fat_ptr _tmp29C=({const char*_tmp1BB="Root(%s)";_tag_fat(_tmp1BB,sizeof(char),9U);});Cyc_fprintf(_tmp29D,_tmp29C,_tag_fat(_tmp1BA,sizeof(void*),1U));});});goto _LL0;}case 1U: _LL3: _tmp1B7=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp1B6)->f1;_tmp1B8=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp1B6)->f2;_LL4: {struct Cyc_Absyn_Exp*e=_tmp1B7;void*t=_tmp1B8;
# 1030
({struct Cyc_String_pa_PrintArg_struct _tmp1BF=({struct Cyc_String_pa_PrintArg_struct _tmp24E;_tmp24E.tag=0U,({
struct _fat_ptr _tmp29E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp24E.f1=_tmp29E;});_tmp24E;});struct Cyc_String_pa_PrintArg_struct _tmp1C0=({struct Cyc_String_pa_PrintArg_struct _tmp24D;_tmp24D.tag=0U,({struct _fat_ptr _tmp29F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp24D.f1=_tmp29F;});_tmp24D;});void*_tmp1BD[2U];_tmp1BD[0]=& _tmp1BF,_tmp1BD[1]=& _tmp1C0;({struct Cyc___cycFILE*_tmp2A1=Cyc_stderr;struct _fat_ptr _tmp2A0=({const char*_tmp1BE="MallocPt(%s,%s)";_tag_fat(_tmp1BE,sizeof(char),16U);});Cyc_fprintf(_tmp2A1,_tmp2A0,_tag_fat(_tmp1BD,sizeof(void*),2U));});});goto _LL0;}default: _LL5: _LL6:
# 1033
({void*_tmp1C1=0U;({struct Cyc___cycFILE*_tmp2A3=Cyc_stderr;struct _fat_ptr _tmp2A2=({const char*_tmp1C2="InitParam(_,_)";_tag_fat(_tmp1C2,sizeof(char),15U);});Cyc_fprintf(_tmp2A3,_tmp2A2,_tag_fat(_tmp1C1,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 1037
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){
{struct Cyc_List_List*x=p;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
void*_tmp1C3=(void*)x->hd;void*_stmttmp22=_tmp1C3;void*_tmp1C4=_stmttmp22;int _tmp1C5;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp1C4)->tag == 0U){_LL1: _tmp1C5=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp1C4)->f1;_LL2: {int i=_tmp1C5;
# 1041
({struct Cyc_Int_pa_PrintArg_struct _tmp1C8=({struct Cyc_Int_pa_PrintArg_struct _tmp24F;_tmp24F.tag=1U,_tmp24F.f1=(unsigned long)i;_tmp24F;});void*_tmp1C6[1U];_tmp1C6[0]=& _tmp1C8;({struct Cyc___cycFILE*_tmp2A5=Cyc_stderr;struct _fat_ptr _tmp2A4=({const char*_tmp1C7=".%d";_tag_fat(_tmp1C7,sizeof(char),4U);});Cyc_fprintf(_tmp2A5,_tmp2A4,_tag_fat(_tmp1C6,sizeof(void*),1U));});});
goto _LL0;}}else{_LL3: _LL4:
# 1044
 if(x->tl != 0){
void*_tmp1C9=(void*)((struct Cyc_List_List*)_check_null(x->tl))->hd;void*_stmttmp23=_tmp1C9;void*_tmp1CA=_stmttmp23;int _tmp1CB;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp1CA)->tag == 0U){_LL6: _tmp1CB=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp1CA)->f1;_LL7: {int i=_tmp1CB;
# 1047
x=x->tl;
({struct Cyc_Int_pa_PrintArg_struct _tmp1CE=({struct Cyc_Int_pa_PrintArg_struct _tmp250;_tmp250.tag=1U,_tmp250.f1=(unsigned long)i;_tmp250;});void*_tmp1CC[1U];_tmp1CC[0]=& _tmp1CE;({struct Cyc___cycFILE*_tmp2A7=Cyc_stderr;struct _fat_ptr _tmp2A6=({const char*_tmp1CD="->%d";_tag_fat(_tmp1CD,sizeof(char),5U);});Cyc_fprintf(_tmp2A7,_tmp2A6,_tag_fat(_tmp1CC,sizeof(void*),1U));});});
continue;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1053
({void*_tmp1CF=0U;({struct Cyc___cycFILE*_tmp2A9=Cyc_stderr;struct _fat_ptr _tmp2A8=({const char*_tmp1D0="*";_tag_fat(_tmp1D0,sizeof(char),2U);});Cyc_fprintf(_tmp2A9,_tmp2A8,_tag_fat(_tmp1CF,sizeof(void*),0U));});});}_LL0:;}}
# 1056
({void*_tmp1D1=0U;({struct Cyc___cycFILE*_tmp2AB=Cyc_stderr;struct _fat_ptr _tmp2AA=({const char*_tmp1D2=" ";_tag_fat(_tmp1D2,sizeof(char),2U);});Cyc_fprintf(_tmp2AB,_tmp2AA,_tag_fat(_tmp1D1,sizeof(void*),0U));});});}
# 1059
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);
Cyc_CfFlowInfo_print_path(p->path);}
# 1064
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp1D3=1;int first=_tmp1D3;
while(x != 0){
if(!first){({void*_tmp1D4=0U;({struct Cyc___cycFILE*_tmp2AD=Cyc_stderr;struct _fat_ptr _tmp2AC=({const char*_tmp1D5=", ";_tag_fat(_tmp1D5,sizeof(char),3U);});Cyc_fprintf(_tmp2AD,_tmp2AC,_tag_fat(_tmp1D4,sizeof(void*),0U));});});first=0;}
pr(x->hd);
x=x->tl;}
# 1071
({void*_tmp1D6=0U;({struct Cyc___cycFILE*_tmp2AF=Cyc_stderr;struct _fat_ptr _tmp2AE=({const char*_tmp1D7="\n";_tag_fat(_tmp1D7,sizeof(char),2U);});Cyc_fprintf(_tmp2AF,_tmp2AE,_tag_fat(_tmp1D6,sizeof(void*),0U));});});}
# 1074
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp1D8=rval;void*_tmp1DA;struct Cyc_Absyn_Vardecl*_tmp1D9;void*_tmp1DD;int _tmp1DC;struct Cyc_Absyn_Exp*_tmp1DB;struct _fat_ptr _tmp1E0;int _tmp1DF;int _tmp1DE;void*_tmp1E1;struct Cyc_CfFlowInfo_Place*_tmp1E2;enum Cyc_CfFlowInfo_InitLevel _tmp1E3;enum Cyc_CfFlowInfo_InitLevel _tmp1E4;switch(*((int*)_tmp1D8)){case 0U: _LL1: _LL2:
({void*_tmp1E5=0U;({struct Cyc___cycFILE*_tmp2B1=Cyc_stderr;struct _fat_ptr _tmp2B0=({const char*_tmp1E6="Zero";_tag_fat(_tmp1E6,sizeof(char),5U);});Cyc_fprintf(_tmp2B1,_tmp2B0,_tag_fat(_tmp1E5,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp1E7=0U;({struct Cyc___cycFILE*_tmp2B3=Cyc_stderr;struct _fat_ptr _tmp2B2=({const char*_tmp1E8="NotZeroAll";_tag_fat(_tmp1E8,sizeof(char),11U);});Cyc_fprintf(_tmp2B3,_tmp2B2,_tag_fat(_tmp1E7,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp1E4=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_LL6: {enum Cyc_CfFlowInfo_InitLevel il=_tmp1E4;
({void*_tmp1E9=0U;({struct Cyc___cycFILE*_tmp2B5=Cyc_stderr;struct _fat_ptr _tmp2B4=({const char*_tmp1EA="Unknown(";_tag_fat(_tmp1EA,sizeof(char),9U);});Cyc_fprintf(_tmp2B5,_tmp2B4,_tag_fat(_tmp1E9,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_initlevel(il);
({void*_tmp1EB=0U;({struct Cyc___cycFILE*_tmp2B7=Cyc_stderr;struct _fat_ptr _tmp2B6=({const char*_tmp1EC=")";_tag_fat(_tmp1EC,sizeof(char),2U);});Cyc_fprintf(_tmp2B7,_tmp2B6,_tag_fat(_tmp1EB,sizeof(void*),0U));});});goto _LL0;}case 3U: _LL7: _tmp1E3=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_LL8: {enum Cyc_CfFlowInfo_InitLevel il=_tmp1E3;
({void*_tmp1ED=0U;({struct Cyc___cycFILE*_tmp2B9=Cyc_stderr;struct _fat_ptr _tmp2B8=({const char*_tmp1EE="Esc(";_tag_fat(_tmp1EE,sizeof(char),5U);});Cyc_fprintf(_tmp2B9,_tmp2B8,_tag_fat(_tmp1ED,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_initlevel(il);
({void*_tmp1EF=0U;({struct Cyc___cycFILE*_tmp2BB=Cyc_stderr;struct _fat_ptr _tmp2BA=({const char*_tmp1F0=")";_tag_fat(_tmp1F0,sizeof(char),2U);});Cyc_fprintf(_tmp2BB,_tmp2BA,_tag_fat(_tmp1EF,sizeof(void*),0U));});});goto _LL0;}case 4U: _LL9: _tmp1E2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_LLA: {struct Cyc_CfFlowInfo_Place*p=_tmp1E2;
({void*_tmp1F1=0U;({struct Cyc___cycFILE*_tmp2BD=Cyc_stderr;struct _fat_ptr _tmp2BC=({const char*_tmp1F2="AddrOf(";_tag_fat(_tmp1F2,sizeof(char),8U);});Cyc_fprintf(_tmp2BD,_tmp2BC,_tag_fat(_tmp1F1,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_place(p);
({void*_tmp1F3=0U;({struct Cyc___cycFILE*_tmp2BF=Cyc_stderr;struct _fat_ptr _tmp2BE=({const char*_tmp1F4=")";_tag_fat(_tmp1F4,sizeof(char),2U);});Cyc_fprintf(_tmp2BF,_tmp2BE,_tag_fat(_tmp1F3,sizeof(void*),0U));});});goto _LL0;}case 5U: _LLB: _tmp1E1=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_LLC: {void*r=_tmp1E1;
({void*_tmp1F5=0U;({struct Cyc___cycFILE*_tmp2C1=Cyc_stderr;struct _fat_ptr _tmp2C0=({const char*_tmp1F6="UniquePtr(";_tag_fat(_tmp1F6,sizeof(char),11U);});Cyc_fprintf(_tmp2C1,_tmp2C0,_tag_fat(_tmp1F5,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_absrval(r);
({void*_tmp1F7=0U;({struct Cyc___cycFILE*_tmp2C3=Cyc_stderr;struct _fat_ptr _tmp2C2=({const char*_tmp1F8=")";_tag_fat(_tmp1F8,sizeof(char),2U);});Cyc_fprintf(_tmp2C3,_tmp2C2,_tag_fat(_tmp1F7,sizeof(void*),0U));});});goto _LL0;}case 6U: _LLD: _tmp1DE=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1).is_union;_tmp1DF=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1).fieldnum;_tmp1E0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f2;_LLE: {int is_union=_tmp1DE;int f=_tmp1DF;struct _fat_ptr d=_tmp1E0;
# 1087
if(is_union){
({void*_tmp1F9=0U;({struct Cyc___cycFILE*_tmp2C5=Cyc_stderr;struct _fat_ptr _tmp2C4=({const char*_tmp1FA="AggrUnion{";_tag_fat(_tmp1FA,sizeof(char),11U);});Cyc_fprintf(_tmp2C5,_tmp2C4,_tag_fat(_tmp1F9,sizeof(void*),0U));});});
if(f != -1)
({struct Cyc_Int_pa_PrintArg_struct _tmp1FD=({struct Cyc_Int_pa_PrintArg_struct _tmp251;_tmp251.tag=1U,_tmp251.f1=(unsigned long)f;_tmp251;});void*_tmp1FB[1U];_tmp1FB[0]=& _tmp1FD;({struct Cyc___cycFILE*_tmp2C7=Cyc_stderr;struct _fat_ptr _tmp2C6=({const char*_tmp1FC="tag == %d;";_tag_fat(_tmp1FC,sizeof(char),11U);});Cyc_fprintf(_tmp2C7,_tmp2C6,_tag_fat(_tmp1FB,sizeof(void*),1U));});});}else{
# 1092
({void*_tmp1FE=0U;({struct Cyc___cycFILE*_tmp2C9=Cyc_stderr;struct _fat_ptr _tmp2C8=({const char*_tmp1FF="AggrStruct{";_tag_fat(_tmp1FF,sizeof(char),12U);});Cyc_fprintf(_tmp2C9,_tmp2C8,_tag_fat(_tmp1FE,sizeof(void*),0U));});});}
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)d.curr)[i]);
if((unsigned)(i + 1)< _get_fat_size(d,sizeof(void*)))({void*_tmp200=0U;({struct Cyc___cycFILE*_tmp2CB=Cyc_stderr;struct _fat_ptr _tmp2CA=({const char*_tmp201=",";_tag_fat(_tmp201,sizeof(char),2U);});Cyc_fprintf(_tmp2CB,_tmp2CA,_tag_fat(_tmp200,sizeof(void*),0U));});});}}
# 1097
({void*_tmp202=0U;({struct Cyc___cycFILE*_tmp2CD=Cyc_stderr;struct _fat_ptr _tmp2CC=({const char*_tmp203="}";_tag_fat(_tmp203,sizeof(char),2U);});Cyc_fprintf(_tmp2CD,_tmp2CC,_tag_fat(_tmp202,sizeof(void*),0U));});});
goto _LL0;}case 7U: _LLF: _tmp1DB=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_tmp1DC=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f2;_tmp1DD=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f3;_LL10: {struct Cyc_Absyn_Exp*e=_tmp1DB;int i=_tmp1DC;void*r=_tmp1DD;
# 1100
({struct Cyc_String_pa_PrintArg_struct _tmp206=({struct Cyc_String_pa_PrintArg_struct _tmp253;_tmp253.tag=0U,({struct _fat_ptr _tmp2CE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp253.f1=_tmp2CE;});_tmp253;});struct Cyc_Int_pa_PrintArg_struct _tmp207=({struct Cyc_Int_pa_PrintArg_struct _tmp252;_tmp252.tag=1U,_tmp252.f1=(unsigned long)i;_tmp252;});void*_tmp204[2U];_tmp204[0]=& _tmp206,_tmp204[1]=& _tmp207;({struct Cyc___cycFILE*_tmp2D0=Cyc_stderr;struct _fat_ptr _tmp2CF=({const char*_tmp205="[Consumed(%s,%d,";_tag_fat(_tmp205,sizeof(char),17U);});Cyc_fprintf(_tmp2D0,_tmp2CF,_tag_fat(_tmp204,sizeof(void*),2U));});});
Cyc_CfFlowInfo_print_absrval(r);({void*_tmp208=0U;({struct Cyc___cycFILE*_tmp2D2=Cyc_stderr;struct _fat_ptr _tmp2D1=({const char*_tmp209=")]";_tag_fat(_tmp209,sizeof(char),3U);});Cyc_fprintf(_tmp2D2,_tmp2D1,_tag_fat(_tmp208,sizeof(void*),0U));});});
goto _LL0;}default: _LL11: _tmp1D9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_tmp1DA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f2;_LL12: {struct Cyc_Absyn_Vardecl*n=_tmp1D9;void*r=_tmp1DA;
# 1104
({struct Cyc_String_pa_PrintArg_struct _tmp20C=({struct Cyc_String_pa_PrintArg_struct _tmp254;_tmp254.tag=0U,({struct _fat_ptr _tmp2D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n->name));_tmp254.f1=_tmp2D3;});_tmp254;});void*_tmp20A[1U];_tmp20A[0]=& _tmp20C;({struct Cyc___cycFILE*_tmp2D5=Cyc_stderr;struct _fat_ptr _tmp2D4=({const char*_tmp20B="[NamedLocation(%s,";_tag_fat(_tmp20B,sizeof(char),19U);});Cyc_fprintf(_tmp2D5,_tmp2D4,_tag_fat(_tmp20A,sizeof(void*),1U));});});
Cyc_CfFlowInfo_print_absrval(r);({void*_tmp20D=0U;({struct Cyc___cycFILE*_tmp2D7=Cyc_stderr;struct _fat_ptr _tmp2D6=({const char*_tmp20E=")]";_tag_fat(_tmp20E,sizeof(char),3U);});Cyc_fprintf(_tmp2D7,_tmp2D6,_tag_fat(_tmp20D,sizeof(void*),0U));});});
goto _LL0;}}_LL0:;}
# 1110
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp20F=0U;({struct Cyc___cycFILE*_tmp2D9=Cyc_stderr;struct _fat_ptr _tmp2D8=({const char*_tmp210="    ";_tag_fat(_tmp210,sizeof(char),5U);});Cyc_fprintf(_tmp2D9,_tmp2D8,_tag_fat(_tmp20F,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp211=0U;({struct Cyc___cycFILE*_tmp2DB=Cyc_stderr;struct _fat_ptr _tmp2DA=({const char*_tmp212=" --> ";_tag_fat(_tmp212,sizeof(char),6U);});Cyc_fprintf(_tmp2DB,_tmp2DA,_tag_fat(_tmp211,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp213=0U;({struct Cyc___cycFILE*_tmp2DD=Cyc_stderr;struct _fat_ptr _tmp2DC=({const char*_tmp214="\n";_tag_fat(_tmp214,sizeof(char),2U);});Cyc_fprintf(_tmp2DD,_tmp2DC,_tag_fat(_tmp213,sizeof(void*),0U));});});}
# 1118
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1122
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp215=f;struct Cyc_List_List*_tmp217;struct Cyc_Dict_Dict _tmp216;if((_tmp215.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp218=0U;({struct Cyc___cycFILE*_tmp2DF=Cyc_stderr;struct _fat_ptr _tmp2DE=({const char*_tmp219="  BottomFL\n";_tag_fat(_tmp219,sizeof(char),12U);});Cyc_fprintf(_tmp2DF,_tmp2DE,_tag_fat(_tmp218,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp216=((_tmp215.ReachableFL).val).f1;_tmp217=((_tmp215.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict fd=_tmp216;struct Cyc_List_List*rlns=_tmp217;
# 1126
({void*_tmp21A=0U;({struct Cyc___cycFILE*_tmp2E1=Cyc_stderr;struct _fat_ptr _tmp2E0=({const char*_tmp21B="  ReachableFL:\n";_tag_fat(_tmp21B,sizeof(char),16U);});Cyc_fprintf(_tmp2E1,_tmp2E0,_tag_fat(_tmp21A,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_flowdict(fd);
({void*_tmp21C=0U;({struct Cyc___cycFILE*_tmp2E3=Cyc_stderr;struct _fat_ptr _tmp2E2=({const char*_tmp21D="\n  Relations: ";_tag_fat(_tmp21D,sizeof(char),15U);});Cyc_fprintf(_tmp2E3,_tmp2E2,_tag_fat(_tmp21C,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,rlns);
({void*_tmp21E=0U;({struct Cyc___cycFILE*_tmp2E5=Cyc_stderr;struct _fat_ptr _tmp2E4=({const char*_tmp21F="\n";_tag_fat(_tmp21F,sizeof(char),2U);});Cyc_fprintf(_tmp2E5,_tmp2E4,_tag_fat(_tmp21E,sizeof(void*),0U));});});
goto _LL0;}}_LL0:;}

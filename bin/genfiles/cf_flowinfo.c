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
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 147 "dict.h"
extern void Cyc_Dict_iter(void(*)(void*,void*),struct Cyc_Dict_Dict);
# 212
extern struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 222
extern int Cyc_Dict_forall_intersect(int(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 283
extern struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
# 837 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 864
void*Cyc_Absyn_compress(void*);
# 895
extern void*Cyc_Absyn_void_type;
# 1121
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1123
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 114
int Cyc_Relations_relns_approx(struct Cyc_List_List*,struct Cyc_List_List*);
# 116
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 118
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 58 "tcutil.h"
int Cyc_Tcutil_is_bits_only_type(void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 137
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*,int);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 33 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned,struct _fat_ptr,struct _fat_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};char Cyc_CfFlowInfo_IsZero[7U]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8U]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9U]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 139 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 156
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*,struct _fat_ptr*);
# 158
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 161
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int,void*);
# 163
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*,void*,struct Cyc_Absyn_Exp*,int,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*,int,void*,int*);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*,void*);
# 168
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*);
# 171
int Cyc_CfFlowInfo_is_init_pointer(void*);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 174
void Cyc_CfFlowInfo_print_absrval(void*);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel);
void Cyc_CfFlowInfo_print_root(void*);
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*);
# 180
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict);
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
return({union Cyc_CfFlowInfo_AbsLVal _tmp1D2;(_tmp1D2.PlaceL).tag=1U,(_tmp1D2.PlaceL).val=x;_tmp1D2;});}
# 50
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void){
return({union Cyc_CfFlowInfo_AbsLVal _tmp1D3;(_tmp1D3.UnknownL).tag=2U,(_tmp1D3.UnknownL).val=0;_tmp1D3;});}
# 53
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void){return({union Cyc_CfFlowInfo_FlowInfo _tmp1D4;(_tmp1D4.BottomFL).tag=1U,(_tmp1D4.BottomFL).val=0;_tmp1D4;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp1D5;(_tmp1D5.ReachableFL).tag=2U,((_tmp1D5.ReachableFL).val).f1=fd,((_tmp1D5.ReachableFL).val).f2=r;_tmp1D5;});}
# 58
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp8=_cycalloc(sizeof(*_tmp8));
({void*_tmp20F=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->tag=0;_tmp0;});_tmp8->zero=_tmp20F;}),({
void*_tmp20E=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=1;_tmp1;});_tmp8->notzeroall=_tmp20E;}),({
void*_tmp20D=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=2,_tmp2->f1=Cyc_CfFlowInfo_NoneIL;_tmp2;});_tmp8->unknown_none=_tmp20D;}),({
void*_tmp20C=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=2,_tmp3->f1=Cyc_CfFlowInfo_AllIL;_tmp3;});_tmp8->unknown_all=_tmp20C;}),({
void*_tmp20B=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=3,_tmp4->f1=Cyc_CfFlowInfo_NoneIL;_tmp4;});_tmp8->esc_none=_tmp20B;}),({
void*_tmp20A=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=3,_tmp5->f1=Cyc_CfFlowInfo_AllIL;_tmp5;});_tmp8->esc_all=_tmp20A;}),({
struct Cyc_Dict_Dict _tmp209=((struct Cyc_Dict_Dict(*)(int(*)(void*,void*)))Cyc_Dict_empty)(Cyc_CfFlowInfo_root_cmp);_tmp8->mt_flowdict=_tmp209;}),({
struct Cyc_CfFlowInfo_Place*_tmp208=({struct Cyc_CfFlowInfo_Place*_tmp7=_cycalloc(sizeof(*_tmp7));({void*_tmp207=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=1,_tmp6->f1=& dummy_exp,_tmp6->f2=Cyc_Absyn_void_type;_tmp6;});_tmp7->root=_tmp207;}),_tmp7->path=0;_tmp7;});_tmp8->dummy_place=_tmp208;});_tmp8;});}
# 74
struct _fat_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp9=*place;struct Cyc_CfFlowInfo_Place _stmttmp0=_tmp9;struct Cyc_CfFlowInfo_Place _tmpA=_stmttmp0;void*_tmpC;void*_tmpB;_tmpB=_tmpA.root;_tmpC=_tmpA.path;{void*root=_tmpB;struct Cyc_List_List*fields=_tmpC;
void*_tmpD=root;void*_tmpE;if(*((int*)_tmpD)== 0){_tmpE=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpE;
# 78
if(fields == 0)
return Cyc_Absynpp_qvar2string(vd->name);else{
# 81
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmp1D6;_tmp1D6.tag=0,({struct _fat_ptr _tmp210=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1D6.f1=_tmp210;});_tmp1D6;});void*_tmpF[1];_tmpF[0]=& _tmp11;({struct _fat_ptr _tmp211=({const char*_tmp10="reachable from %s";_tag_fat(_tmp10,sizeof(char),18U);});Cyc_aprintf(_tmp211,_tag_fat(_tmpF,sizeof(void*),1));});});}}}else{
({void*_tmp12=0U;({struct _fat_ptr _tmp212=({const char*_tmp13="error locations not for VarRoots";_tag_fat(_tmp13,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp212,_tag_fat(_tmp12,sizeof(void*),0));});});};}}
# 87
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f){
int n=0;
for(1;(unsigned)fs;fs=fs->tl){
struct _fat_ptr*_tmp14=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;struct _fat_ptr*f2=_tmp14;
if(Cyc_strptrcmp(f2,f)== 0)return n;
n=n + 1;}
# 94
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp1D7;_tmp1D7.tag=0,_tmp1D7.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp1D7;});void*_tmp15[1];_tmp15[0]=& _tmp17;({struct _fat_ptr _tmp213=({const char*_tmp16="get_field_index_fs failed: %s";_tag_fat(_tmp16,sizeof(char),30U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp213,_tag_fat(_tmp15,sizeof(void*),1));});});}
# 97
int Cyc_CfFlowInfo_get_field_index(void*t,struct _fat_ptr*f){
void*_tmp18=Cyc_Absyn_compress(t);void*_stmttmp1=_tmp18;void*_tmp19=_stmttmp1;void*_tmp1A;union Cyc_Absyn_AggrInfo _tmp1B;switch(*((int*)_tmp19)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19)->f1)== 20){_tmp1B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp1B;
# 100
struct Cyc_List_List*_tmp1C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(info))->impl))->fields;struct Cyc_List_List*fs=_tmp1C;
_tmp1A=fs;goto _LL4;}}else{goto _LL5;}case 7: _tmp1A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp19)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A;
# 103
return Cyc_CfFlowInfo_get_field_index_fs(fs,f);}default: _LL5:
# 105
({struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp1D8;_tmp1D8.tag=0,({struct _fat_ptr _tmp214=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp1D8.f1=_tmp214;});_tmp1D8;});void*_tmp1D[1];_tmp1D[0]=& _tmp1F;({struct _fat_ptr _tmp215=({const char*_tmp1E="get_field_index failed: %s";_tag_fat(_tmp1E,sizeof(char),27U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp215,_tag_fat(_tmp1D,sizeof(void*),1));});});};}struct _tuple12{void*f1;void*f2;};
# 109
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple12 _tmp20=({struct _tuple12 _tmp1D9;_tmp1D9.f1=r1,_tmp1D9.f2=r2;_tmp1D9;});struct _tuple12 _stmttmp2=_tmp20;struct _tuple12 _tmp21=_stmttmp2;int _tmp23;int _tmp22;void*_tmp25;void*_tmp24;if(*((int*)_tmp21.f1)== 0){if(*((int*)_tmp21.f2)== 0){_tmp24=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp25=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;{struct Cyc_Absyn_Vardecl*vd1=_tmp24;struct Cyc_Absyn_Vardecl*vd2=_tmp25;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_tmp21.f2)== 0)
return 1;else{if(*((int*)_tmp21.f1)== 1){if(*((int*)_tmp21.f2)== 1){_tmp24=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp25=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp24;struct Cyc_Absyn_Exp*e2=_tmp25;
return(int)e1 - (int)e2;}}else{
return -1;}}else{if(*((int*)_tmp21.f2)== 1)
return 1;else{_tmp22=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp23=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;{int i1=_tmp22;int i2=_tmp23;
return i1 - i2;}}}}};}}
# 123
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){
struct _tuple12 _tmp26=({struct _tuple12 _tmp1DA;_tmp1DA.f1=p1,_tmp1DA.f2=p2;_tmp1DA;});struct _tuple12 _stmttmp3=_tmp26;struct _tuple12 _tmp27=_stmttmp3;int _tmp29;int _tmp28;if(*((int*)_tmp27.f1)== 0){if(*((int*)_tmp27.f2)== 0){_tmp28=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp27.f1)->f1;_tmp29=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp27.f2)->f1;{int i1=_tmp28;int i2=_tmp29;
# 126
if(i1 == i2)return 0;
if(i1 < i2)return -1;else{
return 1;}}}else{
# 131
return 1;}}else{if(*((int*)_tmp27.f2)== 1)
# 129
return 0;else{
return -1;}};}
# 135
static int Cyc_CfFlowInfo_path_cmp(struct Cyc_List_List*path1,struct Cyc_List_List*path2){
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_CfFlowInfo_pathcon_cmp,path1,path2);}
# 139
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp2A=*p1;struct Cyc_CfFlowInfo_Place _stmttmp4=_tmp2A;struct Cyc_CfFlowInfo_Place _tmp2B=_stmttmp4;void*_tmp2D;void*_tmp2C;_tmp2C=_tmp2B.root;_tmp2D=_tmp2B.path;{void*root1=_tmp2C;struct Cyc_List_List*path1=_tmp2D;
struct Cyc_CfFlowInfo_Place _tmp2E=*p2;struct Cyc_CfFlowInfo_Place _stmttmp5=_tmp2E;struct Cyc_CfFlowInfo_Place _tmp2F=_stmttmp5;void*_tmp31;void*_tmp30;_tmp30=_tmp2F.root;_tmp31=_tmp2F.path;{void*root2=_tmp30;struct Cyc_List_List*path2=_tmp31;
int i=Cyc_CfFlowInfo_root_cmp(root1,root2);
if(i != 0)
return i;
return Cyc_CfFlowInfo_path_cmp(path1,path2);}}}}
# 160 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,int,int,void*,void*);
# 165
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 171
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*))Cyc_List_length)(fs);
struct _fat_ptr d=({unsigned _tmp37=sz;void**_tmp36=_cycalloc(_check_times(_tmp37,sizeof(void*)));({{unsigned _tmp1DB=sz;unsigned i;for(i=0;i < _tmp1DB;++ i){_tmp36[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp36,sizeof(void*),_tmp37);});
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp32=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_stmttmp6=_tmp32;struct Cyc_Absyn_Aggrfield*_tmp33=_stmttmp6;void*_tmp35;void*_tmp34;_tmp34=_tmp33->name;_tmp35=_tmp33->type;{struct _fat_ptr*n=_tmp34;void*t2=_tmp35;
if(_get_fat_size(*n,sizeof(char))!= (unsigned)1)
({void*_tmp216=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,t2,leafval);*((void**)_check_fat_subscript(d,sizeof(void*),i))=_tmp216;});}}}
# 179
return d;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 183
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple13*env,struct Cyc_Absyn_Aggrfield*f){
# 187
struct _tuple13*_tmp38=env;void*_tmp3A;void*_tmp39;_tmp39=_tmp38->f1;_tmp3A=_tmp38->f2;{struct _RegionHandle*t=_tmp39;struct Cyc_List_List*inst=_tmp3A;
void*_tmp3B=Cyc_Tcutil_rsubstitute(t,inst,f->type);void*new_typ=_tmp3B;
struct Cyc_Absyn_Exp*_tmp3C=f->requires_clause;struct Cyc_Absyn_Exp*r=_tmp3C;
struct Cyc_Absyn_Exp*_tmp3D=r == 0?0: Cyc_Tcutil_rsubsexp(t,inst,r);struct Cyc_Absyn_Exp*new_r=_tmp3D;
return({struct Cyc_Absyn_Aggrfield*_tmp3E=_region_malloc(t,sizeof(*_tmp3E));_tmp3E->name=f->name,_tmp3E->tq=f->tq,_tmp3E->type=new_typ,_tmp3E->width=f->width,_tmp3E->attributes=f->attributes,_tmp3E->requires_clause=new_r;_tmp3E;});}}struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 198
static struct _fat_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 204
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
# 208
{struct Cyc_List_List*inst=0;
for(1;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp40=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp40;
void*_tmp41=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;void*t=_tmp41;
{struct Cyc_Absyn_Kind*_tmp42=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_stmttmp7=_tmp42;struct Cyc_Absyn_Kind*_tmp43=_stmttmp7;switch((int)((struct Cyc_Absyn_Kind*)_tmp43)->kind){case Cyc_Absyn_RgnKind:
 goto _LL4;case Cyc_Absyn_EffKind: _LL4:
 continue;default:
 goto _LL0;}_LL0:;}
# 217
inst=({struct Cyc_List_List*_tmp45=_region_malloc(temp,sizeof(*_tmp45));({struct _tuple14*_tmp217=({struct _tuple14*_tmp44=_region_malloc(temp,sizeof(*_tmp44));_tmp44->f1=tv,_tmp44->f2=t;_tmp44;});_tmp45->hd=_tmp217;}),_tmp45->tl=inst;_tmp45;});}
# 219
if(inst != 0){
struct _tuple13 _tmp46=({struct _tuple13 _tmp1DC;_tmp1DC.f1=temp,({struct Cyc_List_List*_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(inst);_tmp1DC.f2=_tmp218;});_tmp1DC;});struct _tuple13 env=_tmp46;
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& env,fs);
struct _fat_ptr _tmp47=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp47;}else{
# 224
struct _fat_ptr _tmp48=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp48;}}
# 208
;_pop_region();}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 228
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 234
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 236
{void*_tmp49=Cyc_Absyn_compress(t);void*_stmttmp8=_tmp49;void*_tmp4A=_stmttmp8;void*_tmp4C;enum Cyc_Absyn_AggrKind _tmp4D;union Cyc_Absyn_AggrInfo _tmp4E;void*_tmp4B;switch(*((int*)_tmp4A)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f1)){case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f1)->f1).KnownDatatypefield).tag == 2){_tmp4B=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypefield*tuf=_tmp4B;
# 238
if(tuf->typs == 0)
return leafval;
_tmp4B=tuf->typs;goto _LL4;}}else{goto _LLF;}case 20: _tmp4E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f1)->f1;_tmp4B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f2;{union Cyc_Absyn_AggrInfo info=_tmp4E;struct Cyc_List_List*targs=_tmp4B;
# 250
struct Cyc_Absyn_Aggrdecl*_tmp52=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp52;
if(ad->impl == 0)
goto _LL0;{
struct Cyc_List_List*_tmp53=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp53;
if(targs == 0){_tmp4D=ad->kind;_tmp4B=fields;goto _LL8;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=6,(_tmp54->f1).is_union=(int)ad->kind == (int)Cyc_Absyn_UnionA,(_tmp54->f1).fieldnum=- 1,({
struct _fat_ptr _tmp219=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,ad->tvs,targs,fields,(int)ad->kind == (int)Cyc_Absyn_UnionA,leafval);_tmp54->f2=_tmp219;});_tmp54;});}}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f2 != 0){_tmp4B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f2)->hd;{void*t=_tmp4B;
# 273
return leafval;}}else{goto _LLF;}default: goto _LLF;}case 6: _tmp4B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4A)->f1;_LL4: {struct Cyc_List_List*tqts=_tmp4B;
# 242
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*))Cyc_List_length)(tqts);
struct _fat_ptr d=({unsigned _tmp51=sz;void**_tmp50=_cycalloc(_check_times(_tmp51,sizeof(void*)));({{unsigned _tmp1DD=sz;unsigned i;for(i=0;i < _tmp1DD;++ i){_tmp50[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp50,sizeof(void*),_tmp51);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp21A=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2,leafval);*((void**)_check_fat_subscript(d,sizeof(void*),i))=_tmp21A;});
tqts=tqts->tl;}}
# 248
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=6,(_tmp4F->f1).is_union=0,(_tmp4F->f1).fieldnum=- 1,_tmp4F->f2=d;_tmp4F;});}case 7: _tmp4D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4A)->f1;_tmp4B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4A)->f2;_LL8: {enum Cyc_Absyn_AggrKind k=_tmp4D;struct Cyc_List_List*fs=_tmp4B;
# 265
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=6,(_tmp55->f1).is_union=(int)k == (int)Cyc_Absyn_UnionA,(_tmp55->f1).fieldnum=- 1,({
struct _fat_ptr _tmp21B=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)k == (int)Cyc_Absyn_UnionA,leafval);_tmp55->f2=_tmp21B;});_tmp55;});}case 4: _tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).elt_type;_tmp4C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).zero_term;{void*et=_tmp4B;void*zeroterm=_tmp4C;
# 268
if(Cyc_Tcutil_force_type2bool(0,zeroterm))
# 271
return(allow_zeroterm && !no_init_bits_only)&& Cyc_Tcutil_is_bits_only_type(et)?fenv->unknown_all: leafval;
goto _LL0;}case 3: _tmp4B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).ptr_atts).nullable;{void*nbl=_tmp4B;
# 275
if(!Cyc_Tcutil_force_type2bool(0,nbl)){
void*_tmp56=leafval;if(*((int*)_tmp56)== 2){if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp56)->f1 == Cyc_CfFlowInfo_AllIL)
return fenv->notzeroall;else{goto _LL14;}}else{_LL14:
 goto _LL11;}_LL11:;}
# 280
goto _LL0;}default: _LLF:
 goto _LL0;}_LL0:;}
# 284
return !no_init_bits_only && Cyc_Tcutil_is_bits_only_type(t)?fenv->unknown_all: leafval;}
# 287
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 293
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmp57=r;struct _fat_ptr _tmp5B;int _tmp5A;void*_tmp5C;int _tmp59;void*_tmp58;switch(*((int*)_tmp57)){case 7: _tmp58=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp57)->f1;_tmp59=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp57)->f2;_tmp5C=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp57)->f3;{struct Cyc_Absyn_Exp*consumer=_tmp58;int iteration=_tmp59;void*r=_tmp5C;
# 296
if(consumer == e && iteration == env_iteration){
*needs_unconsume=1;
return 0;}
# 300
return 1;}case 6: _tmp59=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57)->f1).is_union;_tmp5A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57)->f1).fieldnum;_tmp5B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57)->f2;{int is_union=_tmp59;int field_no=_tmp5A;struct _fat_ptr d=_tmp5B;
# 302
if(!is_union || field_no == -1){
unsigned _tmp5D=_get_fat_size(d,sizeof(void*));unsigned sz=_tmp5D;
{int i=0;for(0;(unsigned)i < sz;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)d.curr)[i],needs_unconsume))
return 1;}}
# 308
return 0;}else{
# 311
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_fat_subscript(d,sizeof(void*),field_no)),needs_unconsume);}}case 8: _tmp58=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp57)->f2;{void*r=_tmp58;
# 313
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,r,needs_unconsume);}default:
 return 0;};}
# 320
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmp5E=r;struct _fat_ptr _tmp62;struct Cyc_CfFlowInfo_UnionRInfo _tmp61;void*_tmp60;int _tmp63;void*_tmp5F;switch(*((int*)_tmp5E)){case 7: _tmp5F=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f1;_tmp63=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f2;_tmp60=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f3;{struct Cyc_Absyn_Exp*consumer=_tmp5F;int iteration=_tmp63;void*r=_tmp60;
# 323
return r;}case 6: _tmp61=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f1;_tmp62=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f2;{struct Cyc_CfFlowInfo_UnionRInfo uinfo=_tmp61;struct _fat_ptr d=_tmp62;
# 325
unsigned _tmp64=_get_fat_size(d,sizeof(void*));unsigned sz=_tmp64;
int change=0;
struct _fat_ptr d2=({unsigned _tmp67=sz;void**_tmp66=_cycalloc(_check_times(_tmp67,sizeof(void*)));({{unsigned _tmp1DE=sz;unsigned i;for(i=0;i < _tmp1DE;++ i){_tmp66[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmp66,sizeof(void*),_tmp67);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp21C=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)d.curr)[i]);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp21C;});
if(((void**)d2.curr)[i]!= ((void**)d.curr)[i])
change=1;}}
# 333
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=6,_tmp65->f1=uinfo,_tmp65->f2=d2;_tmp65;});else{
return r;}}case 8: _tmp5F=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f1;_tmp60=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp5F;void*r2=_tmp60;
# 337
void*_tmp68=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,r2);void*r3=_tmp68;
if(r3 != r2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=8,_tmp69->f1=n,_tmp69->f2=r3;_tmp69;});else{
return r;}}default:
 return r;};}
# 347
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple12 _tmp6A=({struct _tuple12 _tmp1E1;({void*_tmp21D=Cyc_Absyn_compress(t);_tmp1E1.f1=_tmp21D;}),_tmp1E1.f2=r;_tmp1E1;});struct _tuple12 _stmttmp9=_tmp6A;struct _tuple12 _tmp6B=_stmttmp9;enum Cyc_Absyn_AggrKind _tmp6C;union Cyc_Absyn_AggrInfo _tmp70;struct _fat_ptr _tmp6F;struct Cyc_CfFlowInfo_UnionRInfo _tmp6E;void*_tmp71;void*_tmp6D;if(*((int*)_tmp6B.f2)== 8){_tmp6D=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6B.f2)->f1;_tmp71=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6B.f2)->f2;{struct Cyc_Absyn_Vardecl*s=_tmp6D;void*r2=_tmp71;
# 350
void*_tmp72=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,r2);void*r3=_tmp72;
if(r3 != r2)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=8,_tmp73->f1=s,_tmp73->f2=r3;_tmp73;});else{
return r;}}}else{switch(*((int*)_tmp6B.f1)){case 6: if(*((int*)_tmp6B.f2)== 6){_tmp6D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6B.f1)->f1;_tmp6E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6B.f2)->f1;_tmp6F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6B.f2)->f2;{struct Cyc_List_List*tqts=_tmp6D;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp6E;struct _fat_ptr d=_tmp6F;
# 354
unsigned _tmp74=_get_fat_size(d,sizeof(void*));unsigned sz=_tmp74;
struct _fat_ptr d2=({unsigned _tmp77=sz;void**_tmp76=_cycalloc(_check_times(_tmp77,sizeof(void*)));({{unsigned _tmp1DF=sz;unsigned i;for(i=0;i < _tmp1DF;++ i){_tmp76[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp76,sizeof(void*),_tmp77);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp21E=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2,consumer,iteration,((void**)d.curr)[i]);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp21E;});
tqts=tqts->tl;}}
# 360
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=6,_tmp75->f1=b,_tmp75->f2=d2;_tmp75;});}}else{goto _LL9;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)== 20){if(*((int*)_tmp6B.f2)== 6){_tmp70=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)->f1;_tmp6E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6B.f2)->f1;_tmp6F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6B.f2)->f2;{union Cyc_Absyn_AggrInfo info=_tmp70;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp6E;struct _fat_ptr d=_tmp6F;
# 362
struct Cyc_Absyn_Aggrdecl*_tmp78=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp78;
if(ad->impl == 0)return r;
_tmp6C=ad->kind;_tmp6D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp6E=b;_tmp6F=d;goto _LL8;}}else{goto _LL9;}}else{goto _LL9;}case 7: if(*((int*)_tmp6B.f2)== 6){_tmp6C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6B.f1)->f1;_tmp6D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6B.f1)->f2;_tmp6E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6B.f2)->f1;_tmp6F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6B.f2)->f2;_LL8: {enum Cyc_Absyn_AggrKind k=_tmp6C;struct Cyc_List_List*fs=_tmp6D;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp6E;struct _fat_ptr d=_tmp6F;
# 366
struct _fat_ptr d2=({unsigned _tmp7F=_get_fat_size(d,sizeof(void*));void**_tmp7E=_cycalloc(_check_times(_tmp7F,sizeof(void*)));({{unsigned _tmp1E0=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp1E0;++ i){_tmp7E[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmp7E,sizeof(void*),_tmp7F);});
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*))Cyc_List_length)(fs);
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp79=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_stmttmpA=_tmp79;struct Cyc_Absyn_Aggrfield*_tmp7A=_stmttmpA;void*_tmp7C;void*_tmp7B;_tmp7B=_tmp7A->name;_tmp7C=_tmp7A->type;{struct _fat_ptr*n=_tmp7B;void*t2=_tmp7C;
if(_get_fat_size(*n,sizeof(char))!= (unsigned)1)
({void*_tmp21F=Cyc_CfFlowInfo_make_unique_consumed(fenv,t2,consumer,iteration,*((void**)_check_fat_subscript(d,sizeof(void*),i)));*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp21F;});}}}
# 373
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=6,_tmp7D->f1=b,_tmp7D->f2=d2;_tmp7D;});}}else{goto _LL9;}default: _LL9:
# 375
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=7,_tmp80->f1=consumer,_tmp80->f2=iteration,_tmp80->f3=r;_tmp80;});
return r;}};}struct _tuple16{struct Cyc_CfFlowInfo_Place*f1;unsigned f2;};
# 381
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 383
struct _RegionHandle _tmp81=_new_region("r");struct _RegionHandle*r=& _tmp81;_push_region(r);
{struct _tuple16 elem=({struct _tuple16 _tmp1E2;_tmp1E2.f1=place,_tmp1E2.f2=0U;_tmp1E2;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple16*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp82=elem.f1;struct Cyc_CfFlowInfo_Place*place2=_tmp82;
struct Cyc_CfFlowInfo_Place _tmp83=*place;struct Cyc_CfFlowInfo_Place _stmttmpB=_tmp83;struct Cyc_CfFlowInfo_Place _tmp84=_stmttmpB;void*_tmp86;void*_tmp85;_tmp85=_tmp84.root;_tmp86=_tmp84.path;{void*root1=_tmp85;struct Cyc_List_List*fs1=_tmp86;
struct Cyc_CfFlowInfo_Place _tmp87=*place2;struct Cyc_CfFlowInfo_Place _stmttmpC=_tmp87;struct Cyc_CfFlowInfo_Place _tmp88=_stmttmpC;void*_tmp8A;void*_tmp89;_tmp89=_tmp88.root;_tmp8A=_tmp88.path;{void*root2=_tmp89;struct Cyc_List_List*fs2=_tmp8A;
if(Cyc_CfFlowInfo_root_cmp(root1,root2)!= 0)
continue;
for(1;fs1 != 0 && fs2 != 0;(fs1=fs1->tl,fs2=fs2->tl)){
if((void*)fs1->hd != (void*)fs2->hd)break;}
if(fs1 == 0){
int _tmp8B=1;_npop_handler(0);return _tmp8B;}}}}{
# 397
int _tmp8C=0;_npop_handler(0);return _tmp8C;}}
# 384
;_pop_region();}struct Cyc_CfFlowInfo_EscPile{struct Cyc_List_List*places;};
# 407
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 410
if(!((int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_tmp220=({struct Cyc_List_List*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->hd=place,_tmp8D->tl=pile->places;_tmp8D;});pile->places=_tmp220;});}
# 413
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp8E=r;struct _fat_ptr _tmp90;struct Cyc_CfFlowInfo_UnionRInfo _tmp8F;void*_tmp91;switch(*((int*)_tmp8E)){case 7: _tmp91=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp8E)->f3;{void*r=_tmp91;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 8: _tmp91=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp8E)->f2;{void*r=_tmp91;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 4: _tmp91=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp8E)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp91;
Cyc_CfFlowInfo_add_place(pile,p);return;}case 6: _tmp8F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp8E)->f1;_tmp90=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp8E)->f2;{struct Cyc_CfFlowInfo_UnionRInfo b=_tmp8F;struct _fat_ptr d=_tmp90;
# 419
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)d.curr)[i]);}}
return;}default:
 return;};}
# 429
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){
void*_tmp92=old_val;void*_tmp94;void*_tmp93;struct _fat_ptr _tmp96;int _tmp95;switch(*((int*)_tmp92)){case 6: _tmp95=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp92)->f1).is_union;_tmp96=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp92)->f2;{int is_union=_tmp95;struct _fat_ptr d=_tmp96;
# 432
struct _fat_ptr d2=({unsigned _tmp99=_get_fat_size(d,sizeof(void*));void**_tmp98=_cycalloc(_check_times(_tmp99,sizeof(void*)));({{unsigned _tmp1E3=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp1E3;++ i){({
void*_tmp221=Cyc_CfFlowInfo_insert_place_inner(new_val,((void**)d.curr)[(int)i]);_tmp98[i]=_tmp221;});}}0;});_tag_fat(_tmp98,sizeof(void*),_tmp99);});
# 436
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->tag=6,(_tmp97->f1).is_union=is_union,(_tmp97->f1).fieldnum=- 1,_tmp97->f2=d2;_tmp97;});}case 7: _tmp93=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp92)->f1;_tmp95=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp92)->f2;_tmp94=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp92)->f3;{struct Cyc_Absyn_Exp*e=_tmp93;int i=_tmp95;void*rval=_tmp94;
# 438
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=7,_tmp9A->f1=e,_tmp9A->f2=i,({void*_tmp222=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_tmp9A->f3=_tmp222;});_tmp9A;});}case 8: _tmp93=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp92)->f1;_tmp94=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp92)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp93;void*rval=_tmp94;
# 440
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=8,_tmp9B->f1=n,({void*_tmp223=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_tmp9B->f2=_tmp223;});_tmp9B;});}default:
 return new_val;};}
# 447
static struct _fat_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _fat_ptr d,int n,void*rval){
void*_tmp9C=*((void**)_check_fat_subscript(d,sizeof(void*),n));void*old_rval=_tmp9C;
if(old_rval == rval)return d;{
struct _fat_ptr res=({unsigned _tmp9E=_get_fat_size(d,sizeof(void*));void**_tmp9D=_cycalloc(_check_times(_tmp9E,sizeof(void*)));({{unsigned _tmp1E4=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp1E4;++ i){_tmp9D[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmp9D,sizeof(void*),_tmp9E);});
*((void**)_check_fat_subscript(res,sizeof(void*),n))=rval;
return res;}}struct _tuple17{struct Cyc_List_List*f1;void*f2;};
# 460
static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*path,void*old_val,void*new_val){
# 462
if(path == 0)
return Cyc_CfFlowInfo_insert_place_inner(new_val,old_val);{
struct _tuple17 _tmp9F=({struct _tuple17 _tmp1E5;_tmp1E5.f1=path,_tmp1E5.f2=old_val;_tmp1E5;});struct _tuple17 _stmttmpD=_tmp9F;struct _tuple17 _tmpA0=_stmttmpD;void*_tmpA2;struct _fat_ptr _tmpA5;int _tmpA4;void*_tmpA1;int _tmpA3;if(_tmpA0.f1 != 0){if(*((int*)((struct Cyc_List_List*)_tmpA0.f1)->hd)== 0)switch(*((int*)_tmpA0.f2)){case 6: _tmpA3=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmpA0.f1)->hd)->f1;_tmpA1=(_tmpA0.f1)->tl;_tmpA4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA0.f2)->f1).is_union;_tmpA5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA0.f2)->f2;{int i=_tmpA3;struct Cyc_List_List*tl=_tmpA1;int is_union=_tmpA4;struct _fat_ptr d=_tmpA5;
# 466
void*_tmpA6=Cyc_CfFlowInfo_insert_place_outer(tl,*((void**)_check_fat_subscript(d,sizeof(void*),i)),new_val);void*new_child=_tmpA6;
struct _fat_ptr _tmpA7=Cyc_CfFlowInfo_aggr_dict_insert(d,i,new_child);struct _fat_ptr new_d=_tmpA7;
if((void**)new_d.curr == (void**)d.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=6,(_tmpA8->f1).is_union=is_union,(_tmpA8->f1).fieldnum=- 1,_tmpA8->f2=new_d;_tmpA8;});}}case 8: goto _LL5;default: goto _LL7;}else{switch(*((int*)_tmpA0.f2)){case 5: _tmpA1=(_tmpA0.f1)->tl;_tmpA2=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpA0.f2)->f1;{struct Cyc_List_List*tl=_tmpA1;void*rval=_tmpA2;
# 471
void*_tmpA9=Cyc_CfFlowInfo_insert_place_outer(tl,rval,new_val);void*new_rval=_tmpA9;
if(new_rval == rval)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=5,_tmpAA->f1=new_rval;_tmpAA;});}}case 8: goto _LL5;default: goto _LL7;}}}else{if(*((int*)_tmpA0.f2)== 8){_LL5: _tmpA1=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA0.f2)->f1;_tmpA2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA0.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_tmpA1;void*rval=_tmpA2;
# 475
void*_tmpAB=Cyc_CfFlowInfo_insert_place_outer(path,rval,new_val);void*new_rval=_tmpAB;
if(new_rval == rval)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=8,_tmpAC->f1=n,_tmpAC->f2=new_rval;_tmpAC;});}}}else{_LL7:
({void*_tmpAD=0U;({struct _fat_ptr _tmp224=({const char*_tmpAE="bad insert place";_tag_fat(_tmpAE,sizeof(char),17U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp224,_tag_fat(_tmpAD,sizeof(void*),0));});});}};}}
# 485
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmpAF=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;struct Cyc_CfFlowInfo_Place*place=_tmpAF;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;{
void*oldval;void*newval;
{struct _handler_cons _tmpB0;_push_handler(& _tmpB0);{int _tmpB2=0;if(setjmp(_tmpB0.handler))_tmpB2=1;if(!_tmpB2){oldval=Cyc_CfFlowInfo_lookup_place(d,place);;_pop_handler();}else{void*_tmpB1=(void*)Cyc_Core_get_exn_thrown();void*_tmpB3=_tmpB1;void*_tmpB4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpB3)->tag == Cyc_Dict_Absent)
continue;else{_tmpB4=_tmpB3;{void*exn=_tmpB4;(int)_rethrow(exn);}};}}}
{enum Cyc_CfFlowInfo_InitLevel _tmpB5=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _stmttmpE=_tmpB5;enum Cyc_CfFlowInfo_InitLevel _tmpB6=_stmttmpE;if(_tmpB6 == Cyc_CfFlowInfo_AllIL){
newval=fenv->esc_all;goto _LL5;}else{
newval=fenv->esc_none;goto _LL5;}_LL5:;}
# 496
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmpB7=*place;struct Cyc_CfFlowInfo_Place _stmttmpF=_tmpB7;struct Cyc_CfFlowInfo_Place _tmpB8=_stmttmpF;void*_tmpBA;void*_tmpB9;_tmpB9=_tmpB8.root;_tmpBA=_tmpB8.path;{void*root=_tmpB9;struct Cyc_List_List*path=_tmpBA;
d=({struct Cyc_Dict_Dict _tmp228=d;void*_tmp227=root;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(_tmp228,_tmp227,({
struct Cyc_List_List*_tmp226=path;void*_tmp225=
((void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(d,root);
# 499
Cyc_CfFlowInfo_insert_place_outer(_tmp226,_tmp225,newval);}));});}}}}
# 503
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 513
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmpBB=r;void*_tmpBC;enum Cyc_CfFlowInfo_InitLevel _tmpBD;switch(*((int*)_tmpBB)){case 2: _tmpBD=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpBB)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmpBD;
return il;}case 3: _tmpBD=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmpBB)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmpBD;
return il;}case 0:
 goto _LL8;case 1: _LL8:
 return 1U;case 7: _tmpBC=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBB)->f3;{void*r=_tmpBC;
return 0U;}default:
({void*_tmpBE=0U;({struct _fat_ptr _tmp229=({const char*_tmpBF="initlevel_approx";_tag_fat(_tmpBF,sizeof(char),17U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp229,_tag_fat(_tmpBE,sizeof(void*),0));});});};}
# 523
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 525
enum Cyc_CfFlowInfo_InitLevel this_ans;
if((int)acc == 0)return 0U;
{void*_tmpC0=r;struct _fat_ptr _tmpC2;int _tmpC4;int _tmpC3;void*_tmpC1;switch(*((int*)_tmpC0)){case 8: _tmpC1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f2;{void*r=_tmpC1;
return Cyc_CfFlowInfo_initlevel_rec(env,r,acc);}case 6: _tmpC3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1).is_union;_tmpC4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1).fieldnum;_tmpC2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f2;if(_tmpC3){int iu=_tmpC3;int f=_tmpC4;struct _fat_ptr d=_tmpC2;
# 533
unsigned _tmpC5=_get_fat_size(d,sizeof(void*));unsigned sz=_tmpC5;
this_ans=0U;
if(f == -1){
int i=0;for(0;(unsigned)i < sz;++ i){
if((int)Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[i],Cyc_CfFlowInfo_AllIL)== (int)Cyc_CfFlowInfo_AllIL){
this_ans=1U;
break;}}}else{
# 543
if((int)Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_fat_subscript(d,sizeof(void*),f)),Cyc_CfFlowInfo_AllIL)== (int)Cyc_CfFlowInfo_AllIL)
this_ans=1U;}
goto _LL0;}else{_tmpC2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f2;{struct _fat_ptr d=_tmpC2;
# 547
unsigned _tmpC6=_get_fat_size(d,sizeof(void*));unsigned sz=_tmpC6;
this_ans=1U;
{int i=0;for(0;(unsigned)i < sz;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[i],this_ans);}}
goto _LL0;}}case 4: _tmpC1=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmpC1;
# 553
if(((int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,p))
this_ans=1U;else{
# 556
({struct Cyc_List_List*_tmp22A=({struct Cyc_List_List*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->hd=p,_tmpC7->tl=env->seen;_tmpC7;});env->seen=_tmp22A;});
this_ans=({struct Cyc_CfFlowInfo_InitlevelEnv*_tmp22B=env;Cyc_CfFlowInfo_initlevel_rec(_tmp22B,Cyc_CfFlowInfo_lookup_place(env->d,p),Cyc_CfFlowInfo_AllIL);});
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;}
# 560
goto _LL0;}case 5: _tmpC1=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1;{void*r=_tmpC1;
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,r,Cyc_CfFlowInfo_AllIL);goto _LL0;}default:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);goto _LL0;}_LL0:;}
# 564
return this_ans;}
# 566
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmpC8=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp1E6;_tmp1E6.d=d,_tmp1E6.seen=0;_tmp1E6;});struct Cyc_CfFlowInfo_InitlevelEnv env=_tmpC8;
return Cyc_CfFlowInfo_initlevel_rec(& env,r,Cyc_CfFlowInfo_AllIL);}
# 571
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmpC9=*place;struct Cyc_CfFlowInfo_Place _stmttmp10=_tmpC9;struct Cyc_CfFlowInfo_Place _tmpCA=_stmttmp10;void*_tmpCC;void*_tmpCB;_tmpCB=_tmpCA.root;_tmpCC=_tmpCA.path;{void*root=_tmpCB;struct Cyc_List_List*path=_tmpCC;
void*_tmpCD=((void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(d,root);void*ans=_tmpCD;
for(1;path != 0;path=path->tl){
retry: {struct _tuple12 _tmpCE=({struct _tuple12 _tmp1E7;_tmp1E7.f1=ans,_tmp1E7.f2=(void*)path->hd;_tmp1E7;});struct _tuple12 _stmttmp11=_tmpCE;struct _tuple12 _tmpCF=_stmttmp11;int _tmpD3;struct _fat_ptr _tmpD2;struct Cyc_CfFlowInfo_UnionRInfo _tmpD1;void*_tmpD0;switch(*((int*)_tmpCF.f1)){case 8: _tmpD0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF.f1)->f2;{void*rval=_tmpD0;
# 577
ans=rval;goto retry;}case 7: _tmpD0=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCF.f1)->f3;{void*rval=_tmpD0;
# 580
ans=rval;goto retry;}case 6: if(*((int*)_tmpCF.f2)== 0){_tmpD1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f1)->f1;_tmpD2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f1)->f2;_tmpD3=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmpCF.f2)->f1;{struct Cyc_CfFlowInfo_UnionRInfo is_union=_tmpD1;struct _fat_ptr d2=_tmpD2;int fname=_tmpD3;
# 582
ans=*((void**)_check_fat_subscript(d2,sizeof(void*),fname));
goto _LL3;}}else{goto _LLC;}case 5: if(*((int*)_tmpCF.f2)== 1){_tmpD0=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpCF.f1)->f1;{void*rval=_tmpD0;
# 585
ans=rval;
goto _LL3;}}else{goto _LLC;}default: _LLC:
# 593
({void*_tmpD4=0U;({struct _fat_ptr _tmp22C=({const char*_tmpD5="bad lookup_place";_tag_fat(_tmpD5,sizeof(char),17U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp22C,_tag_fat(_tmpD4,sizeof(void*),0));});});}_LL3:;}}
# 595
return ans;}}
# 598
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmpD6=rval;struct _fat_ptr _tmpD9;int _tmpD8;int _tmpD7;void*_tmpDA;switch(*((int*)_tmpD6)){case 3:
 return 0;case 7: _tmpDA=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD6)->f3;{void*r=_tmpDA;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 8: _tmpDA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD6)->f2;{void*r=_tmpDA;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 6: _tmpD7=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD6)->f1).is_union;_tmpD8=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD6)->f1).fieldnum;_tmpD9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD6)->f2;{int is_union=_tmpD7;int field_no=_tmpD8;struct _fat_ptr d=_tmpD9;
# 604
if(is_union && field_no != -1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_fat_subscript(d,sizeof(void*),field_no)));else{
# 607
unsigned sz=_get_fat_size(d,sizeof(void*));
{int i=0;for(0;(unsigned)i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)d.curr)[i]))return 0;}}
return 1;}}default:
# 612
 return 1;};}
# 615
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 618
int Cyc_CfFlowInfo_is_init_pointer(void*rval){
void*_tmpDB=rval;void*_tmpDC;switch(*((int*)_tmpDB)){case 8: _tmpDC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f2;{void*r=_tmpDC;
return Cyc_CfFlowInfo_is_init_pointer(r);}case 4:
 goto _LL6;case 5: _LL6:
 return 1;default:
 return 0;};}
# 628
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle _tmpDD=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDD;_push_region(rgn);
{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->places=0;_tmpDF;});
Cyc_CfFlowInfo_add_places(pile,r);{
struct Cyc_Dict_Dict _tmpDE=Cyc_CfFlowInfo_escape_these(fenv,pile,d);_npop_handler(0);return _tmpDE;}}
# 630
;_pop_region();}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 642
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 647
struct _tuple12 _tmpE0=({struct _tuple12 _tmp1E9;_tmp1E9.f1=oldval,_tmp1E9.f2=newval;_tmp1E9;});struct _tuple12 _stmttmp12=_tmpE0;struct _tuple12 _tmpE1=_stmttmp12;enum Cyc_CfFlowInfo_InitLevel _tmpE2;struct _fat_ptr _tmpE6;struct Cyc_CfFlowInfo_UnionRInfo _tmpE5;struct _fat_ptr _tmpE4;struct Cyc_CfFlowInfo_UnionRInfo _tmpE3;void*_tmpE8;void*_tmpE7;if(*((int*)_tmpE1.f1)== 8){_tmpE7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE1.f1)->f2;{void*r1=_tmpE7;
# 649
return Cyc_CfFlowInfo_assign_place_inner(env,r1,newval);}}else{if(*((int*)_tmpE1.f2)== 8){_tmpE7=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE1.f2)->f1;_tmpE8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE1.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_tmpE7;void*r=_tmpE8;
# 651
void*_tmpE9=Cyc_CfFlowInfo_assign_place_inner(env,oldval,r);void*new_rval=_tmpE9;
if(new_rval == r)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->tag=8,_tmpEA->f1=n,_tmpEA->f2=new_rval;_tmpEA;});}}}else{switch(*((int*)_tmpE1.f1)){case 3: if(*((int*)_tmpE1.f2)== 4){_tmpE7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpE1.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmpE7;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL8;}}else{_LL8:
# 656
 if((int)Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmpEB=0U;({unsigned _tmp22E=env->loc;struct _fat_ptr _tmp22D=({const char*_tmpEC="assignment puts possibly-uninitialized data in an escaped location";_tag_fat(_tmpEC,sizeof(char),67U);});Cyc_CfFlowInfo_aerr(_tmp22E,_tmp22D,_tag_fat(_tmpEB,sizeof(void*),0));});});
# 659
return(env->fenv)->esc_all;}case 6: switch(*((int*)_tmpE1.f2)){case 6: _tmpE3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE1.f1)->f1;_tmpE4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE1.f1)->f2;_tmpE5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE1.f2)->f1;_tmpE6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE1.f2)->f2;{struct Cyc_CfFlowInfo_UnionRInfo is_union1=_tmpE3;struct _fat_ptr d1=_tmpE4;struct Cyc_CfFlowInfo_UnionRInfo is_union2=_tmpE5;struct _fat_ptr d2=_tmpE6;
# 661
struct _fat_ptr new_d=({unsigned _tmpEF=
_get_fat_size(d1,sizeof(void*));void**_tmpEE=_cycalloc(_check_times(_tmpEF,sizeof(void*)));({{unsigned _tmp1E8=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _tmp1E8;++ i){({void*_tmp22F=Cyc_CfFlowInfo_assign_place_inner(env,((void**)d1.curr)[(int)i],*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));_tmpEE[i]=_tmp22F;});}}0;});_tag_fat(_tmpEE,sizeof(void*),_tmpEF);});
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
if(({void*_tmp230=*((void**)_check_fat_subscript(new_d,sizeof(void*),i));_tmp230 != *((void**)_check_fat_subscript(d2,sizeof(void*),i));})){
change=1;break;}}}
# 680
if(!change){
if(!is_union1.is_union)return newval;
new_d=d2;}}
# 685
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->tag=6,_tmpED->f1=is_union2,_tmpED->f2=new_d;_tmpED;});}case 3: goto _LLB;default: goto _LLD;}default: if(*((int*)_tmpE1.f2)== 3){_LLB: _tmpE2=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmpE1.f2)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmpE2;
# 687
enum Cyc_CfFlowInfo_InitLevel _tmpF0=il;if(_tmpF0 == Cyc_CfFlowInfo_NoneIL)
return(env->fenv)->unknown_none;else{
return(env->fenv)->unknown_all;};}}else{_LLD:
# 691
 return newval;}}}};}
# 697
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 701
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*path,int path_prefix,void*oldval,void*newval){
# 712
if(path == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple17 _tmpF1=({struct _tuple17 _tmp1EA;_tmp1EA.f1=path,_tmp1EA.f2=oldval;_tmp1EA;});struct _tuple17 _stmttmp13=_tmpF1;struct _tuple17 _tmpF2=_stmttmp13;struct _fat_ptr _tmpF7;int _tmpF6;int _tmpF5;int _tmpF3;void*_tmpF8;void*_tmpF4;switch(*((int*)_tmpF2.f2)){case 8: _tmpF4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f1;_tmpF8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_tmpF4;void*r=_tmpF8;
# 715
void*_tmpF9=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);void*new_r=_tmpF9;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=8,_tmpFA->f1=n,_tmpFA->f2=new_r;_tmpFA;});}}case 7: _tmpF4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f1;_tmpF3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f2;_tmpF8=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f3;{struct Cyc_Absyn_Exp*x=_tmpF4;int y=_tmpF3;void*r=_tmpF8;
# 719
void*_tmpFB=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);void*new_r=_tmpFB;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=7,_tmpFC->f1=x,_tmpFC->f2=y,_tmpFC->f3=new_r;_tmpFC;});}}default: if(_tmpF2.f1 != 0){if(*((int*)((struct Cyc_List_List*)_tmpF2.f1)->hd)== 1){if(*((int*)_tmpF2.f2)== 5){_tmpF4=(_tmpF2.f1)->tl;_tmpF8=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f1;{struct Cyc_List_List*tl=_tmpF4;void*r=_tmpF8;
# 723
void*_tmpFD=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,r,newval);void*new_r=_tmpFD;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->tag=5,_tmpFE->f1=new_r;_tmpFE;});}}}else{goto _LL9;}}else{if(*((int*)_tmpF2.f2)== 6){_tmpF3=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmpF2.f1)->hd)->f1;_tmpF4=(_tmpF2.f1)->tl;_tmpF5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f1).is_union;_tmpF6=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f1).fieldnum;_tmpF7=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF2.f2)->f2;{int fnum=_tmpF3;struct Cyc_List_List*tl=_tmpF4;int is_union=_tmpF5;int fldnum=_tmpF6;struct _fat_ptr d=_tmpF7;
# 727
void*_tmpFF=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,*((void**)_check_fat_subscript(d,sizeof(void*),fnum)),newval);void*new_child=_tmpFF;
# 729
struct _fat_ptr _tmp100=Cyc_CfFlowInfo_aggr_dict_insert(d,fnum,new_child);struct _fat_ptr new_child_agg=_tmp100;
if((void**)new_child_agg.curr == (void**)d.curr &&(!is_union || fldnum == fnum))return oldval;
d=new_child_agg;
# 734
if(is_union){
int changed=0;
int sz=(int)_get_fat_size(d,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != fnum){
struct _fat_ptr _tmp101=({
struct _fat_ptr _tmp232=d;int _tmp231=i;Cyc_CfFlowInfo_aggr_dict_insert(_tmp232,_tmp231,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->unknown_all,*((void**)_check_fat_subscript(d,sizeof(void*),i))));});
# 739
struct _fat_ptr new_d=_tmp101;
# 742
if((void**)new_d.curr != (void**)d.curr){
d=new_d;
changed=1;}}}}
# 750
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp102=env->root_place;struct Cyc_CfFlowInfo_Place*_stmttmp14=_tmp102;struct Cyc_CfFlowInfo_Place*_tmp103=_stmttmp14;void*_tmp105;void*_tmp104;_tmp104=_tmp103->root;_tmp105=_tmp103->path;{void*root=_tmp104;struct Cyc_List_List*path=_tmp105;
struct Cyc_List_List*_tmp106=({int*_tmp233=({int*_tmp109=_cycalloc_atomic(sizeof(*_tmp109));*_tmp109=path_prefix;_tmp109;});((struct Cyc_List_List*(*)(int(*)(int*,void*),int*,struct Cyc_List_List*))Cyc_List_filter_c)((int(*)(int*,void*))Cyc_CfFlowInfo_nprefix,_tmp233,path);});struct Cyc_List_List*new_path=_tmp106;
struct Cyc_CfFlowInfo_Place*_tmp107=({struct Cyc_CfFlowInfo_Place*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->root=root,_tmp108->path=new_path;_tmp108;});struct Cyc_CfFlowInfo_Place*curr_place=_tmp107;;}}}
# 756
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->tag=6,(_tmp10A->f1).is_union=is_union,(_tmp10A->f1).fieldnum=fnum,_tmp10A->f2=d;_tmp10A;});}}else{goto _LL9;}}}else{_LL9:
({void*_tmp10B=0U;({struct _fat_ptr _tmp234=({const char*_tmp10C="bad assign place";_tag_fat(_tmp10C,sizeof(char),17U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp234,_tag_fat(_tmp10B,sizeof(void*),0));});});}};}}
# 760
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 768
struct Cyc_CfFlowInfo_Place*_tmp10D=place;void*_tmp10F;void*_tmp10E;_tmp10E=_tmp10D->root;_tmp10F=_tmp10D->path;{void*root=_tmp10E;struct Cyc_List_List*path=_tmp10F;
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp1EB;_tmp1EB.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp235=({struct Cyc_CfFlowInfo_EscPile*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->places=0;_tmp110;});_tmp1EB.pile=_tmp235;}),_tmp1EB.d=d,_tmp1EB.loc=loc,_tmp1EB.root_place=place;_tmp1EB;});
void*newval=({struct Cyc_List_List*_tmp237=path;void*_tmp236=((void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(d,root);Cyc_CfFlowInfo_assign_place_outer(& env,_tmp237,0,_tmp236,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp239=fenv;struct Cyc_CfFlowInfo_EscPile*_tmp238=env.pile;Cyc_CfFlowInfo_escape_these(_tmp239,_tmp238,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d,root,newval));});}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 783
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*,void*,void*);struct _tuple18{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 791
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;
# 794
{struct _tuple12 _tmp111=({struct _tuple12 _tmp1ED;_tmp1ED.f1=r1,_tmp1ED.f2=r2;_tmp1ED;});struct _tuple12 _stmttmp15=_tmp111;struct _tuple12 _tmp112=_stmttmp15;struct _fat_ptr _tmp118;int _tmp117;int _tmp116;struct _fat_ptr _tmp115;int _tmp114;int _tmp113;void*_tmp11C;void*_tmp11B;void*_tmp11A;void*_tmp119;if(*((int*)_tmp112.f1)== 8){if(*((int*)_tmp112.f2)== 8){_tmp119=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;_tmp11A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f2;_tmp11B=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;_tmp11C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f2;{struct Cyc_Absyn_Vardecl*n1=_tmp119;void*r1=_tmp11A;struct Cyc_Absyn_Vardecl*n2=_tmp11B;void*r2=_tmp11C;
# 797
if(n1 == n2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=8,_tmp11D->f1=n1,({void*_tmp23A=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp11D->f2=_tmp23A;});_tmp11D;});else{
# 800
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{_tmp119=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;_tmp11A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f2;{struct Cyc_Absyn_Vardecl*n1=_tmp119;void*r1=_tmp11A;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{if(*((int*)_tmp112.f2)== 8){_tmp119=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;_tmp11A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f2;{struct Cyc_Absyn_Vardecl*n2=_tmp119;void*r2=_tmp11A;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}else{if(*((int*)_tmp112.f1)== 7){if(*((int*)_tmp112.f2)== 7){_tmp119=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;_tmp113=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f2;_tmp11A=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f3;_tmp11B=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;_tmp114=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f2;_tmp11C=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f3;{struct Cyc_Absyn_Exp*e1=_tmp119;int i1=_tmp113;void*r1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;int i2=_tmp114;void*r2=_tmp11C;
# 805
if(e1 == e2)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->tag=7,_tmp11E->f1=e1,i1 > i2?_tmp11E->f2=i1:(_tmp11E->f2=i2),({void*_tmp23B=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp11E->f3=_tmp23B;});_tmp11E;});
{void*_tmp11F=r1;void*_tmp120;if(*((int*)_tmp11F)== 4){_tmp120=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp11F)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp120;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL21;}}else{
goto _LL21;}_LL21:;}
# 811
{void*_tmp121=r2;void*_tmp122;if(*((int*)_tmp121)== 4){_tmp122=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp121)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp122;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL26;}}else{
goto _LL26;}_LL26:;}
# 815
goto _LL0;}}else{_tmp119=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;_tmp113=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f2;_tmp11A=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f3;{struct Cyc_Absyn_Exp*e1=_tmp119;int i1=_tmp113;void*r1=_tmp11A;
# 817
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=7,_tmp123->f1=e1,_tmp123->f2=i1,({void*_tmp23C=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp123->f3=_tmp23C;});_tmp123;});}}}else{if(*((int*)_tmp112.f2)== 7){_tmp119=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;_tmp113=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f2;_tmp11A=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f3;{struct Cyc_Absyn_Exp*e2=_tmp119;int i2=_tmp113;void*r2=_tmp11A;
# 819
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=7,_tmp124->f1=e2,_tmp124->f2=i2,({void*_tmp23D=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp124->f3=_tmp23D;});_tmp124;});}}else{switch(*((int*)_tmp112.f1)){case 4: switch(*((int*)_tmp112.f2)){case 4: _tmp119=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;_tmp11A=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;{struct Cyc_CfFlowInfo_Place*p1=_tmp119;struct Cyc_CfFlowInfo_Place*p2=_tmp11A;
# 822
if(Cyc_CfFlowInfo_place_cmp(p1,p2)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,p1);
Cyc_CfFlowInfo_add_place(env->pile,p2);
goto _LL0;}case 1: _tmp119=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp119;
# 828
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _tmp125=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _stmttmp16=_tmp125;enum Cyc_CfFlowInfo_InitLevel _tmp126=_stmttmp16;if(_tmp126 == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}}default: _tmp119=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp119;
# 854
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}case 1: switch(*((int*)_tmp112.f2)){case 4: _tmp119=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp119;
# 834
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _tmp127=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _stmttmp17=_tmp127;enum Cyc_CfFlowInfo_InitLevel _tmp128=_stmttmp17;if(_tmp128 == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}}case 5: _tmp119=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;{void*r2=_tmp119;
# 849
enum Cyc_CfFlowInfo_InitLevel _tmp12C=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _stmttmp19=_tmp12C;enum Cyc_CfFlowInfo_InitLevel _tmp12D=_stmttmp19;if(_tmp12D == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}default: goto _LL1F;}case 5: switch(*((int*)_tmp112.f2)){case 5: _tmp119=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;_tmp11A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;{void*r1=_tmp119;void*r2=_tmp11A;
# 841
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=5,({void*_tmp23E=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp129->f1=_tmp23E;});_tmp129;});}case 1: _tmp119=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1;{void*r1=_tmp119;
# 844
enum Cyc_CfFlowInfo_InitLevel _tmp12A=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _stmttmp18=_tmp12A;enum Cyc_CfFlowInfo_InitLevel _tmp12B=_stmttmp18;if(_tmp12B == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}case 4: goto _LL1B;default: goto _LL1F;}default: if(*((int*)_tmp112.f2)== 4){_LL1B: _tmp119=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp119;
# 855
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}else{if(*((int*)_tmp112.f1)== 6){if(*((int*)_tmp112.f2)== 6){_tmp113=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1).is_union;_tmp114=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f1).fieldnum;_tmp115=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp112.f1)->f2;_tmp116=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1).is_union;_tmp117=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f1).fieldnum;_tmp118=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp112.f2)->f2;{int is_union1=_tmp113;int field_no1=_tmp114;struct _fat_ptr d1=_tmp115;int is_union2=_tmp116;int field_no2=_tmp117;struct _fat_ptr d2=_tmp118;
# 858
struct _fat_ptr new_d=({unsigned _tmp130=
_get_fat_size(d1,sizeof(void*));void**_tmp12F=_cycalloc(_check_times(_tmp130,sizeof(void*)));({{unsigned _tmp1EC=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _tmp1EC;++ i){({void*_tmp23F=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)d1.curr)[(int)i],*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));_tmp12F[i]=_tmp23F;});}}0;});_tag_fat(_tmp12F,sizeof(void*),_tmp130);});
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
if(({void*_tmp240=*((void**)_check_fat_subscript(new_d,sizeof(void*),i));_tmp240 != *((void**)_check_fat_subscript(d2,sizeof(void*),i));})){
change=1;break;}}}
# 878
if(!change){
if(!is_union1)return r2;
new_d=d2;}}
# 883
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=6,(_tmp12E->f1).is_union=is_union1,field_no1 == field_no2?(_tmp12E->f1).fieldnum=field_no1:((_tmp12E->f1).fieldnum=-1),_tmp12E->f2=new_d;_tmp12E;});}}else{goto _LL1F;}}else{_LL1F:
# 885
 goto _LL0;}}}}}}}_LL0:;}{
# 887
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple12 _tmp131=({struct _tuple12 _tmp1F0;_tmp1F0.f1=r1,_tmp1F0.f2=r2;_tmp1F0;});struct _tuple12 _stmttmp1A=_tmp131;struct _tuple12 _tmp132=_stmttmp1A;if(*((int*)_tmp132.f1)== 3)
goto _LL43;else{if(*((int*)_tmp132.f2)== 3){_LL43: {
# 892
struct _tuple18 _tmp133=({struct _tuple18 _tmp1EE;_tmp1EE.f1=il1,_tmp1EE.f2=il2;_tmp1EE;});struct _tuple18 _stmttmp1B=_tmp133;struct _tuple18 _tmp134=_stmttmp1B;if(_tmp134.f2 == Cyc_CfFlowInfo_NoneIL)
goto _LL4A;else{if(_tmp134.f1 == Cyc_CfFlowInfo_NoneIL){_LL4A: return(env->fenv)->esc_none;}else{
return(env->fenv)->esc_all;}};}}else{
# 897
struct _tuple18 _tmp135=({struct _tuple18 _tmp1EF;_tmp1EF.f1=il1,_tmp1EF.f2=il2;_tmp1EF;});struct _tuple18 _stmttmp1C=_tmp135;struct _tuple18 _tmp136=_stmttmp1C;if(_tmp136.f2 == Cyc_CfFlowInfo_NoneIL)
goto _LL51;else{if(_tmp136.f1 == Cyc_CfFlowInfo_NoneIL){_LL51: return(env->fenv)->unknown_none;}else{
return(env->fenv)->unknown_all;}};}};}}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 904
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 906
struct _tuple19 _tmp137=({struct _tuple19 _tmp1F2;_tmp1F2.f1=f1,_tmp1F2.f2=f2;_tmp1F2;});struct _tuple19 _stmttmp1D=_tmp137;struct _tuple19 _tmp138=_stmttmp1D;void*_tmp13C;struct Cyc_Dict_Dict _tmp13B;void*_tmp13A;struct Cyc_Dict_Dict _tmp139;if(((_tmp138.f1).BottomFL).tag == 1)
return f2;else{if(((_tmp138.f2).BottomFL).tag == 1)
return f1;else{_tmp139=(((_tmp138.f1).ReachableFL).val).f1;_tmp13A=(((_tmp138.f1).ReachableFL).val).f2;_tmp13B=(((_tmp138.f2).ReachableFL).val).f1;_tmp13C=(((_tmp138.f2).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp139;struct Cyc_List_List*r1=_tmp13A;struct Cyc_Dict_Dict d2=_tmp13B;struct Cyc_List_List*r2=_tmp13C;
# 912
if(d1.t == d2.t && r1 == r2)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct Cyc_CfFlowInfo_JoinEnv _tmp13D=({struct Cyc_CfFlowInfo_JoinEnv _tmp1F1;_tmp1F1.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp241=({struct Cyc_CfFlowInfo_EscPile*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->places=0;_tmp140;});_tmp1F1.pile=_tmp241;}),_tmp1F1.d1=d1,_tmp1F1.d2=d2;_tmp1F1;});struct Cyc_CfFlowInfo_JoinEnv env=_tmp13D;
struct Cyc_Dict_Dict _tmp13E=((struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*))Cyc_CfFlowInfo_join_absRval,& env,d1,d2);struct Cyc_Dict_Dict outdict=_tmp13E;
struct Cyc_List_List*_tmp13F=Cyc_Relations_join_relns(Cyc_Core_heap_region,r1,r2);struct Cyc_List_List*r=_tmp13F;
return({struct Cyc_Dict_Dict _tmp242=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outdict);Cyc_CfFlowInfo_ReachableFL(_tmp242,r);});}}}};}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 922
struct _tuple20 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple20 pr1,struct _tuple20 pr2){
# 927
struct _tuple20 _tmp141=pr1;void*_tmp143;union Cyc_CfFlowInfo_FlowInfo _tmp142;_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp142;void*r1=_tmp143;
struct _tuple20 _tmp144=pr2;void*_tmp146;union Cyc_CfFlowInfo_FlowInfo _tmp145;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp145;void*r2=_tmp146;
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);
struct _tuple21 _tmp147=({struct _tuple21 _tmp1F9;_tmp1F9.f1=f1,_tmp1F9.f2=f2,_tmp1F9.f3=outflow;_tmp1F9;});struct _tuple21 _stmttmp1E=_tmp147;struct _tuple21 _tmp148=_stmttmp1E;void*_tmp14C;struct Cyc_Dict_Dict _tmp14B;struct Cyc_Dict_Dict _tmp14A;struct Cyc_Dict_Dict _tmp149;if(((_tmp148.f1).BottomFL).tag == 1)
return({struct _tuple20 _tmp1F3;_tmp1F3.f1=outflow,_tmp1F3.f2=r2;_tmp1F3;});else{if(((_tmp148.f2).BottomFL).tag == 1)
return({struct _tuple20 _tmp1F4;_tmp1F4.f1=outflow,_tmp1F4.f2=r1;_tmp1F4;});else{if(((_tmp148.f3).ReachableFL).tag == 2){_tmp149=(((_tmp148.f1).ReachableFL).val).f1;_tmp14A=(((_tmp148.f2).ReachableFL).val).f1;_tmp14B=(((_tmp148.f3).ReachableFL).val).f1;_tmp14C=(((_tmp148.f3).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp149;struct Cyc_Dict_Dict d2=_tmp14A;struct Cyc_Dict_Dict outd=_tmp14B;struct Cyc_List_List*relns=_tmp14C;
# 935
if(((int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,r1,r2))return({struct _tuple20 _tmp1F5;_tmp1F5.f1=outflow,_tmp1F5.f2=r2;_tmp1F5;});
if(((int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,r2,r1))return({struct _tuple20 _tmp1F6;_tmp1F6.f1=outflow,_tmp1F6.f2=r1;_tmp1F6;});{
struct Cyc_CfFlowInfo_JoinEnv _tmp14D=({struct Cyc_CfFlowInfo_JoinEnv _tmp1F8;_tmp1F8.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp243=({struct Cyc_CfFlowInfo_EscPile*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->places=0;_tmp14F;});_tmp1F8.pile=_tmp243;}),_tmp1F8.d1=d1,_tmp1F8.d2=d2;_tmp1F8;});struct Cyc_CfFlowInfo_JoinEnv env=_tmp14D;
void*_tmp14E=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval)(& env,0,r1,r2);void*outr=_tmp14E;
return({struct _tuple20 _tmp1F7;({union Cyc_CfFlowInfo_FlowInfo _tmp245=({struct Cyc_Dict_Dict _tmp244=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outd);Cyc_CfFlowInfo_ReachableFL(_tmp244,relns);});_tmp1F7.f1=_tmp245;}),_tmp1F7.f2=outr;_tmp1F7;});}}}else{
# 941
({void*_tmp150=0U;({struct _fat_ptr _tmp246=({const char*_tmp151="join_flow_and_rval: BottomFL outflow";_tag_fat(_tmp151,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp246,_tag_fat(_tmp150,sizeof(void*),0));});});}}};}}}
# 946
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 949
{struct _tuple12 _tmp152=({struct _tuple12 _tmp1FA;_tmp1FA.f1=r1,_tmp1FA.f2=r2;_tmp1FA;});struct _tuple12 _stmttmp1F=_tmp152;struct _tuple12 _tmp153=_stmttmp1F;struct _fat_ptr _tmp159;int _tmp158;int _tmp157;struct _fat_ptr _tmp156;int _tmp155;int _tmp154;void*_tmp15D;void*_tmp15C;void*_tmp15B;void*_tmp15A;if(*((int*)_tmp153.f1)== 8){if(*((int*)_tmp153.f2)== 8){_tmp15A=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp153.f1)->f1;_tmp15B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp153.f1)->f2;_tmp15C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp153.f2)->f1;_tmp15D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp153.f2)->f2;{struct Cyc_Absyn_Vardecl*n1=_tmp15A;void*r1=_tmp15B;struct Cyc_Absyn_Vardecl*n2=_tmp15C;void*r2=_tmp15D;
# 951
return n1 == n2 && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{
goto _LL6;}}else{if(*((int*)_tmp153.f2)== 8){_LL6:
 return 0;}else{if(*((int*)_tmp153.f1)== 4){if(*((int*)_tmp153.f2)== 4){_tmp15A=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp153.f1)->f1;_tmp15B=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp153.f2)->f1;{struct Cyc_CfFlowInfo_Place*p1=_tmp15A;struct Cyc_CfFlowInfo_Place*p2=_tmp15B;
return Cyc_CfFlowInfo_place_cmp(p1,p2)== 0;}}else{
goto _LLC;}}else{if(*((int*)_tmp153.f2)== 4){_LLC:
 return 0;}else{if(*((int*)_tmp153.f1)== 5){if(*((int*)_tmp153.f2)== 5){_tmp15A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp153.f1)->f1;_tmp15B=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp153.f2)->f1;{void*r1=_tmp15A;void*r2=_tmp15B;
return Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{
goto _LL12;}}else{if(*((int*)_tmp153.f2)== 5){_LL12:
 return 0;}else{if(*((int*)_tmp153.f1)== 6)switch(*((int*)_tmp153.f2)){case 6: _tmp154=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp153.f1)->f1).is_union;_tmp155=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp153.f1)->f1).fieldnum;_tmp156=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp153.f1)->f2;_tmp157=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp153.f2)->f1).is_union;_tmp158=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp153.f2)->f1).fieldnum;_tmp159=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp153.f2)->f2;{int is_union1=_tmp154;int fld1=_tmp155;struct _fat_ptr d1=_tmp156;int is_union2=_tmp157;int fld2=_tmp158;struct _fat_ptr d2=_tmp159;
# 962
if(is_union1 && fld1 != fld2)return 0;
if((void**)d1.curr == (void**)d2.curr)return 1;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(!((int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)d1.curr)[i],*((void**)_check_fat_subscript(d2,sizeof(void*),i))))return 0;}}
return 1;}case 0: goto _LL15;case 1: goto _LL17;default: goto _LL1D;}else{switch(*((int*)_tmp153.f2)){case 0: _LL15:
 goto _LL18;case 1: _LL17: _LL18:
 return 0;default: if(*((int*)_tmp153.f1)== 3){if(*((int*)_tmp153.f2)== 3)
goto _LL0;else{
return 0;}}else{_LL1D:
# 977
 goto _LL0;}}}}}}}}}_LL0:;}{
# 979
struct _tuple18 _tmp15E=({struct _tuple18 _tmp1FB;({enum Cyc_CfFlowInfo_InitLevel _tmp248=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp1FB.f1=_tmp248;}),({enum Cyc_CfFlowInfo_InitLevel _tmp247=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp1FB.f2=_tmp247;});_tmp1FB;});struct _tuple18 _stmttmp20=_tmp15E;struct _tuple18 _tmp15F=_stmttmp20;if(_tmp15F.f1 == Cyc_CfFlowInfo_AllIL)
goto _LL23;else{if(_tmp15F.f2 == Cyc_CfFlowInfo_NoneIL){_LL23:
 return 1;}else{
return 0;}};}}
# 992
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 994
struct _tuple19 _tmp160=({struct _tuple19 _tmp1FC;_tmp1FC.f1=f1,_tmp1FC.f2=f2;_tmp1FC;});struct _tuple19 _stmttmp21=_tmp160;struct _tuple19 _tmp161=_stmttmp21;void*_tmp165;struct Cyc_Dict_Dict _tmp164;void*_tmp163;struct Cyc_Dict_Dict _tmp162;if(((_tmp161.f1).BottomFL).tag == 1)
return 1;else{if(((_tmp161.f2).BottomFL).tag == 1)
return 0;else{_tmp162=(((_tmp161.f1).ReachableFL).val).f1;_tmp163=(((_tmp161.f1).ReachableFL).val).f2;_tmp164=(((_tmp161.f2).ReachableFL).val).f1;_tmp165=(((_tmp161.f2).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp162;struct Cyc_List_List*r1=_tmp163;struct Cyc_Dict_Dict d2=_tmp164;struct Cyc_List_List*r2=_tmp165;
# 998
if(d1.t == d2.t && r1 == r2)return 1;
return((int(*)(int(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_forall_intersect)((int(*)(void*,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx,d1,d2)&&
 Cyc_Relations_relns_approx(r1,r2);}}};}struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 1004
struct _tuple22 Cyc_CfFlowInfo_unname_rval(void*rv){
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp166=rv;void*_tmp168;void*_tmp167;if(*((int*)_tmp166)== 8){_tmp167=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp166)->f1;_tmp168=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp166)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp167;void*rv2=_tmp168;
# 1010
names=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=vd,_tmp169->tl=names;_tmp169;});rv=rv2;goto _LL0;}}else{
# 1012
done=1;goto _LL0;}_LL0:;}
# 1015
return({struct _tuple22 _tmp1FD;_tmp1FD.f1=rv,_tmp1FD.f2=names;_tmp1FD;});}
# 1018
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp16A=il;if(_tmp16A == Cyc_CfFlowInfo_NoneIL){
({void*_tmp16B=0U;({struct Cyc___cycFILE*_tmp24A=Cyc_stderr;struct _fat_ptr _tmp249=({const char*_tmp16C="uninitialized";_tag_fat(_tmp16C,sizeof(char),14U);});Cyc_fprintf(_tmp24A,_tmp249,_tag_fat(_tmp16B,sizeof(void*),0));});});goto _LL0;}else{
({void*_tmp16D=0U;({struct Cyc___cycFILE*_tmp24C=Cyc_stderr;struct _fat_ptr _tmp24B=({const char*_tmp16E="all-initialized";_tag_fat(_tmp16E,sizeof(char),16U);});Cyc_fprintf(_tmp24C,_tmp24B,_tag_fat(_tmp16D,sizeof(void*),0));});});goto _LL0;}_LL0:;}
# 1025
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp16F=root;void*_tmp171;void*_tmp170;switch(*((int*)_tmp16F)){case 0: _tmp170=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp16F)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp170;
# 1028
({struct Cyc_String_pa_PrintArg_struct _tmp174=({struct Cyc_String_pa_PrintArg_struct _tmp1FE;_tmp1FE.tag=0,({struct _fat_ptr _tmp24D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1FE.f1=_tmp24D;});_tmp1FE;});void*_tmp172[1];_tmp172[0]=& _tmp174;({struct Cyc___cycFILE*_tmp24F=Cyc_stderr;struct _fat_ptr _tmp24E=({const char*_tmp173="Root(%s)";_tag_fat(_tmp173,sizeof(char),9U);});Cyc_fprintf(_tmp24F,_tmp24E,_tag_fat(_tmp172,sizeof(void*),1));});});goto _LL0;}case 1: _tmp170=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp16F)->f1;_tmp171=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp16F)->f2;{struct Cyc_Absyn_Exp*e=_tmp170;void*t=_tmp171;
# 1030
({struct Cyc_String_pa_PrintArg_struct _tmp177=({struct Cyc_String_pa_PrintArg_struct _tmp200;_tmp200.tag=0,({
struct _fat_ptr _tmp250=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp200.f1=_tmp250;});_tmp200;});struct Cyc_String_pa_PrintArg_struct _tmp178=({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0,({struct _fat_ptr _tmp251=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp1FF.f1=_tmp251;});_tmp1FF;});void*_tmp175[2];_tmp175[0]=& _tmp177,_tmp175[1]=& _tmp178;({struct Cyc___cycFILE*_tmp253=Cyc_stderr;struct _fat_ptr _tmp252=({const char*_tmp176="MallocPt(%s,%s)";_tag_fat(_tmp176,sizeof(char),16U);});Cyc_fprintf(_tmp253,_tmp252,_tag_fat(_tmp175,sizeof(void*),2));});});goto _LL0;}default:
# 1033
({void*_tmp179=0U;({struct Cyc___cycFILE*_tmp255=Cyc_stderr;struct _fat_ptr _tmp254=({const char*_tmp17A="InitParam(_,_)";_tag_fat(_tmp17A,sizeof(char),15U);});Cyc_fprintf(_tmp255,_tmp254,_tag_fat(_tmp179,sizeof(void*),0));});});goto _LL0;}_LL0:;}
# 1037
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){
{struct Cyc_List_List*x=p;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
void*_tmp17B=(void*)x->hd;void*_stmttmp22=_tmp17B;void*_tmp17C=_stmttmp22;int _tmp17D;if(*((int*)_tmp17C)== 0){_tmp17D=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp17C)->f1;{int i=_tmp17D;
# 1041
({struct Cyc_Int_pa_PrintArg_struct _tmp180=({struct Cyc_Int_pa_PrintArg_struct _tmp201;_tmp201.tag=1,_tmp201.f1=(unsigned long)i;_tmp201;});void*_tmp17E[1];_tmp17E[0]=& _tmp180;({struct Cyc___cycFILE*_tmp257=Cyc_stderr;struct _fat_ptr _tmp256=({const char*_tmp17F=".%d";_tag_fat(_tmp17F,sizeof(char),4U);});Cyc_fprintf(_tmp257,_tmp256,_tag_fat(_tmp17E,sizeof(void*),1));});});
goto _LL0;}}else{
# 1044
if(x->tl != 0){
void*_tmp181=(void*)((struct Cyc_List_List*)_check_null(x->tl))->hd;void*_stmttmp23=_tmp181;void*_tmp182=_stmttmp23;int _tmp183;if(*((int*)_tmp182)== 0){_tmp183=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp182)->f1;{int i=_tmp183;
# 1047
x=x->tl;
({struct Cyc_Int_pa_PrintArg_struct _tmp186=({struct Cyc_Int_pa_PrintArg_struct _tmp202;_tmp202.tag=1,_tmp202.f1=(unsigned long)i;_tmp202;});void*_tmp184[1];_tmp184[0]=& _tmp186;({struct Cyc___cycFILE*_tmp259=Cyc_stderr;struct _fat_ptr _tmp258=({const char*_tmp185="->%d";_tag_fat(_tmp185,sizeof(char),5U);});Cyc_fprintf(_tmp259,_tmp258,_tag_fat(_tmp184,sizeof(void*),1));});});
continue;}}else{
goto _LL5;}_LL5:;}
# 1053
({void*_tmp187=0U;({struct Cyc___cycFILE*_tmp25B=Cyc_stderr;struct _fat_ptr _tmp25A=({const char*_tmp188="*";_tag_fat(_tmp188,sizeof(char),2U);});Cyc_fprintf(_tmp25B,_tmp25A,_tag_fat(_tmp187,sizeof(void*),0));});});}_LL0:;}}
# 1056
({void*_tmp189=0U;({struct Cyc___cycFILE*_tmp25D=Cyc_stderr;struct _fat_ptr _tmp25C=({const char*_tmp18A=" ";_tag_fat(_tmp18A,sizeof(char),2U);});Cyc_fprintf(_tmp25D,_tmp25C,_tag_fat(_tmp189,sizeof(void*),0));});});}
# 1059
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);
Cyc_CfFlowInfo_print_path(p->path);}
# 1064
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp18B=1;int first=_tmp18B;
while(x != 0){
if(!first){({void*_tmp18C=0U;({struct Cyc___cycFILE*_tmp25F=Cyc_stderr;struct _fat_ptr _tmp25E=({const char*_tmp18D=", ";_tag_fat(_tmp18D,sizeof(char),3U);});Cyc_fprintf(_tmp25F,_tmp25E,_tag_fat(_tmp18C,sizeof(void*),0));});});first=0;}
pr(x->hd);
x=x->tl;}
# 1071
({void*_tmp18E=0U;({struct Cyc___cycFILE*_tmp261=Cyc_stderr;struct _fat_ptr _tmp260=({const char*_tmp18F="\n";_tag_fat(_tmp18F,sizeof(char),2U);});Cyc_fprintf(_tmp261,_tmp260,_tag_fat(_tmp18E,sizeof(void*),0));});});}
# 1074
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp190=rval;void*_tmp192;struct _fat_ptr _tmp195;int _tmp194;int _tmp193;void*_tmp191;enum Cyc_CfFlowInfo_InitLevel _tmp196;switch(*((int*)_tmp190)){case 0:
({void*_tmp197=0U;({struct Cyc___cycFILE*_tmp263=Cyc_stderr;struct _fat_ptr _tmp262=({const char*_tmp198="Zero";_tag_fat(_tmp198,sizeof(char),5U);});Cyc_fprintf(_tmp263,_tmp262,_tag_fat(_tmp197,sizeof(void*),0));});});goto _LL0;case 1:
({void*_tmp199=0U;({struct Cyc___cycFILE*_tmp265=Cyc_stderr;struct _fat_ptr _tmp264=({const char*_tmp19A="NotZeroAll";_tag_fat(_tmp19A,sizeof(char),11U);});Cyc_fprintf(_tmp265,_tmp264,_tag_fat(_tmp199,sizeof(void*),0));});});goto _LL0;case 2: _tmp196=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp190)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp196;
({void*_tmp19B=0U;({struct Cyc___cycFILE*_tmp267=Cyc_stderr;struct _fat_ptr _tmp266=({const char*_tmp19C="Unknown(";_tag_fat(_tmp19C,sizeof(char),9U);});Cyc_fprintf(_tmp267,_tmp266,_tag_fat(_tmp19B,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(il);
({void*_tmp19D=0U;({struct Cyc___cycFILE*_tmp269=Cyc_stderr;struct _fat_ptr _tmp268=({const char*_tmp19E=")";_tag_fat(_tmp19E,sizeof(char),2U);});Cyc_fprintf(_tmp269,_tmp268,_tag_fat(_tmp19D,sizeof(void*),0));});});goto _LL0;}case 3: _tmp196=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp190)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp196;
({void*_tmp19F=0U;({struct Cyc___cycFILE*_tmp26B=Cyc_stderr;struct _fat_ptr _tmp26A=({const char*_tmp1A0="Esc(";_tag_fat(_tmp1A0,sizeof(char),5U);});Cyc_fprintf(_tmp26B,_tmp26A,_tag_fat(_tmp19F,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(il);
({void*_tmp1A1=0U;({struct Cyc___cycFILE*_tmp26D=Cyc_stderr;struct _fat_ptr _tmp26C=({const char*_tmp1A2=")";_tag_fat(_tmp1A2,sizeof(char),2U);});Cyc_fprintf(_tmp26D,_tmp26C,_tag_fat(_tmp1A1,sizeof(void*),0));});});goto _LL0;}case 4: _tmp191=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp190)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp191;
({void*_tmp1A3=0U;({struct Cyc___cycFILE*_tmp26F=Cyc_stderr;struct _fat_ptr _tmp26E=({const char*_tmp1A4="AddrOf(";_tag_fat(_tmp1A4,sizeof(char),8U);});Cyc_fprintf(_tmp26F,_tmp26E,_tag_fat(_tmp1A3,sizeof(void*),0));});});Cyc_CfFlowInfo_print_place(p);
({void*_tmp1A5=0U;({struct Cyc___cycFILE*_tmp271=Cyc_stderr;struct _fat_ptr _tmp270=({const char*_tmp1A6=")";_tag_fat(_tmp1A6,sizeof(char),2U);});Cyc_fprintf(_tmp271,_tmp270,_tag_fat(_tmp1A5,sizeof(void*),0));});});goto _LL0;}case 5: _tmp191=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp190)->f1;{void*r=_tmp191;
({void*_tmp1A7=0U;({struct Cyc___cycFILE*_tmp273=Cyc_stderr;struct _fat_ptr _tmp272=({const char*_tmp1A8="UniquePtr(";_tag_fat(_tmp1A8,sizeof(char),11U);});Cyc_fprintf(_tmp273,_tmp272,_tag_fat(_tmp1A7,sizeof(void*),0));});});Cyc_CfFlowInfo_print_absrval(r);
({void*_tmp1A9=0U;({struct Cyc___cycFILE*_tmp275=Cyc_stderr;struct _fat_ptr _tmp274=({const char*_tmp1AA=")";_tag_fat(_tmp1AA,sizeof(char),2U);});Cyc_fprintf(_tmp275,_tmp274,_tag_fat(_tmp1A9,sizeof(void*),0));});});goto _LL0;}case 6: _tmp193=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp190)->f1).is_union;_tmp194=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp190)->f1).fieldnum;_tmp195=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp190)->f2;{int is_union=_tmp193;int f=_tmp194;struct _fat_ptr d=_tmp195;
# 1087
if(is_union){
({void*_tmp1AB=0U;({struct Cyc___cycFILE*_tmp277=Cyc_stderr;struct _fat_ptr _tmp276=({const char*_tmp1AC="AggrUnion{";_tag_fat(_tmp1AC,sizeof(char),11U);});Cyc_fprintf(_tmp277,_tmp276,_tag_fat(_tmp1AB,sizeof(void*),0));});});
if(f != -1)
({struct Cyc_Int_pa_PrintArg_struct _tmp1AF=({struct Cyc_Int_pa_PrintArg_struct _tmp203;_tmp203.tag=1,_tmp203.f1=(unsigned long)f;_tmp203;});void*_tmp1AD[1];_tmp1AD[0]=& _tmp1AF;({struct Cyc___cycFILE*_tmp279=Cyc_stderr;struct _fat_ptr _tmp278=({const char*_tmp1AE="tag == %d;";_tag_fat(_tmp1AE,sizeof(char),11U);});Cyc_fprintf(_tmp279,_tmp278,_tag_fat(_tmp1AD,sizeof(void*),1));});});}else{
# 1092
({void*_tmp1B0=0U;({struct Cyc___cycFILE*_tmp27B=Cyc_stderr;struct _fat_ptr _tmp27A=({const char*_tmp1B1="AggrStruct{";_tag_fat(_tmp1B1,sizeof(char),12U);});Cyc_fprintf(_tmp27B,_tmp27A,_tag_fat(_tmp1B0,sizeof(void*),0));});});}
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)d.curr)[i]);
if((unsigned)(i + 1)< _get_fat_size(d,sizeof(void*)))({void*_tmp1B2=0U;({struct Cyc___cycFILE*_tmp27D=Cyc_stderr;struct _fat_ptr _tmp27C=({const char*_tmp1B3=",";_tag_fat(_tmp1B3,sizeof(char),2U);});Cyc_fprintf(_tmp27D,_tmp27C,_tag_fat(_tmp1B2,sizeof(void*),0));});});}}
# 1097
({void*_tmp1B4=0U;({struct Cyc___cycFILE*_tmp27F=Cyc_stderr;struct _fat_ptr _tmp27E=({const char*_tmp1B5="}";_tag_fat(_tmp1B5,sizeof(char),2U);});Cyc_fprintf(_tmp27F,_tmp27E,_tag_fat(_tmp1B4,sizeof(void*),0));});});
goto _LL0;}case 7: _tmp191=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp190)->f1;_tmp193=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp190)->f2;_tmp192=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp190)->f3;{struct Cyc_Absyn_Exp*e=_tmp191;int i=_tmp193;void*r=_tmp192;
# 1100
({struct Cyc_String_pa_PrintArg_struct _tmp1B8=({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0,({struct _fat_ptr _tmp280=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp205.f1=_tmp280;});_tmp205;});struct Cyc_Int_pa_PrintArg_struct _tmp1B9=({struct Cyc_Int_pa_PrintArg_struct _tmp204;_tmp204.tag=1,_tmp204.f1=(unsigned long)i;_tmp204;});void*_tmp1B6[2];_tmp1B6[0]=& _tmp1B8,_tmp1B6[1]=& _tmp1B9;({struct Cyc___cycFILE*_tmp282=Cyc_stderr;struct _fat_ptr _tmp281=({const char*_tmp1B7="[Consumed(%s,%d,";_tag_fat(_tmp1B7,sizeof(char),17U);});Cyc_fprintf(_tmp282,_tmp281,_tag_fat(_tmp1B6,sizeof(void*),2));});});
Cyc_CfFlowInfo_print_absrval(r);({void*_tmp1BA=0U;({struct Cyc___cycFILE*_tmp284=Cyc_stderr;struct _fat_ptr _tmp283=({const char*_tmp1BB=")]";_tag_fat(_tmp1BB,sizeof(char),3U);});Cyc_fprintf(_tmp284,_tmp283,_tag_fat(_tmp1BA,sizeof(void*),0));});});
goto _LL0;}default: _tmp191=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp190)->f1;_tmp192=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp190)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp191;void*r=_tmp192;
# 1104
({struct Cyc_String_pa_PrintArg_struct _tmp1BE=({struct Cyc_String_pa_PrintArg_struct _tmp206;_tmp206.tag=0,({struct _fat_ptr _tmp285=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n->name));_tmp206.f1=_tmp285;});_tmp206;});void*_tmp1BC[1];_tmp1BC[0]=& _tmp1BE;({struct Cyc___cycFILE*_tmp287=Cyc_stderr;struct _fat_ptr _tmp286=({const char*_tmp1BD="[NamedLocation(%s,";_tag_fat(_tmp1BD,sizeof(char),19U);});Cyc_fprintf(_tmp287,_tmp286,_tag_fat(_tmp1BC,sizeof(void*),1));});});
Cyc_CfFlowInfo_print_absrval(r);({void*_tmp1BF=0U;({struct Cyc___cycFILE*_tmp289=Cyc_stderr;struct _fat_ptr _tmp288=({const char*_tmp1C0=")]";_tag_fat(_tmp1C0,sizeof(char),3U);});Cyc_fprintf(_tmp289,_tmp288,_tag_fat(_tmp1BF,sizeof(void*),0));});});
goto _LL0;}}_LL0:;}
# 1110
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp1C1=0U;({struct Cyc___cycFILE*_tmp28B=Cyc_stderr;struct _fat_ptr _tmp28A=({const char*_tmp1C2="    ";_tag_fat(_tmp1C2,sizeof(char),5U);});Cyc_fprintf(_tmp28B,_tmp28A,_tag_fat(_tmp1C1,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp1C3=0U;({struct Cyc___cycFILE*_tmp28D=Cyc_stderr;struct _fat_ptr _tmp28C=({const char*_tmp1C4=" --> ";_tag_fat(_tmp1C4,sizeof(char),6U);});Cyc_fprintf(_tmp28D,_tmp28C,_tag_fat(_tmp1C3,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp1C5=0U;({struct Cyc___cycFILE*_tmp28F=Cyc_stderr;struct _fat_ptr _tmp28E=({const char*_tmp1C6="\n";_tag_fat(_tmp1C6,sizeof(char),2U);});Cyc_fprintf(_tmp28F,_tmp28E,_tag_fat(_tmp1C5,sizeof(void*),0));});});}
# 1118
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*)(void*,void*),struct Cyc_Dict_Dict))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1122
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp1C7=f;void*_tmp1C9;struct Cyc_Dict_Dict _tmp1C8;if((_tmp1C7.BottomFL).tag == 1){
({void*_tmp1CA=0U;({struct Cyc___cycFILE*_tmp291=Cyc_stderr;struct _fat_ptr _tmp290=({const char*_tmp1CB="  BottomFL\n";_tag_fat(_tmp1CB,sizeof(char),12U);});Cyc_fprintf(_tmp291,_tmp290,_tag_fat(_tmp1CA,sizeof(void*),0));});});goto _LL0;}else{_tmp1C8=((_tmp1C7.ReachableFL).val).f1;_tmp1C9=((_tmp1C7.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_tmp1C8;struct Cyc_List_List*rlns=_tmp1C9;
# 1126
({void*_tmp1CC=0U;({struct Cyc___cycFILE*_tmp293=Cyc_stderr;struct _fat_ptr _tmp292=({const char*_tmp1CD="  ReachableFL:\n";_tag_fat(_tmp1CD,sizeof(char),16U);});Cyc_fprintf(_tmp293,_tmp292,_tag_fat(_tmp1CC,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_flowdict(fd);
({void*_tmp1CE=0U;({struct Cyc___cycFILE*_tmp295=Cyc_stderr;struct _fat_ptr _tmp294=({const char*_tmp1CF="\n  Relations: ";_tag_fat(_tmp1CF,sizeof(char),15U);});Cyc_fprintf(_tmp295,_tmp294,_tag_fat(_tmp1CE,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,rlns);
({void*_tmp1D0=0U;({struct Cyc___cycFILE*_tmp297=Cyc_stderr;struct _fat_ptr _tmp296=({const char*_tmp1D1="\n";_tag_fat(_tmp1D1,sizeof(char),2U);});Cyc_fprintf(_tmp297,_tmp296,_tag_fat(_tmp1D0,sizeof(void*),0));});});
goto _LL0;}}_LL0:;}

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
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};char Cyc_CfFlowInfo_IsZero[7U]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8U]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9U]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;int f4;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 143 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 160
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*,struct _fat_ptr*);
# 162
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 165
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int,void*);
# 167
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*,void*,struct Cyc_Absyn_Exp*,int,void*,int);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*,int,void*,int*);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*,void*);
# 172
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*);
# 175
int Cyc_CfFlowInfo_is_init_pointer(void*);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 178
void Cyc_CfFlowInfo_print_absrval(void*);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel);
void Cyc_CfFlowInfo_print_root(void*);
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*);
# 184
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict);
# 207 "cf_flowinfo.h"
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
return({union Cyc_CfFlowInfo_AbsLVal _tmp1E6;(_tmp1E6.PlaceL).tag=1U,(_tmp1E6.PlaceL).val=x;_tmp1E6;});}
# 50
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void){
return({union Cyc_CfFlowInfo_AbsLVal _tmp1E7;(_tmp1E7.UnknownL).tag=2U,(_tmp1E7.UnknownL).val=0;_tmp1E7;});}
# 53
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void){return({union Cyc_CfFlowInfo_FlowInfo _tmp1E8;(_tmp1E8.BottomFL).tag=1U,(_tmp1E8.BottomFL).val=0;_tmp1E8;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp1E9;(_tmp1E9.ReachableFL).tag=2U,((_tmp1E9.ReachableFL).val).f1=fd,((_tmp1E9.ReachableFL).val).f2=r;_tmp1E9;});}
# 58
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp8=_cycalloc(sizeof(*_tmp8));
({void*_tmp227=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->tag=0;_tmp0;});_tmp8->zero=_tmp227;}),({
void*_tmp226=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=1;_tmp1;});_tmp8->notzeroall=_tmp226;}),({
void*_tmp225=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=2,_tmp2->f1=0U;_tmp2;});_tmp8->unknown_none=_tmp225;}),({
void*_tmp224=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=2,_tmp3->f1=1U;_tmp3;});_tmp8->unknown_all=_tmp224;}),({
void*_tmp223=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=3,_tmp4->f1=0U;_tmp4;});_tmp8->esc_none=_tmp223;}),({
void*_tmp222=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=3,_tmp5->f1=1U;_tmp5;});_tmp8->esc_all=_tmp222;}),({
struct Cyc_Dict_Dict _tmp221=({(struct Cyc_Dict_Dict(*)(int(*)(void*,void*)))Cyc_Dict_empty;})(Cyc_CfFlowInfo_root_cmp);_tmp8->mt_flowdict=_tmp221;}),({
struct Cyc_CfFlowInfo_Place*_tmp220=({struct Cyc_CfFlowInfo_Place*_tmp7=_cycalloc(sizeof(*_tmp7));({void*_tmp21F=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=1,_tmp6->f1=& dummy_exp,_tmp6->f2=Cyc_Absyn_void_type;_tmp6;});_tmp7->root=_tmp21F;}),_tmp7->path=0;_tmp7;});_tmp8->dummy_place=_tmp220;});_tmp8;});}
# 74
struct _fat_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp9=*place;struct Cyc_CfFlowInfo_Place _stmttmp0=_tmp9;struct Cyc_CfFlowInfo_Place _tmpA=_stmttmp0;void*_tmpC;void*_tmpB;_tmpB=_tmpA.root;_tmpC=_tmpA.path;{void*root=_tmpB;struct Cyc_List_List*fields=_tmpC;
void*_tmpD=root;void*_tmpE;if(*((int*)_tmpD)== 0){_tmpE=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpE;
# 78
if(fields == 0)
return Cyc_Absynpp_qvar2string(vd->name);else{
# 81
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0,({struct _fat_ptr _tmp228=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1EA.f1=_tmp228;});_tmp1EA;});void*_tmpF[1];_tmpF[0]=& _tmp11;({struct _fat_ptr _tmp229=({const char*_tmp10="reachable from %s";_tag_fat(_tmp10,sizeof(char),18U);});Cyc_aprintf(_tmp229,_tag_fat(_tmpF,sizeof(void*),1));});});}}}else{
({void*_tmp12=0U;({int(*_tmp22B)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp22A=({const char*_tmp13="error locations not for VarRoots";_tag_fat(_tmp13,sizeof(char),33U);});_tmp22B(_tmp22A,_tag_fat(_tmp12,sizeof(void*),0));});});};}}
# 87
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f){
int n=0;
for(1;(unsigned)fs;fs=fs->tl){
struct _fat_ptr*_tmp14=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;struct _fat_ptr*f2=_tmp14;
if(Cyc_strptrcmp(f2,f)== 0)return n;
n=n + 1;}
# 94
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0,_tmp1EB.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp1EB;});void*_tmp15[1];_tmp15[0]=& _tmp17;({int(*_tmp22D)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp22C=({const char*_tmp16="get_field_index_fs failed: %s";_tag_fat(_tmp16,sizeof(char),30U);});_tmp22D(_tmp22C,_tag_fat(_tmp15,sizeof(void*),1));});});}
# 97
int Cyc_CfFlowInfo_get_field_index(void*t,struct _fat_ptr*f){
void*_tmp18=Cyc_Absyn_compress(t);void*_stmttmp1=_tmp18;void*_tmp19=_stmttmp1;void*_tmp1A;union Cyc_Absyn_AggrInfo _tmp1B;switch(*((int*)_tmp19)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19)->f1)== 20){_tmp1B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp1B;
# 100
struct Cyc_List_List*_tmp1C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(info))->impl))->fields;struct Cyc_List_List*fs=_tmp1C;
_tmp1A=fs;goto _LL4;}}else{goto _LL5;}case 7: _tmp1A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp19)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A;
# 103
return Cyc_CfFlowInfo_get_field_index_fs(fs,f);}default: _LL5:
# 105
({struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp1EC;_tmp1EC.tag=0,({struct _fat_ptr _tmp22E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp1EC.f1=_tmp22E;});_tmp1EC;});void*_tmp1D[1];_tmp1D[0]=& _tmp1F;({int(*_tmp230)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp22F=({const char*_tmp1E="get_field_index failed: %s";_tag_fat(_tmp1E,sizeof(char),27U);});_tmp230(_tmp22F,_tag_fat(_tmp1D,sizeof(void*),1));});});};}struct _tuple12{void*f1;void*f2;};
# 109
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple12 _tmp20=({struct _tuple12 _tmp1ED;_tmp1ED.f1=r1,_tmp1ED.f2=r2;_tmp1ED;});struct _tuple12 _stmttmp2=_tmp20;struct _tuple12 _tmp21=_stmttmp2;int _tmp23;int _tmp22;void*_tmp25;void*_tmp24;if(*((int*)_tmp21.f1)== 0){if(*((int*)_tmp21.f2)== 0){_tmp24=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp25=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;{struct Cyc_Absyn_Vardecl*vd1=_tmp24;struct Cyc_Absyn_Vardecl*vd2=_tmp25;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_tmp21.f2)== 0)
return 1;else{if(*((int*)_tmp21.f1)== 1){if(*((int*)_tmp21.f2)== 1){_tmp24=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp25=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp24;struct Cyc_Absyn_Exp*e2=_tmp25;
return(int)e1 - (int)e2;}}else{
return -1;}}else{if(*((int*)_tmp21.f2)== 1)
return 1;else{_tmp22=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp23=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;{int i1=_tmp22;int i2=_tmp23;
return i1 - i2;}}}}};}}
# 123
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){
struct _tuple12 _tmp26=({struct _tuple12 _tmp1EE;_tmp1EE.f1=p1,_tmp1EE.f2=p2;_tmp1EE;});struct _tuple12 _stmttmp3=_tmp26;struct _tuple12 _tmp27=_stmttmp3;int _tmp29;int _tmp28;if(*((int*)_tmp27.f1)== 0){if(*((int*)_tmp27.f2)== 0){_tmp28=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp27.f1)->f1;_tmp29=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp27.f2)->f1;{int i1=_tmp28;int i2=_tmp29;
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
return({(int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_CfFlowInfo_pathcon_cmp,path1,path2);}
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
unsigned sz=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(fs);
struct _fat_ptr d=({unsigned _tmp37=sz;void**_tmp36=_cycalloc(_check_times(_tmp37,sizeof(void*)));({{unsigned _tmp1EF=sz;unsigned i;for(i=0;i < _tmp1EF;++ i){_tmp36[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp36,sizeof(void*),_tmp37);});
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp32=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_stmttmp6=_tmp32;struct Cyc_Absyn_Aggrfield*_tmp33=_stmttmp6;void*_tmp35;void*_tmp34;_tmp34=_tmp33->name;_tmp35=_tmp33->type;{struct _fat_ptr*n=_tmp34;void*t2=_tmp35;
if(_get_fat_size(*n,sizeof(char))!= 1U)
({void*_tmp231=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,t2,leafval);*((void**)_check_fat_subscript(d,sizeof(void*),i))=_tmp231;});}}}
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
inst=({struct Cyc_List_List*_tmp45=_region_malloc(temp,sizeof(*_tmp45));({struct _tuple14*_tmp232=({struct _tuple14*_tmp44=_region_malloc(temp,sizeof(*_tmp44));_tmp44->f1=tv,_tmp44->f2=t;_tmp44;});_tmp45->hd=_tmp232;}),_tmp45->tl=inst;_tmp45;});}
# 219
if(inst != 0){
struct _tuple13 _tmp46=({struct _tuple13 _tmp1F0;_tmp1F0.f1=temp,({struct Cyc_List_List*_tmp233=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(inst);_tmp1F0.f2=_tmp233;});_tmp1F0;});struct _tuple13 env=_tmp46;
struct Cyc_List_List*subs_fs=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c;})(temp,Cyc_CfFlowInfo_substitute_field,& env,fs);
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
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=6,(_tmp54->f1).is_union=(int)ad->kind == 1,(_tmp54->f1).fieldnum=-1,({
struct _fat_ptr _tmp234=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,ad->tvs,targs,fields,(int)ad->kind == 1,leafval);_tmp54->f2=_tmp234;});_tmp54;});}}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f2 != 0){_tmp4B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f2)->hd;{void*t=_tmp4B;
# 273
return leafval;}}else{goto _LLF;}default: goto _LLF;}case 6: _tmp4B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4A)->f1;_LL4: {struct Cyc_List_List*tqts=_tmp4B;
# 242
unsigned sz=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tqts);
struct _fat_ptr d=({unsigned _tmp51=sz;void**_tmp50=_cycalloc(_check_times(_tmp51,sizeof(void*)));({{unsigned _tmp1F1=sz;unsigned i;for(i=0;i < _tmp1F1;++ i){_tmp50[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp50,sizeof(void*),_tmp51);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp235=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2,leafval);*((void**)_check_fat_subscript(d,sizeof(void*),i))=_tmp235;});
tqts=tqts->tl;}}
# 248
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=6,(_tmp4F->f1).is_union=0,(_tmp4F->f1).fieldnum=-1,_tmp4F->f2=d;_tmp4F;});}case 7: _tmp4D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4A)->f1;_tmp4B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4A)->f2;_LL8: {enum Cyc_Absyn_AggrKind k=_tmp4D;struct Cyc_List_List*fs=_tmp4B;
# 265
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=6,(_tmp55->f1).is_union=(int)k == 1,(_tmp55->f1).fieldnum=-1,({
struct _fat_ptr _tmp236=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)k == 1,leafval);_tmp55->f2=_tmp236;});_tmp55;});}case 4: _tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).elt_type;_tmp4C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).zero_term;{void*et=_tmp4B;void*zeroterm=_tmp4C;
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
void*_tmp57=r;struct _fat_ptr _tmp5B;int _tmp5A;void*_tmp5C;int _tmp59;void*_tmp58;switch(*((int*)_tmp57)){case 7: _tmp58=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp57)->f1;_tmp59=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp57)->f2;_tmp5C=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp57)->f3;_tmp5A=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp57)->f4;{struct Cyc_Absyn_Exp*consumer=_tmp58;int iteration=_tmp59;void*r=_tmp5C;int local=_tmp5A;
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
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)d.curr)[field_no],needs_unconsume);}}case 8: _tmp58=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp57)->f2;{void*r=_tmp58;
# 313
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,r,needs_unconsume);}default:
 return 0;};}
# 320
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmp5E=r;struct _fat_ptr _tmp62;struct Cyc_CfFlowInfo_UnionRInfo _tmp61;int _tmp64;void*_tmp60;int _tmp63;void*_tmp5F;switch(*((int*)_tmp5E)){case 7: _tmp5F=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f1;_tmp63=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f2;_tmp60=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f3;_tmp64=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f4;{struct Cyc_Absyn_Exp*consumer=_tmp5F;int iteration=_tmp63;void*r=_tmp60;int local=_tmp64;
# 323
return r;}case 6: _tmp61=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f1;_tmp62=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f2;{struct Cyc_CfFlowInfo_UnionRInfo uinfo=_tmp61;struct _fat_ptr d=_tmp62;
# 325
unsigned _tmp65=_get_fat_size(d,sizeof(void*));unsigned sz=_tmp65;
int change=0;
struct _fat_ptr d2=({unsigned _tmp68=sz;void**_tmp67=_cycalloc(_check_times(_tmp68,sizeof(void*)));({{unsigned _tmp1F2=sz;unsigned i;for(i=0;i < _tmp1F2;++ i){_tmp67[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmp67,sizeof(void*),_tmp68);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp237=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)d.curr)[i]);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp237;});
if(((void**)d2.curr)[i]!= ((void**)d.curr)[i])
change=1;}}
# 333
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=6,_tmp66->f1=uinfo,_tmp66->f2=d2;_tmp66;});else{
return r;}}case 8: _tmp5F=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f1;_tmp60=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp5F;void*r2=_tmp60;
# 337
void*_tmp69=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,r2);void*r3=_tmp69;
if(r3 != r2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=8,_tmp6A->f1=n,_tmp6A->f2=r3;_tmp6A;});else{
return r;}}default:
 return r;};}
# 347
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r,int local_alias){
struct _tuple12 _tmp6B=({struct _tuple12 _tmp1F5;({void*_tmp238=Cyc_Absyn_compress(t);_tmp1F5.f1=_tmp238;}),_tmp1F5.f2=r;_tmp1F5;});struct _tuple12 _stmttmp9=_tmp6B;struct _tuple12 _tmp6C=_stmttmp9;enum Cyc_Absyn_AggrKind _tmp6D;union Cyc_Absyn_AggrInfo _tmp71;struct _fat_ptr _tmp70;struct Cyc_CfFlowInfo_UnionRInfo _tmp6F;void*_tmp72;void*_tmp6E;if(*((int*)_tmp6C.f2)== 8){_tmp6E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6C.f2)->f1;_tmp72=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6C.f2)->f2;{struct Cyc_Absyn_Vardecl*s=_tmp6E;void*r2=_tmp72;
# 350
void*_tmp73=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,r2,local_alias);void*r3=_tmp73;
if(r3 != r2)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=8,_tmp74->f1=s,_tmp74->f2=r3;_tmp74;});else{
return r;}}}else{switch(*((int*)_tmp6C.f1)){case 6: if(*((int*)_tmp6C.f2)== 6){_tmp6E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6C.f1)->f1;_tmp6F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6C.f2)->f1;_tmp70=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6C.f2)->f2;{struct Cyc_List_List*tqts=_tmp6E;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp6F;struct _fat_ptr d=_tmp70;
# 354
unsigned _tmp75=_get_fat_size(d,sizeof(void*));unsigned sz=_tmp75;
struct _fat_ptr d2=({unsigned _tmp78=sz;void**_tmp77=_cycalloc(_check_times(_tmp78,sizeof(void*)));({{unsigned _tmp1F3=sz;unsigned i;for(i=0;i < _tmp1F3;++ i){_tmp77[i]=fenv->unknown_all;}}0;});_tag_fat(_tmp77,sizeof(void*),_tmp78);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_tmp239=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2,consumer,iteration,((void**)d.curr)[i],local_alias);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp239;});
tqts=tqts->tl;}}
# 360
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=6,_tmp76->f1=b,_tmp76->f2=d2;_tmp76;});}}else{goto _LL9;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6C.f1)->f1)== 20){if(*((int*)_tmp6C.f2)== 6){_tmp71=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6C.f1)->f1)->f1;_tmp6F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6C.f2)->f1;_tmp70=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6C.f2)->f2;{union Cyc_Absyn_AggrInfo info=_tmp71;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp6F;struct _fat_ptr d=_tmp70;
# 362
struct Cyc_Absyn_Aggrdecl*_tmp79=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp79;
if(ad->impl == 0)return r;
_tmp6D=ad->kind;_tmp6E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp6F=b;_tmp70=d;goto _LL8;}}else{goto _LL9;}}else{goto _LL9;}case 7: if(*((int*)_tmp6C.f2)== 6){_tmp6D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6C.f1)->f1;_tmp6E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6C.f1)->f2;_tmp6F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6C.f2)->f1;_tmp70=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp6C.f2)->f2;_LL8: {enum Cyc_Absyn_AggrKind k=_tmp6D;struct Cyc_List_List*fs=_tmp6E;struct Cyc_CfFlowInfo_UnionRInfo b=_tmp6F;struct _fat_ptr d=_tmp70;
# 366
struct _fat_ptr d2=({unsigned _tmp80=_get_fat_size(d,sizeof(void*));void**_tmp7F=_cycalloc(_check_times(_tmp80,sizeof(void*)));({{unsigned _tmp1F4=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp1F4;++ i){_tmp7F[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmp7F,sizeof(void*),_tmp80);});
unsigned sz=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(fs);
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp7A=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_stmttmpA=_tmp7A;struct Cyc_Absyn_Aggrfield*_tmp7B=_stmttmpA;void*_tmp7D;void*_tmp7C;_tmp7C=_tmp7B->name;_tmp7D=_tmp7B->type;{struct _fat_ptr*n=_tmp7C;void*t2=_tmp7D;
if(_get_fat_size(*n,sizeof(char))!= 1U)
({void*_tmp23A=Cyc_CfFlowInfo_make_unique_consumed(fenv,t2,consumer,iteration,*((void**)_check_fat_subscript(d,sizeof(void*),i)),local_alias);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_tmp23A;});}}}
# 373
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=6,_tmp7E->f1=b,_tmp7E->f2=d2;_tmp7E;});}}else{goto _LL9;}default: _LL9:
# 375
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=7,_tmp81->f1=consumer,_tmp81->f2=iteration,_tmp81->f3=r,_tmp81->f4=local_alias;_tmp81;});
return r;}};}struct _tuple16{struct Cyc_CfFlowInfo_Place*f1;unsigned f2;};
# 381
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 383
struct _RegionHandle _tmp82=_new_region("r");struct _RegionHandle*r=& _tmp82;_push_region(r);
{struct _tuple16 elem=({struct _tuple16 _tmp1F6;_tmp1F6.f1=place,_tmp1F6.f2=0U;_tmp1F6;});
struct Cyc_Iter_Iter iter=({(struct Cyc_Iter_Iter(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_make_iter;})(r,set);
while(({(int(*)(struct Cyc_Iter_Iter,struct _tuple16*))Cyc_Iter_next;})(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp83=elem.f1;struct Cyc_CfFlowInfo_Place*place2=_tmp83;
struct Cyc_CfFlowInfo_Place _tmp84=*place;struct Cyc_CfFlowInfo_Place _stmttmpB=_tmp84;struct Cyc_CfFlowInfo_Place _tmp85=_stmttmpB;void*_tmp87;void*_tmp86;_tmp86=_tmp85.root;_tmp87=_tmp85.path;{void*root1=_tmp86;struct Cyc_List_List*fs1=_tmp87;
struct Cyc_CfFlowInfo_Place _tmp88=*place2;struct Cyc_CfFlowInfo_Place _stmttmpC=_tmp88;struct Cyc_CfFlowInfo_Place _tmp89=_stmttmpC;void*_tmp8B;void*_tmp8A;_tmp8A=_tmp89.root;_tmp8B=_tmp89.path;{void*root2=_tmp8A;struct Cyc_List_List*fs2=_tmp8B;
if(Cyc_CfFlowInfo_root_cmp(root1,root2)!= 0)
continue;
for(1;fs1 != 0 && fs2 != 0;(fs1=fs1->tl,fs2=fs2->tl)){
if((void*)fs1->hd != (void*)fs2->hd)break;}
if(fs1 == 0){
int _tmp8C=1;_npop_handler(0);return _tmp8C;}}}}{
# 397
int _tmp8D=0;_npop_handler(0);return _tmp8D;}}
# 384
;_pop_region();}struct Cyc_CfFlowInfo_EscPile{struct Cyc_List_List*places;};
# 407
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 410
if(!({(int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))Cyc_List_mem;})(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_tmp23B=({struct Cyc_List_List*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->hd=place,_tmp8E->tl=pile->places;_tmp8E;});pile->places=_tmp23B;});}
# 413
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp8F=r;struct _fat_ptr _tmp91;struct Cyc_CfFlowInfo_UnionRInfo _tmp90;void*_tmp92;switch(*((int*)_tmp8F)){case 7: _tmp92=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp8F)->f3;{void*r=_tmp92;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 8: _tmp92=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp8F)->f2;{void*r=_tmp92;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 4: _tmp92=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp8F)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp92;
Cyc_CfFlowInfo_add_place(pile,p);return;}case 6: _tmp90=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp8F)->f1;_tmp91=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp8F)->f2;{struct Cyc_CfFlowInfo_UnionRInfo b=_tmp90;struct _fat_ptr d=_tmp91;
# 419
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)d.curr)[i]);}}
return;}default:
 return;};}
# 429
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){
void*_tmp93=old_val;int _tmp97;void*_tmp95;void*_tmp94;struct _fat_ptr _tmp98;int _tmp96;switch(*((int*)_tmp93)){case 6: _tmp96=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp93)->f1).is_union;_tmp98=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp93)->f2;{int is_union=_tmp96;struct _fat_ptr d=_tmp98;
# 432
struct _fat_ptr d2=({unsigned _tmp9B=_get_fat_size(d,sizeof(void*));void**_tmp9A=_cycalloc(_check_times(_tmp9B,sizeof(void*)));({{unsigned _tmp1F7=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp1F7;++ i){({
void*_tmp23C=Cyc_CfFlowInfo_insert_place_inner(new_val,((void**)d.curr)[(int)i]);_tmp9A[i]=_tmp23C;});}}0;});_tag_fat(_tmp9A,sizeof(void*),_tmp9B);});
# 436
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=6,(_tmp99->f1).is_union=is_union,(_tmp99->f1).fieldnum=-1,_tmp99->f2=d2;_tmp99;});}case 7: _tmp94=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp93)->f1;_tmp96=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp93)->f2;_tmp95=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp93)->f3;_tmp97=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp93)->f4;{struct Cyc_Absyn_Exp*e=_tmp94;int i=_tmp96;void*rval=_tmp95;int l=_tmp97;
# 438
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=7,_tmp9C->f1=e,_tmp9C->f2=i,({void*_tmp23D=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_tmp9C->f3=_tmp23D;}),_tmp9C->f4=l;_tmp9C;});}case 8: _tmp94=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp93)->f1;_tmp95=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp93)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp94;void*rval=_tmp95;
# 440
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=8,_tmp9D->f1=n,({void*_tmp23E=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_tmp9D->f2=_tmp23E;});_tmp9D;});}default:
 return new_val;};}
# 447
static struct _fat_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _fat_ptr d,int n,void*rval){
void*_tmp9E=((void**)d.curr)[n];void*old_rval=_tmp9E;
if(old_rval == rval)return d;{
struct _fat_ptr res=({unsigned _tmpA0=_get_fat_size(d,sizeof(void*));void**_tmp9F=_cycalloc(_check_times(_tmpA0,sizeof(void*)));({{unsigned _tmp1F8=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _tmp1F8;++ i){_tmp9F[i]=((void**)d.curr)[(int)i];}}0;});_tag_fat(_tmp9F,sizeof(void*),_tmpA0);});
((void**)res.curr)[n]=rval;
return res;}}struct _tuple17{struct Cyc_List_List*f1;void*f2;};
# 460
static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*path,void*old_val,void*new_val){
# 462
if(path == 0)
return Cyc_CfFlowInfo_insert_place_inner(new_val,old_val);{
struct _tuple17 _tmpA1=({struct _tuple17 _tmp1F9;_tmp1F9.f1=path,_tmp1F9.f2=old_val;_tmp1F9;});struct _tuple17 _stmttmpD=_tmpA1;struct _tuple17 _tmpA2=_stmttmpD;void*_tmpA4;struct _fat_ptr _tmpA7;int _tmpA6;void*_tmpA3;int _tmpA5;if(_tmpA2.f1 != 0){if(*((int*)((struct Cyc_List_List*)_tmpA2.f1)->hd)== 0)switch(*((int*)_tmpA2.f2)){case 6: _tmpA5=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmpA2.f1)->hd)->f1;_tmpA3=(_tmpA2.f1)->tl;_tmpA6=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA2.f2)->f1).is_union;_tmpA7=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA2.f2)->f2;{int i=_tmpA5;struct Cyc_List_List*tl=_tmpA3;int is_union=_tmpA6;struct _fat_ptr d=_tmpA7;
# 466
void*_tmpA8=Cyc_CfFlowInfo_insert_place_outer(tl,((void**)d.curr)[i],new_val);void*new_child=_tmpA8;
struct _fat_ptr _tmpA9=Cyc_CfFlowInfo_aggr_dict_insert(d,i,new_child);struct _fat_ptr new_d=_tmpA9;
if((void**)new_d.curr == (void**)d.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=6,(_tmpAA->f1).is_union=is_union,(_tmpAA->f1).fieldnum=-1,_tmpAA->f2=new_d;_tmpAA;});}}case 8: goto _LL5;default: goto _LL7;}else{switch(*((int*)_tmpA2.f2)){case 5: _tmpA3=(_tmpA2.f1)->tl;_tmpA4=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpA2.f2)->f1;{struct Cyc_List_List*tl=_tmpA3;void*rval=_tmpA4;
# 471
void*_tmpAB=Cyc_CfFlowInfo_insert_place_outer(tl,rval,new_val);void*new_rval=_tmpAB;
if(new_rval == rval)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=5,_tmpAC->f1=new_rval;_tmpAC;});}}case 8: goto _LL5;default: goto _LL7;}}}else{if(*((int*)_tmpA2.f2)== 8){_LL5: _tmpA3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA2.f2)->f1;_tmpA4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA2.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_tmpA3;void*rval=_tmpA4;
# 475
void*_tmpAD=Cyc_CfFlowInfo_insert_place_outer(path,rval,new_val);void*new_rval=_tmpAD;
if(new_rval == rval)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=8,_tmpAE->f1=n,_tmpAE->f2=new_rval;_tmpAE;});}}}else{_LL7:
({void*_tmpAF=0U;({int(*_tmp240)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp23F=({const char*_tmpB0="bad insert place";_tag_fat(_tmpB0,sizeof(char),17U);});_tmp240(_tmp23F,_tag_fat(_tmpAF,sizeof(void*),0));});});}};}}
# 485
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmpB1=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;struct Cyc_CfFlowInfo_Place*place=_tmpB1;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;{
void*oldval;void*newval;
{struct _handler_cons _tmpB2;_push_handler(& _tmpB2);{int _tmpB4=0;if(setjmp(_tmpB2.handler))_tmpB4=1;if(!_tmpB4){oldval=Cyc_CfFlowInfo_lookup_place(d,place);;_pop_handler();}else{void*_tmpB3=(void*)Cyc_Core_get_exn_thrown();void*_tmpB5=_tmpB3;void*_tmpB6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpB5)->tag == Cyc_Dict_Absent)
continue;else{_tmpB6=_tmpB5;{void*exn=_tmpB6;(int)_rethrow(exn);}};}}}
{enum Cyc_CfFlowInfo_InitLevel _tmpB7=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _stmttmpE=_tmpB7;enum Cyc_CfFlowInfo_InitLevel _tmpB8=_stmttmpE;if(_tmpB8 == Cyc_CfFlowInfo_AllIL){
newval=fenv->esc_all;goto _LL5;}else{
newval=fenv->esc_none;goto _LL5;}_LL5:;}
# 496
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmpB9=*place;struct Cyc_CfFlowInfo_Place _stmttmpF=_tmpB9;struct Cyc_CfFlowInfo_Place _tmpBA=_stmttmpF;void*_tmpBC;void*_tmpBB;_tmpBB=_tmpBA.root;_tmpBC=_tmpBA.path;{void*root=_tmpBB;struct Cyc_List_List*path=_tmpBC;
d=({struct Cyc_Dict_Dict(*_tmp245)(struct Cyc_Dict_Dict,void*,void*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _tmp244=d;void*_tmp243=root;_tmp245(_tmp244,_tmp243,({
struct Cyc_List_List*_tmp242=path;void*_tmp241=
({(void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;})(d,root);
# 499
Cyc_CfFlowInfo_insert_place_outer(_tmp242,_tmp241,newval);}));});}}}}
# 503
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 513
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmpBD=r;void*_tmpBE;enum Cyc_CfFlowInfo_InitLevel _tmpBF;switch(*((int*)_tmpBD)){case 2: _tmpBF=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpBD)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmpBF;
return il;}case 3: _tmpBF=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmpBD)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmpBF;
return il;}case 0:
 goto _LL8;case 1: _LL8:
 return 1U;case 7: _tmpBE=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBD)->f3;{void*r=_tmpBE;
return 0U;}default:
({void*_tmpC0=0U;({int(*_tmp247)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp246=({const char*_tmpC1="initlevel_approx";_tag_fat(_tmpC1,sizeof(char),17U);});_tmp247(_tmp246,_tag_fat(_tmpC0,sizeof(void*),0));});});};}
# 523
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 525
enum Cyc_CfFlowInfo_InitLevel this_ans;
if((int)acc == 0)return 0U;
{void*_tmpC2=r;struct _fat_ptr _tmpC4;int _tmpC6;int _tmpC5;void*_tmpC3;switch(*((int*)_tmpC2)){case 8: _tmpC3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC2)->f2;{void*r=_tmpC3;
return Cyc_CfFlowInfo_initlevel_rec(env,r,acc);}case 6: _tmpC5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC2)->f1).is_union;_tmpC6=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC2)->f1).fieldnum;_tmpC4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC2)->f2;if(_tmpC5){int iu=_tmpC5;int f=_tmpC6;struct _fat_ptr d=_tmpC4;
# 533
unsigned _tmpC7=_get_fat_size(d,sizeof(void*));unsigned sz=_tmpC7;
this_ans=0U;
if(f == -1){
int i=0;for(0;(unsigned)i < sz;++ i){
if((int)Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[i],1U)== 1){
this_ans=1U;
break;}}}else{
# 543
if((int)Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[f],1U)== 1)
this_ans=1U;}
goto _LL0;}else{_tmpC4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC2)->f2;{struct _fat_ptr d=_tmpC4;
# 547
unsigned _tmpC8=_get_fat_size(d,sizeof(void*));unsigned sz=_tmpC8;
this_ans=1U;
{int i=0;for(0;(unsigned)i < sz;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[i],this_ans);}}
goto _LL0;}}case 4: _tmpC3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC2)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmpC3;
# 553
if(({(int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))Cyc_List_mem;})(Cyc_CfFlowInfo_place_cmp,env->seen,p))
this_ans=1U;else{
# 556
({struct Cyc_List_List*_tmp248=({struct Cyc_List_List*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->hd=p,_tmpC9->tl=env->seen;_tmpC9;});env->seen=_tmp248;});
this_ans=({struct Cyc_CfFlowInfo_InitlevelEnv*_tmp249=env;Cyc_CfFlowInfo_initlevel_rec(_tmp249,Cyc_CfFlowInfo_lookup_place(env->d,p),1U);});
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;}
# 560
goto _LL0;}case 5: _tmpC3=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpC2)->f1;{void*r=_tmpC3;
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,r,1U);goto _LL0;}default:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);goto _LL0;}_LL0:;}
# 564
return this_ans;}
# 566
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmpCA=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp1FA;_tmp1FA.d=d,_tmp1FA.seen=0;_tmp1FA;});struct Cyc_CfFlowInfo_InitlevelEnv env2=_tmpCA;
return Cyc_CfFlowInfo_initlevel_rec(& env2,r,1U);}
# 571
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmpCB=*place;struct Cyc_CfFlowInfo_Place _stmttmp10=_tmpCB;struct Cyc_CfFlowInfo_Place _tmpCC=_stmttmp10;void*_tmpCE;void*_tmpCD;_tmpCD=_tmpCC.root;_tmpCE=_tmpCC.path;{void*root=_tmpCD;struct Cyc_List_List*path=_tmpCE;
void*_tmpCF=({(void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;})(d,root);void*ans=_tmpCF;
for(1;path != 0;path=path->tl){
retry: {struct _tuple12 _tmpD0=({struct _tuple12 _tmp1FB;_tmp1FB.f1=ans,_tmp1FB.f2=(void*)path->hd;_tmp1FB;});struct _tuple12 _stmttmp11=_tmpD0;struct _tuple12 _tmpD1=_stmttmp11;int _tmpD5;struct _fat_ptr _tmpD4;struct Cyc_CfFlowInfo_UnionRInfo _tmpD3;void*_tmpD2;switch(*((int*)_tmpD1.f1)){case 8: _tmpD2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD1.f1)->f2;{void*rval=_tmpD2;
# 577
ans=rval;goto retry;}case 7: _tmpD2=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD1.f1)->f3;{void*rval=_tmpD2;
# 580
ans=rval;goto retry;}case 6: if(*((int*)_tmpD1.f2)== 0){_tmpD3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD1.f1)->f1;_tmpD4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD1.f1)->f2;_tmpD5=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmpD1.f2)->f1;{struct Cyc_CfFlowInfo_UnionRInfo is_union=_tmpD3;struct _fat_ptr d2=_tmpD4;int fname=_tmpD5;
# 582
ans=*((void**)_check_fat_subscript(d2,sizeof(void*),fname));
goto _LL3;}}else{goto _LLC;}case 5: if(*((int*)_tmpD1.f2)== 1){_tmpD2=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpD1.f1)->f1;{void*rval=_tmpD2;
# 585
ans=rval;
goto _LL3;}}else{goto _LLC;}default: _LLC:
# 593
({void*_tmpD6=0U;({int(*_tmp24B)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp24A=({const char*_tmpD7="bad lookup_place";_tag_fat(_tmpD7,sizeof(char),17U);});_tmp24B(_tmp24A,_tag_fat(_tmpD6,sizeof(void*),0));});});}_LL3:;}}
# 595
return ans;}}
# 598
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmpD8=rval;struct _fat_ptr _tmpDB;int _tmpDA;int _tmpD9;void*_tmpDC;switch(*((int*)_tmpD8)){case 3:
 return 0;case 7: _tmpDC=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD8)->f3;{void*r=_tmpDC;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 8: _tmpDC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD8)->f2;{void*r=_tmpDC;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 6: _tmpD9=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD8)->f1).is_union;_tmpDA=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD8)->f1).fieldnum;_tmpDB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD8)->f2;{int is_union=_tmpD9;int field_no=_tmpDA;struct _fat_ptr d=_tmpDB;
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
void*_tmpDD=rval;void*_tmpDE;switch(*((int*)_tmpDD)){case 8: _tmpDE=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpDD)->f2;{void*r=_tmpDE;
return Cyc_CfFlowInfo_is_init_pointer(r);}case 4:
 goto _LL6;case 5: _LL6:
 return 1;default:
 return 0;};}
# 628
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle _tmpDF=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDF;_push_region(rgn);
{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->places=0;_tmpE1;});
Cyc_CfFlowInfo_add_places(pile,r);{
struct Cyc_Dict_Dict _tmpE0=Cyc_CfFlowInfo_escape_these(fenv,pile,d);_npop_handler(0);return _tmpE0;}}
# 630
;_pop_region();}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 642
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 647
struct _tuple12 _tmpE2=({struct _tuple12 _tmp1FD;_tmp1FD.f1=oldval,_tmp1FD.f2=newval;_tmp1FD;});struct _tuple12 _stmttmp12=_tmpE2;struct _tuple12 _tmpE3=_stmttmp12;enum Cyc_CfFlowInfo_InitLevel _tmpE4;struct _fat_ptr _tmpE8;struct Cyc_CfFlowInfo_UnionRInfo _tmpE7;struct _fat_ptr _tmpE6;struct Cyc_CfFlowInfo_UnionRInfo _tmpE5;void*_tmpEA;void*_tmpE9;if(*((int*)_tmpE3.f1)== 8){_tmpE9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE3.f1)->f2;{void*r1=_tmpE9;
# 649
return Cyc_CfFlowInfo_assign_place_inner(env,r1,newval);}}else{if(*((int*)_tmpE3.f2)== 8){_tmpE9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE3.f2)->f1;_tmpEA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE3.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_tmpE9;void*r=_tmpEA;
# 651
void*_tmpEB=Cyc_CfFlowInfo_assign_place_inner(env,oldval,r);void*new_rval=_tmpEB;
if(new_rval == r)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=8,_tmpEC->f1=n,_tmpEC->f2=new_rval;_tmpEC;});}}}else{switch(*((int*)_tmpE3.f1)){case 3: if(*((int*)_tmpE3.f2)== 4){_tmpE9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpE3.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmpE9;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL8;}}else{_LL8:
# 656
 if((int)Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= 1)
({void*_tmpED=0U;({unsigned _tmp24D=env->loc;struct _fat_ptr _tmp24C=({const char*_tmpEE="assignment puts possibly-uninitialized data in an escaped location";_tag_fat(_tmpEE,sizeof(char),67U);});Cyc_CfFlowInfo_aerr(_tmp24D,_tmp24C,_tag_fat(_tmpED,sizeof(void*),0));});});
# 659
return(env->fenv)->esc_all;}case 6: switch(*((int*)_tmpE3.f2)){case 6: _tmpE5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE3.f1)->f1;_tmpE6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE3.f1)->f2;_tmpE7=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE3.f2)->f1;_tmpE8=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE3.f2)->f2;{struct Cyc_CfFlowInfo_UnionRInfo is_union1=_tmpE5;struct _fat_ptr d1=_tmpE6;struct Cyc_CfFlowInfo_UnionRInfo is_union2=_tmpE7;struct _fat_ptr d2=_tmpE8;
# 661
struct _fat_ptr new_d=({unsigned _tmpF1=
_get_fat_size(d1,sizeof(void*));void**_tmpF0=_cycalloc(_check_times(_tmpF1,sizeof(void*)));({{unsigned _tmp1FC=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _tmp1FC;++ i){({void*_tmp24E=Cyc_CfFlowInfo_assign_place_inner(env,((void**)d1.curr)[(int)i],*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));_tmpF0[i]=_tmp24E;});}}0;});_tag_fat(_tmpF0,sizeof(void*),_tmpF1);});
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
if(({void*_tmp24F=*((void**)_check_fat_subscript(new_d,sizeof(void*),i));_tmp24F != *((void**)_check_fat_subscript(d2,sizeof(void*),i));})){
change=1;break;}}}
# 680
if(!change){
if(!is_union1.is_union)return newval;
new_d=d2;}}
# 685
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->tag=6,_tmpEF->f1=is_union2,_tmpEF->f2=new_d;_tmpEF;});}case 3: goto _LLB;default: goto _LLD;}default: if(*((int*)_tmpE3.f2)== 3){_LLB: _tmpE4=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmpE3.f2)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmpE4;
# 687
enum Cyc_CfFlowInfo_InitLevel _tmpF2=il;if(_tmpF2 == Cyc_CfFlowInfo_NoneIL)
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
struct _tuple17 _tmpF3=({struct _tuple17 _tmp1FE;_tmp1FE.f1=path,_tmp1FE.f2=oldval;_tmp1FE;});struct _tuple17 _stmttmp13=_tmpF3;struct _tuple17 _tmpF4=_stmttmp13;struct _fat_ptr _tmpF9;int _tmpF8;int _tmpF7;int _tmpF5;void*_tmpFA;void*_tmpF6;switch(*((int*)_tmpF4.f2)){case 8: _tmpF6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f1;_tmpFA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_tmpF6;void*r=_tmpFA;
# 715
void*_tmpFB=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);void*new_r=_tmpFB;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=8,_tmpFC->f1=n,_tmpFC->f2=new_r;_tmpFC;});}}case 7: _tmpF6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f1;_tmpF5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f2;_tmpFA=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f3;_tmpF7=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f4;{struct Cyc_Absyn_Exp*x=_tmpF6;int y=_tmpF5;void*r=_tmpFA;int l=_tmpF7;
# 719
void*_tmpFD=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);void*new_r=_tmpFD;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->tag=7,_tmpFE->f1=x,_tmpFE->f2=y,_tmpFE->f3=new_r,_tmpFE->f4=l;_tmpFE;});}}default: if(_tmpF4.f1 != 0){if(*((int*)((struct Cyc_List_List*)_tmpF4.f1)->hd)== 1){if(*((int*)_tmpF4.f2)== 5){_tmpF6=(_tmpF4.f1)->tl;_tmpFA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f1;{struct Cyc_List_List*tl=_tmpF6;void*r=_tmpFA;
# 723
void*_tmpFF=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,r,newval);void*new_r=_tmpFF;
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->tag=5,_tmp100->f1=new_r;_tmp100;});}}}else{goto _LL9;}}else{if(*((int*)_tmpF4.f2)== 6){_tmpF5=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmpF4.f1)->hd)->f1;_tmpF6=(_tmpF4.f1)->tl;_tmpF7=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f1).is_union;_tmpF8=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f1).fieldnum;_tmpF9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF4.f2)->f2;{int fnum=_tmpF5;struct Cyc_List_List*tl=_tmpF6;int is_union=_tmpF7;int fldnum=_tmpF8;struct _fat_ptr d=_tmpF9;
# 727
void*_tmp101=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,((void**)d.curr)[fnum],newval);void*new_child=_tmp101;
# 729
struct _fat_ptr _tmp102=Cyc_CfFlowInfo_aggr_dict_insert(d,fnum,new_child);struct _fat_ptr new_child_agg=_tmp102;
if((void**)new_child_agg.curr == (void**)d.curr &&(!is_union || fldnum == fnum))return oldval;
d=new_child_agg;
# 734
if(is_union){
int changed=0;
int sz=(int)_get_fat_size(d,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != fnum){
struct _fat_ptr _tmp103=({
struct _fat_ptr _tmp251=d;int _tmp250=i;Cyc_CfFlowInfo_aggr_dict_insert(_tmp251,_tmp250,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->unknown_all,((void**)d.curr)[i]));});
# 739
struct _fat_ptr new_d=_tmp103;
# 742
if((void**)new_d.curr != (void**)d.curr){
d=new_d;
changed=1;}}}}
# 750
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp104=env->root_place;struct Cyc_CfFlowInfo_Place*_stmttmp14=_tmp104;struct Cyc_CfFlowInfo_Place*_tmp105=_stmttmp14;void*_tmp107;void*_tmp106;_tmp106=_tmp105->root;_tmp107=_tmp105->path;{void*root=_tmp106;struct Cyc_List_List*path=_tmp107;
struct Cyc_List_List*_tmp108=({struct Cyc_List_List*(*_tmp254)(int(*)(int*,void*),int*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(int(*)(int*,void*),int*,struct Cyc_List_List*))Cyc_List_filter_c;});int(*_tmp253)(int*,void*)=({(int(*)(int*,void*))Cyc_CfFlowInfo_nprefix;});int*_tmp252=({int*_tmp10B=_cycalloc_atomic(sizeof(*_tmp10B));*_tmp10B=path_prefix;_tmp10B;});_tmp254(_tmp253,_tmp252,path);});struct Cyc_List_List*new_path=_tmp108;
struct Cyc_CfFlowInfo_Place*_tmp109=({struct Cyc_CfFlowInfo_Place*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->root=root,_tmp10A->path=new_path;_tmp10A;});struct Cyc_CfFlowInfo_Place*curr_place=_tmp109;;}}}
# 756
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->tag=6,(_tmp10C->f1).is_union=is_union,(_tmp10C->f1).fieldnum=fnum,_tmp10C->f2=d;_tmp10C;});}}else{goto _LL9;}}}else{_LL9:
({void*_tmp10D=0U;({int(*_tmp256)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp255=({const char*_tmp10E="bad assign place";_tag_fat(_tmp10E,sizeof(char),17U);});_tmp256(_tmp255,_tag_fat(_tmp10D,sizeof(void*),0));});});}};}}
# 760
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 768
struct Cyc_CfFlowInfo_Place*_tmp10F=place;void*_tmp111;void*_tmp110;_tmp110=_tmp10F->root;_tmp111=_tmp10F->path;{void*root=_tmp110;struct Cyc_List_List*path=_tmp111;
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp1FF;_tmp1FF.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp257=({struct Cyc_CfFlowInfo_EscPile*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->places=0;_tmp112;});_tmp1FF.pile=_tmp257;}),_tmp1FF.d=d,_tmp1FF.loc=loc,_tmp1FF.root_place=place;_tmp1FF;});
void*newval=({struct Cyc_List_List*_tmp259=path;void*_tmp258=({(void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;})(d,root);Cyc_CfFlowInfo_assign_place_outer(& env,_tmp259,0,_tmp258,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp25B=fenv;struct Cyc_CfFlowInfo_EscPile*_tmp25A=env.pile;Cyc_CfFlowInfo_escape_these(_tmp25B,_tmp25A,({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert;})(d,root,newval));});}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;int is_try_flow;};
# 784
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*,void*,void*);
# 786
static int Cyc_CfFlowInfo_contains_local_consumed(void*r){
void*_tmp113=r;void*_tmp114;int _tmp115;switch(*((int*)_tmp113)){case 7: _tmp115=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp113)->f4;{int l=_tmp115;
# 789
return l;}case 5: _tmp114=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp113)->f1;{void*r1=_tmp114;
# 791
return Cyc_CfFlowInfo_contains_local_consumed(r1);}case 8: _tmp114=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp113)->f2;{void*ac=_tmp114;
# 793
return Cyc_CfFlowInfo_contains_local_consumed(ac);}case 6:
# 795
 return 0;default:
# 797
 return 0;};}
# 802
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*);struct _tuple18{int f1;int f2;};
# 808
static void*Cyc_CfFlowInfo_join_absRval_tryflow(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _tuple18 _tmp116=({struct _tuple18 _tmp200;({int _tmp25D=Cyc_CfFlowInfo_contains_local_consumed(r1);_tmp200.f1=_tmp25D;}),({int _tmp25C=Cyc_CfFlowInfo_contains_local_consumed(r2);_tmp200.f2=_tmp25C;});_tmp200;});struct _tuple18 _stmttmp15=_tmp116;struct _tuple18 _tmp117=_stmttmp15;switch((int)_tmp117.f1){case 1: if(_tmp117.f2 == 0)
return r2;else{goto _LL5;}case 0: if(_tmp117.f2 == 1)
return r1;else{goto _LL5;}default: _LL5:
# 814
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);};}}struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 824
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;
# 827
{struct _tuple12 _tmp118=({struct _tuple12 _tmp202;_tmp202.f1=r1,_tmp202.f2=r2;_tmp202;});struct _tuple12 _stmttmp16=_tmp118;struct _tuple12 _tmp119=_stmttmp16;struct _fat_ptr _tmp11F;struct _fat_ptr _tmp11C;int _tmp11E;int _tmp11D;int _tmp11B;int _tmp11A;void*_tmp123;void*_tmp122;void*_tmp121;void*_tmp120;if(*((int*)_tmp119.f1)== 8){if(*((int*)_tmp119.f2)== 8){_tmp120=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;_tmp121=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f2;_tmp122=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;_tmp123=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f2;{struct Cyc_Absyn_Vardecl*n1=_tmp120;void*r1=_tmp121;struct Cyc_Absyn_Vardecl*n2=_tmp122;void*r2=_tmp123;
# 830
if(n1 == n2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=8,_tmp124->f1=n1,({void*_tmp25E=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp124->f2=_tmp25E;});_tmp124;});else{
# 833
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{_tmp120=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;_tmp121=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f2;{struct Cyc_Absyn_Vardecl*n1=_tmp120;void*r1=_tmp121;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{if(*((int*)_tmp119.f2)== 8){_tmp120=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;_tmp121=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f2;{struct Cyc_Absyn_Vardecl*n2=_tmp120;void*r2=_tmp121;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}else{if(*((int*)_tmp119.f1)== 7){if(*((int*)_tmp119.f2)== 7){_tmp120=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;_tmp11A=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f2;_tmp121=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f3;_tmp11B=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f4;_tmp122=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;_tmp11D=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f2;_tmp123=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f3;_tmp11E=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f4;{struct Cyc_Absyn_Exp*e1=_tmp120;int i1=_tmp11A;void*r1=_tmp121;int l1=_tmp11B;struct Cyc_Absyn_Exp*e2=_tmp122;int i2=_tmp11D;void*r2=_tmp123;int l2=_tmp11E;
# 838
if(e1 == e2)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=7,_tmp125->f1=e1,i1 > i2?_tmp125->f2=i1:(_tmp125->f2=i2),({void*_tmp25F=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp125->f3=_tmp25F;}),_tmp125->f4=l1 && l2;_tmp125;});
{void*_tmp126=r1;void*_tmp127;if(*((int*)_tmp126)== 4){_tmp127=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp126)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp127;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL21;}}else{
goto _LL21;}_LL21:;}
# 844
{void*_tmp128=r2;void*_tmp129;if(*((int*)_tmp128)== 4){_tmp129=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp128)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp129;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL26;}}else{
goto _LL26;}_LL26:;}
# 848
goto _LL0;}}else{_tmp120=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;_tmp11A=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f2;_tmp121=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f3;_tmp11B=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f4;{struct Cyc_Absyn_Exp*e1=_tmp120;int i1=_tmp11A;void*r1=_tmp121;int l1=_tmp11B;
# 850
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=7,_tmp12A->f1=e1,_tmp12A->f2=i1,({void*_tmp260=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp12A->f3=_tmp260;}),_tmp12A->f4=l1;_tmp12A;});}}}else{if(*((int*)_tmp119.f2)== 7){_tmp120=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;_tmp11A=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f2;_tmp121=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f3;_tmp11B=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f4;{struct Cyc_Absyn_Exp*e2=_tmp120;int i2=_tmp11A;void*r2=_tmp121;int l2=_tmp11B;
# 852
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=7,_tmp12B->f1=e2,_tmp12B->f2=i2,({void*_tmp261=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp12B->f3=_tmp261;}),_tmp12B->f4=l2;_tmp12B;});}}else{switch(*((int*)_tmp119.f1)){case 4: switch(*((int*)_tmp119.f2)){case 4: _tmp120=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;_tmp121=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;{struct Cyc_CfFlowInfo_Place*p1=_tmp120;struct Cyc_CfFlowInfo_Place*p2=_tmp121;
# 855
if(Cyc_CfFlowInfo_place_cmp(p1,p2)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,p1);
Cyc_CfFlowInfo_add_place(env->pile,p2);
goto _LL0;}case 1: _tmp120=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp120;
# 861
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _tmp12C=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _stmttmp17=_tmp12C;enum Cyc_CfFlowInfo_InitLevel _tmp12D=_stmttmp17;if(_tmp12D == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}}default: _tmp120=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp120;
# 887
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}case 1: switch(*((int*)_tmp119.f2)){case 4: _tmp120=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp120;
# 867
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _tmp12E=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _stmttmp18=_tmp12E;enum Cyc_CfFlowInfo_InitLevel _tmp12F=_stmttmp18;if(_tmp12F == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}}case 5: _tmp120=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;{void*r2=_tmp120;
# 882
enum Cyc_CfFlowInfo_InitLevel _tmp133=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _stmttmp1A=_tmp133;enum Cyc_CfFlowInfo_InitLevel _tmp134=_stmttmp1A;if(_tmp134 == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}default: goto _LL1F;}case 5: switch(*((int*)_tmp119.f2)){case 5: _tmp120=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;_tmp121=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;{void*r1=_tmp120;void*r2=_tmp121;
# 874
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=5,({void*_tmp262=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_tmp130->f1=_tmp262;});_tmp130;});}case 1: _tmp120=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1;{void*r1=_tmp120;
# 877
enum Cyc_CfFlowInfo_InitLevel _tmp131=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _stmttmp19=_tmp131;enum Cyc_CfFlowInfo_InitLevel _tmp132=_stmttmp19;if(_tmp132 == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}case 4: goto _LL1B;default: goto _LL1F;}default: if(*((int*)_tmp119.f2)== 4){_LL1B: _tmp120=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp120;
# 888
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}else{if(*((int*)_tmp119.f1)== 6){if(*((int*)_tmp119.f2)== 6){_tmp11A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1).is_union;_tmp11B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f1).fieldnum;_tmp11C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp119.f1)->f2;_tmp11D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1).is_union;_tmp11E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f1).fieldnum;_tmp11F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp119.f2)->f2;{int is_union1=_tmp11A;int field_no1=_tmp11B;struct _fat_ptr d1=_tmp11C;int is_union2=_tmp11D;int field_no2=_tmp11E;struct _fat_ptr d2=_tmp11F;
# 891
struct _fat_ptr new_d=({unsigned _tmp137=
_get_fat_size(d1,sizeof(void*));void**_tmp136=_cycalloc(_check_times(_tmp137,sizeof(void*)));({{unsigned _tmp201=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _tmp201;++ i){env->is_try_flow?({void*_tmp264=({(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval_tryflow;})(env,0,((void**)d1.curr)[(int)i],((void**)d2.curr)[(int)i]);_tmp136[i]=_tmp264;}):({
void*_tmp263=Cyc_CfFlowInfo_join_absRval(env,a,((void**)d1.curr)[(int)i],((void**)d2.curr)[(int)i]);_tmp136[i]=_tmp263;});}}0;});_tag_fat(_tmp136,sizeof(void*),_tmp137);});
# 897
int change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(((void**)new_d.curr)[i]!= ((void**)d1.curr)[i]){
change=1;break;}}}
# 902
if(!change){
if(!is_union1)return r1;
new_d=d1;}else{
# 907
change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(((void**)new_d.curr)[i]!= ((void**)d2.curr)[i]){
change=1;break;}}}
# 912
if(!change){
if(!is_union1)return r2;
new_d=d2;}}
# 917
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->tag=6,(_tmp135->f1).is_union=is_union1,field_no1 == field_no2?(_tmp135->f1).fieldnum=field_no1:((_tmp135->f1).fieldnum=-1),_tmp135->f2=new_d;_tmp135;});}}else{goto _LL1F;}}else{_LL1F:
# 919
 goto _LL0;}}}}}}}_LL0:;}{
# 921
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple12 _tmp138=({struct _tuple12 _tmp205;_tmp205.f1=r1,_tmp205.f2=r2;_tmp205;});struct _tuple12 _stmttmp1B=_tmp138;struct _tuple12 _tmp139=_stmttmp1B;if(*((int*)_tmp139.f1)== 3)
goto _LL43;else{if(*((int*)_tmp139.f2)== 3){_LL43: {
# 926
struct _tuple19 _tmp13A=({struct _tuple19 _tmp203;_tmp203.f1=il1,_tmp203.f2=il2;_tmp203;});struct _tuple19 _stmttmp1C=_tmp13A;struct _tuple19 _tmp13B=_stmttmp1C;if(_tmp13B.f2 == Cyc_CfFlowInfo_NoneIL)
goto _LL4A;else{if(_tmp13B.f1 == Cyc_CfFlowInfo_NoneIL){_LL4A: return(env->fenv)->esc_none;}else{
return(env->fenv)->esc_all;}};}}else{
# 931
struct _tuple19 _tmp13C=({struct _tuple19 _tmp204;_tmp204.f1=il1,_tmp204.f2=il2;_tmp204;});struct _tuple19 _stmttmp1D=_tmp13C;struct _tuple19 _tmp13D=_stmttmp1D;if(_tmp13D.f2 == Cyc_CfFlowInfo_NoneIL)
goto _LL51;else{if(_tmp13D.f1 == Cyc_CfFlowInfo_NoneIL){_LL51: return(env->fenv)->unknown_none;}else{
return(env->fenv)->unknown_all;}};}};}}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 942
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_tryflow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo newflow,union Cyc_CfFlowInfo_FlowInfo oldflow){
# 944
struct _tuple20 _tmp13E=({struct _tuple20 _tmp207;_tmp207.f1=newflow,_tmp207.f2=oldflow;_tmp207;});struct _tuple20 _stmttmp1E=_tmp13E;struct _tuple20 _tmp13F=_stmttmp1E;void*_tmp143;struct Cyc_Dict_Dict _tmp142;void*_tmp141;struct Cyc_Dict_Dict _tmp140;if(((_tmp13F.f1).BottomFL).tag == 1)
return oldflow;else{if(((_tmp13F.f2).BottomFL).tag == 1)
return newflow;else{_tmp140=(((_tmp13F.f1).ReachableFL).val).f1;_tmp141=(((_tmp13F.f1).ReachableFL).val).f2;_tmp142=(((_tmp13F.f2).ReachableFL).val).f1;_tmp143=(((_tmp13F.f2).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp140;struct Cyc_List_List*r1=_tmp141;struct Cyc_Dict_Dict d2=_tmp142;struct Cyc_List_List*r2=_tmp143;
# 950
if(d1.t == d2.t && r1 == r2)return newflow;
if(Cyc_CfFlowInfo_flow_lessthan_approx(newflow,oldflow))return oldflow;
if(Cyc_CfFlowInfo_flow_lessthan_approx(oldflow,newflow))return newflow;{
struct Cyc_CfFlowInfo_JoinEnv _tmp144=({struct Cyc_CfFlowInfo_JoinEnv _tmp206;_tmp206.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp265=({struct Cyc_CfFlowInfo_EscPile*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->places=0;_tmp147;});_tmp206.pile=_tmp265;}),_tmp206.d1=d1,_tmp206.d2=d2,_tmp206.is_try_flow=1;_tmp206;});struct Cyc_CfFlowInfo_JoinEnv env=_tmp144;
struct Cyc_Dict_Dict _tmp145=({(struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;})(({(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*))Cyc_CfFlowInfo_join_absRval_tryflow;}),& env,d1,d2);struct Cyc_Dict_Dict outdict=_tmp145;
struct Cyc_List_List*_tmp146=Cyc_Relations_join_relns(Cyc_Core_heap_region,r1,r2);struct Cyc_List_List*r=_tmp146;
return({struct Cyc_Dict_Dict _tmp266=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outdict);Cyc_CfFlowInfo_ReachableFL(_tmp266,r);});}}}};}
# 960
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 962
struct _tuple20 _tmp148=({struct _tuple20 _tmp209;_tmp209.f1=f1,_tmp209.f2=f2;_tmp209;});struct _tuple20 _stmttmp1F=_tmp148;struct _tuple20 _tmp149=_stmttmp1F;void*_tmp14D;struct Cyc_Dict_Dict _tmp14C;void*_tmp14B;struct Cyc_Dict_Dict _tmp14A;if(((_tmp149.f1).BottomFL).tag == 1)
return f2;else{if(((_tmp149.f2).BottomFL).tag == 1)
return f1;else{_tmp14A=(((_tmp149.f1).ReachableFL).val).f1;_tmp14B=(((_tmp149.f1).ReachableFL).val).f2;_tmp14C=(((_tmp149.f2).ReachableFL).val).f1;_tmp14D=(((_tmp149.f2).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp14A;struct Cyc_List_List*r1=_tmp14B;struct Cyc_Dict_Dict d2=_tmp14C;struct Cyc_List_List*r2=_tmp14D;
# 968
if(d1.t == d2.t && r1 == r2)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct Cyc_CfFlowInfo_JoinEnv _tmp14E=({struct Cyc_CfFlowInfo_JoinEnv _tmp208;_tmp208.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp267=({struct Cyc_CfFlowInfo_EscPile*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->places=0;_tmp151;});_tmp208.pile=_tmp267;}),_tmp208.d1=d1,_tmp208.d2=d2,_tmp208.is_try_flow=0;_tmp208;});struct Cyc_CfFlowInfo_JoinEnv env=_tmp14E;
struct Cyc_Dict_Dict _tmp14F=({(struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;})(({(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*))Cyc_CfFlowInfo_join_absRval;}),& env,d1,d2);struct Cyc_Dict_Dict outdict=_tmp14F;
struct Cyc_List_List*_tmp150=Cyc_Relations_join_relns(Cyc_Core_heap_region,r1,r2);struct Cyc_List_List*r=_tmp150;
return({struct Cyc_Dict_Dict _tmp268=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outdict);Cyc_CfFlowInfo_ReachableFL(_tmp268,r);});}}}};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 978
struct _tuple21 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple21 pr1,struct _tuple21 pr2){
# 983
struct _tuple21 _tmp152=pr1;void*_tmp154;union Cyc_CfFlowInfo_FlowInfo _tmp153;_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp153;void*r1=_tmp154;
struct _tuple21 _tmp155=pr2;void*_tmp157;union Cyc_CfFlowInfo_FlowInfo _tmp156;_tmp156=_tmp155.f1;_tmp157=_tmp155.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp156;void*r2=_tmp157;
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);
struct _tuple22 _tmp158=({struct _tuple22 _tmp210;_tmp210.f1=f1,_tmp210.f2=f2,_tmp210.f3=outflow;_tmp210;});struct _tuple22 _stmttmp20=_tmp158;struct _tuple22 _tmp159=_stmttmp20;void*_tmp15D;struct Cyc_Dict_Dict _tmp15C;struct Cyc_Dict_Dict _tmp15B;struct Cyc_Dict_Dict _tmp15A;if(((_tmp159.f1).BottomFL).tag == 1)
return({struct _tuple21 _tmp20A;_tmp20A.f1=outflow,_tmp20A.f2=r2;_tmp20A;});else{if(((_tmp159.f2).BottomFL).tag == 1)
return({struct _tuple21 _tmp20B;_tmp20B.f1=outflow,_tmp20B.f2=r1;_tmp20B;});else{if(((_tmp159.f3).ReachableFL).tag == 2){_tmp15A=(((_tmp159.f1).ReachableFL).val).f1;_tmp15B=(((_tmp159.f2).ReachableFL).val).f1;_tmp15C=(((_tmp159.f3).ReachableFL).val).f1;_tmp15D=(((_tmp159.f3).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp15A;struct Cyc_Dict_Dict d2=_tmp15B;struct Cyc_Dict_Dict outd=_tmp15C;struct Cyc_List_List*relns=_tmp15D;
# 991
if(({(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;})(0,r1,r2))return({struct _tuple21 _tmp20C;_tmp20C.f1=outflow,_tmp20C.f2=r2;_tmp20C;});
if(({(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;})(0,r2,r1))return({struct _tuple21 _tmp20D;_tmp20D.f1=outflow,_tmp20D.f2=r1;_tmp20D;});{
struct Cyc_CfFlowInfo_JoinEnv _tmp15E=({struct Cyc_CfFlowInfo_JoinEnv _tmp20F;_tmp20F.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp269=({struct Cyc_CfFlowInfo_EscPile*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->places=0;_tmp160;});_tmp20F.pile=_tmp269;}),_tmp20F.d1=d1,_tmp20F.d2=d2,_tmp20F.is_try_flow=0;_tmp20F;});struct Cyc_CfFlowInfo_JoinEnv env=_tmp15E;
void*_tmp15F=({(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval;})(& env,0,r1,r2);void*outr=_tmp15F;
return({struct _tuple21 _tmp20E;({union Cyc_CfFlowInfo_FlowInfo _tmp26B=({struct Cyc_Dict_Dict _tmp26A=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outd);Cyc_CfFlowInfo_ReachableFL(_tmp26A,relns);});_tmp20E.f1=_tmp26B;}),_tmp20E.f2=outr;_tmp20E;});}}}else{
# 997
({void*_tmp161=0U;({int(*_tmp26D)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp26C=({const char*_tmp162="join_flow_and_rval: BottomFL outflow";_tag_fat(_tmp162,sizeof(char),37U);});_tmp26D(_tmp26C,_tag_fat(_tmp161,sizeof(void*),0));});});}}};}}}
# 1002
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1005
{struct _tuple12 _tmp163=({struct _tuple12 _tmp211;_tmp211.f1=r1,_tmp211.f2=r2;_tmp211;});struct _tuple12 _stmttmp21=_tmp163;struct _tuple12 _tmp164=_stmttmp21;struct _fat_ptr _tmp16A;int _tmp169;int _tmp168;struct _fat_ptr _tmp167;int _tmp166;int _tmp165;void*_tmp16E;void*_tmp16D;void*_tmp16C;void*_tmp16B;if(*((int*)_tmp164.f1)== 8){if(*((int*)_tmp164.f2)== 8){_tmp16B=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp164.f1)->f1;_tmp16C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp164.f1)->f2;_tmp16D=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp164.f2)->f1;_tmp16E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp164.f2)->f2;{struct Cyc_Absyn_Vardecl*n1=_tmp16B;void*r1=_tmp16C;struct Cyc_Absyn_Vardecl*n2=_tmp16D;void*r2=_tmp16E;
# 1007
return n1 == n2 && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{
goto _LL6;}}else{if(*((int*)_tmp164.f2)== 8){_LL6:
 return 0;}else{if(*((int*)_tmp164.f1)== 4){if(*((int*)_tmp164.f2)== 4){_tmp16B=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp164.f1)->f1;_tmp16C=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp164.f2)->f1;{struct Cyc_CfFlowInfo_Place*p1=_tmp16B;struct Cyc_CfFlowInfo_Place*p2=_tmp16C;
return Cyc_CfFlowInfo_place_cmp(p1,p2)== 0;}}else{
goto _LLC;}}else{if(*((int*)_tmp164.f2)== 4){_LLC:
 return 0;}else{if(*((int*)_tmp164.f1)== 5){if(*((int*)_tmp164.f2)== 5){_tmp16B=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp164.f1)->f1;_tmp16C=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp164.f2)->f1;{void*r1=_tmp16B;void*r2=_tmp16C;
return Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{
goto _LL12;}}else{if(*((int*)_tmp164.f2)== 5){_LL12:
 return 0;}else{if(*((int*)_tmp164.f1)== 6)switch(*((int*)_tmp164.f2)){case 6: _tmp165=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164.f1)->f1).is_union;_tmp166=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164.f1)->f1).fieldnum;_tmp167=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164.f1)->f2;_tmp168=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164.f2)->f1).is_union;_tmp169=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164.f2)->f1).fieldnum;_tmp16A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164.f2)->f2;{int is_union1=_tmp165;int fld1=_tmp166;struct _fat_ptr d1=_tmp167;int is_union2=_tmp168;int fld2=_tmp169;struct _fat_ptr d2=_tmp16A;
# 1018
if(is_union1 && fld1 != fld2)return 0;
if((void**)d1.curr == (void**)d2.curr)return 1;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(!({(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;})(0,((void**)d1.curr)[i],((void**)d2.curr)[i]))return 0;}}
return 1;}case 0: goto _LL15;case 1: goto _LL17;default: goto _LL1D;}else{switch(*((int*)_tmp164.f2)){case 0: _LL15:
 goto _LL18;case 1: _LL17: _LL18:
 return 0;default: if(*((int*)_tmp164.f1)== 3){if(*((int*)_tmp164.f2)== 3)
goto _LL0;else{
return 0;}}else{_LL1D:
# 1033
 goto _LL0;}}}}}}}}}_LL0:;}{
# 1035
struct _tuple19 _tmp16F=({struct _tuple19 _tmp212;({enum Cyc_CfFlowInfo_InitLevel _tmp26F=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp212.f1=_tmp26F;}),({enum Cyc_CfFlowInfo_InitLevel _tmp26E=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp212.f2=_tmp26E;});_tmp212;});struct _tuple19 _stmttmp22=_tmp16F;struct _tuple19 _tmp170=_stmttmp22;if(_tmp170.f1 == Cyc_CfFlowInfo_AllIL)
goto _LL23;else{if(_tmp170.f2 == Cyc_CfFlowInfo_NoneIL){_LL23:
 return 1;}else{
return 0;}};}}
# 1048
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1050
struct _tuple20 _tmp171=({struct _tuple20 _tmp213;_tmp213.f1=f1,_tmp213.f2=f2;_tmp213;});struct _tuple20 _stmttmp23=_tmp171;struct _tuple20 _tmp172=_stmttmp23;void*_tmp176;struct Cyc_Dict_Dict _tmp175;void*_tmp174;struct Cyc_Dict_Dict _tmp173;if(((_tmp172.f1).BottomFL).tag == 1)
return 1;else{if(((_tmp172.f2).BottomFL).tag == 1)
return 0;else{_tmp173=(((_tmp172.f1).ReachableFL).val).f1;_tmp174=(((_tmp172.f1).ReachableFL).val).f2;_tmp175=(((_tmp172.f2).ReachableFL).val).f1;_tmp176=(((_tmp172.f2).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp173;struct Cyc_List_List*r1=_tmp174;struct Cyc_Dict_Dict d2=_tmp175;struct Cyc_List_List*r2=_tmp176;
# 1054
if(d1.t == d2.t && r1 == r2)return 1;
return({(int(*)(int(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_forall_intersect;})(({(int(*)(void*,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;}),d1,d2)&&
 Cyc_Relations_relns_approx(r1,r2);}}};}struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 1060
struct _tuple23 Cyc_CfFlowInfo_unname_rval(void*rv){
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp177=rv;void*_tmp179;void*_tmp178;if(*((int*)_tmp177)== 8){_tmp178=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp177)->f1;_tmp179=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp177)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp178;void*rv2=_tmp179;
# 1066
names=({struct Cyc_List_List*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->hd=vd,_tmp17A->tl=names;_tmp17A;});rv=rv2;goto _LL0;}}else{
# 1068
done=1;goto _LL0;}_LL0:;}
# 1071
return({struct _tuple23 _tmp214;_tmp214.f1=rv,_tmp214.f2=names;_tmp214;});}
# 1074
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp17B=il;if(_tmp17B == Cyc_CfFlowInfo_NoneIL){
({void*_tmp17C=0U;({struct Cyc___cycFILE*_tmp271=Cyc_stderr;struct _fat_ptr _tmp270=({const char*_tmp17D="uninitialized";_tag_fat(_tmp17D,sizeof(char),14U);});Cyc_fprintf(_tmp271,_tmp270,_tag_fat(_tmp17C,sizeof(void*),0));});});goto _LL0;}else{
({void*_tmp17E=0U;({struct Cyc___cycFILE*_tmp273=Cyc_stderr;struct _fat_ptr _tmp272=({const char*_tmp17F="all-initialized";_tag_fat(_tmp17F,sizeof(char),16U);});Cyc_fprintf(_tmp273,_tmp272,_tag_fat(_tmp17E,sizeof(void*),0));});});goto _LL0;}_LL0:;}
# 1081
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp180=root;void*_tmp182;void*_tmp181;switch(*((int*)_tmp180)){case 0: _tmp181=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp180)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp181;
# 1084
({struct Cyc_String_pa_PrintArg_struct _tmp185=({struct Cyc_String_pa_PrintArg_struct _tmp215;_tmp215.tag=0,({struct _fat_ptr _tmp274=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp215.f1=_tmp274;});_tmp215;});void*_tmp183[1];_tmp183[0]=& _tmp185;({struct Cyc___cycFILE*_tmp276=Cyc_stderr;struct _fat_ptr _tmp275=({const char*_tmp184="Root(%s)";_tag_fat(_tmp184,sizeof(char),9U);});Cyc_fprintf(_tmp276,_tmp275,_tag_fat(_tmp183,sizeof(void*),1));});});goto _LL0;}case 1: _tmp181=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp180)->f1;_tmp182=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp180)->f2;{struct Cyc_Absyn_Exp*e=_tmp181;void*t=_tmp182;
# 1086
({struct Cyc_String_pa_PrintArg_struct _tmp188=({struct Cyc_String_pa_PrintArg_struct _tmp217;_tmp217.tag=0,({
struct _fat_ptr _tmp277=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp217.f1=_tmp277;});_tmp217;});struct Cyc_String_pa_PrintArg_struct _tmp189=({struct Cyc_String_pa_PrintArg_struct _tmp216;_tmp216.tag=0,({struct _fat_ptr _tmp278=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp216.f1=_tmp278;});_tmp216;});void*_tmp186[2];_tmp186[0]=& _tmp188,_tmp186[1]=& _tmp189;({struct Cyc___cycFILE*_tmp27A=Cyc_stderr;struct _fat_ptr _tmp279=({const char*_tmp187="MallocPt(%s,%s)";_tag_fat(_tmp187,sizeof(char),16U);});Cyc_fprintf(_tmp27A,_tmp279,_tag_fat(_tmp186,sizeof(void*),2));});});goto _LL0;}default:
# 1089
({void*_tmp18A=0U;({struct Cyc___cycFILE*_tmp27C=Cyc_stderr;struct _fat_ptr _tmp27B=({const char*_tmp18B="InitParam(_,_)";_tag_fat(_tmp18B,sizeof(char),15U);});Cyc_fprintf(_tmp27C,_tmp27B,_tag_fat(_tmp18A,sizeof(void*),0));});});goto _LL0;}_LL0:;}
# 1093
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){
{struct Cyc_List_List*x=p;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
void*_tmp18C=(void*)x->hd;void*_stmttmp24=_tmp18C;void*_tmp18D=_stmttmp24;int _tmp18E;if(*((int*)_tmp18D)== 0){_tmp18E=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp18D)->f1;{int i=_tmp18E;
# 1097
({struct Cyc_Int_pa_PrintArg_struct _tmp191=({struct Cyc_Int_pa_PrintArg_struct _tmp218;_tmp218.tag=1,_tmp218.f1=(unsigned long)i;_tmp218;});void*_tmp18F[1];_tmp18F[0]=& _tmp191;({struct Cyc___cycFILE*_tmp27E=Cyc_stderr;struct _fat_ptr _tmp27D=({const char*_tmp190=".%d";_tag_fat(_tmp190,sizeof(char),4U);});Cyc_fprintf(_tmp27E,_tmp27D,_tag_fat(_tmp18F,sizeof(void*),1));});});
goto _LL0;}}else{
# 1100
if(x->tl != 0){
void*_tmp192=(void*)((struct Cyc_List_List*)_check_null(x->tl))->hd;void*_stmttmp25=_tmp192;void*_tmp193=_stmttmp25;int _tmp194;if(*((int*)_tmp193)== 0){_tmp194=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp193)->f1;{int i=_tmp194;
# 1103
x=x->tl;
({struct Cyc_Int_pa_PrintArg_struct _tmp197=({struct Cyc_Int_pa_PrintArg_struct _tmp219;_tmp219.tag=1,_tmp219.f1=(unsigned long)i;_tmp219;});void*_tmp195[1];_tmp195[0]=& _tmp197;({struct Cyc___cycFILE*_tmp280=Cyc_stderr;struct _fat_ptr _tmp27F=({const char*_tmp196="->%d";_tag_fat(_tmp196,sizeof(char),5U);});Cyc_fprintf(_tmp280,_tmp27F,_tag_fat(_tmp195,sizeof(void*),1));});});
continue;}}else{
goto _LL5;}_LL5:;}
# 1109
({void*_tmp198=0U;({struct Cyc___cycFILE*_tmp282=Cyc_stderr;struct _fat_ptr _tmp281=({const char*_tmp199="*";_tag_fat(_tmp199,sizeof(char),2U);});Cyc_fprintf(_tmp282,_tmp281,_tag_fat(_tmp198,sizeof(void*),0));});});}_LL0:;}}
# 1112
({void*_tmp19A=0U;({struct Cyc___cycFILE*_tmp284=Cyc_stderr;struct _fat_ptr _tmp283=({const char*_tmp19B=" ";_tag_fat(_tmp19B,sizeof(char),2U);});Cyc_fprintf(_tmp284,_tmp283,_tag_fat(_tmp19A,sizeof(void*),0));});});}
# 1115
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);
Cyc_CfFlowInfo_print_path(p->path);}
# 1120
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp19C=1;int first=_tmp19C;
while(x != 0){
if(!first){({void*_tmp19D=0U;({struct Cyc___cycFILE*_tmp286=Cyc_stderr;struct _fat_ptr _tmp285=({const char*_tmp19E=", ";_tag_fat(_tmp19E,sizeof(char),3U);});Cyc_fprintf(_tmp286,_tmp285,_tag_fat(_tmp19D,sizeof(void*),0));});});first=0;}
pr(x->hd);
x=x->tl;}
# 1127
({void*_tmp19F=0U;({struct Cyc___cycFILE*_tmp288=Cyc_stderr;struct _fat_ptr _tmp287=({const char*_tmp1A0="\n";_tag_fat(_tmp1A0,sizeof(char),2U);});Cyc_fprintf(_tmp288,_tmp287,_tag_fat(_tmp19F,sizeof(void*),0));});});}
# 1130
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp1A1=rval;void*_tmp1A3;struct _fat_ptr _tmp1A6;int _tmp1A5;int _tmp1A4;void*_tmp1A2;enum Cyc_CfFlowInfo_InitLevel _tmp1A7;switch(*((int*)_tmp1A1)){case 0:
({void*_tmp1A8=0U;({struct Cyc___cycFILE*_tmp28A=Cyc_stderr;struct _fat_ptr _tmp289=({const char*_tmp1A9="Zero";_tag_fat(_tmp1A9,sizeof(char),5U);});Cyc_fprintf(_tmp28A,_tmp289,_tag_fat(_tmp1A8,sizeof(void*),0));});});goto _LL0;case 1:
({void*_tmp1AA=0U;({struct Cyc___cycFILE*_tmp28C=Cyc_stderr;struct _fat_ptr _tmp28B=({const char*_tmp1AB="NotZeroAll";_tag_fat(_tmp1AB,sizeof(char),11U);});Cyc_fprintf(_tmp28C,_tmp28B,_tag_fat(_tmp1AA,sizeof(void*),0));});});goto _LL0;case 2: _tmp1A7=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp1A7;
({void*_tmp1AC=0U;({struct Cyc___cycFILE*_tmp28E=Cyc_stderr;struct _fat_ptr _tmp28D=({const char*_tmp1AD="Unknown(";_tag_fat(_tmp1AD,sizeof(char),9U);});Cyc_fprintf(_tmp28E,_tmp28D,_tag_fat(_tmp1AC,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(il);
({void*_tmp1AE=0U;({struct Cyc___cycFILE*_tmp290=Cyc_stderr;struct _fat_ptr _tmp28F=({const char*_tmp1AF=")";_tag_fat(_tmp1AF,sizeof(char),2U);});Cyc_fprintf(_tmp290,_tmp28F,_tag_fat(_tmp1AE,sizeof(void*),0));});});goto _LL0;}case 3: _tmp1A7=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp1A7;
({void*_tmp1B0=0U;({struct Cyc___cycFILE*_tmp292=Cyc_stderr;struct _fat_ptr _tmp291=({const char*_tmp1B1="Esc(";_tag_fat(_tmp1B1,sizeof(char),5U);});Cyc_fprintf(_tmp292,_tmp291,_tag_fat(_tmp1B0,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(il);
({void*_tmp1B2=0U;({struct Cyc___cycFILE*_tmp294=Cyc_stderr;struct _fat_ptr _tmp293=({const char*_tmp1B3=")";_tag_fat(_tmp1B3,sizeof(char),2U);});Cyc_fprintf(_tmp294,_tmp293,_tag_fat(_tmp1B2,sizeof(void*),0));});});goto _LL0;}case 4: _tmp1A2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp1A2;
({void*_tmp1B4=0U;({struct Cyc___cycFILE*_tmp296=Cyc_stderr;struct _fat_ptr _tmp295=({const char*_tmp1B5="AddrOf(";_tag_fat(_tmp1B5,sizeof(char),8U);});Cyc_fprintf(_tmp296,_tmp295,_tag_fat(_tmp1B4,sizeof(void*),0));});});Cyc_CfFlowInfo_print_place(p);
({void*_tmp1B6=0U;({struct Cyc___cycFILE*_tmp298=Cyc_stderr;struct _fat_ptr _tmp297=({const char*_tmp1B7=")";_tag_fat(_tmp1B7,sizeof(char),2U);});Cyc_fprintf(_tmp298,_tmp297,_tag_fat(_tmp1B6,sizeof(void*),0));});});goto _LL0;}case 5: _tmp1A2=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1;{void*r=_tmp1A2;
({void*_tmp1B8=0U;({struct Cyc___cycFILE*_tmp29A=Cyc_stderr;struct _fat_ptr _tmp299=({const char*_tmp1B9="UniquePtr(";_tag_fat(_tmp1B9,sizeof(char),11U);});Cyc_fprintf(_tmp29A,_tmp299,_tag_fat(_tmp1B8,sizeof(void*),0));});});Cyc_CfFlowInfo_print_absrval(r);
({void*_tmp1BA=0U;({struct Cyc___cycFILE*_tmp29C=Cyc_stderr;struct _fat_ptr _tmp29B=({const char*_tmp1BB=")";_tag_fat(_tmp1BB,sizeof(char),2U);});Cyc_fprintf(_tmp29C,_tmp29B,_tag_fat(_tmp1BA,sizeof(void*),0));});});goto _LL0;}case 6: _tmp1A4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1).is_union;_tmp1A5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1).fieldnum;_tmp1A6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f2;{int is_union=_tmp1A4;int f=_tmp1A5;struct _fat_ptr d=_tmp1A6;
# 1143
if(is_union){
({void*_tmp1BC=0U;({struct Cyc___cycFILE*_tmp29E=Cyc_stderr;struct _fat_ptr _tmp29D=({const char*_tmp1BD="AggrUnion{";_tag_fat(_tmp1BD,sizeof(char),11U);});Cyc_fprintf(_tmp29E,_tmp29D,_tag_fat(_tmp1BC,sizeof(void*),0));});});
if(f != -1)
({struct Cyc_Int_pa_PrintArg_struct _tmp1C0=({struct Cyc_Int_pa_PrintArg_struct _tmp21A;_tmp21A.tag=1,_tmp21A.f1=(unsigned long)f;_tmp21A;});void*_tmp1BE[1];_tmp1BE[0]=& _tmp1C0;({struct Cyc___cycFILE*_tmp2A0=Cyc_stderr;struct _fat_ptr _tmp29F=({const char*_tmp1BF="tag == %d;";_tag_fat(_tmp1BF,sizeof(char),11U);});Cyc_fprintf(_tmp2A0,_tmp29F,_tag_fat(_tmp1BE,sizeof(void*),1));});});}else{
# 1148
({void*_tmp1C1=0U;({struct Cyc___cycFILE*_tmp2A2=Cyc_stderr;struct _fat_ptr _tmp2A1=({const char*_tmp1C2="AggrStruct{";_tag_fat(_tmp1C2,sizeof(char),12U);});Cyc_fprintf(_tmp2A2,_tmp2A1,_tag_fat(_tmp1C1,sizeof(void*),0));});});}
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)d.curr)[i]);
if((unsigned)(i + 1)< _get_fat_size(d,sizeof(void*)))({void*_tmp1C3=0U;({struct Cyc___cycFILE*_tmp2A4=Cyc_stderr;struct _fat_ptr _tmp2A3=({const char*_tmp1C4=",";_tag_fat(_tmp1C4,sizeof(char),2U);});Cyc_fprintf(_tmp2A4,_tmp2A3,_tag_fat(_tmp1C3,sizeof(void*),0));});});}}
# 1153
({void*_tmp1C5=0U;({struct Cyc___cycFILE*_tmp2A6=Cyc_stderr;struct _fat_ptr _tmp2A5=({const char*_tmp1C6="}";_tag_fat(_tmp1C6,sizeof(char),2U);});Cyc_fprintf(_tmp2A6,_tmp2A5,_tag_fat(_tmp1C5,sizeof(void*),0));});});
goto _LL0;}case 7: _tmp1A2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1;_tmp1A4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f2;_tmp1A3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f3;_tmp1A5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f4;{struct Cyc_Absyn_Exp*e=_tmp1A2;int i=_tmp1A4;void*r=_tmp1A3;int l=_tmp1A5;
# 1156
({struct Cyc_String_pa_PrintArg_struct _tmp1C9=({struct Cyc_String_pa_PrintArg_struct _tmp21C;_tmp21C.tag=0,({struct _fat_ptr _tmp2A7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp21C.f1=_tmp2A7;});_tmp21C;});struct Cyc_Int_pa_PrintArg_struct _tmp1CA=({struct Cyc_Int_pa_PrintArg_struct _tmp21B;_tmp21B.tag=1,_tmp21B.f1=(unsigned long)i;_tmp21B;});void*_tmp1C7[2];_tmp1C7[0]=& _tmp1C9,_tmp1C7[1]=& _tmp1CA;({struct Cyc___cycFILE*_tmp2A9=Cyc_stderr;struct _fat_ptr _tmp2A8=({const char*_tmp1C8="[Consumed(%s,%d,";_tag_fat(_tmp1C8,sizeof(char),17U);});Cyc_fprintf(_tmp2A9,_tmp2A8,_tag_fat(_tmp1C7,sizeof(void*),2));});});
Cyc_CfFlowInfo_print_absrval(r);({struct Cyc_String_pa_PrintArg_struct _tmp1CD=({struct Cyc_String_pa_PrintArg_struct _tmp21D;_tmp21D.tag=0,({struct _fat_ptr _tmp2AA=(struct _fat_ptr)(l?({const char*_tmp1CE="local";_tag_fat(_tmp1CE,sizeof(char),6U);}):({const char*_tmp1CF="";_tag_fat(_tmp1CF,sizeof(char),1U);}));_tmp21D.f1=_tmp2AA;});_tmp21D;});void*_tmp1CB[1];_tmp1CB[0]=& _tmp1CD;({struct Cyc___cycFILE*_tmp2AC=Cyc_stderr;struct _fat_ptr _tmp2AB=({const char*_tmp1CC=",%s)]";_tag_fat(_tmp1CC,sizeof(char),6U);});Cyc_fprintf(_tmp2AC,_tmp2AB,_tag_fat(_tmp1CB,sizeof(void*),1));});});
goto _LL0;}default: _tmp1A2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f1;_tmp1A3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A1)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp1A2;void*r=_tmp1A3;
# 1160
({struct Cyc_String_pa_PrintArg_struct _tmp1D2=({struct Cyc_String_pa_PrintArg_struct _tmp21E;_tmp21E.tag=0,({struct _fat_ptr _tmp2AD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n->name));_tmp21E.f1=_tmp2AD;});_tmp21E;});void*_tmp1D0[1];_tmp1D0[0]=& _tmp1D2;({struct Cyc___cycFILE*_tmp2AF=Cyc_stderr;struct _fat_ptr _tmp2AE=({const char*_tmp1D1="[NamedLocation(%s,";_tag_fat(_tmp1D1,sizeof(char),19U);});Cyc_fprintf(_tmp2AF,_tmp2AE,_tag_fat(_tmp1D0,sizeof(void*),1));});});
Cyc_CfFlowInfo_print_absrval(r);({void*_tmp1D3=0U;({struct Cyc___cycFILE*_tmp2B1=Cyc_stderr;struct _fat_ptr _tmp2B0=({const char*_tmp1D4=")]";_tag_fat(_tmp1D4,sizeof(char),3U);});Cyc_fprintf(_tmp2B1,_tmp2B0,_tag_fat(_tmp1D3,sizeof(void*),0));});});
goto _LL0;}}_LL0:;}
# 1166
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp1D5=0U;({struct Cyc___cycFILE*_tmp2B3=Cyc_stderr;struct _fat_ptr _tmp2B2=({const char*_tmp1D6="    ";_tag_fat(_tmp1D6,sizeof(char),5U);});Cyc_fprintf(_tmp2B3,_tmp2B2,_tag_fat(_tmp1D5,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp1D7=0U;({struct Cyc___cycFILE*_tmp2B5=Cyc_stderr;struct _fat_ptr _tmp2B4=({const char*_tmp1D8=" --> ";_tag_fat(_tmp1D8,sizeof(char),6U);});Cyc_fprintf(_tmp2B5,_tmp2B4,_tag_fat(_tmp1D7,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp1D9=0U;({struct Cyc___cycFILE*_tmp2B7=Cyc_stderr;struct _fat_ptr _tmp2B6=({const char*_tmp1DA="\n";_tag_fat(_tmp1DA,sizeof(char),2U);});Cyc_fprintf(_tmp2B7,_tmp2B6,_tag_fat(_tmp1D9,sizeof(void*),0));});});}
# 1174
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
({(void(*)(void(*)(void*,void*),struct Cyc_Dict_Dict))Cyc_Dict_iter;})(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1178
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp1DB=f;void*_tmp1DD;struct Cyc_Dict_Dict _tmp1DC;if((_tmp1DB.BottomFL).tag == 1){
({void*_tmp1DE=0U;({struct Cyc___cycFILE*_tmp2B9=Cyc_stderr;struct _fat_ptr _tmp2B8=({const char*_tmp1DF="  BottomFL\n";_tag_fat(_tmp1DF,sizeof(char),12U);});Cyc_fprintf(_tmp2B9,_tmp2B8,_tag_fat(_tmp1DE,sizeof(void*),0));});});goto _LL0;}else{_tmp1DC=((_tmp1DB.ReachableFL).val).f1;_tmp1DD=((_tmp1DB.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_tmp1DC;struct Cyc_List_List*rlns=_tmp1DD;
# 1182
({void*_tmp1E0=0U;({struct Cyc___cycFILE*_tmp2BB=Cyc_stderr;struct _fat_ptr _tmp2BA=({const char*_tmp1E1="  ReachableFL:\n";_tag_fat(_tmp1E1,sizeof(char),16U);});Cyc_fprintf(_tmp2BB,_tmp2BA,_tag_fat(_tmp1E0,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_flowdict(fd);
({void*_tmp1E2=0U;({struct Cyc___cycFILE*_tmp2BD=Cyc_stderr;struct _fat_ptr _tmp2BC=({const char*_tmp1E3="\n  Relations: ";_tag_fat(_tmp1E3,sizeof(char),15U);});Cyc_fprintf(_tmp2BD,_tmp2BC,_tag_fat(_tmp1E2,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,rlns);
({void*_tmp1E4=0U;({struct Cyc___cycFILE*_tmp2BF=Cyc_stderr;struct _fat_ptr _tmp2BE=({const char*_tmp1E5="\n";_tag_fat(_tmp1E5,sizeof(char),2U);});Cyc_fprintf(_tmp2BF,_tmp2BE,_tag_fat(_tmp1E4,sizeof(void*),0));});});
goto _LL0;}}_LL0:;}

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
 struct Cyc_Core_Opt{void*v;};
# 116 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
extern struct Cyc_List_List*Cyc_List_map2(void*(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 205
extern struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
extern struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 90 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 153
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 174
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 179
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 184
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 287
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 307
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 410 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 480
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 859 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 862
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 869
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 875
void*Cyc_Absyn_compress(void*);
# 880
int Cyc_Absyn_type2bool(int,void*);
# 889
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 894
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 896
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 901
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 903
extern void*Cyc_Absyn_empty_effect;
# 905
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 907
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*,struct Cyc_Absyn_Enumdecl*);
# 932
extern void*Cyc_Absyn_fat_bound_type;
# 934
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 936
void*Cyc_Absyn_bounds_one (void);
# 938
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 942
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 961
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
# 963
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 966
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 976
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 983
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 985
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 989
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1068
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
# 1112
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1114
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1116
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1126
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 76 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
extern struct Cyc_Absyn_Kind Cyc_Kinds_boolk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tbk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 42
extern struct Cyc_Absyn_Kind Cyc_Kinds_urk;
# 44
extern struct Cyc_Absyn_Kind Cyc_Kinds_ubk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_umk;
# 52
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
extern struct Cyc_Core_Opt Cyc_Kinds_boolko;
extern struct Cyc_Core_Opt Cyc_Kinds_ptrbko;
# 66
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
# 68
void*Cyc_Kinds_copy_kindbound(void*);
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 27 "warn.h"
void Cyc_Warn_vwarn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 33
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 38
void*Cyc_Warn_vimpos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 47 "flags.h"
extern int Cyc_Flags_warn_alias_coerce;
# 51
extern int Cyc_Flags_warn_region_coerce;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
extern int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 50
extern int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 32
void Cyc_Unify_occurs(void*,struct _RegionHandle*,struct Cyc_List_List*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
extern union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RReturn (void);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 110
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 131
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 44 "rgnorder.h"
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*,void*,void*);
# 37 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 80
int Cyc_Tcutil_is_zero_ptr_type(void*,void**,int*,void**);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 88
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 103
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
# 106
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 110
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 112
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 114
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);
# 117
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 137
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 140
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 144
int Cyc_Tcutil_region_in_effect(int,void*,void*);
# 164
void Cyc_Tcutil_check_bound(unsigned,unsigned,void*,int);
# 177
int Cyc_Tcutil_is_noalias_region(void*,int);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*,int);
# 185
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple14{int f1;void*f2;};
# 194
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 197
void*Cyc_Tcutil_normalize_effect(void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 204
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 214
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 228
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 231
int Cyc_Tcutil_zeroable_type(void*);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 243
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 246
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 251
int Cyc_Tcutil_tycon_cmp(void*,void*);
int Cyc_Tcutil_star_cmp(int(*)(void*,void*),void*,void*);
# 65 "attributes.h"
int Cyc_Atts_sub_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 68
int Cyc_Atts_fntype_att(void*);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);
# 39 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Absyn_compress(t);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;if(*((int*)_tmp1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)== 0)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 45
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Absyn_compress(t);void*_stmttmp1=_tmp2;void*_tmp3=_stmttmp1;if(*((int*)_tmp3)== 4)
return 1;else{
return 0;};}
# 51
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp4;void*_tmp5=_stmttmp2;if(*((int*)_tmp5)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)== 5)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 57
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Absyn_compress(t);void*_stmttmp3=_tmp6;void*_tmp7=_stmttmp3;if(*((int*)_tmp7)== 3)
return 1;else{
return 0;};}
# 64
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Absyn_compress(t);void*_stmttmp4=_tmp8;void*_tmp9=_stmttmp4;if(*((int*)_tmp9)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz)
return 1;else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 71
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Absyn_compress(t);void*_stmttmp5=_tmpA;void*_tmpB=_stmttmp5;if(*((int*)_tmpB)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)== 1)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 78
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Absyn_compress(t);void*_stmttmp6=_tmpC;void*_tmpD=_stmttmp6;if(*((int*)_tmpD)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)== 2)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 85
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Absyn_compress(t);void*_stmttmp7=_tmpE;void*_tmpF=_stmttmp7;void*_tmp10;if(*((int*)_tmpF)== 0){_tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;{void*c=_tmp10;
# 88
void*_tmp11=c;switch(*((int*)_tmp11)){case 1:
 goto _LL9;case 4: _LL9:
 goto _LLB;case 15: _LLB:
 goto _LLD;case 16: _LLD:
 return 1;default:
 return 0;};}}else{
# 95
return 0;};}
# 98
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Absyn_compress(t);void*_stmttmp8=_tmp12;void*_tmp13=_stmttmp8;if(*((int*)_tmp13)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed)
return 1;else{goto _LL5;}case 2:
 return 1;default: goto _LL5;}else{_LL5:
 return 0;};}
# 105
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 108
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 111
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp14=Cyc_Absyn_compress(t);void*_stmttmp9=_tmp14;void*_tmp15=_stmttmp9;if(*((int*)_tmp15)== 5)
return 1;else{
return 0;};}
# 117
int Cyc_Tcutil_is_typedef_type(void*t){
void*_tmp16=Cyc_Absyn_compress(t);void*_stmttmpA=_tmp16;void*_tmp17=_stmttmpA;if(*((int*)_tmp17)== 8)
return 1;else{
return 0;};}
# 123
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 131
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp18=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpB=_tmp18;void*_tmp19=_stmttmpB;void*_tmp1A;if(*((int*)_tmp19)== 1)
return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);else{_tmp1A=_tmp19;{void*t=_tmp1A;
return Cyc_Tcutil_is_integral_type(t);}};}
# 139
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp1B=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpC=_tmp1B;void*_tmp1C=_stmttmpC;if(*((int*)_tmp1C)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->f1)== 2)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}}
# 149
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1D=Cyc_Absyn_compress(t);void*_stmttmpD=_tmp1D;void*_tmp1E=_stmttmpD;void*_tmp1F;if(*((int*)_tmp1E)== 3){_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->f1).ptr_atts).zero_term;{void*ztl=_tmp1F;
# 152
return Cyc_Tcutil_force_type2bool(0,ztl);}}else{
return 0;};}
# 158
int Cyc_Tcutil_is_nullable_pointer_type(void*t,int def){
void*_tmp20=Cyc_Absyn_compress(t);void*_stmttmpE=_tmp20;void*_tmp21=_stmttmpE;void*_tmp22;if(*((int*)_tmp21)== 3){_tmp22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->f1).ptr_atts).nullable;{void*nbl=_tmp22;
# 161
return Cyc_Tcutil_force_type2bool(def,nbl);}}else{
return 0;};}
# 167
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp23=Cyc_Absyn_compress(t);void*_stmttmpF=_tmp23;void*_tmp24=_stmttmpF;void*_tmp25;if(*((int*)_tmp24)== 3){_tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;{void*b=_tmp25;
# 170
return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,b);}}else{
return 0;};}
# 177
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp26=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp26;void*_tmp27=_stmttmp10;void*_tmp29;void*_tmp28;if(*((int*)_tmp27)== 3){_tmp28=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).elt_type;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).ptr_atts).bounds;{void*elt_type=_tmp28;void*b=_tmp29;
# 180
if(!Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type))
return 0;
*elt_type_dest=elt_type;
return 1;}}else{
return 0;};}
# 190
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp2A=Cyc_Absyn_compress(t);void*_stmttmp11=_tmp2A;void*_tmp2B=_stmttmp11;void*_tmp2D;void*_tmp2C;if(*((int*)_tmp2B)== 3){_tmp2C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_type;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;{void*elt_type=_tmp2C;void*zt=_tmp2D;
# 193
*elt_type_dest=elt_type;
return Cyc_Absyn_type2bool(0,zt);}}else{
return 0;};}
# 202
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 204
void*_tmp2E=Cyc_Absyn_compress(t);void*_stmttmp12=_tmp2E;void*_tmp2F=_stmttmp12;void*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Tqual _tmp31;void*_tmp30;void*_tmp36;void*_tmp35;void*_tmp34;switch(*((int*)_tmp2F)){case 3: _tmp34=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp35=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).bounds;_tmp36=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).zero_term;{void*elt=_tmp34;void*bnds=_tmp35;void*zt=_tmp36;
# 206
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*ptr_type=t;
*elt_type=elt;
{void*_tmp37=Cyc_Absyn_compress(bnds);void*_stmttmp13=_tmp37;void*_tmp38=_stmttmp13;if(*((int*)_tmp38)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->f1)== 14){
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA:
*is_fat=0;goto _LL7;}_LL7:;}
# 214
return 1;}case 4: _tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).tq;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).zero_term;{void*elt=_tmp30;struct Cyc_Absyn_Tqual tq=_tmp31;struct Cyc_Absyn_Exp*n=_tmp32;void*zt=_tmp33;
# 216
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*elt_type=elt;
*is_fat=0;
({void*_tmp670=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp670;});
return 1;}default:
 return 0;};}
# 229
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 235
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp39=({void*_tmp671=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp671,b);});struct Cyc_Absyn_Exp*eopt=_tmp39;
if(eopt == 0)return 0;{
struct _tuple13 _tmp3A=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp14=_tmp3A;struct _tuple13 _tmp3B=_stmttmp14;int _tmp3D;unsigned _tmp3C;_tmp3C=_tmp3B.f1;_tmp3D=_tmp3B.f2;{unsigned i=_tmp3C;int known=_tmp3D;
return known && i == (unsigned)1;}}}
# 243
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3E=Cyc_Absyn_compress(t);void*_stmttmp15=_tmp3E;void*_tmp3F=_stmttmp15;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;void*_tmp43;void*_tmp42;struct Cyc_List_List*_tmp45;void*_tmp44;switch(*((int*)_tmp3F)){case 0: _tmp44=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f2;{void*c=_tmp44;struct Cyc_List_List*ts=_tmp45;
# 246
void*_tmp46=c;struct Cyc_Absyn_Aggrdecl*_tmp47;switch(*((int*)_tmp46)){case 0:
 goto _LLF;case 1: _LLF:
 goto _LL11;case 2: _LL11:
 goto _LL13;case 16: _LL13:
 goto _LL15;case 15: _LL15:
 return 1;case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp46)->f1).UnknownAggr).tag == 1)
# 253
return 0;else{_tmp47=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp46)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp47;
# 255
if(ad->impl == 0)
return 0;{
struct _RegionHandle _tmp48=_new_region("rgn");struct _RegionHandle*rgn=& _tmp48;_push_region(rgn);
{struct Cyc_List_List*_tmp49=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(rgn,rgn,ad->tvs,ts);struct Cyc_List_List*inst=_tmp49;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp4A=0;_npop_handler(0);return _tmp4A;}}}{
int _tmp4B=1;_npop_handler(0);return _tmp4B;}}
# 258
;_pop_region();}}}default:
# 263
 return 0;};}case 4: _tmp42=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp43=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;{void*t=_tmp42;void*zero_term=_tmp43;
# 268
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6: _tmp41=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F)->f1;{struct Cyc_List_List*tqs=_tmp41;
# 270
for(1;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7: _tmp40=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F)->f2;{struct Cyc_List_List*fs=_tmp40;
# 275
for(1;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}default:
 return 0;};}
# 283
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 285
Cyc_Warn_verr(loc,fmt,ap);}
# 287
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 289
Cyc_Warn_vimpos(fmt,ap);}
# 291
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 293
Cyc_Warn_vwarn(sg,fmt,ap);}
# 297
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 301
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->name=tv->name,_tmp4C->identity=- 1,({void*_tmp672=Cyc_Kinds_copy_kindbound(tv->kind);_tmp4C->kind=_tmp672;});_tmp4C;});}
# 304
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9*_tmp4D=arg;void*_tmp50;struct Cyc_Absyn_Tqual _tmp4F;struct _fat_ptr*_tmp4E;_tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;_tmp50=_tmp4D->f3;{struct _fat_ptr*x=_tmp4E;struct Cyc_Absyn_Tqual y=_tmp4F;void*t=_tmp50;
return({struct _tuple9*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->f1=x,_tmp51->f2=y,({void*_tmp673=Cyc_Tcutil_copy_type(t);_tmp51->f3=_tmp673;});_tmp51;});}}
# 308
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->f1=(*arg).f1,({void*_tmp674=Cyc_Tcutil_copy_type((*arg).f2);_tmp52->f2=_tmp674;});_tmp52;});}
# 311
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
return e == 0?0: Cyc_Tcutil_deep_copy_exp(preserve_types,e);}
# 314
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->name=f->name,_tmp53->tq=f->tq,({void*_tmp676=Cyc_Tcutil_copy_type(f->type);_tmp53->type=_tmp676;}),_tmp53->width=f->width,_tmp53->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp675=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp53->requires_clause=_tmp675;});_tmp53;});}struct _tuple15{void*f1;void*f2;};
# 318
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp54=x;void*_tmp56;void*_tmp55;_tmp55=_tmp54->f1;_tmp56=_tmp54->f2;{void*r1=_tmp55;void*r2=_tmp56;
return({struct _tuple15*_tmp57=_cycalloc(sizeof(*_tmp57));({void*_tmp678=Cyc_Tcutil_copy_type(r1);_tmp57->f1=_tmp678;}),({void*_tmp677=Cyc_Tcutil_copy_type(r2);_tmp57->f2=_tmp677;});_tmp57;});}}
# 322
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 326
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp58=Cyc_Absyn_compress(t);void*_stmttmp16=_tmp58;void*_tmp59=_stmttmp16;struct Cyc_Absyn_Datatypedecl*_tmp5A;struct Cyc_Absyn_Enumdecl*_tmp5B;struct Cyc_Absyn_Aggrdecl*_tmp5C;struct Cyc_Absyn_Typedefdecl*_tmp5F;struct Cyc_List_List*_tmp5E;struct _tuple1*_tmp5D;struct Cyc_Absyn_Exp*_tmp60;struct Cyc_Absyn_Exp*_tmp61;struct Cyc_List_List*_tmp63;enum Cyc_Absyn_AggrKind _tmp62;struct Cyc_List_List*_tmp64;struct Cyc_Absyn_Vardecl*_tmp72;struct Cyc_List_List*_tmp71;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_List_List*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_List_List*_tmp6D;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_VarargInfo*_tmp6B;int _tmp6A;struct Cyc_List_List*_tmp69;void*_tmp68;struct Cyc_Absyn_Tqual _tmp67;void*_tmp66;struct Cyc_List_List*_tmp65;unsigned _tmp77;void*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_Absyn_Tqual _tmp74;void*_tmp73;void*_tmp7F;struct Cyc_Absyn_PtrLoc*_tmp7E;void*_tmp7D;void*_tmp7C;void*_tmp7B;void*_tmp7A;struct Cyc_Absyn_Tqual _tmp79;void*_tmp78;struct Cyc_Absyn_Tvar*_tmp80;struct Cyc_List_List*_tmp82;void*_tmp81;void*_tmp83;switch(*((int*)_tmp59)){case 0: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f2 == 0){_tmp83=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f1;{void*c=_tmp83;
return t;}}else{_tmp81=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f1;_tmp82=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f2;{void*c=_tmp81;struct Cyc_List_List*ts=_tmp82;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=0,_tmp84->f1=c,({struct Cyc_List_List*_tmp679=((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp84->f2=_tmp679;});_tmp84;});}}case 1:
 return t;case 2: _tmp80=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp59)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp80;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3: _tmp78=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).elt_type;_tmp79=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).elt_tq;_tmp7A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).rgn;_tmp7B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).nullable;_tmp7C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).bounds;_tmp7D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).zero_term;_tmp7E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).ptrloc;_tmp7F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).released;{void*elt=_tmp78;struct Cyc_Absyn_Tqual tq=_tmp79;void*rgn=_tmp7A;void*nbl=_tmp7B;void*bs=_tmp7C;void*zt=_tmp7D;struct Cyc_Absyn_PtrLoc*loc=_tmp7E;void*rel=_tmp7F;
# 333
void*_tmp85=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp85;
void*_tmp86=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp86;
void*_tmp87=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp87;
struct Cyc_Absyn_Tqual _tmp88=tq;struct Cyc_Absyn_Tqual tq2=_tmp88;
# 338
void*_tmp89=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp89;
void*_tmp8A=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp8A;
void*_tmp8B=Cyc_Tcutil_copy_type(rel);void*rel2=_tmp8B;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=3,(_tmp8C->f1).elt_type=elt2,(_tmp8C->f1).elt_tq=tq2,((_tmp8C->f1).ptr_atts).rgn=rgn2,((_tmp8C->f1).ptr_atts).nullable=nbl2,((_tmp8C->f1).ptr_atts).bounds=bs2,((_tmp8C->f1).ptr_atts).zero_term=zt2,((_tmp8C->f1).ptr_atts).ptrloc=loc,((_tmp8C->f1).ptr_atts).released=rel2;_tmp8C;});}case 4: _tmp73=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).elt_type;_tmp74=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).tq;_tmp75=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).num_elts;_tmp76=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).zero_term;_tmp77=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).zt_loc;{void*et=_tmp73;struct Cyc_Absyn_Tqual tq=_tmp74;struct Cyc_Absyn_Exp*eo=_tmp75;void*zt=_tmp76;unsigned ztl=_tmp77;
# 343
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=4,({void*_tmp67C=Cyc_Tcutil_copy_type(et);(_tmp8D->f1).elt_type=_tmp67C;}),(_tmp8D->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp67B=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp8D->f1).num_elts=_tmp67B;}),({
void*_tmp67A=Cyc_Tcutil_copy_type(zt);(_tmp8D->f1).zero_term=_tmp67A;}),(_tmp8D->f1).zt_loc=ztl;_tmp8D;});}case 5: _tmp65=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).tvars;_tmp66=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).effect;_tmp67=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).ret_tqual;_tmp68=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).ret_type;_tmp69=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).args;_tmp6A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).c_varargs;_tmp6B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).cyc_varargs;_tmp6C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).rgn_po;_tmp6D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).attributes;_tmp6E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).requires_clause;_tmp6F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).requires_relns;_tmp70=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).ensures_clause;_tmp71=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).ensures_relns;_tmp72=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).return_value;{struct Cyc_List_List*tvs=_tmp65;void*effopt=_tmp66;struct Cyc_Absyn_Tqual rt_tq=_tmp67;void*rt=_tmp68;struct Cyc_List_List*args=_tmp69;int c_varargs=_tmp6A;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp6B;struct Cyc_List_List*rgn_po=_tmp6C;struct Cyc_List_List*atts=_tmp6D;struct Cyc_Absyn_Exp*req=_tmp6E;struct Cyc_List_List*req_rlns=_tmp6F;struct Cyc_Absyn_Exp*ens=_tmp70;struct Cyc_List_List*ens_rlns=_tmp71;struct Cyc_Absyn_Vardecl*ret_val=_tmp72;
# 346
struct Cyc_List_List*_tmp8E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp8E;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmp8F=Cyc_Tcutil_copy_type(rt);void*rt2=_tmp8F;
struct Cyc_List_List*_tmp90=((struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmp90;
int _tmp91=c_varargs;int c_varargs2=_tmp91;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=cyc_varargs == 0?0:({struct Cyc_Absyn_VarargInfo*_tmp9A=_cycalloc(sizeof(*_tmp9A));*_tmp9A=*cyc_varargs;_tmp9A;});
if(cyc_varargs != 0)
({void*_tmp67D=Cyc_Tcutil_copy_type(cyc_varargs->type);((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_varargs2))->type=_tmp67D;});{
struct Cyc_List_List*_tmp92=((struct Cyc_List_List*(*)(struct _tuple15*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmp92;
struct Cyc_List_List*_tmp93=atts;struct Cyc_List_List*atts2=_tmp93;
struct Cyc_Absyn_Exp*_tmp94=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmp94;
struct Cyc_List_List*_tmp95=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmp95;
struct Cyc_Absyn_Exp*_tmp96=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmp96;
struct Cyc_List_List*_tmp97=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmp97;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmp98=_cycalloc(sizeof(*_tmp98));*_tmp98=*ret_val;_tmp98;});
({void*_tmp67E=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp67E;});
if((unsigned)ret_val2->rgn)({void*_tmp67F=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp67F;});}
# 366
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=5,(_tmp99->f1).tvars=tvs2,(_tmp99->f1).effect=effopt2,(_tmp99->f1).ret_tqual=rt_tq,(_tmp99->f1).ret_type=rt2,(_tmp99->f1).args=args2,(_tmp99->f1).c_varargs=c_varargs2,(_tmp99->f1).cyc_varargs=cyc_varargs2,(_tmp99->f1).rgn_po=rgn_po2,(_tmp99->f1).attributes=atts2,(_tmp99->f1).requires_clause=req2,(_tmp99->f1).requires_relns=req_rlns2,(_tmp99->f1).ensures_clause=ens2,(_tmp99->f1).ensures_relns=ens_rlns2,(_tmp99->f1).return_value=ret_val2;_tmp99;});}}case 6: _tmp64=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp59)->f1;{struct Cyc_List_List*tqts=_tmp64;
# 369
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=6,({struct Cyc_List_List*_tmp680=((struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmp9B->f1=_tmp680;});_tmp9B;});}case 7: _tmp62=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp59)->f1;_tmp63=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp59)->f2;{enum Cyc_Absyn_AggrKind k=_tmp62;struct Cyc_List_List*fs=_tmp63;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=7,_tmp9C->f1=k,({struct Cyc_List_List*_tmp681=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmp9C->f2=_tmp681;});_tmp9C;});}case 9: _tmp61=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp59)->f1;{struct Cyc_Absyn_Exp*e=_tmp61;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=9,_tmp9D->f1=e;_tmp9D;});}case 11: _tmp60=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp59)->f1;{struct Cyc_Absyn_Exp*e=_tmp60;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=11,_tmp9E->f1=e;_tmp9E;});}case 8: _tmp5D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp59)->f1;_tmp5E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp59)->f2;_tmp5F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp59)->f3;{struct _tuple1*tdn=_tmp5D;struct Cyc_List_List*ts=_tmp5E;struct Cyc_Absyn_Typedefdecl*td=_tmp5F;
# 374
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=8,_tmp9F->f1=tdn,({struct Cyc_List_List*_tmp682=((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp9F->f2=_tmp682;}),_tmp9F->f3=td,_tmp9F->f4=0;_tmp9F;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp59)->f1)->r)){case 0: _tmp5C=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp59)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp5C;
# 377
struct Cyc_List_List*_tmpA0=((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpA0;
return({union Cyc_Absyn_AggrInfo _tmp683=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp683,targs);});}case 1: _tmp5B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp59)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp5B;
# 380
return Cyc_Absyn_enum_type(ed->name,0);}default: _tmp5A=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp59)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp5A;
# 382
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpA1;
return({union Cyc_Absyn_DatatypeInfo _tmp684=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5EF;_tmp5EF.name=dd->name,_tmp5EF.is_extensible=0;_tmp5EF;}));Cyc_Absyn_datatype_type(_tmp684,targs);});}}};}
# 388
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpA2=d;struct Cyc_Absyn_Exp*_tmpA3;if(*((int*)_tmpA2)== 0){_tmpA3=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpA2)->f1;{struct Cyc_Absyn_Exp*e=_tmpA3;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=0,({struct Cyc_Absyn_Exp*_tmp685=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpA4->f1=_tmp685;});_tmpA4;});}}else{
return d;};}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 394
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 396
return({struct _tuple16*_tmpA5=_cycalloc(sizeof(*_tmpA5));({struct Cyc_List_List*_tmp687=((struct Cyc_List_List*(*)(void*(*)(int,void*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpA5->f1=_tmp687;}),({
struct Cyc_Absyn_Exp*_tmp686=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpA5->f2=_tmp686;});_tmpA5;});}
# 400
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*(*_tmpA6)(int,struct Cyc_Absyn_Exp*)=Cyc_Tcutil_deep_copy_exp;struct Cyc_Absyn_Exp*(*DC)(int,struct Cyc_Absyn_Exp*)=_tmpA6;
struct Cyc_Absyn_Exp*new_e;
int _tmpA7=preserve_types;int pt=_tmpA7;
{void*_tmpA8=e->r;void*_stmttmp17=_tmpA8;void*_tmpA9=_stmttmp17;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpAE;struct Cyc_List_List*_tmpAD;struct _fat_ptr _tmpAC;int _tmpAB;void*_tmpB0;struct _fat_ptr*_tmpB2;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_List_List*_tmpB4;struct Cyc_Core_Opt*_tmpB3;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*_tmpB5;int _tmpBC;int _tmpBB;struct Cyc_Absyn_Exp*_tmpBA;void**_tmpB9;struct Cyc_Absyn_Exp*_tmpB8;int _tmpB7;struct Cyc_Absyn_Enumfield*_tmpBE;void*_tmpBD;struct Cyc_Absyn_Enumfield*_tmpC0;struct Cyc_Absyn_Enumdecl*_tmpBF;struct Cyc_List_List*_tmpC4;void*_tmpC3;struct Cyc_Absyn_Tqual _tmpC2;struct _fat_ptr*_tmpC1;struct Cyc_List_List*_tmpC6;void*_tmpC5;struct Cyc_Absyn_Aggrdecl*_tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpC8;struct _tuple1*_tmpC7;int _tmpCD;void*_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;int _tmpD1;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Vardecl*_tmpCE;struct Cyc_Absyn_Datatypefield*_tmpD4;struct Cyc_Absyn_Datatypedecl*_tmpD3;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD5;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;int _tmpDB;int _tmpDA;struct _fat_ptr*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;int _tmpDF;int _tmpDE;struct _fat_ptr*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_List_List*_tmpE0;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_List_List*_tmpE4;void*_tmpE3;struct Cyc_Absyn_Exp*_tmpE5;void*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;enum Cyc_Absyn_Coercion _tmpED;int _tmpEC;struct Cyc_Absyn_Exp*_tmpEB;void*_tmpEA;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpF0;int _tmpF2;struct Cyc_Absyn_Exp*_tmpF1;int _tmpF6;struct Cyc_Absyn_VarargCallInfo*_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF3;int _tmpFF;int _tmpFE;void*_tmpFD;struct Cyc_Absyn_Tqual _tmpFC;struct _fat_ptr*_tmpFB;struct Cyc_List_List*_tmpFA;int _tmpF9;struct Cyc_List_List*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Core_Opt*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;enum Cyc_Absyn_Incrementor _tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_List_List*_tmp10F;enum Cyc_Absyn_Primop _tmp10E;struct _fat_ptr _tmp110;void*_tmp111;union Cyc_Absyn_Cnst _tmp112;switch(*((int*)_tmpA9)){case 0: _tmp112=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{union Cyc_Absyn_Cnst c=_tmp112;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1: _tmp111=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{void*b=_tmp111;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2: _tmp110=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct _fat_ptr p=_tmp110;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3: _tmp10E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmp10F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{enum Cyc_Absyn_Primop p=_tmp10E;struct Cyc_List_List*es=_tmp10F;
new_e=({enum Cyc_Absyn_Primop _tmp689=p;struct Cyc_List_List*_tmp688=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_primop_exp(_tmp689,_tmp688,e->loc);});goto _LL0;}case 5: _tmp10C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmp10D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp10C;enum Cyc_Absyn_Incrementor i=_tmp10D;
new_e=({struct Cyc_Absyn_Exp*_tmp68B=DC(pt,e1);enum Cyc_Absyn_Incrementor _tmp68A=i;Cyc_Absyn_increment_exp(_tmp68B,_tmp68A,e->loc);});goto _LL0;}case 7: _tmp10A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmp10B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp10A;struct Cyc_Absyn_Exp*e2=_tmp10B;
new_e=({struct Cyc_Absyn_Exp*_tmp68D=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp68C=DC(pt,e2);Cyc_Absyn_and_exp(_tmp68D,_tmp68C,e->loc);});goto _LL0;}case 8: _tmp108=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmp109=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp108;struct Cyc_Absyn_Exp*e2=_tmp109;
new_e=({struct Cyc_Absyn_Exp*_tmp68F=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp68E=DC(pt,e2);Cyc_Absyn_or_exp(_tmp68F,_tmp68E,e->loc);});goto _LL0;}case 9: _tmp106=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmp107=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp106;struct Cyc_Absyn_Exp*e2=_tmp107;
new_e=({struct Cyc_Absyn_Exp*_tmp691=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp690=DC(pt,e2);Cyc_Absyn_seq_exp(_tmp691,_tmp690,e->loc);});goto _LL0;}case 6: _tmp103=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmp104=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmp105=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp103;struct Cyc_Absyn_Exp*e2=_tmp104;struct Cyc_Absyn_Exp*e3=_tmp105;
# 414
new_e=({struct Cyc_Absyn_Exp*_tmp694=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp693=DC(pt,e2);struct Cyc_Absyn_Exp*_tmp692=DC(pt,e3);Cyc_Absyn_conditional_exp(_tmp694,_tmp693,_tmp692,e->loc);});goto _LL0;}case 4: _tmp100=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmp101=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmp102=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp100;struct Cyc_Core_Opt*po=_tmp101;struct Cyc_Absyn_Exp*e2=_tmp102;
# 416
new_e=({struct Cyc_Absyn_Exp*_tmp697=DC(pt,e1);struct Cyc_Core_Opt*_tmp696=(unsigned)po?({struct Cyc_Core_Opt*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->v=(void*)po->v;_tmp113;}): 0;struct Cyc_Absyn_Exp*_tmp695=DC(pt,e2);Cyc_Absyn_assignop_exp(_tmp697,_tmp696,_tmp695,e->loc);});
goto _LL0;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f3 != 0){_tmpF7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpF8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpF9=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f3)->num_varargs;_tmpFA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f3)->injectors;_tmpFB=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f3)->vai)->name;_tmpFC=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f3)->vai)->tq;_tmpFD=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f3)->vai)->type;_tmpFE=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f3)->vai)->inject;_tmpFF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f4;{struct Cyc_Absyn_Exp*e1=_tmpF7;struct Cyc_List_List*es=_tmpF8;int n=_tmpF9;struct Cyc_List_List*is=_tmpFA;struct _fat_ptr*nm=_tmpFB;struct Cyc_Absyn_Tqual tq=_tmpFC;void*t=_tmpFD;int i=_tmpFE;int resolved=_tmpFF;
# 419
new_e=({void*_tmp69D=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->tag=10,({
struct Cyc_Absyn_Exp*_tmp69C=DC(pt,e1);_tmp116->f1=_tmp69C;}),({struct Cyc_List_List*_tmp69B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);_tmp116->f2=_tmp69B;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp69A=({struct Cyc_Absyn_VarargCallInfo*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->num_varargs=n,_tmp115->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp699=({struct Cyc_Absyn_VarargInfo*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->name=nm,_tmp114->tq=tq,({void*_tmp698=Cyc_Tcutil_copy_type(t);_tmp114->type=_tmp698;}),_tmp114->inject=i;_tmp114;});_tmp115->vai=_tmp699;});_tmp115;});
# 421
_tmp116->f3=_tmp69A;}),_tmp116->f4=resolved;_tmp116;});
# 419
Cyc_Absyn_new_exp(_tmp69D,e->loc);});
# 424
goto _LL0;}}else{_tmpF3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpF4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpF5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;_tmpF6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA9)->f4;{struct Cyc_Absyn_Exp*e1=_tmpF3;struct Cyc_List_List*es=_tmpF4;struct Cyc_Absyn_VarargCallInfo*vci=_tmpF5;int resolved=_tmpF6;
# 426
new_e=({void*_tmp6A0=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->tag=10,({struct Cyc_Absyn_Exp*_tmp69F=DC(pt,e1);_tmp117->f1=_tmp69F;}),({struct Cyc_List_List*_tmp69E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);_tmp117->f2=_tmp69E;}),_tmp117->f3=vci,_tmp117->f4=resolved;_tmp117;});Cyc_Absyn_new_exp(_tmp6A0,e->loc);});
goto _LL0;}}case 11: _tmpF1=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpF2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpF1;int b=_tmpF2;
# 429
new_e=b?({struct Cyc_Absyn_Exp*_tmp6A2=DC(pt,e1);Cyc_Absyn_rethrow_exp(_tmp6A2,e->loc);}):({struct Cyc_Absyn_Exp*_tmp6A1=DC(pt,e1);Cyc_Absyn_throw_exp(_tmp6A1,e->loc);});
goto _LL0;}case 12: _tmpF0=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct Cyc_Absyn_Exp*e1=_tmpF0;
# 432
new_e=({struct Cyc_Absyn_Exp*_tmp6A3=DC(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp6A3,e->loc);});goto _LL0;}case 13: _tmpEE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpEF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpEE;struct Cyc_List_List*ts=_tmpEF;
# 434
new_e=({struct Cyc_Absyn_Exp*_tmp6A5=DC(pt,e1);struct Cyc_List_List*_tmp6A4=((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp6A5,_tmp6A4,e->loc);});goto _LL0;}case 14: _tmpEA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpEB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpEC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;_tmpED=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA9)->f4;{void*t=_tmpEA;struct Cyc_Absyn_Exp*e1=_tmpEB;int b=_tmpEC;enum Cyc_Absyn_Coercion c=_tmpED;
# 436
new_e=({void*_tmp6A9=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp6A8=DC(pt,e1);int _tmp6A7=b;enum Cyc_Absyn_Coercion _tmp6A6=c;Cyc_Absyn_cast_exp(_tmp6A9,_tmp6A8,_tmp6A7,_tmp6A6,e->loc);});goto _LL0;}case 16: _tmpE8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpE9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*eo=_tmpE8;struct Cyc_Absyn_Exp*e1=_tmpE9;
# 438
new_e=({struct Cyc_Absyn_Exp*_tmp6AB=(unsigned)eo?DC(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp6AA=DC(pt,e1);Cyc_Absyn_New_exp(_tmp6AB,_tmp6AA,e->loc);});goto _LL0;}case 15: _tmpE7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct Cyc_Absyn_Exp*e1=_tmpE7;
new_e=({struct Cyc_Absyn_Exp*_tmp6AC=DC(pt,e1);Cyc_Absyn_address_exp(_tmp6AC,e->loc);});goto _LL0;}case 17: _tmpE6=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{void*t=_tmpE6;
new_e=({void*_tmp6AD=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp6AD,e->loc);});goto _LL0;}case 18: _tmpE5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct Cyc_Absyn_Exp*e1=_tmpE5;
new_e=({struct Cyc_Absyn_Exp*_tmp6AE=DC(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp6AE,e->loc);});goto _LL0;}case 19: _tmpE3=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpE4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{void*t=_tmpE3;struct Cyc_List_List*ofs=_tmpE4;
new_e=({void*_tmp6B0=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp6AF=ofs;Cyc_Absyn_offsetof_exp(_tmp6B0,_tmp6AF,e->loc);});goto _LL0;}case 20: _tmpE2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct Cyc_Absyn_Exp*e1=_tmpE2;
new_e=({struct Cyc_Absyn_Exp*_tmp6B1=DC(pt,e1);Cyc_Absyn_deref_exp(_tmp6B1,e->loc);});goto _LL0;}case 41: _tmpE1=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct Cyc_Absyn_Exp*e1=_tmpE1;
new_e=({struct Cyc_Absyn_Exp*_tmp6B2=DC(pt,e1);Cyc_Absyn_extension_exp(_tmp6B2,e->loc);});goto _LL0;}case 24: _tmpE0=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct Cyc_List_List*es=_tmpE0;
new_e=({struct Cyc_List_List*_tmp6B3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_tuple_exp(_tmp6B3,e->loc);});goto _LL0;}case 21: _tmpDC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpDD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpDE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;_tmpDF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA9)->f4;{struct Cyc_Absyn_Exp*e1=_tmpDC;struct _fat_ptr*n=_tmpDD;int f1=_tmpDE;int f2=_tmpDF;
# 447
new_e=({void*_tmp6B5=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->tag=21,({struct Cyc_Absyn_Exp*_tmp6B4=DC(pt,e1);_tmp118->f1=_tmp6B4;}),_tmp118->f2=n,_tmp118->f3=f1,_tmp118->f4=f2;_tmp118;});Cyc_Absyn_new_exp(_tmp6B5,e->loc);});goto _LL0;}case 22: _tmpD8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpD9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpDA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;_tmpDB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA9)->f4;{struct Cyc_Absyn_Exp*e1=_tmpD8;struct _fat_ptr*n=_tmpD9;int f1=_tmpDA;int f2=_tmpDB;
# 449
new_e=({void*_tmp6B7=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=22,({struct Cyc_Absyn_Exp*_tmp6B6=DC(pt,e1);_tmp119->f1=_tmp6B6;}),_tmp119->f2=n,_tmp119->f3=f1,_tmp119->f4=f2;_tmp119;});Cyc_Absyn_new_exp(_tmp6B7,e->loc);});goto _LL0;}case 23: _tmpD6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpD7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpD6;struct Cyc_Absyn_Exp*e2=_tmpD7;
# 451
new_e=({struct Cyc_Absyn_Exp*_tmp6B9=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6B8=DC(pt,e2);Cyc_Absyn_subscript_exp(_tmp6B9,_tmp6B8,e->loc);});goto _LL0;}case 26: _tmpD5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct Cyc_List_List*eds=_tmpD5;
# 453
new_e=({void*_tmp6BB=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=26,({struct Cyc_List_List*_tmp6BA=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp11A->f1=_tmp6BA;});_tmp11A;});Cyc_Absyn_new_exp(_tmp6BB,e->loc);});goto _LL0;}case 31: _tmpD2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpD3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpD4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;{struct Cyc_List_List*es=_tmpD2;struct Cyc_Absyn_Datatypedecl*dtd=_tmpD3;struct Cyc_Absyn_Datatypefield*dtf=_tmpD4;
# 455
new_e=({void*_tmp6BD=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=31,({struct Cyc_List_List*_tmp6BC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);_tmp11B->f1=_tmp6BC;}),_tmp11B->f2=dtd,_tmp11B->f3=dtf;_tmp11B;});Cyc_Absyn_new_exp(_tmp6BD,e->loc);});goto _LL0;}case 27: _tmpCE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpCF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpD0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;_tmpD1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA9)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmpCE;struct Cyc_Absyn_Exp*e1=_tmpCF;struct Cyc_Absyn_Exp*e2=_tmpD0;int b=_tmpD1;
# 457
new_e=({void*_tmp6C0=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=27,_tmp11C->f1=vd,({struct Cyc_Absyn_Exp*_tmp6BF=DC(pt,e1);_tmp11C->f2=_tmp6BF;}),({struct Cyc_Absyn_Exp*_tmp6BE=DC(pt,e2);_tmp11C->f3=_tmp6BE;}),_tmp11C->f4=b;_tmp11C;});Cyc_Absyn_new_exp(_tmp6C0,e->loc);});goto _LL0;}case 28: _tmpCB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpCC=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpCD=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;{struct Cyc_Absyn_Exp*e=_tmpCB;void*t=_tmpCC;int b=_tmpCD;
# 459
new_e=({void*_tmp6C3=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=28,({struct Cyc_Absyn_Exp*_tmp6C2=DC(pt,e);_tmp11D->f1=_tmp6C2;}),({void*_tmp6C1=Cyc_Tcutil_copy_type(t);_tmp11D->f2=_tmp6C1;}),_tmp11D->f3=b;_tmp11D;});Cyc_Absyn_new_exp(_tmp6C3,e->loc);});
goto _LL0;}case 29: _tmpC7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpC8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpC9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;_tmpCA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA9)->f4;{struct _tuple1*n=_tmpC7;struct Cyc_List_List*ts=_tmpC8;struct Cyc_List_List*eds=_tmpC9;struct Cyc_Absyn_Aggrdecl*agr=_tmpCA;
# 462
new_e=({void*_tmp6C6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->tag=29,_tmp11E->f1=n,({struct Cyc_List_List*_tmp6C5=((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp11E->f2=_tmp6C5;}),({struct Cyc_List_List*_tmp6C4=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp11E->f3=_tmp6C4;}),_tmp11E->f4=agr;_tmp11E;});Cyc_Absyn_new_exp(_tmp6C6,e->loc);});
# 464
goto _LL0;}case 30: _tmpC5=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpC6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{void*t=_tmpC5;struct Cyc_List_List*eds=_tmpC6;
# 466
new_e=({void*_tmp6C9=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=30,({void*_tmp6C8=Cyc_Tcutil_copy_type(t);_tmp11F->f1=_tmp6C8;}),({struct Cyc_List_List*_tmp6C7=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp11F->f2=_tmp6C7;});_tmp11F;});Cyc_Absyn_new_exp(_tmp6C9,e->loc);});
goto _LL0;}case 25: _tmpC1=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpA9)->f1)->f1;_tmpC2=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpA9)->f1)->f2;_tmpC3=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpA9)->f1)->f3;_tmpC4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct _fat_ptr*vopt=_tmpC1;struct Cyc_Absyn_Tqual tq=_tmpC2;void*t=_tmpC3;struct Cyc_List_List*eds=_tmpC4;
# 469
new_e=({void*_tmp6CD=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->tag=25,({struct _tuple9*_tmp6CC=({struct _tuple9*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->f1=vopt,_tmp120->f2=tq,({void*_tmp6CB=Cyc_Tcutil_copy_type(t);_tmp120->f3=_tmp6CB;});_tmp120;});_tmp121->f1=_tmp6CC;}),({
struct Cyc_List_List*_tmp6CA=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp121->f2=_tmp6CA;});_tmp121;});
# 469
Cyc_Absyn_new_exp(_tmp6CD,e->loc);});
# 471
goto _LL0;}case 32: _tmpBF=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpC0=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmpBF;struct Cyc_Absyn_Enumfield*ef=_tmpC0;
new_e=e;goto _LL0;}case 33: _tmpBD=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpBE=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{void*t=_tmpBD;struct Cyc_Absyn_Enumfield*ef=_tmpBE;
# 474
new_e=({void*_tmp6CF=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=33,({void*_tmp6CE=Cyc_Tcutil_copy_type(t);_tmp122->f1=_tmp6CE;}),_tmp122->f2=ef;_tmp122;});Cyc_Absyn_new_exp(_tmp6CF,e->loc);});goto _LL0;}case 34: _tmpB7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA9)->f1).is_calloc;_tmpB8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA9)->f1).rgn;_tmpB9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA9)->f1).elt_type;_tmpBA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA9)->f1).num_elts;_tmpBB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA9)->f1).fat_result;_tmpBC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA9)->f1).inline_call;{int ic=_tmpB7;struct Cyc_Absyn_Exp*r=_tmpB8;void**t=_tmpB9;struct Cyc_Absyn_Exp*n=_tmpBA;int res=_tmpBB;int inlc=_tmpBC;
# 476
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=DC(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp123=_cycalloc(sizeof(*_tmp123));({void*_tmp6D0=Cyc_Tcutil_copy_type(*t);*_tmp123=_tmp6D0;});_tmp123;});
({void*_tmp6D1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=34,(_tmp124->f1).is_calloc=ic,(_tmp124->f1).rgn=r1,(_tmp124->f1).elt_type=t1,(_tmp124->f1).num_elts=n,(_tmp124->f1).fat_result=res,(_tmp124->f1).inline_call=inlc;_tmp124;});e2->r=_tmp6D1;});
new_e=e2;
goto _LL0;}}case 35: _tmpB5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpB6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpB5;struct Cyc_Absyn_Exp*e2=_tmpB6;
new_e=({struct Cyc_Absyn_Exp*_tmp6D3=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6D2=DC(pt,e2);Cyc_Absyn_swap_exp(_tmp6D3,_tmp6D2,e->loc);});goto _LL0;}case 36: _tmpB3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpB4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Core_Opt*nopt=_tmpB3;struct Cyc_List_List*eds=_tmpB4;
# 484
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->v=(struct _tuple1*)nopt->v;_tmp125;});
new_e=({void*_tmp6D5=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=36,_tmp126->f1=nopt1,({struct Cyc_List_List*_tmp6D4=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp126->f2=_tmp6D4;});_tmp126;});Cyc_Absyn_new_exp(_tmp6D5,e->loc);});
goto _LL0;}case 37:
# 489
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6D6=({const char*_tmp127="deep_copy: statement expressions unsupported";_tag_fat(_tmp127,sizeof(char),45U);});_tmp128->f1=_tmp6D6;});_tmp128;}));case 38: _tmpB1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpB2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpB1;struct _fat_ptr*fn=_tmpB2;
# 491
new_e=({void*_tmp6D8=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=38,({struct Cyc_Absyn_Exp*_tmp6D7=DC(pt,e1);_tmp129->f1=_tmp6D7;}),_tmp129->f2=fn;_tmp129;});Cyc_Absyn_new_exp(_tmp6D8,e->loc);});goto _LL0;}case 39: _tmpB0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{void*t=_tmpB0;
new_e=({void*_tmp6D9=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp6D9,e->loc);});goto _LL0;}case 40: _tmpAB=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;_tmpAC=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA9)->f2;_tmpAD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA9)->f3;_tmpAE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA9)->f4;_tmpAF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA9)->f5;{int v=_tmpAB;struct _fat_ptr t=_tmpAC;struct Cyc_List_List*o=_tmpAD;struct Cyc_List_List*i=_tmpAE;struct Cyc_List_List*c=_tmpAF;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _tmpAA=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpA9)->f1;{struct Cyc_Absyn_Exp*e1=_tmpAA;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 497
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 501
return new_e;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 506
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 508
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Kinds_ak || k == & Cyc_Kinds_tak)){
# 512
struct _RegionHandle _tmp12A=_new_region("r");struct _RegionHandle*r=& _tmp12A;_push_region(r);
{struct Cyc_List_List*_tmp12B=0;struct Cyc_List_List*inst=_tmp12B;
# 515
for(1;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp12C=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp12C;
void*_tmp12D=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp12D;
enum Cyc_Absyn_KindQual _tmp12E=(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk))->kind;enum Cyc_Absyn_KindQual _stmttmp18=_tmp12E;enum Cyc_Absyn_KindQual _tmp12F=_stmttmp18;switch(_tmp12F){case Cyc_Absyn_IntKind:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL4:
 inst=({struct Cyc_List_List*_tmp131=_region_malloc(r,sizeof(*_tmp131));({struct _tuple17*_tmp6DA=({struct _tuple17*_tmp130=_region_malloc(r,sizeof(*_tmp130));_tmp130->f1=tv,_tmp130->f2=t;_tmp130;});_tmp131->hd=_tmp6DA;}),_tmp131->tl=inst;_tmp131;});goto _LL0;default:
 goto _LL0;}_LL0:;}
# 524
if(inst != 0){
field_type=({struct _RegionHandle*_tmp6DC=r;struct Cyc_List_List*_tmp6DB=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp6DC,_tmp6DB,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 513
;_pop_region();}
# 529
return k;}
# 536
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 538
void*_tmp132=Cyc_Absyn_compress(t);void*_stmttmp19=_tmp132;void*_tmp133=_stmttmp19;struct Cyc_Absyn_Typedefdecl*_tmp134;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_PtrInfo _tmp136;struct Cyc_List_List*_tmp138;void*_tmp137;struct Cyc_Absyn_Tvar*_tmp139;struct Cyc_Core_Opt*_tmp13A;switch(*((int*)_tmp133)){case 1: _tmp13A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp133)->f1;{struct Cyc_Core_Opt*k=_tmp13A;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2: _tmp139=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp133)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp139;
return Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);}case 0: _tmp137=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1;_tmp138=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f2;{void*c=_tmp137;struct Cyc_List_List*ts=_tmp138;
# 542
void*_tmp13B=c;int _tmp13F;struct Cyc_Absyn_AggrdeclImpl*_tmp13E;struct Cyc_List_List*_tmp13D;enum Cyc_Absyn_AggrKind _tmp13C;struct Cyc_Absyn_Kind*_tmp140;enum Cyc_Absyn_Size_of _tmp141;switch(*((int*)_tmp13B)){case 0:
 return& Cyc_Kinds_mk;case 1: _tmp141=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp13B)->f2;{enum Cyc_Absyn_Size_of sz=_tmp141;
# 545
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Kinds_bk:& Cyc_Kinds_mk;}case 2:
 return& Cyc_Kinds_mk;case 15:
 goto _LL27;case 16: _LL27:
 goto _LL29;case 3: _LL29:
 return& Cyc_Kinds_bk;case 6:
 return& Cyc_Kinds_urk;case 5:
 return& Cyc_Kinds_rk;case 7:
 return& Cyc_Kinds_trk;case 17: _tmp140=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp13B)->f2;{struct Cyc_Absyn_Kind*k=_tmp140;
return k;}case 4:
 return& Cyc_Kinds_bk;case 8:
 goto _LL37;case 9: _LL37:
 goto _LL39;case 10: _LL39:
 return& Cyc_Kinds_ek;case 12:
 goto _LL3D;case 11: _LL3D:
 return& Cyc_Kinds_boolk;case 13:
 goto _LL41;case 14: _LL41:
 return& Cyc_Kinds_ptrbk;case 18:
 return& Cyc_Kinds_ak;case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp13B)->f1).KnownDatatypefield).tag == 2)
return& Cyc_Kinds_mk;else{
# 565
({struct Cyc_Warn_String_Warn_Warg_struct _tmp143=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0,({struct _fat_ptr _tmp6DD=({const char*_tmp144="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp144,sizeof(char),40U);});_tmp5F0.f1=_tmp6DD;});_tmp5F0;});void*_tmp142[1];_tmp142[0]=& _tmp143;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp142,sizeof(void*),1));});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp13B)->f1).UnknownAggr).tag == 1)
# 568
return& Cyc_Kinds_ak;else{_tmp13C=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp13B)->f1).KnownAggr).val)->kind;_tmp13D=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp13B)->f1).KnownAggr).val)->tvs;_tmp13E=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp13B)->f1).KnownAggr).val)->impl;_tmp13F=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp13B)->f1).KnownAggr).val)->expected_mem_kind;{enum Cyc_Absyn_AggrKind strOrU=_tmp13C;struct Cyc_List_List*tvs=_tmp13D;struct Cyc_Absyn_AggrdeclImpl*i=_tmp13E;int expected_mem_kind=_tmp13F;
# 570
if(i == 0)
return expected_mem_kind?& Cyc_Kinds_mk:& Cyc_Kinds_ak;{
struct Cyc_List_List*_tmp145=i->fields;struct Cyc_List_List*fields=_tmp145;
if(fields == 0)return& Cyc_Kinds_mk;
# 575
if((int)strOrU == (int)0U){
for(1;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){
;}{
void*_tmp146=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp146;
struct Cyc_Absyn_Kind*_tmp147=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp147;
if(k == & Cyc_Kinds_ak || k == & Cyc_Kinds_tak)return k;}}else{
# 584
for(1;fields != 0;fields=fields->tl){
void*_tmp148=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp148;
struct Cyc_Absyn_Kind*_tmp149=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp149;
if(k == & Cyc_Kinds_ak || k == & Cyc_Kinds_tak)return k;}}
# 589
return& Cyc_Kinds_mk;}}}};}case 5:
# 591
 return& Cyc_Kinds_ak;case 7:
 return& Cyc_Kinds_mk;case 3: _tmp136=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1;{struct Cyc_Absyn_PtrInfo pinfo=_tmp136;
# 594
void*_tmp14A=Cyc_Absyn_compress((pinfo.ptr_atts).bounds);void*_stmttmp1A=_tmp14A;void*_tmp14B=_stmttmp1A;if(*((int*)_tmp14B)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14B)->f1)){case 13:  {
# 596
enum Cyc_Absyn_AliasQual _tmp14C=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp1B=_tmp14C;enum Cyc_Absyn_AliasQual _tmp14D=_stmttmp1B;switch(_tmp14D){case Cyc_Absyn_Aliasable:
 return& Cyc_Kinds_bk;case Cyc_Absyn_Unique:
 return& Cyc_Kinds_ubk;case Cyc_Absyn_Top:
 goto _LL5B;default: _LL5B: return& Cyc_Kinds_tbk;};}case 14:
# 602
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 604
enum Cyc_Absyn_AliasQual _tmp14E=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp1C=_tmp14E;enum Cyc_Absyn_AliasQual _tmp14F=_stmttmp1C;switch(_tmp14F){case Cyc_Absyn_Aliasable:
 return& Cyc_Kinds_mk;case Cyc_Absyn_Unique:
 return& Cyc_Kinds_umk;case Cyc_Absyn_Top:
 goto _LL64;default: _LL64: return& Cyc_Kinds_tmk;};}};}case 9:
# 610
 return& Cyc_Kinds_ik;case 11:
# 614
 return& Cyc_Kinds_ak;case 4: _tmp135=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).num_elts;{struct Cyc_Absyn_Exp*num_elts=_tmp135;
# 616
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Kinds_mk;
return& Cyc_Kinds_ak;}case 6:
 return& Cyc_Kinds_mk;case 8: _tmp134=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f3;{struct Cyc_Absyn_Typedefdecl*td=_tmp134;
# 620
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp151=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0,({struct _fat_ptr _tmp6DE=({const char*_tmp153="type_kind: typedef found: ";_tag_fat(_tmp153,sizeof(char),27U);});_tmp5F2.f1=_tmp6DE;});_tmp5F2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp152=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=2,_tmp5F1.f1=(void*)t;_tmp5F1;});void*_tmp150[2];_tmp150[0]=& _tmp151,_tmp150[1]=& _tmp152;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp150,sizeof(void*),2));});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)){case 1:
 return& Cyc_Kinds_bk;case 0:
 goto _LL1C;default: _LL1C:
 return& Cyc_Kinds_ak;}};}
# 629
static void*Cyc_Tcutil_rgns_of(void*);
# 631
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 635
static struct _tuple17*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp154=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_stmttmp1D=_tmp154;struct Cyc_Absyn_Kind*_tmp155=_stmttmp1D;switch(((struct Cyc_Absyn_Kind*)_tmp155)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp155)->aliasqual){case Cyc_Absyn_Unique:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->tag=9,({struct Cyc_Absyn_Exp*_tmp6DF=Cyc_Absyn_uint_exp(0U,0U);_tmp156->f1=_tmp6DF;});_tmp156;});goto _LL0;case Cyc_Absyn_BoolKind:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 646
return({struct _tuple17*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->f1=tv,_tmp157->f2=t;_tmp157;});}
# 653
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp158=Cyc_Absyn_compress(t);void*_stmttmp1E=_tmp158;void*_tmp159=_stmttmp1E;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp162;struct Cyc_Absyn_VarargInfo*_tmp161;struct Cyc_List_List*_tmp160;void*_tmp15F;struct Cyc_Absyn_Tqual _tmp15E;void*_tmp15D;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*_tmp163;void*_tmp164;void*_tmp166;void*_tmp165;struct Cyc_List_List*_tmp167;switch(*((int*)_tmp159)){case 0: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp159)->f2 == 0)
return Cyc_Absyn_empty_effect;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp159)->f1)== 9)
return t;else{_tmp167=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp159)->f2;{struct Cyc_List_List*ts=_tmp167;
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}}}case 1:
 goto _LLA;case 2: _LLA: {
# 660
struct Cyc_Absyn_Kind*_tmp168=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp1F=_tmp168;struct Cyc_Absyn_Kind*_tmp169=_stmttmp1F;switch(((struct Cyc_Absyn_Kind*)_tmp169)->kind){case Cyc_Absyn_RgnKind:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind:
 return t;case Cyc_Absyn_IntKind:
 return Cyc_Absyn_empty_effect;default:
 return Cyc_Absyn_regionsof_eff(t);};}case 3: _tmp165=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp159)->f1).elt_type;_tmp166=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp159)->f1).ptr_atts).rgn;{void*et=_tmp165;void*r=_tmp166;
# 668
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp16A[2];({void*_tmp6E1=Cyc_Absyn_access_eff(r);_tmp16A[0]=_tmp6E1;}),({void*_tmp6E0=Cyc_Tcutil_rgns_of(et);_tmp16A[1]=_tmp6E0;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp16A,sizeof(void*),2));})));}case 4: _tmp164=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp159)->f1).elt_type;{void*et=_tmp164;
# 670
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7: _tmp163=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp159)->f2;{struct Cyc_List_List*fs=_tmp163;
# 672
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5: _tmp15C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159)->f1).tvars;_tmp15D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159)->f1).effect;_tmp15E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159)->f1).ret_tqual;_tmp15F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159)->f1).ret_type;_tmp160=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159)->f1).args;_tmp161=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159)->f1).cyc_varargs;_tmp162=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159)->f1).rgn_po;{struct Cyc_List_List*tvs=_tmp15C;void*eff=_tmp15D;struct Cyc_Absyn_Tqual rt_tq=_tmp15E;void*rt=_tmp15F;struct Cyc_List_List*args=_tmp160;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp161;struct Cyc_List_List*rpo=_tmp162;
# 681
void*_tmp16B=({struct Cyc_List_List*_tmp6E2=((struct Cyc_List_List*(*)(struct _tuple17*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp6E2,(void*)_check_null(eff));});void*e=_tmp16B;
return Cyc_Tcutil_normalize_effect(e);}case 6: _tmp15B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp159)->f1;{struct Cyc_List_List*tqts=_tmp15B;
# 684
struct Cyc_List_List*_tmp16C=0;struct Cyc_List_List*ts=_tmp16C;
for(1;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp16D->tl=ts;_tmp16D;});}
_tmp15A=ts;goto _LL16;}case 8: _tmp15A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp159)->f2;_LL16: {struct Cyc_List_List*ts=_tmp15A;
# 689
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=0,({struct _fat_ptr _tmp6E3=({const char*_tmp170="typedecl in rgns_of";_tag_fat(_tmp170,sizeof(char),20U);});_tmp5F3.f1=_tmp6E3;});_tmp5F3;});void*_tmp16E[1];_tmp16E[0]=& _tmp16F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp16E,sizeof(void*),1));});case 9:
 goto _LL1C;default: _LL1C:
 return Cyc_Absyn_empty_effect;};}
# 699
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp171=e;void*_tmp172;struct Cyc_List_List**_tmp173;if(*((int*)_tmp171)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp171)->f1)){case 9: _tmp173=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp171)->f2;{struct Cyc_List_List**es=_tmp173;
# 703
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp174=(void*)effs->hd;void*eff=_tmp174;
({void*_tmp6E4=(void*)Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp6E4;});{
void*_tmp175=(void*)effs->hd;void*_stmttmp20=_tmp175;void*_tmp176=_stmttmp20;if(*((int*)_tmp176)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f1)){case 9:
 goto _LLB;case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->hd)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->hd)->f1)){case 5: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->tl == 0){_LLB:
 goto _LLD;}else{goto _LL10;}case 7: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->tl == 0){_LLD:
 goto _LLF;}else{goto _LL10;}case 6: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->tl == 0){_LLF:
# 712
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10:
 goto _LL7;}_LL7:;}}}
# 716
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp177=Cyc_Absyn_compress((void*)effs->hd);void*_stmttmp21=_tmp177;void*_tmp178=_stmttmp21;void*_tmp179;struct Cyc_List_List*_tmp17A;if(*((int*)_tmp178)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp178)->f1)){case 9: _tmp17A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp178)->f2;{struct Cyc_List_List*nested_effs=_tmp17A;
# 721
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_revappend)(nested_effs,effects);goto _LL12;}case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp178)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp178)->f2)->hd)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp178)->f2)->hd)->f1)){case 5: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp178)->f2)->tl == 0)
goto _LL18;else{goto _LL1B;}case 7: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp178)->f2)->tl == 0){_LL18:
 goto _LL1A;}else{goto _LL1B;}case 6: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp178)->f2)->tl == 0){_LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp179=_tmp178;{void*e=_tmp179;
effects=({struct Cyc_List_List*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->hd=e,_tmp17B->tl=effects;_tmp17B;});goto _LL12;}}_LL12:;}}
# 727
({struct Cyc_List_List*_tmp6E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(effects);*es=_tmp6E5;});
return e;}}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp171)->f2 != 0){_tmp172=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp171)->f2)->hd;{void*t=_tmp172;
# 730
void*_tmp17C=Cyc_Absyn_compress(t);void*_stmttmp22=_tmp17C;void*_tmp17D=_stmttmp22;switch(*((int*)_tmp17D)){case 1:
 goto _LL21;case 2: _LL21:
 return e;default:
 return Cyc_Tcutil_rgns_of(t);};}}else{goto _LL5;}default: goto _LL5;}else{_LL5:
# 735
 return e;};}}
# 740
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17E=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->tag=5,(_tmp17F->f1).tvars=0,(_tmp17F->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6E6=Cyc_Absyn_empty_tqual(0U);(_tmp17F->f1).ret_tqual=_tmp6E6;}),(_tmp17F->f1).ret_type=Cyc_Absyn_void_type,(_tmp17F->f1).args=0,(_tmp17F->f1).c_varargs=0,(_tmp17F->f1).cyc_varargs=0,(_tmp17F->f1).rgn_po=0,(_tmp17F->f1).attributes=0,(_tmp17F->f1).requires_clause=0,(_tmp17F->f1).requires_relns=0,(_tmp17F->f1).ensures_clause=0,(_tmp17F->f1).ensures_relns=0,(_tmp17F->f1).return_value=0;_tmp17F;});
# 741
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp17E;
# 751
return({void*_tmp6EB=(void*)fntype;void*_tmp6EA=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6E9=Cyc_Absyn_empty_tqual(0U);void*_tmp6E8=Cyc_Absyn_bounds_one();void*_tmp6E7=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_tmp6EB,_tmp6EA,_tmp6E9,_tmp6E8,_tmp6E7,Cyc_Absyn_false_type);});}
# 758
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Absyn_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp180=Cyc_Absyn_compress(e);void*_stmttmp23=_tmp180;void*_tmp181=_stmttmp23;struct Cyc_Core_Opt*_tmp184;void**_tmp183;struct Cyc_Core_Opt*_tmp182;void*_tmp185;struct Cyc_List_List*_tmp186;void*_tmp187;switch(*((int*)_tmp181)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f1)){case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2 != 0){_tmp187=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2)->hd;{void*r2=_tmp187;
# 765
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Absyn_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp188=({struct _tuple15 _tmp5F4;_tmp5F4.f1=r,_tmp5F4.f2=r2;_tmp5F4;});struct _tuple15 _stmttmp24=_tmp188;struct _tuple15 _tmp189=_stmttmp24;struct Cyc_Absyn_Tvar*_tmp18B;struct Cyc_Absyn_Tvar*_tmp18A;if(*((int*)_tmp189.f1)== 2){if(*((int*)_tmp189.f2)== 2){_tmp18A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp189.f1)->f1;_tmp18B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp189.f2)->f1;{struct Cyc_Absyn_Tvar*tv1=_tmp18A;struct Cyc_Absyn_Tvar*tv2=_tmp18B;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE:
 return 0;};}}}else{goto _LL9;}case 9: _tmp186=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2;{struct Cyc_List_List*es=_tmp186;
# 773
for(1;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2 != 0){_tmp185=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2)->hd;{void*t=_tmp185;
# 777
void*_tmp18C=Cyc_Tcutil_rgns_of(t);void*_stmttmp25=_tmp18C;void*_tmp18D=_stmttmp25;void*_tmp18E;void*_tmp18F;if(*((int*)_tmp18D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18D)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18D)->f2 != 0){_tmp18F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18D)->f2)->hd;{void*t=_tmp18F;
# 779
if(!constrain)return 0;{
void*_tmp190=Cyc_Absyn_compress(t);void*_stmttmp26=_tmp190;void*_tmp191=_stmttmp26;struct Cyc_Core_Opt*_tmp194;void**_tmp193;struct Cyc_Core_Opt*_tmp192;if(*((int*)_tmp191)== 1){_tmp192=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp191)->f1;_tmp193=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp191)->f2;_tmp194=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp191)->f4;{struct Cyc_Core_Opt*k=_tmp192;void**p=_tmp193;struct Cyc_Core_Opt*s=_tmp194;
# 784
void*_tmp195=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp195;
# 787
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp6ED=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp196[2];_tmp196[0]=ev,({void*_tmp6EC=Cyc_Absyn_access_eff(r);_tmp196[1]=_tmp6EC;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp196,sizeof(void*),2));})));*p=_tmp6ED;});
return 1;}}else{
return 0;};}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp18E=_tmp18D;{void*e2=_tmp18E;
# 792
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}};}}else{goto _LL9;}default: goto _LL9;}case 1: _tmp182=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp181)->f1;_tmp183=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp181)->f2;_tmp184=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp181)->f4;{struct Cyc_Core_Opt*k=_tmp182;void**p=_tmp183;struct Cyc_Core_Opt*s=_tmp184;
# 795
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp198=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=0,({struct _fat_ptr _tmp6EE=({const char*_tmp199="effect evar has wrong kind";_tag_fat(_tmp199,sizeof(char),27U);});_tmp5F5.f1=_tmp6EE;});_tmp5F5;});void*_tmp197[1];_tmp197[0]=& _tmp198;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp197,sizeof(void*),1));});
if(!constrain)return 0;{
# 800
void*_tmp19A=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp19A;
# 803
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp6F0=Cyc_Absyn_join_eff(({void*_tmp19B[2];_tmp19B[0]=ev,({void*_tmp6EF=Cyc_Absyn_access_eff(r);_tmp19B[1]=_tmp6EF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp19B,sizeof(void*),2));}));*p=_tmp6F0;});
return 1;}}default: _LL9:
 return 0;};}}
# 813
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Absyn_compress(t);{
void*_tmp19C=Cyc_Tcutil_normalize_effect(Cyc_Absyn_compress(e));void*_stmttmp27=_tmp19C;void*_tmp19D=_stmttmp27;struct Cyc_Core_Opt*_tmp1A0;void**_tmp19F;struct Cyc_Core_Opt*_tmp19E;void*_tmp1A1;struct Cyc_List_List*_tmp1A2;switch(*((int*)_tmp19D)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f1)){case 8:
 return 0;case 9: _tmp1A2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2;{struct Cyc_List_List*es=_tmp1A2;
# 818
for(1;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2 != 0){_tmp1A1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2)->hd;{void*t2=_tmp1A1;
# 823
t2=Cyc_Absyn_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1A3=Cyc_Tcutil_rgns_of(t);void*_stmttmp28=_tmp1A3;void*_tmp1A4=_stmttmp28;void*_tmp1A5;void*_tmp1A6;if(*((int*)_tmp1A4)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2 != 0){_tmp1A6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->hd;{void*t3=_tmp1A6;
# 828
struct _tuple15 _tmp1A7=({struct _tuple15 _tmp5F6;({void*_tmp6F1=Cyc_Absyn_compress(t3);_tmp5F6.f1=_tmp6F1;}),_tmp5F6.f2=t2;_tmp5F6;});struct _tuple15 _stmttmp29=_tmp1A7;struct _tuple15 _tmp1A8=_stmttmp29;struct Cyc_Absyn_Tvar*_tmp1AA;struct Cyc_Absyn_Tvar*_tmp1A9;if(*((int*)_tmp1A8.f1)== 2){if(*((int*)_tmp1A8.f2)== 2){_tmp1A9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8.f1)->f1;_tmp1AA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8.f2)->f1;{struct Cyc_Absyn_Tvar*tv1=_tmp1A9;struct Cyc_Absyn_Tvar*tv2=_tmp1AA;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13:
 return t3 == t2;};}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1A5=_tmp1A4;{void*e2=_tmp1A5;
# 832
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}};}}}else{goto _LL9;}default: goto _LL9;}case 1: _tmp19E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19D)->f1;_tmp19F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19D)->f2;_tmp1A0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19D)->f4;{struct Cyc_Core_Opt*k=_tmp19E;void**p=_tmp19F;struct Cyc_Core_Opt*s=_tmp1A0;
# 835
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F7;_tmp5F7.tag=0,({struct _fat_ptr _tmp6F2=({const char*_tmp1AD="effect evar has wrong kind";_tag_fat(_tmp1AD,sizeof(char),27U);});_tmp5F7.f1=_tmp6F2;});_tmp5F7;});void*_tmp1AB[1];_tmp1AB[0]=& _tmp1AC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1AB,sizeof(void*),1));});
if(!may_constrain_evars)return 0;{
# 840
void*_tmp1AE=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp1AE;
# 843
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1AF=Cyc_Absyn_join_eff(({void*_tmp1B0[2];_tmp1B0[0]=ev,({void*_tmp6F3=Cyc_Absyn_regionsof_eff(t);_tmp1B0[1]=_tmp6F3;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B0,sizeof(void*),2));}));void*new_typ=_tmp1AF;
*p=new_typ;
return 1;}}}default: _LL9:
 return 0;};}}
# 853
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp1B1=e;struct Cyc_Core_Opt*_tmp1B4;void**_tmp1B3;struct Cyc_Core_Opt*_tmp1B2;void*_tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_Absyn_Tvar*_tmp1B7;switch(*((int*)_tmp1B1)){case 2: _tmp1B7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B1)->f1;{struct Cyc_Absyn_Tvar*v2=_tmp1B7;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f1)){case 9: _tmp1B6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f2;{struct Cyc_List_List*es=_tmp1B6;
# 858
for(1;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f2 != 0){_tmp1B5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f2)->hd;{void*t=_tmp1B5;
# 863
void*_tmp1B8=Cyc_Tcutil_rgns_of(t);void*_stmttmp2A=_tmp1B8;void*_tmp1B9=_stmttmp2A;void*_tmp1BA;void*_tmp1BB;if(*((int*)_tmp1B9)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->f2 != 0){_tmp1BB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->f2)->hd;{void*t2=_tmp1BB;
# 865
if(!may_constrain_evars)return 0;{
void*_tmp1BC=Cyc_Absyn_compress(t2);void*_stmttmp2B=_tmp1BC;void*_tmp1BD=_stmttmp2B;struct Cyc_Core_Opt*_tmp1C0;void**_tmp1BF;struct Cyc_Core_Opt*_tmp1BE;if(*((int*)_tmp1BD)== 1){_tmp1BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->f1;_tmp1BF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->f2;_tmp1C0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->f4;{struct Cyc_Core_Opt*k=_tmp1BE;void**p=_tmp1BF;struct Cyc_Core_Opt*s=_tmp1C0;
# 871
void*_tmp1C1=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp1C1;
# 873
if(!((int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp6F5=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1C2[2];_tmp1C2[0]=ev,({void*_tmp6F4=Cyc_Absyn_var_type(v);_tmp1C2[1]=_tmp6F4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C2,sizeof(void*),2));})));*p=_tmp6F5;});
return 1;}}else{
return 0;};}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1BA=_tmp1B9;{void*e2=_tmp1BA;
# 878
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}};}}else{goto _LL9;}default: goto _LL9;}case 1: _tmp1B2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B1)->f1;_tmp1B3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B1)->f2;_tmp1B4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B1)->f4;{struct Cyc_Core_Opt*k=_tmp1B2;void**p=_tmp1B3;struct Cyc_Core_Opt*s=_tmp1B4;
# 881
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F8;_tmp5F8.tag=0,({struct _fat_ptr _tmp6F6=({const char*_tmp1C5="effect evar has wrong kind";_tag_fat(_tmp1C5,sizeof(char),27U);});_tmp5F8.f1=_tmp6F6;});_tmp5F8;});void*_tmp1C3[1];_tmp1C3[0]=& _tmp1C4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1C3,sizeof(void*),1));});{
# 885
void*_tmp1C6=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp1C6;
# 887
if(!((int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1C7=Cyc_Absyn_join_eff(({void*_tmp1C8[2];_tmp1C8[0]=ev,({void*_tmp6F7=Cyc_Absyn_var_type(v);_tmp1C8[1]=_tmp6F7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C8,sizeof(void*),2));}));void*new_typ=_tmp1C7;
*p=new_typ;
return 1;}}}default: _LL9:
 return 0;};}}
# 897
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp1C9=e;void*_tmp1CA;struct Cyc_List_List*_tmp1CB;switch(*((int*)_tmp1C9)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f1)){case 9: _tmp1CB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f2;{struct Cyc_List_List*es=_tmp1CB;
# 901
for(1;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f2 != 0){_tmp1CA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f2)->hd;{void*t=_tmp1CA;
# 906
void*_tmp1CC=Cyc_Tcutil_rgns_of(t);void*_stmttmp2C=_tmp1CC;void*_tmp1CD=_stmttmp2C;void*_tmp1CE;void*_tmp1CF;if(*((int*)_tmp1CD)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CD)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CD)->f2 != 0){_tmp1CF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CD)->f2)->hd;{void*t2=_tmp1CF;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1CE=_tmp1CD;{void*e2=_tmp1CE;
return Cyc_Tcutil_evar_in_effect(evar,e2);}};}}else{goto _LL7;}default: goto _LL7;}case 1:
# 910
 return evar == e;default: _LL7:
 return 0;};}}
# 924 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 927
void*_tmp1D0=Cyc_Absyn_compress(e1);void*_stmttmp2D=_tmp1D0;void*_tmp1D1=_stmttmp2D;struct Cyc_Core_Opt*_tmp1D3;void**_tmp1D2;struct Cyc_Absyn_Tvar*_tmp1D4;void*_tmp1D5;void*_tmp1D6;struct Cyc_List_List*_tmp1D7;switch(*((int*)_tmp1D1)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f1)){case 9: _tmp1D7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2;{struct Cyc_List_List*es=_tmp1D7;
# 929
for(1;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2 != 0){_tmp1D6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2)->hd;{void*r=_tmp1D6;
# 940
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2 != 0){_tmp1D5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2)->hd;{void*t=_tmp1D5;
# 944
void*_tmp1D8=Cyc_Tcutil_rgns_of(t);void*_stmttmp2E=_tmp1D8;void*_tmp1D9=_stmttmp2E;void*_tmp1DA;void*_tmp1DB;if(*((int*)_tmp1D9)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f2 != 0){_tmp1DB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f2)->hd;{void*t2=_tmp1DB;
# 949
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1DA=_tmp1D9;{void*e=_tmp1DA;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}};}}else{goto _LLB;}default: goto _LLB;}case 2: _tmp1D4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D1)->f1;{struct Cyc_Absyn_Tvar*v=_tmp1D4;
# 942
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1: _tmp1D2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D1)->f2;_tmp1D3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D1)->f4;{void**p=_tmp1D2;struct Cyc_Core_Opt*s=_tmp1D3;
# 954
if(Cyc_Tcutil_evar_in_effect(e1,e2))
return 0;
# 959
*p=Cyc_Absyn_empty_effect;
# 962
return 1;}default: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=0,({struct _fat_ptr _tmp6F8=({const char*_tmp1DF="subset_effect: bad effect: ";_tag_fat(_tmp1DF,sizeof(char),28U);});_tmp5FA.f1=_tmp6F8;});_tmp5FA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1DE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5F9;_tmp5F9.tag=2,_tmp5F9.f1=(void*)e1;_tmp5F9;});void*_tmp1DC[2];_tmp1DC[0]=& _tmp1DD,_tmp1DC[1]=& _tmp1DE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1DC,sizeof(void*),2));});};}
# 973
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 975
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp1E0=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp2F=_tmp1E0;struct _tuple15*_tmp1E1=_stmttmp2F;void*_tmp1E3;void*_tmp1E2;_tmp1E2=_tmp1E1->f1;_tmp1E3=_tmp1E1->f2;{void*t1a=_tmp1E2;void*t1b=_tmp1E3;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp1E4=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp30=_tmp1E4;struct _tuple15*_tmp1E5=_stmttmp30;void*_tmp1E7;void*_tmp1E6;_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E5->f2;{void*t2a=_tmp1E6;void*t2b=_tmp1E7;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 985
if(!found)return 0;}}}
# 987
return 1;}
# 991
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 993
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 996
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp1E8=t;switch(*((int*)_tmp1E8)){case 0:
 return 0;case 1: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1E8)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1E8)->f2){case Cyc_Absyn_Char_sz:
 return 1;case Cyc_Absyn_Short_sz:
# 1002
 return 4;case Cyc_Absyn_Int_sz:
# 1005
 return 7;case Cyc_Absyn_Long_sz:
# 1008
 return 7;case Cyc_Absyn_LongLong_sz:
# 1011
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1E8)->f2){case Cyc_Absyn_Char_sz:
# 1000
 return 2;case Cyc_Absyn_Short_sz:
# 1003
 return 5;case Cyc_Absyn_Int_sz:
# 1006
 return 8;case Cyc_Absyn_Long_sz:
# 1009
 return 8;case Cyc_Absyn_LongLong_sz:
# 1012
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1E8)->f2){case Cyc_Absyn_Char_sz:
# 1001
 return 3;case Cyc_Absyn_Short_sz:
# 1004
 return 6;case Cyc_Absyn_Int_sz:
# 1007
 return 9;case Cyc_Absyn_Long_sz:
# 1010
 return 9;case Cyc_Absyn_LongLong_sz:
# 1013
 return 15;default: goto _LL4B;}default: _LL4B:
# 1035
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FB;_tmp5FB.tag=0,({struct _fat_ptr _tmp6F9=({const char*_tmp1EB="bad con";_tag_fat(_tmp1EB,sizeof(char),8U);});_tmp5FB.f1=_tmp6F9;});_tmp5FB;});void*_tmp1E9[1];_tmp1E9[0]=& _tmp1EA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1E9,sizeof(void*),1));});}case 2: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp1E8)->f1){case 0:
# 1014
 return 16;case 1:
 return 17;default:
 return 18;}case 3:
 return 19;case 4:
 return 20;case 5:
 return 21;case 6:
 return 22;case 7:
 return 23;case 8:
 return 24;case 9:
 return 25;case 10:
 return 26;case 11:
 return 27;case 12:
 return 28;case 14:
 return 29;case 13:
 return 30;case 15:
 return 31;case 16:
 return 32;case 17:
 return 33;case 18:
 return 34;case 19:
 return 35;default:
 return 36;};}
# 1038
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp1EC=t;void*_tmp1ED;switch(*((int*)_tmp1EC)){case 1:
 return 1;case 2:
 return 2;case 3:
 return 3;case 4:
 return 4;case 5:
 return 5;case 6:
 return 6;case 7:
 return 7;case 8:
 return 8;case 9:
 return 9;case 10:
 return 10;case 11:
 return 11;default: _tmp1ED=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f1;{void*c=_tmp1ED;
return 12 + Cyc_Tcutil_tycon2int(c);}};}
# 1054
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp1EE=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp1EE;
if(qc != 0)return qc;
return((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1059
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp1EF=i;struct _tuple1*_tmp1F0;struct Cyc_Absyn_Datatypedecl*_tmp1F1;if((_tmp1EF.KnownDatatype).tag == 2){_tmp1F1=*(_tmp1EF.KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl*dd=_tmp1F1;
return dd->name;}}else{_tmp1F0=((_tmp1EF.UnknownDatatype).val).name;{struct _tuple1*n=_tmp1F0;
return n;}};}struct _tuple18{struct _tuple1*f1;struct _tuple1*f2;};
# 1065
static struct _tuple18 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp1F2=i;struct _tuple1*_tmp1F4;struct _tuple1*_tmp1F3;struct Cyc_Absyn_Datatypefield*_tmp1F6;struct Cyc_Absyn_Datatypedecl*_tmp1F5;if((_tmp1F2.KnownDatatypefield).tag == 2){_tmp1F5=((_tmp1F2.KnownDatatypefield).val).f1;_tmp1F6=((_tmp1F2.KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*dd=_tmp1F5;struct Cyc_Absyn_Datatypefield*df=_tmp1F6;
# 1068
return({struct _tuple18 _tmp5FC;_tmp5FC.f1=dd->name,_tmp5FC.f2=df->name;_tmp5FC;});}}else{_tmp1F3=((_tmp1F2.UnknownDatatypefield).val).datatype_name;_tmp1F4=((_tmp1F2.UnknownDatatypefield).val).field_name;{struct _tuple1*d=_tmp1F3;struct _tuple1*f=_tmp1F4;
# 1070
return({struct _tuple18 _tmp5FD;_tmp5FD.f1=d,_tmp5FD.f2=f;_tmp5FD;});}};}struct _tuple19{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1073
static struct _tuple19 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp1F7=i;struct Cyc_Absyn_Aggrdecl*_tmp1F8;struct _tuple1*_tmp1FA;enum Cyc_Absyn_AggrKind _tmp1F9;if((_tmp1F7.UnknownAggr).tag == 1){_tmp1F9=((_tmp1F7.UnknownAggr).val).f1;_tmp1FA=((_tmp1F7.UnknownAggr).val).f2;{enum Cyc_Absyn_AggrKind k=_tmp1F9;struct _tuple1*n=_tmp1FA;
return({struct _tuple19 _tmp5FE;_tmp5FE.f1=k,_tmp5FE.f2=n;_tmp5FE;});}}else{_tmp1F8=*(_tmp1F7.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp1F8;
return({struct _tuple19 _tmp5FF;_tmp5FF.f1=ad->kind,_tmp5FF.f2=ad->name;_tmp5FF;});}};}
# 1079
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1085
struct _tuple15 _tmp1FB=({struct _tuple15 _tmp600;_tmp600.f1=t1,_tmp600.f2=t2;_tmp600;});struct _tuple15 _stmttmp31=_tmp1FB;struct _tuple15 _tmp1FC=_stmttmp31;union Cyc_Absyn_AggrInfo _tmp1FE;union Cyc_Absyn_AggrInfo _tmp1FD;union Cyc_Absyn_DatatypeFieldInfo _tmp200;union Cyc_Absyn_DatatypeFieldInfo _tmp1FF;union Cyc_Absyn_DatatypeInfo _tmp202;union Cyc_Absyn_DatatypeInfo _tmp201;struct Cyc_List_List*_tmp204;struct Cyc_List_List*_tmp203;struct _fat_ptr _tmp206;struct _fat_ptr _tmp205;struct _tuple1*_tmp208;struct _tuple1*_tmp207;switch(*((int*)_tmp1FC.f1)){case 15: if(*((int*)_tmp1FC.f2)== 15){_tmp207=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp1FC.f1)->f1;_tmp208=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp1FC.f2)->f1;{struct _tuple1*n1=_tmp207;struct _tuple1*n2=_tmp208;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17: if(*((int*)_tmp1FC.f2)== 17){_tmp205=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp1FC.f1)->f1;_tmp206=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp1FC.f2)->f1;{struct _fat_ptr s1=_tmp205;struct _fat_ptr s2=_tmp206;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16: if(*((int*)_tmp1FC.f2)== 16){_tmp203=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp1FC.f1)->f1;_tmp204=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp1FC.f2)->f1;{struct Cyc_List_List*fs1=_tmp203;struct Cyc_List_List*fs2=_tmp204;
# 1089
return((int(*)(int(*)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18: if(*((int*)_tmp1FC.f2)== 18){_tmp201=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp1FC.f1)->f1;_tmp202=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp1FC.f2)->f1;{union Cyc_Absyn_DatatypeInfo info1=_tmp201;union Cyc_Absyn_DatatypeInfo info2=_tmp202;
# 1091
return({struct _tuple1*_tmp6FA=Cyc_Tcutil_get_datatype_qvar(info1);Cyc_Absyn_qvar_cmp(_tmp6FA,Cyc_Tcutil_get_datatype_qvar(info2));});}}else{goto _LLD;}case 19: if(*((int*)_tmp1FC.f2)== 19){_tmp1FF=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp1FC.f1)->f1;_tmp200=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp1FC.f2)->f1;{union Cyc_Absyn_DatatypeFieldInfo info1=_tmp1FF;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp200;
# 1093
struct _tuple18 _tmp209=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple18 _stmttmp32=_tmp209;struct _tuple18 _tmp20A=_stmttmp32;struct _tuple1*_tmp20C;struct _tuple1*_tmp20B;_tmp20B=_tmp20A.f1;_tmp20C=_tmp20A.f2;{struct _tuple1*d1=_tmp20B;struct _tuple1*f1=_tmp20C;
struct _tuple18 _tmp20D=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple18 _stmttmp33=_tmp20D;struct _tuple18 _tmp20E=_stmttmp33;struct _tuple1*_tmp210;struct _tuple1*_tmp20F;_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;{struct _tuple1*d2=_tmp20F;struct _tuple1*f2=_tmp210;
int _tmp211=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp211;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20: if(*((int*)_tmp1FC.f2)== 20){_tmp1FD=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp1FC.f1)->f1;_tmp1FE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp1FC.f2)->f1;{union Cyc_Absyn_AggrInfo info1=_tmp1FD;union Cyc_Absyn_AggrInfo info2=_tmp1FE;
# 1099
struct _tuple19 _tmp212=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple19 _stmttmp34=_tmp212;struct _tuple19 _tmp213=_stmttmp34;struct _tuple1*_tmp215;enum Cyc_Absyn_AggrKind _tmp214;_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;{enum Cyc_Absyn_AggrKind k1=_tmp214;struct _tuple1*q1=_tmp215;
struct _tuple19 _tmp216=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple19 _stmttmp35=_tmp216;struct _tuple19 _tmp217=_stmttmp35;struct _tuple1*_tmp219;enum Cyc_Absyn_AggrKind _tmp218;_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;{enum Cyc_Absyn_AggrKind k2=_tmp218;struct _tuple1*q2=_tmp219;
int _tmp21A=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp21A;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD:
 return 0;};}}}
# 1108
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6FC)(void*,void*)=cmp;void*_tmp6FB=(void*)_check_null(a1);_tmp6FC(_tmp6FB,(void*)_check_null(a2));});}
# 1114
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp21B=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp21B;
int _tmp21C=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp21C;
return Cyc_Core_intcmp(i1,i2);}
# 1119
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp21D=tqt1;void*_tmp21F;struct Cyc_Absyn_Tqual _tmp21E;_tmp21E=_tmp21D->f1;_tmp21F=_tmp21D->f2;{struct Cyc_Absyn_Tqual tq1=_tmp21E;void*t1=_tmp21F;
struct _tuple12*_tmp220=tqt2;void*_tmp222;struct Cyc_Absyn_Tqual _tmp221;_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;{struct Cyc_Absyn_Tqual tq2=_tmp221;void*t2=_tmp222;
int _tmp223=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp223;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1127
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp224=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp224;
if(zsc != 0)return zsc;{
int _tmp225=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp225;
if(tqc != 0)return tqc;{
int _tmp226=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp226;
if(tc != 0)return tc;{
int _tmp227=((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Atts_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp227;
if(ac != 0)return ac;
ac=((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1144
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1 == t2)return 0;{
int _tmp228=({int _tmp6FD=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6FD,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp228;
if(shallowcmp != 0)
return shallowcmp;{
# 1153
struct _tuple15 _tmp229=({struct _tuple15 _tmp605;_tmp605.f1=t2,_tmp605.f2=t1;_tmp605;});struct _tuple15 _stmttmp36=_tmp229;struct _tuple15 _tmp22A=_stmttmp36;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_List_List*_tmp232;enum Cyc_Absyn_AggrKind _tmp231;struct Cyc_List_List*_tmp230;enum Cyc_Absyn_AggrKind _tmp22F;struct Cyc_List_List*_tmp234;struct Cyc_List_List*_tmp233;struct Cyc_Absyn_FnInfo _tmp236;struct Cyc_Absyn_FnInfo _tmp235;void*_tmp23E;struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_Tqual _tmp23C;void*_tmp23B;void*_tmp23A;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Tqual _tmp238;void*_tmp237;void*_tmp24C;void*_tmp24B;void*_tmp24A;void*_tmp249;void*_tmp248;struct Cyc_Absyn_Tqual _tmp247;void*_tmp246;void*_tmp245;void*_tmp244;void*_tmp243;void*_tmp242;void*_tmp241;struct Cyc_Absyn_Tqual _tmp240;void*_tmp23F;struct Cyc_Absyn_Tvar*_tmp24E;struct Cyc_Absyn_Tvar*_tmp24D;struct Cyc_List_List*_tmp252;void*_tmp251;struct Cyc_List_List*_tmp250;void*_tmp24F;switch(*((int*)_tmp22A.f1)){case 0: if(*((int*)_tmp22A.f2)== 0){_tmp24F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A.f1)->f1;_tmp250=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A.f1)->f2;_tmp251=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A.f2)->f1;_tmp252=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A.f2)->f2;{void*c1=_tmp24F;struct Cyc_List_List*ts1=_tmp250;void*c2=_tmp251;struct Cyc_List_List*ts2=_tmp252;
# 1155
int _tmp253=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp253;
if(c != 0)return c;
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1: if(*((int*)_tmp22A.f2)== 1)
# 1159
({struct Cyc_Warn_String_Warn_Warg_struct _tmp255=({struct Cyc_Warn_String_Warn_Warg_struct _tmp601;_tmp601.tag=0,({struct _fat_ptr _tmp6FE=({const char*_tmp256="typecmp: can only compare closed types";_tag_fat(_tmp256,sizeof(char),39U);});_tmp601.f1=_tmp6FE;});_tmp601;});void*_tmp254[1];_tmp254[0]=& _tmp255;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp254,sizeof(void*),1));});else{goto _LL15;}case 2: if(*((int*)_tmp22A.f2)== 2){_tmp24D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22A.f1)->f1;_tmp24E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22A.f2)->f1;{struct Cyc_Absyn_Tvar*tv2=_tmp24D;struct Cyc_Absyn_Tvar*tv1=_tmp24E;
# 1163
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3: if(*((int*)_tmp22A.f2)== 3){_tmp23F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f1)->f1).elt_type;_tmp240=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f1)->f1).elt_tq;_tmp241=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f1)->f1).ptr_atts).rgn;_tmp242=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f1)->f1).ptr_atts).nullable;_tmp243=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f1)->f1).ptr_atts).bounds;_tmp244=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f1)->f1).ptr_atts).zero_term;_tmp245=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f1)->f1).ptr_atts).released;_tmp246=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f2)->f1).elt_type;_tmp247=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f2)->f1).elt_tq;_tmp248=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f2)->f1).ptr_atts).rgn;_tmp249=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f2)->f1).ptr_atts).nullable;_tmp24A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f2)->f1).ptr_atts).bounds;_tmp24B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f2)->f1).ptr_atts).zero_term;_tmp24C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A.f2)->f1).ptr_atts).released;{void*t2a=_tmp23F;struct Cyc_Absyn_Tqual tqual2a=_tmp240;void*rgn2=_tmp241;void*null2a=_tmp242;void*b2=_tmp243;void*zt2=_tmp244;void*rel2=_tmp245;void*t1a=_tmp246;struct Cyc_Absyn_Tqual tqual1a=_tmp247;void*rgn1=_tmp248;void*null1a=_tmp249;void*b1=_tmp24A;void*zt1=_tmp24B;void*rel1=_tmp24C;
# 1167
int _tmp257=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp257;
if(etc != 0)return etc;{
int _tmp258=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp258;
if(rc != 0)return rc;{
int _tmp259=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp259;
if(tqc != 0)return tqc;{
int _tmp25A=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp25A;
if(cc != 0)return cc;{
int _tmp25B=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp25B;
if(zc != 0)return zc;{
int _tmp25C=Cyc_Tcutil_typecmp(rel1,rel2);int relc=_tmp25C;
if(relc != 0)return relc;{
int _tmp25D=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp25D;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}}else{goto _LL15;}case 4: if(*((int*)_tmp22A.f2)== 4){_tmp237=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A.f1)->f1).elt_type;_tmp238=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A.f1)->f1).tq;_tmp239=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A.f1)->f1).num_elts;_tmp23A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A.f1)->f1).zero_term;_tmp23B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A.f2)->f1).elt_type;_tmp23C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A.f2)->f1).tq;_tmp23D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A.f2)->f1).num_elts;_tmp23E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A.f2)->f1).zero_term;{void*t2a=_tmp237;struct Cyc_Absyn_Tqual tq2a=_tmp238;struct Cyc_Absyn_Exp*e1=_tmp239;void*zt1=_tmp23A;void*t1a=_tmp23B;struct Cyc_Absyn_Tqual tq1a=_tmp23C;struct Cyc_Absyn_Exp*e2=_tmp23D;void*zt2=_tmp23E;
# 1185
int _tmp25E=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp25E;
if(tqc != 0)return tqc;{
int _tmp25F=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp25F;
if(tc != 0)return tc;{
int _tmp260=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp260;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp262=({struct Cyc_Warn_String_Warn_Warg_struct _tmp602;_tmp602.tag=0,({struct _fat_ptr _tmp6FF=({const char*_tmp263="missing expression in array index";_tag_fat(_tmp263,sizeof(char),34U);});_tmp602.f1=_tmp6FF;});_tmp602;});void*_tmp261[1];_tmp261[0]=& _tmp262;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp261,sizeof(void*),1));});
return((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5: if(*((int*)_tmp22A.f2)== 5){_tmp235=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp22A.f1)->f1;_tmp236=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp22A.f2)->f1;{struct Cyc_Absyn_FnInfo f1=_tmp235;struct Cyc_Absyn_FnInfo f2=_tmp236;
# 1197
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp264=f1.args;struct Cyc_List_List*args1=_tmp264;
struct Cyc_List_List*_tmp265=f2.args;struct Cyc_List_List*args2=_tmp265;
for(1;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp266=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp37=_tmp266;struct _tuple9 _tmp267=_stmttmp37;void*_tmp269;struct Cyc_Absyn_Tqual _tmp268;_tmp268=_tmp267.f2;_tmp269=_tmp267.f3;{struct Cyc_Absyn_Tqual tq1=_tmp268;void*t1=_tmp269;
struct _tuple9 _tmp26A=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp38=_tmp26A;struct _tuple9 _tmp26B=_stmttmp38;void*_tmp26D;struct Cyc_Absyn_Tqual _tmp26C;_tmp26C=_tmp26B.f2;_tmp26D=_tmp26B.f3;{struct Cyc_Absyn_Tqual tq2=_tmp26C;void*t2=_tmp26D;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1212
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp700=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp700,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1226
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp26E=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp26E;
struct Cyc_List_List*_tmp26F=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp26F;
for(1;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp270=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp39=_tmp270;struct _tuple15 _tmp271=_stmttmp39;void*_tmp273;void*_tmp272;_tmp272=_tmp271.f1;_tmp273=_tmp271.f2;{void*t1a=_tmp272;void*t1b=_tmp273;
struct _tuple15 _tmp274=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp3A=_tmp274;struct _tuple15 _tmp275=_stmttmp3A;void*_tmp277;void*_tmp276;_tmp276=_tmp275.f1;_tmp277=_tmp275.f2;{void*t2a=_tmp276;void*t2b=_tmp277;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1236
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1244
({struct Cyc_Warn_String_Warn_Warg_struct _tmp279=({struct Cyc_Warn_String_Warn_Warg_struct _tmp603;_tmp603.tag=0,({struct _fat_ptr _tmp701=({const char*_tmp27A="typecmp: function type comparison should never get here!";_tag_fat(_tmp27A,sizeof(char),57U);});_tmp603.f1=_tmp701;});_tmp603;});void*_tmp278[1];_tmp278[0]=& _tmp279;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp278,sizeof(void*),1));});}}}}}else{goto _LL15;}case 6: if(*((int*)_tmp22A.f2)== 6){_tmp233=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp22A.f1)->f1;_tmp234=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp22A.f2)->f1;{struct Cyc_List_List*ts2=_tmp233;struct Cyc_List_List*ts1=_tmp234;
# 1247
return((int(*)(int(*)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7: if(*((int*)_tmp22A.f2)== 7){_tmp22F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp22A.f1)->f1;_tmp230=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp22A.f1)->f2;_tmp231=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp22A.f2)->f1;_tmp232=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp22A.f2)->f2;{enum Cyc_Absyn_AggrKind k2=_tmp22F;struct Cyc_List_List*fs2=_tmp230;enum Cyc_Absyn_AggrKind k1=_tmp231;struct Cyc_List_List*fs1=_tmp232;
# 1250
if((int)k1 != (int)k2)
return(int)k1 == (int)0U?- 1: 1;
return((int(*)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9: if(*((int*)_tmp22A.f2)== 9){_tmp22D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp22A.f1)->f1;_tmp22E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp22A.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp22D;struct Cyc_Absyn_Exp*e2=_tmp22E;
# 1254
_tmp22B=e1;_tmp22C=e2;goto _LL14;}}else{goto _LL15;}case 11: if(*((int*)_tmp22A.f2)== 11){_tmp22B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp22A.f1)->f1;_tmp22C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp22A.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp22B;struct Cyc_Absyn_Exp*e2=_tmp22C;
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp604;_tmp604.tag=0,({struct _fat_ptr _tmp702=({const char*_tmp27D="Unmatched case in typecmp";_tag_fat(_tmp27D,sizeof(char),26U);});_tmp604.f1=_tmp702;});_tmp604;});void*_tmp27B[1];_tmp27B[0]=& _tmp27C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp27B,sizeof(void*),1));});};}}}
# 1262
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp27E=({struct _tuple15 _tmp607;({void*_tmp704=Cyc_Absyn_compress(t1);_tmp607.f1=_tmp704;}),({void*_tmp703=Cyc_Absyn_compress(t2);_tmp607.f2=_tmp703;});_tmp607;});struct _tuple15 _stmttmp3B=_tmp27E;struct _tuple15 _tmp27F=_stmttmp3B;void*_tmp281;void*_tmp280;if(*((int*)_tmp27F.f1)== 0){if(*((int*)_tmp27F.f2)== 0){_tmp280=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F.f1)->f1;_tmp281=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F.f2)->f1;{void*c1=_tmp280;void*c2=_tmp281;
# 1265
struct _tuple15 _tmp282=({struct _tuple15 _tmp606;_tmp606.f1=c1,_tmp606.f2=c2;_tmp606;});struct _tuple15 _stmttmp3C=_tmp282;struct _tuple15 _tmp283=_stmttmp3C;int _tmp285;int _tmp284;switch(*((int*)_tmp283.f1)){case 2: switch(*((int*)_tmp283.f2)){case 2: _tmp284=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp283.f1)->f1;_tmp285=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp283.f2)->f1;{int i1=_tmp284;int i2=_tmp285;
return i2 < i1;}case 1:
 goto _LLB;case 4: _LLB:
 return 1;default: goto _LL26;}case 1: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp283.f1)->f2){case Cyc_Absyn_LongLong_sz: if(*((int*)_tmp283.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp283.f2)->f2 == Cyc_Absyn_LongLong_sz)
return 0;else{goto _LLE;}}else{_LLE:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp283.f2)){case 1: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp283.f2)->f2){case Cyc_Absyn_Int_sz:
# 1273
 goto _LL13;case Cyc_Absyn_Short_sz: _LL19:
# 1278
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1F:
# 1281
 goto _LL21;default: goto _LL26;}case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp283.f2)->f1 == 0)
# 1276
goto _LL17;else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp283.f2)){case 1: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp283.f2)->f2){case Cyc_Absyn_Long_sz: _LL13:
# 1274
 return 0;case Cyc_Absyn_Short_sz: _LL1B:
# 1279
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL21:
# 1282
 goto _LL23;default: goto _LL26;}case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp283.f2)->f1 == 0){_LL17:
# 1277
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(*((int*)_tmp283.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp283.f2)->f2 == Cyc_Absyn_Char_sz){_LL23:
# 1283
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4: if(*((int*)_tmp283.f2)== 1)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp283.f2)->f2){case Cyc_Absyn_Short_sz: _LL1D:
# 1280
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL25:
# 1284
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26:
# 1286
 return 0;};}}else{goto _LL3;}}else{_LL3:
# 1288
 return 0;};}
# 1292
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp286=({struct _tuple15 _tmp609;({void*_tmp706=Cyc_Absyn_compress(t1);_tmp609.f1=_tmp706;}),({void*_tmp705=Cyc_Absyn_compress(t2);_tmp609.f2=_tmp705;});_tmp609;});struct _tuple15 _stmttmp3D=_tmp286;struct _tuple15 _tmp287=_stmttmp3D;void*_tmp289;void*_tmp288;if(*((int*)_tmp287.f1)== 0){if(*((int*)_tmp287.f2)== 0){_tmp288=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp287.f1)->f1;_tmp289=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp287.f2)->f1;{void*c1=_tmp288;void*c2=_tmp289;
# 1295
{struct _tuple15 _tmp28A=({struct _tuple15 _tmp608;_tmp608.f1=c1,_tmp608.f2=c2;_tmp608;});struct _tuple15 _stmttmp3E=_tmp28A;struct _tuple15 _tmp28B=_stmttmp3E;int _tmp28D;int _tmp28C;if(*((int*)_tmp28B.f1)== 2){if(*((int*)_tmp28B.f2)== 2){_tmp28C=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp28B.f1)->f1;_tmp28D=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1;{int i1=_tmp28C;int i2=_tmp28D;
# 1297
if(i1 != 0 && i1 != 1)return t1;
if(i2 != 0 && i2 != 1)return t2;
if(i1 >= i2)return t1;
return t2;}}else{
return t1;}}else{if(*((int*)_tmp28B.f2)== 2)
return t2;else{if(*((int*)_tmp28B.f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_LongLong_sz)
goto _LLF;else{if(*((int*)_tmp28B.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_Long_sz){_LL14:
# 1307
 goto _LL17;}else{if(*((int*)_tmp28B.f2)== 4)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1312
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(*((int*)_tmp28B.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10:
# 1305
 goto _LL13;}else{if(*((int*)_tmp28B.f2)== 4)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f1)->f2 == Cyc_Absyn_Long_sz){_LL20:
# 1314
 goto _LL23;}else{goto _LL24;}}}}}}else{if(*((int*)_tmp28B.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1304
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1308
 return Cyc_Absyn_ulong_type;default: if(*((int*)_tmp28B.f1)== 4)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1313
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1306
 return Cyc_Absyn_slonglong_type;}else{if(*((int*)_tmp28B.f1)== 4)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28B.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1315
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(*((int*)_tmp28B.f1)== 4){_LL18:
# 1310
 goto _LL1B;}else{if(*((int*)_tmp28B.f2)== 4){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24:
# 1316
 goto _LL5;}}}}}}_LL5:;}
# 1318
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 1321
return Cyc_Absyn_sint_type;}
# 1326
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1329
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Absyn_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E->v=t1;_tmp28E;});}}}
# 1337
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1341
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp290=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D;_tmp60D.tag=0,({struct _fat_ptr _tmp707=({const char*_tmp295="type mismatch: expecting ";_tag_fat(_tmp295,sizeof(char),26U);});_tmp60D.f1=_tmp707;});_tmp60D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp291=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp60C;_tmp60C.tag=2,_tmp60C.f1=(void*)t;_tmp60C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp292=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60B;_tmp60B.tag=0,({struct _fat_ptr _tmp708=({const char*_tmp294=" but found ";_tag_fat(_tmp294,sizeof(char),12U);});_tmp60B.f1=_tmp708;});_tmp60B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp293=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp60A;_tmp60A.tag=2,_tmp60A.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt);_tmp60A;});void*_tmp28F[4];_tmp28F[0]=& _tmp290,_tmp28F[1]=& _tmp291,_tmp28F[2]=& _tmp292,_tmp28F[3]=& _tmp293;({unsigned _tmp709=((struct Cyc_Absyn_Exp*)el->hd)->loc;Cyc_Warn_err2(_tmp709,_tag_fat(_tmp28F,sizeof(void*),4));});});
# 1345
return 0;}}}
# 1347
return 1;}
# 1352
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp296=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp3F=_tmp296;void*_tmp297=_stmttmp3F;if(*((int*)_tmp297)== 3){
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{
return 0;}_LL0:;}
# 1358
return 1;}
# 1361
static int Cyc_Tcutil_coerce_Xint_type(void*Xint_type,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Xint_type))
return 1;
# 1365
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Xint_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp299=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60E;_tmp60E.tag=0,({struct _fat_ptr _tmp70A=({const char*_tmp29A="integral size mismatch; conversion supplied";_tag_fat(_tmp29A,sizeof(char),44U);});_tmp60E.f1=_tmp70A;});_tmp60E;});void*_tmp298[1];_tmp298[0]=& _tmp299;({unsigned _tmp70B=e->loc;Cyc_Warn_warn2(_tmp70B,_tag_fat(_tmp298,sizeof(void*),1));});});
Cyc_Tcutil_unchecked_cast(e,Xint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1371
return 0;}
# 1374
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_uint_type,e);}
# 1378
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_sint_type,e);}
# 1384
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1390
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp70C=t;Cyc_Unify_unify(_tmp70C,Cyc_Absyn_bounds_one());});
return Cyc_Absyn_compress(t);}
# 1395
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp29B=Cyc_Absyn_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp29B;
void*_tmp29C=t;struct Cyc_Absyn_Exp*_tmp29D;if(*((int*)_tmp29C)== 9){_tmp29D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp29C)->f1;{struct Cyc_Absyn_Exp*e=_tmp29D;
return e;}}else{
# 1400
struct Cyc_Absyn_Exp*_tmp29E=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp29E;
v->topt=Cyc_Absyn_uint_type;
return v;};}
# 1409
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp29F=Cyc_Absyn_compress(b);void*_stmttmp40=_tmp29F;void*_tmp2A0=_stmttmp40;struct Cyc_List_List*_tmp2A1;if(*((int*)_tmp2A0)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0)->f1)){case 14:
 return 0;case 13: _tmp2A1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0)->f2;{struct Cyc_List_List*ts=_tmp2A1;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp610;_tmp610.tag=0,({struct _fat_ptr _tmp70D=({const char*_tmp2A5="get_bounds_exp: ";_tag_fat(_tmp2A5,sizeof(char),17U);});_tmp610.f1=_tmp70D;});_tmp610;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp60F;_tmp60F.tag=2,_tmp60F.f1=(void*)b;_tmp60F;});void*_tmp2A2[2];_tmp2A2[0]=& _tmp2A3,_tmp2A2[1]=& _tmp2A4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2A2,sizeof(void*),2));});};}}
# 1418
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2A6=Cyc_Absyn_compress(t);void*_stmttmp41=_tmp2A6;void*_tmp2A7=_stmttmp41;void*_tmp2A8;if(*((int*)_tmp2A7)== 3){_tmp2A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7)->f1).ptr_atts).bounds;{void*b=_tmp2A8;
# 1421
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp612;_tmp612.tag=0,({struct _fat_ptr _tmp70E=({const char*_tmp2AC="get_ptr_bounds_exp not pointer: ";_tag_fat(_tmp2AC,sizeof(char),33U);});_tmp612.f1=_tmp70E;});_tmp612;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2AB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp611;_tmp611.tag=2,_tmp611.f1=(void*)t;_tmp611;});void*_tmp2A9[2];_tmp2A9[0]=& _tmp2AA,_tmp2A9[1]=& _tmp2AB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2A9,sizeof(void*),2));});};}
# 1427
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Kinds_boolko,({struct Cyc_Core_Opt*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->v=tvs;_tmp2AD;}));}
# 1431
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Kinds_ptrbko,({struct Cyc_Core_Opt*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE->v=tvs;_tmp2AE;}));}
# 1436
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);struct _tuple20{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1443
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp2AF=({struct _tuple15 _tmp619;_tmp619.f1=t1,_tmp619.f2=t2;_tmp619;});struct _tuple15 _stmttmp42=_tmp2AF;struct _tuple15 _tmp2B0=_stmttmp42;void*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B7;struct Cyc_Absyn_Tqual _tmp2B6;void*_tmp2B5;void*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Tqual _tmp2B2;void*_tmp2B1;struct Cyc_Absyn_PtrInfo _tmp2BA;struct Cyc_Absyn_PtrInfo _tmp2B9;switch(*((int*)_tmp2B0.f1)){case 3: if(*((int*)_tmp2B0.f2)== 3){_tmp2B9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0.f1)->f1;_tmp2BA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0.f2)->f1;{struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2B9;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2BA;
# 1449
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable)&&
 Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable))
return 0;
# 1453
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple20 _tmp2BB=({struct _tuple20 _tmp614;({struct Cyc_Absyn_Exp*_tmp712=({void*_tmp711=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp711,(pinfo_a.ptr_atts).bounds);});_tmp614.f1=_tmp712;}),({
struct Cyc_Absyn_Exp*_tmp710=({void*_tmp70F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp70F,(pinfo_b.ptr_atts).bounds);});_tmp614.f2=_tmp710;});_tmp614;});
# 1454
struct _tuple20 _stmttmp43=_tmp2BB;struct _tuple20 _tmp2BC=_stmttmp43;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;if(_tmp2BC.f2 == 0)
# 1457
goto _LL9;else{if(_tmp2BC.f1 == 0){
# 1460
if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp713=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp713,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
return 0;}else{_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;{struct Cyc_Absyn_Exp*e1=_tmp2BD;struct Cyc_Absyn_Exp*e2=_tmp2BE;
# 1468
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp613;_tmp613.tag=0,({struct _fat_ptr _tmp714=({const char*_tmp2C1="implicit cast to shorter array";_tag_fat(_tmp2C1,sizeof(char),31U);});_tmp613.f1=_tmp714;});_tmp613;});void*_tmp2BF[1];_tmp2BF[0]=& _tmp2C0;({unsigned _tmp715=loc;Cyc_Warn_warn2(_tmp715,_tag_fat(_tmp2BF,sizeof(void*),1));});});
if(!({struct Cyc_Absyn_Exp*_tmp716=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp716,(struct Cyc_Absyn_Exp*)_check_null(e1));}))
return 0;
goto _LL9;}}}_LL9:;}
# 1476
if((pinfo_a.elt_tq).real_const && !(pinfo_b.elt_tq).real_const)
return 0;
# 1479
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(!Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn))
# 1482
return 0;
if(Cyc_Flags_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp618;_tmp618.tag=0,({struct _fat_ptr _tmp717=({const char*_tmp2C8="implicit cast from region ";_tag_fat(_tmp2C8,sizeof(char),27U);});_tmp618.f1=_tmp717;});_tmp618;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp617;_tmp617.tag=2,_tmp617.f1=(void*)(pinfo_a.ptr_atts).rgn;_tmp617;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp616;_tmp616.tag=0,({
struct _fat_ptr _tmp718=({const char*_tmp2C7=" to region ";_tag_fat(_tmp2C7,sizeof(char),12U);});_tmp616.f1=_tmp718;});_tmp616;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp615;_tmp615.tag=2,_tmp615.f1=(void*)(pinfo_b.ptr_atts).rgn;_tmp615;});void*_tmp2C2[4];_tmp2C2[0]=& _tmp2C3,_tmp2C2[1]=& _tmp2C4,_tmp2C2[2]=& _tmp2C5,_tmp2C2[3]=& _tmp2C6;({unsigned _tmp719=loc;Cyc_Warn_warn2(_tmp719,_tag_fat(_tmp2C2,sizeof(void*),4));});});}
# 1488
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)&&(
!Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)|| !(pinfo_b.elt_tq).real_const))
# 1491
return 0;
# 1493
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).released,(pinfo_b.ptr_atts).released)&& !
Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).released))
return 0;
# 1499
if(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type))
return 1;
# 1503
if(!({void*_tmp71A=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp71A,(pinfo_b.ptr_atts).bounds);})||
 Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
return 0;
if(!(pinfo_b.elt_tq).real_const && !
Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type)))
return 0;
return Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type);}}else{goto _LL7;}case 4: if(*((int*)_tmp2B0.f2)== 4){_tmp2B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0.f1)->f1).elt_type;_tmp2B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0.f1)->f1).tq;_tmp2B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0.f1)->f1).num_elts;_tmp2B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0.f1)->f1).zero_term;_tmp2B5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0.f2)->f1).elt_type;_tmp2B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0.f2)->f1).tq;_tmp2B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0.f2)->f1).num_elts;_tmp2B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0.f2)->f1).zero_term;{void*t1a=_tmp2B1;struct Cyc_Absyn_Tqual tq1a=_tmp2B2;struct Cyc_Absyn_Exp*e1=_tmp2B3;void*zt1=_tmp2B4;void*t2a=_tmp2B5;struct Cyc_Absyn_Tqual tq2a=_tmp2B6;struct Cyc_Absyn_Exp*e2=_tmp2B7;void*zt2=_tmp2B8;
# 1513
if(!Cyc_Unify_unify(zt1,zt2))return 0;
if(e1 == 0 || e2 == 0)return 0;
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))return 0;
if(!Cyc_Unify_unify(t1a,t2a))return 0;
# 1518
return !tq1a.real_const || tq2a.real_const;}}else{goto _LL7;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0.f1)->f1)== 4){if(*((int*)_tmp2B0.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0.f2)->f1)== 1)
# 1520
return 0;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7:
# 1522
 return Cyc_Unify_unify(t1,t2);};}}
# 1526
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2C9=Cyc_Absyn_compress(t);void*_stmttmp44=_tmp2C9;void*_tmp2CA=_stmttmp44;void*_tmp2CB;if(*((int*)_tmp2CA)== 3){_tmp2CB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA)->f1).elt_type;{void*e=_tmp2CB;
return e;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61A;_tmp61A.tag=0,({struct _fat_ptr _tmp71B=({const char*_tmp2CE="pointer_elt_type";_tag_fat(_tmp2CE,sizeof(char),17U);});_tmp61A.f1=_tmp71B;});_tmp61A;});void*_tmp2CC[1];_tmp2CC[0]=& _tmp2CD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2CC,sizeof(void*),1));});};}
# 1532
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2CF=Cyc_Absyn_compress(t);void*_stmttmp45=_tmp2CF;void*_tmp2D0=_stmttmp45;struct Cyc_Absyn_PtrAtts*_tmp2D1;if(*((int*)_tmp2D0)== 3){_tmp2D1=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D0)->f1).ptr_atts;{struct Cyc_Absyn_PtrAtts*p=_tmp2D1;
return p->rgn;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61B;_tmp61B.tag=0,({struct _fat_ptr _tmp71C=({const char*_tmp2D4="pointer_elt_type";_tag_fat(_tmp2D4,sizeof(char),17U);});_tmp61B.f1=_tmp71C;});_tmp61B;});void*_tmp2D2[1];_tmp2D2[0]=& _tmp2D3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2D2,sizeof(void*),1));});};}
# 1539
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2D5=Cyc_Absyn_compress(t);void*_stmttmp46=_tmp2D5;void*_tmp2D6=_stmttmp46;void*_tmp2D7;if(*((int*)_tmp2D6)== 3){_tmp2D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D6)->f1).ptr_atts).rgn;{void*r=_tmp2D7;
*rgn=r;return 1;}}else{
return 0;};}
# 1549
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2D8=Cyc_Absyn_compress(t);void*_stmttmp47=_tmp2D8;void*_tmp2D9=_stmttmp47;void*_tmp2DB;void*_tmp2DA;switch(*((int*)_tmp2D9)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9)->f1)){case 1:
 goto _LL4;case 2: _LL4:
 return 1;default: goto _LL7;}case 3: _tmp2DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D9)->f1).ptr_atts).nullable;_tmp2DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D9)->f1).ptr_atts).bounds;{void*nullable=_tmp2DA;void*bounds=_tmp2DB;
# 1557
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7:
 return 0;};}
# 1563
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2DC=e->r;void*_stmttmp48=_tmp2DC;void*_tmp2DD=_stmttmp48;struct Cyc_Absyn_Exp*_tmp2DF;void*_tmp2DE;struct _fat_ptr _tmp2E0;switch(*((int*)_tmp2DD)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1).Int_c).val).f2 == 0)
goto _LL4;else{goto _LLF;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1).Char_c).val).f2 == 0){_LL4:
 goto _LL6;}else{goto _LLF;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1).Short_c).val).f2 == 0){_LL6:
 goto _LL8;}else{goto _LLF;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1).LongLong_c).val).f2 == 0){_LL8:
 goto _LLA;}else{goto _LLF;}case 3: _tmp2E0=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1).Wchar_c).val;{struct _fat_ptr s=_tmp2E0;
# 1571
unsigned long _tmp2E1=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2E1;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(1;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}
# 1581
return 0;}default: goto _LLF;}case 2: _LLA:
# 1569
 return 1;case 14: _tmp2DE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1;_tmp2DF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DD)->f2;{void*t=_tmp2DE;struct Cyc_Absyn_Exp*e2=_tmp2DF;
# 1582
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF:
 return 0;};}
# 1589
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp2E2=Cyc_Absyn_compress(t2);void*_stmttmp49=_tmp2E2;void*_tmp2E3=_stmttmp49;void*_tmp2E4;if(*((int*)_tmp2E3)== 3){_tmp2E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E3)->f1).ptr_atts).nullable;{void*nbl=_tmp2E4;
# 1594
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp71D=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp71D;});
e1->topt=t2;
return 1;}}else{
return 0;};}}struct _tuple21{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1607
struct _tuple21 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Kinds_rk};
# 1611
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp720=({struct _fat_ptr*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));({struct _fat_ptr _tmp71F=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2F2=({struct Cyc_Int_pa_PrintArg_struct _tmp61D;_tmp61D.tag=1,_tmp61D.f1=(unsigned long)counter ++;_tmp61D;});void*_tmp2F0[1];_tmp2F0[0]=& _tmp2F2;({struct _fat_ptr _tmp71E=({const char*_tmp2F1="__aliasvar%d";_tag_fat(_tmp2F1,sizeof(char),13U);});Cyc_aprintf(_tmp71E,_tag_fat(_tmp2F0,sizeof(void*),1));});});*_tmp2F3=_tmp71F;});_tmp2F3;});_tmp2F4->f2=_tmp720;});_tmp2F4;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp721=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF->tag=4,_tmp2EF->f1=vd;_tmp2EF;});Cyc_Absyn_varb_exp(_tmp721,e->loc);});
# 1620
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp2E5=Cyc_Absyn_compress(e_type);void*_stmttmp4A=_tmp2E5;void*_tmp2E6=_stmttmp4A;void*_tmp2E9;struct Cyc_Absyn_Tqual _tmp2E8;void*_tmp2E7;if(*((int*)_tmp2E6)== 3){_tmp2E7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6)->f1).elt_type;_tmp2E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6)->f1).elt_tq;_tmp2E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6)->f1).ptr_atts).rgn;{void*et=_tmp2E7;struct Cyc_Absyn_Tqual tq=_tmp2E8;void*old_r=_tmp2E9;
# 1623
{void*_tmp2EA=Cyc_Absyn_compress(old_r);void*_stmttmp4B=_tmp2EA;void*_tmp2EB=_stmttmp4B;struct Cyc_Core_Opt*_tmp2ED;void**_tmp2EC;if(*((int*)_tmp2EB)== 1){_tmp2EC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EB)->f2;_tmp2ED=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EB)->f4;{void**topt=_tmp2EC;struct Cyc_Core_Opt*ts=_tmp2ED;
# 1625
void*_tmp2EE=Cyc_Absyn_var_type(tv);void*new_r=_tmp2EE;
*topt=new_r;
goto _LL5;}}else{
goto _LL5;}_LL5:;}
# 1630
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 1633
e->topt=0;
vd->initializer=0;{
# 1637
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1639
return({struct _tuple21 _tmp61C;_tmp61C.f1=d,_tmp61C.f2=ve;_tmp61C;});}}
# 1644
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1647
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1652
void*_tmp2F5=Cyc_Absyn_compress(wants_type);void*_stmttmp4C=_tmp2F5;void*_tmp2F6=_stmttmp4C;void*_tmp2F7;if(*((int*)_tmp2F6)== 3){_tmp2F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6)->f1).ptr_atts).rgn;{void*r2=_tmp2F7;
# 1654
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp2F8=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp2F8;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{
return 0;};}
# 1660
return 0;}
# 1664
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1666
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1670
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1673
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1675
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp622;_tmp622.tag=0,({struct _fat_ptr _tmp722=({const char*_tmp301="integral size mismatch; ";_tag_fat(_tmp301,sizeof(char),25U);});_tmp622.f1=_tmp722;});_tmp622;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp621;_tmp621.tag=2,_tmp621.f1=(void*)t1;_tmp621;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp620;_tmp620.tag=0,({
struct _fat_ptr _tmp723=({const char*_tmp300=" -> ";_tag_fat(_tmp300,sizeof(char),5U);});_tmp620.f1=_tmp723;});_tmp620;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61F;_tmp61F.tag=2,_tmp61F.f1=(void*)t2;_tmp61F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2FE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61E;_tmp61E.tag=0,({struct _fat_ptr _tmp724=({const char*_tmp2FF=" conversion supplied";_tag_fat(_tmp2FF,sizeof(char),21U);});_tmp61E.f1=_tmp724;});_tmp61E;});void*_tmp2F9[5];_tmp2F9[0]=& _tmp2FA,_tmp2F9[1]=& _tmp2FB,_tmp2F9[2]=& _tmp2FC,_tmp2F9[3]=& _tmp2FD,_tmp2F9[4]=& _tmp2FE;({unsigned _tmp725=e->loc;Cyc_Warn_warn2(_tmp725,_tag_fat(_tmp2F9,sizeof(void*),5));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1683
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Flags_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp303=({struct Cyc_Warn_String_Warn_Warg_struct _tmp628;_tmp628.tag=0,({struct _fat_ptr _tmp726=({const char*_tmp30B="implicit alias coercion for ";_tag_fat(_tmp30B,sizeof(char),29U);});_tmp628.f1=_tmp726;});_tmp628;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp304=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp627;_tmp627.tag=4,_tmp627.f1=e;_tmp627;});struct Cyc_Warn_String_Warn_Warg_struct _tmp305=({struct Cyc_Warn_String_Warn_Warg_struct _tmp626;_tmp626.tag=0,({struct _fat_ptr _tmp727=({const char*_tmp30A=":";_tag_fat(_tmp30A,sizeof(char),2U);});_tmp626.f1=_tmp727;});_tmp626;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp306=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp625;_tmp625.tag=2,_tmp625.f1=(void*)t1;_tmp625;});struct Cyc_Warn_String_Warn_Warg_struct _tmp307=({struct Cyc_Warn_String_Warn_Warg_struct _tmp624;_tmp624.tag=0,({struct _fat_ptr _tmp728=({const char*_tmp309=" to ";_tag_fat(_tmp309,sizeof(char),5U);});_tmp624.f1=_tmp728;});_tmp624;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp308=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp623;_tmp623.tag=2,_tmp623.f1=(void*)t2;_tmp623;});void*_tmp302[6];_tmp302[0]=& _tmp303,_tmp302[1]=& _tmp304,_tmp302[2]=& _tmp305,_tmp302[3]=& _tmp306,_tmp302[4]=& _tmp307,_tmp302[5]=& _tmp308;({unsigned _tmp729=e->loc;Cyc_Warn_warn2(_tmp729,_tag_fat(_tmp302,sizeof(void*),6));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1690
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1694
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1698
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp30D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62C;_tmp62C.tag=0,({struct _fat_ptr _tmp72A=({const char*_tmp312="implicit cast from ";_tag_fat(_tmp312,sizeof(char),20U);});_tmp62C.f1=_tmp72A;});_tmp62C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp30E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62B;_tmp62B.tag=2,_tmp62B.f1=(void*)t1;_tmp62B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp30F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62A;_tmp62A.tag=0,({struct _fat_ptr _tmp72B=({const char*_tmp311=" to ";_tag_fat(_tmp311,sizeof(char),5U);});_tmp62A.f1=_tmp72B;});_tmp62A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp310=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp629;_tmp629.tag=2,_tmp629.f1=(void*)t2;_tmp629;});void*_tmp30C[4];_tmp30C[0]=& _tmp30D,_tmp30C[1]=& _tmp30E,_tmp30C[2]=& _tmp30F,_tmp30C[3]=& _tmp310;({unsigned _tmp72C=e->loc;Cyc_Warn_warn2(_tmp72C,_tag_fat(_tmp30C,sizeof(void*),4));});});
return 1;}
# 1704
return 0;}
# 1710
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1713
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1723
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple22{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1725
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple22*env,struct Cyc_Absyn_Aggrfield*x){
# 1728
struct _tuple22 _tmp313=*env;struct _tuple22 _stmttmp4D=_tmp313;struct _tuple22 _tmp314=_stmttmp4D;int _tmp317;struct _RegionHandle*_tmp316;struct Cyc_List_List*_tmp315;_tmp315=_tmp314.f1;_tmp316=_tmp314.f2;_tmp317=_tmp314.f3;{struct Cyc_List_List*inst=_tmp315;struct _RegionHandle*r=_tmp316;int flatten=_tmp317;
void*_tmp318=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp318;
struct Cyc_List_List*_tmp319=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp319;
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp31B=_region_malloc(r,sizeof(*_tmp31B));({struct _tuple12*_tmp72D=({struct _tuple12*_tmp31A=_region_malloc(r,sizeof(*_tmp31A));_tmp31A->f1=x->tq,_tmp31A->f2=t;_tmp31A;});_tmp31B->hd=_tmp72D;}),_tmp31B->tl=0;_tmp31B;});
return ts;}}struct _tuple23{struct _RegionHandle*f1;int f2;};
# 1735
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple23*env,struct _tuple12*x){
# 1737
struct _tuple23 _tmp31C=*env;struct _tuple23 _stmttmp4E=_tmp31C;struct _tuple23 _tmp31D=_stmttmp4E;int _tmp31F;struct _RegionHandle*_tmp31E;_tmp31E=_tmp31D.f1;_tmp31F=_tmp31D.f2;{struct _RegionHandle*r=_tmp31E;int flatten=_tmp31F;
struct _tuple12 _tmp320=*x;struct _tuple12 _stmttmp4F=_tmp320;struct _tuple12 _tmp321=_stmttmp4F;void*_tmp323;struct Cyc_Absyn_Tqual _tmp322;_tmp322=_tmp321.f1;_tmp323=_tmp321.f2;{struct Cyc_Absyn_Tqual tq=_tmp322;void*t=_tmp323;
struct Cyc_List_List*_tmp324=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp324;
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp326=_region_malloc(r,sizeof(*_tmp326));({struct _tuple12*_tmp72E=({struct _tuple12*_tmp325=_region_malloc(r,sizeof(*_tmp325));_tmp325->f1=tq,_tmp325->f2=t;_tmp325;});_tmp326->hd=_tmp72E;}),_tmp326->tl=0;_tmp326;});
return ts;}}}
# 1744
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1746
if(flatten){
t1=Cyc_Absyn_compress(t1);{
void*_tmp327=t1;struct Cyc_List_List*_tmp328;struct Cyc_List_List*_tmp329;struct Cyc_List_List*_tmp32B;struct Cyc_Absyn_Aggrdecl*_tmp32A;switch(*((int*)_tmp327)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp327)->f1)){case 0:
 return 0;case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp327)->f1)->f1).KnownAggr).tag == 2){_tmp32A=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp327)->f1)->f1).KnownAggr).val;_tmp32B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp327)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_tmp32A;struct Cyc_List_List*ts=_tmp32B;
# 1764
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 1766
return({struct Cyc_List_List*_tmp336=_region_malloc(r,sizeof(*_tmp336));({struct _tuple12*_tmp730=({struct _tuple12*_tmp335=_region_malloc(r,sizeof(*_tmp335));({struct Cyc_Absyn_Tqual _tmp72F=Cyc_Absyn_empty_tqual(0U);_tmp335->f1=_tmp72F;}),_tmp335->f2=t1;_tmp335;});_tmp336->hd=_tmp730;}),_tmp336->tl=0;_tmp336;});{
struct Cyc_List_List*_tmp337=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp337;
struct _tuple22 env=({struct _tuple22 _tmp62D;_tmp62D.f1=inst,_tmp62D.f2=r,_tmp62D.f3=flatten;_tmp62D;});
struct Cyc_List_List*_tmp338=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp50=_tmp338;struct Cyc_List_List*_tmp339=_stmttmp50;struct Cyc_List_List*_tmp33B;struct Cyc_Absyn_Aggrfield*_tmp33A;if(_tmp339 == 0)
return 0;else{_tmp33A=(struct Cyc_Absyn_Aggrfield*)_tmp339->hd;_tmp33B=_tmp339->tl;{struct Cyc_Absyn_Aggrfield*hd=_tmp33A;struct Cyc_List_List*tl=_tmp33B;
# 1772
struct Cyc_List_List*_tmp33C=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp33C;
env.f3=0;{
struct Cyc_List_List*_tmp33D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple22*,struct Cyc_Absyn_Aggrfield*),struct _tuple22*,struct Cyc_List_List*))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp33D;
struct Cyc_List_List*_tmp33E=({struct Cyc_List_List*_tmp33F=_region_malloc(r,sizeof(*_tmp33F));_tmp33F->hd=hd2,_tmp33F->tl=tl2;_tmp33F;});struct Cyc_List_List*tts=_tmp33E;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rflatten)(r,tts);}}};}}}else{goto _LL9;}default: goto _LL9;}case 6: _tmp329=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp327)->f1;{struct Cyc_List_List*tqs=_tmp329;
# 1751
struct _tuple23 _tmp32C=({struct _tuple23 _tmp62E;_tmp62E.f1=r,_tmp62E.f2=flatten;_tmp62E;});struct _tuple23 env=_tmp32C;
# 1753
struct Cyc_List_List*_tmp32D=tqs;struct Cyc_List_List*_tmp32F;struct _tuple12*_tmp32E;if(_tmp32D == 0)
return 0;else{_tmp32E=(struct _tuple12*)_tmp32D->hd;_tmp32F=_tmp32D->tl;{struct _tuple12*hd=_tmp32E;struct Cyc_List_List*tl=_tmp32F;
# 1756
struct Cyc_List_List*_tmp330=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp330;
env.f2=0;{
struct Cyc_List_List*_tmp331=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple23*,struct _tuple12*),struct _tuple23*,struct Cyc_List_List*))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp331;
struct Cyc_List_List*_tmp332=({struct Cyc_List_List*_tmp334=_region_malloc(r,sizeof(*_tmp334));_tmp334->hd=hd2,_tmp334->tl=tl2;_tmp334;});struct Cyc_List_List*temp=_tmp332;
return({struct _RegionHandle*_tmp731=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rflatten)(_tmp731,({struct Cyc_List_List*_tmp333=_region_malloc(r,sizeof(*_tmp333));_tmp333->hd=hd2,_tmp333->tl=tl2;_tmp333;}));});}}};}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327)->f1 == Cyc_Absyn_StructA){_tmp328=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327)->f2;{struct Cyc_List_List*fs=_tmp328;
# 1779
struct _tuple22 env=({struct _tuple22 _tmp62F;_tmp62F.f1=0,_tmp62F.f2=r,_tmp62F.f3=flatten;_tmp62F;});
struct Cyc_List_List*_tmp340=fs;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Aggrfield*_tmp341;if(_tmp340 == 0)
return 0;else{_tmp341=(struct Cyc_Absyn_Aggrfield*)_tmp340->hd;_tmp342=_tmp340->tl;{struct Cyc_Absyn_Aggrfield*hd=_tmp341;struct Cyc_List_List*tl=_tmp342;
# 1783
struct Cyc_List_List*_tmp343=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp343;
env.f3=0;{
struct Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple22*,struct Cyc_Absyn_Aggrfield*),struct _tuple22*,struct Cyc_List_List*))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp344;
struct Cyc_List_List*_tmp345=({struct Cyc_List_List*_tmp346=_region_malloc(r,sizeof(*_tmp346));_tmp346->hd=hd2,_tmp346->tl=tl2;_tmp346;});struct Cyc_List_List*tts=_tmp345;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rflatten)(r,tts);}}};}}else{goto _LL9;}default: _LL9:
# 1789
 goto _LL0;}_LL0:;}}
# 1792
return({struct Cyc_List_List*_tmp348=_region_malloc(r,sizeof(*_tmp348));({struct _tuple12*_tmp733=({struct _tuple12*_tmp347=_region_malloc(r,sizeof(*_tmp347));({struct Cyc_Absyn_Tqual _tmp732=Cyc_Absyn_empty_tqual(0U);_tmp347->f1=_tmp732;}),_tmp347->f2=t1;_tmp347;});_tmp348->hd=_tmp733;}),_tmp348->tl=0;_tmp348;});}
# 1795
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp349=({struct _tuple15 _tmp630;({void*_tmp735=Cyc_Absyn_compress(t1);_tmp630.f1=_tmp735;}),({void*_tmp734=Cyc_Absyn_compress(t2);_tmp630.f2=_tmp734;});_tmp630;});struct _tuple15 _stmttmp51=_tmp349;struct _tuple15 _tmp34A=_stmttmp51;enum Cyc_Absyn_Size_of _tmp34C;enum Cyc_Absyn_Size_of _tmp34B;if(*((int*)_tmp34A.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A.f1)->f1)== 1){if(*((int*)_tmp34A.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A.f2)->f1)== 1){_tmp34B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A.f1)->f1)->f2;_tmp34C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A.f2)->f1)->f2;{enum Cyc_Absyn_Size_of b1=_tmp34B;enum Cyc_Absyn_Size_of b2=_tmp34C;
# 1798
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 1806
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1808
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp34D=({struct _tuple15 _tmp631;_tmp631.f1=t1,_tmp631.f2=t2;_tmp631;});struct _tuple15 _stmttmp52=_tmp34D;struct _tuple15 _tmp34E=_stmttmp52;struct Cyc_Absyn_FnInfo _tmp350;struct Cyc_Absyn_FnInfo _tmp34F;struct Cyc_List_List*_tmp355;struct Cyc_Absyn_Datatypedecl*_tmp354;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_Datatypefield*_tmp352;struct Cyc_Absyn_Datatypedecl*_tmp351;void*_tmp363;void*_tmp362;void*_tmp361;void*_tmp360;void*_tmp35F;struct Cyc_Absyn_Tqual _tmp35E;void*_tmp35D;void*_tmp35C;void*_tmp35B;void*_tmp35A;void*_tmp359;void*_tmp358;struct Cyc_Absyn_Tqual _tmp357;void*_tmp356;switch(*((int*)_tmp34E.f1)){case 3: if(*((int*)_tmp34E.f2)== 3){_tmp356=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f1)->f1).elt_type;_tmp357=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f1)->f1).elt_tq;_tmp358=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f1)->f1).ptr_atts).rgn;_tmp359=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f1)->f1).ptr_atts).nullable;_tmp35A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f1)->f1).ptr_atts).bounds;_tmp35B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f1)->f1).ptr_atts).zero_term;_tmp35C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f1)->f1).ptr_atts).released;_tmp35D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f2)->f1).elt_type;_tmp35E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f2)->f1).elt_tq;_tmp35F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f2)->f1).ptr_atts).rgn;_tmp360=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f2)->f1).ptr_atts).nullable;_tmp361=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f2)->f1).ptr_atts).bounds;_tmp362=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f2)->f1).ptr_atts).zero_term;_tmp363=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E.f2)->f1).ptr_atts).released;{void*t_a=_tmp356;struct Cyc_Absyn_Tqual q_a=_tmp357;void*rt_a=_tmp358;void*null_a=_tmp359;void*b_a=_tmp35A;void*zt_a=_tmp35B;void*rel_a=_tmp35C;void*t_b=_tmp35D;struct Cyc_Absyn_Tqual q_b=_tmp35E;void*rt_b=_tmp35F;void*null_b=_tmp360;void*b_b=_tmp361;void*zt_b=_tmp362;void*rel_b=_tmp363;
# 1820
if(q_a.real_const && !q_b.real_const)
return 0;
# 1823
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 1827
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 1831
if((Cyc_Unify_unify(rel_a,rel_b)&& !
Cyc_Absyn_type2bool(0,rel_a))&& Cyc_Absyn_type2bool(0,rel_b))
return 0;
# 1835
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 1839
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp364=({void*_tmp736=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp736,b_a);});struct Cyc_Absyn_Exp*e1=_tmp364;
struct Cyc_Absyn_Exp*_tmp365=({void*_tmp737=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp737,b_b);});struct Cyc_Absyn_Exp*e2=_tmp365;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 1848
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 1853
int _tmp366=({void*_tmp738=b_b;Cyc_Unify_unify(_tmp738,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);int deep_subtype=_tmp366;
# 1856
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp73C=po;struct Cyc_List_List*_tmp73B=({struct Cyc_List_List*_tmp368=_cycalloc(sizeof(*_tmp368));({struct _tuple15*_tmp739=({struct _tuple15*_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367->f1=t1,_tmp367->f2=t2;_tmp367;});_tmp368->hd=_tmp739;}),_tmp368->tl=assume;_tmp368;});void*_tmp73A=t_a;Cyc_Tcutil_ptrsubtype(_tmp73C,_tmp73B,_tmp73A,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f1)->f1)== 19){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(*((int*)_tmp34E.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f2)->f1)== 18){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f2)->f1)->f1).KnownDatatype).tag == 2){_tmp351=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp352=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp353=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f1)->f2;_tmp354=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f2)->f1)->f1).KnownDatatype).val;_tmp355=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34E.f2)->f2;{struct Cyc_Absyn_Datatypedecl*dd1=_tmp351;struct Cyc_Absyn_Datatypefield*df=_tmp352;struct Cyc_List_List*ts1=_tmp353;struct Cyc_Absyn_Datatypedecl*dd2=_tmp354;struct Cyc_List_List*ts2=_tmp355;
# 1863
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 1865
if(({int _tmp73D=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts1);_tmp73D != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts2);}))return 0;
for(1;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5: if(*((int*)_tmp34E.f2)== 5){_tmp34F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp34E.f1)->f1;_tmp350=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp34E.f2)->f1;{struct Cyc_Absyn_FnInfo f1=_tmp34F;struct Cyc_Absyn_FnInfo f2=_tmp350;
# 1871
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp369=f1.tvars;struct Cyc_List_List*tvs1=_tmp369;
struct Cyc_List_List*_tmp36A=f2.tvars;struct Cyc_List_List*tvs2=_tmp36A;
if(({int _tmp73E=((int(*)(struct Cyc_List_List*))Cyc_List_length)(tvs1);_tmp73E != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp36C=_cycalloc(sizeof(*_tmp36C));({struct _tuple17*_tmp740=({struct _tuple17*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp73F=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp36B->f2=_tmp73F;});_tmp36B;});_tmp36C->hd=_tmp740;}),_tmp36C->tl=inst;_tmp36C;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 1882
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp743=po;struct Cyc_List_List*_tmp742=assume;void*_tmp741=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D->tag=5,_tmp36D->f1=f1;_tmp36D;});Cyc_Tcutil_subtype(_tmp743,_tmp742,_tmp741,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->tag=5,_tmp36E->f1=f2;_tmp36E;}));});}}}
# 1889
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp36F=f1.args;struct Cyc_List_List*args1=_tmp36F;
struct Cyc_List_List*_tmp370=f2.args;struct Cyc_List_List*args2=_tmp370;
# 1894
if(({int _tmp744=((int(*)(struct Cyc_List_List*))Cyc_List_length)(args1);_tmp744 != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(args2);}))return 0;
# 1896
for(1;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp371=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp53=_tmp371;struct _tuple9 _tmp372=_stmttmp53;void*_tmp374;struct Cyc_Absyn_Tqual _tmp373;_tmp373=_tmp372.f2;_tmp374=_tmp372.f3;{struct Cyc_Absyn_Tqual tq1=_tmp373;void*t1=_tmp374;
struct _tuple9 _tmp375=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp54=_tmp375;struct _tuple9 _tmp376=_stmttmp54;void*_tmp378;struct Cyc_Absyn_Tqual _tmp377;_tmp377=_tmp376.f2;_tmp378=_tmp376.f3;{struct Cyc_Absyn_Tqual tq2=_tmp377;void*t2=_tmp378;
# 1900
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 1904
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp379=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp379;
struct Cyc_Absyn_VarargInfo _tmp37A=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp37A;
# 1909
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 1914
if(!({void*_tmp745=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp745,(void*)_check_null(f2.effect));}))return 0;
# 1916
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 1918
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes))return 0;
# 1920
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 1922
return 0;
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 1925
return 0;
# 1927
return 1;}}}else{goto _LL7;}default: _LL7:
 return 0;};}}
# 1938
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1941
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(1;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp37B=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp55=_tmp37B;struct _tuple12*_tmp37C=_stmttmp55;void*_tmp37E;struct Cyc_Absyn_Tqual _tmp37D;_tmp37D=_tmp37C->f1;_tmp37E=_tmp37C->f2;{struct Cyc_Absyn_Tqual tq1=_tmp37D;void*t1a=_tmp37E;
struct _tuple12*_tmp37F=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp56=_tmp37F;struct _tuple12*_tmp380=_stmttmp56;void*_tmp382;struct Cyc_Absyn_Tqual _tmp381;_tmp381=_tmp380->f1;_tmp382=_tmp380->f2;{struct Cyc_Absyn_Tqual tq2=_tmp381;void*t2a=_tmp382;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 1955
return 1;}
# 1960
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
{void*_tmp383=t2;if(*((int*)_tmp383)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp383)->f1)){case 0:
 return Cyc_Absyn_No_coercion;case 1: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp383)->f1)->f2){case Cyc_Absyn_Int_sz:
# 1968
 goto _LL6;case Cyc_Absyn_Long_sz: _LL6:
# 1970
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;}{
# 1974
void*_tmp384=t1;void*_tmp385;struct Cyc_Absyn_Enumdecl*_tmp386;void*_tmp38A;struct Cyc_Absyn_Exp*_tmp389;struct Cyc_Absyn_Tqual _tmp388;void*_tmp387;void*_tmp391;void*_tmp390;void*_tmp38F;void*_tmp38E;void*_tmp38D;struct Cyc_Absyn_Tqual _tmp38C;void*_tmp38B;switch(*((int*)_tmp384)){case 3: _tmp38B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp384)->f1).elt_type;_tmp38C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp384)->f1).elt_tq;_tmp38D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp384)->f1).ptr_atts).rgn;_tmp38E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp384)->f1).ptr_atts).nullable;_tmp38F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp384)->f1).ptr_atts).bounds;_tmp390=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp384)->f1).ptr_atts).zero_term;_tmp391=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp384)->f1).ptr_atts).released;{void*t_a=_tmp38B;struct Cyc_Absyn_Tqual q_a=_tmp38C;void*rt_a=_tmp38D;void*null_a=_tmp38E;void*b_a=_tmp38F;void*zt_a=_tmp390;void*rel_a=_tmp391;
# 1984 "tcutil.cyc"
{void*_tmp392=t2;void*_tmp399;void*_tmp398;void*_tmp397;void*_tmp396;void*_tmp395;struct Cyc_Absyn_Tqual _tmp394;void*_tmp393;if(*((int*)_tmp392)== 3){_tmp393=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).elt_type;_tmp394=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).elt_tq;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).rgn;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).nullable;_tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).bounds;_tmp398=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).zero_term;_tmp399=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).released;{void*t_b=_tmp393;struct Cyc_Absyn_Tqual q_b=_tmp394;void*rt_b=_tmp395;void*null_b=_tmp396;void*b_b=_tmp397;void*zt_b=_tmp398;void*rel_b=_tmp399;
# 1986
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp39A=({struct Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({struct _tuple15*_tmp746=({struct _tuple15*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->f1=t1,_tmp3A1->f2=t2;_tmp3A1;});_tmp3A2->hd=_tmp746;}),_tmp3A2->tl=0;_tmp3A2;});struct Cyc_List_List*assump=_tmp39A;
int _tmp39B=q_b.real_const || !q_a.real_const;int quals_okay=_tmp39B;
# 1999 "tcutil.cyc"
int _tmp39C=
({void*_tmp747=b_b;Cyc_Unify_unify(_tmp747,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 1999
int deep_castable=_tmp39C;
# 2002
int _tmp39D=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2002
int ptrsub=_tmp39D;
# 2005
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
int released_ok=Cyc_Unify_unify(rel_a,rel_b)|| !Cyc_Absyn_type2bool(0,rel_b);
# 2008
int _tmp39E=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2008
int bitcase=_tmp39E;
# 2011
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp39F=({void*_tmp748=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp748,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp39F;
struct Cyc_Absyn_Exp*_tmp3A0=({void*_tmp749=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp749,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3A0;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2020
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2025
if((((bounds_ok && zeroterm_ok)&& released_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{
goto _LL18;}_LL18:;}
# 2031
return Cyc_Absyn_Unknown_coercion;}case 4: _tmp387=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp384)->f1).elt_type;_tmp388=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp384)->f1).tq;_tmp389=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp384)->f1).num_elts;_tmp38A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp384)->f1).zero_term;{void*t1a=_tmp387;struct Cyc_Absyn_Tqual tq1a=_tmp388;struct Cyc_Absyn_Exp*e1=_tmp389;void*zt1=_tmp38A;
# 2033
{void*_tmp3A3=t2;void*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Tqual _tmp3A5;void*_tmp3A4;if(*((int*)_tmp3A3)== 4){_tmp3A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).elt_type;_tmp3A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).tq;_tmp3A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).num_elts;_tmp3A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).zero_term;{void*t2a=_tmp3A4;struct Cyc_Absyn_Tqual tq2a=_tmp3A5;struct Cyc_Absyn_Exp*e2=_tmp3A6;void*zt2=_tmp3A7;
# 2035
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2040
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{
# 2042
return Cyc_Absyn_Unknown_coercion;};}
# 2044
return Cyc_Absyn_Unknown_coercion;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp384)->f1)){case 15: _tmp386=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp384)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed1=_tmp386;
# 2048
{void*_tmp3A8=t2;struct Cyc_Absyn_Enumdecl*_tmp3A9;if(*((int*)_tmp3A8)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->f1)== 15){_tmp3A9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed2=_tmp3A9;
# 2050
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp74A=((int(*)(struct Cyc_List_List*))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp74A >= ((int(*)(struct Cyc_List_List*))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25:
 goto _LL22;}_LL22:;}
# 2056
goto _LL11;}case 1: _LL11:
 goto _LL13;case 2: _LL13:
# 2059
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp384)->f2 != 0){_tmp385=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp384)->f2)->hd;{void*r1=_tmp385;
# 2062
{void*_tmp3AA=t2;void*_tmp3AB;if(*((int*)_tmp3AA)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f1)== 3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f2 != 0){_tmp3AB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f2)->hd;{void*r2=_tmp3AB;
# 2064
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A:
 goto _LL27;}_LL27:;}
# 2068
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16:
 return Cyc_Absyn_Unknown_coercion;};}}
# 2074
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3AC=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3AC;
({void*_tmp74B=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD->tag=14,_tmp3AD->f1=t,_tmp3AD->f2=inner,_tmp3AD->f3=0,_tmp3AD->f4=c;_tmp3AD;});e->r=_tmp74B;});
e->topt=t;}}
# 2083
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2088
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
return({struct Cyc_Absyn_Tvar*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({struct _fat_ptr*_tmp74E=({struct _fat_ptr*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));({struct _fat_ptr _tmp74D=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B0=({struct Cyc_Int_pa_PrintArg_struct _tmp632;_tmp632.tag=1,_tmp632.f1=(unsigned long)Cyc_Tcutil_tvar_counter ++;_tmp632;});void*_tmp3AE[1];_tmp3AE[0]=& _tmp3B0;({struct _fat_ptr _tmp74C=({const char*_tmp3AF="#%d";_tag_fat(_tmp3AF,sizeof(char),4U);});Cyc_aprintf(_tmp74C,_tag_fat(_tmp3AE,sizeof(void*),1));});});*_tmp3B1=_tmp74D;});_tmp3B1;});_tmp3B2->name=_tmp74E;}),_tmp3B2->identity=- 1,_tmp3B2->kind=k;_tmp3B2;});}
# 2092
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
return(int)*((const char*)_check_fat_subscript((t->name)[0],sizeof(char),0U))== (int)'#';}
# 2097
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2104
struct Cyc_List_List*_tmp3B3=0;struct Cyc_List_List*fn_type_atts=_tmp3B3;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Atts_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->hd=(void*)atts->hd,_tmp3B4->tl=fn_type_atts;_tmp3B4;});}}{
struct Cyc_Absyn_FnInfo _tmp3B5=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3B5;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->tag=5,_tmp3B6->f1=type_info;_tmp3B6;});}}
# 2112
return(void*)_check_null(fd->cached_type);}
# 2118
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2120
union Cyc_Relations_RelnOp _tmp3B7=*rop;union Cyc_Relations_RelnOp _stmttmp57=_tmp3B7;union Cyc_Relations_RelnOp _tmp3B8=_stmttmp57;struct Cyc_Absyn_Vardecl*_tmp3B9;struct Cyc_Absyn_Vardecl*_tmp3BA;switch((_tmp3B8.RNumelts).tag){case 2: _tmp3BA=(_tmp3B8.RVar).val;{struct Cyc_Absyn_Vardecl*vd=_tmp3BA;
# 2122
struct _tuple1 _tmp3BB=*vd->name;struct _tuple1 _stmttmp58=_tmp3BB;struct _tuple1 _tmp3BC=_stmttmp58;struct _fat_ptr*_tmp3BE;union Cyc_Absyn_Nmspace _tmp3BD;_tmp3BD=_tmp3BC.f1;_tmp3BE=_tmp3BC.f2;{union Cyc_Absyn_Nmspace nmspace=_tmp3BD;struct _fat_ptr*var=_tmp3BE;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp74F=(struct _fat_ptr)*var;Cyc_strcmp(_tmp74F,({const char*_tmp3BF="return_value";_tag_fat(_tmp3BF,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp750=Cyc_Relations_RReturn();*rop=_tmp750;});
goto _LL0;}{
# 2128
unsigned c=0U;
{struct Cyc_List_List*_tmp3C0=args;struct Cyc_List_List*a=_tmp3C0;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3C1=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp59=_tmp3C1;struct _tuple9*_tmp3C2=_stmttmp59;struct _fat_ptr*_tmp3C3;_tmp3C3=_tmp3C2->f1;{struct _fat_ptr*vopt=_tmp3C3;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp751=Cyc_Relations_RParam(c);*rop=_tmp751;});
break;}}}}
# 2136
goto _LL0;}}}case 3: _tmp3B9=(_tmp3B8.RNumelts).val;{struct Cyc_Absyn_Vardecl*vd=_tmp3B9;
# 2138
struct _tuple1 _tmp3C4=*vd->name;struct _tuple1 _stmttmp5A=_tmp3C4;struct _tuple1 _tmp3C5=_stmttmp5A;struct _fat_ptr*_tmp3C7;union Cyc_Absyn_Nmspace _tmp3C6;_tmp3C6=_tmp3C5.f1;_tmp3C7=_tmp3C5.f2;{union Cyc_Absyn_Nmspace nmspace=_tmp3C6;struct _fat_ptr*var=_tmp3C7;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3C8=args;struct Cyc_List_List*a=_tmp3C8;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3C9=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5B=_tmp3C9;struct _tuple9*_tmp3CA=_stmttmp5B;struct _fat_ptr*_tmp3CB;_tmp3CB=_tmp3CA->f1;{struct _fat_ptr*vopt=_tmp3CB;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp752=Cyc_Relations_RParamNumelts(c);*rop=_tmp752;});
break;}}}}
# 2148
goto _LL0;}}}default:
 goto _LL0;}_LL0:;}
# 2153
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2155
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2159
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2161
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3CC=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3CC;
((void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2168
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3CD=pr;void*_tmp3CF;struct Cyc_Absyn_Tqual _tmp3CE;_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;{struct Cyc_Absyn_Tqual tq=_tmp3CE;void*t2=_tmp3CF;
return t2 == t?pr:({struct _tuple12*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->f1=tq,_tmp3D0->f2=t;_tmp3D0;});}}struct _tuple24{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple25{struct _tuple24*f1;void*f2;};
# 2172
static struct _tuple25*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2174
return({struct _tuple25*_tmp3D2=_region_malloc(rgn,sizeof(*_tmp3D2));({struct _tuple24*_tmp753=({struct _tuple24*_tmp3D1=_region_malloc(rgn,sizeof(*_tmp3D1));_tmp3D1->f1=(*y).f1,_tmp3D1->f2=(*y).f2;_tmp3D1;});_tmp3D2->f1=_tmp753;}),_tmp3D2->f2=(*y).f3;_tmp3D2;});}
# 2176
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2178
struct _tuple9 _tmp3D3=*orig_arg;struct _tuple9 _stmttmp5C=_tmp3D3;struct _tuple9 _tmp3D4=_stmttmp5C;void*_tmp3D7;struct Cyc_Absyn_Tqual _tmp3D6;struct _fat_ptr*_tmp3D5;_tmp3D5=_tmp3D4.f1;_tmp3D6=_tmp3D4.f2;_tmp3D7=_tmp3D4.f3;{struct _fat_ptr*vopt_orig=_tmp3D5;struct Cyc_Absyn_Tqual tq_orig=_tmp3D6;void*t_orig=_tmp3D7;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->f1=vopt_orig,_tmp3D8->f2=tq_orig,_tmp3D8->f3=t;_tmp3D8;});}}
# 2182
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2186
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2188
return({struct Cyc_Absyn_Exp*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->topt=old->topt,_tmp3D9->r=r,_tmp3D9->loc=old->loc,_tmp3D9->annot=old->annot;_tmp3D9;});}
# 2193
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3DA=e->r;void*_stmttmp5D=_tmp3DA;void*_tmp3DB=_stmttmp5D;void*_tmp3DC;struct Cyc_List_List*_tmp3DE;void*_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;void*_tmp3E1;enum Cyc_Absyn_Coercion _tmp3E5;int _tmp3E4;struct Cyc_Absyn_Exp*_tmp3E3;void*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3EB;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EE;struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_Absyn_Exp*_tmp3EC;struct Cyc_List_List*_tmp3F0;enum Cyc_Absyn_Primop _tmp3EF;switch(*((int*)_tmp3DB)){case 0:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 33: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 1: _LLA:
 return e;case 3: _tmp3EF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_tmp3F0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3DB)->f2;{enum Cyc_Absyn_Primop p=_tmp3EF;struct Cyc_List_List*es=_tmp3F0;
# 2202
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp3F1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp3F1;
struct Cyc_Absyn_Exp*_tmp3F2=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp3F2;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp755=e;Cyc_Tcutil_copye(_tmp755,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->tag=3,_tmp3F4->f1=p,({struct Cyc_List_List*_tmp754=({struct Cyc_Absyn_Exp*_tmp3F3[1];_tmp3F3[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3F3,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp3F4->f2=_tmp754;});_tmp3F4;}));});}else{
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp3F5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp3F5;
struct Cyc_Absyn_Exp*_tmp3F6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp3F6;
struct Cyc_Absyn_Exp*_tmp3F7=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp3F7;
struct Cyc_Absyn_Exp*_tmp3F8=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp3F8;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp757=e;Cyc_Tcutil_copye(_tmp757,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->tag=3,_tmp3FA->f1=p,({struct Cyc_List_List*_tmp756=({struct Cyc_Absyn_Exp*_tmp3F9[2];_tmp3F9[0]=new_e1,_tmp3F9[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3F9,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp3FA->f2=_tmp756;});_tmp3FA;}));});}}
# 2214
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0,({struct _fat_ptr _tmp758=({const char*_tmp3FD="primop does not have 1 or 2 args!";_tag_fat(_tmp3FD,sizeof(char),34U);});_tmp633.f1=_tmp758;});_tmp633;});void*_tmp3FB[1];_tmp3FB[0]=& _tmp3FC;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3FB,sizeof(void*),1));});}case 6: _tmp3EC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_tmp3ED=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3DB)->f2;_tmp3EE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3DB)->f3;{struct Cyc_Absyn_Exp*e1=_tmp3EC;struct Cyc_Absyn_Exp*e2=_tmp3ED;struct Cyc_Absyn_Exp*e3=_tmp3EE;
# 2216
struct Cyc_Absyn_Exp*_tmp3FE=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp3FE;
struct Cyc_Absyn_Exp*_tmp3FF=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp3FF;
struct Cyc_Absyn_Exp*_tmp400=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp400;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp759=e;Cyc_Tcutil_copye(_tmp759,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->tag=6,_tmp401->f1=new_e1,_tmp401->f2=new_e2,_tmp401->f3=new_e3;_tmp401;}));});}case 7: _tmp3EA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_tmp3EB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3DB)->f2;{struct Cyc_Absyn_Exp*e1=_tmp3EA;struct Cyc_Absyn_Exp*e2=_tmp3EB;
# 2222
struct Cyc_Absyn_Exp*_tmp402=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp402;
struct Cyc_Absyn_Exp*_tmp403=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp403;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp75A=e;Cyc_Tcutil_copye(_tmp75A,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404->tag=7,_tmp404->f1=new_e1,_tmp404->f2=new_e2;_tmp404;}));});}case 8: _tmp3E8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_tmp3E9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3DB)->f2;{struct Cyc_Absyn_Exp*e1=_tmp3E8;struct Cyc_Absyn_Exp*e2=_tmp3E9;
# 2227
struct Cyc_Absyn_Exp*_tmp405=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp405;
struct Cyc_Absyn_Exp*_tmp406=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp406;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp75B=e;Cyc_Tcutil_copye(_tmp75B,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->tag=8,_tmp407->f1=new_e1,_tmp407->f2=new_e2;_tmp407;}));});}case 9: _tmp3E6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_tmp3E7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3DB)->f2;{struct Cyc_Absyn_Exp*e1=_tmp3E6;struct Cyc_Absyn_Exp*e2=_tmp3E7;
# 2232
struct Cyc_Absyn_Exp*_tmp408=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp408;
struct Cyc_Absyn_Exp*_tmp409=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp409;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp75C=e;Cyc_Tcutil_copye(_tmp75C,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->tag=9,_tmp40A->f1=new_e1,_tmp40A->f2=new_e2;_tmp40A;}));});}case 14: _tmp3E2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_tmp3E3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DB)->f2;_tmp3E4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DB)->f3;_tmp3E5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DB)->f4;{void*t=_tmp3E2;struct Cyc_Absyn_Exp*e1=_tmp3E3;int b=_tmp3E4;enum Cyc_Absyn_Coercion c=_tmp3E5;
# 2237
struct Cyc_Absyn_Exp*_tmp40B=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40B;
void*_tmp40C=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp40C;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp75D=e;Cyc_Tcutil_copye(_tmp75D,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->tag=14,_tmp40D->f1=new_typ,_tmp40D->f2=new_e1,_tmp40D->f3=b,_tmp40D->f4=c;_tmp40D;}));});}case 17: _tmp3E1=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;{void*t=_tmp3E1;
# 2242
void*_tmp40E=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp40E;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp75E=e;Cyc_Tcutil_copye(_tmp75E,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->tag=17,_tmp40F->f1=new_typ;_tmp40F;}));});}case 18: _tmp3E0=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;{struct Cyc_Absyn_Exp*e1=_tmp3E0;
# 2245
struct Cyc_Absyn_Exp*_tmp410=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp410;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp75F=e;Cyc_Tcutil_copye(_tmp75F,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->tag=18,_tmp411->f1=new_e1;_tmp411;}));});}case 41: _tmp3DF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;{struct Cyc_Absyn_Exp*e1=_tmp3DF;
# 2248
struct Cyc_Absyn_Exp*_tmp412=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp412;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp760=e;Cyc_Tcutil_copye(_tmp760,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->tag=41,_tmp413->f1=new_e1;_tmp413;}));});}case 19: _tmp3DD=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_tmp3DE=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3DB)->f2;{void*t=_tmp3DD;struct Cyc_List_List*f=_tmp3DE;
# 2251
void*_tmp414=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp414;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp761=e;Cyc_Tcutil_copye(_tmp761,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->tag=19,_tmp415->f1=new_typ,_tmp415->f2=f;_tmp415;}));});}case 39: _tmp3DC=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;{void*t=_tmp3DC;
# 2254
void*_tmp416=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp416;
if(new_typ == t)return e;{
# 2257
void*_tmp417=Cyc_Absyn_compress(new_typ);void*_stmttmp5E=_tmp417;void*_tmp418=_stmttmp5E;struct Cyc_Absyn_Exp*_tmp419;if(*((int*)_tmp418)== 9){_tmp419=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp418)->f1;{struct Cyc_Absyn_Exp*e=_tmp419;
return e;}}else{
return({struct Cyc_Absyn_Exp*_tmp762=e;Cyc_Tcutil_copye(_tmp762,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->tag=39,_tmp41A->f1=new_typ;_tmp41A;}));});};}}default:
# 2261
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp41C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp634;_tmp634.tag=0,({struct _fat_ptr _tmp763=({const char*_tmp41D="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp41D,sizeof(char),46U);});_tmp634.f1=_tmp763;});_tmp634;});void*_tmp41B[1];_tmp41B[0]=& _tmp41C;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp41B,sizeof(void*),1));});};}
# 2265
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2268
return e == 0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2271
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2274
void*_tmp41E=f->type;void*t=_tmp41E;
struct Cyc_Absyn_Exp*_tmp41F=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp41F;
void*_tmp420=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp420;
struct Cyc_Absyn_Exp*_tmp421=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp421;
if(t == new_typ && req == new_req)
return f;{
struct Cyc_Absyn_Aggrfield*_tmp422=({struct Cyc_Absyn_Aggrfield*_tmp423=_cycalloc(sizeof(*_tmp423));*_tmp423=*f;_tmp423;});struct Cyc_Absyn_Aggrfield*ans=_tmp422;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2286
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2289
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp424=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp424;
struct Cyc_List_List*_tmp425=fs->tl;struct Cyc_List_List*t=_tmp425;
struct Cyc_Absyn_Aggrfield*_tmp426=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp426;
struct Cyc_List_List*_tmp427=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_typ=_tmp427;
return new_f == f && new_typ == t?fs:({struct Cyc_List_List*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->hd=new_f,_tmp428->tl=new_typ;_tmp428;});}}
# 2297
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2300
struct _tuple0 _tmp429=((struct _tuple0(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp5F=_tmp429;struct _tuple0 _tmp42A=_stmttmp5F;struct Cyc_List_List*_tmp42C;struct Cyc_List_List*_tmp42B;_tmp42B=_tmp42A.f1;_tmp42C=_tmp42A.f2;{struct Cyc_List_List*rpo1a=_tmp42B;struct Cyc_List_List*rpo1b=_tmp42C;
struct Cyc_List_List*_tmp42D=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp42D;
struct Cyc_List_List*_tmp42E=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp42E;
return rpo2a == rpo1a && rpo2b == rpo1b?rgn_po:((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip)(rpo2a,rpo2b);}}
# 2308
static int Cyc_Tcutil_skip_fntypes=0;
# 2310
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2313
void*_tmp42F=Cyc_Absyn_compress(t);void*_stmttmp60=_tmp42F;void*_tmp430=_stmttmp60;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_List_List*_tmp434;void*_tmp433;void*_tmp435;struct Cyc_List_List*_tmp437;enum Cyc_Absyn_AggrKind _tmp436;struct Cyc_List_List*_tmp438;struct Cyc_Absyn_Vardecl*_tmp444;struct Cyc_Absyn_Exp*_tmp443;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_List_List*_tmp441;struct Cyc_List_List*_tmp440;struct Cyc_Absyn_VarargInfo*_tmp43F;int _tmp43E;struct Cyc_List_List*_tmp43D;void*_tmp43C;struct Cyc_Absyn_Tqual _tmp43B;void*_tmp43A;struct Cyc_List_List*_tmp439;void*_tmp44B;void*_tmp44A;void*_tmp449;void*_tmp448;void*_tmp447;struct Cyc_Absyn_Tqual _tmp446;void*_tmp445;unsigned _tmp450;void*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Tqual _tmp44D;void*_tmp44C;void*_tmp454;struct Cyc_Absyn_Typedefdecl*_tmp453;struct Cyc_List_List*_tmp452;struct _tuple1*_tmp451;struct Cyc_Absyn_Tvar*_tmp455;switch(*((int*)_tmp430)){case 2: _tmp455=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp430)->f1;{struct Cyc_Absyn_Tvar*v=_tmp455;
# 2316
struct _handler_cons _tmp456;_push_handler(& _tmp456);{int _tmp458=0;if(setjmp(_tmp456.handler))_tmp458=1;if(!_tmp458){{void*_tmp459=((void*(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0);return _tmp459;};_pop_handler();}else{void*_tmp457=(void*)Cyc_Core_get_exn_thrown();void*_tmp45A=_tmp457;void*_tmp45B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp45A)->tag == Cyc_Core_Not_found)
return t;else{_tmp45B=_tmp45A;{void*exn=_tmp45B;(int)_rethrow(exn);}};}}}case 8: _tmp451=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp430)->f1;_tmp452=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp430)->f2;_tmp453=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp430)->f3;_tmp454=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp430)->f4;{struct _tuple1*n=_tmp451;struct Cyc_List_List*ts=_tmp452;struct Cyc_Absyn_Typedefdecl*td=_tmp453;void*topt=_tmp454;
# 2319
struct Cyc_List_List*_tmp45C=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp45C;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->tag=8,_tmp45D->f1=n,_tmp45D->f2=new_ts,_tmp45D->f3=td,_tmp45D->f4=topt;_tmp45D;});}case 4: _tmp44C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp430)->f1).elt_type;_tmp44D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp430)->f1).tq;_tmp44E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp430)->f1).num_elts;_tmp44F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp430)->f1).zero_term;_tmp450=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp430)->f1).zt_loc;{void*t1=_tmp44C;struct Cyc_Absyn_Tqual tq=_tmp44D;struct Cyc_Absyn_Exp*e=_tmp44E;void*zt=_tmp44F;unsigned ztl=_tmp450;
# 2322
void*_tmp45E=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp45E;
struct Cyc_Absyn_Exp*_tmp45F=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp45F;
void*_tmp460=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp460;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp461=_cycalloc(sizeof(*_tmp461));
_tmp461->tag=4,(_tmp461->f1).elt_type=new_t1,(_tmp461->f1).tq=tq,(_tmp461->f1).num_elts=new_e,(_tmp461->f1).zero_term=new_zt,(_tmp461->f1).zt_loc=ztl;_tmp461;});}case 3: _tmp445=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).elt_type;_tmp446=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).elt_tq;_tmp447=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).ptr_atts).rgn;_tmp448=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).ptr_atts).nullable;_tmp449=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).ptr_atts).bounds;_tmp44A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).ptr_atts).zero_term;_tmp44B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).ptr_atts).released;{void*t1=_tmp445;struct Cyc_Absyn_Tqual tq=_tmp446;void*r=_tmp447;void*n=_tmp448;void*b=_tmp449;void*zt=_tmp44A;void*rel=_tmp44B;
# 2328
void*_tmp462=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp462;
void*_tmp463=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp463;
void*_tmp464=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp464;
void*_tmp465=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp465;
void*_tmp466=Cyc_Tcutil_rsubstitute(rgn,inst,rel);void*new_rel=_tmp466;
if((((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)&& new_rel == rel)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp635;_tmp635.elt_type=new_t1,_tmp635.elt_tq=tq,(_tmp635.ptr_atts).rgn=new_r,(_tmp635.ptr_atts).nullable=n,(_tmp635.ptr_atts).bounds=new_b,(_tmp635.ptr_atts).zero_term=new_zt,(_tmp635.ptr_atts).ptrloc=0,(_tmp635.ptr_atts).released=new_rel;_tmp635;}));}case 5: _tmp439=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).tvars;_tmp43A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).effect;_tmp43B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).ret_tqual;_tmp43C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).ret_type;_tmp43D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).args;_tmp43E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).c_varargs;_tmp43F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).cyc_varargs;_tmp440=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).rgn_po;_tmp441=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).attributes;_tmp442=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).requires_clause;_tmp443=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).ensures_clause;_tmp444=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp430)->f1).return_value;{struct Cyc_List_List*vs=_tmp439;void*eff=_tmp43A;struct Cyc_Absyn_Tqual rtq=_tmp43B;void*rtyp=_tmp43C;struct Cyc_List_List*args=_tmp43D;int c_varargs=_tmp43E;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp43F;struct Cyc_List_List*rgn_po=_tmp440;struct Cyc_List_List*atts=_tmp441;struct Cyc_Absyn_Exp*req=_tmp442;struct Cyc_Absyn_Exp*ens=_tmp443;struct Cyc_Absyn_Vardecl*ret_value=_tmp444;
# 2338
if(Cyc_Tcutil_skip_fntypes)return t;
# 2340
{struct Cyc_List_List*_tmp467=vs;struct Cyc_List_List*p=_tmp467;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp469=_region_malloc(rgn,sizeof(*_tmp469));({struct _tuple17*_tmp765=({struct _tuple17*_tmp468=_region_malloc(rgn,sizeof(*_tmp468));_tmp468->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp764=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp468->f2=_tmp764;});_tmp468;});_tmp469->hd=_tmp765;}),_tmp469->tl=inst;_tmp469;});}}{
struct _tuple0 _tmp46A=({struct _RegionHandle*_tmp767=rgn;struct _RegionHandle*_tmp766=rgn;((struct _tuple0(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rsplit)(_tmp767,_tmp766,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple25*(*)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2342
struct _tuple0 _stmttmp61=_tmp46A;struct _tuple0 _tmp46B=_stmttmp61;struct Cyc_List_List*_tmp46D;struct Cyc_List_List*_tmp46C;_tmp46C=_tmp46B.f1;_tmp46D=_tmp46B.f2;{struct Cyc_List_List*qs=_tmp46C;struct Cyc_List_List*ts=_tmp46D;
# 2344
struct Cyc_List_List*_tmp46E=args;struct Cyc_List_List*args2=_tmp46E;
struct Cyc_List_List*_tmp46F=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp46F;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff == 0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2350
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2354
struct Cyc_Absyn_VarargInfo _tmp470=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp62=_tmp470;struct Cyc_Absyn_VarargInfo _tmp471=_stmttmp62;int _tmp475;void*_tmp474;struct Cyc_Absyn_Tqual _tmp473;struct _fat_ptr*_tmp472;_tmp472=_tmp471.name;_tmp473=_tmp471.tq;_tmp474=_tmp471.type;_tmp475=_tmp471.inject;{struct _fat_ptr*n=_tmp472;struct Cyc_Absyn_Tqual tq=_tmp473;void*t=_tmp474;int i=_tmp475;
void*_tmp476=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp476;
cyc_varargs2=t2 == t?cyc_varargs:({struct Cyc_Absyn_VarargInfo*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->name=n,_tmp477->tq=tq,_tmp477->type=t2,_tmp477->inject=i;_tmp477;});}}{
# 2358
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp478=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp478;
struct Cyc_List_List*_tmp479=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp479;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->tag=5,(_tmp47A->f1).tvars=vs,(_tmp47A->f1).effect=eff2,(_tmp47A->f1).ret_tqual=rtq,({void*_tmp768=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp47A->f1).ret_type=_tmp768;}),(_tmp47A->f1).args=args2,(_tmp47A->f1).c_varargs=c_varargs,(_tmp47A->f1).cyc_varargs=cyc_varargs2,(_tmp47A->f1).rgn_po=rgn_po2,(_tmp47A->f1).attributes=atts,(_tmp47A->f1).requires_clause=req2,(_tmp47A->f1).requires_relns=req_relns2,(_tmp47A->f1).ensures_clause=ens2,(_tmp47A->f1).ensures_relns=ens_relns2,(_tmp47A->f1).return_value=ret_value;_tmp47A;});}}}}}case 6: _tmp438=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp430)->f1;{struct Cyc_List_List*tqts=_tmp438;
# 2367
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp47B=tqts;struct Cyc_List_List*ts1=_tmp47B;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp47C=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp47C;
void*_tmp47D=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp47D;
if(t1 != t2)
change=1;
ts2=({struct Cyc_List_List*_tmp47E=_region_malloc(rgn,sizeof(*_tmp47E));_tmp47E->hd=t2,_tmp47E->tl=ts2;_tmp47E;});}}
# 2376
return !change?t:(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->tag=6,({struct Cyc_List_List*_tmp76A=({struct Cyc_List_List*_tmp769=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp769,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ts2));});_tmp47F->f1=_tmp76A;});_tmp47F;});}case 7: _tmp436=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp430)->f1;_tmp437=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp430)->f2;{enum Cyc_Absyn_AggrKind k=_tmp436;struct Cyc_List_List*fs=_tmp437;
# 2378
struct Cyc_List_List*_tmp480=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp480;
return fs == new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->tag=7,_tmp481->f1=k,_tmp481->f2=new_fs;_tmp481;});}case 1: _tmp435=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp430)->f2;{void*r=_tmp435;
# 2381
return r == 0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0: _tmp433=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp430)->f1;_tmp434=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp430)->f2;{void*c=_tmp433;struct Cyc_List_List*ts=_tmp434;
# 2383
struct Cyc_List_List*_tmp482=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp482;
return ts == new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->tag=0,_tmp483->f1=c,_tmp483->f2=new_ts;_tmp483;});}case 9: _tmp432=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp430)->f1;{struct Cyc_Absyn_Exp*e=_tmp432;
# 2386
struct Cyc_Absyn_Exp*_tmp484=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp484;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->tag=9,_tmp485->f1=new_e;_tmp485;});}case 11: _tmp431=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp430)->f1;{struct Cyc_Absyn_Exp*e=_tmp431;
# 2389
struct Cyc_Absyn_Exp*_tmp486=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp486;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487->tag=11,_tmp487->f1=new_e;_tmp487;});}default:
# 2392
 if(Cyc_Tcutil_skip_fntypes)return t;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp489=({struct Cyc_Warn_String_Warn_Warg_struct _tmp636;_tmp636.tag=0,({struct _fat_ptr _tmp76B=({const char*_tmp48A="found typedecltype in rsubs";_tag_fat(_tmp48A,sizeof(char),28U);});_tmp636.f1=_tmp76B;});_tmp636;});void*_tmp488[1];_tmp488[0]=& _tmp489;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp488,sizeof(void*),1));});};}
# 2397
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2400
if(ts == 0)
return 0;{
void*_tmp48B=(void*)ts->hd;void*old_hd=_tmp48B;
struct Cyc_List_List*_tmp48C=ts->tl;struct Cyc_List_List*old_tl=_tmp48C;
void*_tmp48D=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp48D;
struct Cyc_List_List*_tmp48E=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp48E;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F->hd=new_hd,_tmp48F->tl=new_tl;_tmp48F;});}}
# 2411
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return inst == 0?t: Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}
# 2415
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t){
Cyc_Tcutil_skip_fntypes=1;{
void*_tmp490=Cyc_Tcutil_substitute(inst,t);void*new_typ=_tmp490;
Cyc_Tcutil_skip_fntypes=0;
return new_typ;}}
# 2423
struct _tuple17*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp491=Cyc_Kinds_kind_to_opt(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk));struct Cyc_Core_Opt*k=_tmp491;
return({struct _tuple17*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->f1=tv,({void*_tmp76D=({struct Cyc_Core_Opt*_tmp76C=k;Cyc_Absyn_new_evar(_tmp76C,({struct Cyc_Core_Opt*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->v=s;_tmp492;}));});_tmp493->f2=_tmp76D;});_tmp493;});}struct _tuple26{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2428
struct _tuple17*Cyc_Tcutil_r_make_inst_var(struct _tuple26*env,struct Cyc_Absyn_Tvar*tv){
# 2430
struct _tuple26*_tmp494=env;struct _RegionHandle*_tmp496;struct Cyc_List_List*_tmp495;_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;{struct Cyc_List_List*s=_tmp495;struct _RegionHandle*rgn=_tmp496;
struct Cyc_Core_Opt*_tmp497=Cyc_Kinds_kind_to_opt(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk));struct Cyc_Core_Opt*k=_tmp497;
return({struct _tuple17*_tmp499=_region_malloc(rgn,sizeof(*_tmp499));_tmp499->f1=tv,({void*_tmp76F=({struct Cyc_Core_Opt*_tmp76E=k;Cyc_Absyn_new_evar(_tmp76E,({struct Cyc_Core_Opt*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->v=s;_tmp498;}));});_tmp499->f2=_tmp76F;});_tmp499;});}}
# 2435
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2437
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp49B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp639;_tmp639.tag=0,({struct _fat_ptr _tmp770=({const char*_tmp49F="bitfield ";_tag_fat(_tmp49F,sizeof(char),10U);});_tmp639.f1=_tmp770;});_tmp639;});struct Cyc_Warn_String_Warn_Warg_struct _tmp49C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp638;_tmp638.tag=0,_tmp638.f1=*fn;_tmp638;});struct Cyc_Warn_String_Warn_Warg_struct _tmp49D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp637;_tmp637.tag=0,({struct _fat_ptr _tmp771=({const char*_tmp49E=" does not have constant width";_tag_fat(_tmp49E,sizeof(char),30U);});_tmp637.f1=_tmp771;});_tmp637;});void*_tmp49A[3];_tmp49A[0]=& _tmp49B,_tmp49A[1]=& _tmp49C,_tmp49A[2]=& _tmp49D;({unsigned _tmp772=loc;Cyc_Warn_err2(_tmp772,_tag_fat(_tmp49A,sizeof(void*),3));});});else{
# 2442
struct _tuple13 _tmp4A0=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp63=_tmp4A0;struct _tuple13 _tmp4A1=_stmttmp63;int _tmp4A3;unsigned _tmp4A2;_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;{unsigned i=_tmp4A2;int known=_tmp4A3;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A;_tmp63A.tag=0,({struct _fat_ptr _tmp773=({const char*_tmp4A6="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4A6,sizeof(char),47U);});_tmp63A.f1=_tmp773;});_tmp63A;});void*_tmp4A4[1];_tmp4A4[0]=& _tmp4A5;({unsigned _tmp774=loc;Cyc_Warn_warn2(_tmp774,_tag_fat(_tmp4A4,sizeof(void*),1));});});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63B;_tmp63B.tag=0,({struct _fat_ptr _tmp775=({const char*_tmp4A9="bitfield has negative width";_tag_fat(_tmp4A9,sizeof(char),28U);});_tmp63B.f1=_tmp775;});_tmp63B;});void*_tmp4A7[1];_tmp4A7[0]=& _tmp4A8;({unsigned _tmp776=loc;Cyc_Warn_err2(_tmp776,_tag_fat(_tmp4A7,sizeof(void*),1));});});
w=i;}}{
# 2449
void*_tmp4AA=Cyc_Absyn_compress(field_type);void*_stmttmp64=_tmp4AA;void*_tmp4AB=_stmttmp64;enum Cyc_Absyn_Size_of _tmp4AC;if(*((int*)_tmp4AB)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->f1)== 1){_tmp4AC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->f1)->f2;{enum Cyc_Absyn_Size_of b=_tmp4AC;
# 2452
int bad;
{enum Cyc_Absyn_Size_of _tmp4AD=b;switch(_tmp4AD){case Cyc_Absyn_Char_sz:
 bad=w > (unsigned)8;goto _LL8;case Cyc_Absyn_Short_sz:
 bad=w > (unsigned)16;goto _LL8;case Cyc_Absyn_Long_sz:
 goto _LL10;case Cyc_Absyn_Int_sz: _LL10:
 bad=w > (unsigned)32;goto _LL8;case Cyc_Absyn_LongLong_sz:
 goto _LL14;default: _LL14:
 bad=w > (unsigned)64;goto _LL8;}_LL8:;}
# 2461
if(bad)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63C;_tmp63C.tag=0,({struct _fat_ptr _tmp777=({const char*_tmp4B0="bitfield larger than type";_tag_fat(_tmp4B0,sizeof(char),26U);});_tmp63C.f1=_tmp777;});_tmp63C;});void*_tmp4AE[1];_tmp4AE[0]=& _tmp4AF;({unsigned _tmp778=loc;Cyc_Warn_warn2(_tmp778,_tag_fat(_tmp4AE,sizeof(void*),1));});});
goto _LL3;}}else{goto _LL6;}}else{_LL6:
# 2465
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp640;_tmp640.tag=0,({struct _fat_ptr _tmp779=({const char*_tmp4B7="bitfield ";_tag_fat(_tmp4B7,sizeof(char),10U);});_tmp640.f1=_tmp779;});_tmp640;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4B3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63F;_tmp63F.tag=0,_tmp63F.f1=*fn;_tmp63F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63E;_tmp63E.tag=0,({struct _fat_ptr _tmp77A=({const char*_tmp4B6=" must have integral type but has type ";_tag_fat(_tmp4B6,sizeof(char),39U);});_tmp63E.f1=_tmp77A;});_tmp63E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4B5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63D;_tmp63D.tag=2,_tmp63D.f1=(void*)field_type;_tmp63D;});void*_tmp4B1[4];_tmp4B1[0]=& _tmp4B2,_tmp4B1[1]=& _tmp4B3,_tmp4B1[2]=& _tmp4B4,_tmp4B1[3]=& _tmp4B5;({unsigned _tmp77B=loc;Cyc_Warn_err2(_tmp77B,_tag_fat(_tmp4B1,sizeof(void*),4));});});
# 2467
goto _LL3;}_LL3:;}}}
# 2474
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4B8=t;void*_tmp4BA;struct Cyc_Absyn_Typedefdecl*_tmp4B9;if(*((int*)_tmp4B8)== 8){_tmp4B9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B8)->f3;_tmp4BA=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B8)->f4;{struct Cyc_Absyn_Typedefdecl*td=_tmp4B9;void*tdopt=_tmp4BA;
# 2477
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp641;_tmp641.tag=0,({struct _fat_ptr _tmp77C=({const char*_tmp4BD="extra const";_tag_fat(_tmp4BD,sizeof(char),12U);});_tmp641.f1=_tmp77C;});_tmp641;});void*_tmp4BB[1];_tmp4BB[0]=& _tmp4BC;({unsigned _tmp77D=loc;Cyc_Warn_warn2(_tmp77D,_tag_fat(_tmp4BB,sizeof(void*),1));});});
return 1;}
# 2482
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);
return declared_const;}}else{
return declared_const;};}
# 2489
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp77E=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp77E;});}
# 2493
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2499
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2502
for(1;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0,_tmp644.f1=msg;_tmp644;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp643;_tmp643.tag=0,({struct _fat_ptr _tmp77F=({const char*_tmp4C2=": ";_tag_fat(_tmp4C2,sizeof(char),3U);});_tmp643.f1=_tmp77F;});_tmp643;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp642;_tmp642.tag=0,({struct _fat_ptr _tmp780=a2string(vs->hd);_tmp642.f1=_tmp780;});_tmp642;});void*_tmp4BE[3];_tmp4BE[0]=& _tmp4BF,_tmp4BE[1]=& _tmp4C0,_tmp4BE[2]=& _tmp4C1;({unsigned _tmp781=loc;Cyc_Warn_err2(_tmp781,_tag_fat(_tmp4BE,sizeof(void*),3));});});}}}
# 2507
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2510
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,unsigned,struct _fat_ptr(*)(struct _fat_ptr*),struct _fat_ptr))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2513
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp783=tvs;unsigned _tmp782=loc;((void(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,unsigned,struct _fat_ptr(*)(struct Cyc_Absyn_Tvar*),struct _fat_ptr))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp783,_tmp782,Cyc_Absynpp_tvar2string,({const char*_tmp4C3="duplicate type variable";_tag_fat(_tmp4C3,sizeof(char),24U);}));});}struct _tuple27{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple28{struct Cyc_List_List*f1;void*f2;};struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2528 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2533
struct _RegionHandle _tmp4C4=_new_region("temp");struct _RegionHandle*temp=& _tmp4C4;_push_region(temp);
# 2537
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp784=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp784,({const char*_tmp4C5="";_tag_fat(_tmp4C5,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4C7=_region_malloc(temp,sizeof(*_tmp4C7));({struct _tuple27*_tmp785=({struct _tuple27*_tmp4C6=_region_malloc(temp,sizeof(*_tmp4C6));_tmp4C6->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4C6->f2=0;_tmp4C6;});_tmp4C7->hd=_tmp785;}),_tmp4C7->tl=fields;_tmp4C7;});}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(fields);{
# 2543
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp4F3="struct";_tag_fat(_tmp4F3,sizeof(char),7U);}):({const char*_tmp4F4="union";_tag_fat(_tmp4F4,sizeof(char),6U);});
# 2546
struct Cyc_List_List*ans=0;
for(1;des != 0;des=des->tl){
struct _tuple28*_tmp4C8=(struct _tuple28*)des->hd;struct _tuple28*_stmttmp65=_tmp4C8;struct _tuple28*_tmp4C9=_stmttmp65;void*_tmp4CB;struct Cyc_List_List*_tmp4CA;_tmp4CA=_tmp4C9->f1;_tmp4CB=_tmp4C9->f2;{struct Cyc_List_List*dl=_tmp4CA;void*a=_tmp4CB;
if(dl == 0){
# 2551
struct Cyc_List_List*_tmp4CC=fields;struct Cyc_List_List*fields2=_tmp4CC;
for(1;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple27*)fields2->hd)).f2){
(*((struct _tuple27*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp787=({struct Cyc_List_List*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));({void*_tmp786=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD->tag=1,_tmp4CD->f1=((*((struct _tuple27*)fields2->hd)).f1)->name;_tmp4CD;});_tmp4CE->hd=_tmp786;}),_tmp4CE->tl=0;_tmp4CE;});(*((struct _tuple28*)des->hd)).f1=_tmp787;});
ans=({struct Cyc_List_List*_tmp4D0=_region_malloc(rgn,sizeof(*_tmp4D0));({struct _tuple29*_tmp788=({struct _tuple29*_tmp4CF=_region_malloc(rgn,sizeof(*_tmp4CF));_tmp4CF->f1=(*((struct _tuple27*)fields2->hd)).f1,_tmp4CF->f2=a;_tmp4CF;});_tmp4D0->hd=_tmp788;}),_tmp4D0->tl=ans;_tmp4D0;});
break;}}
# 2559
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp646;_tmp646.tag=0,({struct _fat_ptr _tmp789=({const char*_tmp4D4="too many arguments to ";_tag_fat(_tmp4D4,sizeof(char),23U);});_tmp646.f1=_tmp789;});_tmp646;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp645;_tmp645.tag=0,_tmp645.f1=aggr_str;_tmp645;});void*_tmp4D1[2];_tmp4D1[0]=& _tmp4D2,_tmp4D1[1]=& _tmp4D3;({unsigned _tmp78A=loc;Cyc_Warn_err2(_tmp78A,_tag_fat(_tmp4D1,sizeof(void*),2));});});}else{
if(dl->tl != 0)
# 2563
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp647;_tmp647.tag=0,({struct _fat_ptr _tmp78B=({const char*_tmp4D7="multiple designators are not yet supported";_tag_fat(_tmp4D7,sizeof(char),43U);});_tmp647.f1=_tmp78B;});_tmp647;});void*_tmp4D5[1];_tmp4D5[0]=& _tmp4D6;({unsigned _tmp78C=loc;Cyc_Warn_err2(_tmp78C,_tag_fat(_tmp4D5,sizeof(void*),1));});});else{
# 2566
void*_tmp4D8=(void*)dl->hd;void*_stmttmp66=_tmp4D8;void*_tmp4D9=_stmttmp66;struct _fat_ptr*_tmp4DA;if(*((int*)_tmp4D9)== 0){
# 2568
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp649;_tmp649.tag=0,({struct _fat_ptr _tmp78D=({const char*_tmp4DE="array designator used in argument to ";_tag_fat(_tmp4DE,sizeof(char),38U);});_tmp649.f1=_tmp78D;});_tmp649;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp648;_tmp648.tag=0,_tmp648.f1=aggr_str;_tmp648;});void*_tmp4DB[2];_tmp4DB[0]=& _tmp4DC,_tmp4DB[1]=& _tmp4DD;({unsigned _tmp78E=loc;Cyc_Warn_err2(_tmp78E,_tag_fat(_tmp4DB,sizeof(void*),2));});});
goto _LL3;}else{_tmp4DA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4D9)->f1;{struct _fat_ptr*v=_tmp4DA;
# 2571
struct Cyc_List_List*_tmp4DF=fields;struct Cyc_List_List*fields2=_tmp4DF;
for(1;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple27*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple27*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64C;_tmp64C.tag=0,({struct _fat_ptr _tmp78F=({const char*_tmp4E5="member ";_tag_fat(_tmp4E5,sizeof(char),8U);});_tmp64C.f1=_tmp78F;});_tmp64C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64B;_tmp64B.tag=0,_tmp64B.f1=*v;_tmp64B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64A;_tmp64A.tag=0,({struct _fat_ptr _tmp790=({const char*_tmp4E4=" has already been used as an argument";_tag_fat(_tmp4E4,sizeof(char),38U);});_tmp64A.f1=_tmp790;});_tmp64A;});void*_tmp4E0[3];_tmp4E0[0]=& _tmp4E1,_tmp4E0[1]=& _tmp4E2,_tmp4E0[2]=& _tmp4E3;({unsigned _tmp791=loc;Cyc_Warn_err2(_tmp791,_tag_fat(_tmp4E0,sizeof(void*),3));});});
(*((struct _tuple27*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4E7=_region_malloc(rgn,sizeof(*_tmp4E7));({struct _tuple29*_tmp792=({struct _tuple29*_tmp4E6=_region_malloc(rgn,sizeof(*_tmp4E6));_tmp4E6->f1=(*((struct _tuple27*)fields2->hd)).f1,_tmp4E6->f2=a;_tmp4E6;});_tmp4E7->hd=_tmp792;}),_tmp4E7->tl=ans;_tmp4E7;});
break;}}
# 2580
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64E;_tmp64E.tag=0,({struct _fat_ptr _tmp793=({const char*_tmp4EB="bad field designator ";_tag_fat(_tmp4EB,sizeof(char),22U);});_tmp64E.f1=_tmp793;});_tmp64E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64D;_tmp64D.tag=0,_tmp64D.f1=*v;_tmp64D;});void*_tmp4E8[2];_tmp4E8[0]=& _tmp4E9,_tmp4E8[1]=& _tmp4EA;({unsigned _tmp794=loc;Cyc_Warn_err2(_tmp794,_tag_fat(_tmp4E8,sizeof(void*),2));});});
goto _LL3;}}_LL3:;}}}}
# 2587
if((int)aggr_kind == (int)0U){
if(({int _tmp795=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ans);_tmp795 < ((int(*)(struct Cyc_List_List*))Cyc_List_length)(fields);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64F;_tmp64F.tag=0,({struct _fat_ptr _tmp796=({const char*_tmp4EE="too few arguments to struct";_tag_fat(_tmp4EE,sizeof(char),28U);});_tmp64F.f1=_tmp796;});_tmp64F;});void*_tmp4EC[1];_tmp4EC[0]=& _tmp4ED;({unsigned _tmp797=loc;Cyc_Warn_err2(_tmp797,_tag_fat(_tmp4EC,sizeof(void*),1));});});}else{
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(ans)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp650;_tmp650.tag=0,({struct _fat_ptr _tmp798=({const char*_tmp4F1="union expression should have one member";_tag_fat(_tmp4F1,sizeof(char),40U);});_tmp650.f1=_tmp798;});_tmp650;});void*_tmp4EF[1];_tmp4EF[0]=& _tmp4F0;({unsigned _tmp799=loc;Cyc_Warn_err2(_tmp799,_tag_fat(_tmp4EF,sizeof(void*),1));});});}{
struct Cyc_List_List*_tmp4F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmp4F2;}}}
# 2537
;_pop_region();}
# 2598
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2600
void*_tmp4F5=e1->r;void*_stmttmp67=_tmp4F5;void*_tmp4F6=_stmttmp67;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;switch(*((int*)_tmp4F6)){case 14:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp652;_tmp652.tag=0,({struct _fat_ptr _tmp79A=({const char*_tmp500="we have a cast in a lhs: ";_tag_fat(_tmp500,sizeof(char),26U);});_tmp652.f1=_tmp79A;});_tmp652;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4FF=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp651;_tmp651.tag=4,_tmp651.f1=e1;_tmp651;});void*_tmp4FD[2];_tmp4FD[0]=& _tmp4FE,_tmp4FD[1]=& _tmp4FF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4FD,sizeof(void*),2));});case 20: _tmp4FC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4F6)->f1;{struct Cyc_Absyn_Exp*e1a=_tmp4FC;
_tmp4FB=e1a;goto _LL6;}case 23: _tmp4FB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4F6)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp4FB;
# 2604
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22: _tmp4FA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4F6)->f1;{struct Cyc_Absyn_Exp*e1a=_tmp4FA;
_tmp4F9=e1a;goto _LLA;}case 21: _tmp4F9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4F6)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp4F9;
# 2608
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp502=({struct Cyc_Warn_String_Warn_Warg_struct _tmp654;_tmp654.tag=0,({struct _fat_ptr _tmp79B=({const char*_tmp504="found zero pointer aggregate member assignment: ";_tag_fat(_tmp504,sizeof(char),49U);});_tmp654.f1=_tmp79B;});_tmp654;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp503=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp653;_tmp653.tag=4,_tmp653.f1=e1;_tmp653;});void*_tmp501[2];_tmp501[0]=& _tmp502,_tmp501[1]=& _tmp503;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp501,sizeof(void*),2));});
return 0;}case 13: _tmp4F8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4F6)->f1;{struct Cyc_Absyn_Exp*e1a=_tmp4F8;
_tmp4F7=e1a;goto _LLE;}case 12: _tmp4F7=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4F6)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp4F7;
# 2613
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp506=({struct Cyc_Warn_String_Warn_Warg_struct _tmp656;_tmp656.tag=0,({struct _fat_ptr _tmp79C=({const char*_tmp508="found zero pointer instantiate/noinstantiate: ";_tag_fat(_tmp508,sizeof(char),47U);});_tmp656.f1=_tmp79C;});_tmp656;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp507=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp655;_tmp655.tag=4,_tmp655.f1=e1;_tmp655;});void*_tmp505[2];_tmp505[0]=& _tmp506,_tmp505[1]=& _tmp507;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp505,sizeof(void*),2));});
return 0;}case 1:
 return 0;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp658;_tmp658.tag=0,({struct _fat_ptr _tmp79D=({const char*_tmp50C="found bad lhs in is_zero_ptr_deref: ";_tag_fat(_tmp50C,sizeof(char),37U);});_tmp658.f1=_tmp79D;});_tmp658;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp50B=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp657;_tmp657.tag=4,_tmp657.f1=e1;_tmp657;});void*_tmp509[2];_tmp509[0]=& _tmp50A,_tmp509[1]=& _tmp50B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp509,sizeof(void*),2));});};}
# 2627
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmp50D=Cyc_Absyn_compress(r);void*_stmttmp68=_tmp50D;void*_tmp50E=_stmttmp68;struct Cyc_Absyn_Tvar*_tmp50F;enum Cyc_Absyn_AliasQual _tmp511;enum Cyc_Absyn_KindQual _tmp510;switch(*((int*)_tmp50E)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50E)->f1)){case 7:
 return !must_be_unique;case 6:
 return 1;default: goto _LL9;}case 8: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50E)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50E)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50E)->f4 == 0){_tmp510=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50E)->f3)->kind)->v)->kind;_tmp511=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50E)->f3)->kind)->v)->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp510;enum Cyc_Absyn_AliasQual a=_tmp511;
# 2632
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2: _tmp50F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp50E)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp50F;
# 2636
struct Cyc_Absyn_Kind*_tmp512=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_rk);struct Cyc_Absyn_Kind*_stmttmp69=_tmp512;struct Cyc_Absyn_Kind*_tmp513=_stmttmp69;enum Cyc_Absyn_AliasQual _tmp515;enum Cyc_Absyn_KindQual _tmp514;_tmp514=_tmp513->kind;_tmp515=_tmp513->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp514;enum Cyc_Absyn_AliasQual a=_tmp515;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp516=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp6A=_tmp516;void*_tmp517=_stmttmp6A;struct Cyc_Core_Opt**_tmp518;if(*((int*)_tmp517)== 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp517)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp517)->f2)->aliasqual == Cyc_Absyn_Top){_tmp518=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp517)->f1;{struct Cyc_Core_Opt**x=_tmp518;
# 2640
({struct Cyc_Core_Opt*_tmp79F=({struct Cyc_Core_Opt*_tmp51A=_cycalloc(sizeof(*_tmp51A));({void*_tmp79E=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->tag=2,_tmp519->f1=0,_tmp519->f2=& Cyc_Kinds_rk;_tmp519;});_tmp51A->v=_tmp79E;});_tmp51A;});*x=_tmp79F;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11:
 return 1;};}
# 2644
return 0;}}default: _LL9:
 return 0;};}
# 2651
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp51B=Cyc_Absyn_compress(t);void*_stmttmp6B=_tmp51B;void*_tmp51C=_stmttmp6B;struct Cyc_Absyn_Tvar*_tmp51D;void*_tmp51E;switch(*((int*)_tmp51C)){case 3: _tmp51E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51C)->f1).ptr_atts).rgn;{void*r=_tmp51E;
# 2654
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2: _tmp51D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp51C)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp51D;
# 2656
struct Cyc_Absyn_Kind*_tmp51F=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_stmttmp6C=_tmp51F;struct Cyc_Absyn_Kind*_tmp520=_stmttmp6C;enum Cyc_Absyn_AliasQual _tmp522;enum Cyc_Absyn_KindQual _tmp521;_tmp521=_tmp520->kind;_tmp522=_tmp520->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp521;enum Cyc_Absyn_AliasQual a=_tmp522;
enum Cyc_Absyn_KindQual _tmp523=k;switch(_tmp523){case Cyc_Absyn_BoxKind:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp524=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp6D=_tmp524;void*_tmp525=_stmttmp6D;enum Cyc_Absyn_KindQual _tmp527;struct Cyc_Core_Opt**_tmp526;if(*((int*)_tmp525)== 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp525)->f2)->aliasqual == Cyc_Absyn_Top){_tmp526=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp525)->f1;_tmp527=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp525)->f2)->kind;{struct Cyc_Core_Opt**x=_tmp526;enum Cyc_Absyn_KindQual k=_tmp527;
# 2662
({struct Cyc_Core_Opt*_tmp7A2=({struct Cyc_Core_Opt*_tmp52A=_cycalloc(sizeof(*_tmp52A));({void*_tmp7A1=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529->tag=2,_tmp529->f1=0,({struct Cyc_Absyn_Kind*_tmp7A0=({struct Cyc_Absyn_Kind*_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528->kind=k,_tmp528->aliasqual=Cyc_Absyn_Aliasable;_tmp528;});_tmp529->f2=_tmp7A0;});_tmp529;});_tmp52A->v=_tmp7A1;});_tmp52A;});*x=_tmp7A2;});
return 0;}}else{goto _LL16;}}else{_LL16:
 return 1;};}
# 2666
return 0;default:
 return 0;};}}default:
# 2669
 return 0;};}
# 2672
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
t=Cyc_Absyn_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(t,0))
return 1;{
void*_tmp52B=t;struct Cyc_List_List*_tmp52C;struct Cyc_List_List*_tmp52E;union Cyc_Absyn_DatatypeFieldInfo _tmp52D;struct Cyc_List_List*_tmp530;struct Cyc_Absyn_Aggrdecl**_tmp52F;struct Cyc_List_List*_tmp531;switch(*((int*)_tmp52B)){case 6: _tmp531=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52B)->f1;{struct Cyc_List_List*qts=_tmp531;
# 2678
for(1;qts != 0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52B)->f1)){case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52B)->f1)->f1).KnownAggr).tag == 2){_tmp52F=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52B)->f1)->f1).KnownAggr).val;_tmp530=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52B)->f2;{struct Cyc_Absyn_Aggrdecl**adp=_tmp52F;struct Cyc_List_List*ts=_tmp530;
# 2683
if((*adp)->impl == 0)
return 0;{
struct Cyc_List_List*_tmp532=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp532;
{struct Cyc_List_List*_tmp533=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp533;for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type)))
return 1;}}
return 0;}}}else{
# 2695
goto _LLA;}case 18: _LLA:
 return 0;case 19: _tmp52D=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52B)->f1)->f1;_tmp52E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52B)->f2;{union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp52D;struct Cyc_List_List*ts=_tmp52E;
# 2699
union Cyc_Absyn_DatatypeFieldInfo _tmp534=tinfo;struct Cyc_Absyn_Datatypefield*_tmp536;struct Cyc_Absyn_Datatypedecl*_tmp535;if((_tmp534.UnknownDatatypefield).tag == 1)
return 0;else{_tmp535=((_tmp534.KnownDatatypefield).val).f1;_tmp536=((_tmp534.KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*td=_tmp535;struct Cyc_Absyn_Datatypefield*fld=_tmp536;
# 2702
struct Cyc_List_List*_tmp537=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp537;
{struct Cyc_List_List*_tmp538=fld->typs;struct Cyc_List_List*typs=_tmp538;for(0;typs != 0;typs=typs->tl){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 2708
return 0;}};}default: goto _LLD;}case 7: _tmp52C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52B)->f2;{struct Cyc_List_List*x=_tmp52C;
# 2691
for(1;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD:
# 2710
 return 0;};}}
# 2717
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp539=e->r;void*_stmttmp6E=_tmp539;void*_tmp53A=_stmttmp6E;struct Cyc_Absyn_Stmt*_tmp53B;struct Cyc_Absyn_Exp*_tmp53C;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Exp*_tmp53E;struct Cyc_Absyn_Exp*_tmp541;struct Cyc_Absyn_Exp*_tmp540;struct _fat_ptr*_tmp543;struct Cyc_Absyn_Exp*_tmp542;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_Absyn_Exp*_tmp545;switch(*((int*)_tmp53A)){case 1: if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53A)->f1)== 1)
return 0;else{goto _LL13;}case 22: _tmp545=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;{struct Cyc_Absyn_Exp*e1=_tmp545;
_tmp544=e1;goto _LL6;}case 20: _tmp544=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp544;
# 2722
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21: _tmp542=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_tmp543=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;{struct Cyc_Absyn_Exp*e1=_tmp542;struct _fat_ptr*f=_tmp543;
return Cyc_Tcutil_is_noalias_path(e1);}case 23: _tmp540=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_tmp541=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;{struct Cyc_Absyn_Exp*e1=_tmp540;struct Cyc_Absyn_Exp*e2=_tmp541;
# 2725
void*_tmp546=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp6F=_tmp546;void*_tmp547=_stmttmp6F;if(*((int*)_tmp547)== 6)
return Cyc_Tcutil_is_noalias_path(e1);else{
return 0;};}case 6: _tmp53E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_tmp53F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53A)->f3;{struct Cyc_Absyn_Exp*e1=_tmp53E;struct Cyc_Absyn_Exp*e2=_tmp53F;
# 2730
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9: _tmp53D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;{struct Cyc_Absyn_Exp*e2=_tmp53D;
_tmp53C=e2;goto _LL10;}case 14: _tmp53C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp53C;
return Cyc_Tcutil_is_noalias_path(e2);}case 37: _tmp53B=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;{struct Cyc_Absyn_Stmt*s=_tmp53B;
# 2734
while(1){
void*_tmp548=s->r;void*_stmttmp70=_tmp548;void*_tmp549=_stmttmp70;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Stmt*_tmp54C;struct Cyc_Absyn_Decl*_tmp54B;struct Cyc_Absyn_Stmt*_tmp54E;struct Cyc_Absyn_Stmt*_tmp54D;switch(*((int*)_tmp549)){case 2: _tmp54D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp549)->f1;_tmp54E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp549)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp54D;struct Cyc_Absyn_Stmt*s2=_tmp54E;
s=s2;goto _LL1A;}case 12: _tmp54B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp549)->f1;_tmp54C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp549)->f2;{struct Cyc_Absyn_Decl*d=_tmp54B;struct Cyc_Absyn_Stmt*s1=_tmp54C;
s=s1;goto _LL1A;}case 1: _tmp54A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp549)->f1;{struct Cyc_Absyn_Exp*e=_tmp54A;
return Cyc_Tcutil_is_noalias_path(e);}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp550=({struct Cyc_Warn_String_Warn_Warg_struct _tmp659;_tmp659.tag=0,({struct _fat_ptr _tmp7A3=({const char*_tmp551="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp551,sizeof(char),40U);});_tmp659.f1=_tmp7A3;});_tmp659;});void*_tmp54F[1];_tmp54F[0]=& _tmp550;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp54F,sizeof(void*),1));});}_LL1A:;}}default: _LL13:
# 2742
 return 1;};}
# 2759 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 2761
struct _tuple14 bogus_ans=({struct _tuple14 _tmp667;_tmp667.f1=0,_tmp667.f2=Cyc_Absyn_heap_rgn_type;_tmp667;});
void*_tmp552=e->r;void*_stmttmp71=_tmp552;void*_tmp553=_stmttmp71;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp556;int _tmp559;struct _fat_ptr*_tmp558;struct Cyc_Absyn_Exp*_tmp557;int _tmp55C;struct _fat_ptr*_tmp55B;struct Cyc_Absyn_Exp*_tmp55A;void*_tmp55D;switch(*((int*)_tmp553)){case 1: _tmp55D=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp553)->f1;{void*x=_tmp55D;
# 2765
void*_tmp55E=x;struct Cyc_Absyn_Vardecl*_tmp55F;struct Cyc_Absyn_Vardecl*_tmp560;struct Cyc_Absyn_Vardecl*_tmp561;struct Cyc_Absyn_Vardecl*_tmp562;switch(*((int*)_tmp55E)){case 0:
 goto _LL11;case 2: _LL11:
 return bogus_ans;case 1: _tmp562=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp55E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp562;
# 2769
void*_tmp563=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp72=_tmp563;void*_tmp564=_stmttmp72;if(*((int*)_tmp564)== 4)
# 2771
return({struct _tuple14 _tmp65A;_tmp65A.f1=1,_tmp65A.f2=Cyc_Absyn_heap_rgn_type;_tmp65A;});else{
return({struct _tuple14 _tmp65B;_tmp65B.f1=(vd->tq).real_const,_tmp65B.f2=Cyc_Absyn_heap_rgn_type;_tmp65B;});};}case 4: _tmp561=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp55E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp561;
# 2775
void*_tmp565=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp73=_tmp565;void*_tmp566=_stmttmp73;if(*((int*)_tmp566)== 4)
return({struct _tuple14 _tmp65C;_tmp65C.f1=1,_tmp65C.f2=(void*)_check_null(vd->rgn);_tmp65C;});else{
# 2778
vd->escapes=1;
return({struct _tuple14 _tmp65D;_tmp65D.f1=(vd->tq).real_const,_tmp65D.f2=(void*)_check_null(vd->rgn);_tmp65D;});};}case 5: _tmp560=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp55E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp560;
# 2781
_tmp55F=vd;goto _LL19;}default: _tmp55F=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp55E)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp55F;
# 2783
vd->escapes=1;
return({struct _tuple14 _tmp65E;_tmp65E.f1=(vd->tq).real_const,_tmp65E.f2=(void*)_check_null(vd->rgn);_tmp65E;});}};}case 21: _tmp55A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp553)->f1;_tmp55B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp553)->f2;_tmp55C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp553)->f3;{struct Cyc_Absyn_Exp*e1=_tmp55A;struct _fat_ptr*f=_tmp55B;int is_tagged=_tmp55C;
# 2788
if(is_tagged)return bogus_ans;{
# 2791
void*_tmp567=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp74=_tmp567;void*_tmp568=_stmttmp74;struct Cyc_Absyn_Aggrdecl*_tmp569;struct Cyc_List_List*_tmp56A;switch(*((int*)_tmp568)){case 7: _tmp56A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp568)->f2;{struct Cyc_List_List*fs=_tmp56A;
# 2793
struct Cyc_Absyn_Aggrfield*_tmp56B=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp56B;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp56C=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp75=_tmp56C;struct _tuple14 _tmp56D=_stmttmp75;void*_tmp56F;int _tmp56E;_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;{int c=_tmp56E;void*t=_tmp56F;
return({struct _tuple14 _tmp65F;_tmp65F.f1=(finfo->tq).real_const || c,_tmp65F.f2=t;_tmp65F;});}}
# 2798
return bogus_ans;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp568)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp568)->f1)->f1).KnownAggr).tag == 2){_tmp569=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp568)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp569;
# 2800
struct Cyc_Absyn_Aggrfield*_tmp570=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp570;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp571=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp76=_tmp571;struct _tuple14 _tmp572=_stmttmp76;void*_tmp574;int _tmp573;_tmp573=_tmp572.f1;_tmp574=_tmp572.f2;{int c=_tmp573;void*t=_tmp574;
return({struct _tuple14 _tmp660;_tmp660.f1=(finfo->tq).real_const || c,_tmp660.f2=t;_tmp660;});}}
# 2805
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29:
 return bogus_ans;};}}case 22: _tmp557=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp553)->f1;_tmp558=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp553)->f2;_tmp559=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp553)->f3;{struct Cyc_Absyn_Exp*e1=_tmp557;struct _fat_ptr*f=_tmp558;int is_tagged=_tmp559;
# 2810
if(is_tagged)return bogus_ans;{
# 2813
void*_tmp575=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp77=_tmp575;void*_tmp576=_stmttmp77;void*_tmp578;void*_tmp577;if(*((int*)_tmp576)== 3){_tmp577=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576)->f1).elt_type;_tmp578=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576)->f1).ptr_atts).rgn;{void*t1=_tmp577;void*r=_tmp578;
# 2815
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp579=Cyc_Absyn_compress(t1);void*_stmttmp78=_tmp579;void*_tmp57A=_stmttmp78;struct Cyc_Absyn_Aggrdecl*_tmp57B;struct Cyc_List_List*_tmp57C;switch(*((int*)_tmp57A)){case 7: _tmp57C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57A)->f2;{struct Cyc_List_List*fs=_tmp57C;
# 2818
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57A)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57A)->f1)->f1).KnownAggr).tag == 2){_tmp57B=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57A)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp57B;
# 2820
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B:
 return bogus_ans;}_LL36:;}
# 2823
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp661;_tmp661.f1=(finfo->tq).real_const,_tmp661.f2=r;_tmp661;});
return bogus_ans;}}else{
return bogus_ans;};}}case 20: _tmp556=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp553)->f1;{struct Cyc_Absyn_Exp*e1=_tmp556;
# 2830
void*_tmp57D=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp79=_tmp57D;void*_tmp57E=_stmttmp79;void*_tmp580;struct Cyc_Absyn_Tqual _tmp57F;if(*((int*)_tmp57E)== 3){_tmp57F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57E)->f1).elt_tq;_tmp580=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57E)->f1).ptr_atts).rgn;{struct Cyc_Absyn_Tqual tq=_tmp57F;void*r=_tmp580;
# 2832
return({struct _tuple14 _tmp662;_tmp662.f1=tq.real_const,_tmp662.f2=r;_tmp662;});}}else{
return bogus_ans;};}case 23: _tmp554=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp553)->f1;_tmp555=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp553)->f2;{struct Cyc_Absyn_Exp*e1=_tmp554;struct Cyc_Absyn_Exp*e2=_tmp555;
# 2838
void*t=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*_tmp581=t;struct Cyc_Absyn_Tqual _tmp582;void*_tmp584;struct Cyc_Absyn_Tqual _tmp583;struct Cyc_List_List*_tmp585;switch(*((int*)_tmp581)){case 6: _tmp585=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp581)->f1;{struct Cyc_List_List*ts=_tmp585;
# 2842
struct _tuple13 _tmp586=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp7A=_tmp586;struct _tuple13 _tmp587=_stmttmp7A;int _tmp589;unsigned _tmp588;_tmp588=_tmp587.f1;_tmp589=_tmp587.f2;{unsigned i=_tmp588;int known=_tmp589;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp58A=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp58A;
if(finfo != 0)
return({struct _tuple14 _tmp663;_tmp663.f1=((*finfo).f1).real_const,({void*_tmp7A4=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp663.f2=_tmp7A4;});_tmp663;});
return bogus_ans;}}}case 3: _tmp583=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp581)->f1).elt_tq;_tmp584=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp581)->f1).ptr_atts).rgn;{struct Cyc_Absyn_Tqual tq=_tmp583;void*r=_tmp584;
# 2850
return({struct _tuple14 _tmp664;_tmp664.f1=tq.real_const,_tmp664.f2=r;_tmp664;});}case 4: _tmp582=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp581)->f1).tq;{struct Cyc_Absyn_Tqual tq=_tmp582;
# 2856
return({struct _tuple14 _tmp665;_tmp665.f1=tq.real_const,({void*_tmp7A5=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp665.f2=_tmp7A5;});_tmp665;});}default:
 return bogus_ans;};}default:
# 2860
({struct Cyc_Warn_String_Warn_Warg_struct _tmp58C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp666;_tmp666.tag=0,({struct _fat_ptr _tmp7A6=({const char*_tmp58D="unary & applied to non-lvalue";_tag_fat(_tmp58D,sizeof(char),30U);});_tmp666.f1=_tmp7A6;});_tmp666;});void*_tmp58B[1];_tmp58B[0]=& _tmp58C;({unsigned _tmp7A7=e->loc;Cyc_Warn_err2(_tmp7A7,_tag_fat(_tmp58B,sizeof(void*),1));});});
return bogus_ans;};}
# 2867
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp58E=({void*_tmp7A8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7A8,b);});struct Cyc_Absyn_Exp*eopt=_tmp58E;
if(eopt == 0)return;{
struct _tuple13 _tmp58F=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp7B=_tmp58F;struct _tuple13 _tmp590=_stmttmp7B;int _tmp592;unsigned _tmp591;_tmp591=_tmp590.f1;_tmp592=_tmp590.f2;{unsigned j=_tmp591;int known=_tmp592;
if(known && j <= i)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp594=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66B;_tmp66B.tag=0,({struct _fat_ptr _tmp7A9=({const char*_tmp599="a dereference is out of bounds: ";_tag_fat(_tmp599,sizeof(char),33U);});_tmp66B.f1=_tmp7A9;});_tmp66B;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp595=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp66A;_tmp66A.tag=12,_tmp66A.f1=(int)j;_tmp66A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp596=({struct Cyc_Warn_String_Warn_Warg_struct _tmp669;_tmp669.tag=0,({
struct _fat_ptr _tmp7AA=({const char*_tmp598=" <= ";_tag_fat(_tmp598,sizeof(char),5U);});_tmp669.f1=_tmp7AA;});_tmp669;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp597=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp668;_tmp668.tag=12,_tmp668.f1=(int)i;_tmp668;});void*_tmp593[4];_tmp593[0]=& _tmp594,_tmp593[1]=& _tmp595,_tmp593[2]=& _tmp596,_tmp593[3]=& _tmp597;({void(*_tmp7AC)(unsigned,struct _fat_ptr)=
# 2872
do_warn?Cyc_Warn_warn2: Cyc_Warn_err2;unsigned _tmp7AB=loc;_tmp7AC(_tmp7AB,_tag_fat(_tmp593,sizeof(void*),4));});});
# 2874
return;}}}
# 2877
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 2885
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
int(*_tmp59A)(int,struct Cyc_Absyn_Exp*)=Cyc_Tcutil_cnst_exp;int(*rec)(int,struct Cyc_Absyn_Exp*)=_tmp59A;
void*_tmp59B=e->r;void*_stmttmp7C=_tmp59B;void*_tmp59C=_stmttmp7C;struct Cyc_List_List*_tmp59D;struct Cyc_List_List*_tmp59E;struct Cyc_List_List*_tmp59F;struct Cyc_List_List*_tmp5A0;struct Cyc_List_List*_tmp5A1;struct Cyc_List_List*_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Exp*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A7;struct Cyc_Absyn_Exp*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;void*_tmp5B0;switch(*((int*)_tmp59C)){case 0:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 17: _LL6:
 goto _LL8;case 18: _LL8:
 goto _LLA;case 19: _LLA:
 goto _LLC;case 32: _LLC:
 goto _LLE;case 33: _LLE:
 return 1;case 1: _tmp5B0=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;{void*b=_tmp5B0;
# 2898
void*_tmp5B1=b;struct Cyc_Absyn_Vardecl*_tmp5B2;struct Cyc_Absyn_Vardecl*_tmp5B3;switch(*((int*)_tmp5B1)){case 2:
 return 1;case 1: _tmp5B3=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5B1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp5B3;
# 2901
void*_tmp5B4=Cyc_Absyn_compress(vd->type);void*_stmttmp7D=_tmp5B4;void*_tmp5B5=_stmttmp7D;switch(*((int*)_tmp5B5)){case 4:
 goto _LL40;case 5: _LL40:
 return 1;default:
 return var_okay;};}case 4: _tmp5B2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5B1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp5B2;
# 2907
if((int)vd->sc != (int)Cyc_Absyn_Static)
return var_okay;{
void*_tmp5B6=Cyc_Absyn_compress(vd->type);void*_stmttmp7E=_tmp5B6;void*_tmp5B7=_stmttmp7E;switch(*((int*)_tmp5B7)){case 4:
 goto _LL47;case 5: _LL47:
 return 1;default:
 return var_okay;};}}case 0:
# 2914
 return 0;default:
 return var_okay;};}case 6: _tmp5AD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;_tmp5AE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59C)->f2;_tmp5AF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59C)->f3;{struct Cyc_Absyn_Exp*e1=_tmp5AD;struct Cyc_Absyn_Exp*e2=_tmp5AE;struct Cyc_Absyn_Exp*e3=_tmp5AF;
# 2918
return(rec(0,e1)&& rec(0,e2))&& rec(0,e3);}case 9: _tmp5AB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;_tmp5AC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59C)->f2;{struct Cyc_Absyn_Exp*e1=_tmp5AB;struct Cyc_Absyn_Exp*e2=_tmp5AC;
return rec(0,e1)&& rec(0,e2);}case 41: _tmp5AA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;{struct Cyc_Absyn_Exp*e2=_tmp5AA;
_tmp5A9=e2;goto _LL18;}case 12: _tmp5A9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5A9;
_tmp5A8=e2;goto _LL1A;}case 13: _tmp5A8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5A8;
_tmp5A7=e2;goto _LL1C;}case 14: _tmp5A7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59C)->f2;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp5A7;
# 2925
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 15: _tmp5A6=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;{struct Cyc_Absyn_Exp*e2=_tmp5A6;
return rec(1,e2);}case 27: _tmp5A4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59C)->f2;_tmp5A5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59C)->f3;{struct Cyc_Absyn_Exp*e1=_tmp5A4;struct Cyc_Absyn_Exp*e2=_tmp5A5;
return rec(0,e1)&& rec(0,e2);}case 28: _tmp5A3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;{struct Cyc_Absyn_Exp*e=_tmp5A3;
return rec(0,e);}case 26: _tmp5A2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;{struct Cyc_List_List*des=_tmp5A2;
_tmp5A1=des;goto _LL26;}case 30: _tmp5A1=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp59C)->f2;_LL26: {struct Cyc_List_List*des=_tmp5A1;
_tmp5A0=des;goto _LL28;}case 29: _tmp5A0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp59C)->f3;_LL28: {struct Cyc_List_List*des=_tmp5A0;
# 2932
for(1;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3: _tmp59F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59C)->f2;{struct Cyc_List_List*es=_tmp59F;
_tmp59E=es;goto _LL2C;}case 24: _tmp59E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;_LL2C: {struct Cyc_List_List*es=_tmp59E;
_tmp59D=es;goto _LL2E;}case 31: _tmp59D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp59C)->f1;_LL2E: {struct Cyc_List_List*es=_tmp59D;
# 2939
for(1;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default:
 return 0;};}
# 2946
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 2950
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5B8=Cyc_Absyn_compress(t);void*_stmttmp7F=_tmp5B8;void*_tmp5B9=_stmttmp7F;struct Cyc_List_List*_tmp5BA;struct Cyc_List_List*_tmp5BB;void*_tmp5BC;void*_tmp5BD;struct Cyc_List_List*_tmp5BF;void*_tmp5BE;switch(*((int*)_tmp5B9)){case 0: _tmp5BE=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B9)->f1;_tmp5BF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B9)->f2;{void*c=_tmp5BE;struct Cyc_List_List*ts=_tmp5BF;
# 2954
void*_tmp5C0=c;union Cyc_Absyn_AggrInfo _tmp5C1;struct Cyc_List_List*_tmp5C2;struct Cyc_Absyn_Enumdecl*_tmp5C3;switch(*((int*)_tmp5C0)){case 0:
 goto _LL11;case 1: _LL11:
 goto _LL13;case 2: _LL13:
 return 1;case 15: _tmp5C3=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5C0)->f2;{struct Cyc_Absyn_Enumdecl*edo=_tmp5C3;
# 2959
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5C2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16: _tmp5C2=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5C0)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5C2;
# 2963
{struct Cyc_List_List*_tmp5C4=fs;struct Cyc_List_List*fs2=_tmp5C4;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5C5=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp80=_tmp5C5;struct _tuple13 _tmp5C6=_stmttmp80;int _tmp5C8;unsigned _tmp5C7;_tmp5C7=_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;{unsigned i=_tmp5C7;int known=_tmp5C8;
if(known && i == (unsigned)0)
return 1;}}}}
# 2970
return 0;}case 20: _tmp5C1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5C0)->f1;{union Cyc_Absyn_AggrInfo info=_tmp5C1;
# 2973
struct Cyc_Absyn_Aggrdecl*_tmp5C9=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5C9;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default:
 return 0;};}case 3: _tmp5BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B9)->f1).ptr_atts).nullable;{void*n=_tmp5BD;
# 2981
return Cyc_Tcutil_force_type2bool(1,n);}case 4: _tmp5BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B9)->f1).elt_type;{void*t=_tmp5BC;
return Cyc_Tcutil_zeroable_type(t);}case 6: _tmp5BB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B9)->f1;{struct Cyc_List_List*tqs=_tmp5BB;
# 2984
for(1;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7: _tmp5BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B9)->f2;{struct Cyc_List_List*fs=_tmp5BA;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default:
 return 0;};}
# 2992
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 2994
struct _RegionHandle _tmp5CA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5CA;_push_region(rgn);
{struct Cyc_List_List*_tmp5CB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5CB;
for(1;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5CC=0;_npop_handler(0);return _tmp5CC;}}{
# 3002
int _tmp5CD=1;_npop_handler(0);return _tmp5CD;}}
# 2995
;_pop_region();}
# 3006
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5CE=t;struct Cyc_Absyn_Typedefdecl*_tmp5CF;if(*((int*)_tmp5CE)== 8){_tmp5CF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5CE)->f3;{struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5CF;
# 3009
if(tdopt == 0)
return;{
struct Cyc_Absyn_Tqual _tmp5D0=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5D0;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66E;_tmp66E.tag=0,({struct _fat_ptr _tmp7AD=({const char*_tmp5D6="qualifier within typedef type ";_tag_fat(_tmp5D6,sizeof(char),31U);});_tmp66E.f1=_tmp7AD;});_tmp66E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp66D;_tmp66D.tag=2,_tmp66D.f1=(void*)t;_tmp66D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66C;_tmp66C.tag=0,({struct _fat_ptr _tmp7AE=({const char*_tmp5D5=" is ignored";_tag_fat(_tmp5D5,sizeof(char),12U);});_tmp66C.f1=_tmp7AE;});_tmp66C;});void*_tmp5D1[3];_tmp5D1[0]=& _tmp5D2,_tmp5D1[1]=& _tmp5D3,_tmp5D1[2]=& _tmp5D4;({unsigned _tmp7AF=loc;Cyc_Warn_warn2(_tmp7AF,_tag_fat(_tmp5D1,sizeof(void*),3));});});
return;}}}else{
return;};}
# 3020
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5D7=Cyc_Absyn_compress(t);void*_stmttmp81=_tmp5D7;void*_tmp5D8=_stmttmp81;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_PtrInfo _tmp5DA;switch(*((int*)_tmp5D8)){case 3: _tmp5DA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D8)->f1;{struct Cyc_Absyn_PtrInfo pi=_tmp5DA;
return({void*_tmp7B0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7B0,(pi.ptr_atts).bounds);});}case 4: _tmp5D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D8)->f1).num_elts;{struct Cyc_Absyn_Exp*e=_tmp5D9;
return e;}default:
 return 0;};}
# 3030
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5DB=b;struct Cyc_Absyn_Vardecl*_tmp5DC;struct Cyc_Absyn_Vardecl*_tmp5DD;struct Cyc_Absyn_Vardecl*_tmp5DE;struct Cyc_Absyn_Vardecl*_tmp5DF;switch(*((int*)_tmp5DB)){case 5: _tmp5DF=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5DB)->f1;{struct Cyc_Absyn_Vardecl*x=_tmp5DF;
_tmp5DE=x;goto _LL4;}case 4: _tmp5DE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5DB)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp5DE;
_tmp5DD=x;goto _LL6;}case 3: _tmp5DD=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5DB)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp5DD;
_tmp5DC=x;goto _LL8;}case 1: _tmp5DC=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5DB)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp5DC;
return x->escapes?0: x;}default:
 return 0;};}
# 3041
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5E0=0;struct Cyc_List_List*res=_tmp5E0;
for(1;l != 0;l=l->tl){
if((void**)l->hd != 0)res=({struct Cyc_List_List*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->hd=*((void**)_check_null((void**)l->hd)),_tmp5E1->tl=res;_tmp5E1;});}
return res;}
# 3048
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5E2=Cyc_Absyn_compress(t);void*_stmttmp82=_tmp5E2;void*_tmp5E3=_stmttmp82;unsigned _tmp5E8;void*_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E6;struct Cyc_Absyn_Tqual _tmp5E5;void*_tmp5E4;if(*((int*)_tmp5E3)== 4){_tmp5E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).elt_type;_tmp5E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).tq;_tmp5E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).num_elts;_tmp5E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).zero_term;_tmp5E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).zt_loc;{void*et=_tmp5E4;struct Cyc_Absyn_Tqual tq=_tmp5E5;struct Cyc_Absyn_Exp*eopt=_tmp5E6;void*zt=_tmp5E7;unsigned ztl=_tmp5E8;
# 3051
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
if(convert_tag){
if(eopt->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66F;_tmp66F.tag=0,({struct _fat_ptr _tmp7B1=({const char*_tmp5EB="cannot convert tag without type!";_tag_fat(_tmp5EB,sizeof(char),33U);});_tmp66F.f1=_tmp7B1;});_tmp66F;});void*_tmp5E9[1];_tmp5E9[0]=& _tmp5EA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp5E9,sizeof(void*),1));});{
void*_tmp5EC=Cyc_Absyn_compress((void*)_check_null(eopt->topt));void*_stmttmp83=_tmp5EC;void*_tmp5ED=_stmttmp83;void*_tmp5EE;if(*((int*)_tmp5ED)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5ED)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5ED)->f2 != 0){_tmp5EE=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5ED)->f2)->hd;{void*t=_tmp5EE;
# 3060
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3065
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt,Cyc_Absyn_false_type);}}else{
return t;};}

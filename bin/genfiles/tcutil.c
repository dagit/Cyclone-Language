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
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 283
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 303
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 844 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 847
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 851
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 858
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 864
void*Cyc_Absyn_compress(void*);
# 868
int Cyc_Absyn_type2bool(int,void*);
# 877
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 882
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 884
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 889
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 891
extern void*Cyc_Absyn_empty_effect;
# 893
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 895
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*,struct Cyc_Absyn_Enumdecl*);
# 920
extern void*Cyc_Absyn_fat_bound_type;
# 922
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 924
void*Cyc_Absyn_bounds_one (void);
# 926
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 930
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 949
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
# 951
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 983
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 987
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1006
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
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1066
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
# 1107
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1109
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1111
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1121
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
# 78
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
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
return(int)(Cyc_Tcutil_type_kind(t))->kind == 2;}
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
void*_tmp2E=Cyc_Absyn_compress(t);void*_stmttmp12=_tmp2E;void*_tmp2F=_stmttmp12;struct Cyc_Absyn_Tqual _tmp31;void*_tmp33;void*_tmp32;void*_tmp30;switch(*((int*)_tmp2F)){case 3: _tmp30=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp32=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).bounds;_tmp33=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).zero_term;{void*elt=_tmp30;void*bnds=_tmp32;void*zt=_tmp33;
# 206
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*ptr_type=t;
*elt_type=elt;
{void*_tmp34=Cyc_Absyn_compress(bnds);void*_stmttmp13=_tmp34;void*_tmp35=_stmttmp13;if(*((int*)_tmp35)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35)->f1)== 14){
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA:
*is_fat=0;goto _LL7;}_LL7:;}
# 214
return 1;}case 4: _tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).tq;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).zero_term;{void*elt=_tmp30;struct Cyc_Absyn_Tqual tq=_tmp31;struct Cyc_Absyn_Exp*n=_tmp32;void*zt=_tmp33;
# 216
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*elt_type=elt;
*is_fat=0;
({void*_tmp543=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp543;});
return 1;}default:
 return 0;};}
# 229
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 235
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp36=({void*_tmp544=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp544,b);});struct Cyc_Absyn_Exp*eopt=_tmp36;
if(eopt == 0)return 0;{
struct _tuple13 _tmp37=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp14=_tmp37;struct _tuple13 _tmp38=_stmttmp14;int _tmp3A;unsigned _tmp39;_tmp39=_tmp38.f1;_tmp3A=_tmp38.f2;{unsigned i=_tmp39;int known=_tmp3A;
return known && i == 1U;}}}
# 243
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3B=Cyc_Absyn_compress(t);void*_stmttmp15=_tmp3B;void*_tmp3C=_stmttmp15;void*_tmp3E;void*_tmp3D;switch(*((int*)_tmp3C)){case 0: _tmp3D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C)->f1;_tmp3E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C)->f2;{void*c=_tmp3D;struct Cyc_List_List*ts=_tmp3E;
# 246
void*_tmp3F=c;void*_tmp40;switch(*((int*)_tmp3F)){case 0:
 goto _LLF;case 1: _LLF:
 goto _LL11;case 2: _LL11:
 goto _LL13;case 16: _LL13:
 goto _LL15;case 15: _LL15:
 return 1;case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).UnknownAggr).tag == 1)
# 253
return 0;else{_tmp40=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp40;
# 255
if(ad->impl == 0)
return 0;{
struct _RegionHandle _tmp41=_new_region("rgn");struct _RegionHandle*rgn=& _tmp41;_push_region(rgn);
{struct Cyc_List_List*_tmp42=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(rgn,rgn,ad->tvs,ts);struct Cyc_List_List*inst=_tmp42;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp43=0;_npop_handler(0);return _tmp43;}}}{
int _tmp44=1;_npop_handler(0);return _tmp44;}}
# 258
;_pop_region();}}}default:
# 263
 return 0;};}case 4: _tmp3D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C)->f1).elt_type;_tmp3E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C)->f1).zero_term;{void*t=_tmp3D;void*zero_term=_tmp3E;
# 268
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6: _tmp3D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3C)->f1;{struct Cyc_List_List*tqs=_tmp3D;
# 270
for(1;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7: _tmp3D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C)->f2;{struct Cyc_List_List*fs=_tmp3D;
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
return({struct Cyc_Absyn_Tvar*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->name=tv->name,_tmp45->identity=-1,({void*_tmp545=Cyc_Kinds_copy_kindbound(tv->kind);_tmp45->kind=_tmp545;});_tmp45;});}
# 304
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9*_tmp46=arg;void*_tmp49;struct Cyc_Absyn_Tqual _tmp48;void*_tmp47;_tmp47=_tmp46->f1;_tmp48=_tmp46->f2;_tmp49=_tmp46->f3;{struct _fat_ptr*x=_tmp47;struct Cyc_Absyn_Tqual y=_tmp48;void*t=_tmp49;
return({struct _tuple9*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->f1=x,_tmp4A->f2=y,({void*_tmp546=Cyc_Tcutil_copy_type(t);_tmp4A->f3=_tmp546;});_tmp4A;});}}
# 308
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->f1=(*arg).f1,({void*_tmp547=Cyc_Tcutil_copy_type((*arg).f2);_tmp4B->f2=_tmp547;});_tmp4B;});}
# 311
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
return e == 0?0: Cyc_Tcutil_deep_copy_exp(preserve_types,e);}
# 314
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->name=f->name,_tmp4C->tq=f->tq,({void*_tmp549=Cyc_Tcutil_copy_type(f->type);_tmp4C->type=_tmp549;}),_tmp4C->width=f->width,_tmp4C->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp548=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp4C->requires_clause=_tmp548;});_tmp4C;});}struct _tuple15{void*f1;void*f2;};
# 318
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp4D=x;void*_tmp4F;void*_tmp4E;_tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;{void*r1=_tmp4E;void*r2=_tmp4F;
return({struct _tuple15*_tmp50=_cycalloc(sizeof(*_tmp50));({void*_tmp54B=Cyc_Tcutil_copy_type(r1);_tmp50->f1=_tmp54B;}),({void*_tmp54A=Cyc_Tcutil_copy_type(r2);_tmp50->f2=_tmp54A;});_tmp50;});}}
# 322
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 326
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp51=Cyc_Absyn_compress(t);void*_stmttmp16=_tmp51;void*_tmp52=_stmttmp16;enum Cyc_Absyn_AggrKind _tmp56;void*_tmp61;void*_tmp60;void*_tmp5F;void*_tmp5E;void*_tmp5D;int _tmp59;unsigned _tmp62;void*_tmp5C;void*_tmp5B;void*_tmp5A;void*_tmp58;void*_tmp55;struct Cyc_Absyn_Tqual _tmp57;void*_tmp54;void*_tmp53;switch(*((int*)_tmp52)){case 0: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2 == 0){_tmp53=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f1;{void*c=_tmp53;
return t;}}else{_tmp53=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f1;_tmp54=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2;{void*c=_tmp53;struct Cyc_List_List*ts=_tmp54;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=0,_tmp63->f1=c,({struct Cyc_List_List*_tmp54C=((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp63->f2=_tmp54C;});_tmp63;});}}case 1:
 return t;case 2: _tmp53=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp52)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp53;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3: _tmp53=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).elt_type;_tmp57=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).elt_tq;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).rgn;_tmp55=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).nullable;_tmp58=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).bounds;_tmp5A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).zero_term;_tmp5B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).ptrloc;_tmp5C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).released;{void*elt=_tmp53;struct Cyc_Absyn_Tqual tq=_tmp57;void*rgn=_tmp54;void*nbl=_tmp55;void*bs=_tmp58;void*zt=_tmp5A;struct Cyc_Absyn_PtrLoc*loc=_tmp5B;void*rel=_tmp5C;
# 333
void*_tmp64=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp64;
void*_tmp65=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp65;
void*_tmp66=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp66;
struct Cyc_Absyn_Tqual _tmp67=tq;struct Cyc_Absyn_Tqual tq2=_tmp67;
# 338
void*_tmp68=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp68;
void*_tmp69=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp69;
void*_tmp6A=Cyc_Tcutil_copy_type(rel);void*rel2=_tmp6A;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=3,(_tmp6B->f1).elt_type=elt2,(_tmp6B->f1).elt_tq=tq2,((_tmp6B->f1).ptr_atts).rgn=rgn2,((_tmp6B->f1).ptr_atts).nullable=nbl2,((_tmp6B->f1).ptr_atts).bounds=bs2,((_tmp6B->f1).ptr_atts).zero_term=zt2,((_tmp6B->f1).ptr_atts).ptrloc=loc,((_tmp6B->f1).ptr_atts).released=rel2;_tmp6B;});}case 4: _tmp53=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).elt_type;_tmp57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).tq;_tmp54=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).num_elts;_tmp55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).zero_term;_tmp62=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).zt_loc;{void*et=_tmp53;struct Cyc_Absyn_Tqual tq=_tmp57;struct Cyc_Absyn_Exp*eo=_tmp54;void*zt=_tmp55;unsigned ztl=_tmp62;
# 343
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=4,({void*_tmp54F=Cyc_Tcutil_copy_type(et);(_tmp6C->f1).elt_type=_tmp54F;}),(_tmp6C->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp54E=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp6C->f1).num_elts=_tmp54E;}),({
void*_tmp54D=Cyc_Tcutil_copy_type(zt);(_tmp6C->f1).zero_term=_tmp54D;}),(_tmp6C->f1).zt_loc=ztl;_tmp6C;});}case 5: _tmp53=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).tvars;_tmp54=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).effect;_tmp57=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).ret_tqual;_tmp55=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).ret_type;_tmp58=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).args;_tmp59=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).c_varargs;_tmp5A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).cyc_varargs;_tmp5B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).rgn_po;_tmp5C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).attributes;_tmp5D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).requires_clause;_tmp5E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).requires_relns;_tmp5F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).ensures_clause;_tmp60=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).ensures_relns;_tmp61=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).return_value;{struct Cyc_List_List*tvs=_tmp53;void*effopt=_tmp54;struct Cyc_Absyn_Tqual rt_tq=_tmp57;void*rt=_tmp55;struct Cyc_List_List*args=_tmp58;int c_varargs=_tmp59;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp5A;struct Cyc_List_List*rgn_po=_tmp5B;struct Cyc_List_List*atts=_tmp5C;struct Cyc_Absyn_Exp*req=_tmp5D;struct Cyc_List_List*req_rlns=_tmp5E;struct Cyc_Absyn_Exp*ens=_tmp5F;struct Cyc_List_List*ens_rlns=_tmp60;struct Cyc_Absyn_Vardecl*ret_val=_tmp61;
# 346
struct Cyc_List_List*_tmp6D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp6D;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmp6E=Cyc_Tcutil_copy_type(rt);void*rt2=_tmp6E;
struct Cyc_List_List*_tmp6F=((struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmp6F;
int _tmp70=c_varargs;int c_varargs2=_tmp70;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=cyc_varargs == 0?0:({struct Cyc_Absyn_VarargInfo*_tmp79=_cycalloc(sizeof(*_tmp79));*_tmp79=*cyc_varargs;_tmp79;});
if(cyc_varargs != 0)
({void*_tmp550=Cyc_Tcutil_copy_type(cyc_varargs->type);((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_varargs2))->type=_tmp550;});{
struct Cyc_List_List*_tmp71=((struct Cyc_List_List*(*)(struct _tuple15*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmp71;
struct Cyc_List_List*_tmp72=atts;struct Cyc_List_List*atts2=_tmp72;
struct Cyc_Absyn_Exp*_tmp73=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmp73;
struct Cyc_List_List*_tmp74=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmp74;
struct Cyc_Absyn_Exp*_tmp75=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmp75;
struct Cyc_List_List*_tmp76=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmp76;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmp77=_cycalloc(sizeof(*_tmp77));*_tmp77=*ret_val;_tmp77;});
({void*_tmp551=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp551;});
if((unsigned)ret_val2->rgn)({void*_tmp552=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp552;});}
# 366
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=5,(_tmp78->f1).tvars=tvs2,(_tmp78->f1).effect=effopt2,(_tmp78->f1).ret_tqual=rt_tq,(_tmp78->f1).ret_type=rt2,(_tmp78->f1).args=args2,(_tmp78->f1).c_varargs=c_varargs2,(_tmp78->f1).cyc_varargs=cyc_varargs2,(_tmp78->f1).rgn_po=rgn_po2,(_tmp78->f1).attributes=atts2,(_tmp78->f1).requires_clause=req2,(_tmp78->f1).requires_relns=req_rlns2,(_tmp78->f1).ensures_clause=ens2,(_tmp78->f1).ensures_relns=ens_rlns2,(_tmp78->f1).return_value=ret_val2;_tmp78;});}}case 6: _tmp53=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52)->f1;{struct Cyc_List_List*tqts=_tmp53;
# 369
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=6,({struct Cyc_List_List*_tmp553=((struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmp7A->f1=_tmp553;});_tmp7A;});}case 7: _tmp56=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52)->f1;_tmp53=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52)->f2;{enum Cyc_Absyn_AggrKind k=_tmp56;struct Cyc_List_List*fs=_tmp53;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=7,_tmp7B->f1=k,({struct Cyc_List_List*_tmp554=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmp7B->f2=_tmp554;});_tmp7B;});}case 9: _tmp53=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp52)->f1;{struct Cyc_Absyn_Exp*e=_tmp53;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=9,_tmp7C->f1=e;_tmp7C;});}case 11: _tmp53=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp52)->f1;{struct Cyc_Absyn_Exp*e=_tmp53;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=11,_tmp7D->f1=e;_tmp7D;});}case 8: _tmp53=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f1;_tmp54=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f2;_tmp55=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f3;{struct _tuple1*tdn=_tmp53;struct Cyc_List_List*ts=_tmp54;struct Cyc_Absyn_Typedefdecl*td=_tmp55;
# 374
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=8,_tmp7E->f1=tdn,({struct Cyc_List_List*_tmp555=((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp7E->f2=_tmp555;}),_tmp7E->f3=td,_tmp7E->f4=0;_tmp7E;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f1)->r)){case 0: _tmp53=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp53;
# 377
struct Cyc_List_List*_tmp7F=((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmp7F;
return({union Cyc_Absyn_AggrInfo _tmp556=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp556,targs);});}case 1: _tmp53=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp53;
# 380
return Cyc_Absyn_enum_type(ed->name,0);}default: _tmp53=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp53;
# 382
struct Cyc_List_List*_tmp80=((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmp80;
return({union Cyc_Absyn_DatatypeInfo _tmp557=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp4C3;_tmp4C3.name=dd->name,_tmp4C3.is_extensible=0;_tmp4C3;}));Cyc_Absyn_datatype_type(_tmp557,targs);});}}};}
# 388
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmp81=d;void*_tmp82;if(*((int*)_tmp81)== 0){_tmp82=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp81)->f1;{struct Cyc_Absyn_Exp*e=_tmp82;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=0,({struct Cyc_Absyn_Exp*_tmp558=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmp83->f1=_tmp558;});_tmp83;});}}else{
return d;};}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 394
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 396
return({struct _tuple16*_tmp84=_cycalloc(sizeof(*_tmp84));({struct Cyc_List_List*_tmp55A=((struct Cyc_List_List*(*)(void*(*)(int,void*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmp84->f1=_tmp55A;}),({
struct Cyc_Absyn_Exp*_tmp559=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmp84->f2=_tmp559;});_tmp84;});}
# 400
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*(*_tmp85)(int,struct Cyc_Absyn_Exp*)=Cyc_Tcutil_deep_copy_exp;struct Cyc_Absyn_Exp*(*DC)(int,struct Cyc_Absyn_Exp*)=_tmp85;
struct Cyc_Absyn_Exp*new_e;
int _tmp86=preserve_types;int pt=_tmp86;
{void*_tmp87=e->r;void*_stmttmp17=_tmp87;void*_tmp88=_stmttmp17;enum Cyc_Absyn_Coercion _tmp92;int _tmp8F;int _tmp8E;void*_tmp93;struct Cyc_Absyn_Tqual _tmp90;void*_tmp91;int _tmp8A;void*_tmp8D;void*_tmp8C;enum Cyc_Absyn_Incrementor _tmp94;enum Cyc_Absyn_Primop _tmp95;struct _fat_ptr _tmp8B;void*_tmp89;union Cyc_Absyn_Cnst _tmp96;switch(*((int*)_tmp88)){case 0: _tmp96=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{union Cyc_Absyn_Cnst c=_tmp96;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1: _tmp89=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{void*b=_tmp89;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2: _tmp8B=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct _fat_ptr p=_tmp8B;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3: _tmp95=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp89=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{enum Cyc_Absyn_Primop p=_tmp95;struct Cyc_List_List*es=_tmp89;
new_e=({enum Cyc_Absyn_Primop _tmp55C=p;struct Cyc_List_List*_tmp55B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_primop_exp(_tmp55C,_tmp55B,e->loc);});goto _LL0;}case 5: _tmp89=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp94=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;enum Cyc_Absyn_Incrementor i=_tmp94;
new_e=({struct Cyc_Absyn_Exp*_tmp55E=DC(pt,e1);enum Cyc_Absyn_Incrementor _tmp55D=i;Cyc_Absyn_increment_exp(_tmp55E,_tmp55D,e->loc);});goto _LL0;}case 7: _tmp89=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_Absyn_Exp*e2=_tmp8C;
new_e=({struct Cyc_Absyn_Exp*_tmp560=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp55F=DC(pt,e2);Cyc_Absyn_and_exp(_tmp560,_tmp55F,e->loc);});goto _LL0;}case 8: _tmp89=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_Absyn_Exp*e2=_tmp8C;
new_e=({struct Cyc_Absyn_Exp*_tmp562=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp561=DC(pt,e2);Cyc_Absyn_or_exp(_tmp562,_tmp561,e->loc);});goto _LL0;}case 9: _tmp89=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_Absyn_Exp*e2=_tmp8C;
new_e=({struct Cyc_Absyn_Exp*_tmp564=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp563=DC(pt,e2);Cyc_Absyn_seq_exp(_tmp564,_tmp563,e->loc);});goto _LL0;}case 6: _tmp89=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp88)->f3;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_Absyn_Exp*e2=_tmp8C;struct Cyc_Absyn_Exp*e3=_tmp8D;
# 414
new_e=({struct Cyc_Absyn_Exp*_tmp567=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp566=DC(pt,e2);struct Cyc_Absyn_Exp*_tmp565=DC(pt,e3);Cyc_Absyn_conditional_exp(_tmp567,_tmp566,_tmp565,e->loc);});goto _LL0;}case 4: _tmp89=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp88)->f3;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_Core_Opt*po=_tmp8C;struct Cyc_Absyn_Exp*e2=_tmp8D;
# 416
new_e=({struct Cyc_Absyn_Exp*_tmp56A=DC(pt,e1);struct Cyc_Core_Opt*_tmp569=(unsigned)po?({struct Cyc_Core_Opt*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->v=(void*)po->v;_tmp97;}): 0;struct Cyc_Absyn_Exp*_tmp568=DC(pt,e2);Cyc_Absyn_assignop_exp(_tmp56A,_tmp569,_tmp568,e->loc);});
goto _LL0;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f3 != 0){_tmp89=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f3)->num_varargs;_tmp8D=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f3)->injectors;_tmp91=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f3)->vai)->name;_tmp90=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f3)->vai)->tq;_tmp93=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f3)->vai)->type;_tmp8E=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f3)->vai)->inject;_tmp8F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f4;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_List_List*es=_tmp8C;int n=_tmp8A;struct Cyc_List_List*is=_tmp8D;struct _fat_ptr*nm=_tmp91;struct Cyc_Absyn_Tqual tq=_tmp90;void*t=_tmp93;int i=_tmp8E;int resolved=_tmp8F;
# 419
new_e=({void*_tmp570=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=10,({
struct Cyc_Absyn_Exp*_tmp56F=DC(pt,e1);_tmp9A->f1=_tmp56F;}),({struct Cyc_List_List*_tmp56E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);_tmp9A->f2=_tmp56E;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp56D=({struct Cyc_Absyn_VarargCallInfo*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->num_varargs=n,_tmp99->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp56C=({struct Cyc_Absyn_VarargInfo*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->name=nm,_tmp98->tq=tq,({void*_tmp56B=Cyc_Tcutil_copy_type(t);_tmp98->type=_tmp56B;}),_tmp98->inject=i;_tmp98;});_tmp99->vai=_tmp56C;});_tmp99;});
# 421
_tmp9A->f3=_tmp56D;}),_tmp9A->f4=resolved;_tmp9A;});
# 419
Cyc_Absyn_new_exp(_tmp570,e->loc);});
# 424
goto _LL0;}}else{_tmp89=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f3;_tmp8A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88)->f4;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_List_List*es=_tmp8C;struct Cyc_Absyn_VarargCallInfo*vci=_tmp8D;int resolved=_tmp8A;
# 426
new_e=({void*_tmp573=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=10,({struct Cyc_Absyn_Exp*_tmp572=DC(pt,e1);_tmp9B->f1=_tmp572;}),({struct Cyc_List_List*_tmp571=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);_tmp9B->f2=_tmp571;}),_tmp9B->f3=vci,_tmp9B->f4=resolved;_tmp9B;});Cyc_Absyn_new_exp(_tmp573,e->loc);});
goto _LL0;}}case 11: _tmp89=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;int b=_tmp8A;
# 429
new_e=b?({struct Cyc_Absyn_Exp*_tmp575=DC(pt,e1);Cyc_Absyn_rethrow_exp(_tmp575,e->loc);}):({struct Cyc_Absyn_Exp*_tmp574=DC(pt,e1);Cyc_Absyn_throw_exp(_tmp574,e->loc);});
goto _LL0;}case 12: _tmp89=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct Cyc_Absyn_Exp*e1=_tmp89;
# 432
new_e=({struct Cyc_Absyn_Exp*_tmp576=DC(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp576,e->loc);});goto _LL0;}case 13: _tmp89=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_List_List*ts=_tmp8C;
# 434
new_e=({struct Cyc_Absyn_Exp*_tmp578=DC(pt,e1);struct Cyc_List_List*_tmp577=((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp578,_tmp577,e->loc);});goto _LL0;}case 14: _tmp89=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp88)->f3;_tmp92=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp88)->f4;{void*t=_tmp89;struct Cyc_Absyn_Exp*e1=_tmp8C;int b=_tmp8A;enum Cyc_Absyn_Coercion c=_tmp92;
# 436
new_e=({void*_tmp57C=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp57B=DC(pt,e1);int _tmp57A=b;enum Cyc_Absyn_Coercion _tmp579=c;Cyc_Absyn_cast_exp(_tmp57C,_tmp57B,_tmp57A,_tmp579,e->loc);});goto _LL0;}case 16: _tmp89=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*eo=_tmp89;struct Cyc_Absyn_Exp*e1=_tmp8C;
# 438
new_e=({struct Cyc_Absyn_Exp*_tmp57E=(unsigned)eo?DC(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp57D=DC(pt,e1);Cyc_Absyn_New_exp(_tmp57E,_tmp57D,e->loc);});goto _LL0;}case 15: _tmp89=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct Cyc_Absyn_Exp*e1=_tmp89;
new_e=({struct Cyc_Absyn_Exp*_tmp57F=DC(pt,e1);Cyc_Absyn_address_exp(_tmp57F,e->loc);});goto _LL0;}case 17: _tmp89=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{void*t=_tmp89;
new_e=({void*_tmp580=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp580,e->loc);});goto _LL0;}case 18: _tmp89=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct Cyc_Absyn_Exp*e1=_tmp89;
new_e=({struct Cyc_Absyn_Exp*_tmp581=DC(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp581,e->loc);});goto _LL0;}case 19: _tmp89=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{void*t=_tmp89;struct Cyc_List_List*ofs=_tmp8C;
new_e=({void*_tmp583=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp582=ofs;Cyc_Absyn_offsetof_exp(_tmp583,_tmp582,e->loc);});goto _LL0;}case 20: _tmp89=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct Cyc_Absyn_Exp*e1=_tmp89;
new_e=({struct Cyc_Absyn_Exp*_tmp584=DC(pt,e1);Cyc_Absyn_deref_exp(_tmp584,e->loc);});goto _LL0;}case 41: _tmp89=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct Cyc_Absyn_Exp*e1=_tmp89;
new_e=({struct Cyc_Absyn_Exp*_tmp585=DC(pt,e1);Cyc_Absyn_extension_exp(_tmp585,e->loc);});goto _LL0;}case 24: _tmp89=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct Cyc_List_List*es=_tmp89;
new_e=({struct Cyc_List_List*_tmp586=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_tuple_exp(_tmp586,e->loc);});goto _LL0;}case 21: _tmp89=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp88)->f3;_tmp8E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp88)->f4;{struct Cyc_Absyn_Exp*e1=_tmp89;struct _fat_ptr*n=_tmp8C;int f1=_tmp8A;int f2=_tmp8E;
# 447
new_e=({void*_tmp588=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=21,({struct Cyc_Absyn_Exp*_tmp587=DC(pt,e1);_tmp9C->f1=_tmp587;}),_tmp9C->f2=n,_tmp9C->f3=f1,_tmp9C->f4=f2;_tmp9C;});Cyc_Absyn_new_exp(_tmp588,e->loc);});goto _LL0;}case 22: _tmp89=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp88)->f3;_tmp8E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp88)->f4;{struct Cyc_Absyn_Exp*e1=_tmp89;struct _fat_ptr*n=_tmp8C;int f1=_tmp8A;int f2=_tmp8E;
# 449
new_e=({void*_tmp58A=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=22,({struct Cyc_Absyn_Exp*_tmp589=DC(pt,e1);_tmp9D->f1=_tmp589;}),_tmp9D->f2=n,_tmp9D->f3=f1,_tmp9D->f4=f2;_tmp9D;});Cyc_Absyn_new_exp(_tmp58A,e->loc);});goto _LL0;}case 23: _tmp89=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_Absyn_Exp*e2=_tmp8C;
# 451
new_e=({struct Cyc_Absyn_Exp*_tmp58C=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp58B=DC(pt,e2);Cyc_Absyn_subscript_exp(_tmp58C,_tmp58B,e->loc);});goto _LL0;}case 26: _tmp89=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct Cyc_List_List*eds=_tmp89;
# 453
new_e=({void*_tmp58E=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=26,({struct Cyc_List_List*_tmp58D=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp9E->f1=_tmp58D;});_tmp9E;});Cyc_Absyn_new_exp(_tmp58E,e->loc);});goto _LL0;}case 31: _tmp89=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp88)->f3;{struct Cyc_List_List*es=_tmp89;struct Cyc_Absyn_Datatypedecl*dtd=_tmp8C;struct Cyc_Absyn_Datatypefield*dtf=_tmp8D;
# 455
new_e=({void*_tmp590=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=31,({struct Cyc_List_List*_tmp58F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c)(DC,pt,es);_tmp9F->f1=_tmp58F;}),_tmp9F->f2=dtd,_tmp9F->f3=dtf;_tmp9F;});Cyc_Absyn_new_exp(_tmp590,e->loc);});goto _LL0;}case 27: _tmp89=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp88)->f3;_tmp8A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp88)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmp89;struct Cyc_Absyn_Exp*e1=_tmp8C;struct Cyc_Absyn_Exp*e2=_tmp8D;int b=_tmp8A;
# 457
new_e=({void*_tmp593=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=27,_tmpA0->f1=vd,({struct Cyc_Absyn_Exp*_tmp592=DC(pt,e1);_tmpA0->f2=_tmp592;}),({struct Cyc_Absyn_Exp*_tmp591=DC(pt,e2);_tmpA0->f3=_tmp591;}),_tmpA0->f4=b;_tmpA0;});Cyc_Absyn_new_exp(_tmp593,e->loc);});goto _LL0;}case 28: _tmp89=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp88)->f3;{struct Cyc_Absyn_Exp*e=_tmp89;void*t=_tmp8C;int b=_tmp8A;
# 459
new_e=({void*_tmp596=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=28,({struct Cyc_Absyn_Exp*_tmp595=DC(pt,e);_tmpA1->f1=_tmp595;}),({void*_tmp594=Cyc_Tcutil_copy_type(t);_tmpA1->f2=_tmp594;}),_tmpA1->f3=b;_tmpA1;});Cyc_Absyn_new_exp(_tmp596,e->loc);});
goto _LL0;}case 29: _tmp89=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp8D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp88)->f3;_tmp91=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp88)->f4;{struct _tuple1*n=_tmp89;struct Cyc_List_List*ts=_tmp8C;struct Cyc_List_List*eds=_tmp8D;struct Cyc_Absyn_Aggrdecl*agr=_tmp91;
# 462
new_e=({void*_tmp599=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=29,_tmpA2->f1=n,({struct Cyc_List_List*_tmp598=((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmpA2->f2=_tmp598;}),({struct Cyc_List_List*_tmp597=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmpA2->f3=_tmp597;}),_tmpA2->f4=agr;_tmpA2;});Cyc_Absyn_new_exp(_tmp599,e->loc);});
# 464
goto _LL0;}case 30: _tmp89=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{void*t=_tmp89;struct Cyc_List_List*eds=_tmp8C;
# 466
new_e=({void*_tmp59C=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=30,({void*_tmp59B=Cyc_Tcutil_copy_type(t);_tmpA3->f1=_tmp59B;}),({struct Cyc_List_List*_tmp59A=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmpA3->f2=_tmp59A;});_tmpA3;});Cyc_Absyn_new_exp(_tmp59C,e->loc);});
goto _LL0;}case 25: _tmp89=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp88)->f1)->f1;_tmp90=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp88)->f1)->f2;_tmp8C=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp88)->f1)->f3;_tmp8D=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct _fat_ptr*vopt=_tmp89;struct Cyc_Absyn_Tqual tq=_tmp90;void*t=_tmp8C;struct Cyc_List_List*eds=_tmp8D;
# 469
new_e=({void*_tmp5A0=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=25,({struct _tuple9*_tmp59F=({struct _tuple9*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=vopt,_tmpA4->f2=tq,({void*_tmp59E=Cyc_Tcutil_copy_type(t);_tmpA4->f3=_tmp59E;});_tmpA4;});_tmpA5->f1=_tmp59F;}),({
struct Cyc_List_List*_tmp59D=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmpA5->f2=_tmp59D;});_tmpA5;});
# 469
Cyc_Absyn_new_exp(_tmp5A0,e->loc);});
# 471
goto _LL0;}case 32: _tmp89=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp89;struct Cyc_Absyn_Enumfield*ef=_tmp8C;
new_e=e;goto _LL0;}case 33: _tmp89=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{void*t=_tmp89;struct Cyc_Absyn_Enumfield*ef=_tmp8C;
# 474
new_e=({void*_tmp5A2=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->tag=33,({void*_tmp5A1=Cyc_Tcutil_copy_type(t);_tmpA6->f1=_tmp5A1;}),_tmpA6->f2=ef;_tmpA6;});Cyc_Absyn_new_exp(_tmp5A2,e->loc);});goto _LL0;}case 34: _tmp8A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp88)->f1).is_calloc;_tmp89=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp88)->f1).rgn;_tmp8C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp88)->f1).elt_type;_tmp8D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp88)->f1).num_elts;_tmp8E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp88)->f1).fat_result;_tmp8F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp88)->f1).inline_call;{int ic=_tmp8A;struct Cyc_Absyn_Exp*r=_tmp89;void**t=_tmp8C;struct Cyc_Absyn_Exp*n=_tmp8D;int res=_tmp8E;int inlc=_tmp8F;
# 476
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=DC(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmpA7=_cycalloc(sizeof(*_tmpA7));({void*_tmp5A3=Cyc_Tcutil_copy_type(*t);*_tmpA7=_tmp5A3;});_tmpA7;});
({void*_tmp5A4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=34,(_tmpA8->f1).is_calloc=ic,(_tmpA8->f1).rgn=r1,(_tmpA8->f1).elt_type=t1,(_tmpA8->f1).num_elts=n,(_tmpA8->f1).fat_result=res,(_tmpA8->f1).inline_call=inlc;_tmpA8;});e2->r=_tmp5A4;});
new_e=e2;
goto _LL0;}}case 35: _tmp89=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_Absyn_Exp*e2=_tmp8C;
new_e=({struct Cyc_Absyn_Exp*_tmp5A6=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp5A5=DC(pt,e2);Cyc_Absyn_swap_exp(_tmp5A6,_tmp5A5,e->loc);});goto _LL0;}case 36: _tmp89=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Core_Opt*nopt=_tmp89;struct Cyc_List_List*eds=_tmp8C;
# 484
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->v=(struct _tuple1*)nopt->v;_tmpA9;});
new_e=({void*_tmp5A8=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=36,_tmpAA->f1=nopt1,({struct Cyc_List_List*_tmp5A7=((struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmpAA->f2=_tmp5A7;});_tmpAA;});Cyc_Absyn_new_exp(_tmp5A8,e->loc);});
goto _LL0;}case 37:
# 489
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp5A9=({const char*_tmpAB="deep_copy: statement expressions unsupported";_tag_fat(_tmpAB,sizeof(char),45U);});_tmpAC->f1=_tmp5A9;});_tmpAC;}));case 38: _tmp89=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp88)->f2;{struct Cyc_Absyn_Exp*e1=_tmp89;struct _fat_ptr*fn=_tmp8C;
# 491
new_e=({void*_tmp5AB=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=38,({struct Cyc_Absyn_Exp*_tmp5AA=DC(pt,e1);_tmpAD->f1=_tmp5AA;}),_tmpAD->f2=fn;_tmpAD;});Cyc_Absyn_new_exp(_tmp5AB,e->loc);});goto _LL0;}case 39: _tmp89=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{void*t=_tmp89;
new_e=({void*_tmp5AC=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp5AC,e->loc);});goto _LL0;}case 40: _tmp8A=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp88)->f1;_tmp8B=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp88)->f2;_tmp89=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp88)->f3;_tmp8C=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp88)->f4;_tmp8D=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp88)->f5;{int v=_tmp8A;struct _fat_ptr t=_tmp8B;struct Cyc_List_List*o=_tmp89;struct Cyc_List_List*i=_tmp8C;struct Cyc_List_List*c=_tmp8D;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _tmp89=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp88)->f1;{struct Cyc_Absyn_Exp*e1=_tmp89;
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
struct _RegionHandle _tmpAE=_new_region("r");struct _RegionHandle*r=& _tmpAE;_push_region(r);
{struct Cyc_List_List*_tmpAF=0;struct Cyc_List_List*inst=_tmpAF;
# 515
for(1;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmpB0=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmpB0;
void*_tmpB1=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmpB1;
enum Cyc_Absyn_KindQual _tmpB2=(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk))->kind;enum Cyc_Absyn_KindQual _stmttmp18=_tmpB2;enum Cyc_Absyn_KindQual _tmpB3=_stmttmp18;switch((int)_tmpB3){case Cyc_Absyn_IntKind:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL4:
 inst=({struct Cyc_List_List*_tmpB5=_region_malloc(r,sizeof(*_tmpB5));({struct _tuple17*_tmp5AD=({struct _tuple17*_tmpB4=_region_malloc(r,sizeof(*_tmpB4));_tmpB4->f1=tv,_tmpB4->f2=t;_tmpB4;});_tmpB5->hd=_tmp5AD;}),_tmpB5->tl=inst;_tmpB5;});goto _LL0;default:
 goto _LL0;}_LL0:;}
# 524
if(inst != 0){
field_type=({struct _RegionHandle*_tmp5AF=r;struct Cyc_List_List*_tmp5AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp5AF,_tmp5AE,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 513
;_pop_region();}
# 529
return k;}
# 536
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 538
void*_tmpB6=Cyc_Absyn_compress(t);void*_stmttmp19=_tmpB6;void*_tmpB7=_stmttmp19;struct Cyc_Absyn_PtrInfo _tmpB9;void*_tmpBA;void*_tmpB8;switch(*((int*)_tmpB7)){case 1: _tmpB8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB7)->f1;{struct Cyc_Core_Opt*k=_tmpB8;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2: _tmpB8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB7)->f1;{struct Cyc_Absyn_Tvar*tv=_tmpB8;
return Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);}case 0: _tmpB8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB7)->f1;_tmpBA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB7)->f2;{void*c=_tmpB8;struct Cyc_List_List*ts=_tmpBA;
# 542
void*_tmpBB=c;int _tmpBF;void*_tmpBE;enum Cyc_Absyn_AggrKind _tmpBC;void*_tmpBD;enum Cyc_Absyn_Size_of _tmpC0;switch(*((int*)_tmpBB)){case 0:
 return& Cyc_Kinds_mk;case 1: _tmpC0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmpBB)->f2;{enum Cyc_Absyn_Size_of sz=_tmpC0;
# 545
return((int)sz == 2 ||(int)sz == 3)?& Cyc_Kinds_bk:& Cyc_Kinds_mk;}case 2:
 return& Cyc_Kinds_mk;case 15:
 goto _LL27;case 16: _LL27:
 goto _LL29;case 3: _LL29:
 return& Cyc_Kinds_bk;case 6:
 return& Cyc_Kinds_urk;case 5:
 return& Cyc_Kinds_rk;case 7:
 return& Cyc_Kinds_trk;case 17: _tmpBD=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmpBB)->f2;{struct Cyc_Absyn_Kind*k=_tmpBD;
return k;}case 4:
 return& Cyc_Kinds_bk;case 8:
 goto _LL37;case 9: _LL37:
 goto _LL39;case 10: _LL39:
 return& Cyc_Kinds_ek;case 12:
 goto _LL3D;case 11: _LL3D:
 return& Cyc_Kinds_boolk;case 13:
 goto _LL41;case 14: _LL41:
 return& Cyc_Kinds_ptrbk;case 18:
 return& Cyc_Kinds_ak;case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmpBB)->f1).KnownDatatypefield).tag == 2)
return& Cyc_Kinds_mk;else{
# 565
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C4;_tmp4C4.tag=0,({struct _fat_ptr _tmp5B0=({const char*_tmpC3="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmpC3,sizeof(char),40U);});_tmp4C4.f1=_tmp5B0;});_tmp4C4;});void*_tmpC1[1];_tmpC1[0]=& _tmpC2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpC1,sizeof(void*),1));});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpBB)->f1).UnknownAggr).tag == 1)
# 568
return& Cyc_Kinds_ak;else{_tmpBC=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpBB)->f1).KnownAggr).val)->kind;_tmpBD=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpBB)->f1).KnownAggr).val)->tvs;_tmpBE=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpBB)->f1).KnownAggr).val)->impl;_tmpBF=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpBB)->f1).KnownAggr).val)->expected_mem_kind;{enum Cyc_Absyn_AggrKind strOrU=_tmpBC;struct Cyc_List_List*tvs=_tmpBD;struct Cyc_Absyn_AggrdeclImpl*i=_tmpBE;int expected_mem_kind=_tmpBF;
# 570
if(i == 0)
return expected_mem_kind?& Cyc_Kinds_mk:& Cyc_Kinds_ak;{
struct Cyc_List_List*_tmpC4=i->fields;struct Cyc_List_List*fields=_tmpC4;
if(fields == 0)return& Cyc_Kinds_mk;
# 575
if((int)strOrU == 0){
for(1;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){
;}{
void*_tmpC5=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmpC5;
struct Cyc_Absyn_Kind*_tmpC6=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmpC6;
if(k == & Cyc_Kinds_ak || k == & Cyc_Kinds_tak)return k;}}else{
# 584
for(1;fields != 0;fields=fields->tl){
void*_tmpC7=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmpC7;
struct Cyc_Absyn_Kind*_tmpC8=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmpC8;
if(k == & Cyc_Kinds_ak || k == & Cyc_Kinds_tak)return k;}}
# 589
return& Cyc_Kinds_mk;}}}};}case 5:
# 591
 return& Cyc_Kinds_ak;case 7:
 return& Cyc_Kinds_mk;case 3: _tmpB9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB7)->f1;{struct Cyc_Absyn_PtrInfo pinfo=_tmpB9;
# 594
void*_tmpC9=Cyc_Absyn_compress((pinfo.ptr_atts).bounds);void*_stmttmp1A=_tmpC9;void*_tmpCA=_stmttmp1A;if(*((int*)_tmpCA)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->f1)){case 13:  {
# 596
enum Cyc_Absyn_AliasQual _tmpCB=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp1B=_tmpCB;enum Cyc_Absyn_AliasQual _tmpCC=_stmttmp1B;switch((int)_tmpCC){case Cyc_Absyn_Aliasable:
 return& Cyc_Kinds_bk;case Cyc_Absyn_Unique:
 return& Cyc_Kinds_ubk;case Cyc_Absyn_Top:
 goto _LL5B;default: _LL5B: return& Cyc_Kinds_tbk;};}case 14:
# 602
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 604
enum Cyc_Absyn_AliasQual _tmpCD=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp1C=_tmpCD;enum Cyc_Absyn_AliasQual _tmpCE=_stmttmp1C;switch((int)_tmpCE){case Cyc_Absyn_Aliasable:
 return& Cyc_Kinds_mk;case Cyc_Absyn_Unique:
 return& Cyc_Kinds_umk;case Cyc_Absyn_Top:
 goto _LL64;default: _LL64: return& Cyc_Kinds_tmk;};}};}case 9:
# 610
 return& Cyc_Kinds_ik;case 11:
# 614
 return& Cyc_Kinds_ak;case 4: _tmpB8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB7)->f1).num_elts;{struct Cyc_Absyn_Exp*num_elts=_tmpB8;
# 616
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Kinds_mk;
return& Cyc_Kinds_ak;}case 6:
 return& Cyc_Kinds_mk;case 8: _tmpB8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB7)->f3;{struct Cyc_Absyn_Typedefdecl*td=_tmpB8;
# 620
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C6;_tmp4C6.tag=0,({struct _fat_ptr _tmp5B1=({const char*_tmpD2="type_kind: typedef found: ";_tag_fat(_tmpD2,sizeof(char),27U);});_tmp4C6.f1=_tmp5B1;});_tmp4C6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4C5;_tmp4C5.tag=2,_tmp4C5.f1=(void*)t;_tmp4C5;});void*_tmpCF[2];_tmpCF[0]=& _tmpD0,_tmpCF[1]=& _tmpD1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpCF,sizeof(void*),2));});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpB7)->f1)->r)){case 1:
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
{struct Cyc_Absyn_Kind*_tmpD3=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_stmttmp1D=_tmpD3;struct Cyc_Absyn_Kind*_tmpD4=_stmttmp1D;switch((int)((struct Cyc_Absyn_Kind*)_tmpD4)->kind){case Cyc_Absyn_RgnKind: switch((int)((struct Cyc_Absyn_Kind*)_tmpD4)->aliasqual){case Cyc_Absyn_Unique:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->tag=9,({struct Cyc_Absyn_Exp*_tmp5B2=Cyc_Absyn_uint_exp(0U,0U);_tmpD5->f1=_tmp5B2;});_tmpD5;});goto _LL0;case Cyc_Absyn_BoolKind:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 646
return({struct _tuple17*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->f1=tv,_tmpD6->f2=t;_tmpD6;});}
# 653
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmpD7=Cyc_Absyn_compress(t);void*_stmttmp1E=_tmpD7;void*_tmpD8=_stmttmp1E;void*_tmpDF;void*_tmpDE;void*_tmpDD;void*_tmpDC;struct Cyc_Absyn_Tqual _tmpDB;void*_tmpDA;void*_tmpD9;switch(*((int*)_tmpD8)){case 0: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f2 == 0)
return Cyc_Absyn_empty_effect;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)== 9)
return t;else{_tmpD9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f2;{struct Cyc_List_List*ts=_tmpD9;
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}}}case 1:
 goto _LLA;case 2: _LLA: {
# 660
struct Cyc_Absyn_Kind*_tmpE0=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp1F=_tmpE0;struct Cyc_Absyn_Kind*_tmpE1=_stmttmp1F;switch((int)((struct Cyc_Absyn_Kind*)_tmpE1)->kind){case Cyc_Absyn_RgnKind:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind:
 return t;case Cyc_Absyn_IntKind:
 return Cyc_Absyn_empty_effect;default:
 return Cyc_Absyn_regionsof_eff(t);};}case 3: _tmpD9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD8)->f1).elt_type;_tmpDA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD8)->f1).ptr_atts).rgn;{void*et=_tmpD9;void*r=_tmpDA;
# 668
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmpE2[2];({void*_tmp5B4=Cyc_Absyn_access_eff(r);_tmpE2[0]=_tmp5B4;}),({void*_tmp5B3=Cyc_Tcutil_rgns_of(et);_tmpE2[1]=_tmp5B3;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpE2,sizeof(void*),2));})));}case 4: _tmpD9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).elt_type;{void*et=_tmpD9;
# 670
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7: _tmpD9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD8)->f2;{struct Cyc_List_List*fs=_tmpD9;
# 672
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5: _tmpD9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD8)->f1).tvars;_tmpDA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD8)->f1).effect;_tmpDB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD8)->f1).ret_tqual;_tmpDC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD8)->f1).ret_type;_tmpDD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD8)->f1).args;_tmpDE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD8)->f1).cyc_varargs;_tmpDF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD8)->f1).rgn_po;{struct Cyc_List_List*tvs=_tmpD9;void*eff=_tmpDA;struct Cyc_Absyn_Tqual rt_tq=_tmpDB;void*rt=_tmpDC;struct Cyc_List_List*args=_tmpDD;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmpDE;struct Cyc_List_List*rpo=_tmpDF;
# 681
void*_tmpE3=({struct Cyc_List_List*_tmp5B5=((struct Cyc_List_List*(*)(struct _tuple17*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp5B5,(void*)_check_null(eff));});void*e=_tmpE3;
return Cyc_Tcutil_normalize_effect(e);}case 6: _tmpD9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD8)->f1;{struct Cyc_List_List*tqts=_tmpD9;
# 684
struct Cyc_List_List*_tmpE4=0;struct Cyc_List_List*ts=_tmpE4;
for(1;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmpE5->tl=ts;_tmpE5;});}
_tmpD9=ts;goto _LL16;}case 8: _tmpD9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD8)->f2;_LL16: {struct Cyc_List_List*ts=_tmpD9;
# 689
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C7;_tmp4C7.tag=0,({struct _fat_ptr _tmp5B6=({const char*_tmpE8="typedecl in rgns_of";_tag_fat(_tmpE8,sizeof(char),20U);});_tmp4C7.f1=_tmp5B6;});_tmp4C7;});void*_tmpE6[1];_tmpE6[0]=& _tmpE7;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpE6,sizeof(void*),1));});case 9:
 goto _LL1C;default: _LL1C:
 return Cyc_Absyn_empty_effect;};}
# 699
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Absyn_compress(e);{
void*_tmpE9=e;void*_tmpEA;if(*((int*)_tmpE9)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE9)->f1)){case 9: _tmpEA=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE9)->f2;{struct Cyc_List_List**es=_tmpEA;
# 703
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmpEB=(void*)effs->hd;void*eff=_tmpEB;
({void*_tmp5B7=(void*)Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp5B7;});{
void*_tmpEC=(void*)effs->hd;void*_stmttmp20=_tmpEC;void*_tmpED=_stmttmp20;if(*((int*)_tmpED)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpED)->f1)){case 9:
 goto _LLB;case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpED)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpED)->f2)->hd)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpED)->f2)->hd)->f1)){case 5: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpED)->f2)->tl == 0){_LLB:
 goto _LLD;}else{goto _LL10;}case 7: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpED)->f2)->tl == 0){_LLD:
 goto _LLF;}else{goto _LL10;}case 6: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpED)->f2)->tl == 0){_LLF:
# 712
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10:
 goto _LL7;}_LL7:;}}}
# 716
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmpEE=Cyc_Absyn_compress((void*)effs->hd);void*_stmttmp21=_tmpEE;void*_tmpEF=_stmttmp21;void*_tmpF0;if(*((int*)_tmpEF)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEF)->f1)){case 9: _tmpF0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEF)->f2;{struct Cyc_List_List*nested_effs=_tmpF0;
# 721
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_revappend)(nested_effs,effects);goto _LL12;}case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEF)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEF)->f2)->hd)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEF)->f2)->hd)->f1)){case 5: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEF)->f2)->tl == 0)
goto _LL18;else{goto _LL1B;}case 7: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEF)->f2)->tl == 0){_LL18:
 goto _LL1A;}else{goto _LL1B;}case 6: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEF)->f2)->tl == 0){_LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmpF0=_tmpEF;{void*e=_tmpF0;
effects=({struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->hd=e,_tmpF1->tl=effects;_tmpF1;});goto _LL12;}}_LL12:;}}
# 727
({struct Cyc_List_List*_tmp5B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(effects);*es=_tmp5B8;});
return e;}}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE9)->f2 != 0){_tmpEA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE9)->f2)->hd;{void*t=_tmpEA;
# 730
void*_tmpF2=Cyc_Absyn_compress(t);void*_stmttmp22=_tmpF2;void*_tmpF3=_stmttmp22;switch(*((int*)_tmpF3)){case 1:
 goto _LL21;case 2: _LL21:
 return e;default:
 return Cyc_Tcutil_rgns_of(t);};}}else{goto _LL5;}default: goto _LL5;}else{_LL5:
# 735
 return e;};}}
# 740
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF4=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=5,(_tmpF5->f1).tvars=0,(_tmpF5->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp5B9=Cyc_Absyn_empty_tqual(0U);(_tmpF5->f1).ret_tqual=_tmp5B9;}),(_tmpF5->f1).ret_type=Cyc_Absyn_void_type,(_tmpF5->f1).args=0,(_tmpF5->f1).c_varargs=0,(_tmpF5->f1).cyc_varargs=0,(_tmpF5->f1).rgn_po=0,(_tmpF5->f1).attributes=0,(_tmpF5->f1).requires_clause=0,(_tmpF5->f1).requires_relns=0,(_tmpF5->f1).ensures_clause=0,(_tmpF5->f1).ensures_relns=0,(_tmpF5->f1).return_value=0;_tmpF5;});
# 741
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmpF4;
# 751
return({void*_tmp5BE=(void*)fntype;void*_tmp5BD=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp5BC=Cyc_Absyn_empty_tqual(0U);void*_tmp5BB=Cyc_Absyn_bounds_one();void*_tmp5BA=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_tmp5BE,_tmp5BD,_tmp5BC,_tmp5BB,_tmp5BA,Cyc_Absyn_false_type);});}
# 758
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Absyn_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmpF6=Cyc_Absyn_compress(e);void*_stmttmp23=_tmpF6;void*_tmpF7=_stmttmp23;void*_tmpFA;void*_tmpF9;void*_tmpF8;switch(*((int*)_tmpF7)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)){case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f2 != 0){_tmpF8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f2)->hd;{void*r2=_tmpF8;
# 765
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Absyn_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmpFB=({struct _tuple15 _tmp4C8;_tmp4C8.f1=r,_tmp4C8.f2=r2;_tmp4C8;});struct _tuple15 _stmttmp24=_tmpFB;struct _tuple15 _tmpFC=_stmttmp24;void*_tmpFE;void*_tmpFD;if(*((int*)_tmpFC.f1)== 2){if(*((int*)_tmpFC.f2)== 2){_tmpFD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpFC.f1)->f1;_tmpFE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpFC.f2)->f1;{struct Cyc_Absyn_Tvar*tv1=_tmpFD;struct Cyc_Absyn_Tvar*tv2=_tmpFE;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE:
 return 0;};}}}else{goto _LL9;}case 9: _tmpF8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f2;{struct Cyc_List_List*es=_tmpF8;
# 773
for(1;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f2 != 0){_tmpF8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f2)->hd;{void*t=_tmpF8;
# 777
void*_tmpFF=Cyc_Tcutil_rgns_of(t);void*_stmttmp25=_tmpFF;void*_tmp100=_stmttmp25;void*_tmp101;if(*((int*)_tmp100)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp100)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp100)->f2 != 0){_tmp101=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp100)->f2)->hd;{void*t=_tmp101;
# 779
if(!constrain)return 0;{
void*_tmp102=Cyc_Absyn_compress(t);void*_stmttmp26=_tmp102;void*_tmp103=_stmttmp26;void*_tmp106;void*_tmp105;void*_tmp104;if(*((int*)_tmp103)== 1){_tmp104=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp103)->f1;_tmp105=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp103)->f2;_tmp106=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp103)->f4;{struct Cyc_Core_Opt*k=_tmp104;void**p=(void**)_tmp105;struct Cyc_Core_Opt*s=_tmp106;
# 784
void*_tmp107=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp107;
# 787
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp5C0=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp108[2];_tmp108[0]=ev,({void*_tmp5BF=Cyc_Absyn_access_eff(r);_tmp108[1]=_tmp5BF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp108,sizeof(void*),2));})));*p=_tmp5C0;});
return 1;}}else{
return 0;};}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp101=_tmp100;{void*e2=_tmp101;
# 792
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}};}}else{goto _LL9;}default: goto _LL9;}case 1: _tmpF8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF7)->f1;_tmpF9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF7)->f2;_tmpFA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF7)->f4;{struct Cyc_Core_Opt*k=_tmpF8;void**p=(void**)_tmpF9;struct Cyc_Core_Opt*s=_tmpFA;
# 795
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != 4)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C9;_tmp4C9.tag=0,({struct _fat_ptr _tmp5C1=({const char*_tmp10B="effect evar has wrong kind";_tag_fat(_tmp10B,sizeof(char),27U);});_tmp4C9.f1=_tmp5C1;});_tmp4C9;});void*_tmp109[1];_tmp109[0]=& _tmp10A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp109,sizeof(void*),1));});
if(!constrain)return 0;{
# 800
void*_tmp10C=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp10C;
# 803
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp5C3=Cyc_Absyn_join_eff(({void*_tmp10D[2];_tmp10D[0]=ev,({void*_tmp5C2=Cyc_Absyn_access_eff(r);_tmp10D[1]=_tmp5C2;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp10D,sizeof(void*),2));}));*p=_tmp5C3;});
return 1;}}default: _LL9:
 return 0;};}}
# 813
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Absyn_compress(t);{
void*_tmp10E=Cyc_Tcutil_normalize_effect(Cyc_Absyn_compress(e));void*_stmttmp27=_tmp10E;void*_tmp10F=_stmttmp27;void*_tmp112;void*_tmp111;void*_tmp110;switch(*((int*)_tmp10F)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)){case 8:
 return 0;case 9: _tmp110=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f2;{struct Cyc_List_List*es=_tmp110;
# 818
for(1;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f2 != 0){_tmp110=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f2)->hd;{void*t2=_tmp110;
# 823
t2=Cyc_Absyn_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp113=Cyc_Tcutil_rgns_of(t);void*_stmttmp28=_tmp113;void*_tmp114=_stmttmp28;void*_tmp115;if(*((int*)_tmp114)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp114)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp114)->f2 != 0){_tmp115=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp114)->f2)->hd;{void*t3=_tmp115;
# 828
struct _tuple15 _tmp116=({struct _tuple15 _tmp4CA;({void*_tmp5C4=Cyc_Absyn_compress(t3);_tmp4CA.f1=_tmp5C4;}),_tmp4CA.f2=t2;_tmp4CA;});struct _tuple15 _stmttmp29=_tmp116;struct _tuple15 _tmp117=_stmttmp29;void*_tmp119;void*_tmp118;if(*((int*)_tmp117.f1)== 2){if(*((int*)_tmp117.f2)== 2){_tmp118=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp117.f1)->f1;_tmp119=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp117.f2)->f1;{struct Cyc_Absyn_Tvar*tv1=_tmp118;struct Cyc_Absyn_Tvar*tv2=_tmp119;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13:
 return t3 == t2;};}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp115=_tmp114;{void*e2=_tmp115;
# 832
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}};}}}else{goto _LL9;}default: goto _LL9;}case 1: _tmp110=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp10F)->f1;_tmp111=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp10F)->f2;_tmp112=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp10F)->f4;{struct Cyc_Core_Opt*k=_tmp110;void**p=(void**)_tmp111;struct Cyc_Core_Opt*s=_tmp112;
# 835
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != 4)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CB;_tmp4CB.tag=0,({struct _fat_ptr _tmp5C5=({const char*_tmp11C="effect evar has wrong kind";_tag_fat(_tmp11C,sizeof(char),27U);});_tmp4CB.f1=_tmp5C5;});_tmp4CB;});void*_tmp11A[1];_tmp11A[0]=& _tmp11B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp11A,sizeof(void*),1));});
if(!may_constrain_evars)return 0;{
# 840
void*_tmp11D=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp11D;
# 843
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp11E=Cyc_Absyn_join_eff(({void*_tmp11F[2];_tmp11F[0]=ev,({void*_tmp5C6=Cyc_Absyn_regionsof_eff(t);_tmp11F[1]=_tmp5C6;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp11F,sizeof(void*),2));}));void*new_typ=_tmp11E;
*p=new_typ;
return 1;}}}default: _LL9:
 return 0;};}}
# 853
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp120=e;void*_tmp123;void*_tmp122;void*_tmp121;switch(*((int*)_tmp120)){case 2: _tmp121=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp120)->f1;{struct Cyc_Absyn_Tvar*v2=_tmp121;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp120)->f1)){case 9: _tmp121=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp120)->f2;{struct Cyc_List_List*es=_tmp121;
# 858
for(1;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp120)->f2 != 0){_tmp121=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp120)->f2)->hd;{void*t=_tmp121;
# 863
void*_tmp124=Cyc_Tcutil_rgns_of(t);void*_stmttmp2A=_tmp124;void*_tmp125=_stmttmp2A;void*_tmp126;if(*((int*)_tmp125)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp125)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp125)->f2 != 0){_tmp126=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp125)->f2)->hd;{void*t2=_tmp126;
# 865
if(!may_constrain_evars)return 0;{
void*_tmp127=Cyc_Absyn_compress(t2);void*_stmttmp2B=_tmp127;void*_tmp128=_stmttmp2B;void*_tmp12B;void*_tmp12A;void*_tmp129;if(*((int*)_tmp128)== 1){_tmp129=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp128)->f1;_tmp12A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp128)->f2;_tmp12B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp128)->f4;{struct Cyc_Core_Opt*k=_tmp129;void**p=(void**)_tmp12A;struct Cyc_Core_Opt*s=_tmp12B;
# 871
void*_tmp12C=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp12C;
# 873
if(!((int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp5C8=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp12D[2];_tmp12D[0]=ev,({void*_tmp5C7=Cyc_Absyn_var_type(v);_tmp12D[1]=_tmp5C7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp12D,sizeof(void*),2));})));*p=_tmp5C8;});
return 1;}}else{
return 0;};}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp126=_tmp125;{void*e2=_tmp126;
# 878
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}};}}else{goto _LL9;}default: goto _LL9;}case 1: _tmp121=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp120)->f1;_tmp122=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp120)->f2;_tmp123=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp120)->f4;{struct Cyc_Core_Opt*k=_tmp121;void**p=(void**)_tmp122;struct Cyc_Core_Opt*s=_tmp123;
# 881
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != 4)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CC;_tmp4CC.tag=0,({struct _fat_ptr _tmp5C9=({const char*_tmp130="effect evar has wrong kind";_tag_fat(_tmp130,sizeof(char),27U);});_tmp4CC.f1=_tmp5C9;});_tmp4CC;});void*_tmp12E[1];_tmp12E[0]=& _tmp12F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp12E,sizeof(void*),1));});{
# 885
void*_tmp131=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp131;
# 887
if(!((int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp132=Cyc_Absyn_join_eff(({void*_tmp133[2];_tmp133[0]=ev,({void*_tmp5CA=Cyc_Absyn_var_type(v);_tmp133[1]=_tmp5CA;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp133,sizeof(void*),2));}));void*new_typ=_tmp132;
*p=new_typ;
return 1;}}}default: _LL9:
 return 0;};}}
# 897
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp134=e;void*_tmp135;switch(*((int*)_tmp134)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp134)->f1)){case 9: _tmp135=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp134)->f2;{struct Cyc_List_List*es=_tmp135;
# 901
for(1;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp134)->f2 != 0){_tmp135=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp134)->f2)->hd;{void*t=_tmp135;
# 906
void*_tmp136=Cyc_Tcutil_rgns_of(t);void*_stmttmp2C=_tmp136;void*_tmp137=_stmttmp2C;void*_tmp138;if(*((int*)_tmp137)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2 != 0){_tmp138=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2)->hd;{void*t2=_tmp138;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp138=_tmp137;{void*e2=_tmp138;
return Cyc_Tcutil_evar_in_effect(evar,e2);}};}}else{goto _LL7;}default: goto _LL7;}case 1:
# 910
 return evar == e;default: _LL7:
 return 0;};}}
# 924 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 927
void*_tmp139=Cyc_Absyn_compress(e1);void*_stmttmp2D=_tmp139;void*_tmp13A=_stmttmp2D;void*_tmp13C;void*_tmp13B;switch(*((int*)_tmp13A)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13A)->f1)){case 9: _tmp13B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13A)->f2;{struct Cyc_List_List*es=_tmp13B;
# 929
for(1;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13A)->f2 != 0){_tmp13B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13A)->f2)->hd;{void*r=_tmp13B;
# 940
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13A)->f2 != 0){_tmp13B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13A)->f2)->hd;{void*t=_tmp13B;
# 944
void*_tmp13D=Cyc_Tcutil_rgns_of(t);void*_stmttmp2E=_tmp13D;void*_tmp13E=_stmttmp2E;void*_tmp13F;if(*((int*)_tmp13E)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f2 != 0){_tmp13F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f2)->hd;{void*t2=_tmp13F;
# 949
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp13F=_tmp13E;{void*e=_tmp13F;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}};}}else{goto _LLB;}default: goto _LLB;}case 2: _tmp13B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13A)->f1;{struct Cyc_Absyn_Tvar*v=_tmp13B;
# 942
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1: _tmp13B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp13A)->f2;_tmp13C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp13A)->f4;{void**p=(void**)_tmp13B;struct Cyc_Core_Opt*s=_tmp13C;
# 954
if(Cyc_Tcutil_evar_in_effect(e1,e2))
return 0;
# 959
*p=Cyc_Absyn_empty_effect;
# 962
return 1;}default: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp141=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CE;_tmp4CE.tag=0,({struct _fat_ptr _tmp5CB=({const char*_tmp143="subset_effect: bad effect: ";_tag_fat(_tmp143,sizeof(char),28U);});_tmp4CE.f1=_tmp5CB;});_tmp4CE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp142=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4CD;_tmp4CD.tag=2,_tmp4CD.f1=(void*)e1;_tmp4CD;});void*_tmp140[2];_tmp140[0]=& _tmp141,_tmp140[1]=& _tmp142;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp140,sizeof(void*),2));});};}
# 973
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 975
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp144=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp2F=_tmp144;struct _tuple15*_tmp145=_stmttmp2F;void*_tmp147;void*_tmp146;_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;{void*t1a=_tmp146;void*t1b=_tmp147;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp148=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp30=_tmp148;struct _tuple15*_tmp149=_stmttmp30;void*_tmp14B;void*_tmp14A;_tmp14A=_tmp149->f1;_tmp14B=_tmp149->f2;{void*t2a=_tmp14A;void*t2b=_tmp14B;
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
void*_tmp14C=t;switch(*((int*)_tmp14C)){case 0:
 return 0;case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp14C)->f1){case Cyc_Absyn_Unsigned: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp14C)->f2){case Cyc_Absyn_Char_sz:
 return 1;case Cyc_Absyn_Short_sz:
# 1002
 return 4;case Cyc_Absyn_Int_sz:
# 1005
 return 7;case Cyc_Absyn_Long_sz:
# 1008
 return 7;case Cyc_Absyn_LongLong_sz:
# 1011
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp14C)->f2){case Cyc_Absyn_Char_sz:
# 1000
 return 2;case Cyc_Absyn_Short_sz:
# 1003
 return 5;case Cyc_Absyn_Int_sz:
# 1006
 return 8;case Cyc_Absyn_Long_sz:
# 1009
 return 8;case Cyc_Absyn_LongLong_sz:
# 1012
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp14C)->f2){case Cyc_Absyn_Char_sz:
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CF;_tmp4CF.tag=0,({struct _fat_ptr _tmp5CC=({const char*_tmp14F="bad con";_tag_fat(_tmp14F,sizeof(char),8U);});_tmp4CF.f1=_tmp5CC;});_tmp4CF;});void*_tmp14D[1];_tmp14D[0]=& _tmp14E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp14D,sizeof(void*),1));});}case 2: switch((int)((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp14C)->f1){case 0:
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
void*_tmp150=t;void*_tmp151;switch(*((int*)_tmp150)){case 1:
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
 return 11;default: _tmp151=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp150)->f1;{void*c=_tmp151;
return 12 + Cyc_Tcutil_tycon2int(c);}};}
# 1054
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp152=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp152;
if(qc != 0)return qc;
return((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1059
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp153=i;void*_tmp154;if((_tmp153.KnownDatatype).tag == 2){_tmp154=*(_tmp153.KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl*dd=_tmp154;
return dd->name;}}else{_tmp154=((_tmp153.UnknownDatatype).val).name;{struct _tuple1*n=_tmp154;
return n;}};}struct _tuple18{struct _tuple1*f1;struct _tuple1*f2;};
# 1065
static struct _tuple18 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp155=i;void*_tmp157;void*_tmp156;if((_tmp155.KnownDatatypefield).tag == 2){_tmp156=((_tmp155.KnownDatatypefield).val).f1;_tmp157=((_tmp155.KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*dd=_tmp156;struct Cyc_Absyn_Datatypefield*df=_tmp157;
# 1068
return({struct _tuple18 _tmp4D0;_tmp4D0.f1=dd->name,_tmp4D0.f2=df->name;_tmp4D0;});}}else{_tmp156=((_tmp155.UnknownDatatypefield).val).datatype_name;_tmp157=((_tmp155.UnknownDatatypefield).val).field_name;{struct _tuple1*d=_tmp156;struct _tuple1*f=_tmp157;
# 1070
return({struct _tuple18 _tmp4D1;_tmp4D1.f1=d,_tmp4D1.f2=f;_tmp4D1;});}};}struct _tuple19{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1073
static struct _tuple19 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp158=i;void*_tmp159;enum Cyc_Absyn_AggrKind _tmp15A;if((_tmp158.UnknownAggr).tag == 1){_tmp15A=((_tmp158.UnknownAggr).val).f1;_tmp159=((_tmp158.UnknownAggr).val).f2;{enum Cyc_Absyn_AggrKind k=_tmp15A;struct _tuple1*n=_tmp159;
return({struct _tuple19 _tmp4D2;_tmp4D2.f1=k,_tmp4D2.f2=n;_tmp4D2;});}}else{_tmp159=*(_tmp158.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp159;
return({struct _tuple19 _tmp4D3;_tmp4D3.f1=ad->kind,_tmp4D3.f2=ad->name;_tmp4D3;});}};}
# 1079
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1085
struct _tuple15 _tmp15B=({struct _tuple15 _tmp4D4;_tmp4D4.f1=t1,_tmp4D4.f2=t2;_tmp4D4;});struct _tuple15 _stmttmp31=_tmp15B;struct _tuple15 _tmp15C=_stmttmp31;union Cyc_Absyn_AggrInfo _tmp15E;union Cyc_Absyn_AggrInfo _tmp15D;union Cyc_Absyn_DatatypeFieldInfo _tmp160;union Cyc_Absyn_DatatypeFieldInfo _tmp15F;union Cyc_Absyn_DatatypeInfo _tmp162;union Cyc_Absyn_DatatypeInfo _tmp161;struct _fat_ptr _tmp166;struct _fat_ptr _tmp165;void*_tmp164;void*_tmp163;switch(*((int*)_tmp15C.f1)){case 15: if(*((int*)_tmp15C.f2)== 15){_tmp163=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp15C.f1)->f1;_tmp164=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp15C.f2)->f1;{struct _tuple1*n1=_tmp163;struct _tuple1*n2=_tmp164;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17: if(*((int*)_tmp15C.f2)== 17){_tmp165=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp15C.f1)->f1;_tmp166=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp15C.f2)->f1;{struct _fat_ptr s1=_tmp165;struct _fat_ptr s2=_tmp166;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16: if(*((int*)_tmp15C.f2)== 16){_tmp163=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp15C.f1)->f1;_tmp164=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp15C.f2)->f1;{struct Cyc_List_List*fs1=_tmp163;struct Cyc_List_List*fs2=_tmp164;
# 1089
return((int(*)(int(*)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18: if(*((int*)_tmp15C.f2)== 18){_tmp161=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp15C.f1)->f1;_tmp162=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp15C.f2)->f1;{union Cyc_Absyn_DatatypeInfo info1=_tmp161;union Cyc_Absyn_DatatypeInfo info2=_tmp162;
# 1091
return({struct _tuple1*_tmp5CD=Cyc_Tcutil_get_datatype_qvar(info1);Cyc_Absyn_qvar_cmp(_tmp5CD,Cyc_Tcutil_get_datatype_qvar(info2));});}}else{goto _LLD;}case 19: if(*((int*)_tmp15C.f2)== 19){_tmp15F=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp15C.f1)->f1;_tmp160=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp15C.f2)->f1;{union Cyc_Absyn_DatatypeFieldInfo info1=_tmp15F;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp160;
# 1093
struct _tuple18 _tmp167=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple18 _stmttmp32=_tmp167;struct _tuple18 _tmp168=_stmttmp32;void*_tmp16A;void*_tmp169;_tmp169=_tmp168.f1;_tmp16A=_tmp168.f2;{struct _tuple1*d1=_tmp169;struct _tuple1*f1=_tmp16A;
struct _tuple18 _tmp16B=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple18 _stmttmp33=_tmp16B;struct _tuple18 _tmp16C=_stmttmp33;void*_tmp16E;void*_tmp16D;_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;{struct _tuple1*d2=_tmp16D;struct _tuple1*f2=_tmp16E;
int _tmp16F=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp16F;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20: if(*((int*)_tmp15C.f2)== 20){_tmp15D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp15C.f1)->f1;_tmp15E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp15C.f2)->f1;{union Cyc_Absyn_AggrInfo info1=_tmp15D;union Cyc_Absyn_AggrInfo info2=_tmp15E;
# 1099
struct _tuple19 _tmp170=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple19 _stmttmp34=_tmp170;struct _tuple19 _tmp171=_stmttmp34;void*_tmp173;enum Cyc_Absyn_AggrKind _tmp172;_tmp172=_tmp171.f1;_tmp173=_tmp171.f2;{enum Cyc_Absyn_AggrKind k1=_tmp172;struct _tuple1*q1=_tmp173;
struct _tuple19 _tmp174=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple19 _stmttmp35=_tmp174;struct _tuple19 _tmp175=_stmttmp35;void*_tmp177;enum Cyc_Absyn_AggrKind _tmp176;_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;{enum Cyc_Absyn_AggrKind k2=_tmp176;struct _tuple1*q2=_tmp177;
int _tmp178=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp178;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD:
 return 0;};}}}
# 1108
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return -1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp5CF)(void*,void*)=cmp;void*_tmp5CE=(void*)_check_null(a1);_tmp5CF(_tmp5CE,(void*)_check_null(a2));});}
# 1114
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp179=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp179;
int _tmp17A=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp17A;
return Cyc_Core_intcmp(i1,i2);}
# 1119
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp17B=tqt1;void*_tmp17D;struct Cyc_Absyn_Tqual _tmp17C;_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;{struct Cyc_Absyn_Tqual tq1=_tmp17C;void*t1=_tmp17D;
struct _tuple12*_tmp17E=tqt2;void*_tmp180;struct Cyc_Absyn_Tqual _tmp17F;_tmp17F=_tmp17E->f1;_tmp180=_tmp17E->f2;{struct Cyc_Absyn_Tqual tq2=_tmp17F;void*t2=_tmp180;
int _tmp181=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp181;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1127
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp182=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp182;
if(zsc != 0)return zsc;{
int _tmp183=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp183;
if(tqc != 0)return tqc;{
int _tmp184=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp184;
if(tc != 0)return tc;{
int _tmp185=((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Atts_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp185;
if(ac != 0)return ac;
ac=((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1144
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1 == t2)return 0;{
int _tmp186=({int _tmp5D0=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp5D0,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp186;
if(shallowcmp != 0)
return shallowcmp;{
# 1153
struct _tuple15 _tmp187=({struct _tuple15 _tmp4D8;_tmp4D8.f1=t2,_tmp4D8.f2=t1;_tmp4D8;});struct _tuple15 _stmttmp36=_tmp187;struct _tuple15 _tmp188=_stmttmp36;enum Cyc_Absyn_AggrKind _tmp18C;enum Cyc_Absyn_AggrKind _tmp18B;struct Cyc_Absyn_FnInfo _tmp18E;struct Cyc_Absyn_FnInfo _tmp18D;void*_tmp19A;void*_tmp199;void*_tmp198;void*_tmp197;void*_tmp196;struct Cyc_Absyn_Tqual _tmp192;void*_tmp195;void*_tmp194;void*_tmp193;struct Cyc_Absyn_Tqual _tmp18F;void*_tmp191;void*_tmp190;void*_tmp18A;void*_tmp189;switch(*((int*)_tmp188.f1)){case 0: if(*((int*)_tmp188.f2)== 0){_tmp189=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp188.f1)->f1;_tmp18A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp188.f1)->f2;_tmp190=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp188.f2)->f1;_tmp191=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp188.f2)->f2;{void*c1=_tmp189;struct Cyc_List_List*ts1=_tmp18A;void*c2=_tmp190;struct Cyc_List_List*ts2=_tmp191;
# 1155
int _tmp19B=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp19B;
if(c != 0)return c;
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1: if(*((int*)_tmp188.f2)== 1)
# 1159
return(int)t1 - (int)t2;else{goto _LL15;}case 2: if(*((int*)_tmp188.f2)== 2){_tmp189=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp188.f1)->f1;_tmp18A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp188.f2)->f1;{struct Cyc_Absyn_Tvar*tv2=_tmp189;struct Cyc_Absyn_Tvar*tv1=_tmp18A;
# 1164
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3: if(*((int*)_tmp188.f2)== 3){_tmp189=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f1)->f1).elt_type;_tmp18F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f1)->f1).elt_tq;_tmp18A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f1)->f1).ptr_atts).rgn;_tmp190=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f1)->f1).ptr_atts).nullable;_tmp191=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f1)->f1).ptr_atts).bounds;_tmp193=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f1)->f1).ptr_atts).zero_term;_tmp194=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f1)->f1).ptr_atts).released;_tmp195=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f2)->f1).elt_type;_tmp192=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f2)->f1).elt_tq;_tmp196=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f2)->f1).ptr_atts).rgn;_tmp197=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f2)->f1).ptr_atts).nullable;_tmp198=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f2)->f1).ptr_atts).bounds;_tmp199=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f2)->f1).ptr_atts).zero_term;_tmp19A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188.f2)->f1).ptr_atts).released;{void*t2a=_tmp189;struct Cyc_Absyn_Tqual tqual2a=_tmp18F;void*rgn2=_tmp18A;void*null2a=_tmp190;void*b2=_tmp191;void*zt2=_tmp193;void*rel2=_tmp194;void*t1a=_tmp195;struct Cyc_Absyn_Tqual tqual1a=_tmp192;void*rgn1=_tmp196;void*null1a=_tmp197;void*b1=_tmp198;void*zt1=_tmp199;void*rel1=_tmp19A;
# 1168
int _tmp19C=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp19C;
if(etc != 0)return etc;{
int _tmp19D=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp19D;
if(rc != 0)return rc;{
int _tmp19E=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp19E;
if(tqc != 0)return tqc;{
int _tmp19F=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp19F;
if(cc != 0)return cc;{
int _tmp1A0=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp1A0;
if(zc != 0)return zc;{
int _tmp1A1=Cyc_Tcutil_typecmp(rel1,rel2);int relc=_tmp1A1;
if(relc != 0)return relc;{
int _tmp1A2=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp1A2;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}}else{goto _LL15;}case 4: if(*((int*)_tmp188.f2)== 4){_tmp189=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188.f1)->f1).elt_type;_tmp18F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188.f1)->f1).tq;_tmp18A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188.f1)->f1).num_elts;_tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188.f1)->f1).zero_term;_tmp191=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188.f2)->f1).elt_type;_tmp192=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188.f2)->f1).tq;_tmp193=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188.f2)->f1).num_elts;_tmp194=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188.f2)->f1).zero_term;{void*t2a=_tmp189;struct Cyc_Absyn_Tqual tq2a=_tmp18F;struct Cyc_Absyn_Exp*e1=_tmp18A;void*zt1=_tmp190;void*t1a=_tmp191;struct Cyc_Absyn_Tqual tq1a=_tmp192;struct Cyc_Absyn_Exp*e2=_tmp193;void*zt2=_tmp194;
# 1186
int _tmp1A3=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp1A3;
if(tqc != 0)return tqc;{
int _tmp1A4=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp1A4;
if(tc != 0)return tc;{
int _tmp1A5=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp1A5;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D5;_tmp4D5.tag=0,({struct _fat_ptr _tmp5D1=({const char*_tmp1A8="missing expression in array index";_tag_fat(_tmp1A8,sizeof(char),34U);});_tmp4D5.f1=_tmp5D1;});_tmp4D5;});void*_tmp1A6[1];_tmp1A6[0]=& _tmp1A7;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1A6,sizeof(void*),1));});
return((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5: if(*((int*)_tmp188.f2)== 5){_tmp18D=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp188.f1)->f1;_tmp18E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp188.f2)->f1;{struct Cyc_Absyn_FnInfo f1=_tmp18D;struct Cyc_Absyn_FnInfo f2=_tmp18E;
# 1198
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp1A9=f1.args;struct Cyc_List_List*args1=_tmp1A9;
struct Cyc_List_List*_tmp1AA=f2.args;struct Cyc_List_List*args2=_tmp1AA;
for(1;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp1AB=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp37=_tmp1AB;struct _tuple9 _tmp1AC=_stmttmp37;void*_tmp1AE;struct Cyc_Absyn_Tqual _tmp1AD;_tmp1AD=_tmp1AC.f2;_tmp1AE=_tmp1AC.f3;{struct Cyc_Absyn_Tqual tq1=_tmp1AD;void*t1=_tmp1AE;
struct _tuple9 _tmp1AF=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp38=_tmp1AF;struct _tuple9 _tmp1B0=_stmttmp38;void*_tmp1B2;struct Cyc_Absyn_Tqual _tmp1B1;_tmp1B1=_tmp1B0.f2;_tmp1B2=_tmp1B0.f3;{struct Cyc_Absyn_Tqual tq2=_tmp1B1;void*t2=_tmp1B2;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1213
if(args1 != 0)return 1;
if(args2 != 0)return -1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return -1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return -1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp5D2=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp5D2,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return -1;}
# 1227
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp1B3=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp1B3;
struct Cyc_List_List*_tmp1B4=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp1B4;
for(1;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp1B5=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp39=_tmp1B5;struct _tuple15 _tmp1B6=_stmttmp39;void*_tmp1B8;void*_tmp1B7;_tmp1B7=_tmp1B6.f1;_tmp1B8=_tmp1B6.f2;{void*t1a=_tmp1B7;void*t1b=_tmp1B8;
struct _tuple15 _tmp1B9=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp3A=_tmp1B9;struct _tuple15 _tmp1BA=_stmttmp3A;void*_tmp1BC;void*_tmp1BB;_tmp1BB=_tmp1BA.f1;_tmp1BC=_tmp1BA.f2;{void*t2a=_tmp1BB;void*t2b=_tmp1BC;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1237
if(rpo1 != 0)return 1;
if(rpo2 != 0)return -1;
r=((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1245
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D6;_tmp4D6.tag=0,({struct _fat_ptr _tmp5D3=({const char*_tmp1BF="typecmp: function type comparison should never get here!";_tag_fat(_tmp1BF,sizeof(char),57U);});_tmp4D6.f1=_tmp5D3;});_tmp4D6;});void*_tmp1BD[1];_tmp1BD[0]=& _tmp1BE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1BD,sizeof(void*),1));});}}}}}else{goto _LL15;}case 6: if(*((int*)_tmp188.f2)== 6){_tmp189=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp188.f1)->f1;_tmp18A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp188.f2)->f1;{struct Cyc_List_List*ts2=_tmp189;struct Cyc_List_List*ts1=_tmp18A;
# 1248
return((int(*)(int(*)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7: if(*((int*)_tmp188.f2)== 7){_tmp18B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp188.f1)->f1;_tmp189=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp188.f1)->f2;_tmp18C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp188.f2)->f1;_tmp18A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp188.f2)->f2;{enum Cyc_Absyn_AggrKind k2=_tmp18B;struct Cyc_List_List*fs2=_tmp189;enum Cyc_Absyn_AggrKind k1=_tmp18C;struct Cyc_List_List*fs1=_tmp18A;
# 1251
if((int)k1 != (int)k2)
return(int)k1 == 0?-1: 1;
return((int(*)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9: if(*((int*)_tmp188.f2)== 9){_tmp189=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp188.f1)->f1;_tmp18A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp188.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp189;struct Cyc_Absyn_Exp*e2=_tmp18A;
# 1255
_tmp189=e1;_tmp18A=e2;goto _LL14;}}else{goto _LL15;}case 11: if(*((int*)_tmp188.f2)== 11){_tmp189=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp188.f1)->f1;_tmp18A=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp188.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp189;struct Cyc_Absyn_Exp*e2=_tmp18A;
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D7;_tmp4D7.tag=0,({struct _fat_ptr _tmp5D4=({const char*_tmp1C2="Unmatched case in typecmp";_tag_fat(_tmp1C2,sizeof(char),26U);});_tmp4D7.f1=_tmp5D4;});_tmp4D7;});void*_tmp1C0[1];_tmp1C0[0]=& _tmp1C1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1C0,sizeof(void*),1));});};}}}
# 1263
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp1C3=({struct _tuple15 _tmp4DA;({void*_tmp5D6=Cyc_Absyn_compress(t1);_tmp4DA.f1=_tmp5D6;}),({void*_tmp5D5=Cyc_Absyn_compress(t2);_tmp4DA.f2=_tmp5D5;});_tmp4DA;});struct _tuple15 _stmttmp3B=_tmp1C3;struct _tuple15 _tmp1C4=_stmttmp3B;void*_tmp1C6;void*_tmp1C5;if(*((int*)_tmp1C4.f1)== 0){if(*((int*)_tmp1C4.f2)== 0){_tmp1C5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C4.f1)->f1;_tmp1C6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C4.f2)->f1;{void*c1=_tmp1C5;void*c2=_tmp1C6;
# 1266
struct _tuple15 _tmp1C7=({struct _tuple15 _tmp4D9;_tmp4D9.f1=c1,_tmp4D9.f2=c2;_tmp4D9;});struct _tuple15 _stmttmp3C=_tmp1C7;struct _tuple15 _tmp1C8=_stmttmp3C;int _tmp1CA;int _tmp1C9;switch(*((int*)_tmp1C8.f1)){case 2: switch(*((int*)_tmp1C8.f2)){case 2: _tmp1C9=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp1C8.f1)->f1;_tmp1CA=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp1C8.f2)->f1;{int i1=_tmp1C9;int i2=_tmp1CA;
return i2 < i1;}case 1:
 goto _LLB;case 4: _LLB:
 return 1;default: goto _LL26;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1C8.f1)->f2){case Cyc_Absyn_LongLong_sz: if(*((int*)_tmp1C8.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1C8.f2)->f2 == Cyc_Absyn_LongLong_sz)
return 0;else{goto _LLE;}}else{_LLE:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp1C8.f2)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1C8.f2)->f2){case Cyc_Absyn_Int_sz:
# 1274
 goto _LL13;case Cyc_Absyn_Short_sz: _LL19:
# 1279
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1F:
# 1282
 goto _LL21;default: goto _LL26;}case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp1C8.f2)->f1 == 0)
# 1277
goto _LL17;else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp1C8.f2)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1C8.f2)->f2){case Cyc_Absyn_Long_sz: _LL13:
# 1275
 return 0;case Cyc_Absyn_Short_sz: _LL1B:
# 1280
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL21:
# 1283
 goto _LL23;default: goto _LL26;}case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp1C8.f2)->f1 == 0){_LL17:
# 1278
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(*((int*)_tmp1C8.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1C8.f2)->f2 == Cyc_Absyn_Char_sz){_LL23:
# 1284
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4: if(*((int*)_tmp1C8.f2)== 1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1C8.f2)->f2){case Cyc_Absyn_Short_sz: _LL1D:
# 1281
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL25:
# 1285
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26:
# 1287
 return 0;};}}else{goto _LL3;}}else{_LL3:
# 1289
 return 0;};}
# 1293
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp1CB=({struct _tuple15 _tmp4DC;({void*_tmp5D8=Cyc_Absyn_compress(t1);_tmp4DC.f1=_tmp5D8;}),({void*_tmp5D7=Cyc_Absyn_compress(t2);_tmp4DC.f2=_tmp5D7;});_tmp4DC;});struct _tuple15 _stmttmp3D=_tmp1CB;struct _tuple15 _tmp1CC=_stmttmp3D;void*_tmp1CE;void*_tmp1CD;if(*((int*)_tmp1CC.f1)== 0){if(*((int*)_tmp1CC.f2)== 0){_tmp1CD=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC.f1)->f1;_tmp1CE=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC.f2)->f1;{void*c1=_tmp1CD;void*c2=_tmp1CE;
# 1296
{struct _tuple15 _tmp1CF=({struct _tuple15 _tmp4DB;_tmp4DB.f1=c1,_tmp4DB.f2=c2;_tmp4DB;});struct _tuple15 _stmttmp3E=_tmp1CF;struct _tuple15 _tmp1D0=_stmttmp3E;int _tmp1D2;int _tmp1D1;if(*((int*)_tmp1D0.f1)== 2){if(*((int*)_tmp1D0.f2)== 2){_tmp1D1=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f1;_tmp1D2=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f1;{int i1=_tmp1D1;int i2=_tmp1D2;
# 1298
if(i1 != 0 && i1 != 1)return t1;
if(i2 != 0 && i2 != 1)return t2;
if(i1 >= i2)return t1;
return t2;}}else{
return t1;}}else{if(*((int*)_tmp1D0.f2)== 2)
return t2;else{if(*((int*)_tmp1D0.f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_LongLong_sz)
goto _LLF;else{if(*((int*)_tmp1D0.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_Long_sz){_LL14:
# 1308
 goto _LL17;}else{if(*((int*)_tmp1D0.f2)== 4)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1313
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(*((int*)_tmp1D0.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10:
# 1306
 goto _LL13;}else{if(*((int*)_tmp1D0.f2)== 4)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f1)->f2 == Cyc_Absyn_Long_sz){_LL20:
# 1315
 goto _LL23;}else{goto _LL24;}}}}}}else{if(*((int*)_tmp1D0.f2)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f1 == Cyc_Absyn_Unsigned)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1305
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1309
 return Cyc_Absyn_ulong_type;default: if(*((int*)_tmp1D0.f1)== 4)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1314
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1307
 return Cyc_Absyn_slonglong_type;}else{if(*((int*)_tmp1D0.f1)== 4)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1D0.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1316
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(*((int*)_tmp1D0.f1)== 4){_LL18:
# 1311
 goto _LL1B;}else{if(*((int*)_tmp1D0.f2)== 4){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24:
# 1317
 goto _LL5;}}}}}}_LL5:;}
# 1319
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 1322
return Cyc_Absyn_sint_type;}
# 1327
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1330
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Absyn_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->v=t1;_tmp1D3;});}}}
# 1338
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1342
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E0;_tmp4E0.tag=0,({struct _fat_ptr _tmp5D9=({const char*_tmp1DA="type mismatch: expecting ";_tag_fat(_tmp1DA,sizeof(char),26U);});_tmp4E0.f1=_tmp5D9;});_tmp4E0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1D6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4DF;_tmp4DF.tag=2,_tmp4DF.f1=(void*)t;_tmp4DF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DE;_tmp4DE.tag=0,({struct _fat_ptr _tmp5DA=({const char*_tmp1D9=" but found ";_tag_fat(_tmp1D9,sizeof(char),12U);});_tmp4DE.f1=_tmp5DA;});_tmp4DE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1D8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4DD;_tmp4DD.tag=2,_tmp4DD.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt);_tmp4DD;});void*_tmp1D4[4];_tmp1D4[0]=& _tmp1D5,_tmp1D4[1]=& _tmp1D6,_tmp1D4[2]=& _tmp1D7,_tmp1D4[3]=& _tmp1D8;({unsigned _tmp5DB=((struct Cyc_Absyn_Exp*)el->hd)->loc;Cyc_Warn_err2(_tmp5DB,_tag_fat(_tmp1D4,sizeof(void*),4));});});
# 1346
return 0;}}}
# 1348
return 1;}
# 1353
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp1DB=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp3F=_tmp1DB;void*_tmp1DC=_stmttmp3F;if(*((int*)_tmp1DC)== 3){
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,3U);goto _LL0;}else{
return 0;}_LL0:;}
# 1359
return 1;}
# 1362
static int Cyc_Tcutil_coerce_Xint_type(void*Xint_type,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Xint_type))
return 1;
# 1366
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Xint_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E1;_tmp4E1.tag=0,({struct _fat_ptr _tmp5DC=({const char*_tmp1DF="integral size mismatch; conversion supplied";_tag_fat(_tmp1DF,sizeof(char),44U);});_tmp4E1.f1=_tmp5DC;});_tmp4E1;});void*_tmp1DD[1];_tmp1DD[0]=& _tmp1DE;({unsigned _tmp5DD=e->loc;Cyc_Warn_warn2(_tmp5DD,_tag_fat(_tmp1DD,sizeof(void*),1));});});
Cyc_Tcutil_unchecked_cast(e,Xint_type,1U);
return 1;}
# 1372
return 0;}
# 1375
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_uint_type,e);}
# 1379
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_sint_type,e);}
# 1385
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1391
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp5DE=t;Cyc_Unify_unify(_tmp5DE,Cyc_Absyn_bounds_one());});
return Cyc_Absyn_compress(t);}
# 1396
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp1E0=Cyc_Absyn_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp1E0;
void*_tmp1E1=t;void*_tmp1E2;if(*((int*)_tmp1E1)== 9){_tmp1E2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1E1)->f1;{struct Cyc_Absyn_Exp*e=_tmp1E2;
return e;}}else{
# 1401
struct Cyc_Absyn_Exp*_tmp1E3=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp1E3;
v->topt=Cyc_Absyn_uint_type;
return v;};}
# 1410
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp1E4=Cyc_Absyn_compress(b);void*_stmttmp40=_tmp1E4;void*_tmp1E5=_stmttmp40;void*_tmp1E6;if(*((int*)_tmp1E5)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E5)->f1)){case 14:
 return 0;case 13: _tmp1E6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E5)->f2;{struct Cyc_List_List*ts=_tmp1E6;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E3;_tmp4E3.tag=0,({struct _fat_ptr _tmp5DF=({const char*_tmp1EA="get_bounds_exp: ";_tag_fat(_tmp1EA,sizeof(char),17U);});_tmp4E3.f1=_tmp5DF;});_tmp4E3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4E2;_tmp4E2.tag=2,_tmp4E2.f1=(void*)b;_tmp4E2;});void*_tmp1E7[2];_tmp1E7[0]=& _tmp1E8,_tmp1E7[1]=& _tmp1E9;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1E7,sizeof(void*),2));});};}}
# 1419
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp1EB=Cyc_Absyn_compress(t);void*_stmttmp41=_tmp1EB;void*_tmp1EC=_stmttmp41;void*_tmp1ED;if(*((int*)_tmp1EC)== 3){_tmp1ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->f1).ptr_atts).bounds;{void*b=_tmp1ED;
# 1422
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E5;_tmp4E5.tag=0,({struct _fat_ptr _tmp5E0=({const char*_tmp1F1="get_ptr_bounds_exp not pointer: ";_tag_fat(_tmp1F1,sizeof(char),33U);});_tmp4E5.f1=_tmp5E0;});_tmp4E5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4E4;_tmp4E4.tag=2,_tmp4E4.f1=(void*)t;_tmp4E4;});void*_tmp1EE[2];_tmp1EE[0]=& _tmp1EF,_tmp1EE[1]=& _tmp1F0;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1EE,sizeof(void*),2));});};}
# 1428
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Kinds_boolko,({struct Cyc_Core_Opt*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->v=tvs;_tmp1F2;}));}
# 1432
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Kinds_ptrbko,({struct Cyc_Core_Opt*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->v=tvs;_tmp1F3;}));}
# 1437
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);struct _tuple20{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1444
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp1F4=({struct _tuple15 _tmp4EC;_tmp4EC.f1=t1,_tmp4EC.f2=t2;_tmp4EC;});struct _tuple15 _stmttmp42=_tmp1F4;struct _tuple15 _tmp1F5=_stmttmp42;void*_tmp1FD;void*_tmp1FC;struct Cyc_Absyn_Tqual _tmp1FB;void*_tmp1FA;void*_tmp1F9;void*_tmp1F8;struct Cyc_Absyn_Tqual _tmp1F7;void*_tmp1F6;struct Cyc_Absyn_PtrInfo _tmp1FF;struct Cyc_Absyn_PtrInfo _tmp1FE;switch(*((int*)_tmp1F5.f1)){case 3: if(*((int*)_tmp1F5.f2)== 3){_tmp1FE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f1)->f1;_tmp1FF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->f1;{struct Cyc_Absyn_PtrInfo pinfo_a=_tmp1FE;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp1FF;
# 1450
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable)&&
 Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable))
return 0;
# 1454
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple20 _tmp200=({struct _tuple20 _tmp4E7;({struct Cyc_Absyn_Exp*_tmp5E4=({void*_tmp5E3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp5E3,(pinfo_a.ptr_atts).bounds);});_tmp4E7.f1=_tmp5E4;}),({
struct Cyc_Absyn_Exp*_tmp5E2=({void*_tmp5E1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp5E1,(pinfo_b.ptr_atts).bounds);});_tmp4E7.f2=_tmp5E2;});_tmp4E7;});
# 1455
struct _tuple20 _stmttmp43=_tmp200;struct _tuple20 _tmp201=_stmttmp43;void*_tmp203;void*_tmp202;if(_tmp201.f2 == 0)
# 1458
goto _LL9;else{if(_tmp201.f1 == 0){
# 1461
if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp5E5=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp5E5,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
return 0;}else{_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;{struct Cyc_Absyn_Exp*e1=_tmp202;struct Cyc_Absyn_Exp*e2=_tmp203;
# 1469
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp205=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E6;_tmp4E6.tag=0,({struct _fat_ptr _tmp5E6=({const char*_tmp206="implicit cast to shorter array";_tag_fat(_tmp206,sizeof(char),31U);});_tmp4E6.f1=_tmp5E6;});_tmp4E6;});void*_tmp204[1];_tmp204[0]=& _tmp205;({unsigned _tmp5E7=loc;Cyc_Warn_warn2(_tmp5E7,_tag_fat(_tmp204,sizeof(void*),1));});});
if(!({struct Cyc_Absyn_Exp*_tmp5E8=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp5E8,(struct Cyc_Absyn_Exp*)_check_null(e1));}))
return 0;
goto _LL9;}}}_LL9:;}
# 1477
if((pinfo_a.elt_tq).real_const && !(pinfo_b.elt_tq).real_const)
return 0;
# 1480
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(!Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn))
# 1483
return 0;
if(Cyc_Flags_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp208=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4EB;_tmp4EB.tag=0,({struct _fat_ptr _tmp5E9=({const char*_tmp20D="implicit cast from region ";_tag_fat(_tmp20D,sizeof(char),27U);});_tmp4EB.f1=_tmp5E9;});_tmp4EB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp209=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4EA;_tmp4EA.tag=2,_tmp4EA.f1=(void*)(pinfo_a.ptr_atts).rgn;_tmp4EA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp20A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E9;_tmp4E9.tag=0,({
struct _fat_ptr _tmp5EA=({const char*_tmp20C=" to region ";_tag_fat(_tmp20C,sizeof(char),12U);});_tmp4E9.f1=_tmp5EA;});_tmp4E9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp20B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4E8;_tmp4E8.tag=2,_tmp4E8.f1=(void*)(pinfo_b.ptr_atts).rgn;_tmp4E8;});void*_tmp207[4];_tmp207[0]=& _tmp208,_tmp207[1]=& _tmp209,_tmp207[2]=& _tmp20A,_tmp207[3]=& _tmp20B;({unsigned _tmp5EB=loc;Cyc_Warn_warn2(_tmp5EB,_tag_fat(_tmp207,sizeof(void*),4));});});}
# 1489
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)&&(
!Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)|| !(pinfo_b.elt_tq).real_const))
# 1492
return 0;
# 1494
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).released,(pinfo_b.ptr_atts).released)&& !
Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).released))
return 0;
# 1500
if(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type))
return 1;
# 1504
if(!({void*_tmp5EC=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp5EC,(pinfo_b.ptr_atts).bounds);})||
 Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
return 0;
if(!(pinfo_b.elt_tq).real_const && !
Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type)))
return 0;
return Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type);}}else{goto _LL7;}case 4: if(*((int*)_tmp1F5.f2)== 4){_tmp1F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5.f1)->f1).elt_type;_tmp1F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5.f1)->f1).tq;_tmp1F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5.f1)->f1).num_elts;_tmp1F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5.f1)->f1).zero_term;_tmp1FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5.f2)->f1).elt_type;_tmp1FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5.f2)->f1).tq;_tmp1FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5.f2)->f1).num_elts;_tmp1FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5.f2)->f1).zero_term;{void*t1a=_tmp1F6;struct Cyc_Absyn_Tqual tq1a=_tmp1F7;struct Cyc_Absyn_Exp*e1=_tmp1F8;void*zt1=_tmp1F9;void*t2a=_tmp1FA;struct Cyc_Absyn_Tqual tq2a=_tmp1FB;struct Cyc_Absyn_Exp*e2=_tmp1FC;void*zt2=_tmp1FD;
# 1514
if(!Cyc_Unify_unify(zt1,zt2))return 0;
if(e1 == 0 || e2 == 0)return 0;
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))return 0;
if(!Cyc_Unify_unify(t1a,t2a))return 0;
# 1519
return !tq1a.real_const || tq2a.real_const;}}else{goto _LL7;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5.f1)->f1)== 4){if(*((int*)_tmp1F5.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5.f2)->f1)== 1)
# 1521
return 0;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7:
# 1523
 return Cyc_Unify_unify(t1,t2);};}}
# 1527
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp20E=Cyc_Absyn_compress(t);void*_stmttmp44=_tmp20E;void*_tmp20F=_stmttmp44;void*_tmp210;if(*((int*)_tmp20F)== 3){_tmp210=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20F)->f1).elt_type;{void*e=_tmp210;
return e;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp212=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4ED;_tmp4ED.tag=0,({struct _fat_ptr _tmp5ED=({const char*_tmp213="pointer_elt_type";_tag_fat(_tmp213,sizeof(char),17U);});_tmp4ED.f1=_tmp5ED;});_tmp4ED;});void*_tmp211[1];_tmp211[0]=& _tmp212;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp211,sizeof(void*),1));});};}
# 1533
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp214=Cyc_Absyn_compress(t);void*_stmttmp45=_tmp214;void*_tmp215=_stmttmp45;void*_tmp216;if(*((int*)_tmp215)== 3){_tmp216=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp215)->f1).ptr_atts;{struct Cyc_Absyn_PtrAtts*p=(struct Cyc_Absyn_PtrAtts*)_tmp216;
return p->rgn;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp218=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4EE;_tmp4EE.tag=0,({struct _fat_ptr _tmp5EE=({const char*_tmp219="pointer_elt_type";_tag_fat(_tmp219,sizeof(char),17U);});_tmp4EE.f1=_tmp5EE;});_tmp4EE;});void*_tmp217[1];_tmp217[0]=& _tmp218;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp217,sizeof(void*),1));});};}
# 1540
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp21A=Cyc_Absyn_compress(t);void*_stmttmp46=_tmp21A;void*_tmp21B=_stmttmp46;void*_tmp21C;if(*((int*)_tmp21B)== 3){_tmp21C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->f1).ptr_atts).rgn;{void*r=_tmp21C;
*rgn=r;return 1;}}else{
return 0;};}
# 1550
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp21D=Cyc_Absyn_compress(t);void*_stmttmp47=_tmp21D;void*_tmp21E=_stmttmp47;void*_tmp220;void*_tmp21F;switch(*((int*)_tmp21E)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->f1)){case 1:
 goto _LL4;case 2: _LL4:
 return 1;default: goto _LL7;}case 3: _tmp21F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21E)->f1).ptr_atts).nullable;_tmp220=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21E)->f1).ptr_atts).bounds;{void*nullable=_tmp21F;void*bounds=_tmp220;
# 1558
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7:
 return 0;};}
# 1564
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp221=e->r;void*_stmttmp48=_tmp221;void*_tmp222=_stmttmp48;void*_tmp224;void*_tmp223;struct _fat_ptr _tmp225;switch(*((int*)_tmp222)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp222)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp222)->f1).Int_c).val).f2 == 0)
goto _LL4;else{goto _LLF;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp222)->f1).Char_c).val).f2 == 0){_LL4:
 goto _LL6;}else{goto _LLF;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp222)->f1).Short_c).val).f2 == 0){_LL6:
 goto _LL8;}else{goto _LLF;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp222)->f1).LongLong_c).val).f2 == 0){_LL8:
 goto _LLA;}else{goto _LLF;}case 3: _tmp225=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp222)->f1).Wchar_c).val;{struct _fat_ptr s=_tmp225;
# 1572
unsigned long _tmp226=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp226;
int i=0;
if(l >= 2U &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== 92){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== 48)i=2;else{
if(((int)((const char*)s.curr)[1]== 120 && l >= 3U)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== 48)i=3;else{
return 0;}}
for(1;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= 48)return 0;}
return 1;}
# 1582
return 0;}default: goto _LLF;}case 2: _LLA:
# 1570
 return 1;case 14: _tmp223=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp222)->f1;_tmp224=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp222)->f2;{void*t=_tmp223;struct Cyc_Absyn_Exp*e2=_tmp224;
# 1583
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF:
 return 0;};}
# 1590
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp227=Cyc_Absyn_compress(t2);void*_stmttmp49=_tmp227;void*_tmp228=_stmttmp49;void*_tmp229;if(*((int*)_tmp228)== 3){_tmp229=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228)->f1).ptr_atts).nullable;{void*nbl=_tmp229;
# 1595
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp5EF=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp5EF;});
e1->topt=t2;
return 1;}}else{
return 0;};}}struct _tuple21{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1608
struct _tuple21 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Kinds_rk};
# 1612
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp5F2=({struct _fat_ptr*_tmp238=_cycalloc(sizeof(*_tmp238));({struct _fat_ptr _tmp5F1=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp237=({struct Cyc_Int_pa_PrintArg_struct _tmp4F0;_tmp4F0.tag=1,_tmp4F0.f1=(unsigned long)counter ++;_tmp4F0;});void*_tmp235[1];_tmp235[0]=& _tmp237;({struct _fat_ptr _tmp5F0=({const char*_tmp236="__aliasvar%d";_tag_fat(_tmp236,sizeof(char),13U);});Cyc_aprintf(_tmp5F0,_tag_fat(_tmp235,sizeof(void*),1));});});*_tmp238=_tmp5F1;});_tmp238;});_tmp239->f2=_tmp5F2;});_tmp239;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp5F3=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234->tag=4,_tmp234->f1=vd;_tmp234;});Cyc_Absyn_varb_exp(_tmp5F3,e->loc);});
# 1621
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp22A=Cyc_Absyn_compress(e_type);void*_stmttmp4A=_tmp22A;void*_tmp22B=_stmttmp4A;void*_tmp22E;struct Cyc_Absyn_Tqual _tmp22D;void*_tmp22C;if(*((int*)_tmp22B)== 3){_tmp22C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_type;_tmp22D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_tq;_tmp22E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).ptr_atts).rgn;{void*et=_tmp22C;struct Cyc_Absyn_Tqual tq=_tmp22D;void*old_r=_tmp22E;
# 1624
{void*_tmp22F=Cyc_Absyn_compress(old_r);void*_stmttmp4B=_tmp22F;void*_tmp230=_stmttmp4B;void*_tmp232;void*_tmp231;if(*((int*)_tmp230)== 1){_tmp231=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp230)->f2;_tmp232=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp230)->f4;{void**topt=(void**)_tmp231;struct Cyc_Core_Opt*ts=_tmp232;
# 1626
void*_tmp233=Cyc_Absyn_var_type(tv);void*new_r=_tmp233;
*topt=new_r;
goto _LL5;}}else{
goto _LL5;}_LL5:;}
# 1631
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 1634
e->topt=0;
vd->initializer=0;{
# 1638
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1640
return({struct _tuple21 _tmp4EF;_tmp4EF.f1=d,_tmp4EF.f2=ve;_tmp4EF;});}}
# 1645
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1648
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1653
void*_tmp23A=Cyc_Absyn_compress(wants_type);void*_stmttmp4C=_tmp23A;void*_tmp23B=_stmttmp4C;void*_tmp23C;if(*((int*)_tmp23B)== 3){_tmp23C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23B)->f1).ptr_atts).rgn;{void*r2=_tmp23C;
# 1655
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp23D=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp23D;
return(int)k->kind == 3 &&(int)k->aliasqual == 0;}}}else{
return 0;};}
# 1661
return 0;}
# 1665
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1667
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1671
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1674
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1676
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F5;_tmp4F5.tag=0,({struct _fat_ptr _tmp5F4=({const char*_tmp246="integral size mismatch; ";_tag_fat(_tmp246,sizeof(char),25U);});_tmp4F5.f1=_tmp5F4;});_tmp4F5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp240=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4F4;_tmp4F4.tag=2,_tmp4F4.f1=(void*)t1;_tmp4F4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp241=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F3;_tmp4F3.tag=0,({
struct _fat_ptr _tmp5F5=({const char*_tmp245=" -> ";_tag_fat(_tmp245,sizeof(char),5U);});_tmp4F3.f1=_tmp5F5;});_tmp4F3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp242=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4F2;_tmp4F2.tag=2,_tmp4F2.f1=(void*)t2;_tmp4F2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp243=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F1;_tmp4F1.tag=0,({struct _fat_ptr _tmp5F6=({const char*_tmp244=" conversion supplied";_tag_fat(_tmp244,sizeof(char),21U);});_tmp4F1.f1=_tmp5F6;});_tmp4F1;});void*_tmp23E[5];_tmp23E[0]=& _tmp23F,_tmp23E[1]=& _tmp240,_tmp23E[2]=& _tmp241,_tmp23E[3]=& _tmp242,_tmp23E[4]=& _tmp243;({unsigned _tmp5F7=e->loc;Cyc_Warn_warn2(_tmp5F7,_tag_fat(_tmp23E,sizeof(void*),5));});});
Cyc_Tcutil_unchecked_cast(e,t2,1U);
return 1;}
# 1684
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Flags_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp248=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FB;_tmp4FB.tag=0,({struct _fat_ptr _tmp5F8=({const char*_tmp250="implicit alias coercion for ";_tag_fat(_tmp250,sizeof(char),29U);});_tmp4FB.f1=_tmp5F8;});_tmp4FB;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp249=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4FA;_tmp4FA.tag=4,_tmp4FA.f1=e;_tmp4FA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F9;_tmp4F9.tag=0,({struct _fat_ptr _tmp5F9=({const char*_tmp24F=":";_tag_fat(_tmp24F,sizeof(char),2U);});_tmp4F9.f1=_tmp5F9;});_tmp4F9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp24B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4F8;_tmp4F8.tag=2,_tmp4F8.f1=(void*)t1;_tmp4F8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F7;_tmp4F7.tag=0,({struct _fat_ptr _tmp5FA=({const char*_tmp24E=" to ";_tag_fat(_tmp24E,sizeof(char),5U);});_tmp4F7.f1=_tmp5FA;});_tmp4F7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp24D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4F6;_tmp4F6.tag=2,_tmp4F6.f1=(void*)t2;_tmp4F6;});void*_tmp247[6];_tmp247[0]=& _tmp248,_tmp247[1]=& _tmp249,_tmp247[2]=& _tmp24A,_tmp247[3]=& _tmp24B,_tmp247[4]=& _tmp24C,_tmp247[5]=& _tmp24D;({unsigned _tmp5FB=e->loc;Cyc_Warn_warn2(_tmp5FB,_tag_fat(_tmp247,sizeof(void*),6));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1691
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,3U);
return 1;}
# 1695
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= 0){
# 1699
if((int)c != 1)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != 2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp252=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF;_tmp4FF.tag=0,({struct _fat_ptr _tmp5FC=({const char*_tmp257="implicit cast from ";_tag_fat(_tmp257,sizeof(char),20U);});_tmp4FF.f1=_tmp5FC;});_tmp4FF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp253=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4FE;_tmp4FE.tag=2,_tmp4FE.f1=(void*)t1;_tmp4FE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp254=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FD;_tmp4FD.tag=0,({struct _fat_ptr _tmp5FD=({const char*_tmp256=" to ";_tag_fat(_tmp256,sizeof(char),5U);});_tmp4FD.f1=_tmp5FD;});_tmp4FD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp255=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4FC;_tmp4FC.tag=2,_tmp4FC.f1=(void*)t2;_tmp4FC;});void*_tmp251[4];_tmp251[0]=& _tmp252,_tmp251[1]=& _tmp253,_tmp251[2]=& _tmp254,_tmp251[3]=& _tmp255;({unsigned _tmp5FE=e->loc;Cyc_Warn_warn2(_tmp5FE,_tag_fat(_tmp251,sizeof(void*),4));});});
return 1;}
# 1705
return 0;}
# 1711
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1714
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1724
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple22{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1726
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple22*env,struct Cyc_Absyn_Aggrfield*x){
# 1729
struct _tuple22 _tmp258=*env;struct _tuple22 _stmttmp4D=_tmp258;struct _tuple22 _tmp259=_stmttmp4D;int _tmp25C;void*_tmp25B;void*_tmp25A;_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;_tmp25C=_tmp259.f3;{struct Cyc_List_List*inst=_tmp25A;struct _RegionHandle*r=_tmp25B;int flatten=_tmp25C;
void*_tmp25D=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp25D;
struct Cyc_List_List*_tmp25E=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp25E;
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp260=_region_malloc(r,sizeof(*_tmp260));({struct _tuple12*_tmp5FF=({struct _tuple12*_tmp25F=_region_malloc(r,sizeof(*_tmp25F));_tmp25F->f1=x->tq,_tmp25F->f2=t;_tmp25F;});_tmp260->hd=_tmp5FF;}),_tmp260->tl=0;_tmp260;});
return ts;}}struct _tuple23{struct _RegionHandle*f1;int f2;};
# 1736
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple23*env,struct _tuple12*x){
# 1738
struct _tuple23 _tmp261=*env;struct _tuple23 _stmttmp4E=_tmp261;struct _tuple23 _tmp262=_stmttmp4E;int _tmp264;void*_tmp263;_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;{struct _RegionHandle*r=_tmp263;int flatten=_tmp264;
struct _tuple12 _tmp265=*x;struct _tuple12 _stmttmp4F=_tmp265;struct _tuple12 _tmp266=_stmttmp4F;void*_tmp268;struct Cyc_Absyn_Tqual _tmp267;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;{struct Cyc_Absyn_Tqual tq=_tmp267;void*t=_tmp268;
struct Cyc_List_List*_tmp269=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp269;
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp26B=_region_malloc(r,sizeof(*_tmp26B));({struct _tuple12*_tmp600=({struct _tuple12*_tmp26A=_region_malloc(r,sizeof(*_tmp26A));_tmp26A->f1=tq,_tmp26A->f2=t;_tmp26A;});_tmp26B->hd=_tmp600;}),_tmp26B->tl=0;_tmp26B;});
return ts;}}}
# 1745
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1747
if(flatten){
t1=Cyc_Absyn_compress(t1);{
void*_tmp26C=t1;void*_tmp26E;void*_tmp26D;switch(*((int*)_tmp26C)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f1)){case 0:
 return 0;case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f1)->f1).KnownAggr).tag == 2){_tmp26D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f1)->f1).KnownAggr).val;_tmp26E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_tmp26D;struct Cyc_List_List*ts=_tmp26E;
# 1765
if((((int)ad->kind == 1 || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 1767
return({struct Cyc_List_List*_tmp279=_region_malloc(r,sizeof(*_tmp279));({struct _tuple12*_tmp602=({struct _tuple12*_tmp278=_region_malloc(r,sizeof(*_tmp278));({struct Cyc_Absyn_Tqual _tmp601=Cyc_Absyn_empty_tqual(0U);_tmp278->f1=_tmp601;}),_tmp278->f2=t1;_tmp278;});_tmp279->hd=_tmp602;}),_tmp279->tl=0;_tmp279;});{
struct Cyc_List_List*_tmp27A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp27A;
struct _tuple22 env=({struct _tuple22 _tmp500;_tmp500.f1=inst,_tmp500.f2=r,_tmp500.f3=flatten;_tmp500;});
struct Cyc_List_List*_tmp27B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp50=_tmp27B;struct Cyc_List_List*_tmp27C=_stmttmp50;void*_tmp27E;void*_tmp27D;if(_tmp27C == 0)
return 0;else{_tmp27D=(struct Cyc_Absyn_Aggrfield*)_tmp27C->hd;_tmp27E=_tmp27C->tl;{struct Cyc_Absyn_Aggrfield*hd=_tmp27D;struct Cyc_List_List*tl=_tmp27E;
# 1773
struct Cyc_List_List*_tmp27F=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp27F;
env.f3=0;{
struct Cyc_List_List*_tmp280=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple22*,struct Cyc_Absyn_Aggrfield*),struct _tuple22*,struct Cyc_List_List*))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp280;
struct Cyc_List_List*_tmp281=({struct Cyc_List_List*_tmp282=_region_malloc(r,sizeof(*_tmp282));_tmp282->hd=hd2,_tmp282->tl=tl2;_tmp282;});struct Cyc_List_List*tts=_tmp281;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rflatten)(r,tts);}}};}}}else{goto _LL9;}default: goto _LL9;}case 6: _tmp26D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp26C)->f1;{struct Cyc_List_List*tqs=_tmp26D;
# 1752
struct _tuple23 _tmp26F=({struct _tuple23 _tmp501;_tmp501.f1=r,_tmp501.f2=flatten;_tmp501;});struct _tuple23 env=_tmp26F;
# 1754
struct Cyc_List_List*_tmp270=tqs;void*_tmp272;void*_tmp271;if(_tmp270 == 0)
return 0;else{_tmp271=(struct _tuple12*)_tmp270->hd;_tmp272=_tmp270->tl;{struct _tuple12*hd=_tmp271;struct Cyc_List_List*tl=_tmp272;
# 1757
struct Cyc_List_List*_tmp273=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp273;
env.f2=0;{
struct Cyc_List_List*_tmp274=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple23*,struct _tuple12*),struct _tuple23*,struct Cyc_List_List*))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp274;
struct Cyc_List_List*_tmp275=({struct Cyc_List_List*_tmp277=_region_malloc(r,sizeof(*_tmp277));_tmp277->hd=hd2,_tmp277->tl=tl2;_tmp277;});struct Cyc_List_List*temp=_tmp275;
return({struct _RegionHandle*_tmp603=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rflatten)(_tmp603,({struct Cyc_List_List*_tmp276=_region_malloc(r,sizeof(*_tmp276));_tmp276->hd=hd2,_tmp276->tl=tl2;_tmp276;}));});}}};}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp26C)->f1 == Cyc_Absyn_StructA){_tmp26D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp26C)->f2;{struct Cyc_List_List*fs=_tmp26D;
# 1780
struct _tuple22 env=({struct _tuple22 _tmp502;_tmp502.f1=0,_tmp502.f2=r,_tmp502.f3=flatten;_tmp502;});
struct Cyc_List_List*_tmp283=fs;void*_tmp285;void*_tmp284;if(_tmp283 == 0)
return 0;else{_tmp284=(struct Cyc_Absyn_Aggrfield*)_tmp283->hd;_tmp285=_tmp283->tl;{struct Cyc_Absyn_Aggrfield*hd=_tmp284;struct Cyc_List_List*tl=_tmp285;
# 1784
struct Cyc_List_List*_tmp286=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp286;
env.f3=0;{
struct Cyc_List_List*_tmp287=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple22*,struct Cyc_Absyn_Aggrfield*),struct _tuple22*,struct Cyc_List_List*))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp287;
struct Cyc_List_List*_tmp288=({struct Cyc_List_List*_tmp289=_region_malloc(r,sizeof(*_tmp289));_tmp289->hd=hd2,_tmp289->tl=tl2;_tmp289;});struct Cyc_List_List*tts=_tmp288;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rflatten)(r,tts);}}};}}else{goto _LL9;}default: _LL9:
# 1790
 goto _LL0;}_LL0:;}}
# 1793
return({struct Cyc_List_List*_tmp28B=_region_malloc(r,sizeof(*_tmp28B));({struct _tuple12*_tmp605=({struct _tuple12*_tmp28A=_region_malloc(r,sizeof(*_tmp28A));({struct Cyc_Absyn_Tqual _tmp604=Cyc_Absyn_empty_tqual(0U);_tmp28A->f1=_tmp604;}),_tmp28A->f2=t1;_tmp28A;});_tmp28B->hd=_tmp605;}),_tmp28B->tl=0;_tmp28B;});}
# 1796
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp28C=({struct _tuple15 _tmp503;({void*_tmp607=Cyc_Absyn_compress(t1);_tmp503.f1=_tmp607;}),({void*_tmp606=Cyc_Absyn_compress(t2);_tmp503.f2=_tmp606;});_tmp503;});struct _tuple15 _stmttmp51=_tmp28C;struct _tuple15 _tmp28D=_stmttmp51;enum Cyc_Absyn_Size_of _tmp28F;enum Cyc_Absyn_Size_of _tmp28E;if(*((int*)_tmp28D.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28D.f1)->f1)== 1){if(*((int*)_tmp28D.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28D.f2)->f1)== 1){_tmp28E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28D.f1)->f1)->f2;_tmp28F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28D.f2)->f1)->f2;{enum Cyc_Absyn_Size_of b1=_tmp28E;enum Cyc_Absyn_Size_of b2=_tmp28F;
# 1799
return((int)b1 == (int)b2 ||(int)b1 == 2 &&(int)b2 == 3)||
(int)b1 == 3 &&(int)b2 == 2;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 1807
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1809
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp290=({struct _tuple15 _tmp504;_tmp504.f1=t1,_tmp504.f2=t2;_tmp504;});struct _tuple15 _stmttmp52=_tmp290;struct _tuple15 _tmp291=_stmttmp52;struct Cyc_Absyn_FnInfo _tmp293;struct Cyc_Absyn_FnInfo _tmp292;void*_tmp2A1;void*_tmp2A0;void*_tmp29F;void*_tmp29E;void*_tmp29D;struct Cyc_Absyn_Tqual _tmp29C;void*_tmp29B;void*_tmp29A;void*_tmp298;void*_tmp297;void*_tmp296;void*_tmp295;struct Cyc_Absyn_Tqual _tmp299;void*_tmp294;switch(*((int*)_tmp291.f1)){case 3: if(*((int*)_tmp291.f2)== 3){_tmp294=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f1)->f1).elt_type;_tmp299=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f1)->f1).elt_tq;_tmp295=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f1)->f1).ptr_atts).rgn;_tmp296=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f1)->f1).ptr_atts).nullable;_tmp297=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f1)->f1).ptr_atts).bounds;_tmp298=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f1)->f1).ptr_atts).zero_term;_tmp29A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f1)->f1).ptr_atts).released;_tmp29B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f2)->f1).elt_type;_tmp29C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f2)->f1).elt_tq;_tmp29D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f2)->f1).ptr_atts).rgn;_tmp29E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f2)->f1).ptr_atts).nullable;_tmp29F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f2)->f1).ptr_atts).bounds;_tmp2A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f2)->f1).ptr_atts).zero_term;_tmp2A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f2)->f1).ptr_atts).released;{void*t_a=_tmp294;struct Cyc_Absyn_Tqual q_a=_tmp299;void*rt_a=_tmp295;void*null_a=_tmp296;void*b_a=_tmp297;void*zt_a=_tmp298;void*rel_a=_tmp29A;void*t_b=_tmp29B;struct Cyc_Absyn_Tqual q_b=_tmp29C;void*rt_b=_tmp29D;void*null_b=_tmp29E;void*b_b=_tmp29F;void*zt_b=_tmp2A0;void*rel_b=_tmp2A1;
# 1821
if(q_a.real_const && !q_b.real_const)
return 0;
# 1824
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 1828
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 1832
if((Cyc_Unify_unify(rel_a,rel_b)&& !
Cyc_Absyn_type2bool(0,rel_a))&& Cyc_Absyn_type2bool(0,rel_b))
return 0;
# 1836
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 1840
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp2A2=({void*_tmp608=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp608,b_a);});struct Cyc_Absyn_Exp*e1=_tmp2A2;
struct Cyc_Absyn_Exp*_tmp2A3=({void*_tmp609=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp609,b_b);});struct Cyc_Absyn_Exp*e2=_tmp2A3;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 1849
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 1854
int _tmp2A4=({void*_tmp60A=b_b;Cyc_Unify_unify(_tmp60A,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);int deep_subtype=_tmp2A4;
# 1857
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp60E=po;struct Cyc_List_List*_tmp60D=({struct Cyc_List_List*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));({struct _tuple15*_tmp60B=({struct _tuple15*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5->f1=t1,_tmp2A5->f2=t2;_tmp2A5;});_tmp2A6->hd=_tmp60B;}),_tmp2A6->tl=assume;_tmp2A6;});void*_tmp60C=t_a;Cyc_Tcutil_ptrsubtype(_tmp60E,_tmp60D,_tmp60C,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f1)->f1)== 19){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(*((int*)_tmp291.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f2)->f1)== 18){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f2)->f1)->f1).KnownDatatype).tag == 2){_tmp294=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp295=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp296=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f1)->f2;_tmp297=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f2)->f1)->f1).KnownDatatype).val;_tmp298=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp291.f2)->f2;{struct Cyc_Absyn_Datatypedecl*dd1=_tmp294;struct Cyc_Absyn_Datatypefield*df=_tmp295;struct Cyc_List_List*ts1=_tmp296;struct Cyc_Absyn_Datatypedecl*dd2=_tmp297;struct Cyc_List_List*ts2=_tmp298;
# 1864
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 1866
if(({int _tmp60F=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts1);_tmp60F != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts2);}))return 0;
for(1;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5: if(*((int*)_tmp291.f2)== 5){_tmp292=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp291.f1)->f1;_tmp293=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp291.f2)->f1;{struct Cyc_Absyn_FnInfo f1=_tmp292;struct Cyc_Absyn_FnInfo f2=_tmp293;
# 1872
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp2A7=f1.tvars;struct Cyc_List_List*tvs1=_tmp2A7;
struct Cyc_List_List*_tmp2A8=f2.tvars;struct Cyc_List_List*tvs2=_tmp2A8;
if(({int _tmp610=((int(*)(struct Cyc_List_List*))Cyc_List_length)(tvs1);_tmp610 != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));({struct _tuple17*_tmp612=({struct _tuple17*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp611=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp2A9->f2=_tmp611;});_tmp2A9;});_tmp2AA->hd=_tmp612;}),_tmp2AA->tl=inst;_tmp2AA;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 1883
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp615=po;struct Cyc_List_List*_tmp614=assume;void*_tmp613=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->tag=5,_tmp2AB->f1=f1;_tmp2AB;});Cyc_Tcutil_subtype(_tmp615,_tmp614,_tmp613,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->tag=5,_tmp2AC->f1=f2;_tmp2AC;}));});}}}
# 1890
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp2AD=f1.args;struct Cyc_List_List*args1=_tmp2AD;
struct Cyc_List_List*_tmp2AE=f2.args;struct Cyc_List_List*args2=_tmp2AE;
# 1895
if(({int _tmp616=((int(*)(struct Cyc_List_List*))Cyc_List_length)(args1);_tmp616 != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(args2);}))return 0;
# 1897
for(1;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp2AF=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp53=_tmp2AF;struct _tuple9 _tmp2B0=_stmttmp53;void*_tmp2B2;struct Cyc_Absyn_Tqual _tmp2B1;_tmp2B1=_tmp2B0.f2;_tmp2B2=_tmp2B0.f3;{struct Cyc_Absyn_Tqual tq1=_tmp2B1;void*t1=_tmp2B2;
struct _tuple9 _tmp2B3=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp54=_tmp2B3;struct _tuple9 _tmp2B4=_stmttmp54;void*_tmp2B6;struct Cyc_Absyn_Tqual _tmp2B5;_tmp2B5=_tmp2B4.f2;_tmp2B6=_tmp2B4.f3;{struct Cyc_Absyn_Tqual tq2=_tmp2B5;void*t2=_tmp2B6;
# 1901
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 1905
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp2B7=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp2B7;
struct Cyc_Absyn_VarargInfo _tmp2B8=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp2B8;
# 1910
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 1915
if(!({void*_tmp617=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp617,(void*)_check_null(f2.effect));}))return 0;
# 1917
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 1919
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes))return 0;
# 1921
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 1923
return 0;
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 1926
return 0;
# 1928
return 1;}}}else{goto _LL7;}default: _LL7:
 return 0;};}}
# 1939
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1942
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(1;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp2B9=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp55=_tmp2B9;struct _tuple12*_tmp2BA=_stmttmp55;void*_tmp2BC;struct Cyc_Absyn_Tqual _tmp2BB;_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BA->f2;{struct Cyc_Absyn_Tqual tq1=_tmp2BB;void*t1a=_tmp2BC;
struct _tuple12*_tmp2BD=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp56=_tmp2BD;struct _tuple12*_tmp2BE=_stmttmp56;void*_tmp2C0;struct Cyc_Absyn_Tqual _tmp2BF;_tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BE->f2;{struct Cyc_Absyn_Tqual tq2=_tmp2BF;void*t2a=_tmp2C0;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 1956
return 1;}
# 1961
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return 1U;
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
{void*_tmp2C1=t2;if(*((int*)_tmp2C1)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1)->f1)){case 0:
 return 1U;case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1)->f1)->f2){case Cyc_Absyn_Int_sz:
# 1969
 goto _LL6;case Cyc_Absyn_Long_sz: _LL6:
# 1971
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == 2)return 3U;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;}{
# 1975
void*_tmp2C2=t1;void*_tmp2C9;void*_tmp2C8;void*_tmp2C7;void*_tmp2C6;void*_tmp2C5;struct Cyc_Absyn_Tqual _tmp2C4;void*_tmp2C3;switch(*((int*)_tmp2C2)){case 3: _tmp2C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C2)->f1).elt_type;_tmp2C4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C2)->f1).elt_tq;_tmp2C5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C2)->f1).ptr_atts).rgn;_tmp2C6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C2)->f1).ptr_atts).nullable;_tmp2C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C2)->f1).ptr_atts).bounds;_tmp2C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C2)->f1).ptr_atts).zero_term;_tmp2C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C2)->f1).ptr_atts).released;{void*t_a=_tmp2C3;struct Cyc_Absyn_Tqual q_a=_tmp2C4;void*rt_a=_tmp2C5;void*null_a=_tmp2C6;void*b_a=_tmp2C7;void*zt_a=_tmp2C8;void*rel_a=_tmp2C9;
# 1985 "tcutil.cyc"
{void*_tmp2CA=t2;void*_tmp2D1;void*_tmp2D0;void*_tmp2CF;void*_tmp2CE;void*_tmp2CD;struct Cyc_Absyn_Tqual _tmp2CC;void*_tmp2CB;if(*((int*)_tmp2CA)== 3){_tmp2CB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA)->f1).elt_type;_tmp2CC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA)->f1).elt_tq;_tmp2CD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA)->f1).ptr_atts).rgn;_tmp2CE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA)->f1).ptr_atts).nullable;_tmp2CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA)->f1).ptr_atts).bounds;_tmp2D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA)->f1).ptr_atts).zero_term;_tmp2D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA)->f1).ptr_atts).released;{void*t_b=_tmp2CB;struct Cyc_Absyn_Tqual q_b=_tmp2CC;void*rt_b=_tmp2CD;void*null_b=_tmp2CE;void*b_b=_tmp2CF;void*zt_b=_tmp2D0;void*rel_b=_tmp2D1;
# 1987
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp2D2=({struct Cyc_List_List*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));({struct _tuple15*_tmp618=({struct _tuple15*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));_tmp2D9->f1=t1,_tmp2D9->f2=t2;_tmp2D9;});_tmp2DA->hd=_tmp618;}),_tmp2DA->tl=0;_tmp2DA;});struct Cyc_List_List*assump=_tmp2D2;
int _tmp2D3=q_b.real_const || !q_a.real_const;int quals_okay=_tmp2D3;
# 2000 "tcutil.cyc"
int _tmp2D4=
({void*_tmp619=b_b;Cyc_Unify_unify(_tmp619,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2000
int deep_castable=_tmp2D4;
# 2003
int _tmp2D5=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2003
int ptrsub=_tmp2D5;
# 2006
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
int released_ok=Cyc_Unify_unify(rel_a,rel_b)|| !Cyc_Absyn_type2bool(0,rel_b);
# 2009
int _tmp2D6=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2009
int bitcase=_tmp2D6;
# 2012
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp2D7=({void*_tmp61A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp61A,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp2D7;
struct Cyc_Absyn_Exp*_tmp2D8=({void*_tmp61B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp61B,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp2D8;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2021
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2026
if((((bounds_ok && zeroterm_ok)&& released_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return 0U;}}}}else{
goto _LL18;}_LL18:;}
# 2032
return 0U;}case 4: _tmp2C3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C2)->f1).elt_type;_tmp2C4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C2)->f1).tq;_tmp2C5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C2)->f1).num_elts;_tmp2C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C2)->f1).zero_term;{void*t1a=_tmp2C3;struct Cyc_Absyn_Tqual tq1a=_tmp2C4;struct Cyc_Absyn_Exp*e1=_tmp2C5;void*zt1=_tmp2C6;
# 2034
{void*_tmp2DB=t2;void*_tmp2DF;void*_tmp2DE;struct Cyc_Absyn_Tqual _tmp2DD;void*_tmp2DC;if(*((int*)_tmp2DB)== 4){_tmp2DC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DB)->f1).elt_type;_tmp2DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DB)->f1).tq;_tmp2DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DB)->f1).num_elts;_tmp2DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DB)->f1).zero_term;{void*t2a=_tmp2DC;struct Cyc_Absyn_Tqual tq2a=_tmp2DD;struct Cyc_Absyn_Exp*e2=_tmp2DE;void*zt2=_tmp2DF;
# 2036
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2041
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?1U: 0U;}}else{
# 2043
return 0U;};}
# 2045
return 0U;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)){case 15: _tmp2C3=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed1=_tmp2C3;
# 2049
{void*_tmp2E0=t2;void*_tmp2E1;if(*((int*)_tmp2E0)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E0)->f1)== 15){_tmp2E1=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E0)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed2=_tmp2E1;
# 2051
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp61C=((int(*)(struct Cyc_List_List*))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp61C >= ((int(*)(struct Cyc_List_List*))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return 3U;
goto _LL22;}}else{goto _LL25;}}else{_LL25:
 goto _LL22;}_LL22:;}
# 2057
goto _LL11;}case 1: _LL11:
 goto _LL13;case 2: _LL13:
# 2060
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?3U: 0U;case 3: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f2 != 0){_tmp2C3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f2)->hd;{void*r1=_tmp2C3;
# 2063
{void*_tmp2E2=t2;void*_tmp2E3;if(*((int*)_tmp2E2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E2)->f1)== 3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E2)->f2 != 0){_tmp2E3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E2)->f2)->hd;{void*r2=_tmp2E3;
# 2065
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return 1U;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A:
 goto _LL27;}_LL27:;}
# 2069
return 0U;}}else{goto _LL16;}default: goto _LL16;}default: _LL16:
 return 0U;};}}
# 2075
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp2E4=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp2E4;
({void*_tmp61D=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));_tmp2E5->tag=14,_tmp2E5->f1=t,_tmp2E5->f2=inner,_tmp2E5->f3=0,_tmp2E5->f4=c;_tmp2E5;});e->r=_tmp61D;});
e->topt=t;}}
# 2084
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2089
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
return({struct Cyc_Absyn_Tvar*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));({struct _fat_ptr*_tmp620=({struct _fat_ptr*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));({struct _fat_ptr _tmp61F=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2E8=({struct Cyc_Int_pa_PrintArg_struct _tmp505;_tmp505.tag=1,_tmp505.f1=(unsigned long)Cyc_Tcutil_tvar_counter ++;_tmp505;});void*_tmp2E6[1];_tmp2E6[0]=& _tmp2E8;({struct _fat_ptr _tmp61E=({const char*_tmp2E7="#%d";_tag_fat(_tmp2E7,sizeof(char),4U);});Cyc_aprintf(_tmp61E,_tag_fat(_tmp2E6,sizeof(void*),1));});});*_tmp2E9=_tmp61F;});_tmp2E9;});_tmp2EA->name=_tmp620;}),_tmp2EA->identity=-1,_tmp2EA->kind=k;_tmp2EA;});}
# 2093
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
return(int)*((const char*)_check_fat_subscript((t->name)[0],sizeof(char),0U))== 35;}
# 2098
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2105
struct Cyc_List_List*_tmp2EB=0;struct Cyc_List_List*fn_type_atts=_tmp2EB;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Atts_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC->hd=(void*)atts->hd,_tmp2EC->tl=fn_type_atts;_tmp2EC;});}}{
struct Cyc_Absyn_FnInfo _tmp2ED=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp2ED;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE->tag=5,_tmp2EE->f1=type_info;_tmp2EE;});}}
# 2113
return(void*)_check_null(fd->cached_type);}
# 2119
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2121
union Cyc_Relations_RelnOp _tmp2EF=*rop;union Cyc_Relations_RelnOp _stmttmp57=_tmp2EF;union Cyc_Relations_RelnOp _tmp2F0=_stmttmp57;void*_tmp2F1;switch((_tmp2F0.RNumelts).tag){case 2: _tmp2F1=(_tmp2F0.RVar).val;{struct Cyc_Absyn_Vardecl*vd=_tmp2F1;
# 2123
struct _tuple1 _tmp2F2=*vd->name;struct _tuple1 _stmttmp58=_tmp2F2;struct _tuple1 _tmp2F3=_stmttmp58;void*_tmp2F5;union Cyc_Absyn_Nmspace _tmp2F4;_tmp2F4=_tmp2F3.f1;_tmp2F5=_tmp2F3.f2;{union Cyc_Absyn_Nmspace nmspace=_tmp2F4;struct _fat_ptr*var=_tmp2F5;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp621=(struct _fat_ptr)*var;Cyc_strcmp(_tmp621,({const char*_tmp2F6="return_value";_tag_fat(_tmp2F6,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp622=Cyc_Relations_RReturn();*rop=_tmp622;});
goto _LL0;}{
# 2129
unsigned c=0U;
{struct Cyc_List_List*_tmp2F7=args;struct Cyc_List_List*a=_tmp2F7;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp2F8=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp59=_tmp2F8;struct _tuple9*_tmp2F9=_stmttmp59;void*_tmp2FA;_tmp2FA=_tmp2F9->f1;{struct _fat_ptr*vopt=_tmp2FA;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp623=Cyc_Relations_RParam(c);*rop=_tmp623;});
break;}}}}
# 2137
goto _LL0;}}}case 3: _tmp2F1=(_tmp2F0.RNumelts).val;{struct Cyc_Absyn_Vardecl*vd=_tmp2F1;
# 2139
struct _tuple1 _tmp2FB=*vd->name;struct _tuple1 _stmttmp5A=_tmp2FB;struct _tuple1 _tmp2FC=_stmttmp5A;void*_tmp2FE;union Cyc_Absyn_Nmspace _tmp2FD;_tmp2FD=_tmp2FC.f1;_tmp2FE=_tmp2FC.f2;{union Cyc_Absyn_Nmspace nmspace=_tmp2FD;struct _fat_ptr*var=_tmp2FE;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp2FF=args;struct Cyc_List_List*a=_tmp2FF;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp300=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5B=_tmp300;struct _tuple9*_tmp301=_stmttmp5B;void*_tmp302;_tmp302=_tmp301->f1;{struct _fat_ptr*vopt=_tmp302;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp624=Cyc_Relations_RParamNumelts(c);*rop=_tmp624;});
break;}}}}
# 2149
goto _LL0;}}}default:
 goto _LL0;}_LL0:;}
# 2154
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2156
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2160
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2162
if(e == 0)return 0;{
struct Cyc_List_List*_tmp303=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp303;
((void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2169
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp304=pr;void*_tmp306;struct Cyc_Absyn_Tqual _tmp305;_tmp305=_tmp304->f1;_tmp306=_tmp304->f2;{struct Cyc_Absyn_Tqual tq=_tmp305;void*t2=_tmp306;
return t2 == t?pr:({struct _tuple12*_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307->f1=tq,_tmp307->f2=t;_tmp307;});}}struct _tuple24{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple25{struct _tuple24*f1;void*f2;};
# 2173
static struct _tuple25*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2175
return({struct _tuple25*_tmp309=_region_malloc(rgn,sizeof(*_tmp309));({struct _tuple24*_tmp625=({struct _tuple24*_tmp308=_region_malloc(rgn,sizeof(*_tmp308));_tmp308->f1=(*y).f1,_tmp308->f2=(*y).f2;_tmp308;});_tmp309->f1=_tmp625;}),_tmp309->f2=(*y).f3;_tmp309;});}
# 2177
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2179
struct _tuple9 _tmp30A=*orig_arg;struct _tuple9 _stmttmp5C=_tmp30A;struct _tuple9 _tmp30B=_stmttmp5C;void*_tmp30E;struct Cyc_Absyn_Tqual _tmp30D;void*_tmp30C;_tmp30C=_tmp30B.f1;_tmp30D=_tmp30B.f2;_tmp30E=_tmp30B.f3;{struct _fat_ptr*vopt_orig=_tmp30C;struct Cyc_Absyn_Tqual tq_orig=_tmp30D;void*t_orig=_tmp30E;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->f1=vopt_orig,_tmp30F->f2=tq_orig,_tmp30F->f3=t;_tmp30F;});}}
# 2183
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2187
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2189
return({struct Cyc_Absyn_Exp*_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310->topt=old->topt,_tmp310->r=r,_tmp310->loc=old->loc,_tmp310->annot=old->annot;_tmp310;});}
# 2194
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp311=e->r;void*_stmttmp5D=_tmp311;void*_tmp312=_stmttmp5D;enum Cyc_Absyn_Coercion _tmp316;int _tmp315;void*_tmp317;void*_tmp314;void*_tmp313;enum Cyc_Absyn_Primop _tmp318;switch(*((int*)_tmp312)){case 0:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 33: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 1: _LLA:
 return e;case 3: _tmp318=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp312)->f1;_tmp313=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp312)->f2;{enum Cyc_Absyn_Primop p=_tmp318;struct Cyc_List_List*es=_tmp313;
# 2203
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp319=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp319;
struct Cyc_Absyn_Exp*_tmp31A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp31A;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp627=e;Cyc_Tcutil_copye(_tmp627,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->tag=3,_tmp31C->f1=p,({struct Cyc_List_List*_tmp626=({struct Cyc_Absyn_Exp*_tmp31B[1];_tmp31B[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp31B,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp31C->f2=_tmp626;});_tmp31C;}));});}else{
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp31D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp31D;
struct Cyc_Absyn_Exp*_tmp31E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp31E;
struct Cyc_Absyn_Exp*_tmp31F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp31F;
struct Cyc_Absyn_Exp*_tmp320=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp320;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp629=e;Cyc_Tcutil_copye(_tmp629,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->tag=3,_tmp322->f1=p,({struct Cyc_List_List*_tmp628=({struct Cyc_Absyn_Exp*_tmp321[2];_tmp321[0]=new_e1,_tmp321[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp321,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp322->f2=_tmp628;});_tmp322;}));});}}
# 2215
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp324=({struct Cyc_Warn_String_Warn_Warg_struct _tmp506;_tmp506.tag=0,({struct _fat_ptr _tmp62A=({const char*_tmp325="primop does not have 1 or 2 args!";_tag_fat(_tmp325,sizeof(char),34U);});_tmp506.f1=_tmp62A;});_tmp506;});void*_tmp323[1];_tmp323[0]=& _tmp324;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp323,sizeof(void*),1));});}case 6: _tmp313=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp312)->f1;_tmp314=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp312)->f2;_tmp317=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp312)->f3;{struct Cyc_Absyn_Exp*e1=_tmp313;struct Cyc_Absyn_Exp*e2=_tmp314;struct Cyc_Absyn_Exp*e3=_tmp317;
# 2217
struct Cyc_Absyn_Exp*_tmp326=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp326;
struct Cyc_Absyn_Exp*_tmp327=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp327;
struct Cyc_Absyn_Exp*_tmp328=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp328;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp62B=e;Cyc_Tcutil_copye(_tmp62B,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->tag=6,_tmp329->f1=new_e1,_tmp329->f2=new_e2,_tmp329->f3=new_e3;_tmp329;}));});}case 7: _tmp313=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp312)->f1;_tmp314=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp312)->f2;{struct Cyc_Absyn_Exp*e1=_tmp313;struct Cyc_Absyn_Exp*e2=_tmp314;
# 2223
struct Cyc_Absyn_Exp*_tmp32A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp32A;
struct Cyc_Absyn_Exp*_tmp32B=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp32B;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp62C=e;Cyc_Tcutil_copye(_tmp62C,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->tag=7,_tmp32C->f1=new_e1,_tmp32C->f2=new_e2;_tmp32C;}));});}case 8: _tmp313=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp312)->f1;_tmp314=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp312)->f2;{struct Cyc_Absyn_Exp*e1=_tmp313;struct Cyc_Absyn_Exp*e2=_tmp314;
# 2228
struct Cyc_Absyn_Exp*_tmp32D=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp32D;
struct Cyc_Absyn_Exp*_tmp32E=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp32E;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp62D=e;Cyc_Tcutil_copye(_tmp62D,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F->tag=8,_tmp32F->f1=new_e1,_tmp32F->f2=new_e2;_tmp32F;}));});}case 9: _tmp313=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp312)->f1;_tmp314=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp312)->f2;{struct Cyc_Absyn_Exp*e1=_tmp313;struct Cyc_Absyn_Exp*e2=_tmp314;
# 2233
struct Cyc_Absyn_Exp*_tmp330=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp330;
struct Cyc_Absyn_Exp*_tmp331=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp331;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp62E=e;Cyc_Tcutil_copye(_tmp62E,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332->tag=9,_tmp332->f1=new_e1,_tmp332->f2=new_e2;_tmp332;}));});}case 14: _tmp313=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp312)->f1;_tmp314=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp312)->f2;_tmp315=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp312)->f3;_tmp316=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp312)->f4;{void*t=_tmp313;struct Cyc_Absyn_Exp*e1=_tmp314;int b=_tmp315;enum Cyc_Absyn_Coercion c=_tmp316;
# 2238
struct Cyc_Absyn_Exp*_tmp333=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp333;
void*_tmp334=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp334;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp62F=e;Cyc_Tcutil_copye(_tmp62F,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335->tag=14,_tmp335->f1=new_typ,_tmp335->f2=new_e1,_tmp335->f3=b,_tmp335->f4=c;_tmp335;}));});}case 17: _tmp313=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp312)->f1;{void*t=_tmp313;
# 2243
void*_tmp336=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp336;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp630=e;Cyc_Tcutil_copye(_tmp630,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337->tag=17,_tmp337->f1=new_typ;_tmp337;}));});}case 18: _tmp313=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp312)->f1;{struct Cyc_Absyn_Exp*e1=_tmp313;
# 2246
struct Cyc_Absyn_Exp*_tmp338=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp338;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp631=e;Cyc_Tcutil_copye(_tmp631,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->tag=18,_tmp339->f1=new_e1;_tmp339;}));});}case 41: _tmp313=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp312)->f1;{struct Cyc_Absyn_Exp*e1=_tmp313;
# 2249
struct Cyc_Absyn_Exp*_tmp33A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp33A;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp632=e;Cyc_Tcutil_copye(_tmp632,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->tag=41,_tmp33B->f1=new_e1;_tmp33B;}));});}case 19: _tmp313=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp312)->f1;_tmp314=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp312)->f2;{void*t=_tmp313;struct Cyc_List_List*f=_tmp314;
# 2252
void*_tmp33C=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp33C;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp633=e;Cyc_Tcutil_copye(_tmp633,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->tag=19,_tmp33D->f1=new_typ,_tmp33D->f2=f;_tmp33D;}));});}case 39: _tmp313=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp312)->f1;{void*t=_tmp313;
# 2255
void*_tmp33E=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp33E;
if(new_typ == t)return e;{
# 2258
void*_tmp33F=Cyc_Absyn_compress(new_typ);void*_stmttmp5E=_tmp33F;void*_tmp340=_stmttmp5E;void*_tmp341;if(*((int*)_tmp340)== 9){_tmp341=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp340)->f1;{struct Cyc_Absyn_Exp*e=_tmp341;
return e;}}else{
return({struct Cyc_Absyn_Exp*_tmp634=e;Cyc_Tcutil_copye(_tmp634,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342->tag=39,_tmp342->f1=new_typ;_tmp342;}));});};}}default:
# 2262
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp344=({struct Cyc_Warn_String_Warn_Warg_struct _tmp507;_tmp507.tag=0,({struct _fat_ptr _tmp635=({const char*_tmp345="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp345,sizeof(char),46U);});_tmp507.f1=_tmp635;});_tmp507;});void*_tmp343[1];_tmp343[0]=& _tmp344;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp343,sizeof(void*),1));});};}
# 2266
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2269
return e == 0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2272
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2275
void*_tmp346=f->type;void*t=_tmp346;
struct Cyc_Absyn_Exp*_tmp347=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp347;
void*_tmp348=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp348;
struct Cyc_Absyn_Exp*_tmp349=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp349;
if(t == new_typ && req == new_req)
return f;{
struct Cyc_Absyn_Aggrfield*_tmp34A=({struct Cyc_Absyn_Aggrfield*_tmp34B=_cycalloc(sizeof(*_tmp34B));*_tmp34B=*f;_tmp34B;});struct Cyc_Absyn_Aggrfield*ans=_tmp34A;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2287
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2290
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp34C=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp34C;
struct Cyc_List_List*_tmp34D=fs->tl;struct Cyc_List_List*t=_tmp34D;
struct Cyc_Absyn_Aggrfield*_tmp34E=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp34E;
struct Cyc_List_List*_tmp34F=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_typ=_tmp34F;
return new_f == f && new_typ == t?fs:({struct Cyc_List_List*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350->hd=new_f,_tmp350->tl=new_typ;_tmp350;});}}
# 2298
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2301
struct _tuple0 _tmp351=((struct _tuple0(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp5F=_tmp351;struct _tuple0 _tmp352=_stmttmp5F;void*_tmp354;void*_tmp353;_tmp353=_tmp352.f1;_tmp354=_tmp352.f2;{struct Cyc_List_List*rpo1a=_tmp353;struct Cyc_List_List*rpo1b=_tmp354;
struct Cyc_List_List*_tmp355=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp355;
struct Cyc_List_List*_tmp356=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp356;
return rpo2a == rpo1a && rpo2b == rpo1b?rgn_po:((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip)(rpo2a,rpo2b);}}
# 2309
static int Cyc_Tcutil_skip_fntypes=0;
# 2311
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2314
void*_tmp357=Cyc_Absyn_compress(t);void*_stmttmp60=_tmp357;void*_tmp358=_stmttmp60;enum Cyc_Absyn_AggrKind _tmp35B;void*_tmp365;void*_tmp364;void*_tmp363;void*_tmp362;int _tmp35F;void*_tmp361;void*_tmp360;unsigned _tmp366;struct Cyc_Absyn_Tqual _tmp35C;void*_tmp35E;void*_tmp35D;void*_tmp35A;void*_tmp359;switch(*((int*)_tmp358)){case 2: _tmp359=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp358)->f1;{struct Cyc_Absyn_Tvar*v=_tmp359;
# 2317
struct _handler_cons _tmp367;_push_handler(& _tmp367);{int _tmp369=0;if(setjmp(_tmp367.handler))_tmp369=1;if(!_tmp369){{void*_tmp36A=((void*(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0);return _tmp36A;};_pop_handler();}else{void*_tmp368=(void*)Cyc_Core_get_exn_thrown();void*_tmp36B=_tmp368;void*_tmp36C;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp36B)->tag == Cyc_Core_Not_found)
return t;else{_tmp36C=_tmp36B;{void*exn=_tmp36C;(int)_rethrow(exn);}};}}}case 8: _tmp359=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp358)->f1;_tmp35A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp358)->f2;_tmp35D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp358)->f3;_tmp35E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp358)->f4;{struct _tuple1*n=_tmp359;struct Cyc_List_List*ts=_tmp35A;struct Cyc_Absyn_Typedefdecl*td=_tmp35D;void*topt=_tmp35E;
# 2320
struct Cyc_List_List*_tmp36D=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp36D;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->tag=8,_tmp36E->f1=n,_tmp36E->f2=new_ts,_tmp36E->f3=td,_tmp36E->f4=topt;_tmp36E;});}case 4: _tmp359=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).elt_type;_tmp35C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).tq;_tmp35A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).num_elts;_tmp35D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).zero_term;_tmp366=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).zt_loc;{void*t1=_tmp359;struct Cyc_Absyn_Tqual tq=_tmp35C;struct Cyc_Absyn_Exp*e=_tmp35A;void*zt=_tmp35D;unsigned ztl=_tmp366;
# 2323
void*_tmp36F=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp36F;
struct Cyc_Absyn_Exp*_tmp370=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp370;
void*_tmp371=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp371;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp372=_cycalloc(sizeof(*_tmp372));
_tmp372->tag=4,(_tmp372->f1).elt_type=new_t1,(_tmp372->f1).tq=tq,(_tmp372->f1).num_elts=new_e,(_tmp372->f1).zero_term=new_zt,(_tmp372->f1).zt_loc=ztl;_tmp372;});}case 3: _tmp359=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).elt_type;_tmp35C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).elt_tq;_tmp35A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).ptr_atts).rgn;_tmp35D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).ptr_atts).nullable;_tmp35E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).ptr_atts).bounds;_tmp360=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).ptr_atts).zero_term;_tmp361=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).ptr_atts).released;{void*t1=_tmp359;struct Cyc_Absyn_Tqual tq=_tmp35C;void*r=_tmp35A;void*n=_tmp35D;void*b=_tmp35E;void*zt=_tmp360;void*rel=_tmp361;
# 2329
void*_tmp373=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp373;
void*_tmp374=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp374;
void*_tmp375=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp375;
void*_tmp376=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp376;
void*_tmp377=Cyc_Tcutil_rsubstitute(rgn,inst,rel);void*new_rel=_tmp377;
if((((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)&& new_rel == rel)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp508;_tmp508.elt_type=new_t1,_tmp508.elt_tq=tq,(_tmp508.ptr_atts).rgn=new_r,(_tmp508.ptr_atts).nullable=n,(_tmp508.ptr_atts).bounds=new_b,(_tmp508.ptr_atts).zero_term=new_zt,(_tmp508.ptr_atts).ptrloc=0,(_tmp508.ptr_atts).released=new_rel;_tmp508;}));}case 5: _tmp359=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).tvars;_tmp35A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).effect;_tmp35C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).ret_tqual;_tmp35D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).ret_type;_tmp35E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).args;_tmp35F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).c_varargs;_tmp360=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).cyc_varargs;_tmp361=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).rgn_po;_tmp362=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).attributes;_tmp363=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).requires_clause;_tmp364=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).ensures_clause;_tmp365=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp358)->f1).return_value;{struct Cyc_List_List*vs=_tmp359;void*eff=_tmp35A;struct Cyc_Absyn_Tqual rtq=_tmp35C;void*rtyp=_tmp35D;struct Cyc_List_List*args=_tmp35E;int c_varargs=_tmp35F;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp360;struct Cyc_List_List*rgn_po=_tmp361;struct Cyc_List_List*atts=_tmp362;struct Cyc_Absyn_Exp*req=_tmp363;struct Cyc_Absyn_Exp*ens=_tmp364;struct Cyc_Absyn_Vardecl*ret_value=_tmp365;
# 2339
if(Cyc_Tcutil_skip_fntypes)return t;
# 2341
{struct Cyc_List_List*_tmp378=vs;struct Cyc_List_List*p=_tmp378;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp37A=_region_malloc(rgn,sizeof(*_tmp37A));({struct _tuple17*_tmp637=({struct _tuple17*_tmp379=_region_malloc(rgn,sizeof(*_tmp379));_tmp379->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp636=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp379->f2=_tmp636;});_tmp379;});_tmp37A->hd=_tmp637;}),_tmp37A->tl=inst;_tmp37A;});}}{
struct _tuple0 _tmp37B=({struct _RegionHandle*_tmp639=rgn;struct _RegionHandle*_tmp638=rgn;((struct _tuple0(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rsplit)(_tmp639,_tmp638,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple25*(*)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2343
struct _tuple0 _stmttmp61=_tmp37B;struct _tuple0 _tmp37C=_stmttmp61;void*_tmp37E;void*_tmp37D;_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;{struct Cyc_List_List*qs=_tmp37D;struct Cyc_List_List*ts=_tmp37E;
# 2345
struct Cyc_List_List*_tmp37F=args;struct Cyc_List_List*args2=_tmp37F;
struct Cyc_List_List*_tmp380=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp380;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff == 0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2351
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2355
struct Cyc_Absyn_VarargInfo _tmp381=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp62=_tmp381;struct Cyc_Absyn_VarargInfo _tmp382=_stmttmp62;int _tmp386;void*_tmp385;struct Cyc_Absyn_Tqual _tmp384;void*_tmp383;_tmp383=_tmp382.name;_tmp384=_tmp382.tq;_tmp385=_tmp382.type;_tmp386=_tmp382.inject;{struct _fat_ptr*n=_tmp383;struct Cyc_Absyn_Tqual tq=_tmp384;void*t=_tmp385;int i=_tmp386;
void*_tmp387=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp387;
cyc_varargs2=t2 == t?cyc_varargs:({struct Cyc_Absyn_VarargInfo*_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388->name=n,_tmp388->tq=tq,_tmp388->type=t2,_tmp388->inject=i;_tmp388;});}}{
# 2359
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp389=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp389;
struct Cyc_List_List*_tmp38A=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp38A;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B->tag=5,(_tmp38B->f1).tvars=vs,(_tmp38B->f1).effect=eff2,(_tmp38B->f1).ret_tqual=rtq,({void*_tmp63A=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp38B->f1).ret_type=_tmp63A;}),(_tmp38B->f1).args=args2,(_tmp38B->f1).c_varargs=c_varargs,(_tmp38B->f1).cyc_varargs=cyc_varargs2,(_tmp38B->f1).rgn_po=rgn_po2,(_tmp38B->f1).attributes=atts,(_tmp38B->f1).requires_clause=req2,(_tmp38B->f1).requires_relns=req_relns2,(_tmp38B->f1).ensures_clause=ens2,(_tmp38B->f1).ensures_relns=ens_relns2,(_tmp38B->f1).return_value=ret_value;_tmp38B;});}}}}}case 6: _tmp359=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp358)->f1;{struct Cyc_List_List*tqts=_tmp359;
# 2368
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp38C=tqts;struct Cyc_List_List*ts1=_tmp38C;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp38D=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp38D;
void*_tmp38E=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp38E;
if(t1 != t2)
change=1;
ts2=({struct Cyc_List_List*_tmp38F=_region_malloc(rgn,sizeof(*_tmp38F));_tmp38F->hd=t2,_tmp38F->tl=ts2;_tmp38F;});}}
# 2377
return !change?t:(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->tag=6,({struct Cyc_List_List*_tmp63C=({struct Cyc_List_List*_tmp63B=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp63B,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ts2));});_tmp390->f1=_tmp63C;});_tmp390;});}case 7: _tmp35B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp358)->f1;_tmp359=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp358)->f2;{enum Cyc_Absyn_AggrKind k=_tmp35B;struct Cyc_List_List*fs=_tmp359;
# 2379
struct Cyc_List_List*_tmp391=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp391;
return fs == new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->tag=7,_tmp392->f1=k,_tmp392->f2=new_fs;_tmp392;});}case 1: _tmp359=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp358)->f2;{void*r=_tmp359;
# 2382
return r == 0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0: _tmp359=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp358)->f1;_tmp35A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp358)->f2;{void*c=_tmp359;struct Cyc_List_List*ts=_tmp35A;
# 2384
struct Cyc_List_List*_tmp393=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp393;
return ts == new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->tag=0,_tmp394->f1=c,_tmp394->f2=new_ts;_tmp394;});}case 9: _tmp359=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp358)->f1;{struct Cyc_Absyn_Exp*e=_tmp359;
# 2387
struct Cyc_Absyn_Exp*_tmp395=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp395;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->tag=9,_tmp396->f1=new_e;_tmp396;});}case 11: _tmp359=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp358)->f1;{struct Cyc_Absyn_Exp*e=_tmp359;
# 2390
struct Cyc_Absyn_Exp*_tmp397=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp397;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->tag=11,_tmp398->f1=new_e;_tmp398;});}default:
# 2393
 if(Cyc_Tcutil_skip_fntypes)return t;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp39A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp509;_tmp509.tag=0,({struct _fat_ptr _tmp63D=({const char*_tmp39B="found typedecltype in rsubs";_tag_fat(_tmp39B,sizeof(char),28U);});_tmp509.f1=_tmp63D;});_tmp509;});void*_tmp399[1];_tmp399[0]=& _tmp39A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp399,sizeof(void*),1));});};}
# 2398
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2401
if(ts == 0)
return 0;{
void*_tmp39C=(void*)ts->hd;void*old_hd=_tmp39C;
struct Cyc_List_List*_tmp39D=ts->tl;struct Cyc_List_List*old_tl=_tmp39D;
void*_tmp39E=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp39E;
struct Cyc_List_List*_tmp39F=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp39F;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->hd=new_hd,_tmp3A0->tl=new_tl;_tmp3A0;});}}
# 2412
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return inst == 0?t: Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}
# 2416
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t){
Cyc_Tcutil_skip_fntypes=1;{
void*_tmp3A1=Cyc_Tcutil_substitute(inst,t);void*new_typ=_tmp3A1;
Cyc_Tcutil_skip_fntypes=0;
return new_typ;}}
# 2424
struct _tuple17*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp3A2=Cyc_Kinds_kind_to_opt(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk));struct Cyc_Core_Opt*k=_tmp3A2;
return({struct _tuple17*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->f1=tv,({void*_tmp63F=({struct Cyc_Core_Opt*_tmp63E=k;Cyc_Absyn_new_evar(_tmp63E,({struct Cyc_Core_Opt*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->v=s;_tmp3A3;}));});_tmp3A4->f2=_tmp63F;});_tmp3A4;});}struct _tuple26{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2429
struct _tuple17*Cyc_Tcutil_r_make_inst_var(struct _tuple26*env,struct Cyc_Absyn_Tvar*tv){
# 2431
struct _tuple26*_tmp3A5=env;void*_tmp3A7;void*_tmp3A6;_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;{struct Cyc_List_List*s=_tmp3A6;struct _RegionHandle*rgn=_tmp3A7;
struct Cyc_Core_Opt*_tmp3A8=Cyc_Kinds_kind_to_opt(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk));struct Cyc_Core_Opt*k=_tmp3A8;
return({struct _tuple17*_tmp3AA=_region_malloc(rgn,sizeof(*_tmp3AA));_tmp3AA->f1=tv,({void*_tmp641=({struct Cyc_Core_Opt*_tmp640=k;Cyc_Absyn_new_evar(_tmp640,({struct Cyc_Core_Opt*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->v=s;_tmp3A9;}));});_tmp3AA->f2=_tmp641;});_tmp3AA;});}}
# 2436
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2438
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50C;_tmp50C.tag=0,({struct _fat_ptr _tmp642=({const char*_tmp3B0="bitfield ";_tag_fat(_tmp3B0,sizeof(char),10U);});_tmp50C.f1=_tmp642;});_tmp50C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50B;_tmp50B.tag=0,_tmp50B.f1=*fn;_tmp50B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50A;_tmp50A.tag=0,({struct _fat_ptr _tmp643=({const char*_tmp3AF=" does not have constant width";_tag_fat(_tmp3AF,sizeof(char),30U);});_tmp50A.f1=_tmp643;});_tmp50A;});void*_tmp3AB[3];_tmp3AB[0]=& _tmp3AC,_tmp3AB[1]=& _tmp3AD,_tmp3AB[2]=& _tmp3AE;({unsigned _tmp644=loc;Cyc_Warn_err2(_tmp644,_tag_fat(_tmp3AB,sizeof(void*),3));});});else{
# 2443
struct _tuple13 _tmp3B1=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp63=_tmp3B1;struct _tuple13 _tmp3B2=_stmttmp63;int _tmp3B4;unsigned _tmp3B3;_tmp3B3=_tmp3B2.f1;_tmp3B4=_tmp3B2.f2;{unsigned i=_tmp3B3;int known=_tmp3B4;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50D;_tmp50D.tag=0,({struct _fat_ptr _tmp645=({const char*_tmp3B7="cannot evaluate bitfield width at compile time";_tag_fat(_tmp3B7,sizeof(char),47U);});_tmp50D.f1=_tmp645;});_tmp50D;});void*_tmp3B5[1];_tmp3B5[0]=& _tmp3B6;({unsigned _tmp646=loc;Cyc_Warn_warn2(_tmp646,_tag_fat(_tmp3B5,sizeof(void*),1));});});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50E;_tmp50E.tag=0,({struct _fat_ptr _tmp647=({const char*_tmp3BA="bitfield has negative width";_tag_fat(_tmp3BA,sizeof(char),28U);});_tmp50E.f1=_tmp647;});_tmp50E;});void*_tmp3B8[1];_tmp3B8[0]=& _tmp3B9;({unsigned _tmp648=loc;Cyc_Warn_err2(_tmp648,_tag_fat(_tmp3B8,sizeof(void*),1));});});
w=i;}}{
# 2450
void*_tmp3BB=Cyc_Absyn_compress(field_type);void*_stmttmp64=_tmp3BB;void*_tmp3BC=_stmttmp64;enum Cyc_Absyn_Size_of _tmp3BD;if(*((int*)_tmp3BC)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BC)->f1)== 1){_tmp3BD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BC)->f1)->f2;{enum Cyc_Absyn_Size_of b=_tmp3BD;
# 2453
int bad;
{enum Cyc_Absyn_Size_of _tmp3BE=b;switch((int)_tmp3BE){case Cyc_Absyn_Char_sz:
 bad=w > 8U;goto _LL8;case Cyc_Absyn_Short_sz:
 bad=w > 16U;goto _LL8;case Cyc_Absyn_Long_sz:
 goto _LL10;case Cyc_Absyn_Int_sz: _LL10:
 bad=w > 32U;goto _LL8;case Cyc_Absyn_LongLong_sz:
 goto _LL14;default: _LL14:
 bad=w > 64U;goto _LL8;}_LL8:;}
# 2462
if(bad)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50F;_tmp50F.tag=0,({struct _fat_ptr _tmp649=({const char*_tmp3C1="bitfield larger than type";_tag_fat(_tmp3C1,sizeof(char),26U);});_tmp50F.f1=_tmp649;});_tmp50F;});void*_tmp3BF[1];_tmp3BF[0]=& _tmp3C0;({unsigned _tmp64A=loc;Cyc_Warn_warn2(_tmp64A,_tag_fat(_tmp3BF,sizeof(void*),1));});});
goto _LL3;}}else{goto _LL6;}}else{_LL6:
# 2466
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp513;_tmp513.tag=0,({struct _fat_ptr _tmp64B=({const char*_tmp3C8="bitfield ";_tag_fat(_tmp3C8,sizeof(char),10U);});_tmp513.f1=_tmp64B;});_tmp513;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp512;_tmp512.tag=0,_tmp512.f1=*fn;_tmp512;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp511;_tmp511.tag=0,({struct _fat_ptr _tmp64C=({const char*_tmp3C7=" must have integral type but has type ";_tag_fat(_tmp3C7,sizeof(char),39U);});_tmp511.f1=_tmp64C;});_tmp511;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3C6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp510;_tmp510.tag=2,_tmp510.f1=(void*)field_type;_tmp510;});void*_tmp3C2[4];_tmp3C2[0]=& _tmp3C3,_tmp3C2[1]=& _tmp3C4,_tmp3C2[2]=& _tmp3C5,_tmp3C2[3]=& _tmp3C6;({unsigned _tmp64D=loc;Cyc_Warn_err2(_tmp64D,_tag_fat(_tmp3C2,sizeof(void*),4));});});
# 2468
goto _LL3;}_LL3:;}}}
# 2475
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp3C9=t;void*_tmp3CB;void*_tmp3CA;if(*((int*)_tmp3C9)== 8){_tmp3CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3C9)->f3;_tmp3CB=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3C9)->f4;{struct Cyc_Absyn_Typedefdecl*td=_tmp3CA;void*tdopt=_tmp3CB;
# 2478
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp514;_tmp514.tag=0,({struct _fat_ptr _tmp64E=({const char*_tmp3CE="extra const";_tag_fat(_tmp3CE,sizeof(char),12U);});_tmp514.f1=_tmp64E;});_tmp514;});void*_tmp3CC[1];_tmp3CC[0]=& _tmp3CD;({unsigned _tmp64F=loc;Cyc_Warn_warn2(_tmp64F,_tag_fat(_tmp3CC,sizeof(void*),1));});});
return 1;}
# 2483
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);
return declared_const;}}else{
return declared_const;};}
# 2490
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == -1)
({int _tmp650=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp650;});}
# 2494
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2500
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2503
for(1;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp517;_tmp517.tag=0,_tmp517.f1=msg;_tmp517;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp516;_tmp516.tag=0,({struct _fat_ptr _tmp651=({const char*_tmp3D3=": ";_tag_fat(_tmp3D3,sizeof(char),3U);});_tmp516.f1=_tmp651;});_tmp516;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp515;_tmp515.tag=0,({struct _fat_ptr _tmp652=a2string(vs->hd);_tmp515.f1=_tmp652;});_tmp515;});void*_tmp3CF[3];_tmp3CF[0]=& _tmp3D0,_tmp3CF[1]=& _tmp3D1,_tmp3CF[2]=& _tmp3D2;({unsigned _tmp653=loc;Cyc_Warn_err2(_tmp653,_tag_fat(_tmp3CF,sizeof(void*),3));});});}}}
# 2508
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2511
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,unsigned,struct _fat_ptr(*)(struct _fat_ptr*),struct _fat_ptr))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2514
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp655=tvs;unsigned _tmp654=loc;((void(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,unsigned,struct _fat_ptr(*)(struct Cyc_Absyn_Tvar*),struct _fat_ptr))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp655,_tmp654,Cyc_Absynpp_tvar2string,({const char*_tmp3D4="duplicate type variable";_tag_fat(_tmp3D4,sizeof(char),24U);}));});}struct _tuple27{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple28{struct Cyc_List_List*f1;void*f2;};struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2529 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2534
struct _RegionHandle _tmp3D5=_new_region("temp");struct _RegionHandle*temp=& _tmp3D5;_push_region(temp);
# 2538
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp656=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp656,({const char*_tmp3D6="";_tag_fat(_tmp3D6,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp3D8=_region_malloc(temp,sizeof(*_tmp3D8));({struct _tuple27*_tmp657=({struct _tuple27*_tmp3D7=_region_malloc(temp,sizeof(*_tmp3D7));_tmp3D7->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp3D7->f2=0;_tmp3D7;});_tmp3D8->hd=_tmp657;}),_tmp3D8->tl=fields;_tmp3D8;});}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(fields);{
# 2544
struct _fat_ptr aggr_str=(int)aggr_kind == 0?({const char*_tmp404="struct";_tag_fat(_tmp404,sizeof(char),7U);}):({const char*_tmp405="union";_tag_fat(_tmp405,sizeof(char),6U);});
# 2547
struct Cyc_List_List*ans=0;
for(1;des != 0;des=des->tl){
struct _tuple28*_tmp3D9=(struct _tuple28*)des->hd;struct _tuple28*_stmttmp65=_tmp3D9;struct _tuple28*_tmp3DA=_stmttmp65;void*_tmp3DC;void*_tmp3DB;_tmp3DB=_tmp3DA->f1;_tmp3DC=_tmp3DA->f2;{struct Cyc_List_List*dl=_tmp3DB;void*a=_tmp3DC;
if(dl == 0){
# 2552
struct Cyc_List_List*_tmp3DD=fields;struct Cyc_List_List*fields2=_tmp3DD;
for(1;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple27*)fields2->hd)).f2){
(*((struct _tuple27*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp659=({struct Cyc_List_List*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));({void*_tmp658=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->tag=1,_tmp3DE->f1=((*((struct _tuple27*)fields2->hd)).f1)->name;_tmp3DE;});_tmp3DF->hd=_tmp658;}),_tmp3DF->tl=0;_tmp3DF;});(*((struct _tuple28*)des->hd)).f1=_tmp659;});
ans=({struct Cyc_List_List*_tmp3E1=_region_malloc(rgn,sizeof(*_tmp3E1));({struct _tuple29*_tmp65A=({struct _tuple29*_tmp3E0=_region_malloc(rgn,sizeof(*_tmp3E0));_tmp3E0->f1=(*((struct _tuple27*)fields2->hd)).f1,_tmp3E0->f2=a;_tmp3E0;});_tmp3E1->hd=_tmp65A;}),_tmp3E1->tl=ans;_tmp3E1;});
break;}}
# 2560
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp519;_tmp519.tag=0,({struct _fat_ptr _tmp65B=({const char*_tmp3E5="too many arguments to ";_tag_fat(_tmp3E5,sizeof(char),23U);});_tmp519.f1=_tmp65B;});_tmp519;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp518;_tmp518.tag=0,_tmp518.f1=aggr_str;_tmp518;});void*_tmp3E2[2];_tmp3E2[0]=& _tmp3E3,_tmp3E2[1]=& _tmp3E4;({unsigned _tmp65C=loc;Cyc_Warn_err2(_tmp65C,_tag_fat(_tmp3E2,sizeof(void*),2));});});}else{
if(dl->tl != 0)
# 2564
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51A;_tmp51A.tag=0,({struct _fat_ptr _tmp65D=({const char*_tmp3E8="multiple designators are not yet supported";_tag_fat(_tmp3E8,sizeof(char),43U);});_tmp51A.f1=_tmp65D;});_tmp51A;});void*_tmp3E6[1];_tmp3E6[0]=& _tmp3E7;({unsigned _tmp65E=loc;Cyc_Warn_err2(_tmp65E,_tag_fat(_tmp3E6,sizeof(void*),1));});});else{
# 2567
void*_tmp3E9=(void*)dl->hd;void*_stmttmp66=_tmp3E9;void*_tmp3EA=_stmttmp66;void*_tmp3EB;if(*((int*)_tmp3EA)== 0){
# 2569
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51C;_tmp51C.tag=0,({struct _fat_ptr _tmp65F=({const char*_tmp3EF="array designator used in argument to ";_tag_fat(_tmp3EF,sizeof(char),38U);});_tmp51C.f1=_tmp65F;});_tmp51C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51B;_tmp51B.tag=0,_tmp51B.f1=aggr_str;_tmp51B;});void*_tmp3EC[2];_tmp3EC[0]=& _tmp3ED,_tmp3EC[1]=& _tmp3EE;({unsigned _tmp660=loc;Cyc_Warn_err2(_tmp660,_tag_fat(_tmp3EC,sizeof(void*),2));});});
goto _LL3;}else{_tmp3EB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3EA)->f1;{struct _fat_ptr*v=_tmp3EB;
# 2572
struct Cyc_List_List*_tmp3F0=fields;struct Cyc_List_List*fields2=_tmp3F0;
for(1;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple27*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple27*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51F;_tmp51F.tag=0,({struct _fat_ptr _tmp661=({const char*_tmp3F6="member ";_tag_fat(_tmp3F6,sizeof(char),8U);});_tmp51F.f1=_tmp661;});_tmp51F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51E;_tmp51E.tag=0,_tmp51E.f1=*v;_tmp51E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51D;_tmp51D.tag=0,({struct _fat_ptr _tmp662=({const char*_tmp3F5=" has already been used as an argument";_tag_fat(_tmp3F5,sizeof(char),38U);});_tmp51D.f1=_tmp662;});_tmp51D;});void*_tmp3F1[3];_tmp3F1[0]=& _tmp3F2,_tmp3F1[1]=& _tmp3F3,_tmp3F1[2]=& _tmp3F4;({unsigned _tmp663=loc;Cyc_Warn_err2(_tmp663,_tag_fat(_tmp3F1,sizeof(void*),3));});});
(*((struct _tuple27*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp3F8=_region_malloc(rgn,sizeof(*_tmp3F8));({struct _tuple29*_tmp664=({struct _tuple29*_tmp3F7=_region_malloc(rgn,sizeof(*_tmp3F7));_tmp3F7->f1=(*((struct _tuple27*)fields2->hd)).f1,_tmp3F7->f2=a;_tmp3F7;});_tmp3F8->hd=_tmp664;}),_tmp3F8->tl=ans;_tmp3F8;});
break;}}
# 2581
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp521;_tmp521.tag=0,({struct _fat_ptr _tmp665=({const char*_tmp3FC="bad field designator ";_tag_fat(_tmp3FC,sizeof(char),22U);});_tmp521.f1=_tmp665;});_tmp521;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp520;_tmp520.tag=0,_tmp520.f1=*v;_tmp520;});void*_tmp3F9[2];_tmp3F9[0]=& _tmp3FA,_tmp3F9[1]=& _tmp3FB;({unsigned _tmp666=loc;Cyc_Warn_err2(_tmp666,_tag_fat(_tmp3F9,sizeof(void*),2));});});
goto _LL3;}}_LL3:;}}}}
# 2588
if((int)aggr_kind == 0){
if(({int _tmp667=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ans);_tmp667 < ((int(*)(struct Cyc_List_List*))Cyc_List_length)(fields);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp522;_tmp522.tag=0,({struct _fat_ptr _tmp668=({const char*_tmp3FF="too few arguments to struct";_tag_fat(_tmp3FF,sizeof(char),28U);});_tmp522.f1=_tmp668;});_tmp522;});void*_tmp3FD[1];_tmp3FD[0]=& _tmp3FE;({unsigned _tmp669=loc;Cyc_Warn_err2(_tmp669,_tag_fat(_tmp3FD,sizeof(void*),1));});});}else{
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(ans)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp401=({struct Cyc_Warn_String_Warn_Warg_struct _tmp523;_tmp523.tag=0,({struct _fat_ptr _tmp66A=({const char*_tmp402="union expression should have one member";_tag_fat(_tmp402,sizeof(char),40U);});_tmp523.f1=_tmp66A;});_tmp523;});void*_tmp400[1];_tmp400[0]=& _tmp401;({unsigned _tmp66B=loc;Cyc_Warn_err2(_tmp66B,_tag_fat(_tmp400,sizeof(void*),1));});});}{
struct Cyc_List_List*_tmp403=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmp403;}}}
# 2538
;_pop_region();}
# 2599
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2601
void*_tmp406=e1->r;void*_stmttmp67=_tmp406;void*_tmp407=_stmttmp67;void*_tmp408;switch(*((int*)_tmp407)){case 14:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp525;_tmp525.tag=0,({struct _fat_ptr _tmp66C=({const char*_tmp40C="we have a cast in a lhs: ";_tag_fat(_tmp40C,sizeof(char),26U);});_tmp525.f1=_tmp66C;});_tmp525;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp40B=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp524;_tmp524.tag=4,_tmp524.f1=e1;_tmp524;});void*_tmp409[2];_tmp409[0]=& _tmp40A,_tmp409[1]=& _tmp40B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp409,sizeof(void*),2));});case 20: _tmp408=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp407)->f1;{struct Cyc_Absyn_Exp*e1a=_tmp408;
_tmp408=e1a;goto _LL6;}case 23: _tmp408=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp407)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp408;
# 2605
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22: _tmp408=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp407)->f1;{struct Cyc_Absyn_Exp*e1a=_tmp408;
_tmp408=e1a;goto _LLA;}case 21: _tmp408=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp407)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp408;
# 2609
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp527;_tmp527.tag=0,({struct _fat_ptr _tmp66D=({const char*_tmp410="found zero pointer aggregate member assignment: ";_tag_fat(_tmp410,sizeof(char),49U);});_tmp527.f1=_tmp66D;});_tmp527;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp40F=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp526;_tmp526.tag=4,_tmp526.f1=e1;_tmp526;});void*_tmp40D[2];_tmp40D[0]=& _tmp40E,_tmp40D[1]=& _tmp40F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp40D,sizeof(void*),2));});
return 0;}case 13: _tmp408=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp407)->f1;{struct Cyc_Absyn_Exp*e1a=_tmp408;
_tmp408=e1a;goto _LLE;}case 12: _tmp408=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp407)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp408;
# 2614
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp412=({struct Cyc_Warn_String_Warn_Warg_struct _tmp529;_tmp529.tag=0,({struct _fat_ptr _tmp66E=({const char*_tmp414="found zero pointer instantiate/noinstantiate: ";_tag_fat(_tmp414,sizeof(char),47U);});_tmp529.f1=_tmp66E;});_tmp529;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp413=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp528;_tmp528.tag=4,_tmp528.f1=e1;_tmp528;});void*_tmp411[2];_tmp411[0]=& _tmp412,_tmp411[1]=& _tmp413;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp411,sizeof(void*),2));});
return 0;}case 1:
 return 0;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp416=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52B;_tmp52B.tag=0,({struct _fat_ptr _tmp66F=({const char*_tmp418="found bad lhs in is_zero_ptr_deref: ";_tag_fat(_tmp418,sizeof(char),37U);});_tmp52B.f1=_tmp66F;});_tmp52B;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp417=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp52A;_tmp52A.tag=4,_tmp52A.f1=e1;_tmp52A;});void*_tmp415[2];_tmp415[0]=& _tmp416,_tmp415[1]=& _tmp417;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp415,sizeof(void*),2));});};}
# 2628
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmp419=Cyc_Absyn_compress(r);void*_stmttmp68=_tmp419;void*_tmp41A=_stmttmp68;void*_tmp41B;enum Cyc_Absyn_AliasQual _tmp41D;enum Cyc_Absyn_KindQual _tmp41C;switch(*((int*)_tmp41A)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41A)->f1)){case 7:
 return !must_be_unique;case 6:
 return 1;default: goto _LL9;}case 8: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp41A)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp41A)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp41A)->f4 == 0){_tmp41C=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp41A)->f3)->kind)->v)->kind;_tmp41D=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp41A)->f3)->kind)->v)->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp41C;enum Cyc_Absyn_AliasQual a=_tmp41D;
# 2633
return(int)k == 3 &&((int)a == 1 ||(int)a == 2 && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2: _tmp41B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp41A)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp41B;
# 2637
struct Cyc_Absyn_Kind*_tmp41E=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_rk);struct Cyc_Absyn_Kind*_stmttmp69=_tmp41E;struct Cyc_Absyn_Kind*_tmp41F=_stmttmp69;enum Cyc_Absyn_AliasQual _tmp421;enum Cyc_Absyn_KindQual _tmp420;_tmp420=_tmp41F->kind;_tmp421=_tmp41F->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp420;enum Cyc_Absyn_AliasQual a=_tmp421;
if((int)k == 3 &&((int)a == 1 ||(int)a == 2 && !must_be_unique)){
void*_tmp422=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp6A=_tmp422;void*_tmp423=_stmttmp6A;void*_tmp424;if(*((int*)_tmp423)== 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp423)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp423)->f2)->aliasqual == Cyc_Absyn_Top){_tmp424=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp423)->f1;{struct Cyc_Core_Opt**x=(struct Cyc_Core_Opt**)_tmp424;
# 2641
({struct Cyc_Core_Opt*_tmp671=({struct Cyc_Core_Opt*_tmp426=_cycalloc(sizeof(*_tmp426));({void*_tmp670=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->tag=2,_tmp425->f1=0,_tmp425->f2=& Cyc_Kinds_rk;_tmp425;});_tmp426->v=_tmp670;});_tmp426;});*x=_tmp671;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11:
 return 1;};}
# 2645
return 0;}}default: _LL9:
 return 0;};}
# 2652
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp427=Cyc_Absyn_compress(t);void*_stmttmp6B=_tmp427;void*_tmp428=_stmttmp6B;void*_tmp429;switch(*((int*)_tmp428)){case 3: _tmp429=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp428)->f1).ptr_atts).rgn;{void*r=_tmp429;
# 2655
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2: _tmp429=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp428)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp429;
# 2657
struct Cyc_Absyn_Kind*_tmp42A=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_stmttmp6C=_tmp42A;struct Cyc_Absyn_Kind*_tmp42B=_stmttmp6C;enum Cyc_Absyn_AliasQual _tmp42D;enum Cyc_Absyn_KindQual _tmp42C;_tmp42C=_tmp42B->kind;_tmp42D=_tmp42B->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp42C;enum Cyc_Absyn_AliasQual a=_tmp42D;
enum Cyc_Absyn_KindQual _tmp42E=k;switch((int)_tmp42E){case Cyc_Absyn_BoxKind:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LL10:
 if((int)a == 1 ||(int)a == 2 && !must_be_unique){
void*_tmp42F=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp6D=_tmp42F;void*_tmp430=_stmttmp6D;enum Cyc_Absyn_KindQual _tmp432;void*_tmp431;if(*((int*)_tmp430)== 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp430)->f2)->aliasqual == Cyc_Absyn_Top){_tmp431=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp430)->f1;_tmp432=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp430)->f2)->kind;{struct Cyc_Core_Opt**x=(struct Cyc_Core_Opt**)_tmp431;enum Cyc_Absyn_KindQual k=_tmp432;
# 2663
({struct Cyc_Core_Opt*_tmp674=({struct Cyc_Core_Opt*_tmp435=_cycalloc(sizeof(*_tmp435));({void*_tmp673=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->tag=2,_tmp434->f1=0,({struct Cyc_Absyn_Kind*_tmp672=({struct Cyc_Absyn_Kind*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->kind=k,_tmp433->aliasqual=0U;_tmp433;});_tmp434->f2=_tmp672;});_tmp434;});_tmp435->v=_tmp673;});_tmp435;});*x=_tmp674;});
return 0;}}else{goto _LL16;}}else{_LL16:
 return 1;};}
# 2667
return 0;default:
 return 0;};}}default:
# 2670
 return 0;};}
# 2673
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
t=Cyc_Absyn_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(t,0))
return 1;{
void*_tmp436=t;union Cyc_Absyn_DatatypeFieldInfo _tmp438;void*_tmp439;void*_tmp437;switch(*((int*)_tmp436)){case 6: _tmp437=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp436)->f1;{struct Cyc_List_List*qts=_tmp437;
# 2679
for(1;qts != 0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)){case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->f1).KnownAggr).tag == 2){_tmp437=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->f1).KnownAggr).val;_tmp439=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f2;{struct Cyc_Absyn_Aggrdecl**adp=_tmp437;struct Cyc_List_List*ts=_tmp439;
# 2684
if((*adp)->impl == 0)
return 0;{
struct Cyc_List_List*_tmp43A=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp43A;
{struct Cyc_List_List*_tmp43B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp43B;for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type)))
return 1;}}
return 0;}}}else{
# 2696
goto _LLA;}case 18: _LLA:
 return 0;case 19: _tmp438=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->f1;_tmp437=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f2;{union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp438;struct Cyc_List_List*ts=_tmp437;
# 2700
union Cyc_Absyn_DatatypeFieldInfo _tmp43C=tinfo;void*_tmp43E;void*_tmp43D;if((_tmp43C.UnknownDatatypefield).tag == 1)
return 0;else{_tmp43D=((_tmp43C.KnownDatatypefield).val).f1;_tmp43E=((_tmp43C.KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*td=_tmp43D;struct Cyc_Absyn_Datatypefield*fld=_tmp43E;
# 2703
struct Cyc_List_List*_tmp43F=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp43F;
{struct Cyc_List_List*_tmp440=fld->typs;struct Cyc_List_List*typs=_tmp440;for(0;typs != 0;typs=typs->tl){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 2709
return 0;}};}default: goto _LLD;}case 7: _tmp437=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp436)->f2;{struct Cyc_List_List*x=_tmp437;
# 2692
for(1;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD:
# 2711
 return 0;};}}
# 2718
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp441=e->r;void*_stmttmp6E=_tmp441;void*_tmp442=_stmttmp6E;void*_tmp444;void*_tmp443;switch(*((int*)_tmp442)){case 1: if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp442)->f1)== 1)
return 0;else{goto _LL13;}case 22: _tmp443=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp442)->f1;{struct Cyc_Absyn_Exp*e1=_tmp443;
_tmp443=e1;goto _LL6;}case 20: _tmp443=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp442)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp443;
# 2723
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21: _tmp443=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp442)->f1;_tmp444=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp442)->f2;{struct Cyc_Absyn_Exp*e1=_tmp443;struct _fat_ptr*f=_tmp444;
return Cyc_Tcutil_is_noalias_path(e1);}case 23: _tmp443=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp442)->f1;_tmp444=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp442)->f2;{struct Cyc_Absyn_Exp*e1=_tmp443;struct Cyc_Absyn_Exp*e2=_tmp444;
# 2726
void*_tmp445=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp6F=_tmp445;void*_tmp446=_stmttmp6F;if(*((int*)_tmp446)== 6)
return Cyc_Tcutil_is_noalias_path(e1);else{
return 0;};}case 6: _tmp443=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp442)->f2;_tmp444=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp442)->f3;{struct Cyc_Absyn_Exp*e1=_tmp443;struct Cyc_Absyn_Exp*e2=_tmp444;
# 2731
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9: _tmp443=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp442)->f2;{struct Cyc_Absyn_Exp*e2=_tmp443;
_tmp443=e2;goto _LL10;}case 14: _tmp443=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp442)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp443;
return Cyc_Tcutil_is_noalias_path(e2);}case 37: _tmp443=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp442)->f1;{struct Cyc_Absyn_Stmt*s=_tmp443;
# 2735
while(1){
void*_tmp447=s->r;void*_stmttmp70=_tmp447;void*_tmp448=_stmttmp70;void*_tmp44A;void*_tmp449;switch(*((int*)_tmp448)){case 2: _tmp449=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp448)->f1;_tmp44A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp448)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp449;struct Cyc_Absyn_Stmt*s2=_tmp44A;
s=s2;goto _LL1A;}case 12: _tmp449=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp448)->f1;_tmp44A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp448)->f2;{struct Cyc_Absyn_Decl*d=_tmp449;struct Cyc_Absyn_Stmt*s1=_tmp44A;
s=s1;goto _LL1A;}case 1: _tmp449=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp448)->f1;{struct Cyc_Absyn_Exp*e=_tmp449;
return Cyc_Tcutil_is_noalias_path(e);}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52C;_tmp52C.tag=0,({struct _fat_ptr _tmp675=({const char*_tmp44D="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp44D,sizeof(char),40U);});_tmp52C.f1=_tmp675;});_tmp52C;});void*_tmp44B[1];_tmp44B[0]=& _tmp44C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp44B,sizeof(void*),1));});}_LL1A:;}}default: _LL13:
# 2743
 return 1;};}
# 2760 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 2762
struct _tuple14 bogus_ans=({struct _tuple14 _tmp53A;_tmp53A.f1=0,_tmp53A.f2=Cyc_Absyn_heap_rgn_type;_tmp53A;});
void*_tmp44E=e->r;void*_stmttmp71=_tmp44E;void*_tmp44F=_stmttmp71;int _tmp452;void*_tmp451;void*_tmp450;switch(*((int*)_tmp44F)){case 1: _tmp450=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp44F)->f1;{void*x=_tmp450;
# 2766
void*_tmp453=x;void*_tmp454;switch(*((int*)_tmp453)){case 0:
 goto _LL11;case 2: _LL11:
 return bogus_ans;case 1: _tmp454=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp453)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp454;
# 2770
void*_tmp455=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp72=_tmp455;void*_tmp456=_stmttmp72;if(*((int*)_tmp456)== 4)
# 2772
return({struct _tuple14 _tmp52D;_tmp52D.f1=1,_tmp52D.f2=Cyc_Absyn_heap_rgn_type;_tmp52D;});else{
return({struct _tuple14 _tmp52E;_tmp52E.f1=(vd->tq).real_const,_tmp52E.f2=Cyc_Absyn_heap_rgn_type;_tmp52E;});};}case 4: _tmp454=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp453)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp454;
# 2776
void*_tmp457=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp73=_tmp457;void*_tmp458=_stmttmp73;if(*((int*)_tmp458)== 4)
return({struct _tuple14 _tmp52F;_tmp52F.f1=1,_tmp52F.f2=(void*)_check_null(vd->rgn);_tmp52F;});else{
# 2779
vd->escapes=1;
return({struct _tuple14 _tmp530;_tmp530.f1=(vd->tq).real_const,_tmp530.f2=(void*)_check_null(vd->rgn);_tmp530;});};}case 5: _tmp454=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp453)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp454;
# 2782
_tmp454=vd;goto _LL19;}default: _tmp454=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp453)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp454;
# 2784
vd->escapes=1;
return({struct _tuple14 _tmp531;_tmp531.f1=(vd->tq).real_const,_tmp531.f2=(void*)_check_null(vd->rgn);_tmp531;});}};}case 21: _tmp450=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp44F)->f1;_tmp451=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp44F)->f2;_tmp452=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp44F)->f3;{struct Cyc_Absyn_Exp*e1=_tmp450;struct _fat_ptr*f=_tmp451;int is_tagged=_tmp452;
# 2789
if(is_tagged)return bogus_ans;{
# 2792
void*_tmp459=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp74=_tmp459;void*_tmp45A=_stmttmp74;void*_tmp45B;switch(*((int*)_tmp45A)){case 7: _tmp45B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp45A)->f2;{struct Cyc_List_List*fs=_tmp45B;
# 2794
struct Cyc_Absyn_Aggrfield*_tmp45C=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp45C;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp45D=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp75=_tmp45D;struct _tuple14 _tmp45E=_stmttmp75;void*_tmp460;int _tmp45F;_tmp45F=_tmp45E.f1;_tmp460=_tmp45E.f2;{int c=_tmp45F;void*t=_tmp460;
return({struct _tuple14 _tmp532;_tmp532.f1=(finfo->tq).real_const || c,_tmp532.f2=t;_tmp532;});}}
# 2799
return bogus_ans;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45A)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45A)->f1)->f1).KnownAggr).tag == 2){_tmp45B=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45A)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp45B;
# 2801
struct Cyc_Absyn_Aggrfield*_tmp461=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp461;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp462=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp76=_tmp462;struct _tuple14 _tmp463=_stmttmp76;void*_tmp465;int _tmp464;_tmp464=_tmp463.f1;_tmp465=_tmp463.f2;{int c=_tmp464;void*t=_tmp465;
return({struct _tuple14 _tmp533;_tmp533.f1=(finfo->tq).real_const || c,_tmp533.f2=t;_tmp533;});}}
# 2806
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29:
 return bogus_ans;};}}case 22: _tmp450=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp44F)->f1;_tmp451=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp44F)->f2;_tmp452=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp44F)->f3;{struct Cyc_Absyn_Exp*e1=_tmp450;struct _fat_ptr*f=_tmp451;int is_tagged=_tmp452;
# 2811
if(is_tagged)return bogus_ans;{
# 2814
void*_tmp466=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp77=_tmp466;void*_tmp467=_stmttmp77;void*_tmp469;void*_tmp468;if(*((int*)_tmp467)== 3){_tmp468=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp467)->f1).elt_type;_tmp469=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp467)->f1).ptr_atts).rgn;{void*t1=_tmp468;void*r=_tmp469;
# 2816
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp46A=Cyc_Absyn_compress(t1);void*_stmttmp78=_tmp46A;void*_tmp46B=_stmttmp78;void*_tmp46C;switch(*((int*)_tmp46B)){case 7: _tmp46C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp46B)->f2;{struct Cyc_List_List*fs=_tmp46C;
# 2819
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46B)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46B)->f1)->f1).KnownAggr).tag == 2){_tmp46C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46B)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp46C;
# 2821
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B:
 return bogus_ans;}_LL36:;}
# 2824
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp534;_tmp534.f1=(finfo->tq).real_const,_tmp534.f2=r;_tmp534;});
return bogus_ans;}}else{
return bogus_ans;};}}case 20: _tmp450=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44F)->f1;{struct Cyc_Absyn_Exp*e1=_tmp450;
# 2831
void*_tmp46D=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp79=_tmp46D;void*_tmp46E=_stmttmp79;void*_tmp470;struct Cyc_Absyn_Tqual _tmp46F;if(*((int*)_tmp46E)== 3){_tmp46F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).elt_tq;_tmp470=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).ptr_atts).rgn;{struct Cyc_Absyn_Tqual tq=_tmp46F;void*r=_tmp470;
# 2833
return({struct _tuple14 _tmp535;_tmp535.f1=tq.real_const,_tmp535.f2=r;_tmp535;});}}else{
return bogus_ans;};}case 23: _tmp450=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp44F)->f1;_tmp451=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp44F)->f2;{struct Cyc_Absyn_Exp*e1=_tmp450;struct Cyc_Absyn_Exp*e2=_tmp451;
# 2839
void*t=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*_tmp471=t;struct Cyc_Absyn_Tqual _tmp472;void*_tmp473;switch(*((int*)_tmp471)){case 6: _tmp473=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp471)->f1;{struct Cyc_List_List*ts=_tmp473;
# 2843
struct _tuple13 _tmp474=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp7A=_tmp474;struct _tuple13 _tmp475=_stmttmp7A;int _tmp477;unsigned _tmp476;_tmp476=_tmp475.f1;_tmp477=_tmp475.f2;{unsigned i=_tmp476;int known=_tmp477;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp478=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp478;
if(finfo != 0)
return({struct _tuple14 _tmp536;_tmp536.f1=((*finfo).f1).real_const,({void*_tmp676=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp536.f2=_tmp676;});_tmp536;});
return bogus_ans;}}}case 3: _tmp472=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp471)->f1).elt_tq;_tmp473=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp471)->f1).ptr_atts).rgn;{struct Cyc_Absyn_Tqual tq=_tmp472;void*r=_tmp473;
# 2851
return({struct _tuple14 _tmp537;_tmp537.f1=tq.real_const,_tmp537.f2=r;_tmp537;});}case 4: _tmp472=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp471)->f1).tq;{struct Cyc_Absyn_Tqual tq=_tmp472;
# 2857
return({struct _tuple14 _tmp538;_tmp538.f1=tq.real_const,({void*_tmp677=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp538.f2=_tmp677;});_tmp538;});}default:
 return bogus_ans;};}default:
# 2861
({struct Cyc_Warn_String_Warn_Warg_struct _tmp47A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp539;_tmp539.tag=0,({struct _fat_ptr _tmp678=({const char*_tmp47B="unary & applied to non-lvalue";_tag_fat(_tmp47B,sizeof(char),30U);});_tmp539.f1=_tmp678;});_tmp539;});void*_tmp479[1];_tmp479[0]=& _tmp47A;({unsigned _tmp679=e->loc;Cyc_Warn_err2(_tmp679,_tag_fat(_tmp479,sizeof(void*),1));});});
return bogus_ans;};}
# 2868
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp47C=({void*_tmp67A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp67A,b);});struct Cyc_Absyn_Exp*eopt=_tmp47C;
if(eopt == 0)return;{
struct _tuple13 _tmp47D=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp7B=_tmp47D;struct _tuple13 _tmp47E=_stmttmp7B;int _tmp480;unsigned _tmp47F;_tmp47F=_tmp47E.f1;_tmp480=_tmp47E.f2;{unsigned j=_tmp47F;int known=_tmp480;
if(known && j <= i)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp482=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53E;_tmp53E.tag=0,({struct _fat_ptr _tmp67B=({const char*_tmp487="a dereference is out of bounds: ";_tag_fat(_tmp487,sizeof(char),33U);});_tmp53E.f1=_tmp67B;});_tmp53E;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp483=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp53D;_tmp53D.tag=12,_tmp53D.f1=(int)j;_tmp53D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp484=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53C;_tmp53C.tag=0,({
struct _fat_ptr _tmp67C=({const char*_tmp486=" <= ";_tag_fat(_tmp486,sizeof(char),5U);});_tmp53C.f1=_tmp67C;});_tmp53C;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp485=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp53B;_tmp53B.tag=12,_tmp53B.f1=(int)i;_tmp53B;});void*_tmp481[4];_tmp481[0]=& _tmp482,_tmp481[1]=& _tmp483,_tmp481[2]=& _tmp484,_tmp481[3]=& _tmp485;({void(*_tmp67E)(unsigned,struct _fat_ptr)=
# 2873
do_warn?Cyc_Warn_warn2: Cyc_Warn_err2;unsigned _tmp67D=loc;_tmp67E(_tmp67D,_tag_fat(_tmp481,sizeof(void*),4));});});
# 2875
return;}}}
# 2878
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 2886
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
int(*_tmp488)(int,struct Cyc_Absyn_Exp*)=Cyc_Tcutil_cnst_exp;int(*rec)(int,struct Cyc_Absyn_Exp*)=_tmp488;
void*_tmp489=e->r;void*_stmttmp7C=_tmp489;void*_tmp48A=_stmttmp7C;void*_tmp48D;void*_tmp48C;void*_tmp48B;switch(*((int*)_tmp48A)){case 0:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 17: _LL6:
 goto _LL8;case 18: _LL8:
 goto _LLA;case 19: _LLA:
 goto _LLC;case 32: _LLC:
 goto _LLE;case 33: _LLE:
 return 1;case 1: _tmp48B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;{void*b=_tmp48B;
# 2899
void*_tmp48E=b;void*_tmp48F;switch(*((int*)_tmp48E)){case 2:
 return 1;case 1: _tmp48F=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp48E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp48F;
# 2902
void*_tmp490=Cyc_Absyn_compress(vd->type);void*_stmttmp7D=_tmp490;void*_tmp491=_stmttmp7D;switch(*((int*)_tmp491)){case 4:
 goto _LL40;case 5: _LL40:
 return 1;default:
 return var_okay;};}case 4: _tmp48F=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp48E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp48F;
# 2908
if((int)vd->sc != 0)
return var_okay;{
void*_tmp492=Cyc_Absyn_compress(vd->type);void*_stmttmp7E=_tmp492;void*_tmp493=_stmttmp7E;switch(*((int*)_tmp493)){case 4:
 goto _LL47;case 5: _LL47:
 return 1;default:
 return var_okay;};}}case 0:
# 2915
 return 0;default:
 return var_okay;};}case 6: _tmp48B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;_tmp48C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp48A)->f2;_tmp48D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp48A)->f3;{struct Cyc_Absyn_Exp*e1=_tmp48B;struct Cyc_Absyn_Exp*e2=_tmp48C;struct Cyc_Absyn_Exp*e3=_tmp48D;
# 2919
return(rec(0,e1)&& rec(0,e2))&& rec(0,e3);}case 9: _tmp48B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;_tmp48C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp48A)->f2;{struct Cyc_Absyn_Exp*e1=_tmp48B;struct Cyc_Absyn_Exp*e2=_tmp48C;
return rec(0,e1)&& rec(0,e2);}case 41: _tmp48B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;{struct Cyc_Absyn_Exp*e2=_tmp48B;
_tmp48B=e2;goto _LL18;}case 12: _tmp48B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp48B;
_tmp48B=e2;goto _LL1A;}case 13: _tmp48B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp48B;
_tmp48B=e2;goto _LL1C;}case 14: _tmp48B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp48A)->f2;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp48B;
# 2926
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 15: _tmp48B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;{struct Cyc_Absyn_Exp*e2=_tmp48B;
return rec(1,e2);}case 27: _tmp48B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp48A)->f2;_tmp48C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp48A)->f3;{struct Cyc_Absyn_Exp*e1=_tmp48B;struct Cyc_Absyn_Exp*e2=_tmp48C;
return rec(0,e1)&& rec(0,e2);}case 28: _tmp48B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;{struct Cyc_Absyn_Exp*e=_tmp48B;
return rec(0,e);}case 26: _tmp48B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;{struct Cyc_List_List*des=_tmp48B;
_tmp48B=des;goto _LL26;}case 30: _tmp48B=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp48A)->f2;_LL26: {struct Cyc_List_List*des=_tmp48B;
_tmp48B=des;goto _LL28;}case 29: _tmp48B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp48A)->f3;_LL28: {struct Cyc_List_List*des=_tmp48B;
# 2933
for(1;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3: _tmp48B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp48A)->f2;{struct Cyc_List_List*es=_tmp48B;
_tmp48B=es;goto _LL2C;}case 24: _tmp48B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;_LL2C: {struct Cyc_List_List*es=_tmp48B;
_tmp48B=es;goto _LL2E;}case 31: _tmp48B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp48A)->f1;_LL2E: {struct Cyc_List_List*es=_tmp48B;
# 2940
for(1;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default:
 return 0;};}
# 2947
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 2951
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp494=Cyc_Absyn_compress(t);void*_stmttmp7F=_tmp494;void*_tmp495=_stmttmp7F;void*_tmp497;void*_tmp496;switch(*((int*)_tmp495)){case 0: _tmp496=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp495)->f1;_tmp497=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp495)->f2;{void*c=_tmp496;struct Cyc_List_List*ts=_tmp497;
# 2955
void*_tmp498=c;union Cyc_Absyn_AggrInfo _tmp499;void*_tmp49A;switch(*((int*)_tmp498)){case 0:
 goto _LL11;case 1: _LL11:
 goto _LL13;case 2: _LL13:
 return 1;case 15: _tmp49A=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp498)->f2;{struct Cyc_Absyn_Enumdecl*edo=_tmp49A;
# 2960
if(edo == 0 || edo->fields == 0)
return 0;
_tmp49A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16: _tmp49A=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp498)->f1;_LL17: {struct Cyc_List_List*fs=_tmp49A;
# 2964
{struct Cyc_List_List*_tmp49B=fs;struct Cyc_List_List*fs2=_tmp49B;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp49C=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp80=_tmp49C;struct _tuple13 _tmp49D=_stmttmp80;int _tmp49F;unsigned _tmp49E;_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;{unsigned i=_tmp49E;int known=_tmp49F;
if(known && i == 0U)
return 1;}}}}
# 2971
return 0;}case 20: _tmp499=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp498)->f1;{union Cyc_Absyn_AggrInfo info=_tmp499;
# 2974
struct Cyc_Absyn_Aggrdecl*_tmp4A0=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp4A0;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == 1 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default:
 return 0;};}case 3: _tmp496=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp495)->f1).ptr_atts).nullable;{void*n=_tmp496;
# 2982
return Cyc_Tcutil_force_type2bool(1,n);}case 4: _tmp496=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp495)->f1).elt_type;{void*t=_tmp496;
return Cyc_Tcutil_zeroable_type(t);}case 6: _tmp496=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp495)->f1;{struct Cyc_List_List*tqs=_tmp496;
# 2985
for(1;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7: _tmp496=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp495)->f2;{struct Cyc_List_List*fs=_tmp496;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default:
 return 0;};}
# 2993
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 2995
struct _RegionHandle _tmp4A1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4A1;_push_region(rgn);
{struct Cyc_List_List*_tmp4A2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp4A2;
for(1;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp4A3=0;_npop_handler(0);return _tmp4A3;}}{
# 3003
int _tmp4A4=1;_npop_handler(0);return _tmp4A4;}}
# 2996
;_pop_region();}
# 3007
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp4A5=t;void*_tmp4A6;if(*((int*)_tmp4A5)== 8){_tmp4A6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4A5)->f3;{struct Cyc_Absyn_Typedefdecl*tdopt=_tmp4A6;
# 3010
if(tdopt == 0)
return;{
struct Cyc_Absyn_Tqual _tmp4A7=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp4A7;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp541;_tmp541.tag=0,({struct _fat_ptr _tmp67F=({const char*_tmp4AD="qualifier within typedef type ";_tag_fat(_tmp4AD,sizeof(char),31U);});_tmp541.f1=_tmp67F;});_tmp541;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4AA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp540;_tmp540.tag=2,_tmp540.f1=(void*)t;_tmp540;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53F;_tmp53F.tag=0,({struct _fat_ptr _tmp680=({const char*_tmp4AC=" is ignored";_tag_fat(_tmp4AC,sizeof(char),12U);});_tmp53F.f1=_tmp680;});_tmp53F;});void*_tmp4A8[3];_tmp4A8[0]=& _tmp4A9,_tmp4A8[1]=& _tmp4AA,_tmp4A8[2]=& _tmp4AB;({unsigned _tmp681=loc;Cyc_Warn_warn2(_tmp681,_tag_fat(_tmp4A8,sizeof(void*),3));});});
return;}}}else{
return;};}
# 3021
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp4AE=Cyc_Absyn_compress(t);void*_stmttmp81=_tmp4AE;void*_tmp4AF=_stmttmp81;void*_tmp4B0;struct Cyc_Absyn_PtrInfo _tmp4B1;switch(*((int*)_tmp4AF)){case 3: _tmp4B1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AF)->f1;{struct Cyc_Absyn_PtrInfo pi=_tmp4B1;
return({void*_tmp682=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp682,(pi.ptr_atts).bounds);});}case 4: _tmp4B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AF)->f1).num_elts;{struct Cyc_Absyn_Exp*e=_tmp4B0;
return e;}default:
 return 0;};}
# 3031
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp4B2=b;void*_tmp4B3;switch(*((int*)_tmp4B2)){case 5: _tmp4B3=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp4B2)->f1;{struct Cyc_Absyn_Vardecl*x=_tmp4B3;
_tmp4B3=x;goto _LL4;}case 4: _tmp4B3=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp4B2)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp4B3;
_tmp4B3=x;goto _LL6;}case 3: _tmp4B3=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp4B2)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp4B3;
_tmp4B3=x;goto _LL8;}case 1: _tmp4B3=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp4B2)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp4B3;
return x->escapes?0: x;}default:
 return 0;};}
# 3042
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp4B4=0;struct Cyc_List_List*res=_tmp4B4;
for(1;l != 0;l=l->tl){
if((void**)l->hd != 0)res=({struct Cyc_List_List*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->hd=*((void**)_check_null((void**)l->hd)),_tmp4B5->tl=res;_tmp4B5;});}
return res;}
# 3049
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp4B6=Cyc_Absyn_compress(t);void*_stmttmp82=_tmp4B6;void*_tmp4B7=_stmttmp82;unsigned _tmp4BC;void*_tmp4BB;void*_tmp4BA;struct Cyc_Absyn_Tqual _tmp4B9;void*_tmp4B8;if(*((int*)_tmp4B7)== 4){_tmp4B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B7)->f1).elt_type;_tmp4B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B7)->f1).tq;_tmp4BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B7)->f1).num_elts;_tmp4BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B7)->f1).zero_term;_tmp4BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B7)->f1).zt_loc;{void*et=_tmp4B8;struct Cyc_Absyn_Tqual tq=_tmp4B9;struct Cyc_Absyn_Exp*eopt=_tmp4BA;void*zt=_tmp4BB;unsigned ztl=_tmp4BC;
# 3052
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
if(convert_tag){
if(eopt->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp542;_tmp542.tag=0,({struct _fat_ptr _tmp683=({const char*_tmp4BF="cannot convert tag without type!";_tag_fat(_tmp4BF,sizeof(char),33U);});_tmp542.f1=_tmp683;});_tmp542;});void*_tmp4BD[1];_tmp4BD[0]=& _tmp4BE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4BD,sizeof(void*),1));});{
void*_tmp4C0=Cyc_Absyn_compress((void*)_check_null(eopt->topt));void*_stmttmp83=_tmp4C0;void*_tmp4C1=_stmttmp83;void*_tmp4C2;if(*((int*)_tmp4C1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f2 != 0){_tmp4C2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f2)->hd;{void*t=_tmp4C2;
# 3061
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3066
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt,Cyc_Absyn_false_type);}}else{
return t;};}

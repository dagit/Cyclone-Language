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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
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
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 379
extern struct Cyc_List_List*Cyc_List_tabulate_c(int,void*(*)(void*,int),void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 94
extern int Cyc_Set_cardinality(struct Cyc_Set_Set*);
# 97
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 284
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 297
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 304
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 663 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 844
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 857
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 864
void*Cyc_Absyn_compress(void*);
# 877
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 879
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 882
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 884
extern void*Cyc_Absyn_sint_type;
# 887
void*Cyc_Absyn_gen_float_type(unsigned);
# 889
extern void*Cyc_Absyn_unique_rgn_type;
# 891
extern void*Cyc_Absyn_empty_effect;
# 893
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 895
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_enum_type(struct _tuple2*,struct Cyc_Absyn_Enumdecl*);
# 924
void*Cyc_Absyn_bounds_one (void);
# 926
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 949
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 967
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1037
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1121
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 74
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 41 "tcutil.h"
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
# 52
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 107
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 114
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple13{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 151
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 158
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr);
# 166
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 177
int Cyc_Tcutil_is_noalias_region(void*,int);
# 185
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 228
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
# 40
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 47
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
# 50
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_trko;
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 142
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int,int);
# 144
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 32
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct _tuple15{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;void*_tmp4;int _tmp5;void*_tmp3;void*_tmp2;switch(*((int*)_tmp1)){case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;{struct Cyc_List_List*exist_ts=_tmp2;struct Cyc_List_List*dps=_tmp3;int dots=_tmp5;
# 52
if(topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp358;_tmp358.tag=0,({struct _fat_ptr _tmp3EC=({const char*_tmp8="cannot determine pattern type";_tag_fat(_tmp8,sizeof(char),30U);});_tmp358.f1=_tmp3EC;});_tmp358;});void*_tmp6[1];_tmp6[0]=& _tmp7;({unsigned _tmp3ED=p->loc;Cyc_Warn_err2(_tmp3ED,_tag_fat(_tmp6,sizeof(void*),1));});});else{
# 55
void*_tmp9=Cyc_Absyn_compress(*topt);void*_stmttmp1=_tmp9;void*_tmpA=_stmttmp1;union Cyc_Absyn_AggrInfo _tmpB;if(*((int*)_tmpA)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpA)->f1)== 20){_tmpB=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpA)->f1)->f1;{union Cyc_Absyn_AggrInfo ainfo=_tmpB;
# 57
({void*_tmp3EF=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->tag=7,({union Cyc_Absyn_AggrInfo*_tmp3EE=({union Cyc_Absyn_AggrInfo*_tmpC=_cycalloc(sizeof(*_tmpC));*_tmpC=ainfo;_tmpC;});_tmpD->f1=_tmp3EE;}),_tmpD->f2=exist_ts,_tmpD->f3=dps,_tmpD->f4=dots;_tmpD;});p->r=_tmp3EF;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}}else{goto _LL12;}}else{_LL12:
# 61
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp35A;_tmp35A.tag=0,({struct _fat_ptr _tmp3F0=({const char*_tmp11="pattern expects aggregate type instead of ";_tag_fat(_tmp11,sizeof(char),43U);});_tmp35A.f1=_tmp3F0;});_tmp35A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp10=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp359;_tmp359.tag=2,_tmp359.f1=(void*)*topt;_tmp359;});void*_tmpE[2];_tmpE[0]=& _tmpF,_tmpE[1]=& _tmp10;({unsigned _tmp3F1=p->loc;Cyc_Warn_err2(_tmp3F1,_tag_fat(_tmpE,sizeof(void*),2));});});
goto _LLF;}_LLF:;}
# 64
return;}}else{if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).tag == 2){_tmp2=((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).val;_tmp3=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;{struct Cyc_Absyn_Aggrdecl**adp=_tmp2;struct Cyc_List_List**exist_ts=(struct Cyc_List_List**)_tmp3;struct Cyc_List_List*dps=_tmp4;int dots=_tmp5;
# 67
struct Cyc_Absyn_Aggrdecl*_tmp12=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp12;
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp35C;_tmp35C.tag=0,({struct _fat_ptr _tmp3F2=({const char*_tmp16="can't destructure an abstract";_tag_fat(_tmp16,sizeof(char),30U);});_tmp35C.f1=_tmp3F2;});_tmp35C;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp15=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp35B;_tmp35B.tag=6,_tmp35B.f1=ad;_tmp35B;});void*_tmp13[2];_tmp13[0]=& _tmp14,_tmp13[1]=& _tmp15;({unsigned _tmp3F3=p->loc;Cyc_Warn_err2(_tmp3F3,_tag_fat(_tmp13,sizeof(void*),2));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 73
int more_exists=({int _tmp3F4=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp3F4 - ({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(*exist_ts);});
if(more_exists < 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _tmp35D;_tmp35D.tag=0,({struct _fat_ptr _tmp3F5=({const char*_tmp19="too many existential type variables in pattern";_tag_fat(_tmp19,sizeof(char),47U);});_tmp35D.f1=_tmp3F5;});_tmp35D;});void*_tmp17[1];_tmp17[0]=& _tmp18;({unsigned _tmp3F6=p->loc;Cyc_Warn_err2(_tmp3F6,_tag_fat(_tmp17,sizeof(void*),1));});});{
struct Cyc_List_List**_tmp1A=exist_ts;struct Cyc_List_List**ts=_tmp1A;
{int n=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);for(0;n != 0;-- n){
ts=&((struct Cyc_List_List*)_check_null(*ts))->tl;}}
*ts=0;}}else{
if(more_exists > 0){
# 82
struct Cyc_List_List*_tmp1B=0;struct Cyc_List_List*new_ts=_tmp1B;
for(1;more_exists != 0;-- more_exists){
new_ts=({struct Cyc_List_List*_tmp1D=_cycalloc(sizeof(*_tmp1D));({struct Cyc_Absyn_Tvar*_tmp3F7=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=1,_tmp1C->f1=0;_tmp1C;}));_tmp1D->hd=_tmp3F7;}),_tmp1D->tl=new_ts;_tmp1D;});}
({struct Cyc_List_List*_tmp3F8=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;})(*exist_ts,new_ts);*exist_ts=_tmp3F8;});}}
# 87
return;}}}else{
# 106
({struct Cyc_Warn_String_Warn_Warg_struct _tmp30=({struct Cyc_Warn_String_Warn_Warg_struct _tmp35E;_tmp35E.tag=0,({struct _fat_ptr _tmp3F9=({const char*_tmp31="resolve_pat unknownAggr";_tag_fat(_tmp31,sizeof(char),24U);});_tmp35E.f1=_tmp3F9;});_tmp35E;});void*_tmp2F[1];_tmp2F[0]=& _tmp30;({int(*_tmp3FA)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp3FA(_tag_fat(_tmp2F,sizeof(void*),1));});});}}case 17: _tmp2=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e=_tmp2;
# 89
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcutil_is_const_exp(e)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp35F;_tmp35F.tag=0,({struct _fat_ptr _tmp3FB=({const char*_tmp20="non-constant expression in case pattern";_tag_fat(_tmp20,sizeof(char),40U);});_tmp35F.f1=_tmp3FB;});_tmp35F;});void*_tmp1E[1];_tmp1E[0]=& _tmp1F;({unsigned _tmp3FC=p->loc;Cyc_Warn_err2(_tmp3FC,_tag_fat(_tmp1E,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 95
struct _tuple15 _tmp21=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple15 _stmttmp2=_tmp21;struct _tuple15 _tmp22=_stmttmp2;int _tmp24;unsigned _tmp23;_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;{unsigned i=_tmp23;int known=_tmp24;
if(!known){
({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp360;_tmp360.tag=0,({struct _fat_ptr _tmp3FD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp360.f1=_tmp3FD;});_tmp360;});void*_tmp25[1];_tmp25[0]=& _tmp27;({unsigned _tmp3FF=p->loc;struct _fat_ptr _tmp3FE=({const char*_tmp26="cannot evaluate expression at compile time\nWe suggest using x && x == %s instead";_tag_fat(_tmp26,sizeof(char),81U);});Cyc_Warn_err(_tmp3FF,_tmp3FE,_tag_fat(_tmp25,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}
# 101
({void*_tmp400=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=10,_tmp28->f1=2U,_tmp28->f2=(int)i;_tmp28;});p->r=_tmp400;});
return;}}}case 15:
# 104
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp361;_tmp361.tag=0,({struct _fat_ptr _tmp401=({const char*_tmp2B="resolve_pat UnknownId_p";_tag_fat(_tmp2B,sizeof(char),24U);});_tmp361.f1=_tmp401;});_tmp361;});void*_tmp29[1];_tmp29[0]=& _tmp2A;({int(*_tmp402)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp402(_tag_fat(_tmp29,sizeof(void*),1));});});case 16:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp362;_tmp362.tag=0,({struct _fat_ptr _tmp403=({const char*_tmp2E="resolve_pat UnknownCall_p";_tag_fat(_tmp2E,sizeof(char),26U);});_tmp362.f1=_tmp403;});_tmp362;});void*_tmp2C[1];_tmp2C[0]=& _tmp2D;({int(*_tmp404)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp404(_tag_fat(_tmp2C,sizeof(void*),1));});});default:
# 108
 return;};}
# 112
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){
return({struct Cyc_Absyn_Pat*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->loc=loc,_tmp32->topt=0,_tmp32->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp32;});}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 115
static struct _tuple16*Cyc_Tcpat_wild_dlp(unsigned loc){
return({struct _tuple16*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=0,({struct Cyc_Absyn_Pat*_tmp405=Cyc_Tcpat_wild_pat(loc);_tmp33->f2=_tmp405;});_tmp33;});}
# 119
static struct Cyc_List_List*Cyc_Tcpat_remove_dots(struct Cyc_List_List*earlier,int num_pats,int num_fields,void*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str){
# 121
if(num_pats >= num_fields)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp35=({struct Cyc_Warn_String_Warn_Warg_struct _tmp365;_tmp365.tag=0,({struct _fat_ptr _tmp406=({const char*_tmp39="unnecessary ... in ";_tag_fat(_tmp39,sizeof(char),20U);});_tmp365.f1=_tmp406;});_tmp365;});struct Cyc_Warn_String_Warn_Warg_struct _tmp36=({struct Cyc_Warn_String_Warn_Warg_struct _tmp364;_tmp364.tag=0,_tmp364.f1=msg_str;_tmp364;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37=({struct Cyc_Warn_String_Warn_Warg_struct _tmp363;_tmp363.tag=0,({struct _fat_ptr _tmp407=({const char*_tmp38=" pattern";_tag_fat(_tmp38,sizeof(char),9U);});_tmp363.f1=_tmp407;});_tmp363;});void*_tmp34[3];_tmp34[0]=& _tmp35,_tmp34[1]=& _tmp36,_tmp34[2]=& _tmp37;({unsigned _tmp408=loc;Cyc_Warn_warn2(_tmp408,_tag_fat(_tmp34,sizeof(void*),3));});});{
struct Cyc_List_List*new_ps=0;
{int i=0;for(0;i < num_fields - num_pats;++ i){
new_ps=({struct Cyc_List_List*_tmp3A=_cycalloc(sizeof(*_tmp3A));({void*_tmp409=f(loc);_tmp3A->hd=_tmp409;}),_tmp3A->tl=new_ps;_tmp3A;});}}
return Cyc_List_imp_append(earlier,new_ps);}}
# 128
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 131
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
return topt == 0?Cyc_Absyn_new_evar(& Cyc_Kinds_mko,({struct Cyc_Core_Opt*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->v=s;_tmp3B;})):*topt;}
# 134
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 138
if(topt == 0)
return numt;
if(Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;{
void*_tmp3C=Cyc_Absyn_compress(numt);void*_stmttmp3=_tmp3C;void*_tmp3D=_stmttmp3;if(*((int*)_tmp3D)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f1)){case 15:
 goto _LL4;case 16: _LL4:
 return*topt;default: goto _LL5;}else{_LL5:
 return numt;};}}struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 149
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 151
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp40A=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_tmp40A;});}
# 155
({struct Cyc_List_List*_tmp40C=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));({struct _tuple17*_tmp40B=({struct _tuple17*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->f1=vd,_tmp3E->f2=e;_tmp3E;});_tmp3F->hd=_tmp40B;}),_tmp3F->tl=*v_result_ptr;_tmp3F;});*v_result_ptr=_tmp40C;});}
# 157
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 159
struct Cyc_Tcpat_TcPatResult _tmp40=res1;void*_tmp42;void*_tmp41;_tmp41=_tmp40.tvars_and_bounds_opt;_tmp42=_tmp40.patvars;{struct _tuple1*p1=_tmp41;struct Cyc_List_List*vs1=_tmp42;
struct Cyc_Tcpat_TcPatResult _tmp43=res2;void*_tmp45;void*_tmp44;_tmp44=_tmp43.tvars_and_bounds_opt;_tmp45=_tmp43.patvars;{struct _tuple1*p2=_tmp44;struct Cyc_List_List*vs2=_tmp45;
if(p1 == 0 && p2 == 0)
return({struct Cyc_Tcpat_TcPatResult _tmp366;_tmp366.tvars_and_bounds_opt=0,({struct Cyc_List_List*_tmp40D=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(vs1,vs2);_tmp366.patvars=_tmp40D;});_tmp366;});
if(p1 == 0)
p1=({struct _tuple1*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->f1=0,_tmp46->f2=0;_tmp46;});
if(p2 == 0)
p2=({struct _tuple1*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->f1=0,_tmp47->f2=0;_tmp47;});
return({struct Cyc_Tcpat_TcPatResult _tmp367;({struct _tuple1*_tmp411=({struct _tuple1*_tmp48=_cycalloc(sizeof(*_tmp48));({struct Cyc_List_List*_tmp410=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*p1).f1,(*p2).f1);_tmp48->f1=_tmp410;}),({
struct Cyc_List_List*_tmp40F=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*p1).f2,(*p2).f2);_tmp48->f2=_tmp40F;});_tmp48;});
# 167
_tmp367.tvars_and_bounds_opt=_tmp411;}),({
# 169
struct Cyc_List_List*_tmp40E=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(vs1,vs2);_tmp367.patvars=_tmp40E;});_tmp367;});}}}
# 173
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp413=t;Cyc_Tcutil_promote_array(_tmp413,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_List_List*_tmp412=Cyc_Tcenv_lookup_type_vars(te);_tmp49->v=_tmp412;});_tmp49;})): rgn_opt,0);}): t;}struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};
# 180
static struct _tuple18*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple18*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->f1=tv,_tmp4A->f2=0;_tmp4A;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 184
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 187
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp390;_tmp390.tvars_and_bounds_opt=0,_tmp390.patvars=0;_tmp390;});
# 192
{void*_tmp4B=p->r;void*_stmttmp4=_tmp4B;void*_tmp4C=_stmttmp4;void*_tmp4F;int _tmp50;void*_tmp4E;void*_tmp4D;switch(*((int*)_tmp4C)){case 0:
# 194
 t=({struct Cyc_List_List*_tmp414=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp414,topt);});goto _LL0;case 1: _tmp4D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;_tmp4E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4C)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp4D;struct Cyc_Absyn_Pat*p2=_tmp4E;
# 198
struct _tuple2*_tmp51=vd->name;struct _tuple2*_stmttmp5=_tmp51;struct _tuple2*_tmp52=_stmttmp5;struct _fat_ptr _tmp53;_tmp53=*_tmp52->f2;{struct _fat_ptr v=_tmp53;
if(({struct _fat_ptr _tmp416=(struct _fat_ptr)v;Cyc_strcmp(_tmp416,({const char*_tmp54="true";_tag_fat(_tmp54,sizeof(char),5U);}));})== 0 ||({struct _fat_ptr _tmp415=(struct _fat_ptr)v;Cyc_strcmp(_tmp415,({const char*_tmp55="false";_tag_fat(_tmp55,sizeof(char),6U);}));})== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp57=({struct Cyc_Warn_String_Warn_Warg_struct _tmp36A;_tmp36A.tag=0,({struct _fat_ptr _tmp417=({const char*_tmp5B="you probably do not want to use ";_tag_fat(_tmp5B,sizeof(char),33U);});_tmp36A.f1=_tmp417;});_tmp36A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp58=({struct Cyc_Warn_String_Warn_Warg_struct _tmp369;_tmp369.tag=0,_tmp369.f1=v;_tmp369;});struct Cyc_Warn_String_Warn_Warg_struct _tmp59=({struct Cyc_Warn_String_Warn_Warg_struct _tmp368;_tmp368.tag=0,({
struct _fat_ptr _tmp418=({const char*_tmp5A=" as a local variable";_tag_fat(_tmp5A,sizeof(char),21U);});_tmp368.f1=_tmp418;});_tmp368;});void*_tmp56[3];_tmp56[0]=& _tmp57,_tmp56[1]=& _tmp58,_tmp56[2]=& _tmp59;({unsigned _tmp419=p->loc;Cyc_Warn_warn2(_tmp419,_tag_fat(_tmp56,sizeof(void*),3));});});
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(p2->topt);
# 206
{void*_tmp5C=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp5C;void*_tmp5D=_stmttmp6;if(*((int*)_tmp5D)== 4){
# 208
if(rgn_pat == 0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp36B;_tmp36B.tag=0,({struct _fat_ptr _tmp41A=({const char*_tmp60="array reference would point into unknown/unallowed region";_tag_fat(_tmp60,sizeof(char),58U);});_tmp36B.f1=_tmp41A;});_tmp36B;});void*_tmp5E[1];_tmp5E[0]=& _tmp5F;({unsigned _tmp41B=p->loc;Cyc_Warn_err2(_tmp41B,_tag_fat(_tmp5E,sizeof(void*),1));});});
goto _LL2E;}else{
# 212
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62=({struct Cyc_Warn_String_Warn_Warg_struct _tmp36C;_tmp36C.tag=0,({struct _fat_ptr _tmp41C=({const char*_tmp63="pattern would point to an abstract member";_tag_fat(_tmp63,sizeof(char),42U);});_tmp36C.f1=_tmp41C;});_tmp36C;});void*_tmp61[1];_tmp61[0]=& _tmp62;({unsigned _tmp41D=p->loc;Cyc_Warn_err2(_tmp41D,_tag_fat(_tmp61,sizeof(void*),1));});});
goto _LL2E;}_LL2E:;}
# 216
({struct Cyc_Absyn_Vardecl**_tmp420=({struct Cyc_Absyn_Vardecl**_tmp64=_cycalloc(sizeof(*_tmp64));*_tmp64=vd;_tmp64;});struct Cyc_Absyn_Exp*_tmp41F=access_exp;struct Cyc_List_List**_tmp41E=& res.patvars;Cyc_Tcpat_set_vd(_tmp420,_tmp41F,_tmp41E,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}}case 2: _tmp4D=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;_tmp4E=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4C)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp4D;struct Cyc_Absyn_Vardecl*vd=_tmp4E;
# 219
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp423=p->loc;struct Cyc_Tcenv_Tenv*_tmp422=te;Cyc_Tcenv_add_type_vars(_tmp423,_tmp422,({struct Cyc_Absyn_Tvar*_tmp71[1];_tmp71[0]=tv;({struct Cyc_List_List*(*_tmp421)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp421(_tag_fat(_tmp71,sizeof(struct Cyc_Absyn_Tvar*),1));});}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp424=({struct _tuple1*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=0,_tmp65->f2=0;_tmp65;});res.tvars_and_bounds_opt=_tmp424;});
({struct Cyc_List_List*_tmp428=({
struct Cyc_List_List*(*_tmp427)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp426=(*res.tvars_and_bounds_opt).f1;_tmp427(_tmp426,({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));({struct _tuple18*_tmp425=({struct _tuple18*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=tv,_tmp66->f2=1;_tmp66;});_tmp67->hd=_tmp425;}),_tmp67->tl=0;_tmp67;}));});
# 222
(*res.tvars_and_bounds_opt).f1=_tmp428;});
# 224
({unsigned _tmp42B=p->loc;struct Cyc_Tcenv_Tenv*_tmp42A=te2;struct Cyc_List_List*_tmp429=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp42B,_tmp42A,_tmp429,& Cyc_Kinds_tmk,1,0,vd->type);});
# 227
t=({struct Cyc_List_List*_tmp42C=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp42C,topt);});
{void*_tmp68=Cyc_Absyn_compress(t);void*_stmttmp7=_tmp68;void*_tmp69=_stmttmp7;if(*((int*)_tmp69)== 4){
# 230
if(rgn_pat == 0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp36D;_tmp36D.tag=0,({struct _fat_ptr _tmp42D=({const char*_tmp6C="array reference would point into unknown/unallowed region";_tag_fat(_tmp6C,sizeof(char),58U);});_tmp36D.f1=_tmp42D;});_tmp36D;});void*_tmp6A[1];_tmp6A[0]=& _tmp6B;({unsigned _tmp42E=p->loc;Cyc_Warn_err2(_tmp42E,_tag_fat(_tmp6A,sizeof(void*),1));});});
goto _LL33;}else{
# 234
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp36E;_tmp36E.tag=0,({struct _fat_ptr _tmp42F=({const char*_tmp6F="pattern would point to an abstract member";_tag_fat(_tmp6F,sizeof(char),42U);});_tmp36E.f1=_tmp42F;});_tmp36E;});void*_tmp6D[1];_tmp6D[0]=& _tmp6E;({unsigned _tmp430=p->loc;Cyc_Warn_err2(_tmp430,_tag_fat(_tmp6D,sizeof(void*),1));});});
goto _LL33;}_LL33:;}
# 238
({struct Cyc_Absyn_Vardecl**_tmp433=({struct Cyc_Absyn_Vardecl**_tmp70=_cycalloc(sizeof(*_tmp70));*_tmp70=vd;_tmp70;});struct Cyc_Absyn_Exp*_tmp432=access_exp;struct Cyc_List_List**_tmp431=& res.patvars;Cyc_Tcpat_set_vd(_tmp433,_tmp432,_tmp431,vd->type);});
goto _LL0;}case 3: _tmp4D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;_tmp4E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4C)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp4D;struct Cyc_Absyn_Pat*p2=_tmp4E;
# 242
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(p2->topt);
if(!allow_ref_pat || rgn_pat == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp73=({struct Cyc_Warn_String_Warn_Warg_struct _tmp36F;_tmp36F.tag=0,({struct _fat_ptr _tmp434=({const char*_tmp74="* pattern would point into an unknown/unallowed region";_tag_fat(_tmp74,sizeof(char),55U);});_tmp36F.f1=_tmp434;});_tmp36F;});void*_tmp72[1];_tmp72[0]=& _tmp73;({unsigned _tmp435=p->loc;Cyc_Warn_err2(_tmp435,_tag_fat(_tmp72,sizeof(void*),1));});});
goto _LL0;}else{
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp76=({struct Cyc_Warn_String_Warn_Warg_struct _tmp370;_tmp370.tag=0,({struct _fat_ptr _tmp436=({const char*_tmp77="* pattern cannot take the address of an alias-free path";_tag_fat(_tmp77,sizeof(char),56U);});_tmp370.f1=_tmp436;});_tmp370;});void*_tmp75[1];_tmp75[0]=& _tmp76;({unsigned _tmp437=p->loc;Cyc_Warn_err2(_tmp437,_tag_fat(_tmp75,sizeof(void*),1));});});}{
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PtrInfo _tmp78=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp78;
pi.elt_type=t;
(pi.ptr_atts).rgn=rgn_pat;
(pi.ptr_atts).nullable=Cyc_Absyn_false_type;{
void*t2=Cyc_Absyn_pointer_type(pi);
if((unsigned)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);
new_access_exp->topt=t2;}
# 259
({struct Cyc_Absyn_Vardecl**_tmp43A=({struct Cyc_Absyn_Vardecl**_tmp79=_cycalloc(sizeof(*_tmp79));*_tmp79=vd;_tmp79;});struct Cyc_Absyn_Exp*_tmp439=new_access_exp;struct Cyc_List_List**_tmp438=& res.patvars;Cyc_Tcpat_set_vd(_tmp43A,_tmp439,_tmp438,t2);});
goto _LL0;}}}case 4: _tmp4D=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;_tmp4E=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4C)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp4D;struct Cyc_Absyn_Vardecl*vd=_tmp4E;
# 265
({struct Cyc_Absyn_Vardecl**_tmp43D=({struct Cyc_Absyn_Vardecl**_tmp7A=_cycalloc(sizeof(*_tmp7A));*_tmp7A=vd;_tmp7A;});struct Cyc_Absyn_Exp*_tmp43C=access_exp;struct Cyc_List_List**_tmp43B=& res.patvars;Cyc_Tcpat_set_vd(_tmp43D,_tmp43C,_tmp43B,vd->type);});
# 269
({unsigned _tmp440=p->loc;struct Cyc_Tcenv_Tenv*_tmp43F=te;Cyc_Tcenv_add_type_vars(_tmp440,_tmp43F,({struct Cyc_Absyn_Tvar*_tmp7B[1];_tmp7B[0]=tv;({struct Cyc_List_List*(*_tmp43E)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp43E(_tag_fat(_tmp7B,sizeof(struct Cyc_Absyn_Tvar*),1));});}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp441=({struct _tuple1*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=0,_tmp7C->f2=0;_tmp7C;});res.tvars_and_bounds_opt=_tmp441;});
({struct Cyc_List_List*_tmp443=({struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));
({struct _tuple18*_tmp442=({struct _tuple18*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=tv,_tmp7D->f2=0;_tmp7D;});_tmp7E->hd=_tmp442;}),_tmp7E->tl=(*res.tvars_and_bounds_opt).f1;_tmp7E;});
# 272
(*res.tvars_and_bounds_opt).f1=_tmp443;});
# 274
t=Cyc_Absyn_uint_type;
goto _LL0;}case 10: if(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp4C)->f1 == Cyc_Absyn_Unsigned){
# 277
t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;}else{
t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;}case 11:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 12: _tmp50=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp4C)->f2;{int i=_tmp50;
t=({void**_tmp444=topt;Cyc_Tcpat_num_type(_tmp444,Cyc_Absyn_gen_float_type((unsigned)i));});goto _LL0;}case 13: _tmp4D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp4D;
t=({void**_tmp445=topt;Cyc_Tcpat_num_type(_tmp445,Cyc_Absyn_enum_type(ed->name,ed));});goto _LL0;}case 14: _tmp4D=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;{void*tenum=_tmp4D;
t=Cyc_Tcpat_num_type(topt,tenum);goto _LL0;}case 9:
# 284
 if(topt != 0){
void*_tmp7F=Cyc_Absyn_compress(*topt);void*_stmttmp8=_tmp7F;void*_tmp80=_stmttmp8;if(*((int*)_tmp80)== 3){
t=*topt;goto tcpat_end;}else{
goto _LL38;}_LL38:;}{
# 289
struct Cyc_Absyn_PtrInfo _tmp81=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp81;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
t=Cyc_Absyn_pointer_type(pi);
goto _LL0;}case 6: _tmp4D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;{struct Cyc_Absyn_Pat*p2=_tmp4D;
# 297
void*inner_typ=Cyc_Absyn_void_type;
void**_tmp82=0;void**inner_topt=_tmp82;
int elt_const=0;
if(topt != 0){
void*_tmp83=Cyc_Absyn_compress(*topt);void*_stmttmp9=_tmp83;void*_tmp84=_stmttmp9;struct Cyc_Absyn_Tqual _tmp86;void*_tmp85;if(*((int*)_tmp84)== 3){_tmp85=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84)->f1).elt_type;_tmp86=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84)->f1).elt_tq;{void*elt_typ=_tmp85;struct Cyc_Absyn_Tqual elt_tq=_tmp86;
# 303
inner_typ=elt_typ;
inner_topt=& inner_typ;
elt_const=elt_tq.real_const;
goto _LL3D;}}else{
goto _LL3D;}_LL3D:;}{
# 312
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Kinds_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp446=res;Cyc_Tcpat_combine_results(_tmp446,Cyc_Tcpat_tcPatRec(te,p2,inner_topt,ptr_rgn,1,new_access_exp));});
# 321
{void*_tmp87=Cyc_Absyn_compress((void*)_check_null(p2->topt));void*_stmttmpA=_tmp87;void*_tmp88=_stmttmpA;void*_tmp8B;void*_tmp8A;void*_tmp89;if(*((int*)_tmp88)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)== 19){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f1).KnownDatatypefield).tag == 2){_tmp89=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f1).KnownDatatypefield).val).f1;_tmp8A=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f1).KnownDatatypefield).val).f2;_tmp8B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp89;struct Cyc_Absyn_Datatypefield*tuf=_tmp8A;struct Cyc_List_List*targs=_tmp8B;
# 325
{void*_tmp8C=Cyc_Absyn_compress(inner_typ);void*_stmttmpB=_tmp8C;void*_tmp8D=_stmttmpB;if(*((int*)_tmp8D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8D)->f1)== 19)
# 327
goto DONT_PROMOTE;else{goto _LL4A;}}else{_LL4A:
 goto _LL47;}_LL47:;}{
# 331
void*new_type=({union Cyc_Absyn_DatatypeInfo _tmp447=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp8F=_cycalloc(sizeof(*_tmp8F));*_tmp8F=tud;_tmp8F;}));Cyc_Absyn_datatype_type(_tmp447,targs);});
# 333
p2->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=3,(_tmp8E->f1).elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _tmp44C=Cyc_Absyn_const_tqual(0U);(_tmp8E->f1).elt_tq=_tmp44C;}):({
struct Cyc_Absyn_Tqual _tmp44B=Cyc_Absyn_empty_tqual(0U);(_tmp8E->f1).elt_tq=_tmp44B;}),
((_tmp8E->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp44A=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmp8E->f1).ptr_atts).nullable=_tmp44A;}),({
void*_tmp449=Cyc_Absyn_bounds_one();((_tmp8E->f1).ptr_atts).bounds=_tmp449;}),((_tmp8E->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp8E->f1).ptr_atts).ptrloc=0,({
void*_tmp448=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmp8E->f1).ptr_atts).autoreleased=_tmp448;});_tmp8E;});
goto _LL42;}}}else{goto _LL45;}}else{goto _LL45;}}else{_LL45:
# 342
 DONT_PROMOTE: {
struct Cyc_Absyn_PtrInfo _tmp90=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp90;
pi.elt_type=(void*)_check_null(p2->topt);
({struct Cyc_Absyn_Tqual _tmp44D=elt_const?Cyc_Absyn_const_tqual(0U): Cyc_Absyn_empty_tqual(0U);pi.elt_tq=_tmp44D;});
(pi.ptr_atts).rgn=ptr_rgn;
t=Cyc_Absyn_pointer_type(pi);}}_LL42:;}
# 349
if((unsigned)new_access_exp)new_access_exp->topt=p2->topt;
goto _LL0;}}case 5: _tmp4D=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;_tmp50=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4C)->f2;{struct Cyc_List_List**ps_p=_tmp4D;int dots=_tmp50;
# 353
struct Cyc_List_List*_tmp91=*ps_p;struct Cyc_List_List*ps=_tmp91;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp92=Cyc_Absyn_compress(*topt);void*_stmttmpC=_tmp92;void*_tmp93=_stmttmpC;void*_tmp94;if(*((int*)_tmp93)== 6){_tmp94=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp93)->f1;{struct Cyc_List_List*tqts=_tmp94;
# 359
topt_ts=tqts;
if(dots){
({struct Cyc_List_List*_tmp453=({struct Cyc_List_List*(*_tmp452)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;});struct Cyc_List_List*_tmp451=ps;int _tmp450=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ps);int _tmp44F=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tqts);unsigned _tmp44E=p->loc;_tmp452(_tmp451,_tmp450,_tmp44F,Cyc_Tcpat_wild_pat,_tmp44E,({const char*_tmp95="tuple";_tag_fat(_tmp95,sizeof(char),6U);}));});*ps_p=_tmp453;});
# 363
ps=*ps_p;}
# 365
goto _LL4C;}}else{
goto _LL4C;}_LL4C:;}else{
# 368
if(dots)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp97=({struct Cyc_Warn_String_Warn_Warg_struct _tmp371;_tmp371.tag=0,({struct _fat_ptr _tmp454=({const char*_tmp98="cannot determine missing fields for ... in tuple pattern";_tag_fat(_tmp98,sizeof(char),57U);});_tmp371.f1=_tmp454;});_tmp371;});void*_tmp96[1];_tmp96[0]=& _tmp97;({unsigned _tmp455=p->loc;Cyc_Warn_err2(_tmp455,_tag_fat(_tmp96,sizeof(void*),1));});});}
{int i=0;for(0;ps != 0;(ps=ps->tl,i ++)){
void**_tmp99=0;void**inner_topt=_tmp99;
if(topt_ts != 0){
inner_topt=&(*((struct _tuple19*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 376
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=({struct Cyc_Absyn_Exp*_tmp456=access_exp;Cyc_Absyn_subscript_exp(_tmp456,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp372;(_tmp372.Int_c).tag=5U,((_tmp372.Int_c).val).f1=1U,((_tmp372.Int_c).val).f2=i;_tmp372;}),0U),0U);});
# 381
res=({struct Cyc_Tcpat_TcPatResult _tmp457=res;Cyc_Tcpat_combine_results(_tmp457,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)ps->hd,inner_topt,rgn_pat,allow_ref_pat,new_access_exp));});
# 384
if((unsigned)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)ps->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct _tuple19*_tmp459=({struct _tuple19*_tmp9A=_cycalloc(sizeof(*_tmp9A));({struct Cyc_Absyn_Tqual _tmp458=Cyc_Absyn_empty_tqual(0U);_tmp9A->f1=_tmp458;}),_tmp9A->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)ps->hd)->topt);_tmp9A;});_tmp9B->hd=_tmp459;}),_tmp9B->tl=pat_ts;_tmp9B;});}}}
# 387
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=6,({struct Cyc_List_List*_tmp45A=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(pat_ts);_tmp9C->f1=_tmp45A;});_tmp9C;});
goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C)->f1)->KnownAggr).tag == 2){_tmp4D=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C)->f1)->KnownAggr).val;_tmp4E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C)->f2;_tmp4F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C)->f3;_tmp50=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C)->f4;{struct Cyc_Absyn_Aggrdecl*ad=_tmp4D;struct Cyc_List_List*exist_ts=_tmp4E;struct Cyc_List_List**dps_p=_tmp4F;int dots=_tmp50;
# 391
struct Cyc_List_List*_tmp9D=*dps_p;struct Cyc_List_List*dps=_tmp9D;
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp374;_tmp374.tag=0,({struct _fat_ptr _tmp45B=({const char*_tmpA1="can't destructure abstract ";_tag_fat(_tmpA1,sizeof(char),28U);});_tmp374.f1=_tmp45B;});_tmp374;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpA0=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp373;_tmp373.tag=6,_tmp373.f1=ad;_tmp373;});void*_tmp9E[2];_tmp9E[0]=& _tmp9F,_tmp9E[1]=& _tmpA0;({unsigned _tmp45C=p->loc;Cyc_Warn_err2(_tmp45C,_tag_fat(_tmp9E,sizeof(void*),2));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL0;}
# 399
if((int)ad->kind == 1 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
allow_ref_pat=0;
if(exist_ts != 0){
# 405
if(topt == 0 || Cyc_Tcutil_type_kind(*topt)!= & Cyc_Kinds_ak)
allow_ref_pat=0;}
# 408
{struct _RegionHandle _tmpA2=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA2;_push_region(rgn);
# 410
{struct Cyc_List_List*_tmpA3=0;struct Cyc_List_List*var_tvs=_tmpA3;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpA4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct Cyc_List_List*u=_tmpA4;
{struct Cyc_List_List*t=exist_ts;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(u))->hd;
u=u->tl;{
void*_tmpA5=Cyc_Kinds_compress_kb(tv->kind);void*kb1=_tmpA5;
void*_tmpA6=Cyc_Kinds_compress_kb(uv->kind);void*kb2=_tmpA6;
struct Cyc_Absyn_Kind*k2;
{void*_tmpA7=kb2;void*_tmpA8;switch(*((int*)_tmpA7)){case 2: _tmpA8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;{struct Cyc_Absyn_Kind*k=_tmpA8;
_tmpA8=k;goto _LL55;}case 0: _tmpA8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL55: {struct Cyc_Absyn_Kind*k=_tmpA8;
k2=k;goto _LL51;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp375;_tmp375.tag=0,({struct _fat_ptr _tmp45D=({const char*_tmpAB="unconstrained existential tyvar in aggregate";_tag_fat(_tmpAB,sizeof(char),45U);});_tmp375.f1=_tmp45D;});_tmp375;});void*_tmpA9[1];_tmpA9[0]=& _tmpAA;({int(*_tmp45E)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp45E(_tag_fat(_tmpA9,sizeof(void*),1));});});}_LL51:;}
# 425
{void*_tmpAC=kb1;void*_tmpAE;void*_tmpAD;switch(*((int*)_tmpAC)){case 0: _tmpAD=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAC)->f1;{struct Cyc_Absyn_Kind*k1=_tmpAD;
# 428
if(!Cyc_Kinds_kind_leq(k2,k1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp37B;_tmp37B.tag=0,({struct _fat_ptr _tmp45F=({const char*_tmpB8="type variable ";_tag_fat(_tmpB8,sizeof(char),15U);});_tmp37B.f1=_tmp45F;});_tmp37B;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp37A;_tmp37A.tag=7,_tmp37A.f1=tv;_tmp37A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp379;_tmp379.tag=0,({struct _fat_ptr _tmp460=({const char*_tmpB7=" has kind ";_tag_fat(_tmpB7,sizeof(char),11U);});_tmp379.f1=_tmp460;});_tmp379;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp378;_tmp378.tag=8,_tmp378.f1=(void*)kb1;_tmp378;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp377;_tmp377.tag=0,({
struct _fat_ptr _tmp461=({const char*_tmpB6=" but must have at least kind ";_tag_fat(_tmpB6,sizeof(char),30U);});_tmp377.f1=_tmp461;});_tmp377;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp376;_tmp376.tag=9,_tmp376.f1=k2;_tmp376;});void*_tmpAF[6];_tmpAF[0]=& _tmpB0,_tmpAF[1]=& _tmpB1,_tmpAF[2]=& _tmpB2,_tmpAF[3]=& _tmpB3,_tmpAF[4]=& _tmpB4,_tmpAF[5]=& _tmpB5;({unsigned _tmp462=p->loc;Cyc_Warn_err2(_tmp462,_tag_fat(_tmpAF,sizeof(void*),6));});});
goto _LL58;}case 2: _tmpAD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAC)->f1;_tmpAE=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAC)->f2;{struct Cyc_Core_Opt**f=_tmpAD;struct Cyc_Absyn_Kind*k=_tmpAE;
_tmpAD=f;goto _LL5E;}default: _tmpAD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAC)->f1;_LL5E: {struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_tmpAD;
({struct Cyc_Core_Opt*_tmp463=({struct Cyc_Core_Opt*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->v=kb2;_tmpB9;});*f=_tmp463;});goto _LL58;}}_LL58:;}{
# 435
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->tag=2,_tmpC0->f1=tv;_tmpC0;});
var_tvs=({struct Cyc_List_List*_tmpBA=_region_malloc(rgn,sizeof(*_tmpBA));_tmpBA->hd=vartype,_tmpBA->tl=var_tvs;_tmpBA;});
# 439
if((int)k2->kind == 3){
if((int)k2->aliasqual != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp37C;_tmp37C.tag=0,({struct _fat_ptr _tmp464=({const char*_tmpBD="opened existential had unique or top region kind";_tag_fat(_tmpBD,sizeof(char),49U);});_tmp37C.f1=_tmp464;});_tmp37C;});void*_tmpBB[1];_tmpBB[0]=& _tmpBC;({int(*_tmp465)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp465(_tag_fat(_tmpBB,sizeof(void*),1));});});
outlives_constraints=({struct Cyc_List_List*_tmpBF=_cycalloc(sizeof(*_tmpBF));
({struct _tuple0*_tmp466=({struct _tuple0*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->f1=Cyc_Absyn_empty_effect,_tmpBE->f2=vartype;_tmpBE;});_tmpBF->hd=_tmp466;}),_tmpBF->tl=outlives_constraints;_tmpBF;});}}}}}
# 447
var_tvs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(var_tvs);{
# 449
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,exist_ts);
# 451
struct _tuple13 _tmpC1=({struct _tuple13 _tmp385;({struct Cyc_List_List*_tmp467=Cyc_Tcenv_lookup_type_vars(te2);_tmp385.f1=_tmp467;}),_tmp385.f2=rgn;_tmp385;});struct _tuple13 env=_tmpC1;
struct Cyc_List_List*_tmpC2=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c;})(rgn,Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmpC2;
struct Cyc_List_List*_tmpC3=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip;})(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,var_tvs);struct Cyc_List_List*exist_inst=_tmpC3;
struct Cyc_List_List*_tmpC4=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),all_inst);struct Cyc_List_List*all_typs=_tmpC4;
struct Cyc_List_List*_tmpC5=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),exist_inst);struct Cyc_List_List*exist_typs=_tmpC5;
struct Cyc_List_List*_tmpC6=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rappend;})(rgn,all_inst,exist_inst);struct Cyc_List_List*inst=_tmpC6;
# 458
if(exist_ts != 0 ||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp468=({struct _tuple1*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=0,_tmpC7->f2=0;_tmpC7;});res.tvars_and_bounds_opt=_tmp468;});
({struct Cyc_List_List*_tmp46B=({
struct Cyc_List_List*(*_tmp46A)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp469=(*res.tvars_and_bounds_opt).f1;_tmp46A(_tmp469,({(struct Cyc_List_List*(*)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_add_false,exist_ts));});
# 461
(*res.tvars_and_bounds_opt).f1=_tmp46B;});
# 463
({struct Cyc_List_List*_tmp46C=
({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 463
(*res.tvars_and_bounds_opt).f2=_tmp46C;});{
# 465
struct Cyc_List_List*_tmpC8=0;struct Cyc_List_List*rpo_inst=_tmpC8;
{struct Cyc_List_List*_tmpC9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmpC9;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));({struct _tuple0*_tmp46F=({struct _tuple0*_tmpCA=_cycalloc(sizeof(*_tmpCA));({void*_tmp46E=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f1);_tmpCA->f1=_tmp46E;}),({
void*_tmp46D=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f2);_tmpCA->f2=_tmp46D;});_tmpCA;});
# 467
_tmpCB->hd=_tmp46F;}),_tmpCB->tl=rpo_inst;_tmpCB;});}}
# 470
rpo_inst=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(rpo_inst);
({struct Cyc_List_List*_tmp470=
({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*res.tvars_and_bounds_opt).f2,rpo_inst);
# 471
(*res.tvars_and_bounds_opt).f2=_tmp470;});}}
# 476
t=({union Cyc_Absyn_AggrInfo _tmp471=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpCC=_cycalloc(sizeof(*_tmpCC));*_tmpCC=ad;_tmpCC;}));Cyc_Absyn_aggr_type(_tmp471,all_typs);});
if(dots &&(int)ad->kind == 1)
({void*_tmpCD=0U;({unsigned _tmp473=p->loc;struct _fat_ptr _tmp472=({const char*_tmpCE="`...' pattern not allowed in union pattern";_tag_fat(_tmpCE,sizeof(char),43U);});Cyc_Warn_warn(_tmp473,_tmp472,_tag_fat(_tmpCD,sizeof(void*),0));});});else{
if(dots){
({struct Cyc_List_List*_tmp479=({struct Cyc_List_List*(*_tmp478)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;});struct Cyc_List_List*_tmp477=dps;int _tmp476=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(dps);int _tmp475=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);unsigned _tmp474=p->loc;_tmp478(_tmp477,_tmp476,_tmp475,Cyc_Tcpat_wild_dlp,_tmp474,({const char*_tmpCF="struct";_tag_fat(_tmpCF,sizeof(char),7U);}));});*dps_p=_tmp479;});
# 482
dps=*dps_p;}}{
# 484
struct Cyc_List_List*fields=
({(struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators;})(rgn,p->loc,dps,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(1;fields != 0;fields=fields->tl){
struct _tuple20*_tmpD0=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmpD=_tmpD0;struct _tuple20*_tmpD1=_stmttmpD;void*_tmpD3;void*_tmpD2;_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;{struct Cyc_Absyn_Aggrfield*field=_tmpD2;struct Cyc_Absyn_Pat*pat=_tmpD3;
void*_tmpD4=Cyc_Tcutil_rsubstitute(rgn,inst,field->type);void*inst_fieldtyp=_tmpD4;
# 490
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,field->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp47A=res;Cyc_Tcpat_combine_results(_tmp47A,Cyc_Tcpat_tcPatRec(te2,pat,& inst_fieldtyp,rgn_pat,allow_ref_pat,new_access_exp));});
# 498
if(!Cyc_Unify_unify((void*)_check_null(pat->topt),inst_fieldtyp))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp384;_tmp384.tag=0,({struct _fat_ptr _tmp47B=({const char*_tmpE1="field ";_tag_fat(_tmpE1,sizeof(char),7U);});_tmp384.f1=_tmp47B;});_tmp384;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp383;_tmp383.tag=0,_tmp383.f1=*field->name;_tmp383;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp382;_tmp382.tag=0,({struct _fat_ptr _tmp47C=({const char*_tmpE0=" of ";_tag_fat(_tmpE0,sizeof(char),5U);});_tmp382.f1=_tmp47C;});_tmp382;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpD9=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp381;_tmp381.tag=6,_tmp381.f1=ad;_tmp381;});struct Cyc_Warn_String_Warn_Warg_struct _tmpDA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp380;_tmp380.tag=0,({
struct _fat_ptr _tmp47D=({const char*_tmpDF=" pattern expects type ";_tag_fat(_tmpDF,sizeof(char),23U);});_tmp380.f1=_tmp47D;});_tmp380;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpDB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp37F;_tmp37F.tag=2,_tmp37F.f1=(void*)inst_fieldtyp;_tmp37F;});struct Cyc_Warn_String_Warn_Warg_struct _tmpDC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp37E;_tmp37E.tag=0,({struct _fat_ptr _tmp47E=({const char*_tmpDE=" != ";_tag_fat(_tmpDE,sizeof(char),5U);});_tmp37E.f1=_tmp47E;});_tmp37E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpDD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp37D;_tmp37D.tag=2,_tmp37D.f1=(void*)_check_null(pat->topt);_tmp37D;});void*_tmpD5[8];_tmpD5[0]=& _tmpD6,_tmpD5[1]=& _tmpD7,_tmpD5[2]=& _tmpD8,_tmpD5[3]=& _tmpD9,_tmpD5[4]=& _tmpDA,_tmpD5[5]=& _tmpDB,_tmpD5[6]=& _tmpDC,_tmpD5[7]=& _tmpDD;({unsigned _tmp47F=p->loc;Cyc_Warn_err2(_tmp47F,_tag_fat(_tmpD5,sizeof(void*),8));});});
# 502
if((unsigned)new_access_exp)new_access_exp->topt=pat->topt;}}}}}
# 410
;_pop_region();}
# 505
goto _LL0;}}else{_LL24:
# 545
 goto _LL26;}}else{
# 544
goto _LL24;}case 8: _tmp4D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4C)->f1;_tmp4E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4C)->f2;_tmp4F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4C)->f3;_tmp50=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4C)->f4;{struct Cyc_Absyn_Datatypedecl*tud=_tmp4D;struct Cyc_Absyn_Datatypefield*tuf=_tmp4E;struct Cyc_List_List**ps_p=(struct Cyc_List_List**)_tmp4F;int dots=_tmp50;
# 508
struct Cyc_List_List*_tmpE2=*ps_p;struct Cyc_List_List*ps=_tmpE2;
struct Cyc_List_List*tqts=tuf->typs;
# 511
struct _tuple13 _tmpE3=({struct _tuple13 _tmp38F;({struct Cyc_List_List*_tmp480=Cyc_Tcenv_lookup_type_vars(te);_tmp38F.f1=_tmp480;}),_tmp38F.f2=Cyc_Core_heap_region;_tmp38F;});struct _tuple13 env=_tmpE3;
struct Cyc_List_List*_tmpE4=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmpE4;
struct Cyc_List_List*_tmpE5=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),inst);struct Cyc_List_List*all_typs=_tmpE5;
t=({union Cyc_Absyn_DatatypeFieldInfo _tmp481=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp481,all_typs);});
if(dots){
({struct Cyc_List_List*_tmp487=({struct Cyc_List_List*(*_tmp486)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;});struct Cyc_List_List*_tmp485=ps;int _tmp484=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ps);int _tmp483=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tqts);unsigned _tmp482=p->loc;_tmp486(_tmp485,_tmp484,_tmp483,Cyc_Tcpat_wild_pat,_tmp482,({const char*_tmpE6="datatype field";_tag_fat(_tmpE6,sizeof(char),15U);}));});*ps_p=_tmp487;});
# 518
ps=*ps_p;}
# 520
for(1;ps != 0 && tqts != 0;(ps=ps->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpE7=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmpE7;
# 524
void*_tmpE8=Cyc_Tcutil_substitute(inst,(*((struct _tuple19*)tqts->hd)).f2);void*field_typ=_tmpE8;
# 527
if((unsigned)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
res=({struct Cyc_Tcpat_TcPatResult _tmp488=res;Cyc_Tcpat_combine_results(_tmp488,Cyc_Tcpat_tcPatRec(te,p2,& field_typ,rgn_pat,allow_ref_pat,0));});
# 534
if(!Cyc_Unify_unify((void*)_check_null(p2->topt),field_typ))
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpEA=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp38A;_tmp38A.tag=1,_tmp38A.f1=tuf->name;_tmp38A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp389;_tmp389.tag=0,({struct _fat_ptr _tmp489=({const char*_tmpF0=" expects argument type ";_tag_fat(_tmpF0,sizeof(char),24U);});_tmp389.f1=_tmp489;});_tmp389;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpEC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp388;_tmp388.tag=2,_tmp388.f1=(void*)field_typ;_tmp388;});struct Cyc_Warn_String_Warn_Warg_struct _tmpED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp387;_tmp387.tag=0,({
struct _fat_ptr _tmp48A=({const char*_tmpEF=" not ";_tag_fat(_tmpEF,sizeof(char),6U);});_tmp387.f1=_tmp48A;});_tmp387;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpEE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp386;_tmp386.tag=2,_tmp386.f1=(void*)_check_null(p2->topt);_tmp386;});void*_tmpE9[5];_tmpE9[0]=& _tmpEA,_tmpE9[1]=& _tmpEB,_tmpE9[2]=& _tmpEC,_tmpE9[3]=& _tmpED,_tmpE9[4]=& _tmpEE;({unsigned _tmp48B=p2->loc;Cyc_Warn_err2(_tmp48B,_tag_fat(_tmpE9,sizeof(void*),5));});});}
# 538
if(ps != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp38C;_tmp38C.tag=0,({struct _fat_ptr _tmp48C=({const char*_tmpF4="too many arguments for datatype constructor ";_tag_fat(_tmpF4,sizeof(char),45U);});_tmp38C.f1=_tmp48C;});_tmp38C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp38B;_tmp38B.tag=1,_tmp38B.f1=tuf->name;_tmp38B;});void*_tmpF1[2];_tmpF1[0]=& _tmpF2,_tmpF1[1]=& _tmpF3;({unsigned _tmp48D=p->loc;Cyc_Warn_err2(_tmp48D,_tag_fat(_tmpF1,sizeof(void*),2));});});
if(tqts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp38E;_tmp38E.tag=0,({struct _fat_ptr _tmp48E=({const char*_tmpF8="too few arguments for datatype constructor ";_tag_fat(_tmpF8,sizeof(char),44U);});_tmp38E.f1=_tmp48E;});_tmp38E;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp38D;_tmp38D.tag=1,_tmp38D.f1=tuf->name;_tmp38D;});void*_tmpF5[2];_tmpF5[0]=& _tmpF6,_tmpF5[1]=& _tmpF7;({unsigned _tmp48F=p->loc;Cyc_Warn_err2(_tmp48F,_tag_fat(_tmpF5,sizeof(void*),2));});});
goto _LL0;}case 15: _LL26:
# 546
 goto _LL28;case 17: _LL28:
 goto _LL2A;default: _LL2A:
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL0;}_LL0:;}
# 550
tcpat_end:
 p->topt=t;
return res;}}
# 555
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 557
struct Cyc_Tcpat_TcPatResult _tmpF9=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);struct Cyc_Tcpat_TcPatResult ans=_tmpF9;
# 559
struct _tuple1 _tmpFA=({(struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split;})(ans.patvars);struct _tuple1 _stmttmpE=_tmpFA;struct _tuple1 _tmpFB=_stmttmpE;void*_tmpFC;_tmpFC=_tmpFB.f1;{struct Cyc_List_List*vs1=_tmpFC;
struct Cyc_List_List*_tmpFD=0;struct Cyc_List_List*vs=_tmpFD;
{struct Cyc_List_List*x=vs1;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)vs=({struct Cyc_List_List*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),_tmpFE->tl=vs;_tmpFE;});}}
({struct Cyc_List_List*_tmp491=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_get_name,vs);unsigned _tmp490=p->loc;Cyc_Tcutil_check_unique_vars(_tmp491,_tmp490,({const char*_tmpFF="pattern contains a repeated variable";_tag_fat(_tmpFF,sizeof(char),37U);}));});
# 568
{struct Cyc_List_List*x=ans.patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp100=(struct _tuple17*)x->hd;struct _tuple17*_stmttmpF=_tmp100;struct _tuple17*_tmp101=_stmttmpF;void*_tmp103;void*_tmp102;_tmp102=_tmp101->f1;_tmp103=(struct Cyc_Absyn_Exp**)& _tmp101->f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmp102;struct Cyc_Absyn_Exp**expopt=(struct Cyc_Absyn_Exp**)_tmp103;
if(*expopt != 0 &&*expopt != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp492=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*expopt));*expopt=_tmp492;});}}}
# 573
return ans;}}
# 579
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 582
struct _tuple0 _tmp104=({struct _tuple0 _tmp391;({void*_tmp494=Cyc_Absyn_compress(old_type);_tmp391.f1=_tmp494;}),({void*_tmp493=Cyc_Absyn_compress(new_type);_tmp391.f2=_tmp493;});_tmp391;});struct _tuple0 _stmttmp10=_tmp104;struct _tuple0 _tmp105=_stmttmp10;struct Cyc_Absyn_PtrInfo _tmp107;struct Cyc_Absyn_PtrInfo _tmp106;if(*((int*)_tmp105.f1)== 3){if(*((int*)_tmp105.f2)== 3){_tmp106=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105.f1)->f1;_tmp107=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105.f2)->f1;{struct Cyc_Absyn_PtrInfo pold=_tmp106;struct Cyc_Absyn_PtrInfo pnew=_tmp107;
# 584
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp108=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->tag=3,(_tmp109->f1).elt_type=pold.elt_type,(_tmp109->f1).elt_tq=pnew.elt_tq,
((_tmp109->f1).ptr_atts).rgn=(pold.ptr_atts).rgn,((_tmp109->f1).ptr_atts).nullable=(pnew.ptr_atts).nullable,((_tmp109->f1).ptr_atts).bounds=(pnew.ptr_atts).bounds,((_tmp109->f1).ptr_atts).zero_term=(pnew.ptr_atts).zero_term,((_tmp109->f1).ptr_atts).ptrloc=(pold.ptr_atts).ptrloc,((_tmp109->f1).ptr_atts).autoreleased=(pnew.ptr_atts).autoreleased;_tmp109;});
# 584
struct Cyc_Absyn_PointerType_Absyn_Type_struct*ptry=_tmp108;
# 591
return({struct Cyc_RgnOrder_RgnPO*_tmp499=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp498=assump;void*_tmp497=(void*)ptry;Cyc_Tcutil_subtype(_tmp499,_tmp498,_tmp497,new_type);})&&({
struct Cyc_RgnOrder_RgnPO*_tmp496=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_Absyn_Exp*_tmp495=initializer;Cyc_Tcutil_coerce_assign(_tmp496,_tmp495,(void*)ptry);});}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 600
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 603
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));({struct _tuple0*_tmp49B=({struct _tuple0*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->f1=Cyc_Absyn_unique_rgn_type,({void*_tmp49A=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->tag=2,_tmp118->f1=tv;_tmp118;});_tmp119->f2=_tmp49A;});_tmp119;});_tmp11A->hd=_tmp49B;}),_tmp11A->tl=0;_tmp11A;});
if(({struct Cyc_RgnOrder_RgnPO*_tmp49E=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp49D=assump;void*_tmp49C=old_type;Cyc_Tcutil_subtype(_tmp49E,_tmp49D,_tmp49C,new_type);})){
# 621 "tcpat.cyc"
struct _tuple0 _tmp10A=({struct _tuple0 _tmp393;({void*_tmp4A0=Cyc_Absyn_compress(old_type);_tmp393.f1=_tmp4A0;}),({void*_tmp49F=Cyc_Absyn_compress(new_type);_tmp393.f2=_tmp49F;});_tmp393;});struct _tuple0 _stmttmp11=_tmp10A;struct _tuple0 _tmp10B=_stmttmp11;struct Cyc_Absyn_PtrInfo _tmp10D;struct Cyc_Absyn_PtrInfo _tmp10C;if(*((int*)_tmp10B.f1)== 3){if(*((int*)_tmp10B.f2)== 3){_tmp10C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10B.f1)->f1;_tmp10D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10B.f2)->f1;{struct Cyc_Absyn_PtrInfo pold=_tmp10C;struct Cyc_Absyn_PtrInfo pnew=_tmp10D;
goto _LL0;}}else{goto _LL3;}}else{_LL3:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp392;_tmp392.tag=0,({struct _fat_ptr _tmp4A1=({const char*_tmp110="alias requires pointer type";_tag_fat(_tmp110,sizeof(char),28U);});_tmp392.f1=_tmp4A1;});_tmp392;});void*_tmp10E[1];_tmp10E[0]=& _tmp10F;({unsigned _tmp4A2=loc;Cyc_Warn_err2(_tmp4A2,_tag_fat(_tmp10E,sizeof(void*),1));});});goto _LL0;}_LL0:;}else{
# 626
({struct Cyc_Warn_String_Warn_Warg_struct _tmp112=({struct Cyc_Warn_String_Warn_Warg_struct _tmp397;_tmp397.tag=0,({struct _fat_ptr _tmp4A3=({const char*_tmp117="cannot alias value of type ";_tag_fat(_tmp117,sizeof(char),28U);});_tmp397.f1=_tmp4A3;});_tmp397;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp113=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp396;_tmp396.tag=2,_tmp396.f1=(void*)old_type;_tmp396;});struct Cyc_Warn_String_Warn_Warg_struct _tmp114=({struct Cyc_Warn_String_Warn_Warg_struct _tmp395;_tmp395.tag=0,({struct _fat_ptr _tmp4A4=({const char*_tmp116=" to type ";_tag_fat(_tmp116,sizeof(char),10U);});_tmp395.f1=_tmp4A4;});_tmp395;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp115=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp394;_tmp394.tag=2,_tmp394.f1=(void*)new_type;_tmp394;});void*_tmp111[4];_tmp111[0]=& _tmp112,_tmp111[1]=& _tmp113,_tmp111[2]=& _tmp114,_tmp111[3]=& _tmp115;({unsigned _tmp4A5=loc;Cyc_Warn_err2(_tmp4A5,_tag_fat(_tmp111,sizeof(void*),4));});});}}
# 632
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 634
void*_tmp11B=p->r;void*_stmttmp12=_tmp11B;void*_tmp11C=_stmttmp12;void*_tmp11F;void*_tmp11E;void*_tmp11D;switch(*((int*)_tmp11C)){case 6: _tmp11D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp11C)->f1;{struct Cyc_Absyn_Pat*p2=_tmp11D;
# 636
void*_tmp120=(void*)_check_null(p->topt);void*_stmttmp13=_tmp120;void*_tmp121=_stmttmp13;void*_tmp122;if(*((int*)_tmp121)== 3){_tmp122=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp121)->f1).ptr_atts).rgn;{void*rt=_tmp122;
# 638
Cyc_Tcenv_check_rgn_accessible(te,p->loc,rt);
({struct Cyc_Tcenv_Tenv*_tmp4A8=te;struct Cyc_Absyn_Pat*_tmp4A7=p2;int _tmp4A6=Cyc_Tcutil_is_noalias_region(rt,0);Cyc_Tcpat_check_pat_regions_rec(_tmp4A8,_tmp4A7,_tmp4A6,patvars);});
return;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp124=({struct Cyc_Warn_String_Warn_Warg_struct _tmp398;_tmp398.tag=0,({struct _fat_ptr _tmp4A9=({const char*_tmp125="check_pat_regions: bad pointer type";_tag_fat(_tmp125,sizeof(char),36U);});_tmp398.f1=_tmp4A9;});_tmp398;});void*_tmp123[1];_tmp123[0]=& _tmp124;({int(*_tmp4AA)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4AA(_tag_fat(_tmp123,sizeof(void*),1));});});};}case 7: _tmp11D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp11C)->f1;_tmp11E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp11C)->f2;_tmp11F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp11C)->f3;{union Cyc_Absyn_AggrInfo*ai=_tmp11D;struct Cyc_List_List*exist_ts=_tmp11E;struct Cyc_List_List*dps=_tmp11F;
# 644
for(1;dps != 0;dps=dps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple16*)dps->hd)).f2,did_noalias_deref,patvars);}
return;}case 8: _tmp11D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp11C)->f3;{struct Cyc_List_List*ps=_tmp11D;
# 648
did_noalias_deref=0;_tmp11D=ps;goto _LL8;}case 5: _tmp11D=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp11C)->f1;_LL8: {struct Cyc_List_List*ps=_tmp11D;
# 650
for(1;ps != 0;ps=ps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)ps->hd,did_noalias_deref,patvars);}
return;}case 3: _tmp11D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp11C)->f1;_tmp11E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp11C)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp11D;struct Cyc_Absyn_Pat*p2=_tmp11E;
# 654
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp126=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp14=_tmp126;struct _tuple17*_tmp127=_stmttmp14;void*_tmp129;void*_tmp128;_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmp128;struct Cyc_Absyn_Exp*eopt=_tmp129;
# 660
if((vdopt != 0 &&*vdopt == vd)&& eopt != 0){
{void*_tmp12A=eopt->r;void*_stmttmp15=_tmp12A;void*_tmp12B=_stmttmp15;void*_tmp12C;if(*((int*)_tmp12B)== 15){_tmp12C=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;{struct Cyc_Absyn_Exp*e=_tmp12C;
# 663
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp399;_tmp399.tag=0,({struct _fat_ptr _tmp4AB=({const char*_tmp12F="reference pattern not allowed on alias-free pointers";_tag_fat(_tmp12F,sizeof(char),53U);});_tmp399.f1=_tmp4AB;});_tmp399;});void*_tmp12D[1];_tmp12D[0]=& _tmp12E;({unsigned _tmp4AC=p->loc;Cyc_Warn_err2(_tmp4AC,_tag_fat(_tmp12D,sizeof(void*),1));});});
goto _LL19;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp131=({struct Cyc_Warn_String_Warn_Warg_struct _tmp39A;_tmp39A.tag=0,({struct _fat_ptr _tmp4AD=({const char*_tmp132="check_pat_regions: bad reference access exp";_tag_fat(_tmp132,sizeof(char),44U);});_tmp39A.f1=_tmp4AD;});_tmp39A;});void*_tmp130[1];_tmp130[0]=& _tmp131;({int(*_tmp4AE)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4AE(_tag_fat(_tmp130,sizeof(void*),1));});});}_LL19:;}
# 668
break;}}}}
# 671
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 1: _tmp11D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp11C)->f1;_tmp11E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp11C)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp11D;struct Cyc_Absyn_Pat*p2=_tmp11E;
# 674
{void*_tmp133=p->topt;void*_stmttmp16=_tmp133;void*_tmp134=_stmttmp16;if(_tmp134 != 0){if(*((int*)_tmp134)== 4){
# 676
if(!did_noalias_deref)
goto _LL1E;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp136=({struct Cyc_Warn_String_Warn_Warg_struct _tmp39B;_tmp39B.tag=0,({struct _fat_ptr _tmp4AF=({const char*_tmp137="pattern to array would create alias of no-alias pointer";_tag_fat(_tmp137,sizeof(char),56U);});_tmp39B.f1=_tmp4AF;});_tmp39B;});void*_tmp135[1];_tmp135[0]=& _tmp136;({unsigned _tmp4B0=p->loc;Cyc_Warn_err2(_tmp4B0,_tag_fat(_tmp135,sizeof(void*),1));});});
return;}else{goto _LL21;}}else{_LL21:
 goto _LL1E;}_LL1E:;}
# 682
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 2: _tmp11D=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp11C)->f1;_tmp11E=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp11C)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp11D;struct Cyc_Absyn_Vardecl*vd=_tmp11E;
# 685
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp138=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp17=_tmp138;struct _tuple17*_tmp139=_stmttmp17;void*_tmp13B;void*_tmp13A;_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmp13A;struct Cyc_Absyn_Exp*eopt=_tmp13B;
# 689
if(vdopt != 0 &&*vdopt == vd){
if(eopt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp39C;_tmp39C.tag=0,({struct _fat_ptr _tmp4B1=({const char*_tmp13E="cannot alias pattern expression in datatype";_tag_fat(_tmp13E,sizeof(char),44U);});_tmp39C.f1=_tmp4B1;});_tmp39C;});void*_tmp13C[1];_tmp13C[0]=& _tmp13D;({unsigned _tmp4B2=p->loc;Cyc_Warn_err2(_tmp4B2,_tag_fat(_tmp13C,sizeof(void*),1));});});else{
# 693
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp4B5=p->loc;struct Cyc_Tcenv_Tenv*_tmp4B4=te;Cyc_Tcenv_add_type_vars(_tmp4B5,_tmp4B4,({struct Cyc_Absyn_Tvar*_tmp140[1];_tmp140[0]=tv;({struct Cyc_List_List*(*_tmp4B3)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp4B3(_tag_fat(_tmp140,sizeof(struct Cyc_Absyn_Tvar*),1));});}));});
te2=({struct Cyc_Tcenv_Tenv*_tmp4B6=te2;Cyc_Tcenv_add_region(_tmp4B6,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->tag=2,_tmp13F->f1=tv;_tmp13F;}),0,0);});
# 696
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(eopt->topt),tv,vd->type,eopt);}
# 698
break;}}}}
# 701
goto _LL0;}default:
 return;}_LL0:;}
# 717 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 719
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp141=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp18=_tmp141;struct _tuple17*_tmp142=_stmttmp18;void*_tmp144;void*_tmp143;_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmp143;struct Cyc_Absyn_Exp*eopt=_tmp144;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp145=eopt;struct Cyc_Absyn_Exp*e=_tmp145;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))&& !Cyc_Tcutil_is_noalias_path(e)){
# 726
if(vdopt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp147=({struct Cyc_Warn_String_Warn_Warg_struct _tmp39E;_tmp39E.tag=0,({struct _fat_ptr _tmp4B7=({const char*_tmp14A="pattern dereferences an alias-free-pointer ";_tag_fat(_tmp14A,sizeof(char),44U);});_tmp39E.f1=_tmp4B7;});_tmp39E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp148=({struct Cyc_Warn_String_Warn_Warg_struct _tmp39D;_tmp39D.tag=0,({
struct _fat_ptr _tmp4B8=({const char*_tmp149="from a non-unique path";_tag_fat(_tmp149,sizeof(char),23U);});_tmp39D.f1=_tmp4B8;});_tmp39D;});void*_tmp146[2];_tmp146[0]=& _tmp147,_tmp146[1]=& _tmp148;({unsigned _tmp4B9=p->loc;Cyc_Warn_err2(_tmp4B9,_tag_fat(_tmp146,sizeof(void*),2));});});else{
# 730
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A1;_tmp3A1.tag=0,({struct _fat_ptr _tmp4BA=({const char*_tmp150="pattern for variable ";_tag_fat(_tmp150,sizeof(char),22U);});_tmp3A1.f1=_tmp4BA;});_tmp3A1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp14D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3A0;_tmp3A0.tag=1,_tmp3A0.f1=(*vdopt)->name;_tmp3A0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp14E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp39F;_tmp39F.tag=0,({
struct _fat_ptr _tmp4BB=({const char*_tmp14F=" dereferences an alias-free-pointer from a non-unique path";_tag_fat(_tmp14F,sizeof(char),59U);});_tmp39F.f1=_tmp4BB;});_tmp39F;});void*_tmp14B[3];_tmp14B[0]=& _tmp14C,_tmp14B[1]=& _tmp14D,_tmp14B[2]=& _tmp14E;({unsigned _tmp4BC=p->loc;Cyc_Warn_err2(_tmp4BC,_tag_fat(_tmp14B,sizeof(void*),3));});});}}}}}}}
# 777 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 787
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 801
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){return({union Cyc_Tcpat_Name_value _tmp3A2;(_tmp3A2.Name_v).tag=1U,(_tmp3A2.Name_v).val=s;_tmp3A2;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp3A3;(_tmp3A3.Int_v).tag=2U,(_tmp3A3.Int_v).val=i;_tmp3A3;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 818
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp151=c1->name;union Cyc_Tcpat_Name_value _stmttmp19=_tmp151;union Cyc_Tcpat_Name_value _tmp152=_stmttmp19;int _tmp153;struct _fat_ptr _tmp154;if((_tmp152.Name_v).tag == 1){_tmp154=(_tmp152.Name_v).val;{struct _fat_ptr n1=_tmp154;
# 821
union Cyc_Tcpat_Name_value _tmp155=c2->name;union Cyc_Tcpat_Name_value _stmttmp1A=_tmp155;union Cyc_Tcpat_Name_value _tmp156=_stmttmp1A;struct _fat_ptr _tmp157;if((_tmp156.Name_v).tag == 1){_tmp157=(_tmp156.Name_v).val;{struct _fat_ptr n2=_tmp157;
return Cyc_strcmp((struct _fat_ptr)n1,(struct _fat_ptr)n2);}}else{
return -1;};}}else{_tmp153=(_tmp152.Int_v).val;{int i1=_tmp153;
# 826
union Cyc_Tcpat_Name_value _tmp158=c2->name;union Cyc_Tcpat_Name_value _stmttmp1B=_tmp158;union Cyc_Tcpat_Name_value _tmp159=_stmttmp1B;int _tmp15A;if((_tmp159.Name_v).tag == 1)
return 1;else{_tmp15A=(_tmp159.Int_v).val;{int i2=_tmp15A;
return i1 - i2;}};}};}
# 834
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set (void){
return({(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty;})(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 838
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 842
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned ans=0U;
struct Cyc_List_List*_tmp15B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp15B;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 849
return ans;}
# 852
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp15C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp15C;for(0;fields != 0;(fields=fields->tl,++ i)){
if(Cyc_strcmp((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,(struct _fat_ptr)*f)== 0)return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A6;_tmp3A6.tag=0,({struct _fat_ptr _tmp4BD=({const char*_tmp162="get_member_offset ";_tag_fat(_tmp162,sizeof(char),19U);});_tmp3A6.f1=_tmp4BD;});_tmp3A6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp15F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A5;_tmp3A5.tag=0,_tmp3A5.f1=*f;_tmp3A5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp160=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A4;_tmp3A4.tag=0,({struct _fat_ptr _tmp4BE=({const char*_tmp161=" failed";_tag_fat(_tmp161,sizeof(char),8U);});_tmp3A4.f1=_tmp4BE;});_tmp3A4;});void*_tmp15D[3];_tmp15D[0]=& _tmp15E,_tmp15D[1]=& _tmp15F,_tmp15D[2]=& _tmp160;({int(*_tmp4BF)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4BF(_tag_fat(_tmp15D,sizeof(void*),3));});});}
# 859
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp163=pw;void*_tmp164;if((_tmp163.where_clause).tag == 2){_tmp164=(_tmp163.where_clause).val;{struct Cyc_Absyn_Exp*e=_tmp164;
return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=0,_tmp165->f1=e;_tmp165;});}}else{_tmp164=(_tmp163.pattern).val;{struct Cyc_Absyn_Pat*p=_tmp164;
# 863
void*_tmp166=p->r;void*_stmttmp1C=_tmp166;void*_tmp167=_stmttmp1C;union Cyc_Absyn_AggrInfo _tmp168;void*_tmp16A;struct _fat_ptr _tmp16B;char _tmp16D;int _tmp16C;enum Cyc_Absyn_Sign _tmp16E;void*_tmp169;switch(*((int*)_tmp167)){case 1: _tmp169=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp167)->f2;{struct Cyc_Absyn_Pat*p1=_tmp169;
_tmp169=p1;goto _LL9;}case 3: _tmp169=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp167)->f2;_LL9: {struct Cyc_Absyn_Pat*p1=_tmp169;
return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp3A7;(_tmp3A7.pattern).tag=1U,(_tmp3A7.pattern).val=p1;_tmp3A7;}));}case 9:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _tmp16E=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp167)->f1;_tmp16C=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp167)->f2;{enum Cyc_Absyn_Sign s=_tmp16E;int i=_tmp16C;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=6,_tmp16F->f1=(unsigned)i;_tmp16F;});}case 11: _tmp16D=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp167)->f1;{char c=_tmp16D;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->tag=6,_tmp170->f1=(unsigned)c;_tmp170;});}case 12: _tmp16B=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp167)->f1;_tmp16C=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp167)->f2;{struct _fat_ptr f=_tmp16B;int i=_tmp16C;
return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->tag=5,_tmp171->f1=f,_tmp171->f2=i;_tmp171;});}case 13: _tmp169=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp167)->f1;_tmp16A=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp167)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp169;struct Cyc_Absyn_Enumfield*ef=_tmp16A;
return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->tag=3,_tmp172->f1=ed,_tmp172->f2=ef;_tmp172;});}case 14: _tmp169=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp167)->f1;_tmp16A=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp167)->f2;{void*t=_tmp169;struct Cyc_Absyn_Enumfield*ef=_tmp16A;
return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->tag=4,_tmp173->f1=t,_tmp173->f2=ef;_tmp173;});}case 6:
# 873
 if(!Cyc_Tcutil_is_pointer_type((void*)_check_null(p->topt))|| !
Cyc_Tcutil_is_nullable_pointer_type((void*)_check_null(p->topt),0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp175=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A8;_tmp3A8.tag=0,({struct _fat_ptr _tmp4C0=({const char*_tmp176="non-null pointer type or non-pointer type in pointer pattern";_tag_fat(_tmp176,sizeof(char),61U);});_tmp3A8.f1=_tmp4C0;});_tmp3A8;});void*_tmp174[1];_tmp174[0]=& _tmp175;({int(*_tmp4C1)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4C1(_tag_fat(_tmp174,sizeof(void*),1));});});
return(void*)& Cyc_Tcpat_NeqNull_val;case 8: _tmp169=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp167)->f1;_tmp16A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp167)->f2;{struct Cyc_Absyn_Datatypedecl*ddecl=_tmp169;struct Cyc_Absyn_Datatypefield*df=_tmp16A;
# 878
if(ddecl->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->tag=9,_tmp177->f1=ddecl,_tmp177->f2=df;_tmp177;});
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->tag=7,({int _tmp4C2=(int)Cyc_Tcpat_datatype_tag_number(ddecl,df->name);_tmp178->f1=_tmp4C2;}),_tmp178->f2=ddecl,_tmp178->f3=df;_tmp178;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp167)->f1 != 0){_tmp168=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp167)->f1;_tmp169=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp167)->f3;{union Cyc_Absyn_AggrInfo info=_tmp168;struct Cyc_List_List*dlps=_tmp169;
# 882
struct Cyc_Absyn_Aggrdecl*_tmp179=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp179;
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A9;_tmp3A9.tag=0,({struct _fat_ptr _tmp4C3=({const char*_tmp17C="non-tagged aggregate in pattern test";_tag_fat(_tmp17C,sizeof(char),37U);});_tmp3A9.f1=_tmp4C3;});_tmp3A9;});void*_tmp17A[1];_tmp17A[0]=& _tmp17B;({int(*_tmp4C4)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4C4(_tag_fat(_tmp17A,sizeof(void*),1));});});{
void*_tmp17D=(void*)((struct Cyc_List_List*)_check_null((*((struct _tuple16*)((struct Cyc_List_List*)_check_null(dlps))->hd)).f1))->hd;void*_stmttmp1D=_tmp17D;void*_tmp17E=_stmttmp1D;void*_tmp17F;if(*((int*)_tmp17E)== 1){_tmp17F=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp17E)->f1;{struct _fat_ptr*f=_tmp17F;
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->tag=8,_tmp180->f1=f,({int _tmp4C5=Cyc_Tcpat_get_member_offset(ad,f);_tmp180->f2=_tmp4C5;});_tmp180;});}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AA;_tmp3AA.tag=0,({struct _fat_ptr _tmp4C6=({const char*_tmp183="no field name in tagged union pattern";_tag_fat(_tmp183,sizeof(char),38U);});_tmp3AA.f1=_tmp4C6;});_tmp3AA;});void*_tmp181[1];_tmp181[0]=& _tmp182;({int(*_tmp4C7)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4C7(_tag_fat(_tmp181,sizeof(void*),1));});});};}}}else{goto _LL1C;}default: _LL1C:
# 889
({struct Cyc_Warn_String_Warn_Warg_struct _tmp185=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AB;_tmp3AB.tag=0,({struct _fat_ptr _tmp4C8=({const char*_tmp186="non-test pattern in pattern test";_tag_fat(_tmp186,sizeof(char),33U);});_tmp3AB.f1=_tmp4C8;});_tmp3AB;});void*_tmp184[1];_tmp184[0]=& _tmp185;({int(*_tmp4C9)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4C9(_tag_fat(_tmp184,sizeof(void*),1));});});};}};}
# 894
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp3AC;(_tmp3AC.pattern).tag=1U,(_tmp3AC.pattern).val=p;_tmp3AC;});}
# 898
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp188=_cycalloc(sizeof(*_tmp188));({union Cyc_Tcpat_Name_value _tmp4CB=Cyc_Tcpat_Name_v(({const char*_tmp187="NULL";_tag_fat(_tmp187,sizeof(char),5U);}));_tmp188->name=_tmp4CB;}),_tmp188->arity=0,_tmp188->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4CA=Cyc_Tcpat_pw(p);_tmp188->orig_pat=_tmp4CA;});_tmp188;});}
# 901
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18A=_cycalloc(sizeof(*_tmp18A));({union Cyc_Tcpat_Name_value _tmp4CD=Cyc_Tcpat_Name_v(({const char*_tmp189="&";_tag_fat(_tmp189,sizeof(char),2U);}));_tmp18A->name=_tmp4CD;}),_tmp18A->arity=1,_tmp18A->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4CC=Cyc_Tcpat_pw(p);_tmp18A->orig_pat=_tmp4CC;});_tmp18A;});}
# 904
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18C=_cycalloc(sizeof(*_tmp18C));({union Cyc_Tcpat_Name_value _tmp4CF=Cyc_Tcpat_Name_v(({const char*_tmp18B="&";_tag_fat(_tmp18B,sizeof(char),2U);}));_tmp18C->name=_tmp4CF;}),_tmp18C->arity=1,_tmp18C->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _tmp4CE=Cyc_Tcpat_pw(p);_tmp18C->orig_pat=_tmp4CE;});_tmp18C;});}
# 907
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp18D=_cycalloc(sizeof(*_tmp18D));({union Cyc_Tcpat_Name_value _tmp4D0=Cyc_Tcpat_Int_v(i);_tmp18D->name=_tmp4D0;}),_tmp18D->arity=0,_tmp18D->span=0,_tmp18D->orig_pat=p;_tmp18D;});}
# 910
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18E=_cycalloc(sizeof(*_tmp18E));({union Cyc_Tcpat_Name_value _tmp4D2=Cyc_Tcpat_Name_v(f);_tmp18E->name=_tmp4D2;}),_tmp18E->arity=0,_tmp18E->span=0,({union Cyc_Tcpat_PatOrWhere _tmp4D1=Cyc_Tcpat_pw(p);_tmp18E->orig_pat=_tmp4D1;});_tmp18E;});}
# 913
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18F=_cycalloc(sizeof(*_tmp18F));({union Cyc_Tcpat_Name_value _tmp4D4=Cyc_Tcpat_Int_v((int)c);_tmp18F->name=_tmp4D4;}),_tmp18F->arity=0,_tmp18F->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _tmp4D3=Cyc_Tcpat_pw(p);_tmp18F->orig_pat=_tmp4D3;});_tmp18F;});}
# 916
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp191=_cycalloc(sizeof(*_tmp191));({union Cyc_Tcpat_Name_value _tmp4D5=Cyc_Tcpat_Name_v(({const char*_tmp190="$";_tag_fat(_tmp190,sizeof(char),2U);}));_tmp191->name=_tmp4D5;}),_tmp191->arity=i,_tmp191->span=& Cyc_Tcpat_one_opt,_tmp191->orig_pat=p;_tmp191;});}
# 921
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->tag=1,({struct Cyc_Tcpat_Con_s*_tmp4D6=Cyc_Tcpat_null_con(p);_tmp192->f1=_tmp4D6;}),_tmp192->f2=0;_tmp192;});}
# 924
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->tag=1,({struct Cyc_Tcpat_Con_s*_tmp4D7=Cyc_Tcpat_int_con(i,p);_tmp193->f1=_tmp4D7;}),_tmp193->f2=0;_tmp193;});}
# 927
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->tag=1,({struct Cyc_Tcpat_Con_s*_tmp4D8=Cyc_Tcpat_char_con(c,p);_tmp194->f1=_tmp4D8;}),_tmp194->f2=0;_tmp194;});}
# 930
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->tag=1,({struct Cyc_Tcpat_Con_s*_tmp4D9=Cyc_Tcpat_float_con(f,p);_tmp195->f1=_tmp4D9;}),_tmp195->f2=0;_tmp195;});}
# 933
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->tag=1,({struct Cyc_Tcpat_Con_s*_tmp4DB=Cyc_Tcpat_null_ptr_con(p0);_tmp197->f1=_tmp4DB;}),({struct Cyc_List_List*_tmp4DA=({struct Cyc_List_List*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->hd=p,_tmp196->tl=0;_tmp196;});_tmp197->f2=_tmp4DA;});_tmp197;});}
# 936
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->tag=1,({struct Cyc_Tcpat_Con_s*_tmp4DD=Cyc_Tcpat_ptr_con(p0);_tmp199->f1=_tmp4DD;}),({struct Cyc_List_List*_tmp4DC=({struct Cyc_List_List*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->hd=p,_tmp198->tl=0;_tmp198;});_tmp199->f2=_tmp4DC;});_tmp199;});}
# 939
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->tag=1,({struct Cyc_Tcpat_Con_s*_tmp4DF=({int _tmp4DE=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ss);Cyc_Tcpat_tuple_con(_tmp4DE,p);});_tmp19A->f1=_tmp4DF;}),_tmp19A->f2=ss;_tmp19A;});}
# 942
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 944
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp19C=_cycalloc(sizeof(*_tmp19C));({union Cyc_Tcpat_Name_value _tmp4E2=Cyc_Tcpat_Name_v(con_name);_tmp19C->name=_tmp4E2;}),({int _tmp4E1=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ps);_tmp19C->arity=_tmp4E1;}),_tmp19C->span=span,({union Cyc_Tcpat_PatOrWhere _tmp4E0=Cyc_Tcpat_pw(p);_tmp19C->orig_pat=_tmp4E0;});_tmp19C;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->tag=1,_tmp19B->f1=c,_tmp19B->f2=ps;_tmp19B;});}
# 949
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp19D=p->r;void*_stmttmp1E=_tmp19D;void*_tmp19E=_stmttmp1E;void*_tmp1A1;void*_tmp1A0;void*_tmp19F;struct _fat_ptr _tmp1A2;char _tmp1A3;int _tmp1A5;enum Cyc_Absyn_Sign _tmp1A4;switch(*((int*)_tmp19E)){case 0:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 4: _LL6:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->tag=0;_tmp1A6;});goto _LL0;case 9:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 10: _tmp1A4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp19E)->f1;_tmp1A5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp19E)->f2;{enum Cyc_Absyn_Sign sn=_tmp1A4;int i=_tmp1A5;
s=({int _tmp4E3=i;Cyc_Tcpat_int_pat(_tmp4E3,Cyc_Tcpat_pw(p));});goto _LL0;}case 11: _tmp1A3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp19E)->f1;{char c=_tmp1A3;
s=Cyc_Tcpat_char_pat(c,p);goto _LL0;}case 12: _tmp1A2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp19E)->f1;{struct _fat_ptr f=_tmp1A2;
s=Cyc_Tcpat_float_pat(f,p);goto _LL0;}case 1: _tmp19F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp19E)->f2;{struct Cyc_Absyn_Pat*p2=_tmp19F;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 3: _tmp19F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp19E)->f2;{struct Cyc_Absyn_Pat*p2=_tmp19F;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 6: _tmp19F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp19E)->f1;{struct Cyc_Absyn_Pat*pp=_tmp19F;
# 962
{void*_tmp1A7=Cyc_Absyn_compress((void*)_check_null(p->topt));void*_stmttmp1F=_tmp1A7;void*_tmp1A8=_stmttmp1F;void*_tmp1A9;if(*((int*)_tmp1A8)== 3){_tmp1A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).ptr_atts).nullable;{void*n=_tmp1A9;
# 964
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
void*ss=Cyc_Tcpat_compile_pat(pp);
s=is_nullable?Cyc_Tcpat_null_ptr_pat(ss,p): Cyc_Tcpat_ptr_pat(ss,p);
goto _LL27;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AD;_tmp3AD.tag=0,({struct _fat_ptr _tmp4E4=({const char*_tmp1AC="expecting pointertype for pattern!";_tag_fat(_tmp1AC,sizeof(char),35U);});_tmp3AD.f1=_tmp4E4;});_tmp3AD;});void*_tmp1AA[1];_tmp1AA[0]=& _tmp1AB;({int(*_tmp4E5)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4E5(_tag_fat(_tmp1AA,sizeof(void*),1));});});}_LL27:;}
# 970
goto _LL0;}case 8: _tmp19F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp19E)->f1;_tmp1A0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp19E)->f2;_tmp1A1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp19E)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_tmp19F;struct Cyc_Absyn_Datatypefield*tuf=_tmp1A0;struct Cyc_List_List*ps=_tmp1A1;
# 972
int*span;
{void*_tmp1AD=Cyc_Absyn_compress((void*)_check_null(p->topt));void*_stmttmp20=_tmp1AD;void*_tmp1AE=_stmttmp20;if(*((int*)_tmp1AE)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AE)->f1)){case 18:
# 975
 span=tud->is_extensible?0:({int*_tmp1AF=_cycalloc_atomic(sizeof(*_tmp1AF));({int _tmp4E6=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v);*_tmp1AF=_tmp4E6;});_tmp1AF;});
goto _LL2C;case 19:
 span=& Cyc_Tcpat_one_opt;goto _LL2C;default: goto _LL31;}else{_LL31:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AE;_tmp3AE.tag=0,({struct _fat_ptr _tmp4E7=({const char*_tmp1B2="void datatype pattern has bad type";_tag_fat(_tmp1B2,sizeof(char),35U);});_tmp3AE.f1=_tmp4E7;});_tmp3AE;});void*_tmp1B0[1];_tmp1B0[0]=& _tmp1B1;({int(*_tmp4E8)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4E8(_tag_fat(_tmp1B0,sizeof(void*),1));});});}_LL2C:;}
# 980
s=({struct _fat_ptr _tmp4EB=*(*tuf->name).f2;int*_tmp4EA=span;struct Cyc_List_List*_tmp4E9=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_con_pat(_tmp4EB,_tmp4EA,_tmp4E9,p);});
goto _LL0;}case 5: _tmp19F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp19E)->f1;{struct Cyc_List_List*ps=_tmp19F;
# 983
s=({struct Cyc_List_List*_tmp4EC=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_tuple_pat(_tmp4EC,Cyc_Tcpat_pw(p));});goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp19E)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp19E)->f1)->KnownAggr).tag == 2){_tmp19F=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp19E)->f1)->KnownAggr).val;_tmp1A0=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp19E)->f3;{struct Cyc_Absyn_Aggrdecl*ad=_tmp19F;struct Cyc_List_List*dlps=_tmp1A0;
# 988
if((int)ad->kind == 0){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 992
int found=({struct _fat_ptr _tmp4ED=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp4ED,({const char*_tmp1C0="";_tag_fat(_tmp1C0,sizeof(char),1U);}));})== 0;
{struct Cyc_List_List*dlps0=dlps;for(0;!found && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple16*_tmp1B3=(struct _tuple16*)dlps0->hd;struct _tuple16*_stmttmp21=_tmp1B3;struct _tuple16*_tmp1B4=_stmttmp21;void*_tmp1B6;void*_tmp1B5;_tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B4->f2;{struct Cyc_List_List*dl=_tmp1B5;struct Cyc_Absyn_Pat*p=_tmp1B6;
struct Cyc_List_List*_tmp1B7=dl;void*_tmp1B8;if(_tmp1B7 != 0){if(*((int*)((struct Cyc_List_List*)_tmp1B7)->hd)== 1){if(((struct Cyc_List_List*)_tmp1B7)->tl == 0){_tmp1B8=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1B7->hd)->f1;{struct _fat_ptr*f=_tmp1B8;
# 997
if(Cyc_strptrcmp(f,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({void*_tmp4EE=Cyc_Tcpat_compile_pat(p);_tmp1B9->hd=_tmp4EE;}),_tmp1B9->tl=ps;_tmp1B9;});
found=1;}
# 1001
goto _LL36;}}else{goto _LL39;}}else{goto _LL39;}}else{_LL39:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AF;_tmp3AF.tag=0,({struct _fat_ptr _tmp4EF=({const char*_tmp1BC="bad designator(s)";_tag_fat(_tmp1BC,sizeof(char),18U);});_tmp3AF.f1=_tmp4EF;});_tmp3AF;});void*_tmp1BA[1];_tmp1BA[0]=& _tmp1BB;({int(*_tmp4F0)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4F0(_tag_fat(_tmp1BA,sizeof(void*),1));});});}_LL36:;}}}
# 1005
if(!found)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B0;_tmp3B0.tag=0,({struct _fat_ptr _tmp4F1=({const char*_tmp1BF="bad designator";_tag_fat(_tmp1BF,sizeof(char),15U);});_tmp3B0.f1=_tmp4F1;});_tmp3B0;});void*_tmp1BD[1];_tmp1BD[0]=& _tmp1BE;({int(*_tmp4F2)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4F2(_tag_fat(_tmp1BD,sizeof(void*),1));});});}}
# 1008
s=({struct Cyc_List_List*_tmp4F3=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(ps);Cyc_Tcpat_tuple_pat(_tmp4F3,Cyc_Tcpat_pw(p));});}else{
# 1011
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B1;_tmp3B1.tag=0,({struct _fat_ptr _tmp4F4=({const char*_tmp1C3="patterns on untagged unions not yet supported.";_tag_fat(_tmp1C3,sizeof(char),47U);});_tmp3B1.f1=_tmp4F4;});_tmp3B1;});void*_tmp1C1[1];_tmp1C1[0]=& _tmp1C2;({unsigned _tmp4F5=p->loc;Cyc_Warn_err2(_tmp4F5,_tag_fat(_tmp1C1,sizeof(void*),1));});});{
int*span=({int*_tmp1CB=_cycalloc_atomic(sizeof(*_tmp1CB));({int _tmp4F6=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);*_tmp1CB=_tmp4F6;});_tmp1CB;});
struct Cyc_List_List*_tmp1C4=dlps;void*_tmp1C6;void*_tmp1C5;if(_tmp1C4 != 0){if(((struct _tuple16*)((struct Cyc_List_List*)_tmp1C4)->hd)->f1 != 0){if(*((int*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp1C4)->hd)->f1)->hd)== 1){if(((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp1C4)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp1C4)->tl == 0){_tmp1C5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple16*)_tmp1C4->hd)->f1)->hd)->f1;_tmp1C6=((struct _tuple16*)_tmp1C4->hd)->f2;{struct _fat_ptr*f=_tmp1C5;struct Cyc_Absyn_Pat*p2=_tmp1C6;
# 1016
s=({struct _fat_ptr _tmp4FA=*f;int*_tmp4F9=span;struct Cyc_List_List*_tmp4F8=({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({void*_tmp4F7=Cyc_Tcpat_compile_pat(p2);_tmp1C7->hd=_tmp4F7;}),_tmp1C7->tl=0;_tmp1C7;});Cyc_Tcpat_con_pat(_tmp4FA,_tmp4F9,_tmp4F8,p);});
goto _LL3B;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{_LL3E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B2;_tmp3B2.tag=0,({struct _fat_ptr _tmp4FB=({const char*_tmp1CA="bad union pattern";_tag_fat(_tmp1CA,sizeof(char),18U);});_tmp3B2.f1=_tmp4FB;});_tmp3B2;});void*_tmp1C8[1];_tmp1C8[0]=& _tmp1C9;({int(*_tmp4FC)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp4FC(_tag_fat(_tmp1C8,sizeof(void*),1));});});}_LL3B:;}}
# 1021
goto _LL0;}}else{goto _LL23;}}else{_LL23: _LL24:
# 1033
 goto _LL26;}case 13: _tmp19F=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp19E)->f2;{struct Cyc_Absyn_Enumfield*ef=_tmp19F;
# 1028
_tmp19F=ef;goto _LL1E;}case 14: _tmp19F=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp19E)->f2;_LL1E: {struct Cyc_Absyn_Enumfield*ef=_tmp19F;
s=Cyc_Tcpat_con_pat(*(*ef->name).f2,0,0,p);goto _LL0;}case 15:
# 1031
 goto _LL22;case 16: _LL22:
 goto _LL24;default: _LL26:
# 1034
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->tag=0;_tmp1CC;});}_LL0:;}
# 1036
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1061
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp1CD=({struct _tuple0 _tmp3B3;_tmp3B3.f1=a1,_tmp3B3.f2=a2;_tmp3B3;});struct _tuple0 _stmttmp22=_tmp1CD;struct _tuple0 _tmp1CE=_stmttmp22;int _tmp1D1;int _tmp1CF;void*_tmp1D2;void*_tmp1D0;unsigned _tmp1D4;unsigned _tmp1D3;switch(*((int*)_tmp1CE.f1)){case 0: if(*((int*)_tmp1CE.f2)== 0)
return 1;else{goto _LLB;}case 1: if(*((int*)_tmp1CE.f2)== 1)
return 1;else{goto _LLB;}case 2: if(*((int*)_tmp1CE.f2)== 2){_tmp1D3=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1CE.f1)->f1;_tmp1D4=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1CE.f2)->f1;{unsigned i1=_tmp1D3;unsigned i2=_tmp1D4;
return i1 == i2;}}else{goto _LLB;}case 3: if(*((int*)_tmp1CE.f2)== 3){_tmp1D0=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1CE.f1)->f2;_tmp1D3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1CE.f1)->f3;_tmp1D2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1CE.f2)->f2;_tmp1D4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1CE.f2)->f3;{struct Cyc_Absyn_Datatypefield*df1=_tmp1D0;unsigned i1=_tmp1D3;struct Cyc_Absyn_Datatypefield*df2=_tmp1D2;unsigned i2=_tmp1D4;
# 1067
return df1 == df2 && i1 == i2;}}else{goto _LLB;}default: if(*((int*)_tmp1CE.f2)== 4){_tmp1CF=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1CE.f1)->f1;_tmp1D0=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1CE.f1)->f2;_tmp1D1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1CE.f2)->f1;_tmp1D2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1CE.f2)->f2;{int b1=_tmp1CF;struct _fat_ptr*f1=_tmp1D0;int b2=_tmp1D1;struct _fat_ptr*f2=_tmp1D2;
# 1069
return b1 == b2 && Cyc_strptrcmp(f1,f2)== 0;}}else{_LLB:
 return 0;}};}
# 1074
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
for(1;p1 != 0 && p2 != 0;(p1=p1->tl,p2=p2->tl)){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;}
if(p1 != p2)return 0;
return 1;}
# 1081
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp1D5=Cyc_Tcpat_get_pat_test(con->orig_pat);void*test=_tmp1D5;
void*_tmp1D6=d2;void*_tmp1D9;void*_tmp1D8;void*_tmp1D7;if(*((int*)_tmp1D6)== 2){_tmp1D7=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1D6)->f1;_tmp1D8=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1D6)->f2;_tmp1D9=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1D6)->f3;{struct Cyc_List_List*access2=_tmp1D7;struct Cyc_List_List*switch_clauses=_tmp1D8;void*default_decision=_tmp1D9;
# 1085
if(Cyc_Tcpat_same_path(access,access2)&&(int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->tag=2,_tmp1DC->f1=access2,({
struct Cyc_List_List*_tmp4FE=({struct Cyc_List_List*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));({struct _tuple0*_tmp4FD=({struct _tuple0*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->f1=test,_tmp1DA->f2=d1;_tmp1DA;});_tmp1DB->hd=_tmp4FD;}),_tmp1DB->tl=switch_clauses;_tmp1DB;});_tmp1DC->f2=_tmp4FE;}),_tmp1DC->f3=default_decision;_tmp1DC;});
# 1089
goto _LL4;}}else{_LL4:
 return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->tag=2,_tmp1DF->f1=access,({struct Cyc_List_List*_tmp500=({struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));({struct _tuple0*_tmp4FF=({struct _tuple0*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->f1=test,_tmp1DD->f2=d1;_tmp1DD;});_tmp1DE->hd=_tmp4FF;}),_tmp1DE->tl=0;_tmp1DE;});_tmp1DF->f2=_tmp500;}),_tmp1DF->f3=d2;_tmp1DF;});};}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1102
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1105
static void Cyc_Tcpat_print_tab(int i){
for(1;i != 0;-- i){
({void*_tmp1E0=0U;({struct Cyc___cycFILE*_tmp502=Cyc_stderr;struct _fat_ptr _tmp501=({const char*_tmp1E1=" ";_tag_fat(_tmp1E1,sizeof(char),2U);});Cyc_fprintf(_tmp502,_tmp501,_tag_fat(_tmp1E0,sizeof(void*),0));});});}}
# 1109
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp1E2=c->name;union Cyc_Tcpat_Name_value _stmttmp23=_tmp1E2;union Cyc_Tcpat_Name_value _tmp1E3=_stmttmp23;int _tmp1E4;struct _fat_ptr _tmp1E5;if((_tmp1E3.Name_v).tag == 1){_tmp1E5=(_tmp1E3.Name_v).val;{struct _fat_ptr s=_tmp1E5;
({struct Cyc_String_pa_PrintArg_struct _tmp1E8=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0,_tmp3B4.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3B4;});void*_tmp1E6[1];_tmp1E6[0]=& _tmp1E8;({struct Cyc___cycFILE*_tmp504=Cyc_stderr;struct _fat_ptr _tmp503=({const char*_tmp1E7="%s";_tag_fat(_tmp1E7,sizeof(char),3U);});Cyc_fprintf(_tmp504,_tmp503,_tag_fat(_tmp1E6,sizeof(void*),1));});});goto _LL0;}}else{_tmp1E4=(_tmp1E3.Int_v).val;{int i=_tmp1E4;
({struct Cyc_Int_pa_PrintArg_struct _tmp1EB=({struct Cyc_Int_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=1,_tmp3B5.f1=(unsigned long)i;_tmp3B5;});void*_tmp1E9[1];_tmp1E9[0]=& _tmp1EB;({struct Cyc___cycFILE*_tmp506=Cyc_stderr;struct _fat_ptr _tmp505=({const char*_tmp1EA="%d";_tag_fat(_tmp1EA,sizeof(char),3U);});Cyc_fprintf(_tmp506,_tmp505,_tag_fat(_tmp1E9,sizeof(void*),1));});});goto _LL0;}}_LL0:;}
# 1116
static void Cyc_Tcpat_print_access(void*a){
void*_tmp1EC=a;int _tmp1ED;void*_tmp1EE;unsigned _tmp1EF;switch(*((int*)_tmp1EC)){case 0:
({void*_tmp1F0=0U;({struct Cyc___cycFILE*_tmp508=Cyc_stderr;struct _fat_ptr _tmp507=({const char*_tmp1F1="DUMMY";_tag_fat(_tmp1F1,sizeof(char),6U);});Cyc_fprintf(_tmp508,_tmp507,_tag_fat(_tmp1F0,sizeof(void*),0));});});goto _LL0;case 1:
({void*_tmp1F2=0U;({struct Cyc___cycFILE*_tmp50A=Cyc_stderr;struct _fat_ptr _tmp509=({const char*_tmp1F3="*";_tag_fat(_tmp1F3,sizeof(char),2U);});Cyc_fprintf(_tmp50A,_tmp509,_tag_fat(_tmp1F2,sizeof(void*),0));});});goto _LL0;case 2: _tmp1EF=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1EC)->f1;{unsigned i=_tmp1EF;
({struct Cyc_Int_pa_PrintArg_struct _tmp1F6=({struct Cyc_Int_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=1,_tmp3B6.f1=(unsigned long)((int)i);_tmp3B6;});void*_tmp1F4[1];_tmp1F4[0]=& _tmp1F6;({struct Cyc___cycFILE*_tmp50C=Cyc_stderr;struct _fat_ptr _tmp50B=({const char*_tmp1F5="[%d]";_tag_fat(_tmp1F5,sizeof(char),5U);});Cyc_fprintf(_tmp50C,_tmp50B,_tag_fat(_tmp1F4,sizeof(void*),1));});});goto _LL0;}case 3: _tmp1EE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1EC)->f2;_tmp1EF=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1EC)->f3;{struct Cyc_Absyn_Datatypefield*f=_tmp1EE;unsigned i=_tmp1EF;
# 1122
({struct Cyc_String_pa_PrintArg_struct _tmp1F9=({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0,({struct _fat_ptr _tmp50D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp3B8.f1=_tmp50D;});_tmp3B8;});struct Cyc_Int_pa_PrintArg_struct _tmp1FA=({struct Cyc_Int_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=1,_tmp3B7.f1=(unsigned long)((int)i);_tmp3B7;});void*_tmp1F7[2];_tmp1F7[0]=& _tmp1F9,_tmp1F7[1]=& _tmp1FA;({struct Cyc___cycFILE*_tmp50F=Cyc_stderr;struct _fat_ptr _tmp50E=({const char*_tmp1F8="%s[%d]";_tag_fat(_tmp1F8,sizeof(char),7U);});Cyc_fprintf(_tmp50F,_tmp50E,_tag_fat(_tmp1F7,sizeof(void*),2));});});goto _LL0;}default: _tmp1ED=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1EC)->f1;_tmp1EE=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1EC)->f2;{int tagged=_tmp1ED;struct _fat_ptr*f=_tmp1EE;
# 1124
if(tagged)
({void*_tmp1FB=0U;({struct Cyc___cycFILE*_tmp511=Cyc_stderr;struct _fat_ptr _tmp510=({const char*_tmp1FC=".tagunion";_tag_fat(_tmp1FC,sizeof(char),10U);});Cyc_fprintf(_tmp511,_tmp510,_tag_fat(_tmp1FB,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1FF=({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0,_tmp3B9.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp3B9;});void*_tmp1FD[1];_tmp1FD[0]=& _tmp1FF;({struct Cyc___cycFILE*_tmp513=Cyc_stderr;struct _fat_ptr _tmp512=({const char*_tmp1FE=".%s";_tag_fat(_tmp1FE,sizeof(char),4U);});Cyc_fprintf(_tmp513,_tmp512,_tag_fat(_tmp1FD,sizeof(void*),1));});});
goto _LL0;}}_LL0:;}
# 1131
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp200=p;int _tmp202;unsigned _tmp203;struct _fat_ptr _tmp204;void*_tmp201;switch(*((int*)_tmp200)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp200)->f1 == 0){
({void*_tmp205=0U;({struct Cyc___cycFILE*_tmp515=Cyc_stderr;struct _fat_ptr _tmp514=({const char*_tmp206="where(NULL)";_tag_fat(_tmp206,sizeof(char),12U);});Cyc_fprintf(_tmp515,_tmp514,_tag_fat(_tmp205,sizeof(void*),0));});});goto _LL0;}else{_tmp201=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp200)->f1;{struct Cyc_Absyn_Exp*e=_tmp201;
({struct Cyc_String_pa_PrintArg_struct _tmp209=({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0,({struct _fat_ptr _tmp516=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(e)));_tmp3BA.f1=_tmp516;});_tmp3BA;});void*_tmp207[1];_tmp207[0]=& _tmp209;({struct Cyc___cycFILE*_tmp518=Cyc_stderr;struct _fat_ptr _tmp517=({const char*_tmp208="where(%s)";_tag_fat(_tmp208,sizeof(char),10U);});Cyc_fprintf(_tmp518,_tmp517,_tag_fat(_tmp207,sizeof(void*),1));});});goto _LL0;}}case 1:
({void*_tmp20A=0U;({struct Cyc___cycFILE*_tmp51A=Cyc_stderr;struct _fat_ptr _tmp519=({const char*_tmp20B="NULL";_tag_fat(_tmp20B,sizeof(char),5U);});Cyc_fprintf(_tmp51A,_tmp519,_tag_fat(_tmp20A,sizeof(void*),0));});});goto _LL0;case 2:
({void*_tmp20C=0U;({struct Cyc___cycFILE*_tmp51C=Cyc_stderr;struct _fat_ptr _tmp51B=({const char*_tmp20D="NOT-NULL:";_tag_fat(_tmp20D,sizeof(char),10U);});Cyc_fprintf(_tmp51C,_tmp51B,_tag_fat(_tmp20C,sizeof(void*),0));});});goto _LL0;case 4: _tmp201=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp200)->f2;{struct Cyc_Absyn_Enumfield*ef=_tmp201;
_tmp201=ef;goto _LLC;}case 3: _tmp201=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp200)->f2;_LLC: {struct Cyc_Absyn_Enumfield*ef=_tmp201;
({struct Cyc_String_pa_PrintArg_struct _tmp210=({struct Cyc_String_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=0,({struct _fat_ptr _tmp51D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ef->name));_tmp3BB.f1=_tmp51D;});_tmp3BB;});void*_tmp20E[1];_tmp20E[0]=& _tmp210;({struct Cyc___cycFILE*_tmp51F=Cyc_stderr;struct _fat_ptr _tmp51E=({const char*_tmp20F="%s";_tag_fat(_tmp20F,sizeof(char),3U);});Cyc_fprintf(_tmp51F,_tmp51E,_tag_fat(_tmp20E,sizeof(void*),1));});});goto _LL0;}case 5: _tmp204=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp200)->f1;{struct _fat_ptr s=_tmp204;
({struct Cyc_String_pa_PrintArg_struct _tmp213=({struct Cyc_String_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=0,_tmp3BC.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3BC;});void*_tmp211[1];_tmp211[0]=& _tmp213;({struct Cyc___cycFILE*_tmp521=Cyc_stderr;struct _fat_ptr _tmp520=({const char*_tmp212="%s";_tag_fat(_tmp212,sizeof(char),3U);});Cyc_fprintf(_tmp521,_tmp520,_tag_fat(_tmp211,sizeof(void*),1));});});goto _LL0;}case 6: _tmp203=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp200)->f1;{unsigned i=_tmp203;
({struct Cyc_Int_pa_PrintArg_struct _tmp216=({struct Cyc_Int_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=1,_tmp3BD.f1=(unsigned long)((int)i);_tmp3BD;});void*_tmp214[1];_tmp214[0]=& _tmp216;({struct Cyc___cycFILE*_tmp523=Cyc_stderr;struct _fat_ptr _tmp522=({const char*_tmp215="%d";_tag_fat(_tmp215,sizeof(char),3U);});Cyc_fprintf(_tmp523,_tmp522,_tag_fat(_tmp214,sizeof(void*),1));});});goto _LL0;}case 7: _tmp202=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp200)->f1;{int i=_tmp202;
({struct Cyc_Int_pa_PrintArg_struct _tmp219=({struct Cyc_Int_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=1,_tmp3BE.f1=(unsigned long)i;_tmp3BE;});void*_tmp217[1];_tmp217[0]=& _tmp219;({struct Cyc___cycFILE*_tmp525=Cyc_stderr;struct _fat_ptr _tmp524=({const char*_tmp218="datatypetag(%d)";_tag_fat(_tmp218,sizeof(char),16U);});Cyc_fprintf(_tmp525,_tmp524,_tag_fat(_tmp217,sizeof(void*),1));});});goto _LL0;}case 8: _tmp201=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp200)->f1;_tmp202=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp200)->f2;{struct _fat_ptr*f=_tmp201;int i=_tmp202;
({struct Cyc_String_pa_PrintArg_struct _tmp21C=({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0,_tmp3C0.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp3C0;});struct Cyc_Int_pa_PrintArg_struct _tmp21D=({struct Cyc_Int_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=1,_tmp3BF.f1=(unsigned long)i;_tmp3BF;});void*_tmp21A[2];_tmp21A[0]=& _tmp21C,_tmp21A[1]=& _tmp21D;({struct Cyc___cycFILE*_tmp527=Cyc_stderr;struct _fat_ptr _tmp526=({const char*_tmp21B="uniontag[%s](%d)";_tag_fat(_tmp21B,sizeof(char),17U);});Cyc_fprintf(_tmp527,_tmp526,_tag_fat(_tmp21A,sizeof(void*),2));});});goto _LL0;}default: _tmp201=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp200)->f2;{struct Cyc_Absyn_Datatypefield*f=_tmp201;
# 1144
({struct Cyc_String_pa_PrintArg_struct _tmp220=({struct Cyc_String_pa_PrintArg_struct _tmp3C1;_tmp3C1.tag=0,({struct _fat_ptr _tmp528=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp3C1.f1=_tmp528;});_tmp3C1;});void*_tmp21E[1];_tmp21E[0]=& _tmp220;({struct Cyc___cycFILE*_tmp52A=Cyc_stderr;struct _fat_ptr _tmp529=({const char*_tmp21F="datatypefield(%s)";_tag_fat(_tmp21F,sizeof(char),18U);});Cyc_fprintf(_tmp52A,_tmp529,_tag_fat(_tmp21E,sizeof(void*),1));});});}}_LL0:;}
# 1148
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp223=({struct Cyc_String_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=0,({struct _fat_ptr _tmp52B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp3C2.f1=_tmp52B;});_tmp3C2;});void*_tmp221[1];_tmp221[0]=& _tmp223;({struct Cyc___cycFILE*_tmp52D=Cyc_stderr;struct _fat_ptr _tmp52C=({const char*_tmp222="%s";_tag_fat(_tmp222,sizeof(char),3U);});Cyc_fprintf(_tmp52D,_tmp52C,_tag_fat(_tmp221,sizeof(void*),1));});});}
# 1152
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp224=d;void*_tmp227;void*_tmp226;void*_tmp225;switch(*((int*)_tmp224)){case 1: _tmp225=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp224)->f1;{struct Cyc_Tcpat_Rhs*rhs=_tmp225;
# 1155
Cyc_Tcpat_print_tab(tab);
({void*_tmp228=0U;({struct Cyc___cycFILE*_tmp52F=Cyc_stderr;struct _fat_ptr _tmp52E=({const char*_tmp229="Success(";_tag_fat(_tmp229,sizeof(char),9U);});Cyc_fprintf(_tmp52F,_tmp52E,_tag_fat(_tmp228,sizeof(void*),0));});});Cyc_Tcpat_print_rhs(rhs);({void*_tmp22A=0U;({struct Cyc___cycFILE*_tmp531=Cyc_stderr;struct _fat_ptr _tmp530=({const char*_tmp22B=")\n";_tag_fat(_tmp22B,sizeof(char),3U);});Cyc_fprintf(_tmp531,_tmp530,_tag_fat(_tmp22A,sizeof(void*),0));});});
goto _LL0;}case 0:
({void*_tmp22C=0U;({struct Cyc___cycFILE*_tmp533=Cyc_stderr;struct _fat_ptr _tmp532=({const char*_tmp22D="Failure\n";_tag_fat(_tmp22D,sizeof(char),9U);});Cyc_fprintf(_tmp533,_tmp532,_tag_fat(_tmp22C,sizeof(void*),0));});});goto _LL0;default: _tmp225=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp224)->f1;_tmp226=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp224)->f2;_tmp227=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp224)->f3;{struct Cyc_List_List*a=_tmp225;struct Cyc_List_List*cases=_tmp226;void*def=_tmp227;
# 1160
Cyc_Tcpat_print_tab(tab);
({void*_tmp22E=0U;({struct Cyc___cycFILE*_tmp535=Cyc_stderr;struct _fat_ptr _tmp534=({const char*_tmp22F="Switch[";_tag_fat(_tmp22F,sizeof(char),8U);});Cyc_fprintf(_tmp535,_tmp534,_tag_fat(_tmp22E,sizeof(void*),0));});});
for(1;a != 0;a=a->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)a->hd)->access);
if(a->tl != 0)({void*_tmp230=0U;({struct Cyc___cycFILE*_tmp537=Cyc_stderr;struct _fat_ptr _tmp536=({const char*_tmp231=",";_tag_fat(_tmp231,sizeof(char),2U);});Cyc_fprintf(_tmp537,_tmp536,_tag_fat(_tmp230,sizeof(void*),0));});});}
# 1166
({void*_tmp232=0U;({struct Cyc___cycFILE*_tmp539=Cyc_stderr;struct _fat_ptr _tmp538=({const char*_tmp233="] {\n";_tag_fat(_tmp233,sizeof(char),5U);});Cyc_fprintf(_tmp539,_tmp538,_tag_fat(_tmp232,sizeof(void*),0));});});
for(1;cases != 0;cases=cases->tl){
struct _tuple0 _tmp234=*((struct _tuple0*)cases->hd);struct _tuple0 _stmttmp24=_tmp234;struct _tuple0 _tmp235=_stmttmp24;void*_tmp237;void*_tmp236;_tmp236=_tmp235.f1;_tmp237=_tmp235.f2;{void*pt=_tmp236;void*d=_tmp237;
Cyc_Tcpat_print_tab(tab);
({void*_tmp238=0U;({struct Cyc___cycFILE*_tmp53B=Cyc_stderr;struct _fat_ptr _tmp53A=({const char*_tmp239="case ";_tag_fat(_tmp239,sizeof(char),6U);});Cyc_fprintf(_tmp53B,_tmp53A,_tag_fat(_tmp238,sizeof(void*),0));});});
Cyc_Tcpat_print_pat_test(pt);
({void*_tmp23A=0U;({struct Cyc___cycFILE*_tmp53D=Cyc_stderr;struct _fat_ptr _tmp53C=({const char*_tmp23B=":\n";_tag_fat(_tmp23B,sizeof(char),3U);});Cyc_fprintf(_tmp53D,_tmp53C,_tag_fat(_tmp23A,sizeof(void*),0));});});
Cyc_Tcpat_print_dec_tree(d,tab + 7);}}
# 1175
Cyc_Tcpat_print_tab(tab);
({void*_tmp23C=0U;({struct Cyc___cycFILE*_tmp53F=Cyc_stderr;struct _fat_ptr _tmp53E=({const char*_tmp23D="default:\n";_tag_fat(_tmp23D,sizeof(char),10U);});Cyc_fprintf(_tmp53F,_tmp53E,_tag_fat(_tmp23C,sizeof(void*),0));});});
Cyc_Tcpat_print_dec_tree(def,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp23E=0U;({struct Cyc___cycFILE*_tmp541=Cyc_stderr;struct _fat_ptr _tmp540=({const char*_tmp23F="}\n";_tag_fat(_tmp23F,sizeof(char),3U);});Cyc_fprintf(_tmp541,_tmp540,_tag_fat(_tmp23E,sizeof(void*),0));});});}}_LL0:;}
# 1183
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1191
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp240=td;void*_tmp241;if(*((int*)_tmp240)== 1){_tmp241=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp240)->f1;{struct Cyc_Set_Set*cs=_tmp241;
# 1201
return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->tag=1,({struct Cyc_Set_Set*_tmp542=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_insert;})(cs,c);_tmp242->f1=_tmp542;});_tmp242;});}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp244=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C3;_tmp3C3.tag=0,({struct _fat_ptr _tmp543=({const char*_tmp245="add_neg called when td is Positive";_tag_fat(_tmp245,sizeof(char),35U);});_tmp3C3.f1=_tmp543;});_tmp3C3;});void*_tmp243[1];_tmp243[0]=& _tmp244;({int(*_tmp544)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp544(_tag_fat(_tmp243,sizeof(void*),1));});});};}
# 1208
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp246=td;void*_tmp247;if(*((int*)_tmp246)== 0){_tmp247=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp246)->f1;{struct Cyc_Tcpat_Con_s*c2=_tmp247;
# 1212
return Cyc_Tcpat_compare_con(c,c2)== 0?0U: 1U;}}else{_tmp247=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp246)->f1;{struct Cyc_Set_Set*cs=_tmp247;
# 1215
if(({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;})(cs,c))return 1U;
# 1218
if(c->span != 0 &&({int _tmp545=*((int*)_check_null(c->span));_tmp545 == ({(int(*)(struct Cyc_Set_Set*))Cyc_Set_cardinality;})(cs)+ 1;}))
return 0U;
# 1221
return 2U;}};}struct _tuple22{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1229
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp248=ctxt;void*_tmp24B;void*_tmp24A;void*_tmp249;if(_tmp248 == 0)
return 0;else{_tmp249=((struct _tuple22*)_tmp248->hd)->f1;_tmp24A=((struct _tuple22*)_tmp248->hd)->f2;_tmp24B=_tmp248->tl;{struct Cyc_Tcpat_Con_s*c=_tmp249;struct Cyc_List_List*args=_tmp24A;struct Cyc_List_List*tl=_tmp24B;
return({struct Cyc_List_List*_tmp24E=_cycalloc(sizeof(*_tmp24E));({struct _tuple22*_tmp547=({struct _tuple22*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->f1=c,({struct Cyc_List_List*_tmp546=({struct Cyc_List_List*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->hd=dsc,_tmp24C->tl=args;_tmp24C;});_tmp24D->f2=_tmp546;});_tmp24D;});_tmp24E->hd=_tmp547;}),_tmp24E->tl=tl;_tmp24E;});}};}
# 1239
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp24F=ctxt;void*_tmp252;void*_tmp251;void*_tmp250;if(_tmp24F == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp254=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4;_tmp3C4.tag=0,({struct _fat_ptr _tmp548=({const char*_tmp255="norm_context: empty context";_tag_fat(_tmp255,sizeof(char),28U);});_tmp3C4.f1=_tmp548;});_tmp3C4;});void*_tmp253[1];_tmp253[0]=& _tmp254;({int(*_tmp549)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp549(_tag_fat(_tmp253,sizeof(void*),1));});});else{_tmp250=((struct _tuple22*)_tmp24F->hd)->f1;_tmp251=((struct _tuple22*)_tmp24F->hd)->f2;_tmp252=_tmp24F->tl;{struct Cyc_Tcpat_Con_s*c=_tmp250;struct Cyc_List_List*args=_tmp251;struct Cyc_List_List*tl=_tmp252;
return({struct Cyc_List_List*_tmp54B=tl;Cyc_Tcpat_augment(_tmp54B,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->tag=0,_tmp256->f1=c,({struct Cyc_List_List*_tmp54A=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(args);_tmp256->f2=_tmp54A;});_tmp256;}));});}};}
# 1251
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1253
struct _tuple1 _tmp257=({struct _tuple1 _tmp3C6;_tmp3C6.f1=ctxt,_tmp3C6.f2=work;_tmp3C6;});struct _tuple1 _stmttmp25=_tmp257;struct _tuple1 _tmp258=_stmttmp25;void*_tmp25D;void*_tmp25C;void*_tmp25B;void*_tmp25A;void*_tmp259;if(_tmp258.f1 == 0){if(_tmp258.f2 == 0)
return dsc;else{
goto _LL6;}}else{if(_tmp258.f2 == 0){_LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C5;_tmp3C5.tag=0,({struct _fat_ptr _tmp54C=({const char*_tmp260="build_desc: ctxt and work don't match";_tag_fat(_tmp260,sizeof(char),38U);});_tmp3C5.f1=_tmp54C;});_tmp3C5;});void*_tmp25E[1];_tmp25E[0]=& _tmp25F;({int(*_tmp54D)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp54D(_tag_fat(_tmp25E,sizeof(void*),1));});});}else{_tmp259=((struct _tuple22*)(_tmp258.f1)->hd)->f1;_tmp25A=((struct _tuple22*)(_tmp258.f1)->hd)->f2;_tmp25B=(_tmp258.f1)->tl;_tmp25C=((struct _tuple21*)(_tmp258.f2)->hd)->f3;_tmp25D=(_tmp258.f2)->tl;{struct Cyc_Tcpat_Con_s*c=_tmp259;struct Cyc_List_List*args=_tmp25A;struct Cyc_List_List*rest=_tmp25B;struct Cyc_List_List*dargs=_tmp25C;struct Cyc_List_List*work2=_tmp25D;
# 1258
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp261=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->tag=0,_tmp263->f1=c,({struct Cyc_List_List*_tmp550=({struct Cyc_List_List*(*_tmp54F)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp54E=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(args);_tmp54F(_tmp54E,({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->hd=dsc,_tmp262->tl=dargs;_tmp262;}));});_tmp263->f2=_tmp550;});_tmp263;});struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*td=_tmp261;
return Cyc_Tcpat_build_desc(rest,(void*)td,work2);}}};}
# 1263
static void*Cyc_Tcpat_match(void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Tcpat_Rhs*,struct Cyc_List_List*);struct _tuple23{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1268
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp264=allmrules;void*_tmp267;void*_tmp266;void*_tmp265;if(_tmp264 == 0)
return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->tag=0,_tmp268->f1=dsc;_tmp268;});else{_tmp265=((struct _tuple23*)_tmp264->hd)->f1;_tmp266=((struct _tuple23*)_tmp264->hd)->f2;_tmp267=_tmp264->tl;{void*pat1=_tmp265;struct Cyc_Tcpat_Rhs*rhs1=_tmp266;struct Cyc_List_List*rulerest=_tmp267;
# 1272
return Cyc_Tcpat_match(pat1,0,dsc,0,0,rhs1,rulerest);}};}
# 1277
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp552=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->tag=1,({struct Cyc_Set_Set*_tmp551=Cyc_Tcpat_empty_con_set();_tmp269->f1=_tmp551;});_tmp269;});Cyc_Tcpat_or_match(_tmp552,allmrules);});}
# 1284
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1287
struct Cyc_List_List*_tmp26A=work;void*_tmp26E;void*_tmp26D;void*_tmp26C;void*_tmp26B;if(_tmp26A == 0)
return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->tag=1,_tmp26F->f1=right_hand_side;_tmp26F;});else{if(((struct _tuple21*)((struct Cyc_List_List*)_tmp26A)->hd)->f1 == 0){if(((struct _tuple21*)((struct Cyc_List_List*)_tmp26A)->hd)->f2 == 0){if(((struct _tuple21*)((struct Cyc_List_List*)_tmp26A)->hd)->f3 == 0){_tmp26B=_tmp26A->tl;{struct Cyc_List_List*workr=_tmp26B;
# 1290
return({struct Cyc_List_List*_tmp555=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp554=workr;struct Cyc_Tcpat_Rhs*_tmp553=right_hand_side;Cyc_Tcpat_and_match(_tmp555,_tmp554,_tmp553,rules);});}}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _tmp26B=((struct _tuple21*)_tmp26A->hd)->f1;_tmp26C=((struct _tuple21*)_tmp26A->hd)->f2;_tmp26D=((struct _tuple21*)_tmp26A->hd)->f3;_tmp26E=_tmp26A->tl;{struct Cyc_List_List*pats=_tmp26B;struct Cyc_List_List*objs=_tmp26C;struct Cyc_List_List*dscs=_tmp26D;struct Cyc_List_List*workr=_tmp26E;
# 1292
if((pats == 0 || objs == 0)|| dscs == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp271=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C7;_tmp3C7.tag=0,({struct _fat_ptr _tmp556=({const char*_tmp272="tcpat:and_match: malformed work frame";_tag_fat(_tmp272,sizeof(char),38U);});_tmp3C7.f1=_tmp556;});_tmp3C7;});void*_tmp270[1];_tmp270[0]=& _tmp271;({int(*_tmp557)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp557(_tag_fat(_tmp270,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp273=pats;struct Cyc_List_List*_stmttmp26=_tmp273;struct Cyc_List_List*_tmp274=_stmttmp26;void*_tmp276;void*_tmp275;_tmp275=(void*)_tmp274->hd;_tmp276=_tmp274->tl;{void*pat1=_tmp275;struct Cyc_List_List*patr=_tmp276;
struct Cyc_List_List*_tmp277=objs;struct Cyc_List_List*_stmttmp27=_tmp277;struct Cyc_List_List*_tmp278=_stmttmp27;void*_tmp27A;void*_tmp279;_tmp279=(struct Cyc_List_List*)_tmp278->hd;_tmp27A=_tmp278->tl;{struct Cyc_List_List*obj1=_tmp279;struct Cyc_List_List*objr=_tmp27A;
struct Cyc_List_List*_tmp27B=dscs;struct Cyc_List_List*_stmttmp28=_tmp27B;struct Cyc_List_List*_tmp27C=_stmttmp28;void*_tmp27E;void*_tmp27D;_tmp27D=(void*)_tmp27C->hd;_tmp27E=_tmp27C->tl;{void*dsc1=_tmp27D;struct Cyc_List_List*dscr=_tmp27E;
struct _tuple21*_tmp27F=({struct _tuple21*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281->f1=patr,_tmp281->f2=objr,_tmp281->f3=dscr;_tmp281;});struct _tuple21*wf=_tmp27F;
return({void*_tmp55D=pat1;struct Cyc_List_List*_tmp55C=obj1;void*_tmp55B=dsc1;struct Cyc_List_List*_tmp55A=ctx;struct Cyc_List_List*_tmp559=({struct Cyc_List_List*_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280->hd=wf,_tmp280->tl=workr;_tmp280;});struct Cyc_Tcpat_Rhs*_tmp558=right_hand_side;Cyc_Tcpat_match(_tmp55D,_tmp55C,_tmp55B,_tmp55A,_tmp559,_tmp558,rules);});}}}}}}};}
# 1303
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp282=dsc;void*_tmp283;if(*((int*)_tmp282)== 1){_tmp283=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp282)->f1;{struct Cyc_Set_Set*ncs=_tmp283;
# 1309
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->tag=1,({struct Cyc_Set_Set*_tmp55E=Cyc_Tcpat_empty_con_set();_tmp286->f1=_tmp55E;});_tmp286;});
struct Cyc_List_List*_tmp284=0;struct Cyc_List_List*res=_tmp284;
{int i=0;for(0;i < pcon->arity;++ i){
res=({struct Cyc_List_List*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->hd=any,_tmp285->tl=res;_tmp285;});}}
return res;}}else{_tmp283=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp282)->f2;{struct Cyc_List_List*dargs=_tmp283;
return dargs;}};}
# 1318
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp287=pw;void*_tmp288;if((_tmp287.where_clause).tag == 2)
return(void*)& Cyc_Tcpat_Dummy_val;else{_tmp288=(_tmp287.pattern).val;{struct Cyc_Absyn_Pat*p=_tmp288;
# 1322
void*_tmp289=p->r;void*_stmttmp29=_tmp289;void*_tmp28A=_stmttmp29;union Cyc_Absyn_AggrInfo _tmp28B;void*_tmp28D;void*_tmp28C;switch(*((int*)_tmp28A)){case 6:
# 1324
 if(i != 0)
({struct Cyc_Int_pa_PrintArg_struct _tmp290=({struct Cyc_Int_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=1,_tmp3C8.f1=(unsigned long)i;_tmp3C8;});void*_tmp28E[1];_tmp28E[0]=& _tmp290;({int(*_tmp560)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp55F=({const char*_tmp28F="get_access on pointer pattern with offset %d\n";_tag_fat(_tmp28F,sizeof(char),46U);});_tmp560(_tmp55F,_tag_fat(_tmp28E,sizeof(void*),1));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291->tag=2,_tmp291->f1=(unsigned)i;_tmp291;});case 8: _tmp28C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp28A)->f1;_tmp28D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp28A)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp28C;struct Cyc_Absyn_Datatypefield*tuf=_tmp28D;
return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->tag=3,_tmp292->f1=tud,_tmp292->f2=tuf,_tmp292->f3=(unsigned)i;_tmp292;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp28A)->f1 != 0){_tmp28B=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp28A)->f1;_tmp28C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp28A)->f3;{union Cyc_Absyn_AggrInfo info=_tmp28B;struct Cyc_List_List*dlps=_tmp28C;
# 1330
struct Cyc_Absyn_Aggrdecl*_tmp293=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp293;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
struct Cyc_List_List*_tmp294=(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(dlps))->hd)).f1;struct Cyc_List_List*_stmttmp2A=_tmp294;struct Cyc_List_List*_tmp295=_stmttmp2A;void*_tmp296;if(_tmp295 != 0){if(*((int*)((struct Cyc_List_List*)_tmp295)->hd)== 1){if(((struct Cyc_List_List*)_tmp295)->tl == 0){_tmp296=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp295->hd)->f1;{struct _fat_ptr*f=_tmp296;
# 1334
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->tag=4,_tmp297->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmp297->f2=f;_tmp297;});}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13:
# 1336
({struct Cyc_String_pa_PrintArg_struct _tmp29A=({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0,({struct _fat_ptr _tmp561=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(p));_tmp3C9.f1=_tmp561;});_tmp3C9;});void*_tmp298[1];_tmp298[0]=& _tmp29A;({int(*_tmp563)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp562=({const char*_tmp299="get_access on bad aggr pattern: %s";_tag_fat(_tmp299,sizeof(char),35U);});_tmp563(_tmp562,_tag_fat(_tmp298,sizeof(void*),1));});});};}{
# 1338
struct Cyc_List_List*_tmp29B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp29B;
int _tmp29C=i;int orig_i=_tmp29C;
for(1;i != 0;-- i){fields=((struct Cyc_List_List*)_check_null(fields))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->tag=4,_tmp29D->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmp29D->f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fields))->hd)->name;_tmp29D;});}}}else{goto _LLE;}default: _LLE:
({struct Cyc_String_pa_PrintArg_struct _tmp2A0=({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0,({struct _fat_ptr _tmp564=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(p));_tmp3CA.f1=_tmp564;});_tmp3CA;});void*_tmp29E[1];_tmp29E[0]=& _tmp2A0;({int(*_tmp566)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp565=({const char*_tmp29F="get_access on bad pattern: %s";_tag_fat(_tmp29F,sizeof(char),30U);});_tmp566(_tmp565,_tag_fat(_tmp29E,sizeof(void*),1));});});};}};}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1348
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple24*env,int i){
struct _tuple24*_tmp2A1=env;void*_tmp2A3;void*_tmp2A2;_tmp2A2=_tmp2A1->f1;_tmp2A3=_tmp2A1->f2;{struct Cyc_List_List*obj=_tmp2A2;struct Cyc_Tcpat_Con_s*pcon=_tmp2A3;
void*acc=Cyc_Tcpat_get_access(pcon->orig_pat,i);
return({struct Cyc_List_List*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));({struct Cyc_Tcpat_PathNode*_tmp567=({struct Cyc_Tcpat_PathNode*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4->orig_pat=pcon->orig_pat,_tmp2A4->access=acc;_tmp2A4;});_tmp2A5->hd=_tmp567;}),_tmp2A5->tl=obj;_tmp2A5;});}}
# 1353
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple24 _tmp2A6=({struct _tuple24 _tmp3CB;_tmp3CB.f1=obj,_tmp3CB.f2=pcon;_tmp3CB;});struct _tuple24 env=_tmp2A6;
return({(struct Cyc_List_List*(*)(int,struct Cyc_List_List*(*)(struct _tuple24*,int),struct _tuple24*))Cyc_List_tabulate_c;})(pcon->arity,Cyc_Tcpat_getoarg,& env);}
# 1361
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1364
void*_tmp2A7=p;void*_tmp2A9;void*_tmp2A8;if(*((int*)_tmp2A7)== 0)
return({struct Cyc_List_List*_tmp56A=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp569=work;struct Cyc_Tcpat_Rhs*_tmp568=right_hand_side;Cyc_Tcpat_and_match(_tmp56A,_tmp569,_tmp568,rules);});else{_tmp2A8=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2A7)->f1;_tmp2A9=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2A7)->f2;{struct Cyc_Tcpat_Con_s*pcon=_tmp2A8;struct Cyc_List_List*pargs=_tmp2A9;
# 1367
enum Cyc_Tcpat_Answer _tmp2AA=Cyc_Tcpat_static_match(pcon,dsc);enum Cyc_Tcpat_Answer _stmttmp2B=_tmp2AA;enum Cyc_Tcpat_Answer _tmp2AB=_stmttmp2B;switch((int)_tmp2AB){case Cyc_Tcpat_Yes:  {
# 1369
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));({struct _tuple22*_tmp56B=({struct _tuple22*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE->f1=pcon,_tmp2AE->f2=0;_tmp2AE;});_tmp2AF->hd=_tmp56B;}),_tmp2AF->tl=ctx;_tmp2AF;});
struct _tuple21*work_frame=({struct _tuple21*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->f1=pargs,({struct Cyc_List_List*_tmp56D=Cyc_Tcpat_getoargs(pcon,obj);_tmp2AD->f2=_tmp56D;}),({
struct Cyc_List_List*_tmp56C=Cyc_Tcpat_getdargs(pcon,dsc);_tmp2AD->f3=_tmp56C;});_tmp2AD;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->hd=work_frame,_tmp2AC->tl=work;_tmp2AC;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No:
# 1375
 return({void*_tmp56E=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp56E,rules);});default:  {
# 1377
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));({struct _tuple22*_tmp56F=({struct _tuple22*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->f1=pcon,_tmp2B4->f2=0;_tmp2B4;});_tmp2B5->hd=_tmp56F;}),_tmp2B5->tl=ctx;_tmp2B5;});
struct _tuple21*work_frame=({struct _tuple21*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->f1=pargs,({struct Cyc_List_List*_tmp571=Cyc_Tcpat_getoargs(pcon,obj);_tmp2B3->f2=_tmp571;}),({
struct Cyc_List_List*_tmp570=Cyc_Tcpat_getdargs(pcon,dsc);_tmp2B3->f3=_tmp570;});_tmp2B3;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->hd=work_frame,_tmp2B2->tl=work;_tmp2B2;});
void*_tmp2B0=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);void*s=_tmp2B0;
void*_tmp2B1=({void*_tmp574=({struct Cyc_List_List*_tmp573=ctx;void*_tmp572=Cyc_Tcpat_add_neg(dsc,pcon);Cyc_Tcpat_build_desc(_tmp573,_tmp572,work);});Cyc_Tcpat_or_match(_tmp574,rules);});void*f=_tmp2B1;
return Cyc_Tcpat_ifeq(obj,pcon,s,f);}};}};}
# 1393
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1398
void*_tmp2B6=d;void*_tmp2B8;void*_tmp2B7;switch(*((int*)_tmp2B6)){case 0: _tmp2B7=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2B6)->f1;{void*td=_tmp2B7;
# 1400
if(!(*exhaust_done)){not_exhaust(env1,td);*exhaust_done=1;}
goto _LL0;}case 1: _tmp2B7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2B6)->f1;{struct Cyc_Tcpat_Rhs*r=_tmp2B7;
rhs_appears(env2,r);goto _LL0;}default: _tmp2B7=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2B6)->f2;_tmp2B8=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2B6)->f3;{struct Cyc_List_List*cases=_tmp2B7;void*def=_tmp2B8;
# 1404
for(1;cases != 0;cases=cases->tl){
struct _tuple0 _tmp2B9=*((struct _tuple0*)cases->hd);struct _tuple0 _stmttmp2C=_tmp2B9;struct _tuple0 _tmp2BA=_stmttmp2C;void*_tmp2BB;_tmp2BB=_tmp2BA.f2;{void*d=_tmp2BB;
Cyc_Tcpat_check_exhaust_overlap(d,not_exhaust,env1,rhs_appears,env2,exhaust_done);}}
# 1409
Cyc_Tcpat_check_exhaust_overlap(def,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1411
goto _LL0;}}_LL0:;}
# 1419
static struct _tuple23*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1->used=0,_tmp2C1->pat_loc=(swc->pattern)->loc,_tmp2C1->rhs=swc->body;_tmp2C1;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp2BC=({union Cyc_Tcpat_PatOrWhere _tmp3CC;(_tmp3CC.where_clause).tag=2U,(_tmp3CC.where_clause).val=swc->where_clause;_tmp3CC;});union Cyc_Tcpat_PatOrWhere w=_tmp2BC;
sp=({struct Cyc_List_List*_tmp577=({void*_tmp2BD[2];_tmp2BD[0]=sp0,({void*_tmp575=Cyc_Tcpat_int_pat(*ctr,w);_tmp2BD[1]=_tmp575;});({struct Cyc_List_List*(*_tmp576)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp576(_tag_fat(_tmp2BD,sizeof(void*),2));});});Cyc_Tcpat_tuple_pat(_tmp577,w);});
*ctr=*ctr + 1;}else{
# 1428
sp=({struct Cyc_List_List*_tmp57A=({void*_tmp2BE[2];_tmp2BE[0]=sp0,({void*_tmp578=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->tag=0;_tmp2BF;});_tmp2BE[1]=_tmp578;});({struct Cyc_List_List*(*_tmp579)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp579(_tag_fat(_tmp2BE,sizeof(void*),2));});});Cyc_Tcpat_tuple_pat(_tmp57A,({union Cyc_Tcpat_PatOrWhere _tmp3CD;(_tmp3CD.where_clause).tag=2U,(_tmp3CD.where_clause).val=0;_tmp3CD;}));});}
return({struct _tuple23*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->f1=sp,_tmp2C0->f2=rhs;_tmp2C0;});}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1434
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1436
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp2C2=d;void*_tmp2C4;void*_tmp2C3;if(*((int*)_tmp2C2)== 0){_tmp2C3=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2C2)->f1;_tmp2C4=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2C2)->f2;{struct Cyc_Tcpat_Con_s*c=_tmp2C3;struct Cyc_List_List*ds=_tmp2C4;
# 1440
union Cyc_Tcpat_Name_value _tmp2C5=c->name;union Cyc_Tcpat_Name_value n=_tmp2C5;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp2C6=c->orig_pat;union Cyc_Tcpat_PatOrWhere _stmttmp2D=_tmp2C6;union Cyc_Tcpat_PatOrWhere _tmp2C7=_stmttmp2D;void*_tmp2C8;if((_tmp2C7.where_clause).tag == 2)
return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(ds))->hd);else{_tmp2C8=(_tmp2C7.pattern).val;{struct Cyc_Absyn_Pat*p2=_tmp2C8;
p=p2;goto _LL5;}}_LL5:;}{
# 1446
void*_tmp2C9=p->r;void*_stmttmp2E=_tmp2C9;void*_tmp2CA=_stmttmp2E;struct _fat_ptr _tmp2CC;char _tmp2CE;int _tmp2CD;void*_tmp2CF;void*_tmp2CB;switch(*((int*)_tmp2CA)){case 0:
 return({const char*_tmp2D0="_";_tag_fat(_tmp2D0,sizeof(char),2U);});case 1: _tmp2CB=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp2CB;
return Cyc_Absynpp_qvar2string(vd->name);}case 3: _tmp2CB=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp2CB;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D3=({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0,({struct _fat_ptr _tmp57B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp3CE.f1=_tmp57B;});_tmp3CE;});void*_tmp2D1[1];_tmp2D1[0]=& _tmp2D3;({struct _fat_ptr _tmp57C=({const char*_tmp2D2="*%s";_tag_fat(_tmp2D2,sizeof(char),4U);});Cyc_aprintf(_tmp57C,_tag_fat(_tmp2D1,sizeof(void*),1));});});}case 4: _tmp2CB=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1;_tmp2CF=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2CA)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp2CB;struct Cyc_Absyn_Vardecl*vd=_tmp2CF;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D6=({struct Cyc_String_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=0,({struct _fat_ptr _tmp57D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp3D0.f1=_tmp57D;});_tmp3D0;});struct Cyc_String_pa_PrintArg_struct _tmp2D7=({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0,_tmp3CF.f1=(struct _fat_ptr)((struct _fat_ptr)*tv->name);_tmp3CF;});void*_tmp2D4[2];_tmp2D4[0]=& _tmp2D6,_tmp2D4[1]=& _tmp2D7;({struct _fat_ptr _tmp57E=({const char*_tmp2D5="%s<`%s>";_tag_fat(_tmp2D5,sizeof(char),8U);});Cyc_aprintf(_tmp57E,_tag_fat(_tmp2D4,sizeof(void*),2));});});}case 5:
# 1452
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DA=({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0,({struct _fat_ptr _tmp57F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp3D1.f1=_tmp57F;});_tmp3D1;});void*_tmp2D8[1];_tmp2D8[0]=& _tmp2DA;({struct _fat_ptr _tmp580=({const char*_tmp2D9="$(%s)";_tag_fat(_tmp2D9,sizeof(char),6U);});Cyc_aprintf(_tmp580,_tag_fat(_tmp2D8,sizeof(void*),1));});});case 6:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DD=({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0,({struct _fat_ptr _tmp581=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp3D2.f1=_tmp581;});_tmp3D2;});void*_tmp2DB[1];_tmp2DB[0]=& _tmp2DD;({struct _fat_ptr _tmp582=({const char*_tmp2DC="&%s";_tag_fat(_tmp2DC,sizeof(char),4U);});Cyc_aprintf(_tmp582,_tag_fat(_tmp2DB,sizeof(void*),1));});});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1)->KnownAggr).tag == 2){_tmp2CB=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1)->KnownAggr).val;_tmp2CF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2CA)->f3;{struct Cyc_Absyn_Aggrdecl*ad=_tmp2CB;struct Cyc_List_List*dlps=_tmp2CF;
# 1455
if((int)ad->kind == 1){
struct Cyc_List_List*_tmp2DE=dlps;void*_tmp2DF;if(_tmp2DE != 0){if(((struct _tuple16*)((struct Cyc_List_List*)_tmp2DE)->hd)->f1 != 0){if(*((int*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp2DE)->hd)->f1)->hd)== 1){_tmp2DF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple16*)_tmp2DE->hd)->f1)->hd)->f1;{struct _fat_ptr*f=_tmp2DF;
# 1458
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E2=({struct Cyc_String_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=0,({struct _fat_ptr _tmp583=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp3D5.f1=_tmp583;});_tmp3D5;});struct Cyc_String_pa_PrintArg_struct _tmp2E3=({struct Cyc_String_pa_PrintArg_struct _tmp3D4;_tmp3D4.tag=0,_tmp3D4.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp3D4;});struct Cyc_String_pa_PrintArg_struct _tmp2E4=({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0,({
struct _fat_ptr _tmp584=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp3D3.f1=_tmp584;});_tmp3D3;});void*_tmp2E0[3];_tmp2E0[0]=& _tmp2E2,_tmp2E0[1]=& _tmp2E3,_tmp2E0[2]=& _tmp2E4;({struct _fat_ptr _tmp585=({const char*_tmp2E1="%s{.%s = %s}";_tag_fat(_tmp2E1,sizeof(char),13U);});Cyc_aprintf(_tmp585,_tag_fat(_tmp2E0,sizeof(void*),3));});});}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E:
 goto _LL2B;}_LL2B:;}
# 1462
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E7=({struct Cyc_String_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=0,({struct _fat_ptr _tmp586=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp3D7.f1=_tmp586;});_tmp3D7;});struct Cyc_String_pa_PrintArg_struct _tmp2E8=({struct Cyc_String_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=0,({struct _fat_ptr _tmp587=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp3D6.f1=_tmp587;});_tmp3D6;});void*_tmp2E5[2];_tmp2E5[0]=& _tmp2E7,_tmp2E5[1]=& _tmp2E8;({struct _fat_ptr _tmp588=({const char*_tmp2E6="%s{%s}";_tag_fat(_tmp2E6,sizeof(char),7U);});Cyc_aprintf(_tmp588,_tag_fat(_tmp2E5,sizeof(void*),2));});});}}else{goto _LL29;}}else{goto _LL29;}case 8: _tmp2CB=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2CA)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_tmp2CB;
# 1464
if(ds == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EB=({struct Cyc_String_pa_PrintArg_struct _tmp3D8;_tmp3D8.tag=0,({struct _fat_ptr _tmp589=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp3D8.f1=_tmp589;});_tmp3D8;});void*_tmp2E9[1];_tmp2E9[0]=& _tmp2EB;({struct _fat_ptr _tmp58A=({const char*_tmp2EA="%s";_tag_fat(_tmp2EA,sizeof(char),3U);});Cyc_aprintf(_tmp58A,_tag_fat(_tmp2E9,sizeof(void*),1));});});else{
# 1467
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EE=({struct Cyc_String_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=0,({struct _fat_ptr _tmp58B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp3DA.f1=_tmp58B;});_tmp3DA;});struct Cyc_String_pa_PrintArg_struct _tmp2EF=({struct Cyc_String_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=0,({struct _fat_ptr _tmp58C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp3D9.f1=_tmp58C;});_tmp3D9;});void*_tmp2EC[2];_tmp2EC[0]=& _tmp2EE,_tmp2EC[1]=& _tmp2EF;({struct _fat_ptr _tmp58D=({const char*_tmp2ED="%s(%s)";_tag_fat(_tmp2ED,sizeof(char),7U);});Cyc_aprintf(_tmp58D,_tag_fat(_tmp2EC,sizeof(void*),2));});});}}case 9:
 return({const char*_tmp2F0="NULL";_tag_fat(_tmp2F0,sizeof(char),5U);});case 10: _tmp2CD=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2CA)->f2;{int i=_tmp2CD;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2F3=({struct Cyc_Int_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=1,_tmp3DB.f1=(unsigned long)i;_tmp3DB;});void*_tmp2F1[1];_tmp2F1[0]=& _tmp2F3;({struct _fat_ptr _tmp58E=({const char*_tmp2F2="%d";_tag_fat(_tmp2F2,sizeof(char),3U);});Cyc_aprintf(_tmp58E,_tag_fat(_tmp2F1,sizeof(void*),1));});});}case 11: _tmp2CE=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1;{char c=_tmp2CE;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2F6=({struct Cyc_Int_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=1,_tmp3DC.f1=(unsigned long)((int)c);_tmp3DC;});void*_tmp2F4[1];_tmp2F4[0]=& _tmp2F6;({struct _fat_ptr _tmp58F=({const char*_tmp2F5="%d";_tag_fat(_tmp2F5,sizeof(char),3U);});Cyc_aprintf(_tmp58F,_tag_fat(_tmp2F4,sizeof(void*),1));});});}case 12: _tmp2CC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1;_tmp2CD=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2CA)->f2;{struct _fat_ptr f=_tmp2CC;int i=_tmp2CD;
return f;}case 13: _tmp2CB=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2CA)->f2;{struct Cyc_Absyn_Enumfield*ef=_tmp2CB;
_tmp2CB=ef;goto _LL26;}case 14: _tmp2CB=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2CA)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp2CB;
return Cyc_Absynpp_qvar2string(ef->name);}case 17: _tmp2CB=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2CA)->f1;{struct Cyc_Absyn_Exp*e=_tmp2CB;
return Cyc_Absynpp_exp2string(e);}default: _LL29:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}}}else{_tmp2C3=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2C2)->f1;{struct Cyc_Set_Set*s=_tmp2C3;
# 1478
if(({(int(*)(struct Cyc_Set_Set*))Cyc_Set_is_empty;})(s))return({const char*_tmp2F7="_";_tag_fat(_tmp2F7,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*_tmp2F8=({(struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*))Cyc_Set_choose;})(s);struct Cyc_Tcpat_Con_s*c=_tmp2F8;
# 1482
if((int)(((c->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp2F9=({union Cyc_Tcpat_PatOrWhere _tmp314=c->orig_pat;if((_tmp314.pattern).tag != 1)_throw_match();(_tmp314.pattern).val;});struct Cyc_Absyn_Pat*orig_pat=_tmp2F9;
void*_tmp2FA=Cyc_Absyn_compress((void*)_check_null(orig_pat->topt));void*_stmttmp2F=_tmp2FA;void*_tmp2FB=_stmttmp2F;void*_tmp2FC;switch(*((int*)_tmp2FB)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2FB)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2FB)->f1)->f2 == Cyc_Absyn_Char_sz){
# 1487
{int i=0;for(0;i < 256;++ i){
if(!({int(*_tmp591)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp590=s;_tmp591(_tmp590,Cyc_Tcpat_char_con((char)i,orig_pat));}))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2FF=({struct Cyc_Int_pa_PrintArg_struct _tmp3DD;_tmp3DD.tag=1,_tmp3DD.f1=(unsigned long)i;_tmp3DD;});void*_tmp2FD[1];_tmp2FD[0]=& _tmp2FF;({struct _fat_ptr _tmp592=({const char*_tmp2FE="%d";_tag_fat(_tmp2FE,sizeof(char),3U);});Cyc_aprintf(_tmp592,_tag_fat(_tmp2FD,sizeof(void*),1));});});}}
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{
# 1493
{unsigned i=0U;for(0;i < 4294967295U;++ i){
if(!({int(*_tmp594)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp593=s;_tmp594(_tmp593,Cyc_Tcpat_int_con((int)i,c->orig_pat));}))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp302=({struct Cyc_Int_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=1,_tmp3DE.f1=(unsigned long)((int)i);_tmp3DE;});void*_tmp300[1];_tmp300[0]=& _tmp302;({struct _fat_ptr _tmp595=({const char*_tmp301="%d";_tag_fat(_tmp301,sizeof(char),3U);});Cyc_aprintf(_tmp595,_tag_fat(_tmp300,sizeof(void*),1));});});}}
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 18: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2FB)->f1)->f1).KnownDatatype).tag == 2){_tmp2FC=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2FB)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl*tud=_tmp2FC;
# 1502
if(tud->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp305=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp305;
int span=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(fields);
for(1;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)fields->hd)->name).f2;
struct Cyc_List_List*_tmp306=((struct Cyc_Absyn_Datatypefield*)fields->hd)->typs;struct Cyc_List_List*args=_tmp306;
if(!({int(*_tmp598)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp597=s;_tmp598(_tmp597,({struct Cyc_Tcpat_Con_s*_tmp307=_cycalloc(sizeof(*_tmp307));({union Cyc_Tcpat_Name_value _tmp596=Cyc_Tcpat_Name_v(n);_tmp307->name=_tmp596;}),_tmp307->arity=0,_tmp307->span=0,_tmp307->orig_pat=c->orig_pat;_tmp307;}));}))
return({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(args)== 0?n:(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30A=({struct Cyc_String_pa_PrintArg_struct _tmp3DF;_tmp3DF.tag=0,_tmp3DF.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp3DF;});void*_tmp308[1];_tmp308[0]=& _tmp30A;({struct _fat_ptr _tmp599=({const char*_tmp309="%s(...)";_tag_fat(_tmp309,sizeof(char),8U);});Cyc_aprintf(_tmp599,_tag_fat(_tmp308,sizeof(void*),1));});});}
# 1511
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL3B;}case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2FB)->f1)->f1).KnownAggr).tag == 2){_tmp2FC=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2FB)->f1)->f1).KnownAggr).val;if((int)((struct Cyc_Absyn_Aggrdecl*)_tmp2FC)->kind == 1){struct Cyc_Absyn_Aggrdecl*ad=_tmp2FC;
# 1513
struct Cyc_List_List*_tmp30B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp30B;
int span=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(fields);
struct _tuple2*_tmp30C=ad->name;struct _tuple2*_stmttmp30=_tmp30C;struct _tuple2*_tmp30D=_stmttmp30;struct _fat_ptr _tmp30E;_tmp30E=*_tmp30D->f2;{struct _fat_ptr union_name=_tmp30E;
for(1;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;
if(!({int(*_tmp59C)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp59B=s;_tmp59C(_tmp59B,({struct Cyc_Tcpat_Con_s*_tmp30F=_cycalloc(sizeof(*_tmp30F));({union Cyc_Tcpat_Name_value _tmp59A=Cyc_Tcpat_Name_v(n);_tmp30F->name=_tmp59A;}),_tmp30F->arity=0,_tmp30F->span=0,_tmp30F->orig_pat=c->orig_pat;_tmp30F;}));}))
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp312=({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0,_tmp3E1.f1=(struct _fat_ptr)((struct _fat_ptr)union_name);_tmp3E1;});struct Cyc_String_pa_PrintArg_struct _tmp313=({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0,_tmp3E0.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp3E0;});void*_tmp310[2];_tmp310[0]=& _tmp312,_tmp310[1]=& _tmp313;({struct _fat_ptr _tmp59D=({const char*_tmp311="%s{.%s = _}";_tag_fat(_tmp311,sizeof(char),12U);});Cyc_aprintf(_tmp59D,_tag_fat(_tmp310,sizeof(void*),2));});});}
# 1521
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL3B;}}else{goto _LL3B;}default: goto _LL3B;}case 3: _tmp2FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FB)->f1).ptr_atts).nullable;{void*n=_tmp2FC;
# 1498
if(Cyc_Tcutil_force_type2bool(0,n)&& !({int(*_tmp59F)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp59E=s;_tmp59F(_tmp59E,Cyc_Tcpat_null_con(orig_pat));}))
return({const char*_tmp303="NULL";_tag_fat(_tmp303,sizeof(char),5U);});
return({const char*_tmp304="&_";_tag_fat(_tmp304,sizeof(char),3U);});}default: _LL3B:
# 1522
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}}}};}
# 1526
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _fat_ptr*_tmp315=_cycalloc(sizeof(*_tmp315));({struct _fat_ptr _tmp5A0=Cyc_Tcpat_desc2string(d);*_tmp315=_tmp5A0;});_tmp315;});}
# 1529
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp316=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_desc2stringptr,ds);struct Cyc_List_List*ss=_tmp316;
struct _fat_ptr*comma=({struct _fat_ptr*_tmp31A=_cycalloc(sizeof(*_tmp31A));({struct _fat_ptr _tmp5A1=({const char*_tmp319=",";_tag_fat(_tmp319,sizeof(char),2U);});*_tmp31A=_tmp5A1;});_tmp31A;});
{struct Cyc_List_List*_tmp317=ss;struct Cyc_List_List*x=_tmp317;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
if(x->tl != 0){
({struct Cyc_List_List*_tmp5A2=({struct Cyc_List_List*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->hd=comma,_tmp318->tl=x->tl;_tmp318;});x->tl=_tmp5A2;});
x=x->tl;}}}
# 1537
return(struct _fat_ptr)Cyc_strconcat_l(ss);}
# 1540
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){
struct _handler_cons _tmp31B;_push_handler(& _tmp31B);{int _tmp31D=0;if(setjmp(_tmp31B.handler))_tmp31D=1;if(!_tmp31D){
{struct _fat_ptr _tmp31E=Cyc_Tcpat_desc2string(desc);struct _fat_ptr s=_tmp31E;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp320=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E3;_tmp3E3.tag=0,({struct _fat_ptr _tmp5A3=({const char*_tmp322="patterns may not be exhaustive.\n\tmissing case for ";_tag_fat(_tmp322,sizeof(char),51U);});_tmp3E3.f1=_tmp5A3;});_tmp3E3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp321=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E2;_tmp3E2.tag=0,_tmp3E2.f1=s;_tmp3E2;});void*_tmp31F[2];_tmp31F[0]=& _tmp320,_tmp31F[1]=& _tmp321;({unsigned _tmp5A4=loc;Cyc_Warn_err2(_tmp5A4,_tag_fat(_tmp31F,sizeof(void*),2));});});}
# 1542
;_pop_handler();}else{void*_tmp31C=(void*)Cyc_Core_get_exn_thrown();void*_tmp323=_tmp31C;void*_tmp324;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp323)->tag == Cyc_Tcpat_Desc2string){
# 1545
({struct Cyc_Warn_String_Warn_Warg_struct _tmp326=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E4;_tmp3E4.tag=0,({struct _fat_ptr _tmp5A5=({const char*_tmp327="patterns may not be exhaustive.";_tag_fat(_tmp327,sizeof(char),32U);});_tmp3E4.f1=_tmp5A5;});_tmp3E4;});void*_tmp325[1];_tmp325[0]=& _tmp326;({unsigned _tmp5A6=loc;Cyc_Warn_err2(_tmp5A6,_tag_fat(_tmp325,sizeof(void*),1));});});
goto _LL0;}else{_tmp324=_tmp323;{void*exn=_tmp324;(int)_rethrow(exn);}}_LL0:;}}}
# 1549
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1553
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1559
int _tmp328=0;int where_ctr=_tmp328;
int*_tmp329=& where_ctr;int*env=_tmp329;
struct Cyc_List_List*_tmp32A=({(struct Cyc_List_List*(*)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcpat_get_match,env,swcs);struct Cyc_List_List*match_rules=_tmp32A;
void*_tmp32B=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp32B;
*dopt=dec_tree;{
# 1565
int _tmp32C=0;int ex_done=_tmp32C;
({(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;})(dec_tree,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1568
for(1;match_rules != 0;match_rules=match_rules->tl){
struct _tuple23*_tmp32D=(struct _tuple23*)match_rules->hd;struct _tuple23*_stmttmp31=_tmp32D;struct _tuple23*_tmp32E=_stmttmp31;unsigned _tmp330;int _tmp32F;_tmp32F=(_tmp32E->f2)->used;_tmp330=(_tmp32E->f2)->pat_loc;{int b=_tmp32F;unsigned loc2=_tmp330;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp332=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E5;_tmp3E5.tag=0,({struct _fat_ptr _tmp5A7=({const char*_tmp333="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp333,sizeof(char),74U);});_tmp3E5.f1=_tmp5A7;});_tmp3E5;});void*_tmp331[1];_tmp331[0]=& _tmp332;({unsigned _tmp5A8=loc2;Cyc_Warn_err2(_tmp5A8,_tag_fat(_tmp331,sizeof(void*),1));});});
# 1573
break;}}}}}
# 1582
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple15*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp334;_push_handler(& _tmp334);{int _tmp336=0;if(setjmp(_tmp334.handler))_tmp336=1;if(!_tmp336){
{struct _fat_ptr _tmp337=Cyc_Tcpat_desc2string(desc);struct _fat_ptr s=_tmp337;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp339=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E7;_tmp3E7.tag=0,({struct _fat_ptr _tmp5A9=({const char*_tmp33B="pattern not exhaustive.\n\tmissing case for ";_tag_fat(_tmp33B,sizeof(char),43U);});_tmp3E7.f1=_tmp5A9;});_tmp3E7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp33A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E6;_tmp3E6.tag=0,_tmp3E6.f1=s;_tmp3E6;});void*_tmp338[2];_tmp338[0]=& _tmp339,_tmp338[1]=& _tmp33A;({unsigned _tmp5AA=(*pr).f1;Cyc_Warn_warn2(_tmp5AA,_tag_fat(_tmp338,sizeof(void*),2));});});}
# 1585
;_pop_handler();}else{void*_tmp335=(void*)Cyc_Core_get_exn_thrown();void*_tmp33C=_tmp335;void*_tmp33D;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp33C)->tag == Cyc_Tcpat_Desc2string){
# 1588
({struct Cyc_Warn_String_Warn_Warg_struct _tmp33F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E8;_tmp3E8.tag=0,({struct _fat_ptr _tmp5AB=({const char*_tmp340="pattern not exhaustive.";_tag_fat(_tmp340,sizeof(char),24U);});_tmp3E8.f1=_tmp5AB;});_tmp3E8;});void*_tmp33E[1];_tmp33E[0]=& _tmp33F;({unsigned _tmp5AC=(*pr).f1;Cyc_Warn_warn2(_tmp5AC,_tag_fat(_tmp33E,sizeof(void*),1));});});
goto _LL0;}else{_tmp33D=_tmp33C;{void*exn=_tmp33D;(int)_rethrow(exn);}}_LL0:;}}}}
# 1592
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1596
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->used=0,_tmp347->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_tmp5AD=Cyc_Absyn_skip_stmt(0U);_tmp347->rhs=_tmp5AD;});_tmp347;});
struct Cyc_List_List*_tmp341=({struct Cyc_List_List*_tmp346=_cycalloc(sizeof(*_tmp346));({struct _tuple23*_tmp5AF=({struct _tuple23*_tmp345=_cycalloc(sizeof(*_tmp345));({void*_tmp5AE=Cyc_Tcpat_compile_pat(p);_tmp345->f1=_tmp5AE;}),_tmp345->f2=rhs;_tmp345;});_tmp346->hd=_tmp5AF;}),_tmp346->tl=0;_tmp346;});struct Cyc_List_List*match_rules=_tmp341;
void*_tmp342=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp342;
struct _tuple15 _tmp343=({struct _tuple15 _tmp3E9;_tmp3E9.f1=loc,_tmp3E9.f2=1;_tmp3E9;});struct _tuple15 exhaust_env=_tmp343;
int _tmp344=0;int ex_done=_tmp344;
({(void(*)(void*,void(*)(struct _tuple15*,void*),struct _tuple15*,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;})(dec_tree,Cyc_Tcpat_not_exhaust_warn,& exhaust_env,Cyc_Tcpat_dummy_fn,0,& ex_done);
# 1604
*dopt=dec_tree;
return exhaust_env.f2;}
# 1612
static struct _tuple23*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1616
if(swc->where_clause != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp349=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EA;_tmp3EA.tag=0,({struct _fat_ptr _tmp5B0=({const char*_tmp34A="&&-clauses not supported in exception handlers.";_tag_fat(_tmp34A,sizeof(char),48U);});_tmp3EA.f1=_tmp5B0;});_tmp3EA;});void*_tmp348[1];_tmp348[0]=& _tmp349;({unsigned _tmp5B1=((struct Cyc_Absyn_Exp*)_check_null(swc->where_clause))->loc;Cyc_Warn_err2(_tmp5B1,_tag_fat(_tmp348,sizeof(void*),1));});});{
# 1619
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C->used=0,_tmp34C->pat_loc=(swc->pattern)->loc,_tmp34C->rhs=swc->body;_tmp34C;});
return({struct _tuple23*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->f1=sp0,_tmp34B->f2=rhs;_tmp34B;});}}
# 1622
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1625
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1629
struct Cyc_List_List*_tmp34D=({(struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_get_match2,swcs);struct Cyc_List_List*match_rules=_tmp34D;
void*_tmp34E=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp34E;
*dopt=dec_tree;{
int _tmp34F=0;int ex_done=_tmp34F;
({(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;})(dec_tree,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1635
for(1;match_rules != 0;match_rules=match_rules->tl){
# 1637
if(match_rules->tl == 0)break;{
struct _tuple23*_tmp350=(struct _tuple23*)match_rules->hd;struct _tuple23*_stmttmp32=_tmp350;struct _tuple23*_tmp351=_stmttmp32;unsigned _tmp353;int _tmp352;_tmp352=(_tmp351->f2)->used;_tmp353=(_tmp351->f2)->pat_loc;{int b=_tmp352;unsigned loc2=_tmp353;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp355=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EB;_tmp3EB.tag=0,({struct _fat_ptr _tmp5B2=({const char*_tmp356="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp356,sizeof(char),74U);});_tmp3EB.f1=_tmp5B2;});_tmp3EB;});void*_tmp354[1];_tmp354[0]=& _tmp355;({unsigned _tmp5B3=loc2;Cyc_Warn_err2(_tmp5B3,_tag_fat(_tmp354,sizeof(void*),1));});});
break;}}}}}}
# 1646
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp357=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;struct Cyc_List_List*l=_tmp357;for(0;l != 0;l=l->tl){
if((*((struct _tuple17*)l->hd)).f1 != 0)
return 1;}}
return 0;}

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
# 153 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 174
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 179
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 184
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 288
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 301
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 308
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 410 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 480
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 673 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 859
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 868
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 875
void*Cyc_Absyn_compress(void*);
# 889
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 891
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 894
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 896
extern void*Cyc_Absyn_sint_type;
# 899
void*Cyc_Absyn_gen_float_type(unsigned);
# 901
extern void*Cyc_Absyn_unique_rgn_type;
# 903
extern void*Cyc_Absyn_empty_effect;
# 905
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 907
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_enum_type(struct _tuple2*,struct Cyc_Absyn_Enumdecl*);
# 936
void*Cyc_Absyn_bounds_one (void);
# 938
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 961
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 969
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1039
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1126
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
void*_tmp0=p->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;int _tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List**_tmp4;struct Cyc_Absyn_Aggrdecl**_tmp3;int _tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;switch(*((int*)_tmp1)){case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2: {struct Cyc_List_List*exist_ts=_tmp7;struct Cyc_List_List*dps=_tmp8;int dots=_tmp9;
# 52
if(topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A9;_tmp3A9.tag=0,({struct _fat_ptr _tmp43D=({const char*_tmpC="cannot determine pattern type";_tag_fat(_tmpC,sizeof(char),30U);});_tmp3A9.f1=_tmp43D;});_tmp3A9;});void*_tmpA[1];_tmpA[0]=& _tmpB;({unsigned _tmp43E=p->loc;Cyc_Warn_err2(_tmp43E,_tag_fat(_tmpA,sizeof(void*),1));});});else{
# 55
void*_tmpD=Cyc_Absyn_compress(*topt);void*_stmttmp1=_tmpD;void*_tmpE=_stmttmp1;union Cyc_Absyn_AggrInfo _tmpF;if(*((int*)_tmpE)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->f1)== 20){_LL10: _tmpF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->f1)->f1;_LL11: {union Cyc_Absyn_AggrInfo ainfo=_tmpF;
# 57
({void*_tmp440=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=7,({union Cyc_Absyn_AggrInfo*_tmp43F=({union Cyc_Absyn_AggrInfo*_tmp10=_cycalloc(sizeof(*_tmp10));*_tmp10=ainfo;_tmp10;});_tmp11->f1=_tmp43F;}),_tmp11->f2=exist_ts,_tmp11->f3=dps,_tmp11->f4=dots;_tmp11;});p->r=_tmp440;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}}else{goto _LL12;}}else{_LL12: _LL13:
# 61
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AB;_tmp3AB.tag=0,({struct _fat_ptr _tmp441=({const char*_tmp15="pattern expects aggregate type instead of ";_tag_fat(_tmp15,sizeof(char),43U);});_tmp3AB.f1=_tmp441;});_tmp3AB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3AA;_tmp3AA.tag=2,_tmp3AA.f1=(void*)*topt;_tmp3AA;});void*_tmp12[2];_tmp12[0]=& _tmp13,_tmp12[1]=& _tmp14;({unsigned _tmp442=p->loc;Cyc_Warn_err2(_tmp442,_tag_fat(_tmp12,sizeof(void*),2));});});
goto _LLF;}_LLF:;}
# 64
return;}}else{if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).tag == 2){_LL3: _tmp3=((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).val;_tmp4=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp3;struct Cyc_List_List**exist_ts=_tmp4;struct Cyc_List_List*dps=_tmp5;int dots=_tmp6;
# 67
struct Cyc_Absyn_Aggrdecl*_tmp16=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp16;
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AD;_tmp3AD.tag=0,({struct _fat_ptr _tmp443=({const char*_tmp1A="can't destructure an abstract";_tag_fat(_tmp1A,sizeof(char),30U);});_tmp3AD.f1=_tmp443;});_tmp3AD;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp19=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3AC;_tmp3AC.tag=6,_tmp3AC.f1=ad;_tmp3AC;});void*_tmp17[2];_tmp17[0]=& _tmp18,_tmp17[1]=& _tmp19;({unsigned _tmp444=p->loc;Cyc_Warn_err2(_tmp444,_tag_fat(_tmp17,sizeof(void*),2));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 73
int more_exists=({int _tmp445=((int(*)(struct Cyc_List_List*))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp445 - ((int(*)(struct Cyc_List_List*))Cyc_List_length)(*exist_ts);});
if(more_exists < 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AE;_tmp3AE.tag=0,({struct _fat_ptr _tmp446=({const char*_tmp1D="too many existential type variables in pattern";_tag_fat(_tmp1D,sizeof(char),47U);});_tmp3AE.f1=_tmp446;});_tmp3AE;});void*_tmp1B[1];_tmp1B[0]=& _tmp1C;({unsigned _tmp447=p->loc;Cyc_Warn_err2(_tmp447,_tag_fat(_tmp1B,sizeof(void*),1));});});{
struct Cyc_List_List**_tmp1E=exist_ts;struct Cyc_List_List**ts=_tmp1E;
{int n=((int(*)(struct Cyc_List_List*))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);for(0;n != 0;-- n){
ts=&((struct Cyc_List_List*)_check_null(*ts))->tl;}}
*ts=0;}}else{
if(more_exists > 0){
# 82
struct Cyc_List_List*_tmp1F=0;struct Cyc_List_List*new_ts=_tmp1F;
for(1;more_exists != 0;-- more_exists){
new_ts=({struct Cyc_List_List*_tmp21=_cycalloc(sizeof(*_tmp21));({struct Cyc_Absyn_Tvar*_tmp448=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=1,_tmp20->f1=0;_tmp20;}));_tmp21->hd=_tmp448;}),_tmp21->tl=new_ts;_tmp21;});}
({struct Cyc_List_List*_tmp449=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(*exist_ts,new_ts);*exist_ts=_tmp449;});}}
# 87
return;}}}else{_LLB: _LLC:
# 106
({struct Cyc_Warn_String_Warn_Warg_struct _tmp34=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AF;_tmp3AF.tag=0,({struct _fat_ptr _tmp44A=({const char*_tmp35="resolve_pat unknownAggr";_tag_fat(_tmp35,sizeof(char),24U);});_tmp3AF.f1=_tmp44A;});_tmp3AF;});void*_tmp33[1];_tmp33[0]=& _tmp34;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp33,sizeof(void*),1));});}}case 17: _LL5: _tmp2=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp2;
# 89
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcutil_is_const_exp(e)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B0;_tmp3B0.tag=0,({struct _fat_ptr _tmp44B=({const char*_tmp24="non-constant expression in case pattern";_tag_fat(_tmp24,sizeof(char),40U);});_tmp3B0.f1=_tmp44B;});_tmp3B0;});void*_tmp22[1];_tmp22[0]=& _tmp23;({unsigned _tmp44C=p->loc;Cyc_Warn_err2(_tmp44C,_tag_fat(_tmp22,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 95
struct _tuple15 _tmp25=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple15 _stmttmp2=_tmp25;struct _tuple15 _tmp26=_stmttmp2;int _tmp28;unsigned _tmp27;_LL15: _tmp27=_tmp26.f1;_tmp28=_tmp26.f2;_LL16: {unsigned i=_tmp27;int known=_tmp28;
if(!known){
({struct Cyc_String_pa_PrintArg_struct _tmp2B=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0,({struct _fat_ptr _tmp44D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3B1.f1=_tmp44D;});_tmp3B1;});void*_tmp29[1];_tmp29[0]=& _tmp2B;({unsigned _tmp44F=p->loc;struct _fat_ptr _tmp44E=({const char*_tmp2A="cannot evaluate expression at compile time\nWe suggest using x && x == %s instead";_tag_fat(_tmp2A,sizeof(char),81U);});Cyc_Warn_err(_tmp44F,_tmp44E,_tag_fat(_tmp29,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}
# 101
({void*_tmp450=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=10,_tmp2C->f1=Cyc_Absyn_None,_tmp2C->f2=(int)i;_tmp2C;});p->r=_tmp450;});
return;}}}case 15: _LL7: _LL8:
# 104
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B2;_tmp3B2.tag=0,({struct _fat_ptr _tmp451=({const char*_tmp2F="resolve_pat UnknownId_p";_tag_fat(_tmp2F,sizeof(char),24U);});_tmp3B2.f1=_tmp451;});_tmp3B2;});void*_tmp2D[1];_tmp2D[0]=& _tmp2E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2D,sizeof(void*),1));});case 16: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp31=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B3;_tmp3B3.tag=0,({struct _fat_ptr _tmp452=({const char*_tmp32="resolve_pat UnknownCall_p";_tag_fat(_tmp32,sizeof(char),26U);});_tmp3B3.f1=_tmp452;});_tmp3B3;});void*_tmp30[1];_tmp30[0]=& _tmp31;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp30,sizeof(void*),1));});default: _LLD: _LLE:
# 108
 return;}_LL0:;}
# 112
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){
return({struct Cyc_Absyn_Pat*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->loc=loc,_tmp36->topt=0,_tmp36->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp36;});}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 115
static struct _tuple16*Cyc_Tcpat_wild_dlp(unsigned loc){
return({struct _tuple16*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=0,({struct Cyc_Absyn_Pat*_tmp453=Cyc_Tcpat_wild_pat(loc);_tmp37->f2=_tmp453;});_tmp37;});}
# 119
static struct Cyc_List_List*Cyc_Tcpat_remove_dots(struct Cyc_List_List*earlier,int num_pats,int num_fields,void*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str){
# 121
if(num_pats >= num_fields)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp39=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B6;_tmp3B6.tag=0,({struct _fat_ptr _tmp454=({const char*_tmp3D="unnecessary ... in ";_tag_fat(_tmp3D,sizeof(char),20U);});_tmp3B6.f1=_tmp454;});_tmp3B6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B5;_tmp3B5.tag=0,_tmp3B5.f1=msg_str;_tmp3B5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B4;_tmp3B4.tag=0,({struct _fat_ptr _tmp455=({const char*_tmp3C=" pattern";_tag_fat(_tmp3C,sizeof(char),9U);});_tmp3B4.f1=_tmp455;});_tmp3B4;});void*_tmp38[3];_tmp38[0]=& _tmp39,_tmp38[1]=& _tmp3A,_tmp38[2]=& _tmp3B;({unsigned _tmp456=loc;Cyc_Warn_warn2(_tmp456,_tag_fat(_tmp38,sizeof(void*),3));});});{
struct Cyc_List_List*new_ps=0;
{int i=0;for(0;i < num_fields - num_pats;++ i){
new_ps=({struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));({void*_tmp457=f(loc);_tmp3E->hd=_tmp457;}),_tmp3E->tl=new_ps;_tmp3E;});}}
return Cyc_List_imp_append(earlier,new_ps);}}
# 128
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 131
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
return topt == 0?Cyc_Absyn_new_evar(& Cyc_Kinds_mko,({struct Cyc_Core_Opt*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->v=s;_tmp3F;})):*topt;}
# 134
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 138
if(topt == 0)
return numt;
if(Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;{
void*_tmp40=Cyc_Absyn_compress(numt);void*_stmttmp3=_tmp40;void*_tmp41=_stmttmp3;if(*((int*)_tmp41)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)){case 15: _LL1: _LL2:
 goto _LL4;case 16: _LL3: _LL4:
 return*topt;default: goto _LL5;}else{_LL5: _LL6:
 return numt;}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 149
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 151
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp458=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_tmp458;});}
# 155
({struct Cyc_List_List*_tmp45A=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));({struct _tuple17*_tmp459=({struct _tuple17*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=vd,_tmp42->f2=e;_tmp42;});_tmp43->hd=_tmp459;}),_tmp43->tl=*v_result_ptr;_tmp43;});*v_result_ptr=_tmp45A;});}
# 157
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 159
struct Cyc_Tcpat_TcPatResult _tmp44=res1;struct Cyc_List_List*_tmp46;struct _tuple1*_tmp45;_LL1: _tmp45=_tmp44.tvars_and_bounds_opt;_tmp46=_tmp44.patvars;_LL2: {struct _tuple1*p1=_tmp45;struct Cyc_List_List*vs1=_tmp46;
struct Cyc_Tcpat_TcPatResult _tmp47=res2;struct Cyc_List_List*_tmp49;struct _tuple1*_tmp48;_LL4: _tmp48=_tmp47.tvars_and_bounds_opt;_tmp49=_tmp47.patvars;_LL5: {struct _tuple1*p2=_tmp48;struct Cyc_List_List*vs2=_tmp49;
if(p1 == 0 && p2 == 0)
return({struct Cyc_Tcpat_TcPatResult _tmp3B7;_tmp3B7.tvars_and_bounds_opt=0,({struct Cyc_List_List*_tmp45B=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(vs1,vs2);_tmp3B7.patvars=_tmp45B;});_tmp3B7;});
if(p1 == 0)
p1=({struct _tuple1*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->f1=0,_tmp4A->f2=0;_tmp4A;});
if(p2 == 0)
p2=({struct _tuple1*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->f1=0,_tmp4B->f2=0;_tmp4B;});
return({struct Cyc_Tcpat_TcPatResult _tmp3B8;({struct _tuple1*_tmp45F=({struct _tuple1*_tmp4C=_cycalloc(sizeof(*_tmp4C));({struct Cyc_List_List*_tmp45E=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)((*p1).f1,(*p2).f1);_tmp4C->f1=_tmp45E;}),({
struct Cyc_List_List*_tmp45D=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)((*p1).f2,(*p2).f2);_tmp4C->f2=_tmp45D;});_tmp4C;});
# 167
_tmp3B8.tvars_and_bounds_opt=_tmp45F;}),({
# 169
struct Cyc_List_List*_tmp45C=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(vs1,vs2);_tmp3B8.patvars=_tmp45C;});_tmp3B8;});}}}
# 173
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp461=t;Cyc_Tcutil_promote_array(_tmp461,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct Cyc_List_List*_tmp460=Cyc_Tcenv_lookup_type_vars(te);_tmp4D->v=_tmp460;});_tmp4D;})): rgn_opt,0);}): t;}struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};
# 180
static struct _tuple18*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple18*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=tv,_tmp4E->f2=0;_tmp4E;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 184
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 187
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp3E1;_tmp3E1.tvars_and_bounds_opt=0,_tmp3E1.patvars=0;_tmp3E1;});
# 192
{void*_tmp4F=p->r;void*_stmttmp4=_tmp4F;void*_tmp50=_stmttmp4;int _tmp54;struct Cyc_List_List**_tmp53;struct Cyc_Absyn_Datatypefield*_tmp52;struct Cyc_Absyn_Datatypedecl*_tmp51;int _tmp58;struct Cyc_List_List**_tmp57;struct Cyc_List_List*_tmp56;struct Cyc_Absyn_Aggrdecl*_tmp55;int _tmp5A;struct Cyc_List_List**_tmp59;struct Cyc_Absyn_Pat*_tmp5B;void*_tmp5C;struct Cyc_Absyn_Enumdecl*_tmp5D;int _tmp5E;struct Cyc_Absyn_Vardecl*_tmp60;struct Cyc_Absyn_Tvar*_tmp5F;struct Cyc_Absyn_Pat*_tmp62;struct Cyc_Absyn_Vardecl*_tmp61;struct Cyc_Absyn_Vardecl*_tmp64;struct Cyc_Absyn_Tvar*_tmp63;struct Cyc_Absyn_Pat*_tmp66;struct Cyc_Absyn_Vardecl*_tmp65;switch(*((int*)_tmp50)){case 0: _LL1: _LL2:
# 194
 t=({struct Cyc_List_List*_tmp462=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp462,topt);});goto _LL0;case 1: _LL3: _tmp65=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_tmp66=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp50)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp65;struct Cyc_Absyn_Pat*p2=_tmp66;
# 198
struct _tuple2*_tmp67=vd->name;struct _tuple2*_stmttmp5=_tmp67;struct _tuple2*_tmp68=_stmttmp5;struct _fat_ptr _tmp69;_LL2C: _tmp69=*_tmp68->f2;_LL2D: {struct _fat_ptr v=_tmp69;
if(({struct _fat_ptr _tmp464=(struct _fat_ptr)v;Cyc_strcmp(_tmp464,({const char*_tmp6A="true";_tag_fat(_tmp6A,sizeof(char),5U);}));})== 0 ||({struct _fat_ptr _tmp463=(struct _fat_ptr)v;Cyc_strcmp(_tmp463,({const char*_tmp6B="false";_tag_fat(_tmp6B,sizeof(char),6U);}));})== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BB;_tmp3BB.tag=0,({struct _fat_ptr _tmp465=({const char*_tmp71="you probably do not want to use ";_tag_fat(_tmp71,sizeof(char),33U);});_tmp3BB.f1=_tmp465;});_tmp3BB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BA;_tmp3BA.tag=0,_tmp3BA.f1=v;_tmp3BA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B9;_tmp3B9.tag=0,({
struct _fat_ptr _tmp466=({const char*_tmp70=" as a local variable";_tag_fat(_tmp70,sizeof(char),21U);});_tmp3B9.f1=_tmp466;});_tmp3B9;});void*_tmp6C[3];_tmp6C[0]=& _tmp6D,_tmp6C[1]=& _tmp6E,_tmp6C[2]=& _tmp6F;({unsigned _tmp467=p->loc;Cyc_Warn_warn2(_tmp467,_tag_fat(_tmp6C,sizeof(void*),3));});});
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(p2->topt);
# 206
{void*_tmp72=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp72;void*_tmp73=_stmttmp6;if(*((int*)_tmp73)== 4){_LL2F: _LL30:
# 208
 if(rgn_pat == 0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp75=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BC;_tmp3BC.tag=0,({struct _fat_ptr _tmp468=({const char*_tmp76="array reference would point into unknown/unallowed region";_tag_fat(_tmp76,sizeof(char),58U);});_tmp3BC.f1=_tmp468;});_tmp3BC;});void*_tmp74[1];_tmp74[0]=& _tmp75;({unsigned _tmp469=p->loc;Cyc_Warn_err2(_tmp469,_tag_fat(_tmp74,sizeof(void*),1));});});
goto _LL2E;}else{_LL31: _LL32:
# 212
 if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp78=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BD;_tmp3BD.tag=0,({struct _fat_ptr _tmp46A=({const char*_tmp79="pattern would point to an abstract member";_tag_fat(_tmp79,sizeof(char),42U);});_tmp3BD.f1=_tmp46A;});_tmp3BD;});void*_tmp77[1];_tmp77[0]=& _tmp78;({unsigned _tmp46B=p->loc;Cyc_Warn_err2(_tmp46B,_tag_fat(_tmp77,sizeof(void*),1));});});
goto _LL2E;}_LL2E:;}
# 216
({struct Cyc_Absyn_Vardecl**_tmp46E=({struct Cyc_Absyn_Vardecl**_tmp7A=_cycalloc(sizeof(*_tmp7A));*_tmp7A=vd;_tmp7A;});struct Cyc_Absyn_Exp*_tmp46D=access_exp;struct Cyc_List_List**_tmp46C=& res.patvars;Cyc_Tcpat_set_vd(_tmp46E,_tmp46D,_tmp46C,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}}case 2: _LL5: _tmp63=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_tmp64=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp50)->f2;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp63;struct Cyc_Absyn_Vardecl*vd=_tmp64;
# 219
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp470=p->loc;struct Cyc_Tcenv_Tenv*_tmp46F=te;Cyc_Tcenv_add_type_vars(_tmp470,_tmp46F,({struct Cyc_Absyn_Tvar*_tmp87[1];_tmp87[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp87,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp471=({struct _tuple1*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->f1=0,_tmp7B->f2=0;_tmp7B;});res.tvars_and_bounds_opt=_tmp471;});
({struct Cyc_List_List*_tmp474=({
struct Cyc_List_List*_tmp473=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp473,({struct Cyc_List_List*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct _tuple18*_tmp472=({struct _tuple18*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=tv,_tmp7C->f2=1;_tmp7C;});_tmp7D->hd=_tmp472;}),_tmp7D->tl=0;_tmp7D;}));});
# 222
(*res.tvars_and_bounds_opt).f1=_tmp474;});
# 224
({unsigned _tmp477=p->loc;struct Cyc_Tcenv_Tenv*_tmp476=te2;struct Cyc_List_List*_tmp475=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp477,_tmp476,_tmp475,& Cyc_Kinds_tmk,1,0,vd->type);});
# 227
t=({struct Cyc_List_List*_tmp478=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp478,topt);});
{void*_tmp7E=Cyc_Absyn_compress(t);void*_stmttmp7=_tmp7E;void*_tmp7F=_stmttmp7;if(*((int*)_tmp7F)== 4){_LL34: _LL35:
# 230
 if(rgn_pat == 0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp81=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BE;_tmp3BE.tag=0,({struct _fat_ptr _tmp479=({const char*_tmp82="array reference would point into unknown/unallowed region";_tag_fat(_tmp82,sizeof(char),58U);});_tmp3BE.f1=_tmp479;});_tmp3BE;});void*_tmp80[1];_tmp80[0]=& _tmp81;({unsigned _tmp47A=p->loc;Cyc_Warn_err2(_tmp47A,_tag_fat(_tmp80,sizeof(void*),1));});});
goto _LL33;}else{_LL36: _LL37:
# 234
 if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BF;_tmp3BF.tag=0,({struct _fat_ptr _tmp47B=({const char*_tmp85="pattern would point to an abstract member";_tag_fat(_tmp85,sizeof(char),42U);});_tmp3BF.f1=_tmp47B;});_tmp3BF;});void*_tmp83[1];_tmp83[0]=& _tmp84;({unsigned _tmp47C=p->loc;Cyc_Warn_err2(_tmp47C,_tag_fat(_tmp83,sizeof(void*),1));});});
goto _LL33;}_LL33:;}
# 238
({struct Cyc_Absyn_Vardecl**_tmp47F=({struct Cyc_Absyn_Vardecl**_tmp86=_cycalloc(sizeof(*_tmp86));*_tmp86=vd;_tmp86;});struct Cyc_Absyn_Exp*_tmp47E=access_exp;struct Cyc_List_List**_tmp47D=& res.patvars;Cyc_Tcpat_set_vd(_tmp47F,_tmp47E,_tmp47D,vd->type);});
goto _LL0;}case 3: _LL7: _tmp61=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_tmp62=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp50)->f2;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp61;struct Cyc_Absyn_Pat*p2=_tmp62;
# 242
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(p2->topt);
if(!allow_ref_pat || rgn_pat == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C0;_tmp3C0.tag=0,({struct _fat_ptr _tmp480=({const char*_tmp8A="* pattern would point into an unknown/unallowed region";_tag_fat(_tmp8A,sizeof(char),55U);});_tmp3C0.f1=_tmp480;});_tmp3C0;});void*_tmp88[1];_tmp88[0]=& _tmp89;({unsigned _tmp481=p->loc;Cyc_Warn_err2(_tmp481,_tag_fat(_tmp88,sizeof(void*),1));});});
goto _LL0;}else{
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C1;_tmp3C1.tag=0,({struct _fat_ptr _tmp482=({const char*_tmp8D="* pattern cannot take the address of an alias-free path";_tag_fat(_tmp8D,sizeof(char),56U);});_tmp3C1.f1=_tmp482;});_tmp3C1;});void*_tmp8B[1];_tmp8B[0]=& _tmp8C;({unsigned _tmp483=p->loc;Cyc_Warn_err2(_tmp483,_tag_fat(_tmp8B,sizeof(void*),1));});});}{
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PtrInfo _tmp8E=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp8E;
pi.elt_type=t;
(pi.ptr_atts).rgn=rgn_pat;
(pi.ptr_atts).nullable=Cyc_Absyn_false_type;{
void*t2=Cyc_Absyn_pointer_type(pi);
if((unsigned)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);
new_access_exp->topt=t2;}
# 259
({struct Cyc_Absyn_Vardecl**_tmp486=({struct Cyc_Absyn_Vardecl**_tmp8F=_cycalloc(sizeof(*_tmp8F));*_tmp8F=vd;_tmp8F;});struct Cyc_Absyn_Exp*_tmp485=new_access_exp;struct Cyc_List_List**_tmp484=& res.patvars;Cyc_Tcpat_set_vd(_tmp486,_tmp485,_tmp484,t2);});
goto _LL0;}}}case 4: _LL9: _tmp5F=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_tmp60=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp50)->f2;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp5F;struct Cyc_Absyn_Vardecl*vd=_tmp60;
# 265
({struct Cyc_Absyn_Vardecl**_tmp489=({struct Cyc_Absyn_Vardecl**_tmp90=_cycalloc(sizeof(*_tmp90));*_tmp90=vd;_tmp90;});struct Cyc_Absyn_Exp*_tmp488=access_exp;struct Cyc_List_List**_tmp487=& res.patvars;Cyc_Tcpat_set_vd(_tmp489,_tmp488,_tmp487,vd->type);});
# 269
({unsigned _tmp48B=p->loc;struct Cyc_Tcenv_Tenv*_tmp48A=te;Cyc_Tcenv_add_type_vars(_tmp48B,_tmp48A,({struct Cyc_Absyn_Tvar*_tmp91[1];_tmp91[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp91,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp48C=({struct _tuple1*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->f1=0,_tmp92->f2=0;_tmp92;});res.tvars_and_bounds_opt=_tmp48C;});
({struct Cyc_List_List*_tmp48E=({struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*_tmp94));
({struct _tuple18*_tmp48D=({struct _tuple18*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->f1=tv,_tmp93->f2=0;_tmp93;});_tmp94->hd=_tmp48D;}),_tmp94->tl=(*res.tvars_and_bounds_opt).f1;_tmp94;});
# 272
(*res.tvars_and_bounds_opt).f1=_tmp48E;});
# 274
t=Cyc_Absyn_uint_type;
goto _LL0;}case 10: if(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp50)->f1 == Cyc_Absyn_Unsigned){_LLB: _LLC:
# 277
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;}else{_LLD: _LLE:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;}case 11: _LLF: _LL10:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 12: _LL11: _tmp5E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp50)->f2;_LL12: {int i=_tmp5E;
t=({void**_tmp48F=topt;Cyc_Tcpat_num_type(_tmp48F,Cyc_Absyn_gen_float_type((unsigned)i));});goto _LL0;}case 13: _LL13: _tmp5D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_LL14: {struct Cyc_Absyn_Enumdecl*ed=_tmp5D;
t=({void**_tmp490=topt;Cyc_Tcpat_num_type(_tmp490,Cyc_Absyn_enum_type(ed->name,ed));});goto _LL0;}case 14: _LL15: _tmp5C=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_LL16: {void*tenum=_tmp5C;
t=Cyc_Tcpat_num_type(topt,tenum);goto _LL0;}case 9: _LL17: _LL18:
# 284
 if(topt != 0){
void*_tmp95=Cyc_Absyn_compress(*topt);void*_stmttmp8=_tmp95;void*_tmp96=_stmttmp8;if(*((int*)_tmp96)== 3){_LL39: _LL3A:
 t=*topt;goto tcpat_end;}else{_LL3B: _LL3C:
 goto _LL38;}_LL38:;}{
# 289
struct Cyc_Absyn_PtrInfo _tmp97=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp97;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
t=Cyc_Absyn_pointer_type(pi);
goto _LL0;}case 6: _LL19: _tmp5B=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_LL1A: {struct Cyc_Absyn_Pat*p2=_tmp5B;
# 297
void*inner_typ=Cyc_Absyn_void_type;
void**_tmp98=0;void**inner_topt=_tmp98;
int elt_const=0;
if(topt != 0){
void*_tmp99=Cyc_Absyn_compress(*topt);void*_stmttmp9=_tmp99;void*_tmp9A=_stmttmp9;struct Cyc_Absyn_Tqual _tmp9C;void*_tmp9B;if(*((int*)_tmp9A)== 3){_LL3E: _tmp9B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A)->f1).elt_type;_tmp9C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A)->f1).elt_tq;_LL3F: {void*elt_typ=_tmp9B;struct Cyc_Absyn_Tqual elt_tq=_tmp9C;
# 303
inner_typ=elt_typ;
inner_topt=& inner_typ;
elt_const=elt_tq.real_const;
goto _LL3D;}}else{_LL40: _LL41:
 goto _LL3D;}_LL3D:;}{
# 312
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Kinds_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp491=res;Cyc_Tcpat_combine_results(_tmp491,Cyc_Tcpat_tcPatRec(te,p2,inner_topt,ptr_rgn,1,new_access_exp));});
# 321
{void*_tmp9D=Cyc_Absyn_compress((void*)_check_null(p2->topt));void*_stmttmpA=_tmp9D;void*_tmp9E=_stmttmpA;struct Cyc_List_List*_tmpA1;struct Cyc_Absyn_Datatypefield*_tmpA0;struct Cyc_Absyn_Datatypedecl*_tmp9F;if(*((int*)_tmp9E)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9E)->f1)== 19){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9E)->f1)->f1).KnownDatatypefield).tag == 2){_LL43: _tmp9F=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9E)->f1)->f1).KnownDatatypefield).val).f1;_tmpA0=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9E)->f1)->f1).KnownDatatypefield).val).f2;_tmpA1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9E)->f2;_LL44: {struct Cyc_Absyn_Datatypedecl*tud=_tmp9F;struct Cyc_Absyn_Datatypefield*tuf=_tmpA0;struct Cyc_List_List*targs=_tmpA1;
# 325
{void*_tmpA2=Cyc_Absyn_compress(inner_typ);void*_stmttmpB=_tmpA2;void*_tmpA3=_stmttmpB;if(*((int*)_tmpA3)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpA3)->f1)== 19){_LL48: _LL49:
# 327
 goto DONT_PROMOTE;}else{goto _LL4A;}}else{_LL4A: _LL4B:
 goto _LL47;}_LL47:;}{
# 331
void*new_type=({union Cyc_Absyn_DatatypeInfo _tmp492=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmpA5=_cycalloc(sizeof(*_tmpA5));*_tmpA5=tud;_tmpA5;}));Cyc_Absyn_datatype_type(_tmp492,targs);});
# 333
p2->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=3,(_tmpA4->f1).elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _tmp497=Cyc_Absyn_const_tqual(0U);(_tmpA4->f1).elt_tq=_tmp497;}):({
struct Cyc_Absyn_Tqual _tmp496=Cyc_Absyn_empty_tqual(0U);(_tmpA4->f1).elt_tq=_tmp496;}),
((_tmpA4->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp495=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmpA4->f1).ptr_atts).nullable=_tmp495;}),({
void*_tmp494=Cyc_Absyn_bounds_one();((_tmpA4->f1).ptr_atts).bounds=_tmp494;}),((_tmpA4->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmpA4->f1).ptr_atts).ptrloc=0,({
void*_tmp493=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmpA4->f1).ptr_atts).released=_tmp493;});_tmpA4;});
goto _LL42;}}}else{goto _LL45;}}else{goto _LL45;}}else{_LL45: _LL46:
# 342
 DONT_PROMOTE: {
struct Cyc_Absyn_PtrInfo _tmpA6=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmpA6;
pi.elt_type=(void*)_check_null(p2->topt);
({struct Cyc_Absyn_Tqual _tmp498=elt_const?Cyc_Absyn_const_tqual(0U): Cyc_Absyn_empty_tqual(0U);pi.elt_tq=_tmp498;});
(pi.ptr_atts).rgn=ptr_rgn;
t=Cyc_Absyn_pointer_type(pi);}}_LL42:;}
# 349
if((unsigned)new_access_exp)new_access_exp->topt=p2->topt;
goto _LL0;}}case 5: _LL1B: _tmp59=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_tmp5A=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp50)->f2;_LL1C: {struct Cyc_List_List**ps_p=_tmp59;int dots=_tmp5A;
# 353
struct Cyc_List_List*_tmpA7=*ps_p;struct Cyc_List_List*ps=_tmpA7;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpA8=Cyc_Absyn_compress(*topt);void*_stmttmpC=_tmpA8;void*_tmpA9=_stmttmpC;struct Cyc_List_List*_tmpAA;if(*((int*)_tmpA9)== 6){_LL4D: _tmpAA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA9)->f1;_LL4E: {struct Cyc_List_List*tqts=_tmpAA;
# 359
topt_ts=tqts;
if(dots){
({struct Cyc_List_List*_tmp49D=({struct Cyc_List_List*_tmp49C=ps;int _tmp49B=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ps);int _tmp49A=((int(*)(struct Cyc_List_List*))Cyc_List_length)(tqts);unsigned _tmp499=p->loc;((struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots)(_tmp49C,_tmp49B,_tmp49A,Cyc_Tcpat_wild_pat,_tmp499,({const char*_tmpAB="tuple";_tag_fat(_tmpAB,sizeof(char),6U);}));});*ps_p=_tmp49D;});
# 363
ps=*ps_p;}
# 365
goto _LL4C;}}else{_LL4F: _LL50:
 goto _LL4C;}_LL4C:;}else{
# 368
if(dots)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C2;_tmp3C2.tag=0,({struct _fat_ptr _tmp49E=({const char*_tmpAE="cannot determine missing fields for ... in tuple pattern";_tag_fat(_tmpAE,sizeof(char),57U);});_tmp3C2.f1=_tmp49E;});_tmp3C2;});void*_tmpAC[1];_tmpAC[0]=& _tmpAD;({unsigned _tmp49F=p->loc;Cyc_Warn_err2(_tmp49F,_tag_fat(_tmpAC,sizeof(void*),1));});});}
{int i=0;for(0;ps != 0;(ps=ps->tl,i ++)){
void**_tmpAF=0;void**inner_topt=_tmpAF;
if(topt_ts != 0){
inner_topt=&(*((struct _tuple19*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 376
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=({struct Cyc_Absyn_Exp*_tmp4A0=access_exp;Cyc_Absyn_subscript_exp(_tmp4A0,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3C3;(_tmp3C3.Int_c).tag=5U,((_tmp3C3.Int_c).val).f1=Cyc_Absyn_Unsigned,((_tmp3C3.Int_c).val).f2=i;_tmp3C3;}),0U),0U);});
# 381
res=({struct Cyc_Tcpat_TcPatResult _tmp4A1=res;Cyc_Tcpat_combine_results(_tmp4A1,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)ps->hd,inner_topt,rgn_pat,allow_ref_pat,new_access_exp));});
# 384
if((unsigned)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)ps->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));({struct _tuple19*_tmp4A3=({struct _tuple19*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct Cyc_Absyn_Tqual _tmp4A2=Cyc_Absyn_empty_tqual(0U);_tmpB0->f1=_tmp4A2;}),_tmpB0->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)ps->hd)->topt);_tmpB0;});_tmpB1->hd=_tmp4A3;}),_tmpB1->tl=pat_ts;_tmpB1;});}}}
# 387
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=6,({struct Cyc_List_List*_tmp4A4=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(pat_ts);_tmpB2->f1=_tmp4A4;});_tmpB2;});
goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp50)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp50)->f1)->KnownAggr).tag == 2){_LL1D: _tmp55=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp50)->f1)->KnownAggr).val;_tmp56=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp50)->f2;_tmp57=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp50)->f3;_tmp58=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp50)->f4;_LL1E: {struct Cyc_Absyn_Aggrdecl*ad=_tmp55;struct Cyc_List_List*exist_ts=_tmp56;struct Cyc_List_List**dps_p=_tmp57;int dots=_tmp58;
# 391
struct Cyc_List_List*_tmpB3=*dps_p;struct Cyc_List_List*dps=_tmpB3;
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C5;_tmp3C5.tag=0,({struct _fat_ptr _tmp4A5=({const char*_tmpB7="can't destructure abstract ";_tag_fat(_tmpB7,sizeof(char),28U);});_tmp3C5.f1=_tmp4A5;});_tmp3C5;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3C4;_tmp3C4.tag=6,_tmp3C4.f1=ad;_tmp3C4;});void*_tmpB4[2];_tmpB4[0]=& _tmpB5,_tmpB4[1]=& _tmpB6;({unsigned _tmp4A6=p->loc;Cyc_Warn_err2(_tmp4A6,_tag_fat(_tmpB4,sizeof(void*),2));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL0;}
# 399
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
allow_ref_pat=0;
if(exist_ts != 0){
# 405
if(topt == 0 || Cyc_Tcutil_type_kind(*topt)!= & Cyc_Kinds_ak)
allow_ref_pat=0;}
# 408
{struct _RegionHandle _tmpB8=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB8;_push_region(rgn);
# 410
{struct Cyc_List_List*_tmpB9=0;struct Cyc_List_List*var_tvs=_tmpB9;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpBA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct Cyc_List_List*u=_tmpBA;
{struct Cyc_List_List*t=exist_ts;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(u))->hd;
u=u->tl;{
void*_tmpBB=Cyc_Kinds_compress_kb(tv->kind);void*kb1=_tmpBB;
void*_tmpBC=Cyc_Kinds_compress_kb(uv->kind);void*kb2=_tmpBC;
struct Cyc_Absyn_Kind*k2;
{void*_tmpBD=kb2;struct Cyc_Absyn_Kind*_tmpBE;struct Cyc_Absyn_Kind*_tmpBF;switch(*((int*)_tmpBD)){case 2: _LL52: _tmpBF=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBD)->f2;_LL53: {struct Cyc_Absyn_Kind*k=_tmpBF;
_tmpBE=k;goto _LL55;}case 0: _LL54: _tmpBE=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpBD)->f1;_LL55: {struct Cyc_Absyn_Kind*k=_tmpBE;
k2=k;goto _LL51;}default: _LL56: _LL57:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C6;_tmp3C6.tag=0,({struct _fat_ptr _tmp4A7=({const char*_tmpC2="unconstrained existential tyvar in aggregate";_tag_fat(_tmpC2,sizeof(char),45U);});_tmp3C6.f1=_tmp4A7;});_tmp3C6;});void*_tmpC0[1];_tmpC0[0]=& _tmpC1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpC0,sizeof(void*),1));});}_LL51:;}
# 425
{void*_tmpC3=kb1;struct Cyc_Core_Opt**_tmpC4;struct Cyc_Absyn_Kind*_tmpC6;struct Cyc_Core_Opt**_tmpC5;struct Cyc_Absyn_Kind*_tmpC7;switch(*((int*)_tmpC3)){case 0: _LL59: _tmpC7=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC3)->f1;_LL5A: {struct Cyc_Absyn_Kind*k1=_tmpC7;
# 428
if(!Cyc_Kinds_kind_leq(k2,k1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CC;_tmp3CC.tag=0,({struct _fat_ptr _tmp4A8=({const char*_tmpD1="type variable ";_tag_fat(_tmpD1,sizeof(char),15U);});_tmp3CC.f1=_tmp4A8;});_tmp3CC;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp3CB;_tmp3CB.tag=7,_tmp3CB.f1=tv;_tmp3CB;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CA;_tmp3CA.tag=0,({struct _fat_ptr _tmp4A9=({const char*_tmpD0=" has kind ";_tag_fat(_tmpD0,sizeof(char),11U);});_tmp3CA.f1=_tmp4A9;});_tmp3CA;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp3C9;_tmp3C9.tag=8,_tmp3C9.f1=(void*)kb1;_tmp3C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C8;_tmp3C8.tag=0,({
struct _fat_ptr _tmp4AA=({const char*_tmpCF=" but must have at least kind ";_tag_fat(_tmpCF,sizeof(char),30U);});_tmp3C8.f1=_tmp4AA;});_tmp3C8;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp3C7;_tmp3C7.tag=9,_tmp3C7.f1=k2;_tmp3C7;});void*_tmpC8[6];_tmpC8[0]=& _tmpC9,_tmpC8[1]=& _tmpCA,_tmpC8[2]=& _tmpCB,_tmpC8[3]=& _tmpCC,_tmpC8[4]=& _tmpCD,_tmpC8[5]=& _tmpCE;({unsigned _tmp4AB=p->loc;Cyc_Warn_err2(_tmp4AB,_tag_fat(_tmpC8,sizeof(void*),6));});});
goto _LL58;}case 2: _LL5B: _tmpC5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC3)->f1;_tmpC6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC3)->f2;_LL5C: {struct Cyc_Core_Opt**f=_tmpC5;struct Cyc_Absyn_Kind*k=_tmpC6;
_tmpC4=f;goto _LL5E;}default: _LL5D: _tmpC4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC3)->f1;_LL5E: {struct Cyc_Core_Opt**f=_tmpC4;
({struct Cyc_Core_Opt*_tmp4AC=({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->v=kb2;_tmpD2;});*f=_tmp4AC;});goto _LL58;}}_LL58:;}{
# 435
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=2,_tmpD9->f1=tv;_tmpD9;});
var_tvs=({struct Cyc_List_List*_tmpD3=_region_malloc(rgn,sizeof(*_tmpD3));_tmpD3->hd=vartype,_tmpD3->tl=var_tvs;_tmpD3;});
# 439
if((int)k2->kind == (int)Cyc_Absyn_RgnKind){
if((int)k2->aliasqual != (int)Cyc_Absyn_Aliasable)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CD;_tmp3CD.tag=0,({struct _fat_ptr _tmp4AD=({const char*_tmpD6="opened existential had unique or top region kind";_tag_fat(_tmpD6,sizeof(char),49U);});_tmp3CD.f1=_tmp4AD;});_tmp3CD;});void*_tmpD4[1];_tmpD4[0]=& _tmpD5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpD4,sizeof(void*),1));});
outlives_constraints=({struct Cyc_List_List*_tmpD8=_cycalloc(sizeof(*_tmpD8));
({struct _tuple0*_tmp4AE=({struct _tuple0*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->f1=Cyc_Absyn_empty_effect,_tmpD7->f2=vartype;_tmpD7;});_tmpD8->hd=_tmp4AE;}),_tmpD8->tl=outlives_constraints;_tmpD8;});}}}}}
# 447
var_tvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(var_tvs);{
# 449
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,exist_ts);
# 451
struct _tuple13 _tmpDA=({struct _tuple13 _tmp3D6;({struct Cyc_List_List*_tmp4AF=Cyc_Tcenv_lookup_type_vars(te2);_tmp3D6.f1=_tmp4AF;}),_tmp3D6.f2=rgn;_tmp3D6;});struct _tuple13 env=_tmpDA;
struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmpDB;
struct Cyc_List_List*_tmpDC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,var_tvs);struct Cyc_List_List*exist_inst=_tmpDC;
struct Cyc_List_List*_tmpDD=((struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmpDD;
struct Cyc_List_List*_tmpDE=((struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmpDE;
struct Cyc_List_List*_tmpDF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rappend)(rgn,all_inst,exist_inst);struct Cyc_List_List*inst=_tmpDF;
# 458
if(exist_ts != 0 ||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp4B0=({struct _tuple1*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->f1=0,_tmpE0->f2=0;_tmpE0;});res.tvars_and_bounds_opt=_tmp4B0;});
({struct Cyc_List_List*_tmp4B2=({
struct Cyc_List_List*_tmp4B1=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp4B1,((struct Cyc_List_List*(*)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcpat_add_false,exist_ts));});
# 461
(*res.tvars_and_bounds_opt).f1=_tmp4B2;});
# 463
({struct Cyc_List_List*_tmp4B3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 463
(*res.tvars_and_bounds_opt).f2=_tmp4B3;});{
# 465
struct Cyc_List_List*_tmpE1=0;struct Cyc_List_List*rpo_inst=_tmpE1;
{struct Cyc_List_List*_tmpE2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmpE2;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*_tmpE4));({struct _tuple0*_tmp4B6=({struct _tuple0*_tmpE3=_cycalloc(sizeof(*_tmpE3));({void*_tmp4B5=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f1);_tmpE3->f1=_tmp4B5;}),({
void*_tmp4B4=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f2);_tmpE3->f2=_tmp4B4;});_tmpE3;});
# 467
_tmpE4->hd=_tmp4B6;}),_tmpE4->tl=rpo_inst;_tmpE4;});}}
# 470
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(rpo_inst);
({struct Cyc_List_List*_tmp4B7=
((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,rpo_inst);
# 471
(*res.tvars_and_bounds_opt).f2=_tmp4B7;});}}
# 476
t=({union Cyc_Absyn_AggrInfo _tmp4B8=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE5=_cycalloc(sizeof(*_tmpE5));*_tmpE5=ad;_tmpE5;}));Cyc_Absyn_aggr_type(_tmp4B8,all_typs);});
if(dots &&(int)ad->kind == (int)Cyc_Absyn_UnionA)
({void*_tmpE6=0U;({unsigned _tmp4BA=p->loc;struct _fat_ptr _tmp4B9=({const char*_tmpE7="`...' pattern not allowed in union pattern";_tag_fat(_tmpE7,sizeof(char),43U);});Cyc_Warn_warn(_tmp4BA,_tmp4B9,_tag_fat(_tmpE6,sizeof(void*),0));});});else{
if(dots){
({struct Cyc_List_List*_tmp4BF=({struct Cyc_List_List*_tmp4BE=dps;int _tmp4BD=((int(*)(struct Cyc_List_List*))Cyc_List_length)(dps);int _tmp4BC=((int(*)(struct Cyc_List_List*))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);unsigned _tmp4BB=p->loc;((struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots)(_tmp4BE,_tmp4BD,_tmp4BC,Cyc_Tcpat_wild_dlp,_tmp4BB,({const char*_tmpE8="struct";_tag_fat(_tmpE8,sizeof(char),7U);}));});*dps_p=_tmp4BF;});
# 482
dps=*dps_p;}}{
# 484
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,dps,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(1;fields != 0;fields=fields->tl){
struct _tuple20*_tmpE9=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmpD=_tmpE9;struct _tuple20*_tmpEA=_stmttmpD;struct Cyc_Absyn_Pat*_tmpEC;struct Cyc_Absyn_Aggrfield*_tmpEB;_LL60: _tmpEB=_tmpEA->f1;_tmpEC=_tmpEA->f2;_LL61: {struct Cyc_Absyn_Aggrfield*field=_tmpEB;struct Cyc_Absyn_Pat*pat=_tmpEC;
void*_tmpED=Cyc_Tcutil_rsubstitute(rgn,inst,field->type);void*inst_fieldtyp=_tmpED;
# 490
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,field->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp4C0=res;Cyc_Tcpat_combine_results(_tmp4C0,Cyc_Tcpat_tcPatRec(te2,pat,& inst_fieldtyp,rgn_pat,allow_ref_pat,new_access_exp));});
# 498
if(!Cyc_Unify_unify((void*)_check_null(pat->topt),inst_fieldtyp))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpEF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D5;_tmp3D5.tag=0,({struct _fat_ptr _tmp4C1=({const char*_tmpFA="field ";_tag_fat(_tmpFA,sizeof(char),7U);});_tmp3D5.f1=_tmp4C1;});_tmp3D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D4;_tmp3D4.tag=0,_tmp3D4.f1=*field->name;_tmp3D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D3;_tmp3D3.tag=0,({struct _fat_ptr _tmp4C2=({const char*_tmpF9=" of ";_tag_fat(_tmpF9,sizeof(char),5U);});_tmp3D3.f1=_tmp4C2;});_tmp3D3;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3D2;_tmp3D2.tag=6,_tmp3D2.f1=ad;_tmp3D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D1;_tmp3D1.tag=0,({
struct _fat_ptr _tmp4C3=({const char*_tmpF8=" pattern expects type ";_tag_fat(_tmpF8,sizeof(char),23U);});_tmp3D1.f1=_tmp4C3;});_tmp3D1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3D0;_tmp3D0.tag=2,_tmp3D0.f1=(void*)inst_fieldtyp;_tmp3D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CF;_tmp3CF.tag=0,({struct _fat_ptr _tmp4C4=({const char*_tmpF7=" != ";_tag_fat(_tmpF7,sizeof(char),5U);});_tmp3CF.f1=_tmp4C4;});_tmp3CF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3CE;_tmp3CE.tag=2,_tmp3CE.f1=(void*)_check_null(pat->topt);_tmp3CE;});void*_tmpEE[8];_tmpEE[0]=& _tmpEF,_tmpEE[1]=& _tmpF0,_tmpEE[2]=& _tmpF1,_tmpEE[3]=& _tmpF2,_tmpEE[4]=& _tmpF3,_tmpEE[5]=& _tmpF4,_tmpEE[6]=& _tmpF5,_tmpEE[7]=& _tmpF6;({unsigned _tmp4C5=p->loc;Cyc_Warn_err2(_tmp4C5,_tag_fat(_tmpEE,sizeof(void*),8));});});
# 502
if((unsigned)new_access_exp)new_access_exp->topt=pat->topt;}}}}}
# 410
;_pop_region();}
# 505
goto _LL0;}}else{_LL23: _LL24:
# 545
 goto _LL26;}}else{_LL21: _LL22:
# 544
 goto _LL24;}case 8: _LL1F: _tmp51=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp50)->f2;_tmp53=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp50)->f3;_tmp54=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp50)->f4;_LL20: {struct Cyc_Absyn_Datatypedecl*tud=_tmp51;struct Cyc_Absyn_Datatypefield*tuf=_tmp52;struct Cyc_List_List**ps_p=_tmp53;int dots=_tmp54;
# 508
struct Cyc_List_List*_tmpFB=*ps_p;struct Cyc_List_List*ps=_tmpFB;
struct Cyc_List_List*tqts=tuf->typs;
# 511
struct _tuple13 _tmpFC=({struct _tuple13 _tmp3E0;({struct Cyc_List_List*_tmp4C6=Cyc_Tcenv_lookup_type_vars(te);_tmp3E0.f1=_tmp4C6;}),_tmp3E0.f2=Cyc_Core_heap_region;_tmp3E0;});struct _tuple13 env=_tmpFC;
struct Cyc_List_List*_tmpFD=((struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmpFD;
struct Cyc_List_List*_tmpFE=((struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmpFE;
t=({union Cyc_Absyn_DatatypeFieldInfo _tmp4C7=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp4C7,all_typs);});
if(dots){
({struct Cyc_List_List*_tmp4CC=({struct Cyc_List_List*_tmp4CB=ps;int _tmp4CA=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ps);int _tmp4C9=((int(*)(struct Cyc_List_List*))Cyc_List_length)(tqts);unsigned _tmp4C8=p->loc;((struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots)(_tmp4CB,_tmp4CA,_tmp4C9,Cyc_Tcpat_wild_pat,_tmp4C8,({const char*_tmpFF="datatype field";_tag_fat(_tmpFF,sizeof(char),15U);}));});*ps_p=_tmp4CC;});
# 518
ps=*ps_p;}
# 520
for(1;ps != 0 && tqts != 0;(ps=ps->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp100=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp100;
# 524
void*_tmp101=Cyc_Tcutil_substitute(inst,(*((struct _tuple19*)tqts->hd)).f2);void*field_typ=_tmp101;
# 527
if((unsigned)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
res=({struct Cyc_Tcpat_TcPatResult _tmp4CD=res;Cyc_Tcpat_combine_results(_tmp4CD,Cyc_Tcpat_tcPatRec(te,p2,& field_typ,rgn_pat,allow_ref_pat,0));});
# 534
if(!Cyc_Unify_unify((void*)_check_null(p2->topt),field_typ))
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp103=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3DB;_tmp3DB.tag=1,_tmp3DB.f1=tuf->name;_tmp3DB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp104=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DA;_tmp3DA.tag=0,({struct _fat_ptr _tmp4CE=({const char*_tmp109=" expects argument type ";_tag_fat(_tmp109,sizeof(char),24U);});_tmp3DA.f1=_tmp4CE;});_tmp3DA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp105=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3D9;_tmp3D9.tag=2,_tmp3D9.f1=(void*)field_typ;_tmp3D9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp106=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D8;_tmp3D8.tag=0,({
struct _fat_ptr _tmp4CF=({const char*_tmp108=" not ";_tag_fat(_tmp108,sizeof(char),6U);});_tmp3D8.f1=_tmp4CF;});_tmp3D8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp107=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3D7;_tmp3D7.tag=2,_tmp3D7.f1=(void*)_check_null(p2->topt);_tmp3D7;});void*_tmp102[5];_tmp102[0]=& _tmp103,_tmp102[1]=& _tmp104,_tmp102[2]=& _tmp105,_tmp102[3]=& _tmp106,_tmp102[4]=& _tmp107;({unsigned _tmp4D0=p2->loc;Cyc_Warn_err2(_tmp4D0,_tag_fat(_tmp102,sizeof(void*),5));});});}
# 538
if(ps != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DD;_tmp3DD.tag=0,({struct _fat_ptr _tmp4D1=({const char*_tmp10D="too many arguments for datatype constructor ";_tag_fat(_tmp10D,sizeof(char),45U);});_tmp3DD.f1=_tmp4D1;});_tmp3DD;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3DC;_tmp3DC.tag=1,_tmp3DC.f1=tuf->name;_tmp3DC;});void*_tmp10A[2];_tmp10A[0]=& _tmp10B,_tmp10A[1]=& _tmp10C;({unsigned _tmp4D2=p->loc;Cyc_Warn_err2(_tmp4D2,_tag_fat(_tmp10A,sizeof(void*),2));});});
if(tqts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DF;_tmp3DF.tag=0,({struct _fat_ptr _tmp4D3=({const char*_tmp111="too few arguments for datatype constructor ";_tag_fat(_tmp111,sizeof(char),44U);});_tmp3DF.f1=_tmp4D3;});_tmp3DF;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp110=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3DE;_tmp3DE.tag=1,_tmp3DE.f1=tuf->name;_tmp3DE;});void*_tmp10E[2];_tmp10E[0]=& _tmp10F,_tmp10E[1]=& _tmp110;({unsigned _tmp4D4=p->loc;Cyc_Warn_err2(_tmp4D4,_tag_fat(_tmp10E,sizeof(void*),2));});});
goto _LL0;}case 15: _LL25: _LL26:
# 546
 goto _LL28;case 17: _LL27: _LL28:
 goto _LL2A;default: _LL29: _LL2A:
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL0;}_LL0:;}
# 550
tcpat_end:
 p->topt=t;
return res;}}
# 555
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 557
struct Cyc_Tcpat_TcPatResult _tmp112=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);struct Cyc_Tcpat_TcPatResult ans=_tmp112;
# 559
struct _tuple1 _tmp113=((struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split)(ans.patvars);struct _tuple1 _stmttmpE=_tmp113;struct _tuple1 _tmp114=_stmttmpE;struct Cyc_List_List*_tmp115;_LL1: _tmp115=_tmp114.f1;_LL2: {struct Cyc_List_List*vs1=_tmp115;
struct Cyc_List_List*_tmp116=0;struct Cyc_List_List*vs=_tmp116;
{struct Cyc_List_List*x=vs1;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)vs=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),_tmp117->tl=vs;_tmp117;});}}
({struct Cyc_List_List*_tmp4D6=((struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcpat_get_name,vs);unsigned _tmp4D5=p->loc;Cyc_Tcutil_check_unique_vars(_tmp4D6,_tmp4D5,({const char*_tmp118="pattern contains a repeated variable";_tag_fat(_tmp118,sizeof(char),37U);}));});
# 568
{struct Cyc_List_List*x=ans.patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp119=(struct _tuple17*)x->hd;struct _tuple17*_stmttmpF=_tmp119;struct _tuple17*_tmp11A=_stmttmpF;struct Cyc_Absyn_Exp**_tmp11C;struct Cyc_Absyn_Vardecl**_tmp11B;_LL4: _tmp11B=_tmp11A->f1;_tmp11C=(struct Cyc_Absyn_Exp**)& _tmp11A->f2;_LL5: {struct Cyc_Absyn_Vardecl**vdopt=_tmp11B;struct Cyc_Absyn_Exp**expopt=_tmp11C;
if(*expopt != 0 &&*expopt != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp4D7=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*expopt));*expopt=_tmp4D7;});}}}
# 573
return ans;}}
# 579
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 582
struct _tuple0 _tmp11D=({struct _tuple0 _tmp3E2;({void*_tmp4D9=Cyc_Absyn_compress(old_type);_tmp3E2.f1=_tmp4D9;}),({void*_tmp4D8=Cyc_Absyn_compress(new_type);_tmp3E2.f2=_tmp4D8;});_tmp3E2;});struct _tuple0 _stmttmp10=_tmp11D;struct _tuple0 _tmp11E=_stmttmp10;struct Cyc_Absyn_PtrInfo _tmp120;struct Cyc_Absyn_PtrInfo _tmp11F;if(*((int*)_tmp11E.f1)== 3){if(*((int*)_tmp11E.f2)== 3){_LL1: _tmp11F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11E.f1)->f1;_tmp120=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11E.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pold=_tmp11F;struct Cyc_Absyn_PtrInfo pnew=_tmp120;
# 584
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp121=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=3,(_tmp122->f1).elt_type=pold.elt_type,(_tmp122->f1).elt_tq=pnew.elt_tq,
((_tmp122->f1).ptr_atts).rgn=(pold.ptr_atts).rgn,((_tmp122->f1).ptr_atts).nullable=(pnew.ptr_atts).nullable,((_tmp122->f1).ptr_atts).bounds=(pnew.ptr_atts).bounds,((_tmp122->f1).ptr_atts).zero_term=(pnew.ptr_atts).zero_term,((_tmp122->f1).ptr_atts).ptrloc=(pold.ptr_atts).ptrloc,((_tmp122->f1).ptr_atts).released=(pnew.ptr_atts).released;_tmp122;});
# 584
struct Cyc_Absyn_PointerType_Absyn_Type_struct*ptry=_tmp121;
# 591
return({struct Cyc_RgnOrder_RgnPO*_tmp4DE=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp4DD=assump;void*_tmp4DC=(void*)ptry;Cyc_Tcutil_subtype(_tmp4DE,_tmp4DD,_tmp4DC,new_type);})&&({
struct Cyc_RgnOrder_RgnPO*_tmp4DB=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_Absyn_Exp*_tmp4DA=initializer;Cyc_Tcutil_coerce_assign(_tmp4DB,_tmp4DA,(void*)ptry);});}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 600
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 603
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));({struct _tuple0*_tmp4E0=({struct _tuple0*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->f1=Cyc_Absyn_unique_rgn_type,({void*_tmp4DF=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=2,_tmp131->f1=tv;_tmp131;});_tmp132->f2=_tmp4DF;});_tmp132;});_tmp133->hd=_tmp4E0;}),_tmp133->tl=0;_tmp133;});
if(({struct Cyc_RgnOrder_RgnPO*_tmp4E3=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp4E2=assump;void*_tmp4E1=old_type;Cyc_Tcutil_subtype(_tmp4E3,_tmp4E2,_tmp4E1,new_type);})){
# 621 "tcpat.cyc"
struct _tuple0 _tmp123=({struct _tuple0 _tmp3E4;({void*_tmp4E5=Cyc_Absyn_compress(old_type);_tmp3E4.f1=_tmp4E5;}),({void*_tmp4E4=Cyc_Absyn_compress(new_type);_tmp3E4.f2=_tmp4E4;});_tmp3E4;});struct _tuple0 _stmttmp11=_tmp123;struct _tuple0 _tmp124=_stmttmp11;struct Cyc_Absyn_PtrInfo _tmp126;struct Cyc_Absyn_PtrInfo _tmp125;if(*((int*)_tmp124.f1)== 3){if(*((int*)_tmp124.f2)== 3){_LL1: _tmp125=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp124.f1)->f1;_tmp126=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp124.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pold=_tmp125;struct Cyc_Absyn_PtrInfo pnew=_tmp126;
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp128=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E3;_tmp3E3.tag=0,({struct _fat_ptr _tmp4E6=({const char*_tmp129="alias requires pointer type";_tag_fat(_tmp129,sizeof(char),28U);});_tmp3E3.f1=_tmp4E6;});_tmp3E3;});void*_tmp127[1];_tmp127[0]=& _tmp128;({unsigned _tmp4E7=loc;Cyc_Warn_err2(_tmp4E7,_tag_fat(_tmp127,sizeof(void*),1));});});goto _LL0;}_LL0:;}else{
# 626
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E8;_tmp3E8.tag=0,({struct _fat_ptr _tmp4E8=({const char*_tmp130="cannot alias value of type ";_tag_fat(_tmp130,sizeof(char),28U);});_tmp3E8.f1=_tmp4E8;});_tmp3E8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp12C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3E7;_tmp3E7.tag=2,_tmp3E7.f1=(void*)old_type;_tmp3E7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E6;_tmp3E6.tag=0,({struct _fat_ptr _tmp4E9=({const char*_tmp12F=" to type ";_tag_fat(_tmp12F,sizeof(char),10U);});_tmp3E6.f1=_tmp4E9;});_tmp3E6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp12E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3E5;_tmp3E5.tag=2,_tmp3E5.f1=(void*)new_type;_tmp3E5;});void*_tmp12A[4];_tmp12A[0]=& _tmp12B,_tmp12A[1]=& _tmp12C,_tmp12A[2]=& _tmp12D,_tmp12A[3]=& _tmp12E;({unsigned _tmp4EA=loc;Cyc_Warn_err2(_tmp4EA,_tag_fat(_tmp12A,sizeof(void*),4));});});}}
# 632
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 634
void*_tmp134=p->r;void*_stmttmp12=_tmp134;void*_tmp135=_stmttmp12;struct Cyc_Absyn_Vardecl*_tmp137;struct Cyc_Absyn_Tvar*_tmp136;struct Cyc_Absyn_Pat*_tmp139;struct Cyc_Absyn_Vardecl*_tmp138;struct Cyc_Absyn_Pat*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13A;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp13F;union Cyc_Absyn_AggrInfo*_tmp13E;struct Cyc_Absyn_Pat*_tmp141;switch(*((int*)_tmp135)){case 6: _LL1: _tmp141=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp135)->f1;_LL2: {struct Cyc_Absyn_Pat*p2=_tmp141;
# 636
void*_tmp142=(void*)_check_null(p->topt);void*_stmttmp13=_tmp142;void*_tmp143=_stmttmp13;void*_tmp144;if(*((int*)_tmp143)== 3){_LL12: _tmp144=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143)->f1).ptr_atts).rgn;_LL13: {void*rt=_tmp144;
# 638
Cyc_Tcenv_check_rgn_accessible(te,p->loc,rt);
({struct Cyc_Tcenv_Tenv*_tmp4ED=te;struct Cyc_Absyn_Pat*_tmp4EC=p2;int _tmp4EB=Cyc_Tcutil_is_noalias_region(rt,0);Cyc_Tcpat_check_pat_regions_rec(_tmp4ED,_tmp4EC,_tmp4EB,patvars);});
return;}}else{_LL14: _LL15:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp146=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E9;_tmp3E9.tag=0,({struct _fat_ptr _tmp4EE=({const char*_tmp147="check_pat_regions: bad pointer type";_tag_fat(_tmp147,sizeof(char),36U);});_tmp3E9.f1=_tmp4EE;});_tmp3E9;});void*_tmp145[1];_tmp145[0]=& _tmp146;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp145,sizeof(void*),1));});}_LL11:;}case 7: _LL3: _tmp13E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp135)->f1;_tmp13F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp135)->f2;_tmp140=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp135)->f3;_LL4: {union Cyc_Absyn_AggrInfo*ai=_tmp13E;struct Cyc_List_List*exist_ts=_tmp13F;struct Cyc_List_List*dps=_tmp140;
# 644
for(1;dps != 0;dps=dps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple16*)dps->hd)).f2,did_noalias_deref,patvars);}
return;}case 8: _LL5: _tmp13D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp135)->f3;_LL6: {struct Cyc_List_List*ps=_tmp13D;
# 648
did_noalias_deref=0;_tmp13C=ps;goto _LL8;}case 5: _LL7: _tmp13C=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp135)->f1;_LL8: {struct Cyc_List_List*ps=_tmp13C;
# 650
for(1;ps != 0;ps=ps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)ps->hd,did_noalias_deref,patvars);}
return;}case 3: _LL9: _tmp13A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp135)->f1;_tmp13B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp135)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp13A;struct Cyc_Absyn_Pat*p2=_tmp13B;
# 654
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp148=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp14=_tmp148;struct _tuple17*_tmp149=_stmttmp14;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Vardecl**_tmp14A;_LL17: _tmp14A=_tmp149->f1;_tmp14B=_tmp149->f2;_LL18: {struct Cyc_Absyn_Vardecl**vdopt=_tmp14A;struct Cyc_Absyn_Exp*eopt=_tmp14B;
# 660
if((vdopt != 0 &&*vdopt == vd)&& eopt != 0){
{void*_tmp14C=eopt->r;void*_stmttmp15=_tmp14C;void*_tmp14D=_stmttmp15;struct Cyc_Absyn_Exp*_tmp14E;if(*((int*)_tmp14D)== 15){_LL1A: _tmp14E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp14D)->f1;_LL1B: {struct Cyc_Absyn_Exp*e=_tmp14E;
# 663
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp150=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EA;_tmp3EA.tag=0,({struct _fat_ptr _tmp4EF=({const char*_tmp151="reference pattern not allowed on alias-free pointers";_tag_fat(_tmp151,sizeof(char),53U);});_tmp3EA.f1=_tmp4EF;});_tmp3EA;});void*_tmp14F[1];_tmp14F[0]=& _tmp150;({unsigned _tmp4F0=p->loc;Cyc_Warn_err2(_tmp4F0,_tag_fat(_tmp14F,sizeof(void*),1));});});
goto _LL19;}}else{_LL1C: _LL1D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp153=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EB;_tmp3EB.tag=0,({struct _fat_ptr _tmp4F1=({const char*_tmp154="check_pat_regions: bad reference access exp";_tag_fat(_tmp154,sizeof(char),44U);});_tmp3EB.f1=_tmp4F1;});_tmp3EB;});void*_tmp152[1];_tmp152[0]=& _tmp153;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp152,sizeof(void*),1));});}_LL19:;}
# 668
break;}}}}
# 671
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 1: _LLB: _tmp138=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp135)->f1;_tmp139=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp135)->f2;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp138;struct Cyc_Absyn_Pat*p2=_tmp139;
# 674
{void*_tmp155=p->topt;void*_stmttmp16=_tmp155;void*_tmp156=_stmttmp16;if(_tmp156 != 0){if(*((int*)_tmp156)== 4){_LL1F: _LL20:
# 676
 if(!did_noalias_deref)
goto _LL1E;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp158=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EC;_tmp3EC.tag=0,({struct _fat_ptr _tmp4F2=({const char*_tmp159="pattern to array would create alias of no-alias pointer";_tag_fat(_tmp159,sizeof(char),56U);});_tmp3EC.f1=_tmp4F2;});_tmp3EC;});void*_tmp157[1];_tmp157[0]=& _tmp158;({unsigned _tmp4F3=p->loc;Cyc_Warn_err2(_tmp4F3,_tag_fat(_tmp157,sizeof(void*),1));});});
return;}else{goto _LL21;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 682
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 2: _LLD: _tmp136=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp135)->f1;_tmp137=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp135)->f2;_LLE: {struct Cyc_Absyn_Tvar*tv=_tmp136;struct Cyc_Absyn_Vardecl*vd=_tmp137;
# 685
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp15A=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp17=_tmp15A;struct _tuple17*_tmp15B=_stmttmp17;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Vardecl**_tmp15C;_LL24: _tmp15C=_tmp15B->f1;_tmp15D=_tmp15B->f2;_LL25: {struct Cyc_Absyn_Vardecl**vdopt=_tmp15C;struct Cyc_Absyn_Exp*eopt=_tmp15D;
# 689
if(vdopt != 0 &&*vdopt == vd){
if(eopt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3ED;_tmp3ED.tag=0,({struct _fat_ptr _tmp4F4=({const char*_tmp160="cannot alias pattern expression in datatype";_tag_fat(_tmp160,sizeof(char),44U);});_tmp3ED.f1=_tmp4F4;});_tmp3ED;});void*_tmp15E[1];_tmp15E[0]=& _tmp15F;({unsigned _tmp4F5=p->loc;Cyc_Warn_err2(_tmp4F5,_tag_fat(_tmp15E,sizeof(void*),1));});});else{
# 693
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp4F7=p->loc;struct Cyc_Tcenv_Tenv*_tmp4F6=te;Cyc_Tcenv_add_type_vars(_tmp4F7,_tmp4F6,({struct Cyc_Absyn_Tvar*_tmp162[1];_tmp162[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp162,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
te2=({struct Cyc_Tcenv_Tenv*_tmp4F8=te2;Cyc_Tcenv_add_region(_tmp4F8,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->tag=2,_tmp161->f1=tv;_tmp161;}),0,0);});
# 696
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(eopt->topt),tv,vd->type,eopt);}
# 698
break;}}}}
# 701
goto _LL0;}default: _LLF: _LL10:
 return;}_LL0:;}
# 717 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 719
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp163=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp18=_tmp163;struct _tuple17*_tmp164=_stmttmp18;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Vardecl**_tmp165;_LL1: _tmp165=_tmp164->f1;_tmp166=_tmp164->f2;_LL2: {struct Cyc_Absyn_Vardecl**vdopt=_tmp165;struct Cyc_Absyn_Exp*eopt=_tmp166;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp167=eopt;struct Cyc_Absyn_Exp*e=_tmp167;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))&& !Cyc_Tcutil_is_noalias_path(e)){
# 726
if(vdopt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp169=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EF;_tmp3EF.tag=0,({struct _fat_ptr _tmp4F9=({const char*_tmp16C="pattern dereferences an alias-free-pointer ";_tag_fat(_tmp16C,sizeof(char),44U);});_tmp3EF.f1=_tmp4F9;});_tmp3EF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EE;_tmp3EE.tag=0,({
struct _fat_ptr _tmp4FA=({const char*_tmp16B="from a non-unique path";_tag_fat(_tmp16B,sizeof(char),23U);});_tmp3EE.f1=_tmp4FA;});_tmp3EE;});void*_tmp168[2];_tmp168[0]=& _tmp169,_tmp168[1]=& _tmp16A;({unsigned _tmp4FB=p->loc;Cyc_Warn_err2(_tmp4FB,_tag_fat(_tmp168,sizeof(void*),2));});});else{
# 730
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F2;_tmp3F2.tag=0,({struct _fat_ptr _tmp4FC=({const char*_tmp172="pattern for variable ";_tag_fat(_tmp172,sizeof(char),22U);});_tmp3F2.f1=_tmp4FC;});_tmp3F2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3F1;_tmp3F1.tag=1,_tmp3F1.f1=(*vdopt)->name;_tmp3F1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp170=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F0;_tmp3F0.tag=0,({
struct _fat_ptr _tmp4FD=({const char*_tmp171=" dereferences an alias-free-pointer from a non-unique path";_tag_fat(_tmp171,sizeof(char),59U);});_tmp3F0.f1=_tmp4FD;});_tmp3F0;});void*_tmp16D[3];_tmp16D[0]=& _tmp16E,_tmp16D[1]=& _tmp16F,_tmp16D[2]=& _tmp170;({unsigned _tmp4FE=p->loc;Cyc_Warn_err2(_tmp4FE,_tag_fat(_tmp16D,sizeof(void*),3));});});}}}}}}}
# 777 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 787
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 801
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){return({union Cyc_Tcpat_Name_value _tmp3F3;(_tmp3F3.Name_v).tag=1U,(_tmp3F3.Name_v).val=s;_tmp3F3;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp3F4;(_tmp3F4.Int_v).tag=2U,(_tmp3F4.Int_v).val=i;_tmp3F4;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 818
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp173=c1->name;union Cyc_Tcpat_Name_value _stmttmp19=_tmp173;union Cyc_Tcpat_Name_value _tmp174=_stmttmp19;int _tmp175;struct _fat_ptr _tmp176;if((_tmp174.Name_v).tag == 1){_LL1: _tmp176=(_tmp174.Name_v).val;_LL2: {struct _fat_ptr n1=_tmp176;
# 821
union Cyc_Tcpat_Name_value _tmp177=c2->name;union Cyc_Tcpat_Name_value _stmttmp1A=_tmp177;union Cyc_Tcpat_Name_value _tmp178=_stmttmp1A;struct _fat_ptr _tmp179;if((_tmp178.Name_v).tag == 1){_LL6: _tmp179=(_tmp178.Name_v).val;_LL7: {struct _fat_ptr n2=_tmp179;
return Cyc_strcmp((struct _fat_ptr)n1,(struct _fat_ptr)n2);}}else{_LL8: _LL9:
 return - 1;}_LL5:;}}else{_LL3: _tmp175=(_tmp174.Int_v).val;_LL4: {int i1=_tmp175;
# 826
union Cyc_Tcpat_Name_value _tmp17A=c2->name;union Cyc_Tcpat_Name_value _stmttmp1B=_tmp17A;union Cyc_Tcpat_Name_value _tmp17B=_stmttmp1B;int _tmp17C;if((_tmp17B.Name_v).tag == 1){_LLB: _LLC:
 return 1;}else{_LLD: _tmp17C=(_tmp17B.Int_v).val;_LLE: {int i2=_tmp17C;
return i1 - i2;}}_LLA:;}}_LL0:;}
# 834
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set (void){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 838
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 842
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned ans=0U;
struct Cyc_List_List*_tmp17D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp17D;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 849
return ans;}
# 852
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp17E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp17E;for(0;fields != 0;(fields=fields->tl,++ i)){
if(Cyc_strcmp((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,(struct _fat_ptr)*f)== 0)return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F7;_tmp3F7.tag=0,({struct _fat_ptr _tmp4FF=({const char*_tmp184="get_member_offset ";_tag_fat(_tmp184,sizeof(char),19U);});_tmp3F7.f1=_tmp4FF;});_tmp3F7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp181=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F6;_tmp3F6.tag=0,_tmp3F6.f1=*f;_tmp3F6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F5;_tmp3F5.tag=0,({struct _fat_ptr _tmp500=({const char*_tmp183=" failed";_tag_fat(_tmp183,sizeof(char),8U);});_tmp3F5.f1=_tmp500;});_tmp3F5;});void*_tmp17F[3];_tmp17F[0]=& _tmp180,_tmp17F[1]=& _tmp181,_tmp17F[2]=& _tmp182;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp17F,sizeof(void*),3));});}
# 859
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp185=pw;struct Cyc_Absyn_Pat*_tmp186;struct Cyc_Absyn_Exp*_tmp187;if((_tmp185.where_clause).tag == 2){_LL1: _tmp187=(_tmp185.where_clause).val;_LL2: {struct Cyc_Absyn_Exp*e=_tmp187;
return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->tag=0,_tmp188->f1=e;_tmp188;});}}else{_LL3: _tmp186=(_tmp185.pattern).val;_LL4: {struct Cyc_Absyn_Pat*p=_tmp186;
# 863
void*_tmp189=p->r;void*_stmttmp1C=_tmp189;void*_tmp18A=_stmttmp1C;struct Cyc_List_List*_tmp18C;union Cyc_Absyn_AggrInfo _tmp18B;struct Cyc_Absyn_Datatypefield*_tmp18E;struct Cyc_Absyn_Datatypedecl*_tmp18D;struct Cyc_Absyn_Enumfield*_tmp190;void*_tmp18F;struct Cyc_Absyn_Enumfield*_tmp192;struct Cyc_Absyn_Enumdecl*_tmp191;int _tmp194;struct _fat_ptr _tmp193;char _tmp195;int _tmp197;enum Cyc_Absyn_Sign _tmp196;struct Cyc_Absyn_Pat*_tmp198;struct Cyc_Absyn_Pat*_tmp199;switch(*((int*)_tmp18A)){case 1: _LL6: _tmp199=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp18A)->f2;_LL7: {struct Cyc_Absyn_Pat*p1=_tmp199;
_tmp198=p1;goto _LL9;}case 3: _LL8: _tmp198=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp18A)->f2;_LL9: {struct Cyc_Absyn_Pat*p1=_tmp198;
return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp3F8;(_tmp3F8.pattern).tag=1U,(_tmp3F8.pattern).val=p1;_tmp3F8;}));}case 9: _LLA: _LLB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _LLC: _tmp196=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp18A)->f1;_tmp197=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp18A)->f2;_LLD: {enum Cyc_Absyn_Sign s=_tmp196;int i=_tmp197;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->tag=6,_tmp19A->f1=(unsigned)i;_tmp19A;});}case 11: _LLE: _tmp195=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp18A)->f1;_LLF: {char c=_tmp195;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->tag=6,_tmp19B->f1=(unsigned)c;_tmp19B;});}case 12: _LL10: _tmp193=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp18A)->f1;_tmp194=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp18A)->f2;_LL11: {struct _fat_ptr f=_tmp193;int i=_tmp194;
return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->tag=5,_tmp19C->f1=f,_tmp19C->f2=i;_tmp19C;});}case 13: _LL12: _tmp191=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp18A)->f1;_tmp192=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp18A)->f2;_LL13: {struct Cyc_Absyn_Enumdecl*ed=_tmp191;struct Cyc_Absyn_Enumfield*ef=_tmp192;
return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->tag=3,_tmp19D->f1=ed,_tmp19D->f2=ef;_tmp19D;});}case 14: _LL14: _tmp18F=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp18A)->f1;_tmp190=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp18A)->f2;_LL15: {void*t=_tmp18F;struct Cyc_Absyn_Enumfield*ef=_tmp190;
return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=4,_tmp19E->f1=t,_tmp19E->f2=ef;_tmp19E;});}case 6: _LL16: _LL17:
# 873
 if(!Cyc_Tcutil_is_pointer_type((void*)_check_null(p->topt))|| !
Cyc_Tcutil_is_nullable_pointer_type((void*)_check_null(p->topt),0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F9;_tmp3F9.tag=0,({struct _fat_ptr _tmp501=({const char*_tmp1A1="non-null pointer type or non-pointer type in pointer pattern";_tag_fat(_tmp1A1,sizeof(char),61U);});_tmp3F9.f1=_tmp501;});_tmp3F9;});void*_tmp19F[1];_tmp19F[0]=& _tmp1A0;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp19F,sizeof(void*),1));});
return(void*)& Cyc_Tcpat_NeqNull_val;case 8: _LL18: _tmp18D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp18A)->f1;_tmp18E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp18A)->f2;_LL19: {struct Cyc_Absyn_Datatypedecl*ddecl=_tmp18D;struct Cyc_Absyn_Datatypefield*df=_tmp18E;
# 878
if(ddecl->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=9,_tmp1A2->f1=ddecl,_tmp1A2->f2=df;_tmp1A2;});
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->tag=7,({int _tmp502=(int)Cyc_Tcpat_datatype_tag_number(ddecl,df->name);_tmp1A3->f1=_tmp502;}),_tmp1A3->f2=ddecl,_tmp1A3->f3=df;_tmp1A3;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp18A)->f1 != 0){_LL1A: _tmp18B=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp18A)->f1;_tmp18C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp18A)->f3;_LL1B: {union Cyc_Absyn_AggrInfo info=_tmp18B;struct Cyc_List_List*dlps=_tmp18C;
# 882
struct Cyc_Absyn_Aggrdecl*_tmp1A4=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A4;
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FA;_tmp3FA.tag=0,({struct _fat_ptr _tmp503=({const char*_tmp1A7="non-tagged aggregate in pattern test";_tag_fat(_tmp1A7,sizeof(char),37U);});_tmp3FA.f1=_tmp503;});_tmp3FA;});void*_tmp1A5[1];_tmp1A5[0]=& _tmp1A6;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1A5,sizeof(void*),1));});{
void*_tmp1A8=(void*)((struct Cyc_List_List*)_check_null((*((struct _tuple16*)((struct Cyc_List_List*)_check_null(dlps))->hd)).f1))->hd;void*_stmttmp1D=_tmp1A8;void*_tmp1A9=_stmttmp1D;struct _fat_ptr*_tmp1AA;if(*((int*)_tmp1A9)== 1){_LL1F: _tmp1AA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1A9)->f1;_LL20: {struct _fat_ptr*f=_tmp1AA;
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->tag=8,_tmp1AB->f1=f,({int _tmp504=Cyc_Tcpat_get_member_offset(ad,f);_tmp1AB->f2=_tmp504;});_tmp1AB;});}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FB;_tmp3FB.tag=0,({struct _fat_ptr _tmp505=({const char*_tmp1AE="no field name in tagged union pattern";_tag_fat(_tmp1AE,sizeof(char),38U);});_tmp3FB.f1=_tmp505;});_tmp3FB;});void*_tmp1AC[1];_tmp1AC[0]=& _tmp1AD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1AC,sizeof(void*),1));});}_LL1E:;}}}else{goto _LL1C;}default: _LL1C: _LL1D:
# 889
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FC;_tmp3FC.tag=0,({struct _fat_ptr _tmp506=({const char*_tmp1B1="non-test pattern in pattern test";_tag_fat(_tmp1B1,sizeof(char),33U);});_tmp3FC.f1=_tmp506;});_tmp3FC;});void*_tmp1AF[1];_tmp1AF[0]=& _tmp1B0;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1AF,sizeof(void*),1));});}_LL5:;}}_LL0:;}
# 894
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp3FD;(_tmp3FD.pattern).tag=1U,(_tmp3FD.pattern).val=p;_tmp3FD;});}
# 898
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));({union Cyc_Tcpat_Name_value _tmp508=Cyc_Tcpat_Name_v(({const char*_tmp1B2="NULL";_tag_fat(_tmp1B2,sizeof(char),5U);}));_tmp1B3->name=_tmp508;}),_tmp1B3->arity=0,_tmp1B3->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp507=Cyc_Tcpat_pw(p);_tmp1B3->orig_pat=_tmp507;});_tmp1B3;});}
# 901
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));({union Cyc_Tcpat_Name_value _tmp50A=Cyc_Tcpat_Name_v(({const char*_tmp1B4="&";_tag_fat(_tmp1B4,sizeof(char),2U);}));_tmp1B5->name=_tmp50A;}),_tmp1B5->arity=1,_tmp1B5->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp509=Cyc_Tcpat_pw(p);_tmp1B5->orig_pat=_tmp509;});_tmp1B5;});}
# 904
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));({union Cyc_Tcpat_Name_value _tmp50C=Cyc_Tcpat_Name_v(({const char*_tmp1B6="&";_tag_fat(_tmp1B6,sizeof(char),2U);}));_tmp1B7->name=_tmp50C;}),_tmp1B7->arity=1,_tmp1B7->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _tmp50B=Cyc_Tcpat_pw(p);_tmp1B7->orig_pat=_tmp50B;});_tmp1B7;});}
# 907
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));({union Cyc_Tcpat_Name_value _tmp50D=Cyc_Tcpat_Int_v(i);_tmp1B8->name=_tmp50D;}),_tmp1B8->arity=0,_tmp1B8->span=0,_tmp1B8->orig_pat=p;_tmp1B8;});}
# 910
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({union Cyc_Tcpat_Name_value _tmp50F=Cyc_Tcpat_Name_v(f);_tmp1B9->name=_tmp50F;}),_tmp1B9->arity=0,_tmp1B9->span=0,({union Cyc_Tcpat_PatOrWhere _tmp50E=Cyc_Tcpat_pw(p);_tmp1B9->orig_pat=_tmp50E;});_tmp1B9;});}
# 913
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));({union Cyc_Tcpat_Name_value _tmp511=Cyc_Tcpat_Int_v((int)c);_tmp1BA->name=_tmp511;}),_tmp1BA->arity=0,_tmp1BA->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _tmp510=Cyc_Tcpat_pw(p);_tmp1BA->orig_pat=_tmp510;});_tmp1BA;});}
# 916
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));({union Cyc_Tcpat_Name_value _tmp512=Cyc_Tcpat_Name_v(({const char*_tmp1BB="$";_tag_fat(_tmp1BB,sizeof(char),2U);}));_tmp1BC->name=_tmp512;}),_tmp1BC->arity=i,_tmp1BC->span=& Cyc_Tcpat_one_opt,_tmp1BC->orig_pat=p;_tmp1BC;});}
# 921
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->tag=1,({struct Cyc_Tcpat_Con_s*_tmp513=Cyc_Tcpat_null_con(p);_tmp1BD->f1=_tmp513;}),_tmp1BD->f2=0;_tmp1BD;});}
# 924
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->tag=1,({struct Cyc_Tcpat_Con_s*_tmp514=Cyc_Tcpat_int_con(i,p);_tmp1BE->f1=_tmp514;}),_tmp1BE->f2=0;_tmp1BE;});}
# 927
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->tag=1,({struct Cyc_Tcpat_Con_s*_tmp515=Cyc_Tcpat_char_con(c,p);_tmp1BF->f1=_tmp515;}),_tmp1BF->f2=0;_tmp1BF;});}
# 930
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->tag=1,({struct Cyc_Tcpat_Con_s*_tmp516=Cyc_Tcpat_float_con(f,p);_tmp1C0->f1=_tmp516;}),_tmp1C0->f2=0;_tmp1C0;});}
# 933
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->tag=1,({struct Cyc_Tcpat_Con_s*_tmp518=Cyc_Tcpat_null_ptr_con(p0);_tmp1C2->f1=_tmp518;}),({struct Cyc_List_List*_tmp517=({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->hd=p,_tmp1C1->tl=0;_tmp1C1;});_tmp1C2->f2=_tmp517;});_tmp1C2;});}
# 936
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->tag=1,({struct Cyc_Tcpat_Con_s*_tmp51A=Cyc_Tcpat_ptr_con(p0);_tmp1C4->f1=_tmp51A;}),({struct Cyc_List_List*_tmp519=({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=p,_tmp1C3->tl=0;_tmp1C3;});_tmp1C4->f2=_tmp519;});_tmp1C4;});}
# 939
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->tag=1,({struct Cyc_Tcpat_Con_s*_tmp51C=({int _tmp51B=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp51B,p);});_tmp1C5->f1=_tmp51C;}),_tmp1C5->f2=ss;_tmp1C5;});}
# 942
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 944
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({union Cyc_Tcpat_Name_value _tmp51F=Cyc_Tcpat_Name_v(con_name);_tmp1C7->name=_tmp51F;}),({int _tmp51E=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ps);_tmp1C7->arity=_tmp51E;}),_tmp1C7->span=span,({union Cyc_Tcpat_PatOrWhere _tmp51D=Cyc_Tcpat_pw(p);_tmp1C7->orig_pat=_tmp51D;});_tmp1C7;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->tag=1,_tmp1C6->f1=c,_tmp1C6->f2=ps;_tmp1C6;});}
# 949
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1C8=p->r;void*_stmttmp1E=_tmp1C8;void*_tmp1C9=_stmttmp1E;struct Cyc_Absyn_Enumfield*_tmp1CA;struct Cyc_Absyn_Enumfield*_tmp1CB;struct Cyc_List_List*_tmp1CD;struct Cyc_Absyn_Aggrdecl*_tmp1CC;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*_tmp1D1;struct Cyc_Absyn_Datatypefield*_tmp1D0;struct Cyc_Absyn_Datatypedecl*_tmp1CF;struct Cyc_Absyn_Pat*_tmp1D2;struct Cyc_Absyn_Pat*_tmp1D3;struct Cyc_Absyn_Pat*_tmp1D4;struct _fat_ptr _tmp1D5;char _tmp1D6;int _tmp1D8;enum Cyc_Absyn_Sign _tmp1D7;switch(*((int*)_tmp1C9)){case 0: _LL1: _LL2:
 goto _LL4;case 2: _LL3: _LL4:
 goto _LL6;case 4: _LL5: _LL6:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->tag=0;_tmp1D9;});goto _LL0;case 9: _LL7: _LL8:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 10: _LL9: _tmp1D7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1;_tmp1D8=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C9)->f2;_LLA: {enum Cyc_Absyn_Sign sn=_tmp1D7;int i=_tmp1D8;
s=({int _tmp520=i;Cyc_Tcpat_int_pat(_tmp520,Cyc_Tcpat_pw(p));});goto _LL0;}case 11: _LLB: _tmp1D6=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1;_LLC: {char c=_tmp1D6;
s=Cyc_Tcpat_char_pat(c,p);goto _LL0;}case 12: _LLD: _tmp1D5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1;_LLE: {struct _fat_ptr f=_tmp1D5;
s=Cyc_Tcpat_float_pat(f,p);goto _LL0;}case 1: _LLF: _tmp1D4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C9)->f2;_LL10: {struct Cyc_Absyn_Pat*p2=_tmp1D4;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 3: _LL11: _tmp1D3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C9)->f2;_LL12: {struct Cyc_Absyn_Pat*p2=_tmp1D3;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 6: _LL13: _tmp1D2=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1;_LL14: {struct Cyc_Absyn_Pat*pp=_tmp1D2;
# 962
{void*_tmp1DA=Cyc_Absyn_compress((void*)_check_null(p->topt));void*_stmttmp1F=_tmp1DA;void*_tmp1DB=_stmttmp1F;void*_tmp1DC;if(*((int*)_tmp1DB)== 3){_LL28: _tmp1DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DB)->f1).ptr_atts).nullable;_LL29: {void*n=_tmp1DC;
# 964
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
void*ss=Cyc_Tcpat_compile_pat(pp);
s=is_nullable?Cyc_Tcpat_null_ptr_pat(ss,p): Cyc_Tcpat_ptr_pat(ss,p);
goto _LL27;}}else{_LL2A: _LL2B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FE;_tmp3FE.tag=0,({struct _fat_ptr _tmp521=({const char*_tmp1DF="expecting pointertype for pattern!";_tag_fat(_tmp1DF,sizeof(char),35U);});_tmp3FE.f1=_tmp521;});_tmp3FE;});void*_tmp1DD[1];_tmp1DD[0]=& _tmp1DE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1DD,sizeof(void*),1));});}_LL27:;}
# 970
goto _LL0;}case 8: _LL15: _tmp1CF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1;_tmp1D0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C9)->f2;_tmp1D1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C9)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1CF;struct Cyc_Absyn_Datatypefield*tuf=_tmp1D0;struct Cyc_List_List*ps=_tmp1D1;
# 972
int*span;
{void*_tmp1E0=Cyc_Absyn_compress((void*)_check_null(p->topt));void*_stmttmp20=_tmp1E0;void*_tmp1E1=_stmttmp20;if(*((int*)_tmp1E1)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E1)->f1)){case 18: _LL2D: _LL2E:
# 975
 span=tud->is_extensible?0:({int*_tmp1E2=_cycalloc_atomic(sizeof(*_tmp1E2));({int _tmp522=((int(*)(struct Cyc_List_List*))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v);*_tmp1E2=_tmp522;});_tmp1E2;});
goto _LL2C;case 19: _LL2F: _LL30:
 span=& Cyc_Tcpat_one_opt;goto _LL2C;default: goto _LL31;}else{_LL31: _LL32:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FF;_tmp3FF.tag=0,({struct _fat_ptr _tmp523=({const char*_tmp1E5="void datatype pattern has bad type";_tag_fat(_tmp1E5,sizeof(char),35U);});_tmp3FF.f1=_tmp523;});_tmp3FF;});void*_tmp1E3[1];_tmp1E3[0]=& _tmp1E4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1E3,sizeof(void*),1));});}_LL2C:;}
# 980
s=({struct _fat_ptr _tmp526=*(*tuf->name).f2;int*_tmp525=span;struct Cyc_List_List*_tmp524=((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_con_pat(_tmp526,_tmp525,_tmp524,p);});
goto _LL0;}case 5: _LL17: _tmp1CE=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1;_LL18: {struct Cyc_List_List*ps=_tmp1CE;
# 983
s=({struct Cyc_List_List*_tmp527=((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_tuple_pat(_tmp527,Cyc_Tcpat_pw(p));});goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1)->KnownAggr).tag == 2){_LL19: _tmp1CC=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C9)->f1)->KnownAggr).val;_tmp1CD=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C9)->f3;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1CC;struct Cyc_List_List*dlps=_tmp1CD;
# 988
if((int)ad->kind == (int)Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 992
int found=({struct _fat_ptr _tmp528=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp528,({const char*_tmp1F3="";_tag_fat(_tmp1F3,sizeof(char),1U);}));})== 0;
{struct Cyc_List_List*dlps0=dlps;for(0;!found && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple16*_tmp1E6=(struct _tuple16*)dlps0->hd;struct _tuple16*_stmttmp21=_tmp1E6;struct _tuple16*_tmp1E7=_stmttmp21;struct Cyc_Absyn_Pat*_tmp1E9;struct Cyc_List_List*_tmp1E8;_LL34: _tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E7->f2;_LL35: {struct Cyc_List_List*dl=_tmp1E8;struct Cyc_Absyn_Pat*p=_tmp1E9;
struct Cyc_List_List*_tmp1EA=dl;struct _fat_ptr*_tmp1EB;if(_tmp1EA != 0){if(*((int*)((struct Cyc_List_List*)_tmp1EA)->hd)== 1){if(((struct Cyc_List_List*)_tmp1EA)->tl == 0){_LL37: _tmp1EB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1EA->hd)->f1;_LL38: {struct _fat_ptr*f=_tmp1EB;
# 997
if(Cyc_strptrcmp(f,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));({void*_tmp529=Cyc_Tcpat_compile_pat(p);_tmp1EC->hd=_tmp529;}),_tmp1EC->tl=ps;_tmp1EC;});
found=1;}
# 1001
goto _LL36;}}else{goto _LL39;}}else{goto _LL39;}}else{_LL39: _LL3A:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp400;_tmp400.tag=0,({struct _fat_ptr _tmp52A=({const char*_tmp1EF="bad designator(s)";_tag_fat(_tmp1EF,sizeof(char),18U);});_tmp400.f1=_tmp52A;});_tmp400;});void*_tmp1ED[1];_tmp1ED[0]=& _tmp1EE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1ED,sizeof(void*),1));});}_LL36:;}}}
# 1005
if(!found)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp401;_tmp401.tag=0,({struct _fat_ptr _tmp52B=({const char*_tmp1F2="bad designator";_tag_fat(_tmp1F2,sizeof(char),15U);});_tmp401.f1=_tmp52B;});_tmp401;});void*_tmp1F0[1];_tmp1F0[0]=& _tmp1F1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1F0,sizeof(void*),1));});}}
# 1008
s=({struct Cyc_List_List*_tmp52C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp52C,Cyc_Tcpat_pw(p));});}else{
# 1011
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp402;_tmp402.tag=0,({struct _fat_ptr _tmp52D=({const char*_tmp1F6="patterns on untagged unions not yet supported.";_tag_fat(_tmp1F6,sizeof(char),47U);});_tmp402.f1=_tmp52D;});_tmp402;});void*_tmp1F4[1];_tmp1F4[0]=& _tmp1F5;({unsigned _tmp52E=p->loc;Cyc_Warn_err2(_tmp52E,_tag_fat(_tmp1F4,sizeof(void*),1));});});{
int*span=({int*_tmp1FE=_cycalloc_atomic(sizeof(*_tmp1FE));({int _tmp52F=((int(*)(struct Cyc_List_List*))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);*_tmp1FE=_tmp52F;});_tmp1FE;});
struct Cyc_List_List*_tmp1F7=dlps;struct Cyc_Absyn_Pat*_tmp1F9;struct _fat_ptr*_tmp1F8;if(_tmp1F7 != 0){if(((struct _tuple16*)((struct Cyc_List_List*)_tmp1F7)->hd)->f1 != 0){if(*((int*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp1F7)->hd)->f1)->hd)== 1){if(((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp1F7)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp1F7)->tl == 0){_LL3C: _tmp1F8=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple16*)_tmp1F7->hd)->f1)->hd)->f1;_tmp1F9=((struct _tuple16*)_tmp1F7->hd)->f2;_LL3D: {struct _fat_ptr*f=_tmp1F8;struct Cyc_Absyn_Pat*p2=_tmp1F9;
# 1016
s=({struct _fat_ptr _tmp533=*f;int*_tmp532=span;struct Cyc_List_List*_tmp531=({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));({void*_tmp530=Cyc_Tcpat_compile_pat(p2);_tmp1FA->hd=_tmp530;}),_tmp1FA->tl=0;_tmp1FA;});Cyc_Tcpat_con_pat(_tmp533,_tmp532,_tmp531,p);});
goto _LL3B;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{_LL3E: _LL3F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp403;_tmp403.tag=0,({struct _fat_ptr _tmp534=({const char*_tmp1FD="bad union pattern";_tag_fat(_tmp1FD,sizeof(char),18U);});_tmp403.f1=_tmp534;});_tmp403;});void*_tmp1FB[1];_tmp1FB[0]=& _tmp1FC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1FB,sizeof(void*),1));});}_LL3B:;}}
# 1021
goto _LL0;}}else{goto _LL23;}}else{_LL23: _LL24:
# 1033
 goto _LL26;}case 13: _LL1B: _tmp1CB=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1C9)->f2;_LL1C: {struct Cyc_Absyn_Enumfield*ef=_tmp1CB;
# 1028
_tmp1CA=ef;goto _LL1E;}case 14: _LL1D: _tmp1CA=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1C9)->f2;_LL1E: {struct Cyc_Absyn_Enumfield*ef=_tmp1CA;
s=Cyc_Tcpat_con_pat(*(*ef->name).f2,0,0,p);goto _LL0;}case 15: _LL1F: _LL20:
# 1031
 goto _LL22;case 16: _LL21: _LL22:
 goto _LL24;default: _LL25: _LL26:
# 1034
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->tag=0;_tmp1FF;});}_LL0:;}
# 1036
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1061
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp200=({struct _tuple0 _tmp404;_tmp404.f1=a1,_tmp404.f2=a2;_tmp404;});struct _tuple0 _stmttmp22=_tmp200;struct _tuple0 _tmp201=_stmttmp22;struct _fat_ptr*_tmp205;int _tmp204;struct _fat_ptr*_tmp203;int _tmp202;unsigned _tmp209;struct Cyc_Absyn_Datatypefield*_tmp208;unsigned _tmp207;struct Cyc_Absyn_Datatypefield*_tmp206;unsigned _tmp20B;unsigned _tmp20A;switch(*((int*)_tmp201.f1)){case 0: if(*((int*)_tmp201.f2)== 0){_LL1: _LL2:
 return 1;}else{goto _LLB;}case 1: if(*((int*)_tmp201.f2)== 1){_LL3: _LL4:
 return 1;}else{goto _LLB;}case 2: if(*((int*)_tmp201.f2)== 2){_LL5: _tmp20A=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp201.f1)->f1;_tmp20B=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp201.f2)->f1;_LL6: {unsigned i1=_tmp20A;unsigned i2=_tmp20B;
return i1 == i2;}}else{goto _LLB;}case 3: if(*((int*)_tmp201.f2)== 3){_LL7: _tmp206=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp201.f1)->f2;_tmp207=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp201.f1)->f3;_tmp208=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp201.f2)->f2;_tmp209=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp201.f2)->f3;_LL8: {struct Cyc_Absyn_Datatypefield*df1=_tmp206;unsigned i1=_tmp207;struct Cyc_Absyn_Datatypefield*df2=_tmp208;unsigned i2=_tmp209;
# 1067
return df1 == df2 && i1 == i2;}}else{goto _LLB;}default: if(*((int*)_tmp201.f2)== 4){_LL9: _tmp202=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp201.f1)->f1;_tmp203=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp201.f1)->f2;_tmp204=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp201.f2)->f1;_tmp205=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp201.f2)->f2;_LLA: {int b1=_tmp202;struct _fat_ptr*f1=_tmp203;int b2=_tmp204;struct _fat_ptr*f2=_tmp205;
# 1069
return b1 == b2 && Cyc_strptrcmp(f1,f2)== 0;}}else{_LLB: _LLC:
 return 0;}}_LL0:;}
# 1074
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
for(1;p1 != 0 && p2 != 0;(p1=p1->tl,p2=p2->tl)){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;}
if(p1 != p2)return 0;
return 1;}
# 1081
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp20C=Cyc_Tcpat_get_pat_test(con->orig_pat);void*test=_tmp20C;
void*_tmp20D=d2;void*_tmp210;struct Cyc_List_List*_tmp20F;struct Cyc_List_List*_tmp20E;if(*((int*)_tmp20D)== 2){_LL1: _tmp20E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp20D)->f1;_tmp20F=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp20D)->f2;_tmp210=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp20D)->f3;_LL2: {struct Cyc_List_List*access2=_tmp20E;struct Cyc_List_List*switch_clauses=_tmp20F;void*default_decision=_tmp210;
# 1085
if(Cyc_Tcpat_same_path(access,access2)&&(int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->tag=2,_tmp213->f1=access2,({
struct Cyc_List_List*_tmp536=({struct Cyc_List_List*_tmp212=_cycalloc(sizeof(*_tmp212));({struct _tuple0*_tmp535=({struct _tuple0*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->f1=test,_tmp211->f2=d1;_tmp211;});_tmp212->hd=_tmp535;}),_tmp212->tl=switch_clauses;_tmp212;});_tmp213->f2=_tmp536;}),_tmp213->f3=default_decision;_tmp213;});
# 1089
goto _LL4;}}else{_LL3: _LL4:
 return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->tag=2,_tmp216->f1=access,({struct Cyc_List_List*_tmp538=({struct Cyc_List_List*_tmp215=_cycalloc(sizeof(*_tmp215));({struct _tuple0*_tmp537=({struct _tuple0*_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214->f1=test,_tmp214->f2=d1;_tmp214;});_tmp215->hd=_tmp537;}),_tmp215->tl=0;_tmp215;});_tmp216->f2=_tmp538;}),_tmp216->f3=d2;_tmp216;});}_LL0:;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1102
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1105
static void Cyc_Tcpat_print_tab(int i){
for(1;i != 0;-- i){
({void*_tmp217=0U;({struct Cyc___cycFILE*_tmp53A=Cyc_stderr;struct _fat_ptr _tmp539=({const char*_tmp218=" ";_tag_fat(_tmp218,sizeof(char),2U);});Cyc_fprintf(_tmp53A,_tmp539,_tag_fat(_tmp217,sizeof(void*),0));});});}}
# 1109
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp219=c->name;union Cyc_Tcpat_Name_value _stmttmp23=_tmp219;union Cyc_Tcpat_Name_value _tmp21A=_stmttmp23;int _tmp21B;struct _fat_ptr _tmp21C;if((_tmp21A.Name_v).tag == 1){_LL1: _tmp21C=(_tmp21A.Name_v).val;_LL2: {struct _fat_ptr s=_tmp21C;
({struct Cyc_String_pa_PrintArg_struct _tmp21F=({struct Cyc_String_pa_PrintArg_struct _tmp405;_tmp405.tag=0,_tmp405.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp405;});void*_tmp21D[1];_tmp21D[0]=& _tmp21F;({struct Cyc___cycFILE*_tmp53C=Cyc_stderr;struct _fat_ptr _tmp53B=({const char*_tmp21E="%s";_tag_fat(_tmp21E,sizeof(char),3U);});Cyc_fprintf(_tmp53C,_tmp53B,_tag_fat(_tmp21D,sizeof(void*),1));});});goto _LL0;}}else{_LL3: _tmp21B=(_tmp21A.Int_v).val;_LL4: {int i=_tmp21B;
({struct Cyc_Int_pa_PrintArg_struct _tmp222=({struct Cyc_Int_pa_PrintArg_struct _tmp406;_tmp406.tag=1,_tmp406.f1=(unsigned long)i;_tmp406;});void*_tmp220[1];_tmp220[0]=& _tmp222;({struct Cyc___cycFILE*_tmp53E=Cyc_stderr;struct _fat_ptr _tmp53D=({const char*_tmp221="%d";_tag_fat(_tmp221,sizeof(char),3U);});Cyc_fprintf(_tmp53E,_tmp53D,_tag_fat(_tmp220,sizeof(void*),1));});});goto _LL0;}}_LL0:;}
# 1116
static void Cyc_Tcpat_print_access(void*a){
void*_tmp223=a;struct _fat_ptr*_tmp225;int _tmp224;unsigned _tmp227;struct Cyc_Absyn_Datatypefield*_tmp226;unsigned _tmp228;switch(*((int*)_tmp223)){case 0: _LL1: _LL2:
({void*_tmp229=0U;({struct Cyc___cycFILE*_tmp540=Cyc_stderr;struct _fat_ptr _tmp53F=({const char*_tmp22A="DUMMY";_tag_fat(_tmp22A,sizeof(char),6U);});Cyc_fprintf(_tmp540,_tmp53F,_tag_fat(_tmp229,sizeof(void*),0));});});goto _LL0;case 1: _LL3: _LL4:
({void*_tmp22B=0U;({struct Cyc___cycFILE*_tmp542=Cyc_stderr;struct _fat_ptr _tmp541=({const char*_tmp22C="*";_tag_fat(_tmp22C,sizeof(char),2U);});Cyc_fprintf(_tmp542,_tmp541,_tag_fat(_tmp22B,sizeof(void*),0));});});goto _LL0;case 2: _LL5: _tmp228=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp223)->f1;_LL6: {unsigned i=_tmp228;
({struct Cyc_Int_pa_PrintArg_struct _tmp22F=({struct Cyc_Int_pa_PrintArg_struct _tmp407;_tmp407.tag=1,_tmp407.f1=(unsigned long)((int)i);_tmp407;});void*_tmp22D[1];_tmp22D[0]=& _tmp22F;({struct Cyc___cycFILE*_tmp544=Cyc_stderr;struct _fat_ptr _tmp543=({const char*_tmp22E="[%d]";_tag_fat(_tmp22E,sizeof(char),5U);});Cyc_fprintf(_tmp544,_tmp543,_tag_fat(_tmp22D,sizeof(void*),1));});});goto _LL0;}case 3: _LL7: _tmp226=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp223)->f2;_tmp227=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp223)->f3;_LL8: {struct Cyc_Absyn_Datatypefield*f=_tmp226;unsigned i=_tmp227;
# 1122
({struct Cyc_String_pa_PrintArg_struct _tmp232=({struct Cyc_String_pa_PrintArg_struct _tmp409;_tmp409.tag=0,({struct _fat_ptr _tmp545=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp409.f1=_tmp545;});_tmp409;});struct Cyc_Int_pa_PrintArg_struct _tmp233=({struct Cyc_Int_pa_PrintArg_struct _tmp408;_tmp408.tag=1,_tmp408.f1=(unsigned long)((int)i);_tmp408;});void*_tmp230[2];_tmp230[0]=& _tmp232,_tmp230[1]=& _tmp233;({struct Cyc___cycFILE*_tmp547=Cyc_stderr;struct _fat_ptr _tmp546=({const char*_tmp231="%s[%d]";_tag_fat(_tmp231,sizeof(char),7U);});Cyc_fprintf(_tmp547,_tmp546,_tag_fat(_tmp230,sizeof(void*),2));});});goto _LL0;}default: _LL9: _tmp224=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp223)->f1;_tmp225=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp223)->f2;_LLA: {int tagged=_tmp224;struct _fat_ptr*f=_tmp225;
# 1124
if(tagged)
({void*_tmp234=0U;({struct Cyc___cycFILE*_tmp549=Cyc_stderr;struct _fat_ptr _tmp548=({const char*_tmp235=".tagunion";_tag_fat(_tmp235,sizeof(char),10U);});Cyc_fprintf(_tmp549,_tmp548,_tag_fat(_tmp234,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp238=({struct Cyc_String_pa_PrintArg_struct _tmp40A;_tmp40A.tag=0,_tmp40A.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp40A;});void*_tmp236[1];_tmp236[0]=& _tmp238;({struct Cyc___cycFILE*_tmp54B=Cyc_stderr;struct _fat_ptr _tmp54A=({const char*_tmp237=".%s";_tag_fat(_tmp237,sizeof(char),4U);});Cyc_fprintf(_tmp54B,_tmp54A,_tag_fat(_tmp236,sizeof(void*),1));});});
goto _LL0;}}_LL0:;}
# 1131
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp239=p;struct Cyc_Absyn_Datatypefield*_tmp23A;int _tmp23C;struct _fat_ptr*_tmp23B;int _tmp23D;unsigned _tmp23E;struct _fat_ptr _tmp23F;struct Cyc_Absyn_Enumfield*_tmp240;struct Cyc_Absyn_Enumfield*_tmp241;struct Cyc_Absyn_Exp*_tmp242;switch(*((int*)_tmp239)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp239)->f1 == 0){_LL1: _LL2:
({void*_tmp243=0U;({struct Cyc___cycFILE*_tmp54D=Cyc_stderr;struct _fat_ptr _tmp54C=({const char*_tmp244="where(NULL)";_tag_fat(_tmp244,sizeof(char),12U);});Cyc_fprintf(_tmp54D,_tmp54C,_tag_fat(_tmp243,sizeof(void*),0));});});goto _LL0;}else{_LL3: _tmp242=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp239)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp242;
({struct Cyc_String_pa_PrintArg_struct _tmp247=({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0,({struct _fat_ptr _tmp54E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(e)));_tmp40B.f1=_tmp54E;});_tmp40B;});void*_tmp245[1];_tmp245[0]=& _tmp247;({struct Cyc___cycFILE*_tmp550=Cyc_stderr;struct _fat_ptr _tmp54F=({const char*_tmp246="where(%s)";_tag_fat(_tmp246,sizeof(char),10U);});Cyc_fprintf(_tmp550,_tmp54F,_tag_fat(_tmp245,sizeof(void*),1));});});goto _LL0;}}case 1: _LL5: _LL6:
({void*_tmp248=0U;({struct Cyc___cycFILE*_tmp552=Cyc_stderr;struct _fat_ptr _tmp551=({const char*_tmp249="NULL";_tag_fat(_tmp249,sizeof(char),5U);});Cyc_fprintf(_tmp552,_tmp551,_tag_fat(_tmp248,sizeof(void*),0));});});goto _LL0;case 2: _LL7: _LL8:
({void*_tmp24A=0U;({struct Cyc___cycFILE*_tmp554=Cyc_stderr;struct _fat_ptr _tmp553=({const char*_tmp24B="NOT-NULL:";_tag_fat(_tmp24B,sizeof(char),10U);});Cyc_fprintf(_tmp554,_tmp553,_tag_fat(_tmp24A,sizeof(void*),0));});});goto _LL0;case 4: _LL9: _tmp241=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp239)->f2;_LLA: {struct Cyc_Absyn_Enumfield*ef=_tmp241;
_tmp240=ef;goto _LLC;}case 3: _LLB: _tmp240=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp239)->f2;_LLC: {struct Cyc_Absyn_Enumfield*ef=_tmp240;
({struct Cyc_String_pa_PrintArg_struct _tmp24E=({struct Cyc_String_pa_PrintArg_struct _tmp40C;_tmp40C.tag=0,({struct _fat_ptr _tmp555=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ef->name));_tmp40C.f1=_tmp555;});_tmp40C;});void*_tmp24C[1];_tmp24C[0]=& _tmp24E;({struct Cyc___cycFILE*_tmp557=Cyc_stderr;struct _fat_ptr _tmp556=({const char*_tmp24D="%s";_tag_fat(_tmp24D,sizeof(char),3U);});Cyc_fprintf(_tmp557,_tmp556,_tag_fat(_tmp24C,sizeof(void*),1));});});goto _LL0;}case 5: _LLD: _tmp23F=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp239)->f1;_LLE: {struct _fat_ptr s=_tmp23F;
({struct Cyc_String_pa_PrintArg_struct _tmp251=({struct Cyc_String_pa_PrintArg_struct _tmp40D;_tmp40D.tag=0,_tmp40D.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp40D;});void*_tmp24F[1];_tmp24F[0]=& _tmp251;({struct Cyc___cycFILE*_tmp559=Cyc_stderr;struct _fat_ptr _tmp558=({const char*_tmp250="%s";_tag_fat(_tmp250,sizeof(char),3U);});Cyc_fprintf(_tmp559,_tmp558,_tag_fat(_tmp24F,sizeof(void*),1));});});goto _LL0;}case 6: _LLF: _tmp23E=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp239)->f1;_LL10: {unsigned i=_tmp23E;
({struct Cyc_Int_pa_PrintArg_struct _tmp254=({struct Cyc_Int_pa_PrintArg_struct _tmp40E;_tmp40E.tag=1,_tmp40E.f1=(unsigned long)((int)i);_tmp40E;});void*_tmp252[1];_tmp252[0]=& _tmp254;({struct Cyc___cycFILE*_tmp55B=Cyc_stderr;struct _fat_ptr _tmp55A=({const char*_tmp253="%d";_tag_fat(_tmp253,sizeof(char),3U);});Cyc_fprintf(_tmp55B,_tmp55A,_tag_fat(_tmp252,sizeof(void*),1));});});goto _LL0;}case 7: _LL11: _tmp23D=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp239)->f1;_LL12: {int i=_tmp23D;
({struct Cyc_Int_pa_PrintArg_struct _tmp257=({struct Cyc_Int_pa_PrintArg_struct _tmp40F;_tmp40F.tag=1,_tmp40F.f1=(unsigned long)i;_tmp40F;});void*_tmp255[1];_tmp255[0]=& _tmp257;({struct Cyc___cycFILE*_tmp55D=Cyc_stderr;struct _fat_ptr _tmp55C=({const char*_tmp256="datatypetag(%d)";_tag_fat(_tmp256,sizeof(char),16U);});Cyc_fprintf(_tmp55D,_tmp55C,_tag_fat(_tmp255,sizeof(void*),1));});});goto _LL0;}case 8: _LL13: _tmp23B=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp239)->f1;_tmp23C=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp239)->f2;_LL14: {struct _fat_ptr*f=_tmp23B;int i=_tmp23C;
({struct Cyc_String_pa_PrintArg_struct _tmp25A=({struct Cyc_String_pa_PrintArg_struct _tmp411;_tmp411.tag=0,_tmp411.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp411;});struct Cyc_Int_pa_PrintArg_struct _tmp25B=({struct Cyc_Int_pa_PrintArg_struct _tmp410;_tmp410.tag=1,_tmp410.f1=(unsigned long)i;_tmp410;});void*_tmp258[2];_tmp258[0]=& _tmp25A,_tmp258[1]=& _tmp25B;({struct Cyc___cycFILE*_tmp55F=Cyc_stderr;struct _fat_ptr _tmp55E=({const char*_tmp259="uniontag[%s](%d)";_tag_fat(_tmp259,sizeof(char),17U);});Cyc_fprintf(_tmp55F,_tmp55E,_tag_fat(_tmp258,sizeof(void*),2));});});goto _LL0;}default: _LL15: _tmp23A=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp239)->f2;_LL16: {struct Cyc_Absyn_Datatypefield*f=_tmp23A;
# 1144
({struct Cyc_String_pa_PrintArg_struct _tmp25E=({struct Cyc_String_pa_PrintArg_struct _tmp412;_tmp412.tag=0,({struct _fat_ptr _tmp560=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp412.f1=_tmp560;});_tmp412;});void*_tmp25C[1];_tmp25C[0]=& _tmp25E;({struct Cyc___cycFILE*_tmp562=Cyc_stderr;struct _fat_ptr _tmp561=({const char*_tmp25D="datatypefield(%s)";_tag_fat(_tmp25D,sizeof(char),18U);});Cyc_fprintf(_tmp562,_tmp561,_tag_fat(_tmp25C,sizeof(void*),1));});});}}_LL0:;}
# 1148
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp261=({struct Cyc_String_pa_PrintArg_struct _tmp413;_tmp413.tag=0,({struct _fat_ptr _tmp563=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp413.f1=_tmp563;});_tmp413;});void*_tmp25F[1];_tmp25F[0]=& _tmp261;({struct Cyc___cycFILE*_tmp565=Cyc_stderr;struct _fat_ptr _tmp564=({const char*_tmp260="%s";_tag_fat(_tmp260,sizeof(char),3U);});Cyc_fprintf(_tmp565,_tmp564,_tag_fat(_tmp25F,sizeof(void*),1));});});}
# 1152
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp262=d;void*_tmp265;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp263;struct Cyc_Tcpat_Rhs*_tmp266;switch(*((int*)_tmp262)){case 1: _LL1: _tmp266=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp262)->f1;_LL2: {struct Cyc_Tcpat_Rhs*rhs=_tmp266;
# 1155
Cyc_Tcpat_print_tab(tab);
({void*_tmp267=0U;({struct Cyc___cycFILE*_tmp567=Cyc_stderr;struct _fat_ptr _tmp566=({const char*_tmp268="Success(";_tag_fat(_tmp268,sizeof(char),9U);});Cyc_fprintf(_tmp567,_tmp566,_tag_fat(_tmp267,sizeof(void*),0));});});Cyc_Tcpat_print_rhs(rhs);({void*_tmp269=0U;({struct Cyc___cycFILE*_tmp569=Cyc_stderr;struct _fat_ptr _tmp568=({const char*_tmp26A=")\n";_tag_fat(_tmp26A,sizeof(char),3U);});Cyc_fprintf(_tmp569,_tmp568,_tag_fat(_tmp269,sizeof(void*),0));});});
goto _LL0;}case 0: _LL3: _LL4:
({void*_tmp26B=0U;({struct Cyc___cycFILE*_tmp56B=Cyc_stderr;struct _fat_ptr _tmp56A=({const char*_tmp26C="Failure\n";_tag_fat(_tmp26C,sizeof(char),9U);});Cyc_fprintf(_tmp56B,_tmp56A,_tag_fat(_tmp26B,sizeof(void*),0));});});goto _LL0;default: _LL5: _tmp263=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp262)->f1;_tmp264=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp262)->f2;_tmp265=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp262)->f3;_LL6: {struct Cyc_List_List*a=_tmp263;struct Cyc_List_List*cases=_tmp264;void*def=_tmp265;
# 1160
Cyc_Tcpat_print_tab(tab);
({void*_tmp26D=0U;({struct Cyc___cycFILE*_tmp56D=Cyc_stderr;struct _fat_ptr _tmp56C=({const char*_tmp26E="Switch[";_tag_fat(_tmp26E,sizeof(char),8U);});Cyc_fprintf(_tmp56D,_tmp56C,_tag_fat(_tmp26D,sizeof(void*),0));});});
for(1;a != 0;a=a->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)a->hd)->access);
if(a->tl != 0)({void*_tmp26F=0U;({struct Cyc___cycFILE*_tmp56F=Cyc_stderr;struct _fat_ptr _tmp56E=({const char*_tmp270=",";_tag_fat(_tmp270,sizeof(char),2U);});Cyc_fprintf(_tmp56F,_tmp56E,_tag_fat(_tmp26F,sizeof(void*),0));});});}
# 1166
({void*_tmp271=0U;({struct Cyc___cycFILE*_tmp571=Cyc_stderr;struct _fat_ptr _tmp570=({const char*_tmp272="] {\n";_tag_fat(_tmp272,sizeof(char),5U);});Cyc_fprintf(_tmp571,_tmp570,_tag_fat(_tmp271,sizeof(void*),0));});});
for(1;cases != 0;cases=cases->tl){
struct _tuple0 _tmp273=*((struct _tuple0*)cases->hd);struct _tuple0 _stmttmp24=_tmp273;struct _tuple0 _tmp274=_stmttmp24;void*_tmp276;void*_tmp275;_LL8: _tmp275=_tmp274.f1;_tmp276=_tmp274.f2;_LL9: {void*pt=_tmp275;void*d=_tmp276;
Cyc_Tcpat_print_tab(tab);
({void*_tmp277=0U;({struct Cyc___cycFILE*_tmp573=Cyc_stderr;struct _fat_ptr _tmp572=({const char*_tmp278="case ";_tag_fat(_tmp278,sizeof(char),6U);});Cyc_fprintf(_tmp573,_tmp572,_tag_fat(_tmp277,sizeof(void*),0));});});
Cyc_Tcpat_print_pat_test(pt);
({void*_tmp279=0U;({struct Cyc___cycFILE*_tmp575=Cyc_stderr;struct _fat_ptr _tmp574=({const char*_tmp27A=":\n";_tag_fat(_tmp27A,sizeof(char),3U);});Cyc_fprintf(_tmp575,_tmp574,_tag_fat(_tmp279,sizeof(void*),0));});});
Cyc_Tcpat_print_dec_tree(d,tab + 7);}}
# 1175
Cyc_Tcpat_print_tab(tab);
({void*_tmp27B=0U;({struct Cyc___cycFILE*_tmp577=Cyc_stderr;struct _fat_ptr _tmp576=({const char*_tmp27C="default:\n";_tag_fat(_tmp27C,sizeof(char),10U);});Cyc_fprintf(_tmp577,_tmp576,_tag_fat(_tmp27B,sizeof(void*),0));});});
Cyc_Tcpat_print_dec_tree(def,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp27D=0U;({struct Cyc___cycFILE*_tmp579=Cyc_stderr;struct _fat_ptr _tmp578=({const char*_tmp27E="}\n";_tag_fat(_tmp27E,sizeof(char),3U);});Cyc_fprintf(_tmp579,_tmp578,_tag_fat(_tmp27D,sizeof(void*),0));});});}}_LL0:;}
# 1183
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1191
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp27F=td;struct Cyc_Set_Set*_tmp280;if(*((int*)_tmp27F)== 1){_LL1: _tmp280=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp27F)->f1;_LL2: {struct Cyc_Set_Set*cs=_tmp280;
# 1201
return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281->tag=1,({struct Cyc_Set_Set*_tmp57A=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_insert)(cs,c);_tmp281->f1=_tmp57A;});_tmp281;});}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp283=({struct Cyc_Warn_String_Warn_Warg_struct _tmp414;_tmp414.tag=0,({struct _fat_ptr _tmp57B=({const char*_tmp284="add_neg called when td is Positive";_tag_fat(_tmp284,sizeof(char),35U);});_tmp414.f1=_tmp57B;});_tmp414;});void*_tmp282[1];_tmp282[0]=& _tmp283;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp282,sizeof(void*),1));});}_LL0:;}
# 1208
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp285=td;struct Cyc_Set_Set*_tmp286;struct Cyc_Tcpat_Con_s*_tmp287;if(*((int*)_tmp285)== 0){_LL1: _tmp287=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp285)->f1;_LL2: {struct Cyc_Tcpat_Con_s*c2=_tmp287;
# 1212
return Cyc_Tcpat_compare_con(c,c2)== 0?Cyc_Tcpat_Yes: Cyc_Tcpat_No;}}else{_LL3: _tmp286=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp285)->f1;_LL4: {struct Cyc_Set_Set*cs=_tmp286;
# 1215
if(((int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member)(cs,c))return Cyc_Tcpat_No;
# 1218
if(c->span != 0 &&({int _tmp57C=*((int*)_check_null(c->span));_tmp57C == ((int(*)(struct Cyc_Set_Set*))Cyc_Set_cardinality)(cs)+ 1;}))
return Cyc_Tcpat_Yes;
# 1221
return Cyc_Tcpat_Maybe;}}_LL0:;}struct _tuple22{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1229
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp288=ctxt;struct Cyc_List_List*_tmp28B;struct Cyc_List_List*_tmp28A;struct Cyc_Tcpat_Con_s*_tmp289;if(_tmp288 == 0){_LL1: _LL2:
 return 0;}else{_LL3: _tmp289=((struct _tuple22*)_tmp288->hd)->f1;_tmp28A=((struct _tuple22*)_tmp288->hd)->f2;_tmp28B=_tmp288->tl;_LL4: {struct Cyc_Tcpat_Con_s*c=_tmp289;struct Cyc_List_List*args=_tmp28A;struct Cyc_List_List*tl=_tmp28B;
return({struct Cyc_List_List*_tmp28E=_cycalloc(sizeof(*_tmp28E));({struct _tuple22*_tmp57E=({struct _tuple22*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D->f1=c,({struct Cyc_List_List*_tmp57D=({struct Cyc_List_List*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->hd=dsc,_tmp28C->tl=args;_tmp28C;});_tmp28D->f2=_tmp57D;});_tmp28D;});_tmp28E->hd=_tmp57E;}),_tmp28E->tl=tl;_tmp28E;});}}_LL0:;}
# 1239
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp28F=ctxt;struct Cyc_List_List*_tmp292;struct Cyc_List_List*_tmp291;struct Cyc_Tcpat_Con_s*_tmp290;if(_tmp28F == 0){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp294=({struct Cyc_Warn_String_Warn_Warg_struct _tmp415;_tmp415.tag=0,({struct _fat_ptr _tmp57F=({const char*_tmp295="norm_context: empty context";_tag_fat(_tmp295,sizeof(char),28U);});_tmp415.f1=_tmp57F;});_tmp415;});void*_tmp293[1];_tmp293[0]=& _tmp294;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp293,sizeof(void*),1));});}else{_LL3: _tmp290=((struct _tuple22*)_tmp28F->hd)->f1;_tmp291=((struct _tuple22*)_tmp28F->hd)->f2;_tmp292=_tmp28F->tl;_LL4: {struct Cyc_Tcpat_Con_s*c=_tmp290;struct Cyc_List_List*args=_tmp291;struct Cyc_List_List*tl=_tmp292;
return({struct Cyc_List_List*_tmp581=tl;Cyc_Tcpat_augment(_tmp581,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->tag=0,_tmp296->f1=c,({struct Cyc_List_List*_tmp580=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(args);_tmp296->f2=_tmp580;});_tmp296;}));});}}_LL0:;}
# 1251
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1253
struct _tuple1 _tmp297=({struct _tuple1 _tmp417;_tmp417.f1=ctxt,_tmp417.f2=work;_tmp417;});struct _tuple1 _stmttmp25=_tmp297;struct _tuple1 _tmp298=_stmttmp25;struct Cyc_List_List*_tmp29D;struct Cyc_List_List*_tmp29C;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29A;struct Cyc_Tcpat_Con_s*_tmp299;if(_tmp298.f1 == 0){if(_tmp298.f2 == 0){_LL1: _LL2:
 return dsc;}else{_LL3: _LL4:
 goto _LL6;}}else{if(_tmp298.f2 == 0){_LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp416;_tmp416.tag=0,({struct _fat_ptr _tmp582=({const char*_tmp2A0="build_desc: ctxt and work don't match";_tag_fat(_tmp2A0,sizeof(char),38U);});_tmp416.f1=_tmp582;});_tmp416;});void*_tmp29E[1];_tmp29E[0]=& _tmp29F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp29E,sizeof(void*),1));});}else{_LL7: _tmp299=((struct _tuple22*)(_tmp298.f1)->hd)->f1;_tmp29A=((struct _tuple22*)(_tmp298.f1)->hd)->f2;_tmp29B=(_tmp298.f1)->tl;_tmp29C=((struct _tuple21*)(_tmp298.f2)->hd)->f3;_tmp29D=(_tmp298.f2)->tl;_LL8: {struct Cyc_Tcpat_Con_s*c=_tmp299;struct Cyc_List_List*args=_tmp29A;struct Cyc_List_List*rest=_tmp29B;struct Cyc_List_List*dargs=_tmp29C;struct Cyc_List_List*work2=_tmp29D;
# 1258
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2A1=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3->tag=0,_tmp2A3->f1=c,({struct Cyc_List_List*_tmp584=({struct Cyc_List_List*_tmp583=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(args);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp583,({struct Cyc_List_List*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->hd=dsc,_tmp2A2->tl=dargs;_tmp2A2;}));});_tmp2A3->f2=_tmp584;});_tmp2A3;});struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*td=_tmp2A1;
return Cyc_Tcpat_build_desc(rest,(void*)td,work2);}}}_LL0:;}
# 1263
static void*Cyc_Tcpat_match(void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Tcpat_Rhs*,struct Cyc_List_List*);struct _tuple23{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1268
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp2A4=allmrules;struct Cyc_List_List*_tmp2A7;struct Cyc_Tcpat_Rhs*_tmp2A6;void*_tmp2A5;if(_tmp2A4 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->tag=0,_tmp2A8->f1=dsc;_tmp2A8;});}else{_LL3: _tmp2A5=((struct _tuple23*)_tmp2A4->hd)->f1;_tmp2A6=((struct _tuple23*)_tmp2A4->hd)->f2;_tmp2A7=_tmp2A4->tl;_LL4: {void*pat1=_tmp2A5;struct Cyc_Tcpat_Rhs*rhs1=_tmp2A6;struct Cyc_List_List*rulerest=_tmp2A7;
# 1272
return Cyc_Tcpat_match(pat1,0,dsc,0,0,rhs1,rulerest);}}_LL0:;}
# 1277
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp586=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->tag=1,({struct Cyc_Set_Set*_tmp585=Cyc_Tcpat_empty_con_set();_tmp2A9->f1=_tmp585;});_tmp2A9;});Cyc_Tcpat_or_match(_tmp586,allmrules);});}
# 1284
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1287
struct Cyc_List_List*_tmp2AA=work;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2AD;struct Cyc_List_List*_tmp2AC;struct Cyc_List_List*_tmp2AB;struct Cyc_List_List*_tmp2AF;if(_tmp2AA == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->tag=1,_tmp2B0->f1=right_hand_side;_tmp2B0;});}else{if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2AA)->hd)->f1 == 0){if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2AA)->hd)->f2 == 0){if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2AA)->hd)->f3 == 0){_LL3: _tmp2AF=_tmp2AA->tl;_LL4: {struct Cyc_List_List*workr=_tmp2AF;
# 1290
return({struct Cyc_List_List*_tmp589=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp588=workr;struct Cyc_Tcpat_Rhs*_tmp587=right_hand_side;Cyc_Tcpat_and_match(_tmp589,_tmp588,_tmp587,rules);});}}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _tmp2AB=((struct _tuple21*)_tmp2AA->hd)->f1;_tmp2AC=((struct _tuple21*)_tmp2AA->hd)->f2;_tmp2AD=((struct _tuple21*)_tmp2AA->hd)->f3;_tmp2AE=_tmp2AA->tl;_LL6: {struct Cyc_List_List*pats=_tmp2AB;struct Cyc_List_List*objs=_tmp2AC;struct Cyc_List_List*dscs=_tmp2AD;struct Cyc_List_List*workr=_tmp2AE;
# 1292
if((pats == 0 || objs == 0)|| dscs == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp418;_tmp418.tag=0,({struct _fat_ptr _tmp58A=({const char*_tmp2B3="tcpat:and_match: malformed work frame";_tag_fat(_tmp2B3,sizeof(char),38U);});_tmp418.f1=_tmp58A;});_tmp418;});void*_tmp2B1[1];_tmp2B1[0]=& _tmp2B2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2B1,sizeof(void*),1));});{
struct Cyc_List_List*_tmp2B4=pats;struct Cyc_List_List*_stmttmp26=_tmp2B4;struct Cyc_List_List*_tmp2B5=_stmttmp26;struct Cyc_List_List*_tmp2B7;void*_tmp2B6;_LL8: _tmp2B6=(void*)_tmp2B5->hd;_tmp2B7=_tmp2B5->tl;_LL9: {void*pat1=_tmp2B6;struct Cyc_List_List*patr=_tmp2B7;
struct Cyc_List_List*_tmp2B8=objs;struct Cyc_List_List*_stmttmp27=_tmp2B8;struct Cyc_List_List*_tmp2B9=_stmttmp27;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BA;_LLB: _tmp2BA=(struct Cyc_List_List*)_tmp2B9->hd;_tmp2BB=_tmp2B9->tl;_LLC: {struct Cyc_List_List*obj1=_tmp2BA;struct Cyc_List_List*objr=_tmp2BB;
struct Cyc_List_List*_tmp2BC=dscs;struct Cyc_List_List*_stmttmp28=_tmp2BC;struct Cyc_List_List*_tmp2BD=_stmttmp28;struct Cyc_List_List*_tmp2BF;void*_tmp2BE;_LLE: _tmp2BE=(void*)_tmp2BD->hd;_tmp2BF=_tmp2BD->tl;_LLF: {void*dsc1=_tmp2BE;struct Cyc_List_List*dscr=_tmp2BF;
struct _tuple21*_tmp2C0=({struct _tuple21*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2->f1=patr,_tmp2C2->f2=objr,_tmp2C2->f3=dscr;_tmp2C2;});struct _tuple21*wf=_tmp2C0;
return({void*_tmp590=pat1;struct Cyc_List_List*_tmp58F=obj1;void*_tmp58E=dsc1;struct Cyc_List_List*_tmp58D=ctx;struct Cyc_List_List*_tmp58C=({struct Cyc_List_List*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1->hd=wf,_tmp2C1->tl=workr;_tmp2C1;});struct Cyc_Tcpat_Rhs*_tmp58B=right_hand_side;Cyc_Tcpat_match(_tmp590,_tmp58F,_tmp58E,_tmp58D,_tmp58C,_tmp58B,rules);});}}}}}}}_LL0:;}
# 1303
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp2C3=dsc;struct Cyc_List_List*_tmp2C4;struct Cyc_Set_Set*_tmp2C5;if(*((int*)_tmp2C3)== 1){_LL1: _tmp2C5=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2C3)->f1;_LL2: {struct Cyc_Set_Set*ncs=_tmp2C5;
# 1309
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->tag=1,({struct Cyc_Set_Set*_tmp591=Cyc_Tcpat_empty_con_set();_tmp2C8->f1=_tmp591;});_tmp2C8;});
struct Cyc_List_List*_tmp2C6=0;struct Cyc_List_List*res=_tmp2C6;
{int i=0;for(0;i < pcon->arity;++ i){
res=({struct Cyc_List_List*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7->hd=any,_tmp2C7->tl=res;_tmp2C7;});}}
return res;}}else{_LL3: _tmp2C4=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2C3)->f2;_LL4: {struct Cyc_List_List*dargs=_tmp2C4;
return dargs;}}_LL0:;}
# 1318
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp2C9=pw;struct Cyc_Absyn_Pat*_tmp2CA;if((_tmp2C9.where_clause).tag == 2){_LL1: _LL2:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL3: _tmp2CA=(_tmp2C9.pattern).val;_LL4: {struct Cyc_Absyn_Pat*p=_tmp2CA;
# 1322
void*_tmp2CB=p->r;void*_stmttmp29=_tmp2CB;void*_tmp2CC=_stmttmp29;struct Cyc_List_List*_tmp2CE;union Cyc_Absyn_AggrInfo _tmp2CD;struct Cyc_Absyn_Datatypefield*_tmp2D0;struct Cyc_Absyn_Datatypedecl*_tmp2CF;switch(*((int*)_tmp2CC)){case 6: _LL6: _LL7:
# 1324
 if(i != 0)
({struct Cyc_Int_pa_PrintArg_struct _tmp2D3=({struct Cyc_Int_pa_PrintArg_struct _tmp419;_tmp419.tag=1,_tmp419.f1=(unsigned long)i;_tmp419;});void*_tmp2D1[1];_tmp2D1[0]=& _tmp2D3;({struct _fat_ptr _tmp592=({const char*_tmp2D2="get_access on pointer pattern with offset %d\n";_tag_fat(_tmp2D2,sizeof(char),46U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp592,_tag_fat(_tmp2D1,sizeof(void*),1));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5: _LL8: _LL9:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->tag=2,_tmp2D4->f1=(unsigned)i;_tmp2D4;});case 8: _LLA: _tmp2CF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2CC)->f1;_tmp2D0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2CC)->f2;_LLB: {struct Cyc_Absyn_Datatypedecl*tud=_tmp2CF;struct Cyc_Absyn_Datatypefield*tuf=_tmp2D0;
return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->tag=3,_tmp2D5->f1=tud,_tmp2D5->f2=tuf,_tmp2D5->f3=(unsigned)i;_tmp2D5;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2CC)->f1 != 0){_LLC: _tmp2CD=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2CC)->f1;_tmp2CE=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2CC)->f3;_LLD: {union Cyc_Absyn_AggrInfo info=_tmp2CD;struct Cyc_List_List*dlps=_tmp2CE;
# 1330
struct Cyc_Absyn_Aggrdecl*_tmp2D6=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2D6;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
struct Cyc_List_List*_tmp2D7=(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(dlps))->hd)).f1;struct Cyc_List_List*_stmttmp2A=_tmp2D7;struct Cyc_List_List*_tmp2D8=_stmttmp2A;struct _fat_ptr*_tmp2D9;if(_tmp2D8 != 0){if(*((int*)((struct Cyc_List_List*)_tmp2D8)->hd)== 1){if(((struct Cyc_List_List*)_tmp2D8)->tl == 0){_LL11: _tmp2D9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D8->hd)->f1;_LL12: {struct _fat_ptr*f=_tmp2D9;
# 1334
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));_tmp2DA->tag=4,_tmp2DA->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmp2DA->f2=f;_tmp2DA;});}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _LL14:
# 1336
({struct Cyc_String_pa_PrintArg_struct _tmp2DD=({struct Cyc_String_pa_PrintArg_struct _tmp41A;_tmp41A.tag=0,({struct _fat_ptr _tmp593=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(p));_tmp41A.f1=_tmp593;});_tmp41A;});void*_tmp2DB[1];_tmp2DB[0]=& _tmp2DD;({struct _fat_ptr _tmp594=({const char*_tmp2DC="get_access on bad aggr pattern: %s";_tag_fat(_tmp2DC,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp594,_tag_fat(_tmp2DB,sizeof(void*),1));});});}_LL10:;}{
# 1338
struct Cyc_List_List*_tmp2DE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp2DE;
int _tmp2DF=i;int orig_i=_tmp2DF;
for(1;i != 0;-- i){fields=((struct Cyc_List_List*)_check_null(fields))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2E0=_cycalloc(sizeof(*_tmp2E0));_tmp2E0->tag=4,_tmp2E0->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmp2E0->f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fields))->hd)->name;_tmp2E0;});}}}else{goto _LLE;}default: _LLE: _LLF:
({struct Cyc_String_pa_PrintArg_struct _tmp2E3=({struct Cyc_String_pa_PrintArg_struct _tmp41B;_tmp41B.tag=0,({struct _fat_ptr _tmp595=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(p));_tmp41B.f1=_tmp595;});_tmp41B;});void*_tmp2E1[1];_tmp2E1[0]=& _tmp2E3;({struct _fat_ptr _tmp596=({const char*_tmp2E2="get_access on bad pattern: %s";_tag_fat(_tmp2E2,sizeof(char),30U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp596,_tag_fat(_tmp2E1,sizeof(void*),1));});});}_LL5:;}}_LL0:;}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1348
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple24*env,int i){
struct _tuple24*_tmp2E4=env;struct Cyc_Tcpat_Con_s*_tmp2E6;struct Cyc_List_List*_tmp2E5;_LL1: _tmp2E5=_tmp2E4->f1;_tmp2E6=_tmp2E4->f2;_LL2: {struct Cyc_List_List*obj=_tmp2E5;struct Cyc_Tcpat_Con_s*pcon=_tmp2E6;
void*acc=Cyc_Tcpat_get_access(pcon->orig_pat,i);
return({struct Cyc_List_List*_tmp2E8=_cycalloc(sizeof(*_tmp2E8));({struct Cyc_Tcpat_PathNode*_tmp597=({struct Cyc_Tcpat_PathNode*_tmp2E7=_cycalloc(sizeof(*_tmp2E7));_tmp2E7->orig_pat=pcon->orig_pat,_tmp2E7->access=acc;_tmp2E7;});_tmp2E8->hd=_tmp597;}),_tmp2E8->tl=obj;_tmp2E8;});}}
# 1353
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple24 _tmp2E9=({struct _tuple24 _tmp41C;_tmp41C.f1=obj,_tmp41C.f2=pcon;_tmp41C;});struct _tuple24 env=_tmp2E9;
return((struct Cyc_List_List*(*)(int,struct Cyc_List_List*(*)(struct _tuple24*,int),struct _tuple24*))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& env);}
# 1361
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1364
void*_tmp2EA=p;struct Cyc_List_List*_tmp2EC;struct Cyc_Tcpat_Con_s*_tmp2EB;if(*((int*)_tmp2EA)== 0){_LL1: _LL2:
 return({struct Cyc_List_List*_tmp59A=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp599=work;struct Cyc_Tcpat_Rhs*_tmp598=right_hand_side;Cyc_Tcpat_and_match(_tmp59A,_tmp599,_tmp598,rules);});}else{_LL3: _tmp2EB=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2EA)->f1;_tmp2EC=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2EA)->f2;_LL4: {struct Cyc_Tcpat_Con_s*pcon=_tmp2EB;struct Cyc_List_List*pargs=_tmp2EC;
# 1367
enum Cyc_Tcpat_Answer _tmp2ED=Cyc_Tcpat_static_match(pcon,dsc);enum Cyc_Tcpat_Answer _stmttmp2B=_tmp2ED;enum Cyc_Tcpat_Answer _tmp2EE=_stmttmp2B;switch(_tmp2EE){case Cyc_Tcpat_Yes: _LL6: _LL7: {
# 1369
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));({struct _tuple22*_tmp59B=({struct _tuple22*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1->f1=pcon,_tmp2F1->f2=0;_tmp2F1;});_tmp2F2->hd=_tmp59B;}),_tmp2F2->tl=ctx;_tmp2F2;});
struct _tuple21*work_frame=({struct _tuple21*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0->f1=pargs,({struct Cyc_List_List*_tmp59D=Cyc_Tcpat_getoargs(pcon,obj);_tmp2F0->f2=_tmp59D;}),({
struct Cyc_List_List*_tmp59C=Cyc_Tcpat_getdargs(pcon,dsc);_tmp2F0->f3=_tmp59C;});_tmp2F0;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF->hd=work_frame,_tmp2EF->tl=work;_tmp2EF;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL8: _LL9:
# 1375
 return({void*_tmp59E=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp59E,rules);});default: _LLA: _LLB: {
# 1377
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));({struct _tuple22*_tmp59F=({struct _tuple22*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7->f1=pcon,_tmp2F7->f2=0;_tmp2F7;});_tmp2F8->hd=_tmp59F;}),_tmp2F8->tl=ctx;_tmp2F8;});
struct _tuple21*work_frame=({struct _tuple21*_tmp2F6=_cycalloc(sizeof(*_tmp2F6));_tmp2F6->f1=pargs,({struct Cyc_List_List*_tmp5A1=Cyc_Tcpat_getoargs(pcon,obj);_tmp2F6->f2=_tmp5A1;}),({
struct Cyc_List_List*_tmp5A0=Cyc_Tcpat_getdargs(pcon,dsc);_tmp2F6->f3=_tmp5A0;});_tmp2F6;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5->hd=work_frame,_tmp2F5->tl=work;_tmp2F5;});
void*_tmp2F3=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);void*s=_tmp2F3;
void*_tmp2F4=({void*_tmp5A4=({struct Cyc_List_List*_tmp5A3=ctx;void*_tmp5A2=Cyc_Tcpat_add_neg(dsc,pcon);Cyc_Tcpat_build_desc(_tmp5A3,_tmp5A2,work);});Cyc_Tcpat_or_match(_tmp5A4,rules);});void*f=_tmp2F4;
return Cyc_Tcpat_ifeq(obj,pcon,s,f);}}_LL5:;}}_LL0:;}
# 1393
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1398
void*_tmp2F9=d;void*_tmp2FB;struct Cyc_List_List*_tmp2FA;struct Cyc_Tcpat_Rhs*_tmp2FC;void*_tmp2FD;switch(*((int*)_tmp2F9)){case 0: _LL1: _tmp2FD=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2F9)->f1;_LL2: {void*td=_tmp2FD;
# 1400
if(!(*exhaust_done)){not_exhaust(env1,td);*exhaust_done=1;}
goto _LL0;}case 1: _LL3: _tmp2FC=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2F9)->f1;_LL4: {struct Cyc_Tcpat_Rhs*r=_tmp2FC;
rhs_appears(env2,r);goto _LL0;}default: _LL5: _tmp2FA=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2F9)->f2;_tmp2FB=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2F9)->f3;_LL6: {struct Cyc_List_List*cases=_tmp2FA;void*def=_tmp2FB;
# 1404
for(1;cases != 0;cases=cases->tl){
struct _tuple0 _tmp2FE=*((struct _tuple0*)cases->hd);struct _tuple0 _stmttmp2C=_tmp2FE;struct _tuple0 _tmp2FF=_stmttmp2C;void*_tmp300;_LL8: _tmp300=_tmp2FF.f2;_LL9: {void*d=_tmp300;
Cyc_Tcpat_check_exhaust_overlap(d,not_exhaust,env1,rhs_appears,env2,exhaust_done);}}
# 1409
Cyc_Tcpat_check_exhaust_overlap(def,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1411
goto _LL0;}}_LL0:;}
# 1419
static struct _tuple23*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306->used=0,_tmp306->pat_loc=(swc->pattern)->loc,_tmp306->rhs=swc->body;_tmp306;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp301=({union Cyc_Tcpat_PatOrWhere _tmp41D;(_tmp41D.where_clause).tag=2U,(_tmp41D.where_clause).val=swc->where_clause;_tmp41D;});union Cyc_Tcpat_PatOrWhere w=_tmp301;
sp=({struct Cyc_List_List*_tmp5A6=({void*_tmp302[2];_tmp302[0]=sp0,({void*_tmp5A5=Cyc_Tcpat_int_pat(*ctr,w);_tmp302[1]=_tmp5A5;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp302,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_tmp5A6,w);});
*ctr=*ctr + 1;}else{
# 1428
sp=({struct Cyc_List_List*_tmp5A8=({void*_tmp303[2];_tmp303[0]=sp0,({void*_tmp5A7=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->tag=0;_tmp304;});_tmp303[1]=_tmp5A7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp303,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_tmp5A8,({union Cyc_Tcpat_PatOrWhere _tmp41E;(_tmp41E.where_clause).tag=2U,(_tmp41E.where_clause).val=0;_tmp41E;}));});}
return({struct _tuple23*_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305->f1=sp,_tmp305->f2=rhs;_tmp305;});}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1434
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1436
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp307=d;struct Cyc_Set_Set*_tmp308;struct Cyc_List_List*_tmp30A;struct Cyc_Tcpat_Con_s*_tmp309;if(*((int*)_tmp307)== 0){_LL1: _tmp309=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp307)->f1;_tmp30A=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp307)->f2;_LL2: {struct Cyc_Tcpat_Con_s*c=_tmp309;struct Cyc_List_List*ds=_tmp30A;
# 1440
union Cyc_Tcpat_Name_value _tmp30B=c->name;union Cyc_Tcpat_Name_value n=_tmp30B;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp30C=c->orig_pat;union Cyc_Tcpat_PatOrWhere _stmttmp2D=_tmp30C;union Cyc_Tcpat_PatOrWhere _tmp30D=_stmttmp2D;struct Cyc_Absyn_Pat*_tmp30E;if((_tmp30D.where_clause).tag == 2){_LL6: _LL7:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(ds))->hd);}else{_LL8: _tmp30E=(_tmp30D.pattern).val;_LL9: {struct Cyc_Absyn_Pat*p2=_tmp30E;
p=p2;goto _LL5;}}_LL5:;}{
# 1446
void*_tmp30F=p->r;void*_stmttmp2E=_tmp30F;void*_tmp310=_stmttmp2E;struct Cyc_Absyn_Exp*_tmp311;struct Cyc_Absyn_Enumfield*_tmp312;struct Cyc_Absyn_Enumfield*_tmp313;int _tmp315;struct _fat_ptr _tmp314;char _tmp316;int _tmp317;struct Cyc_Absyn_Datatypefield*_tmp318;struct Cyc_List_List*_tmp31A;struct Cyc_Absyn_Aggrdecl*_tmp319;struct Cyc_Absyn_Vardecl*_tmp31C;struct Cyc_Absyn_Tvar*_tmp31B;struct Cyc_Absyn_Vardecl*_tmp31D;struct Cyc_Absyn_Vardecl*_tmp31E;switch(*((int*)_tmp310)){case 0: _LLB: _LLC:
 return({const char*_tmp31F="_";_tag_fat(_tmp31F,sizeof(char),2U);});case 1: _LLD: _tmp31E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp310)->f1;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp31E;
return Cyc_Absynpp_qvar2string(vd->name);}case 3: _LLF: _tmp31D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp310)->f1;_LL10: {struct Cyc_Absyn_Vardecl*vd=_tmp31D;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp322=({struct Cyc_String_pa_PrintArg_struct _tmp41F;_tmp41F.tag=0,({struct _fat_ptr _tmp5A9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp41F.f1=_tmp5A9;});_tmp41F;});void*_tmp320[1];_tmp320[0]=& _tmp322;({struct _fat_ptr _tmp5AA=({const char*_tmp321="*%s";_tag_fat(_tmp321,sizeof(char),4U);});Cyc_aprintf(_tmp5AA,_tag_fat(_tmp320,sizeof(void*),1));});});}case 4: _LL11: _tmp31B=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp310)->f1;_tmp31C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp310)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp31B;struct Cyc_Absyn_Vardecl*vd=_tmp31C;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp325=({struct Cyc_String_pa_PrintArg_struct _tmp421;_tmp421.tag=0,({struct _fat_ptr _tmp5AB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp421.f1=_tmp5AB;});_tmp421;});struct Cyc_String_pa_PrintArg_struct _tmp326=({struct Cyc_String_pa_PrintArg_struct _tmp420;_tmp420.tag=0,_tmp420.f1=(struct _fat_ptr)((struct _fat_ptr)*tv->name);_tmp420;});void*_tmp323[2];_tmp323[0]=& _tmp325,_tmp323[1]=& _tmp326;({struct _fat_ptr _tmp5AC=({const char*_tmp324="%s<`%s>";_tag_fat(_tmp324,sizeof(char),8U);});Cyc_aprintf(_tmp5AC,_tag_fat(_tmp323,sizeof(void*),2));});});}case 5: _LL13: _LL14:
# 1452
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp329=({struct Cyc_String_pa_PrintArg_struct _tmp422;_tmp422.tag=0,({struct _fat_ptr _tmp5AD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp422.f1=_tmp5AD;});_tmp422;});void*_tmp327[1];_tmp327[0]=& _tmp329;({struct _fat_ptr _tmp5AE=({const char*_tmp328="$(%s)";_tag_fat(_tmp328,sizeof(char),6U);});Cyc_aprintf(_tmp5AE,_tag_fat(_tmp327,sizeof(void*),1));});});case 6: _LL15: _LL16:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32C=({struct Cyc_String_pa_PrintArg_struct _tmp423;_tmp423.tag=0,({struct _fat_ptr _tmp5AF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp423.f1=_tmp5AF;});_tmp423;});void*_tmp32A[1];_tmp32A[0]=& _tmp32C;({struct _fat_ptr _tmp5B0=({const char*_tmp32B="&%s";_tag_fat(_tmp32B,sizeof(char),4U);});Cyc_aprintf(_tmp5B0,_tag_fat(_tmp32A,sizeof(void*),1));});});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp310)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp310)->f1)->KnownAggr).tag == 2){_LL17: _tmp319=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp310)->f1)->KnownAggr).val;_tmp31A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp310)->f3;_LL18: {struct Cyc_Absyn_Aggrdecl*ad=_tmp319;struct Cyc_List_List*dlps=_tmp31A;
# 1455
if((int)ad->kind == (int)Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp32D=dlps;struct _fat_ptr*_tmp32E;if(_tmp32D != 0){if(((struct _tuple16*)((struct Cyc_List_List*)_tmp32D)->hd)->f1 != 0){if(*((int*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp32D)->hd)->f1)->hd)== 1){_LL2C: _tmp32E=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple16*)_tmp32D->hd)->f1)->hd)->f1;_LL2D: {struct _fat_ptr*f=_tmp32E;
# 1458
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp331=({struct Cyc_String_pa_PrintArg_struct _tmp426;_tmp426.tag=0,({struct _fat_ptr _tmp5B1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp426.f1=_tmp5B1;});_tmp426;});struct Cyc_String_pa_PrintArg_struct _tmp332=({struct Cyc_String_pa_PrintArg_struct _tmp425;_tmp425.tag=0,_tmp425.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp425;});struct Cyc_String_pa_PrintArg_struct _tmp333=({struct Cyc_String_pa_PrintArg_struct _tmp424;_tmp424.tag=0,({
struct _fat_ptr _tmp5B2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp424.f1=_tmp5B2;});_tmp424;});void*_tmp32F[3];_tmp32F[0]=& _tmp331,_tmp32F[1]=& _tmp332,_tmp32F[2]=& _tmp333;({struct _fat_ptr _tmp5B3=({const char*_tmp330="%s{.%s = %s}";_tag_fat(_tmp330,sizeof(char),13U);});Cyc_aprintf(_tmp5B3,_tag_fat(_tmp32F,sizeof(void*),3));});});}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 1462
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp336=({struct Cyc_String_pa_PrintArg_struct _tmp428;_tmp428.tag=0,({struct _fat_ptr _tmp5B4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp428.f1=_tmp5B4;});_tmp428;});struct Cyc_String_pa_PrintArg_struct _tmp337=({struct Cyc_String_pa_PrintArg_struct _tmp427;_tmp427.tag=0,({struct _fat_ptr _tmp5B5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp427.f1=_tmp5B5;});_tmp427;});void*_tmp334[2];_tmp334[0]=& _tmp336,_tmp334[1]=& _tmp337;({struct _fat_ptr _tmp5B6=({const char*_tmp335="%s{%s}";_tag_fat(_tmp335,sizeof(char),7U);});Cyc_aprintf(_tmp5B6,_tag_fat(_tmp334,sizeof(void*),2));});});}}else{goto _LL29;}}else{goto _LL29;}case 8: _LL19: _tmp318=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp310)->f2;_LL1A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp318;
# 1464
if(ds == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33A=({struct Cyc_String_pa_PrintArg_struct _tmp429;_tmp429.tag=0,({struct _fat_ptr _tmp5B7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp429.f1=_tmp5B7;});_tmp429;});void*_tmp338[1];_tmp338[0]=& _tmp33A;({struct _fat_ptr _tmp5B8=({const char*_tmp339="%s";_tag_fat(_tmp339,sizeof(char),3U);});Cyc_aprintf(_tmp5B8,_tag_fat(_tmp338,sizeof(void*),1));});});else{
# 1467
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33D=({struct Cyc_String_pa_PrintArg_struct _tmp42B;_tmp42B.tag=0,({struct _fat_ptr _tmp5B9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp42B.f1=_tmp5B9;});_tmp42B;});struct Cyc_String_pa_PrintArg_struct _tmp33E=({struct Cyc_String_pa_PrintArg_struct _tmp42A;_tmp42A.tag=0,({struct _fat_ptr _tmp5BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp42A.f1=_tmp5BA;});_tmp42A;});void*_tmp33B[2];_tmp33B[0]=& _tmp33D,_tmp33B[1]=& _tmp33E;({struct _fat_ptr _tmp5BB=({const char*_tmp33C="%s(%s)";_tag_fat(_tmp33C,sizeof(char),7U);});Cyc_aprintf(_tmp5BB,_tag_fat(_tmp33B,sizeof(void*),2));});});}}case 9: _LL1B: _LL1C:
 return({const char*_tmp33F="NULL";_tag_fat(_tmp33F,sizeof(char),5U);});case 10: _LL1D: _tmp317=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp310)->f2;_LL1E: {int i=_tmp317;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp342=({struct Cyc_Int_pa_PrintArg_struct _tmp42C;_tmp42C.tag=1,_tmp42C.f1=(unsigned long)i;_tmp42C;});void*_tmp340[1];_tmp340[0]=& _tmp342;({struct _fat_ptr _tmp5BC=({const char*_tmp341="%d";_tag_fat(_tmp341,sizeof(char),3U);});Cyc_aprintf(_tmp5BC,_tag_fat(_tmp340,sizeof(void*),1));});});}case 11: _LL1F: _tmp316=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp310)->f1;_LL20: {char c=_tmp316;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp345=({struct Cyc_Int_pa_PrintArg_struct _tmp42D;_tmp42D.tag=1,_tmp42D.f1=(unsigned long)((int)c);_tmp42D;});void*_tmp343[1];_tmp343[0]=& _tmp345;({struct _fat_ptr _tmp5BD=({const char*_tmp344="%d";_tag_fat(_tmp344,sizeof(char),3U);});Cyc_aprintf(_tmp5BD,_tag_fat(_tmp343,sizeof(void*),1));});});}case 12: _LL21: _tmp314=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp310)->f1;_tmp315=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp310)->f2;_LL22: {struct _fat_ptr f=_tmp314;int i=_tmp315;
return f;}case 13: _LL23: _tmp313=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp310)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp313;
_tmp312=ef;goto _LL26;}case 14: _LL25: _tmp312=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp310)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp312;
return Cyc_Absynpp_qvar2string(ef->name);}case 17: _LL27: _tmp311=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp310)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp311;
return Cyc_Absynpp_exp2string(e);}default: _LL29: _LL2A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LLA:;}}}else{_LL3: _tmp308=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp307)->f1;_LL4: {struct Cyc_Set_Set*s=_tmp308;
# 1478
if(((int(*)(struct Cyc_Set_Set*))Cyc_Set_is_empty)(s))return({const char*_tmp346="_";_tag_fat(_tmp346,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*_tmp347=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*))Cyc_Set_choose)(s);struct Cyc_Tcpat_Con_s*c=_tmp347;
# 1482
if((int)(((c->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp348=({union Cyc_Tcpat_PatOrWhere _tmp365=c->orig_pat;if((_tmp365.pattern).tag != 1)_throw_match();(_tmp365.pattern).val;});struct Cyc_Absyn_Pat*orig_pat=_tmp348;
void*_tmp349=Cyc_Absyn_compress((void*)_check_null(orig_pat->topt));void*_stmttmp2F=_tmp349;void*_tmp34A=_stmttmp2F;void*_tmp34B;struct Cyc_Absyn_Aggrdecl*_tmp34C;struct Cyc_Absyn_Datatypedecl*_tmp34D;switch(*((int*)_tmp34A)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A)->f1)->f2 == Cyc_Absyn_Char_sz){_LL31: _LL32:
# 1487
{int i=0;for(0;i < 256;++ i){
if(!({struct Cyc_Set_Set*_tmp5BE=s;((int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member)(_tmp5BE,Cyc_Tcpat_char_con((char)i,orig_pat));}))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp350=({struct Cyc_Int_pa_PrintArg_struct _tmp42E;_tmp42E.tag=1,_tmp42E.f1=(unsigned long)i;_tmp42E;});void*_tmp34E[1];_tmp34E[0]=& _tmp350;({struct _fat_ptr _tmp5BF=({const char*_tmp34F="%d";_tag_fat(_tmp34F,sizeof(char),3U);});Cyc_aprintf(_tmp5BF,_tag_fat(_tmp34E,sizeof(void*),1));});});}}
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL33: _LL34:
# 1493
{unsigned i=0U;for(0;i < (unsigned)-1;++ i){
if(!({struct Cyc_Set_Set*_tmp5C0=s;((int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member)(_tmp5C0,Cyc_Tcpat_int_con((int)i,c->orig_pat));}))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp353=({struct Cyc_Int_pa_PrintArg_struct _tmp42F;_tmp42F.tag=1,_tmp42F.f1=(unsigned long)((int)i);_tmp42F;});void*_tmp351[1];_tmp351[0]=& _tmp353;({struct _fat_ptr _tmp5C1=({const char*_tmp352="%d";_tag_fat(_tmp352,sizeof(char),3U);});Cyc_aprintf(_tmp5C1,_tag_fat(_tmp351,sizeof(void*),1));});});}}
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 18: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A)->f1)->f1).KnownDatatype).tag == 2){_LL37: _tmp34D=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A)->f1)->f1).KnownDatatype).val;_LL38: {struct Cyc_Absyn_Datatypedecl*tud=_tmp34D;
# 1502
if(tud->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp356=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp356;
int span=((int(*)(struct Cyc_List_List*))Cyc_List_length)(fields);
for(1;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)fields->hd)->name).f2;
struct Cyc_List_List*_tmp357=((struct Cyc_Absyn_Datatypefield*)fields->hd)->typs;struct Cyc_List_List*args=_tmp357;
if(!({struct Cyc_Set_Set*_tmp5C3=s;((int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member)(_tmp5C3,({struct Cyc_Tcpat_Con_s*_tmp358=_cycalloc(sizeof(*_tmp358));({union Cyc_Tcpat_Name_value _tmp5C2=Cyc_Tcpat_Name_v(n);_tmp358->name=_tmp5C2;}),_tmp358->arity=0,_tmp358->span=0,_tmp358->orig_pat=c->orig_pat;_tmp358;}));}))
return((int(*)(struct Cyc_List_List*))Cyc_List_length)(args)== 0?n:(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35B=({struct Cyc_String_pa_PrintArg_struct _tmp430;_tmp430.tag=0,_tmp430.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp430;});void*_tmp359[1];_tmp359[0]=& _tmp35B;({struct _fat_ptr _tmp5C4=({const char*_tmp35A="%s(...)";_tag_fat(_tmp35A,sizeof(char),8U);});Cyc_aprintf(_tmp5C4,_tag_fat(_tmp359,sizeof(void*),1));});});}
# 1511
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL3B;}case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp34C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34A)->f1)->f1).KnownAggr).val;if((int)_tmp34C->kind == (int)Cyc_Absyn_UnionA){_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp34C;
# 1513
struct Cyc_List_List*_tmp35C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp35C;
int span=((int(*)(struct Cyc_List_List*))Cyc_List_length)(fields);
struct _tuple2*_tmp35D=ad->name;struct _tuple2*_stmttmp30=_tmp35D;struct _tuple2*_tmp35E=_stmttmp30;struct _fat_ptr _tmp35F;_LL3E: _tmp35F=*_tmp35E->f2;_LL3F: {struct _fat_ptr union_name=_tmp35F;
for(1;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;
if(!({struct Cyc_Set_Set*_tmp5C6=s;((int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member)(_tmp5C6,({struct Cyc_Tcpat_Con_s*_tmp360=_cycalloc(sizeof(*_tmp360));({union Cyc_Tcpat_Name_value _tmp5C5=Cyc_Tcpat_Name_v(n);_tmp360->name=_tmp5C5;}),_tmp360->arity=0,_tmp360->span=0,_tmp360->orig_pat=c->orig_pat;_tmp360;}));}))
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp363=({struct Cyc_String_pa_PrintArg_struct _tmp432;_tmp432.tag=0,_tmp432.f1=(struct _fat_ptr)((struct _fat_ptr)union_name);_tmp432;});struct Cyc_String_pa_PrintArg_struct _tmp364=({struct Cyc_String_pa_PrintArg_struct _tmp431;_tmp431.tag=0,_tmp431.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp431;});void*_tmp361[2];_tmp361[0]=& _tmp363,_tmp361[1]=& _tmp364;({struct _fat_ptr _tmp5C7=({const char*_tmp362="%s{.%s = _}";_tag_fat(_tmp362,sizeof(char),12U);});Cyc_aprintf(_tmp5C7,_tag_fat(_tmp361,sizeof(void*),2));});});}
# 1521
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL3B;}}else{goto _LL3B;}default: goto _LL3B;}case 3: _LL35: _tmp34B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34A)->f1).ptr_atts).nullable;_LL36: {void*n=_tmp34B;
# 1498
if(Cyc_Tcutil_force_type2bool(0,n)&& !({struct Cyc_Set_Set*_tmp5C8=s;((int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member)(_tmp5C8,Cyc_Tcpat_null_con(orig_pat));}))
return({const char*_tmp354="NULL";_tag_fat(_tmp354,sizeof(char),5U);});
return({const char*_tmp355="&_";_tag_fat(_tmp355,sizeof(char),3U);});}default: _LL3B: _LL3C:
# 1522
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL30:;}}}}_LL0:;}
# 1526
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _fat_ptr*_tmp366=_cycalloc(sizeof(*_tmp366));({struct _fat_ptr _tmp5C9=Cyc_Tcpat_desc2string(d);*_tmp366=_tmp5C9;});_tmp366;});}
# 1529
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp367=((struct Cyc_List_List*(*)(struct _fat_ptr*(*)(void*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);struct Cyc_List_List*ss=_tmp367;
struct _fat_ptr*comma=({struct _fat_ptr*_tmp36B=_cycalloc(sizeof(*_tmp36B));({struct _fat_ptr _tmp5CA=({const char*_tmp36A=",";_tag_fat(_tmp36A,sizeof(char),2U);});*_tmp36B=_tmp5CA;});_tmp36B;});
{struct Cyc_List_List*_tmp368=ss;struct Cyc_List_List*x=_tmp368;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
if(x->tl != 0){
({struct Cyc_List_List*_tmp5CB=({struct Cyc_List_List*_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369->hd=comma,_tmp369->tl=x->tl;_tmp369;});x->tl=_tmp5CB;});
x=x->tl;}}}
# 1537
return(struct _fat_ptr)Cyc_strconcat_l(ss);}
# 1540
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){
struct _handler_cons _tmp36C;_push_handler(& _tmp36C);{int _tmp36E=0;if(setjmp(_tmp36C.handler))_tmp36E=1;if(!_tmp36E){
{struct _fat_ptr _tmp36F=Cyc_Tcpat_desc2string(desc);struct _fat_ptr s=_tmp36F;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp371=({struct Cyc_Warn_String_Warn_Warg_struct _tmp434;_tmp434.tag=0,({struct _fat_ptr _tmp5CC=({const char*_tmp373="patterns may not be exhaustive.\n\tmissing case for ";_tag_fat(_tmp373,sizeof(char),51U);});_tmp434.f1=_tmp5CC;});_tmp434;});struct Cyc_Warn_String_Warn_Warg_struct _tmp372=({struct Cyc_Warn_String_Warn_Warg_struct _tmp433;_tmp433.tag=0,_tmp433.f1=s;_tmp433;});void*_tmp370[2];_tmp370[0]=& _tmp371,_tmp370[1]=& _tmp372;({unsigned _tmp5CD=loc;Cyc_Warn_err2(_tmp5CD,_tag_fat(_tmp370,sizeof(void*),2));});});}
# 1542
;_pop_handler();}else{void*_tmp36D=(void*)Cyc_Core_get_exn_thrown();void*_tmp374=_tmp36D;void*_tmp375;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp374)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1545
({struct Cyc_Warn_String_Warn_Warg_struct _tmp377=({struct Cyc_Warn_String_Warn_Warg_struct _tmp435;_tmp435.tag=0,({struct _fat_ptr _tmp5CE=({const char*_tmp378="patterns may not be exhaustive.";_tag_fat(_tmp378,sizeof(char),32U);});_tmp435.f1=_tmp5CE;});_tmp435;});void*_tmp376[1];_tmp376[0]=& _tmp377;({unsigned _tmp5CF=loc;Cyc_Warn_err2(_tmp5CF,_tag_fat(_tmp376,sizeof(void*),1));});});
goto _LL0;}else{_LL3: _tmp375=_tmp374;_LL4: {void*exn=_tmp375;(int)_rethrow(exn);}}_LL0:;}}}
# 1549
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1553
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1559
int _tmp379=0;int where_ctr=_tmp379;
int*_tmp37A=& where_ctr;int*env=_tmp37A;
struct Cyc_List_List*_tmp37B=((struct Cyc_List_List*(*)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcpat_get_match,env,swcs);struct Cyc_List_List*match_rules=_tmp37B;
void*_tmp37C=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp37C;
*dopt=dec_tree;{
# 1565
int _tmp37D=0;int ex_done=_tmp37D;
((void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1568
for(1;match_rules != 0;match_rules=match_rules->tl){
struct _tuple23*_tmp37E=(struct _tuple23*)match_rules->hd;struct _tuple23*_stmttmp31=_tmp37E;struct _tuple23*_tmp37F=_stmttmp31;unsigned _tmp381;int _tmp380;_LL1: _tmp380=(_tmp37F->f2)->used;_tmp381=(_tmp37F->f2)->pat_loc;_LL2: {int b=_tmp380;unsigned loc2=_tmp381;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp383=({struct Cyc_Warn_String_Warn_Warg_struct _tmp436;_tmp436.tag=0,({struct _fat_ptr _tmp5D0=({const char*_tmp384="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp384,sizeof(char),74U);});_tmp436.f1=_tmp5D0;});_tmp436;});void*_tmp382[1];_tmp382[0]=& _tmp383;({unsigned _tmp5D1=loc2;Cyc_Warn_err2(_tmp5D1,_tag_fat(_tmp382,sizeof(void*),1));});});
# 1573
break;}}}}}
# 1582
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple15*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp385;_push_handler(& _tmp385);{int _tmp387=0;if(setjmp(_tmp385.handler))_tmp387=1;if(!_tmp387){
{struct _fat_ptr _tmp388=Cyc_Tcpat_desc2string(desc);struct _fat_ptr s=_tmp388;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp438;_tmp438.tag=0,({struct _fat_ptr _tmp5D2=({const char*_tmp38C="pattern not exhaustive.\n\tmissing case for ";_tag_fat(_tmp38C,sizeof(char),43U);});_tmp438.f1=_tmp5D2;});_tmp438;});struct Cyc_Warn_String_Warn_Warg_struct _tmp38B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp437;_tmp437.tag=0,_tmp437.f1=s;_tmp437;});void*_tmp389[2];_tmp389[0]=& _tmp38A,_tmp389[1]=& _tmp38B;({unsigned _tmp5D3=(*pr).f1;Cyc_Warn_warn2(_tmp5D3,_tag_fat(_tmp389,sizeof(void*),2));});});}
# 1585
;_pop_handler();}else{void*_tmp386=(void*)Cyc_Core_get_exn_thrown();void*_tmp38D=_tmp386;void*_tmp38E;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp38D)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1588
({struct Cyc_Warn_String_Warn_Warg_struct _tmp390=({struct Cyc_Warn_String_Warn_Warg_struct _tmp439;_tmp439.tag=0,({struct _fat_ptr _tmp5D4=({const char*_tmp391="pattern not exhaustive.";_tag_fat(_tmp391,sizeof(char),24U);});_tmp439.f1=_tmp5D4;});_tmp439;});void*_tmp38F[1];_tmp38F[0]=& _tmp390;({unsigned _tmp5D5=(*pr).f1;Cyc_Warn_warn2(_tmp5D5,_tag_fat(_tmp38F,sizeof(void*),1));});});
goto _LL0;}else{_LL3: _tmp38E=_tmp38D;_LL4: {void*exn=_tmp38E;(int)_rethrow(exn);}}_LL0:;}}}}
# 1592
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1596
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->used=0,_tmp398->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_tmp5D6=Cyc_Absyn_skip_stmt(0U);_tmp398->rhs=_tmp5D6;});_tmp398;});
struct Cyc_List_List*_tmp392=({struct Cyc_List_List*_tmp397=_cycalloc(sizeof(*_tmp397));({struct _tuple23*_tmp5D8=({struct _tuple23*_tmp396=_cycalloc(sizeof(*_tmp396));({void*_tmp5D7=Cyc_Tcpat_compile_pat(p);_tmp396->f1=_tmp5D7;}),_tmp396->f2=rhs;_tmp396;});_tmp397->hd=_tmp5D8;}),_tmp397->tl=0;_tmp397;});struct Cyc_List_List*match_rules=_tmp392;
void*_tmp393=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp393;
struct _tuple15 _tmp394=({struct _tuple15 _tmp43A;_tmp43A.f1=loc,_tmp43A.f2=1;_tmp43A;});struct _tuple15 exhaust_env=_tmp394;
int _tmp395=0;int ex_done=_tmp395;
((void(*)(void*,void(*)(struct _tuple15*,void*),struct _tuple15*,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_warn,& exhaust_env,Cyc_Tcpat_dummy_fn,0,& ex_done);
# 1604
*dopt=dec_tree;
return exhaust_env.f2;}
# 1612
static struct _tuple23*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1616
if(swc->where_clause != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp39A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp43B;_tmp43B.tag=0,({struct _fat_ptr _tmp5D9=({const char*_tmp39B="&&-clauses not supported in exception handlers.";_tag_fat(_tmp39B,sizeof(char),48U);});_tmp43B.f1=_tmp5D9;});_tmp43B;});void*_tmp399[1];_tmp399[0]=& _tmp39A;({unsigned _tmp5DA=((struct Cyc_Absyn_Exp*)_check_null(swc->where_clause))->loc;Cyc_Warn_err2(_tmp5DA,_tag_fat(_tmp399,sizeof(void*),1));});});{
# 1619
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->used=0,_tmp39D->pat_loc=(swc->pattern)->loc,_tmp39D->rhs=swc->body;_tmp39D;});
return({struct _tuple23*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->f1=sp0,_tmp39C->f2=rhs;_tmp39C;});}}
# 1622
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1625
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1629
struct Cyc_List_List*_tmp39E=((struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);struct Cyc_List_List*match_rules=_tmp39E;
void*_tmp39F=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp39F;
*dopt=dec_tree;{
int _tmp3A0=0;int ex_done=_tmp3A0;
((void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1635
for(1;match_rules != 0;match_rules=match_rules->tl){
# 1637
if(match_rules->tl == 0)break;{
struct _tuple23*_tmp3A1=(struct _tuple23*)match_rules->hd;struct _tuple23*_stmttmp32=_tmp3A1;struct _tuple23*_tmp3A2=_stmttmp32;unsigned _tmp3A4;int _tmp3A3;_LL1: _tmp3A3=(_tmp3A2->f2)->used;_tmp3A4=(_tmp3A2->f2)->pat_loc;_LL2: {int b=_tmp3A3;unsigned loc2=_tmp3A4;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp43C;_tmp43C.tag=0,({struct _fat_ptr _tmp5DB=({const char*_tmp3A7="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp3A7,sizeof(char),74U);});_tmp43C.f1=_tmp5DB;});_tmp43C;});void*_tmp3A5[1];_tmp3A5[0]=& _tmp3A6;({unsigned _tmp5DC=loc2;Cyc_Warn_err2(_tmp5DC,_tag_fat(_tmp3A5,sizeof(void*),1));});});
break;}}}}}}
# 1646
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp3A8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;struct Cyc_List_List*l=_tmp3A8;for(0;l != 0;l=l->tl){
if((*((struct _tuple17*)l->hd)).f1 != 0)
return 1;}}
return 0;}

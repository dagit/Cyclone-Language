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

/* Pools */
struct _pool; // defined in runtime_memory.c
struct _PoolHandle {
  struct _RuntimeStack s;
  struct _pool *p;
};
struct _PoolHandle _new_pool(void);
void _free_pool(struct _PoolHandle *h);
void _push_pool(struct _PoolHandle * r);
void _pop_pool(void);

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
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 379
extern struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 94
extern int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
extern void*Cyc_Set_choose(struct Cyc_Set_Set*s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 292
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 305
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 312
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 513
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 531
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 699 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 886
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 895
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_compress(void*);
# 916
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 918
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 921
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 923
extern void*Cyc_Absyn_sint_type;
# 926
void*Cyc_Absyn_gen_float_type(unsigned i);
# 928
extern void*Cyc_Absyn_unique_rgn_type;
# 930
extern void*Cyc_Absyn_empty_effect;
# 932
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 934
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 963
void*Cyc_Absyn_bounds_one (void);
# 965
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 988
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1154
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 74
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
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
int Cyc_Tcutil_is_nullable_pointer_type(void*,int def);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 107
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 114
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*assume,void*t1,void*t2);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple13{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 151
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 158
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr err_msg);
# 166
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 177
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 185
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 228
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 235
int Cyc_Tcutil_force_type2bool(int desired,void*);
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
# 141
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 143
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened,int lifo);
# 146
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 32
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct _tuple15{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 46 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;int _tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List**_tmp4;struct Cyc_Absyn_Aggrdecl**_tmp3;int _tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;switch(*((int*)_tmp1)){case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2: {struct Cyc_List_List*exist_ts=_tmp7;struct Cyc_List_List*dps=_tmp8;int dots=_tmp9;
# 52
if(topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A6;_tmp3A6.tag=0U,({struct _fat_ptr _tmp439=({const char*_tmpC="cannot determine pattern type";_tag_fat(_tmpC,sizeof(char),30U);});_tmp3A6.f1=_tmp439;});_tmp3A6;});void*_tmpA[1U];_tmpA[0]=& _tmpB;({unsigned _tmp43A=p->loc;Cyc_Warn_err2(_tmp43A,_tag_fat(_tmpA,sizeof(void*),1U));});});else{
# 55
void*_tmpD=Cyc_Absyn_compress(*topt);void*_stmttmp1=_tmpD;void*_tmpE=_stmttmp1;union Cyc_Absyn_AggrInfo _tmpF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->f1)->tag == 20U){_LL10: _tmpF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE)->f1)->f1;_LL11: {union Cyc_Absyn_AggrInfo ainfo=_tmpF;
# 57
({void*_tmp43C=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp43B=({union Cyc_Absyn_AggrInfo*_tmp10=_cycalloc(sizeof(*_tmp10));*_tmp10=ainfo;_tmp10;});_tmp11->f1=_tmp43B;}),_tmp11->f2=exist_ts,_tmp11->f3=dps,_tmp11->f4=dots;_tmp11;});p->r=_tmp43C;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}}else{goto _LL12;}}else{_LL12: _LL13:
# 61
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A8;_tmp3A8.tag=0U,({struct _fat_ptr _tmp43D=({const char*_tmp15="pattern expects aggregate type instead of ";_tag_fat(_tmp15,sizeof(char),43U);});_tmp3A8.f1=_tmp43D;});_tmp3A8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3A7;_tmp3A7.tag=2U,_tmp3A7.f1=(void*)*topt;_tmp3A7;});void*_tmp12[2U];_tmp12[0]=& _tmp13,_tmp12[1]=& _tmp14;({unsigned _tmp43E=p->loc;Cyc_Warn_err2(_tmp43E,_tag_fat(_tmp12,sizeof(void*),2U));});});
goto _LLF;}_LLF:;}
# 64
return;}}else{if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).tag == 2){_LL3: _tmp3=((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).val;_tmp4=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp3;struct Cyc_List_List**exist_ts=_tmp4;struct Cyc_List_List*dps=_tmp5;int dots=_tmp6;
# 67
struct Cyc_Absyn_Aggrdecl*_tmp16=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp16;
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AA;_tmp3AA.tag=0U,({struct _fat_ptr _tmp43F=({const char*_tmp1A="can't destructure an abstract";_tag_fat(_tmp1A,sizeof(char),30U);});_tmp3AA.f1=_tmp43F;});_tmp3AA;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp19=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3A9;_tmp3A9.tag=6U,_tmp3A9.f1=ad;_tmp3A9;});void*_tmp17[2U];_tmp17[0]=& _tmp18,_tmp17[1]=& _tmp19;({unsigned _tmp440=p->loc;Cyc_Warn_err2(_tmp440,_tag_fat(_tmp17,sizeof(void*),2U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 73
int more_exists=({int _tmp441=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp441 - ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*exist_ts);});
if(more_exists < 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AB;_tmp3AB.tag=0U,({struct _fat_ptr _tmp442=({const char*_tmp1D="too many existential type variables in pattern";_tag_fat(_tmp1D,sizeof(char),47U);});_tmp3AB.f1=_tmp442;});_tmp3AB;});void*_tmp1B[1U];_tmp1B[0]=& _tmp1C;({unsigned _tmp443=p->loc;Cyc_Warn_err2(_tmp443,_tag_fat(_tmp1B,sizeof(void*),1U));});});{
struct Cyc_List_List**_tmp1E=exist_ts;struct Cyc_List_List**ts=_tmp1E;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);for(0;n != 0;-- n){
ts=&((struct Cyc_List_List*)_check_null(*ts))->tl;}}
*ts=0;}}else{
if(more_exists > 0){
# 82
struct Cyc_List_List*_tmp1F=0;struct Cyc_List_List*new_ts=_tmp1F;
for(0;more_exists != 0;-- more_exists){
new_ts=({struct Cyc_List_List*_tmp21=_cycalloc(sizeof(*_tmp21));({struct Cyc_Absyn_Tvar*_tmp444=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=1U,_tmp20->f1=0;_tmp20;}));_tmp21->hd=_tmp444;}),_tmp21->tl=new_ts;_tmp21;});}
({struct Cyc_List_List*_tmp445=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*exist_ts,new_ts);*exist_ts=_tmp445;});}}
# 87
return;}}}else{_LLB: _LLC:
# 100
({struct Cyc_Warn_String_Warn_Warg_struct _tmp31=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AC;_tmp3AC.tag=0U,({struct _fat_ptr _tmp446=({const char*_tmp32="resolve_pat unknownAggr";_tag_fat(_tmp32,sizeof(char),24U);});_tmp3AC.f1=_tmp446;});_tmp3AC;});void*_tmp30[1U];_tmp30[0]=& _tmp31;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp30,sizeof(void*),1U));});}}case 17U: _LL5: _tmp2=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp2;
# 89
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcutil_is_const_exp(e)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AD;_tmp3AD.tag=0U,({struct _fat_ptr _tmp447=({const char*_tmp24="non-constant expression in case pattern";_tag_fat(_tmp24,sizeof(char),40U);});_tmp3AD.f1=_tmp447;});_tmp3AD;});void*_tmp22[1U];_tmp22[0]=& _tmp23;({unsigned _tmp448=p->loc;Cyc_Warn_err2(_tmp448,_tag_fat(_tmp22,sizeof(void*),1U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 94
struct _tuple15 _tmp25=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple15 _stmttmp2=_tmp25;struct _tuple15 _tmp26=_stmttmp2;int _tmp28;unsigned _tmp27;_LL15: _tmp27=_tmp26.f1;_tmp28=_tmp26.f2;_LL16: {unsigned i=_tmp27;int known=_tmp28;
({void*_tmp449=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=10U,_tmp29->f1=Cyc_Absyn_None,_tmp29->f2=(int)i;_tmp29;});p->r=_tmp449;});
return;}}}case 15U: _LL7: _LL8:
# 98
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AE;_tmp3AE.tag=0U,({struct _fat_ptr _tmp44A=({const char*_tmp2C="resolve_pat UnknownId_p";_tag_fat(_tmp2C,sizeof(char),24U);});_tmp3AE.f1=_tmp44A;});_tmp3AE;});void*_tmp2A[1U];_tmp2A[0]=& _tmp2B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2A,sizeof(void*),1U));});case 16U: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AF;_tmp3AF.tag=0U,({struct _fat_ptr _tmp44B=({const char*_tmp2F="resolve_pat UnknownCall_p";_tag_fat(_tmp2F,sizeof(char),26U);});_tmp3AF.f1=_tmp44B;});_tmp3AF;});void*_tmp2D[1U];_tmp2D[0]=& _tmp2E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2D,sizeof(void*),1U));});default: _LLD: _LLE:
# 102
 return;}_LL0:;}
# 106
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){
return({struct Cyc_Absyn_Pat*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->loc=loc,_tmp33->topt=0,_tmp33->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp33;});}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 109
static struct _tuple16*Cyc_Tcpat_wild_dlp(unsigned loc){
return({struct _tuple16*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->f1=0,({struct Cyc_Absyn_Pat*_tmp44C=Cyc_Tcpat_wild_pat(loc);_tmp34->f2=_tmp44C;});_tmp34;});}
# 113
static struct Cyc_List_List*Cyc_Tcpat_remove_dots(struct Cyc_List_List*earlier,int num_pats,int num_fields,void*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str){
# 115
if(num_pats >= num_fields)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B2;_tmp3B2.tag=0U,({struct _fat_ptr _tmp44D=({const char*_tmp3A="unnecessary ... in ";_tag_fat(_tmp3A,sizeof(char),20U);});_tmp3B2.f1=_tmp44D;});_tmp3B2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B1;_tmp3B1.tag=0U,_tmp3B1.f1=msg_str;_tmp3B1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp38=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B0;_tmp3B0.tag=0U,({struct _fat_ptr _tmp44E=({const char*_tmp39=" pattern";_tag_fat(_tmp39,sizeof(char),9U);});_tmp3B0.f1=_tmp44E;});_tmp3B0;});void*_tmp35[3U];_tmp35[0]=& _tmp36,_tmp35[1]=& _tmp37,_tmp35[2]=& _tmp38;({unsigned _tmp44F=loc;Cyc_Warn_warn2(_tmp44F,_tag_fat(_tmp35,sizeof(void*),3U));});});{
struct Cyc_List_List*new_ps=0;
{int i=0;for(0;i < num_fields - num_pats;++ i){
new_ps=({struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));({void*_tmp450=f(loc);_tmp3B->hd=_tmp450;}),_tmp3B->tl=new_ps;_tmp3B;});}}
return Cyc_List_imp_append(earlier,new_ps);}}
# 122
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 125
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
return topt == 0?Cyc_Absyn_new_evar(& Cyc_Kinds_mko,({struct Cyc_Core_Opt*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->v=s;_tmp3C;})):*topt;}
# 128
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 132
if(topt == 0)
return numt;
if(Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;{
void*_tmp3D=Cyc_Absyn_compress(numt);void*_stmttmp3=_tmp3D;void*_tmp3E=_stmttmp3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f1)){case 15U: _LL1: _LL2:
 goto _LL4;case 16U: _LL3: _LL4:
 return*topt;default: goto _LL5;}else{_LL5: _LL6:
 return numt;}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 143
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 145
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp451=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_tmp451;});}
# 149
({struct Cyc_List_List*_tmp453=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));({struct _tuple17*_tmp452=({struct _tuple17*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->f1=vd,_tmp3F->f2=e;_tmp3F;});_tmp40->hd=_tmp452;}),_tmp40->tl=*v_result_ptr;_tmp40;});*v_result_ptr=_tmp453;});}
# 151
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 153
struct Cyc_Tcpat_TcPatResult _tmp41=res1;struct Cyc_List_List*_tmp43;struct _tuple1*_tmp42;_LL1: _tmp42=_tmp41.tvars_and_bounds_opt;_tmp43=_tmp41.patvars;_LL2: {struct _tuple1*p1=_tmp42;struct Cyc_List_List*vs1=_tmp43;
struct Cyc_Tcpat_TcPatResult _tmp44=res2;struct Cyc_List_List*_tmp46;struct _tuple1*_tmp45;_LL4: _tmp45=_tmp44.tvars_and_bounds_opt;_tmp46=_tmp44.patvars;_LL5: {struct _tuple1*p2=_tmp45;struct Cyc_List_List*vs2=_tmp46;
if(p1 == 0 && p2 == 0)
return({struct Cyc_Tcpat_TcPatResult _tmp3B3;_tmp3B3.tvars_and_bounds_opt=0,({struct Cyc_List_List*_tmp454=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(vs1,vs2);_tmp3B3.patvars=_tmp454;});_tmp3B3;});
if(p1 == 0)
p1=({struct _tuple1*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->f1=0,_tmp47->f2=0;_tmp47;});
if(p2 == 0)
p2=({struct _tuple1*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->f1=0,_tmp48->f2=0;_tmp48;});
return({struct Cyc_Tcpat_TcPatResult _tmp3B4;({struct _tuple1*_tmp458=({struct _tuple1*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_List_List*_tmp457=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*p1).f1,(*p2).f1);_tmp49->f1=_tmp457;}),({
struct Cyc_List_List*_tmp456=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*p1).f2,(*p2).f2);_tmp49->f2=_tmp456;});_tmp49;});
# 161
_tmp3B4.tvars_and_bounds_opt=_tmp458;}),({
# 163
struct Cyc_List_List*_tmp455=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(vs1,vs2);_tmp3B4.patvars=_tmp455;});_tmp3B4;});}}}
# 167
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp45A=t;Cyc_Tcutil_promote_array(_tmp45A,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct Cyc_List_List*_tmp459=Cyc_Tcenv_lookup_type_vars(te);_tmp4A->v=_tmp459;});_tmp4A;})): rgn_opt,0);}): t;}struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};
# 174
static struct _tuple18*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple18*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->f1=tv,_tmp4B->f2=0;_tmp4B;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 178
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 181
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp3DD;_tmp3DD.tvars_and_bounds_opt=0,_tmp3DD.patvars=0;_tmp3DD;});
# 186
{void*_tmp4C=p->r;void*_stmttmp4=_tmp4C;void*_tmp4D=_stmttmp4;int _tmp51;struct Cyc_List_List**_tmp50;struct Cyc_Absyn_Datatypefield*_tmp4F;struct Cyc_Absyn_Datatypedecl*_tmp4E;int _tmp55;struct Cyc_List_List**_tmp54;struct Cyc_List_List*_tmp53;struct Cyc_Absyn_Aggrdecl*_tmp52;int _tmp57;struct Cyc_List_List**_tmp56;struct Cyc_Absyn_Pat*_tmp58;void*_tmp59;struct Cyc_Absyn_Enumdecl*_tmp5A;int _tmp5B;struct Cyc_Absyn_Vardecl*_tmp5D;struct Cyc_Absyn_Tvar*_tmp5C;struct Cyc_Absyn_Pat*_tmp5F;struct Cyc_Absyn_Vardecl*_tmp5E;struct Cyc_Absyn_Vardecl*_tmp61;struct Cyc_Absyn_Tvar*_tmp60;struct Cyc_Absyn_Pat*_tmp63;struct Cyc_Absyn_Vardecl*_tmp62;switch(*((int*)_tmp4D)){case 0U: _LL1: _LL2:
# 188
 t=({struct Cyc_List_List*_tmp45B=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp45B,topt);});goto _LL0;case 1U: _LL3: _tmp62=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_tmp63=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4D)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp62;struct Cyc_Absyn_Pat*p2=_tmp63;
# 192
struct _tuple2*_tmp64=vd->name;struct _tuple2*_stmttmp5=_tmp64;struct _tuple2*_tmp65=_stmttmp5;struct _fat_ptr _tmp66;_LL2C: _tmp66=*_tmp65->f2;_LL2D: {struct _fat_ptr v=_tmp66;
if(({struct _fat_ptr _tmp45D=(struct _fat_ptr)v;Cyc_strcmp(_tmp45D,({const char*_tmp67="true";_tag_fat(_tmp67,sizeof(char),5U);}));})== 0 ||({struct _fat_ptr _tmp45C=(struct _fat_ptr)v;Cyc_strcmp(_tmp45C,({const char*_tmp68="false";_tag_fat(_tmp68,sizeof(char),6U);}));})== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B7;_tmp3B7.tag=0U,({struct _fat_ptr _tmp45E=({const char*_tmp6E="you probably do not want to use ";_tag_fat(_tmp6E,sizeof(char),33U);});_tmp3B7.f1=_tmp45E;});_tmp3B7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B6;_tmp3B6.tag=0U,_tmp3B6.f1=v;_tmp3B6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B5;_tmp3B5.tag=0U,({
struct _fat_ptr _tmp45F=({const char*_tmp6D=" as a local variable";_tag_fat(_tmp6D,sizeof(char),21U);});_tmp3B5.f1=_tmp45F;});_tmp3B5;});void*_tmp69[3U];_tmp69[0]=& _tmp6A,_tmp69[1]=& _tmp6B,_tmp69[2]=& _tmp6C;({unsigned _tmp460=p->loc;Cyc_Warn_warn2(_tmp460,_tag_fat(_tmp69,sizeof(void*),3U));});});
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(p2->topt);
# 200
{void*_tmp6F=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp6F;void*_tmp70=_stmttmp6;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70)->tag == 4U){_LL2F: _LL30:
# 202
 if(rgn_pat == 0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp72=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B8;_tmp3B8.tag=0U,({struct _fat_ptr _tmp461=({const char*_tmp73="array reference would point into unknown/unallowed region";_tag_fat(_tmp73,sizeof(char),58U);});_tmp3B8.f1=_tmp461;});_tmp3B8;});void*_tmp71[1U];_tmp71[0]=& _tmp72;({unsigned _tmp462=p->loc;Cyc_Warn_err2(_tmp462,_tag_fat(_tmp71,sizeof(void*),1U));});});
goto _LL2E;}else{_LL31: _LL32:
# 206
 if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp75=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B9;_tmp3B9.tag=0U,({struct _fat_ptr _tmp463=({const char*_tmp76="pattern would point to an abstract member";_tag_fat(_tmp76,sizeof(char),42U);});_tmp3B9.f1=_tmp463;});_tmp3B9;});void*_tmp74[1U];_tmp74[0]=& _tmp75;({unsigned _tmp464=p->loc;Cyc_Warn_err2(_tmp464,_tag_fat(_tmp74,sizeof(void*),1U));});});
goto _LL2E;}_LL2E:;}
# 210
({struct Cyc_Absyn_Vardecl**_tmp467=({struct Cyc_Absyn_Vardecl**_tmp77=_cycalloc(sizeof(*_tmp77));*_tmp77=vd;_tmp77;});struct Cyc_Absyn_Exp*_tmp466=access_exp;struct Cyc_List_List**_tmp465=& res.patvars;Cyc_Tcpat_set_vd(_tmp467,_tmp466,_tmp465,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}}case 2U: _LL5: _tmp60=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_tmp61=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4D)->f2;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp60;struct Cyc_Absyn_Vardecl*vd=_tmp61;
# 213
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp469=p->loc;struct Cyc_Tcenv_Tenv*_tmp468=te;Cyc_Tcenv_add_type_vars(_tmp469,_tmp468,({struct Cyc_Absyn_Tvar*_tmp84[1U];_tmp84[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp84,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp46A=({struct _tuple1*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->f1=0,_tmp78->f2=0;_tmp78;});res.tvars_and_bounds_opt=_tmp46A;});
({struct Cyc_List_List*_tmp46D=({
struct Cyc_List_List*_tmp46C=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp46C,({struct Cyc_List_List*_tmp7A=_cycalloc(sizeof(*_tmp7A));({struct _tuple18*_tmp46B=({struct _tuple18*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->f1=tv,_tmp79->f2=1;_tmp79;});_tmp7A->hd=_tmp46B;}),_tmp7A->tl=0;_tmp7A;}));});
# 216
(*res.tvars_and_bounds_opt).f1=_tmp46D;});
# 218
({unsigned _tmp470=p->loc;struct Cyc_Tcenv_Tenv*_tmp46F=te2;struct Cyc_List_List*_tmp46E=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp470,_tmp46F,_tmp46E,& Cyc_Kinds_tmk,1,0,vd->type);});
# 221
t=({struct Cyc_List_List*_tmp471=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp471,topt);});
{void*_tmp7B=Cyc_Absyn_compress(t);void*_stmttmp7=_tmp7B;void*_tmp7C=_stmttmp7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7C)->tag == 4U){_LL34: _LL35:
# 224
 if(rgn_pat == 0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BA;_tmp3BA.tag=0U,({struct _fat_ptr _tmp472=({const char*_tmp7F="array reference would point into unknown/unallowed region";_tag_fat(_tmp7F,sizeof(char),58U);});_tmp3BA.f1=_tmp472;});_tmp3BA;});void*_tmp7D[1U];_tmp7D[0]=& _tmp7E;({unsigned _tmp473=p->loc;Cyc_Warn_err2(_tmp473,_tag_fat(_tmp7D,sizeof(void*),1U));});});
goto _LL33;}else{_LL36: _LL37:
# 228
 if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp81=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BB;_tmp3BB.tag=0U,({struct _fat_ptr _tmp474=({const char*_tmp82="pattern would point to an abstract member";_tag_fat(_tmp82,sizeof(char),42U);});_tmp3BB.f1=_tmp474;});_tmp3BB;});void*_tmp80[1U];_tmp80[0]=& _tmp81;({unsigned _tmp475=p->loc;Cyc_Warn_err2(_tmp475,_tag_fat(_tmp80,sizeof(void*),1U));});});
goto _LL33;}_LL33:;}
# 232
({struct Cyc_Absyn_Vardecl**_tmp478=({struct Cyc_Absyn_Vardecl**_tmp83=_cycalloc(sizeof(*_tmp83));*_tmp83=vd;_tmp83;});struct Cyc_Absyn_Exp*_tmp477=access_exp;struct Cyc_List_List**_tmp476=& res.patvars;Cyc_Tcpat_set_vd(_tmp478,_tmp477,_tmp476,vd->type);});
goto _LL0;}case 3U: _LL7: _tmp5E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_tmp5F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4D)->f2;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp5E;struct Cyc_Absyn_Pat*p2=_tmp5F;
# 236
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(p2->topt);
if(!allow_ref_pat || rgn_pat == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp86=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BC;_tmp3BC.tag=0U,({struct _fat_ptr _tmp479=({const char*_tmp87="* pattern would point into an unknown/unallowed region";_tag_fat(_tmp87,sizeof(char),55U);});_tmp3BC.f1=_tmp479;});_tmp3BC;});void*_tmp85[1U];_tmp85[0]=& _tmp86;({unsigned _tmp47A=p->loc;Cyc_Warn_err2(_tmp47A,_tag_fat(_tmp85,sizeof(void*),1U));});});
goto _LL0;}else{
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BD;_tmp3BD.tag=0U,({struct _fat_ptr _tmp47B=({const char*_tmp8A="* pattern cannot take the address of an alias-free path";_tag_fat(_tmp8A,sizeof(char),56U);});_tmp3BD.f1=_tmp47B;});_tmp3BD;});void*_tmp88[1U];_tmp88[0]=& _tmp89;({unsigned _tmp47C=p->loc;Cyc_Warn_err2(_tmp47C,_tag_fat(_tmp88,sizeof(void*),1U));});});}{
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PtrInfo _tmp8B=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp8B;
pi.elt_type=t;
(pi.ptr_atts).rgn=rgn_pat;
(pi.ptr_atts).nullable=Cyc_Absyn_false_type;{
void*t2=Cyc_Absyn_pointer_type(pi);
if((unsigned)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);
new_access_exp->topt=t2;}
# 253
({struct Cyc_Absyn_Vardecl**_tmp47F=({struct Cyc_Absyn_Vardecl**_tmp8C=_cycalloc(sizeof(*_tmp8C));*_tmp8C=vd;_tmp8C;});struct Cyc_Absyn_Exp*_tmp47E=new_access_exp;struct Cyc_List_List**_tmp47D=& res.patvars;Cyc_Tcpat_set_vd(_tmp47F,_tmp47E,_tmp47D,t2);});
goto _LL0;}}}case 4U: _LL9: _tmp5C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_tmp5D=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4D)->f2;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp5C;struct Cyc_Absyn_Vardecl*vd=_tmp5D;
# 259
({struct Cyc_Absyn_Vardecl**_tmp482=({struct Cyc_Absyn_Vardecl**_tmp8D=_cycalloc(sizeof(*_tmp8D));*_tmp8D=vd;_tmp8D;});struct Cyc_Absyn_Exp*_tmp481=access_exp;struct Cyc_List_List**_tmp480=& res.patvars;Cyc_Tcpat_set_vd(_tmp482,_tmp481,_tmp480,vd->type);});
# 263
({unsigned _tmp484=p->loc;struct Cyc_Tcenv_Tenv*_tmp483=te;Cyc_Tcenv_add_type_vars(_tmp484,_tmp483,({struct Cyc_Absyn_Tvar*_tmp8E[1U];_tmp8E[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp8E,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp485=({struct _tuple1*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->f1=0,_tmp8F->f2=0;_tmp8F;});res.tvars_and_bounds_opt=_tmp485;});
({struct Cyc_List_List*_tmp487=({struct Cyc_List_List*_tmp91=_cycalloc(sizeof(*_tmp91));
({struct _tuple18*_tmp486=({struct _tuple18*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=tv,_tmp90->f2=0;_tmp90;});_tmp91->hd=_tmp486;}),_tmp91->tl=(*res.tvars_and_bounds_opt).f1;_tmp91;});
# 266
(*res.tvars_and_bounds_opt).f1=_tmp487;});
# 268
t=Cyc_Absyn_uint_type;
goto _LL0;}case 10U: if(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp4D)->f1 == Cyc_Absyn_Unsigned){_LLB: _LLC:
# 271
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;}else{_LLD: _LLE:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;}case 11U: _LLF: _LL10:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 12U: _LL11: _tmp5B=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp4D)->f2;_LL12: {int i=_tmp5B;
t=({void**_tmp488=topt;Cyc_Tcpat_num_type(_tmp488,Cyc_Absyn_gen_float_type((unsigned)i));});goto _LL0;}case 13U: _LL13: _tmp5A=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_LL14: {struct Cyc_Absyn_Enumdecl*ed=_tmp5A;
t=({void**_tmp489=topt;Cyc_Tcpat_num_type(_tmp489,Cyc_Absyn_enum_type(ed->name,ed));});goto _LL0;}case 14U: _LL15: _tmp59=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_LL16: {void*tenum=_tmp59;
t=Cyc_Tcpat_num_type(topt,tenum);goto _LL0;}case 9U: _LL17: _LL18:
# 278
 if(topt != 0){
void*_tmp92=Cyc_Absyn_compress(*topt);void*_stmttmp8=_tmp92;void*_tmp93=_stmttmp8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->tag == 3U){_LL39: _LL3A:
 t=*topt;goto tcpat_end;}else{_LL3B: _LL3C:
 goto _LL38;}_LL38:;}{
# 283
struct Cyc_Absyn_PtrInfo _tmp94=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp94;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
t=Cyc_Absyn_pointer_type(pi);
goto _LL0;}case 6U: _LL19: _tmp58=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_LL1A: {struct Cyc_Absyn_Pat*p2=_tmp58;
# 291
void*inner_typ=Cyc_Absyn_void_type;
void**_tmp95=0;void**inner_topt=_tmp95;
int elt_const=0;
if(topt != 0){
void*_tmp96=Cyc_Absyn_compress(*topt);void*_stmttmp9=_tmp96;void*_tmp97=_stmttmp9;struct Cyc_Absyn_Tqual _tmp99;void*_tmp98;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->tag == 3U){_LL3E: _tmp98=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).elt_type;_tmp99=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).elt_tq;_LL3F: {void*elt_typ=_tmp98;struct Cyc_Absyn_Tqual elt_tq=_tmp99;
# 297
inner_typ=elt_typ;
inner_topt=& inner_typ;
elt_const=elt_tq.real_const;
goto _LL3D;}}else{_LL40: _LL41:
 goto _LL3D;}_LL3D:;}{
# 306
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Kinds_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp48A=res;Cyc_Tcpat_combine_results(_tmp48A,Cyc_Tcpat_tcPatRec(te,p2,inner_topt,ptr_rgn,1,new_access_exp));});
# 315
{void*_tmp9A=Cyc_Absyn_compress((void*)_check_null(p2->topt));void*_stmttmpA=_tmp9A;void*_tmp9B=_stmttmpA;struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Datatypefield*_tmp9D;struct Cyc_Absyn_Datatypedecl*_tmp9C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9B)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9B)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9B)->f1)->f1).KnownDatatypefield).tag == 2){_LL43: _tmp9C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9B)->f1)->f1).KnownDatatypefield).val).f1;_tmp9D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9B)->f1)->f1).KnownDatatypefield).val).f2;_tmp9E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9B)->f2;_LL44: {struct Cyc_Absyn_Datatypedecl*tud=_tmp9C;struct Cyc_Absyn_Datatypefield*tuf=_tmp9D;struct Cyc_List_List*targs=_tmp9E;
# 319
{void*_tmp9F=Cyc_Absyn_compress(inner_typ);void*_stmttmpB=_tmp9F;void*_tmpA0=_stmttmpB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpA0)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpA0)->f1)->tag == 19U){_LL48: _LL49:
# 321
 goto DONT_PROMOTE;}else{goto _LL4A;}}else{_LL4A: _LL4B:
 goto _LL47;}_LL47:;}{
# 325
void*new_type=({union Cyc_Absyn_DatatypeInfo _tmp48B=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmpA2=_cycalloc(sizeof(*_tmpA2));*_tmpA2=tud;_tmpA2;}));Cyc_Absyn_datatype_type(_tmp48B,targs);});
# 327
p2->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=3U,(_tmpA1->f1).elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _tmp48F=Cyc_Absyn_const_tqual(0U);(_tmpA1->f1).elt_tq=_tmp48F;}):({
struct Cyc_Absyn_Tqual _tmp48E=Cyc_Absyn_empty_tqual(0U);(_tmpA1->f1).elt_tq=_tmp48E;}),
((_tmpA1->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp48D=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmpA1->f1).ptr_atts).nullable=_tmp48D;}),({
void*_tmp48C=Cyc_Absyn_bounds_one();((_tmpA1->f1).ptr_atts).bounds=_tmp48C;}),((_tmpA1->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmpA1->f1).ptr_atts).ptrloc=0;_tmpA1;});
# 334
goto _LL42;}}}else{goto _LL45;}}else{goto _LL45;}}else{_LL45: _LL46:
# 336
 DONT_PROMOTE: {
struct Cyc_Absyn_PtrInfo _tmpA3=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmpA3;
pi.elt_type=(void*)_check_null(p2->topt);
({struct Cyc_Absyn_Tqual _tmp490=elt_const?Cyc_Absyn_const_tqual(0U): Cyc_Absyn_empty_tqual(0U);pi.elt_tq=_tmp490;});
(pi.ptr_atts).rgn=ptr_rgn;
t=Cyc_Absyn_pointer_type(pi);}}_LL42:;}
# 343
if((unsigned)new_access_exp)new_access_exp->topt=p2->topt;
goto _LL0;}}case 5U: _LL1B: _tmp56=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_tmp57=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4D)->f2;_LL1C: {struct Cyc_List_List**ps_p=_tmp56;int dots=_tmp57;
# 347
struct Cyc_List_List*_tmpA4=*ps_p;struct Cyc_List_List*ps=_tmpA4;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpA5=Cyc_Absyn_compress(*topt);void*_stmttmpC=_tmpA5;void*_tmpA6=_stmttmpC;struct Cyc_List_List*_tmpA7;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA6)->tag == 6U){_LL4D: _tmpA7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA6)->f1;_LL4E: {struct Cyc_List_List*tqts=_tmpA7;
# 353
topt_ts=tqts;
if(dots){
({struct Cyc_List_List*_tmp495=({struct Cyc_List_List*_tmp494=ps;int _tmp493=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);int _tmp492=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);unsigned _tmp491=p->loc;((struct Cyc_List_List*(*)(struct Cyc_List_List*earlier,int num_pats,int num_fields,struct Cyc_Absyn_Pat*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str))Cyc_Tcpat_remove_dots)(_tmp494,_tmp493,_tmp492,Cyc_Tcpat_wild_pat,_tmp491,({const char*_tmpA8="tuple";_tag_fat(_tmpA8,sizeof(char),6U);}));});*ps_p=_tmp495;});
# 357
ps=*ps_p;}
# 359
goto _LL4C;}}else{_LL4F: _LL50:
 goto _LL4C;}_LL4C:;}else{
# 362
if(dots)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BE;_tmp3BE.tag=0U,({struct _fat_ptr _tmp496=({const char*_tmpAB="cannot determine missing fields for ... in tuple pattern";_tag_fat(_tmpAB,sizeof(char),57U);});_tmp3BE.f1=_tmp496;});_tmp3BE;});void*_tmpA9[1U];_tmpA9[0]=& _tmpAA;({unsigned _tmp497=p->loc;Cyc_Warn_err2(_tmp497,_tag_fat(_tmpA9,sizeof(void*),1U));});});}
{int i=0;for(0;ps != 0;(ps=ps->tl,i ++)){
void**_tmpAC=0;void**inner_topt=_tmpAC;
if(topt_ts != 0){
inner_topt=&(*((struct _tuple19*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 370
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=({struct Cyc_Absyn_Exp*_tmp498=access_exp;Cyc_Absyn_subscript_exp(_tmp498,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3BF;(_tmp3BF.Int_c).tag=5U,((_tmp3BF.Int_c).val).f1=Cyc_Absyn_Unsigned,((_tmp3BF.Int_c).val).f2=i;_tmp3BF;}),0U),0U);});
# 375
res=({struct Cyc_Tcpat_TcPatResult _tmp499=res;Cyc_Tcpat_combine_results(_tmp499,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)ps->hd,inner_topt,rgn_pat,allow_ref_pat,new_access_exp));});
# 378
if((unsigned)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)ps->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmpAE=_cycalloc(sizeof(*_tmpAE));({struct _tuple19*_tmp49B=({struct _tuple19*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct Cyc_Absyn_Tqual _tmp49A=Cyc_Absyn_empty_tqual(0U);_tmpAD->f1=_tmp49A;}),_tmpAD->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)ps->hd)->topt);_tmpAD;});_tmpAE->hd=_tmp49B;}),_tmpAE->tl=pat_ts;_tmpAE;});}}}
# 381
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=6U,({struct Cyc_List_List*_tmp49C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpAF->f1=_tmp49C;});_tmpAF;});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D)->f1)->KnownAggr).tag == 2){_LL1D: _tmp52=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D)->f1)->KnownAggr).val;_tmp53=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D)->f2;_tmp54=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D)->f3;_tmp55=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D)->f4;_LL1E: {struct Cyc_Absyn_Aggrdecl*ad=_tmp52;struct Cyc_List_List*exist_ts=_tmp53;struct Cyc_List_List**dps_p=_tmp54;int dots=_tmp55;
# 385
struct Cyc_List_List*_tmpB0=*dps_p;struct Cyc_List_List*dps=_tmpB0;
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C1;_tmp3C1.tag=0U,({struct _fat_ptr _tmp49D=({const char*_tmpB4="can't destructure abstract ";_tag_fat(_tmpB4,sizeof(char),28U);});_tmp3C1.f1=_tmp49D;});_tmp3C1;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3C0;_tmp3C0.tag=6U,_tmp3C0.f1=ad;_tmp3C0;});void*_tmpB1[2U];_tmpB1[0]=& _tmpB2,_tmpB1[1]=& _tmpB3;({unsigned _tmp49E=p->loc;Cyc_Warn_err2(_tmp49E,_tag_fat(_tmpB1,sizeof(void*),2U));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL0;}
# 393
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
allow_ref_pat=0;
if(exist_ts != 0){
# 399
if(topt == 0 || Cyc_Tcutil_type_kind(*topt)!= & Cyc_Kinds_ak)
allow_ref_pat=0;}
# 402
{struct _RegionHandle _tmpB5=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB5;_push_region(rgn);
# 404
{struct Cyc_List_List*_tmpB6=0;struct Cyc_List_List*var_tvs=_tmpB6;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpB7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct Cyc_List_List*u=_tmpB7;
{struct Cyc_List_List*t=exist_ts;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(u))->hd;
u=u->tl;{
void*_tmpB8=Cyc_Kinds_compress_kb(tv->kind);void*kb1=_tmpB8;
void*_tmpB9=Cyc_Kinds_compress_kb(uv->kind);void*kb2=_tmpB9;
struct Cyc_Absyn_Kind*k2;
{void*_tmpBA=kb2;struct Cyc_Absyn_Kind*_tmpBB;struct Cyc_Absyn_Kind*_tmpBC;switch(*((int*)_tmpBA)){case 2U: _LL52: _tmpBC=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBA)->f2;_LL53: {struct Cyc_Absyn_Kind*k=_tmpBC;
_tmpBB=k;goto _LL55;}case 0U: _LL54: _tmpBB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpBA)->f1;_LL55: {struct Cyc_Absyn_Kind*k=_tmpBB;
k2=k;goto _LL51;}default: _LL56: _LL57:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C2;_tmp3C2.tag=0U,({struct _fat_ptr _tmp49F=({const char*_tmpBF="unconstrained existential tyvar in aggregate";_tag_fat(_tmpBF,sizeof(char),45U);});_tmp3C2.f1=_tmp49F;});_tmp3C2;});void*_tmpBD[1U];_tmpBD[0]=& _tmpBE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpBD,sizeof(void*),1U));});}_LL51:;}
# 419
{void*_tmpC0=kb1;struct Cyc_Core_Opt**_tmpC1;struct Cyc_Absyn_Kind*_tmpC3;struct Cyc_Core_Opt**_tmpC2;struct Cyc_Absyn_Kind*_tmpC4;switch(*((int*)_tmpC0)){case 0U: _LL59: _tmpC4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC0)->f1;_LL5A: {struct Cyc_Absyn_Kind*k1=_tmpC4;
# 422
if(!Cyc_Kinds_kind_leq(k2,k1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C8;_tmp3C8.tag=0U,({struct _fat_ptr _tmp4A0=({const char*_tmpCE="type variable ";_tag_fat(_tmpCE,sizeof(char),15U);});_tmp3C8.f1=_tmp4A0;});_tmp3C8;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpC7=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp3C7;_tmp3C7.tag=7U,_tmp3C7.f1=tv;_tmp3C7;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C6;_tmp3C6.tag=0U,({struct _fat_ptr _tmp4A1=({const char*_tmpCD=" has kind ";_tag_fat(_tmpCD,sizeof(char),11U);});_tmp3C6.f1=_tmp4A1;});_tmp3C6;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp3C5;_tmp3C5.tag=8U,_tmp3C5.f1=(void*)kb1;_tmp3C5;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4;_tmp3C4.tag=0U,({
struct _fat_ptr _tmp4A2=({const char*_tmpCC=" but must have at least kind ";_tag_fat(_tmpCC,sizeof(char),30U);});_tmp3C4.f1=_tmp4A2;});_tmp3C4;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp3C3;_tmp3C3.tag=9U,_tmp3C3.f1=k2;_tmp3C3;});void*_tmpC5[6U];_tmpC5[0]=& _tmpC6,_tmpC5[1]=& _tmpC7,_tmpC5[2]=& _tmpC8,_tmpC5[3]=& _tmpC9,_tmpC5[4]=& _tmpCA,_tmpC5[5]=& _tmpCB;({unsigned _tmp4A3=p->loc;Cyc_Warn_err2(_tmp4A3,_tag_fat(_tmpC5,sizeof(void*),6U));});});
goto _LL58;}case 2U: _LL5B: _tmpC2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC0)->f1;_tmpC3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC0)->f2;_LL5C: {struct Cyc_Core_Opt**f=_tmpC2;struct Cyc_Absyn_Kind*k=_tmpC3;
_tmpC1=f;goto _LL5E;}default: _LL5D: _tmpC1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC0)->f1;_LL5E: {struct Cyc_Core_Opt**f=_tmpC1;
({struct Cyc_Core_Opt*_tmp4A4=({struct Cyc_Core_Opt*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->v=kb2;_tmpCF;});*f=_tmp4A4;});goto _LL58;}}_LL58:;}{
# 429
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->tag=2U,_tmpD6->f1=tv;_tmpD6;});
var_tvs=({struct Cyc_List_List*_tmpD0=_region_malloc(rgn,sizeof(*_tmpD0));_tmpD0->hd=vartype,_tmpD0->tl=var_tvs;_tmpD0;});
# 433
if((int)k2->kind == (int)Cyc_Absyn_RgnKind){
if((int)k2->aliasqual != (int)Cyc_Absyn_Aliasable)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C9;_tmp3C9.tag=0U,({struct _fat_ptr _tmp4A5=({const char*_tmpD3="opened existential had unique or top region kind";_tag_fat(_tmpD3,sizeof(char),49U);});_tmp3C9.f1=_tmp4A5;});_tmp3C9;});void*_tmpD1[1U];_tmpD1[0]=& _tmpD2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpD1,sizeof(void*),1U));});
outlives_constraints=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));
({struct _tuple0*_tmp4A6=({struct _tuple0*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=Cyc_Absyn_empty_effect,_tmpD4->f2=vartype;_tmpD4;});_tmpD5->hd=_tmp4A6;}),_tmpD5->tl=outlives_constraints;_tmpD5;});}}}}}
# 441
var_tvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(var_tvs);{
# 443
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,exist_ts);
# 445
struct _tuple13 _tmpD7=({struct _tuple13 _tmp3D2;({struct Cyc_List_List*_tmp4A7=Cyc_Tcenv_lookup_type_vars(te2);_tmp3D2.f1=_tmp4A7;}),_tmp3D2.f2=rgn;_tmp3D2;});struct _tuple13 env=_tmpD7;
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmpD8;
struct Cyc_List_List*_tmpD9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,var_tvs);struct Cyc_List_List*exist_inst=_tmpD9;
struct Cyc_List_List*_tmpDA=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmpDA;
struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmpDB;
struct Cyc_List_List*_tmpDC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,all_inst,exist_inst);struct Cyc_List_List*inst=_tmpDC;
# 452
if(exist_ts != 0 ||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp4A8=({struct _tuple1*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->f1=0,_tmpDD->f2=0;_tmpDD;});res.tvars_and_bounds_opt=_tmp4A8;});
({struct Cyc_List_List*_tmp4AA=({
struct Cyc_List_List*_tmp4A9=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp4A9,((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,exist_ts));});
# 455
(*res.tvars_and_bounds_opt).f1=_tmp4AA;});
# 457
({struct Cyc_List_List*_tmp4AB=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 457
(*res.tvars_and_bounds_opt).f2=_tmp4AB;});{
# 459
struct Cyc_List_List*_tmpDE=0;struct Cyc_List_List*rpo_inst=_tmpDE;
{struct Cyc_List_List*_tmpDF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmpDF;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmpE1=_cycalloc(sizeof(*_tmpE1));({struct _tuple0*_tmp4AE=({struct _tuple0*_tmpE0=_cycalloc(sizeof(*_tmpE0));({void*_tmp4AD=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f1);_tmpE0->f1=_tmp4AD;}),({
void*_tmp4AC=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f2);_tmpE0->f2=_tmp4AC;});_tmpE0;});
# 461
_tmpE1->hd=_tmp4AE;}),_tmpE1->tl=rpo_inst;_tmpE1;});}}
# 464
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
({struct Cyc_List_List*_tmp4AF=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,rpo_inst);
# 465
(*res.tvars_and_bounds_opt).f2=_tmp4AF;});}}
# 470
t=({union Cyc_Absyn_AggrInfo _tmp4B0=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE2=_cycalloc(sizeof(*_tmpE2));*_tmpE2=ad;_tmpE2;}));Cyc_Absyn_aggr_type(_tmp4B0,all_typs);});
if(dots &&(int)ad->kind == (int)Cyc_Absyn_UnionA)
({void*_tmpE3=0U;({unsigned _tmp4B2=p->loc;struct _fat_ptr _tmp4B1=({const char*_tmpE4="`...' pattern not allowed in union pattern";_tag_fat(_tmpE4,sizeof(char),43U);});Cyc_Warn_warn(_tmp4B2,_tmp4B1,_tag_fat(_tmpE3,sizeof(void*),0U));});});else{
if(dots){
({struct Cyc_List_List*_tmp4B7=({struct Cyc_List_List*_tmp4B6=dps;int _tmp4B5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dps);int _tmp4B4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);unsigned _tmp4B3=p->loc;((struct Cyc_List_List*(*)(struct Cyc_List_List*earlier,int num_pats,int num_fields,struct _tuple16*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str))Cyc_Tcpat_remove_dots)(_tmp4B6,_tmp4B5,_tmp4B4,Cyc_Tcpat_wild_dlp,_tmp4B3,({const char*_tmpE5="struct";_tag_fat(_tmpE5,sizeof(char),7U);}));});*dps_p=_tmp4B7;});
# 476
dps=*dps_p;}}{
# 478
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,dps,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple20*_tmpE6=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmpD=_tmpE6;struct _tuple20*_tmpE7=_stmttmpD;struct Cyc_Absyn_Pat*_tmpE9;struct Cyc_Absyn_Aggrfield*_tmpE8;_LL60: _tmpE8=_tmpE7->f1;_tmpE9=_tmpE7->f2;_LL61: {struct Cyc_Absyn_Aggrfield*field=_tmpE8;struct Cyc_Absyn_Pat*pat=_tmpE9;
void*_tmpEA=Cyc_Tcutil_rsubstitute(rgn,inst,field->type);void*inst_fieldtyp=_tmpEA;
# 484
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,field->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp4B8=res;Cyc_Tcpat_combine_results(_tmp4B8,Cyc_Tcpat_tcPatRec(te2,pat,& inst_fieldtyp,rgn_pat,allow_ref_pat,new_access_exp));});
# 492
if(!Cyc_Unify_unify((void*)_check_null(pat->topt),inst_fieldtyp))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpEC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D1;_tmp3D1.tag=0U,({struct _fat_ptr _tmp4B9=({const char*_tmpF7="field ";_tag_fat(_tmpF7,sizeof(char),7U);});_tmp3D1.f1=_tmp4B9;});_tmp3D1;});struct Cyc_Warn_String_Warn_Warg_struct _tmpED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D0;_tmp3D0.tag=0U,_tmp3D0.f1=*field->name;_tmp3D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmpEE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CF;_tmp3CF.tag=0U,({struct _fat_ptr _tmp4BA=({const char*_tmpF6=" of ";_tag_fat(_tmpF6,sizeof(char),5U);});_tmp3CF.f1=_tmp4BA;});_tmp3CF;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpEF=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3CE;_tmp3CE.tag=6U,_tmp3CE.f1=ad;_tmp3CE;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CD;_tmp3CD.tag=0U,({
struct _fat_ptr _tmp4BB=({const char*_tmpF5=" pattern expects type ";_tag_fat(_tmpF5,sizeof(char),23U);});_tmp3CD.f1=_tmp4BB;});_tmp3CD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3CC;_tmp3CC.tag=2U,_tmp3CC.f1=(void*)inst_fieldtyp;_tmp3CC;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CB;_tmp3CB.tag=0U,({struct _fat_ptr _tmp4BC=({const char*_tmpF4=" != ";_tag_fat(_tmpF4,sizeof(char),5U);});_tmp3CB.f1=_tmp4BC;});_tmp3CB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3CA;_tmp3CA.tag=2U,_tmp3CA.f1=(void*)_check_null(pat->topt);_tmp3CA;});void*_tmpEB[8U];_tmpEB[0]=& _tmpEC,_tmpEB[1]=& _tmpED,_tmpEB[2]=& _tmpEE,_tmpEB[3]=& _tmpEF,_tmpEB[4]=& _tmpF0,_tmpEB[5]=& _tmpF1,_tmpEB[6]=& _tmpF2,_tmpEB[7]=& _tmpF3;({unsigned _tmp4BD=p->loc;Cyc_Warn_err2(_tmp4BD,_tag_fat(_tmpEB,sizeof(void*),8U));});});
# 496
if((unsigned)new_access_exp)new_access_exp->topt=pat->topt;}}}}}
# 404
;_pop_region();}
# 499
goto _LL0;}}else{_LL23: _LL24:
# 539
 goto _LL26;}}else{_LL21: _LL22:
# 538
 goto _LL24;}case 8U: _LL1F: _tmp4E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4D)->f1;_tmp4F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4D)->f2;_tmp50=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4D)->f3;_tmp51=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4D)->f4;_LL20: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4E;struct Cyc_Absyn_Datatypefield*tuf=_tmp4F;struct Cyc_List_List**ps_p=_tmp50;int dots=_tmp51;
# 502
struct Cyc_List_List*_tmpF8=*ps_p;struct Cyc_List_List*ps=_tmpF8;
struct Cyc_List_List*tqts=tuf->typs;
# 505
struct _tuple13 _tmpF9=({struct _tuple13 _tmp3DC;({struct Cyc_List_List*_tmp4BE=Cyc_Tcenv_lookup_type_vars(te);_tmp3DC.f1=_tmp4BE;}),_tmp3DC.f2=Cyc_Core_heap_region;_tmp3DC;});struct _tuple13 env=_tmpF9;
struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmpFA;
struct Cyc_List_List*_tmpFB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmpFB;
t=({union Cyc_Absyn_DatatypeFieldInfo _tmp4BF=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp4BF,all_typs);});
if(dots){
({struct Cyc_List_List*_tmp4C4=({struct Cyc_List_List*_tmp4C3=ps;int _tmp4C2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);int _tmp4C1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);unsigned _tmp4C0=p->loc;((struct Cyc_List_List*(*)(struct Cyc_List_List*earlier,int num_pats,int num_fields,struct Cyc_Absyn_Pat*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str))Cyc_Tcpat_remove_dots)(_tmp4C3,_tmp4C2,_tmp4C1,Cyc_Tcpat_wild_pat,_tmp4C0,({const char*_tmpFC="datatype field";_tag_fat(_tmpFC,sizeof(char),15U);}));});*ps_p=_tmp4C4;});
# 512
ps=*ps_p;}
# 514
for(0;ps != 0 && tqts != 0;(ps=ps->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpFD=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmpFD;
# 518
void*_tmpFE=Cyc_Tcutil_substitute(inst,(*((struct _tuple19*)tqts->hd)).f2);void*field_typ=_tmpFE;
# 521
if((unsigned)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
res=({struct Cyc_Tcpat_TcPatResult _tmp4C5=res;Cyc_Tcpat_combine_results(_tmp4C5,Cyc_Tcpat_tcPatRec(te,p2,& field_typ,rgn_pat,allow_ref_pat,0));});
# 528
if(!Cyc_Unify_unify((void*)_check_null(p2->topt),field_typ))
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp100=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3D7;_tmp3D7.tag=1U,_tmp3D7.f1=tuf->name;_tmp3D7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp101=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D6;_tmp3D6.tag=0U,({struct _fat_ptr _tmp4C6=({const char*_tmp106=" expects argument type ";_tag_fat(_tmp106,sizeof(char),24U);});_tmp3D6.f1=_tmp4C6;});_tmp3D6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp102=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3D5;_tmp3D5.tag=2U,_tmp3D5.f1=(void*)field_typ;_tmp3D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp103=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D4;_tmp3D4.tag=0U,({
struct _fat_ptr _tmp4C7=({const char*_tmp105=" not ";_tag_fat(_tmp105,sizeof(char),6U);});_tmp3D4.f1=_tmp4C7;});_tmp3D4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp104=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3D3;_tmp3D3.tag=2U,_tmp3D3.f1=(void*)_check_null(p2->topt);_tmp3D3;});void*_tmpFF[5U];_tmpFF[0]=& _tmp100,_tmpFF[1]=& _tmp101,_tmpFF[2]=& _tmp102,_tmpFF[3]=& _tmp103,_tmpFF[4]=& _tmp104;({unsigned _tmp4C8=p2->loc;Cyc_Warn_err2(_tmp4C8,_tag_fat(_tmpFF,sizeof(void*),5U));});});}
# 532
if(ps != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp108=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D9;_tmp3D9.tag=0U,({struct _fat_ptr _tmp4C9=({const char*_tmp10A="too many arguments for datatype constructor ";_tag_fat(_tmp10A,sizeof(char),45U);});_tmp3D9.f1=_tmp4C9;});_tmp3D9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp109=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3D8;_tmp3D8.tag=1U,_tmp3D8.f1=tuf->name;_tmp3D8;});void*_tmp107[2U];_tmp107[0]=& _tmp108,_tmp107[1]=& _tmp109;({unsigned _tmp4CA=p->loc;Cyc_Warn_err2(_tmp4CA,_tag_fat(_tmp107,sizeof(void*),2U));});});
if(tqts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DB;_tmp3DB.tag=0U,({struct _fat_ptr _tmp4CB=({const char*_tmp10E="too few arguments for datatype constructor ";_tag_fat(_tmp10E,sizeof(char),44U);});_tmp3DB.f1=_tmp4CB;});_tmp3DB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3DA;_tmp3DA.tag=1U,_tmp3DA.f1=tuf->name;_tmp3DA;});void*_tmp10B[2U];_tmp10B[0]=& _tmp10C,_tmp10B[1]=& _tmp10D;({unsigned _tmp4CC=p->loc;Cyc_Warn_err2(_tmp4CC,_tag_fat(_tmp10B,sizeof(void*),2U));});});
goto _LL0;}case 15U: _LL25: _LL26:
# 540
 goto _LL28;case 17U: _LL27: _LL28:
 goto _LL2A;default: _LL29: _LL2A:
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL0;}_LL0:;}
# 544
tcpat_end:
 p->topt=t;
return res;}}
# 549
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 551
struct Cyc_Tcpat_TcPatResult _tmp10F=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);struct Cyc_Tcpat_TcPatResult ans=_tmp10F;
# 553
struct _tuple1 _tmp110=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(ans.patvars);struct _tuple1 _stmttmpE=_tmp110;struct _tuple1 _tmp111=_stmttmpE;struct Cyc_List_List*_tmp112;_LL1: _tmp112=_tmp111.f1;_LL2: {struct Cyc_List_List*vs1=_tmp112;
struct Cyc_List_List*_tmp113=0;struct Cyc_List_List*vs=_tmp113;
{struct Cyc_List_List*x=vs1;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)vs=({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),_tmp114->tl=vs;_tmp114;});}}
({struct Cyc_List_List*_tmp4CE=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_name,vs);unsigned _tmp4CD=p->loc;Cyc_Tcutil_check_unique_vars(_tmp4CE,_tmp4CD,({const char*_tmp115="pattern contains a repeated variable";_tag_fat(_tmp115,sizeof(char),37U);}));});
# 562
{struct Cyc_List_List*x=ans.patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp116=(struct _tuple17*)x->hd;struct _tuple17*_stmttmpF=_tmp116;struct _tuple17*_tmp117=_stmttmpF;struct Cyc_Absyn_Exp**_tmp119;struct Cyc_Absyn_Vardecl**_tmp118;_LL4: _tmp118=_tmp117->f1;_tmp119=(struct Cyc_Absyn_Exp**)& _tmp117->f2;_LL5: {struct Cyc_Absyn_Vardecl**vdopt=_tmp118;struct Cyc_Absyn_Exp**expopt=_tmp119;
if(*expopt != 0 &&*expopt != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp4CF=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*expopt));*expopt=_tmp4CF;});}}}
# 567
return ans;}}
# 573
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 576
struct _tuple0 _tmp11A=({struct _tuple0 _tmp3DE;({void*_tmp4D1=Cyc_Absyn_compress(old_type);_tmp3DE.f1=_tmp4D1;}),({void*_tmp4D0=Cyc_Absyn_compress(new_type);_tmp3DE.f2=_tmp4D0;});_tmp3DE;});struct _tuple0 _stmttmp10=_tmp11A;struct _tuple0 _tmp11B=_stmttmp10;struct Cyc_Absyn_PtrInfo _tmp11D;struct Cyc_Absyn_PtrInfo _tmp11C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11B.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11B.f2)->tag == 3U){_LL1: _tmp11C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11B.f1)->f1;_tmp11D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11B.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pold=_tmp11C;struct Cyc_Absyn_PtrInfo pnew=_tmp11D;
# 578
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11E=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=3U,(_tmp11F->f1).elt_type=pold.elt_type,(_tmp11F->f1).elt_tq=pnew.elt_tq,
((_tmp11F->f1).ptr_atts).rgn=(pold.ptr_atts).rgn,((_tmp11F->f1).ptr_atts).nullable=(pnew.ptr_atts).nullable,((_tmp11F->f1).ptr_atts).bounds=(pnew.ptr_atts).bounds,((_tmp11F->f1).ptr_atts).zero_term=(pnew.ptr_atts).zero_term,((_tmp11F->f1).ptr_atts).ptrloc=(pold.ptr_atts).ptrloc;_tmp11F;});
# 578
struct Cyc_Absyn_PointerType_Absyn_Type_struct*ptry=_tmp11E;
# 584
return({struct Cyc_RgnOrder_RgnPO*_tmp4D6=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp4D5=assump;void*_tmp4D4=(void*)ptry;Cyc_Tcutil_subtype(_tmp4D6,_tmp4D5,_tmp4D4,new_type);})&&({
struct Cyc_RgnOrder_RgnPO*_tmp4D3=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_Absyn_Exp*_tmp4D2=initializer;Cyc_Tcutil_coerce_assign(_tmp4D3,_tmp4D2,(void*)ptry);});}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 593
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 596
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp130=_cycalloc(sizeof(*_tmp130));({struct _tuple0*_tmp4D8=({struct _tuple0*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->f1=Cyc_Absyn_unique_rgn_type,({void*_tmp4D7=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=2U,_tmp12E->f1=tv;_tmp12E;});_tmp12F->f2=_tmp4D7;});_tmp12F;});_tmp130->hd=_tmp4D8;}),_tmp130->tl=0;_tmp130;});
if(({struct Cyc_RgnOrder_RgnPO*_tmp4DB=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp4DA=assump;void*_tmp4D9=old_type;Cyc_Tcutil_subtype(_tmp4DB,_tmp4DA,_tmp4D9,new_type);})){
# 614 "tcpat.cyc"
struct _tuple0 _tmp120=({struct _tuple0 _tmp3E0;({void*_tmp4DD=Cyc_Absyn_compress(old_type);_tmp3E0.f1=_tmp4DD;}),({void*_tmp4DC=Cyc_Absyn_compress(new_type);_tmp3E0.f2=_tmp4DC;});_tmp3E0;});struct _tuple0 _stmttmp11=_tmp120;struct _tuple0 _tmp121=_stmttmp11;struct Cyc_Absyn_PtrInfo _tmp123;struct Cyc_Absyn_PtrInfo _tmp122;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp121.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp121.f2)->tag == 3U){_LL1: _tmp122=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp121.f1)->f1;_tmp123=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp121.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pold=_tmp122;struct Cyc_Absyn_PtrInfo pnew=_tmp123;
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp125=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DF;_tmp3DF.tag=0U,({struct _fat_ptr _tmp4DE=({const char*_tmp126="alias requires pointer type";_tag_fat(_tmp126,sizeof(char),28U);});_tmp3DF.f1=_tmp4DE;});_tmp3DF;});void*_tmp124[1U];_tmp124[0]=& _tmp125;({unsigned _tmp4DF=loc;Cyc_Warn_err2(_tmp4DF,_tag_fat(_tmp124,sizeof(void*),1U));});});goto _LL0;}_LL0:;}else{
# 619
({struct Cyc_Warn_String_Warn_Warg_struct _tmp128=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E4;_tmp3E4.tag=0U,({struct _fat_ptr _tmp4E0=({const char*_tmp12D="cannot alias value of type ";_tag_fat(_tmp12D,sizeof(char),28U);});_tmp3E4.f1=_tmp4E0;});_tmp3E4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp129=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3E3;_tmp3E3.tag=2U,_tmp3E3.f1=(void*)old_type;_tmp3E3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E2;_tmp3E2.tag=0U,({struct _fat_ptr _tmp4E1=({const char*_tmp12C=" to type ";_tag_fat(_tmp12C,sizeof(char),10U);});_tmp3E2.f1=_tmp4E1;});_tmp3E2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp12B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3E1;_tmp3E1.tag=2U,_tmp3E1.f1=(void*)new_type;_tmp3E1;});void*_tmp127[4U];_tmp127[0]=& _tmp128,_tmp127[1]=& _tmp129,_tmp127[2]=& _tmp12A,_tmp127[3]=& _tmp12B;({unsigned _tmp4E2=loc;Cyc_Warn_err2(_tmp4E2,_tag_fat(_tmp127,sizeof(void*),4U));});});}}
# 625
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 627
void*_tmp131=p->r;void*_stmttmp12=_tmp131;void*_tmp132=_stmttmp12;struct Cyc_Absyn_Vardecl*_tmp134;struct Cyc_Absyn_Tvar*_tmp133;struct Cyc_Absyn_Pat*_tmp136;struct Cyc_Absyn_Vardecl*_tmp135;struct Cyc_Absyn_Pat*_tmp138;struct Cyc_Absyn_Vardecl*_tmp137;struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13C;union Cyc_Absyn_AggrInfo*_tmp13B;struct Cyc_Absyn_Pat*_tmp13E;switch(*((int*)_tmp132)){case 6U: _LL1: _tmp13E=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp132)->f1;_LL2: {struct Cyc_Absyn_Pat*p2=_tmp13E;
# 629
void*_tmp13F=(void*)_check_null(p->topt);void*_stmttmp13=_tmp13F;void*_tmp140=_stmttmp13;void*_tmp141;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140)->tag == 3U){_LL12: _tmp141=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140)->f1).ptr_atts).rgn;_LL13: {void*rt=_tmp141;
# 631
Cyc_Tcenv_check_rgn_accessible(te,p->loc,rt);
({struct Cyc_Tcenv_Tenv*_tmp4E5=te;struct Cyc_Absyn_Pat*_tmp4E4=p2;int _tmp4E3=Cyc_Tcutil_is_noalias_region(rt,0);Cyc_Tcpat_check_pat_regions_rec(_tmp4E5,_tmp4E4,_tmp4E3,patvars);});
return;}}else{_LL14: _LL15:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp143=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E5;_tmp3E5.tag=0U,({struct _fat_ptr _tmp4E6=({const char*_tmp144="check_pat_regions: bad pointer type";_tag_fat(_tmp144,sizeof(char),36U);});_tmp3E5.f1=_tmp4E6;});_tmp3E5;});void*_tmp142[1U];_tmp142[0]=& _tmp143;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp142,sizeof(void*),1U));});}_LL11:;}case 7U: _LL3: _tmp13B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp132)->f1;_tmp13C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp132)->f2;_tmp13D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp132)->f3;_LL4: {union Cyc_Absyn_AggrInfo*ai=_tmp13B;struct Cyc_List_List*exist_ts=_tmp13C;struct Cyc_List_List*dps=_tmp13D;
# 637
for(0;dps != 0;dps=dps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple16*)dps->hd)).f2,did_noalias_deref,patvars);}
return;}case 8U: _LL5: _tmp13A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp132)->f3;_LL6: {struct Cyc_List_List*ps=_tmp13A;
# 641
did_noalias_deref=0;_tmp139=ps;goto _LL8;}case 5U: _LL7: _tmp139=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp132)->f1;_LL8: {struct Cyc_List_List*ps=_tmp139;
# 643
for(0;ps != 0;ps=ps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)ps->hd,did_noalias_deref,patvars);}
return;}case 3U: _LL9: _tmp137=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp132)->f1;_tmp138=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp132)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp137;struct Cyc_Absyn_Pat*p2=_tmp138;
# 647
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp145=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp14=_tmp145;struct _tuple17*_tmp146=_stmttmp14;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Vardecl**_tmp147;_LL17: _tmp147=_tmp146->f1;_tmp148=_tmp146->f2;_LL18: {struct Cyc_Absyn_Vardecl**vdopt=_tmp147;struct Cyc_Absyn_Exp*eopt=_tmp148;
# 653
if((vdopt != 0 &&*vdopt == vd)&& eopt != 0){
{void*_tmp149=eopt->r;void*_stmttmp15=_tmp149;void*_tmp14A=_stmttmp15;struct Cyc_Absyn_Exp*_tmp14B;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp14A)->tag == 15U){_LL1A: _tmp14B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp14A)->f1;_LL1B: {struct Cyc_Absyn_Exp*e=_tmp14B;
# 656
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E6;_tmp3E6.tag=0U,({struct _fat_ptr _tmp4E7=({const char*_tmp14E="reference pattern not allowed on alias-free pointers";_tag_fat(_tmp14E,sizeof(char),53U);});_tmp3E6.f1=_tmp4E7;});_tmp3E6;});void*_tmp14C[1U];_tmp14C[0]=& _tmp14D;({unsigned _tmp4E8=p->loc;Cyc_Warn_err2(_tmp4E8,_tag_fat(_tmp14C,sizeof(void*),1U));});});
goto _LL19;}}else{_LL1C: _LL1D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp150=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E7;_tmp3E7.tag=0U,({struct _fat_ptr _tmp4E9=({const char*_tmp151="check_pat_regions: bad reference access exp";_tag_fat(_tmp151,sizeof(char),44U);});_tmp3E7.f1=_tmp4E9;});_tmp3E7;});void*_tmp14F[1U];_tmp14F[0]=& _tmp150;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp14F,sizeof(void*),1U));});}_LL19:;}
# 661
break;}}}}
# 664
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 1U: _LLB: _tmp135=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp132)->f1;_tmp136=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp132)->f2;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp135;struct Cyc_Absyn_Pat*p2=_tmp136;
# 667
{void*_tmp152=p->topt;void*_stmttmp16=_tmp152;void*_tmp153=_stmttmp16;if(_tmp153 != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp153)->tag == 4U){_LL1F: _LL20:
# 669
 if(!did_noalias_deref)
goto _LL1E;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp155=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E8;_tmp3E8.tag=0U,({struct _fat_ptr _tmp4EA=({const char*_tmp156="pattern to array would create alias of no-alias pointer";_tag_fat(_tmp156,sizeof(char),56U);});_tmp3E8.f1=_tmp4EA;});_tmp3E8;});void*_tmp154[1U];_tmp154[0]=& _tmp155;({unsigned _tmp4EB=p->loc;Cyc_Warn_err2(_tmp4EB,_tag_fat(_tmp154,sizeof(void*),1U));});});
return;}else{goto _LL21;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 675
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 2U: _LLD: _tmp133=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp132)->f1;_tmp134=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp132)->f2;_LLE: {struct Cyc_Absyn_Tvar*tv=_tmp133;struct Cyc_Absyn_Vardecl*vd=_tmp134;
# 678
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp157=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp17=_tmp157;struct _tuple17*_tmp158=_stmttmp17;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Vardecl**_tmp159;_LL24: _tmp159=_tmp158->f1;_tmp15A=_tmp158->f2;_LL25: {struct Cyc_Absyn_Vardecl**vdopt=_tmp159;struct Cyc_Absyn_Exp*eopt=_tmp15A;
# 682
if(vdopt != 0 &&*vdopt == vd){
if(eopt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E9;_tmp3E9.tag=0U,({struct _fat_ptr _tmp4EC=({const char*_tmp15D="cannot alias pattern expression in datatype";_tag_fat(_tmp15D,sizeof(char),44U);});_tmp3E9.f1=_tmp4EC;});_tmp3E9;});void*_tmp15B[1U];_tmp15B[0]=& _tmp15C;({unsigned _tmp4ED=p->loc;Cyc_Warn_err2(_tmp4ED,_tag_fat(_tmp15B,sizeof(void*),1U));});});else{
# 686
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp4EF=p->loc;struct Cyc_Tcenv_Tenv*_tmp4EE=te;Cyc_Tcenv_add_type_vars(_tmp4EF,_tmp4EE,({struct Cyc_Absyn_Tvar*_tmp15F[1U];_tmp15F[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp15F,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
te2=({struct Cyc_Tcenv_Tenv*_tmp4F0=te2;Cyc_Tcenv_add_region(_tmp4F0,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->tag=2U,_tmp15E->f1=tv;_tmp15E;}),0,0);});
# 689
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(eopt->topt),tv,vd->type,eopt);}
# 691
break;}}}}
# 694
goto _LL0;}default: _LLF: _LL10:
 return;}_LL0:;}
# 710 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 712
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple17*_tmp160=(struct _tuple17*)x->hd;struct _tuple17*_stmttmp18=_tmp160;struct _tuple17*_tmp161=_stmttmp18;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Vardecl**_tmp162;_LL1: _tmp162=_tmp161->f1;_tmp163=_tmp161->f2;_LL2: {struct Cyc_Absyn_Vardecl**vdopt=_tmp162;struct Cyc_Absyn_Exp*eopt=_tmp163;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp164=eopt;struct Cyc_Absyn_Exp*e=_tmp164;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))&& !Cyc_Tcutil_is_noalias_path(e)){
# 719
if(vdopt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp166=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EB;_tmp3EB.tag=0U,({struct _fat_ptr _tmp4F1=({const char*_tmp169="pattern dereferences an alias-free-pointer ";_tag_fat(_tmp169,sizeof(char),44U);});_tmp3EB.f1=_tmp4F1;});_tmp3EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp167=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EA;_tmp3EA.tag=0U,({
struct _fat_ptr _tmp4F2=({const char*_tmp168="from a non-unique path";_tag_fat(_tmp168,sizeof(char),23U);});_tmp3EA.f1=_tmp4F2;});_tmp3EA;});void*_tmp165[2U];_tmp165[0]=& _tmp166,_tmp165[1]=& _tmp167;({unsigned _tmp4F3=p->loc;Cyc_Warn_err2(_tmp4F3,_tag_fat(_tmp165,sizeof(void*),2U));});});else{
# 723
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EE;_tmp3EE.tag=0U,({struct _fat_ptr _tmp4F4=({const char*_tmp16F="pattern for variable ";_tag_fat(_tmp16F,sizeof(char),22U);});_tmp3EE.f1=_tmp4F4;});_tmp3EE;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp16C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3ED;_tmp3ED.tag=1U,_tmp3ED.f1=(*vdopt)->name;_tmp3ED;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EC;_tmp3EC.tag=0U,({
struct _fat_ptr _tmp4F5=({const char*_tmp16E=" dereferences an alias-free-pointer from a non-unique path";_tag_fat(_tmp16E,sizeof(char),59U);});_tmp3EC.f1=_tmp4F5;});_tmp3EC;});void*_tmp16A[3U];_tmp16A[0]=& _tmp16B,_tmp16A[1]=& _tmp16C,_tmp16A[2]=& _tmp16D;({unsigned _tmp4F6=p->loc;Cyc_Warn_err2(_tmp4F6,_tag_fat(_tmp16A,sizeof(void*),3U));});});}}}}}}}
# 770 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1U};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2U};
# 780
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0U};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1U};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 794
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){return({union Cyc_Tcpat_Name_value _tmp3EF;(_tmp3EF.Name_v).tag=1U,(_tmp3EF.Name_v).val=s;_tmp3EF;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp3F0;(_tmp3F0.Int_v).tag=2U,(_tmp3F0.Int_v).val=i;_tmp3F0;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 811
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp170=c1->name;union Cyc_Tcpat_Name_value _stmttmp19=_tmp170;union Cyc_Tcpat_Name_value _tmp171=_stmttmp19;int _tmp172;struct _fat_ptr _tmp173;if((_tmp171.Name_v).tag == 1){_LL1: _tmp173=(_tmp171.Name_v).val;_LL2: {struct _fat_ptr n1=_tmp173;
# 814
union Cyc_Tcpat_Name_value _tmp174=c2->name;union Cyc_Tcpat_Name_value _stmttmp1A=_tmp174;union Cyc_Tcpat_Name_value _tmp175=_stmttmp1A;struct _fat_ptr _tmp176;if((_tmp175.Name_v).tag == 1){_LL6: _tmp176=(_tmp175.Name_v).val;_LL7: {struct _fat_ptr n2=_tmp176;
return Cyc_strcmp((struct _fat_ptr)n1,(struct _fat_ptr)n2);}}else{_LL8: _LL9:
 return - 1;}_LL5:;}}else{_LL3: _tmp172=(_tmp171.Int_v).val;_LL4: {int i1=_tmp172;
# 819
union Cyc_Tcpat_Name_value _tmp177=c2->name;union Cyc_Tcpat_Name_value _stmttmp1B=_tmp177;union Cyc_Tcpat_Name_value _tmp178=_stmttmp1B;int _tmp179;if((_tmp178.Name_v).tag == 1){_LLB: _LLC:
 return 1;}else{_LLD: _tmp179=(_tmp178.Int_v).val;_LLE: {int i2=_tmp179;
return i1 - i2;}}_LLA:;}}_LL0:;}
# 827
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set (void){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 831
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 835
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned ans=0U;
struct Cyc_List_List*_tmp17A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp17A;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 842
return ans;}
# 845
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp17B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp17B;for(0;fields != 0;(fields=fields->tl,++ i)){
if(Cyc_strcmp((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,(struct _fat_ptr)*f)== 0)return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F3;_tmp3F3.tag=0U,({struct _fat_ptr _tmp4F7=({const char*_tmp181="get_member_offset ";_tag_fat(_tmp181,sizeof(char),19U);});_tmp3F3.f1=_tmp4F7;});_tmp3F3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F2;_tmp3F2.tag=0U,_tmp3F2.f1=*f;_tmp3F2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F1;_tmp3F1.tag=0U,({struct _fat_ptr _tmp4F8=({const char*_tmp180=" failed";_tag_fat(_tmp180,sizeof(char),8U);});_tmp3F1.f1=_tmp4F8;});_tmp3F1;});void*_tmp17C[3U];_tmp17C[0]=& _tmp17D,_tmp17C[1]=& _tmp17E,_tmp17C[2]=& _tmp17F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp17C,sizeof(void*),3U));});}
# 852
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp182=pw;struct Cyc_Absyn_Pat*_tmp183;struct Cyc_Absyn_Exp*_tmp184;if((_tmp182.where_clause).tag == 2){_LL1: _tmp184=(_tmp182.where_clause).val;_LL2: {struct Cyc_Absyn_Exp*e=_tmp184;
return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->tag=0U,_tmp185->f1=e;_tmp185;});}}else{_LL3: _tmp183=(_tmp182.pattern).val;_LL4: {struct Cyc_Absyn_Pat*p=_tmp183;
# 856
void*_tmp186=p->r;void*_stmttmp1C=_tmp186;void*_tmp187=_stmttmp1C;struct Cyc_List_List*_tmp189;union Cyc_Absyn_AggrInfo _tmp188;struct Cyc_Absyn_Datatypefield*_tmp18B;struct Cyc_Absyn_Datatypedecl*_tmp18A;struct Cyc_Absyn_Enumfield*_tmp18D;void*_tmp18C;struct Cyc_Absyn_Enumfield*_tmp18F;struct Cyc_Absyn_Enumdecl*_tmp18E;int _tmp191;struct _fat_ptr _tmp190;char _tmp192;int _tmp194;enum Cyc_Absyn_Sign _tmp193;struct Cyc_Absyn_Pat*_tmp195;struct Cyc_Absyn_Pat*_tmp196;switch(*((int*)_tmp187)){case 1U: _LL6: _tmp196=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp187)->f2;_LL7: {struct Cyc_Absyn_Pat*p1=_tmp196;
_tmp195=p1;goto _LL9;}case 3U: _LL8: _tmp195=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp187)->f2;_LL9: {struct Cyc_Absyn_Pat*p1=_tmp195;
return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp3F4;(_tmp3F4.pattern).tag=1U,(_tmp3F4.pattern).val=p1;_tmp3F4;}));}case 9U: _LLA: _LLB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10U: _LLC: _tmp193=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp187)->f1;_tmp194=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp187)->f2;_LLD: {enum Cyc_Absyn_Sign s=_tmp193;int i=_tmp194;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->tag=6U,_tmp197->f1=(unsigned)i;_tmp197;});}case 11U: _LLE: _tmp192=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp187)->f1;_LLF: {char c=_tmp192;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->tag=6U,_tmp198->f1=(unsigned)c;_tmp198;});}case 12U: _LL10: _tmp190=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp187)->f1;_tmp191=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp187)->f2;_LL11: {struct _fat_ptr f=_tmp190;int i=_tmp191;
return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->tag=5U,_tmp199->f1=f,_tmp199->f2=i;_tmp199;});}case 13U: _LL12: _tmp18E=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp187)->f1;_tmp18F=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp187)->f2;_LL13: {struct Cyc_Absyn_Enumdecl*ed=_tmp18E;struct Cyc_Absyn_Enumfield*ef=_tmp18F;
return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->tag=3U,_tmp19A->f1=ed,_tmp19A->f2=ef;_tmp19A;});}case 14U: _LL14: _tmp18C=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp187)->f1;_tmp18D=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp187)->f2;_LL15: {void*t=_tmp18C;struct Cyc_Absyn_Enumfield*ef=_tmp18D;
return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->tag=4U,_tmp19B->f1=t,_tmp19B->f2=ef;_tmp19B;});}case 6U: _LL16: _LL17:
# 866
 if(!Cyc_Tcutil_is_pointer_type((void*)_check_null(p->topt))|| !
Cyc_Tcutil_is_nullable_pointer_type((void*)_check_null(p->topt),0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F5;_tmp3F5.tag=0U,({struct _fat_ptr _tmp4F9=({const char*_tmp19E="non-null pointer type or non-pointer type in pointer pattern";_tag_fat(_tmp19E,sizeof(char),61U);});_tmp3F5.f1=_tmp4F9;});_tmp3F5;});void*_tmp19C[1U];_tmp19C[0]=& _tmp19D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp19C,sizeof(void*),1U));});
return(void*)& Cyc_Tcpat_NeqNull_val;case 8U: _LL18: _tmp18A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp187)->f1;_tmp18B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp187)->f2;_LL19: {struct Cyc_Absyn_Datatypedecl*ddecl=_tmp18A;struct Cyc_Absyn_Datatypefield*df=_tmp18B;
# 871
if(ddecl->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->tag=9U,_tmp19F->f1=ddecl,_tmp19F->f2=df;_tmp19F;});
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->tag=7U,({int _tmp4FA=(int)Cyc_Tcpat_datatype_tag_number(ddecl,df->name);_tmp1A0->f1=_tmp4FA;}),_tmp1A0->f2=ddecl,_tmp1A0->f3=df;_tmp1A0;});}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp187)->f1 != 0){_LL1A: _tmp188=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp187)->f1;_tmp189=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp187)->f3;_LL1B: {union Cyc_Absyn_AggrInfo info=_tmp188;struct Cyc_List_List*dlps=_tmp189;
# 875
struct Cyc_Absyn_Aggrdecl*_tmp1A1=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A1;
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F6;_tmp3F6.tag=0U,({struct _fat_ptr _tmp4FB=({const char*_tmp1A4="non-tagged aggregate in pattern test";_tag_fat(_tmp1A4,sizeof(char),37U);});_tmp3F6.f1=_tmp4FB;});_tmp3F6;});void*_tmp1A2[1U];_tmp1A2[0]=& _tmp1A3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1A2,sizeof(void*),1U));});{
void*_tmp1A5=(void*)((struct Cyc_List_List*)_check_null((*((struct _tuple16*)((struct Cyc_List_List*)_check_null(dlps))->hd)).f1))->hd;void*_stmttmp1D=_tmp1A5;void*_tmp1A6=_stmttmp1D;struct _fat_ptr*_tmp1A7;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1A6)->tag == 1U){_LL1F: _tmp1A7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1A6)->f1;_LL20: {struct _fat_ptr*f=_tmp1A7;
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->tag=8U,_tmp1A8->f1=f,({int _tmp4FC=Cyc_Tcpat_get_member_offset(ad,f);_tmp1A8->f2=_tmp4FC;});_tmp1A8;});}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F7;_tmp3F7.tag=0U,({struct _fat_ptr _tmp4FD=({const char*_tmp1AB="no field name in tagged union pattern";_tag_fat(_tmp1AB,sizeof(char),38U);});_tmp3F7.f1=_tmp4FD;});_tmp3F7;});void*_tmp1A9[1U];_tmp1A9[0]=& _tmp1AA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1A9,sizeof(void*),1U));});}_LL1E:;}}}else{goto _LL1C;}default: _LL1C: _LL1D:
# 882
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F8;_tmp3F8.tag=0U,({struct _fat_ptr _tmp4FE=({const char*_tmp1AE="non-test pattern in pattern test";_tag_fat(_tmp1AE,sizeof(char),33U);});_tmp3F8.f1=_tmp4FE;});_tmp3F8;});void*_tmp1AC[1U];_tmp1AC[0]=& _tmp1AD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1AC,sizeof(void*),1U));});}_LL5:;}}_LL0:;}
# 887
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp3F9;(_tmp3F9.pattern).tag=1U,(_tmp3F9.pattern).val=p;_tmp3F9;});}
# 891
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));({union Cyc_Tcpat_Name_value _tmp500=Cyc_Tcpat_Name_v(({const char*_tmp1AF="NULL";_tag_fat(_tmp1AF,sizeof(char),5U);}));_tmp1B0->name=_tmp500;}),_tmp1B0->arity=0,_tmp1B0->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4FF=Cyc_Tcpat_pw(p);_tmp1B0->orig_pat=_tmp4FF;});_tmp1B0;});}
# 894
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));({union Cyc_Tcpat_Name_value _tmp502=Cyc_Tcpat_Name_v(({const char*_tmp1B1="&";_tag_fat(_tmp1B1,sizeof(char),2U);}));_tmp1B2->name=_tmp502;}),_tmp1B2->arity=1,_tmp1B2->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp501=Cyc_Tcpat_pw(p);_tmp1B2->orig_pat=_tmp501;});_tmp1B2;});}
# 897
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));({union Cyc_Tcpat_Name_value _tmp504=Cyc_Tcpat_Name_v(({const char*_tmp1B3="&";_tag_fat(_tmp1B3,sizeof(char),2U);}));_tmp1B4->name=_tmp504;}),_tmp1B4->arity=1,_tmp1B4->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _tmp503=Cyc_Tcpat_pw(p);_tmp1B4->orig_pat=_tmp503;});_tmp1B4;});}
# 900
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));({union Cyc_Tcpat_Name_value _tmp505=Cyc_Tcpat_Int_v(i);_tmp1B5->name=_tmp505;}),_tmp1B5->arity=0,_tmp1B5->span=0,_tmp1B5->orig_pat=p;_tmp1B5;});}
# 903
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));({union Cyc_Tcpat_Name_value _tmp507=Cyc_Tcpat_Name_v(f);_tmp1B6->name=_tmp507;}),_tmp1B6->arity=0,_tmp1B6->span=0,({union Cyc_Tcpat_PatOrWhere _tmp506=Cyc_Tcpat_pw(p);_tmp1B6->orig_pat=_tmp506;});_tmp1B6;});}
# 906
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));({union Cyc_Tcpat_Name_value _tmp509=Cyc_Tcpat_Int_v((int)c);_tmp1B7->name=_tmp509;}),_tmp1B7->arity=0,_tmp1B7->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _tmp508=Cyc_Tcpat_pw(p);_tmp1B7->orig_pat=_tmp508;});_tmp1B7;});}
# 909
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({union Cyc_Tcpat_Name_value _tmp50A=Cyc_Tcpat_Name_v(({const char*_tmp1B8="$";_tag_fat(_tmp1B8,sizeof(char),2U);}));_tmp1B9->name=_tmp50A;}),_tmp1B9->arity=i,_tmp1B9->span=& Cyc_Tcpat_one_opt,_tmp1B9->orig_pat=p;_tmp1B9;});}
# 914
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp50B=Cyc_Tcpat_null_con(p);_tmp1BA->f1=_tmp50B;}),_tmp1BA->f2=0;_tmp1BA;});}
# 917
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp50C=Cyc_Tcpat_int_con(i,p);_tmp1BB->f1=_tmp50C;}),_tmp1BB->f2=0;_tmp1BB;});}
# 920
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp50D=Cyc_Tcpat_char_con(c,p);_tmp1BC->f1=_tmp50D;}),_tmp1BC->f2=0;_tmp1BC;});}
# 923
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp50E=Cyc_Tcpat_float_con(f,p);_tmp1BD->f1=_tmp50E;}),_tmp1BD->f2=0;_tmp1BD;});}
# 926
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp510=Cyc_Tcpat_null_ptr_con(p0);_tmp1BF->f1=_tmp510;}),({struct Cyc_List_List*_tmp50F=({struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=p,_tmp1BE->tl=0;_tmp1BE;});_tmp1BF->f2=_tmp50F;});_tmp1BF;});}
# 929
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp512=Cyc_Tcpat_ptr_con(p0);_tmp1C1->f1=_tmp512;}),({struct Cyc_List_List*_tmp511=({struct Cyc_List_List*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->hd=p,_tmp1C0->tl=0;_tmp1C0;});_tmp1C1->f2=_tmp511;});_tmp1C1;});}
# 932
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp514=({int _tmp513=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp513,p);});_tmp1C2->f1=_tmp514;}),_tmp1C2->f2=ss;_tmp1C2;});}
# 935
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 937
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));({union Cyc_Tcpat_Name_value _tmp517=Cyc_Tcpat_Name_v(con_name);_tmp1C4->name=_tmp517;}),({int _tmp516=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp1C4->arity=_tmp516;}),_tmp1C4->span=span,({union Cyc_Tcpat_PatOrWhere _tmp515=Cyc_Tcpat_pw(p);_tmp1C4->orig_pat=_tmp515;});_tmp1C4;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->tag=1U,_tmp1C3->f1=c,_tmp1C3->f2=ps;_tmp1C3;});}
# 942
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1C5=p->r;void*_stmttmp1E=_tmp1C5;void*_tmp1C6=_stmttmp1E;struct Cyc_Absyn_Enumfield*_tmp1C7;struct Cyc_Absyn_Enumfield*_tmp1C8;struct Cyc_List_List*_tmp1CA;struct Cyc_Absyn_Aggrdecl*_tmp1C9;struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_Datatypefield*_tmp1CD;struct Cyc_Absyn_Datatypedecl*_tmp1CC;struct Cyc_Absyn_Pat*_tmp1CF;struct Cyc_Absyn_Pat*_tmp1D0;struct Cyc_Absyn_Pat*_tmp1D1;struct _fat_ptr _tmp1D2;char _tmp1D3;int _tmp1D5;enum Cyc_Absyn_Sign _tmp1D4;switch(*((int*)_tmp1C6)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->tag=0U;_tmp1D6;});goto _LL0;case 9U: _LL7: _LL8:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 10U: _LL9: _tmp1D4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1;_tmp1D5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C6)->f2;_LLA: {enum Cyc_Absyn_Sign sn=_tmp1D4;int i=_tmp1D5;
s=({int _tmp518=i;Cyc_Tcpat_int_pat(_tmp518,Cyc_Tcpat_pw(p));});goto _LL0;}case 11U: _LLB: _tmp1D3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1;_LLC: {char c=_tmp1D3;
s=Cyc_Tcpat_char_pat(c,p);goto _LL0;}case 12U: _LLD: _tmp1D2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1;_LLE: {struct _fat_ptr f=_tmp1D2;
s=Cyc_Tcpat_float_pat(f,p);goto _LL0;}case 1U: _LLF: _tmp1D1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C6)->f2;_LL10: {struct Cyc_Absyn_Pat*p2=_tmp1D1;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 3U: _LL11: _tmp1D0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C6)->f2;_LL12: {struct Cyc_Absyn_Pat*p2=_tmp1D0;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 6U: _LL13: _tmp1CF=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1;_LL14: {struct Cyc_Absyn_Pat*pp=_tmp1CF;
# 955
{void*_tmp1D7=Cyc_Absyn_compress((void*)_check_null(p->topt));void*_stmttmp1F=_tmp1D7;void*_tmp1D8=_stmttmp1F;void*_tmp1D9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D8)->tag == 3U){_LL28: _tmp1D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D8)->f1).ptr_atts).nullable;_LL29: {void*n=_tmp1D9;
# 957
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
void*ss=Cyc_Tcpat_compile_pat(pp);
s=is_nullable?Cyc_Tcpat_null_ptr_pat(ss,p): Cyc_Tcpat_ptr_pat(ss,p);
goto _LL27;}}else{_LL2A: _LL2B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FA;_tmp3FA.tag=0U,({struct _fat_ptr _tmp519=({const char*_tmp1DC="expecting pointertype for pattern!";_tag_fat(_tmp1DC,sizeof(char),35U);});_tmp3FA.f1=_tmp519;});_tmp3FA;});void*_tmp1DA[1U];_tmp1DA[0]=& _tmp1DB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1DA,sizeof(void*),1U));});}_LL27:;}
# 963
goto _LL0;}case 8U: _LL15: _tmp1CC=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1;_tmp1CD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C6)->f2;_tmp1CE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C6)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1CC;struct Cyc_Absyn_Datatypefield*tuf=_tmp1CD;struct Cyc_List_List*ps=_tmp1CE;
# 965
int*span;
{void*_tmp1DD=Cyc_Absyn_compress((void*)_check_null(p->topt));void*_stmttmp20=_tmp1DD;void*_tmp1DE=_stmttmp20;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->f1)){case 18U: _LL2D: _LL2E:
# 968
 span=tud->is_extensible?0:({int*_tmp1DF=_cycalloc_atomic(sizeof(*_tmp1DF));({int _tmp51A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v);*_tmp1DF=_tmp51A;});_tmp1DF;});
goto _LL2C;case 19U: _LL2F: _LL30:
 span=& Cyc_Tcpat_one_opt;goto _LL2C;default: goto _LL31;}else{_LL31: _LL32:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FB;_tmp3FB.tag=0U,({struct _fat_ptr _tmp51B=({const char*_tmp1E2="void datatype pattern has bad type";_tag_fat(_tmp1E2,sizeof(char),35U);});_tmp3FB.f1=_tmp51B;});_tmp3FB;});void*_tmp1E0[1U];_tmp1E0[0]=& _tmp1E1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1E0,sizeof(void*),1U));});}_LL2C:;}
# 973
s=({struct _fat_ptr _tmp51E=*(*tuf->name).f2;int*_tmp51D=span;struct Cyc_List_List*_tmp51C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_con_pat(_tmp51E,_tmp51D,_tmp51C,p);});
goto _LL0;}case 5U: _LL17: _tmp1CB=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1;_LL18: {struct Cyc_List_List*ps=_tmp1CB;
# 976
s=({struct Cyc_List_List*_tmp51F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_tuple_pat(_tmp51F,Cyc_Tcpat_pw(p));});goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1)->KnownAggr).tag == 2){_LL19: _tmp1C9=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C6)->f1)->KnownAggr).val;_tmp1CA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C6)->f3;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1C9;struct Cyc_List_List*dlps=_tmp1CA;
# 981
if((int)ad->kind == (int)Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 985
int found=({struct _fat_ptr _tmp520=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp520,({const char*_tmp1F0="";_tag_fat(_tmp1F0,sizeof(char),1U);}));})== 0;
{struct Cyc_List_List*dlps0=dlps;for(0;!found && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple16*_tmp1E3=(struct _tuple16*)dlps0->hd;struct _tuple16*_stmttmp21=_tmp1E3;struct _tuple16*_tmp1E4=_stmttmp21;struct Cyc_Absyn_Pat*_tmp1E6;struct Cyc_List_List*_tmp1E5;_LL34: _tmp1E5=_tmp1E4->f1;_tmp1E6=_tmp1E4->f2;_LL35: {struct Cyc_List_List*dl=_tmp1E5;struct Cyc_Absyn_Pat*p=_tmp1E6;
struct Cyc_List_List*_tmp1E7=dl;struct _fat_ptr*_tmp1E8;if(_tmp1E7 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp1E7)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp1E7)->tl == 0){_LL37: _tmp1E8=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1E7->hd)->f1;_LL38: {struct _fat_ptr*f=_tmp1E8;
# 990
if(Cyc_strptrcmp(f,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));({void*_tmp521=Cyc_Tcpat_compile_pat(p);_tmp1E9->hd=_tmp521;}),_tmp1E9->tl=ps;_tmp1E9;});
found=1;}
# 994
goto _LL36;}}else{goto _LL39;}}else{goto _LL39;}}else{_LL39: _LL3A:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FC;_tmp3FC.tag=0U,({struct _fat_ptr _tmp522=({const char*_tmp1EC="bad designator(s)";_tag_fat(_tmp1EC,sizeof(char),18U);});_tmp3FC.f1=_tmp522;});_tmp3FC;});void*_tmp1EA[1U];_tmp1EA[0]=& _tmp1EB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1EA,sizeof(void*),1U));});}_LL36:;}}}
# 998
if(!found)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FD;_tmp3FD.tag=0U,({struct _fat_ptr _tmp523=({const char*_tmp1EF="bad designator";_tag_fat(_tmp1EF,sizeof(char),15U);});_tmp3FD.f1=_tmp523;});_tmp3FD;});void*_tmp1ED[1U];_tmp1ED[0]=& _tmp1EE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1ED,sizeof(void*),1U));});}}
# 1001
s=({struct Cyc_List_List*_tmp524=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp524,Cyc_Tcpat_pw(p));});}else{
# 1004
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FE;_tmp3FE.tag=0U,({struct _fat_ptr _tmp525=({const char*_tmp1F3="patterns on untagged unions not yet supported.";_tag_fat(_tmp1F3,sizeof(char),47U);});_tmp3FE.f1=_tmp525;});_tmp3FE;});void*_tmp1F1[1U];_tmp1F1[0]=& _tmp1F2;({unsigned _tmp526=p->loc;Cyc_Warn_err2(_tmp526,_tag_fat(_tmp1F1,sizeof(void*),1U));});});{
int*span=({int*_tmp1FB=_cycalloc_atomic(sizeof(*_tmp1FB));({int _tmp527=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);*_tmp1FB=_tmp527;});_tmp1FB;});
struct Cyc_List_List*_tmp1F4=dlps;struct Cyc_Absyn_Pat*_tmp1F6;struct _fat_ptr*_tmp1F5;if(_tmp1F4 != 0){if(((struct _tuple16*)((struct Cyc_List_List*)_tmp1F4)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp1F4)->hd)->f1)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp1F4)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp1F4)->tl == 0){_LL3C: _tmp1F5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple16*)_tmp1F4->hd)->f1)->hd)->f1;_tmp1F6=((struct _tuple16*)_tmp1F4->hd)->f2;_LL3D: {struct _fat_ptr*f=_tmp1F5;struct Cyc_Absyn_Pat*p2=_tmp1F6;
# 1009
s=({struct _fat_ptr _tmp52B=*f;int*_tmp52A=span;struct Cyc_List_List*_tmp529=({struct Cyc_List_List*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));({void*_tmp528=Cyc_Tcpat_compile_pat(p2);_tmp1F7->hd=_tmp528;}),_tmp1F7->tl=0;_tmp1F7;});Cyc_Tcpat_con_pat(_tmp52B,_tmp52A,_tmp529,p);});
goto _LL3B;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{_LL3E: _LL3F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FF;_tmp3FF.tag=0U,({struct _fat_ptr _tmp52C=({const char*_tmp1FA="bad union pattern";_tag_fat(_tmp1FA,sizeof(char),18U);});_tmp3FF.f1=_tmp52C;});_tmp3FF;});void*_tmp1F8[1U];_tmp1F8[0]=& _tmp1F9;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1F8,sizeof(void*),1U));});}_LL3B:;}}
# 1014
goto _LL0;}}else{goto _LL23;}}else{_LL23: _LL24:
# 1026
 goto _LL26;}case 13U: _LL1B: _tmp1C8=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1C6)->f2;_LL1C: {struct Cyc_Absyn_Enumfield*ef=_tmp1C8;
# 1021
_tmp1C7=ef;goto _LL1E;}case 14U: _LL1D: _tmp1C7=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1C6)->f2;_LL1E: {struct Cyc_Absyn_Enumfield*ef=_tmp1C7;
s=Cyc_Tcpat_con_pat(*(*ef->name).f2,0,0,p);goto _LL0;}case 15U: _LL1F: _LL20:
# 1024
 goto _LL22;case 16U: _LL21: _LL22:
 goto _LL24;default: _LL25: _LL26:
# 1027
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->tag=0U;_tmp1FC;});}_LL0:;}
# 1029
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1054
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp1FD=({struct _tuple0 _tmp400;_tmp400.f1=a1,_tmp400.f2=a2;_tmp400;});struct _tuple0 _stmttmp22=_tmp1FD;struct _tuple0 _tmp1FE=_stmttmp22;struct _fat_ptr*_tmp202;int _tmp201;struct _fat_ptr*_tmp200;int _tmp1FF;unsigned _tmp206;struct Cyc_Absyn_Datatypefield*_tmp205;unsigned _tmp204;struct Cyc_Absyn_Datatypefield*_tmp203;unsigned _tmp208;unsigned _tmp207;switch(*((int*)_tmp1FE.f1)){case 0U: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp1FE.f2)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LLB;}case 1U: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp1FE.f2)->tag == 1U){_LL3: _LL4:
 return 1;}else{goto _LLB;}case 2U: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1FE.f2)->tag == 2U){_LL5: _tmp207=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1FE.f1)->f1;_tmp208=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1FE.f2)->f1;_LL6: {unsigned i1=_tmp207;unsigned i2=_tmp208;
return i1 == i2;}}else{goto _LLB;}case 3U: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1FE.f2)->tag == 3U){_LL7: _tmp203=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1FE.f1)->f2;_tmp204=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1FE.f1)->f3;_tmp205=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1FE.f2)->f2;_tmp206=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1FE.f2)->f3;_LL8: {struct Cyc_Absyn_Datatypefield*df1=_tmp203;unsigned i1=_tmp204;struct Cyc_Absyn_Datatypefield*df2=_tmp205;unsigned i2=_tmp206;
# 1060
return df1 == df2 && i1 == i2;}}else{goto _LLB;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1FE.f2)->tag == 4U){_LL9: _tmp1FF=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1FE.f1)->f1;_tmp200=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1FE.f1)->f2;_tmp201=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1FE.f2)->f1;_tmp202=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1FE.f2)->f2;_LLA: {int b1=_tmp1FF;struct _fat_ptr*f1=_tmp200;int b2=_tmp201;struct _fat_ptr*f2=_tmp202;
# 1062
return b1 == b2 && Cyc_strptrcmp(f1,f2)== 0;}}else{_LLB: _LLC:
 return 0;}}_LL0:;}
# 1067
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
for(0;p1 != 0 && p2 != 0;(p1=p1->tl,p2=p2->tl)){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;}
if(p1 != p2)return 0;
return 1;}
# 1074
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp209=Cyc_Tcpat_get_pat_test(con->orig_pat);void*test=_tmp209;
void*_tmp20A=d2;void*_tmp20D;struct Cyc_List_List*_tmp20C;struct Cyc_List_List*_tmp20B;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp20A)->tag == 2U){_LL1: _tmp20B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp20A)->f1;_tmp20C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp20A)->f2;_tmp20D=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp20A)->f3;_LL2: {struct Cyc_List_List*access2=_tmp20B;struct Cyc_List_List*switch_clauses=_tmp20C;void*default_decision=_tmp20D;
# 1078
if(Cyc_Tcpat_same_path(access,access2)&&(int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->tag=2U,_tmp210->f1=access2,({
struct Cyc_List_List*_tmp52E=({struct Cyc_List_List*_tmp20F=_cycalloc(sizeof(*_tmp20F));({struct _tuple0*_tmp52D=({struct _tuple0*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->f1=test,_tmp20E->f2=d1;_tmp20E;});_tmp20F->hd=_tmp52D;}),_tmp20F->tl=switch_clauses;_tmp20F;});_tmp210->f2=_tmp52E;}),_tmp210->f3=default_decision;_tmp210;});
# 1082
goto _LL4;}}else{_LL3: _LL4:
 return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->tag=2U,_tmp213->f1=access,({struct Cyc_List_List*_tmp530=({struct Cyc_List_List*_tmp212=_cycalloc(sizeof(*_tmp212));({struct _tuple0*_tmp52F=({struct _tuple0*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->f1=test,_tmp211->f2=d1;_tmp211;});_tmp212->hd=_tmp52F;}),_tmp212->tl=0;_tmp212;});_tmp213->f2=_tmp530;}),_tmp213->f3=d2;_tmp213;});}_LL0:;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1095
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1098
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp214=0U;({struct Cyc___cycFILE*_tmp532=Cyc_stderr;struct _fat_ptr _tmp531=({const char*_tmp215=" ";_tag_fat(_tmp215,sizeof(char),2U);});Cyc_fprintf(_tmp532,_tmp531,_tag_fat(_tmp214,sizeof(void*),0U));});});}}
# 1102
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp216=c->name;union Cyc_Tcpat_Name_value _stmttmp23=_tmp216;union Cyc_Tcpat_Name_value _tmp217=_stmttmp23;int _tmp218;struct _fat_ptr _tmp219;if((_tmp217.Name_v).tag == 1){_LL1: _tmp219=(_tmp217.Name_v).val;_LL2: {struct _fat_ptr s=_tmp219;
({struct Cyc_String_pa_PrintArg_struct _tmp21C=({struct Cyc_String_pa_PrintArg_struct _tmp401;_tmp401.tag=0U,_tmp401.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp401;});void*_tmp21A[1U];_tmp21A[0]=& _tmp21C;({struct Cyc___cycFILE*_tmp534=Cyc_stderr;struct _fat_ptr _tmp533=({const char*_tmp21B="%s";_tag_fat(_tmp21B,sizeof(char),3U);});Cyc_fprintf(_tmp534,_tmp533,_tag_fat(_tmp21A,sizeof(void*),1U));});});goto _LL0;}}else{_LL3: _tmp218=(_tmp217.Int_v).val;_LL4: {int i=_tmp218;
({struct Cyc_Int_pa_PrintArg_struct _tmp21F=({struct Cyc_Int_pa_PrintArg_struct _tmp402;_tmp402.tag=1U,_tmp402.f1=(unsigned long)i;_tmp402;});void*_tmp21D[1U];_tmp21D[0]=& _tmp21F;({struct Cyc___cycFILE*_tmp536=Cyc_stderr;struct _fat_ptr _tmp535=({const char*_tmp21E="%d";_tag_fat(_tmp21E,sizeof(char),3U);});Cyc_fprintf(_tmp536,_tmp535,_tag_fat(_tmp21D,sizeof(void*),1U));});});goto _LL0;}}_LL0:;}
# 1109
static void Cyc_Tcpat_print_access(void*a){
void*_tmp220=a;struct _fat_ptr*_tmp222;int _tmp221;unsigned _tmp224;struct Cyc_Absyn_Datatypefield*_tmp223;unsigned _tmp225;switch(*((int*)_tmp220)){case 0U: _LL1: _LL2:
({void*_tmp226=0U;({struct Cyc___cycFILE*_tmp538=Cyc_stderr;struct _fat_ptr _tmp537=({const char*_tmp227="DUMMY";_tag_fat(_tmp227,sizeof(char),6U);});Cyc_fprintf(_tmp538,_tmp537,_tag_fat(_tmp226,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp228=0U;({struct Cyc___cycFILE*_tmp53A=Cyc_stderr;struct _fat_ptr _tmp539=({const char*_tmp229="*";_tag_fat(_tmp229,sizeof(char),2U);});Cyc_fprintf(_tmp53A,_tmp539,_tag_fat(_tmp228,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp225=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp220)->f1;_LL6: {unsigned i=_tmp225;
({struct Cyc_Int_pa_PrintArg_struct _tmp22C=({struct Cyc_Int_pa_PrintArg_struct _tmp403;_tmp403.tag=1U,_tmp403.f1=(unsigned long)((int)i);_tmp403;});void*_tmp22A[1U];_tmp22A[0]=& _tmp22C;({struct Cyc___cycFILE*_tmp53C=Cyc_stderr;struct _fat_ptr _tmp53B=({const char*_tmp22B="[%d]";_tag_fat(_tmp22B,sizeof(char),5U);});Cyc_fprintf(_tmp53C,_tmp53B,_tag_fat(_tmp22A,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL7: _tmp223=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp220)->f2;_tmp224=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp220)->f3;_LL8: {struct Cyc_Absyn_Datatypefield*f=_tmp223;unsigned i=_tmp224;
# 1115
({struct Cyc_String_pa_PrintArg_struct _tmp22F=({struct Cyc_String_pa_PrintArg_struct _tmp405;_tmp405.tag=0U,({struct _fat_ptr _tmp53D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp405.f1=_tmp53D;});_tmp405;});struct Cyc_Int_pa_PrintArg_struct _tmp230=({struct Cyc_Int_pa_PrintArg_struct _tmp404;_tmp404.tag=1U,_tmp404.f1=(unsigned long)((int)i);_tmp404;});void*_tmp22D[2U];_tmp22D[0]=& _tmp22F,_tmp22D[1]=& _tmp230;({struct Cyc___cycFILE*_tmp53F=Cyc_stderr;struct _fat_ptr _tmp53E=({const char*_tmp22E="%s[%d]";_tag_fat(_tmp22E,sizeof(char),7U);});Cyc_fprintf(_tmp53F,_tmp53E,_tag_fat(_tmp22D,sizeof(void*),2U));});});goto _LL0;}default: _LL9: _tmp221=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp220)->f1;_tmp222=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp220)->f2;_LLA: {int tagged=_tmp221;struct _fat_ptr*f=_tmp222;
# 1117
if(tagged)
({void*_tmp231=0U;({struct Cyc___cycFILE*_tmp541=Cyc_stderr;struct _fat_ptr _tmp540=({const char*_tmp232=".tagunion";_tag_fat(_tmp232,sizeof(char),10U);});Cyc_fprintf(_tmp541,_tmp540,_tag_fat(_tmp231,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp235=({struct Cyc_String_pa_PrintArg_struct _tmp406;_tmp406.tag=0U,_tmp406.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp406;});void*_tmp233[1U];_tmp233[0]=& _tmp235;({struct Cyc___cycFILE*_tmp543=Cyc_stderr;struct _fat_ptr _tmp542=({const char*_tmp234=".%s";_tag_fat(_tmp234,sizeof(char),4U);});Cyc_fprintf(_tmp543,_tmp542,_tag_fat(_tmp233,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1124
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp236=p;struct Cyc_Absyn_Datatypefield*_tmp237;int _tmp239;struct _fat_ptr*_tmp238;int _tmp23A;unsigned _tmp23B;struct _fat_ptr _tmp23C;struct Cyc_Absyn_Enumfield*_tmp23D;struct Cyc_Absyn_Enumfield*_tmp23E;struct Cyc_Absyn_Exp*_tmp23F;switch(*((int*)_tmp236)){case 0U: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp236)->f1 == 0){_LL1: _LL2:
({void*_tmp240=0U;({struct Cyc___cycFILE*_tmp545=Cyc_stderr;struct _fat_ptr _tmp544=({const char*_tmp241="where(NULL)";_tag_fat(_tmp241,sizeof(char),12U);});Cyc_fprintf(_tmp545,_tmp544,_tag_fat(_tmp240,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp23F=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp236)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp23F;
({struct Cyc_String_pa_PrintArg_struct _tmp244=({struct Cyc_String_pa_PrintArg_struct _tmp407;_tmp407.tag=0U,({struct _fat_ptr _tmp546=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(e)));_tmp407.f1=_tmp546;});_tmp407;});void*_tmp242[1U];_tmp242[0]=& _tmp244;({struct Cyc___cycFILE*_tmp548=Cyc_stderr;struct _fat_ptr _tmp547=({const char*_tmp243="where(%s)";_tag_fat(_tmp243,sizeof(char),10U);});Cyc_fprintf(_tmp548,_tmp547,_tag_fat(_tmp242,sizeof(void*),1U));});});goto _LL0;}}case 1U: _LL5: _LL6:
({void*_tmp245=0U;({struct Cyc___cycFILE*_tmp54A=Cyc_stderr;struct _fat_ptr _tmp549=({const char*_tmp246="NULL";_tag_fat(_tmp246,sizeof(char),5U);});Cyc_fprintf(_tmp54A,_tmp549,_tag_fat(_tmp245,sizeof(void*),0U));});});goto _LL0;case 2U: _LL7: _LL8:
({void*_tmp247=0U;({struct Cyc___cycFILE*_tmp54C=Cyc_stderr;struct _fat_ptr _tmp54B=({const char*_tmp248="NOT-NULL:";_tag_fat(_tmp248,sizeof(char),10U);});Cyc_fprintf(_tmp54C,_tmp54B,_tag_fat(_tmp247,sizeof(void*),0U));});});goto _LL0;case 4U: _LL9: _tmp23E=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp236)->f2;_LLA: {struct Cyc_Absyn_Enumfield*ef=_tmp23E;
_tmp23D=ef;goto _LLC;}case 3U: _LLB: _tmp23D=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp236)->f2;_LLC: {struct Cyc_Absyn_Enumfield*ef=_tmp23D;
({struct Cyc_String_pa_PrintArg_struct _tmp24B=({struct Cyc_String_pa_PrintArg_struct _tmp408;_tmp408.tag=0U,({struct _fat_ptr _tmp54D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ef->name));_tmp408.f1=_tmp54D;});_tmp408;});void*_tmp249[1U];_tmp249[0]=& _tmp24B;({struct Cyc___cycFILE*_tmp54F=Cyc_stderr;struct _fat_ptr _tmp54E=({const char*_tmp24A="%s";_tag_fat(_tmp24A,sizeof(char),3U);});Cyc_fprintf(_tmp54F,_tmp54E,_tag_fat(_tmp249,sizeof(void*),1U));});});goto _LL0;}case 5U: _LLD: _tmp23C=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp236)->f1;_LLE: {struct _fat_ptr s=_tmp23C;
({struct Cyc_String_pa_PrintArg_struct _tmp24E=({struct Cyc_String_pa_PrintArg_struct _tmp409;_tmp409.tag=0U,_tmp409.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp409;});void*_tmp24C[1U];_tmp24C[0]=& _tmp24E;({struct Cyc___cycFILE*_tmp551=Cyc_stderr;struct _fat_ptr _tmp550=({const char*_tmp24D="%s";_tag_fat(_tmp24D,sizeof(char),3U);});Cyc_fprintf(_tmp551,_tmp550,_tag_fat(_tmp24C,sizeof(void*),1U));});});goto _LL0;}case 6U: _LLF: _tmp23B=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp236)->f1;_LL10: {unsigned i=_tmp23B;
({struct Cyc_Int_pa_PrintArg_struct _tmp251=({struct Cyc_Int_pa_PrintArg_struct _tmp40A;_tmp40A.tag=1U,_tmp40A.f1=(unsigned long)((int)i);_tmp40A;});void*_tmp24F[1U];_tmp24F[0]=& _tmp251;({struct Cyc___cycFILE*_tmp553=Cyc_stderr;struct _fat_ptr _tmp552=({const char*_tmp250="%d";_tag_fat(_tmp250,sizeof(char),3U);});Cyc_fprintf(_tmp553,_tmp552,_tag_fat(_tmp24F,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL11: _tmp23A=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp236)->f1;_LL12: {int i=_tmp23A;
({struct Cyc_Int_pa_PrintArg_struct _tmp254=({struct Cyc_Int_pa_PrintArg_struct _tmp40B;_tmp40B.tag=1U,_tmp40B.f1=(unsigned long)i;_tmp40B;});void*_tmp252[1U];_tmp252[0]=& _tmp254;({struct Cyc___cycFILE*_tmp555=Cyc_stderr;struct _fat_ptr _tmp554=({const char*_tmp253="datatypetag(%d)";_tag_fat(_tmp253,sizeof(char),16U);});Cyc_fprintf(_tmp555,_tmp554,_tag_fat(_tmp252,sizeof(void*),1U));});});goto _LL0;}case 8U: _LL13: _tmp238=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp236)->f1;_tmp239=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp236)->f2;_LL14: {struct _fat_ptr*f=_tmp238;int i=_tmp239;
({struct Cyc_String_pa_PrintArg_struct _tmp257=({struct Cyc_String_pa_PrintArg_struct _tmp40D;_tmp40D.tag=0U,_tmp40D.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp40D;});struct Cyc_Int_pa_PrintArg_struct _tmp258=({struct Cyc_Int_pa_PrintArg_struct _tmp40C;_tmp40C.tag=1U,_tmp40C.f1=(unsigned long)i;_tmp40C;});void*_tmp255[2U];_tmp255[0]=& _tmp257,_tmp255[1]=& _tmp258;({struct Cyc___cycFILE*_tmp557=Cyc_stderr;struct _fat_ptr _tmp556=({const char*_tmp256="uniontag[%s](%d)";_tag_fat(_tmp256,sizeof(char),17U);});Cyc_fprintf(_tmp557,_tmp556,_tag_fat(_tmp255,sizeof(void*),2U));});});goto _LL0;}default: _LL15: _tmp237=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp236)->f2;_LL16: {struct Cyc_Absyn_Datatypefield*f=_tmp237;
# 1137
({struct Cyc_String_pa_PrintArg_struct _tmp25B=({struct Cyc_String_pa_PrintArg_struct _tmp40E;_tmp40E.tag=0U,({struct _fat_ptr _tmp558=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp40E.f1=_tmp558;});_tmp40E;});void*_tmp259[1U];_tmp259[0]=& _tmp25B;({struct Cyc___cycFILE*_tmp55A=Cyc_stderr;struct _fat_ptr _tmp559=({const char*_tmp25A="datatypefield(%s)";_tag_fat(_tmp25A,sizeof(char),18U);});Cyc_fprintf(_tmp55A,_tmp559,_tag_fat(_tmp259,sizeof(void*),1U));});});}}_LL0:;}
# 1141
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp25E=({struct Cyc_String_pa_PrintArg_struct _tmp40F;_tmp40F.tag=0U,({struct _fat_ptr _tmp55B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp40F.f1=_tmp55B;});_tmp40F;});void*_tmp25C[1U];_tmp25C[0]=& _tmp25E;({struct Cyc___cycFILE*_tmp55D=Cyc_stderr;struct _fat_ptr _tmp55C=({const char*_tmp25D="%s";_tag_fat(_tmp25D,sizeof(char),3U);});Cyc_fprintf(_tmp55D,_tmp55C,_tag_fat(_tmp25C,sizeof(void*),1U));});});}
# 1145
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp25F=d;void*_tmp262;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp260;struct Cyc_Tcpat_Rhs*_tmp263;switch(*((int*)_tmp25F)){case 1U: _LL1: _tmp263=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp25F)->f1;_LL2: {struct Cyc_Tcpat_Rhs*rhs=_tmp263;
# 1148
Cyc_Tcpat_print_tab(tab);
({void*_tmp264=0U;({struct Cyc___cycFILE*_tmp55F=Cyc_stderr;struct _fat_ptr _tmp55E=({const char*_tmp265="Success(";_tag_fat(_tmp265,sizeof(char),9U);});Cyc_fprintf(_tmp55F,_tmp55E,_tag_fat(_tmp264,sizeof(void*),0U));});});Cyc_Tcpat_print_rhs(rhs);({void*_tmp266=0U;({struct Cyc___cycFILE*_tmp561=Cyc_stderr;struct _fat_ptr _tmp560=({const char*_tmp267=")\n";_tag_fat(_tmp267,sizeof(char),3U);});Cyc_fprintf(_tmp561,_tmp560,_tag_fat(_tmp266,sizeof(void*),0U));});});
goto _LL0;}case 0U: _LL3: _LL4:
({void*_tmp268=0U;({struct Cyc___cycFILE*_tmp563=Cyc_stderr;struct _fat_ptr _tmp562=({const char*_tmp269="Failure\n";_tag_fat(_tmp269,sizeof(char),9U);});Cyc_fprintf(_tmp563,_tmp562,_tag_fat(_tmp268,sizeof(void*),0U));});});goto _LL0;default: _LL5: _tmp260=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp25F)->f1;_tmp261=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp25F)->f2;_tmp262=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp25F)->f3;_LL6: {struct Cyc_List_List*a=_tmp260;struct Cyc_List_List*cases=_tmp261;void*def=_tmp262;
# 1153
Cyc_Tcpat_print_tab(tab);
({void*_tmp26A=0U;({struct Cyc___cycFILE*_tmp565=Cyc_stderr;struct _fat_ptr _tmp564=({const char*_tmp26B="Switch[";_tag_fat(_tmp26B,sizeof(char),8U);});Cyc_fprintf(_tmp565,_tmp564,_tag_fat(_tmp26A,sizeof(void*),0U));});});
for(0;a != 0;a=a->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)a->hd)->access);
if(a->tl != 0)({void*_tmp26C=0U;({struct Cyc___cycFILE*_tmp567=Cyc_stderr;struct _fat_ptr _tmp566=({const char*_tmp26D=",";_tag_fat(_tmp26D,sizeof(char),2U);});Cyc_fprintf(_tmp567,_tmp566,_tag_fat(_tmp26C,sizeof(void*),0U));});});}
# 1159
({void*_tmp26E=0U;({struct Cyc___cycFILE*_tmp569=Cyc_stderr;struct _fat_ptr _tmp568=({const char*_tmp26F="] {\n";_tag_fat(_tmp26F,sizeof(char),5U);});Cyc_fprintf(_tmp569,_tmp568,_tag_fat(_tmp26E,sizeof(void*),0U));});});
for(0;cases != 0;cases=cases->tl){
struct _tuple0 _tmp270=*((struct _tuple0*)cases->hd);struct _tuple0 _stmttmp24=_tmp270;struct _tuple0 _tmp271=_stmttmp24;void*_tmp273;void*_tmp272;_LL8: _tmp272=_tmp271.f1;_tmp273=_tmp271.f2;_LL9: {void*pt=_tmp272;void*d=_tmp273;
Cyc_Tcpat_print_tab(tab);
({void*_tmp274=0U;({struct Cyc___cycFILE*_tmp56B=Cyc_stderr;struct _fat_ptr _tmp56A=({const char*_tmp275="case ";_tag_fat(_tmp275,sizeof(char),6U);});Cyc_fprintf(_tmp56B,_tmp56A,_tag_fat(_tmp274,sizeof(void*),0U));});});
Cyc_Tcpat_print_pat_test(pt);
({void*_tmp276=0U;({struct Cyc___cycFILE*_tmp56D=Cyc_stderr;struct _fat_ptr _tmp56C=({const char*_tmp277=":\n";_tag_fat(_tmp277,sizeof(char),3U);});Cyc_fprintf(_tmp56D,_tmp56C,_tag_fat(_tmp276,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(d,tab + 7);}}
# 1168
Cyc_Tcpat_print_tab(tab);
({void*_tmp278=0U;({struct Cyc___cycFILE*_tmp56F=Cyc_stderr;struct _fat_ptr _tmp56E=({const char*_tmp279="default:\n";_tag_fat(_tmp279,sizeof(char),10U);});Cyc_fprintf(_tmp56F,_tmp56E,_tag_fat(_tmp278,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(def,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp27A=0U;({struct Cyc___cycFILE*_tmp571=Cyc_stderr;struct _fat_ptr _tmp570=({const char*_tmp27B="}\n";_tag_fat(_tmp27B,sizeof(char),3U);});Cyc_fprintf(_tmp571,_tmp570,_tag_fat(_tmp27A,sizeof(void*),0U));});});}}_LL0:;}
# 1176
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1184
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp27C=td;struct Cyc_Set_Set*_tmp27D;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp27C)->tag == 1U){_LL1: _tmp27D=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp27C)->f1;_LL2: {struct Cyc_Set_Set*cs=_tmp27D;
# 1194
return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->tag=1U,({struct Cyc_Set_Set*_tmp572=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(cs,c);_tmp27E->f1=_tmp572;});_tmp27E;});}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp280=({struct Cyc_Warn_String_Warn_Warg_struct _tmp410;_tmp410.tag=0U,({struct _fat_ptr _tmp573=({const char*_tmp281="add_neg called when td is Positive";_tag_fat(_tmp281,sizeof(char),35U);});_tmp410.f1=_tmp573;});_tmp410;});void*_tmp27F[1U];_tmp27F[0]=& _tmp280;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp27F,sizeof(void*),1U));});}_LL0:;}
# 1201
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp282=td;struct Cyc_Set_Set*_tmp283;struct Cyc_Tcpat_Con_s*_tmp284;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp282)->tag == 0U){_LL1: _tmp284=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp282)->f1;_LL2: {struct Cyc_Tcpat_Con_s*c2=_tmp284;
# 1205
return Cyc_Tcpat_compare_con(c,c2)== 0?Cyc_Tcpat_Yes: Cyc_Tcpat_No;}}else{_LL3: _tmp283=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp282)->f1;_LL4: {struct Cyc_Set_Set*cs=_tmp283;
# 1208
if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(cs,c))return Cyc_Tcpat_No;
# 1211
if(c->span != 0 &&({int _tmp574=*((int*)_check_null(c->span));_tmp574 == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(cs)+ 1;}))
return Cyc_Tcpat_Yes;
# 1214
return Cyc_Tcpat_Maybe;}}_LL0:;}struct _tuple22{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1222
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp285=ctxt;struct Cyc_List_List*_tmp288;struct Cyc_List_List*_tmp287;struct Cyc_Tcpat_Con_s*_tmp286;if(_tmp285 == 0){_LL1: _LL2:
 return 0;}else{_LL3: _tmp286=((struct _tuple22*)_tmp285->hd)->f1;_tmp287=((struct _tuple22*)_tmp285->hd)->f2;_tmp288=_tmp285->tl;_LL4: {struct Cyc_Tcpat_Con_s*c=_tmp286;struct Cyc_List_List*args=_tmp287;struct Cyc_List_List*tl=_tmp288;
return({struct Cyc_List_List*_tmp28B=_cycalloc(sizeof(*_tmp28B));({struct _tuple22*_tmp576=({struct _tuple22*_tmp28A=_cycalloc(sizeof(*_tmp28A));_tmp28A->f1=c,({struct Cyc_List_List*_tmp575=({struct Cyc_List_List*_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289->hd=dsc,_tmp289->tl=args;_tmp289;});_tmp28A->f2=_tmp575;});_tmp28A;});_tmp28B->hd=_tmp576;}),_tmp28B->tl=tl;_tmp28B;});}}_LL0:;}
# 1232
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp28C=ctxt;struct Cyc_List_List*_tmp28F;struct Cyc_List_List*_tmp28E;struct Cyc_Tcpat_Con_s*_tmp28D;if(_tmp28C == 0){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp291=({struct Cyc_Warn_String_Warn_Warg_struct _tmp411;_tmp411.tag=0U,({struct _fat_ptr _tmp577=({const char*_tmp292="norm_context: empty context";_tag_fat(_tmp292,sizeof(char),28U);});_tmp411.f1=_tmp577;});_tmp411;});void*_tmp290[1U];_tmp290[0]=& _tmp291;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp290,sizeof(void*),1U));});}else{_LL3: _tmp28D=((struct _tuple22*)_tmp28C->hd)->f1;_tmp28E=((struct _tuple22*)_tmp28C->hd)->f2;_tmp28F=_tmp28C->tl;_LL4: {struct Cyc_Tcpat_Con_s*c=_tmp28D;struct Cyc_List_List*args=_tmp28E;struct Cyc_List_List*tl=_tmp28F;
return({struct Cyc_List_List*_tmp579=tl;Cyc_Tcpat_augment(_tmp579,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293->tag=0U,_tmp293->f1=c,({struct Cyc_List_List*_tmp578=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(args);_tmp293->f2=_tmp578;});_tmp293;}));});}}_LL0:;}
# 1244
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1246
struct _tuple1 _tmp294=({struct _tuple1 _tmp413;_tmp413.f1=ctxt,_tmp413.f2=work;_tmp413;});struct _tuple1 _stmttmp25=_tmp294;struct _tuple1 _tmp295=_stmttmp25;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp298;struct Cyc_List_List*_tmp297;struct Cyc_Tcpat_Con_s*_tmp296;if(_tmp295.f1 == 0){if(_tmp295.f2 == 0){_LL1: _LL2:
 return dsc;}else{_LL3: _LL4:
 goto _LL6;}}else{if(_tmp295.f2 == 0){_LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp412;_tmp412.tag=0U,({struct _fat_ptr _tmp57A=({const char*_tmp29D="build_desc: ctxt and work don't match";_tag_fat(_tmp29D,sizeof(char),38U);});_tmp412.f1=_tmp57A;});_tmp412;});void*_tmp29B[1U];_tmp29B[0]=& _tmp29C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp29B,sizeof(void*),1U));});}else{_LL7: _tmp296=((struct _tuple22*)(_tmp295.f1)->hd)->f1;_tmp297=((struct _tuple22*)(_tmp295.f1)->hd)->f2;_tmp298=(_tmp295.f1)->tl;_tmp299=((struct _tuple21*)(_tmp295.f2)->hd)->f3;_tmp29A=(_tmp295.f2)->tl;_LL8: {struct Cyc_Tcpat_Con_s*c=_tmp296;struct Cyc_List_List*args=_tmp297;struct Cyc_List_List*rest=_tmp298;struct Cyc_List_List*dargs=_tmp299;struct Cyc_List_List*work2=_tmp29A;
# 1251
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp29E=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->tag=0U,_tmp2A0->f1=c,({struct Cyc_List_List*_tmp57C=({struct Cyc_List_List*_tmp57B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(args);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp57B,({struct Cyc_List_List*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->hd=dsc,_tmp29F->tl=dargs;_tmp29F;}));});_tmp2A0->f2=_tmp57C;});_tmp2A0;});struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*td=_tmp29E;
return Cyc_Tcpat_build_desc(rest,(void*)td,work2);}}}_LL0:;}
# 1256
static void*Cyc_Tcpat_match(void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Tcpat_Rhs*,struct Cyc_List_List*);struct _tuple23{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1261
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp2A1=allmrules;struct Cyc_List_List*_tmp2A4;struct Cyc_Tcpat_Rhs*_tmp2A3;void*_tmp2A2;if(_tmp2A1 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5->tag=0U,_tmp2A5->f1=dsc;_tmp2A5;});}else{_LL3: _tmp2A2=((struct _tuple23*)_tmp2A1->hd)->f1;_tmp2A3=((struct _tuple23*)_tmp2A1->hd)->f2;_tmp2A4=_tmp2A1->tl;_LL4: {void*pat1=_tmp2A2;struct Cyc_Tcpat_Rhs*rhs1=_tmp2A3;struct Cyc_List_List*rulerest=_tmp2A4;
# 1265
return Cyc_Tcpat_match(pat1,0,dsc,0,0,rhs1,rulerest);}}_LL0:;}
# 1270
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp57E=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6->tag=1U,({struct Cyc_Set_Set*_tmp57D=Cyc_Tcpat_empty_con_set();_tmp2A6->f1=_tmp57D;});_tmp2A6;});Cyc_Tcpat_or_match(_tmp57E,allmrules);});}
# 1277
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1280
struct Cyc_List_List*_tmp2A7=work;struct Cyc_List_List*_tmp2AB;struct Cyc_List_List*_tmp2AA;struct Cyc_List_List*_tmp2A9;struct Cyc_List_List*_tmp2A8;struct Cyc_List_List*_tmp2AC;if(_tmp2A7 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->tag=1U,_tmp2AD->f1=right_hand_side;_tmp2AD;});}else{if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2A7)->hd)->f1 == 0){if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2A7)->hd)->f2 == 0){if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2A7)->hd)->f3 == 0){_LL3: _tmp2AC=_tmp2A7->tl;_LL4: {struct Cyc_List_List*workr=_tmp2AC;
# 1283
return({struct Cyc_List_List*_tmp581=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp580=workr;struct Cyc_Tcpat_Rhs*_tmp57F=right_hand_side;Cyc_Tcpat_and_match(_tmp581,_tmp580,_tmp57F,rules);});}}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _tmp2A8=((struct _tuple21*)_tmp2A7->hd)->f1;_tmp2A9=((struct _tuple21*)_tmp2A7->hd)->f2;_tmp2AA=((struct _tuple21*)_tmp2A7->hd)->f3;_tmp2AB=_tmp2A7->tl;_LL6: {struct Cyc_List_List*pats=_tmp2A8;struct Cyc_List_List*objs=_tmp2A9;struct Cyc_List_List*dscs=_tmp2AA;struct Cyc_List_List*workr=_tmp2AB;
# 1285
if((pats == 0 || objs == 0)|| dscs == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp414;_tmp414.tag=0U,({struct _fat_ptr _tmp582=({const char*_tmp2B0="tcpat:and_match: malformed work frame";_tag_fat(_tmp2B0,sizeof(char),38U);});_tmp414.f1=_tmp582;});_tmp414;});void*_tmp2AE[1U];_tmp2AE[0]=& _tmp2AF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2AE,sizeof(void*),1U));});{
struct Cyc_List_List*_tmp2B1=pats;struct Cyc_List_List*_stmttmp26=_tmp2B1;struct Cyc_List_List*_tmp2B2=_stmttmp26;struct Cyc_List_List*_tmp2B4;void*_tmp2B3;_LL8: _tmp2B3=(void*)_tmp2B2->hd;_tmp2B4=_tmp2B2->tl;_LL9: {void*pat1=_tmp2B3;struct Cyc_List_List*patr=_tmp2B4;
struct Cyc_List_List*_tmp2B5=objs;struct Cyc_List_List*_stmttmp27=_tmp2B5;struct Cyc_List_List*_tmp2B6=_stmttmp27;struct Cyc_List_List*_tmp2B8;struct Cyc_List_List*_tmp2B7;_LLB: _tmp2B7=(struct Cyc_List_List*)_tmp2B6->hd;_tmp2B8=_tmp2B6->tl;_LLC: {struct Cyc_List_List*obj1=_tmp2B7;struct Cyc_List_List*objr=_tmp2B8;
struct Cyc_List_List*_tmp2B9=dscs;struct Cyc_List_List*_stmttmp28=_tmp2B9;struct Cyc_List_List*_tmp2BA=_stmttmp28;struct Cyc_List_List*_tmp2BC;void*_tmp2BB;_LLE: _tmp2BB=(void*)_tmp2BA->hd;_tmp2BC=_tmp2BA->tl;_LLF: {void*dsc1=_tmp2BB;struct Cyc_List_List*dscr=_tmp2BC;
struct _tuple21*_tmp2BD=({struct _tuple21*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->f1=patr,_tmp2BF->f2=objr,_tmp2BF->f3=dscr;_tmp2BF;});struct _tuple21*wf=_tmp2BD;
return({void*_tmp588=pat1;struct Cyc_List_List*_tmp587=obj1;void*_tmp586=dsc1;struct Cyc_List_List*_tmp585=ctx;struct Cyc_List_List*_tmp584=({struct Cyc_List_List*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->hd=wf,_tmp2BE->tl=workr;_tmp2BE;});struct Cyc_Tcpat_Rhs*_tmp583=right_hand_side;Cyc_Tcpat_match(_tmp588,_tmp587,_tmp586,_tmp585,_tmp584,_tmp583,rules);});}}}}}}}_LL0:;}
# 1296
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp2C0=dsc;struct Cyc_List_List*_tmp2C1;struct Cyc_Set_Set*_tmp2C2;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2C0)->tag == 1U){_LL1: _tmp2C2=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2C0)->f1;_LL2: {struct Cyc_Set_Set*ncs=_tmp2C2;
# 1302
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5->tag=1U,({struct Cyc_Set_Set*_tmp589=Cyc_Tcpat_empty_con_set();_tmp2C5->f1=_tmp589;});_tmp2C5;});
struct Cyc_List_List*_tmp2C3=0;struct Cyc_List_List*res=_tmp2C3;
{int i=0;for(0;i < pcon->arity;++ i){
res=({struct Cyc_List_List*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4->hd=any,_tmp2C4->tl=res;_tmp2C4;});}}
return res;}}else{_LL3: _tmp2C1=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2C0)->f2;_LL4: {struct Cyc_List_List*dargs=_tmp2C1;
return dargs;}}_LL0:;}
# 1311
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp2C6=pw;struct Cyc_Absyn_Pat*_tmp2C7;if((_tmp2C6.where_clause).tag == 2){_LL1: _LL2:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL3: _tmp2C7=(_tmp2C6.pattern).val;_LL4: {struct Cyc_Absyn_Pat*p=_tmp2C7;
# 1315
void*_tmp2C8=p->r;void*_stmttmp29=_tmp2C8;void*_tmp2C9=_stmttmp29;struct Cyc_List_List*_tmp2CB;union Cyc_Absyn_AggrInfo _tmp2CA;struct Cyc_Absyn_Datatypefield*_tmp2CD;struct Cyc_Absyn_Datatypedecl*_tmp2CC;switch(*((int*)_tmp2C9)){case 6U: _LL6: _LL7:
# 1317
 if(i != 0)
({struct Cyc_Int_pa_PrintArg_struct _tmp2D0=({struct Cyc_Int_pa_PrintArg_struct _tmp415;_tmp415.tag=1U,_tmp415.f1=(unsigned long)i;_tmp415;});void*_tmp2CE[1U];_tmp2CE[0]=& _tmp2D0;({struct _fat_ptr _tmp58A=({const char*_tmp2CF="get_access on pointer pattern with offset %d\n";_tag_fat(_tmp2CF,sizeof(char),46U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp58A,_tag_fat(_tmp2CE,sizeof(void*),1U));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5U: _LL8: _LL9:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->tag=2U,_tmp2D1->f1=(unsigned)i;_tmp2D1;});case 8U: _LLA: _tmp2CC=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2C9)->f1;_tmp2CD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2C9)->f2;_LLB: {struct Cyc_Absyn_Datatypedecl*tud=_tmp2CC;struct Cyc_Absyn_Datatypefield*tuf=_tmp2CD;
return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->tag=3U,_tmp2D2->f1=tud,_tmp2D2->f2=tuf,_tmp2D2->f3=(unsigned)i;_tmp2D2;});}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2C9)->f1 != 0){_LLC: _tmp2CA=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2C9)->f1;_tmp2CB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2C9)->f3;_LLD: {union Cyc_Absyn_AggrInfo info=_tmp2CA;struct Cyc_List_List*dlps=_tmp2CB;
# 1323
struct Cyc_Absyn_Aggrdecl*_tmp2D3=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2D3;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
struct Cyc_List_List*_tmp2D4=(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(dlps))->hd)).f1;struct Cyc_List_List*_stmttmp2A=_tmp2D4;struct Cyc_List_List*_tmp2D5=_stmttmp2A;struct _fat_ptr*_tmp2D6;if(_tmp2D5 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp2D5)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp2D5)->tl == 0){_LL11: _tmp2D6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D5->hd)->f1;_LL12: {struct _fat_ptr*f=_tmp2D6;
# 1327
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->tag=4U,_tmp2D7->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmp2D7->f2=f;_tmp2D7;});}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _LL14:
# 1329
({struct Cyc_String_pa_PrintArg_struct _tmp2DA=({struct Cyc_String_pa_PrintArg_struct _tmp416;_tmp416.tag=0U,({struct _fat_ptr _tmp58B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(p));_tmp416.f1=_tmp58B;});_tmp416;});void*_tmp2D8[1U];_tmp2D8[0]=& _tmp2DA;({struct _fat_ptr _tmp58C=({const char*_tmp2D9="get_access on bad aggr pattern: %s";_tag_fat(_tmp2D9,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp58C,_tag_fat(_tmp2D8,sizeof(void*),1U));});});}_LL10:;}{
# 1331
struct Cyc_List_List*_tmp2DB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp2DB;
int _tmp2DC=i;int orig_i=_tmp2DC;
for(0;i != 0;-- i){fields=((struct Cyc_List_List*)_check_null(fields))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->tag=4U,_tmp2DD->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmp2DD->f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fields))->hd)->name;_tmp2DD;});}}}else{goto _LLE;}default: _LLE: _LLF:
({struct Cyc_String_pa_PrintArg_struct _tmp2E0=({struct Cyc_String_pa_PrintArg_struct _tmp417;_tmp417.tag=0U,({struct _fat_ptr _tmp58D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(p));_tmp417.f1=_tmp58D;});_tmp417;});void*_tmp2DE[1U];_tmp2DE[0]=& _tmp2E0;({struct _fat_ptr _tmp58E=({const char*_tmp2DF="get_access on bad pattern: %s";_tag_fat(_tmp2DF,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp58E,_tag_fat(_tmp2DE,sizeof(void*),1U));});});}_LL5:;}}_LL0:;}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1341
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple24*env,int i){
struct _tuple24*_tmp2E1=env;struct Cyc_Tcpat_Con_s*_tmp2E3;struct Cyc_List_List*_tmp2E2;_LL1: _tmp2E2=_tmp2E1->f1;_tmp2E3=_tmp2E1->f2;_LL2: {struct Cyc_List_List*obj=_tmp2E2;struct Cyc_Tcpat_Con_s*pcon=_tmp2E3;
void*acc=Cyc_Tcpat_get_access(pcon->orig_pat,i);
return({struct Cyc_List_List*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));({struct Cyc_Tcpat_PathNode*_tmp58F=({struct Cyc_Tcpat_PathNode*_tmp2E4=_cycalloc(sizeof(*_tmp2E4));_tmp2E4->orig_pat=pcon->orig_pat,_tmp2E4->access=acc;_tmp2E4;});_tmp2E5->hd=_tmp58F;}),_tmp2E5->tl=obj;_tmp2E5;});}}
# 1346
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple24 _tmp2E6=({struct _tuple24 _tmp418;_tmp418.f1=obj,_tmp418.f2=pcon;_tmp418;});struct _tuple24 env=_tmp2E6;
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple24*,int),struct _tuple24*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& env);}
# 1354
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1357
void*_tmp2E7=p;struct Cyc_List_List*_tmp2E9;struct Cyc_Tcpat_Con_s*_tmp2E8;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2E7)->tag == 0U){_LL1: _LL2:
 return({struct Cyc_List_List*_tmp592=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp591=work;struct Cyc_Tcpat_Rhs*_tmp590=right_hand_side;Cyc_Tcpat_and_match(_tmp592,_tmp591,_tmp590,rules);});}else{_LL3: _tmp2E8=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2E7)->f1;_tmp2E9=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2E7)->f2;_LL4: {struct Cyc_Tcpat_Con_s*pcon=_tmp2E8;struct Cyc_List_List*pargs=_tmp2E9;
# 1360
enum Cyc_Tcpat_Answer _tmp2EA=Cyc_Tcpat_static_match(pcon,dsc);enum Cyc_Tcpat_Answer _stmttmp2B=_tmp2EA;enum Cyc_Tcpat_Answer _tmp2EB=_stmttmp2B;switch(_tmp2EB){case Cyc_Tcpat_Yes: _LL6: _LL7: {
# 1362
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));({struct _tuple22*_tmp593=({struct _tuple22*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE->f1=pcon,_tmp2EE->f2=0;_tmp2EE;});_tmp2EF->hd=_tmp593;}),_tmp2EF->tl=ctx;_tmp2EF;});
struct _tuple21*work_frame=({struct _tuple21*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->f1=pargs,({struct Cyc_List_List*_tmp595=Cyc_Tcpat_getoargs(pcon,obj);_tmp2ED->f2=_tmp595;}),({
struct Cyc_List_List*_tmp594=Cyc_Tcpat_getdargs(pcon,dsc);_tmp2ED->f3=_tmp594;});_tmp2ED;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC->hd=work_frame,_tmp2EC->tl=work;_tmp2EC;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL8: _LL9:
# 1368
 return({void*_tmp596=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp596,rules);});default: _LLA: _LLB: {
# 1370
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));({struct _tuple22*_tmp597=({struct _tuple22*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->f1=pcon,_tmp2F4->f2=0;_tmp2F4;});_tmp2F5->hd=_tmp597;}),_tmp2F5->tl=ctx;_tmp2F5;});
struct _tuple21*work_frame=({struct _tuple21*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3->f1=pargs,({struct Cyc_List_List*_tmp599=Cyc_Tcpat_getoargs(pcon,obj);_tmp2F3->f2=_tmp599;}),({
struct Cyc_List_List*_tmp598=Cyc_Tcpat_getdargs(pcon,dsc);_tmp2F3->f3=_tmp598;});_tmp2F3;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2->hd=work_frame,_tmp2F2->tl=work;_tmp2F2;});
void*_tmp2F0=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);void*s=_tmp2F0;
void*_tmp2F1=({void*_tmp59C=({struct Cyc_List_List*_tmp59B=ctx;void*_tmp59A=Cyc_Tcpat_add_neg(dsc,pcon);Cyc_Tcpat_build_desc(_tmp59B,_tmp59A,work);});Cyc_Tcpat_or_match(_tmp59C,rules);});void*f=_tmp2F1;
return Cyc_Tcpat_ifeq(obj,pcon,s,f);}}_LL5:;}}_LL0:;}
# 1386
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1391
void*_tmp2F6=d;void*_tmp2F8;struct Cyc_List_List*_tmp2F7;struct Cyc_Tcpat_Rhs*_tmp2F9;void*_tmp2FA;switch(*((int*)_tmp2F6)){case 0U: _LL1: _tmp2FA=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2F6)->f1;_LL2: {void*td=_tmp2FA;
# 1393
if(!(*exhaust_done)){not_exhaust(env1,td);*exhaust_done=1;}
goto _LL0;}case 1U: _LL3: _tmp2F9=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2F6)->f1;_LL4: {struct Cyc_Tcpat_Rhs*r=_tmp2F9;
rhs_appears(env2,r);goto _LL0;}default: _LL5: _tmp2F7=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2F6)->f2;_tmp2F8=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2F6)->f3;_LL6: {struct Cyc_List_List*cases=_tmp2F7;void*def=_tmp2F8;
# 1397
for(0;cases != 0;cases=cases->tl){
struct _tuple0 _tmp2FB=*((struct _tuple0*)cases->hd);struct _tuple0 _stmttmp2C=_tmp2FB;struct _tuple0 _tmp2FC=_stmttmp2C;void*_tmp2FD;_LL8: _tmp2FD=_tmp2FC.f2;_LL9: {void*d=_tmp2FD;
Cyc_Tcpat_check_exhaust_overlap(d,not_exhaust,env1,rhs_appears,env2,exhaust_done);}}
# 1402
Cyc_Tcpat_check_exhaust_overlap(def,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1404
goto _LL0;}}_LL0:;}
# 1412
static struct _tuple23*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->used=0,_tmp303->pat_loc=(swc->pattern)->loc,_tmp303->rhs=swc->body;_tmp303;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp2FE=({union Cyc_Tcpat_PatOrWhere _tmp419;(_tmp419.where_clause).tag=2U,(_tmp419.where_clause).val=swc->where_clause;_tmp419;});union Cyc_Tcpat_PatOrWhere w=_tmp2FE;
sp=({struct Cyc_List_List*_tmp59E=({void*_tmp2FF[2U];_tmp2FF[0]=sp0,({void*_tmp59D=Cyc_Tcpat_int_pat(*ctr,w);_tmp2FF[1]=_tmp59D;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2FF,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp59E,w);});
*ctr=*ctr + 1;}else{
# 1421
sp=({struct Cyc_List_List*_tmp5A0=({void*_tmp300[2U];_tmp300[0]=sp0,({void*_tmp59F=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->tag=0U;_tmp301;});_tmp300[1]=_tmp59F;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp300,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp5A0,({union Cyc_Tcpat_PatOrWhere _tmp41A;(_tmp41A.where_clause).tag=2U,(_tmp41A.where_clause).val=0;_tmp41A;}));});}
return({struct _tuple23*_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302->f1=sp,_tmp302->f2=rhs;_tmp302;});}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1427
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1429
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp304=d;struct Cyc_Set_Set*_tmp305;struct Cyc_List_List*_tmp307;struct Cyc_Tcpat_Con_s*_tmp306;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp304)->tag == 0U){_LL1: _tmp306=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp304)->f1;_tmp307=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp304)->f2;_LL2: {struct Cyc_Tcpat_Con_s*c=_tmp306;struct Cyc_List_List*ds=_tmp307;
# 1433
union Cyc_Tcpat_Name_value _tmp308=c->name;union Cyc_Tcpat_Name_value n=_tmp308;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp309=c->orig_pat;union Cyc_Tcpat_PatOrWhere _stmttmp2D=_tmp309;union Cyc_Tcpat_PatOrWhere _tmp30A=_stmttmp2D;struct Cyc_Absyn_Pat*_tmp30B;if((_tmp30A.where_clause).tag == 2){_LL6: _LL7:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(ds))->hd);}else{_LL8: _tmp30B=(_tmp30A.pattern).val;_LL9: {struct Cyc_Absyn_Pat*p2=_tmp30B;
p=p2;goto _LL5;}}_LL5:;}{
# 1439
void*_tmp30C=p->r;void*_stmttmp2E=_tmp30C;void*_tmp30D=_stmttmp2E;struct Cyc_Absyn_Exp*_tmp30E;struct Cyc_Absyn_Enumfield*_tmp30F;struct Cyc_Absyn_Enumfield*_tmp310;int _tmp312;struct _fat_ptr _tmp311;char _tmp313;int _tmp314;struct Cyc_Absyn_Datatypefield*_tmp315;struct Cyc_List_List*_tmp317;struct Cyc_Absyn_Aggrdecl*_tmp316;struct Cyc_Absyn_Vardecl*_tmp319;struct Cyc_Absyn_Tvar*_tmp318;struct Cyc_Absyn_Vardecl*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp31B;switch(*((int*)_tmp30D)){case 0U: _LLB: _LLC:
 return({const char*_tmp31C="_";_tag_fat(_tmp31C,sizeof(char),2U);});case 1U: _LLD: _tmp31B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp30D)->f1;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp31B;
return Cyc_Absynpp_qvar2string(vd->name);}case 3U: _LLF: _tmp31A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp30D)->f1;_LL10: {struct Cyc_Absyn_Vardecl*vd=_tmp31A;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp41B;_tmp41B.tag=0U,({struct _fat_ptr _tmp5A1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp41B.f1=_tmp5A1;});_tmp41B;});void*_tmp31D[1U];_tmp31D[0]=& _tmp31F;({struct _fat_ptr _tmp5A2=({const char*_tmp31E="*%s";_tag_fat(_tmp31E,sizeof(char),4U);});Cyc_aprintf(_tmp5A2,_tag_fat(_tmp31D,sizeof(void*),1U));});});}case 4U: _LL11: _tmp318=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp30D)->f1;_tmp319=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp30D)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp318;struct Cyc_Absyn_Vardecl*vd=_tmp319;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp322=({struct Cyc_String_pa_PrintArg_struct _tmp41D;_tmp41D.tag=0U,({struct _fat_ptr _tmp5A3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp41D.f1=_tmp5A3;});_tmp41D;});struct Cyc_String_pa_PrintArg_struct _tmp323=({struct Cyc_String_pa_PrintArg_struct _tmp41C;_tmp41C.tag=0U,_tmp41C.f1=(struct _fat_ptr)((struct _fat_ptr)*tv->name);_tmp41C;});void*_tmp320[2U];_tmp320[0]=& _tmp322,_tmp320[1]=& _tmp323;({struct _fat_ptr _tmp5A4=({const char*_tmp321="%s<`%s>";_tag_fat(_tmp321,sizeof(char),8U);});Cyc_aprintf(_tmp5A4,_tag_fat(_tmp320,sizeof(void*),2U));});});}case 5U: _LL13: _LL14:
# 1445
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp326=({struct Cyc_String_pa_PrintArg_struct _tmp41E;_tmp41E.tag=0U,({struct _fat_ptr _tmp5A5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp41E.f1=_tmp5A5;});_tmp41E;});void*_tmp324[1U];_tmp324[0]=& _tmp326;({struct _fat_ptr _tmp5A6=({const char*_tmp325="$(%s)";_tag_fat(_tmp325,sizeof(char),6U);});Cyc_aprintf(_tmp5A6,_tag_fat(_tmp324,sizeof(void*),1U));});});case 6U: _LL15: _LL16:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp329=({struct Cyc_String_pa_PrintArg_struct _tmp41F;_tmp41F.tag=0U,({struct _fat_ptr _tmp5A7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp41F.f1=_tmp5A7;});_tmp41F;});void*_tmp327[1U];_tmp327[0]=& _tmp329;({struct _fat_ptr _tmp5A8=({const char*_tmp328="&%s";_tag_fat(_tmp328,sizeof(char),4U);});Cyc_aprintf(_tmp5A8,_tag_fat(_tmp327,sizeof(void*),1U));});});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp30D)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp30D)->f1)->KnownAggr).tag == 2){_LL17: _tmp316=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp30D)->f1)->KnownAggr).val;_tmp317=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp30D)->f3;_LL18: {struct Cyc_Absyn_Aggrdecl*ad=_tmp316;struct Cyc_List_List*dlps=_tmp317;
# 1448
if((int)ad->kind == (int)Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp32A=dlps;struct _fat_ptr*_tmp32B;if(_tmp32A != 0){if(((struct _tuple16*)((struct Cyc_List_List*)_tmp32A)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)_tmp32A)->hd)->f1)->hd)->tag == 1U){_LL2C: _tmp32B=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple16*)_tmp32A->hd)->f1)->hd)->f1;_LL2D: {struct _fat_ptr*f=_tmp32B;
# 1451
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32E=({struct Cyc_String_pa_PrintArg_struct _tmp422;_tmp422.tag=0U,({struct _fat_ptr _tmp5A9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp422.f1=_tmp5A9;});_tmp422;});struct Cyc_String_pa_PrintArg_struct _tmp32F=({struct Cyc_String_pa_PrintArg_struct _tmp421;_tmp421.tag=0U,_tmp421.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp421;});struct Cyc_String_pa_PrintArg_struct _tmp330=({struct Cyc_String_pa_PrintArg_struct _tmp420;_tmp420.tag=0U,({
struct _fat_ptr _tmp5AA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp420.f1=_tmp5AA;});_tmp420;});void*_tmp32C[3U];_tmp32C[0]=& _tmp32E,_tmp32C[1]=& _tmp32F,_tmp32C[2]=& _tmp330;({struct _fat_ptr _tmp5AB=({const char*_tmp32D="%s{.%s = %s}";_tag_fat(_tmp32D,sizeof(char),13U);});Cyc_aprintf(_tmp5AB,_tag_fat(_tmp32C,sizeof(void*),3U));});});}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 1455
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp333=({struct Cyc_String_pa_PrintArg_struct _tmp424;_tmp424.tag=0U,({struct _fat_ptr _tmp5AC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp424.f1=_tmp5AC;});_tmp424;});struct Cyc_String_pa_PrintArg_struct _tmp334=({struct Cyc_String_pa_PrintArg_struct _tmp423;_tmp423.tag=0U,({struct _fat_ptr _tmp5AD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp423.f1=_tmp5AD;});_tmp423;});void*_tmp331[2U];_tmp331[0]=& _tmp333,_tmp331[1]=& _tmp334;({struct _fat_ptr _tmp5AE=({const char*_tmp332="%s{%s}";_tag_fat(_tmp332,sizeof(char),7U);});Cyc_aprintf(_tmp5AE,_tag_fat(_tmp331,sizeof(void*),2U));});});}}else{goto _LL29;}}else{goto _LL29;}case 8U: _LL19: _tmp315=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp30D)->f2;_LL1A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp315;
# 1457
if(ds == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp337=({struct Cyc_String_pa_PrintArg_struct _tmp425;_tmp425.tag=0U,({struct _fat_ptr _tmp5AF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp425.f1=_tmp5AF;});_tmp425;});void*_tmp335[1U];_tmp335[0]=& _tmp337;({struct _fat_ptr _tmp5B0=({const char*_tmp336="%s";_tag_fat(_tmp336,sizeof(char),3U);});Cyc_aprintf(_tmp5B0,_tag_fat(_tmp335,sizeof(void*),1U));});});else{
# 1460
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33A=({struct Cyc_String_pa_PrintArg_struct _tmp427;_tmp427.tag=0U,({struct _fat_ptr _tmp5B1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp427.f1=_tmp5B1;});_tmp427;});struct Cyc_String_pa_PrintArg_struct _tmp33B=({struct Cyc_String_pa_PrintArg_struct _tmp426;_tmp426.tag=0U,({struct _fat_ptr _tmp5B2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp426.f1=_tmp5B2;});_tmp426;});void*_tmp338[2U];_tmp338[0]=& _tmp33A,_tmp338[1]=& _tmp33B;({struct _fat_ptr _tmp5B3=({const char*_tmp339="%s(%s)";_tag_fat(_tmp339,sizeof(char),7U);});Cyc_aprintf(_tmp5B3,_tag_fat(_tmp338,sizeof(void*),2U));});});}}case 9U: _LL1B: _LL1C:
 return({const char*_tmp33C="NULL";_tag_fat(_tmp33C,sizeof(char),5U);});case 10U: _LL1D: _tmp314=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp30D)->f2;_LL1E: {int i=_tmp314;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp33F=({struct Cyc_Int_pa_PrintArg_struct _tmp428;_tmp428.tag=1U,_tmp428.f1=(unsigned long)i;_tmp428;});void*_tmp33D[1U];_tmp33D[0]=& _tmp33F;({struct _fat_ptr _tmp5B4=({const char*_tmp33E="%d";_tag_fat(_tmp33E,sizeof(char),3U);});Cyc_aprintf(_tmp5B4,_tag_fat(_tmp33D,sizeof(void*),1U));});});}case 11U: _LL1F: _tmp313=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp30D)->f1;_LL20: {char c=_tmp313;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp342=({struct Cyc_Int_pa_PrintArg_struct _tmp429;_tmp429.tag=1U,_tmp429.f1=(unsigned long)((int)c);_tmp429;});void*_tmp340[1U];_tmp340[0]=& _tmp342;({struct _fat_ptr _tmp5B5=({const char*_tmp341="%d";_tag_fat(_tmp341,sizeof(char),3U);});Cyc_aprintf(_tmp5B5,_tag_fat(_tmp340,sizeof(void*),1U));});});}case 12U: _LL21: _tmp311=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp30D)->f1;_tmp312=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp30D)->f2;_LL22: {struct _fat_ptr f=_tmp311;int i=_tmp312;
return f;}case 13U: _LL23: _tmp310=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp30D)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp310;
_tmp30F=ef;goto _LL26;}case 14U: _LL25: _tmp30F=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp30D)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp30F;
return Cyc_Absynpp_qvar2string(ef->name);}case 17U: _LL27: _tmp30E=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp30D)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp30E;
return Cyc_Absynpp_exp2string(e);}default: _LL29: _LL2A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LLA:;}}}else{_LL3: _tmp305=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp304)->f1;_LL4: {struct Cyc_Set_Set*s=_tmp305;
# 1471
if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(s))return({const char*_tmp343="_";_tag_fat(_tmp343,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*_tmp344=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(s);struct Cyc_Tcpat_Con_s*c=_tmp344;
# 1475
if((int)(((c->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp345=({union Cyc_Tcpat_PatOrWhere _tmp362=c->orig_pat;if((_tmp362.pattern).tag != 1)_throw_match();(_tmp362.pattern).val;});struct Cyc_Absyn_Pat*orig_pat=_tmp345;
void*_tmp346=Cyc_Absyn_compress((void*)_check_null(orig_pat->topt));void*_stmttmp2F=_tmp346;void*_tmp347=_stmttmp2F;void*_tmp348;struct Cyc_Absyn_Aggrdecl*_tmp349;struct Cyc_Absyn_Datatypedecl*_tmp34A;switch(*((int*)_tmp347)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->f2 == Cyc_Absyn_Char_sz){_LL31: _LL32:
# 1480
{int i=0;for(0;i < 256;++ i){
if(!({struct Cyc_Set_Set*_tmp5B6=s;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5B6,Cyc_Tcpat_char_con((char)i,orig_pat));}))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp34D=({struct Cyc_Int_pa_PrintArg_struct _tmp42A;_tmp42A.tag=1U,_tmp42A.f1=(unsigned long)i;_tmp42A;});void*_tmp34B[1U];_tmp34B[0]=& _tmp34D;({struct _fat_ptr _tmp5B7=({const char*_tmp34C="%d";_tag_fat(_tmp34C,sizeof(char),3U);});Cyc_aprintf(_tmp5B7,_tag_fat(_tmp34B,sizeof(void*),1U));});});}}
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL33: _LL34:
# 1486
{unsigned i=0U;for(0;i < (unsigned)-1;++ i){
if(!({struct Cyc_Set_Set*_tmp5B8=s;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5B8,Cyc_Tcpat_int_con((int)i,c->orig_pat));}))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp350=({struct Cyc_Int_pa_PrintArg_struct _tmp42B;_tmp42B.tag=1U,_tmp42B.f1=(unsigned long)((int)i);_tmp42B;});void*_tmp34E[1U];_tmp34E[0]=& _tmp350;({struct _fat_ptr _tmp5B9=({const char*_tmp34F="%d";_tag_fat(_tmp34F,sizeof(char),3U);});Cyc_aprintf(_tmp5B9,_tag_fat(_tmp34E,sizeof(void*),1U));});});}}
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->f1).KnownDatatype).tag == 2){_LL37: _tmp34A=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->f1).KnownDatatype).val;_LL38: {struct Cyc_Absyn_Datatypedecl*tud=_tmp34A;
# 1495
if(tud->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp353=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp353;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
for(0;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)fields->hd)->name).f2;
struct Cyc_List_List*_tmp354=((struct Cyc_Absyn_Datatypefield*)fields->hd)->typs;struct Cyc_List_List*args=_tmp354;
if(!({struct Cyc_Set_Set*_tmp5BB=s;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5BB,({struct Cyc_Tcpat_Con_s*_tmp355=_cycalloc(sizeof(*_tmp355));({union Cyc_Tcpat_Name_value _tmp5BA=Cyc_Tcpat_Name_v(n);_tmp355->name=_tmp5BA;}),_tmp355->arity=0,_tmp355->span=0,_tmp355->orig_pat=c->orig_pat;_tmp355;}));}))
return((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args)== 0?n:(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp358=({struct Cyc_String_pa_PrintArg_struct _tmp42C;_tmp42C.tag=0U,_tmp42C.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp42C;});void*_tmp356[1U];_tmp356[0]=& _tmp358;({struct _fat_ptr _tmp5BC=({const char*_tmp357="%s(...)";_tag_fat(_tmp357,sizeof(char),8U);});Cyc_aprintf(_tmp5BC,_tag_fat(_tmp356,sizeof(void*),1U));});});}
# 1504
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL3B;}case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp349=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->f1).KnownAggr).val;if((int)_tmp349->kind == (int)Cyc_Absyn_UnionA){_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp349;
# 1506
struct Cyc_List_List*_tmp359=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp359;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
struct _tuple2*_tmp35A=ad->name;struct _tuple2*_stmttmp30=_tmp35A;struct _tuple2*_tmp35B=_stmttmp30;struct _fat_ptr _tmp35C;_LL3E: _tmp35C=*_tmp35B->f2;_LL3F: {struct _fat_ptr union_name=_tmp35C;
for(0;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;
if(!({struct Cyc_Set_Set*_tmp5BE=s;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5BE,({struct Cyc_Tcpat_Con_s*_tmp35D=_cycalloc(sizeof(*_tmp35D));({union Cyc_Tcpat_Name_value _tmp5BD=Cyc_Tcpat_Name_v(n);_tmp35D->name=_tmp5BD;}),_tmp35D->arity=0,_tmp35D->span=0,_tmp35D->orig_pat=c->orig_pat;_tmp35D;}));}))
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp360=({struct Cyc_String_pa_PrintArg_struct _tmp42E;_tmp42E.tag=0U,_tmp42E.f1=(struct _fat_ptr)((struct _fat_ptr)union_name);_tmp42E;});struct Cyc_String_pa_PrintArg_struct _tmp361=({struct Cyc_String_pa_PrintArg_struct _tmp42D;_tmp42D.tag=0U,_tmp42D.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp42D;});void*_tmp35E[2U];_tmp35E[0]=& _tmp360,_tmp35E[1]=& _tmp361;({struct _fat_ptr _tmp5BF=({const char*_tmp35F="%s{.%s = _}";_tag_fat(_tmp35F,sizeof(char),12U);});Cyc_aprintf(_tmp5BF,_tag_fat(_tmp35E,sizeof(void*),2U));});});}
# 1514
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL3B;}}else{goto _LL3B;}default: goto _LL3B;}case 3U: _LL35: _tmp348=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp347)->f1).ptr_atts).nullable;_LL36: {void*n=_tmp348;
# 1491
if(Cyc_Tcutil_force_type2bool(0,n)&& !({struct Cyc_Set_Set*_tmp5C0=s;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5C0,Cyc_Tcpat_null_con(orig_pat));}))
return({const char*_tmp351="NULL";_tag_fat(_tmp351,sizeof(char),5U);});
return({const char*_tmp352="&_";_tag_fat(_tmp352,sizeof(char),3U);});}default: _LL3B: _LL3C:
# 1515
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL30:;}}}}_LL0:;}
# 1519
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _fat_ptr*_tmp363=_cycalloc(sizeof(*_tmp363));({struct _fat_ptr _tmp5C1=Cyc_Tcpat_desc2string(d);*_tmp363=_tmp5C1;});_tmp363;});}
# 1522
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp364=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);struct Cyc_List_List*ss=_tmp364;
struct _fat_ptr*comma=({struct _fat_ptr*_tmp368=_cycalloc(sizeof(*_tmp368));({struct _fat_ptr _tmp5C2=({const char*_tmp367=",";_tag_fat(_tmp367,sizeof(char),2U);});*_tmp368=_tmp5C2;});_tmp368;});
{struct Cyc_List_List*_tmp365=ss;struct Cyc_List_List*x=_tmp365;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
if(x->tl != 0){
({struct Cyc_List_List*_tmp5C3=({struct Cyc_List_List*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->hd=comma,_tmp366->tl=x->tl;_tmp366;});x->tl=_tmp5C3;});
x=x->tl;}}}
# 1530
return(struct _fat_ptr)Cyc_strconcat_l(ss);}
# 1533
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){
struct _handler_cons _tmp369;_push_handler(& _tmp369);{int _tmp36B=0;if(setjmp(_tmp369.handler))_tmp36B=1;if(!_tmp36B){
{struct _fat_ptr _tmp36C=Cyc_Tcpat_desc2string(desc);struct _fat_ptr s=_tmp36C;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp430;_tmp430.tag=0U,({struct _fat_ptr _tmp5C4=({const char*_tmp370="patterns may not be exhaustive.\n\tmissing case for ";_tag_fat(_tmp370,sizeof(char),51U);});_tmp430.f1=_tmp5C4;});_tmp430;});struct Cyc_Warn_String_Warn_Warg_struct _tmp36F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp42F;_tmp42F.tag=0U,_tmp42F.f1=s;_tmp42F;});void*_tmp36D[2U];_tmp36D[0]=& _tmp36E,_tmp36D[1]=& _tmp36F;({unsigned _tmp5C5=loc;Cyc_Warn_err2(_tmp5C5,_tag_fat(_tmp36D,sizeof(void*),2U));});});}
# 1535
;_pop_handler();}else{void*_tmp36A=(void*)Cyc_Core_get_exn_thrown();void*_tmp371=_tmp36A;void*_tmp372;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp371)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1538
({struct Cyc_Warn_String_Warn_Warg_struct _tmp374=({struct Cyc_Warn_String_Warn_Warg_struct _tmp431;_tmp431.tag=0U,({struct _fat_ptr _tmp5C6=({const char*_tmp375="patterns may not be exhaustive.";_tag_fat(_tmp375,sizeof(char),32U);});_tmp431.f1=_tmp5C6;});_tmp431;});void*_tmp373[1U];_tmp373[0]=& _tmp374;({unsigned _tmp5C7=loc;Cyc_Warn_err2(_tmp5C7,_tag_fat(_tmp373,sizeof(void*),1U));});});
goto _LL0;}else{_LL3: _tmp372=_tmp371;_LL4: {void*exn=_tmp372;(int)_rethrow(exn);}}_LL0:;}}}
# 1542
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1546
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1552
int _tmp376=0;int where_ctr=_tmp376;
int*_tmp377=& where_ctr;int*env=_tmp377;
struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct _tuple23*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,env,swcs);struct Cyc_List_List*match_rules=_tmp378;
void*_tmp379=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp379;
*dopt=dec_tree;{
# 1558
int _tmp37A=0;int ex_done=_tmp37A;
((void(*)(void*d,void(*not_exhaust)(unsigned,void*),unsigned env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1561
for(0;match_rules != 0;match_rules=match_rules->tl){
struct _tuple23*_tmp37B=(struct _tuple23*)match_rules->hd;struct _tuple23*_stmttmp31=_tmp37B;struct _tuple23*_tmp37C=_stmttmp31;unsigned _tmp37E;int _tmp37D;_LL1: _tmp37D=(_tmp37C->f2)->used;_tmp37E=(_tmp37C->f2)->pat_loc;_LL2: {int b=_tmp37D;unsigned loc2=_tmp37E;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp380=({struct Cyc_Warn_String_Warn_Warg_struct _tmp432;_tmp432.tag=0U,({struct _fat_ptr _tmp5C8=({const char*_tmp381="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp381,sizeof(char),74U);});_tmp432.f1=_tmp5C8;});_tmp432;});void*_tmp37F[1U];_tmp37F[0]=& _tmp380;({unsigned _tmp5C9=loc2;Cyc_Warn_err2(_tmp5C9,_tag_fat(_tmp37F,sizeof(void*),1U));});});
# 1566
break;}}}}}
# 1575
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple15*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp382;_push_handler(& _tmp382);{int _tmp384=0;if(setjmp(_tmp382.handler))_tmp384=1;if(!_tmp384){
{struct _fat_ptr _tmp385=Cyc_Tcpat_desc2string(desc);struct _fat_ptr s=_tmp385;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp387=({struct Cyc_Warn_String_Warn_Warg_struct _tmp434;_tmp434.tag=0U,({struct _fat_ptr _tmp5CA=({const char*_tmp389="pattern not exhaustive.\n\tmissing case for ";_tag_fat(_tmp389,sizeof(char),43U);});_tmp434.f1=_tmp5CA;});_tmp434;});struct Cyc_Warn_String_Warn_Warg_struct _tmp388=({struct Cyc_Warn_String_Warn_Warg_struct _tmp433;_tmp433.tag=0U,_tmp433.f1=s;_tmp433;});void*_tmp386[2U];_tmp386[0]=& _tmp387,_tmp386[1]=& _tmp388;({unsigned _tmp5CB=(*pr).f1;Cyc_Warn_warn2(_tmp5CB,_tag_fat(_tmp386,sizeof(void*),2U));});});}
# 1578
;_pop_handler();}else{void*_tmp383=(void*)Cyc_Core_get_exn_thrown();void*_tmp38A=_tmp383;void*_tmp38B;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp38A)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1581
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp435;_tmp435.tag=0U,({struct _fat_ptr _tmp5CC=({const char*_tmp38E="pattern not exhaustive.";_tag_fat(_tmp38E,sizeof(char),24U);});_tmp435.f1=_tmp5CC;});_tmp435;});void*_tmp38C[1U];_tmp38C[0]=& _tmp38D;({unsigned _tmp5CD=(*pr).f1;Cyc_Warn_warn2(_tmp5CD,_tag_fat(_tmp38C,sizeof(void*),1U));});});
goto _LL0;}else{_LL3: _tmp38B=_tmp38A;_LL4: {void*exn=_tmp38B;(int)_rethrow(exn);}}_LL0:;}}}}
# 1585
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1589
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->used=0,_tmp395->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_tmp5CE=Cyc_Absyn_skip_stmt(0U);_tmp395->rhs=_tmp5CE;});_tmp395;});
struct Cyc_List_List*_tmp38F=({struct Cyc_List_List*_tmp394=_cycalloc(sizeof(*_tmp394));({struct _tuple23*_tmp5D0=({struct _tuple23*_tmp393=_cycalloc(sizeof(*_tmp393));({void*_tmp5CF=Cyc_Tcpat_compile_pat(p);_tmp393->f1=_tmp5CF;}),_tmp393->f2=rhs;_tmp393;});_tmp394->hd=_tmp5D0;}),_tmp394->tl=0;_tmp394;});struct Cyc_List_List*match_rules=_tmp38F;
void*_tmp390=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp390;
struct _tuple15 _tmp391=({struct _tuple15 _tmp436;_tmp436.f1=loc,_tmp436.f2=1;_tmp436;});struct _tuple15 exhaust_env=_tmp391;
int _tmp392=0;int ex_done=_tmp392;
((void(*)(void*d,void(*not_exhaust)(struct _tuple15*,void*),struct _tuple15*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_warn,& exhaust_env,Cyc_Tcpat_dummy_fn,0,& ex_done);
# 1597
*dopt=dec_tree;
return exhaust_env.f2;}
# 1605
static struct _tuple23*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1609
if(swc->where_clause != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp397=({struct Cyc_Warn_String_Warn_Warg_struct _tmp437;_tmp437.tag=0U,({struct _fat_ptr _tmp5D1=({const char*_tmp398="&&-clauses not supported in exception handlers.";_tag_fat(_tmp398,sizeof(char),48U);});_tmp437.f1=_tmp5D1;});_tmp437;});void*_tmp396[1U];_tmp396[0]=& _tmp397;({unsigned _tmp5D2=((struct Cyc_Absyn_Exp*)_check_null(swc->where_clause))->loc;Cyc_Warn_err2(_tmp5D2,_tag_fat(_tmp396,sizeof(void*),1U));});});{
# 1612
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->used=0,_tmp39A->pat_loc=(swc->pattern)->loc,_tmp39A->rhs=swc->body;_tmp39A;});
return({struct _tuple23*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->f1=sp0,_tmp399->f2=rhs;_tmp399;});}}
# 1615
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1618
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1622
struct Cyc_List_List*_tmp39B=((struct Cyc_List_List*(*)(struct _tuple23*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);struct Cyc_List_List*match_rules=_tmp39B;
void*_tmp39C=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp39C;
*dopt=dec_tree;{
int _tmp39D=0;int ex_done=_tmp39D;
((void(*)(void*d,void(*not_exhaust)(unsigned,void*),unsigned env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1628
for(0;match_rules != 0;match_rules=match_rules->tl){
# 1630
if(match_rules->tl == 0)break;{
struct _tuple23*_tmp39E=(struct _tuple23*)match_rules->hd;struct _tuple23*_stmttmp32=_tmp39E;struct _tuple23*_tmp39F=_stmttmp32;unsigned _tmp3A1;int _tmp3A0;_LL1: _tmp3A0=(_tmp39F->f2)->used;_tmp3A1=(_tmp39F->f2)->pat_loc;_LL2: {int b=_tmp3A0;unsigned loc2=_tmp3A1;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp438;_tmp438.tag=0U,({struct _fat_ptr _tmp5D3=({const char*_tmp3A4="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp3A4,sizeof(char),74U);});_tmp438.f1=_tmp5D3;});_tmp438;});void*_tmp3A2[1U];_tmp3A2[0]=& _tmp3A3;({unsigned _tmp5D4=loc2;Cyc_Warn_err2(_tmp5D4,_tag_fat(_tmp3A2,sizeof(void*),1U));});});
break;}}}}}}
# 1639
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp3A5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;struct Cyc_List_List*l=_tmp3A5;for(0;l != 0;l=l->tl){
if((*((struct _tuple17*)l->hd)).f1 != 0)
return 1;}}
return 0;}

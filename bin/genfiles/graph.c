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
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100 "cycboot.h"
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 75
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 82
extern struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 85
extern struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*,void*);
# 97
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);
# 233
extern struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict);
# 270
extern struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,void*);
# 50 "graph.h"
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*)(void*,void*));
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict,void*);
# 56
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict,void*,void*);
# 59
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*);
# 69
int Cyc_Graph_is_edge(struct Cyc_Dict_Dict,void*,void*);
# 72
struct Cyc_Set_Set*Cyc_Graph_get_targets(struct Cyc_Dict_Dict,void*);
# 27 "graph.cyc"
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*cmp)(void*,void*)){
return((struct Cyc_Dict_Dict(*)(int(*)(void*,void*)))Cyc_Dict_empty)(cmp);}
# 34
struct Cyc_Set_Set*Cyc_Graph_get_targets(struct Cyc_Dict_Dict g,void*source){
struct Cyc_Set_Set**_tmp0=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_stmttmp0=_tmp0;struct Cyc_Set_Set**_tmp1=_stmttmp0;void*_tmp2;if(_tmp1 == 0)
return Cyc_Set_empty(g.rel);else{_tmp2=*_tmp1;{struct Cyc_Set_Set*targets=_tmp2;
return targets;}};}
# 45
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict g,void*source){
if(((int(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_member)(g,source))return g;else{
return({struct Cyc_Dict_Dict _tmp97=g;void*_tmp96=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert)(_tmp97,_tmp96,Cyc_Set_empty(g.rel));});}}
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp3=Cyc_Graph_get_targets(g,source);struct Cyc_Set_Set*sourceTargets=_tmp3;
return({struct Cyc_Dict_Dict _tmp99=g;void*_tmp98=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert)(_tmp99,_tmp98,Cyc_Set_insert(sourceTargets,target));});}
# 61
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*_tmp4=Cyc_Graph_get_targets(g,source);struct Cyc_Set_Set*sourceTargets=_tmp4;
return({struct Cyc_Dict_Dict _tmp9B=g;void*_tmp9A=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert)(_tmp9B,_tmp9A,Cyc_Set_union_two(sourceTargets,targets));});}
# 69
struct Cyc_Dict_Dict Cyc_Graph_remove_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp5=Cyc_Graph_get_targets(g,source);struct Cyc_Set_Set*sourceTargets=_tmp5;
return({struct Cyc_Dict_Dict _tmp9D=g;void*_tmp9C=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert)(_tmp9D,_tmp9C,Cyc_Set_delete(sourceTargets,target));});}
# 77
struct Cyc_Dict_Dict Cyc_Graph_remove_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*_tmp6=Cyc_Graph_get_targets(g,source);struct Cyc_Set_Set*sourceTargets=_tmp6;
return({struct Cyc_Dict_Dict _tmp9F=g;void*_tmp9E=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert)(_tmp9F,_tmp9E,Cyc_Set_diff(sourceTargets,targets));});}
# 86
static struct Cyc_List_List*Cyc_Graph_to_list(struct Cyc_Set_Set*ts){
# 88
struct Cyc_List_List*_tmp7=0;struct Cyc_List_List*result=_tmp7;
while(!Cyc_Set_is_empty(ts)){
void*_tmp8=Cyc_Set_choose(ts);void*z=_tmp8;
ts=Cyc_Set_delete(ts,z);
result=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->hd=z,_tmp9->tl=result;_tmp9;});}
# 94
return result;}
# 100
int Cyc_Graph_is_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set**_tmpA=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_stmttmp1=_tmpA;struct Cyc_Set_Set**_tmpB=_stmttmp1;void*_tmpC;if(_tmpB == 0)
# 103
return 0;else{_tmpC=*_tmpB;{struct Cyc_Set_Set*sourceTargets=_tmpC;
# 105
return Cyc_Set_member(sourceTargets,target);}};}struct _tuple0{void*f1;struct Cyc_Set_Set*f2;};
# 112
void Cyc_Graph_print(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,void*)){
({void*_tmpD=0U;({struct Cyc___cycFILE*_tmpA1=f;struct _fat_ptr _tmpA0=({const char*_tmpE="digraph {\n";_tag_fat(_tmpE,sizeof(char),11U);});Cyc_fprintf(_tmpA1,_tmpA0,_tag_fat(_tmpD,sizeof(void*),0));});});{
struct Cyc_List_List*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict))Cyc_Dict_to_list)(g);struct Cyc_List_List*edges=_tmpF;
for(1;(unsigned)edges;edges=edges->tl){
struct _tuple0*_tmp10=(struct _tuple0*)edges->hd;struct _tuple0*_stmttmp2=_tmp10;struct _tuple0*_tmp11=_stmttmp2;void*_tmp13;void*_tmp12;_tmp12=_tmp11->f1;_tmp13=_tmp11->f2;{void*s=_tmp12;struct Cyc_Set_Set*ts=_tmp13;
struct Cyc_List_List*_tmp14=Cyc_Graph_to_list(ts);struct Cyc_List_List*tslist=_tmp14;
if((unsigned)tslist)
for(1;(unsigned)tslist;tslist=tslist->tl){
nodeprint(f,s);
({void*_tmp15=0U;({struct Cyc___cycFILE*_tmpA3=f;struct _fat_ptr _tmpA2=({const char*_tmp16=" -> ";_tag_fat(_tmp16,sizeof(char),5U);});Cyc_fprintf(_tmpA3,_tmpA2,_tag_fat(_tmp15,sizeof(void*),0));});});
nodeprint(f,tslist->hd);
({void*_tmp17=0U;({struct Cyc___cycFILE*_tmpA5=f;struct _fat_ptr _tmpA4=({const char*_tmp18=";\n";_tag_fat(_tmp18,sizeof(char),3U);});Cyc_fprintf(_tmpA5,_tmpA4,_tag_fat(_tmp17,sizeof(void*),0));});});}else{
# 126
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmpA7=f;struct _fat_ptr _tmpA6=({const char*_tmp1A="node ";_tag_fat(_tmp1A,sizeof(char),6U);});Cyc_fprintf(_tmpA7,_tmpA6,_tag_fat(_tmp19,sizeof(void*),0));});});
nodeprint(f,s);
({void*_tmp1B=0U;({struct Cyc___cycFILE*_tmpA9=f;struct _fat_ptr _tmpA8=({const char*_tmp1C="; // no targets\n";_tag_fat(_tmp1C,sizeof(char),17U);});Cyc_fprintf(_tmpA9,_tmpA8,_tag_fat(_tmp1B,sizeof(void*),0));});});}}}
# 131
({void*_tmp1D=0U;({struct Cyc___cycFILE*_tmpAB=f;struct _fat_ptr _tmpAA=({const char*_tmp1E="}\n";_tag_fat(_tmp1E,sizeof(char),3U);});Cyc_fprintf(_tmpAB,_tmpAA,_tag_fat(_tmp1D,sizeof(void*),0));});});}}
# 141 "graph.cyc"
static struct Cyc_List_List*Cyc_Graph_sourcesOf(struct Cyc_Dict_Dict g,void*node){
struct Cyc_List_List*_tmp1F=0;struct Cyc_List_List*result=_tmp1F;
# 144
{struct Cyc_List_List*_tmp20=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict))Cyc_Dict_to_list)(g);struct Cyc_List_List*edges=_tmp20;for(0;(unsigned)edges;edges=edges->tl){
struct _tuple0*_tmp21=(struct _tuple0*)edges->hd;struct _tuple0*_stmttmp3=_tmp21;struct _tuple0*_tmp22=_stmttmp3;void*_tmp24;void*_tmp23;_tmp23=_tmp22->f1;_tmp24=_tmp22->f2;{void*s=_tmp23;struct Cyc_Set_Set*ts=_tmp24;
if(Cyc_Set_member(ts,node))result=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->hd=s,_tmp25->tl=result;_tmp25;});}}}
# 148
return result;}struct _tuple1{struct Cyc_Set_Set*f1;struct Cyc_Dict_Dict f2;};
# 157
static struct _tuple1 Cyc_Graph_divideGraph(struct Cyc_Dict_Dict g,void*source){
# 160
struct Cyc_Set_Set**_tmp26=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_stmttmp4=_tmp26;struct Cyc_Set_Set**_tmp27=_stmttmp4;void*_tmp28;if(_tmp27 == 0)
return({struct _tuple1 _tmp94;({struct Cyc_Set_Set*_tmpAC=Cyc_Set_empty(g.rel);_tmp94.f1=_tmpAC;}),_tmp94.f2=g;_tmp94;});else{_tmp28=*_tmp27;{struct Cyc_Set_Set*sourceTargets=_tmp28;
return({struct _tuple1 _tmp95;_tmp95.f1=sourceTargets,({struct Cyc_Dict_Dict _tmpAD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_delete)(g,source);_tmp95.f2=_tmpAD;});_tmp95;});}};}
# 169
static struct Cyc_Dict_Dict Cyc_Graph_add_edgeTc(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp29=Cyc_Graph_get_targets(g,target);struct Cyc_Set_Set*targetTargets=_tmp29;
struct _tuple1 _tmp2A=Cyc_Graph_divideGraph(g,source);struct _tuple1 _stmttmp5=_tmp2A;struct _tuple1 _tmp2B=_stmttmp5;struct Cyc_Dict_Dict _tmp2D;void*_tmp2C;_tmp2C=_tmp2B.f1;_tmp2D=_tmp2B.f2;{struct Cyc_Set_Set*sourceTargets=_tmp2C;struct Cyc_Dict_Dict graphWithoutSource=_tmp2D;
struct Cyc_List_List*_tmp2E=Cyc_Graph_sourcesOf(graphWithoutSource,source);struct Cyc_List_List*sourceSources=_tmp2E;
struct Cyc_Set_Set*_tmp2F=({struct Cyc_Set_Set*_tmpAE=Cyc_Set_union_two(sourceTargets,targetTargets);Cyc_Set_insert(_tmpAE,target);});struct Cyc_Set_Set*newSourceTargets=_tmp2F;
struct Cyc_Dict_Dict _tmp30=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert)(graphWithoutSource,source,newSourceTargets);struct Cyc_Dict_Dict result=_tmp30;
{struct Cyc_List_List*_tmp31=sourceSources;struct Cyc_List_List*s=_tmp31;for(0;(unsigned)s;s=s->tl){
result=Cyc_Graph_add_edges(result,s->hd,newSourceTargets);}}
# 178
return result;}}
# 180
struct Cyc_Dict_Dict Cyc_Graph_tc_slow(struct Cyc_Dict_Dict g){
struct Cyc_Dict_Dict _tmp32=Cyc_Graph_empty(g.rel);struct Cyc_Dict_Dict result=_tmp32;
struct Cyc_List_List*_tmp33=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict))Cyc_Dict_to_list)(g);struct Cyc_List_List*edges=_tmp33;
for(1;(unsigned)edges;edges=edges->tl){
struct _tuple0*_tmp34=(struct _tuple0*)edges->hd;struct _tuple0*_stmttmp6=_tmp34;struct _tuple0*_tmp35=_stmttmp6;void*_tmp37;void*_tmp36;_tmp36=_tmp35->f1;_tmp37=_tmp35->f2;{void*source=_tmp36;struct Cyc_Set_Set*targets=_tmp37;
struct Cyc_List_List*_tmp38=Cyc_Graph_to_list(targets);struct Cyc_List_List*tslist=_tmp38;
if((unsigned)tslist){
struct Cyc_List_List*_tmp39=tslist;struct Cyc_List_List*t=_tmp39;for(0;(unsigned)t;t=t->tl){
result=Cyc_Graph_add_edgeTc(result,source,t->hd);}}else{
result=Cyc_Graph_add_node(result,source);}}}
# 191
return result;}
# 198
struct Cyc_Dict_Dict Cyc_Graph_tkernel(struct Cyc_Dict_Dict g){
int(*_tmp3A)(void*,void*)=g.rel;int(*cmp)(void*,void*)=_tmp3A;
struct Cyc_Dict_Dict _tmp3B=Cyc_Graph_empty(cmp);struct Cyc_Dict_Dict closure=_tmp3B;
struct Cyc_Dict_Dict _tmp3C=Cyc_Graph_empty(cmp);struct Cyc_Dict_Dict result=_tmp3C;
{struct Cyc_List_List*_tmp3D=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict))Cyc_Dict_to_list)(g);struct Cyc_List_List*edges=_tmp3D;for(0;(unsigned)edges;edges=edges->tl){
struct _tuple0*_tmp3E=(struct _tuple0*)edges->hd;struct _tuple0*_stmttmp7=_tmp3E;struct _tuple0*_tmp3F=_stmttmp7;void*_tmp41;void*_tmp40;_tmp40=_tmp3F->f1;_tmp41=_tmp3F->f2;{void*source=_tmp40;struct Cyc_Set_Set*targets=_tmp41;
while(!Cyc_Set_is_empty(targets)){
void*_tmp42=Cyc_Set_choose(targets);void*target=_tmp42;
targets=Cyc_Set_delete(targets,target);
if(Cyc_Graph_is_edge(closure,source,target))continue;
closure=Cyc_Graph_add_edgeTc(closure,source,target);
result=Cyc_Graph_add_edge(result,source,target);}}}}
# 212
return result;}struct Cyc_Graph_nodestate{void*root;int C;int visitindex;};struct Cyc_Graph_componentstate{struct Cyc_Set_Set*Succ;struct Cyc_Set_Set*nodes;};struct Cyc_Graph_tcstate{struct Cyc_Set_Set*visited;int visitindex;struct Cyc_Dict_Dict ns;struct Cyc_Dict_Dict cs;int Cindex;struct Cyc_List_List*nstack;struct Cyc_List_List*cstack;};
# 238
static void Cyc_Graph_comp_tc(struct Cyc_Graph_tcstate*ts,struct Cyc_Dict_Dict g,void*v){
# 240
int(*_tmp43)(void*,void*)=g.rel;int(*cmp)(void*,void*)=_tmp43;
({struct Cyc_Set_Set*_tmpAF=Cyc_Set_insert(ts->visited,v);ts->visited=_tmpAF;});{
struct Cyc_Graph_nodestate*_tmp44=({struct Cyc_Graph_nodestate*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->root=v,_tmp58->C=0,_tmp58->visitindex=ts->visitindex ++;_tmp58;});struct Cyc_Graph_nodestate*nsv=_tmp44;
({struct Cyc_Dict_Dict _tmpB0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Graph_nodestate*))Cyc_Dict_insert)(ts->ns,v,nsv);ts->ns=_tmpB0;});
({struct Cyc_List_List*_tmpB1=({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->hd=v,_tmp45->tl=ts->nstack;_tmp45;});ts->nstack=_tmpB1;});{
int _tmp46=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts->cstack);int hsaved=_tmp46;
struct Cyc_Set_Set*_tmp47=Cyc_Graph_get_targets(g,v);struct Cyc_Set_Set*targets=_tmp47;
while(!Cyc_Set_is_empty(targets)){
void*_tmp48=Cyc_Set_choose(targets);void*w=_tmp48;
targets=Cyc_Set_delete(targets,w);
if(cmp(v,w)== 0)continue;{
int is_forward_edge;
struct Cyc_Graph_nodestate*nsw;
if(Cyc_Set_member(ts->visited,w)){
nsw=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(ts->ns,w);
is_forward_edge=nsw->visitindex < nsv->visitindex?0: 1;}else{
# 258
is_forward_edge=0;
Cyc_Graph_comp_tc(ts,g,w);
nsw=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(ts->ns,w);}{
# 262
int _tmp49=nsw->C;int Cw=_tmp49;
if(Cw == 0){
struct Cyc_Graph_nodestate*_tmp4A=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(ts->ns,nsv->root);struct Cyc_Graph_nodestate*nsrootv=_tmp4A;
struct Cyc_Graph_nodestate*_tmp4B=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(ts->ns,nsw->root);struct Cyc_Graph_nodestate*nsrootw=_tmp4B;
# 267
if(nsrootv->visitindex > nsrootw->visitindex)
nsv->root=nsw->root;}else{
# 271
if(!is_forward_edge)
({struct Cyc_List_List*_tmpB2=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=(void*)Cw,_tmp4C->tl=ts->cstack;_tmp4C;});ts->cstack=_tmpB2;});}}}}
# 274
if(cmp(nsv->root,v)!= 0)
return;{
# 277
int _tmp4D=ts->Cindex ++;int Cnew=_tmp4D;
struct Cyc_Graph_componentstate*_tmp4E=({struct Cyc_Graph_componentstate*_tmp57=_cycalloc(sizeof(*_tmp57));({struct Cyc_Set_Set*_tmpB4=((struct Cyc_Set_Set*(*)(int(*)(int,int)))Cyc_Set_empty)(Cyc_Core_intcmp);_tmp57->Succ=_tmpB4;}),({
struct Cyc_Set_Set*_tmpB3=Cyc_Set_empty(cmp);_tmp57->nodes=_tmpB3;});_tmp57;});
# 278
struct Cyc_Graph_componentstate*csCnew=_tmp4E;
# 280
({struct Cyc_Dict_Dict _tmpB5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,int,struct Cyc_Graph_componentstate*))Cyc_Dict_insert)(ts->cs,Cnew,csCnew);ts->cs=_tmpB5;});
{struct Cyc_List_List*_tmp4F=ts->nstack;struct Cyc_List_List*_stmttmp8=_tmp4F;struct Cyc_List_List*_tmp50=_stmttmp8;void*_tmp51;if(_tmp50 == 0)
goto _LL0;else{_tmp51=(void*)_tmp50->hd;{void*top=_tmp51;
# 284
if(cmp(top,v)!= 0 ||
 Cyc_Graph_is_edge(g,v,v))
({struct Cyc_Set_Set*_tmpB6=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,int))Cyc_Set_insert)(csCnew->Succ,Cnew);csCnew->Succ=_tmpB6;});
# 288
goto _LL0;}}_LL0:;}
# 290
{int _tmp52=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts->cstack);int hnow=_tmp52;for(0;hnow > hsaved;-- hnow){
int _tmp53=(int)((struct Cyc_List_List*)_check_null(ts->cstack))->hd;int X=_tmp53;
ts->cstack=((struct Cyc_List_List*)_check_null(ts->cstack))->tl;
if(!((int(*)(struct Cyc_Set_Set*,int))Cyc_Set_member)(csCnew->Succ,X)){
struct Cyc_Set_Set*_tmp54=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,int))Cyc_Set_insert)(csCnew->Succ,X);struct Cyc_Set_Set*s=_tmp54;
s=({struct Cyc_Set_Set*_tmpB7=s;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_union_two)(_tmpB7,(((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict,int))Cyc_Dict_lookup)(ts->cs,X))->Succ);});
csCnew->Succ=s;}}}
# 299
while((unsigned)ts->nstack){
void*_tmp55=((struct Cyc_List_List*)_check_null(ts->nstack))->hd;void*w=_tmp55;
ts->nstack=((struct Cyc_List_List*)_check_null(ts->nstack))->tl;{
struct Cyc_Graph_nodestate*_tmp56=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(ts->ns,w);struct Cyc_Graph_nodestate*nsw=_tmp56;
nsw->C=Cnew;
({struct Cyc_Set_Set*_tmpB8=Cyc_Set_insert(csCnew->nodes,w);csCnew->nodes=_tmpB8;});
if(cmp(w,v)== 0)break;}}}}}}
# 308
struct Cyc_Dict_Dict Cyc_Graph_tc(struct Cyc_Dict_Dict g){
int(*_tmp59)(void*,void*)=g.rel;int(*cmp)(void*,void*)=_tmp59;
struct Cyc_Graph_tcstate*_tmp5A=({struct Cyc_Graph_tcstate*_tmp68=_cycalloc(sizeof(*_tmp68));
({
struct Cyc_Set_Set*_tmpBB=Cyc_Set_empty(cmp);_tmp68->visited=_tmpBB;}),_tmp68->visitindex=1,({
# 314
struct Cyc_Dict_Dict _tmpBA=((struct Cyc_Dict_Dict(*)(int(*)(void*,void*)))Cyc_Dict_empty)(cmp);_tmp68->ns=_tmpBA;}),({
struct Cyc_Dict_Dict _tmpB9=((struct Cyc_Dict_Dict(*)(int(*)(int,int)))Cyc_Dict_empty)(Cyc_Core_intcmp);_tmp68->cs=_tmpB9;}),_tmp68->Cindex=1,_tmp68->nstack=0,_tmp68->cstack=0;_tmp68;});
# 310
struct Cyc_Graph_tcstate*ts=_tmp5A;
# 320
{struct Cyc_List_List*_tmp5B=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict))Cyc_Dict_to_list)(g);struct Cyc_List_List*edges=_tmp5B;for(0;(unsigned)edges;edges=edges->tl){
struct _tuple0*_tmp5C=(struct _tuple0*)edges->hd;struct _tuple0*_stmttmp9=_tmp5C;struct _tuple0*_tmp5D=_stmttmp9;void*_tmp5F;void*_tmp5E;_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;{void*source=_tmp5E;struct Cyc_Set_Set*targets=_tmp5F;
if(!Cyc_Set_member(ts->visited,source))Cyc_Graph_comp_tc(ts,g,source);}}}{
# 325
struct Cyc_Dict_Dict _tmp60=Cyc_Graph_empty(cmp);struct Cyc_Dict_Dict result=_tmp60;
{int _tmp61=1;int C=_tmp61;for(0;C < ts->Cindex;++ C){
struct Cyc_Graph_componentstate*_tmp62=((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict,int))Cyc_Dict_lookup)(ts->cs,C);struct Cyc_Graph_componentstate*cs=_tmp62;
struct Cyc_Set_Set*_tmp63=Cyc_Set_empty(cmp);struct Cyc_Set_Set*targets=_tmp63;
struct Cyc_Set_Set*_tmp64=cs->Succ;struct Cyc_Set_Set*Succ=_tmp64;
while(!((int(*)(struct Cyc_Set_Set*))Cyc_Set_is_empty)(Succ)){
int _tmp65=((int(*)(struct Cyc_Set_Set*))Cyc_Set_choose)(Succ);int C2=_tmp65;
Succ=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,int))Cyc_Set_delete)(Succ,C2);
targets=({struct Cyc_Set_Set*_tmpBC=targets;Cyc_Set_union_two(_tmpBC,(
((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict,int))Cyc_Dict_lookup)(ts->cs,C2))->nodes);});}{
# 336
struct Cyc_Set_Set*_tmp66=cs->nodes;struct Cyc_Set_Set*nodes=_tmp66;
while(!Cyc_Set_is_empty(nodes)){
void*_tmp67=Cyc_Set_choose(nodes);void*v=_tmp67;
nodes=Cyc_Set_delete(nodes,v);
result=Cyc_Graph_add_edges(result,v,targets);}}}}
# 343
return result;}}
# 356 "graph.cyc"
static void Cyc_Graph_traverse(struct Cyc_Dict_Dict input,struct Cyc_Dict_Dict*output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned k){
# 360
({struct Cyc_List_List*_tmpBD=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=x,_tmp69->tl=*S;_tmp69;});*S=_tmpBD;});
({struct Cyc_Dict_Dict _tmpBE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert)(*N,x,k);*N=_tmpBE;});
{struct Cyc_Set_Set*_tmp6A=Cyc_Graph_get_targets(input,x);struct Cyc_Set_Set*targets=_tmp6A;for(0;!Cyc_Set_is_empty(targets);1){
void*_tmp6B=Cyc_Set_choose(targets);void*y=_tmp6B;
targets=Cyc_Set_delete(targets,y);
if(((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(*N,y)== (unsigned)0)
Cyc_Graph_traverse(input,output,N,S,y,k + (unsigned)1);{
unsigned _tmp6C=((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(*N,y);unsigned Ny=_tmp6C;
if(({unsigned _tmpBF=Ny;_tmpBF < ((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(*N,x);}))
({struct Cyc_Dict_Dict _tmpC0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert)(*N,x,Ny);*N=_tmpC0;});}}}
# 371
if(({unsigned _tmpC1=((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(*N,x);_tmpC1 == k;})){
int(*_tmp6D)(void*,void*)=(*output).rel;int(*cmp)(void*,void*)=_tmp6D;
struct Cyc_Set_Set*_tmp6E=Cyc_Set_empty(cmp);struct Cyc_Set_Set*component=_tmp6E;
{struct Cyc_List_List*_tmp6F=*S;struct Cyc_List_List*s=_tmp6F;for(0;(unsigned)s;s=s->tl){
void*_tmp70=s->hd;void*top=_tmp70;
component=Cyc_Set_insert(component,top);
if(cmp(top,x)== 0)break;}}
# 379
for(1;(unsigned)*S;*S=((struct Cyc_List_List*)_check_null(*S))->tl){
void*_tmp71=((struct Cyc_List_List*)_check_null(*S))->hd;void*top=_tmp71;
({struct Cyc_Dict_Dict _tmpC2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert)(*N,top,- 1U);*N=_tmpC2;});
({struct Cyc_Dict_Dict _tmpC3=Cyc_Graph_add_node(*output,top);*output=_tmpC3;});
({struct Cyc_Dict_Dict _tmpC4=Cyc_Graph_add_edges(*output,top,component);*output=_tmpC4;});
if(cmp(top,x)== 0){
*S=((struct Cyc_List_List*)_check_null(*S))->tl;
break;}}}}
# 391
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict input){
int(*_tmp72)(void*,void*)=input.rel;int(*cmp)(void*,void*)=_tmp72;
struct Cyc_List_List*_tmp73=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict))Cyc_Dict_to_list)(input);struct Cyc_List_List*edges=_tmp73;
struct Cyc_Dict_Dict _tmp74=Cyc_Graph_empty(cmp);struct Cyc_Dict_Dict output=_tmp74;
struct Cyc_List_List*_tmp75=0;struct Cyc_List_List*S=_tmp75;
struct Cyc_Dict_Dict N=((struct Cyc_Dict_Dict(*)(int(*)(void*,void*)))Cyc_Dict_empty)(cmp);
{struct Cyc_List_List*_tmp76=edges;struct Cyc_List_List*e=_tmp76;for(0;(unsigned)e;e=e->tl){
struct _tuple0*_tmp77=(struct _tuple0*)e->hd;struct _tuple0*_stmttmpA=_tmp77;struct _tuple0*_tmp78=_stmttmpA;void*_tmp79;_tmp79=_tmp78->f1;{void*x=_tmp79;
N=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert)(N,x,0U);}}}
# 401
{struct Cyc_List_List*_tmp7A=edges;struct Cyc_List_List*e=_tmp7A;for(0;(unsigned)e;e=e->tl){
struct _tuple0*_tmp7B=(struct _tuple0*)e->hd;struct _tuple0*_stmttmpB=_tmp7B;struct _tuple0*_tmp7C=_stmttmpB;void*_tmp7D;_tmp7D=_tmp7C->f1;{void*x=_tmp7D;
if(((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(N,x)== (unsigned)0)
Cyc_Graph_traverse(input,& output,& N,& S,x,1U);}}}
# 406
return output;}
# 410
static void Cyc_Graph_tsort0(struct Cyc_Dict_Dict input,struct Cyc_List_List**output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned k){
# 414
({struct Cyc_List_List*_tmpC5=({struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->hd=x,_tmp7E->tl=*S;_tmp7E;});*S=_tmpC5;});
({struct Cyc_Dict_Dict _tmpC6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert)(*N,x,k);*N=_tmpC6;});
{struct Cyc_Set_Set*_tmp7F=Cyc_Graph_get_targets(input,x);struct Cyc_Set_Set*targets=_tmp7F;for(0;!Cyc_Set_is_empty(targets);1){
void*_tmp80=Cyc_Set_choose(targets);void*y=_tmp80;
targets=Cyc_Set_delete(targets,y);
if(((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(*N,y)== (unsigned)0)
Cyc_Graph_tsort0(input,output,N,S,y,k + (unsigned)1);{
unsigned _tmp81=((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(*N,y);unsigned Ny=_tmp81;
if(({unsigned _tmpC7=Ny;_tmpC7 < ((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(*N,x);}))
({struct Cyc_Dict_Dict _tmpC8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert)(*N,x,Ny);*N=_tmpC8;});}}}
# 425
if(({unsigned _tmpC9=((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(*N,x);_tmpC9 == k;})){
int(*_tmp82)(void*,void*)=input.rel;int(*cmp)(void*,void*)=_tmp82;
struct Cyc_Set_Set*_tmp83=Cyc_Set_empty(cmp);struct Cyc_Set_Set*component=_tmp83;
{struct Cyc_List_List*_tmp84=*S;struct Cyc_List_List*s=_tmp84;for(0;(unsigned)s;s=s->tl){
void*_tmp85=s->hd;void*top=_tmp85;
component=Cyc_Set_insert(component,top);
if(cmp(top,x)== 0)break;}}
# 433
for(1;(unsigned)*S;*S=((struct Cyc_List_List*)_check_null(*S))->tl){
void*_tmp86=((struct Cyc_List_List*)_check_null(*S))->hd;void*top=_tmp86;
({struct Cyc_Dict_Dict _tmpCA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert)(*N,top,- 1U);*N=_tmpCA;});
({struct Cyc_List_List*_tmpCB=({struct Cyc_List_List*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->hd=top,_tmp87->tl=*output;_tmp87;});*output=_tmpCB;});
if(cmp(top,x)== 0){
*S=((struct Cyc_List_List*)_check_null(*S))->tl;
break;}}}}
# 444
struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict input){
int(*_tmp88)(void*,void*)=input.rel;int(*cmp)(void*,void*)=_tmp88;
struct Cyc_List_List*_tmp89=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict))Cyc_Dict_to_list)(input);struct Cyc_List_List*edges=_tmp89;
struct Cyc_List_List*_tmp8A=0;struct Cyc_List_List*output=_tmp8A;
struct Cyc_List_List*_tmp8B=0;struct Cyc_List_List*S=_tmp8B;
struct Cyc_Dict_Dict N=((struct Cyc_Dict_Dict(*)(int(*)(void*,void*)))Cyc_Dict_empty)(cmp);
{struct Cyc_List_List*_tmp8C=edges;struct Cyc_List_List*e=_tmp8C;for(0;(unsigned)e;e=e->tl){
struct _tuple0*_tmp8D=(struct _tuple0*)e->hd;struct _tuple0*_stmttmpC=_tmp8D;struct _tuple0*_tmp8E=_stmttmpC;void*_tmp8F;_tmp8F=_tmp8E->f1;{void*x=_tmp8F;
N=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert)(N,x,0U);}}}
# 454
{struct Cyc_List_List*_tmp90=edges;struct Cyc_List_List*e=_tmp90;for(0;(unsigned)e;e=e->tl){
struct _tuple0*_tmp91=(struct _tuple0*)e->hd;struct _tuple0*_stmttmpD=_tmp91;struct _tuple0*_tmp92=_stmttmpD;void*_tmp93;_tmp93=_tmp92->f1;{void*x=_tmp93;
if(((unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup)(N,x)== (unsigned)0)
Cyc_Graph_tsort0(input,& output,& N,& S,x,1U);}}}
# 459
return output;}

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
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 111 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 379
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Set_Set;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 64
struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 302
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 315
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 322
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 424 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 513
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 520
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 538
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 703 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 886
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 894
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 900
void*Cyc_Absyn_compress_kb(void*);
# 911
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 913
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 916
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 918
extern void*Cyc_Absyn_sint_type;
# 921
void*Cyc_Absyn_gen_float_type(unsigned i);
# 923
extern void*Cyc_Absyn_unique_rgn_type;
# 925
extern void*Cyc_Absyn_empty_effect;
# 927
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 929
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 958
void*Cyc_Absyn_bounds_one();
# 985
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 993
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1059
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1151
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 72
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 140
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 142
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 41
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_array_type(void*);
# 97
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 106
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 110
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 121
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 146
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 153
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 156
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 162
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 189
int Cyc_Tcutil_unify(void*,void*);
# 194
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 196
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 216
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 224
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr err_msg);
# 235
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 246
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 254
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 259
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 269
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 284
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 309
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 316
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 319
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**);
# 321
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**);
# 62 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 44 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Aggrdecl**_tmp2A;struct Cyc_List_List**_tmp29;struct Cyc_List_List*_tmp28;int _tmp27;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp25;int _tmp24;switch(*((int*)_tmp1)){case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp26=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp25=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp24=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2:
# 50
 if(topt == 0)
({void*_tmp2=0U;({unsigned _tmp3D0=p->loc;struct _fat_ptr _tmp3CF=({const char*_tmp3="cannot determine pattern type";_tag_fat(_tmp3,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp3D0,_tmp3CF,_tag_fat(_tmp2,sizeof(void*),0U));});});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp4=t;union Cyc_Absyn_AggrInfo _tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->f1)->tag == 20U){_LL10: _tmpA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->f1)->f1;_LL11:
# 55
({void*_tmp3D2=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp3D1=({union Cyc_Absyn_AggrInfo*_tmp5=_cycalloc(sizeof(*_tmp5));*_tmp5=_tmpA;_tmp5;});_tmp6->f1=_tmp3D1;}),_tmp6->f2=_tmp26,_tmp6->f3=_tmp25,_tmp6->f4=_tmp24;_tmp6;});p->r=_tmp3D2;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}else{goto _LL12;}}else{_LL12: _LL13:
# 59
({struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp37E;_tmp37E.tag=0U,({struct _fat_ptr _tmp3D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp37E.f1=_tmp3D3;});_tmp37E;});void*_tmp7[1U];_tmp7[0]=& _tmp9;({unsigned _tmp3D5=p->loc;struct _fat_ptr _tmp3D4=({const char*_tmp8="pattern expects aggregate type instead of %s";_tag_fat(_tmp8,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp3D5,_tmp3D4,_tag_fat(_tmp7,sizeof(void*),1U));});});
goto _LLF;}_LLF:;}
# 62
return;};}else{if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).tag == 2){_LL3: _tmp2A=((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).val;_tmp29=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp28=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp27=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {
# 65
struct Cyc_Absyn_Aggrdecl*_tmpB=*_tmp2A;
if(_tmpB->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp37F;_tmp37F.tag=0U,({
struct _fat_ptr _tmp3D6=(struct _fat_ptr)((int)_tmpB->kind == (int)Cyc_Absyn_StructA?({const char*_tmpF="struct";_tag_fat(_tmpF,sizeof(char),7U);}):({const char*_tmp10="union";_tag_fat(_tmp10,sizeof(char),6U);}));_tmp37F.f1=_tmp3D6;});_tmp37F;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({unsigned _tmp3D8=p->loc;struct _fat_ptr _tmp3D7=({const char*_tmpD="can't destructure an abstract %s";_tag_fat(_tmpD,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp3D8,_tmp3D7,_tag_fat(_tmpC,sizeof(void*),1U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 72
int more_exists=({int _tmp3D9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB->impl))->exist_vars);_tmp3D9 - ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp29);});
if(more_exists < 0){
({void*_tmp11=0U;({unsigned _tmp3DB=p->loc;struct _fat_ptr _tmp3DA=({const char*_tmp12="too many existentially bound type variables in pattern";_tag_fat(_tmp12,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp3DB,_tmp3DA,_tag_fat(_tmp11,sizeof(void*),0U));});});{
struct Cyc_List_List**_tmp13=_tmp29;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB->impl))->exist_vars);for(0;n != 0;-- n){
_tmp13=&((struct Cyc_List_List*)_check_null(*_tmp13))->tl;}}
*_tmp13=0;};}else{
if(more_exists > 0){
# 81
struct Cyc_List_List*_tmp14=0;
for(0;more_exists != 0;-- more_exists){
_tmp14=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct Cyc_Absyn_Tvar*_tmp3DC=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=1U,_tmp15->f1=0;_tmp15;}));_tmp16->hd=_tmp3DC;}),_tmp16->tl=_tmp14;_tmp16;});}
({struct Cyc_List_List*_tmp3DD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp29,_tmp14);*_tmp29=_tmp3DD;});}}
# 86
return;};}}else{_LLB: _LLC:
# 99
({void*_tmp22=0U;({struct _fat_ptr _tmp3DE=({const char*_tmp23="resolve_pat unknownAggr";_tag_fat(_tmp23,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp3DE,_tag_fat(_tmp22,sizeof(void*),0U));});});}}case 17U: _LL5: _tmp2B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6:
# 88
 Cyc_Tcexp_tcExp(te,0,_tmp2B);
if(!Cyc_Tcutil_is_const_exp(_tmp2B)){
({void*_tmp17=0U;({unsigned _tmp3E0=p->loc;struct _fat_ptr _tmp3DF=({const char*_tmp18="non-constant expression in case pattern";_tag_fat(_tmp18,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp3E0,_tmp3DF,_tag_fat(_tmp17,sizeof(void*),0U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 93
struct _tuple14 _tmp19=Cyc_Evexp_eval_const_uint_exp(_tmp2B);struct _tuple14 _tmp1A=_tmp19;unsigned _tmp1D;int _tmp1C;_LL15: _tmp1D=_tmp1A.f1;_tmp1C=_tmp1A.f2;_LL16:;
({void*_tmp3E1=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=10U,_tmp1B->f1=Cyc_Absyn_None,_tmp1B->f2=(int)_tmp1D;_tmp1B;});p->r=_tmp3E1;});
return;};case 15U: _LL7: _LL8:
# 97
({void*_tmp1E=0U;({struct _fat_ptr _tmp3E2=({const char*_tmp1F="resolve_pat UnknownId_p";_tag_fat(_tmp1F,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp3E2,_tag_fat(_tmp1E,sizeof(void*),0U));});});case 16U: _LL9: _LLA:
({void*_tmp20=0U;({struct _fat_ptr _tmp3E3=({const char*_tmp21="resolve_pat UnknownCall_p";_tag_fat(_tmp21,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp3E3,_tag_fat(_tmp20,sizeof(void*),0U));});});default: _LLD: _LLE:
# 101
 return;}_LL0:;}
# 105
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 108
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->v=s;_tmp2C;}));}
# 113
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 117
if(topt != 0 && Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;
# 120
{void*_tmp2D=Cyc_Tcutil_compress(numt);void*_tmp2E=_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->f1)){case 15U: _LL1: _LL2:
 goto _LL4;case 16U: _LL3: _LL4:
 if(topt != 0)return*topt;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 125
return numt;}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 128
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 130
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp3E4=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_tmp3E4;});}
# 134
({struct Cyc_List_List*_tmp3E6=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _tuple15*_tmp3E5=({struct _tuple15*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->f1=vd,_tmp2F->f2=e;_tmp2F;});_tmp30->hd=_tmp3E5;}),_tmp30->tl=*v_result_ptr;_tmp30;});*v_result_ptr=_tmp3E6;});}
# 136
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 138
struct Cyc_Tcpat_TcPatResult _tmp31=res1;struct _tuple1*_tmp39;struct Cyc_List_List*_tmp38;_LL1: _tmp39=_tmp31.tvars_and_bounds_opt;_tmp38=_tmp31.patvars;_LL2:;{
struct Cyc_Tcpat_TcPatResult _tmp32=res2;struct _tuple1*_tmp37;struct Cyc_List_List*_tmp36;_LL4: _tmp37=_tmp32.tvars_and_bounds_opt;_tmp36=_tmp32.patvars;_LL5:;
if(_tmp39 != 0 || _tmp37 != 0){
if(_tmp39 == 0)
_tmp39=({struct _tuple1*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=0,_tmp33->f2=0;_tmp33;});
if(_tmp37 == 0)
_tmp37=({struct _tuple1*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->f1=0,_tmp34->f2=0;_tmp34;});
return({struct Cyc_Tcpat_TcPatResult _tmp380;({struct _tuple1*_tmp3EA=({struct _tuple1*_tmp35=_cycalloc(sizeof(*_tmp35));({struct Cyc_List_List*_tmp3E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp39).f1,(*_tmp37).f1);_tmp35->f1=_tmp3E9;}),({
struct Cyc_List_List*_tmp3E8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp39).f2,(*_tmp37).f2);_tmp35->f2=_tmp3E8;});_tmp35;});
# 145
_tmp380.tvars_and_bounds_opt=_tmp3EA;}),({
# 147
struct Cyc_List_List*_tmp3E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp38,_tmp36);_tmp380.patvars=_tmp3E7;});_tmp380;});}
# 149
return({struct Cyc_Tcpat_TcPatResult _tmp381;_tmp381.tvars_and_bounds_opt=0,({struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp38,_tmp36);_tmp381.patvars=_tmp3EB;});_tmp381;});};}
# 152
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){
return({struct Cyc_Absyn_Pat*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->loc=loc,_tmp3A->topt=0,_tmp3A->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp3A;});}
# 157
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp3ED=t;Cyc_Tcutil_promote_array(_tmp3ED,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp3B=_cycalloc(sizeof(*_tmp3B));({struct Cyc_List_List*_tmp3EC=Cyc_Tcenv_lookup_type_vars(te);_tmp3B->v=_tmp3EC;});_tmp3B;})): rgn_opt,0);}): t;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};
# 164
static struct _tuple16*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple16*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=tv,_tmp3C->f2=0;_tmp3C;});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 168
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 171
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp394;_tmp394.tvars_and_bounds_opt=0,_tmp394.patvars=0;_tmp394;});
# 176
{void*_tmp3D=p->r;void*_tmp3E=_tmp3D;struct Cyc_Absyn_Datatypedecl*_tmpF4;struct Cyc_Absyn_Datatypefield*_tmpF3;struct Cyc_List_List**_tmpF2;int _tmpF1;struct Cyc_Absyn_Aggrdecl*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_List_List**_tmpEE;int _tmpED;struct Cyc_List_List**_tmpEC;int _tmpEB;struct Cyc_Absyn_Pat*_tmpEA;void*_tmpE9;struct Cyc_Absyn_Enumdecl*_tmpE8;int _tmpE7;struct Cyc_Absyn_Tvar*_tmpE6;struct Cyc_Absyn_Vardecl*_tmpE5;struct Cyc_Absyn_Vardecl*_tmpE4;struct Cyc_Absyn_Pat*_tmpE3;struct Cyc_Absyn_Tvar*_tmpE2;struct Cyc_Absyn_Vardecl*_tmpE1;struct Cyc_Absyn_Vardecl*_tmpE0;struct Cyc_Absyn_Pat*_tmpDF;switch(*((int*)_tmp3E)){case 0U: _LL1: _LL2:
# 179
 if(topt != 0)
t=*topt;else{
# 182
t=({struct Cyc_List_List*_tmp3EE=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp3EE,topt);});}
goto _LL0;case 1U: _LL3: _tmpE0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpDF=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL4: {
# 186
struct _tuple2*_tmp3F=_tmpE0->name;struct _tuple2*_tmp40=_tmp3F;struct _fat_ptr _tmp4D;_LL2E: _tmp4D=*_tmp40->f2;_LL2F:;
if(({struct _fat_ptr _tmp3F0=(struct _fat_ptr)_tmp4D;Cyc_strcmp(_tmp3F0,({const char*_tmp41="true";_tag_fat(_tmp41,sizeof(char),5U);}));})== 0 ||({struct _fat_ptr _tmp3EF=(struct _fat_ptr)_tmp4D;Cyc_strcmp(_tmp3EF,({const char*_tmp42="false";_tag_fat(_tmp42,sizeof(char),6U);}));})== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp45=({struct Cyc_String_pa_PrintArg_struct _tmp382;_tmp382.tag=0U,_tmp382.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp4D);_tmp382;});void*_tmp43[1U];_tmp43[0]=& _tmp45;({unsigned _tmp3F2=p->loc;struct _fat_ptr _tmp3F1=({const char*_tmp44="you probably do not want to use %s as a local variable";_tag_fat(_tmp44,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp3F2,_tmp3F1,_tag_fat(_tmp43,sizeof(void*),1U));});});
res=Cyc_Tcpat_tcPatRec(te,_tmpDF,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpDF->topt);
# 193
{void*_tmp46=Cyc_Tcutil_compress(t);void*_tmp47=_tmp46;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->tag == 4U){_LL31: _LL32:
# 195
 if(rgn_pat == 0 || !allow_ref_pat)
({void*_tmp48=0U;({unsigned _tmp3F4=p->loc;struct _fat_ptr _tmp3F3=({const char*_tmp49="array reference would point into unknown/unallowed region";_tag_fat(_tmp49,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp3F4,_tmp3F3,_tag_fat(_tmp48,sizeof(void*),0U));});});
goto _LL30;}else{_LL33: _LL34:
# 199
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp4A=0U;({unsigned _tmp3F6=p->loc;struct _fat_ptr _tmp3F5=({const char*_tmp4B="pattern would point to an abstract member";_tag_fat(_tmp4B,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp3F6,_tmp3F5,_tag_fat(_tmp4A,sizeof(void*),0U));});});
goto _LL30;}_LL30:;}
# 203
({struct Cyc_Absyn_Vardecl**_tmp3F9=({struct Cyc_Absyn_Vardecl**_tmp4C=_cycalloc(sizeof(*_tmp4C));*_tmp4C=_tmpE0;_tmp4C;});struct Cyc_Absyn_Exp*_tmp3F8=access_exp;struct Cyc_List_List**_tmp3F7=& res.patvars;Cyc_Tcpat_set_vd(_tmp3F9,_tmp3F8,_tmp3F7,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}case 2U: _LL5: _tmpE2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL6: {
# 206
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp3FB=p->loc;struct Cyc_Tcenv_Tenv*_tmp3FA=te;Cyc_Tcenv_add_type_vars(_tmp3FB,_tmp3FA,({struct Cyc_Absyn_Tvar*_tmp58[1U];_tmp58[0]=_tmpE2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp58,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp3FC=({struct _tuple1*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=0,_tmp4E->f2=0;_tmp4E;});res.tvars_and_bounds_opt=_tmp3FC;});
({struct Cyc_List_List*_tmp3FF=({
struct Cyc_List_List*_tmp3FE=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3FE,({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _tuple16*_tmp3FD=({struct _tuple16*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=_tmpE2,_tmp4F->f2=1;_tmp4F;});_tmp50->hd=_tmp3FD;}),_tmp50->tl=0;_tmp50;}));});
# 209
(*res.tvars_and_bounds_opt).f1=_tmp3FF;});
# 211
({unsigned _tmp402=p->loc;struct Cyc_Tcenv_Tenv*_tmp401=te2;struct Cyc_List_List*_tmp400=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp402,_tmp401,_tmp400,& Cyc_Tcutil_tmk,1,0,_tmpE1->type);});
# 214
if(topt != 0)t=*topt;else{
t=({struct Cyc_List_List*_tmp403=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp403,topt);});}
{void*_tmp51=Cyc_Tcutil_compress(t);void*_tmp52=_tmp51;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->tag == 4U){_LL36: _LL37:
# 218
 if(rgn_pat == 0 || !allow_ref_pat)
({void*_tmp53=0U;({unsigned _tmp405=p->loc;struct _fat_ptr _tmp404=({const char*_tmp54="array reference would point into unknown/unallowed region";_tag_fat(_tmp54,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp405,_tmp404,_tag_fat(_tmp53,sizeof(void*),0U));});});
goto _LL35;}else{_LL38: _LL39:
# 222
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp55=0U;({unsigned _tmp407=p->loc;struct _fat_ptr _tmp406=({const char*_tmp56="pattern would point to an abstract member";_tag_fat(_tmp56,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp407,_tmp406,_tag_fat(_tmp55,sizeof(void*),0U));});});
goto _LL35;}_LL35:;}
# 226
({struct Cyc_Absyn_Vardecl**_tmp40A=({struct Cyc_Absyn_Vardecl**_tmp57=_cycalloc(sizeof(*_tmp57));*_tmp57=_tmpE1;_tmp57;});struct Cyc_Absyn_Exp*_tmp409=access_exp;struct Cyc_List_List**_tmp408=& res.patvars;Cyc_Tcpat_set_vd(_tmp40A,_tmp409,_tmp408,_tmpE1->type);});
goto _LL0;}case 3U: _LL7: _tmpE4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL8:
# 230
 res=Cyc_Tcpat_tcPatRec(te,_tmpE3,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpE3->topt);
if(!allow_ref_pat || rgn_pat == 0){
({void*_tmp59=0U;({unsigned _tmp40C=p->loc;struct _fat_ptr _tmp40B=({const char*_tmp5A="* pattern would point into an unknown/unallowed region";_tag_fat(_tmp5A,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp40C,_tmp40B,_tag_fat(_tmp59,sizeof(void*),0U));});});
goto _LL0;}else{
# 237
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({void*_tmp5B=0U;({unsigned _tmp40E=p->loc;struct _fat_ptr _tmp40D=({const char*_tmp5C="* pattern cannot take the address of an alias-free path";_tag_fat(_tmp5C,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp40E,_tmp40D,_tag_fat(_tmp5B,sizeof(void*),0U));});});}{
# 240
struct Cyc_Absyn_Exp*new_access_exp=0;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=3U,(_tmp5E->f1).elt_type=t,({struct Cyc_Absyn_Tqual _tmp411=Cyc_Absyn_empty_tqual(0U);(_tmp5E->f1).elt_tq=_tmp411;}),
((_tmp5E->f1).ptr_atts).rgn=rgn_pat,((_tmp5E->f1).ptr_atts).nullable=Cyc_Absyn_false_type,({
void*_tmp410=Cyc_Tcutil_any_bounds(& te);((_tmp5E->f1).ptr_atts).bounds=_tmp410;}),({void*_tmp40F=Cyc_Tcutil_any_bool(& te);((_tmp5E->f1).ptr_atts).zero_term=_tmp40F;}),((_tmp5E->f1).ptr_atts).ptrloc=0;_tmp5E;});
# 245
if((unsigned)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);
new_access_exp->topt=t2;}
# 249
({struct Cyc_Absyn_Vardecl**_tmp414=({struct Cyc_Absyn_Vardecl**_tmp5D=_cycalloc(sizeof(*_tmp5D));*_tmp5D=_tmpE4;_tmp5D;});struct Cyc_Absyn_Exp*_tmp413=new_access_exp;struct Cyc_List_List**_tmp412=& res.patvars;Cyc_Tcpat_set_vd(_tmp414,_tmp413,_tmp412,t2);});
goto _LL0;};case 4U: _LL9: _tmpE6=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LLA:
# 255
({struct Cyc_Absyn_Vardecl**_tmp417=({struct Cyc_Absyn_Vardecl**_tmp5F=_cycalloc(sizeof(*_tmp5F));*_tmp5F=_tmpE5;_tmp5F;});struct Cyc_Absyn_Exp*_tmp416=access_exp;struct Cyc_List_List**_tmp415=& res.patvars;Cyc_Tcpat_set_vd(_tmp417,_tmp416,_tmp415,_tmpE5->type);});
# 259
({unsigned _tmp419=p->loc;struct Cyc_Tcenv_Tenv*_tmp418=te;Cyc_Tcenv_add_type_vars(_tmp419,_tmp418,({struct Cyc_Absyn_Tvar*_tmp60[1U];_tmp60[0]=_tmpE6;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp60,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp41A=({struct _tuple1*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->f1=0,_tmp61->f2=0;_tmp61;});res.tvars_and_bounds_opt=_tmp41A;});
({struct Cyc_List_List*_tmp41C=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));
({struct _tuple16*_tmp41B=({struct _tuple16*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=_tmpE6,_tmp62->f2=0;_tmp62;});_tmp63->hd=_tmp41B;}),_tmp63->tl=(*res.tvars_and_bounds_opt).f1;_tmp63;});
# 262
(*res.tvars_and_bounds_opt).f1=_tmp41C;});
# 264
t=Cyc_Absyn_uint_type;
goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3E)->f1){case Cyc_Absyn_Unsigned: _LLB: _LLC:
# 267
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;case Cyc_Absyn_None: _LLD: _LLE:
 goto _LL10;default: _LLF: _LL10:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;}case 11U: _LL11: _LL12:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 12U: _LL13: _tmpE7=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL14:
 t=({void**_tmp41D=topt;Cyc_Tcpat_num_type(_tmp41D,Cyc_Absyn_gen_float_type((unsigned)_tmpE7));});goto _LL0;case 13U: _LL15: _tmpE8=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL16:
# 273
 t=({void**_tmp41E=topt;Cyc_Tcpat_num_type(_tmp41E,Cyc_Absyn_enum_type(_tmpE8->name,_tmpE8));});
goto _LL0;case 14U: _LL17: _tmpE9=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL18:
 t=Cyc_Tcpat_num_type(topt,_tmpE9);goto _LL0;case 9U: _LL19: _LL1A:
# 277
 if(topt != 0){
void*_tmp64=Cyc_Tcutil_compress(*topt);void*_tmp65=_tmp64;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65)->tag == 3U){_LL3B: _LL3C:
 t=*topt;goto tcpat_end;}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}{
# 282
struct Cyc_Core_Opt*_tmp66=Cyc_Tcenv_lookup_opt_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=3U,({void*_tmp423=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp66);(_tmp67->f1).elt_type=_tmp423;}),({
struct Cyc_Absyn_Tqual _tmp422=Cyc_Absyn_empty_tqual(0U);(_tmp67->f1).elt_tq=_tmp422;}),
({void*_tmp421=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp66);((_tmp67->f1).ptr_atts).rgn=_tmp421;}),((_tmp67->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
# 287
void*_tmp420=Cyc_Tcutil_any_bounds(& te);((_tmp67->f1).ptr_atts).bounds=_tmp420;}),({void*_tmp41F=Cyc_Tcutil_any_bool(& te);((_tmp67->f1).ptr_atts).zero_term=_tmp41F;}),((_tmp67->f1).ptr_atts).ptrloc=0;_tmp67;});
goto _LL0;};case 6U: _LL1B: _tmpEA=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL1C: {
# 293
void*inner_typ=Cyc_Absyn_void_type;
void**_tmp68=0;
int elt_const=0;
if(topt != 0){
void*_tmp69=Cyc_Tcutil_compress(*topt);void*_tmp6A=_tmp69;void*_tmp6C;struct Cyc_Absyn_Tqual _tmp6B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A)->tag == 3U){_LL40: _tmp6C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A)->f1).elt_type;_tmp6B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A)->f1).elt_tq;_LL41:
# 299
 inner_typ=_tmp6C;
_tmp68=& inner_typ;
elt_const=_tmp6B.real_const;
goto _LL3F;}else{_LL42: _LL43:
 goto _LL3F;}_LL3F:;}{
# 308
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp424=res;Cyc_Tcpat_combine_results(_tmp424,Cyc_Tcpat_tcPatRec(te,_tmpEA,_tmp68,ptr_rgn,1,new_access_exp));});
# 317
{void*_tmp6D=Cyc_Tcutil_compress((void*)_check_null(_tmpEA->topt));void*_tmp6E=_tmp6D;struct Cyc_Absyn_Datatypedecl*_tmp76;struct Cyc_Absyn_Datatypefield*_tmp75;struct Cyc_List_List*_tmp74;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1)->f1).KnownDatatypefield).tag == 2){_LL45: _tmp76=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1)->f1).KnownDatatypefield).val).f1;_tmp75=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1)->f1).KnownDatatypefield).val).f2;_tmp74=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f2;_LL46:
# 321
{void*_tmp6F=Cyc_Tcutil_compress(inner_typ);void*_tmp70=_tmp6F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70)->f1)->tag == 19U){_LL4A: _LL4B:
# 323
 goto DONT_PROMOTE;}else{goto _LL4C;}}else{_LL4C: _LL4D:
 goto _LL49;}_LL49:;}{
# 327
void*new_type=({union Cyc_Absyn_DatatypeInfo _tmp425=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp72=_cycalloc(sizeof(*_tmp72));*_tmp72=_tmp76;_tmp72;}));Cyc_Absyn_datatype_type(_tmp425,_tmp74);});
# 329
_tmpEA->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=3U,(_tmp71->f1).elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _tmp429=Cyc_Absyn_const_tqual(0U);(_tmp71->f1).elt_tq=_tmp429;}):({
struct Cyc_Absyn_Tqual _tmp428=Cyc_Absyn_empty_tqual(0U);(_tmp71->f1).elt_tq=_tmp428;}),
((_tmp71->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp427=Cyc_Tcutil_any_bool(& te);((_tmp71->f1).ptr_atts).nullable=_tmp427;}),({
void*_tmp426=Cyc_Absyn_bounds_one();((_tmp71->f1).ptr_atts).bounds=_tmp426;}),((_tmp71->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp71->f1).ptr_atts).ptrloc=0;_tmp71;});
# 336
goto _LL44;};}else{goto _LL47;}}else{goto _LL47;}}else{_LL47: _LL48:
# 338
 DONT_PROMOTE:
 t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=3U,(_tmp73->f1).elt_type=(void*)_check_null(_tmpEA->topt),
elt_const?({struct Cyc_Absyn_Tqual _tmp42E=Cyc_Absyn_const_tqual(0U);(_tmp73->f1).elt_tq=_tmp42E;}):({
struct Cyc_Absyn_Tqual _tmp42D=Cyc_Absyn_empty_tqual(0U);(_tmp73->f1).elt_tq=_tmp42D;}),
((_tmp73->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp42C=Cyc_Tcutil_any_bool(& te);((_tmp73->f1).ptr_atts).nullable=_tmp42C;}),({
void*_tmp42B=Cyc_Tcutil_any_bounds(& te);((_tmp73->f1).ptr_atts).bounds=_tmp42B;}),({void*_tmp42A=Cyc_Tcutil_any_bool(& te);((_tmp73->f1).ptr_atts).zero_term=_tmp42A;}),((_tmp73->f1).ptr_atts).ptrloc=0;_tmp73;});}_LL44:;}
# 345
if((unsigned)new_access_exp)new_access_exp->topt=_tmpEA->topt;
goto _LL0;};}case 5U: _LL1D: _tmpEC=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpEB=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL1E: {
# 349
struct Cyc_List_List*_tmp77=*_tmpEC;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp78=Cyc_Tcutil_compress(*topt);void*_tmp79=_tmp78;struct Cyc_List_List*_tmp7F;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp79)->tag == 6U){_LL4F: _tmp7F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp79)->f1;_LL50:
# 355
 topt_ts=_tmp7F;
if(_tmpEB){
# 358
int _tmp7A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp77);
int _tmp7B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7F);
if(_tmp7A < _tmp7B){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp7B - _tmp7A;++ i){
wild_ps=({struct Cyc_List_List*_tmp7C=_cycalloc(sizeof(*_tmp7C));({struct Cyc_Absyn_Pat*_tmp42F=Cyc_Tcpat_wild_pat(p->loc);_tmp7C->hd=_tmp42F;}),_tmp7C->tl=wild_ps;_tmp7C;});}}
({struct Cyc_List_List*_tmp430=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp77,wild_ps);*_tmpEC=_tmp430;});
_tmp77=*_tmpEC;}else{
if(({int _tmp431=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp77);_tmp431 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7F);}))
({void*_tmp7D=0U;({unsigned _tmp433=p->loc;struct _fat_ptr _tmp432=({const char*_tmp7E="unnecessary ... in tuple pattern";_tag_fat(_tmp7E,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp433,_tmp432,_tag_fat(_tmp7D,sizeof(void*),0U));});});}}
# 370
goto _LL4E;}else{_LL51: _LL52:
# 372
 goto _LL4E;}_LL4E:;}else{
# 374
if(_tmpEB)
({void*_tmp80=0U;({unsigned _tmp435=p->loc;struct _fat_ptr _tmp434=({const char*_tmp81="cannot determine missing fields for ... in tuple pattern";_tag_fat(_tmp81,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp435,_tmp434,_tag_fat(_tmp80,sizeof(void*),0U));});});}
{int i=0;for(0;_tmp77 != 0;(_tmp77=_tmp77->tl,i ++)){
void**_tmp82=0;
if(topt_ts != 0){
_tmp82=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 382
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=({struct Cyc_Absyn_Exp*_tmp436=access_exp;Cyc_Absyn_subscript_exp(_tmp436,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp383;(_tmp383.Int_c).tag=5U,((_tmp383.Int_c).val).f1=Cyc_Absyn_Unsigned,((_tmp383.Int_c).val).f2=i;_tmp383;}),0U),0U);});
# 387
res=({struct Cyc_Tcpat_TcPatResult _tmp437=res;Cyc_Tcpat_combine_results(_tmp437,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp77->hd,_tmp82,rgn_pat,allow_ref_pat,new_access_exp));});
# 390
if((unsigned)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp77->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));({struct _tuple17*_tmp439=({struct _tuple17*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Absyn_Tqual _tmp438=Cyc_Absyn_empty_tqual(0U);_tmp83->f1=_tmp438;}),_tmp83->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp77->hd)->topt);_tmp83;});_tmp84->hd=_tmp439;}),_tmp84->tl=pat_ts;_tmp84;});};}}
# 393
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=6U,({struct Cyc_List_List*_tmp43A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp85->f1=_tmp43A;});_tmp85;});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1)->KnownAggr).tag == 2){_LL1F: _tmpF0=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1)->KnownAggr).val;_tmpEF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_tmpEE=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f3;_tmpED=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f4;_LL20: {
# 397
struct Cyc_List_List*_tmp86=*_tmpEE;
struct _fat_ptr aggr_str=(int)_tmpF0->kind == (int)Cyc_Absyn_StructA?({const char*_tmpC5="struct";_tag_fat(_tmpC5,sizeof(char),7U);}):({const char*_tmpC6="union";_tag_fat(_tmpC6,sizeof(char),6U);});
if(_tmpF0->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp384;_tmp384.tag=0U,_tmp384.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp384;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({unsigned _tmp43C=p->loc;struct _fat_ptr _tmp43B=({const char*_tmp88="can't destructure an abstract %s";_tag_fat(_tmp88,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp43C,_tmp43B,_tag_fat(_tmp87,sizeof(void*),1U));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL0;}
# 406
if((int)_tmpF0->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->tagged)
allow_ref_pat=0;
if(_tmpEF != 0){
# 412
if(topt == 0 || Cyc_Tcutil_type_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}
# 415
{struct _RegionHandle _tmp8A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8A;_push_region(rgn);
# 417
{struct Cyc_List_List*_tmp8B=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp8C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpEF;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp8C))->hd;
_tmp8C=_tmp8C->tl;{
void*_tmp8D=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp8E=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp8F=_tmp8E;struct Cyc_Absyn_Kind*_tmp93;struct Cyc_Absyn_Kind*_tmp92;switch(*((int*)_tmp8F)){case 2U: _LL54: _tmp92=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F)->f2;_LL55:
 _tmp93=_tmp92;goto _LL57;case 0U: _LL56: _tmp93=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F)->f1;_LL57:
 k2=_tmp93;goto _LL53;default: _LL58: _LL59:
({void*_tmp90=0U;({struct _fat_ptr _tmp43D=({const char*_tmp91="unconstrained existential type variable in aggregate";_tag_fat(_tmp91,sizeof(char),53U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp43D,_tag_fat(_tmp90,sizeof(void*),0U));});});}_LL53:;}
# 433
{void*_tmp94=_tmp8D;struct Cyc_Core_Opt**_tmp99;struct Cyc_Core_Opt**_tmp98;struct Cyc_Absyn_Kind*_tmp97;struct Cyc_Absyn_Kind*_tmp96;switch(*((int*)_tmp94)){case 0U: _LL5B: _tmp96=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL5C:
# 436
 if(!Cyc_Tcutil_kind_leq(k2,_tmp96))
error=1;
goto _LL5A;case 2U: _LL5D: _tmp98=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f1;_tmp97=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;_LL5E:
 _tmp99=_tmp98;goto _LL60;default: _LL5F: _tmp99=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL60:
({struct Cyc_Core_Opt*_tmp43E=({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->v=_tmp8E;_tmp95;});*_tmp99=_tmp43E;});goto _LL5A;}_LL5A:;}
# 442
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmp9C=({struct Cyc_String_pa_PrintArg_struct _tmp387;_tmp387.tag=0U,_tmp387.f1=(struct _fat_ptr)((struct _fat_ptr)*tv->name);_tmp387;});struct Cyc_String_pa_PrintArg_struct _tmp9D=({struct Cyc_String_pa_PrintArg_struct _tmp386;_tmp386.tag=0U,({
# 446
struct _fat_ptr _tmp43F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kindbound2string(_tmp8D));_tmp386.f1=_tmp43F;});_tmp386;});struct Cyc_String_pa_PrintArg_struct _tmp9E=({struct Cyc_String_pa_PrintArg_struct _tmp385;_tmp385.tag=0U,({struct _fat_ptr _tmp440=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k2));_tmp385.f1=_tmp440;});_tmp385;});void*_tmp9A[3U];_tmp9A[0]=& _tmp9C,_tmp9A[1]=& _tmp9D,_tmp9A[2]=& _tmp9E;({unsigned _tmp442=p->loc;struct _fat_ptr _tmp441=({const char*_tmp9B="type variable %s has kind %s but must have at least kind %s";_tag_fat(_tmp9B,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp442,_tmp441,_tag_fat(_tmp9A,sizeof(void*),3U));});});{
# 448
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=2U,_tmpA4->f1=tv;_tmpA4;});
_tmp8B=({struct Cyc_List_List*_tmp9F=_region_malloc(rgn,sizeof(*_tmp9F));_tmp9F->hd=vartype,_tmp9F->tl=_tmp8B;_tmp9F;});
# 452
if((int)k2->kind == (int)Cyc_Absyn_RgnKind){
if((int)k2->aliasqual == (int)Cyc_Absyn_Aliasable)
outlives_constraints=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*_tmpA1));
({struct _tuple0*_tmp443=({struct _tuple0*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->f1=Cyc_Absyn_empty_effect,_tmpA0->f2=vartype;_tmpA0;});_tmpA1->hd=_tmp443;}),_tmpA1->tl=outlives_constraints;_tmpA1;});else{
# 457
({void*_tmpA2=0U;({struct _fat_ptr _tmp444=({const char*_tmpA3="opened existential had unique or top region kind";_tag_fat(_tmpA3,sizeof(char),49U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp444,_tag_fat(_tmpA2,sizeof(void*),0U));});});}}};};}}
# 461
_tmp8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8B);{
# 463
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmpEF);
# 465
struct Cyc_List_List*_tmpA5=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmpA6=({struct _tuple12 _tmp38C;_tmp38C.f1=_tmpA5,_tmp38C.f2=rgn;_tmp38C;});
struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpA6,_tmpF0->tvs);
struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->exist_vars,_tmp8B);
struct Cyc_List_List*_tmpA9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpA7);
struct Cyc_List_List*_tmpAA=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpA8);
struct Cyc_List_List*_tmpAB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpA7,_tmpA8);
# 473
if(_tmpEF != 0 ||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp445=({struct _tuple1*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->f1=0,_tmpAC->f2=0;_tmpAC;});res.tvars_and_bounds_opt=_tmp445;});
({struct Cyc_List_List*_tmp447=({
struct Cyc_List_List*_tmp446=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp446,((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmpEF));});
# 476
(*res.tvars_and_bounds_opt).f1=_tmp447;});
# 478
({struct Cyc_List_List*_tmp448=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 478
(*res.tvars_and_bounds_opt).f2=_tmp448;});{
# 480
struct Cyc_List_List*_tmpAD=0;
{struct Cyc_List_List*_tmpAE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->rgn_po;for(0;_tmpAE != 0;_tmpAE=_tmpAE->tl){
_tmpAD=({struct Cyc_List_List*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct _tuple0*_tmp44B=({struct _tuple0*_tmpAF=_cycalloc(sizeof(*_tmpAF));({void*_tmp44A=Cyc_Tcutil_rsubstitute(rgn,_tmpAB,(*((struct _tuple0*)_tmpAE->hd)).f1);_tmpAF->f1=_tmp44A;}),({
void*_tmp449=Cyc_Tcutil_rsubstitute(rgn,_tmpAB,(*((struct _tuple0*)_tmpAE->hd)).f2);_tmpAF->f2=_tmp449;});_tmpAF;});
# 482
_tmpB0->hd=_tmp44B;}),_tmpB0->tl=_tmpAD;_tmpB0;});}}
# 485
_tmpAD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpAD);
({struct Cyc_List_List*_tmp44C=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmpAD);
# 486
(*res.tvars_and_bounds_opt).f2=_tmp44C;});};}
# 491
t=({union Cyc_Absyn_AggrInfo _tmp44D=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpB1=_cycalloc(sizeof(*_tmpB1));*_tmpB1=_tmpF0;_tmpB1;}));Cyc_Absyn_aggr_type(_tmp44D,_tmpA9);});
if(_tmpED &&(int)_tmpF0->kind == (int)Cyc_Absyn_UnionA)
({void*_tmpB2=0U;({unsigned _tmp44F=p->loc;struct _fat_ptr _tmp44E=({const char*_tmpB3="`...' pattern not allowed in union pattern";_tag_fat(_tmpB3,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp44F,_tmp44E,_tag_fat(_tmpB2,sizeof(void*),0U));});});else{
if(_tmpED){
# 496
int _tmpB4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp86);
int _tmpB5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->fields);
if(_tmpB4 < _tmpB5){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmpB5 - _tmpB4;++ i){
wild_dps=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));({struct _tuple18*_tmp451=({struct _tuple18*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->f1=0,({struct Cyc_Absyn_Pat*_tmp450=Cyc_Tcpat_wild_pat(p->loc);_tmpB6->f2=_tmp450;});_tmpB6;});_tmpB7->hd=_tmp451;}),_tmpB7->tl=wild_dps;_tmpB7;});}}
({struct Cyc_List_List*_tmp452=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp86,wild_dps);*_tmpEE=_tmp452;});
_tmp86=*_tmpEE;}else{
if(_tmpB4 == _tmpB5)
({void*_tmpB8=0U;({unsigned _tmp454=p->loc;struct _fat_ptr _tmp453=({const char*_tmpB9="unnecessary ... in struct pattern";_tag_fat(_tmpB9,sizeof(char),34U);});Cyc_Tcutil_warn(_tmp454,_tmp453,_tag_fat(_tmpB8,sizeof(void*),0U));});});}}}{
# 507
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp86,_tmpF0->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmpBA=(struct _tuple19*)fields->hd;struct _tuple19*_tmpBB=_tmpBA;struct Cyc_Absyn_Aggrfield*_tmpC4;struct Cyc_Absyn_Pat*_tmpC3;_LL62: _tmpC4=_tmpBB->f1;_tmpC3=_tmpBB->f2;_LL63:;{
void*_tmpBC=Cyc_Tcutil_rsubstitute(rgn,_tmpAB,_tmpC4->type);
# 513
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmpC4->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp455=res;Cyc_Tcpat_combine_results(_tmp455,Cyc_Tcpat_tcPatRec(te2,_tmpC3,& _tmpBC,rgn_pat,allow_ref_pat,new_access_exp));});
# 521
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpC3->topt),_tmpBC))
({struct Cyc_String_pa_PrintArg_struct _tmpBF=({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0U,_tmp38B.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmpC4->name);_tmp38B;});struct Cyc_String_pa_PrintArg_struct _tmpC0=({struct Cyc_String_pa_PrintArg_struct _tmp38A;_tmp38A.tag=0U,_tmp38A.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp38A;});struct Cyc_String_pa_PrintArg_struct _tmpC1=({struct Cyc_String_pa_PrintArg_struct _tmp389;_tmp389.tag=0U,({
struct _fat_ptr _tmp456=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmpBC));_tmp389.f1=_tmp456;});_tmp389;});struct Cyc_String_pa_PrintArg_struct _tmpC2=({struct Cyc_String_pa_PrintArg_struct _tmp388;_tmp388.tag=0U,({
struct _fat_ptr _tmp457=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpC3->topt)));_tmp388.f1=_tmp457;});_tmp388;});void*_tmpBD[4U];_tmpBD[0]=& _tmpBF,_tmpBD[1]=& _tmpC0,_tmpBD[2]=& _tmpC1,_tmpBD[3]=& _tmpC2;({unsigned _tmp459=p->loc;struct _fat_ptr _tmp458=({const char*_tmpBE="field %s of %s pattern expects type %s != %s";_tag_fat(_tmpBE,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp459,_tmp458,_tag_fat(_tmpBD,sizeof(void*),4U));});});
if((unsigned)new_access_exp)new_access_exp->topt=_tmpC3->topt;};}};};}
# 417
;_pop_region(rgn);}
# 528
goto _LL0;}}else{_LL25: _LL26:
# 581
 goto _LL28;}}else{_LL23: _LL24:
# 580
 goto _LL26;}case 8U: _LL21: _tmpF4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpF3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_tmpF2=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f3;_tmpF1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f4;_LL22: {
# 531
struct Cyc_List_List*_tmpC7=*_tmpF2;
struct Cyc_List_List*tqts=_tmpF3->typs;
# 534
struct Cyc_List_List*_tmpC8=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmpC9=({struct _tuple12 _tmp393;_tmp393.f1=_tmpC8,_tmp393.f2=Cyc_Core_heap_region;_tmp393;});
struct Cyc_List_List*_tmpCA=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmpC9,_tmpF4->tvs);
struct Cyc_List_List*_tmpCB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpCA);
t=({union Cyc_Absyn_DatatypeFieldInfo _tmp45A=Cyc_Absyn_KnownDatatypefield(_tmpF4,_tmpF3);Cyc_Absyn_datatype_field_type(_tmp45A,_tmpCB);});
if(_tmpF1){
# 541
int _tmpCC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC7);
int _tmpCD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmpCC < _tmpCD){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpCD - _tmpCC;++ i){
wild_ps=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));({struct Cyc_Absyn_Pat*_tmp45B=Cyc_Tcpat_wild_pat(p->loc);_tmpCE->hd=_tmp45B;}),_tmpCE->tl=wild_ps;_tmpCE;});}}
({struct Cyc_List_List*_tmp45C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC7,wild_ps);*_tmpF2=_tmp45C;});
_tmpC7=*_tmpF2;}else{
if(_tmpCC == _tmpCD)
({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp38D;_tmp38D.tag=0U,({
struct _fat_ptr _tmp45D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF4->name));_tmp38D.f1=_tmp45D;});_tmp38D;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({unsigned _tmp45F=p->loc;struct _fat_ptr _tmp45E=({const char*_tmpD0="unnecessary ... in datatype field %s";_tag_fat(_tmpD0,sizeof(char),37U);});Cyc_Tcutil_warn(_tmp45F,_tmp45E,_tag_fat(_tmpCF,sizeof(void*),1U));});});}}
# 553
for(0;_tmpC7 != 0 && tqts != 0;(_tmpC7=_tmpC7->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpD2=(struct Cyc_Absyn_Pat*)_tmpC7->hd;
# 557
void*_tmpD3=Cyc_Tcutil_substitute(_tmpCA,(*((struct _tuple17*)tqts->hd)).f2);
# 560
if((unsigned)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
res=({struct Cyc_Tcpat_TcPatResult _tmp460=res;Cyc_Tcpat_combine_results(_tmp460,Cyc_Tcpat_tcPatRec(te,_tmpD2,& _tmpD3,rgn_pat,allow_ref_pat,0));});
# 567
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpD2->topt),_tmpD3))
({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp390;_tmp390.tag=0U,({
struct _fat_ptr _tmp461=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF3->name));_tmp390.f1=_tmp461;});_tmp390;});struct Cyc_String_pa_PrintArg_struct _tmpD7=({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U,({struct _fat_ptr _tmp462=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmpD3));_tmp38F.f1=_tmp462;});_tmp38F;});struct Cyc_String_pa_PrintArg_struct _tmpD8=({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0U,({
struct _fat_ptr _tmp463=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpD2->topt)));_tmp38E.f1=_tmp463;});_tmp38E;});void*_tmpD4[3U];_tmpD4[0]=& _tmpD6,_tmpD4[1]=& _tmpD7,_tmpD4[2]=& _tmpD8;({unsigned _tmp465=_tmpD2->loc;struct _fat_ptr _tmp464=({const char*_tmpD5="%s expects argument type %s, not %s";_tag_fat(_tmpD5,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp465,_tmp464,_tag_fat(_tmpD4,sizeof(void*),3U));});});}
# 572
if(_tmpC7 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp391;_tmp391.tag=0U,({
struct _fat_ptr _tmp466=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF3->name));_tmp391.f1=_tmp466;});_tmp391;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({unsigned _tmp468=p->loc;struct _fat_ptr _tmp467=({const char*_tmpDA="too many arguments for datatype constructor %s";_tag_fat(_tmpDA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp468,_tmp467,_tag_fat(_tmpD9,sizeof(void*),1U));});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0U,({
struct _fat_ptr _tmp469=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF3->name));_tmp392.f1=_tmp469;});_tmp392;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({unsigned _tmp46B=p->loc;struct _fat_ptr _tmp46A=({const char*_tmpDD="too few arguments for datatype constructor %s";_tag_fat(_tmpDD,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp46B,_tmp46A,_tag_fat(_tmpDC,sizeof(void*),1U));});});
goto _LL0;}case 15U: _LL27: _LL28:
# 582
 goto _LL2A;case 17U: _LL29: _LL2A:
 goto _LL2C;default: _LL2B: _LL2C:
# 585
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL0;}_LL0:;}
# 587
tcpat_end:
 p->topt=t;
return res;};}
# 592
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 594
struct Cyc_Tcpat_TcPatResult _tmpF5=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 596
struct _tuple1 _tmpF6=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpF5.patvars);struct _tuple1 _tmpF7=_tmpF6;struct Cyc_List_List*_tmpFF;_LL1: _tmpFF=_tmpF7.f1;_LL2:;{
struct Cyc_List_List*_tmpF8=0;
{struct Cyc_List_List*x=_tmpFF;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)_tmpF8=({struct Cyc_List_List*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),_tmpF9->tl=_tmpF8;_tmpF9;});}}
({struct Cyc_List_List*_tmp46D=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_name,_tmpF8);unsigned _tmp46C=p->loc;Cyc_Tcutil_check_unique_vars(_tmp46D,_tmp46C,({const char*_tmpFA="pattern contains a repeated variable";_tag_fat(_tmpFA,sizeof(char),37U);}));});
# 605
{struct Cyc_List_List*x=_tmpF5.patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmpFB=(struct _tuple15*)x->hd;struct _tuple15*_tmpFC=_tmpFB;struct Cyc_Absyn_Vardecl**_tmpFE;struct Cyc_Absyn_Exp**_tmpFD;_LL4: _tmpFE=_tmpFC->f1;_tmpFD=(struct Cyc_Absyn_Exp**)& _tmpFC->f2;_LL5:;
if(*_tmpFD != 0 &&*_tmpFD != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp46E=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmpFD));*_tmpFD=_tmp46E;});}}
# 610
return _tmpF5;};}
# 616
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 619
struct _tuple0 _tmp100=({struct _tuple0 _tmp395;({void*_tmp470=Cyc_Tcutil_compress(old_type);_tmp395.f1=_tmp470;}),({void*_tmp46F=Cyc_Tcutil_compress(new_type);_tmp395.f2=_tmp46F;});_tmp395;});struct _tuple0 _tmp101=_tmp100;struct Cyc_Absyn_PtrInfo _tmp105;struct Cyc_Absyn_PtrInfo _tmp104;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101.f2)->tag == 3U){_LL1: _tmp105=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101.f1)->f1;_tmp104=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101.f2)->f1;_LL2: {
# 621
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp102=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=3U,(_tmp103->f1).elt_type=_tmp105.elt_type,(_tmp103->f1).elt_tq=_tmp104.elt_tq,
((_tmp103->f1).ptr_atts).rgn=(_tmp105.ptr_atts).rgn,((_tmp103->f1).ptr_atts).nullable=(_tmp104.ptr_atts).nullable,((_tmp103->f1).ptr_atts).bounds=(_tmp104.ptr_atts).bounds,((_tmp103->f1).ptr_atts).zero_term=(_tmp104.ptr_atts).zero_term,((_tmp103->f1).ptr_atts).ptrloc=(_tmp105.ptr_atts).ptrloc;_tmp103;});
# 627
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp102,new_type)&&
 Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp102);}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 636
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 639
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));({struct _tuple0*_tmp472=({struct _tuple0*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->f1=Cyc_Absyn_unique_rgn_type,({void*_tmp471=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->tag=2U,_tmp110->f1=tv;_tmp110;});_tmp111->f2=_tmp471;});_tmp111;});_tmp112->hd=_tmp472;}),_tmp112->tl=0;_tmp112;});
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 657 "tcpat.cyc"
struct _tuple0 _tmp106=({struct _tuple0 _tmp396;({void*_tmp474=Cyc_Tcutil_compress(old_type);_tmp396.f1=_tmp474;}),({void*_tmp473=Cyc_Tcutil_compress(new_type);_tmp396.f2=_tmp473;});_tmp396;});struct _tuple0 _tmp107=_tmp106;struct Cyc_Absyn_PtrInfo _tmp10B;struct Cyc_Absyn_PtrInfo _tmp10A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f2)->tag == 3U){_LL1: _tmp10B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f1)->f1;_tmp10A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f2)->f1;_LL2:
 goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp108=0U;({unsigned _tmp476=loc;struct _fat_ptr _tmp475=({const char*_tmp109="alias requires pointer type";_tag_fat(_tmp109,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp476,_tmp475,_tag_fat(_tmp108,sizeof(void*),0U));});});goto _LL0;}_LL0:;}else{
# 662
({struct Cyc_String_pa_PrintArg_struct _tmp10E=({struct Cyc_String_pa_PrintArg_struct _tmp398;_tmp398.tag=0U,({
struct _fat_ptr _tmp477=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(old_type));_tmp398.f1=_tmp477;});_tmp398;});struct Cyc_String_pa_PrintArg_struct _tmp10F=({struct Cyc_String_pa_PrintArg_struct _tmp397;_tmp397.tag=0U,({struct _fat_ptr _tmp478=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(new_type));_tmp397.f1=_tmp478;});_tmp397;});void*_tmp10C[2U];_tmp10C[0]=& _tmp10E,_tmp10C[1]=& _tmp10F;({unsigned _tmp47A=loc;struct _fat_ptr _tmp479=({const char*_tmp10D="cannot alias value of type %s to type %s";_tag_fat(_tmp10D,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp47A,_tmp479,_tag_fat(_tmp10C,sizeof(void*),2U));});});}}
# 669
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 673
void*_tmp113=p->r;void*_tmp114=_tmp113;struct Cyc_Absyn_Tvar*_tmp13C;struct Cyc_Absyn_Vardecl*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13A;struct Cyc_Absyn_Pat*_tmp139;struct Cyc_Absyn_Vardecl*_tmp138;struct Cyc_Absyn_Pat*_tmp137;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp135;union Cyc_Absyn_AggrInfo*_tmp134;struct Cyc_List_List*_tmp133;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_Pat*_tmp131;switch(*((int*)_tmp114)){case 6U: _LL1: _tmp131=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_LL2: {
# 675
void*_tmp115=(void*)_check_null(p->topt);void*_tmp116=_tmp115;void*_tmp119;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp116)->tag == 3U){_LL12: _tmp119=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp116)->f1).ptr_atts).rgn;_LL13:
# 677
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp119);
({struct Cyc_Tcenv_Tenv*_tmp47D=te;struct Cyc_Absyn_Pat*_tmp47C=_tmp131;int _tmp47B=Cyc_Tcutil_is_noalias_region(_tmp119,0);Cyc_Tcpat_check_pat_regions_rec(_tmp47D,_tmp47C,_tmp47B,patvars);});
return;}else{_LL14: _LL15:
({void*_tmp117=0U;({struct _fat_ptr _tmp47E=({const char*_tmp118="check_pat_regions: bad pointer type";_tag_fat(_tmp118,sizeof(char),36U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp47E,_tag_fat(_tmp117,sizeof(void*),0U));});});}_LL11:;}case 7U: _LL3: _tmp134=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_tmp133=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp114)->f2;_tmp132=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp114)->f3;_LL4:
# 683
 for(0;_tmp132 != 0;_tmp132=_tmp132->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple18*)_tmp132->hd)).f2,did_noalias_deref,patvars);}
return;case 8U: _LL5: _tmp135=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp114)->f3;_LL6:
# 687
 did_noalias_deref=0;_tmp136=_tmp135;goto _LL8;case 5U: _LL7: _tmp136=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_LL8:
# 689
 for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp136->hd,did_noalias_deref,patvars);}
return;case 3U: _LL9: _tmp138=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_tmp137=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp114)->f2;_LLA:
# 693
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp11A=(struct _tuple15*)x->hd;struct _tuple15*_tmp11B=_tmp11A;struct Cyc_Absyn_Vardecl**_tmp124;struct Cyc_Absyn_Exp*_tmp123;_LL17: _tmp124=_tmp11B->f1;_tmp123=_tmp11B->f2;_LL18:;
# 699
if((_tmp124 != 0 &&*_tmp124 == _tmp138)&& _tmp123 != 0){
{void*_tmp11C=_tmp123->r;void*_tmp11D=_tmp11C;struct Cyc_Absyn_Exp*_tmp122;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp11D)->tag == 15U){_LL1A: _tmp122=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp11D)->f1;_LL1B:
# 702
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp122->topt)))
({void*_tmp11E=0U;({unsigned _tmp480=p->loc;struct _fat_ptr _tmp47F=({const char*_tmp11F="reference pattern not allowed on alias-free pointers";_tag_fat(_tmp11F,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp480,_tmp47F,_tag_fat(_tmp11E,sizeof(void*),0U));});});
goto _LL19;}else{_LL1C: _LL1D:
# 706
({void*_tmp120=0U;({struct _fat_ptr _tmp481=({const char*_tmp121="check_pat_regions: bad reference access expression";_tag_fat(_tmp121,sizeof(char),51U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp481,_tag_fat(_tmp120,sizeof(void*),0U));});});}_LL19:;}
# 708
break;}}}
# 711
Cyc_Tcpat_check_pat_regions_rec(te,_tmp137,did_noalias_deref,patvars);
return;case 1U: _LLB: _tmp13A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_tmp139=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp114)->f2;_LLC:
# 714
{void*_tmp125=p->topt;void*_tmp126=_tmp125;if(_tmp126 != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp126)->tag == 4U){_LL1F: _LL20:
# 716
 if(did_noalias_deref){
({void*_tmp127=0U;({unsigned _tmp483=p->loc;struct _fat_ptr _tmp482=({const char*_tmp128="pattern to array would create alias of no-alias pointer";_tag_fat(_tmp128,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp483,_tmp482,_tag_fat(_tmp127,sizeof(void*),0U));});});
return;}
# 720
goto _LL1E;}else{goto _LL21;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 723
Cyc_Tcpat_check_pat_regions_rec(te,_tmp139,did_noalias_deref,patvars);
return;case 2U: _LLD: _tmp13C=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_tmp13B=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp114)->f2;_LLE:
# 726
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp129=(struct _tuple15*)x->hd;struct _tuple15*_tmp12A=_tmp129;struct Cyc_Absyn_Vardecl**_tmp130;struct Cyc_Absyn_Exp*_tmp12F;_LL24: _tmp130=_tmp12A->f1;_tmp12F=_tmp12A->f2;_LL25:;
# 730
if(_tmp130 != 0 &&*_tmp130 == _tmp13B){
if(_tmp12F == 0)
({void*_tmp12B=0U;({unsigned _tmp485=p->loc;struct _fat_ptr _tmp484=({const char*_tmp12C="cannot alias pattern expression in datatype";_tag_fat(_tmp12C,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp485,_tmp484,_tag_fat(_tmp12B,sizeof(void*),0U));});});else{
# 734
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp487=p->loc;struct Cyc_Tcenv_Tenv*_tmp486=te;Cyc_Tcenv_add_type_vars(_tmp487,_tmp486,({struct Cyc_Absyn_Tvar*_tmp12E[1U];_tmp12E[0]=_tmp13C;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp12E,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
te2=({struct Cyc_Tcenv_Tenv*_tmp488=te2;Cyc_Tcenv_add_region(_tmp488,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=2U,_tmp12D->f1=_tmp13C;_tmp12D;}),0);});
# 737
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp12F->topt),_tmp13C,_tmp13B->type,_tmp12F);}
# 740
break;}}}
# 743
goto _LL0;default: _LLF: _LL10:
 return;}_LL0:;}
# 759 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 761
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp13D=(struct _tuple15*)x->hd;struct _tuple15*_tmp13E=_tmp13D;struct Cyc_Absyn_Vardecl**_tmp148;struct Cyc_Absyn_Exp*_tmp147;_LL1: _tmp148=_tmp13E->f1;_tmp147=_tmp13E->f2;_LL2:;
if(_tmp147 != 0){
struct Cyc_Absyn_Exp*_tmp13F=_tmp147;
# 768
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp13F->topt))&& !Cyc_Tcutil_is_noalias_path(_tmp13F))
# 770
({struct Cyc_String_pa_PrintArg_struct _tmp142=({struct Cyc_String_pa_PrintArg_struct _tmp399;_tmp399.tag=0U,({
# 772
struct _fat_ptr _tmp48B=(struct _fat_ptr)(_tmp148 != 0?(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp145=({struct Cyc_String_pa_PrintArg_struct _tmp39A;_tmp39A.tag=0U,({
# 774
struct _fat_ptr _tmp489=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string((*_tmp148)->name));_tmp39A.f1=_tmp489;});_tmp39A;});void*_tmp143[1U];_tmp143[0]=& _tmp145;({struct _fat_ptr _tmp48A=({const char*_tmp144="for variable %s";_tag_fat(_tmp144,sizeof(char),16U);});Cyc_aprintf(_tmp48A,_tag_fat(_tmp143,sizeof(void*),1U));});}):({const char*_tmp146="";_tag_fat(_tmp146,sizeof(char),1U);}));
# 772
_tmp399.f1=_tmp48B;});_tmp399;});void*_tmp140[1U];_tmp140[0]=& _tmp142;({unsigned _tmp48D=p->loc;struct _fat_ptr _tmp48C=({const char*_tmp141="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_fat(_tmp141,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp48D,_tmp48C,_tag_fat(_tmp140,sizeof(void*),1U));});});}}};}
# 828 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1U};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2U};
# 838
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0U};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1U};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 852
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){return({union Cyc_Tcpat_Name_value _tmp39B;(_tmp39B.Name_v).tag=1U,(_tmp39B.Name_v).val=s;_tmp39B;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp39C;(_tmp39C.Int_v).tag=2U,(_tmp39C.Int_v).val=i;_tmp39C;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 869
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp149=c1->name;union Cyc_Tcpat_Name_value _tmp14A=_tmp149;int _tmp152;struct _fat_ptr _tmp151;if((_tmp14A.Name_v).tag == 1){_LL1: _tmp151=(_tmp14A.Name_v).val;_LL2: {
# 872
union Cyc_Tcpat_Name_value _tmp14B=c2->name;union Cyc_Tcpat_Name_value _tmp14C=_tmp14B;struct _fat_ptr _tmp14D;if((_tmp14C.Name_v).tag == 1){_LL6: _tmp14D=(_tmp14C.Name_v).val;_LL7:
 return Cyc_strcmp((struct _fat_ptr)_tmp151,(struct _fat_ptr)_tmp14D);}else{_LL8: _LL9:
 return - 1;}_LL5:;}}else{_LL3: _tmp152=(_tmp14A.Int_v).val;_LL4: {
# 877
union Cyc_Tcpat_Name_value _tmp14E=c2->name;union Cyc_Tcpat_Name_value _tmp14F=_tmp14E;int _tmp150;if((_tmp14F.Name_v).tag == 1){_LLB: _LLC:
 return 1;}else{_LLD: _tmp150=(_tmp14F.Int_v).val;_LLE:
 return _tmp152 - _tmp150;}_LLA:;}}_LL0:;}
# 885
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 889
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 893
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned ans=0U;
struct Cyc_List_List*_tmp153=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp153))->hd)->name)!= 0){
++ ans;
_tmp153=_tmp153->tl;}
# 900
return ans;}
# 903
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp154=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp154 != 0;_tmp154=_tmp154->tl){
struct Cyc_Absyn_Aggrfield*_tmp155=(struct Cyc_Absyn_Aggrfield*)_tmp154->hd;
if(Cyc_strcmp((struct _fat_ptr)*_tmp155->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 910
({void*_tmp156=0U;({struct _fat_ptr _tmp48F=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp159=({struct Cyc_String_pa_PrintArg_struct _tmp39D;_tmp39D.tag=0U,_tmp39D.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp39D;});void*_tmp157[1U];_tmp157[0]=& _tmp159;({struct _fat_ptr _tmp48E=({const char*_tmp158="get_member_offset %s failed";_tag_fat(_tmp158,sizeof(char),28U);});Cyc_aprintf(_tmp48E,_tag_fat(_tmp157,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp48F,_tag_fat(_tmp156,sizeof(void*),0U));});});}
# 913
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp15A=pw;struct Cyc_Absyn_Pat*_tmp189;struct Cyc_Absyn_Exp*_tmp188;if((_tmp15A.where_clause).tag == 2){_LL1: _tmp188=(_tmp15A.where_clause).val;_LL2:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B->tag=0U,_tmp15B->f1=_tmp188;_tmp15B;});}else{_LL3: _tmp189=(_tmp15A.pattern).val;_LL4: {
# 917
void*_tmp15C=_tmp189->r;void*_tmp15D=_tmp15C;union Cyc_Absyn_AggrInfo _tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Datatypedecl*_tmp185;struct Cyc_Absyn_Datatypefield*_tmp184;void*_tmp183;struct Cyc_Absyn_Enumfield*_tmp182;struct Cyc_Absyn_Enumdecl*_tmp181;struct Cyc_Absyn_Enumfield*_tmp180;struct _fat_ptr _tmp17F;int _tmp17E;char _tmp17D;enum Cyc_Absyn_Sign _tmp17C;int _tmp17B;struct Cyc_Absyn_Pat*_tmp17A;struct Cyc_Absyn_Pat*_tmp179;switch(*((int*)_tmp15D)){case 1U: _LL6: _tmp179=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL7:
 _tmp17A=_tmp179;goto _LL9;case 3U: _LL8: _tmp17A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL9:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp39E;(_tmp39E.pattern).tag=1U,(_tmp39E.pattern).val=_tmp17A;_tmp39E;}));case 9U: _LLA: _LLB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10U: _LLC: _tmp17C=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp17B=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LLD:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->tag=6U,_tmp15E->f1=(unsigned)_tmp17B;_tmp15E;});case 11U: _LLE: _tmp17D=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_LLF:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->tag=6U,_tmp15F->f1=(unsigned)_tmp17D;_tmp15F;});case 12U: _LL10: _tmp17F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp17E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL11:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->tag=5U,_tmp160->f1=_tmp17F,_tmp160->f2=_tmp17E;_tmp160;});case 13U: _LL12: _tmp181=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp180=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL13:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->tag=3U,_tmp161->f1=_tmp181,_tmp161->f2=_tmp180;_tmp161;});case 14U: _LL14: _tmp183=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp182=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL15:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->tag=4U,_tmp162->f1=_tmp183,_tmp162->f2=_tmp182;_tmp162;});case 6U: _LL16: _LL17:
# 927
{void*_tmp163=Cyc_Tcutil_compress((void*)_check_null(_tmp189->topt));void*_tmp164=_tmp163;void*_tmp165;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp164)->tag == 3U){_LL1F: _tmp165=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp164)->f1).ptr_atts).nullable;_LL20:
# 929
 if(Cyc_Tcutil_force_type2bool(0,_tmp165))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 934
({void*_tmp166=0U;({struct _fat_ptr _tmp490=({const char*_tmp167="non-null pointer type or non-pointer type in pointer pattern";_tag_fat(_tmp167,sizeof(char),61U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp490,_tag_fat(_tmp166,sizeof(void*),0U));});});case 8U: _LL18: _tmp185=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp184=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL19:
# 936
 if(_tmp185->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->tag=9U,_tmp168->f1=_tmp185,_tmp168->f2=_tmp184;_tmp168;});else{
# 939
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->tag=7U,({int _tmp491=(int)Cyc_Tcpat_datatype_tag_number(_tmp185,_tmp184->name);_tmp169->f1=_tmp491;}),_tmp169->f2=_tmp185,_tmp169->f3=_tmp184;_tmp169;});}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15D)->f1 != 0){_LL1A: _tmp187=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp186=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15D)->f3;_LL1B: {
# 941
struct Cyc_Absyn_Aggrdecl*_tmp16A=Cyc_Absyn_get_known_aggrdecl(_tmp187);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp16A->impl))->tagged){
struct _tuple18*_tmp16B=(struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp186))->hd;struct _tuple18*_tmp16C=_tmp16B;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Pat*_tmp173;_LL24: _tmp174=_tmp16C->f1;_tmp173=_tmp16C->f2;_LL25:;{
struct _fat_ptr*f;
{void*_tmp16D=(void*)((struct Cyc_List_List*)_check_null(_tmp174))->hd;void*_tmp16E=_tmp16D;struct _fat_ptr*_tmp171;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp16E)->tag == 1U){_LL27: _tmp171=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp16E)->f1;_LL28:
 f=_tmp171;goto _LL26;}else{_LL29: _LL2A:
({void*_tmp16F=0U;({struct _fat_ptr _tmp492=({const char*_tmp170="no field name in tagged union pattern";_tag_fat(_tmp170,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp492,_tag_fat(_tmp16F,sizeof(void*),0U));});});}_LL26:;}
# 949
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->tag=8U,_tmp172->f1=f,({int _tmp493=Cyc_Tcpat_get_member_offset(_tmp16A,f);_tmp172->f2=_tmp493;});_tmp172;});};}else{
# 951
({void*_tmp175=0U;({struct _fat_ptr _tmp494=({const char*_tmp176="non-tagged aggregate in pattern test";_tag_fat(_tmp176,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp494,_tag_fat(_tmp175,sizeof(void*),0U));});});}}}else{goto _LL1C;}default: _LL1C: _LL1D:
({void*_tmp177=0U;({struct _fat_ptr _tmp495=({const char*_tmp178="non-test pattern in pattern test";_tag_fat(_tmp178,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp495,_tag_fat(_tmp177,sizeof(void*),0U));});});}_LL5:;}}_LL0:;}
# 957
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp39F;(_tmp39F.pattern).tag=1U,(_tmp39F.pattern).val=p;_tmp39F;});}
# 961
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18B=_cycalloc(sizeof(*_tmp18B));({union Cyc_Tcpat_Name_value _tmp497=Cyc_Tcpat_Name_v(({const char*_tmp18A="NULL";_tag_fat(_tmp18A,sizeof(char),5U);}));_tmp18B->name=_tmp497;}),_tmp18B->arity=0,_tmp18B->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp496=Cyc_Tcpat_pw(p);_tmp18B->orig_pat=_tmp496;});_tmp18B;});}
# 964
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18D=_cycalloc(sizeof(*_tmp18D));({union Cyc_Tcpat_Name_value _tmp499=Cyc_Tcpat_Name_v(({const char*_tmp18C="&";_tag_fat(_tmp18C,sizeof(char),2U);}));_tmp18D->name=_tmp499;}),_tmp18D->arity=1,_tmp18D->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp498=Cyc_Tcpat_pw(p);_tmp18D->orig_pat=_tmp498;});_tmp18D;});}
# 967
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18F=_cycalloc(sizeof(*_tmp18F));({union Cyc_Tcpat_Name_value _tmp49B=Cyc_Tcpat_Name_v(({const char*_tmp18E="&";_tag_fat(_tmp18E,sizeof(char),2U);}));_tmp18F->name=_tmp49B;}),_tmp18F->arity=1,_tmp18F->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _tmp49A=Cyc_Tcpat_pw(p);_tmp18F->orig_pat=_tmp49A;});_tmp18F;});}
# 970
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp190=_cycalloc(sizeof(*_tmp190));({union Cyc_Tcpat_Name_value _tmp49C=Cyc_Tcpat_Int_v(i);_tmp190->name=_tmp49C;}),_tmp190->arity=0,_tmp190->span=0,_tmp190->orig_pat=p;_tmp190;});}
# 973
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp191=_cycalloc(sizeof(*_tmp191));({union Cyc_Tcpat_Name_value _tmp49E=Cyc_Tcpat_Name_v(f);_tmp191->name=_tmp49E;}),_tmp191->arity=0,_tmp191->span=0,({union Cyc_Tcpat_PatOrWhere _tmp49D=Cyc_Tcpat_pw(p);_tmp191->orig_pat=_tmp49D;});_tmp191;});}
# 976
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp192=_cycalloc(sizeof(*_tmp192));({union Cyc_Tcpat_Name_value _tmp4A0=Cyc_Tcpat_Int_v((int)c);_tmp192->name=_tmp4A0;}),_tmp192->arity=0,_tmp192->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _tmp49F=Cyc_Tcpat_pw(p);_tmp192->orig_pat=_tmp49F;});_tmp192;});}
# 979
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp194=_cycalloc(sizeof(*_tmp194));({union Cyc_Tcpat_Name_value _tmp4A1=Cyc_Tcpat_Name_v(({const char*_tmp193="$";_tag_fat(_tmp193,sizeof(char),2U);}));_tmp194->name=_tmp4A1;}),_tmp194->arity=i,_tmp194->span=& Cyc_Tcpat_one_opt,_tmp194->orig_pat=p;_tmp194;});}
# 984
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4A2=Cyc_Tcpat_null_con(p);_tmp195->f1=_tmp4A2;}),_tmp195->f2=0;_tmp195;});}
# 987
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4A3=Cyc_Tcpat_int_con(i,p);_tmp196->f1=_tmp4A3;}),_tmp196->f2=0;_tmp196;});}
# 990
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4A4=Cyc_Tcpat_char_con(c,p);_tmp197->f1=_tmp4A4;}),_tmp197->f2=0;_tmp197;});}
# 993
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4A5=Cyc_Tcpat_float_con(f,p);_tmp198->f1=_tmp4A5;}),_tmp198->f2=0;_tmp198;});}
# 996
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4A7=Cyc_Tcpat_null_ptr_con(p0);_tmp19A->f1=_tmp4A7;}),({struct Cyc_List_List*_tmp4A6=({struct Cyc_List_List*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->hd=p,_tmp199->tl=0;_tmp199;});_tmp19A->f2=_tmp4A6;});_tmp19A;});}
# 999
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4A9=Cyc_Tcpat_ptr_con(p0);_tmp19C->f1=_tmp4A9;}),({struct Cyc_List_List*_tmp4A8=({struct Cyc_List_List*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->hd=p,_tmp19B->tl=0;_tmp19B;});_tmp19C->f2=_tmp4A8;});_tmp19C;});}
# 1002
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4AB=({int _tmp4AA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp4AA,p);});_tmp19D->f1=_tmp4AB;}),_tmp19D->f2=ss;_tmp19D;});}
# 1005
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1007
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp19F=_cycalloc(sizeof(*_tmp19F));({union Cyc_Tcpat_Name_value _tmp4AE=Cyc_Tcpat_Name_v(con_name);_tmp19F->name=_tmp4AE;}),({int _tmp4AD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp19F->arity=_tmp4AD;}),_tmp19F->span=span,({union Cyc_Tcpat_PatOrWhere _tmp4AC=Cyc_Tcpat_pw(p);_tmp19F->orig_pat=_tmp4AC;});_tmp19F;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=1U,_tmp19E->f1=c,_tmp19E->f2=ps;_tmp19E;});}
# 1012
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1A0=p->r;void*_tmp1A1=_tmp1A0;void*_tmp1D8;struct Cyc_Absyn_Enumfield*_tmp1D7;struct Cyc_Absyn_Enumdecl*_tmp1D6;struct Cyc_Absyn_Enumfield*_tmp1D5;struct Cyc_Absyn_Aggrdecl*_tmp1D4;struct Cyc_List_List*_tmp1D3;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Datatypedecl*_tmp1D1;struct Cyc_Absyn_Datatypefield*_tmp1D0;struct Cyc_List_List*_tmp1CF;struct Cyc_Absyn_Pat*_tmp1CE;struct Cyc_Absyn_Pat*_tmp1CD;struct Cyc_Absyn_Pat*_tmp1CC;struct _fat_ptr _tmp1CB;char _tmp1CA;enum Cyc_Absyn_Sign _tmp1C9;int _tmp1C8;switch(*((int*)_tmp1A1)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=0U;_tmp1A2;});goto _LL0;case 9U: _LL7: _LL8:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 10U: _LL9: _tmp1C9=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_tmp1C8=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LLA:
 s=({int _tmp4AF=_tmp1C8;Cyc_Tcpat_int_pat(_tmp4AF,Cyc_Tcpat_pw(p));});goto _LL0;case 11U: _LLB: _tmp1CA=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_LLC:
 s=Cyc_Tcpat_char_pat(_tmp1CA,p);goto _LL0;case 12U: _LLD: _tmp1CB=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_LLE:
 s=Cyc_Tcpat_float_pat(_tmp1CB,p);goto _LL0;case 1U: _LLF: _tmp1CC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LL10:
 s=Cyc_Tcpat_compile_pat(_tmp1CC);goto _LL0;case 3U: _LL11: _tmp1CD=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LL12:
 s=Cyc_Tcpat_compile_pat(_tmp1CD);goto _LL0;case 6U: _LL13: _tmp1CE=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_LL14:
# 1025
{void*_tmp1A3=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1A4=_tmp1A3;void*_tmp1A7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->tag == 3U){_LL28: _tmp1A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).ptr_atts).nullable;_LL29: {
# 1027
int is_nullable=Cyc_Tcutil_force_type2bool(0,_tmp1A7);
void*ss=Cyc_Tcpat_compile_pat(_tmp1CE);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(ss,p);else{
# 1032
s=Cyc_Tcpat_ptr_pat(ss,p);}
goto _LL27;}}else{_LL2A: _LL2B:
({void*_tmp1A5=0U;({struct _fat_ptr _tmp4B0=({const char*_tmp1A6="expecting pointertype for pattern!";_tag_fat(_tmp1A6,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4B0,_tag_fat(_tmp1A5,sizeof(void*),0U));});});}_LL27:;}
# 1036
goto _LL0;case 8U: _LL15: _tmp1D1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_tmp1D0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_tmp1CF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A1)->f3;_LL16: {
# 1038
int*span;
{void*_tmp1A8=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1A9=_tmp1A8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f1)){case 18U: _LL2D: _LL2E:
# 1041
 if(_tmp1D1->is_extensible)
span=0;else{
# 1044
span=({int*_tmp1AA=_cycalloc_atomic(sizeof(*_tmp1AA));({int _tmp4B1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D1->fields))->v);*_tmp1AA=_tmp4B1;});_tmp1AA;});}
goto _LL2C;case 19U: _LL2F: _LL30:
 span=& Cyc_Tcpat_one_opt;goto _LL2C;default: goto _LL31;}else{_LL31: _LL32:
 span=({void*_tmp1AB=0U;({struct _fat_ptr _tmp4B2=({const char*_tmp1AC="void datatype pattern has bad type";_tag_fat(_tmp1AC,sizeof(char),35U);});((int*(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4B2,_tag_fat(_tmp1AB,sizeof(void*),0U));});});goto _LL2C;}_LL2C:;}
# 1049
s=({struct _fat_ptr _tmp4B5=*(*_tmp1D0->name).f2;int*_tmp4B4=span;struct Cyc_List_List*_tmp4B3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1CF);Cyc_Tcpat_con_pat(_tmp4B5,_tmp4B4,_tmp4B3,p);});
goto _LL0;}case 5U: _LL17: _tmp1D2=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_LL18:
# 1053
 s=({struct Cyc_List_List*_tmp4B6=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1D2);Cyc_Tcpat_tuple_pat(_tmp4B6,Cyc_Tcpat_pw(p));});goto _LL0;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1)->KnownAggr).tag == 2){_LL19: _tmp1D4=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1)->KnownAggr).val;_tmp1D3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A1)->f3;_LL1A:
# 1058
 if((int)_tmp1D4->kind == (int)Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1D4->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1062
int found=({struct _fat_ptr _tmp4B7=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp4B7,({const char*_tmp1B8="";_tag_fat(_tmp1B8,sizeof(char),1U);}));})== 0;
{struct Cyc_List_List*dlps0=_tmp1D3;for(0;!found && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple18*_tmp1AD=(struct _tuple18*)dlps0->hd;struct _tuple18*_tmp1AE=_tmp1AD;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_Pat*_tmp1B4;_LL34: _tmp1B5=_tmp1AE->f1;_tmp1B4=_tmp1AE->f2;_LL35:;{
struct Cyc_List_List*_tmp1AF=_tmp1B5;struct _fat_ptr*_tmp1B3;if(_tmp1AF != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp1AF)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp1AF)->tl == 0){_LL37: _tmp1B3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1AF->hd)->f1;_LL38:
# 1067
 if(Cyc_strptrcmp(_tmp1B3,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));({void*_tmp4B8=Cyc_Tcpat_compile_pat(_tmp1B4);_tmp1B0->hd=_tmp4B8;}),_tmp1B0->tl=ps;_tmp1B0;});
found=1;}
# 1071
goto _LL36;}else{goto _LL39;}}else{goto _LL39;}}else{_LL39: _LL3A:
({void*_tmp1B1=0U;({struct _fat_ptr _tmp4B9=({const char*_tmp1B2="bad designator(s)";_tag_fat(_tmp1B2,sizeof(char),18U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4B9,_tag_fat(_tmp1B1,sizeof(void*),0U));});});}_LL36:;};}}
# 1075
if(!found)
({void*_tmp1B6=0U;({struct _fat_ptr _tmp4BA=({const char*_tmp1B7="bad designator";_tag_fat(_tmp1B7,sizeof(char),15U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4BA,_tag_fat(_tmp1B6,sizeof(void*),0U));});});}}
# 1078
s=({struct Cyc_List_List*_tmp4BB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp4BB,Cyc_Tcpat_pw(p));});}else{
# 1081
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1D4->impl))->tagged)
({void*_tmp1B9=0U;({unsigned _tmp4BD=p->loc;struct _fat_ptr _tmp4BC=({const char*_tmp1BA="patterns on untagged unions not yet supported.";_tag_fat(_tmp1BA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp4BD,_tmp4BC,_tag_fat(_tmp1B9,sizeof(void*),0U));});});{
int*span=({int*_tmp1C1=_cycalloc_atomic(sizeof(*_tmp1C1));({int _tmp4BE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1D4->impl))->fields);*_tmp1C1=_tmp4BE;});_tmp1C1;});
struct Cyc_List_List*_tmp1BB=_tmp1D3;struct _fat_ptr*_tmp1C0;struct Cyc_Absyn_Pat*_tmp1BF;if(_tmp1BB != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp1BB)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp1BB)->hd)->f1)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp1BB)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp1BB)->tl == 0){_LL3C: _tmp1C0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp1BB->hd)->f1)->hd)->f1;_tmp1BF=((struct _tuple18*)_tmp1BB->hd)->f2;_LL3D:
# 1086
 s=({struct _fat_ptr _tmp4C2=*_tmp1C0;int*_tmp4C1=span;struct Cyc_List_List*_tmp4C0=({struct Cyc_List_List*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));({void*_tmp4BF=Cyc_Tcpat_compile_pat(_tmp1BF);_tmp1BC->hd=_tmp4BF;}),_tmp1BC->tl=0;_tmp1BC;});Cyc_Tcpat_con_pat(_tmp4C2,_tmp4C1,_tmp4C0,p);});
goto _LL3B;}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{_LL3E: _LL3F:
({void*_tmp1BD=0U;({struct _fat_ptr _tmp4C3=({const char*_tmp1BE="bad union pattern";_tag_fat(_tmp1BE,sizeof(char),18U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4C3,_tag_fat(_tmp1BD,sizeof(void*),0U));});});}_LL3B:;};}
# 1091
goto _LL0;}else{goto _LL23;}}else{_LL23: _LL24:
# 1119
 goto _LL26;}case 13U: _LL1B: _tmp1D6=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_tmp1D5=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LL1C:
# 1099
 s=Cyc_Tcpat_con_pat(*(*_tmp1D5->name).f2,0,0,p);
goto _LL0;case 14U: _LL1D: _tmp1D8=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_tmp1D7=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LL1E: {
# 1105
struct Cyc_List_List*fields;
{void*_tmp1C2=Cyc_Tcutil_compress(_tmp1D8);void*_tmp1C3=_tmp1C2;struct Cyc_List_List*_tmp1C6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->tag == 0U){if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f1)->tag == 16U){_LL41: _tmp1C6=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f1)->f1;_LL42:
 fields=_tmp1C6;goto _LL40;}else{goto _LL43;}}else{_LL43: _LL44:
({void*_tmp1C4=0U;({struct _fat_ptr _tmp4C4=({const char*_tmp1C5="bad type in AnonEnum_p";_tag_fat(_tmp1C5,sizeof(char),23U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4C4,_tag_fat(_tmp1C4,sizeof(void*),0U));});});}_LL40:;}
# 1115
s=Cyc_Tcpat_con_pat(*(*_tmp1D7->name).f2,0,0,p);
goto _LL0;}case 15U: _LL1F: _LL20:
 goto _LL22;case 16U: _LL21: _LL22:
 goto _LL24;default: _LL25: _LL26:
# 1120
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->tag=0U;_tmp1C7;});}_LL0:;}
# 1122
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1151
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp1D9=({struct _tuple0 _tmp3A0;_tmp3A0.f1=a1,_tmp3A0.f2=a2;_tmp3A0;});struct _tuple0 _tmp1DA=_tmp1D9;int _tmp1E4;struct _fat_ptr*_tmp1E3;int _tmp1E2;struct _fat_ptr*_tmp1E1;struct Cyc_Absyn_Datatypefield*_tmp1E0;unsigned _tmp1DF;struct Cyc_Absyn_Datatypefield*_tmp1DE;unsigned _tmp1DD;unsigned _tmp1DC;unsigned _tmp1DB;switch(*((int*)_tmp1DA.f1)){case 0U: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp1DA.f2)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LLB;}case 1U: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp1DA.f2)->tag == 1U){_LL3: _LL4:
 return 1;}else{goto _LLB;}case 2U: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1DA.f2)->tag == 2U){_LL5: _tmp1DC=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1DA.f1)->f1;_tmp1DB=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1DA.f2)->f1;_LL6:
 return _tmp1DC == _tmp1DB;}else{goto _LLB;}case 3U: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DA.f2)->tag == 3U){_LL7: _tmp1E0=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DA.f1)->f2;_tmp1DF=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DA.f1)->f3;_tmp1DE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DA.f2)->f2;_tmp1DD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DA.f2)->f3;_LL8:
# 1157
 return _tmp1E0 == _tmp1DE && _tmp1DF == _tmp1DD;}else{goto _LLB;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DA.f2)->tag == 4U){_LL9: _tmp1E4=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DA.f1)->f1;_tmp1E3=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DA.f1)->f2;_tmp1E2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DA.f2)->f1;_tmp1E1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DA.f2)->f2;_LLA:
# 1159
 return _tmp1E4 == _tmp1E2 && Cyc_strptrcmp(_tmp1E3,_tmp1E1)== 0;}else{_LLB: _LLC:
 return 0;}}_LL0:;}
# 1164
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0 && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1170
if(p1 != p2)return 0;
return 1;}
# 1174
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp1E5=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp1E6=d2;struct Cyc_List_List*_tmp1EC;struct Cyc_List_List*_tmp1EB;void*_tmp1EA;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1E6)->tag == 2U){_LL1: _tmp1EC=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1E6)->f1;_tmp1EB=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1E6)->f2;_tmp1EA=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1E6)->f3;_LL2:
# 1178
 if(Cyc_Tcpat_same_path(access,_tmp1EC)&&(int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->tag=2U,_tmp1E9->f1=_tmp1EC,({
struct Cyc_List_List*_tmp4C6=({struct Cyc_List_List*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));({struct _tuple0*_tmp4C5=({struct _tuple0*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->f1=_tmp1E5,_tmp1E7->f2=d1;_tmp1E7;});_tmp1E8->hd=_tmp4C5;}),_tmp1E8->tl=_tmp1EB;_tmp1E8;});_tmp1E9->f2=_tmp4C6;}),_tmp1E9->f3=_tmp1EA;_tmp1E9;});else{
# 1182
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1185
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->tag=2U,_tmp1EF->f1=access,({struct Cyc_List_List*_tmp4C8=({struct Cyc_List_List*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));({struct _tuple0*_tmp4C7=({struct _tuple0*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->f1=_tmp1E5,_tmp1ED->f2=d1;_tmp1ED;});_tmp1EE->hd=_tmp4C7;}),_tmp1EE->tl=0;_tmp1EE;});_tmp1EF->f2=_tmp4C8;}),_tmp1EF->f3=d2;_tmp1EF;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1196
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1199
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp1F0=0U;({struct Cyc___cycFILE*_tmp4CA=Cyc_stderr;struct _fat_ptr _tmp4C9=({const char*_tmp1F1=" ";_tag_fat(_tmp1F1,sizeof(char),2U);});Cyc_fprintf(_tmp4CA,_tmp4C9,_tag_fat(_tmp1F0,sizeof(void*),0U));});});}}
# 1205
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp1F2=c->name;
union Cyc_Tcpat_Name_value _tmp1F3=_tmp1F2;int _tmp1FB;struct _fat_ptr _tmp1FA;if((_tmp1F3.Name_v).tag == 1){_LL1: _tmp1FA=(_tmp1F3.Name_v).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp1F6=({struct Cyc_String_pa_PrintArg_struct _tmp3A1;_tmp3A1.tag=0U,_tmp3A1.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp1FA);_tmp3A1;});void*_tmp1F4[1U];_tmp1F4[0]=& _tmp1F6;({struct Cyc___cycFILE*_tmp4CC=Cyc_stderr;struct _fat_ptr _tmp4CB=({const char*_tmp1F5="%s";_tag_fat(_tmp1F5,sizeof(char),3U);});Cyc_fprintf(_tmp4CC,_tmp4CB,_tag_fat(_tmp1F4,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _tmp1FB=(_tmp1F3.Int_v).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp1F9=({struct Cyc_Int_pa_PrintArg_struct _tmp3A2;_tmp3A2.tag=1U,_tmp3A2.f1=(unsigned long)_tmp1FB;_tmp3A2;});void*_tmp1F7[1U];_tmp1F7[0]=& _tmp1F9;({struct Cyc___cycFILE*_tmp4CE=Cyc_stderr;struct _fat_ptr _tmp4CD=({const char*_tmp1F8="%d";_tag_fat(_tmp1F8,sizeof(char),3U);});Cyc_fprintf(_tmp4CE,_tmp4CD,_tag_fat(_tmp1F7,sizeof(void*),1U));});});goto _LL0;}_LL0:;}
# 1213
static void Cyc_Tcpat_print_access(void*a){
void*_tmp1FC=a;int _tmp212;struct _fat_ptr*_tmp211;struct Cyc_Absyn_Datatypefield*_tmp210;unsigned _tmp20F;unsigned _tmp20E;switch(*((int*)_tmp1FC)){case 0U: _LL1: _LL2:
({void*_tmp1FD=0U;({struct Cyc___cycFILE*_tmp4D0=Cyc_stderr;struct _fat_ptr _tmp4CF=({const char*_tmp1FE="DUMMY";_tag_fat(_tmp1FE,sizeof(char),6U);});Cyc_fprintf(_tmp4D0,_tmp4CF,_tag_fat(_tmp1FD,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp1FF=0U;({struct Cyc___cycFILE*_tmp4D2=Cyc_stderr;struct _fat_ptr _tmp4D1=({const char*_tmp200="*";_tag_fat(_tmp200,sizeof(char),2U);});Cyc_fprintf(_tmp4D2,_tmp4D1,_tag_fat(_tmp1FF,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp20E=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1FC)->f1;_LL6:
({struct Cyc_Int_pa_PrintArg_struct _tmp203=({struct Cyc_Int_pa_PrintArg_struct _tmp3A3;_tmp3A3.tag=1U,_tmp3A3.f1=(unsigned long)((int)_tmp20E);_tmp3A3;});void*_tmp201[1U];_tmp201[0]=& _tmp203;({struct Cyc___cycFILE*_tmp4D4=Cyc_stderr;struct _fat_ptr _tmp4D3=({const char*_tmp202="[%d]";_tag_fat(_tmp202,sizeof(char),5U);});Cyc_fprintf(_tmp4D4,_tmp4D3,_tag_fat(_tmp201,sizeof(void*),1U));});});goto _LL0;case 3U: _LL7: _tmp210=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1FC)->f2;_tmp20F=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1FC)->f3;_LL8:
# 1219
({struct Cyc_String_pa_PrintArg_struct _tmp206=({struct Cyc_String_pa_PrintArg_struct _tmp3A5;_tmp3A5.tag=0U,({struct _fat_ptr _tmp4D5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp210->name));_tmp3A5.f1=_tmp4D5;});_tmp3A5;});struct Cyc_Int_pa_PrintArg_struct _tmp207=({struct Cyc_Int_pa_PrintArg_struct _tmp3A4;_tmp3A4.tag=1U,_tmp3A4.f1=(unsigned long)((int)_tmp20F);_tmp3A4;});void*_tmp204[2U];_tmp204[0]=& _tmp206,_tmp204[1]=& _tmp207;({struct Cyc___cycFILE*_tmp4D7=Cyc_stderr;struct _fat_ptr _tmp4D6=({const char*_tmp205="%s[%d]";_tag_fat(_tmp205,sizeof(char),7U);});Cyc_fprintf(_tmp4D7,_tmp4D6,_tag_fat(_tmp204,sizeof(void*),2U));});});goto _LL0;default: _LL9: _tmp212=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1FC)->f1;_tmp211=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1FC)->f2;_LLA:
# 1221
 if(_tmp212)
({struct Cyc_String_pa_PrintArg_struct _tmp20A=({struct Cyc_String_pa_PrintArg_struct _tmp3A6;_tmp3A6.tag=0U,_tmp3A6.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp211);_tmp3A6;});void*_tmp208[1U];_tmp208[0]=& _tmp20A;({struct Cyc___cycFILE*_tmp4D9=Cyc_stderr;struct _fat_ptr _tmp4D8=({const char*_tmp209=".tagunion.%s";_tag_fat(_tmp209,sizeof(char),13U);});Cyc_fprintf(_tmp4D9,_tmp4D8,_tag_fat(_tmp208,sizeof(void*),1U));});});else{
# 1224
({struct Cyc_String_pa_PrintArg_struct _tmp20D=({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0U,_tmp3A7.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp211);_tmp3A7;});void*_tmp20B[1U];_tmp20B[0]=& _tmp20D;({struct Cyc___cycFILE*_tmp4DB=Cyc_stderr;struct _fat_ptr _tmp4DA=({const char*_tmp20C=".%s";_tag_fat(_tmp20C,sizeof(char),4U);});Cyc_fprintf(_tmp4DB,_tmp4DA,_tag_fat(_tmp20B,sizeof(void*),1U));});});}
goto _LL0;}_LL0:;}
# 1229
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp213=p;struct Cyc_Absyn_Datatypefield*_tmp238;struct _fat_ptr*_tmp237;int _tmp236;int _tmp235;unsigned _tmp234;struct _fat_ptr _tmp233;struct Cyc_Absyn_Enumfield*_tmp232;struct Cyc_Absyn_Enumfield*_tmp231;struct Cyc_Absyn_Exp*_tmp230;switch(*((int*)_tmp213)){case 0U: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp213)->f1 == 0){_LL1: _LL2:
({void*_tmp214=0U;({struct Cyc___cycFILE*_tmp4DD=Cyc_stderr;struct _fat_ptr _tmp4DC=({const char*_tmp215="where(NULL)";_tag_fat(_tmp215,sizeof(char),12U);});Cyc_fprintf(_tmp4DD,_tmp4DC,_tag_fat(_tmp214,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp230=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp213)->f1;_LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp218=({struct Cyc_String_pa_PrintArg_struct _tmp3A8;_tmp3A8.tag=0U,({struct _fat_ptr _tmp4DE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp230)));_tmp3A8.f1=_tmp4DE;});_tmp3A8;});void*_tmp216[1U];_tmp216[0]=& _tmp218;({struct Cyc___cycFILE*_tmp4E0=Cyc_stderr;struct _fat_ptr _tmp4DF=({const char*_tmp217="where(%s)";_tag_fat(_tmp217,sizeof(char),10U);});Cyc_fprintf(_tmp4E0,_tmp4DF,_tag_fat(_tmp216,sizeof(void*),1U));});});goto _LL0;}case 1U: _LL5: _LL6:
({void*_tmp219=0U;({struct Cyc___cycFILE*_tmp4E2=Cyc_stderr;struct _fat_ptr _tmp4E1=({const char*_tmp21A="NULL";_tag_fat(_tmp21A,sizeof(char),5U);});Cyc_fprintf(_tmp4E2,_tmp4E1,_tag_fat(_tmp219,sizeof(void*),0U));});});goto _LL0;case 2U: _LL7: _LL8:
({void*_tmp21B=0U;({struct Cyc___cycFILE*_tmp4E4=Cyc_stderr;struct _fat_ptr _tmp4E3=({const char*_tmp21C="NOT-NULL:";_tag_fat(_tmp21C,sizeof(char),10U);});Cyc_fprintf(_tmp4E4,_tmp4E3,_tag_fat(_tmp21B,sizeof(void*),0U));});});goto _LL0;case 4U: _LL9: _tmp231=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp213)->f2;_LLA:
 _tmp232=_tmp231;goto _LLC;case 3U: _LLB: _tmp232=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp213)->f2;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp21F=({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0U,({struct _fat_ptr _tmp4E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp232->name));_tmp3A9.f1=_tmp4E5;});_tmp3A9;});void*_tmp21D[1U];_tmp21D[0]=& _tmp21F;({struct Cyc___cycFILE*_tmp4E7=Cyc_stderr;struct _fat_ptr _tmp4E6=({const char*_tmp21E="%s";_tag_fat(_tmp21E,sizeof(char),3U);});Cyc_fprintf(_tmp4E7,_tmp4E6,_tag_fat(_tmp21D,sizeof(void*),1U));});});goto _LL0;case 5U: _LLD: _tmp233=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp213)->f1;_LLE:
({struct Cyc_String_pa_PrintArg_struct _tmp222=({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0U,_tmp3AA.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp233);_tmp3AA;});void*_tmp220[1U];_tmp220[0]=& _tmp222;({struct Cyc___cycFILE*_tmp4E9=Cyc_stderr;struct _fat_ptr _tmp4E8=({const char*_tmp221="%s";_tag_fat(_tmp221,sizeof(char),3U);});Cyc_fprintf(_tmp4E9,_tmp4E8,_tag_fat(_tmp220,sizeof(void*),1U));});});goto _LL0;case 6U: _LLF: _tmp234=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp213)->f1;_LL10:
({struct Cyc_Int_pa_PrintArg_struct _tmp225=({struct Cyc_Int_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=1U,_tmp3AB.f1=(unsigned long)((int)_tmp234);_tmp3AB;});void*_tmp223[1U];_tmp223[0]=& _tmp225;({struct Cyc___cycFILE*_tmp4EB=Cyc_stderr;struct _fat_ptr _tmp4EA=({const char*_tmp224="%d";_tag_fat(_tmp224,sizeof(char),3U);});Cyc_fprintf(_tmp4EB,_tmp4EA,_tag_fat(_tmp223,sizeof(void*),1U));});});goto _LL0;case 7U: _LL11: _tmp235=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp213)->f1;_LL12:
({struct Cyc_Int_pa_PrintArg_struct _tmp228=({struct Cyc_Int_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=1U,_tmp3AC.f1=(unsigned long)_tmp235;_tmp3AC;});void*_tmp226[1U];_tmp226[0]=& _tmp228;({struct Cyc___cycFILE*_tmp4ED=Cyc_stderr;struct _fat_ptr _tmp4EC=({const char*_tmp227="datatypetag(%d)";_tag_fat(_tmp227,sizeof(char),16U);});Cyc_fprintf(_tmp4ED,_tmp4EC,_tag_fat(_tmp226,sizeof(void*),1U));});});goto _LL0;case 8U: _LL13: _tmp237=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp213)->f1;_tmp236=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp213)->f2;_LL14:
({struct Cyc_String_pa_PrintArg_struct _tmp22B=({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0U,_tmp3AE.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp237);_tmp3AE;});struct Cyc_Int_pa_PrintArg_struct _tmp22C=({struct Cyc_Int_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=1U,_tmp3AD.f1=(unsigned long)_tmp236;_tmp3AD;});void*_tmp229[2U];_tmp229[0]=& _tmp22B,_tmp229[1]=& _tmp22C;({struct Cyc___cycFILE*_tmp4EF=Cyc_stderr;struct _fat_ptr _tmp4EE=({const char*_tmp22A="uniontag[%s](%d)";_tag_fat(_tmp22A,sizeof(char),17U);});Cyc_fprintf(_tmp4EF,_tmp4EE,_tag_fat(_tmp229,sizeof(void*),2U));});});goto _LL0;default: _LL15: _tmp238=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp213)->f2;_LL16:
# 1242
({struct Cyc_String_pa_PrintArg_struct _tmp22F=({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0U,({struct _fat_ptr _tmp4F0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp238->name));_tmp3AF.f1=_tmp4F0;});_tmp3AF;});void*_tmp22D[1U];_tmp22D[0]=& _tmp22F;({struct Cyc___cycFILE*_tmp4F2=Cyc_stderr;struct _fat_ptr _tmp4F1=({const char*_tmp22E="datatypefield(%s)";_tag_fat(_tmp22E,sizeof(char),18U);});Cyc_fprintf(_tmp4F2,_tmp4F1,_tag_fat(_tmp22D,sizeof(void*),1U));});});}_LL0:;}
# 1246
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp23B=({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0U,({struct _fat_ptr _tmp4F3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp3B0.f1=_tmp4F3;});_tmp3B0;});void*_tmp239[1U];_tmp239[0]=& _tmp23B;({struct Cyc___cycFILE*_tmp4F5=Cyc_stderr;struct _fat_ptr _tmp4F4=({const char*_tmp23A="%s";_tag_fat(_tmp23A,sizeof(char),3U);});Cyc_fprintf(_tmp4F5,_tmp4F4,_tag_fat(_tmp239,sizeof(void*),1U));});});}
# 1250
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp23C=d;struct Cyc_List_List*_tmp258;struct Cyc_List_List*_tmp257;void*_tmp256;struct Cyc_Tcpat_Rhs*_tmp255;switch(*((int*)_tmp23C)){case 1U: _LL1: _tmp255=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp23C)->f1;_LL2:
# 1253
 Cyc_Tcpat_print_tab(tab);
({void*_tmp23D=0U;({struct Cyc___cycFILE*_tmp4F7=Cyc_stderr;struct _fat_ptr _tmp4F6=({const char*_tmp23E="Success(";_tag_fat(_tmp23E,sizeof(char),9U);});Cyc_fprintf(_tmp4F7,_tmp4F6,_tag_fat(_tmp23D,sizeof(void*),0U));});});Cyc_Tcpat_print_rhs(_tmp255);({void*_tmp23F=0U;({struct Cyc___cycFILE*_tmp4F9=Cyc_stderr;struct _fat_ptr _tmp4F8=({const char*_tmp240=")\n";_tag_fat(_tmp240,sizeof(char),3U);});Cyc_fprintf(_tmp4F9,_tmp4F8,_tag_fat(_tmp23F,sizeof(void*),0U));});});
goto _LL0;case 0U: _LL3: _LL4:
({void*_tmp241=0U;({struct Cyc___cycFILE*_tmp4FB=Cyc_stderr;struct _fat_ptr _tmp4FA=({const char*_tmp242="Failure\n";_tag_fat(_tmp242,sizeof(char),9U);});Cyc_fprintf(_tmp4FB,_tmp4FA,_tag_fat(_tmp241,sizeof(void*),0U));});});goto _LL0;default: _LL5: _tmp258=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23C)->f1;_tmp257=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23C)->f2;_tmp256=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23C)->f3;_LL6:
# 1258
 Cyc_Tcpat_print_tab(tab);
({void*_tmp243=0U;({struct Cyc___cycFILE*_tmp4FD=Cyc_stderr;struct _fat_ptr _tmp4FC=({const char*_tmp244="Switch[";_tag_fat(_tmp244,sizeof(char),8U);});Cyc_fprintf(_tmp4FD,_tmp4FC,_tag_fat(_tmp243,sizeof(void*),0U));});});
for(0;_tmp258 != 0;_tmp258=_tmp258->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp258->hd)->access);
if(_tmp258->tl != 0)({void*_tmp245=0U;({struct Cyc___cycFILE*_tmp4FF=Cyc_stderr;struct _fat_ptr _tmp4FE=({const char*_tmp246=",";_tag_fat(_tmp246,sizeof(char),2U);});Cyc_fprintf(_tmp4FF,_tmp4FE,_tag_fat(_tmp245,sizeof(void*),0U));});});}
# 1264
({void*_tmp247=0U;({struct Cyc___cycFILE*_tmp501=Cyc_stderr;struct _fat_ptr _tmp500=({const char*_tmp248="] {\n";_tag_fat(_tmp248,sizeof(char),5U);});Cyc_fprintf(_tmp501,_tmp500,_tag_fat(_tmp247,sizeof(void*),0U));});});
for(0;_tmp257 != 0;_tmp257=_tmp257->tl){
struct _tuple0 _tmp249=*((struct _tuple0*)_tmp257->hd);struct _tuple0 _tmp24A=_tmp249;void*_tmp250;void*_tmp24F;_LL8: _tmp250=_tmp24A.f1;_tmp24F=_tmp24A.f2;_LL9:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp24B=0U;({struct Cyc___cycFILE*_tmp503=Cyc_stderr;struct _fat_ptr _tmp502=({const char*_tmp24C="case ";_tag_fat(_tmp24C,sizeof(char),6U);});Cyc_fprintf(_tmp503,_tmp502,_tag_fat(_tmp24B,sizeof(void*),0U));});});
Cyc_Tcpat_print_pat_test(_tmp250);
({void*_tmp24D=0U;({struct Cyc___cycFILE*_tmp505=Cyc_stderr;struct _fat_ptr _tmp504=({const char*_tmp24E=":\n";_tag_fat(_tmp24E,sizeof(char),3U);});Cyc_fprintf(_tmp505,_tmp504,_tag_fat(_tmp24D,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(_tmp24F,tab + 7);}
# 1273
Cyc_Tcpat_print_tab(tab);
({void*_tmp251=0U;({struct Cyc___cycFILE*_tmp507=Cyc_stderr;struct _fat_ptr _tmp506=({const char*_tmp252="default:\n";_tag_fat(_tmp252,sizeof(char),10U);});Cyc_fprintf(_tmp507,_tmp506,_tag_fat(_tmp251,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(_tmp256,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp253=0U;({struct Cyc___cycFILE*_tmp509=Cyc_stderr;struct _fat_ptr _tmp508=({const char*_tmp254="}\n";_tag_fat(_tmp254,sizeof(char),3U);});Cyc_fprintf(_tmp509,_tmp508,_tag_fat(_tmp253,sizeof(void*),0U));});});}_LL0:;}
# 1281
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1289
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp259=td;struct Cyc_Set_Set*_tmp25D;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp259)->tag == 1U){_LL1: _tmp25D=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp259)->f1;_LL2:
# 1299
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->tag=1U,({struct Cyc_Set_Set*_tmp50A=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp25D,c);_tmp25A->f1=_tmp50A;});_tmp25A;});}else{_LL3: _LL4:
({void*_tmp25B=0U;({struct _fat_ptr _tmp50B=({const char*_tmp25C="add_neg called when td is Positive";_tag_fat(_tmp25C,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp50B,_tag_fat(_tmp25B,sizeof(void*),0U));});});}_LL0:;}
# 1306
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp25E=td;struct Cyc_Set_Set*_tmp260;struct Cyc_Tcpat_Con_s*_tmp25F;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp25E)->tag == 0U){_LL1: _tmp25F=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp25E)->f1;_LL2:
# 1310
 if(Cyc_Tcpat_compare_con(c,_tmp25F)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL3: _tmp260=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp25E)->f1;_LL4:
# 1314
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp260,c))return Cyc_Tcpat_No;else{
# 1317
if(c->span != 0 &&({int _tmp50C=*((int*)_check_null(c->span));_tmp50C == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp260)+ 1;}))
return Cyc_Tcpat_Yes;else{
# 1320
return Cyc_Tcpat_Maybe;}}}_LL0:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1328
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp261=ctxt;struct Cyc_Tcpat_Con_s*_tmp267;struct Cyc_List_List*_tmp266;struct Cyc_List_List*_tmp265;if(_tmp261 == 0){_LL1: _LL2:
 return 0;}else{_LL3: _tmp267=((struct _tuple21*)_tmp261->hd)->f1;_tmp266=((struct _tuple21*)_tmp261->hd)->f2;_tmp265=_tmp261->tl;_LL4:
# 1332
 return({struct Cyc_List_List*_tmp264=_cycalloc(sizeof(*_tmp264));
({struct _tuple21*_tmp50E=({struct _tuple21*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->f1=_tmp267,({struct Cyc_List_List*_tmp50D=({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->hd=dsc,_tmp262->tl=_tmp266;_tmp262;});_tmp263->f2=_tmp50D;});_tmp263;});_tmp264->hd=_tmp50E;}),_tmp264->tl=_tmp265;_tmp264;});}_LL0:;}
# 1340
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp268=ctxt;struct Cyc_Tcpat_Con_s*_tmp26E;struct Cyc_List_List*_tmp26D;struct Cyc_List_List*_tmp26C;if(_tmp268 == 0){_LL1: _LL2:
({void*_tmp269=0U;({struct _fat_ptr _tmp50F=({const char*_tmp26A="norm_context: empty context";_tag_fat(_tmp26A,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp50F,_tag_fat(_tmp269,sizeof(void*),0U));});});}else{_LL3: _tmp26E=((struct _tuple21*)_tmp268->hd)->f1;_tmp26D=((struct _tuple21*)_tmp268->hd)->f2;_tmp26C=_tmp268->tl;_LL4:
# 1344
 return({struct Cyc_List_List*_tmp511=_tmp26C;Cyc_Tcpat_augment(_tmp511,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->tag=0U,_tmp26B->f1=_tmp26E,({struct Cyc_List_List*_tmp510=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp26D);_tmp26B->f2=_tmp510;});_tmp26B;}));});}_LL0:;}
# 1353
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1355
struct _tuple1 _tmp26F=({struct _tuple1 _tmp3B1;_tmp3B1.f1=ctxt,_tmp3B1.f2=work;_tmp3B1;});struct _tuple1 _tmp270=_tmp26F;struct Cyc_Tcpat_Con_s*_tmp27A;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp278;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp276;if(_tmp270.f1 == 0){if(_tmp270.f2 == 0){_LL1: _LL2:
 return dsc;}else{_LL3: _LL4:
 goto _LL6;}}else{if(_tmp270.f2 == 0){_LL5: _LL6:
({void*_tmp271=0U;({struct _fat_ptr _tmp512=({const char*_tmp272="build_desc: ctxt and work don't match";_tag_fat(_tmp272,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp512,_tag_fat(_tmp271,sizeof(void*),0U));});});}else{_LL7: _tmp27A=((struct _tuple21*)(_tmp270.f1)->hd)->f1;_tmp279=((struct _tuple21*)(_tmp270.f1)->hd)->f2;_tmp278=(_tmp270.f1)->tl;_tmp277=((struct _tuple20*)(_tmp270.f2)->hd)->f3;_tmp276=(_tmp270.f2)->tl;_LL8: {
# 1360
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp273=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275->tag=0U,_tmp275->f1=_tmp27A,({struct Cyc_List_List*_tmp514=({struct Cyc_List_List*_tmp513=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp279);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp513,({struct Cyc_List_List*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->hd=dsc,_tmp274->tl=_tmp277;_tmp274;}));});_tmp275->f2=_tmp514;});_tmp275;});
return Cyc_Tcpat_build_desc(_tmp278,(void*)_tmp273,_tmp276);}}}_LL0:;}
# 1365
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1372
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp27B=allmrules;void*_tmp27F;struct Cyc_Tcpat_Rhs*_tmp27E;struct Cyc_List_List*_tmp27D;if(_tmp27B == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->tag=0U,_tmp27C->f1=dsc;_tmp27C;});}else{_LL3: _tmp27F=((struct _tuple22*)_tmp27B->hd)->f1;_tmp27E=((struct _tuple22*)_tmp27B->hd)->f2;_tmp27D=_tmp27B->tl;_LL4:
# 1376
 return Cyc_Tcpat_match(_tmp27F,0,dsc,0,0,_tmp27E,_tmp27D);}_LL0:;}
# 1381
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp516=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280->tag=1U,({struct Cyc_Set_Set*_tmp515=Cyc_Tcpat_empty_con_set();_tmp280->f1=_tmp515;});_tmp280;});Cyc_Tcpat_or_match(_tmp516,allmrules);});}
# 1388
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1391
struct Cyc_List_List*_tmp281=work;struct Cyc_List_List*_tmp298;struct Cyc_List_List*_tmp297;struct Cyc_List_List*_tmp296;struct Cyc_List_List*_tmp295;struct Cyc_List_List*_tmp294;if(_tmp281 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->tag=1U,_tmp282->f1=right_hand_side;_tmp282;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp281)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp281)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp281)->hd)->f3 == 0){_LL3: _tmp294=_tmp281->tl;_LL4:
# 1394
 return({struct Cyc_List_List*_tmp519=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp518=_tmp294;struct Cyc_Tcpat_Rhs*_tmp517=right_hand_side;Cyc_Tcpat_and_match(_tmp519,_tmp518,_tmp517,rules);});}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _tmp298=((struct _tuple20*)_tmp281->hd)->f1;_tmp297=((struct _tuple20*)_tmp281->hd)->f2;_tmp296=((struct _tuple20*)_tmp281->hd)->f3;_tmp295=_tmp281->tl;_LL6:
# 1396
 if((_tmp298 == 0 || _tmp297 == 0)|| _tmp296 == 0)
({void*_tmp283=0U;({struct _fat_ptr _tmp51A=({const char*_tmp284="tcpat:and_match: malformed work frame";_tag_fat(_tmp284,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp51A,_tag_fat(_tmp283,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp285=_tmp298;struct Cyc_List_List*_tmp286=_tmp285;void*_tmp293;struct Cyc_List_List*_tmp292;_LL8: _tmp293=(void*)_tmp286->hd;_tmp292=_tmp286->tl;_LL9:;{
struct Cyc_List_List*_tmp287=_tmp297;struct Cyc_List_List*_tmp288=_tmp287;struct Cyc_List_List*_tmp291;struct Cyc_List_List*_tmp290;_LLB: _tmp291=(struct Cyc_List_List*)_tmp288->hd;_tmp290=_tmp288->tl;_LLC:;{
struct Cyc_List_List*_tmp289=_tmp296;struct Cyc_List_List*_tmp28A=_tmp289;void*_tmp28F;struct Cyc_List_List*_tmp28E;_LLE: _tmp28F=(void*)_tmp28A->hd;_tmp28E=_tmp28A->tl;_LLF:;{
struct _tuple20*_tmp28B=({struct _tuple20*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D->f1=_tmp292,_tmp28D->f2=_tmp290,_tmp28D->f3=_tmp28E;_tmp28D;});
return({void*_tmp520=_tmp293;struct Cyc_List_List*_tmp51F=_tmp291;void*_tmp51E=_tmp28F;struct Cyc_List_List*_tmp51D=ctx;struct Cyc_List_List*_tmp51C=({struct Cyc_List_List*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->hd=_tmp28B,_tmp28C->tl=_tmp295;_tmp28C;});struct Cyc_Tcpat_Rhs*_tmp51B=right_hand_side;Cyc_Tcpat_match(_tmp520,_tmp51F,_tmp51E,_tmp51D,_tmp51C,_tmp51B,rules);});};};};};}}_LL0:;}
# 1407
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp299=dsc;struct Cyc_List_List*_tmp29E;struct Cyc_Set_Set*_tmp29D;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp299)->tag == 1U){_LL1: _tmp29D=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp299)->f1;_LL2: {
# 1413
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->tag=1U,({struct Cyc_Set_Set*_tmp521=Cyc_Tcpat_empty_con_set();_tmp29C->f1=_tmp521;});_tmp29C;});
struct Cyc_List_List*_tmp29A=0;
{int i=0;for(0;i < pcon->arity;++ i){
_tmp29A=({struct Cyc_List_List*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->hd=any,_tmp29B->tl=_tmp29A;_tmp29B;});}}
return _tmp29A;}}else{_LL3: _tmp29E=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp299)->f2;_LL4:
 return _tmp29E;}_LL0:;}
# 1422
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp29F=pw;struct Cyc_Absyn_Pat*_tmp2BC;if((_tmp29F.where_clause).tag == 2){_LL1: _LL2:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL3: _tmp2BC=(_tmp29F.pattern).val;_LL4: {
# 1426
void*_tmp2A0=_tmp2BC->r;void*_tmp2A1=_tmp2A0;union Cyc_Absyn_AggrInfo _tmp2BB;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_Datatypedecl*_tmp2B9;struct Cyc_Absyn_Datatypefield*_tmp2B8;switch(*((int*)_tmp2A1)){case 6U: _LL6: _LL7:
# 1428
 if(i != 0)
({void*_tmp2A2=0U;({struct _fat_ptr _tmp523=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2A5=({struct Cyc_Int_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=1U,_tmp3B2.f1=(unsigned long)i;_tmp3B2;});void*_tmp2A3[1U];_tmp2A3[0]=& _tmp2A5;({struct _fat_ptr _tmp522=({const char*_tmp2A4="get_access on pointer pattern with offset %d\n";_tag_fat(_tmp2A4,sizeof(char),46U);});Cyc_aprintf(_tmp522,_tag_fat(_tmp2A3,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp523,_tag_fat(_tmp2A2,sizeof(void*),0U));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5U: _LL8: _LL9:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6->tag=2U,_tmp2A6->f1=(unsigned)i;_tmp2A6;});case 8U: _LLA: _tmp2B9=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2A1)->f1;_tmp2B8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2A1)->f2;_LLB:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->tag=3U,_tmp2A7->f1=_tmp2B9,_tmp2A7->f2=_tmp2B8,_tmp2A7->f3=(unsigned)i;_tmp2A7;});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A1)->f1 != 0){_LLC: _tmp2BB=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A1)->f1;_tmp2BA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A1)->f3;_LLD: {
# 1434
struct Cyc_Absyn_Aggrdecl*_tmp2A8=Cyc_Absyn_get_known_aggrdecl(_tmp2BB);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A8->impl))->tagged){
struct Cyc_List_List*_tmp2A9=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp2BA))->hd)).f1;struct Cyc_List_List*_tmp2AA=_tmp2A9;struct _fat_ptr*_tmp2B0;if(_tmp2AA != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp2AA)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp2AA)->tl == 0){_LL11: _tmp2B0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2AA->hd)->f1;_LL12:
# 1438
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->tag=4U,_tmp2AB->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A8->impl))->tagged,_tmp2AB->f2=_tmp2B0;_tmp2AB;});}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _LL14:
({void*_tmp2AC=0U;({struct _fat_ptr _tmp526=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2AF=({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0U,({struct _fat_ptr _tmp524=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(_tmp2BC));_tmp3B3.f1=_tmp524;});_tmp3B3;});void*_tmp2AD[1U];_tmp2AD[0]=& _tmp2AF;({struct _fat_ptr _tmp525=({const char*_tmp2AE="get_access on bad aggr pattern: %s";_tag_fat(_tmp2AE,sizeof(char),35U);});Cyc_aprintf(_tmp525,_tag_fat(_tmp2AD,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp526,_tag_fat(_tmp2AC,sizeof(void*),0U));});});}_LL10:;}{
# 1442
struct Cyc_List_List*_tmp2B1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A8->impl))->fields;
int _tmp2B2=i;
for(0;i != 0;-- i){_tmp2B1=((struct Cyc_List_List*)_check_null(_tmp2B1))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->tag=4U,_tmp2B3->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A8->impl))->tagged,_tmp2B3->f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2B1))->hd)->name;_tmp2B3;});};}}else{goto _LLE;}default: _LLE: _LLF:
({void*_tmp2B4=0U;({struct _fat_ptr _tmp529=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B7=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0U,({struct _fat_ptr _tmp527=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(_tmp2BC));_tmp3B4.f1=_tmp527;});_tmp3B4;});void*_tmp2B5[1U];_tmp2B5[0]=& _tmp2B7;({struct _fat_ptr _tmp528=({const char*_tmp2B6="get_access on bad pattern: %s";_tag_fat(_tmp2B6,sizeof(char),30U);});Cyc_aprintf(_tmp528,_tag_fat(_tmp2B5,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp529,_tag_fat(_tmp2B4,sizeof(void*),0U));});});}_LL5:;}}_LL0:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1452
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp2BD=env;struct Cyc_List_List*_tmp2C1;struct Cyc_Tcpat_Con_s*_tmp2C0;_LL1: _tmp2C1=_tmp2BD->f1;_tmp2C0=_tmp2BD->f2;_LL2:;{
void*acc=Cyc_Tcpat_get_access(_tmp2C0->orig_pat,i);
return({struct Cyc_List_List*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));({struct Cyc_Tcpat_PathNode*_tmp52A=({struct Cyc_Tcpat_PathNode*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->orig_pat=_tmp2C0->orig_pat,_tmp2BE->access=acc;_tmp2BE;});_tmp2BF->hd=_tmp52A;}),_tmp2BF->tl=_tmp2C1;_tmp2BF;});};}
# 1457
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp2C2=({struct _tuple23 _tmp3B5;_tmp3B5.f1=obj,_tmp3B5.f2=pcon;_tmp3B5;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp2C2);}
# 1465
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1469
void*_tmp2C3=p;struct Cyc_Tcpat_Con_s*_tmp2D1;struct Cyc_List_List*_tmp2D0;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2C3)->tag == 0U){_LL1: _LL2:
# 1471
 return({struct Cyc_List_List*_tmp52D=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp52C=work;struct Cyc_Tcpat_Rhs*_tmp52B=right_hand_side;Cyc_Tcpat_and_match(_tmp52D,_tmp52C,_tmp52B,rules);});}else{_LL3: _tmp2D1=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2C3)->f1;_tmp2D0=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2C3)->f2;_LL4: {
# 1473
enum Cyc_Tcpat_Answer _tmp2C4=Cyc_Tcpat_static_match(_tmp2D1,dsc);enum Cyc_Tcpat_Answer _tmp2C5=_tmp2C4;switch(_tmp2C5){case Cyc_Tcpat_Yes: _LL6: _LL7: {
# 1475
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));({struct _tuple21*_tmp52E=({struct _tuple21*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->f1=_tmp2D1,_tmp2C8->f2=0;_tmp2C8;});_tmp2C9->hd=_tmp52E;}),_tmp2C9->tl=ctx;_tmp2C9;});
struct _tuple20*work_frame=({struct _tuple20*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7->f1=_tmp2D0,({struct Cyc_List_List*_tmp530=Cyc_Tcpat_getoargs(_tmp2D1,obj);_tmp2C7->f2=_tmp530;}),({
struct Cyc_List_List*_tmp52F=Cyc_Tcpat_getdargs(_tmp2D1,dsc);_tmp2C7->f3=_tmp52F;});_tmp2C7;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6->hd=work_frame,_tmp2C6->tl=work;_tmp2C6;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL8: _LL9:
# 1481
 return({void*_tmp531=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp531,rules);});default: _LLA: _LLB: {
# 1483
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));({struct _tuple21*_tmp532=({struct _tuple21*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->f1=_tmp2D1,_tmp2CE->f2=0;_tmp2CE;});_tmp2CF->hd=_tmp532;}),_tmp2CF->tl=ctx;_tmp2CF;});
struct _tuple20*work_frame=({struct _tuple20*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->f1=_tmp2D0,({struct Cyc_List_List*_tmp534=Cyc_Tcpat_getoargs(_tmp2D1,obj);_tmp2CD->f2=_tmp534;}),({
struct Cyc_List_List*_tmp533=Cyc_Tcpat_getdargs(_tmp2D1,dsc);_tmp2CD->f3=_tmp533;});_tmp2CD;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->hd=work_frame,_tmp2CC->tl=work;_tmp2CC;});
void*_tmp2CA=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp2CB=({void*_tmp537=({struct Cyc_List_List*_tmp536=ctx;void*_tmp535=Cyc_Tcpat_add_neg(dsc,_tmp2D1);Cyc_Tcpat_build_desc(_tmp536,_tmp535,work);});Cyc_Tcpat_or_match(_tmp537,rules);});
# 1490
return Cyc_Tcpat_ifeq(obj,_tmp2D1,_tmp2CA,_tmp2CB);}}_LL5:;}}_LL0:;}
# 1500
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1505
void*_tmp2D2=d;struct Cyc_List_List*_tmp2D9;void*_tmp2D8;struct Cyc_Tcpat_Rhs*_tmp2D7;void*_tmp2D6;switch(*((int*)_tmp2D2)){case 0U: _LL1: _tmp2D6=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2D2)->f1;_LL2:
# 1507
 if(!(*exhaust_done)){not_exhaust(env1,_tmp2D6);*exhaust_done=1;}
goto _LL0;case 1U: _LL3: _tmp2D7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2D2)->f1;_LL4:
 rhs_appears(env2,_tmp2D7);goto _LL0;default: _LL5: _tmp2D9=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D2)->f2;_tmp2D8=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D2)->f3;_LL6:
# 1511
 for(0;_tmp2D9 != 0;_tmp2D9=_tmp2D9->tl){
struct _tuple0 _tmp2D3=*((struct _tuple0*)_tmp2D9->hd);struct _tuple0 _tmp2D4=_tmp2D3;void*_tmp2D5;_LL8: _tmp2D5=_tmp2D4.f2;_LL9:;
Cyc_Tcpat_check_exhaust_overlap(_tmp2D5,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1516
Cyc_Tcpat_check_exhaust_overlap(_tmp2D8,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1518
goto _LL0;}_LL0:;}
# 1526
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1528
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));_tmp2DF->used=0,_tmp2DF->pat_loc=(swc->pattern)->loc,_tmp2DF->rhs=swc->body;_tmp2DF;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp2DA=({union Cyc_Tcpat_PatOrWhere _tmp3B6;(_tmp3B6.where_clause).tag=2U,(_tmp3B6.where_clause).val=swc->where_clause;_tmp3B6;});
sp=({struct Cyc_List_List*_tmp539=({void*_tmp2DB[2U];_tmp2DB[0]=sp0,({void*_tmp538=Cyc_Tcpat_int_pat(*ctr,_tmp2DA);_tmp2DB[1]=_tmp538;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2DB,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp539,_tmp2DA);});
*ctr=*ctr + 1;}else{
# 1536
sp=({struct Cyc_List_List*_tmp53B=({void*_tmp2DC[2U];_tmp2DC[0]=sp0,({void*_tmp53A=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->tag=0U;_tmp2DD;});_tmp2DC[1]=_tmp53A;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2DC,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp53B,({union Cyc_Tcpat_PatOrWhere _tmp3B7;(_tmp3B7.where_clause).tag=2U,(_tmp3B7.where_clause).val=0;_tmp3B7;}));});}
return({struct _tuple22*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));_tmp2DE->f1=sp,_tmp2DE->f2=rhs;_tmp2DE;});}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1542
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1544
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp2E0=d;struct Cyc_Set_Set*_tmp341;struct Cyc_Tcpat_Con_s*_tmp340;struct Cyc_List_List*_tmp33F;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E0)->tag == 0U){_LL1: _tmp340=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E0)->f1;_tmp33F=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E0)->f2;_LL2: {
# 1548
union Cyc_Tcpat_Name_value _tmp2E1=_tmp340->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp2E2=_tmp340->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp2E3=_tmp2E2;struct Cyc_Absyn_Pat*_tmp2E4;if((_tmp2E3.where_clause).tag == 2){_LL6: _LL7:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp33F))->hd);}else{_LL8: _tmp2E4=(_tmp2E3.pattern).val;_LL9:
 p=_tmp2E4;goto _LL5;}_LL5:;}{
# 1554
void*_tmp2E5=p->r;void*_tmp2E6=_tmp2E5;struct Cyc_Absyn_Exp*_tmp31B;struct Cyc_Absyn_Enumfield*_tmp31A;struct Cyc_Absyn_Enumfield*_tmp319;struct _fat_ptr _tmp318;int _tmp317;char _tmp316;int _tmp315;struct Cyc_Absyn_Datatypefield*_tmp314;struct Cyc_Absyn_Aggrdecl*_tmp313;struct Cyc_List_List*_tmp312;struct Cyc_Absyn_Tvar*_tmp311;struct Cyc_Absyn_Vardecl*_tmp310;struct Cyc_Absyn_Vardecl*_tmp30F;struct Cyc_Absyn_Vardecl*_tmp30E;switch(*((int*)_tmp2E6)){case 0U: _LLB: _LLC:
 return({const char*_tmp2E7="_";_tag_fat(_tmp2E7,sizeof(char),2U);});case 1U: _LLD: _tmp30E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1;_LLE:
 return Cyc_Absynpp_qvar2string(_tmp30E->name);case 3U: _LLF: _tmp30F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1;_LL10:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EA=({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0U,({struct _fat_ptr _tmp53C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp30F->name));_tmp3B8.f1=_tmp53C;});_tmp3B8;});void*_tmp2E8[1U];_tmp2E8[0]=& _tmp2EA;({struct _fat_ptr _tmp53D=({const char*_tmp2E9="*%s";_tag_fat(_tmp2E9,sizeof(char),4U);});Cyc_aprintf(_tmp53D,_tag_fat(_tmp2E8,sizeof(void*),1U));});});case 4U: _LL11: _tmp311=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1;_tmp310=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2E6)->f2;_LL12:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2ED=({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0U,({struct _fat_ptr _tmp53E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp310->name));_tmp3BA.f1=_tmp53E;});_tmp3BA;});struct Cyc_String_pa_PrintArg_struct _tmp2EE=({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0U,_tmp3B9.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp311->name);_tmp3B9;});void*_tmp2EB[2U];_tmp2EB[0]=& _tmp2ED,_tmp2EB[1]=& _tmp2EE;({struct _fat_ptr _tmp53F=({const char*_tmp2EC="%s<`%s>";_tag_fat(_tmp2EC,sizeof(char),8U);});Cyc_aprintf(_tmp53F,_tag_fat(_tmp2EB,sizeof(void*),2U));});});case 5U: _LL13: _LL14:
# 1560
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F1=({struct Cyc_String_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=0U,({struct _fat_ptr _tmp540=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp33F));_tmp3BB.f1=_tmp540;});_tmp3BB;});void*_tmp2EF[1U];_tmp2EF[0]=& _tmp2F1;({struct _fat_ptr _tmp541=({const char*_tmp2F0="$(%s)";_tag_fat(_tmp2F0,sizeof(char),6U);});Cyc_aprintf(_tmp541,_tag_fat(_tmp2EF,sizeof(void*),1U));});});case 6U: _LL15: _LL16:
# 1562
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F4=({struct Cyc_String_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=0U,({struct _fat_ptr _tmp542=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp33F));_tmp3BC.f1=_tmp542;});_tmp3BC;});void*_tmp2F2[1U];_tmp2F2[0]=& _tmp2F4;({struct _fat_ptr _tmp543=({const char*_tmp2F3="&%s";_tag_fat(_tmp2F3,sizeof(char),4U);});Cyc_aprintf(_tmp543,_tag_fat(_tmp2F2,sizeof(void*),1U));});});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1)->KnownAggr).tag == 2){_LL17: _tmp313=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1)->KnownAggr).val;_tmp312=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2E6)->f3;_LL18:
# 1564
 if((int)_tmp313->kind == (int)Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2F5=_tmp312;struct _fat_ptr*_tmp2FB;if(_tmp2F5 != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp2F5)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp2F5)->hd)->f1)->hd)->tag == 1U){_LL2C: _tmp2FB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp2F5->hd)->f1)->hd)->f1;_LL2D:
# 1567
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F8=({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0U,({struct _fat_ptr _tmp544=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp313->name));_tmp3BF.f1=_tmp544;});_tmp3BF;});struct Cyc_String_pa_PrintArg_struct _tmp2F9=({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0U,_tmp3BE.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp2FB);_tmp3BE;});struct Cyc_String_pa_PrintArg_struct _tmp2FA=({struct Cyc_String_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=0U,({
struct _fat_ptr _tmp545=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp33F));_tmp3BD.f1=_tmp545;});_tmp3BD;});void*_tmp2F6[3U];_tmp2F6[0]=& _tmp2F8,_tmp2F6[1]=& _tmp2F9,_tmp2F6[2]=& _tmp2FA;({struct _fat_ptr _tmp546=({const char*_tmp2F7="%s{.%s = %s}";_tag_fat(_tmp2F7,sizeof(char),13U);});Cyc_aprintf(_tmp546,_tag_fat(_tmp2F6,sizeof(void*),3U));});});}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 1572
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp3C1;_tmp3C1.tag=0U,({struct _fat_ptr _tmp547=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp313->name));_tmp3C1.f1=_tmp547;});_tmp3C1;});struct Cyc_String_pa_PrintArg_struct _tmp2FF=({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0U,({struct _fat_ptr _tmp548=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp33F));_tmp3C0.f1=_tmp548;});_tmp3C0;});void*_tmp2FC[2U];_tmp2FC[0]=& _tmp2FE,_tmp2FC[1]=& _tmp2FF;({struct _fat_ptr _tmp549=({const char*_tmp2FD="%s{%s}";_tag_fat(_tmp2FD,sizeof(char),7U);});Cyc_aprintf(_tmp549,_tag_fat(_tmp2FC,sizeof(void*),2U));});});}else{goto _LL29;}}else{goto _LL29;}case 8U: _LL19: _tmp314=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2E6)->f2;_LL1A:
# 1574
 if(_tmp33F == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp302=({struct Cyc_String_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=0U,({struct _fat_ptr _tmp54A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp314->name));_tmp3C2.f1=_tmp54A;});_tmp3C2;});void*_tmp300[1U];_tmp300[0]=& _tmp302;({struct _fat_ptr _tmp54B=({const char*_tmp301="%s";_tag_fat(_tmp301,sizeof(char),3U);});Cyc_aprintf(_tmp54B,_tag_fat(_tmp300,sizeof(void*),1U));});});else{
# 1577
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp305=({struct Cyc_String_pa_PrintArg_struct _tmp3C4;_tmp3C4.tag=0U,({struct _fat_ptr _tmp54C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp314->name));_tmp3C4.f1=_tmp54C;});_tmp3C4;});struct Cyc_String_pa_PrintArg_struct _tmp306=({struct Cyc_String_pa_PrintArg_struct _tmp3C3;_tmp3C3.tag=0U,({struct _fat_ptr _tmp54D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp33F));_tmp3C3.f1=_tmp54D;});_tmp3C3;});void*_tmp303[2U];_tmp303[0]=& _tmp305,_tmp303[1]=& _tmp306;({struct _fat_ptr _tmp54E=({const char*_tmp304="%s(%s)";_tag_fat(_tmp304,sizeof(char),7U);});Cyc_aprintf(_tmp54E,_tag_fat(_tmp303,sizeof(void*),2U));});});}case 9U: _LL1B: _LL1C:
 return({const char*_tmp307="NULL";_tag_fat(_tmp307,sizeof(char),5U);});case 10U: _LL1D: _tmp315=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2E6)->f2;_LL1E:
 return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30A=({struct Cyc_Int_pa_PrintArg_struct _tmp3C5;_tmp3C5.tag=1U,_tmp3C5.f1=(unsigned long)_tmp315;_tmp3C5;});void*_tmp308[1U];_tmp308[0]=& _tmp30A;({struct _fat_ptr _tmp54F=({const char*_tmp309="%d";_tag_fat(_tmp309,sizeof(char),3U);});Cyc_aprintf(_tmp54F,_tag_fat(_tmp308,sizeof(void*),1U));});});case 11U: _LL1F: _tmp316=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1;_LL20:
 return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30D=({struct Cyc_Int_pa_PrintArg_struct _tmp3C6;_tmp3C6.tag=1U,_tmp3C6.f1=(unsigned long)((int)_tmp316);_tmp3C6;});void*_tmp30B[1U];_tmp30B[0]=& _tmp30D;({struct _fat_ptr _tmp550=({const char*_tmp30C="%d";_tag_fat(_tmp30C,sizeof(char),3U);});Cyc_aprintf(_tmp550,_tag_fat(_tmp30B,sizeof(void*),1U));});});case 12U: _LL21: _tmp318=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1;_tmp317=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2E6)->f2;_LL22:
 return _tmp318;case 13U: _LL23: _tmp319=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2E6)->f2;_LL24:
 _tmp31A=_tmp319;goto _LL26;case 14U: _LL25: _tmp31A=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2E6)->f2;_LL26:
 return Cyc_Absynpp_qvar2string(_tmp31A->name);case 17U: _LL27: _tmp31B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2E6)->f1;_LL28:
 return Cyc_Absynpp_exp2string(_tmp31B);default: _LL29: _LL2A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LLA:;};}}else{_LL3: _tmp341=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2E0)->f1;_LL4:
# 1588
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp341))return({const char*_tmp31C="_";_tag_fat(_tmp31C,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*_tmp31D=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp341);
# 1592
if((int)(((_tmp31D->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp31E=({union Cyc_Tcpat_PatOrWhere _tmp33E=_tmp31D->orig_pat;if((_tmp33E.pattern).tag != 1)_throw_match();(_tmp33E.pattern).val;});
void*_tmp31F=Cyc_Tcutil_compress((void*)_check_null(_tmp31E->topt));void*_tmp320=_tmp31F;struct Cyc_Absyn_PtrAtts _tmp33D;struct Cyc_Absyn_Aggrdecl*_tmp33C;struct Cyc_Absyn_Datatypedecl*_tmp33B;switch(*((int*)_tmp320)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp320)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp320)->f1)->f2 == Cyc_Absyn_Char_sz){_LL31: _LL32:
# 1597
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp321=Cyc_Tcpat_char_con((char)i,_tmp31E);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp341,_tmp321))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp324=({struct Cyc_Int_pa_PrintArg_struct _tmp3C7;_tmp3C7.tag=1U,_tmp3C7.f1=(unsigned long)i;_tmp3C7;});void*_tmp322[1U];_tmp322[0]=& _tmp324;({struct _fat_ptr _tmp551=({const char*_tmp323="%d";_tag_fat(_tmp323,sizeof(char),3U);});Cyc_aprintf(_tmp551,_tag_fat(_tmp322,sizeof(void*),1U));});});}}
# 1602
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL33: _LL34:
# 1605
{unsigned i=0U;for(0;i < (unsigned)-1;++ i){
struct Cyc_Tcpat_Con_s*_tmp325=Cyc_Tcpat_int_con((int)i,_tmp31D->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp341,_tmp325))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp328=({struct Cyc_Int_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=1U,_tmp3C8.f1=(unsigned long)((int)i);_tmp3C8;});void*_tmp326[1U];_tmp326[0]=& _tmp328;({struct _fat_ptr _tmp552=({const char*_tmp327="%d";_tag_fat(_tmp327,sizeof(char),3U);});Cyc_aprintf(_tmp552,_tag_fat(_tmp326,sizeof(void*),1U));});});}}
# 1610
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp320)->f1)->f1).KnownDatatype).tag == 2){_LL37: _tmp33B=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp320)->f1)->f1).KnownDatatype).val;_LL38:
# 1620
 if(_tmp33B->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp32C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp33B->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32C);
for(0;(unsigned)_tmp32C;_tmp32C=_tmp32C->tl){
struct _fat_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp32C->hd)->name).f2;
struct Cyc_List_List*_tmp32D=((struct Cyc_Absyn_Datatypefield*)_tmp32C->hd)->typs;
if(!({struct Cyc_Set_Set*_tmp554=_tmp341;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp554,({struct Cyc_Tcpat_Con_s*_tmp32E=_cycalloc(sizeof(*_tmp32E));({union Cyc_Tcpat_Name_value _tmp553=Cyc_Tcpat_Name_v(n);_tmp32E->name=_tmp553;}),_tmp32E->arity=0,_tmp32E->span=0,_tmp32E->orig_pat=_tmp31D->orig_pat;_tmp32E;}));})){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32D)== 0)
return n;else{
# 1630
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp331=({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0U,_tmp3C9.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp3C9;});void*_tmp32F[1U];_tmp32F[0]=& _tmp331;({struct _fat_ptr _tmp555=({const char*_tmp330="%s(...)";_tag_fat(_tmp330,sizeof(char),8U);});Cyc_aprintf(_tmp555,_tag_fat(_tmp32F,sizeof(void*),1U));});});}}}
# 1633
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL3B;}case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp320)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp33C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp320)->f1)->f1).KnownAggr).val;if((int)_tmp33C->kind == (int)Cyc_Absyn_UnionA){_LL3A: {
# 1635
struct Cyc_List_List*_tmp332=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp33C->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp332);
struct _tuple2*_tmp333=_tmp33C->name;struct _tuple2*_tmp334=_tmp333;struct _fat_ptr _tmp33A;_LL3E: _tmp33A=*_tmp334->f2;_LL3F:;
for(0;(unsigned)_tmp332;_tmp332=_tmp332->tl){
struct _fat_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp332->hd)->name;
if(!({struct Cyc_Set_Set*_tmp557=_tmp341;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp557,({struct Cyc_Tcpat_Con_s*_tmp335=_cycalloc(sizeof(*_tmp335));({union Cyc_Tcpat_Name_value _tmp556=Cyc_Tcpat_Name_v(n);_tmp335->name=_tmp556;}),_tmp335->arity=0,_tmp335->span=0,_tmp335->orig_pat=_tmp31D->orig_pat;_tmp335;}));}))
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp338=({struct Cyc_String_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=0U,_tmp3CB.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp33A);_tmp3CB;});struct Cyc_String_pa_PrintArg_struct _tmp339=({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0U,_tmp3CA.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp3CA;});void*_tmp336[2U];_tmp336[0]=& _tmp338,_tmp336[1]=& _tmp339;({struct _fat_ptr _tmp558=({const char*_tmp337="%s{.%s = _}";_tag_fat(_tmp337,sizeof(char),12U);});Cyc_aprintf(_tmp558,_tag_fat(_tmp336,sizeof(void*),2U));});});}
# 1643
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL3B;}}else{goto _LL3B;}default: goto _LL3B;}case 3U: _LL35: _tmp33D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp320)->f1).ptr_atts;_LL36: {
# 1612
void*_tmp329=_tmp33D.nullable;
int is_nullable=Cyc_Tcutil_force_type2bool(0,_tmp329);
if(is_nullable){
if(!({struct Cyc_Set_Set*_tmp559=_tmp341;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp559,Cyc_Tcpat_null_con(_tmp31E));}))
return({const char*_tmp32A="NULL";_tag_fat(_tmp32A,sizeof(char),5U);});}
# 1618
return({const char*_tmp32B="&_";_tag_fat(_tmp32B,sizeof(char),3U);});}default: _LL3B: _LL3C:
# 1644
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL30:;};};}_LL0:;}
# 1648
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _fat_ptr*_tmp342=_cycalloc(sizeof(*_tmp342));({struct _fat_ptr _tmp55A=Cyc_Tcpat_desc2string(d);*_tmp342=_tmp55A;});_tmp342;});}
# 1651
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _fat_ptr*comma=({struct _fat_ptr*_tmp347=_cycalloc(sizeof(*_tmp347));({struct _fat_ptr _tmp55B=({const char*_tmp346=",";_tag_fat(_tmp346,sizeof(char),2U);});*_tmp347=_tmp55B;});_tmp347;});
{struct Cyc_List_List*_tmp344=_tmp343;for(0;_tmp344 != 0;_tmp344=((struct Cyc_List_List*)_check_null(_tmp344))->tl){
if(_tmp344->tl != 0){
({struct Cyc_List_List*_tmp55C=({struct Cyc_List_List*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345->hd=comma,_tmp345->tl=_tmp344->tl;_tmp345;});_tmp344->tl=_tmp55C;});
_tmp344=_tmp344->tl;}}}
# 1660
return(struct _fat_ptr)Cyc_strconcat_l(_tmp343);}
# 1663
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){
struct _handler_cons _tmp348;_push_handler(& _tmp348);{int _tmp34A=0;if(setjmp(_tmp348.handler))_tmp34A=1;if(!_tmp34A){
{struct _fat_ptr _tmp34B=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp34E=({struct Cyc_String_pa_PrintArg_struct _tmp3CC;_tmp3CC.tag=0U,_tmp3CC.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp34B);_tmp3CC;});void*_tmp34C[1U];_tmp34C[0]=& _tmp34E;({unsigned _tmp55E=loc;struct _fat_ptr _tmp55D=({const char*_tmp34D="patterns may not be exhaustive.\n\tmissing case for %s";_tag_fat(_tmp34D,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp55E,_tmp55D,_tag_fat(_tmp34C,sizeof(void*),1U));});});}
# 1665
;_pop_handler();}else{void*_tmp349=(void*)Cyc_Core_get_exn_thrown();void*_tmp34F=_tmp349;void*_tmp352;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp34F)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1669
({void*_tmp350=0U;({unsigned _tmp560=loc;struct _fat_ptr _tmp55F=({const char*_tmp351="patterns may not be exhaustive.";_tag_fat(_tmp351,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp560,_tmp55F,_tag_fat(_tmp350,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp352=_tmp34F;_LL4:(int)_rethrow(_tmp352);}_LL0:;}};}
# 1673
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1677
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1683
int _tmp353=0;
int*_tmp354=& _tmp353;
struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp354,swcs);
void*_tmp356=Cyc_Tcpat_match_compile(_tmp355);
*dopt=_tmp356;{
# 1689
int _tmp357=0;
((void(*)(void*d,void(*not_exhaust)(unsigned,void*),unsigned env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp356,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp357);
# 1692
for(0;_tmp355 != 0;_tmp355=_tmp355->tl){
struct _tuple22*_tmp358=(struct _tuple22*)_tmp355->hd;struct _tuple22*_tmp359=_tmp358;int _tmp35D;unsigned _tmp35C;_LL1: _tmp35D=(_tmp359->f2)->used;_tmp35C=(_tmp359->f2)->pat_loc;_LL2:;
if(!_tmp35D){
({void*_tmp35A=0U;({unsigned _tmp562=_tmp35C;struct _fat_ptr _tmp561=({const char*_tmp35B="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp35B,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp562,_tmp561,_tag_fat(_tmp35A,sizeof(void*),0U));});});
# 1697
break;}}};}
# 1706
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp35E;_push_handler(& _tmp35E);{int _tmp360=0;if(setjmp(_tmp35E.handler))_tmp360=1;if(!_tmp360){
{struct _fat_ptr _tmp361=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp364=({struct Cyc_String_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=0U,_tmp3CD.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp361);_tmp3CD;});void*_tmp362[1U];_tmp362[0]=& _tmp364;({unsigned _tmp564=(*pr).f1;struct _fat_ptr _tmp563=({const char*_tmp363="pattern not exhaustive.\n\tmissing case for %s";_tag_fat(_tmp363,sizeof(char),45U);});Cyc_Tcutil_warn(_tmp564,_tmp563,_tag_fat(_tmp362,sizeof(void*),1U));});});}
# 1709
;_pop_handler();}else{void*_tmp35F=(void*)Cyc_Core_get_exn_thrown();void*_tmp365=_tmp35F;void*_tmp368;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp365)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1713
({void*_tmp366=0U;({unsigned _tmp566=(*pr).f1;struct _fat_ptr _tmp565=({const char*_tmp367="pattern not exhaustive.";_tag_fat(_tmp367,sizeof(char),24U);});Cyc_Tcutil_warn(_tmp566,_tmp565,_tag_fat(_tmp366,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp368=_tmp365;_LL4:(int)_rethrow(_tmp368);}_LL0:;}};};}
# 1717
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1721
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F->used=0,_tmp36F->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_tmp567=Cyc_Absyn_skip_stmt(0U);_tmp36F->rhs=_tmp567;});_tmp36F;});
struct Cyc_List_List*_tmp369=({struct Cyc_List_List*_tmp36E=_cycalloc(sizeof(*_tmp36E));({struct _tuple22*_tmp569=({struct _tuple22*_tmp36D=_cycalloc(sizeof(*_tmp36D));({void*_tmp568=Cyc_Tcpat_compile_pat(p);_tmp36D->f1=_tmp568;}),_tmp36D->f2=rhs;_tmp36D;});_tmp36E->hd=_tmp569;}),_tmp36E->tl=0;_tmp36E;});
void*_tmp36A=Cyc_Tcpat_match_compile(_tmp369);
struct _tuple14 _tmp36B=({struct _tuple14 _tmp3CE;_tmp3CE.f1=loc,_tmp3CE.f2=1;_tmp3CE;});
int _tmp36C=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp36A,Cyc_Tcpat_not_exhaust_warn,& _tmp36B,Cyc_Tcpat_dummy_fn,0,& _tmp36C);
# 1729
*dopt=_tmp36A;
return _tmp36B.f2;}
# 1737
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1741
if(swc->where_clause != 0)
({void*_tmp370=0U;({unsigned _tmp56B=((struct Cyc_Absyn_Exp*)_check_null(swc->where_clause))->loc;struct _fat_ptr _tmp56A=({const char*_tmp371="&&-clauses not supported in exception handlers.";_tag_fat(_tmp371,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp56B,_tmp56A,_tag_fat(_tmp370,sizeof(void*),0U));});});{
# 1744
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->used=0,_tmp373->pat_loc=(swc->pattern)->loc,_tmp373->rhs=swc->body;_tmp373;});
return({struct _tuple22*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->f1=sp0,_tmp372->f2=rhs;_tmp372;});};}
# 1747
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1750
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1754
struct Cyc_List_List*_tmp374=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp375=Cyc_Tcpat_match_compile(_tmp374);
*dopt=_tmp375;{
int _tmp376=0;
((void(*)(void*d,void(*not_exhaust)(unsigned,void*),unsigned env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp375,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp376);
# 1760
for(0;_tmp374 != 0;_tmp374=_tmp374->tl){
# 1762
if(_tmp374->tl == 0)break;{
struct _tuple22*_tmp377=(struct _tuple22*)_tmp374->hd;struct _tuple22*_tmp378=_tmp377;int _tmp37C;unsigned _tmp37B;_LL1: _tmp37C=(_tmp378->f2)->used;_tmp37B=(_tmp378->f2)->pat_loc;_LL2:;
if(!_tmp37C){
({void*_tmp379=0U;({unsigned _tmp56D=_tmp37B;struct _fat_ptr _tmp56C=({const char*_tmp37A="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp37A,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp56D,_tmp56C,_tag_fat(_tmp379,sizeof(void*),0U));});});
break;}};}};}
# 1771
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp37D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;for(0;_tmp37D != 0;_tmp37D=_tmp37D->tl){
if((*((struct _tuple15*)_tmp37D->hd)).f1 != 0)
return 1;}}
return 0;}

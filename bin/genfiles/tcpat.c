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
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 290
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 303
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 310
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 691 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 874
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 882
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 888
void*Cyc_Absyn_compress_kb(void*);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 901
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 904
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 906
extern void*Cyc_Absyn_sint_type;
# 909
void*Cyc_Absyn_gen_float_type(unsigned i);
# 911
extern void*Cyc_Absyn_unique_rgn_type;
# 913
extern void*Cyc_Absyn_empty_effect;
# 915
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 917
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 946
void*Cyc_Absyn_bounds_one();
# 973
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1047
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1139
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
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
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
# 114
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 121
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*assume,void*t1,void*t2);
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 147
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 154
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 157
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 163
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 185
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 187
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 207
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 214
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr err_msg);
# 222
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 233
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 241
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 246
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 269
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 294
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 301
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 304
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 306
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 88 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 138
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 140
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 142
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 45 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Aggrdecl**_tmp2A;struct Cyc_List_List**_tmp29;struct Cyc_List_List*_tmp28;int _tmp27;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp25;int _tmp24;switch(*((int*)_tmp1)){case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp26=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp25=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp24=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2:
# 51
 if(topt == 0)
({void*_tmp2=0U;({unsigned _tmp3D4=p->loc;struct _fat_ptr _tmp3D3=({const char*_tmp3="cannot determine pattern type";_tag_fat(_tmp3,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp3D4,_tmp3D3,_tag_fat(_tmp2,sizeof(void*),0U));});});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp4=t;union Cyc_Absyn_AggrInfo _tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->f1)->tag == 20U){_LL10: _tmpA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->f1)->f1;_LL11:
# 56
({void*_tmp3D6=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp3D5=({union Cyc_Absyn_AggrInfo*_tmp5=_cycalloc(sizeof(*_tmp5));*_tmp5=_tmpA;_tmp5;});_tmp6->f1=_tmp3D5;}),_tmp6->f2=_tmp26,_tmp6->f3=_tmp25,_tmp6->f4=_tmp24;_tmp6;});p->r=_tmp3D6;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}else{goto _LL12;}}else{_LL12: _LL13:
# 60
({struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp382;_tmp382.tag=0U,({struct _fat_ptr _tmp3D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp382.f1=_tmp3D7;});_tmp382;});void*_tmp7[1U];_tmp7[0]=& _tmp9;({unsigned _tmp3D9=p->loc;struct _fat_ptr _tmp3D8=({const char*_tmp8="pattern expects aggregate type instead of %s";_tag_fat(_tmp8,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp3D9,_tmp3D8,_tag_fat(_tmp7,sizeof(void*),1U));});});
goto _LLF;}_LLF:;}
# 63
return;};}else{if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).tag == 2){_LL3: _tmp2A=((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).val;_tmp29=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp28=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp27=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {
# 66
struct Cyc_Absyn_Aggrdecl*_tmpB=*_tmp2A;
if(_tmpB->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp383;_tmp383.tag=0U,({
struct _fat_ptr _tmp3DA=(struct _fat_ptr)((int)_tmpB->kind == (int)Cyc_Absyn_StructA?({const char*_tmpF="struct";_tag_fat(_tmpF,sizeof(char),7U);}):({const char*_tmp10="union";_tag_fat(_tmp10,sizeof(char),6U);}));_tmp383.f1=_tmp3DA;});_tmp383;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({unsigned _tmp3DC=p->loc;struct _fat_ptr _tmp3DB=({const char*_tmpD="can't destructure an abstract %s";_tag_fat(_tmpD,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp3DC,_tmp3DB,_tag_fat(_tmpC,sizeof(void*),1U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 73
int more_exists=({int _tmp3DD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB->impl))->exist_vars);_tmp3DD - ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp29);});
if(more_exists < 0){
({void*_tmp11=0U;({unsigned _tmp3DF=p->loc;struct _fat_ptr _tmp3DE=({const char*_tmp12="too many existentially bound type variables in pattern";_tag_fat(_tmp12,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp3DF,_tmp3DE,_tag_fat(_tmp11,sizeof(void*),0U));});});{
struct Cyc_List_List**_tmp13=_tmp29;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB->impl))->exist_vars);for(0;n != 0;-- n){
_tmp13=&((struct Cyc_List_List*)_check_null(*_tmp13))->tl;}}
*_tmp13=0;};}else{
if(more_exists > 0){
# 82
struct Cyc_List_List*_tmp14=0;
for(0;more_exists != 0;-- more_exists){
_tmp14=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct Cyc_Absyn_Tvar*_tmp3E0=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=1U,_tmp15->f1=0;_tmp15;}));_tmp16->hd=_tmp3E0;}),_tmp16->tl=_tmp14;_tmp16;});}
({struct Cyc_List_List*_tmp3E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp29,_tmp14);*_tmp29=_tmp3E1;});}}
# 87
return;};}}else{_LLB: _LLC:
# 100
({void*_tmp22=0U;({struct _fat_ptr _tmp3E2=({const char*_tmp23="resolve_pat unknownAggr";_tag_fat(_tmp23,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp3E2,_tag_fat(_tmp22,sizeof(void*),0U));});});}}case 17U: _LL5: _tmp2B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6:
# 89
 Cyc_Tcexp_tcExp(te,0,_tmp2B);
if(!Cyc_Tcutil_is_const_exp(_tmp2B)){
({void*_tmp17=0U;({unsigned _tmp3E4=p->loc;struct _fat_ptr _tmp3E3=({const char*_tmp18="non-constant expression in case pattern";_tag_fat(_tmp18,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp3E4,_tmp3E3,_tag_fat(_tmp17,sizeof(void*),0U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 94
struct _tuple14 _tmp19=Cyc_Evexp_eval_const_uint_exp(_tmp2B);struct _tuple14 _tmp1A=_tmp19;unsigned _tmp1D;int _tmp1C;_LL15: _tmp1D=_tmp1A.f1;_tmp1C=_tmp1A.f2;_LL16:;
({void*_tmp3E5=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=10U,_tmp1B->f1=Cyc_Absyn_None,_tmp1B->f2=(int)_tmp1D;_tmp1B;});p->r=_tmp3E5;});
return;};case 15U: _LL7: _LL8:
# 98
({void*_tmp1E=0U;({struct _fat_ptr _tmp3E6=({const char*_tmp1F="resolve_pat UnknownId_p";_tag_fat(_tmp1F,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp3E6,_tag_fat(_tmp1E,sizeof(void*),0U));});});case 16U: _LL9: _LLA:
({void*_tmp20=0U;({struct _fat_ptr _tmp3E7=({const char*_tmp21="resolve_pat UnknownCall_p";_tag_fat(_tmp21,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp3E7,_tag_fat(_tmp20,sizeof(void*),0U));});});default: _LLD: _LLE:
# 102
 return;}_LL0:;}
# 106
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 109
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->v=s;_tmp2C;}));}
# 114
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 118
if(topt != 0 && Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;
# 121
{void*_tmp2D=Cyc_Tcutil_compress(numt);void*_tmp2E=_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->f1)){case 15U: _LL1: _LL2:
 goto _LL4;case 16U: _LL3: _LL4:
 if(topt != 0)return*topt;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 126
return numt;}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 129
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 131
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp3E8=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_tmp3E8;});}
# 135
({struct Cyc_List_List*_tmp3EA=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _tuple15*_tmp3E9=({struct _tuple15*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->f1=vd,_tmp2F->f2=e;_tmp2F;});_tmp30->hd=_tmp3E9;}),_tmp30->tl=*v_result_ptr;_tmp30;});*v_result_ptr=_tmp3EA;});}
# 137
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 139
struct Cyc_Tcpat_TcPatResult _tmp31=res1;struct _tuple1*_tmp39;struct Cyc_List_List*_tmp38;_LL1: _tmp39=_tmp31.tvars_and_bounds_opt;_tmp38=_tmp31.patvars;_LL2:;{
struct Cyc_Tcpat_TcPatResult _tmp32=res2;struct _tuple1*_tmp37;struct Cyc_List_List*_tmp36;_LL4: _tmp37=_tmp32.tvars_and_bounds_opt;_tmp36=_tmp32.patvars;_LL5:;
if(_tmp39 != 0 || _tmp37 != 0){
if(_tmp39 == 0)
_tmp39=({struct _tuple1*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=0,_tmp33->f2=0;_tmp33;});
if(_tmp37 == 0)
_tmp37=({struct _tuple1*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->f1=0,_tmp34->f2=0;_tmp34;});
return({struct Cyc_Tcpat_TcPatResult _tmp384;({struct _tuple1*_tmp3EE=({struct _tuple1*_tmp35=_cycalloc(sizeof(*_tmp35));({struct Cyc_List_List*_tmp3ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp39).f1,(*_tmp37).f1);_tmp35->f1=_tmp3ED;}),({
struct Cyc_List_List*_tmp3EC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp39).f2,(*_tmp37).f2);_tmp35->f2=_tmp3EC;});_tmp35;});
# 146
_tmp384.tvars_and_bounds_opt=_tmp3EE;}),({
# 148
struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp38,_tmp36);_tmp384.patvars=_tmp3EB;});_tmp384;});}
# 150
return({struct Cyc_Tcpat_TcPatResult _tmp385;_tmp385.tvars_and_bounds_opt=0,({struct Cyc_List_List*_tmp3EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp38,_tmp36);_tmp385.patvars=_tmp3EF;});_tmp385;});};}
# 153
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){
return({struct Cyc_Absyn_Pat*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->loc=loc,_tmp3A->topt=0,_tmp3A->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp3A;});}
# 158
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp3F1=t;Cyc_Tcutil_promote_array(_tmp3F1,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp3B=_cycalloc(sizeof(*_tmp3B));({struct Cyc_List_List*_tmp3F0=Cyc_Tcenv_lookup_type_vars(te);_tmp3B->v=_tmp3F0;});_tmp3B;})): rgn_opt,0);}): t;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};
# 165
static struct _tuple16*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple16*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=tv,_tmp3C->f2=0;_tmp3C;});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 169
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 172
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp398;_tmp398.tvars_and_bounds_opt=0,_tmp398.patvars=0;_tmp398;});
# 177
{void*_tmp3D=p->r;void*_tmp3E=_tmp3D;struct Cyc_Absyn_Datatypedecl*_tmpF8;struct Cyc_Absyn_Datatypefield*_tmpF7;struct Cyc_List_List**_tmpF6;int _tmpF5;struct Cyc_Absyn_Aggrdecl*_tmpF4;struct Cyc_List_List*_tmpF3;struct Cyc_List_List**_tmpF2;int _tmpF1;struct Cyc_List_List**_tmpF0;int _tmpEF;struct Cyc_Absyn_Pat*_tmpEE;void*_tmpED;struct Cyc_Absyn_Enumdecl*_tmpEC;int _tmpEB;struct Cyc_Absyn_Tvar*_tmpEA;struct Cyc_Absyn_Vardecl*_tmpE9;struct Cyc_Absyn_Vardecl*_tmpE8;struct Cyc_Absyn_Pat*_tmpE7;struct Cyc_Absyn_Tvar*_tmpE6;struct Cyc_Absyn_Vardecl*_tmpE5;struct Cyc_Absyn_Vardecl*_tmpE4;struct Cyc_Absyn_Pat*_tmpE3;switch(*((int*)_tmp3E)){case 0U: _LL1: _LL2:
# 180
 if(topt != 0)
t=*topt;else{
# 183
t=({struct Cyc_List_List*_tmp3F2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp3F2,topt);});}
goto _LL0;case 1U: _LL3: _tmpE4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL4: {
# 187
struct _tuple2*_tmp3F=_tmpE4->name;struct _tuple2*_tmp40=_tmp3F;struct _fat_ptr _tmp4D;_LL2E: _tmp4D=*_tmp40->f2;_LL2F:;
if(({struct _fat_ptr _tmp3F4=(struct _fat_ptr)_tmp4D;Cyc_strcmp(_tmp3F4,({const char*_tmp41="true";_tag_fat(_tmp41,sizeof(char),5U);}));})== 0 ||({struct _fat_ptr _tmp3F3=(struct _fat_ptr)_tmp4D;Cyc_strcmp(_tmp3F3,({const char*_tmp42="false";_tag_fat(_tmp42,sizeof(char),6U);}));})== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp45=({struct Cyc_String_pa_PrintArg_struct _tmp386;_tmp386.tag=0U,_tmp386.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp4D);_tmp386;});void*_tmp43[1U];_tmp43[0]=& _tmp45;({unsigned _tmp3F6=p->loc;struct _fat_ptr _tmp3F5=({const char*_tmp44="you probably do not want to use %s as a local variable";_tag_fat(_tmp44,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp3F6,_tmp3F5,_tag_fat(_tmp43,sizeof(void*),1U));});});
res=Cyc_Tcpat_tcPatRec(te,_tmpE3,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpE3->topt);
# 194
{void*_tmp46=Cyc_Tcutil_compress(t);void*_tmp47=_tmp46;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->tag == 4U){_LL31: _LL32:
# 196
 if(rgn_pat == 0 || !allow_ref_pat)
({void*_tmp48=0U;({unsigned _tmp3F8=p->loc;struct _fat_ptr _tmp3F7=({const char*_tmp49="array reference would point into unknown/unallowed region";_tag_fat(_tmp49,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp3F8,_tmp3F7,_tag_fat(_tmp48,sizeof(void*),0U));});});
goto _LL30;}else{_LL33: _LL34:
# 200
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp4A=0U;({unsigned _tmp3FA=p->loc;struct _fat_ptr _tmp3F9=({const char*_tmp4B="pattern would point to an abstract member";_tag_fat(_tmp4B,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp3FA,_tmp3F9,_tag_fat(_tmp4A,sizeof(void*),0U));});});
goto _LL30;}_LL30:;}
# 204
({struct Cyc_Absyn_Vardecl**_tmp3FD=({struct Cyc_Absyn_Vardecl**_tmp4C=_cycalloc(sizeof(*_tmp4C));*_tmp4C=_tmpE4;_tmp4C;});struct Cyc_Absyn_Exp*_tmp3FC=access_exp;struct Cyc_List_List**_tmp3FB=& res.patvars;Cyc_Tcpat_set_vd(_tmp3FD,_tmp3FC,_tmp3FB,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}case 2U: _LL5: _tmpE6=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE5=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL6: {
# 207
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp3FF=p->loc;struct Cyc_Tcenv_Tenv*_tmp3FE=te;Cyc_Tcenv_add_type_vars(_tmp3FF,_tmp3FE,({struct Cyc_Absyn_Tvar*_tmp58[1U];_tmp58[0]=_tmpE6;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp58,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp400=({struct _tuple1*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=0,_tmp4E->f2=0;_tmp4E;});res.tvars_and_bounds_opt=_tmp400;});
({struct Cyc_List_List*_tmp403=({
struct Cyc_List_List*_tmp402=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp402,({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _tuple16*_tmp401=({struct _tuple16*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=_tmpE6,_tmp4F->f2=1;_tmp4F;});_tmp50->hd=_tmp401;}),_tmp50->tl=0;_tmp50;}));});
# 210
(*res.tvars_and_bounds_opt).f1=_tmp403;});
# 212
({unsigned _tmp406=p->loc;struct Cyc_Tcenv_Tenv*_tmp405=te2;struct Cyc_List_List*_tmp404=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp406,_tmp405,_tmp404,& Cyc_Tcutil_tmk,1,0,_tmpE5->type);});
# 215
if(topt != 0)t=*topt;else{
t=({struct Cyc_List_List*_tmp407=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp407,topt);});}
{void*_tmp51=Cyc_Tcutil_compress(t);void*_tmp52=_tmp51;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->tag == 4U){_LL36: _LL37:
# 219
 if(rgn_pat == 0 || !allow_ref_pat)
({void*_tmp53=0U;({unsigned _tmp409=p->loc;struct _fat_ptr _tmp408=({const char*_tmp54="array reference would point into unknown/unallowed region";_tag_fat(_tmp54,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp409,_tmp408,_tag_fat(_tmp53,sizeof(void*),0U));});});
goto _LL35;}else{_LL38: _LL39:
# 223
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp55=0U;({unsigned _tmp40B=p->loc;struct _fat_ptr _tmp40A=({const char*_tmp56="pattern would point to an abstract member";_tag_fat(_tmp56,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp40B,_tmp40A,_tag_fat(_tmp55,sizeof(void*),0U));});});
goto _LL35;}_LL35:;}
# 227
({struct Cyc_Absyn_Vardecl**_tmp40E=({struct Cyc_Absyn_Vardecl**_tmp57=_cycalloc(sizeof(*_tmp57));*_tmp57=_tmpE5;_tmp57;});struct Cyc_Absyn_Exp*_tmp40D=access_exp;struct Cyc_List_List**_tmp40C=& res.patvars;Cyc_Tcpat_set_vd(_tmp40E,_tmp40D,_tmp40C,_tmpE5->type);});
goto _LL0;}case 3U: _LL7: _tmpE8=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL8:
# 231
 res=Cyc_Tcpat_tcPatRec(te,_tmpE7,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpE7->topt);
if(!allow_ref_pat || rgn_pat == 0){
({void*_tmp59=0U;({unsigned _tmp410=p->loc;struct _fat_ptr _tmp40F=({const char*_tmp5A="* pattern would point into an unknown/unallowed region";_tag_fat(_tmp5A,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp410,_tmp40F,_tag_fat(_tmp59,sizeof(void*),0U));});});
goto _LL0;}else{
# 238
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({void*_tmp5B=0U;({unsigned _tmp412=p->loc;struct _fat_ptr _tmp411=({const char*_tmp5C="* pattern cannot take the address of an alias-free path";_tag_fat(_tmp5C,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp412,_tmp411,_tag_fat(_tmp5B,sizeof(void*),0U));});});}{
# 241
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_List_List*_tmp5D=Cyc_Tcenv_lookup_type_vars(te);
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=3U,(_tmp5F->f1).elt_type=t,({struct Cyc_Absyn_Tqual _tmp415=Cyc_Absyn_empty_tqual(0U);(_tmp5F->f1).elt_tq=_tmp415;}),
((_tmp5F->f1).ptr_atts).rgn=rgn_pat,((_tmp5F->f1).ptr_atts).nullable=Cyc_Absyn_false_type,({
void*_tmp414=Cyc_Tcutil_any_bounds(_tmp5D);((_tmp5F->f1).ptr_atts).bounds=_tmp414;}),({void*_tmp413=Cyc_Tcutil_any_bool(_tmp5D);((_tmp5F->f1).ptr_atts).zero_term=_tmp413;}),((_tmp5F->f1).ptr_atts).ptrloc=0;_tmp5F;});
# 247
if((unsigned)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);
new_access_exp->topt=t2;}
# 251
({struct Cyc_Absyn_Vardecl**_tmp418=({struct Cyc_Absyn_Vardecl**_tmp5E=_cycalloc(sizeof(*_tmp5E));*_tmp5E=_tmpE8;_tmp5E;});struct Cyc_Absyn_Exp*_tmp417=new_access_exp;struct Cyc_List_List**_tmp416=& res.patvars;Cyc_Tcpat_set_vd(_tmp418,_tmp417,_tmp416,t2);});
goto _LL0;};case 4U: _LL9: _tmpEA=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE9=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LLA:
# 257
({struct Cyc_Absyn_Vardecl**_tmp41B=({struct Cyc_Absyn_Vardecl**_tmp60=_cycalloc(sizeof(*_tmp60));*_tmp60=_tmpE9;_tmp60;});struct Cyc_Absyn_Exp*_tmp41A=access_exp;struct Cyc_List_List**_tmp419=& res.patvars;Cyc_Tcpat_set_vd(_tmp41B,_tmp41A,_tmp419,_tmpE9->type);});
# 261
({unsigned _tmp41D=p->loc;struct Cyc_Tcenv_Tenv*_tmp41C=te;Cyc_Tcenv_add_type_vars(_tmp41D,_tmp41C,({struct Cyc_Absyn_Tvar*_tmp61[1U];_tmp61[0]=_tmpEA;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp61,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp41E=({struct _tuple1*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=0,_tmp62->f2=0;_tmp62;});res.tvars_and_bounds_opt=_tmp41E;});
({struct Cyc_List_List*_tmp420=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));
({struct _tuple16*_tmp41F=({struct _tuple16*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->f1=_tmpEA,_tmp63->f2=0;_tmp63;});_tmp64->hd=_tmp41F;}),_tmp64->tl=(*res.tvars_and_bounds_opt).f1;_tmp64;});
# 264
(*res.tvars_and_bounds_opt).f1=_tmp420;});
# 266
t=Cyc_Absyn_uint_type;
goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3E)->f1){case Cyc_Absyn_Unsigned: _LLB: _LLC:
# 269
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;case Cyc_Absyn_None: _LLD: _LLE:
 goto _LL10;default: _LLF: _LL10:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;}case 11U: _LL11: _LL12:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 12U: _LL13: _tmpEB=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL14:
 t=({void**_tmp421=topt;Cyc_Tcpat_num_type(_tmp421,Cyc_Absyn_gen_float_type((unsigned)_tmpEB));});goto _LL0;case 13U: _LL15: _tmpEC=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL16:
# 275
 t=({void**_tmp422=topt;Cyc_Tcpat_num_type(_tmp422,Cyc_Absyn_enum_type(_tmpEC->name,_tmpEC));});
goto _LL0;case 14U: _LL17: _tmpED=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL18:
 t=Cyc_Tcpat_num_type(topt,_tmpED);goto _LL0;case 9U: _LL19: _LL1A:
# 279
 if(topt != 0){
void*_tmp65=Cyc_Tcutil_compress(*topt);void*_tmp66=_tmp65;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66)->tag == 3U){_LL3B: _LL3C:
 t=*topt;goto tcpat_end;}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}{
# 284
struct Cyc_List_List*_tmp67=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=3U,({void*_tmp427=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->v=_tmp67;_tmp68;}));(_tmp6A->f1).elt_type=_tmp427;}),({
struct Cyc_Absyn_Tqual _tmp426=Cyc_Absyn_empty_tqual(0U);(_tmp6A->f1).elt_tq=_tmp426;}),
({void*_tmp425=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->v=_tmp67;_tmp69;}));((_tmp6A->f1).ptr_atts).rgn=_tmp425;}),((_tmp6A->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
# 289
void*_tmp424=Cyc_Tcutil_any_bounds(_tmp67);((_tmp6A->f1).ptr_atts).bounds=_tmp424;}),({void*_tmp423=Cyc_Tcutil_any_bool(_tmp67);((_tmp6A->f1).ptr_atts).zero_term=_tmp423;}),((_tmp6A->f1).ptr_atts).ptrloc=0;_tmp6A;});
goto _LL0;};case 6U: _LL1B: _tmpEE=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL1C: {
# 295
void*inner_typ=Cyc_Absyn_void_type;
void**_tmp6B=0;
int elt_const=0;
if(topt != 0){
void*_tmp6C=Cyc_Tcutil_compress(*topt);void*_tmp6D=_tmp6C;void*_tmp6F;struct Cyc_Absyn_Tqual _tmp6E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->tag == 3U){_LL40: _tmp6F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp6E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_tq;_LL41:
# 301
 inner_typ=_tmp6F;
_tmp6B=& inner_typ;
elt_const=_tmp6E.real_const;
goto _LL3F;}else{_LL42: _LL43:
 goto _LL3F;}_LL3F:;}{
# 310
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp428=res;Cyc_Tcpat_combine_results(_tmp428,Cyc_Tcpat_tcPatRec(te,_tmpEE,_tmp6B,ptr_rgn,1,new_access_exp));});
# 319
{void*_tmp70=Cyc_Tcutil_compress((void*)_check_null(_tmpEE->topt));void*_tmp71=_tmp70;struct Cyc_Absyn_Datatypedecl*_tmp7A;struct Cyc_Absyn_Datatypefield*_tmp79;struct Cyc_List_List*_tmp78;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1).KnownDatatypefield).tag == 2){_LL45: _tmp7A=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1).KnownDatatypefield).val).f1;_tmp79=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1).KnownDatatypefield).val).f2;_tmp78=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2;_LL46:
# 323
{void*_tmp72=Cyc_Tcutil_compress(inner_typ);void*_tmp73=_tmp72;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp73)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp73)->f1)->tag == 19U){_LL4A: _LL4B:
# 325
 goto DONT_PROMOTE;}else{goto _LL4C;}}else{_LL4C: _LL4D:
 goto _LL49;}_LL49:;}{
# 329
void*new_type=({union Cyc_Absyn_DatatypeInfo _tmp429=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp75=_cycalloc(sizeof(*_tmp75));*_tmp75=_tmp7A;_tmp75;}));Cyc_Absyn_datatype_type(_tmp429,_tmp78);});
# 331
_tmpEE->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=3U,(_tmp74->f1).elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _tmp42D=Cyc_Absyn_const_tqual(0U);(_tmp74->f1).elt_tq=_tmp42D;}):({
struct Cyc_Absyn_Tqual _tmp42C=Cyc_Absyn_empty_tqual(0U);(_tmp74->f1).elt_tq=_tmp42C;}),
((_tmp74->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp42B=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmp74->f1).ptr_atts).nullable=_tmp42B;}),({
void*_tmp42A=Cyc_Absyn_bounds_one();((_tmp74->f1).ptr_atts).bounds=_tmp42A;}),((_tmp74->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp74->f1).ptr_atts).ptrloc=0;_tmp74;});
# 338
goto _LL44;};}else{goto _LL47;}}else{goto _LL47;}}else{_LL47: _LL48:
# 340
 DONT_PROMOTE: {
struct Cyc_List_List*_tmp76=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=3U,(_tmp77->f1).elt_type=(void*)_check_null(_tmpEE->topt),
elt_const?({struct Cyc_Absyn_Tqual _tmp432=Cyc_Absyn_const_tqual(0U);(_tmp77->f1).elt_tq=_tmp432;}):({
struct Cyc_Absyn_Tqual _tmp431=Cyc_Absyn_empty_tqual(0U);(_tmp77->f1).elt_tq=_tmp431;}),
((_tmp77->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp430=Cyc_Tcutil_any_bool(_tmp76);((_tmp77->f1).ptr_atts).nullable=_tmp430;}),({
void*_tmp42F=Cyc_Tcutil_any_bounds(_tmp76);((_tmp77->f1).ptr_atts).bounds=_tmp42F;}),({void*_tmp42E=Cyc_Tcutil_any_bool(_tmp76);((_tmp77->f1).ptr_atts).zero_term=_tmp42E;}),((_tmp77->f1).ptr_atts).ptrloc=0;_tmp77;});}}_LL44:;}
# 348
if((unsigned)new_access_exp)new_access_exp->topt=_tmpEE->topt;
goto _LL0;};}case 5U: _LL1D: _tmpF0=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpEF=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL1E: {
# 352
struct Cyc_List_List*_tmp7B=*_tmpF0;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp7C=Cyc_Tcutil_compress(*topt);void*_tmp7D=_tmp7C;struct Cyc_List_List*_tmp83;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7D)->tag == 6U){_LL4F: _tmp83=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7D)->f1;_LL50:
# 358
 topt_ts=_tmp83;
if(_tmpEF){
# 361
int _tmp7E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7B);
int _tmp7F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp83);
if(_tmp7E < _tmp7F){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp7F - _tmp7E;++ i){
wild_ps=({struct Cyc_List_List*_tmp80=_cycalloc(sizeof(*_tmp80));({struct Cyc_Absyn_Pat*_tmp433=Cyc_Tcpat_wild_pat(p->loc);_tmp80->hd=_tmp433;}),_tmp80->tl=wild_ps;_tmp80;});}}
({struct Cyc_List_List*_tmp434=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp7B,wild_ps);*_tmpF0=_tmp434;});
_tmp7B=*_tmpF0;}else{
if(({int _tmp435=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7B);_tmp435 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp83);}))
({void*_tmp81=0U;({unsigned _tmp437=p->loc;struct _fat_ptr _tmp436=({const char*_tmp82="unnecessary ... in tuple pattern";_tag_fat(_tmp82,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp437,_tmp436,_tag_fat(_tmp81,sizeof(void*),0U));});});}}
# 373
goto _LL4E;}else{_LL51: _LL52:
# 375
 goto _LL4E;}_LL4E:;}else{
# 377
if(_tmpEF)
({void*_tmp84=0U;({unsigned _tmp439=p->loc;struct _fat_ptr _tmp438=({const char*_tmp85="cannot determine missing fields for ... in tuple pattern";_tag_fat(_tmp85,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp439,_tmp438,_tag_fat(_tmp84,sizeof(void*),0U));});});}
{int i=0;for(0;_tmp7B != 0;(_tmp7B=_tmp7B->tl,i ++)){
void**_tmp86=0;
if(topt_ts != 0){
_tmp86=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 385
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=({struct Cyc_Absyn_Exp*_tmp43A=access_exp;Cyc_Absyn_subscript_exp(_tmp43A,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp387;(_tmp387.Int_c).tag=5U,((_tmp387.Int_c).val).f1=Cyc_Absyn_Unsigned,((_tmp387.Int_c).val).f2=i;_tmp387;}),0U),0U);});
# 390
res=({struct Cyc_Tcpat_TcPatResult _tmp43B=res;Cyc_Tcpat_combine_results(_tmp43B,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp7B->hd,_tmp86,rgn_pat,allow_ref_pat,new_access_exp));});
# 393
if((unsigned)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp7B->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmp88=_cycalloc(sizeof(*_tmp88));({struct _tuple17*_tmp43D=({struct _tuple17*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Absyn_Tqual _tmp43C=Cyc_Absyn_empty_tqual(0U);_tmp87->f1=_tmp43C;}),_tmp87->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp7B->hd)->topt);_tmp87;});_tmp88->hd=_tmp43D;}),_tmp88->tl=pat_ts;_tmp88;});};}}
# 396
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=6U,({struct Cyc_List_List*_tmp43E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp89->f1=_tmp43E;});_tmp89;});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1)->KnownAggr).tag == 2){_LL1F: _tmpF4=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1)->KnownAggr).val;_tmpF3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_tmpF2=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f3;_tmpF1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f4;_LL20: {
# 400
struct Cyc_List_List*_tmp8A=*_tmpF2;
struct _fat_ptr aggr_str=(int)_tmpF4->kind == (int)Cyc_Absyn_StructA?({const char*_tmpC9="struct";_tag_fat(_tmpC9,sizeof(char),7U);}):({const char*_tmpCA="union";_tag_fat(_tmpCA,sizeof(char),6U);});
if(_tmpF4->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8D=({struct Cyc_String_pa_PrintArg_struct _tmp388;_tmp388.tag=0U,_tmp388.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp388;});void*_tmp8B[1U];_tmp8B[0]=& _tmp8D;({unsigned _tmp440=p->loc;struct _fat_ptr _tmp43F=({const char*_tmp8C="can't destructure an abstract %s";_tag_fat(_tmp8C,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp440,_tmp43F,_tag_fat(_tmp8B,sizeof(void*),1U));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL0;}
# 409
if((int)_tmpF4->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF4->impl))->tagged)
allow_ref_pat=0;
if(_tmpF3 != 0){
# 415
if(topt == 0 || Cyc_Tcutil_type_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}
# 418
{struct _RegionHandle _tmp8E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8E;_push_region(rgn);
# 420
{struct Cyc_List_List*_tmp8F=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp90=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF4->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpF3;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp90))->hd;
_tmp90=_tmp90->tl;{
void*_tmp91=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp92=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp93=_tmp92;struct Cyc_Absyn_Kind*_tmp97;struct Cyc_Absyn_Kind*_tmp96;switch(*((int*)_tmp93)){case 2U: _LL54: _tmp96=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93)->f2;_LL55:
 _tmp97=_tmp96;goto _LL57;case 0U: _LL56: _tmp97=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp93)->f1;_LL57:
 k2=_tmp97;goto _LL53;default: _LL58: _LL59:
({void*_tmp94=0U;({struct _fat_ptr _tmp441=({const char*_tmp95="unconstrained existential type variable in aggregate";_tag_fat(_tmp95,sizeof(char),53U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp441,_tag_fat(_tmp94,sizeof(void*),0U));});});}_LL53:;}
# 436
{void*_tmp98=_tmp91;struct Cyc_Core_Opt**_tmp9D;struct Cyc_Core_Opt**_tmp9C;struct Cyc_Absyn_Kind*_tmp9B;struct Cyc_Absyn_Kind*_tmp9A;switch(*((int*)_tmp98)){case 0U: _LL5B: _tmp9A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp98)->f1;_LL5C:
# 439
 if(!Cyc_Tcutil_kind_leq(k2,_tmp9A))
error=1;
goto _LL5A;case 2U: _LL5D: _tmp9C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98)->f1;_tmp9B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98)->f2;_LL5E:
 _tmp9D=_tmp9C;goto _LL60;default: _LL5F: _tmp9D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp98)->f1;_LL60:
({struct Cyc_Core_Opt*_tmp442=({struct Cyc_Core_Opt*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->v=_tmp92;_tmp99;});*_tmp9D=_tmp442;});goto _LL5A;}_LL5A:;}
# 445
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0U,_tmp38B.f1=(struct _fat_ptr)((struct _fat_ptr)*tv->name);_tmp38B;});struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp38A;_tmp38A.tag=0U,({
# 449
struct _fat_ptr _tmp443=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kindbound2string(_tmp91));_tmp38A.f1=_tmp443;});_tmp38A;});struct Cyc_String_pa_PrintArg_struct _tmpA2=({struct Cyc_String_pa_PrintArg_struct _tmp389;_tmp389.tag=0U,({struct _fat_ptr _tmp444=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k2));_tmp389.f1=_tmp444;});_tmp389;});void*_tmp9E[3U];_tmp9E[0]=& _tmpA0,_tmp9E[1]=& _tmpA1,_tmp9E[2]=& _tmpA2;({unsigned _tmp446=p->loc;struct _fat_ptr _tmp445=({const char*_tmp9F="type variable %s has kind %s but must have at least kind %s";_tag_fat(_tmp9F,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp446,_tmp445,_tag_fat(_tmp9E,sizeof(void*),3U));});});{
# 451
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=2U,_tmpA8->f1=tv;_tmpA8;});
_tmp8F=({struct Cyc_List_List*_tmpA3=_region_malloc(rgn,sizeof(*_tmpA3));_tmpA3->hd=vartype,_tmpA3->tl=_tmp8F;_tmpA3;});
# 455
if((int)k2->kind == (int)Cyc_Absyn_RgnKind){
if((int)k2->aliasqual == (int)Cyc_Absyn_Aliasable)
outlives_constraints=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));
({struct _tuple0*_tmp447=({struct _tuple0*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=Cyc_Absyn_empty_effect,_tmpA4->f2=vartype;_tmpA4;});_tmpA5->hd=_tmp447;}),_tmpA5->tl=outlives_constraints;_tmpA5;});else{
# 460
({void*_tmpA6=0U;({struct _fat_ptr _tmp448=({const char*_tmpA7="opened existential had unique or top region kind";_tag_fat(_tmpA7,sizeof(char),49U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp448,_tag_fat(_tmpA6,sizeof(void*),0U));});});}}};};}}
# 464
_tmp8F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8F);{
# 466
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmpF3);
# 468
struct Cyc_List_List*_tmpA9=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmpAA=({struct _tuple12 _tmp390;_tmp390.f1=_tmpA9,_tmp390.f2=rgn;_tmp390;});
struct Cyc_List_List*_tmpAB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpAA,_tmpF4->tvs);
struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF4->impl))->exist_vars,_tmp8F);
struct Cyc_List_List*_tmpAD=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpAB);
struct Cyc_List_List*_tmpAE=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpAC);
struct Cyc_List_List*_tmpAF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpAB,_tmpAC);
# 476
if(_tmpF3 != 0 ||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF4->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp449=({struct _tuple1*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->f1=0,_tmpB0->f2=0;_tmpB0;});res.tvars_and_bounds_opt=_tmp449;});
({struct Cyc_List_List*_tmp44B=({
struct Cyc_List_List*_tmp44A=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp44A,((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmpF3));});
# 479
(*res.tvars_and_bounds_opt).f1=_tmp44B;});
# 481
({struct Cyc_List_List*_tmp44C=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 481
(*res.tvars_and_bounds_opt).f2=_tmp44C;});{
# 483
struct Cyc_List_List*_tmpB1=0;
{struct Cyc_List_List*_tmpB2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF4->impl))->rgn_po;for(0;_tmpB2 != 0;_tmpB2=_tmpB2->tl){
_tmpB1=({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct _tuple0*_tmp44F=({struct _tuple0*_tmpB3=_cycalloc(sizeof(*_tmpB3));({void*_tmp44E=Cyc_Tcutil_rsubstitute(rgn,_tmpAF,(*((struct _tuple0*)_tmpB2->hd)).f1);_tmpB3->f1=_tmp44E;}),({
void*_tmp44D=Cyc_Tcutil_rsubstitute(rgn,_tmpAF,(*((struct _tuple0*)_tmpB2->hd)).f2);_tmpB3->f2=_tmp44D;});_tmpB3;});
# 485
_tmpB4->hd=_tmp44F;}),_tmpB4->tl=_tmpB1;_tmpB4;});}}
# 488
_tmpB1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB1);
({struct Cyc_List_List*_tmp450=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmpB1);
# 489
(*res.tvars_and_bounds_opt).f2=_tmp450;});};}
# 494
t=({union Cyc_Absyn_AggrInfo _tmp451=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpB5=_cycalloc(sizeof(*_tmpB5));*_tmpB5=_tmpF4;_tmpB5;}));Cyc_Absyn_aggr_type(_tmp451,_tmpAD);});
if(_tmpF1 &&(int)_tmpF4->kind == (int)Cyc_Absyn_UnionA)
({void*_tmpB6=0U;({unsigned _tmp453=p->loc;struct _fat_ptr _tmp452=({const char*_tmpB7="`...' pattern not allowed in union pattern";_tag_fat(_tmpB7,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp453,_tmp452,_tag_fat(_tmpB6,sizeof(void*),0U));});});else{
if(_tmpF1){
# 499
int _tmpB8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8A);
int _tmpB9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF4->impl))->fields);
if(_tmpB8 < _tmpB9){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmpB9 - _tmpB8;++ i){
wild_dps=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct _tuple18*_tmp455=({struct _tuple18*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=0,({struct Cyc_Absyn_Pat*_tmp454=Cyc_Tcpat_wild_pat(p->loc);_tmpBA->f2=_tmp454;});_tmpBA;});_tmpBB->hd=_tmp455;}),_tmpBB->tl=wild_dps;_tmpBB;});}}
({struct Cyc_List_List*_tmp456=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8A,wild_dps);*_tmpF2=_tmp456;});
_tmp8A=*_tmpF2;}else{
if(_tmpB8 == _tmpB9)
({void*_tmpBC=0U;({unsigned _tmp458=p->loc;struct _fat_ptr _tmp457=({const char*_tmpBD="unnecessary ... in struct pattern";_tag_fat(_tmpBD,sizeof(char),34U);});Cyc_Tcutil_warn(_tmp458,_tmp457,_tag_fat(_tmpBC,sizeof(void*),0U));});});}}}{
# 510
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp8A,_tmpF4->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF4->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmpBE=(struct _tuple19*)fields->hd;struct _tuple19*_tmpBF=_tmpBE;struct Cyc_Absyn_Aggrfield*_tmpC8;struct Cyc_Absyn_Pat*_tmpC7;_LL62: _tmpC8=_tmpBF->f1;_tmpC7=_tmpBF->f2;_LL63:;{
void*_tmpC0=Cyc_Tcutil_rsubstitute(rgn,_tmpAF,_tmpC8->type);
# 516
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmpC8->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp459=res;Cyc_Tcpat_combine_results(_tmp459,Cyc_Tcpat_tcPatRec(te2,_tmpC7,& _tmpC0,rgn_pat,allow_ref_pat,new_access_exp));});
# 524
if(!Cyc_Unify_unify((void*)_check_null(_tmpC7->topt),_tmpC0))
({struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U,_tmp38F.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmpC8->name);_tmp38F;});struct Cyc_String_pa_PrintArg_struct _tmpC4=({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0U,_tmp38E.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp38E;});struct Cyc_String_pa_PrintArg_struct _tmpC5=({struct Cyc_String_pa_PrintArg_struct _tmp38D;_tmp38D.tag=0U,({
struct _fat_ptr _tmp45A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmpC0));_tmp38D.f1=_tmp45A;});_tmp38D;});struct Cyc_String_pa_PrintArg_struct _tmpC6=({struct Cyc_String_pa_PrintArg_struct _tmp38C;_tmp38C.tag=0U,({
struct _fat_ptr _tmp45B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpC7->topt)));_tmp38C.f1=_tmp45B;});_tmp38C;});void*_tmpC1[4U];_tmpC1[0]=& _tmpC3,_tmpC1[1]=& _tmpC4,_tmpC1[2]=& _tmpC5,_tmpC1[3]=& _tmpC6;({unsigned _tmp45D=p->loc;struct _fat_ptr _tmp45C=({const char*_tmpC2="field %s of %s pattern expects type %s != %s";_tag_fat(_tmpC2,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp45D,_tmp45C,_tag_fat(_tmpC1,sizeof(void*),4U));});});
if((unsigned)new_access_exp)new_access_exp->topt=_tmpC7->topt;};}};};}
# 420
;_pop_region(rgn);}
# 531
goto _LL0;}}else{_LL25: _LL26:
# 584
 goto _LL28;}}else{_LL23: _LL24:
# 583
 goto _LL26;}case 8U: _LL21: _tmpF8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpF7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_tmpF6=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f3;_tmpF5=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f4;_LL22: {
# 534
struct Cyc_List_List*_tmpCB=*_tmpF6;
struct Cyc_List_List*tqts=_tmpF7->typs;
# 537
struct Cyc_List_List*_tmpCC=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmpCD=({struct _tuple12 _tmp397;_tmp397.f1=_tmpCC,_tmp397.f2=Cyc_Core_heap_region;_tmp397;});
struct Cyc_List_List*_tmpCE=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmpCD,_tmpF8->tvs);
struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpCE);
t=({union Cyc_Absyn_DatatypeFieldInfo _tmp45E=Cyc_Absyn_KnownDatatypefield(_tmpF8,_tmpF7);Cyc_Absyn_datatype_field_type(_tmp45E,_tmpCF);});
if(_tmpF5){
# 544
int _tmpD0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCB);
int _tmpD1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmpD0 < _tmpD1){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpD1 - _tmpD0;++ i){
wild_ps=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));({struct Cyc_Absyn_Pat*_tmp45F=Cyc_Tcpat_wild_pat(p->loc);_tmpD2->hd=_tmp45F;}),_tmpD2->tl=wild_ps;_tmpD2;});}}
({struct Cyc_List_List*_tmp460=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCB,wild_ps);*_tmpF6=_tmp460;});
_tmpCB=*_tmpF6;}else{
if(_tmpD0 == _tmpD1)
({struct Cyc_String_pa_PrintArg_struct _tmpD5=({struct Cyc_String_pa_PrintArg_struct _tmp391;_tmp391.tag=0U,({
struct _fat_ptr _tmp461=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF8->name));_tmp391.f1=_tmp461;});_tmp391;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD5;({unsigned _tmp463=p->loc;struct _fat_ptr _tmp462=({const char*_tmpD4="unnecessary ... in datatype field %s";_tag_fat(_tmpD4,sizeof(char),37U);});Cyc_Tcutil_warn(_tmp463,_tmp462,_tag_fat(_tmpD3,sizeof(void*),1U));});});}}
# 556
for(0;_tmpCB != 0 && tqts != 0;(_tmpCB=_tmpCB->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpD6=(struct Cyc_Absyn_Pat*)_tmpCB->hd;
# 560
void*_tmpD7=Cyc_Tcutil_substitute(_tmpCE,(*((struct _tuple17*)tqts->hd)).f2);
# 563
if((unsigned)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
res=({struct Cyc_Tcpat_TcPatResult _tmp464=res;Cyc_Tcpat_combine_results(_tmp464,Cyc_Tcpat_tcPatRec(te,_tmpD6,& _tmpD7,rgn_pat,allow_ref_pat,0));});
# 570
if(!Cyc_Unify_unify((void*)_check_null(_tmpD6->topt),_tmpD7))
({struct Cyc_String_pa_PrintArg_struct _tmpDA=({struct Cyc_String_pa_PrintArg_struct _tmp394;_tmp394.tag=0U,({
struct _fat_ptr _tmp465=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF7->name));_tmp394.f1=_tmp465;});_tmp394;});struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp393;_tmp393.tag=0U,({struct _fat_ptr _tmp466=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmpD7));_tmp393.f1=_tmp466;});_tmp393;});struct Cyc_String_pa_PrintArg_struct _tmpDC=({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0U,({
struct _fat_ptr _tmp467=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpD6->topt)));_tmp392.f1=_tmp467;});_tmp392;});void*_tmpD8[3U];_tmpD8[0]=& _tmpDA,_tmpD8[1]=& _tmpDB,_tmpD8[2]=& _tmpDC;({unsigned _tmp469=_tmpD6->loc;struct _fat_ptr _tmp468=({const char*_tmpD9="%s expects argument type %s, not %s";_tag_fat(_tmpD9,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp469,_tmp468,_tag_fat(_tmpD8,sizeof(void*),3U));});});}
# 575
if(_tmpCB != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDF=({struct Cyc_String_pa_PrintArg_struct _tmp395;_tmp395.tag=0U,({
struct _fat_ptr _tmp46A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF7->name));_tmp395.f1=_tmp46A;});_tmp395;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({unsigned _tmp46C=p->loc;struct _fat_ptr _tmp46B=({const char*_tmpDE="too many arguments for datatype constructor %s";_tag_fat(_tmpDE,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp46C,_tmp46B,_tag_fat(_tmpDD,sizeof(void*),1U));});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpE2=({struct Cyc_String_pa_PrintArg_struct _tmp396;_tmp396.tag=0U,({
struct _fat_ptr _tmp46D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF7->name));_tmp396.f1=_tmp46D;});_tmp396;});void*_tmpE0[1U];_tmpE0[0]=& _tmpE2;({unsigned _tmp46F=p->loc;struct _fat_ptr _tmp46E=({const char*_tmpE1="too few arguments for datatype constructor %s";_tag_fat(_tmpE1,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp46F,_tmp46E,_tag_fat(_tmpE0,sizeof(void*),1U));});});
goto _LL0;}case 15U: _LL27: _LL28:
# 585
 goto _LL2A;case 17U: _LL29: _LL2A:
 goto _LL2C;default: _LL2B: _LL2C:
# 588
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL0;}_LL0:;}
# 590
tcpat_end:
 p->topt=t;
return res;};}
# 595
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 597
struct Cyc_Tcpat_TcPatResult _tmpF9=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 599
struct _tuple1 _tmpFA=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpF9.patvars);struct _tuple1 _tmpFB=_tmpFA;struct Cyc_List_List*_tmp103;_LL1: _tmp103=_tmpFB.f1;_LL2:;{
struct Cyc_List_List*_tmpFC=0;
{struct Cyc_List_List*x=_tmp103;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)_tmpFC=({struct Cyc_List_List*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),_tmpFD->tl=_tmpFC;_tmpFD;});}}
({struct Cyc_List_List*_tmp471=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_name,_tmpFC);unsigned _tmp470=p->loc;Cyc_Tcutil_check_unique_vars(_tmp471,_tmp470,({const char*_tmpFE="pattern contains a repeated variable";_tag_fat(_tmpFE,sizeof(char),37U);}));});
# 608
{struct Cyc_List_List*x=_tmpF9.patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmpFF=(struct _tuple15*)x->hd;struct _tuple15*_tmp100=_tmpFF;struct Cyc_Absyn_Vardecl**_tmp102;struct Cyc_Absyn_Exp**_tmp101;_LL4: _tmp102=_tmp100->f1;_tmp101=(struct Cyc_Absyn_Exp**)& _tmp100->f2;_LL5:;
if(*_tmp101 != 0 &&*_tmp101 != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp472=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp101));*_tmp101=_tmp472;});}}
# 613
return _tmpF9;};}
# 619
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 622
struct _tuple0 _tmp104=({struct _tuple0 _tmp399;({void*_tmp474=Cyc_Tcutil_compress(old_type);_tmp399.f1=_tmp474;}),({void*_tmp473=Cyc_Tcutil_compress(new_type);_tmp399.f2=_tmp473;});_tmp399;});struct _tuple0 _tmp105=_tmp104;struct Cyc_Absyn_PtrInfo _tmp109;struct Cyc_Absyn_PtrInfo _tmp108;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105.f2)->tag == 3U){_LL1: _tmp109=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105.f1)->f1;_tmp108=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105.f2)->f1;_LL2: {
# 624
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp106=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->tag=3U,(_tmp107->f1).elt_type=_tmp109.elt_type,(_tmp107->f1).elt_tq=_tmp108.elt_tq,
((_tmp107->f1).ptr_atts).rgn=(_tmp109.ptr_atts).rgn,((_tmp107->f1).ptr_atts).nullable=(_tmp108.ptr_atts).nullable,((_tmp107->f1).ptr_atts).bounds=(_tmp108.ptr_atts).bounds,((_tmp107->f1).ptr_atts).zero_term=(_tmp108.ptr_atts).zero_term,((_tmp107->f1).ptr_atts).ptrloc=(_tmp109.ptr_atts).ptrloc;_tmp107;});
# 630
return({struct Cyc_RgnOrder_RgnPO*_tmp479=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp478=assump;void*_tmp477=(void*)_tmp106;Cyc_Tcutil_subtype(_tmp479,_tmp478,_tmp477,new_type);})&&({
struct Cyc_RgnOrder_RgnPO*_tmp476=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_Absyn_Exp*_tmp475=initializer;Cyc_Tcutil_coerce_assign(_tmp476,_tmp475,(void*)_tmp106);});}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 639
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 642
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));({struct _tuple0*_tmp47B=({struct _tuple0*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->f1=Cyc_Absyn_unique_rgn_type,({void*_tmp47A=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->tag=2U,_tmp114->f1=tv;_tmp114;});_tmp115->f2=_tmp47A;});_tmp115;});_tmp116->hd=_tmp47B;}),_tmp116->tl=0;_tmp116;});
if(({struct Cyc_RgnOrder_RgnPO*_tmp47E=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp47D=assump;void*_tmp47C=old_type;Cyc_Tcutil_subtype(_tmp47E,_tmp47D,_tmp47C,new_type);})){
# 660 "tcpat.cyc"
struct _tuple0 _tmp10A=({struct _tuple0 _tmp39A;({void*_tmp480=Cyc_Tcutil_compress(old_type);_tmp39A.f1=_tmp480;}),({void*_tmp47F=Cyc_Tcutil_compress(new_type);_tmp39A.f2=_tmp47F;});_tmp39A;});struct _tuple0 _tmp10B=_tmp10A;struct Cyc_Absyn_PtrInfo _tmp10F;struct Cyc_Absyn_PtrInfo _tmp10E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10B.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10B.f2)->tag == 3U){_LL1: _tmp10F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10B.f1)->f1;_tmp10E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10B.f2)->f1;_LL2:
 goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp10C=0U;({unsigned _tmp482=loc;struct _fat_ptr _tmp481=({const char*_tmp10D="alias requires pointer type";_tag_fat(_tmp10D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp482,_tmp481,_tag_fat(_tmp10C,sizeof(void*),0U));});});goto _LL0;}_LL0:;}else{
# 665
({struct Cyc_String_pa_PrintArg_struct _tmp112=({struct Cyc_String_pa_PrintArg_struct _tmp39C;_tmp39C.tag=0U,({
struct _fat_ptr _tmp483=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(old_type));_tmp39C.f1=_tmp483;});_tmp39C;});struct Cyc_String_pa_PrintArg_struct _tmp113=({struct Cyc_String_pa_PrintArg_struct _tmp39B;_tmp39B.tag=0U,({struct _fat_ptr _tmp484=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(new_type));_tmp39B.f1=_tmp484;});_tmp39B;});void*_tmp110[2U];_tmp110[0]=& _tmp112,_tmp110[1]=& _tmp113;({unsigned _tmp486=loc;struct _fat_ptr _tmp485=({const char*_tmp111="cannot alias value of type %s to type %s";_tag_fat(_tmp111,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp486,_tmp485,_tag_fat(_tmp110,sizeof(void*),2U));});});}}
# 672
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 676
void*_tmp117=p->r;void*_tmp118=_tmp117;struct Cyc_Absyn_Tvar*_tmp140;struct Cyc_Absyn_Vardecl*_tmp13F;struct Cyc_Absyn_Vardecl*_tmp13E;struct Cyc_Absyn_Pat*_tmp13D;struct Cyc_Absyn_Vardecl*_tmp13C;struct Cyc_Absyn_Pat*_tmp13B;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*_tmp139;union Cyc_Absyn_AggrInfo*_tmp138;struct Cyc_List_List*_tmp137;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_Pat*_tmp135;switch(*((int*)_tmp118)){case 6U: _LL1: _tmp135=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp118)->f1;_LL2: {
# 678
void*_tmp119=(void*)_check_null(p->topt);void*_tmp11A=_tmp119;void*_tmp11D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11A)->tag == 3U){_LL12: _tmp11D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11A)->f1).ptr_atts).rgn;_LL13:
# 680
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp11D);
({struct Cyc_Tcenv_Tenv*_tmp489=te;struct Cyc_Absyn_Pat*_tmp488=_tmp135;int _tmp487=Cyc_Tcutil_is_noalias_region(_tmp11D,0);Cyc_Tcpat_check_pat_regions_rec(_tmp489,_tmp488,_tmp487,patvars);});
return;}else{_LL14: _LL15:
({void*_tmp11B=0U;({struct _fat_ptr _tmp48A=({const char*_tmp11C="check_pat_regions: bad pointer type";_tag_fat(_tmp11C,sizeof(char),36U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp48A,_tag_fat(_tmp11B,sizeof(void*),0U));});});}_LL11:;}case 7U: _LL3: _tmp138=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp118)->f1;_tmp137=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp118)->f2;_tmp136=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp118)->f3;_LL4:
# 686
 for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple18*)_tmp136->hd)).f2,did_noalias_deref,patvars);}
return;case 8U: _LL5: _tmp139=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp118)->f3;_LL6:
# 690
 did_noalias_deref=0;_tmp13A=_tmp139;goto _LL8;case 5U: _LL7: _tmp13A=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp118)->f1;_LL8:
# 692
 for(0;_tmp13A != 0;_tmp13A=_tmp13A->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp13A->hd,did_noalias_deref,patvars);}
return;case 3U: _LL9: _tmp13C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp118)->f1;_tmp13B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp118)->f2;_LLA:
# 696
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp11E=(struct _tuple15*)x->hd;struct _tuple15*_tmp11F=_tmp11E;struct Cyc_Absyn_Vardecl**_tmp128;struct Cyc_Absyn_Exp*_tmp127;_LL17: _tmp128=_tmp11F->f1;_tmp127=_tmp11F->f2;_LL18:;
# 702
if((_tmp128 != 0 &&*_tmp128 == _tmp13C)&& _tmp127 != 0){
{void*_tmp120=_tmp127->r;void*_tmp121=_tmp120;struct Cyc_Absyn_Exp*_tmp126;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp121)->tag == 15U){_LL1A: _tmp126=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp121)->f1;_LL1B:
# 705
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp126->topt)))
({void*_tmp122=0U;({unsigned _tmp48C=p->loc;struct _fat_ptr _tmp48B=({const char*_tmp123="reference pattern not allowed on alias-free pointers";_tag_fat(_tmp123,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp48C,_tmp48B,_tag_fat(_tmp122,sizeof(void*),0U));});});
goto _LL19;}else{_LL1C: _LL1D:
# 709
({void*_tmp124=0U;({struct _fat_ptr _tmp48D=({const char*_tmp125="check_pat_regions: bad reference access expression";_tag_fat(_tmp125,sizeof(char),51U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp48D,_tag_fat(_tmp124,sizeof(void*),0U));});});}_LL19:;}
# 711
break;}}}
# 714
Cyc_Tcpat_check_pat_regions_rec(te,_tmp13B,did_noalias_deref,patvars);
return;case 1U: _LLB: _tmp13E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp118)->f1;_tmp13D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp118)->f2;_LLC:
# 717
{void*_tmp129=p->topt;void*_tmp12A=_tmp129;if(_tmp12A != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12A)->tag == 4U){_LL1F: _LL20:
# 719
 if(did_noalias_deref){
({void*_tmp12B=0U;({unsigned _tmp48F=p->loc;struct _fat_ptr _tmp48E=({const char*_tmp12C="pattern to array would create alias of no-alias pointer";_tag_fat(_tmp12C,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp48F,_tmp48E,_tag_fat(_tmp12B,sizeof(void*),0U));});});
return;}
# 723
goto _LL1E;}else{goto _LL21;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 726
Cyc_Tcpat_check_pat_regions_rec(te,_tmp13D,did_noalias_deref,patvars);
return;case 2U: _LLD: _tmp140=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp118)->f1;_tmp13F=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp118)->f2;_LLE:
# 729
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp12D=(struct _tuple15*)x->hd;struct _tuple15*_tmp12E=_tmp12D;struct Cyc_Absyn_Vardecl**_tmp134;struct Cyc_Absyn_Exp*_tmp133;_LL24: _tmp134=_tmp12E->f1;_tmp133=_tmp12E->f2;_LL25:;
# 733
if(_tmp134 != 0 &&*_tmp134 == _tmp13F){
if(_tmp133 == 0)
({void*_tmp12F=0U;({unsigned _tmp491=p->loc;struct _fat_ptr _tmp490=({const char*_tmp130="cannot alias pattern expression in datatype";_tag_fat(_tmp130,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp491,_tmp490,_tag_fat(_tmp12F,sizeof(void*),0U));});});else{
# 737
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp493=p->loc;struct Cyc_Tcenv_Tenv*_tmp492=te;Cyc_Tcenv_add_type_vars(_tmp493,_tmp492,({struct Cyc_Absyn_Tvar*_tmp132[1U];_tmp132[0]=_tmp140;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp132,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
te2=({struct Cyc_Tcenv_Tenv*_tmp494=te2;Cyc_Tcenv_add_region(_tmp494,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=2U,_tmp131->f1=_tmp140;_tmp131;}),0);});
# 740
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp133->topt),_tmp140,_tmp13F->type,_tmp133);}
# 743
break;}}}
# 746
goto _LL0;default: _LLF: _LL10:
 return;}_LL0:;}
# 762 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 764
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp141=(struct _tuple15*)x->hd;struct _tuple15*_tmp142=_tmp141;struct Cyc_Absyn_Vardecl**_tmp14C;struct Cyc_Absyn_Exp*_tmp14B;_LL1: _tmp14C=_tmp142->f1;_tmp14B=_tmp142->f2;_LL2:;
if(_tmp14B != 0){
struct Cyc_Absyn_Exp*_tmp143=_tmp14B;
# 771
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp143->topt))&& !Cyc_Tcutil_is_noalias_path(_tmp143))
# 773
({struct Cyc_String_pa_PrintArg_struct _tmp146=({struct Cyc_String_pa_PrintArg_struct _tmp39D;_tmp39D.tag=0U,({
# 775
struct _fat_ptr _tmp497=(struct _fat_ptr)(_tmp14C != 0?(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp149=({struct Cyc_String_pa_PrintArg_struct _tmp39E;_tmp39E.tag=0U,({
# 777
struct _fat_ptr _tmp495=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string((*_tmp14C)->name));_tmp39E.f1=_tmp495;});_tmp39E;});void*_tmp147[1U];_tmp147[0]=& _tmp149;({struct _fat_ptr _tmp496=({const char*_tmp148="for variable %s";_tag_fat(_tmp148,sizeof(char),16U);});Cyc_aprintf(_tmp496,_tag_fat(_tmp147,sizeof(void*),1U));});}):({const char*_tmp14A="";_tag_fat(_tmp14A,sizeof(char),1U);}));
# 775
_tmp39D.f1=_tmp497;});_tmp39D;});void*_tmp144[1U];_tmp144[0]=& _tmp146;({unsigned _tmp499=p->loc;struct _fat_ptr _tmp498=({const char*_tmp145="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_fat(_tmp145,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp499,_tmp498,_tag_fat(_tmp144,sizeof(void*),1U));});});}}};}
# 831 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1U};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2U};
# 841
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0U};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1U};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 855
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){return({union Cyc_Tcpat_Name_value _tmp39F;(_tmp39F.Name_v).tag=1U,(_tmp39F.Name_v).val=s;_tmp39F;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp3A0;(_tmp3A0.Int_v).tag=2U,(_tmp3A0.Int_v).val=i;_tmp3A0;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 872
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp14D=c1->name;union Cyc_Tcpat_Name_value _tmp14E=_tmp14D;int _tmp156;struct _fat_ptr _tmp155;if((_tmp14E.Name_v).tag == 1){_LL1: _tmp155=(_tmp14E.Name_v).val;_LL2: {
# 875
union Cyc_Tcpat_Name_value _tmp14F=c2->name;union Cyc_Tcpat_Name_value _tmp150=_tmp14F;struct _fat_ptr _tmp151;if((_tmp150.Name_v).tag == 1){_LL6: _tmp151=(_tmp150.Name_v).val;_LL7:
 return Cyc_strcmp((struct _fat_ptr)_tmp155,(struct _fat_ptr)_tmp151);}else{_LL8: _LL9:
 return - 1;}_LL5:;}}else{_LL3: _tmp156=(_tmp14E.Int_v).val;_LL4: {
# 880
union Cyc_Tcpat_Name_value _tmp152=c2->name;union Cyc_Tcpat_Name_value _tmp153=_tmp152;int _tmp154;if((_tmp153.Name_v).tag == 1){_LLB: _LLC:
 return 1;}else{_LLD: _tmp154=(_tmp153.Int_v).val;_LLE:
 return _tmp156 - _tmp154;}_LLA:;}}_LL0:;}
# 888
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 892
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 896
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned ans=0U;
struct Cyc_List_List*_tmp157=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp157))->hd)->name)!= 0){
++ ans;
_tmp157=_tmp157->tl;}
# 903
return ans;}
# 906
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp158=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp158 != 0;_tmp158=_tmp158->tl){
struct Cyc_Absyn_Aggrfield*_tmp159=(struct Cyc_Absyn_Aggrfield*)_tmp158->hd;
if(Cyc_strcmp((struct _fat_ptr)*_tmp159->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 913
({void*_tmp15A=0U;({struct _fat_ptr _tmp49B=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp3A1;_tmp3A1.tag=0U,_tmp3A1.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp3A1;});void*_tmp15B[1U];_tmp15B[0]=& _tmp15D;({struct _fat_ptr _tmp49A=({const char*_tmp15C="get_member_offset %s failed";_tag_fat(_tmp15C,sizeof(char),28U);});Cyc_aprintf(_tmp49A,_tag_fat(_tmp15B,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp49B,_tag_fat(_tmp15A,sizeof(void*),0U));});});}
# 916
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp15E=pw;struct Cyc_Absyn_Pat*_tmp18D;struct Cyc_Absyn_Exp*_tmp18C;if((_tmp15E.where_clause).tag == 2){_LL1: _tmp18C=(_tmp15E.where_clause).val;_LL2:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->tag=0U,_tmp15F->f1=_tmp18C;_tmp15F;});}else{_LL3: _tmp18D=(_tmp15E.pattern).val;_LL4: {
# 920
void*_tmp160=_tmp18D->r;void*_tmp161=_tmp160;union Cyc_Absyn_AggrInfo _tmp18B;struct Cyc_List_List*_tmp18A;struct Cyc_Absyn_Datatypedecl*_tmp189;struct Cyc_Absyn_Datatypefield*_tmp188;void*_tmp187;struct Cyc_Absyn_Enumfield*_tmp186;struct Cyc_Absyn_Enumdecl*_tmp185;struct Cyc_Absyn_Enumfield*_tmp184;struct _fat_ptr _tmp183;int _tmp182;char _tmp181;enum Cyc_Absyn_Sign _tmp180;int _tmp17F;struct Cyc_Absyn_Pat*_tmp17E;struct Cyc_Absyn_Pat*_tmp17D;switch(*((int*)_tmp161)){case 1U: _LL6: _tmp17D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp161)->f2;_LL7:
 _tmp17E=_tmp17D;goto _LL9;case 3U: _LL8: _tmp17E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp161)->f2;_LL9:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp3A2;(_tmp3A2.pattern).tag=1U,(_tmp3A2.pattern).val=_tmp17E;_tmp3A2;}));case 9U: _LLA: _LLB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10U: _LLC: _tmp180=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp161)->f1;_tmp17F=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp161)->f2;_LLD:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->tag=6U,_tmp162->f1=(unsigned)_tmp17F;_tmp162;});case 11U: _LLE: _tmp181=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp161)->f1;_LLF:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->tag=6U,_tmp163->f1=(unsigned)_tmp181;_tmp163;});case 12U: _LL10: _tmp183=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp161)->f1;_tmp182=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp161)->f2;_LL11:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->tag=5U,_tmp164->f1=_tmp183,_tmp164->f2=_tmp182;_tmp164;});case 13U: _LL12: _tmp185=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp161)->f1;_tmp184=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp161)->f2;_LL13:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=3U,_tmp165->f1=_tmp185,_tmp165->f2=_tmp184;_tmp165;});case 14U: _LL14: _tmp187=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp161)->f1;_tmp186=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp161)->f2;_LL15:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->tag=4U,_tmp166->f1=_tmp187,_tmp166->f2=_tmp186;_tmp166;});case 6U: _LL16: _LL17:
# 930
{void*_tmp167=Cyc_Tcutil_compress((void*)_check_null(_tmp18D->topt));void*_tmp168=_tmp167;void*_tmp169;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168)->tag == 3U){_LL1F: _tmp169=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168)->f1).ptr_atts).nullable;_LL20:
# 932
 if(Cyc_Tcutil_force_type2bool(0,_tmp169))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 937
({void*_tmp16A=0U;({struct _fat_ptr _tmp49C=({const char*_tmp16B="non-null pointer type or non-pointer type in pointer pattern";_tag_fat(_tmp16B,sizeof(char),61U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp49C,_tag_fat(_tmp16A,sizeof(void*),0U));});});case 8U: _LL18: _tmp189=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp161)->f1;_tmp188=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp161)->f2;_LL19:
# 939
 if(_tmp189->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->tag=9U,_tmp16C->f1=_tmp189,_tmp16C->f2=_tmp188;_tmp16C;});else{
# 942
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->tag=7U,({int _tmp49D=(int)Cyc_Tcpat_datatype_tag_number(_tmp189,_tmp188->name);_tmp16D->f1=_tmp49D;}),_tmp16D->f2=_tmp189,_tmp16D->f3=_tmp188;_tmp16D;});}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp161)->f1 != 0){_LL1A: _tmp18B=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp161)->f1;_tmp18A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp161)->f3;_LL1B: {
# 944
struct Cyc_Absyn_Aggrdecl*_tmp16E=Cyc_Absyn_get_known_aggrdecl(_tmp18B);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp16E->impl))->tagged){
struct _tuple18*_tmp16F=(struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp18A))->hd;struct _tuple18*_tmp170=_tmp16F;struct Cyc_List_List*_tmp178;struct Cyc_Absyn_Pat*_tmp177;_LL24: _tmp178=_tmp170->f1;_tmp177=_tmp170->f2;_LL25:;{
struct _fat_ptr*f;
{void*_tmp171=(void*)((struct Cyc_List_List*)_check_null(_tmp178))->hd;void*_tmp172=_tmp171;struct _fat_ptr*_tmp175;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp172)->tag == 1U){_LL27: _tmp175=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp172)->f1;_LL28:
 f=_tmp175;goto _LL26;}else{_LL29: _LL2A:
({void*_tmp173=0U;({struct _fat_ptr _tmp49E=({const char*_tmp174="no field name in tagged union pattern";_tag_fat(_tmp174,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp49E,_tag_fat(_tmp173,sizeof(void*),0U));});});}_LL26:;}
# 952
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->tag=8U,_tmp176->f1=f,({int _tmp49F=Cyc_Tcpat_get_member_offset(_tmp16E,f);_tmp176->f2=_tmp49F;});_tmp176;});};}else{
# 954
({void*_tmp179=0U;({struct _fat_ptr _tmp4A0=({const char*_tmp17A="non-tagged aggregate in pattern test";_tag_fat(_tmp17A,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4A0,_tag_fat(_tmp179,sizeof(void*),0U));});});}}}else{goto _LL1C;}default: _LL1C: _LL1D:
({void*_tmp17B=0U;({struct _fat_ptr _tmp4A1=({const char*_tmp17C="non-test pattern in pattern test";_tag_fat(_tmp17C,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4A1,_tag_fat(_tmp17B,sizeof(void*),0U));});});}_LL5:;}}_LL0:;}
# 960
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp3A3;(_tmp3A3.pattern).tag=1U,(_tmp3A3.pattern).val=p;_tmp3A3;});}
# 964
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18F=_cycalloc(sizeof(*_tmp18F));({union Cyc_Tcpat_Name_value _tmp4A3=Cyc_Tcpat_Name_v(({const char*_tmp18E="NULL";_tag_fat(_tmp18E,sizeof(char),5U);}));_tmp18F->name=_tmp4A3;}),_tmp18F->arity=0,_tmp18F->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4A2=Cyc_Tcpat_pw(p);_tmp18F->orig_pat=_tmp4A2;});_tmp18F;});}
# 967
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp191=_cycalloc(sizeof(*_tmp191));({union Cyc_Tcpat_Name_value _tmp4A5=Cyc_Tcpat_Name_v(({const char*_tmp190="&";_tag_fat(_tmp190,sizeof(char),2U);}));_tmp191->name=_tmp4A5;}),_tmp191->arity=1,_tmp191->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4A4=Cyc_Tcpat_pw(p);_tmp191->orig_pat=_tmp4A4;});_tmp191;});}
# 970
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp193=_cycalloc(sizeof(*_tmp193));({union Cyc_Tcpat_Name_value _tmp4A7=Cyc_Tcpat_Name_v(({const char*_tmp192="&";_tag_fat(_tmp192,sizeof(char),2U);}));_tmp193->name=_tmp4A7;}),_tmp193->arity=1,_tmp193->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _tmp4A6=Cyc_Tcpat_pw(p);_tmp193->orig_pat=_tmp4A6;});_tmp193;});}
# 973
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp194=_cycalloc(sizeof(*_tmp194));({union Cyc_Tcpat_Name_value _tmp4A8=Cyc_Tcpat_Int_v(i);_tmp194->name=_tmp4A8;}),_tmp194->arity=0,_tmp194->span=0,_tmp194->orig_pat=p;_tmp194;});}
# 976
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp195=_cycalloc(sizeof(*_tmp195));({union Cyc_Tcpat_Name_value _tmp4AA=Cyc_Tcpat_Name_v(f);_tmp195->name=_tmp4AA;}),_tmp195->arity=0,_tmp195->span=0,({union Cyc_Tcpat_PatOrWhere _tmp4A9=Cyc_Tcpat_pw(p);_tmp195->orig_pat=_tmp4A9;});_tmp195;});}
# 979
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp196=_cycalloc(sizeof(*_tmp196));({union Cyc_Tcpat_Name_value _tmp4AC=Cyc_Tcpat_Int_v((int)c);_tmp196->name=_tmp4AC;}),_tmp196->arity=0,_tmp196->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _tmp4AB=Cyc_Tcpat_pw(p);_tmp196->orig_pat=_tmp4AB;});_tmp196;});}
# 982
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp198=_cycalloc(sizeof(*_tmp198));({union Cyc_Tcpat_Name_value _tmp4AD=Cyc_Tcpat_Name_v(({const char*_tmp197="$";_tag_fat(_tmp197,sizeof(char),2U);}));_tmp198->name=_tmp4AD;}),_tmp198->arity=i,_tmp198->span=& Cyc_Tcpat_one_opt,_tmp198->orig_pat=p;_tmp198;});}
# 987
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4AE=Cyc_Tcpat_null_con(p);_tmp199->f1=_tmp4AE;}),_tmp199->f2=0;_tmp199;});}
# 990
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4AF=Cyc_Tcpat_int_con(i,p);_tmp19A->f1=_tmp4AF;}),_tmp19A->f2=0;_tmp19A;});}
# 993
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4B0=Cyc_Tcpat_char_con(c,p);_tmp19B->f1=_tmp4B0;}),_tmp19B->f2=0;_tmp19B;});}
# 996
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4B1=Cyc_Tcpat_float_con(f,p);_tmp19C->f1=_tmp4B1;}),_tmp19C->f2=0;_tmp19C;});}
# 999
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4B3=Cyc_Tcpat_null_ptr_con(p0);_tmp19E->f1=_tmp4B3;}),({struct Cyc_List_List*_tmp4B2=({struct Cyc_List_List*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->hd=p,_tmp19D->tl=0;_tmp19D;});_tmp19E->f2=_tmp4B2;});_tmp19E;});}
# 1002
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4B5=Cyc_Tcpat_ptr_con(p0);_tmp1A0->f1=_tmp4B5;}),({struct Cyc_List_List*_tmp4B4=({struct Cyc_List_List*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=p,_tmp19F->tl=0;_tmp19F;});_tmp1A0->f2=_tmp4B4;});_tmp1A0;});}
# 1005
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4B7=({int _tmp4B6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp4B6,p);});_tmp1A1->f1=_tmp4B7;}),_tmp1A1->f2=ss;_tmp1A1;});}
# 1008
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1010
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));({union Cyc_Tcpat_Name_value _tmp4BA=Cyc_Tcpat_Name_v(con_name);_tmp1A3->name=_tmp4BA;}),({int _tmp4B9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp1A3->arity=_tmp4B9;}),_tmp1A3->span=span,({union Cyc_Tcpat_PatOrWhere _tmp4B8=Cyc_Tcpat_pw(p);_tmp1A3->orig_pat=_tmp4B8;});_tmp1A3;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=1U,_tmp1A2->f1=c,_tmp1A2->f2=ps;_tmp1A2;});}
# 1015
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1A4=p->r;void*_tmp1A5=_tmp1A4;void*_tmp1DC;struct Cyc_Absyn_Enumfield*_tmp1DB;struct Cyc_Absyn_Enumdecl*_tmp1DA;struct Cyc_Absyn_Enumfield*_tmp1D9;struct Cyc_Absyn_Aggrdecl*_tmp1D8;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*_tmp1D6;struct Cyc_Absyn_Datatypedecl*_tmp1D5;struct Cyc_Absyn_Datatypefield*_tmp1D4;struct Cyc_List_List*_tmp1D3;struct Cyc_Absyn_Pat*_tmp1D2;struct Cyc_Absyn_Pat*_tmp1D1;struct Cyc_Absyn_Pat*_tmp1D0;struct _fat_ptr _tmp1CF;char _tmp1CE;enum Cyc_Absyn_Sign _tmp1CD;int _tmp1CC;switch(*((int*)_tmp1A5)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->tag=0U;_tmp1A6;});goto _LL0;case 9U: _LL7: _LL8:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 10U: _LL9: _tmp1CD=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1;_tmp1CC=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1A5)->f2;_LLA:
 s=({int _tmp4BB=_tmp1CC;Cyc_Tcpat_int_pat(_tmp4BB,Cyc_Tcpat_pw(p));});goto _LL0;case 11U: _LLB: _tmp1CE=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1;_LLC:
 s=Cyc_Tcpat_char_pat(_tmp1CE,p);goto _LL0;case 12U: _LLD: _tmp1CF=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1;_LLE:
 s=Cyc_Tcpat_float_pat(_tmp1CF,p);goto _LL0;case 1U: _LLF: _tmp1D0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1A5)->f2;_LL10:
 s=Cyc_Tcpat_compile_pat(_tmp1D0);goto _LL0;case 3U: _LL11: _tmp1D1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1A5)->f2;_LL12:
 s=Cyc_Tcpat_compile_pat(_tmp1D1);goto _LL0;case 6U: _LL13: _tmp1D2=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1;_LL14:
# 1028
{void*_tmp1A7=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1A8=_tmp1A7;void*_tmp1AB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->tag == 3U){_LL28: _tmp1AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).ptr_atts).nullable;_LL29: {
# 1030
int is_nullable=Cyc_Tcutil_force_type2bool(0,_tmp1AB);
void*ss=Cyc_Tcpat_compile_pat(_tmp1D2);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(ss,p);else{
# 1035
s=Cyc_Tcpat_ptr_pat(ss,p);}
goto _LL27;}}else{_LL2A: _LL2B:
({void*_tmp1A9=0U;({struct _fat_ptr _tmp4BC=({const char*_tmp1AA="expecting pointertype for pattern!";_tag_fat(_tmp1AA,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4BC,_tag_fat(_tmp1A9,sizeof(void*),0U));});});}_LL27:;}
# 1039
goto _LL0;case 8U: _LL15: _tmp1D5=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1;_tmp1D4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A5)->f2;_tmp1D3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A5)->f3;_LL16: {
# 1041
int*span;
{void*_tmp1AC=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1AD=_tmp1AC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AD)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AD)->f1)){case 18U: _LL2D: _LL2E:
# 1044
 if(_tmp1D5->is_extensible)
span=0;else{
# 1047
span=({int*_tmp1AE=_cycalloc_atomic(sizeof(*_tmp1AE));({int _tmp4BD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D5->fields))->v);*_tmp1AE=_tmp4BD;});_tmp1AE;});}
goto _LL2C;case 19U: _LL2F: _LL30:
 span=& Cyc_Tcpat_one_opt;goto _LL2C;default: goto _LL31;}else{_LL31: _LL32:
 span=({void*_tmp1AF=0U;({struct _fat_ptr _tmp4BE=({const char*_tmp1B0="void datatype pattern has bad type";_tag_fat(_tmp1B0,sizeof(char),35U);});((int*(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4BE,_tag_fat(_tmp1AF,sizeof(void*),0U));});});goto _LL2C;}_LL2C:;}
# 1052
s=({struct _fat_ptr _tmp4C1=*(*_tmp1D4->name).f2;int*_tmp4C0=span;struct Cyc_List_List*_tmp4BF=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1D3);Cyc_Tcpat_con_pat(_tmp4C1,_tmp4C0,_tmp4BF,p);});
goto _LL0;}case 5U: _LL17: _tmp1D6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1;_LL18:
# 1056
 s=({struct Cyc_List_List*_tmp4C2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1D6);Cyc_Tcpat_tuple_pat(_tmp4C2,Cyc_Tcpat_pw(p));});goto _LL0;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1)->KnownAggr).tag == 2){_LL19: _tmp1D8=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1)->KnownAggr).val;_tmp1D7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A5)->f3;_LL1A:
# 1061
 if((int)_tmp1D8->kind == (int)Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1D8->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1065
int found=({struct _fat_ptr _tmp4C3=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp4C3,({const char*_tmp1BC="";_tag_fat(_tmp1BC,sizeof(char),1U);}));})== 0;
{struct Cyc_List_List*dlps0=_tmp1D7;for(0;!found && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple18*_tmp1B1=(struct _tuple18*)dlps0->hd;struct _tuple18*_tmp1B2=_tmp1B1;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Pat*_tmp1B8;_LL34: _tmp1B9=_tmp1B2->f1;_tmp1B8=_tmp1B2->f2;_LL35:;{
struct Cyc_List_List*_tmp1B3=_tmp1B9;struct _fat_ptr*_tmp1B7;if(_tmp1B3 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp1B3)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp1B3)->tl == 0){_LL37: _tmp1B7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1B3->hd)->f1;_LL38:
# 1070
 if(Cyc_strptrcmp(_tmp1B7,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));({void*_tmp4C4=Cyc_Tcpat_compile_pat(_tmp1B8);_tmp1B4->hd=_tmp4C4;}),_tmp1B4->tl=ps;_tmp1B4;});
found=1;}
# 1074
goto _LL36;}else{goto _LL39;}}else{goto _LL39;}}else{_LL39: _LL3A:
({void*_tmp1B5=0U;({struct _fat_ptr _tmp4C5=({const char*_tmp1B6="bad designator(s)";_tag_fat(_tmp1B6,sizeof(char),18U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4C5,_tag_fat(_tmp1B5,sizeof(void*),0U));});});}_LL36:;};}}
# 1078
if(!found)
({void*_tmp1BA=0U;({struct _fat_ptr _tmp4C6=({const char*_tmp1BB="bad designator";_tag_fat(_tmp1BB,sizeof(char),15U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4C6,_tag_fat(_tmp1BA,sizeof(void*),0U));});});}}
# 1081
s=({struct Cyc_List_List*_tmp4C7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp4C7,Cyc_Tcpat_pw(p));});}else{
# 1084
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1D8->impl))->tagged)
({void*_tmp1BD=0U;({unsigned _tmp4C9=p->loc;struct _fat_ptr _tmp4C8=({const char*_tmp1BE="patterns on untagged unions not yet supported.";_tag_fat(_tmp1BE,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp4C9,_tmp4C8,_tag_fat(_tmp1BD,sizeof(void*),0U));});});{
int*span=({int*_tmp1C5=_cycalloc_atomic(sizeof(*_tmp1C5));({int _tmp4CA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1D8->impl))->fields);*_tmp1C5=_tmp4CA;});_tmp1C5;});
struct Cyc_List_List*_tmp1BF=_tmp1D7;struct _fat_ptr*_tmp1C4;struct Cyc_Absyn_Pat*_tmp1C3;if(_tmp1BF != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp1BF)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp1BF)->hd)->f1)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp1BF)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp1BF)->tl == 0){_LL3C: _tmp1C4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp1BF->hd)->f1)->hd)->f1;_tmp1C3=((struct _tuple18*)_tmp1BF->hd)->f2;_LL3D:
# 1089
 s=({struct _fat_ptr _tmp4CE=*_tmp1C4;int*_tmp4CD=span;struct Cyc_List_List*_tmp4CC=({struct Cyc_List_List*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));({void*_tmp4CB=Cyc_Tcpat_compile_pat(_tmp1C3);_tmp1C0->hd=_tmp4CB;}),_tmp1C0->tl=0;_tmp1C0;});Cyc_Tcpat_con_pat(_tmp4CE,_tmp4CD,_tmp4CC,p);});
goto _LL3B;}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{_LL3E: _LL3F:
({void*_tmp1C1=0U;({struct _fat_ptr _tmp4CF=({const char*_tmp1C2="bad union pattern";_tag_fat(_tmp1C2,sizeof(char),18U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CF,_tag_fat(_tmp1C1,sizeof(void*),0U));});});}_LL3B:;};}
# 1094
goto _LL0;}else{goto _LL23;}}else{_LL23: _LL24:
# 1122
 goto _LL26;}case 13U: _LL1B: _tmp1DA=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1;_tmp1D9=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1A5)->f2;_LL1C:
# 1102
 s=Cyc_Tcpat_con_pat(*(*_tmp1D9->name).f2,0,0,p);
goto _LL0;case 14U: _LL1D: _tmp1DC=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1A5)->f1;_tmp1DB=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1A5)->f2;_LL1E: {
# 1108
struct Cyc_List_List*fields;
{void*_tmp1C6=Cyc_Tcutil_compress(_tmp1DC);void*_tmp1C7=_tmp1C6;struct Cyc_List_List*_tmp1CA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->tag == 0U){if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f1)->tag == 16U){_LL41: _tmp1CA=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f1)->f1;_LL42:
 fields=_tmp1CA;goto _LL40;}else{goto _LL43;}}else{_LL43: _LL44:
({void*_tmp1C8=0U;({struct _fat_ptr _tmp4D0=({const char*_tmp1C9="bad type in AnonEnum_p";_tag_fat(_tmp1C9,sizeof(char),23U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D0,_tag_fat(_tmp1C8,sizeof(void*),0U));});});}_LL40:;}
# 1118
s=Cyc_Tcpat_con_pat(*(*_tmp1DB->name).f2,0,0,p);
goto _LL0;}case 15U: _LL1F: _LL20:
 goto _LL22;case 16U: _LL21: _LL22:
 goto _LL24;default: _LL25: _LL26:
# 1123
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->tag=0U;_tmp1CB;});}_LL0:;}
# 1125
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1154
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp1DD=({struct _tuple0 _tmp3A4;_tmp3A4.f1=a1,_tmp3A4.f2=a2;_tmp3A4;});struct _tuple0 _tmp1DE=_tmp1DD;int _tmp1E8;struct _fat_ptr*_tmp1E7;int _tmp1E6;struct _fat_ptr*_tmp1E5;struct Cyc_Absyn_Datatypefield*_tmp1E4;unsigned _tmp1E3;struct Cyc_Absyn_Datatypefield*_tmp1E2;unsigned _tmp1E1;unsigned _tmp1E0;unsigned _tmp1DF;switch(*((int*)_tmp1DE.f1)){case 0U: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp1DE.f2)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LLB;}case 1U: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp1DE.f2)->tag == 1U){_LL3: _LL4:
 return 1;}else{goto _LLB;}case 2U: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1DE.f2)->tag == 2U){_LL5: _tmp1E0=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1DE.f1)->f1;_tmp1DF=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1DE.f2)->f1;_LL6:
 return _tmp1E0 == _tmp1DF;}else{goto _LLB;}case 3U: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DE.f2)->tag == 3U){_LL7: _tmp1E4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DE.f1)->f2;_tmp1E3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DE.f1)->f3;_tmp1E2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DE.f2)->f2;_tmp1E1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1DE.f2)->f3;_LL8:
# 1160
 return _tmp1E4 == _tmp1E2 && _tmp1E3 == _tmp1E1;}else{goto _LLB;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DE.f2)->tag == 4U){_LL9: _tmp1E8=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DE.f1)->f1;_tmp1E7=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DE.f1)->f2;_tmp1E6=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DE.f2)->f1;_tmp1E5=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1DE.f2)->f2;_LLA:
# 1162
 return _tmp1E8 == _tmp1E6 && Cyc_strptrcmp(_tmp1E7,_tmp1E5)== 0;}else{_LLB: _LLC:
 return 0;}}_LL0:;}
# 1167
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0 && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1173
if(p1 != p2)return 0;
return 1;}
# 1177
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp1E9=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp1EA=d2;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1EF;void*_tmp1EE;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1EA)->tag == 2U){_LL1: _tmp1F0=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1EA)->f1;_tmp1EF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1EA)->f2;_tmp1EE=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1EA)->f3;_LL2:
# 1181
 if(Cyc_Tcpat_same_path(access,_tmp1F0)&&(int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->tag=2U,_tmp1ED->f1=_tmp1F0,({
struct Cyc_List_List*_tmp4D2=({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));({struct _tuple0*_tmp4D1=({struct _tuple0*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->f1=_tmp1E9,_tmp1EB->f2=d1;_tmp1EB;});_tmp1EC->hd=_tmp4D1;}),_tmp1EC->tl=_tmp1EF;_tmp1EC;});_tmp1ED->f2=_tmp4D2;}),_tmp1ED->f3=_tmp1EE;_tmp1ED;});else{
# 1185
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1188
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->tag=2U,_tmp1F3->f1=access,({struct Cyc_List_List*_tmp4D4=({struct Cyc_List_List*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));({struct _tuple0*_tmp4D3=({struct _tuple0*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->f1=_tmp1E9,_tmp1F1->f2=d1;_tmp1F1;});_tmp1F2->hd=_tmp4D3;}),_tmp1F2->tl=0;_tmp1F2;});_tmp1F3->f2=_tmp4D4;}),_tmp1F3->f3=d2;_tmp1F3;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1199
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1202
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp1F4=0U;({struct Cyc___cycFILE*_tmp4D6=Cyc_stderr;struct _fat_ptr _tmp4D5=({const char*_tmp1F5=" ";_tag_fat(_tmp1F5,sizeof(char),2U);});Cyc_fprintf(_tmp4D6,_tmp4D5,_tag_fat(_tmp1F4,sizeof(void*),0U));});});}}
# 1208
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp1F6=c->name;
union Cyc_Tcpat_Name_value _tmp1F7=_tmp1F6;int _tmp1FF;struct _fat_ptr _tmp1FE;if((_tmp1F7.Name_v).tag == 1){_LL1: _tmp1FE=(_tmp1F7.Name_v).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp1FA=({struct Cyc_String_pa_PrintArg_struct _tmp3A5;_tmp3A5.tag=0U,_tmp3A5.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp1FE);_tmp3A5;});void*_tmp1F8[1U];_tmp1F8[0]=& _tmp1FA;({struct Cyc___cycFILE*_tmp4D8=Cyc_stderr;struct _fat_ptr _tmp4D7=({const char*_tmp1F9="%s";_tag_fat(_tmp1F9,sizeof(char),3U);});Cyc_fprintf(_tmp4D8,_tmp4D7,_tag_fat(_tmp1F8,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _tmp1FF=(_tmp1F7.Int_v).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp1FD=({struct Cyc_Int_pa_PrintArg_struct _tmp3A6;_tmp3A6.tag=1U,_tmp3A6.f1=(unsigned long)_tmp1FF;_tmp3A6;});void*_tmp1FB[1U];_tmp1FB[0]=& _tmp1FD;({struct Cyc___cycFILE*_tmp4DA=Cyc_stderr;struct _fat_ptr _tmp4D9=({const char*_tmp1FC="%d";_tag_fat(_tmp1FC,sizeof(char),3U);});Cyc_fprintf(_tmp4DA,_tmp4D9,_tag_fat(_tmp1FB,sizeof(void*),1U));});});goto _LL0;}_LL0:;}
# 1216
static void Cyc_Tcpat_print_access(void*a){
void*_tmp200=a;int _tmp216;struct _fat_ptr*_tmp215;struct Cyc_Absyn_Datatypefield*_tmp214;unsigned _tmp213;unsigned _tmp212;switch(*((int*)_tmp200)){case 0U: _LL1: _LL2:
({void*_tmp201=0U;({struct Cyc___cycFILE*_tmp4DC=Cyc_stderr;struct _fat_ptr _tmp4DB=({const char*_tmp202="DUMMY";_tag_fat(_tmp202,sizeof(char),6U);});Cyc_fprintf(_tmp4DC,_tmp4DB,_tag_fat(_tmp201,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp203=0U;({struct Cyc___cycFILE*_tmp4DE=Cyc_stderr;struct _fat_ptr _tmp4DD=({const char*_tmp204="*";_tag_fat(_tmp204,sizeof(char),2U);});Cyc_fprintf(_tmp4DE,_tmp4DD,_tag_fat(_tmp203,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp212=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp200)->f1;_LL6:
({struct Cyc_Int_pa_PrintArg_struct _tmp207=({struct Cyc_Int_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=1U,_tmp3A7.f1=(unsigned long)((int)_tmp212);_tmp3A7;});void*_tmp205[1U];_tmp205[0]=& _tmp207;({struct Cyc___cycFILE*_tmp4E0=Cyc_stderr;struct _fat_ptr _tmp4DF=({const char*_tmp206="[%d]";_tag_fat(_tmp206,sizeof(char),5U);});Cyc_fprintf(_tmp4E0,_tmp4DF,_tag_fat(_tmp205,sizeof(void*),1U));});});goto _LL0;case 3U: _LL7: _tmp214=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp200)->f2;_tmp213=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp200)->f3;_LL8:
# 1222
({struct Cyc_String_pa_PrintArg_struct _tmp20A=({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0U,({struct _fat_ptr _tmp4E1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp214->name));_tmp3A9.f1=_tmp4E1;});_tmp3A9;});struct Cyc_Int_pa_PrintArg_struct _tmp20B=({struct Cyc_Int_pa_PrintArg_struct _tmp3A8;_tmp3A8.tag=1U,_tmp3A8.f1=(unsigned long)((int)_tmp213);_tmp3A8;});void*_tmp208[2U];_tmp208[0]=& _tmp20A,_tmp208[1]=& _tmp20B;({struct Cyc___cycFILE*_tmp4E3=Cyc_stderr;struct _fat_ptr _tmp4E2=({const char*_tmp209="%s[%d]";_tag_fat(_tmp209,sizeof(char),7U);});Cyc_fprintf(_tmp4E3,_tmp4E2,_tag_fat(_tmp208,sizeof(void*),2U));});});goto _LL0;default: _LL9: _tmp216=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp200)->f1;_tmp215=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp200)->f2;_LLA:
# 1224
 if(_tmp216)
({struct Cyc_String_pa_PrintArg_struct _tmp20E=({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0U,_tmp3AA.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp215);_tmp3AA;});void*_tmp20C[1U];_tmp20C[0]=& _tmp20E;({struct Cyc___cycFILE*_tmp4E5=Cyc_stderr;struct _fat_ptr _tmp4E4=({const char*_tmp20D=".tagunion.%s";_tag_fat(_tmp20D,sizeof(char),13U);});Cyc_fprintf(_tmp4E5,_tmp4E4,_tag_fat(_tmp20C,sizeof(void*),1U));});});else{
# 1227
({struct Cyc_String_pa_PrintArg_struct _tmp211=({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0U,_tmp3AB.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp215);_tmp3AB;});void*_tmp20F[1U];_tmp20F[0]=& _tmp211;({struct Cyc___cycFILE*_tmp4E7=Cyc_stderr;struct _fat_ptr _tmp4E6=({const char*_tmp210=".%s";_tag_fat(_tmp210,sizeof(char),4U);});Cyc_fprintf(_tmp4E7,_tmp4E6,_tag_fat(_tmp20F,sizeof(void*),1U));});});}
goto _LL0;}_LL0:;}
# 1232
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp217=p;struct Cyc_Absyn_Datatypefield*_tmp23C;struct _fat_ptr*_tmp23B;int _tmp23A;int _tmp239;unsigned _tmp238;struct _fat_ptr _tmp237;struct Cyc_Absyn_Enumfield*_tmp236;struct Cyc_Absyn_Enumfield*_tmp235;struct Cyc_Absyn_Exp*_tmp234;switch(*((int*)_tmp217)){case 0U: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp217)->f1 == 0){_LL1: _LL2:
({void*_tmp218=0U;({struct Cyc___cycFILE*_tmp4E9=Cyc_stderr;struct _fat_ptr _tmp4E8=({const char*_tmp219="where(NULL)";_tag_fat(_tmp219,sizeof(char),12U);});Cyc_fprintf(_tmp4E9,_tmp4E8,_tag_fat(_tmp218,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp234=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp217)->f1;_LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp21C=({struct Cyc_String_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=0U,({struct _fat_ptr _tmp4EA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp234)));_tmp3AC.f1=_tmp4EA;});_tmp3AC;});void*_tmp21A[1U];_tmp21A[0]=& _tmp21C;({struct Cyc___cycFILE*_tmp4EC=Cyc_stderr;struct _fat_ptr _tmp4EB=({const char*_tmp21B="where(%s)";_tag_fat(_tmp21B,sizeof(char),10U);});Cyc_fprintf(_tmp4EC,_tmp4EB,_tag_fat(_tmp21A,sizeof(void*),1U));});});goto _LL0;}case 1U: _LL5: _LL6:
({void*_tmp21D=0U;({struct Cyc___cycFILE*_tmp4EE=Cyc_stderr;struct _fat_ptr _tmp4ED=({const char*_tmp21E="NULL";_tag_fat(_tmp21E,sizeof(char),5U);});Cyc_fprintf(_tmp4EE,_tmp4ED,_tag_fat(_tmp21D,sizeof(void*),0U));});});goto _LL0;case 2U: _LL7: _LL8:
({void*_tmp21F=0U;({struct Cyc___cycFILE*_tmp4F0=Cyc_stderr;struct _fat_ptr _tmp4EF=({const char*_tmp220="NOT-NULL:";_tag_fat(_tmp220,sizeof(char),10U);});Cyc_fprintf(_tmp4F0,_tmp4EF,_tag_fat(_tmp21F,sizeof(void*),0U));});});goto _LL0;case 4U: _LL9: _tmp235=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp217)->f2;_LLA:
 _tmp236=_tmp235;goto _LLC;case 3U: _LLB: _tmp236=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp217)->f2;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp223=({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0U,({struct _fat_ptr _tmp4F1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp236->name));_tmp3AD.f1=_tmp4F1;});_tmp3AD;});void*_tmp221[1U];_tmp221[0]=& _tmp223;({struct Cyc___cycFILE*_tmp4F3=Cyc_stderr;struct _fat_ptr _tmp4F2=({const char*_tmp222="%s";_tag_fat(_tmp222,sizeof(char),3U);});Cyc_fprintf(_tmp4F3,_tmp4F2,_tag_fat(_tmp221,sizeof(void*),1U));});});goto _LL0;case 5U: _LLD: _tmp237=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp217)->f1;_LLE:
({struct Cyc_String_pa_PrintArg_struct _tmp226=({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0U,_tmp3AE.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp237);_tmp3AE;});void*_tmp224[1U];_tmp224[0]=& _tmp226;({struct Cyc___cycFILE*_tmp4F5=Cyc_stderr;struct _fat_ptr _tmp4F4=({const char*_tmp225="%s";_tag_fat(_tmp225,sizeof(char),3U);});Cyc_fprintf(_tmp4F5,_tmp4F4,_tag_fat(_tmp224,sizeof(void*),1U));});});goto _LL0;case 6U: _LLF: _tmp238=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp217)->f1;_LL10:
({struct Cyc_Int_pa_PrintArg_struct _tmp229=({struct Cyc_Int_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=1U,_tmp3AF.f1=(unsigned long)((int)_tmp238);_tmp3AF;});void*_tmp227[1U];_tmp227[0]=& _tmp229;({struct Cyc___cycFILE*_tmp4F7=Cyc_stderr;struct _fat_ptr _tmp4F6=({const char*_tmp228="%d";_tag_fat(_tmp228,sizeof(char),3U);});Cyc_fprintf(_tmp4F7,_tmp4F6,_tag_fat(_tmp227,sizeof(void*),1U));});});goto _LL0;case 7U: _LL11: _tmp239=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp217)->f1;_LL12:
({struct Cyc_Int_pa_PrintArg_struct _tmp22C=({struct Cyc_Int_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=1U,_tmp3B0.f1=(unsigned long)_tmp239;_tmp3B0;});void*_tmp22A[1U];_tmp22A[0]=& _tmp22C;({struct Cyc___cycFILE*_tmp4F9=Cyc_stderr;struct _fat_ptr _tmp4F8=({const char*_tmp22B="datatypetag(%d)";_tag_fat(_tmp22B,sizeof(char),16U);});Cyc_fprintf(_tmp4F9,_tmp4F8,_tag_fat(_tmp22A,sizeof(void*),1U));});});goto _LL0;case 8U: _LL13: _tmp23B=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp217)->f1;_tmp23A=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp217)->f2;_LL14:
({struct Cyc_String_pa_PrintArg_struct _tmp22F=({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0U,_tmp3B2.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp23B);_tmp3B2;});struct Cyc_Int_pa_PrintArg_struct _tmp230=({struct Cyc_Int_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=1U,_tmp3B1.f1=(unsigned long)_tmp23A;_tmp3B1;});void*_tmp22D[2U];_tmp22D[0]=& _tmp22F,_tmp22D[1]=& _tmp230;({struct Cyc___cycFILE*_tmp4FB=Cyc_stderr;struct _fat_ptr _tmp4FA=({const char*_tmp22E="uniontag[%s](%d)";_tag_fat(_tmp22E,sizeof(char),17U);});Cyc_fprintf(_tmp4FB,_tmp4FA,_tag_fat(_tmp22D,sizeof(void*),2U));});});goto _LL0;default: _LL15: _tmp23C=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp217)->f2;_LL16:
# 1245
({struct Cyc_String_pa_PrintArg_struct _tmp233=({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0U,({struct _fat_ptr _tmp4FC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp23C->name));_tmp3B3.f1=_tmp4FC;});_tmp3B3;});void*_tmp231[1U];_tmp231[0]=& _tmp233;({struct Cyc___cycFILE*_tmp4FE=Cyc_stderr;struct _fat_ptr _tmp4FD=({const char*_tmp232="datatypefield(%s)";_tag_fat(_tmp232,sizeof(char),18U);});Cyc_fprintf(_tmp4FE,_tmp4FD,_tag_fat(_tmp231,sizeof(void*),1U));});});}_LL0:;}
# 1249
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp23F=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0U,({struct _fat_ptr _tmp4FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp3B4.f1=_tmp4FF;});_tmp3B4;});void*_tmp23D[1U];_tmp23D[0]=& _tmp23F;({struct Cyc___cycFILE*_tmp501=Cyc_stderr;struct _fat_ptr _tmp500=({const char*_tmp23E="%s";_tag_fat(_tmp23E,sizeof(char),3U);});Cyc_fprintf(_tmp501,_tmp500,_tag_fat(_tmp23D,sizeof(void*),1U));});});}
# 1253
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp240=d;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25B;void*_tmp25A;struct Cyc_Tcpat_Rhs*_tmp259;switch(*((int*)_tmp240)){case 1U: _LL1: _tmp259=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp240)->f1;_LL2:
# 1256
 Cyc_Tcpat_print_tab(tab);
({void*_tmp241=0U;({struct Cyc___cycFILE*_tmp503=Cyc_stderr;struct _fat_ptr _tmp502=({const char*_tmp242="Success(";_tag_fat(_tmp242,sizeof(char),9U);});Cyc_fprintf(_tmp503,_tmp502,_tag_fat(_tmp241,sizeof(void*),0U));});});Cyc_Tcpat_print_rhs(_tmp259);({void*_tmp243=0U;({struct Cyc___cycFILE*_tmp505=Cyc_stderr;struct _fat_ptr _tmp504=({const char*_tmp244=")\n";_tag_fat(_tmp244,sizeof(char),3U);});Cyc_fprintf(_tmp505,_tmp504,_tag_fat(_tmp243,sizeof(void*),0U));});});
goto _LL0;case 0U: _LL3: _LL4:
({void*_tmp245=0U;({struct Cyc___cycFILE*_tmp507=Cyc_stderr;struct _fat_ptr _tmp506=({const char*_tmp246="Failure\n";_tag_fat(_tmp246,sizeof(char),9U);});Cyc_fprintf(_tmp507,_tmp506,_tag_fat(_tmp245,sizeof(void*),0U));});});goto _LL0;default: _LL5: _tmp25C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp240)->f1;_tmp25B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp240)->f2;_tmp25A=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp240)->f3;_LL6:
# 1261
 Cyc_Tcpat_print_tab(tab);
({void*_tmp247=0U;({struct Cyc___cycFILE*_tmp509=Cyc_stderr;struct _fat_ptr _tmp508=({const char*_tmp248="Switch[";_tag_fat(_tmp248,sizeof(char),8U);});Cyc_fprintf(_tmp509,_tmp508,_tag_fat(_tmp247,sizeof(void*),0U));});});
for(0;_tmp25C != 0;_tmp25C=_tmp25C->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp25C->hd)->access);
if(_tmp25C->tl != 0)({void*_tmp249=0U;({struct Cyc___cycFILE*_tmp50B=Cyc_stderr;struct _fat_ptr _tmp50A=({const char*_tmp24A=",";_tag_fat(_tmp24A,sizeof(char),2U);});Cyc_fprintf(_tmp50B,_tmp50A,_tag_fat(_tmp249,sizeof(void*),0U));});});}
# 1267
({void*_tmp24B=0U;({struct Cyc___cycFILE*_tmp50D=Cyc_stderr;struct _fat_ptr _tmp50C=({const char*_tmp24C="] {\n";_tag_fat(_tmp24C,sizeof(char),5U);});Cyc_fprintf(_tmp50D,_tmp50C,_tag_fat(_tmp24B,sizeof(void*),0U));});});
for(0;_tmp25B != 0;_tmp25B=_tmp25B->tl){
struct _tuple0 _tmp24D=*((struct _tuple0*)_tmp25B->hd);struct _tuple0 _tmp24E=_tmp24D;void*_tmp254;void*_tmp253;_LL8: _tmp254=_tmp24E.f1;_tmp253=_tmp24E.f2;_LL9:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp24F=0U;({struct Cyc___cycFILE*_tmp50F=Cyc_stderr;struct _fat_ptr _tmp50E=({const char*_tmp250="case ";_tag_fat(_tmp250,sizeof(char),6U);});Cyc_fprintf(_tmp50F,_tmp50E,_tag_fat(_tmp24F,sizeof(void*),0U));});});
Cyc_Tcpat_print_pat_test(_tmp254);
({void*_tmp251=0U;({struct Cyc___cycFILE*_tmp511=Cyc_stderr;struct _fat_ptr _tmp510=({const char*_tmp252=":\n";_tag_fat(_tmp252,sizeof(char),3U);});Cyc_fprintf(_tmp511,_tmp510,_tag_fat(_tmp251,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(_tmp253,tab + 7);}
# 1276
Cyc_Tcpat_print_tab(tab);
({void*_tmp255=0U;({struct Cyc___cycFILE*_tmp513=Cyc_stderr;struct _fat_ptr _tmp512=({const char*_tmp256="default:\n";_tag_fat(_tmp256,sizeof(char),10U);});Cyc_fprintf(_tmp513,_tmp512,_tag_fat(_tmp255,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(_tmp25A,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp257=0U;({struct Cyc___cycFILE*_tmp515=Cyc_stderr;struct _fat_ptr _tmp514=({const char*_tmp258="}\n";_tag_fat(_tmp258,sizeof(char),3U);});Cyc_fprintf(_tmp515,_tmp514,_tag_fat(_tmp257,sizeof(void*),0U));});});}_LL0:;}
# 1284
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1292
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp25D=td;struct Cyc_Set_Set*_tmp261;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp25D)->tag == 1U){_LL1: _tmp261=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp25D)->f1;_LL2:
# 1302
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->tag=1U,({struct Cyc_Set_Set*_tmp516=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp261,c);_tmp25E->f1=_tmp516;});_tmp25E;});}else{_LL3: _LL4:
({void*_tmp25F=0U;({struct _fat_ptr _tmp517=({const char*_tmp260="add_neg called when td is Positive";_tag_fat(_tmp260,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp517,_tag_fat(_tmp25F,sizeof(void*),0U));});});}_LL0:;}
# 1309
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp262=td;struct Cyc_Set_Set*_tmp264;struct Cyc_Tcpat_Con_s*_tmp263;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp262)->tag == 0U){_LL1: _tmp263=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp262)->f1;_LL2:
# 1313
 if(Cyc_Tcpat_compare_con(c,_tmp263)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL3: _tmp264=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp262)->f1;_LL4:
# 1317
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp264,c))return Cyc_Tcpat_No;else{
# 1320
if(c->span != 0 &&({int _tmp518=*((int*)_check_null(c->span));_tmp518 == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp264)+ 1;}))
return Cyc_Tcpat_Yes;else{
# 1323
return Cyc_Tcpat_Maybe;}}}_LL0:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1331
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp265=ctxt;struct Cyc_Tcpat_Con_s*_tmp26B;struct Cyc_List_List*_tmp26A;struct Cyc_List_List*_tmp269;if(_tmp265 == 0){_LL1: _LL2:
 return 0;}else{_LL3: _tmp26B=((struct _tuple21*)_tmp265->hd)->f1;_tmp26A=((struct _tuple21*)_tmp265->hd)->f2;_tmp269=_tmp265->tl;_LL4:
# 1335
 return({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*_tmp268));
({struct _tuple21*_tmp51A=({struct _tuple21*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->f1=_tmp26B,({struct Cyc_List_List*_tmp519=({struct Cyc_List_List*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->hd=dsc,_tmp266->tl=_tmp26A;_tmp266;});_tmp267->f2=_tmp519;});_tmp267;});_tmp268->hd=_tmp51A;}),_tmp268->tl=_tmp269;_tmp268;});}_LL0:;}
# 1343
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp26C=ctxt;struct Cyc_Tcpat_Con_s*_tmp272;struct Cyc_List_List*_tmp271;struct Cyc_List_List*_tmp270;if(_tmp26C == 0){_LL1: _LL2:
({void*_tmp26D=0U;({struct _fat_ptr _tmp51B=({const char*_tmp26E="norm_context: empty context";_tag_fat(_tmp26E,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp51B,_tag_fat(_tmp26D,sizeof(void*),0U));});});}else{_LL3: _tmp272=((struct _tuple21*)_tmp26C->hd)->f1;_tmp271=((struct _tuple21*)_tmp26C->hd)->f2;_tmp270=_tmp26C->tl;_LL4:
# 1347
 return({struct Cyc_List_List*_tmp51D=_tmp270;Cyc_Tcpat_augment(_tmp51D,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->tag=0U,_tmp26F->f1=_tmp272,({struct Cyc_List_List*_tmp51C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp271);_tmp26F->f2=_tmp51C;});_tmp26F;}));});}_LL0:;}
# 1356
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1358
struct _tuple1 _tmp273=({struct _tuple1 _tmp3B5;_tmp3B5.f1=ctxt,_tmp3B5.f2=work;_tmp3B5;});struct _tuple1 _tmp274=_tmp273;struct Cyc_Tcpat_Con_s*_tmp27E;struct Cyc_List_List*_tmp27D;struct Cyc_List_List*_tmp27C;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27A;if(_tmp274.f1 == 0){if(_tmp274.f2 == 0){_LL1: _LL2:
 return dsc;}else{_LL3: _LL4:
 goto _LL6;}}else{if(_tmp274.f2 == 0){_LL5: _LL6:
({void*_tmp275=0U;({struct _fat_ptr _tmp51E=({const char*_tmp276="build_desc: ctxt and work don't match";_tag_fat(_tmp276,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp51E,_tag_fat(_tmp275,sizeof(void*),0U));});});}else{_LL7: _tmp27E=((struct _tuple21*)(_tmp274.f1)->hd)->f1;_tmp27D=((struct _tuple21*)(_tmp274.f1)->hd)->f2;_tmp27C=(_tmp274.f1)->tl;_tmp27B=((struct _tuple20*)(_tmp274.f2)->hd)->f3;_tmp27A=(_tmp274.f2)->tl;_LL8: {
# 1363
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp277=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->tag=0U,_tmp279->f1=_tmp27E,({struct Cyc_List_List*_tmp520=({struct Cyc_List_List*_tmp51F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp27D);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp51F,({struct Cyc_List_List*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->hd=dsc,_tmp278->tl=_tmp27B;_tmp278;}));});_tmp279->f2=_tmp520;});_tmp279;});
return Cyc_Tcpat_build_desc(_tmp27C,(void*)_tmp277,_tmp27A);}}}_LL0:;}
# 1368
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1375
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp27F=allmrules;void*_tmp283;struct Cyc_Tcpat_Rhs*_tmp282;struct Cyc_List_List*_tmp281;if(_tmp27F == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280->tag=0U,_tmp280->f1=dsc;_tmp280;});}else{_LL3: _tmp283=((struct _tuple22*)_tmp27F->hd)->f1;_tmp282=((struct _tuple22*)_tmp27F->hd)->f2;_tmp281=_tmp27F->tl;_LL4:
# 1379
 return Cyc_Tcpat_match(_tmp283,0,dsc,0,0,_tmp282,_tmp281);}_LL0:;}
# 1384
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp522=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->tag=1U,({struct Cyc_Set_Set*_tmp521=Cyc_Tcpat_empty_con_set();_tmp284->f1=_tmp521;});_tmp284;});Cyc_Tcpat_or_match(_tmp522,allmrules);});}
# 1391
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1394
struct Cyc_List_List*_tmp285=work;struct Cyc_List_List*_tmp29C;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp298;if(_tmp285 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->tag=1U,_tmp286->f1=right_hand_side;_tmp286;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp285)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp285)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp285)->hd)->f3 == 0){_LL3: _tmp298=_tmp285->tl;_LL4:
# 1397
 return({struct Cyc_List_List*_tmp525=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp524=_tmp298;struct Cyc_Tcpat_Rhs*_tmp523=right_hand_side;Cyc_Tcpat_and_match(_tmp525,_tmp524,_tmp523,rules);});}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _tmp29C=((struct _tuple20*)_tmp285->hd)->f1;_tmp29B=((struct _tuple20*)_tmp285->hd)->f2;_tmp29A=((struct _tuple20*)_tmp285->hd)->f3;_tmp299=_tmp285->tl;_LL6:
# 1399
 if((_tmp29C == 0 || _tmp29B == 0)|| _tmp29A == 0)
({void*_tmp287=0U;({struct _fat_ptr _tmp526=({const char*_tmp288="tcpat:and_match: malformed work frame";_tag_fat(_tmp288,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp526,_tag_fat(_tmp287,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp289=_tmp29C;struct Cyc_List_List*_tmp28A=_tmp289;void*_tmp297;struct Cyc_List_List*_tmp296;_LL8: _tmp297=(void*)_tmp28A->hd;_tmp296=_tmp28A->tl;_LL9:;{
struct Cyc_List_List*_tmp28B=_tmp29B;struct Cyc_List_List*_tmp28C=_tmp28B;struct Cyc_List_List*_tmp295;struct Cyc_List_List*_tmp294;_LLB: _tmp295=(struct Cyc_List_List*)_tmp28C->hd;_tmp294=_tmp28C->tl;_LLC:;{
struct Cyc_List_List*_tmp28D=_tmp29A;struct Cyc_List_List*_tmp28E=_tmp28D;void*_tmp293;struct Cyc_List_List*_tmp292;_LLE: _tmp293=(void*)_tmp28E->hd;_tmp292=_tmp28E->tl;_LLF:;{
struct _tuple20*_tmp28F=({struct _tuple20*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291->f1=_tmp296,_tmp291->f2=_tmp294,_tmp291->f3=_tmp292;_tmp291;});
return({void*_tmp52C=_tmp297;struct Cyc_List_List*_tmp52B=_tmp295;void*_tmp52A=_tmp293;struct Cyc_List_List*_tmp529=ctx;struct Cyc_List_List*_tmp528=({struct Cyc_List_List*_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290->hd=_tmp28F,_tmp290->tl=_tmp299;_tmp290;});struct Cyc_Tcpat_Rhs*_tmp527=right_hand_side;Cyc_Tcpat_match(_tmp52C,_tmp52B,_tmp52A,_tmp529,_tmp528,_tmp527,rules);});};};};};}}_LL0:;}
# 1410
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp29D=dsc;struct Cyc_List_List*_tmp2A2;struct Cyc_Set_Set*_tmp2A1;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp29D)->tag == 1U){_LL1: _tmp2A1=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp29D)->f1;_LL2: {
# 1416
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->tag=1U,({struct Cyc_Set_Set*_tmp52D=Cyc_Tcpat_empty_con_set();_tmp2A0->f1=_tmp52D;});_tmp2A0;});
struct Cyc_List_List*_tmp29E=0;
{int i=0;for(0;i < pcon->arity;++ i){
_tmp29E=({struct Cyc_List_List*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->hd=any,_tmp29F->tl=_tmp29E;_tmp29F;});}}
return _tmp29E;}}else{_LL3: _tmp2A2=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp29D)->f2;_LL4:
 return _tmp2A2;}_LL0:;}
# 1425
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp2A3=pw;struct Cyc_Absyn_Pat*_tmp2C0;if((_tmp2A3.where_clause).tag == 2){_LL1: _LL2:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL3: _tmp2C0=(_tmp2A3.pattern).val;_LL4: {
# 1429
void*_tmp2A4=_tmp2C0->r;void*_tmp2A5=_tmp2A4;union Cyc_Absyn_AggrInfo _tmp2BF;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Datatypedecl*_tmp2BD;struct Cyc_Absyn_Datatypefield*_tmp2BC;switch(*((int*)_tmp2A5)){case 6U: _LL6: _LL7:
# 1431
 if(i != 0)
({void*_tmp2A6=0U;({struct _fat_ptr _tmp52F=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2A9=({struct Cyc_Int_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=1U,_tmp3B6.f1=(unsigned long)i;_tmp3B6;});void*_tmp2A7[1U];_tmp2A7[0]=& _tmp2A9;({struct _fat_ptr _tmp52E=({const char*_tmp2A8="get_access on pointer pattern with offset %d\n";_tag_fat(_tmp2A8,sizeof(char),46U);});Cyc_aprintf(_tmp52E,_tag_fat(_tmp2A7,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp52F,_tag_fat(_tmp2A6,sizeof(void*),0U));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5U: _LL8: _LL9:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->tag=2U,_tmp2AA->f1=(unsigned)i;_tmp2AA;});case 8U: _LLA: _tmp2BD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2A5)->f1;_tmp2BC=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2A5)->f2;_LLB:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->tag=3U,_tmp2AB->f1=_tmp2BD,_tmp2AB->f2=_tmp2BC,_tmp2AB->f3=(unsigned)i;_tmp2AB;});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A5)->f1 != 0){_LLC: _tmp2BF=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A5)->f1;_tmp2BE=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A5)->f3;_LLD: {
# 1437
struct Cyc_Absyn_Aggrdecl*_tmp2AC=Cyc_Absyn_get_known_aggrdecl(_tmp2BF);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AC->impl))->tagged){
struct Cyc_List_List*_tmp2AD=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp2BE))->hd)).f1;struct Cyc_List_List*_tmp2AE=_tmp2AD;struct _fat_ptr*_tmp2B4;if(_tmp2AE != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp2AE)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp2AE)->tl == 0){_LL11: _tmp2B4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2AE->hd)->f1;_LL12:
# 1441
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));_tmp2AF->tag=4U,_tmp2AF->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AC->impl))->tagged,_tmp2AF->f2=_tmp2B4;_tmp2AF;});}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _LL14:
({void*_tmp2B0=0U;({struct _fat_ptr _tmp532=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B3=({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0U,({struct _fat_ptr _tmp530=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(_tmp2C0));_tmp3B7.f1=_tmp530;});_tmp3B7;});void*_tmp2B1[1U];_tmp2B1[0]=& _tmp2B3;({struct _fat_ptr _tmp531=({const char*_tmp2B2="get_access on bad aggr pattern: %s";_tag_fat(_tmp2B2,sizeof(char),35U);});Cyc_aprintf(_tmp531,_tag_fat(_tmp2B1,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp532,_tag_fat(_tmp2B0,sizeof(void*),0U));});});}_LL10:;}{
# 1445
struct Cyc_List_List*_tmp2B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AC->impl))->fields;
int _tmp2B6=i;
for(0;i != 0;-- i){_tmp2B5=((struct Cyc_List_List*)_check_null(_tmp2B5))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->tag=4U,_tmp2B7->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AC->impl))->tagged,_tmp2B7->f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2B5))->hd)->name;_tmp2B7;});};}}else{goto _LLE;}default: _LLE: _LLF:
({void*_tmp2B8=0U;({struct _fat_ptr _tmp535=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2BB=({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0U,({struct _fat_ptr _tmp533=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(_tmp2C0));_tmp3B8.f1=_tmp533;});_tmp3B8;});void*_tmp2B9[1U];_tmp2B9[0]=& _tmp2BB;({struct _fat_ptr _tmp534=({const char*_tmp2BA="get_access on bad pattern: %s";_tag_fat(_tmp2BA,sizeof(char),30U);});Cyc_aprintf(_tmp534,_tag_fat(_tmp2B9,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp535,_tag_fat(_tmp2B8,sizeof(void*),0U));});});}_LL5:;}}_LL0:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1455
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp2C1=env;struct Cyc_List_List*_tmp2C5;struct Cyc_Tcpat_Con_s*_tmp2C4;_LL1: _tmp2C5=_tmp2C1->f1;_tmp2C4=_tmp2C1->f2;_LL2:;{
void*acc=Cyc_Tcpat_get_access(_tmp2C4->orig_pat,i);
return({struct Cyc_List_List*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));({struct Cyc_Tcpat_PathNode*_tmp536=({struct Cyc_Tcpat_PathNode*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2->orig_pat=_tmp2C4->orig_pat,_tmp2C2->access=acc;_tmp2C2;});_tmp2C3->hd=_tmp536;}),_tmp2C3->tl=_tmp2C5;_tmp2C3;});};}
# 1460
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp2C6=({struct _tuple23 _tmp3B9;_tmp3B9.f1=obj,_tmp3B9.f2=pcon;_tmp3B9;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp2C6);}
# 1468
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1472
void*_tmp2C7=p;struct Cyc_Tcpat_Con_s*_tmp2D5;struct Cyc_List_List*_tmp2D4;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2C7)->tag == 0U){_LL1: _LL2:
# 1474
 return({struct Cyc_List_List*_tmp539=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp538=work;struct Cyc_Tcpat_Rhs*_tmp537=right_hand_side;Cyc_Tcpat_and_match(_tmp539,_tmp538,_tmp537,rules);});}else{_LL3: _tmp2D5=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2C7)->f1;_tmp2D4=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2C7)->f2;_LL4: {
# 1476
enum Cyc_Tcpat_Answer _tmp2C8=Cyc_Tcpat_static_match(_tmp2D5,dsc);enum Cyc_Tcpat_Answer _tmp2C9=_tmp2C8;switch(_tmp2C9){case Cyc_Tcpat_Yes: _LL6: _LL7: {
# 1478
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));({struct _tuple21*_tmp53A=({struct _tuple21*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->f1=_tmp2D5,_tmp2CC->f2=0;_tmp2CC;});_tmp2CD->hd=_tmp53A;}),_tmp2CD->tl=ctx;_tmp2CD;});
struct _tuple20*work_frame=({struct _tuple20*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->f1=_tmp2D4,({struct Cyc_List_List*_tmp53C=Cyc_Tcpat_getoargs(_tmp2D5,obj);_tmp2CB->f2=_tmp53C;}),({
struct Cyc_List_List*_tmp53B=Cyc_Tcpat_getdargs(_tmp2D5,dsc);_tmp2CB->f3=_tmp53B;});_tmp2CB;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA->hd=work_frame,_tmp2CA->tl=work;_tmp2CA;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL8: _LL9:
# 1484
 return({void*_tmp53D=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp53D,rules);});default: _LLA: _LLB: {
# 1486
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));({struct _tuple21*_tmp53E=({struct _tuple21*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->f1=_tmp2D5,_tmp2D2->f2=0;_tmp2D2;});_tmp2D3->hd=_tmp53E;}),_tmp2D3->tl=ctx;_tmp2D3;});
struct _tuple20*work_frame=({struct _tuple20*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->f1=_tmp2D4,({struct Cyc_List_List*_tmp540=Cyc_Tcpat_getoargs(_tmp2D5,obj);_tmp2D1->f2=_tmp540;}),({
struct Cyc_List_List*_tmp53F=Cyc_Tcpat_getdargs(_tmp2D5,dsc);_tmp2D1->f3=_tmp53F;});_tmp2D1;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->hd=work_frame,_tmp2D0->tl=work;_tmp2D0;});
void*_tmp2CE=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp2CF=({void*_tmp543=({struct Cyc_List_List*_tmp542=ctx;void*_tmp541=Cyc_Tcpat_add_neg(dsc,_tmp2D5);Cyc_Tcpat_build_desc(_tmp542,_tmp541,work);});Cyc_Tcpat_or_match(_tmp543,rules);});
# 1493
return Cyc_Tcpat_ifeq(obj,_tmp2D5,_tmp2CE,_tmp2CF);}}_LL5:;}}_LL0:;}
# 1503
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1508
void*_tmp2D6=d;struct Cyc_List_List*_tmp2DD;void*_tmp2DC;struct Cyc_Tcpat_Rhs*_tmp2DB;void*_tmp2DA;switch(*((int*)_tmp2D6)){case 0U: _LL1: _tmp2DA=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2D6)->f1;_LL2:
# 1510
 if(!(*exhaust_done)){not_exhaust(env1,_tmp2DA);*exhaust_done=1;}
goto _LL0;case 1U: _LL3: _tmp2DB=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2D6)->f1;_LL4:
 rhs_appears(env2,_tmp2DB);goto _LL0;default: _LL5: _tmp2DD=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D6)->f2;_tmp2DC=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D6)->f3;_LL6:
# 1514
 for(0;_tmp2DD != 0;_tmp2DD=_tmp2DD->tl){
struct _tuple0 _tmp2D7=*((struct _tuple0*)_tmp2DD->hd);struct _tuple0 _tmp2D8=_tmp2D7;void*_tmp2D9;_LL8: _tmp2D9=_tmp2D8.f2;_LL9:;
Cyc_Tcpat_check_exhaust_overlap(_tmp2D9,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1519
Cyc_Tcpat_check_exhaust_overlap(_tmp2DC,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1521
goto _LL0;}_LL0:;}
# 1529
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1531
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3->used=0,_tmp2E3->pat_loc=(swc->pattern)->loc,_tmp2E3->rhs=swc->body;_tmp2E3;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp2DE=({union Cyc_Tcpat_PatOrWhere _tmp3BA;(_tmp3BA.where_clause).tag=2U,(_tmp3BA.where_clause).val=swc->where_clause;_tmp3BA;});
sp=({struct Cyc_List_List*_tmp545=({void*_tmp2DF[2U];_tmp2DF[0]=sp0,({void*_tmp544=Cyc_Tcpat_int_pat(*ctr,_tmp2DE);_tmp2DF[1]=_tmp544;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2DF,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp545,_tmp2DE);});
*ctr=*ctr + 1;}else{
# 1539
sp=({struct Cyc_List_List*_tmp547=({void*_tmp2E0[2U];_tmp2E0[0]=sp0,({void*_tmp546=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2E1=_cycalloc(sizeof(*_tmp2E1));_tmp2E1->tag=0U;_tmp2E1;});_tmp2E0[1]=_tmp546;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2E0,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp547,({union Cyc_Tcpat_PatOrWhere _tmp3BB;(_tmp3BB.where_clause).tag=2U,(_tmp3BB.where_clause).val=0;_tmp3BB;}));});}
return({struct _tuple22*_tmp2E2=_cycalloc(sizeof(*_tmp2E2));_tmp2E2->f1=sp,_tmp2E2->f2=rhs;_tmp2E2;});}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1545
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1547
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp2E4=d;struct Cyc_Set_Set*_tmp345;struct Cyc_Tcpat_Con_s*_tmp344;struct Cyc_List_List*_tmp343;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E4)->tag == 0U){_LL1: _tmp344=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E4)->f1;_tmp343=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E4)->f2;_LL2: {
# 1551
union Cyc_Tcpat_Name_value _tmp2E5=_tmp344->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp2E6=_tmp344->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp2E7=_tmp2E6;struct Cyc_Absyn_Pat*_tmp2E8;if((_tmp2E7.where_clause).tag == 2){_LL6: _LL7:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp343))->hd);}else{_LL8: _tmp2E8=(_tmp2E7.pattern).val;_LL9:
 p=_tmp2E8;goto _LL5;}_LL5:;}{
# 1557
void*_tmp2E9=p->r;void*_tmp2EA=_tmp2E9;struct Cyc_Absyn_Exp*_tmp31F;struct Cyc_Absyn_Enumfield*_tmp31E;struct Cyc_Absyn_Enumfield*_tmp31D;struct _fat_ptr _tmp31C;int _tmp31B;char _tmp31A;int _tmp319;struct Cyc_Absyn_Datatypefield*_tmp318;struct Cyc_Absyn_Aggrdecl*_tmp317;struct Cyc_List_List*_tmp316;struct Cyc_Absyn_Tvar*_tmp315;struct Cyc_Absyn_Vardecl*_tmp314;struct Cyc_Absyn_Vardecl*_tmp313;struct Cyc_Absyn_Vardecl*_tmp312;switch(*((int*)_tmp2EA)){case 0U: _LLB: _LLC:
 return({const char*_tmp2EB="_";_tag_fat(_tmp2EB,sizeof(char),2U);});case 1U: _LLD: _tmp312=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1;_LLE:
 return Cyc_Absynpp_qvar2string(_tmp312->name);case 3U: _LLF: _tmp313=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1;_LL10:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EE=({struct Cyc_String_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=0U,({struct _fat_ptr _tmp548=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp313->name));_tmp3BC.f1=_tmp548;});_tmp3BC;});void*_tmp2EC[1U];_tmp2EC[0]=& _tmp2EE;({struct _fat_ptr _tmp549=({const char*_tmp2ED="*%s";_tag_fat(_tmp2ED,sizeof(char),4U);});Cyc_aprintf(_tmp549,_tag_fat(_tmp2EC,sizeof(void*),1U));});});case 4U: _LL11: _tmp315=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1;_tmp314=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2EA)->f2;_LL12:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F1=({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0U,({struct _fat_ptr _tmp54A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp314->name));_tmp3BE.f1=_tmp54A;});_tmp3BE;});struct Cyc_String_pa_PrintArg_struct _tmp2F2=({struct Cyc_String_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=0U,_tmp3BD.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp315->name);_tmp3BD;});void*_tmp2EF[2U];_tmp2EF[0]=& _tmp2F1,_tmp2EF[1]=& _tmp2F2;({struct _fat_ptr _tmp54B=({const char*_tmp2F0="%s<`%s>";_tag_fat(_tmp2F0,sizeof(char),8U);});Cyc_aprintf(_tmp54B,_tag_fat(_tmp2EF,sizeof(void*),2U));});});case 5U: _LL13: _LL14:
# 1563
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F5=({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0U,({struct _fat_ptr _tmp54C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp343));_tmp3BF.f1=_tmp54C;});_tmp3BF;});void*_tmp2F3[1U];_tmp2F3[0]=& _tmp2F5;({struct _fat_ptr _tmp54D=({const char*_tmp2F4="$(%s)";_tag_fat(_tmp2F4,sizeof(char),6U);});Cyc_aprintf(_tmp54D,_tag_fat(_tmp2F3,sizeof(void*),1U));});});case 6U: _LL15: _LL16:
# 1565
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F8=({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0U,({struct _fat_ptr _tmp54E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp343));_tmp3C0.f1=_tmp54E;});_tmp3C0;});void*_tmp2F6[1U];_tmp2F6[0]=& _tmp2F8;({struct _fat_ptr _tmp54F=({const char*_tmp2F7="&%s";_tag_fat(_tmp2F7,sizeof(char),4U);});Cyc_aprintf(_tmp54F,_tag_fat(_tmp2F6,sizeof(void*),1U));});});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1)->KnownAggr).tag == 2){_LL17: _tmp317=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1)->KnownAggr).val;_tmp316=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2EA)->f3;_LL18:
# 1567
 if((int)_tmp317->kind == (int)Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2F9=_tmp316;struct _fat_ptr*_tmp2FF;if(_tmp2F9 != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp2F9)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp2F9)->hd)->f1)->hd)->tag == 1U){_LL2C: _tmp2FF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp2F9->hd)->f1)->hd)->f1;_LL2D:
# 1570
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FC=({struct Cyc_String_pa_PrintArg_struct _tmp3C3;_tmp3C3.tag=0U,({struct _fat_ptr _tmp550=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp317->name));_tmp3C3.f1=_tmp550;});_tmp3C3;});struct Cyc_String_pa_PrintArg_struct _tmp2FD=({struct Cyc_String_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=0U,_tmp3C2.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp2FF);_tmp3C2;});struct Cyc_String_pa_PrintArg_struct _tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp3C1;_tmp3C1.tag=0U,({
struct _fat_ptr _tmp551=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp343));_tmp3C1.f1=_tmp551;});_tmp3C1;});void*_tmp2FA[3U];_tmp2FA[0]=& _tmp2FC,_tmp2FA[1]=& _tmp2FD,_tmp2FA[2]=& _tmp2FE;({struct _fat_ptr _tmp552=({const char*_tmp2FB="%s{.%s = %s}";_tag_fat(_tmp2FB,sizeof(char),13U);});Cyc_aprintf(_tmp552,_tag_fat(_tmp2FA,sizeof(void*),3U));});});}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 1575
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp302=({struct Cyc_String_pa_PrintArg_struct _tmp3C5;_tmp3C5.tag=0U,({struct _fat_ptr _tmp553=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp317->name));_tmp3C5.f1=_tmp553;});_tmp3C5;});struct Cyc_String_pa_PrintArg_struct _tmp303=({struct Cyc_String_pa_PrintArg_struct _tmp3C4;_tmp3C4.tag=0U,({struct _fat_ptr _tmp554=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp343));_tmp3C4.f1=_tmp554;});_tmp3C4;});void*_tmp300[2U];_tmp300[0]=& _tmp302,_tmp300[1]=& _tmp303;({struct _fat_ptr _tmp555=({const char*_tmp301="%s{%s}";_tag_fat(_tmp301,sizeof(char),7U);});Cyc_aprintf(_tmp555,_tag_fat(_tmp300,sizeof(void*),2U));});});}else{goto _LL29;}}else{goto _LL29;}case 8U: _LL19: _tmp318=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2EA)->f2;_LL1A:
# 1577
 if(_tmp343 == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp306=({struct Cyc_String_pa_PrintArg_struct _tmp3C6;_tmp3C6.tag=0U,({struct _fat_ptr _tmp556=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp318->name));_tmp3C6.f1=_tmp556;});_tmp3C6;});void*_tmp304[1U];_tmp304[0]=& _tmp306;({struct _fat_ptr _tmp557=({const char*_tmp305="%s";_tag_fat(_tmp305,sizeof(char),3U);});Cyc_aprintf(_tmp557,_tag_fat(_tmp304,sizeof(void*),1U));});});else{
# 1580
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp309=({struct Cyc_String_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=0U,({struct _fat_ptr _tmp558=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp318->name));_tmp3C8.f1=_tmp558;});_tmp3C8;});struct Cyc_String_pa_PrintArg_struct _tmp30A=({struct Cyc_String_pa_PrintArg_struct _tmp3C7;_tmp3C7.tag=0U,({struct _fat_ptr _tmp559=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(_tmp343));_tmp3C7.f1=_tmp559;});_tmp3C7;});void*_tmp307[2U];_tmp307[0]=& _tmp309,_tmp307[1]=& _tmp30A;({struct _fat_ptr _tmp55A=({const char*_tmp308="%s(%s)";_tag_fat(_tmp308,sizeof(char),7U);});Cyc_aprintf(_tmp55A,_tag_fat(_tmp307,sizeof(void*),2U));});});}case 9U: _LL1B: _LL1C:
 return({const char*_tmp30B="NULL";_tag_fat(_tmp30B,sizeof(char),5U);});case 10U: _LL1D: _tmp319=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2EA)->f2;_LL1E:
 return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30E=({struct Cyc_Int_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=1U,_tmp3C9.f1=(unsigned long)_tmp319;_tmp3C9;});void*_tmp30C[1U];_tmp30C[0]=& _tmp30E;({struct _fat_ptr _tmp55B=({const char*_tmp30D="%d";_tag_fat(_tmp30D,sizeof(char),3U);});Cyc_aprintf(_tmp55B,_tag_fat(_tmp30C,sizeof(void*),1U));});});case 11U: _LL1F: _tmp31A=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1;_LL20:
 return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp311=({struct Cyc_Int_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=1U,_tmp3CA.f1=(unsigned long)((int)_tmp31A);_tmp3CA;});void*_tmp30F[1U];_tmp30F[0]=& _tmp311;({struct _fat_ptr _tmp55C=({const char*_tmp310="%d";_tag_fat(_tmp310,sizeof(char),3U);});Cyc_aprintf(_tmp55C,_tag_fat(_tmp30F,sizeof(void*),1U));});});case 12U: _LL21: _tmp31C=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1;_tmp31B=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2EA)->f2;_LL22:
 return _tmp31C;case 13U: _LL23: _tmp31D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2EA)->f2;_LL24:
 _tmp31E=_tmp31D;goto _LL26;case 14U: _LL25: _tmp31E=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2EA)->f2;_LL26:
 return Cyc_Absynpp_qvar2string(_tmp31E->name);case 17U: _LL27: _tmp31F=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2EA)->f1;_LL28:
 return Cyc_Absynpp_exp2string(_tmp31F);default: _LL29: _LL2A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LLA:;};}}else{_LL3: _tmp345=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2E4)->f1;_LL4:
# 1591
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp345))return({const char*_tmp320="_";_tag_fat(_tmp320,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*_tmp321=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp345);
# 1595
if((int)(((_tmp321->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp322=({union Cyc_Tcpat_PatOrWhere _tmp342=_tmp321->orig_pat;if((_tmp342.pattern).tag != 1)_throw_match();(_tmp342.pattern).val;});
void*_tmp323=Cyc_Tcutil_compress((void*)_check_null(_tmp322->topt));void*_tmp324=_tmp323;struct Cyc_Absyn_PtrAtts _tmp341;struct Cyc_Absyn_Aggrdecl*_tmp340;struct Cyc_Absyn_Datatypedecl*_tmp33F;switch(*((int*)_tmp324)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp324)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp324)->f1)->f2 == Cyc_Absyn_Char_sz){_LL31: _LL32:
# 1600
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp325=Cyc_Tcpat_char_con((char)i,_tmp322);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp345,_tmp325))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp328=({struct Cyc_Int_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=1U,_tmp3CB.f1=(unsigned long)i;_tmp3CB;});void*_tmp326[1U];_tmp326[0]=& _tmp328;({struct _fat_ptr _tmp55D=({const char*_tmp327="%d";_tag_fat(_tmp327,sizeof(char),3U);});Cyc_aprintf(_tmp55D,_tag_fat(_tmp326,sizeof(void*),1U));});});}}
# 1605
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL33: _LL34:
# 1608
{unsigned i=0U;for(0;i < (unsigned)-1;++ i){
struct Cyc_Tcpat_Con_s*_tmp329=Cyc_Tcpat_int_con((int)i,_tmp321->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp345,_tmp329))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp32C=({struct Cyc_Int_pa_PrintArg_struct _tmp3CC;_tmp3CC.tag=1U,_tmp3CC.f1=(unsigned long)((int)i);_tmp3CC;});void*_tmp32A[1U];_tmp32A[0]=& _tmp32C;({struct _fat_ptr _tmp55E=({const char*_tmp32B="%d";_tag_fat(_tmp32B,sizeof(char),3U);});Cyc_aprintf(_tmp55E,_tag_fat(_tmp32A,sizeof(void*),1U));});});}}
# 1613
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp324)->f1)->f1).KnownDatatype).tag == 2){_LL37: _tmp33F=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp324)->f1)->f1).KnownDatatype).val;_LL38:
# 1623
 if(_tmp33F->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp330=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp33F->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp330);
for(0;(unsigned)_tmp330;_tmp330=_tmp330->tl){
struct _fat_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp330->hd)->name).f2;
struct Cyc_List_List*_tmp331=((struct Cyc_Absyn_Datatypefield*)_tmp330->hd)->typs;
if(!({struct Cyc_Set_Set*_tmp560=_tmp345;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp560,({struct Cyc_Tcpat_Con_s*_tmp332=_cycalloc(sizeof(*_tmp332));({union Cyc_Tcpat_Name_value _tmp55F=Cyc_Tcpat_Name_v(n);_tmp332->name=_tmp55F;}),_tmp332->arity=0,_tmp332->span=0,_tmp332->orig_pat=_tmp321->orig_pat;_tmp332;}));})){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp331)== 0)
return n;else{
# 1633
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp335=({struct Cyc_String_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=0U,_tmp3CD.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp3CD;});void*_tmp333[1U];_tmp333[0]=& _tmp335;({struct _fat_ptr _tmp561=({const char*_tmp334="%s(...)";_tag_fat(_tmp334,sizeof(char),8U);});Cyc_aprintf(_tmp561,_tag_fat(_tmp333,sizeof(void*),1U));});});}}}
# 1636
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL3B;}case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp324)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp340=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp324)->f1)->f1).KnownAggr).val;if((int)_tmp340->kind == (int)Cyc_Absyn_UnionA){_LL3A: {
# 1638
struct Cyc_List_List*_tmp336=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp340->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp336);
struct _tuple2*_tmp337=_tmp340->name;struct _tuple2*_tmp338=_tmp337;struct _fat_ptr _tmp33E;_LL3E: _tmp33E=*_tmp338->f2;_LL3F:;
for(0;(unsigned)_tmp336;_tmp336=_tmp336->tl){
struct _fat_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp336->hd)->name;
if(!({struct Cyc_Set_Set*_tmp563=_tmp345;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp563,({struct Cyc_Tcpat_Con_s*_tmp339=_cycalloc(sizeof(*_tmp339));({union Cyc_Tcpat_Name_value _tmp562=Cyc_Tcpat_Name_v(n);_tmp339->name=_tmp562;}),_tmp339->arity=0,_tmp339->span=0,_tmp339->orig_pat=_tmp321->orig_pat;_tmp339;}));}))
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33C=({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0U,_tmp3CF.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp33E);_tmp3CF;});struct Cyc_String_pa_PrintArg_struct _tmp33D=({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0U,_tmp3CE.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp3CE;});void*_tmp33A[2U];_tmp33A[0]=& _tmp33C,_tmp33A[1]=& _tmp33D;({struct _fat_ptr _tmp564=({const char*_tmp33B="%s{.%s = _}";_tag_fat(_tmp33B,sizeof(char),12U);});Cyc_aprintf(_tmp564,_tag_fat(_tmp33A,sizeof(void*),2U));});});}
# 1646
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL3B;}}else{goto _LL3B;}default: goto _LL3B;}case 3U: _LL35: _tmp341=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).ptr_atts;_LL36: {
# 1615
void*_tmp32D=_tmp341.nullable;
int is_nullable=Cyc_Tcutil_force_type2bool(0,_tmp32D);
if(is_nullable){
if(!({struct Cyc_Set_Set*_tmp565=_tmp345;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp565,Cyc_Tcpat_null_con(_tmp322));}))
return({const char*_tmp32E="NULL";_tag_fat(_tmp32E,sizeof(char),5U);});}
# 1621
return({const char*_tmp32F="&_";_tag_fat(_tmp32F,sizeof(char),3U);});}default: _LL3B: _LL3C:
# 1647
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL30:;};};}_LL0:;}
# 1651
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _fat_ptr*_tmp346=_cycalloc(sizeof(*_tmp346));({struct _fat_ptr _tmp566=Cyc_Tcpat_desc2string(d);*_tmp346=_tmp566;});_tmp346;});}
# 1654
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _fat_ptr*comma=({struct _fat_ptr*_tmp34B=_cycalloc(sizeof(*_tmp34B));({struct _fat_ptr _tmp567=({const char*_tmp34A=",";_tag_fat(_tmp34A,sizeof(char),2U);});*_tmp34B=_tmp567;});_tmp34B;});
{struct Cyc_List_List*_tmp348=_tmp347;for(0;_tmp348 != 0;_tmp348=((struct Cyc_List_List*)_check_null(_tmp348))->tl){
if(_tmp348->tl != 0){
({struct Cyc_List_List*_tmp568=({struct Cyc_List_List*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->hd=comma,_tmp349->tl=_tmp348->tl;_tmp349;});_tmp348->tl=_tmp568;});
_tmp348=_tmp348->tl;}}}
# 1663
return(struct _fat_ptr)Cyc_strconcat_l(_tmp347);}
# 1666
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){
struct _handler_cons _tmp34C;_push_handler(& _tmp34C);{int _tmp34E=0;if(setjmp(_tmp34C.handler))_tmp34E=1;if(!_tmp34E){
{struct _fat_ptr _tmp34F=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp352=({struct Cyc_String_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=0U,_tmp3D0.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp34F);_tmp3D0;});void*_tmp350[1U];_tmp350[0]=& _tmp352;({unsigned _tmp56A=loc;struct _fat_ptr _tmp569=({const char*_tmp351="patterns may not be exhaustive.\n\tmissing case for %s";_tag_fat(_tmp351,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp56A,_tmp569,_tag_fat(_tmp350,sizeof(void*),1U));});});}
# 1668
;_pop_handler();}else{void*_tmp34D=(void*)Cyc_Core_get_exn_thrown();void*_tmp353=_tmp34D;void*_tmp356;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp353)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1672
({void*_tmp354=0U;({unsigned _tmp56C=loc;struct _fat_ptr _tmp56B=({const char*_tmp355="patterns may not be exhaustive.";_tag_fat(_tmp355,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp56C,_tmp56B,_tag_fat(_tmp354,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp356=_tmp353;_LL4:(int)_rethrow(_tmp356);}_LL0:;}};}
# 1676
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1680
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1686
int _tmp357=0;
int*_tmp358=& _tmp357;
struct Cyc_List_List*_tmp359=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp358,swcs);
void*_tmp35A=Cyc_Tcpat_match_compile(_tmp359);
*dopt=_tmp35A;{
# 1692
int _tmp35B=0;
((void(*)(void*d,void(*not_exhaust)(unsigned,void*),unsigned env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp35A,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp35B);
# 1695
for(0;_tmp359 != 0;_tmp359=_tmp359->tl){
struct _tuple22*_tmp35C=(struct _tuple22*)_tmp359->hd;struct _tuple22*_tmp35D=_tmp35C;int _tmp361;unsigned _tmp360;_LL1: _tmp361=(_tmp35D->f2)->used;_tmp360=(_tmp35D->f2)->pat_loc;_LL2:;
if(!_tmp361){
({void*_tmp35E=0U;({unsigned _tmp56E=_tmp360;struct _fat_ptr _tmp56D=({const char*_tmp35F="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp35F,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp56E,_tmp56D,_tag_fat(_tmp35E,sizeof(void*),0U));});});
# 1700
break;}}};}
# 1709
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp362;_push_handler(& _tmp362);{int _tmp364=0;if(setjmp(_tmp362.handler))_tmp364=1;if(!_tmp364){
{struct _fat_ptr _tmp365=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp368=({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0U,_tmp3D1.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp365);_tmp3D1;});void*_tmp366[1U];_tmp366[0]=& _tmp368;({unsigned _tmp570=(*pr).f1;struct _fat_ptr _tmp56F=({const char*_tmp367="pattern not exhaustive.\n\tmissing case for %s";_tag_fat(_tmp367,sizeof(char),45U);});Cyc_Tcutil_warn(_tmp570,_tmp56F,_tag_fat(_tmp366,sizeof(void*),1U));});});}
# 1712
;_pop_handler();}else{void*_tmp363=(void*)Cyc_Core_get_exn_thrown();void*_tmp369=_tmp363;void*_tmp36C;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp369)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1716
({void*_tmp36A=0U;({unsigned _tmp572=(*pr).f1;struct _fat_ptr _tmp571=({const char*_tmp36B="pattern not exhaustive.";_tag_fat(_tmp36B,sizeof(char),24U);});Cyc_Tcutil_warn(_tmp572,_tmp571,_tag_fat(_tmp36A,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp36C=_tmp369;_LL4:(int)_rethrow(_tmp36C);}_LL0:;}};};}
# 1720
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1724
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->used=0,_tmp373->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_tmp573=Cyc_Absyn_skip_stmt(0U);_tmp373->rhs=_tmp573;});_tmp373;});
struct Cyc_List_List*_tmp36D=({struct Cyc_List_List*_tmp372=_cycalloc(sizeof(*_tmp372));({struct _tuple22*_tmp575=({struct _tuple22*_tmp371=_cycalloc(sizeof(*_tmp371));({void*_tmp574=Cyc_Tcpat_compile_pat(p);_tmp371->f1=_tmp574;}),_tmp371->f2=rhs;_tmp371;});_tmp372->hd=_tmp575;}),_tmp372->tl=0;_tmp372;});
void*_tmp36E=Cyc_Tcpat_match_compile(_tmp36D);
struct _tuple14 _tmp36F=({struct _tuple14 _tmp3D2;_tmp3D2.f1=loc,_tmp3D2.f2=1;_tmp3D2;});
int _tmp370=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp36E,Cyc_Tcpat_not_exhaust_warn,& _tmp36F,Cyc_Tcpat_dummy_fn,0,& _tmp370);
# 1732
*dopt=_tmp36E;
return _tmp36F.f2;}
# 1740
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1744
if(swc->where_clause != 0)
({void*_tmp374=0U;({unsigned _tmp577=((struct Cyc_Absyn_Exp*)_check_null(swc->where_clause))->loc;struct _fat_ptr _tmp576=({const char*_tmp375="&&-clauses not supported in exception handlers.";_tag_fat(_tmp375,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp577,_tmp576,_tag_fat(_tmp374,sizeof(void*),0U));});});{
# 1747
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->used=0,_tmp377->pat_loc=(swc->pattern)->loc,_tmp377->rhs=swc->body;_tmp377;});
return({struct _tuple22*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376->f1=sp0,_tmp376->f2=rhs;_tmp376;});};}
# 1750
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1753
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1757
struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp379=Cyc_Tcpat_match_compile(_tmp378);
*dopt=_tmp379;{
int _tmp37A=0;
((void(*)(void*d,void(*not_exhaust)(unsigned,void*),unsigned env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp379,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp37A);
# 1763
for(0;_tmp378 != 0;_tmp378=_tmp378->tl){
# 1765
if(_tmp378->tl == 0)break;{
struct _tuple22*_tmp37B=(struct _tuple22*)_tmp378->hd;struct _tuple22*_tmp37C=_tmp37B;int _tmp380;unsigned _tmp37F;_LL1: _tmp380=(_tmp37C->f2)->used;_tmp37F=(_tmp37C->f2)->pat_loc;_LL2:;
if(!_tmp380){
({void*_tmp37D=0U;({unsigned _tmp579=_tmp37F;struct _fat_ptr _tmp578=({const char*_tmp37E="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp37E,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp579,_tmp578,_tag_fat(_tmp37D,sizeof(void*),0U));});});
break;}};}};}
# 1774
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp381=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;for(0;_tmp381 != 0;_tmp381=_tmp381->tl){
if((*((struct _tuple15*)_tmp381->hd)).f1 != 0)
return 1;}}
return 0;}

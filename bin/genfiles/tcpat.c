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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 694 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 877
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 885
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 891
void*Cyc_Absyn_compress_kb(void*);
# 902
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 904
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 907
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 909
extern void*Cyc_Absyn_sint_type;
# 912
void*Cyc_Absyn_gen_float_type(unsigned i);
# 914
extern void*Cyc_Absyn_unique_rgn_type;
# 916
extern void*Cyc_Absyn_empty_effect;
# 918
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 920
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 949
void*Cyc_Absyn_bounds_one();
# 976
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1053
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1148
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 73
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);
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
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple13{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 207
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
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
# 273
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 298
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 305
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 308
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 310
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 139
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 141
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 143
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct _tuple15{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 46 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;int _tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List**_tmp4;struct Cyc_Absyn_Aggrdecl**_tmp3;int _tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;switch(*((int*)_tmp1)){case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2: {struct Cyc_List_List*exist_ts=_tmp7;struct Cyc_List_List*dps=_tmp8;int dots=_tmp9;
# 52
if(topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C7;_tmp3C7.tag=0U,({struct _fat_ptr _tmp45B=({const char*_tmpC="cannot determine pattern type";_tag_fat(_tmpC,sizeof(char),30U);});_tmp3C7.f1=_tmp45B;});_tmp3C7;});void*_tmpA[1U];_tmpA[0]=& _tmpB;({unsigned _tmp45C=p->loc;Cyc_Warn_err2(_tmp45C,_tag_fat(_tmpA,sizeof(void*),1U));});});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmpD=t;union Cyc_Absyn_AggrInfo _tmpE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 20U){_LL10: _tmpE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->f1;_LL11: {union Cyc_Absyn_AggrInfo ainfo=_tmpE;
# 57
({void*_tmp45E=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp45D=({union Cyc_Absyn_AggrInfo*_tmpF=_cycalloc(sizeof(*_tmpF));*_tmpF=ainfo;_tmpF;});_tmp10->f1=_tmp45D;}),_tmp10->f2=exist_ts,_tmp10->f3=dps,_tmp10->f4=dots;_tmp10;});p->r=_tmp45E;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}}else{goto _LL12;}}else{_LL12: _LL13:
# 61
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C9;_tmp3C9.tag=0U,({struct _fat_ptr _tmp45F=({const char*_tmp14="pattern expects aggregate type instead of ";_tag_fat(_tmp14,sizeof(char),43U);});_tmp3C9.f1=_tmp45F;});_tmp3C9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp13=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3C8;_tmp3C8.tag=2U,_tmp3C8.f1=(void*)t;_tmp3C8;});void*_tmp11[2U];_tmp11[0]=& _tmp12,_tmp11[1]=& _tmp13;({unsigned _tmp460=p->loc;Cyc_Warn_err2(_tmp460,_tag_fat(_tmp11,sizeof(void*),2U));});});
goto _LLF;}_LLF:;}
# 64
return;}}}else{if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).tag == 2){_LL3: _tmp3=((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).val;_tmp4=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp3;struct Cyc_List_List**exist_ts=_tmp4;struct Cyc_List_List*dps=_tmp5;int dots=_tmp6;
# 67
struct Cyc_Absyn_Aggrdecl*_tmp15=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp15;
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CB;_tmp3CB.tag=0U,({struct _fat_ptr _tmp461=({const char*_tmp19="can't destructure an abstract";_tag_fat(_tmp19,sizeof(char),30U);});_tmp3CB.f1=_tmp461;});_tmp3CB;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp18=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3CA;_tmp3CA.tag=6U,_tmp3CA.f1=ad;_tmp3CA;});void*_tmp16[2U];_tmp16[0]=& _tmp17,_tmp16[1]=& _tmp18;({unsigned _tmp462=p->loc;Cyc_Warn_err2(_tmp462,_tag_fat(_tmp16,sizeof(void*),2U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 73
int more_exists=({int _tmp463=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp463 - ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*exist_ts);});
if(more_exists < 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CC;_tmp3CC.tag=0U,({struct _fat_ptr _tmp464=({const char*_tmp1C="too many existential type variables in pattern";_tag_fat(_tmp1C,sizeof(char),47U);});_tmp3CC.f1=_tmp464;});_tmp3CC;});void*_tmp1A[1U];_tmp1A[0]=& _tmp1B;({unsigned _tmp465=p->loc;Cyc_Warn_err2(_tmp465,_tag_fat(_tmp1A,sizeof(void*),1U));});});{
struct Cyc_List_List**_tmp1D=exist_ts;struct Cyc_List_List**ts=_tmp1D;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);for(0;n != 0;-- n){
ts=&((struct Cyc_List_List*)_check_null(*ts))->tl;}}
*ts=0;}}else{
if(more_exists > 0){
# 82
struct Cyc_List_List*_tmp1E=0;struct Cyc_List_List*new_ts=_tmp1E;
for(0;more_exists != 0;-- more_exists){
new_ts=({struct Cyc_List_List*_tmp20=_cycalloc(sizeof(*_tmp20));({struct Cyc_Absyn_Tvar*_tmp466=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=1U,_tmp1F->f1=0;_tmp1F;}));_tmp20->hd=_tmp466;}),_tmp20->tl=new_ts;_tmp20;});}
({struct Cyc_List_List*_tmp467=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*exist_ts,new_ts);*exist_ts=_tmp467;});}}
# 87
return;}}}else{_LLB: _LLC:
# 100
({struct Cyc_Warn_String_Warn_Warg_struct _tmp30=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CD;_tmp3CD.tag=0U,({struct _fat_ptr _tmp468=({const char*_tmp31="resolve_pat unknownAggr";_tag_fat(_tmp31,sizeof(char),24U);});_tmp3CD.f1=_tmp468;});_tmp3CD;});void*_tmp2F[1U];_tmp2F[0]=& _tmp30;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2F,sizeof(void*),1U));});}}case 17U: _LL5: _tmp2=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp2;
# 89
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcutil_is_const_exp(e)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CE;_tmp3CE.tag=0U,({struct _fat_ptr _tmp469=({const char*_tmp23="non-constant expression in case pattern";_tag_fat(_tmp23,sizeof(char),40U);});_tmp3CE.f1=_tmp469;});_tmp3CE;});void*_tmp21[1U];_tmp21[0]=& _tmp22;({unsigned _tmp46A=p->loc;Cyc_Warn_err2(_tmp46A,_tag_fat(_tmp21,sizeof(void*),1U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 94
struct _tuple15 _tmp24=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple15 _stmttmp1=_tmp24;struct _tuple15 _tmp25=_stmttmp1;int _tmp27;unsigned _tmp26;_LL15: _tmp26=_tmp25.f1;_tmp27=_tmp25.f2;_LL16: {unsigned i=_tmp26;int known=_tmp27;
({void*_tmp46B=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=10U,_tmp28->f1=Cyc_Absyn_None,_tmp28->f2=(int)i;_tmp28;});p->r=_tmp46B;});
return;}}}case 15U: _LL7: _LL8:
# 98
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CF;_tmp3CF.tag=0U,({struct _fat_ptr _tmp46C=({const char*_tmp2B="resolve_pat UnknownId_p";_tag_fat(_tmp2B,sizeof(char),24U);});_tmp3CF.f1=_tmp46C;});_tmp3CF;});void*_tmp29[1U];_tmp29[0]=& _tmp2A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp29,sizeof(void*),1U));});case 16U: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D0;_tmp3D0.tag=0U,({struct _fat_ptr _tmp46D=({const char*_tmp2E="resolve_pat UnknownCall_p";_tag_fat(_tmp2E,sizeof(char),26U);});_tmp3D0.f1=_tmp46D;});_tmp3D0;});void*_tmp2C[1U];_tmp2C[0]=& _tmp2D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C,sizeof(void*),1U));});default: _LLD: _LLE:
# 102
 return;}_LL0:;}
# 106
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 109
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->v=s;_tmp32;}));}
# 114
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 118
if(topt != 0 && Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;
# 121
{void*_tmp33=Cyc_Tcutil_compress(numt);void*_stmttmp2=_tmp33;void*_tmp34=_stmttmp2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34)->f1)){case 15U: _LL1: _LL2:
 goto _LL4;case 16U: _LL3: _LL4:
 if(topt != 0)return*topt;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 126
return numt;}struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 129
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 131
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp46E=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_tmp46E;});}
# 135
({struct Cyc_List_List*_tmp470=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _tuple16*_tmp46F=({struct _tuple16*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->f1=vd,_tmp35->f2=e;_tmp35;});_tmp36->hd=_tmp46F;}),_tmp36->tl=*v_result_ptr;_tmp36;});*v_result_ptr=_tmp470;});}
# 137
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 139
struct Cyc_Tcpat_TcPatResult _tmp37=res1;struct Cyc_List_List*_tmp39;struct _tuple1*_tmp38;_LL1: _tmp38=_tmp37.tvars_and_bounds_opt;_tmp39=_tmp37.patvars;_LL2: {struct _tuple1*p1=_tmp38;struct Cyc_List_List*vs1=_tmp39;
struct Cyc_Tcpat_TcPatResult _tmp3A=res2;struct Cyc_List_List*_tmp3C;struct _tuple1*_tmp3B;_LL4: _tmp3B=_tmp3A.tvars_and_bounds_opt;_tmp3C=_tmp3A.patvars;_LL5: {struct _tuple1*p2=_tmp3B;struct Cyc_List_List*vs2=_tmp3C;
if(p1 != 0 || p2 != 0){
if(p1 == 0)
p1=({struct _tuple1*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->f1=0,_tmp3D->f2=0;_tmp3D;});
if(p2 == 0)
p2=({struct _tuple1*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->f1=0,_tmp3E->f2=0;_tmp3E;});
return({struct Cyc_Tcpat_TcPatResult _tmp3D1;({struct _tuple1*_tmp474=({struct _tuple1*_tmp3F=_cycalloc(sizeof(*_tmp3F));({struct Cyc_List_List*_tmp473=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*p1).f1,(*p2).f1);_tmp3F->f1=_tmp473;}),({
struct Cyc_List_List*_tmp472=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*p1).f2,(*p2).f2);_tmp3F->f2=_tmp472;});_tmp3F;});
# 146
_tmp3D1.tvars_and_bounds_opt=_tmp474;}),({
# 148
struct Cyc_List_List*_tmp471=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(vs1,vs2);_tmp3D1.patvars=_tmp471;});_tmp3D1;});}
# 150
return({struct Cyc_Tcpat_TcPatResult _tmp3D2;_tmp3D2.tvars_and_bounds_opt=0,({struct Cyc_List_List*_tmp475=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(vs1,vs2);_tmp3D2.patvars=_tmp475;});_tmp3D2;});}}}
# 153
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){
return({struct Cyc_Absyn_Pat*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->loc=loc,_tmp40->topt=0,_tmp40->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp40;});}
# 158
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp477=t;Cyc_Tcutil_promote_array(_tmp477,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp41=_cycalloc(sizeof(*_tmp41));({struct Cyc_List_List*_tmp476=Cyc_Tcenv_lookup_type_vars(te);_tmp41->v=_tmp476;});_tmp41;})): rgn_opt,0);}): t;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;int f2;};
# 165
static struct _tuple17*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple17*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=tv,_tmp42->f2=0;_tmp42;});}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 169
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 172
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp3FD;_tmp3FD.tvars_and_bounds_opt=0,_tmp3FD.patvars=0;_tmp3FD;});
# 177
{void*_tmp43=p->r;void*_stmttmp3=_tmp43;void*_tmp44=_stmttmp3;int _tmp48;struct Cyc_List_List**_tmp47;struct Cyc_Absyn_Datatypefield*_tmp46;struct Cyc_Absyn_Datatypedecl*_tmp45;int _tmp4C;struct Cyc_List_List**_tmp4B;struct Cyc_List_List*_tmp4A;struct Cyc_Absyn_Aggrdecl*_tmp49;int _tmp4E;struct Cyc_List_List**_tmp4D;struct Cyc_Absyn_Pat*_tmp4F;void*_tmp50;struct Cyc_Absyn_Enumdecl*_tmp51;int _tmp52;struct Cyc_Absyn_Vardecl*_tmp54;struct Cyc_Absyn_Tvar*_tmp53;struct Cyc_Absyn_Pat*_tmp56;struct Cyc_Absyn_Vardecl*_tmp55;struct Cyc_Absyn_Vardecl*_tmp58;struct Cyc_Absyn_Tvar*_tmp57;struct Cyc_Absyn_Pat*_tmp5A;struct Cyc_Absyn_Vardecl*_tmp59;switch(*((int*)_tmp44)){case 0U: _LL1: _LL2:
# 180
 if(topt != 0)
t=*topt;else{
# 183
t=({struct Cyc_List_List*_tmp478=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp478,topt);});}
goto _LL0;case 1U: _LL3: _tmp59=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_tmp5A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp44)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp59;struct Cyc_Absyn_Pat*p2=_tmp5A;
# 187
struct _tuple2*_tmp5B=vd->name;struct _tuple2*_stmttmp4=_tmp5B;struct _tuple2*_tmp5C=_stmttmp4;struct _fat_ptr _tmp5D;_LL2E: _tmp5D=*_tmp5C->f2;_LL2F: {struct _fat_ptr v=_tmp5D;
if(({struct _fat_ptr _tmp47A=(struct _fat_ptr)v;Cyc_strcmp(_tmp47A,({const char*_tmp5E="true";_tag_fat(_tmp5E,sizeof(char),5U);}));})== 0 ||({struct _fat_ptr _tmp479=(struct _fat_ptr)v;Cyc_strcmp(_tmp479,({const char*_tmp5F="false";_tag_fat(_tmp5F,sizeof(char),6U);}));})== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp61=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D5;_tmp3D5.tag=0U,({struct _fat_ptr _tmp47B=({const char*_tmp65="you probably do not want to use ";_tag_fat(_tmp65,sizeof(char),33U);});_tmp3D5.f1=_tmp47B;});_tmp3D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp62=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D4;_tmp3D4.tag=0U,_tmp3D4.f1=v;_tmp3D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp63=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D3;_tmp3D3.tag=0U,({
struct _fat_ptr _tmp47C=({const char*_tmp64=" as a local variable";_tag_fat(_tmp64,sizeof(char),21U);});_tmp3D3.f1=_tmp47C;});_tmp3D3;});void*_tmp60[3U];_tmp60[0]=& _tmp61,_tmp60[1]=& _tmp62,_tmp60[2]=& _tmp63;({unsigned _tmp47D=p->loc;Cyc_Warn_warn2(_tmp47D,_tag_fat(_tmp60,sizeof(void*),3U));});});
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(p2->topt);
# 195
{void*_tmp66=Cyc_Tcutil_compress(t);void*_stmttmp5=_tmp66;void*_tmp67=_stmttmp5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67)->tag == 4U){_LL31: _LL32:
# 197
 if(rgn_pat == 0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp69=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D6;_tmp3D6.tag=0U,({struct _fat_ptr _tmp47E=({const char*_tmp6A="array reference would point into unknown/unallowed region";_tag_fat(_tmp6A,sizeof(char),58U);});_tmp3D6.f1=_tmp47E;});_tmp3D6;});void*_tmp68[1U];_tmp68[0]=& _tmp69;({unsigned _tmp47F=p->loc;Cyc_Warn_err2(_tmp47F,_tag_fat(_tmp68,sizeof(void*),1U));});});
goto _LL30;}else{_LL33: _LL34:
# 201
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D7;_tmp3D7.tag=0U,({struct _fat_ptr _tmp480=({const char*_tmp6D="pattern would point to an abstract member";_tag_fat(_tmp6D,sizeof(char),42U);});_tmp3D7.f1=_tmp480;});_tmp3D7;});void*_tmp6B[1U];_tmp6B[0]=& _tmp6C;({unsigned _tmp481=p->loc;Cyc_Warn_err2(_tmp481,_tag_fat(_tmp6B,sizeof(void*),1U));});});
goto _LL30;}_LL30:;}
# 205
({struct Cyc_Absyn_Vardecl**_tmp484=({struct Cyc_Absyn_Vardecl**_tmp6E=_cycalloc(sizeof(*_tmp6E));*_tmp6E=vd;_tmp6E;});struct Cyc_Absyn_Exp*_tmp483=access_exp;struct Cyc_List_List**_tmp482=& res.patvars;Cyc_Tcpat_set_vd(_tmp484,_tmp483,_tmp482,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}}case 2U: _LL5: _tmp57=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_tmp58=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp44)->f2;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp57;struct Cyc_Absyn_Vardecl*vd=_tmp58;
# 208
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp486=p->loc;struct Cyc_Tcenv_Tenv*_tmp485=te;Cyc_Tcenv_add_type_vars(_tmp486,_tmp485,({struct Cyc_Absyn_Tvar*_tmp7B[1U];_tmp7B[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp7B,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp487=({struct _tuple1*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=0,_tmp6F->f2=0;_tmp6F;});res.tvars_and_bounds_opt=_tmp487;});
({struct Cyc_List_List*_tmp48A=({
struct Cyc_List_List*_tmp489=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp489,({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));({struct _tuple17*_tmp488=({struct _tuple17*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->f1=tv,_tmp70->f2=1;_tmp70;});_tmp71->hd=_tmp488;}),_tmp71->tl=0;_tmp71;}));});
# 211
(*res.tvars_and_bounds_opt).f1=_tmp48A;});
# 213
({unsigned _tmp48D=p->loc;struct Cyc_Tcenv_Tenv*_tmp48C=te2;struct Cyc_List_List*_tmp48B=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp48D,_tmp48C,_tmp48B,& Cyc_Tcutil_tmk,1,0,vd->type);});
# 216
if(topt != 0)t=*topt;else{
t=({struct Cyc_List_List*_tmp48E=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp48E,topt);});}
{void*_tmp72=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmp72;void*_tmp73=_stmttmp6;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp73)->tag == 4U){_LL36: _LL37:
# 220
 if(rgn_pat == 0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp75=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D8;_tmp3D8.tag=0U,({struct _fat_ptr _tmp48F=({const char*_tmp76="array reference would point into unknown/unallowed region";_tag_fat(_tmp76,sizeof(char),58U);});_tmp3D8.f1=_tmp48F;});_tmp3D8;});void*_tmp74[1U];_tmp74[0]=& _tmp75;({unsigned _tmp490=p->loc;Cyc_Warn_err2(_tmp490,_tag_fat(_tmp74,sizeof(void*),1U));});});
goto _LL35;}else{_LL38: _LL39:
# 224
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp78=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D9;_tmp3D9.tag=0U,({struct _fat_ptr _tmp491=({const char*_tmp79="pattern would point to an abstract member";_tag_fat(_tmp79,sizeof(char),42U);});_tmp3D9.f1=_tmp491;});_tmp3D9;});void*_tmp77[1U];_tmp77[0]=& _tmp78;({unsigned _tmp492=p->loc;Cyc_Warn_err2(_tmp492,_tag_fat(_tmp77,sizeof(void*),1U));});});
goto _LL35;}_LL35:;}
# 228
({struct Cyc_Absyn_Vardecl**_tmp495=({struct Cyc_Absyn_Vardecl**_tmp7A=_cycalloc(sizeof(*_tmp7A));*_tmp7A=vd;_tmp7A;});struct Cyc_Absyn_Exp*_tmp494=access_exp;struct Cyc_List_List**_tmp493=& res.patvars;Cyc_Tcpat_set_vd(_tmp495,_tmp494,_tmp493,vd->type);});
goto _LL0;}case 3U: _LL7: _tmp55=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_tmp56=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp44)->f2;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp55;struct Cyc_Absyn_Pat*p2=_tmp56;
# 232
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(p2->topt);
if(!allow_ref_pat || rgn_pat == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DA;_tmp3DA.tag=0U,({struct _fat_ptr _tmp496=({const char*_tmp7E="* pattern would point into an unknown/unallowed region";_tag_fat(_tmp7E,sizeof(char),55U);});_tmp3DA.f1=_tmp496;});_tmp3DA;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7D;({unsigned _tmp497=p->loc;Cyc_Warn_err2(_tmp497,_tag_fat(_tmp7C,sizeof(void*),1U));});});
goto _LL0;}else{
# 239
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp80=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DB;_tmp3DB.tag=0U,({struct _fat_ptr _tmp498=({const char*_tmp81="* pattern cannot take the address of an alias-free path";_tag_fat(_tmp81,sizeof(char),56U);});_tmp3DB.f1=_tmp498;});_tmp3DB;});void*_tmp7F[1U];_tmp7F[0]=& _tmp80;({unsigned _tmp499=p->loc;Cyc_Warn_err2(_tmp499,_tag_fat(_tmp7F,sizeof(void*),1U));});});}{
# 242
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_List_List*_tmp82=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmp82;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=3U,(_tmp84->f1).elt_type=t,({struct Cyc_Absyn_Tqual _tmp49C=Cyc_Absyn_empty_tqual(0U);(_tmp84->f1).elt_tq=_tmp49C;}),
((_tmp84->f1).ptr_atts).rgn=rgn_pat,((_tmp84->f1).ptr_atts).nullable=Cyc_Absyn_false_type,({
void*_tmp49B=Cyc_Tcutil_any_bounds(tvs);((_tmp84->f1).ptr_atts).bounds=_tmp49B;}),({void*_tmp49A=Cyc_Tcutil_any_bool(tvs);((_tmp84->f1).ptr_atts).zero_term=_tmp49A;}),((_tmp84->f1).ptr_atts).ptrloc=0;_tmp84;});
# 248
if((unsigned)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);
new_access_exp->topt=t2;}
# 252
({struct Cyc_Absyn_Vardecl**_tmp49F=({struct Cyc_Absyn_Vardecl**_tmp83=_cycalloc(sizeof(*_tmp83));*_tmp83=vd;_tmp83;});struct Cyc_Absyn_Exp*_tmp49E=new_access_exp;struct Cyc_List_List**_tmp49D=& res.patvars;Cyc_Tcpat_set_vd(_tmp49F,_tmp49E,_tmp49D,t2);});
goto _LL0;}}case 4U: _LL9: _tmp53=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_tmp54=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp44)->f2;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp53;struct Cyc_Absyn_Vardecl*vd=_tmp54;
# 258
({struct Cyc_Absyn_Vardecl**_tmp4A2=({struct Cyc_Absyn_Vardecl**_tmp85=_cycalloc(sizeof(*_tmp85));*_tmp85=vd;_tmp85;});struct Cyc_Absyn_Exp*_tmp4A1=access_exp;struct Cyc_List_List**_tmp4A0=& res.patvars;Cyc_Tcpat_set_vd(_tmp4A2,_tmp4A1,_tmp4A0,vd->type);});
# 262
({unsigned _tmp4A4=p->loc;struct Cyc_Tcenv_Tenv*_tmp4A3=te;Cyc_Tcenv_add_type_vars(_tmp4A4,_tmp4A3,({struct Cyc_Absyn_Tvar*_tmp86[1U];_tmp86[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp86,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp4A5=({struct _tuple1*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->f1=0,_tmp87->f2=0;_tmp87;});res.tvars_and_bounds_opt=_tmp4A5;});
({struct Cyc_List_List*_tmp4A7=({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));
({struct _tuple17*_tmp4A6=({struct _tuple17*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=tv,_tmp88->f2=0;_tmp88;});_tmp89->hd=_tmp4A6;}),_tmp89->tl=(*res.tvars_and_bounds_opt).f1;_tmp89;});
# 265
(*res.tvars_and_bounds_opt).f1=_tmp4A7;});
# 267
t=Cyc_Absyn_uint_type;
goto _LL0;}case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp44)->f1){case Cyc_Absyn_Unsigned: _LLB: _LLC:
# 270
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;case Cyc_Absyn_None: _LLD: _LLE:
 goto _LL10;default: _LLF: _LL10:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;}case 11U: _LL11: _LL12:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 12U: _LL13: _tmp52=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp44)->f2;_LL14: {int i=_tmp52;
t=({void**_tmp4A8=topt;Cyc_Tcpat_num_type(_tmp4A8,Cyc_Absyn_gen_float_type((unsigned)i));});goto _LL0;}case 13U: _LL15: _tmp51=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_LL16: {struct Cyc_Absyn_Enumdecl*ed=_tmp51;
# 276
t=({void**_tmp4A9=topt;Cyc_Tcpat_num_type(_tmp4A9,Cyc_Absyn_enum_type(ed->name,ed));});
goto _LL0;}case 14U: _LL17: _tmp50=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_LL18: {void*tenum=_tmp50;
t=Cyc_Tcpat_num_type(topt,tenum);goto _LL0;}case 9U: _LL19: _LL1A:
# 280
 if(topt != 0){
void*_tmp8A=Cyc_Tcutil_compress(*topt);void*_stmttmp7=_tmp8A;void*_tmp8B=_stmttmp7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->tag == 3U){_LL3B: _LL3C:
 t=*topt;goto tcpat_end;}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}{
# 285
struct Cyc_List_List*_tmp8C=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmp8C;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=3U,({void*_tmp4AE=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->v=tvs;_tmp8D;}));(_tmp8F->f1).elt_type=_tmp4AE;}),({
struct Cyc_Absyn_Tqual _tmp4AD=Cyc_Absyn_empty_tqual(0U);(_tmp8F->f1).elt_tq=_tmp4AD;}),
({void*_tmp4AC=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->v=tvs;_tmp8E;}));((_tmp8F->f1).ptr_atts).rgn=_tmp4AC;}),((_tmp8F->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
# 290
void*_tmp4AB=Cyc_Tcutil_any_bounds(tvs);((_tmp8F->f1).ptr_atts).bounds=_tmp4AB;}),({void*_tmp4AA=Cyc_Tcutil_any_bool(tvs);((_tmp8F->f1).ptr_atts).zero_term=_tmp4AA;}),((_tmp8F->f1).ptr_atts).ptrloc=0;_tmp8F;});
goto _LL0;}case 6U: _LL1B: _tmp4F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_LL1C: {struct Cyc_Absyn_Pat*p2=_tmp4F;
# 296
void*inner_typ=Cyc_Absyn_void_type;
void**_tmp90=0;void**inner_topt=_tmp90;
int elt_const=0;
if(topt != 0){
void*_tmp91=Cyc_Tcutil_compress(*topt);void*_stmttmp8=_tmp91;void*_tmp92=_stmttmp8;struct Cyc_Absyn_Tqual _tmp94;void*_tmp93;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp92)->tag == 3U){_LL40: _tmp93=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp92)->f1).elt_type;_tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp92)->f1).elt_tq;_LL41: {void*elt_typ=_tmp93;struct Cyc_Absyn_Tqual elt_tq=_tmp94;
# 302
inner_typ=elt_typ;
inner_topt=& inner_typ;
elt_const=elt_tq.real_const;
goto _LL3F;}}else{_LL42: _LL43:
 goto _LL3F;}_LL3F:;}{
# 311
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp4AF=res;Cyc_Tcpat_combine_results(_tmp4AF,Cyc_Tcpat_tcPatRec(te,p2,inner_topt,ptr_rgn,1,new_access_exp));});
# 320
{void*_tmp95=Cyc_Tcutil_compress((void*)_check_null(p2->topt));void*_stmttmp9=_tmp95;void*_tmp96=_stmttmp9;struct Cyc_List_List*_tmp99;struct Cyc_Absyn_Datatypefield*_tmp98;struct Cyc_Absyn_Datatypedecl*_tmp97;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96)->f1)->f1).KnownDatatypefield).tag == 2){_LL45: _tmp97=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96)->f1)->f1).KnownDatatypefield).val).f1;_tmp98=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96)->f1)->f1).KnownDatatypefield).val).f2;_tmp99=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96)->f2;_LL46: {struct Cyc_Absyn_Datatypedecl*tud=_tmp97;struct Cyc_Absyn_Datatypefield*tuf=_tmp98;struct Cyc_List_List*targs=_tmp99;
# 324
{void*_tmp9A=Cyc_Tcutil_compress(inner_typ);void*_stmttmpA=_tmp9A;void*_tmp9B=_stmttmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9B)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9B)->f1)->tag == 19U){_LL4A: _LL4B:
# 326
 goto DONT_PROMOTE;}else{goto _LL4C;}}else{_LL4C: _LL4D:
 goto _LL49;}_LL49:;}{
# 330
void*new_type=({union Cyc_Absyn_DatatypeInfo _tmp4B0=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp9D=_cycalloc(sizeof(*_tmp9D));*_tmp9D=tud;_tmp9D;}));Cyc_Absyn_datatype_type(_tmp4B0,targs);});
# 332
p2->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=3U,(_tmp9C->f1).elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _tmp4B4=Cyc_Absyn_const_tqual(0U);(_tmp9C->f1).elt_tq=_tmp4B4;}):({
struct Cyc_Absyn_Tqual _tmp4B3=Cyc_Absyn_empty_tqual(0U);(_tmp9C->f1).elt_tq=_tmp4B3;}),
((_tmp9C->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp4B2=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmp9C->f1).ptr_atts).nullable=_tmp4B2;}),({
void*_tmp4B1=Cyc_Absyn_bounds_one();((_tmp9C->f1).ptr_atts).bounds=_tmp4B1;}),((_tmp9C->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp9C->f1).ptr_atts).ptrloc=0;_tmp9C;});
# 339
goto _LL44;}}}else{goto _LL47;}}else{goto _LL47;}}else{_LL47: _LL48:
# 341
 DONT_PROMOTE: {
struct Cyc_List_List*_tmp9E=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmp9E;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=3U,(_tmp9F->f1).elt_type=(void*)_check_null(p2->topt),
elt_const?({struct Cyc_Absyn_Tqual _tmp4B9=Cyc_Absyn_const_tqual(0U);(_tmp9F->f1).elt_tq=_tmp4B9;}):({
struct Cyc_Absyn_Tqual _tmp4B8=Cyc_Absyn_empty_tqual(0U);(_tmp9F->f1).elt_tq=_tmp4B8;}),
((_tmp9F->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp4B7=Cyc_Tcutil_any_bool(tvs);((_tmp9F->f1).ptr_atts).nullable=_tmp4B7;}),({
void*_tmp4B6=Cyc_Tcutil_any_bounds(tvs);((_tmp9F->f1).ptr_atts).bounds=_tmp4B6;}),({void*_tmp4B5=Cyc_Tcutil_any_bool(tvs);((_tmp9F->f1).ptr_atts).zero_term=_tmp4B5;}),((_tmp9F->f1).ptr_atts).ptrloc=0;_tmp9F;});}}_LL44:;}
# 349
if((unsigned)new_access_exp)new_access_exp->topt=p2->topt;
goto _LL0;}}case 5U: _LL1D: _tmp4D=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_tmp4E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp44)->f2;_LL1E: {struct Cyc_List_List**ps_p=_tmp4D;int dots=_tmp4E;
# 353
struct Cyc_List_List*_tmpA0=*ps_p;struct Cyc_List_List*ps=_tmpA0;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpA1=Cyc_Tcutil_compress(*topt);void*_stmttmpB=_tmpA1;void*_tmpA2=_stmttmpB;struct Cyc_List_List*_tmpA3;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA2)->tag == 6U){_LL4F: _tmpA3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA2)->f1;_LL50: {struct Cyc_List_List*tqts=_tmpA3;
# 359
topt_ts=tqts;
if(dots){
# 362
int _tmpA4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);int lps=_tmpA4;
int _tmpA5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);int lts=_tmpA5;
if(lps < lts){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < lts - lps;++ i){
wild_ps=({struct Cyc_List_List*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct Cyc_Absyn_Pat*_tmp4BA=Cyc_Tcpat_wild_pat(p->loc);_tmpA6->hd=_tmp4BA;}),_tmpA6->tl=wild_ps;_tmpA6;});}}
({struct Cyc_List_List*_tmp4BB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(ps,wild_ps);*ps_p=_tmp4BB;});
ps=*ps_p;}else{
if(({int _tmp4BC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp4BC == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);}))
({void*_tmpA7=0U;({unsigned _tmp4BE=p->loc;struct _fat_ptr _tmp4BD=({const char*_tmpA8="unnecessary ... in tuple pattern";_tag_fat(_tmpA8,sizeof(char),33U);});Cyc_Warn_warn(_tmp4BE,_tmp4BD,_tag_fat(_tmpA7,sizeof(void*),0U));});});}}
# 374
goto _LL4E;}}else{_LL51: _LL52:
# 376
 goto _LL4E;}_LL4E:;}else{
# 378
if(dots)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DC;_tmp3DC.tag=0U,({struct _fat_ptr _tmp4BF=({const char*_tmpAB="cannot determine missing fields for ... in tuple pattern";_tag_fat(_tmpAB,sizeof(char),57U);});_tmp3DC.f1=_tmp4BF;});_tmp3DC;});void*_tmpA9[1U];_tmpA9[0]=& _tmpAA;({unsigned _tmp4C0=p->loc;Cyc_Warn_err2(_tmp4C0,_tag_fat(_tmpA9,sizeof(void*),1U));});});}
{int i=0;for(0;ps != 0;(ps=ps->tl,i ++)){
void**_tmpAC=0;void**inner_topt=_tmpAC;
if(topt_ts != 0){
inner_topt=&(*((struct _tuple18*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 386
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=({struct Cyc_Absyn_Exp*_tmp4C1=access_exp;Cyc_Absyn_subscript_exp(_tmp4C1,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3DD;(_tmp3DD.Int_c).tag=5U,((_tmp3DD.Int_c).val).f1=Cyc_Absyn_Unsigned,((_tmp3DD.Int_c).val).f2=i;_tmp3DD;}),0U),0U);});
# 391
res=({struct Cyc_Tcpat_TcPatResult _tmp4C2=res;Cyc_Tcpat_combine_results(_tmp4C2,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)ps->hd,inner_topt,rgn_pat,allow_ref_pat,new_access_exp));});
# 394
if((unsigned)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)ps->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmpAE=_cycalloc(sizeof(*_tmpAE));({struct _tuple18*_tmp4C4=({struct _tuple18*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct Cyc_Absyn_Tqual _tmp4C3=Cyc_Absyn_empty_tqual(0U);_tmpAD->f1=_tmp4C3;}),_tmpAD->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)ps->hd)->topt);_tmpAD;});_tmpAE->hd=_tmp4C4;}),_tmpAE->tl=pat_ts;_tmpAE;});}}}
# 397
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=6U,({struct Cyc_List_List*_tmp4C5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpAF->f1=_tmp4C5;});_tmpAF;});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp44)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp44)->f1)->KnownAggr).tag == 2){_LL1F: _tmp49=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp44)->f1)->KnownAggr).val;_tmp4A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp44)->f2;_tmp4B=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp44)->f3;_tmp4C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp44)->f4;_LL20: {struct Cyc_Absyn_Aggrdecl*ad=_tmp49;struct Cyc_List_List*exist_ts=_tmp4A;struct Cyc_List_List**dps_p=_tmp4B;int dots=_tmp4C;
# 401
struct Cyc_List_List*_tmpB0=*dps_p;struct Cyc_List_List*dps=_tmpB0;
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DF;_tmp3DF.tag=0U,({struct _fat_ptr _tmp4C6=({const char*_tmpB4="can't destructure abstract ";_tag_fat(_tmpB4,sizeof(char),28U);});_tmp3DF.f1=_tmp4C6;});_tmp3DF;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3DE;_tmp3DE.tag=6U,_tmp3DE.f1=ad;_tmp3DE;});void*_tmpB1[2U];_tmpB1[0]=& _tmpB2,_tmpB1[1]=& _tmpB3;({unsigned _tmp4C7=p->loc;Cyc_Warn_err2(_tmp4C7,_tag_fat(_tmpB1,sizeof(void*),2U));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL0;}
# 409
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
allow_ref_pat=0;
if(exist_ts != 0){
# 415
if(topt == 0 || Cyc_Tcutil_type_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}
# 418
{struct _RegionHandle _tmpB5=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB5;_push_region(rgn);
# 420
{struct Cyc_List_List*_tmpB6=0;struct Cyc_List_List*var_tvs=_tmpB6;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpB7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct Cyc_List_List*u=_tmpB7;
{struct Cyc_List_List*t=exist_ts;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(u))->hd;
u=u->tl;{
void*_tmpB8=Cyc_Absyn_compress_kb(tv->kind);void*kb1=_tmpB8;
void*_tmpB9=Cyc_Absyn_compress_kb(uv->kind);void*kb2=_tmpB9;
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmpBA=kb2;struct Cyc_Absyn_Kind*_tmpBB;struct Cyc_Absyn_Kind*_tmpBC;switch(*((int*)_tmpBA)){case 2U: _LL54: _tmpBC=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBA)->f2;_LL55: {struct Cyc_Absyn_Kind*k=_tmpBC;
_tmpBB=k;goto _LL57;}case 0U: _LL56: _tmpBB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpBA)->f1;_LL57: {struct Cyc_Absyn_Kind*k=_tmpBB;
k2=k;goto _LL53;}default: _LL58: _LL59:
# 435
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E0;_tmp3E0.tag=0U,({struct _fat_ptr _tmp4C8=({const char*_tmpBF="unconstrained existential type variable in aggregate";_tag_fat(_tmpBF,sizeof(char),53U);});_tmp3E0.f1=_tmp4C8;});_tmp3E0;});void*_tmpBD[1U];_tmpBD[0]=& _tmpBE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpBD,sizeof(void*),1U));});}_LL53:;}
# 437
{void*_tmpC0=kb1;struct Cyc_Core_Opt**_tmpC1;struct Cyc_Absyn_Kind*_tmpC3;struct Cyc_Core_Opt**_tmpC2;struct Cyc_Absyn_Kind*_tmpC4;switch(*((int*)_tmpC0)){case 0U: _LL5B: _tmpC4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC0)->f1;_LL5C: {struct Cyc_Absyn_Kind*k1=_tmpC4;
# 440
if(!Cyc_Tcutil_kind_leq(k2,k1))
error=1;
goto _LL5A;}case 2U: _LL5D: _tmpC2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC0)->f1;_tmpC3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC0)->f2;_LL5E: {struct Cyc_Core_Opt**f=_tmpC2;struct Cyc_Absyn_Kind*k=_tmpC3;
_tmpC1=f;goto _LL60;}default: _LL5F: _tmpC1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC0)->f1;_LL60: {struct Cyc_Core_Opt**f=_tmpC1;
({struct Cyc_Core_Opt*_tmp4C9=({struct Cyc_Core_Opt*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->v=kb2;_tmpC5;});*f=_tmp4C9;});goto _LL5A;}}_LL5A:;}
# 446
if(error)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E6;_tmp3E6.tag=0U,({struct _fat_ptr _tmp4CA=({const char*_tmpCF="type variable ";_tag_fat(_tmpCF,sizeof(char),15U);});_tmp3E6.f1=_tmp4CA;});_tmp3E6;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpC8=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp3E5;_tmp3E5.tag=7U,_tmp3E5.f1=tv;_tmp3E5;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E4;_tmp3E4.tag=0U,({struct _fat_ptr _tmp4CB=({const char*_tmpCE=" has kind ";_tag_fat(_tmpCE,sizeof(char),11U);});_tmp3E4.f1=_tmp4CB;});_tmp3E4;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp3E3;_tmp3E3.tag=8U,_tmp3E3.f1=(void*)kb1;_tmp3E3;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E2;_tmp3E2.tag=0U,({
struct _fat_ptr _tmp4CC=({const char*_tmpCD=" but must have at least kind ";_tag_fat(_tmpCD,sizeof(char),30U);});_tmp3E2.f1=_tmp4CC;});_tmp3E2;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp3E1;_tmp3E1.tag=9U,_tmp3E1.f1=k2;_tmp3E1;});void*_tmpC6[6U];_tmpC6[0]=& _tmpC7,_tmpC6[1]=& _tmpC8,_tmpC6[2]=& _tmpC9,_tmpC6[3]=& _tmpCA,_tmpC6[4]=& _tmpCB,_tmpC6[5]=& _tmpCC;({unsigned _tmp4CD=p->loc;Cyc_Warn_err2(_tmp4CD,_tag_fat(_tmpC6,sizeof(void*),6U));});});{
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->tag=2U,_tmpD6->f1=tv;_tmpD6;});
var_tvs=({struct Cyc_List_List*_tmpD0=_region_malloc(rgn,sizeof(*_tmpD0));_tmpD0->hd=vartype,_tmpD0->tl=var_tvs;_tmpD0;});
# 453
if((int)k2->kind == (int)Cyc_Absyn_RgnKind){
if((int)k2->aliasqual == (int)Cyc_Absyn_Aliasable)
outlives_constraints=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));
({struct _tuple0*_tmp4CE=({struct _tuple0*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->f1=Cyc_Absyn_empty_effect,_tmpD1->f2=vartype;_tmpD1;});_tmpD2->hd=_tmp4CE;}),_tmpD2->tl=outlives_constraints;_tmpD2;});else{
# 458
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E7;_tmp3E7.tag=0U,({struct _fat_ptr _tmp4CF=({const char*_tmpD5="opened existential had unique or top region kind";_tag_fat(_tmpD5,sizeof(char),49U);});_tmp3E7.f1=_tmp4CF;});_tmp3E7;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpD3,sizeof(void*),1U));});}}}}}}
# 462
var_tvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(var_tvs);{
# 464
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,exist_ts);
# 466
struct Cyc_List_List*_tmpD7=Cyc_Tcenv_lookup_type_vars(te2);struct Cyc_List_List*tenv_tvs=_tmpD7;
struct _tuple13 _tmpD8=({struct _tuple13 _tmp3F0;_tmp3F0.f1=tenv_tvs,_tmp3F0.f2=rgn;_tmp3F0;});struct _tuple13 env=_tmpD8;
struct Cyc_List_List*_tmpD9=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmpD9;
struct Cyc_List_List*_tmpDA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,var_tvs);struct Cyc_List_List*exist_inst=_tmpDA;
struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmpDB;
struct Cyc_List_List*_tmpDC=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmpDC;
struct Cyc_List_List*_tmpDD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,all_inst,exist_inst);struct Cyc_List_List*inst=_tmpDD;
# 474
if(exist_ts != 0 ||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp4D0=({struct _tuple1*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->f1=0,_tmpDE->f2=0;_tmpDE;});res.tvars_and_bounds_opt=_tmp4D0;});
({struct Cyc_List_List*_tmp4D2=({
struct Cyc_List_List*_tmp4D1=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp4D1,((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,exist_ts));});
# 477
(*res.tvars_and_bounds_opt).f1=_tmp4D2;});
# 479
({struct Cyc_List_List*_tmp4D3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 479
(*res.tvars_and_bounds_opt).f2=_tmp4D3;});{
# 481
struct Cyc_List_List*_tmpDF=0;struct Cyc_List_List*rpo_inst=_tmpDF;
{struct Cyc_List_List*_tmpE0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmpE0;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct _tuple0*_tmp4D6=({struct _tuple0*_tmpE1=_cycalloc(sizeof(*_tmpE1));({void*_tmp4D5=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f1);_tmpE1->f1=_tmp4D5;}),({
void*_tmp4D4=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f2);_tmpE1->f2=_tmp4D4;});_tmpE1;});
# 483
_tmpE2->hd=_tmp4D6;}),_tmpE2->tl=rpo_inst;_tmpE2;});}}
# 486
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
({struct Cyc_List_List*_tmp4D7=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,rpo_inst);
# 487
(*res.tvars_and_bounds_opt).f2=_tmp4D7;});}}
# 492
t=({union Cyc_Absyn_AggrInfo _tmp4D8=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE3=_cycalloc(sizeof(*_tmpE3));*_tmpE3=ad;_tmpE3;}));Cyc_Absyn_aggr_type(_tmp4D8,all_typs);});
if(dots &&(int)ad->kind == (int)Cyc_Absyn_UnionA)
({void*_tmpE4=0U;({unsigned _tmp4DA=p->loc;struct _fat_ptr _tmp4D9=({const char*_tmpE5="`...' pattern not allowed in union pattern";_tag_fat(_tmpE5,sizeof(char),43U);});Cyc_Warn_warn(_tmp4DA,_tmp4D9,_tag_fat(_tmpE4,sizeof(void*),0U));});});else{
if(dots){
# 497
int _tmpE6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dps);int ldps=_tmpE6;
int _tmpE7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);int lfields=_tmpE7;
if(ldps < lfields){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < lfields - ldps;++ i){
wild_dps=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));({struct _tuple19*_tmp4DC=({struct _tuple19*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->f1=0,({struct Cyc_Absyn_Pat*_tmp4DB=Cyc_Tcpat_wild_pat(p->loc);_tmpE8->f2=_tmp4DB;});_tmpE8;});_tmpE9->hd=_tmp4DC;}),_tmpE9->tl=wild_dps;_tmpE9;});}}
({struct Cyc_List_List*_tmp4DD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(dps,wild_dps);*dps_p=_tmp4DD;});
dps=*dps_p;}else{
if(ldps == lfields)
({void*_tmpEA=0U;({unsigned _tmp4DF=p->loc;struct _fat_ptr _tmp4DE=({const char*_tmpEB="unnecessary ... in struct pattern";_tag_fat(_tmpEB,sizeof(char),34U);});Cyc_Warn_warn(_tmp4DF,_tmp4DE,_tag_fat(_tmpEA,sizeof(void*),0U));});});}}}{
# 508
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,dps,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple20*_tmpEC=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmpC=_tmpEC;struct _tuple20*_tmpED=_stmttmpC;struct Cyc_Absyn_Pat*_tmpEF;struct Cyc_Absyn_Aggrfield*_tmpEE;_LL62: _tmpEE=_tmpED->f1;_tmpEF=_tmpED->f2;_LL63: {struct Cyc_Absyn_Aggrfield*field=_tmpEE;struct Cyc_Absyn_Pat*pat=_tmpEF;
void*_tmpF0=Cyc_Tcutil_rsubstitute(rgn,inst,field->type);void*inst_fieldtyp=_tmpF0;
# 514
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,field->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _tmp4E0=res;Cyc_Tcpat_combine_results(_tmp4E0,Cyc_Tcpat_tcPatRec(te2,pat,& inst_fieldtyp,rgn_pat,allow_ref_pat,new_access_exp));});
# 522
if(!Cyc_Unify_unify((void*)_check_null(pat->topt),inst_fieldtyp))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EF;_tmp3EF.tag=0U,({struct _fat_ptr _tmp4E1=({const char*_tmpFD="field ";_tag_fat(_tmpFD,sizeof(char),7U);});_tmp3EF.f1=_tmp4E1;});_tmp3EF;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EE;_tmp3EE.tag=0U,_tmp3EE.f1=*field->name;_tmp3EE;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3ED;_tmp3ED.tag=0U,({struct _fat_ptr _tmp4E2=({const char*_tmpFC=" of ";_tag_fat(_tmpFC,sizeof(char),5U);});_tmp3ED.f1=_tmp4E2;});_tmp3ED;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3EC;_tmp3EC.tag=6U,_tmp3EC.f1=ad;_tmp3EC;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EB;_tmp3EB.tag=0U,({
struct _fat_ptr _tmp4E3=({const char*_tmpFB=" pattern expects type ";_tag_fat(_tmpFB,sizeof(char),23U);});_tmp3EB.f1=_tmp4E3;});_tmp3EB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3EA;_tmp3EA.tag=2U,_tmp3EA.f1=(void*)inst_fieldtyp;_tmp3EA;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E9;_tmp3E9.tag=0U,({struct _fat_ptr _tmp4E4=({const char*_tmpFA=" != ";_tag_fat(_tmpFA,sizeof(char),5U);});_tmp3E9.f1=_tmp4E4;});_tmp3E9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3E8;_tmp3E8.tag=2U,_tmp3E8.f1=(void*)_check_null(pat->topt);_tmp3E8;});void*_tmpF1[8U];_tmpF1[0]=& _tmpF2,_tmpF1[1]=& _tmpF3,_tmpF1[2]=& _tmpF4,_tmpF1[3]=& _tmpF5,_tmpF1[4]=& _tmpF6,_tmpF1[5]=& _tmpF7,_tmpF1[6]=& _tmpF8,_tmpF1[7]=& _tmpF9;({unsigned _tmp4E5=p->loc;Cyc_Warn_err2(_tmp4E5,_tag_fat(_tmpF1,sizeof(void*),8U));});});
# 526
if((unsigned)new_access_exp)new_access_exp->topt=pat->topt;}}}}}
# 420
;_pop_region();}
# 529
goto _LL0;}}else{_LL25: _LL26:
# 578
 goto _LL28;}}else{_LL23: _LL24:
# 577
 goto _LL26;}case 8U: _LL21: _tmp45=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp44)->f1;_tmp46=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp44)->f2;_tmp47=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp44)->f3;_tmp48=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp44)->f4;_LL22: {struct Cyc_Absyn_Datatypedecl*tud=_tmp45;struct Cyc_Absyn_Datatypefield*tuf=_tmp46;struct Cyc_List_List**ps_p=_tmp47;int dots=_tmp48;
# 532
struct Cyc_List_List*_tmpFE=*ps_p;struct Cyc_List_List*ps=_tmpFE;
struct Cyc_List_List*tqts=tuf->typs;
# 535
struct Cyc_List_List*_tmpFF=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmpFF;
struct _tuple13 _tmp100=({struct _tuple13 _tmp3FC;_tmp3FC.f1=tenv_tvs,_tmp3FC.f2=Cyc_Core_heap_region;_tmp3FC;});struct _tuple13 env=_tmp100;
struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp101;
struct Cyc_List_List*_tmp102=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp102;
t=({union Cyc_Absyn_DatatypeFieldInfo _tmp4E6=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp4E6,all_typs);});
if(dots){
# 542
int _tmp103=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);int lps=_tmp103;
int _tmp104=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);int ltqts=_tmp104;
if(lps < ltqts){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < ltqts - lps;++ i){
wild_ps=({struct Cyc_List_List*_tmp105=_cycalloc(sizeof(*_tmp105));({struct Cyc_Absyn_Pat*_tmp4E7=Cyc_Tcpat_wild_pat(p->loc);_tmp105->hd=_tmp4E7;}),_tmp105->tl=wild_ps;_tmp105;});}}
({struct Cyc_List_List*_tmp4E8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(ps,wild_ps);*ps_p=_tmp4E8;});
ps=*ps_p;}else{
if(lps == ltqts)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp107=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F2;_tmp3F2.tag=0U,({struct _fat_ptr _tmp4E9=({const char*_tmp109="unnecessary ... in datatype field ";_tag_fat(_tmp109,sizeof(char),35U);});_tmp3F2.f1=_tmp4E9;});_tmp3F2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp108=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3F1;_tmp3F1.tag=1U,_tmp3F1.f1=tud->name;_tmp3F1;});void*_tmp106[2U];_tmp106[0]=& _tmp107,_tmp106[1]=& _tmp108;({unsigned _tmp4EA=p->loc;Cyc_Warn_warn2(_tmp4EA,_tag_fat(_tmp106,sizeof(void*),2U));});});}}
# 553
for(0;ps != 0 && tqts != 0;(ps=ps->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp10A=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp10A;
# 557
void*_tmp10B=Cyc_Tcutil_substitute(inst,(*((struct _tuple18*)tqts->hd)).f2);void*field_typ=_tmp10B;
# 560
if((unsigned)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
res=({struct Cyc_Tcpat_TcPatResult _tmp4EB=res;Cyc_Tcpat_combine_results(_tmp4EB,Cyc_Tcpat_tcPatRec(te,p2,& field_typ,rgn_pat,allow_ref_pat,0));});
# 567
if(!Cyc_Unify_unify((void*)_check_null(p2->topt),field_typ))
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3F7;_tmp3F7.tag=1U,_tmp3F7.f1=tuf->name;_tmp3F7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F6;_tmp3F6.tag=0U,({struct _fat_ptr _tmp4EC=({const char*_tmp113=" expects argument type ";_tag_fat(_tmp113,sizeof(char),24U);});_tmp3F6.f1=_tmp4EC;});_tmp3F6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F5;_tmp3F5.tag=2U,_tmp3F5.f1=(void*)field_typ;_tmp3F5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp110=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F4;_tmp3F4.tag=0U,({
struct _fat_ptr _tmp4ED=({const char*_tmp112=" not ";_tag_fat(_tmp112,sizeof(char),6U);});_tmp3F4.f1=_tmp4ED;});_tmp3F4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp111=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F3;_tmp3F3.tag=2U,_tmp3F3.f1=(void*)_check_null(p2->topt);_tmp3F3;});void*_tmp10C[5U];_tmp10C[0]=& _tmp10D,_tmp10C[1]=& _tmp10E,_tmp10C[2]=& _tmp10F,_tmp10C[3]=& _tmp110,_tmp10C[4]=& _tmp111;({unsigned _tmp4EE=p2->loc;Cyc_Warn_err2(_tmp4EE,_tag_fat(_tmp10C,sizeof(void*),5U));});});}
# 571
if(ps != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp115=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F9;_tmp3F9.tag=0U,({struct _fat_ptr _tmp4EF=({const char*_tmp117="too many arguments for datatype constructor ";_tag_fat(_tmp117,sizeof(char),45U);});_tmp3F9.f1=_tmp4EF;});_tmp3F9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp116=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3F8;_tmp3F8.tag=1U,_tmp3F8.f1=tuf->name;_tmp3F8;});void*_tmp114[2U];_tmp114[0]=& _tmp115,_tmp114[1]=& _tmp116;({unsigned _tmp4F0=p->loc;Cyc_Warn_err2(_tmp4F0,_tag_fat(_tmp114,sizeof(void*),2U));});});
if(tqts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp119=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FB;_tmp3FB.tag=0U,({struct _fat_ptr _tmp4F1=({const char*_tmp11B="too few arguments for datatype constructor ";_tag_fat(_tmp11B,sizeof(char),44U);});_tmp3FB.f1=_tmp4F1;});_tmp3FB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3FA;_tmp3FA.tag=1U,_tmp3FA.f1=tuf->name;_tmp3FA;});void*_tmp118[2U];_tmp118[0]=& _tmp119,_tmp118[1]=& _tmp11A;({unsigned _tmp4F2=p->loc;Cyc_Warn_err2(_tmp4F2,_tag_fat(_tmp118,sizeof(void*),2U));});});
goto _LL0;}case 15U: _LL27: _LL28:
# 579
 goto _LL2A;case 17U: _LL29: _LL2A:
 goto _LL2C;default: _LL2B: _LL2C:
# 582
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL0;}_LL0:;}
# 584
tcpat_end:
 p->topt=t;
return res;}}
# 589
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 591
struct Cyc_Tcpat_TcPatResult _tmp11C=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);struct Cyc_Tcpat_TcPatResult ans=_tmp11C;
# 593
struct _tuple1 _tmp11D=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(ans.patvars);struct _tuple1 _stmttmpD=_tmp11D;struct _tuple1 _tmp11E=_stmttmpD;struct Cyc_List_List*_tmp11F;_LL1: _tmp11F=_tmp11E.f1;_LL2: {struct Cyc_List_List*vs1=_tmp11F;
struct Cyc_List_List*_tmp120=0;struct Cyc_List_List*vs=_tmp120;
{struct Cyc_List_List*x=vs1;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)vs=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),_tmp121->tl=vs;_tmp121;});}}
({struct Cyc_List_List*_tmp4F4=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_name,vs);unsigned _tmp4F3=p->loc;Cyc_Tcutil_check_unique_vars(_tmp4F4,_tmp4F3,({const char*_tmp122="pattern contains a repeated variable";_tag_fat(_tmp122,sizeof(char),37U);}));});
# 602
{struct Cyc_List_List*x=ans.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp123=(struct _tuple16*)x->hd;struct _tuple16*_stmttmpE=_tmp123;struct _tuple16*_tmp124=_stmttmpE;struct Cyc_Absyn_Exp**_tmp126;struct Cyc_Absyn_Vardecl**_tmp125;_LL4: _tmp125=_tmp124->f1;_tmp126=(struct Cyc_Absyn_Exp**)& _tmp124->f2;_LL5: {struct Cyc_Absyn_Vardecl**vdopt=_tmp125;struct Cyc_Absyn_Exp**expopt=_tmp126;
if(*expopt != 0 &&*expopt != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp4F5=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*expopt));*expopt=_tmp4F5;});}}}
# 607
return ans;}}
# 613
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 616
struct _tuple0 _tmp127=({struct _tuple0 _tmp3FE;({void*_tmp4F7=Cyc_Tcutil_compress(old_type);_tmp3FE.f1=_tmp4F7;}),({void*_tmp4F6=Cyc_Tcutil_compress(new_type);_tmp3FE.f2=_tmp4F6;});_tmp3FE;});struct _tuple0 _stmttmpF=_tmp127;struct _tuple0 _tmp128=_stmttmpF;struct Cyc_Absyn_PtrInfo _tmp12A;struct Cyc_Absyn_PtrInfo _tmp129;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128.f2)->tag == 3U){_LL1: _tmp129=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128.f1)->f1;_tmp12A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pold=_tmp129;struct Cyc_Absyn_PtrInfo pnew=_tmp12A;
# 618
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12B=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=3U,(_tmp12C->f1).elt_type=pold.elt_type,(_tmp12C->f1).elt_tq=pnew.elt_tq,
((_tmp12C->f1).ptr_atts).rgn=(pold.ptr_atts).rgn,((_tmp12C->f1).ptr_atts).nullable=(pnew.ptr_atts).nullable,((_tmp12C->f1).ptr_atts).bounds=(pnew.ptr_atts).bounds,((_tmp12C->f1).ptr_atts).zero_term=(pnew.ptr_atts).zero_term,((_tmp12C->f1).ptr_atts).ptrloc=(pold.ptr_atts).ptrloc;_tmp12C;});
# 618
struct Cyc_Absyn_PointerType_Absyn_Type_struct*ptry=_tmp12B;
# 624
return({struct Cyc_RgnOrder_RgnPO*_tmp4FC=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp4FB=assump;void*_tmp4FA=(void*)ptry;Cyc_Tcutil_subtype(_tmp4FC,_tmp4FB,_tmp4FA,new_type);})&&({
struct Cyc_RgnOrder_RgnPO*_tmp4F9=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_Absyn_Exp*_tmp4F8=initializer;Cyc_Tcutil_coerce_assign(_tmp4F9,_tmp4F8,(void*)ptry);});}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 633
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 636
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*_tmp13D));({struct _tuple0*_tmp4FE=({struct _tuple0*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->f1=Cyc_Absyn_unique_rgn_type,({void*_tmp4FD=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->tag=2U,_tmp13B->f1=tv;_tmp13B;});_tmp13C->f2=_tmp4FD;});_tmp13C;});_tmp13D->hd=_tmp4FE;}),_tmp13D->tl=0;_tmp13D;});
if(({struct Cyc_RgnOrder_RgnPO*_tmp501=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_tmp500=assump;void*_tmp4FF=old_type;Cyc_Tcutil_subtype(_tmp501,_tmp500,_tmp4FF,new_type);})){
# 654 "tcpat.cyc"
struct _tuple0 _tmp12D=({struct _tuple0 _tmp400;({void*_tmp503=Cyc_Tcutil_compress(old_type);_tmp400.f1=_tmp503;}),({void*_tmp502=Cyc_Tcutil_compress(new_type);_tmp400.f2=_tmp502;});_tmp400;});struct _tuple0 _stmttmp10=_tmp12D;struct _tuple0 _tmp12E=_stmttmp10;struct Cyc_Absyn_PtrInfo _tmp130;struct Cyc_Absyn_PtrInfo _tmp12F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E.f2)->tag == 3U){_LL1: _tmp12F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E.f1)->f1;_tmp130=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pold=_tmp12F;struct Cyc_Absyn_PtrInfo pnew=_tmp130;
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp132=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FF;_tmp3FF.tag=0U,({struct _fat_ptr _tmp504=({const char*_tmp133="alias requires pointer type";_tag_fat(_tmp133,sizeof(char),28U);});_tmp3FF.f1=_tmp504;});_tmp3FF;});void*_tmp131[1U];_tmp131[0]=& _tmp132;({unsigned _tmp505=loc;Cyc_Warn_err2(_tmp505,_tag_fat(_tmp131,sizeof(void*),1U));});});goto _LL0;}_LL0:;}else{
# 659
({struct Cyc_Warn_String_Warn_Warg_struct _tmp135=({struct Cyc_Warn_String_Warn_Warg_struct _tmp404;_tmp404.tag=0U,({struct _fat_ptr _tmp506=({const char*_tmp13A="cannot alias value of type ";_tag_fat(_tmp13A,sizeof(char),28U);});_tmp404.f1=_tmp506;});_tmp404;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp136=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp403;_tmp403.tag=2U,_tmp403.f1=(void*)old_type;_tmp403;});struct Cyc_Warn_String_Warn_Warg_struct _tmp137=({struct Cyc_Warn_String_Warn_Warg_struct _tmp402;_tmp402.tag=0U,({struct _fat_ptr _tmp507=({const char*_tmp139=" to type ";_tag_fat(_tmp139,sizeof(char),10U);});_tmp402.f1=_tmp507;});_tmp402;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp138=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp401;_tmp401.tag=2U,_tmp401.f1=(void*)new_type;_tmp401;});void*_tmp134[4U];_tmp134[0]=& _tmp135,_tmp134[1]=& _tmp136,_tmp134[2]=& _tmp137,_tmp134[3]=& _tmp138;({unsigned _tmp508=loc;Cyc_Warn_err2(_tmp508,_tag_fat(_tmp134,sizeof(void*),4U));});});}}
# 665
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 669
void*_tmp13E=p->r;void*_stmttmp11=_tmp13E;void*_tmp13F=_stmttmp11;struct Cyc_Absyn_Vardecl*_tmp141;struct Cyc_Absyn_Tvar*_tmp140;struct Cyc_Absyn_Pat*_tmp143;struct Cyc_Absyn_Vardecl*_tmp142;struct Cyc_Absyn_Pat*_tmp145;struct Cyc_Absyn_Vardecl*_tmp144;struct Cyc_List_List*_tmp146;struct Cyc_List_List*_tmp147;struct Cyc_List_List*_tmp14A;struct Cyc_List_List*_tmp149;union Cyc_Absyn_AggrInfo*_tmp148;struct Cyc_Absyn_Pat*_tmp14B;switch(*((int*)_tmp13F)){case 6U: _LL1: _tmp14B=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp13F)->f1;_LL2: {struct Cyc_Absyn_Pat*p2=_tmp14B;
# 671
void*_tmp14C=(void*)_check_null(p->topt);void*_stmttmp12=_tmp14C;void*_tmp14D=_stmttmp12;void*_tmp14E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->tag == 3U){_LL12: _tmp14E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1).ptr_atts).rgn;_LL13: {void*rt=_tmp14E;
# 673
Cyc_Tcenv_check_rgn_accessible(te,p->loc,rt);
({struct Cyc_Tcenv_Tenv*_tmp50B=te;struct Cyc_Absyn_Pat*_tmp50A=p2;int _tmp509=Cyc_Tcutil_is_noalias_region(rt,0);Cyc_Tcpat_check_pat_regions_rec(_tmp50B,_tmp50A,_tmp509,patvars);});
return;}}else{_LL14: _LL15:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp150=({struct Cyc_Warn_String_Warn_Warg_struct _tmp405;_tmp405.tag=0U,({struct _fat_ptr _tmp50C=({const char*_tmp151="check_pat_regions: bad pointer type";_tag_fat(_tmp151,sizeof(char),36U);});_tmp405.f1=_tmp50C;});_tmp405;});void*_tmp14F[1U];_tmp14F[0]=& _tmp150;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp14F,sizeof(void*),1U));});}_LL11:;}case 7U: _LL3: _tmp148=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp13F)->f1;_tmp149=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp13F)->f2;_tmp14A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp13F)->f3;_LL4: {union Cyc_Absyn_AggrInfo*ai=_tmp148;struct Cyc_List_List*exist_ts=_tmp149;struct Cyc_List_List*dps=_tmp14A;
# 679
for(0;dps != 0;dps=dps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple19*)dps->hd)).f2,did_noalias_deref,patvars);}
return;}case 8U: _LL5: _tmp147=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp13F)->f3;_LL6: {struct Cyc_List_List*ps=_tmp147;
# 683
did_noalias_deref=0;_tmp146=ps;goto _LL8;}case 5U: _LL7: _tmp146=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp13F)->f1;_LL8: {struct Cyc_List_List*ps=_tmp146;
# 685
for(0;ps != 0;ps=ps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)ps->hd,did_noalias_deref,patvars);}
return;}case 3U: _LL9: _tmp144=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp13F)->f1;_tmp145=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp13F)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp144;struct Cyc_Absyn_Pat*p2=_tmp145;
# 689
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp152=(struct _tuple16*)x->hd;struct _tuple16*_stmttmp13=_tmp152;struct _tuple16*_tmp153=_stmttmp13;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Vardecl**_tmp154;_LL17: _tmp154=_tmp153->f1;_tmp155=_tmp153->f2;_LL18: {struct Cyc_Absyn_Vardecl**vdopt=_tmp154;struct Cyc_Absyn_Exp*eopt=_tmp155;
# 695
if((vdopt != 0 &&*vdopt == vd)&& eopt != 0){
{void*_tmp156=eopt->r;void*_stmttmp14=_tmp156;void*_tmp157=_stmttmp14;struct Cyc_Absyn_Exp*_tmp158;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp157)->tag == 15U){_LL1A: _tmp158=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp157)->f1;_LL1B: {struct Cyc_Absyn_Exp*e=_tmp158;
# 698
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp406;_tmp406.tag=0U,({struct _fat_ptr _tmp50D=({const char*_tmp15B="reference pattern not allowed on alias-free pointers";_tag_fat(_tmp15B,sizeof(char),53U);});_tmp406.f1=_tmp50D;});_tmp406;});void*_tmp159[1U];_tmp159[0]=& _tmp15A;({unsigned _tmp50E=p->loc;Cyc_Warn_err2(_tmp50E,_tag_fat(_tmp159,sizeof(void*),1U));});});
goto _LL19;}}else{_LL1C: _LL1D:
# 702
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp407;_tmp407.tag=0U,({struct _fat_ptr _tmp50F=({const char*_tmp15E="check_pat_regions: bad reference access expression";_tag_fat(_tmp15E,sizeof(char),51U);});_tmp407.f1=_tmp50F;});_tmp407;});void*_tmp15C[1U];_tmp15C[0]=& _tmp15D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp15C,sizeof(void*),1U));});}_LL19:;}
# 704
break;}}}}
# 707
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 1U: _LLB: _tmp142=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp13F)->f1;_tmp143=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp13F)->f2;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp142;struct Cyc_Absyn_Pat*p2=_tmp143;
# 710
{void*_tmp15F=p->topt;void*_stmttmp15=_tmp15F;void*_tmp160=_stmttmp15;if(_tmp160 != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp160)->tag == 4U){_LL1F: _LL20:
# 712
 if(did_noalias_deref){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp162=({struct Cyc_Warn_String_Warn_Warg_struct _tmp408;_tmp408.tag=0U,({struct _fat_ptr _tmp510=({const char*_tmp163="pattern to array would create alias of no-alias pointer";_tag_fat(_tmp163,sizeof(char),56U);});_tmp408.f1=_tmp510;});_tmp408;});void*_tmp161[1U];_tmp161[0]=& _tmp162;({unsigned _tmp511=p->loc;Cyc_Warn_err2(_tmp511,_tag_fat(_tmp161,sizeof(void*),1U));});});
return;}
# 716
goto _LL1E;}else{goto _LL21;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 719
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 2U: _LLD: _tmp140=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp13F)->f1;_tmp141=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp13F)->f2;_LLE: {struct Cyc_Absyn_Tvar*tv=_tmp140;struct Cyc_Absyn_Vardecl*vd=_tmp141;
# 722
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp164=(struct _tuple16*)x->hd;struct _tuple16*_stmttmp16=_tmp164;struct _tuple16*_tmp165=_stmttmp16;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Vardecl**_tmp166;_LL24: _tmp166=_tmp165->f1;_tmp167=_tmp165->f2;_LL25: {struct Cyc_Absyn_Vardecl**vdopt=_tmp166;struct Cyc_Absyn_Exp*eopt=_tmp167;
# 726
if(vdopt != 0 &&*vdopt == vd){
if(eopt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp169=({struct Cyc_Warn_String_Warn_Warg_struct _tmp409;_tmp409.tag=0U,({struct _fat_ptr _tmp512=({const char*_tmp16A="cannot alias pattern expression in datatype";_tag_fat(_tmp16A,sizeof(char),44U);});_tmp409.f1=_tmp512;});_tmp409;});void*_tmp168[1U];_tmp168[0]=& _tmp169;({unsigned _tmp513=p->loc;Cyc_Warn_err2(_tmp513,_tag_fat(_tmp168,sizeof(void*),1U));});});else{
# 730
struct Cyc_Tcenv_Tenv*te2=({unsigned _tmp515=p->loc;struct Cyc_Tcenv_Tenv*_tmp514=te;Cyc_Tcenv_add_type_vars(_tmp515,_tmp514,({struct Cyc_Absyn_Tvar*_tmp16C[1U];_tmp16C[0]=tv;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp16C,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
te2=({struct Cyc_Tcenv_Tenv*_tmp516=te2;Cyc_Tcenv_add_region(_tmp516,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=2U,_tmp16B->f1=tv;_tmp16B;}),0);});
# 733
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(eopt->topt),tv,vd->type,eopt);}
# 736
break;}}}}
# 739
goto _LL0;}default: _LLF: _LL10:
 return;}_LL0:;}
# 755 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 757
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp16D=(struct _tuple16*)x->hd;struct _tuple16*_stmttmp17=_tmp16D;struct _tuple16*_tmp16E=_stmttmp17;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Vardecl**_tmp16F;_LL1: _tmp16F=_tmp16E->f1;_tmp170=_tmp16E->f2;_LL2: {struct Cyc_Absyn_Vardecl**vdopt=_tmp16F;struct Cyc_Absyn_Exp*eopt=_tmp170;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp171=eopt;struct Cyc_Absyn_Exp*e=_tmp171;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))&& !Cyc_Tcutil_is_noalias_path(e)){
# 764
if(vdopt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp173=({struct Cyc_Warn_String_Warn_Warg_struct _tmp40B;_tmp40B.tag=0U,({struct _fat_ptr _tmp517=({const char*_tmp176="pattern dereferences an alias-free-pointer ";_tag_fat(_tmp176,sizeof(char),44U);});_tmp40B.f1=_tmp517;});_tmp40B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp174=({struct Cyc_Warn_String_Warn_Warg_struct _tmp40A;_tmp40A.tag=0U,({
struct _fat_ptr _tmp518=({const char*_tmp175="from a non-unique path";_tag_fat(_tmp175,sizeof(char),23U);});_tmp40A.f1=_tmp518;});_tmp40A;});void*_tmp172[2U];_tmp172[0]=& _tmp173,_tmp172[1]=& _tmp174;({unsigned _tmp519=p->loc;Cyc_Warn_err2(_tmp519,_tag_fat(_tmp172,sizeof(void*),2U));});});else{
# 768
({struct Cyc_Warn_String_Warn_Warg_struct _tmp178=({struct Cyc_Warn_String_Warn_Warg_struct _tmp40E;_tmp40E.tag=0U,({struct _fat_ptr _tmp51A=({const char*_tmp17C="pattern for variable ";_tag_fat(_tmp17C,sizeof(char),22U);});_tmp40E.f1=_tmp51A;});_tmp40E;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp179=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp40D;_tmp40D.tag=1U,_tmp40D.f1=(*vdopt)->name;_tmp40D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp40C;_tmp40C.tag=0U,({
struct _fat_ptr _tmp51B=({const char*_tmp17B=" dereferences an alias-free-pointer from a non-unique path";_tag_fat(_tmp17B,sizeof(char),59U);});_tmp40C.f1=_tmp51B;});_tmp40C;});void*_tmp177[3U];_tmp177[0]=& _tmp178,_tmp177[1]=& _tmp179,_tmp177[2]=& _tmp17A;({unsigned _tmp51C=p->loc;Cyc_Warn_err2(_tmp51C,_tag_fat(_tmp177,sizeof(void*),3U));});});}}}}}}}
# 822 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1U};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2U};
# 832
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0U};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1U};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 846
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){return({union Cyc_Tcpat_Name_value _tmp40F;(_tmp40F.Name_v).tag=1U,(_tmp40F.Name_v).val=s;_tmp40F;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp410;(_tmp410.Int_v).tag=2U,(_tmp410.Int_v).val=i;_tmp410;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 863
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp17D=c1->name;union Cyc_Tcpat_Name_value _stmttmp18=_tmp17D;union Cyc_Tcpat_Name_value _tmp17E=_stmttmp18;int _tmp17F;struct _fat_ptr _tmp180;if((_tmp17E.Name_v).tag == 1){_LL1: _tmp180=(_tmp17E.Name_v).val;_LL2: {struct _fat_ptr n1=_tmp180;
# 866
union Cyc_Tcpat_Name_value _tmp181=c2->name;union Cyc_Tcpat_Name_value _stmttmp19=_tmp181;union Cyc_Tcpat_Name_value _tmp182=_stmttmp19;struct _fat_ptr _tmp183;if((_tmp182.Name_v).tag == 1){_LL6: _tmp183=(_tmp182.Name_v).val;_LL7: {struct _fat_ptr n2=_tmp183;
return Cyc_strcmp((struct _fat_ptr)n1,(struct _fat_ptr)n2);}}else{_LL8: _LL9:
 return - 1;}_LL5:;}}else{_LL3: _tmp17F=(_tmp17E.Int_v).val;_LL4: {int i1=_tmp17F;
# 871
union Cyc_Tcpat_Name_value _tmp184=c2->name;union Cyc_Tcpat_Name_value _stmttmp1A=_tmp184;union Cyc_Tcpat_Name_value _tmp185=_stmttmp1A;int _tmp186;if((_tmp185.Name_v).tag == 1){_LLB: _LLC:
 return 1;}else{_LLD: _tmp186=(_tmp185.Int_v).val;_LLE: {int i2=_tmp186;
return i1 - i2;}}_LLA:;}}_LL0:;}
# 879
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 883
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 887
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned ans=0U;
struct Cyc_List_List*_tmp187=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp187;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 894
return ans;}
# 897
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp188=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp188;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp189=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*field=_tmp189;
if(Cyc_strcmp((struct _fat_ptr)*field->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 904
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp413;_tmp413.tag=0U,({struct _fat_ptr _tmp51D=({const char*_tmp18F="get_member_offset ";_tag_fat(_tmp18F,sizeof(char),19U);});_tmp413.f1=_tmp51D;});_tmp413;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp412;_tmp412.tag=0U,_tmp412.f1=*f;_tmp412;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp411;_tmp411.tag=0U,({struct _fat_ptr _tmp51E=({const char*_tmp18E=" failed";_tag_fat(_tmp18E,sizeof(char),8U);});_tmp411.f1=_tmp51E;});_tmp411;});void*_tmp18A[3U];_tmp18A[0]=& _tmp18B,_tmp18A[1]=& _tmp18C,_tmp18A[2]=& _tmp18D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp18A,sizeof(void*),3U));});}
# 907
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp190=pw;struct Cyc_Absyn_Pat*_tmp191;struct Cyc_Absyn_Exp*_tmp192;if((_tmp190.where_clause).tag == 2){_LL1: _tmp192=(_tmp190.where_clause).val;_LL2: {struct Cyc_Absyn_Exp*e=_tmp192;
return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->tag=0U,_tmp193->f1=e;_tmp193;});}}else{_LL3: _tmp191=(_tmp190.pattern).val;_LL4: {struct Cyc_Absyn_Pat*p=_tmp191;
# 911
void*_tmp194=p->r;void*_stmttmp1B=_tmp194;void*_tmp195=_stmttmp1B;struct Cyc_List_List*_tmp197;union Cyc_Absyn_AggrInfo _tmp196;struct Cyc_Absyn_Datatypefield*_tmp199;struct Cyc_Absyn_Datatypedecl*_tmp198;struct Cyc_Absyn_Enumfield*_tmp19B;void*_tmp19A;struct Cyc_Absyn_Enumfield*_tmp19D;struct Cyc_Absyn_Enumdecl*_tmp19C;int _tmp19F;struct _fat_ptr _tmp19E;char _tmp1A0;int _tmp1A2;enum Cyc_Absyn_Sign _tmp1A1;struct Cyc_Absyn_Pat*_tmp1A3;struct Cyc_Absyn_Pat*_tmp1A4;switch(*((int*)_tmp195)){case 1U: _LL6: _tmp1A4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp195)->f2;_LL7: {struct Cyc_Absyn_Pat*p1=_tmp1A4;
_tmp1A3=p1;goto _LL9;}case 3U: _LL8: _tmp1A3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp195)->f2;_LL9: {struct Cyc_Absyn_Pat*p1=_tmp1A3;
return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp414;(_tmp414.pattern).tag=1U,(_tmp414.pattern).val=p1;_tmp414;}));}case 9U: _LLA: _LLB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10U: _LLC: _tmp1A1=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp195)->f1;_tmp1A2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp195)->f2;_LLD: {enum Cyc_Absyn_Sign s=_tmp1A1;int i=_tmp1A2;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->tag=6U,_tmp1A5->f1=(unsigned)i;_tmp1A5;});}case 11U: _LLE: _tmp1A0=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp195)->f1;_LLF: {char c=_tmp1A0;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->tag=6U,_tmp1A6->f1=(unsigned)c;_tmp1A6;});}case 12U: _LL10: _tmp19E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp195)->f1;_tmp19F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp195)->f2;_LL11: {struct _fat_ptr f=_tmp19E;int i=_tmp19F;
return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->tag=5U,_tmp1A7->f1=f,_tmp1A7->f2=i;_tmp1A7;});}case 13U: _LL12: _tmp19C=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp195)->f1;_tmp19D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp195)->f2;_LL13: {struct Cyc_Absyn_Enumdecl*ed=_tmp19C;struct Cyc_Absyn_Enumfield*ef=_tmp19D;
return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->tag=3U,_tmp1A8->f1=ed,_tmp1A8->f2=ef;_tmp1A8;});}case 14U: _LL14: _tmp19A=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp195)->f1;_tmp19B=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp195)->f2;_LL15: {void*t=_tmp19A;struct Cyc_Absyn_Enumfield*ef=_tmp19B;
return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->tag=4U,_tmp1A9->f1=t,_tmp1A9->f2=ef;_tmp1A9;});}case 6U: _LL16: _LL17:
# 921
{void*_tmp1AA=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_stmttmp1C=_tmp1AA;void*_tmp1AB=_stmttmp1C;void*_tmp1AC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AB)->tag == 3U){_LL1F: _tmp1AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AB)->f1).ptr_atts).nullable;_LL20: {void*n=_tmp1AC;
# 923
if(Cyc_Tcutil_force_type2bool(0,n))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LL1E;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 928
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp415;_tmp415.tag=0U,({struct _fat_ptr _tmp51F=({const char*_tmp1AF="non-null pointer type or non-pointer type in pointer pattern";_tag_fat(_tmp1AF,sizeof(char),61U);});_tmp415.f1=_tmp51F;});_tmp415;});void*_tmp1AD[1U];_tmp1AD[0]=& _tmp1AE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1AD,sizeof(void*),1U));});case 8U: _LL18: _tmp198=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp195)->f1;_tmp199=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp195)->f2;_LL19: {struct Cyc_Absyn_Datatypedecl*ddecl=_tmp198;struct Cyc_Absyn_Datatypefield*df=_tmp199;
# 930
if(ddecl->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->tag=9U,_tmp1B0->f1=ddecl,_tmp1B0->f2=df;_tmp1B0;});else{
# 933
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->tag=7U,({int _tmp520=(int)Cyc_Tcpat_datatype_tag_number(ddecl,df->name);_tmp1B1->f1=_tmp520;}),_tmp1B1->f2=ddecl,_tmp1B1->f3=df;_tmp1B1;});}}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp195)->f1 != 0){_LL1A: _tmp196=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp195)->f1;_tmp197=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp195)->f3;_LL1B: {union Cyc_Absyn_AggrInfo info=_tmp196;struct Cyc_List_List*dlps=_tmp197;
# 935
struct Cyc_Absyn_Aggrdecl*_tmp1B2=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1B2;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
struct _tuple19*_tmp1B3=(struct _tuple19*)((struct Cyc_List_List*)_check_null(dlps))->hd;struct _tuple19*_stmttmp1D=_tmp1B3;struct _tuple19*_tmp1B4=_stmttmp1D;struct Cyc_Absyn_Pat*_tmp1B6;struct Cyc_List_List*_tmp1B5;_LL24: _tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B4->f2;_LL25: {struct Cyc_List_List*designators=_tmp1B5;struct Cyc_Absyn_Pat*pat=_tmp1B6;
struct _fat_ptr*f;
{void*_tmp1B7=(void*)((struct Cyc_List_List*)_check_null(designators))->hd;void*_stmttmp1E=_tmp1B7;void*_tmp1B8=_stmttmp1E;struct _fat_ptr*_tmp1B9;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1B8)->tag == 1U){_LL27: _tmp1B9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1B8)->f1;_LL28: {struct _fat_ptr*fn=_tmp1B9;
f=fn;goto _LL26;}}else{_LL29: _LL2A:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp416;_tmp416.tag=0U,({struct _fat_ptr _tmp521=({const char*_tmp1BC="no field name in tagged union pattern";_tag_fat(_tmp1BC,sizeof(char),38U);});_tmp416.f1=_tmp521;});_tmp416;});void*_tmp1BA[1U];_tmp1BA[0]=& _tmp1BB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1BA,sizeof(void*),1U));});}_LL26:;}
# 943
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->tag=8U,_tmp1BD->f1=f,({int _tmp522=Cyc_Tcpat_get_member_offset(ad,f);_tmp1BD->f2=_tmp522;});_tmp1BD;});}}else{
# 945
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp417;_tmp417.tag=0U,({struct _fat_ptr _tmp523=({const char*_tmp1C0="non-tagged aggregate in pattern test";_tag_fat(_tmp1C0,sizeof(char),37U);});_tmp417.f1=_tmp523;});_tmp417;});void*_tmp1BE[1U];_tmp1BE[0]=& _tmp1BF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1BE,sizeof(void*),1U));});}}}else{goto _LL1C;}default: _LL1C: _LL1D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp418;_tmp418.tag=0U,({struct _fat_ptr _tmp524=({const char*_tmp1C3="non-test pattern in pattern test";_tag_fat(_tmp1C3,sizeof(char),33U);});_tmp418.f1=_tmp524;});_tmp418;});void*_tmp1C1[1U];_tmp1C1[0]=& _tmp1C2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1C1,sizeof(void*),1U));});}_LL5:;}}_LL0:;}
# 951
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp419;(_tmp419.pattern).tag=1U,(_tmp419.pattern).val=p;_tmp419;});}
# 955
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));({union Cyc_Tcpat_Name_value _tmp526=Cyc_Tcpat_Name_v(({const char*_tmp1C4="NULL";_tag_fat(_tmp1C4,sizeof(char),5U);}));_tmp1C5->name=_tmp526;}),_tmp1C5->arity=0,_tmp1C5->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp525=Cyc_Tcpat_pw(p);_tmp1C5->orig_pat=_tmp525;});_tmp1C5;});}
# 958
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({union Cyc_Tcpat_Name_value _tmp528=Cyc_Tcpat_Name_v(({const char*_tmp1C6="&";_tag_fat(_tmp1C6,sizeof(char),2U);}));_tmp1C7->name=_tmp528;}),_tmp1C7->arity=1,_tmp1C7->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp527=Cyc_Tcpat_pw(p);_tmp1C7->orig_pat=_tmp527;});_tmp1C7;});}
# 961
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));({union Cyc_Tcpat_Name_value _tmp52A=Cyc_Tcpat_Name_v(({const char*_tmp1C8="&";_tag_fat(_tmp1C8,sizeof(char),2U);}));_tmp1C9->name=_tmp52A;}),_tmp1C9->arity=1,_tmp1C9->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _tmp529=Cyc_Tcpat_pw(p);_tmp1C9->orig_pat=_tmp529;});_tmp1C9;});}
# 964
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));({union Cyc_Tcpat_Name_value _tmp52B=Cyc_Tcpat_Int_v(i);_tmp1CA->name=_tmp52B;}),_tmp1CA->arity=0,_tmp1CA->span=0,_tmp1CA->orig_pat=p;_tmp1CA;});}
# 967
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));({union Cyc_Tcpat_Name_value _tmp52D=Cyc_Tcpat_Name_v(f);_tmp1CB->name=_tmp52D;}),_tmp1CB->arity=0,_tmp1CB->span=0,({union Cyc_Tcpat_PatOrWhere _tmp52C=Cyc_Tcpat_pw(p);_tmp1CB->orig_pat=_tmp52C;});_tmp1CB;});}
# 970
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));({union Cyc_Tcpat_Name_value _tmp52F=Cyc_Tcpat_Int_v((int)c);_tmp1CC->name=_tmp52F;}),_tmp1CC->arity=0,_tmp1CC->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _tmp52E=Cyc_Tcpat_pw(p);_tmp1CC->orig_pat=_tmp52E;});_tmp1CC;});}
# 973
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));({union Cyc_Tcpat_Name_value _tmp530=Cyc_Tcpat_Name_v(({const char*_tmp1CD="$";_tag_fat(_tmp1CD,sizeof(char),2U);}));_tmp1CE->name=_tmp530;}),_tmp1CE->arity=i,_tmp1CE->span=& Cyc_Tcpat_one_opt,_tmp1CE->orig_pat=p;_tmp1CE;});}
# 978
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp531=Cyc_Tcpat_null_con(p);_tmp1CF->f1=_tmp531;}),_tmp1CF->f2=0;_tmp1CF;});}
# 981
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp532=Cyc_Tcpat_int_con(i,p);_tmp1D0->f1=_tmp532;}),_tmp1D0->f2=0;_tmp1D0;});}
# 984
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp533=Cyc_Tcpat_char_con(c,p);_tmp1D1->f1=_tmp533;}),_tmp1D1->f2=0;_tmp1D1;});}
# 987
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp534=Cyc_Tcpat_float_con(f,p);_tmp1D2->f1=_tmp534;}),_tmp1D2->f2=0;_tmp1D2;});}
# 990
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp536=Cyc_Tcpat_null_ptr_con(p0);_tmp1D4->f1=_tmp536;}),({struct Cyc_List_List*_tmp535=({struct Cyc_List_List*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->hd=p,_tmp1D3->tl=0;_tmp1D3;});_tmp1D4->f2=_tmp535;});_tmp1D4;});}
# 993
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp538=Cyc_Tcpat_ptr_con(p0);_tmp1D6->f1=_tmp538;}),({struct Cyc_List_List*_tmp537=({struct Cyc_List_List*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->hd=p,_tmp1D5->tl=0;_tmp1D5;});_tmp1D6->f2=_tmp537;});_tmp1D6;});}
# 996
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp53A=({int _tmp539=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp539,p);});_tmp1D7->f1=_tmp53A;}),_tmp1D7->f2=ss;_tmp1D7;});}
# 999
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1001
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));({union Cyc_Tcpat_Name_value _tmp53D=Cyc_Tcpat_Name_v(con_name);_tmp1D9->name=_tmp53D;}),({int _tmp53C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp1D9->arity=_tmp53C;}),_tmp1D9->span=span,({union Cyc_Tcpat_PatOrWhere _tmp53B=Cyc_Tcpat_pw(p);_tmp1D9->orig_pat=_tmp53B;});_tmp1D9;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->tag=1U,_tmp1D8->f1=c,_tmp1D8->f2=ps;_tmp1D8;});}
# 1006
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1DA=p->r;void*_stmttmp1F=_tmp1DA;void*_tmp1DB=_stmttmp1F;struct Cyc_Absyn_Enumfield*_tmp1DD;void*_tmp1DC;struct Cyc_Absyn_Enumfield*_tmp1DF;struct Cyc_Absyn_Enumdecl*_tmp1DE;struct Cyc_List_List*_tmp1E1;struct Cyc_Absyn_Aggrdecl*_tmp1E0;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List*_tmp1E5;struct Cyc_Absyn_Datatypefield*_tmp1E4;struct Cyc_Absyn_Datatypedecl*_tmp1E3;struct Cyc_Absyn_Pat*_tmp1E6;struct Cyc_Absyn_Pat*_tmp1E7;struct Cyc_Absyn_Pat*_tmp1E8;struct _fat_ptr _tmp1E9;char _tmp1EA;int _tmp1EC;enum Cyc_Absyn_Sign _tmp1EB;switch(*((int*)_tmp1DB)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->tag=0U;_tmp1ED;});goto _LL0;case 9U: _LL7: _LL8:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 10U: _LL9: _tmp1EB=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1;_tmp1EC=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DB)->f2;_LLA: {enum Cyc_Absyn_Sign sn=_tmp1EB;int i=_tmp1EC;
s=({int _tmp53E=i;Cyc_Tcpat_int_pat(_tmp53E,Cyc_Tcpat_pw(p));});goto _LL0;}case 11U: _LLB: _tmp1EA=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1;_LLC: {char c=_tmp1EA;
s=Cyc_Tcpat_char_pat(c,p);goto _LL0;}case 12U: _LLD: _tmp1E9=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1;_LLE: {struct _fat_ptr f=_tmp1E9;
s=Cyc_Tcpat_float_pat(f,p);goto _LL0;}case 1U: _LLF: _tmp1E8=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DB)->f2;_LL10: {struct Cyc_Absyn_Pat*p2=_tmp1E8;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 3U: _LL11: _tmp1E7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DB)->f2;_LL12: {struct Cyc_Absyn_Pat*p2=_tmp1E7;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 6U: _LL13: _tmp1E6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1;_LL14: {struct Cyc_Absyn_Pat*pp=_tmp1E6;
# 1019
{void*_tmp1EE=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_stmttmp20=_tmp1EE;void*_tmp1EF=_stmttmp20;void*_tmp1F0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EF)->tag == 3U){_LL28: _tmp1F0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EF)->f1).ptr_atts).nullable;_LL29: {void*n=_tmp1F0;
# 1021
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
void*ss=Cyc_Tcpat_compile_pat(pp);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(ss,p);else{
# 1026
s=Cyc_Tcpat_ptr_pat(ss,p);}
goto _LL27;}}else{_LL2A: _LL2B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp41A;_tmp41A.tag=0U,({struct _fat_ptr _tmp53F=({const char*_tmp1F3="expecting pointertype for pattern!";_tag_fat(_tmp1F3,sizeof(char),35U);});_tmp41A.f1=_tmp53F;});_tmp41A;});void*_tmp1F1[1U];_tmp1F1[0]=& _tmp1F2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1F1,sizeof(void*),1U));});}_LL27:;}
# 1030
goto _LL0;}case 8U: _LL15: _tmp1E3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1;_tmp1E4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DB)->f2;_tmp1E5=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DB)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1E3;struct Cyc_Absyn_Datatypefield*tuf=_tmp1E4;struct Cyc_List_List*ps=_tmp1E5;
# 1032
int*span;
{void*_tmp1F4=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_stmttmp21=_tmp1F4;void*_tmp1F5=_stmttmp21;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f1)){case 18U: _LL2D: _LL2E:
# 1035
 if(tud->is_extensible)
span=0;else{
# 1038
span=({int*_tmp1F6=_cycalloc_atomic(sizeof(*_tmp1F6));({int _tmp540=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v);*_tmp1F6=_tmp540;});_tmp1F6;});}
goto _LL2C;case 19U: _LL2F: _LL30:
 span=& Cyc_Tcpat_one_opt;goto _LL2C;default: goto _LL31;}else{_LL31: _LL32:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp41B;_tmp41B.tag=0U,({struct _fat_ptr _tmp541=({const char*_tmp1F9="void datatype pattern has bad type";_tag_fat(_tmp1F9,sizeof(char),35U);});_tmp41B.f1=_tmp541;});_tmp41B;});void*_tmp1F7[1U];_tmp1F7[0]=& _tmp1F8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1F7,sizeof(void*),1U));});}_LL2C:;}
# 1043
s=({struct _fat_ptr _tmp544=*(*tuf->name).f2;int*_tmp543=span;struct Cyc_List_List*_tmp542=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_con_pat(_tmp544,_tmp543,_tmp542,p);});
goto _LL0;}case 5U: _LL17: _tmp1E2=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1;_LL18: {struct Cyc_List_List*ps=_tmp1E2;
# 1047
s=({struct Cyc_List_List*_tmp545=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_tuple_pat(_tmp545,Cyc_Tcpat_pw(p));});goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1)->KnownAggr).tag == 2){_LL19: _tmp1E0=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1)->KnownAggr).val;_tmp1E1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DB)->f3;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1E0;struct Cyc_List_List*dlps=_tmp1E1;
# 1052
if((int)ad->kind == (int)Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1056
int found=({struct _fat_ptr _tmp546=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp546,({const char*_tmp207="";_tag_fat(_tmp207,sizeof(char),1U);}));})== 0;
{struct Cyc_List_List*dlps0=dlps;for(0;!found && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple19*_tmp1FA=(struct _tuple19*)dlps0->hd;struct _tuple19*_stmttmp22=_tmp1FA;struct _tuple19*_tmp1FB=_stmttmp22;struct Cyc_Absyn_Pat*_tmp1FD;struct Cyc_List_List*_tmp1FC;_LL34: _tmp1FC=_tmp1FB->f1;_tmp1FD=_tmp1FB->f2;_LL35: {struct Cyc_List_List*dl=_tmp1FC;struct Cyc_Absyn_Pat*p=_tmp1FD;
struct Cyc_List_List*_tmp1FE=dl;struct _fat_ptr*_tmp1FF;if(_tmp1FE != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp1FE)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp1FE)->tl == 0){_LL37: _tmp1FF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1FE->hd)->f1;_LL38: {struct _fat_ptr*f=_tmp1FF;
# 1061
if(Cyc_strptrcmp(f,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp200=_cycalloc(sizeof(*_tmp200));({void*_tmp547=Cyc_Tcpat_compile_pat(p);_tmp200->hd=_tmp547;}),_tmp200->tl=ps;_tmp200;});
found=1;}
# 1065
goto _LL36;}}else{goto _LL39;}}else{goto _LL39;}}else{_LL39: _LL3A:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp202=({struct Cyc_Warn_String_Warn_Warg_struct _tmp41C;_tmp41C.tag=0U,({struct _fat_ptr _tmp548=({const char*_tmp203="bad designator(s)";_tag_fat(_tmp203,sizeof(char),18U);});_tmp41C.f1=_tmp548;});_tmp41C;});void*_tmp201[1U];_tmp201[0]=& _tmp202;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp201,sizeof(void*),1U));});}_LL36:;}}}
# 1069
if(!found)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp205=({struct Cyc_Warn_String_Warn_Warg_struct _tmp41D;_tmp41D.tag=0U,({struct _fat_ptr _tmp549=({const char*_tmp206="bad designator";_tag_fat(_tmp206,sizeof(char),15U);});_tmp41D.f1=_tmp549;});_tmp41D;});void*_tmp204[1U];_tmp204[0]=& _tmp205;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp204,sizeof(void*),1U));});}}
# 1072
s=({struct Cyc_List_List*_tmp54A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp54A,Cyc_Tcpat_pw(p));});}else{
# 1075
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp209=({struct Cyc_Warn_String_Warn_Warg_struct _tmp41E;_tmp41E.tag=0U,({struct _fat_ptr _tmp54B=({const char*_tmp20A="patterns on untagged unions not yet supported.";_tag_fat(_tmp20A,sizeof(char),47U);});_tmp41E.f1=_tmp54B;});_tmp41E;});void*_tmp208[1U];_tmp208[0]=& _tmp209;({unsigned _tmp54C=p->loc;Cyc_Warn_err2(_tmp54C,_tag_fat(_tmp208,sizeof(void*),1U));});});{
int*span=({int*_tmp212=_cycalloc_atomic(sizeof(*_tmp212));({int _tmp54D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);*_tmp212=_tmp54D;});_tmp212;});
struct Cyc_List_List*_tmp20B=dlps;struct Cyc_Absyn_Pat*_tmp20D;struct _fat_ptr*_tmp20C;if(_tmp20B != 0){if(((struct _tuple19*)((struct Cyc_List_List*)_tmp20B)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple19*)((struct Cyc_List_List*)_tmp20B)->hd)->f1)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct _tuple19*)((struct Cyc_List_List*)_tmp20B)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp20B)->tl == 0){_LL3C: _tmp20C=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple19*)_tmp20B->hd)->f1)->hd)->f1;_tmp20D=((struct _tuple19*)_tmp20B->hd)->f2;_LL3D: {struct _fat_ptr*f=_tmp20C;struct Cyc_Absyn_Pat*p2=_tmp20D;
# 1080
s=({struct _fat_ptr _tmp551=*f;int*_tmp550=span;struct Cyc_List_List*_tmp54F=({struct Cyc_List_List*_tmp20E=_cycalloc(sizeof(*_tmp20E));({void*_tmp54E=Cyc_Tcpat_compile_pat(p2);_tmp20E->hd=_tmp54E;}),_tmp20E->tl=0;_tmp20E;});Cyc_Tcpat_con_pat(_tmp551,_tmp550,_tmp54F,p);});
goto _LL3B;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{_LL3E: _LL3F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp210=({struct Cyc_Warn_String_Warn_Warg_struct _tmp41F;_tmp41F.tag=0U,({struct _fat_ptr _tmp552=({const char*_tmp211="bad union pattern";_tag_fat(_tmp211,sizeof(char),18U);});_tmp41F.f1=_tmp552;});_tmp41F;});void*_tmp20F[1U];_tmp20F[0]=& _tmp210;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp20F,sizeof(void*),1U));});}_LL3B:;}}
# 1085
goto _LL0;}}else{goto _LL23;}}else{_LL23: _LL24:
# 1113
 goto _LL26;}case 13U: _LL1B: _tmp1DE=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1;_tmp1DF=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1DB)->f2;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp1DE;struct Cyc_Absyn_Enumfield*ef=_tmp1DF;
# 1093
s=Cyc_Tcpat_con_pat(*(*ef->name).f2,0,0,p);
goto _LL0;}case 14U: _LL1D: _tmp1DC=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1DB)->f1;_tmp1DD=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1DB)->f2;_LL1E: {void*tenum=_tmp1DC;struct Cyc_Absyn_Enumfield*ef=_tmp1DD;
# 1099
struct Cyc_List_List*fields;
{void*_tmp213=Cyc_Tcutil_compress(tenum);void*_stmttmp23=_tmp213;void*_tmp214=_stmttmp23;struct Cyc_List_List*_tmp215;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp214)->tag == 0U){if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp214)->f1)->tag == 16U){_LL41: _tmp215=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp214)->f1)->f1;_LL42: {struct Cyc_List_List*fs=_tmp215;
fields=fs;goto _LL40;}}else{goto _LL43;}}else{_LL43: _LL44:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp217=({struct Cyc_Warn_String_Warn_Warg_struct _tmp420;_tmp420.tag=0U,({struct _fat_ptr _tmp553=({const char*_tmp218="bad type in AnonEnum_p";_tag_fat(_tmp218,sizeof(char),23U);});_tmp420.f1=_tmp553;});_tmp420;});void*_tmp216[1U];_tmp216[0]=& _tmp217;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp216,sizeof(void*),1U));});}_LL40:;}
# 1109
s=Cyc_Tcpat_con_pat(*(*ef->name).f2,0,0,p);
goto _LL0;}case 15U: _LL1F: _LL20:
 goto _LL22;case 16U: _LL21: _LL22:
 goto _LL24;default: _LL25: _LL26:
# 1114
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219->tag=0U;_tmp219;});}_LL0:;}
# 1116
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1145
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp21A=({struct _tuple0 _tmp421;_tmp421.f1=a1,_tmp421.f2=a2;_tmp421;});struct _tuple0 _stmttmp24=_tmp21A;struct _tuple0 _tmp21B=_stmttmp24;struct _fat_ptr*_tmp21F;int _tmp21E;struct _fat_ptr*_tmp21D;int _tmp21C;unsigned _tmp223;struct Cyc_Absyn_Datatypefield*_tmp222;unsigned _tmp221;struct Cyc_Absyn_Datatypefield*_tmp220;unsigned _tmp225;unsigned _tmp224;switch(*((int*)_tmp21B.f1)){case 0U: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp21B.f2)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LLB;}case 1U: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp21B.f2)->tag == 1U){_LL3: _LL4:
 return 1;}else{goto _LLB;}case 2U: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp21B.f2)->tag == 2U){_LL5: _tmp224=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp21B.f1)->f1;_tmp225=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp21B.f2)->f1;_LL6: {unsigned i1=_tmp224;unsigned i2=_tmp225;
return i1 == i2;}}else{goto _LLB;}case 3U: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp21B.f2)->tag == 3U){_LL7: _tmp220=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp21B.f1)->f2;_tmp221=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp21B.f1)->f3;_tmp222=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp21B.f2)->f2;_tmp223=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp21B.f2)->f3;_LL8: {struct Cyc_Absyn_Datatypefield*df1=_tmp220;unsigned i1=_tmp221;struct Cyc_Absyn_Datatypefield*df2=_tmp222;unsigned i2=_tmp223;
# 1151
return df1 == df2 && i1 == i2;}}else{goto _LLB;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp21B.f2)->tag == 4U){_LL9: _tmp21C=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp21B.f1)->f1;_tmp21D=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp21B.f1)->f2;_tmp21E=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp21B.f2)->f1;_tmp21F=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp21B.f2)->f2;_LLA: {int b1=_tmp21C;struct _fat_ptr*f1=_tmp21D;int b2=_tmp21E;struct _fat_ptr*f2=_tmp21F;
# 1153
return b1 == b2 && Cyc_strptrcmp(f1,f2)== 0;}}else{_LLB: _LLC:
 return 0;}}_LL0:;}
# 1158
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0 && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1164
if(p1 != p2)return 0;
return 1;}
# 1168
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp226=Cyc_Tcpat_get_pat_test(con->orig_pat);void*test=_tmp226;
{void*_tmp227=d2;void*_tmp22A;struct Cyc_List_List*_tmp229;struct Cyc_List_List*_tmp228;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp227)->tag == 2U){_LL1: _tmp228=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp227)->f2;_tmp22A=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp227)->f3;_LL2: {struct Cyc_List_List*access2=_tmp228;struct Cyc_List_List*switch_clauses=_tmp229;void*default_decision=_tmp22A;
# 1172
if(Cyc_Tcpat_same_path(access,access2)&&(int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->tag=2U,_tmp22D->f1=access2,({
struct Cyc_List_List*_tmp555=({struct Cyc_List_List*_tmp22C=_cycalloc(sizeof(*_tmp22C));({struct _tuple0*_tmp554=({struct _tuple0*_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B->f1=test,_tmp22B->f2=d1;_tmp22B;});_tmp22C->hd=_tmp554;}),_tmp22C->tl=switch_clauses;_tmp22C;});_tmp22D->f2=_tmp555;}),_tmp22D->f3=default_decision;_tmp22D;});else{
# 1176
goto _LL0;}}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1179
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230->tag=2U,_tmp230->f1=access,({struct Cyc_List_List*_tmp557=({struct Cyc_List_List*_tmp22F=_cycalloc(sizeof(*_tmp22F));({struct _tuple0*_tmp556=({struct _tuple0*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->f1=test,_tmp22E->f2=d1;_tmp22E;});_tmp22F->hd=_tmp556;}),_tmp22F->tl=0;_tmp22F;});_tmp230->f2=_tmp557;}),_tmp230->f3=d2;_tmp230;});}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1190
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1193
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp231=0U;({struct Cyc___cycFILE*_tmp559=Cyc_stderr;struct _fat_ptr _tmp558=({const char*_tmp232=" ";_tag_fat(_tmp232,sizeof(char),2U);});Cyc_fprintf(_tmp559,_tmp558,_tag_fat(_tmp231,sizeof(void*),0U));});});}}
# 1199
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp233=c->name;union Cyc_Tcpat_Name_value n=_tmp233;
union Cyc_Tcpat_Name_value _tmp234=n;int _tmp235;struct _fat_ptr _tmp236;if((_tmp234.Name_v).tag == 1){_LL1: _tmp236=(_tmp234.Name_v).val;_LL2: {struct _fat_ptr s=_tmp236;
({struct Cyc_String_pa_PrintArg_struct _tmp239=({struct Cyc_String_pa_PrintArg_struct _tmp422;_tmp422.tag=0U,_tmp422.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp422;});void*_tmp237[1U];_tmp237[0]=& _tmp239;({struct Cyc___cycFILE*_tmp55B=Cyc_stderr;struct _fat_ptr _tmp55A=({const char*_tmp238="%s";_tag_fat(_tmp238,sizeof(char),3U);});Cyc_fprintf(_tmp55B,_tmp55A,_tag_fat(_tmp237,sizeof(void*),1U));});});goto _LL0;}}else{_LL3: _tmp235=(_tmp234.Int_v).val;_LL4: {int i=_tmp235;
({struct Cyc_Int_pa_PrintArg_struct _tmp23C=({struct Cyc_Int_pa_PrintArg_struct _tmp423;_tmp423.tag=1U,_tmp423.f1=(unsigned long)i;_tmp423;});void*_tmp23A[1U];_tmp23A[0]=& _tmp23C;({struct Cyc___cycFILE*_tmp55D=Cyc_stderr;struct _fat_ptr _tmp55C=({const char*_tmp23B="%d";_tag_fat(_tmp23B,sizeof(char),3U);});Cyc_fprintf(_tmp55D,_tmp55C,_tag_fat(_tmp23A,sizeof(void*),1U));});});goto _LL0;}}_LL0:;}
# 1207
static void Cyc_Tcpat_print_access(void*a){
void*_tmp23D=a;struct _fat_ptr*_tmp23F;int _tmp23E;unsigned _tmp241;struct Cyc_Absyn_Datatypefield*_tmp240;unsigned _tmp242;switch(*((int*)_tmp23D)){case 0U: _LL1: _LL2:
({void*_tmp243=0U;({struct Cyc___cycFILE*_tmp55F=Cyc_stderr;struct _fat_ptr _tmp55E=({const char*_tmp244="DUMMY";_tag_fat(_tmp244,sizeof(char),6U);});Cyc_fprintf(_tmp55F,_tmp55E,_tag_fat(_tmp243,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp245=0U;({struct Cyc___cycFILE*_tmp561=Cyc_stderr;struct _fat_ptr _tmp560=({const char*_tmp246="*";_tag_fat(_tmp246,sizeof(char),2U);});Cyc_fprintf(_tmp561,_tmp560,_tag_fat(_tmp245,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp242=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp23D)->f1;_LL6: {unsigned i=_tmp242;
({struct Cyc_Int_pa_PrintArg_struct _tmp249=({struct Cyc_Int_pa_PrintArg_struct _tmp424;_tmp424.tag=1U,_tmp424.f1=(unsigned long)((int)i);_tmp424;});void*_tmp247[1U];_tmp247[0]=& _tmp249;({struct Cyc___cycFILE*_tmp563=Cyc_stderr;struct _fat_ptr _tmp562=({const char*_tmp248="[%d]";_tag_fat(_tmp248,sizeof(char),5U);});Cyc_fprintf(_tmp563,_tmp562,_tag_fat(_tmp247,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL7: _tmp240=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp23D)->f2;_tmp241=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp23D)->f3;_LL8: {struct Cyc_Absyn_Datatypefield*f=_tmp240;unsigned i=_tmp241;
# 1213
({struct Cyc_String_pa_PrintArg_struct _tmp24C=({struct Cyc_String_pa_PrintArg_struct _tmp426;_tmp426.tag=0U,({struct _fat_ptr _tmp564=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp426.f1=_tmp564;});_tmp426;});struct Cyc_Int_pa_PrintArg_struct _tmp24D=({struct Cyc_Int_pa_PrintArg_struct _tmp425;_tmp425.tag=1U,_tmp425.f1=(unsigned long)((int)i);_tmp425;});void*_tmp24A[2U];_tmp24A[0]=& _tmp24C,_tmp24A[1]=& _tmp24D;({struct Cyc___cycFILE*_tmp566=Cyc_stderr;struct _fat_ptr _tmp565=({const char*_tmp24B="%s[%d]";_tag_fat(_tmp24B,sizeof(char),7U);});Cyc_fprintf(_tmp566,_tmp565,_tag_fat(_tmp24A,sizeof(void*),2U));});});goto _LL0;}default: _LL9: _tmp23E=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp23D)->f1;_tmp23F=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp23D)->f2;_LLA: {int tagged=_tmp23E;struct _fat_ptr*f=_tmp23F;
# 1215
if(tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp250=({struct Cyc_String_pa_PrintArg_struct _tmp427;_tmp427.tag=0U,_tmp427.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp427;});void*_tmp24E[1U];_tmp24E[0]=& _tmp250;({struct Cyc___cycFILE*_tmp568=Cyc_stderr;struct _fat_ptr _tmp567=({const char*_tmp24F=".tagunion.%s";_tag_fat(_tmp24F,sizeof(char),13U);});Cyc_fprintf(_tmp568,_tmp567,_tag_fat(_tmp24E,sizeof(void*),1U));});});else{
# 1218
({struct Cyc_String_pa_PrintArg_struct _tmp253=({struct Cyc_String_pa_PrintArg_struct _tmp428;_tmp428.tag=0U,_tmp428.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp428;});void*_tmp251[1U];_tmp251[0]=& _tmp253;({struct Cyc___cycFILE*_tmp56A=Cyc_stderr;struct _fat_ptr _tmp569=({const char*_tmp252=".%s";_tag_fat(_tmp252,sizeof(char),4U);});Cyc_fprintf(_tmp56A,_tmp569,_tag_fat(_tmp251,sizeof(void*),1U));});});}
goto _LL0;}}_LL0:;}
# 1223
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp254=p;struct Cyc_Absyn_Datatypefield*_tmp255;int _tmp257;struct _fat_ptr*_tmp256;int _tmp258;unsigned _tmp259;struct _fat_ptr _tmp25A;struct Cyc_Absyn_Enumfield*_tmp25B;struct Cyc_Absyn_Enumfield*_tmp25C;struct Cyc_Absyn_Exp*_tmp25D;switch(*((int*)_tmp254)){case 0U: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp254)->f1 == 0){_LL1: _LL2:
({void*_tmp25E=0U;({struct Cyc___cycFILE*_tmp56C=Cyc_stderr;struct _fat_ptr _tmp56B=({const char*_tmp25F="where(NULL)";_tag_fat(_tmp25F,sizeof(char),12U);});Cyc_fprintf(_tmp56C,_tmp56B,_tag_fat(_tmp25E,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp25D=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp254)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp25D;
({struct Cyc_String_pa_PrintArg_struct _tmp262=({struct Cyc_String_pa_PrintArg_struct _tmp429;_tmp429.tag=0U,({struct _fat_ptr _tmp56D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(e)));_tmp429.f1=_tmp56D;});_tmp429;});void*_tmp260[1U];_tmp260[0]=& _tmp262;({struct Cyc___cycFILE*_tmp56F=Cyc_stderr;struct _fat_ptr _tmp56E=({const char*_tmp261="where(%s)";_tag_fat(_tmp261,sizeof(char),10U);});Cyc_fprintf(_tmp56F,_tmp56E,_tag_fat(_tmp260,sizeof(void*),1U));});});goto _LL0;}}case 1U: _LL5: _LL6:
({void*_tmp263=0U;({struct Cyc___cycFILE*_tmp571=Cyc_stderr;struct _fat_ptr _tmp570=({const char*_tmp264="NULL";_tag_fat(_tmp264,sizeof(char),5U);});Cyc_fprintf(_tmp571,_tmp570,_tag_fat(_tmp263,sizeof(void*),0U));});});goto _LL0;case 2U: _LL7: _LL8:
({void*_tmp265=0U;({struct Cyc___cycFILE*_tmp573=Cyc_stderr;struct _fat_ptr _tmp572=({const char*_tmp266="NOT-NULL:";_tag_fat(_tmp266,sizeof(char),10U);});Cyc_fprintf(_tmp573,_tmp572,_tag_fat(_tmp265,sizeof(void*),0U));});});goto _LL0;case 4U: _LL9: _tmp25C=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp254)->f2;_LLA: {struct Cyc_Absyn_Enumfield*ef=_tmp25C;
_tmp25B=ef;goto _LLC;}case 3U: _LLB: _tmp25B=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp254)->f2;_LLC: {struct Cyc_Absyn_Enumfield*ef=_tmp25B;
({struct Cyc_String_pa_PrintArg_struct _tmp269=({struct Cyc_String_pa_PrintArg_struct _tmp42A;_tmp42A.tag=0U,({struct _fat_ptr _tmp574=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ef->name));_tmp42A.f1=_tmp574;});_tmp42A;});void*_tmp267[1U];_tmp267[0]=& _tmp269;({struct Cyc___cycFILE*_tmp576=Cyc_stderr;struct _fat_ptr _tmp575=({const char*_tmp268="%s";_tag_fat(_tmp268,sizeof(char),3U);});Cyc_fprintf(_tmp576,_tmp575,_tag_fat(_tmp267,sizeof(void*),1U));});});goto _LL0;}case 5U: _LLD: _tmp25A=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp254)->f1;_LLE: {struct _fat_ptr s=_tmp25A;
({struct Cyc_String_pa_PrintArg_struct _tmp26C=({struct Cyc_String_pa_PrintArg_struct _tmp42B;_tmp42B.tag=0U,_tmp42B.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp42B;});void*_tmp26A[1U];_tmp26A[0]=& _tmp26C;({struct Cyc___cycFILE*_tmp578=Cyc_stderr;struct _fat_ptr _tmp577=({const char*_tmp26B="%s";_tag_fat(_tmp26B,sizeof(char),3U);});Cyc_fprintf(_tmp578,_tmp577,_tag_fat(_tmp26A,sizeof(void*),1U));});});goto _LL0;}case 6U: _LLF: _tmp259=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp254)->f1;_LL10: {unsigned i=_tmp259;
({struct Cyc_Int_pa_PrintArg_struct _tmp26F=({struct Cyc_Int_pa_PrintArg_struct _tmp42C;_tmp42C.tag=1U,_tmp42C.f1=(unsigned long)((int)i);_tmp42C;});void*_tmp26D[1U];_tmp26D[0]=& _tmp26F;({struct Cyc___cycFILE*_tmp57A=Cyc_stderr;struct _fat_ptr _tmp579=({const char*_tmp26E="%d";_tag_fat(_tmp26E,sizeof(char),3U);});Cyc_fprintf(_tmp57A,_tmp579,_tag_fat(_tmp26D,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL11: _tmp258=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp254)->f1;_LL12: {int i=_tmp258;
({struct Cyc_Int_pa_PrintArg_struct _tmp272=({struct Cyc_Int_pa_PrintArg_struct _tmp42D;_tmp42D.tag=1U,_tmp42D.f1=(unsigned long)i;_tmp42D;});void*_tmp270[1U];_tmp270[0]=& _tmp272;({struct Cyc___cycFILE*_tmp57C=Cyc_stderr;struct _fat_ptr _tmp57B=({const char*_tmp271="datatypetag(%d)";_tag_fat(_tmp271,sizeof(char),16U);});Cyc_fprintf(_tmp57C,_tmp57B,_tag_fat(_tmp270,sizeof(void*),1U));});});goto _LL0;}case 8U: _LL13: _tmp256=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp254)->f1;_tmp257=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp254)->f2;_LL14: {struct _fat_ptr*f=_tmp256;int i=_tmp257;
({struct Cyc_String_pa_PrintArg_struct _tmp275=({struct Cyc_String_pa_PrintArg_struct _tmp42F;_tmp42F.tag=0U,_tmp42F.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp42F;});struct Cyc_Int_pa_PrintArg_struct _tmp276=({struct Cyc_Int_pa_PrintArg_struct _tmp42E;_tmp42E.tag=1U,_tmp42E.f1=(unsigned long)i;_tmp42E;});void*_tmp273[2U];_tmp273[0]=& _tmp275,_tmp273[1]=& _tmp276;({struct Cyc___cycFILE*_tmp57E=Cyc_stderr;struct _fat_ptr _tmp57D=({const char*_tmp274="uniontag[%s](%d)";_tag_fat(_tmp274,sizeof(char),17U);});Cyc_fprintf(_tmp57E,_tmp57D,_tag_fat(_tmp273,sizeof(void*),2U));});});goto _LL0;}default: _LL15: _tmp255=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp254)->f2;_LL16: {struct Cyc_Absyn_Datatypefield*f=_tmp255;
# 1236
({struct Cyc_String_pa_PrintArg_struct _tmp279=({struct Cyc_String_pa_PrintArg_struct _tmp430;_tmp430.tag=0U,({struct _fat_ptr _tmp57F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp430.f1=_tmp57F;});_tmp430;});void*_tmp277[1U];_tmp277[0]=& _tmp279;({struct Cyc___cycFILE*_tmp581=Cyc_stderr;struct _fat_ptr _tmp580=({const char*_tmp278="datatypefield(%s)";_tag_fat(_tmp278,sizeof(char),18U);});Cyc_fprintf(_tmp581,_tmp580,_tag_fat(_tmp277,sizeof(void*),1U));});});}}_LL0:;}
# 1240
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp27C=({struct Cyc_String_pa_PrintArg_struct _tmp431;_tmp431.tag=0U,({struct _fat_ptr _tmp582=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp431.f1=_tmp582;});_tmp431;});void*_tmp27A[1U];_tmp27A[0]=& _tmp27C;({struct Cyc___cycFILE*_tmp584=Cyc_stderr;struct _fat_ptr _tmp583=({const char*_tmp27B="%s";_tag_fat(_tmp27B,sizeof(char),3U);});Cyc_fprintf(_tmp584,_tmp583,_tag_fat(_tmp27A,sizeof(void*),1U));});});}
# 1244
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp27D=d;void*_tmp280;struct Cyc_List_List*_tmp27F;struct Cyc_List_List*_tmp27E;struct Cyc_Tcpat_Rhs*_tmp281;switch(*((int*)_tmp27D)){case 1U: _LL1: _tmp281=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp27D)->f1;_LL2: {struct Cyc_Tcpat_Rhs*rhs=_tmp281;
# 1247
Cyc_Tcpat_print_tab(tab);
({void*_tmp282=0U;({struct Cyc___cycFILE*_tmp586=Cyc_stderr;struct _fat_ptr _tmp585=({const char*_tmp283="Success(";_tag_fat(_tmp283,sizeof(char),9U);});Cyc_fprintf(_tmp586,_tmp585,_tag_fat(_tmp282,sizeof(void*),0U));});});Cyc_Tcpat_print_rhs(rhs);({void*_tmp284=0U;({struct Cyc___cycFILE*_tmp588=Cyc_stderr;struct _fat_ptr _tmp587=({const char*_tmp285=")\n";_tag_fat(_tmp285,sizeof(char),3U);});Cyc_fprintf(_tmp588,_tmp587,_tag_fat(_tmp284,sizeof(void*),0U));});});
goto _LL0;}case 0U: _LL3: _LL4:
({void*_tmp286=0U;({struct Cyc___cycFILE*_tmp58A=Cyc_stderr;struct _fat_ptr _tmp589=({const char*_tmp287="Failure\n";_tag_fat(_tmp287,sizeof(char),9U);});Cyc_fprintf(_tmp58A,_tmp589,_tag_fat(_tmp286,sizeof(void*),0U));});});goto _LL0;default: _LL5: _tmp27E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27D)->f1;_tmp27F=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27D)->f2;_tmp280=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27D)->f3;_LL6: {struct Cyc_List_List*a=_tmp27E;struct Cyc_List_List*cases=_tmp27F;void*def=_tmp280;
# 1252
Cyc_Tcpat_print_tab(tab);
({void*_tmp288=0U;({struct Cyc___cycFILE*_tmp58C=Cyc_stderr;struct _fat_ptr _tmp58B=({const char*_tmp289="Switch[";_tag_fat(_tmp289,sizeof(char),8U);});Cyc_fprintf(_tmp58C,_tmp58B,_tag_fat(_tmp288,sizeof(void*),0U));});});
for(0;a != 0;a=a->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)a->hd)->access);
if(a->tl != 0)({void*_tmp28A=0U;({struct Cyc___cycFILE*_tmp58E=Cyc_stderr;struct _fat_ptr _tmp58D=({const char*_tmp28B=",";_tag_fat(_tmp28B,sizeof(char),2U);});Cyc_fprintf(_tmp58E,_tmp58D,_tag_fat(_tmp28A,sizeof(void*),0U));});});}
# 1258
({void*_tmp28C=0U;({struct Cyc___cycFILE*_tmp590=Cyc_stderr;struct _fat_ptr _tmp58F=({const char*_tmp28D="] {\n";_tag_fat(_tmp28D,sizeof(char),5U);});Cyc_fprintf(_tmp590,_tmp58F,_tag_fat(_tmp28C,sizeof(void*),0U));});});
for(0;cases != 0;cases=cases->tl){
struct _tuple0 _tmp28E=*((struct _tuple0*)cases->hd);struct _tuple0 _stmttmp25=_tmp28E;struct _tuple0 _tmp28F=_stmttmp25;void*_tmp291;void*_tmp290;_LL8: _tmp290=_tmp28F.f1;_tmp291=_tmp28F.f2;_LL9: {void*pt=_tmp290;void*d=_tmp291;
Cyc_Tcpat_print_tab(tab);
({void*_tmp292=0U;({struct Cyc___cycFILE*_tmp592=Cyc_stderr;struct _fat_ptr _tmp591=({const char*_tmp293="case ";_tag_fat(_tmp293,sizeof(char),6U);});Cyc_fprintf(_tmp592,_tmp591,_tag_fat(_tmp292,sizeof(void*),0U));});});
Cyc_Tcpat_print_pat_test(pt);
({void*_tmp294=0U;({struct Cyc___cycFILE*_tmp594=Cyc_stderr;struct _fat_ptr _tmp593=({const char*_tmp295=":\n";_tag_fat(_tmp295,sizeof(char),3U);});Cyc_fprintf(_tmp594,_tmp593,_tag_fat(_tmp294,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(d,tab + 7);}}
# 1267
Cyc_Tcpat_print_tab(tab);
({void*_tmp296=0U;({struct Cyc___cycFILE*_tmp596=Cyc_stderr;struct _fat_ptr _tmp595=({const char*_tmp297="default:\n";_tag_fat(_tmp297,sizeof(char),10U);});Cyc_fprintf(_tmp596,_tmp595,_tag_fat(_tmp296,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(def,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp298=0U;({struct Cyc___cycFILE*_tmp598=Cyc_stderr;struct _fat_ptr _tmp597=({const char*_tmp299="}\n";_tag_fat(_tmp299,sizeof(char),3U);});Cyc_fprintf(_tmp598,_tmp597,_tag_fat(_tmp298,sizeof(void*),0U));});});}}_LL0:;}
# 1275
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1283
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp29A=td;struct Cyc_Set_Set*_tmp29B;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp29A)->tag == 1U){_LL1: _tmp29B=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp29A)->f1;_LL2: {struct Cyc_Set_Set*cs=_tmp29B;
# 1293
return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->tag=1U,({struct Cyc_Set_Set*_tmp599=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(cs,c);_tmp29C->f1=_tmp599;});_tmp29C;});}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp432;_tmp432.tag=0U,({struct _fat_ptr _tmp59A=({const char*_tmp29F="add_neg called when td is Positive";_tag_fat(_tmp29F,sizeof(char),35U);});_tmp432.f1=_tmp59A;});_tmp432;});void*_tmp29D[1U];_tmp29D[0]=& _tmp29E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp29D,sizeof(void*),1U));});}_LL0:;}
# 1300
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2A0=td;struct Cyc_Set_Set*_tmp2A1;struct Cyc_Tcpat_Con_s*_tmp2A2;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2A0)->tag == 0U){_LL1: _tmp2A2=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2A0)->f1;_LL2: {struct Cyc_Tcpat_Con_s*c2=_tmp2A2;
# 1304
if(Cyc_Tcpat_compare_con(c,c2)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}}else{_LL3: _tmp2A1=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2A0)->f1;_LL4: {struct Cyc_Set_Set*cs=_tmp2A1;
# 1308
if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(cs,c))return Cyc_Tcpat_No;else{
# 1311
if(c->span != 0 &&({int _tmp59B=*((int*)_check_null(c->span));_tmp59B == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(cs)+ 1;}))
return Cyc_Tcpat_Yes;else{
# 1314
return Cyc_Tcpat_Maybe;}}}}_LL0:;}struct _tuple22{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1322
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp2A3=ctxt;struct Cyc_List_List*_tmp2A6;struct Cyc_List_List*_tmp2A5;struct Cyc_Tcpat_Con_s*_tmp2A4;if(_tmp2A3 == 0){_LL1: _LL2:
 return 0;}else{_LL3: _tmp2A4=((struct _tuple22*)_tmp2A3->hd)->f1;_tmp2A5=((struct _tuple22*)_tmp2A3->hd)->f2;_tmp2A6=_tmp2A3->tl;_LL4: {struct Cyc_Tcpat_Con_s*c=_tmp2A4;struct Cyc_List_List*args=_tmp2A5;struct Cyc_List_List*rest=_tmp2A6;
# 1326
return({struct Cyc_List_List*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));
({struct _tuple22*_tmp59D=({struct _tuple22*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->f1=c,({struct Cyc_List_List*_tmp59C=({struct Cyc_List_List*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->hd=dsc,_tmp2A7->tl=args;_tmp2A7;});_tmp2A8->f2=_tmp59C;});_tmp2A8;});_tmp2A9->hd=_tmp59D;}),_tmp2A9->tl=rest;_tmp2A9;});}}_LL0:;}
# 1334
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp2AA=ctxt;struct Cyc_List_List*_tmp2AD;struct Cyc_List_List*_tmp2AC;struct Cyc_Tcpat_Con_s*_tmp2AB;if(_tmp2AA == 0){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp433;_tmp433.tag=0U,({struct _fat_ptr _tmp59E=({const char*_tmp2B0="norm_context: empty context";_tag_fat(_tmp2B0,sizeof(char),28U);});_tmp433.f1=_tmp59E;});_tmp433;});void*_tmp2AE[1U];_tmp2AE[0]=& _tmp2AF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2AE,sizeof(void*),1U));});}else{_LL3: _tmp2AB=((struct _tuple22*)_tmp2AA->hd)->f1;_tmp2AC=((struct _tuple22*)_tmp2AA->hd)->f2;_tmp2AD=_tmp2AA->tl;_LL4: {struct Cyc_Tcpat_Con_s*c=_tmp2AB;struct Cyc_List_List*args=_tmp2AC;struct Cyc_List_List*rest=_tmp2AD;
# 1338
return({struct Cyc_List_List*_tmp5A0=rest;Cyc_Tcpat_augment(_tmp5A0,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->tag=0U,_tmp2B1->f1=c,({struct Cyc_List_List*_tmp59F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(args);_tmp2B1->f2=_tmp59F;});_tmp2B1;}));});}}_LL0:;}
# 1347
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1349
struct _tuple1 _tmp2B2=({struct _tuple1 _tmp435;_tmp435.f1=ctxt,_tmp435.f2=work;_tmp435;});struct _tuple1 _stmttmp26=_tmp2B2;struct _tuple1 _tmp2B3=_stmttmp26;struct Cyc_List_List*_tmp2B8;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List*_tmp2B5;struct Cyc_Tcpat_Con_s*_tmp2B4;if(_tmp2B3.f1 == 0){if(_tmp2B3.f2 == 0){_LL1: _LL2:
 return dsc;}else{_LL3: _LL4:
 goto _LL6;}}else{if(_tmp2B3.f2 == 0){_LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp434;_tmp434.tag=0U,({struct _fat_ptr _tmp5A1=({const char*_tmp2BB="build_desc: ctxt and work don't match";_tag_fat(_tmp2BB,sizeof(char),38U);});_tmp434.f1=_tmp5A1;});_tmp434;});void*_tmp2B9[1U];_tmp2B9[0]=& _tmp2BA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2B9,sizeof(void*),1U));});}else{_LL7: _tmp2B4=((struct _tuple22*)(_tmp2B3.f1)->hd)->f1;_tmp2B5=((struct _tuple22*)(_tmp2B3.f1)->hd)->f2;_tmp2B6=(_tmp2B3.f1)->tl;_tmp2B7=((struct _tuple21*)(_tmp2B3.f2)->hd)->f3;_tmp2B8=(_tmp2B3.f2)->tl;_LL8: {struct Cyc_Tcpat_Con_s*c=_tmp2B4;struct Cyc_List_List*args=_tmp2B5;struct Cyc_List_List*rest=_tmp2B6;struct Cyc_List_List*dargs=_tmp2B7;struct Cyc_List_List*work2=_tmp2B8;
# 1354
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2BC=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->tag=0U,_tmp2BE->f1=c,({struct Cyc_List_List*_tmp5A3=({struct Cyc_List_List*_tmp5A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(args);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5A2,({struct Cyc_List_List*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD->hd=dsc,_tmp2BD->tl=dargs;_tmp2BD;}));});_tmp2BE->f2=_tmp5A3;});_tmp2BE;});struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*td=_tmp2BC;
return Cyc_Tcpat_build_desc(rest,(void*)td,work2);}}}_LL0:;}
# 1359
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple23{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1366
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp2BF=allmrules;struct Cyc_List_List*_tmp2C2;struct Cyc_Tcpat_Rhs*_tmp2C1;void*_tmp2C0;if(_tmp2BF == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));_tmp2C3->tag=0U,_tmp2C3->f1=dsc;_tmp2C3;});}else{_LL3: _tmp2C0=((struct _tuple23*)_tmp2BF->hd)->f1;_tmp2C1=((struct _tuple23*)_tmp2BF->hd)->f2;_tmp2C2=_tmp2BF->tl;_LL4: {void*pat1=_tmp2C0;struct Cyc_Tcpat_Rhs*rhs1=_tmp2C1;struct Cyc_List_List*rulerest=_tmp2C2;
# 1370
return Cyc_Tcpat_match(pat1,0,dsc,0,0,rhs1,rulerest);}}_LL0:;}
# 1375
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp5A5=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4->tag=1U,({struct Cyc_Set_Set*_tmp5A4=Cyc_Tcpat_empty_con_set();_tmp2C4->f1=_tmp5A4;});_tmp2C4;});Cyc_Tcpat_or_match(_tmp5A5,allmrules);});}
# 1382
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1385
struct Cyc_List_List*_tmp2C5=work;struct Cyc_List_List*_tmp2C9;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2C7;struct Cyc_List_List*_tmp2C6;struct Cyc_List_List*_tmp2CA;if(_tmp2C5 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->tag=1U,_tmp2CB->f1=right_hand_side;_tmp2CB;});}else{if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2C5)->hd)->f1 == 0){if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2C5)->hd)->f2 == 0){if(((struct _tuple21*)((struct Cyc_List_List*)_tmp2C5)->hd)->f3 == 0){_LL3: _tmp2CA=_tmp2C5->tl;_LL4: {struct Cyc_List_List*workr=_tmp2CA;
# 1388
return({struct Cyc_List_List*_tmp5A8=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp5A7=workr;struct Cyc_Tcpat_Rhs*_tmp5A6=right_hand_side;Cyc_Tcpat_and_match(_tmp5A8,_tmp5A7,_tmp5A6,rules);});}}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _tmp2C6=((struct _tuple21*)_tmp2C5->hd)->f1;_tmp2C7=((struct _tuple21*)_tmp2C5->hd)->f2;_tmp2C8=((struct _tuple21*)_tmp2C5->hd)->f3;_tmp2C9=_tmp2C5->tl;_LL6: {struct Cyc_List_List*pats=_tmp2C6;struct Cyc_List_List*objs=_tmp2C7;struct Cyc_List_List*dscs=_tmp2C8;struct Cyc_List_List*workr=_tmp2C9;
# 1390
if((pats == 0 || objs == 0)|| dscs == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp436;_tmp436.tag=0U,({struct _fat_ptr _tmp5A9=({const char*_tmp2CE="tcpat:and_match: malformed work frame";_tag_fat(_tmp2CE,sizeof(char),38U);});_tmp436.f1=_tmp5A9;});_tmp436;});void*_tmp2CC[1U];_tmp2CC[0]=& _tmp2CD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2CC,sizeof(void*),1U));});{
struct Cyc_List_List*_tmp2CF=pats;struct Cyc_List_List*_stmttmp27=_tmp2CF;struct Cyc_List_List*_tmp2D0=_stmttmp27;struct Cyc_List_List*_tmp2D2;void*_tmp2D1;_LL8: _tmp2D1=(void*)_tmp2D0->hd;_tmp2D2=_tmp2D0->tl;_LL9: {void*pat1=_tmp2D1;struct Cyc_List_List*patr=_tmp2D2;
struct Cyc_List_List*_tmp2D3=objs;struct Cyc_List_List*_stmttmp28=_tmp2D3;struct Cyc_List_List*_tmp2D4=_stmttmp28;struct Cyc_List_List*_tmp2D6;struct Cyc_List_List*_tmp2D5;_LLB: _tmp2D5=(struct Cyc_List_List*)_tmp2D4->hd;_tmp2D6=_tmp2D4->tl;_LLC: {struct Cyc_List_List*obj1=_tmp2D5;struct Cyc_List_List*objr=_tmp2D6;
struct Cyc_List_List*_tmp2D7=dscs;struct Cyc_List_List*_stmttmp29=_tmp2D7;struct Cyc_List_List*_tmp2D8=_stmttmp29;struct Cyc_List_List*_tmp2DA;void*_tmp2D9;_LLE: _tmp2D9=(void*)_tmp2D8->hd;_tmp2DA=_tmp2D8->tl;_LLF: {void*dsc1=_tmp2D9;struct Cyc_List_List*dscr=_tmp2DA;
struct _tuple21*_tmp2DB=({struct _tuple21*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->f1=patr,_tmp2DD->f2=objr,_tmp2DD->f3=dscr;_tmp2DD;});struct _tuple21*wf=_tmp2DB;
return({void*_tmp5AF=pat1;struct Cyc_List_List*_tmp5AE=obj1;void*_tmp5AD=dsc1;struct Cyc_List_List*_tmp5AC=ctx;struct Cyc_List_List*_tmp5AB=({struct Cyc_List_List*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));_tmp2DC->hd=wf,_tmp2DC->tl=workr;_tmp2DC;});struct Cyc_Tcpat_Rhs*_tmp5AA=right_hand_side;Cyc_Tcpat_match(_tmp5AF,_tmp5AE,_tmp5AD,_tmp5AC,_tmp5AB,_tmp5AA,rules);});}}}}}}}_LL0:;}
# 1401
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp2DE=dsc;struct Cyc_List_List*_tmp2DF;struct Cyc_Set_Set*_tmp2E0;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2DE)->tag == 1U){_LL1: _tmp2E0=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2DE)->f1;_LL2: {struct Cyc_Set_Set*ncs=_tmp2E0;
# 1407
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3->tag=1U,({struct Cyc_Set_Set*_tmp5B0=Cyc_Tcpat_empty_con_set();_tmp2E3->f1=_tmp5B0;});_tmp2E3;});
struct Cyc_List_List*_tmp2E1=0;struct Cyc_List_List*res=_tmp2E1;
{int i=0;for(0;i < pcon->arity;++ i){
res=({struct Cyc_List_List*_tmp2E2=_cycalloc(sizeof(*_tmp2E2));_tmp2E2->hd=any,_tmp2E2->tl=res;_tmp2E2;});}}
return res;}}else{_LL3: _tmp2DF=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2DE)->f2;_LL4: {struct Cyc_List_List*dargs=_tmp2DF;
return dargs;}}_LL0:;}
# 1416
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp2E4=pw;struct Cyc_Absyn_Pat*_tmp2E5;if((_tmp2E4.where_clause).tag == 2){_LL1: _LL2:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL3: _tmp2E5=(_tmp2E4.pattern).val;_LL4: {struct Cyc_Absyn_Pat*p=_tmp2E5;
# 1420
void*_tmp2E6=p->r;void*_stmttmp2A=_tmp2E6;void*_tmp2E7=_stmttmp2A;struct Cyc_List_List*_tmp2E9;union Cyc_Absyn_AggrInfo _tmp2E8;struct Cyc_Absyn_Datatypefield*_tmp2EB;struct Cyc_Absyn_Datatypedecl*_tmp2EA;switch(*((int*)_tmp2E7)){case 6U: _LL6: _LL7:
# 1422
 if(i != 0)
({struct Cyc_Int_pa_PrintArg_struct _tmp2EE=({struct Cyc_Int_pa_PrintArg_struct _tmp437;_tmp437.tag=1U,_tmp437.f1=(unsigned long)i;_tmp437;});void*_tmp2EC[1U];_tmp2EC[0]=& _tmp2EE;({struct _fat_ptr _tmp5B1=({const char*_tmp2ED="get_access on pointer pattern with offset %d\n";_tag_fat(_tmp2ED,sizeof(char),46U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp5B1,_tag_fat(_tmp2EC,sizeof(void*),1U));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5U: _LL8: _LL9:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF->tag=2U,_tmp2EF->f1=(unsigned)i;_tmp2EF;});case 8U: _LLA: _tmp2EA=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2E7)->f1;_tmp2EB=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2E7)->f2;_LLB: {struct Cyc_Absyn_Datatypedecl*tud=_tmp2EA;struct Cyc_Absyn_Datatypefield*tuf=_tmp2EB;
return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0->tag=3U,_tmp2F0->f1=tud,_tmp2F0->f2=tuf,_tmp2F0->f3=(unsigned)i;_tmp2F0;});}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2E7)->f1 != 0){_LLC: _tmp2E8=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2E7)->f1;_tmp2E9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2E7)->f3;_LLD: {union Cyc_Absyn_AggrInfo info=_tmp2E8;struct Cyc_List_List*dlps=_tmp2E9;
# 1428
struct Cyc_Absyn_Aggrdecl*_tmp2F1=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2F1;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
struct Cyc_List_List*_tmp2F2=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(dlps))->hd)).f1;struct Cyc_List_List*_stmttmp2B=_tmp2F2;struct Cyc_List_List*_tmp2F3=_stmttmp2B;struct _fat_ptr*_tmp2F4;if(_tmp2F3 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp2F3)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp2F3)->tl == 0){_LL11: _tmp2F4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2F3->hd)->f1;_LL12: {struct _fat_ptr*f=_tmp2F4;
# 1432
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5->tag=4U,_tmp2F5->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmp2F5->f2=f;_tmp2F5;});}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _LL14:
({struct Cyc_String_pa_PrintArg_struct _tmp2F8=({struct Cyc_String_pa_PrintArg_struct _tmp438;_tmp438.tag=0U,({struct _fat_ptr _tmp5B2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(p));_tmp438.f1=_tmp5B2;});_tmp438;});void*_tmp2F6[1U];_tmp2F6[0]=& _tmp2F8;({struct _fat_ptr _tmp5B3=({const char*_tmp2F7="get_access on bad aggr pattern: %s";_tag_fat(_tmp2F7,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp5B3,_tag_fat(_tmp2F6,sizeof(void*),1U));});});}_LL10:;}{
# 1436
struct Cyc_List_List*_tmp2F9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp2F9;
int _tmp2FA=i;int orig_i=_tmp2FA;
for(0;i != 0;-- i){fields=((struct Cyc_List_List*)_check_null(fields))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->tag=4U,_tmp2FB->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmp2FB->f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fields))->hd)->name;_tmp2FB;});}}}else{goto _LLE;}default: _LLE: _LLF:
({struct Cyc_String_pa_PrintArg_struct _tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp439;_tmp439.tag=0U,({struct _fat_ptr _tmp5B4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_pat2string(p));_tmp439.f1=_tmp5B4;});_tmp439;});void*_tmp2FC[1U];_tmp2FC[0]=& _tmp2FE;({struct _fat_ptr _tmp5B5=({const char*_tmp2FD="get_access on bad pattern: %s";_tag_fat(_tmp2FD,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp5B5,_tag_fat(_tmp2FC,sizeof(void*),1U));});});}_LL5:;}}_LL0:;}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1446
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple24*env,int i){
struct _tuple24*_tmp2FF=env;struct Cyc_Tcpat_Con_s*_tmp301;struct Cyc_List_List*_tmp300;_LL1: _tmp300=_tmp2FF->f1;_tmp301=_tmp2FF->f2;_LL2: {struct Cyc_List_List*obj=_tmp300;struct Cyc_Tcpat_Con_s*pcon=_tmp301;
void*acc=Cyc_Tcpat_get_access(pcon->orig_pat,i);
return({struct Cyc_List_List*_tmp303=_cycalloc(sizeof(*_tmp303));({struct Cyc_Tcpat_PathNode*_tmp5B6=({struct Cyc_Tcpat_PathNode*_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302->orig_pat=pcon->orig_pat,_tmp302->access=acc;_tmp302;});_tmp303->hd=_tmp5B6;}),_tmp303->tl=obj;_tmp303;});}}
# 1451
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple24 _tmp304=({struct _tuple24 _tmp43A;_tmp43A.f1=obj,_tmp43A.f2=pcon;_tmp43A;});struct _tuple24 env=_tmp304;
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple24*,int),struct _tuple24*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& env);}
# 1459
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1463
void*_tmp305=p;struct Cyc_List_List*_tmp307;struct Cyc_Tcpat_Con_s*_tmp306;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp305)->tag == 0U){_LL1: _LL2:
# 1465
 return({struct Cyc_List_List*_tmp5B9=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp5B8=work;struct Cyc_Tcpat_Rhs*_tmp5B7=right_hand_side;Cyc_Tcpat_and_match(_tmp5B9,_tmp5B8,_tmp5B7,rules);});}else{_LL3: _tmp306=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp305)->f1;_tmp307=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp305)->f2;_LL4: {struct Cyc_Tcpat_Con_s*pcon=_tmp306;struct Cyc_List_List*pargs=_tmp307;
# 1467
enum Cyc_Tcpat_Answer _tmp308=Cyc_Tcpat_static_match(pcon,dsc);enum Cyc_Tcpat_Answer _stmttmp2C=_tmp308;enum Cyc_Tcpat_Answer _tmp309=_stmttmp2C;switch(_tmp309){case Cyc_Tcpat_Yes: _LL6: _LL7: {
# 1469
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp30D=_cycalloc(sizeof(*_tmp30D));({struct _tuple22*_tmp5BA=({struct _tuple22*_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C->f1=pcon,_tmp30C->f2=0;_tmp30C;});_tmp30D->hd=_tmp5BA;}),_tmp30D->tl=ctx;_tmp30D;});
struct _tuple21*work_frame=({struct _tuple21*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B->f1=pargs,({struct Cyc_List_List*_tmp5BC=Cyc_Tcpat_getoargs(pcon,obj);_tmp30B->f2=_tmp5BC;}),({
struct Cyc_List_List*_tmp5BB=Cyc_Tcpat_getdargs(pcon,dsc);_tmp30B->f3=_tmp5BB;});_tmp30B;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A->hd=work_frame,_tmp30A->tl=work;_tmp30A;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL8: _LL9:
# 1475
 return({void*_tmp5BD=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp5BD,rules);});default: _LLA: _LLB: {
# 1477
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp313=_cycalloc(sizeof(*_tmp313));({struct _tuple22*_tmp5BE=({struct _tuple22*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->f1=pcon,_tmp312->f2=0;_tmp312;});_tmp313->hd=_tmp5BE;}),_tmp313->tl=ctx;_tmp313;});
struct _tuple21*work_frame=({struct _tuple21*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311->f1=pargs,({struct Cyc_List_List*_tmp5C0=Cyc_Tcpat_getoargs(pcon,obj);_tmp311->f2=_tmp5C0;}),({
struct Cyc_List_List*_tmp5BF=Cyc_Tcpat_getdargs(pcon,dsc);_tmp311->f3=_tmp5BF;});_tmp311;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310->hd=work_frame,_tmp310->tl=work;_tmp310;});
void*_tmp30E=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);void*s=_tmp30E;
void*_tmp30F=({void*_tmp5C3=({struct Cyc_List_List*_tmp5C2=ctx;void*_tmp5C1=Cyc_Tcpat_add_neg(dsc,pcon);Cyc_Tcpat_build_desc(_tmp5C2,_tmp5C1,work);});Cyc_Tcpat_or_match(_tmp5C3,rules);});void*f=_tmp30F;
# 1484
return Cyc_Tcpat_ifeq(obj,pcon,s,f);}}_LL5:;}}_LL0:;}
# 1494
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1499
void*_tmp314=d;void*_tmp316;struct Cyc_List_List*_tmp315;struct Cyc_Tcpat_Rhs*_tmp317;void*_tmp318;switch(*((int*)_tmp314)){case 0U: _LL1: _tmp318=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp314)->f1;_LL2: {void*td=_tmp318;
# 1501
if(!(*exhaust_done)){not_exhaust(env1,td);*exhaust_done=1;}
goto _LL0;}case 1U: _LL3: _tmp317=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp314)->f1;_LL4: {struct Cyc_Tcpat_Rhs*r=_tmp317;
rhs_appears(env2,r);goto _LL0;}default: _LL5: _tmp315=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp314)->f2;_tmp316=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp314)->f3;_LL6: {struct Cyc_List_List*cases=_tmp315;void*def=_tmp316;
# 1505
for(0;cases != 0;cases=cases->tl){
struct _tuple0 _tmp319=*((struct _tuple0*)cases->hd);struct _tuple0 _stmttmp2D=_tmp319;struct _tuple0 _tmp31A=_stmttmp2D;void*_tmp31B;_LL8: _tmp31B=_tmp31A.f2;_LL9: {void*d=_tmp31B;
Cyc_Tcpat_check_exhaust_overlap(d,not_exhaust,env1,rhs_appears,env2,exhaust_done);}}
# 1510
Cyc_Tcpat_check_exhaust_overlap(def,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1512
goto _LL0;}}_LL0:;}
# 1520
static struct _tuple23*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1522
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->used=0,_tmp321->pat_loc=(swc->pattern)->loc,_tmp321->rhs=swc->body;_tmp321;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp31C=({union Cyc_Tcpat_PatOrWhere _tmp43B;(_tmp43B.where_clause).tag=2U,(_tmp43B.where_clause).val=swc->where_clause;_tmp43B;});union Cyc_Tcpat_PatOrWhere w=_tmp31C;
sp=({struct Cyc_List_List*_tmp5C5=({void*_tmp31D[2U];_tmp31D[0]=sp0,({void*_tmp5C4=Cyc_Tcpat_int_pat(*ctr,w);_tmp31D[1]=_tmp5C4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp31D,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp5C5,w);});
*ctr=*ctr + 1;}else{
# 1530
sp=({struct Cyc_List_List*_tmp5C7=({void*_tmp31E[2U];_tmp31E[0]=sp0,({void*_tmp5C6=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->tag=0U;_tmp31F;});_tmp31E[1]=_tmp5C6;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp31E,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp5C7,({union Cyc_Tcpat_PatOrWhere _tmp43C;(_tmp43C.where_clause).tag=2U,(_tmp43C.where_clause).val=0;_tmp43C;}));});}
return({struct _tuple23*_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320->f1=sp,_tmp320->f2=rhs;_tmp320;});}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1536
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1538
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp322=d;struct Cyc_Set_Set*_tmp323;struct Cyc_List_List*_tmp325;struct Cyc_Tcpat_Con_s*_tmp324;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp322)->tag == 0U){_LL1: _tmp324=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp322)->f1;_tmp325=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp322)->f2;_LL2: {struct Cyc_Tcpat_Con_s*c=_tmp324;struct Cyc_List_List*ds=_tmp325;
# 1542
union Cyc_Tcpat_Name_value _tmp326=c->name;union Cyc_Tcpat_Name_value n=_tmp326;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp327=c->orig_pat;union Cyc_Tcpat_PatOrWhere _stmttmp2E=_tmp327;union Cyc_Tcpat_PatOrWhere _tmp328=_stmttmp2E;struct Cyc_Absyn_Pat*_tmp329;if((_tmp328.where_clause).tag == 2){_LL6: _LL7:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(ds))->hd);}else{_LL8: _tmp329=(_tmp328.pattern).val;_LL9: {struct Cyc_Absyn_Pat*p2=_tmp329;
p=p2;goto _LL5;}}_LL5:;}{
# 1548
void*_tmp32A=p->r;void*_stmttmp2F=_tmp32A;void*_tmp32B=_stmttmp2F;struct Cyc_Absyn_Exp*_tmp32C;struct Cyc_Absyn_Enumfield*_tmp32D;struct Cyc_Absyn_Enumfield*_tmp32E;int _tmp330;struct _fat_ptr _tmp32F;char _tmp331;int _tmp332;struct Cyc_Absyn_Datatypefield*_tmp333;struct Cyc_List_List*_tmp335;struct Cyc_Absyn_Aggrdecl*_tmp334;struct Cyc_Absyn_Vardecl*_tmp337;struct Cyc_Absyn_Tvar*_tmp336;struct Cyc_Absyn_Vardecl*_tmp338;struct Cyc_Absyn_Vardecl*_tmp339;switch(*((int*)_tmp32B)){case 0U: _LLB: _LLC:
 return({const char*_tmp33A="_";_tag_fat(_tmp33A,sizeof(char),2U);});case 1U: _LLD: _tmp339=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp32B)->f1;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp339;
return Cyc_Absynpp_qvar2string(vd->name);}case 3U: _LLF: _tmp338=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp32B)->f1;_LL10: {struct Cyc_Absyn_Vardecl*vd=_tmp338;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33D=({struct Cyc_String_pa_PrintArg_struct _tmp43D;_tmp43D.tag=0U,({struct _fat_ptr _tmp5C8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp43D.f1=_tmp5C8;});_tmp43D;});void*_tmp33B[1U];_tmp33B[0]=& _tmp33D;({struct _fat_ptr _tmp5C9=({const char*_tmp33C="*%s";_tag_fat(_tmp33C,sizeof(char),4U);});Cyc_aprintf(_tmp5C9,_tag_fat(_tmp33B,sizeof(void*),1U));});});}case 4U: _LL11: _tmp336=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp32B)->f1;_tmp337=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp32B)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp336;struct Cyc_Absyn_Vardecl*vd=_tmp337;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp340=({struct Cyc_String_pa_PrintArg_struct _tmp43F;_tmp43F.tag=0U,({struct _fat_ptr _tmp5CA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp43F.f1=_tmp5CA;});_tmp43F;});struct Cyc_String_pa_PrintArg_struct _tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp43E;_tmp43E.tag=0U,_tmp43E.f1=(struct _fat_ptr)((struct _fat_ptr)*tv->name);_tmp43E;});void*_tmp33E[2U];_tmp33E[0]=& _tmp340,_tmp33E[1]=& _tmp341;({struct _fat_ptr _tmp5CB=({const char*_tmp33F="%s<`%s>";_tag_fat(_tmp33F,sizeof(char),8U);});Cyc_aprintf(_tmp5CB,_tag_fat(_tmp33E,sizeof(void*),2U));});});}case 5U: _LL13: _LL14:
# 1554
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp344=({struct Cyc_String_pa_PrintArg_struct _tmp440;_tmp440.tag=0U,({struct _fat_ptr _tmp5CC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp440.f1=_tmp5CC;});_tmp440;});void*_tmp342[1U];_tmp342[0]=& _tmp344;({struct _fat_ptr _tmp5CD=({const char*_tmp343="$(%s)";_tag_fat(_tmp343,sizeof(char),6U);});Cyc_aprintf(_tmp5CD,_tag_fat(_tmp342,sizeof(void*),1U));});});case 6U: _LL15: _LL16:
# 1556
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp347=({struct Cyc_String_pa_PrintArg_struct _tmp441;_tmp441.tag=0U,({struct _fat_ptr _tmp5CE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp441.f1=_tmp5CE;});_tmp441;});void*_tmp345[1U];_tmp345[0]=& _tmp347;({struct _fat_ptr _tmp5CF=({const char*_tmp346="&%s";_tag_fat(_tmp346,sizeof(char),4U);});Cyc_aprintf(_tmp5CF,_tag_fat(_tmp345,sizeof(void*),1U));});});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp32B)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp32B)->f1)->KnownAggr).tag == 2){_LL17: _tmp334=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp32B)->f1)->KnownAggr).val;_tmp335=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp32B)->f3;_LL18: {struct Cyc_Absyn_Aggrdecl*ad=_tmp334;struct Cyc_List_List*dlps=_tmp335;
# 1558
if((int)ad->kind == (int)Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp348=dlps;struct _fat_ptr*_tmp349;if(_tmp348 != 0){if(((struct _tuple19*)((struct Cyc_List_List*)_tmp348)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple19*)((struct Cyc_List_List*)_tmp348)->hd)->f1)->hd)->tag == 1U){_LL2C: _tmp349=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple19*)_tmp348->hd)->f1)->hd)->f1;_LL2D: {struct _fat_ptr*f=_tmp349;
# 1561
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34C=({struct Cyc_String_pa_PrintArg_struct _tmp444;_tmp444.tag=0U,({struct _fat_ptr _tmp5D0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp444.f1=_tmp5D0;});_tmp444;});struct Cyc_String_pa_PrintArg_struct _tmp34D=({struct Cyc_String_pa_PrintArg_struct _tmp443;_tmp443.tag=0U,_tmp443.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp443;});struct Cyc_String_pa_PrintArg_struct _tmp34E=({struct Cyc_String_pa_PrintArg_struct _tmp442;_tmp442.tag=0U,({
struct _fat_ptr _tmp5D1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp442.f1=_tmp5D1;});_tmp442;});void*_tmp34A[3U];_tmp34A[0]=& _tmp34C,_tmp34A[1]=& _tmp34D,_tmp34A[2]=& _tmp34E;({struct _fat_ptr _tmp5D2=({const char*_tmp34B="%s{.%s = %s}";_tag_fat(_tmp34B,sizeof(char),13U);});Cyc_aprintf(_tmp5D2,_tag_fat(_tmp34A,sizeof(void*),3U));});});}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 1566
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp351=({struct Cyc_String_pa_PrintArg_struct _tmp446;_tmp446.tag=0U,({struct _fat_ptr _tmp5D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp446.f1=_tmp5D3;});_tmp446;});struct Cyc_String_pa_PrintArg_struct _tmp352=({struct Cyc_String_pa_PrintArg_struct _tmp445;_tmp445.tag=0U,({struct _fat_ptr _tmp5D4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp445.f1=_tmp5D4;});_tmp445;});void*_tmp34F[2U];_tmp34F[0]=& _tmp351,_tmp34F[1]=& _tmp352;({struct _fat_ptr _tmp5D5=({const char*_tmp350="%s{%s}";_tag_fat(_tmp350,sizeof(char),7U);});Cyc_aprintf(_tmp5D5,_tag_fat(_tmp34F,sizeof(void*),2U));});});}}else{goto _LL29;}}else{goto _LL29;}case 8U: _LL19: _tmp333=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp32B)->f2;_LL1A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp333;
# 1568
if(ds == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp355=({struct Cyc_String_pa_PrintArg_struct _tmp447;_tmp447.tag=0U,({struct _fat_ptr _tmp5D6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp447.f1=_tmp5D6;});_tmp447;});void*_tmp353[1U];_tmp353[0]=& _tmp355;({struct _fat_ptr _tmp5D7=({const char*_tmp354="%s";_tag_fat(_tmp354,sizeof(char),3U);});Cyc_aprintf(_tmp5D7,_tag_fat(_tmp353,sizeof(void*),1U));});});else{
# 1571
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp358=({struct Cyc_String_pa_PrintArg_struct _tmp449;_tmp449.tag=0U,({struct _fat_ptr _tmp5D8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp449.f1=_tmp5D8;});_tmp449;});struct Cyc_String_pa_PrintArg_struct _tmp359=({struct Cyc_String_pa_PrintArg_struct _tmp448;_tmp448.tag=0U,({struct _fat_ptr _tmp5D9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcpat_descs2string(ds));_tmp448.f1=_tmp5D9;});_tmp448;});void*_tmp356[2U];_tmp356[0]=& _tmp358,_tmp356[1]=& _tmp359;({struct _fat_ptr _tmp5DA=({const char*_tmp357="%s(%s)";_tag_fat(_tmp357,sizeof(char),7U);});Cyc_aprintf(_tmp5DA,_tag_fat(_tmp356,sizeof(void*),2U));});});}}case 9U: _LL1B: _LL1C:
 return({const char*_tmp35A="NULL";_tag_fat(_tmp35A,sizeof(char),5U);});case 10U: _LL1D: _tmp332=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp32B)->f2;_LL1E: {int i=_tmp332;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp35D=({struct Cyc_Int_pa_PrintArg_struct _tmp44A;_tmp44A.tag=1U,_tmp44A.f1=(unsigned long)i;_tmp44A;});void*_tmp35B[1U];_tmp35B[0]=& _tmp35D;({struct _fat_ptr _tmp5DB=({const char*_tmp35C="%d";_tag_fat(_tmp35C,sizeof(char),3U);});Cyc_aprintf(_tmp5DB,_tag_fat(_tmp35B,sizeof(void*),1U));});});}case 11U: _LL1F: _tmp331=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp32B)->f1;_LL20: {char c=_tmp331;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp360=({struct Cyc_Int_pa_PrintArg_struct _tmp44B;_tmp44B.tag=1U,_tmp44B.f1=(unsigned long)((int)c);_tmp44B;});void*_tmp35E[1U];_tmp35E[0]=& _tmp360;({struct _fat_ptr _tmp5DC=({const char*_tmp35F="%d";_tag_fat(_tmp35F,sizeof(char),3U);});Cyc_aprintf(_tmp5DC,_tag_fat(_tmp35E,sizeof(void*),1U));});});}case 12U: _LL21: _tmp32F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp32B)->f1;_tmp330=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp32B)->f2;_LL22: {struct _fat_ptr f=_tmp32F;int i=_tmp330;
return f;}case 13U: _LL23: _tmp32E=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp32B)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp32E;
_tmp32D=ef;goto _LL26;}case 14U: _LL25: _tmp32D=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp32B)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp32D;
return Cyc_Absynpp_qvar2string(ef->name);}case 17U: _LL27: _tmp32C=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp32B)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp32C;
return Cyc_Absynpp_exp2string(e);}default: _LL29: _LL2A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LLA:;}}}else{_LL3: _tmp323=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp322)->f1;_LL4: {struct Cyc_Set_Set*s=_tmp323;
# 1582
if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(s))return({const char*_tmp361="_";_tag_fat(_tmp361,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*_tmp362=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(s);struct Cyc_Tcpat_Con_s*c=_tmp362;
# 1586
if((int)(((c->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp363=({union Cyc_Tcpat_PatOrWhere _tmp383=c->orig_pat;if((_tmp383.pattern).tag != 1)_throw_match();(_tmp383.pattern).val;});struct Cyc_Absyn_Pat*orig_pat=_tmp363;
void*_tmp364=Cyc_Tcutil_compress((void*)_check_null(orig_pat->topt));void*_stmttmp30=_tmp364;void*_tmp365=_stmttmp30;struct Cyc_Absyn_PtrAtts _tmp366;struct Cyc_Absyn_Aggrdecl*_tmp367;struct Cyc_Absyn_Datatypedecl*_tmp368;switch(*((int*)_tmp365)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f1)->f2 == Cyc_Absyn_Char_sz){_LL31: _LL32:
# 1591
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp369=Cyc_Tcpat_char_con((char)i,orig_pat);struct Cyc_Tcpat_Con_s*c=_tmp369;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(s,c))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp36C=({struct Cyc_Int_pa_PrintArg_struct _tmp44C;_tmp44C.tag=1U,_tmp44C.f1=(unsigned long)i;_tmp44C;});void*_tmp36A[1U];_tmp36A[0]=& _tmp36C;({struct _fat_ptr _tmp5DD=({const char*_tmp36B="%d";_tag_fat(_tmp36B,sizeof(char),3U);});Cyc_aprintf(_tmp5DD,_tag_fat(_tmp36A,sizeof(void*),1U));});});}}
# 1596
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL33: _LL34:
# 1599
{unsigned i=0U;for(0;i < (unsigned)-1;++ i){
struct Cyc_Tcpat_Con_s*_tmp36D=Cyc_Tcpat_int_con((int)i,c->orig_pat);struct Cyc_Tcpat_Con_s*c=_tmp36D;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(s,c))
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp370=({struct Cyc_Int_pa_PrintArg_struct _tmp44D;_tmp44D.tag=1U,_tmp44D.f1=(unsigned long)((int)i);_tmp44D;});void*_tmp36E[1U];_tmp36E[0]=& _tmp370;({struct _fat_ptr _tmp5DE=({const char*_tmp36F="%d";_tag_fat(_tmp36F,sizeof(char),3U);});Cyc_aprintf(_tmp5DE,_tag_fat(_tmp36E,sizeof(void*),1U));});});}}
# 1604
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f1)->f1).KnownDatatype).tag == 2){_LL37: _tmp368=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f1)->f1).KnownDatatype).val;_LL38: {struct Cyc_Absyn_Datatypedecl*tud=_tmp368;
# 1614
if(tud->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp374=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp374;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
for(0;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)fields->hd)->name).f2;
struct Cyc_List_List*_tmp375=((struct Cyc_Absyn_Datatypefield*)fields->hd)->typs;struct Cyc_List_List*args=_tmp375;
if(!({struct Cyc_Set_Set*_tmp5E0=s;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5E0,({struct Cyc_Tcpat_Con_s*_tmp376=_cycalloc(sizeof(*_tmp376));({union Cyc_Tcpat_Name_value _tmp5DF=Cyc_Tcpat_Name_v(n);_tmp376->name=_tmp5DF;}),_tmp376->arity=0,_tmp376->span=0,_tmp376->orig_pat=c->orig_pat;_tmp376;}));})){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args)== 0)
return n;else{
# 1624
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp379=({struct Cyc_String_pa_PrintArg_struct _tmp44E;_tmp44E.tag=0U,_tmp44E.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp44E;});void*_tmp377[1U];_tmp377[0]=& _tmp379;({struct _fat_ptr _tmp5E1=({const char*_tmp378="%s(...)";_tag_fat(_tmp378,sizeof(char),8U);});Cyc_aprintf(_tmp5E1,_tag_fat(_tmp377,sizeof(void*),1U));});});}}}
# 1627
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL3B;}case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp367=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp365)->f1)->f1).KnownAggr).val;if((int)_tmp367->kind == (int)Cyc_Absyn_UnionA){_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp367;
# 1629
struct Cyc_List_List*_tmp37A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp37A;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
struct _tuple2*_tmp37B=ad->name;struct _tuple2*_stmttmp31=_tmp37B;struct _tuple2*_tmp37C=_stmttmp31;struct _fat_ptr _tmp37D;_LL3E: _tmp37D=*_tmp37C->f2;_LL3F: {struct _fat_ptr union_name=_tmp37D;
for(0;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;
if(!({struct Cyc_Set_Set*_tmp5E3=s;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5E3,({struct Cyc_Tcpat_Con_s*_tmp37E=_cycalloc(sizeof(*_tmp37E));({union Cyc_Tcpat_Name_value _tmp5E2=Cyc_Tcpat_Name_v(n);_tmp37E->name=_tmp5E2;}),_tmp37E->arity=0,_tmp37E->span=0,_tmp37E->orig_pat=c->orig_pat;_tmp37E;}));}))
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp381=({struct Cyc_String_pa_PrintArg_struct _tmp450;_tmp450.tag=0U,_tmp450.f1=(struct _fat_ptr)((struct _fat_ptr)union_name);_tmp450;});struct Cyc_String_pa_PrintArg_struct _tmp382=({struct Cyc_String_pa_PrintArg_struct _tmp44F;_tmp44F.tag=0U,_tmp44F.f1=(struct _fat_ptr)((struct _fat_ptr)n);_tmp44F;});void*_tmp37F[2U];_tmp37F[0]=& _tmp381,_tmp37F[1]=& _tmp382;({struct _fat_ptr _tmp5E4=({const char*_tmp380="%s{.%s = _}";_tag_fat(_tmp380,sizeof(char),12U);});Cyc_aprintf(_tmp5E4,_tag_fat(_tmp37F,sizeof(void*),2U));});});}
# 1637
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL3B;}}else{goto _LL3B;}default: goto _LL3B;}case 3U: _LL35: _tmp366=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365)->f1).ptr_atts;_LL36: {struct Cyc_Absyn_PtrAtts patts=_tmp366;
# 1606
void*_tmp371=patts.nullable;void*n=_tmp371;
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
if(is_nullable){
if(!({struct Cyc_Set_Set*_tmp5E5=s;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5E5,Cyc_Tcpat_null_con(orig_pat));}))
return({const char*_tmp372="NULL";_tag_fat(_tmp372,sizeof(char),5U);});}
# 1612
return({const char*_tmp373="&_";_tag_fat(_tmp373,sizeof(char),3U);});}default: _LL3B: _LL3C:
# 1638
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL30:;}}}}_LL0:;}
# 1642
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _fat_ptr*_tmp384=_cycalloc(sizeof(*_tmp384));({struct _fat_ptr _tmp5E6=Cyc_Tcpat_desc2string(d);*_tmp384=_tmp5E6;});_tmp384;});}
# 1645
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp385=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);struct Cyc_List_List*ss=_tmp385;
struct _fat_ptr*comma=({struct _fat_ptr*_tmp389=_cycalloc(sizeof(*_tmp389));({struct _fat_ptr _tmp5E7=({const char*_tmp388=",";_tag_fat(_tmp388,sizeof(char),2U);});*_tmp389=_tmp5E7;});_tmp389;});
{struct Cyc_List_List*_tmp386=ss;struct Cyc_List_List*x=_tmp386;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
if(x->tl != 0){
({struct Cyc_List_List*_tmp5E8=({struct Cyc_List_List*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->hd=comma,_tmp387->tl=x->tl;_tmp387;});x->tl=_tmp5E8;});
x=x->tl;}}}
# 1654
return(struct _fat_ptr)Cyc_strconcat_l(ss);}
# 1657
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){
struct _handler_cons _tmp38A;_push_handler(& _tmp38A);{int _tmp38C=0;if(setjmp(_tmp38A.handler))_tmp38C=1;if(!_tmp38C){
{struct _fat_ptr _tmp38D=Cyc_Tcpat_desc2string(desc);struct _fat_ptr s=_tmp38D;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp452;_tmp452.tag=0U,({struct _fat_ptr _tmp5E9=({const char*_tmp391="patterns may not be exhaustive.\n\tmissing case for ";_tag_fat(_tmp391,sizeof(char),51U);});_tmp452.f1=_tmp5E9;});_tmp452;});struct Cyc_Warn_String_Warn_Warg_struct _tmp390=({struct Cyc_Warn_String_Warn_Warg_struct _tmp451;_tmp451.tag=0U,_tmp451.f1=s;_tmp451;});void*_tmp38E[2U];_tmp38E[0]=& _tmp38F,_tmp38E[1]=& _tmp390;({unsigned _tmp5EA=loc;Cyc_Warn_err2(_tmp5EA,_tag_fat(_tmp38E,sizeof(void*),2U));});});}
# 1659
;_pop_handler();}else{void*_tmp38B=(void*)Cyc_Core_get_exn_thrown();void*_tmp392=_tmp38B;void*_tmp393;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp392)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1663
({struct Cyc_Warn_String_Warn_Warg_struct _tmp395=({struct Cyc_Warn_String_Warn_Warg_struct _tmp453;_tmp453.tag=0U,({struct _fat_ptr _tmp5EB=({const char*_tmp396="patterns may not be exhaustive.";_tag_fat(_tmp396,sizeof(char),32U);});_tmp453.f1=_tmp5EB;});_tmp453;});void*_tmp394[1U];_tmp394[0]=& _tmp395;({unsigned _tmp5EC=loc;Cyc_Warn_err2(_tmp5EC,_tag_fat(_tmp394,sizeof(void*),1U));});});
goto _LL0;}else{_LL3: _tmp393=_tmp392;_LL4: {void*exn=_tmp393;(int)_rethrow(exn);}}_LL0:;}}}
# 1667
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1671
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1677
int _tmp397=0;int where_ctr=_tmp397;
int*_tmp398=& where_ctr;int*env=_tmp398;
struct Cyc_List_List*_tmp399=((struct Cyc_List_List*(*)(struct _tuple23*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,env,swcs);struct Cyc_List_List*match_rules=_tmp399;
void*_tmp39A=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp39A;
*dopt=dec_tree;{
# 1683
int _tmp39B=0;int ex_done=_tmp39B;
((void(*)(void*d,void(*not_exhaust)(unsigned,void*),unsigned env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1686
for(0;match_rules != 0;match_rules=match_rules->tl){
struct _tuple23*_tmp39C=(struct _tuple23*)match_rules->hd;struct _tuple23*_stmttmp32=_tmp39C;struct _tuple23*_tmp39D=_stmttmp32;unsigned _tmp39F;int _tmp39E;_LL1: _tmp39E=(_tmp39D->f2)->used;_tmp39F=(_tmp39D->f2)->pat_loc;_LL2: {int b=_tmp39E;unsigned loc2=_tmp39F;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp454;_tmp454.tag=0U,({struct _fat_ptr _tmp5ED=({const char*_tmp3A2="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp3A2,sizeof(char),74U);});_tmp454.f1=_tmp5ED;});_tmp454;});void*_tmp3A0[1U];_tmp3A0[0]=& _tmp3A1;({unsigned _tmp5EE=loc2;Cyc_Warn_err2(_tmp5EE,_tag_fat(_tmp3A0,sizeof(void*),1U));});});
# 1691
break;}}}}}
# 1700
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple15*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp3A3;_push_handler(& _tmp3A3);{int _tmp3A5=0;if(setjmp(_tmp3A3.handler))_tmp3A5=1;if(!_tmp3A5){
{struct _fat_ptr _tmp3A6=Cyc_Tcpat_desc2string(desc);struct _fat_ptr s=_tmp3A6;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp456;_tmp456.tag=0U,({struct _fat_ptr _tmp5EF=({const char*_tmp3AA="pattern not exhaustive.\n\tmissing case for ";_tag_fat(_tmp3AA,sizeof(char),43U);});_tmp456.f1=_tmp5EF;});_tmp456;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp455;_tmp455.tag=0U,_tmp455.f1=s;_tmp455;});void*_tmp3A7[2U];_tmp3A7[0]=& _tmp3A8,_tmp3A7[1]=& _tmp3A9;({unsigned _tmp5F0=(*pr).f1;Cyc_Warn_warn2(_tmp5F0,_tag_fat(_tmp3A7,sizeof(void*),2U));});});}
# 1703
;_pop_handler();}else{void*_tmp3A4=(void*)Cyc_Core_get_exn_thrown();void*_tmp3AB=_tmp3A4;void*_tmp3AC;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3AB)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1707
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp457;_tmp457.tag=0U,({struct _fat_ptr _tmp5F1=({const char*_tmp3AF="pattern not exhaustive.";_tag_fat(_tmp3AF,sizeof(char),24U);});_tmp457.f1=_tmp5F1;});_tmp457;});void*_tmp3AD[1U];_tmp3AD[0]=& _tmp3AE;({unsigned _tmp5F2=(*pr).f1;Cyc_Warn_warn2(_tmp5F2,_tag_fat(_tmp3AD,sizeof(void*),1U));});});
goto _LL0;}else{_LL3: _tmp3AC=_tmp3AB;_LL4: {void*exn=_tmp3AC;(int)_rethrow(exn);}}_LL0:;}}}}
# 1711
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1715
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->used=0,_tmp3B6->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_tmp5F3=Cyc_Absyn_skip_stmt(0U);_tmp3B6->rhs=_tmp5F3;});_tmp3B6;});
struct Cyc_List_List*_tmp3B0=({struct Cyc_List_List*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));({struct _tuple23*_tmp5F5=({struct _tuple23*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));({void*_tmp5F4=Cyc_Tcpat_compile_pat(p);_tmp3B4->f1=_tmp5F4;}),_tmp3B4->f2=rhs;_tmp3B4;});_tmp3B5->hd=_tmp5F5;}),_tmp3B5->tl=0;_tmp3B5;});struct Cyc_List_List*match_rules=_tmp3B0;
void*_tmp3B1=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp3B1;
struct _tuple15 _tmp3B2=({struct _tuple15 _tmp458;_tmp458.f1=loc,_tmp458.f2=1;_tmp458;});struct _tuple15 exhaust_env=_tmp3B2;
int _tmp3B3=0;int ex_done=_tmp3B3;
((void(*)(void*d,void(*not_exhaust)(struct _tuple15*,void*),struct _tuple15*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_warn,& exhaust_env,Cyc_Tcpat_dummy_fn,0,& ex_done);
# 1723
*dopt=dec_tree;
return exhaust_env.f2;}
# 1731
static struct _tuple23*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1735
if(swc->where_clause != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp459;_tmp459.tag=0U,({struct _fat_ptr _tmp5F6=({const char*_tmp3B9="&&-clauses not supported in exception handlers.";_tag_fat(_tmp3B9,sizeof(char),48U);});_tmp459.f1=_tmp5F6;});_tmp459;});void*_tmp3B7[1U];_tmp3B7[0]=& _tmp3B8;({unsigned _tmp5F7=((struct Cyc_Absyn_Exp*)_check_null(swc->where_clause))->loc;Cyc_Warn_err2(_tmp5F7,_tag_fat(_tmp3B7,sizeof(void*),1U));});});{
# 1738
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->used=0,_tmp3BB->pat_loc=(swc->pattern)->loc,_tmp3BB->rhs=swc->body;_tmp3BB;});
return({struct _tuple23*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->f1=sp0,_tmp3BA->f2=rhs;_tmp3BA;});}}
# 1741
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1744
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1748
struct Cyc_List_List*_tmp3BC=((struct Cyc_List_List*(*)(struct _tuple23*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);struct Cyc_List_List*match_rules=_tmp3BC;
void*_tmp3BD=Cyc_Tcpat_match_compile(match_rules);void*dec_tree=_tmp3BD;
*dopt=dec_tree;{
int _tmp3BE=0;int ex_done=_tmp3BE;
((void(*)(void*d,void(*not_exhaust)(unsigned,void*),unsigned env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1754
for(0;match_rules != 0;match_rules=match_rules->tl){
# 1756
if(match_rules->tl == 0)break;{
struct _tuple23*_tmp3BF=(struct _tuple23*)match_rules->hd;struct _tuple23*_stmttmp33=_tmp3BF;struct _tuple23*_tmp3C0=_stmttmp33;unsigned _tmp3C2;int _tmp3C1;_LL1: _tmp3C1=(_tmp3C0->f2)->used;_tmp3C2=(_tmp3C0->f2)->pat_loc;_LL2: {int b=_tmp3C1;unsigned loc2=_tmp3C2;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp45A;_tmp45A.tag=0U,({struct _fat_ptr _tmp5F8=({const char*_tmp3C5="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_tmp3C5,sizeof(char),74U);});_tmp45A.f1=_tmp5F8;});_tmp45A;});void*_tmp3C3[1U];_tmp3C3[0]=& _tmp3C4;({unsigned _tmp5F9=loc2;Cyc_Warn_err2(_tmp5F9,_tag_fat(_tmp3C3,sizeof(void*),1U));});});
break;}}}}}}
# 1765
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp3C6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;struct Cyc_List_List*l=_tmp3C6;for(0;l != 0;l=l->tl){
if((*((struct _tuple16*)l->hd)).f1 != 0)
return 1;}}
return 0;}

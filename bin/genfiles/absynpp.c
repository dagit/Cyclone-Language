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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Buffer_t;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
extern void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 258
extern int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
extern int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 513
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 531
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 699 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 886
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 896
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_compress(void*);
# 907
int Cyc_Absyn_type2bool(int def,void*);
# 916
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1152
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1160
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 207 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 71 "kinds.h"
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 73 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 77
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 50
extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
extern struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
extern struct Cyc_PP_Doc*Cyc_PP_nil_doc (void);
# 69
extern struct Cyc_PP_Doc*Cyc_PP_blank_doc (void);
# 72
extern struct Cyc_PP_Doc*Cyc_PP_line_doc (void);
# 78
extern struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr s);
# 80
extern struct Cyc_PP_Doc*Cyc_PP_textptr(struct _fat_ptr*p);
# 83
extern struct Cyc_PP_Doc*Cyc_PP_text_width(struct _fat_ptr s,int w);
# 91
extern struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
extern struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr);
# 108
extern struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr sep,struct Cyc_List_List*l);
# 112
extern struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l);
# 117
extern struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr sep,struct Cyc_List_List*l0);
# 120
extern struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l);
# 123
extern struct Cyc_PP_Doc*Cyc_PP_group(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*l);
# 129
extern struct Cyc_PP_Doc*Cyc_PP_egroup(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*l);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 52 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*);
# 62
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 78
int Cyc_Absynpp_is_anon_aggrtype(void*);
extern struct _fat_ptr Cyc_Absynpp_cyc_string;
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual,void*);struct _tuple14{int f1;struct Cyc_List_List*f2;};
# 95 "absynpp.h"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 35 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt,struct Cyc_List_List*varsinblock);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*requires,struct Cyc_Absyn_Exp*ensures);
# 56
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 63
static int Cyc_Absynpp_expand_typedefs;
# 67
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4U]="Cyc";
# 69
struct _fat_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4U};
struct _fat_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;static char _tmp1[14U]="__NoCycPrefix";
# 73
static struct _fat_ptr Cyc_Absynpp_nocyc_str={_tmp1,_tmp1,_tmp1 + 14U};
static struct _fat_ptr*Cyc_Absynpp_nocyc_strptr=& Cyc_Absynpp_nocyc_str;
# 77
static int Cyc_Absynpp_add_cyc_prefix;
# 81
static int Cyc_Absynpp_to_VC;
# 84
static int Cyc_Absynpp_decls_first;
# 88
static int Cyc_Absynpp_rewrite_temp_tvars;
# 91
static int Cyc_Absynpp_print_all_tvars;
# 94
static int Cyc_Absynpp_print_all_kinds;
# 97
static int Cyc_Absynpp_print_all_effects;
# 100
static int Cyc_Absynpp_print_using_stmts;
# 105
static int Cyc_Absynpp_print_externC_stmts;
# 109
static int Cyc_Absynpp_print_full_evars;
# 112
static int Cyc_Absynpp_generate_line_directives;
# 115
static int Cyc_Absynpp_use_curr_namespace;
# 118
static int Cyc_Absynpp_print_zeroterm;
# 121
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 124
int Cyc_Absynpp_print_for_cycdoc=0;
# 145
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 164
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 184
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 204
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 224
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 245
static void Cyc_Absynpp_curr_namespace_add(struct _fat_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp5D4=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5D4,({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=v,_tmp2->tl=0;_tmp2;}));});}
# 249
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 253
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 257
static void Cyc_Absynpp_curr_namespace_drop (void){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 261
struct _fat_ptr Cyc_Absynpp_char_escape(char c){
char _tmp3=c;switch(_tmp3){case 7U: _LL1: _LL2:
 return({const char*_tmp4="\\a";_tag_fat(_tmp4,sizeof(char),3U);});case 8U: _LL3: _LL4:
 return({const char*_tmp5="\\b";_tag_fat(_tmp5,sizeof(char),3U);});case 12U: _LL5: _LL6:
 return({const char*_tmp6="\\f";_tag_fat(_tmp6,sizeof(char),3U);});case 10U: _LL7: _LL8:
 return({const char*_tmp7="\\n";_tag_fat(_tmp7,sizeof(char),3U);});case 13U: _LL9: _LLA:
 return({const char*_tmp8="\\r";_tag_fat(_tmp8,sizeof(char),3U);});case 9U: _LLB: _LLC:
 return({const char*_tmp9="\\t";_tag_fat(_tmp9,sizeof(char),3U);});case 11U: _LLD: _LLE:
 return({const char*_tmpA="\\v";_tag_fat(_tmpA,sizeof(char),3U);});case 92U: _LLF: _LL10:
 return({const char*_tmpB="\\\\";_tag_fat(_tmpB,sizeof(char),3U);});case 34U: _LL11: _LL12:
 return({const char*_tmpC="\"";_tag_fat(_tmpC,sizeof(char),2U);});case 39U: _LL13: _LL14:
 return({const char*_tmpD="\\'";_tag_fat(_tmpD,sizeof(char),3U);});default: _LL15: _LL16:
# 274
 if((int)c >= (int)' ' &&(int)c <= (int)'~'){
struct _fat_ptr _tmpE=Cyc_Core_new_string(2U);struct _fat_ptr t=_tmpE;
({struct _fat_ptr _tmpF=_fat_ptr_plus(t,sizeof(char),0);char _tmp10=*((char*)_check_fat_subscript(_tmpF,sizeof(char),0U));char _tmp11=c;if(_get_fat_size(_tmpF,sizeof(char))== 1U &&(_tmp10 == 0 && _tmp11 != 0))_throw_arraybounds();*((char*)_tmpF.curr)=_tmp11;});
return(struct _fat_ptr)t;}else{
# 279
struct _fat_ptr _tmp12=Cyc_Core_new_string(5U);struct _fat_ptr t=_tmp12;
int j=0;
({struct _fat_ptr _tmp13=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp14=*((char*)_check_fat_subscript(_tmp13,sizeof(char),0U));char _tmp15='\\';if(_get_fat_size(_tmp13,sizeof(char))== 1U &&(_tmp14 == 0 && _tmp15 != 0))_throw_arraybounds();*((char*)_tmp13.curr)=_tmp15;});
({struct _fat_ptr _tmp16=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp17=*((char*)_check_fat_subscript(_tmp16,sizeof(char),0U));char _tmp18=(char)((int)'0' + ((int)((unsigned char)c)>> 6 & 3));if(_get_fat_size(_tmp16,sizeof(char))== 1U &&(_tmp17 == 0 && _tmp18 != 0))_throw_arraybounds();*((char*)_tmp16.curr)=_tmp18;});
({struct _fat_ptr _tmp19=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp1A=*((char*)_check_fat_subscript(_tmp19,sizeof(char),0U));char _tmp1B=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_fat_size(_tmp19,sizeof(char))== 1U &&(_tmp1A == 0 && _tmp1B != 0))_throw_arraybounds();*((char*)_tmp19.curr)=_tmp1B;});
({struct _fat_ptr _tmp1C=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp1D=*((char*)_check_fat_subscript(_tmp1C,sizeof(char),0U));char _tmp1E=(char)((int)'0' + ((int)c & 7));if(_get_fat_size(_tmp1C,sizeof(char))== 1U &&(_tmp1D == 0 && _tmp1E != 0))_throw_arraybounds();*((char*)_tmp1C.curr)=_tmp1E;});
return(struct _fat_ptr)t;}}_LL0:;}
# 290
static int Cyc_Absynpp_special(struct _fat_ptr s){
int sz=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((((int)c <= (int)' ' ||(int)c >= (int)'~')||(int)c == (int)'"')||(int)c == (int)'\\')
return 1;}}
# 297
return 0;}
# 300
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 303
int n=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
# 305
if(n > 0 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),n))== (int)'\000')-- n;{
# 307
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1F=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _stmttmp0=_tmp1F;char _tmp20=_stmttmp0;char _tmp21;switch(_tmp20){case 7U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL8;case 10U: _LL7: _LL8:
 goto _LLA;case 13U: _LL9: _LLA:
 goto _LLC;case 9U: _LLB: _LLC:
 goto _LLE;case 11U: _LLD: _LLE:
 goto _LL10;case 92U: _LLF: _LL10:
 goto _LL12;case 34U: _LL11: _LL12:
 len +=2;goto _LL0;default: _LL13: _tmp21=_tmp20;_LL14: {char c=_tmp21;
# 320
if((int)c >= (int)' ' &&(int)c <= (int)'~')++ len;else{
len +=4;}
goto _LL0;}}_LL0:;}}{
# 325
struct _fat_ptr t=Cyc_Core_new_string((unsigned)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp22=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _stmttmp1=_tmp22;char _tmp23=_stmttmp1;char _tmp24;switch(_tmp23){case 7U: _LL16: _LL17:
({struct _fat_ptr _tmp25=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp26=*((char*)_check_fat_subscript(_tmp25,sizeof(char),0U));char _tmp27='\\';if(_get_fat_size(_tmp25,sizeof(char))== 1U &&(_tmp26 == 0 && _tmp27 != 0))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({struct _fat_ptr _tmp28=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp29=*((char*)_check_fat_subscript(_tmp28,sizeof(char),0U));char _tmp2A='a';if(_get_fat_size(_tmp28,sizeof(char))== 1U &&(_tmp29 == 0 && _tmp2A != 0))_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL15;case 8U: _LL18: _LL19:
({struct _fat_ptr _tmp2B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp2C=*((char*)_check_fat_subscript(_tmp2B,sizeof(char),0U));char _tmp2D='\\';if(_get_fat_size(_tmp2B,sizeof(char))== 1U &&(_tmp2C == 0 && _tmp2D != 0))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({struct _fat_ptr _tmp2E=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp2F=*((char*)_check_fat_subscript(_tmp2E,sizeof(char),0U));char _tmp30='b';if(_get_fat_size(_tmp2E,sizeof(char))== 1U &&(_tmp2F == 0 && _tmp30 != 0))_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL15;case 12U: _LL1A: _LL1B:
({struct _fat_ptr _tmp31=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp32=*((char*)_check_fat_subscript(_tmp31,sizeof(char),0U));char _tmp33='\\';if(_get_fat_size(_tmp31,sizeof(char))== 1U &&(_tmp32 == 0 && _tmp33 != 0))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({struct _fat_ptr _tmp34=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp35=*((char*)_check_fat_subscript(_tmp34,sizeof(char),0U));char _tmp36='f';if(_get_fat_size(_tmp34,sizeof(char))== 1U &&(_tmp35 == 0 && _tmp36 != 0))_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL15;case 10U: _LL1C: _LL1D:
({struct _fat_ptr _tmp37=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp38=*((char*)_check_fat_subscript(_tmp37,sizeof(char),0U));char _tmp39='\\';if(_get_fat_size(_tmp37,sizeof(char))== 1U &&(_tmp38 == 0 && _tmp39 != 0))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({struct _fat_ptr _tmp3A=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp3B=*((char*)_check_fat_subscript(_tmp3A,sizeof(char),0U));char _tmp3C='n';if(_get_fat_size(_tmp3A,sizeof(char))== 1U &&(_tmp3B == 0 && _tmp3C != 0))_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL15;case 13U: _LL1E: _LL1F:
({struct _fat_ptr _tmp3D=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp3E=*((char*)_check_fat_subscript(_tmp3D,sizeof(char),0U));char _tmp3F='\\';if(_get_fat_size(_tmp3D,sizeof(char))== 1U &&(_tmp3E == 0 && _tmp3F != 0))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({struct _fat_ptr _tmp40=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp41=*((char*)_check_fat_subscript(_tmp40,sizeof(char),0U));char _tmp42='r';if(_get_fat_size(_tmp40,sizeof(char))== 1U &&(_tmp41 == 0 && _tmp42 != 0))_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL15;case 9U: _LL20: _LL21:
({struct _fat_ptr _tmp43=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp44=*((char*)_check_fat_subscript(_tmp43,sizeof(char),0U));char _tmp45='\\';if(_get_fat_size(_tmp43,sizeof(char))== 1U &&(_tmp44 == 0 && _tmp45 != 0))_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({struct _fat_ptr _tmp46=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp47=*((char*)_check_fat_subscript(_tmp46,sizeof(char),0U));char _tmp48='t';if(_get_fat_size(_tmp46,sizeof(char))== 1U &&(_tmp47 == 0 && _tmp48 != 0))_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL15;case 11U: _LL22: _LL23:
({struct _fat_ptr _tmp49=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp4A=*((char*)_check_fat_subscript(_tmp49,sizeof(char),0U));char _tmp4B='\\';if(_get_fat_size(_tmp49,sizeof(char))== 1U &&(_tmp4A == 0 && _tmp4B != 0))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({struct _fat_ptr _tmp4C=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp4D=*((char*)_check_fat_subscript(_tmp4C,sizeof(char),0U));char _tmp4E='v';if(_get_fat_size(_tmp4C,sizeof(char))== 1U &&(_tmp4D == 0 && _tmp4E != 0))_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL15;case 92U: _LL24: _LL25:
({struct _fat_ptr _tmp4F=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp50=*((char*)_check_fat_subscript(_tmp4F,sizeof(char),0U));char _tmp51='\\';if(_get_fat_size(_tmp4F,sizeof(char))== 1U &&(_tmp50 == 0 && _tmp51 != 0))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({struct _fat_ptr _tmp52=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp53=*((char*)_check_fat_subscript(_tmp52,sizeof(char),0U));char _tmp54='\\';if(_get_fat_size(_tmp52,sizeof(char))== 1U &&(_tmp53 == 0 && _tmp54 != 0))_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL15;case 34U: _LL26: _LL27:
({struct _fat_ptr _tmp55=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp56=*((char*)_check_fat_subscript(_tmp55,sizeof(char),0U));char _tmp57='\\';if(_get_fat_size(_tmp55,sizeof(char))== 1U &&(_tmp56 == 0 && _tmp57 != 0))_throw_arraybounds();*((char*)_tmp55.curr)=_tmp57;});({struct _fat_ptr _tmp58=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp59=*((char*)_check_fat_subscript(_tmp58,sizeof(char),0U));char _tmp5A='"';if(_get_fat_size(_tmp58,sizeof(char))== 1U &&(_tmp59 == 0 && _tmp5A != 0))_throw_arraybounds();*((char*)_tmp58.curr)=_tmp5A;});goto _LL15;default: _LL28: _tmp24=_tmp23;_LL29: {char c=_tmp24;
# 339
if((int)c >= (int)' ' &&(int)c <= (int)'~')({struct _fat_ptr _tmp5B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5C=*((char*)_check_fat_subscript(_tmp5B,sizeof(char),0U));char _tmp5D=c;if(_get_fat_size(_tmp5B,sizeof(char))== 1U &&(_tmp5C == 0 && _tmp5D != 0))_throw_arraybounds();*((char*)_tmp5B.curr)=_tmp5D;});else{
# 341
unsigned char uc=(unsigned char)c;
# 344
({struct _fat_ptr _tmp5E=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5F=*((char*)_check_fat_subscript(_tmp5E,sizeof(char),0U));char _tmp60='\\';if(_get_fat_size(_tmp5E,sizeof(char))== 1U &&(_tmp5F == 0 && _tmp60 != 0))_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});
({struct _fat_ptr _tmp61=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp62=*((char*)_check_fat_subscript(_tmp61,sizeof(char),0U));char _tmp63=(char)((int)'0' + ((int)uc >> 6 & 7));if(_get_fat_size(_tmp61,sizeof(char))== 1U &&(_tmp62 == 0 && _tmp63 != 0))_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});
({struct _fat_ptr _tmp64=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp65=*((char*)_check_fat_subscript(_tmp64,sizeof(char),0U));char _tmp66=(char)((int)'0' + ((int)uc >> 3 & 7));if(_get_fat_size(_tmp64,sizeof(char))== 1U &&(_tmp65 == 0 && _tmp66 != 0))_throw_arraybounds();*((char*)_tmp64.curr)=_tmp66;});
({struct _fat_ptr _tmp67=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp68=*((char*)_check_fat_subscript(_tmp67,sizeof(char),0U));char _tmp69=(char)((int)'0' + ((int)uc & 7));if(_get_fat_size(_tmp67,sizeof(char))== 1U &&(_tmp68 == 0 && _tmp69 != 0))_throw_arraybounds();*((char*)_tmp67.curr)=_tmp69;});}
# 349
goto _LL15;}}_LL15:;}}
# 351
return(struct _fat_ptr)t;}}}}static char _tmp6A[9U]="restrict";
# 354
static struct _fat_ptr Cyc_Absynpp_restrict_string={_tmp6A,_tmp6A,_tmp6A + 9U};static char _tmp6B[9U]="volatile";
static struct _fat_ptr Cyc_Absynpp_volatile_string={_tmp6B,_tmp6B,_tmp6B + 9U};static char _tmp6C[6U]="const";
static struct _fat_ptr Cyc_Absynpp_const_str={_tmp6C,_tmp6C,_tmp6C + 6U};
static struct _fat_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _fat_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _fat_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 361
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 364
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_restrict_sp,_tmp6D->tl=l;_tmp6D;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_volatile_sp,_tmp6E->tl=l;_tmp6E;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->hd=Cyc_Absynpp_const_sp,_tmp6F->tl=l;_tmp6F;});
return({struct _fat_ptr _tmp5D7=({const char*_tmp70="";_tag_fat(_tmp70,sizeof(char),1U);});struct _fat_ptr _tmp5D6=({const char*_tmp71=" ";_tag_fat(_tmp71,sizeof(char),2U);});struct _fat_ptr _tmp5D5=({const char*_tmp72=" ";_tag_fat(_tmp72,sizeof(char),2U);});Cyc_PP_egroup(_tmp5D7,_tmp5D6,_tmp5D5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 370
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){return Cyc_Kinds_kind2string(ka);}
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 373
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp73=Cyc_Kinds_compress_kb(c);void*_stmttmp2=_tmp73;void*_tmp74=_stmttmp2;struct Cyc_Absyn_Kind*_tmp75;struct Cyc_Absyn_Kind*_tmp76;switch(*((int*)_tmp74)){case 0U: _LL1: _tmp76=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp74)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp76;
return Cyc_Absynpp_kind2string(k);}case 1U: _LL3: _LL4:
# 377
 if(Cyc_PP_tex_output)
return({const char*_tmp77="{?}";_tag_fat(_tmp77,sizeof(char),4U);});else{
return({const char*_tmp78="?";_tag_fat(_tmp78,sizeof(char),2U);});}default: _LL5: _tmp75=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp74)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp75;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp5A8;_tmp5A8.tag=0U,({struct _fat_ptr _tmp5D8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k));_tmp5A8.f1=_tmp5D8;});_tmp5A8;});void*_tmp79[1U];_tmp79[0]=& _tmp7B;({struct _fat_ptr _tmp5D9=({const char*_tmp7A="<=%s";_tag_fat(_tmp7A,sizeof(char),5U);});Cyc_aprintf(_tmp5D9,_tag_fat(_tmp79,sizeof(void*),1U));});});}}_LL0:;}
# 384
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp7C=Cyc_Kinds_compress_kb(c);void*_stmttmp3=_tmp7C;void*_tmp7D=_stmttmp3;struct Cyc_Absyn_Kind*_tmp7E;struct Cyc_Absyn_Kind*_tmp7F;switch(*((int*)_tmp7D)){case 0U: _LL1: _tmp7F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7D)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp7F;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1U: _LL3: _LL4:
# 388
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp80="{?}";_tag_fat(_tmp80,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp81="?";_tag_fat(_tmp81,sizeof(char),2U);}));}default: _LL5: _tmp7E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7D)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp7E;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}}_LL0:;}
# 395
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp5DC=({const char*_tmp82="<";_tag_fat(_tmp82,sizeof(char),2U);});struct _fat_ptr _tmp5DB=({const char*_tmp83=">";_tag_fat(_tmp83,sizeof(char),2U);});struct _fat_ptr _tmp5DA=({const char*_tmp84=",";_tag_fat(_tmp84,sizeof(char),2U);});Cyc_PP_egroup(_tmp5DC,_tmp5DB,_tmp5DA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 399
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr _tmp85=*tv->name;struct _fat_ptr n=_tmp85;
# 403
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
# 405
struct _fat_ptr kstring=({const char*_tmp8E="K";_tag_fat(_tmp8E,sizeof(char),2U);});
{void*_tmp86=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp4=_tmp86;void*_tmp87=_stmttmp4;struct Cyc_Absyn_Kind*_tmp88;struct Cyc_Absyn_Kind*_tmp89;switch(*((int*)_tmp87)){case 2U: _LL1: _tmp89=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87)->f2;_LL2: {struct Cyc_Absyn_Kind*k=_tmp89;
_tmp88=k;goto _LL4;}case 0U: _LL3: _tmp88=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp87)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_tmp88;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 411
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8C=({struct Cyc_String_pa_PrintArg_struct _tmp5AA;_tmp5AA.tag=0U,_tmp5AA.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_tmp5AA;});struct Cyc_String_pa_PrintArg_struct _tmp8D=({struct Cyc_String_pa_PrintArg_struct _tmp5A9;_tmp5A9.tag=0U,({struct _fat_ptr _tmp5DD=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(n,sizeof(char),1));_tmp5A9.f1=_tmp5DD;});_tmp5A9;});void*_tmp8A[2U];_tmp8A[0]=& _tmp8C,_tmp8A[1]=& _tmp8D;({struct _fat_ptr _tmp5DE=({const char*_tmp8B="`G%s%s";_tag_fat(_tmp8B,sizeof(char),7U);});Cyc_aprintf(_tmp5DE,_tag_fat(_tmp8A,sizeof(void*),2U));});}));}
# 413
return Cyc_PP_text(n);}
# 416
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmp8F=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp5=_tmp8F;void*_tmp90=_stmttmp5;struct Cyc_Absyn_Kind*_tmp91;struct Cyc_Absyn_Kind*_tmp92;switch(*((int*)_tmp90)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp90)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp90)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmp92=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp90)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmp92;
# 421
return({struct Cyc_PP_Doc*_tmp93[3U];({struct Cyc_PP_Doc*_tmp5E1=Cyc_Absynpp_tvar2doc(tv);_tmp93[0]=_tmp5E1;}),({struct Cyc_PP_Doc*_tmp5E0=Cyc_PP_text(({const char*_tmp94="::";_tag_fat(_tmp94,sizeof(char),3U);}));_tmp93[1]=_tmp5E0;}),({struct Cyc_PP_Doc*_tmp5DF=Cyc_Absynpp_kind2doc(k);_tmp93[2]=_tmp5DF;});Cyc_PP_cat(_tag_fat(_tmp93,sizeof(struct Cyc_PP_Doc*),3U));});}}default: _LL5: _tmp91=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp91;
# 420
_tmp92=k;goto _LL8;}}_LL0:;}
# 425
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _tmp5E4=({const char*_tmp95="<";_tag_fat(_tmp95,sizeof(char),2U);});struct _fat_ptr _tmp5E3=({const char*_tmp96=">";_tag_fat(_tmp96,sizeof(char),2U);});struct _fat_ptr _tmp5E2=({const char*_tmp97=",";_tag_fat(_tmp97,sizeof(char),2U);});Cyc_PP_egroup(_tmp5E4,_tmp5E3,_tmp5E2,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 429
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _tmp5E7=({const char*_tmp98="<";_tag_fat(_tmp98,sizeof(char),2U);});struct _fat_ptr _tmp5E6=({const char*_tmp99=">";_tag_fat(_tmp99,sizeof(char),2U);});struct _fat_ptr _tmp5E5=({const char*_tmp9A=",";_tag_fat(_tmp9A,sizeof(char),2U);});Cyc_PP_egroup(_tmp5E7,_tmp5E6,_tmp5E5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 435
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple17*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 439
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp5EA=({const char*_tmp9B="(";_tag_fat(_tmp9B,sizeof(char),2U);});struct _fat_ptr _tmp5E9=({const char*_tmp9C=")";_tag_fat(_tmp9C,sizeof(char),2U);});struct _fat_ptr _tmp5E8=({const char*_tmp9D=",";_tag_fat(_tmp9D,sizeof(char),2U);});Cyc_PP_group(_tmp5EA,_tmp5E9,_tmp5E8,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 443
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmp9E=att;switch(*((int*)_tmp9E)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();default: _LL7: _LL8:
 return Cyc_PP_text(Cyc_Absynpp_attribute2string(att));}_LL0:;}
# 452
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp9F=(void*)atts->hd;void*_stmttmp6=_tmp9F;void*_tmpA0=_stmttmp6;switch(*((int*)_tmpA0)){case 1U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmpA1=" _stdcall ";_tag_fat(_tmpA1,sizeof(char),11U);}));case 2U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmpA2=" _cdecl ";_tag_fat(_tmpA2,sizeof(char),9U);}));case 3U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmpA3=" _fastcall ";_tag_fat(_tmpA3,sizeof(char),12U);}));default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 460
return Cyc_PP_nil_doc();}
# 463
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpA4=(void*)atts2->hd;void*_stmttmp7=_tmpA4;void*_tmpA5=_stmttmp7;switch(*((int*)_tmpA5)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 472
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpA6[3U];({struct Cyc_PP_Doc*_tmp5F0=Cyc_PP_text(({const char*_tmpA7=" __declspec(";_tag_fat(_tmpA7,sizeof(char),13U);}));_tmpA6[0]=_tmp5F0;}),({
struct Cyc_PP_Doc*_tmp5EF=({struct _fat_ptr _tmp5EE=({const char*_tmpA8="";_tag_fat(_tmpA8,sizeof(char),1U);});struct _fat_ptr _tmp5ED=({const char*_tmpA9="";_tag_fat(_tmpA9,sizeof(char),1U);});struct _fat_ptr _tmp5EC=({const char*_tmpAA=" ";_tag_fat(_tmpAA,sizeof(char),2U);});Cyc_PP_group(_tmp5EE,_tmp5ED,_tmp5EC,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpA6[1]=_tmp5EF;}),({
struct Cyc_PP_Doc*_tmp5EB=Cyc_PP_text(({const char*_tmpAB=")";_tag_fat(_tmpAB,sizeof(char),2U);}));_tmpA6[2]=_tmp5EB;});Cyc_PP_cat(_tag_fat(_tmpA6,sizeof(struct Cyc_PP_Doc*),3U));});}
# 479
struct _fat_ptr Cyc_Absynpp_attribute2string(void*a){
void*_tmpAC=a;struct _fat_ptr _tmpAD;struct _fat_ptr _tmpAE;int _tmpAF;int _tmpB0;int _tmpB1;int _tmpB3;int _tmpB2;int _tmpB5;int _tmpB4;struct _fat_ptr _tmpB6;struct Cyc_Absyn_Exp*_tmpB7;int _tmpB8;switch(*((int*)_tmpAC)){case 0U: _LL1: _tmpB8=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL2: {int i=_tmpB8;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpBB=({struct Cyc_Int_pa_PrintArg_struct _tmp5AB;_tmp5AB.tag=1U,_tmp5AB.f1=(unsigned long)i;_tmp5AB;});void*_tmpB9[1U];_tmpB9[0]=& _tmpBB;({struct _fat_ptr _tmp5F1=({const char*_tmpBA="regparm(%d)";_tag_fat(_tmpBA,sizeof(char),12U);});Cyc_aprintf(_tmp5F1,_tag_fat(_tmpB9,sizeof(void*),1U));});});}case 1U: _LL3: _LL4:
 return({const char*_tmpBC="stdcall";_tag_fat(_tmpBC,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmpBD="cdecl";_tag_fat(_tmpBD,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmpBE="fastcall";_tag_fat(_tmpBE,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmpBF="noreturn";_tag_fat(_tmpBF,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmpC0="const";_tag_fat(_tmpC0,sizeof(char),6U);});case 6U: _LLD: _tmpB7=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmpB7;
# 488
if(e == 0)return({const char*_tmpC1="aligned";_tag_fat(_tmpC1,sizeof(char),8U);});else{
# 490
enum Cyc_Flags_C_Compilers _tmpC2=Cyc_Flags_c_compiler;switch(_tmpC2){case Cyc_Flags_Gcc_c: _LL3C: _LL3D:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC5=({struct Cyc_String_pa_PrintArg_struct _tmp5AC;_tmp5AC.tag=0U,({struct _fat_ptr _tmp5F2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5AC.f1=_tmp5F2;});_tmp5AC;});void*_tmpC3[1U];_tmpC3[0]=& _tmpC5;({struct _fat_ptr _tmp5F3=({const char*_tmpC4="aligned(%s)";_tag_fat(_tmpC4,sizeof(char),12U);});Cyc_aprintf(_tmp5F3,_tag_fat(_tmpC3,sizeof(void*),1U));});});case Cyc_Flags_Vc_c: _LL3E: _LL3F:
 goto _LL41;default: _LL40: _LL41:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC8=({struct Cyc_String_pa_PrintArg_struct _tmp5AD;_tmp5AD.tag=0U,({struct _fat_ptr _tmp5F4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5AD.f1=_tmp5F4;});_tmp5AD;});void*_tmpC6[1U];_tmpC6[0]=& _tmpC8;({struct _fat_ptr _tmp5F5=({const char*_tmpC7="align(%s)";_tag_fat(_tmpC7,sizeof(char),10U);});Cyc_aprintf(_tmp5F5,_tag_fat(_tmpC6,sizeof(void*),1U));});});}_LL3B:;}}case 7U: _LLF: _LL10:
# 495
 return({const char*_tmpC9="packed";_tag_fat(_tmpC9,sizeof(char),7U);});case 8U: _LL11: _tmpB6=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL12: {struct _fat_ptr s=_tmpB6;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpCC=({struct Cyc_String_pa_PrintArg_struct _tmp5AE;_tmp5AE.tag=0U,_tmp5AE.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5AE;});void*_tmpCA[1U];_tmpCA[0]=& _tmpCC;({struct _fat_ptr _tmp5F6=({const char*_tmpCB="section(\"%s\")";_tag_fat(_tmpCB,sizeof(char),14U);});Cyc_aprintf(_tmp5F6,_tag_fat(_tmpCA,sizeof(void*),1U));});});}case 9U: _LL13: _LL14:
 return({const char*_tmpCD="nocommon";_tag_fat(_tmpCD,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmpCE="shared";_tag_fat(_tmpCE,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmpCF="unused";_tag_fat(_tmpCF,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmpD0="weak";_tag_fat(_tmpD0,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmpD1="dllimport";_tag_fat(_tmpD1,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmpD2="dllexport";_tag_fat(_tmpD2,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmpD3="no_instrument_function";_tag_fat(_tmpD3,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmpD4="constructor";_tag_fat(_tmpD4,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmpD5="destructor";_tag_fat(_tmpD5,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmpD6="no_check_memory_usage";_tag_fat(_tmpD6,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmpB4=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f2;_tmpB5=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f3;_LL28: {int n=_tmpB4;int m=_tmpB5;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD9=({struct Cyc_Int_pa_PrintArg_struct _tmp5B0;_tmp5B0.tag=1U,_tmp5B0.f1=(unsigned)n;_tmp5B0;});struct Cyc_Int_pa_PrintArg_struct _tmpDA=({struct Cyc_Int_pa_PrintArg_struct _tmp5AF;_tmp5AF.tag=1U,_tmp5AF.f1=(unsigned)m;_tmp5AF;});void*_tmpD7[2U];_tmpD7[0]=& _tmpD9,_tmpD7[1]=& _tmpDA;({struct _fat_ptr _tmp5F7=({const char*_tmpD8="format(printf,%u,%u)";_tag_fat(_tmpD8,sizeof(char),21U);});Cyc_aprintf(_tmp5F7,_tag_fat(_tmpD7,sizeof(void*),2U));});});}}else{_LL29: _tmpB2=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f2;_tmpB3=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAC)->f3;_LL2A: {int n=_tmpB2;int m=_tmpB3;
# 509
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDD=({struct Cyc_Int_pa_PrintArg_struct _tmp5B2;_tmp5B2.tag=1U,_tmp5B2.f1=(unsigned)n;_tmp5B2;});struct Cyc_Int_pa_PrintArg_struct _tmpDE=({struct Cyc_Int_pa_PrintArg_struct _tmp5B1;_tmp5B1.tag=1U,_tmp5B1.f1=(unsigned)m;_tmp5B1;});void*_tmpDB[2U];_tmpDB[0]=& _tmpDD,_tmpDB[1]=& _tmpDE;({struct _fat_ptr _tmp5F8=({const char*_tmpDC="format(scanf,%u,%u)";_tag_fat(_tmpDC,sizeof(char),20U);});Cyc_aprintf(_tmp5F8,_tag_fat(_tmpDB,sizeof(void*),2U));});});}}case 20U: _LL2B: _tmpB1=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL2C: {int n=_tmpB1;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE1=({struct Cyc_Int_pa_PrintArg_struct _tmp5B3;_tmp5B3.tag=1U,_tmp5B3.f1=(unsigned long)n;_tmp5B3;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _fat_ptr _tmp5F9=({const char*_tmpE0="initializes(%d)";_tag_fat(_tmpE0,sizeof(char),16U);});Cyc_aprintf(_tmp5F9,_tag_fat(_tmpDF,sizeof(void*),1U));});});}case 21U: _LL2D: _tmpB0=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL2E: {int n=_tmpB0;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE4=({struct Cyc_Int_pa_PrintArg_struct _tmp5B4;_tmp5B4.tag=1U,_tmp5B4.f1=(unsigned long)n;_tmp5B4;});void*_tmpE2[1U];_tmpE2[0]=& _tmpE4;({struct _fat_ptr _tmp5FA=({const char*_tmpE3="noliveunique(%d)";_tag_fat(_tmpE3,sizeof(char),17U);});Cyc_aprintf(_tmp5FA,_tag_fat(_tmpE2,sizeof(void*),1U));});});}case 22U: _LL2F: _tmpAF=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL30: {int n=_tmpAF;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE7=({struct Cyc_Int_pa_PrintArg_struct _tmp5B5;_tmp5B5.tag=1U,_tmp5B5.f1=(unsigned long)n;_tmp5B5;});void*_tmpE5[1U];_tmpE5[0]=& _tmpE7;({struct _fat_ptr _tmp5FB=({const char*_tmpE6="consume(%d)";_tag_fat(_tmpE6,sizeof(char),12U);});Cyc_aprintf(_tmp5FB,_tag_fat(_tmpE5,sizeof(void*),1U));});});}case 23U: _LL31: _LL32:
 return({const char*_tmpE8="pure";_tag_fat(_tmpE8,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmpE9="always_inline";_tag_fat(_tmpE9,sizeof(char),14U);});case 24U: _LL35: _tmpAE=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL36: {struct _fat_ptr s=_tmpAE;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpEC=({struct Cyc_String_pa_PrintArg_struct _tmp5B6;_tmp5B6.tag=0U,_tmp5B6.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5B6;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEC;({struct _fat_ptr _tmp5FC=({const char*_tmpEB="__mode__(\"%s\")";_tag_fat(_tmpEB,sizeof(char),15U);});Cyc_aprintf(_tmp5FC,_tag_fat(_tmpEA,sizeof(void*),1U));});});}case 25U: _LL37: _tmpAD=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpAC)->f1;_LL38: {struct _fat_ptr s=_tmpAD;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpEF=({struct Cyc_String_pa_PrintArg_struct _tmp5B7;_tmp5B7.tag=0U,_tmp5B7.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5B7;});void*_tmpED[1U];_tmpED[0]=& _tmpEF;({struct _fat_ptr _tmp5FD=({const char*_tmpEE="alias(\"%s\")";_tag_fat(_tmpEE,sizeof(char),12U);});Cyc_aprintf(_tmp5FD,_tag_fat(_tmpED,sizeof(void*),1U));});});}default: _LL39: _LL3A:
 return({const char*_tmpF0="no_throw";_tag_fat(_tmpF0,sizeof(char),9U);});}_LL0:;}
# 521
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absynpp_attribute2string(a));}
# 525
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Flags_C_Compilers _tmpF1=Cyc_Flags_c_compiler;if(_tmpF1 == Cyc_Flags_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 530
 return({struct Cyc_PP_Doc*_tmpF2[2U];({struct Cyc_PP_Doc*_tmp602=Cyc_PP_text(({const char*_tmpF3=" __attribute__";_tag_fat(_tmpF3,sizeof(char),15U);}));_tmpF2[0]=_tmp602;}),({
struct Cyc_PP_Doc*_tmp601=({struct _fat_ptr _tmp600=({const char*_tmpF4="((";_tag_fat(_tmpF4,sizeof(char),3U);});struct _fat_ptr _tmp5FF=({const char*_tmpF5="))";_tag_fat(_tmpF5,sizeof(char),3U);});struct _fat_ptr _tmp5FE=({const char*_tmpF6=",";_tag_fat(_tmpF6,sizeof(char),2U);});Cyc_PP_group(_tmp600,_tmp5FF,_tmp5FE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpF2[1]=_tmp601;});Cyc_PP_cat(_tag_fat(_tmpF2,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}}
# 535
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpF7=(void*)tms->hd;void*_stmttmp8=_tmpF7;void*_tmpF8=_stmttmp8;switch(*((int*)_tmpF8)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 540
enum Cyc_Flags_C_Compilers _tmpF9=Cyc_Flags_c_compiler;if(_tmpF9 == Cyc_Flags_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 544
 return 0;}_LL0:;}}
# 548
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 551
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question (void){
if(!((unsigned)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmpFA="{?}";_tag_fat(_tmpFA,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmpFB="?";_tag_fat(_tmpFB,sizeof(char),2U);}));}}
# 558
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 560
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb (void){
if(!((unsigned)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmpFC="{\\lb}";_tag_fat(_tmpFC,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmpFD="{";_tag_fat(_tmpFD,sizeof(char),2U);}));}}
# 567
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 569
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb (void){
if(!((unsigned)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmpFE="{\\rb}";_tag_fat(_tmpFE,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmpFF="}";_tag_fat(_tmpFF,sizeof(char),2U);}));}}
# 576
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 578
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar (void){
if(!((unsigned)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmp100="\\$";_tag_fat(_tmp100,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmp101="$";_tag_fat(_tmp101,sizeof(char),2U);}));}}
# 585
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 587
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmp102[3U];({struct Cyc_PP_Doc*_tmp605=Cyc_Absynpp_lb();_tmp102[0]=_tmp605;}),({struct Cyc_PP_Doc*_tmp604=Cyc_PP_seq(sep,ss);_tmp102[1]=_tmp604;}),({struct Cyc_PP_Doc*_tmp603=Cyc_Absynpp_rb();_tmp102[2]=_tmp603;});Cyc_PP_cat(_tag_fat(_tmp102,sizeof(struct Cyc_PP_Doc*),3U));});}
# 592
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmp103=(void*)tms->hd;void*_stmttmp9=_tmp103;void*_tmp104=_stmttmp9;struct Cyc_List_List*_tmp105;switch(*((int*)_tmp104)){case 0U: _LL1: _LL2:
({void*_tmp106=0U;({struct Cyc___cycFILE*_tmp607=Cyc_stderr;struct _fat_ptr _tmp606=({const char*_tmp107="Carray_mod ";_tag_fat(_tmp107,sizeof(char),12U);});Cyc_fprintf(_tmp607,_tmp606,_tag_fat(_tmp106,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp108=0U;({struct Cyc___cycFILE*_tmp609=Cyc_stderr;struct _fat_ptr _tmp608=({const char*_tmp109="ConstArray_mod ";_tag_fat(_tmp109,sizeof(char),16U);});Cyc_fprintf(_tmp609,_tmp608,_tag_fat(_tmp108,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp104)->f1)->tag == 1U){_LL5: _tmp105=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp104)->f1)->f1;_LL6: {struct Cyc_List_List*ys=_tmp105;
# 598
({void*_tmp10A=0U;({struct Cyc___cycFILE*_tmp60B=Cyc_stderr;struct _fat_ptr _tmp60A=({const char*_tmp10B="Function_mod(";_tag_fat(_tmp10B,sizeof(char),14U);});Cyc_fprintf(_tmp60B,_tmp60A,_tag_fat(_tmp10A,sizeof(void*),0U));});});
for(0;ys != 0;ys=ys->tl){
struct _fat_ptr*_tmp10C=(*((struct _tuple8*)ys->hd)).f1;struct _fat_ptr*v=_tmp10C;
if(v == 0)({void*_tmp10D=0U;({struct Cyc___cycFILE*_tmp60D=Cyc_stderr;struct _fat_ptr _tmp60C=({const char*_tmp10E="?";_tag_fat(_tmp10E,sizeof(char),2U);});Cyc_fprintf(_tmp60D,_tmp60C,_tag_fat(_tmp10D,sizeof(void*),0U));});});else{
({void*_tmp10F=0U;({struct Cyc___cycFILE*_tmp60F=Cyc_stderr;struct _fat_ptr _tmp60E=*v;Cyc_fprintf(_tmp60F,_tmp60E,_tag_fat(_tmp10F,sizeof(void*),0U));});});}
if(ys->tl != 0)({void*_tmp110=0U;({struct Cyc___cycFILE*_tmp611=Cyc_stderr;struct _fat_ptr _tmp610=({const char*_tmp111=",";_tag_fat(_tmp111,sizeof(char),2U);});Cyc_fprintf(_tmp611,_tmp610,_tag_fat(_tmp110,sizeof(void*),0U));});});}
# 605
({void*_tmp112=0U;({struct Cyc___cycFILE*_tmp613=Cyc_stderr;struct _fat_ptr _tmp612=({const char*_tmp113=") ";_tag_fat(_tmp113,sizeof(char),3U);});Cyc_fprintf(_tmp613,_tmp612,_tag_fat(_tmp112,sizeof(void*),0U));});});
goto _LL0;}}else{_LL7: _LL8:
# 608
({void*_tmp114=0U;({struct Cyc___cycFILE*_tmp615=Cyc_stderr;struct _fat_ptr _tmp614=({const char*_tmp115="Function_mod()";_tag_fat(_tmp115,sizeof(char),15U);});Cyc_fprintf(_tmp615,_tmp614,_tag_fat(_tmp114,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmp116=0U;({struct Cyc___cycFILE*_tmp617=Cyc_stderr;struct _fat_ptr _tmp616=({const char*_tmp117="Attributes_mod ";_tag_fat(_tmp117,sizeof(char),16U);});Cyc_fprintf(_tmp617,_tmp616,_tag_fat(_tmp116,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmp118=0U;({struct Cyc___cycFILE*_tmp619=Cyc_stderr;struct _fat_ptr _tmp618=({const char*_tmp119="TypeParams_mod ";_tag_fat(_tmp119,sizeof(char),16U);});Cyc_fprintf(_tmp619,_tmp618,_tag_fat(_tmp118,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp11A=0U;({struct Cyc___cycFILE*_tmp61B=Cyc_stderr;struct _fat_ptr _tmp61A=({const char*_tmp11B="Pointer_mod ";_tag_fat(_tmp11B,sizeof(char),13U);});Cyc_fprintf(_tmp61B,_tmp61A,_tag_fat(_tmp11A,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 613
({void*_tmp11C=0U;({struct Cyc___cycFILE*_tmp61D=Cyc_stderr;struct _fat_ptr _tmp61C=({const char*_tmp11D="\n";_tag_fat(_tmp11D,sizeof(char),2U);});Cyc_fprintf(_tmp61D,_tmp61C,_tag_fat(_tmp11C,sizeof(void*),0U));});});}
# 616
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 618
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp162[3U];({struct Cyc_PP_Doc*_tmp61F=Cyc_PP_text(({const char*_tmp163="(";_tag_fat(_tmp163,sizeof(char),2U);}));_tmp162[0]=_tmp61F;}),_tmp162[1]=rest,({struct Cyc_PP_Doc*_tmp61E=Cyc_PP_text(({const char*_tmp164=")";_tag_fat(_tmp164,sizeof(char),2U);}));_tmp162[2]=_tmp61E;});Cyc_PP_cat(_tag_fat(_tmp162,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmp11E=(void*)tms->hd;void*_stmttmpA=_tmp11E;void*_tmp11F=_stmttmpA;struct Cyc_Absyn_Tqual _tmp124;void*_tmp123;void*_tmp122;void*_tmp121;void*_tmp120;int _tmp127;unsigned _tmp126;struct Cyc_List_List*_tmp125;struct Cyc_List_List*_tmp128;void*_tmp129;void*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;void*_tmp12C;switch(*((int*)_tmp11F)){case 0U: _LL1: _tmp12C=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp11F)->f1;_LL2: {void*zeroterm=_tmp12C;
# 624
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp12D[2U];_tmp12D[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_tmp621=Cyc_PP_text(({const char*_tmp12E="[]@zeroterm";_tag_fat(_tmp12E,sizeof(char),12U);}));_tmp12D[1]=_tmp621;}):({struct Cyc_PP_Doc*_tmp620=Cyc_PP_text(({const char*_tmp12F="[]";_tag_fat(_tmp12F,sizeof(char),3U);}));_tmp12D[1]=_tmp620;});Cyc_PP_cat(_tag_fat(_tmp12D,sizeof(struct Cyc_PP_Doc*),2U));});}case 1U: _LL3: _tmp12A=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp11F)->f1;_tmp12B=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp11F)->f2;_LL4: {struct Cyc_Absyn_Exp*e=_tmp12A;void*zeroterm=_tmp12B;
# 628
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp130[4U];_tmp130[0]=rest,({struct Cyc_PP_Doc*_tmp625=Cyc_PP_text(({const char*_tmp131="[";_tag_fat(_tmp131,sizeof(char),2U);}));_tmp130[1]=_tmp625;}),({struct Cyc_PP_Doc*_tmp624=Cyc_Absynpp_exp2doc(e);_tmp130[2]=_tmp624;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_tmp623=Cyc_PP_text(({const char*_tmp132="]@zeroterm";_tag_fat(_tmp132,sizeof(char),11U);}));_tmp130[3]=_tmp623;}):({struct Cyc_PP_Doc*_tmp622=Cyc_PP_text(({const char*_tmp133="]";_tag_fat(_tmp133,sizeof(char),2U);}));_tmp130[3]=_tmp622;});Cyc_PP_cat(_tag_fat(_tmp130,sizeof(struct Cyc_PP_Doc*),4U));});}case 3U: _LL5: _tmp129=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp11F)->f1;_LL6: {void*args=_tmp129;
# 632
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp134=args;unsigned _tmp136;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_List_List*_tmp13B;void*_tmp13A;struct Cyc_Absyn_VarargInfo*_tmp139;int _tmp138;struct Cyc_List_List*_tmp137;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp134)->tag == 1U){_LLE: _tmp137=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp134)->f1;_tmp138=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp134)->f2;_tmp139=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp134)->f3;_tmp13A=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp134)->f4;_tmp13B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp134)->f5;_tmp13C=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp134)->f6;_tmp13D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp134)->f7;_LLF: {struct Cyc_List_List*args2=_tmp137;int c_varargs=_tmp138;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp139;void*effopt=_tmp13A;struct Cyc_List_List*rgn_po=_tmp13B;struct Cyc_Absyn_Exp*req=_tmp13C;struct Cyc_Absyn_Exp*ens=_tmp13D;
# 635
return({struct Cyc_PP_Doc*_tmp13E[2U];_tmp13E[0]=rest,({struct Cyc_PP_Doc*_tmp626=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);_tmp13E[1]=_tmp626;});Cyc_PP_cat(_tag_fat(_tmp13E,sizeof(struct Cyc_PP_Doc*),2U));});}}else{_LL10: _tmp135=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp134)->f1;_tmp136=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp134)->f2;_LL11: {struct Cyc_List_List*sl=_tmp135;unsigned loc=_tmp136;
# 637
return({struct Cyc_PP_Doc*_tmp13F[2U];_tmp13F[0]=rest,({struct Cyc_PP_Doc*_tmp62A=({struct _fat_ptr _tmp629=({const char*_tmp140="(";_tag_fat(_tmp140,sizeof(char),2U);});struct _fat_ptr _tmp628=({const char*_tmp141=")";_tag_fat(_tmp141,sizeof(char),2U);});struct _fat_ptr _tmp627=({const char*_tmp142=",";_tag_fat(_tmp142,sizeof(char),2U);});Cyc_PP_group(_tmp629,_tmp628,_tmp627,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,sl));});_tmp13F[1]=_tmp62A;});Cyc_PP_cat(_tag_fat(_tmp13F,sizeof(struct Cyc_PP_Doc*),2U));});}}_LLD:;}}case 5U: _LL7: _tmp128=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp11F)->f2;_LL8: {struct Cyc_List_List*atts=_tmp128;
# 640
enum Cyc_Flags_C_Compilers _tmp143=Cyc_Flags_c_compiler;if(_tmp143 == Cyc_Flags_Gcc_c){_LL13: _LL14:
# 642
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp144[2U];_tmp144[0]=rest,({struct Cyc_PP_Doc*_tmp62B=Cyc_Absynpp_atts2doc(atts);_tmp144[1]=_tmp62B;});Cyc_PP_cat(_tag_fat(_tmp144,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 646
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp145[2U];({struct Cyc_PP_Doc*_tmp62C=Cyc_Absynpp_callconv2doc(atts);_tmp145[0]=_tmp62C;}),_tmp145[1]=rest;Cyc_PP_cat(_tag_fat(_tmp145,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp125=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11F)->f1;_tmp126=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11F)->f2;_tmp127=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11F)->f3;_LLA: {struct Cyc_List_List*ts=_tmp125;unsigned loc=_tmp126;int print_kinds=_tmp127;
# 651
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_tmp146[2U];_tmp146[0]=rest,({struct Cyc_PP_Doc*_tmp62D=Cyc_Absynpp_ktvars2doc(ts);_tmp146[1]=_tmp62D;});Cyc_PP_cat(_tag_fat(_tmp146,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 655
return({struct Cyc_PP_Doc*_tmp147[2U];_tmp147[0]=rest,({struct Cyc_PP_Doc*_tmp62E=Cyc_Absynpp_tvars2doc(ts);_tmp147[1]=_tmp62E;});Cyc_PP_cat(_tag_fat(_tmp147,sizeof(struct Cyc_PP_Doc*),2U));});}}default: _LLB: _tmp120=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F)->f1).rgn;_tmp121=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F)->f1).nullable;_tmp122=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F)->f1).bounds;_tmp123=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F)->f1).zero_term;_tmp124=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F)->f2;_LLC: {void*rgn=_tmp120;void*nullable=_tmp121;void*bd=_tmp122;void*zt=_tmp123;struct Cyc_Absyn_Tqual tq2=_tmp124;
# 659
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_tmp148=Cyc_Absyn_compress(bd);void*_stmttmpB=_tmp148;void*_tmp149=_stmttmpB;void*_tmp14A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f2 != 0){_LL1A: _tmp14A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f2)->hd;_LL1B: {void*targ=_tmp14A;
# 667
{void*_tmp14B=Cyc_Absyn_compress(targ);void*_stmttmpC=_tmp14B;void*_tmp14C=_stmttmpC;struct Cyc_Absyn_Exp*_tmp14D;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp14C)->tag == 9U){_LL1F: _tmp14D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp14C)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp14D;
# 669
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp14E="*";_tag_fat(_tmp14E,sizeof(char),2U);}):({const char*_tmp14F="@";_tag_fat(_tmp14F,sizeof(char),2U);}));{
struct _tuple12 _tmp150=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmpD=_tmp150;struct _tuple12 _tmp151=_stmttmpD;int _tmp153;unsigned _tmp152;_LL24: _tmp152=_tmp151.f1;_tmp153=_tmp151.f2;_LL25: {unsigned val=_tmp152;int known=_tmp153;
if(!known || val != (unsigned)1)
ptr=({struct Cyc_PP_Doc*_tmp154[4U];_tmp154[0]=ptr,({struct Cyc_PP_Doc*_tmp631=Cyc_Absynpp_lb();_tmp154[1]=_tmp631;}),({struct Cyc_PP_Doc*_tmp630=Cyc_Absynpp_exp2doc(e);_tmp154[2]=_tmp630;}),({struct Cyc_PP_Doc*_tmp62F=Cyc_Absynpp_rb();_tmp154[3]=_tmp62F;});Cyc_PP_cat(_tag_fat(_tmp154,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}}}}else{_LL21: _LL22:
# 675
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp155="*";_tag_fat(_tmp155,sizeof(char),2U);}):({const char*_tmp156="@";_tag_fat(_tmp156,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp157[4U];_tmp157[0]=ptr,({struct Cyc_PP_Doc*_tmp634=Cyc_Absynpp_lb();_tmp157[1]=_tmp634;}),({struct Cyc_PP_Doc*_tmp633=Cyc_Absynpp_typ2doc(targ);_tmp157[2]=_tmp633;}),({struct Cyc_PP_Doc*_tmp632=Cyc_Absynpp_rb();_tmp157[3]=_tmp632;});Cyc_PP_cat(_tag_fat(_tmp157,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}_LL1E:;}
# 679
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 681
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp158="*";_tag_fat(_tmp158,sizeof(char),2U);}):({const char*_tmp159="@";_tag_fat(_tmp159,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp15A[4U];_tmp15A[0]=ptr,({struct Cyc_PP_Doc*_tmp637=Cyc_Absynpp_lb();_tmp15A[1]=_tmp637;}),({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_typ2doc(bd);_tmp15A[2]=_tmp636;}),({struct Cyc_PP_Doc*_tmp635=Cyc_Absynpp_rb();_tmp15A[3]=_tmp635;});Cyc_PP_cat(_tag_fat(_tmp15A,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 686
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr && Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp15B="@zeroterm";_tag_fat(_tmp15B,sizeof(char),10U);}));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp15C="@nozeroterm";_tag_fat(_tmp15C,sizeof(char),12U);}));}}
# 692
{void*_tmp15D=Cyc_Absyn_compress(rgn);void*_stmttmpE=_tmp15D;void*_tmp15E=_stmttmpE;switch(*((int*)_tmp15E)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15E)->f1)->tag == 5U){_LL27: _LL28:
 goto _LL26;}else{goto _LL2B;}case 1U: _LL29: if(Cyc_Absynpp_print_for_cycdoc){_LL2A:
 goto _LL26;}else{goto _LL2B;}default: _LL2B: _LL2C:
 rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL26:;}{
# 697
struct Cyc_PP_Doc*spacer1=tqd != mt &&(ztd != mt || rgd != mt)?Cyc_PP_text(({const char*_tmp161=" ";_tag_fat(_tmp161,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp160=" ";_tag_fat(_tmp160,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp15F[7U];_tmp15F[0]=ptr,_tmp15F[1]=ztd,_tmp15F[2]=rgd,_tmp15F[3]=spacer1,_tmp15F[4]=tqd,_tmp15F[5]=spacer2,_tmp15F[6]=rest;Cyc_PP_cat(_tag_fat(_tmp15F,sizeof(struct Cyc_PP_Doc*),7U));});}}}_LL0:;}}
# 703
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp165=Cyc_Absyn_compress(t);void*_stmttmpF=_tmp165;void*_tmp166=_stmttmpF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp166)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp166)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp167="`H";_tag_fat(_tmp167,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp168="`U";_tag_fat(_tmp168,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp169="`RC";_tag_fat(_tmp169,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 713
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 717
void*_tmp16A=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp16A;void*_tmp16B=_stmttmp10;struct Cyc_List_List*_tmp16C;void*_tmp16D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2 != 0){_LL1: _tmp16D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2)->hd;_LL2: {void*r=_tmp16D;
({struct Cyc_List_List*_tmp639=({struct Cyc_List_List*_tmp16E=_cycalloc(sizeof(*_tmp16E));({struct Cyc_PP_Doc*_tmp638=Cyc_Absynpp_rgn2doc(r);_tmp16E->hd=_tmp638;}),_tmp16E->tl=*rgions;_tmp16E;});*rgions=_tmp639;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2;_LL4: {struct Cyc_List_List*ts=_tmp16C;
# 720
for(0;ts != 0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
# 723
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp63B=({struct Cyc_List_List*_tmp16F=_cycalloc(sizeof(*_tmp16F));({struct Cyc_PP_Doc*_tmp63A=Cyc_Absynpp_typ2doc(t);_tmp16F->hd=_tmp63A;}),_tmp16F->tl=*effects;_tmp16F;});*effects=_tmp63B;});goto _LL0;}_LL0:;}
# 728
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0 && effects != 0)
return({struct _fat_ptr _tmp63E=({const char*_tmp170="";_tag_fat(_tmp170,sizeof(char),1U);});struct _fat_ptr _tmp63D=({const char*_tmp171="";_tag_fat(_tmp171,sizeof(char),1U);});struct _fat_ptr _tmp63C=({const char*_tmp172="+";_tag_fat(_tmp172,sizeof(char),2U);});Cyc_PP_group(_tmp63E,_tmp63D,_tmp63C,effects);});else{
# 736
struct Cyc_PP_Doc*_tmp173=({struct _fat_ptr _tmp63F=({const char*_tmp178=",";_tag_fat(_tmp178,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp63F,rgions);});struct Cyc_PP_Doc*doc1=_tmp173;
return({struct _fat_ptr _tmp643=({const char*_tmp174="";_tag_fat(_tmp174,sizeof(char),1U);});struct _fat_ptr _tmp642=({const char*_tmp175="";_tag_fat(_tmp175,sizeof(char),1U);});struct _fat_ptr _tmp641=({const char*_tmp176="+";_tag_fat(_tmp176,sizeof(char),2U);});Cyc_PP_group(_tmp643,_tmp642,_tmp641,({struct Cyc_List_List*_tmp640=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp640,({struct Cyc_List_List*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->hd=doc1,_tmp177->tl=0;_tmp177;}));}));});}}
# 741
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp179=k;if(_tmp179 == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp17A="struct ";_tag_fat(_tmp17A,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp17B="union ";_tag_fat(_tmp17B,sizeof(char),7U);}));}_LL0:;}
# 749
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp17C=t;struct Cyc_Absyn_Datatypedecl*_tmp17D;struct Cyc_Absyn_Enumdecl*_tmp17E;struct Cyc_Absyn_Aggrdecl*_tmp17F;struct Cyc_Absyn_Typedefdecl*_tmp182;struct Cyc_List_List*_tmp181;struct _tuple0*_tmp180;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_List_List*_tmp186;enum Cyc_Absyn_AggrKind _tmp185;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Tvar*_tmp188;struct Cyc_Core_Opt*_tmp18C;int _tmp18B;void*_tmp18A;struct Cyc_Core_Opt*_tmp189;void*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18F;struct Cyc_List_List*_tmp190;struct _fat_ptr _tmp191;struct _tuple0*_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_List_List*_tmp195;union Cyc_Absyn_AggrInfo _tmp194;int _tmp196;enum Cyc_Absyn_Size_of _tmp198;enum Cyc_Absyn_Sign _tmp197;struct Cyc_List_List*_tmp19A;union Cyc_Absyn_DatatypeFieldInfo _tmp199;struct Cyc_List_List*_tmp19C;union Cyc_Absyn_DatatypeInfo _tmp19B;switch(*((int*)_tmp17C)){case 4U: _LL1: _LL2:
# 753
 return Cyc_PP_text(({const char*_tmp19D="[[[array]]]";_tag_fat(_tmp19D,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)){case 0U: _LL7: _LL8:
# 757
 s=Cyc_PP_text(({const char*_tmp19E="void";_tag_fat(_tmp19E,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp19B=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f1;_tmp19C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f2;_LLE: {union Cyc_Absyn_DatatypeInfo tu_info=_tmp19B;struct Cyc_List_List*ts=_tmp19C;
# 773
{union Cyc_Absyn_DatatypeInfo _tmp1A6=tu_info;int _tmp1A8;struct _tuple0*_tmp1A7;int _tmp1AA;struct _tuple0*_tmp1A9;if((_tmp1A6.UnknownDatatype).tag == 1){_LL48: _tmp1A9=((_tmp1A6.UnknownDatatype).val).name;_tmp1AA=((_tmp1A6.UnknownDatatype).val).is_extensible;_LL49: {struct _tuple0*n=_tmp1A9;int is_x=_tmp1AA;
_tmp1A7=n;_tmp1A8=is_x;goto _LL4B;}}else{_LL4A: _tmp1A7=(*(_tmp1A6.KnownDatatype).val)->name;_tmp1A8=(*(_tmp1A6.KnownDatatype).val)->is_extensible;_LL4B: {struct _tuple0*n=_tmp1A7;int is_x=_tmp1A8;
# 776
struct Cyc_PP_Doc*_tmp1AB=Cyc_PP_text(({const char*_tmp1AF="datatype ";_tag_fat(_tmp1AF,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp1AB;
struct Cyc_PP_Doc*_tmp1AC=is_x?Cyc_PP_text(({const char*_tmp1AE="@extensible ";_tag_fat(_tmp1AE,sizeof(char),13U);})): Cyc_PP_nil_doc();struct Cyc_PP_Doc*ext=_tmp1AC;
s=({struct Cyc_PP_Doc*_tmp1AD[4U];_tmp1AD[0]=ext,_tmp1AD[1]=kw,({struct Cyc_PP_Doc*_tmp645=Cyc_Absynpp_qvar2doc(n);_tmp1AD[2]=_tmp645;}),({struct Cyc_PP_Doc*_tmp644=Cyc_Absynpp_tps2doc(ts);_tmp1AD[3]=_tmp644;});Cyc_PP_cat(_tag_fat(_tmp1AD,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 781
goto _LL0;}case 19U: _LLF: _tmp199=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f1;_tmp19A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f2;_LL10: {union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp199;struct Cyc_List_List*ts=_tmp19A;
# 783
{union Cyc_Absyn_DatatypeFieldInfo _tmp1B0=tuf_info;struct _tuple0*_tmp1B3;int _tmp1B2;struct _tuple0*_tmp1B1;int _tmp1B6;struct _tuple0*_tmp1B5;struct _tuple0*_tmp1B4;if((_tmp1B0.UnknownDatatypefield).tag == 1){_LL4D: _tmp1B4=((_tmp1B0.UnknownDatatypefield).val).datatype_name;_tmp1B5=((_tmp1B0.UnknownDatatypefield).val).field_name;_tmp1B6=((_tmp1B0.UnknownDatatypefield).val).is_extensible;_LL4E: {struct _tuple0*tname=_tmp1B4;struct _tuple0*fname=_tmp1B5;int is_x=_tmp1B6;
# 785
_tmp1B1=tname;_tmp1B2=is_x;_tmp1B3=fname;goto _LL50;}}else{_LL4F: _tmp1B1=(((_tmp1B0.KnownDatatypefield).val).f1)->name;_tmp1B2=(((_tmp1B0.KnownDatatypefield).val).f1)->is_extensible;_tmp1B3=(((_tmp1B0.KnownDatatypefield).val).f2)->name;_LL50: {struct _tuple0*tname=_tmp1B1;int is_x=_tmp1B2;struct _tuple0*fname=_tmp1B3;
# 788
struct Cyc_PP_Doc*_tmp1B7=Cyc_PP_text(is_x?({const char*_tmp1BA="@extensible datatype ";_tag_fat(_tmp1BA,sizeof(char),22U);}):({const char*_tmp1BB="datatype ";_tag_fat(_tmp1BB,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp1B7;
s=({struct Cyc_PP_Doc*_tmp1B8[4U];_tmp1B8[0]=kw,({struct Cyc_PP_Doc*_tmp648=Cyc_Absynpp_qvar2doc(tname);_tmp1B8[1]=_tmp648;}),({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp1B9=".";_tag_fat(_tmp1B9,sizeof(char),2U);}));_tmp1B8[2]=_tmp647;}),({struct Cyc_PP_Doc*_tmp646=Cyc_Absynpp_qvar2doc(fname);_tmp1B8[3]=_tmp646;});Cyc_PP_cat(_tag_fat(_tmp1B8,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 792
goto _LL0;}case 1U: _LL11: _tmp197=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f1;_tmp198=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f2;_LL12: {enum Cyc_Absyn_Sign sn=_tmp197;enum Cyc_Absyn_Size_of sz=_tmp198;
# 794
struct _fat_ptr sns;
struct _fat_ptr ts;
{enum Cyc_Absyn_Sign _tmp1BC=sn;switch(_tmp1BC){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp1BD="";_tag_fat(_tmp1BD,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp1BE="unsigned ";_tag_fat(_tmp1BE,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 801
{enum Cyc_Absyn_Size_of _tmp1BF=sz;switch(_tmp1BF){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 803
{enum Cyc_Absyn_Sign _tmp1C0=sn;switch(_tmp1C0){case Cyc_Absyn_None: _LL66: _LL67:
 sns=({const char*_tmp1C1="";_tag_fat(_tmp1C1,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed: _LL68: _LL69:
 sns=({const char*_tmp1C2="signed ";_tag_fat(_tmp1C2,sizeof(char),8U);});goto _LL65;default: _LL6A: _LL6B:
 sns=({const char*_tmp1C3="unsigned ";_tag_fat(_tmp1C3,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 808
ts=({const char*_tmp1C4="char";_tag_fat(_tmp1C4,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp1C5="short";_tag_fat(_tmp1C5,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp1C6="int";_tag_fat(_tmp1C6,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp1C7="long";_tag_fat(_tmp1C7,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
# 815
{enum Cyc_Flags_C_Compilers _tmp1C8=Cyc_Flags_c_compiler;if(_tmp1C8 == Cyc_Flags_Gcc_c){_LL6D: _LL6E:
 ts=({const char*_tmp1C9="long long";_tag_fat(_tmp1C9,sizeof(char),10U);});goto _LL6C;}else{_LL6F: _LL70:
 ts=({const char*_tmp1CA="__int64";_tag_fat(_tmp1CA,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 819
goto _LL58;}_LL58:;}
# 821
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1CD=({struct Cyc_String_pa_PrintArg_struct _tmp5B9;_tmp5B9.tag=0U,_tmp5B9.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_tmp5B9;});struct Cyc_String_pa_PrintArg_struct _tmp1CE=({struct Cyc_String_pa_PrintArg_struct _tmp5B8;_tmp5B8.tag=0U,_tmp5B8.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_tmp5B8;});void*_tmp1CB[2U];_tmp1CB[0]=& _tmp1CD,_tmp1CB[1]=& _tmp1CE;({struct _fat_ptr _tmp649=({const char*_tmp1CC="%s%s";_tag_fat(_tmp1CC,sizeof(char),5U);});Cyc_aprintf(_tmp649,_tag_fat(_tmp1CB,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp196=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f1;_LL14: {int i=_tmp196;
# 824
{int _tmp1CF=i;switch(_tmp1CF){case 0U: _LL72: _LL73:
 s=Cyc_PP_text(({const char*_tmp1D0="float";_tag_fat(_tmp1D0,sizeof(char),6U);}));goto _LL71;case 1U: _LL74: _LL75:
 s=Cyc_PP_text(({const char*_tmp1D1="double";_tag_fat(_tmp1D1,sizeof(char),7U);}));goto _LL71;default: _LL76: _LL77:
 s=Cyc_PP_text(({const char*_tmp1D2="long double";_tag_fat(_tmp1D2,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 829
goto _LL0;}case 20U: _LL17: _tmp194=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f1;_tmp195=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f2;_LL18: {union Cyc_Absyn_AggrInfo info=_tmp194;struct Cyc_List_List*ts=_tmp195;
# 832
struct _tuple11 _tmp1D4=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp11=_tmp1D4;struct _tuple11 _tmp1D5=_stmttmp11;struct _tuple0*_tmp1D7;enum Cyc_Absyn_AggrKind _tmp1D6;_LL79: _tmp1D6=_tmp1D5.f1;_tmp1D7=_tmp1D5.f2;_LL7A: {enum Cyc_Absyn_AggrKind k=_tmp1D6;struct _tuple0*n=_tmp1D7;
s=({struct Cyc_PP_Doc*_tmp1D8[3U];({struct Cyc_PP_Doc*_tmp64C=Cyc_Absynpp_aggr_kind2doc(k);_tmp1D8[0]=_tmp64C;}),({struct Cyc_PP_Doc*_tmp64B=Cyc_Absynpp_qvar2doc(n);_tmp1D8[1]=_tmp64B;}),({struct Cyc_PP_Doc*_tmp64A=Cyc_Absynpp_tps2doc(ts);_tmp1D8[2]=_tmp64A;});Cyc_PP_cat(_tag_fat(_tmp1D8,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 16U: _LL1B: _tmp193=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f1;_LL1C: {struct Cyc_List_List*fs=_tmp193;
# 841
s=({struct Cyc_PP_Doc*_tmp1DA[4U];({struct Cyc_PP_Doc*_tmp650=Cyc_PP_text(({const char*_tmp1DB="enum ";_tag_fat(_tmp1DB,sizeof(char),6U);}));_tmp1DA[0]=_tmp650;}),({struct Cyc_PP_Doc*_tmp64F=Cyc_Absynpp_lb();_tmp1DA[1]=_tmp64F;}),({struct Cyc_PP_Doc*_tmp64E=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_tmp1DA[2]=_tmp64E;}),({struct Cyc_PP_Doc*_tmp64D=Cyc_Absynpp_rb();_tmp1DA[3]=_tmp64D;});Cyc_PP_cat(_tag_fat(_tmp1DA,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1D: _tmp192=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f1;_LL1E: {struct _tuple0*n=_tmp192;
s=({struct Cyc_PP_Doc*_tmp1DC[2U];({struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp1DD="enum ";_tag_fat(_tmp1DD,sizeof(char),6U);}));_tmp1DC[0]=_tmp652;}),({struct Cyc_PP_Doc*_tmp651=Cyc_Absynpp_qvar2doc(n);_tmp1DC[1]=_tmp651;});Cyc_PP_cat(_tag_fat(_tmp1DC,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 17U: _LL23: _tmp191=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)->f1;_LL24: {struct _fat_ptr t=_tmp191;
# 845
s=Cyc_PP_text(t);goto _LL0;}case 3U: _LL27: _tmp190=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f2;_LL28: {struct Cyc_List_List*ts=_tmp190;
# 855
s=({struct Cyc_PP_Doc*_tmp1E5[3U];({struct Cyc_PP_Doc*_tmp655=Cyc_PP_text(({const char*_tmp1E6="region_t<";_tag_fat(_tmp1E6,sizeof(char),10U);}));_tmp1E5[0]=_tmp655;}),({struct Cyc_PP_Doc*_tmp654=Cyc_Absynpp_rgn2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1E5[1]=_tmp654;}),({struct Cyc_PP_Doc*_tmp653=Cyc_PP_text(({const char*_tmp1E7=">";_tag_fat(_tmp1E7,sizeof(char),2U);}));_tmp1E5[2]=_tmp653;});Cyc_PP_cat(_tag_fat(_tmp1E5,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 4U: _LL29: _tmp18F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f2;_LL2A: {struct Cyc_List_List*ts=_tmp18F;
# 857
s=({struct Cyc_PP_Doc*_tmp1E8[3U];({struct Cyc_PP_Doc*_tmp658=Cyc_PP_text(({const char*_tmp1E9="tag_t<";_tag_fat(_tmp1E9,sizeof(char),7U);}));_tmp1E8[0]=_tmp658;}),({struct Cyc_PP_Doc*_tmp657=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1E8[1]=_tmp657;}),({struct Cyc_PP_Doc*_tmp656=Cyc_PP_text(({const char*_tmp1EA=">";_tag_fat(_tmp1EA,sizeof(char),2U);}));_tmp1E8[2]=_tmp656;});Cyc_PP_cat(_tag_fat(_tmp1E8,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: _LL31: _tmp18E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f2;_LL32: {struct Cyc_List_List*ts=_tmp18E;
# 862
s=({struct Cyc_PP_Doc*_tmp1EB[3U];({struct Cyc_PP_Doc*_tmp65B=Cyc_PP_text(({const char*_tmp1EC="regions(";_tag_fat(_tmp1EC,sizeof(char),9U);}));_tmp1EB[0]=_tmp65B;}),({struct Cyc_PP_Doc*_tmp65A=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1EB[1]=_tmp65A;}),({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp1ED=")";_tag_fat(_tmp1ED,sizeof(char),2U);}));_tmp1EB[2]=_tmp659;});Cyc_PP_cat(_tag_fat(_tmp1EB,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL33: _LL34:
 goto _LL36;case 9U: _LL35: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 868
 s=Cyc_PP_text(({const char*_tmp1EE="@true";_tag_fat(_tmp1EE,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
 s=Cyc_PP_text(({const char*_tmp1EF="@false";_tag_fat(_tmp1EF,sizeof(char),7U);}));goto _LL0;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f2 != 0){_LL41: _tmp18D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f2)->hd;_LL42: {void*t=_tmp18D;
# 871
s=({struct Cyc_PP_Doc*_tmp1F0[4U];({struct Cyc_PP_Doc*_tmp65F=Cyc_PP_text(({const char*_tmp1F1="@thin @numelts";_tag_fat(_tmp1F1,sizeof(char),15U);}));_tmp1F0[0]=_tmp65F;}),({struct Cyc_PP_Doc*_tmp65E=Cyc_Absynpp_lb();_tmp1F0[1]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_typ2doc(t);_tmp1F0[2]=_tmp65D;}),({struct Cyc_PP_Doc*_tmp65C=Cyc_Absynpp_rb();_tmp1F0[3]=_tmp65C;});Cyc_PP_cat(_tag_fat(_tmp1F0,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}}else{_LL43: _LL44:
# 873
 s=Cyc_PP_text(({const char*_tmp1F2="@thin";_tag_fat(_tmp1F2,sizeof(char),6U);}));goto _LL0;}default: _LL45: _LL46:
 s=Cyc_PP_text(({const char*_tmp1F3="@fat";_tag_fat(_tmp1F3,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp189=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17C)->f1;_tmp18A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17C)->f2;_tmp18B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17C)->f3;_tmp18C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17C)->f4;_LLA: {struct Cyc_Core_Opt*k=_tmp189;void*topt=_tmp18A;int i=_tmp18B;struct Cyc_Core_Opt*tvs=_tmp18C;
# 759
if(topt != 0)
# 761
return Cyc_Absynpp_ntyp2doc(topt);else{
# 763
struct _fat_ptr kindstring=k == 0?({const char*_tmp1A3="K";_tag_fat(_tmp1A3,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A1=({struct Cyc_String_pa_PrintArg_struct _tmp5BB;_tmp5BB.tag=0U,_tmp5BB.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_tmp5BB;});struct Cyc_Int_pa_PrintArg_struct _tmp1A2=({struct Cyc_Int_pa_PrintArg_struct _tmp5BA;_tmp5BA.tag=1U,_tmp5BA.f1=(unsigned long)i;_tmp5BA;});void*_tmp19F[2U];_tmp19F[0]=& _tmp1A1,_tmp19F[1]=& _tmp1A2;({struct _fat_ptr _tmp660=({const char*_tmp1A0="`E%s%d";_tag_fat(_tmp1A0,sizeof(char),7U);});Cyc_aprintf(_tmp660,_tag_fat(_tmp19F,sizeof(void*),2U));});}));}
# 766
goto _LL0;}case 2U: _LLB: _tmp188=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp17C)->f1;_LLC: {struct Cyc_Absyn_Tvar*tv=_tmp188;
# 768
s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp1A4[3U];_tmp1A4[0]=s,({struct Cyc_PP_Doc*_tmp662=Cyc_PP_text(({const char*_tmp1A5="::";_tag_fat(_tmp1A5,sizeof(char),3U);}));_tmp1A4[1]=_tmp662;}),({struct Cyc_PP_Doc*_tmp661=Cyc_Absynpp_kindbound2doc(tv->kind);_tmp1A4[2]=_tmp661;});Cyc_PP_cat(_tag_fat(_tmp1A4,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 6U: _LL15: _tmp187=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp17C)->f1;_LL16: {struct Cyc_List_List*ts=_tmp187;
# 830
s=({struct Cyc_PP_Doc*_tmp1D3[2U];({struct Cyc_PP_Doc*_tmp664=Cyc_Absynpp_dollar();_tmp1D3[0]=_tmp664;}),({struct Cyc_PP_Doc*_tmp663=Cyc_Absynpp_args2doc(ts);_tmp1D3[1]=_tmp663;});Cyc_PP_cat(_tag_fat(_tmp1D3,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 7U: _LL19: _tmp185=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp17C)->f1;_tmp186=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp17C)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp185;struct Cyc_List_List*fs=_tmp186;
# 836
s=({struct Cyc_PP_Doc*_tmp1D9[4U];({struct Cyc_PP_Doc*_tmp668=Cyc_Absynpp_aggr_kind2doc(k);_tmp1D9[0]=_tmp668;}),({struct Cyc_PP_Doc*_tmp667=Cyc_Absynpp_lb();_tmp1D9[1]=_tmp667;}),({
struct Cyc_PP_Doc*_tmp666=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_tmp1D9[2]=_tmp666;}),({
struct Cyc_PP_Doc*_tmp665=Cyc_Absynpp_rb();_tmp1D9[3]=_tmp665;});Cyc_PP_cat(_tag_fat(_tmp1D9,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 9U: _LL1F: _tmp184=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp17C)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp184;
# 843
s=({struct Cyc_PP_Doc*_tmp1DE[3U];({struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp1DF="valueof_t(";_tag_fat(_tmp1DF,sizeof(char),11U);}));_tmp1DE[0]=_tmp66B;}),({struct Cyc_PP_Doc*_tmp66A=Cyc_Absynpp_exp2doc(e);_tmp1DE[1]=_tmp66A;}),({struct Cyc_PP_Doc*_tmp669=Cyc_PP_text(({const char*_tmp1E0=")";_tag_fat(_tmp1E0,sizeof(char),2U);}));_tmp1DE[2]=_tmp669;});Cyc_PP_cat(_tag_fat(_tmp1DE,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 11U: _LL21: _tmp183=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp17C)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp183;
s=({struct Cyc_PP_Doc*_tmp1E1[3U];({struct Cyc_PP_Doc*_tmp66E=Cyc_PP_text(({const char*_tmp1E2="typeof(";_tag_fat(_tmp1E2,sizeof(char),8U);}));_tmp1E1[0]=_tmp66E;}),({struct Cyc_PP_Doc*_tmp66D=Cyc_Absynpp_exp2doc(e);_tmp1E1[1]=_tmp66D;}),({struct Cyc_PP_Doc*_tmp66C=Cyc_PP_text(({const char*_tmp1E3=")";_tag_fat(_tmp1E3,sizeof(char),2U);}));_tmp1E1[2]=_tmp66C;});Cyc_PP_cat(_tag_fat(_tmp1E1,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL25: _tmp180=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17C)->f1;_tmp181=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17C)->f2;_tmp182=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17C)->f3;_LL26: {struct _tuple0*n=_tmp180;struct Cyc_List_List*ts=_tmp181;struct Cyc_Absyn_Typedefdecl*kopt=_tmp182;
# 852
s=({struct Cyc_PP_Doc*_tmp1E4[2U];({struct Cyc_PP_Doc*_tmp670=Cyc_Absynpp_qvar2doc(n);_tmp1E4[0]=_tmp670;}),({struct Cyc_PP_Doc*_tmp66F=Cyc_Absynpp_tps2doc(ts);_tmp1E4[1]=_tmp66F;});Cyc_PP_cat(_tag_fat(_tmp1E4,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp17C)->f1)->r)){case 0U: _LL37: _tmp17F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp17C)->f1)->r)->f1;_LL38: {struct Cyc_Absyn_Aggrdecl*d=_tmp17F;
# 865
s=Cyc_Absynpp_aggrdecl2doc(d);goto _LL0;}case 1U: _LL39: _tmp17E=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp17C)->f1)->r)->f1;_LL3A: {struct Cyc_Absyn_Enumdecl*d=_tmp17E;
s=Cyc_Absynpp_enumdecl2doc(d);goto _LL0;}default: _LL3B: _tmp17D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp17C)->f1)->r)->f1;_LL3C: {struct Cyc_Absyn_Datatypedecl*d=_tmp17D;
s=Cyc_Absynpp_datatypedecl2doc(d);goto _LL0;}}}_LL0:;}
# 876
return s;}
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _fat_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple18{void*f1;void*f2;};
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple18*cmp){
struct _tuple18*_tmp1F4=cmp;void*_tmp1F6;void*_tmp1F5;_LL1: _tmp1F5=_tmp1F4->f1;_tmp1F6=_tmp1F4->f2;_LL2: {void*r1=_tmp1F5;void*r2=_tmp1F6;
return({struct Cyc_PP_Doc*_tmp1F7[3U];({struct Cyc_PP_Doc*_tmp673=Cyc_Absynpp_rgn2doc(r1);_tmp1F7[0]=_tmp673;}),({struct Cyc_PP_Doc*_tmp672=Cyc_PP_text(({const char*_tmp1F8=" > ";_tag_fat(_tmp1F8,sizeof(char),4U);}));_tmp1F7[1]=_tmp672;}),({struct Cyc_PP_Doc*_tmp671=Cyc_Absynpp_rgn2doc(r2);_tmp1F7[2]=_tmp671;});Cyc_PP_cat(_tag_fat(_tmp1F7,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 888
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _tmp676=({const char*_tmp1F9="";_tag_fat(_tmp1F9,sizeof(char),1U);});struct _fat_ptr _tmp675=({const char*_tmp1FA="";_tag_fat(_tmp1FA,sizeof(char),1U);});struct _fat_ptr _tmp674=({const char*_tmp1FB=",";_tag_fat(_tmp1FB,sizeof(char),2U);});Cyc_PP_group(_tmp676,_tmp675,_tmp674,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 892
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*_tmp1FC=(*t).f1;struct _fat_ptr*vo=_tmp1FC;
struct Cyc_Core_Opt*dopt=vo == 0?0:({struct Cyc_Core_Opt*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));({struct Cyc_PP_Doc*_tmp677=Cyc_PP_text(*vo);_tmp1FD->v=_tmp677;});_tmp1FD;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 898
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 902
struct Cyc_List_List*_tmp1FE=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_List_List*arg_docs=_tmp1FE;
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_tmp679=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp679,({struct Cyc_List_List*_tmp200=_cycalloc(sizeof(*_tmp200));({struct Cyc_PP_Doc*_tmp678=Cyc_PP_text(({const char*_tmp1FF="...";_tag_fat(_tmp1FF,sizeof(char),4U);}));_tmp200->hd=_tmp678;}),_tmp200->tl=0;_tmp200;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp201=({struct Cyc_PP_Doc*_tmp203[3U];({struct Cyc_PP_Doc*_tmp67D=Cyc_PP_text(({const char*_tmp204="...";_tag_fat(_tmp204,sizeof(char),4U);}));_tmp203[0]=_tmp67D;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp67C=Cyc_PP_text(({const char*_tmp205=" inject ";_tag_fat(_tmp205,sizeof(char),9U);}));_tmp203[1]=_tmp67C;}):({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(({const char*_tmp206=" ";_tag_fat(_tmp206,sizeof(char),2U);}));_tmp203[1]=_tmp67B;}),({
struct Cyc_PP_Doc*_tmp67A=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->f1=cyc_varargs->name,_tmp207->f2=cyc_varargs->tq,_tmp207->f3=cyc_varargs->type;_tmp207;}));_tmp203[2]=_tmp67A;});Cyc_PP_cat(_tag_fat(_tmp203,sizeof(struct Cyc_PP_Doc*),3U));});
# 907
struct Cyc_PP_Doc*varargs_doc=_tmp201;
# 911
arg_docs=({struct Cyc_List_List*_tmp67E=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp67E,({struct Cyc_List_List*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->hd=varargs_doc,_tmp202->tl=0;_tmp202;}));});}}{
# 913
struct Cyc_PP_Doc*_tmp208=({struct _fat_ptr _tmp681=({const char*_tmp217="";_tag_fat(_tmp217,sizeof(char),1U);});struct _fat_ptr _tmp680=({const char*_tmp218="";_tag_fat(_tmp218,sizeof(char),1U);});struct _fat_ptr _tmp67F=({const char*_tmp219=",";_tag_fat(_tmp219,sizeof(char),2U);});Cyc_PP_group(_tmp681,_tmp680,_tmp67F,arg_docs);});struct Cyc_PP_Doc*arg_doc=_tmp208;
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_tmp209[3U];_tmp209[0]=arg_doc,({struct Cyc_PP_Doc*_tmp683=Cyc_PP_text(({const char*_tmp20A=";";_tag_fat(_tmp20A,sizeof(char),2U);}));_tmp209[1]=_tmp683;}),({struct Cyc_PP_Doc*_tmp682=Cyc_Absynpp_eff2doc(effopt);_tmp209[2]=_tmp682;});Cyc_PP_cat(_tag_fat(_tmp209,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
arg_doc=({struct Cyc_PP_Doc*_tmp20B[3U];_tmp20B[0]=arg_doc,({struct Cyc_PP_Doc*_tmp685=Cyc_PP_text(({const char*_tmp20C=":";_tag_fat(_tmp20C,sizeof(char),2U);}));_tmp20B[1]=_tmp685;}),({struct Cyc_PP_Doc*_tmp684=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp20B[2]=_tmp684;});Cyc_PP_cat(_tag_fat(_tmp20B,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp20D=({struct Cyc_PP_Doc*_tmp214[3U];({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp215="(";_tag_fat(_tmp215,sizeof(char),2U);}));_tmp214[0]=_tmp687;}),_tmp214[1]=arg_doc,({struct Cyc_PP_Doc*_tmp686=Cyc_PP_text(({const char*_tmp216=")";_tag_fat(_tmp216,sizeof(char),2U);}));_tmp214[2]=_tmp686;});Cyc_PP_cat(_tag_fat(_tmp214,sizeof(struct Cyc_PP_Doc*),3U));});struct Cyc_PP_Doc*res=_tmp20D;
if(req != 0)
res=({struct Cyc_PP_Doc*_tmp20E[4U];_tmp20E[0]=res,({struct Cyc_PP_Doc*_tmp68A=Cyc_PP_text(({const char*_tmp20F=" @requires(";_tag_fat(_tmp20F,sizeof(char),12U);}));_tmp20E[1]=_tmp68A;}),({struct Cyc_PP_Doc*_tmp689=Cyc_Absynpp_exp2doc(req);_tmp20E[2]=_tmp689;}),({struct Cyc_PP_Doc*_tmp688=Cyc_PP_text(({const char*_tmp210=")";_tag_fat(_tmp210,sizeof(char),2U);}));_tmp20E[3]=_tmp688;});Cyc_PP_cat(_tag_fat(_tmp20E,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
res=({struct Cyc_PP_Doc*_tmp211[4U];_tmp211[0]=res,({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp212=" @ensures(";_tag_fat(_tmp212,sizeof(char),11U);}));_tmp211[1]=_tmp68D;}),({struct Cyc_PP_Doc*_tmp68C=Cyc_Absynpp_exp2doc(ens);_tmp211[2]=_tmp68C;}),({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp213=")";_tag_fat(_tmp213,sizeof(char),2U);}));_tmp211[3]=_tmp68B;});Cyc_PP_cat(_tag_fat(_tmp211,sizeof(struct Cyc_PP_Doc*),4U));});
return res;}}}
# 926
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _fat_ptr*v){return Cyc_PP_text(*v);}
# 928
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp21A=0;struct Cyc_List_List*prefix=_tmp21A;
int match;
{union Cyc_Absyn_Nmspace _tmp21B=(*q).f1;union Cyc_Absyn_Nmspace _stmttmp12=_tmp21B;union Cyc_Absyn_Nmspace _tmp21C=_stmttmp12;struct Cyc_List_List*_tmp21D;struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmp21F;switch((_tmp21C.C_n).tag){case 4U: _LL1: _LL2:
 _tmp21F=0;goto _LL4;case 1U: _LL3: _tmp21F=(_tmp21C.Rel_n).val;_LL4: {struct Cyc_List_List*x=_tmp21F;
# 934
match=0;
if(((unsigned)x && Cyc_Absynpp_qvar_to_Cids)&& !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 938
prefix=x;}
goto _LL0;}case 3U: _LL5: _tmp21E=(_tmp21C.C_n).val;_LL6: {struct Cyc_List_List*x=_tmp21E;
# 941
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
# 943
goto _LL0;}default: _LL7: _tmp21D=(_tmp21C.Abs_n).val;_LL8: {struct Cyc_List_List*x=_tmp21D;
# 945
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
if(Cyc_Absynpp_qvar_to_Cids){
if((unsigned)x && !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 950
prefix=Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220->hd=Cyc_Absynpp_cyc_stringptr,_tmp220->tl=x;_tmp220;}): x;}}
# 952
goto _LL0;}}_LL0:;}
# 954
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _fat_ptr)({struct Cyc_List_List*_tmp68F=({struct Cyc_List_List*_tmp68E=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp68E,({struct Cyc_List_List*_tmp221=_cycalloc(sizeof(*_tmp221));
_tmp221->hd=(*q).f2,_tmp221->tl=0;_tmp221;}));});
# 955
Cyc_str_sepstr(_tmp68F,({const char*_tmp222="_";_tag_fat(_tmp222,sizeof(char),2U);}));});else{
# 959
if(match)
return*(*q).f2;else{
# 962
return(struct _fat_ptr)({struct Cyc_List_List*_tmp691=({struct Cyc_List_List*_tmp690=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp690,({struct Cyc_List_List*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->hd=(*q).f2,_tmp223->tl=0;_tmp223;}));});Cyc_str_sepstr(_tmp691,({const char*_tmp224="::";_tag_fat(_tmp224,sizeof(char),3U);}));});}}}
# 966
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 970
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr _tmp225=Cyc_Absynpp_qvar2string(q);struct _fat_ptr qs=_tmp225;
if(Cyc_PP_tex_output)
# 974
return({struct _fat_ptr _tmp694=(struct _fat_ptr)({struct _fat_ptr _tmp693=(struct _fat_ptr)({struct _fat_ptr _tmp692=({const char*_tmp226="\\textbf{";_tag_fat(_tmp226,sizeof(char),9U);});Cyc_strconcat(_tmp692,(struct _fat_ptr)qs);});Cyc_strconcat(_tmp693,({const char*_tmp227="}";_tag_fat(_tmp227,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp694,(int)
Cyc_strlen((struct _fat_ptr)qs));});else{
return Cyc_PP_text(qs);}}
# 979
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 981
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 984
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp228=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp13=_tmp228;union Cyc_Absyn_Nmspace _tmp229=_stmttmp13;struct Cyc_List_List*_tmp22A;struct Cyc_List_List*_tmp22B;switch((_tmp229.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp229.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 995
 return(struct _fat_ptr)({struct _fat_ptr _tmp695=({const char*_tmp22C="/* bad namespace : */ ";_tag_fat(_tmp22C,sizeof(char),23U);});Cyc_strconcat(_tmp695,(struct _fat_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp22B=(_tmp229.C_n).val;_LL6: {struct Cyc_List_List*l=_tmp22B;
# 988
_tmp22A=l;goto _LL8;}default: _LL7: _tmp22A=(_tmp229.Abs_n).val;_LL8: {struct Cyc_List_List*l=_tmp22A;
# 990
if(((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,l,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 993
goto _LLA;}}}_LL0:;}else{
# 998
return*(*v).f2;}}
# 1001
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 1004
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _fat_ptr _tmp22D=Cyc_Absynpp_typedef_name2string(v);struct _fat_ptr vs=_tmp22D;
if(Cyc_PP_tex_output)
# 1008
return({struct _fat_ptr _tmp698=(struct _fat_ptr)({struct _fat_ptr _tmp697=(struct _fat_ptr)({struct _fat_ptr _tmp696=({const char*_tmp22E="\\textbf{";_tag_fat(_tmp22E,sizeof(char),9U);});Cyc_strconcat(_tmp696,(struct _fat_ptr)vs);});Cyc_strconcat(_tmp697,({const char*_tmp22F="}";_tag_fat(_tmp22F,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp698,(int)
Cyc_strlen((struct _fat_ptr)vs));});else{
return Cyc_PP_text(vs);}}
# 1013
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp699=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp699,t,0);});}
# 1017
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_tmp230=f;unsigned _tmp231;struct _fat_ptr*_tmp232;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp230)->tag == 0U){_LL1: _tmp232=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp230)->f1;_LL2: {struct _fat_ptr*n=_tmp232;
return Cyc_PP_textptr(n);}}else{_LL3: _tmp231=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp230)->f1;_LL4: {unsigned n=_tmp231;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp235=({struct Cyc_Int_pa_PrintArg_struct _tmp5BC;_tmp5BC.tag=1U,_tmp5BC.f1=(unsigned long)((int)n);_tmp5BC;});void*_tmp233[1U];_tmp233[0]=& _tmp235;({struct _fat_ptr _tmp69A=({const char*_tmp234="%d";_tag_fat(_tmp234,sizeof(char),3U);});Cyc_aprintf(_tmp69A,_tag_fat(_tmp233,sizeof(void*),1U));});}));}}_LL0:;}
# 1031 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp236=e->r;void*_stmttmp14=_tmp236;void*_tmp237=_stmttmp14;struct Cyc_Absyn_Exp*_tmp238;struct Cyc_Absyn_Exp*_tmp239;enum Cyc_Absyn_Primop _tmp23A;switch(*((int*)_tmp237)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp23A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp237)->f1;_LL6: {enum Cyc_Absyn_Primop p=_tmp23A;
# 1036
enum Cyc_Absyn_Primop _tmp23B=p;switch(_tmp23B){case Cyc_Absyn_Plus: _LL58: _LL59:
 return 100;case Cyc_Absyn_Times: _LL5A: _LL5B:
 return 110;case Cyc_Absyn_Minus: _LL5C: _LL5D:
 return 100;case Cyc_Absyn_Div: _LL5E: _LL5F:
 goto _LL61;case Cyc_Absyn_Mod: _LL60: _LL61:
 return 110;case Cyc_Absyn_Eq: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Neq: _LL64: _LL65:
 return 70;case Cyc_Absyn_Gt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lt: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Gte: _LL6A: _LL6B:
 goto _LL6D;case Cyc_Absyn_Lte: _LL6C: _LL6D:
 return 80;case Cyc_Absyn_Not: _LL6E: _LL6F:
 goto _LL71;case Cyc_Absyn_Bitnot: _LL70: _LL71:
 return 130;case Cyc_Absyn_Bitand: _LL72: _LL73:
 return 60;case Cyc_Absyn_Bitor: _LL74: _LL75:
 return 40;case Cyc_Absyn_Bitxor: _LL76: _LL77:
 return 50;case Cyc_Absyn_Bitlshift: _LL78: _LL79:
 return 90;case Cyc_Absyn_Bitlrshift: _LL7A: _LL7B:
 return 80;case Cyc_Absyn_Numelts: _LL7C: _LL7D:
 return 140;default: _LL7E: _LL7F:
 return 140;}_LL57:;}case 4U: _LL7: _LL8:
# 1058
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return - 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp239=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp237)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp239;
return Cyc_Absynpp_exp_prec(e2);}case 13U: _LL1B: _tmp238=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp237)->f1;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp238;
return Cyc_Absynpp_exp_prec(e2);}case 14U: _LL1D: _LL1E:
 return 120;case 16U: _LL1F: _LL20:
 return 15;case 15U: _LL21: _LL22:
 goto _LL24;case 17U: _LL23: _LL24:
 goto _LL26;case 18U: _LL25: _LL26:
 goto _LL28;case 39U: _LL27: _LL28:
 goto _LL2A;case 40U: _LL29: _LL2A:
 goto _LL2C;case 38U: _LL2B: _LL2C:
 goto _LL2E;case 19U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 42U: _LL31: _LL32:
 goto _LL34;case 41U: _LL33: _LL34:
 return 130;case 21U: _LL35: _LL36:
 goto _LL38;case 22U: _LL37: _LL38:
 goto _LL3A;case 23U: _LL39: _LL3A:
 return 140;case 24U: _LL3B: _LL3C:
 return 150;case 25U: _LL3D: _LL3E:
 goto _LL40;case 26U: _LL3F: _LL40:
 goto _LL42;case 27U: _LL41: _LL42:
 goto _LL44;case 28U: _LL43: _LL44:
 goto _LL46;case 29U: _LL45: _LL46:
 goto _LL48;case 30U: _LL47: _LL48:
 goto _LL4A;case 31U: _LL49: _LL4A:
 goto _LL4C;case 32U: _LL4B: _LL4C:
 goto _LL4E;case 33U: _LL4D: _LL4E:
 goto _LL50;case 34U: _LL4F: _LL50:
 goto _LL52;case 35U: _LL51: _LL52:
 goto _LL54;case 36U: _LL53: _LL54:
 return 140;default: _LL55: _LL56:
 return 10000;}_LL0:;}
# 1101
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct _tuple19{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1105
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
struct _tuple19*_tmp23C=(struct _tuple19*)o->hd;struct _tuple19*_stmttmp15=_tmp23C;struct _tuple19*_tmp23D=_stmttmp15;struct Cyc_Absyn_Exp*_tmp23F;struct _fat_ptr _tmp23E;_LL1: _tmp23E=_tmp23D->f1;_tmp23F=_tmp23D->f2;_LL2: {struct _fat_ptr c=_tmp23E;struct Cyc_Absyn_Exp*e=_tmp23F;
s=({struct Cyc_PP_Doc*_tmp240[6U];_tmp240[0]=s,({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_text(({const char*_tmp241="\"";_tag_fat(_tmp241,sizeof(char),2U);}));_tmp240[1]=_tmp69F;}),({struct Cyc_PP_Doc*_tmp69E=Cyc_PP_text(c);_tmp240[2]=_tmp69E;}),({struct Cyc_PP_Doc*_tmp69D=Cyc_PP_text(({const char*_tmp242="\" (";_tag_fat(_tmp242,sizeof(char),4U);}));_tmp240[3]=_tmp69D;}),({struct Cyc_PP_Doc*_tmp69C=Cyc_Absynpp_exp2doc(e);_tmp240[4]=_tmp69C;}),({struct Cyc_PP_Doc*_tmp69B=Cyc_PP_text(({const char*_tmp243=")";_tag_fat(_tmp243,sizeof(char),2U);}));_tmp240[5]=_tmp69B;});Cyc_PP_cat(_tag_fat(_tmp240,sizeof(struct Cyc_PP_Doc*),6U));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp244[2U];_tmp244[0]=s,({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(({const char*_tmp245=",";_tag_fat(_tmp245,sizeof(char),2U);}));_tmp244[1]=_tmp6A0;});Cyc_PP_cat(_tag_fat(_tmp244,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1115
return s;}
# 1118
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp246[2U];({struct Cyc_PP_Doc*_tmp6A2=Cyc_PP_text(({const char*_tmp247=": ";_tag_fat(_tmp247,sizeof(char),3U);}));_tmp246[0]=_tmp6A2;}),({struct Cyc_PP_Doc*_tmp6A1=Cyc_Absynpp_asm_iolist_doc_in(o);_tmp246[1]=_tmp6A1;});Cyc_PP_cat(_tag_fat(_tmp246,sizeof(struct Cyc_PP_Doc*),2U));});
# 1123
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_tmp248[3U];_tmp248[0]=s,({struct Cyc_PP_Doc*_tmp6A4=Cyc_PP_text(({const char*_tmp249=": : ";_tag_fat(_tmp249,sizeof(char),5U);}));_tmp248[1]=_tmp6A4;}),({struct Cyc_PP_Doc*_tmp6A3=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp248[2]=_tmp6A3;});Cyc_PP_cat(_tag_fat(_tmp248,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1127
s=({struct Cyc_PP_Doc*_tmp24A[3U];_tmp24A[0]=s,({struct Cyc_PP_Doc*_tmp6A6=Cyc_PP_text(({const char*_tmp24B=" : ";_tag_fat(_tmp24B,sizeof(char),4U);}));_tmp24A[1]=_tmp6A6;}),({struct Cyc_PP_Doc*_tmp6A5=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp24A[2]=_tmp6A5;});Cyc_PP_cat(_tag_fat(_tmp24A,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1129
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_tmp24C[2U];_tmp24C[0]=s,ncol == 0?({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_text(({const char*_tmp24D=": : :";_tag_fat(_tmp24D,sizeof(char),6U);}));_tmp24C[1]=_tmp6A9;}):(ncol == 1?({struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_text(({const char*_tmp24E=" : : ";_tag_fat(_tmp24E,sizeof(char),6U);}));_tmp24C[1]=_tmp6A8;}):({struct Cyc_PP_Doc*_tmp6A7=Cyc_PP_text(({const char*_tmp24F=" : ";_tag_fat(_tmp24F,sizeof(char),4U);}));_tmp24C[1]=_tmp6A7;}));Cyc_PP_cat(_tag_fat(_tmp24C,sizeof(struct Cyc_PP_Doc*),2U));});
while(cl != 0){
s=({struct Cyc_PP_Doc*_tmp250[4U];_tmp250[0]=s,({struct Cyc_PP_Doc*_tmp6AC=Cyc_PP_text(({const char*_tmp251="\"";_tag_fat(_tmp251,sizeof(char),2U);}));_tmp250[1]=_tmp6AC;}),({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_tmp250[2]=_tmp6AB;}),({struct Cyc_PP_Doc*_tmp6AA=Cyc_PP_text(({const char*_tmp252="\"";_tag_fat(_tmp252,sizeof(char),2U);}));_tmp250[3]=_tmp6AA;});Cyc_PP_cat(_tag_fat(_tmp250,sizeof(struct Cyc_PP_Doc*),4U));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_tmp253[2U];_tmp253[0]=s,({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp254=", ";_tag_fat(_tmp254,sizeof(char),3U);}));_tmp253[1]=_tmp6AD;});Cyc_PP_cat(_tag_fat(_tmp253,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1139
return s;}
# 1142
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp255=e->r;void*_stmttmp16=_tmp255;void*_tmp256=_stmttmp16;struct Cyc_Absyn_Stmt*_tmp257;struct Cyc_List_List*_tmp259;struct Cyc_Core_Opt*_tmp258;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*_tmp25A;int _tmp260;struct Cyc_Absyn_Exp*_tmp25F;void**_tmp25E;struct Cyc_Absyn_Exp*_tmp25D;int _tmp25C;struct Cyc_Absyn_Enumfield*_tmp261;struct Cyc_Absyn_Enumfield*_tmp262;struct Cyc_Absyn_Datatypefield*_tmp264;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp265;struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp267;struct _tuple0*_tmp266;void*_tmp26A;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Vardecl*_tmp26B;struct Cyc_List_List*_tmp26E;struct Cyc_List_List*_tmp270;struct _tuple8*_tmp26F;struct Cyc_List_List*_tmp271;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp272;struct _fat_ptr*_tmp275;struct Cyc_Absyn_Exp*_tmp274;struct _fat_ptr*_tmp277;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_List_List*_tmp27A;void*_tmp279;struct _fat_ptr*_tmp27C;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_List_List*_tmp281;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp27F;struct _fat_ptr _tmp27E;int _tmp27D;void*_tmp282;struct Cyc_Absyn_Exp*_tmp283;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Exp*_tmp285;void*_tmp286;struct Cyc_Absyn_Exp*_tmp288;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*_tmp289;struct Cyc_Absyn_Exp*_tmp28B;void*_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_List_List*_tmp290;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;enum Cyc_Absyn_Incrementor _tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Core_Opt*_tmp29D;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_List_List*_tmp2A0;enum Cyc_Absyn_Primop _tmp29F;struct _fat_ptr _tmp2A1;void*_tmp2A2;union Cyc_Absyn_Cnst _tmp2A3;switch(*((int*)_tmp256)){case 0U: _LL1: _tmp2A3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp2A3;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1U: _LL3: _tmp2A2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL4: {void*b=_tmp2A2;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL5: _tmp2A1=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL6: {struct _fat_ptr p=_tmp2A1;
# 1149
s=({struct Cyc_PP_Doc*_tmp2A4[4U];({struct Cyc_PP_Doc*_tmp6B1=Cyc_PP_text(({const char*_tmp2A5="__cyclone_pragma__";_tag_fat(_tmp2A5,sizeof(char),19U);}));_tmp2A4[0]=_tmp6B1;}),({struct Cyc_PP_Doc*_tmp6B0=Cyc_PP_text(({const char*_tmp2A6="(";_tag_fat(_tmp2A6,sizeof(char),2U);}));_tmp2A4[1]=_tmp6B0;}),({struct Cyc_PP_Doc*_tmp6AF=Cyc_PP_text(p);_tmp2A4[2]=_tmp6AF;}),({struct Cyc_PP_Doc*_tmp6AE=Cyc_PP_text(({const char*_tmp2A7=")";_tag_fat(_tmp2A7,sizeof(char),2U);}));_tmp2A4[3]=_tmp6AE;});Cyc_PP_cat(_tag_fat(_tmp2A4,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 3U: _LL7: _tmp29F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp2A0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp29F;struct Cyc_List_List*es=_tmp2A0;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4U: _LL9: _tmp29C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp29D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_tmp29E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp256)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp29C;struct Cyc_Core_Opt*popt=_tmp29D;struct Cyc_Absyn_Exp*e2=_tmp29E;
# 1152
s=({struct Cyc_PP_Doc*_tmp2A8[5U];({struct Cyc_PP_Doc*_tmp6B7=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2A8[0]=_tmp6B7;}),({
struct Cyc_PP_Doc*_tmp6B6=Cyc_PP_text(({const char*_tmp2A9=" ";_tag_fat(_tmp2A9,sizeof(char),2U);}));_tmp2A8[1]=_tmp6B6;}),
popt == 0?({struct Cyc_PP_Doc*_tmp6B5=Cyc_PP_nil_doc();_tmp2A8[2]=_tmp6B5;}):({struct Cyc_PP_Doc*_tmp6B4=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_tmp2A8[2]=_tmp6B4;}),({
struct Cyc_PP_Doc*_tmp6B3=Cyc_PP_text(({const char*_tmp2AA="= ";_tag_fat(_tmp2AA,sizeof(char),3U);}));_tmp2A8[3]=_tmp6B3;}),({
struct Cyc_PP_Doc*_tmp6B2=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2A8[4]=_tmp6B2;});Cyc_PP_cat(_tag_fat(_tmp2A8,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp29A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp29B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LLC: {struct Cyc_Absyn_Exp*e2=_tmp29A;enum Cyc_Absyn_Incrementor i=_tmp29B;
# 1159
struct Cyc_PP_Doc*_tmp2AB=Cyc_Absynpp_exp2doc_prec(myprec,e2);struct Cyc_PP_Doc*es=_tmp2AB;
{enum Cyc_Absyn_Incrementor _tmp2AC=i;switch(_tmp2AC){case Cyc_Absyn_PreInc: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp2AD[2U];({struct Cyc_PP_Doc*_tmp6B8=Cyc_PP_text(({const char*_tmp2AE="++";_tag_fat(_tmp2AE,sizeof(char),3U);}));_tmp2AD[0]=_tmp6B8;}),_tmp2AD[1]=es;Cyc_PP_cat(_tag_fat(_tmp2AD,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PreDec: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp2AF[2U];({struct Cyc_PP_Doc*_tmp6B9=Cyc_PP_text(({const char*_tmp2B0="--";_tag_fat(_tmp2B0,sizeof(char),3U);}));_tmp2AF[0]=_tmp6B9;}),_tmp2AF[1]=es;Cyc_PP_cat(_tag_fat(_tmp2AF,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PostInc: _LL5C: _LL5D:
 s=({struct Cyc_PP_Doc*_tmp2B1[2U];_tmp2B1[0]=es,({struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_text(({const char*_tmp2B2="++";_tag_fat(_tmp2B2,sizeof(char),3U);}));_tmp2B1[1]=_tmp6BA;});Cyc_PP_cat(_tag_fat(_tmp2B1,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PostDec: _LL5E: _LL5F:
 goto _LL61;default: _LL60: _LL61:
 s=({struct Cyc_PP_Doc*_tmp2B3[2U];_tmp2B3[0]=es,({struct Cyc_PP_Doc*_tmp6BB=Cyc_PP_text(({const char*_tmp2B4="--";_tag_fat(_tmp2B4,sizeof(char),3U);}));_tmp2B3[1]=_tmp6BB;});Cyc_PP_cat(_tag_fat(_tmp2B3,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;}_LL57:;}
# 1167
goto _LL0;}case 6U: _LLD: _tmp297=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp298=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_tmp299=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp256)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp297;struct Cyc_Absyn_Exp*e2=_tmp298;struct Cyc_Absyn_Exp*e3=_tmp299;
# 1169
s=({struct Cyc_PP_Doc*_tmp2B5[5U];({struct Cyc_PP_Doc*_tmp6C0=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2B5[0]=_tmp6C0;}),({struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_text(({const char*_tmp2B6=" ? ";_tag_fat(_tmp2B6,sizeof(char),4U);}));_tmp2B5[1]=_tmp6BF;}),({struct Cyc_PP_Doc*_tmp6BE=Cyc_Absynpp_exp2doc_prec(0,e2);_tmp2B5[2]=_tmp6BE;}),({
struct Cyc_PP_Doc*_tmp6BD=Cyc_PP_text(({const char*_tmp2B7=" : ";_tag_fat(_tmp2B7,sizeof(char),4U);}));_tmp2B5[3]=_tmp6BD;}),({struct Cyc_PP_Doc*_tmp6BC=Cyc_Absynpp_exp2doc_prec(myprec,e3);_tmp2B5[4]=_tmp6BC;});Cyc_PP_cat(_tag_fat(_tmp2B5,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 7U: _LLF: _tmp295=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp296=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp295;struct Cyc_Absyn_Exp*e2=_tmp296;
# 1173
s=({struct Cyc_PP_Doc*_tmp2B8[3U];({struct Cyc_PP_Doc*_tmp6C3=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2B8[0]=_tmp6C3;}),({struct Cyc_PP_Doc*_tmp6C2=Cyc_PP_text(({const char*_tmp2B9=" && ";_tag_fat(_tmp2B9,sizeof(char),5U);}));_tmp2B8[1]=_tmp6C2;}),({struct Cyc_PP_Doc*_tmp6C1=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2B8[2]=_tmp6C1;});Cyc_PP_cat(_tag_fat(_tmp2B8,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 8U: _LL11: _tmp293=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp294=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp293;struct Cyc_Absyn_Exp*e2=_tmp294;
# 1176
s=({struct Cyc_PP_Doc*_tmp2BA[3U];({struct Cyc_PP_Doc*_tmp6C6=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2BA[0]=_tmp6C6;}),({struct Cyc_PP_Doc*_tmp6C5=Cyc_PP_text(({const char*_tmp2BB=" || ";_tag_fat(_tmp2BB,sizeof(char),5U);}));_tmp2BA[1]=_tmp6C5;}),({struct Cyc_PP_Doc*_tmp6C4=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2BA[2]=_tmp6C4;});Cyc_PP_cat(_tag_fat(_tmp2BA,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 9U: _LL13: _tmp291=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp292=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp291;struct Cyc_Absyn_Exp*e2=_tmp292;
# 1181
s=({struct Cyc_PP_Doc*_tmp2BC[3U];({struct Cyc_PP_Doc*_tmp6C9=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_tmp2BC[0]=_tmp6C9;}),({struct Cyc_PP_Doc*_tmp6C8=Cyc_PP_text(({const char*_tmp2BD=", ";_tag_fat(_tmp2BD,sizeof(char),3U);}));_tmp2BC[1]=_tmp6C8;}),({struct Cyc_PP_Doc*_tmp6C7=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_tmp2BC[2]=_tmp6C7;});Cyc_PP_cat(_tag_fat(_tmp2BC,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 10U: _LL15: _tmp28F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp290=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp28F;struct Cyc_List_List*es=_tmp290;
# 1184
s=({struct Cyc_PP_Doc*_tmp2BE[4U];({struct Cyc_PP_Doc*_tmp6CD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2BE[0]=_tmp6CD;}),({
struct Cyc_PP_Doc*_tmp6CC=Cyc_PP_text(({const char*_tmp2BF="(";_tag_fat(_tmp2BF,sizeof(char),2U);}));_tmp2BE[1]=_tmp6CC;}),({
struct Cyc_PP_Doc*_tmp6CB=Cyc_Absynpp_exps2doc_prec(20,es);_tmp2BE[2]=_tmp6CB;}),({
struct Cyc_PP_Doc*_tmp6CA=Cyc_PP_text(({const char*_tmp2C0=")";_tag_fat(_tmp2C0,sizeof(char),2U);}));_tmp2BE[3]=_tmp6CA;});Cyc_PP_cat(_tag_fat(_tmp2BE,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 11U: _LL17: _tmp28E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp28E;
s=({struct Cyc_PP_Doc*_tmp2C1[2U];({struct Cyc_PP_Doc*_tmp6CF=Cyc_PP_text(({const char*_tmp2C2="throw ";_tag_fat(_tmp2C2,sizeof(char),7U);}));_tmp2C1[0]=_tmp6CF;}),({struct Cyc_PP_Doc*_tmp6CE=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C1[1]=_tmp6CE;});Cyc_PP_cat(_tag_fat(_tmp2C1,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 12U: _LL19: _tmp28D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp28D;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13U: _LL1B: _tmp28C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp28C;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14U: _LL1D: _tmp28A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp28B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL1E: {void*t=_tmp28A;struct Cyc_Absyn_Exp*e1=_tmp28B;
# 1193
s=({struct Cyc_PP_Doc*_tmp2C3[4U];({struct Cyc_PP_Doc*_tmp6D3=Cyc_PP_text(({const char*_tmp2C4="(";_tag_fat(_tmp2C4,sizeof(char),2U);}));_tmp2C3[0]=_tmp6D3;}),({struct Cyc_PP_Doc*_tmp6D2=Cyc_Absynpp_typ2doc(t);_tmp2C3[1]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_PP_text(({const char*_tmp2C5=")";_tag_fat(_tmp2C5,sizeof(char),2U);}));_tmp2C3[2]=_tmp6D1;}),({struct Cyc_PP_Doc*_tmp6D0=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C3[3]=_tmp6D0;});Cyc_PP_cat(_tag_fat(_tmp2C3,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1F: _tmp289=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp289;
s=({struct Cyc_PP_Doc*_tmp2C6[2U];({struct Cyc_PP_Doc*_tmp6D5=Cyc_PP_text(({const char*_tmp2C7="&";_tag_fat(_tmp2C7,sizeof(char),2U);}));_tmp2C6[0]=_tmp6D5;}),({struct Cyc_PP_Doc*_tmp6D4=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C6[1]=_tmp6D4;});Cyc_PP_cat(_tag_fat(_tmp2C6,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 16U: _LL21: _tmp287=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp288=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL22: {struct Cyc_Absyn_Exp*ropt=_tmp287;struct Cyc_Absyn_Exp*e1=_tmp288;
# 1196
if(ropt == 0)
s=({struct Cyc_PP_Doc*_tmp2C8[2U];({struct Cyc_PP_Doc*_tmp6D7=Cyc_PP_text(({const char*_tmp2C9="new ";_tag_fat(_tmp2C9,sizeof(char),5U);}));_tmp2C8[0]=_tmp6D7;}),({struct Cyc_PP_Doc*_tmp6D6=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C8[1]=_tmp6D6;});Cyc_PP_cat(_tag_fat(_tmp2C8,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1199
s=({struct Cyc_PP_Doc*_tmp2CA[4U];({struct Cyc_PP_Doc*_tmp6DB=Cyc_PP_text(({const char*_tmp2CB="rnew(";_tag_fat(_tmp2CB,sizeof(char),6U);}));_tmp2CA[0]=_tmp6DB;}),({struct Cyc_PP_Doc*_tmp6DA=Cyc_Absynpp_exp2doc(ropt);_tmp2CA[1]=_tmp6DA;}),({struct Cyc_PP_Doc*_tmp6D9=Cyc_PP_text(({const char*_tmp2CC=") ";_tag_fat(_tmp2CC,sizeof(char),3U);}));_tmp2CA[2]=_tmp6D9;}),({struct Cyc_PP_Doc*_tmp6D8=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2CA[3]=_tmp6D8;});Cyc_PP_cat(_tag_fat(_tmp2CA,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;}case 17U: _LL23: _tmp286=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL24: {void*t=_tmp286;
s=({struct Cyc_PP_Doc*_tmp2CD[3U];({struct Cyc_PP_Doc*_tmp6DE=Cyc_PP_text(({const char*_tmp2CE="sizeof(";_tag_fat(_tmp2CE,sizeof(char),8U);}));_tmp2CD[0]=_tmp6DE;}),({struct Cyc_PP_Doc*_tmp6DD=Cyc_Absynpp_typ2doc(t);_tmp2CD[1]=_tmp6DD;}),({struct Cyc_PP_Doc*_tmp6DC=Cyc_PP_text(({const char*_tmp2CF=")";_tag_fat(_tmp2CF,sizeof(char),2U);}));_tmp2CD[2]=_tmp6DC;});Cyc_PP_cat(_tag_fat(_tmp2CD,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 18U: _LL25: _tmp285=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp285;
s=({struct Cyc_PP_Doc*_tmp2D0[3U];({struct Cyc_PP_Doc*_tmp6E1=Cyc_PP_text(({const char*_tmp2D1="sizeof(";_tag_fat(_tmp2D1,sizeof(char),8U);}));_tmp2D0[0]=_tmp6E1;}),({struct Cyc_PP_Doc*_tmp6E0=Cyc_Absynpp_exp2doc(e1);_tmp2D0[1]=_tmp6E0;}),({struct Cyc_PP_Doc*_tmp6DF=Cyc_PP_text(({const char*_tmp2D2=")";_tag_fat(_tmp2D2,sizeof(char),2U);}));_tmp2D0[2]=_tmp6DF;});Cyc_PP_cat(_tag_fat(_tmp2D0,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 42U: _LL27: _tmp284=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp284;
# 1204
s=({struct Cyc_PP_Doc*_tmp2D3[3U];({struct Cyc_PP_Doc*_tmp6E4=Cyc_PP_text(({const char*_tmp2D4="@assert(";_tag_fat(_tmp2D4,sizeof(char),9U);}));_tmp2D3[0]=_tmp6E4;}),({struct Cyc_PP_Doc*_tmp6E3=Cyc_Absynpp_exp2doc(e);_tmp2D3[1]=_tmp6E3;}),({struct Cyc_PP_Doc*_tmp6E2=Cyc_PP_text(({const char*_tmp2D5=")";_tag_fat(_tmp2D5,sizeof(char),2U);}));_tmp2D3[2]=_tmp6E2;});Cyc_PP_cat(_tag_fat(_tmp2D3,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 41U: _LL29: _tmp283=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp283;
# 1206
s=({struct Cyc_PP_Doc*_tmp2D6[3U];({struct Cyc_PP_Doc*_tmp6E7=Cyc_PP_text(({const char*_tmp2D7="__extension__(";_tag_fat(_tmp2D7,sizeof(char),15U);}));_tmp2D6[0]=_tmp6E7;}),({struct Cyc_PP_Doc*_tmp6E6=Cyc_Absynpp_exp2doc(e);_tmp2D6[1]=_tmp6E6;}),({struct Cyc_PP_Doc*_tmp6E5=Cyc_PP_text(({const char*_tmp2D8=")";_tag_fat(_tmp2D8,sizeof(char),2U);}));_tmp2D6[2]=_tmp6E5;});Cyc_PP_cat(_tag_fat(_tmp2D6,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 39U: _LL2B: _tmp282=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL2C: {void*t=_tmp282;
s=({struct Cyc_PP_Doc*_tmp2D9[3U];({struct Cyc_PP_Doc*_tmp6EA=Cyc_PP_text(({const char*_tmp2DA="valueof(";_tag_fat(_tmp2DA,sizeof(char),9U);}));_tmp2D9[0]=_tmp6EA;}),({struct Cyc_PP_Doc*_tmp6E9=Cyc_Absynpp_typ2doc(t);_tmp2D9[1]=_tmp6E9;}),({struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_text(({const char*_tmp2DB=")";_tag_fat(_tmp2DB,sizeof(char),2U);}));_tmp2D9[2]=_tmp6E8;});Cyc_PP_cat(_tag_fat(_tmp2D9,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 40U: _LL2D: _tmp27D=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp27E=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_tmp27F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp256)->f3;_tmp280=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp256)->f4;_tmp281=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp256)->f5;_LL2E: {int vol=_tmp27D;struct _fat_ptr t=_tmp27E;struct Cyc_List_List*o=_tmp27F;struct Cyc_List_List*i=_tmp280;struct Cyc_List_List*cl=_tmp281;
# 1209
if(vol)
s=({struct Cyc_PP_Doc*_tmp2DC[7U];({struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_text(({const char*_tmp2DD="__asm__";_tag_fat(_tmp2DD,sizeof(char),8U);}));_tmp2DC[0]=_tmp6F1;}),({struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_text(({const char*_tmp2DE=" volatile (";_tag_fat(_tmp2DE,sizeof(char),12U);}));_tmp2DC[1]=_tmp6F0;}),({struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_text(({const char*_tmp2DF="\"";_tag_fat(_tmp2DF,sizeof(char),2U);}));_tmp2DC[2]=_tmp6EF;}),({struct Cyc_PP_Doc*_tmp6EE=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_tmp2DC[3]=_tmp6EE;}),({struct Cyc_PP_Doc*_tmp6ED=Cyc_PP_text(({const char*_tmp2E0="\"";_tag_fat(_tmp2E0,sizeof(char),2U);}));_tmp2DC[4]=_tmp6ED;}),({struct Cyc_PP_Doc*_tmp6EC=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2DC[5]=_tmp6EC;}),({struct Cyc_PP_Doc*_tmp6EB=Cyc_PP_text(({const char*_tmp2E1=")";_tag_fat(_tmp2E1,sizeof(char),2U);}));_tmp2DC[6]=_tmp6EB;});Cyc_PP_cat(_tag_fat(_tmp2DC,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1212
s=({struct Cyc_PP_Doc*_tmp2E2[6U];({struct Cyc_PP_Doc*_tmp6F7=Cyc_PP_text(({const char*_tmp2E3="__asm__(";_tag_fat(_tmp2E3,sizeof(char),9U);}));_tmp2E2[0]=_tmp6F7;}),({struct Cyc_PP_Doc*_tmp6F6=Cyc_PP_text(({const char*_tmp2E4="\"";_tag_fat(_tmp2E4,sizeof(char),2U);}));_tmp2E2[1]=_tmp6F6;}),({struct Cyc_PP_Doc*_tmp6F5=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_tmp2E2[2]=_tmp6F5;}),({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp2E5="\"";_tag_fat(_tmp2E5,sizeof(char),2U);}));_tmp2E2[3]=_tmp6F4;}),({struct Cyc_PP_Doc*_tmp6F3=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2E2[4]=_tmp6F3;}),({struct Cyc_PP_Doc*_tmp6F2=Cyc_PP_text(({const char*_tmp2E6=")";_tag_fat(_tmp2E6,sizeof(char),2U);}));_tmp2E2[5]=_tmp6F2;});Cyc_PP_cat(_tag_fat(_tmp2E2,sizeof(struct Cyc_PP_Doc*),6U));});}
goto _LL0;}case 38U: _LL2F: _tmp27B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp27C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL30: {struct Cyc_Absyn_Exp*e=_tmp27B;struct _fat_ptr*f=_tmp27C;
# 1215
s=({struct Cyc_PP_Doc*_tmp2E7[5U];({struct Cyc_PP_Doc*_tmp6FC=Cyc_PP_text(({const char*_tmp2E8="tagcheck(";_tag_fat(_tmp2E8,sizeof(char),10U);}));_tmp2E7[0]=_tmp6FC;}),({struct Cyc_PP_Doc*_tmp6FB=Cyc_Absynpp_exp2doc(e);_tmp2E7[1]=_tmp6FB;}),({struct Cyc_PP_Doc*_tmp6FA=Cyc_PP_text(({const char*_tmp2E9=".";_tag_fat(_tmp2E9,sizeof(char),2U);}));_tmp2E7[2]=_tmp6FA;}),({struct Cyc_PP_Doc*_tmp6F9=Cyc_PP_textptr(f);_tmp2E7[3]=_tmp6F9;}),({struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_text(({const char*_tmp2EA=")";_tag_fat(_tmp2EA,sizeof(char),2U);}));_tmp2E7[4]=_tmp6F8;});Cyc_PP_cat(_tag_fat(_tmp2E7,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 19U: _LL31: _tmp279=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp27A=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL32: {void*t=_tmp279;struct Cyc_List_List*l=_tmp27A;
# 1218
s=({struct Cyc_PP_Doc*_tmp2EB[5U];({struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp2EC="offsetof(";_tag_fat(_tmp2EC,sizeof(char),10U);}));_tmp2EB[0]=_tmp702;}),({struct Cyc_PP_Doc*_tmp701=Cyc_Absynpp_typ2doc(t);_tmp2EB[1]=_tmp701;}),({struct Cyc_PP_Doc*_tmp700=Cyc_PP_text(({const char*_tmp2ED=",";_tag_fat(_tmp2ED,sizeof(char),2U);}));_tmp2EB[2]=_tmp700;}),({
struct Cyc_PP_Doc*_tmp6FF=({struct _fat_ptr _tmp6FE=({const char*_tmp2EE=".";_tag_fat(_tmp2EE,sizeof(char),2U);});Cyc_PP_seq(_tmp6FE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_offsetof_field_to_doc,l));});_tmp2EB[3]=_tmp6FF;}),({struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_text(({const char*_tmp2EF=")";_tag_fat(_tmp2EF,sizeof(char),2U);}));_tmp2EB[4]=_tmp6FD;});Cyc_PP_cat(_tag_fat(_tmp2EB,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 20U: _LL33: _tmp278=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp278;
s=({struct Cyc_PP_Doc*_tmp2F0[2U];({struct Cyc_PP_Doc*_tmp704=Cyc_PP_text(({const char*_tmp2F1="*";_tag_fat(_tmp2F1,sizeof(char),2U);}));_tmp2F0[0]=_tmp704;}),({struct Cyc_PP_Doc*_tmp703=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2F0[1]=_tmp703;});Cyc_PP_cat(_tag_fat(_tmp2F0,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 21U: _LL35: _tmp276=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp277=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp276;struct _fat_ptr*n=_tmp277;
# 1223
s=({struct Cyc_PP_Doc*_tmp2F2[3U];({struct Cyc_PP_Doc*_tmp707=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2F2[0]=_tmp707;}),({struct Cyc_PP_Doc*_tmp706=Cyc_PP_text(({const char*_tmp2F3=".";_tag_fat(_tmp2F3,sizeof(char),2U);}));_tmp2F2[1]=_tmp706;}),({struct Cyc_PP_Doc*_tmp705=Cyc_PP_textptr(n);_tmp2F2[2]=_tmp705;});Cyc_PP_cat(_tag_fat(_tmp2F2,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 22U: _LL37: _tmp274=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp275=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp274;struct _fat_ptr*n=_tmp275;
# 1225
s=({struct Cyc_PP_Doc*_tmp2F4[3U];({struct Cyc_PP_Doc*_tmp70A=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2F4[0]=_tmp70A;}),({struct Cyc_PP_Doc*_tmp709=Cyc_PP_text(({const char*_tmp2F5="->";_tag_fat(_tmp2F5,sizeof(char),3U);}));_tmp2F4[1]=_tmp709;}),({struct Cyc_PP_Doc*_tmp708=Cyc_PP_textptr(n);_tmp2F4[2]=_tmp708;});Cyc_PP_cat(_tag_fat(_tmp2F4,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 23U: _LL39: _tmp272=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp273=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp272;struct Cyc_Absyn_Exp*e2=_tmp273;
# 1227
s=({struct Cyc_PP_Doc*_tmp2F6[4U];({struct Cyc_PP_Doc*_tmp70E=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2F6[0]=_tmp70E;}),({struct Cyc_PP_Doc*_tmp70D=Cyc_PP_text(({const char*_tmp2F7="[";_tag_fat(_tmp2F7,sizeof(char),2U);}));_tmp2F6[1]=_tmp70D;}),({struct Cyc_PP_Doc*_tmp70C=Cyc_Absynpp_exp2doc(e2);_tmp2F6[2]=_tmp70C;}),({struct Cyc_PP_Doc*_tmp70B=Cyc_PP_text(({const char*_tmp2F8="]";_tag_fat(_tmp2F8,sizeof(char),2U);}));_tmp2F6[3]=_tmp70B;});Cyc_PP_cat(_tag_fat(_tmp2F6,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 24U: _LL3B: _tmp271=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL3C: {struct Cyc_List_List*es=_tmp271;
# 1229
s=({struct Cyc_PP_Doc*_tmp2F9[4U];({struct Cyc_PP_Doc*_tmp712=Cyc_Absynpp_dollar();_tmp2F9[0]=_tmp712;}),({struct Cyc_PP_Doc*_tmp711=Cyc_PP_text(({const char*_tmp2FA="(";_tag_fat(_tmp2FA,sizeof(char),2U);}));_tmp2F9[1]=_tmp711;}),({struct Cyc_PP_Doc*_tmp710=Cyc_Absynpp_exps2doc_prec(20,es);_tmp2F9[2]=_tmp710;}),({struct Cyc_PP_Doc*_tmp70F=Cyc_PP_text(({const char*_tmp2FB=")";_tag_fat(_tmp2FB,sizeof(char),2U);}));_tmp2F9[3]=_tmp70F;});Cyc_PP_cat(_tag_fat(_tmp2F9,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 25U: _LL3D: _tmp26F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp270=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL3E: {struct _tuple8*vat=_tmp26F;struct Cyc_List_List*des=_tmp270;
# 1231
s=({struct Cyc_PP_Doc*_tmp2FC[4U];({struct Cyc_PP_Doc*_tmp717=Cyc_PP_text(({const char*_tmp2FD="(";_tag_fat(_tmp2FD,sizeof(char),2U);}));_tmp2FC[0]=_tmp717;}),({struct Cyc_PP_Doc*_tmp716=Cyc_Absynpp_typ2doc((*vat).f3);_tmp2FC[1]=_tmp716;}),({struct Cyc_PP_Doc*_tmp715=Cyc_PP_text(({const char*_tmp2FE=")";_tag_fat(_tmp2FE,sizeof(char),2U);}));_tmp2FC[2]=_tmp715;}),({
struct Cyc_PP_Doc*_tmp714=({struct _fat_ptr _tmp713=({const char*_tmp2FF=",";_tag_fat(_tmp2FF,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp713,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});_tmp2FC[3]=_tmp714;});Cyc_PP_cat(_tag_fat(_tmp2FC,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 26U: _LL3F: _tmp26E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL40: {struct Cyc_List_List*des=_tmp26E;
s=({struct _fat_ptr _tmp718=({const char*_tmp300=",";_tag_fat(_tmp300,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp718,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 27U: _LL41: _tmp26B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp26C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_tmp26D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp256)->f3;_LL42: {struct Cyc_Absyn_Vardecl*vd=_tmp26B;struct Cyc_Absyn_Exp*e1=_tmp26C;struct Cyc_Absyn_Exp*e2=_tmp26D;
# 1236
s=({struct Cyc_PP_Doc*_tmp301[8U];({struct Cyc_PP_Doc*_tmp720=Cyc_Absynpp_lb();_tmp301[0]=_tmp720;}),({struct Cyc_PP_Doc*_tmp71F=Cyc_PP_text(({const char*_tmp302="for ";_tag_fat(_tmp302,sizeof(char),5U);}));_tmp301[1]=_tmp71F;}),({
struct Cyc_PP_Doc*_tmp71E=Cyc_PP_text(*(*vd->name).f2);_tmp301[2]=_tmp71E;}),({struct Cyc_PP_Doc*_tmp71D=Cyc_PP_text(({const char*_tmp303=" < ";_tag_fat(_tmp303,sizeof(char),4U);}));_tmp301[3]=_tmp71D;}),({struct Cyc_PP_Doc*_tmp71C=Cyc_Absynpp_exp2doc(e1);_tmp301[4]=_tmp71C;}),({struct Cyc_PP_Doc*_tmp71B=Cyc_PP_text(({const char*_tmp304=" : ";_tag_fat(_tmp304,sizeof(char),4U);}));_tmp301[5]=_tmp71B;}),({
struct Cyc_PP_Doc*_tmp71A=Cyc_Absynpp_exp2doc(e2);_tmp301[6]=_tmp71A;}),({struct Cyc_PP_Doc*_tmp719=Cyc_Absynpp_rb();_tmp301[7]=_tmp719;});Cyc_PP_cat(_tag_fat(_tmp301,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 28U: _LL43: _tmp269=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp26A=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL44: {struct Cyc_Absyn_Exp*e=_tmp269;void*t=_tmp26A;
# 1241
s=({struct Cyc_PP_Doc*_tmp305[7U];({struct Cyc_PP_Doc*_tmp727=Cyc_Absynpp_lb();_tmp305[0]=_tmp727;}),({struct Cyc_PP_Doc*_tmp726=Cyc_PP_text(({const char*_tmp306="for x ";_tag_fat(_tmp306,sizeof(char),7U);}));_tmp305[1]=_tmp726;}),({
struct Cyc_PP_Doc*_tmp725=Cyc_PP_text(({const char*_tmp307=" < ";_tag_fat(_tmp307,sizeof(char),4U);}));_tmp305[2]=_tmp725;}),({
struct Cyc_PP_Doc*_tmp724=Cyc_Absynpp_exp2doc(e);_tmp305[3]=_tmp724;}),({
struct Cyc_PP_Doc*_tmp723=Cyc_PP_text(({const char*_tmp308=" : ";_tag_fat(_tmp308,sizeof(char),4U);}));_tmp305[4]=_tmp723;}),({
struct Cyc_PP_Doc*_tmp722=Cyc_Absynpp_typ2doc(t);_tmp305[5]=_tmp722;}),({
struct Cyc_PP_Doc*_tmp721=Cyc_Absynpp_rb();_tmp305[6]=_tmp721;});Cyc_PP_cat(_tag_fat(_tmp305,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 29U: _LL45: _tmp266=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp267=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_tmp268=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp256)->f3;_LL46: {struct _tuple0*n=_tmp266;struct Cyc_List_List*ts=_tmp267;struct Cyc_List_List*des=_tmp268;
# 1249
struct Cyc_List_List*_tmp309=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des);struct Cyc_List_List*des_doc=_tmp309;
s=({struct Cyc_PP_Doc*_tmp30A[2U];({struct Cyc_PP_Doc*_tmp72B=Cyc_Absynpp_qvar2doc(n);_tmp30A[0]=_tmp72B;}),({
struct Cyc_PP_Doc*_tmp72A=({struct _fat_ptr _tmp729=({const char*_tmp30B=",";_tag_fat(_tmp30B,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp729,
ts != 0?({struct Cyc_List_List*_tmp30C=_cycalloc(sizeof(*_tmp30C));({struct Cyc_PP_Doc*_tmp728=Cyc_Absynpp_tps2doc(ts);_tmp30C->hd=_tmp728;}),_tmp30C->tl=des_doc;_tmp30C;}): des_doc);});
# 1251
_tmp30A[1]=_tmp72A;});Cyc_PP_cat(_tag_fat(_tmp30A,sizeof(struct Cyc_PP_Doc*),2U));});
# 1253
goto _LL0;}case 30U: _LL47: _tmp265=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL48: {struct Cyc_List_List*des=_tmp265;
s=({struct _fat_ptr _tmp72C=({const char*_tmp30D=",";_tag_fat(_tmp30D,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp72C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 31U: _LL49: _tmp263=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp264=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp256)->f3;_LL4A: {struct Cyc_List_List*es=_tmp263;struct Cyc_Absyn_Datatypefield*tuf=_tmp264;
# 1256
if(es == 0)
# 1258
s=Cyc_Absynpp_qvar2doc(tuf->name);else{
# 1260
s=({struct Cyc_PP_Doc*_tmp30E[2U];({struct Cyc_PP_Doc*_tmp731=Cyc_Absynpp_qvar2doc(tuf->name);_tmp30E[0]=_tmp731;}),({
struct Cyc_PP_Doc*_tmp730=({struct _fat_ptr _tmp72F=({const char*_tmp30F="(";_tag_fat(_tmp30F,sizeof(char),2U);});struct _fat_ptr _tmp72E=({const char*_tmp310=")";_tag_fat(_tmp310,sizeof(char),2U);});struct _fat_ptr _tmp72D=({const char*_tmp311=",";_tag_fat(_tmp311,sizeof(char),2U);});Cyc_PP_egroup(_tmp72F,_tmp72E,_tmp72D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,es));});_tmp30E[1]=_tmp730;});Cyc_PP_cat(_tag_fat(_tmp30E,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;}case 32U: _LL4B: _tmp262=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL4C: {struct Cyc_Absyn_Enumfield*ef=_tmp262;
_tmp261=ef;goto _LL4E;}case 33U: _LL4D: _tmp261=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL4E: {struct Cyc_Absyn_Enumfield*ef=_tmp261;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 34U: _LL4F: _tmp25C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp256)->f1).is_calloc;_tmp25D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp256)->f1).rgn;_tmp25E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp256)->f1).elt_type;_tmp25F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp256)->f1).num_elts;_tmp260=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp256)->f1).inline_call;_LL50: {int is_calloc=_tmp25C;struct Cyc_Absyn_Exp*rgnopt=_tmp25D;void**topt=_tmp25E;struct Cyc_Absyn_Exp*e=_tmp25F;int inline_call=_tmp260;
# 1268
if(is_calloc){
# 1270
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U);
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp312[5U];({struct Cyc_PP_Doc*_tmp736=Cyc_PP_text(({const char*_tmp313="calloc(";_tag_fat(_tmp313,sizeof(char),8U);}));_tmp312[0]=_tmp736;}),({struct Cyc_PP_Doc*_tmp735=Cyc_Absynpp_exp2doc(e);_tmp312[1]=_tmp735;}),({struct Cyc_PP_Doc*_tmp734=Cyc_PP_text(({const char*_tmp314=",";_tag_fat(_tmp314,sizeof(char),2U);}));_tmp312[2]=_tmp734;}),({struct Cyc_PP_Doc*_tmp733=Cyc_Absynpp_exp2doc(st);_tmp312[3]=_tmp733;}),({struct Cyc_PP_Doc*_tmp732=Cyc_PP_text(({const char*_tmp315=")";_tag_fat(_tmp315,sizeof(char),2U);}));_tmp312[4]=_tmp732;});Cyc_PP_cat(_tag_fat(_tmp312,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1274
s=({struct Cyc_PP_Doc*_tmp316[7U];({struct Cyc_PP_Doc*_tmp73D=Cyc_PP_text(({const char*_tmp317="rcalloc(";_tag_fat(_tmp317,sizeof(char),9U);}));_tmp316[0]=_tmp73D;}),({struct Cyc_PP_Doc*_tmp73C=Cyc_Absynpp_exp2doc(rgnopt);_tmp316[1]=_tmp73C;}),({struct Cyc_PP_Doc*_tmp73B=Cyc_PP_text(({const char*_tmp318=",";_tag_fat(_tmp318,sizeof(char),2U);}));_tmp316[2]=_tmp73B;}),({
struct Cyc_PP_Doc*_tmp73A=Cyc_Absynpp_exp2doc(e);_tmp316[3]=_tmp73A;}),({struct Cyc_PP_Doc*_tmp739=Cyc_PP_text(({const char*_tmp319=",";_tag_fat(_tmp319,sizeof(char),2U);}));_tmp316[4]=_tmp739;}),({struct Cyc_PP_Doc*_tmp738=Cyc_Absynpp_exp2doc(st);_tmp316[5]=_tmp738;}),({struct Cyc_PP_Doc*_tmp737=Cyc_PP_text(({const char*_tmp31A=")";_tag_fat(_tmp31A,sizeof(char),2U);}));_tmp316[6]=_tmp737;});Cyc_PP_cat(_tag_fat(_tmp316,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1277
struct Cyc_Absyn_Exp*new_e;
# 1279
if(topt == 0)
new_e=e;else{
# 1282
new_e=({struct Cyc_Absyn_Exp*_tmp73E=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp73E,e,0U);});}
# 1284
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp31B[3U];({struct Cyc_PP_Doc*_tmp741=Cyc_PP_text(({const char*_tmp31C="malloc(";_tag_fat(_tmp31C,sizeof(char),8U);}));_tmp31B[0]=_tmp741;}),({struct Cyc_PP_Doc*_tmp740=Cyc_Absynpp_exp2doc(new_e);_tmp31B[1]=_tmp740;}),({struct Cyc_PP_Doc*_tmp73F=Cyc_PP_text(({const char*_tmp31D=")";_tag_fat(_tmp31D,sizeof(char),2U);}));_tmp31B[2]=_tmp73F;});Cyc_PP_cat(_tag_fat(_tmp31B,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1287
if(inline_call)
s=({struct Cyc_PP_Doc*_tmp31E[5U];({struct Cyc_PP_Doc*_tmp746=Cyc_PP_text(({const char*_tmp31F="rmalloc_inline(";_tag_fat(_tmp31F,sizeof(char),16U);}));_tmp31E[0]=_tmp746;}),({struct Cyc_PP_Doc*_tmp745=Cyc_Absynpp_exp2doc(rgnopt);_tmp31E[1]=_tmp745;}),({struct Cyc_PP_Doc*_tmp744=Cyc_PP_text(({const char*_tmp320=",";_tag_fat(_tmp320,sizeof(char),2U);}));_tmp31E[2]=_tmp744;}),({
struct Cyc_PP_Doc*_tmp743=Cyc_Absynpp_exp2doc(new_e);_tmp31E[3]=_tmp743;}),({struct Cyc_PP_Doc*_tmp742=Cyc_PP_text(({const char*_tmp321=")";_tag_fat(_tmp321,sizeof(char),2U);}));_tmp31E[4]=_tmp742;});Cyc_PP_cat(_tag_fat(_tmp31E,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1291
s=({struct Cyc_PP_Doc*_tmp322[5U];({struct Cyc_PP_Doc*_tmp74B=Cyc_PP_text(({const char*_tmp323="rmalloc(";_tag_fat(_tmp323,sizeof(char),9U);}));_tmp322[0]=_tmp74B;}),({struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_exp2doc(rgnopt);_tmp322[1]=_tmp74A;}),({struct Cyc_PP_Doc*_tmp749=Cyc_PP_text(({const char*_tmp324=",";_tag_fat(_tmp324,sizeof(char),2U);}));_tmp322[2]=_tmp749;}),({
struct Cyc_PP_Doc*_tmp748=Cyc_Absynpp_exp2doc(new_e);_tmp322[3]=_tmp748;}),({struct Cyc_PP_Doc*_tmp747=Cyc_PP_text(({const char*_tmp325=")";_tag_fat(_tmp325,sizeof(char),2U);}));_tmp322[4]=_tmp747;});Cyc_PP_cat(_tag_fat(_tmp322,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1295
goto _LL0;}case 35U: _LL51: _tmp25A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp25B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp25A;struct Cyc_Absyn_Exp*e2=_tmp25B;
# 1297
s=({struct Cyc_PP_Doc*_tmp326[3U];({struct Cyc_PP_Doc*_tmp74E=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp326[0]=_tmp74E;}),({struct Cyc_PP_Doc*_tmp74D=Cyc_PP_text(({const char*_tmp327=" :=: ";_tag_fat(_tmp327,sizeof(char),6U);}));_tmp326[1]=_tmp74D;}),({struct Cyc_PP_Doc*_tmp74C=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp326[2]=_tmp74C;});Cyc_PP_cat(_tag_fat(_tmp326,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 36U: _LL53: _tmp258=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp259=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL54: {struct Cyc_Core_Opt*n=_tmp258;struct Cyc_List_List*des=_tmp259;
# 1300
s=({struct _fat_ptr _tmp74F=({const char*_tmp328=",";_tag_fat(_tmp328,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp74F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _LL55: _tmp257=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL56: {struct Cyc_Absyn_Stmt*s2=_tmp257;
# 1303
s=({struct Cyc_PP_Doc*_tmp329[7U];({struct Cyc_PP_Doc*_tmp756=Cyc_PP_text(({const char*_tmp32A="(";_tag_fat(_tmp32A,sizeof(char),2U);}));_tmp329[0]=_tmp756;}),({struct Cyc_PP_Doc*_tmp755=Cyc_Absynpp_lb();_tmp329[1]=_tmp755;}),({struct Cyc_PP_Doc*_tmp754=Cyc_PP_blank_doc();_tmp329[2]=_tmp754;}),({
struct Cyc_PP_Doc*_tmp753=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_tmp329[3]=_tmp753;}),({
struct Cyc_PP_Doc*_tmp752=Cyc_PP_blank_doc();_tmp329[4]=_tmp752;}),({struct Cyc_PP_Doc*_tmp751=Cyc_Absynpp_rb();_tmp329[5]=_tmp751;}),({struct Cyc_PP_Doc*_tmp750=Cyc_PP_text(({const char*_tmp32B=")";_tag_fat(_tmp32B,sizeof(char),2U);}));_tmp329[6]=_tmp750;});Cyc_PP_cat(_tag_fat(_tmp329,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}}_LL0:;}
# 1308
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp32C[3U];({struct Cyc_PP_Doc*_tmp758=Cyc_PP_text(({const char*_tmp32D="(";_tag_fat(_tmp32D,sizeof(char),2U);}));_tmp32C[0]=_tmp758;}),_tmp32C[1]=s,({struct Cyc_PP_Doc*_tmp757=Cyc_PP_text(({const char*_tmp32E=")";_tag_fat(_tmp32E,sizeof(char),2U);}));_tmp32C[2]=_tmp757;});Cyc_PP_cat(_tag_fat(_tmp32C,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1313
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp32F=d;struct _fat_ptr*_tmp330;struct Cyc_Absyn_Exp*_tmp331;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp32F)->tag == 0U){_LL1: _tmp331=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp32F)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp331;
return({struct Cyc_PP_Doc*_tmp332[3U];({struct Cyc_PP_Doc*_tmp75B=Cyc_PP_text(({const char*_tmp333=".[";_tag_fat(_tmp333,sizeof(char),3U);}));_tmp332[0]=_tmp75B;}),({struct Cyc_PP_Doc*_tmp75A=Cyc_Absynpp_exp2doc(e);_tmp332[1]=_tmp75A;}),({struct Cyc_PP_Doc*_tmp759=Cyc_PP_text(({const char*_tmp334="]";_tag_fat(_tmp334,sizeof(char),2U);}));_tmp332[2]=_tmp759;});Cyc_PP_cat(_tag_fat(_tmp332,sizeof(struct Cyc_PP_Doc*),3U));});}}else{_LL3: _tmp330=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32F)->f1;_LL4: {struct _fat_ptr*v=_tmp330;
return({struct Cyc_PP_Doc*_tmp335[2U];({struct Cyc_PP_Doc*_tmp75D=Cyc_PP_text(({const char*_tmp336=".";_tag_fat(_tmp336,sizeof(char),2U);}));_tmp335[0]=_tmp75D;}),({struct Cyc_PP_Doc*_tmp75C=Cyc_PP_textptr(v);_tmp335[1]=_tmp75C;});Cyc_PP_cat(_tag_fat(_tmp335,sizeof(struct Cyc_PP_Doc*),2U));});}}_LL0:;}
# 1320
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp337[2U];({struct Cyc_PP_Doc*_tmp762=({struct _fat_ptr _tmp761=({const char*_tmp338="";_tag_fat(_tmp338,sizeof(char),1U);});struct _fat_ptr _tmp760=({const char*_tmp339="=";_tag_fat(_tmp339,sizeof(char),2U);});struct _fat_ptr _tmp75F=({const char*_tmp33A="=";_tag_fat(_tmp33A,sizeof(char),2U);});Cyc_PP_egroup(_tmp761,_tmp760,_tmp75F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp337[0]=_tmp762;}),({
struct Cyc_PP_Doc*_tmp75E=Cyc_Absynpp_exp2doc((*de).f2);_tmp337[1]=_tmp75E;});Cyc_PP_cat(_tag_fat(_tmp337,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1326
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _tmp765=({const char*_tmp33B="";_tag_fat(_tmp33B,sizeof(char),1U);});struct _fat_ptr _tmp764=({const char*_tmp33C="";_tag_fat(_tmp33C,sizeof(char),1U);});struct _fat_ptr _tmp763=({const char*_tmp33D=",";_tag_fat(_tmp33D,sizeof(char),2U);});Cyc_PP_group(_tmp765,_tmp764,_tmp763,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1330
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_tmp352=({unsigned _tmp351=28U + 1U;char*_tmp350=_cycalloc_atomic(_check_times(_tmp351,sizeof(char)));({{unsigned _tmp5BD=28U;unsigned i;for(i=0;i < _tmp5BD;++ i){_tmp350[i]='z';}_tmp350[_tmp5BD]=0;}0;});_tmp350;});_tag_fat(_tmp352,sizeof(char),29U);});
({struct _fat_ptr _tmp33E=_fat_ptr_plus(x,sizeof(char),27);char _tmp33F=*((char*)_check_fat_subscript(_tmp33E,sizeof(char),0U));char _tmp340='\000';if(_get_fat_size(_tmp33E,sizeof(char))== 1U &&(_tmp33F == 0 && _tmp340 != 0))_throw_arraybounds();*((char*)_tmp33E.curr)=_tmp340;});
({struct _fat_ptr _tmp341=_fat_ptr_plus(x,sizeof(char),26);char _tmp342=*((char*)_check_fat_subscript(_tmp341,sizeof(char),0U));char _tmp343='L';if(_get_fat_size(_tmp341,sizeof(char))== 1U &&(_tmp342 == 0 && _tmp343 != 0))_throw_arraybounds();*((char*)_tmp341.curr)=_tmp343;});
({struct _fat_ptr _tmp344=_fat_ptr_plus(x,sizeof(char),25);char _tmp345=*((char*)_check_fat_subscript(_tmp344,sizeof(char),0U));char _tmp346='L';if(_get_fat_size(_tmp344,sizeof(char))== 1U &&(_tmp345 == 0 && _tmp346 != 0))_throw_arraybounds();*((char*)_tmp344.curr)=_tmp346;});
({struct _fat_ptr _tmp347=_fat_ptr_plus(x,sizeof(char),24);char _tmp348=*((char*)_check_fat_subscript(_tmp347,sizeof(char),0U));char _tmp349='U';if(_get_fat_size(_tmp347,sizeof(char))== 1U &&(_tmp348 == 0 && _tmp349 != 0))_throw_arraybounds();*((char*)_tmp347.curr)=_tmp349;});
({struct _fat_ptr _tmp34A=_fat_ptr_plus(x,sizeof(char),23);char _tmp34B=*((char*)_check_fat_subscript(_tmp34A,sizeof(char),0U));char _tmp34C='0';if(_get_fat_size(_tmp34A,sizeof(char))== 1U &&(_tmp34B == 0 && _tmp34C != 0))_throw_arraybounds();*((char*)_tmp34A.curr)=_tmp34C;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _fat_ptr _tmp34D=_fat_ptr_plus(x,sizeof(char),index);char _tmp34E=*((char*)_check_fat_subscript(_tmp34D,sizeof(char),0U));char _tmp34F=c;if(_get_fat_size(_tmp34D,sizeof(char))== 1U &&(_tmp34E == 0 && _tmp34F != 0))_throw_arraybounds();*((char*)_tmp34D.curr)=_tmp34F;});
i=i / (unsigned long long)10;
-- index;}
# 1344
return(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1348
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp353=c;struct _fat_ptr _tmp354;struct _fat_ptr _tmp355;struct _fat_ptr _tmp356;long long _tmp358;enum Cyc_Absyn_Sign _tmp357;int _tmp35A;enum Cyc_Absyn_Sign _tmp359;short _tmp35C;enum Cyc_Absyn_Sign _tmp35B;struct _fat_ptr _tmp35D;char _tmp35F;enum Cyc_Absyn_Sign _tmp35E;switch((_tmp353.String_c).tag){case 2U: _LL1: _tmp35E=((_tmp353.Char_c).val).f1;_tmp35F=((_tmp353.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp35E;char ch=_tmp35F;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp362=({struct Cyc_String_pa_PrintArg_struct _tmp5BE;_tmp5BE.tag=0U,({struct _fat_ptr _tmp766=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp5BE.f1=_tmp766;});_tmp5BE;});void*_tmp360[1U];_tmp360[0]=& _tmp362;({struct _fat_ptr _tmp767=({const char*_tmp361="'%s'";_tag_fat(_tmp361,sizeof(char),5U);});Cyc_aprintf(_tmp767,_tag_fat(_tmp360,sizeof(void*),1U));});}));}case 3U: _LL3: _tmp35D=(_tmp353.Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp35D;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp365=({struct Cyc_String_pa_PrintArg_struct _tmp5BF;_tmp5BF.tag=0U,_tmp5BF.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5BF;});void*_tmp363[1U];_tmp363[0]=& _tmp365;({struct _fat_ptr _tmp768=({const char*_tmp364="L'%s'";_tag_fat(_tmp364,sizeof(char),6U);});Cyc_aprintf(_tmp768,_tag_fat(_tmp363,sizeof(void*),1U));});}));}case 4U: _LL5: _tmp35B=((_tmp353.Short_c).val).f1;_tmp35C=((_tmp353.Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp35B;short s=_tmp35C;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp368=({struct Cyc_Int_pa_PrintArg_struct _tmp5C0;_tmp5C0.tag=1U,_tmp5C0.f1=(unsigned long)((int)s);_tmp5C0;});void*_tmp366[1U];_tmp366[0]=& _tmp368;({struct _fat_ptr _tmp769=({const char*_tmp367="%d";_tag_fat(_tmp367,sizeof(char),3U);});Cyc_aprintf(_tmp769,_tag_fat(_tmp366,sizeof(void*),1U));});}));}case 5U: _LL7: _tmp359=((_tmp353.Int_c).val).f1;_tmp35A=((_tmp353.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign sn=_tmp359;int i=_tmp35A;
# 1354
if((int)sn == (int)1U)return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp36B=({struct Cyc_Int_pa_PrintArg_struct _tmp5C1;_tmp5C1.tag=1U,_tmp5C1.f1=(unsigned)i;_tmp5C1;});void*_tmp369[1U];_tmp369[0]=& _tmp36B;({struct _fat_ptr _tmp76A=({const char*_tmp36A="%uU";_tag_fat(_tmp36A,sizeof(char),4U);});Cyc_aprintf(_tmp76A,_tag_fat(_tmp369,sizeof(void*),1U));});}));else{
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp36E=({struct Cyc_Int_pa_PrintArg_struct _tmp5C2;_tmp5C2.tag=1U,_tmp5C2.f1=(unsigned long)i;_tmp5C2;});void*_tmp36C[1U];_tmp36C[0]=& _tmp36E;({struct _fat_ptr _tmp76B=({const char*_tmp36D="%d";_tag_fat(_tmp36D,sizeof(char),3U);});Cyc_aprintf(_tmp76B,_tag_fat(_tmp36C,sizeof(void*),1U));});}));}}case 6U: _LL9: _tmp357=((_tmp353.LongLong_c).val).f1;_tmp358=((_tmp353.LongLong_c).val).f2;_LLA: {enum Cyc_Absyn_Sign sg=_tmp357;long long i=_tmp358;
# 1358
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7U: _LLB: _tmp356=((_tmp353.Float_c).val).f1;_LLC: {struct _fat_ptr x=_tmp356;
return Cyc_PP_text(x);}case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp36F="NULL";_tag_fat(_tmp36F,sizeof(char),5U);}));case 8U: _LLF: _tmp355=(_tmp353.String_c).val;_LL10: {struct _fat_ptr s=_tmp355;
return({struct Cyc_PP_Doc*_tmp370[3U];({struct Cyc_PP_Doc*_tmp76E=Cyc_PP_text(({const char*_tmp371="\"";_tag_fat(_tmp371,sizeof(char),2U);}));_tmp370[0]=_tmp76E;}),({struct Cyc_PP_Doc*_tmp76D=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_tmp370[1]=_tmp76D;}),({struct Cyc_PP_Doc*_tmp76C=Cyc_PP_text(({const char*_tmp372="\"";_tag_fat(_tmp372,sizeof(char),2U);}));_tmp370[2]=_tmp76C;});Cyc_PP_cat(_tag_fat(_tmp370,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL11: _tmp354=(_tmp353.Wstring_c).val;_LL12: {struct _fat_ptr s=_tmp354;
return({struct Cyc_PP_Doc*_tmp373[3U];({struct Cyc_PP_Doc*_tmp771=Cyc_PP_text(({const char*_tmp374="L\"";_tag_fat(_tmp374,sizeof(char),3U);}));_tmp373[0]=_tmp771;}),({struct Cyc_PP_Doc*_tmp770=Cyc_PP_text(s);_tmp373[1]=_tmp770;}),({struct Cyc_PP_Doc*_tmp76F=Cyc_PP_text(({const char*_tmp375="\"";_tag_fat(_tmp375,sizeof(char),2U);}));_tmp373[2]=_tmp76F;});Cyc_PP_cat(_tag_fat(_tmp373,sizeof(struct Cyc_PP_Doc*),3U));});}}_LL0:;}
# 1366
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp774=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp378=({struct Cyc_String_pa_PrintArg_struct _tmp5C3;_tmp5C3.tag=0U,({struct _fat_ptr _tmp772=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5C3.f1=_tmp772;});_tmp5C3;});void*_tmp376[1U];_tmp376[0]=& _tmp378;({struct _fat_ptr _tmp773=({const char*_tmp377="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_tmp377,sizeof(char),47U);});Cyc_aprintf(_tmp773,_tag_fat(_tmp376,sizeof(void*),1U));});});_tmp379->f1=_tmp774;});_tmp379;}));
# 1372
return({struct Cyc_PP_Doc*_tmp37A[3U];({struct Cyc_PP_Doc*_tmp777=Cyc_PP_text(({const char*_tmp37B="numelts(";_tag_fat(_tmp37B,sizeof(char),9U);}));_tmp37A[0]=_tmp777;}),({struct Cyc_PP_Doc*_tmp776=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp37A[1]=_tmp776;}),({struct Cyc_PP_Doc*_tmp775=Cyc_PP_text(({const char*_tmp37C=")";_tag_fat(_tmp37C,sizeof(char),2U);}));_tmp37A[2]=_tmp775;});Cyc_PP_cat(_tag_fat(_tmp37A,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1374
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp77A=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37F=({struct Cyc_String_pa_PrintArg_struct _tmp5C4;_tmp5C4.tag=0U,({struct _fat_ptr _tmp778=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5C4.f1=_tmp778;});_tmp5C4;});void*_tmp37D[1U];_tmp37D[0]=& _tmp37F;({struct _fat_ptr _tmp779=({const char*_tmp37E="Absynpp::primapp2doc: %s with no args";_tag_fat(_tmp37E,sizeof(char),38U);});Cyc_aprintf(_tmp779,_tag_fat(_tmp37D,sizeof(void*),1U));});});_tmp380->f1=_tmp77A;});_tmp380;}));else{
# 1378
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp381[3U];_tmp381[0]=ps,({struct Cyc_PP_Doc*_tmp77B=Cyc_PP_text(({const char*_tmp382=" ";_tag_fat(_tmp382,sizeof(char),2U);}));_tmp381[1]=_tmp77B;}),_tmp381[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_tmp381,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp77E=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp385=({struct Cyc_String_pa_PrintArg_struct _tmp5C5;_tmp5C5.tag=0U,({struct _fat_ptr _tmp77C=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5C5.f1=_tmp77C;});_tmp5C5;});void*_tmp383[1U];_tmp383[0]=& _tmp385;({struct _fat_ptr _tmp77D=({const char*_tmp384="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_tmp384,sizeof(char),47U);});Cyc_aprintf(_tmp77D,_tag_fat(_tmp383,sizeof(void*),1U));});});_tmp386->f1=_tmp77E;});_tmp386;}));else{
# 1384
return({struct Cyc_PP_Doc*_tmp387[5U];_tmp387[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp780=Cyc_PP_text(({const char*_tmp388=" ";_tag_fat(_tmp388,sizeof(char),2U);}));_tmp387[1]=_tmp780;}),_tmp387[2]=ps,({struct Cyc_PP_Doc*_tmp77F=Cyc_PP_text(({const char*_tmp389=" ";_tag_fat(_tmp389,sizeof(char),2U);}));_tmp387[3]=_tmp77F;}),_tmp387[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_tmp387,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1388
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp38A=p;switch(_tmp38A){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp38B="+";_tag_fat(_tmp38B,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp38C="*";_tag_fat(_tmp38C,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp38D="-";_tag_fat(_tmp38D,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp38E="/";_tag_fat(_tmp38E,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp38F="\\%";_tag_fat(_tmp38F,sizeof(char),3U);}):({const char*_tmp390="%";_tag_fat(_tmp390,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp391="==";_tag_fat(_tmp391,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp392="!=";_tag_fat(_tmp392,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp393=">";_tag_fat(_tmp393,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp394="<";_tag_fat(_tmp394,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp395=">=";_tag_fat(_tmp395,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp396="<=";_tag_fat(_tmp396,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp397="!";_tag_fat(_tmp397,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp398="~";_tag_fat(_tmp398,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp399="&";_tag_fat(_tmp399,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp39A="|";_tag_fat(_tmp39A,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp39B="^";_tag_fat(_tmp39B,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp39C="<<";_tag_fat(_tmp39C,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp39D=">>";_tag_fat(_tmp39D,sizeof(char),3U);});case Cyc_Absyn_Numelts: _LL25: _LL26:
 return({const char*_tmp39E="numelts";_tag_fat(_tmp39E,sizeof(char),8U);});default: _LL27: _LL28:
 return({const char*_tmp39F="?";_tag_fat(_tmp39F,sizeof(char),2U);});}_LL0:;}
# 1413
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1417
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp3A0=s->r;void*_stmttmp17=_tmp3A0;void*_tmp3A1=_stmttmp17;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3A1)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1432 "absynpp.cyc"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_tmp3A2=d->r;void*_stmttmp18=_tmp3A2;void*_tmp3A3=_stmttmp18;struct Cyc_Absyn_Vardecl*_tmp3A4;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3A3)->tag == 0U){_LL1: _tmp3A4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3A3)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3A4;
# 1435
if(((int(*)(int(*compare)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*l,struct _tuple0*x))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple14 _tmp5C6;_tmp5C6.f1=1,({struct Cyc_List_List*_tmp781=({struct Cyc_List_List*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->hd=vd->name,_tmp3A5->tl=0;_tmp3A5;});_tmp5C6.f2=_tmp781;});_tmp5C6;});else{
# 1438
return({struct _tuple14 _tmp5C7;_tmp5C7.f1=0,({struct Cyc_List_List*_tmp782=({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->hd=vd->name,_tmp3A6->tl=varsinblock;_tmp3A6;});_tmp5C7.f2=_tmp782;});_tmp5C7;});}}}else{_LL3: _LL4:
# 1440
 return({struct _tuple14 _tmp5C8;_tmp5C8.f1=0,_tmp5C8.f2=varsinblock;_tmp5C8;});}_LL0:;}
# 1444
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_tmp3A7[8U];({struct Cyc_PP_Doc*_tmp78A=Cyc_PP_text(({const char*_tmp3A8="(";_tag_fat(_tmp3A8,sizeof(char),2U);}));_tmp3A7[0]=_tmp78A;}),({struct Cyc_PP_Doc*_tmp789=Cyc_Absynpp_lb();_tmp3A7[1]=_tmp789;}),({struct Cyc_PP_Doc*_tmp788=Cyc_PP_blank_doc();_tmp3A7[2]=_tmp788;}),({struct Cyc_PP_Doc*_tmp787=Cyc_PP_nest(2,d);_tmp3A7[3]=_tmp787;}),({struct Cyc_PP_Doc*_tmp786=Cyc_PP_line_doc();_tmp3A7[4]=_tmp786;}),({struct Cyc_PP_Doc*_tmp785=Cyc_Absynpp_rb();_tmp3A7[5]=_tmp785;}),({
struct Cyc_PP_Doc*_tmp784=Cyc_PP_text(({const char*_tmp3A9=");";_tag_fat(_tmp3A9,sizeof(char),3U);}));_tmp3A7[6]=_tmp784;}),({struct Cyc_PP_Doc*_tmp783=Cyc_PP_line_doc();_tmp3A7[7]=_tmp783;});Cyc_PP_cat(_tag_fat(_tmp3A7,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1449
return({struct Cyc_PP_Doc*_tmp3AA[6U];({struct Cyc_PP_Doc*_tmp790=Cyc_Absynpp_lb();_tmp3AA[0]=_tmp790;}),({struct Cyc_PP_Doc*_tmp78F=Cyc_PP_blank_doc();_tmp3AA[1]=_tmp78F;}),({struct Cyc_PP_Doc*_tmp78E=Cyc_PP_nest(2,d);_tmp3AA[2]=_tmp78E;}),({struct Cyc_PP_Doc*_tmp78D=Cyc_PP_line_doc();_tmp3AA[3]=_tmp78D;}),({struct Cyc_PP_Doc*_tmp78C=Cyc_Absynpp_rb();_tmp3AA[4]=_tmp78C;}),({struct Cyc_PP_Doc*_tmp78B=Cyc_PP_line_doc();_tmp3AA[5]=_tmp78B;});Cyc_PP_cat(_tag_fat(_tmp3AA,sizeof(struct Cyc_PP_Doc*),6U));});}}
# 1452
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_tmp3AB=st->r;void*_stmttmp19=_tmp3AB;void*_tmp3AC=_stmttmp19;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Stmt*_tmp3AD;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Stmt*_tmp3AF;struct Cyc_Absyn_Stmt*_tmp3B2;struct _fat_ptr*_tmp3B1;struct Cyc_Absyn_Stmt*_tmp3B4;struct Cyc_Absyn_Decl*_tmp3B3;struct Cyc_List_List*_tmp3B5;struct Cyc_List_List*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B6;struct Cyc_Absyn_Stmt*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3B8;struct _fat_ptr*_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Stmt*_tmp3C1;struct Cyc_Absyn_Stmt*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Stmt*_tmp3C4;struct Cyc_Absyn_Stmt*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C5;switch(*((int*)_tmp3AC)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3C6=";";_tag_fat(_tmp3C6,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp3C5=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp3C5;
s=({struct Cyc_PP_Doc*_tmp3C7[2U];({struct Cyc_PP_Doc*_tmp792=Cyc_Absynpp_exp2doc(e);_tmp3C7[0]=_tmp792;}),({struct Cyc_PP_Doc*_tmp791=Cyc_PP_text(({const char*_tmp3C8=";";_tag_fat(_tmp3C8,sizeof(char),2U);}));_tmp3C7[1]=_tmp791;});Cyc_PP_cat(_tag_fat(_tmp3C7,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 2U: _LL5: _tmp3C3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_tmp3C4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp3C3;struct Cyc_Absyn_Stmt*s2=_tmp3C4;
# 1458
if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp3C9[2U];({struct Cyc_PP_Doc*_tmp796=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3C9[0]=_tmp796;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_tmp795=({int _tmp794=stmtexp;Cyc_Absynpp_block(_tmp794,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp3C9[1]=_tmp795;}):({
struct Cyc_PP_Doc*_tmp793=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp3C9[1]=_tmp793;});Cyc_PP_cat(_tag_fat(_tmp3C9,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(Cyc_Absynpp_is_declaration(s2))
s=({struct Cyc_PP_Doc*_tmp3CA[3U];({struct Cyc_PP_Doc*_tmp79A=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp3CA[0]=_tmp79A;}),({
struct Cyc_PP_Doc*_tmp799=Cyc_PP_line_doc();_tmp3CA[1]=_tmp799;}),({
struct Cyc_PP_Doc*_tmp798=({int _tmp797=stmtexp;Cyc_Absynpp_block(_tmp797,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp3CA[2]=_tmp798;});Cyc_PP_cat(_tag_fat(_tmp3CA,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1469
s=({struct Cyc_PP_Doc*_tmp3CB[3U];({struct Cyc_PP_Doc*_tmp79D=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp3CB[0]=_tmp79D;}),({struct Cyc_PP_Doc*_tmp79C=Cyc_PP_line_doc();_tmp3CB[1]=_tmp79C;}),({
struct Cyc_PP_Doc*_tmp79B=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp3CB[2]=_tmp79B;});Cyc_PP_cat(_tag_fat(_tmp3CB,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1473
s=({struct Cyc_PP_Doc*_tmp3CC[3U];({struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_stmt2doc(s1,0,0);_tmp3CC[0]=_tmp7A0;}),({struct Cyc_PP_Doc*_tmp79F=Cyc_PP_line_doc();_tmp3CC[1]=_tmp79F;}),({struct Cyc_PP_Doc*_tmp79E=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_tmp3CC[2]=_tmp79E;});Cyc_PP_cat(_tag_fat(_tmp3CC,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 3U: _LL7: _tmp3C2=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp3C2;
# 1476
if(eopt == 0)
s=Cyc_PP_text(({const char*_tmp3CD="return;";_tag_fat(_tmp3CD,sizeof(char),8U);}));else{
# 1479
s=({struct Cyc_PP_Doc*_tmp3CE[3U];({struct Cyc_PP_Doc*_tmp7A4=Cyc_PP_text(({const char*_tmp3CF="return ";_tag_fat(_tmp3CF,sizeof(char),8U);}));_tmp3CE[0]=_tmp7A4;}),
eopt == 0?({struct Cyc_PP_Doc*_tmp7A3=Cyc_PP_nil_doc();_tmp3CE[1]=_tmp7A3;}):({struct Cyc_PP_Doc*_tmp7A2=Cyc_Absynpp_exp2doc(eopt);_tmp3CE[1]=_tmp7A2;}),({
struct Cyc_PP_Doc*_tmp7A1=Cyc_PP_text(({const char*_tmp3D0=";";_tag_fat(_tmp3D0,sizeof(char),2U);}));_tmp3CE[2]=_tmp7A1;});Cyc_PP_cat(_tag_fat(_tmp3CE,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 4U: _LL9: _tmp3BF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_tmp3C0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2;_tmp3C1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp3BF;struct Cyc_Absyn_Stmt*s1=_tmp3C0;struct Cyc_Absyn_Stmt*s2=_tmp3C1;
# 1484
int print_else;
{void*_tmp3D1=s2->r;void*_stmttmp1A=_tmp3D1;void*_tmp3D2=_stmttmp1A;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3D2)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1489
s=({struct Cyc_PP_Doc*_tmp3D3[5U];({struct Cyc_PP_Doc*_tmp7AD=Cyc_PP_text(({const char*_tmp3D4="if (";_tag_fat(_tmp3D4,sizeof(char),5U);}));_tmp3D3[0]=_tmp7AD;}),({
struct Cyc_PP_Doc*_tmp7AC=Cyc_Absynpp_exp2doc(e);_tmp3D3[1]=_tmp7AC;}),({
struct Cyc_PP_Doc*_tmp7AB=Cyc_PP_text(({const char*_tmp3D5=") ";_tag_fat(_tmp3D5,sizeof(char),3U);}));_tmp3D3[2]=_tmp7AB;}),({
struct Cyc_PP_Doc*_tmp7AA=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3D3[3]=_tmp7AA;}),
print_else?({
struct Cyc_PP_Doc*_tmp7A9=({struct Cyc_PP_Doc*_tmp3D6[3U];({struct Cyc_PP_Doc*_tmp7A8=Cyc_PP_line_doc();_tmp3D6[0]=_tmp7A8;}),({
struct Cyc_PP_Doc*_tmp7A7=Cyc_PP_text(({const char*_tmp3D7="else ";_tag_fat(_tmp3D7,sizeof(char),6U);}));_tmp3D6[1]=_tmp7A7;}),({
struct Cyc_PP_Doc*_tmp7A6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_tmp3D6[2]=_tmp7A6;});Cyc_PP_cat(_tag_fat(_tmp3D6,sizeof(struct Cyc_PP_Doc*),3U));});
# 1494
_tmp3D3[4]=_tmp7A9;}):({
# 1497
struct Cyc_PP_Doc*_tmp7A5=Cyc_PP_nil_doc();_tmp3D3[4]=_tmp7A5;});Cyc_PP_cat(_tag_fat(_tmp3D3,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp3BD=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1).f1;_tmp3BE=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp3BD;struct Cyc_Absyn_Stmt*s1=_tmp3BE;
# 1500
s=({struct Cyc_PP_Doc*_tmp3D8[4U];({struct Cyc_PP_Doc*_tmp7B1=Cyc_PP_text(({const char*_tmp3D9="while (";_tag_fat(_tmp3D9,sizeof(char),8U);}));_tmp3D8[0]=_tmp7B1;}),({
struct Cyc_PP_Doc*_tmp7B0=Cyc_Absynpp_exp2doc(e);_tmp3D8[1]=_tmp7B0;}),({
struct Cyc_PP_Doc*_tmp7AF=Cyc_PP_text(({const char*_tmp3DA=") ";_tag_fat(_tmp3DA,sizeof(char),3U);}));_tmp3D8[2]=_tmp7AF;}),({
struct Cyc_PP_Doc*_tmp7AE=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3D8[3]=_tmp7AE;});Cyc_PP_cat(_tag_fat(_tmp3D8,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp3DB="break;";_tag_fat(_tmp3DB,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp3DC="continue;";_tag_fat(_tmp3DC,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp3BC=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_LL12: {struct _fat_ptr*x=_tmp3BC;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3DF=({struct Cyc_String_pa_PrintArg_struct _tmp5C9;_tmp5C9.tag=0U,_tmp5C9.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp5C9;});void*_tmp3DD[1U];_tmp3DD[0]=& _tmp3DF;({struct _fat_ptr _tmp7B2=({const char*_tmp3DE="goto %s;";_tag_fat(_tmp3DE,sizeof(char),9U);});Cyc_aprintf(_tmp7B2,_tag_fat(_tmp3DD,sizeof(void*),1U));});}));goto _LL0;}case 9U: _LL13: _tmp3B8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_tmp3B9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2).f1;_tmp3BA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f3).f1;_tmp3BB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f4;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp3B8;struct Cyc_Absyn_Exp*e2=_tmp3B9;struct Cyc_Absyn_Exp*e3=_tmp3BA;struct Cyc_Absyn_Stmt*s1=_tmp3BB;
# 1509
s=({struct Cyc_PP_Doc*_tmp3E0[8U];({struct Cyc_PP_Doc*_tmp7BA=Cyc_PP_text(({const char*_tmp3E1="for(";_tag_fat(_tmp3E1,sizeof(char),5U);}));_tmp3E0[0]=_tmp7BA;}),({
struct Cyc_PP_Doc*_tmp7B9=Cyc_Absynpp_exp2doc(e1);_tmp3E0[1]=_tmp7B9;}),({
struct Cyc_PP_Doc*_tmp7B8=Cyc_PP_text(({const char*_tmp3E2="; ";_tag_fat(_tmp3E2,sizeof(char),3U);}));_tmp3E0[2]=_tmp7B8;}),({
struct Cyc_PP_Doc*_tmp7B7=Cyc_Absynpp_exp2doc(e2);_tmp3E0[3]=_tmp7B7;}),({
struct Cyc_PP_Doc*_tmp7B6=Cyc_PP_text(({const char*_tmp3E3="; ";_tag_fat(_tmp3E3,sizeof(char),3U);}));_tmp3E0[4]=_tmp7B6;}),({
struct Cyc_PP_Doc*_tmp7B5=Cyc_Absynpp_exp2doc(e3);_tmp3E0[5]=_tmp7B5;}),({
struct Cyc_PP_Doc*_tmp7B4=Cyc_PP_text(({const char*_tmp3E4=") ";_tag_fat(_tmp3E4,sizeof(char),3U);}));_tmp3E0[6]=_tmp7B4;}),({
struct Cyc_PP_Doc*_tmp7B3=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3E0[7]=_tmp7B3;});Cyc_PP_cat(_tag_fat(_tmp3E0,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 10U: _LL15: _tmp3B6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_tmp3B7=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp3B6;struct Cyc_List_List*ss=_tmp3B7;
# 1519
s=({struct Cyc_PP_Doc*_tmp3E5[8U];({struct Cyc_PP_Doc*_tmp7C2=Cyc_PP_text(({const char*_tmp3E6="switch (";_tag_fat(_tmp3E6,sizeof(char),9U);}));_tmp3E5[0]=_tmp7C2;}),({
struct Cyc_PP_Doc*_tmp7C1=Cyc_Absynpp_exp2doc(e);_tmp3E5[1]=_tmp7C1;}),({
struct Cyc_PP_Doc*_tmp7C0=Cyc_PP_text(({const char*_tmp3E7=") ";_tag_fat(_tmp3E7,sizeof(char),3U);}));_tmp3E5[2]=_tmp7C0;}),({
struct Cyc_PP_Doc*_tmp7BF=Cyc_Absynpp_lb();_tmp3E5[3]=_tmp7BF;}),({
struct Cyc_PP_Doc*_tmp7BE=Cyc_PP_line_doc();_tmp3E5[4]=_tmp7BE;}),({
struct Cyc_PP_Doc*_tmp7BD=Cyc_Absynpp_switchclauses2doc(ss);_tmp3E5[5]=_tmp7BD;}),({
struct Cyc_PP_Doc*_tmp7BC=Cyc_PP_line_doc();_tmp3E5[6]=_tmp7BC;}),({
struct Cyc_PP_Doc*_tmp7BB=Cyc_Absynpp_rb();_tmp3E5[7]=_tmp7BB;});Cyc_PP_cat(_tag_fat(_tmp3E5,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp3E8="fallthru;";_tag_fat(_tmp3E8,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp3B5=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_LL1A: {struct Cyc_List_List*es=_tmp3B5;
# 1530
s=({struct Cyc_PP_Doc*_tmp3E9[3U];({struct Cyc_PP_Doc*_tmp7C5=Cyc_PP_text(({const char*_tmp3EA="fallthru(";_tag_fat(_tmp3EA,sizeof(char),10U);}));_tmp3E9[0]=_tmp7C5;}),({struct Cyc_PP_Doc*_tmp7C4=Cyc_Absynpp_exps2doc_prec(20,es);_tmp3E9[1]=_tmp7C4;}),({struct Cyc_PP_Doc*_tmp7C3=Cyc_PP_text(({const char*_tmp3EB=");";_tag_fat(_tmp3EB,sizeof(char),3U);}));_tmp3E9[2]=_tmp7C3;});Cyc_PP_cat(_tag_fat(_tmp3E9,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 12U: _LL1B: _tmp3B3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_tmp3B4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp3B3;struct Cyc_Absyn_Stmt*s1=_tmp3B4;
# 1532
struct _tuple14 _tmp3EC=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple14 _stmttmp1B=_tmp3EC;struct _tuple14 _tmp3ED=_stmttmp1B;struct Cyc_List_List*_tmp3EF;int _tmp3EE;_LL29: _tmp3EE=_tmp3ED.f1;_tmp3EF=_tmp3ED.f2;_LL2A: {int newblock=_tmp3EE;struct Cyc_List_List*newvarsinblock=_tmp3EF;
s=({struct Cyc_PP_Doc*_tmp3F0[3U];({struct Cyc_PP_Doc*_tmp7C8=Cyc_Absynpp_decl2doc(d);_tmp3F0[0]=_tmp7C8;}),({struct Cyc_PP_Doc*_tmp7C7=Cyc_PP_line_doc();_tmp3F0[1]=_tmp7C7;}),({struct Cyc_PP_Doc*_tmp7C6=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_tmp3F0[2]=_tmp7C6;});Cyc_PP_cat(_tag_fat(_tmp3F0,sizeof(struct Cyc_PP_Doc*),3U));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}}case 13U: _LL1D: _tmp3B1=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_tmp3B2=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2;_LL1E: {struct _fat_ptr*x=_tmp3B1;struct Cyc_Absyn_Stmt*s1=_tmp3B2;
# 1537
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp3F1[3U];({struct Cyc_PP_Doc*_tmp7CC=Cyc_PP_textptr(x);_tmp3F1[0]=_tmp7CC;}),({struct Cyc_PP_Doc*_tmp7CB=Cyc_PP_text(({const char*_tmp3F2=": ";_tag_fat(_tmp3F2,sizeof(char),3U);}));_tmp3F1[1]=_tmp7CB;}),({struct Cyc_PP_Doc*_tmp7CA=({int _tmp7C9=stmtexp;Cyc_Absynpp_block(_tmp7C9,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_tmp3F1[2]=_tmp7CA;});Cyc_PP_cat(_tag_fat(_tmp3F1,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1540
s=({struct Cyc_PP_Doc*_tmp3F3[3U];({struct Cyc_PP_Doc*_tmp7CF=Cyc_PP_textptr(x);_tmp3F3[0]=_tmp7CF;}),({struct Cyc_PP_Doc*_tmp7CE=Cyc_PP_text(({const char*_tmp3F4=": ";_tag_fat(_tmp3F4,sizeof(char),3U);}));_tmp3F3[1]=_tmp7CE;}),({struct Cyc_PP_Doc*_tmp7CD=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_tmp3F3[2]=_tmp7CD;});Cyc_PP_cat(_tag_fat(_tmp3F3,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 14U: _LL1F: _tmp3AF=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_tmp3B0=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2).f1;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp3AF;struct Cyc_Absyn_Exp*e=_tmp3B0;
# 1543
s=({struct Cyc_PP_Doc*_tmp3F5[5U];({struct Cyc_PP_Doc*_tmp7D4=Cyc_PP_text(({const char*_tmp3F6="do ";_tag_fat(_tmp3F6,sizeof(char),4U);}));_tmp3F5[0]=_tmp7D4;}),({
struct Cyc_PP_Doc*_tmp7D3=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3F5[1]=_tmp7D3;}),({
struct Cyc_PP_Doc*_tmp7D2=Cyc_PP_text(({const char*_tmp3F7=" while (";_tag_fat(_tmp3F7,sizeof(char),9U);}));_tmp3F5[2]=_tmp7D2;}),({
struct Cyc_PP_Doc*_tmp7D1=Cyc_Absynpp_exp2doc(e);_tmp3F5[3]=_tmp7D1;}),({
struct Cyc_PP_Doc*_tmp7D0=Cyc_PP_text(({const char*_tmp3F8=");";_tag_fat(_tmp3F8,sizeof(char),3U);}));_tmp3F5[4]=_tmp7D0;});Cyc_PP_cat(_tag_fat(_tmp3F5,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}default: _LL21: _tmp3AD=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f1;_tmp3AE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3AC)->f2;_LL22: {struct Cyc_Absyn_Stmt*s1=_tmp3AD;struct Cyc_List_List*ss=_tmp3AE;
# 1550
s=({struct Cyc_PP_Doc*_tmp3F9[4U];({struct Cyc_PP_Doc*_tmp7D8=Cyc_PP_text(({const char*_tmp3FA="try ";_tag_fat(_tmp3FA,sizeof(char),5U);}));_tmp3F9[0]=_tmp7D8;}),({
struct Cyc_PP_Doc*_tmp7D7=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3F9[1]=_tmp7D7;}),({
struct Cyc_PP_Doc*_tmp7D6=Cyc_PP_text(({const char*_tmp3FB=" catch ";_tag_fat(_tmp3FB,sizeof(char),8U);}));_tmp3F9[2]=_tmp7D6;}),({
struct Cyc_PP_Doc*_tmp7D5=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_tmp3F9[3]=_tmp7D5;});Cyc_PP_cat(_tag_fat(_tmp3F9,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}_LL0:;}
# 1556
return s;}
# 1559
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3FC=p->r;void*_stmttmp1C=_tmp3FC;void*_tmp3FD=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp3FE;int _tmp401;struct Cyc_List_List*_tmp400;struct Cyc_Absyn_Datatypefield*_tmp3FF;struct Cyc_Absyn_Datatypefield*_tmp402;struct Cyc_Absyn_Enumfield*_tmp403;struct Cyc_Absyn_Enumfield*_tmp404;int _tmp407;struct Cyc_List_List*_tmp406;struct Cyc_List_List*_tmp405;int _tmp40B;struct Cyc_List_List*_tmp40A;struct Cyc_List_List*_tmp409;union Cyc_Absyn_AggrInfo _tmp408;int _tmp40E;struct Cyc_List_List*_tmp40D;struct _tuple0*_tmp40C;struct _tuple0*_tmp40F;struct Cyc_Absyn_Pat*_tmp411;struct Cyc_Absyn_Vardecl*_tmp410;struct Cyc_Absyn_Vardecl*_tmp412;struct Cyc_Absyn_Pat*_tmp413;int _tmp415;struct Cyc_List_List*_tmp414;struct Cyc_Absyn_Vardecl*_tmp417;struct Cyc_Absyn_Tvar*_tmp416;struct Cyc_Absyn_Vardecl*_tmp419;struct Cyc_Absyn_Tvar*_tmp418;struct Cyc_Absyn_Pat*_tmp41B;struct Cyc_Absyn_Vardecl*_tmp41A;struct Cyc_Absyn_Vardecl*_tmp41C;struct _fat_ptr _tmp41D;char _tmp41E;int _tmp420;enum Cyc_Absyn_Sign _tmp41F;switch(*((int*)_tmp3FD)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp421="_";_tag_fat(_tmp421,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp422="NULL";_tag_fat(_tmp422,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp41F=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_tmp420=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp41F;int i=_tmp420;
# 1565
if((int)sg != (int)1U)
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp425=({struct Cyc_Int_pa_PrintArg_struct _tmp5CA;_tmp5CA.tag=1U,_tmp5CA.f1=(unsigned long)i;_tmp5CA;});void*_tmp423[1U];_tmp423[0]=& _tmp425;({struct _fat_ptr _tmp7D9=({const char*_tmp424="%d";_tag_fat(_tmp424,sizeof(char),3U);});Cyc_aprintf(_tmp7D9,_tag_fat(_tmp423,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp428=({struct Cyc_Int_pa_PrintArg_struct _tmp5CB;_tmp5CB.tag=1U,_tmp5CB.f1=(unsigned)i;_tmp5CB;});void*_tmp426[1U];_tmp426[0]=& _tmp428;({struct _fat_ptr _tmp7DA=({const char*_tmp427="%u";_tag_fat(_tmp427,sizeof(char),3U);});Cyc_aprintf(_tmp7DA,_tag_fat(_tmp426,sizeof(void*),1U));});}));}
goto _LL0;}case 11U: _LL7: _tmp41E=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_LL8: {char ch=_tmp41E;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp42B=({struct Cyc_String_pa_PrintArg_struct _tmp5CC;_tmp5CC.tag=0U,({struct _fat_ptr _tmp7DB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp5CC.f1=_tmp7DB;});_tmp5CC;});void*_tmp429[1U];_tmp429[0]=& _tmp42B;({struct _fat_ptr _tmp7DC=({const char*_tmp42A="'%s'";_tag_fat(_tmp42A,sizeof(char),5U);});Cyc_aprintf(_tmp7DC,_tag_fat(_tmp429,sizeof(void*),1U));});}));goto _LL0;}case 12U: _LL9: _tmp41D=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_LLA: {struct _fat_ptr x=_tmp41D;
s=Cyc_PP_text(x);goto _LL0;}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2)->r)->tag == 0U){_LLB: _tmp41C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp41C;
# 1572
s=Cyc_Absynpp_qvar2doc(vd->name);goto _LL0;}}else{_LLD: _tmp41A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_tmp41B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp41A;struct Cyc_Absyn_Pat*p2=_tmp41B;
# 1574
s=({struct Cyc_PP_Doc*_tmp42C[3U];({struct Cyc_PP_Doc*_tmp7DF=Cyc_Absynpp_qvar2doc(vd->name);_tmp42C[0]=_tmp7DF;}),({struct Cyc_PP_Doc*_tmp7DE=Cyc_PP_text(({const char*_tmp42D=" as ";_tag_fat(_tmp42D,sizeof(char),5U);}));_tmp42C[1]=_tmp7DE;}),({struct Cyc_PP_Doc*_tmp7DD=Cyc_Absynpp_pat2doc(p2);_tmp42C[2]=_tmp7DD;});Cyc_PP_cat(_tag_fat(_tmp42C,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 2U: _LLF: _tmp418=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_tmp419=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp418;struct Cyc_Absyn_Vardecl*vd=_tmp419;
# 1576
s=({struct Cyc_PP_Doc*_tmp42E[5U];({struct Cyc_PP_Doc*_tmp7E4=Cyc_PP_text(({const char*_tmp42F="alias";_tag_fat(_tmp42F,sizeof(char),6U);}));_tmp42E[0]=_tmp7E4;}),({struct Cyc_PP_Doc*_tmp7E3=Cyc_PP_text(({const char*_tmp430=" <";_tag_fat(_tmp430,sizeof(char),3U);}));_tmp42E[1]=_tmp7E3;}),({struct Cyc_PP_Doc*_tmp7E2=Cyc_Absynpp_tvar2doc(tv);_tmp42E[2]=_tmp7E2;}),({struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_text(({const char*_tmp431="> ";_tag_fat(_tmp431,sizeof(char),3U);}));_tmp42E[3]=_tmp7E1;}),({
struct Cyc_PP_Doc*_tmp7E0=Cyc_Absynpp_vardecl2doc(vd);_tmp42E[4]=_tmp7E0;});Cyc_PP_cat(_tag_fat(_tmp42E,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 4U: _LL11: _tmp416=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_tmp417=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp416;struct Cyc_Absyn_Vardecl*vd=_tmp417;
# 1580
s=({struct Cyc_PP_Doc*_tmp432[4U];({struct Cyc_PP_Doc*_tmp7E8=Cyc_Absynpp_qvar2doc(vd->name);_tmp432[0]=_tmp7E8;}),({struct Cyc_PP_Doc*_tmp7E7=Cyc_PP_text(({const char*_tmp433="<";_tag_fat(_tmp433,sizeof(char),2U);}));_tmp432[1]=_tmp7E7;}),({struct Cyc_PP_Doc*_tmp7E6=Cyc_Absynpp_tvar2doc(tv);_tmp432[2]=_tmp7E6;}),({struct Cyc_PP_Doc*_tmp7E5=Cyc_PP_text(({const char*_tmp434=">";_tag_fat(_tmp434,sizeof(char),2U);}));_tmp432[3]=_tmp7E5;});Cyc_PP_cat(_tag_fat(_tmp432,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 5U: _LL13: _tmp414=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_tmp415=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LL14: {struct Cyc_List_List*ts=_tmp414;int dots=_tmp415;
# 1583
s=({struct Cyc_PP_Doc*_tmp435[4U];({struct Cyc_PP_Doc*_tmp7EE=Cyc_Absynpp_dollar();_tmp435[0]=_tmp7EE;}),({struct Cyc_PP_Doc*_tmp7ED=Cyc_PP_text(({const char*_tmp436="(";_tag_fat(_tmp436,sizeof(char),2U);}));_tmp435[1]=_tmp7ED;}),({struct Cyc_PP_Doc*_tmp7EC=({struct _fat_ptr _tmp7EB=({const char*_tmp437=",";_tag_fat(_tmp437,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp7EB,ts);});_tmp435[2]=_tmp7EC;}),
dots?({struct Cyc_PP_Doc*_tmp7EA=Cyc_PP_text(({const char*_tmp438=", ...)";_tag_fat(_tmp438,sizeof(char),7U);}));_tmp435[3]=_tmp7EA;}):({struct Cyc_PP_Doc*_tmp7E9=Cyc_PP_text(({const char*_tmp439=")";_tag_fat(_tmp439,sizeof(char),2U);}));_tmp435[3]=_tmp7E9;});Cyc_PP_cat(_tag_fat(_tmp435,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LL15: _tmp413=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_LL16: {struct Cyc_Absyn_Pat*p2=_tmp413;
# 1587
s=({struct Cyc_PP_Doc*_tmp43A[2U];({struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_text(({const char*_tmp43B="&";_tag_fat(_tmp43B,sizeof(char),2U);}));_tmp43A[0]=_tmp7F0;}),({struct Cyc_PP_Doc*_tmp7EF=Cyc_Absynpp_pat2doc(p2);_tmp43A[1]=_tmp7EF;});Cyc_PP_cat(_tag_fat(_tmp43A,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2)->r)->tag == 0U){_LL17: _tmp412=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp412;
# 1590
s=({struct Cyc_PP_Doc*_tmp43C[2U];({struct Cyc_PP_Doc*_tmp7F2=Cyc_PP_text(({const char*_tmp43D="*";_tag_fat(_tmp43D,sizeof(char),2U);}));_tmp43C[0]=_tmp7F2;}),({struct Cyc_PP_Doc*_tmp7F1=Cyc_Absynpp_qvar2doc(vd->name);_tmp43C[1]=_tmp7F1;});Cyc_PP_cat(_tag_fat(_tmp43C,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}else{_LL19: _tmp410=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_tmp411=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp410;struct Cyc_Absyn_Pat*p2=_tmp411;
# 1593
s=({struct Cyc_PP_Doc*_tmp43E[4U];({struct Cyc_PP_Doc*_tmp7F6=Cyc_PP_text(({const char*_tmp43F="*";_tag_fat(_tmp43F,sizeof(char),2U);}));_tmp43E[0]=_tmp7F6;}),({struct Cyc_PP_Doc*_tmp7F5=Cyc_Absynpp_qvar2doc(vd->name);_tmp43E[1]=_tmp7F5;}),({struct Cyc_PP_Doc*_tmp7F4=Cyc_PP_text(({const char*_tmp440=" as ";_tag_fat(_tmp440,sizeof(char),5U);}));_tmp43E[2]=_tmp7F4;}),({struct Cyc_PP_Doc*_tmp7F3=Cyc_Absynpp_pat2doc(p2);_tmp43E[3]=_tmp7F3;});Cyc_PP_cat(_tag_fat(_tmp43E,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}case 15U: _LL1B: _tmp40F=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_LL1C: {struct _tuple0*q=_tmp40F;
# 1596
s=Cyc_Absynpp_qvar2doc(q);
goto _LL0;}case 16U: _LL1D: _tmp40C=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_tmp40D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_tmp40E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3FD)->f3;_LL1E: {struct _tuple0*q=_tmp40C;struct Cyc_List_List*ps=_tmp40D;int dots=_tmp40E;
# 1599
struct _fat_ptr term=dots?({const char*_tmp444=", ...)";_tag_fat(_tmp444,sizeof(char),7U);}):({const char*_tmp445=")";_tag_fat(_tmp445,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp441[2U];({struct Cyc_PP_Doc*_tmp7FB=Cyc_Absynpp_qvar2doc(q);_tmp441[0]=_tmp7FB;}),({struct Cyc_PP_Doc*_tmp7FA=({struct _fat_ptr _tmp7F9=({const char*_tmp442="(";_tag_fat(_tmp442,sizeof(char),2U);});struct _fat_ptr _tmp7F8=term;struct _fat_ptr _tmp7F7=({const char*_tmp443=",";_tag_fat(_tmp443,sizeof(char),2U);});Cyc_PP_group(_tmp7F9,_tmp7F8,_tmp7F7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp441[1]=_tmp7FA;});Cyc_PP_cat(_tag_fat(_tmp441,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1 != 0){_LL1F: _tmp408=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_tmp409=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_tmp40A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3FD)->f3;_tmp40B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3FD)->f4;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp408;struct Cyc_List_List*exists=_tmp409;struct Cyc_List_List*dps=_tmp40A;int dots=_tmp40B;
# 1603
struct _fat_ptr term=dots?({const char*_tmp44F=", ...}";_tag_fat(_tmp44F,sizeof(char),7U);}):({const char*_tmp450="}";_tag_fat(_tmp450,sizeof(char),2U);});
struct _tuple11 _tmp446=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp1D=_tmp446;struct _tuple11 _tmp447=_stmttmp1D;struct _tuple0*_tmp448;_LL2E: _tmp448=_tmp447.f2;_LL2F: {struct _tuple0*n=_tmp448;
s=({struct Cyc_PP_Doc*_tmp449[4U];({struct Cyc_PP_Doc*_tmp805=Cyc_Absynpp_qvar2doc(n);_tmp449[0]=_tmp805;}),({struct Cyc_PP_Doc*_tmp804=Cyc_Absynpp_lb();_tmp449[1]=_tmp804;}),({
struct Cyc_PP_Doc*_tmp803=({struct _fat_ptr _tmp802=({const char*_tmp44A="[";_tag_fat(_tmp44A,sizeof(char),2U);});struct _fat_ptr _tmp801=({const char*_tmp44B="]";_tag_fat(_tmp44B,sizeof(char),2U);});struct _fat_ptr _tmp800=({const char*_tmp44C=",";_tag_fat(_tmp44C,sizeof(char),2U);});Cyc_PP_egroup(_tmp802,_tmp801,_tmp800,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp449[2]=_tmp803;}),({
struct Cyc_PP_Doc*_tmp7FF=({struct _fat_ptr _tmp7FE=({const char*_tmp44D="";_tag_fat(_tmp44D,sizeof(char),1U);});struct _fat_ptr _tmp7FD=term;struct _fat_ptr _tmp7FC=({const char*_tmp44E=",";_tag_fat(_tmp44E,sizeof(char),2U);});Cyc_PP_group(_tmp7FE,_tmp7FD,_tmp7FC,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp449[3]=_tmp7FF;});Cyc_PP_cat(_tag_fat(_tmp449,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}}else{_LL21: _tmp405=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_tmp406=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3FD)->f3;_tmp407=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3FD)->f4;_LL22: {struct Cyc_List_List*exists=_tmp405;struct Cyc_List_List*dps=_tmp406;int dots=_tmp407;
# 1610
struct _fat_ptr term=dots?({const char*_tmp457=", ...}";_tag_fat(_tmp457,sizeof(char),7U);}):({const char*_tmp458="}";_tag_fat(_tmp458,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp451[3U];({struct Cyc_PP_Doc*_tmp80E=Cyc_Absynpp_lb();_tmp451[0]=_tmp80E;}),({
struct Cyc_PP_Doc*_tmp80D=({struct _fat_ptr _tmp80C=({const char*_tmp452="[";_tag_fat(_tmp452,sizeof(char),2U);});struct _fat_ptr _tmp80B=({const char*_tmp453="]";_tag_fat(_tmp453,sizeof(char),2U);});struct _fat_ptr _tmp80A=({const char*_tmp454=",";_tag_fat(_tmp454,sizeof(char),2U);});Cyc_PP_egroup(_tmp80C,_tmp80B,_tmp80A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp451[1]=_tmp80D;}),({
struct Cyc_PP_Doc*_tmp809=({struct _fat_ptr _tmp808=({const char*_tmp455="";_tag_fat(_tmp455,sizeof(char),1U);});struct _fat_ptr _tmp807=term;struct _fat_ptr _tmp806=({const char*_tmp456=",";_tag_fat(_tmp456,sizeof(char),2U);});Cyc_PP_group(_tmp808,_tmp807,_tmp806,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp451[2]=_tmp809;});Cyc_PP_cat(_tag_fat(_tmp451,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp404=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp404;
_tmp403=ef;goto _LL26;}case 14U: _LL25: _tmp403=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp403;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3FD)->f3 == 0){_LL27: _tmp402=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_LL28: {struct Cyc_Absyn_Datatypefield*tuf=_tmp402;
s=Cyc_Absynpp_qvar2doc(tuf->name);goto _LL0;}}else{_LL29: _tmp3FF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3FD)->f2;_tmp400=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3FD)->f3;_tmp401=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3FD)->f4;_LL2A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3FF;struct Cyc_List_List*ps=_tmp400;int dots=_tmp401;
# 1619
struct _fat_ptr term=dots?({const char*_tmp45C=", ...)";_tag_fat(_tmp45C,sizeof(char),7U);}):({const char*_tmp45D=")";_tag_fat(_tmp45D,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp459[2U];({struct Cyc_PP_Doc*_tmp813=Cyc_Absynpp_qvar2doc(tuf->name);_tmp459[0]=_tmp813;}),({struct Cyc_PP_Doc*_tmp812=({struct _fat_ptr _tmp811=({const char*_tmp45A="(";_tag_fat(_tmp45A,sizeof(char),2U);});struct _fat_ptr _tmp810=term;struct _fat_ptr _tmp80F=({const char*_tmp45B=",";_tag_fat(_tmp45B,sizeof(char),2U);});Cyc_PP_egroup(_tmp811,_tmp810,_tmp80F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp459[1]=_tmp812;});Cyc_PP_cat(_tag_fat(_tmp459,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp3FE=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3FD)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp3FE;
# 1623
s=Cyc_Absynpp_exp2doc(e);goto _LL0;}}_LL0:;}
# 1625
return s;}
# 1628
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp){
return({struct Cyc_PP_Doc*_tmp45E[2U];({struct Cyc_PP_Doc*_tmp818=({struct _fat_ptr _tmp817=({const char*_tmp45F="";_tag_fat(_tmp45F,sizeof(char),1U);});struct _fat_ptr _tmp816=({const char*_tmp460="=";_tag_fat(_tmp460,sizeof(char),2U);});struct _fat_ptr _tmp815=({const char*_tmp461="=";_tag_fat(_tmp461,sizeof(char),2U);});Cyc_PP_egroup(_tmp817,_tmp816,_tmp815,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp45E[0]=_tmp818;}),({
struct Cyc_PP_Doc*_tmp814=Cyc_Absynpp_pat2doc((*dp).f2);_tmp45E[1]=_tmp814;});Cyc_PP_cat(_tag_fat(_tmp45E,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1633
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1635
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp462[2U];({struct Cyc_PP_Doc*_tmp81B=Cyc_PP_text(({const char*_tmp463="default: ";_tag_fat(_tmp463,sizeof(char),10U);}));_tmp462[0]=_tmp81B;}),({
struct Cyc_PP_Doc*_tmp81A=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp464[2U];({struct Cyc_PP_Doc*_tmp819=Cyc_PP_line_doc();_tmp464[0]=_tmp819;}),_tmp464[1]=body;Cyc_PP_cat(_tag_fat(_tmp464,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp462[1]=_tmp81A;});Cyc_PP_cat(_tag_fat(_tmp462,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp465[4U];({struct Cyc_PP_Doc*_tmp820=Cyc_PP_text(({const char*_tmp466="case ";_tag_fat(_tmp466,sizeof(char),6U);}));_tmp465[0]=_tmp820;}),({
struct Cyc_PP_Doc*_tmp81F=Cyc_Absynpp_pat2doc(c->pattern);_tmp465[1]=_tmp81F;}),({
struct Cyc_PP_Doc*_tmp81E=Cyc_PP_text(({const char*_tmp467=": ";_tag_fat(_tmp467,sizeof(char),3U);}));_tmp465[2]=_tmp81E;}),({
struct Cyc_PP_Doc*_tmp81D=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp468[2U];({struct Cyc_PP_Doc*_tmp81C=Cyc_PP_line_doc();_tmp468[0]=_tmp81C;}),_tmp468[1]=body;Cyc_PP_cat(_tag_fat(_tmp468,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp465[3]=_tmp81D;});Cyc_PP_cat(_tag_fat(_tmp465,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1647
return({struct Cyc_PP_Doc*_tmp469[6U];({struct Cyc_PP_Doc*_tmp827=Cyc_PP_text(({const char*_tmp46A="case ";_tag_fat(_tmp46A,sizeof(char),6U);}));_tmp469[0]=_tmp827;}),({
struct Cyc_PP_Doc*_tmp826=Cyc_Absynpp_pat2doc(c->pattern);_tmp469[1]=_tmp826;}),({
struct Cyc_PP_Doc*_tmp825=Cyc_PP_text(({const char*_tmp46B=" && ";_tag_fat(_tmp46B,sizeof(char),5U);}));_tmp469[2]=_tmp825;}),({
struct Cyc_PP_Doc*_tmp824=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp469[3]=_tmp824;}),({
struct Cyc_PP_Doc*_tmp823=Cyc_PP_text(({const char*_tmp46C=": ";_tag_fat(_tmp46C,sizeof(char),3U);}));_tmp469[4]=_tmp823;}),({
struct Cyc_PP_Doc*_tmp822=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp46D[2U];({struct Cyc_PP_Doc*_tmp821=Cyc_PP_line_doc();_tmp46D[0]=_tmp821;}),_tmp46D[1]=body;Cyc_PP_cat(_tag_fat(_tmp46D,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp469[5]=_tmp822;});Cyc_PP_cat(_tag_fat(_tmp469,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1655
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _fat_ptr _tmp828=({const char*_tmp46E="";_tag_fat(_tmp46E,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp828,cs);});}
# 1659
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1663
return({struct Cyc_PP_Doc*_tmp46F[3U];({struct Cyc_PP_Doc*_tmp82B=Cyc_Absynpp_qvar2doc(f->name);_tmp46F[0]=_tmp82B;}),({struct Cyc_PP_Doc*_tmp82A=Cyc_PP_text(({const char*_tmp470=" = ";_tag_fat(_tmp470,sizeof(char),4U);}));_tmp46F[1]=_tmp82A;}),({struct Cyc_PP_Doc*_tmp829=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp46F[2]=_tmp829;});Cyc_PP_cat(_tag_fat(_tmp46F,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1666
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _fat_ptr _tmp82C=({const char*_tmp471=",";_tag_fat(_tmp471,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp82C,fs);});}
# 1670
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1674
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _fat_ptr _tmp82D=({const char*_tmp472=",";_tag_fat(_tmp472,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp82D,vds);});}
# 1678
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp473=vd;struct Cyc_List_List*_tmp47A;struct Cyc_Absyn_Exp*_tmp479;void*_tmp478;struct Cyc_Absyn_Tqual _tmp477;unsigned _tmp476;struct _tuple0*_tmp475;enum Cyc_Absyn_Scope _tmp474;_LL1: _tmp474=_tmp473->sc;_tmp475=_tmp473->name;_tmp476=_tmp473->varloc;_tmp477=_tmp473->tq;_tmp478=_tmp473->type;_tmp479=_tmp473->initializer;_tmp47A=_tmp473->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp474;struct _tuple0*name=_tmp475;unsigned varloc=_tmp476;struct Cyc_Absyn_Tqual tq=_tmp477;void*type=_tmp478;struct Cyc_Absyn_Exp*initializer=_tmp479;struct Cyc_List_List*atts=_tmp47A;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _tmp47B=Cyc_Flags_c_compiler;if(_tmp47B == Cyc_Flags_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1687
{void*_tmp47C=Cyc_Absyn_compress(type);void*_stmttmp1E=_tmp47C;void*_tmp47D=_stmttmp1E;struct Cyc_List_List*_tmp47E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47D)->tag == 5U){_LL9: _tmp47E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47D)->f1).attributes;_LLA: {struct Cyc_List_List*atts2=_tmp47E;
# 1689
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1693
goto _LL3;}_LL3:;}{
# 1696
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Flags_C_Compilers _tmp47F=Cyc_Flags_c_compiler;if(_tmp47F == Cyc_Flags_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1701
s=({struct Cyc_PP_Doc*_tmp480[5U];_tmp480[0]=tmp_doc,({
# 1703
struct Cyc_PP_Doc*_tmp837=Cyc_Absynpp_scope2doc(sc);_tmp480[1]=_tmp837;}),({
struct Cyc_PP_Doc*_tmp836=({struct Cyc_Absyn_Tqual _tmp835=tq;void*_tmp834=type;Cyc_Absynpp_tqtd2doc(_tmp835,_tmp834,({struct Cyc_Core_Opt*_tmp482=_cycalloc(sizeof(*_tmp482));({struct Cyc_PP_Doc*_tmp833=({struct Cyc_PP_Doc*_tmp481[2U];_tmp481[0]=beforenamedoc,_tmp481[1]=sn;Cyc_PP_cat(_tag_fat(_tmp481,sizeof(struct Cyc_PP_Doc*),2U));});_tmp482->v=_tmp833;});_tmp482;}));});_tmp480[2]=_tmp836;}),
initializer == 0?({
struct Cyc_PP_Doc*_tmp832=Cyc_PP_nil_doc();_tmp480[3]=_tmp832;}):({
struct Cyc_PP_Doc*_tmp831=({struct Cyc_PP_Doc*_tmp483[2U];({struct Cyc_PP_Doc*_tmp830=Cyc_PP_text(({const char*_tmp484=" = ";_tag_fat(_tmp484,sizeof(char),4U);}));_tmp483[0]=_tmp830;}),({struct Cyc_PP_Doc*_tmp82F=Cyc_Absynpp_exp2doc(initializer);_tmp483[1]=_tmp82F;});Cyc_PP_cat(_tag_fat(_tmp483,sizeof(struct Cyc_PP_Doc*),2U));});_tmp480[3]=_tmp831;}),({
struct Cyc_PP_Doc*_tmp82E=Cyc_PP_text(({const char*_tmp485=";";_tag_fat(_tmp485,sizeof(char),2U);}));_tmp480[4]=_tmp82E;});Cyc_PP_cat(_tag_fat(_tmp480,sizeof(struct Cyc_PP_Doc*),5U));});
return s;}}}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1712
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
struct _tuple20 _tmp486=*x;struct _tuple20 _stmttmp1F=_tmp486;struct _tuple20 _tmp487=_stmttmp1F;struct _tuple0*_tmp488;_LL1: _tmp488=_tmp487.f2;_LL2: {struct _tuple0*v=_tmp488;
return Cyc_Absynpp_qvar2doc(v);}}
# 1717
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp489[4U];({struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_scope2doc(ad->sc);_tmp489[0]=_tmp83B;}),({
struct Cyc_PP_Doc*_tmp83A=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp489[1]=_tmp83A;}),({
struct Cyc_PP_Doc*_tmp839=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp489[2]=_tmp839;}),({
struct Cyc_PP_Doc*_tmp838=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp489[3]=_tmp838;});Cyc_PP_cat(_tag_fat(_tmp489,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1724
return({struct Cyc_PP_Doc*_tmp48A[12U];({struct Cyc_PP_Doc*_tmp84C=Cyc_Absynpp_scope2doc(ad->sc);_tmp48A[0]=_tmp84C;}),({
struct Cyc_PP_Doc*_tmp84B=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp48A[1]=_tmp84B;}),({
struct Cyc_PP_Doc*_tmp84A=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp48A[2]=_tmp84A;}),({
struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp48A[3]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_PP_blank_doc();_tmp48A[4]=_tmp848;}),({struct Cyc_PP_Doc*_tmp847=Cyc_Absynpp_lb();_tmp48A[5]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp846=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp48A[6]=_tmp846;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp845=Cyc_PP_nil_doc();_tmp48A[7]=_tmp845;}):({
struct Cyc_PP_Doc*_tmp844=({struct Cyc_PP_Doc*_tmp48B[2U];({struct Cyc_PP_Doc*_tmp843=Cyc_PP_text(({const char*_tmp48C=":";_tag_fat(_tmp48C,sizeof(char),2U);}));_tmp48B[0]=_tmp843;}),({struct Cyc_PP_Doc*_tmp842=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp48B[1]=_tmp842;});Cyc_PP_cat(_tag_fat(_tmp48B,sizeof(struct Cyc_PP_Doc*),2U));});_tmp48A[7]=_tmp844;}),({
struct Cyc_PP_Doc*_tmp841=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp48D[2U];({struct Cyc_PP_Doc*_tmp840=Cyc_PP_line_doc();_tmp48D[0]=_tmp840;}),({struct Cyc_PP_Doc*_tmp83F=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp48D[1]=_tmp83F;});Cyc_PP_cat(_tag_fat(_tmp48D,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp48A[8]=_tmp841;}),({
struct Cyc_PP_Doc*_tmp83E=Cyc_PP_line_doc();_tmp48A[9]=_tmp83E;}),({
struct Cyc_PP_Doc*_tmp83D=Cyc_Absynpp_rb();_tmp48A[10]=_tmp83D;}),({
struct Cyc_PP_Doc*_tmp83C=Cyc_Absynpp_atts2doc(ad->attributes);_tmp48A[11]=_tmp83C;});Cyc_PP_cat(_tag_fat(_tmp48A,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1738
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp48E=dd;int _tmp493;struct Cyc_Core_Opt*_tmp492;struct Cyc_List_List*_tmp491;struct _tuple0*_tmp490;enum Cyc_Absyn_Scope _tmp48F;_LL1: _tmp48F=_tmp48E->sc;_tmp490=_tmp48E->name;_tmp491=_tmp48E->tvs;_tmp492=_tmp48E->fields;_tmp493=_tmp48E->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp48F;struct _tuple0*name=_tmp490;struct Cyc_List_List*tvs=_tmp491;struct Cyc_Core_Opt*fields=_tmp492;int is_x=_tmp493;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp494[5U];({struct Cyc_PP_Doc*_tmp853=Cyc_Absynpp_scope2doc(sc);_tmp494[0]=_tmp853;}),
is_x?({struct Cyc_PP_Doc*_tmp852=Cyc_PP_text(({const char*_tmp495="@extensible ";_tag_fat(_tmp495,sizeof(char),13U);}));_tmp494[1]=_tmp852;}):({struct Cyc_PP_Doc*_tmp851=Cyc_PP_blank_doc();_tmp494[1]=_tmp851;}),({
struct Cyc_PP_Doc*_tmp850=Cyc_PP_text(({const char*_tmp496="datatype ";_tag_fat(_tmp496,sizeof(char),10U);}));_tmp494[2]=_tmp850;}),
is_x?({struct Cyc_PP_Doc*_tmp84F=Cyc_Absynpp_qvar2bolddoc(name);_tmp494[3]=_tmp84F;}):({struct Cyc_PP_Doc*_tmp84E=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp494[3]=_tmp84E;}),({
struct Cyc_PP_Doc*_tmp84D=Cyc_Absynpp_ktvars2doc(tvs);_tmp494[4]=_tmp84D;});Cyc_PP_cat(_tag_fat(_tmp494,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1747
return({struct Cyc_PP_Doc*_tmp497[10U];({struct Cyc_PP_Doc*_tmp861=Cyc_Absynpp_scope2doc(sc);_tmp497[0]=_tmp861;}),
is_x?({struct Cyc_PP_Doc*_tmp860=Cyc_PP_text(({const char*_tmp498="@extensible ";_tag_fat(_tmp498,sizeof(char),13U);}));_tmp497[1]=_tmp860;}):({struct Cyc_PP_Doc*_tmp85F=Cyc_PP_blank_doc();_tmp497[1]=_tmp85F;}),({
struct Cyc_PP_Doc*_tmp85E=Cyc_PP_text(({const char*_tmp499="datatype ";_tag_fat(_tmp499,sizeof(char),10U);}));_tmp497[2]=_tmp85E;}),
is_x?({struct Cyc_PP_Doc*_tmp85D=Cyc_Absynpp_qvar2bolddoc(name);_tmp497[3]=_tmp85D;}):({struct Cyc_PP_Doc*_tmp85C=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp497[3]=_tmp85C;}),({
struct Cyc_PP_Doc*_tmp85B=Cyc_Absynpp_ktvars2doc(tvs);_tmp497[4]=_tmp85B;}),({
struct Cyc_PP_Doc*_tmp85A=Cyc_PP_blank_doc();_tmp497[5]=_tmp85A;}),({struct Cyc_PP_Doc*_tmp859=Cyc_Absynpp_lb();_tmp497[6]=_tmp859;}),({
struct Cyc_PP_Doc*_tmp858=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp49A[2U];({struct Cyc_PP_Doc*_tmp857=Cyc_PP_line_doc();_tmp49A[0]=_tmp857;}),({struct Cyc_PP_Doc*_tmp856=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_tmp49A[1]=_tmp856;});Cyc_PP_cat(_tag_fat(_tmp49A,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp497[7]=_tmp858;}),({
struct Cyc_PP_Doc*_tmp855=Cyc_PP_line_doc();_tmp497[8]=_tmp855;}),({
struct Cyc_PP_Doc*_tmp854=Cyc_Absynpp_rb();_tmp497[9]=_tmp854;});Cyc_PP_cat(_tag_fat(_tmp497,sizeof(struct Cyc_PP_Doc*),10U));});}}}
# 1758
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp49B=ed;struct Cyc_Core_Opt*_tmp49E;struct _tuple0*_tmp49D;enum Cyc_Absyn_Scope _tmp49C;_LL1: _tmp49C=_tmp49B->sc;_tmp49D=_tmp49B->name;_tmp49E=_tmp49B->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp49C;struct _tuple0*n=_tmp49D;struct Cyc_Core_Opt*fields=_tmp49E;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp49F[3U];({struct Cyc_PP_Doc*_tmp864=Cyc_Absynpp_scope2doc(sc);_tmp49F[0]=_tmp864;}),({
struct Cyc_PP_Doc*_tmp863=Cyc_PP_text(({const char*_tmp4A0="enum ";_tag_fat(_tmp4A0,sizeof(char),6U);}));_tmp49F[1]=_tmp863;}),({
struct Cyc_PP_Doc*_tmp862=Cyc_Absynpp_typedef_name2bolddoc(n);_tmp49F[2]=_tmp862;});Cyc_PP_cat(_tag_fat(_tmp49F,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1766
return({struct Cyc_PP_Doc*_tmp4A1[8U];({struct Cyc_PP_Doc*_tmp86E=Cyc_Absynpp_scope2doc(sc);_tmp4A1[0]=_tmp86E;}),({
struct Cyc_PP_Doc*_tmp86D=Cyc_PP_text(({const char*_tmp4A2="enum ";_tag_fat(_tmp4A2,sizeof(char),6U);}));_tmp4A1[1]=_tmp86D;}),({
struct Cyc_PP_Doc*_tmp86C=Cyc_Absynpp_qvar2bolddoc(n);_tmp4A1[2]=_tmp86C;}),({
struct Cyc_PP_Doc*_tmp86B=Cyc_PP_blank_doc();_tmp4A1[3]=_tmp86B;}),({struct Cyc_PP_Doc*_tmp86A=Cyc_Absynpp_lb();_tmp4A1[4]=_tmp86A;}),({
struct Cyc_PP_Doc*_tmp869=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4A3[2U];({struct Cyc_PP_Doc*_tmp868=Cyc_PP_line_doc();_tmp4A3[0]=_tmp868;}),({struct Cyc_PP_Doc*_tmp867=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_tmp4A3[1]=_tmp867;});Cyc_PP_cat(_tag_fat(_tmp4A3,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4A1[5]=_tmp869;}),({
struct Cyc_PP_Doc*_tmp866=Cyc_PP_line_doc();_tmp4A1[6]=_tmp866;}),({
struct Cyc_PP_Doc*_tmp865=Cyc_Absynpp_rb();_tmp4A1[7]=_tmp865;});Cyc_PP_cat(_tag_fat(_tmp4A1,sizeof(struct Cyc_PP_Doc*),8U));});}}}
# 1775
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp4A4=d->r;void*_stmttmp20=_tmp4A4;void*_tmp4A5=_stmttmp20;struct _tuple10*_tmp4A9;struct Cyc_List_List*_tmp4A8;struct Cyc_List_List*_tmp4A7;struct Cyc_List_List*_tmp4A6;struct Cyc_List_List*_tmp4AA;struct Cyc_List_List*_tmp4AC;struct _tuple0*_tmp4AB;struct Cyc_List_List*_tmp4AE;struct _fat_ptr*_tmp4AD;struct Cyc_Absyn_Typedefdecl*_tmp4AF;struct Cyc_Absyn_Enumdecl*_tmp4B0;struct Cyc_List_List*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Pat*_tmp4B2;struct Cyc_Absyn_Datatypedecl*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Vardecl*_tmp4B6;struct Cyc_Absyn_Tvar*_tmp4B5;struct Cyc_Absyn_Tvar*_tmp4B8;struct Cyc_Absyn_Vardecl*_tmp4B9;struct Cyc_Absyn_Aggrdecl*_tmp4BA;struct Cyc_Absyn_Fndecl*_tmp4BB;switch(*((int*)_tmp4A5)){case 1U: _LL1: _tmp4BB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp4BB;
# 1779
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC->tag=5U,_tmp4CC->f1=type_info;_tmp4CC;});
if(fd->cached_type != 0){
void*_tmp4BC=Cyc_Absyn_compress((void*)_check_null(fd->cached_type));void*_stmttmp21=_tmp4BC;void*_tmp4BD=_stmttmp21;struct Cyc_Absyn_FnInfo _tmp4BE;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4BD)->tag == 5U){_LL26: _tmp4BE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4BD)->f1;_LL27: {struct Cyc_Absyn_FnInfo i=_tmp4BE;
# 1785
({struct Cyc_List_List*_tmp86F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp86F;});goto _LL25;}}else{_LL28: _LL29:
({void*_tmp4BF=0U;({struct _fat_ptr _tmp870=({const char*_tmp4C0="function has non-function type";_tag_fat(_tmp4C0,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp870,_tag_fat(_tmp4BF,sizeof(void*),0U));});});}_LL25:;}{
# 1788
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((fd->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
enum Cyc_Flags_C_Compilers _tmp4C1=Cyc_Flags_c_compiler;if(_tmp4C1 == Cyc_Flags_Gcc_c){_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp4C2="inline ";_tag_fat(_tmp4C2,sizeof(char),8U);}));goto _LL2A;}else{_LL2D: _LL2E:
 inlinedoc=Cyc_PP_text(({const char*_tmp4C3="__inline ";_tag_fat(_tmp4C3,sizeof(char),10U);}));goto _LL2A;}_LL2A:;}else{
# 1796
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _tmp4C4=Cyc_Flags_c_compiler;if(_tmp4C4 == Cyc_Flags_Gcc_c){_LL30: _LL31:
 beforenamedoc=attsdoc;goto _LL2F;}else{_LL32: _LL33:
 beforenamedoc=Cyc_Absynpp_callconv2doc((fd->i).attributes);goto _LL2F;}_LL2F:;}{
# 1803
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp873=Cyc_Absyn_empty_tqual(0U);void*_tmp872=t;Cyc_Absynpp_tqtd2doc(_tmp873,_tmp872,({struct Cyc_Core_Opt*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));({
struct Cyc_PP_Doc*_tmp871=({struct Cyc_PP_Doc*_tmp4CA[2U];_tmp4CA[0]=beforenamedoc,_tmp4CA[1]=namedoc;Cyc_PP_cat(_tag_fat(_tmp4CA,sizeof(struct Cyc_PP_Doc*),2U));});_tmp4CB->v=_tmp871;});_tmp4CB;}));});
# 1811
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp4C8[5U];({struct Cyc_PP_Doc*_tmp87A=Cyc_PP_blank_doc();_tmp4C8[0]=_tmp87A;}),({struct Cyc_PP_Doc*_tmp879=Cyc_Absynpp_lb();_tmp4C8[1]=_tmp879;}),({
struct Cyc_PP_Doc*_tmp878=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4C9[2U];({struct Cyc_PP_Doc*_tmp877=Cyc_PP_line_doc();_tmp4C9[0]=_tmp877;}),({struct Cyc_PP_Doc*_tmp876=Cyc_Absynpp_stmt2doc(fd->body,0,0);_tmp4C9[1]=_tmp876;});Cyc_PP_cat(_tag_fat(_tmp4C9,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4C8[2]=_tmp878;}),({
struct Cyc_PP_Doc*_tmp875=Cyc_PP_line_doc();_tmp4C8[3]=_tmp875;}),({
struct Cyc_PP_Doc*_tmp874=Cyc_Absynpp_rb();_tmp4C8[4]=_tmp874;});Cyc_PP_cat(_tag_fat(_tmp4C8,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp4C5[4U];_tmp4C5[0]=inlinedoc,_tmp4C5[1]=scopedoc,_tmp4C5[2]=tqtddoc,_tmp4C5[3]=bodydoc;Cyc_PP_cat(_tag_fat(_tmp4C5,sizeof(struct Cyc_PP_Doc*),4U));});
# 1817
{enum Cyc_Flags_C_Compilers _tmp4C6=Cyc_Flags_c_compiler;if(_tmp4C6 == Cyc_Flags_Vc_c){_LL35: _LL36:
 s=({struct Cyc_PP_Doc*_tmp4C7[2U];_tmp4C7[0]=attsdoc,_tmp4C7[1]=s;Cyc_PP_cat(_tag_fat(_tmp4C7,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL34;}else{_LL37: _LL38:
 goto _LL34;}_LL34:;}
# 1822
goto _LL0;}}}}}case 6U: _LL3: _tmp4BA=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp4BA;
# 1825
s=({struct Cyc_PP_Doc*_tmp4CD[2U];({struct Cyc_PP_Doc*_tmp87C=Cyc_Absynpp_aggrdecl2doc(ad);_tmp4CD[0]=_tmp87C;}),({struct Cyc_PP_Doc*_tmp87B=Cyc_PP_text(({const char*_tmp4CE=";";_tag_fat(_tmp4CE,sizeof(char),2U);}));_tmp4CD[1]=_tmp87B;});Cyc_PP_cat(_tag_fat(_tmp4CD,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 0U: _LL5: _tmp4B9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp4B9;
# 1828
s=Cyc_Absynpp_vardecl2doc(vd);
goto _LL0;}case 5U: _LL7: _tmp4B8=((struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp4B8;
# 1831
s=({struct Cyc_PP_Doc*_tmp4CF[5U];({struct Cyc_PP_Doc*_tmp881=Cyc_PP_text(({const char*_tmp4D0="autopool";_tag_fat(_tmp4D0,sizeof(char),9U);}));_tmp4CF[0]=_tmp881;}),({
struct Cyc_PP_Doc*_tmp880=Cyc_PP_text(({const char*_tmp4D1="<";_tag_fat(_tmp4D1,sizeof(char),2U);}));_tmp4CF[1]=_tmp880;}),({
struct Cyc_PP_Doc*_tmp87F=Cyc_Absynpp_tvar2doc(tv);_tmp4CF[2]=_tmp87F;}),({
struct Cyc_PP_Doc*_tmp87E=Cyc_PP_text(({const char*_tmp4D2=">";_tag_fat(_tmp4D2,sizeof(char),2U);}));_tmp4CF[3]=_tmp87E;}),({
struct Cyc_PP_Doc*_tmp87D=Cyc_PP_text(({const char*_tmp4D3=";";_tag_fat(_tmp4D3,sizeof(char),2U);}));_tmp4CF[4]=_tmp87D;});Cyc_PP_cat(_tag_fat(_tmp4CF,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 4U: _LL9: _tmp4B5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_tmp4B6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4A5)->f2;_tmp4B7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4A5)->f3;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp4B5;struct Cyc_Absyn_Vardecl*vd=_tmp4B6;struct Cyc_Absyn_Exp*open_exp_opt=_tmp4B7;
# 1838
s=({struct Cyc_PP_Doc*_tmp4D4[7U];({struct Cyc_PP_Doc*_tmp88C=Cyc_PP_text(({const char*_tmp4D5="region";_tag_fat(_tmp4D5,sizeof(char),7U);}));_tmp4D4[0]=_tmp88C;}),({
struct Cyc_PP_Doc*_tmp88B=Cyc_PP_text(({const char*_tmp4D6="<";_tag_fat(_tmp4D6,sizeof(char),2U);}));_tmp4D4[1]=_tmp88B;}),({
struct Cyc_PP_Doc*_tmp88A=Cyc_Absynpp_tvar2doc(tv);_tmp4D4[2]=_tmp88A;}),({
struct Cyc_PP_Doc*_tmp889=Cyc_PP_text(({const char*_tmp4D7=">";_tag_fat(_tmp4D7,sizeof(char),2U);}));_tmp4D4[3]=_tmp889;}),({
struct Cyc_PP_Doc*_tmp888=Cyc_Absynpp_qvar2doc(vd->name);_tmp4D4[4]=_tmp888;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_tmp887=({struct Cyc_PP_Doc*_tmp4D8[3U];({struct Cyc_PP_Doc*_tmp886=Cyc_PP_text(({const char*_tmp4D9=" = open(";_tag_fat(_tmp4D9,sizeof(char),9U);}));_tmp4D8[0]=_tmp886;}),({struct Cyc_PP_Doc*_tmp885=Cyc_Absynpp_exp2doc(open_exp_opt);_tmp4D8[1]=_tmp885;}),({
struct Cyc_PP_Doc*_tmp884=Cyc_PP_text(({const char*_tmp4DA=")";_tag_fat(_tmp4DA,sizeof(char),2U);}));_tmp4D8[2]=_tmp884;});Cyc_PP_cat(_tag_fat(_tmp4D8,sizeof(struct Cyc_PP_Doc*),3U));});
# 1843
_tmp4D4[5]=_tmp887;}):({
struct Cyc_PP_Doc*_tmp883=Cyc_PP_nil_doc();_tmp4D4[5]=_tmp883;}),({
struct Cyc_PP_Doc*_tmp882=Cyc_PP_text(({const char*_tmp4DB=";";_tag_fat(_tmp4DB,sizeof(char),2U);}));_tmp4D4[6]=_tmp882;});Cyc_PP_cat(_tag_fat(_tmp4D4,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 7U: _LLB: _tmp4B4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LLC: {struct Cyc_Absyn_Datatypedecl*dd=_tmp4B4;
# 1848
s=({struct Cyc_PP_Doc*_tmp4DC[2U];({struct Cyc_PP_Doc*_tmp88E=Cyc_Absynpp_datatypedecl2doc(dd);_tmp4DC[0]=_tmp88E;}),({struct Cyc_PP_Doc*_tmp88D=Cyc_PP_text(({const char*_tmp4DD=";";_tag_fat(_tmp4DD,sizeof(char),2U);}));_tmp4DC[1]=_tmp88D;});Cyc_PP_cat(_tag_fat(_tmp4DC,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 2U: _LLD: _tmp4B2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_tmp4B3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4A5)->f3;_LLE: {struct Cyc_Absyn_Pat*p=_tmp4B2;struct Cyc_Absyn_Exp*e=_tmp4B3;
# 1851
s=({struct Cyc_PP_Doc*_tmp4DE[5U];({struct Cyc_PP_Doc*_tmp893=Cyc_PP_text(({const char*_tmp4DF="let ";_tag_fat(_tmp4DF,sizeof(char),5U);}));_tmp4DE[0]=_tmp893;}),({
struct Cyc_PP_Doc*_tmp892=Cyc_Absynpp_pat2doc(p);_tmp4DE[1]=_tmp892;}),({
struct Cyc_PP_Doc*_tmp891=Cyc_PP_text(({const char*_tmp4E0=" = ";_tag_fat(_tmp4E0,sizeof(char),4U);}));_tmp4DE[2]=_tmp891;}),({
struct Cyc_PP_Doc*_tmp890=Cyc_Absynpp_exp2doc(e);_tmp4DE[3]=_tmp890;}),({
struct Cyc_PP_Doc*_tmp88F=Cyc_PP_text(({const char*_tmp4E1=";";_tag_fat(_tmp4E1,sizeof(char),2U);}));_tmp4DE[4]=_tmp88F;});Cyc_PP_cat(_tag_fat(_tmp4DE,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 3U: _LLF: _tmp4B1=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LL10: {struct Cyc_List_List*vds=_tmp4B1;
# 1858
s=({struct Cyc_PP_Doc*_tmp4E2[3U];({struct Cyc_PP_Doc*_tmp896=Cyc_PP_text(({const char*_tmp4E3="let ";_tag_fat(_tmp4E3,sizeof(char),5U);}));_tmp4E2[0]=_tmp896;}),({struct Cyc_PP_Doc*_tmp895=Cyc_Absynpp_ids2doc(vds);_tmp4E2[1]=_tmp895;}),({struct Cyc_PP_Doc*_tmp894=Cyc_PP_text(({const char*_tmp4E4=";";_tag_fat(_tmp4E4,sizeof(char),2U);}));_tmp4E2[2]=_tmp894;});Cyc_PP_cat(_tag_fat(_tmp4E2,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 8U: _LL11: _tmp4B0=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LL12: {struct Cyc_Absyn_Enumdecl*ed=_tmp4B0;
# 1861
s=({struct Cyc_PP_Doc*_tmp4E5[2U];({struct Cyc_PP_Doc*_tmp898=Cyc_Absynpp_enumdecl2doc(ed);_tmp4E5[0]=_tmp898;}),({struct Cyc_PP_Doc*_tmp897=Cyc_PP_text(({const char*_tmp4E6=";";_tag_fat(_tmp4E6,sizeof(char),2U);}));_tmp4E5[1]=_tmp897;});Cyc_PP_cat(_tag_fat(_tmp4E5,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 9U: _LL13: _tmp4AF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LL14: {struct Cyc_Absyn_Typedefdecl*td=_tmp4AF;
# 1864
void*t;
if(td->defn != 0)
t=(void*)_check_null(td->defn);else{
# 1868
t=Cyc_Absyn_new_evar(td->kind,0);}
s=({struct Cyc_PP_Doc*_tmp4E7[4U];({struct Cyc_PP_Doc*_tmp8A1=Cyc_PP_text(({const char*_tmp4E8="typedef ";_tag_fat(_tmp4E8,sizeof(char),9U);}));_tmp4E7[0]=_tmp8A1;}),({
struct Cyc_PP_Doc*_tmp8A0=({struct Cyc_Absyn_Tqual _tmp89F=td->tq;void*_tmp89E=t;Cyc_Absynpp_tqtd2doc(_tmp89F,_tmp89E,({struct Cyc_Core_Opt*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));({
# 1872
struct Cyc_PP_Doc*_tmp89D=({struct Cyc_PP_Doc*_tmp4E9[2U];({struct Cyc_PP_Doc*_tmp89C=Cyc_Absynpp_typedef_name2bolddoc(td->name);_tmp4E9[0]=_tmp89C;}),({
struct Cyc_PP_Doc*_tmp89B=Cyc_Absynpp_tvars2doc(td->tvs);_tmp4E9[1]=_tmp89B;});Cyc_PP_cat(_tag_fat(_tmp4E9,sizeof(struct Cyc_PP_Doc*),2U));});
# 1872
_tmp4EA->v=_tmp89D;});_tmp4EA;}));});
# 1870
_tmp4E7[1]=_tmp8A0;}),({
# 1875
struct Cyc_PP_Doc*_tmp89A=Cyc_Absynpp_atts2doc(td->atts);_tmp4E7[2]=_tmp89A;}),({
struct Cyc_PP_Doc*_tmp899=Cyc_PP_text(({const char*_tmp4EB=";";_tag_fat(_tmp4EB,sizeof(char),2U);}));_tmp4E7[3]=_tmp899;});Cyc_PP_cat(_tag_fat(_tmp4E7,sizeof(struct Cyc_PP_Doc*),4U));});
# 1878
goto _LL0;}case 10U: _LL15: _tmp4AD=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_tmp4AE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4A5)->f2;_LL16: {struct _fat_ptr*v=_tmp4AD;struct Cyc_List_List*tdl=_tmp4AE;
# 1880
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_tmp4EC[8U];({struct Cyc_PP_Doc*_tmp8AA=Cyc_PP_text(({const char*_tmp4ED="namespace ";_tag_fat(_tmp4ED,sizeof(char),11U);}));_tmp4EC[0]=_tmp8AA;}),({
struct Cyc_PP_Doc*_tmp8A9=Cyc_PP_textptr(v);_tmp4EC[1]=_tmp8A9;}),({
struct Cyc_PP_Doc*_tmp8A8=Cyc_PP_blank_doc();_tmp4EC[2]=_tmp8A8;}),({struct Cyc_PP_Doc*_tmp8A7=Cyc_Absynpp_lb();_tmp4EC[3]=_tmp8A7;}),({
struct Cyc_PP_Doc*_tmp8A6=Cyc_PP_line_doc();_tmp4EC[4]=_tmp8A6;}),({
struct Cyc_PP_Doc*_tmp8A5=({struct _fat_ptr _tmp8A4=({const char*_tmp4EE="";_tag_fat(_tmp4EE,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8A4,tdl);});_tmp4EC[5]=_tmp8A5;}),({
struct Cyc_PP_Doc*_tmp8A3=Cyc_PP_line_doc();_tmp4EC[6]=_tmp8A3;}),({
struct Cyc_PP_Doc*_tmp8A2=Cyc_Absynpp_rb();_tmp4EC[7]=_tmp8A2;});Cyc_PP_cat(_tag_fat(_tmp4EC,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 11U: _LL17: _tmp4AB=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_tmp4AC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4A5)->f2;_LL18: {struct _tuple0*q=_tmp4AB;struct Cyc_List_List*tdl=_tmp4AC;
# 1891
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp4EF[8U];({struct Cyc_PP_Doc*_tmp8B3=Cyc_PP_text(({const char*_tmp4F0="using ";_tag_fat(_tmp4F0,sizeof(char),7U);}));_tmp4EF[0]=_tmp8B3;}),({
struct Cyc_PP_Doc*_tmp8B2=Cyc_Absynpp_qvar2doc(q);_tmp4EF[1]=_tmp8B2;}),({
struct Cyc_PP_Doc*_tmp8B1=Cyc_PP_blank_doc();_tmp4EF[2]=_tmp8B1;}),({struct Cyc_PP_Doc*_tmp8B0=Cyc_Absynpp_lb();_tmp4EF[3]=_tmp8B0;}),({
struct Cyc_PP_Doc*_tmp8AF=Cyc_PP_line_doc();_tmp4EF[4]=_tmp8AF;}),({
struct Cyc_PP_Doc*_tmp8AE=({struct _fat_ptr _tmp8AD=({const char*_tmp4F1="";_tag_fat(_tmp4F1,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8AD,tdl);});_tmp4EF[5]=_tmp8AE;}),({
struct Cyc_PP_Doc*_tmp8AC=Cyc_PP_line_doc();_tmp4EF[6]=_tmp8AC;}),({
struct Cyc_PP_Doc*_tmp8AB=Cyc_Absynpp_rb();_tmp4EF[7]=_tmp8AB;});Cyc_PP_cat(_tag_fat(_tmp4EF,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1900
s=({struct Cyc_PP_Doc*_tmp4F2[11U];({struct Cyc_PP_Doc*_tmp8BF=Cyc_PP_text(({const char*_tmp4F3="/* using ";_tag_fat(_tmp4F3,sizeof(char),10U);}));_tmp4F2[0]=_tmp8BF;}),({
struct Cyc_PP_Doc*_tmp8BE=Cyc_Absynpp_qvar2doc(q);_tmp4F2[1]=_tmp8BE;}),({
struct Cyc_PP_Doc*_tmp8BD=Cyc_PP_blank_doc();_tmp4F2[2]=_tmp8BD;}),({
struct Cyc_PP_Doc*_tmp8BC=Cyc_Absynpp_lb();_tmp4F2[3]=_tmp8BC;}),({
struct Cyc_PP_Doc*_tmp8BB=Cyc_PP_text(({const char*_tmp4F4=" */";_tag_fat(_tmp4F4,sizeof(char),4U);}));_tmp4F2[4]=_tmp8BB;}),({
struct Cyc_PP_Doc*_tmp8BA=Cyc_PP_line_doc();_tmp4F2[5]=_tmp8BA;}),({
struct Cyc_PP_Doc*_tmp8B9=({struct _fat_ptr _tmp8B8=({const char*_tmp4F5="";_tag_fat(_tmp4F5,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8B8,tdl);});_tmp4F2[6]=_tmp8B9;}),({
struct Cyc_PP_Doc*_tmp8B7=Cyc_PP_line_doc();_tmp4F2[7]=_tmp8B7;}),({
struct Cyc_PP_Doc*_tmp8B6=Cyc_PP_text(({const char*_tmp4F6="/* ";_tag_fat(_tmp4F6,sizeof(char),4U);}));_tmp4F2[8]=_tmp8B6;}),({
struct Cyc_PP_Doc*_tmp8B5=Cyc_Absynpp_rb();_tmp4F2[9]=_tmp8B5;}),({
struct Cyc_PP_Doc*_tmp8B4=Cyc_PP_text(({const char*_tmp4F7=" */";_tag_fat(_tmp4F7,sizeof(char),4U);}));_tmp4F2[10]=_tmp8B4;});Cyc_PP_cat(_tag_fat(_tmp4F2,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;}case 12U: _LL19: _tmp4AA=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_LL1A: {struct Cyc_List_List*tdl=_tmp4AA;
# 1913
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp4F8[6U];({struct Cyc_PP_Doc*_tmp8C6=Cyc_PP_text(({const char*_tmp4F9="extern \"C\" ";_tag_fat(_tmp4F9,sizeof(char),12U);}));_tmp4F8[0]=_tmp8C6;}),({
struct Cyc_PP_Doc*_tmp8C5=Cyc_Absynpp_lb();_tmp4F8[1]=_tmp8C5;}),({
struct Cyc_PP_Doc*_tmp8C4=Cyc_PP_line_doc();_tmp4F8[2]=_tmp8C4;}),({
struct Cyc_PP_Doc*_tmp8C3=({struct _fat_ptr _tmp8C2=({const char*_tmp4FA="";_tag_fat(_tmp4FA,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8C2,tdl);});_tmp4F8[3]=_tmp8C3;}),({
struct Cyc_PP_Doc*_tmp8C1=Cyc_PP_line_doc();_tmp4F8[4]=_tmp8C1;}),({
struct Cyc_PP_Doc*_tmp8C0=Cyc_Absynpp_rb();_tmp4F8[5]=_tmp8C0;});Cyc_PP_cat(_tag_fat(_tmp4F8,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1921
s=({struct Cyc_PP_Doc*_tmp4FB[9U];({struct Cyc_PP_Doc*_tmp8D0=Cyc_PP_text(({const char*_tmp4FC="/* extern \"C\" ";_tag_fat(_tmp4FC,sizeof(char),15U);}));_tmp4FB[0]=_tmp8D0;}),({
struct Cyc_PP_Doc*_tmp8CF=Cyc_Absynpp_lb();_tmp4FB[1]=_tmp8CF;}),({
struct Cyc_PP_Doc*_tmp8CE=Cyc_PP_text(({const char*_tmp4FD=" */";_tag_fat(_tmp4FD,sizeof(char),4U);}));_tmp4FB[2]=_tmp8CE;}),({
struct Cyc_PP_Doc*_tmp8CD=Cyc_PP_line_doc();_tmp4FB[3]=_tmp8CD;}),({
struct Cyc_PP_Doc*_tmp8CC=({struct _fat_ptr _tmp8CB=({const char*_tmp4FE="";_tag_fat(_tmp4FE,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8CB,tdl);});_tmp4FB[4]=_tmp8CC;}),({
struct Cyc_PP_Doc*_tmp8CA=Cyc_PP_line_doc();_tmp4FB[5]=_tmp8CA;}),({
struct Cyc_PP_Doc*_tmp8C9=Cyc_PP_text(({const char*_tmp4FF="/* ";_tag_fat(_tmp4FF,sizeof(char),4U);}));_tmp4FB[6]=_tmp8C9;}),({
struct Cyc_PP_Doc*_tmp8C8=Cyc_Absynpp_rb();_tmp4FB[7]=_tmp8C8;}),({
struct Cyc_PP_Doc*_tmp8C7=Cyc_PP_text(({const char*_tmp500=" */";_tag_fat(_tmp500,sizeof(char),4U);}));_tmp4FB[8]=_tmp8C7;});Cyc_PP_cat(_tag_fat(_tmp4FB,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 13U: _LL1B: _tmp4A6=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4A5)->f1;_tmp4A7=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4A5)->f2;_tmp4A8=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4A5)->f3;_tmp4A9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4A5)->f4;_LL1C: {struct Cyc_List_List*tdl=_tmp4A6;struct Cyc_List_List*ovrs=_tmp4A7;struct Cyc_List_List*exs=_tmp4A8;struct _tuple10*wc=_tmp4A9;
# 1932
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp501[7U];({struct Cyc_PP_Doc*_tmp8D8=Cyc_Absynpp_rb();_tmp501[0]=_tmp8D8;}),({struct Cyc_PP_Doc*_tmp8D7=Cyc_PP_text(({const char*_tmp502=" export ";_tag_fat(_tmp502,sizeof(char),9U);}));_tmp501[1]=_tmp8D7;}),({struct Cyc_PP_Doc*_tmp8D6=Cyc_Absynpp_lb();_tmp501[2]=_tmp8D6;}),({
struct Cyc_PP_Doc*_tmp8D5=Cyc_PP_line_doc();_tmp501[3]=_tmp8D5;}),({struct Cyc_PP_Doc*_tmp8D4=({struct _fat_ptr _tmp8D3=({const char*_tmp503=",";_tag_fat(_tmp503,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple20*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp8D3,exs);});_tmp501[4]=_tmp8D4;}),({
struct Cyc_PP_Doc*_tmp8D2=Cyc_PP_line_doc();_tmp501[5]=_tmp8D2;}),({struct Cyc_PP_Doc*_tmp8D1=Cyc_Absynpp_rb();_tmp501[6]=_tmp8D1;});Cyc_PP_cat(_tag_fat(_tmp501,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1940
exs_doc=Cyc_Absynpp_rb();}
if(ovrs != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp504[7U];({struct Cyc_PP_Doc*_tmp8E0=Cyc_Absynpp_rb();_tmp504[0]=_tmp8E0;}),({struct Cyc_PP_Doc*_tmp8DF=Cyc_PP_text(({const char*_tmp505=" cycdef ";_tag_fat(_tmp505,sizeof(char),9U);}));_tmp504[1]=_tmp8DF;}),({struct Cyc_PP_Doc*_tmp8DE=Cyc_Absynpp_lb();_tmp504[2]=_tmp8DE;}),({
struct Cyc_PP_Doc*_tmp8DD=Cyc_PP_line_doc();_tmp504[3]=_tmp8DD;}),({struct Cyc_PP_Doc*_tmp8DC=({struct _fat_ptr _tmp8DB=({const char*_tmp506="";_tag_fat(_tmp506,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8DB,ovrs);});_tmp504[4]=_tmp8DC;}),({
struct Cyc_PP_Doc*_tmp8DA=Cyc_PP_line_doc();_tmp504[5]=_tmp8DA;}),({struct Cyc_PP_Doc*_tmp8D9=Cyc_Absynpp_rb();_tmp504[6]=_tmp8D9;});Cyc_PP_cat(_tag_fat(_tmp504,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1946
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp507[6U];({struct Cyc_PP_Doc*_tmp8E6=Cyc_PP_text(({const char*_tmp508="extern \"C include\" ";_tag_fat(_tmp508,sizeof(char),20U);}));_tmp507[0]=_tmp8E6;}),({
struct Cyc_PP_Doc*_tmp8E5=Cyc_Absynpp_lb();_tmp507[1]=_tmp8E5;}),({
struct Cyc_PP_Doc*_tmp8E4=Cyc_PP_line_doc();_tmp507[2]=_tmp8E4;}),({
struct Cyc_PP_Doc*_tmp8E3=({struct _fat_ptr _tmp8E2=({const char*_tmp509="";_tag_fat(_tmp509,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8E2,tdl);});_tmp507[3]=_tmp8E3;}),({
struct Cyc_PP_Doc*_tmp8E1=Cyc_PP_line_doc();_tmp507[4]=_tmp8E1;}),_tmp507[5]=exs_doc;Cyc_PP_cat(_tag_fat(_tmp507,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1954
s=({struct Cyc_PP_Doc*_tmp50A[9U];({struct Cyc_PP_Doc*_tmp8F0=Cyc_PP_text(({const char*_tmp50B="/* extern \"C include\" ";_tag_fat(_tmp50B,sizeof(char),23U);}));_tmp50A[0]=_tmp8F0;}),({
struct Cyc_PP_Doc*_tmp8EF=Cyc_Absynpp_lb();_tmp50A[1]=_tmp8EF;}),({
struct Cyc_PP_Doc*_tmp8EE=Cyc_PP_text(({const char*_tmp50C=" */";_tag_fat(_tmp50C,sizeof(char),4U);}));_tmp50A[2]=_tmp8EE;}),({
struct Cyc_PP_Doc*_tmp8ED=Cyc_PP_line_doc();_tmp50A[3]=_tmp8ED;}),({
struct Cyc_PP_Doc*_tmp8EC=({struct _fat_ptr _tmp8EB=({const char*_tmp50D="";_tag_fat(_tmp50D,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8EB,tdl);});_tmp50A[4]=_tmp8EC;}),({
struct Cyc_PP_Doc*_tmp8EA=Cyc_PP_line_doc();_tmp50A[5]=_tmp8EA;}),({
struct Cyc_PP_Doc*_tmp8E9=Cyc_PP_text(({const char*_tmp50E="/* ";_tag_fat(_tmp50E,sizeof(char),4U);}));_tmp50A[6]=_tmp8E9;}),({
struct Cyc_PP_Doc*_tmp8E8=Cyc_Absynpp_rb();_tmp50A[7]=_tmp8E8;}),({
struct Cyc_PP_Doc*_tmp8E7=Cyc_PP_text(({const char*_tmp50F=" */";_tag_fat(_tmp50F,sizeof(char),4U);}));_tmp50A[8]=_tmp8E7;});Cyc_PP_cat(_tag_fat(_tmp50A,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 14U: _LL1D: _LL1E:
# 1965
 s=({struct Cyc_PP_Doc*_tmp510[2U];({struct Cyc_PP_Doc*_tmp8F2=Cyc_PP_text(({const char*_tmp511="__cyclone_port_on__;";_tag_fat(_tmp511,sizeof(char),21U);}));_tmp510[0]=_tmp8F2;}),({struct Cyc_PP_Doc*_tmp8F1=Cyc_Absynpp_lb();_tmp510[1]=_tmp8F1;});Cyc_PP_cat(_tag_fat(_tmp510,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1968
 s=({struct Cyc_PP_Doc*_tmp512[2U];({struct Cyc_PP_Doc*_tmp8F4=Cyc_PP_text(({const char*_tmp513="__cyclone_port_off__;";_tag_fat(_tmp513,sizeof(char),22U);}));_tmp512[0]=_tmp8F4;}),({struct Cyc_PP_Doc*_tmp8F3=Cyc_Absynpp_lb();_tmp512[1]=_tmp8F3;});Cyc_PP_cat(_tag_fat(_tmp512,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 16U: _LL21: _LL22:
# 1971
 s=({struct Cyc_PP_Doc*_tmp514[2U];({struct Cyc_PP_Doc*_tmp8F6=Cyc_PP_text(({const char*_tmp515="__tempest_on__;";_tag_fat(_tmp515,sizeof(char),16U);}));_tmp514[0]=_tmp8F6;}),({struct Cyc_PP_Doc*_tmp8F5=Cyc_Absynpp_lb();_tmp514[1]=_tmp8F5;});Cyc_PP_cat(_tag_fat(_tmp514,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL23: _LL24:
# 1974
 s=({struct Cyc_PP_Doc*_tmp516[2U];({struct Cyc_PP_Doc*_tmp8F8=Cyc_PP_text(({const char*_tmp517="__tempest_off__;";_tag_fat(_tmp517,sizeof(char),17U);}));_tmp516[0]=_tmp8F8;}),({struct Cyc_PP_Doc*_tmp8F7=Cyc_Absynpp_lb();_tmp516[1]=_tmp8F7;});Cyc_PP_cat(_tag_fat(_tmp516,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 1978
return s;}
# 1981
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp518=sc;switch(_tmp518){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp519="static ";_tag_fat(_tmp519,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp51A="extern ";_tag_fat(_tmp51A,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp51B="extern \"C\" ";_tag_fat(_tmp51B,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp51C="abstract ";_tag_fat(_tmp51C,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp51D="register ";_tag_fat(_tmp51D,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;}}
# 1995
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp51E=Cyc_Absyn_compress(t);void*_stmttmp22=_tmp51E;void*_tmp51F=_stmttmp22;struct Cyc_List_List*_tmp520;struct Cyc_Absyn_Tvar*_tmp521;switch(*((int*)_tmp51F)){case 2U: _LL1: _tmp521=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp51F)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp521;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51F)->f1)->tag == 9U){_LL3: _tmp520=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51F)->f2;_LL4: {struct Cyc_List_List*l=_tmp520;
return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2007
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp522=t;void*_tmp524;struct Cyc_Absyn_Typedefdecl*_tmp523;switch(*((int*)_tmp522)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp522)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp523=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp522)->f3;_tmp524=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp522)->f4;if(_tmp524 != 0){_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmp523;void*x=_tmp524;
# 2014
return Cyc_Absynpp_is_anon_aggrtype(x);}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2021
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2024
if(tms != 0 && tms->tl != 0){
struct _tuple18 _tmp525=({struct _tuple18 _tmp5CD;_tmp5CD.f1=(void*)tms->hd,_tmp5CD.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp5CD;});struct _tuple18 _stmttmp23=_tmp525;struct _tuple18 _tmp526=_stmttmp23;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp526.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp526.f2)->tag == 3U){_LL1: _LL2:
# 2027
 return({struct Cyc_List_List*_tmp528=_region_malloc(r,sizeof(*_tmp528));_tmp528->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp8FA=({struct Cyc_List_List*_tmp527=_region_malloc(r,sizeof(*_tmp527));_tmp527->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp8F9=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp527->tl=_tmp8F9;});_tmp527;});_tmp528->tl=_tmp8FA;});_tmp528;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp529=_region_malloc(r,sizeof(*_tmp529));_tmp529->hd=atts,_tmp529->tl=tms;_tmp529;});}_LL0:;}else{
# 2030
return({struct Cyc_List_List*_tmp52A=_region_malloc(r,sizeof(*_tmp52A));_tmp52A->hd=atts,_tmp52A->tl=tms;_tmp52A;});}}
# 2033
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp52B=({struct _fat_ptr _tmp8FB=({const char*_tmp531="`";_tag_fat(_tmp531,sizeof(char),2U);});Cyc_strconcat(_tmp8FB,(struct _fat_ptr)*t->name);});struct _fat_ptr s=_tmp52B;
({struct _fat_ptr _tmp52C=_fat_ptr_plus(s,sizeof(char),1);char _tmp52D=*((char*)_check_fat_subscript(_tmp52C,sizeof(char),0U));char _tmp52E='t';if(_get_fat_size(_tmp52C,sizeof(char))== 1U &&(_tmp52D == 0 && _tmp52E != 0))_throw_arraybounds();*((char*)_tmp52C.curr)=_tmp52E;});
({struct _fat_ptr*_tmp8FC=({unsigned _tmp530=1;struct _fat_ptr*_tmp52F=_cycalloc(_check_times(_tmp530,sizeof(struct _fat_ptr)));_tmp52F[0]=(struct _fat_ptr)s;_tmp52F;});t->name=_tmp8FC;});}}
# 2042
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2044
void*_tmp532=t;void*_tmp536;struct Cyc_Absyn_Typedefdecl*_tmp535;struct Cyc_List_List*_tmp534;struct _tuple0*_tmp533;int _tmp539;void*_tmp538;struct Cyc_Core_Opt*_tmp537;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_List_List*_tmp542;struct Cyc_List_List*_tmp541;struct Cyc_Absyn_VarargInfo*_tmp540;int _tmp53F;struct Cyc_List_List*_tmp53E;void*_tmp53D;struct Cyc_Absyn_Tqual _tmp53C;void*_tmp53B;struct Cyc_List_List*_tmp53A;struct Cyc_Absyn_PtrAtts _tmp547;struct Cyc_Absyn_Tqual _tmp546;void*_tmp545;unsigned _tmp54C;void*_tmp54B;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Tqual _tmp549;void*_tmp548;switch(*((int*)_tmp532)){case 4U: _LL1: _tmp548=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).elt_type;_tmp549=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).tq;_tmp54A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).num_elts;_tmp54B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).zero_term;_tmp54C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp532)->f1).zt_loc;_LL2: {void*t2=_tmp548;struct Cyc_Absyn_Tqual tq2=_tmp549;struct Cyc_Absyn_Exp*e=_tmp54A;void*zeroterm=_tmp54B;unsigned ztl=_tmp54C;
# 2047
struct _tuple13 _tmp54D=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp24=_tmp54D;struct _tuple13 _tmp54E=_stmttmp24;struct Cyc_List_List*_tmp551;void*_tmp550;struct Cyc_Absyn_Tqual _tmp54F;_LLE: _tmp54F=_tmp54E.f1;_tmp550=_tmp54E.f2;_tmp551=_tmp54E.f3;_LLF: {struct Cyc_Absyn_Tqual tq3=_tmp54F;void*t3=_tmp550;struct Cyc_List_List*tml3=_tmp551;
void*tm;
if(e == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp552=_region_malloc(r,sizeof(*_tmp552));_tmp552->tag=0U,_tmp552->f1=zeroterm,_tmp552->f2=ztl;_tmp552;});else{
# 2052
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp553=_region_malloc(r,sizeof(*_tmp553));_tmp553->tag=1U,_tmp553->f1=e,_tmp553->f2=zeroterm,_tmp553->f3=ztl;_tmp553;});}
return({struct _tuple13 _tmp5CE;_tmp5CE.f1=tq3,_tmp5CE.f2=t3,({struct Cyc_List_List*_tmp8FD=({struct Cyc_List_List*_tmp554=_region_malloc(r,sizeof(*_tmp554));_tmp554->hd=tm,_tmp554->tl=tml3;_tmp554;});_tmp5CE.f3=_tmp8FD;});_tmp5CE;});}}case 3U: _LL3: _tmp545=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp532)->f1).elt_type;_tmp546=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp532)->f1).elt_tq;_tmp547=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp532)->f1).ptr_atts;_LL4: {void*t2=_tmp545;struct Cyc_Absyn_Tqual tq2=_tmp546;struct Cyc_Absyn_PtrAtts ptratts=_tmp547;
# 2056
struct _tuple13 _tmp555=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp25=_tmp555;struct _tuple13 _tmp556=_stmttmp25;struct Cyc_List_List*_tmp559;void*_tmp558;struct Cyc_Absyn_Tqual _tmp557;_LL11: _tmp557=_tmp556.f1;_tmp558=_tmp556.f2;_tmp559=_tmp556.f3;_LL12: {struct Cyc_Absyn_Tqual tq3=_tmp557;void*t3=_tmp558;struct Cyc_List_List*tml3=_tmp559;
tml3=({struct Cyc_List_List*_tmp55B=_region_malloc(r,sizeof(*_tmp55B));({void*_tmp8FE=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp55A=_region_malloc(r,sizeof(*_tmp55A));_tmp55A->tag=2U,_tmp55A->f1=ptratts,_tmp55A->f2=tq;_tmp55A;});_tmp55B->hd=_tmp8FE;}),_tmp55B->tl=tml3;_tmp55B;});
return({struct _tuple13 _tmp5CF;_tmp5CF.f1=tq3,_tmp5CF.f2=t3,_tmp5CF.f3=tml3;_tmp5CF;});}}case 5U: _LL5: _tmp53A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).tvars;_tmp53B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).effect;_tmp53C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).ret_tqual;_tmp53D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).ret_type;_tmp53E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).args;_tmp53F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).c_varargs;_tmp540=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).cyc_varargs;_tmp541=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).rgn_po;_tmp542=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).attributes;_tmp543=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).requires_clause;_tmp544=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp532)->f1).ensures_clause;_LL6: {struct Cyc_List_List*typvars=_tmp53A;void*effopt=_tmp53B;struct Cyc_Absyn_Tqual t2qual=_tmp53C;void*t2=_tmp53D;struct Cyc_List_List*args=_tmp53E;int c_varargs=_tmp53F;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp540;struct Cyc_List_List*rgn_po=_tmp541;struct Cyc_List_List*fn_atts=_tmp542;struct Cyc_Absyn_Exp*req=_tmp543;struct Cyc_Absyn_Exp*ens=_tmp544;
# 2062
if(!Cyc_Absynpp_print_all_tvars){
# 2066
if(effopt == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2071
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2074
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2079
struct _tuple13 _tmp55C=Cyc_Absynpp_to_tms(r,t2qual,t2);struct _tuple13 _stmttmp26=_tmp55C;struct _tuple13 _tmp55D=_stmttmp26;struct Cyc_List_List*_tmp560;void*_tmp55F;struct Cyc_Absyn_Tqual _tmp55E;_LL14: _tmp55E=_tmp55D.f1;_tmp55F=_tmp55D.f2;_tmp560=_tmp55D.f3;_LL15: {struct Cyc_Absyn_Tqual tq3=_tmp55E;void*t3=_tmp55F;struct Cyc_List_List*tml3=_tmp560;
struct Cyc_List_List*tms=tml3;
# 2090 "absynpp.cyc"
{enum Cyc_Flags_C_Compilers _tmp561=Cyc_Flags_c_compiler;if(_tmp561 == Cyc_Flags_Gcc_c){_LL17: _LL18:
# 2092
 if(fn_atts != 0)
# 2095
tms=({struct _RegionHandle*_tmp900=r;void*_tmp8FF=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp562=_region_malloc(r,sizeof(*_tmp562));_tmp562->tag=5U,_tmp562->f1=0U,_tmp562->f2=fn_atts;_tmp562;});Cyc_Absynpp_bubble_attributes(_tmp900,_tmp8FF,tms);});
# 2097
tms=({struct Cyc_List_List*_tmp565=_region_malloc(r,sizeof(*_tmp565));({void*_tmp902=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp564=_region_malloc(r,sizeof(*_tmp564));
_tmp564->tag=3U,({void*_tmp901=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp563=_region_malloc(r,sizeof(*_tmp563));_tmp563->tag=1U,_tmp563->f1=args,_tmp563->f2=c_varargs,_tmp563->f3=cyc_varargs,_tmp563->f4=effopt,_tmp563->f5=rgn_po,_tmp563->f6=req,_tmp563->f7=ens;_tmp563;});_tmp564->f1=_tmp901;});_tmp564;});
# 2097
_tmp565->hd=_tmp902;}),_tmp565->tl=tms;_tmp565;});
# 2101
goto _LL16;}else{_LL19: _LL1A:
# 2103
 tms=({struct Cyc_List_List*_tmp568=_region_malloc(r,sizeof(*_tmp568));({void*_tmp904=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp567=_region_malloc(r,sizeof(*_tmp567));
_tmp567->tag=3U,({void*_tmp903=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp566=_region_malloc(r,sizeof(*_tmp566));_tmp566->tag=1U,_tmp566->f1=args,_tmp566->f2=c_varargs,_tmp566->f3=cyc_varargs,_tmp566->f4=effopt,_tmp566->f5=rgn_po,_tmp566->f6=req,_tmp566->f7=ens;_tmp566;});_tmp567->f1=_tmp903;});_tmp567;});
# 2103
_tmp568->hd=_tmp904;}),_tmp568->tl=tms;_tmp568;});
# 2107
for(0;fn_atts != 0;fn_atts=fn_atts->tl){
void*_tmp569=(void*)fn_atts->hd;void*_stmttmp27=_tmp569;void*_tmp56A=_stmttmp27;switch(*((int*)_tmp56A)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2112
 tms=({struct Cyc_List_List*_tmp56D=_region_malloc(r,sizeof(*_tmp56D));({void*_tmp906=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp56C=_region_malloc(r,sizeof(*_tmp56C));_tmp56C->tag=5U,_tmp56C->f1=0U,({struct Cyc_List_List*_tmp905=({struct Cyc_List_List*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->hd=(void*)fn_atts->hd,_tmp56B->tl=0;_tmp56B;});_tmp56C->f2=_tmp905;});_tmp56C;});_tmp56D->hd=_tmp906;}),_tmp56D->tl=tms;_tmp56D;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2116
goto _LL16;}_LL16:;}
# 2119
AfterAtts:
 if(typvars != 0)
tms=({struct Cyc_List_List*_tmp56F=_region_malloc(r,sizeof(*_tmp56F));({void*_tmp907=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp56E=_region_malloc(r,sizeof(*_tmp56E));_tmp56E->tag=4U,_tmp56E->f1=typvars,_tmp56E->f2=0U,_tmp56E->f3=1;_tmp56E;});_tmp56F->hd=_tmp907;}),_tmp56F->tl=tms;_tmp56F;});
return({struct _tuple13 _tmp5D0;_tmp5D0.f1=tq3,_tmp5D0.f2=t3,_tmp5D0.f3=tms;_tmp5D0;});}}}case 1U: _LL7: _tmp537=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp532)->f1;_tmp538=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp532)->f2;_tmp539=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp532)->f3;_LL8: {struct Cyc_Core_Opt*k=_tmp537;void*topt=_tmp538;int i=_tmp539;
# 2125
if(topt == 0)
return({struct _tuple13 _tmp5D1;_tmp5D1.f1=tq,_tmp5D1.f2=t,_tmp5D1.f3=0;_tmp5D1;});else{
# 2128
return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8U: _LL9: _tmp533=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp532)->f1;_tmp534=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp532)->f2;_tmp535=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp532)->f3;_tmp536=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp532)->f4;_LLA: {struct _tuple0*n=_tmp533;struct Cyc_List_List*ts=_tmp534;struct Cyc_Absyn_Typedefdecl*td=_tmp535;void*topt=_tmp536;
# 2134
if(topt == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple13 _tmp5D2;_tmp5D2.f1=tq,_tmp5D2.f2=t,_tmp5D2.f3=0;_tmp5D2;});else{
# 2137
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}}default: _LLB: _LLC:
# 2142
 return({struct _tuple13 _tmp5D3;_tmp5D3.f1=tq,_tmp5D3.f2=t,_tmp5D3.f3=0;_tmp5D3;});}_LL0:;}
# 2146
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2148
void*_tmp570=t;void*_tmp571;void*_tmp572;switch(*((int*)_tmp570)){case 1U: _LL1: _tmp572=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp570)->f2;if(_tmp572 != 0){_LL2: {void*def=_tmp572;
return Cyc_Absynpp_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp571=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp570)->f1).elt_type;_LL4: {void*elt_typ=_tmp571;
# 2151
L: {
void*_tmp573=elt_typ;void*_tmp574;void*_tmp575;switch(*((int*)_tmp573)){case 1U: _LL8: _tmp575=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp573)->f2;if(_tmp575 != 0){_LL9: {void*t=_tmp575;
elt_typ=t;goto L;}}else{goto _LLE;}case 8U: _LLA: _tmp574=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp573)->f4;if(_tmp574 != 0){_LLB: {void*t=_tmp574;
elt_typ=t;goto L;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp573)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp573)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 2158
 return 0;}_LL0:;}
# 2162
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp576=_new_region("temp");struct _RegionHandle*temp=& _tmp576;_push_region(temp);
{struct _tuple13 _tmp577=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple13 _stmttmp28=_tmp577;struct _tuple13 _tmp578=_stmttmp28;struct Cyc_List_List*_tmp57B;void*_tmp57A;struct Cyc_Absyn_Tqual _tmp579;_LL1: _tmp579=_tmp578.f1;_tmp57A=_tmp578.f2;_tmp57B=_tmp578.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp579;void*t=_tmp57A;struct Cyc_List_List*tms=_tmp57B;
tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);
if(tms == 0 && dopt == 0){
struct Cyc_PP_Doc*_tmp57D=({struct Cyc_PP_Doc*_tmp57C[2U];({struct Cyc_PP_Doc*_tmp909=Cyc_Absynpp_tqual2doc(tq);_tmp57C[0]=_tmp909;}),({struct Cyc_PP_Doc*_tmp908=Cyc_Absynpp_ntyp2doc(t);_tmp57C[1]=_tmp908;});Cyc_PP_cat(_tag_fat(_tmp57C,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp57D;}else{
# 2169
struct Cyc_PP_Doc*_tmp580=({struct Cyc_PP_Doc*_tmp57E[4U];({
struct Cyc_PP_Doc*_tmp90F=Cyc_Absynpp_tqual2doc(tq);_tmp57E[0]=_tmp90F;}),({
struct Cyc_PP_Doc*_tmp90E=Cyc_Absynpp_ntyp2doc(t);_tmp57E[1]=_tmp90E;}),({
struct Cyc_PP_Doc*_tmp90D=Cyc_PP_text(({const char*_tmp57F=" ";_tag_fat(_tmp57F,sizeof(char),2U);}));_tmp57E[2]=_tmp90D;}),({
struct Cyc_PP_Doc*_tmp90C=({int _tmp90B=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp90A=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp90B,_tmp90A,tms);});_tmp57E[3]=_tmp90C;});Cyc_PP_cat(_tag_fat(_tmp57E,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp580;}}}
# 2164
;_pop_region();}
# 2177
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp581=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp581;
if((unsigned)req)
requires_doc=({struct Cyc_PP_Doc*_tmp582[2U];({struct Cyc_PP_Doc*_tmp911=Cyc_PP_text(({const char*_tmp583="@requires ";_tag_fat(_tmp583,sizeof(char),11U);}));_tmp582[0]=_tmp911;}),({struct Cyc_PP_Doc*_tmp910=Cyc_Absynpp_exp2doc(req);_tmp582[1]=_tmp910;});Cyc_PP_cat(_tag_fat(_tmp582,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2183
requires_doc=Cyc_PP_nil_doc();}{
# 2185
enum Cyc_Flags_C_Compilers _tmp584=Cyc_Flags_c_compiler;if(_tmp584 == Cyc_Flags_Gcc_c){_LL1: _LL2:
# 2188
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp585[5U];({struct Cyc_PP_Doc*_tmp919=({struct Cyc_Absyn_Tqual _tmp918=f->tq;void*_tmp917=f->type;Cyc_Absynpp_tqtd2doc(_tmp918,_tmp917,({struct Cyc_Core_Opt*_tmp586=_cycalloc(sizeof(*_tmp586));({struct Cyc_PP_Doc*_tmp916=Cyc_PP_textptr(f->name);_tmp586->v=_tmp916;});_tmp586;}));});_tmp585[0]=_tmp919;}),({
struct Cyc_PP_Doc*_tmp915=Cyc_PP_text(({const char*_tmp587=":";_tag_fat(_tmp587,sizeof(char),2U);}));_tmp585[1]=_tmp915;}),({struct Cyc_PP_Doc*_tmp914=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp585[2]=_tmp914;}),({
struct Cyc_PP_Doc*_tmp913=Cyc_Absynpp_atts2doc(f->attributes);_tmp585[3]=_tmp913;}),({struct Cyc_PP_Doc*_tmp912=Cyc_PP_text(({const char*_tmp588=";";_tag_fat(_tmp588,sizeof(char),2U);}));_tmp585[4]=_tmp912;});Cyc_PP_cat(_tag_fat(_tmp585,sizeof(struct Cyc_PP_Doc*),5U));});
return({struct Cyc_PP_Doc*_tmp589[4U];({struct Cyc_PP_Doc*_tmp91F=({struct Cyc_Absyn_Tqual _tmp91E=f->tq;void*_tmp91D=f->type;Cyc_Absynpp_tqtd2doc(_tmp91E,_tmp91D,({struct Cyc_Core_Opt*_tmp58A=_cycalloc(sizeof(*_tmp58A));({struct Cyc_PP_Doc*_tmp91C=Cyc_PP_textptr(f->name);_tmp58A->v=_tmp91C;});_tmp58A;}));});_tmp589[0]=_tmp91F;}),({
struct Cyc_PP_Doc*_tmp91B=Cyc_Absynpp_atts2doc(f->attributes);_tmp589[1]=_tmp91B;}),_tmp589[2]=requires_doc,({struct Cyc_PP_Doc*_tmp91A=Cyc_PP_text(({const char*_tmp58B=";";_tag_fat(_tmp58B,sizeof(char),2U);}));_tmp589[3]=_tmp91A;});Cyc_PP_cat(_tag_fat(_tmp589,sizeof(struct Cyc_PP_Doc*),4U));});}else{_LL3: _LL4:
# 2195
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp58C[5U];({struct Cyc_PP_Doc*_tmp927=Cyc_Absynpp_atts2doc(f->attributes);_tmp58C[0]=_tmp927;}),({
struct Cyc_PP_Doc*_tmp926=({struct Cyc_Absyn_Tqual _tmp925=f->tq;void*_tmp924=f->type;Cyc_Absynpp_tqtd2doc(_tmp925,_tmp924,({struct Cyc_Core_Opt*_tmp58D=_cycalloc(sizeof(*_tmp58D));({struct Cyc_PP_Doc*_tmp923=Cyc_PP_textptr(f->name);_tmp58D->v=_tmp923;});_tmp58D;}));});_tmp58C[1]=_tmp926;}),({
struct Cyc_PP_Doc*_tmp922=Cyc_PP_text(({const char*_tmp58E=":";_tag_fat(_tmp58E,sizeof(char),2U);}));_tmp58C[2]=_tmp922;}),({struct Cyc_PP_Doc*_tmp921=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp58C[3]=_tmp921;}),({struct Cyc_PP_Doc*_tmp920=Cyc_PP_text(({const char*_tmp58F=";";_tag_fat(_tmp58F,sizeof(char),2U);}));_tmp58C[4]=_tmp920;});Cyc_PP_cat(_tag_fat(_tmp58C,sizeof(struct Cyc_PP_Doc*),5U));});
return({struct Cyc_PP_Doc*_tmp590[4U];({struct Cyc_PP_Doc*_tmp92D=Cyc_Absynpp_atts2doc(f->attributes);_tmp590[0]=_tmp92D;}),({
struct Cyc_PP_Doc*_tmp92C=({struct Cyc_Absyn_Tqual _tmp92B=f->tq;void*_tmp92A=f->type;Cyc_Absynpp_tqtd2doc(_tmp92B,_tmp92A,({struct Cyc_Core_Opt*_tmp591=_cycalloc(sizeof(*_tmp591));({struct Cyc_PP_Doc*_tmp929=Cyc_PP_textptr(f->name);_tmp591->v=_tmp929;});_tmp591;}));});_tmp590[1]=_tmp92C;}),_tmp590[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp928=Cyc_PP_text(({const char*_tmp592=";";_tag_fat(_tmp592,sizeof(char),2U);}));_tmp590[3]=_tmp928;});Cyc_PP_cat(_tag_fat(_tmp590,sizeof(struct Cyc_PP_Doc*),4U));});}_LL0:;}}
# 2206
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp92E=({const char*_tmp593="";_tag_fat(_tmp593,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp92E,fields);});}
# 2210
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp594[3U];({struct Cyc_PP_Doc*_tmp932=Cyc_Absynpp_scope2doc(f->sc);_tmp594[0]=_tmp932;}),({struct Cyc_PP_Doc*_tmp931=Cyc_Absynpp_typedef_name2doc(f->name);_tmp594[1]=_tmp931;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp930=Cyc_PP_nil_doc();_tmp594[2]=_tmp930;}):({struct Cyc_PP_Doc*_tmp92F=Cyc_Absynpp_args2doc(f->typs);_tmp594[2]=_tmp92F;});Cyc_PP_cat(_tag_fat(_tmp594,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2215
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp933=({const char*_tmp595=",";_tag_fat(_tmp595,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp933,fields);});}
# 2224
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp934=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp934,72,f);});
({void*_tmp596=0U;({struct Cyc___cycFILE*_tmp936=f;struct _fat_ptr _tmp935=({const char*_tmp597="\n";_tag_fat(_tmp597,sizeof(char),2U);});Cyc_fprintf(_tmp936,_tmp935,_tag_fat(_tmp596,sizeof(void*),0U));});});}}
# 2231
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _tmp937=({const char*_tmp598="";_tag_fat(_tmp598,sizeof(char),1U);});Cyc_PP_seql(_tmp937,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2234
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0,0),72);}
struct _fat_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _fat_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _fat_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;}}
# 2248
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
# 2253
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*t){
if(t == 0)
return(struct _fat_ptr)({const char*_tmp599="NULL";_tag_fat(_tmp599,sizeof(char),5U);});{
void*_tmp59A=t;struct Cyc_Absyn_Typedefdecl*_tmp59B;switch(*((int*)_tmp59A)){case 0U: _LL1: _LL2:
 return(struct _fat_ptr)({const char*_tmp59C="AppType";_tag_fat(_tmp59C,sizeof(char),8U);});case 1U: _LL3: _LL4:
 return(struct _fat_ptr)({const char*_tmp59D="Evar";_tag_fat(_tmp59D,sizeof(char),5U);});case 2U: _LL5: _LL6:
 return(struct _fat_ptr)({const char*_tmp59E="Vartype";_tag_fat(_tmp59E,sizeof(char),8U);});case 3U: _LL7: _LL8:
 return(struct _fat_ptr)({const char*_tmp59F="Pointertype";_tag_fat(_tmp59F,sizeof(char),12U);});case 4U: _LL9: _LLA:
 return(struct _fat_ptr)({const char*_tmp5A0="ArrayType";_tag_fat(_tmp5A0,sizeof(char),10U);});case 5U: _LLB: _LLC:
 return(struct _fat_ptr)({const char*_tmp5A1="FnType";_tag_fat(_tmp5A1,sizeof(char),7U);});case 6U: _LLD: _LLE:
 return(struct _fat_ptr)({const char*_tmp5A2="TupleType";_tag_fat(_tmp5A2,sizeof(char),10U);});case 7U: _LLF: _LL10:
 return(struct _fat_ptr)({const char*_tmp5A3="AnonAggrType";_tag_fat(_tmp5A3,sizeof(char),13U);});case 8U: _LL11: _tmp59B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp59A)->f3;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp59B;
# 2266
return(struct _fat_ptr)({const char*_tmp5A4="Typedeftype";_tag_fat(_tmp5A4,sizeof(char),12U);});}case 9U: _LL13: _LL14:
 return(struct _fat_ptr)({const char*_tmp5A5="Valueoftype";_tag_fat(_tmp5A5,sizeof(char),12U);});case 10U: _LL15: _LL16:
 return(struct _fat_ptr)({const char*_tmp5A6="Typedecltype";_tag_fat(_tmp5A6,sizeof(char),13U);});default: _LL17: _LL18:
 return(struct _fat_ptr)({const char*_tmp5A7="Typeoftype";_tag_fat(_tmp5A7,sizeof(char),11U);});}_LL0:;}}

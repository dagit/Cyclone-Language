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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
extern struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;
# 53 "/tmp/cyclone/include/cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Buffer_t;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "/tmp/cyclone/include/list.h"
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
# 38 "/tmp/cyclone/include/string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 50 "/tmp/cyclone/include/string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 696 "/tmp/cyclone/src/absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 882
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 891
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 896
void*Cyc_Absyn_compress_kb(void*);
# 900
void*Cyc_Absyn_compress(void*);
# 905
int Cyc_Absyn_type2bool(int def,void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1149
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1157
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple12{unsigned f1;int f2;};
# 28 "/tmp/cyclone/src/evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "/tmp/cyclone/src/relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 257 "/tmp/cyclone/src/tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 40 "/tmp/cyclone/src/warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 53 "/tmp/cyclone/src/flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 57
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 39 "/tmp/cyclone/include/pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 50
extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
extern struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "/tmp/cyclone/include/pp.h"
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
# 52 "/tmp/cyclone/src/absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
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
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _fat_ptr Cyc_Absynpp_cyc_string;
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple14{int f1;struct Cyc_List_List*f2;};
# 95 "/tmp/cyclone/src/absynpp.h"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 35 "/tmp/cyclone/src/absynpp.cyc"
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
struct _fat_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 72
static int Cyc_Absynpp_add_cyc_prefix;
# 76
static int Cyc_Absynpp_to_VC;
# 79
static int Cyc_Absynpp_decls_first;
# 83
static int Cyc_Absynpp_rewrite_temp_tvars;
# 86
static int Cyc_Absynpp_print_all_tvars;
# 89
static int Cyc_Absynpp_print_all_kinds;
# 92
static int Cyc_Absynpp_print_all_effects;
# 95
static int Cyc_Absynpp_print_using_stmts;
# 100
static int Cyc_Absynpp_print_externC_stmts;
# 104
static int Cyc_Absynpp_print_full_evars;
# 107
static int Cyc_Absynpp_generate_line_directives;
# 110
static int Cyc_Absynpp_use_curr_namespace;
# 113
static int Cyc_Absynpp_print_zeroterm;
# 116
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 119
int Cyc_Absynpp_print_for_cycdoc=0;
# 140
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
# 159
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 179
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 199
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 219
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 240
static void Cyc_Absynpp_curr_namespace_add(struct _fat_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp5E4=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5E4,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v,_tmp1->tl=0;_tmp1;}));});}
# 244
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 248
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 252
static void Cyc_Absynpp_curr_namespace_drop (void){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 256
struct _fat_ptr Cyc_Absynpp_char_escape(char c){
char _tmp2=c;switch(_tmp2){case 7U: _LL1: _LL2:
 return({const char*_tmp3="\\a";_tag_fat(_tmp3,sizeof(char),3U);});case 8U: _LL3: _LL4:
 return({const char*_tmp4="\\b";_tag_fat(_tmp4,sizeof(char),3U);});case 12U: _LL5: _LL6:
 return({const char*_tmp5="\\f";_tag_fat(_tmp5,sizeof(char),3U);});case 10U: _LL7: _LL8:
 return({const char*_tmp6="\\n";_tag_fat(_tmp6,sizeof(char),3U);});case 13U: _LL9: _LLA:
 return({const char*_tmp7="\\r";_tag_fat(_tmp7,sizeof(char),3U);});case 9U: _LLB: _LLC:
 return({const char*_tmp8="\\t";_tag_fat(_tmp8,sizeof(char),3U);});case 11U: _LLD: _LLE:
 return({const char*_tmp9="\\v";_tag_fat(_tmp9,sizeof(char),3U);});case 92U: _LLF: _LL10:
 return({const char*_tmpA="\\\\";_tag_fat(_tmpA,sizeof(char),3U);});case 34U: _LL11: _LL12:
 return({const char*_tmpB="\"";_tag_fat(_tmpB,sizeof(char),2U);});case 39U: _LL13: _LL14:
 return({const char*_tmpC="\\'";_tag_fat(_tmpC,sizeof(char),3U);});default: _LL15: _LL16:
# 269
 if((int)c >= (int)' ' &&(int)c <= (int)'~'){
struct _fat_ptr _tmpD=Cyc_Core_new_string(2U);struct _fat_ptr t=_tmpD;
({struct _fat_ptr _tmpE=_fat_ptr_plus(t,sizeof(char),0);char _tmpF=*((char*)_check_fat_subscript(_tmpE,sizeof(char),0U));char _tmp10=c;if(_get_fat_size(_tmpE,sizeof(char))== 1U &&(_tmpF == 0 && _tmp10 != 0))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _fat_ptr)t;}else{
# 274
struct _fat_ptr _tmp11=Cyc_Core_new_string(5U);struct _fat_ptr t=_tmp11;
int j=0;
({struct _fat_ptr _tmp12=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp13=*((char*)_check_fat_subscript(_tmp12,sizeof(char),0U));char _tmp14='\\';if(_get_fat_size(_tmp12,sizeof(char))== 1U &&(_tmp13 == 0 && _tmp14 != 0))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _fat_ptr _tmp15=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp16=*((char*)_check_fat_subscript(_tmp15,sizeof(char),0U));char _tmp17=(char)((int)'0' + ((int)((unsigned char)c)>> 6 & 3));if(_get_fat_size(_tmp15,sizeof(char))== 1U &&(_tmp16 == 0 && _tmp17 != 0))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _fat_ptr _tmp18=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp19=*((char*)_check_fat_subscript(_tmp18,sizeof(char),0U));char _tmp1A=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_fat_size(_tmp18,sizeof(char))== 1U &&(_tmp19 == 0 && _tmp1A != 0))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _fat_ptr _tmp1B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp1C=*((char*)_check_fat_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=(char)((int)'0' + ((int)c & 7));if(_get_fat_size(_tmp1B,sizeof(char))== 1U &&(_tmp1C == 0 && _tmp1D != 0))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _fat_ptr)t;}}_LL0:;}
# 285
static int Cyc_Absynpp_special(struct _fat_ptr s){
int sz=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((((int)c <= (int)' ' ||(int)c >= (int)'~')||(int)c == (int)'"')||(int)c == (int)'\\')
return 1;}}
# 292
return 0;}
# 295
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 298
int n=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
# 300
if(n > 0 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),n))== (int)'\000')-- n;{
# 302
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1E=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _stmttmp0=_tmp1E;char _tmp1F=_stmttmp0;char _tmp20;switch(_tmp1F){case 7U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL8;case 10U: _LL7: _LL8:
 goto _LLA;case 13U: _LL9: _LLA:
 goto _LLC;case 9U: _LLB: _LLC:
 goto _LLE;case 11U: _LLD: _LLE:
 goto _LL10;case 92U: _LLF: _LL10:
 goto _LL12;case 34U: _LL11: _LL12:
 len +=2;goto _LL0;default: _LL13: _tmp20=_tmp1F;_LL14: {char c=_tmp20;
# 315
if((int)c >= (int)' ' &&(int)c <= (int)'~')++ len;else{
len +=4;}
goto _LL0;}}_LL0:;}}{
# 320
struct _fat_ptr t=Cyc_Core_new_string((unsigned)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp21=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _stmttmp1=_tmp21;char _tmp22=_stmttmp1;char _tmp23;switch(_tmp22){case 7U: _LL16: _LL17:
({struct _fat_ptr _tmp24=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp25=*((char*)_check_fat_subscript(_tmp24,sizeof(char),0U));char _tmp26='\\';if(_get_fat_size(_tmp24,sizeof(char))== 1U &&(_tmp25 == 0 && _tmp26 != 0))_throw_arraybounds();*((char*)_tmp24.curr)=_tmp26;});({struct _fat_ptr _tmp27=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp28=*((char*)_check_fat_subscript(_tmp27,sizeof(char),0U));char _tmp29='a';if(_get_fat_size(_tmp27,sizeof(char))== 1U &&(_tmp28 == 0 && _tmp29 != 0))_throw_arraybounds();*((char*)_tmp27.curr)=_tmp29;});goto _LL15;case 8U: _LL18: _LL19:
({struct _fat_ptr _tmp2A=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp2B=*((char*)_check_fat_subscript(_tmp2A,sizeof(char),0U));char _tmp2C='\\';if(_get_fat_size(_tmp2A,sizeof(char))== 1U &&(_tmp2B == 0 && _tmp2C != 0))_throw_arraybounds();*((char*)_tmp2A.curr)=_tmp2C;});({struct _fat_ptr _tmp2D=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp2E=*((char*)_check_fat_subscript(_tmp2D,sizeof(char),0U));char _tmp2F='b';if(_get_fat_size(_tmp2D,sizeof(char))== 1U &&(_tmp2E == 0 && _tmp2F != 0))_throw_arraybounds();*((char*)_tmp2D.curr)=_tmp2F;});goto _LL15;case 12U: _LL1A: _LL1B:
({struct _fat_ptr _tmp30=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp31=*((char*)_check_fat_subscript(_tmp30,sizeof(char),0U));char _tmp32='\\';if(_get_fat_size(_tmp30,sizeof(char))== 1U &&(_tmp31 == 0 && _tmp32 != 0))_throw_arraybounds();*((char*)_tmp30.curr)=_tmp32;});({struct _fat_ptr _tmp33=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp34=*((char*)_check_fat_subscript(_tmp33,sizeof(char),0U));char _tmp35='f';if(_get_fat_size(_tmp33,sizeof(char))== 1U &&(_tmp34 == 0 && _tmp35 != 0))_throw_arraybounds();*((char*)_tmp33.curr)=_tmp35;});goto _LL15;case 10U: _LL1C: _LL1D:
({struct _fat_ptr _tmp36=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp37=*((char*)_check_fat_subscript(_tmp36,sizeof(char),0U));char _tmp38='\\';if(_get_fat_size(_tmp36,sizeof(char))== 1U &&(_tmp37 == 0 && _tmp38 != 0))_throw_arraybounds();*((char*)_tmp36.curr)=_tmp38;});({struct _fat_ptr _tmp39=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp3A=*((char*)_check_fat_subscript(_tmp39,sizeof(char),0U));char _tmp3B='n';if(_get_fat_size(_tmp39,sizeof(char))== 1U &&(_tmp3A == 0 && _tmp3B != 0))_throw_arraybounds();*((char*)_tmp39.curr)=_tmp3B;});goto _LL15;case 13U: _LL1E: _LL1F:
({struct _fat_ptr _tmp3C=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp3D=*((char*)_check_fat_subscript(_tmp3C,sizeof(char),0U));char _tmp3E='\\';if(_get_fat_size(_tmp3C,sizeof(char))== 1U &&(_tmp3D == 0 && _tmp3E != 0))_throw_arraybounds();*((char*)_tmp3C.curr)=_tmp3E;});({struct _fat_ptr _tmp3F=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp40=*((char*)_check_fat_subscript(_tmp3F,sizeof(char),0U));char _tmp41='r';if(_get_fat_size(_tmp3F,sizeof(char))== 1U &&(_tmp40 == 0 && _tmp41 != 0))_throw_arraybounds();*((char*)_tmp3F.curr)=_tmp41;});goto _LL15;case 9U: _LL20: _LL21:
({struct _fat_ptr _tmp42=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp43=*((char*)_check_fat_subscript(_tmp42,sizeof(char),0U));char _tmp44='\\';if(_get_fat_size(_tmp42,sizeof(char))== 1U &&(_tmp43 == 0 && _tmp44 != 0))_throw_arraybounds();*((char*)_tmp42.curr)=_tmp44;});({struct _fat_ptr _tmp45=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp46=*((char*)_check_fat_subscript(_tmp45,sizeof(char),0U));char _tmp47='t';if(_get_fat_size(_tmp45,sizeof(char))== 1U &&(_tmp46 == 0 && _tmp47 != 0))_throw_arraybounds();*((char*)_tmp45.curr)=_tmp47;});goto _LL15;case 11U: _LL22: _LL23:
({struct _fat_ptr _tmp48=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp49=*((char*)_check_fat_subscript(_tmp48,sizeof(char),0U));char _tmp4A='\\';if(_get_fat_size(_tmp48,sizeof(char))== 1U &&(_tmp49 == 0 && _tmp4A != 0))_throw_arraybounds();*((char*)_tmp48.curr)=_tmp4A;});({struct _fat_ptr _tmp4B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp4C=*((char*)_check_fat_subscript(_tmp4B,sizeof(char),0U));char _tmp4D='v';if(_get_fat_size(_tmp4B,sizeof(char))== 1U &&(_tmp4C == 0 && _tmp4D != 0))_throw_arraybounds();*((char*)_tmp4B.curr)=_tmp4D;});goto _LL15;case 92U: _LL24: _LL25:
({struct _fat_ptr _tmp4E=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp4F=*((char*)_check_fat_subscript(_tmp4E,sizeof(char),0U));char _tmp50='\\';if(_get_fat_size(_tmp4E,sizeof(char))== 1U &&(_tmp4F == 0 && _tmp50 != 0))_throw_arraybounds();*((char*)_tmp4E.curr)=_tmp50;});({struct _fat_ptr _tmp51=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp52=*((char*)_check_fat_subscript(_tmp51,sizeof(char),0U));char _tmp53='\\';if(_get_fat_size(_tmp51,sizeof(char))== 1U &&(_tmp52 == 0 && _tmp53 != 0))_throw_arraybounds();*((char*)_tmp51.curr)=_tmp53;});goto _LL15;case 34U: _LL26: _LL27:
({struct _fat_ptr _tmp54=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp55=*((char*)_check_fat_subscript(_tmp54,sizeof(char),0U));char _tmp56='\\';if(_get_fat_size(_tmp54,sizeof(char))== 1U &&(_tmp55 == 0 && _tmp56 != 0))_throw_arraybounds();*((char*)_tmp54.curr)=_tmp56;});({struct _fat_ptr _tmp57=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp58=*((char*)_check_fat_subscript(_tmp57,sizeof(char),0U));char _tmp59='"';if(_get_fat_size(_tmp57,sizeof(char))== 1U &&(_tmp58 == 0 && _tmp59 != 0))_throw_arraybounds();*((char*)_tmp57.curr)=_tmp59;});goto _LL15;default: _LL28: _tmp23=_tmp22;_LL29: {char c=_tmp23;
# 334
if((int)c >= (int)' ' &&(int)c <= (int)'~')({struct _fat_ptr _tmp5A=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5B=*((char*)_check_fat_subscript(_tmp5A,sizeof(char),0U));char _tmp5C=c;if(_get_fat_size(_tmp5A,sizeof(char))== 1U &&(_tmp5B == 0 && _tmp5C != 0))_throw_arraybounds();*((char*)_tmp5A.curr)=_tmp5C;});else{
# 336
unsigned char uc=(unsigned char)c;
# 339
({struct _fat_ptr _tmp5D=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5E=*((char*)_check_fat_subscript(_tmp5D,sizeof(char),0U));char _tmp5F='\\';if(_get_fat_size(_tmp5D,sizeof(char))== 1U &&(_tmp5E == 0 && _tmp5F != 0))_throw_arraybounds();*((char*)_tmp5D.curr)=_tmp5F;});
({struct _fat_ptr _tmp60=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp61=*((char*)_check_fat_subscript(_tmp60,sizeof(char),0U));char _tmp62=(char)((int)'0' + ((int)uc >> 6 & 7));if(_get_fat_size(_tmp60,sizeof(char))== 1U &&(_tmp61 == 0 && _tmp62 != 0))_throw_arraybounds();*((char*)_tmp60.curr)=_tmp62;});
({struct _fat_ptr _tmp63=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp64=*((char*)_check_fat_subscript(_tmp63,sizeof(char),0U));char _tmp65=(char)((int)'0' + ((int)uc >> 3 & 7));if(_get_fat_size(_tmp63,sizeof(char))== 1U &&(_tmp64 == 0 && _tmp65 != 0))_throw_arraybounds();*((char*)_tmp63.curr)=_tmp65;});
({struct _fat_ptr _tmp66=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp67=*((char*)_check_fat_subscript(_tmp66,sizeof(char),0U));char _tmp68=(char)((int)'0' + ((int)uc & 7));if(_get_fat_size(_tmp66,sizeof(char))== 1U &&(_tmp67 == 0 && _tmp68 != 0))_throw_arraybounds();*((char*)_tmp66.curr)=_tmp68;});}
# 344
goto _LL15;}}_LL15:;}}
# 346
return(struct _fat_ptr)t;}}}}static char _tmp69[9U]="restrict";
# 349
static struct _fat_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9U};static char _tmp6A[9U]="volatile";
static struct _fat_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9U};static char _tmp6B[6U]="const";
static struct _fat_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6U};
static struct _fat_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _fat_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _fat_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 356
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 359
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp,_tmp6C->tl=l;_tmp6C;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp,_tmp6D->tl=l;_tmp6D;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp,_tmp6E->tl=l;_tmp6E;});
return({struct _fat_ptr _tmp5E7=({const char*_tmp6F="";_tag_fat(_tmp6F,sizeof(char),1U);});struct _fat_ptr _tmp5E6=({const char*_tmp70=" ";_tag_fat(_tmp70,sizeof(char),2U);});struct _fat_ptr _tmp5E5=({const char*_tmp71=" ";_tag_fat(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp5E7,_tmp5E6,_tmp5E5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 365
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_AliasQual _tmp74;enum Cyc_Absyn_KindQual _tmp73;_LL1: _tmp73=_tmp72->kind;_tmp74=_tmp72->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp73;enum Cyc_Absyn_AliasQual a=_tmp74;
enum Cyc_Absyn_KindQual _tmp75=k;switch(_tmp75){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 369
enum Cyc_Absyn_AliasQual _tmp76=a;switch(_tmp76){case Cyc_Absyn_Aliasable: _LL17: _LL18:
 return({const char*_tmp77="A";_tag_fat(_tmp77,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL19: _LL1A:
 return({const char*_tmp78="UA";_tag_fat(_tmp78,sizeof(char),3U);});case Cyc_Absyn_Top: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return({const char*_tmp79="TA";_tag_fat(_tmp79,sizeof(char),3U);});}_LL16:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 375
enum Cyc_Absyn_AliasQual _tmp7A=a;switch(_tmp7A){case Cyc_Absyn_Aliasable: _LL20: _LL21:
 return({const char*_tmp7B="M";_tag_fat(_tmp7B,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL22: _LL23:
 return({const char*_tmp7C="UM";_tag_fat(_tmp7C,sizeof(char),3U);});case Cyc_Absyn_Top: _LL24: _LL25:
 goto _LL27;default: _LL26: _LL27: return({const char*_tmp7D="TM";_tag_fat(_tmp7D,sizeof(char),3U);});}_LL1F:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 381
enum Cyc_Absyn_AliasQual _tmp7E=a;switch(_tmp7E){case Cyc_Absyn_Aliasable: _LL29: _LL2A:
 return({const char*_tmp7F="B";_tag_fat(_tmp7F,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL2B: _LL2C:
 return({const char*_tmp80="UB";_tag_fat(_tmp80,sizeof(char),3U);});case Cyc_Absyn_Top: _LL2D: _LL2E:
 goto _LL30;default: _LL2F: _LL30: return({const char*_tmp81="TB";_tag_fat(_tmp81,sizeof(char),3U);});}_LL28:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 387
enum Cyc_Absyn_AliasQual _tmp82=a;switch(_tmp82){case Cyc_Absyn_Aliasable: _LL32: _LL33:
 return({const char*_tmp83="R";_tag_fat(_tmp83,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL34: _LL35:
 return({const char*_tmp84="UR";_tag_fat(_tmp84,sizeof(char),3U);});case Cyc_Absyn_Top: _LL36: _LL37:
 goto _LL39;default: _LL38: _LL39: return({const char*_tmp85="TR";_tag_fat(_tmp85,sizeof(char),3U);});}_LL31:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 392
 return({const char*_tmp86="E";_tag_fat(_tmp86,sizeof(char),2U);});case Cyc_Absyn_IntKind: _LLE: _LLF:
 return({const char*_tmp87="I";_tag_fat(_tmp87,sizeof(char),2U);});case Cyc_Absyn_BoolKind: _LL10: _LL11:
 return({const char*_tmp88="BOOL";_tag_fat(_tmp88,sizeof(char),5U);});case Cyc_Absyn_PtrBndKind: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15: return({const char*_tmp89="PTRBND";_tag_fat(_tmp89,sizeof(char),7U);});}_LL3:;}}
# 398
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 400
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp8A=Cyc_Absyn_compress_kb(c);void*_stmttmp2=_tmp8A;void*_tmp8B=_stmttmp2;struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8D;switch(*((int*)_tmp8B)){case 0U: _LL1: _tmp8D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp8D;
return Cyc_Absynpp_kind2string(k);}case 1U: _LL3: _LL4:
# 404
 if(Cyc_PP_tex_output)
return({const char*_tmp8E="{?}";_tag_fat(_tmp8E,sizeof(char),4U);});else{
return({const char*_tmp8F="?";_tag_fat(_tmp8F,sizeof(char),2U);});}default: _LL5: _tmp8C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp8C;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp92=({struct Cyc_String_pa_PrintArg_struct _tmp5B8;_tmp5B8.tag=0U,({struct _fat_ptr _tmp5E8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k));_tmp5B8.f1=_tmp5E8;});_tmp5B8;});void*_tmp90[1U];_tmp90[0]=& _tmp92;({struct _fat_ptr _tmp5E9=({const char*_tmp91="<=%s";_tag_fat(_tmp91,sizeof(char),5U);});Cyc_aprintf(_tmp5E9,_tag_fat(_tmp90,sizeof(void*),1U));});});}}_LL0:;}
# 411
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp93=Cyc_Absyn_compress_kb(c);void*_stmttmp3=_tmp93;void*_tmp94=_stmttmp3;struct Cyc_Absyn_Kind*_tmp95;struct Cyc_Absyn_Kind*_tmp96;switch(*((int*)_tmp94)){case 0U: _LL1: _tmp96=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp96;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1U: _LL3: _LL4:
# 415
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp97="{?}";_tag_fat(_tmp97,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp98="?";_tag_fat(_tmp98,sizeof(char),2U);}));}default: _LL5: _tmp95=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp95;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}}_LL0:;}
# 422
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp5EC=({const char*_tmp99="<";_tag_fat(_tmp99,sizeof(char),2U);});struct _fat_ptr _tmp5EB=({const char*_tmp9A=">";_tag_fat(_tmp9A,sizeof(char),2U);});struct _fat_ptr _tmp5EA=({const char*_tmp9B=",";_tag_fat(_tmp9B,sizeof(char),2U);});Cyc_PP_egroup(_tmp5EC,_tmp5EB,_tmp5EA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 426
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr _tmp9C=*tv->name;struct _fat_ptr n=_tmp9C;
# 430
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
# 432
struct _fat_ptr kstring=({const char*_tmpA5="K";_tag_fat(_tmpA5,sizeof(char),2U);});
{void*_tmp9D=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp4=_tmp9D;void*_tmp9E=_stmttmp4;struct Cyc_Absyn_Kind*_tmp9F;struct Cyc_Absyn_Kind*_tmpA0;switch(*((int*)_tmp9E)){case 2U: _LL1: _tmpA0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f2;_LL2: {struct Cyc_Absyn_Kind*k=_tmpA0;
_tmp9F=k;goto _LL4;}case 0U: _LL3: _tmp9F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9E)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_tmp9F;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 438
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp5BA;_tmp5BA.tag=0U,_tmp5BA.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_tmp5BA;});struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp5B9;_tmp5B9.tag=0U,({struct _fat_ptr _tmp5ED=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(n,sizeof(char),1));_tmp5B9.f1=_tmp5ED;});_tmp5B9;});void*_tmpA1[2U];_tmpA1[0]=& _tmpA3,_tmpA1[1]=& _tmpA4;({struct _fat_ptr _tmp5EE=({const char*_tmpA2="`G%s%s";_tag_fat(_tmpA2,sizeof(char),7U);});Cyc_aprintf(_tmp5EE,_tag_fat(_tmpA1,sizeof(void*),2U));});}));}
# 440
return Cyc_PP_text(n);}
# 443
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp5=_tmpA6;void*_tmpA7=_stmttmp5;struct Cyc_Absyn_Kind*_tmpA8;struct Cyc_Absyn_Kind*_tmpA9;switch(*((int*)_tmpA7)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpA9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmpA9;
# 448
return({struct Cyc_PP_Doc*_tmpAA[3U];({struct Cyc_PP_Doc*_tmp5F1=Cyc_Absynpp_tvar2doc(tv);_tmpAA[0]=_tmp5F1;}),({struct Cyc_PP_Doc*_tmp5F0=Cyc_PP_text(({const char*_tmpAB="::";_tag_fat(_tmpAB,sizeof(char),3U);}));_tmpAA[1]=_tmp5F0;}),({struct Cyc_PP_Doc*_tmp5EF=Cyc_Absynpp_kind2doc(k);_tmpAA[2]=_tmp5EF;});Cyc_PP_cat(_tag_fat(_tmpAA,sizeof(struct Cyc_PP_Doc*),3U));});}}default: _LL5: _tmpA8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmpA8;
# 447
_tmpA9=k;goto _LL8;}}_LL0:;}
# 452
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _tmp5F4=({const char*_tmpAC="<";_tag_fat(_tmpAC,sizeof(char),2U);});struct _fat_ptr _tmp5F3=({const char*_tmpAD=">";_tag_fat(_tmpAD,sizeof(char),2U);});struct _fat_ptr _tmp5F2=({const char*_tmpAE=",";_tag_fat(_tmpAE,sizeof(char),2U);});Cyc_PP_egroup(_tmp5F4,_tmp5F3,_tmp5F2,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 456
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _tmp5F7=({const char*_tmpAF="<";_tag_fat(_tmpAF,sizeof(char),2U);});struct _fat_ptr _tmp5F6=({const char*_tmpB0=">";_tag_fat(_tmpB0,sizeof(char),2U);});struct _fat_ptr _tmp5F5=({const char*_tmpB1=",";_tag_fat(_tmpB1,sizeof(char),2U);});Cyc_PP_egroup(_tmp5F7,_tmp5F6,_tmp5F5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 462
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple17*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 466
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp5FA=({const char*_tmpB2="(";_tag_fat(_tmpB2,sizeof(char),2U);});struct _fat_ptr _tmp5F9=({const char*_tmpB3=")";_tag_fat(_tmpB3,sizeof(char),2U);});struct _fat_ptr _tmp5F8=({const char*_tmpB4=",";_tag_fat(_tmpB4,sizeof(char),2U);});Cyc_PP_group(_tmp5FA,_tmp5F9,_tmp5F8,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 470
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB5=att;switch(*((int*)_tmpB5)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();default: _LL7: _LL8:
 return Cyc_PP_text(Cyc_Absynpp_attribute2string(att));}_LL0:;}
# 479
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB6=(void*)atts->hd;void*_stmttmp6=_tmpB6;void*_tmpB7=_stmttmp6;switch(*((int*)_tmpB7)){case 1U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmpB8=" _stdcall ";_tag_fat(_tmpB8,sizeof(char),11U);}));case 2U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmpB9=" _cdecl ";_tag_fat(_tmpB9,sizeof(char),9U);}));case 3U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmpBA=" _fastcall ";_tag_fat(_tmpBA,sizeof(char),12U);}));default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 487
return Cyc_PP_nil_doc();}
# 490
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpBB=(void*)atts2->hd;void*_stmttmp7=_tmpBB;void*_tmpBC=_stmttmp7;switch(*((int*)_tmpBC)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 499
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBD[3U];({struct Cyc_PP_Doc*_tmp600=Cyc_PP_text(({const char*_tmpBE=" __declspec(";_tag_fat(_tmpBE,sizeof(char),13U);}));_tmpBD[0]=_tmp600;}),({
struct Cyc_PP_Doc*_tmp5FF=({struct _fat_ptr _tmp5FE=({const char*_tmpBF="";_tag_fat(_tmpBF,sizeof(char),1U);});struct _fat_ptr _tmp5FD=({const char*_tmpC0="";_tag_fat(_tmpC0,sizeof(char),1U);});struct _fat_ptr _tmp5FC=({const char*_tmpC1=" ";_tag_fat(_tmpC1,sizeof(char),2U);});Cyc_PP_group(_tmp5FE,_tmp5FD,_tmp5FC,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBD[1]=_tmp5FF;}),({
struct Cyc_PP_Doc*_tmp5FB=Cyc_PP_text(({const char*_tmpC2=")";_tag_fat(_tmpC2,sizeof(char),2U);}));_tmpBD[2]=_tmp5FB;});Cyc_PP_cat(_tag_fat(_tmpBD,sizeof(struct Cyc_PP_Doc*),3U));});}
# 506
struct _fat_ptr Cyc_Absynpp_attribute2string(void*a){
void*_tmpC3=a;struct _fat_ptr _tmpC4;struct _fat_ptr _tmpC5;int _tmpC6;int _tmpC7;int _tmpC8;int _tmpCA;int _tmpC9;int _tmpCC;int _tmpCB;struct _fat_ptr _tmpCD;struct Cyc_Absyn_Exp*_tmpCE;int _tmpCF;switch(*((int*)_tmpC3)){case 0U: _LL1: _tmpCF=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpC3)->f1;_LL2: {int i=_tmpCF;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD2=({struct Cyc_Int_pa_PrintArg_struct _tmp5BB;_tmp5BB.tag=1U,_tmp5BB.f1=(unsigned long)i;_tmp5BB;});void*_tmpD0[1U];_tmpD0[0]=& _tmpD2;({struct _fat_ptr _tmp601=({const char*_tmpD1="regparm(%d)";_tag_fat(_tmpD1,sizeof(char),12U);});Cyc_aprintf(_tmp601,_tag_fat(_tmpD0,sizeof(void*),1U));});});}case 1U: _LL3: _LL4:
 return({const char*_tmpD3="stdcall";_tag_fat(_tmpD3,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmpD4="cdecl";_tag_fat(_tmpD4,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmpD5="fastcall";_tag_fat(_tmpD5,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmpD6="noreturn";_tag_fat(_tmpD6,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmpD7="const";_tag_fat(_tmpD7,sizeof(char),6U);});case 6U: _LLD: _tmpCE=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpC3)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmpCE;
# 515
if(e == 0)return({const char*_tmpD8="aligned";_tag_fat(_tmpD8,sizeof(char),8U);});else{
# 517
enum Cyc_Flags_C_Compilers _tmpD9=Cyc_Flags_c_compiler;switch(_tmpD9){case Cyc_Flags_Gcc_c: _LL3A: _LL3B:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpDC=({struct Cyc_String_pa_PrintArg_struct _tmp5BC;_tmp5BC.tag=0U,({struct _fat_ptr _tmp602=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5BC.f1=_tmp602;});_tmp5BC;});void*_tmpDA[1U];_tmpDA[0]=& _tmpDC;({struct _fat_ptr _tmp603=({const char*_tmpDB="aligned(%s)";_tag_fat(_tmpDB,sizeof(char),12U);});Cyc_aprintf(_tmp603,_tag_fat(_tmpDA,sizeof(void*),1U));});});case Cyc_Flags_Vc_c: _LL3C: _LL3D:
 goto _LL3F;default: _LL3E: _LL3F:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpDF=({struct Cyc_String_pa_PrintArg_struct _tmp5BD;_tmp5BD.tag=0U,({struct _fat_ptr _tmp604=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5BD.f1=_tmp604;});_tmp5BD;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({struct _fat_ptr _tmp605=({const char*_tmpDE="align(%s)";_tag_fat(_tmpDE,sizeof(char),10U);});Cyc_aprintf(_tmp605,_tag_fat(_tmpDD,sizeof(void*),1U));});});}_LL39:;}}case 7U: _LLF: _LL10:
# 522
 return({const char*_tmpE0="packed";_tag_fat(_tmpE0,sizeof(char),7U);});case 8U: _LL11: _tmpCD=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpC3)->f1;_LL12: {struct _fat_ptr s=_tmpCD;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE3=({struct Cyc_String_pa_PrintArg_struct _tmp5BE;_tmp5BE.tag=0U,_tmp5BE.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5BE;});void*_tmpE1[1U];_tmpE1[0]=& _tmpE3;({struct _fat_ptr _tmp606=({const char*_tmpE2="section(\"%s\")";_tag_fat(_tmpE2,sizeof(char),14U);});Cyc_aprintf(_tmp606,_tag_fat(_tmpE1,sizeof(void*),1U));});});}case 9U: _LL13: _LL14:
 return({const char*_tmpE4="nocommon";_tag_fat(_tmpE4,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmpE5="shared";_tag_fat(_tmpE5,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmpE6="unused";_tag_fat(_tmpE6,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmpE7="weak";_tag_fat(_tmpE7,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmpE8="dllimport";_tag_fat(_tmpE8,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmpE9="dllexport";_tag_fat(_tmpE9,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmpEA="no_instrument_function";_tag_fat(_tmpEA,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmpEB="constructor";_tag_fat(_tmpEB,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmpEC="destructor";_tag_fat(_tmpEC,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmpED="no_check_memory_usage";_tag_fat(_tmpED,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC3)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmpCB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC3)->f2;_tmpCC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC3)->f3;_LL28: {int n=_tmpCB;int m=_tmpCC;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF0=({struct Cyc_Int_pa_PrintArg_struct _tmp5C0;_tmp5C0.tag=1U,_tmp5C0.f1=(unsigned)n;_tmp5C0;});struct Cyc_Int_pa_PrintArg_struct _tmpF1=({struct Cyc_Int_pa_PrintArg_struct _tmp5BF;_tmp5BF.tag=1U,_tmp5BF.f1=(unsigned)m;_tmp5BF;});void*_tmpEE[2U];_tmpEE[0]=& _tmpF0,_tmpEE[1]=& _tmpF1;({struct _fat_ptr _tmp607=({const char*_tmpEF="format(printf,%u,%u)";_tag_fat(_tmpEF,sizeof(char),21U);});Cyc_aprintf(_tmp607,_tag_fat(_tmpEE,sizeof(void*),2U));});});}}else{_LL29: _tmpC9=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC3)->f2;_tmpCA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC3)->f3;_LL2A: {int n=_tmpC9;int m=_tmpCA;
# 536
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF4=({struct Cyc_Int_pa_PrintArg_struct _tmp5C2;_tmp5C2.tag=1U,_tmp5C2.f1=(unsigned)n;_tmp5C2;});struct Cyc_Int_pa_PrintArg_struct _tmpF5=({struct Cyc_Int_pa_PrintArg_struct _tmp5C1;_tmp5C1.tag=1U,_tmp5C1.f1=(unsigned)m;_tmp5C1;});void*_tmpF2[2U];_tmpF2[0]=& _tmpF4,_tmpF2[1]=& _tmpF5;({struct _fat_ptr _tmp608=({const char*_tmpF3="format(scanf,%u,%u)";_tag_fat(_tmpF3,sizeof(char),20U);});Cyc_aprintf(_tmp608,_tag_fat(_tmpF2,sizeof(void*),2U));});});}}case 20U: _LL2B: _tmpC8=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpC3)->f1;_LL2C: {int n=_tmpC8;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF8=({struct Cyc_Int_pa_PrintArg_struct _tmp5C3;_tmp5C3.tag=1U,_tmp5C3.f1=(unsigned long)n;_tmp5C3;});void*_tmpF6[1U];_tmpF6[0]=& _tmpF8;({struct _fat_ptr _tmp609=({const char*_tmpF7="initializes(%d)";_tag_fat(_tmpF7,sizeof(char),16U);});Cyc_aprintf(_tmp609,_tag_fat(_tmpF6,sizeof(void*),1U));});});}case 21U: _LL2D: _tmpC7=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpC3)->f1;_LL2E: {int n=_tmpC7;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFB=({struct Cyc_Int_pa_PrintArg_struct _tmp5C4;_tmp5C4.tag=1U,_tmp5C4.f1=(unsigned long)n;_tmp5C4;});void*_tmpF9[1U];_tmpF9[0]=& _tmpFB;({struct _fat_ptr _tmp60A=({const char*_tmpFA="noliveunique(%d)";_tag_fat(_tmpFA,sizeof(char),17U);});Cyc_aprintf(_tmp60A,_tag_fat(_tmpF9,sizeof(void*),1U));});});}case 22U: _LL2F: _tmpC6=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpC3)->f1;_LL30: {int n=_tmpC6;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFE=({struct Cyc_Int_pa_PrintArg_struct _tmp5C5;_tmp5C5.tag=1U,_tmp5C5.f1=(unsigned long)n;_tmp5C5;});void*_tmpFC[1U];_tmpFC[0]=& _tmpFE;({struct _fat_ptr _tmp60B=({const char*_tmpFD="consume(%d)";_tag_fat(_tmpFD,sizeof(char),12U);});Cyc_aprintf(_tmp60B,_tag_fat(_tmpFC,sizeof(void*),1U));});});}case 23U: _LL31: _LL32:
 return({const char*_tmpFF="pure";_tag_fat(_tmpFF,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmp100="always_inline";_tag_fat(_tmp100,sizeof(char),14U);});case 24U: _LL35: _tmpC5=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpC3)->f1;_LL36: {struct _fat_ptr s=_tmpC5;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp103=({struct Cyc_String_pa_PrintArg_struct _tmp5C6;_tmp5C6.tag=0U,_tmp5C6.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5C6;});void*_tmp101[1U];_tmp101[0]=& _tmp103;({struct _fat_ptr _tmp60C=({const char*_tmp102="__mode__(\"%s\")";_tag_fat(_tmp102,sizeof(char),15U);});Cyc_aprintf(_tmp60C,_tag_fat(_tmp101,sizeof(void*),1U));});});}default: _LL37: _tmpC4=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpC3)->f1;_LL38: {struct _fat_ptr s=_tmpC4;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp106=({struct Cyc_String_pa_PrintArg_struct _tmp5C7;_tmp5C7.tag=0U,_tmp5C7.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5C7;});void*_tmp104[1U];_tmp104[0]=& _tmp106;({struct _fat_ptr _tmp60D=({const char*_tmp105="alias(\"%s\")";_tag_fat(_tmp105,sizeof(char),12U);});Cyc_aprintf(_tmp60D,_tag_fat(_tmp104,sizeof(void*),1U));});});}}_LL0:;}
# 547
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absynpp_attribute2string(a));}
# 551
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Flags_C_Compilers _tmp107=Cyc_Flags_c_compiler;if(_tmp107 == Cyc_Flags_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 556
 return({struct Cyc_PP_Doc*_tmp108[2U];({struct Cyc_PP_Doc*_tmp612=Cyc_PP_text(({const char*_tmp109=" __attribute__";_tag_fat(_tmp109,sizeof(char),15U);}));_tmp108[0]=_tmp612;}),({
struct Cyc_PP_Doc*_tmp611=({struct _fat_ptr _tmp610=({const char*_tmp10A="((";_tag_fat(_tmp10A,sizeof(char),3U);});struct _fat_ptr _tmp60F=({const char*_tmp10B="))";_tag_fat(_tmp10B,sizeof(char),3U);});struct _fat_ptr _tmp60E=({const char*_tmp10C=",";_tag_fat(_tmp10C,sizeof(char),2U);});Cyc_PP_group(_tmp610,_tmp60F,_tmp60E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmp108[1]=_tmp611;});Cyc_PP_cat(_tag_fat(_tmp108,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}}
# 561
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmp10D=(void*)tms->hd;void*_stmttmp8=_tmp10D;void*_tmp10E=_stmttmp8;switch(*((int*)_tmp10E)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 566
enum Cyc_Flags_C_Compilers _tmp10F=Cyc_Flags_c_compiler;if(_tmp10F == Cyc_Flags_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 570
 return 0;}_LL0:;}}
# 574
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 577
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question (void){
if(!((unsigned)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmp110="{?}";_tag_fat(_tmp110,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmp111="?";_tag_fat(_tmp111,sizeof(char),2U);}));}}
# 584
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 586
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb (void){
if(!((unsigned)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmp112="{\\lb}";_tag_fat(_tmp112,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmp113="{";_tag_fat(_tmp113,sizeof(char),2U);}));}}
# 593
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 595
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb (void){
if(!((unsigned)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmp114="{\\rb}";_tag_fat(_tmp114,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmp115="}";_tag_fat(_tmp115,sizeof(char),2U);}));}}
# 602
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 604
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar (void){
if(!((unsigned)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmp116="\\$";_tag_fat(_tmp116,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmp117="$";_tag_fat(_tmp117,sizeof(char),2U);}));}}
# 611
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 613
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmp118[3U];({struct Cyc_PP_Doc*_tmp615=Cyc_Absynpp_lb();_tmp118[0]=_tmp615;}),({struct Cyc_PP_Doc*_tmp614=Cyc_PP_seq(sep,ss);_tmp118[1]=_tmp614;}),({struct Cyc_PP_Doc*_tmp613=Cyc_Absynpp_rb();_tmp118[2]=_tmp613;});Cyc_PP_cat(_tag_fat(_tmp118,sizeof(struct Cyc_PP_Doc*),3U));});}
# 618
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmp119=(void*)tms->hd;void*_stmttmp9=_tmp119;void*_tmp11A=_stmttmp9;struct Cyc_List_List*_tmp11B;switch(*((int*)_tmp11A)){case 0U: _LL1: _LL2:
({void*_tmp11C=0U;({struct Cyc___cycFILE*_tmp617=Cyc_stderr;struct _fat_ptr _tmp616=({const char*_tmp11D="Carray_mod ";_tag_fat(_tmp11D,sizeof(char),12U);});Cyc_fprintf(_tmp617,_tmp616,_tag_fat(_tmp11C,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp11E=0U;({struct Cyc___cycFILE*_tmp619=Cyc_stderr;struct _fat_ptr _tmp618=({const char*_tmp11F="ConstArray_mod ";_tag_fat(_tmp11F,sizeof(char),16U);});Cyc_fprintf(_tmp619,_tmp618,_tag_fat(_tmp11E,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp11A)->f1)->tag == 1U){_LL5: _tmp11B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp11A)->f1)->f1;_LL6: {struct Cyc_List_List*ys=_tmp11B;
# 624
({void*_tmp120=0U;({struct Cyc___cycFILE*_tmp61B=Cyc_stderr;struct _fat_ptr _tmp61A=({const char*_tmp121="Function_mod(";_tag_fat(_tmp121,sizeof(char),14U);});Cyc_fprintf(_tmp61B,_tmp61A,_tag_fat(_tmp120,sizeof(void*),0U));});});
for(0;ys != 0;ys=ys->tl){
struct _fat_ptr*_tmp122=(*((struct _tuple8*)ys->hd)).f1;struct _fat_ptr*v=_tmp122;
if(v == 0)({void*_tmp123=0U;({struct Cyc___cycFILE*_tmp61D=Cyc_stderr;struct _fat_ptr _tmp61C=({const char*_tmp124="?";_tag_fat(_tmp124,sizeof(char),2U);});Cyc_fprintf(_tmp61D,_tmp61C,_tag_fat(_tmp123,sizeof(void*),0U));});});else{
({void*_tmp125=0U;({struct Cyc___cycFILE*_tmp61F=Cyc_stderr;struct _fat_ptr _tmp61E=*v;Cyc_fprintf(_tmp61F,_tmp61E,_tag_fat(_tmp125,sizeof(void*),0U));});});}
if(ys->tl != 0)({void*_tmp126=0U;({struct Cyc___cycFILE*_tmp621=Cyc_stderr;struct _fat_ptr _tmp620=({const char*_tmp127=",";_tag_fat(_tmp127,sizeof(char),2U);});Cyc_fprintf(_tmp621,_tmp620,_tag_fat(_tmp126,sizeof(void*),0U));});});}
# 631
({void*_tmp128=0U;({struct Cyc___cycFILE*_tmp623=Cyc_stderr;struct _fat_ptr _tmp622=({const char*_tmp129=") ";_tag_fat(_tmp129,sizeof(char),3U);});Cyc_fprintf(_tmp623,_tmp622,_tag_fat(_tmp128,sizeof(void*),0U));});});
goto _LL0;}}else{_LL7: _LL8:
# 634
({void*_tmp12A=0U;({struct Cyc___cycFILE*_tmp625=Cyc_stderr;struct _fat_ptr _tmp624=({const char*_tmp12B="Function_mod()";_tag_fat(_tmp12B,sizeof(char),15U);});Cyc_fprintf(_tmp625,_tmp624,_tag_fat(_tmp12A,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmp12C=0U;({struct Cyc___cycFILE*_tmp627=Cyc_stderr;struct _fat_ptr _tmp626=({const char*_tmp12D="Attributes_mod ";_tag_fat(_tmp12D,sizeof(char),16U);});Cyc_fprintf(_tmp627,_tmp626,_tag_fat(_tmp12C,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmp12E=0U;({struct Cyc___cycFILE*_tmp629=Cyc_stderr;struct _fat_ptr _tmp628=({const char*_tmp12F="TypeParams_mod ";_tag_fat(_tmp12F,sizeof(char),16U);});Cyc_fprintf(_tmp629,_tmp628,_tag_fat(_tmp12E,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp130=0U;({struct Cyc___cycFILE*_tmp62B=Cyc_stderr;struct _fat_ptr _tmp62A=({const char*_tmp131="Pointer_mod ";_tag_fat(_tmp131,sizeof(char),13U);});Cyc_fprintf(_tmp62B,_tmp62A,_tag_fat(_tmp130,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 639
({void*_tmp132=0U;({struct Cyc___cycFILE*_tmp62D=Cyc_stderr;struct _fat_ptr _tmp62C=({const char*_tmp133="\n";_tag_fat(_tmp133,sizeof(char),2U);});Cyc_fprintf(_tmp62D,_tmp62C,_tag_fat(_tmp132,sizeof(void*),0U));});});}
# 642
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 644
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp178[3U];({struct Cyc_PP_Doc*_tmp62F=Cyc_PP_text(({const char*_tmp179="(";_tag_fat(_tmp179,sizeof(char),2U);}));_tmp178[0]=_tmp62F;}),_tmp178[1]=rest,({struct Cyc_PP_Doc*_tmp62E=Cyc_PP_text(({const char*_tmp17A=")";_tag_fat(_tmp17A,sizeof(char),2U);}));_tmp178[2]=_tmp62E;});Cyc_PP_cat(_tag_fat(_tmp178,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmp134=(void*)tms->hd;void*_stmttmpA=_tmp134;void*_tmp135=_stmttmpA;struct Cyc_Absyn_Tqual _tmp13A;void*_tmp139;void*_tmp138;void*_tmp137;void*_tmp136;int _tmp13D;unsigned _tmp13C;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13E;void*_tmp13F;void*_tmp141;struct Cyc_Absyn_Exp*_tmp140;void*_tmp142;switch(*((int*)_tmp135)){case 0U: _LL1: _tmp142=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp135)->f1;_LL2: {void*zeroterm=_tmp142;
# 650
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp143[2U];_tmp143[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_tmp631=Cyc_PP_text(({const char*_tmp144="[]@zeroterm";_tag_fat(_tmp144,sizeof(char),12U);}));_tmp143[1]=_tmp631;}):({struct Cyc_PP_Doc*_tmp630=Cyc_PP_text(({const char*_tmp145="[]";_tag_fat(_tmp145,sizeof(char),3U);}));_tmp143[1]=_tmp630;});Cyc_PP_cat(_tag_fat(_tmp143,sizeof(struct Cyc_PP_Doc*),2U));});}case 1U: _LL3: _tmp140=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp135)->f1;_tmp141=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp135)->f2;_LL4: {struct Cyc_Absyn_Exp*e=_tmp140;void*zeroterm=_tmp141;
# 654
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp146[4U];_tmp146[0]=rest,({struct Cyc_PP_Doc*_tmp635=Cyc_PP_text(({const char*_tmp147="[";_tag_fat(_tmp147,sizeof(char),2U);}));_tmp146[1]=_tmp635;}),({struct Cyc_PP_Doc*_tmp634=Cyc_Absynpp_exp2doc(e);_tmp146[2]=_tmp634;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_tmp633=Cyc_PP_text(({const char*_tmp148="]@zeroterm";_tag_fat(_tmp148,sizeof(char),11U);}));_tmp146[3]=_tmp633;}):({struct Cyc_PP_Doc*_tmp632=Cyc_PP_text(({const char*_tmp149="]";_tag_fat(_tmp149,sizeof(char),2U);}));_tmp146[3]=_tmp632;});Cyc_PP_cat(_tag_fat(_tmp146,sizeof(struct Cyc_PP_Doc*),4U));});}case 3U: _LL5: _tmp13F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp135)->f1;_LL6: {void*args=_tmp13F;
# 658
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp14A=args;unsigned _tmp14C;struct Cyc_List_List*_tmp14B;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_List_List*_tmp151;void*_tmp150;struct Cyc_Absyn_VarargInfo*_tmp14F;int _tmp14E;struct Cyc_List_List*_tmp14D;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14A)->tag == 1U){_LLE: _tmp14D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14A)->f1;_tmp14E=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14A)->f2;_tmp14F=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14A)->f3;_tmp150=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14A)->f4;_tmp151=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14A)->f5;_tmp152=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14A)->f6;_tmp153=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14A)->f7;_LLF: {struct Cyc_List_List*args2=_tmp14D;int c_varargs=_tmp14E;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp14F;void*effopt=_tmp150;struct Cyc_List_List*rgn_po=_tmp151;struct Cyc_Absyn_Exp*req=_tmp152;struct Cyc_Absyn_Exp*ens=_tmp153;
# 661
return({struct Cyc_PP_Doc*_tmp154[2U];_tmp154[0]=rest,({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);_tmp154[1]=_tmp636;});Cyc_PP_cat(_tag_fat(_tmp154,sizeof(struct Cyc_PP_Doc*),2U));});}}else{_LL10: _tmp14B=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp14A)->f1;_tmp14C=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp14A)->f2;_LL11: {struct Cyc_List_List*sl=_tmp14B;unsigned loc=_tmp14C;
# 663
return({struct Cyc_PP_Doc*_tmp155[2U];_tmp155[0]=rest,({struct Cyc_PP_Doc*_tmp63A=({struct _fat_ptr _tmp639=({const char*_tmp156="(";_tag_fat(_tmp156,sizeof(char),2U);});struct _fat_ptr _tmp638=({const char*_tmp157=")";_tag_fat(_tmp157,sizeof(char),2U);});struct _fat_ptr _tmp637=({const char*_tmp158=",";_tag_fat(_tmp158,sizeof(char),2U);});Cyc_PP_group(_tmp639,_tmp638,_tmp637,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,sl));});_tmp155[1]=_tmp63A;});Cyc_PP_cat(_tag_fat(_tmp155,sizeof(struct Cyc_PP_Doc*),2U));});}}_LLD:;}}case 5U: _LL7: _tmp13E=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp135)->f2;_LL8: {struct Cyc_List_List*atts=_tmp13E;
# 666
enum Cyc_Flags_C_Compilers _tmp159=Cyc_Flags_c_compiler;if(_tmp159 == Cyc_Flags_Gcc_c){_LL13: _LL14:
# 668
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp15A[2U];_tmp15A[0]=rest,({struct Cyc_PP_Doc*_tmp63B=Cyc_Absynpp_atts2doc(atts);_tmp15A[1]=_tmp63B;});Cyc_PP_cat(_tag_fat(_tmp15A,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 672
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp15B[2U];({struct Cyc_PP_Doc*_tmp63C=Cyc_Absynpp_callconv2doc(atts);_tmp15B[0]=_tmp63C;}),_tmp15B[1]=rest;Cyc_PP_cat(_tag_fat(_tmp15B,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp13B=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp135)->f1;_tmp13C=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp135)->f2;_tmp13D=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp135)->f3;_LLA: {struct Cyc_List_List*ts=_tmp13B;unsigned loc=_tmp13C;int print_kinds=_tmp13D;
# 677
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_tmp15C[2U];_tmp15C[0]=rest,({struct Cyc_PP_Doc*_tmp63D=Cyc_Absynpp_ktvars2doc(ts);_tmp15C[1]=_tmp63D;});Cyc_PP_cat(_tag_fat(_tmp15C,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 681
return({struct Cyc_PP_Doc*_tmp15D[2U];_tmp15D[0]=rest,({struct Cyc_PP_Doc*_tmp63E=Cyc_Absynpp_tvars2doc(ts);_tmp15D[1]=_tmp63E;});Cyc_PP_cat(_tag_fat(_tmp15D,sizeof(struct Cyc_PP_Doc*),2U));});}}default: _LLB: _tmp136=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp135)->f1).rgn;_tmp137=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp135)->f1).nullable;_tmp138=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp135)->f1).bounds;_tmp139=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp135)->f1).zero_term;_tmp13A=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp135)->f2;_LLC: {void*rgn=_tmp136;void*nullable=_tmp137;void*bd=_tmp138;void*zt=_tmp139;struct Cyc_Absyn_Tqual tq2=_tmp13A;
# 685
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_tmp15E=Cyc_Absyn_compress(bd);void*_stmttmpB=_tmp15E;void*_tmp15F=_stmttmpB;void*_tmp160;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15F)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15F)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15F)->f2 != 0){_LL1A: _tmp160=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15F)->f2)->hd;_LL1B: {void*targ=_tmp160;
# 693
{void*_tmp161=Cyc_Absyn_compress(targ);void*_stmttmpC=_tmp161;void*_tmp162=_stmttmpC;struct Cyc_Absyn_Exp*_tmp163;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp162)->tag == 9U){_LL1F: _tmp163=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp162)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp163;
# 695
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp164="*";_tag_fat(_tmp164,sizeof(char),2U);}):({const char*_tmp165="@";_tag_fat(_tmp165,sizeof(char),2U);}));{
struct _tuple12 _tmp166=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmpD=_tmp166;struct _tuple12 _tmp167=_stmttmpD;int _tmp169;unsigned _tmp168;_LL24: _tmp168=_tmp167.f1;_tmp169=_tmp167.f2;_LL25: {unsigned val=_tmp168;int known=_tmp169;
if(!known || val != (unsigned)1)
ptr=({struct Cyc_PP_Doc*_tmp16A[4U];_tmp16A[0]=ptr,({struct Cyc_PP_Doc*_tmp641=Cyc_Absynpp_lb();_tmp16A[1]=_tmp641;}),({struct Cyc_PP_Doc*_tmp640=Cyc_Absynpp_exp2doc(e);_tmp16A[2]=_tmp640;}),({struct Cyc_PP_Doc*_tmp63F=Cyc_Absynpp_rb();_tmp16A[3]=_tmp63F;});Cyc_PP_cat(_tag_fat(_tmp16A,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}}}}else{_LL21: _LL22:
# 701
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp16B="*";_tag_fat(_tmp16B,sizeof(char),2U);}):({const char*_tmp16C="@";_tag_fat(_tmp16C,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp16D[4U];_tmp16D[0]=ptr,({struct Cyc_PP_Doc*_tmp644=Cyc_Absynpp_lb();_tmp16D[1]=_tmp644;}),({struct Cyc_PP_Doc*_tmp643=Cyc_Absynpp_typ2doc(targ);_tmp16D[2]=_tmp643;}),({struct Cyc_PP_Doc*_tmp642=Cyc_Absynpp_rb();_tmp16D[3]=_tmp642;});Cyc_PP_cat(_tag_fat(_tmp16D,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}_LL1E:;}
# 705
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 707
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp16E="*";_tag_fat(_tmp16E,sizeof(char),2U);}):({const char*_tmp16F="@";_tag_fat(_tmp16F,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp170[4U];_tmp170[0]=ptr,({struct Cyc_PP_Doc*_tmp647=Cyc_Absynpp_lb();_tmp170[1]=_tmp647;}),({struct Cyc_PP_Doc*_tmp646=Cyc_Absynpp_typ2doc(bd);_tmp170[2]=_tmp646;}),({struct Cyc_PP_Doc*_tmp645=Cyc_Absynpp_rb();_tmp170[3]=_tmp645;});Cyc_PP_cat(_tag_fat(_tmp170,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 712
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr && Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp171="@zeroterm";_tag_fat(_tmp171,sizeof(char),10U);}));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp172="@nozeroterm";_tag_fat(_tmp172,sizeof(char),12U);}));}}
# 718
{void*_tmp173=Cyc_Absyn_compress(rgn);void*_stmttmpE=_tmp173;void*_tmp174=_stmttmpE;switch(*((int*)_tmp174)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f1)->tag == 5U){_LL27: _LL28:
 goto _LL26;}else{goto _LL2B;}case 1U: _LL29: if(Cyc_Absynpp_print_for_cycdoc){_LL2A:
 goto _LL26;}else{goto _LL2B;}default: _LL2B: _LL2C:
 rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL26:;}{
# 723
struct Cyc_PP_Doc*spacer1=tqd != mt &&(ztd != mt || rgd != mt)?Cyc_PP_text(({const char*_tmp177=" ";_tag_fat(_tmp177,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp176=" ";_tag_fat(_tmp176,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp175[7U];_tmp175[0]=ptr,_tmp175[1]=ztd,_tmp175[2]=rgd,_tmp175[3]=spacer1,_tmp175[4]=tqd,_tmp175[5]=spacer2,_tmp175[6]=rest;Cyc_PP_cat(_tag_fat(_tmp175,sizeof(struct Cyc_PP_Doc*),7U));});}}}_LL0:;}}
# 729
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp17B=Cyc_Absyn_compress(t);void*_stmttmpF=_tmp17B;void*_tmp17C=_stmttmpF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17C)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp17D="`H";_tag_fat(_tmp17D,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp17E="`U";_tag_fat(_tmp17E,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp17F="`RC";_tag_fat(_tmp17F,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 739
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 743
void*_tmp180=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp180;void*_tmp181=_stmttmp10;struct Cyc_List_List*_tmp182;void*_tmp183;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2 != 0){_LL1: _tmp183=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2)->hd;_LL2: {void*r=_tmp183;
({struct Cyc_List_List*_tmp649=({struct Cyc_List_List*_tmp184=_cycalloc(sizeof(*_tmp184));({struct Cyc_PP_Doc*_tmp648=Cyc_Absynpp_rgn2doc(r);_tmp184->hd=_tmp648;}),_tmp184->tl=*rgions;_tmp184;});*rgions=_tmp649;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp182=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2;_LL4: {struct Cyc_List_List*ts=_tmp182;
# 746
for(0;ts != 0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
# 749
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp64B=({struct Cyc_List_List*_tmp185=_cycalloc(sizeof(*_tmp185));({struct Cyc_PP_Doc*_tmp64A=Cyc_Absynpp_typ2doc(t);_tmp185->hd=_tmp64A;}),_tmp185->tl=*effects;_tmp185;});*effects=_tmp64B;});goto _LL0;}_LL0:;}
# 754
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0 && effects != 0)
return({struct _fat_ptr _tmp64E=({const char*_tmp186="";_tag_fat(_tmp186,sizeof(char),1U);});struct _fat_ptr _tmp64D=({const char*_tmp187="";_tag_fat(_tmp187,sizeof(char),1U);});struct _fat_ptr _tmp64C=({const char*_tmp188="+";_tag_fat(_tmp188,sizeof(char),2U);});Cyc_PP_group(_tmp64E,_tmp64D,_tmp64C,effects);});else{
# 762
struct Cyc_PP_Doc*_tmp189=({struct _fat_ptr _tmp64F=({const char*_tmp18E=",";_tag_fat(_tmp18E,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp64F,rgions);});struct Cyc_PP_Doc*doc1=_tmp189;
return({struct _fat_ptr _tmp653=({const char*_tmp18A="";_tag_fat(_tmp18A,sizeof(char),1U);});struct _fat_ptr _tmp652=({const char*_tmp18B="";_tag_fat(_tmp18B,sizeof(char),1U);});struct _fat_ptr _tmp651=({const char*_tmp18C="+";_tag_fat(_tmp18C,sizeof(char),2U);});Cyc_PP_group(_tmp653,_tmp652,_tmp651,({struct Cyc_List_List*_tmp650=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp650,({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=doc1,_tmp18D->tl=0;_tmp18D;}));}));});}}
# 767
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp18F=k;if(_tmp18F == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp190="struct ";_tag_fat(_tmp190,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp191="union ";_tag_fat(_tmp191,sizeof(char),7U);}));}_LL0:;}
# 775
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp192=t;struct Cyc_Absyn_Datatypedecl*_tmp193;struct Cyc_Absyn_Enumdecl*_tmp194;struct Cyc_Absyn_Aggrdecl*_tmp195;struct Cyc_Absyn_Typedefdecl*_tmp198;struct Cyc_List_List*_tmp197;struct _tuple0*_tmp196;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_List_List*_tmp19C;enum Cyc_Absyn_AggrKind _tmp19B;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Tvar*_tmp19E;struct Cyc_Core_Opt*_tmp1A2;int _tmp1A1;void*_tmp1A0;struct Cyc_Core_Opt*_tmp19F;void*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A6;struct _fat_ptr _tmp1A7;struct _tuple0*_tmp1A8;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AB;union Cyc_Absyn_AggrInfo _tmp1AA;int _tmp1AC;enum Cyc_Absyn_Size_of _tmp1AE;enum Cyc_Absyn_Sign _tmp1AD;struct Cyc_List_List*_tmp1B0;union Cyc_Absyn_DatatypeFieldInfo _tmp1AF;struct Cyc_List_List*_tmp1B2;union Cyc_Absyn_DatatypeInfo _tmp1B1;switch(*((int*)_tmp192)){case 4U: _LL1: _LL2:
# 779
 return Cyc_PP_text(({const char*_tmp1B3="[[[array]]]";_tag_fat(_tmp1B3,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)){case 0U: _LL7: _LL8:
# 783
 s=Cyc_PP_text(({const char*_tmp1B4="void";_tag_fat(_tmp1B4,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp1B1=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f1;_tmp1B2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LLE: {union Cyc_Absyn_DatatypeInfo tu_info=_tmp1B1;struct Cyc_List_List*ts=_tmp1B2;
# 799
{union Cyc_Absyn_DatatypeInfo _tmp1BC=tu_info;int _tmp1BE;struct _tuple0*_tmp1BD;int _tmp1C0;struct _tuple0*_tmp1BF;if((_tmp1BC.UnknownDatatype).tag == 1){_LL48: _tmp1BF=((_tmp1BC.UnknownDatatype).val).name;_tmp1C0=((_tmp1BC.UnknownDatatype).val).is_extensible;_LL49: {struct _tuple0*n=_tmp1BF;int is_x=_tmp1C0;
_tmp1BD=n;_tmp1BE=is_x;goto _LL4B;}}else{_LL4A: _tmp1BD=(*(_tmp1BC.KnownDatatype).val)->name;_tmp1BE=(*(_tmp1BC.KnownDatatype).val)->is_extensible;_LL4B: {struct _tuple0*n=_tmp1BD;int is_x=_tmp1BE;
# 802
struct Cyc_PP_Doc*_tmp1C1=Cyc_PP_text(({const char*_tmp1C5="datatype ";_tag_fat(_tmp1C5,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp1C1;
struct Cyc_PP_Doc*_tmp1C2=is_x?Cyc_PP_text(({const char*_tmp1C4="@extensible ";_tag_fat(_tmp1C4,sizeof(char),13U);})): Cyc_PP_nil_doc();struct Cyc_PP_Doc*ext=_tmp1C2;
s=({struct Cyc_PP_Doc*_tmp1C3[4U];_tmp1C3[0]=ext,_tmp1C3[1]=kw,({struct Cyc_PP_Doc*_tmp655=Cyc_Absynpp_qvar2doc(n);_tmp1C3[2]=_tmp655;}),({struct Cyc_PP_Doc*_tmp654=Cyc_Absynpp_tps2doc(ts);_tmp1C3[3]=_tmp654;});Cyc_PP_cat(_tag_fat(_tmp1C3,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 807
goto _LL0;}case 19U: _LLF: _tmp1AF=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f1;_tmp1B0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL10: {union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp1AF;struct Cyc_List_List*ts=_tmp1B0;
# 809
{union Cyc_Absyn_DatatypeFieldInfo _tmp1C6=tuf_info;struct _tuple0*_tmp1C9;int _tmp1C8;struct _tuple0*_tmp1C7;int _tmp1CC;struct _tuple0*_tmp1CB;struct _tuple0*_tmp1CA;if((_tmp1C6.UnknownDatatypefield).tag == 1){_LL4D: _tmp1CA=((_tmp1C6.UnknownDatatypefield).val).datatype_name;_tmp1CB=((_tmp1C6.UnknownDatatypefield).val).field_name;_tmp1CC=((_tmp1C6.UnknownDatatypefield).val).is_extensible;_LL4E: {struct _tuple0*tname=_tmp1CA;struct _tuple0*fname=_tmp1CB;int is_x=_tmp1CC;
# 811
_tmp1C7=tname;_tmp1C8=is_x;_tmp1C9=fname;goto _LL50;}}else{_LL4F: _tmp1C7=(((_tmp1C6.KnownDatatypefield).val).f1)->name;_tmp1C8=(((_tmp1C6.KnownDatatypefield).val).f1)->is_extensible;_tmp1C9=(((_tmp1C6.KnownDatatypefield).val).f2)->name;_LL50: {struct _tuple0*tname=_tmp1C7;int is_x=_tmp1C8;struct _tuple0*fname=_tmp1C9;
# 814
struct Cyc_PP_Doc*_tmp1CD=Cyc_PP_text(is_x?({const char*_tmp1D0="@extensible datatype ";_tag_fat(_tmp1D0,sizeof(char),22U);}):({const char*_tmp1D1="datatype ";_tag_fat(_tmp1D1,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp1CD;
s=({struct Cyc_PP_Doc*_tmp1CE[4U];_tmp1CE[0]=kw,({struct Cyc_PP_Doc*_tmp658=Cyc_Absynpp_qvar2doc(tname);_tmp1CE[1]=_tmp658;}),({struct Cyc_PP_Doc*_tmp657=Cyc_PP_text(({const char*_tmp1CF=".";_tag_fat(_tmp1CF,sizeof(char),2U);}));_tmp1CE[2]=_tmp657;}),({struct Cyc_PP_Doc*_tmp656=Cyc_Absynpp_qvar2doc(fname);_tmp1CE[3]=_tmp656;});Cyc_PP_cat(_tag_fat(_tmp1CE,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 818
goto _LL0;}case 1U: _LL11: _tmp1AD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f1;_tmp1AE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f2;_LL12: {enum Cyc_Absyn_Sign sn=_tmp1AD;enum Cyc_Absyn_Size_of sz=_tmp1AE;
# 820
struct _fat_ptr sns;
struct _fat_ptr ts;
{enum Cyc_Absyn_Sign _tmp1D2=sn;switch(_tmp1D2){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp1D3="";_tag_fat(_tmp1D3,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp1D4="unsigned ";_tag_fat(_tmp1D4,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 827
{enum Cyc_Absyn_Size_of _tmp1D5=sz;switch(_tmp1D5){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 829
{enum Cyc_Absyn_Sign _tmp1D6=sn;switch(_tmp1D6){case Cyc_Absyn_None: _LL66: _LL67:
 sns=({const char*_tmp1D7="";_tag_fat(_tmp1D7,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed: _LL68: _LL69:
 sns=({const char*_tmp1D8="signed ";_tag_fat(_tmp1D8,sizeof(char),8U);});goto _LL65;default: _LL6A: _LL6B:
 sns=({const char*_tmp1D9="unsigned ";_tag_fat(_tmp1D9,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 834
ts=({const char*_tmp1DA="char";_tag_fat(_tmp1DA,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp1DB="short";_tag_fat(_tmp1DB,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp1DC="int";_tag_fat(_tmp1DC,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp1DD="long";_tag_fat(_tmp1DD,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
# 841
{enum Cyc_Flags_C_Compilers _tmp1DE=Cyc_Flags_c_compiler;if(_tmp1DE == Cyc_Flags_Gcc_c){_LL6D: _LL6E:
 ts=({const char*_tmp1DF="long long";_tag_fat(_tmp1DF,sizeof(char),10U);});goto _LL6C;}else{_LL6F: _LL70:
 ts=({const char*_tmp1E0="__int64";_tag_fat(_tmp1E0,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 845
goto _LL58;}_LL58:;}
# 847
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1E3=({struct Cyc_String_pa_PrintArg_struct _tmp5C9;_tmp5C9.tag=0U,_tmp5C9.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_tmp5C9;});struct Cyc_String_pa_PrintArg_struct _tmp1E4=({struct Cyc_String_pa_PrintArg_struct _tmp5C8;_tmp5C8.tag=0U,_tmp5C8.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_tmp5C8;});void*_tmp1E1[2U];_tmp1E1[0]=& _tmp1E3,_tmp1E1[1]=& _tmp1E4;({struct _fat_ptr _tmp659=({const char*_tmp1E2="%s%s";_tag_fat(_tmp1E2,sizeof(char),5U);});Cyc_aprintf(_tmp659,_tag_fat(_tmp1E1,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp1AC=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f1;_LL14: {int i=_tmp1AC;
# 850
{int _tmp1E5=i;switch(_tmp1E5){case 0U: _LL72: _LL73:
 s=Cyc_PP_text(({const char*_tmp1E6="float";_tag_fat(_tmp1E6,sizeof(char),6U);}));goto _LL71;case 1U: _LL74: _LL75:
 s=Cyc_PP_text(({const char*_tmp1E7="double";_tag_fat(_tmp1E7,sizeof(char),7U);}));goto _LL71;default: _LL76: _LL77:
 s=Cyc_PP_text(({const char*_tmp1E8="long double";_tag_fat(_tmp1E8,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 855
goto _LL0;}case 20U: _LL17: _tmp1AA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f1;_tmp1AB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL18: {union Cyc_Absyn_AggrInfo info=_tmp1AA;struct Cyc_List_List*ts=_tmp1AB;
# 858
struct _tuple11 _tmp1EA=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp11=_tmp1EA;struct _tuple11 _tmp1EB=_stmttmp11;struct _tuple0*_tmp1ED;enum Cyc_Absyn_AggrKind _tmp1EC;_LL79: _tmp1EC=_tmp1EB.f1;_tmp1ED=_tmp1EB.f2;_LL7A: {enum Cyc_Absyn_AggrKind k=_tmp1EC;struct _tuple0*n=_tmp1ED;
s=({struct Cyc_PP_Doc*_tmp1EE[3U];({struct Cyc_PP_Doc*_tmp65C=Cyc_Absynpp_aggr_kind2doc(k);_tmp1EE[0]=_tmp65C;}),({struct Cyc_PP_Doc*_tmp65B=Cyc_Absynpp_qvar2doc(n);_tmp1EE[1]=_tmp65B;}),({struct Cyc_PP_Doc*_tmp65A=Cyc_Absynpp_tps2doc(ts);_tmp1EE[2]=_tmp65A;});Cyc_PP_cat(_tag_fat(_tmp1EE,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 16U: _LL1B: _tmp1A9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f1;_LL1C: {struct Cyc_List_List*fs=_tmp1A9;
# 867
s=({struct Cyc_PP_Doc*_tmp1F0[4U];({struct Cyc_PP_Doc*_tmp660=Cyc_PP_text(({const char*_tmp1F1="enum ";_tag_fat(_tmp1F1,sizeof(char),6U);}));_tmp1F0[0]=_tmp660;}),({struct Cyc_PP_Doc*_tmp65F=Cyc_Absynpp_lb();_tmp1F0[1]=_tmp65F;}),({struct Cyc_PP_Doc*_tmp65E=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_tmp1F0[2]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_rb();_tmp1F0[3]=_tmp65D;});Cyc_PP_cat(_tag_fat(_tmp1F0,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1D: _tmp1A8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f1;_LL1E: {struct _tuple0*n=_tmp1A8;
s=({struct Cyc_PP_Doc*_tmp1F2[2U];({struct Cyc_PP_Doc*_tmp662=Cyc_PP_text(({const char*_tmp1F3="enum ";_tag_fat(_tmp1F3,sizeof(char),6U);}));_tmp1F2[0]=_tmp662;}),({struct Cyc_PP_Doc*_tmp661=Cyc_Absynpp_qvar2doc(n);_tmp1F2[1]=_tmp661;});Cyc_PP_cat(_tag_fat(_tmp1F2,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 17U: _LL23: _tmp1A7=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)->f1;_LL24: {struct _fat_ptr t=_tmp1A7;
# 871
s=Cyc_PP_text(t);goto _LL0;}case 3U: _LL27: _tmp1A6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL28: {struct Cyc_List_List*ts=_tmp1A6;
# 881
s=({struct Cyc_PP_Doc*_tmp1FB[3U];({struct Cyc_PP_Doc*_tmp665=Cyc_PP_text(({const char*_tmp1FC="region_t<";_tag_fat(_tmp1FC,sizeof(char),10U);}));_tmp1FB[0]=_tmp665;}),({struct Cyc_PP_Doc*_tmp664=Cyc_Absynpp_rgn2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1FB[1]=_tmp664;}),({struct Cyc_PP_Doc*_tmp663=Cyc_PP_text(({const char*_tmp1FD=">";_tag_fat(_tmp1FD,sizeof(char),2U);}));_tmp1FB[2]=_tmp663;});Cyc_PP_cat(_tag_fat(_tmp1FB,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 4U: _LL29: _tmp1A5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL2A: {struct Cyc_List_List*ts=_tmp1A5;
# 883
s=({struct Cyc_PP_Doc*_tmp1FE[3U];({struct Cyc_PP_Doc*_tmp668=Cyc_PP_text(({const char*_tmp1FF="tag_t<";_tag_fat(_tmp1FF,sizeof(char),7U);}));_tmp1FE[0]=_tmp668;}),({struct Cyc_PP_Doc*_tmp667=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1FE[1]=_tmp667;}),({struct Cyc_PP_Doc*_tmp666=Cyc_PP_text(({const char*_tmp200=">";_tag_fat(_tmp200,sizeof(char),2U);}));_tmp1FE[2]=_tmp666;});Cyc_PP_cat(_tag_fat(_tmp1FE,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: _LL31: _tmp1A4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL32: {struct Cyc_List_List*ts=_tmp1A4;
# 888
s=({struct Cyc_PP_Doc*_tmp201[3U];({struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp202="regions(";_tag_fat(_tmp202,sizeof(char),9U);}));_tmp201[0]=_tmp66B;}),({struct Cyc_PP_Doc*_tmp66A=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp201[1]=_tmp66A;}),({struct Cyc_PP_Doc*_tmp669=Cyc_PP_text(({const char*_tmp203=")";_tag_fat(_tmp203,sizeof(char),2U);}));_tmp201[2]=_tmp669;});Cyc_PP_cat(_tag_fat(_tmp201,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL33: _LL34:
 goto _LL36;case 9U: _LL35: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 894
 s=Cyc_PP_text(({const char*_tmp204="@true";_tag_fat(_tmp204,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
 s=Cyc_PP_text(({const char*_tmp205="@false";_tag_fat(_tmp205,sizeof(char),7U);}));goto _LL0;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2 != 0){_LL41: _tmp1A3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2)->hd;_LL42: {void*t=_tmp1A3;
# 897
s=({struct Cyc_PP_Doc*_tmp206[4U];({struct Cyc_PP_Doc*_tmp66F=Cyc_PP_text(({const char*_tmp207="@thin @numelts";_tag_fat(_tmp207,sizeof(char),15U);}));_tmp206[0]=_tmp66F;}),({struct Cyc_PP_Doc*_tmp66E=Cyc_Absynpp_lb();_tmp206[1]=_tmp66E;}),({struct Cyc_PP_Doc*_tmp66D=Cyc_Absynpp_typ2doc(t);_tmp206[2]=_tmp66D;}),({struct Cyc_PP_Doc*_tmp66C=Cyc_Absynpp_rb();_tmp206[3]=_tmp66C;});Cyc_PP_cat(_tag_fat(_tmp206,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}}else{_LL43: _LL44:
# 899
 s=Cyc_PP_text(({const char*_tmp208="@thin";_tag_fat(_tmp208,sizeof(char),6U);}));goto _LL0;}default: _LL45: _LL46:
 s=Cyc_PP_text(({const char*_tmp209="@fat";_tag_fat(_tmp209,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp19F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp192)->f1;_tmp1A0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp192)->f2;_tmp1A1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp192)->f3;_tmp1A2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp192)->f4;_LLA: {struct Cyc_Core_Opt*k=_tmp19F;void*topt=_tmp1A0;int i=_tmp1A1;struct Cyc_Core_Opt*tvs=_tmp1A2;
# 785
if(topt != 0)
# 787
return Cyc_Absynpp_ntyp2doc(topt);else{
# 789
struct _fat_ptr kindstring=k == 0?({const char*_tmp1B9="K";_tag_fat(_tmp1B9,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B7=({struct Cyc_String_pa_PrintArg_struct _tmp5CB;_tmp5CB.tag=0U,_tmp5CB.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_tmp5CB;});struct Cyc_Int_pa_PrintArg_struct _tmp1B8=({struct Cyc_Int_pa_PrintArg_struct _tmp5CA;_tmp5CA.tag=1U,_tmp5CA.f1=(unsigned long)i;_tmp5CA;});void*_tmp1B5[2U];_tmp1B5[0]=& _tmp1B7,_tmp1B5[1]=& _tmp1B8;({struct _fat_ptr _tmp670=({const char*_tmp1B6="`E%s%d";_tag_fat(_tmp1B6,sizeof(char),7U);});Cyc_aprintf(_tmp670,_tag_fat(_tmp1B5,sizeof(void*),2U));});}));}
# 792
goto _LL0;}case 2U: _LLB: _tmp19E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp192)->f1;_LLC: {struct Cyc_Absyn_Tvar*tv=_tmp19E;
# 794
s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp1BA[3U];_tmp1BA[0]=s,({struct Cyc_PP_Doc*_tmp672=Cyc_PP_text(({const char*_tmp1BB="::";_tag_fat(_tmp1BB,sizeof(char),3U);}));_tmp1BA[1]=_tmp672;}),({struct Cyc_PP_Doc*_tmp671=Cyc_Absynpp_kindbound2doc(tv->kind);_tmp1BA[2]=_tmp671;});Cyc_PP_cat(_tag_fat(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 6U: _LL15: _tmp19D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp192)->f1;_LL16: {struct Cyc_List_List*ts=_tmp19D;
# 856
s=({struct Cyc_PP_Doc*_tmp1E9[2U];({struct Cyc_PP_Doc*_tmp674=Cyc_Absynpp_dollar();_tmp1E9[0]=_tmp674;}),({struct Cyc_PP_Doc*_tmp673=Cyc_Absynpp_args2doc(ts);_tmp1E9[1]=_tmp673;});Cyc_PP_cat(_tag_fat(_tmp1E9,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 7U: _LL19: _tmp19B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp192)->f1;_tmp19C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp192)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp19B;struct Cyc_List_List*fs=_tmp19C;
# 862
s=({struct Cyc_PP_Doc*_tmp1EF[4U];({struct Cyc_PP_Doc*_tmp678=Cyc_Absynpp_aggr_kind2doc(k);_tmp1EF[0]=_tmp678;}),({struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_lb();_tmp1EF[1]=_tmp677;}),({
struct Cyc_PP_Doc*_tmp676=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_tmp1EF[2]=_tmp676;}),({
struct Cyc_PP_Doc*_tmp675=Cyc_Absynpp_rb();_tmp1EF[3]=_tmp675;});Cyc_PP_cat(_tag_fat(_tmp1EF,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 9U: _LL1F: _tmp19A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp192)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp19A;
# 869
s=({struct Cyc_PP_Doc*_tmp1F4[3U];({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(({const char*_tmp1F5="valueof_t(";_tag_fat(_tmp1F5,sizeof(char),11U);}));_tmp1F4[0]=_tmp67B;}),({struct Cyc_PP_Doc*_tmp67A=Cyc_Absynpp_exp2doc(e);_tmp1F4[1]=_tmp67A;}),({struct Cyc_PP_Doc*_tmp679=Cyc_PP_text(({const char*_tmp1F6=")";_tag_fat(_tmp1F6,sizeof(char),2U);}));_tmp1F4[2]=_tmp679;});Cyc_PP_cat(_tag_fat(_tmp1F4,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 11U: _LL21: _tmp199=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp192)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp199;
s=({struct Cyc_PP_Doc*_tmp1F7[3U];({struct Cyc_PP_Doc*_tmp67E=Cyc_PP_text(({const char*_tmp1F8="typeof(";_tag_fat(_tmp1F8,sizeof(char),8U);}));_tmp1F7[0]=_tmp67E;}),({struct Cyc_PP_Doc*_tmp67D=Cyc_Absynpp_exp2doc(e);_tmp1F7[1]=_tmp67D;}),({struct Cyc_PP_Doc*_tmp67C=Cyc_PP_text(({const char*_tmp1F9=")";_tag_fat(_tmp1F9,sizeof(char),2U);}));_tmp1F7[2]=_tmp67C;});Cyc_PP_cat(_tag_fat(_tmp1F7,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL25: _tmp196=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp192)->f1;_tmp197=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp192)->f2;_tmp198=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp192)->f3;_LL26: {struct _tuple0*n=_tmp196;struct Cyc_List_List*ts=_tmp197;struct Cyc_Absyn_Typedefdecl*kopt=_tmp198;
# 878
s=({struct Cyc_PP_Doc*_tmp1FA[2U];({struct Cyc_PP_Doc*_tmp680=Cyc_Absynpp_qvar2doc(n);_tmp1FA[0]=_tmp680;}),({struct Cyc_PP_Doc*_tmp67F=Cyc_Absynpp_tps2doc(ts);_tmp1FA[1]=_tmp67F;});Cyc_PP_cat(_tag_fat(_tmp1FA,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp192)->f1)->r)){case 0U: _LL37: _tmp195=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp192)->f1)->r)->f1;_LL38: {struct Cyc_Absyn_Aggrdecl*d=_tmp195;
# 891
s=Cyc_Absynpp_aggrdecl2doc(d);goto _LL0;}case 1U: _LL39: _tmp194=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp192)->f1)->r)->f1;_LL3A: {struct Cyc_Absyn_Enumdecl*d=_tmp194;
s=Cyc_Absynpp_enumdecl2doc(d);goto _LL0;}default: _LL3B: _tmp193=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp192)->f1)->r)->f1;_LL3C: {struct Cyc_Absyn_Datatypedecl*d=_tmp193;
s=Cyc_Absynpp_datatypedecl2doc(d);goto _LL0;}}}_LL0:;}
# 902
return s;}
# 905
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _fat_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple18{void*f1;void*f2;};
# 909
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple18*cmp){
struct _tuple18*_tmp20A=cmp;void*_tmp20C;void*_tmp20B;_LL1: _tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;_LL2: {void*r1=_tmp20B;void*r2=_tmp20C;
return({struct Cyc_PP_Doc*_tmp20D[3U];({struct Cyc_PP_Doc*_tmp683=Cyc_Absynpp_rgn2doc(r1);_tmp20D[0]=_tmp683;}),({struct Cyc_PP_Doc*_tmp682=Cyc_PP_text(({const char*_tmp20E=" > ";_tag_fat(_tmp20E,sizeof(char),4U);}));_tmp20D[1]=_tmp682;}),({struct Cyc_PP_Doc*_tmp681=Cyc_Absynpp_rgn2doc(r2);_tmp20D[2]=_tmp681;});Cyc_PP_cat(_tag_fat(_tmp20D,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 914
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _tmp686=({const char*_tmp20F="";_tag_fat(_tmp20F,sizeof(char),1U);});struct _fat_ptr _tmp685=({const char*_tmp210="";_tag_fat(_tmp210,sizeof(char),1U);});struct _fat_ptr _tmp684=({const char*_tmp211=",";_tag_fat(_tmp211,sizeof(char),2U);});Cyc_PP_group(_tmp686,_tmp685,_tmp684,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 918
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*_tmp212=(*t).f1;struct _fat_ptr*vo=_tmp212;
struct Cyc_Core_Opt*dopt=vo == 0?0:({struct Cyc_Core_Opt*_tmp213=_cycalloc(sizeof(*_tmp213));({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(*vo);_tmp213->v=_tmp687;});_tmp213;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 924
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 928
struct Cyc_List_List*_tmp214=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_List_List*arg_docs=_tmp214;
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_tmp689=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp689,({struct Cyc_List_List*_tmp216=_cycalloc(sizeof(*_tmp216));({struct Cyc_PP_Doc*_tmp688=Cyc_PP_text(({const char*_tmp215="...";_tag_fat(_tmp215,sizeof(char),4U);}));_tmp216->hd=_tmp688;}),_tmp216->tl=0;_tmp216;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp217=({struct Cyc_PP_Doc*_tmp219[3U];({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp21A="...";_tag_fat(_tmp21A,sizeof(char),4U);}));_tmp219[0]=_tmp68D;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp68C=Cyc_PP_text(({const char*_tmp21B=" inject ";_tag_fat(_tmp21B,sizeof(char),9U);}));_tmp219[1]=_tmp68C;}):({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp21C=" ";_tag_fat(_tmp21C,sizeof(char),2U);}));_tmp219[1]=_tmp68B;}),({
struct Cyc_PP_Doc*_tmp68A=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D->f1=cyc_varargs->name,_tmp21D->f2=cyc_varargs->tq,_tmp21D->f3=cyc_varargs->type;_tmp21D;}));_tmp219[2]=_tmp68A;});Cyc_PP_cat(_tag_fat(_tmp219,sizeof(struct Cyc_PP_Doc*),3U));});
# 933
struct Cyc_PP_Doc*varargs_doc=_tmp217;
# 937
arg_docs=({struct Cyc_List_List*_tmp68E=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp68E,({struct Cyc_List_List*_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218->hd=varargs_doc,_tmp218->tl=0;_tmp218;}));});}}{
# 939
struct Cyc_PP_Doc*_tmp21E=({struct _fat_ptr _tmp691=({const char*_tmp22D="";_tag_fat(_tmp22D,sizeof(char),1U);});struct _fat_ptr _tmp690=({const char*_tmp22E="";_tag_fat(_tmp22E,sizeof(char),1U);});struct _fat_ptr _tmp68F=({const char*_tmp22F=",";_tag_fat(_tmp22F,sizeof(char),2U);});Cyc_PP_group(_tmp691,_tmp690,_tmp68F,arg_docs);});struct Cyc_PP_Doc*arg_doc=_tmp21E;
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_tmp21F[3U];_tmp21F[0]=arg_doc,({struct Cyc_PP_Doc*_tmp693=Cyc_PP_text(({const char*_tmp220=";";_tag_fat(_tmp220,sizeof(char),2U);}));_tmp21F[1]=_tmp693;}),({struct Cyc_PP_Doc*_tmp692=Cyc_Absynpp_eff2doc(effopt);_tmp21F[2]=_tmp692;});Cyc_PP_cat(_tag_fat(_tmp21F,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
arg_doc=({struct Cyc_PP_Doc*_tmp221[3U];_tmp221[0]=arg_doc,({struct Cyc_PP_Doc*_tmp695=Cyc_PP_text(({const char*_tmp222=":";_tag_fat(_tmp222,sizeof(char),2U);}));_tmp221[1]=_tmp695;}),({struct Cyc_PP_Doc*_tmp694=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp221[2]=_tmp694;});Cyc_PP_cat(_tag_fat(_tmp221,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp223=({struct Cyc_PP_Doc*_tmp22A[3U];({struct Cyc_PP_Doc*_tmp697=Cyc_PP_text(({const char*_tmp22B="(";_tag_fat(_tmp22B,sizeof(char),2U);}));_tmp22A[0]=_tmp697;}),_tmp22A[1]=arg_doc,({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp22C=")";_tag_fat(_tmp22C,sizeof(char),2U);}));_tmp22A[2]=_tmp696;});Cyc_PP_cat(_tag_fat(_tmp22A,sizeof(struct Cyc_PP_Doc*),3U));});struct Cyc_PP_Doc*res=_tmp223;
if(req != 0)
res=({struct Cyc_PP_Doc*_tmp224[4U];_tmp224[0]=res,({struct Cyc_PP_Doc*_tmp69A=Cyc_PP_text(({const char*_tmp225=" @requires(";_tag_fat(_tmp225,sizeof(char),12U);}));_tmp224[1]=_tmp69A;}),({struct Cyc_PP_Doc*_tmp699=Cyc_Absynpp_exp2doc(req);_tmp224[2]=_tmp699;}),({struct Cyc_PP_Doc*_tmp698=Cyc_PP_text(({const char*_tmp226=")";_tag_fat(_tmp226,sizeof(char),2U);}));_tmp224[3]=_tmp698;});Cyc_PP_cat(_tag_fat(_tmp224,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
res=({struct Cyc_PP_Doc*_tmp227[4U];_tmp227[0]=res,({struct Cyc_PP_Doc*_tmp69D=Cyc_PP_text(({const char*_tmp228=" @ensures(";_tag_fat(_tmp228,sizeof(char),11U);}));_tmp227[1]=_tmp69D;}),({struct Cyc_PP_Doc*_tmp69C=Cyc_Absynpp_exp2doc(ens);_tmp227[2]=_tmp69C;}),({struct Cyc_PP_Doc*_tmp69B=Cyc_PP_text(({const char*_tmp229=")";_tag_fat(_tmp229,sizeof(char),2U);}));_tmp227[3]=_tmp69B;});Cyc_PP_cat(_tag_fat(_tmp227,sizeof(struct Cyc_PP_Doc*),4U));});
return res;}}}
# 952
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _fat_ptr*v){return Cyc_PP_text(*v);}
# 954
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp230=0;struct Cyc_List_List*prefix=_tmp230;
int match;
{union Cyc_Absyn_Nmspace _tmp231=(*q).f1;union Cyc_Absyn_Nmspace _stmttmp12=_tmp231;union Cyc_Absyn_Nmspace _tmp232=_stmttmp12;struct Cyc_List_List*_tmp233;struct Cyc_List_List*_tmp234;struct Cyc_List_List*_tmp235;switch((_tmp232.C_n).tag){case 4U: _LL1: _LL2:
 _tmp235=0;goto _LL4;case 1U: _LL3: _tmp235=(_tmp232.Rel_n).val;_LL4: {struct Cyc_List_List*x=_tmp235;
# 960
match=0;
prefix=x;
goto _LL0;}case 3U: _LL5: _tmp234=(_tmp232.C_n).val;_LL6: {struct Cyc_List_List*x=_tmp234;
# 964
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
# 966
goto _LL0;}default: _LL7: _tmp233=(_tmp232.Abs_n).val;_LL8: {struct Cyc_List_List*x=_tmp233;
# 968
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
prefix=Cyc_Absynpp_qvar_to_Cids && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236->hd=Cyc_Absynpp_cyc_stringptr,_tmp236->tl=x;_tmp236;}): x;
goto _LL0;}}_LL0:;}
# 972
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _fat_ptr)({struct Cyc_List_List*_tmp69F=({struct Cyc_List_List*_tmp69E=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp69E,({struct Cyc_List_List*_tmp237=_cycalloc(sizeof(*_tmp237));
_tmp237->hd=(*q).f2,_tmp237->tl=0;_tmp237;}));});
# 973
Cyc_str_sepstr(_tmp69F,({const char*_tmp238="_";_tag_fat(_tmp238,sizeof(char),2U);}));});else{
# 977
if(match)
return*(*q).f2;else{
# 980
return(struct _fat_ptr)({struct Cyc_List_List*_tmp6A1=({struct Cyc_List_List*_tmp6A0=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6A0,({struct Cyc_List_List*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->hd=(*q).f2,_tmp239->tl=0;_tmp239;}));});Cyc_str_sepstr(_tmp6A1,({const char*_tmp23A="::";_tag_fat(_tmp23A,sizeof(char),3U);}));});}}}
# 984
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 988
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr _tmp23B=Cyc_Absynpp_qvar2string(q);struct _fat_ptr qs=_tmp23B;
if(Cyc_PP_tex_output)
# 992
return({struct _fat_ptr _tmp6A4=(struct _fat_ptr)({struct _fat_ptr _tmp6A3=(struct _fat_ptr)({struct _fat_ptr _tmp6A2=({const char*_tmp23C="\\textbf{";_tag_fat(_tmp23C,sizeof(char),9U);});Cyc_strconcat(_tmp6A2,(struct _fat_ptr)qs);});Cyc_strconcat(_tmp6A3,({const char*_tmp23D="}";_tag_fat(_tmp23D,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp6A4,(int)
Cyc_strlen((struct _fat_ptr)qs));});else{
return Cyc_PP_text(qs);}}
# 997
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 999
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 1002
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp23E=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp13=_tmp23E;union Cyc_Absyn_Nmspace _tmp23F=_stmttmp13;struct Cyc_List_List*_tmp240;struct Cyc_List_List*_tmp241;switch((_tmp23F.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp23F.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 1013
 return(struct _fat_ptr)({struct _fat_ptr _tmp6A5=({const char*_tmp242="/* bad namespace : */ ";_tag_fat(_tmp242,sizeof(char),23U);});Cyc_strconcat(_tmp6A5,(struct _fat_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp241=(_tmp23F.C_n).val;_LL6: {struct Cyc_List_List*l=_tmp241;
# 1006
_tmp240=l;goto _LL8;}default: _LL7: _tmp240=(_tmp23F.Abs_n).val;_LL8: {struct Cyc_List_List*l=_tmp240;
# 1008
if(((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,l,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 1011
goto _LLA;}}}_LL0:;}else{
# 1016
return*(*v).f2;}}
# 1019
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 1022
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _fat_ptr _tmp243=Cyc_Absynpp_typedef_name2string(v);struct _fat_ptr vs=_tmp243;
if(Cyc_PP_tex_output)
# 1026
return({struct _fat_ptr _tmp6A8=(struct _fat_ptr)({struct _fat_ptr _tmp6A7=(struct _fat_ptr)({struct _fat_ptr _tmp6A6=({const char*_tmp244="\\textbf{";_tag_fat(_tmp244,sizeof(char),9U);});Cyc_strconcat(_tmp6A6,(struct _fat_ptr)vs);});Cyc_strconcat(_tmp6A7,({const char*_tmp245="}";_tag_fat(_tmp245,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp6A8,(int)
Cyc_strlen((struct _fat_ptr)vs));});else{
return Cyc_PP_text(vs);}}
# 1031
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp6A9=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp6A9,t,0);});}
# 1035
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_tmp246=f;unsigned _tmp247;struct _fat_ptr*_tmp248;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp246)->tag == 0U){_LL1: _tmp248=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp246)->f1;_LL2: {struct _fat_ptr*n=_tmp248;
return Cyc_PP_textptr(n);}}else{_LL3: _tmp247=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp246)->f1;_LL4: {unsigned n=_tmp247;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp24B=({struct Cyc_Int_pa_PrintArg_struct _tmp5CC;_tmp5CC.tag=1U,_tmp5CC.f1=(unsigned long)((int)n);_tmp5CC;});void*_tmp249[1U];_tmp249[0]=& _tmp24B;({struct _fat_ptr _tmp6AA=({const char*_tmp24A="%d";_tag_fat(_tmp24A,sizeof(char),3U);});Cyc_aprintf(_tmp6AA,_tag_fat(_tmp249,sizeof(void*),1U));});}));}}_LL0:;}
# 1049 "/tmp/cyclone/src/absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp24C=e->r;void*_stmttmp14=_tmp24C;void*_tmp24D=_stmttmp14;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_Absyn_Exp*_tmp24F;enum Cyc_Absyn_Primop _tmp250;switch(*((int*)_tmp24D)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp250=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp24D)->f1;_LL6: {enum Cyc_Absyn_Primop p=_tmp250;
# 1054
enum Cyc_Absyn_Primop _tmp251=p;switch(_tmp251){case Cyc_Absyn_Plus: _LL58: _LL59:
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
# 1076
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return - 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp24F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp24D)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp24F;
return Cyc_Absynpp_exp_prec(e2);}case 13U: _LL1B: _tmp24E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp24D)->f1;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp24E;
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
# 1119
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct _tuple19{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1123
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
struct _tuple19*_tmp252=(struct _tuple19*)o->hd;struct _tuple19*_stmttmp15=_tmp252;struct _tuple19*_tmp253=_stmttmp15;struct Cyc_Absyn_Exp*_tmp255;struct _fat_ptr _tmp254;_LL1: _tmp254=_tmp253->f1;_tmp255=_tmp253->f2;_LL2: {struct _fat_ptr c=_tmp254;struct Cyc_Absyn_Exp*e=_tmp255;
s=({struct Cyc_PP_Doc*_tmp256[5U];({struct Cyc_PP_Doc*_tmp6AF=Cyc_PP_text(({const char*_tmp257="\"";_tag_fat(_tmp257,sizeof(char),2U);}));_tmp256[0]=_tmp6AF;}),({struct Cyc_PP_Doc*_tmp6AE=Cyc_PP_text(c);_tmp256[1]=_tmp6AE;}),({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp258="\" (";_tag_fat(_tmp258,sizeof(char),4U);}));_tmp256[2]=_tmp6AD;}),({struct Cyc_PP_Doc*_tmp6AC=Cyc_Absynpp_exp2doc(e);_tmp256[3]=_tmp6AC;}),({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_text(({const char*_tmp259=")";_tag_fat(_tmp259,sizeof(char),2U);}));_tmp256[4]=_tmp6AB;});Cyc_PP_cat(_tag_fat(_tmp256,sizeof(struct Cyc_PP_Doc*),5U));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp25A[2U];_tmp25A[0]=s,({struct Cyc_PP_Doc*_tmp6B0=Cyc_PP_text(({const char*_tmp25B=",";_tag_fat(_tmp25B,sizeof(char),2U);}));_tmp25A[1]=_tmp6B0;});Cyc_PP_cat(_tag_fat(_tmp25A,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1133
return s;}
# 1136
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp25C[2U];({struct Cyc_PP_Doc*_tmp6B2=Cyc_PP_text(({const char*_tmp25D=": ";_tag_fat(_tmp25D,sizeof(char),3U);}));_tmp25C[0]=_tmp6B2;}),({struct Cyc_PP_Doc*_tmp6B1=Cyc_Absynpp_asm_iolist_doc_in(o);_tmp25C[1]=_tmp6B1;});Cyc_PP_cat(_tag_fat(_tmp25C,sizeof(struct Cyc_PP_Doc*),2U));});
# 1141
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_tmp25E[3U];_tmp25E[0]=s,({struct Cyc_PP_Doc*_tmp6B4=Cyc_PP_text(({const char*_tmp25F=": : ";_tag_fat(_tmp25F,sizeof(char),5U);}));_tmp25E[1]=_tmp6B4;}),({struct Cyc_PP_Doc*_tmp6B3=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp25E[2]=_tmp6B3;});Cyc_PP_cat(_tag_fat(_tmp25E,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1145
s=({struct Cyc_PP_Doc*_tmp260[3U];_tmp260[0]=s,({struct Cyc_PP_Doc*_tmp6B6=Cyc_PP_text(({const char*_tmp261=" : ";_tag_fat(_tmp261,sizeof(char),4U);}));_tmp260[1]=_tmp6B6;}),({struct Cyc_PP_Doc*_tmp6B5=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp260[2]=_tmp6B5;});Cyc_PP_cat(_tag_fat(_tmp260,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1147
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_tmp262[2U];_tmp262[0]=s,ncol == 0?({struct Cyc_PP_Doc*_tmp6B9=Cyc_PP_text(({const char*_tmp263=": : :";_tag_fat(_tmp263,sizeof(char),6U);}));_tmp262[1]=_tmp6B9;}):(ncol == 1?({struct Cyc_PP_Doc*_tmp6B8=Cyc_PP_text(({const char*_tmp264=" : : ";_tag_fat(_tmp264,sizeof(char),6U);}));_tmp262[1]=_tmp6B8;}):({struct Cyc_PP_Doc*_tmp6B7=Cyc_PP_text(({const char*_tmp265=" : ";_tag_fat(_tmp265,sizeof(char),4U);}));_tmp262[1]=_tmp6B7;}));Cyc_PP_cat(_tag_fat(_tmp262,sizeof(struct Cyc_PP_Doc*),2U));});
while(cl != 0){
s=({struct Cyc_PP_Doc*_tmp266[4U];_tmp266[0]=s,({struct Cyc_PP_Doc*_tmp6BC=Cyc_PP_text(({const char*_tmp267="\"";_tag_fat(_tmp267,sizeof(char),2U);}));_tmp266[1]=_tmp6BC;}),({struct Cyc_PP_Doc*_tmp6BB=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_tmp266[2]=_tmp6BB;}),({struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_text(({const char*_tmp268="\"";_tag_fat(_tmp268,sizeof(char),2U);}));_tmp266[3]=_tmp6BA;});Cyc_PP_cat(_tag_fat(_tmp266,sizeof(struct Cyc_PP_Doc*),4U));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_tmp269[2U];_tmp269[0]=s,({struct Cyc_PP_Doc*_tmp6BD=Cyc_PP_text(({const char*_tmp26A=", ";_tag_fat(_tmp26A,sizeof(char),3U);}));_tmp269[1]=_tmp6BD;});Cyc_PP_cat(_tag_fat(_tmp269,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1157
return s;}
# 1160
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp26B=e->r;void*_stmttmp16=_tmp26B;void*_tmp26C=_stmttmp16;struct Cyc_Absyn_Stmt*_tmp26D;struct Cyc_List_List*_tmp26F;struct Cyc_Core_Opt*_tmp26E;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*_tmp270;int _tmp276;struct Cyc_Absyn_Exp*_tmp275;void**_tmp274;struct Cyc_Absyn_Exp*_tmp273;int _tmp272;struct Cyc_Absyn_Enumfield*_tmp277;struct Cyc_Absyn_Enumfield*_tmp278;struct Cyc_Absyn_Datatypefield*_tmp27A;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27E;struct Cyc_List_List*_tmp27D;struct _tuple0*_tmp27C;void*_tmp280;struct Cyc_Absyn_Exp*_tmp27F;struct Cyc_Absyn_Exp*_tmp283;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Vardecl*_tmp281;struct Cyc_List_List*_tmp284;struct Cyc_List_List*_tmp286;struct _tuple8*_tmp285;struct Cyc_List_List*_tmp287;struct Cyc_Absyn_Exp*_tmp289;struct Cyc_Absyn_Exp*_tmp288;struct _fat_ptr*_tmp28B;struct Cyc_Absyn_Exp*_tmp28A;struct _fat_ptr*_tmp28D;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_List_List*_tmp290;void*_tmp28F;struct _fat_ptr*_tmp292;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_List_List*_tmp297;struct Cyc_List_List*_tmp296;struct Cyc_List_List*_tmp295;struct _fat_ptr _tmp294;int _tmp293;void*_tmp298;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp29B;void*_tmp29C;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp2A1;void*_tmp2A0;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_List_List*_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AD;enum Cyc_Absyn_Incrementor _tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Core_Opt*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_List_List*_tmp2B6;enum Cyc_Absyn_Primop _tmp2B5;struct _fat_ptr _tmp2B7;void*_tmp2B8;union Cyc_Absyn_Cnst _tmp2B9;switch(*((int*)_tmp26C)){case 0U: _LL1: _tmp2B9=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp2B9;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1U: _LL3: _tmp2B8=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL4: {void*b=_tmp2B8;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL5: _tmp2B7=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL6: {struct _fat_ptr p=_tmp2B7;
# 1167
s=({struct Cyc_PP_Doc*_tmp2BA[4U];({struct Cyc_PP_Doc*_tmp6C1=Cyc_PP_text(({const char*_tmp2BB="__cyclone_pragma__";_tag_fat(_tmp2BB,sizeof(char),19U);}));_tmp2BA[0]=_tmp6C1;}),({struct Cyc_PP_Doc*_tmp6C0=Cyc_PP_text(({const char*_tmp2BC="(";_tag_fat(_tmp2BC,sizeof(char),2U);}));_tmp2BA[1]=_tmp6C0;}),({struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_text(p);_tmp2BA[2]=_tmp6BF;}),({struct Cyc_PP_Doc*_tmp6BE=Cyc_PP_text(({const char*_tmp2BD=")";_tag_fat(_tmp2BD,sizeof(char),2U);}));_tmp2BA[3]=_tmp6BE;});Cyc_PP_cat(_tag_fat(_tmp2BA,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 3U: _LL7: _tmp2B5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2B6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp2B5;struct Cyc_List_List*es=_tmp2B6;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4U: _LL9: _tmp2B2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2B3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp2B4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp2B2;struct Cyc_Core_Opt*popt=_tmp2B3;struct Cyc_Absyn_Exp*e2=_tmp2B4;
# 1170
s=({struct Cyc_PP_Doc*_tmp2BE[5U];({struct Cyc_PP_Doc*_tmp6C7=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2BE[0]=_tmp6C7;}),({
struct Cyc_PP_Doc*_tmp6C6=Cyc_PP_text(({const char*_tmp2BF=" ";_tag_fat(_tmp2BF,sizeof(char),2U);}));_tmp2BE[1]=_tmp6C6;}),
popt == 0?({struct Cyc_PP_Doc*_tmp6C5=Cyc_PP_nil_doc();_tmp2BE[2]=_tmp6C5;}):({struct Cyc_PP_Doc*_tmp6C4=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_tmp2BE[2]=_tmp6C4;}),({
struct Cyc_PP_Doc*_tmp6C3=Cyc_PP_text(({const char*_tmp2C0="= ";_tag_fat(_tmp2C0,sizeof(char),3U);}));_tmp2BE[3]=_tmp6C3;}),({
struct Cyc_PP_Doc*_tmp6C2=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2BE[4]=_tmp6C2;});Cyc_PP_cat(_tag_fat(_tmp2BE,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp2B0=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2B1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LLC: {struct Cyc_Absyn_Exp*e2=_tmp2B0;enum Cyc_Absyn_Incrementor i=_tmp2B1;
# 1177
struct Cyc_PP_Doc*_tmp2C1=Cyc_Absynpp_exp2doc_prec(myprec,e2);struct Cyc_PP_Doc*es=_tmp2C1;
{enum Cyc_Absyn_Incrementor _tmp2C2=i;switch(_tmp2C2){case Cyc_Absyn_PreInc: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp2C3[2U];({struct Cyc_PP_Doc*_tmp6C8=Cyc_PP_text(({const char*_tmp2C4="++";_tag_fat(_tmp2C4,sizeof(char),3U);}));_tmp2C3[0]=_tmp6C8;}),_tmp2C3[1]=es;Cyc_PP_cat(_tag_fat(_tmp2C3,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PreDec: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp2C5[2U];({struct Cyc_PP_Doc*_tmp6C9=Cyc_PP_text(({const char*_tmp2C6="--";_tag_fat(_tmp2C6,sizeof(char),3U);}));_tmp2C5[0]=_tmp6C9;}),_tmp2C5[1]=es;Cyc_PP_cat(_tag_fat(_tmp2C5,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PostInc: _LL5C: _LL5D:
 s=({struct Cyc_PP_Doc*_tmp2C7[2U];_tmp2C7[0]=es,({struct Cyc_PP_Doc*_tmp6CA=Cyc_PP_text(({const char*_tmp2C8="++";_tag_fat(_tmp2C8,sizeof(char),3U);}));_tmp2C7[1]=_tmp6CA;});Cyc_PP_cat(_tag_fat(_tmp2C7,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PostDec: _LL5E: _LL5F:
 goto _LL61;default: _LL60: _LL61:
 s=({struct Cyc_PP_Doc*_tmp2C9[2U];_tmp2C9[0]=es,({struct Cyc_PP_Doc*_tmp6CB=Cyc_PP_text(({const char*_tmp2CA="--";_tag_fat(_tmp2CA,sizeof(char),3U);}));_tmp2C9[1]=_tmp6CB;});Cyc_PP_cat(_tag_fat(_tmp2C9,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;}_LL57:;}
# 1185
goto _LL0;}case 6U: _LLD: _tmp2AD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2AE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp2AF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp2AD;struct Cyc_Absyn_Exp*e2=_tmp2AE;struct Cyc_Absyn_Exp*e3=_tmp2AF;
# 1187
s=({struct Cyc_PP_Doc*_tmp2CB[5U];({struct Cyc_PP_Doc*_tmp6D0=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2CB[0]=_tmp6D0;}),({struct Cyc_PP_Doc*_tmp6CF=Cyc_PP_text(({const char*_tmp2CC=" ? ";_tag_fat(_tmp2CC,sizeof(char),4U);}));_tmp2CB[1]=_tmp6CF;}),({struct Cyc_PP_Doc*_tmp6CE=Cyc_Absynpp_exp2doc_prec(0,e2);_tmp2CB[2]=_tmp6CE;}),({
struct Cyc_PP_Doc*_tmp6CD=Cyc_PP_text(({const char*_tmp2CD=" : ";_tag_fat(_tmp2CD,sizeof(char),4U);}));_tmp2CB[3]=_tmp6CD;}),({struct Cyc_PP_Doc*_tmp6CC=Cyc_Absynpp_exp2doc_prec(myprec,e3);_tmp2CB[4]=_tmp6CC;});Cyc_PP_cat(_tag_fat(_tmp2CB,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 7U: _LLF: _tmp2AB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2AC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp2AB;struct Cyc_Absyn_Exp*e2=_tmp2AC;
# 1191
s=({struct Cyc_PP_Doc*_tmp2CE[3U];({struct Cyc_PP_Doc*_tmp6D3=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2CE[0]=_tmp6D3;}),({struct Cyc_PP_Doc*_tmp6D2=Cyc_PP_text(({const char*_tmp2CF=" && ";_tag_fat(_tmp2CF,sizeof(char),5U);}));_tmp2CE[1]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2CE[2]=_tmp6D1;});Cyc_PP_cat(_tag_fat(_tmp2CE,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 8U: _LL11: _tmp2A9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2AA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp2A9;struct Cyc_Absyn_Exp*e2=_tmp2AA;
# 1194
s=({struct Cyc_PP_Doc*_tmp2D0[3U];({struct Cyc_PP_Doc*_tmp6D6=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D0[0]=_tmp6D6;}),({struct Cyc_PP_Doc*_tmp6D5=Cyc_PP_text(({const char*_tmp2D1=" || ";_tag_fat(_tmp2D1,sizeof(char),5U);}));_tmp2D0[1]=_tmp6D5;}),({struct Cyc_PP_Doc*_tmp6D4=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2D0[2]=_tmp6D4;});Cyc_PP_cat(_tag_fat(_tmp2D0,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 9U: _LL13: _tmp2A7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2A8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp2A7;struct Cyc_Absyn_Exp*e2=_tmp2A8;
# 1199
s=({struct Cyc_PP_Doc*_tmp2D2[3U];({struct Cyc_PP_Doc*_tmp6D9=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_tmp2D2[0]=_tmp6D9;}),({struct Cyc_PP_Doc*_tmp6D8=Cyc_PP_text(({const char*_tmp2D3=", ";_tag_fat(_tmp2D3,sizeof(char),3U);}));_tmp2D2[1]=_tmp6D8;}),({struct Cyc_PP_Doc*_tmp6D7=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_tmp2D2[2]=_tmp6D7;});Cyc_PP_cat(_tag_fat(_tmp2D2,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 10U: _LL15: _tmp2A5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2A6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp2A5;struct Cyc_List_List*es=_tmp2A6;
# 1202
s=({struct Cyc_PP_Doc*_tmp2D4[4U];({struct Cyc_PP_Doc*_tmp6DD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D4[0]=_tmp6DD;}),({
struct Cyc_PP_Doc*_tmp6DC=Cyc_PP_text(({const char*_tmp2D5="(";_tag_fat(_tmp2D5,sizeof(char),2U);}));_tmp2D4[1]=_tmp6DC;}),({
struct Cyc_PP_Doc*_tmp6DB=Cyc_Absynpp_exps2doc_prec(20,es);_tmp2D4[2]=_tmp6DB;}),({
struct Cyc_PP_Doc*_tmp6DA=Cyc_PP_text(({const char*_tmp2D6=")";_tag_fat(_tmp2D6,sizeof(char),2U);}));_tmp2D4[3]=_tmp6DA;});Cyc_PP_cat(_tag_fat(_tmp2D4,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 11U: _LL17: _tmp2A4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp2A4;
s=({struct Cyc_PP_Doc*_tmp2D7[2U];({struct Cyc_PP_Doc*_tmp6DF=Cyc_PP_text(({const char*_tmp2D8="throw ";_tag_fat(_tmp2D8,sizeof(char),7U);}));_tmp2D7[0]=_tmp6DF;}),({struct Cyc_PP_Doc*_tmp6DE=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D7[1]=_tmp6DE;});Cyc_PP_cat(_tag_fat(_tmp2D7,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 12U: _LL19: _tmp2A3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp2A3;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13U: _LL1B: _tmp2A2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp2A2;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14U: _LL1D: _tmp2A0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp2A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL1E: {void*t=_tmp2A0;struct Cyc_Absyn_Exp*e1=_tmp2A1;
# 1211
s=({struct Cyc_PP_Doc*_tmp2D9[4U];({struct Cyc_PP_Doc*_tmp6E3=Cyc_PP_text(({const char*_tmp2DA="(";_tag_fat(_tmp2DA,sizeof(char),2U);}));_tmp2D9[0]=_tmp6E3;}),({struct Cyc_PP_Doc*_tmp6E2=Cyc_Absynpp_typ2doc(t);_tmp2D9[1]=_tmp6E2;}),({struct Cyc_PP_Doc*_tmp6E1=Cyc_PP_text(({const char*_tmp2DB=")";_tag_fat(_tmp2DB,sizeof(char),2U);}));_tmp2D9[2]=_tmp6E1;}),({struct Cyc_PP_Doc*_tmp6E0=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D9[3]=_tmp6E0;});Cyc_PP_cat(_tag_fat(_tmp2D9,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1F: _tmp29F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp29F;
s=({struct Cyc_PP_Doc*_tmp2DC[2U];({struct Cyc_PP_Doc*_tmp6E5=Cyc_PP_text(({const char*_tmp2DD="&";_tag_fat(_tmp2DD,sizeof(char),2U);}));_tmp2DC[0]=_tmp6E5;}),({struct Cyc_PP_Doc*_tmp6E4=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2DC[1]=_tmp6E4;});Cyc_PP_cat(_tag_fat(_tmp2DC,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 16U: _LL21: _tmp29D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp29E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL22: {struct Cyc_Absyn_Exp*ropt=_tmp29D;struct Cyc_Absyn_Exp*e1=_tmp29E;
# 1214
if(ropt == 0)
s=({struct Cyc_PP_Doc*_tmp2DE[2U];({struct Cyc_PP_Doc*_tmp6E7=Cyc_PP_text(({const char*_tmp2DF="new ";_tag_fat(_tmp2DF,sizeof(char),5U);}));_tmp2DE[0]=_tmp6E7;}),({struct Cyc_PP_Doc*_tmp6E6=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2DE[1]=_tmp6E6;});Cyc_PP_cat(_tag_fat(_tmp2DE,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1217
s=({struct Cyc_PP_Doc*_tmp2E0[4U];({struct Cyc_PP_Doc*_tmp6EB=Cyc_PP_text(({const char*_tmp2E1="rnew(";_tag_fat(_tmp2E1,sizeof(char),6U);}));_tmp2E0[0]=_tmp6EB;}),({struct Cyc_PP_Doc*_tmp6EA=Cyc_Absynpp_exp2doc(ropt);_tmp2E0[1]=_tmp6EA;}),({struct Cyc_PP_Doc*_tmp6E9=Cyc_PP_text(({const char*_tmp2E2=") ";_tag_fat(_tmp2E2,sizeof(char),3U);}));_tmp2E0[2]=_tmp6E9;}),({struct Cyc_PP_Doc*_tmp6E8=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2E0[3]=_tmp6E8;});Cyc_PP_cat(_tag_fat(_tmp2E0,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;}case 17U: _LL23: _tmp29C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL24: {void*t=_tmp29C;
s=({struct Cyc_PP_Doc*_tmp2E3[3U];({struct Cyc_PP_Doc*_tmp6EE=Cyc_PP_text(({const char*_tmp2E4="sizeof(";_tag_fat(_tmp2E4,sizeof(char),8U);}));_tmp2E3[0]=_tmp6EE;}),({struct Cyc_PP_Doc*_tmp6ED=Cyc_Absynpp_typ2doc(t);_tmp2E3[1]=_tmp6ED;}),({struct Cyc_PP_Doc*_tmp6EC=Cyc_PP_text(({const char*_tmp2E5=")";_tag_fat(_tmp2E5,sizeof(char),2U);}));_tmp2E3[2]=_tmp6EC;});Cyc_PP_cat(_tag_fat(_tmp2E3,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 18U: _LL25: _tmp29B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp29B;
s=({struct Cyc_PP_Doc*_tmp2E6[3U];({struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_text(({const char*_tmp2E7="sizeof(";_tag_fat(_tmp2E7,sizeof(char),8U);}));_tmp2E6[0]=_tmp6F1;}),({struct Cyc_PP_Doc*_tmp6F0=Cyc_Absynpp_exp2doc(e1);_tmp2E6[1]=_tmp6F0;}),({struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_text(({const char*_tmp2E8=")";_tag_fat(_tmp2E8,sizeof(char),2U);}));_tmp2E6[2]=_tmp6EF;});Cyc_PP_cat(_tag_fat(_tmp2E6,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 42U: _LL27: _tmp29A=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp29A;
# 1222
s=({struct Cyc_PP_Doc*_tmp2E9[3U];({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp2EA="@assert(";_tag_fat(_tmp2EA,sizeof(char),9U);}));_tmp2E9[0]=_tmp6F4;}),({struct Cyc_PP_Doc*_tmp6F3=Cyc_Absynpp_exp2doc(e);_tmp2E9[1]=_tmp6F3;}),({struct Cyc_PP_Doc*_tmp6F2=Cyc_PP_text(({const char*_tmp2EB=")";_tag_fat(_tmp2EB,sizeof(char),2U);}));_tmp2E9[2]=_tmp6F2;});Cyc_PP_cat(_tag_fat(_tmp2E9,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 41U: _LL29: _tmp299=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp299;
# 1224
s=({struct Cyc_PP_Doc*_tmp2EC[3U];({struct Cyc_PP_Doc*_tmp6F7=Cyc_PP_text(({const char*_tmp2ED="__extension__(";_tag_fat(_tmp2ED,sizeof(char),15U);}));_tmp2EC[0]=_tmp6F7;}),({struct Cyc_PP_Doc*_tmp6F6=Cyc_Absynpp_exp2doc(e);_tmp2EC[1]=_tmp6F6;}),({struct Cyc_PP_Doc*_tmp6F5=Cyc_PP_text(({const char*_tmp2EE=")";_tag_fat(_tmp2EE,sizeof(char),2U);}));_tmp2EC[2]=_tmp6F5;});Cyc_PP_cat(_tag_fat(_tmp2EC,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 39U: _LL2B: _tmp298=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL2C: {void*t=_tmp298;
s=({struct Cyc_PP_Doc*_tmp2EF[3U];({struct Cyc_PP_Doc*_tmp6FA=Cyc_PP_text(({const char*_tmp2F0="valueof(";_tag_fat(_tmp2F0,sizeof(char),9U);}));_tmp2EF[0]=_tmp6FA;}),({struct Cyc_PP_Doc*_tmp6F9=Cyc_Absynpp_typ2doc(t);_tmp2EF[1]=_tmp6F9;}),({struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_text(({const char*_tmp2F1=")";_tag_fat(_tmp2F1,sizeof(char),2U);}));_tmp2EF[2]=_tmp6F8;});Cyc_PP_cat(_tag_fat(_tmp2EF,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 40U: _LL2D: _tmp293=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp294=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp295=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_tmp296=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26C)->f4;_tmp297=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26C)->f5;_LL2E: {int vol=_tmp293;struct _fat_ptr t=_tmp294;struct Cyc_List_List*o=_tmp295;struct Cyc_List_List*i=_tmp296;struct Cyc_List_List*cl=_tmp297;
# 1227
if(vol)
s=({struct Cyc_PP_Doc*_tmp2F2[7U];({struct Cyc_PP_Doc*_tmp701=Cyc_PP_text(({const char*_tmp2F3="__asm__";_tag_fat(_tmp2F3,sizeof(char),8U);}));_tmp2F2[0]=_tmp701;}),({struct Cyc_PP_Doc*_tmp700=Cyc_PP_text(({const char*_tmp2F4=" volatile (";_tag_fat(_tmp2F4,sizeof(char),12U);}));_tmp2F2[1]=_tmp700;}),({struct Cyc_PP_Doc*_tmp6FF=Cyc_PP_text(({const char*_tmp2F5="\"";_tag_fat(_tmp2F5,sizeof(char),2U);}));_tmp2F2[2]=_tmp6FF;}),({struct Cyc_PP_Doc*_tmp6FE=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_tmp2F2[3]=_tmp6FE;}),({struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_text(({const char*_tmp2F6="\"";_tag_fat(_tmp2F6,sizeof(char),2U);}));_tmp2F2[4]=_tmp6FD;}),({struct Cyc_PP_Doc*_tmp6FC=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2F2[5]=_tmp6FC;}),({struct Cyc_PP_Doc*_tmp6FB=Cyc_PP_text(({const char*_tmp2F7=")";_tag_fat(_tmp2F7,sizeof(char),2U);}));_tmp2F2[6]=_tmp6FB;});Cyc_PP_cat(_tag_fat(_tmp2F2,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1230
s=({struct Cyc_PP_Doc*_tmp2F8[6U];({struct Cyc_PP_Doc*_tmp707=Cyc_PP_text(({const char*_tmp2F9="__asm__(";_tag_fat(_tmp2F9,sizeof(char),9U);}));_tmp2F8[0]=_tmp707;}),({struct Cyc_PP_Doc*_tmp706=Cyc_PP_text(({const char*_tmp2FA="\"";_tag_fat(_tmp2FA,sizeof(char),2U);}));_tmp2F8[1]=_tmp706;}),({struct Cyc_PP_Doc*_tmp705=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_tmp2F8[2]=_tmp705;}),({struct Cyc_PP_Doc*_tmp704=Cyc_PP_text(({const char*_tmp2FB="\"";_tag_fat(_tmp2FB,sizeof(char),2U);}));_tmp2F8[3]=_tmp704;}),({struct Cyc_PP_Doc*_tmp703=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2F8[4]=_tmp703;}),({struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp2FC=")";_tag_fat(_tmp2FC,sizeof(char),2U);}));_tmp2F8[5]=_tmp702;});Cyc_PP_cat(_tag_fat(_tmp2F8,sizeof(struct Cyc_PP_Doc*),6U));});}
goto _LL0;}case 38U: _LL2F: _tmp291=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp292=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL30: {struct Cyc_Absyn_Exp*e=_tmp291;struct _fat_ptr*f=_tmp292;
# 1233
s=({struct Cyc_PP_Doc*_tmp2FD[5U];({struct Cyc_PP_Doc*_tmp70C=Cyc_PP_text(({const char*_tmp2FE="tagcheck(";_tag_fat(_tmp2FE,sizeof(char),10U);}));_tmp2FD[0]=_tmp70C;}),({struct Cyc_PP_Doc*_tmp70B=Cyc_Absynpp_exp2doc(e);_tmp2FD[1]=_tmp70B;}),({struct Cyc_PP_Doc*_tmp70A=Cyc_PP_text(({const char*_tmp2FF=".";_tag_fat(_tmp2FF,sizeof(char),2U);}));_tmp2FD[2]=_tmp70A;}),({struct Cyc_PP_Doc*_tmp709=Cyc_PP_textptr(f);_tmp2FD[3]=_tmp709;}),({struct Cyc_PP_Doc*_tmp708=Cyc_PP_text(({const char*_tmp300=")";_tag_fat(_tmp300,sizeof(char),2U);}));_tmp2FD[4]=_tmp708;});Cyc_PP_cat(_tag_fat(_tmp2FD,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 19U: _LL31: _tmp28F=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp290=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL32: {void*t=_tmp28F;struct Cyc_List_List*l=_tmp290;
# 1236
s=({struct Cyc_PP_Doc*_tmp301[5U];({struct Cyc_PP_Doc*_tmp712=Cyc_PP_text(({const char*_tmp302="offsetof(";_tag_fat(_tmp302,sizeof(char),10U);}));_tmp301[0]=_tmp712;}),({struct Cyc_PP_Doc*_tmp711=Cyc_Absynpp_typ2doc(t);_tmp301[1]=_tmp711;}),({struct Cyc_PP_Doc*_tmp710=Cyc_PP_text(({const char*_tmp303=",";_tag_fat(_tmp303,sizeof(char),2U);}));_tmp301[2]=_tmp710;}),({
struct Cyc_PP_Doc*_tmp70F=({struct _fat_ptr _tmp70E=({const char*_tmp304=".";_tag_fat(_tmp304,sizeof(char),2U);});Cyc_PP_seq(_tmp70E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_offsetof_field_to_doc,l));});_tmp301[3]=_tmp70F;}),({struct Cyc_PP_Doc*_tmp70D=Cyc_PP_text(({const char*_tmp305=")";_tag_fat(_tmp305,sizeof(char),2U);}));_tmp301[4]=_tmp70D;});Cyc_PP_cat(_tag_fat(_tmp301,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 20U: _LL33: _tmp28E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp28E;
s=({struct Cyc_PP_Doc*_tmp306[2U];({struct Cyc_PP_Doc*_tmp714=Cyc_PP_text(({const char*_tmp307="*";_tag_fat(_tmp307,sizeof(char),2U);}));_tmp306[0]=_tmp714;}),({struct Cyc_PP_Doc*_tmp713=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp306[1]=_tmp713;});Cyc_PP_cat(_tag_fat(_tmp306,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 21U: _LL35: _tmp28C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp28D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp28C;struct _fat_ptr*n=_tmp28D;
# 1241
s=({struct Cyc_PP_Doc*_tmp308[3U];({struct Cyc_PP_Doc*_tmp717=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp308[0]=_tmp717;}),({struct Cyc_PP_Doc*_tmp716=Cyc_PP_text(({const char*_tmp309=".";_tag_fat(_tmp309,sizeof(char),2U);}));_tmp308[1]=_tmp716;}),({struct Cyc_PP_Doc*_tmp715=Cyc_PP_textptr(n);_tmp308[2]=_tmp715;});Cyc_PP_cat(_tag_fat(_tmp308,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 22U: _LL37: _tmp28A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp28B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp28A;struct _fat_ptr*n=_tmp28B;
# 1243
s=({struct Cyc_PP_Doc*_tmp30A[3U];({struct Cyc_PP_Doc*_tmp71A=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp30A[0]=_tmp71A;}),({struct Cyc_PP_Doc*_tmp719=Cyc_PP_text(({const char*_tmp30B="->";_tag_fat(_tmp30B,sizeof(char),3U);}));_tmp30A[1]=_tmp719;}),({struct Cyc_PP_Doc*_tmp718=Cyc_PP_textptr(n);_tmp30A[2]=_tmp718;});Cyc_PP_cat(_tag_fat(_tmp30A,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 23U: _LL39: _tmp288=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp289=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp288;struct Cyc_Absyn_Exp*e2=_tmp289;
# 1245
s=({struct Cyc_PP_Doc*_tmp30C[4U];({struct Cyc_PP_Doc*_tmp71E=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp30C[0]=_tmp71E;}),({struct Cyc_PP_Doc*_tmp71D=Cyc_PP_text(({const char*_tmp30D="[";_tag_fat(_tmp30D,sizeof(char),2U);}));_tmp30C[1]=_tmp71D;}),({struct Cyc_PP_Doc*_tmp71C=Cyc_Absynpp_exp2doc(e2);_tmp30C[2]=_tmp71C;}),({struct Cyc_PP_Doc*_tmp71B=Cyc_PP_text(({const char*_tmp30E="]";_tag_fat(_tmp30E,sizeof(char),2U);}));_tmp30C[3]=_tmp71B;});Cyc_PP_cat(_tag_fat(_tmp30C,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 24U: _LL3B: _tmp287=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL3C: {struct Cyc_List_List*es=_tmp287;
# 1247
s=({struct Cyc_PP_Doc*_tmp30F[4U];({struct Cyc_PP_Doc*_tmp722=Cyc_Absynpp_dollar();_tmp30F[0]=_tmp722;}),({struct Cyc_PP_Doc*_tmp721=Cyc_PP_text(({const char*_tmp310="(";_tag_fat(_tmp310,sizeof(char),2U);}));_tmp30F[1]=_tmp721;}),({struct Cyc_PP_Doc*_tmp720=Cyc_Absynpp_exps2doc_prec(20,es);_tmp30F[2]=_tmp720;}),({struct Cyc_PP_Doc*_tmp71F=Cyc_PP_text(({const char*_tmp311=")";_tag_fat(_tmp311,sizeof(char),2U);}));_tmp30F[3]=_tmp71F;});Cyc_PP_cat(_tag_fat(_tmp30F,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 25U: _LL3D: _tmp285=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp286=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL3E: {struct _tuple8*vat=_tmp285;struct Cyc_List_List*des=_tmp286;
# 1249
s=({struct Cyc_PP_Doc*_tmp312[4U];({struct Cyc_PP_Doc*_tmp727=Cyc_PP_text(({const char*_tmp313="(";_tag_fat(_tmp313,sizeof(char),2U);}));_tmp312[0]=_tmp727;}),({struct Cyc_PP_Doc*_tmp726=Cyc_Absynpp_typ2doc((*vat).f3);_tmp312[1]=_tmp726;}),({struct Cyc_PP_Doc*_tmp725=Cyc_PP_text(({const char*_tmp314=")";_tag_fat(_tmp314,sizeof(char),2U);}));_tmp312[2]=_tmp725;}),({
struct Cyc_PP_Doc*_tmp724=({struct _fat_ptr _tmp723=({const char*_tmp315=",";_tag_fat(_tmp315,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp723,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});_tmp312[3]=_tmp724;});Cyc_PP_cat(_tag_fat(_tmp312,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 26U: _LL3F: _tmp284=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL40: {struct Cyc_List_List*des=_tmp284;
s=({struct _fat_ptr _tmp728=({const char*_tmp316=",";_tag_fat(_tmp316,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp728,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 27U: _LL41: _tmp281=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp282=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp283=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_LL42: {struct Cyc_Absyn_Vardecl*vd=_tmp281;struct Cyc_Absyn_Exp*e1=_tmp282;struct Cyc_Absyn_Exp*e2=_tmp283;
# 1254
s=({struct Cyc_PP_Doc*_tmp317[8U];({struct Cyc_PP_Doc*_tmp730=Cyc_Absynpp_lb();_tmp317[0]=_tmp730;}),({struct Cyc_PP_Doc*_tmp72F=Cyc_PP_text(({const char*_tmp318="for ";_tag_fat(_tmp318,sizeof(char),5U);}));_tmp317[1]=_tmp72F;}),({
struct Cyc_PP_Doc*_tmp72E=Cyc_PP_text(*(*vd->name).f2);_tmp317[2]=_tmp72E;}),({struct Cyc_PP_Doc*_tmp72D=Cyc_PP_text(({const char*_tmp319=" < ";_tag_fat(_tmp319,sizeof(char),4U);}));_tmp317[3]=_tmp72D;}),({struct Cyc_PP_Doc*_tmp72C=Cyc_Absynpp_exp2doc(e1);_tmp317[4]=_tmp72C;}),({struct Cyc_PP_Doc*_tmp72B=Cyc_PP_text(({const char*_tmp31A=" : ";_tag_fat(_tmp31A,sizeof(char),4U);}));_tmp317[5]=_tmp72B;}),({
struct Cyc_PP_Doc*_tmp72A=Cyc_Absynpp_exp2doc(e2);_tmp317[6]=_tmp72A;}),({struct Cyc_PP_Doc*_tmp729=Cyc_Absynpp_rb();_tmp317[7]=_tmp729;});Cyc_PP_cat(_tag_fat(_tmp317,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 28U: _LL43: _tmp27F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp280=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL44: {struct Cyc_Absyn_Exp*e=_tmp27F;void*t=_tmp280;
# 1259
s=({struct Cyc_PP_Doc*_tmp31B[7U];({struct Cyc_PP_Doc*_tmp737=Cyc_Absynpp_lb();_tmp31B[0]=_tmp737;}),({struct Cyc_PP_Doc*_tmp736=Cyc_PP_text(({const char*_tmp31C="for x ";_tag_fat(_tmp31C,sizeof(char),7U);}));_tmp31B[1]=_tmp736;}),({
struct Cyc_PP_Doc*_tmp735=Cyc_PP_text(({const char*_tmp31D=" < ";_tag_fat(_tmp31D,sizeof(char),4U);}));_tmp31B[2]=_tmp735;}),({
struct Cyc_PP_Doc*_tmp734=Cyc_Absynpp_exp2doc(e);_tmp31B[3]=_tmp734;}),({
struct Cyc_PP_Doc*_tmp733=Cyc_PP_text(({const char*_tmp31E=" : ";_tag_fat(_tmp31E,sizeof(char),4U);}));_tmp31B[4]=_tmp733;}),({
struct Cyc_PP_Doc*_tmp732=Cyc_Absynpp_typ2doc(t);_tmp31B[5]=_tmp732;}),({
struct Cyc_PP_Doc*_tmp731=Cyc_Absynpp_rb();_tmp31B[6]=_tmp731;});Cyc_PP_cat(_tag_fat(_tmp31B,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 29U: _LL45: _tmp27C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp27D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp27E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_LL46: {struct _tuple0*n=_tmp27C;struct Cyc_List_List*ts=_tmp27D;struct Cyc_List_List*des=_tmp27E;
# 1267
struct Cyc_List_List*_tmp31F=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des);struct Cyc_List_List*des_doc=_tmp31F;
s=({struct Cyc_PP_Doc*_tmp320[2U];({struct Cyc_PP_Doc*_tmp73B=Cyc_Absynpp_qvar2doc(n);_tmp320[0]=_tmp73B;}),({
struct Cyc_PP_Doc*_tmp73A=({struct _fat_ptr _tmp739=({const char*_tmp321=",";_tag_fat(_tmp321,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp739,
ts != 0?({struct Cyc_List_List*_tmp322=_cycalloc(sizeof(*_tmp322));({struct Cyc_PP_Doc*_tmp738=Cyc_Absynpp_tps2doc(ts);_tmp322->hd=_tmp738;}),_tmp322->tl=des_doc;_tmp322;}): des_doc);});
# 1269
_tmp320[1]=_tmp73A;});Cyc_PP_cat(_tag_fat(_tmp320,sizeof(struct Cyc_PP_Doc*),2U));});
# 1271
goto _LL0;}case 30U: _LL47: _tmp27B=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL48: {struct Cyc_List_List*des=_tmp27B;
s=({struct _fat_ptr _tmp73C=({const char*_tmp323=",";_tag_fat(_tmp323,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp73C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 31U: _LL49: _tmp279=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp27A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_LL4A: {struct Cyc_List_List*es=_tmp279;struct Cyc_Absyn_Datatypefield*tuf=_tmp27A;
# 1274
if(es == 0)
# 1276
s=Cyc_Absynpp_qvar2doc(tuf->name);else{
# 1278
s=({struct Cyc_PP_Doc*_tmp324[2U];({struct Cyc_PP_Doc*_tmp741=Cyc_Absynpp_qvar2doc(tuf->name);_tmp324[0]=_tmp741;}),({
struct Cyc_PP_Doc*_tmp740=({struct _fat_ptr _tmp73F=({const char*_tmp325="(";_tag_fat(_tmp325,sizeof(char),2U);});struct _fat_ptr _tmp73E=({const char*_tmp326=")";_tag_fat(_tmp326,sizeof(char),2U);});struct _fat_ptr _tmp73D=({const char*_tmp327=",";_tag_fat(_tmp327,sizeof(char),2U);});Cyc_PP_egroup(_tmp73F,_tmp73E,_tmp73D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,es));});_tmp324[1]=_tmp740;});Cyc_PP_cat(_tag_fat(_tmp324,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;}case 32U: _LL4B: _tmp278=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL4C: {struct Cyc_Absyn_Enumfield*ef=_tmp278;
_tmp277=ef;goto _LL4E;}case 33U: _LL4D: _tmp277=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL4E: {struct Cyc_Absyn_Enumfield*ef=_tmp277;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 34U: _LL4F: _tmp272=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26C)->f1).is_calloc;_tmp273=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26C)->f1).rgn;_tmp274=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26C)->f1).elt_type;_tmp275=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26C)->f1).num_elts;_tmp276=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26C)->f1).inline_call;_LL50: {int is_calloc=_tmp272;struct Cyc_Absyn_Exp*rgnopt=_tmp273;void**topt=_tmp274;struct Cyc_Absyn_Exp*e=_tmp275;int inline_call=_tmp276;
# 1286
if(is_calloc){
# 1288
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U);
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp328[5U];({struct Cyc_PP_Doc*_tmp746=Cyc_PP_text(({const char*_tmp329="calloc(";_tag_fat(_tmp329,sizeof(char),8U);}));_tmp328[0]=_tmp746;}),({struct Cyc_PP_Doc*_tmp745=Cyc_Absynpp_exp2doc(e);_tmp328[1]=_tmp745;}),({struct Cyc_PP_Doc*_tmp744=Cyc_PP_text(({const char*_tmp32A=",";_tag_fat(_tmp32A,sizeof(char),2U);}));_tmp328[2]=_tmp744;}),({struct Cyc_PP_Doc*_tmp743=Cyc_Absynpp_exp2doc(st);_tmp328[3]=_tmp743;}),({struct Cyc_PP_Doc*_tmp742=Cyc_PP_text(({const char*_tmp32B=")";_tag_fat(_tmp32B,sizeof(char),2U);}));_tmp328[4]=_tmp742;});Cyc_PP_cat(_tag_fat(_tmp328,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1292
s=({struct Cyc_PP_Doc*_tmp32C[7U];({struct Cyc_PP_Doc*_tmp74D=Cyc_PP_text(({const char*_tmp32D="rcalloc(";_tag_fat(_tmp32D,sizeof(char),9U);}));_tmp32C[0]=_tmp74D;}),({struct Cyc_PP_Doc*_tmp74C=Cyc_Absynpp_exp2doc(rgnopt);_tmp32C[1]=_tmp74C;}),({struct Cyc_PP_Doc*_tmp74B=Cyc_PP_text(({const char*_tmp32E=",";_tag_fat(_tmp32E,sizeof(char),2U);}));_tmp32C[2]=_tmp74B;}),({
struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_exp2doc(e);_tmp32C[3]=_tmp74A;}),({struct Cyc_PP_Doc*_tmp749=Cyc_PP_text(({const char*_tmp32F=",";_tag_fat(_tmp32F,sizeof(char),2U);}));_tmp32C[4]=_tmp749;}),({struct Cyc_PP_Doc*_tmp748=Cyc_Absynpp_exp2doc(st);_tmp32C[5]=_tmp748;}),({struct Cyc_PP_Doc*_tmp747=Cyc_PP_text(({const char*_tmp330=")";_tag_fat(_tmp330,sizeof(char),2U);}));_tmp32C[6]=_tmp747;});Cyc_PP_cat(_tag_fat(_tmp32C,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1295
struct Cyc_Absyn_Exp*new_e;
# 1297
if(topt == 0)
new_e=e;else{
# 1300
new_e=({struct Cyc_Absyn_Exp*_tmp74E=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp74E,e,0U);});}
# 1302
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp331[3U];({struct Cyc_PP_Doc*_tmp751=Cyc_PP_text(({const char*_tmp332="malloc(";_tag_fat(_tmp332,sizeof(char),8U);}));_tmp331[0]=_tmp751;}),({struct Cyc_PP_Doc*_tmp750=Cyc_Absynpp_exp2doc(new_e);_tmp331[1]=_tmp750;}),({struct Cyc_PP_Doc*_tmp74F=Cyc_PP_text(({const char*_tmp333=")";_tag_fat(_tmp333,sizeof(char),2U);}));_tmp331[2]=_tmp74F;});Cyc_PP_cat(_tag_fat(_tmp331,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1305
if(inline_call)
s=({struct Cyc_PP_Doc*_tmp334[5U];({struct Cyc_PP_Doc*_tmp756=Cyc_PP_text(({const char*_tmp335="rmalloc_inline(";_tag_fat(_tmp335,sizeof(char),16U);}));_tmp334[0]=_tmp756;}),({struct Cyc_PP_Doc*_tmp755=Cyc_Absynpp_exp2doc(rgnopt);_tmp334[1]=_tmp755;}),({struct Cyc_PP_Doc*_tmp754=Cyc_PP_text(({const char*_tmp336=",";_tag_fat(_tmp336,sizeof(char),2U);}));_tmp334[2]=_tmp754;}),({
struct Cyc_PP_Doc*_tmp753=Cyc_Absynpp_exp2doc(new_e);_tmp334[3]=_tmp753;}),({struct Cyc_PP_Doc*_tmp752=Cyc_PP_text(({const char*_tmp337=")";_tag_fat(_tmp337,sizeof(char),2U);}));_tmp334[4]=_tmp752;});Cyc_PP_cat(_tag_fat(_tmp334,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1309
s=({struct Cyc_PP_Doc*_tmp338[5U];({struct Cyc_PP_Doc*_tmp75B=Cyc_PP_text(({const char*_tmp339="rmalloc(";_tag_fat(_tmp339,sizeof(char),9U);}));_tmp338[0]=_tmp75B;}),({struct Cyc_PP_Doc*_tmp75A=Cyc_Absynpp_exp2doc(rgnopt);_tmp338[1]=_tmp75A;}),({struct Cyc_PP_Doc*_tmp759=Cyc_PP_text(({const char*_tmp33A=",";_tag_fat(_tmp33A,sizeof(char),2U);}));_tmp338[2]=_tmp759;}),({
struct Cyc_PP_Doc*_tmp758=Cyc_Absynpp_exp2doc(new_e);_tmp338[3]=_tmp758;}),({struct Cyc_PP_Doc*_tmp757=Cyc_PP_text(({const char*_tmp33B=")";_tag_fat(_tmp33B,sizeof(char),2U);}));_tmp338[4]=_tmp757;});Cyc_PP_cat(_tag_fat(_tmp338,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1313
goto _LL0;}case 35U: _LL51: _tmp270=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp271=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp270;struct Cyc_Absyn_Exp*e2=_tmp271;
# 1315
s=({struct Cyc_PP_Doc*_tmp33C[3U];({struct Cyc_PP_Doc*_tmp75E=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp33C[0]=_tmp75E;}),({struct Cyc_PP_Doc*_tmp75D=Cyc_PP_text(({const char*_tmp33D=" :=: ";_tag_fat(_tmp33D,sizeof(char),6U);}));_tmp33C[1]=_tmp75D;}),({struct Cyc_PP_Doc*_tmp75C=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp33C[2]=_tmp75C;});Cyc_PP_cat(_tag_fat(_tmp33C,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 36U: _LL53: _tmp26E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp26F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL54: {struct Cyc_Core_Opt*n=_tmp26E;struct Cyc_List_List*des=_tmp26F;
# 1318
s=({struct _fat_ptr _tmp75F=({const char*_tmp33E=",";_tag_fat(_tmp33E,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp75F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _LL55: _tmp26D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL56: {struct Cyc_Absyn_Stmt*s2=_tmp26D;
# 1321
s=({struct Cyc_PP_Doc*_tmp33F[7U];({struct Cyc_PP_Doc*_tmp766=Cyc_PP_text(({const char*_tmp340="(";_tag_fat(_tmp340,sizeof(char),2U);}));_tmp33F[0]=_tmp766;}),({struct Cyc_PP_Doc*_tmp765=Cyc_Absynpp_lb();_tmp33F[1]=_tmp765;}),({struct Cyc_PP_Doc*_tmp764=Cyc_PP_blank_doc();_tmp33F[2]=_tmp764;}),({
struct Cyc_PP_Doc*_tmp763=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_tmp33F[3]=_tmp763;}),({
struct Cyc_PP_Doc*_tmp762=Cyc_PP_blank_doc();_tmp33F[4]=_tmp762;}),({struct Cyc_PP_Doc*_tmp761=Cyc_Absynpp_rb();_tmp33F[5]=_tmp761;}),({struct Cyc_PP_Doc*_tmp760=Cyc_PP_text(({const char*_tmp341=")";_tag_fat(_tmp341,sizeof(char),2U);}));_tmp33F[6]=_tmp760;});Cyc_PP_cat(_tag_fat(_tmp33F,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}}_LL0:;}
# 1326
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp342[3U];({struct Cyc_PP_Doc*_tmp768=Cyc_PP_text(({const char*_tmp343="(";_tag_fat(_tmp343,sizeof(char),2U);}));_tmp342[0]=_tmp768;}),_tmp342[1]=s,({struct Cyc_PP_Doc*_tmp767=Cyc_PP_text(({const char*_tmp344=")";_tag_fat(_tmp344,sizeof(char),2U);}));_tmp342[2]=_tmp767;});Cyc_PP_cat(_tag_fat(_tmp342,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1331
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp345=d;struct _fat_ptr*_tmp346;struct Cyc_Absyn_Exp*_tmp347;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp345)->tag == 0U){_LL1: _tmp347=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp345)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp347;
return({struct Cyc_PP_Doc*_tmp348[3U];({struct Cyc_PP_Doc*_tmp76B=Cyc_PP_text(({const char*_tmp349=".[";_tag_fat(_tmp349,sizeof(char),3U);}));_tmp348[0]=_tmp76B;}),({struct Cyc_PP_Doc*_tmp76A=Cyc_Absynpp_exp2doc(e);_tmp348[1]=_tmp76A;}),({struct Cyc_PP_Doc*_tmp769=Cyc_PP_text(({const char*_tmp34A="]";_tag_fat(_tmp34A,sizeof(char),2U);}));_tmp348[2]=_tmp769;});Cyc_PP_cat(_tag_fat(_tmp348,sizeof(struct Cyc_PP_Doc*),3U));});}}else{_LL3: _tmp346=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp345)->f1;_LL4: {struct _fat_ptr*v=_tmp346;
return({struct Cyc_PP_Doc*_tmp34B[2U];({struct Cyc_PP_Doc*_tmp76D=Cyc_PP_text(({const char*_tmp34C=".";_tag_fat(_tmp34C,sizeof(char),2U);}));_tmp34B[0]=_tmp76D;}),({struct Cyc_PP_Doc*_tmp76C=Cyc_PP_textptr(v);_tmp34B[1]=_tmp76C;});Cyc_PP_cat(_tag_fat(_tmp34B,sizeof(struct Cyc_PP_Doc*),2U));});}}_LL0:;}
# 1338
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp34D[2U];({struct Cyc_PP_Doc*_tmp772=({struct _fat_ptr _tmp771=({const char*_tmp34E="";_tag_fat(_tmp34E,sizeof(char),1U);});struct _fat_ptr _tmp770=({const char*_tmp34F="=";_tag_fat(_tmp34F,sizeof(char),2U);});struct _fat_ptr _tmp76F=({const char*_tmp350="=";_tag_fat(_tmp350,sizeof(char),2U);});Cyc_PP_egroup(_tmp771,_tmp770,_tmp76F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp34D[0]=_tmp772;}),({
struct Cyc_PP_Doc*_tmp76E=Cyc_Absynpp_exp2doc((*de).f2);_tmp34D[1]=_tmp76E;});Cyc_PP_cat(_tag_fat(_tmp34D,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1344
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _tmp775=({const char*_tmp351="";_tag_fat(_tmp351,sizeof(char),1U);});struct _fat_ptr _tmp774=({const char*_tmp352="";_tag_fat(_tmp352,sizeof(char),1U);});struct _fat_ptr _tmp773=({const char*_tmp353=",";_tag_fat(_tmp353,sizeof(char),2U);});Cyc_PP_group(_tmp775,_tmp774,_tmp773,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1348
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_tmp368=({unsigned _tmp367=28U + 1U;char*_tmp366=_cycalloc_atomic(_check_times(_tmp367,sizeof(char)));({{unsigned _tmp5CD=28U;unsigned i;for(i=0;i < _tmp5CD;++ i){_tmp366[i]='z';}_tmp366[_tmp5CD]=0;}0;});_tmp366;});_tag_fat(_tmp368,sizeof(char),29U);});
({struct _fat_ptr _tmp354=_fat_ptr_plus(x,sizeof(char),27);char _tmp355=*((char*)_check_fat_subscript(_tmp354,sizeof(char),0U));char _tmp356='\000';if(_get_fat_size(_tmp354,sizeof(char))== 1U &&(_tmp355 == 0 && _tmp356 != 0))_throw_arraybounds();*((char*)_tmp354.curr)=_tmp356;});
({struct _fat_ptr _tmp357=_fat_ptr_plus(x,sizeof(char),26);char _tmp358=*((char*)_check_fat_subscript(_tmp357,sizeof(char),0U));char _tmp359='L';if(_get_fat_size(_tmp357,sizeof(char))== 1U &&(_tmp358 == 0 && _tmp359 != 0))_throw_arraybounds();*((char*)_tmp357.curr)=_tmp359;});
({struct _fat_ptr _tmp35A=_fat_ptr_plus(x,sizeof(char),25);char _tmp35B=*((char*)_check_fat_subscript(_tmp35A,sizeof(char),0U));char _tmp35C='L';if(_get_fat_size(_tmp35A,sizeof(char))== 1U &&(_tmp35B == 0 && _tmp35C != 0))_throw_arraybounds();*((char*)_tmp35A.curr)=_tmp35C;});
({struct _fat_ptr _tmp35D=_fat_ptr_plus(x,sizeof(char),24);char _tmp35E=*((char*)_check_fat_subscript(_tmp35D,sizeof(char),0U));char _tmp35F='U';if(_get_fat_size(_tmp35D,sizeof(char))== 1U &&(_tmp35E == 0 && _tmp35F != 0))_throw_arraybounds();*((char*)_tmp35D.curr)=_tmp35F;});
({struct _fat_ptr _tmp360=_fat_ptr_plus(x,sizeof(char),23);char _tmp361=*((char*)_check_fat_subscript(_tmp360,sizeof(char),0U));char _tmp362='0';if(_get_fat_size(_tmp360,sizeof(char))== 1U &&(_tmp361 == 0 && _tmp362 != 0))_throw_arraybounds();*((char*)_tmp360.curr)=_tmp362;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _fat_ptr _tmp363=_fat_ptr_plus(x,sizeof(char),index);char _tmp364=*((char*)_check_fat_subscript(_tmp363,sizeof(char),0U));char _tmp365=c;if(_get_fat_size(_tmp363,sizeof(char))== 1U &&(_tmp364 == 0 && _tmp365 != 0))_throw_arraybounds();*((char*)_tmp363.curr)=_tmp365;});
i=i / (unsigned long long)10;
-- index;}
# 1362
return(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1366
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp369=c;struct _fat_ptr _tmp36A;struct _fat_ptr _tmp36B;struct _fat_ptr _tmp36C;long long _tmp36E;enum Cyc_Absyn_Sign _tmp36D;int _tmp370;enum Cyc_Absyn_Sign _tmp36F;short _tmp372;enum Cyc_Absyn_Sign _tmp371;struct _fat_ptr _tmp373;char _tmp375;enum Cyc_Absyn_Sign _tmp374;switch((_tmp369.String_c).tag){case 2U: _LL1: _tmp374=((_tmp369.Char_c).val).f1;_tmp375=((_tmp369.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp374;char ch=_tmp375;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp378=({struct Cyc_String_pa_PrintArg_struct _tmp5CE;_tmp5CE.tag=0U,({struct _fat_ptr _tmp776=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp5CE.f1=_tmp776;});_tmp5CE;});void*_tmp376[1U];_tmp376[0]=& _tmp378;({struct _fat_ptr _tmp777=({const char*_tmp377="'%s'";_tag_fat(_tmp377,sizeof(char),5U);});Cyc_aprintf(_tmp777,_tag_fat(_tmp376,sizeof(void*),1U));});}));}case 3U: _LL3: _tmp373=(_tmp369.Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp373;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37B=({struct Cyc_String_pa_PrintArg_struct _tmp5CF;_tmp5CF.tag=0U,_tmp5CF.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5CF;});void*_tmp379[1U];_tmp379[0]=& _tmp37B;({struct _fat_ptr _tmp778=({const char*_tmp37A="L'%s'";_tag_fat(_tmp37A,sizeof(char),6U);});Cyc_aprintf(_tmp778,_tag_fat(_tmp379,sizeof(void*),1U));});}));}case 4U: _LL5: _tmp371=((_tmp369.Short_c).val).f1;_tmp372=((_tmp369.Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp371;short s=_tmp372;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp37E=({struct Cyc_Int_pa_PrintArg_struct _tmp5D0;_tmp5D0.tag=1U,_tmp5D0.f1=(unsigned long)((int)s);_tmp5D0;});void*_tmp37C[1U];_tmp37C[0]=& _tmp37E;({struct _fat_ptr _tmp779=({const char*_tmp37D="%d";_tag_fat(_tmp37D,sizeof(char),3U);});Cyc_aprintf(_tmp779,_tag_fat(_tmp37C,sizeof(void*),1U));});}));}case 5U: _LL7: _tmp36F=((_tmp369.Int_c).val).f1;_tmp370=((_tmp369.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign sn=_tmp36F;int i=_tmp370;
# 1372
if((int)sn == (int)1U)return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp381=({struct Cyc_Int_pa_PrintArg_struct _tmp5D1;_tmp5D1.tag=1U,_tmp5D1.f1=(unsigned)i;_tmp5D1;});void*_tmp37F[1U];_tmp37F[0]=& _tmp381;({struct _fat_ptr _tmp77A=({const char*_tmp380="%uU";_tag_fat(_tmp380,sizeof(char),4U);});Cyc_aprintf(_tmp77A,_tag_fat(_tmp37F,sizeof(void*),1U));});}));else{
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp384=({struct Cyc_Int_pa_PrintArg_struct _tmp5D2;_tmp5D2.tag=1U,_tmp5D2.f1=(unsigned long)i;_tmp5D2;});void*_tmp382[1U];_tmp382[0]=& _tmp384;({struct _fat_ptr _tmp77B=({const char*_tmp383="%d";_tag_fat(_tmp383,sizeof(char),3U);});Cyc_aprintf(_tmp77B,_tag_fat(_tmp382,sizeof(void*),1U));});}));}}case 6U: _LL9: _tmp36D=((_tmp369.LongLong_c).val).f1;_tmp36E=((_tmp369.LongLong_c).val).f2;_LLA: {enum Cyc_Absyn_Sign sg=_tmp36D;long long i=_tmp36E;
# 1376
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7U: _LLB: _tmp36C=((_tmp369.Float_c).val).f1;_LLC: {struct _fat_ptr x=_tmp36C;
return Cyc_PP_text(x);}case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp385="NULL";_tag_fat(_tmp385,sizeof(char),5U);}));case 8U: _LLF: _tmp36B=(_tmp369.String_c).val;_LL10: {struct _fat_ptr s=_tmp36B;
return({struct Cyc_PP_Doc*_tmp386[3U];({struct Cyc_PP_Doc*_tmp77E=Cyc_PP_text(({const char*_tmp387="\"";_tag_fat(_tmp387,sizeof(char),2U);}));_tmp386[0]=_tmp77E;}),({struct Cyc_PP_Doc*_tmp77D=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_tmp386[1]=_tmp77D;}),({struct Cyc_PP_Doc*_tmp77C=Cyc_PP_text(({const char*_tmp388="\"";_tag_fat(_tmp388,sizeof(char),2U);}));_tmp386[2]=_tmp77C;});Cyc_PP_cat(_tag_fat(_tmp386,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL11: _tmp36A=(_tmp369.Wstring_c).val;_LL12: {struct _fat_ptr s=_tmp36A;
return({struct Cyc_PP_Doc*_tmp389[3U];({struct Cyc_PP_Doc*_tmp781=Cyc_PP_text(({const char*_tmp38A="L\"";_tag_fat(_tmp38A,sizeof(char),3U);}));_tmp389[0]=_tmp781;}),({struct Cyc_PP_Doc*_tmp780=Cyc_PP_text(s);_tmp389[1]=_tmp780;}),({struct Cyc_PP_Doc*_tmp77F=Cyc_PP_text(({const char*_tmp38B="\"";_tag_fat(_tmp38B,sizeof(char),2U);}));_tmp389[2]=_tmp77F;});Cyc_PP_cat(_tag_fat(_tmp389,sizeof(struct Cyc_PP_Doc*),3U));});}}_LL0:;}
# 1384
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp784=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp38E=({struct Cyc_String_pa_PrintArg_struct _tmp5D3;_tmp5D3.tag=0U,({struct _fat_ptr _tmp782=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5D3.f1=_tmp782;});_tmp5D3;});void*_tmp38C[1U];_tmp38C[0]=& _tmp38E;({struct _fat_ptr _tmp783=({const char*_tmp38D="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_tmp38D,sizeof(char),47U);});Cyc_aprintf(_tmp783,_tag_fat(_tmp38C,sizeof(void*),1U));});});_tmp38F->f1=_tmp784;});_tmp38F;}));
# 1390
return({struct Cyc_PP_Doc*_tmp390[3U];({struct Cyc_PP_Doc*_tmp787=Cyc_PP_text(({const char*_tmp391="numelts(";_tag_fat(_tmp391,sizeof(char),9U);}));_tmp390[0]=_tmp787;}),({struct Cyc_PP_Doc*_tmp786=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp390[1]=_tmp786;}),({struct Cyc_PP_Doc*_tmp785=Cyc_PP_text(({const char*_tmp392=")";_tag_fat(_tmp392,sizeof(char),2U);}));_tmp390[2]=_tmp785;});Cyc_PP_cat(_tag_fat(_tmp390,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1392
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp78A=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp395=({struct Cyc_String_pa_PrintArg_struct _tmp5D4;_tmp5D4.tag=0U,({struct _fat_ptr _tmp788=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5D4.f1=_tmp788;});_tmp5D4;});void*_tmp393[1U];_tmp393[0]=& _tmp395;({struct _fat_ptr _tmp789=({const char*_tmp394="Absynpp::primapp2doc: %s with no args";_tag_fat(_tmp394,sizeof(char),38U);});Cyc_aprintf(_tmp789,_tag_fat(_tmp393,sizeof(void*),1U));});});_tmp396->f1=_tmp78A;});_tmp396;}));else{
# 1396
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp397[3U];_tmp397[0]=ps,({struct Cyc_PP_Doc*_tmp78B=Cyc_PP_text(({const char*_tmp398=" ";_tag_fat(_tmp398,sizeof(char),2U);}));_tmp397[1]=_tmp78B;}),_tmp397[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_tmp397,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp78E=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39B=({struct Cyc_String_pa_PrintArg_struct _tmp5D5;_tmp5D5.tag=0U,({struct _fat_ptr _tmp78C=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5D5.f1=_tmp78C;});_tmp5D5;});void*_tmp399[1U];_tmp399[0]=& _tmp39B;({struct _fat_ptr _tmp78D=({const char*_tmp39A="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_tmp39A,sizeof(char),47U);});Cyc_aprintf(_tmp78D,_tag_fat(_tmp399,sizeof(void*),1U));});});_tmp39C->f1=_tmp78E;});_tmp39C;}));else{
# 1402
return({struct Cyc_PP_Doc*_tmp39D[5U];_tmp39D[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp790=Cyc_PP_text(({const char*_tmp39E=" ";_tag_fat(_tmp39E,sizeof(char),2U);}));_tmp39D[1]=_tmp790;}),_tmp39D[2]=ps,({struct Cyc_PP_Doc*_tmp78F=Cyc_PP_text(({const char*_tmp39F=" ";_tag_fat(_tmp39F,sizeof(char),2U);}));_tmp39D[3]=_tmp78F;}),_tmp39D[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_tmp39D,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1406
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp3A0=p;switch(_tmp3A0){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp3A1="+";_tag_fat(_tmp3A1,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp3A2="*";_tag_fat(_tmp3A2,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp3A3="-";_tag_fat(_tmp3A3,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp3A4="/";_tag_fat(_tmp3A4,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp3A5="\\%";_tag_fat(_tmp3A5,sizeof(char),3U);}):({const char*_tmp3A6="%";_tag_fat(_tmp3A6,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp3A7="==";_tag_fat(_tmp3A7,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp3A8="!=";_tag_fat(_tmp3A8,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp3A9=">";_tag_fat(_tmp3A9,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp3AA="<";_tag_fat(_tmp3AA,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp3AB=">=";_tag_fat(_tmp3AB,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp3AC="<=";_tag_fat(_tmp3AC,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp3AD="!";_tag_fat(_tmp3AD,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp3AE="~";_tag_fat(_tmp3AE,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp3AF="&";_tag_fat(_tmp3AF,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp3B0="|";_tag_fat(_tmp3B0,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp3B1="^";_tag_fat(_tmp3B1,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp3B2="<<";_tag_fat(_tmp3B2,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp3B3=">>";_tag_fat(_tmp3B3,sizeof(char),3U);});case Cyc_Absyn_Numelts: _LL25: _LL26:
 return({const char*_tmp3B4="numelts";_tag_fat(_tmp3B4,sizeof(char),8U);});default: _LL27: _LL28:
 return({const char*_tmp3B5="?";_tag_fat(_tmp3B5,sizeof(char),2U);});}_LL0:;}
# 1431
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1435
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp3B6=s->r;void*_stmttmp17=_tmp3B6;void*_tmp3B7=_stmttmp17;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B7)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1450 "/tmp/cyclone/src/absynpp.cyc"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_tmp3B8=d->r;void*_stmttmp18=_tmp3B8;void*_tmp3B9=_stmttmp18;struct Cyc_Absyn_Vardecl*_tmp3BA;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3B9)->tag == 0U){_LL1: _tmp3BA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3B9)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3BA;
# 1453
if(((int(*)(int(*compare)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*l,struct _tuple0*x))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple14 _tmp5D6;_tmp5D6.f1=1,({struct Cyc_List_List*_tmp791=({struct Cyc_List_List*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->hd=vd->name,_tmp3BB->tl=0;_tmp3BB;});_tmp5D6.f2=_tmp791;});_tmp5D6;});else{
# 1456
return({struct _tuple14 _tmp5D7;_tmp5D7.f1=0,({struct Cyc_List_List*_tmp792=({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->hd=vd->name,_tmp3BC->tl=varsinblock;_tmp3BC;});_tmp5D7.f2=_tmp792;});_tmp5D7;});}}}else{_LL3: _LL4:
# 1458
 return({struct _tuple14 _tmp5D8;_tmp5D8.f1=0,_tmp5D8.f2=varsinblock;_tmp5D8;});}_LL0:;}
# 1462
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_tmp3BD[8U];({struct Cyc_PP_Doc*_tmp79A=Cyc_PP_text(({const char*_tmp3BE="(";_tag_fat(_tmp3BE,sizeof(char),2U);}));_tmp3BD[0]=_tmp79A;}),({struct Cyc_PP_Doc*_tmp799=Cyc_Absynpp_lb();_tmp3BD[1]=_tmp799;}),({struct Cyc_PP_Doc*_tmp798=Cyc_PP_blank_doc();_tmp3BD[2]=_tmp798;}),({struct Cyc_PP_Doc*_tmp797=Cyc_PP_nest(2,d);_tmp3BD[3]=_tmp797;}),({struct Cyc_PP_Doc*_tmp796=Cyc_PP_line_doc();_tmp3BD[4]=_tmp796;}),({struct Cyc_PP_Doc*_tmp795=Cyc_Absynpp_rb();_tmp3BD[5]=_tmp795;}),({
struct Cyc_PP_Doc*_tmp794=Cyc_PP_text(({const char*_tmp3BF=");";_tag_fat(_tmp3BF,sizeof(char),3U);}));_tmp3BD[6]=_tmp794;}),({struct Cyc_PP_Doc*_tmp793=Cyc_PP_line_doc();_tmp3BD[7]=_tmp793;});Cyc_PP_cat(_tag_fat(_tmp3BD,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1467
return({struct Cyc_PP_Doc*_tmp3C0[6U];({struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_lb();_tmp3C0[0]=_tmp7A0;}),({struct Cyc_PP_Doc*_tmp79F=Cyc_PP_blank_doc();_tmp3C0[1]=_tmp79F;}),({struct Cyc_PP_Doc*_tmp79E=Cyc_PP_nest(2,d);_tmp3C0[2]=_tmp79E;}),({struct Cyc_PP_Doc*_tmp79D=Cyc_PP_line_doc();_tmp3C0[3]=_tmp79D;}),({struct Cyc_PP_Doc*_tmp79C=Cyc_Absynpp_rb();_tmp3C0[4]=_tmp79C;}),({struct Cyc_PP_Doc*_tmp79B=Cyc_PP_line_doc();_tmp3C0[5]=_tmp79B;});Cyc_PP_cat(_tag_fat(_tmp3C0,sizeof(struct Cyc_PP_Doc*),6U));});}}
# 1470
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_tmp3C1=st->r;void*_stmttmp19=_tmp3C1;void*_tmp3C2=_stmttmp19;struct Cyc_List_List*_tmp3C4;struct Cyc_Absyn_Stmt*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_Absyn_Stmt*_tmp3C5;struct Cyc_Absyn_Stmt*_tmp3C8;struct _fat_ptr*_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3CA;struct Cyc_Absyn_Decl*_tmp3C9;struct Cyc_List_List*_tmp3CB;struct Cyc_List_List*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CC;struct Cyc_Absyn_Stmt*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3CE;struct _fat_ptr*_tmp3D2;struct Cyc_Absyn_Stmt*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D3;struct Cyc_Absyn_Stmt*_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Stmt*_tmp3DA;struct Cyc_Absyn_Stmt*_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DB;switch(*((int*)_tmp3C2)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3DC=";";_tag_fat(_tmp3DC,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp3DB=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp3DB;
s=({struct Cyc_PP_Doc*_tmp3DD[2U];({struct Cyc_PP_Doc*_tmp7A2=Cyc_Absynpp_exp2doc(e);_tmp3DD[0]=_tmp7A2;}),({struct Cyc_PP_Doc*_tmp7A1=Cyc_PP_text(({const char*_tmp3DE=";";_tag_fat(_tmp3DE,sizeof(char),2U);}));_tmp3DD[1]=_tmp7A1;});Cyc_PP_cat(_tag_fat(_tmp3DD,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 2U: _LL5: _tmp3D9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_tmp3DA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp3D9;struct Cyc_Absyn_Stmt*s2=_tmp3DA;
# 1476
if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp3DF[2U];({struct Cyc_PP_Doc*_tmp7A6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3DF[0]=_tmp7A6;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_tmp7A5=({int _tmp7A4=stmtexp;Cyc_Absynpp_block(_tmp7A4,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp3DF[1]=_tmp7A5;}):({
struct Cyc_PP_Doc*_tmp7A3=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp3DF[1]=_tmp7A3;});Cyc_PP_cat(_tag_fat(_tmp3DF,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(Cyc_Absynpp_is_declaration(s2))
s=({struct Cyc_PP_Doc*_tmp3E0[3U];({struct Cyc_PP_Doc*_tmp7AA=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp3E0[0]=_tmp7AA;}),({
struct Cyc_PP_Doc*_tmp7A9=Cyc_PP_line_doc();_tmp3E0[1]=_tmp7A9;}),({
struct Cyc_PP_Doc*_tmp7A8=({int _tmp7A7=stmtexp;Cyc_Absynpp_block(_tmp7A7,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp3E0[2]=_tmp7A8;});Cyc_PP_cat(_tag_fat(_tmp3E0,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1487
s=({struct Cyc_PP_Doc*_tmp3E1[3U];({struct Cyc_PP_Doc*_tmp7AD=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp3E1[0]=_tmp7AD;}),({struct Cyc_PP_Doc*_tmp7AC=Cyc_PP_line_doc();_tmp3E1[1]=_tmp7AC;}),({
struct Cyc_PP_Doc*_tmp7AB=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp3E1[2]=_tmp7AB;});Cyc_PP_cat(_tag_fat(_tmp3E1,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1491
s=({struct Cyc_PP_Doc*_tmp3E2[3U];({struct Cyc_PP_Doc*_tmp7B0=Cyc_Absynpp_stmt2doc(s1,0,0);_tmp3E2[0]=_tmp7B0;}),({struct Cyc_PP_Doc*_tmp7AF=Cyc_PP_line_doc();_tmp3E2[1]=_tmp7AF;}),({struct Cyc_PP_Doc*_tmp7AE=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_tmp3E2[2]=_tmp7AE;});Cyc_PP_cat(_tag_fat(_tmp3E2,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 3U: _LL7: _tmp3D8=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp3D8;
# 1494
if(eopt == 0)
s=Cyc_PP_text(({const char*_tmp3E3="return;";_tag_fat(_tmp3E3,sizeof(char),8U);}));else{
# 1497
s=({struct Cyc_PP_Doc*_tmp3E4[3U];({struct Cyc_PP_Doc*_tmp7B4=Cyc_PP_text(({const char*_tmp3E5="return ";_tag_fat(_tmp3E5,sizeof(char),8U);}));_tmp3E4[0]=_tmp7B4;}),
eopt == 0?({struct Cyc_PP_Doc*_tmp7B3=Cyc_PP_nil_doc();_tmp3E4[1]=_tmp7B3;}):({struct Cyc_PP_Doc*_tmp7B2=Cyc_Absynpp_exp2doc(eopt);_tmp3E4[1]=_tmp7B2;}),({
struct Cyc_PP_Doc*_tmp7B1=Cyc_PP_text(({const char*_tmp3E6=";";_tag_fat(_tmp3E6,sizeof(char),2U);}));_tmp3E4[2]=_tmp7B1;});Cyc_PP_cat(_tag_fat(_tmp3E4,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 4U: _LL9: _tmp3D5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_tmp3D6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2;_tmp3D7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp3D5;struct Cyc_Absyn_Stmt*s1=_tmp3D6;struct Cyc_Absyn_Stmt*s2=_tmp3D7;
# 1502
int print_else;
{void*_tmp3E7=s2->r;void*_stmttmp1A=_tmp3E7;void*_tmp3E8=_stmttmp1A;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3E8)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1507
s=({struct Cyc_PP_Doc*_tmp3E9[5U];({struct Cyc_PP_Doc*_tmp7BD=Cyc_PP_text(({const char*_tmp3EA="if (";_tag_fat(_tmp3EA,sizeof(char),5U);}));_tmp3E9[0]=_tmp7BD;}),({
struct Cyc_PP_Doc*_tmp7BC=Cyc_Absynpp_exp2doc(e);_tmp3E9[1]=_tmp7BC;}),({
struct Cyc_PP_Doc*_tmp7BB=Cyc_PP_text(({const char*_tmp3EB=") ";_tag_fat(_tmp3EB,sizeof(char),3U);}));_tmp3E9[2]=_tmp7BB;}),({
struct Cyc_PP_Doc*_tmp7BA=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3E9[3]=_tmp7BA;}),
print_else?({
struct Cyc_PP_Doc*_tmp7B9=({struct Cyc_PP_Doc*_tmp3EC[3U];({struct Cyc_PP_Doc*_tmp7B8=Cyc_PP_line_doc();_tmp3EC[0]=_tmp7B8;}),({
struct Cyc_PP_Doc*_tmp7B7=Cyc_PP_text(({const char*_tmp3ED="else ";_tag_fat(_tmp3ED,sizeof(char),6U);}));_tmp3EC[1]=_tmp7B7;}),({
struct Cyc_PP_Doc*_tmp7B6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_tmp3EC[2]=_tmp7B6;});Cyc_PP_cat(_tag_fat(_tmp3EC,sizeof(struct Cyc_PP_Doc*),3U));});
# 1512
_tmp3E9[4]=_tmp7B9;}):({
# 1515
struct Cyc_PP_Doc*_tmp7B5=Cyc_PP_nil_doc();_tmp3E9[4]=_tmp7B5;});Cyc_PP_cat(_tag_fat(_tmp3E9,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp3D3=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1).f1;_tmp3D4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp3D3;struct Cyc_Absyn_Stmt*s1=_tmp3D4;
# 1518
s=({struct Cyc_PP_Doc*_tmp3EE[4U];({struct Cyc_PP_Doc*_tmp7C1=Cyc_PP_text(({const char*_tmp3EF="while (";_tag_fat(_tmp3EF,sizeof(char),8U);}));_tmp3EE[0]=_tmp7C1;}),({
struct Cyc_PP_Doc*_tmp7C0=Cyc_Absynpp_exp2doc(e);_tmp3EE[1]=_tmp7C0;}),({
struct Cyc_PP_Doc*_tmp7BF=Cyc_PP_text(({const char*_tmp3F0=") ";_tag_fat(_tmp3F0,sizeof(char),3U);}));_tmp3EE[2]=_tmp7BF;}),({
struct Cyc_PP_Doc*_tmp7BE=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3EE[3]=_tmp7BE;});Cyc_PP_cat(_tag_fat(_tmp3EE,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp3F1="break;";_tag_fat(_tmp3F1,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp3F2="continue;";_tag_fat(_tmp3F2,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp3D2=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_LL12: {struct _fat_ptr*x=_tmp3D2;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3F5=({struct Cyc_String_pa_PrintArg_struct _tmp5D9;_tmp5D9.tag=0U,_tmp5D9.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp5D9;});void*_tmp3F3[1U];_tmp3F3[0]=& _tmp3F5;({struct _fat_ptr _tmp7C2=({const char*_tmp3F4="goto %s;";_tag_fat(_tmp3F4,sizeof(char),9U);});Cyc_aprintf(_tmp7C2,_tag_fat(_tmp3F3,sizeof(void*),1U));});}));goto _LL0;}case 9U: _LL13: _tmp3CE=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_tmp3CF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2).f1;_tmp3D0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f3).f1;_tmp3D1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f4;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp3CE;struct Cyc_Absyn_Exp*e2=_tmp3CF;struct Cyc_Absyn_Exp*e3=_tmp3D0;struct Cyc_Absyn_Stmt*s1=_tmp3D1;
# 1527
s=({struct Cyc_PP_Doc*_tmp3F6[8U];({struct Cyc_PP_Doc*_tmp7CA=Cyc_PP_text(({const char*_tmp3F7="for(";_tag_fat(_tmp3F7,sizeof(char),5U);}));_tmp3F6[0]=_tmp7CA;}),({
struct Cyc_PP_Doc*_tmp7C9=Cyc_Absynpp_exp2doc(e1);_tmp3F6[1]=_tmp7C9;}),({
struct Cyc_PP_Doc*_tmp7C8=Cyc_PP_text(({const char*_tmp3F8="; ";_tag_fat(_tmp3F8,sizeof(char),3U);}));_tmp3F6[2]=_tmp7C8;}),({
struct Cyc_PP_Doc*_tmp7C7=Cyc_Absynpp_exp2doc(e2);_tmp3F6[3]=_tmp7C7;}),({
struct Cyc_PP_Doc*_tmp7C6=Cyc_PP_text(({const char*_tmp3F9="; ";_tag_fat(_tmp3F9,sizeof(char),3U);}));_tmp3F6[4]=_tmp7C6;}),({
struct Cyc_PP_Doc*_tmp7C5=Cyc_Absynpp_exp2doc(e3);_tmp3F6[5]=_tmp7C5;}),({
struct Cyc_PP_Doc*_tmp7C4=Cyc_PP_text(({const char*_tmp3FA=") ";_tag_fat(_tmp3FA,sizeof(char),3U);}));_tmp3F6[6]=_tmp7C4;}),({
struct Cyc_PP_Doc*_tmp7C3=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3F6[7]=_tmp7C3;});Cyc_PP_cat(_tag_fat(_tmp3F6,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 10U: _LL15: _tmp3CC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_tmp3CD=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp3CC;struct Cyc_List_List*ss=_tmp3CD;
# 1537
s=({struct Cyc_PP_Doc*_tmp3FB[8U];({struct Cyc_PP_Doc*_tmp7D2=Cyc_PP_text(({const char*_tmp3FC="switch (";_tag_fat(_tmp3FC,sizeof(char),9U);}));_tmp3FB[0]=_tmp7D2;}),({
struct Cyc_PP_Doc*_tmp7D1=Cyc_Absynpp_exp2doc(e);_tmp3FB[1]=_tmp7D1;}),({
struct Cyc_PP_Doc*_tmp7D0=Cyc_PP_text(({const char*_tmp3FD=") ";_tag_fat(_tmp3FD,sizeof(char),3U);}));_tmp3FB[2]=_tmp7D0;}),({
struct Cyc_PP_Doc*_tmp7CF=Cyc_Absynpp_lb();_tmp3FB[3]=_tmp7CF;}),({
struct Cyc_PP_Doc*_tmp7CE=Cyc_PP_line_doc();_tmp3FB[4]=_tmp7CE;}),({
struct Cyc_PP_Doc*_tmp7CD=Cyc_Absynpp_switchclauses2doc(ss);_tmp3FB[5]=_tmp7CD;}),({
struct Cyc_PP_Doc*_tmp7CC=Cyc_PP_line_doc();_tmp3FB[6]=_tmp7CC;}),({
struct Cyc_PP_Doc*_tmp7CB=Cyc_Absynpp_rb();_tmp3FB[7]=_tmp7CB;});Cyc_PP_cat(_tag_fat(_tmp3FB,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp3FE="fallthru;";_tag_fat(_tmp3FE,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp3CB=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_LL1A: {struct Cyc_List_List*es=_tmp3CB;
# 1548
s=({struct Cyc_PP_Doc*_tmp3FF[3U];({struct Cyc_PP_Doc*_tmp7D5=Cyc_PP_text(({const char*_tmp400="fallthru(";_tag_fat(_tmp400,sizeof(char),10U);}));_tmp3FF[0]=_tmp7D5;}),({struct Cyc_PP_Doc*_tmp7D4=Cyc_Absynpp_exps2doc_prec(20,es);_tmp3FF[1]=_tmp7D4;}),({struct Cyc_PP_Doc*_tmp7D3=Cyc_PP_text(({const char*_tmp401=");";_tag_fat(_tmp401,sizeof(char),3U);}));_tmp3FF[2]=_tmp7D3;});Cyc_PP_cat(_tag_fat(_tmp3FF,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 12U: _LL1B: _tmp3C9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_tmp3CA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp3C9;struct Cyc_Absyn_Stmt*s1=_tmp3CA;
# 1550
struct _tuple14 _tmp402=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple14 _stmttmp1B=_tmp402;struct _tuple14 _tmp403=_stmttmp1B;struct Cyc_List_List*_tmp405;int _tmp404;_LL29: _tmp404=_tmp403.f1;_tmp405=_tmp403.f2;_LL2A: {int newblock=_tmp404;struct Cyc_List_List*newvarsinblock=_tmp405;
s=({struct Cyc_PP_Doc*_tmp406[3U];({struct Cyc_PP_Doc*_tmp7D8=Cyc_Absynpp_decl2doc(d);_tmp406[0]=_tmp7D8;}),({struct Cyc_PP_Doc*_tmp7D7=Cyc_PP_line_doc();_tmp406[1]=_tmp7D7;}),({struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_tmp406[2]=_tmp7D6;});Cyc_PP_cat(_tag_fat(_tmp406,sizeof(struct Cyc_PP_Doc*),3U));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}}case 13U: _LL1D: _tmp3C7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_tmp3C8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2;_LL1E: {struct _fat_ptr*x=_tmp3C7;struct Cyc_Absyn_Stmt*s1=_tmp3C8;
# 1555
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp407[3U];({struct Cyc_PP_Doc*_tmp7DC=Cyc_PP_textptr(x);_tmp407[0]=_tmp7DC;}),({struct Cyc_PP_Doc*_tmp7DB=Cyc_PP_text(({const char*_tmp408=": ";_tag_fat(_tmp408,sizeof(char),3U);}));_tmp407[1]=_tmp7DB;}),({struct Cyc_PP_Doc*_tmp7DA=({int _tmp7D9=stmtexp;Cyc_Absynpp_block(_tmp7D9,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_tmp407[2]=_tmp7DA;});Cyc_PP_cat(_tag_fat(_tmp407,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1558
s=({struct Cyc_PP_Doc*_tmp409[3U];({struct Cyc_PP_Doc*_tmp7DF=Cyc_PP_textptr(x);_tmp409[0]=_tmp7DF;}),({struct Cyc_PP_Doc*_tmp7DE=Cyc_PP_text(({const char*_tmp40A=": ";_tag_fat(_tmp40A,sizeof(char),3U);}));_tmp409[1]=_tmp7DE;}),({struct Cyc_PP_Doc*_tmp7DD=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_tmp409[2]=_tmp7DD;});Cyc_PP_cat(_tag_fat(_tmp409,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 14U: _LL1F: _tmp3C5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_tmp3C6=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2).f1;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp3C5;struct Cyc_Absyn_Exp*e=_tmp3C6;
# 1561
s=({struct Cyc_PP_Doc*_tmp40B[5U];({struct Cyc_PP_Doc*_tmp7E4=Cyc_PP_text(({const char*_tmp40C="do ";_tag_fat(_tmp40C,sizeof(char),4U);}));_tmp40B[0]=_tmp7E4;}),({
struct Cyc_PP_Doc*_tmp7E3=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp40B[1]=_tmp7E3;}),({
struct Cyc_PP_Doc*_tmp7E2=Cyc_PP_text(({const char*_tmp40D=" while (";_tag_fat(_tmp40D,sizeof(char),9U);}));_tmp40B[2]=_tmp7E2;}),({
struct Cyc_PP_Doc*_tmp7E1=Cyc_Absynpp_exp2doc(e);_tmp40B[3]=_tmp7E1;}),({
struct Cyc_PP_Doc*_tmp7E0=Cyc_PP_text(({const char*_tmp40E=");";_tag_fat(_tmp40E,sizeof(char),3U);}));_tmp40B[4]=_tmp7E0;});Cyc_PP_cat(_tag_fat(_tmp40B,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}default: _LL21: _tmp3C3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f1;_tmp3C4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3C2)->f2;_LL22: {struct Cyc_Absyn_Stmt*s1=_tmp3C3;struct Cyc_List_List*ss=_tmp3C4;
# 1568
s=({struct Cyc_PP_Doc*_tmp40F[4U];({struct Cyc_PP_Doc*_tmp7E8=Cyc_PP_text(({const char*_tmp410="try ";_tag_fat(_tmp410,sizeof(char),5U);}));_tmp40F[0]=_tmp7E8;}),({
struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp40F[1]=_tmp7E7;}),({
struct Cyc_PP_Doc*_tmp7E6=Cyc_PP_text(({const char*_tmp411=" catch ";_tag_fat(_tmp411,sizeof(char),8U);}));_tmp40F[2]=_tmp7E6;}),({
struct Cyc_PP_Doc*_tmp7E5=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_tmp40F[3]=_tmp7E5;});Cyc_PP_cat(_tag_fat(_tmp40F,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}_LL0:;}
# 1574
return s;}
# 1577
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp412=p->r;void*_stmttmp1C=_tmp412;void*_tmp413=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp414;int _tmp417;struct Cyc_List_List*_tmp416;struct Cyc_Absyn_Datatypefield*_tmp415;struct Cyc_Absyn_Datatypefield*_tmp418;struct Cyc_Absyn_Enumfield*_tmp419;struct Cyc_Absyn_Enumfield*_tmp41A;int _tmp41D;struct Cyc_List_List*_tmp41C;struct Cyc_List_List*_tmp41B;int _tmp421;struct Cyc_List_List*_tmp420;struct Cyc_List_List*_tmp41F;union Cyc_Absyn_AggrInfo _tmp41E;int _tmp424;struct Cyc_List_List*_tmp423;struct _tuple0*_tmp422;struct _tuple0*_tmp425;struct Cyc_Absyn_Pat*_tmp427;struct Cyc_Absyn_Vardecl*_tmp426;struct Cyc_Absyn_Vardecl*_tmp428;struct Cyc_Absyn_Pat*_tmp429;int _tmp42B;struct Cyc_List_List*_tmp42A;struct Cyc_Absyn_Vardecl*_tmp42D;struct Cyc_Absyn_Tvar*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42F;struct Cyc_Absyn_Tvar*_tmp42E;struct Cyc_Absyn_Pat*_tmp431;struct Cyc_Absyn_Vardecl*_tmp430;struct Cyc_Absyn_Vardecl*_tmp432;struct _fat_ptr _tmp433;char _tmp434;int _tmp436;enum Cyc_Absyn_Sign _tmp435;switch(*((int*)_tmp413)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp437="_";_tag_fat(_tmp437,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp438="NULL";_tag_fat(_tmp438,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp435=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_tmp436=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp435;int i=_tmp436;
# 1583
if((int)sg != (int)1U)
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp43B=({struct Cyc_Int_pa_PrintArg_struct _tmp5DA;_tmp5DA.tag=1U,_tmp5DA.f1=(unsigned long)i;_tmp5DA;});void*_tmp439[1U];_tmp439[0]=& _tmp43B;({struct _fat_ptr _tmp7E9=({const char*_tmp43A="%d";_tag_fat(_tmp43A,sizeof(char),3U);});Cyc_aprintf(_tmp7E9,_tag_fat(_tmp439,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp43E=({struct Cyc_Int_pa_PrintArg_struct _tmp5DB;_tmp5DB.tag=1U,_tmp5DB.f1=(unsigned)i;_tmp5DB;});void*_tmp43C[1U];_tmp43C[0]=& _tmp43E;({struct _fat_ptr _tmp7EA=({const char*_tmp43D="%u";_tag_fat(_tmp43D,sizeof(char),3U);});Cyc_aprintf(_tmp7EA,_tag_fat(_tmp43C,sizeof(void*),1U));});}));}
goto _LL0;}case 11U: _LL7: _tmp434=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_LL8: {char ch=_tmp434;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp441=({struct Cyc_String_pa_PrintArg_struct _tmp5DC;_tmp5DC.tag=0U,({struct _fat_ptr _tmp7EB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp5DC.f1=_tmp7EB;});_tmp5DC;});void*_tmp43F[1U];_tmp43F[0]=& _tmp441;({struct _fat_ptr _tmp7EC=({const char*_tmp440="'%s'";_tag_fat(_tmp440,sizeof(char),5U);});Cyc_aprintf(_tmp7EC,_tag_fat(_tmp43F,sizeof(void*),1U));});}));goto _LL0;}case 12U: _LL9: _tmp433=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_LLA: {struct _fat_ptr x=_tmp433;
s=Cyc_PP_text(x);goto _LL0;}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp413)->f2)->r)->tag == 0U){_LLB: _tmp432=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp432;
# 1590
s=Cyc_Absynpp_qvar2doc(vd->name);goto _LL0;}}else{_LLD: _tmp430=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_tmp431=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp430;struct Cyc_Absyn_Pat*p2=_tmp431;
# 1592
s=({struct Cyc_PP_Doc*_tmp442[3U];({struct Cyc_PP_Doc*_tmp7EF=Cyc_Absynpp_qvar2doc(vd->name);_tmp442[0]=_tmp7EF;}),({struct Cyc_PP_Doc*_tmp7EE=Cyc_PP_text(({const char*_tmp443=" as ";_tag_fat(_tmp443,sizeof(char),5U);}));_tmp442[1]=_tmp7EE;}),({struct Cyc_PP_Doc*_tmp7ED=Cyc_Absynpp_pat2doc(p2);_tmp442[2]=_tmp7ED;});Cyc_PP_cat(_tag_fat(_tmp442,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 2U: _LLF: _tmp42E=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_tmp42F=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp42E;struct Cyc_Absyn_Vardecl*vd=_tmp42F;
# 1594
s=({struct Cyc_PP_Doc*_tmp444[5U];({struct Cyc_PP_Doc*_tmp7F4=Cyc_PP_text(({const char*_tmp445="alias";_tag_fat(_tmp445,sizeof(char),6U);}));_tmp444[0]=_tmp7F4;}),({struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_text(({const char*_tmp446=" <";_tag_fat(_tmp446,sizeof(char),3U);}));_tmp444[1]=_tmp7F3;}),({struct Cyc_PP_Doc*_tmp7F2=Cyc_Absynpp_tvar2doc(tv);_tmp444[2]=_tmp7F2;}),({struct Cyc_PP_Doc*_tmp7F1=Cyc_PP_text(({const char*_tmp447="> ";_tag_fat(_tmp447,sizeof(char),3U);}));_tmp444[3]=_tmp7F1;}),({
struct Cyc_PP_Doc*_tmp7F0=Cyc_Absynpp_vardecl2doc(vd);_tmp444[4]=_tmp7F0;});Cyc_PP_cat(_tag_fat(_tmp444,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 4U: _LL11: _tmp42C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_tmp42D=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp42C;struct Cyc_Absyn_Vardecl*vd=_tmp42D;
# 1598
s=({struct Cyc_PP_Doc*_tmp448[4U];({struct Cyc_PP_Doc*_tmp7F8=Cyc_Absynpp_qvar2doc(vd->name);_tmp448[0]=_tmp7F8;}),({struct Cyc_PP_Doc*_tmp7F7=Cyc_PP_text(({const char*_tmp449="<";_tag_fat(_tmp449,sizeof(char),2U);}));_tmp448[1]=_tmp7F7;}),({struct Cyc_PP_Doc*_tmp7F6=Cyc_Absynpp_tvar2doc(tv);_tmp448[2]=_tmp7F6;}),({struct Cyc_PP_Doc*_tmp7F5=Cyc_PP_text(({const char*_tmp44A=">";_tag_fat(_tmp44A,sizeof(char),2U);}));_tmp448[3]=_tmp7F5;});Cyc_PP_cat(_tag_fat(_tmp448,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 5U: _LL13: _tmp42A=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_tmp42B=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LL14: {struct Cyc_List_List*ts=_tmp42A;int dots=_tmp42B;
# 1601
s=({struct Cyc_PP_Doc*_tmp44B[4U];({struct Cyc_PP_Doc*_tmp7FE=Cyc_Absynpp_dollar();_tmp44B[0]=_tmp7FE;}),({struct Cyc_PP_Doc*_tmp7FD=Cyc_PP_text(({const char*_tmp44C="(";_tag_fat(_tmp44C,sizeof(char),2U);}));_tmp44B[1]=_tmp7FD;}),({struct Cyc_PP_Doc*_tmp7FC=({struct _fat_ptr _tmp7FB=({const char*_tmp44D=",";_tag_fat(_tmp44D,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp7FB,ts);});_tmp44B[2]=_tmp7FC;}),
dots?({struct Cyc_PP_Doc*_tmp7FA=Cyc_PP_text(({const char*_tmp44E=", ...)";_tag_fat(_tmp44E,sizeof(char),7U);}));_tmp44B[3]=_tmp7FA;}):({struct Cyc_PP_Doc*_tmp7F9=Cyc_PP_text(({const char*_tmp44F=")";_tag_fat(_tmp44F,sizeof(char),2U);}));_tmp44B[3]=_tmp7F9;});Cyc_PP_cat(_tag_fat(_tmp44B,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LL15: _tmp429=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_LL16: {struct Cyc_Absyn_Pat*p2=_tmp429;
# 1605
s=({struct Cyc_PP_Doc*_tmp450[2U];({struct Cyc_PP_Doc*_tmp800=Cyc_PP_text(({const char*_tmp451="&";_tag_fat(_tmp451,sizeof(char),2U);}));_tmp450[0]=_tmp800;}),({struct Cyc_PP_Doc*_tmp7FF=Cyc_Absynpp_pat2doc(p2);_tmp450[1]=_tmp7FF;});Cyc_PP_cat(_tag_fat(_tmp450,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp413)->f2)->r)->tag == 0U){_LL17: _tmp428=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp428;
# 1608
s=({struct Cyc_PP_Doc*_tmp452[2U];({struct Cyc_PP_Doc*_tmp802=Cyc_PP_text(({const char*_tmp453="*";_tag_fat(_tmp453,sizeof(char),2U);}));_tmp452[0]=_tmp802;}),({struct Cyc_PP_Doc*_tmp801=Cyc_Absynpp_qvar2doc(vd->name);_tmp452[1]=_tmp801;});Cyc_PP_cat(_tag_fat(_tmp452,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}else{_LL19: _tmp426=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_tmp427=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp426;struct Cyc_Absyn_Pat*p2=_tmp427;
# 1611
s=({struct Cyc_PP_Doc*_tmp454[4U];({struct Cyc_PP_Doc*_tmp806=Cyc_PP_text(({const char*_tmp455="*";_tag_fat(_tmp455,sizeof(char),2U);}));_tmp454[0]=_tmp806;}),({struct Cyc_PP_Doc*_tmp805=Cyc_Absynpp_qvar2doc(vd->name);_tmp454[1]=_tmp805;}),({struct Cyc_PP_Doc*_tmp804=Cyc_PP_text(({const char*_tmp456=" as ";_tag_fat(_tmp456,sizeof(char),5U);}));_tmp454[2]=_tmp804;}),({struct Cyc_PP_Doc*_tmp803=Cyc_Absynpp_pat2doc(p2);_tmp454[3]=_tmp803;});Cyc_PP_cat(_tag_fat(_tmp454,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}case 15U: _LL1B: _tmp425=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_LL1C: {struct _tuple0*q=_tmp425;
# 1614
s=Cyc_Absynpp_qvar2doc(q);
goto _LL0;}case 16U: _LL1D: _tmp422=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_tmp423=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_tmp424=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp413)->f3;_LL1E: {struct _tuple0*q=_tmp422;struct Cyc_List_List*ps=_tmp423;int dots=_tmp424;
# 1617
struct _fat_ptr term=dots?({const char*_tmp45A=", ...)";_tag_fat(_tmp45A,sizeof(char),7U);}):({const char*_tmp45B=")";_tag_fat(_tmp45B,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp457[2U];({struct Cyc_PP_Doc*_tmp80B=Cyc_Absynpp_qvar2doc(q);_tmp457[0]=_tmp80B;}),({struct Cyc_PP_Doc*_tmp80A=({struct _fat_ptr _tmp809=({const char*_tmp458="(";_tag_fat(_tmp458,sizeof(char),2U);});struct _fat_ptr _tmp808=term;struct _fat_ptr _tmp807=({const char*_tmp459=",";_tag_fat(_tmp459,sizeof(char),2U);});Cyc_PP_group(_tmp809,_tmp808,_tmp807,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp457[1]=_tmp80A;});Cyc_PP_cat(_tag_fat(_tmp457,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413)->f1 != 0){_LL1F: _tmp41E=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_tmp41F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_tmp420=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413)->f3;_tmp421=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413)->f4;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp41E;struct Cyc_List_List*exists=_tmp41F;struct Cyc_List_List*dps=_tmp420;int dots=_tmp421;
# 1621
struct _fat_ptr term=dots?({const char*_tmp465=", ...}";_tag_fat(_tmp465,sizeof(char),7U);}):({const char*_tmp466="}";_tag_fat(_tmp466,sizeof(char),2U);});
struct _tuple11 _tmp45C=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp1D=_tmp45C;struct _tuple11 _tmp45D=_stmttmp1D;struct _tuple0*_tmp45E;_LL2E: _tmp45E=_tmp45D.f2;_LL2F: {struct _tuple0*n=_tmp45E;
s=({struct Cyc_PP_Doc*_tmp45F[4U];({struct Cyc_PP_Doc*_tmp815=Cyc_Absynpp_qvar2doc(n);_tmp45F[0]=_tmp815;}),({struct Cyc_PP_Doc*_tmp814=Cyc_Absynpp_lb();_tmp45F[1]=_tmp814;}),({
struct Cyc_PP_Doc*_tmp813=({struct _fat_ptr _tmp812=({const char*_tmp460="[";_tag_fat(_tmp460,sizeof(char),2U);});struct _fat_ptr _tmp811=({const char*_tmp461="]";_tag_fat(_tmp461,sizeof(char),2U);});struct _fat_ptr _tmp810=({const char*_tmp462=",";_tag_fat(_tmp462,sizeof(char),2U);});Cyc_PP_egroup(_tmp812,_tmp811,_tmp810,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp45F[2]=_tmp813;}),({
struct Cyc_PP_Doc*_tmp80F=({struct _fat_ptr _tmp80E=({const char*_tmp463="";_tag_fat(_tmp463,sizeof(char),1U);});struct _fat_ptr _tmp80D=term;struct _fat_ptr _tmp80C=({const char*_tmp464=",";_tag_fat(_tmp464,sizeof(char),2U);});Cyc_PP_group(_tmp80E,_tmp80D,_tmp80C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp45F[3]=_tmp80F;});Cyc_PP_cat(_tag_fat(_tmp45F,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}}else{_LL21: _tmp41B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_tmp41C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413)->f3;_tmp41D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413)->f4;_LL22: {struct Cyc_List_List*exists=_tmp41B;struct Cyc_List_List*dps=_tmp41C;int dots=_tmp41D;
# 1628
struct _fat_ptr term=dots?({const char*_tmp46D=", ...}";_tag_fat(_tmp46D,sizeof(char),7U);}):({const char*_tmp46E="}";_tag_fat(_tmp46E,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp467[3U];({struct Cyc_PP_Doc*_tmp81E=Cyc_Absynpp_lb();_tmp467[0]=_tmp81E;}),({
struct Cyc_PP_Doc*_tmp81D=({struct _fat_ptr _tmp81C=({const char*_tmp468="[";_tag_fat(_tmp468,sizeof(char),2U);});struct _fat_ptr _tmp81B=({const char*_tmp469="]";_tag_fat(_tmp469,sizeof(char),2U);});struct _fat_ptr _tmp81A=({const char*_tmp46A=",";_tag_fat(_tmp46A,sizeof(char),2U);});Cyc_PP_egroup(_tmp81C,_tmp81B,_tmp81A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp467[1]=_tmp81D;}),({
struct Cyc_PP_Doc*_tmp819=({struct _fat_ptr _tmp818=({const char*_tmp46B="";_tag_fat(_tmp46B,sizeof(char),1U);});struct _fat_ptr _tmp817=term;struct _fat_ptr _tmp816=({const char*_tmp46C=",";_tag_fat(_tmp46C,sizeof(char),2U);});Cyc_PP_group(_tmp818,_tmp817,_tmp816,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp467[2]=_tmp819;});Cyc_PP_cat(_tag_fat(_tmp467,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp41A=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp41A;
_tmp419=ef;goto _LL26;}case 14U: _LL25: _tmp419=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp419;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp413)->f3 == 0){_LL27: _tmp418=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_LL28: {struct Cyc_Absyn_Datatypefield*tuf=_tmp418;
s=Cyc_Absynpp_qvar2doc(tuf->name);goto _LL0;}}else{_LL29: _tmp415=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp413)->f2;_tmp416=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp413)->f3;_tmp417=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp413)->f4;_LL2A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp415;struct Cyc_List_List*ps=_tmp416;int dots=_tmp417;
# 1637
struct _fat_ptr term=dots?({const char*_tmp472=", ...)";_tag_fat(_tmp472,sizeof(char),7U);}):({const char*_tmp473=")";_tag_fat(_tmp473,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp46F[2U];({struct Cyc_PP_Doc*_tmp823=Cyc_Absynpp_qvar2doc(tuf->name);_tmp46F[0]=_tmp823;}),({struct Cyc_PP_Doc*_tmp822=({struct _fat_ptr _tmp821=({const char*_tmp470="(";_tag_fat(_tmp470,sizeof(char),2U);});struct _fat_ptr _tmp820=term;struct _fat_ptr _tmp81F=({const char*_tmp471=",";_tag_fat(_tmp471,sizeof(char),2U);});Cyc_PP_egroup(_tmp821,_tmp820,_tmp81F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp46F[1]=_tmp822;});Cyc_PP_cat(_tag_fat(_tmp46F,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp414=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp413)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp414;
# 1641
s=Cyc_Absynpp_exp2doc(e);goto _LL0;}}_LL0:;}
# 1643
return s;}
# 1646
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp){
return({struct Cyc_PP_Doc*_tmp474[2U];({struct Cyc_PP_Doc*_tmp828=({struct _fat_ptr _tmp827=({const char*_tmp475="";_tag_fat(_tmp475,sizeof(char),1U);});struct _fat_ptr _tmp826=({const char*_tmp476="=";_tag_fat(_tmp476,sizeof(char),2U);});struct _fat_ptr _tmp825=({const char*_tmp477="=";_tag_fat(_tmp477,sizeof(char),2U);});Cyc_PP_egroup(_tmp827,_tmp826,_tmp825,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp474[0]=_tmp828;}),({
struct Cyc_PP_Doc*_tmp824=Cyc_Absynpp_pat2doc((*dp).f2);_tmp474[1]=_tmp824;});Cyc_PP_cat(_tag_fat(_tmp474,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1651
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1653
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp478[2U];({struct Cyc_PP_Doc*_tmp82B=Cyc_PP_text(({const char*_tmp479="default: ";_tag_fat(_tmp479,sizeof(char),10U);}));_tmp478[0]=_tmp82B;}),({
struct Cyc_PP_Doc*_tmp82A=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp47A[2U];({struct Cyc_PP_Doc*_tmp829=Cyc_PP_line_doc();_tmp47A[0]=_tmp829;}),_tmp47A[1]=body;Cyc_PP_cat(_tag_fat(_tmp47A,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp478[1]=_tmp82A;});Cyc_PP_cat(_tag_fat(_tmp478,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp47B[4U];({struct Cyc_PP_Doc*_tmp830=Cyc_PP_text(({const char*_tmp47C="case ";_tag_fat(_tmp47C,sizeof(char),6U);}));_tmp47B[0]=_tmp830;}),({
struct Cyc_PP_Doc*_tmp82F=Cyc_Absynpp_pat2doc(c->pattern);_tmp47B[1]=_tmp82F;}),({
struct Cyc_PP_Doc*_tmp82E=Cyc_PP_text(({const char*_tmp47D=": ";_tag_fat(_tmp47D,sizeof(char),3U);}));_tmp47B[2]=_tmp82E;}),({
struct Cyc_PP_Doc*_tmp82D=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp47E[2U];({struct Cyc_PP_Doc*_tmp82C=Cyc_PP_line_doc();_tmp47E[0]=_tmp82C;}),_tmp47E[1]=body;Cyc_PP_cat(_tag_fat(_tmp47E,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp47B[3]=_tmp82D;});Cyc_PP_cat(_tag_fat(_tmp47B,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1665
return({struct Cyc_PP_Doc*_tmp47F[6U];({struct Cyc_PP_Doc*_tmp837=Cyc_PP_text(({const char*_tmp480="case ";_tag_fat(_tmp480,sizeof(char),6U);}));_tmp47F[0]=_tmp837;}),({
struct Cyc_PP_Doc*_tmp836=Cyc_Absynpp_pat2doc(c->pattern);_tmp47F[1]=_tmp836;}),({
struct Cyc_PP_Doc*_tmp835=Cyc_PP_text(({const char*_tmp481=" && ";_tag_fat(_tmp481,sizeof(char),5U);}));_tmp47F[2]=_tmp835;}),({
struct Cyc_PP_Doc*_tmp834=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp47F[3]=_tmp834;}),({
struct Cyc_PP_Doc*_tmp833=Cyc_PP_text(({const char*_tmp482=": ";_tag_fat(_tmp482,sizeof(char),3U);}));_tmp47F[4]=_tmp833;}),({
struct Cyc_PP_Doc*_tmp832=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp483[2U];({struct Cyc_PP_Doc*_tmp831=Cyc_PP_line_doc();_tmp483[0]=_tmp831;}),_tmp483[1]=body;Cyc_PP_cat(_tag_fat(_tmp483,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp47F[5]=_tmp832;});Cyc_PP_cat(_tag_fat(_tmp47F,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1673
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _fat_ptr _tmp838=({const char*_tmp484="";_tag_fat(_tmp484,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp838,cs);});}
# 1677
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1681
return({struct Cyc_PP_Doc*_tmp485[3U];({struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_qvar2doc(f->name);_tmp485[0]=_tmp83B;}),({struct Cyc_PP_Doc*_tmp83A=Cyc_PP_text(({const char*_tmp486=" = ";_tag_fat(_tmp486,sizeof(char),4U);}));_tmp485[1]=_tmp83A;}),({struct Cyc_PP_Doc*_tmp839=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp485[2]=_tmp839;});Cyc_PP_cat(_tag_fat(_tmp485,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1684
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _fat_ptr _tmp83C=({const char*_tmp487=",";_tag_fat(_tmp487,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp83C,fs);});}
# 1688
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1692
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _fat_ptr _tmp83D=({const char*_tmp488=",";_tag_fat(_tmp488,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp83D,vds);});}
# 1696
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp489=vd;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Exp*_tmp48F;void*_tmp48E;struct Cyc_Absyn_Tqual _tmp48D;unsigned _tmp48C;struct _tuple0*_tmp48B;enum Cyc_Absyn_Scope _tmp48A;_LL1: _tmp48A=_tmp489->sc;_tmp48B=_tmp489->name;_tmp48C=_tmp489->varloc;_tmp48D=_tmp489->tq;_tmp48E=_tmp489->type;_tmp48F=_tmp489->initializer;_tmp490=_tmp489->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp48A;struct _tuple0*name=_tmp48B;unsigned varloc=_tmp48C;struct Cyc_Absyn_Tqual tq=_tmp48D;void*type=_tmp48E;struct Cyc_Absyn_Exp*initializer=_tmp48F;struct Cyc_List_List*atts=_tmp490;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _tmp491=Cyc_Flags_c_compiler;if(_tmp491 == Cyc_Flags_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1705
{void*_tmp492=Cyc_Absyn_compress(type);void*_stmttmp1E=_tmp492;void*_tmp493=_stmttmp1E;struct Cyc_List_List*_tmp494;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp493)->tag == 5U){_LL9: _tmp494=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp493)->f1).attributes;_LLA: {struct Cyc_List_List*atts2=_tmp494;
# 1707
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1711
goto _LL3;}_LL3:;}{
# 1714
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Flags_C_Compilers _tmp495=Cyc_Flags_c_compiler;if(_tmp495 == Cyc_Flags_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1719
s=({struct Cyc_PP_Doc*_tmp496[5U];_tmp496[0]=tmp_doc,({
# 1721
struct Cyc_PP_Doc*_tmp847=Cyc_Absynpp_scope2doc(sc);_tmp496[1]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp846=({struct Cyc_Absyn_Tqual _tmp845=tq;void*_tmp844=type;Cyc_Absynpp_tqtd2doc(_tmp845,_tmp844,({struct Cyc_Core_Opt*_tmp498=_cycalloc(sizeof(*_tmp498));({struct Cyc_PP_Doc*_tmp843=({struct Cyc_PP_Doc*_tmp497[2U];_tmp497[0]=beforenamedoc,_tmp497[1]=sn;Cyc_PP_cat(_tag_fat(_tmp497,sizeof(struct Cyc_PP_Doc*),2U));});_tmp498->v=_tmp843;});_tmp498;}));});_tmp496[2]=_tmp846;}),
initializer == 0?({
struct Cyc_PP_Doc*_tmp842=Cyc_PP_nil_doc();_tmp496[3]=_tmp842;}):({
struct Cyc_PP_Doc*_tmp841=({struct Cyc_PP_Doc*_tmp499[2U];({struct Cyc_PP_Doc*_tmp840=Cyc_PP_text(({const char*_tmp49A=" = ";_tag_fat(_tmp49A,sizeof(char),4U);}));_tmp499[0]=_tmp840;}),({struct Cyc_PP_Doc*_tmp83F=Cyc_Absynpp_exp2doc(initializer);_tmp499[1]=_tmp83F;});Cyc_PP_cat(_tag_fat(_tmp499,sizeof(struct Cyc_PP_Doc*),2U));});_tmp496[3]=_tmp841;}),({
struct Cyc_PP_Doc*_tmp83E=Cyc_PP_text(({const char*_tmp49B=";";_tag_fat(_tmp49B,sizeof(char),2U);}));_tmp496[4]=_tmp83E;});Cyc_PP_cat(_tag_fat(_tmp496,sizeof(struct Cyc_PP_Doc*),5U));});
return s;}}}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1730
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
struct _tuple20 _tmp49C=*x;struct _tuple20 _stmttmp1F=_tmp49C;struct _tuple20 _tmp49D=_stmttmp1F;struct _tuple0*_tmp49E;_LL1: _tmp49E=_tmp49D.f2;_LL2: {struct _tuple0*v=_tmp49E;
return Cyc_Absynpp_qvar2doc(v);}}
# 1735
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp49F[4U];({struct Cyc_PP_Doc*_tmp84B=Cyc_Absynpp_scope2doc(ad->sc);_tmp49F[0]=_tmp84B;}),({
struct Cyc_PP_Doc*_tmp84A=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp49F[1]=_tmp84A;}),({
struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp49F[2]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp49F[3]=_tmp848;});Cyc_PP_cat(_tag_fat(_tmp49F,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1742
return({struct Cyc_PP_Doc*_tmp4A0[12U];({struct Cyc_PP_Doc*_tmp85C=Cyc_Absynpp_scope2doc(ad->sc);_tmp4A0[0]=_tmp85C;}),({
struct Cyc_PP_Doc*_tmp85B=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp4A0[1]=_tmp85B;}),({
struct Cyc_PP_Doc*_tmp85A=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp4A0[2]=_tmp85A;}),({
struct Cyc_PP_Doc*_tmp859=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp4A0[3]=_tmp859;}),({
struct Cyc_PP_Doc*_tmp858=Cyc_PP_blank_doc();_tmp4A0[4]=_tmp858;}),({struct Cyc_PP_Doc*_tmp857=Cyc_Absynpp_lb();_tmp4A0[5]=_tmp857;}),({
struct Cyc_PP_Doc*_tmp856=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp4A0[6]=_tmp856;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp855=Cyc_PP_nil_doc();_tmp4A0[7]=_tmp855;}):({
struct Cyc_PP_Doc*_tmp854=({struct Cyc_PP_Doc*_tmp4A1[2U];({struct Cyc_PP_Doc*_tmp853=Cyc_PP_text(({const char*_tmp4A2=":";_tag_fat(_tmp4A2,sizeof(char),2U);}));_tmp4A1[0]=_tmp853;}),({struct Cyc_PP_Doc*_tmp852=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp4A1[1]=_tmp852;});Cyc_PP_cat(_tag_fat(_tmp4A1,sizeof(struct Cyc_PP_Doc*),2U));});_tmp4A0[7]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp851=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4A3[2U];({struct Cyc_PP_Doc*_tmp850=Cyc_PP_line_doc();_tmp4A3[0]=_tmp850;}),({struct Cyc_PP_Doc*_tmp84F=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp4A3[1]=_tmp84F;});Cyc_PP_cat(_tag_fat(_tmp4A3,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4A0[8]=_tmp851;}),({
struct Cyc_PP_Doc*_tmp84E=Cyc_PP_line_doc();_tmp4A0[9]=_tmp84E;}),({
struct Cyc_PP_Doc*_tmp84D=Cyc_Absynpp_rb();_tmp4A0[10]=_tmp84D;}),({
struct Cyc_PP_Doc*_tmp84C=Cyc_Absynpp_atts2doc(ad->attributes);_tmp4A0[11]=_tmp84C;});Cyc_PP_cat(_tag_fat(_tmp4A0,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1756
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp4A4=dd;int _tmp4A9;struct Cyc_Core_Opt*_tmp4A8;struct Cyc_List_List*_tmp4A7;struct _tuple0*_tmp4A6;enum Cyc_Absyn_Scope _tmp4A5;_LL1: _tmp4A5=_tmp4A4->sc;_tmp4A6=_tmp4A4->name;_tmp4A7=_tmp4A4->tvs;_tmp4A8=_tmp4A4->fields;_tmp4A9=_tmp4A4->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp4A5;struct _tuple0*name=_tmp4A6;struct Cyc_List_List*tvs=_tmp4A7;struct Cyc_Core_Opt*fields=_tmp4A8;int is_x=_tmp4A9;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp4AA[5U];({struct Cyc_PP_Doc*_tmp863=Cyc_Absynpp_scope2doc(sc);_tmp4AA[0]=_tmp863;}),
is_x?({struct Cyc_PP_Doc*_tmp862=Cyc_PP_text(({const char*_tmp4AB="@extensible ";_tag_fat(_tmp4AB,sizeof(char),13U);}));_tmp4AA[1]=_tmp862;}):({struct Cyc_PP_Doc*_tmp861=Cyc_PP_blank_doc();_tmp4AA[1]=_tmp861;}),({
struct Cyc_PP_Doc*_tmp860=Cyc_PP_text(({const char*_tmp4AC="datatype ";_tag_fat(_tmp4AC,sizeof(char),10U);}));_tmp4AA[2]=_tmp860;}),
is_x?({struct Cyc_PP_Doc*_tmp85F=Cyc_Absynpp_qvar2bolddoc(name);_tmp4AA[3]=_tmp85F;}):({struct Cyc_PP_Doc*_tmp85E=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp4AA[3]=_tmp85E;}),({
struct Cyc_PP_Doc*_tmp85D=Cyc_Absynpp_ktvars2doc(tvs);_tmp4AA[4]=_tmp85D;});Cyc_PP_cat(_tag_fat(_tmp4AA,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1765
return({struct Cyc_PP_Doc*_tmp4AD[10U];({struct Cyc_PP_Doc*_tmp871=Cyc_Absynpp_scope2doc(sc);_tmp4AD[0]=_tmp871;}),
is_x?({struct Cyc_PP_Doc*_tmp870=Cyc_PP_text(({const char*_tmp4AE="@extensible ";_tag_fat(_tmp4AE,sizeof(char),13U);}));_tmp4AD[1]=_tmp870;}):({struct Cyc_PP_Doc*_tmp86F=Cyc_PP_blank_doc();_tmp4AD[1]=_tmp86F;}),({
struct Cyc_PP_Doc*_tmp86E=Cyc_PP_text(({const char*_tmp4AF="datatype ";_tag_fat(_tmp4AF,sizeof(char),10U);}));_tmp4AD[2]=_tmp86E;}),
is_x?({struct Cyc_PP_Doc*_tmp86D=Cyc_Absynpp_qvar2bolddoc(name);_tmp4AD[3]=_tmp86D;}):({struct Cyc_PP_Doc*_tmp86C=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp4AD[3]=_tmp86C;}),({
struct Cyc_PP_Doc*_tmp86B=Cyc_Absynpp_ktvars2doc(tvs);_tmp4AD[4]=_tmp86B;}),({
struct Cyc_PP_Doc*_tmp86A=Cyc_PP_blank_doc();_tmp4AD[5]=_tmp86A;}),({struct Cyc_PP_Doc*_tmp869=Cyc_Absynpp_lb();_tmp4AD[6]=_tmp869;}),({
struct Cyc_PP_Doc*_tmp868=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4B0[2U];({struct Cyc_PP_Doc*_tmp867=Cyc_PP_line_doc();_tmp4B0[0]=_tmp867;}),({struct Cyc_PP_Doc*_tmp866=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_tmp4B0[1]=_tmp866;});Cyc_PP_cat(_tag_fat(_tmp4B0,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4AD[7]=_tmp868;}),({
struct Cyc_PP_Doc*_tmp865=Cyc_PP_line_doc();_tmp4AD[8]=_tmp865;}),({
struct Cyc_PP_Doc*_tmp864=Cyc_Absynpp_rb();_tmp4AD[9]=_tmp864;});Cyc_PP_cat(_tag_fat(_tmp4AD,sizeof(struct Cyc_PP_Doc*),10U));});}}}
# 1776
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp4B1=ed;struct Cyc_Core_Opt*_tmp4B4;struct _tuple0*_tmp4B3;enum Cyc_Absyn_Scope _tmp4B2;_LL1: _tmp4B2=_tmp4B1->sc;_tmp4B3=_tmp4B1->name;_tmp4B4=_tmp4B1->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp4B2;struct _tuple0*n=_tmp4B3;struct Cyc_Core_Opt*fields=_tmp4B4;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp4B5[3U];({struct Cyc_PP_Doc*_tmp874=Cyc_Absynpp_scope2doc(sc);_tmp4B5[0]=_tmp874;}),({
struct Cyc_PP_Doc*_tmp873=Cyc_PP_text(({const char*_tmp4B6="enum ";_tag_fat(_tmp4B6,sizeof(char),6U);}));_tmp4B5[1]=_tmp873;}),({
struct Cyc_PP_Doc*_tmp872=Cyc_Absynpp_typedef_name2bolddoc(n);_tmp4B5[2]=_tmp872;});Cyc_PP_cat(_tag_fat(_tmp4B5,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1784
return({struct Cyc_PP_Doc*_tmp4B7[8U];({struct Cyc_PP_Doc*_tmp87E=Cyc_Absynpp_scope2doc(sc);_tmp4B7[0]=_tmp87E;}),({
struct Cyc_PP_Doc*_tmp87D=Cyc_PP_text(({const char*_tmp4B8="enum ";_tag_fat(_tmp4B8,sizeof(char),6U);}));_tmp4B7[1]=_tmp87D;}),({
struct Cyc_PP_Doc*_tmp87C=Cyc_Absynpp_qvar2bolddoc(n);_tmp4B7[2]=_tmp87C;}),({
struct Cyc_PP_Doc*_tmp87B=Cyc_PP_blank_doc();_tmp4B7[3]=_tmp87B;}),({struct Cyc_PP_Doc*_tmp87A=Cyc_Absynpp_lb();_tmp4B7[4]=_tmp87A;}),({
struct Cyc_PP_Doc*_tmp879=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4B9[2U];({struct Cyc_PP_Doc*_tmp878=Cyc_PP_line_doc();_tmp4B9[0]=_tmp878;}),({struct Cyc_PP_Doc*_tmp877=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_tmp4B9[1]=_tmp877;});Cyc_PP_cat(_tag_fat(_tmp4B9,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4B7[5]=_tmp879;}),({
struct Cyc_PP_Doc*_tmp876=Cyc_PP_line_doc();_tmp4B7[6]=_tmp876;}),({
struct Cyc_PP_Doc*_tmp875=Cyc_Absynpp_rb();_tmp4B7[7]=_tmp875;});Cyc_PP_cat(_tag_fat(_tmp4B7,sizeof(struct Cyc_PP_Doc*),8U));});}}}
# 1793
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp4BA=d->r;void*_stmttmp20=_tmp4BA;void*_tmp4BB=_stmttmp20;struct _tuple10*_tmp4BF;struct Cyc_List_List*_tmp4BE;struct Cyc_List_List*_tmp4BD;struct Cyc_List_List*_tmp4BC;struct Cyc_List_List*_tmp4C0;struct Cyc_List_List*_tmp4C2;struct _tuple0*_tmp4C1;struct Cyc_List_List*_tmp4C4;struct _fat_ptr*_tmp4C3;struct Cyc_Absyn_Typedefdecl*_tmp4C5;struct Cyc_Absyn_Enumdecl*_tmp4C6;struct Cyc_List_List*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Pat*_tmp4C8;struct Cyc_Absyn_Datatypedecl*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Vardecl*_tmp4CC;struct Cyc_Absyn_Tvar*_tmp4CB;struct Cyc_Absyn_Vardecl*_tmp4CE;struct Cyc_Absyn_Aggrdecl*_tmp4CF;struct Cyc_Absyn_Fndecl*_tmp4D0;switch(*((int*)_tmp4BB)){case 1U: _LL1: _tmp4D0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp4D0;
# 1797
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->tag=5U,_tmp4E1->f1=type_info;_tmp4E1;});
if(fd->cached_type != 0){
void*_tmp4D1=Cyc_Absyn_compress((void*)_check_null(fd->cached_type));void*_stmttmp21=_tmp4D1;void*_tmp4D2=_stmttmp21;struct Cyc_Absyn_FnInfo _tmp4D3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->tag == 5U){_LL24: _tmp4D3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1;_LL25: {struct Cyc_Absyn_FnInfo i=_tmp4D3;
# 1803
({struct Cyc_List_List*_tmp87F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp87F;});goto _LL23;}}else{_LL26: _LL27:
({void*_tmp4D4=0U;({struct _fat_ptr _tmp880=({const char*_tmp4D5="function has non-function type";_tag_fat(_tmp4D5,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp880,_tag_fat(_tmp4D4,sizeof(void*),0U));});});}_LL23:;}{
# 1806
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((fd->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
enum Cyc_Flags_C_Compilers _tmp4D6=Cyc_Flags_c_compiler;if(_tmp4D6 == Cyc_Flags_Gcc_c){_LL29: _LL2A:
 inlinedoc=Cyc_PP_text(({const char*_tmp4D7="inline ";_tag_fat(_tmp4D7,sizeof(char),8U);}));goto _LL28;}else{_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp4D8="__inline ";_tag_fat(_tmp4D8,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1814
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _tmp4D9=Cyc_Flags_c_compiler;if(_tmp4D9 == Cyc_Flags_Gcc_c){_LL2E: _LL2F:
 beforenamedoc=attsdoc;goto _LL2D;}else{_LL30: _LL31:
 beforenamedoc=Cyc_Absynpp_callconv2doc((fd->i).attributes);goto _LL2D;}_LL2D:;}{
# 1821
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp883=Cyc_Absyn_empty_tqual(0U);void*_tmp882=t;Cyc_Absynpp_tqtd2doc(_tmp883,_tmp882,({struct Cyc_Core_Opt*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));({
struct Cyc_PP_Doc*_tmp881=({struct Cyc_PP_Doc*_tmp4DF[2U];_tmp4DF[0]=beforenamedoc,_tmp4DF[1]=namedoc;Cyc_PP_cat(_tag_fat(_tmp4DF,sizeof(struct Cyc_PP_Doc*),2U));});_tmp4E0->v=_tmp881;});_tmp4E0;}));});
# 1829
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp4DD[5U];({struct Cyc_PP_Doc*_tmp88A=Cyc_PP_blank_doc();_tmp4DD[0]=_tmp88A;}),({struct Cyc_PP_Doc*_tmp889=Cyc_Absynpp_lb();_tmp4DD[1]=_tmp889;}),({
struct Cyc_PP_Doc*_tmp888=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4DE[2U];({struct Cyc_PP_Doc*_tmp887=Cyc_PP_line_doc();_tmp4DE[0]=_tmp887;}),({struct Cyc_PP_Doc*_tmp886=Cyc_Absynpp_stmt2doc(fd->body,0,0);_tmp4DE[1]=_tmp886;});Cyc_PP_cat(_tag_fat(_tmp4DE,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4DD[2]=_tmp888;}),({
struct Cyc_PP_Doc*_tmp885=Cyc_PP_line_doc();_tmp4DD[3]=_tmp885;}),({
struct Cyc_PP_Doc*_tmp884=Cyc_Absynpp_rb();_tmp4DD[4]=_tmp884;});Cyc_PP_cat(_tag_fat(_tmp4DD,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp4DA[4U];_tmp4DA[0]=inlinedoc,_tmp4DA[1]=scopedoc,_tmp4DA[2]=tqtddoc,_tmp4DA[3]=bodydoc;Cyc_PP_cat(_tag_fat(_tmp4DA,sizeof(struct Cyc_PP_Doc*),4U));});
# 1835
{enum Cyc_Flags_C_Compilers _tmp4DB=Cyc_Flags_c_compiler;if(_tmp4DB == Cyc_Flags_Vc_c){_LL33: _LL34:
 s=({struct Cyc_PP_Doc*_tmp4DC[2U];_tmp4DC[0]=attsdoc,_tmp4DC[1]=s;Cyc_PP_cat(_tag_fat(_tmp4DC,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 1840
goto _LL0;}}}}}case 5U: _LL3: _tmp4CF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp4CF;
# 1843
s=({struct Cyc_PP_Doc*_tmp4E2[2U];({struct Cyc_PP_Doc*_tmp88C=Cyc_Absynpp_aggrdecl2doc(ad);_tmp4E2[0]=_tmp88C;}),({struct Cyc_PP_Doc*_tmp88B=Cyc_PP_text(({const char*_tmp4E3=";";_tag_fat(_tmp4E3,sizeof(char),2U);}));_tmp4E2[1]=_tmp88B;});Cyc_PP_cat(_tag_fat(_tmp4E2,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 0U: _LL5: _tmp4CE=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp4CE;
# 1846
s=Cyc_Absynpp_vardecl2doc(vd);
goto _LL0;}case 4U: _LL7: _tmp4CB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_tmp4CC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4BB)->f2;_tmp4CD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4BB)->f3;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp4CB;struct Cyc_Absyn_Vardecl*vd=_tmp4CC;struct Cyc_Absyn_Exp*open_exp_opt=_tmp4CD;
# 1849
s=({struct Cyc_PP_Doc*_tmp4E4[7U];({struct Cyc_PP_Doc*_tmp897=Cyc_PP_text(({const char*_tmp4E5="region";_tag_fat(_tmp4E5,sizeof(char),7U);}));_tmp4E4[0]=_tmp897;}),({
struct Cyc_PP_Doc*_tmp896=Cyc_PP_text(({const char*_tmp4E6="<";_tag_fat(_tmp4E6,sizeof(char),2U);}));_tmp4E4[1]=_tmp896;}),({
struct Cyc_PP_Doc*_tmp895=Cyc_Absynpp_tvar2doc(tv);_tmp4E4[2]=_tmp895;}),({
struct Cyc_PP_Doc*_tmp894=Cyc_PP_text(({const char*_tmp4E7=">";_tag_fat(_tmp4E7,sizeof(char),2U);}));_tmp4E4[3]=_tmp894;}),({
struct Cyc_PP_Doc*_tmp893=Cyc_Absynpp_qvar2doc(vd->name);_tmp4E4[4]=_tmp893;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_tmp892=({struct Cyc_PP_Doc*_tmp4E8[3U];({struct Cyc_PP_Doc*_tmp891=Cyc_PP_text(({const char*_tmp4E9=" = open(";_tag_fat(_tmp4E9,sizeof(char),9U);}));_tmp4E8[0]=_tmp891;}),({struct Cyc_PP_Doc*_tmp890=Cyc_Absynpp_exp2doc(open_exp_opt);_tmp4E8[1]=_tmp890;}),({
struct Cyc_PP_Doc*_tmp88F=Cyc_PP_text(({const char*_tmp4EA=")";_tag_fat(_tmp4EA,sizeof(char),2U);}));_tmp4E8[2]=_tmp88F;});Cyc_PP_cat(_tag_fat(_tmp4E8,sizeof(struct Cyc_PP_Doc*),3U));});
# 1854
_tmp4E4[5]=_tmp892;}):({
struct Cyc_PP_Doc*_tmp88E=Cyc_PP_nil_doc();_tmp4E4[5]=_tmp88E;}),({
struct Cyc_PP_Doc*_tmp88D=Cyc_PP_text(({const char*_tmp4EB=";";_tag_fat(_tmp4EB,sizeof(char),2U);}));_tmp4E4[6]=_tmp88D;});Cyc_PP_cat(_tag_fat(_tmp4E4,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 6U: _LL9: _tmp4CA=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_LLA: {struct Cyc_Absyn_Datatypedecl*dd=_tmp4CA;
# 1859
s=({struct Cyc_PP_Doc*_tmp4EC[2U];({struct Cyc_PP_Doc*_tmp899=Cyc_Absynpp_datatypedecl2doc(dd);_tmp4EC[0]=_tmp899;}),({struct Cyc_PP_Doc*_tmp898=Cyc_PP_text(({const char*_tmp4ED=";";_tag_fat(_tmp4ED,sizeof(char),2U);}));_tmp4EC[1]=_tmp898;});Cyc_PP_cat(_tag_fat(_tmp4EC,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 2U: _LLB: _tmp4C8=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_tmp4C9=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4BB)->f3;_LLC: {struct Cyc_Absyn_Pat*p=_tmp4C8;struct Cyc_Absyn_Exp*e=_tmp4C9;
# 1862
s=({struct Cyc_PP_Doc*_tmp4EE[5U];({struct Cyc_PP_Doc*_tmp89E=Cyc_PP_text(({const char*_tmp4EF="let ";_tag_fat(_tmp4EF,sizeof(char),5U);}));_tmp4EE[0]=_tmp89E;}),({
struct Cyc_PP_Doc*_tmp89D=Cyc_Absynpp_pat2doc(p);_tmp4EE[1]=_tmp89D;}),({
struct Cyc_PP_Doc*_tmp89C=Cyc_PP_text(({const char*_tmp4F0=" = ";_tag_fat(_tmp4F0,sizeof(char),4U);}));_tmp4EE[2]=_tmp89C;}),({
struct Cyc_PP_Doc*_tmp89B=Cyc_Absynpp_exp2doc(e);_tmp4EE[3]=_tmp89B;}),({
struct Cyc_PP_Doc*_tmp89A=Cyc_PP_text(({const char*_tmp4F1=";";_tag_fat(_tmp4F1,sizeof(char),2U);}));_tmp4EE[4]=_tmp89A;});Cyc_PP_cat(_tag_fat(_tmp4EE,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 3U: _LLD: _tmp4C7=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_LLE: {struct Cyc_List_List*vds=_tmp4C7;
# 1869
s=({struct Cyc_PP_Doc*_tmp4F2[3U];({struct Cyc_PP_Doc*_tmp8A1=Cyc_PP_text(({const char*_tmp4F3="let ";_tag_fat(_tmp4F3,sizeof(char),5U);}));_tmp4F2[0]=_tmp8A1;}),({struct Cyc_PP_Doc*_tmp8A0=Cyc_Absynpp_ids2doc(vds);_tmp4F2[1]=_tmp8A0;}),({struct Cyc_PP_Doc*_tmp89F=Cyc_PP_text(({const char*_tmp4F4=";";_tag_fat(_tmp4F4,sizeof(char),2U);}));_tmp4F2[2]=_tmp89F;});Cyc_PP_cat(_tag_fat(_tmp4F2,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 7U: _LLF: _tmp4C6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp4C6;
# 1872
s=({struct Cyc_PP_Doc*_tmp4F5[2U];({struct Cyc_PP_Doc*_tmp8A3=Cyc_Absynpp_enumdecl2doc(ed);_tmp4F5[0]=_tmp8A3;}),({struct Cyc_PP_Doc*_tmp8A2=Cyc_PP_text(({const char*_tmp4F6=";";_tag_fat(_tmp4F6,sizeof(char),2U);}));_tmp4F5[1]=_tmp8A2;});Cyc_PP_cat(_tag_fat(_tmp4F5,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 8U: _LL11: _tmp4C5=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp4C5;
# 1875
void*t;
if(td->defn != 0)
t=(void*)_check_null(td->defn);else{
# 1879
t=Cyc_Absyn_new_evar(td->kind,0);}
s=({struct Cyc_PP_Doc*_tmp4F7[4U];({struct Cyc_PP_Doc*_tmp8AC=Cyc_PP_text(({const char*_tmp4F8="typedef ";_tag_fat(_tmp4F8,sizeof(char),9U);}));_tmp4F7[0]=_tmp8AC;}),({
struct Cyc_PP_Doc*_tmp8AB=({struct Cyc_Absyn_Tqual _tmp8AA=td->tq;void*_tmp8A9=t;Cyc_Absynpp_tqtd2doc(_tmp8AA,_tmp8A9,({struct Cyc_Core_Opt*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));({
# 1883
struct Cyc_PP_Doc*_tmp8A8=({struct Cyc_PP_Doc*_tmp4F9[2U];({struct Cyc_PP_Doc*_tmp8A7=Cyc_Absynpp_typedef_name2bolddoc(td->name);_tmp4F9[0]=_tmp8A7;}),({
struct Cyc_PP_Doc*_tmp8A6=Cyc_Absynpp_tvars2doc(td->tvs);_tmp4F9[1]=_tmp8A6;});Cyc_PP_cat(_tag_fat(_tmp4F9,sizeof(struct Cyc_PP_Doc*),2U));});
# 1883
_tmp4FA->v=_tmp8A8;});_tmp4FA;}));});
# 1881
_tmp4F7[1]=_tmp8AB;}),({
# 1886
struct Cyc_PP_Doc*_tmp8A5=Cyc_Absynpp_atts2doc(td->atts);_tmp4F7[2]=_tmp8A5;}),({
struct Cyc_PP_Doc*_tmp8A4=Cyc_PP_text(({const char*_tmp4FB=";";_tag_fat(_tmp4FB,sizeof(char),2U);}));_tmp4F7[3]=_tmp8A4;});Cyc_PP_cat(_tag_fat(_tmp4F7,sizeof(struct Cyc_PP_Doc*),4U));});
# 1889
goto _LL0;}case 9U: _LL13: _tmp4C3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_tmp4C4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4BB)->f2;_LL14: {struct _fat_ptr*v=_tmp4C3;struct Cyc_List_List*tdl=_tmp4C4;
# 1891
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_tmp4FC[8U];({struct Cyc_PP_Doc*_tmp8B5=Cyc_PP_text(({const char*_tmp4FD="namespace ";_tag_fat(_tmp4FD,sizeof(char),11U);}));_tmp4FC[0]=_tmp8B5;}),({
struct Cyc_PP_Doc*_tmp8B4=Cyc_PP_textptr(v);_tmp4FC[1]=_tmp8B4;}),({
struct Cyc_PP_Doc*_tmp8B3=Cyc_PP_blank_doc();_tmp4FC[2]=_tmp8B3;}),({struct Cyc_PP_Doc*_tmp8B2=Cyc_Absynpp_lb();_tmp4FC[3]=_tmp8B2;}),({
struct Cyc_PP_Doc*_tmp8B1=Cyc_PP_line_doc();_tmp4FC[4]=_tmp8B1;}),({
struct Cyc_PP_Doc*_tmp8B0=({struct _fat_ptr _tmp8AF=({const char*_tmp4FE="";_tag_fat(_tmp4FE,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8AF,tdl);});_tmp4FC[5]=_tmp8B0;}),({
struct Cyc_PP_Doc*_tmp8AE=Cyc_PP_line_doc();_tmp4FC[6]=_tmp8AE;}),({
struct Cyc_PP_Doc*_tmp8AD=Cyc_Absynpp_rb();_tmp4FC[7]=_tmp8AD;});Cyc_PP_cat(_tag_fat(_tmp4FC,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10U: _LL15: _tmp4C1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_tmp4C2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4BB)->f2;_LL16: {struct _tuple0*q=_tmp4C1;struct Cyc_List_List*tdl=_tmp4C2;
# 1902
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp4FF[8U];({struct Cyc_PP_Doc*_tmp8BE=Cyc_PP_text(({const char*_tmp500="using ";_tag_fat(_tmp500,sizeof(char),7U);}));_tmp4FF[0]=_tmp8BE;}),({
struct Cyc_PP_Doc*_tmp8BD=Cyc_Absynpp_qvar2doc(q);_tmp4FF[1]=_tmp8BD;}),({
struct Cyc_PP_Doc*_tmp8BC=Cyc_PP_blank_doc();_tmp4FF[2]=_tmp8BC;}),({struct Cyc_PP_Doc*_tmp8BB=Cyc_Absynpp_lb();_tmp4FF[3]=_tmp8BB;}),({
struct Cyc_PP_Doc*_tmp8BA=Cyc_PP_line_doc();_tmp4FF[4]=_tmp8BA;}),({
struct Cyc_PP_Doc*_tmp8B9=({struct _fat_ptr _tmp8B8=({const char*_tmp501="";_tag_fat(_tmp501,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8B8,tdl);});_tmp4FF[5]=_tmp8B9;}),({
struct Cyc_PP_Doc*_tmp8B7=Cyc_PP_line_doc();_tmp4FF[6]=_tmp8B7;}),({
struct Cyc_PP_Doc*_tmp8B6=Cyc_Absynpp_rb();_tmp4FF[7]=_tmp8B6;});Cyc_PP_cat(_tag_fat(_tmp4FF,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1911
s=({struct Cyc_PP_Doc*_tmp502[11U];({struct Cyc_PP_Doc*_tmp8CA=Cyc_PP_text(({const char*_tmp503="/* using ";_tag_fat(_tmp503,sizeof(char),10U);}));_tmp502[0]=_tmp8CA;}),({
struct Cyc_PP_Doc*_tmp8C9=Cyc_Absynpp_qvar2doc(q);_tmp502[1]=_tmp8C9;}),({
struct Cyc_PP_Doc*_tmp8C8=Cyc_PP_blank_doc();_tmp502[2]=_tmp8C8;}),({
struct Cyc_PP_Doc*_tmp8C7=Cyc_Absynpp_lb();_tmp502[3]=_tmp8C7;}),({
struct Cyc_PP_Doc*_tmp8C6=Cyc_PP_text(({const char*_tmp504=" */";_tag_fat(_tmp504,sizeof(char),4U);}));_tmp502[4]=_tmp8C6;}),({
struct Cyc_PP_Doc*_tmp8C5=Cyc_PP_line_doc();_tmp502[5]=_tmp8C5;}),({
struct Cyc_PP_Doc*_tmp8C4=({struct _fat_ptr _tmp8C3=({const char*_tmp505="";_tag_fat(_tmp505,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8C3,tdl);});_tmp502[6]=_tmp8C4;}),({
struct Cyc_PP_Doc*_tmp8C2=Cyc_PP_line_doc();_tmp502[7]=_tmp8C2;}),({
struct Cyc_PP_Doc*_tmp8C1=Cyc_PP_text(({const char*_tmp506="/* ";_tag_fat(_tmp506,sizeof(char),4U);}));_tmp502[8]=_tmp8C1;}),({
struct Cyc_PP_Doc*_tmp8C0=Cyc_Absynpp_rb();_tmp502[9]=_tmp8C0;}),({
struct Cyc_PP_Doc*_tmp8BF=Cyc_PP_text(({const char*_tmp507=" */";_tag_fat(_tmp507,sizeof(char),4U);}));_tmp502[10]=_tmp8BF;});Cyc_PP_cat(_tag_fat(_tmp502,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;}case 11U: _LL17: _tmp4C0=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_LL18: {struct Cyc_List_List*tdl=_tmp4C0;
# 1924
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp508[6U];({struct Cyc_PP_Doc*_tmp8D1=Cyc_PP_text(({const char*_tmp509="extern \"C\" ";_tag_fat(_tmp509,sizeof(char),12U);}));_tmp508[0]=_tmp8D1;}),({
struct Cyc_PP_Doc*_tmp8D0=Cyc_Absynpp_lb();_tmp508[1]=_tmp8D0;}),({
struct Cyc_PP_Doc*_tmp8CF=Cyc_PP_line_doc();_tmp508[2]=_tmp8CF;}),({
struct Cyc_PP_Doc*_tmp8CE=({struct _fat_ptr _tmp8CD=({const char*_tmp50A="";_tag_fat(_tmp50A,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8CD,tdl);});_tmp508[3]=_tmp8CE;}),({
struct Cyc_PP_Doc*_tmp8CC=Cyc_PP_line_doc();_tmp508[4]=_tmp8CC;}),({
struct Cyc_PP_Doc*_tmp8CB=Cyc_Absynpp_rb();_tmp508[5]=_tmp8CB;});Cyc_PP_cat(_tag_fat(_tmp508,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1932
s=({struct Cyc_PP_Doc*_tmp50B[9U];({struct Cyc_PP_Doc*_tmp8DB=Cyc_PP_text(({const char*_tmp50C="/* extern \"C\" ";_tag_fat(_tmp50C,sizeof(char),15U);}));_tmp50B[0]=_tmp8DB;}),({
struct Cyc_PP_Doc*_tmp8DA=Cyc_Absynpp_lb();_tmp50B[1]=_tmp8DA;}),({
struct Cyc_PP_Doc*_tmp8D9=Cyc_PP_text(({const char*_tmp50D=" */";_tag_fat(_tmp50D,sizeof(char),4U);}));_tmp50B[2]=_tmp8D9;}),({
struct Cyc_PP_Doc*_tmp8D8=Cyc_PP_line_doc();_tmp50B[3]=_tmp8D8;}),({
struct Cyc_PP_Doc*_tmp8D7=({struct _fat_ptr _tmp8D6=({const char*_tmp50E="";_tag_fat(_tmp50E,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8D6,tdl);});_tmp50B[4]=_tmp8D7;}),({
struct Cyc_PP_Doc*_tmp8D5=Cyc_PP_line_doc();_tmp50B[5]=_tmp8D5;}),({
struct Cyc_PP_Doc*_tmp8D4=Cyc_PP_text(({const char*_tmp50F="/* ";_tag_fat(_tmp50F,sizeof(char),4U);}));_tmp50B[6]=_tmp8D4;}),({
struct Cyc_PP_Doc*_tmp8D3=Cyc_Absynpp_rb();_tmp50B[7]=_tmp8D3;}),({
struct Cyc_PP_Doc*_tmp8D2=Cyc_PP_text(({const char*_tmp510=" */";_tag_fat(_tmp510,sizeof(char),4U);}));_tmp50B[8]=_tmp8D2;});Cyc_PP_cat(_tag_fat(_tmp50B,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 12U: _LL19: _tmp4BC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BB)->f1;_tmp4BD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BB)->f2;_tmp4BE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BB)->f3;_tmp4BF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BB)->f4;_LL1A: {struct Cyc_List_List*tdl=_tmp4BC;struct Cyc_List_List*ovrs=_tmp4BD;struct Cyc_List_List*exs=_tmp4BE;struct _tuple10*wc=_tmp4BF;
# 1943
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp511[7U];({struct Cyc_PP_Doc*_tmp8E3=Cyc_Absynpp_rb();_tmp511[0]=_tmp8E3;}),({struct Cyc_PP_Doc*_tmp8E2=Cyc_PP_text(({const char*_tmp512=" export ";_tag_fat(_tmp512,sizeof(char),9U);}));_tmp511[1]=_tmp8E2;}),({struct Cyc_PP_Doc*_tmp8E1=Cyc_Absynpp_lb();_tmp511[2]=_tmp8E1;}),({
struct Cyc_PP_Doc*_tmp8E0=Cyc_PP_line_doc();_tmp511[3]=_tmp8E0;}),({struct Cyc_PP_Doc*_tmp8DF=({struct _fat_ptr _tmp8DE=({const char*_tmp513=",";_tag_fat(_tmp513,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple20*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp8DE,exs);});_tmp511[4]=_tmp8DF;}),({
struct Cyc_PP_Doc*_tmp8DD=Cyc_PP_line_doc();_tmp511[5]=_tmp8DD;}),({struct Cyc_PP_Doc*_tmp8DC=Cyc_Absynpp_rb();_tmp511[6]=_tmp8DC;});Cyc_PP_cat(_tag_fat(_tmp511,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1951
exs_doc=Cyc_Absynpp_rb();}
if(ovrs != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp514[7U];({struct Cyc_PP_Doc*_tmp8EB=Cyc_Absynpp_rb();_tmp514[0]=_tmp8EB;}),({struct Cyc_PP_Doc*_tmp8EA=Cyc_PP_text(({const char*_tmp515=" cycdef ";_tag_fat(_tmp515,sizeof(char),9U);}));_tmp514[1]=_tmp8EA;}),({struct Cyc_PP_Doc*_tmp8E9=Cyc_Absynpp_lb();_tmp514[2]=_tmp8E9;}),({
struct Cyc_PP_Doc*_tmp8E8=Cyc_PP_line_doc();_tmp514[3]=_tmp8E8;}),({struct Cyc_PP_Doc*_tmp8E7=({struct _fat_ptr _tmp8E6=({const char*_tmp516="";_tag_fat(_tmp516,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8E6,ovrs);});_tmp514[4]=_tmp8E7;}),({
struct Cyc_PP_Doc*_tmp8E5=Cyc_PP_line_doc();_tmp514[5]=_tmp8E5;}),({struct Cyc_PP_Doc*_tmp8E4=Cyc_Absynpp_rb();_tmp514[6]=_tmp8E4;});Cyc_PP_cat(_tag_fat(_tmp514,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1957
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp517[6U];({struct Cyc_PP_Doc*_tmp8F1=Cyc_PP_text(({const char*_tmp518="extern \"C include\" ";_tag_fat(_tmp518,sizeof(char),20U);}));_tmp517[0]=_tmp8F1;}),({
struct Cyc_PP_Doc*_tmp8F0=Cyc_Absynpp_lb();_tmp517[1]=_tmp8F0;}),({
struct Cyc_PP_Doc*_tmp8EF=Cyc_PP_line_doc();_tmp517[2]=_tmp8EF;}),({
struct Cyc_PP_Doc*_tmp8EE=({struct _fat_ptr _tmp8ED=({const char*_tmp519="";_tag_fat(_tmp519,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8ED,tdl);});_tmp517[3]=_tmp8EE;}),({
struct Cyc_PP_Doc*_tmp8EC=Cyc_PP_line_doc();_tmp517[4]=_tmp8EC;}),_tmp517[5]=exs_doc;Cyc_PP_cat(_tag_fat(_tmp517,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1965
s=({struct Cyc_PP_Doc*_tmp51A[9U];({struct Cyc_PP_Doc*_tmp8FB=Cyc_PP_text(({const char*_tmp51B="/* extern \"C include\" ";_tag_fat(_tmp51B,sizeof(char),23U);}));_tmp51A[0]=_tmp8FB;}),({
struct Cyc_PP_Doc*_tmp8FA=Cyc_Absynpp_lb();_tmp51A[1]=_tmp8FA;}),({
struct Cyc_PP_Doc*_tmp8F9=Cyc_PP_text(({const char*_tmp51C=" */";_tag_fat(_tmp51C,sizeof(char),4U);}));_tmp51A[2]=_tmp8F9;}),({
struct Cyc_PP_Doc*_tmp8F8=Cyc_PP_line_doc();_tmp51A[3]=_tmp8F8;}),({
struct Cyc_PP_Doc*_tmp8F7=({struct _fat_ptr _tmp8F6=({const char*_tmp51D="";_tag_fat(_tmp51D,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8F6,tdl);});_tmp51A[4]=_tmp8F7;}),({
struct Cyc_PP_Doc*_tmp8F5=Cyc_PP_line_doc();_tmp51A[5]=_tmp8F5;}),({
struct Cyc_PP_Doc*_tmp8F4=Cyc_PP_text(({const char*_tmp51E="/* ";_tag_fat(_tmp51E,sizeof(char),4U);}));_tmp51A[6]=_tmp8F4;}),({
struct Cyc_PP_Doc*_tmp8F3=Cyc_Absynpp_rb();_tmp51A[7]=_tmp8F3;}),({
struct Cyc_PP_Doc*_tmp8F2=Cyc_PP_text(({const char*_tmp51F=" */";_tag_fat(_tmp51F,sizeof(char),4U);}));_tmp51A[8]=_tmp8F2;});Cyc_PP_cat(_tag_fat(_tmp51A,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 13U: _LL1B: _LL1C:
# 1976
 s=({struct Cyc_PP_Doc*_tmp520[2U];({struct Cyc_PP_Doc*_tmp8FD=Cyc_PP_text(({const char*_tmp521="__cyclone_port_on__;";_tag_fat(_tmp521,sizeof(char),21U);}));_tmp520[0]=_tmp8FD;}),({struct Cyc_PP_Doc*_tmp8FC=Cyc_Absynpp_lb();_tmp520[1]=_tmp8FC;});Cyc_PP_cat(_tag_fat(_tmp520,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1979
 s=({struct Cyc_PP_Doc*_tmp522[2U];({struct Cyc_PP_Doc*_tmp8FF=Cyc_PP_text(({const char*_tmp523="__cyclone_port_off__;";_tag_fat(_tmp523,sizeof(char),22U);}));_tmp522[0]=_tmp8FF;}),({struct Cyc_PP_Doc*_tmp8FE=Cyc_Absynpp_lb();_tmp522[1]=_tmp8FE;});Cyc_PP_cat(_tag_fat(_tmp522,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1982
 s=({struct Cyc_PP_Doc*_tmp524[2U];({struct Cyc_PP_Doc*_tmp901=Cyc_PP_text(({const char*_tmp525="__tempest_on__;";_tag_fat(_tmp525,sizeof(char),16U);}));_tmp524[0]=_tmp901;}),({struct Cyc_PP_Doc*_tmp900=Cyc_Absynpp_lb();_tmp524[1]=_tmp900;});Cyc_PP_cat(_tag_fat(_tmp524,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1985
 s=({struct Cyc_PP_Doc*_tmp526[2U];({struct Cyc_PP_Doc*_tmp903=Cyc_PP_text(({const char*_tmp527="__tempest_off__;";_tag_fat(_tmp527,sizeof(char),17U);}));_tmp526[0]=_tmp903;}),({struct Cyc_PP_Doc*_tmp902=Cyc_Absynpp_lb();_tmp526[1]=_tmp902;});Cyc_PP_cat(_tag_fat(_tmp526,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 1989
return s;}
# 1992
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp528=sc;switch(_tmp528){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp529="static ";_tag_fat(_tmp529,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp52A="extern ";_tag_fat(_tmp52A,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp52B="extern \"C\" ";_tag_fat(_tmp52B,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp52C="abstract ";_tag_fat(_tmp52C,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp52D="register ";_tag_fat(_tmp52D,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;}}
# 2006
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp52E=Cyc_Absyn_compress(t);void*_stmttmp22=_tmp52E;void*_tmp52F=_stmttmp22;struct Cyc_List_List*_tmp530;struct Cyc_Absyn_Tvar*_tmp531;switch(*((int*)_tmp52F)){case 2U: _LL1: _tmp531=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp52F)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp531;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52F)->f1)->tag == 9U){_LL3: _tmp530=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52F)->f2;_LL4: {struct Cyc_List_List*l=_tmp530;
return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2018
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp532=t;void*_tmp534;struct Cyc_Absyn_Typedefdecl*_tmp533;switch(*((int*)_tmp532)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp532)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp533=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp532)->f3;_tmp534=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp532)->f4;if(_tmp534 != 0){_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmp533;void*x=_tmp534;
# 2025
return Cyc_Absynpp_is_anon_aggrtype(x);}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2032
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2035
if(tms != 0 && tms->tl != 0){
struct _tuple18 _tmp535=({struct _tuple18 _tmp5DD;_tmp5DD.f1=(void*)tms->hd,_tmp5DD.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp5DD;});struct _tuple18 _stmttmp23=_tmp535;struct _tuple18 _tmp536=_stmttmp23;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp536.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp536.f2)->tag == 3U){_LL1: _LL2:
# 2038
 return({struct Cyc_List_List*_tmp538=_region_malloc(r,sizeof(*_tmp538));_tmp538->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp905=({struct Cyc_List_List*_tmp537=_region_malloc(r,sizeof(*_tmp537));_tmp537->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp904=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp537->tl=_tmp904;});_tmp537;});_tmp538->tl=_tmp905;});_tmp538;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp539=_region_malloc(r,sizeof(*_tmp539));_tmp539->hd=atts,_tmp539->tl=tms;_tmp539;});}_LL0:;}else{
# 2041
return({struct Cyc_List_List*_tmp53A=_region_malloc(r,sizeof(*_tmp53A));_tmp53A->hd=atts,_tmp53A->tl=tms;_tmp53A;});}}
# 2044
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp53B=({struct _fat_ptr _tmp906=({const char*_tmp541="`";_tag_fat(_tmp541,sizeof(char),2U);});Cyc_strconcat(_tmp906,(struct _fat_ptr)*t->name);});struct _fat_ptr s=_tmp53B;
({struct _fat_ptr _tmp53C=_fat_ptr_plus(s,sizeof(char),1);char _tmp53D=*((char*)_check_fat_subscript(_tmp53C,sizeof(char),0U));char _tmp53E='t';if(_get_fat_size(_tmp53C,sizeof(char))== 1U &&(_tmp53D == 0 && _tmp53E != 0))_throw_arraybounds();*((char*)_tmp53C.curr)=_tmp53E;});
({struct _fat_ptr*_tmp907=({unsigned _tmp540=1;struct _fat_ptr*_tmp53F=_cycalloc(_check_times(_tmp540,sizeof(struct _fat_ptr)));_tmp53F[0]=(struct _fat_ptr)s;_tmp53F;});t->name=_tmp907;});}}
# 2053
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2055
void*_tmp542=t;void*_tmp546;struct Cyc_Absyn_Typedefdecl*_tmp545;struct Cyc_List_List*_tmp544;struct _tuple0*_tmp543;int _tmp549;void*_tmp548;struct Cyc_Core_Opt*_tmp547;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_List_List*_tmp552;struct Cyc_List_List*_tmp551;struct Cyc_Absyn_VarargInfo*_tmp550;int _tmp54F;struct Cyc_List_List*_tmp54E;void*_tmp54D;struct Cyc_Absyn_Tqual _tmp54C;void*_tmp54B;struct Cyc_List_List*_tmp54A;struct Cyc_Absyn_PtrAtts _tmp557;struct Cyc_Absyn_Tqual _tmp556;void*_tmp555;unsigned _tmp55C;void*_tmp55B;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Tqual _tmp559;void*_tmp558;switch(*((int*)_tmp542)){case 4U: _LL1: _tmp558=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp542)->f1).elt_type;_tmp559=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp542)->f1).tq;_tmp55A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp542)->f1).num_elts;_tmp55B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp542)->f1).zero_term;_tmp55C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp542)->f1).zt_loc;_LL2: {void*t2=_tmp558;struct Cyc_Absyn_Tqual tq2=_tmp559;struct Cyc_Absyn_Exp*e=_tmp55A;void*zeroterm=_tmp55B;unsigned ztl=_tmp55C;
# 2058
struct _tuple13 _tmp55D=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp24=_tmp55D;struct _tuple13 _tmp55E=_stmttmp24;struct Cyc_List_List*_tmp561;void*_tmp560;struct Cyc_Absyn_Tqual _tmp55F;_LLE: _tmp55F=_tmp55E.f1;_tmp560=_tmp55E.f2;_tmp561=_tmp55E.f3;_LLF: {struct Cyc_Absyn_Tqual tq3=_tmp55F;void*t3=_tmp560;struct Cyc_List_List*tml3=_tmp561;
void*tm;
if(e == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp562=_region_malloc(r,sizeof(*_tmp562));_tmp562->tag=0U,_tmp562->f1=zeroterm,_tmp562->f2=ztl;_tmp562;});else{
# 2063
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp563=_region_malloc(r,sizeof(*_tmp563));_tmp563->tag=1U,_tmp563->f1=e,_tmp563->f2=zeroterm,_tmp563->f3=ztl;_tmp563;});}
return({struct _tuple13 _tmp5DE;_tmp5DE.f1=tq3,_tmp5DE.f2=t3,({struct Cyc_List_List*_tmp908=({struct Cyc_List_List*_tmp564=_region_malloc(r,sizeof(*_tmp564));_tmp564->hd=tm,_tmp564->tl=tml3;_tmp564;});_tmp5DE.f3=_tmp908;});_tmp5DE;});}}case 3U: _LL3: _tmp555=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp542)->f1).elt_type;_tmp556=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp542)->f1).elt_tq;_tmp557=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp542)->f1).ptr_atts;_LL4: {void*t2=_tmp555;struct Cyc_Absyn_Tqual tq2=_tmp556;struct Cyc_Absyn_PtrAtts ptratts=_tmp557;
# 2067
struct _tuple13 _tmp565=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp25=_tmp565;struct _tuple13 _tmp566=_stmttmp25;struct Cyc_List_List*_tmp569;void*_tmp568;struct Cyc_Absyn_Tqual _tmp567;_LL11: _tmp567=_tmp566.f1;_tmp568=_tmp566.f2;_tmp569=_tmp566.f3;_LL12: {struct Cyc_Absyn_Tqual tq3=_tmp567;void*t3=_tmp568;struct Cyc_List_List*tml3=_tmp569;
tml3=({struct Cyc_List_List*_tmp56B=_region_malloc(r,sizeof(*_tmp56B));({void*_tmp909=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp56A=_region_malloc(r,sizeof(*_tmp56A));_tmp56A->tag=2U,_tmp56A->f1=ptratts,_tmp56A->f2=tq;_tmp56A;});_tmp56B->hd=_tmp909;}),_tmp56B->tl=tml3;_tmp56B;});
return({struct _tuple13 _tmp5DF;_tmp5DF.f1=tq3,_tmp5DF.f2=t3,_tmp5DF.f3=tml3;_tmp5DF;});}}case 5U: _LL5: _tmp54A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).tvars;_tmp54B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).effect;_tmp54C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).ret_tqual;_tmp54D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).ret_type;_tmp54E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).args;_tmp54F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).c_varargs;_tmp550=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).cyc_varargs;_tmp551=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).rgn_po;_tmp552=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).attributes;_tmp553=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).requires_clause;_tmp554=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp542)->f1).ensures_clause;_LL6: {struct Cyc_List_List*typvars=_tmp54A;void*effopt=_tmp54B;struct Cyc_Absyn_Tqual t2qual=_tmp54C;void*t2=_tmp54D;struct Cyc_List_List*args=_tmp54E;int c_varargs=_tmp54F;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp550;struct Cyc_List_List*rgn_po=_tmp551;struct Cyc_List_List*fn_atts=_tmp552;struct Cyc_Absyn_Exp*req=_tmp553;struct Cyc_Absyn_Exp*ens=_tmp554;
# 2073
if(!Cyc_Absynpp_print_all_tvars){
# 2077
if(effopt == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2082
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2085
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2090
struct _tuple13 _tmp56C=Cyc_Absynpp_to_tms(r,t2qual,t2);struct _tuple13 _stmttmp26=_tmp56C;struct _tuple13 _tmp56D=_stmttmp26;struct Cyc_List_List*_tmp570;void*_tmp56F;struct Cyc_Absyn_Tqual _tmp56E;_LL14: _tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;_tmp570=_tmp56D.f3;_LL15: {struct Cyc_Absyn_Tqual tq3=_tmp56E;void*t3=_tmp56F;struct Cyc_List_List*tml3=_tmp570;
struct Cyc_List_List*tms=tml3;
# 2101 "/tmp/cyclone/src/absynpp.cyc"
{enum Cyc_Flags_C_Compilers _tmp571=Cyc_Flags_c_compiler;if(_tmp571 == Cyc_Flags_Gcc_c){_LL17: _LL18:
# 2103
 if(fn_atts != 0)
# 2106
tms=({struct _RegionHandle*_tmp90B=r;void*_tmp90A=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp572=_region_malloc(r,sizeof(*_tmp572));_tmp572->tag=5U,_tmp572->f1=0U,_tmp572->f2=fn_atts;_tmp572;});Cyc_Absynpp_bubble_attributes(_tmp90B,_tmp90A,tms);});
# 2108
tms=({struct Cyc_List_List*_tmp575=_region_malloc(r,sizeof(*_tmp575));({void*_tmp90D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp574=_region_malloc(r,sizeof(*_tmp574));
_tmp574->tag=3U,({void*_tmp90C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp573=_region_malloc(r,sizeof(*_tmp573));_tmp573->tag=1U,_tmp573->f1=args,_tmp573->f2=c_varargs,_tmp573->f3=cyc_varargs,_tmp573->f4=effopt,_tmp573->f5=rgn_po,_tmp573->f6=req,_tmp573->f7=ens;_tmp573;});_tmp574->f1=_tmp90C;});_tmp574;});
# 2108
_tmp575->hd=_tmp90D;}),_tmp575->tl=tms;_tmp575;});
# 2112
goto _LL16;}else{_LL19: _LL1A:
# 2114
 tms=({struct Cyc_List_List*_tmp578=_region_malloc(r,sizeof(*_tmp578));({void*_tmp90F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp577=_region_malloc(r,sizeof(*_tmp577));
_tmp577->tag=3U,({void*_tmp90E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp576=_region_malloc(r,sizeof(*_tmp576));_tmp576->tag=1U,_tmp576->f1=args,_tmp576->f2=c_varargs,_tmp576->f3=cyc_varargs,_tmp576->f4=effopt,_tmp576->f5=rgn_po,_tmp576->f6=req,_tmp576->f7=ens;_tmp576;});_tmp577->f1=_tmp90E;});_tmp577;});
# 2114
_tmp578->hd=_tmp90F;}),_tmp578->tl=tms;_tmp578;});
# 2118
for(0;fn_atts != 0;fn_atts=fn_atts->tl){
void*_tmp579=(void*)fn_atts->hd;void*_stmttmp27=_tmp579;void*_tmp57A=_stmttmp27;switch(*((int*)_tmp57A)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2123
 tms=({struct Cyc_List_List*_tmp57D=_region_malloc(r,sizeof(*_tmp57D));({void*_tmp911=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp57C=_region_malloc(r,sizeof(*_tmp57C));_tmp57C->tag=5U,_tmp57C->f1=0U,({struct Cyc_List_List*_tmp910=({struct Cyc_List_List*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->hd=(void*)fn_atts->hd,_tmp57B->tl=0;_tmp57B;});_tmp57C->f2=_tmp910;});_tmp57C;});_tmp57D->hd=_tmp911;}),_tmp57D->tl=tms;_tmp57D;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2127
goto _LL16;}_LL16:;}
# 2130
AfterAtts:
 if(typvars != 0)
tms=({struct Cyc_List_List*_tmp57F=_region_malloc(r,sizeof(*_tmp57F));({void*_tmp912=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp57E=_region_malloc(r,sizeof(*_tmp57E));_tmp57E->tag=4U,_tmp57E->f1=typvars,_tmp57E->f2=0U,_tmp57E->f3=1;_tmp57E;});_tmp57F->hd=_tmp912;}),_tmp57F->tl=tms;_tmp57F;});
return({struct _tuple13 _tmp5E0;_tmp5E0.f1=tq3,_tmp5E0.f2=t3,_tmp5E0.f3=tms;_tmp5E0;});}}}case 1U: _LL7: _tmp547=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp542)->f1;_tmp548=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp542)->f2;_tmp549=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp542)->f3;_LL8: {struct Cyc_Core_Opt*k=_tmp547;void*topt=_tmp548;int i=_tmp549;
# 2136
if(topt == 0)
return({struct _tuple13 _tmp5E1;_tmp5E1.f1=tq,_tmp5E1.f2=t,_tmp5E1.f3=0;_tmp5E1;});else{
# 2139
return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8U: _LL9: _tmp543=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp542)->f1;_tmp544=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp542)->f2;_tmp545=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp542)->f3;_tmp546=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp542)->f4;_LLA: {struct _tuple0*n=_tmp543;struct Cyc_List_List*ts=_tmp544;struct Cyc_Absyn_Typedefdecl*td=_tmp545;void*topt=_tmp546;
# 2145
if(topt == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple13 _tmp5E2;_tmp5E2.f1=tq,_tmp5E2.f2=t,_tmp5E2.f3=0;_tmp5E2;});else{
# 2148
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}}default: _LLB: _LLC:
# 2153
 return({struct _tuple13 _tmp5E3;_tmp5E3.f1=tq,_tmp5E3.f2=t,_tmp5E3.f3=0;_tmp5E3;});}_LL0:;}
# 2157
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2159
void*_tmp580=t;void*_tmp581;void*_tmp582;switch(*((int*)_tmp580)){case 1U: _LL1: _tmp582=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp580)->f2;if(_tmp582 != 0){_LL2: {void*def=_tmp582;
return Cyc_Absynpp_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp581=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp580)->f1).elt_type;_LL4: {void*elt_typ=_tmp581;
# 2162
L: {
void*_tmp583=elt_typ;void*_tmp584;void*_tmp585;switch(*((int*)_tmp583)){case 1U: _LL8: _tmp585=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp583)->f2;if(_tmp585 != 0){_LL9: {void*t=_tmp585;
elt_typ=t;goto L;}}else{goto _LLE;}case 8U: _LLA: _tmp584=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp583)->f4;if(_tmp584 != 0){_LLB: {void*t=_tmp584;
elt_typ=t;goto L;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp583)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp583)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 2169
 return 0;}_LL0:;}
# 2173
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp586=_new_region("temp");struct _RegionHandle*temp=& _tmp586;_push_region(temp);
{struct _tuple13 _tmp587=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple13 _stmttmp28=_tmp587;struct _tuple13 _tmp588=_stmttmp28;struct Cyc_List_List*_tmp58B;void*_tmp58A;struct Cyc_Absyn_Tqual _tmp589;_LL1: _tmp589=_tmp588.f1;_tmp58A=_tmp588.f2;_tmp58B=_tmp588.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp589;void*t=_tmp58A;struct Cyc_List_List*tms=_tmp58B;
tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);
if(tms == 0 && dopt == 0){
struct Cyc_PP_Doc*_tmp58D=({struct Cyc_PP_Doc*_tmp58C[2U];({struct Cyc_PP_Doc*_tmp914=Cyc_Absynpp_tqual2doc(tq);_tmp58C[0]=_tmp914;}),({struct Cyc_PP_Doc*_tmp913=Cyc_Absynpp_ntyp2doc(t);_tmp58C[1]=_tmp913;});Cyc_PP_cat(_tag_fat(_tmp58C,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp58D;}else{
# 2180
struct Cyc_PP_Doc*_tmp590=({struct Cyc_PP_Doc*_tmp58E[4U];({
struct Cyc_PP_Doc*_tmp91A=Cyc_Absynpp_tqual2doc(tq);_tmp58E[0]=_tmp91A;}),({
struct Cyc_PP_Doc*_tmp919=Cyc_Absynpp_ntyp2doc(t);_tmp58E[1]=_tmp919;}),({
struct Cyc_PP_Doc*_tmp918=Cyc_PP_text(({const char*_tmp58F=" ";_tag_fat(_tmp58F,sizeof(char),2U);}));_tmp58E[2]=_tmp918;}),({
struct Cyc_PP_Doc*_tmp917=({int _tmp916=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp915=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp916,_tmp915,tms);});_tmp58E[3]=_tmp917;});Cyc_PP_cat(_tag_fat(_tmp58E,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp590;}}}
# 2175
;_pop_region();}
# 2188
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp591=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp591;
if((unsigned)req)
requires_doc=({struct Cyc_PP_Doc*_tmp592[2U];({struct Cyc_PP_Doc*_tmp91C=Cyc_PP_text(({const char*_tmp593="@requires ";_tag_fat(_tmp593,sizeof(char),11U);}));_tmp592[0]=_tmp91C;}),({struct Cyc_PP_Doc*_tmp91B=Cyc_Absynpp_exp2doc(req);_tmp592[1]=_tmp91B;});Cyc_PP_cat(_tag_fat(_tmp592,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2194
requires_doc=Cyc_PP_nil_doc();}{
# 2196
enum Cyc_Flags_C_Compilers _tmp594=Cyc_Flags_c_compiler;if(_tmp594 == Cyc_Flags_Gcc_c){_LL1: _LL2:
# 2199
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp595[5U];({struct Cyc_PP_Doc*_tmp924=({struct Cyc_Absyn_Tqual _tmp923=f->tq;void*_tmp922=f->type;Cyc_Absynpp_tqtd2doc(_tmp923,_tmp922,({struct Cyc_Core_Opt*_tmp596=_cycalloc(sizeof(*_tmp596));({struct Cyc_PP_Doc*_tmp921=Cyc_PP_textptr(f->name);_tmp596->v=_tmp921;});_tmp596;}));});_tmp595[0]=_tmp924;}),({
struct Cyc_PP_Doc*_tmp920=Cyc_PP_text(({const char*_tmp597=":";_tag_fat(_tmp597,sizeof(char),2U);}));_tmp595[1]=_tmp920;}),({struct Cyc_PP_Doc*_tmp91F=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp595[2]=_tmp91F;}),({
struct Cyc_PP_Doc*_tmp91E=Cyc_Absynpp_atts2doc(f->attributes);_tmp595[3]=_tmp91E;}),({struct Cyc_PP_Doc*_tmp91D=Cyc_PP_text(({const char*_tmp598=";";_tag_fat(_tmp598,sizeof(char),2U);}));_tmp595[4]=_tmp91D;});Cyc_PP_cat(_tag_fat(_tmp595,sizeof(struct Cyc_PP_Doc*),5U));});
return({struct Cyc_PP_Doc*_tmp599[4U];({struct Cyc_PP_Doc*_tmp92A=({struct Cyc_Absyn_Tqual _tmp929=f->tq;void*_tmp928=f->type;Cyc_Absynpp_tqtd2doc(_tmp929,_tmp928,({struct Cyc_Core_Opt*_tmp59A=_cycalloc(sizeof(*_tmp59A));({struct Cyc_PP_Doc*_tmp927=Cyc_PP_textptr(f->name);_tmp59A->v=_tmp927;});_tmp59A;}));});_tmp599[0]=_tmp92A;}),({
struct Cyc_PP_Doc*_tmp926=Cyc_Absynpp_atts2doc(f->attributes);_tmp599[1]=_tmp926;}),_tmp599[2]=requires_doc,({struct Cyc_PP_Doc*_tmp925=Cyc_PP_text(({const char*_tmp59B=";";_tag_fat(_tmp59B,sizeof(char),2U);}));_tmp599[3]=_tmp925;});Cyc_PP_cat(_tag_fat(_tmp599,sizeof(struct Cyc_PP_Doc*),4U));});}else{_LL3: _LL4:
# 2206
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp59C[5U];({struct Cyc_PP_Doc*_tmp932=Cyc_Absynpp_atts2doc(f->attributes);_tmp59C[0]=_tmp932;}),({
struct Cyc_PP_Doc*_tmp931=({struct Cyc_Absyn_Tqual _tmp930=f->tq;void*_tmp92F=f->type;Cyc_Absynpp_tqtd2doc(_tmp930,_tmp92F,({struct Cyc_Core_Opt*_tmp59D=_cycalloc(sizeof(*_tmp59D));({struct Cyc_PP_Doc*_tmp92E=Cyc_PP_textptr(f->name);_tmp59D->v=_tmp92E;});_tmp59D;}));});_tmp59C[1]=_tmp931;}),({
struct Cyc_PP_Doc*_tmp92D=Cyc_PP_text(({const char*_tmp59E=":";_tag_fat(_tmp59E,sizeof(char),2U);}));_tmp59C[2]=_tmp92D;}),({struct Cyc_PP_Doc*_tmp92C=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp59C[3]=_tmp92C;}),({struct Cyc_PP_Doc*_tmp92B=Cyc_PP_text(({const char*_tmp59F=";";_tag_fat(_tmp59F,sizeof(char),2U);}));_tmp59C[4]=_tmp92B;});Cyc_PP_cat(_tag_fat(_tmp59C,sizeof(struct Cyc_PP_Doc*),5U));});
return({struct Cyc_PP_Doc*_tmp5A0[4U];({struct Cyc_PP_Doc*_tmp938=Cyc_Absynpp_atts2doc(f->attributes);_tmp5A0[0]=_tmp938;}),({
struct Cyc_PP_Doc*_tmp937=({struct Cyc_Absyn_Tqual _tmp936=f->tq;void*_tmp935=f->type;Cyc_Absynpp_tqtd2doc(_tmp936,_tmp935,({struct Cyc_Core_Opt*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));({struct Cyc_PP_Doc*_tmp934=Cyc_PP_textptr(f->name);_tmp5A1->v=_tmp934;});_tmp5A1;}));});_tmp5A0[1]=_tmp937;}),_tmp5A0[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp933=Cyc_PP_text(({const char*_tmp5A2=";";_tag_fat(_tmp5A2,sizeof(char),2U);}));_tmp5A0[3]=_tmp933;});Cyc_PP_cat(_tag_fat(_tmp5A0,sizeof(struct Cyc_PP_Doc*),4U));});}_LL0:;}}
# 2217
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp939=({const char*_tmp5A3="";_tag_fat(_tmp5A3,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp939,fields);});}
# 2221
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp5A4[3U];({struct Cyc_PP_Doc*_tmp93D=Cyc_Absynpp_scope2doc(f->sc);_tmp5A4[0]=_tmp93D;}),({struct Cyc_PP_Doc*_tmp93C=Cyc_Absynpp_typedef_name2doc(f->name);_tmp5A4[1]=_tmp93C;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp93B=Cyc_PP_nil_doc();_tmp5A4[2]=_tmp93B;}):({struct Cyc_PP_Doc*_tmp93A=Cyc_Absynpp_args2doc(f->typs);_tmp5A4[2]=_tmp93A;});Cyc_PP_cat(_tag_fat(_tmp5A4,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2226
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp93E=({const char*_tmp5A5=",";_tag_fat(_tmp5A5,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp93E,fields);});}
# 2235
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp93F=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp93F,72,f);});
({void*_tmp5A6=0U;({struct Cyc___cycFILE*_tmp941=f;struct _fat_ptr _tmp940=({const char*_tmp5A7="\n";_tag_fat(_tmp5A7,sizeof(char),2U);});Cyc_fprintf(_tmp941,_tmp940,_tag_fat(_tmp5A6,sizeof(void*),0U));});});}}
# 2242
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _tmp942=({const char*_tmp5A8="";_tag_fat(_tmp5A8,sizeof(char),1U);});Cyc_PP_seql(_tmp942,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2245
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
# 2259
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
# 2264
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*t){
if(t == 0)
return(struct _fat_ptr)({const char*_tmp5A9="NULL";_tag_fat(_tmp5A9,sizeof(char),5U);});{
void*_tmp5AA=t;struct Cyc_Absyn_Typedefdecl*_tmp5AB;switch(*((int*)_tmp5AA)){case 0U: _LL1: _LL2:
 return(struct _fat_ptr)({const char*_tmp5AC="AppType";_tag_fat(_tmp5AC,sizeof(char),8U);});case 1U: _LL3: _LL4:
 return(struct _fat_ptr)({const char*_tmp5AD="Evar";_tag_fat(_tmp5AD,sizeof(char),5U);});case 2U: _LL5: _LL6:
 return(struct _fat_ptr)({const char*_tmp5AE="Vartype";_tag_fat(_tmp5AE,sizeof(char),8U);});case 3U: _LL7: _LL8:
 return(struct _fat_ptr)({const char*_tmp5AF="Pointertype";_tag_fat(_tmp5AF,sizeof(char),12U);});case 4U: _LL9: _LLA:
 return(struct _fat_ptr)({const char*_tmp5B0="ArrayType";_tag_fat(_tmp5B0,sizeof(char),10U);});case 5U: _LLB: _LLC:
 return(struct _fat_ptr)({const char*_tmp5B1="FnType";_tag_fat(_tmp5B1,sizeof(char),7U);});case 6U: _LLD: _LLE:
 return(struct _fat_ptr)({const char*_tmp5B2="TupleType";_tag_fat(_tmp5B2,sizeof(char),10U);});case 7U: _LLF: _LL10:
 return(struct _fat_ptr)({const char*_tmp5B3="AnonAggrType";_tag_fat(_tmp5B3,sizeof(char),13U);});case 8U: _LL11: _tmp5AB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5AA)->f3;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp5AB;
# 2277
return(struct _fat_ptr)({const char*_tmp5B4="Typedeftype";_tag_fat(_tmp5B4,sizeof(char),12U);});}case 9U: _LL13: _LL14:
 return(struct _fat_ptr)({const char*_tmp5B5="Valueoftype";_tag_fat(_tmp5B5,sizeof(char),12U);});case 10U: _LL15: _LL16:
 return(struct _fat_ptr)({const char*_tmp5B6="Typedecltype";_tag_fat(_tmp5B6,sizeof(char),13U);});default: _LL17: _LL18:
 return(struct _fat_ptr)({const char*_tmp5B7="Typeoftype";_tag_fat(_tmp5B7,sizeof(char),11U);});}_LL0:;}}

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
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 885
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 895
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 900
void*Cyc_Absyn_compress_kb(void*);
# 904
void*Cyc_Absyn_compress(void*);
# 909
int Cyc_Absyn_type2bool(int def,void*);
# 918
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1153
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1161
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 257 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 73 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 77
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_EvalOrder =11U,Cyc_Flags_CCompiler =12U,Cyc_Flags_AllPasses =13U};
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
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp5E6=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5E6,({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=v,_tmp2->tl=0;_tmp2;}));});}
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
return({struct _fat_ptr _tmp5E9=({const char*_tmp70="";_tag_fat(_tmp70,sizeof(char),1U);});struct _fat_ptr _tmp5E8=({const char*_tmp71=" ";_tag_fat(_tmp71,sizeof(char),2U);});struct _fat_ptr _tmp5E7=({const char*_tmp72=" ";_tag_fat(_tmp72,sizeof(char),2U);});Cyc_PP_egroup(_tmp5E9,_tmp5E8,_tmp5E7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 370
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp73=ka;enum Cyc_Absyn_AliasQual _tmp75;enum Cyc_Absyn_KindQual _tmp74;_LL1: _tmp74=_tmp73->kind;_tmp75=_tmp73->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp74;enum Cyc_Absyn_AliasQual a=_tmp75;
enum Cyc_Absyn_KindQual _tmp76=k;switch(_tmp76){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 374
enum Cyc_Absyn_AliasQual _tmp77=a;switch(_tmp77){case Cyc_Absyn_Aliasable: _LL17: _LL18:
 return({const char*_tmp78="A";_tag_fat(_tmp78,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL19: _LL1A:
 return({const char*_tmp79="UA";_tag_fat(_tmp79,sizeof(char),3U);});case Cyc_Absyn_Top: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return({const char*_tmp7A="TA";_tag_fat(_tmp7A,sizeof(char),3U);});}_LL16:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 380
enum Cyc_Absyn_AliasQual _tmp7B=a;switch(_tmp7B){case Cyc_Absyn_Aliasable: _LL20: _LL21:
 return({const char*_tmp7C="M";_tag_fat(_tmp7C,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL22: _LL23:
 return({const char*_tmp7D="UM";_tag_fat(_tmp7D,sizeof(char),3U);});case Cyc_Absyn_Top: _LL24: _LL25:
 goto _LL27;default: _LL26: _LL27: return({const char*_tmp7E="TM";_tag_fat(_tmp7E,sizeof(char),3U);});}_LL1F:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 386
enum Cyc_Absyn_AliasQual _tmp7F=a;switch(_tmp7F){case Cyc_Absyn_Aliasable: _LL29: _LL2A:
 return({const char*_tmp80="B";_tag_fat(_tmp80,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL2B: _LL2C:
 return({const char*_tmp81="UB";_tag_fat(_tmp81,sizeof(char),3U);});case Cyc_Absyn_Top: _LL2D: _LL2E:
 goto _LL30;default: _LL2F: _LL30: return({const char*_tmp82="TB";_tag_fat(_tmp82,sizeof(char),3U);});}_LL28:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 392
enum Cyc_Absyn_AliasQual _tmp83=a;switch(_tmp83){case Cyc_Absyn_Aliasable: _LL32: _LL33:
 return({const char*_tmp84="R";_tag_fat(_tmp84,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL34: _LL35:
 return({const char*_tmp85="UR";_tag_fat(_tmp85,sizeof(char),3U);});case Cyc_Absyn_Top: _LL36: _LL37:
 goto _LL39;default: _LL38: _LL39: return({const char*_tmp86="TR";_tag_fat(_tmp86,sizeof(char),3U);});}_LL31:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 397
 return({const char*_tmp87="E";_tag_fat(_tmp87,sizeof(char),2U);});case Cyc_Absyn_IntKind: _LLE: _LLF:
 return({const char*_tmp88="I";_tag_fat(_tmp88,sizeof(char),2U);});case Cyc_Absyn_BoolKind: _LL10: _LL11:
 return({const char*_tmp89="BOOL";_tag_fat(_tmp89,sizeof(char),5U);});case Cyc_Absyn_PtrBndKind: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15: return({const char*_tmp8A="PTRBND";_tag_fat(_tmp8A,sizeof(char),7U);});}_LL3:;}}
# 403
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 405
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp8B=Cyc_Absyn_compress_kb(c);void*_stmttmp2=_tmp8B;void*_tmp8C=_stmttmp2;struct Cyc_Absyn_Kind*_tmp8D;struct Cyc_Absyn_Kind*_tmp8E;switch(*((int*)_tmp8C)){case 0U: _LL1: _tmp8E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8C)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp8E;
return Cyc_Absynpp_kind2string(k);}case 1U: _LL3: _LL4:
# 409
 if(Cyc_PP_tex_output)
return({const char*_tmp8F="{?}";_tag_fat(_tmp8F,sizeof(char),4U);});else{
return({const char*_tmp90="?";_tag_fat(_tmp90,sizeof(char),2U);});}default: _LL5: _tmp8D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8C)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp8D;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp93=({struct Cyc_String_pa_PrintArg_struct _tmp5BA;_tmp5BA.tag=0U,({struct _fat_ptr _tmp5EA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k));_tmp5BA.f1=_tmp5EA;});_tmp5BA;});void*_tmp91[1U];_tmp91[0]=& _tmp93;({struct _fat_ptr _tmp5EB=({const char*_tmp92="<=%s";_tag_fat(_tmp92,sizeof(char),5U);});Cyc_aprintf(_tmp5EB,_tag_fat(_tmp91,sizeof(void*),1U));});});}}_LL0:;}
# 416
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp94=Cyc_Absyn_compress_kb(c);void*_stmttmp3=_tmp94;void*_tmp95=_stmttmp3;struct Cyc_Absyn_Kind*_tmp96;struct Cyc_Absyn_Kind*_tmp97;switch(*((int*)_tmp95)){case 0U: _LL1: _tmp97=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp97;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1U: _LL3: _LL4:
# 420
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp98="{?}";_tag_fat(_tmp98,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp99="?";_tag_fat(_tmp99,sizeof(char),2U);}));}default: _LL5: _tmp96=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp95)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp96;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}}_LL0:;}
# 427
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp5EE=({const char*_tmp9A="<";_tag_fat(_tmp9A,sizeof(char),2U);});struct _fat_ptr _tmp5ED=({const char*_tmp9B=">";_tag_fat(_tmp9B,sizeof(char),2U);});struct _fat_ptr _tmp5EC=({const char*_tmp9C=",";_tag_fat(_tmp9C,sizeof(char),2U);});Cyc_PP_egroup(_tmp5EE,_tmp5ED,_tmp5EC,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 431
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr _tmp9D=*tv->name;struct _fat_ptr n=_tmp9D;
# 435
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
# 437
struct _fat_ptr kstring=({const char*_tmpA6="K";_tag_fat(_tmpA6,sizeof(char),2U);});
{void*_tmp9E=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp4=_tmp9E;void*_tmp9F=_stmttmp4;struct Cyc_Absyn_Kind*_tmpA0;struct Cyc_Absyn_Kind*_tmpA1;switch(*((int*)_tmp9F)){case 2U: _LL1: _tmpA1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9F)->f2;_LL2: {struct Cyc_Absyn_Kind*k=_tmpA1;
_tmpA0=k;goto _LL4;}case 0U: _LL3: _tmpA0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9F)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_tmpA0;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 443
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp5BC;_tmp5BC.tag=0U,_tmp5BC.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_tmp5BC;});struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp5BB;_tmp5BB.tag=0U,({struct _fat_ptr _tmp5EF=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(n,sizeof(char),1));_tmp5BB.f1=_tmp5EF;});_tmp5BB;});void*_tmpA2[2U];_tmpA2[0]=& _tmpA4,_tmpA2[1]=& _tmpA5;({struct _fat_ptr _tmp5F0=({const char*_tmpA3="`G%s%s";_tag_fat(_tmpA3,sizeof(char),7U);});Cyc_aprintf(_tmp5F0,_tag_fat(_tmpA2,sizeof(void*),2U));});}));}
# 445
return Cyc_PP_text(n);}
# 448
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA7=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp5=_tmpA7;void*_tmpA8=_stmttmp5;struct Cyc_Absyn_Kind*_tmpA9;struct Cyc_Absyn_Kind*_tmpAA;switch(*((int*)_tmpA8)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA8)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA8)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpAA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA8)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmpAA;
# 453
return({struct Cyc_PP_Doc*_tmpAB[3U];({struct Cyc_PP_Doc*_tmp5F3=Cyc_Absynpp_tvar2doc(tv);_tmpAB[0]=_tmp5F3;}),({struct Cyc_PP_Doc*_tmp5F2=Cyc_PP_text(({const char*_tmpAC="::";_tag_fat(_tmpAC,sizeof(char),3U);}));_tmpAB[1]=_tmp5F2;}),({struct Cyc_PP_Doc*_tmp5F1=Cyc_Absynpp_kind2doc(k);_tmpAB[2]=_tmp5F1;});Cyc_PP_cat(_tag_fat(_tmpAB,sizeof(struct Cyc_PP_Doc*),3U));});}}default: _LL5: _tmpA9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmpA9;
# 452
_tmpAA=k;goto _LL8;}}_LL0:;}
# 457
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _tmp5F6=({const char*_tmpAD="<";_tag_fat(_tmpAD,sizeof(char),2U);});struct _fat_ptr _tmp5F5=({const char*_tmpAE=">";_tag_fat(_tmpAE,sizeof(char),2U);});struct _fat_ptr _tmp5F4=({const char*_tmpAF=",";_tag_fat(_tmpAF,sizeof(char),2U);});Cyc_PP_egroup(_tmp5F6,_tmp5F5,_tmp5F4,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 461
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _tmp5F9=({const char*_tmpB0="<";_tag_fat(_tmpB0,sizeof(char),2U);});struct _fat_ptr _tmp5F8=({const char*_tmpB1=">";_tag_fat(_tmpB1,sizeof(char),2U);});struct _fat_ptr _tmp5F7=({const char*_tmpB2=",";_tag_fat(_tmpB2,sizeof(char),2U);});Cyc_PP_egroup(_tmp5F9,_tmp5F8,_tmp5F7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 467
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple17*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 471
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp5FC=({const char*_tmpB3="(";_tag_fat(_tmpB3,sizeof(char),2U);});struct _fat_ptr _tmp5FB=({const char*_tmpB4=")";_tag_fat(_tmpB4,sizeof(char),2U);});struct _fat_ptr _tmp5FA=({const char*_tmpB5=",";_tag_fat(_tmpB5,sizeof(char),2U);});Cyc_PP_group(_tmp5FC,_tmp5FB,_tmp5FA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 475
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB6=att;switch(*((int*)_tmpB6)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();default: _LL7: _LL8:
 return Cyc_PP_text(Cyc_Absynpp_attribute2string(att));}_LL0:;}
# 484
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB7=(void*)atts->hd;void*_stmttmp6=_tmpB7;void*_tmpB8=_stmttmp6;switch(*((int*)_tmpB8)){case 1U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmpB9=" _stdcall ";_tag_fat(_tmpB9,sizeof(char),11U);}));case 2U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmpBA=" _cdecl ";_tag_fat(_tmpBA,sizeof(char),9U);}));case 3U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmpBB=" _fastcall ";_tag_fat(_tmpBB,sizeof(char),12U);}));default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 492
return Cyc_PP_nil_doc();}
# 495
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpBC=(void*)atts2->hd;void*_stmttmp7=_tmpBC;void*_tmpBD=_stmttmp7;switch(*((int*)_tmpBD)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 504
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBE[3U];({struct Cyc_PP_Doc*_tmp602=Cyc_PP_text(({const char*_tmpBF=" __declspec(";_tag_fat(_tmpBF,sizeof(char),13U);}));_tmpBE[0]=_tmp602;}),({
struct Cyc_PP_Doc*_tmp601=({struct _fat_ptr _tmp600=({const char*_tmpC0="";_tag_fat(_tmpC0,sizeof(char),1U);});struct _fat_ptr _tmp5FF=({const char*_tmpC1="";_tag_fat(_tmpC1,sizeof(char),1U);});struct _fat_ptr _tmp5FE=({const char*_tmpC2=" ";_tag_fat(_tmpC2,sizeof(char),2U);});Cyc_PP_group(_tmp600,_tmp5FF,_tmp5FE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBE[1]=_tmp601;}),({
struct Cyc_PP_Doc*_tmp5FD=Cyc_PP_text(({const char*_tmpC3=")";_tag_fat(_tmpC3,sizeof(char),2U);}));_tmpBE[2]=_tmp5FD;});Cyc_PP_cat(_tag_fat(_tmpBE,sizeof(struct Cyc_PP_Doc*),3U));});}
# 511
struct _fat_ptr Cyc_Absynpp_attribute2string(void*a){
void*_tmpC4=a;struct _fat_ptr _tmpC5;struct _fat_ptr _tmpC6;int _tmpC7;int _tmpC8;int _tmpC9;int _tmpCB;int _tmpCA;int _tmpCD;int _tmpCC;struct _fat_ptr _tmpCE;struct Cyc_Absyn_Exp*_tmpCF;int _tmpD0;switch(*((int*)_tmpC4)){case 0U: _LL1: _tmpD0=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpC4)->f1;_LL2: {int i=_tmpD0;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD3=({struct Cyc_Int_pa_PrintArg_struct _tmp5BD;_tmp5BD.tag=1U,_tmp5BD.f1=(unsigned long)i;_tmp5BD;});void*_tmpD1[1U];_tmpD1[0]=& _tmpD3;({struct _fat_ptr _tmp603=({const char*_tmpD2="regparm(%d)";_tag_fat(_tmpD2,sizeof(char),12U);});Cyc_aprintf(_tmp603,_tag_fat(_tmpD1,sizeof(void*),1U));});});}case 1U: _LL3: _LL4:
 return({const char*_tmpD4="stdcall";_tag_fat(_tmpD4,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmpD5="cdecl";_tag_fat(_tmpD5,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmpD6="fastcall";_tag_fat(_tmpD6,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmpD7="noreturn";_tag_fat(_tmpD7,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmpD8="const";_tag_fat(_tmpD8,sizeof(char),6U);});case 6U: _LLD: _tmpCF=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpC4)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmpCF;
# 520
if(e == 0)return({const char*_tmpD9="aligned";_tag_fat(_tmpD9,sizeof(char),8U);});else{
# 522
enum Cyc_Flags_C_Compilers _tmpDA=Cyc_Flags_c_compiler;switch(_tmpDA){case Cyc_Flags_Gcc_c: _LL3C: _LL3D:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpDD=({struct Cyc_String_pa_PrintArg_struct _tmp5BE;_tmp5BE.tag=0U,({struct _fat_ptr _tmp604=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5BE.f1=_tmp604;});_tmp5BE;});void*_tmpDB[1U];_tmpDB[0]=& _tmpDD;({struct _fat_ptr _tmp605=({const char*_tmpDC="aligned(%s)";_tag_fat(_tmpDC,sizeof(char),12U);});Cyc_aprintf(_tmp605,_tag_fat(_tmpDB,sizeof(void*),1U));});});case Cyc_Flags_Vc_c: _LL3E: _LL3F:
 goto _LL41;default: _LL40: _LL41:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE0=({struct Cyc_String_pa_PrintArg_struct _tmp5BF;_tmp5BF.tag=0U,({struct _fat_ptr _tmp606=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5BF.f1=_tmp606;});_tmp5BF;});void*_tmpDE[1U];_tmpDE[0]=& _tmpE0;({struct _fat_ptr _tmp607=({const char*_tmpDF="align(%s)";_tag_fat(_tmpDF,sizeof(char),10U);});Cyc_aprintf(_tmp607,_tag_fat(_tmpDE,sizeof(void*),1U));});});}_LL3B:;}}case 7U: _LLF: _LL10:
# 527
 return({const char*_tmpE1="packed";_tag_fat(_tmpE1,sizeof(char),7U);});case 8U: _LL11: _tmpCE=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpC4)->f1;_LL12: {struct _fat_ptr s=_tmpCE;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE4=({struct Cyc_String_pa_PrintArg_struct _tmp5C0;_tmp5C0.tag=0U,_tmp5C0.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5C0;});void*_tmpE2[1U];_tmpE2[0]=& _tmpE4;({struct _fat_ptr _tmp608=({const char*_tmpE3="section(\"%s\")";_tag_fat(_tmpE3,sizeof(char),14U);});Cyc_aprintf(_tmp608,_tag_fat(_tmpE2,sizeof(void*),1U));});});}case 9U: _LL13: _LL14:
 return({const char*_tmpE5="nocommon";_tag_fat(_tmpE5,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmpE6="shared";_tag_fat(_tmpE6,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmpE7="unused";_tag_fat(_tmpE7,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmpE8="weak";_tag_fat(_tmpE8,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmpE9="dllimport";_tag_fat(_tmpE9,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmpEA="dllexport";_tag_fat(_tmpEA,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmpEB="no_instrument_function";_tag_fat(_tmpEB,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmpEC="constructor";_tag_fat(_tmpEC,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmpED="destructor";_tag_fat(_tmpED,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmpEE="no_check_memory_usage";_tag_fat(_tmpEE,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC4)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmpCC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC4)->f2;_tmpCD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC4)->f3;_LL28: {int n=_tmpCC;int m=_tmpCD;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF1=({struct Cyc_Int_pa_PrintArg_struct _tmp5C2;_tmp5C2.tag=1U,_tmp5C2.f1=(unsigned)n;_tmp5C2;});struct Cyc_Int_pa_PrintArg_struct _tmpF2=({struct Cyc_Int_pa_PrintArg_struct _tmp5C1;_tmp5C1.tag=1U,_tmp5C1.f1=(unsigned)m;_tmp5C1;});void*_tmpEF[2U];_tmpEF[0]=& _tmpF1,_tmpEF[1]=& _tmpF2;({struct _fat_ptr _tmp609=({const char*_tmpF0="format(printf,%u,%u)";_tag_fat(_tmpF0,sizeof(char),21U);});Cyc_aprintf(_tmp609,_tag_fat(_tmpEF,sizeof(void*),2U));});});}}else{_LL29: _tmpCA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC4)->f2;_tmpCB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC4)->f3;_LL2A: {int n=_tmpCA;int m=_tmpCB;
# 541
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF5=({struct Cyc_Int_pa_PrintArg_struct _tmp5C4;_tmp5C4.tag=1U,_tmp5C4.f1=(unsigned)n;_tmp5C4;});struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp5C3;_tmp5C3.tag=1U,_tmp5C3.f1=(unsigned)m;_tmp5C3;});void*_tmpF3[2U];_tmpF3[0]=& _tmpF5,_tmpF3[1]=& _tmpF6;({struct _fat_ptr _tmp60A=({const char*_tmpF4="format(scanf,%u,%u)";_tag_fat(_tmpF4,sizeof(char),20U);});Cyc_aprintf(_tmp60A,_tag_fat(_tmpF3,sizeof(void*),2U));});});}}case 20U: _LL2B: _tmpC9=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpC4)->f1;_LL2C: {int n=_tmpC9;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF9=({struct Cyc_Int_pa_PrintArg_struct _tmp5C5;_tmp5C5.tag=1U,_tmp5C5.f1=(unsigned long)n;_tmp5C5;});void*_tmpF7[1U];_tmpF7[0]=& _tmpF9;({struct _fat_ptr _tmp60B=({const char*_tmpF8="initializes(%d)";_tag_fat(_tmpF8,sizeof(char),16U);});Cyc_aprintf(_tmp60B,_tag_fat(_tmpF7,sizeof(void*),1U));});});}case 21U: _LL2D: _tmpC8=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpC4)->f1;_LL2E: {int n=_tmpC8;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFC=({struct Cyc_Int_pa_PrintArg_struct _tmp5C6;_tmp5C6.tag=1U,_tmp5C6.f1=(unsigned long)n;_tmp5C6;});void*_tmpFA[1U];_tmpFA[0]=& _tmpFC;({struct _fat_ptr _tmp60C=({const char*_tmpFB="noliveunique(%d)";_tag_fat(_tmpFB,sizeof(char),17U);});Cyc_aprintf(_tmp60C,_tag_fat(_tmpFA,sizeof(void*),1U));});});}case 22U: _LL2F: _tmpC7=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpC4)->f1;_LL30: {int n=_tmpC7;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFF=({struct Cyc_Int_pa_PrintArg_struct _tmp5C7;_tmp5C7.tag=1U,_tmp5C7.f1=(unsigned long)n;_tmp5C7;});void*_tmpFD[1U];_tmpFD[0]=& _tmpFF;({struct _fat_ptr _tmp60D=({const char*_tmpFE="consume(%d)";_tag_fat(_tmpFE,sizeof(char),12U);});Cyc_aprintf(_tmp60D,_tag_fat(_tmpFD,sizeof(void*),1U));});});}case 23U: _LL31: _LL32:
 return({const char*_tmp100="pure";_tag_fat(_tmp100,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmp101="always_inline";_tag_fat(_tmp101,sizeof(char),14U);});case 24U: _LL35: _tmpC6=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpC4)->f1;_LL36: {struct _fat_ptr s=_tmpC6;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp104=({struct Cyc_String_pa_PrintArg_struct _tmp5C8;_tmp5C8.tag=0U,_tmp5C8.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5C8;});void*_tmp102[1U];_tmp102[0]=& _tmp104;({struct _fat_ptr _tmp60E=({const char*_tmp103="__mode__(\"%s\")";_tag_fat(_tmp103,sizeof(char),15U);});Cyc_aprintf(_tmp60E,_tag_fat(_tmp102,sizeof(void*),1U));});});}case 25U: _LL37: _tmpC5=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpC4)->f1;_LL38: {struct _fat_ptr s=_tmpC5;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp107=({struct Cyc_String_pa_PrintArg_struct _tmp5C9;_tmp5C9.tag=0U,_tmp5C9.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5C9;});void*_tmp105[1U];_tmp105[0]=& _tmp107;({struct _fat_ptr _tmp60F=({const char*_tmp106="alias(\"%s\")";_tag_fat(_tmp106,sizeof(char),12U);});Cyc_aprintf(_tmp60F,_tag_fat(_tmp105,sizeof(void*),1U));});});}default: _LL39: _LL3A:
 return({const char*_tmp108="no_throw";_tag_fat(_tmp108,sizeof(char),9U);});}_LL0:;}
# 553
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absynpp_attribute2string(a));}
# 557
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Flags_C_Compilers _tmp109=Cyc_Flags_c_compiler;if(_tmp109 == Cyc_Flags_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 562
 return({struct Cyc_PP_Doc*_tmp10A[2U];({struct Cyc_PP_Doc*_tmp614=Cyc_PP_text(({const char*_tmp10B=" __attribute__";_tag_fat(_tmp10B,sizeof(char),15U);}));_tmp10A[0]=_tmp614;}),({
struct Cyc_PP_Doc*_tmp613=({struct _fat_ptr _tmp612=({const char*_tmp10C="((";_tag_fat(_tmp10C,sizeof(char),3U);});struct _fat_ptr _tmp611=({const char*_tmp10D="))";_tag_fat(_tmp10D,sizeof(char),3U);});struct _fat_ptr _tmp610=({const char*_tmp10E=",";_tag_fat(_tmp10E,sizeof(char),2U);});Cyc_PP_group(_tmp612,_tmp611,_tmp610,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmp10A[1]=_tmp613;});Cyc_PP_cat(_tag_fat(_tmp10A,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}}
# 567
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmp10F=(void*)tms->hd;void*_stmttmp8=_tmp10F;void*_tmp110=_stmttmp8;switch(*((int*)_tmp110)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 572
enum Cyc_Flags_C_Compilers _tmp111=Cyc_Flags_c_compiler;if(_tmp111 == Cyc_Flags_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 576
 return 0;}_LL0:;}}
# 580
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 583
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question (void){
if(!((unsigned)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmp112="{?}";_tag_fat(_tmp112,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmp113="?";_tag_fat(_tmp113,sizeof(char),2U);}));}}
# 590
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 592
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb (void){
if(!((unsigned)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmp114="{\\lb}";_tag_fat(_tmp114,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmp115="{";_tag_fat(_tmp115,sizeof(char),2U);}));}}
# 599
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 601
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb (void){
if(!((unsigned)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmp116="{\\rb}";_tag_fat(_tmp116,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmp117="}";_tag_fat(_tmp117,sizeof(char),2U);}));}}
# 608
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 610
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar (void){
if(!((unsigned)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmp118="\\$";_tag_fat(_tmp118,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmp119="$";_tag_fat(_tmp119,sizeof(char),2U);}));}}
# 617
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 619
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmp11A[3U];({struct Cyc_PP_Doc*_tmp617=Cyc_Absynpp_lb();_tmp11A[0]=_tmp617;}),({struct Cyc_PP_Doc*_tmp616=Cyc_PP_seq(sep,ss);_tmp11A[1]=_tmp616;}),({struct Cyc_PP_Doc*_tmp615=Cyc_Absynpp_rb();_tmp11A[2]=_tmp615;});Cyc_PP_cat(_tag_fat(_tmp11A,sizeof(struct Cyc_PP_Doc*),3U));});}
# 624
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmp11B=(void*)tms->hd;void*_stmttmp9=_tmp11B;void*_tmp11C=_stmttmp9;struct Cyc_List_List*_tmp11D;switch(*((int*)_tmp11C)){case 0U: _LL1: _LL2:
({void*_tmp11E=0U;({struct Cyc___cycFILE*_tmp619=Cyc_stderr;struct _fat_ptr _tmp618=({const char*_tmp11F="Carray_mod ";_tag_fat(_tmp11F,sizeof(char),12U);});Cyc_fprintf(_tmp619,_tmp618,_tag_fat(_tmp11E,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp120=0U;({struct Cyc___cycFILE*_tmp61B=Cyc_stderr;struct _fat_ptr _tmp61A=({const char*_tmp121="ConstArray_mod ";_tag_fat(_tmp121,sizeof(char),16U);});Cyc_fprintf(_tmp61B,_tmp61A,_tag_fat(_tmp120,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp11C)->f1)->tag == 1U){_LL5: _tmp11D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp11C)->f1)->f1;_LL6: {struct Cyc_List_List*ys=_tmp11D;
# 630
({void*_tmp122=0U;({struct Cyc___cycFILE*_tmp61D=Cyc_stderr;struct _fat_ptr _tmp61C=({const char*_tmp123="Function_mod(";_tag_fat(_tmp123,sizeof(char),14U);});Cyc_fprintf(_tmp61D,_tmp61C,_tag_fat(_tmp122,sizeof(void*),0U));});});
for(0;ys != 0;ys=ys->tl){
struct _fat_ptr*_tmp124=(*((struct _tuple8*)ys->hd)).f1;struct _fat_ptr*v=_tmp124;
if(v == 0)({void*_tmp125=0U;({struct Cyc___cycFILE*_tmp61F=Cyc_stderr;struct _fat_ptr _tmp61E=({const char*_tmp126="?";_tag_fat(_tmp126,sizeof(char),2U);});Cyc_fprintf(_tmp61F,_tmp61E,_tag_fat(_tmp125,sizeof(void*),0U));});});else{
({void*_tmp127=0U;({struct Cyc___cycFILE*_tmp621=Cyc_stderr;struct _fat_ptr _tmp620=*v;Cyc_fprintf(_tmp621,_tmp620,_tag_fat(_tmp127,sizeof(void*),0U));});});}
if(ys->tl != 0)({void*_tmp128=0U;({struct Cyc___cycFILE*_tmp623=Cyc_stderr;struct _fat_ptr _tmp622=({const char*_tmp129=",";_tag_fat(_tmp129,sizeof(char),2U);});Cyc_fprintf(_tmp623,_tmp622,_tag_fat(_tmp128,sizeof(void*),0U));});});}
# 637
({void*_tmp12A=0U;({struct Cyc___cycFILE*_tmp625=Cyc_stderr;struct _fat_ptr _tmp624=({const char*_tmp12B=") ";_tag_fat(_tmp12B,sizeof(char),3U);});Cyc_fprintf(_tmp625,_tmp624,_tag_fat(_tmp12A,sizeof(void*),0U));});});
goto _LL0;}}else{_LL7: _LL8:
# 640
({void*_tmp12C=0U;({struct Cyc___cycFILE*_tmp627=Cyc_stderr;struct _fat_ptr _tmp626=({const char*_tmp12D="Function_mod()";_tag_fat(_tmp12D,sizeof(char),15U);});Cyc_fprintf(_tmp627,_tmp626,_tag_fat(_tmp12C,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmp12E=0U;({struct Cyc___cycFILE*_tmp629=Cyc_stderr;struct _fat_ptr _tmp628=({const char*_tmp12F="Attributes_mod ";_tag_fat(_tmp12F,sizeof(char),16U);});Cyc_fprintf(_tmp629,_tmp628,_tag_fat(_tmp12E,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmp130=0U;({struct Cyc___cycFILE*_tmp62B=Cyc_stderr;struct _fat_ptr _tmp62A=({const char*_tmp131="TypeParams_mod ";_tag_fat(_tmp131,sizeof(char),16U);});Cyc_fprintf(_tmp62B,_tmp62A,_tag_fat(_tmp130,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp132=0U;({struct Cyc___cycFILE*_tmp62D=Cyc_stderr;struct _fat_ptr _tmp62C=({const char*_tmp133="Pointer_mod ";_tag_fat(_tmp133,sizeof(char),13U);});Cyc_fprintf(_tmp62D,_tmp62C,_tag_fat(_tmp132,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 645
({void*_tmp134=0U;({struct Cyc___cycFILE*_tmp62F=Cyc_stderr;struct _fat_ptr _tmp62E=({const char*_tmp135="\n";_tag_fat(_tmp135,sizeof(char),2U);});Cyc_fprintf(_tmp62F,_tmp62E,_tag_fat(_tmp134,sizeof(void*),0U));});});}
# 648
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 650
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp17A[3U];({struct Cyc_PP_Doc*_tmp631=Cyc_PP_text(({const char*_tmp17B="(";_tag_fat(_tmp17B,sizeof(char),2U);}));_tmp17A[0]=_tmp631;}),_tmp17A[1]=rest,({struct Cyc_PP_Doc*_tmp630=Cyc_PP_text(({const char*_tmp17C=")";_tag_fat(_tmp17C,sizeof(char),2U);}));_tmp17A[2]=_tmp630;});Cyc_PP_cat(_tag_fat(_tmp17A,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmp136=(void*)tms->hd;void*_stmttmpA=_tmp136;void*_tmp137=_stmttmpA;struct Cyc_Absyn_Tqual _tmp13C;void*_tmp13B;void*_tmp13A;void*_tmp139;void*_tmp138;int _tmp13F;unsigned _tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp140;void*_tmp141;void*_tmp143;struct Cyc_Absyn_Exp*_tmp142;void*_tmp144;switch(*((int*)_tmp137)){case 0U: _LL1: _tmp144=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp137)->f1;_LL2: {void*zeroterm=_tmp144;
# 656
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp145[2U];_tmp145[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_tmp633=Cyc_PP_text(({const char*_tmp146="[]@zeroterm";_tag_fat(_tmp146,sizeof(char),12U);}));_tmp145[1]=_tmp633;}):({struct Cyc_PP_Doc*_tmp632=Cyc_PP_text(({const char*_tmp147="[]";_tag_fat(_tmp147,sizeof(char),3U);}));_tmp145[1]=_tmp632;});Cyc_PP_cat(_tag_fat(_tmp145,sizeof(struct Cyc_PP_Doc*),2U));});}case 1U: _LL3: _tmp142=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp137)->f1;_tmp143=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp137)->f2;_LL4: {struct Cyc_Absyn_Exp*e=_tmp142;void*zeroterm=_tmp143;
# 660
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp148[4U];_tmp148[0]=rest,({struct Cyc_PP_Doc*_tmp637=Cyc_PP_text(({const char*_tmp149="[";_tag_fat(_tmp149,sizeof(char),2U);}));_tmp148[1]=_tmp637;}),({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_exp2doc(e);_tmp148[2]=_tmp636;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_tmp635=Cyc_PP_text(({const char*_tmp14A="]@zeroterm";_tag_fat(_tmp14A,sizeof(char),11U);}));_tmp148[3]=_tmp635;}):({struct Cyc_PP_Doc*_tmp634=Cyc_PP_text(({const char*_tmp14B="]";_tag_fat(_tmp14B,sizeof(char),2U);}));_tmp148[3]=_tmp634;});Cyc_PP_cat(_tag_fat(_tmp148,sizeof(struct Cyc_PP_Doc*),4U));});}case 3U: _LL5: _tmp141=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp137)->f1;_LL6: {void*args=_tmp141;
# 664
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp14C=args;unsigned _tmp14E;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_List_List*_tmp153;void*_tmp152;struct Cyc_Absyn_VarargInfo*_tmp151;int _tmp150;struct Cyc_List_List*_tmp14F;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14C)->tag == 1U){_LLE: _tmp14F=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14C)->f1;_tmp150=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14C)->f2;_tmp151=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14C)->f3;_tmp152=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14C)->f4;_tmp153=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14C)->f5;_tmp154=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14C)->f6;_tmp155=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp14C)->f7;_LLF: {struct Cyc_List_List*args2=_tmp14F;int c_varargs=_tmp150;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp151;void*effopt=_tmp152;struct Cyc_List_List*rgn_po=_tmp153;struct Cyc_Absyn_Exp*req=_tmp154;struct Cyc_Absyn_Exp*ens=_tmp155;
# 667
return({struct Cyc_PP_Doc*_tmp156[2U];_tmp156[0]=rest,({struct Cyc_PP_Doc*_tmp638=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);_tmp156[1]=_tmp638;});Cyc_PP_cat(_tag_fat(_tmp156,sizeof(struct Cyc_PP_Doc*),2U));});}}else{_LL10: _tmp14D=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp14C)->f1;_tmp14E=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp14C)->f2;_LL11: {struct Cyc_List_List*sl=_tmp14D;unsigned loc=_tmp14E;
# 669
return({struct Cyc_PP_Doc*_tmp157[2U];_tmp157[0]=rest,({struct Cyc_PP_Doc*_tmp63C=({struct _fat_ptr _tmp63B=({const char*_tmp158="(";_tag_fat(_tmp158,sizeof(char),2U);});struct _fat_ptr _tmp63A=({const char*_tmp159=")";_tag_fat(_tmp159,sizeof(char),2U);});struct _fat_ptr _tmp639=({const char*_tmp15A=",";_tag_fat(_tmp15A,sizeof(char),2U);});Cyc_PP_group(_tmp63B,_tmp63A,_tmp639,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,sl));});_tmp157[1]=_tmp63C;});Cyc_PP_cat(_tag_fat(_tmp157,sizeof(struct Cyc_PP_Doc*),2U));});}}_LLD:;}}case 5U: _LL7: _tmp140=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp137)->f2;_LL8: {struct Cyc_List_List*atts=_tmp140;
# 672
enum Cyc_Flags_C_Compilers _tmp15B=Cyc_Flags_c_compiler;if(_tmp15B == Cyc_Flags_Gcc_c){_LL13: _LL14:
# 674
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp15C[2U];_tmp15C[0]=rest,({struct Cyc_PP_Doc*_tmp63D=Cyc_Absynpp_atts2doc(atts);_tmp15C[1]=_tmp63D;});Cyc_PP_cat(_tag_fat(_tmp15C,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 678
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp15D[2U];({struct Cyc_PP_Doc*_tmp63E=Cyc_Absynpp_callconv2doc(atts);_tmp15D[0]=_tmp63E;}),_tmp15D[1]=rest;Cyc_PP_cat(_tag_fat(_tmp15D,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp13D=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp137)->f1;_tmp13E=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp137)->f2;_tmp13F=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp137)->f3;_LLA: {struct Cyc_List_List*ts=_tmp13D;unsigned loc=_tmp13E;int print_kinds=_tmp13F;
# 683
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_tmp15E[2U];_tmp15E[0]=rest,({struct Cyc_PP_Doc*_tmp63F=Cyc_Absynpp_ktvars2doc(ts);_tmp15E[1]=_tmp63F;});Cyc_PP_cat(_tag_fat(_tmp15E,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 687
return({struct Cyc_PP_Doc*_tmp15F[2U];_tmp15F[0]=rest,({struct Cyc_PP_Doc*_tmp640=Cyc_Absynpp_tvars2doc(ts);_tmp15F[1]=_tmp640;});Cyc_PP_cat(_tag_fat(_tmp15F,sizeof(struct Cyc_PP_Doc*),2U));});}}default: _LLB: _tmp138=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp137)->f1).rgn;_tmp139=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp137)->f1).nullable;_tmp13A=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp137)->f1).bounds;_tmp13B=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp137)->f1).zero_term;_tmp13C=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp137)->f2;_LLC: {void*rgn=_tmp138;void*nullable=_tmp139;void*bd=_tmp13A;void*zt=_tmp13B;struct Cyc_Absyn_Tqual tq2=_tmp13C;
# 691
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_tmp160=Cyc_Absyn_compress(bd);void*_stmttmpB=_tmp160;void*_tmp161=_stmttmpB;void*_tmp162;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp161)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp161)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp161)->f2 != 0){_LL1A: _tmp162=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp161)->f2)->hd;_LL1B: {void*targ=_tmp162;
# 699
{void*_tmp163=Cyc_Absyn_compress(targ);void*_stmttmpC=_tmp163;void*_tmp164=_stmttmpC;struct Cyc_Absyn_Exp*_tmp165;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp164)->tag == 9U){_LL1F: _tmp165=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp164)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp165;
# 701
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp166="*";_tag_fat(_tmp166,sizeof(char),2U);}):({const char*_tmp167="@";_tag_fat(_tmp167,sizeof(char),2U);}));{
struct _tuple12 _tmp168=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmpD=_tmp168;struct _tuple12 _tmp169=_stmttmpD;int _tmp16B;unsigned _tmp16A;_LL24: _tmp16A=_tmp169.f1;_tmp16B=_tmp169.f2;_LL25: {unsigned val=_tmp16A;int known=_tmp16B;
if(!known || val != (unsigned)1)
ptr=({struct Cyc_PP_Doc*_tmp16C[4U];_tmp16C[0]=ptr,({struct Cyc_PP_Doc*_tmp643=Cyc_Absynpp_lb();_tmp16C[1]=_tmp643;}),({struct Cyc_PP_Doc*_tmp642=Cyc_Absynpp_exp2doc(e);_tmp16C[2]=_tmp642;}),({struct Cyc_PP_Doc*_tmp641=Cyc_Absynpp_rb();_tmp16C[3]=_tmp641;});Cyc_PP_cat(_tag_fat(_tmp16C,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}}}}else{_LL21: _LL22:
# 707
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp16D="*";_tag_fat(_tmp16D,sizeof(char),2U);}):({const char*_tmp16E="@";_tag_fat(_tmp16E,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp16F[4U];_tmp16F[0]=ptr,({struct Cyc_PP_Doc*_tmp646=Cyc_Absynpp_lb();_tmp16F[1]=_tmp646;}),({struct Cyc_PP_Doc*_tmp645=Cyc_Absynpp_typ2doc(targ);_tmp16F[2]=_tmp645;}),({struct Cyc_PP_Doc*_tmp644=Cyc_Absynpp_rb();_tmp16F[3]=_tmp644;});Cyc_PP_cat(_tag_fat(_tmp16F,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}_LL1E:;}
# 711
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 713
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp170="*";_tag_fat(_tmp170,sizeof(char),2U);}):({const char*_tmp171="@";_tag_fat(_tmp171,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp172[4U];_tmp172[0]=ptr,({struct Cyc_PP_Doc*_tmp649=Cyc_Absynpp_lb();_tmp172[1]=_tmp649;}),({struct Cyc_PP_Doc*_tmp648=Cyc_Absynpp_typ2doc(bd);_tmp172[2]=_tmp648;}),({struct Cyc_PP_Doc*_tmp647=Cyc_Absynpp_rb();_tmp172[3]=_tmp647;});Cyc_PP_cat(_tag_fat(_tmp172,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 718
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr && Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp173="@zeroterm";_tag_fat(_tmp173,sizeof(char),10U);}));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp174="@nozeroterm";_tag_fat(_tmp174,sizeof(char),12U);}));}}
# 724
{void*_tmp175=Cyc_Absyn_compress(rgn);void*_stmttmpE=_tmp175;void*_tmp176=_stmttmpE;switch(*((int*)_tmp176)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f1)->tag == 5U){_LL27: _LL28:
 goto _LL26;}else{goto _LL2B;}case 1U: _LL29: if(Cyc_Absynpp_print_for_cycdoc){_LL2A:
 goto _LL26;}else{goto _LL2B;}default: _LL2B: _LL2C:
 rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL26:;}{
# 729
struct Cyc_PP_Doc*spacer1=tqd != mt &&(ztd != mt || rgd != mt)?Cyc_PP_text(({const char*_tmp179=" ";_tag_fat(_tmp179,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp178=" ";_tag_fat(_tmp178,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp177[7U];_tmp177[0]=ptr,_tmp177[1]=ztd,_tmp177[2]=rgd,_tmp177[3]=spacer1,_tmp177[4]=tqd,_tmp177[5]=spacer2,_tmp177[6]=rest;Cyc_PP_cat(_tag_fat(_tmp177,sizeof(struct Cyc_PP_Doc*),7U));});}}}_LL0:;}}
# 735
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp17D=Cyc_Absyn_compress(t);void*_stmttmpF=_tmp17D;void*_tmp17E=_stmttmpF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17E)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp17F="`H";_tag_fat(_tmp17F,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp180="`U";_tag_fat(_tmp180,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp181="`RC";_tag_fat(_tmp181,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 745
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 749
void*_tmp182=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp182;void*_tmp183=_stmttmp10;struct Cyc_List_List*_tmp184;void*_tmp185;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp183)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp183)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp183)->f2 != 0){_LL1: _tmp185=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp183)->f2)->hd;_LL2: {void*r=_tmp185;
({struct Cyc_List_List*_tmp64B=({struct Cyc_List_List*_tmp186=_cycalloc(sizeof(*_tmp186));({struct Cyc_PP_Doc*_tmp64A=Cyc_Absynpp_rgn2doc(r);_tmp186->hd=_tmp64A;}),_tmp186->tl=*rgions;_tmp186;});*rgions=_tmp64B;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp184=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp183)->f2;_LL4: {struct Cyc_List_List*ts=_tmp184;
# 752
for(0;ts != 0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
# 755
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp64D=({struct Cyc_List_List*_tmp187=_cycalloc(sizeof(*_tmp187));({struct Cyc_PP_Doc*_tmp64C=Cyc_Absynpp_typ2doc(t);_tmp187->hd=_tmp64C;}),_tmp187->tl=*effects;_tmp187;});*effects=_tmp64D;});goto _LL0;}_LL0:;}
# 760
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0 && effects != 0)
return({struct _fat_ptr _tmp650=({const char*_tmp188="";_tag_fat(_tmp188,sizeof(char),1U);});struct _fat_ptr _tmp64F=({const char*_tmp189="";_tag_fat(_tmp189,sizeof(char),1U);});struct _fat_ptr _tmp64E=({const char*_tmp18A="+";_tag_fat(_tmp18A,sizeof(char),2U);});Cyc_PP_group(_tmp650,_tmp64F,_tmp64E,effects);});else{
# 768
struct Cyc_PP_Doc*_tmp18B=({struct _fat_ptr _tmp651=({const char*_tmp190=",";_tag_fat(_tmp190,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp651,rgions);});struct Cyc_PP_Doc*doc1=_tmp18B;
return({struct _fat_ptr _tmp655=({const char*_tmp18C="";_tag_fat(_tmp18C,sizeof(char),1U);});struct _fat_ptr _tmp654=({const char*_tmp18D="";_tag_fat(_tmp18D,sizeof(char),1U);});struct _fat_ptr _tmp653=({const char*_tmp18E="+";_tag_fat(_tmp18E,sizeof(char),2U);});Cyc_PP_group(_tmp655,_tmp654,_tmp653,({struct Cyc_List_List*_tmp652=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp652,({struct Cyc_List_List*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->hd=doc1,_tmp18F->tl=0;_tmp18F;}));}));});}}
# 773
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp191=k;if(_tmp191 == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp192="struct ";_tag_fat(_tmp192,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp193="union ";_tag_fat(_tmp193,sizeof(char),7U);}));}_LL0:;}
# 781
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp194=t;struct Cyc_Absyn_Datatypedecl*_tmp195;struct Cyc_Absyn_Enumdecl*_tmp196;struct Cyc_Absyn_Aggrdecl*_tmp197;struct Cyc_Absyn_Typedefdecl*_tmp19A;struct Cyc_List_List*_tmp199;struct _tuple0*_tmp198;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_List_List*_tmp19E;enum Cyc_Absyn_AggrKind _tmp19D;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_Tvar*_tmp1A0;struct Cyc_Core_Opt*_tmp1A4;int _tmp1A3;void*_tmp1A2;struct Cyc_Core_Opt*_tmp1A1;void*_tmp1A5;struct Cyc_List_List*_tmp1A6;struct Cyc_List_List*_tmp1A7;struct Cyc_List_List*_tmp1A8;struct _fat_ptr _tmp1A9;struct _tuple0*_tmp1AA;struct Cyc_List_List*_tmp1AB;struct Cyc_List_List*_tmp1AD;union Cyc_Absyn_AggrInfo _tmp1AC;int _tmp1AE;enum Cyc_Absyn_Size_of _tmp1B0;enum Cyc_Absyn_Sign _tmp1AF;struct Cyc_List_List*_tmp1B2;union Cyc_Absyn_DatatypeFieldInfo _tmp1B1;struct Cyc_List_List*_tmp1B4;union Cyc_Absyn_DatatypeInfo _tmp1B3;switch(*((int*)_tmp194)){case 4U: _LL1: _LL2:
# 785
 return Cyc_PP_text(({const char*_tmp1B5="[[[array]]]";_tag_fat(_tmp1B5,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)){case 0U: _LL7: _LL8:
# 789
 s=Cyc_PP_text(({const char*_tmp1B6="void";_tag_fat(_tmp1B6,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp1B3=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f1;_tmp1B4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LLE: {union Cyc_Absyn_DatatypeInfo tu_info=_tmp1B3;struct Cyc_List_List*ts=_tmp1B4;
# 805
{union Cyc_Absyn_DatatypeInfo _tmp1BE=tu_info;int _tmp1C0;struct _tuple0*_tmp1BF;int _tmp1C2;struct _tuple0*_tmp1C1;if((_tmp1BE.UnknownDatatype).tag == 1){_LL48: _tmp1C1=((_tmp1BE.UnknownDatatype).val).name;_tmp1C2=((_tmp1BE.UnknownDatatype).val).is_extensible;_LL49: {struct _tuple0*n=_tmp1C1;int is_x=_tmp1C2;
_tmp1BF=n;_tmp1C0=is_x;goto _LL4B;}}else{_LL4A: _tmp1BF=(*(_tmp1BE.KnownDatatype).val)->name;_tmp1C0=(*(_tmp1BE.KnownDatatype).val)->is_extensible;_LL4B: {struct _tuple0*n=_tmp1BF;int is_x=_tmp1C0;
# 808
struct Cyc_PP_Doc*_tmp1C3=Cyc_PP_text(({const char*_tmp1C7="datatype ";_tag_fat(_tmp1C7,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp1C3;
struct Cyc_PP_Doc*_tmp1C4=is_x?Cyc_PP_text(({const char*_tmp1C6="@extensible ";_tag_fat(_tmp1C6,sizeof(char),13U);})): Cyc_PP_nil_doc();struct Cyc_PP_Doc*ext=_tmp1C4;
s=({struct Cyc_PP_Doc*_tmp1C5[4U];_tmp1C5[0]=ext,_tmp1C5[1]=kw,({struct Cyc_PP_Doc*_tmp657=Cyc_Absynpp_qvar2doc(n);_tmp1C5[2]=_tmp657;}),({struct Cyc_PP_Doc*_tmp656=Cyc_Absynpp_tps2doc(ts);_tmp1C5[3]=_tmp656;});Cyc_PP_cat(_tag_fat(_tmp1C5,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 813
goto _LL0;}case 19U: _LLF: _tmp1B1=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f1;_tmp1B2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL10: {union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp1B1;struct Cyc_List_List*ts=_tmp1B2;
# 815
{union Cyc_Absyn_DatatypeFieldInfo _tmp1C8=tuf_info;struct _tuple0*_tmp1CB;int _tmp1CA;struct _tuple0*_tmp1C9;int _tmp1CE;struct _tuple0*_tmp1CD;struct _tuple0*_tmp1CC;if((_tmp1C8.UnknownDatatypefield).tag == 1){_LL4D: _tmp1CC=((_tmp1C8.UnknownDatatypefield).val).datatype_name;_tmp1CD=((_tmp1C8.UnknownDatatypefield).val).field_name;_tmp1CE=((_tmp1C8.UnknownDatatypefield).val).is_extensible;_LL4E: {struct _tuple0*tname=_tmp1CC;struct _tuple0*fname=_tmp1CD;int is_x=_tmp1CE;
# 817
_tmp1C9=tname;_tmp1CA=is_x;_tmp1CB=fname;goto _LL50;}}else{_LL4F: _tmp1C9=(((_tmp1C8.KnownDatatypefield).val).f1)->name;_tmp1CA=(((_tmp1C8.KnownDatatypefield).val).f1)->is_extensible;_tmp1CB=(((_tmp1C8.KnownDatatypefield).val).f2)->name;_LL50: {struct _tuple0*tname=_tmp1C9;int is_x=_tmp1CA;struct _tuple0*fname=_tmp1CB;
# 820
struct Cyc_PP_Doc*_tmp1CF=Cyc_PP_text(is_x?({const char*_tmp1D2="@extensible datatype ";_tag_fat(_tmp1D2,sizeof(char),22U);}):({const char*_tmp1D3="datatype ";_tag_fat(_tmp1D3,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp1CF;
s=({struct Cyc_PP_Doc*_tmp1D0[4U];_tmp1D0[0]=kw,({struct Cyc_PP_Doc*_tmp65A=Cyc_Absynpp_qvar2doc(tname);_tmp1D0[1]=_tmp65A;}),({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp1D1=".";_tag_fat(_tmp1D1,sizeof(char),2U);}));_tmp1D0[2]=_tmp659;}),({struct Cyc_PP_Doc*_tmp658=Cyc_Absynpp_qvar2doc(fname);_tmp1D0[3]=_tmp658;});Cyc_PP_cat(_tag_fat(_tmp1D0,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 824
goto _LL0;}case 1U: _LL11: _tmp1AF=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f1;_tmp1B0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f2;_LL12: {enum Cyc_Absyn_Sign sn=_tmp1AF;enum Cyc_Absyn_Size_of sz=_tmp1B0;
# 826
struct _fat_ptr sns;
struct _fat_ptr ts;
{enum Cyc_Absyn_Sign _tmp1D4=sn;switch(_tmp1D4){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp1D5="";_tag_fat(_tmp1D5,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp1D6="unsigned ";_tag_fat(_tmp1D6,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 833
{enum Cyc_Absyn_Size_of _tmp1D7=sz;switch(_tmp1D7){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 835
{enum Cyc_Absyn_Sign _tmp1D8=sn;switch(_tmp1D8){case Cyc_Absyn_None: _LL66: _LL67:
 sns=({const char*_tmp1D9="";_tag_fat(_tmp1D9,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed: _LL68: _LL69:
 sns=({const char*_tmp1DA="signed ";_tag_fat(_tmp1DA,sizeof(char),8U);});goto _LL65;default: _LL6A: _LL6B:
 sns=({const char*_tmp1DB="unsigned ";_tag_fat(_tmp1DB,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 840
ts=({const char*_tmp1DC="char";_tag_fat(_tmp1DC,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp1DD="short";_tag_fat(_tmp1DD,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp1DE="int";_tag_fat(_tmp1DE,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp1DF="long";_tag_fat(_tmp1DF,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
# 847
{enum Cyc_Flags_C_Compilers _tmp1E0=Cyc_Flags_c_compiler;if(_tmp1E0 == Cyc_Flags_Gcc_c){_LL6D: _LL6E:
 ts=({const char*_tmp1E1="long long";_tag_fat(_tmp1E1,sizeof(char),10U);});goto _LL6C;}else{_LL6F: _LL70:
 ts=({const char*_tmp1E2="__int64";_tag_fat(_tmp1E2,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 851
goto _LL58;}_LL58:;}
# 853
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1E5=({struct Cyc_String_pa_PrintArg_struct _tmp5CB;_tmp5CB.tag=0U,_tmp5CB.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_tmp5CB;});struct Cyc_String_pa_PrintArg_struct _tmp1E6=({struct Cyc_String_pa_PrintArg_struct _tmp5CA;_tmp5CA.tag=0U,_tmp5CA.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_tmp5CA;});void*_tmp1E3[2U];_tmp1E3[0]=& _tmp1E5,_tmp1E3[1]=& _tmp1E6;({struct _fat_ptr _tmp65B=({const char*_tmp1E4="%s%s";_tag_fat(_tmp1E4,sizeof(char),5U);});Cyc_aprintf(_tmp65B,_tag_fat(_tmp1E3,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp1AE=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f1;_LL14: {int i=_tmp1AE;
# 856
{int _tmp1E7=i;switch(_tmp1E7){case 0U: _LL72: _LL73:
 s=Cyc_PP_text(({const char*_tmp1E8="float";_tag_fat(_tmp1E8,sizeof(char),6U);}));goto _LL71;case 1U: _LL74: _LL75:
 s=Cyc_PP_text(({const char*_tmp1E9="double";_tag_fat(_tmp1E9,sizeof(char),7U);}));goto _LL71;default: _LL76: _LL77:
 s=Cyc_PP_text(({const char*_tmp1EA="long double";_tag_fat(_tmp1EA,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 861
goto _LL0;}case 20U: _LL17: _tmp1AC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f1;_tmp1AD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL18: {union Cyc_Absyn_AggrInfo info=_tmp1AC;struct Cyc_List_List*ts=_tmp1AD;
# 864
struct _tuple11 _tmp1EC=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp11=_tmp1EC;struct _tuple11 _tmp1ED=_stmttmp11;struct _tuple0*_tmp1EF;enum Cyc_Absyn_AggrKind _tmp1EE;_LL79: _tmp1EE=_tmp1ED.f1;_tmp1EF=_tmp1ED.f2;_LL7A: {enum Cyc_Absyn_AggrKind k=_tmp1EE;struct _tuple0*n=_tmp1EF;
s=({struct Cyc_PP_Doc*_tmp1F0[3U];({struct Cyc_PP_Doc*_tmp65E=Cyc_Absynpp_aggr_kind2doc(k);_tmp1F0[0]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_qvar2doc(n);_tmp1F0[1]=_tmp65D;}),({struct Cyc_PP_Doc*_tmp65C=Cyc_Absynpp_tps2doc(ts);_tmp1F0[2]=_tmp65C;});Cyc_PP_cat(_tag_fat(_tmp1F0,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 16U: _LL1B: _tmp1AB=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f1;_LL1C: {struct Cyc_List_List*fs=_tmp1AB;
# 873
s=({struct Cyc_PP_Doc*_tmp1F2[4U];({struct Cyc_PP_Doc*_tmp662=Cyc_PP_text(({const char*_tmp1F3="enum ";_tag_fat(_tmp1F3,sizeof(char),6U);}));_tmp1F2[0]=_tmp662;}),({struct Cyc_PP_Doc*_tmp661=Cyc_Absynpp_lb();_tmp1F2[1]=_tmp661;}),({struct Cyc_PP_Doc*_tmp660=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_tmp1F2[2]=_tmp660;}),({struct Cyc_PP_Doc*_tmp65F=Cyc_Absynpp_rb();_tmp1F2[3]=_tmp65F;});Cyc_PP_cat(_tag_fat(_tmp1F2,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1D: _tmp1AA=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f1;_LL1E: {struct _tuple0*n=_tmp1AA;
s=({struct Cyc_PP_Doc*_tmp1F4[2U];({struct Cyc_PP_Doc*_tmp664=Cyc_PP_text(({const char*_tmp1F5="enum ";_tag_fat(_tmp1F5,sizeof(char),6U);}));_tmp1F4[0]=_tmp664;}),({struct Cyc_PP_Doc*_tmp663=Cyc_Absynpp_qvar2doc(n);_tmp1F4[1]=_tmp663;});Cyc_PP_cat(_tag_fat(_tmp1F4,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 17U: _LL23: _tmp1A9=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)->f1;_LL24: {struct _fat_ptr t=_tmp1A9;
# 877
s=Cyc_PP_text(t);goto _LL0;}case 3U: _LL27: _tmp1A8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL28: {struct Cyc_List_List*ts=_tmp1A8;
# 887
s=({struct Cyc_PP_Doc*_tmp1FD[3U];({struct Cyc_PP_Doc*_tmp667=Cyc_PP_text(({const char*_tmp1FE="region_t<";_tag_fat(_tmp1FE,sizeof(char),10U);}));_tmp1FD[0]=_tmp667;}),({struct Cyc_PP_Doc*_tmp666=Cyc_Absynpp_rgn2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1FD[1]=_tmp666;}),({struct Cyc_PP_Doc*_tmp665=Cyc_PP_text(({const char*_tmp1FF=">";_tag_fat(_tmp1FF,sizeof(char),2U);}));_tmp1FD[2]=_tmp665;});Cyc_PP_cat(_tag_fat(_tmp1FD,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 4U: _LL29: _tmp1A7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL2A: {struct Cyc_List_List*ts=_tmp1A7;
# 889
s=({struct Cyc_PP_Doc*_tmp200[3U];({struct Cyc_PP_Doc*_tmp66A=Cyc_PP_text(({const char*_tmp201="tag_t<";_tag_fat(_tmp201,sizeof(char),7U);}));_tmp200[0]=_tmp66A;}),({struct Cyc_PP_Doc*_tmp669=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp200[1]=_tmp669;}),({struct Cyc_PP_Doc*_tmp668=Cyc_PP_text(({const char*_tmp202=">";_tag_fat(_tmp202,sizeof(char),2U);}));_tmp200[2]=_tmp668;});Cyc_PP_cat(_tag_fat(_tmp200,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: _LL31: _tmp1A6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL32: {struct Cyc_List_List*ts=_tmp1A6;
# 894
s=({struct Cyc_PP_Doc*_tmp203[3U];({struct Cyc_PP_Doc*_tmp66D=Cyc_PP_text(({const char*_tmp204="regions(";_tag_fat(_tmp204,sizeof(char),9U);}));_tmp203[0]=_tmp66D;}),({struct Cyc_PP_Doc*_tmp66C=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp203[1]=_tmp66C;}),({struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp205=")";_tag_fat(_tmp205,sizeof(char),2U);}));_tmp203[2]=_tmp66B;});Cyc_PP_cat(_tag_fat(_tmp203,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL33: _LL34:
 goto _LL36;case 9U: _LL35: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 900
 s=Cyc_PP_text(({const char*_tmp206="@true";_tag_fat(_tmp206,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
 s=Cyc_PP_text(({const char*_tmp207="@false";_tag_fat(_tmp207,sizeof(char),7U);}));goto _LL0;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2 != 0){_LL41: _tmp1A5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2)->hd;_LL42: {void*t=_tmp1A5;
# 903
s=({struct Cyc_PP_Doc*_tmp208[4U];({struct Cyc_PP_Doc*_tmp671=Cyc_PP_text(({const char*_tmp209="@thin @numelts";_tag_fat(_tmp209,sizeof(char),15U);}));_tmp208[0]=_tmp671;}),({struct Cyc_PP_Doc*_tmp670=Cyc_Absynpp_lb();_tmp208[1]=_tmp670;}),({struct Cyc_PP_Doc*_tmp66F=Cyc_Absynpp_typ2doc(t);_tmp208[2]=_tmp66F;}),({struct Cyc_PP_Doc*_tmp66E=Cyc_Absynpp_rb();_tmp208[3]=_tmp66E;});Cyc_PP_cat(_tag_fat(_tmp208,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}}else{_LL43: _LL44:
# 905
 s=Cyc_PP_text(({const char*_tmp20A="@thin";_tag_fat(_tmp20A,sizeof(char),6U);}));goto _LL0;}default: _LL45: _LL46:
 s=Cyc_PP_text(({const char*_tmp20B="@fat";_tag_fat(_tmp20B,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp1A1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp194)->f1;_tmp1A2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp194)->f2;_tmp1A3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp194)->f3;_tmp1A4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp194)->f4;_LLA: {struct Cyc_Core_Opt*k=_tmp1A1;void*topt=_tmp1A2;int i=_tmp1A3;struct Cyc_Core_Opt*tvs=_tmp1A4;
# 791
if(topt != 0)
# 793
return Cyc_Absynpp_ntyp2doc(topt);else{
# 795
struct _fat_ptr kindstring=k == 0?({const char*_tmp1BB="K";_tag_fat(_tmp1BB,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B9=({struct Cyc_String_pa_PrintArg_struct _tmp5CD;_tmp5CD.tag=0U,_tmp5CD.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_tmp5CD;});struct Cyc_Int_pa_PrintArg_struct _tmp1BA=({struct Cyc_Int_pa_PrintArg_struct _tmp5CC;_tmp5CC.tag=1U,_tmp5CC.f1=(unsigned long)i;_tmp5CC;});void*_tmp1B7[2U];_tmp1B7[0]=& _tmp1B9,_tmp1B7[1]=& _tmp1BA;({struct _fat_ptr _tmp672=({const char*_tmp1B8="`E%s%d";_tag_fat(_tmp1B8,sizeof(char),7U);});Cyc_aprintf(_tmp672,_tag_fat(_tmp1B7,sizeof(void*),2U));});}));}
# 798
goto _LL0;}case 2U: _LLB: _tmp1A0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp194)->f1;_LLC: {struct Cyc_Absyn_Tvar*tv=_tmp1A0;
# 800
s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp1BC[3U];_tmp1BC[0]=s,({struct Cyc_PP_Doc*_tmp674=Cyc_PP_text(({const char*_tmp1BD="::";_tag_fat(_tmp1BD,sizeof(char),3U);}));_tmp1BC[1]=_tmp674;}),({struct Cyc_PP_Doc*_tmp673=Cyc_Absynpp_kindbound2doc(tv->kind);_tmp1BC[2]=_tmp673;});Cyc_PP_cat(_tag_fat(_tmp1BC,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 6U: _LL15: _tmp19F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp194)->f1;_LL16: {struct Cyc_List_List*ts=_tmp19F;
# 862
s=({struct Cyc_PP_Doc*_tmp1EB[2U];({struct Cyc_PP_Doc*_tmp676=Cyc_Absynpp_dollar();_tmp1EB[0]=_tmp676;}),({struct Cyc_PP_Doc*_tmp675=Cyc_Absynpp_args2doc(ts);_tmp1EB[1]=_tmp675;});Cyc_PP_cat(_tag_fat(_tmp1EB,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 7U: _LL19: _tmp19D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp194)->f1;_tmp19E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp194)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp19D;struct Cyc_List_List*fs=_tmp19E;
# 868
s=({struct Cyc_PP_Doc*_tmp1F1[4U];({struct Cyc_PP_Doc*_tmp67A=Cyc_Absynpp_aggr_kind2doc(k);_tmp1F1[0]=_tmp67A;}),({struct Cyc_PP_Doc*_tmp679=Cyc_Absynpp_lb();_tmp1F1[1]=_tmp679;}),({
struct Cyc_PP_Doc*_tmp678=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_tmp1F1[2]=_tmp678;}),({
struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_rb();_tmp1F1[3]=_tmp677;});Cyc_PP_cat(_tag_fat(_tmp1F1,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 9U: _LL1F: _tmp19C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp194)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp19C;
# 875
s=({struct Cyc_PP_Doc*_tmp1F6[3U];({struct Cyc_PP_Doc*_tmp67D=Cyc_PP_text(({const char*_tmp1F7="valueof_t(";_tag_fat(_tmp1F7,sizeof(char),11U);}));_tmp1F6[0]=_tmp67D;}),({struct Cyc_PP_Doc*_tmp67C=Cyc_Absynpp_exp2doc(e);_tmp1F6[1]=_tmp67C;}),({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(({const char*_tmp1F8=")";_tag_fat(_tmp1F8,sizeof(char),2U);}));_tmp1F6[2]=_tmp67B;});Cyc_PP_cat(_tag_fat(_tmp1F6,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 11U: _LL21: _tmp19B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp194)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp19B;
s=({struct Cyc_PP_Doc*_tmp1F9[3U];({struct Cyc_PP_Doc*_tmp680=Cyc_PP_text(({const char*_tmp1FA="typeof(";_tag_fat(_tmp1FA,sizeof(char),8U);}));_tmp1F9[0]=_tmp680;}),({struct Cyc_PP_Doc*_tmp67F=Cyc_Absynpp_exp2doc(e);_tmp1F9[1]=_tmp67F;}),({struct Cyc_PP_Doc*_tmp67E=Cyc_PP_text(({const char*_tmp1FB=")";_tag_fat(_tmp1FB,sizeof(char),2U);}));_tmp1F9[2]=_tmp67E;});Cyc_PP_cat(_tag_fat(_tmp1F9,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL25: _tmp198=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp194)->f1;_tmp199=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp194)->f2;_tmp19A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp194)->f3;_LL26: {struct _tuple0*n=_tmp198;struct Cyc_List_List*ts=_tmp199;struct Cyc_Absyn_Typedefdecl*kopt=_tmp19A;
# 884
s=({struct Cyc_PP_Doc*_tmp1FC[2U];({struct Cyc_PP_Doc*_tmp682=Cyc_Absynpp_qvar2doc(n);_tmp1FC[0]=_tmp682;}),({struct Cyc_PP_Doc*_tmp681=Cyc_Absynpp_tps2doc(ts);_tmp1FC[1]=_tmp681;});Cyc_PP_cat(_tag_fat(_tmp1FC,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp194)->f1)->r)){case 0U: _LL37: _tmp197=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp194)->f1)->r)->f1;_LL38: {struct Cyc_Absyn_Aggrdecl*d=_tmp197;
# 897
s=Cyc_Absynpp_aggrdecl2doc(d);goto _LL0;}case 1U: _LL39: _tmp196=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp194)->f1)->r)->f1;_LL3A: {struct Cyc_Absyn_Enumdecl*d=_tmp196;
s=Cyc_Absynpp_enumdecl2doc(d);goto _LL0;}default: _LL3B: _tmp195=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp194)->f1)->r)->f1;_LL3C: {struct Cyc_Absyn_Datatypedecl*d=_tmp195;
s=Cyc_Absynpp_datatypedecl2doc(d);goto _LL0;}}}_LL0:;}
# 908
return s;}
# 911
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _fat_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple18{void*f1;void*f2;};
# 915
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple18*cmp){
struct _tuple18*_tmp20C=cmp;void*_tmp20E;void*_tmp20D;_LL1: _tmp20D=_tmp20C->f1;_tmp20E=_tmp20C->f2;_LL2: {void*r1=_tmp20D;void*r2=_tmp20E;
return({struct Cyc_PP_Doc*_tmp20F[3U];({struct Cyc_PP_Doc*_tmp685=Cyc_Absynpp_rgn2doc(r1);_tmp20F[0]=_tmp685;}),({struct Cyc_PP_Doc*_tmp684=Cyc_PP_text(({const char*_tmp210=" > ";_tag_fat(_tmp210,sizeof(char),4U);}));_tmp20F[1]=_tmp684;}),({struct Cyc_PP_Doc*_tmp683=Cyc_Absynpp_rgn2doc(r2);_tmp20F[2]=_tmp683;});Cyc_PP_cat(_tag_fat(_tmp20F,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 920
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _tmp688=({const char*_tmp211="";_tag_fat(_tmp211,sizeof(char),1U);});struct _fat_ptr _tmp687=({const char*_tmp212="";_tag_fat(_tmp212,sizeof(char),1U);});struct _fat_ptr _tmp686=({const char*_tmp213=",";_tag_fat(_tmp213,sizeof(char),2U);});Cyc_PP_group(_tmp688,_tmp687,_tmp686,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 924
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*_tmp214=(*t).f1;struct _fat_ptr*vo=_tmp214;
struct Cyc_Core_Opt*dopt=vo == 0?0:({struct Cyc_Core_Opt*_tmp215=_cycalloc(sizeof(*_tmp215));({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(*vo);_tmp215->v=_tmp689;});_tmp215;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 930
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 934
struct Cyc_List_List*_tmp216=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_List_List*arg_docs=_tmp216;
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_tmp68B=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp68B,({struct Cyc_List_List*_tmp218=_cycalloc(sizeof(*_tmp218));({struct Cyc_PP_Doc*_tmp68A=Cyc_PP_text(({const char*_tmp217="...";_tag_fat(_tmp217,sizeof(char),4U);}));_tmp218->hd=_tmp68A;}),_tmp218->tl=0;_tmp218;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp219=({struct Cyc_PP_Doc*_tmp21B[3U];({struct Cyc_PP_Doc*_tmp68F=Cyc_PP_text(({const char*_tmp21C="...";_tag_fat(_tmp21C,sizeof(char),4U);}));_tmp21B[0]=_tmp68F;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp68E=Cyc_PP_text(({const char*_tmp21D=" inject ";_tag_fat(_tmp21D,sizeof(char),9U);}));_tmp21B[1]=_tmp68E;}):({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp21E=" ";_tag_fat(_tmp21E,sizeof(char),2U);}));_tmp21B[1]=_tmp68D;}),({
struct Cyc_PP_Doc*_tmp68C=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->f1=cyc_varargs->name,_tmp21F->f2=cyc_varargs->tq,_tmp21F->f3=cyc_varargs->type;_tmp21F;}));_tmp21B[2]=_tmp68C;});Cyc_PP_cat(_tag_fat(_tmp21B,sizeof(struct Cyc_PP_Doc*),3U));});
# 939
struct Cyc_PP_Doc*varargs_doc=_tmp219;
# 943
arg_docs=({struct Cyc_List_List*_tmp690=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp690,({struct Cyc_List_List*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A->hd=varargs_doc,_tmp21A->tl=0;_tmp21A;}));});}}{
# 945
struct Cyc_PP_Doc*_tmp220=({struct _fat_ptr _tmp693=({const char*_tmp22F="";_tag_fat(_tmp22F,sizeof(char),1U);});struct _fat_ptr _tmp692=({const char*_tmp230="";_tag_fat(_tmp230,sizeof(char),1U);});struct _fat_ptr _tmp691=({const char*_tmp231=",";_tag_fat(_tmp231,sizeof(char),2U);});Cyc_PP_group(_tmp693,_tmp692,_tmp691,arg_docs);});struct Cyc_PP_Doc*arg_doc=_tmp220;
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_tmp221[3U];_tmp221[0]=arg_doc,({struct Cyc_PP_Doc*_tmp695=Cyc_PP_text(({const char*_tmp222=";";_tag_fat(_tmp222,sizeof(char),2U);}));_tmp221[1]=_tmp695;}),({struct Cyc_PP_Doc*_tmp694=Cyc_Absynpp_eff2doc(effopt);_tmp221[2]=_tmp694;});Cyc_PP_cat(_tag_fat(_tmp221,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
arg_doc=({struct Cyc_PP_Doc*_tmp223[3U];_tmp223[0]=arg_doc,({struct Cyc_PP_Doc*_tmp697=Cyc_PP_text(({const char*_tmp224=":";_tag_fat(_tmp224,sizeof(char),2U);}));_tmp223[1]=_tmp697;}),({struct Cyc_PP_Doc*_tmp696=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp223[2]=_tmp696;});Cyc_PP_cat(_tag_fat(_tmp223,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp225=({struct Cyc_PP_Doc*_tmp22C[3U];({struct Cyc_PP_Doc*_tmp699=Cyc_PP_text(({const char*_tmp22D="(";_tag_fat(_tmp22D,sizeof(char),2U);}));_tmp22C[0]=_tmp699;}),_tmp22C[1]=arg_doc,({struct Cyc_PP_Doc*_tmp698=Cyc_PP_text(({const char*_tmp22E=")";_tag_fat(_tmp22E,sizeof(char),2U);}));_tmp22C[2]=_tmp698;});Cyc_PP_cat(_tag_fat(_tmp22C,sizeof(struct Cyc_PP_Doc*),3U));});struct Cyc_PP_Doc*res=_tmp225;
if(req != 0)
res=({struct Cyc_PP_Doc*_tmp226[4U];_tmp226[0]=res,({struct Cyc_PP_Doc*_tmp69C=Cyc_PP_text(({const char*_tmp227=" @requires(";_tag_fat(_tmp227,sizeof(char),12U);}));_tmp226[1]=_tmp69C;}),({struct Cyc_PP_Doc*_tmp69B=Cyc_Absynpp_exp2doc(req);_tmp226[2]=_tmp69B;}),({struct Cyc_PP_Doc*_tmp69A=Cyc_PP_text(({const char*_tmp228=")";_tag_fat(_tmp228,sizeof(char),2U);}));_tmp226[3]=_tmp69A;});Cyc_PP_cat(_tag_fat(_tmp226,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
res=({struct Cyc_PP_Doc*_tmp229[4U];_tmp229[0]=res,({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_text(({const char*_tmp22A=" @ensures(";_tag_fat(_tmp22A,sizeof(char),11U);}));_tmp229[1]=_tmp69F;}),({struct Cyc_PP_Doc*_tmp69E=Cyc_Absynpp_exp2doc(ens);_tmp229[2]=_tmp69E;}),({struct Cyc_PP_Doc*_tmp69D=Cyc_PP_text(({const char*_tmp22B=")";_tag_fat(_tmp22B,sizeof(char),2U);}));_tmp229[3]=_tmp69D;});Cyc_PP_cat(_tag_fat(_tmp229,sizeof(struct Cyc_PP_Doc*),4U));});
return res;}}}
# 958
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _fat_ptr*v){return Cyc_PP_text(*v);}
# 960
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp232=0;struct Cyc_List_List*prefix=_tmp232;
int match;
{union Cyc_Absyn_Nmspace _tmp233=(*q).f1;union Cyc_Absyn_Nmspace _stmttmp12=_tmp233;union Cyc_Absyn_Nmspace _tmp234=_stmttmp12;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp236;struct Cyc_List_List*_tmp237;switch((_tmp234.C_n).tag){case 4U: _LL1: _LL2:
 _tmp237=0;goto _LL4;case 1U: _LL3: _tmp237=(_tmp234.Rel_n).val;_LL4: {struct Cyc_List_List*x=_tmp237;
# 966
match=0;
if(((unsigned)x && Cyc_Absynpp_qvar_to_Cids)&& !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 970
prefix=x;}
goto _LL0;}case 3U: _LL5: _tmp236=(_tmp234.C_n).val;_LL6: {struct Cyc_List_List*x=_tmp236;
# 973
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
# 975
goto _LL0;}default: _LL7: _tmp235=(_tmp234.Abs_n).val;_LL8: {struct Cyc_List_List*x=_tmp235;
# 977
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
if(Cyc_Absynpp_qvar_to_Cids){
if((unsigned)x && !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 982
prefix=Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238->hd=Cyc_Absynpp_cyc_stringptr,_tmp238->tl=x;_tmp238;}): x;}}
# 984
goto _LL0;}}_LL0:;}
# 986
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _fat_ptr)({struct Cyc_List_List*_tmp6A1=({struct Cyc_List_List*_tmp6A0=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6A0,({struct Cyc_List_List*_tmp239=_cycalloc(sizeof(*_tmp239));
_tmp239->hd=(*q).f2,_tmp239->tl=0;_tmp239;}));});
# 987
Cyc_str_sepstr(_tmp6A1,({const char*_tmp23A="_";_tag_fat(_tmp23A,sizeof(char),2U);}));});else{
# 991
if(match)
return*(*q).f2;else{
# 994
return(struct _fat_ptr)({struct Cyc_List_List*_tmp6A3=({struct Cyc_List_List*_tmp6A2=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6A2,({struct Cyc_List_List*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->hd=(*q).f2,_tmp23B->tl=0;_tmp23B;}));});Cyc_str_sepstr(_tmp6A3,({const char*_tmp23C="::";_tag_fat(_tmp23C,sizeof(char),3U);}));});}}}
# 998
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 1002
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr _tmp23D=Cyc_Absynpp_qvar2string(q);struct _fat_ptr qs=_tmp23D;
if(Cyc_PP_tex_output)
# 1006
return({struct _fat_ptr _tmp6A6=(struct _fat_ptr)({struct _fat_ptr _tmp6A5=(struct _fat_ptr)({struct _fat_ptr _tmp6A4=({const char*_tmp23E="\\textbf{";_tag_fat(_tmp23E,sizeof(char),9U);});Cyc_strconcat(_tmp6A4,(struct _fat_ptr)qs);});Cyc_strconcat(_tmp6A5,({const char*_tmp23F="}";_tag_fat(_tmp23F,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp6A6,(int)
Cyc_strlen((struct _fat_ptr)qs));});else{
return Cyc_PP_text(qs);}}
# 1011
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 1013
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 1016
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp240=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp13=_tmp240;union Cyc_Absyn_Nmspace _tmp241=_stmttmp13;struct Cyc_List_List*_tmp242;struct Cyc_List_List*_tmp243;switch((_tmp241.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp241.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 1027
 return(struct _fat_ptr)({struct _fat_ptr _tmp6A7=({const char*_tmp244="/* bad namespace : */ ";_tag_fat(_tmp244,sizeof(char),23U);});Cyc_strconcat(_tmp6A7,(struct _fat_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp243=(_tmp241.C_n).val;_LL6: {struct Cyc_List_List*l=_tmp243;
# 1020
_tmp242=l;goto _LL8;}default: _LL7: _tmp242=(_tmp241.Abs_n).val;_LL8: {struct Cyc_List_List*l=_tmp242;
# 1022
if(((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,l,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 1025
goto _LLA;}}}_LL0:;}else{
# 1030
return*(*v).f2;}}
# 1033
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 1036
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _fat_ptr _tmp245=Cyc_Absynpp_typedef_name2string(v);struct _fat_ptr vs=_tmp245;
if(Cyc_PP_tex_output)
# 1040
return({struct _fat_ptr _tmp6AA=(struct _fat_ptr)({struct _fat_ptr _tmp6A9=(struct _fat_ptr)({struct _fat_ptr _tmp6A8=({const char*_tmp246="\\textbf{";_tag_fat(_tmp246,sizeof(char),9U);});Cyc_strconcat(_tmp6A8,(struct _fat_ptr)vs);});Cyc_strconcat(_tmp6A9,({const char*_tmp247="}";_tag_fat(_tmp247,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp6AA,(int)
Cyc_strlen((struct _fat_ptr)vs));});else{
return Cyc_PP_text(vs);}}
# 1045
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp6AB=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp6AB,t,0);});}
# 1049
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_tmp248=f;unsigned _tmp249;struct _fat_ptr*_tmp24A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp248)->tag == 0U){_LL1: _tmp24A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp248)->f1;_LL2: {struct _fat_ptr*n=_tmp24A;
return Cyc_PP_textptr(n);}}else{_LL3: _tmp249=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp248)->f1;_LL4: {unsigned n=_tmp249;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp24D=({struct Cyc_Int_pa_PrintArg_struct _tmp5CE;_tmp5CE.tag=1U,_tmp5CE.f1=(unsigned long)((int)n);_tmp5CE;});void*_tmp24B[1U];_tmp24B[0]=& _tmp24D;({struct _fat_ptr _tmp6AC=({const char*_tmp24C="%d";_tag_fat(_tmp24C,sizeof(char),3U);});Cyc_aprintf(_tmp6AC,_tag_fat(_tmp24B,sizeof(void*),1U));});}));}}_LL0:;}
# 1063 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp24E=e->r;void*_stmttmp14=_tmp24E;void*_tmp24F=_stmttmp14;struct Cyc_Absyn_Exp*_tmp250;struct Cyc_Absyn_Exp*_tmp251;enum Cyc_Absyn_Primop _tmp252;switch(*((int*)_tmp24F)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp252=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp24F)->f1;_LL6: {enum Cyc_Absyn_Primop p=_tmp252;
# 1068
enum Cyc_Absyn_Primop _tmp253=p;switch(_tmp253){case Cyc_Absyn_Plus: _LL58: _LL59:
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
# 1090
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return - 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp251=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp24F)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp251;
return Cyc_Absynpp_exp_prec(e2);}case 13U: _LL1B: _tmp250=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp24F)->f1;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp250;
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
# 1133
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct _tuple19{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1137
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
struct _tuple19*_tmp254=(struct _tuple19*)o->hd;struct _tuple19*_stmttmp15=_tmp254;struct _tuple19*_tmp255=_stmttmp15;struct Cyc_Absyn_Exp*_tmp257;struct _fat_ptr _tmp256;_LL1: _tmp256=_tmp255->f1;_tmp257=_tmp255->f2;_LL2: {struct _fat_ptr c=_tmp256;struct Cyc_Absyn_Exp*e=_tmp257;
s=({struct Cyc_PP_Doc*_tmp258[6U];_tmp258[0]=s,({struct Cyc_PP_Doc*_tmp6B1=Cyc_PP_text(({const char*_tmp259="\"";_tag_fat(_tmp259,sizeof(char),2U);}));_tmp258[1]=_tmp6B1;}),({struct Cyc_PP_Doc*_tmp6B0=Cyc_PP_text(c);_tmp258[2]=_tmp6B0;}),({struct Cyc_PP_Doc*_tmp6AF=Cyc_PP_text(({const char*_tmp25A="\" (";_tag_fat(_tmp25A,sizeof(char),4U);}));_tmp258[3]=_tmp6AF;}),({struct Cyc_PP_Doc*_tmp6AE=Cyc_Absynpp_exp2doc(e);_tmp258[4]=_tmp6AE;}),({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp25B=")";_tag_fat(_tmp25B,sizeof(char),2U);}));_tmp258[5]=_tmp6AD;});Cyc_PP_cat(_tag_fat(_tmp258,sizeof(struct Cyc_PP_Doc*),6U));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp25C[2U];_tmp25C[0]=s,({struct Cyc_PP_Doc*_tmp6B2=Cyc_PP_text(({const char*_tmp25D=",";_tag_fat(_tmp25D,sizeof(char),2U);}));_tmp25C[1]=_tmp6B2;});Cyc_PP_cat(_tag_fat(_tmp25C,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1147
return s;}
# 1150
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp25E[2U];({struct Cyc_PP_Doc*_tmp6B4=Cyc_PP_text(({const char*_tmp25F=": ";_tag_fat(_tmp25F,sizeof(char),3U);}));_tmp25E[0]=_tmp6B4;}),({struct Cyc_PP_Doc*_tmp6B3=Cyc_Absynpp_asm_iolist_doc_in(o);_tmp25E[1]=_tmp6B3;});Cyc_PP_cat(_tag_fat(_tmp25E,sizeof(struct Cyc_PP_Doc*),2U));});
# 1155
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_tmp260[3U];_tmp260[0]=s,({struct Cyc_PP_Doc*_tmp6B6=Cyc_PP_text(({const char*_tmp261=": : ";_tag_fat(_tmp261,sizeof(char),5U);}));_tmp260[1]=_tmp6B6;}),({struct Cyc_PP_Doc*_tmp6B5=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp260[2]=_tmp6B5;});Cyc_PP_cat(_tag_fat(_tmp260,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1159
s=({struct Cyc_PP_Doc*_tmp262[3U];_tmp262[0]=s,({struct Cyc_PP_Doc*_tmp6B8=Cyc_PP_text(({const char*_tmp263=" : ";_tag_fat(_tmp263,sizeof(char),4U);}));_tmp262[1]=_tmp6B8;}),({struct Cyc_PP_Doc*_tmp6B7=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp262[2]=_tmp6B7;});Cyc_PP_cat(_tag_fat(_tmp262,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1161
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_tmp264[2U];_tmp264[0]=s,ncol == 0?({struct Cyc_PP_Doc*_tmp6BB=Cyc_PP_text(({const char*_tmp265=": : :";_tag_fat(_tmp265,sizeof(char),6U);}));_tmp264[1]=_tmp6BB;}):(ncol == 1?({struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_text(({const char*_tmp266=" : : ";_tag_fat(_tmp266,sizeof(char),6U);}));_tmp264[1]=_tmp6BA;}):({struct Cyc_PP_Doc*_tmp6B9=Cyc_PP_text(({const char*_tmp267=" : ";_tag_fat(_tmp267,sizeof(char),4U);}));_tmp264[1]=_tmp6B9;}));Cyc_PP_cat(_tag_fat(_tmp264,sizeof(struct Cyc_PP_Doc*),2U));});
while(cl != 0){
s=({struct Cyc_PP_Doc*_tmp268[4U];_tmp268[0]=s,({struct Cyc_PP_Doc*_tmp6BE=Cyc_PP_text(({const char*_tmp269="\"";_tag_fat(_tmp269,sizeof(char),2U);}));_tmp268[1]=_tmp6BE;}),({struct Cyc_PP_Doc*_tmp6BD=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_tmp268[2]=_tmp6BD;}),({struct Cyc_PP_Doc*_tmp6BC=Cyc_PP_text(({const char*_tmp26A="\"";_tag_fat(_tmp26A,sizeof(char),2U);}));_tmp268[3]=_tmp6BC;});Cyc_PP_cat(_tag_fat(_tmp268,sizeof(struct Cyc_PP_Doc*),4U));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_tmp26B[2U];_tmp26B[0]=s,({struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_text(({const char*_tmp26C=", ";_tag_fat(_tmp26C,sizeof(char),3U);}));_tmp26B[1]=_tmp6BF;});Cyc_PP_cat(_tag_fat(_tmp26B,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1171
return s;}
# 1174
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp26D=e->r;void*_stmttmp16=_tmp26D;void*_tmp26E=_stmttmp16;struct Cyc_Absyn_Stmt*_tmp26F;struct Cyc_List_List*_tmp271;struct Cyc_Core_Opt*_tmp270;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp272;int _tmp278;struct Cyc_Absyn_Exp*_tmp277;void**_tmp276;struct Cyc_Absyn_Exp*_tmp275;int _tmp274;struct Cyc_Absyn_Enumfield*_tmp279;struct Cyc_Absyn_Enumfield*_tmp27A;struct Cyc_Absyn_Datatypefield*_tmp27C;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27D;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp27F;struct _tuple0*_tmp27E;void*_tmp282;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Vardecl*_tmp283;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;struct _tuple8*_tmp287;struct Cyc_List_List*_tmp289;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_Absyn_Exp*_tmp28A;struct _fat_ptr*_tmp28D;struct Cyc_Absyn_Exp*_tmp28C;struct _fat_ptr*_tmp28F;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_List_List*_tmp292;void*_tmp291;struct _fat_ptr*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp298;struct Cyc_List_List*_tmp297;struct _fat_ptr _tmp296;int _tmp295;void*_tmp29A;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_Absyn_Exp*_tmp29D;void*_tmp29E;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A3;void*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_List_List*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2AF;enum Cyc_Absyn_Incrementor _tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Core_Opt*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_List_List*_tmp2B8;enum Cyc_Absyn_Primop _tmp2B7;struct _fat_ptr _tmp2B9;void*_tmp2BA;union Cyc_Absyn_Cnst _tmp2BB;switch(*((int*)_tmp26E)){case 0U: _LL1: _tmp2BB=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp2BB;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1U: _LL3: _tmp2BA=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL4: {void*b=_tmp2BA;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL5: _tmp2B9=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL6: {struct _fat_ptr p=_tmp2B9;
# 1181
s=({struct Cyc_PP_Doc*_tmp2BC[4U];({struct Cyc_PP_Doc*_tmp6C3=Cyc_PP_text(({const char*_tmp2BD="__cyclone_pragma__";_tag_fat(_tmp2BD,sizeof(char),19U);}));_tmp2BC[0]=_tmp6C3;}),({struct Cyc_PP_Doc*_tmp6C2=Cyc_PP_text(({const char*_tmp2BE="(";_tag_fat(_tmp2BE,sizeof(char),2U);}));_tmp2BC[1]=_tmp6C2;}),({struct Cyc_PP_Doc*_tmp6C1=Cyc_PP_text(p);_tmp2BC[2]=_tmp6C1;}),({struct Cyc_PP_Doc*_tmp6C0=Cyc_PP_text(({const char*_tmp2BF=")";_tag_fat(_tmp2BF,sizeof(char),2U);}));_tmp2BC[3]=_tmp6C0;});Cyc_PP_cat(_tag_fat(_tmp2BC,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 3U: _LL7: _tmp2B7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2B8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp2B7;struct Cyc_List_List*es=_tmp2B8;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4U: _LL9: _tmp2B4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2B5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_tmp2B6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp26E)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp2B4;struct Cyc_Core_Opt*popt=_tmp2B5;struct Cyc_Absyn_Exp*e2=_tmp2B6;
# 1184
s=({struct Cyc_PP_Doc*_tmp2C0[5U];({struct Cyc_PP_Doc*_tmp6C9=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C0[0]=_tmp6C9;}),({
struct Cyc_PP_Doc*_tmp6C8=Cyc_PP_text(({const char*_tmp2C1=" ";_tag_fat(_tmp2C1,sizeof(char),2U);}));_tmp2C0[1]=_tmp6C8;}),
popt == 0?({struct Cyc_PP_Doc*_tmp6C7=Cyc_PP_nil_doc();_tmp2C0[2]=_tmp6C7;}):({struct Cyc_PP_Doc*_tmp6C6=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_tmp2C0[2]=_tmp6C6;}),({
struct Cyc_PP_Doc*_tmp6C5=Cyc_PP_text(({const char*_tmp2C2="= ";_tag_fat(_tmp2C2,sizeof(char),3U);}));_tmp2C0[3]=_tmp6C5;}),({
struct Cyc_PP_Doc*_tmp6C4=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2C0[4]=_tmp6C4;});Cyc_PP_cat(_tag_fat(_tmp2C0,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp2B2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2B3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LLC: {struct Cyc_Absyn_Exp*e2=_tmp2B2;enum Cyc_Absyn_Incrementor i=_tmp2B3;
# 1191
struct Cyc_PP_Doc*_tmp2C3=Cyc_Absynpp_exp2doc_prec(myprec,e2);struct Cyc_PP_Doc*es=_tmp2C3;
{enum Cyc_Absyn_Incrementor _tmp2C4=i;switch(_tmp2C4){case Cyc_Absyn_PreInc: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp2C5[2U];({struct Cyc_PP_Doc*_tmp6CA=Cyc_PP_text(({const char*_tmp2C6="++";_tag_fat(_tmp2C6,sizeof(char),3U);}));_tmp2C5[0]=_tmp6CA;}),_tmp2C5[1]=es;Cyc_PP_cat(_tag_fat(_tmp2C5,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PreDec: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp2C7[2U];({struct Cyc_PP_Doc*_tmp6CB=Cyc_PP_text(({const char*_tmp2C8="--";_tag_fat(_tmp2C8,sizeof(char),3U);}));_tmp2C7[0]=_tmp6CB;}),_tmp2C7[1]=es;Cyc_PP_cat(_tag_fat(_tmp2C7,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PostInc: _LL5C: _LL5D:
 s=({struct Cyc_PP_Doc*_tmp2C9[2U];_tmp2C9[0]=es,({struct Cyc_PP_Doc*_tmp6CC=Cyc_PP_text(({const char*_tmp2CA="++";_tag_fat(_tmp2CA,sizeof(char),3U);}));_tmp2C9[1]=_tmp6CC;});Cyc_PP_cat(_tag_fat(_tmp2C9,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PostDec: _LL5E: _LL5F:
 goto _LL61;default: _LL60: _LL61:
 s=({struct Cyc_PP_Doc*_tmp2CB[2U];_tmp2CB[0]=es,({struct Cyc_PP_Doc*_tmp6CD=Cyc_PP_text(({const char*_tmp2CC="--";_tag_fat(_tmp2CC,sizeof(char),3U);}));_tmp2CB[1]=_tmp6CD;});Cyc_PP_cat(_tag_fat(_tmp2CB,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;}_LL57:;}
# 1199
goto _LL0;}case 6U: _LLD: _tmp2AF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2B0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_tmp2B1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp26E)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp2AF;struct Cyc_Absyn_Exp*e2=_tmp2B0;struct Cyc_Absyn_Exp*e3=_tmp2B1;
# 1201
s=({struct Cyc_PP_Doc*_tmp2CD[5U];({struct Cyc_PP_Doc*_tmp6D2=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2CD[0]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_PP_text(({const char*_tmp2CE=" ? ";_tag_fat(_tmp2CE,sizeof(char),4U);}));_tmp2CD[1]=_tmp6D1;}),({struct Cyc_PP_Doc*_tmp6D0=Cyc_Absynpp_exp2doc_prec(0,e2);_tmp2CD[2]=_tmp6D0;}),({
struct Cyc_PP_Doc*_tmp6CF=Cyc_PP_text(({const char*_tmp2CF=" : ";_tag_fat(_tmp2CF,sizeof(char),4U);}));_tmp2CD[3]=_tmp6CF;}),({struct Cyc_PP_Doc*_tmp6CE=Cyc_Absynpp_exp2doc_prec(myprec,e3);_tmp2CD[4]=_tmp6CE;});Cyc_PP_cat(_tag_fat(_tmp2CD,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 7U: _LLF: _tmp2AD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2AE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp2AD;struct Cyc_Absyn_Exp*e2=_tmp2AE;
# 1205
s=({struct Cyc_PP_Doc*_tmp2D0[3U];({struct Cyc_PP_Doc*_tmp6D5=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D0[0]=_tmp6D5;}),({struct Cyc_PP_Doc*_tmp6D4=Cyc_PP_text(({const char*_tmp2D1=" && ";_tag_fat(_tmp2D1,sizeof(char),5U);}));_tmp2D0[1]=_tmp6D4;}),({struct Cyc_PP_Doc*_tmp6D3=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2D0[2]=_tmp6D3;});Cyc_PP_cat(_tag_fat(_tmp2D0,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 8U: _LL11: _tmp2AB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2AC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp2AB;struct Cyc_Absyn_Exp*e2=_tmp2AC;
# 1208
s=({struct Cyc_PP_Doc*_tmp2D2[3U];({struct Cyc_PP_Doc*_tmp6D8=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D2[0]=_tmp6D8;}),({struct Cyc_PP_Doc*_tmp6D7=Cyc_PP_text(({const char*_tmp2D3=" || ";_tag_fat(_tmp2D3,sizeof(char),5U);}));_tmp2D2[1]=_tmp6D7;}),({struct Cyc_PP_Doc*_tmp6D6=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2D2[2]=_tmp6D6;});Cyc_PP_cat(_tag_fat(_tmp2D2,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 9U: _LL13: _tmp2A9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp2A9;struct Cyc_Absyn_Exp*e2=_tmp2AA;
# 1213
s=({struct Cyc_PP_Doc*_tmp2D4[3U];({struct Cyc_PP_Doc*_tmp6DB=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_tmp2D4[0]=_tmp6DB;}),({struct Cyc_PP_Doc*_tmp6DA=Cyc_PP_text(({const char*_tmp2D5=", ";_tag_fat(_tmp2D5,sizeof(char),3U);}));_tmp2D4[1]=_tmp6DA;}),({struct Cyc_PP_Doc*_tmp6D9=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_tmp2D4[2]=_tmp6D9;});Cyc_PP_cat(_tag_fat(_tmp2D4,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 10U: _LL15: _tmp2A7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2A8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp2A7;struct Cyc_List_List*es=_tmp2A8;
# 1216
s=({struct Cyc_PP_Doc*_tmp2D6[4U];({struct Cyc_PP_Doc*_tmp6DF=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D6[0]=_tmp6DF;}),({
struct Cyc_PP_Doc*_tmp6DE=Cyc_PP_text(({const char*_tmp2D7="(";_tag_fat(_tmp2D7,sizeof(char),2U);}));_tmp2D6[1]=_tmp6DE;}),({
struct Cyc_PP_Doc*_tmp6DD=Cyc_Absynpp_exps2doc_prec(20,es);_tmp2D6[2]=_tmp6DD;}),({
struct Cyc_PP_Doc*_tmp6DC=Cyc_PP_text(({const char*_tmp2D8=")";_tag_fat(_tmp2D8,sizeof(char),2U);}));_tmp2D6[3]=_tmp6DC;});Cyc_PP_cat(_tag_fat(_tmp2D6,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 11U: _LL17: _tmp2A6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp2A6;
s=({struct Cyc_PP_Doc*_tmp2D9[2U];({struct Cyc_PP_Doc*_tmp6E1=Cyc_PP_text(({const char*_tmp2DA="throw ";_tag_fat(_tmp2DA,sizeof(char),7U);}));_tmp2D9[0]=_tmp6E1;}),({struct Cyc_PP_Doc*_tmp6E0=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D9[1]=_tmp6E0;});Cyc_PP_cat(_tag_fat(_tmp2D9,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 12U: _LL19: _tmp2A5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp2A5;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13U: _LL1B: _tmp2A4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp2A4;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14U: _LL1D: _tmp2A2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2A3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL1E: {void*t=_tmp2A2;struct Cyc_Absyn_Exp*e1=_tmp2A3;
# 1225
s=({struct Cyc_PP_Doc*_tmp2DB[4U];({struct Cyc_PP_Doc*_tmp6E5=Cyc_PP_text(({const char*_tmp2DC="(";_tag_fat(_tmp2DC,sizeof(char),2U);}));_tmp2DB[0]=_tmp6E5;}),({struct Cyc_PP_Doc*_tmp6E4=Cyc_Absynpp_typ2doc(t);_tmp2DB[1]=_tmp6E4;}),({struct Cyc_PP_Doc*_tmp6E3=Cyc_PP_text(({const char*_tmp2DD=")";_tag_fat(_tmp2DD,sizeof(char),2U);}));_tmp2DB[2]=_tmp6E3;}),({struct Cyc_PP_Doc*_tmp6E2=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2DB[3]=_tmp6E2;});Cyc_PP_cat(_tag_fat(_tmp2DB,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1F: _tmp2A1=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp2A1;
s=({struct Cyc_PP_Doc*_tmp2DE[2U];({struct Cyc_PP_Doc*_tmp6E7=Cyc_PP_text(({const char*_tmp2DF="&";_tag_fat(_tmp2DF,sizeof(char),2U);}));_tmp2DE[0]=_tmp6E7;}),({struct Cyc_PP_Doc*_tmp6E6=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2DE[1]=_tmp6E6;});Cyc_PP_cat(_tag_fat(_tmp2DE,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 16U: _LL21: _tmp29F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp2A0=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL22: {struct Cyc_Absyn_Exp*ropt=_tmp29F;struct Cyc_Absyn_Exp*e1=_tmp2A0;
# 1228
if(ropt == 0)
s=({struct Cyc_PP_Doc*_tmp2E0[2U];({struct Cyc_PP_Doc*_tmp6E9=Cyc_PP_text(({const char*_tmp2E1="new ";_tag_fat(_tmp2E1,sizeof(char),5U);}));_tmp2E0[0]=_tmp6E9;}),({struct Cyc_PP_Doc*_tmp6E8=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2E0[1]=_tmp6E8;});Cyc_PP_cat(_tag_fat(_tmp2E0,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1231
s=({struct Cyc_PP_Doc*_tmp2E2[4U];({struct Cyc_PP_Doc*_tmp6ED=Cyc_PP_text(({const char*_tmp2E3="rnew(";_tag_fat(_tmp2E3,sizeof(char),6U);}));_tmp2E2[0]=_tmp6ED;}),({struct Cyc_PP_Doc*_tmp6EC=Cyc_Absynpp_exp2doc(ropt);_tmp2E2[1]=_tmp6EC;}),({struct Cyc_PP_Doc*_tmp6EB=Cyc_PP_text(({const char*_tmp2E4=") ";_tag_fat(_tmp2E4,sizeof(char),3U);}));_tmp2E2[2]=_tmp6EB;}),({struct Cyc_PP_Doc*_tmp6EA=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2E2[3]=_tmp6EA;});Cyc_PP_cat(_tag_fat(_tmp2E2,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;}case 17U: _LL23: _tmp29E=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL24: {void*t=_tmp29E;
s=({struct Cyc_PP_Doc*_tmp2E5[3U];({struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_text(({const char*_tmp2E6="sizeof(";_tag_fat(_tmp2E6,sizeof(char),8U);}));_tmp2E5[0]=_tmp6F0;}),({struct Cyc_PP_Doc*_tmp6EF=Cyc_Absynpp_typ2doc(t);_tmp2E5[1]=_tmp6EF;}),({struct Cyc_PP_Doc*_tmp6EE=Cyc_PP_text(({const char*_tmp2E7=")";_tag_fat(_tmp2E7,sizeof(char),2U);}));_tmp2E5[2]=_tmp6EE;});Cyc_PP_cat(_tag_fat(_tmp2E5,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 18U: _LL25: _tmp29D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp29D;
s=({struct Cyc_PP_Doc*_tmp2E8[3U];({struct Cyc_PP_Doc*_tmp6F3=Cyc_PP_text(({const char*_tmp2E9="sizeof(";_tag_fat(_tmp2E9,sizeof(char),8U);}));_tmp2E8[0]=_tmp6F3;}),({struct Cyc_PP_Doc*_tmp6F2=Cyc_Absynpp_exp2doc(e1);_tmp2E8[1]=_tmp6F2;}),({struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_text(({const char*_tmp2EA=")";_tag_fat(_tmp2EA,sizeof(char),2U);}));_tmp2E8[2]=_tmp6F1;});Cyc_PP_cat(_tag_fat(_tmp2E8,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 42U: _LL27: _tmp29C=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp29C;
# 1236
s=({struct Cyc_PP_Doc*_tmp2EB[3U];({struct Cyc_PP_Doc*_tmp6F6=Cyc_PP_text(({const char*_tmp2EC="@assert(";_tag_fat(_tmp2EC,sizeof(char),9U);}));_tmp2EB[0]=_tmp6F6;}),({struct Cyc_PP_Doc*_tmp6F5=Cyc_Absynpp_exp2doc(e);_tmp2EB[1]=_tmp6F5;}),({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp2ED=")";_tag_fat(_tmp2ED,sizeof(char),2U);}));_tmp2EB[2]=_tmp6F4;});Cyc_PP_cat(_tag_fat(_tmp2EB,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 41U: _LL29: _tmp29B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp29B;
# 1238
s=({struct Cyc_PP_Doc*_tmp2EE[3U];({struct Cyc_PP_Doc*_tmp6F9=Cyc_PP_text(({const char*_tmp2EF="__extension__(";_tag_fat(_tmp2EF,sizeof(char),15U);}));_tmp2EE[0]=_tmp6F9;}),({struct Cyc_PP_Doc*_tmp6F8=Cyc_Absynpp_exp2doc(e);_tmp2EE[1]=_tmp6F8;}),({struct Cyc_PP_Doc*_tmp6F7=Cyc_PP_text(({const char*_tmp2F0=")";_tag_fat(_tmp2F0,sizeof(char),2U);}));_tmp2EE[2]=_tmp6F7;});Cyc_PP_cat(_tag_fat(_tmp2EE,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 39U: _LL2B: _tmp29A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL2C: {void*t=_tmp29A;
s=({struct Cyc_PP_Doc*_tmp2F1[3U];({struct Cyc_PP_Doc*_tmp6FC=Cyc_PP_text(({const char*_tmp2F2="valueof(";_tag_fat(_tmp2F2,sizeof(char),9U);}));_tmp2F1[0]=_tmp6FC;}),({struct Cyc_PP_Doc*_tmp6FB=Cyc_Absynpp_typ2doc(t);_tmp2F1[1]=_tmp6FB;}),({struct Cyc_PP_Doc*_tmp6FA=Cyc_PP_text(({const char*_tmp2F3=")";_tag_fat(_tmp2F3,sizeof(char),2U);}));_tmp2F1[2]=_tmp6FA;});Cyc_PP_cat(_tag_fat(_tmp2F1,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 40U: _LL2D: _tmp295=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp296=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_tmp297=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26E)->f3;_tmp298=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26E)->f4;_tmp299=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp26E)->f5;_LL2E: {int vol=_tmp295;struct _fat_ptr t=_tmp296;struct Cyc_List_List*o=_tmp297;struct Cyc_List_List*i=_tmp298;struct Cyc_List_List*cl=_tmp299;
# 1241
if(vol)
s=({struct Cyc_PP_Doc*_tmp2F4[7U];({struct Cyc_PP_Doc*_tmp703=Cyc_PP_text(({const char*_tmp2F5="__asm__";_tag_fat(_tmp2F5,sizeof(char),8U);}));_tmp2F4[0]=_tmp703;}),({struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp2F6=" volatile (";_tag_fat(_tmp2F6,sizeof(char),12U);}));_tmp2F4[1]=_tmp702;}),({struct Cyc_PP_Doc*_tmp701=Cyc_PP_text(({const char*_tmp2F7="\"";_tag_fat(_tmp2F7,sizeof(char),2U);}));_tmp2F4[2]=_tmp701;}),({struct Cyc_PP_Doc*_tmp700=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_tmp2F4[3]=_tmp700;}),({struct Cyc_PP_Doc*_tmp6FF=Cyc_PP_text(({const char*_tmp2F8="\"";_tag_fat(_tmp2F8,sizeof(char),2U);}));_tmp2F4[4]=_tmp6FF;}),({struct Cyc_PP_Doc*_tmp6FE=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2F4[5]=_tmp6FE;}),({struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_text(({const char*_tmp2F9=")";_tag_fat(_tmp2F9,sizeof(char),2U);}));_tmp2F4[6]=_tmp6FD;});Cyc_PP_cat(_tag_fat(_tmp2F4,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1244
s=({struct Cyc_PP_Doc*_tmp2FA[6U];({struct Cyc_PP_Doc*_tmp709=Cyc_PP_text(({const char*_tmp2FB="__asm__(";_tag_fat(_tmp2FB,sizeof(char),9U);}));_tmp2FA[0]=_tmp709;}),({struct Cyc_PP_Doc*_tmp708=Cyc_PP_text(({const char*_tmp2FC="\"";_tag_fat(_tmp2FC,sizeof(char),2U);}));_tmp2FA[1]=_tmp708;}),({struct Cyc_PP_Doc*_tmp707=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_tmp2FA[2]=_tmp707;}),({struct Cyc_PP_Doc*_tmp706=Cyc_PP_text(({const char*_tmp2FD="\"";_tag_fat(_tmp2FD,sizeof(char),2U);}));_tmp2FA[3]=_tmp706;}),({struct Cyc_PP_Doc*_tmp705=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2FA[4]=_tmp705;}),({struct Cyc_PP_Doc*_tmp704=Cyc_PP_text(({const char*_tmp2FE=")";_tag_fat(_tmp2FE,sizeof(char),2U);}));_tmp2FA[5]=_tmp704;});Cyc_PP_cat(_tag_fat(_tmp2FA,sizeof(struct Cyc_PP_Doc*),6U));});}
goto _LL0;}case 38U: _LL2F: _tmp293=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp294=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL30: {struct Cyc_Absyn_Exp*e=_tmp293;struct _fat_ptr*f=_tmp294;
# 1247
s=({struct Cyc_PP_Doc*_tmp2FF[5U];({struct Cyc_PP_Doc*_tmp70E=Cyc_PP_text(({const char*_tmp300="tagcheck(";_tag_fat(_tmp300,sizeof(char),10U);}));_tmp2FF[0]=_tmp70E;}),({struct Cyc_PP_Doc*_tmp70D=Cyc_Absynpp_exp2doc(e);_tmp2FF[1]=_tmp70D;}),({struct Cyc_PP_Doc*_tmp70C=Cyc_PP_text(({const char*_tmp301=".";_tag_fat(_tmp301,sizeof(char),2U);}));_tmp2FF[2]=_tmp70C;}),({struct Cyc_PP_Doc*_tmp70B=Cyc_PP_textptr(f);_tmp2FF[3]=_tmp70B;}),({struct Cyc_PP_Doc*_tmp70A=Cyc_PP_text(({const char*_tmp302=")";_tag_fat(_tmp302,sizeof(char),2U);}));_tmp2FF[4]=_tmp70A;});Cyc_PP_cat(_tag_fat(_tmp2FF,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 19U: _LL31: _tmp291=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp292=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL32: {void*t=_tmp291;struct Cyc_List_List*l=_tmp292;
# 1250
s=({struct Cyc_PP_Doc*_tmp303[5U];({struct Cyc_PP_Doc*_tmp714=Cyc_PP_text(({const char*_tmp304="offsetof(";_tag_fat(_tmp304,sizeof(char),10U);}));_tmp303[0]=_tmp714;}),({struct Cyc_PP_Doc*_tmp713=Cyc_Absynpp_typ2doc(t);_tmp303[1]=_tmp713;}),({struct Cyc_PP_Doc*_tmp712=Cyc_PP_text(({const char*_tmp305=",";_tag_fat(_tmp305,sizeof(char),2U);}));_tmp303[2]=_tmp712;}),({
struct Cyc_PP_Doc*_tmp711=({struct _fat_ptr _tmp710=({const char*_tmp306=".";_tag_fat(_tmp306,sizeof(char),2U);});Cyc_PP_seq(_tmp710,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_offsetof_field_to_doc,l));});_tmp303[3]=_tmp711;}),({struct Cyc_PP_Doc*_tmp70F=Cyc_PP_text(({const char*_tmp307=")";_tag_fat(_tmp307,sizeof(char),2U);}));_tmp303[4]=_tmp70F;});Cyc_PP_cat(_tag_fat(_tmp303,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 20U: _LL33: _tmp290=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp290;
s=({struct Cyc_PP_Doc*_tmp308[2U];({struct Cyc_PP_Doc*_tmp716=Cyc_PP_text(({const char*_tmp309="*";_tag_fat(_tmp309,sizeof(char),2U);}));_tmp308[0]=_tmp716;}),({struct Cyc_PP_Doc*_tmp715=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp308[1]=_tmp715;});Cyc_PP_cat(_tag_fat(_tmp308,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 21U: _LL35: _tmp28E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp28F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp28E;struct _fat_ptr*n=_tmp28F;
# 1255
s=({struct Cyc_PP_Doc*_tmp30A[3U];({struct Cyc_PP_Doc*_tmp719=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp30A[0]=_tmp719;}),({struct Cyc_PP_Doc*_tmp718=Cyc_PP_text(({const char*_tmp30B=".";_tag_fat(_tmp30B,sizeof(char),2U);}));_tmp30A[1]=_tmp718;}),({struct Cyc_PP_Doc*_tmp717=Cyc_PP_textptr(n);_tmp30A[2]=_tmp717;});Cyc_PP_cat(_tag_fat(_tmp30A,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 22U: _LL37: _tmp28C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp28D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp28C;struct _fat_ptr*n=_tmp28D;
# 1257
s=({struct Cyc_PP_Doc*_tmp30C[3U];({struct Cyc_PP_Doc*_tmp71C=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp30C[0]=_tmp71C;}),({struct Cyc_PP_Doc*_tmp71B=Cyc_PP_text(({const char*_tmp30D="->";_tag_fat(_tmp30D,sizeof(char),3U);}));_tmp30C[1]=_tmp71B;}),({struct Cyc_PP_Doc*_tmp71A=Cyc_PP_textptr(n);_tmp30C[2]=_tmp71A;});Cyc_PP_cat(_tag_fat(_tmp30C,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 23U: _LL39: _tmp28A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp28B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp28A;struct Cyc_Absyn_Exp*e2=_tmp28B;
# 1259
s=({struct Cyc_PP_Doc*_tmp30E[4U];({struct Cyc_PP_Doc*_tmp720=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp30E[0]=_tmp720;}),({struct Cyc_PP_Doc*_tmp71F=Cyc_PP_text(({const char*_tmp30F="[";_tag_fat(_tmp30F,sizeof(char),2U);}));_tmp30E[1]=_tmp71F;}),({struct Cyc_PP_Doc*_tmp71E=Cyc_Absynpp_exp2doc(e2);_tmp30E[2]=_tmp71E;}),({struct Cyc_PP_Doc*_tmp71D=Cyc_PP_text(({const char*_tmp310="]";_tag_fat(_tmp310,sizeof(char),2U);}));_tmp30E[3]=_tmp71D;});Cyc_PP_cat(_tag_fat(_tmp30E,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 24U: _LL3B: _tmp289=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL3C: {struct Cyc_List_List*es=_tmp289;
# 1261
s=({struct Cyc_PP_Doc*_tmp311[4U];({struct Cyc_PP_Doc*_tmp724=Cyc_Absynpp_dollar();_tmp311[0]=_tmp724;}),({struct Cyc_PP_Doc*_tmp723=Cyc_PP_text(({const char*_tmp312="(";_tag_fat(_tmp312,sizeof(char),2U);}));_tmp311[1]=_tmp723;}),({struct Cyc_PP_Doc*_tmp722=Cyc_Absynpp_exps2doc_prec(20,es);_tmp311[2]=_tmp722;}),({struct Cyc_PP_Doc*_tmp721=Cyc_PP_text(({const char*_tmp313=")";_tag_fat(_tmp313,sizeof(char),2U);}));_tmp311[3]=_tmp721;});Cyc_PP_cat(_tag_fat(_tmp311,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 25U: _LL3D: _tmp287=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp288=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL3E: {struct _tuple8*vat=_tmp287;struct Cyc_List_List*des=_tmp288;
# 1263
s=({struct Cyc_PP_Doc*_tmp314[4U];({struct Cyc_PP_Doc*_tmp729=Cyc_PP_text(({const char*_tmp315="(";_tag_fat(_tmp315,sizeof(char),2U);}));_tmp314[0]=_tmp729;}),({struct Cyc_PP_Doc*_tmp728=Cyc_Absynpp_typ2doc((*vat).f3);_tmp314[1]=_tmp728;}),({struct Cyc_PP_Doc*_tmp727=Cyc_PP_text(({const char*_tmp316=")";_tag_fat(_tmp316,sizeof(char),2U);}));_tmp314[2]=_tmp727;}),({
struct Cyc_PP_Doc*_tmp726=({struct _fat_ptr _tmp725=({const char*_tmp317=",";_tag_fat(_tmp317,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp725,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});_tmp314[3]=_tmp726;});Cyc_PP_cat(_tag_fat(_tmp314,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 26U: _LL3F: _tmp286=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL40: {struct Cyc_List_List*des=_tmp286;
s=({struct _fat_ptr _tmp72A=({const char*_tmp318=",";_tag_fat(_tmp318,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp72A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 27U: _LL41: _tmp283=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp284=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_tmp285=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp26E)->f3;_LL42: {struct Cyc_Absyn_Vardecl*vd=_tmp283;struct Cyc_Absyn_Exp*e1=_tmp284;struct Cyc_Absyn_Exp*e2=_tmp285;
# 1268
s=({struct Cyc_PP_Doc*_tmp319[8U];({struct Cyc_PP_Doc*_tmp732=Cyc_Absynpp_lb();_tmp319[0]=_tmp732;}),({struct Cyc_PP_Doc*_tmp731=Cyc_PP_text(({const char*_tmp31A="for ";_tag_fat(_tmp31A,sizeof(char),5U);}));_tmp319[1]=_tmp731;}),({
struct Cyc_PP_Doc*_tmp730=Cyc_PP_text(*(*vd->name).f2);_tmp319[2]=_tmp730;}),({struct Cyc_PP_Doc*_tmp72F=Cyc_PP_text(({const char*_tmp31B=" < ";_tag_fat(_tmp31B,sizeof(char),4U);}));_tmp319[3]=_tmp72F;}),({struct Cyc_PP_Doc*_tmp72E=Cyc_Absynpp_exp2doc(e1);_tmp319[4]=_tmp72E;}),({struct Cyc_PP_Doc*_tmp72D=Cyc_PP_text(({const char*_tmp31C=" : ";_tag_fat(_tmp31C,sizeof(char),4U);}));_tmp319[5]=_tmp72D;}),({
struct Cyc_PP_Doc*_tmp72C=Cyc_Absynpp_exp2doc(e2);_tmp319[6]=_tmp72C;}),({struct Cyc_PP_Doc*_tmp72B=Cyc_Absynpp_rb();_tmp319[7]=_tmp72B;});Cyc_PP_cat(_tag_fat(_tmp319,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 28U: _LL43: _tmp281=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp282=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL44: {struct Cyc_Absyn_Exp*e=_tmp281;void*t=_tmp282;
# 1273
s=({struct Cyc_PP_Doc*_tmp31D[7U];({struct Cyc_PP_Doc*_tmp739=Cyc_Absynpp_lb();_tmp31D[0]=_tmp739;}),({struct Cyc_PP_Doc*_tmp738=Cyc_PP_text(({const char*_tmp31E="for x ";_tag_fat(_tmp31E,sizeof(char),7U);}));_tmp31D[1]=_tmp738;}),({
struct Cyc_PP_Doc*_tmp737=Cyc_PP_text(({const char*_tmp31F=" < ";_tag_fat(_tmp31F,sizeof(char),4U);}));_tmp31D[2]=_tmp737;}),({
struct Cyc_PP_Doc*_tmp736=Cyc_Absynpp_exp2doc(e);_tmp31D[3]=_tmp736;}),({
struct Cyc_PP_Doc*_tmp735=Cyc_PP_text(({const char*_tmp320=" : ";_tag_fat(_tmp320,sizeof(char),4U);}));_tmp31D[4]=_tmp735;}),({
struct Cyc_PP_Doc*_tmp734=Cyc_Absynpp_typ2doc(t);_tmp31D[5]=_tmp734;}),({
struct Cyc_PP_Doc*_tmp733=Cyc_Absynpp_rb();_tmp31D[6]=_tmp733;});Cyc_PP_cat(_tag_fat(_tmp31D,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 29U: _LL45: _tmp27E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp27F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_tmp280=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp26E)->f3;_LL46: {struct _tuple0*n=_tmp27E;struct Cyc_List_List*ts=_tmp27F;struct Cyc_List_List*des=_tmp280;
# 1281
struct Cyc_List_List*_tmp321=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des);struct Cyc_List_List*des_doc=_tmp321;
s=({struct Cyc_PP_Doc*_tmp322[2U];({struct Cyc_PP_Doc*_tmp73D=Cyc_Absynpp_qvar2doc(n);_tmp322[0]=_tmp73D;}),({
struct Cyc_PP_Doc*_tmp73C=({struct _fat_ptr _tmp73B=({const char*_tmp323=",";_tag_fat(_tmp323,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp73B,
ts != 0?({struct Cyc_List_List*_tmp324=_cycalloc(sizeof(*_tmp324));({struct Cyc_PP_Doc*_tmp73A=Cyc_Absynpp_tps2doc(ts);_tmp324->hd=_tmp73A;}),_tmp324->tl=des_doc;_tmp324;}): des_doc);});
# 1283
_tmp322[1]=_tmp73C;});Cyc_PP_cat(_tag_fat(_tmp322,sizeof(struct Cyc_PP_Doc*),2U));});
# 1285
goto _LL0;}case 30U: _LL47: _tmp27D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL48: {struct Cyc_List_List*des=_tmp27D;
s=({struct _fat_ptr _tmp73E=({const char*_tmp325=",";_tag_fat(_tmp325,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp73E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 31U: _LL49: _tmp27B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp27C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp26E)->f3;_LL4A: {struct Cyc_List_List*es=_tmp27B;struct Cyc_Absyn_Datatypefield*tuf=_tmp27C;
# 1288
if(es == 0)
# 1290
s=Cyc_Absynpp_qvar2doc(tuf->name);else{
# 1292
s=({struct Cyc_PP_Doc*_tmp326[2U];({struct Cyc_PP_Doc*_tmp743=Cyc_Absynpp_qvar2doc(tuf->name);_tmp326[0]=_tmp743;}),({
struct Cyc_PP_Doc*_tmp742=({struct _fat_ptr _tmp741=({const char*_tmp327="(";_tag_fat(_tmp327,sizeof(char),2U);});struct _fat_ptr _tmp740=({const char*_tmp328=")";_tag_fat(_tmp328,sizeof(char),2U);});struct _fat_ptr _tmp73F=({const char*_tmp329=",";_tag_fat(_tmp329,sizeof(char),2U);});Cyc_PP_egroup(_tmp741,_tmp740,_tmp73F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,es));});_tmp326[1]=_tmp742;});Cyc_PP_cat(_tag_fat(_tmp326,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;}case 32U: _LL4B: _tmp27A=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL4C: {struct Cyc_Absyn_Enumfield*ef=_tmp27A;
_tmp279=ef;goto _LL4E;}case 33U: _LL4D: _tmp279=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL4E: {struct Cyc_Absyn_Enumfield*ef=_tmp279;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 34U: _LL4F: _tmp274=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26E)->f1).is_calloc;_tmp275=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26E)->f1).rgn;_tmp276=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26E)->f1).elt_type;_tmp277=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26E)->f1).num_elts;_tmp278=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp26E)->f1).inline_call;_LL50: {int is_calloc=_tmp274;struct Cyc_Absyn_Exp*rgnopt=_tmp275;void**topt=_tmp276;struct Cyc_Absyn_Exp*e=_tmp277;int inline_call=_tmp278;
# 1300
if(is_calloc){
# 1302
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U);
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp32A[5U];({struct Cyc_PP_Doc*_tmp748=Cyc_PP_text(({const char*_tmp32B="calloc(";_tag_fat(_tmp32B,sizeof(char),8U);}));_tmp32A[0]=_tmp748;}),({struct Cyc_PP_Doc*_tmp747=Cyc_Absynpp_exp2doc(e);_tmp32A[1]=_tmp747;}),({struct Cyc_PP_Doc*_tmp746=Cyc_PP_text(({const char*_tmp32C=",";_tag_fat(_tmp32C,sizeof(char),2U);}));_tmp32A[2]=_tmp746;}),({struct Cyc_PP_Doc*_tmp745=Cyc_Absynpp_exp2doc(st);_tmp32A[3]=_tmp745;}),({struct Cyc_PP_Doc*_tmp744=Cyc_PP_text(({const char*_tmp32D=")";_tag_fat(_tmp32D,sizeof(char),2U);}));_tmp32A[4]=_tmp744;});Cyc_PP_cat(_tag_fat(_tmp32A,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1306
s=({struct Cyc_PP_Doc*_tmp32E[7U];({struct Cyc_PP_Doc*_tmp74F=Cyc_PP_text(({const char*_tmp32F="rcalloc(";_tag_fat(_tmp32F,sizeof(char),9U);}));_tmp32E[0]=_tmp74F;}),({struct Cyc_PP_Doc*_tmp74E=Cyc_Absynpp_exp2doc(rgnopt);_tmp32E[1]=_tmp74E;}),({struct Cyc_PP_Doc*_tmp74D=Cyc_PP_text(({const char*_tmp330=",";_tag_fat(_tmp330,sizeof(char),2U);}));_tmp32E[2]=_tmp74D;}),({
struct Cyc_PP_Doc*_tmp74C=Cyc_Absynpp_exp2doc(e);_tmp32E[3]=_tmp74C;}),({struct Cyc_PP_Doc*_tmp74B=Cyc_PP_text(({const char*_tmp331=",";_tag_fat(_tmp331,sizeof(char),2U);}));_tmp32E[4]=_tmp74B;}),({struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_exp2doc(st);_tmp32E[5]=_tmp74A;}),({struct Cyc_PP_Doc*_tmp749=Cyc_PP_text(({const char*_tmp332=")";_tag_fat(_tmp332,sizeof(char),2U);}));_tmp32E[6]=_tmp749;});Cyc_PP_cat(_tag_fat(_tmp32E,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1309
struct Cyc_Absyn_Exp*new_e;
# 1311
if(topt == 0)
new_e=e;else{
# 1314
new_e=({struct Cyc_Absyn_Exp*_tmp750=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp750,e,0U);});}
# 1316
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp333[3U];({struct Cyc_PP_Doc*_tmp753=Cyc_PP_text(({const char*_tmp334="malloc(";_tag_fat(_tmp334,sizeof(char),8U);}));_tmp333[0]=_tmp753;}),({struct Cyc_PP_Doc*_tmp752=Cyc_Absynpp_exp2doc(new_e);_tmp333[1]=_tmp752;}),({struct Cyc_PP_Doc*_tmp751=Cyc_PP_text(({const char*_tmp335=")";_tag_fat(_tmp335,sizeof(char),2U);}));_tmp333[2]=_tmp751;});Cyc_PP_cat(_tag_fat(_tmp333,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1319
if(inline_call)
s=({struct Cyc_PP_Doc*_tmp336[5U];({struct Cyc_PP_Doc*_tmp758=Cyc_PP_text(({const char*_tmp337="rmalloc_inline(";_tag_fat(_tmp337,sizeof(char),16U);}));_tmp336[0]=_tmp758;}),({struct Cyc_PP_Doc*_tmp757=Cyc_Absynpp_exp2doc(rgnopt);_tmp336[1]=_tmp757;}),({struct Cyc_PP_Doc*_tmp756=Cyc_PP_text(({const char*_tmp338=",";_tag_fat(_tmp338,sizeof(char),2U);}));_tmp336[2]=_tmp756;}),({
struct Cyc_PP_Doc*_tmp755=Cyc_Absynpp_exp2doc(new_e);_tmp336[3]=_tmp755;}),({struct Cyc_PP_Doc*_tmp754=Cyc_PP_text(({const char*_tmp339=")";_tag_fat(_tmp339,sizeof(char),2U);}));_tmp336[4]=_tmp754;});Cyc_PP_cat(_tag_fat(_tmp336,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1323
s=({struct Cyc_PP_Doc*_tmp33A[5U];({struct Cyc_PP_Doc*_tmp75D=Cyc_PP_text(({const char*_tmp33B="rmalloc(";_tag_fat(_tmp33B,sizeof(char),9U);}));_tmp33A[0]=_tmp75D;}),({struct Cyc_PP_Doc*_tmp75C=Cyc_Absynpp_exp2doc(rgnopt);_tmp33A[1]=_tmp75C;}),({struct Cyc_PP_Doc*_tmp75B=Cyc_PP_text(({const char*_tmp33C=",";_tag_fat(_tmp33C,sizeof(char),2U);}));_tmp33A[2]=_tmp75B;}),({
struct Cyc_PP_Doc*_tmp75A=Cyc_Absynpp_exp2doc(new_e);_tmp33A[3]=_tmp75A;}),({struct Cyc_PP_Doc*_tmp759=Cyc_PP_text(({const char*_tmp33D=")";_tag_fat(_tmp33D,sizeof(char),2U);}));_tmp33A[4]=_tmp759;});Cyc_PP_cat(_tag_fat(_tmp33A,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1327
goto _LL0;}case 35U: _LL51: _tmp272=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp273=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp272;struct Cyc_Absyn_Exp*e2=_tmp273;
# 1329
s=({struct Cyc_PP_Doc*_tmp33E[3U];({struct Cyc_PP_Doc*_tmp760=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp33E[0]=_tmp760;}),({struct Cyc_PP_Doc*_tmp75F=Cyc_PP_text(({const char*_tmp33F=" :=: ";_tag_fat(_tmp33F,sizeof(char),6U);}));_tmp33E[1]=_tmp75F;}),({struct Cyc_PP_Doc*_tmp75E=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp33E[2]=_tmp75E;});Cyc_PP_cat(_tag_fat(_tmp33E,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 36U: _LL53: _tmp270=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp271=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL54: {struct Cyc_Core_Opt*n=_tmp270;struct Cyc_List_List*des=_tmp271;
# 1332
s=({struct _fat_ptr _tmp761=({const char*_tmp340=",";_tag_fat(_tmp340,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp761,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _LL55: _tmp26F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL56: {struct Cyc_Absyn_Stmt*s2=_tmp26F;
# 1335
s=({struct Cyc_PP_Doc*_tmp341[7U];({struct Cyc_PP_Doc*_tmp768=Cyc_PP_text(({const char*_tmp342="(";_tag_fat(_tmp342,sizeof(char),2U);}));_tmp341[0]=_tmp768;}),({struct Cyc_PP_Doc*_tmp767=Cyc_Absynpp_lb();_tmp341[1]=_tmp767;}),({struct Cyc_PP_Doc*_tmp766=Cyc_PP_blank_doc();_tmp341[2]=_tmp766;}),({
struct Cyc_PP_Doc*_tmp765=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_tmp341[3]=_tmp765;}),({
struct Cyc_PP_Doc*_tmp764=Cyc_PP_blank_doc();_tmp341[4]=_tmp764;}),({struct Cyc_PP_Doc*_tmp763=Cyc_Absynpp_rb();_tmp341[5]=_tmp763;}),({struct Cyc_PP_Doc*_tmp762=Cyc_PP_text(({const char*_tmp343=")";_tag_fat(_tmp343,sizeof(char),2U);}));_tmp341[6]=_tmp762;});Cyc_PP_cat(_tag_fat(_tmp341,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}}_LL0:;}
# 1340
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp344[3U];({struct Cyc_PP_Doc*_tmp76A=Cyc_PP_text(({const char*_tmp345="(";_tag_fat(_tmp345,sizeof(char),2U);}));_tmp344[0]=_tmp76A;}),_tmp344[1]=s,({struct Cyc_PP_Doc*_tmp769=Cyc_PP_text(({const char*_tmp346=")";_tag_fat(_tmp346,sizeof(char),2U);}));_tmp344[2]=_tmp769;});Cyc_PP_cat(_tag_fat(_tmp344,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1345
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp347=d;struct _fat_ptr*_tmp348;struct Cyc_Absyn_Exp*_tmp349;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp347)->tag == 0U){_LL1: _tmp349=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp347)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp349;
return({struct Cyc_PP_Doc*_tmp34A[3U];({struct Cyc_PP_Doc*_tmp76D=Cyc_PP_text(({const char*_tmp34B=".[";_tag_fat(_tmp34B,sizeof(char),3U);}));_tmp34A[0]=_tmp76D;}),({struct Cyc_PP_Doc*_tmp76C=Cyc_Absynpp_exp2doc(e);_tmp34A[1]=_tmp76C;}),({struct Cyc_PP_Doc*_tmp76B=Cyc_PP_text(({const char*_tmp34C="]";_tag_fat(_tmp34C,sizeof(char),2U);}));_tmp34A[2]=_tmp76B;});Cyc_PP_cat(_tag_fat(_tmp34A,sizeof(struct Cyc_PP_Doc*),3U));});}}else{_LL3: _tmp348=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp347)->f1;_LL4: {struct _fat_ptr*v=_tmp348;
return({struct Cyc_PP_Doc*_tmp34D[2U];({struct Cyc_PP_Doc*_tmp76F=Cyc_PP_text(({const char*_tmp34E=".";_tag_fat(_tmp34E,sizeof(char),2U);}));_tmp34D[0]=_tmp76F;}),({struct Cyc_PP_Doc*_tmp76E=Cyc_PP_textptr(v);_tmp34D[1]=_tmp76E;});Cyc_PP_cat(_tag_fat(_tmp34D,sizeof(struct Cyc_PP_Doc*),2U));});}}_LL0:;}
# 1352
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp34F[2U];({struct Cyc_PP_Doc*_tmp774=({struct _fat_ptr _tmp773=({const char*_tmp350="";_tag_fat(_tmp350,sizeof(char),1U);});struct _fat_ptr _tmp772=({const char*_tmp351="=";_tag_fat(_tmp351,sizeof(char),2U);});struct _fat_ptr _tmp771=({const char*_tmp352="=";_tag_fat(_tmp352,sizeof(char),2U);});Cyc_PP_egroup(_tmp773,_tmp772,_tmp771,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp34F[0]=_tmp774;}),({
struct Cyc_PP_Doc*_tmp770=Cyc_Absynpp_exp2doc((*de).f2);_tmp34F[1]=_tmp770;});Cyc_PP_cat(_tag_fat(_tmp34F,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1358
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _tmp777=({const char*_tmp353="";_tag_fat(_tmp353,sizeof(char),1U);});struct _fat_ptr _tmp776=({const char*_tmp354="";_tag_fat(_tmp354,sizeof(char),1U);});struct _fat_ptr _tmp775=({const char*_tmp355=",";_tag_fat(_tmp355,sizeof(char),2U);});Cyc_PP_group(_tmp777,_tmp776,_tmp775,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1362
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_tmp36A=({unsigned _tmp369=28U + 1U;char*_tmp368=_cycalloc_atomic(_check_times(_tmp369,sizeof(char)));({{unsigned _tmp5CF=28U;unsigned i;for(i=0;i < _tmp5CF;++ i){_tmp368[i]='z';}_tmp368[_tmp5CF]=0;}0;});_tmp368;});_tag_fat(_tmp36A,sizeof(char),29U);});
({struct _fat_ptr _tmp356=_fat_ptr_plus(x,sizeof(char),27);char _tmp357=*((char*)_check_fat_subscript(_tmp356,sizeof(char),0U));char _tmp358='\000';if(_get_fat_size(_tmp356,sizeof(char))== 1U &&(_tmp357 == 0 && _tmp358 != 0))_throw_arraybounds();*((char*)_tmp356.curr)=_tmp358;});
({struct _fat_ptr _tmp359=_fat_ptr_plus(x,sizeof(char),26);char _tmp35A=*((char*)_check_fat_subscript(_tmp359,sizeof(char),0U));char _tmp35B='L';if(_get_fat_size(_tmp359,sizeof(char))== 1U &&(_tmp35A == 0 && _tmp35B != 0))_throw_arraybounds();*((char*)_tmp359.curr)=_tmp35B;});
({struct _fat_ptr _tmp35C=_fat_ptr_plus(x,sizeof(char),25);char _tmp35D=*((char*)_check_fat_subscript(_tmp35C,sizeof(char),0U));char _tmp35E='L';if(_get_fat_size(_tmp35C,sizeof(char))== 1U &&(_tmp35D == 0 && _tmp35E != 0))_throw_arraybounds();*((char*)_tmp35C.curr)=_tmp35E;});
({struct _fat_ptr _tmp35F=_fat_ptr_plus(x,sizeof(char),24);char _tmp360=*((char*)_check_fat_subscript(_tmp35F,sizeof(char),0U));char _tmp361='U';if(_get_fat_size(_tmp35F,sizeof(char))== 1U &&(_tmp360 == 0 && _tmp361 != 0))_throw_arraybounds();*((char*)_tmp35F.curr)=_tmp361;});
({struct _fat_ptr _tmp362=_fat_ptr_plus(x,sizeof(char),23);char _tmp363=*((char*)_check_fat_subscript(_tmp362,sizeof(char),0U));char _tmp364='0';if(_get_fat_size(_tmp362,sizeof(char))== 1U &&(_tmp363 == 0 && _tmp364 != 0))_throw_arraybounds();*((char*)_tmp362.curr)=_tmp364;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _fat_ptr _tmp365=_fat_ptr_plus(x,sizeof(char),index);char _tmp366=*((char*)_check_fat_subscript(_tmp365,sizeof(char),0U));char _tmp367=c;if(_get_fat_size(_tmp365,sizeof(char))== 1U &&(_tmp366 == 0 && _tmp367 != 0))_throw_arraybounds();*((char*)_tmp365.curr)=_tmp367;});
i=i / (unsigned long long)10;
-- index;}
# 1376
return(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1380
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp36B=c;struct _fat_ptr _tmp36C;struct _fat_ptr _tmp36D;struct _fat_ptr _tmp36E;long long _tmp370;enum Cyc_Absyn_Sign _tmp36F;int _tmp372;enum Cyc_Absyn_Sign _tmp371;short _tmp374;enum Cyc_Absyn_Sign _tmp373;struct _fat_ptr _tmp375;char _tmp377;enum Cyc_Absyn_Sign _tmp376;switch((_tmp36B.String_c).tag){case 2U: _LL1: _tmp376=((_tmp36B.Char_c).val).f1;_tmp377=((_tmp36B.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp376;char ch=_tmp377;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37A=({struct Cyc_String_pa_PrintArg_struct _tmp5D0;_tmp5D0.tag=0U,({struct _fat_ptr _tmp778=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp5D0.f1=_tmp778;});_tmp5D0;});void*_tmp378[1U];_tmp378[0]=& _tmp37A;({struct _fat_ptr _tmp779=({const char*_tmp379="'%s'";_tag_fat(_tmp379,sizeof(char),5U);});Cyc_aprintf(_tmp779,_tag_fat(_tmp378,sizeof(void*),1U));});}));}case 3U: _LL3: _tmp375=(_tmp36B.Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp375;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37D=({struct Cyc_String_pa_PrintArg_struct _tmp5D1;_tmp5D1.tag=0U,_tmp5D1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5D1;});void*_tmp37B[1U];_tmp37B[0]=& _tmp37D;({struct _fat_ptr _tmp77A=({const char*_tmp37C="L'%s'";_tag_fat(_tmp37C,sizeof(char),6U);});Cyc_aprintf(_tmp77A,_tag_fat(_tmp37B,sizeof(void*),1U));});}));}case 4U: _LL5: _tmp373=((_tmp36B.Short_c).val).f1;_tmp374=((_tmp36B.Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp373;short s=_tmp374;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp380=({struct Cyc_Int_pa_PrintArg_struct _tmp5D2;_tmp5D2.tag=1U,_tmp5D2.f1=(unsigned long)((int)s);_tmp5D2;});void*_tmp37E[1U];_tmp37E[0]=& _tmp380;({struct _fat_ptr _tmp77B=({const char*_tmp37F="%d";_tag_fat(_tmp37F,sizeof(char),3U);});Cyc_aprintf(_tmp77B,_tag_fat(_tmp37E,sizeof(void*),1U));});}));}case 5U: _LL7: _tmp371=((_tmp36B.Int_c).val).f1;_tmp372=((_tmp36B.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign sn=_tmp371;int i=_tmp372;
# 1386
if((int)sn == (int)1U)return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp383=({struct Cyc_Int_pa_PrintArg_struct _tmp5D3;_tmp5D3.tag=1U,_tmp5D3.f1=(unsigned)i;_tmp5D3;});void*_tmp381[1U];_tmp381[0]=& _tmp383;({struct _fat_ptr _tmp77C=({const char*_tmp382="%uU";_tag_fat(_tmp382,sizeof(char),4U);});Cyc_aprintf(_tmp77C,_tag_fat(_tmp381,sizeof(void*),1U));});}));else{
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp386=({struct Cyc_Int_pa_PrintArg_struct _tmp5D4;_tmp5D4.tag=1U,_tmp5D4.f1=(unsigned long)i;_tmp5D4;});void*_tmp384[1U];_tmp384[0]=& _tmp386;({struct _fat_ptr _tmp77D=({const char*_tmp385="%d";_tag_fat(_tmp385,sizeof(char),3U);});Cyc_aprintf(_tmp77D,_tag_fat(_tmp384,sizeof(void*),1U));});}));}}case 6U: _LL9: _tmp36F=((_tmp36B.LongLong_c).val).f1;_tmp370=((_tmp36B.LongLong_c).val).f2;_LLA: {enum Cyc_Absyn_Sign sg=_tmp36F;long long i=_tmp370;
# 1390
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7U: _LLB: _tmp36E=((_tmp36B.Float_c).val).f1;_LLC: {struct _fat_ptr x=_tmp36E;
return Cyc_PP_text(x);}case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp387="NULL";_tag_fat(_tmp387,sizeof(char),5U);}));case 8U: _LLF: _tmp36D=(_tmp36B.String_c).val;_LL10: {struct _fat_ptr s=_tmp36D;
return({struct Cyc_PP_Doc*_tmp388[3U];({struct Cyc_PP_Doc*_tmp780=Cyc_PP_text(({const char*_tmp389="\"";_tag_fat(_tmp389,sizeof(char),2U);}));_tmp388[0]=_tmp780;}),({struct Cyc_PP_Doc*_tmp77F=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_tmp388[1]=_tmp77F;}),({struct Cyc_PP_Doc*_tmp77E=Cyc_PP_text(({const char*_tmp38A="\"";_tag_fat(_tmp38A,sizeof(char),2U);}));_tmp388[2]=_tmp77E;});Cyc_PP_cat(_tag_fat(_tmp388,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL11: _tmp36C=(_tmp36B.Wstring_c).val;_LL12: {struct _fat_ptr s=_tmp36C;
return({struct Cyc_PP_Doc*_tmp38B[3U];({struct Cyc_PP_Doc*_tmp783=Cyc_PP_text(({const char*_tmp38C="L\"";_tag_fat(_tmp38C,sizeof(char),3U);}));_tmp38B[0]=_tmp783;}),({struct Cyc_PP_Doc*_tmp782=Cyc_PP_text(s);_tmp38B[1]=_tmp782;}),({struct Cyc_PP_Doc*_tmp781=Cyc_PP_text(({const char*_tmp38D="\"";_tag_fat(_tmp38D,sizeof(char),2U);}));_tmp38B[2]=_tmp781;});Cyc_PP_cat(_tag_fat(_tmp38B,sizeof(struct Cyc_PP_Doc*),3U));});}}_LL0:;}
# 1398
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp786=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp390=({struct Cyc_String_pa_PrintArg_struct _tmp5D5;_tmp5D5.tag=0U,({struct _fat_ptr _tmp784=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5D5.f1=_tmp784;});_tmp5D5;});void*_tmp38E[1U];_tmp38E[0]=& _tmp390;({struct _fat_ptr _tmp785=({const char*_tmp38F="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_tmp38F,sizeof(char),47U);});Cyc_aprintf(_tmp785,_tag_fat(_tmp38E,sizeof(void*),1U));});});_tmp391->f1=_tmp786;});_tmp391;}));
# 1404
return({struct Cyc_PP_Doc*_tmp392[3U];({struct Cyc_PP_Doc*_tmp789=Cyc_PP_text(({const char*_tmp393="numelts(";_tag_fat(_tmp393,sizeof(char),9U);}));_tmp392[0]=_tmp789;}),({struct Cyc_PP_Doc*_tmp788=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp392[1]=_tmp788;}),({struct Cyc_PP_Doc*_tmp787=Cyc_PP_text(({const char*_tmp394=")";_tag_fat(_tmp394,sizeof(char),2U);}));_tmp392[2]=_tmp787;});Cyc_PP_cat(_tag_fat(_tmp392,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1406
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp78C=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp397=({struct Cyc_String_pa_PrintArg_struct _tmp5D6;_tmp5D6.tag=0U,({struct _fat_ptr _tmp78A=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5D6.f1=_tmp78A;});_tmp5D6;});void*_tmp395[1U];_tmp395[0]=& _tmp397;({struct _fat_ptr _tmp78B=({const char*_tmp396="Absynpp::primapp2doc: %s with no args";_tag_fat(_tmp396,sizeof(char),38U);});Cyc_aprintf(_tmp78B,_tag_fat(_tmp395,sizeof(void*),1U));});});_tmp398->f1=_tmp78C;});_tmp398;}));else{
# 1410
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp399[3U];_tmp399[0]=ps,({struct Cyc_PP_Doc*_tmp78D=Cyc_PP_text(({const char*_tmp39A=" ";_tag_fat(_tmp39A,sizeof(char),2U);}));_tmp399[1]=_tmp78D;}),_tmp399[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_tmp399,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp790=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39D=({struct Cyc_String_pa_PrintArg_struct _tmp5D7;_tmp5D7.tag=0U,({struct _fat_ptr _tmp78E=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp5D7.f1=_tmp78E;});_tmp5D7;});void*_tmp39B[1U];_tmp39B[0]=& _tmp39D;({struct _fat_ptr _tmp78F=({const char*_tmp39C="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_tmp39C,sizeof(char),47U);});Cyc_aprintf(_tmp78F,_tag_fat(_tmp39B,sizeof(void*),1U));});});_tmp39E->f1=_tmp790;});_tmp39E;}));else{
# 1416
return({struct Cyc_PP_Doc*_tmp39F[5U];_tmp39F[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp792=Cyc_PP_text(({const char*_tmp3A0=" ";_tag_fat(_tmp3A0,sizeof(char),2U);}));_tmp39F[1]=_tmp792;}),_tmp39F[2]=ps,({struct Cyc_PP_Doc*_tmp791=Cyc_PP_text(({const char*_tmp3A1=" ";_tag_fat(_tmp3A1,sizeof(char),2U);}));_tmp39F[3]=_tmp791;}),_tmp39F[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_tmp39F,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1420
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp3A2=p;switch(_tmp3A2){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp3A3="+";_tag_fat(_tmp3A3,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp3A4="*";_tag_fat(_tmp3A4,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp3A5="-";_tag_fat(_tmp3A5,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp3A6="/";_tag_fat(_tmp3A6,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp3A7="\\%";_tag_fat(_tmp3A7,sizeof(char),3U);}):({const char*_tmp3A8="%";_tag_fat(_tmp3A8,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp3A9="==";_tag_fat(_tmp3A9,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp3AA="!=";_tag_fat(_tmp3AA,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp3AB=">";_tag_fat(_tmp3AB,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp3AC="<";_tag_fat(_tmp3AC,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp3AD=">=";_tag_fat(_tmp3AD,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp3AE="<=";_tag_fat(_tmp3AE,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp3AF="!";_tag_fat(_tmp3AF,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp3B0="~";_tag_fat(_tmp3B0,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp3B1="&";_tag_fat(_tmp3B1,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp3B2="|";_tag_fat(_tmp3B2,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp3B3="^";_tag_fat(_tmp3B3,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp3B4="<<";_tag_fat(_tmp3B4,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp3B5=">>";_tag_fat(_tmp3B5,sizeof(char),3U);});case Cyc_Absyn_Numelts: _LL25: _LL26:
 return({const char*_tmp3B6="numelts";_tag_fat(_tmp3B6,sizeof(char),8U);});default: _LL27: _LL28:
 return({const char*_tmp3B7="?";_tag_fat(_tmp3B7,sizeof(char),2U);});}_LL0:;}
# 1445
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1449
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp3B8=s->r;void*_stmttmp17=_tmp3B8;void*_tmp3B9=_stmttmp17;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B9)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1464 "absynpp.cyc"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_tmp3BA=d->r;void*_stmttmp18=_tmp3BA;void*_tmp3BB=_stmttmp18;struct Cyc_Absyn_Vardecl*_tmp3BC;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3BB)->tag == 0U){_LL1: _tmp3BC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3BB)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3BC;
# 1467
if(((int(*)(int(*compare)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*l,struct _tuple0*x))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple14 _tmp5D8;_tmp5D8.f1=1,({struct Cyc_List_List*_tmp793=({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->hd=vd->name,_tmp3BD->tl=0;_tmp3BD;});_tmp5D8.f2=_tmp793;});_tmp5D8;});else{
# 1470
return({struct _tuple14 _tmp5D9;_tmp5D9.f1=0,({struct Cyc_List_List*_tmp794=({struct Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->hd=vd->name,_tmp3BE->tl=varsinblock;_tmp3BE;});_tmp5D9.f2=_tmp794;});_tmp5D9;});}}}else{_LL3: _LL4:
# 1472
 return({struct _tuple14 _tmp5DA;_tmp5DA.f1=0,_tmp5DA.f2=varsinblock;_tmp5DA;});}_LL0:;}
# 1476
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_tmp3BF[8U];({struct Cyc_PP_Doc*_tmp79C=Cyc_PP_text(({const char*_tmp3C0="(";_tag_fat(_tmp3C0,sizeof(char),2U);}));_tmp3BF[0]=_tmp79C;}),({struct Cyc_PP_Doc*_tmp79B=Cyc_Absynpp_lb();_tmp3BF[1]=_tmp79B;}),({struct Cyc_PP_Doc*_tmp79A=Cyc_PP_blank_doc();_tmp3BF[2]=_tmp79A;}),({struct Cyc_PP_Doc*_tmp799=Cyc_PP_nest(2,d);_tmp3BF[3]=_tmp799;}),({struct Cyc_PP_Doc*_tmp798=Cyc_PP_line_doc();_tmp3BF[4]=_tmp798;}),({struct Cyc_PP_Doc*_tmp797=Cyc_Absynpp_rb();_tmp3BF[5]=_tmp797;}),({
struct Cyc_PP_Doc*_tmp796=Cyc_PP_text(({const char*_tmp3C1=");";_tag_fat(_tmp3C1,sizeof(char),3U);}));_tmp3BF[6]=_tmp796;}),({struct Cyc_PP_Doc*_tmp795=Cyc_PP_line_doc();_tmp3BF[7]=_tmp795;});Cyc_PP_cat(_tag_fat(_tmp3BF,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1481
return({struct Cyc_PP_Doc*_tmp3C2[6U];({struct Cyc_PP_Doc*_tmp7A2=Cyc_Absynpp_lb();_tmp3C2[0]=_tmp7A2;}),({struct Cyc_PP_Doc*_tmp7A1=Cyc_PP_blank_doc();_tmp3C2[1]=_tmp7A1;}),({struct Cyc_PP_Doc*_tmp7A0=Cyc_PP_nest(2,d);_tmp3C2[2]=_tmp7A0;}),({struct Cyc_PP_Doc*_tmp79F=Cyc_PP_line_doc();_tmp3C2[3]=_tmp79F;}),({struct Cyc_PP_Doc*_tmp79E=Cyc_Absynpp_rb();_tmp3C2[4]=_tmp79E;}),({struct Cyc_PP_Doc*_tmp79D=Cyc_PP_line_doc();_tmp3C2[5]=_tmp79D;});Cyc_PP_cat(_tag_fat(_tmp3C2,sizeof(struct Cyc_PP_Doc*),6U));});}}
# 1484
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_tmp3C3=st->r;void*_stmttmp19=_tmp3C3;void*_tmp3C4=_stmttmp19;struct Cyc_List_List*_tmp3C6;struct Cyc_Absyn_Stmt*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3CA;struct _fat_ptr*_tmp3C9;struct Cyc_Absyn_Stmt*_tmp3CC;struct Cyc_Absyn_Decl*_tmp3CB;struct Cyc_List_List*_tmp3CD;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D0;struct _fat_ptr*_tmp3D4;struct Cyc_Absyn_Stmt*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Stmt*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3D8;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Stmt*_tmp3DC;struct Cyc_Absyn_Stmt*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DD;switch(*((int*)_tmp3C4)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3DE=";";_tag_fat(_tmp3DE,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp3DD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp3DD;
s=({struct Cyc_PP_Doc*_tmp3DF[2U];({struct Cyc_PP_Doc*_tmp7A4=Cyc_Absynpp_exp2doc(e);_tmp3DF[0]=_tmp7A4;}),({struct Cyc_PP_Doc*_tmp7A3=Cyc_PP_text(({const char*_tmp3E0=";";_tag_fat(_tmp3E0,sizeof(char),2U);}));_tmp3DF[1]=_tmp7A3;});Cyc_PP_cat(_tag_fat(_tmp3DF,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 2U: _LL5: _tmp3DB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_tmp3DC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp3DB;struct Cyc_Absyn_Stmt*s2=_tmp3DC;
# 1490
if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp3E1[2U];({struct Cyc_PP_Doc*_tmp7A8=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3E1[0]=_tmp7A8;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_tmp7A7=({int _tmp7A6=stmtexp;Cyc_Absynpp_block(_tmp7A6,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp3E1[1]=_tmp7A7;}):({
struct Cyc_PP_Doc*_tmp7A5=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp3E1[1]=_tmp7A5;});Cyc_PP_cat(_tag_fat(_tmp3E1,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(Cyc_Absynpp_is_declaration(s2))
s=({struct Cyc_PP_Doc*_tmp3E2[3U];({struct Cyc_PP_Doc*_tmp7AC=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp3E2[0]=_tmp7AC;}),({
struct Cyc_PP_Doc*_tmp7AB=Cyc_PP_line_doc();_tmp3E2[1]=_tmp7AB;}),({
struct Cyc_PP_Doc*_tmp7AA=({int _tmp7A9=stmtexp;Cyc_Absynpp_block(_tmp7A9,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp3E2[2]=_tmp7AA;});Cyc_PP_cat(_tag_fat(_tmp3E2,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1501
s=({struct Cyc_PP_Doc*_tmp3E3[3U];({struct Cyc_PP_Doc*_tmp7AF=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp3E3[0]=_tmp7AF;}),({struct Cyc_PP_Doc*_tmp7AE=Cyc_PP_line_doc();_tmp3E3[1]=_tmp7AE;}),({
struct Cyc_PP_Doc*_tmp7AD=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp3E3[2]=_tmp7AD;});Cyc_PP_cat(_tag_fat(_tmp3E3,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1505
s=({struct Cyc_PP_Doc*_tmp3E4[3U];({struct Cyc_PP_Doc*_tmp7B2=Cyc_Absynpp_stmt2doc(s1,0,0);_tmp3E4[0]=_tmp7B2;}),({struct Cyc_PP_Doc*_tmp7B1=Cyc_PP_line_doc();_tmp3E4[1]=_tmp7B1;}),({struct Cyc_PP_Doc*_tmp7B0=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_tmp3E4[2]=_tmp7B0;});Cyc_PP_cat(_tag_fat(_tmp3E4,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 3U: _LL7: _tmp3DA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp3DA;
# 1508
if(eopt == 0)
s=Cyc_PP_text(({const char*_tmp3E5="return;";_tag_fat(_tmp3E5,sizeof(char),8U);}));else{
# 1511
s=({struct Cyc_PP_Doc*_tmp3E6[3U];({struct Cyc_PP_Doc*_tmp7B6=Cyc_PP_text(({const char*_tmp3E7="return ";_tag_fat(_tmp3E7,sizeof(char),8U);}));_tmp3E6[0]=_tmp7B6;}),
eopt == 0?({struct Cyc_PP_Doc*_tmp7B5=Cyc_PP_nil_doc();_tmp3E6[1]=_tmp7B5;}):({struct Cyc_PP_Doc*_tmp7B4=Cyc_Absynpp_exp2doc(eopt);_tmp3E6[1]=_tmp7B4;}),({
struct Cyc_PP_Doc*_tmp7B3=Cyc_PP_text(({const char*_tmp3E8=";";_tag_fat(_tmp3E8,sizeof(char),2U);}));_tmp3E6[2]=_tmp7B3;});Cyc_PP_cat(_tag_fat(_tmp3E6,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 4U: _LL9: _tmp3D7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_tmp3D8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2;_tmp3D9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp3D7;struct Cyc_Absyn_Stmt*s1=_tmp3D8;struct Cyc_Absyn_Stmt*s2=_tmp3D9;
# 1516
int print_else;
{void*_tmp3E9=s2->r;void*_stmttmp1A=_tmp3E9;void*_tmp3EA=_stmttmp1A;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3EA)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1521
s=({struct Cyc_PP_Doc*_tmp3EB[5U];({struct Cyc_PP_Doc*_tmp7BF=Cyc_PP_text(({const char*_tmp3EC="if (";_tag_fat(_tmp3EC,sizeof(char),5U);}));_tmp3EB[0]=_tmp7BF;}),({
struct Cyc_PP_Doc*_tmp7BE=Cyc_Absynpp_exp2doc(e);_tmp3EB[1]=_tmp7BE;}),({
struct Cyc_PP_Doc*_tmp7BD=Cyc_PP_text(({const char*_tmp3ED=") ";_tag_fat(_tmp3ED,sizeof(char),3U);}));_tmp3EB[2]=_tmp7BD;}),({
struct Cyc_PP_Doc*_tmp7BC=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3EB[3]=_tmp7BC;}),
print_else?({
struct Cyc_PP_Doc*_tmp7BB=({struct Cyc_PP_Doc*_tmp3EE[3U];({struct Cyc_PP_Doc*_tmp7BA=Cyc_PP_line_doc();_tmp3EE[0]=_tmp7BA;}),({
struct Cyc_PP_Doc*_tmp7B9=Cyc_PP_text(({const char*_tmp3EF="else ";_tag_fat(_tmp3EF,sizeof(char),6U);}));_tmp3EE[1]=_tmp7B9;}),({
struct Cyc_PP_Doc*_tmp7B8=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_tmp3EE[2]=_tmp7B8;});Cyc_PP_cat(_tag_fat(_tmp3EE,sizeof(struct Cyc_PP_Doc*),3U));});
# 1526
_tmp3EB[4]=_tmp7BB;}):({
# 1529
struct Cyc_PP_Doc*_tmp7B7=Cyc_PP_nil_doc();_tmp3EB[4]=_tmp7B7;});Cyc_PP_cat(_tag_fat(_tmp3EB,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp3D5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1).f1;_tmp3D6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp3D5;struct Cyc_Absyn_Stmt*s1=_tmp3D6;
# 1532
s=({struct Cyc_PP_Doc*_tmp3F0[4U];({struct Cyc_PP_Doc*_tmp7C3=Cyc_PP_text(({const char*_tmp3F1="while (";_tag_fat(_tmp3F1,sizeof(char),8U);}));_tmp3F0[0]=_tmp7C3;}),({
struct Cyc_PP_Doc*_tmp7C2=Cyc_Absynpp_exp2doc(e);_tmp3F0[1]=_tmp7C2;}),({
struct Cyc_PP_Doc*_tmp7C1=Cyc_PP_text(({const char*_tmp3F2=") ";_tag_fat(_tmp3F2,sizeof(char),3U);}));_tmp3F0[2]=_tmp7C1;}),({
struct Cyc_PP_Doc*_tmp7C0=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3F0[3]=_tmp7C0;});Cyc_PP_cat(_tag_fat(_tmp3F0,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp3F3="break;";_tag_fat(_tmp3F3,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp3F4="continue;";_tag_fat(_tmp3F4,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp3D4=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_LL12: {struct _fat_ptr*x=_tmp3D4;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3F7=({struct Cyc_String_pa_PrintArg_struct _tmp5DB;_tmp5DB.tag=0U,_tmp5DB.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp5DB;});void*_tmp3F5[1U];_tmp3F5[0]=& _tmp3F7;({struct _fat_ptr _tmp7C4=({const char*_tmp3F6="goto %s;";_tag_fat(_tmp3F6,sizeof(char),9U);});Cyc_aprintf(_tmp7C4,_tag_fat(_tmp3F5,sizeof(void*),1U));});}));goto _LL0;}case 9U: _LL13: _tmp3D0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_tmp3D1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2).f1;_tmp3D2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f3).f1;_tmp3D3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f4;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp3D0;struct Cyc_Absyn_Exp*e2=_tmp3D1;struct Cyc_Absyn_Exp*e3=_tmp3D2;struct Cyc_Absyn_Stmt*s1=_tmp3D3;
# 1541
s=({struct Cyc_PP_Doc*_tmp3F8[8U];({struct Cyc_PP_Doc*_tmp7CC=Cyc_PP_text(({const char*_tmp3F9="for(";_tag_fat(_tmp3F9,sizeof(char),5U);}));_tmp3F8[0]=_tmp7CC;}),({
struct Cyc_PP_Doc*_tmp7CB=Cyc_Absynpp_exp2doc(e1);_tmp3F8[1]=_tmp7CB;}),({
struct Cyc_PP_Doc*_tmp7CA=Cyc_PP_text(({const char*_tmp3FA="; ";_tag_fat(_tmp3FA,sizeof(char),3U);}));_tmp3F8[2]=_tmp7CA;}),({
struct Cyc_PP_Doc*_tmp7C9=Cyc_Absynpp_exp2doc(e2);_tmp3F8[3]=_tmp7C9;}),({
struct Cyc_PP_Doc*_tmp7C8=Cyc_PP_text(({const char*_tmp3FB="; ";_tag_fat(_tmp3FB,sizeof(char),3U);}));_tmp3F8[4]=_tmp7C8;}),({
struct Cyc_PP_Doc*_tmp7C7=Cyc_Absynpp_exp2doc(e3);_tmp3F8[5]=_tmp7C7;}),({
struct Cyc_PP_Doc*_tmp7C6=Cyc_PP_text(({const char*_tmp3FC=") ";_tag_fat(_tmp3FC,sizeof(char),3U);}));_tmp3F8[6]=_tmp7C6;}),({
struct Cyc_PP_Doc*_tmp7C5=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3F8[7]=_tmp7C5;});Cyc_PP_cat(_tag_fat(_tmp3F8,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 10U: _LL15: _tmp3CE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_tmp3CF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp3CE;struct Cyc_List_List*ss=_tmp3CF;
# 1551
s=({struct Cyc_PP_Doc*_tmp3FD[8U];({struct Cyc_PP_Doc*_tmp7D4=Cyc_PP_text(({const char*_tmp3FE="switch (";_tag_fat(_tmp3FE,sizeof(char),9U);}));_tmp3FD[0]=_tmp7D4;}),({
struct Cyc_PP_Doc*_tmp7D3=Cyc_Absynpp_exp2doc(e);_tmp3FD[1]=_tmp7D3;}),({
struct Cyc_PP_Doc*_tmp7D2=Cyc_PP_text(({const char*_tmp3FF=") ";_tag_fat(_tmp3FF,sizeof(char),3U);}));_tmp3FD[2]=_tmp7D2;}),({
struct Cyc_PP_Doc*_tmp7D1=Cyc_Absynpp_lb();_tmp3FD[3]=_tmp7D1;}),({
struct Cyc_PP_Doc*_tmp7D0=Cyc_PP_line_doc();_tmp3FD[4]=_tmp7D0;}),({
struct Cyc_PP_Doc*_tmp7CF=Cyc_Absynpp_switchclauses2doc(ss);_tmp3FD[5]=_tmp7CF;}),({
struct Cyc_PP_Doc*_tmp7CE=Cyc_PP_line_doc();_tmp3FD[6]=_tmp7CE;}),({
struct Cyc_PP_Doc*_tmp7CD=Cyc_Absynpp_rb();_tmp3FD[7]=_tmp7CD;});Cyc_PP_cat(_tag_fat(_tmp3FD,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp400="fallthru;";_tag_fat(_tmp400,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp3CD=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_LL1A: {struct Cyc_List_List*es=_tmp3CD;
# 1562
s=({struct Cyc_PP_Doc*_tmp401[3U];({struct Cyc_PP_Doc*_tmp7D7=Cyc_PP_text(({const char*_tmp402="fallthru(";_tag_fat(_tmp402,sizeof(char),10U);}));_tmp401[0]=_tmp7D7;}),({struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_exps2doc_prec(20,es);_tmp401[1]=_tmp7D6;}),({struct Cyc_PP_Doc*_tmp7D5=Cyc_PP_text(({const char*_tmp403=");";_tag_fat(_tmp403,sizeof(char),3U);}));_tmp401[2]=_tmp7D5;});Cyc_PP_cat(_tag_fat(_tmp401,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 12U: _LL1B: _tmp3CB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_tmp3CC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp3CB;struct Cyc_Absyn_Stmt*s1=_tmp3CC;
# 1564
struct _tuple14 _tmp404=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple14 _stmttmp1B=_tmp404;struct _tuple14 _tmp405=_stmttmp1B;struct Cyc_List_List*_tmp407;int _tmp406;_LL29: _tmp406=_tmp405.f1;_tmp407=_tmp405.f2;_LL2A: {int newblock=_tmp406;struct Cyc_List_List*newvarsinblock=_tmp407;
s=({struct Cyc_PP_Doc*_tmp408[3U];({struct Cyc_PP_Doc*_tmp7DA=Cyc_Absynpp_decl2doc(d);_tmp408[0]=_tmp7DA;}),({struct Cyc_PP_Doc*_tmp7D9=Cyc_PP_line_doc();_tmp408[1]=_tmp7D9;}),({struct Cyc_PP_Doc*_tmp7D8=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_tmp408[2]=_tmp7D8;});Cyc_PP_cat(_tag_fat(_tmp408,sizeof(struct Cyc_PP_Doc*),3U));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}}case 13U: _LL1D: _tmp3C9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_tmp3CA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2;_LL1E: {struct _fat_ptr*x=_tmp3C9;struct Cyc_Absyn_Stmt*s1=_tmp3CA;
# 1569
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp409[3U];({struct Cyc_PP_Doc*_tmp7DE=Cyc_PP_textptr(x);_tmp409[0]=_tmp7DE;}),({struct Cyc_PP_Doc*_tmp7DD=Cyc_PP_text(({const char*_tmp40A=": ";_tag_fat(_tmp40A,sizeof(char),3U);}));_tmp409[1]=_tmp7DD;}),({struct Cyc_PP_Doc*_tmp7DC=({int _tmp7DB=stmtexp;Cyc_Absynpp_block(_tmp7DB,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_tmp409[2]=_tmp7DC;});Cyc_PP_cat(_tag_fat(_tmp409,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1572
s=({struct Cyc_PP_Doc*_tmp40B[3U];({struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_textptr(x);_tmp40B[0]=_tmp7E1;}),({struct Cyc_PP_Doc*_tmp7E0=Cyc_PP_text(({const char*_tmp40C=": ";_tag_fat(_tmp40C,sizeof(char),3U);}));_tmp40B[1]=_tmp7E0;}),({struct Cyc_PP_Doc*_tmp7DF=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_tmp40B[2]=_tmp7DF;});Cyc_PP_cat(_tag_fat(_tmp40B,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 14U: _LL1F: _tmp3C7=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_tmp3C8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2).f1;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp3C7;struct Cyc_Absyn_Exp*e=_tmp3C8;
# 1575
s=({struct Cyc_PP_Doc*_tmp40D[5U];({struct Cyc_PP_Doc*_tmp7E6=Cyc_PP_text(({const char*_tmp40E="do ";_tag_fat(_tmp40E,sizeof(char),4U);}));_tmp40D[0]=_tmp7E6;}),({
struct Cyc_PP_Doc*_tmp7E5=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp40D[1]=_tmp7E5;}),({
struct Cyc_PP_Doc*_tmp7E4=Cyc_PP_text(({const char*_tmp40F=" while (";_tag_fat(_tmp40F,sizeof(char),9U);}));_tmp40D[2]=_tmp7E4;}),({
struct Cyc_PP_Doc*_tmp7E3=Cyc_Absynpp_exp2doc(e);_tmp40D[3]=_tmp7E3;}),({
struct Cyc_PP_Doc*_tmp7E2=Cyc_PP_text(({const char*_tmp410=");";_tag_fat(_tmp410,sizeof(char),3U);}));_tmp40D[4]=_tmp7E2;});Cyc_PP_cat(_tag_fat(_tmp40D,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}default: _LL21: _tmp3C5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f1;_tmp3C6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3C4)->f2;_LL22: {struct Cyc_Absyn_Stmt*s1=_tmp3C5;struct Cyc_List_List*ss=_tmp3C6;
# 1582
s=({struct Cyc_PP_Doc*_tmp411[4U];({struct Cyc_PP_Doc*_tmp7EA=Cyc_PP_text(({const char*_tmp412="try ";_tag_fat(_tmp412,sizeof(char),5U);}));_tmp411[0]=_tmp7EA;}),({
struct Cyc_PP_Doc*_tmp7E9=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp411[1]=_tmp7E9;}),({
struct Cyc_PP_Doc*_tmp7E8=Cyc_PP_text(({const char*_tmp413=" catch ";_tag_fat(_tmp413,sizeof(char),8U);}));_tmp411[2]=_tmp7E8;}),({
struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_tmp411[3]=_tmp7E7;});Cyc_PP_cat(_tag_fat(_tmp411,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}_LL0:;}
# 1588
return s;}
# 1591
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp414=p->r;void*_stmttmp1C=_tmp414;void*_tmp415=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp416;int _tmp419;struct Cyc_List_List*_tmp418;struct Cyc_Absyn_Datatypefield*_tmp417;struct Cyc_Absyn_Datatypefield*_tmp41A;struct Cyc_Absyn_Enumfield*_tmp41B;struct Cyc_Absyn_Enumfield*_tmp41C;int _tmp41F;struct Cyc_List_List*_tmp41E;struct Cyc_List_List*_tmp41D;int _tmp423;struct Cyc_List_List*_tmp422;struct Cyc_List_List*_tmp421;union Cyc_Absyn_AggrInfo _tmp420;int _tmp426;struct Cyc_List_List*_tmp425;struct _tuple0*_tmp424;struct _tuple0*_tmp427;struct Cyc_Absyn_Pat*_tmp429;struct Cyc_Absyn_Vardecl*_tmp428;struct Cyc_Absyn_Vardecl*_tmp42A;struct Cyc_Absyn_Pat*_tmp42B;int _tmp42D;struct Cyc_List_List*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42F;struct Cyc_Absyn_Tvar*_tmp42E;struct Cyc_Absyn_Vardecl*_tmp431;struct Cyc_Absyn_Tvar*_tmp430;struct Cyc_Absyn_Pat*_tmp433;struct Cyc_Absyn_Vardecl*_tmp432;struct Cyc_Absyn_Vardecl*_tmp434;struct _fat_ptr _tmp435;char _tmp436;int _tmp438;enum Cyc_Absyn_Sign _tmp437;switch(*((int*)_tmp415)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp439="_";_tag_fat(_tmp439,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp43A="NULL";_tag_fat(_tmp43A,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp437=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_tmp438=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp437;int i=_tmp438;
# 1597
if((int)sg != (int)1U)
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp43D=({struct Cyc_Int_pa_PrintArg_struct _tmp5DC;_tmp5DC.tag=1U,_tmp5DC.f1=(unsigned long)i;_tmp5DC;});void*_tmp43B[1U];_tmp43B[0]=& _tmp43D;({struct _fat_ptr _tmp7EB=({const char*_tmp43C="%d";_tag_fat(_tmp43C,sizeof(char),3U);});Cyc_aprintf(_tmp7EB,_tag_fat(_tmp43B,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp440=({struct Cyc_Int_pa_PrintArg_struct _tmp5DD;_tmp5DD.tag=1U,_tmp5DD.f1=(unsigned)i;_tmp5DD;});void*_tmp43E[1U];_tmp43E[0]=& _tmp440;({struct _fat_ptr _tmp7EC=({const char*_tmp43F="%u";_tag_fat(_tmp43F,sizeof(char),3U);});Cyc_aprintf(_tmp7EC,_tag_fat(_tmp43E,sizeof(void*),1U));});}));}
goto _LL0;}case 11U: _LL7: _tmp436=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_LL8: {char ch=_tmp436;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp443=({struct Cyc_String_pa_PrintArg_struct _tmp5DE;_tmp5DE.tag=0U,({struct _fat_ptr _tmp7ED=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp5DE.f1=_tmp7ED;});_tmp5DE;});void*_tmp441[1U];_tmp441[0]=& _tmp443;({struct _fat_ptr _tmp7EE=({const char*_tmp442="'%s'";_tag_fat(_tmp442,sizeof(char),5U);});Cyc_aprintf(_tmp7EE,_tag_fat(_tmp441,sizeof(void*),1U));});}));goto _LL0;}case 12U: _LL9: _tmp435=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_LLA: {struct _fat_ptr x=_tmp435;
s=Cyc_PP_text(x);goto _LL0;}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp415)->f2)->r)->tag == 0U){_LLB: _tmp434=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp434;
# 1604
s=Cyc_Absynpp_qvar2doc(vd->name);goto _LL0;}}else{_LLD: _tmp432=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_tmp433=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp432;struct Cyc_Absyn_Pat*p2=_tmp433;
# 1606
s=({struct Cyc_PP_Doc*_tmp444[3U];({struct Cyc_PP_Doc*_tmp7F1=Cyc_Absynpp_qvar2doc(vd->name);_tmp444[0]=_tmp7F1;}),({struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_text(({const char*_tmp445=" as ";_tag_fat(_tmp445,sizeof(char),5U);}));_tmp444[1]=_tmp7F0;}),({struct Cyc_PP_Doc*_tmp7EF=Cyc_Absynpp_pat2doc(p2);_tmp444[2]=_tmp7EF;});Cyc_PP_cat(_tag_fat(_tmp444,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 2U: _LLF: _tmp430=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_tmp431=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp430;struct Cyc_Absyn_Vardecl*vd=_tmp431;
# 1608
s=({struct Cyc_PP_Doc*_tmp446[5U];({struct Cyc_PP_Doc*_tmp7F6=Cyc_PP_text(({const char*_tmp447="alias";_tag_fat(_tmp447,sizeof(char),6U);}));_tmp446[0]=_tmp7F6;}),({struct Cyc_PP_Doc*_tmp7F5=Cyc_PP_text(({const char*_tmp448=" <";_tag_fat(_tmp448,sizeof(char),3U);}));_tmp446[1]=_tmp7F5;}),({struct Cyc_PP_Doc*_tmp7F4=Cyc_Absynpp_tvar2doc(tv);_tmp446[2]=_tmp7F4;}),({struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_text(({const char*_tmp449="> ";_tag_fat(_tmp449,sizeof(char),3U);}));_tmp446[3]=_tmp7F3;}),({
struct Cyc_PP_Doc*_tmp7F2=Cyc_Absynpp_vardecl2doc(vd);_tmp446[4]=_tmp7F2;});Cyc_PP_cat(_tag_fat(_tmp446,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 4U: _LL11: _tmp42E=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_tmp42F=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp42E;struct Cyc_Absyn_Vardecl*vd=_tmp42F;
# 1612
s=({struct Cyc_PP_Doc*_tmp44A[4U];({struct Cyc_PP_Doc*_tmp7FA=Cyc_Absynpp_qvar2doc(vd->name);_tmp44A[0]=_tmp7FA;}),({struct Cyc_PP_Doc*_tmp7F9=Cyc_PP_text(({const char*_tmp44B="<";_tag_fat(_tmp44B,sizeof(char),2U);}));_tmp44A[1]=_tmp7F9;}),({struct Cyc_PP_Doc*_tmp7F8=Cyc_Absynpp_tvar2doc(tv);_tmp44A[2]=_tmp7F8;}),({struct Cyc_PP_Doc*_tmp7F7=Cyc_PP_text(({const char*_tmp44C=">";_tag_fat(_tmp44C,sizeof(char),2U);}));_tmp44A[3]=_tmp7F7;});Cyc_PP_cat(_tag_fat(_tmp44A,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 5U: _LL13: _tmp42C=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_tmp42D=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LL14: {struct Cyc_List_List*ts=_tmp42C;int dots=_tmp42D;
# 1615
s=({struct Cyc_PP_Doc*_tmp44D[4U];({struct Cyc_PP_Doc*_tmp800=Cyc_Absynpp_dollar();_tmp44D[0]=_tmp800;}),({struct Cyc_PP_Doc*_tmp7FF=Cyc_PP_text(({const char*_tmp44E="(";_tag_fat(_tmp44E,sizeof(char),2U);}));_tmp44D[1]=_tmp7FF;}),({struct Cyc_PP_Doc*_tmp7FE=({struct _fat_ptr _tmp7FD=({const char*_tmp44F=",";_tag_fat(_tmp44F,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp7FD,ts);});_tmp44D[2]=_tmp7FE;}),
dots?({struct Cyc_PP_Doc*_tmp7FC=Cyc_PP_text(({const char*_tmp450=", ...)";_tag_fat(_tmp450,sizeof(char),7U);}));_tmp44D[3]=_tmp7FC;}):({struct Cyc_PP_Doc*_tmp7FB=Cyc_PP_text(({const char*_tmp451=")";_tag_fat(_tmp451,sizeof(char),2U);}));_tmp44D[3]=_tmp7FB;});Cyc_PP_cat(_tag_fat(_tmp44D,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LL15: _tmp42B=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_LL16: {struct Cyc_Absyn_Pat*p2=_tmp42B;
# 1619
s=({struct Cyc_PP_Doc*_tmp452[2U];({struct Cyc_PP_Doc*_tmp802=Cyc_PP_text(({const char*_tmp453="&";_tag_fat(_tmp453,sizeof(char),2U);}));_tmp452[0]=_tmp802;}),({struct Cyc_PP_Doc*_tmp801=Cyc_Absynpp_pat2doc(p2);_tmp452[1]=_tmp801;});Cyc_PP_cat(_tag_fat(_tmp452,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp415)->f2)->r)->tag == 0U){_LL17: _tmp42A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp42A;
# 1622
s=({struct Cyc_PP_Doc*_tmp454[2U];({struct Cyc_PP_Doc*_tmp804=Cyc_PP_text(({const char*_tmp455="*";_tag_fat(_tmp455,sizeof(char),2U);}));_tmp454[0]=_tmp804;}),({struct Cyc_PP_Doc*_tmp803=Cyc_Absynpp_qvar2doc(vd->name);_tmp454[1]=_tmp803;});Cyc_PP_cat(_tag_fat(_tmp454,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}else{_LL19: _tmp428=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_tmp429=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp428;struct Cyc_Absyn_Pat*p2=_tmp429;
# 1625
s=({struct Cyc_PP_Doc*_tmp456[4U];({struct Cyc_PP_Doc*_tmp808=Cyc_PP_text(({const char*_tmp457="*";_tag_fat(_tmp457,sizeof(char),2U);}));_tmp456[0]=_tmp808;}),({struct Cyc_PP_Doc*_tmp807=Cyc_Absynpp_qvar2doc(vd->name);_tmp456[1]=_tmp807;}),({struct Cyc_PP_Doc*_tmp806=Cyc_PP_text(({const char*_tmp458=" as ";_tag_fat(_tmp458,sizeof(char),5U);}));_tmp456[2]=_tmp806;}),({struct Cyc_PP_Doc*_tmp805=Cyc_Absynpp_pat2doc(p2);_tmp456[3]=_tmp805;});Cyc_PP_cat(_tag_fat(_tmp456,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}case 15U: _LL1B: _tmp427=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_LL1C: {struct _tuple0*q=_tmp427;
# 1628
s=Cyc_Absynpp_qvar2doc(q);
goto _LL0;}case 16U: _LL1D: _tmp424=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_tmp425=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_tmp426=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp415)->f3;_LL1E: {struct _tuple0*q=_tmp424;struct Cyc_List_List*ps=_tmp425;int dots=_tmp426;
# 1631
struct _fat_ptr term=dots?({const char*_tmp45C=", ...)";_tag_fat(_tmp45C,sizeof(char),7U);}):({const char*_tmp45D=")";_tag_fat(_tmp45D,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp459[2U];({struct Cyc_PP_Doc*_tmp80D=Cyc_Absynpp_qvar2doc(q);_tmp459[0]=_tmp80D;}),({struct Cyc_PP_Doc*_tmp80C=({struct _fat_ptr _tmp80B=({const char*_tmp45A="(";_tag_fat(_tmp45A,sizeof(char),2U);});struct _fat_ptr _tmp80A=term;struct _fat_ptr _tmp809=({const char*_tmp45B=",";_tag_fat(_tmp45B,sizeof(char),2U);});Cyc_PP_group(_tmp80B,_tmp80A,_tmp809,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp459[1]=_tmp80C;});Cyc_PP_cat(_tag_fat(_tmp459,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp415)->f1 != 0){_LL1F: _tmp420=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_tmp421=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_tmp422=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp415)->f3;_tmp423=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp415)->f4;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp420;struct Cyc_List_List*exists=_tmp421;struct Cyc_List_List*dps=_tmp422;int dots=_tmp423;
# 1635
struct _fat_ptr term=dots?({const char*_tmp467=", ...}";_tag_fat(_tmp467,sizeof(char),7U);}):({const char*_tmp468="}";_tag_fat(_tmp468,sizeof(char),2U);});
struct _tuple11 _tmp45E=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp1D=_tmp45E;struct _tuple11 _tmp45F=_stmttmp1D;struct _tuple0*_tmp460;_LL2E: _tmp460=_tmp45F.f2;_LL2F: {struct _tuple0*n=_tmp460;
s=({struct Cyc_PP_Doc*_tmp461[4U];({struct Cyc_PP_Doc*_tmp817=Cyc_Absynpp_qvar2doc(n);_tmp461[0]=_tmp817;}),({struct Cyc_PP_Doc*_tmp816=Cyc_Absynpp_lb();_tmp461[1]=_tmp816;}),({
struct Cyc_PP_Doc*_tmp815=({struct _fat_ptr _tmp814=({const char*_tmp462="[";_tag_fat(_tmp462,sizeof(char),2U);});struct _fat_ptr _tmp813=({const char*_tmp463="]";_tag_fat(_tmp463,sizeof(char),2U);});struct _fat_ptr _tmp812=({const char*_tmp464=",";_tag_fat(_tmp464,sizeof(char),2U);});Cyc_PP_egroup(_tmp814,_tmp813,_tmp812,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp461[2]=_tmp815;}),({
struct Cyc_PP_Doc*_tmp811=({struct _fat_ptr _tmp810=({const char*_tmp465="";_tag_fat(_tmp465,sizeof(char),1U);});struct _fat_ptr _tmp80F=term;struct _fat_ptr _tmp80E=({const char*_tmp466=",";_tag_fat(_tmp466,sizeof(char),2U);});Cyc_PP_group(_tmp810,_tmp80F,_tmp80E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp461[3]=_tmp811;});Cyc_PP_cat(_tag_fat(_tmp461,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}}else{_LL21: _tmp41D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_tmp41E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp415)->f3;_tmp41F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp415)->f4;_LL22: {struct Cyc_List_List*exists=_tmp41D;struct Cyc_List_List*dps=_tmp41E;int dots=_tmp41F;
# 1642
struct _fat_ptr term=dots?({const char*_tmp46F=", ...}";_tag_fat(_tmp46F,sizeof(char),7U);}):({const char*_tmp470="}";_tag_fat(_tmp470,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp469[3U];({struct Cyc_PP_Doc*_tmp820=Cyc_Absynpp_lb();_tmp469[0]=_tmp820;}),({
struct Cyc_PP_Doc*_tmp81F=({struct _fat_ptr _tmp81E=({const char*_tmp46A="[";_tag_fat(_tmp46A,sizeof(char),2U);});struct _fat_ptr _tmp81D=({const char*_tmp46B="]";_tag_fat(_tmp46B,sizeof(char),2U);});struct _fat_ptr _tmp81C=({const char*_tmp46C=",";_tag_fat(_tmp46C,sizeof(char),2U);});Cyc_PP_egroup(_tmp81E,_tmp81D,_tmp81C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp469[1]=_tmp81F;}),({
struct Cyc_PP_Doc*_tmp81B=({struct _fat_ptr _tmp81A=({const char*_tmp46D="";_tag_fat(_tmp46D,sizeof(char),1U);});struct _fat_ptr _tmp819=term;struct _fat_ptr _tmp818=({const char*_tmp46E=",";_tag_fat(_tmp46E,sizeof(char),2U);});Cyc_PP_group(_tmp81A,_tmp819,_tmp818,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp469[2]=_tmp81B;});Cyc_PP_cat(_tag_fat(_tmp469,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp41C=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp41C;
_tmp41B=ef;goto _LL26;}case 14U: _LL25: _tmp41B=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp41B;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp415)->f3 == 0){_LL27: _tmp41A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_LL28: {struct Cyc_Absyn_Datatypefield*tuf=_tmp41A;
s=Cyc_Absynpp_qvar2doc(tuf->name);goto _LL0;}}else{_LL29: _tmp417=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp415)->f2;_tmp418=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp415)->f3;_tmp419=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp415)->f4;_LL2A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp417;struct Cyc_List_List*ps=_tmp418;int dots=_tmp419;
# 1651
struct _fat_ptr term=dots?({const char*_tmp474=", ...)";_tag_fat(_tmp474,sizeof(char),7U);}):({const char*_tmp475=")";_tag_fat(_tmp475,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp471[2U];({struct Cyc_PP_Doc*_tmp825=Cyc_Absynpp_qvar2doc(tuf->name);_tmp471[0]=_tmp825;}),({struct Cyc_PP_Doc*_tmp824=({struct _fat_ptr _tmp823=({const char*_tmp472="(";_tag_fat(_tmp472,sizeof(char),2U);});struct _fat_ptr _tmp822=term;struct _fat_ptr _tmp821=({const char*_tmp473=",";_tag_fat(_tmp473,sizeof(char),2U);});Cyc_PP_egroup(_tmp823,_tmp822,_tmp821,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp471[1]=_tmp824;});Cyc_PP_cat(_tag_fat(_tmp471,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp416=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp415)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp416;
# 1655
s=Cyc_Absynpp_exp2doc(e);goto _LL0;}}_LL0:;}
# 1657
return s;}
# 1660
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp){
return({struct Cyc_PP_Doc*_tmp476[2U];({struct Cyc_PP_Doc*_tmp82A=({struct _fat_ptr _tmp829=({const char*_tmp477="";_tag_fat(_tmp477,sizeof(char),1U);});struct _fat_ptr _tmp828=({const char*_tmp478="=";_tag_fat(_tmp478,sizeof(char),2U);});struct _fat_ptr _tmp827=({const char*_tmp479="=";_tag_fat(_tmp479,sizeof(char),2U);});Cyc_PP_egroup(_tmp829,_tmp828,_tmp827,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp476[0]=_tmp82A;}),({
struct Cyc_PP_Doc*_tmp826=Cyc_Absynpp_pat2doc((*dp).f2);_tmp476[1]=_tmp826;});Cyc_PP_cat(_tag_fat(_tmp476,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1665
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1667
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp47A[2U];({struct Cyc_PP_Doc*_tmp82D=Cyc_PP_text(({const char*_tmp47B="default: ";_tag_fat(_tmp47B,sizeof(char),10U);}));_tmp47A[0]=_tmp82D;}),({
struct Cyc_PP_Doc*_tmp82C=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp47C[2U];({struct Cyc_PP_Doc*_tmp82B=Cyc_PP_line_doc();_tmp47C[0]=_tmp82B;}),_tmp47C[1]=body;Cyc_PP_cat(_tag_fat(_tmp47C,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp47A[1]=_tmp82C;});Cyc_PP_cat(_tag_fat(_tmp47A,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp47D[4U];({struct Cyc_PP_Doc*_tmp832=Cyc_PP_text(({const char*_tmp47E="case ";_tag_fat(_tmp47E,sizeof(char),6U);}));_tmp47D[0]=_tmp832;}),({
struct Cyc_PP_Doc*_tmp831=Cyc_Absynpp_pat2doc(c->pattern);_tmp47D[1]=_tmp831;}),({
struct Cyc_PP_Doc*_tmp830=Cyc_PP_text(({const char*_tmp47F=": ";_tag_fat(_tmp47F,sizeof(char),3U);}));_tmp47D[2]=_tmp830;}),({
struct Cyc_PP_Doc*_tmp82F=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp480[2U];({struct Cyc_PP_Doc*_tmp82E=Cyc_PP_line_doc();_tmp480[0]=_tmp82E;}),_tmp480[1]=body;Cyc_PP_cat(_tag_fat(_tmp480,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp47D[3]=_tmp82F;});Cyc_PP_cat(_tag_fat(_tmp47D,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1679
return({struct Cyc_PP_Doc*_tmp481[6U];({struct Cyc_PP_Doc*_tmp839=Cyc_PP_text(({const char*_tmp482="case ";_tag_fat(_tmp482,sizeof(char),6U);}));_tmp481[0]=_tmp839;}),({
struct Cyc_PP_Doc*_tmp838=Cyc_Absynpp_pat2doc(c->pattern);_tmp481[1]=_tmp838;}),({
struct Cyc_PP_Doc*_tmp837=Cyc_PP_text(({const char*_tmp483=" && ";_tag_fat(_tmp483,sizeof(char),5U);}));_tmp481[2]=_tmp837;}),({
struct Cyc_PP_Doc*_tmp836=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp481[3]=_tmp836;}),({
struct Cyc_PP_Doc*_tmp835=Cyc_PP_text(({const char*_tmp484=": ";_tag_fat(_tmp484,sizeof(char),3U);}));_tmp481[4]=_tmp835;}),({
struct Cyc_PP_Doc*_tmp834=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp485[2U];({struct Cyc_PP_Doc*_tmp833=Cyc_PP_line_doc();_tmp485[0]=_tmp833;}),_tmp485[1]=body;Cyc_PP_cat(_tag_fat(_tmp485,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp481[5]=_tmp834;});Cyc_PP_cat(_tag_fat(_tmp481,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1687
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _fat_ptr _tmp83A=({const char*_tmp486="";_tag_fat(_tmp486,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp83A,cs);});}
# 1691
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1695
return({struct Cyc_PP_Doc*_tmp487[3U];({struct Cyc_PP_Doc*_tmp83D=Cyc_Absynpp_qvar2doc(f->name);_tmp487[0]=_tmp83D;}),({struct Cyc_PP_Doc*_tmp83C=Cyc_PP_text(({const char*_tmp488=" = ";_tag_fat(_tmp488,sizeof(char),4U);}));_tmp487[1]=_tmp83C;}),({struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp487[2]=_tmp83B;});Cyc_PP_cat(_tag_fat(_tmp487,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1698
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _fat_ptr _tmp83E=({const char*_tmp489=",";_tag_fat(_tmp489,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp83E,fs);});}
# 1702
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1706
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _fat_ptr _tmp83F=({const char*_tmp48A=",";_tag_fat(_tmp48A,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp83F,vds);});}
# 1710
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp48B=vd;struct Cyc_List_List*_tmp492;struct Cyc_Absyn_Exp*_tmp491;void*_tmp490;struct Cyc_Absyn_Tqual _tmp48F;unsigned _tmp48E;struct _tuple0*_tmp48D;enum Cyc_Absyn_Scope _tmp48C;_LL1: _tmp48C=_tmp48B->sc;_tmp48D=_tmp48B->name;_tmp48E=_tmp48B->varloc;_tmp48F=_tmp48B->tq;_tmp490=_tmp48B->type;_tmp491=_tmp48B->initializer;_tmp492=_tmp48B->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp48C;struct _tuple0*name=_tmp48D;unsigned varloc=_tmp48E;struct Cyc_Absyn_Tqual tq=_tmp48F;void*type=_tmp490;struct Cyc_Absyn_Exp*initializer=_tmp491;struct Cyc_List_List*atts=_tmp492;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _tmp493=Cyc_Flags_c_compiler;if(_tmp493 == Cyc_Flags_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1719
{void*_tmp494=Cyc_Absyn_compress(type);void*_stmttmp1E=_tmp494;void*_tmp495=_stmttmp1E;struct Cyc_List_List*_tmp496;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp495)->tag == 5U){_LL9: _tmp496=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp495)->f1).attributes;_LLA: {struct Cyc_List_List*atts2=_tmp496;
# 1721
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1725
goto _LL3;}_LL3:;}{
# 1728
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Flags_C_Compilers _tmp497=Cyc_Flags_c_compiler;if(_tmp497 == Cyc_Flags_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1733
s=({struct Cyc_PP_Doc*_tmp498[5U];_tmp498[0]=tmp_doc,({
# 1735
struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_scope2doc(sc);_tmp498[1]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=({struct Cyc_Absyn_Tqual _tmp847=tq;void*_tmp846=type;Cyc_Absynpp_tqtd2doc(_tmp847,_tmp846,({struct Cyc_Core_Opt*_tmp49A=_cycalloc(sizeof(*_tmp49A));({struct Cyc_PP_Doc*_tmp845=({struct Cyc_PP_Doc*_tmp499[2U];_tmp499[0]=beforenamedoc,_tmp499[1]=sn;Cyc_PP_cat(_tag_fat(_tmp499,sizeof(struct Cyc_PP_Doc*),2U));});_tmp49A->v=_tmp845;});_tmp49A;}));});_tmp498[2]=_tmp848;}),
initializer == 0?({
struct Cyc_PP_Doc*_tmp844=Cyc_PP_nil_doc();_tmp498[3]=_tmp844;}):({
struct Cyc_PP_Doc*_tmp843=({struct Cyc_PP_Doc*_tmp49B[2U];({struct Cyc_PP_Doc*_tmp842=Cyc_PP_text(({const char*_tmp49C=" = ";_tag_fat(_tmp49C,sizeof(char),4U);}));_tmp49B[0]=_tmp842;}),({struct Cyc_PP_Doc*_tmp841=Cyc_Absynpp_exp2doc(initializer);_tmp49B[1]=_tmp841;});Cyc_PP_cat(_tag_fat(_tmp49B,sizeof(struct Cyc_PP_Doc*),2U));});_tmp498[3]=_tmp843;}),({
struct Cyc_PP_Doc*_tmp840=Cyc_PP_text(({const char*_tmp49D=";";_tag_fat(_tmp49D,sizeof(char),2U);}));_tmp498[4]=_tmp840;});Cyc_PP_cat(_tag_fat(_tmp498,sizeof(struct Cyc_PP_Doc*),5U));});
return s;}}}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1744
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
struct _tuple20 _tmp49E=*x;struct _tuple20 _stmttmp1F=_tmp49E;struct _tuple20 _tmp49F=_stmttmp1F;struct _tuple0*_tmp4A0;_LL1: _tmp4A0=_tmp49F.f2;_LL2: {struct _tuple0*v=_tmp4A0;
return Cyc_Absynpp_qvar2doc(v);}}
# 1749
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp4A1[4U];({struct Cyc_PP_Doc*_tmp84D=Cyc_Absynpp_scope2doc(ad->sc);_tmp4A1[0]=_tmp84D;}),({
struct Cyc_PP_Doc*_tmp84C=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp4A1[1]=_tmp84C;}),({
struct Cyc_PP_Doc*_tmp84B=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp4A1[2]=_tmp84B;}),({
struct Cyc_PP_Doc*_tmp84A=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp4A1[3]=_tmp84A;});Cyc_PP_cat(_tag_fat(_tmp4A1,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1756
return({struct Cyc_PP_Doc*_tmp4A2[12U];({struct Cyc_PP_Doc*_tmp85E=Cyc_Absynpp_scope2doc(ad->sc);_tmp4A2[0]=_tmp85E;}),({
struct Cyc_PP_Doc*_tmp85D=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp4A2[1]=_tmp85D;}),({
struct Cyc_PP_Doc*_tmp85C=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp4A2[2]=_tmp85C;}),({
struct Cyc_PP_Doc*_tmp85B=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp4A2[3]=_tmp85B;}),({
struct Cyc_PP_Doc*_tmp85A=Cyc_PP_blank_doc();_tmp4A2[4]=_tmp85A;}),({struct Cyc_PP_Doc*_tmp859=Cyc_Absynpp_lb();_tmp4A2[5]=_tmp859;}),({
struct Cyc_PP_Doc*_tmp858=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp4A2[6]=_tmp858;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp857=Cyc_PP_nil_doc();_tmp4A2[7]=_tmp857;}):({
struct Cyc_PP_Doc*_tmp856=({struct Cyc_PP_Doc*_tmp4A3[2U];({struct Cyc_PP_Doc*_tmp855=Cyc_PP_text(({const char*_tmp4A4=":";_tag_fat(_tmp4A4,sizeof(char),2U);}));_tmp4A3[0]=_tmp855;}),({struct Cyc_PP_Doc*_tmp854=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp4A3[1]=_tmp854;});Cyc_PP_cat(_tag_fat(_tmp4A3,sizeof(struct Cyc_PP_Doc*),2U));});_tmp4A2[7]=_tmp856;}),({
struct Cyc_PP_Doc*_tmp853=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4A5[2U];({struct Cyc_PP_Doc*_tmp852=Cyc_PP_line_doc();_tmp4A5[0]=_tmp852;}),({struct Cyc_PP_Doc*_tmp851=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp4A5[1]=_tmp851;});Cyc_PP_cat(_tag_fat(_tmp4A5,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4A2[8]=_tmp853;}),({
struct Cyc_PP_Doc*_tmp850=Cyc_PP_line_doc();_tmp4A2[9]=_tmp850;}),({
struct Cyc_PP_Doc*_tmp84F=Cyc_Absynpp_rb();_tmp4A2[10]=_tmp84F;}),({
struct Cyc_PP_Doc*_tmp84E=Cyc_Absynpp_atts2doc(ad->attributes);_tmp4A2[11]=_tmp84E;});Cyc_PP_cat(_tag_fat(_tmp4A2,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1770
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp4A6=dd;int _tmp4AB;struct Cyc_Core_Opt*_tmp4AA;struct Cyc_List_List*_tmp4A9;struct _tuple0*_tmp4A8;enum Cyc_Absyn_Scope _tmp4A7;_LL1: _tmp4A7=_tmp4A6->sc;_tmp4A8=_tmp4A6->name;_tmp4A9=_tmp4A6->tvs;_tmp4AA=_tmp4A6->fields;_tmp4AB=_tmp4A6->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp4A7;struct _tuple0*name=_tmp4A8;struct Cyc_List_List*tvs=_tmp4A9;struct Cyc_Core_Opt*fields=_tmp4AA;int is_x=_tmp4AB;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp4AC[5U];({struct Cyc_PP_Doc*_tmp865=Cyc_Absynpp_scope2doc(sc);_tmp4AC[0]=_tmp865;}),
is_x?({struct Cyc_PP_Doc*_tmp864=Cyc_PP_text(({const char*_tmp4AD="@extensible ";_tag_fat(_tmp4AD,sizeof(char),13U);}));_tmp4AC[1]=_tmp864;}):({struct Cyc_PP_Doc*_tmp863=Cyc_PP_blank_doc();_tmp4AC[1]=_tmp863;}),({
struct Cyc_PP_Doc*_tmp862=Cyc_PP_text(({const char*_tmp4AE="datatype ";_tag_fat(_tmp4AE,sizeof(char),10U);}));_tmp4AC[2]=_tmp862;}),
is_x?({struct Cyc_PP_Doc*_tmp861=Cyc_Absynpp_qvar2bolddoc(name);_tmp4AC[3]=_tmp861;}):({struct Cyc_PP_Doc*_tmp860=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp4AC[3]=_tmp860;}),({
struct Cyc_PP_Doc*_tmp85F=Cyc_Absynpp_ktvars2doc(tvs);_tmp4AC[4]=_tmp85F;});Cyc_PP_cat(_tag_fat(_tmp4AC,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1779
return({struct Cyc_PP_Doc*_tmp4AF[10U];({struct Cyc_PP_Doc*_tmp873=Cyc_Absynpp_scope2doc(sc);_tmp4AF[0]=_tmp873;}),
is_x?({struct Cyc_PP_Doc*_tmp872=Cyc_PP_text(({const char*_tmp4B0="@extensible ";_tag_fat(_tmp4B0,sizeof(char),13U);}));_tmp4AF[1]=_tmp872;}):({struct Cyc_PP_Doc*_tmp871=Cyc_PP_blank_doc();_tmp4AF[1]=_tmp871;}),({
struct Cyc_PP_Doc*_tmp870=Cyc_PP_text(({const char*_tmp4B1="datatype ";_tag_fat(_tmp4B1,sizeof(char),10U);}));_tmp4AF[2]=_tmp870;}),
is_x?({struct Cyc_PP_Doc*_tmp86F=Cyc_Absynpp_qvar2bolddoc(name);_tmp4AF[3]=_tmp86F;}):({struct Cyc_PP_Doc*_tmp86E=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp4AF[3]=_tmp86E;}),({
struct Cyc_PP_Doc*_tmp86D=Cyc_Absynpp_ktvars2doc(tvs);_tmp4AF[4]=_tmp86D;}),({
struct Cyc_PP_Doc*_tmp86C=Cyc_PP_blank_doc();_tmp4AF[5]=_tmp86C;}),({struct Cyc_PP_Doc*_tmp86B=Cyc_Absynpp_lb();_tmp4AF[6]=_tmp86B;}),({
struct Cyc_PP_Doc*_tmp86A=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4B2[2U];({struct Cyc_PP_Doc*_tmp869=Cyc_PP_line_doc();_tmp4B2[0]=_tmp869;}),({struct Cyc_PP_Doc*_tmp868=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_tmp4B2[1]=_tmp868;});Cyc_PP_cat(_tag_fat(_tmp4B2,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4AF[7]=_tmp86A;}),({
struct Cyc_PP_Doc*_tmp867=Cyc_PP_line_doc();_tmp4AF[8]=_tmp867;}),({
struct Cyc_PP_Doc*_tmp866=Cyc_Absynpp_rb();_tmp4AF[9]=_tmp866;});Cyc_PP_cat(_tag_fat(_tmp4AF,sizeof(struct Cyc_PP_Doc*),10U));});}}}
# 1790
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp4B3=ed;struct Cyc_Core_Opt*_tmp4B6;struct _tuple0*_tmp4B5;enum Cyc_Absyn_Scope _tmp4B4;_LL1: _tmp4B4=_tmp4B3->sc;_tmp4B5=_tmp4B3->name;_tmp4B6=_tmp4B3->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp4B4;struct _tuple0*n=_tmp4B5;struct Cyc_Core_Opt*fields=_tmp4B6;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp4B7[3U];({struct Cyc_PP_Doc*_tmp876=Cyc_Absynpp_scope2doc(sc);_tmp4B7[0]=_tmp876;}),({
struct Cyc_PP_Doc*_tmp875=Cyc_PP_text(({const char*_tmp4B8="enum ";_tag_fat(_tmp4B8,sizeof(char),6U);}));_tmp4B7[1]=_tmp875;}),({
struct Cyc_PP_Doc*_tmp874=Cyc_Absynpp_typedef_name2bolddoc(n);_tmp4B7[2]=_tmp874;});Cyc_PP_cat(_tag_fat(_tmp4B7,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1798
return({struct Cyc_PP_Doc*_tmp4B9[8U];({struct Cyc_PP_Doc*_tmp880=Cyc_Absynpp_scope2doc(sc);_tmp4B9[0]=_tmp880;}),({
struct Cyc_PP_Doc*_tmp87F=Cyc_PP_text(({const char*_tmp4BA="enum ";_tag_fat(_tmp4BA,sizeof(char),6U);}));_tmp4B9[1]=_tmp87F;}),({
struct Cyc_PP_Doc*_tmp87E=Cyc_Absynpp_qvar2bolddoc(n);_tmp4B9[2]=_tmp87E;}),({
struct Cyc_PP_Doc*_tmp87D=Cyc_PP_blank_doc();_tmp4B9[3]=_tmp87D;}),({struct Cyc_PP_Doc*_tmp87C=Cyc_Absynpp_lb();_tmp4B9[4]=_tmp87C;}),({
struct Cyc_PP_Doc*_tmp87B=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4BB[2U];({struct Cyc_PP_Doc*_tmp87A=Cyc_PP_line_doc();_tmp4BB[0]=_tmp87A;}),({struct Cyc_PP_Doc*_tmp879=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_tmp4BB[1]=_tmp879;});Cyc_PP_cat(_tag_fat(_tmp4BB,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4B9[5]=_tmp87B;}),({
struct Cyc_PP_Doc*_tmp878=Cyc_PP_line_doc();_tmp4B9[6]=_tmp878;}),({
struct Cyc_PP_Doc*_tmp877=Cyc_Absynpp_rb();_tmp4B9[7]=_tmp877;});Cyc_PP_cat(_tag_fat(_tmp4B9,sizeof(struct Cyc_PP_Doc*),8U));});}}}
# 1807
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp4BC=d->r;void*_stmttmp20=_tmp4BC;void*_tmp4BD=_stmttmp20;struct _tuple10*_tmp4C1;struct Cyc_List_List*_tmp4C0;struct Cyc_List_List*_tmp4BF;struct Cyc_List_List*_tmp4BE;struct Cyc_List_List*_tmp4C2;struct Cyc_List_List*_tmp4C4;struct _tuple0*_tmp4C3;struct Cyc_List_List*_tmp4C6;struct _fat_ptr*_tmp4C5;struct Cyc_Absyn_Typedefdecl*_tmp4C7;struct Cyc_Absyn_Enumdecl*_tmp4C8;struct Cyc_List_List*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Pat*_tmp4CA;struct Cyc_Absyn_Datatypedecl*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Vardecl*_tmp4CE;struct Cyc_Absyn_Tvar*_tmp4CD;struct Cyc_Absyn_Vardecl*_tmp4D0;struct Cyc_Absyn_Aggrdecl*_tmp4D1;struct Cyc_Absyn_Fndecl*_tmp4D2;switch(*((int*)_tmp4BD)){case 1U: _LL1: _tmp4D2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp4D2;
# 1811
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->tag=5U,_tmp4E3->f1=type_info;_tmp4E3;});
if(fd->cached_type != 0){
void*_tmp4D3=Cyc_Absyn_compress((void*)_check_null(fd->cached_type));void*_stmttmp21=_tmp4D3;void*_tmp4D4=_stmttmp21;struct Cyc_Absyn_FnInfo _tmp4D5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D4)->tag == 5U){_LL24: _tmp4D5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D4)->f1;_LL25: {struct Cyc_Absyn_FnInfo i=_tmp4D5;
# 1817
({struct Cyc_List_List*_tmp881=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp881;});goto _LL23;}}else{_LL26: _LL27:
({void*_tmp4D6=0U;({struct _fat_ptr _tmp882=({const char*_tmp4D7="function has non-function type";_tag_fat(_tmp4D7,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp882,_tag_fat(_tmp4D6,sizeof(void*),0U));});});}_LL23:;}{
# 1820
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((fd->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
enum Cyc_Flags_C_Compilers _tmp4D8=Cyc_Flags_c_compiler;if(_tmp4D8 == Cyc_Flags_Gcc_c){_LL29: _LL2A:
 inlinedoc=Cyc_PP_text(({const char*_tmp4D9="inline ";_tag_fat(_tmp4D9,sizeof(char),8U);}));goto _LL28;}else{_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp4DA="__inline ";_tag_fat(_tmp4DA,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1828
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _tmp4DB=Cyc_Flags_c_compiler;if(_tmp4DB == Cyc_Flags_Gcc_c){_LL2E: _LL2F:
 beforenamedoc=attsdoc;goto _LL2D;}else{_LL30: _LL31:
 beforenamedoc=Cyc_Absynpp_callconv2doc((fd->i).attributes);goto _LL2D;}_LL2D:;}{
# 1835
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp885=Cyc_Absyn_empty_tqual(0U);void*_tmp884=t;Cyc_Absynpp_tqtd2doc(_tmp885,_tmp884,({struct Cyc_Core_Opt*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));({
struct Cyc_PP_Doc*_tmp883=({struct Cyc_PP_Doc*_tmp4E1[2U];_tmp4E1[0]=beforenamedoc,_tmp4E1[1]=namedoc;Cyc_PP_cat(_tag_fat(_tmp4E1,sizeof(struct Cyc_PP_Doc*),2U));});_tmp4E2->v=_tmp883;});_tmp4E2;}));});
# 1843
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp4DF[5U];({struct Cyc_PP_Doc*_tmp88C=Cyc_PP_blank_doc();_tmp4DF[0]=_tmp88C;}),({struct Cyc_PP_Doc*_tmp88B=Cyc_Absynpp_lb();_tmp4DF[1]=_tmp88B;}),({
struct Cyc_PP_Doc*_tmp88A=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4E0[2U];({struct Cyc_PP_Doc*_tmp889=Cyc_PP_line_doc();_tmp4E0[0]=_tmp889;}),({struct Cyc_PP_Doc*_tmp888=Cyc_Absynpp_stmt2doc(fd->body,0,0);_tmp4E0[1]=_tmp888;});Cyc_PP_cat(_tag_fat(_tmp4E0,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp4DF[2]=_tmp88A;}),({
struct Cyc_PP_Doc*_tmp887=Cyc_PP_line_doc();_tmp4DF[3]=_tmp887;}),({
struct Cyc_PP_Doc*_tmp886=Cyc_Absynpp_rb();_tmp4DF[4]=_tmp886;});Cyc_PP_cat(_tag_fat(_tmp4DF,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp4DC[4U];_tmp4DC[0]=inlinedoc,_tmp4DC[1]=scopedoc,_tmp4DC[2]=tqtddoc,_tmp4DC[3]=bodydoc;Cyc_PP_cat(_tag_fat(_tmp4DC,sizeof(struct Cyc_PP_Doc*),4U));});
# 1849
{enum Cyc_Flags_C_Compilers _tmp4DD=Cyc_Flags_c_compiler;if(_tmp4DD == Cyc_Flags_Vc_c){_LL33: _LL34:
 s=({struct Cyc_PP_Doc*_tmp4DE[2U];_tmp4DE[0]=attsdoc,_tmp4DE[1]=s;Cyc_PP_cat(_tag_fat(_tmp4DE,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 1854
goto _LL0;}}}}}case 5U: _LL3: _tmp4D1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp4D1;
# 1857
s=({struct Cyc_PP_Doc*_tmp4E4[2U];({struct Cyc_PP_Doc*_tmp88E=Cyc_Absynpp_aggrdecl2doc(ad);_tmp4E4[0]=_tmp88E;}),({struct Cyc_PP_Doc*_tmp88D=Cyc_PP_text(({const char*_tmp4E5=";";_tag_fat(_tmp4E5,sizeof(char),2U);}));_tmp4E4[1]=_tmp88D;});Cyc_PP_cat(_tag_fat(_tmp4E4,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 0U: _LL5: _tmp4D0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp4D0;
# 1860
s=Cyc_Absynpp_vardecl2doc(vd);
goto _LL0;}case 4U: _LL7: _tmp4CD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_tmp4CE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4BD)->f2;_tmp4CF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4BD)->f3;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp4CD;struct Cyc_Absyn_Vardecl*vd=_tmp4CE;struct Cyc_Absyn_Exp*open_exp_opt=_tmp4CF;
# 1863
s=({struct Cyc_PP_Doc*_tmp4E6[7U];({struct Cyc_PP_Doc*_tmp899=Cyc_PP_text(({const char*_tmp4E7="region";_tag_fat(_tmp4E7,sizeof(char),7U);}));_tmp4E6[0]=_tmp899;}),({
struct Cyc_PP_Doc*_tmp898=Cyc_PP_text(({const char*_tmp4E8="<";_tag_fat(_tmp4E8,sizeof(char),2U);}));_tmp4E6[1]=_tmp898;}),({
struct Cyc_PP_Doc*_tmp897=Cyc_Absynpp_tvar2doc(tv);_tmp4E6[2]=_tmp897;}),({
struct Cyc_PP_Doc*_tmp896=Cyc_PP_text(({const char*_tmp4E9=">";_tag_fat(_tmp4E9,sizeof(char),2U);}));_tmp4E6[3]=_tmp896;}),({
struct Cyc_PP_Doc*_tmp895=Cyc_Absynpp_qvar2doc(vd->name);_tmp4E6[4]=_tmp895;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_tmp894=({struct Cyc_PP_Doc*_tmp4EA[3U];({struct Cyc_PP_Doc*_tmp893=Cyc_PP_text(({const char*_tmp4EB=" = open(";_tag_fat(_tmp4EB,sizeof(char),9U);}));_tmp4EA[0]=_tmp893;}),({struct Cyc_PP_Doc*_tmp892=Cyc_Absynpp_exp2doc(open_exp_opt);_tmp4EA[1]=_tmp892;}),({
struct Cyc_PP_Doc*_tmp891=Cyc_PP_text(({const char*_tmp4EC=")";_tag_fat(_tmp4EC,sizeof(char),2U);}));_tmp4EA[2]=_tmp891;});Cyc_PP_cat(_tag_fat(_tmp4EA,sizeof(struct Cyc_PP_Doc*),3U));});
# 1868
_tmp4E6[5]=_tmp894;}):({
struct Cyc_PP_Doc*_tmp890=Cyc_PP_nil_doc();_tmp4E6[5]=_tmp890;}),({
struct Cyc_PP_Doc*_tmp88F=Cyc_PP_text(({const char*_tmp4ED=";";_tag_fat(_tmp4ED,sizeof(char),2U);}));_tmp4E6[6]=_tmp88F;});Cyc_PP_cat(_tag_fat(_tmp4E6,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 6U: _LL9: _tmp4CC=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_LLA: {struct Cyc_Absyn_Datatypedecl*dd=_tmp4CC;
# 1873
s=({struct Cyc_PP_Doc*_tmp4EE[2U];({struct Cyc_PP_Doc*_tmp89B=Cyc_Absynpp_datatypedecl2doc(dd);_tmp4EE[0]=_tmp89B;}),({struct Cyc_PP_Doc*_tmp89A=Cyc_PP_text(({const char*_tmp4EF=";";_tag_fat(_tmp4EF,sizeof(char),2U);}));_tmp4EE[1]=_tmp89A;});Cyc_PP_cat(_tag_fat(_tmp4EE,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 2U: _LLB: _tmp4CA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_tmp4CB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4BD)->f3;_LLC: {struct Cyc_Absyn_Pat*p=_tmp4CA;struct Cyc_Absyn_Exp*e=_tmp4CB;
# 1876
s=({struct Cyc_PP_Doc*_tmp4F0[5U];({struct Cyc_PP_Doc*_tmp8A0=Cyc_PP_text(({const char*_tmp4F1="let ";_tag_fat(_tmp4F1,sizeof(char),5U);}));_tmp4F0[0]=_tmp8A0;}),({
struct Cyc_PP_Doc*_tmp89F=Cyc_Absynpp_pat2doc(p);_tmp4F0[1]=_tmp89F;}),({
struct Cyc_PP_Doc*_tmp89E=Cyc_PP_text(({const char*_tmp4F2=" = ";_tag_fat(_tmp4F2,sizeof(char),4U);}));_tmp4F0[2]=_tmp89E;}),({
struct Cyc_PP_Doc*_tmp89D=Cyc_Absynpp_exp2doc(e);_tmp4F0[3]=_tmp89D;}),({
struct Cyc_PP_Doc*_tmp89C=Cyc_PP_text(({const char*_tmp4F3=";";_tag_fat(_tmp4F3,sizeof(char),2U);}));_tmp4F0[4]=_tmp89C;});Cyc_PP_cat(_tag_fat(_tmp4F0,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 3U: _LLD: _tmp4C9=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_LLE: {struct Cyc_List_List*vds=_tmp4C9;
# 1883
s=({struct Cyc_PP_Doc*_tmp4F4[3U];({struct Cyc_PP_Doc*_tmp8A3=Cyc_PP_text(({const char*_tmp4F5="let ";_tag_fat(_tmp4F5,sizeof(char),5U);}));_tmp4F4[0]=_tmp8A3;}),({struct Cyc_PP_Doc*_tmp8A2=Cyc_Absynpp_ids2doc(vds);_tmp4F4[1]=_tmp8A2;}),({struct Cyc_PP_Doc*_tmp8A1=Cyc_PP_text(({const char*_tmp4F6=";";_tag_fat(_tmp4F6,sizeof(char),2U);}));_tmp4F4[2]=_tmp8A1;});Cyc_PP_cat(_tag_fat(_tmp4F4,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 7U: _LLF: _tmp4C8=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp4C8;
# 1886
s=({struct Cyc_PP_Doc*_tmp4F7[2U];({struct Cyc_PP_Doc*_tmp8A5=Cyc_Absynpp_enumdecl2doc(ed);_tmp4F7[0]=_tmp8A5;}),({struct Cyc_PP_Doc*_tmp8A4=Cyc_PP_text(({const char*_tmp4F8=";";_tag_fat(_tmp4F8,sizeof(char),2U);}));_tmp4F7[1]=_tmp8A4;});Cyc_PP_cat(_tag_fat(_tmp4F7,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 8U: _LL11: _tmp4C7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp4C7;
# 1889
void*t;
if(td->defn != 0)
t=(void*)_check_null(td->defn);else{
# 1893
t=Cyc_Absyn_new_evar(td->kind,0);}
s=({struct Cyc_PP_Doc*_tmp4F9[4U];({struct Cyc_PP_Doc*_tmp8AE=Cyc_PP_text(({const char*_tmp4FA="typedef ";_tag_fat(_tmp4FA,sizeof(char),9U);}));_tmp4F9[0]=_tmp8AE;}),({
struct Cyc_PP_Doc*_tmp8AD=({struct Cyc_Absyn_Tqual _tmp8AC=td->tq;void*_tmp8AB=t;Cyc_Absynpp_tqtd2doc(_tmp8AC,_tmp8AB,({struct Cyc_Core_Opt*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));({
# 1897
struct Cyc_PP_Doc*_tmp8AA=({struct Cyc_PP_Doc*_tmp4FB[2U];({struct Cyc_PP_Doc*_tmp8A9=Cyc_Absynpp_typedef_name2bolddoc(td->name);_tmp4FB[0]=_tmp8A9;}),({
struct Cyc_PP_Doc*_tmp8A8=Cyc_Absynpp_tvars2doc(td->tvs);_tmp4FB[1]=_tmp8A8;});Cyc_PP_cat(_tag_fat(_tmp4FB,sizeof(struct Cyc_PP_Doc*),2U));});
# 1897
_tmp4FC->v=_tmp8AA;});_tmp4FC;}));});
# 1895
_tmp4F9[1]=_tmp8AD;}),({
# 1900
struct Cyc_PP_Doc*_tmp8A7=Cyc_Absynpp_atts2doc(td->atts);_tmp4F9[2]=_tmp8A7;}),({
struct Cyc_PP_Doc*_tmp8A6=Cyc_PP_text(({const char*_tmp4FD=";";_tag_fat(_tmp4FD,sizeof(char),2U);}));_tmp4F9[3]=_tmp8A6;});Cyc_PP_cat(_tag_fat(_tmp4F9,sizeof(struct Cyc_PP_Doc*),4U));});
# 1903
goto _LL0;}case 9U: _LL13: _tmp4C5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_tmp4C6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4BD)->f2;_LL14: {struct _fat_ptr*v=_tmp4C5;struct Cyc_List_List*tdl=_tmp4C6;
# 1905
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_tmp4FE[8U];({struct Cyc_PP_Doc*_tmp8B7=Cyc_PP_text(({const char*_tmp4FF="namespace ";_tag_fat(_tmp4FF,sizeof(char),11U);}));_tmp4FE[0]=_tmp8B7;}),({
struct Cyc_PP_Doc*_tmp8B6=Cyc_PP_textptr(v);_tmp4FE[1]=_tmp8B6;}),({
struct Cyc_PP_Doc*_tmp8B5=Cyc_PP_blank_doc();_tmp4FE[2]=_tmp8B5;}),({struct Cyc_PP_Doc*_tmp8B4=Cyc_Absynpp_lb();_tmp4FE[3]=_tmp8B4;}),({
struct Cyc_PP_Doc*_tmp8B3=Cyc_PP_line_doc();_tmp4FE[4]=_tmp8B3;}),({
struct Cyc_PP_Doc*_tmp8B2=({struct _fat_ptr _tmp8B1=({const char*_tmp500="";_tag_fat(_tmp500,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8B1,tdl);});_tmp4FE[5]=_tmp8B2;}),({
struct Cyc_PP_Doc*_tmp8B0=Cyc_PP_line_doc();_tmp4FE[6]=_tmp8B0;}),({
struct Cyc_PP_Doc*_tmp8AF=Cyc_Absynpp_rb();_tmp4FE[7]=_tmp8AF;});Cyc_PP_cat(_tag_fat(_tmp4FE,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10U: _LL15: _tmp4C3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_tmp4C4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4BD)->f2;_LL16: {struct _tuple0*q=_tmp4C3;struct Cyc_List_List*tdl=_tmp4C4;
# 1916
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp501[8U];({struct Cyc_PP_Doc*_tmp8C0=Cyc_PP_text(({const char*_tmp502="using ";_tag_fat(_tmp502,sizeof(char),7U);}));_tmp501[0]=_tmp8C0;}),({
struct Cyc_PP_Doc*_tmp8BF=Cyc_Absynpp_qvar2doc(q);_tmp501[1]=_tmp8BF;}),({
struct Cyc_PP_Doc*_tmp8BE=Cyc_PP_blank_doc();_tmp501[2]=_tmp8BE;}),({struct Cyc_PP_Doc*_tmp8BD=Cyc_Absynpp_lb();_tmp501[3]=_tmp8BD;}),({
struct Cyc_PP_Doc*_tmp8BC=Cyc_PP_line_doc();_tmp501[4]=_tmp8BC;}),({
struct Cyc_PP_Doc*_tmp8BB=({struct _fat_ptr _tmp8BA=({const char*_tmp503="";_tag_fat(_tmp503,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8BA,tdl);});_tmp501[5]=_tmp8BB;}),({
struct Cyc_PP_Doc*_tmp8B9=Cyc_PP_line_doc();_tmp501[6]=_tmp8B9;}),({
struct Cyc_PP_Doc*_tmp8B8=Cyc_Absynpp_rb();_tmp501[7]=_tmp8B8;});Cyc_PP_cat(_tag_fat(_tmp501,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1925
s=({struct Cyc_PP_Doc*_tmp504[11U];({struct Cyc_PP_Doc*_tmp8CC=Cyc_PP_text(({const char*_tmp505="/* using ";_tag_fat(_tmp505,sizeof(char),10U);}));_tmp504[0]=_tmp8CC;}),({
struct Cyc_PP_Doc*_tmp8CB=Cyc_Absynpp_qvar2doc(q);_tmp504[1]=_tmp8CB;}),({
struct Cyc_PP_Doc*_tmp8CA=Cyc_PP_blank_doc();_tmp504[2]=_tmp8CA;}),({
struct Cyc_PP_Doc*_tmp8C9=Cyc_Absynpp_lb();_tmp504[3]=_tmp8C9;}),({
struct Cyc_PP_Doc*_tmp8C8=Cyc_PP_text(({const char*_tmp506=" */";_tag_fat(_tmp506,sizeof(char),4U);}));_tmp504[4]=_tmp8C8;}),({
struct Cyc_PP_Doc*_tmp8C7=Cyc_PP_line_doc();_tmp504[5]=_tmp8C7;}),({
struct Cyc_PP_Doc*_tmp8C6=({struct _fat_ptr _tmp8C5=({const char*_tmp507="";_tag_fat(_tmp507,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8C5,tdl);});_tmp504[6]=_tmp8C6;}),({
struct Cyc_PP_Doc*_tmp8C4=Cyc_PP_line_doc();_tmp504[7]=_tmp8C4;}),({
struct Cyc_PP_Doc*_tmp8C3=Cyc_PP_text(({const char*_tmp508="/* ";_tag_fat(_tmp508,sizeof(char),4U);}));_tmp504[8]=_tmp8C3;}),({
struct Cyc_PP_Doc*_tmp8C2=Cyc_Absynpp_rb();_tmp504[9]=_tmp8C2;}),({
struct Cyc_PP_Doc*_tmp8C1=Cyc_PP_text(({const char*_tmp509=" */";_tag_fat(_tmp509,sizeof(char),4U);}));_tmp504[10]=_tmp8C1;});Cyc_PP_cat(_tag_fat(_tmp504,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;}case 11U: _LL17: _tmp4C2=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_LL18: {struct Cyc_List_List*tdl=_tmp4C2;
# 1938
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp50A[6U];({struct Cyc_PP_Doc*_tmp8D3=Cyc_PP_text(({const char*_tmp50B="extern \"C\" ";_tag_fat(_tmp50B,sizeof(char),12U);}));_tmp50A[0]=_tmp8D3;}),({
struct Cyc_PP_Doc*_tmp8D2=Cyc_Absynpp_lb();_tmp50A[1]=_tmp8D2;}),({
struct Cyc_PP_Doc*_tmp8D1=Cyc_PP_line_doc();_tmp50A[2]=_tmp8D1;}),({
struct Cyc_PP_Doc*_tmp8D0=({struct _fat_ptr _tmp8CF=({const char*_tmp50C="";_tag_fat(_tmp50C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8CF,tdl);});_tmp50A[3]=_tmp8D0;}),({
struct Cyc_PP_Doc*_tmp8CE=Cyc_PP_line_doc();_tmp50A[4]=_tmp8CE;}),({
struct Cyc_PP_Doc*_tmp8CD=Cyc_Absynpp_rb();_tmp50A[5]=_tmp8CD;});Cyc_PP_cat(_tag_fat(_tmp50A,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1946
s=({struct Cyc_PP_Doc*_tmp50D[9U];({struct Cyc_PP_Doc*_tmp8DD=Cyc_PP_text(({const char*_tmp50E="/* extern \"C\" ";_tag_fat(_tmp50E,sizeof(char),15U);}));_tmp50D[0]=_tmp8DD;}),({
struct Cyc_PP_Doc*_tmp8DC=Cyc_Absynpp_lb();_tmp50D[1]=_tmp8DC;}),({
struct Cyc_PP_Doc*_tmp8DB=Cyc_PP_text(({const char*_tmp50F=" */";_tag_fat(_tmp50F,sizeof(char),4U);}));_tmp50D[2]=_tmp8DB;}),({
struct Cyc_PP_Doc*_tmp8DA=Cyc_PP_line_doc();_tmp50D[3]=_tmp8DA;}),({
struct Cyc_PP_Doc*_tmp8D9=({struct _fat_ptr _tmp8D8=({const char*_tmp510="";_tag_fat(_tmp510,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8D8,tdl);});_tmp50D[4]=_tmp8D9;}),({
struct Cyc_PP_Doc*_tmp8D7=Cyc_PP_line_doc();_tmp50D[5]=_tmp8D7;}),({
struct Cyc_PP_Doc*_tmp8D6=Cyc_PP_text(({const char*_tmp511="/* ";_tag_fat(_tmp511,sizeof(char),4U);}));_tmp50D[6]=_tmp8D6;}),({
struct Cyc_PP_Doc*_tmp8D5=Cyc_Absynpp_rb();_tmp50D[7]=_tmp8D5;}),({
struct Cyc_PP_Doc*_tmp8D4=Cyc_PP_text(({const char*_tmp512=" */";_tag_fat(_tmp512,sizeof(char),4U);}));_tmp50D[8]=_tmp8D4;});Cyc_PP_cat(_tag_fat(_tmp50D,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 12U: _LL19: _tmp4BE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BD)->f1;_tmp4BF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BD)->f2;_tmp4C0=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BD)->f3;_tmp4C1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BD)->f4;_LL1A: {struct Cyc_List_List*tdl=_tmp4BE;struct Cyc_List_List*ovrs=_tmp4BF;struct Cyc_List_List*exs=_tmp4C0;struct _tuple10*wc=_tmp4C1;
# 1957
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp513[7U];({struct Cyc_PP_Doc*_tmp8E5=Cyc_Absynpp_rb();_tmp513[0]=_tmp8E5;}),({struct Cyc_PP_Doc*_tmp8E4=Cyc_PP_text(({const char*_tmp514=" export ";_tag_fat(_tmp514,sizeof(char),9U);}));_tmp513[1]=_tmp8E4;}),({struct Cyc_PP_Doc*_tmp8E3=Cyc_Absynpp_lb();_tmp513[2]=_tmp8E3;}),({
struct Cyc_PP_Doc*_tmp8E2=Cyc_PP_line_doc();_tmp513[3]=_tmp8E2;}),({struct Cyc_PP_Doc*_tmp8E1=({struct _fat_ptr _tmp8E0=({const char*_tmp515=",";_tag_fat(_tmp515,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple20*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp8E0,exs);});_tmp513[4]=_tmp8E1;}),({
struct Cyc_PP_Doc*_tmp8DF=Cyc_PP_line_doc();_tmp513[5]=_tmp8DF;}),({struct Cyc_PP_Doc*_tmp8DE=Cyc_Absynpp_rb();_tmp513[6]=_tmp8DE;});Cyc_PP_cat(_tag_fat(_tmp513,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1965
exs_doc=Cyc_Absynpp_rb();}
if(ovrs != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp516[7U];({struct Cyc_PP_Doc*_tmp8ED=Cyc_Absynpp_rb();_tmp516[0]=_tmp8ED;}),({struct Cyc_PP_Doc*_tmp8EC=Cyc_PP_text(({const char*_tmp517=" cycdef ";_tag_fat(_tmp517,sizeof(char),9U);}));_tmp516[1]=_tmp8EC;}),({struct Cyc_PP_Doc*_tmp8EB=Cyc_Absynpp_lb();_tmp516[2]=_tmp8EB;}),({
struct Cyc_PP_Doc*_tmp8EA=Cyc_PP_line_doc();_tmp516[3]=_tmp8EA;}),({struct Cyc_PP_Doc*_tmp8E9=({struct _fat_ptr _tmp8E8=({const char*_tmp518="";_tag_fat(_tmp518,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8E8,ovrs);});_tmp516[4]=_tmp8E9;}),({
struct Cyc_PP_Doc*_tmp8E7=Cyc_PP_line_doc();_tmp516[5]=_tmp8E7;}),({struct Cyc_PP_Doc*_tmp8E6=Cyc_Absynpp_rb();_tmp516[6]=_tmp8E6;});Cyc_PP_cat(_tag_fat(_tmp516,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1971
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp519[6U];({struct Cyc_PP_Doc*_tmp8F3=Cyc_PP_text(({const char*_tmp51A="extern \"C include\" ";_tag_fat(_tmp51A,sizeof(char),20U);}));_tmp519[0]=_tmp8F3;}),({
struct Cyc_PP_Doc*_tmp8F2=Cyc_Absynpp_lb();_tmp519[1]=_tmp8F2;}),({
struct Cyc_PP_Doc*_tmp8F1=Cyc_PP_line_doc();_tmp519[2]=_tmp8F1;}),({
struct Cyc_PP_Doc*_tmp8F0=({struct _fat_ptr _tmp8EF=({const char*_tmp51B="";_tag_fat(_tmp51B,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8EF,tdl);});_tmp519[3]=_tmp8F0;}),({
struct Cyc_PP_Doc*_tmp8EE=Cyc_PP_line_doc();_tmp519[4]=_tmp8EE;}),_tmp519[5]=exs_doc;Cyc_PP_cat(_tag_fat(_tmp519,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1979
s=({struct Cyc_PP_Doc*_tmp51C[9U];({struct Cyc_PP_Doc*_tmp8FD=Cyc_PP_text(({const char*_tmp51D="/* extern \"C include\" ";_tag_fat(_tmp51D,sizeof(char),23U);}));_tmp51C[0]=_tmp8FD;}),({
struct Cyc_PP_Doc*_tmp8FC=Cyc_Absynpp_lb();_tmp51C[1]=_tmp8FC;}),({
struct Cyc_PP_Doc*_tmp8FB=Cyc_PP_text(({const char*_tmp51E=" */";_tag_fat(_tmp51E,sizeof(char),4U);}));_tmp51C[2]=_tmp8FB;}),({
struct Cyc_PP_Doc*_tmp8FA=Cyc_PP_line_doc();_tmp51C[3]=_tmp8FA;}),({
struct Cyc_PP_Doc*_tmp8F9=({struct _fat_ptr _tmp8F8=({const char*_tmp51F="";_tag_fat(_tmp51F,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp8F8,tdl);});_tmp51C[4]=_tmp8F9;}),({
struct Cyc_PP_Doc*_tmp8F7=Cyc_PP_line_doc();_tmp51C[5]=_tmp8F7;}),({
struct Cyc_PP_Doc*_tmp8F6=Cyc_PP_text(({const char*_tmp520="/* ";_tag_fat(_tmp520,sizeof(char),4U);}));_tmp51C[6]=_tmp8F6;}),({
struct Cyc_PP_Doc*_tmp8F5=Cyc_Absynpp_rb();_tmp51C[7]=_tmp8F5;}),({
struct Cyc_PP_Doc*_tmp8F4=Cyc_PP_text(({const char*_tmp521=" */";_tag_fat(_tmp521,sizeof(char),4U);}));_tmp51C[8]=_tmp8F4;});Cyc_PP_cat(_tag_fat(_tmp51C,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 13U: _LL1B: _LL1C:
# 1990
 s=({struct Cyc_PP_Doc*_tmp522[2U];({struct Cyc_PP_Doc*_tmp8FF=Cyc_PP_text(({const char*_tmp523="__cyclone_port_on__;";_tag_fat(_tmp523,sizeof(char),21U);}));_tmp522[0]=_tmp8FF;}),({struct Cyc_PP_Doc*_tmp8FE=Cyc_Absynpp_lb();_tmp522[1]=_tmp8FE;});Cyc_PP_cat(_tag_fat(_tmp522,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1993
 s=({struct Cyc_PP_Doc*_tmp524[2U];({struct Cyc_PP_Doc*_tmp901=Cyc_PP_text(({const char*_tmp525="__cyclone_port_off__;";_tag_fat(_tmp525,sizeof(char),22U);}));_tmp524[0]=_tmp901;}),({struct Cyc_PP_Doc*_tmp900=Cyc_Absynpp_lb();_tmp524[1]=_tmp900;});Cyc_PP_cat(_tag_fat(_tmp524,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1996
 s=({struct Cyc_PP_Doc*_tmp526[2U];({struct Cyc_PP_Doc*_tmp903=Cyc_PP_text(({const char*_tmp527="__tempest_on__;";_tag_fat(_tmp527,sizeof(char),16U);}));_tmp526[0]=_tmp903;}),({struct Cyc_PP_Doc*_tmp902=Cyc_Absynpp_lb();_tmp526[1]=_tmp902;});Cyc_PP_cat(_tag_fat(_tmp526,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1999
 s=({struct Cyc_PP_Doc*_tmp528[2U];({struct Cyc_PP_Doc*_tmp905=Cyc_PP_text(({const char*_tmp529="__tempest_off__;";_tag_fat(_tmp529,sizeof(char),17U);}));_tmp528[0]=_tmp905;}),({struct Cyc_PP_Doc*_tmp904=Cyc_Absynpp_lb();_tmp528[1]=_tmp904;});Cyc_PP_cat(_tag_fat(_tmp528,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 2003
return s;}
# 2006
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp52A=sc;switch(_tmp52A){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp52B="static ";_tag_fat(_tmp52B,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp52C="extern ";_tag_fat(_tmp52C,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp52D="extern \"C\" ";_tag_fat(_tmp52D,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp52E="abstract ";_tag_fat(_tmp52E,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp52F="register ";_tag_fat(_tmp52F,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;}}
# 2020
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp530=Cyc_Absyn_compress(t);void*_stmttmp22=_tmp530;void*_tmp531=_stmttmp22;struct Cyc_List_List*_tmp532;struct Cyc_Absyn_Tvar*_tmp533;switch(*((int*)_tmp531)){case 2U: _LL1: _tmp533=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp531)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp533;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp531)->f1)->tag == 9U){_LL3: _tmp532=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp531)->f2;_LL4: {struct Cyc_List_List*l=_tmp532;
return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2032
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp534=t;void*_tmp536;struct Cyc_Absyn_Typedefdecl*_tmp535;switch(*((int*)_tmp534)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp534)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp535=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp534)->f3;_tmp536=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp534)->f4;if(_tmp536 != 0){_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmp535;void*x=_tmp536;
# 2039
return Cyc_Absynpp_is_anon_aggrtype(x);}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2046
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2049
if(tms != 0 && tms->tl != 0){
struct _tuple18 _tmp537=({struct _tuple18 _tmp5DF;_tmp5DF.f1=(void*)tms->hd,_tmp5DF.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp5DF;});struct _tuple18 _stmttmp23=_tmp537;struct _tuple18 _tmp538=_stmttmp23;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp538.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp538.f2)->tag == 3U){_LL1: _LL2:
# 2052
 return({struct Cyc_List_List*_tmp53A=_region_malloc(r,sizeof(*_tmp53A));_tmp53A->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp907=({struct Cyc_List_List*_tmp539=_region_malloc(r,sizeof(*_tmp539));_tmp539->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp906=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp539->tl=_tmp906;});_tmp539;});_tmp53A->tl=_tmp907;});_tmp53A;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp53B=_region_malloc(r,sizeof(*_tmp53B));_tmp53B->hd=atts,_tmp53B->tl=tms;_tmp53B;});}_LL0:;}else{
# 2055
return({struct Cyc_List_List*_tmp53C=_region_malloc(r,sizeof(*_tmp53C));_tmp53C->hd=atts,_tmp53C->tl=tms;_tmp53C;});}}
# 2058
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp53D=({struct _fat_ptr _tmp908=({const char*_tmp543="`";_tag_fat(_tmp543,sizeof(char),2U);});Cyc_strconcat(_tmp908,(struct _fat_ptr)*t->name);});struct _fat_ptr s=_tmp53D;
({struct _fat_ptr _tmp53E=_fat_ptr_plus(s,sizeof(char),1);char _tmp53F=*((char*)_check_fat_subscript(_tmp53E,sizeof(char),0U));char _tmp540='t';if(_get_fat_size(_tmp53E,sizeof(char))== 1U &&(_tmp53F == 0 && _tmp540 != 0))_throw_arraybounds();*((char*)_tmp53E.curr)=_tmp540;});
({struct _fat_ptr*_tmp909=({unsigned _tmp542=1;struct _fat_ptr*_tmp541=_cycalloc(_check_times(_tmp542,sizeof(struct _fat_ptr)));_tmp541[0]=(struct _fat_ptr)s;_tmp541;});t->name=_tmp909;});}}
# 2067
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2069
void*_tmp544=t;void*_tmp548;struct Cyc_Absyn_Typedefdecl*_tmp547;struct Cyc_List_List*_tmp546;struct _tuple0*_tmp545;int _tmp54B;void*_tmp54A;struct Cyc_Core_Opt*_tmp549;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp553;struct Cyc_Absyn_VarargInfo*_tmp552;int _tmp551;struct Cyc_List_List*_tmp550;void*_tmp54F;struct Cyc_Absyn_Tqual _tmp54E;void*_tmp54D;struct Cyc_List_List*_tmp54C;struct Cyc_Absyn_PtrAtts _tmp559;struct Cyc_Absyn_Tqual _tmp558;void*_tmp557;unsigned _tmp55E;void*_tmp55D;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Tqual _tmp55B;void*_tmp55A;switch(*((int*)_tmp544)){case 4U: _LL1: _tmp55A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp544)->f1).elt_type;_tmp55B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp544)->f1).tq;_tmp55C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp544)->f1).num_elts;_tmp55D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp544)->f1).zero_term;_tmp55E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp544)->f1).zt_loc;_LL2: {void*t2=_tmp55A;struct Cyc_Absyn_Tqual tq2=_tmp55B;struct Cyc_Absyn_Exp*e=_tmp55C;void*zeroterm=_tmp55D;unsigned ztl=_tmp55E;
# 2072
struct _tuple13 _tmp55F=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp24=_tmp55F;struct _tuple13 _tmp560=_stmttmp24;struct Cyc_List_List*_tmp563;void*_tmp562;struct Cyc_Absyn_Tqual _tmp561;_LLE: _tmp561=_tmp560.f1;_tmp562=_tmp560.f2;_tmp563=_tmp560.f3;_LLF: {struct Cyc_Absyn_Tqual tq3=_tmp561;void*t3=_tmp562;struct Cyc_List_List*tml3=_tmp563;
void*tm;
if(e == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp564=_region_malloc(r,sizeof(*_tmp564));_tmp564->tag=0U,_tmp564->f1=zeroterm,_tmp564->f2=ztl;_tmp564;});else{
# 2077
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp565=_region_malloc(r,sizeof(*_tmp565));_tmp565->tag=1U,_tmp565->f1=e,_tmp565->f2=zeroterm,_tmp565->f3=ztl;_tmp565;});}
return({struct _tuple13 _tmp5E0;_tmp5E0.f1=tq3,_tmp5E0.f2=t3,({struct Cyc_List_List*_tmp90A=({struct Cyc_List_List*_tmp566=_region_malloc(r,sizeof(*_tmp566));_tmp566->hd=tm,_tmp566->tl=tml3;_tmp566;});_tmp5E0.f3=_tmp90A;});_tmp5E0;});}}case 3U: _LL3: _tmp557=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544)->f1).elt_type;_tmp558=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544)->f1).elt_tq;_tmp559=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544)->f1).ptr_atts;_LL4: {void*t2=_tmp557;struct Cyc_Absyn_Tqual tq2=_tmp558;struct Cyc_Absyn_PtrAtts ptratts=_tmp559;
# 2081
struct _tuple13 _tmp567=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp25=_tmp567;struct _tuple13 _tmp568=_stmttmp25;struct Cyc_List_List*_tmp56B;void*_tmp56A;struct Cyc_Absyn_Tqual _tmp569;_LL11: _tmp569=_tmp568.f1;_tmp56A=_tmp568.f2;_tmp56B=_tmp568.f3;_LL12: {struct Cyc_Absyn_Tqual tq3=_tmp569;void*t3=_tmp56A;struct Cyc_List_List*tml3=_tmp56B;
tml3=({struct Cyc_List_List*_tmp56D=_region_malloc(r,sizeof(*_tmp56D));({void*_tmp90B=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp56C=_region_malloc(r,sizeof(*_tmp56C));_tmp56C->tag=2U,_tmp56C->f1=ptratts,_tmp56C->f2=tq;_tmp56C;});_tmp56D->hd=_tmp90B;}),_tmp56D->tl=tml3;_tmp56D;});
return({struct _tuple13 _tmp5E1;_tmp5E1.f1=tq3,_tmp5E1.f2=t3,_tmp5E1.f3=tml3;_tmp5E1;});}}case 5U: _LL5: _tmp54C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).tvars;_tmp54D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).effect;_tmp54E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).ret_tqual;_tmp54F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).ret_type;_tmp550=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).args;_tmp551=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).c_varargs;_tmp552=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).cyc_varargs;_tmp553=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).rgn_po;_tmp554=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).attributes;_tmp555=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).requires_clause;_tmp556=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544)->f1).ensures_clause;_LL6: {struct Cyc_List_List*typvars=_tmp54C;void*effopt=_tmp54D;struct Cyc_Absyn_Tqual t2qual=_tmp54E;void*t2=_tmp54F;struct Cyc_List_List*args=_tmp550;int c_varargs=_tmp551;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp552;struct Cyc_List_List*rgn_po=_tmp553;struct Cyc_List_List*fn_atts=_tmp554;struct Cyc_Absyn_Exp*req=_tmp555;struct Cyc_Absyn_Exp*ens=_tmp556;
# 2087
if(!Cyc_Absynpp_print_all_tvars){
# 2091
if(effopt == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2096
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2099
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2104
struct _tuple13 _tmp56E=Cyc_Absynpp_to_tms(r,t2qual,t2);struct _tuple13 _stmttmp26=_tmp56E;struct _tuple13 _tmp56F=_stmttmp26;struct Cyc_List_List*_tmp572;void*_tmp571;struct Cyc_Absyn_Tqual _tmp570;_LL14: _tmp570=_tmp56F.f1;_tmp571=_tmp56F.f2;_tmp572=_tmp56F.f3;_LL15: {struct Cyc_Absyn_Tqual tq3=_tmp570;void*t3=_tmp571;struct Cyc_List_List*tml3=_tmp572;
struct Cyc_List_List*tms=tml3;
# 2115 "absynpp.cyc"
{enum Cyc_Flags_C_Compilers _tmp573=Cyc_Flags_c_compiler;if(_tmp573 == Cyc_Flags_Gcc_c){_LL17: _LL18:
# 2117
 if(fn_atts != 0)
# 2120
tms=({struct _RegionHandle*_tmp90D=r;void*_tmp90C=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp574=_region_malloc(r,sizeof(*_tmp574));_tmp574->tag=5U,_tmp574->f1=0U,_tmp574->f2=fn_atts;_tmp574;});Cyc_Absynpp_bubble_attributes(_tmp90D,_tmp90C,tms);});
# 2122
tms=({struct Cyc_List_List*_tmp577=_region_malloc(r,sizeof(*_tmp577));({void*_tmp90F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp576=_region_malloc(r,sizeof(*_tmp576));
_tmp576->tag=3U,({void*_tmp90E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp575=_region_malloc(r,sizeof(*_tmp575));_tmp575->tag=1U,_tmp575->f1=args,_tmp575->f2=c_varargs,_tmp575->f3=cyc_varargs,_tmp575->f4=effopt,_tmp575->f5=rgn_po,_tmp575->f6=req,_tmp575->f7=ens;_tmp575;});_tmp576->f1=_tmp90E;});_tmp576;});
# 2122
_tmp577->hd=_tmp90F;}),_tmp577->tl=tms;_tmp577;});
# 2126
goto _LL16;}else{_LL19: _LL1A:
# 2128
 tms=({struct Cyc_List_List*_tmp57A=_region_malloc(r,sizeof(*_tmp57A));({void*_tmp911=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp579=_region_malloc(r,sizeof(*_tmp579));
_tmp579->tag=3U,({void*_tmp910=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp578=_region_malloc(r,sizeof(*_tmp578));_tmp578->tag=1U,_tmp578->f1=args,_tmp578->f2=c_varargs,_tmp578->f3=cyc_varargs,_tmp578->f4=effopt,_tmp578->f5=rgn_po,_tmp578->f6=req,_tmp578->f7=ens;_tmp578;});_tmp579->f1=_tmp910;});_tmp579;});
# 2128
_tmp57A->hd=_tmp911;}),_tmp57A->tl=tms;_tmp57A;});
# 2132
for(0;fn_atts != 0;fn_atts=fn_atts->tl){
void*_tmp57B=(void*)fn_atts->hd;void*_stmttmp27=_tmp57B;void*_tmp57C=_stmttmp27;switch(*((int*)_tmp57C)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2137
 tms=({struct Cyc_List_List*_tmp57F=_region_malloc(r,sizeof(*_tmp57F));({void*_tmp913=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp57E=_region_malloc(r,sizeof(*_tmp57E));_tmp57E->tag=5U,_tmp57E->f1=0U,({struct Cyc_List_List*_tmp912=({struct Cyc_List_List*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->hd=(void*)fn_atts->hd,_tmp57D->tl=0;_tmp57D;});_tmp57E->f2=_tmp912;});_tmp57E;});_tmp57F->hd=_tmp913;}),_tmp57F->tl=tms;_tmp57F;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2141
goto _LL16;}_LL16:;}
# 2144
AfterAtts:
 if(typvars != 0)
tms=({struct Cyc_List_List*_tmp581=_region_malloc(r,sizeof(*_tmp581));({void*_tmp914=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp580=_region_malloc(r,sizeof(*_tmp580));_tmp580->tag=4U,_tmp580->f1=typvars,_tmp580->f2=0U,_tmp580->f3=1;_tmp580;});_tmp581->hd=_tmp914;}),_tmp581->tl=tms;_tmp581;});
return({struct _tuple13 _tmp5E2;_tmp5E2.f1=tq3,_tmp5E2.f2=t3,_tmp5E2.f3=tms;_tmp5E2;});}}}case 1U: _LL7: _tmp549=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp544)->f1;_tmp54A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp544)->f2;_tmp54B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp544)->f3;_LL8: {struct Cyc_Core_Opt*k=_tmp549;void*topt=_tmp54A;int i=_tmp54B;
# 2150
if(topt == 0)
return({struct _tuple13 _tmp5E3;_tmp5E3.f1=tq,_tmp5E3.f2=t,_tmp5E3.f3=0;_tmp5E3;});else{
# 2153
return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8U: _LL9: _tmp545=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp544)->f1;_tmp546=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp544)->f2;_tmp547=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp544)->f3;_tmp548=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp544)->f4;_LLA: {struct _tuple0*n=_tmp545;struct Cyc_List_List*ts=_tmp546;struct Cyc_Absyn_Typedefdecl*td=_tmp547;void*topt=_tmp548;
# 2159
if(topt == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple13 _tmp5E4;_tmp5E4.f1=tq,_tmp5E4.f2=t,_tmp5E4.f3=0;_tmp5E4;});else{
# 2162
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}}default: _LLB: _LLC:
# 2167
 return({struct _tuple13 _tmp5E5;_tmp5E5.f1=tq,_tmp5E5.f2=t,_tmp5E5.f3=0;_tmp5E5;});}_LL0:;}
# 2171
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2173
void*_tmp582=t;void*_tmp583;void*_tmp584;switch(*((int*)_tmp582)){case 1U: _LL1: _tmp584=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp582)->f2;if(_tmp584 != 0){_LL2: {void*def=_tmp584;
return Cyc_Absynpp_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp583=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp582)->f1).elt_type;_LL4: {void*elt_typ=_tmp583;
# 2176
L: {
void*_tmp585=elt_typ;void*_tmp586;void*_tmp587;switch(*((int*)_tmp585)){case 1U: _LL8: _tmp587=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp585)->f2;if(_tmp587 != 0){_LL9: {void*t=_tmp587;
elt_typ=t;goto L;}}else{goto _LLE;}case 8U: _LLA: _tmp586=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp585)->f4;if(_tmp586 != 0){_LLB: {void*t=_tmp586;
elt_typ=t;goto L;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp585)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp585)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 2183
 return 0;}_LL0:;}
# 2187
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp588=_new_region("temp");struct _RegionHandle*temp=& _tmp588;_push_region(temp);
{struct _tuple13 _tmp589=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple13 _stmttmp28=_tmp589;struct _tuple13 _tmp58A=_stmttmp28;struct Cyc_List_List*_tmp58D;void*_tmp58C;struct Cyc_Absyn_Tqual _tmp58B;_LL1: _tmp58B=_tmp58A.f1;_tmp58C=_tmp58A.f2;_tmp58D=_tmp58A.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp58B;void*t=_tmp58C;struct Cyc_List_List*tms=_tmp58D;
tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);
if(tms == 0 && dopt == 0){
struct Cyc_PP_Doc*_tmp58F=({struct Cyc_PP_Doc*_tmp58E[2U];({struct Cyc_PP_Doc*_tmp916=Cyc_Absynpp_tqual2doc(tq);_tmp58E[0]=_tmp916;}),({struct Cyc_PP_Doc*_tmp915=Cyc_Absynpp_ntyp2doc(t);_tmp58E[1]=_tmp915;});Cyc_PP_cat(_tag_fat(_tmp58E,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp58F;}else{
# 2194
struct Cyc_PP_Doc*_tmp592=({struct Cyc_PP_Doc*_tmp590[4U];({
struct Cyc_PP_Doc*_tmp91C=Cyc_Absynpp_tqual2doc(tq);_tmp590[0]=_tmp91C;}),({
struct Cyc_PP_Doc*_tmp91B=Cyc_Absynpp_ntyp2doc(t);_tmp590[1]=_tmp91B;}),({
struct Cyc_PP_Doc*_tmp91A=Cyc_PP_text(({const char*_tmp591=" ";_tag_fat(_tmp591,sizeof(char),2U);}));_tmp590[2]=_tmp91A;}),({
struct Cyc_PP_Doc*_tmp919=({int _tmp918=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp917=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp918,_tmp917,tms);});_tmp590[3]=_tmp919;});Cyc_PP_cat(_tag_fat(_tmp590,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp592;}}}
# 2189
;_pop_region();}
# 2202
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp593=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp593;
if((unsigned)req)
requires_doc=({struct Cyc_PP_Doc*_tmp594[2U];({struct Cyc_PP_Doc*_tmp91E=Cyc_PP_text(({const char*_tmp595="@requires ";_tag_fat(_tmp595,sizeof(char),11U);}));_tmp594[0]=_tmp91E;}),({struct Cyc_PP_Doc*_tmp91D=Cyc_Absynpp_exp2doc(req);_tmp594[1]=_tmp91D;});Cyc_PP_cat(_tag_fat(_tmp594,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2208
requires_doc=Cyc_PP_nil_doc();}{
# 2210
enum Cyc_Flags_C_Compilers _tmp596=Cyc_Flags_c_compiler;if(_tmp596 == Cyc_Flags_Gcc_c){_LL1: _LL2:
# 2213
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp597[5U];({struct Cyc_PP_Doc*_tmp926=({struct Cyc_Absyn_Tqual _tmp925=f->tq;void*_tmp924=f->type;Cyc_Absynpp_tqtd2doc(_tmp925,_tmp924,({struct Cyc_Core_Opt*_tmp598=_cycalloc(sizeof(*_tmp598));({struct Cyc_PP_Doc*_tmp923=Cyc_PP_textptr(f->name);_tmp598->v=_tmp923;});_tmp598;}));});_tmp597[0]=_tmp926;}),({
struct Cyc_PP_Doc*_tmp922=Cyc_PP_text(({const char*_tmp599=":";_tag_fat(_tmp599,sizeof(char),2U);}));_tmp597[1]=_tmp922;}),({struct Cyc_PP_Doc*_tmp921=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp597[2]=_tmp921;}),({
struct Cyc_PP_Doc*_tmp920=Cyc_Absynpp_atts2doc(f->attributes);_tmp597[3]=_tmp920;}),({struct Cyc_PP_Doc*_tmp91F=Cyc_PP_text(({const char*_tmp59A=";";_tag_fat(_tmp59A,sizeof(char),2U);}));_tmp597[4]=_tmp91F;});Cyc_PP_cat(_tag_fat(_tmp597,sizeof(struct Cyc_PP_Doc*),5U));});
return({struct Cyc_PP_Doc*_tmp59B[4U];({struct Cyc_PP_Doc*_tmp92C=({struct Cyc_Absyn_Tqual _tmp92B=f->tq;void*_tmp92A=f->type;Cyc_Absynpp_tqtd2doc(_tmp92B,_tmp92A,({struct Cyc_Core_Opt*_tmp59C=_cycalloc(sizeof(*_tmp59C));({struct Cyc_PP_Doc*_tmp929=Cyc_PP_textptr(f->name);_tmp59C->v=_tmp929;});_tmp59C;}));});_tmp59B[0]=_tmp92C;}),({
struct Cyc_PP_Doc*_tmp928=Cyc_Absynpp_atts2doc(f->attributes);_tmp59B[1]=_tmp928;}),_tmp59B[2]=requires_doc,({struct Cyc_PP_Doc*_tmp927=Cyc_PP_text(({const char*_tmp59D=";";_tag_fat(_tmp59D,sizeof(char),2U);}));_tmp59B[3]=_tmp927;});Cyc_PP_cat(_tag_fat(_tmp59B,sizeof(struct Cyc_PP_Doc*),4U));});}else{_LL3: _LL4:
# 2220
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp59E[5U];({struct Cyc_PP_Doc*_tmp934=Cyc_Absynpp_atts2doc(f->attributes);_tmp59E[0]=_tmp934;}),({
struct Cyc_PP_Doc*_tmp933=({struct Cyc_Absyn_Tqual _tmp932=f->tq;void*_tmp931=f->type;Cyc_Absynpp_tqtd2doc(_tmp932,_tmp931,({struct Cyc_Core_Opt*_tmp59F=_cycalloc(sizeof(*_tmp59F));({struct Cyc_PP_Doc*_tmp930=Cyc_PP_textptr(f->name);_tmp59F->v=_tmp930;});_tmp59F;}));});_tmp59E[1]=_tmp933;}),({
struct Cyc_PP_Doc*_tmp92F=Cyc_PP_text(({const char*_tmp5A0=":";_tag_fat(_tmp5A0,sizeof(char),2U);}));_tmp59E[2]=_tmp92F;}),({struct Cyc_PP_Doc*_tmp92E=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp59E[3]=_tmp92E;}),({struct Cyc_PP_Doc*_tmp92D=Cyc_PP_text(({const char*_tmp5A1=";";_tag_fat(_tmp5A1,sizeof(char),2U);}));_tmp59E[4]=_tmp92D;});Cyc_PP_cat(_tag_fat(_tmp59E,sizeof(struct Cyc_PP_Doc*),5U));});
return({struct Cyc_PP_Doc*_tmp5A2[4U];({struct Cyc_PP_Doc*_tmp93A=Cyc_Absynpp_atts2doc(f->attributes);_tmp5A2[0]=_tmp93A;}),({
struct Cyc_PP_Doc*_tmp939=({struct Cyc_Absyn_Tqual _tmp938=f->tq;void*_tmp937=f->type;Cyc_Absynpp_tqtd2doc(_tmp938,_tmp937,({struct Cyc_Core_Opt*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));({struct Cyc_PP_Doc*_tmp936=Cyc_PP_textptr(f->name);_tmp5A3->v=_tmp936;});_tmp5A3;}));});_tmp5A2[1]=_tmp939;}),_tmp5A2[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp935=Cyc_PP_text(({const char*_tmp5A4=";";_tag_fat(_tmp5A4,sizeof(char),2U);}));_tmp5A2[3]=_tmp935;});Cyc_PP_cat(_tag_fat(_tmp5A2,sizeof(struct Cyc_PP_Doc*),4U));});}_LL0:;}}
# 2231
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp93B=({const char*_tmp5A5="";_tag_fat(_tmp5A5,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp93B,fields);});}
# 2235
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp5A6[3U];({struct Cyc_PP_Doc*_tmp93F=Cyc_Absynpp_scope2doc(f->sc);_tmp5A6[0]=_tmp93F;}),({struct Cyc_PP_Doc*_tmp93E=Cyc_Absynpp_typedef_name2doc(f->name);_tmp5A6[1]=_tmp93E;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp93D=Cyc_PP_nil_doc();_tmp5A6[2]=_tmp93D;}):({struct Cyc_PP_Doc*_tmp93C=Cyc_Absynpp_args2doc(f->typs);_tmp5A6[2]=_tmp93C;});Cyc_PP_cat(_tag_fat(_tmp5A6,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2240
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp940=({const char*_tmp5A7=",";_tag_fat(_tmp5A7,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp940,fields);});}
# 2249
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp941=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp941,72,f);});
({void*_tmp5A8=0U;({struct Cyc___cycFILE*_tmp943=f;struct _fat_ptr _tmp942=({const char*_tmp5A9="\n";_tag_fat(_tmp5A9,sizeof(char),2U);});Cyc_fprintf(_tmp943,_tmp942,_tag_fat(_tmp5A8,sizeof(void*),0U));});});}}
# 2256
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _tmp944=({const char*_tmp5AA="";_tag_fat(_tmp5AA,sizeof(char),1U);});Cyc_PP_seql(_tmp944,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2259
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
# 2273
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
# 2278
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*t){
if(t == 0)
return(struct _fat_ptr)({const char*_tmp5AB="NULL";_tag_fat(_tmp5AB,sizeof(char),5U);});{
void*_tmp5AC=t;struct Cyc_Absyn_Typedefdecl*_tmp5AD;switch(*((int*)_tmp5AC)){case 0U: _LL1: _LL2:
 return(struct _fat_ptr)({const char*_tmp5AE="AppType";_tag_fat(_tmp5AE,sizeof(char),8U);});case 1U: _LL3: _LL4:
 return(struct _fat_ptr)({const char*_tmp5AF="Evar";_tag_fat(_tmp5AF,sizeof(char),5U);});case 2U: _LL5: _LL6:
 return(struct _fat_ptr)({const char*_tmp5B0="Vartype";_tag_fat(_tmp5B0,sizeof(char),8U);});case 3U: _LL7: _LL8:
 return(struct _fat_ptr)({const char*_tmp5B1="Pointertype";_tag_fat(_tmp5B1,sizeof(char),12U);});case 4U: _LL9: _LLA:
 return(struct _fat_ptr)({const char*_tmp5B2="ArrayType";_tag_fat(_tmp5B2,sizeof(char),10U);});case 5U: _LLB: _LLC:
 return(struct _fat_ptr)({const char*_tmp5B3="FnType";_tag_fat(_tmp5B3,sizeof(char),7U);});case 6U: _LLD: _LLE:
 return(struct _fat_ptr)({const char*_tmp5B4="TupleType";_tag_fat(_tmp5B4,sizeof(char),10U);});case 7U: _LLF: _LL10:
 return(struct _fat_ptr)({const char*_tmp5B5="AnonAggrType";_tag_fat(_tmp5B5,sizeof(char),13U);});case 8U: _LL11: _tmp5AD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5AC)->f3;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp5AD;
# 2291
return(struct _fat_ptr)({const char*_tmp5B6="Typedeftype";_tag_fat(_tmp5B6,sizeof(char),12U);});}case 9U: _LL13: _LL14:
 return(struct _fat_ptr)({const char*_tmp5B7="Valueoftype";_tag_fat(_tmp5B7,sizeof(char),12U);});case 10U: _LL15: _LL16:
 return(struct _fat_ptr)({const char*_tmp5B8="Typedecltype";_tag_fat(_tmp5B8,sizeof(char),13U);});default: _LL17: _LL18:
 return(struct _fat_ptr)({const char*_tmp5B9="Typeoftype";_tag_fat(_tmp5B9,sizeof(char),11U);});}_LL0:;}}

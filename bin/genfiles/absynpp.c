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
struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Buffer_t;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 258
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 62
struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 66
struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 693 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 876
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 885
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 890
void*Cyc_Absyn_compress_kb(void*);
# 896
int Cyc_Absyn_type2bool(int def,void*);
# 901
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1132
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1142
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1150
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 107 "tcutil.h"
void*Cyc_Tcutil_compress(void*);
# 263
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _fat_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _fat_ptr s,int w);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*l);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 52 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 62
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 77
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _fat_ptr Cyc_Absynpp_cyc_string;
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple14{int f1;struct Cyc_List_List*f2;};
# 94 "absynpp.h"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 42 "absynpp.cyc"
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
# 63
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 70
static int Cyc_Absynpp_expand_typedefs;
# 74
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4U]="Cyc";
# 76
struct _fat_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4U};
struct _fat_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 79
static int Cyc_Absynpp_add_cyc_prefix;
# 83
static int Cyc_Absynpp_to_VC;
# 86
static int Cyc_Absynpp_decls_first;
# 90
static int Cyc_Absynpp_rewrite_temp_tvars;
# 93
static int Cyc_Absynpp_print_all_tvars;
# 96
static int Cyc_Absynpp_print_all_kinds;
# 99
static int Cyc_Absynpp_print_all_effects;
# 102
static int Cyc_Absynpp_print_using_stmts;
# 107
static int Cyc_Absynpp_print_externC_stmts;
# 111
static int Cyc_Absynpp_print_full_evars;
# 114
static int Cyc_Absynpp_generate_line_directives;
# 117
static int Cyc_Absynpp_use_curr_namespace;
# 120
static int Cyc_Absynpp_print_zeroterm;
# 123
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 126
int Cyc_Absynpp_print_for_cycdoc=0;
# 147
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
# 166
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 186
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 206
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 226
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 247
static void Cyc_Absynpp_curr_namespace_add(struct _fat_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp58F=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp58F,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v,_tmp1->tl=0;_tmp1;}));});}
# 251
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 255
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 259
static void Cyc_Absynpp_curr_namespace_drop(){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 263
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
# 276
 if((int)c >= (int)' ' &&(int)c <= (int)'~'){
struct _fat_ptr _tmpD=Cyc_Core_new_string(2U);struct _fat_ptr t=_tmpD;
({struct _fat_ptr _tmpE=_fat_ptr_plus(t,sizeof(char),0);char _tmpF=*((char*)_check_fat_subscript(_tmpE,sizeof(char),0U));char _tmp10=c;if(_get_fat_size(_tmpE,sizeof(char))== 1U &&(_tmpF == 0 && _tmp10 != 0))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _fat_ptr)t;}else{
# 281
struct _fat_ptr _tmp11=Cyc_Core_new_string(5U);struct _fat_ptr t=_tmp11;
int j=0;
({struct _fat_ptr _tmp12=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp13=*((char*)_check_fat_subscript(_tmp12,sizeof(char),0U));char _tmp14='\\';if(_get_fat_size(_tmp12,sizeof(char))== 1U &&(_tmp13 == 0 && _tmp14 != 0))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _fat_ptr _tmp15=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp16=*((char*)_check_fat_subscript(_tmp15,sizeof(char),0U));char _tmp17=(char)((int)'0' + ((int)((unsigned char)c)>> 6 & 3));if(_get_fat_size(_tmp15,sizeof(char))== 1U &&(_tmp16 == 0 && _tmp17 != 0))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _fat_ptr _tmp18=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp19=*((char*)_check_fat_subscript(_tmp18,sizeof(char),0U));char _tmp1A=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_fat_size(_tmp18,sizeof(char))== 1U &&(_tmp19 == 0 && _tmp1A != 0))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _fat_ptr _tmp1B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp1C=*((char*)_check_fat_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=(char)((int)'0' + ((int)c & 7));if(_get_fat_size(_tmp1B,sizeof(char))== 1U &&(_tmp1C == 0 && _tmp1D != 0))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _fat_ptr)t;}}_LL0:;}
# 292
static int Cyc_Absynpp_special(struct _fat_ptr s){
int sz=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((((int)c <= (int)' ' ||(int)c >= (int)'~')||(int)c == (int)'"')||(int)c == (int)'\\')
return 1;}}
# 299
return 0;}
# 302
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 305
int n=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
# 307
if(n > 0 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),n))== (int)'\000')-- n;{
# 309
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
# 322
if((int)c >= (int)' ' &&(int)c <= (int)'~')++ len;else{
len +=4;}
goto _LL0;}}_LL0:;}}{
# 327
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
# 341
if((int)c >= (int)' ' &&(int)c <= (int)'~')({struct _fat_ptr _tmp5A=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5B=*((char*)_check_fat_subscript(_tmp5A,sizeof(char),0U));char _tmp5C=c;if(_get_fat_size(_tmp5A,sizeof(char))== 1U &&(_tmp5B == 0 && _tmp5C != 0))_throw_arraybounds();*((char*)_tmp5A.curr)=_tmp5C;});else{
# 343
({struct _fat_ptr _tmp5D=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5E=*((char*)_check_fat_subscript(_tmp5D,sizeof(char),0U));char _tmp5F='\\';if(_get_fat_size(_tmp5D,sizeof(char))== 1U &&(_tmp5E == 0 && _tmp5F != 0))_throw_arraybounds();*((char*)_tmp5D.curr)=_tmp5F;});
({struct _fat_ptr _tmp60=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp61=*((char*)_check_fat_subscript(_tmp60,sizeof(char),0U));char _tmp62=(char)((int)'0' + ((int)c >> 6 & 7));if(_get_fat_size(_tmp60,sizeof(char))== 1U &&(_tmp61 == 0 && _tmp62 != 0))_throw_arraybounds();*((char*)_tmp60.curr)=_tmp62;});
({struct _fat_ptr _tmp63=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp64=*((char*)_check_fat_subscript(_tmp63,sizeof(char),0U));char _tmp65=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_fat_size(_tmp63,sizeof(char))== 1U &&(_tmp64 == 0 && _tmp65 != 0))_throw_arraybounds();*((char*)_tmp63.curr)=_tmp65;});
({struct _fat_ptr _tmp66=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp67=*((char*)_check_fat_subscript(_tmp66,sizeof(char),0U));char _tmp68=(char)((int)'0' + ((int)c & 7));if(_get_fat_size(_tmp66,sizeof(char))== 1U &&(_tmp67 == 0 && _tmp68 != 0))_throw_arraybounds();*((char*)_tmp66.curr)=_tmp68;});}
# 348
goto _LL15;}}_LL15:;}}
# 350
return(struct _fat_ptr)t;}}}}static char _tmp69[9U]="restrict";
# 353
static struct _fat_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9U};static char _tmp6A[9U]="volatile";
static struct _fat_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9U};static char _tmp6B[6U]="const";
static struct _fat_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6U};
static struct _fat_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _fat_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _fat_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 360
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 363
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp,_tmp6C->tl=l;_tmp6C;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp,_tmp6D->tl=l;_tmp6D;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp,_tmp6E->tl=l;_tmp6E;});
return({struct _fat_ptr _tmp592=({const char*_tmp6F="";_tag_fat(_tmp6F,sizeof(char),1U);});struct _fat_ptr _tmp591=({const char*_tmp70=" ";_tag_fat(_tmp70,sizeof(char),2U);});struct _fat_ptr _tmp590=({const char*_tmp71=" ";_tag_fat(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp592,_tmp591,_tmp590,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 369
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_AliasQual _tmp74;enum Cyc_Absyn_KindQual _tmp73;_LL1: _tmp73=_tmp72->kind;_tmp74=_tmp72->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp73;enum Cyc_Absyn_AliasQual a=_tmp74;
enum Cyc_Absyn_KindQual _tmp75=k;switch(_tmp75){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 373
enum Cyc_Absyn_AliasQual _tmp76=a;switch(_tmp76){case Cyc_Absyn_Aliasable: _LL17: _LL18:
 return({const char*_tmp77="A";_tag_fat(_tmp77,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL19: _LL1A:
 return({const char*_tmp78="UA";_tag_fat(_tmp78,sizeof(char),3U);});case Cyc_Absyn_Top: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return({const char*_tmp79="TA";_tag_fat(_tmp79,sizeof(char),3U);});}_LL16:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 379
enum Cyc_Absyn_AliasQual _tmp7A=a;switch(_tmp7A){case Cyc_Absyn_Aliasable: _LL20: _LL21:
 return({const char*_tmp7B="M";_tag_fat(_tmp7B,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL22: _LL23:
 return({const char*_tmp7C="UM";_tag_fat(_tmp7C,sizeof(char),3U);});case Cyc_Absyn_Top: _LL24: _LL25:
 goto _LL27;default: _LL26: _LL27: return({const char*_tmp7D="TM";_tag_fat(_tmp7D,sizeof(char),3U);});}_LL1F:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 385
enum Cyc_Absyn_AliasQual _tmp7E=a;switch(_tmp7E){case Cyc_Absyn_Aliasable: _LL29: _LL2A:
 return({const char*_tmp7F="B";_tag_fat(_tmp7F,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL2B: _LL2C:
 return({const char*_tmp80="UB";_tag_fat(_tmp80,sizeof(char),3U);});case Cyc_Absyn_Top: _LL2D: _LL2E:
 goto _LL30;default: _LL2F: _LL30: return({const char*_tmp81="TB";_tag_fat(_tmp81,sizeof(char),3U);});}_LL28:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 391
enum Cyc_Absyn_AliasQual _tmp82=a;switch(_tmp82){case Cyc_Absyn_Aliasable: _LL32: _LL33:
 return({const char*_tmp83="R";_tag_fat(_tmp83,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL34: _LL35:
 return({const char*_tmp84="UR";_tag_fat(_tmp84,sizeof(char),3U);});case Cyc_Absyn_Top: _LL36: _LL37:
 goto _LL39;default: _LL38: _LL39: return({const char*_tmp85="TR";_tag_fat(_tmp85,sizeof(char),3U);});}_LL31:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 396
 return({const char*_tmp86="E";_tag_fat(_tmp86,sizeof(char),2U);});case Cyc_Absyn_IntKind: _LLE: _LLF:
 return({const char*_tmp87="I";_tag_fat(_tmp87,sizeof(char),2U);});case Cyc_Absyn_BoolKind: _LL10: _LL11:
 return({const char*_tmp88="BOOL";_tag_fat(_tmp88,sizeof(char),5U);});case Cyc_Absyn_PtrBndKind: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15: return({const char*_tmp89="PTRBND";_tag_fat(_tmp89,sizeof(char),7U);});}_LL3:;}}
# 402
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 404
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp8A=Cyc_Absyn_compress_kb(c);void*_stmttmp2=_tmp8A;void*_tmp8B=_stmttmp2;struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8D;switch(*((int*)_tmp8B)){case 0U: _LL1: _tmp8D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp8D;
return Cyc_Absynpp_kind2string(k);}case 1U: _LL3: _LL4:
# 408
 if(Cyc_PP_tex_output)
return({const char*_tmp8E="{?}";_tag_fat(_tmp8E,sizeof(char),4U);});else{
return({const char*_tmp8F="?";_tag_fat(_tmp8F,sizeof(char),2U);});}default: _LL5: _tmp8C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp8C;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp92=({struct Cyc_String_pa_PrintArg_struct _tmp570;_tmp570.tag=0U,({struct _fat_ptr _tmp593=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k));_tmp570.f1=_tmp593;});_tmp570;});void*_tmp90[1U];_tmp90[0]=& _tmp92;({struct _fat_ptr _tmp594=({const char*_tmp91="<=%s";_tag_fat(_tmp91,sizeof(char),5U);});Cyc_aprintf(_tmp594,_tag_fat(_tmp90,sizeof(void*),1U));});});}}_LL0:;}
# 415
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp93=Cyc_Absyn_compress_kb(c);void*_stmttmp3=_tmp93;void*_tmp94=_stmttmp3;struct Cyc_Absyn_Kind*_tmp95;struct Cyc_Absyn_Kind*_tmp96;switch(*((int*)_tmp94)){case 0U: _LL1: _tmp96=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp96;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1U: _LL3: _LL4:
# 419
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp97="{?}";_tag_fat(_tmp97,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp98="?";_tag_fat(_tmp98,sizeof(char),2U);}));}default: _LL5: _tmp95=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp95;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}}_LL0:;}
# 426
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp597=({const char*_tmp99="<";_tag_fat(_tmp99,sizeof(char),2U);});struct _fat_ptr _tmp596=({const char*_tmp9A=">";_tag_fat(_tmp9A,sizeof(char),2U);});struct _fat_ptr _tmp595=({const char*_tmp9B=",";_tag_fat(_tmp9B,sizeof(char),2U);});Cyc_PP_egroup(_tmp597,_tmp596,_tmp595,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 430
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr _tmp9C=*tv->name;struct _fat_ptr n=_tmp9C;
# 434
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
# 436
struct _fat_ptr kstring=({const char*_tmpA5="K";_tag_fat(_tmpA5,sizeof(char),2U);});
{void*_tmp9D=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp4=_tmp9D;void*_tmp9E=_stmttmp4;struct Cyc_Absyn_Kind*_tmp9F;struct Cyc_Absyn_Kind*_tmpA0;switch(*((int*)_tmp9E)){case 2U: _LL1: _tmpA0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f2;_LL2: {struct Cyc_Absyn_Kind*k=_tmpA0;
_tmp9F=k;goto _LL4;}case 0U: _LL3: _tmp9F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9E)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_tmp9F;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 442
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp572;_tmp572.tag=0U,_tmp572.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_tmp572;});struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp571;_tmp571.tag=0U,({struct _fat_ptr _tmp598=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(n,sizeof(char),1));_tmp571.f1=_tmp598;});_tmp571;});void*_tmpA1[2U];_tmpA1[0]=& _tmpA3,_tmpA1[1]=& _tmpA4;({struct _fat_ptr _tmp599=({const char*_tmpA2="`G%s%s";_tag_fat(_tmpA2,sizeof(char),7U);});Cyc_aprintf(_tmp599,_tag_fat(_tmpA1,sizeof(void*),2U));});}));}
# 444
return Cyc_PP_text(n);}
# 447
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp5=_tmpA6;void*_tmpA7=_stmttmp5;struct Cyc_Absyn_Kind*_tmpA8;struct Cyc_Absyn_Kind*_tmpA9;switch(*((int*)_tmpA7)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpA9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmpA9;
# 452
return({struct Cyc_PP_Doc*_tmpAA[3U];({struct Cyc_PP_Doc*_tmp59C=Cyc_Absynpp_tvar2doc(tv);_tmpAA[0]=_tmp59C;}),({struct Cyc_PP_Doc*_tmp59B=Cyc_PP_text(({const char*_tmpAB="::";_tag_fat(_tmpAB,sizeof(char),3U);}));_tmpAA[1]=_tmp59B;}),({struct Cyc_PP_Doc*_tmp59A=Cyc_Absynpp_kind2doc(k);_tmpAA[2]=_tmp59A;});Cyc_PP_cat(_tag_fat(_tmpAA,sizeof(struct Cyc_PP_Doc*),3U));});}}default: _LL5: _tmpA8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmpA8;
# 451
_tmpA9=k;goto _LL8;}}_LL0:;}
# 456
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _tmp59F=({const char*_tmpAC="<";_tag_fat(_tmpAC,sizeof(char),2U);});struct _fat_ptr _tmp59E=({const char*_tmpAD=">";_tag_fat(_tmpAD,sizeof(char),2U);});struct _fat_ptr _tmp59D=({const char*_tmpAE=",";_tag_fat(_tmpAE,sizeof(char),2U);});Cyc_PP_egroup(_tmp59F,_tmp59E,_tmp59D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _tmp5A2=({const char*_tmpAF="<";_tag_fat(_tmpAF,sizeof(char),2U);});struct _fat_ptr _tmp5A1=({const char*_tmpB0=">";_tag_fat(_tmpB0,sizeof(char),2U);});struct _fat_ptr _tmp5A0=({const char*_tmpB1=",";_tag_fat(_tmpB1,sizeof(char),2U);});Cyc_PP_egroup(_tmp5A2,_tmp5A1,_tmp5A0,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 466
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple17*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 470
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp5A5=({const char*_tmpB2="(";_tag_fat(_tmpB2,sizeof(char),2U);});struct _fat_ptr _tmp5A4=({const char*_tmpB3=")";_tag_fat(_tmpB3,sizeof(char),2U);});struct _fat_ptr _tmp5A3=({const char*_tmpB4=",";_tag_fat(_tmpB4,sizeof(char),2U);});Cyc_PP_group(_tmp5A5,_tmp5A4,_tmp5A3,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 474
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB5=att;switch(*((int*)_tmpB5)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();default: _LL7: _LL8:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL0:;}
# 483
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB6=(void*)atts->hd;void*_stmttmp6=_tmpB6;void*_tmpB7=_stmttmp6;switch(*((int*)_tmpB7)){case 1U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmpB8=" _stdcall ";_tag_fat(_tmpB8,sizeof(char),11U);}));case 2U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmpB9=" _cdecl ";_tag_fat(_tmpB9,sizeof(char),9U);}));case 3U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmpBA=" _fastcall ";_tag_fat(_tmpBA,sizeof(char),12U);}));default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 491
return Cyc_PP_nil_doc();}
# 494
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpBB=(void*)atts2->hd;void*_stmttmp7=_tmpBB;void*_tmpBC=_stmttmp7;switch(*((int*)_tmpBC)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 503
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBD[3U];({struct Cyc_PP_Doc*_tmp5AB=Cyc_PP_text(({const char*_tmpBE=" __declspec(";_tag_fat(_tmpBE,sizeof(char),13U);}));_tmpBD[0]=_tmp5AB;}),({
struct Cyc_PP_Doc*_tmp5AA=({struct _fat_ptr _tmp5A9=({const char*_tmpBF="";_tag_fat(_tmpBF,sizeof(char),1U);});struct _fat_ptr _tmp5A8=({const char*_tmpC0="";_tag_fat(_tmpC0,sizeof(char),1U);});struct _fat_ptr _tmp5A7=({const char*_tmpC1=" ";_tag_fat(_tmpC1,sizeof(char),2U);});Cyc_PP_group(_tmp5A9,_tmp5A8,_tmp5A7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBD[1]=_tmp5AA;}),({
struct Cyc_PP_Doc*_tmp5A6=Cyc_PP_text(({const char*_tmpC2=")";_tag_fat(_tmpC2,sizeof(char),2U);}));_tmpBD[2]=_tmp5A6;});Cyc_PP_cat(_tag_fat(_tmpBD,sizeof(struct Cyc_PP_Doc*),3U));});}
# 510
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 514
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC3=Cyc_Cyclone_c_compiler;if(_tmpC3 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 519
 return({struct Cyc_PP_Doc*_tmpC4[2U];({struct Cyc_PP_Doc*_tmp5B0=Cyc_PP_text(({const char*_tmpC5=" __attribute__";_tag_fat(_tmpC5,sizeof(char),15U);}));_tmpC4[0]=_tmp5B0;}),({
struct Cyc_PP_Doc*_tmp5AF=({struct _fat_ptr _tmp5AE=({const char*_tmpC6="((";_tag_fat(_tmpC6,sizeof(char),3U);});struct _fat_ptr _tmp5AD=({const char*_tmpC7="))";_tag_fat(_tmpC7,sizeof(char),3U);});struct _fat_ptr _tmp5AC=({const char*_tmpC8=",";_tag_fat(_tmpC8,sizeof(char),2U);});Cyc_PP_group(_tmp5AE,_tmp5AD,_tmp5AC,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC4[1]=_tmp5AF;});Cyc_PP_cat(_tag_fat(_tmpC4,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}}
# 524
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC9=(void*)tms->hd;void*_stmttmp8=_tmpC9;void*_tmpCA=_stmttmp8;switch(*((int*)_tmpCA)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 529
enum Cyc_Cyclone_C_Compilers _tmpCB=Cyc_Cyclone_c_compiler;if(_tmpCB == Cyc_Cyclone_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 533
 return 0;}_LL0:;}}
# 537
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 540
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmpCC="{?}";_tag_fat(_tmpCC,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmpCD="?";_tag_fat(_tmpCD,sizeof(char),2U);}));}}
# 547
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 549
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmpCE="{\\lb}";_tag_fat(_tmpCE,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmpCF="{";_tag_fat(_tmpCF,sizeof(char),2U);}));}}
# 556
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 558
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmpD0="{\\rb}";_tag_fat(_tmpD0,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmpD1="}";_tag_fat(_tmpD1,sizeof(char),2U);}));}}
# 565
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 567
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmpD2="\\$";_tag_fat(_tmpD2,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmpD3="$";_tag_fat(_tmpD3,sizeof(char),2U);}));}}
# 574
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 576
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD4[3U];({struct Cyc_PP_Doc*_tmp5B3=Cyc_Absynpp_lb();_tmpD4[0]=_tmp5B3;}),({struct Cyc_PP_Doc*_tmp5B2=Cyc_PP_seq(sep,ss);_tmpD4[1]=_tmp5B2;}),({struct Cyc_PP_Doc*_tmp5B1=Cyc_Absynpp_rb();_tmpD4[2]=_tmp5B1;});Cyc_PP_cat(_tag_fat(_tmpD4,sizeof(struct Cyc_PP_Doc*),3U));});}
# 581
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD5=(void*)tms->hd;void*_stmttmp9=_tmpD5;void*_tmpD6=_stmttmp9;struct Cyc_List_List*_tmpD7;switch(*((int*)_tmpD6)){case 0U: _LL1: _LL2:
({void*_tmpD8=0U;({struct Cyc___cycFILE*_tmp5B5=Cyc_stderr;struct _fat_ptr _tmp5B4=({const char*_tmpD9="Carray_mod ";_tag_fat(_tmpD9,sizeof(char),12U);});Cyc_fprintf(_tmp5B5,_tmp5B4,_tag_fat(_tmpD8,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpDA=0U;({struct Cyc___cycFILE*_tmp5B7=Cyc_stderr;struct _fat_ptr _tmp5B6=({const char*_tmpDB="ConstArray_mod ";_tag_fat(_tmpDB,sizeof(char),16U);});Cyc_fprintf(_tmp5B7,_tmp5B6,_tag_fat(_tmpDA,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->tag == 1U){_LL5: _tmpD7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->f1;_LL6: {struct Cyc_List_List*ys=_tmpD7;
# 587
({void*_tmpDC=0U;({struct Cyc___cycFILE*_tmp5B9=Cyc_stderr;struct _fat_ptr _tmp5B8=({const char*_tmpDD="Function_mod(";_tag_fat(_tmpDD,sizeof(char),14U);});Cyc_fprintf(_tmp5B9,_tmp5B8,_tag_fat(_tmpDC,sizeof(void*),0U));});});
for(0;ys != 0;ys=ys->tl){
struct _fat_ptr*_tmpDE=(*((struct _tuple8*)ys->hd)).f1;struct _fat_ptr*v=_tmpDE;
if(v == 0)({void*_tmpDF=0U;({struct Cyc___cycFILE*_tmp5BB=Cyc_stderr;struct _fat_ptr _tmp5BA=({const char*_tmpE0="?";_tag_fat(_tmpE0,sizeof(char),2U);});Cyc_fprintf(_tmp5BB,_tmp5BA,_tag_fat(_tmpDF,sizeof(void*),0U));});});else{
({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp5BD=Cyc_stderr;struct _fat_ptr _tmp5BC=*v;Cyc_fprintf(_tmp5BD,_tmp5BC,_tag_fat(_tmpE1,sizeof(void*),0U));});});}
if(ys->tl != 0)({void*_tmpE2=0U;({struct Cyc___cycFILE*_tmp5BF=Cyc_stderr;struct _fat_ptr _tmp5BE=({const char*_tmpE3=",";_tag_fat(_tmpE3,sizeof(char),2U);});Cyc_fprintf(_tmp5BF,_tmp5BE,_tag_fat(_tmpE2,sizeof(void*),0U));});});}
# 594
({void*_tmpE4=0U;({struct Cyc___cycFILE*_tmp5C1=Cyc_stderr;struct _fat_ptr _tmp5C0=({const char*_tmpE5=") ";_tag_fat(_tmpE5,sizeof(char),3U);});Cyc_fprintf(_tmp5C1,_tmp5C0,_tag_fat(_tmpE4,sizeof(void*),0U));});});
goto _LL0;}}else{_LL7: _LL8:
# 597
({void*_tmpE6=0U;({struct Cyc___cycFILE*_tmp5C3=Cyc_stderr;struct _fat_ptr _tmp5C2=({const char*_tmpE7="Function_mod()";_tag_fat(_tmpE7,sizeof(char),15U);});Cyc_fprintf(_tmp5C3,_tmp5C2,_tag_fat(_tmpE6,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE8=0U;({struct Cyc___cycFILE*_tmp5C5=Cyc_stderr;struct _fat_ptr _tmp5C4=({const char*_tmpE9="Attributes_mod ";_tag_fat(_tmpE9,sizeof(char),16U);});Cyc_fprintf(_tmp5C5,_tmp5C4,_tag_fat(_tmpE8,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpEA=0U;({struct Cyc___cycFILE*_tmp5C7=Cyc_stderr;struct _fat_ptr _tmp5C6=({const char*_tmpEB="TypeParams_mod ";_tag_fat(_tmpEB,sizeof(char),16U);});Cyc_fprintf(_tmp5C7,_tmp5C6,_tag_fat(_tmpEA,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpEC=0U;({struct Cyc___cycFILE*_tmp5C9=Cyc_stderr;struct _fat_ptr _tmp5C8=({const char*_tmpED="Pointer_mod ";_tag_fat(_tmpED,sizeof(char),13U);});Cyc_fprintf(_tmp5C9,_tmp5C8,_tag_fat(_tmpEC,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 602
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp5CB=Cyc_stderr;struct _fat_ptr _tmp5CA=({const char*_tmpEF="\n";_tag_fat(_tmpEF,sizeof(char),2U);});Cyc_fprintf(_tmp5CB,_tmp5CA,_tag_fat(_tmpEE,sizeof(void*),0U));});});}
# 605
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 607
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp134[3U];({struct Cyc_PP_Doc*_tmp5CD=Cyc_PP_text(({const char*_tmp135="(";_tag_fat(_tmp135,sizeof(char),2U);}));_tmp134[0]=_tmp5CD;}),_tmp134[1]=rest,({struct Cyc_PP_Doc*_tmp5CC=Cyc_PP_text(({const char*_tmp136=")";_tag_fat(_tmp136,sizeof(char),2U);}));_tmp134[2]=_tmp5CC;});Cyc_PP_cat(_tag_fat(_tmp134,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpF0=(void*)tms->hd;void*_stmttmpA=_tmpF0;void*_tmpF1=_stmttmpA;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF5;void*_tmpF4;void*_tmpF3;void*_tmpF2;int _tmpF9;unsigned _tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpFA;void*_tmpFB;void*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFE;switch(*((int*)_tmpF1)){case 0U: _LL1: _tmpFE=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL2: {void*zeroterm=_tmpFE;
# 613
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpFF[2U];_tmpFF[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_tmp5CF=Cyc_PP_text(({const char*_tmp100="[]@zeroterm";_tag_fat(_tmp100,sizeof(char),12U);}));_tmpFF[1]=_tmp5CF;}):({struct Cyc_PP_Doc*_tmp5CE=Cyc_PP_text(({const char*_tmp101="[]";_tag_fat(_tmp101,sizeof(char),3U);}));_tmpFF[1]=_tmp5CE;});Cyc_PP_cat(_tag_fat(_tmpFF,sizeof(struct Cyc_PP_Doc*),2U));});}case 1U: _LL3: _tmpFC=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmpFD=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL4: {struct Cyc_Absyn_Exp*e=_tmpFC;void*zeroterm=_tmpFD;
# 617
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp102[4U];_tmp102[0]=rest,({struct Cyc_PP_Doc*_tmp5D3=Cyc_PP_text(({const char*_tmp103="[";_tag_fat(_tmp103,sizeof(char),2U);}));_tmp102[1]=_tmp5D3;}),({struct Cyc_PP_Doc*_tmp5D2=Cyc_Absynpp_exp2doc(e);_tmp102[2]=_tmp5D2;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_tmp5D1=Cyc_PP_text(({const char*_tmp104="]@zeroterm";_tag_fat(_tmp104,sizeof(char),11U);}));_tmp102[3]=_tmp5D1;}):({struct Cyc_PP_Doc*_tmp5D0=Cyc_PP_text(({const char*_tmp105="]";_tag_fat(_tmp105,sizeof(char),2U);}));_tmp102[3]=_tmp5D0;});Cyc_PP_cat(_tag_fat(_tmp102,sizeof(struct Cyc_PP_Doc*),4U));});}case 3U: _LL5: _tmpFB=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL6: {void*args=_tmpFB;
# 621
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp106=args;unsigned _tmp108;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_List_List*_tmp10D;void*_tmp10C;struct Cyc_Absyn_VarargInfo*_tmp10B;int _tmp10A;struct Cyc_List_List*_tmp109;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->tag == 1U){_LLE: _tmp109=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f1;_tmp10A=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f2;_tmp10B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f3;_tmp10C=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f4;_tmp10D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f5;_tmp10E=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f6;_tmp10F=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f7;_LLF: {struct Cyc_List_List*args2=_tmp109;int c_varargs=_tmp10A;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp10B;void*effopt=_tmp10C;struct Cyc_List_List*rgn_po=_tmp10D;struct Cyc_Absyn_Exp*req=_tmp10E;struct Cyc_Absyn_Exp*ens=_tmp10F;
# 624
return({struct Cyc_PP_Doc*_tmp110[2U];_tmp110[0]=rest,({struct Cyc_PP_Doc*_tmp5D4=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);_tmp110[1]=_tmp5D4;});Cyc_PP_cat(_tag_fat(_tmp110,sizeof(struct Cyc_PP_Doc*),2U));});}}else{_LL10: _tmp107=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp106)->f1;_tmp108=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp106)->f2;_LL11: {struct Cyc_List_List*sl=_tmp107;unsigned loc=_tmp108;
# 626
return({struct Cyc_PP_Doc*_tmp111[2U];_tmp111[0]=rest,({struct Cyc_PP_Doc*_tmp5D8=({struct _fat_ptr _tmp5D7=({const char*_tmp112="(";_tag_fat(_tmp112,sizeof(char),2U);});struct _fat_ptr _tmp5D6=({const char*_tmp113=")";_tag_fat(_tmp113,sizeof(char),2U);});struct _fat_ptr _tmp5D5=({const char*_tmp114=",";_tag_fat(_tmp114,sizeof(char),2U);});Cyc_PP_group(_tmp5D7,_tmp5D6,_tmp5D5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,sl));});_tmp111[1]=_tmp5D8;});Cyc_PP_cat(_tag_fat(_tmp111,sizeof(struct Cyc_PP_Doc*),2U));});}}_LLD:;}}case 5U: _LL7: _tmpFA=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL8: {struct Cyc_List_List*atts=_tmpFA;
# 629
enum Cyc_Cyclone_C_Compilers _tmp115=Cyc_Cyclone_c_compiler;if(_tmp115 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 631
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp116[2U];_tmp116[0]=rest,({struct Cyc_PP_Doc*_tmp5D9=Cyc_Absynpp_atts2doc(atts);_tmp116[1]=_tmp5D9;});Cyc_PP_cat(_tag_fat(_tmp116,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 635
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp117[2U];({struct Cyc_PP_Doc*_tmp5DA=Cyc_Absynpp_callconv2doc(atts);_tmp117[0]=_tmp5DA;}),_tmp117[1]=rest;Cyc_PP_cat(_tag_fat(_tmp117,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmpF7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmpF8=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_tmpF9=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f3;_LLA: {struct Cyc_List_List*ts=_tmpF7;unsigned loc=_tmpF8;int print_kinds=_tmpF9;
# 640
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_tmp118[2U];_tmp118[0]=rest,({struct Cyc_PP_Doc*_tmp5DB=Cyc_Absynpp_ktvars2doc(ts);_tmp118[1]=_tmp5DB;});Cyc_PP_cat(_tag_fat(_tmp118,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 644
return({struct Cyc_PP_Doc*_tmp119[2U];_tmp119[0]=rest,({struct Cyc_PP_Doc*_tmp5DC=Cyc_Absynpp_tvars2doc(ts);_tmp119[1]=_tmp5DC;});Cyc_PP_cat(_tag_fat(_tmp119,sizeof(struct Cyc_PP_Doc*),2U));});}}default: _LLB: _tmpF2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).rgn;_tmpF3=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).nullable;_tmpF4=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).bounds;_tmpF5=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).zero_term;_tmpF6=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LLC: {void*rgn=_tmpF2;void*nullable=_tmpF3;void*bd=_tmpF4;void*zt=_tmpF5;struct Cyc_Absyn_Tqual tq2=_tmpF6;
# 648
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_tmp11A=Cyc_Tcutil_compress(bd);void*_stmttmpB=_tmp11A;void*_tmp11B=_stmttmpB;void*_tmp11C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11B)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11B)->f2 != 0){_LL1A: _tmp11C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11B)->f2)->hd;_LL1B: {void*targ=_tmp11C;
# 656
{void*_tmp11D=Cyc_Tcutil_compress(targ);void*_stmttmpC=_tmp11D;void*_tmp11E=_stmttmpC;struct Cyc_Absyn_Exp*_tmp11F;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp11E)->tag == 9U){_LL1F: _tmp11F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp11E)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp11F;
# 658
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp120="*";_tag_fat(_tmp120,sizeof(char),2U);}):({const char*_tmp121="@";_tag_fat(_tmp121,sizeof(char),2U);}));{
struct _tuple12 _tmp122=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmpD=_tmp122;struct _tuple12 _tmp123=_stmttmpD;int _tmp125;unsigned _tmp124;_LL24: _tmp124=_tmp123.f1;_tmp125=_tmp123.f2;_LL25: {unsigned val=_tmp124;int known=_tmp125;
if(!known || val != (unsigned)1)
ptr=({struct Cyc_PP_Doc*_tmp126[4U];_tmp126[0]=ptr,({struct Cyc_PP_Doc*_tmp5DF=Cyc_Absynpp_lb();_tmp126[1]=_tmp5DF;}),({struct Cyc_PP_Doc*_tmp5DE=Cyc_Absynpp_exp2doc(e);_tmp126[2]=_tmp5DE;}),({struct Cyc_PP_Doc*_tmp5DD=Cyc_Absynpp_rb();_tmp126[3]=_tmp5DD;});Cyc_PP_cat(_tag_fat(_tmp126,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}}}}else{_LL21: _LL22:
# 664
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp127="*";_tag_fat(_tmp127,sizeof(char),2U);}):({const char*_tmp128="@";_tag_fat(_tmp128,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp129[4U];_tmp129[0]=ptr,({struct Cyc_PP_Doc*_tmp5E2=Cyc_Absynpp_lb();_tmp129[1]=_tmp5E2;}),({struct Cyc_PP_Doc*_tmp5E1=Cyc_Absynpp_typ2doc(targ);_tmp129[2]=_tmp5E1;}),({struct Cyc_PP_Doc*_tmp5E0=Cyc_Absynpp_rb();_tmp129[3]=_tmp5E0;});Cyc_PP_cat(_tag_fat(_tmp129,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}_LL1E:;}
# 668
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 670
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp12A="*";_tag_fat(_tmp12A,sizeof(char),2U);}):({const char*_tmp12B="@";_tag_fat(_tmp12B,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp12C[4U];_tmp12C[0]=ptr,({struct Cyc_PP_Doc*_tmp5E5=Cyc_Absynpp_lb();_tmp12C[1]=_tmp5E5;}),({struct Cyc_PP_Doc*_tmp5E4=Cyc_Absynpp_typ2doc(bd);_tmp12C[2]=_tmp5E4;}),({struct Cyc_PP_Doc*_tmp5E3=Cyc_Absynpp_rb();_tmp12C[3]=_tmp5E3;});Cyc_PP_cat(_tag_fat(_tmp12C,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 675
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr && Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp12D="@zeroterm";_tag_fat(_tmp12D,sizeof(char),10U);}));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp12E="@nozeroterm";_tag_fat(_tmp12E,sizeof(char),12U);}));}}
# 681
{void*_tmp12F=Cyc_Tcutil_compress(rgn);void*_stmttmpE=_tmp12F;void*_tmp130=_stmttmpE;switch(*((int*)_tmp130)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1)->tag == 5U){_LL27: _LL28:
 goto _LL26;}else{goto _LL2B;}case 1U: _LL29: if(Cyc_Absynpp_print_for_cycdoc){_LL2A:
 goto _LL26;}else{goto _LL2B;}default: _LL2B: _LL2C:
 rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL26:;}{
# 686
struct Cyc_PP_Doc*spacer1=tqd != mt &&(ztd != mt || rgd != mt)?Cyc_PP_text(({const char*_tmp133=" ";_tag_fat(_tmp133,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp132=" ";_tag_fat(_tmp132,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp131[7U];_tmp131[0]=ptr,_tmp131[1]=ztd,_tmp131[2]=rgd,_tmp131[3]=spacer1,_tmp131[4]=tqd,_tmp131[5]=spacer2,_tmp131[6]=rest;Cyc_PP_cat(_tag_fat(_tmp131,sizeof(struct Cyc_PP_Doc*),7U));});}}}_LL0:;}}
# 692
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_stmttmpF=_tmp137;void*_tmp138=_stmttmpF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp138)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp138)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp139="`H";_tag_fat(_tmp139,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp13A="`U";_tag_fat(_tmp13A,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp13B="`RC";_tag_fat(_tmp13B,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 702
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 706
void*_tmp13C=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp13C;void*_tmp13D=_stmttmp10;struct Cyc_List_List*_tmp13E;void*_tmp13F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2 != 0){_LL1: _tmp13F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2)->hd;_LL2: {void*r=_tmp13F;
({struct Cyc_List_List*_tmp5E7=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_PP_Doc*_tmp5E6=Cyc_Absynpp_rgn2doc(r);_tmp140->hd=_tmp5E6;}),_tmp140->tl=*rgions;_tmp140;});*rgions=_tmp5E7;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp13E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2;_LL4: {struct Cyc_List_List*ts=_tmp13E;
# 709
for(0;ts != 0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
# 712
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp5E9=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));({struct Cyc_PP_Doc*_tmp5E8=Cyc_Absynpp_typ2doc(t);_tmp141->hd=_tmp5E8;}),_tmp141->tl=*effects;_tmp141;});*effects=_tmp5E9;});goto _LL0;}_LL0:;}
# 717
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0 && effects != 0)
return({struct _fat_ptr _tmp5EC=({const char*_tmp142="";_tag_fat(_tmp142,sizeof(char),1U);});struct _fat_ptr _tmp5EB=({const char*_tmp143="";_tag_fat(_tmp143,sizeof(char),1U);});struct _fat_ptr _tmp5EA=({const char*_tmp144="+";_tag_fat(_tmp144,sizeof(char),2U);});Cyc_PP_group(_tmp5EC,_tmp5EB,_tmp5EA,effects);});else{
# 725
struct Cyc_PP_Doc*_tmp145=({struct _fat_ptr _tmp5ED=({const char*_tmp14A=",";_tag_fat(_tmp14A,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp5ED,rgions);});struct Cyc_PP_Doc*doc1=_tmp145;
return({struct _fat_ptr _tmp5F1=({const char*_tmp146="";_tag_fat(_tmp146,sizeof(char),1U);});struct _fat_ptr _tmp5F0=({const char*_tmp147="";_tag_fat(_tmp147,sizeof(char),1U);});struct _fat_ptr _tmp5EF=({const char*_tmp148="+";_tag_fat(_tmp148,sizeof(char),2U);});Cyc_PP_group(_tmp5F1,_tmp5F0,_tmp5EF,({struct Cyc_List_List*_tmp5EE=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5EE,({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=doc1,_tmp149->tl=0;_tmp149;}));}));});}}
# 730
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp14B=k;if(_tmp14B == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp14C="struct ";_tag_fat(_tmp14C,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp14D="union ";_tag_fat(_tmp14D,sizeof(char),7U);}));}_LL0:;}
# 738
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp14E=t;struct Cyc_Absyn_Datatypedecl*_tmp14F;struct Cyc_Absyn_Enumdecl*_tmp150;struct Cyc_Absyn_Aggrdecl*_tmp151;struct Cyc_Absyn_Typedefdecl*_tmp154;struct Cyc_List_List*_tmp153;struct _tuple0*_tmp152;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*_tmp158;enum Cyc_Absyn_AggrKind _tmp157;struct Cyc_List_List*_tmp159;struct Cyc_Absyn_Tvar*_tmp15A;struct Cyc_Core_Opt*_tmp15E;int _tmp15D;void*_tmp15C;struct Cyc_Core_Opt*_tmp15B;void*_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp162;struct _fat_ptr _tmp163;struct _tuple0*_tmp164;struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp167;union Cyc_Absyn_AggrInfo _tmp166;int _tmp168;enum Cyc_Absyn_Size_of _tmp16A;enum Cyc_Absyn_Sign _tmp169;struct Cyc_List_List*_tmp16C;union Cyc_Absyn_DatatypeFieldInfo _tmp16B;struct Cyc_List_List*_tmp16E;union Cyc_Absyn_DatatypeInfo _tmp16D;switch(*((int*)_tmp14E)){case 4U: _LL1: _LL2:
# 742
 return Cyc_PP_text(({const char*_tmp16F="[[[array]]]";_tag_fat(_tmp16F,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)){case 0U: _LL7: _LL8:
# 746
 s=Cyc_PP_text(({const char*_tmp170="void";_tag_fat(_tmp170,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp16D=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp16E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LLE: {union Cyc_Absyn_DatatypeInfo tu_info=_tmp16D;struct Cyc_List_List*ts=_tmp16E;
# 762
{union Cyc_Absyn_DatatypeInfo _tmp178=tu_info;int _tmp17A;struct _tuple0*_tmp179;int _tmp17C;struct _tuple0*_tmp17B;if((_tmp178.UnknownDatatype).tag == 1){_LL48: _tmp17B=((_tmp178.UnknownDatatype).val).name;_tmp17C=((_tmp178.UnknownDatatype).val).is_extensible;_LL49: {struct _tuple0*n=_tmp17B;int is_x=_tmp17C;
_tmp179=n;_tmp17A=is_x;goto _LL4B;}}else{_LL4A: _tmp179=(*(_tmp178.KnownDatatype).val)->name;_tmp17A=(*(_tmp178.KnownDatatype).val)->is_extensible;_LL4B: {struct _tuple0*n=_tmp179;int is_x=_tmp17A;
# 765
struct Cyc_PP_Doc*_tmp17D=Cyc_PP_text(({const char*_tmp181="datatype ";_tag_fat(_tmp181,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp17D;
struct Cyc_PP_Doc*_tmp17E=is_x?Cyc_PP_text(({const char*_tmp180="@extensible ";_tag_fat(_tmp180,sizeof(char),13U);})): Cyc_PP_nil_doc();struct Cyc_PP_Doc*ext=_tmp17E;
s=({struct Cyc_PP_Doc*_tmp17F[4U];_tmp17F[0]=ext,_tmp17F[1]=kw,({struct Cyc_PP_Doc*_tmp5F3=Cyc_Absynpp_qvar2doc(n);_tmp17F[2]=_tmp5F3;}),({struct Cyc_PP_Doc*_tmp5F2=Cyc_Absynpp_tps2doc(ts);_tmp17F[3]=_tmp5F2;});Cyc_PP_cat(_tag_fat(_tmp17F,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 770
goto _LL0;}case 19U: _LLF: _tmp16B=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL10: {union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp16B;struct Cyc_List_List*ts=_tmp16C;
# 772
{union Cyc_Absyn_DatatypeFieldInfo _tmp182=tuf_info;struct _tuple0*_tmp185;int _tmp184;struct _tuple0*_tmp183;int _tmp188;struct _tuple0*_tmp187;struct _tuple0*_tmp186;if((_tmp182.UnknownDatatypefield).tag == 1){_LL4D: _tmp186=((_tmp182.UnknownDatatypefield).val).datatype_name;_tmp187=((_tmp182.UnknownDatatypefield).val).field_name;_tmp188=((_tmp182.UnknownDatatypefield).val).is_extensible;_LL4E: {struct _tuple0*tname=_tmp186;struct _tuple0*fname=_tmp187;int is_x=_tmp188;
# 774
_tmp183=tname;_tmp184=is_x;_tmp185=fname;goto _LL50;}}else{_LL4F: _tmp183=(((_tmp182.KnownDatatypefield).val).f1)->name;_tmp184=(((_tmp182.KnownDatatypefield).val).f1)->is_extensible;_tmp185=(((_tmp182.KnownDatatypefield).val).f2)->name;_LL50: {struct _tuple0*tname=_tmp183;int is_x=_tmp184;struct _tuple0*fname=_tmp185;
# 777
struct Cyc_PP_Doc*_tmp189=Cyc_PP_text(is_x?({const char*_tmp18C="@extensible datatype ";_tag_fat(_tmp18C,sizeof(char),22U);}):({const char*_tmp18D="datatype ";_tag_fat(_tmp18D,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp189;
s=({struct Cyc_PP_Doc*_tmp18A[4U];_tmp18A[0]=kw,({struct Cyc_PP_Doc*_tmp5F6=Cyc_Absynpp_qvar2doc(tname);_tmp18A[1]=_tmp5F6;}),({struct Cyc_PP_Doc*_tmp5F5=Cyc_PP_text(({const char*_tmp18B=".";_tag_fat(_tmp18B,sizeof(char),2U);}));_tmp18A[2]=_tmp5F5;}),({struct Cyc_PP_Doc*_tmp5F4=Cyc_Absynpp_qvar2doc(fname);_tmp18A[3]=_tmp5F4;});Cyc_PP_cat(_tag_fat(_tmp18A,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 781
goto _LL0;}case 1U: _LL11: _tmp169=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp16A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f2;_LL12: {enum Cyc_Absyn_Sign sn=_tmp169;enum Cyc_Absyn_Size_of sz=_tmp16A;
# 783
struct _fat_ptr sns;
struct _fat_ptr ts;
{enum Cyc_Absyn_Sign _tmp18E=sn;switch(_tmp18E){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp18F="";_tag_fat(_tmp18F,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp190="unsigned ";_tag_fat(_tmp190,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 790
{enum Cyc_Absyn_Size_of _tmp191=sz;switch(_tmp191){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 792
{enum Cyc_Absyn_Sign _tmp192=sn;switch(_tmp192){case Cyc_Absyn_None: _LL66: _LL67:
 sns=({const char*_tmp193="";_tag_fat(_tmp193,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed: _LL68: _LL69:
 sns=({const char*_tmp194="signed ";_tag_fat(_tmp194,sizeof(char),8U);});goto _LL65;default: _LL6A: _LL6B:
 sns=({const char*_tmp195="unsigned ";_tag_fat(_tmp195,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 797
ts=({const char*_tmp196="char";_tag_fat(_tmp196,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp197="short";_tag_fat(_tmp197,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp198="int";_tag_fat(_tmp198,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp199="long";_tag_fat(_tmp199,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
# 804
{enum Cyc_Cyclone_C_Compilers _tmp19A=Cyc_Cyclone_c_compiler;if(_tmp19A == Cyc_Cyclone_Gcc_c){_LL6D: _LL6E:
 ts=({const char*_tmp19B="long long";_tag_fat(_tmp19B,sizeof(char),10U);});goto _LL6C;}else{_LL6F: _LL70:
 ts=({const char*_tmp19C="__int64";_tag_fat(_tmp19C,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 808
goto _LL58;}_LL58:;}
# 810
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19F=({struct Cyc_String_pa_PrintArg_struct _tmp574;_tmp574.tag=0U,_tmp574.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_tmp574;});struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp573;_tmp573.tag=0U,_tmp573.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_tmp573;});void*_tmp19D[2U];_tmp19D[0]=& _tmp19F,_tmp19D[1]=& _tmp1A0;({struct _fat_ptr _tmp5F7=({const char*_tmp19E="%s%s";_tag_fat(_tmp19E,sizeof(char),5U);});Cyc_aprintf(_tmp5F7,_tag_fat(_tmp19D,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp168=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL14: {int i=_tmp168;
# 813
{int _tmp1A1=i;switch(_tmp1A1){case 0U: _LL72: _LL73:
 s=Cyc_PP_text(({const char*_tmp1A2="float";_tag_fat(_tmp1A2,sizeof(char),6U);}));goto _LL71;case 1U: _LL74: _LL75:
 s=Cyc_PP_text(({const char*_tmp1A3="double";_tag_fat(_tmp1A3,sizeof(char),7U);}));goto _LL71;default: _LL76: _LL77:
 s=Cyc_PP_text(({const char*_tmp1A4="long double";_tag_fat(_tmp1A4,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 818
goto _LL0;}case 20U: _LL17: _tmp166=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp167=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL18: {union Cyc_Absyn_AggrInfo info=_tmp166;struct Cyc_List_List*ts=_tmp167;
# 821
struct _tuple11 _tmp1A6=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp11=_tmp1A6;struct _tuple11 _tmp1A7=_stmttmp11;struct _tuple0*_tmp1A9;enum Cyc_Absyn_AggrKind _tmp1A8;_LL79: _tmp1A8=_tmp1A7.f1;_tmp1A9=_tmp1A7.f2;_LL7A: {enum Cyc_Absyn_AggrKind k=_tmp1A8;struct _tuple0*n=_tmp1A9;
s=({struct Cyc_PP_Doc*_tmp1AA[3U];({struct Cyc_PP_Doc*_tmp5FA=Cyc_Absynpp_aggr_kind2doc(k);_tmp1AA[0]=_tmp5FA;}),({struct Cyc_PP_Doc*_tmp5F9=Cyc_Absynpp_qvar2doc(n);_tmp1AA[1]=_tmp5F9;}),({struct Cyc_PP_Doc*_tmp5F8=Cyc_Absynpp_tps2doc(ts);_tmp1AA[2]=_tmp5F8;});Cyc_PP_cat(_tag_fat(_tmp1AA,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 16U: _LL1B: _tmp165=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1C: {struct Cyc_List_List*fs=_tmp165;
# 830
s=({struct Cyc_PP_Doc*_tmp1AC[4U];({struct Cyc_PP_Doc*_tmp5FE=Cyc_PP_text(({const char*_tmp1AD="enum ";_tag_fat(_tmp1AD,sizeof(char),6U);}));_tmp1AC[0]=_tmp5FE;}),({struct Cyc_PP_Doc*_tmp5FD=Cyc_Absynpp_lb();_tmp1AC[1]=_tmp5FD;}),({struct Cyc_PP_Doc*_tmp5FC=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_tmp1AC[2]=_tmp5FC;}),({struct Cyc_PP_Doc*_tmp5FB=Cyc_Absynpp_rb();_tmp1AC[3]=_tmp5FB;});Cyc_PP_cat(_tag_fat(_tmp1AC,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1D: _tmp164=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1E: {struct _tuple0*n=_tmp164;
s=({struct Cyc_PP_Doc*_tmp1AE[2U];({struct Cyc_PP_Doc*_tmp600=Cyc_PP_text(({const char*_tmp1AF="enum ";_tag_fat(_tmp1AF,sizeof(char),6U);}));_tmp1AE[0]=_tmp600;}),({struct Cyc_PP_Doc*_tmp5FF=Cyc_Absynpp_qvar2doc(n);_tmp1AE[1]=_tmp5FF;});Cyc_PP_cat(_tag_fat(_tmp1AE,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 17U: _LL23: _tmp163=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL24: {struct _fat_ptr t=_tmp163;
# 834
s=Cyc_PP_text(t);goto _LL0;}case 3U: _LL27: _tmp162=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL28: {struct Cyc_List_List*ts=_tmp162;
# 844
s=({struct Cyc_PP_Doc*_tmp1B7[3U];({struct Cyc_PP_Doc*_tmp603=Cyc_PP_text(({const char*_tmp1B8="region_t<";_tag_fat(_tmp1B8,sizeof(char),10U);}));_tmp1B7[0]=_tmp603;}),({struct Cyc_PP_Doc*_tmp602=Cyc_Absynpp_rgn2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1B7[1]=_tmp602;}),({struct Cyc_PP_Doc*_tmp601=Cyc_PP_text(({const char*_tmp1B9=">";_tag_fat(_tmp1B9,sizeof(char),2U);}));_tmp1B7[2]=_tmp601;});Cyc_PP_cat(_tag_fat(_tmp1B7,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 4U: _LL29: _tmp161=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL2A: {struct Cyc_List_List*ts=_tmp161;
# 846
s=({struct Cyc_PP_Doc*_tmp1BA[3U];({struct Cyc_PP_Doc*_tmp606=Cyc_PP_text(({const char*_tmp1BB="tag_t<";_tag_fat(_tmp1BB,sizeof(char),7U);}));_tmp1BA[0]=_tmp606;}),({struct Cyc_PP_Doc*_tmp605=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1BA[1]=_tmp605;}),({struct Cyc_PP_Doc*_tmp604=Cyc_PP_text(({const char*_tmp1BC=">";_tag_fat(_tmp1BC,sizeof(char),2U);}));_tmp1BA[2]=_tmp604;});Cyc_PP_cat(_tag_fat(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: _LL31: _tmp160=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL32: {struct Cyc_List_List*ts=_tmp160;
# 851
s=({struct Cyc_PP_Doc*_tmp1BD[3U];({struct Cyc_PP_Doc*_tmp609=Cyc_PP_text(({const char*_tmp1BE="regions(";_tag_fat(_tmp1BE,sizeof(char),9U);}));_tmp1BD[0]=_tmp609;}),({struct Cyc_PP_Doc*_tmp608=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1BD[1]=_tmp608;}),({struct Cyc_PP_Doc*_tmp607=Cyc_PP_text(({const char*_tmp1BF=")";_tag_fat(_tmp1BF,sizeof(char),2U);}));_tmp1BD[2]=_tmp607;});Cyc_PP_cat(_tag_fat(_tmp1BD,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL33: _LL34:
 goto _LL36;case 9U: _LL35: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 857
 s=Cyc_PP_text(({const char*_tmp1C0="@true";_tag_fat(_tmp1C0,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
 s=Cyc_PP_text(({const char*_tmp1C1="@false";_tag_fat(_tmp1C1,sizeof(char),7U);}));goto _LL0;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2 != 0){_LL41: _tmp15F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2)->hd;_LL42: {void*t=_tmp15F;
# 860
s=({struct Cyc_PP_Doc*_tmp1C2[4U];({struct Cyc_PP_Doc*_tmp60D=Cyc_PP_text(({const char*_tmp1C3="@thin @numelts";_tag_fat(_tmp1C3,sizeof(char),15U);}));_tmp1C2[0]=_tmp60D;}),({struct Cyc_PP_Doc*_tmp60C=Cyc_Absynpp_lb();_tmp1C2[1]=_tmp60C;}),({struct Cyc_PP_Doc*_tmp60B=Cyc_Absynpp_typ2doc(t);_tmp1C2[2]=_tmp60B;}),({struct Cyc_PP_Doc*_tmp60A=Cyc_Absynpp_rb();_tmp1C2[3]=_tmp60A;});Cyc_PP_cat(_tag_fat(_tmp1C2,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}}else{_LL43: _LL44:
# 862
 s=Cyc_PP_text(({const char*_tmp1C4="@thin";_tag_fat(_tmp1C4,sizeof(char),6U);}));goto _LL0;}default: _LL45: _LL46:
 s=Cyc_PP_text(({const char*_tmp1C5="@fat";_tag_fat(_tmp1C5,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp15B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f1;_tmp15C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f2;_tmp15D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f3;_tmp15E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f4;_LLA: {struct Cyc_Core_Opt*k=_tmp15B;void*topt=_tmp15C;int i=_tmp15D;struct Cyc_Core_Opt*tvs=_tmp15E;
# 748
if(topt != 0)
# 750
return Cyc_Absynpp_ntyp2doc(topt);else{
# 752
struct _fat_ptr kindstring=k == 0?({const char*_tmp175="K";_tag_fat(_tmp175,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp576;_tmp576.tag=0U,_tmp576.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_tmp576;});struct Cyc_Int_pa_PrintArg_struct _tmp174=({struct Cyc_Int_pa_PrintArg_struct _tmp575;_tmp575.tag=1U,_tmp575.f1=(unsigned long)i;_tmp575;});void*_tmp171[2U];_tmp171[0]=& _tmp173,_tmp171[1]=& _tmp174;({struct _fat_ptr _tmp60E=({const char*_tmp172="`E%s%d";_tag_fat(_tmp172,sizeof(char),7U);});Cyc_aprintf(_tmp60E,_tag_fat(_tmp171,sizeof(void*),2U));});}));}
# 755
goto _LL0;}case 2U: _LLB: _tmp15A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->f1;_LLC: {struct Cyc_Absyn_Tvar*tv=_tmp15A;
# 757
s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp176[3U];_tmp176[0]=s,({struct Cyc_PP_Doc*_tmp610=Cyc_PP_text(({const char*_tmp177="::";_tag_fat(_tmp177,sizeof(char),3U);}));_tmp176[1]=_tmp610;}),({struct Cyc_PP_Doc*_tmp60F=Cyc_Absynpp_kindbound2doc(tv->kind);_tmp176[2]=_tmp60F;});Cyc_PP_cat(_tag_fat(_tmp176,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 6U: _LL15: _tmp159=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14E)->f1;_LL16: {struct Cyc_List_List*ts=_tmp159;
# 819
s=({struct Cyc_PP_Doc*_tmp1A5[2U];({struct Cyc_PP_Doc*_tmp612=Cyc_Absynpp_dollar();_tmp1A5[0]=_tmp612;}),({struct Cyc_PP_Doc*_tmp611=Cyc_Absynpp_args2doc(ts);_tmp1A5[1]=_tmp611;});Cyc_PP_cat(_tag_fat(_tmp1A5,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 7U: _LL19: _tmp157=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f1;_tmp158=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp157;struct Cyc_List_List*fs=_tmp158;
# 825
s=({struct Cyc_PP_Doc*_tmp1AB[4U];({struct Cyc_PP_Doc*_tmp616=Cyc_Absynpp_aggr_kind2doc(k);_tmp1AB[0]=_tmp616;}),({struct Cyc_PP_Doc*_tmp615=Cyc_Absynpp_lb();_tmp1AB[1]=_tmp615;}),({
struct Cyc_PP_Doc*_tmp614=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_tmp1AB[2]=_tmp614;}),({
struct Cyc_PP_Doc*_tmp613=Cyc_Absynpp_rb();_tmp1AB[3]=_tmp613;});Cyc_PP_cat(_tag_fat(_tmp1AB,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 9U: _LL1F: _tmp156=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp14E)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp156;
# 832
s=({struct Cyc_PP_Doc*_tmp1B0[3U];({struct Cyc_PP_Doc*_tmp619=Cyc_PP_text(({const char*_tmp1B1="valueof_t(";_tag_fat(_tmp1B1,sizeof(char),11U);}));_tmp1B0[0]=_tmp619;}),({struct Cyc_PP_Doc*_tmp618=Cyc_Absynpp_exp2doc(e);_tmp1B0[1]=_tmp618;}),({struct Cyc_PP_Doc*_tmp617=Cyc_PP_text(({const char*_tmp1B2=")";_tag_fat(_tmp1B2,sizeof(char),2U);}));_tmp1B0[2]=_tmp617;});Cyc_PP_cat(_tag_fat(_tmp1B0,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 11U: _LL21: _tmp155=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp14E)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp155;
s=({struct Cyc_PP_Doc*_tmp1B3[3U];({struct Cyc_PP_Doc*_tmp61C=Cyc_PP_text(({const char*_tmp1B4="typeof(";_tag_fat(_tmp1B4,sizeof(char),8U);}));_tmp1B3[0]=_tmp61C;}),({struct Cyc_PP_Doc*_tmp61B=Cyc_Absynpp_exp2doc(e);_tmp1B3[1]=_tmp61B;}),({struct Cyc_PP_Doc*_tmp61A=Cyc_PP_text(({const char*_tmp1B5=")";_tag_fat(_tmp1B5,sizeof(char),2U);}));_tmp1B3[2]=_tmp61A;});Cyc_PP_cat(_tag_fat(_tmp1B3,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL25: _tmp152=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f1;_tmp153=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f2;_tmp154=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f3;_LL26: {struct _tuple0*n=_tmp152;struct Cyc_List_List*ts=_tmp153;struct Cyc_Absyn_Typedefdecl*kopt=_tmp154;
# 841
s=({struct Cyc_PP_Doc*_tmp1B6[2U];({struct Cyc_PP_Doc*_tmp61E=Cyc_Absynpp_qvar2doc(n);_tmp1B6[0]=_tmp61E;}),({struct Cyc_PP_Doc*_tmp61D=Cyc_Absynpp_tps2doc(ts);_tmp1B6[1]=_tmp61D;});Cyc_PP_cat(_tag_fat(_tmp1B6,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)){case 0U: _LL37: _tmp151=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL38: {struct Cyc_Absyn_Aggrdecl*d=_tmp151;
# 854
s=Cyc_Absynpp_aggrdecl2doc(d);goto _LL0;}case 1U: _LL39: _tmp150=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL3A: {struct Cyc_Absyn_Enumdecl*d=_tmp150;
s=Cyc_Absynpp_enumdecl2doc(d);goto _LL0;}default: _LL3B: _tmp14F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL3C: {struct Cyc_Absyn_Datatypedecl*d=_tmp14F;
s=Cyc_Absynpp_datatypedecl2doc(d);goto _LL0;}}}_LL0:;}
# 865
return s;}
# 868
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _fat_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple18{void*f1;void*f2;};
# 872
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple18*cmp){
struct _tuple18*_tmp1C6=cmp;void*_tmp1C8;void*_tmp1C7;_LL1: _tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C6->f2;_LL2: {void*r1=_tmp1C7;void*r2=_tmp1C8;
return({struct Cyc_PP_Doc*_tmp1C9[3U];({struct Cyc_PP_Doc*_tmp621=Cyc_Absynpp_rgn2doc(r1);_tmp1C9[0]=_tmp621;}),({struct Cyc_PP_Doc*_tmp620=Cyc_PP_text(({const char*_tmp1CA=" > ";_tag_fat(_tmp1CA,sizeof(char),4U);}));_tmp1C9[1]=_tmp620;}),({struct Cyc_PP_Doc*_tmp61F=Cyc_Absynpp_rgn2doc(r2);_tmp1C9[2]=_tmp61F;});Cyc_PP_cat(_tag_fat(_tmp1C9,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 877
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _tmp624=({const char*_tmp1CB="";_tag_fat(_tmp1CB,sizeof(char),1U);});struct _fat_ptr _tmp623=({const char*_tmp1CC="";_tag_fat(_tmp1CC,sizeof(char),1U);});struct _fat_ptr _tmp622=({const char*_tmp1CD=",";_tag_fat(_tmp1CD,sizeof(char),2U);});Cyc_PP_group(_tmp624,_tmp623,_tmp622,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 881
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*_tmp1CE=(*t).f1;struct _fat_ptr*vo=_tmp1CE;
struct Cyc_Core_Opt*dopt=vo == 0?0:({struct Cyc_Core_Opt*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({struct Cyc_PP_Doc*_tmp625=Cyc_PP_text(*vo);_tmp1CF->v=_tmp625;});_tmp1CF;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 887
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 891
struct Cyc_List_List*_tmp1D0=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_List_List*arg_docs=_tmp1D0;
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_tmp627=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp627,({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));({struct Cyc_PP_Doc*_tmp626=Cyc_PP_text(({const char*_tmp1D1="...";_tag_fat(_tmp1D1,sizeof(char),4U);}));_tmp1D2->hd=_tmp626;}),_tmp1D2->tl=0;_tmp1D2;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1D3=({struct Cyc_PP_Doc*_tmp1D5[3U];({struct Cyc_PP_Doc*_tmp62B=Cyc_PP_text(({const char*_tmp1D6="...";_tag_fat(_tmp1D6,sizeof(char),4U);}));_tmp1D5[0]=_tmp62B;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp62A=Cyc_PP_text(({const char*_tmp1D7=" inject ";_tag_fat(_tmp1D7,sizeof(char),9U);}));_tmp1D5[1]=_tmp62A;}):({struct Cyc_PP_Doc*_tmp629=Cyc_PP_text(({const char*_tmp1D8=" ";_tag_fat(_tmp1D8,sizeof(char),2U);}));_tmp1D5[1]=_tmp629;}),({
struct Cyc_PP_Doc*_tmp628=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->f1=cyc_varargs->name,_tmp1D9->f2=cyc_varargs->tq,_tmp1D9->f3=cyc_varargs->type;_tmp1D9;}));_tmp1D5[2]=_tmp628;});Cyc_PP_cat(_tag_fat(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3U));});
# 896
struct Cyc_PP_Doc*varargs_doc=_tmp1D3;
# 900
arg_docs=({struct Cyc_List_List*_tmp62C=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp62C,({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=varargs_doc,_tmp1D4->tl=0;_tmp1D4;}));});}}{
# 902
struct Cyc_PP_Doc*_tmp1DA=({struct _fat_ptr _tmp62F=({const char*_tmp1E9="";_tag_fat(_tmp1E9,sizeof(char),1U);});struct _fat_ptr _tmp62E=({const char*_tmp1EA="";_tag_fat(_tmp1EA,sizeof(char),1U);});struct _fat_ptr _tmp62D=({const char*_tmp1EB=",";_tag_fat(_tmp1EB,sizeof(char),2U);});Cyc_PP_group(_tmp62F,_tmp62E,_tmp62D,arg_docs);});struct Cyc_PP_Doc*arg_doc=_tmp1DA;
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_tmp1DB[3U];_tmp1DB[0]=arg_doc,({struct Cyc_PP_Doc*_tmp631=Cyc_PP_text(({const char*_tmp1DC=";";_tag_fat(_tmp1DC,sizeof(char),2U);}));_tmp1DB[1]=_tmp631;}),({struct Cyc_PP_Doc*_tmp630=Cyc_Absynpp_eff2doc(effopt);_tmp1DB[2]=_tmp630;});Cyc_PP_cat(_tag_fat(_tmp1DB,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
arg_doc=({struct Cyc_PP_Doc*_tmp1DD[3U];_tmp1DD[0]=arg_doc,({struct Cyc_PP_Doc*_tmp633=Cyc_PP_text(({const char*_tmp1DE=":";_tag_fat(_tmp1DE,sizeof(char),2U);}));_tmp1DD[1]=_tmp633;}),({struct Cyc_PP_Doc*_tmp632=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1DD[2]=_tmp632;});Cyc_PP_cat(_tag_fat(_tmp1DD,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp1DF=({struct Cyc_PP_Doc*_tmp1E6[3U];({struct Cyc_PP_Doc*_tmp635=Cyc_PP_text(({const char*_tmp1E7="(";_tag_fat(_tmp1E7,sizeof(char),2U);}));_tmp1E6[0]=_tmp635;}),_tmp1E6[1]=arg_doc,({struct Cyc_PP_Doc*_tmp634=Cyc_PP_text(({const char*_tmp1E8=")";_tag_fat(_tmp1E8,sizeof(char),2U);}));_tmp1E6[2]=_tmp634;});Cyc_PP_cat(_tag_fat(_tmp1E6,sizeof(struct Cyc_PP_Doc*),3U));});struct Cyc_PP_Doc*res=_tmp1DF;
if(req != 0)
res=({struct Cyc_PP_Doc*_tmp1E0[4U];_tmp1E0[0]=res,({struct Cyc_PP_Doc*_tmp638=Cyc_PP_text(({const char*_tmp1E1=" @requires(";_tag_fat(_tmp1E1,sizeof(char),12U);}));_tmp1E0[1]=_tmp638;}),({struct Cyc_PP_Doc*_tmp637=Cyc_Absynpp_exp2doc(req);_tmp1E0[2]=_tmp637;}),({struct Cyc_PP_Doc*_tmp636=Cyc_PP_text(({const char*_tmp1E2=")";_tag_fat(_tmp1E2,sizeof(char),2U);}));_tmp1E0[3]=_tmp636;});Cyc_PP_cat(_tag_fat(_tmp1E0,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
res=({struct Cyc_PP_Doc*_tmp1E3[4U];_tmp1E3[0]=res,({struct Cyc_PP_Doc*_tmp63B=Cyc_PP_text(({const char*_tmp1E4=" @ensures(";_tag_fat(_tmp1E4,sizeof(char),11U);}));_tmp1E3[1]=_tmp63B;}),({struct Cyc_PP_Doc*_tmp63A=Cyc_Absynpp_exp2doc(ens);_tmp1E3[2]=_tmp63A;}),({struct Cyc_PP_Doc*_tmp639=Cyc_PP_text(({const char*_tmp1E5=")";_tag_fat(_tmp1E5,sizeof(char),2U);}));_tmp1E3[3]=_tmp639;});Cyc_PP_cat(_tag_fat(_tmp1E3,sizeof(struct Cyc_PP_Doc*),4U));});
return res;}}}
# 915
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _fat_ptr*v){return Cyc_PP_text(*v);}
# 917
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1EC=0;struct Cyc_List_List*prefix=_tmp1EC;
int match;
{union Cyc_Absyn_Nmspace _tmp1ED=(*q).f1;union Cyc_Absyn_Nmspace _stmttmp12=_tmp1ED;union Cyc_Absyn_Nmspace _tmp1EE=_stmttmp12;struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1F1;switch((_tmp1EE.C_n).tag){case 4U: _LL1: _LL2:
 _tmp1F1=0;goto _LL4;case 1U: _LL3: _tmp1F1=(_tmp1EE.Rel_n).val;_LL4: {struct Cyc_List_List*x=_tmp1F1;
# 923
match=0;
prefix=x;
goto _LL0;}case 3U: _LL5: _tmp1F0=(_tmp1EE.C_n).val;_LL6: {struct Cyc_List_List*x=_tmp1F0;
# 927
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
# 929
goto _LL0;}default: _LL7: _tmp1EF=(_tmp1EE.Abs_n).val;_LL8: {struct Cyc_List_List*x=_tmp1EF;
# 931
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
prefix=Cyc_Absynpp_qvar_to_Cids && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->hd=Cyc_Absynpp_cyc_stringptr,_tmp1F2->tl=x;_tmp1F2;}): x;
goto _LL0;}}_LL0:;}
# 935
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _fat_ptr)({struct Cyc_List_List*_tmp63D=({struct Cyc_List_List*_tmp63C=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp63C,({struct Cyc_List_List*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3->hd=(*q).f2,_tmp1F3->tl=0;_tmp1F3;}));});
# 936
Cyc_str_sepstr(_tmp63D,({const char*_tmp1F4="_";_tag_fat(_tmp1F4,sizeof(char),2U);}));});else{
# 940
if(match)
return*(*q).f2;else{
# 943
return(struct _fat_ptr)({struct Cyc_List_List*_tmp63F=({struct Cyc_List_List*_tmp63E=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp63E,({struct Cyc_List_List*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=(*q).f2,_tmp1F5->tl=0;_tmp1F5;}));});Cyc_str_sepstr(_tmp63F,({const char*_tmp1F6="::";_tag_fat(_tmp1F6,sizeof(char),3U);}));});}}}
# 947
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 951
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr _tmp1F7=Cyc_Absynpp_qvar2string(q);struct _fat_ptr qs=_tmp1F7;
if(Cyc_PP_tex_output)
# 955
return({struct _fat_ptr _tmp642=(struct _fat_ptr)({struct _fat_ptr _tmp641=(struct _fat_ptr)({struct _fat_ptr _tmp640=({const char*_tmp1F8="\\textbf{";_tag_fat(_tmp1F8,sizeof(char),9U);});Cyc_strconcat(_tmp640,(struct _fat_ptr)qs);});Cyc_strconcat(_tmp641,({const char*_tmp1F9="}";_tag_fat(_tmp1F9,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp642,(int)
Cyc_strlen((struct _fat_ptr)qs));});else{
return Cyc_PP_text(qs);}}
# 960
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 962
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 965
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1FA=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp13=_tmp1FA;union Cyc_Absyn_Nmspace _tmp1FB=_stmttmp13;struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FD;switch((_tmp1FB.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp1FB.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 976
 return(struct _fat_ptr)({struct _fat_ptr _tmp643=({const char*_tmp1FE="/* bad namespace : */ ";_tag_fat(_tmp1FE,sizeof(char),23U);});Cyc_strconcat(_tmp643,(struct _fat_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1FD=(_tmp1FB.C_n).val;_LL6: {struct Cyc_List_List*l=_tmp1FD;
# 969
_tmp1FC=l;goto _LL8;}default: _LL7: _tmp1FC=(_tmp1FB.Abs_n).val;_LL8: {struct Cyc_List_List*l=_tmp1FC;
# 971
if(((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,l,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 974
goto _LLA;}}}_LL0:;}else{
# 979
return*(*v).f2;}}
# 982
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 985
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _fat_ptr _tmp1FF=Cyc_Absynpp_typedef_name2string(v);struct _fat_ptr vs=_tmp1FF;
if(Cyc_PP_tex_output)
# 989
return({struct _fat_ptr _tmp646=(struct _fat_ptr)({struct _fat_ptr _tmp645=(struct _fat_ptr)({struct _fat_ptr _tmp644=({const char*_tmp200="\\textbf{";_tag_fat(_tmp200,sizeof(char),9U);});Cyc_strconcat(_tmp644,(struct _fat_ptr)vs);});Cyc_strconcat(_tmp645,({const char*_tmp201="}";_tag_fat(_tmp201,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp646,(int)
Cyc_strlen((struct _fat_ptr)vs));});else{
return Cyc_PP_text(vs);}}
# 994
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp647=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp647,t,0);});}
# 998
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_tmp202=f;unsigned _tmp203;struct _fat_ptr*_tmp204;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->tag == 0U){_LL1: _tmp204=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->f1;_LL2: {struct _fat_ptr*n=_tmp204;
return Cyc_PP_textptr(n);}}else{_LL3: _tmp203=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp202)->f1;_LL4: {unsigned n=_tmp203;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp207=({struct Cyc_Int_pa_PrintArg_struct _tmp577;_tmp577.tag=1U,_tmp577.f1=(unsigned long)((int)n);_tmp577;});void*_tmp205[1U];_tmp205[0]=& _tmp207;({struct _fat_ptr _tmp648=({const char*_tmp206="%d";_tag_fat(_tmp206,sizeof(char),3U);});Cyc_aprintf(_tmp648,_tag_fat(_tmp205,sizeof(void*),1U));});}));}}_LL0:;}
# 1012 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp208=e->r;void*_stmttmp14=_tmp208;void*_tmp209=_stmttmp14;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_Absyn_Exp*_tmp20B;enum Cyc_Absyn_Primop _tmp20C;switch(*((int*)_tmp209)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp20C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL6: {enum Cyc_Absyn_Primop p=_tmp20C;
# 1017
enum Cyc_Absyn_Primop _tmp20D=p;switch(_tmp20D){case Cyc_Absyn_Plus: _LL56: _LL57:
 return 100;case Cyc_Absyn_Times: _LL58: _LL59:
 return 110;case Cyc_Absyn_Minus: _LL5A: _LL5B:
 return 100;case Cyc_Absyn_Div: _LL5C: _LL5D:
 goto _LL5F;case Cyc_Absyn_Mod: _LL5E: _LL5F:
 return 110;case Cyc_Absyn_Eq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Neq: _LL62: _LL63:
 return 70;case Cyc_Absyn_Gt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Lt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Gte: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6A: _LL6B:
 return 80;case Cyc_Absyn_Not: _LL6C: _LL6D:
 goto _LL6F;case Cyc_Absyn_Bitnot: _LL6E: _LL6F:
 return 130;case Cyc_Absyn_Bitand: _LL70: _LL71:
 return 60;case Cyc_Absyn_Bitor: _LL72: _LL73:
 return 40;case Cyc_Absyn_Bitxor: _LL74: _LL75:
 return 50;case Cyc_Absyn_Bitlshift: _LL76: _LL77:
 return 90;case Cyc_Absyn_Bitlrshift: _LL78: _LL79:
 return 80;case Cyc_Absyn_Numelts: _LL7A: _LL7B:
 return 140;default: _LL7C: _LL7D:
 return 140;}_LL55:;}case 4U: _LL7: _LL8:
# 1039
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return - 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp20B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp20B;
return Cyc_Absynpp_exp_prec(e2);}case 13U: _LL1B: _tmp20A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp20A;
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
 goto _LL32;case 41U: _LL31: _LL32:
 return 130;case 21U: _LL33: _LL34:
 goto _LL36;case 22U: _LL35: _LL36:
 goto _LL38;case 23U: _LL37: _LL38:
 return 140;case 24U: _LL39: _LL3A:
 return 150;case 25U: _LL3B: _LL3C:
 goto _LL3E;case 26U: _LL3D: _LL3E:
 goto _LL40;case 27U: _LL3F: _LL40:
 goto _LL42;case 28U: _LL41: _LL42:
 goto _LL44;case 29U: _LL43: _LL44:
 goto _LL46;case 30U: _LL45: _LL46:
 goto _LL48;case 31U: _LL47: _LL48:
 goto _LL4A;case 32U: _LL49: _LL4A:
 goto _LL4C;case 33U: _LL4B: _LL4C:
 goto _LL4E;case 34U: _LL4D: _LL4E:
 goto _LL50;case 35U: _LL4F: _LL50:
 goto _LL52;case 36U: _LL51: _LL52:
 return 140;default: _LL53: _LL54:
 return 10000;}_LL0:;}
# 1081
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct _tuple19{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1085
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
struct _tuple19*_tmp20E=(struct _tuple19*)o->hd;struct _tuple19*_stmttmp15=_tmp20E;struct _tuple19*_tmp20F=_stmttmp15;struct Cyc_Absyn_Exp*_tmp211;struct _fat_ptr _tmp210;_LL1: _tmp210=_tmp20F->f1;_tmp211=_tmp20F->f2;_LL2: {struct _fat_ptr c=_tmp210;struct Cyc_Absyn_Exp*e=_tmp211;
s=({struct Cyc_PP_Doc*_tmp212[5U];({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_text(({const char*_tmp213="\"";_tag_fat(_tmp213,sizeof(char),2U);}));_tmp212[0]=_tmp64D;}),({struct Cyc_PP_Doc*_tmp64C=Cyc_PP_text(c);_tmp212[1]=_tmp64C;}),({struct Cyc_PP_Doc*_tmp64B=Cyc_PP_text(({const char*_tmp214="\" (";_tag_fat(_tmp214,sizeof(char),4U);}));_tmp212[2]=_tmp64B;}),({struct Cyc_PP_Doc*_tmp64A=Cyc_Absynpp_exp2doc(e);_tmp212[3]=_tmp64A;}),({struct Cyc_PP_Doc*_tmp649=Cyc_PP_text(({const char*_tmp215=")";_tag_fat(_tmp215,sizeof(char),2U);}));_tmp212[4]=_tmp649;});Cyc_PP_cat(_tag_fat(_tmp212,sizeof(struct Cyc_PP_Doc*),5U));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp216[2U];_tmp216[0]=s,({struct Cyc_PP_Doc*_tmp64E=Cyc_PP_text(({const char*_tmp217=",";_tag_fat(_tmp217,sizeof(char),2U);}));_tmp216[1]=_tmp64E;});Cyc_PP_cat(_tag_fat(_tmp216,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1095
return s;}
# 1098
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp218[2U];({struct Cyc_PP_Doc*_tmp650=Cyc_PP_text(({const char*_tmp219=": ";_tag_fat(_tmp219,sizeof(char),3U);}));_tmp218[0]=_tmp650;}),({struct Cyc_PP_Doc*_tmp64F=Cyc_Absynpp_asm_iolist_doc_in(o);_tmp218[1]=_tmp64F;});Cyc_PP_cat(_tag_fat(_tmp218,sizeof(struct Cyc_PP_Doc*),2U));});
# 1103
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_tmp21A[3U];_tmp21A[0]=s,({struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp21B=": : ";_tag_fat(_tmp21B,sizeof(char),5U);}));_tmp21A[1]=_tmp652;}),({struct Cyc_PP_Doc*_tmp651=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp21A[2]=_tmp651;});Cyc_PP_cat(_tag_fat(_tmp21A,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1107
s=({struct Cyc_PP_Doc*_tmp21C[3U];_tmp21C[0]=s,({struct Cyc_PP_Doc*_tmp654=Cyc_PP_text(({const char*_tmp21D=" : ";_tag_fat(_tmp21D,sizeof(char),4U);}));_tmp21C[1]=_tmp654;}),({struct Cyc_PP_Doc*_tmp653=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp21C[2]=_tmp653;});Cyc_PP_cat(_tag_fat(_tmp21C,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1109
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_tmp21E[2U];_tmp21E[0]=s,ncol == 0?({struct Cyc_PP_Doc*_tmp657=Cyc_PP_text(({const char*_tmp21F=": : :";_tag_fat(_tmp21F,sizeof(char),6U);}));_tmp21E[1]=_tmp657;}):(ncol == 1?({struct Cyc_PP_Doc*_tmp656=Cyc_PP_text(({const char*_tmp220=" : : ";_tag_fat(_tmp220,sizeof(char),6U);}));_tmp21E[1]=_tmp656;}):({struct Cyc_PP_Doc*_tmp655=Cyc_PP_text(({const char*_tmp221=" : ";_tag_fat(_tmp221,sizeof(char),4U);}));_tmp21E[1]=_tmp655;}));Cyc_PP_cat(_tag_fat(_tmp21E,sizeof(struct Cyc_PP_Doc*),2U));});
while(cl != 0){
s=({struct Cyc_PP_Doc*_tmp222[4U];_tmp222[0]=s,({struct Cyc_PP_Doc*_tmp65A=Cyc_PP_text(({const char*_tmp223="\"";_tag_fat(_tmp223,sizeof(char),2U);}));_tmp222[1]=_tmp65A;}),({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_tmp222[2]=_tmp659;}),({struct Cyc_PP_Doc*_tmp658=Cyc_PP_text(({const char*_tmp224="\"";_tag_fat(_tmp224,sizeof(char),2U);}));_tmp222[3]=_tmp658;});Cyc_PP_cat(_tag_fat(_tmp222,sizeof(struct Cyc_PP_Doc*),4U));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_tmp225[2U];_tmp225[0]=s,({struct Cyc_PP_Doc*_tmp65B=Cyc_PP_text(({const char*_tmp226=", ";_tag_fat(_tmp226,sizeof(char),3U);}));_tmp225[1]=_tmp65B;});Cyc_PP_cat(_tag_fat(_tmp225,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1119
return s;}
# 1122
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp227=e->r;void*_stmttmp16=_tmp227;void*_tmp228=_stmttmp16;struct Cyc_Absyn_Stmt*_tmp229;struct Cyc_List_List*_tmp22B;struct Cyc_Core_Opt*_tmp22A;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp22C;int _tmp232;struct Cyc_Absyn_Exp*_tmp231;void**_tmp230;struct Cyc_Absyn_Exp*_tmp22F;int _tmp22E;struct Cyc_Absyn_Enumfield*_tmp233;struct Cyc_Absyn_Enumfield*_tmp234;struct Cyc_Absyn_Datatypefield*_tmp236;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp237;struct Cyc_List_List*_tmp23A;struct Cyc_List_List*_tmp239;struct _tuple0*_tmp238;void*_tmp23C;struct Cyc_Absyn_Exp*_tmp23B;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Vardecl*_tmp23D;struct Cyc_List_List*_tmp240;struct Cyc_List_List*_tmp242;struct _tuple8*_tmp241;struct Cyc_List_List*_tmp243;struct Cyc_Absyn_Exp*_tmp245;struct Cyc_Absyn_Exp*_tmp244;struct _fat_ptr*_tmp247;struct Cyc_Absyn_Exp*_tmp246;struct _fat_ptr*_tmp249;struct Cyc_Absyn_Exp*_tmp248;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_List_List*_tmp24C;void*_tmp24B;struct _fat_ptr*_tmp24E;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp252;struct Cyc_List_List*_tmp251;struct _fat_ptr _tmp250;int _tmp24F;void*_tmp254;struct Cyc_Absyn_Exp*_tmp255;struct Cyc_Absyn_Exp*_tmp256;void*_tmp257;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Exp*_tmp25A;struct Cyc_Absyn_Exp*_tmp25C;void*_tmp25B;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Exp*_tmp25F;struct Cyc_List_List*_tmp261;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*_tmp268;enum Cyc_Absyn_Incrementor _tmp26C;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Core_Opt*_tmp26E;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_List_List*_tmp271;enum Cyc_Absyn_Primop _tmp270;struct _fat_ptr _tmp272;void*_tmp273;union Cyc_Absyn_Cnst _tmp274;switch(*((int*)_tmp228)){case 0U: _LL1: _tmp274=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp274;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1U: _LL3: _tmp273=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL4: {void*b=_tmp273;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL5: _tmp272=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL6: {struct _fat_ptr p=_tmp272;
# 1129
s=({struct Cyc_PP_Doc*_tmp275[4U];({struct Cyc_PP_Doc*_tmp65F=Cyc_PP_text(({const char*_tmp276="__cyclone_pragma__";_tag_fat(_tmp276,sizeof(char),19U);}));_tmp275[0]=_tmp65F;}),({struct Cyc_PP_Doc*_tmp65E=Cyc_PP_text(({const char*_tmp277="(";_tag_fat(_tmp277,sizeof(char),2U);}));_tmp275[1]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_PP_text(p);_tmp275[2]=_tmp65D;}),({struct Cyc_PP_Doc*_tmp65C=Cyc_PP_text(({const char*_tmp278=")";_tag_fat(_tmp278,sizeof(char),2U);}));_tmp275[3]=_tmp65C;});Cyc_PP_cat(_tag_fat(_tmp275,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 3U: _LL7: _tmp270=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp271=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp270;struct Cyc_List_List*es=_tmp271;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4U: _LL9: _tmp26D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp26E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp26F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp26D;struct Cyc_Core_Opt*popt=_tmp26E;struct Cyc_Absyn_Exp*e2=_tmp26F;
# 1132
s=({struct Cyc_PP_Doc*_tmp279[5U];({struct Cyc_PP_Doc*_tmp665=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp279[0]=_tmp665;}),({
struct Cyc_PP_Doc*_tmp664=Cyc_PP_text(({const char*_tmp27A=" ";_tag_fat(_tmp27A,sizeof(char),2U);}));_tmp279[1]=_tmp664;}),
popt == 0?({struct Cyc_PP_Doc*_tmp663=Cyc_PP_nil_doc();_tmp279[2]=_tmp663;}):({struct Cyc_PP_Doc*_tmp662=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_tmp279[2]=_tmp662;}),({
struct Cyc_PP_Doc*_tmp661=Cyc_PP_text(({const char*_tmp27B="= ";_tag_fat(_tmp27B,sizeof(char),3U);}));_tmp279[3]=_tmp661;}),({
struct Cyc_PP_Doc*_tmp660=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp279[4]=_tmp660;});Cyc_PP_cat(_tag_fat(_tmp279,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp26B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp26C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LLC: {struct Cyc_Absyn_Exp*e2=_tmp26B;enum Cyc_Absyn_Incrementor i=_tmp26C;
# 1139
struct Cyc_PP_Doc*_tmp27C=Cyc_Absynpp_exp2doc_prec(myprec,e2);struct Cyc_PP_Doc*es=_tmp27C;
{enum Cyc_Absyn_Incrementor _tmp27D=i;switch(_tmp27D){case Cyc_Absyn_PreInc: _LL56: _LL57:
 s=({struct Cyc_PP_Doc*_tmp27E[2U];({struct Cyc_PP_Doc*_tmp666=Cyc_PP_text(({const char*_tmp27F="++";_tag_fat(_tmp27F,sizeof(char),3U);}));_tmp27E[0]=_tmp666;}),_tmp27E[1]=es;Cyc_PP_cat(_tag_fat(_tmp27E,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PreDec: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp280[2U];({struct Cyc_PP_Doc*_tmp667=Cyc_PP_text(({const char*_tmp281="--";_tag_fat(_tmp281,sizeof(char),3U);}));_tmp280[0]=_tmp667;}),_tmp280[1]=es;Cyc_PP_cat(_tag_fat(_tmp280,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostInc: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp282[2U];_tmp282[0]=es,({struct Cyc_PP_Doc*_tmp668=Cyc_PP_text(({const char*_tmp283="++";_tag_fat(_tmp283,sizeof(char),3U);}));_tmp282[1]=_tmp668;});Cyc_PP_cat(_tag_fat(_tmp282,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostDec: _LL5C: _LL5D:
 goto _LL5F;default: _LL5E: _LL5F:
 s=({struct Cyc_PP_Doc*_tmp284[2U];_tmp284[0]=es,({struct Cyc_PP_Doc*_tmp669=Cyc_PP_text(({const char*_tmp285="--";_tag_fat(_tmp285,sizeof(char),3U);}));_tmp284[1]=_tmp669;});Cyc_PP_cat(_tag_fat(_tmp284,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;}_LL55:;}
# 1147
goto _LL0;}case 6U: _LLD: _tmp268=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp269=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp26A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp268;struct Cyc_Absyn_Exp*e2=_tmp269;struct Cyc_Absyn_Exp*e3=_tmp26A;
# 1149
s=({struct Cyc_PP_Doc*_tmp286[5U];({struct Cyc_PP_Doc*_tmp66E=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp286[0]=_tmp66E;}),({struct Cyc_PP_Doc*_tmp66D=Cyc_PP_text(({const char*_tmp287=" ? ";_tag_fat(_tmp287,sizeof(char),4U);}));_tmp286[1]=_tmp66D;}),({struct Cyc_PP_Doc*_tmp66C=Cyc_Absynpp_exp2doc_prec(0,e2);_tmp286[2]=_tmp66C;}),({
struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp288=" : ";_tag_fat(_tmp288,sizeof(char),4U);}));_tmp286[3]=_tmp66B;}),({struct Cyc_PP_Doc*_tmp66A=Cyc_Absynpp_exp2doc_prec(myprec,e3);_tmp286[4]=_tmp66A;});Cyc_PP_cat(_tag_fat(_tmp286,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 7U: _LLF: _tmp266=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp267=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp266;struct Cyc_Absyn_Exp*e2=_tmp267;
# 1153
s=({struct Cyc_PP_Doc*_tmp289[3U];({struct Cyc_PP_Doc*_tmp671=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp289[0]=_tmp671;}),({struct Cyc_PP_Doc*_tmp670=Cyc_PP_text(({const char*_tmp28A=" && ";_tag_fat(_tmp28A,sizeof(char),5U);}));_tmp289[1]=_tmp670;}),({struct Cyc_PP_Doc*_tmp66F=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp289[2]=_tmp66F;});Cyc_PP_cat(_tag_fat(_tmp289,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 8U: _LL11: _tmp264=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp265=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp264;struct Cyc_Absyn_Exp*e2=_tmp265;
# 1156
s=({struct Cyc_PP_Doc*_tmp28B[3U];({struct Cyc_PP_Doc*_tmp674=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp28B[0]=_tmp674;}),({struct Cyc_PP_Doc*_tmp673=Cyc_PP_text(({const char*_tmp28C=" || ";_tag_fat(_tmp28C,sizeof(char),5U);}));_tmp28B[1]=_tmp673;}),({struct Cyc_PP_Doc*_tmp672=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp28B[2]=_tmp672;});Cyc_PP_cat(_tag_fat(_tmp28B,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 9U: _LL13: _tmp262=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp263=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp262;struct Cyc_Absyn_Exp*e2=_tmp263;
# 1161
s=({struct Cyc_PP_Doc*_tmp28D[3U];({struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_tmp28D[0]=_tmp677;}),({struct Cyc_PP_Doc*_tmp676=Cyc_PP_text(({const char*_tmp28E=", ";_tag_fat(_tmp28E,sizeof(char),3U);}));_tmp28D[1]=_tmp676;}),({struct Cyc_PP_Doc*_tmp675=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_tmp28D[2]=_tmp675;});Cyc_PP_cat(_tag_fat(_tmp28D,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 10U: _LL15: _tmp260=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp261=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp260;struct Cyc_List_List*es=_tmp261;
# 1164
s=({struct Cyc_PP_Doc*_tmp28F[4U];({struct Cyc_PP_Doc*_tmp67B=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp28F[0]=_tmp67B;}),({
struct Cyc_PP_Doc*_tmp67A=Cyc_PP_text(({const char*_tmp290="(";_tag_fat(_tmp290,sizeof(char),2U);}));_tmp28F[1]=_tmp67A;}),({
struct Cyc_PP_Doc*_tmp679=Cyc_Absynpp_exps2doc_prec(20,es);_tmp28F[2]=_tmp679;}),({
struct Cyc_PP_Doc*_tmp678=Cyc_PP_text(({const char*_tmp291=")";_tag_fat(_tmp291,sizeof(char),2U);}));_tmp28F[3]=_tmp678;});Cyc_PP_cat(_tag_fat(_tmp28F,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 11U: _LL17: _tmp25F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp25F;
s=({struct Cyc_PP_Doc*_tmp292[2U];({struct Cyc_PP_Doc*_tmp67D=Cyc_PP_text(({const char*_tmp293="throw ";_tag_fat(_tmp293,sizeof(char),7U);}));_tmp292[0]=_tmp67D;}),({struct Cyc_PP_Doc*_tmp67C=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp292[1]=_tmp67C;});Cyc_PP_cat(_tag_fat(_tmp292,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 12U: _LL19: _tmp25E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp25E;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13U: _LL1B: _tmp25D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp25D;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14U: _LL1D: _tmp25B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp25C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL1E: {void*t=_tmp25B;struct Cyc_Absyn_Exp*e1=_tmp25C;
# 1173
s=({struct Cyc_PP_Doc*_tmp294[4U];({struct Cyc_PP_Doc*_tmp681=Cyc_PP_text(({const char*_tmp295="(";_tag_fat(_tmp295,sizeof(char),2U);}));_tmp294[0]=_tmp681;}),({struct Cyc_PP_Doc*_tmp680=Cyc_Absynpp_typ2doc(t);_tmp294[1]=_tmp680;}),({struct Cyc_PP_Doc*_tmp67F=Cyc_PP_text(({const char*_tmp296=")";_tag_fat(_tmp296,sizeof(char),2U);}));_tmp294[2]=_tmp67F;}),({struct Cyc_PP_Doc*_tmp67E=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp294[3]=_tmp67E;});Cyc_PP_cat(_tag_fat(_tmp294,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1F: _tmp25A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp25A;
s=({struct Cyc_PP_Doc*_tmp297[2U];({struct Cyc_PP_Doc*_tmp683=Cyc_PP_text(({const char*_tmp298="&";_tag_fat(_tmp298,sizeof(char),2U);}));_tmp297[0]=_tmp683;}),({struct Cyc_PP_Doc*_tmp682=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp297[1]=_tmp682;});Cyc_PP_cat(_tag_fat(_tmp297,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 16U: _LL21: _tmp258=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp259=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL22: {struct Cyc_Absyn_Exp*ropt=_tmp258;struct Cyc_Absyn_Exp*e1=_tmp259;
# 1176
if(ropt == 0)
s=({struct Cyc_PP_Doc*_tmp299[2U];({struct Cyc_PP_Doc*_tmp685=Cyc_PP_text(({const char*_tmp29A="new ";_tag_fat(_tmp29A,sizeof(char),5U);}));_tmp299[0]=_tmp685;}),({struct Cyc_PP_Doc*_tmp684=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp299[1]=_tmp684;});Cyc_PP_cat(_tag_fat(_tmp299,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1179
s=({struct Cyc_PP_Doc*_tmp29B[4U];({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp29C="rnew(";_tag_fat(_tmp29C,sizeof(char),6U);}));_tmp29B[0]=_tmp689;}),({struct Cyc_PP_Doc*_tmp688=Cyc_Absynpp_exp2doc(ropt);_tmp29B[1]=_tmp688;}),({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp29D=") ";_tag_fat(_tmp29D,sizeof(char),3U);}));_tmp29B[2]=_tmp687;}),({struct Cyc_PP_Doc*_tmp686=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp29B[3]=_tmp686;});Cyc_PP_cat(_tag_fat(_tmp29B,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;}case 17U: _LL23: _tmp257=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL24: {void*t=_tmp257;
s=({struct Cyc_PP_Doc*_tmp29E[3U];({struct Cyc_PP_Doc*_tmp68C=Cyc_PP_text(({const char*_tmp29F="sizeof(";_tag_fat(_tmp29F,sizeof(char),8U);}));_tmp29E[0]=_tmp68C;}),({struct Cyc_PP_Doc*_tmp68B=Cyc_Absynpp_typ2doc(t);_tmp29E[1]=_tmp68B;}),({struct Cyc_PP_Doc*_tmp68A=Cyc_PP_text(({const char*_tmp2A0=")";_tag_fat(_tmp2A0,sizeof(char),2U);}));_tmp29E[2]=_tmp68A;});Cyc_PP_cat(_tag_fat(_tmp29E,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 18U: _LL25: _tmp256=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp256;
s=({struct Cyc_PP_Doc*_tmp2A1[3U];({struct Cyc_PP_Doc*_tmp68F=Cyc_PP_text(({const char*_tmp2A2="sizeof(";_tag_fat(_tmp2A2,sizeof(char),8U);}));_tmp2A1[0]=_tmp68F;}),({struct Cyc_PP_Doc*_tmp68E=Cyc_Absynpp_exp2doc(e1);_tmp2A1[1]=_tmp68E;}),({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp2A3=")";_tag_fat(_tmp2A3,sizeof(char),2U);}));_tmp2A1[2]=_tmp68D;});Cyc_PP_cat(_tag_fat(_tmp2A1,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 41U: _LL27: _tmp255=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp255;
# 1184
s=({struct Cyc_PP_Doc*_tmp2A4[3U];({struct Cyc_PP_Doc*_tmp692=Cyc_PP_text(({const char*_tmp2A5="__extension__(";_tag_fat(_tmp2A5,sizeof(char),15U);}));_tmp2A4[0]=_tmp692;}),({struct Cyc_PP_Doc*_tmp691=Cyc_Absynpp_exp2doc(e);_tmp2A4[1]=_tmp691;}),({struct Cyc_PP_Doc*_tmp690=Cyc_PP_text(({const char*_tmp2A6=")";_tag_fat(_tmp2A6,sizeof(char),2U);}));_tmp2A4[2]=_tmp690;});Cyc_PP_cat(_tag_fat(_tmp2A4,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 39U: _LL29: _tmp254=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL2A: {void*t=_tmp254;
s=({struct Cyc_PP_Doc*_tmp2A7[3U];({struct Cyc_PP_Doc*_tmp695=Cyc_PP_text(({const char*_tmp2A8="valueof(";_tag_fat(_tmp2A8,sizeof(char),9U);}));_tmp2A7[0]=_tmp695;}),({struct Cyc_PP_Doc*_tmp694=Cyc_Absynpp_typ2doc(t);_tmp2A7[1]=_tmp694;}),({struct Cyc_PP_Doc*_tmp693=Cyc_PP_text(({const char*_tmp2A9=")";_tag_fat(_tmp2A9,sizeof(char),2U);}));_tmp2A7[2]=_tmp693;});Cyc_PP_cat(_tag_fat(_tmp2A7,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 40U: _LL2B: _tmp24F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp250=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp251=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_tmp252=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f4;_tmp253=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f5;_LL2C: {int vol=_tmp24F;struct _fat_ptr t=_tmp250;struct Cyc_List_List*o=_tmp251;struct Cyc_List_List*i=_tmp252;struct Cyc_List_List*cl=_tmp253;
# 1187
if(vol)
s=({struct Cyc_PP_Doc*_tmp2AA[7U];({struct Cyc_PP_Doc*_tmp69C=Cyc_PP_text(({const char*_tmp2AB="__asm__";_tag_fat(_tmp2AB,sizeof(char),8U);}));_tmp2AA[0]=_tmp69C;}),({struct Cyc_PP_Doc*_tmp69B=Cyc_PP_text(({const char*_tmp2AC=" volatile (";_tag_fat(_tmp2AC,sizeof(char),12U);}));_tmp2AA[1]=_tmp69B;}),({struct Cyc_PP_Doc*_tmp69A=Cyc_PP_text(({const char*_tmp2AD="\"";_tag_fat(_tmp2AD,sizeof(char),2U);}));_tmp2AA[2]=_tmp69A;}),({struct Cyc_PP_Doc*_tmp699=Cyc_PP_text(t);_tmp2AA[3]=_tmp699;}),({struct Cyc_PP_Doc*_tmp698=Cyc_PP_text(({const char*_tmp2AE="\"";_tag_fat(_tmp2AE,sizeof(char),2U);}));_tmp2AA[4]=_tmp698;}),({struct Cyc_PP_Doc*_tmp697=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2AA[5]=_tmp697;}),({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp2AF=")";_tag_fat(_tmp2AF,sizeof(char),2U);}));_tmp2AA[6]=_tmp696;});Cyc_PP_cat(_tag_fat(_tmp2AA,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1190
s=({struct Cyc_PP_Doc*_tmp2B0[6U];({struct Cyc_PP_Doc*_tmp6A2=Cyc_PP_text(({const char*_tmp2B1="__asm__(";_tag_fat(_tmp2B1,sizeof(char),9U);}));_tmp2B0[0]=_tmp6A2;}),({struct Cyc_PP_Doc*_tmp6A1=Cyc_PP_text(({const char*_tmp2B2="\"";_tag_fat(_tmp2B2,sizeof(char),2U);}));_tmp2B0[1]=_tmp6A1;}),({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(t);_tmp2B0[2]=_tmp6A0;}),({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_text(({const char*_tmp2B3="\"";_tag_fat(_tmp2B3,sizeof(char),2U);}));_tmp2B0[3]=_tmp69F;}),({struct Cyc_PP_Doc*_tmp69E=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2B0[4]=_tmp69E;}),({struct Cyc_PP_Doc*_tmp69D=Cyc_PP_text(({const char*_tmp2B4=")";_tag_fat(_tmp2B4,sizeof(char),2U);}));_tmp2B0[5]=_tmp69D;});Cyc_PP_cat(_tag_fat(_tmp2B0,sizeof(struct Cyc_PP_Doc*),6U));});}
goto _LL0;}case 38U: _LL2D: _tmp24D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp24E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp24D;struct _fat_ptr*f=_tmp24E;
# 1193
s=({struct Cyc_PP_Doc*_tmp2B5[5U];({struct Cyc_PP_Doc*_tmp6A7=Cyc_PP_text(({const char*_tmp2B6="tagcheck(";_tag_fat(_tmp2B6,sizeof(char),10U);}));_tmp2B5[0]=_tmp6A7;}),({struct Cyc_PP_Doc*_tmp6A6=Cyc_Absynpp_exp2doc(e);_tmp2B5[1]=_tmp6A6;}),({struct Cyc_PP_Doc*_tmp6A5=Cyc_PP_text(({const char*_tmp2B7=".";_tag_fat(_tmp2B7,sizeof(char),2U);}));_tmp2B5[2]=_tmp6A5;}),({struct Cyc_PP_Doc*_tmp6A4=Cyc_PP_textptr(f);_tmp2B5[3]=_tmp6A4;}),({struct Cyc_PP_Doc*_tmp6A3=Cyc_PP_text(({const char*_tmp2B8=")";_tag_fat(_tmp2B8,sizeof(char),2U);}));_tmp2B5[4]=_tmp6A3;});Cyc_PP_cat(_tag_fat(_tmp2B5,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 19U: _LL2F: _tmp24B=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp24C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL30: {void*t=_tmp24B;struct Cyc_List_List*l=_tmp24C;
# 1196
s=({struct Cyc_PP_Doc*_tmp2B9[5U];({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp2BA="offsetof(";_tag_fat(_tmp2BA,sizeof(char),10U);}));_tmp2B9[0]=_tmp6AD;}),({struct Cyc_PP_Doc*_tmp6AC=Cyc_Absynpp_typ2doc(t);_tmp2B9[1]=_tmp6AC;}),({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_text(({const char*_tmp2BB=",";_tag_fat(_tmp2BB,sizeof(char),2U);}));_tmp2B9[2]=_tmp6AB;}),({
struct Cyc_PP_Doc*_tmp6AA=({struct _fat_ptr _tmp6A9=({const char*_tmp2BC=".";_tag_fat(_tmp2BC,sizeof(char),2U);});Cyc_PP_seq(_tmp6A9,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_offsetof_field_to_doc,l));});_tmp2B9[3]=_tmp6AA;}),({struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_text(({const char*_tmp2BD=")";_tag_fat(_tmp2BD,sizeof(char),2U);}));_tmp2B9[4]=_tmp6A8;});Cyc_PP_cat(_tag_fat(_tmp2B9,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 20U: _LL31: _tmp24A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp24A;
s=({struct Cyc_PP_Doc*_tmp2BE[2U];({struct Cyc_PP_Doc*_tmp6AF=Cyc_PP_text(({const char*_tmp2BF="*";_tag_fat(_tmp2BF,sizeof(char),2U);}));_tmp2BE[0]=_tmp6AF;}),({struct Cyc_PP_Doc*_tmp6AE=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2BE[1]=_tmp6AE;});Cyc_PP_cat(_tag_fat(_tmp2BE,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 21U: _LL33: _tmp248=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp249=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp248;struct _fat_ptr*n=_tmp249;
# 1201
s=({struct Cyc_PP_Doc*_tmp2C0[3U];({struct Cyc_PP_Doc*_tmp6B2=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C0[0]=_tmp6B2;}),({struct Cyc_PP_Doc*_tmp6B1=Cyc_PP_text(({const char*_tmp2C1=".";_tag_fat(_tmp2C1,sizeof(char),2U);}));_tmp2C0[1]=_tmp6B1;}),({struct Cyc_PP_Doc*_tmp6B0=Cyc_PP_textptr(n);_tmp2C0[2]=_tmp6B0;});Cyc_PP_cat(_tag_fat(_tmp2C0,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 22U: _LL35: _tmp246=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp247=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp246;struct _fat_ptr*n=_tmp247;
# 1203
s=({struct Cyc_PP_Doc*_tmp2C2[3U];({struct Cyc_PP_Doc*_tmp6B5=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C2[0]=_tmp6B5;}),({struct Cyc_PP_Doc*_tmp6B4=Cyc_PP_text(({const char*_tmp2C3="->";_tag_fat(_tmp2C3,sizeof(char),3U);}));_tmp2C2[1]=_tmp6B4;}),({struct Cyc_PP_Doc*_tmp6B3=Cyc_PP_textptr(n);_tmp2C2[2]=_tmp6B3;});Cyc_PP_cat(_tag_fat(_tmp2C2,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 23U: _LL37: _tmp244=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp245=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp244;struct Cyc_Absyn_Exp*e2=_tmp245;
# 1205
s=({struct Cyc_PP_Doc*_tmp2C4[4U];({struct Cyc_PP_Doc*_tmp6B9=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C4[0]=_tmp6B9;}),({struct Cyc_PP_Doc*_tmp6B8=Cyc_PP_text(({const char*_tmp2C5="[";_tag_fat(_tmp2C5,sizeof(char),2U);}));_tmp2C4[1]=_tmp6B8;}),({struct Cyc_PP_Doc*_tmp6B7=Cyc_Absynpp_exp2doc(e2);_tmp2C4[2]=_tmp6B7;}),({struct Cyc_PP_Doc*_tmp6B6=Cyc_PP_text(({const char*_tmp2C6="]";_tag_fat(_tmp2C6,sizeof(char),2U);}));_tmp2C4[3]=_tmp6B6;});Cyc_PP_cat(_tag_fat(_tmp2C4,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 24U: _LL39: _tmp243=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL3A: {struct Cyc_List_List*es=_tmp243;
# 1207
s=({struct Cyc_PP_Doc*_tmp2C7[4U];({struct Cyc_PP_Doc*_tmp6BD=Cyc_Absynpp_dollar();_tmp2C7[0]=_tmp6BD;}),({struct Cyc_PP_Doc*_tmp6BC=Cyc_PP_text(({const char*_tmp2C8="(";_tag_fat(_tmp2C8,sizeof(char),2U);}));_tmp2C7[1]=_tmp6BC;}),({struct Cyc_PP_Doc*_tmp6BB=Cyc_Absynpp_exps2doc_prec(20,es);_tmp2C7[2]=_tmp6BB;}),({struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_text(({const char*_tmp2C9=")";_tag_fat(_tmp2C9,sizeof(char),2U);}));_tmp2C7[3]=_tmp6BA;});Cyc_PP_cat(_tag_fat(_tmp2C7,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 25U: _LL3B: _tmp241=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp242=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL3C: {struct _tuple8*vat=_tmp241;struct Cyc_List_List*des=_tmp242;
# 1209
s=({struct Cyc_PP_Doc*_tmp2CA[4U];({struct Cyc_PP_Doc*_tmp6C2=Cyc_PP_text(({const char*_tmp2CB="(";_tag_fat(_tmp2CB,sizeof(char),2U);}));_tmp2CA[0]=_tmp6C2;}),({struct Cyc_PP_Doc*_tmp6C1=Cyc_Absynpp_typ2doc((*vat).f3);_tmp2CA[1]=_tmp6C1;}),({struct Cyc_PP_Doc*_tmp6C0=Cyc_PP_text(({const char*_tmp2CC=")";_tag_fat(_tmp2CC,sizeof(char),2U);}));_tmp2CA[2]=_tmp6C0;}),({
struct Cyc_PP_Doc*_tmp6BF=({struct _fat_ptr _tmp6BE=({const char*_tmp2CD=",";_tag_fat(_tmp2CD,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6BE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});_tmp2CA[3]=_tmp6BF;});Cyc_PP_cat(_tag_fat(_tmp2CA,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 26U: _LL3D: _tmp240=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL3E: {struct Cyc_List_List*des=_tmp240;
s=({struct _fat_ptr _tmp6C3=({const char*_tmp2CE=",";_tag_fat(_tmp2CE,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6C3,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 27U: _LL3F: _tmp23D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp23E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp23F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL40: {struct Cyc_Absyn_Vardecl*vd=_tmp23D;struct Cyc_Absyn_Exp*e1=_tmp23E;struct Cyc_Absyn_Exp*e2=_tmp23F;
# 1214
s=({struct Cyc_PP_Doc*_tmp2CF[8U];({struct Cyc_PP_Doc*_tmp6CB=Cyc_Absynpp_lb();_tmp2CF[0]=_tmp6CB;}),({struct Cyc_PP_Doc*_tmp6CA=Cyc_PP_text(({const char*_tmp2D0="for ";_tag_fat(_tmp2D0,sizeof(char),5U);}));_tmp2CF[1]=_tmp6CA;}),({
struct Cyc_PP_Doc*_tmp6C9=Cyc_PP_text(*(*vd->name).f2);_tmp2CF[2]=_tmp6C9;}),({struct Cyc_PP_Doc*_tmp6C8=Cyc_PP_text(({const char*_tmp2D1=" < ";_tag_fat(_tmp2D1,sizeof(char),4U);}));_tmp2CF[3]=_tmp6C8;}),({struct Cyc_PP_Doc*_tmp6C7=Cyc_Absynpp_exp2doc(e1);_tmp2CF[4]=_tmp6C7;}),({struct Cyc_PP_Doc*_tmp6C6=Cyc_PP_text(({const char*_tmp2D2=" : ";_tag_fat(_tmp2D2,sizeof(char),4U);}));_tmp2CF[5]=_tmp6C6;}),({
struct Cyc_PP_Doc*_tmp6C5=Cyc_Absynpp_exp2doc(e2);_tmp2CF[6]=_tmp6C5;}),({struct Cyc_PP_Doc*_tmp6C4=Cyc_Absynpp_rb();_tmp2CF[7]=_tmp6C4;});Cyc_PP_cat(_tag_fat(_tmp2CF,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 28U: _LL41: _tmp23B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp23C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL42: {struct Cyc_Absyn_Exp*e=_tmp23B;void*t=_tmp23C;
# 1219
s=({struct Cyc_PP_Doc*_tmp2D3[7U];({struct Cyc_PP_Doc*_tmp6D2=Cyc_Absynpp_lb();_tmp2D3[0]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_PP_text(({const char*_tmp2D4="for x ";_tag_fat(_tmp2D4,sizeof(char),7U);}));_tmp2D3[1]=_tmp6D1;}),({
struct Cyc_PP_Doc*_tmp6D0=Cyc_PP_text(({const char*_tmp2D5=" < ";_tag_fat(_tmp2D5,sizeof(char),4U);}));_tmp2D3[2]=_tmp6D0;}),({
struct Cyc_PP_Doc*_tmp6CF=Cyc_Absynpp_exp2doc(e);_tmp2D3[3]=_tmp6CF;}),({
struct Cyc_PP_Doc*_tmp6CE=Cyc_PP_text(({const char*_tmp2D6=" : ";_tag_fat(_tmp2D6,sizeof(char),4U);}));_tmp2D3[4]=_tmp6CE;}),({
struct Cyc_PP_Doc*_tmp6CD=Cyc_Absynpp_typ2doc(t);_tmp2D3[5]=_tmp6CD;}),({
struct Cyc_PP_Doc*_tmp6CC=Cyc_Absynpp_rb();_tmp2D3[6]=_tmp6CC;});Cyc_PP_cat(_tag_fat(_tmp2D3,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 29U: _LL43: _tmp238=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp239=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp23A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL44: {struct _tuple0*n=_tmp238;struct Cyc_List_List*ts=_tmp239;struct Cyc_List_List*des=_tmp23A;
# 1227
struct Cyc_List_List*_tmp2D7=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des);struct Cyc_List_List*des_doc=_tmp2D7;
s=({struct Cyc_PP_Doc*_tmp2D8[2U];({struct Cyc_PP_Doc*_tmp6D6=Cyc_Absynpp_qvar2doc(n);_tmp2D8[0]=_tmp6D6;}),({
struct Cyc_PP_Doc*_tmp6D5=({struct _fat_ptr _tmp6D4=({const char*_tmp2D9=",";_tag_fat(_tmp2D9,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6D4,
ts != 0?({struct Cyc_List_List*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));({struct Cyc_PP_Doc*_tmp6D3=Cyc_Absynpp_tps2doc(ts);_tmp2DA->hd=_tmp6D3;}),_tmp2DA->tl=des_doc;_tmp2DA;}): des_doc);});
# 1229
_tmp2D8[1]=_tmp6D5;});Cyc_PP_cat(_tag_fat(_tmp2D8,sizeof(struct Cyc_PP_Doc*),2U));});
# 1231
goto _LL0;}case 30U: _LL45: _tmp237=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL46: {struct Cyc_List_List*des=_tmp237;
s=({struct _fat_ptr _tmp6D7=({const char*_tmp2DB=",";_tag_fat(_tmp2DB,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6D7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 31U: _LL47: _tmp235=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp236=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL48: {struct Cyc_List_List*es=_tmp235;struct Cyc_Absyn_Datatypefield*tuf=_tmp236;
# 1234
if(es == 0)
# 1236
s=Cyc_Absynpp_qvar2doc(tuf->name);else{
# 1238
s=({struct Cyc_PP_Doc*_tmp2DC[2U];({struct Cyc_PP_Doc*_tmp6DC=Cyc_Absynpp_qvar2doc(tuf->name);_tmp2DC[0]=_tmp6DC;}),({
struct Cyc_PP_Doc*_tmp6DB=({struct _fat_ptr _tmp6DA=({const char*_tmp2DD="(";_tag_fat(_tmp2DD,sizeof(char),2U);});struct _fat_ptr _tmp6D9=({const char*_tmp2DE=")";_tag_fat(_tmp2DE,sizeof(char),2U);});struct _fat_ptr _tmp6D8=({const char*_tmp2DF=",";_tag_fat(_tmp2DF,sizeof(char),2U);});Cyc_PP_egroup(_tmp6DA,_tmp6D9,_tmp6D8,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,es));});_tmp2DC[1]=_tmp6DB;});Cyc_PP_cat(_tag_fat(_tmp2DC,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;}case 32U: _LL49: _tmp234=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL4A: {struct Cyc_Absyn_Enumfield*ef=_tmp234;
_tmp233=ef;goto _LL4C;}case 33U: _LL4B: _tmp233=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL4C: {struct Cyc_Absyn_Enumfield*ef=_tmp233;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 34U: _LL4D: _tmp22E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).is_calloc;_tmp22F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).rgn;_tmp230=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).elt_type;_tmp231=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).num_elts;_tmp232=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).inline_call;_LL4E: {int is_calloc=_tmp22E;struct Cyc_Absyn_Exp*rgnopt=_tmp22F;void**topt=_tmp230;struct Cyc_Absyn_Exp*e=_tmp231;int inline_call=_tmp232;
# 1246
if(is_calloc){
# 1248
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U);
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp2E0[5U];({struct Cyc_PP_Doc*_tmp6E1=Cyc_PP_text(({const char*_tmp2E1="calloc(";_tag_fat(_tmp2E1,sizeof(char),8U);}));_tmp2E0[0]=_tmp6E1;}),({struct Cyc_PP_Doc*_tmp6E0=Cyc_Absynpp_exp2doc(e);_tmp2E0[1]=_tmp6E0;}),({struct Cyc_PP_Doc*_tmp6DF=Cyc_PP_text(({const char*_tmp2E2=",";_tag_fat(_tmp2E2,sizeof(char),2U);}));_tmp2E0[2]=_tmp6DF;}),({struct Cyc_PP_Doc*_tmp6DE=Cyc_Absynpp_exp2doc(st);_tmp2E0[3]=_tmp6DE;}),({struct Cyc_PP_Doc*_tmp6DD=Cyc_PP_text(({const char*_tmp2E3=")";_tag_fat(_tmp2E3,sizeof(char),2U);}));_tmp2E0[4]=_tmp6DD;});Cyc_PP_cat(_tag_fat(_tmp2E0,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1252
s=({struct Cyc_PP_Doc*_tmp2E4[7U];({struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_text(({const char*_tmp2E5="rcalloc(";_tag_fat(_tmp2E5,sizeof(char),9U);}));_tmp2E4[0]=_tmp6E8;}),({struct Cyc_PP_Doc*_tmp6E7=Cyc_Absynpp_exp2doc(rgnopt);_tmp2E4[1]=_tmp6E7;}),({struct Cyc_PP_Doc*_tmp6E6=Cyc_PP_text(({const char*_tmp2E6=",";_tag_fat(_tmp2E6,sizeof(char),2U);}));_tmp2E4[2]=_tmp6E6;}),({
struct Cyc_PP_Doc*_tmp6E5=Cyc_Absynpp_exp2doc(e);_tmp2E4[3]=_tmp6E5;}),({struct Cyc_PP_Doc*_tmp6E4=Cyc_PP_text(({const char*_tmp2E7=",";_tag_fat(_tmp2E7,sizeof(char),2U);}));_tmp2E4[4]=_tmp6E4;}),({struct Cyc_PP_Doc*_tmp6E3=Cyc_Absynpp_exp2doc(st);_tmp2E4[5]=_tmp6E3;}),({struct Cyc_PP_Doc*_tmp6E2=Cyc_PP_text(({const char*_tmp2E8=")";_tag_fat(_tmp2E8,sizeof(char),2U);}));_tmp2E4[6]=_tmp6E2;});Cyc_PP_cat(_tag_fat(_tmp2E4,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1255
struct Cyc_Absyn_Exp*new_e;
# 1257
if(topt == 0)
new_e=e;else{
# 1260
new_e=({struct Cyc_Absyn_Exp*_tmp6E9=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp6E9,e,0U);});}
# 1262
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp2E9[3U];({struct Cyc_PP_Doc*_tmp6EC=Cyc_PP_text(({const char*_tmp2EA="malloc(";_tag_fat(_tmp2EA,sizeof(char),8U);}));_tmp2E9[0]=_tmp6EC;}),({struct Cyc_PP_Doc*_tmp6EB=Cyc_Absynpp_exp2doc(new_e);_tmp2E9[1]=_tmp6EB;}),({struct Cyc_PP_Doc*_tmp6EA=Cyc_PP_text(({const char*_tmp2EB=")";_tag_fat(_tmp2EB,sizeof(char),2U);}));_tmp2E9[2]=_tmp6EA;});Cyc_PP_cat(_tag_fat(_tmp2E9,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1265
if(inline_call)
s=({struct Cyc_PP_Doc*_tmp2EC[5U];({struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_text(({const char*_tmp2ED="rmalloc_inline(";_tag_fat(_tmp2ED,sizeof(char),16U);}));_tmp2EC[0]=_tmp6F1;}),({struct Cyc_PP_Doc*_tmp6F0=Cyc_Absynpp_exp2doc(rgnopt);_tmp2EC[1]=_tmp6F0;}),({struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_text(({const char*_tmp2EE=",";_tag_fat(_tmp2EE,sizeof(char),2U);}));_tmp2EC[2]=_tmp6EF;}),({
struct Cyc_PP_Doc*_tmp6EE=Cyc_Absynpp_exp2doc(new_e);_tmp2EC[3]=_tmp6EE;}),({struct Cyc_PP_Doc*_tmp6ED=Cyc_PP_text(({const char*_tmp2EF=")";_tag_fat(_tmp2EF,sizeof(char),2U);}));_tmp2EC[4]=_tmp6ED;});Cyc_PP_cat(_tag_fat(_tmp2EC,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1269
s=({struct Cyc_PP_Doc*_tmp2F0[5U];({struct Cyc_PP_Doc*_tmp6F6=Cyc_PP_text(({const char*_tmp2F1="rmalloc(";_tag_fat(_tmp2F1,sizeof(char),9U);}));_tmp2F0[0]=_tmp6F6;}),({struct Cyc_PP_Doc*_tmp6F5=Cyc_Absynpp_exp2doc(rgnopt);_tmp2F0[1]=_tmp6F5;}),({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp2F2=",";_tag_fat(_tmp2F2,sizeof(char),2U);}));_tmp2F0[2]=_tmp6F4;}),({
struct Cyc_PP_Doc*_tmp6F3=Cyc_Absynpp_exp2doc(new_e);_tmp2F0[3]=_tmp6F3;}),({struct Cyc_PP_Doc*_tmp6F2=Cyc_PP_text(({const char*_tmp2F3=")";_tag_fat(_tmp2F3,sizeof(char),2U);}));_tmp2F0[4]=_tmp6F2;});Cyc_PP_cat(_tag_fat(_tmp2F0,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1273
goto _LL0;}case 35U: _LL4F: _tmp22C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp22D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL50: {struct Cyc_Absyn_Exp*e1=_tmp22C;struct Cyc_Absyn_Exp*e2=_tmp22D;
# 1275
s=({struct Cyc_PP_Doc*_tmp2F4[3U];({struct Cyc_PP_Doc*_tmp6F9=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2F4[0]=_tmp6F9;}),({struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_text(({const char*_tmp2F5=" :=: ";_tag_fat(_tmp2F5,sizeof(char),6U);}));_tmp2F4[1]=_tmp6F8;}),({struct Cyc_PP_Doc*_tmp6F7=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2F4[2]=_tmp6F7;});Cyc_PP_cat(_tag_fat(_tmp2F4,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 36U: _LL51: _tmp22A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp22B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL52: {struct Cyc_Core_Opt*n=_tmp22A;struct Cyc_List_List*des=_tmp22B;
# 1278
s=({struct _fat_ptr _tmp6FA=({const char*_tmp2F6=",";_tag_fat(_tmp2F6,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6FA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _LL53: _tmp229=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL54: {struct Cyc_Absyn_Stmt*s2=_tmp229;
# 1281
s=({struct Cyc_PP_Doc*_tmp2F7[7U];({struct Cyc_PP_Doc*_tmp701=Cyc_PP_text(({const char*_tmp2F8="(";_tag_fat(_tmp2F8,sizeof(char),2U);}));_tmp2F7[0]=_tmp701;}),({struct Cyc_PP_Doc*_tmp700=Cyc_Absynpp_lb();_tmp2F7[1]=_tmp700;}),({struct Cyc_PP_Doc*_tmp6FF=Cyc_PP_blank_doc();_tmp2F7[2]=_tmp6FF;}),({
struct Cyc_PP_Doc*_tmp6FE=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_tmp2F7[3]=_tmp6FE;}),({
struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_blank_doc();_tmp2F7[4]=_tmp6FD;}),({struct Cyc_PP_Doc*_tmp6FC=Cyc_Absynpp_rb();_tmp2F7[5]=_tmp6FC;}),({struct Cyc_PP_Doc*_tmp6FB=Cyc_PP_text(({const char*_tmp2F9=")";_tag_fat(_tmp2F9,sizeof(char),2U);}));_tmp2F7[6]=_tmp6FB;});Cyc_PP_cat(_tag_fat(_tmp2F7,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}}_LL0:;}
# 1286
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2FA[3U];({struct Cyc_PP_Doc*_tmp703=Cyc_PP_text(({const char*_tmp2FB="(";_tag_fat(_tmp2FB,sizeof(char),2U);}));_tmp2FA[0]=_tmp703;}),_tmp2FA[1]=s,({struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp2FC=")";_tag_fat(_tmp2FC,sizeof(char),2U);}));_tmp2FA[2]=_tmp702;});Cyc_PP_cat(_tag_fat(_tmp2FA,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1291
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2FD=d;struct _fat_ptr*_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2FD)->tag == 0U){_LL1: _tmp2FF=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2FD)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2FF;
return({struct Cyc_PP_Doc*_tmp300[3U];({struct Cyc_PP_Doc*_tmp706=Cyc_PP_text(({const char*_tmp301=".[";_tag_fat(_tmp301,sizeof(char),3U);}));_tmp300[0]=_tmp706;}),({struct Cyc_PP_Doc*_tmp705=Cyc_Absynpp_exp2doc(e);_tmp300[1]=_tmp705;}),({struct Cyc_PP_Doc*_tmp704=Cyc_PP_text(({const char*_tmp302="]";_tag_fat(_tmp302,sizeof(char),2U);}));_tmp300[2]=_tmp704;});Cyc_PP_cat(_tag_fat(_tmp300,sizeof(struct Cyc_PP_Doc*),3U));});}}else{_LL3: _tmp2FE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2FD)->f1;_LL4: {struct _fat_ptr*v=_tmp2FE;
return({struct Cyc_PP_Doc*_tmp303[2U];({struct Cyc_PP_Doc*_tmp708=Cyc_PP_text(({const char*_tmp304=".";_tag_fat(_tmp304,sizeof(char),2U);}));_tmp303[0]=_tmp708;}),({struct Cyc_PP_Doc*_tmp707=Cyc_PP_textptr(v);_tmp303[1]=_tmp707;});Cyc_PP_cat(_tag_fat(_tmp303,sizeof(struct Cyc_PP_Doc*),2U));});}}_LL0:;}
# 1298
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp305[2U];({struct Cyc_PP_Doc*_tmp70D=({struct _fat_ptr _tmp70C=({const char*_tmp306="";_tag_fat(_tmp306,sizeof(char),1U);});struct _fat_ptr _tmp70B=({const char*_tmp307="=";_tag_fat(_tmp307,sizeof(char),2U);});struct _fat_ptr _tmp70A=({const char*_tmp308="=";_tag_fat(_tmp308,sizeof(char),2U);});Cyc_PP_egroup(_tmp70C,_tmp70B,_tmp70A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp305[0]=_tmp70D;}),({
struct Cyc_PP_Doc*_tmp709=Cyc_Absynpp_exp2doc((*de).f2);_tmp305[1]=_tmp709;});Cyc_PP_cat(_tag_fat(_tmp305,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1304
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _tmp710=({const char*_tmp309="";_tag_fat(_tmp309,sizeof(char),1U);});struct _fat_ptr _tmp70F=({const char*_tmp30A="";_tag_fat(_tmp30A,sizeof(char),1U);});struct _fat_ptr _tmp70E=({const char*_tmp30B=",";_tag_fat(_tmp30B,sizeof(char),2U);});Cyc_PP_group(_tmp710,_tmp70F,_tmp70E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1308
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_tmp320=({unsigned _tmp31F=28U + 1U;char*_tmp31E=_cycalloc_atomic(_check_times(_tmp31F,sizeof(char)));({{unsigned _tmp578=28U;unsigned i;for(i=0;i < _tmp578;++ i){_tmp31E[i]='z';}_tmp31E[_tmp578]=0;}0;});_tmp31E;});_tag_fat(_tmp320,sizeof(char),29U);});
({struct _fat_ptr _tmp30C=_fat_ptr_plus(x,sizeof(char),27);char _tmp30D=*((char*)_check_fat_subscript(_tmp30C,sizeof(char),0U));char _tmp30E='\000';if(_get_fat_size(_tmp30C,sizeof(char))== 1U &&(_tmp30D == 0 && _tmp30E != 0))_throw_arraybounds();*((char*)_tmp30C.curr)=_tmp30E;});
({struct _fat_ptr _tmp30F=_fat_ptr_plus(x,sizeof(char),26);char _tmp310=*((char*)_check_fat_subscript(_tmp30F,sizeof(char),0U));char _tmp311='L';if(_get_fat_size(_tmp30F,sizeof(char))== 1U &&(_tmp310 == 0 && _tmp311 != 0))_throw_arraybounds();*((char*)_tmp30F.curr)=_tmp311;});
({struct _fat_ptr _tmp312=_fat_ptr_plus(x,sizeof(char),25);char _tmp313=*((char*)_check_fat_subscript(_tmp312,sizeof(char),0U));char _tmp314='L';if(_get_fat_size(_tmp312,sizeof(char))== 1U &&(_tmp313 == 0 && _tmp314 != 0))_throw_arraybounds();*((char*)_tmp312.curr)=_tmp314;});
({struct _fat_ptr _tmp315=_fat_ptr_plus(x,sizeof(char),24);char _tmp316=*((char*)_check_fat_subscript(_tmp315,sizeof(char),0U));char _tmp317='U';if(_get_fat_size(_tmp315,sizeof(char))== 1U &&(_tmp316 == 0 && _tmp317 != 0))_throw_arraybounds();*((char*)_tmp315.curr)=_tmp317;});
({struct _fat_ptr _tmp318=_fat_ptr_plus(x,sizeof(char),23);char _tmp319=*((char*)_check_fat_subscript(_tmp318,sizeof(char),0U));char _tmp31A='0';if(_get_fat_size(_tmp318,sizeof(char))== 1U &&(_tmp319 == 0 && _tmp31A != 0))_throw_arraybounds();*((char*)_tmp318.curr)=_tmp31A;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _fat_ptr _tmp31B=_fat_ptr_plus(x,sizeof(char),index);char _tmp31C=*((char*)_check_fat_subscript(_tmp31B,sizeof(char),0U));char _tmp31D=c;if(_get_fat_size(_tmp31B,sizeof(char))== 1U &&(_tmp31C == 0 && _tmp31D != 0))_throw_arraybounds();*((char*)_tmp31B.curr)=_tmp31D;});
i=i / (unsigned long long)10;
-- index;}
# 1322
return(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1326
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp321=c;struct _fat_ptr _tmp322;struct _fat_ptr _tmp323;struct _fat_ptr _tmp324;long long _tmp326;enum Cyc_Absyn_Sign _tmp325;int _tmp328;enum Cyc_Absyn_Sign _tmp327;short _tmp32A;enum Cyc_Absyn_Sign _tmp329;struct _fat_ptr _tmp32B;char _tmp32D;enum Cyc_Absyn_Sign _tmp32C;switch((_tmp321.String_c).tag){case 2U: _LL1: _tmp32C=((_tmp321.Char_c).val).f1;_tmp32D=((_tmp321.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp32C;char ch=_tmp32D;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp330=({struct Cyc_String_pa_PrintArg_struct _tmp579;_tmp579.tag=0U,({struct _fat_ptr _tmp711=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp579.f1=_tmp711;});_tmp579;});void*_tmp32E[1U];_tmp32E[0]=& _tmp330;({struct _fat_ptr _tmp712=({const char*_tmp32F="'%s'";_tag_fat(_tmp32F,sizeof(char),5U);});Cyc_aprintf(_tmp712,_tag_fat(_tmp32E,sizeof(void*),1U));});}));}case 3U: _LL3: _tmp32B=(_tmp321.Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp32B;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp333=({struct Cyc_String_pa_PrintArg_struct _tmp57A;_tmp57A.tag=0U,_tmp57A.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp57A;});void*_tmp331[1U];_tmp331[0]=& _tmp333;({struct _fat_ptr _tmp713=({const char*_tmp332="L'%s'";_tag_fat(_tmp332,sizeof(char),6U);});Cyc_aprintf(_tmp713,_tag_fat(_tmp331,sizeof(void*),1U));});}));}case 4U: _LL5: _tmp329=((_tmp321.Short_c).val).f1;_tmp32A=((_tmp321.Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp329;short s=_tmp32A;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp336=({struct Cyc_Int_pa_PrintArg_struct _tmp57B;_tmp57B.tag=1U,_tmp57B.f1=(unsigned long)((int)s);_tmp57B;});void*_tmp334[1U];_tmp334[0]=& _tmp336;({struct _fat_ptr _tmp714=({const char*_tmp335="%d";_tag_fat(_tmp335,sizeof(char),3U);});Cyc_aprintf(_tmp714,_tag_fat(_tmp334,sizeof(void*),1U));});}));}case 5U: _LL7: _tmp327=((_tmp321.Int_c).val).f1;_tmp328=((_tmp321.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign sn=_tmp327;int i=_tmp328;
# 1332
if((int)sn == (int)1U)return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp339=({struct Cyc_Int_pa_PrintArg_struct _tmp57C;_tmp57C.tag=1U,_tmp57C.f1=(unsigned)i;_tmp57C;});void*_tmp337[1U];_tmp337[0]=& _tmp339;({struct _fat_ptr _tmp715=({const char*_tmp338="%uU";_tag_fat(_tmp338,sizeof(char),4U);});Cyc_aprintf(_tmp715,_tag_fat(_tmp337,sizeof(void*),1U));});}));else{
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp33C=({struct Cyc_Int_pa_PrintArg_struct _tmp57D;_tmp57D.tag=1U,_tmp57D.f1=(unsigned long)i;_tmp57D;});void*_tmp33A[1U];_tmp33A[0]=& _tmp33C;({struct _fat_ptr _tmp716=({const char*_tmp33B="%d";_tag_fat(_tmp33B,sizeof(char),3U);});Cyc_aprintf(_tmp716,_tag_fat(_tmp33A,sizeof(void*),1U));});}));}}case 6U: _LL9: _tmp325=((_tmp321.LongLong_c).val).f1;_tmp326=((_tmp321.LongLong_c).val).f2;_LLA: {enum Cyc_Absyn_Sign sg=_tmp325;long long i=_tmp326;
# 1336
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7U: _LLB: _tmp324=((_tmp321.Float_c).val).f1;_LLC: {struct _fat_ptr x=_tmp324;
return Cyc_PP_text(x);}case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp33D="NULL";_tag_fat(_tmp33D,sizeof(char),5U);}));case 8U: _LLF: _tmp323=(_tmp321.String_c).val;_LL10: {struct _fat_ptr s=_tmp323;
return({struct Cyc_PP_Doc*_tmp33E[3U];({struct Cyc_PP_Doc*_tmp719=Cyc_PP_text(({const char*_tmp33F="\"";_tag_fat(_tmp33F,sizeof(char),2U);}));_tmp33E[0]=_tmp719;}),({struct Cyc_PP_Doc*_tmp718=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_tmp33E[1]=_tmp718;}),({struct Cyc_PP_Doc*_tmp717=Cyc_PP_text(({const char*_tmp340="\"";_tag_fat(_tmp340,sizeof(char),2U);}));_tmp33E[2]=_tmp717;});Cyc_PP_cat(_tag_fat(_tmp33E,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL11: _tmp322=(_tmp321.Wstring_c).val;_LL12: {struct _fat_ptr s=_tmp322;
return({struct Cyc_PP_Doc*_tmp341[3U];({struct Cyc_PP_Doc*_tmp71C=Cyc_PP_text(({const char*_tmp342="L\"";_tag_fat(_tmp342,sizeof(char),3U);}));_tmp341[0]=_tmp71C;}),({struct Cyc_PP_Doc*_tmp71B=Cyc_PP_text(s);_tmp341[1]=_tmp71B;}),({struct Cyc_PP_Doc*_tmp71A=Cyc_PP_text(({const char*_tmp343="\"";_tag_fat(_tmp343,sizeof(char),2U);}));_tmp341[2]=_tmp71A;});Cyc_PP_cat(_tag_fat(_tmp341,sizeof(struct Cyc_PP_Doc*),3U));});}}_LL0:;}
# 1344
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp71F=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp346=({struct Cyc_String_pa_PrintArg_struct _tmp57E;_tmp57E.tag=0U,({struct _fat_ptr _tmp71D=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp57E.f1=_tmp71D;});_tmp57E;});void*_tmp344[1U];_tmp344[0]=& _tmp346;({struct _fat_ptr _tmp71E=({const char*_tmp345="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_tmp345,sizeof(char),47U);});Cyc_aprintf(_tmp71E,_tag_fat(_tmp344,sizeof(void*),1U));});});_tmp347->f1=_tmp71F;});_tmp347;}));
# 1350
return({struct Cyc_PP_Doc*_tmp348[3U];({struct Cyc_PP_Doc*_tmp722=Cyc_PP_text(({const char*_tmp349="numelts(";_tag_fat(_tmp349,sizeof(char),9U);}));_tmp348[0]=_tmp722;}),({struct Cyc_PP_Doc*_tmp721=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp348[1]=_tmp721;}),({struct Cyc_PP_Doc*_tmp720=Cyc_PP_text(({const char*_tmp34A=")";_tag_fat(_tmp34A,sizeof(char),2U);}));_tmp348[2]=_tmp720;});Cyc_PP_cat(_tag_fat(_tmp348,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1352
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp725=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34D=({struct Cyc_String_pa_PrintArg_struct _tmp57F;_tmp57F.tag=0U,({struct _fat_ptr _tmp723=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp57F.f1=_tmp723;});_tmp57F;});void*_tmp34B[1U];_tmp34B[0]=& _tmp34D;({struct _fat_ptr _tmp724=({const char*_tmp34C="Absynpp::primapp2doc: %s with no args";_tag_fat(_tmp34C,sizeof(char),38U);});Cyc_aprintf(_tmp724,_tag_fat(_tmp34B,sizeof(void*),1U));});});_tmp34E->f1=_tmp725;});_tmp34E;}));else{
# 1356
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp34F[3U];_tmp34F[0]=ps,({struct Cyc_PP_Doc*_tmp726=Cyc_PP_text(({const char*_tmp350=" ";_tag_fat(_tmp350,sizeof(char),2U);}));_tmp34F[1]=_tmp726;}),_tmp34F[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_tmp34F,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp729=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp353=({struct Cyc_String_pa_PrintArg_struct _tmp580;_tmp580.tag=0U,({struct _fat_ptr _tmp727=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp580.f1=_tmp727;});_tmp580;});void*_tmp351[1U];_tmp351[0]=& _tmp353;({struct _fat_ptr _tmp728=({const char*_tmp352="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_tmp352,sizeof(char),47U);});Cyc_aprintf(_tmp728,_tag_fat(_tmp351,sizeof(void*),1U));});});_tmp354->f1=_tmp729;});_tmp354;}));else{
# 1362
return({struct Cyc_PP_Doc*_tmp355[5U];_tmp355[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp72B=Cyc_PP_text(({const char*_tmp356=" ";_tag_fat(_tmp356,sizeof(char),2U);}));_tmp355[1]=_tmp72B;}),_tmp355[2]=ps,({struct Cyc_PP_Doc*_tmp72A=Cyc_PP_text(({const char*_tmp357=" ";_tag_fat(_tmp357,sizeof(char),2U);}));_tmp355[3]=_tmp72A;}),_tmp355[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_tmp355,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1366
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp358=p;switch(_tmp358){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp359="+";_tag_fat(_tmp359,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp35A="*";_tag_fat(_tmp35A,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp35B="-";_tag_fat(_tmp35B,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp35C="/";_tag_fat(_tmp35C,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp35D="\\%";_tag_fat(_tmp35D,sizeof(char),3U);}):({const char*_tmp35E="%";_tag_fat(_tmp35E,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp35F="==";_tag_fat(_tmp35F,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp360="!=";_tag_fat(_tmp360,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp361=">";_tag_fat(_tmp361,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp362="<";_tag_fat(_tmp362,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp363=">=";_tag_fat(_tmp363,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp364="<=";_tag_fat(_tmp364,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp365="!";_tag_fat(_tmp365,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp366="~";_tag_fat(_tmp366,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp367="&";_tag_fat(_tmp367,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp368="|";_tag_fat(_tmp368,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp369="^";_tag_fat(_tmp369,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp36A="<<";_tag_fat(_tmp36A,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp36B=">>";_tag_fat(_tmp36B,sizeof(char),3U);});case Cyc_Absyn_Numelts: _LL25: _LL26:
 return({const char*_tmp36C="numelts";_tag_fat(_tmp36C,sizeof(char),8U);});default: _LL27: _LL28:
 return({const char*_tmp36D="?";_tag_fat(_tmp36D,sizeof(char),2U);});}_LL0:;}
# 1391
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1395
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp36E=s->r;void*_stmttmp17=_tmp36E;void*_tmp36F=_stmttmp17;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp36F)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1410 "absynpp.cyc"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_tmp370=d->r;void*_stmttmp18=_tmp370;void*_tmp371=_stmttmp18;struct Cyc_Absyn_Vardecl*_tmp372;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp371)->tag == 0U){_LL1: _tmp372=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp371)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp372;
# 1413
if(((int(*)(int(*compare)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*l,struct _tuple0*x))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple14 _tmp581;_tmp581.f1=1,({struct Cyc_List_List*_tmp72C=({struct Cyc_List_List*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->hd=vd->name,_tmp373->tl=0;_tmp373;});_tmp581.f2=_tmp72C;});_tmp581;});else{
# 1416
return({struct _tuple14 _tmp582;_tmp582.f1=0,({struct Cyc_List_List*_tmp72D=({struct Cyc_List_List*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->hd=vd->name,_tmp374->tl=varsinblock;_tmp374;});_tmp582.f2=_tmp72D;});_tmp582;});}}}else{_LL3: _LL4:
# 1418
 return({struct _tuple14 _tmp583;_tmp583.f1=0,_tmp583.f2=varsinblock;_tmp583;});}_LL0:;}
# 1422
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_tmp375[8U];({struct Cyc_PP_Doc*_tmp735=Cyc_PP_text(({const char*_tmp376="(";_tag_fat(_tmp376,sizeof(char),2U);}));_tmp375[0]=_tmp735;}),({struct Cyc_PP_Doc*_tmp734=Cyc_Absynpp_lb();_tmp375[1]=_tmp734;}),({struct Cyc_PP_Doc*_tmp733=Cyc_PP_blank_doc();_tmp375[2]=_tmp733;}),({struct Cyc_PP_Doc*_tmp732=Cyc_PP_nest(2,d);_tmp375[3]=_tmp732;}),({struct Cyc_PP_Doc*_tmp731=Cyc_PP_line_doc();_tmp375[4]=_tmp731;}),({struct Cyc_PP_Doc*_tmp730=Cyc_Absynpp_rb();_tmp375[5]=_tmp730;}),({
struct Cyc_PP_Doc*_tmp72F=Cyc_PP_text(({const char*_tmp377=");";_tag_fat(_tmp377,sizeof(char),3U);}));_tmp375[6]=_tmp72F;}),({struct Cyc_PP_Doc*_tmp72E=Cyc_PP_line_doc();_tmp375[7]=_tmp72E;});Cyc_PP_cat(_tag_fat(_tmp375,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1427
return({struct Cyc_PP_Doc*_tmp378[6U];({struct Cyc_PP_Doc*_tmp73B=Cyc_Absynpp_lb();_tmp378[0]=_tmp73B;}),({struct Cyc_PP_Doc*_tmp73A=Cyc_PP_blank_doc();_tmp378[1]=_tmp73A;}),({struct Cyc_PP_Doc*_tmp739=Cyc_PP_nest(2,d);_tmp378[2]=_tmp739;}),({struct Cyc_PP_Doc*_tmp738=Cyc_PP_line_doc();_tmp378[3]=_tmp738;}),({struct Cyc_PP_Doc*_tmp737=Cyc_Absynpp_rb();_tmp378[4]=_tmp737;}),({struct Cyc_PP_Doc*_tmp736=Cyc_PP_line_doc();_tmp378[5]=_tmp736;});Cyc_PP_cat(_tag_fat(_tmp378,sizeof(struct Cyc_PP_Doc*),6U));});}}
# 1430
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_tmp379=st->r;void*_stmttmp19=_tmp379;void*_tmp37A=_stmttmp19;struct Cyc_List_List*_tmp37C;struct Cyc_Absyn_Stmt*_tmp37B;struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Stmt*_tmp37D;struct Cyc_Absyn_Stmt*_tmp380;struct _fat_ptr*_tmp37F;struct Cyc_Absyn_Stmt*_tmp382;struct Cyc_Absyn_Decl*_tmp381;struct Cyc_List_List*_tmp383;struct Cyc_List_List*_tmp385;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_Absyn_Stmt*_tmp389;struct Cyc_Absyn_Exp*_tmp388;struct Cyc_Absyn_Exp*_tmp387;struct Cyc_Absyn_Exp*_tmp386;struct _fat_ptr*_tmp38A;struct Cyc_Absyn_Stmt*_tmp38C;struct Cyc_Absyn_Exp*_tmp38B;struct Cyc_Absyn_Stmt*_tmp38F;struct Cyc_Absyn_Stmt*_tmp38E;struct Cyc_Absyn_Exp*_tmp38D;struct Cyc_Absyn_Exp*_tmp390;struct Cyc_Absyn_Stmt*_tmp392;struct Cyc_Absyn_Stmt*_tmp391;struct Cyc_Absyn_Exp*_tmp393;switch(*((int*)_tmp37A)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp394=";";_tag_fat(_tmp394,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp393=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp393;
s=({struct Cyc_PP_Doc*_tmp395[2U];({struct Cyc_PP_Doc*_tmp73D=Cyc_Absynpp_exp2doc(e);_tmp395[0]=_tmp73D;}),({struct Cyc_PP_Doc*_tmp73C=Cyc_PP_text(({const char*_tmp396=";";_tag_fat(_tmp396,sizeof(char),2U);}));_tmp395[1]=_tmp73C;});Cyc_PP_cat(_tag_fat(_tmp395,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 2U: _LL5: _tmp391=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp392=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp391;struct Cyc_Absyn_Stmt*s2=_tmp392;
# 1436
if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp397[2U];({struct Cyc_PP_Doc*_tmp741=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp397[0]=_tmp741;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_tmp740=({int _tmp73F=stmtexp;Cyc_Absynpp_block(_tmp73F,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp397[1]=_tmp740;}):({
struct Cyc_PP_Doc*_tmp73E=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp397[1]=_tmp73E;});Cyc_PP_cat(_tag_fat(_tmp397,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(Cyc_Absynpp_is_declaration(s2))
s=({struct Cyc_PP_Doc*_tmp398[3U];({struct Cyc_PP_Doc*_tmp745=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp398[0]=_tmp745;}),({
struct Cyc_PP_Doc*_tmp744=Cyc_PP_line_doc();_tmp398[1]=_tmp744;}),({
struct Cyc_PP_Doc*_tmp743=({int _tmp742=stmtexp;Cyc_Absynpp_block(_tmp742,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp398[2]=_tmp743;});Cyc_PP_cat(_tag_fat(_tmp398,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1447
s=({struct Cyc_PP_Doc*_tmp399[3U];({struct Cyc_PP_Doc*_tmp748=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp399[0]=_tmp748;}),({struct Cyc_PP_Doc*_tmp747=Cyc_PP_line_doc();_tmp399[1]=_tmp747;}),({
struct Cyc_PP_Doc*_tmp746=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp399[2]=_tmp746;});Cyc_PP_cat(_tag_fat(_tmp399,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1451
s=({struct Cyc_PP_Doc*_tmp39A[3U];({struct Cyc_PP_Doc*_tmp74B=Cyc_Absynpp_stmt2doc(s1,0,0);_tmp39A[0]=_tmp74B;}),({struct Cyc_PP_Doc*_tmp74A=Cyc_PP_line_doc();_tmp39A[1]=_tmp74A;}),({struct Cyc_PP_Doc*_tmp749=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_tmp39A[2]=_tmp749;});Cyc_PP_cat(_tag_fat(_tmp39A,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 3U: _LL7: _tmp390=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp390;
# 1454
if(eopt == 0)
s=Cyc_PP_text(({const char*_tmp39B="return;";_tag_fat(_tmp39B,sizeof(char),8U);}));else{
# 1457
s=({struct Cyc_PP_Doc*_tmp39C[3U];({struct Cyc_PP_Doc*_tmp74F=Cyc_PP_text(({const char*_tmp39D="return ";_tag_fat(_tmp39D,sizeof(char),8U);}));_tmp39C[0]=_tmp74F;}),
eopt == 0?({struct Cyc_PP_Doc*_tmp74E=Cyc_PP_nil_doc();_tmp39C[1]=_tmp74E;}):({struct Cyc_PP_Doc*_tmp74D=Cyc_Absynpp_exp2doc(eopt);_tmp39C[1]=_tmp74D;}),({
struct Cyc_PP_Doc*_tmp74C=Cyc_PP_text(({const char*_tmp39E=";";_tag_fat(_tmp39E,sizeof(char),2U);}));_tmp39C[2]=_tmp74C;});Cyc_PP_cat(_tag_fat(_tmp39C,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 4U: _LL9: _tmp38D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp38E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_tmp38F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37A)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp38D;struct Cyc_Absyn_Stmt*s1=_tmp38E;struct Cyc_Absyn_Stmt*s2=_tmp38F;
# 1462
int print_else;
{void*_tmp39F=s2->r;void*_stmttmp1A=_tmp39F;void*_tmp3A0=_stmttmp1A;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3A0)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1467
s=({struct Cyc_PP_Doc*_tmp3A1[5U];({struct Cyc_PP_Doc*_tmp758=Cyc_PP_text(({const char*_tmp3A2="if (";_tag_fat(_tmp3A2,sizeof(char),5U);}));_tmp3A1[0]=_tmp758;}),({
struct Cyc_PP_Doc*_tmp757=Cyc_Absynpp_exp2doc(e);_tmp3A1[1]=_tmp757;}),({
struct Cyc_PP_Doc*_tmp756=Cyc_PP_text(({const char*_tmp3A3=") ";_tag_fat(_tmp3A3,sizeof(char),3U);}));_tmp3A1[2]=_tmp756;}),({
struct Cyc_PP_Doc*_tmp755=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3A1[3]=_tmp755;}),
print_else?({
struct Cyc_PP_Doc*_tmp754=({struct Cyc_PP_Doc*_tmp3A4[3U];({struct Cyc_PP_Doc*_tmp753=Cyc_PP_line_doc();_tmp3A4[0]=_tmp753;}),({
struct Cyc_PP_Doc*_tmp752=Cyc_PP_text(({const char*_tmp3A5="else ";_tag_fat(_tmp3A5,sizeof(char),6U);}));_tmp3A4[1]=_tmp752;}),({
struct Cyc_PP_Doc*_tmp751=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_tmp3A4[2]=_tmp751;});Cyc_PP_cat(_tag_fat(_tmp3A4,sizeof(struct Cyc_PP_Doc*),3U));});
# 1472
_tmp3A1[4]=_tmp754;}):({
# 1475
struct Cyc_PP_Doc*_tmp750=Cyc_PP_nil_doc();_tmp3A1[4]=_tmp750;});Cyc_PP_cat(_tag_fat(_tmp3A1,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp38B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1).f1;_tmp38C=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp38B;struct Cyc_Absyn_Stmt*s1=_tmp38C;
# 1478
s=({struct Cyc_PP_Doc*_tmp3A6[4U];({struct Cyc_PP_Doc*_tmp75C=Cyc_PP_text(({const char*_tmp3A7="while (";_tag_fat(_tmp3A7,sizeof(char),8U);}));_tmp3A6[0]=_tmp75C;}),({
struct Cyc_PP_Doc*_tmp75B=Cyc_Absynpp_exp2doc(e);_tmp3A6[1]=_tmp75B;}),({
struct Cyc_PP_Doc*_tmp75A=Cyc_PP_text(({const char*_tmp3A8=") ";_tag_fat(_tmp3A8,sizeof(char),3U);}));_tmp3A6[2]=_tmp75A;}),({
struct Cyc_PP_Doc*_tmp759=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3A6[3]=_tmp759;});Cyc_PP_cat(_tag_fat(_tmp3A6,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp3A9="break;";_tag_fat(_tmp3A9,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp3AA="continue;";_tag_fat(_tmp3AA,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp38A=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_LL12: {struct _fat_ptr*x=_tmp38A;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AD=({struct Cyc_String_pa_PrintArg_struct _tmp584;_tmp584.tag=0U,_tmp584.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp584;});void*_tmp3AB[1U];_tmp3AB[0]=& _tmp3AD;({struct _fat_ptr _tmp75D=({const char*_tmp3AC="goto %s;";_tag_fat(_tmp3AC,sizeof(char),9U);});Cyc_aprintf(_tmp75D,_tag_fat(_tmp3AB,sizeof(void*),1U));});}));goto _LL0;}case 9U: _LL13: _tmp386=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp387=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2).f1;_tmp388=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37A)->f3).f1;_tmp389=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37A)->f4;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp386;struct Cyc_Absyn_Exp*e2=_tmp387;struct Cyc_Absyn_Exp*e3=_tmp388;struct Cyc_Absyn_Stmt*s1=_tmp389;
# 1487
s=({struct Cyc_PP_Doc*_tmp3AE[8U];({struct Cyc_PP_Doc*_tmp765=Cyc_PP_text(({const char*_tmp3AF="for(";_tag_fat(_tmp3AF,sizeof(char),5U);}));_tmp3AE[0]=_tmp765;}),({
struct Cyc_PP_Doc*_tmp764=Cyc_Absynpp_exp2doc(e1);_tmp3AE[1]=_tmp764;}),({
struct Cyc_PP_Doc*_tmp763=Cyc_PP_text(({const char*_tmp3B0="; ";_tag_fat(_tmp3B0,sizeof(char),3U);}));_tmp3AE[2]=_tmp763;}),({
struct Cyc_PP_Doc*_tmp762=Cyc_Absynpp_exp2doc(e2);_tmp3AE[3]=_tmp762;}),({
struct Cyc_PP_Doc*_tmp761=Cyc_PP_text(({const char*_tmp3B1="; ";_tag_fat(_tmp3B1,sizeof(char),3U);}));_tmp3AE[4]=_tmp761;}),({
struct Cyc_PP_Doc*_tmp760=Cyc_Absynpp_exp2doc(e3);_tmp3AE[5]=_tmp760;}),({
struct Cyc_PP_Doc*_tmp75F=Cyc_PP_text(({const char*_tmp3B2=") ";_tag_fat(_tmp3B2,sizeof(char),3U);}));_tmp3AE[6]=_tmp75F;}),({
struct Cyc_PP_Doc*_tmp75E=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3AE[7]=_tmp75E;});Cyc_PP_cat(_tag_fat(_tmp3AE,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 10U: _LL15: _tmp384=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp385=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp384;struct Cyc_List_List*ss=_tmp385;
# 1497
s=({struct Cyc_PP_Doc*_tmp3B3[8U];({struct Cyc_PP_Doc*_tmp76D=Cyc_PP_text(({const char*_tmp3B4="switch (";_tag_fat(_tmp3B4,sizeof(char),9U);}));_tmp3B3[0]=_tmp76D;}),({
struct Cyc_PP_Doc*_tmp76C=Cyc_Absynpp_exp2doc(e);_tmp3B3[1]=_tmp76C;}),({
struct Cyc_PP_Doc*_tmp76B=Cyc_PP_text(({const char*_tmp3B5=") ";_tag_fat(_tmp3B5,sizeof(char),3U);}));_tmp3B3[2]=_tmp76B;}),({
struct Cyc_PP_Doc*_tmp76A=Cyc_Absynpp_lb();_tmp3B3[3]=_tmp76A;}),({
struct Cyc_PP_Doc*_tmp769=Cyc_PP_line_doc();_tmp3B3[4]=_tmp769;}),({
struct Cyc_PP_Doc*_tmp768=Cyc_Absynpp_switchclauses2doc(ss);_tmp3B3[5]=_tmp768;}),({
struct Cyc_PP_Doc*_tmp767=Cyc_PP_line_doc();_tmp3B3[6]=_tmp767;}),({
struct Cyc_PP_Doc*_tmp766=Cyc_Absynpp_rb();_tmp3B3[7]=_tmp766;});Cyc_PP_cat(_tag_fat(_tmp3B3,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp3B6="fallthru;";_tag_fat(_tmp3B6,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp383=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_LL1A: {struct Cyc_List_List*es=_tmp383;
# 1508
s=({struct Cyc_PP_Doc*_tmp3B7[3U];({struct Cyc_PP_Doc*_tmp770=Cyc_PP_text(({const char*_tmp3B8="fallthru(";_tag_fat(_tmp3B8,sizeof(char),10U);}));_tmp3B7[0]=_tmp770;}),({struct Cyc_PP_Doc*_tmp76F=Cyc_Absynpp_exps2doc_prec(20,es);_tmp3B7[1]=_tmp76F;}),({struct Cyc_PP_Doc*_tmp76E=Cyc_PP_text(({const char*_tmp3B9=");";_tag_fat(_tmp3B9,sizeof(char),3U);}));_tmp3B7[2]=_tmp76E;});Cyc_PP_cat(_tag_fat(_tmp3B7,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 12U: _LL1B: _tmp381=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp382=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp381;struct Cyc_Absyn_Stmt*s1=_tmp382;
# 1510
struct _tuple14 _tmp3BA=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple14 _stmttmp1B=_tmp3BA;struct _tuple14 _tmp3BB=_stmttmp1B;struct Cyc_List_List*_tmp3BD;int _tmp3BC;_LL29: _tmp3BC=_tmp3BB.f1;_tmp3BD=_tmp3BB.f2;_LL2A: {int newblock=_tmp3BC;struct Cyc_List_List*newvarsinblock=_tmp3BD;
s=({struct Cyc_PP_Doc*_tmp3BE[3U];({struct Cyc_PP_Doc*_tmp773=Cyc_Absynpp_decl2doc(d);_tmp3BE[0]=_tmp773;}),({struct Cyc_PP_Doc*_tmp772=Cyc_PP_line_doc();_tmp3BE[1]=_tmp772;}),({struct Cyc_PP_Doc*_tmp771=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_tmp3BE[2]=_tmp771;});Cyc_PP_cat(_tag_fat(_tmp3BE,sizeof(struct Cyc_PP_Doc*),3U));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}}case 13U: _LL1D: _tmp37F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp380=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL1E: {struct _fat_ptr*x=_tmp37F;struct Cyc_Absyn_Stmt*s1=_tmp380;
# 1515
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp3BF[3U];({struct Cyc_PP_Doc*_tmp777=Cyc_PP_textptr(x);_tmp3BF[0]=_tmp777;}),({struct Cyc_PP_Doc*_tmp776=Cyc_PP_text(({const char*_tmp3C0=": ";_tag_fat(_tmp3C0,sizeof(char),3U);}));_tmp3BF[1]=_tmp776;}),({struct Cyc_PP_Doc*_tmp775=({int _tmp774=stmtexp;Cyc_Absynpp_block(_tmp774,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_tmp3BF[2]=_tmp775;});Cyc_PP_cat(_tag_fat(_tmp3BF,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1518
s=({struct Cyc_PP_Doc*_tmp3C1[3U];({struct Cyc_PP_Doc*_tmp77A=Cyc_PP_textptr(x);_tmp3C1[0]=_tmp77A;}),({struct Cyc_PP_Doc*_tmp779=Cyc_PP_text(({const char*_tmp3C2=": ";_tag_fat(_tmp3C2,sizeof(char),3U);}));_tmp3C1[1]=_tmp779;}),({struct Cyc_PP_Doc*_tmp778=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_tmp3C1[2]=_tmp778;});Cyc_PP_cat(_tag_fat(_tmp3C1,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 14U: _LL1F: _tmp37D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp37E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2).f1;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp37D;struct Cyc_Absyn_Exp*e=_tmp37E;
# 1521
s=({struct Cyc_PP_Doc*_tmp3C3[5U];({struct Cyc_PP_Doc*_tmp77F=Cyc_PP_text(({const char*_tmp3C4="do ";_tag_fat(_tmp3C4,sizeof(char),4U);}));_tmp3C3[0]=_tmp77F;}),({
struct Cyc_PP_Doc*_tmp77E=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3C3[1]=_tmp77E;}),({
struct Cyc_PP_Doc*_tmp77D=Cyc_PP_text(({const char*_tmp3C5=" while (";_tag_fat(_tmp3C5,sizeof(char),9U);}));_tmp3C3[2]=_tmp77D;}),({
struct Cyc_PP_Doc*_tmp77C=Cyc_Absynpp_exp2doc(e);_tmp3C3[3]=_tmp77C;}),({
struct Cyc_PP_Doc*_tmp77B=Cyc_PP_text(({const char*_tmp3C6=");";_tag_fat(_tmp3C6,sizeof(char),3U);}));_tmp3C3[4]=_tmp77B;});Cyc_PP_cat(_tag_fat(_tmp3C3,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}default: _LL21: _tmp37B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp37C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL22: {struct Cyc_Absyn_Stmt*s1=_tmp37B;struct Cyc_List_List*ss=_tmp37C;
# 1528
s=({struct Cyc_PP_Doc*_tmp3C7[4U];({struct Cyc_PP_Doc*_tmp783=Cyc_PP_text(({const char*_tmp3C8="try ";_tag_fat(_tmp3C8,sizeof(char),5U);}));_tmp3C7[0]=_tmp783;}),({
struct Cyc_PP_Doc*_tmp782=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3C7[1]=_tmp782;}),({
struct Cyc_PP_Doc*_tmp781=Cyc_PP_text(({const char*_tmp3C9=" catch ";_tag_fat(_tmp3C9,sizeof(char),8U);}));_tmp3C7[2]=_tmp781;}),({
struct Cyc_PP_Doc*_tmp780=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_tmp3C7[3]=_tmp780;});Cyc_PP_cat(_tag_fat(_tmp3C7,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}_LL0:;}
# 1534
return s;}
# 1537
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3CA=p->r;void*_stmttmp1C=_tmp3CA;void*_tmp3CB=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp3CC;int _tmp3CF;struct Cyc_List_List*_tmp3CE;struct Cyc_Absyn_Datatypefield*_tmp3CD;struct Cyc_Absyn_Datatypefield*_tmp3D0;struct Cyc_Absyn_Enumfield*_tmp3D1;struct Cyc_Absyn_Enumfield*_tmp3D2;int _tmp3D5;struct Cyc_List_List*_tmp3D4;struct Cyc_List_List*_tmp3D3;int _tmp3D9;struct Cyc_List_List*_tmp3D8;struct Cyc_List_List*_tmp3D7;union Cyc_Absyn_AggrInfo _tmp3D6;int _tmp3DC;struct Cyc_List_List*_tmp3DB;struct _tuple0*_tmp3DA;struct _tuple0*_tmp3DD;struct Cyc_Absyn_Pat*_tmp3DF;struct Cyc_Absyn_Vardecl*_tmp3DE;struct Cyc_Absyn_Vardecl*_tmp3E0;struct Cyc_Absyn_Pat*_tmp3E1;int _tmp3E3;struct Cyc_List_List*_tmp3E2;struct Cyc_Absyn_Vardecl*_tmp3E5;struct Cyc_Absyn_Tvar*_tmp3E4;struct Cyc_Absyn_Vardecl*_tmp3E7;struct Cyc_Absyn_Tvar*_tmp3E6;struct Cyc_Absyn_Pat*_tmp3E9;struct Cyc_Absyn_Vardecl*_tmp3E8;struct Cyc_Absyn_Vardecl*_tmp3EA;struct _fat_ptr _tmp3EB;char _tmp3EC;int _tmp3EE;enum Cyc_Absyn_Sign _tmp3ED;switch(*((int*)_tmp3CB)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3EF="_";_tag_fat(_tmp3EF,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp3F0="NULL";_tag_fat(_tmp3F0,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp3ED=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp3EE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp3ED;int i=_tmp3EE;
# 1543
if((int)sg != (int)1U)
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3F3=({struct Cyc_Int_pa_PrintArg_struct _tmp585;_tmp585.tag=1U,_tmp585.f1=(unsigned long)i;_tmp585;});void*_tmp3F1[1U];_tmp3F1[0]=& _tmp3F3;({struct _fat_ptr _tmp784=({const char*_tmp3F2="%d";_tag_fat(_tmp3F2,sizeof(char),3U);});Cyc_aprintf(_tmp784,_tag_fat(_tmp3F1,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3F6=({struct Cyc_Int_pa_PrintArg_struct _tmp586;_tmp586.tag=1U,_tmp586.f1=(unsigned)i;_tmp586;});void*_tmp3F4[1U];_tmp3F4[0]=& _tmp3F6;({struct _fat_ptr _tmp785=({const char*_tmp3F5="%u";_tag_fat(_tmp3F5,sizeof(char),3U);});Cyc_aprintf(_tmp785,_tag_fat(_tmp3F4,sizeof(void*),1U));});}));}
goto _LL0;}case 11U: _LL7: _tmp3EC=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL8: {char ch=_tmp3EC;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3F9=({struct Cyc_String_pa_PrintArg_struct _tmp587;_tmp587.tag=0U,({struct _fat_ptr _tmp786=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp587.f1=_tmp786;});_tmp587;});void*_tmp3F7[1U];_tmp3F7[0]=& _tmp3F9;({struct _fat_ptr _tmp787=({const char*_tmp3F8="'%s'";_tag_fat(_tmp3F8,sizeof(char),5U);});Cyc_aprintf(_tmp787,_tag_fat(_tmp3F7,sizeof(void*),1U));});}));goto _LL0;}case 12U: _LL9: _tmp3EB=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LLA: {struct _fat_ptr x=_tmp3EB;
s=Cyc_PP_text(x);goto _LL0;}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2)->r)->tag == 0U){_LLB: _tmp3EA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp3EA;
# 1550
s=Cyc_Absynpp_qvar2doc(vd->name);goto _LL0;}}else{_LLD: _tmp3E8=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp3E9=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp3E8;struct Cyc_Absyn_Pat*p2=_tmp3E9;
# 1552
s=({struct Cyc_PP_Doc*_tmp3FA[3U];({struct Cyc_PP_Doc*_tmp78A=Cyc_Absynpp_qvar2doc(vd->name);_tmp3FA[0]=_tmp78A;}),({struct Cyc_PP_Doc*_tmp789=Cyc_PP_text(({const char*_tmp3FB=" as ";_tag_fat(_tmp3FB,sizeof(char),5U);}));_tmp3FA[1]=_tmp789;}),({struct Cyc_PP_Doc*_tmp788=Cyc_Absynpp_pat2doc(p2);_tmp3FA[2]=_tmp788;});Cyc_PP_cat(_tag_fat(_tmp3FA,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 2U: _LLF: _tmp3E6=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp3E7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp3E6;struct Cyc_Absyn_Vardecl*vd=_tmp3E7;
# 1554
s=({struct Cyc_PP_Doc*_tmp3FC[5U];({struct Cyc_PP_Doc*_tmp78F=Cyc_PP_text(({const char*_tmp3FD="alias";_tag_fat(_tmp3FD,sizeof(char),6U);}));_tmp3FC[0]=_tmp78F;}),({struct Cyc_PP_Doc*_tmp78E=Cyc_PP_text(({const char*_tmp3FE=" <";_tag_fat(_tmp3FE,sizeof(char),3U);}));_tmp3FC[1]=_tmp78E;}),({struct Cyc_PP_Doc*_tmp78D=Cyc_Absynpp_tvar2doc(tv);_tmp3FC[2]=_tmp78D;}),({struct Cyc_PP_Doc*_tmp78C=Cyc_PP_text(({const char*_tmp3FF="> ";_tag_fat(_tmp3FF,sizeof(char),3U);}));_tmp3FC[3]=_tmp78C;}),({
struct Cyc_PP_Doc*_tmp78B=Cyc_Absynpp_vardecl2doc(vd);_tmp3FC[4]=_tmp78B;});Cyc_PP_cat(_tag_fat(_tmp3FC,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 4U: _LL11: _tmp3E4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp3E5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp3E4;struct Cyc_Absyn_Vardecl*vd=_tmp3E5;
# 1558
s=({struct Cyc_PP_Doc*_tmp400[4U];({struct Cyc_PP_Doc*_tmp793=Cyc_Absynpp_qvar2doc(vd->name);_tmp400[0]=_tmp793;}),({struct Cyc_PP_Doc*_tmp792=Cyc_PP_text(({const char*_tmp401="<";_tag_fat(_tmp401,sizeof(char),2U);}));_tmp400[1]=_tmp792;}),({struct Cyc_PP_Doc*_tmp791=Cyc_Absynpp_tvar2doc(tv);_tmp400[2]=_tmp791;}),({struct Cyc_PP_Doc*_tmp790=Cyc_PP_text(({const char*_tmp402=">";_tag_fat(_tmp402,sizeof(char),2U);}));_tmp400[3]=_tmp790;});Cyc_PP_cat(_tag_fat(_tmp400,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 5U: _LL13: _tmp3E2=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp3E3=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL14: {struct Cyc_List_List*ts=_tmp3E2;int dots=_tmp3E3;
# 1561
s=({struct Cyc_PP_Doc*_tmp403[4U];({struct Cyc_PP_Doc*_tmp799=Cyc_Absynpp_dollar();_tmp403[0]=_tmp799;}),({struct Cyc_PP_Doc*_tmp798=Cyc_PP_text(({const char*_tmp404="(";_tag_fat(_tmp404,sizeof(char),2U);}));_tmp403[1]=_tmp798;}),({struct Cyc_PP_Doc*_tmp797=({struct _fat_ptr _tmp796=({const char*_tmp405=",";_tag_fat(_tmp405,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp796,ts);});_tmp403[2]=_tmp797;}),
dots?({struct Cyc_PP_Doc*_tmp795=Cyc_PP_text(({const char*_tmp406=", ...)";_tag_fat(_tmp406,sizeof(char),7U);}));_tmp403[3]=_tmp795;}):({struct Cyc_PP_Doc*_tmp794=Cyc_PP_text(({const char*_tmp407=")";_tag_fat(_tmp407,sizeof(char),2U);}));_tmp403[3]=_tmp794;});Cyc_PP_cat(_tag_fat(_tmp403,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LL15: _tmp3E1=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL16: {struct Cyc_Absyn_Pat*p2=_tmp3E1;
# 1565
s=({struct Cyc_PP_Doc*_tmp408[2U];({struct Cyc_PP_Doc*_tmp79B=Cyc_PP_text(({const char*_tmp409="&";_tag_fat(_tmp409,sizeof(char),2U);}));_tmp408[0]=_tmp79B;}),({struct Cyc_PP_Doc*_tmp79A=Cyc_Absynpp_pat2doc(p2);_tmp408[1]=_tmp79A;});Cyc_PP_cat(_tag_fat(_tmp408,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2)->r)->tag == 0U){_LL17: _tmp3E0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp3E0;
# 1568
s=({struct Cyc_PP_Doc*_tmp40A[2U];({struct Cyc_PP_Doc*_tmp79D=Cyc_PP_text(({const char*_tmp40B="*";_tag_fat(_tmp40B,sizeof(char),2U);}));_tmp40A[0]=_tmp79D;}),({struct Cyc_PP_Doc*_tmp79C=Cyc_Absynpp_qvar2doc(vd->name);_tmp40A[1]=_tmp79C;});Cyc_PP_cat(_tag_fat(_tmp40A,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}else{_LL19: _tmp3DE=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp3DF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp3DE;struct Cyc_Absyn_Pat*p2=_tmp3DF;
# 1571
s=({struct Cyc_PP_Doc*_tmp40C[4U];({struct Cyc_PP_Doc*_tmp7A1=Cyc_PP_text(({const char*_tmp40D="*";_tag_fat(_tmp40D,sizeof(char),2U);}));_tmp40C[0]=_tmp7A1;}),({struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_qvar2doc(vd->name);_tmp40C[1]=_tmp7A0;}),({struct Cyc_PP_Doc*_tmp79F=Cyc_PP_text(({const char*_tmp40E=" as ";_tag_fat(_tmp40E,sizeof(char),5U);}));_tmp40C[2]=_tmp79F;}),({struct Cyc_PP_Doc*_tmp79E=Cyc_Absynpp_pat2doc(p2);_tmp40C[3]=_tmp79E;});Cyc_PP_cat(_tag_fat(_tmp40C,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}case 15U: _LL1B: _tmp3DD=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL1C: {struct _tuple0*q=_tmp3DD;
# 1574
s=Cyc_Absynpp_qvar2doc(q);
goto _LL0;}case 16U: _LL1D: _tmp3DA=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp3DB=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_tmp3DC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3;_LL1E: {struct _tuple0*q=_tmp3DA;struct Cyc_List_List*ps=_tmp3DB;int dots=_tmp3DC;
# 1577
struct _fat_ptr term=dots?({const char*_tmp412=", ...)";_tag_fat(_tmp412,sizeof(char),7U);}):({const char*_tmp413=")";_tag_fat(_tmp413,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp40F[2U];({struct Cyc_PP_Doc*_tmp7A6=Cyc_Absynpp_qvar2doc(q);_tmp40F[0]=_tmp7A6;}),({struct Cyc_PP_Doc*_tmp7A5=({struct _fat_ptr _tmp7A4=({const char*_tmp410="(";_tag_fat(_tmp410,sizeof(char),2U);});struct _fat_ptr _tmp7A3=term;struct _fat_ptr _tmp7A2=({const char*_tmp411=",";_tag_fat(_tmp411,sizeof(char),2U);});Cyc_PP_group(_tmp7A4,_tmp7A3,_tmp7A2,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp40F[1]=_tmp7A5;});Cyc_PP_cat(_tag_fat(_tmp40F,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1 != 0){_LL1F: _tmp3D6=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp3D7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_tmp3D8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3;_tmp3D9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f4;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp3D6;struct Cyc_List_List*exists=_tmp3D7;struct Cyc_List_List*dps=_tmp3D8;int dots=_tmp3D9;
# 1581
struct _fat_ptr term=dots?({const char*_tmp41D=", ...}";_tag_fat(_tmp41D,sizeof(char),7U);}):({const char*_tmp41E="}";_tag_fat(_tmp41E,sizeof(char),2U);});
struct _tuple11 _tmp414=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp1D=_tmp414;struct _tuple11 _tmp415=_stmttmp1D;struct _tuple0*_tmp416;_LL2E: _tmp416=_tmp415.f2;_LL2F: {struct _tuple0*n=_tmp416;
s=({struct Cyc_PP_Doc*_tmp417[4U];({struct Cyc_PP_Doc*_tmp7B0=Cyc_Absynpp_qvar2doc(n);_tmp417[0]=_tmp7B0;}),({struct Cyc_PP_Doc*_tmp7AF=Cyc_Absynpp_lb();_tmp417[1]=_tmp7AF;}),({
struct Cyc_PP_Doc*_tmp7AE=({struct _fat_ptr _tmp7AD=({const char*_tmp418="[";_tag_fat(_tmp418,sizeof(char),2U);});struct _fat_ptr _tmp7AC=({const char*_tmp419="]";_tag_fat(_tmp419,sizeof(char),2U);});struct _fat_ptr _tmp7AB=({const char*_tmp41A=",";_tag_fat(_tmp41A,sizeof(char),2U);});Cyc_PP_egroup(_tmp7AD,_tmp7AC,_tmp7AB,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp417[2]=_tmp7AE;}),({
struct Cyc_PP_Doc*_tmp7AA=({struct _fat_ptr _tmp7A9=({const char*_tmp41B="";_tag_fat(_tmp41B,sizeof(char),1U);});struct _fat_ptr _tmp7A8=term;struct _fat_ptr _tmp7A7=({const char*_tmp41C=",";_tag_fat(_tmp41C,sizeof(char),2U);});Cyc_PP_group(_tmp7A9,_tmp7A8,_tmp7A7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp417[3]=_tmp7AA;});Cyc_PP_cat(_tag_fat(_tmp417,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}}else{_LL21: _tmp3D3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_tmp3D4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3;_tmp3D5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f4;_LL22: {struct Cyc_List_List*exists=_tmp3D3;struct Cyc_List_List*dps=_tmp3D4;int dots=_tmp3D5;
# 1588
struct _fat_ptr term=dots?({const char*_tmp425=", ...}";_tag_fat(_tmp425,sizeof(char),7U);}):({const char*_tmp426="}";_tag_fat(_tmp426,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp41F[3U];({struct Cyc_PP_Doc*_tmp7B9=Cyc_Absynpp_lb();_tmp41F[0]=_tmp7B9;}),({
struct Cyc_PP_Doc*_tmp7B8=({struct _fat_ptr _tmp7B7=({const char*_tmp420="[";_tag_fat(_tmp420,sizeof(char),2U);});struct _fat_ptr _tmp7B6=({const char*_tmp421="]";_tag_fat(_tmp421,sizeof(char),2U);});struct _fat_ptr _tmp7B5=({const char*_tmp422=",";_tag_fat(_tmp422,sizeof(char),2U);});Cyc_PP_egroup(_tmp7B7,_tmp7B6,_tmp7B5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp41F[1]=_tmp7B8;}),({
struct Cyc_PP_Doc*_tmp7B4=({struct _fat_ptr _tmp7B3=({const char*_tmp423="";_tag_fat(_tmp423,sizeof(char),1U);});struct _fat_ptr _tmp7B2=term;struct _fat_ptr _tmp7B1=({const char*_tmp424=",";_tag_fat(_tmp424,sizeof(char),2U);});Cyc_PP_group(_tmp7B3,_tmp7B2,_tmp7B1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp41F[2]=_tmp7B4;});Cyc_PP_cat(_tag_fat(_tmp41F,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp3D2=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp3D2;
_tmp3D1=ef;goto _LL26;}case 14U: _LL25: _tmp3D1=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp3D1;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3 == 0){_LL27: _tmp3D0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL28: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3D0;
s=Cyc_Absynpp_qvar2doc(tuf->name);goto _LL0;}}else{_LL29: _tmp3CD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_tmp3CE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3;_tmp3CF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f4;_LL2A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3CD;struct Cyc_List_List*ps=_tmp3CE;int dots=_tmp3CF;
# 1597
struct _fat_ptr term=dots?({const char*_tmp42A=", ...)";_tag_fat(_tmp42A,sizeof(char),7U);}):({const char*_tmp42B=")";_tag_fat(_tmp42B,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp427[2U];({struct Cyc_PP_Doc*_tmp7BE=Cyc_Absynpp_qvar2doc(tuf->name);_tmp427[0]=_tmp7BE;}),({struct Cyc_PP_Doc*_tmp7BD=({struct _fat_ptr _tmp7BC=({const char*_tmp428="(";_tag_fat(_tmp428,sizeof(char),2U);});struct _fat_ptr _tmp7BB=term;struct _fat_ptr _tmp7BA=({const char*_tmp429=",";_tag_fat(_tmp429,sizeof(char),2U);});Cyc_PP_egroup(_tmp7BC,_tmp7BB,_tmp7BA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp427[1]=_tmp7BD;});Cyc_PP_cat(_tag_fat(_tmp427,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp3CC=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp3CC;
# 1601
s=Cyc_Absynpp_exp2doc(e);goto _LL0;}}_LL0:;}
# 1603
return s;}
# 1606
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp){
return({struct Cyc_PP_Doc*_tmp42C[2U];({struct Cyc_PP_Doc*_tmp7C3=({struct _fat_ptr _tmp7C2=({const char*_tmp42D="";_tag_fat(_tmp42D,sizeof(char),1U);});struct _fat_ptr _tmp7C1=({const char*_tmp42E="=";_tag_fat(_tmp42E,sizeof(char),2U);});struct _fat_ptr _tmp7C0=({const char*_tmp42F="=";_tag_fat(_tmp42F,sizeof(char),2U);});Cyc_PP_egroup(_tmp7C2,_tmp7C1,_tmp7C0,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp42C[0]=_tmp7C3;}),({
struct Cyc_PP_Doc*_tmp7BF=Cyc_Absynpp_pat2doc((*dp).f2);_tmp42C[1]=_tmp7BF;});Cyc_PP_cat(_tag_fat(_tmp42C,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1611
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1613
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp430[2U];({struct Cyc_PP_Doc*_tmp7C6=Cyc_PP_text(({const char*_tmp431="default: ";_tag_fat(_tmp431,sizeof(char),10U);}));_tmp430[0]=_tmp7C6;}),({
struct Cyc_PP_Doc*_tmp7C5=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp432[2U];({struct Cyc_PP_Doc*_tmp7C4=Cyc_PP_line_doc();_tmp432[0]=_tmp7C4;}),_tmp432[1]=body;Cyc_PP_cat(_tag_fat(_tmp432,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp430[1]=_tmp7C5;});Cyc_PP_cat(_tag_fat(_tmp430,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp433[4U];({struct Cyc_PP_Doc*_tmp7CB=Cyc_PP_text(({const char*_tmp434="case ";_tag_fat(_tmp434,sizeof(char),6U);}));_tmp433[0]=_tmp7CB;}),({
struct Cyc_PP_Doc*_tmp7CA=Cyc_Absynpp_pat2doc(c->pattern);_tmp433[1]=_tmp7CA;}),({
struct Cyc_PP_Doc*_tmp7C9=Cyc_PP_text(({const char*_tmp435=": ";_tag_fat(_tmp435,sizeof(char),3U);}));_tmp433[2]=_tmp7C9;}),({
struct Cyc_PP_Doc*_tmp7C8=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp436[2U];({struct Cyc_PP_Doc*_tmp7C7=Cyc_PP_line_doc();_tmp436[0]=_tmp7C7;}),_tmp436[1]=body;Cyc_PP_cat(_tag_fat(_tmp436,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp433[3]=_tmp7C8;});Cyc_PP_cat(_tag_fat(_tmp433,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1625
return({struct Cyc_PP_Doc*_tmp437[6U];({struct Cyc_PP_Doc*_tmp7D2=Cyc_PP_text(({const char*_tmp438="case ";_tag_fat(_tmp438,sizeof(char),6U);}));_tmp437[0]=_tmp7D2;}),({
struct Cyc_PP_Doc*_tmp7D1=Cyc_Absynpp_pat2doc(c->pattern);_tmp437[1]=_tmp7D1;}),({
struct Cyc_PP_Doc*_tmp7D0=Cyc_PP_text(({const char*_tmp439=" && ";_tag_fat(_tmp439,sizeof(char),5U);}));_tmp437[2]=_tmp7D0;}),({
struct Cyc_PP_Doc*_tmp7CF=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp437[3]=_tmp7CF;}),({
struct Cyc_PP_Doc*_tmp7CE=Cyc_PP_text(({const char*_tmp43A=": ";_tag_fat(_tmp43A,sizeof(char),3U);}));_tmp437[4]=_tmp7CE;}),({
struct Cyc_PP_Doc*_tmp7CD=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43B[2U];({struct Cyc_PP_Doc*_tmp7CC=Cyc_PP_line_doc();_tmp43B[0]=_tmp7CC;}),_tmp43B[1]=body;Cyc_PP_cat(_tag_fat(_tmp43B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp437[5]=_tmp7CD;});Cyc_PP_cat(_tag_fat(_tmp437,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1633
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _fat_ptr _tmp7D3=({const char*_tmp43C="";_tag_fat(_tmp43C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp7D3,cs);});}
# 1637
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1641
return({struct Cyc_PP_Doc*_tmp43D[3U];({struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_qvar2doc(f->name);_tmp43D[0]=_tmp7D6;}),({struct Cyc_PP_Doc*_tmp7D5=Cyc_PP_text(({const char*_tmp43E=" = ";_tag_fat(_tmp43E,sizeof(char),4U);}));_tmp43D[1]=_tmp7D5;}),({struct Cyc_PP_Doc*_tmp7D4=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp43D[2]=_tmp7D4;});Cyc_PP_cat(_tag_fat(_tmp43D,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1644
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _fat_ptr _tmp7D7=({const char*_tmp43F=",";_tag_fat(_tmp43F,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp7D7,fs);});}
# 1648
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1652
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _fat_ptr _tmp7D8=({const char*_tmp440=",";_tag_fat(_tmp440,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp7D8,vds);});}
# 1656
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp441=vd;struct Cyc_List_List*_tmp448;struct Cyc_Absyn_Exp*_tmp447;void*_tmp446;struct Cyc_Absyn_Tqual _tmp445;unsigned _tmp444;struct _tuple0*_tmp443;enum Cyc_Absyn_Scope _tmp442;_LL1: _tmp442=_tmp441->sc;_tmp443=_tmp441->name;_tmp444=_tmp441->varloc;_tmp445=_tmp441->tq;_tmp446=_tmp441->type;_tmp447=_tmp441->initializer;_tmp448=_tmp441->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp442;struct _tuple0*name=_tmp443;unsigned varloc=_tmp444;struct Cyc_Absyn_Tqual tq=_tmp445;void*type=_tmp446;struct Cyc_Absyn_Exp*initializer=_tmp447;struct Cyc_List_List*atts=_tmp448;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp449=Cyc_Cyclone_c_compiler;if(_tmp449 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1665
{void*_tmp44A=Cyc_Tcutil_compress(type);void*_stmttmp1E=_tmp44A;void*_tmp44B=_stmttmp1E;struct Cyc_List_List*_tmp44C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->tag == 5U){_LL9: _tmp44C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).attributes;_LLA: {struct Cyc_List_List*atts2=_tmp44C;
# 1667
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1671
goto _LL3;}_LL3:;}{
# 1674
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp44D=Cyc_Cyclone_c_compiler;if(_tmp44D == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1679
s=({struct Cyc_PP_Doc*_tmp44E[5U];_tmp44E[0]=tmp_doc,({
# 1681
struct Cyc_PP_Doc*_tmp7E2=Cyc_Absynpp_scope2doc(sc);_tmp44E[1]=_tmp7E2;}),({
struct Cyc_PP_Doc*_tmp7E1=({struct Cyc_Absyn_Tqual _tmp7E0=tq;void*_tmp7DF=type;Cyc_Absynpp_tqtd2doc(_tmp7E0,_tmp7DF,({struct Cyc_Core_Opt*_tmp450=_cycalloc(sizeof(*_tmp450));({struct Cyc_PP_Doc*_tmp7DE=({struct Cyc_PP_Doc*_tmp44F[2U];_tmp44F[0]=beforenamedoc,_tmp44F[1]=sn;Cyc_PP_cat(_tag_fat(_tmp44F,sizeof(struct Cyc_PP_Doc*),2U));});_tmp450->v=_tmp7DE;});_tmp450;}));});_tmp44E[2]=_tmp7E1;}),
initializer == 0?({
struct Cyc_PP_Doc*_tmp7DD=Cyc_PP_nil_doc();_tmp44E[3]=_tmp7DD;}):({
struct Cyc_PP_Doc*_tmp7DC=({struct Cyc_PP_Doc*_tmp451[2U];({struct Cyc_PP_Doc*_tmp7DB=Cyc_PP_text(({const char*_tmp452=" = ";_tag_fat(_tmp452,sizeof(char),4U);}));_tmp451[0]=_tmp7DB;}),({struct Cyc_PP_Doc*_tmp7DA=Cyc_Absynpp_exp2doc(initializer);_tmp451[1]=_tmp7DA;});Cyc_PP_cat(_tag_fat(_tmp451,sizeof(struct Cyc_PP_Doc*),2U));});_tmp44E[3]=_tmp7DC;}),({
struct Cyc_PP_Doc*_tmp7D9=Cyc_PP_text(({const char*_tmp453=";";_tag_fat(_tmp453,sizeof(char),2U);}));_tmp44E[4]=_tmp7D9;});Cyc_PP_cat(_tag_fat(_tmp44E,sizeof(struct Cyc_PP_Doc*),5U));});
return s;}}}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1690
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
struct _tuple20 _tmp454=*x;struct _tuple20 _stmttmp1F=_tmp454;struct _tuple20 _tmp455=_stmttmp1F;struct _tuple0*_tmp456;_LL1: _tmp456=_tmp455.f2;_LL2: {struct _tuple0*v=_tmp456;
return Cyc_Absynpp_qvar2doc(v);}}
# 1695
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp457[4U];({struct Cyc_PP_Doc*_tmp7E6=Cyc_Absynpp_scope2doc(ad->sc);_tmp457[0]=_tmp7E6;}),({
struct Cyc_PP_Doc*_tmp7E5=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp457[1]=_tmp7E5;}),({
struct Cyc_PP_Doc*_tmp7E4=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp457[2]=_tmp7E4;}),({
struct Cyc_PP_Doc*_tmp7E3=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp457[3]=_tmp7E3;});Cyc_PP_cat(_tag_fat(_tmp457,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1702
return({struct Cyc_PP_Doc*_tmp458[12U];({struct Cyc_PP_Doc*_tmp7F7=Cyc_Absynpp_scope2doc(ad->sc);_tmp458[0]=_tmp7F7;}),({
struct Cyc_PP_Doc*_tmp7F6=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp458[1]=_tmp7F6;}),({
struct Cyc_PP_Doc*_tmp7F5=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp458[2]=_tmp7F5;}),({
struct Cyc_PP_Doc*_tmp7F4=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp458[3]=_tmp7F4;}),({
struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_blank_doc();_tmp458[4]=_tmp7F3;}),({struct Cyc_PP_Doc*_tmp7F2=Cyc_Absynpp_lb();_tmp458[5]=_tmp7F2;}),({
struct Cyc_PP_Doc*_tmp7F1=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp458[6]=_tmp7F1;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_nil_doc();_tmp458[7]=_tmp7F0;}):({
struct Cyc_PP_Doc*_tmp7EF=({struct Cyc_PP_Doc*_tmp459[2U];({struct Cyc_PP_Doc*_tmp7EE=Cyc_PP_text(({const char*_tmp45A=":";_tag_fat(_tmp45A,sizeof(char),2U);}));_tmp459[0]=_tmp7EE;}),({struct Cyc_PP_Doc*_tmp7ED=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp459[1]=_tmp7ED;});Cyc_PP_cat(_tag_fat(_tmp459,sizeof(struct Cyc_PP_Doc*),2U));});_tmp458[7]=_tmp7EF;}),({
struct Cyc_PP_Doc*_tmp7EC=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp45B[2U];({struct Cyc_PP_Doc*_tmp7EB=Cyc_PP_line_doc();_tmp45B[0]=_tmp7EB;}),({struct Cyc_PP_Doc*_tmp7EA=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp45B[1]=_tmp7EA;});Cyc_PP_cat(_tag_fat(_tmp45B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp458[8]=_tmp7EC;}),({
struct Cyc_PP_Doc*_tmp7E9=Cyc_PP_line_doc();_tmp458[9]=_tmp7E9;}),({
struct Cyc_PP_Doc*_tmp7E8=Cyc_Absynpp_rb();_tmp458[10]=_tmp7E8;}),({
struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_atts2doc(ad->attributes);_tmp458[11]=_tmp7E7;});Cyc_PP_cat(_tag_fat(_tmp458,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1716
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp45C=dd;int _tmp461;struct Cyc_Core_Opt*_tmp460;struct Cyc_List_List*_tmp45F;struct _tuple0*_tmp45E;enum Cyc_Absyn_Scope _tmp45D;_LL1: _tmp45D=_tmp45C->sc;_tmp45E=_tmp45C->name;_tmp45F=_tmp45C->tvs;_tmp460=_tmp45C->fields;_tmp461=_tmp45C->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp45D;struct _tuple0*name=_tmp45E;struct Cyc_List_List*tvs=_tmp45F;struct Cyc_Core_Opt*fields=_tmp460;int is_x=_tmp461;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp462[5U];({struct Cyc_PP_Doc*_tmp7FE=Cyc_Absynpp_scope2doc(sc);_tmp462[0]=_tmp7FE;}),
is_x?({struct Cyc_PP_Doc*_tmp7FD=Cyc_PP_text(({const char*_tmp463="@extensible ";_tag_fat(_tmp463,sizeof(char),13U);}));_tmp462[1]=_tmp7FD;}):({struct Cyc_PP_Doc*_tmp7FC=Cyc_PP_blank_doc();_tmp462[1]=_tmp7FC;}),({
struct Cyc_PP_Doc*_tmp7FB=Cyc_PP_text(({const char*_tmp464="datatype ";_tag_fat(_tmp464,sizeof(char),10U);}));_tmp462[2]=_tmp7FB;}),
is_x?({struct Cyc_PP_Doc*_tmp7FA=Cyc_Absynpp_qvar2bolddoc(name);_tmp462[3]=_tmp7FA;}):({struct Cyc_PP_Doc*_tmp7F9=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp462[3]=_tmp7F9;}),({
struct Cyc_PP_Doc*_tmp7F8=Cyc_Absynpp_ktvars2doc(tvs);_tmp462[4]=_tmp7F8;});Cyc_PP_cat(_tag_fat(_tmp462,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1725
return({struct Cyc_PP_Doc*_tmp465[10U];({struct Cyc_PP_Doc*_tmp80C=Cyc_Absynpp_scope2doc(sc);_tmp465[0]=_tmp80C;}),
is_x?({struct Cyc_PP_Doc*_tmp80B=Cyc_PP_text(({const char*_tmp466="@extensible ";_tag_fat(_tmp466,sizeof(char),13U);}));_tmp465[1]=_tmp80B;}):({struct Cyc_PP_Doc*_tmp80A=Cyc_PP_blank_doc();_tmp465[1]=_tmp80A;}),({
struct Cyc_PP_Doc*_tmp809=Cyc_PP_text(({const char*_tmp467="datatype ";_tag_fat(_tmp467,sizeof(char),10U);}));_tmp465[2]=_tmp809;}),
is_x?({struct Cyc_PP_Doc*_tmp808=Cyc_Absynpp_qvar2bolddoc(name);_tmp465[3]=_tmp808;}):({struct Cyc_PP_Doc*_tmp807=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp465[3]=_tmp807;}),({
struct Cyc_PP_Doc*_tmp806=Cyc_Absynpp_ktvars2doc(tvs);_tmp465[4]=_tmp806;}),({
struct Cyc_PP_Doc*_tmp805=Cyc_PP_blank_doc();_tmp465[5]=_tmp805;}),({struct Cyc_PP_Doc*_tmp804=Cyc_Absynpp_lb();_tmp465[6]=_tmp804;}),({
struct Cyc_PP_Doc*_tmp803=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp468[2U];({struct Cyc_PP_Doc*_tmp802=Cyc_PP_line_doc();_tmp468[0]=_tmp802;}),({struct Cyc_PP_Doc*_tmp801=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_tmp468[1]=_tmp801;});Cyc_PP_cat(_tag_fat(_tmp468,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp465[7]=_tmp803;}),({
struct Cyc_PP_Doc*_tmp800=Cyc_PP_line_doc();_tmp465[8]=_tmp800;}),({
struct Cyc_PP_Doc*_tmp7FF=Cyc_Absynpp_rb();_tmp465[9]=_tmp7FF;});Cyc_PP_cat(_tag_fat(_tmp465,sizeof(struct Cyc_PP_Doc*),10U));});}}}
# 1736
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp469=ed;struct Cyc_Core_Opt*_tmp46C;struct _tuple0*_tmp46B;enum Cyc_Absyn_Scope _tmp46A;_LL1: _tmp46A=_tmp469->sc;_tmp46B=_tmp469->name;_tmp46C=_tmp469->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp46A;struct _tuple0*n=_tmp46B;struct Cyc_Core_Opt*fields=_tmp46C;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp46D[3U];({struct Cyc_PP_Doc*_tmp80F=Cyc_Absynpp_scope2doc(sc);_tmp46D[0]=_tmp80F;}),({
struct Cyc_PP_Doc*_tmp80E=Cyc_PP_text(({const char*_tmp46E="enum ";_tag_fat(_tmp46E,sizeof(char),6U);}));_tmp46D[1]=_tmp80E;}),({
struct Cyc_PP_Doc*_tmp80D=Cyc_Absynpp_typedef_name2bolddoc(n);_tmp46D[2]=_tmp80D;});Cyc_PP_cat(_tag_fat(_tmp46D,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1744
return({struct Cyc_PP_Doc*_tmp46F[8U];({struct Cyc_PP_Doc*_tmp819=Cyc_Absynpp_scope2doc(sc);_tmp46F[0]=_tmp819;}),({
struct Cyc_PP_Doc*_tmp818=Cyc_PP_text(({const char*_tmp470="enum ";_tag_fat(_tmp470,sizeof(char),6U);}));_tmp46F[1]=_tmp818;}),({
struct Cyc_PP_Doc*_tmp817=Cyc_Absynpp_qvar2bolddoc(n);_tmp46F[2]=_tmp817;}),({
struct Cyc_PP_Doc*_tmp816=Cyc_PP_blank_doc();_tmp46F[3]=_tmp816;}),({struct Cyc_PP_Doc*_tmp815=Cyc_Absynpp_lb();_tmp46F[4]=_tmp815;}),({
struct Cyc_PP_Doc*_tmp814=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp471[2U];({struct Cyc_PP_Doc*_tmp813=Cyc_PP_line_doc();_tmp471[0]=_tmp813;}),({struct Cyc_PP_Doc*_tmp812=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_tmp471[1]=_tmp812;});Cyc_PP_cat(_tag_fat(_tmp471,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp46F[5]=_tmp814;}),({
struct Cyc_PP_Doc*_tmp811=Cyc_PP_line_doc();_tmp46F[6]=_tmp811;}),({
struct Cyc_PP_Doc*_tmp810=Cyc_Absynpp_rb();_tmp46F[7]=_tmp810;});Cyc_PP_cat(_tag_fat(_tmp46F,sizeof(struct Cyc_PP_Doc*),8U));});}}}
# 1753
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp472=d->r;void*_stmttmp20=_tmp472;void*_tmp473=_stmttmp20;struct _tuple10*_tmp477;struct Cyc_List_List*_tmp476;struct Cyc_List_List*_tmp475;struct Cyc_List_List*_tmp474;struct Cyc_List_List*_tmp478;struct Cyc_List_List*_tmp47A;struct _tuple0*_tmp479;struct Cyc_List_List*_tmp47C;struct _fat_ptr*_tmp47B;struct Cyc_Absyn_Typedefdecl*_tmp47D;struct Cyc_Absyn_Enumdecl*_tmp47E;struct Cyc_List_List*_tmp47F;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Pat*_tmp480;struct Cyc_Absyn_Datatypedecl*_tmp482;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Vardecl*_tmp484;struct Cyc_Absyn_Tvar*_tmp483;struct Cyc_Absyn_Vardecl*_tmp486;struct Cyc_Absyn_Aggrdecl*_tmp487;struct Cyc_Absyn_Fndecl*_tmp488;switch(*((int*)_tmp473)){case 1U: _LL1: _tmp488=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp488;
# 1757
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->tag=5U,_tmp499->f1=type_info;_tmp499;});
if(fd->cached_type != 0){
void*_tmp489=Cyc_Tcutil_compress((void*)_check_null(fd->cached_type));void*_stmttmp21=_tmp489;void*_tmp48A=_stmttmp21;struct Cyc_Absyn_FnInfo _tmp48B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48A)->tag == 5U){_LL24: _tmp48B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48A)->f1;_LL25: {struct Cyc_Absyn_FnInfo i=_tmp48B;
# 1763
({struct Cyc_List_List*_tmp81A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp81A;});goto _LL23;}}else{_LL26: _LL27:
({void*_tmp48C=0U;({struct _fat_ptr _tmp81B=({const char*_tmp48D="function has non-function type";_tag_fat(_tmp48D,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp81B,_tag_fat(_tmp48C,sizeof(void*),0U));});});}_LL23:;}{
# 1766
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((fd->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp48E=Cyc_Cyclone_c_compiler;if(_tmp48E == Cyc_Cyclone_Gcc_c){_LL29: _LL2A:
 inlinedoc=Cyc_PP_text(({const char*_tmp48F="inline ";_tag_fat(_tmp48F,sizeof(char),8U);}));goto _LL28;}else{_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp490="__inline ";_tag_fat(_tmp490,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1774
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp491=Cyc_Cyclone_c_compiler;if(_tmp491 == Cyc_Cyclone_Gcc_c){_LL2E: _LL2F:
 beforenamedoc=attsdoc;goto _LL2D;}else{_LL30: _LL31:
 beforenamedoc=Cyc_Absynpp_callconv2doc((fd->i).attributes);goto _LL2D;}_LL2D:;}{
# 1781
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp81E=Cyc_Absyn_empty_tqual(0U);void*_tmp81D=t;Cyc_Absynpp_tqtd2doc(_tmp81E,_tmp81D,({struct Cyc_Core_Opt*_tmp498=_cycalloc(sizeof(*_tmp498));({
struct Cyc_PP_Doc*_tmp81C=({struct Cyc_PP_Doc*_tmp497[2U];_tmp497[0]=beforenamedoc,_tmp497[1]=namedoc;Cyc_PP_cat(_tag_fat(_tmp497,sizeof(struct Cyc_PP_Doc*),2U));});_tmp498->v=_tmp81C;});_tmp498;}));});
# 1789
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp495[5U];({struct Cyc_PP_Doc*_tmp825=Cyc_PP_blank_doc();_tmp495[0]=_tmp825;}),({struct Cyc_PP_Doc*_tmp824=Cyc_Absynpp_lb();_tmp495[1]=_tmp824;}),({
struct Cyc_PP_Doc*_tmp823=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp496[2U];({struct Cyc_PP_Doc*_tmp822=Cyc_PP_line_doc();_tmp496[0]=_tmp822;}),({struct Cyc_PP_Doc*_tmp821=Cyc_Absynpp_stmt2doc(fd->body,0,0);_tmp496[1]=_tmp821;});Cyc_PP_cat(_tag_fat(_tmp496,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp495[2]=_tmp823;}),({
struct Cyc_PP_Doc*_tmp820=Cyc_PP_line_doc();_tmp495[3]=_tmp820;}),({
struct Cyc_PP_Doc*_tmp81F=Cyc_Absynpp_rb();_tmp495[4]=_tmp81F;});Cyc_PP_cat(_tag_fat(_tmp495,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp492[4U];_tmp492[0]=inlinedoc,_tmp492[1]=scopedoc,_tmp492[2]=tqtddoc,_tmp492[3]=bodydoc;Cyc_PP_cat(_tag_fat(_tmp492,sizeof(struct Cyc_PP_Doc*),4U));});
# 1795
{enum Cyc_Cyclone_C_Compilers _tmp493=Cyc_Cyclone_c_compiler;if(_tmp493 == Cyc_Cyclone_Vc_c){_LL33: _LL34:
 s=({struct Cyc_PP_Doc*_tmp494[2U];_tmp494[0]=attsdoc,_tmp494[1]=s;Cyc_PP_cat(_tag_fat(_tmp494,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 1800
goto _LL0;}}}}}case 5U: _LL3: _tmp487=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp487;
# 1803
s=({struct Cyc_PP_Doc*_tmp49A[2U];({struct Cyc_PP_Doc*_tmp827=Cyc_Absynpp_aggrdecl2doc(ad);_tmp49A[0]=_tmp827;}),({struct Cyc_PP_Doc*_tmp826=Cyc_PP_text(({const char*_tmp49B=";";_tag_fat(_tmp49B,sizeof(char),2U);}));_tmp49A[1]=_tmp826;});Cyc_PP_cat(_tag_fat(_tmp49A,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 0U: _LL5: _tmp486=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp486;
# 1806
s=Cyc_Absynpp_vardecl2doc(vd);
goto _LL0;}case 4U: _LL7: _tmp483=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp484=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp473)->f2;_tmp485=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp473)->f3;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp483;struct Cyc_Absyn_Vardecl*vd=_tmp484;struct Cyc_Absyn_Exp*open_exp_opt=_tmp485;
# 1809
s=({struct Cyc_PP_Doc*_tmp49C[7U];({struct Cyc_PP_Doc*_tmp832=Cyc_PP_text(({const char*_tmp49D="region";_tag_fat(_tmp49D,sizeof(char),7U);}));_tmp49C[0]=_tmp832;}),({
struct Cyc_PP_Doc*_tmp831=Cyc_PP_text(({const char*_tmp49E="<";_tag_fat(_tmp49E,sizeof(char),2U);}));_tmp49C[1]=_tmp831;}),({
struct Cyc_PP_Doc*_tmp830=Cyc_Absynpp_tvar2doc(tv);_tmp49C[2]=_tmp830;}),({
struct Cyc_PP_Doc*_tmp82F=Cyc_PP_text(({const char*_tmp49F=">";_tag_fat(_tmp49F,sizeof(char),2U);}));_tmp49C[3]=_tmp82F;}),({
struct Cyc_PP_Doc*_tmp82E=Cyc_Absynpp_qvar2doc(vd->name);_tmp49C[4]=_tmp82E;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_tmp82D=({struct Cyc_PP_Doc*_tmp4A0[3U];({struct Cyc_PP_Doc*_tmp82C=Cyc_PP_text(({const char*_tmp4A1=" = open(";_tag_fat(_tmp4A1,sizeof(char),9U);}));_tmp4A0[0]=_tmp82C;}),({struct Cyc_PP_Doc*_tmp82B=Cyc_Absynpp_exp2doc(open_exp_opt);_tmp4A0[1]=_tmp82B;}),({
struct Cyc_PP_Doc*_tmp82A=Cyc_PP_text(({const char*_tmp4A2=")";_tag_fat(_tmp4A2,sizeof(char),2U);}));_tmp4A0[2]=_tmp82A;});Cyc_PP_cat(_tag_fat(_tmp4A0,sizeof(struct Cyc_PP_Doc*),3U));});
# 1814
_tmp49C[5]=_tmp82D;}):({
struct Cyc_PP_Doc*_tmp829=Cyc_PP_nil_doc();_tmp49C[5]=_tmp829;}),({
struct Cyc_PP_Doc*_tmp828=Cyc_PP_text(({const char*_tmp4A3=";";_tag_fat(_tmp4A3,sizeof(char),2U);}));_tmp49C[6]=_tmp828;});Cyc_PP_cat(_tag_fat(_tmp49C,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 6U: _LL9: _tmp482=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LLA: {struct Cyc_Absyn_Datatypedecl*dd=_tmp482;
# 1819
s=({struct Cyc_PP_Doc*_tmp4A4[2U];({struct Cyc_PP_Doc*_tmp834=Cyc_Absynpp_datatypedecl2doc(dd);_tmp4A4[0]=_tmp834;}),({struct Cyc_PP_Doc*_tmp833=Cyc_PP_text(({const char*_tmp4A5=";";_tag_fat(_tmp4A5,sizeof(char),2U);}));_tmp4A4[1]=_tmp833;});Cyc_PP_cat(_tag_fat(_tmp4A4,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 2U: _LLB: _tmp480=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp481=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp473)->f3;_LLC: {struct Cyc_Absyn_Pat*p=_tmp480;struct Cyc_Absyn_Exp*e=_tmp481;
# 1822
s=({struct Cyc_PP_Doc*_tmp4A6[5U];({struct Cyc_PP_Doc*_tmp839=Cyc_PP_text(({const char*_tmp4A7="let ";_tag_fat(_tmp4A7,sizeof(char),5U);}));_tmp4A6[0]=_tmp839;}),({
struct Cyc_PP_Doc*_tmp838=Cyc_Absynpp_pat2doc(p);_tmp4A6[1]=_tmp838;}),({
struct Cyc_PP_Doc*_tmp837=Cyc_PP_text(({const char*_tmp4A8=" = ";_tag_fat(_tmp4A8,sizeof(char),4U);}));_tmp4A6[2]=_tmp837;}),({
struct Cyc_PP_Doc*_tmp836=Cyc_Absynpp_exp2doc(e);_tmp4A6[3]=_tmp836;}),({
struct Cyc_PP_Doc*_tmp835=Cyc_PP_text(({const char*_tmp4A9=";";_tag_fat(_tmp4A9,sizeof(char),2U);}));_tmp4A6[4]=_tmp835;});Cyc_PP_cat(_tag_fat(_tmp4A6,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 3U: _LLD: _tmp47F=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LLE: {struct Cyc_List_List*vds=_tmp47F;
# 1829
s=({struct Cyc_PP_Doc*_tmp4AA[3U];({struct Cyc_PP_Doc*_tmp83C=Cyc_PP_text(({const char*_tmp4AB="let ";_tag_fat(_tmp4AB,sizeof(char),5U);}));_tmp4AA[0]=_tmp83C;}),({struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_ids2doc(vds);_tmp4AA[1]=_tmp83B;}),({struct Cyc_PP_Doc*_tmp83A=Cyc_PP_text(({const char*_tmp4AC=";";_tag_fat(_tmp4AC,sizeof(char),2U);}));_tmp4AA[2]=_tmp83A;});Cyc_PP_cat(_tag_fat(_tmp4AA,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 7U: _LLF: _tmp47E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp47E;
# 1832
s=({struct Cyc_PP_Doc*_tmp4AD[2U];({struct Cyc_PP_Doc*_tmp83E=Cyc_Absynpp_enumdecl2doc(ed);_tmp4AD[0]=_tmp83E;}),({struct Cyc_PP_Doc*_tmp83D=Cyc_PP_text(({const char*_tmp4AE=";";_tag_fat(_tmp4AE,sizeof(char),2U);}));_tmp4AD[1]=_tmp83D;});Cyc_PP_cat(_tag_fat(_tmp4AD,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 8U: _LL11: _tmp47D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp47D;
# 1835
void*t;
if(td->defn != 0)
t=(void*)_check_null(td->defn);else{
# 1839
t=Cyc_Absyn_new_evar(td->kind,0);}
s=({struct Cyc_PP_Doc*_tmp4AF[4U];({struct Cyc_PP_Doc*_tmp847=Cyc_PP_text(({const char*_tmp4B0="typedef ";_tag_fat(_tmp4B0,sizeof(char),9U);}));_tmp4AF[0]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp846=({struct Cyc_Absyn_Tqual _tmp845=td->tq;void*_tmp844=t;Cyc_Absynpp_tqtd2doc(_tmp845,_tmp844,({struct Cyc_Core_Opt*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));({
# 1843
struct Cyc_PP_Doc*_tmp843=({struct Cyc_PP_Doc*_tmp4B1[2U];({struct Cyc_PP_Doc*_tmp842=Cyc_Absynpp_typedef_name2bolddoc(td->name);_tmp4B1[0]=_tmp842;}),({
struct Cyc_PP_Doc*_tmp841=Cyc_Absynpp_tvars2doc(td->tvs);_tmp4B1[1]=_tmp841;});Cyc_PP_cat(_tag_fat(_tmp4B1,sizeof(struct Cyc_PP_Doc*),2U));});
# 1843
_tmp4B2->v=_tmp843;});_tmp4B2;}));});
# 1841
_tmp4AF[1]=_tmp846;}),({
# 1846
struct Cyc_PP_Doc*_tmp840=Cyc_Absynpp_atts2doc(td->atts);_tmp4AF[2]=_tmp840;}),({
struct Cyc_PP_Doc*_tmp83F=Cyc_PP_text(({const char*_tmp4B3=";";_tag_fat(_tmp4B3,sizeof(char),2U);}));_tmp4AF[3]=_tmp83F;});Cyc_PP_cat(_tag_fat(_tmp4AF,sizeof(struct Cyc_PP_Doc*),4U));});
# 1849
goto _LL0;}case 9U: _LL13: _tmp47B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp47C=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp473)->f2;_LL14: {struct _fat_ptr*v=_tmp47B;struct Cyc_List_List*tdl=_tmp47C;
# 1851
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_tmp4B4[8U];({struct Cyc_PP_Doc*_tmp850=Cyc_PP_text(({const char*_tmp4B5="namespace ";_tag_fat(_tmp4B5,sizeof(char),11U);}));_tmp4B4[0]=_tmp850;}),({
struct Cyc_PP_Doc*_tmp84F=Cyc_PP_textptr(v);_tmp4B4[1]=_tmp84F;}),({
struct Cyc_PP_Doc*_tmp84E=Cyc_PP_blank_doc();_tmp4B4[2]=_tmp84E;}),({struct Cyc_PP_Doc*_tmp84D=Cyc_Absynpp_lb();_tmp4B4[3]=_tmp84D;}),({
struct Cyc_PP_Doc*_tmp84C=Cyc_PP_line_doc();_tmp4B4[4]=_tmp84C;}),({
struct Cyc_PP_Doc*_tmp84B=({struct _fat_ptr _tmp84A=({const char*_tmp4B6="";_tag_fat(_tmp4B6,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp84A,tdl);});_tmp4B4[5]=_tmp84B;}),({
struct Cyc_PP_Doc*_tmp849=Cyc_PP_line_doc();_tmp4B4[6]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_Absynpp_rb();_tmp4B4[7]=_tmp848;});Cyc_PP_cat(_tag_fat(_tmp4B4,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10U: _LL15: _tmp479=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp47A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp473)->f2;_LL16: {struct _tuple0*q=_tmp479;struct Cyc_List_List*tdl=_tmp47A;
# 1862
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp4B7[8U];({struct Cyc_PP_Doc*_tmp859=Cyc_PP_text(({const char*_tmp4B8="using ";_tag_fat(_tmp4B8,sizeof(char),7U);}));_tmp4B7[0]=_tmp859;}),({
struct Cyc_PP_Doc*_tmp858=Cyc_Absynpp_qvar2doc(q);_tmp4B7[1]=_tmp858;}),({
struct Cyc_PP_Doc*_tmp857=Cyc_PP_blank_doc();_tmp4B7[2]=_tmp857;}),({struct Cyc_PP_Doc*_tmp856=Cyc_Absynpp_lb();_tmp4B7[3]=_tmp856;}),({
struct Cyc_PP_Doc*_tmp855=Cyc_PP_line_doc();_tmp4B7[4]=_tmp855;}),({
struct Cyc_PP_Doc*_tmp854=({struct _fat_ptr _tmp853=({const char*_tmp4B9="";_tag_fat(_tmp4B9,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp853,tdl);});_tmp4B7[5]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp852=Cyc_PP_line_doc();_tmp4B7[6]=_tmp852;}),({
struct Cyc_PP_Doc*_tmp851=Cyc_Absynpp_rb();_tmp4B7[7]=_tmp851;});Cyc_PP_cat(_tag_fat(_tmp4B7,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1871
s=({struct Cyc_PP_Doc*_tmp4BA[11U];({struct Cyc_PP_Doc*_tmp865=Cyc_PP_text(({const char*_tmp4BB="/* using ";_tag_fat(_tmp4BB,sizeof(char),10U);}));_tmp4BA[0]=_tmp865;}),({
struct Cyc_PP_Doc*_tmp864=Cyc_Absynpp_qvar2doc(q);_tmp4BA[1]=_tmp864;}),({
struct Cyc_PP_Doc*_tmp863=Cyc_PP_blank_doc();_tmp4BA[2]=_tmp863;}),({
struct Cyc_PP_Doc*_tmp862=Cyc_Absynpp_lb();_tmp4BA[3]=_tmp862;}),({
struct Cyc_PP_Doc*_tmp861=Cyc_PP_text(({const char*_tmp4BC=" */";_tag_fat(_tmp4BC,sizeof(char),4U);}));_tmp4BA[4]=_tmp861;}),({
struct Cyc_PP_Doc*_tmp860=Cyc_PP_line_doc();_tmp4BA[5]=_tmp860;}),({
struct Cyc_PP_Doc*_tmp85F=({struct _fat_ptr _tmp85E=({const char*_tmp4BD="";_tag_fat(_tmp4BD,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp85E,tdl);});_tmp4BA[6]=_tmp85F;}),({
struct Cyc_PP_Doc*_tmp85D=Cyc_PP_line_doc();_tmp4BA[7]=_tmp85D;}),({
struct Cyc_PP_Doc*_tmp85C=Cyc_PP_text(({const char*_tmp4BE="/* ";_tag_fat(_tmp4BE,sizeof(char),4U);}));_tmp4BA[8]=_tmp85C;}),({
struct Cyc_PP_Doc*_tmp85B=Cyc_Absynpp_rb();_tmp4BA[9]=_tmp85B;}),({
struct Cyc_PP_Doc*_tmp85A=Cyc_PP_text(({const char*_tmp4BF=" */";_tag_fat(_tmp4BF,sizeof(char),4U);}));_tmp4BA[10]=_tmp85A;});Cyc_PP_cat(_tag_fat(_tmp4BA,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;}case 11U: _LL17: _tmp478=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL18: {struct Cyc_List_List*tdl=_tmp478;
# 1884
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp4C0[6U];({struct Cyc_PP_Doc*_tmp86C=Cyc_PP_text(({const char*_tmp4C1="extern \"C\" ";_tag_fat(_tmp4C1,sizeof(char),12U);}));_tmp4C0[0]=_tmp86C;}),({
struct Cyc_PP_Doc*_tmp86B=Cyc_Absynpp_lb();_tmp4C0[1]=_tmp86B;}),({
struct Cyc_PP_Doc*_tmp86A=Cyc_PP_line_doc();_tmp4C0[2]=_tmp86A;}),({
struct Cyc_PP_Doc*_tmp869=({struct _fat_ptr _tmp868=({const char*_tmp4C2="";_tag_fat(_tmp4C2,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp868,tdl);});_tmp4C0[3]=_tmp869;}),({
struct Cyc_PP_Doc*_tmp867=Cyc_PP_line_doc();_tmp4C0[4]=_tmp867;}),({
struct Cyc_PP_Doc*_tmp866=Cyc_Absynpp_rb();_tmp4C0[5]=_tmp866;});Cyc_PP_cat(_tag_fat(_tmp4C0,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1892
s=({struct Cyc_PP_Doc*_tmp4C3[9U];({struct Cyc_PP_Doc*_tmp876=Cyc_PP_text(({const char*_tmp4C4="/* extern \"C\" ";_tag_fat(_tmp4C4,sizeof(char),15U);}));_tmp4C3[0]=_tmp876;}),({
struct Cyc_PP_Doc*_tmp875=Cyc_Absynpp_lb();_tmp4C3[1]=_tmp875;}),({
struct Cyc_PP_Doc*_tmp874=Cyc_PP_text(({const char*_tmp4C5=" */";_tag_fat(_tmp4C5,sizeof(char),4U);}));_tmp4C3[2]=_tmp874;}),({
struct Cyc_PP_Doc*_tmp873=Cyc_PP_line_doc();_tmp4C3[3]=_tmp873;}),({
struct Cyc_PP_Doc*_tmp872=({struct _fat_ptr _tmp871=({const char*_tmp4C6="";_tag_fat(_tmp4C6,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp871,tdl);});_tmp4C3[4]=_tmp872;}),({
struct Cyc_PP_Doc*_tmp870=Cyc_PP_line_doc();_tmp4C3[5]=_tmp870;}),({
struct Cyc_PP_Doc*_tmp86F=Cyc_PP_text(({const char*_tmp4C7="/* ";_tag_fat(_tmp4C7,sizeof(char),4U);}));_tmp4C3[6]=_tmp86F;}),({
struct Cyc_PP_Doc*_tmp86E=Cyc_Absynpp_rb();_tmp4C3[7]=_tmp86E;}),({
struct Cyc_PP_Doc*_tmp86D=Cyc_PP_text(({const char*_tmp4C8=" */";_tag_fat(_tmp4C8,sizeof(char),4U);}));_tmp4C3[8]=_tmp86D;});Cyc_PP_cat(_tag_fat(_tmp4C3,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 12U: _LL19: _tmp474=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp475=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp473)->f2;_tmp476=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp473)->f3;_tmp477=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp473)->f4;_LL1A: {struct Cyc_List_List*tdl=_tmp474;struct Cyc_List_List*ovrs=_tmp475;struct Cyc_List_List*exs=_tmp476;struct _tuple10*wc=_tmp477;
# 1903
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp4C9[7U];({struct Cyc_PP_Doc*_tmp87E=Cyc_Absynpp_rb();_tmp4C9[0]=_tmp87E;}),({struct Cyc_PP_Doc*_tmp87D=Cyc_PP_text(({const char*_tmp4CA=" export ";_tag_fat(_tmp4CA,sizeof(char),9U);}));_tmp4C9[1]=_tmp87D;}),({struct Cyc_PP_Doc*_tmp87C=Cyc_Absynpp_lb();_tmp4C9[2]=_tmp87C;}),({
struct Cyc_PP_Doc*_tmp87B=Cyc_PP_line_doc();_tmp4C9[3]=_tmp87B;}),({struct Cyc_PP_Doc*_tmp87A=({struct _fat_ptr _tmp879=({const char*_tmp4CB=",";_tag_fat(_tmp4CB,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple20*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp879,exs);});_tmp4C9[4]=_tmp87A;}),({
struct Cyc_PP_Doc*_tmp878=Cyc_PP_line_doc();_tmp4C9[5]=_tmp878;}),({struct Cyc_PP_Doc*_tmp877=Cyc_Absynpp_rb();_tmp4C9[6]=_tmp877;});Cyc_PP_cat(_tag_fat(_tmp4C9,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1911
exs_doc=Cyc_Absynpp_rb();}
if(ovrs != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp4CC[7U];({struct Cyc_PP_Doc*_tmp886=Cyc_Absynpp_rb();_tmp4CC[0]=_tmp886;}),({struct Cyc_PP_Doc*_tmp885=Cyc_PP_text(({const char*_tmp4CD=" cycdef ";_tag_fat(_tmp4CD,sizeof(char),9U);}));_tmp4CC[1]=_tmp885;}),({struct Cyc_PP_Doc*_tmp884=Cyc_Absynpp_lb();_tmp4CC[2]=_tmp884;}),({
struct Cyc_PP_Doc*_tmp883=Cyc_PP_line_doc();_tmp4CC[3]=_tmp883;}),({struct Cyc_PP_Doc*_tmp882=({struct _fat_ptr _tmp881=({const char*_tmp4CE="";_tag_fat(_tmp4CE,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp881,ovrs);});_tmp4CC[4]=_tmp882;}),({
struct Cyc_PP_Doc*_tmp880=Cyc_PP_line_doc();_tmp4CC[5]=_tmp880;}),({struct Cyc_PP_Doc*_tmp87F=Cyc_Absynpp_rb();_tmp4CC[6]=_tmp87F;});Cyc_PP_cat(_tag_fat(_tmp4CC,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1917
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp4CF[6U];({struct Cyc_PP_Doc*_tmp88C=Cyc_PP_text(({const char*_tmp4D0="extern \"C include\" ";_tag_fat(_tmp4D0,sizeof(char),20U);}));_tmp4CF[0]=_tmp88C;}),({
struct Cyc_PP_Doc*_tmp88B=Cyc_Absynpp_lb();_tmp4CF[1]=_tmp88B;}),({
struct Cyc_PP_Doc*_tmp88A=Cyc_PP_line_doc();_tmp4CF[2]=_tmp88A;}),({
struct Cyc_PP_Doc*_tmp889=({struct _fat_ptr _tmp888=({const char*_tmp4D1="";_tag_fat(_tmp4D1,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp888,tdl);});_tmp4CF[3]=_tmp889;}),({
struct Cyc_PP_Doc*_tmp887=Cyc_PP_line_doc();_tmp4CF[4]=_tmp887;}),_tmp4CF[5]=exs_doc;Cyc_PP_cat(_tag_fat(_tmp4CF,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1925
s=({struct Cyc_PP_Doc*_tmp4D2[9U];({struct Cyc_PP_Doc*_tmp896=Cyc_PP_text(({const char*_tmp4D3="/* extern \"C include\" ";_tag_fat(_tmp4D3,sizeof(char),23U);}));_tmp4D2[0]=_tmp896;}),({
struct Cyc_PP_Doc*_tmp895=Cyc_Absynpp_lb();_tmp4D2[1]=_tmp895;}),({
struct Cyc_PP_Doc*_tmp894=Cyc_PP_text(({const char*_tmp4D4=" */";_tag_fat(_tmp4D4,sizeof(char),4U);}));_tmp4D2[2]=_tmp894;}),({
struct Cyc_PP_Doc*_tmp893=Cyc_PP_line_doc();_tmp4D2[3]=_tmp893;}),({
struct Cyc_PP_Doc*_tmp892=({struct _fat_ptr _tmp891=({const char*_tmp4D5="";_tag_fat(_tmp4D5,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp891,tdl);});_tmp4D2[4]=_tmp892;}),({
struct Cyc_PP_Doc*_tmp890=Cyc_PP_line_doc();_tmp4D2[5]=_tmp890;}),({
struct Cyc_PP_Doc*_tmp88F=Cyc_PP_text(({const char*_tmp4D6="/* ";_tag_fat(_tmp4D6,sizeof(char),4U);}));_tmp4D2[6]=_tmp88F;}),({
struct Cyc_PP_Doc*_tmp88E=Cyc_Absynpp_rb();_tmp4D2[7]=_tmp88E;}),({
struct Cyc_PP_Doc*_tmp88D=Cyc_PP_text(({const char*_tmp4D7=" */";_tag_fat(_tmp4D7,sizeof(char),4U);}));_tmp4D2[8]=_tmp88D;});Cyc_PP_cat(_tag_fat(_tmp4D2,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 13U: _LL1B: _LL1C:
# 1936
 s=({struct Cyc_PP_Doc*_tmp4D8[2U];({struct Cyc_PP_Doc*_tmp898=Cyc_PP_text(({const char*_tmp4D9="__cyclone_port_on__;";_tag_fat(_tmp4D9,sizeof(char),21U);}));_tmp4D8[0]=_tmp898;}),({struct Cyc_PP_Doc*_tmp897=Cyc_Absynpp_lb();_tmp4D8[1]=_tmp897;});Cyc_PP_cat(_tag_fat(_tmp4D8,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1939
 s=({struct Cyc_PP_Doc*_tmp4DA[2U];({struct Cyc_PP_Doc*_tmp89A=Cyc_PP_text(({const char*_tmp4DB="__cyclone_port_off__;";_tag_fat(_tmp4DB,sizeof(char),22U);}));_tmp4DA[0]=_tmp89A;}),({struct Cyc_PP_Doc*_tmp899=Cyc_Absynpp_lb();_tmp4DA[1]=_tmp899;});Cyc_PP_cat(_tag_fat(_tmp4DA,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1942
 s=({struct Cyc_PP_Doc*_tmp4DC[2U];({struct Cyc_PP_Doc*_tmp89C=Cyc_PP_text(({const char*_tmp4DD="__tempest_on__;";_tag_fat(_tmp4DD,sizeof(char),16U);}));_tmp4DC[0]=_tmp89C;}),({struct Cyc_PP_Doc*_tmp89B=Cyc_Absynpp_lb();_tmp4DC[1]=_tmp89B;});Cyc_PP_cat(_tag_fat(_tmp4DC,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1945
 s=({struct Cyc_PP_Doc*_tmp4DE[2U];({struct Cyc_PP_Doc*_tmp89E=Cyc_PP_text(({const char*_tmp4DF="__tempest_off__;";_tag_fat(_tmp4DF,sizeof(char),17U);}));_tmp4DE[0]=_tmp89E;}),({struct Cyc_PP_Doc*_tmp89D=Cyc_Absynpp_lb();_tmp4DE[1]=_tmp89D;});Cyc_PP_cat(_tag_fat(_tmp4DE,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 1949
return s;}
# 1952
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4E0=sc;switch(_tmp4E0){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4E1="static ";_tag_fat(_tmp4E1,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4E2="extern ";_tag_fat(_tmp4E2,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4E3="extern \"C\" ";_tag_fat(_tmp4E3,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4E4="abstract ";_tag_fat(_tmp4E4,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4E5="register ";_tag_fat(_tmp4E5,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;}}
# 1966
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4E6=Cyc_Tcutil_compress(t);void*_stmttmp22=_tmp4E6;void*_tmp4E7=_stmttmp22;struct Cyc_List_List*_tmp4E8;struct Cyc_Absyn_Tvar*_tmp4E9;switch(*((int*)_tmp4E7)){case 2U: _LL1: _tmp4E9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E7)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp4E9;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E7)->f1)->tag == 9U){_LL3: _tmp4E8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E7)->f2;_LL4: {struct Cyc_List_List*l=_tmp4E8;
return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 1978
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4EA=t;void*_tmp4EC;struct Cyc_Absyn_Typedefdecl*_tmp4EB;switch(*((int*)_tmp4EA)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EA)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp4EB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4EA)->f3;_tmp4EC=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4EA)->f4;if(_tmp4EC != 0){_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmp4EB;void*x=_tmp4EC;
# 1985
return Cyc_Absynpp_is_anon_aggrtype(x);}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1992
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1995
if(tms != 0 && tms->tl != 0){
struct _tuple18 _tmp4ED=({struct _tuple18 _tmp588;_tmp588.f1=(void*)tms->hd,_tmp588.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp588;});struct _tuple18 _stmttmp23=_tmp4ED;struct _tuple18 _tmp4EE=_stmttmp23;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4EE.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4EE.f2)->tag == 3U){_LL1: _LL2:
# 1998
 return({struct Cyc_List_List*_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0));_tmp4F0->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp8A0=({struct Cyc_List_List*_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF));_tmp4EF->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp89F=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4EF->tl=_tmp89F;});_tmp4EF;});_tmp4F0->tl=_tmp8A0;});_tmp4F0;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4F1=_region_malloc(r,sizeof(*_tmp4F1));_tmp4F1->hd=atts,_tmp4F1->tl=tms;_tmp4F1;});}_LL0:;}else{
# 2001
return({struct Cyc_List_List*_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2));_tmp4F2->hd=atts,_tmp4F2->tl=tms;_tmp4F2;});}}
# 2004
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp4F3=({struct _fat_ptr _tmp8A1=({const char*_tmp4F9="`";_tag_fat(_tmp4F9,sizeof(char),2U);});Cyc_strconcat(_tmp8A1,(struct _fat_ptr)*t->name);});struct _fat_ptr s=_tmp4F3;
({struct _fat_ptr _tmp4F4=_fat_ptr_plus(s,sizeof(char),1);char _tmp4F5=*((char*)_check_fat_subscript(_tmp4F4,sizeof(char),0U));char _tmp4F6='t';if(_get_fat_size(_tmp4F4,sizeof(char))== 1U &&(_tmp4F5 == 0 && _tmp4F6 != 0))_throw_arraybounds();*((char*)_tmp4F4.curr)=_tmp4F6;});
({struct _fat_ptr*_tmp8A2=({unsigned _tmp4F8=1;struct _fat_ptr*_tmp4F7=_cycalloc(_check_times(_tmp4F8,sizeof(struct _fat_ptr)));_tmp4F7[0]=(struct _fat_ptr)s;_tmp4F7;});t->name=_tmp8A2;});}}
# 2013
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2015
void*_tmp4FA=t;void*_tmp4FE;struct Cyc_Absyn_Typedefdecl*_tmp4FD;struct Cyc_List_List*_tmp4FC;struct _tuple0*_tmp4FB;int _tmp501;void*_tmp500;struct Cyc_Core_Opt*_tmp4FF;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*_tmp509;struct Cyc_Absyn_VarargInfo*_tmp508;int _tmp507;struct Cyc_List_List*_tmp506;void*_tmp505;struct Cyc_Absyn_Tqual _tmp504;void*_tmp503;struct Cyc_List_List*_tmp502;struct Cyc_Absyn_PtrAtts _tmp50F;struct Cyc_Absyn_Tqual _tmp50E;void*_tmp50D;unsigned _tmp514;void*_tmp513;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Tqual _tmp511;void*_tmp510;switch(*((int*)_tmp4FA)){case 4U: _LL1: _tmp510=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).elt_type;_tmp511=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).tq;_tmp512=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).num_elts;_tmp513=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).zero_term;_tmp514=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).zt_loc;_LL2: {void*t2=_tmp510;struct Cyc_Absyn_Tqual tq2=_tmp511;struct Cyc_Absyn_Exp*e=_tmp512;void*zeroterm=_tmp513;unsigned ztl=_tmp514;
# 2018
struct _tuple13 _tmp515=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp24=_tmp515;struct _tuple13 _tmp516=_stmttmp24;struct Cyc_List_List*_tmp519;void*_tmp518;struct Cyc_Absyn_Tqual _tmp517;_LLE: _tmp517=_tmp516.f1;_tmp518=_tmp516.f2;_tmp519=_tmp516.f3;_LLF: {struct Cyc_Absyn_Tqual tq3=_tmp517;void*t3=_tmp518;struct Cyc_List_List*tml3=_tmp519;
void*tm;
if(e == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp51A=_region_malloc(r,sizeof(*_tmp51A));_tmp51A->tag=0U,_tmp51A->f1=zeroterm,_tmp51A->f2=ztl;_tmp51A;});else{
# 2023
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp51B=_region_malloc(r,sizeof(*_tmp51B));_tmp51B->tag=1U,_tmp51B->f1=e,_tmp51B->f2=zeroterm,_tmp51B->f3=ztl;_tmp51B;});}
return({struct _tuple13 _tmp589;_tmp589.f1=tq3,_tmp589.f2=t3,({struct Cyc_List_List*_tmp8A3=({struct Cyc_List_List*_tmp51C=_region_malloc(r,sizeof(*_tmp51C));_tmp51C->hd=tm,_tmp51C->tl=tml3;_tmp51C;});_tmp589.f3=_tmp8A3;});_tmp589;});}}case 3U: _LL3: _tmp50D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).elt_type;_tmp50E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).elt_tq;_tmp50F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).ptr_atts;_LL4: {void*t2=_tmp50D;struct Cyc_Absyn_Tqual tq2=_tmp50E;struct Cyc_Absyn_PtrAtts ptratts=_tmp50F;
# 2027
struct _tuple13 _tmp51D=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp25=_tmp51D;struct _tuple13 _tmp51E=_stmttmp25;struct Cyc_List_List*_tmp521;void*_tmp520;struct Cyc_Absyn_Tqual _tmp51F;_LL11: _tmp51F=_tmp51E.f1;_tmp520=_tmp51E.f2;_tmp521=_tmp51E.f3;_LL12: {struct Cyc_Absyn_Tqual tq3=_tmp51F;void*t3=_tmp520;struct Cyc_List_List*tml3=_tmp521;
tml3=({struct Cyc_List_List*_tmp523=_region_malloc(r,sizeof(*_tmp523));({void*_tmp8A4=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp522=_region_malloc(r,sizeof(*_tmp522));_tmp522->tag=2U,_tmp522->f1=ptratts,_tmp522->f2=tq;_tmp522;});_tmp523->hd=_tmp8A4;}),_tmp523->tl=tml3;_tmp523;});
return({struct _tuple13 _tmp58A;_tmp58A.f1=tq3,_tmp58A.f2=t3,_tmp58A.f3=tml3;_tmp58A;});}}case 5U: _LL5: _tmp502=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).tvars;_tmp503=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).effect;_tmp504=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).ret_tqual;_tmp505=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).ret_type;_tmp506=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).args;_tmp507=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).c_varargs;_tmp508=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).cyc_varargs;_tmp509=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).rgn_po;_tmp50A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).attributes;_tmp50B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).requires_clause;_tmp50C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).ensures_clause;_LL6: {struct Cyc_List_List*typvars=_tmp502;void*effopt=_tmp503;struct Cyc_Absyn_Tqual t2qual=_tmp504;void*t2=_tmp505;struct Cyc_List_List*args=_tmp506;int c_varargs=_tmp507;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp508;struct Cyc_List_List*rgn_po=_tmp509;struct Cyc_List_List*fn_atts=_tmp50A;struct Cyc_Absyn_Exp*req=_tmp50B;struct Cyc_Absyn_Exp*ens=_tmp50C;
# 2033
if(!Cyc_Absynpp_print_all_tvars){
# 2037
if(effopt == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2042
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2045
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2050
struct _tuple13 _tmp524=Cyc_Absynpp_to_tms(r,t2qual,t2);struct _tuple13 _stmttmp26=_tmp524;struct _tuple13 _tmp525=_stmttmp26;struct Cyc_List_List*_tmp528;void*_tmp527;struct Cyc_Absyn_Tqual _tmp526;_LL14: _tmp526=_tmp525.f1;_tmp527=_tmp525.f2;_tmp528=_tmp525.f3;_LL15: {struct Cyc_Absyn_Tqual tq3=_tmp526;void*t3=_tmp527;struct Cyc_List_List*tml3=_tmp528;
struct Cyc_List_List*tms=tml3;
# 2061 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp529=Cyc_Cyclone_c_compiler;if(_tmp529 == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2063
 if(fn_atts != 0)
# 2066
tms=({struct _RegionHandle*_tmp8A6=r;void*_tmp8A5=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp52A=_region_malloc(r,sizeof(*_tmp52A));_tmp52A->tag=5U,_tmp52A->f1=0U,_tmp52A->f2=fn_atts;_tmp52A;});Cyc_Absynpp_bubble_attributes(_tmp8A6,_tmp8A5,tms);});
# 2068
tms=({struct Cyc_List_List*_tmp52D=_region_malloc(r,sizeof(*_tmp52D));({void*_tmp8A8=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp52C=_region_malloc(r,sizeof(*_tmp52C));
_tmp52C->tag=3U,({void*_tmp8A7=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp52B=_region_malloc(r,sizeof(*_tmp52B));_tmp52B->tag=1U,_tmp52B->f1=args,_tmp52B->f2=c_varargs,_tmp52B->f3=cyc_varargs,_tmp52B->f4=effopt,_tmp52B->f5=rgn_po,_tmp52B->f6=req,_tmp52B->f7=ens;_tmp52B;});_tmp52C->f1=_tmp8A7;});_tmp52C;});
# 2068
_tmp52D->hd=_tmp8A8;}),_tmp52D->tl=tms;_tmp52D;});
# 2072
goto _LL16;}else{_LL19: _LL1A:
# 2074
 tms=({struct Cyc_List_List*_tmp530=_region_malloc(r,sizeof(*_tmp530));({void*_tmp8AA=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp52F=_region_malloc(r,sizeof(*_tmp52F));
_tmp52F->tag=3U,({void*_tmp8A9=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp52E=_region_malloc(r,sizeof(*_tmp52E));_tmp52E->tag=1U,_tmp52E->f1=args,_tmp52E->f2=c_varargs,_tmp52E->f3=cyc_varargs,_tmp52E->f4=effopt,_tmp52E->f5=rgn_po,_tmp52E->f6=req,_tmp52E->f7=ens;_tmp52E;});_tmp52F->f1=_tmp8A9;});_tmp52F;});
# 2074
_tmp530->hd=_tmp8AA;}),_tmp530->tl=tms;_tmp530;});
# 2078
for(0;fn_atts != 0;fn_atts=fn_atts->tl){
void*_tmp531=(void*)fn_atts->hd;void*_stmttmp27=_tmp531;void*_tmp532=_stmttmp27;switch(*((int*)_tmp532)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2083
 tms=({struct Cyc_List_List*_tmp535=_region_malloc(r,sizeof(*_tmp535));({void*_tmp8AC=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp534=_region_malloc(r,sizeof(*_tmp534));_tmp534->tag=5U,_tmp534->f1=0U,({struct Cyc_List_List*_tmp8AB=({struct Cyc_List_List*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->hd=(void*)fn_atts->hd,_tmp533->tl=0;_tmp533;});_tmp534->f2=_tmp8AB;});_tmp534;});_tmp535->hd=_tmp8AC;}),_tmp535->tl=tms;_tmp535;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2087
goto _LL16;}_LL16:;}
# 2090
AfterAtts:
 if(typvars != 0)
tms=({struct Cyc_List_List*_tmp537=_region_malloc(r,sizeof(*_tmp537));({void*_tmp8AD=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp536=_region_malloc(r,sizeof(*_tmp536));_tmp536->tag=4U,_tmp536->f1=typvars,_tmp536->f2=0U,_tmp536->f3=1;_tmp536;});_tmp537->hd=_tmp8AD;}),_tmp537->tl=tms;_tmp537;});
return({struct _tuple13 _tmp58B;_tmp58B.f1=tq3,_tmp58B.f2=t3,_tmp58B.f3=tms;_tmp58B;});}}}case 1U: _LL7: _tmp4FF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FA)->f1;_tmp500=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FA)->f2;_tmp501=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FA)->f3;_LL8: {struct Cyc_Core_Opt*k=_tmp4FF;void*topt=_tmp500;int i=_tmp501;
# 2096
if(topt == 0)
return({struct _tuple13 _tmp58C;_tmp58C.f1=tq,_tmp58C.f2=t,_tmp58C.f3=0;_tmp58C;});else{
# 2099
return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8U: _LL9: _tmp4FB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FA)->f1;_tmp4FC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FA)->f2;_tmp4FD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FA)->f3;_tmp4FE=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FA)->f4;_LLA: {struct _tuple0*n=_tmp4FB;struct Cyc_List_List*ts=_tmp4FC;struct Cyc_Absyn_Typedefdecl*td=_tmp4FD;void*topt=_tmp4FE;
# 2105
if(topt == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple13 _tmp58D;_tmp58D.f1=tq,_tmp58D.f2=t,_tmp58D.f3=0;_tmp58D;});else{
# 2108
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}}default: _LLB: _LLC:
# 2113
 return({struct _tuple13 _tmp58E;_tmp58E.f1=tq,_tmp58E.f2=t,_tmp58E.f3=0;_tmp58E;});}_LL0:;}
# 2117
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2119
void*_tmp538=t;void*_tmp539;void*_tmp53A;switch(*((int*)_tmp538)){case 1U: _LL1: _tmp53A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp538)->f2;if(_tmp53A != 0){_LL2: {void*def=_tmp53A;
return Cyc_Absynpp_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp539=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp538)->f1).elt_type;_LL4: {void*elt_typ=_tmp539;
# 2122
L: {
void*_tmp53B=elt_typ;void*_tmp53C;void*_tmp53D;switch(*((int*)_tmp53B)){case 1U: _LL8: _tmp53D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp53B)->f2;if(_tmp53D != 0){_LL9: {void*t=_tmp53D;
elt_typ=t;goto L;}}else{goto _LLE;}case 8U: _LLA: _tmp53C=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53B)->f4;if(_tmp53C != 0){_LLB: {void*t=_tmp53C;
elt_typ=t;goto L;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53B)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53B)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 2129
 return 0;}_LL0:;}
# 2133
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp53E=_new_region("temp");struct _RegionHandle*temp=& _tmp53E;_push_region(temp);
{struct _tuple13 _tmp53F=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple13 _stmttmp28=_tmp53F;struct _tuple13 _tmp540=_stmttmp28;struct Cyc_List_List*_tmp543;void*_tmp542;struct Cyc_Absyn_Tqual _tmp541;_LL1: _tmp541=_tmp540.f1;_tmp542=_tmp540.f2;_tmp543=_tmp540.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp541;void*t=_tmp542;struct Cyc_List_List*tms=_tmp543;
tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);
if(tms == 0 && dopt == 0){
struct Cyc_PP_Doc*_tmp545=({struct Cyc_PP_Doc*_tmp544[2U];({struct Cyc_PP_Doc*_tmp8AF=Cyc_Absynpp_tqual2doc(tq);_tmp544[0]=_tmp8AF;}),({struct Cyc_PP_Doc*_tmp8AE=Cyc_Absynpp_ntyp2doc(t);_tmp544[1]=_tmp8AE;});Cyc_PP_cat(_tag_fat(_tmp544,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp545;}else{
# 2140
struct Cyc_PP_Doc*_tmp548=({struct Cyc_PP_Doc*_tmp546[4U];({
struct Cyc_PP_Doc*_tmp8B5=Cyc_Absynpp_tqual2doc(tq);_tmp546[0]=_tmp8B5;}),({
struct Cyc_PP_Doc*_tmp8B4=Cyc_Absynpp_ntyp2doc(t);_tmp546[1]=_tmp8B4;}),({
struct Cyc_PP_Doc*_tmp8B3=Cyc_PP_text(({const char*_tmp547=" ";_tag_fat(_tmp547,sizeof(char),2U);}));_tmp546[2]=_tmp8B3;}),({
struct Cyc_PP_Doc*_tmp8B2=({int _tmp8B1=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp8B0=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp8B1,_tmp8B0,tms);});_tmp546[3]=_tmp8B2;});Cyc_PP_cat(_tag_fat(_tmp546,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp548;}}}
# 2135
;_pop_region(temp);}
# 2148
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp549=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp549;
if((unsigned)req)
requires_doc=({struct Cyc_PP_Doc*_tmp54A[2U];({struct Cyc_PP_Doc*_tmp8B7=Cyc_PP_text(({const char*_tmp54B="@requires ";_tag_fat(_tmp54B,sizeof(char),11U);}));_tmp54A[0]=_tmp8B7;}),({struct Cyc_PP_Doc*_tmp8B6=Cyc_Absynpp_exp2doc(req);_tmp54A[1]=_tmp8B6;});Cyc_PP_cat(_tag_fat(_tmp54A,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2154
requires_doc=Cyc_PP_nil_doc();}{
# 2156
enum Cyc_Cyclone_C_Compilers _tmp54C=Cyc_Cyclone_c_compiler;if(_tmp54C == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2159
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp54D[5U];({struct Cyc_PP_Doc*_tmp8BF=({struct Cyc_Absyn_Tqual _tmp8BE=f->tq;void*_tmp8BD=f->type;Cyc_Absynpp_tqtd2doc(_tmp8BE,_tmp8BD,({struct Cyc_Core_Opt*_tmp54E=_cycalloc(sizeof(*_tmp54E));({struct Cyc_PP_Doc*_tmp8BC=Cyc_PP_textptr(f->name);_tmp54E->v=_tmp8BC;});_tmp54E;}));});_tmp54D[0]=_tmp8BF;}),({
struct Cyc_PP_Doc*_tmp8BB=Cyc_PP_text(({const char*_tmp54F=":";_tag_fat(_tmp54F,sizeof(char),2U);}));_tmp54D[1]=_tmp8BB;}),({struct Cyc_PP_Doc*_tmp8BA=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp54D[2]=_tmp8BA;}),({
struct Cyc_PP_Doc*_tmp8B9=Cyc_Absynpp_atts2doc(f->attributes);_tmp54D[3]=_tmp8B9;}),({struct Cyc_PP_Doc*_tmp8B8=Cyc_PP_text(({const char*_tmp550=";";_tag_fat(_tmp550,sizeof(char),2U);}));_tmp54D[4]=_tmp8B8;});Cyc_PP_cat(_tag_fat(_tmp54D,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2164
return({struct Cyc_PP_Doc*_tmp551[4U];({struct Cyc_PP_Doc*_tmp8C5=({struct Cyc_Absyn_Tqual _tmp8C4=f->tq;void*_tmp8C3=f->type;Cyc_Absynpp_tqtd2doc(_tmp8C4,_tmp8C3,({struct Cyc_Core_Opt*_tmp552=_cycalloc(sizeof(*_tmp552));({struct Cyc_PP_Doc*_tmp8C2=Cyc_PP_textptr(f->name);_tmp552->v=_tmp8C2;});_tmp552;}));});_tmp551[0]=_tmp8C5;}),({
struct Cyc_PP_Doc*_tmp8C1=Cyc_Absynpp_atts2doc(f->attributes);_tmp551[1]=_tmp8C1;}),_tmp551[2]=requires_doc,({struct Cyc_PP_Doc*_tmp8C0=Cyc_PP_text(({const char*_tmp553=";";_tag_fat(_tmp553,sizeof(char),2U);}));_tmp551[3]=_tmp8C0;});Cyc_PP_cat(_tag_fat(_tmp551,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2167
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp554[5U];({struct Cyc_PP_Doc*_tmp8CD=Cyc_Absynpp_atts2doc(f->attributes);_tmp554[0]=_tmp8CD;}),({
struct Cyc_PP_Doc*_tmp8CC=({struct Cyc_Absyn_Tqual _tmp8CB=f->tq;void*_tmp8CA=f->type;Cyc_Absynpp_tqtd2doc(_tmp8CB,_tmp8CA,({struct Cyc_Core_Opt*_tmp555=_cycalloc(sizeof(*_tmp555));({struct Cyc_PP_Doc*_tmp8C9=Cyc_PP_textptr(f->name);_tmp555->v=_tmp8C9;});_tmp555;}));});_tmp554[1]=_tmp8CC;}),({
struct Cyc_PP_Doc*_tmp8C8=Cyc_PP_text(({const char*_tmp556=":";_tag_fat(_tmp556,sizeof(char),2U);}));_tmp554[2]=_tmp8C8;}),({struct Cyc_PP_Doc*_tmp8C7=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp554[3]=_tmp8C7;}),({struct Cyc_PP_Doc*_tmp8C6=Cyc_PP_text(({const char*_tmp557=";";_tag_fat(_tmp557,sizeof(char),2U);}));_tmp554[4]=_tmp8C6;});Cyc_PP_cat(_tag_fat(_tmp554,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2172
return({struct Cyc_PP_Doc*_tmp558[4U];({struct Cyc_PP_Doc*_tmp8D3=Cyc_Absynpp_atts2doc(f->attributes);_tmp558[0]=_tmp8D3;}),({
struct Cyc_PP_Doc*_tmp8D2=({struct Cyc_Absyn_Tqual _tmp8D1=f->tq;void*_tmp8D0=f->type;Cyc_Absynpp_tqtd2doc(_tmp8D1,_tmp8D0,({struct Cyc_Core_Opt*_tmp559=_cycalloc(sizeof(*_tmp559));({struct Cyc_PP_Doc*_tmp8CF=Cyc_PP_textptr(f->name);_tmp559->v=_tmp8CF;});_tmp559;}));});_tmp558[1]=_tmp8D2;}),_tmp558[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp8CE=Cyc_PP_text(({const char*_tmp55A=";";_tag_fat(_tmp55A,sizeof(char),2U);}));_tmp558[3]=_tmp8CE;});Cyc_PP_cat(_tag_fat(_tmp558,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;}}
# 2179
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp8D4=({const char*_tmp55B="";_tag_fat(_tmp55B,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp8D4,fields);});}
# 2183
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp55C[3U];({struct Cyc_PP_Doc*_tmp8D8=Cyc_Absynpp_scope2doc(f->sc);_tmp55C[0]=_tmp8D8;}),({struct Cyc_PP_Doc*_tmp8D7=Cyc_Absynpp_typedef_name2doc(f->name);_tmp55C[1]=_tmp8D7;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp8D6=Cyc_PP_nil_doc();_tmp55C[2]=_tmp8D6;}):({struct Cyc_PP_Doc*_tmp8D5=Cyc_Absynpp_args2doc(f->typs);_tmp55C[2]=_tmp8D5;});Cyc_PP_cat(_tag_fat(_tmp55C,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2188
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp8D9=({const char*_tmp55D=",";_tag_fat(_tmp55D,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp8D9,fields);});}
# 2197
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp8DA=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp8DA,72,f);});
({void*_tmp55E=0U;({struct Cyc___cycFILE*_tmp8DC=f;struct _fat_ptr _tmp8DB=({const char*_tmp55F="\n";_tag_fat(_tmp55F,sizeof(char),2U);});Cyc_fprintf(_tmp8DC,_tmp8DB,_tag_fat(_tmp55E,sizeof(void*),0U));});});}}
# 2204
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _tmp8DD=({const char*_tmp560="";_tag_fat(_tmp560,sizeof(char),1U);});Cyc_PP_seql(_tmp8DD,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2207
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
# 2221
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
# 2226
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*t){
if(t == 0)
return(struct _fat_ptr)({const char*_tmp561="NULL";_tag_fat(_tmp561,sizeof(char),5U);});{
void*_tmp562=t;struct Cyc_Absyn_Typedefdecl*_tmp563;switch(*((int*)_tmp562)){case 0U: _LL1: _LL2:
 return(struct _fat_ptr)({const char*_tmp564="AppType";_tag_fat(_tmp564,sizeof(char),8U);});case 1U: _LL3: _LL4:
 return(struct _fat_ptr)({const char*_tmp565="Evar";_tag_fat(_tmp565,sizeof(char),5U);});case 2U: _LL5: _LL6:
 return(struct _fat_ptr)({const char*_tmp566="Vartype";_tag_fat(_tmp566,sizeof(char),8U);});case 3U: _LL7: _LL8:
 return(struct _fat_ptr)({const char*_tmp567="Pointertype";_tag_fat(_tmp567,sizeof(char),12U);});case 4U: _LL9: _LLA:
 return(struct _fat_ptr)({const char*_tmp568="ArrayType";_tag_fat(_tmp568,sizeof(char),10U);});case 5U: _LLB: _LLC:
 return(struct _fat_ptr)({const char*_tmp569="FnType";_tag_fat(_tmp569,sizeof(char),7U);});case 6U: _LLD: _LLE:
 return(struct _fat_ptr)({const char*_tmp56A="TupleType";_tag_fat(_tmp56A,sizeof(char),10U);});case 7U: _LLF: _LL10:
 return(struct _fat_ptr)({const char*_tmp56B="AnonAggrType";_tag_fat(_tmp56B,sizeof(char),13U);});case 8U: _LL11: _tmp563=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp562)->f3;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp563;
# 2239
return(struct _fat_ptr)({const char*_tmp56C="Typedeftype";_tag_fat(_tmp56C,sizeof(char),12U);});}case 9U: _LL13: _LL14:
 return(struct _fat_ptr)({const char*_tmp56D="Valueoftype";_tag_fat(_tmp56D,sizeof(char),12U);});case 10U: _LL15: _LL16:
 return(struct _fat_ptr)({const char*_tmp56E="Typedecltype";_tag_fat(_tmp56E,sizeof(char),13U);});default: _LL17: _LL18:
 return(struct _fat_ptr)({const char*_tmp56F="Typeoftype";_tag_fat(_tmp56F,sizeof(char),11U);});}_LL0:;}}

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
# 1130
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1140
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1148
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
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};
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
# 51 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 61
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 69
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 76
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _fat_ptr Cyc_Absynpp_cyc_string;
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple14{int f1;struct Cyc_List_List*f2;};
# 93 "absynpp.h"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 39 "absynpp.cyc"
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
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 67
static int Cyc_Absynpp_expand_typedefs;
# 71
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4U]="Cyc";
# 73
struct _fat_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4U};
struct _fat_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 76
static int Cyc_Absynpp_add_cyc_prefix;
# 80
static int Cyc_Absynpp_to_VC;
# 83
static int Cyc_Absynpp_decls_first;
# 87
static int Cyc_Absynpp_rewrite_temp_tvars;
# 90
static int Cyc_Absynpp_print_all_tvars;
# 93
static int Cyc_Absynpp_print_all_kinds;
# 96
static int Cyc_Absynpp_print_all_effects;
# 99
static int Cyc_Absynpp_print_using_stmts;
# 104
static int Cyc_Absynpp_print_externC_stmts;
# 108
static int Cyc_Absynpp_print_full_evars;
# 111
static int Cyc_Absynpp_generate_line_directives;
# 114
static int Cyc_Absynpp_use_curr_namespace;
# 117
static int Cyc_Absynpp_print_zeroterm;
# 120
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 123
int Cyc_Absynpp_print_for_cycdoc=0;
# 144
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
# 163
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 183
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 203
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 223
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 244
static void Cyc_Absynpp_curr_namespace_add(struct _fat_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp580=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp580,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v,_tmp1->tl=0;_tmp1;}));});}
# 248
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 252
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 256
static void Cyc_Absynpp_curr_namespace_drop(){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 260
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
# 273
 if((int)c >= (int)' ' &&(int)c <= (int)'~'){
struct _fat_ptr _tmpD=Cyc_Core_new_string(2U);
({struct _fat_ptr _tmpE=_fat_ptr_plus(_tmpD,sizeof(char),0);char _tmpF=*((char*)_check_fat_subscript(_tmpE,sizeof(char),0U));char _tmp10=c;if(_get_fat_size(_tmpE,sizeof(char))== 1U &&(_tmpF == 0 && _tmp10 != 0))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _fat_ptr)_tmpD;}else{
# 278
struct _fat_ptr _tmp11=Cyc_Core_new_string(5U);
int j=0;
({struct _fat_ptr _tmp12=_fat_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp13=*((char*)_check_fat_subscript(_tmp12,sizeof(char),0U));char _tmp14='\\';if(_get_fat_size(_tmp12,sizeof(char))== 1U &&(_tmp13 == 0 && _tmp14 != 0))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _fat_ptr _tmp15=_fat_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp16=*((char*)_check_fat_subscript(_tmp15,sizeof(char),0U));char _tmp17=(char)((int)'0' + ((int)((unsigned char)c)>> 6 & 3));if(_get_fat_size(_tmp15,sizeof(char))== 1U &&(_tmp16 == 0 && _tmp17 != 0))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _fat_ptr _tmp18=_fat_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp19=*((char*)_check_fat_subscript(_tmp18,sizeof(char),0U));char _tmp1A=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_fat_size(_tmp18,sizeof(char))== 1U &&(_tmp19 == 0 && _tmp1A != 0))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _fat_ptr _tmp1B=_fat_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp1C=*((char*)_check_fat_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=(char)((int)'0' + ((int)c & 7));if(_get_fat_size(_tmp1B,sizeof(char))== 1U &&(_tmp1C == 0 && _tmp1D != 0))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _fat_ptr)_tmp11;}}_LL0:;}
# 289
static int Cyc_Absynpp_special(struct _fat_ptr s){
int sz=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((((int)c <= (int)' ' ||(int)c >= (int)'~')||(int)c == (int)'"')||(int)c == (int)'\\')
return 1;}}
# 296
return 0;}
# 299
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 302
int n=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
# 304
if(n > 0 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),n))== (int)'\000')-- n;{
# 306
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1E=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _tmp1F=_tmp1E;char _tmp20;switch(_tmp1F){case 7U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL8;case 10U: _LL7: _LL8:
 goto _LLA;case 13U: _LL9: _LLA:
 goto _LLC;case 9U: _LLB: _LLC:
 goto _LLE;case 11U: _LLD: _LLE:
 goto _LL10;case 92U: _LLF: _LL10:
 goto _LL12;case 34U: _LL11: _LL12:
 len +=2;goto _LL0;default: _LL13: _tmp20=_tmp1F;_LL14:
# 319
 if((int)_tmp20 >= (int)' ' &&(int)_tmp20 <= (int)'~')++ len;else{
len +=4;}
goto _LL0;}_LL0:;}}{
# 324
struct _fat_ptr t=Cyc_Core_new_string((unsigned)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp21=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _tmp22=_tmp21;char _tmp68;switch(_tmp22){case 7U: _LL16: _LL17:
({struct _fat_ptr _tmp23=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp24=*((char*)_check_fat_subscript(_tmp23,sizeof(char),0U));char _tmp25='\\';if(_get_fat_size(_tmp23,sizeof(char))== 1U &&(_tmp24 == 0 && _tmp25 != 0))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});({struct _fat_ptr _tmp26=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp27=*((char*)_check_fat_subscript(_tmp26,sizeof(char),0U));char _tmp28='a';if(_get_fat_size(_tmp26,sizeof(char))== 1U &&(_tmp27 == 0 && _tmp28 != 0))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});goto _LL15;case 8U: _LL18: _LL19:
({struct _fat_ptr _tmp29=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp2A=*((char*)_check_fat_subscript(_tmp29,sizeof(char),0U));char _tmp2B='\\';if(_get_fat_size(_tmp29,sizeof(char))== 1U &&(_tmp2A == 0 && _tmp2B != 0))_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});({struct _fat_ptr _tmp2C=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp2D=*((char*)_check_fat_subscript(_tmp2C,sizeof(char),0U));char _tmp2E='b';if(_get_fat_size(_tmp2C,sizeof(char))== 1U &&(_tmp2D == 0 && _tmp2E != 0))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});goto _LL15;case 12U: _LL1A: _LL1B:
({struct _fat_ptr _tmp2F=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp30=*((char*)_check_fat_subscript(_tmp2F,sizeof(char),0U));char _tmp31='\\';if(_get_fat_size(_tmp2F,sizeof(char))== 1U &&(_tmp30 == 0 && _tmp31 != 0))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});({struct _fat_ptr _tmp32=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp33=*((char*)_check_fat_subscript(_tmp32,sizeof(char),0U));char _tmp34='f';if(_get_fat_size(_tmp32,sizeof(char))== 1U &&(_tmp33 == 0 && _tmp34 != 0))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});goto _LL15;case 10U: _LL1C: _LL1D:
({struct _fat_ptr _tmp35=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp36=*((char*)_check_fat_subscript(_tmp35,sizeof(char),0U));char _tmp37='\\';if(_get_fat_size(_tmp35,sizeof(char))== 1U &&(_tmp36 == 0 && _tmp37 != 0))_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});({struct _fat_ptr _tmp38=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp39=*((char*)_check_fat_subscript(_tmp38,sizeof(char),0U));char _tmp3A='n';if(_get_fat_size(_tmp38,sizeof(char))== 1U &&(_tmp39 == 0 && _tmp3A != 0))_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});goto _LL15;case 13U: _LL1E: _LL1F:
({struct _fat_ptr _tmp3B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp3C=*((char*)_check_fat_subscript(_tmp3B,sizeof(char),0U));char _tmp3D='\\';if(_get_fat_size(_tmp3B,sizeof(char))== 1U &&(_tmp3C == 0 && _tmp3D != 0))_throw_arraybounds();*((char*)_tmp3B.curr)=_tmp3D;});({struct _fat_ptr _tmp3E=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp3F=*((char*)_check_fat_subscript(_tmp3E,sizeof(char),0U));char _tmp40='r';if(_get_fat_size(_tmp3E,sizeof(char))== 1U &&(_tmp3F == 0 && _tmp40 != 0))_throw_arraybounds();*((char*)_tmp3E.curr)=_tmp40;});goto _LL15;case 9U: _LL20: _LL21:
({struct _fat_ptr _tmp41=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp42=*((char*)_check_fat_subscript(_tmp41,sizeof(char),0U));char _tmp43='\\';if(_get_fat_size(_tmp41,sizeof(char))== 1U &&(_tmp42 == 0 && _tmp43 != 0))_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});({struct _fat_ptr _tmp44=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp45=*((char*)_check_fat_subscript(_tmp44,sizeof(char),0U));char _tmp46='t';if(_get_fat_size(_tmp44,sizeof(char))== 1U &&(_tmp45 == 0 && _tmp46 != 0))_throw_arraybounds();*((char*)_tmp44.curr)=_tmp46;});goto _LL15;case 11U: _LL22: _LL23:
({struct _fat_ptr _tmp47=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp48=*((char*)_check_fat_subscript(_tmp47,sizeof(char),0U));char _tmp49='\\';if(_get_fat_size(_tmp47,sizeof(char))== 1U &&(_tmp48 == 0 && _tmp49 != 0))_throw_arraybounds();*((char*)_tmp47.curr)=_tmp49;});({struct _fat_ptr _tmp4A=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp4B=*((char*)_check_fat_subscript(_tmp4A,sizeof(char),0U));char _tmp4C='v';if(_get_fat_size(_tmp4A,sizeof(char))== 1U &&(_tmp4B == 0 && _tmp4C != 0))_throw_arraybounds();*((char*)_tmp4A.curr)=_tmp4C;});goto _LL15;case 92U: _LL24: _LL25:
({struct _fat_ptr _tmp4D=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp4E=*((char*)_check_fat_subscript(_tmp4D,sizeof(char),0U));char _tmp4F='\\';if(_get_fat_size(_tmp4D,sizeof(char))== 1U &&(_tmp4E == 0 && _tmp4F != 0))_throw_arraybounds();*((char*)_tmp4D.curr)=_tmp4F;});({struct _fat_ptr _tmp50=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp51=*((char*)_check_fat_subscript(_tmp50,sizeof(char),0U));char _tmp52='\\';if(_get_fat_size(_tmp50,sizeof(char))== 1U &&(_tmp51 == 0 && _tmp52 != 0))_throw_arraybounds();*((char*)_tmp50.curr)=_tmp52;});goto _LL15;case 34U: _LL26: _LL27:
({struct _fat_ptr _tmp53=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp54=*((char*)_check_fat_subscript(_tmp53,sizeof(char),0U));char _tmp55='\\';if(_get_fat_size(_tmp53,sizeof(char))== 1U &&(_tmp54 == 0 && _tmp55 != 0))_throw_arraybounds();*((char*)_tmp53.curr)=_tmp55;});({struct _fat_ptr _tmp56=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp57=*((char*)_check_fat_subscript(_tmp56,sizeof(char),0U));char _tmp58='"';if(_get_fat_size(_tmp56,sizeof(char))== 1U &&(_tmp57 == 0 && _tmp58 != 0))_throw_arraybounds();*((char*)_tmp56.curr)=_tmp58;});goto _LL15;default: _LL28: _tmp68=_tmp22;_LL29:
# 338
 if((int)_tmp68 >= (int)' ' &&(int)_tmp68 <= (int)'~')({struct _fat_ptr _tmp59=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5A=*((char*)_check_fat_subscript(_tmp59,sizeof(char),0U));char _tmp5B=_tmp68;if(_get_fat_size(_tmp59,sizeof(char))== 1U &&(_tmp5A == 0 && _tmp5B != 0))_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});else{
# 340
({struct _fat_ptr _tmp5C=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5D=*((char*)_check_fat_subscript(_tmp5C,sizeof(char),0U));char _tmp5E='\\';if(_get_fat_size(_tmp5C,sizeof(char))== 1U &&(_tmp5D == 0 && _tmp5E != 0))_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});
({struct _fat_ptr _tmp5F=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp60=*((char*)_check_fat_subscript(_tmp5F,sizeof(char),0U));char _tmp61=(char)((int)'0' + ((int)_tmp68 >> 6 & 7));if(_get_fat_size(_tmp5F,sizeof(char))== 1U &&(_tmp60 == 0 && _tmp61 != 0))_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});
({struct _fat_ptr _tmp62=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp63=*((char*)_check_fat_subscript(_tmp62,sizeof(char),0U));char _tmp64=(char)((int)'0' + ((int)_tmp68 >> 3 & 7));if(_get_fat_size(_tmp62,sizeof(char))== 1U &&(_tmp63 == 0 && _tmp64 != 0))_throw_arraybounds();*((char*)_tmp62.curr)=_tmp64;});
({struct _fat_ptr _tmp65=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp66=*((char*)_check_fat_subscript(_tmp65,sizeof(char),0U));char _tmp67=(char)((int)'0' + ((int)_tmp68 & 7));if(_get_fat_size(_tmp65,sizeof(char))== 1U &&(_tmp66 == 0 && _tmp67 != 0))_throw_arraybounds();*((char*)_tmp65.curr)=_tmp67;});}
# 345
goto _LL15;}_LL15:;}}
# 347
return(struct _fat_ptr)t;};};};}static char _tmp69[9U]="restrict";
# 350
static struct _fat_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9U};static char _tmp6A[9U]="volatile";
static struct _fat_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9U};static char _tmp6B[6U]="const";
static struct _fat_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6U};
static struct _fat_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _fat_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _fat_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 357
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 360
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp,_tmp6C->tl=l;_tmp6C;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp,_tmp6D->tl=l;_tmp6D;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp,_tmp6E->tl=l;_tmp6E;});
return({struct _fat_ptr _tmp583=({const char*_tmp6F="";_tag_fat(_tmp6F,sizeof(char),1U);});struct _fat_ptr _tmp582=({const char*_tmp70=" ";_tag_fat(_tmp70,sizeof(char),2U);});struct _fat_ptr _tmp581=({const char*_tmp71=" ";_tag_fat(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp583,_tmp582,_tmp581,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 366
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_KindQual _tmp89;enum Cyc_Absyn_AliasQual _tmp88;_LL1: _tmp89=_tmp72->kind;_tmp88=_tmp72->aliasqual;_LL2:;{
enum Cyc_Absyn_KindQual _tmp73=_tmp89;switch(_tmp73){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 370
enum Cyc_Absyn_AliasQual _tmp74=_tmp88;switch(_tmp74){case Cyc_Absyn_Aliasable: _LL17: _LL18:
 return({const char*_tmp75="A";_tag_fat(_tmp75,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL19: _LL1A:
 return({const char*_tmp76="UA";_tag_fat(_tmp76,sizeof(char),3U);});case Cyc_Absyn_Top: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return({const char*_tmp77="TA";_tag_fat(_tmp77,sizeof(char),3U);});}_LL16:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 376
enum Cyc_Absyn_AliasQual _tmp78=_tmp88;switch(_tmp78){case Cyc_Absyn_Aliasable: _LL20: _LL21:
 return({const char*_tmp79="M";_tag_fat(_tmp79,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL22: _LL23:
 return({const char*_tmp7A="UM";_tag_fat(_tmp7A,sizeof(char),3U);});case Cyc_Absyn_Top: _LL24: _LL25:
 goto _LL27;default: _LL26: _LL27: return({const char*_tmp7B="TM";_tag_fat(_tmp7B,sizeof(char),3U);});}_LL1F:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 382
enum Cyc_Absyn_AliasQual _tmp7C=_tmp88;switch(_tmp7C){case Cyc_Absyn_Aliasable: _LL29: _LL2A:
 return({const char*_tmp7D="B";_tag_fat(_tmp7D,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL2B: _LL2C:
 return({const char*_tmp7E="UB";_tag_fat(_tmp7E,sizeof(char),3U);});case Cyc_Absyn_Top: _LL2D: _LL2E:
 goto _LL30;default: _LL2F: _LL30: return({const char*_tmp7F="TB";_tag_fat(_tmp7F,sizeof(char),3U);});}_LL28:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 388
enum Cyc_Absyn_AliasQual _tmp80=_tmp88;switch(_tmp80){case Cyc_Absyn_Aliasable: _LL32: _LL33:
 return({const char*_tmp81="R";_tag_fat(_tmp81,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL34: _LL35:
 return({const char*_tmp82="UR";_tag_fat(_tmp82,sizeof(char),3U);});case Cyc_Absyn_Top: _LL36: _LL37:
 goto _LL39;default: _LL38: _LL39: return({const char*_tmp83="TR";_tag_fat(_tmp83,sizeof(char),3U);});}_LL31:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 393
 return({const char*_tmp84="E";_tag_fat(_tmp84,sizeof(char),2U);});case Cyc_Absyn_IntKind: _LLE: _LLF:
 return({const char*_tmp85="I";_tag_fat(_tmp85,sizeof(char),2U);});case Cyc_Absyn_BoolKind: _LL10: _LL11:
 return({const char*_tmp86="BOOL";_tag_fat(_tmp86,sizeof(char),5U);});case Cyc_Absyn_PtrBndKind: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15: return({const char*_tmp87="PTRBND";_tag_fat(_tmp87,sizeof(char),7U);});}_LL3:;};}
# 399
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 401
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp8A=Cyc_Absyn_compress_kb(c);void*_tmp8B=_tmp8A;struct Cyc_Absyn_Kind*_tmp92;struct Cyc_Absyn_Kind*_tmp91;switch(*((int*)_tmp8B)){case 0U: _LL1: _tmp91=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B)->f1;_LL2:
 return Cyc_Absynpp_kind2string(_tmp91);case 1U: _LL3: _LL4:
# 405
 if(Cyc_PP_tex_output)
return({const char*_tmp8C="{?}";_tag_fat(_tmp8C,sizeof(char),4U);});else{
return({const char*_tmp8D="?";_tag_fat(_tmp8D,sizeof(char),2U);});}default: _LL5: _tmp92=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B)->f2;_LL6:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp561;_tmp561.tag=0U,({struct _fat_ptr _tmp584=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(_tmp92));_tmp561.f1=_tmp584;});_tmp561;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({struct _fat_ptr _tmp585=({const char*_tmp8F="<=%s";_tag_fat(_tmp8F,sizeof(char),5U);});Cyc_aprintf(_tmp585,_tag_fat(_tmp8E,sizeof(void*),1U));});});}_LL0:;}
# 412
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp93=Cyc_Absyn_compress_kb(c);void*_tmp94=_tmp93;struct Cyc_Absyn_Kind*_tmp98;struct Cyc_Absyn_Kind*_tmp97;switch(*((int*)_tmp94)){case 0U: _LL1: _tmp97=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL2:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp97));case 1U: _LL3: _LL4:
# 416
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp95="{?}";_tag_fat(_tmp95,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp96="?";_tag_fat(_tmp96,sizeof(char),2U);}));}default: _LL5: _tmp98=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;_LL6:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp98));}_LL0:;}
# 423
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp588=({const char*_tmp99="<";_tag_fat(_tmp99,sizeof(char),2U);});struct _fat_ptr _tmp587=({const char*_tmp9A=">";_tag_fat(_tmp9A,sizeof(char),2U);});struct _fat_ptr _tmp586=({const char*_tmp9B=",";_tag_fat(_tmp9B,sizeof(char),2U);});Cyc_PP_egroup(_tmp588,_tmp587,_tmp586,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 427
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr _tmp9C=*tv->name;
# 431
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
# 433
struct _fat_ptr kstring=({const char*_tmpA5="K";_tag_fat(_tmpA5,sizeof(char),2U);});
{void*_tmp9D=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9E=_tmp9D;struct Cyc_Absyn_Kind*_tmpA0;struct Cyc_Absyn_Kind*_tmp9F;switch(*((int*)_tmp9E)){case 2U: _LL1: _tmp9F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f2;_LL2:
 _tmpA0=_tmp9F;goto _LL4;case 0U: _LL3: _tmpA0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9E)->f1;_LL4:
 kstring=Cyc_Absynpp_kind2string(_tmpA0);goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 439
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp563;_tmp563.tag=0U,_tmp563.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_tmp563;});struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp562;_tmp562.tag=0U,({struct _fat_ptr _tmp589=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(_tmp9C,sizeof(char),1));_tmp562.f1=_tmp589;});_tmp562;});void*_tmpA1[2U];_tmpA1[0]=& _tmpA3,_tmpA1[1]=& _tmpA4;({struct _fat_ptr _tmp58A=({const char*_tmpA2="`G%s%s";_tag_fat(_tmpA2,sizeof(char),7U);});Cyc_aprintf(_tmp58A,_tag_fat(_tmpA1,sizeof(void*),2U));});}));}
# 441
return Cyc_PP_text(_tmp9C);}
# 444
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA7=_tmpA6;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Absyn_Kind*_tmpAA;switch(*((int*)_tmpA7)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpAA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL8:
# 449
 return({struct Cyc_PP_Doc*_tmpA8[3U];({struct Cyc_PP_Doc*_tmp58D=Cyc_Absynpp_tvar2doc(tv);_tmpA8[0]=_tmp58D;}),({struct Cyc_PP_Doc*_tmp58C=Cyc_PP_text(({const char*_tmpA9="::";_tag_fat(_tmpA9,sizeof(char),3U);}));_tmpA8[1]=_tmp58C;}),({struct Cyc_PP_Doc*_tmp58B=Cyc_Absynpp_kind2doc(_tmpAA);_tmpA8[2]=_tmp58B;});Cyc_PP_cat(_tag_fat(_tmpA8,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL5: _tmpAB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;_LL6:
# 448
 _tmpAA=_tmpAB;goto _LL8;}_LL0:;}
# 453
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _tmp590=({const char*_tmpAC="<";_tag_fat(_tmpAC,sizeof(char),2U);});struct _fat_ptr _tmp58F=({const char*_tmpAD=">";_tag_fat(_tmpAD,sizeof(char),2U);});struct _fat_ptr _tmp58E=({const char*_tmpAE=",";_tag_fat(_tmpAE,sizeof(char),2U);});Cyc_PP_egroup(_tmp590,_tmp58F,_tmp58E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 457
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _tmp593=({const char*_tmpAF="<";_tag_fat(_tmpAF,sizeof(char),2U);});struct _fat_ptr _tmp592=({const char*_tmpB0=">";_tag_fat(_tmpB0,sizeof(char),2U);});struct _fat_ptr _tmp591=({const char*_tmpB1=",";_tag_fat(_tmpB1,sizeof(char),2U);});Cyc_PP_egroup(_tmp593,_tmp592,_tmp591,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 463
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple17*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 467
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp596=({const char*_tmpB2="(";_tag_fat(_tmpB2,sizeof(char),2U);});struct _fat_ptr _tmp595=({const char*_tmpB3=")";_tag_fat(_tmpB3,sizeof(char),2U);});struct _fat_ptr _tmp594=({const char*_tmpB4=",";_tag_fat(_tmpB4,sizeof(char),2U);});Cyc_PP_group(_tmp596,_tmp595,_tmp594,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 471
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB5=att;switch(*((int*)_tmpB5)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();default: _LL7: _LL8:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL0:;}
# 480
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB6=(void*)atts->hd;void*_tmpB7=_tmpB6;switch(*((int*)_tmpB7)){case 1U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmpB8=" _stdcall ";_tag_fat(_tmpB8,sizeof(char),11U);}));case 2U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmpB9=" _cdecl ";_tag_fat(_tmpB9,sizeof(char),9U);}));case 3U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmpBA=" _fastcall ";_tag_fat(_tmpBA,sizeof(char),12U);}));default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 488
return Cyc_PP_nil_doc();}
# 491
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpBB=(void*)atts2->hd;void*_tmpBC=_tmpBB;switch(*((int*)_tmpBC)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 500
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBD[3U];({struct Cyc_PP_Doc*_tmp59C=Cyc_PP_text(({const char*_tmpBE=" __declspec(";_tag_fat(_tmpBE,sizeof(char),13U);}));_tmpBD[0]=_tmp59C;}),({
struct Cyc_PP_Doc*_tmp59B=({struct _fat_ptr _tmp59A=({const char*_tmpBF="";_tag_fat(_tmpBF,sizeof(char),1U);});struct _fat_ptr _tmp599=({const char*_tmpC0="";_tag_fat(_tmpC0,sizeof(char),1U);});struct _fat_ptr _tmp598=({const char*_tmpC1=" ";_tag_fat(_tmpC1,sizeof(char),2U);});Cyc_PP_group(_tmp59A,_tmp599,_tmp598,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBD[1]=_tmp59B;}),({
struct Cyc_PP_Doc*_tmp597=Cyc_PP_text(({const char*_tmpC2=")";_tag_fat(_tmpC2,sizeof(char),2U);}));_tmpBD[2]=_tmp597;});Cyc_PP_cat(_tag_fat(_tmpBD,sizeof(struct Cyc_PP_Doc*),3U));});}
# 507
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 511
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC3=Cyc_Cyclone_c_compiler;if(_tmpC3 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 516
 return({struct Cyc_PP_Doc*_tmpC4[2U];({struct Cyc_PP_Doc*_tmp5A1=Cyc_PP_text(({const char*_tmpC5=" __attribute__";_tag_fat(_tmpC5,sizeof(char),15U);}));_tmpC4[0]=_tmp5A1;}),({
struct Cyc_PP_Doc*_tmp5A0=({struct _fat_ptr _tmp59F=({const char*_tmpC6="((";_tag_fat(_tmpC6,sizeof(char),3U);});struct _fat_ptr _tmp59E=({const char*_tmpC7="))";_tag_fat(_tmpC7,sizeof(char),3U);});struct _fat_ptr _tmp59D=({const char*_tmpC8=",";_tag_fat(_tmpC8,sizeof(char),2U);});Cyc_PP_group(_tmp59F,_tmp59E,_tmp59D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC4[1]=_tmp5A0;});Cyc_PP_cat(_tag_fat(_tmpC4,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;};}
# 521
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC9=(void*)tms->hd;void*_tmpCA=_tmpC9;switch(*((int*)_tmpCA)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 526
enum Cyc_Cyclone_C_Compilers _tmpCB=Cyc_Cyclone_c_compiler;if(_tmpCB == Cyc_Cyclone_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 530
 return 0;}_LL0:;};}
# 534
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 537
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmpCC="{?}";_tag_fat(_tmpCC,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmpCD="?";_tag_fat(_tmpCD,sizeof(char),2U);}));}}
# 544
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 546
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmpCE="{\\lb}";_tag_fat(_tmpCE,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmpCF="{";_tag_fat(_tmpCF,sizeof(char),2U);}));}}
# 553
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 555
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmpD0="{\\rb}";_tag_fat(_tmpD0,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmpD1="}";_tag_fat(_tmpD1,sizeof(char),2U);}));}}
# 562
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 564
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmpD2="\\$";_tag_fat(_tmpD2,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmpD3="$";_tag_fat(_tmpD3,sizeof(char),2U);}));}}
# 571
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 573
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD4[3U];({struct Cyc_PP_Doc*_tmp5A4=Cyc_Absynpp_lb();_tmpD4[0]=_tmp5A4;}),({struct Cyc_PP_Doc*_tmp5A3=Cyc_PP_seq(sep,ss);_tmpD4[1]=_tmp5A3;}),({struct Cyc_PP_Doc*_tmp5A2=Cyc_Absynpp_rb();_tmpD4[2]=_tmp5A2;});Cyc_PP_cat(_tag_fat(_tmpD4,sizeof(struct Cyc_PP_Doc*),3U));});}
# 578
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD5=(void*)tms->hd;void*_tmpD6=_tmpD5;struct Cyc_List_List*_tmpED;switch(*((int*)_tmpD6)){case 0U: _LL1: _LL2:
({void*_tmpD7=0U;({struct Cyc___cycFILE*_tmp5A6=Cyc_stderr;struct _fat_ptr _tmp5A5=({const char*_tmpD8="Carray_mod ";_tag_fat(_tmpD8,sizeof(char),12U);});Cyc_fprintf(_tmp5A6,_tmp5A5,_tag_fat(_tmpD7,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpD9=0U;({struct Cyc___cycFILE*_tmp5A8=Cyc_stderr;struct _fat_ptr _tmp5A7=({const char*_tmpDA="ConstArray_mod ";_tag_fat(_tmpDA,sizeof(char),16U);});Cyc_fprintf(_tmp5A8,_tmp5A7,_tag_fat(_tmpD9,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->tag == 1U){_LL5: _tmpED=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->f1;_LL6:
# 584
({void*_tmpDB=0U;({struct Cyc___cycFILE*_tmp5AA=Cyc_stderr;struct _fat_ptr _tmp5A9=({const char*_tmpDC="Function_mod(";_tag_fat(_tmpDC,sizeof(char),14U);});Cyc_fprintf(_tmp5AA,_tmp5A9,_tag_fat(_tmpDB,sizeof(void*),0U));});});
for(0;_tmpED != 0;_tmpED=_tmpED->tl){
struct _fat_ptr*_tmpDD=(*((struct _tuple8*)_tmpED->hd)).f1;
if(_tmpDD == 0)({void*_tmpDE=0U;({struct Cyc___cycFILE*_tmp5AC=Cyc_stderr;struct _fat_ptr _tmp5AB=({const char*_tmpDF="?";_tag_fat(_tmpDF,sizeof(char),2U);});Cyc_fprintf(_tmp5AC,_tmp5AB,_tag_fat(_tmpDE,sizeof(void*),0U));});});else{
({void*_tmpE0=0U;({struct Cyc___cycFILE*_tmp5AE=Cyc_stderr;struct _fat_ptr _tmp5AD=*_tmpDD;Cyc_fprintf(_tmp5AE,_tmp5AD,_tag_fat(_tmpE0,sizeof(void*),0U));});});}
if(_tmpED->tl != 0)({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp5B0=Cyc_stderr;struct _fat_ptr _tmp5AF=({const char*_tmpE2=",";_tag_fat(_tmpE2,sizeof(char),2U);});Cyc_fprintf(_tmp5B0,_tmp5AF,_tag_fat(_tmpE1,sizeof(void*),0U));});});}
# 591
({void*_tmpE3=0U;({struct Cyc___cycFILE*_tmp5B2=Cyc_stderr;struct _fat_ptr _tmp5B1=({const char*_tmpE4=") ";_tag_fat(_tmpE4,sizeof(char),3U);});Cyc_fprintf(_tmp5B2,_tmp5B1,_tag_fat(_tmpE3,sizeof(void*),0U));});});
goto _LL0;}else{_LL7: _LL8:
# 594
({void*_tmpE5=0U;({struct Cyc___cycFILE*_tmp5B4=Cyc_stderr;struct _fat_ptr _tmp5B3=({const char*_tmpE6="Function_mod()";_tag_fat(_tmpE6,sizeof(char),15U);});Cyc_fprintf(_tmp5B4,_tmp5B3,_tag_fat(_tmpE5,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE7=0U;({struct Cyc___cycFILE*_tmp5B6=Cyc_stderr;struct _fat_ptr _tmp5B5=({const char*_tmpE8="Attributes_mod ";_tag_fat(_tmpE8,sizeof(char),16U);});Cyc_fprintf(_tmp5B6,_tmp5B5,_tag_fat(_tmpE7,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpE9=0U;({struct Cyc___cycFILE*_tmp5B8=Cyc_stderr;struct _fat_ptr _tmp5B7=({const char*_tmpEA="TypeParams_mod ";_tag_fat(_tmpEA,sizeof(char),16U);});Cyc_fprintf(_tmp5B8,_tmp5B7,_tag_fat(_tmpE9,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpEB=0U;({struct Cyc___cycFILE*_tmp5BA=Cyc_stderr;struct _fat_ptr _tmp5B9=({const char*_tmpEC="Pointer_mod ";_tag_fat(_tmpEC,sizeof(char),13U);});Cyc_fprintf(_tmp5BA,_tmp5B9,_tag_fat(_tmpEB,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 599
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp5BC=Cyc_stderr;struct _fat_ptr _tmp5BB=({const char*_tmpEF="\n";_tag_fat(_tmpEF,sizeof(char),2U);});Cyc_fprintf(_tmp5BC,_tmp5BB,_tag_fat(_tmpEE,sizeof(void*),0U));});});}
# 602
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 604
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp134[3U];({struct Cyc_PP_Doc*_tmp5BE=Cyc_PP_text(({const char*_tmp135="(";_tag_fat(_tmp135,sizeof(char),2U);}));_tmp134[0]=_tmp5BE;}),_tmp134[1]=rest,({struct Cyc_PP_Doc*_tmp5BD=Cyc_PP_text(({const char*_tmp136=")";_tag_fat(_tmp136,sizeof(char),2U);}));_tmp134[2]=_tmp5BD;});Cyc_PP_cat(_tag_fat(_tmp134,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpF0=(void*)tms->hd;void*_tmpF1=_tmpF0;void*_tmp133;void*_tmp132;void*_tmp131;void*_tmp130;struct Cyc_Absyn_Tqual _tmp12F;struct Cyc_List_List*_tmp12E;unsigned _tmp12D;int _tmp12C;struct Cyc_List_List*_tmp12B;void*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;void*_tmp128;void*_tmp127;switch(*((int*)_tmpF1)){case 0U: _LL1: _tmp127=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL2:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF2[2U];_tmpF2[0]=rest,Cyc_Absyn_type2bool(0,_tmp127)?({
struct Cyc_PP_Doc*_tmp5C0=Cyc_PP_text(({const char*_tmpF3="[]@zeroterm";_tag_fat(_tmpF3,sizeof(char),12U);}));_tmpF2[1]=_tmp5C0;}):({struct Cyc_PP_Doc*_tmp5BF=Cyc_PP_text(({const char*_tmpF4="[]";_tag_fat(_tmpF4,sizeof(char),3U);}));_tmpF2[1]=_tmp5BF;});Cyc_PP_cat(_tag_fat(_tmpF2,sizeof(struct Cyc_PP_Doc*),2U));});case 1U: _LL3: _tmp129=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmp128=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL4:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF5[4U];_tmpF5[0]=rest,({struct Cyc_PP_Doc*_tmp5C4=Cyc_PP_text(({const char*_tmpF6="[";_tag_fat(_tmpF6,sizeof(char),2U);}));_tmpF5[1]=_tmp5C4;}),({struct Cyc_PP_Doc*_tmp5C3=Cyc_Absynpp_exp2doc(_tmp129);_tmpF5[2]=_tmp5C3;}),
Cyc_Absyn_type2bool(0,_tmp128)?({struct Cyc_PP_Doc*_tmp5C2=Cyc_PP_text(({const char*_tmpF7="]@zeroterm";_tag_fat(_tmpF7,sizeof(char),11U);}));_tmpF5[3]=_tmp5C2;}):({struct Cyc_PP_Doc*_tmp5C1=Cyc_PP_text(({const char*_tmpF8="]";_tag_fat(_tmpF8,sizeof(char),2U);}));_tmpF5[3]=_tmp5C1;});Cyc_PP_cat(_tag_fat(_tmpF5,sizeof(struct Cyc_PP_Doc*),4U));});case 3U: _LL5: _tmp12A=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL6:
# 618
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmpF9=_tmp12A;struct Cyc_List_List*_tmp107;unsigned _tmp106;struct Cyc_List_List*_tmp105;int _tmp104;struct Cyc_Absyn_VarargInfo*_tmp103;void*_tmp102;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->tag == 1U){_LLE: _tmp105=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f1;_tmp104=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f2;_tmp103=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f3;_tmp102=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f4;_tmp101=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f5;_tmp100=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f6;_tmpFF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f7;_LLF:
# 621
 return({struct Cyc_PP_Doc*_tmpFA[2U];_tmpFA[0]=rest,({struct Cyc_PP_Doc*_tmp5C5=Cyc_Absynpp_funargs2doc(_tmp105,_tmp104,_tmp103,_tmp102,_tmp101,_tmp100,_tmpFF);_tmpFA[1]=_tmp5C5;});Cyc_PP_cat(_tag_fat(_tmpFA,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL10: _tmp107=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF9)->f1;_tmp106=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF9)->f2;_LL11:
# 623
 return({struct Cyc_PP_Doc*_tmpFB[2U];_tmpFB[0]=rest,({struct Cyc_PP_Doc*_tmp5C9=({struct _fat_ptr _tmp5C8=({const char*_tmpFC="(";_tag_fat(_tmpFC,sizeof(char),2U);});struct _fat_ptr _tmp5C7=({const char*_tmpFD=")";_tag_fat(_tmpFD,sizeof(char),2U);});struct _fat_ptr _tmp5C6=({const char*_tmpFE=",";_tag_fat(_tmpFE,sizeof(char),2U);});Cyc_PP_group(_tmp5C8,_tmp5C7,_tmp5C6,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp107));});_tmpFB[1]=_tmp5C9;});Cyc_PP_cat(_tag_fat(_tmpFB,sizeof(struct Cyc_PP_Doc*),2U));});}_LLD:;};case 5U: _LL7: _tmp12B=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL8: {
# 626
enum Cyc_Cyclone_C_Compilers _tmp108=Cyc_Cyclone_c_compiler;if(_tmp108 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp109[2U];_tmp109[0]=rest,({struct Cyc_PP_Doc*_tmp5CA=Cyc_Absynpp_atts2doc(_tmp12B);_tmp109[1]=_tmp5CA;});Cyc_PP_cat(_tag_fat(_tmp109,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp10A[2U];({struct Cyc_PP_Doc*_tmp5CB=Cyc_Absynpp_callconv2doc(_tmp12B);_tmp10A[0]=_tmp5CB;}),_tmp10A[1]=rest;Cyc_PP_cat(_tag_fat(_tmp10A,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp12E=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmp12D=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_tmp12C=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f3;_LLA:
# 637
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp12C)
return({struct Cyc_PP_Doc*_tmp10B[2U];_tmp10B[0]=rest,({struct Cyc_PP_Doc*_tmp5CC=Cyc_Absynpp_ktvars2doc(_tmp12E);_tmp10B[1]=_tmp5CC;});Cyc_PP_cat(_tag_fat(_tmp10B,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 641
return({struct Cyc_PP_Doc*_tmp10C[2U];_tmp10C[0]=rest,({struct Cyc_PP_Doc*_tmp5CD=Cyc_Absynpp_tvars2doc(_tmp12E);_tmp10C[1]=_tmp5CD;});Cyc_PP_cat(_tag_fat(_tmp10C,sizeof(struct Cyc_PP_Doc*),2U));});}default: _LLB: _tmp133=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).rgn;_tmp132=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).nullable;_tmp131=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).bounds;_tmp130=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).zero_term;_tmp12F=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LLC: {
# 645
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp12F);
{void*_tmp10D=Cyc_Tcutil_compress(_tmp131);void*_tmp10E=_tmp10D;void*_tmp11F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2 != 0){_LL1A: _tmp11F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2)->hd;_LL1B:
# 653
{void*_tmp10F=Cyc_Tcutil_compress(_tmp11F);void*_tmp110=_tmp10F;struct Cyc_Absyn_Exp*_tmp11B;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp110)->tag == 9U){_LL1F: _tmp11B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp110)->f1;_LL20:
# 655
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp132)?({const char*_tmp111="*";_tag_fat(_tmp111,sizeof(char),2U);}):({const char*_tmp112="@";_tag_fat(_tmp112,sizeof(char),2U);}));{
struct _tuple12 _tmp113=Cyc_Evexp_eval_const_uint_exp(_tmp11B);struct _tuple12 _tmp114=_tmp113;unsigned _tmp117;int _tmp116;_LL24: _tmp117=_tmp114.f1;_tmp116=_tmp114.f2;_LL25:;
if(!_tmp116 || _tmp117 != (unsigned)1)
ptr=({struct Cyc_PP_Doc*_tmp115[4U];_tmp115[0]=ptr,({struct Cyc_PP_Doc*_tmp5D0=Cyc_Absynpp_lb();_tmp115[1]=_tmp5D0;}),({struct Cyc_PP_Doc*_tmp5CF=Cyc_Absynpp_exp2doc(_tmp11B);_tmp115[2]=_tmp5CF;}),({struct Cyc_PP_Doc*_tmp5CE=Cyc_Absynpp_rb();_tmp115[3]=_tmp5CE;});Cyc_PP_cat(_tag_fat(_tmp115,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;};}else{_LL21: _LL22:
# 661
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp132)?({const char*_tmp118="*";_tag_fat(_tmp118,sizeof(char),2U);}):({const char*_tmp119="@";_tag_fat(_tmp119,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp11A[4U];_tmp11A[0]=ptr,({struct Cyc_PP_Doc*_tmp5D3=Cyc_Absynpp_lb();_tmp11A[1]=_tmp5D3;}),({struct Cyc_PP_Doc*_tmp5D2=Cyc_Absynpp_typ2doc(_tmp11F);_tmp11A[2]=_tmp5D2;}),({struct Cyc_PP_Doc*_tmp5D1=Cyc_Absynpp_rb();_tmp11A[3]=_tmp5D1;});Cyc_PP_cat(_tag_fat(_tmp11A,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}_LL1E:;}
# 665
goto _LL17;}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 667
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp132)?({const char*_tmp11C="*";_tag_fat(_tmp11C,sizeof(char),2U);}):({const char*_tmp11D="@";_tag_fat(_tmp11D,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp11E[4U];_tmp11E[0]=ptr,({struct Cyc_PP_Doc*_tmp5D6=Cyc_Absynpp_lb();_tmp11E[1]=_tmp5D6;}),({struct Cyc_PP_Doc*_tmp5D5=Cyc_Absynpp_typ2doc(_tmp131);_tmp11E[2]=_tmp5D5;}),({struct Cyc_PP_Doc*_tmp5D4=Cyc_Absynpp_rb();_tmp11E[3]=_tmp5D4;});Cyc_PP_cat(_tag_fat(_tmp11E,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 672
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr && Cyc_Absyn_type2bool(0,_tmp130))
ztd=Cyc_PP_text(({const char*_tmp120="@zeroterm";_tag_fat(_tmp120,sizeof(char),10U);}));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,_tmp130))
ztd=Cyc_PP_text(({const char*_tmp121="@nozeroterm";_tag_fat(_tmp121,sizeof(char),12U);}));}}
# 678
{void*_tmp122=Cyc_Tcutil_compress(_tmp133);void*_tmp123=_tmp122;switch(*((int*)_tmp123)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp123)->f1)->tag == 5U){_LL27: _LL28:
 goto _LL26;}else{goto _LL2B;}case 1U: _LL29: if(Cyc_Absynpp_print_for_cycdoc){_LL2A:
 goto _LL26;}else{goto _LL2B;}default: _LL2B: _LL2C:
 rgd=Cyc_Absynpp_rgn2doc(_tmp133);}_LL26:;}{
# 683
struct Cyc_PP_Doc*spacer1=tqd != mt &&(ztd != mt || rgd != mt)?Cyc_PP_text(({const char*_tmp126=" ";_tag_fat(_tmp126,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp125=" ";_tag_fat(_tmp125,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp124[7U];_tmp124[0]=ptr,_tmp124[1]=ztd,_tmp124[2]=rgd,_tmp124[3]=spacer1,_tmp124[4]=tqd,_tmp124[5]=spacer2,_tmp124[6]=rest;Cyc_PP_cat(_tag_fat(_tmp124,sizeof(struct Cyc_PP_Doc*),7U));});};}}_LL0:;};}
# 689
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_tmp138=_tmp137;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp138)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp138)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp139="`H";_tag_fat(_tmp139,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp13A="`U";_tag_fat(_tmp13A,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp13B="`RC";_tag_fat(_tmp13B,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 699
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 703
void*_tmp13C=Cyc_Tcutil_compress(t);void*_tmp13D=_tmp13C;struct Cyc_List_List*_tmp141;void*_tmp140;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2 != 0){_LL1: _tmp140=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2)->hd;_LL2:
({struct Cyc_List_List*_tmp5D8=({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));({struct Cyc_PP_Doc*_tmp5D7=Cyc_Absynpp_rgn2doc(_tmp140);_tmp13E->hd=_tmp5D7;}),_tmp13E->tl=*rgions;_tmp13E;});*rgions=_tmp5D8;});goto _LL0;}else{goto _LL5;}case 9U: _LL3: _tmp141=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2;_LL4:
# 706
 for(0;_tmp141 != 0;_tmp141=_tmp141->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp141->hd);}
# 709
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp5DA=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));({struct Cyc_PP_Doc*_tmp5D9=Cyc_Absynpp_typ2doc(t);_tmp13F->hd=_tmp5D9;}),_tmp13F->tl=*effects;_tmp13F;});*effects=_tmp5DA;});goto _LL0;}_LL0:;}
# 714
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0 && effects != 0)
return({struct _fat_ptr _tmp5DD=({const char*_tmp142="";_tag_fat(_tmp142,sizeof(char),1U);});struct _fat_ptr _tmp5DC=({const char*_tmp143="";_tag_fat(_tmp143,sizeof(char),1U);});struct _fat_ptr _tmp5DB=({const char*_tmp144="+";_tag_fat(_tmp144,sizeof(char),2U);});Cyc_PP_group(_tmp5DD,_tmp5DC,_tmp5DB,effects);});else{
# 722
struct Cyc_PP_Doc*_tmp145=({struct _fat_ptr _tmp5DE=({const char*_tmp14A=",";_tag_fat(_tmp14A,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp5DE,rgions);});
return({struct _fat_ptr _tmp5E2=({const char*_tmp146="";_tag_fat(_tmp146,sizeof(char),1U);});struct _fat_ptr _tmp5E1=({const char*_tmp147="";_tag_fat(_tmp147,sizeof(char),1U);});struct _fat_ptr _tmp5E0=({const char*_tmp148="+";_tag_fat(_tmp148,sizeof(char),2U);});Cyc_PP_group(_tmp5E2,_tmp5E1,_tmp5E0,({struct Cyc_List_List*_tmp5DF=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5DF,({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=_tmp145,_tmp149->tl=0;_tmp149;}));}));});}}
# 727
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp14B=k;if(_tmp14B == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp14C="struct ";_tag_fat(_tmp14C,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp14D="union ";_tag_fat(_tmp14D,sizeof(char),7U);}));}_LL0:;}
# 735
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp14E=t;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Enumdecl*_tmp1C4;struct Cyc_Absyn_Aggrdecl*_tmp1C3;struct _tuple0*_tmp1C2;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_Typedefdecl*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1BE;enum Cyc_Absyn_AggrKind _tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BB;struct Cyc_Absyn_Tvar*_tmp1BA;struct Cyc_Core_Opt*_tmp1B9;void*_tmp1B8;int _tmp1B7;struct Cyc_Core_Opt*_tmp1B6;void*_tmp1B5;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct Cyc_List_List*_tmp1B2;struct _fat_ptr _tmp1B1;struct _tuple0*_tmp1B0;struct Cyc_List_List*_tmp1AF;union Cyc_Absyn_AggrInfo _tmp1AE;struct Cyc_List_List*_tmp1AD;int _tmp1AC;enum Cyc_Absyn_Sign _tmp1AB;enum Cyc_Absyn_Size_of _tmp1AA;union Cyc_Absyn_DatatypeFieldInfo _tmp1A9;struct Cyc_List_List*_tmp1A8;union Cyc_Absyn_DatatypeInfo _tmp1A7;struct Cyc_List_List*_tmp1A6;switch(*((int*)_tmp14E)){case 4U: _LL1: _LL2:
# 739
 return Cyc_PP_text(({const char*_tmp14F="[[[array]]]";_tag_fat(_tmp14F,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)){case 0U: _LL7: _LL8:
# 743
 s=Cyc_PP_text(({const char*_tmp150="void";_tag_fat(_tmp150,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp1A7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp1A6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LLE:
# 759
{union Cyc_Absyn_DatatypeInfo _tmp158=_tmp1A7;struct _tuple0*_tmp161;int _tmp160;struct _tuple0*_tmp15F;int _tmp15E;if((_tmp158.UnknownDatatype).tag == 1){_LL48: _tmp15F=((_tmp158.UnknownDatatype).val).name;_tmp15E=((_tmp158.UnknownDatatype).val).is_extensible;_LL49:
 _tmp161=_tmp15F;_tmp160=_tmp15E;goto _LL4B;}else{_LL4A: _tmp161=(*(_tmp158.KnownDatatype).val)->name;_tmp160=(*(_tmp158.KnownDatatype).val)->is_extensible;_LL4B: {
# 762
struct Cyc_PP_Doc*_tmp159=Cyc_PP_text(({const char*_tmp15D="datatype ";_tag_fat(_tmp15D,sizeof(char),10U);}));
struct Cyc_PP_Doc*_tmp15A=_tmp160?Cyc_PP_text(({const char*_tmp15C="@extensible ";_tag_fat(_tmp15C,sizeof(char),13U);})): Cyc_PP_nil_doc();
s=({struct Cyc_PP_Doc*_tmp15B[4U];_tmp15B[0]=_tmp15A,_tmp15B[1]=_tmp159,({struct Cyc_PP_Doc*_tmp5E4=Cyc_Absynpp_qvar2doc(_tmp161);_tmp15B[2]=_tmp5E4;}),({struct Cyc_PP_Doc*_tmp5E3=Cyc_Absynpp_tps2doc(_tmp1A6);_tmp15B[3]=_tmp5E3;});Cyc_PP_cat(_tag_fat(_tmp15B,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 767
goto _LL0;case 19U: _LLF: _tmp1A9=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp1A8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL10:
# 769
{union Cyc_Absyn_DatatypeFieldInfo _tmp162=_tmp1A9;struct _tuple0*_tmp16D;int _tmp16C;struct _tuple0*_tmp16B;struct _tuple0*_tmp16A;struct _tuple0*_tmp169;int _tmp168;if((_tmp162.UnknownDatatypefield).tag == 1){_LL4D: _tmp16A=((_tmp162.UnknownDatatypefield).val).datatype_name;_tmp169=((_tmp162.UnknownDatatypefield).val).field_name;_tmp168=((_tmp162.UnknownDatatypefield).val).is_extensible;_LL4E:
# 771
 _tmp16D=_tmp16A;_tmp16C=_tmp168;_tmp16B=_tmp169;goto _LL50;}else{_LL4F: _tmp16D=(((_tmp162.KnownDatatypefield).val).f1)->name;_tmp16C=(((_tmp162.KnownDatatypefield).val).f1)->is_extensible;_tmp16B=(((_tmp162.KnownDatatypefield).val).f2)->name;_LL50: {
# 774
struct Cyc_PP_Doc*_tmp163=Cyc_PP_text(_tmp16C?({const char*_tmp166="@extensible datatype ";_tag_fat(_tmp166,sizeof(char),22U);}):({const char*_tmp167="datatype ";_tag_fat(_tmp167,sizeof(char),10U);}));
s=({struct Cyc_PP_Doc*_tmp164[4U];_tmp164[0]=_tmp163,({struct Cyc_PP_Doc*_tmp5E7=Cyc_Absynpp_qvar2doc(_tmp16D);_tmp164[1]=_tmp5E7;}),({struct Cyc_PP_Doc*_tmp5E6=Cyc_PP_text(({const char*_tmp165=".";_tag_fat(_tmp165,sizeof(char),2U);}));_tmp164[2]=_tmp5E6;}),({struct Cyc_PP_Doc*_tmp5E5=Cyc_Absynpp_qvar2doc(_tmp16B);_tmp164[3]=_tmp5E5;});Cyc_PP_cat(_tag_fat(_tmp164,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 778
goto _LL0;case 1U: _LL11: _tmp1AB=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp1AA=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f2;_LL12: {
# 780
struct _fat_ptr sns;
struct _fat_ptr ts;
{enum Cyc_Absyn_Sign _tmp16E=_tmp1AB;switch(_tmp16E){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp16F="";_tag_fat(_tmp16F,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp170="unsigned ";_tag_fat(_tmp170,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 787
{enum Cyc_Absyn_Size_of _tmp171=_tmp1AA;switch(_tmp171){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 789
{enum Cyc_Absyn_Sign _tmp172=_tmp1AB;switch(_tmp172){case Cyc_Absyn_None: _LL66: _LL67:
 sns=({const char*_tmp173="";_tag_fat(_tmp173,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed: _LL68: _LL69:
 sns=({const char*_tmp174="signed ";_tag_fat(_tmp174,sizeof(char),8U);});goto _LL65;default: _LL6A: _LL6B:
 sns=({const char*_tmp175="unsigned ";_tag_fat(_tmp175,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 794
ts=({const char*_tmp176="char";_tag_fat(_tmp176,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp177="short";_tag_fat(_tmp177,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp178="int";_tag_fat(_tmp178,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp179="long";_tag_fat(_tmp179,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
# 801
{enum Cyc_Cyclone_C_Compilers _tmp17A=Cyc_Cyclone_c_compiler;if(_tmp17A == Cyc_Cyclone_Gcc_c){_LL6D: _LL6E:
 ts=({const char*_tmp17B="long long";_tag_fat(_tmp17B,sizeof(char),10U);});goto _LL6C;}else{_LL6F: _LL70:
 ts=({const char*_tmp17C="__int64";_tag_fat(_tmp17C,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 805
goto _LL58;}_LL58:;}
# 807
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp565;_tmp565.tag=0U,_tmp565.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_tmp565;});struct Cyc_String_pa_PrintArg_struct _tmp180=({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0U,_tmp564.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_tmp564;});void*_tmp17D[2U];_tmp17D[0]=& _tmp17F,_tmp17D[1]=& _tmp180;({struct _fat_ptr _tmp5E8=({const char*_tmp17E="%s%s";_tag_fat(_tmp17E,sizeof(char),5U);});Cyc_aprintf(_tmp5E8,_tag_fat(_tmp17D,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp1AC=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL14:
# 810
{int _tmp181=_tmp1AC;switch(_tmp181){case 0U: _LL72: _LL73:
 s=Cyc_PP_text(({const char*_tmp182="float";_tag_fat(_tmp182,sizeof(char),6U);}));goto _LL71;case 1U: _LL74: _LL75:
 s=Cyc_PP_text(({const char*_tmp183="double";_tag_fat(_tmp183,sizeof(char),7U);}));goto _LL71;default: _LL76: _LL77:
 s=Cyc_PP_text(({const char*_tmp184="long double";_tag_fat(_tmp184,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 815
goto _LL0;case 20U: _LL17: _tmp1AE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp1AD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL18: {
# 818
struct _tuple11 _tmp186=Cyc_Absyn_aggr_kinded_name(_tmp1AE);struct _tuple11 _tmp187=_tmp186;enum Cyc_Absyn_AggrKind _tmp18A;struct _tuple0*_tmp189;_LL79: _tmp18A=_tmp187.f1;_tmp189=_tmp187.f2;_LL7A:;
s=({struct Cyc_PP_Doc*_tmp188[3U];({struct Cyc_PP_Doc*_tmp5EB=Cyc_Absynpp_aggr_kind2doc(_tmp18A);_tmp188[0]=_tmp5EB;}),({struct Cyc_PP_Doc*_tmp5EA=Cyc_Absynpp_qvar2doc(_tmp189);_tmp188[1]=_tmp5EA;}),({struct Cyc_PP_Doc*_tmp5E9=Cyc_Absynpp_tps2doc(_tmp1AD);_tmp188[2]=_tmp5E9;});Cyc_PP_cat(_tag_fat(_tmp188,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 16U: _LL1B: _tmp1AF=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1C:
# 827
 s=({struct Cyc_PP_Doc*_tmp18C[4U];({struct Cyc_PP_Doc*_tmp5EF=Cyc_PP_text(({const char*_tmp18D="enum ";_tag_fat(_tmp18D,sizeof(char),6U);}));_tmp18C[0]=_tmp5EF;}),({struct Cyc_PP_Doc*_tmp5EE=Cyc_Absynpp_lb();_tmp18C[1]=_tmp5EE;}),({struct Cyc_PP_Doc*_tmp5ED=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp1AF));_tmp18C[2]=_tmp5ED;}),({struct Cyc_PP_Doc*_tmp5EC=Cyc_Absynpp_rb();_tmp18C[3]=_tmp5EC;});Cyc_PP_cat(_tag_fat(_tmp18C,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 15U: _LL1D: _tmp1B0=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1E:
 s=({struct Cyc_PP_Doc*_tmp18E[2U];({struct Cyc_PP_Doc*_tmp5F1=Cyc_PP_text(({const char*_tmp18F="enum ";_tag_fat(_tmp18F,sizeof(char),6U);}));_tmp18E[0]=_tmp5F1;}),({struct Cyc_PP_Doc*_tmp5F0=Cyc_Absynpp_qvar2doc(_tmp1B0);_tmp18E[1]=_tmp5F0;});Cyc_PP_cat(_tag_fat(_tmp18E,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 17U: _LL23: _tmp1B1=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL24:
# 831
 s=Cyc_PP_text(_tmp1B1);goto _LL0;case 3U: _LL27: _tmp1B2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL28:
# 841
 s=({struct Cyc_PP_Doc*_tmp197[3U];({struct Cyc_PP_Doc*_tmp5F4=Cyc_PP_text(({const char*_tmp198="region_t<";_tag_fat(_tmp198,sizeof(char),10U);}));_tmp197[0]=_tmp5F4;}),({struct Cyc_PP_Doc*_tmp5F3=Cyc_Absynpp_rgn2doc((void*)((struct Cyc_List_List*)_check_null(_tmp1B2))->hd);_tmp197[1]=_tmp5F3;}),({struct Cyc_PP_Doc*_tmp5F2=Cyc_PP_text(({const char*_tmp199=">";_tag_fat(_tmp199,sizeof(char),2U);}));_tmp197[2]=_tmp5F2;});Cyc_PP_cat(_tag_fat(_tmp197,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 4U: _LL29: _tmp1B3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL2A:
# 843
 s=({struct Cyc_PP_Doc*_tmp19A[3U];({struct Cyc_PP_Doc*_tmp5F7=Cyc_PP_text(({const char*_tmp19B="tag_t<";_tag_fat(_tmp19B,sizeof(char),7U);}));_tmp19A[0]=_tmp5F7;}),({struct Cyc_PP_Doc*_tmp5F6=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(_tmp1B3))->hd);_tmp19A[1]=_tmp5F6;}),({struct Cyc_PP_Doc*_tmp5F5=Cyc_PP_text(({const char*_tmp19C=">";_tag_fat(_tmp19C,sizeof(char),2U);}));_tmp19A[2]=_tmp5F5;});Cyc_PP_cat(_tag_fat(_tmp19A,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: _LL31: _tmp1B4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL32:
# 848
 s=({struct Cyc_PP_Doc*_tmp19D[3U];({struct Cyc_PP_Doc*_tmp5FA=Cyc_PP_text(({const char*_tmp19E="regions(";_tag_fat(_tmp19E,sizeof(char),9U);}));_tmp19D[0]=_tmp5FA;}),({struct Cyc_PP_Doc*_tmp5F9=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(_tmp1B4))->hd);_tmp19D[1]=_tmp5F9;}),({struct Cyc_PP_Doc*_tmp5F8=Cyc_PP_text(({const char*_tmp19F=")";_tag_fat(_tmp19F,sizeof(char),2U);}));_tmp19D[2]=_tmp5F8;});Cyc_PP_cat(_tag_fat(_tmp19D,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 8U: _LL33: _LL34:
 goto _LL36;case 9U: _LL35: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 854
 s=Cyc_PP_text(({const char*_tmp1A0="@true";_tag_fat(_tmp1A0,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
 s=Cyc_PP_text(({const char*_tmp1A1="@false";_tag_fat(_tmp1A1,sizeof(char),7U);}));goto _LL0;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2 != 0){_LL41: _tmp1B5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2)->hd;_LL42:
# 857
 s=({struct Cyc_PP_Doc*_tmp1A2[4U];({struct Cyc_PP_Doc*_tmp5FE=Cyc_PP_text(({const char*_tmp1A3="@thin @numelts";_tag_fat(_tmp1A3,sizeof(char),15U);}));_tmp1A2[0]=_tmp5FE;}),({struct Cyc_PP_Doc*_tmp5FD=Cyc_Absynpp_lb();_tmp1A2[1]=_tmp5FD;}),({struct Cyc_PP_Doc*_tmp5FC=Cyc_Absynpp_typ2doc(_tmp1B5);_tmp1A2[2]=_tmp5FC;}),({struct Cyc_PP_Doc*_tmp5FB=Cyc_Absynpp_rb();_tmp1A2[3]=_tmp5FB;});Cyc_PP_cat(_tag_fat(_tmp1A2,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}else{_LL43: _LL44:
# 859
 s=Cyc_PP_text(({const char*_tmp1A4="@thin";_tag_fat(_tmp1A4,sizeof(char),6U);}));goto _LL0;}default: _LL45: _LL46:
 s=Cyc_PP_text(({const char*_tmp1A5="@fat";_tag_fat(_tmp1A5,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp1B9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f1;_tmp1B8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f2;_tmp1B7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f3;_tmp1B6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f4;_LLA:
# 745
 if(_tmp1B8 != 0)
# 747
return Cyc_Absynpp_ntyp2doc(_tmp1B8);else{
# 749
struct _fat_ptr kindstring=_tmp1B9 == 0?({const char*_tmp155="K";_tag_fat(_tmp155,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp1B9->v);
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp567;_tmp567.tag=0U,_tmp567.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_tmp567;});struct Cyc_Int_pa_PrintArg_struct _tmp154=({struct Cyc_Int_pa_PrintArg_struct _tmp566;_tmp566.tag=1U,_tmp566.f1=(unsigned long)_tmp1B7;_tmp566;});void*_tmp151[2U];_tmp151[0]=& _tmp153,_tmp151[1]=& _tmp154;({struct _fat_ptr _tmp5FF=({const char*_tmp152="`E%s%d";_tag_fat(_tmp152,sizeof(char),7U);});Cyc_aprintf(_tmp5FF,_tag_fat(_tmp151,sizeof(void*),2U));});}));}
# 752
goto _LL0;case 2U: _LLB: _tmp1BA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->f1;_LLC:
# 754
 s=Cyc_Absynpp_tvar2doc(_tmp1BA);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp156[3U];_tmp156[0]=s,({struct Cyc_PP_Doc*_tmp601=Cyc_PP_text(({const char*_tmp157="::";_tag_fat(_tmp157,sizeof(char),3U);}));_tmp156[1]=_tmp601;}),({struct Cyc_PP_Doc*_tmp600=Cyc_Absynpp_kindbound2doc(_tmp1BA->kind);_tmp156[2]=_tmp600;});Cyc_PP_cat(_tag_fat(_tmp156,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 6U: _LL15: _tmp1BB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14E)->f1;_LL16:
# 816
 s=({struct Cyc_PP_Doc*_tmp185[2U];({struct Cyc_PP_Doc*_tmp603=Cyc_Absynpp_dollar();_tmp185[0]=_tmp603;}),({struct Cyc_PP_Doc*_tmp602=Cyc_Absynpp_args2doc(_tmp1BB);_tmp185[1]=_tmp602;});Cyc_PP_cat(_tag_fat(_tmp185,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 7U: _LL19: _tmp1BD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f1;_tmp1BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f2;_LL1A:
# 822
 s=({struct Cyc_PP_Doc*_tmp18B[4U];({struct Cyc_PP_Doc*_tmp607=Cyc_Absynpp_aggr_kind2doc(_tmp1BD);_tmp18B[0]=_tmp607;}),({struct Cyc_PP_Doc*_tmp606=Cyc_Absynpp_lb();_tmp18B[1]=_tmp606;}),({
struct Cyc_PP_Doc*_tmp605=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp1BC));_tmp18B[2]=_tmp605;}),({
struct Cyc_PP_Doc*_tmp604=Cyc_Absynpp_rb();_tmp18B[3]=_tmp604;});Cyc_PP_cat(_tag_fat(_tmp18B,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 9U: _LL1F: _tmp1BE=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp14E)->f1;_LL20:
# 829
 s=({struct Cyc_PP_Doc*_tmp190[3U];({struct Cyc_PP_Doc*_tmp60A=Cyc_PP_text(({const char*_tmp191="valueof_t(";_tag_fat(_tmp191,sizeof(char),11U);}));_tmp190[0]=_tmp60A;}),({struct Cyc_PP_Doc*_tmp609=Cyc_Absynpp_exp2doc(_tmp1BE);_tmp190[1]=_tmp609;}),({struct Cyc_PP_Doc*_tmp608=Cyc_PP_text(({const char*_tmp192=")";_tag_fat(_tmp192,sizeof(char),2U);}));_tmp190[2]=_tmp608;});Cyc_PP_cat(_tag_fat(_tmp190,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 11U: _LL21: _tmp1BF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp14E)->f1;_LL22:
 s=({struct Cyc_PP_Doc*_tmp193[3U];({struct Cyc_PP_Doc*_tmp60D=Cyc_PP_text(({const char*_tmp194="typeof(";_tag_fat(_tmp194,sizeof(char),8U);}));_tmp193[0]=_tmp60D;}),({struct Cyc_PP_Doc*_tmp60C=Cyc_Absynpp_exp2doc(_tmp1BF);_tmp193[1]=_tmp60C;}),({struct Cyc_PP_Doc*_tmp60B=Cyc_PP_text(({const char*_tmp195=")";_tag_fat(_tmp195,sizeof(char),2U);}));_tmp193[2]=_tmp60B;});Cyc_PP_cat(_tag_fat(_tmp193,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 8U: _LL25: _tmp1C2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f1;_tmp1C1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f2;_tmp1C0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f3;_LL26:
# 838
 s=({struct Cyc_PP_Doc*_tmp196[2U];({struct Cyc_PP_Doc*_tmp60F=Cyc_Absynpp_qvar2doc(_tmp1C2);_tmp196[0]=_tmp60F;}),({struct Cyc_PP_Doc*_tmp60E=Cyc_Absynpp_tps2doc(_tmp1C1);_tmp196[1]=_tmp60E;});Cyc_PP_cat(_tag_fat(_tmp196,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)){case 0U: _LL37: _tmp1C3=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL38:
# 851
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1C3);goto _LL0;case 1U: _LL39: _tmp1C4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL3A:
 s=Cyc_Absynpp_enumdecl2doc(_tmp1C4);goto _LL0;default: _LL3B: _tmp1C5=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL3C:
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1C5);goto _LL0;}}_LL0:;}
# 862
return s;}
# 865
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _fat_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple18{void*f1;void*f2;};
# 869
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple18*cmp){
struct _tuple18*_tmp1C6=cmp;void*_tmp1CA;void*_tmp1C9;_LL1: _tmp1CA=_tmp1C6->f1;_tmp1C9=_tmp1C6->f2;_LL2:;
return({struct Cyc_PP_Doc*_tmp1C7[3U];({struct Cyc_PP_Doc*_tmp612=Cyc_Absynpp_rgn2doc(_tmp1CA);_tmp1C7[0]=_tmp612;}),({struct Cyc_PP_Doc*_tmp611=Cyc_PP_text(({const char*_tmp1C8=" > ";_tag_fat(_tmp1C8,sizeof(char),4U);}));_tmp1C7[1]=_tmp611;}),({struct Cyc_PP_Doc*_tmp610=Cyc_Absynpp_rgn2doc(_tmp1C9);_tmp1C7[2]=_tmp610;});Cyc_PP_cat(_tag_fat(_tmp1C7,sizeof(struct Cyc_PP_Doc*),3U));});}
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _tmp615=({const char*_tmp1CB="";_tag_fat(_tmp1CB,sizeof(char),1U);});struct _fat_ptr _tmp614=({const char*_tmp1CC="";_tag_fat(_tmp1CC,sizeof(char),1U);});struct _fat_ptr _tmp613=({const char*_tmp1CD=",";_tag_fat(_tmp1CD,sizeof(char),2U);});Cyc_PP_group(_tmp615,_tmp614,_tmp613,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 878
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*_tmp1CE=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1CE == 0?0:({struct Cyc_Core_Opt*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({struct Cyc_PP_Doc*_tmp616=Cyc_PP_text(*_tmp1CE);_tmp1CF->v=_tmp616;});_tmp1CF;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 884
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 888
struct Cyc_List_List*_tmp1D0=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
_tmp1D0=({struct Cyc_List_List*_tmp618=_tmp1D0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp618,({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));({struct Cyc_PP_Doc*_tmp617=Cyc_PP_text(({const char*_tmp1D1="...";_tag_fat(_tmp1D1,sizeof(char),4U);}));_tmp1D2->hd=_tmp617;}),_tmp1D2->tl=0;_tmp1D2;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1D3=({struct Cyc_PP_Doc*_tmp1D5[3U];({struct Cyc_PP_Doc*_tmp61C=Cyc_PP_text(({const char*_tmp1D6="...";_tag_fat(_tmp1D6,sizeof(char),4U);}));_tmp1D5[0]=_tmp61C;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp61B=Cyc_PP_text(({const char*_tmp1D7=" inject ";_tag_fat(_tmp1D7,sizeof(char),9U);}));_tmp1D5[1]=_tmp61B;}):({struct Cyc_PP_Doc*_tmp61A=Cyc_PP_text(({const char*_tmp1D8=" ";_tag_fat(_tmp1D8,sizeof(char),2U);}));_tmp1D5[1]=_tmp61A;}),({
struct Cyc_PP_Doc*_tmp619=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->f1=cyc_varargs->name,_tmp1D9->f2=cyc_varargs->tq,_tmp1D9->f3=cyc_varargs->type;_tmp1D9;}));_tmp1D5[2]=_tmp619;});Cyc_PP_cat(_tag_fat(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3U));});
# 897
_tmp1D0=({struct Cyc_List_List*_tmp61D=_tmp1D0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp61D,({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=_tmp1D3,_tmp1D4->tl=0;_tmp1D4;}));});}}{
# 899
struct Cyc_PP_Doc*_tmp1DA=({struct _fat_ptr _tmp620=({const char*_tmp1E9="";_tag_fat(_tmp1E9,sizeof(char),1U);});struct _fat_ptr _tmp61F=({const char*_tmp1EA="";_tag_fat(_tmp1EA,sizeof(char),1U);});struct _fat_ptr _tmp61E=({const char*_tmp1EB=",";_tag_fat(_tmp1EB,sizeof(char),2U);});Cyc_PP_group(_tmp620,_tmp61F,_tmp61E,_tmp1D0);});
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
_tmp1DA=({struct Cyc_PP_Doc*_tmp1DB[3U];_tmp1DB[0]=_tmp1DA,({struct Cyc_PP_Doc*_tmp622=Cyc_PP_text(({const char*_tmp1DC=";";_tag_fat(_tmp1DC,sizeof(char),2U);}));_tmp1DB[1]=_tmp622;}),({struct Cyc_PP_Doc*_tmp621=Cyc_Absynpp_eff2doc(effopt);_tmp1DB[2]=_tmp621;});Cyc_PP_cat(_tag_fat(_tmp1DB,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
_tmp1DA=({struct Cyc_PP_Doc*_tmp1DD[3U];_tmp1DD[0]=_tmp1DA,({struct Cyc_PP_Doc*_tmp624=Cyc_PP_text(({const char*_tmp1DE=":";_tag_fat(_tmp1DE,sizeof(char),2U);}));_tmp1DD[1]=_tmp624;}),({struct Cyc_PP_Doc*_tmp623=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1DD[2]=_tmp623;});Cyc_PP_cat(_tag_fat(_tmp1DD,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp1DF=({struct Cyc_PP_Doc*_tmp1E6[3U];({struct Cyc_PP_Doc*_tmp626=Cyc_PP_text(({const char*_tmp1E7="(";_tag_fat(_tmp1E7,sizeof(char),2U);}));_tmp1E6[0]=_tmp626;}),_tmp1E6[1]=_tmp1DA,({struct Cyc_PP_Doc*_tmp625=Cyc_PP_text(({const char*_tmp1E8=")";_tag_fat(_tmp1E8,sizeof(char),2U);}));_tmp1E6[2]=_tmp625;});Cyc_PP_cat(_tag_fat(_tmp1E6,sizeof(struct Cyc_PP_Doc*),3U));});
if(req != 0)
_tmp1DF=({struct Cyc_PP_Doc*_tmp1E0[4U];_tmp1E0[0]=_tmp1DF,({struct Cyc_PP_Doc*_tmp629=Cyc_PP_text(({const char*_tmp1E1=" @requires(";_tag_fat(_tmp1E1,sizeof(char),12U);}));_tmp1E0[1]=_tmp629;}),({struct Cyc_PP_Doc*_tmp628=Cyc_Absynpp_exp2doc(req);_tmp1E0[2]=_tmp628;}),({struct Cyc_PP_Doc*_tmp627=Cyc_PP_text(({const char*_tmp1E2=")";_tag_fat(_tmp1E2,sizeof(char),2U);}));_tmp1E0[3]=_tmp627;});Cyc_PP_cat(_tag_fat(_tmp1E0,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
_tmp1DF=({struct Cyc_PP_Doc*_tmp1E3[4U];_tmp1E3[0]=_tmp1DF,({struct Cyc_PP_Doc*_tmp62C=Cyc_PP_text(({const char*_tmp1E4=" @ensures(";_tag_fat(_tmp1E4,sizeof(char),11U);}));_tmp1E3[1]=_tmp62C;}),({struct Cyc_PP_Doc*_tmp62B=Cyc_Absynpp_exp2doc(ens);_tmp1E3[2]=_tmp62B;}),({struct Cyc_PP_Doc*_tmp62A=Cyc_PP_text(({const char*_tmp1E5=")";_tag_fat(_tmp1E5,sizeof(char),2U);}));_tmp1E3[3]=_tmp62A;});Cyc_PP_cat(_tag_fat(_tmp1E3,sizeof(struct Cyc_PP_Doc*),4U));});
return _tmp1DF;};};}
# 912
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _fat_ptr*v){return Cyc_PP_text(*v);}
# 914
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1EC=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1ED=(*q).f1;union Cyc_Absyn_Nmspace _tmp1EE=_tmp1ED;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F1;struct Cyc_List_List*_tmp1F0;switch((_tmp1EE.C_n).tag){case 4U: _LL1: _LL2:
 _tmp1F0=0;goto _LL4;case 1U: _LL3: _tmp1F0=(_tmp1EE.Rel_n).val;_LL4:
# 920
 match=0;
_tmp1EC=_tmp1F0;
goto _LL0;case 3U: _LL5: _tmp1F1=(_tmp1EE.C_n).val;_LL6:
# 924
 match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1F1,Cyc_Absynpp_curr_namespace);
# 926
goto _LL0;default: _LL7: _tmp1F2=(_tmp1EE.Abs_n).val;_LL8:
# 928
 match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1F2,Cyc_Absynpp_curr_namespace);
_tmp1EC=Cyc_Absynpp_qvar_to_Cids && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->hd=Cyc_Absynpp_cyc_stringptr,_tmp1EF->tl=_tmp1F2;_tmp1EF;}): _tmp1F2;
goto _LL0;}_LL0:;}
# 932
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _fat_ptr)({struct Cyc_List_List*_tmp62E=({struct Cyc_List_List*_tmp62D=_tmp1EC;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp62D,({struct Cyc_List_List*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3->hd=(*q).f2,_tmp1F3->tl=0;_tmp1F3;}));});
# 933
Cyc_str_sepstr(_tmp62E,({const char*_tmp1F4="_";_tag_fat(_tmp1F4,sizeof(char),2U);}));});else{
# 937
if(match)
return*(*q).f2;else{
# 940
return(struct _fat_ptr)({struct Cyc_List_List*_tmp630=({struct Cyc_List_List*_tmp62F=_tmp1EC;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp62F,({struct Cyc_List_List*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=(*q).f2,_tmp1F5->tl=0;_tmp1F5;}));});Cyc_str_sepstr(_tmp630,({const char*_tmp1F6="::";_tag_fat(_tmp1F6,sizeof(char),3U);}));});}}}
# 944
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 948
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr _tmp1F7=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 952
return({struct _fat_ptr _tmp633=(struct _fat_ptr)({struct _fat_ptr _tmp632=(struct _fat_ptr)({struct _fat_ptr _tmp631=({const char*_tmp1F8="\\textbf{";_tag_fat(_tmp1F8,sizeof(char),9U);});Cyc_strconcat(_tmp631,(struct _fat_ptr)_tmp1F7);});Cyc_strconcat(_tmp632,({const char*_tmp1F9="}";_tag_fat(_tmp1F9,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp633,(int)
Cyc_strlen((struct _fat_ptr)_tmp1F7));});else{
return Cyc_PP_text(_tmp1F7);}}
# 957
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 959
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 962
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1FA=(*v).f1;union Cyc_Absyn_Nmspace _tmp1FB=_tmp1FA;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp1FD;switch((_tmp1FB.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp1FB.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 973
 return(struct _fat_ptr)({struct _fat_ptr _tmp634=({const char*_tmp1FC="/* bad namespace : */ ";_tag_fat(_tmp1FC,sizeof(char),23U);});Cyc_strconcat(_tmp634,(struct _fat_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1FD=(_tmp1FB.C_n).val;_LL6:
# 966
 _tmp1FE=_tmp1FD;goto _LL8;default: _LL7: _tmp1FE=(_tmp1FB.Abs_n).val;_LL8:
# 968
 if(((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1FE,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 971
goto _LLA;}}_LL0:;}else{
# 976
return*(*v).f2;}}
# 979
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 982
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _fat_ptr _tmp1FF=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
# 986
return({struct _fat_ptr _tmp637=(struct _fat_ptr)({struct _fat_ptr _tmp636=(struct _fat_ptr)({struct _fat_ptr _tmp635=({const char*_tmp200="\\textbf{";_tag_fat(_tmp200,sizeof(char),9U);});Cyc_strconcat(_tmp635,(struct _fat_ptr)_tmp1FF);});Cyc_strconcat(_tmp636,({const char*_tmp201="}";_tag_fat(_tmp201,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp637,(int)
Cyc_strlen((struct _fat_ptr)_tmp1FF));});else{
return Cyc_PP_text(_tmp1FF);}}
# 991
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp638=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp638,t,0);});}
# 995
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_tmp202=f;unsigned _tmp207;struct _fat_ptr*_tmp206;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->tag == 0U){_LL1: _tmp206=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->f1;_LL2:
 return Cyc_PP_textptr(_tmp206);}else{_LL3: _tmp207=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp202)->f1;_LL4:
 return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp205=({struct Cyc_Int_pa_PrintArg_struct _tmp568;_tmp568.tag=1U,_tmp568.f1=(unsigned long)((int)_tmp207);_tmp568;});void*_tmp203[1U];_tmp203[0]=& _tmp205;({struct _fat_ptr _tmp639=({const char*_tmp204="%d";_tag_fat(_tmp204,sizeof(char),3U);});Cyc_aprintf(_tmp639,_tag_fat(_tmp203,sizeof(void*),1U));});}));}_LL0:;}
# 1009 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp208=e->r;void*_tmp209=_tmp208;struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_Absyn_Exp*_tmp20C;enum Cyc_Absyn_Primop _tmp20B;switch(*((int*)_tmp209)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp20B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL6: {
# 1014
enum Cyc_Absyn_Primop _tmp20A=_tmp20B;switch(_tmp20A){case Cyc_Absyn_Plus: _LL56: _LL57:
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
# 1036
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return - 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp20C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL1A:
 return Cyc_Absynpp_exp_prec(_tmp20C);case 13U: _LL1B: _tmp20D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL1C:
 return Cyc_Absynpp_exp_prec(_tmp20D);case 14U: _LL1D: _LL1E:
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
# 1078
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct _tuple19{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1082
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
struct _tuple19*_tmp20E=(struct _tuple19*)o->hd;struct _tuple19*_tmp20F=_tmp20E;struct _fat_ptr _tmp217;struct Cyc_Absyn_Exp*_tmp216;_LL1: _tmp217=_tmp20F->f1;_tmp216=_tmp20F->f2;_LL2:;
s=({struct Cyc_PP_Doc*_tmp210[5U];({struct Cyc_PP_Doc*_tmp63E=Cyc_PP_text(({const char*_tmp211="\"";_tag_fat(_tmp211,sizeof(char),2U);}));_tmp210[0]=_tmp63E;}),({struct Cyc_PP_Doc*_tmp63D=Cyc_PP_text(_tmp217);_tmp210[1]=_tmp63D;}),({struct Cyc_PP_Doc*_tmp63C=Cyc_PP_text(({const char*_tmp212="\" (";_tag_fat(_tmp212,sizeof(char),4U);}));_tmp210[2]=_tmp63C;}),({struct Cyc_PP_Doc*_tmp63B=Cyc_Absynpp_exp2doc(_tmp216);_tmp210[3]=_tmp63B;}),({struct Cyc_PP_Doc*_tmp63A=Cyc_PP_text(({const char*_tmp213=")";_tag_fat(_tmp213,sizeof(char),2U);}));_tmp210[4]=_tmp63A;});Cyc_PP_cat(_tag_fat(_tmp210,sizeof(struct Cyc_PP_Doc*),5U));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp214[2U];_tmp214[0]=s,({struct Cyc_PP_Doc*_tmp63F=Cyc_PP_text(({const char*_tmp215=",";_tag_fat(_tmp215,sizeof(char),2U);}));_tmp214[1]=_tmp63F;});Cyc_PP_cat(_tag_fat(_tmp214,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1092
return s;}
# 1095
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp218[2U];({struct Cyc_PP_Doc*_tmp641=Cyc_PP_text(({const char*_tmp219=": ";_tag_fat(_tmp219,sizeof(char),3U);}));_tmp218[0]=_tmp641;}),({struct Cyc_PP_Doc*_tmp640=Cyc_Absynpp_asm_iolist_doc_in(o);_tmp218[1]=_tmp640;});Cyc_PP_cat(_tag_fat(_tmp218,sizeof(struct Cyc_PP_Doc*),2U));});
# 1100
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_tmp21A[3U];_tmp21A[0]=s,({struct Cyc_PP_Doc*_tmp643=Cyc_PP_text(({const char*_tmp21B=": : ";_tag_fat(_tmp21B,sizeof(char),5U);}));_tmp21A[1]=_tmp643;}),({struct Cyc_PP_Doc*_tmp642=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp21A[2]=_tmp642;});Cyc_PP_cat(_tag_fat(_tmp21A,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1104
s=({struct Cyc_PP_Doc*_tmp21C[3U];_tmp21C[0]=s,({struct Cyc_PP_Doc*_tmp645=Cyc_PP_text(({const char*_tmp21D=" : ";_tag_fat(_tmp21D,sizeof(char),4U);}));_tmp21C[1]=_tmp645;}),({struct Cyc_PP_Doc*_tmp644=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp21C[2]=_tmp644;});Cyc_PP_cat(_tag_fat(_tmp21C,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1106
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_tmp21E[2U];_tmp21E[0]=s,ncol == 0?({struct Cyc_PP_Doc*_tmp648=Cyc_PP_text(({const char*_tmp21F=": : :";_tag_fat(_tmp21F,sizeof(char),6U);}));_tmp21E[1]=_tmp648;}):(ncol == 1?({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp220=" : : ";_tag_fat(_tmp220,sizeof(char),6U);}));_tmp21E[1]=_tmp647;}):({struct Cyc_PP_Doc*_tmp646=Cyc_PP_text(({const char*_tmp221=" : ";_tag_fat(_tmp221,sizeof(char),4U);}));_tmp21E[1]=_tmp646;}));Cyc_PP_cat(_tag_fat(_tmp21E,sizeof(struct Cyc_PP_Doc*),2U));});
while(cl != 0){
s=({struct Cyc_PP_Doc*_tmp222[4U];_tmp222[0]=s,({struct Cyc_PP_Doc*_tmp64B=Cyc_PP_text(({const char*_tmp223="\"";_tag_fat(_tmp223,sizeof(char),2U);}));_tmp222[1]=_tmp64B;}),({struct Cyc_PP_Doc*_tmp64A=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_tmp222[2]=_tmp64A;}),({struct Cyc_PP_Doc*_tmp649=Cyc_PP_text(({const char*_tmp224="\"";_tag_fat(_tmp224,sizeof(char),2U);}));_tmp222[3]=_tmp649;});Cyc_PP_cat(_tag_fat(_tmp222,sizeof(struct Cyc_PP_Doc*),4U));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_tmp225[2U];_tmp225[0]=s,({struct Cyc_PP_Doc*_tmp64C=Cyc_PP_text(({const char*_tmp226=", ";_tag_fat(_tmp226,sizeof(char),3U);}));_tmp225[1]=_tmp64C;});Cyc_PP_cat(_tag_fat(_tmp225,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1116
return s;}
# 1119
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp227=e->r;void*_tmp228=_tmp227;struct Cyc_Absyn_Stmt*_tmp2F9;struct Cyc_Core_Opt*_tmp2F8;struct Cyc_List_List*_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F5;int _tmp2F4;struct Cyc_Absyn_Exp*_tmp2F3;void**_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F1;int _tmp2F0;struct Cyc_Absyn_Enumfield*_tmp2EF;struct Cyc_Absyn_Enumfield*_tmp2EE;struct Cyc_List_List*_tmp2ED;struct Cyc_Absyn_Datatypefield*_tmp2EC;struct Cyc_List_List*_tmp2EB;struct _tuple0*_tmp2EA;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List*_tmp2E8;struct Cyc_Absyn_Exp*_tmp2E7;void*_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E3;struct Cyc_List_List*_tmp2E2;struct _tuple8*_tmp2E1;struct Cyc_List_List*_tmp2E0;struct Cyc_List_List*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DC;struct _fat_ptr*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DA;struct _fat_ptr*_tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;void*_tmp2D7;struct Cyc_List_List*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;struct _fat_ptr*_tmp2D4;int _tmp2D3;struct _fat_ptr _tmp2D2;struct Cyc_List_List*_tmp2D1;struct Cyc_List_List*_tmp2D0;struct Cyc_List_List*_tmp2CF;void*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;void*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;void*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_List_List*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B7;enum Cyc_Absyn_Incrementor _tmp2B6;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Core_Opt*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;enum Cyc_Absyn_Primop _tmp2B2;struct Cyc_List_List*_tmp2B1;struct _fat_ptr _tmp2B0;void*_tmp2AF;union Cyc_Absyn_Cnst _tmp2AE;switch(*((int*)_tmp228)){case 0U: _LL1: _tmp2AE=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL2:
 s=Cyc_Absynpp_cnst2doc(_tmp2AE);goto _LL0;case 1U: _LL3: _tmp2AF=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL4:
 s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(_tmp2AF));goto _LL0;case 2U: _LL5: _tmp2B0=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL6:
# 1126
 s=({struct Cyc_PP_Doc*_tmp229[4U];({struct Cyc_PP_Doc*_tmp650=Cyc_PP_text(({const char*_tmp22A="__cyclone_pragma__";_tag_fat(_tmp22A,sizeof(char),19U);}));_tmp229[0]=_tmp650;}),({struct Cyc_PP_Doc*_tmp64F=Cyc_PP_text(({const char*_tmp22B="(";_tag_fat(_tmp22B,sizeof(char),2U);}));_tmp229[1]=_tmp64F;}),({struct Cyc_PP_Doc*_tmp64E=Cyc_PP_text(_tmp2B0);_tmp229[2]=_tmp64E;}),({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_text(({const char*_tmp22C=")";_tag_fat(_tmp22C,sizeof(char),2U);}));_tmp229[3]=_tmp64D;});Cyc_PP_cat(_tag_fat(_tmp229,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 3U: _LL7: _tmp2B2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2B1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL8:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp2B2,_tmp2B1);goto _LL0;case 4U: _LL9: _tmp2B5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2B4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp2B3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LLA:
# 1129
 s=({struct Cyc_PP_Doc*_tmp22D[5U];({struct Cyc_PP_Doc*_tmp656=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B5);_tmp22D[0]=_tmp656;}),({
struct Cyc_PP_Doc*_tmp655=Cyc_PP_text(({const char*_tmp22E=" ";_tag_fat(_tmp22E,sizeof(char),2U);}));_tmp22D[1]=_tmp655;}),
_tmp2B4 == 0?({struct Cyc_PP_Doc*_tmp654=Cyc_PP_nil_doc();_tmp22D[2]=_tmp654;}):({struct Cyc_PP_Doc*_tmp653=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp2B4->v);_tmp22D[2]=_tmp653;}),({
struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp22F="= ";_tag_fat(_tmp22F,sizeof(char),3U);}));_tmp22D[3]=_tmp652;}),({
struct Cyc_PP_Doc*_tmp651=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B3);_tmp22D[4]=_tmp651;});Cyc_PP_cat(_tag_fat(_tmp22D,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 5U: _LLB: _tmp2B7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2B6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LLC: {
# 1136
struct Cyc_PP_Doc*_tmp230=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B7);
{enum Cyc_Absyn_Incrementor _tmp231=_tmp2B6;switch(_tmp231){case Cyc_Absyn_PreInc: _LL56: _LL57:
 s=({struct Cyc_PP_Doc*_tmp232[2U];({struct Cyc_PP_Doc*_tmp657=Cyc_PP_text(({const char*_tmp233="++";_tag_fat(_tmp233,sizeof(char),3U);}));_tmp232[0]=_tmp657;}),_tmp232[1]=_tmp230;Cyc_PP_cat(_tag_fat(_tmp232,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PreDec: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp234[2U];({struct Cyc_PP_Doc*_tmp658=Cyc_PP_text(({const char*_tmp235="--";_tag_fat(_tmp235,sizeof(char),3U);}));_tmp234[0]=_tmp658;}),_tmp234[1]=_tmp230;Cyc_PP_cat(_tag_fat(_tmp234,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostInc: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp236[2U];_tmp236[0]=_tmp230,({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp237="++";_tag_fat(_tmp237,sizeof(char),3U);}));_tmp236[1]=_tmp659;});Cyc_PP_cat(_tag_fat(_tmp236,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostDec: _LL5C: _LL5D:
 goto _LL5F;default: _LL5E: _LL5F:
 s=({struct Cyc_PP_Doc*_tmp238[2U];_tmp238[0]=_tmp230,({struct Cyc_PP_Doc*_tmp65A=Cyc_PP_text(({const char*_tmp239="--";_tag_fat(_tmp239,sizeof(char),3U);}));_tmp238[1]=_tmp65A;});Cyc_PP_cat(_tag_fat(_tmp238,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;}_LL55:;}
# 1144
goto _LL0;}case 6U: _LLD: _tmp2BA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2B9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp2B8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LLE:
# 1146
 s=({struct Cyc_PP_Doc*_tmp23A[5U];({struct Cyc_PP_Doc*_tmp65F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BA);_tmp23A[0]=_tmp65F;}),({struct Cyc_PP_Doc*_tmp65E=Cyc_PP_text(({const char*_tmp23B=" ? ";_tag_fat(_tmp23B,sizeof(char),4U);}));_tmp23A[1]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_exp2doc_prec(0,_tmp2B9);_tmp23A[2]=_tmp65D;}),({
struct Cyc_PP_Doc*_tmp65C=Cyc_PP_text(({const char*_tmp23C=" : ";_tag_fat(_tmp23C,sizeof(char),4U);}));_tmp23A[3]=_tmp65C;}),({struct Cyc_PP_Doc*_tmp65B=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B8);_tmp23A[4]=_tmp65B;});Cyc_PP_cat(_tag_fat(_tmp23A,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 7U: _LLF: _tmp2BC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2BB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL10:
# 1150
 s=({struct Cyc_PP_Doc*_tmp23D[3U];({struct Cyc_PP_Doc*_tmp662=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BC);_tmp23D[0]=_tmp662;}),({struct Cyc_PP_Doc*_tmp661=Cyc_PP_text(({const char*_tmp23E=" && ";_tag_fat(_tmp23E,sizeof(char),5U);}));_tmp23D[1]=_tmp661;}),({struct Cyc_PP_Doc*_tmp660=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BB);_tmp23D[2]=_tmp660;});Cyc_PP_cat(_tag_fat(_tmp23D,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 8U: _LL11: _tmp2BE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2BD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL12:
# 1153
 s=({struct Cyc_PP_Doc*_tmp23F[3U];({struct Cyc_PP_Doc*_tmp665=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BE);_tmp23F[0]=_tmp665;}),({struct Cyc_PP_Doc*_tmp664=Cyc_PP_text(({const char*_tmp240=" || ";_tag_fat(_tmp240,sizeof(char),5U);}));_tmp23F[1]=_tmp664;}),({struct Cyc_PP_Doc*_tmp663=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BD);_tmp23F[2]=_tmp663;});Cyc_PP_cat(_tag_fat(_tmp23F,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 9U: _LL13: _tmp2C0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2BF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL14:
# 1158
 s=({struct Cyc_PP_Doc*_tmp241[3U];({struct Cyc_PP_Doc*_tmp668=Cyc_Absynpp_exp2doc_prec(myprec - 1,_tmp2C0);_tmp241[0]=_tmp668;}),({struct Cyc_PP_Doc*_tmp667=Cyc_PP_text(({const char*_tmp242=", ";_tag_fat(_tmp242,sizeof(char),3U);}));_tmp241[1]=_tmp667;}),({struct Cyc_PP_Doc*_tmp666=Cyc_Absynpp_exp2doc_prec(myprec - 1,_tmp2BF);_tmp241[2]=_tmp666;});Cyc_PP_cat(_tag_fat(_tmp241,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 10U: _LL15: _tmp2C2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2C1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL16:
# 1161
 s=({struct Cyc_PP_Doc*_tmp243[4U];({struct Cyc_PP_Doc*_tmp66C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C2);_tmp243[0]=_tmp66C;}),({
struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp244="(";_tag_fat(_tmp244,sizeof(char),2U);}));_tmp243[1]=_tmp66B;}),({
struct Cyc_PP_Doc*_tmp66A=Cyc_Absynpp_exps2doc_prec(20,_tmp2C1);_tmp243[2]=_tmp66A;}),({
struct Cyc_PP_Doc*_tmp669=Cyc_PP_text(({const char*_tmp245=")";_tag_fat(_tmp245,sizeof(char),2U);}));_tmp243[3]=_tmp669;});Cyc_PP_cat(_tag_fat(_tmp243,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 11U: _LL17: _tmp2C3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL18:
 s=({struct Cyc_PP_Doc*_tmp246[2U];({struct Cyc_PP_Doc*_tmp66E=Cyc_PP_text(({const char*_tmp247="throw ";_tag_fat(_tmp247,sizeof(char),7U);}));_tmp246[0]=_tmp66E;}),({struct Cyc_PP_Doc*_tmp66D=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C3);_tmp246[1]=_tmp66D;});Cyc_PP_cat(_tag_fat(_tmp246,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 12U: _LL19: _tmp2C4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL1A:
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2C4);goto _LL0;case 13U: _LL1B: _tmp2C5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL1C:
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2C5);goto _LL0;case 14U: _LL1D: _tmp2C7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2C6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL1E:
# 1170
 s=({struct Cyc_PP_Doc*_tmp248[4U];({struct Cyc_PP_Doc*_tmp672=Cyc_PP_text(({const char*_tmp249="(";_tag_fat(_tmp249,sizeof(char),2U);}));_tmp248[0]=_tmp672;}),({struct Cyc_PP_Doc*_tmp671=Cyc_Absynpp_typ2doc(_tmp2C7);_tmp248[1]=_tmp671;}),({struct Cyc_PP_Doc*_tmp670=Cyc_PP_text(({const char*_tmp24A=")";_tag_fat(_tmp24A,sizeof(char),2U);}));_tmp248[2]=_tmp670;}),({struct Cyc_PP_Doc*_tmp66F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C6);_tmp248[3]=_tmp66F;});Cyc_PP_cat(_tag_fat(_tmp248,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 15U: _LL1F: _tmp2C8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL20:
 s=({struct Cyc_PP_Doc*_tmp24B[2U];({struct Cyc_PP_Doc*_tmp674=Cyc_PP_text(({const char*_tmp24C="&";_tag_fat(_tmp24C,sizeof(char),2U);}));_tmp24B[0]=_tmp674;}),({struct Cyc_PP_Doc*_tmp673=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C8);_tmp24B[1]=_tmp673;});Cyc_PP_cat(_tag_fat(_tmp24B,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 16U: _LL21: _tmp2CA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2C9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL22:
# 1173
 if(_tmp2CA == 0)
s=({struct Cyc_PP_Doc*_tmp24D[2U];({struct Cyc_PP_Doc*_tmp676=Cyc_PP_text(({const char*_tmp24E="new ";_tag_fat(_tmp24E,sizeof(char),5U);}));_tmp24D[0]=_tmp676;}),({struct Cyc_PP_Doc*_tmp675=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C9);_tmp24D[1]=_tmp675;});Cyc_PP_cat(_tag_fat(_tmp24D,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1176
s=({struct Cyc_PP_Doc*_tmp24F[4U];({struct Cyc_PP_Doc*_tmp67A=Cyc_PP_text(({const char*_tmp250="rnew(";_tag_fat(_tmp250,sizeof(char),6U);}));_tmp24F[0]=_tmp67A;}),({struct Cyc_PP_Doc*_tmp679=Cyc_Absynpp_exp2doc(_tmp2CA);_tmp24F[1]=_tmp679;}),({struct Cyc_PP_Doc*_tmp678=Cyc_PP_text(({const char*_tmp251=") ";_tag_fat(_tmp251,sizeof(char),3U);}));_tmp24F[2]=_tmp678;}),({struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C9);_tmp24F[3]=_tmp677;});Cyc_PP_cat(_tag_fat(_tmp24F,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;case 17U: _LL23: _tmp2CB=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL24:
 s=({struct Cyc_PP_Doc*_tmp252[3U];({struct Cyc_PP_Doc*_tmp67D=Cyc_PP_text(({const char*_tmp253="sizeof(";_tag_fat(_tmp253,sizeof(char),8U);}));_tmp252[0]=_tmp67D;}),({struct Cyc_PP_Doc*_tmp67C=Cyc_Absynpp_typ2doc(_tmp2CB);_tmp252[1]=_tmp67C;}),({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(({const char*_tmp254=")";_tag_fat(_tmp254,sizeof(char),2U);}));_tmp252[2]=_tmp67B;});Cyc_PP_cat(_tag_fat(_tmp252,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 18U: _LL25: _tmp2CC=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL26:
 s=({struct Cyc_PP_Doc*_tmp255[3U];({struct Cyc_PP_Doc*_tmp680=Cyc_PP_text(({const char*_tmp256="sizeof(";_tag_fat(_tmp256,sizeof(char),8U);}));_tmp255[0]=_tmp680;}),({struct Cyc_PP_Doc*_tmp67F=Cyc_Absynpp_exp2doc(_tmp2CC);_tmp255[1]=_tmp67F;}),({struct Cyc_PP_Doc*_tmp67E=Cyc_PP_text(({const char*_tmp257=")";_tag_fat(_tmp257,sizeof(char),2U);}));_tmp255[2]=_tmp67E;});Cyc_PP_cat(_tag_fat(_tmp255,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 41U: _LL27: _tmp2CD=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL28:
# 1181
 s=({struct Cyc_PP_Doc*_tmp258[3U];({struct Cyc_PP_Doc*_tmp683=Cyc_PP_text(({const char*_tmp259="__extension__(";_tag_fat(_tmp259,sizeof(char),15U);}));_tmp258[0]=_tmp683;}),({struct Cyc_PP_Doc*_tmp682=Cyc_Absynpp_exp2doc(_tmp2CD);_tmp258[1]=_tmp682;}),({struct Cyc_PP_Doc*_tmp681=Cyc_PP_text(({const char*_tmp25A=")";_tag_fat(_tmp25A,sizeof(char),2U);}));_tmp258[2]=_tmp681;});Cyc_PP_cat(_tag_fat(_tmp258,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 39U: _LL29: _tmp2CE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL2A:
 s=({struct Cyc_PP_Doc*_tmp25B[3U];({struct Cyc_PP_Doc*_tmp686=Cyc_PP_text(({const char*_tmp25C="valueof(";_tag_fat(_tmp25C,sizeof(char),9U);}));_tmp25B[0]=_tmp686;}),({struct Cyc_PP_Doc*_tmp685=Cyc_Absynpp_typ2doc(_tmp2CE);_tmp25B[1]=_tmp685;}),({struct Cyc_PP_Doc*_tmp684=Cyc_PP_text(({const char*_tmp25D=")";_tag_fat(_tmp25D,sizeof(char),2U);}));_tmp25B[2]=_tmp684;});Cyc_PP_cat(_tag_fat(_tmp25B,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 40U: _LL2B: _tmp2D3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2D2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp2D1=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_tmp2D0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f4;_tmp2CF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f5;_LL2C:
# 1184
 if(_tmp2D3)
s=({struct Cyc_PP_Doc*_tmp25E[7U];({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp25F="__asm__";_tag_fat(_tmp25F,sizeof(char),8U);}));_tmp25E[0]=_tmp68D;}),({struct Cyc_PP_Doc*_tmp68C=Cyc_PP_text(({const char*_tmp260=" volatile (";_tag_fat(_tmp260,sizeof(char),12U);}));_tmp25E[1]=_tmp68C;}),({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp261="\"";_tag_fat(_tmp261,sizeof(char),2U);}));_tmp25E[2]=_tmp68B;}),({struct Cyc_PP_Doc*_tmp68A=Cyc_PP_text(_tmp2D2);_tmp25E[3]=_tmp68A;}),({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp262="\"";_tag_fat(_tmp262,sizeof(char),2U);}));_tmp25E[4]=_tmp689;}),({struct Cyc_PP_Doc*_tmp688=Cyc_Absynpp_asm_iolist_doc(_tmp2D1,_tmp2D0,_tmp2CF);_tmp25E[5]=_tmp688;}),({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp263=")";_tag_fat(_tmp263,sizeof(char),2U);}));_tmp25E[6]=_tmp687;});Cyc_PP_cat(_tag_fat(_tmp25E,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1187
s=({struct Cyc_PP_Doc*_tmp264[6U];({struct Cyc_PP_Doc*_tmp693=Cyc_PP_text(({const char*_tmp265="__asm__(";_tag_fat(_tmp265,sizeof(char),9U);}));_tmp264[0]=_tmp693;}),({struct Cyc_PP_Doc*_tmp692=Cyc_PP_text(({const char*_tmp266="\"";_tag_fat(_tmp266,sizeof(char),2U);}));_tmp264[1]=_tmp692;}),({struct Cyc_PP_Doc*_tmp691=Cyc_PP_text(_tmp2D2);_tmp264[2]=_tmp691;}),({struct Cyc_PP_Doc*_tmp690=Cyc_PP_text(({const char*_tmp267="\"";_tag_fat(_tmp267,sizeof(char),2U);}));_tmp264[3]=_tmp690;}),({struct Cyc_PP_Doc*_tmp68F=Cyc_Absynpp_asm_iolist_doc(_tmp2D1,_tmp2D0,_tmp2CF);_tmp264[4]=_tmp68F;}),({struct Cyc_PP_Doc*_tmp68E=Cyc_PP_text(({const char*_tmp268=")";_tag_fat(_tmp268,sizeof(char),2U);}));_tmp264[5]=_tmp68E;});Cyc_PP_cat(_tag_fat(_tmp264,sizeof(struct Cyc_PP_Doc*),6U));});}
goto _LL0;case 38U: _LL2D: _tmp2D5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2D4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL2E:
# 1190
 s=({struct Cyc_PP_Doc*_tmp269[5U];({struct Cyc_PP_Doc*_tmp698=Cyc_PP_text(({const char*_tmp26A="tagcheck(";_tag_fat(_tmp26A,sizeof(char),10U);}));_tmp269[0]=_tmp698;}),({struct Cyc_PP_Doc*_tmp697=Cyc_Absynpp_exp2doc(_tmp2D5);_tmp269[1]=_tmp697;}),({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp26B=".";_tag_fat(_tmp26B,sizeof(char),2U);}));_tmp269[2]=_tmp696;}),({struct Cyc_PP_Doc*_tmp695=Cyc_PP_textptr(_tmp2D4);_tmp269[3]=_tmp695;}),({struct Cyc_PP_Doc*_tmp694=Cyc_PP_text(({const char*_tmp26C=")";_tag_fat(_tmp26C,sizeof(char),2U);}));_tmp269[4]=_tmp694;});Cyc_PP_cat(_tag_fat(_tmp269,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 19U: _LL2F: _tmp2D7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2D6=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL30:
# 1193
 s=({struct Cyc_PP_Doc*_tmp26D[5U];({struct Cyc_PP_Doc*_tmp69E=Cyc_PP_text(({const char*_tmp26E="offsetof(";_tag_fat(_tmp26E,sizeof(char),10U);}));_tmp26D[0]=_tmp69E;}),({struct Cyc_PP_Doc*_tmp69D=Cyc_Absynpp_typ2doc(_tmp2D7);_tmp26D[1]=_tmp69D;}),({struct Cyc_PP_Doc*_tmp69C=Cyc_PP_text(({const char*_tmp26F=",";_tag_fat(_tmp26F,sizeof(char),2U);}));_tmp26D[2]=_tmp69C;}),({
struct Cyc_PP_Doc*_tmp69B=({struct _fat_ptr _tmp69A=({const char*_tmp270=".";_tag_fat(_tmp270,sizeof(char),2U);});Cyc_PP_seq(_tmp69A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_offsetof_field_to_doc,_tmp2D6));});_tmp26D[3]=_tmp69B;}),({struct Cyc_PP_Doc*_tmp699=Cyc_PP_text(({const char*_tmp271=")";_tag_fat(_tmp271,sizeof(char),2U);}));_tmp26D[4]=_tmp699;});Cyc_PP_cat(_tag_fat(_tmp26D,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 20U: _LL31: _tmp2D8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL32:
 s=({struct Cyc_PP_Doc*_tmp272[2U];({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(({const char*_tmp273="*";_tag_fat(_tmp273,sizeof(char),2U);}));_tmp272[0]=_tmp6A0;}),({struct Cyc_PP_Doc*_tmp69F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D8);_tmp272[1]=_tmp69F;});Cyc_PP_cat(_tag_fat(_tmp272,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 21U: _LL33: _tmp2DA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2D9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL34:
# 1198
 s=({struct Cyc_PP_Doc*_tmp274[3U];({struct Cyc_PP_Doc*_tmp6A3=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2DA);_tmp274[0]=_tmp6A3;}),({struct Cyc_PP_Doc*_tmp6A2=Cyc_PP_text(({const char*_tmp275=".";_tag_fat(_tmp275,sizeof(char),2U);}));_tmp274[1]=_tmp6A2;}),({struct Cyc_PP_Doc*_tmp6A1=Cyc_PP_textptr(_tmp2D9);_tmp274[2]=_tmp6A1;});Cyc_PP_cat(_tag_fat(_tmp274,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 22U: _LL35: _tmp2DC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2DB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL36:
# 1200
 s=({struct Cyc_PP_Doc*_tmp276[3U];({struct Cyc_PP_Doc*_tmp6A6=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2DC);_tmp276[0]=_tmp6A6;}),({struct Cyc_PP_Doc*_tmp6A5=Cyc_PP_text(({const char*_tmp277="->";_tag_fat(_tmp277,sizeof(char),3U);}));_tmp276[1]=_tmp6A5;}),({struct Cyc_PP_Doc*_tmp6A4=Cyc_PP_textptr(_tmp2DB);_tmp276[2]=_tmp6A4;});Cyc_PP_cat(_tag_fat(_tmp276,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 23U: _LL37: _tmp2DE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2DD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL38:
# 1202
 s=({struct Cyc_PP_Doc*_tmp278[4U];({struct Cyc_PP_Doc*_tmp6AA=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2DE);_tmp278[0]=_tmp6AA;}),({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_text(({const char*_tmp279="[";_tag_fat(_tmp279,sizeof(char),2U);}));_tmp278[1]=_tmp6A9;}),({struct Cyc_PP_Doc*_tmp6A8=Cyc_Absynpp_exp2doc(_tmp2DD);_tmp278[2]=_tmp6A8;}),({struct Cyc_PP_Doc*_tmp6A7=Cyc_PP_text(({const char*_tmp27A="]";_tag_fat(_tmp27A,sizeof(char),2U);}));_tmp278[3]=_tmp6A7;});Cyc_PP_cat(_tag_fat(_tmp278,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 24U: _LL39: _tmp2DF=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL3A:
# 1204
 s=({struct Cyc_PP_Doc*_tmp27B[4U];({struct Cyc_PP_Doc*_tmp6AE=Cyc_Absynpp_dollar();_tmp27B[0]=_tmp6AE;}),({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp27C="(";_tag_fat(_tmp27C,sizeof(char),2U);}));_tmp27B[1]=_tmp6AD;}),({struct Cyc_PP_Doc*_tmp6AC=Cyc_Absynpp_exps2doc_prec(20,_tmp2DF);_tmp27B[2]=_tmp6AC;}),({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_text(({const char*_tmp27D=")";_tag_fat(_tmp27D,sizeof(char),2U);}));_tmp27B[3]=_tmp6AB;});Cyc_PP_cat(_tag_fat(_tmp27B,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 25U: _LL3B: _tmp2E1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2E0=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL3C:
# 1206
 s=({struct Cyc_PP_Doc*_tmp27E[4U];({struct Cyc_PP_Doc*_tmp6B3=Cyc_PP_text(({const char*_tmp27F="(";_tag_fat(_tmp27F,sizeof(char),2U);}));_tmp27E[0]=_tmp6B3;}),({struct Cyc_PP_Doc*_tmp6B2=Cyc_Absynpp_typ2doc((*_tmp2E1).f3);_tmp27E[1]=_tmp6B2;}),({struct Cyc_PP_Doc*_tmp6B1=Cyc_PP_text(({const char*_tmp280=")";_tag_fat(_tmp280,sizeof(char),2U);}));_tmp27E[2]=_tmp6B1;}),({
struct Cyc_PP_Doc*_tmp6B0=({struct _fat_ptr _tmp6AF=({const char*_tmp281=",";_tag_fat(_tmp281,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6AF,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2E0));});_tmp27E[3]=_tmp6B0;});Cyc_PP_cat(_tag_fat(_tmp27E,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 26U: _LL3D: _tmp2E2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL3E:
 s=({struct _fat_ptr _tmp6B4=({const char*_tmp282=",";_tag_fat(_tmp282,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6B4,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2E2));});goto _LL0;case 27U: _LL3F: _tmp2E5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2E4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp2E3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL40:
# 1211
 s=({struct Cyc_PP_Doc*_tmp283[8U];({struct Cyc_PP_Doc*_tmp6BC=Cyc_Absynpp_lb();_tmp283[0]=_tmp6BC;}),({struct Cyc_PP_Doc*_tmp6BB=Cyc_PP_text(({const char*_tmp284="for ";_tag_fat(_tmp284,sizeof(char),5U);}));_tmp283[1]=_tmp6BB;}),({
struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_text(*(*_tmp2E5->name).f2);_tmp283[2]=_tmp6BA;}),({struct Cyc_PP_Doc*_tmp6B9=Cyc_PP_text(({const char*_tmp285=" < ";_tag_fat(_tmp285,sizeof(char),4U);}));_tmp283[3]=_tmp6B9;}),({struct Cyc_PP_Doc*_tmp6B8=Cyc_Absynpp_exp2doc(_tmp2E4);_tmp283[4]=_tmp6B8;}),({struct Cyc_PP_Doc*_tmp6B7=Cyc_PP_text(({const char*_tmp286=" : ";_tag_fat(_tmp286,sizeof(char),4U);}));_tmp283[5]=_tmp6B7;}),({
struct Cyc_PP_Doc*_tmp6B6=Cyc_Absynpp_exp2doc(_tmp2E3);_tmp283[6]=_tmp6B6;}),({struct Cyc_PP_Doc*_tmp6B5=Cyc_Absynpp_rb();_tmp283[7]=_tmp6B5;});Cyc_PP_cat(_tag_fat(_tmp283,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 28U: _LL41: _tmp2E7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2E6=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL42:
# 1216
 s=({struct Cyc_PP_Doc*_tmp287[7U];({struct Cyc_PP_Doc*_tmp6C3=Cyc_Absynpp_lb();_tmp287[0]=_tmp6C3;}),({struct Cyc_PP_Doc*_tmp6C2=Cyc_PP_text(({const char*_tmp288="for x ";_tag_fat(_tmp288,sizeof(char),7U);}));_tmp287[1]=_tmp6C2;}),({
struct Cyc_PP_Doc*_tmp6C1=Cyc_PP_text(({const char*_tmp289=" < ";_tag_fat(_tmp289,sizeof(char),4U);}));_tmp287[2]=_tmp6C1;}),({
struct Cyc_PP_Doc*_tmp6C0=Cyc_Absynpp_exp2doc(_tmp2E7);_tmp287[3]=_tmp6C0;}),({
struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_text(({const char*_tmp28A=" : ";_tag_fat(_tmp28A,sizeof(char),4U);}));_tmp287[4]=_tmp6BF;}),({
struct Cyc_PP_Doc*_tmp6BE=Cyc_Absynpp_typ2doc(_tmp2E6);_tmp287[5]=_tmp6BE;}),({
struct Cyc_PP_Doc*_tmp6BD=Cyc_Absynpp_rb();_tmp287[6]=_tmp6BD;});Cyc_PP_cat(_tag_fat(_tmp287,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 29U: _LL43: _tmp2EA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2E9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp2E8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL44: {
# 1224
struct Cyc_List_List*_tmp28B=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2E8);
s=({struct Cyc_PP_Doc*_tmp28C[2U];({struct Cyc_PP_Doc*_tmp6C7=Cyc_Absynpp_qvar2doc(_tmp2EA);_tmp28C[0]=_tmp6C7;}),({
struct Cyc_PP_Doc*_tmp6C6=({struct _fat_ptr _tmp6C5=({const char*_tmp28D=",";_tag_fat(_tmp28D,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6C5,
_tmp2E9 != 0?({struct Cyc_List_List*_tmp28E=_cycalloc(sizeof(*_tmp28E));({struct Cyc_PP_Doc*_tmp6C4=Cyc_Absynpp_tps2doc(_tmp2E9);_tmp28E->hd=_tmp6C4;}),_tmp28E->tl=_tmp28B;_tmp28E;}): _tmp28B);});
# 1226
_tmp28C[1]=_tmp6C6;});Cyc_PP_cat(_tag_fat(_tmp28C,sizeof(struct Cyc_PP_Doc*),2U));});
# 1228
goto _LL0;}case 30U: _LL45: _tmp2EB=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL46:
 s=({struct _fat_ptr _tmp6C8=({const char*_tmp28F=",";_tag_fat(_tmp28F,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6C8,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2EB));});goto _LL0;case 31U: _LL47: _tmp2ED=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2EC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL48:
# 1231
 if(_tmp2ED == 0)
# 1233
s=Cyc_Absynpp_qvar2doc(_tmp2EC->name);else{
# 1235
s=({struct Cyc_PP_Doc*_tmp290[2U];({struct Cyc_PP_Doc*_tmp6CD=Cyc_Absynpp_qvar2doc(_tmp2EC->name);_tmp290[0]=_tmp6CD;}),({
struct Cyc_PP_Doc*_tmp6CC=({struct _fat_ptr _tmp6CB=({const char*_tmp291="(";_tag_fat(_tmp291,sizeof(char),2U);});struct _fat_ptr _tmp6CA=({const char*_tmp292=")";_tag_fat(_tmp292,sizeof(char),2U);});struct _fat_ptr _tmp6C9=({const char*_tmp293=",";_tag_fat(_tmp293,sizeof(char),2U);});Cyc_PP_egroup(_tmp6CB,_tmp6CA,_tmp6C9,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2ED));});_tmp290[1]=_tmp6CC;});Cyc_PP_cat(_tag_fat(_tmp290,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;case 32U: _LL49: _tmp2EE=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL4A:
 _tmp2EF=_tmp2EE;goto _LL4C;case 33U: _LL4B: _tmp2EF=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL4C:
 s=Cyc_Absynpp_qvar2doc(_tmp2EF->name);goto _LL0;case 34U: _LL4D: _tmp2F4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).is_calloc;_tmp2F3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).rgn;_tmp2F2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).elt_type;_tmp2F1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).num_elts;_tmp2F0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).inline_call;_LL4E:
# 1243
 if(_tmp2F4){
# 1245
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(_tmp2F2)),0U);
if(_tmp2F3 == 0)
s=({struct Cyc_PP_Doc*_tmp294[5U];({struct Cyc_PP_Doc*_tmp6D2=Cyc_PP_text(({const char*_tmp295="calloc(";_tag_fat(_tmp295,sizeof(char),8U);}));_tmp294[0]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_Absynpp_exp2doc(_tmp2F1);_tmp294[1]=_tmp6D1;}),({struct Cyc_PP_Doc*_tmp6D0=Cyc_PP_text(({const char*_tmp296=",";_tag_fat(_tmp296,sizeof(char),2U);}));_tmp294[2]=_tmp6D0;}),({struct Cyc_PP_Doc*_tmp6CF=Cyc_Absynpp_exp2doc(st);_tmp294[3]=_tmp6CF;}),({struct Cyc_PP_Doc*_tmp6CE=Cyc_PP_text(({const char*_tmp297=")";_tag_fat(_tmp297,sizeof(char),2U);}));_tmp294[4]=_tmp6CE;});Cyc_PP_cat(_tag_fat(_tmp294,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1249
s=({struct Cyc_PP_Doc*_tmp298[7U];({struct Cyc_PP_Doc*_tmp6D9=Cyc_PP_text(({const char*_tmp299="rcalloc(";_tag_fat(_tmp299,sizeof(char),9U);}));_tmp298[0]=_tmp6D9;}),({struct Cyc_PP_Doc*_tmp6D8=Cyc_Absynpp_exp2doc(_tmp2F3);_tmp298[1]=_tmp6D8;}),({struct Cyc_PP_Doc*_tmp6D7=Cyc_PP_text(({const char*_tmp29A=",";_tag_fat(_tmp29A,sizeof(char),2U);}));_tmp298[2]=_tmp6D7;}),({
struct Cyc_PP_Doc*_tmp6D6=Cyc_Absynpp_exp2doc(_tmp2F1);_tmp298[3]=_tmp6D6;}),({struct Cyc_PP_Doc*_tmp6D5=Cyc_PP_text(({const char*_tmp29B=",";_tag_fat(_tmp29B,sizeof(char),2U);}));_tmp298[4]=_tmp6D5;}),({struct Cyc_PP_Doc*_tmp6D4=Cyc_Absynpp_exp2doc(st);_tmp298[5]=_tmp6D4;}),({struct Cyc_PP_Doc*_tmp6D3=Cyc_PP_text(({const char*_tmp29C=")";_tag_fat(_tmp29C,sizeof(char),2U);}));_tmp298[6]=_tmp6D3;});Cyc_PP_cat(_tag_fat(_tmp298,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1252
struct Cyc_Absyn_Exp*new_e;
# 1254
if(_tmp2F2 == 0)
new_e=_tmp2F1;else{
# 1257
new_e=({struct Cyc_Absyn_Exp*_tmp6DA=Cyc_Absyn_sizeoftype_exp(*_tmp2F2,0U);Cyc_Absyn_times_exp(_tmp6DA,_tmp2F1,0U);});}
# 1259
if(_tmp2F3 == 0)
s=({struct Cyc_PP_Doc*_tmp29D[3U];({struct Cyc_PP_Doc*_tmp6DD=Cyc_PP_text(({const char*_tmp29E="malloc(";_tag_fat(_tmp29E,sizeof(char),8U);}));_tmp29D[0]=_tmp6DD;}),({struct Cyc_PP_Doc*_tmp6DC=Cyc_Absynpp_exp2doc(new_e);_tmp29D[1]=_tmp6DC;}),({struct Cyc_PP_Doc*_tmp6DB=Cyc_PP_text(({const char*_tmp29F=")";_tag_fat(_tmp29F,sizeof(char),2U);}));_tmp29D[2]=_tmp6DB;});Cyc_PP_cat(_tag_fat(_tmp29D,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1262
if(_tmp2F0)
s=({struct Cyc_PP_Doc*_tmp2A0[5U];({struct Cyc_PP_Doc*_tmp6E2=Cyc_PP_text(({const char*_tmp2A1="rmalloc_inline(";_tag_fat(_tmp2A1,sizeof(char),16U);}));_tmp2A0[0]=_tmp6E2;}),({struct Cyc_PP_Doc*_tmp6E1=Cyc_Absynpp_exp2doc(_tmp2F3);_tmp2A0[1]=_tmp6E1;}),({struct Cyc_PP_Doc*_tmp6E0=Cyc_PP_text(({const char*_tmp2A2=",";_tag_fat(_tmp2A2,sizeof(char),2U);}));_tmp2A0[2]=_tmp6E0;}),({
struct Cyc_PP_Doc*_tmp6DF=Cyc_Absynpp_exp2doc(new_e);_tmp2A0[3]=_tmp6DF;}),({struct Cyc_PP_Doc*_tmp6DE=Cyc_PP_text(({const char*_tmp2A3=")";_tag_fat(_tmp2A3,sizeof(char),2U);}));_tmp2A0[4]=_tmp6DE;});Cyc_PP_cat(_tag_fat(_tmp2A0,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1266
s=({struct Cyc_PP_Doc*_tmp2A4[5U];({struct Cyc_PP_Doc*_tmp6E7=Cyc_PP_text(({const char*_tmp2A5="rmalloc(";_tag_fat(_tmp2A5,sizeof(char),9U);}));_tmp2A4[0]=_tmp6E7;}),({struct Cyc_PP_Doc*_tmp6E6=Cyc_Absynpp_exp2doc(_tmp2F3);_tmp2A4[1]=_tmp6E6;}),({struct Cyc_PP_Doc*_tmp6E5=Cyc_PP_text(({const char*_tmp2A6=",";_tag_fat(_tmp2A6,sizeof(char),2U);}));_tmp2A4[2]=_tmp6E5;}),({
struct Cyc_PP_Doc*_tmp6E4=Cyc_Absynpp_exp2doc(new_e);_tmp2A4[3]=_tmp6E4;}),({struct Cyc_PP_Doc*_tmp6E3=Cyc_PP_text(({const char*_tmp2A7=")";_tag_fat(_tmp2A7,sizeof(char),2U);}));_tmp2A4[4]=_tmp6E3;});Cyc_PP_cat(_tag_fat(_tmp2A4,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1270
goto _LL0;case 35U: _LL4F: _tmp2F6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2F5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL50:
# 1272
 s=({struct Cyc_PP_Doc*_tmp2A8[3U];({struct Cyc_PP_Doc*_tmp6EA=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2F6);_tmp2A8[0]=_tmp6EA;}),({struct Cyc_PP_Doc*_tmp6E9=Cyc_PP_text(({const char*_tmp2A9=" :=: ";_tag_fat(_tmp2A9,sizeof(char),6U);}));_tmp2A8[1]=_tmp6E9;}),({struct Cyc_PP_Doc*_tmp6E8=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2F5);_tmp2A8[2]=_tmp6E8;});Cyc_PP_cat(_tag_fat(_tmp2A8,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 36U: _LL51: _tmp2F8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp2F7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL52:
# 1275
 s=({struct _fat_ptr _tmp6EB=({const char*_tmp2AA=",";_tag_fat(_tmp2AA,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6EB,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2F7));});
goto _LL0;default: _LL53: _tmp2F9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL54:
# 1278
 s=({struct Cyc_PP_Doc*_tmp2AB[7U];({struct Cyc_PP_Doc*_tmp6F2=Cyc_PP_text(({const char*_tmp2AC="(";_tag_fat(_tmp2AC,sizeof(char),2U);}));_tmp2AB[0]=_tmp6F2;}),({struct Cyc_PP_Doc*_tmp6F1=Cyc_Absynpp_lb();_tmp2AB[1]=_tmp6F1;}),({struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_blank_doc();_tmp2AB[2]=_tmp6F0;}),({
struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F9,1,0));_tmp2AB[3]=_tmp6EF;}),({
struct Cyc_PP_Doc*_tmp6EE=Cyc_PP_blank_doc();_tmp2AB[4]=_tmp6EE;}),({struct Cyc_PP_Doc*_tmp6ED=Cyc_Absynpp_rb();_tmp2AB[5]=_tmp6ED;}),({struct Cyc_PP_Doc*_tmp6EC=Cyc_PP_text(({const char*_tmp2AD=")";_tag_fat(_tmp2AD,sizeof(char),2U);}));_tmp2AB[6]=_tmp6EC;});Cyc_PP_cat(_tag_fat(_tmp2AB,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}_LL0:;}
# 1283
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2FA[3U];({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp2FB="(";_tag_fat(_tmp2FB,sizeof(char),2U);}));_tmp2FA[0]=_tmp6F4;}),_tmp2FA[1]=s,({struct Cyc_PP_Doc*_tmp6F3=Cyc_PP_text(({const char*_tmp2FC=")";_tag_fat(_tmp2FC,sizeof(char),2U);}));_tmp2FA[2]=_tmp6F3;});Cyc_PP_cat(_tag_fat(_tmp2FA,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1288
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2FD=d;struct _fat_ptr*_tmp304;struct Cyc_Absyn_Exp*_tmp303;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2FD)->tag == 0U){_LL1: _tmp303=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2FD)->f1;_LL2:
 return({struct Cyc_PP_Doc*_tmp2FE[3U];({struct Cyc_PP_Doc*_tmp6F7=Cyc_PP_text(({const char*_tmp2FF=".[";_tag_fat(_tmp2FF,sizeof(char),3U);}));_tmp2FE[0]=_tmp6F7;}),({struct Cyc_PP_Doc*_tmp6F6=Cyc_Absynpp_exp2doc(_tmp303);_tmp2FE[1]=_tmp6F6;}),({struct Cyc_PP_Doc*_tmp6F5=Cyc_PP_text(({const char*_tmp300="]";_tag_fat(_tmp300,sizeof(char),2U);}));_tmp2FE[2]=_tmp6F5;});Cyc_PP_cat(_tag_fat(_tmp2FE,sizeof(struct Cyc_PP_Doc*),3U));});}else{_LL3: _tmp304=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2FD)->f1;_LL4:
 return({struct Cyc_PP_Doc*_tmp301[2U];({struct Cyc_PP_Doc*_tmp6F9=Cyc_PP_text(({const char*_tmp302=".";_tag_fat(_tmp302,sizeof(char),2U);}));_tmp301[0]=_tmp6F9;}),({struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_textptr(_tmp304);_tmp301[1]=_tmp6F8;});Cyc_PP_cat(_tag_fat(_tmp301,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}
# 1295
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp305[2U];({struct Cyc_PP_Doc*_tmp6FE=({struct _fat_ptr _tmp6FD=({const char*_tmp306="";_tag_fat(_tmp306,sizeof(char),1U);});struct _fat_ptr _tmp6FC=({const char*_tmp307="=";_tag_fat(_tmp307,sizeof(char),2U);});struct _fat_ptr _tmp6FB=({const char*_tmp308="=";_tag_fat(_tmp308,sizeof(char),2U);});Cyc_PP_egroup(_tmp6FD,_tmp6FC,_tmp6FB,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp305[0]=_tmp6FE;}),({
struct Cyc_PP_Doc*_tmp6FA=Cyc_Absynpp_exp2doc((*de).f2);_tmp305[1]=_tmp6FA;});Cyc_PP_cat(_tag_fat(_tmp305,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1301
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _tmp701=({const char*_tmp309="";_tag_fat(_tmp309,sizeof(char),1U);});struct _fat_ptr _tmp700=({const char*_tmp30A="";_tag_fat(_tmp30A,sizeof(char),1U);});struct _fat_ptr _tmp6FF=({const char*_tmp30B=",";_tag_fat(_tmp30B,sizeof(char),2U);});Cyc_PP_group(_tmp701,_tmp700,_tmp6FF,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1305
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_tmp320=({unsigned _tmp31F=28U + 1U;char*_tmp31E=_cycalloc_atomic(_check_times(_tmp31F,sizeof(char)));({{unsigned _tmp569=28U;unsigned i;for(i=0;i < _tmp569;++ i){_tmp31E[i]='z';}_tmp31E[_tmp569]=0;}0;});_tmp31E;});_tag_fat(_tmp320,sizeof(char),29U);});
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
# 1319
return(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1323
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp321=c;struct _fat_ptr _tmp343;struct _fat_ptr _tmp342;struct _fat_ptr _tmp341;enum Cyc_Absyn_Sign _tmp340;long long _tmp33F;enum Cyc_Absyn_Sign _tmp33E;int _tmp33D;enum Cyc_Absyn_Sign _tmp33C;short _tmp33B;struct _fat_ptr _tmp33A;enum Cyc_Absyn_Sign _tmp339;char _tmp338;switch((_tmp321.String_c).tag){case 2U: _LL1: _tmp339=((_tmp321.Char_c).val).f1;_tmp338=((_tmp321.Char_c).val).f2;_LL2:
 return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp324=({struct Cyc_String_pa_PrintArg_struct _tmp56A;_tmp56A.tag=0U,({struct _fat_ptr _tmp702=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(_tmp338));_tmp56A.f1=_tmp702;});_tmp56A;});void*_tmp322[1U];_tmp322[0]=& _tmp324;({struct _fat_ptr _tmp703=({const char*_tmp323="'%s'";_tag_fat(_tmp323,sizeof(char),5U);});Cyc_aprintf(_tmp703,_tag_fat(_tmp322,sizeof(void*),1U));});}));case 3U: _LL3: _tmp33A=(_tmp321.Wchar_c).val;_LL4:
 return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp327=({struct Cyc_String_pa_PrintArg_struct _tmp56B;_tmp56B.tag=0U,_tmp56B.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp33A);_tmp56B;});void*_tmp325[1U];_tmp325[0]=& _tmp327;({struct _fat_ptr _tmp704=({const char*_tmp326="L'%s'";_tag_fat(_tmp326,sizeof(char),6U);});Cyc_aprintf(_tmp704,_tag_fat(_tmp325,sizeof(void*),1U));});}));case 4U: _LL5: _tmp33C=((_tmp321.Short_c).val).f1;_tmp33B=((_tmp321.Short_c).val).f2;_LL6:
 return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp32A=({struct Cyc_Int_pa_PrintArg_struct _tmp56C;_tmp56C.tag=1U,_tmp56C.f1=(unsigned long)((int)_tmp33B);_tmp56C;});void*_tmp328[1U];_tmp328[0]=& _tmp32A;({struct _fat_ptr _tmp705=({const char*_tmp329="%d";_tag_fat(_tmp329,sizeof(char),3U);});Cyc_aprintf(_tmp705,_tag_fat(_tmp328,sizeof(void*),1U));});}));case 5U: _LL7: _tmp33E=((_tmp321.Int_c).val).f1;_tmp33D=((_tmp321.Int_c).val).f2;_LL8:
# 1329
 if((int)_tmp33E == (int)1U)return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp32D=({struct Cyc_Int_pa_PrintArg_struct _tmp56D;_tmp56D.tag=1U,_tmp56D.f1=(unsigned)_tmp33D;_tmp56D;});void*_tmp32B[1U];_tmp32B[0]=& _tmp32D;({struct _fat_ptr _tmp706=({const char*_tmp32C="%uU";_tag_fat(_tmp32C,sizeof(char),4U);});Cyc_aprintf(_tmp706,_tag_fat(_tmp32B,sizeof(void*),1U));});}));else{
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp330=({struct Cyc_Int_pa_PrintArg_struct _tmp56E;_tmp56E.tag=1U,_tmp56E.f1=(unsigned long)_tmp33D;_tmp56E;});void*_tmp32E[1U];_tmp32E[0]=& _tmp330;({struct _fat_ptr _tmp707=({const char*_tmp32F="%d";_tag_fat(_tmp32F,sizeof(char),3U);});Cyc_aprintf(_tmp707,_tag_fat(_tmp32E,sizeof(void*),1U));});}));}case 6U: _LL9: _tmp340=((_tmp321.LongLong_c).val).f1;_tmp33F=((_tmp321.LongLong_c).val).f2;_LLA:
# 1333
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp33F));case 7U: _LLB: _tmp341=((_tmp321.Float_c).val).f1;_LLC:
 return Cyc_PP_text(_tmp341);case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp331="NULL";_tag_fat(_tmp331,sizeof(char),5U);}));case 8U: _LLF: _tmp342=(_tmp321.String_c).val;_LL10:
 return({struct Cyc_PP_Doc*_tmp332[3U];({struct Cyc_PP_Doc*_tmp70A=Cyc_PP_text(({const char*_tmp333="\"";_tag_fat(_tmp333,sizeof(char),2U);}));_tmp332[0]=_tmp70A;}),({struct Cyc_PP_Doc*_tmp709=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp342));_tmp332[1]=_tmp709;}),({struct Cyc_PP_Doc*_tmp708=Cyc_PP_text(({const char*_tmp334="\"";_tag_fat(_tmp334,sizeof(char),2U);}));_tmp332[2]=_tmp708;});Cyc_PP_cat(_tag_fat(_tmp332,sizeof(struct Cyc_PP_Doc*),3U));});default: _LL11: _tmp343=(_tmp321.Wstring_c).val;_LL12:
 return({struct Cyc_PP_Doc*_tmp335[3U];({struct Cyc_PP_Doc*_tmp70D=Cyc_PP_text(({const char*_tmp336="L\"";_tag_fat(_tmp336,sizeof(char),3U);}));_tmp335[0]=_tmp70D;}),({struct Cyc_PP_Doc*_tmp70C=Cyc_PP_text(_tmp343);_tmp335[1]=_tmp70C;}),({struct Cyc_PP_Doc*_tmp70B=Cyc_PP_text(({const char*_tmp337="\"";_tag_fat(_tmp337,sizeof(char),2U);}));_tmp335[2]=_tmp70B;});Cyc_PP_cat(_tag_fat(_tmp335,sizeof(struct Cyc_PP_Doc*),3U));});}_LL0:;}
# 1341
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp710=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp346=({struct Cyc_String_pa_PrintArg_struct _tmp56F;_tmp56F.tag=0U,({struct _fat_ptr _tmp70E=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp56F.f1=_tmp70E;});_tmp56F;});void*_tmp344[1U];_tmp344[0]=& _tmp346;({struct _fat_ptr _tmp70F=({const char*_tmp345="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_tmp345,sizeof(char),47U);});Cyc_aprintf(_tmp70F,_tag_fat(_tmp344,sizeof(void*),1U));});});_tmp347->f1=_tmp710;});_tmp347;}));
# 1347
return({struct Cyc_PP_Doc*_tmp348[3U];({struct Cyc_PP_Doc*_tmp713=Cyc_PP_text(({const char*_tmp349="numelts(";_tag_fat(_tmp349,sizeof(char),9U);}));_tmp348[0]=_tmp713;}),({struct Cyc_PP_Doc*_tmp712=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp348[1]=_tmp712;}),({struct Cyc_PP_Doc*_tmp711=Cyc_PP_text(({const char*_tmp34A=")";_tag_fat(_tmp34A,sizeof(char),2U);}));_tmp348[2]=_tmp711;});Cyc_PP_cat(_tag_fat(_tmp348,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1349
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp716=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34D=({struct Cyc_String_pa_PrintArg_struct _tmp570;_tmp570.tag=0U,({struct _fat_ptr _tmp714=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp570.f1=_tmp714;});_tmp570;});void*_tmp34B[1U];_tmp34B[0]=& _tmp34D;({struct _fat_ptr _tmp715=({const char*_tmp34C="Absynpp::primapp2doc: %s with no args";_tag_fat(_tmp34C,sizeof(char),38U);});Cyc_aprintf(_tmp715,_tag_fat(_tmp34B,sizeof(void*),1U));});});_tmp34E->f1=_tmp716;});_tmp34E;}));else{
# 1353
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp34F[3U];_tmp34F[0]=ps,({struct Cyc_PP_Doc*_tmp717=Cyc_PP_text(({const char*_tmp350=" ";_tag_fat(_tmp350,sizeof(char),2U);}));_tmp34F[1]=_tmp717;}),_tmp34F[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_tmp34F,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp71A=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp353=({struct Cyc_String_pa_PrintArg_struct _tmp571;_tmp571.tag=0U,({struct _fat_ptr _tmp718=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp571.f1=_tmp718;});_tmp571;});void*_tmp351[1U];_tmp351[0]=& _tmp353;({struct _fat_ptr _tmp719=({const char*_tmp352="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_tmp352,sizeof(char),47U);});Cyc_aprintf(_tmp719,_tag_fat(_tmp351,sizeof(void*),1U));});});_tmp354->f1=_tmp71A;});_tmp354;}));else{
# 1359
return({struct Cyc_PP_Doc*_tmp355[5U];_tmp355[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp71C=Cyc_PP_text(({const char*_tmp356=" ";_tag_fat(_tmp356,sizeof(char),2U);}));_tmp355[1]=_tmp71C;}),_tmp355[2]=ps,({struct Cyc_PP_Doc*_tmp71B=Cyc_PP_text(({const char*_tmp357=" ";_tag_fat(_tmp357,sizeof(char),2U);}));_tmp355[3]=_tmp71B;}),_tmp355[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_tmp355,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1363
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
# 1388
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1392
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp36E=s->r;void*_tmp36F=_tmp36E;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp36F)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1407 "absynpp.cyc"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_tmp370=d->r;void*_tmp371=_tmp370;struct Cyc_Absyn_Vardecl*_tmp374;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp371)->tag == 0U){_LL1: _tmp374=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp371)->f1;_LL2:
# 1410
 if(((int(*)(int(*compare)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*l,struct _tuple0*x))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,varsinblock,_tmp374->name))
return({struct _tuple14 _tmp572;_tmp572.f1=1,({struct Cyc_List_List*_tmp71D=({struct Cyc_List_List*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->hd=_tmp374->name,_tmp372->tl=0;_tmp372;});_tmp572.f2=_tmp71D;});_tmp572;});else{
# 1413
return({struct _tuple14 _tmp573;_tmp573.f1=0,({struct Cyc_List_List*_tmp71E=({struct Cyc_List_List*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->hd=_tmp374->name,_tmp373->tl=varsinblock;_tmp373;});_tmp573.f2=_tmp71E;});_tmp573;});}}else{_LL3: _LL4:
# 1415
 return({struct _tuple14 _tmp574;_tmp574.f1=0,_tmp574.f2=varsinblock;_tmp574;});}_LL0:;}
# 1419
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_tmp375[8U];({struct Cyc_PP_Doc*_tmp726=Cyc_PP_text(({const char*_tmp376="(";_tag_fat(_tmp376,sizeof(char),2U);}));_tmp375[0]=_tmp726;}),({struct Cyc_PP_Doc*_tmp725=Cyc_Absynpp_lb();_tmp375[1]=_tmp725;}),({struct Cyc_PP_Doc*_tmp724=Cyc_PP_blank_doc();_tmp375[2]=_tmp724;}),({struct Cyc_PP_Doc*_tmp723=Cyc_PP_nest(2,d);_tmp375[3]=_tmp723;}),({struct Cyc_PP_Doc*_tmp722=Cyc_PP_line_doc();_tmp375[4]=_tmp722;}),({struct Cyc_PP_Doc*_tmp721=Cyc_Absynpp_rb();_tmp375[5]=_tmp721;}),({
struct Cyc_PP_Doc*_tmp720=Cyc_PP_text(({const char*_tmp377=");";_tag_fat(_tmp377,sizeof(char),3U);}));_tmp375[6]=_tmp720;}),({struct Cyc_PP_Doc*_tmp71F=Cyc_PP_line_doc();_tmp375[7]=_tmp71F;});Cyc_PP_cat(_tag_fat(_tmp375,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1424
return({struct Cyc_PP_Doc*_tmp378[6U];({struct Cyc_PP_Doc*_tmp72C=Cyc_Absynpp_lb();_tmp378[0]=_tmp72C;}),({struct Cyc_PP_Doc*_tmp72B=Cyc_PP_blank_doc();_tmp378[1]=_tmp72B;}),({struct Cyc_PP_Doc*_tmp72A=Cyc_PP_nest(2,d);_tmp378[2]=_tmp72A;}),({struct Cyc_PP_Doc*_tmp729=Cyc_PP_line_doc();_tmp378[3]=_tmp729;}),({struct Cyc_PP_Doc*_tmp728=Cyc_Absynpp_rb();_tmp378[4]=_tmp728;}),({struct Cyc_PP_Doc*_tmp727=Cyc_PP_line_doc();_tmp378[5]=_tmp727;});Cyc_PP_cat(_tag_fat(_tmp378,sizeof(struct Cyc_PP_Doc*),6U));});}}
# 1427
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_tmp379=st->r;void*_tmp37A=_tmp379;struct Cyc_Absyn_Stmt*_tmp3C9;struct Cyc_List_List*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C6;struct _fat_ptr*_tmp3C5;struct Cyc_Absyn_Stmt*_tmp3C4;struct Cyc_Absyn_Decl*_tmp3C3;struct Cyc_Absyn_Stmt*_tmp3C2;struct Cyc_List_List*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BB;struct _fat_ptr*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Stmt*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Stmt*_tmp3B6;struct Cyc_Absyn_Stmt*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_Absyn_Stmt*_tmp3B3;struct Cyc_Absyn_Stmt*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B1;switch(*((int*)_tmp37A)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp37B=";";_tag_fat(_tmp37B,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp3B1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_LL4:
 s=({struct Cyc_PP_Doc*_tmp37C[2U];({struct Cyc_PP_Doc*_tmp72E=Cyc_Absynpp_exp2doc(_tmp3B1);_tmp37C[0]=_tmp72E;}),({struct Cyc_PP_Doc*_tmp72D=Cyc_PP_text(({const char*_tmp37D=";";_tag_fat(_tmp37D,sizeof(char),2U);}));_tmp37C[1]=_tmp72D;});Cyc_PP_cat(_tag_fat(_tmp37C,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 2U: _LL5: _tmp3B3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp3B2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL6:
# 1433
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3B3))
s=({struct Cyc_PP_Doc*_tmp37E[2U];({struct Cyc_PP_Doc*_tmp732=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(_tmp3B3,0,0));_tmp37E[0]=_tmp732;}),
Cyc_Absynpp_is_declaration(_tmp3B2)?({
struct Cyc_PP_Doc*_tmp731=({int _tmp730=stmtexp;Cyc_Absynpp_block(_tmp730,Cyc_Absynpp_stmt2doc(_tmp3B2,stmtexp,0));});_tmp37E[1]=_tmp731;}):({
struct Cyc_PP_Doc*_tmp72F=Cyc_Absynpp_stmt2doc(_tmp3B2,stmtexp,varsinblock);_tmp37E[1]=_tmp72F;});Cyc_PP_cat(_tag_fat(_tmp37E,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(Cyc_Absynpp_is_declaration(_tmp3B2))
s=({struct Cyc_PP_Doc*_tmp37F[3U];({struct Cyc_PP_Doc*_tmp736=Cyc_Absynpp_stmt2doc(_tmp3B3,0,varsinblock);_tmp37F[0]=_tmp736;}),({
struct Cyc_PP_Doc*_tmp735=Cyc_PP_line_doc();_tmp37F[1]=_tmp735;}),({
struct Cyc_PP_Doc*_tmp734=({int _tmp733=stmtexp;Cyc_Absynpp_block(_tmp733,Cyc_Absynpp_stmt2doc(_tmp3B2,stmtexp,0));});_tmp37F[2]=_tmp734;});Cyc_PP_cat(_tag_fat(_tmp37F,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1444
s=({struct Cyc_PP_Doc*_tmp380[3U];({struct Cyc_PP_Doc*_tmp739=Cyc_Absynpp_stmt2doc(_tmp3B3,0,varsinblock);_tmp380[0]=_tmp739;}),({struct Cyc_PP_Doc*_tmp738=Cyc_PP_line_doc();_tmp380[1]=_tmp738;}),({
struct Cyc_PP_Doc*_tmp737=Cyc_Absynpp_stmt2doc(_tmp3B2,stmtexp,varsinblock);_tmp380[2]=_tmp737;});Cyc_PP_cat(_tag_fat(_tmp380,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1448
s=({struct Cyc_PP_Doc*_tmp381[3U];({struct Cyc_PP_Doc*_tmp73C=Cyc_Absynpp_stmt2doc(_tmp3B3,0,0);_tmp381[0]=_tmp73C;}),({struct Cyc_PP_Doc*_tmp73B=Cyc_PP_line_doc();_tmp381[1]=_tmp73B;}),({struct Cyc_PP_Doc*_tmp73A=Cyc_Absynpp_stmt2doc(_tmp3B2,stmtexp,0);_tmp381[2]=_tmp73A;});Cyc_PP_cat(_tag_fat(_tmp381,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 3U: _LL7: _tmp3B4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_LL8:
# 1451
 if(_tmp3B4 == 0)
s=Cyc_PP_text(({const char*_tmp382="return;";_tag_fat(_tmp382,sizeof(char),8U);}));else{
# 1454
s=({struct Cyc_PP_Doc*_tmp383[3U];({struct Cyc_PP_Doc*_tmp740=Cyc_PP_text(({const char*_tmp384="return ";_tag_fat(_tmp384,sizeof(char),8U);}));_tmp383[0]=_tmp740;}),
_tmp3B4 == 0?({struct Cyc_PP_Doc*_tmp73F=Cyc_PP_nil_doc();_tmp383[1]=_tmp73F;}):({struct Cyc_PP_Doc*_tmp73E=Cyc_Absynpp_exp2doc(_tmp3B4);_tmp383[1]=_tmp73E;}),({
struct Cyc_PP_Doc*_tmp73D=Cyc_PP_text(({const char*_tmp385=";";_tag_fat(_tmp385,sizeof(char),2U);}));_tmp383[2]=_tmp73D;});Cyc_PP_cat(_tag_fat(_tmp383,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 4U: _LL9: _tmp3B7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp3B6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_tmp3B5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37A)->f3;_LLA: {
# 1459
int print_else;
{void*_tmp386=_tmp3B5->r;void*_tmp387=_tmp386;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp387)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1464
s=({struct Cyc_PP_Doc*_tmp388[5U];({struct Cyc_PP_Doc*_tmp749=Cyc_PP_text(({const char*_tmp389="if (";_tag_fat(_tmp389,sizeof(char),5U);}));_tmp388[0]=_tmp749;}),({
struct Cyc_PP_Doc*_tmp748=Cyc_Absynpp_exp2doc(_tmp3B7);_tmp388[1]=_tmp748;}),({
struct Cyc_PP_Doc*_tmp747=Cyc_PP_text(({const char*_tmp38A=") ";_tag_fat(_tmp38A,sizeof(char),3U);}));_tmp388[2]=_tmp747;}),({
struct Cyc_PP_Doc*_tmp746=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(_tmp3B6,0,0));_tmp388[3]=_tmp746;}),
print_else?({
struct Cyc_PP_Doc*_tmp745=({struct Cyc_PP_Doc*_tmp38B[3U];({struct Cyc_PP_Doc*_tmp744=Cyc_PP_line_doc();_tmp38B[0]=_tmp744;}),({
struct Cyc_PP_Doc*_tmp743=Cyc_PP_text(({const char*_tmp38C="else ";_tag_fat(_tmp38C,sizeof(char),6U);}));_tmp38B[1]=_tmp743;}),({
struct Cyc_PP_Doc*_tmp742=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(_tmp3B5,0,0));_tmp38B[2]=_tmp742;});Cyc_PP_cat(_tag_fat(_tmp38B,sizeof(struct Cyc_PP_Doc*),3U));});
# 1469
_tmp388[4]=_tmp745;}):({
# 1472
struct Cyc_PP_Doc*_tmp741=Cyc_PP_nil_doc();_tmp388[4]=_tmp741;});Cyc_PP_cat(_tag_fat(_tmp388,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp3B9=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1).f1;_tmp3B8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LLC:
# 1475
 s=({struct Cyc_PP_Doc*_tmp38D[4U];({struct Cyc_PP_Doc*_tmp74D=Cyc_PP_text(({const char*_tmp38E="while (";_tag_fat(_tmp38E,sizeof(char),8U);}));_tmp38D[0]=_tmp74D;}),({
struct Cyc_PP_Doc*_tmp74C=Cyc_Absynpp_exp2doc(_tmp3B9);_tmp38D[1]=_tmp74C;}),({
struct Cyc_PP_Doc*_tmp74B=Cyc_PP_text(({const char*_tmp38F=") ";_tag_fat(_tmp38F,sizeof(char),3U);}));_tmp38D[2]=_tmp74B;}),({
struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(_tmp3B8,0,0));_tmp38D[3]=_tmp74A;});Cyc_PP_cat(_tag_fat(_tmp38D,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp390="break;";_tag_fat(_tmp390,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp391="continue;";_tag_fat(_tmp391,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp3BA=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_LL12:
 s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp394=({struct Cyc_String_pa_PrintArg_struct _tmp575;_tmp575.tag=0U,_tmp575.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp3BA);_tmp575;});void*_tmp392[1U];_tmp392[0]=& _tmp394;({struct _fat_ptr _tmp74E=({const char*_tmp393="goto %s;";_tag_fat(_tmp393,sizeof(char),9U);});Cyc_aprintf(_tmp74E,_tag_fat(_tmp392,sizeof(void*),1U));});}));goto _LL0;case 9U: _LL13: _tmp3BE=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp3BD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2).f1;_tmp3BC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37A)->f3).f1;_tmp3BB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37A)->f4;_LL14:
# 1484
 s=({struct Cyc_PP_Doc*_tmp395[8U];({struct Cyc_PP_Doc*_tmp756=Cyc_PP_text(({const char*_tmp396="for(";_tag_fat(_tmp396,sizeof(char),5U);}));_tmp395[0]=_tmp756;}),({
struct Cyc_PP_Doc*_tmp755=Cyc_Absynpp_exp2doc(_tmp3BE);_tmp395[1]=_tmp755;}),({
struct Cyc_PP_Doc*_tmp754=Cyc_PP_text(({const char*_tmp397="; ";_tag_fat(_tmp397,sizeof(char),3U);}));_tmp395[2]=_tmp754;}),({
struct Cyc_PP_Doc*_tmp753=Cyc_Absynpp_exp2doc(_tmp3BD);_tmp395[3]=_tmp753;}),({
struct Cyc_PP_Doc*_tmp752=Cyc_PP_text(({const char*_tmp398="; ";_tag_fat(_tmp398,sizeof(char),3U);}));_tmp395[4]=_tmp752;}),({
struct Cyc_PP_Doc*_tmp751=Cyc_Absynpp_exp2doc(_tmp3BC);_tmp395[5]=_tmp751;}),({
struct Cyc_PP_Doc*_tmp750=Cyc_PP_text(({const char*_tmp399=") ";_tag_fat(_tmp399,sizeof(char),3U);}));_tmp395[6]=_tmp750;}),({
struct Cyc_PP_Doc*_tmp74F=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(_tmp3BB,0,0));_tmp395[7]=_tmp74F;});Cyc_PP_cat(_tag_fat(_tmp395,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 10U: _LL15: _tmp3C0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp3BF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL16:
# 1494
 s=({struct Cyc_PP_Doc*_tmp39A[8U];({struct Cyc_PP_Doc*_tmp75E=Cyc_PP_text(({const char*_tmp39B="switch (";_tag_fat(_tmp39B,sizeof(char),9U);}));_tmp39A[0]=_tmp75E;}),({
struct Cyc_PP_Doc*_tmp75D=Cyc_Absynpp_exp2doc(_tmp3C0);_tmp39A[1]=_tmp75D;}),({
struct Cyc_PP_Doc*_tmp75C=Cyc_PP_text(({const char*_tmp39C=") ";_tag_fat(_tmp39C,sizeof(char),3U);}));_tmp39A[2]=_tmp75C;}),({
struct Cyc_PP_Doc*_tmp75B=Cyc_Absynpp_lb();_tmp39A[3]=_tmp75B;}),({
struct Cyc_PP_Doc*_tmp75A=Cyc_PP_line_doc();_tmp39A[4]=_tmp75A;}),({
struct Cyc_PP_Doc*_tmp759=Cyc_Absynpp_switchclauses2doc(_tmp3BF);_tmp39A[5]=_tmp759;}),({
struct Cyc_PP_Doc*_tmp758=Cyc_PP_line_doc();_tmp39A[6]=_tmp758;}),({
struct Cyc_PP_Doc*_tmp757=Cyc_Absynpp_rb();_tmp39A[7]=_tmp757;});Cyc_PP_cat(_tag_fat(_tmp39A,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp39D="fallthru;";_tag_fat(_tmp39D,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp3C1=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_LL1A:
# 1505
 s=({struct Cyc_PP_Doc*_tmp39E[3U];({struct Cyc_PP_Doc*_tmp761=Cyc_PP_text(({const char*_tmp39F="fallthru(";_tag_fat(_tmp39F,sizeof(char),10U);}));_tmp39E[0]=_tmp761;}),({struct Cyc_PP_Doc*_tmp760=Cyc_Absynpp_exps2doc_prec(20,_tmp3C1);_tmp39E[1]=_tmp760;}),({struct Cyc_PP_Doc*_tmp75F=Cyc_PP_text(({const char*_tmp3A0=");";_tag_fat(_tmp3A0,sizeof(char),3U);}));_tmp39E[2]=_tmp75F;});Cyc_PP_cat(_tag_fat(_tmp39E,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 12U: _LL1B: _tmp3C3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp3C2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL1C: {
# 1507
struct _tuple14 _tmp3A1=Cyc_Absynpp_shadows(_tmp3C3,varsinblock);struct _tuple14 _tmp3A2=_tmp3A1;int _tmp3A5;struct Cyc_List_List*_tmp3A4;_LL29: _tmp3A5=_tmp3A2.f1;_tmp3A4=_tmp3A2.f2;_LL2A:;
s=({struct Cyc_PP_Doc*_tmp3A3[3U];({struct Cyc_PP_Doc*_tmp764=Cyc_Absynpp_decl2doc(_tmp3C3);_tmp3A3[0]=_tmp764;}),({struct Cyc_PP_Doc*_tmp763=Cyc_PP_line_doc();_tmp3A3[1]=_tmp763;}),({struct Cyc_PP_Doc*_tmp762=Cyc_Absynpp_stmt2doc(_tmp3C2,stmtexp,_tmp3A4);_tmp3A3[2]=_tmp762;});Cyc_PP_cat(_tag_fat(_tmp3A3,sizeof(struct Cyc_PP_Doc*),3U));});
if(_tmp3A5)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}case 13U: _LL1D: _tmp3C5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp3C4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL1E:
# 1512
 if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(_tmp3C4))
s=({struct Cyc_PP_Doc*_tmp3A6[3U];({struct Cyc_PP_Doc*_tmp768=Cyc_PP_textptr(_tmp3C5);_tmp3A6[0]=_tmp768;}),({struct Cyc_PP_Doc*_tmp767=Cyc_PP_text(({const char*_tmp3A7=": ";_tag_fat(_tmp3A7,sizeof(char),3U);}));_tmp3A6[1]=_tmp767;}),({struct Cyc_PP_Doc*_tmp766=({int _tmp765=stmtexp;Cyc_Absynpp_block(_tmp765,Cyc_Absynpp_stmt2doc(_tmp3C4,stmtexp,0));});_tmp3A6[2]=_tmp766;});Cyc_PP_cat(_tag_fat(_tmp3A6,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1515
s=({struct Cyc_PP_Doc*_tmp3A8[3U];({struct Cyc_PP_Doc*_tmp76B=Cyc_PP_textptr(_tmp3C5);_tmp3A8[0]=_tmp76B;}),({struct Cyc_PP_Doc*_tmp76A=Cyc_PP_text(({const char*_tmp3A9=": ";_tag_fat(_tmp3A9,sizeof(char),3U);}));_tmp3A8[1]=_tmp76A;}),({struct Cyc_PP_Doc*_tmp769=Cyc_Absynpp_stmt2doc(_tmp3C4,stmtexp,varsinblock);_tmp3A8[2]=_tmp769;});Cyc_PP_cat(_tag_fat(_tmp3A8,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 14U: _LL1F: _tmp3C7=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp3C6=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2).f1;_LL20:
# 1518
 s=({struct Cyc_PP_Doc*_tmp3AA[5U];({struct Cyc_PP_Doc*_tmp770=Cyc_PP_text(({const char*_tmp3AB="do ";_tag_fat(_tmp3AB,sizeof(char),4U);}));_tmp3AA[0]=_tmp770;}),({
struct Cyc_PP_Doc*_tmp76F=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(_tmp3C7,0,0));_tmp3AA[1]=_tmp76F;}),({
struct Cyc_PP_Doc*_tmp76E=Cyc_PP_text(({const char*_tmp3AC=" while (";_tag_fat(_tmp3AC,sizeof(char),9U);}));_tmp3AA[2]=_tmp76E;}),({
struct Cyc_PP_Doc*_tmp76D=Cyc_Absynpp_exp2doc(_tmp3C6);_tmp3AA[3]=_tmp76D;}),({
struct Cyc_PP_Doc*_tmp76C=Cyc_PP_text(({const char*_tmp3AD=");";_tag_fat(_tmp3AD,sizeof(char),3U);}));_tmp3AA[4]=_tmp76C;});Cyc_PP_cat(_tag_fat(_tmp3AA,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;default: _LL21: _tmp3C9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp37A)->f1;_tmp3C8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp37A)->f2;_LL22:
# 1525
 s=({struct Cyc_PP_Doc*_tmp3AE[4U];({struct Cyc_PP_Doc*_tmp774=Cyc_PP_text(({const char*_tmp3AF="try ";_tag_fat(_tmp3AF,sizeof(char),5U);}));_tmp3AE[0]=_tmp774;}),({
struct Cyc_PP_Doc*_tmp773=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(_tmp3C9,0,0));_tmp3AE[1]=_tmp773;}),({
struct Cyc_PP_Doc*_tmp772=Cyc_PP_text(({const char*_tmp3B0=" catch ";_tag_fat(_tmp3B0,sizeof(char),8U);}));_tmp3AE[2]=_tmp772;}),({
struct Cyc_PP_Doc*_tmp771=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(_tmp3C8));_tmp3AE[3]=_tmp771;});Cyc_PP_cat(_tag_fat(_tmp3AE,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}_LL0:;}
# 1531
return s;}
# 1534
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3CA=p->r;void*_tmp3CB=_tmp3CA;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Datatypefield*_tmp42A;struct Cyc_List_List*_tmp429;int _tmp428;struct Cyc_Absyn_Datatypefield*_tmp427;struct Cyc_Absyn_Enumfield*_tmp426;struct Cyc_Absyn_Enumfield*_tmp425;struct Cyc_List_List*_tmp424;struct Cyc_List_List*_tmp423;int _tmp422;union Cyc_Absyn_AggrInfo _tmp421;struct Cyc_List_List*_tmp420;struct Cyc_List_List*_tmp41F;int _tmp41E;struct _tuple0*_tmp41D;struct Cyc_List_List*_tmp41C;int _tmp41B;struct _tuple0*_tmp41A;struct Cyc_Absyn_Vardecl*_tmp419;struct Cyc_Absyn_Pat*_tmp418;struct Cyc_Absyn_Vardecl*_tmp417;struct Cyc_Absyn_Pat*_tmp416;struct Cyc_List_List*_tmp415;int _tmp414;struct Cyc_Absyn_Tvar*_tmp413;struct Cyc_Absyn_Vardecl*_tmp412;struct Cyc_Absyn_Tvar*_tmp411;struct Cyc_Absyn_Vardecl*_tmp410;struct Cyc_Absyn_Vardecl*_tmp40F;struct Cyc_Absyn_Pat*_tmp40E;struct Cyc_Absyn_Vardecl*_tmp40D;struct _fat_ptr _tmp40C;char _tmp40B;enum Cyc_Absyn_Sign _tmp40A;int _tmp409;switch(*((int*)_tmp3CB)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3CC="_";_tag_fat(_tmp3CC,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp3CD="NULL";_tag_fat(_tmp3CD,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp40A=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp409=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL6:
# 1540
 if((int)_tmp40A != (int)1U)
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D0=({struct Cyc_Int_pa_PrintArg_struct _tmp576;_tmp576.tag=1U,_tmp576.f1=(unsigned long)_tmp409;_tmp576;});void*_tmp3CE[1U];_tmp3CE[0]=& _tmp3D0;({struct _fat_ptr _tmp775=({const char*_tmp3CF="%d";_tag_fat(_tmp3CF,sizeof(char),3U);});Cyc_aprintf(_tmp775,_tag_fat(_tmp3CE,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D3=({struct Cyc_Int_pa_PrintArg_struct _tmp577;_tmp577.tag=1U,_tmp577.f1=(unsigned)_tmp409;_tmp577;});void*_tmp3D1[1U];_tmp3D1[0]=& _tmp3D3;({struct _fat_ptr _tmp776=({const char*_tmp3D2="%u";_tag_fat(_tmp3D2,sizeof(char),3U);});Cyc_aprintf(_tmp776,_tag_fat(_tmp3D1,sizeof(void*),1U));});}));}
goto _LL0;case 11U: _LL7: _tmp40B=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL8:
 s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D6=({struct Cyc_String_pa_PrintArg_struct _tmp578;_tmp578.tag=0U,({struct _fat_ptr _tmp777=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(_tmp40B));_tmp578.f1=_tmp777;});_tmp578;});void*_tmp3D4[1U];_tmp3D4[0]=& _tmp3D6;({struct _fat_ptr _tmp778=({const char*_tmp3D5="'%s'";_tag_fat(_tmp3D5,sizeof(char),5U);});Cyc_aprintf(_tmp778,_tag_fat(_tmp3D4,sizeof(void*),1U));});}));goto _LL0;case 12U: _LL9: _tmp40C=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LLA:
 s=Cyc_PP_text(_tmp40C);goto _LL0;case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2)->r)->tag == 0U){_LLB: _tmp40D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LLC:
# 1547
 s=Cyc_Absynpp_qvar2doc(_tmp40D->name);goto _LL0;}else{_LLD: _tmp40F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp40E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LLE:
# 1549
 s=({struct Cyc_PP_Doc*_tmp3D7[3U];({struct Cyc_PP_Doc*_tmp77B=Cyc_Absynpp_qvar2doc(_tmp40F->name);_tmp3D7[0]=_tmp77B;}),({struct Cyc_PP_Doc*_tmp77A=Cyc_PP_text(({const char*_tmp3D8=" as ";_tag_fat(_tmp3D8,sizeof(char),5U);}));_tmp3D7[1]=_tmp77A;}),({struct Cyc_PP_Doc*_tmp779=Cyc_Absynpp_pat2doc(_tmp40E);_tmp3D7[2]=_tmp779;});Cyc_PP_cat(_tag_fat(_tmp3D7,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 2U: _LLF: _tmp411=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp410=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL10:
# 1551
 s=({struct Cyc_PP_Doc*_tmp3D9[5U];({struct Cyc_PP_Doc*_tmp780=Cyc_PP_text(({const char*_tmp3DA="alias";_tag_fat(_tmp3DA,sizeof(char),6U);}));_tmp3D9[0]=_tmp780;}),({struct Cyc_PP_Doc*_tmp77F=Cyc_PP_text(({const char*_tmp3DB=" <";_tag_fat(_tmp3DB,sizeof(char),3U);}));_tmp3D9[1]=_tmp77F;}),({struct Cyc_PP_Doc*_tmp77E=Cyc_Absynpp_tvar2doc(_tmp411);_tmp3D9[2]=_tmp77E;}),({struct Cyc_PP_Doc*_tmp77D=Cyc_PP_text(({const char*_tmp3DC="> ";_tag_fat(_tmp3DC,sizeof(char),3U);}));_tmp3D9[3]=_tmp77D;}),({
struct Cyc_PP_Doc*_tmp77C=Cyc_Absynpp_vardecl2doc(_tmp410);_tmp3D9[4]=_tmp77C;});Cyc_PP_cat(_tag_fat(_tmp3D9,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 4U: _LL11: _tmp413=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp412=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL12:
# 1555
 s=({struct Cyc_PP_Doc*_tmp3DD[4U];({struct Cyc_PP_Doc*_tmp784=Cyc_Absynpp_qvar2doc(_tmp412->name);_tmp3DD[0]=_tmp784;}),({struct Cyc_PP_Doc*_tmp783=Cyc_PP_text(({const char*_tmp3DE="<";_tag_fat(_tmp3DE,sizeof(char),2U);}));_tmp3DD[1]=_tmp783;}),({struct Cyc_PP_Doc*_tmp782=Cyc_Absynpp_tvar2doc(_tmp413);_tmp3DD[2]=_tmp782;}),({struct Cyc_PP_Doc*_tmp781=Cyc_PP_text(({const char*_tmp3DF=">";_tag_fat(_tmp3DF,sizeof(char),2U);}));_tmp3DD[3]=_tmp781;});Cyc_PP_cat(_tag_fat(_tmp3DD,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 5U: _LL13: _tmp415=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp414=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL14:
# 1558
 s=({struct Cyc_PP_Doc*_tmp3E0[4U];({struct Cyc_PP_Doc*_tmp78A=Cyc_Absynpp_dollar();_tmp3E0[0]=_tmp78A;}),({struct Cyc_PP_Doc*_tmp789=Cyc_PP_text(({const char*_tmp3E1="(";_tag_fat(_tmp3E1,sizeof(char),2U);}));_tmp3E0[1]=_tmp789;}),({struct Cyc_PP_Doc*_tmp788=({struct _fat_ptr _tmp787=({const char*_tmp3E2=",";_tag_fat(_tmp3E2,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp787,_tmp415);});_tmp3E0[2]=_tmp788;}),
_tmp414?({struct Cyc_PP_Doc*_tmp786=Cyc_PP_text(({const char*_tmp3E3=", ...)";_tag_fat(_tmp3E3,sizeof(char),7U);}));_tmp3E0[3]=_tmp786;}):({struct Cyc_PP_Doc*_tmp785=Cyc_PP_text(({const char*_tmp3E4=")";_tag_fat(_tmp3E4,sizeof(char),2U);}));_tmp3E0[3]=_tmp785;});Cyc_PP_cat(_tag_fat(_tmp3E0,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 6U: _LL15: _tmp416=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL16:
# 1562
 s=({struct Cyc_PP_Doc*_tmp3E5[2U];({struct Cyc_PP_Doc*_tmp78C=Cyc_PP_text(({const char*_tmp3E6="&";_tag_fat(_tmp3E6,sizeof(char),2U);}));_tmp3E5[0]=_tmp78C;}),({struct Cyc_PP_Doc*_tmp78B=Cyc_Absynpp_pat2doc(_tmp416);_tmp3E5[1]=_tmp78B;});Cyc_PP_cat(_tag_fat(_tmp3E5,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2)->r)->tag == 0U){_LL17: _tmp417=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL18:
# 1565
 s=({struct Cyc_PP_Doc*_tmp3E7[2U];({struct Cyc_PP_Doc*_tmp78E=Cyc_PP_text(({const char*_tmp3E8="*";_tag_fat(_tmp3E8,sizeof(char),2U);}));_tmp3E7[0]=_tmp78E;}),({struct Cyc_PP_Doc*_tmp78D=Cyc_Absynpp_qvar2doc(_tmp417->name);_tmp3E7[1]=_tmp78D;});Cyc_PP_cat(_tag_fat(_tmp3E7,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}else{_LL19: _tmp419=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp418=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL1A:
# 1568
 s=({struct Cyc_PP_Doc*_tmp3E9[4U];({struct Cyc_PP_Doc*_tmp792=Cyc_PP_text(({const char*_tmp3EA="*";_tag_fat(_tmp3EA,sizeof(char),2U);}));_tmp3E9[0]=_tmp792;}),({struct Cyc_PP_Doc*_tmp791=Cyc_Absynpp_qvar2doc(_tmp419->name);_tmp3E9[1]=_tmp791;}),({struct Cyc_PP_Doc*_tmp790=Cyc_PP_text(({const char*_tmp3EB=" as ";_tag_fat(_tmp3EB,sizeof(char),5U);}));_tmp3E9[2]=_tmp790;}),({struct Cyc_PP_Doc*_tmp78F=Cyc_Absynpp_pat2doc(_tmp418);_tmp3E9[3]=_tmp78F;});Cyc_PP_cat(_tag_fat(_tmp3E9,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 15U: _LL1B: _tmp41A=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL1C:
# 1571
 s=Cyc_Absynpp_qvar2doc(_tmp41A);
goto _LL0;case 16U: _LL1D: _tmp41D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp41C=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_tmp41B=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3;_LL1E: {
# 1574
struct _fat_ptr term=_tmp41B?({const char*_tmp3EF=", ...)";_tag_fat(_tmp3EF,sizeof(char),7U);}):({const char*_tmp3F0=")";_tag_fat(_tmp3F0,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3EC[2U];({struct Cyc_PP_Doc*_tmp797=Cyc_Absynpp_qvar2doc(_tmp41D);_tmp3EC[0]=_tmp797;}),({struct Cyc_PP_Doc*_tmp796=({struct _fat_ptr _tmp795=({const char*_tmp3ED="(";_tag_fat(_tmp3ED,sizeof(char),2U);});struct _fat_ptr _tmp794=term;struct _fat_ptr _tmp793=({const char*_tmp3EE=",";_tag_fat(_tmp3EE,sizeof(char),2U);});Cyc_PP_group(_tmp795,_tmp794,_tmp793,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp41C));});_tmp3EC[1]=_tmp796;});Cyc_PP_cat(_tag_fat(_tmp3EC,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1 != 0){_LL1F: _tmp421=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_tmp420=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_tmp41F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3;_tmp41E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f4;_LL20: {
# 1578
struct _fat_ptr term=_tmp41E?({const char*_tmp3FA=", ...}";_tag_fat(_tmp3FA,sizeof(char),7U);}):({const char*_tmp3FB="}";_tag_fat(_tmp3FB,sizeof(char),2U);});
struct _tuple11 _tmp3F1=Cyc_Absyn_aggr_kinded_name(_tmp421);struct _tuple11 _tmp3F2=_tmp3F1;struct _tuple0*_tmp3F9;_LL2E: _tmp3F9=_tmp3F2.f2;_LL2F:;
s=({struct Cyc_PP_Doc*_tmp3F3[4U];({struct Cyc_PP_Doc*_tmp7A1=Cyc_Absynpp_qvar2doc(_tmp3F9);_tmp3F3[0]=_tmp7A1;}),({struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_lb();_tmp3F3[1]=_tmp7A0;}),({
struct Cyc_PP_Doc*_tmp79F=({struct _fat_ptr _tmp79E=({const char*_tmp3F4="[";_tag_fat(_tmp3F4,sizeof(char),2U);});struct _fat_ptr _tmp79D=({const char*_tmp3F5="]";_tag_fat(_tmp3F5,sizeof(char),2U);});struct _fat_ptr _tmp79C=({const char*_tmp3F6=",";_tag_fat(_tmp3F6,sizeof(char),2U);});Cyc_PP_egroup(_tmp79E,_tmp79D,_tmp79C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp420));});_tmp3F3[2]=_tmp79F;}),({
struct Cyc_PP_Doc*_tmp79B=({struct _fat_ptr _tmp79A=({const char*_tmp3F7="";_tag_fat(_tmp3F7,sizeof(char),1U);});struct _fat_ptr _tmp799=term;struct _fat_ptr _tmp798=({const char*_tmp3F8=",";_tag_fat(_tmp3F8,sizeof(char),2U);});Cyc_PP_group(_tmp79A,_tmp799,_tmp798,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp41F));});_tmp3F3[3]=_tmp79B;});Cyc_PP_cat(_tag_fat(_tmp3F3,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}else{_LL21: _tmp424=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_tmp423=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3;_tmp422=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CB)->f4;_LL22: {
# 1585
struct _fat_ptr term=_tmp422?({const char*_tmp402=", ...}";_tag_fat(_tmp402,sizeof(char),7U);}):({const char*_tmp403="}";_tag_fat(_tmp403,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3FC[3U];({struct Cyc_PP_Doc*_tmp7AA=Cyc_Absynpp_lb();_tmp3FC[0]=_tmp7AA;}),({
struct Cyc_PP_Doc*_tmp7A9=({struct _fat_ptr _tmp7A8=({const char*_tmp3FD="[";_tag_fat(_tmp3FD,sizeof(char),2U);});struct _fat_ptr _tmp7A7=({const char*_tmp3FE="]";_tag_fat(_tmp3FE,sizeof(char),2U);});struct _fat_ptr _tmp7A6=({const char*_tmp3FF=",";_tag_fat(_tmp3FF,sizeof(char),2U);});Cyc_PP_egroup(_tmp7A8,_tmp7A7,_tmp7A6,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp424));});_tmp3FC[1]=_tmp7A9;}),({
struct Cyc_PP_Doc*_tmp7A5=({struct _fat_ptr _tmp7A4=({const char*_tmp400="";_tag_fat(_tmp400,sizeof(char),1U);});struct _fat_ptr _tmp7A3=term;struct _fat_ptr _tmp7A2=({const char*_tmp401=",";_tag_fat(_tmp401,sizeof(char),2U);});Cyc_PP_group(_tmp7A4,_tmp7A3,_tmp7A2,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp423));});_tmp3FC[2]=_tmp7A5;});Cyc_PP_cat(_tag_fat(_tmp3FC,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp425=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL24:
 _tmp426=_tmp425;goto _LL26;case 14U: _LL25: _tmp426=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL26:
 s=Cyc_Absynpp_qvar2doc(_tmp426->name);goto _LL0;case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3 == 0){_LL27: _tmp427=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_LL28:
 s=Cyc_Absynpp_qvar2doc(_tmp427->name);goto _LL0;}else{_LL29: _tmp42A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f2;_tmp429=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f3;_tmp428=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CB)->f4;_LL2A: {
# 1594
struct _fat_ptr term=_tmp428?({const char*_tmp407=", ...)";_tag_fat(_tmp407,sizeof(char),7U);}):({const char*_tmp408=")";_tag_fat(_tmp408,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp404[2U];({struct Cyc_PP_Doc*_tmp7AF=Cyc_Absynpp_qvar2doc(_tmp42A->name);_tmp404[0]=_tmp7AF;}),({struct Cyc_PP_Doc*_tmp7AE=({struct _fat_ptr _tmp7AD=({const char*_tmp405="(";_tag_fat(_tmp405,sizeof(char),2U);});struct _fat_ptr _tmp7AC=term;struct _fat_ptr _tmp7AB=({const char*_tmp406=",";_tag_fat(_tmp406,sizeof(char),2U);});Cyc_PP_egroup(_tmp7AD,_tmp7AC,_tmp7AB,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp429));});_tmp404[1]=_tmp7AE;});Cyc_PP_cat(_tag_fat(_tmp404,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp42B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3CB)->f1;_LL2C:
# 1598
 s=Cyc_Absynpp_exp2doc(_tmp42B);goto _LL0;}_LL0:;}
# 1600
return s;}
# 1603
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp){
return({struct Cyc_PP_Doc*_tmp42C[2U];({struct Cyc_PP_Doc*_tmp7B4=({struct _fat_ptr _tmp7B3=({const char*_tmp42D="";_tag_fat(_tmp42D,sizeof(char),1U);});struct _fat_ptr _tmp7B2=({const char*_tmp42E="=";_tag_fat(_tmp42E,sizeof(char),2U);});struct _fat_ptr _tmp7B1=({const char*_tmp42F="=";_tag_fat(_tmp42F,sizeof(char),2U);});Cyc_PP_egroup(_tmp7B3,_tmp7B2,_tmp7B1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp42C[0]=_tmp7B4;}),({
struct Cyc_PP_Doc*_tmp7B0=Cyc_Absynpp_pat2doc((*dp).f2);_tmp42C[1]=_tmp7B0;});Cyc_PP_cat(_tag_fat(_tmp42C,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1608
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1610
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp430[2U];({struct Cyc_PP_Doc*_tmp7B7=Cyc_PP_text(({const char*_tmp431="default: ";_tag_fat(_tmp431,sizeof(char),10U);}));_tmp430[0]=_tmp7B7;}),({
struct Cyc_PP_Doc*_tmp7B6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp432[2U];({struct Cyc_PP_Doc*_tmp7B5=Cyc_PP_line_doc();_tmp432[0]=_tmp7B5;}),_tmp432[1]=body;Cyc_PP_cat(_tag_fat(_tmp432,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp430[1]=_tmp7B6;});Cyc_PP_cat(_tag_fat(_tmp430,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp433[4U];({struct Cyc_PP_Doc*_tmp7BC=Cyc_PP_text(({const char*_tmp434="case ";_tag_fat(_tmp434,sizeof(char),6U);}));_tmp433[0]=_tmp7BC;}),({
struct Cyc_PP_Doc*_tmp7BB=Cyc_Absynpp_pat2doc(c->pattern);_tmp433[1]=_tmp7BB;}),({
struct Cyc_PP_Doc*_tmp7BA=Cyc_PP_text(({const char*_tmp435=": ";_tag_fat(_tmp435,sizeof(char),3U);}));_tmp433[2]=_tmp7BA;}),({
struct Cyc_PP_Doc*_tmp7B9=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp436[2U];({struct Cyc_PP_Doc*_tmp7B8=Cyc_PP_line_doc();_tmp436[0]=_tmp7B8;}),_tmp436[1]=body;Cyc_PP_cat(_tag_fat(_tmp436,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp433[3]=_tmp7B9;});Cyc_PP_cat(_tag_fat(_tmp433,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1622
return({struct Cyc_PP_Doc*_tmp437[6U];({struct Cyc_PP_Doc*_tmp7C3=Cyc_PP_text(({const char*_tmp438="case ";_tag_fat(_tmp438,sizeof(char),6U);}));_tmp437[0]=_tmp7C3;}),({
struct Cyc_PP_Doc*_tmp7C2=Cyc_Absynpp_pat2doc(c->pattern);_tmp437[1]=_tmp7C2;}),({
struct Cyc_PP_Doc*_tmp7C1=Cyc_PP_text(({const char*_tmp439=" && ";_tag_fat(_tmp439,sizeof(char),5U);}));_tmp437[2]=_tmp7C1;}),({
struct Cyc_PP_Doc*_tmp7C0=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp437[3]=_tmp7C0;}),({
struct Cyc_PP_Doc*_tmp7BF=Cyc_PP_text(({const char*_tmp43A=": ";_tag_fat(_tmp43A,sizeof(char),3U);}));_tmp437[4]=_tmp7BF;}),({
struct Cyc_PP_Doc*_tmp7BE=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43B[2U];({struct Cyc_PP_Doc*_tmp7BD=Cyc_PP_line_doc();_tmp43B[0]=_tmp7BD;}),_tmp43B[1]=body;Cyc_PP_cat(_tag_fat(_tmp43B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp437[5]=_tmp7BE;});Cyc_PP_cat(_tag_fat(_tmp437,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1630
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _fat_ptr _tmp7C4=({const char*_tmp43C="";_tag_fat(_tmp43C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp7C4,cs);});}
# 1634
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1638
return({struct Cyc_PP_Doc*_tmp43D[3U];({struct Cyc_PP_Doc*_tmp7C7=Cyc_Absynpp_qvar2doc(f->name);_tmp43D[0]=_tmp7C7;}),({struct Cyc_PP_Doc*_tmp7C6=Cyc_PP_text(({const char*_tmp43E=" = ";_tag_fat(_tmp43E,sizeof(char),4U);}));_tmp43D[1]=_tmp7C6;}),({struct Cyc_PP_Doc*_tmp7C5=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp43D[2]=_tmp7C5;});Cyc_PP_cat(_tag_fat(_tmp43D,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1641
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _fat_ptr _tmp7C8=({const char*_tmp43F=",";_tag_fat(_tmp43F,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp7C8,fs);});}
# 1645
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1649
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _fat_ptr _tmp7C9=({const char*_tmp440=",";_tag_fat(_tmp440,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp7C9,vds);});}
# 1653
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp441=vd;enum Cyc_Absyn_Scope _tmp453;struct _tuple0*_tmp452;unsigned _tmp451;struct Cyc_Absyn_Tqual _tmp450;void*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_List_List*_tmp44D;_LL1: _tmp453=_tmp441->sc;_tmp452=_tmp441->name;_tmp451=_tmp441->varloc;_tmp450=_tmp441->tq;_tmp44F=_tmp441->type;_tmp44E=_tmp441->initializer;_tmp44D=_tmp441->attributes;_LL2:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp452);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp44D);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp442=Cyc_Cyclone_c_compiler;if(_tmp442 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1662
{void*_tmp443=Cyc_Tcutil_compress(_tmp44F);void*_tmp444=_tmp443;struct Cyc_List_List*_tmp445;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp444)->tag == 5U){_LL9: _tmp445=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp444)->f1).attributes;_LLA:
# 1664
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp445);
goto _LL8;}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1668
goto _LL3;}_LL3:;}{
# 1671
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp446=Cyc_Cyclone_c_compiler;if(_tmp446 == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1676
s=({struct Cyc_PP_Doc*_tmp447[5U];_tmp447[0]=tmp_doc,({
# 1678
struct Cyc_PP_Doc*_tmp7D3=Cyc_Absynpp_scope2doc(_tmp453);_tmp447[1]=_tmp7D3;}),({
struct Cyc_PP_Doc*_tmp7D2=({struct Cyc_Absyn_Tqual _tmp7D1=_tmp450;void*_tmp7D0=_tmp44F;Cyc_Absynpp_tqtd2doc(_tmp7D1,_tmp7D0,({struct Cyc_Core_Opt*_tmp449=_cycalloc(sizeof(*_tmp449));({struct Cyc_PP_Doc*_tmp7CF=({struct Cyc_PP_Doc*_tmp448[2U];_tmp448[0]=beforenamedoc,_tmp448[1]=sn;Cyc_PP_cat(_tag_fat(_tmp448,sizeof(struct Cyc_PP_Doc*),2U));});_tmp449->v=_tmp7CF;});_tmp449;}));});_tmp447[2]=_tmp7D2;}),
_tmp44E == 0?({
struct Cyc_PP_Doc*_tmp7CE=Cyc_PP_nil_doc();_tmp447[3]=_tmp7CE;}):({
struct Cyc_PP_Doc*_tmp7CD=({struct Cyc_PP_Doc*_tmp44A[2U];({struct Cyc_PP_Doc*_tmp7CC=Cyc_PP_text(({const char*_tmp44B=" = ";_tag_fat(_tmp44B,sizeof(char),4U);}));_tmp44A[0]=_tmp7CC;}),({struct Cyc_PP_Doc*_tmp7CB=Cyc_Absynpp_exp2doc(_tmp44E);_tmp44A[1]=_tmp7CB;});Cyc_PP_cat(_tag_fat(_tmp44A,sizeof(struct Cyc_PP_Doc*),2U));});_tmp447[3]=_tmp7CD;}),({
struct Cyc_PP_Doc*_tmp7CA=Cyc_PP_text(({const char*_tmp44C=";";_tag_fat(_tmp44C,sizeof(char),2U);}));_tmp447[4]=_tmp7CA;});Cyc_PP_cat(_tag_fat(_tmp447,sizeof(struct Cyc_PP_Doc*),5U));});
return s;};};}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1687
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
struct _tuple20 _tmp454=*x;struct _tuple20 _tmp455=_tmp454;struct _tuple0*_tmp456;_LL1: _tmp456=_tmp455.f2;_LL2:;
return Cyc_Absynpp_qvar2doc(_tmp456);}
# 1692
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp457[4U];({struct Cyc_PP_Doc*_tmp7D7=Cyc_Absynpp_scope2doc(ad->sc);_tmp457[0]=_tmp7D7;}),({
struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp457[1]=_tmp7D6;}),({
struct Cyc_PP_Doc*_tmp7D5=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp457[2]=_tmp7D5;}),({
struct Cyc_PP_Doc*_tmp7D4=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp457[3]=_tmp7D4;});Cyc_PP_cat(_tag_fat(_tmp457,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1699
return({struct Cyc_PP_Doc*_tmp458[12U];({struct Cyc_PP_Doc*_tmp7E8=Cyc_Absynpp_scope2doc(ad->sc);_tmp458[0]=_tmp7E8;}),({
struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp458[1]=_tmp7E7;}),({
struct Cyc_PP_Doc*_tmp7E6=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp458[2]=_tmp7E6;}),({
struct Cyc_PP_Doc*_tmp7E5=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp458[3]=_tmp7E5;}),({
struct Cyc_PP_Doc*_tmp7E4=Cyc_PP_blank_doc();_tmp458[4]=_tmp7E4;}),({struct Cyc_PP_Doc*_tmp7E3=Cyc_Absynpp_lb();_tmp458[5]=_tmp7E3;}),({
struct Cyc_PP_Doc*_tmp7E2=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp458[6]=_tmp7E2;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_nil_doc();_tmp458[7]=_tmp7E1;}):({
struct Cyc_PP_Doc*_tmp7E0=({struct Cyc_PP_Doc*_tmp459[2U];({struct Cyc_PP_Doc*_tmp7DF=Cyc_PP_text(({const char*_tmp45A=":";_tag_fat(_tmp45A,sizeof(char),2U);}));_tmp459[0]=_tmp7DF;}),({struct Cyc_PP_Doc*_tmp7DE=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp459[1]=_tmp7DE;});Cyc_PP_cat(_tag_fat(_tmp459,sizeof(struct Cyc_PP_Doc*),2U));});_tmp458[7]=_tmp7E0;}),({
struct Cyc_PP_Doc*_tmp7DD=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp45B[2U];({struct Cyc_PP_Doc*_tmp7DC=Cyc_PP_line_doc();_tmp45B[0]=_tmp7DC;}),({struct Cyc_PP_Doc*_tmp7DB=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp45B[1]=_tmp7DB;});Cyc_PP_cat(_tag_fat(_tmp45B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp458[8]=_tmp7DD;}),({
struct Cyc_PP_Doc*_tmp7DA=Cyc_PP_line_doc();_tmp458[9]=_tmp7DA;}),({
struct Cyc_PP_Doc*_tmp7D9=Cyc_Absynpp_rb();_tmp458[10]=_tmp7D9;}),({
struct Cyc_PP_Doc*_tmp7D8=Cyc_Absynpp_atts2doc(ad->attributes);_tmp458[11]=_tmp7D8;});Cyc_PP_cat(_tag_fat(_tmp458,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1713
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp45C=dd;enum Cyc_Absyn_Scope _tmp468;struct _tuple0*_tmp467;struct Cyc_List_List*_tmp466;struct Cyc_Core_Opt*_tmp465;int _tmp464;_LL1: _tmp468=_tmp45C->sc;_tmp467=_tmp45C->name;_tmp466=_tmp45C->tvs;_tmp465=_tmp45C->fields;_tmp464=_tmp45C->is_extensible;_LL2:;
if(_tmp465 == 0)
return({struct Cyc_PP_Doc*_tmp45D[5U];({struct Cyc_PP_Doc*_tmp7EF=Cyc_Absynpp_scope2doc(_tmp468);_tmp45D[0]=_tmp7EF;}),
_tmp464?({struct Cyc_PP_Doc*_tmp7EE=Cyc_PP_text(({const char*_tmp45E="@extensible ";_tag_fat(_tmp45E,sizeof(char),13U);}));_tmp45D[1]=_tmp7EE;}):({struct Cyc_PP_Doc*_tmp7ED=Cyc_PP_blank_doc();_tmp45D[1]=_tmp7ED;}),({
struct Cyc_PP_Doc*_tmp7EC=Cyc_PP_text(({const char*_tmp45F="datatype ";_tag_fat(_tmp45F,sizeof(char),10U);}));_tmp45D[2]=_tmp7EC;}),
_tmp464?({struct Cyc_PP_Doc*_tmp7EB=Cyc_Absynpp_qvar2bolddoc(_tmp467);_tmp45D[3]=_tmp7EB;}):({struct Cyc_PP_Doc*_tmp7EA=Cyc_Absynpp_typedef_name2bolddoc(_tmp467);_tmp45D[3]=_tmp7EA;}),({
struct Cyc_PP_Doc*_tmp7E9=Cyc_Absynpp_ktvars2doc(_tmp466);_tmp45D[4]=_tmp7E9;});Cyc_PP_cat(_tag_fat(_tmp45D,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1722
return({struct Cyc_PP_Doc*_tmp460[10U];({struct Cyc_PP_Doc*_tmp7FD=Cyc_Absynpp_scope2doc(_tmp468);_tmp460[0]=_tmp7FD;}),
_tmp464?({struct Cyc_PP_Doc*_tmp7FC=Cyc_PP_text(({const char*_tmp461="@extensible ";_tag_fat(_tmp461,sizeof(char),13U);}));_tmp460[1]=_tmp7FC;}):({struct Cyc_PP_Doc*_tmp7FB=Cyc_PP_blank_doc();_tmp460[1]=_tmp7FB;}),({
struct Cyc_PP_Doc*_tmp7FA=Cyc_PP_text(({const char*_tmp462="datatype ";_tag_fat(_tmp462,sizeof(char),10U);}));_tmp460[2]=_tmp7FA;}),
_tmp464?({struct Cyc_PP_Doc*_tmp7F9=Cyc_Absynpp_qvar2bolddoc(_tmp467);_tmp460[3]=_tmp7F9;}):({struct Cyc_PP_Doc*_tmp7F8=Cyc_Absynpp_typedef_name2bolddoc(_tmp467);_tmp460[3]=_tmp7F8;}),({
struct Cyc_PP_Doc*_tmp7F7=Cyc_Absynpp_ktvars2doc(_tmp466);_tmp460[4]=_tmp7F7;}),({
struct Cyc_PP_Doc*_tmp7F6=Cyc_PP_blank_doc();_tmp460[5]=_tmp7F6;}),({struct Cyc_PP_Doc*_tmp7F5=Cyc_Absynpp_lb();_tmp460[6]=_tmp7F5;}),({
struct Cyc_PP_Doc*_tmp7F4=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp463[2U];({struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_line_doc();_tmp463[0]=_tmp7F3;}),({struct Cyc_PP_Doc*_tmp7F2=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp465->v);_tmp463[1]=_tmp7F2;});Cyc_PP_cat(_tag_fat(_tmp463,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp460[7]=_tmp7F4;}),({
struct Cyc_PP_Doc*_tmp7F1=Cyc_PP_line_doc();_tmp460[8]=_tmp7F1;}),({
struct Cyc_PP_Doc*_tmp7F0=Cyc_Absynpp_rb();_tmp460[9]=_tmp7F0;});Cyc_PP_cat(_tag_fat(_tmp460,sizeof(struct Cyc_PP_Doc*),10U));});}}
# 1733
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp469=ed;enum Cyc_Absyn_Scope _tmp471;struct _tuple0*_tmp470;struct Cyc_Core_Opt*_tmp46F;_LL1: _tmp471=_tmp469->sc;_tmp470=_tmp469->name;_tmp46F=_tmp469->fields;_LL2:;
if(_tmp46F == 0)
return({struct Cyc_PP_Doc*_tmp46A[3U];({struct Cyc_PP_Doc*_tmp800=Cyc_Absynpp_scope2doc(_tmp471);_tmp46A[0]=_tmp800;}),({
struct Cyc_PP_Doc*_tmp7FF=Cyc_PP_text(({const char*_tmp46B="enum ";_tag_fat(_tmp46B,sizeof(char),6U);}));_tmp46A[1]=_tmp7FF;}),({
struct Cyc_PP_Doc*_tmp7FE=Cyc_Absynpp_typedef_name2bolddoc(_tmp470);_tmp46A[2]=_tmp7FE;});Cyc_PP_cat(_tag_fat(_tmp46A,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1741
return({struct Cyc_PP_Doc*_tmp46C[8U];({struct Cyc_PP_Doc*_tmp80A=Cyc_Absynpp_scope2doc(_tmp471);_tmp46C[0]=_tmp80A;}),({
struct Cyc_PP_Doc*_tmp809=Cyc_PP_text(({const char*_tmp46D="enum ";_tag_fat(_tmp46D,sizeof(char),6U);}));_tmp46C[1]=_tmp809;}),({
struct Cyc_PP_Doc*_tmp808=Cyc_Absynpp_qvar2bolddoc(_tmp470);_tmp46C[2]=_tmp808;}),({
struct Cyc_PP_Doc*_tmp807=Cyc_PP_blank_doc();_tmp46C[3]=_tmp807;}),({struct Cyc_PP_Doc*_tmp806=Cyc_Absynpp_lb();_tmp46C[4]=_tmp806;}),({
struct Cyc_PP_Doc*_tmp805=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp46E[2U];({struct Cyc_PP_Doc*_tmp804=Cyc_PP_line_doc();_tmp46E[0]=_tmp804;}),({struct Cyc_PP_Doc*_tmp803=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp46F->v);_tmp46E[1]=_tmp803;});Cyc_PP_cat(_tag_fat(_tmp46E,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp46C[5]=_tmp805;}),({
struct Cyc_PP_Doc*_tmp802=Cyc_PP_line_doc();_tmp46C[6]=_tmp802;}),({
struct Cyc_PP_Doc*_tmp801=Cyc_Absynpp_rb();_tmp46C[7]=_tmp801;});Cyc_PP_cat(_tag_fat(_tmp46C,sizeof(struct Cyc_PP_Doc*),8U));});}}
# 1750
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp472=d->r;void*_tmp473=_tmp472;struct Cyc_List_List*_tmp4DF;struct Cyc_List_List*_tmp4DE;struct Cyc_List_List*_tmp4DD;struct _tuple10*_tmp4DC;struct Cyc_List_List*_tmp4DB;struct _tuple0*_tmp4DA;struct Cyc_List_List*_tmp4D9;struct _fat_ptr*_tmp4D8;struct Cyc_List_List*_tmp4D7;struct Cyc_Absyn_Typedefdecl*_tmp4D6;struct Cyc_Absyn_Enumdecl*_tmp4D5;struct Cyc_List_List*_tmp4D4;struct Cyc_Absyn_Pat*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_Absyn_Datatypedecl*_tmp4D1;struct Cyc_Absyn_Tvar*_tmp4D0;struct Cyc_Absyn_Vardecl*_tmp4CF;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Vardecl*_tmp4CD;struct Cyc_Absyn_Aggrdecl*_tmp4CC;struct Cyc_Absyn_Fndecl*_tmp4CB;switch(*((int*)_tmp473)){case 1U: _LL1: _tmp4CB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL2: {
# 1754
struct Cyc_Absyn_FnInfo type_info=_tmp4CB->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->tag=5U,_tmp484->f1=type_info;_tmp484;});
if(_tmp4CB->cached_type != 0){
void*_tmp474=Cyc_Tcutil_compress((void*)_check_null(_tmp4CB->cached_type));void*_tmp475=_tmp474;struct Cyc_Absyn_FnInfo _tmp478;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp475)->tag == 5U){_LL24: _tmp478=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp475)->f1;_LL25:
# 1760
({struct Cyc_List_List*_tmp80B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((_tmp4CB->i).attributes,_tmp478.attributes);(_tmp4CB->i).attributes=_tmp80B;});goto _LL23;}else{_LL26: _LL27:
({void*_tmp476=0U;({struct _fat_ptr _tmp80C=({const char*_tmp477="function has non-function type";_tag_fat(_tmp477,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp80C,_tag_fat(_tmp476,sizeof(void*),0U));});});}_LL23:;}{
# 1763
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((_tmp4CB->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4CB->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp479=Cyc_Cyclone_c_compiler;if(_tmp479 == Cyc_Cyclone_Gcc_c){_LL29: _LL2A:
 inlinedoc=Cyc_PP_text(({const char*_tmp47A="inline ";_tag_fat(_tmp47A,sizeof(char),8U);}));goto _LL28;}else{_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp47B="__inline ";_tag_fat(_tmp47B,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1771
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4CB->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp47C=Cyc_Cyclone_c_compiler;if(_tmp47C == Cyc_Cyclone_Gcc_c){_LL2E: _LL2F:
 beforenamedoc=attsdoc;goto _LL2D;}else{_LL30: _LL31:
 beforenamedoc=Cyc_Absynpp_callconv2doc((_tmp4CB->i).attributes);goto _LL2D;}_LL2D:;}{
# 1778
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4CB->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp80F=Cyc_Absyn_empty_tqual(0U);void*_tmp80E=t;Cyc_Absynpp_tqtd2doc(_tmp80F,_tmp80E,({struct Cyc_Core_Opt*_tmp483=_cycalloc(sizeof(*_tmp483));({
struct Cyc_PP_Doc*_tmp80D=({struct Cyc_PP_Doc*_tmp482[2U];_tmp482[0]=beforenamedoc,_tmp482[1]=namedoc;Cyc_PP_cat(_tag_fat(_tmp482,sizeof(struct Cyc_PP_Doc*),2U));});_tmp483->v=_tmp80D;});_tmp483;}));});
# 1786
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp480[5U];({struct Cyc_PP_Doc*_tmp816=Cyc_PP_blank_doc();_tmp480[0]=_tmp816;}),({struct Cyc_PP_Doc*_tmp815=Cyc_Absynpp_lb();_tmp480[1]=_tmp815;}),({
struct Cyc_PP_Doc*_tmp814=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp481[2U];({struct Cyc_PP_Doc*_tmp813=Cyc_PP_line_doc();_tmp481[0]=_tmp813;}),({struct Cyc_PP_Doc*_tmp812=Cyc_Absynpp_stmt2doc(_tmp4CB->body,0,0);_tmp481[1]=_tmp812;});Cyc_PP_cat(_tag_fat(_tmp481,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp480[2]=_tmp814;}),({
struct Cyc_PP_Doc*_tmp811=Cyc_PP_line_doc();_tmp480[3]=_tmp811;}),({
struct Cyc_PP_Doc*_tmp810=Cyc_Absynpp_rb();_tmp480[4]=_tmp810;});Cyc_PP_cat(_tag_fat(_tmp480,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp47D[4U];_tmp47D[0]=inlinedoc,_tmp47D[1]=scopedoc,_tmp47D[2]=tqtddoc,_tmp47D[3]=bodydoc;Cyc_PP_cat(_tag_fat(_tmp47D,sizeof(struct Cyc_PP_Doc*),4U));});
# 1792
{enum Cyc_Cyclone_C_Compilers _tmp47E=Cyc_Cyclone_c_compiler;if(_tmp47E == Cyc_Cyclone_Vc_c){_LL33: _LL34:
 s=({struct Cyc_PP_Doc*_tmp47F[2U];_tmp47F[0]=attsdoc,_tmp47F[1]=s;Cyc_PP_cat(_tag_fat(_tmp47F,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 1797
goto _LL0;};};};};}case 5U: _LL3: _tmp4CC=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL4:
# 1800
 s=({struct Cyc_PP_Doc*_tmp485[2U];({struct Cyc_PP_Doc*_tmp818=Cyc_Absynpp_aggrdecl2doc(_tmp4CC);_tmp485[0]=_tmp818;}),({struct Cyc_PP_Doc*_tmp817=Cyc_PP_text(({const char*_tmp486=";";_tag_fat(_tmp486,sizeof(char),2U);}));_tmp485[1]=_tmp817;});Cyc_PP_cat(_tag_fat(_tmp485,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 0U: _LL5: _tmp4CD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL6:
# 1803
 s=Cyc_Absynpp_vardecl2doc(_tmp4CD);
goto _LL0;case 4U: _LL7: _tmp4D0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp4CF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp473)->f2;_tmp4CE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp473)->f3;_LL8:
# 1806
 s=({struct Cyc_PP_Doc*_tmp487[7U];({struct Cyc_PP_Doc*_tmp823=Cyc_PP_text(({const char*_tmp488="region";_tag_fat(_tmp488,sizeof(char),7U);}));_tmp487[0]=_tmp823;}),({
struct Cyc_PP_Doc*_tmp822=Cyc_PP_text(({const char*_tmp489="<";_tag_fat(_tmp489,sizeof(char),2U);}));_tmp487[1]=_tmp822;}),({
struct Cyc_PP_Doc*_tmp821=Cyc_Absynpp_tvar2doc(_tmp4D0);_tmp487[2]=_tmp821;}),({
struct Cyc_PP_Doc*_tmp820=Cyc_PP_text(({const char*_tmp48A=">";_tag_fat(_tmp48A,sizeof(char),2U);}));_tmp487[3]=_tmp820;}),({
struct Cyc_PP_Doc*_tmp81F=Cyc_Absynpp_qvar2doc(_tmp4CF->name);_tmp487[4]=_tmp81F;}),
(unsigned)_tmp4CE?({struct Cyc_PP_Doc*_tmp81E=({struct Cyc_PP_Doc*_tmp48B[3U];({struct Cyc_PP_Doc*_tmp81D=Cyc_PP_text(({const char*_tmp48C=" = open(";_tag_fat(_tmp48C,sizeof(char),9U);}));_tmp48B[0]=_tmp81D;}),({struct Cyc_PP_Doc*_tmp81C=Cyc_Absynpp_exp2doc(_tmp4CE);_tmp48B[1]=_tmp81C;}),({
struct Cyc_PP_Doc*_tmp81B=Cyc_PP_text(({const char*_tmp48D=")";_tag_fat(_tmp48D,sizeof(char),2U);}));_tmp48B[2]=_tmp81B;});Cyc_PP_cat(_tag_fat(_tmp48B,sizeof(struct Cyc_PP_Doc*),3U));});
# 1811
_tmp487[5]=_tmp81E;}):({
struct Cyc_PP_Doc*_tmp81A=Cyc_PP_nil_doc();_tmp487[5]=_tmp81A;}),({
struct Cyc_PP_Doc*_tmp819=Cyc_PP_text(({const char*_tmp48E=";";_tag_fat(_tmp48E,sizeof(char),2U);}));_tmp487[6]=_tmp819;});Cyc_PP_cat(_tag_fat(_tmp487,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 6U: _LL9: _tmp4D1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LLA:
# 1816
 s=({struct Cyc_PP_Doc*_tmp48F[2U];({struct Cyc_PP_Doc*_tmp825=Cyc_Absynpp_datatypedecl2doc(_tmp4D1);_tmp48F[0]=_tmp825;}),({struct Cyc_PP_Doc*_tmp824=Cyc_PP_text(({const char*_tmp490=";";_tag_fat(_tmp490,sizeof(char),2U);}));_tmp48F[1]=_tmp824;});Cyc_PP_cat(_tag_fat(_tmp48F,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 2U: _LLB: _tmp4D3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp4D2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp473)->f3;_LLC:
# 1819
 s=({struct Cyc_PP_Doc*_tmp491[5U];({struct Cyc_PP_Doc*_tmp82A=Cyc_PP_text(({const char*_tmp492="let ";_tag_fat(_tmp492,sizeof(char),5U);}));_tmp491[0]=_tmp82A;}),({
struct Cyc_PP_Doc*_tmp829=Cyc_Absynpp_pat2doc(_tmp4D3);_tmp491[1]=_tmp829;}),({
struct Cyc_PP_Doc*_tmp828=Cyc_PP_text(({const char*_tmp493=" = ";_tag_fat(_tmp493,sizeof(char),4U);}));_tmp491[2]=_tmp828;}),({
struct Cyc_PP_Doc*_tmp827=Cyc_Absynpp_exp2doc(_tmp4D2);_tmp491[3]=_tmp827;}),({
struct Cyc_PP_Doc*_tmp826=Cyc_PP_text(({const char*_tmp494=";";_tag_fat(_tmp494,sizeof(char),2U);}));_tmp491[4]=_tmp826;});Cyc_PP_cat(_tag_fat(_tmp491,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 3U: _LLD: _tmp4D4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LLE:
# 1826
 s=({struct Cyc_PP_Doc*_tmp495[3U];({struct Cyc_PP_Doc*_tmp82D=Cyc_PP_text(({const char*_tmp496="let ";_tag_fat(_tmp496,sizeof(char),5U);}));_tmp495[0]=_tmp82D;}),({struct Cyc_PP_Doc*_tmp82C=Cyc_Absynpp_ids2doc(_tmp4D4);_tmp495[1]=_tmp82C;}),({struct Cyc_PP_Doc*_tmp82B=Cyc_PP_text(({const char*_tmp497=";";_tag_fat(_tmp497,sizeof(char),2U);}));_tmp495[2]=_tmp82B;});Cyc_PP_cat(_tag_fat(_tmp495,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 7U: _LLF: _tmp4D5=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL10:
# 1829
 s=({struct Cyc_PP_Doc*_tmp498[2U];({struct Cyc_PP_Doc*_tmp82F=Cyc_Absynpp_enumdecl2doc(_tmp4D5);_tmp498[0]=_tmp82F;}),({struct Cyc_PP_Doc*_tmp82E=Cyc_PP_text(({const char*_tmp499=";";_tag_fat(_tmp499,sizeof(char),2U);}));_tmp498[1]=_tmp82E;});Cyc_PP_cat(_tag_fat(_tmp498,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 8U: _LL11: _tmp4D6=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL12: {
# 1832
void*t;
if(_tmp4D6->defn != 0)
t=(void*)_check_null(_tmp4D6->defn);else{
# 1836
t=Cyc_Absyn_new_evar(_tmp4D6->kind,0);}
s=({struct Cyc_PP_Doc*_tmp49A[4U];({struct Cyc_PP_Doc*_tmp838=Cyc_PP_text(({const char*_tmp49B="typedef ";_tag_fat(_tmp49B,sizeof(char),9U);}));_tmp49A[0]=_tmp838;}),({
struct Cyc_PP_Doc*_tmp837=({struct Cyc_Absyn_Tqual _tmp836=_tmp4D6->tq;void*_tmp835=t;Cyc_Absynpp_tqtd2doc(_tmp836,_tmp835,({struct Cyc_Core_Opt*_tmp49D=_cycalloc(sizeof(*_tmp49D));({
# 1840
struct Cyc_PP_Doc*_tmp834=({struct Cyc_PP_Doc*_tmp49C[2U];({struct Cyc_PP_Doc*_tmp833=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D6->name);_tmp49C[0]=_tmp833;}),({
struct Cyc_PP_Doc*_tmp832=Cyc_Absynpp_tvars2doc(_tmp4D6->tvs);_tmp49C[1]=_tmp832;});Cyc_PP_cat(_tag_fat(_tmp49C,sizeof(struct Cyc_PP_Doc*),2U));});
# 1840
_tmp49D->v=_tmp834;});_tmp49D;}));});
# 1838
_tmp49A[1]=_tmp837;}),({
# 1843
struct Cyc_PP_Doc*_tmp831=Cyc_Absynpp_atts2doc(_tmp4D6->atts);_tmp49A[2]=_tmp831;}),({
struct Cyc_PP_Doc*_tmp830=Cyc_PP_text(({const char*_tmp49E=";";_tag_fat(_tmp49E,sizeof(char),2U);}));_tmp49A[3]=_tmp830;});Cyc_PP_cat(_tag_fat(_tmp49A,sizeof(struct Cyc_PP_Doc*),4U));});
# 1846
goto _LL0;}case 9U: _LL13: _tmp4D8=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp4D7=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp473)->f2;_LL14:
# 1848
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4D8);
s=({struct Cyc_PP_Doc*_tmp49F[8U];({struct Cyc_PP_Doc*_tmp841=Cyc_PP_text(({const char*_tmp4A0="namespace ";_tag_fat(_tmp4A0,sizeof(char),11U);}));_tmp49F[0]=_tmp841;}),({
struct Cyc_PP_Doc*_tmp840=Cyc_PP_textptr(_tmp4D8);_tmp49F[1]=_tmp840;}),({
struct Cyc_PP_Doc*_tmp83F=Cyc_PP_blank_doc();_tmp49F[2]=_tmp83F;}),({struct Cyc_PP_Doc*_tmp83E=Cyc_Absynpp_lb();_tmp49F[3]=_tmp83E;}),({
struct Cyc_PP_Doc*_tmp83D=Cyc_PP_line_doc();_tmp49F[4]=_tmp83D;}),({
struct Cyc_PP_Doc*_tmp83C=({struct _fat_ptr _tmp83B=({const char*_tmp4A1="";_tag_fat(_tmp4A1,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp83B,_tmp4D7);});_tmp49F[5]=_tmp83C;}),({
struct Cyc_PP_Doc*_tmp83A=Cyc_PP_line_doc();_tmp49F[6]=_tmp83A;}),({
struct Cyc_PP_Doc*_tmp839=Cyc_Absynpp_rb();_tmp49F[7]=_tmp839;});Cyc_PP_cat(_tag_fat(_tmp49F,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;case 10U: _LL15: _tmp4DA=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp4D9=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp473)->f2;_LL16:
# 1859
 if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp4A2[8U];({struct Cyc_PP_Doc*_tmp84A=Cyc_PP_text(({const char*_tmp4A3="using ";_tag_fat(_tmp4A3,sizeof(char),7U);}));_tmp4A2[0]=_tmp84A;}),({
struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_qvar2doc(_tmp4DA);_tmp4A2[1]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_PP_blank_doc();_tmp4A2[2]=_tmp848;}),({struct Cyc_PP_Doc*_tmp847=Cyc_Absynpp_lb();_tmp4A2[3]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp846=Cyc_PP_line_doc();_tmp4A2[4]=_tmp846;}),({
struct Cyc_PP_Doc*_tmp845=({struct _fat_ptr _tmp844=({const char*_tmp4A4="";_tag_fat(_tmp4A4,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp844,_tmp4D9);});_tmp4A2[5]=_tmp845;}),({
struct Cyc_PP_Doc*_tmp843=Cyc_PP_line_doc();_tmp4A2[6]=_tmp843;}),({
struct Cyc_PP_Doc*_tmp842=Cyc_Absynpp_rb();_tmp4A2[7]=_tmp842;});Cyc_PP_cat(_tag_fat(_tmp4A2,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1868
s=({struct Cyc_PP_Doc*_tmp4A5[11U];({struct Cyc_PP_Doc*_tmp856=Cyc_PP_text(({const char*_tmp4A6="/* using ";_tag_fat(_tmp4A6,sizeof(char),10U);}));_tmp4A5[0]=_tmp856;}),({
struct Cyc_PP_Doc*_tmp855=Cyc_Absynpp_qvar2doc(_tmp4DA);_tmp4A5[1]=_tmp855;}),({
struct Cyc_PP_Doc*_tmp854=Cyc_PP_blank_doc();_tmp4A5[2]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp853=Cyc_Absynpp_lb();_tmp4A5[3]=_tmp853;}),({
struct Cyc_PP_Doc*_tmp852=Cyc_PP_text(({const char*_tmp4A7=" */";_tag_fat(_tmp4A7,sizeof(char),4U);}));_tmp4A5[4]=_tmp852;}),({
struct Cyc_PP_Doc*_tmp851=Cyc_PP_line_doc();_tmp4A5[5]=_tmp851;}),({
struct Cyc_PP_Doc*_tmp850=({struct _fat_ptr _tmp84F=({const char*_tmp4A8="";_tag_fat(_tmp4A8,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp84F,_tmp4D9);});_tmp4A5[6]=_tmp850;}),({
struct Cyc_PP_Doc*_tmp84E=Cyc_PP_line_doc();_tmp4A5[7]=_tmp84E;}),({
struct Cyc_PP_Doc*_tmp84D=Cyc_PP_text(({const char*_tmp4A9="/* ";_tag_fat(_tmp4A9,sizeof(char),4U);}));_tmp4A5[8]=_tmp84D;}),({
struct Cyc_PP_Doc*_tmp84C=Cyc_Absynpp_rb();_tmp4A5[9]=_tmp84C;}),({
struct Cyc_PP_Doc*_tmp84B=Cyc_PP_text(({const char*_tmp4AA=" */";_tag_fat(_tmp4AA,sizeof(char),4U);}));_tmp4A5[10]=_tmp84B;});Cyc_PP_cat(_tag_fat(_tmp4A5,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;case 11U: _LL17: _tmp4DB=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_LL18:
# 1881
 if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp4AB[6U];({struct Cyc_PP_Doc*_tmp85D=Cyc_PP_text(({const char*_tmp4AC="extern \"C\" ";_tag_fat(_tmp4AC,sizeof(char),12U);}));_tmp4AB[0]=_tmp85D;}),({
struct Cyc_PP_Doc*_tmp85C=Cyc_Absynpp_lb();_tmp4AB[1]=_tmp85C;}),({
struct Cyc_PP_Doc*_tmp85B=Cyc_PP_line_doc();_tmp4AB[2]=_tmp85B;}),({
struct Cyc_PP_Doc*_tmp85A=({struct _fat_ptr _tmp859=({const char*_tmp4AD="";_tag_fat(_tmp4AD,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp859,_tmp4DB);});_tmp4AB[3]=_tmp85A;}),({
struct Cyc_PP_Doc*_tmp858=Cyc_PP_line_doc();_tmp4AB[4]=_tmp858;}),({
struct Cyc_PP_Doc*_tmp857=Cyc_Absynpp_rb();_tmp4AB[5]=_tmp857;});Cyc_PP_cat(_tag_fat(_tmp4AB,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1889
s=({struct Cyc_PP_Doc*_tmp4AE[9U];({struct Cyc_PP_Doc*_tmp867=Cyc_PP_text(({const char*_tmp4AF="/* extern \"C\" ";_tag_fat(_tmp4AF,sizeof(char),15U);}));_tmp4AE[0]=_tmp867;}),({
struct Cyc_PP_Doc*_tmp866=Cyc_Absynpp_lb();_tmp4AE[1]=_tmp866;}),({
struct Cyc_PP_Doc*_tmp865=Cyc_PP_text(({const char*_tmp4B0=" */";_tag_fat(_tmp4B0,sizeof(char),4U);}));_tmp4AE[2]=_tmp865;}),({
struct Cyc_PP_Doc*_tmp864=Cyc_PP_line_doc();_tmp4AE[3]=_tmp864;}),({
struct Cyc_PP_Doc*_tmp863=({struct _fat_ptr _tmp862=({const char*_tmp4B1="";_tag_fat(_tmp4B1,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp862,_tmp4DB);});_tmp4AE[4]=_tmp863;}),({
struct Cyc_PP_Doc*_tmp861=Cyc_PP_line_doc();_tmp4AE[5]=_tmp861;}),({
struct Cyc_PP_Doc*_tmp860=Cyc_PP_text(({const char*_tmp4B2="/* ";_tag_fat(_tmp4B2,sizeof(char),4U);}));_tmp4AE[6]=_tmp860;}),({
struct Cyc_PP_Doc*_tmp85F=Cyc_Absynpp_rb();_tmp4AE[7]=_tmp85F;}),({
struct Cyc_PP_Doc*_tmp85E=Cyc_PP_text(({const char*_tmp4B3=" */";_tag_fat(_tmp4B3,sizeof(char),4U);}));_tmp4AE[8]=_tmp85E;});Cyc_PP_cat(_tag_fat(_tmp4AE,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;case 12U: _LL19: _tmp4DF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp473)->f1;_tmp4DE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp473)->f2;_tmp4DD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp473)->f3;_tmp4DC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp473)->f4;_LL1A:
# 1900
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(_tmp4DD != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp4B4[7U];({struct Cyc_PP_Doc*_tmp86F=Cyc_Absynpp_rb();_tmp4B4[0]=_tmp86F;}),({struct Cyc_PP_Doc*_tmp86E=Cyc_PP_text(({const char*_tmp4B5=" export ";_tag_fat(_tmp4B5,sizeof(char),9U);}));_tmp4B4[1]=_tmp86E;}),({struct Cyc_PP_Doc*_tmp86D=Cyc_Absynpp_lb();_tmp4B4[2]=_tmp86D;}),({
struct Cyc_PP_Doc*_tmp86C=Cyc_PP_line_doc();_tmp4B4[3]=_tmp86C;}),({struct Cyc_PP_Doc*_tmp86B=({struct _fat_ptr _tmp86A=({const char*_tmp4B6=",";_tag_fat(_tmp4B6,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple20*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp86A,_tmp4DD);});_tmp4B4[4]=_tmp86B;}),({
struct Cyc_PP_Doc*_tmp869=Cyc_PP_line_doc();_tmp4B4[5]=_tmp869;}),({struct Cyc_PP_Doc*_tmp868=Cyc_Absynpp_rb();_tmp4B4[6]=_tmp868;});Cyc_PP_cat(_tag_fat(_tmp4B4,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1908
exs_doc=Cyc_Absynpp_rb();}
if(_tmp4DE != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp4B7[7U];({struct Cyc_PP_Doc*_tmp877=Cyc_Absynpp_rb();_tmp4B7[0]=_tmp877;}),({struct Cyc_PP_Doc*_tmp876=Cyc_PP_text(({const char*_tmp4B8=" cycdef ";_tag_fat(_tmp4B8,sizeof(char),9U);}));_tmp4B7[1]=_tmp876;}),({struct Cyc_PP_Doc*_tmp875=Cyc_Absynpp_lb();_tmp4B7[2]=_tmp875;}),({
struct Cyc_PP_Doc*_tmp874=Cyc_PP_line_doc();_tmp4B7[3]=_tmp874;}),({struct Cyc_PP_Doc*_tmp873=({struct _fat_ptr _tmp872=({const char*_tmp4B9="";_tag_fat(_tmp4B9,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp872,_tmp4DE);});_tmp4B7[4]=_tmp873;}),({
struct Cyc_PP_Doc*_tmp871=Cyc_PP_line_doc();_tmp4B7[5]=_tmp871;}),({struct Cyc_PP_Doc*_tmp870=Cyc_Absynpp_rb();_tmp4B7[6]=_tmp870;});Cyc_PP_cat(_tag_fat(_tmp4B7,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1914
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp4BA[6U];({struct Cyc_PP_Doc*_tmp87D=Cyc_PP_text(({const char*_tmp4BB="extern \"C include\" ";_tag_fat(_tmp4BB,sizeof(char),20U);}));_tmp4BA[0]=_tmp87D;}),({
struct Cyc_PP_Doc*_tmp87C=Cyc_Absynpp_lb();_tmp4BA[1]=_tmp87C;}),({
struct Cyc_PP_Doc*_tmp87B=Cyc_PP_line_doc();_tmp4BA[2]=_tmp87B;}),({
struct Cyc_PP_Doc*_tmp87A=({struct _fat_ptr _tmp879=({const char*_tmp4BC="";_tag_fat(_tmp4BC,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp879,_tmp4DF);});_tmp4BA[3]=_tmp87A;}),({
struct Cyc_PP_Doc*_tmp878=Cyc_PP_line_doc();_tmp4BA[4]=_tmp878;}),_tmp4BA[5]=exs_doc;Cyc_PP_cat(_tag_fat(_tmp4BA,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1922
s=({struct Cyc_PP_Doc*_tmp4BD[9U];({struct Cyc_PP_Doc*_tmp887=Cyc_PP_text(({const char*_tmp4BE="/* extern \"C include\" ";_tag_fat(_tmp4BE,sizeof(char),23U);}));_tmp4BD[0]=_tmp887;}),({
struct Cyc_PP_Doc*_tmp886=Cyc_Absynpp_lb();_tmp4BD[1]=_tmp886;}),({
struct Cyc_PP_Doc*_tmp885=Cyc_PP_text(({const char*_tmp4BF=" */";_tag_fat(_tmp4BF,sizeof(char),4U);}));_tmp4BD[2]=_tmp885;}),({
struct Cyc_PP_Doc*_tmp884=Cyc_PP_line_doc();_tmp4BD[3]=_tmp884;}),({
struct Cyc_PP_Doc*_tmp883=({struct _fat_ptr _tmp882=({const char*_tmp4C0="";_tag_fat(_tmp4C0,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp882,_tmp4DF);});_tmp4BD[4]=_tmp883;}),({
struct Cyc_PP_Doc*_tmp881=Cyc_PP_line_doc();_tmp4BD[5]=_tmp881;}),({
struct Cyc_PP_Doc*_tmp880=Cyc_PP_text(({const char*_tmp4C1="/* ";_tag_fat(_tmp4C1,sizeof(char),4U);}));_tmp4BD[6]=_tmp880;}),({
struct Cyc_PP_Doc*_tmp87F=Cyc_Absynpp_rb();_tmp4BD[7]=_tmp87F;}),({
struct Cyc_PP_Doc*_tmp87E=Cyc_PP_text(({const char*_tmp4C2=" */";_tag_fat(_tmp4C2,sizeof(char),4U);}));_tmp4BD[8]=_tmp87E;});Cyc_PP_cat(_tag_fat(_tmp4BD,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;case 13U: _LL1B: _LL1C:
# 1933
 s=({struct Cyc_PP_Doc*_tmp4C3[2U];({struct Cyc_PP_Doc*_tmp889=Cyc_PP_text(({const char*_tmp4C4="__cyclone_port_on__;";_tag_fat(_tmp4C4,sizeof(char),21U);}));_tmp4C3[0]=_tmp889;}),({struct Cyc_PP_Doc*_tmp888=Cyc_Absynpp_lb();_tmp4C3[1]=_tmp888;});Cyc_PP_cat(_tag_fat(_tmp4C3,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1936
 s=({struct Cyc_PP_Doc*_tmp4C5[2U];({struct Cyc_PP_Doc*_tmp88B=Cyc_PP_text(({const char*_tmp4C6="__cyclone_port_off__;";_tag_fat(_tmp4C6,sizeof(char),22U);}));_tmp4C5[0]=_tmp88B;}),({struct Cyc_PP_Doc*_tmp88A=Cyc_Absynpp_lb();_tmp4C5[1]=_tmp88A;});Cyc_PP_cat(_tag_fat(_tmp4C5,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1939
 s=({struct Cyc_PP_Doc*_tmp4C7[2U];({struct Cyc_PP_Doc*_tmp88D=Cyc_PP_text(({const char*_tmp4C8="__tempest_on__;";_tag_fat(_tmp4C8,sizeof(char),16U);}));_tmp4C7[0]=_tmp88D;}),({struct Cyc_PP_Doc*_tmp88C=Cyc_Absynpp_lb();_tmp4C7[1]=_tmp88C;});Cyc_PP_cat(_tag_fat(_tmp4C7,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1942
 s=({struct Cyc_PP_Doc*_tmp4C9[2U];({struct Cyc_PP_Doc*_tmp88F=Cyc_PP_text(({const char*_tmp4CA="__tempest_off__;";_tag_fat(_tmp4CA,sizeof(char),17U);}));_tmp4C9[0]=_tmp88F;}),({struct Cyc_PP_Doc*_tmp88E=Cyc_Absynpp_lb();_tmp4C9[1]=_tmp88E;});Cyc_PP_cat(_tag_fat(_tmp4C9,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 1946
return s;}
# 1949
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4E0=sc;switch(_tmp4E0){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4E1="static ";_tag_fat(_tmp4E1,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4E2="extern ";_tag_fat(_tmp4E2,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4E3="extern \"C\" ";_tag_fat(_tmp4E3,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4E4="abstract ";_tag_fat(_tmp4E4,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4E5="register ";_tag_fat(_tmp4E5,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;};}
# 1963
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4E6=Cyc_Tcutil_compress(t);void*_tmp4E7=_tmp4E6;struct Cyc_List_List*_tmp4E9;struct Cyc_Absyn_Tvar*_tmp4E8;switch(*((int*)_tmp4E7)){case 2U: _LL1: _tmp4E8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E7)->f1;_LL2:
 return Cyc_Tcutil_is_temp_tvar(_tmp4E8);case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E7)->f1)->tag == 9U){_LL3: _tmp4E9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E7)->f2;_LL4:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4E9);}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 1975
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4EA=t;struct Cyc_Absyn_Typedefdecl*_tmp4EC;void*_tmp4EB;switch(*((int*)_tmp4EA)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EA)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp4EC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4EA)->f3;_tmp4EB=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4EA)->f4;if(_tmp4EB != 0){_LL6:
# 1982
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4EB);}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1989
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1992
if(tms != 0 && tms->tl != 0){
struct _tuple18 _tmp4ED=({struct _tuple18 _tmp579;_tmp579.f1=(void*)tms->hd,_tmp579.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp579;});struct _tuple18 _tmp4EE=_tmp4ED;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4EE.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4EE.f2)->tag == 3U){_LL1: _LL2:
# 1995
 return({struct Cyc_List_List*_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0));_tmp4F0->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp891=({struct Cyc_List_List*_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF));_tmp4EF->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp890=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4EF->tl=_tmp890;});_tmp4EF;});_tmp4F0->tl=_tmp891;});_tmp4F0;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4F1=_region_malloc(r,sizeof(*_tmp4F1));_tmp4F1->hd=atts,_tmp4F1->tl=tms;_tmp4F1;});}_LL0:;}else{
# 1998
return({struct Cyc_List_List*_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2));_tmp4F2->hd=atts,_tmp4F2->tl=tms;_tmp4F2;});}}
# 2001
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp4F3=({struct _fat_ptr _tmp892=({const char*_tmp4F9="`";_tag_fat(_tmp4F9,sizeof(char),2U);});Cyc_strconcat(_tmp892,(struct _fat_ptr)*t->name);});
({struct _fat_ptr _tmp4F4=_fat_ptr_plus(_tmp4F3,sizeof(char),1);char _tmp4F5=*((char*)_check_fat_subscript(_tmp4F4,sizeof(char),0U));char _tmp4F6='t';if(_get_fat_size(_tmp4F4,sizeof(char))== 1U &&(_tmp4F5 == 0 && _tmp4F6 != 0))_throw_arraybounds();*((char*)_tmp4F4.curr)=_tmp4F6;});
({struct _fat_ptr*_tmp893=({unsigned _tmp4F8=1;struct _fat_ptr*_tmp4F7=_cycalloc(_check_times(_tmp4F8,sizeof(struct _fat_ptr)));_tmp4F7[0]=(struct _fat_ptr)_tmp4F3;_tmp4F7;});t->name=_tmp893;});};}
# 2010
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2012
void*_tmp4FA=t;struct _tuple0*_tmp537;struct Cyc_List_List*_tmp536;struct Cyc_Absyn_Typedefdecl*_tmp535;void*_tmp534;struct Cyc_Core_Opt*_tmp533;void*_tmp532;int _tmp531;struct Cyc_List_List*_tmp530;void*_tmp52F;struct Cyc_Absyn_Tqual _tmp52E;void*_tmp52D;struct Cyc_List_List*_tmp52C;int _tmp52B;struct Cyc_Absyn_VarargInfo*_tmp52A;struct Cyc_List_List*_tmp529;struct Cyc_List_List*_tmp528;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp526;void*_tmp525;struct Cyc_Absyn_Tqual _tmp524;struct Cyc_Absyn_PtrAtts _tmp523;void*_tmp522;struct Cyc_Absyn_Tqual _tmp521;struct Cyc_Absyn_Exp*_tmp520;void*_tmp51F;unsigned _tmp51E;switch(*((int*)_tmp4FA)){case 4U: _LL1: _tmp522=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).elt_type;_tmp521=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).tq;_tmp520=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).num_elts;_tmp51F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).zero_term;_tmp51E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).zt_loc;_LL2: {
# 2015
struct _tuple13 _tmp4FB=Cyc_Absynpp_to_tms(r,_tmp521,_tmp522);struct _tuple13 _tmp4FC=_tmp4FB;struct Cyc_Absyn_Tqual _tmp502;void*_tmp501;struct Cyc_List_List*_tmp500;_LLE: _tmp502=_tmp4FC.f1;_tmp501=_tmp4FC.f2;_tmp500=_tmp4FC.f3;_LLF:;{
void*tm;
if(_tmp520 == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4FD=_region_malloc(r,sizeof(*_tmp4FD));_tmp4FD->tag=0U,_tmp4FD->f1=_tmp51F,_tmp4FD->f2=_tmp51E;_tmp4FD;});else{
# 2020
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4FE=_region_malloc(r,sizeof(*_tmp4FE));_tmp4FE->tag=1U,_tmp4FE->f1=_tmp520,_tmp4FE->f2=_tmp51F,_tmp4FE->f3=_tmp51E;_tmp4FE;});}
return({struct _tuple13 _tmp57A;_tmp57A.f1=_tmp502,_tmp57A.f2=_tmp501,({struct Cyc_List_List*_tmp894=({struct Cyc_List_List*_tmp4FF=_region_malloc(r,sizeof(*_tmp4FF));_tmp4FF->hd=tm,_tmp4FF->tl=_tmp500;_tmp4FF;});_tmp57A.f3=_tmp894;});_tmp57A;});};}case 3U: _LL3: _tmp525=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).elt_type;_tmp524=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).elt_tq;_tmp523=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).ptr_atts;_LL4: {
# 2024
struct _tuple13 _tmp503=Cyc_Absynpp_to_tms(r,_tmp524,_tmp525);struct _tuple13 _tmp504=_tmp503;struct Cyc_Absyn_Tqual _tmp509;void*_tmp508;struct Cyc_List_List*_tmp507;_LL11: _tmp509=_tmp504.f1;_tmp508=_tmp504.f2;_tmp507=_tmp504.f3;_LL12:;
_tmp507=({struct Cyc_List_List*_tmp506=_region_malloc(r,sizeof(*_tmp506));({void*_tmp895=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp505=_region_malloc(r,sizeof(*_tmp505));_tmp505->tag=2U,_tmp505->f1=_tmp523,_tmp505->f2=tq;_tmp505;});_tmp506->hd=_tmp895;}),_tmp506->tl=_tmp507;_tmp506;});
return({struct _tuple13 _tmp57B;_tmp57B.f1=_tmp509,_tmp57B.f2=_tmp508,_tmp57B.f3=_tmp507;_tmp57B;});}case 5U: _LL5: _tmp530=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).tvars;_tmp52F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).effect;_tmp52E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).ret_tqual;_tmp52D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).ret_type;_tmp52C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).args;_tmp52B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).c_varargs;_tmp52A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).cyc_varargs;_tmp529=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).rgn_po;_tmp528=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).attributes;_tmp527=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).requires_clause;_tmp526=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FA)->f1).ensures_clause;_LL6:
# 2030
 if(!Cyc_Absynpp_print_all_tvars){
# 2034
if(_tmp52F == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp52F)){
_tmp52F=0;
_tmp530=0;}}else{
# 2039
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2042
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absynpp_rewrite_temp_tvar,_tmp530);}{
# 2047
struct _tuple13 _tmp50A=Cyc_Absynpp_to_tms(r,_tmp52E,_tmp52D);struct _tuple13 _tmp50B=_tmp50A;struct Cyc_Absyn_Tqual _tmp51D;void*_tmp51C;struct Cyc_List_List*_tmp51B;_LL14: _tmp51D=_tmp50B.f1;_tmp51C=_tmp50B.f2;_tmp51B=_tmp50B.f3;_LL15:;{
struct Cyc_List_List*tms=_tmp51B;
# 2058 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp50C=Cyc_Cyclone_c_compiler;if(_tmp50C == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2060
 if(_tmp528 != 0)
# 2063
tms=({struct _RegionHandle*_tmp897=r;void*_tmp896=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp50D=_region_malloc(r,sizeof(*_tmp50D));_tmp50D->tag=5U,_tmp50D->f1=0U,_tmp50D->f2=_tmp528;_tmp50D;});Cyc_Absynpp_bubble_attributes(_tmp897,_tmp896,tms);});
# 2065
tms=({struct Cyc_List_List*_tmp510=_region_malloc(r,sizeof(*_tmp510));({void*_tmp899=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp50F=_region_malloc(r,sizeof(*_tmp50F));
_tmp50F->tag=3U,({void*_tmp898=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp50E=_region_malloc(r,sizeof(*_tmp50E));_tmp50E->tag=1U,_tmp50E->f1=_tmp52C,_tmp50E->f2=_tmp52B,_tmp50E->f3=_tmp52A,_tmp50E->f4=_tmp52F,_tmp50E->f5=_tmp529,_tmp50E->f6=_tmp527,_tmp50E->f7=_tmp526;_tmp50E;});_tmp50F->f1=_tmp898;});_tmp50F;});
# 2065
_tmp510->hd=_tmp899;}),_tmp510->tl=tms;_tmp510;});
# 2069
goto _LL16;}else{_LL19: _LL1A:
# 2071
 tms=({struct Cyc_List_List*_tmp513=_region_malloc(r,sizeof(*_tmp513));({void*_tmp89B=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp512=_region_malloc(r,sizeof(*_tmp512));
_tmp512->tag=3U,({void*_tmp89A=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp511=_region_malloc(r,sizeof(*_tmp511));_tmp511->tag=1U,_tmp511->f1=_tmp52C,_tmp511->f2=_tmp52B,_tmp511->f3=_tmp52A,_tmp511->f4=_tmp52F,_tmp511->f5=_tmp529,_tmp511->f6=_tmp527,_tmp511->f7=_tmp526;_tmp511;});_tmp512->f1=_tmp89A;});_tmp512;});
# 2071
_tmp513->hd=_tmp89B;}),_tmp513->tl=tms;_tmp513;});
# 2075
for(0;_tmp528 != 0;_tmp528=_tmp528->tl){
void*_tmp514=(void*)_tmp528->hd;void*_tmp515=_tmp514;switch(*((int*)_tmp515)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2080
 tms=({struct Cyc_List_List*_tmp518=_region_malloc(r,sizeof(*_tmp518));({void*_tmp89D=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp517=_region_malloc(r,sizeof(*_tmp517));_tmp517->tag=5U,_tmp517->f1=0U,({struct Cyc_List_List*_tmp89C=({struct Cyc_List_List*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->hd=(void*)_tmp528->hd,_tmp516->tl=0;_tmp516;});_tmp517->f2=_tmp89C;});_tmp517;});_tmp518->hd=_tmp89D;}),_tmp518->tl=tms;_tmp518;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2084
goto _LL16;}_LL16:;}
# 2087
AfterAtts:
 if(_tmp530 != 0)
tms=({struct Cyc_List_List*_tmp51A=_region_malloc(r,sizeof(*_tmp51A));({void*_tmp89E=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp519=_region_malloc(r,sizeof(*_tmp519));_tmp519->tag=4U,_tmp519->f1=_tmp530,_tmp519->f2=0U,_tmp519->f3=1;_tmp519;});_tmp51A->hd=_tmp89E;}),_tmp51A->tl=tms;_tmp51A;});
return({struct _tuple13 _tmp57C;_tmp57C.f1=_tmp51D,_tmp57C.f2=_tmp51C,_tmp57C.f3=tms;_tmp57C;});};};case 1U: _LL7: _tmp533=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FA)->f1;_tmp532=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FA)->f2;_tmp531=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FA)->f3;_LL8:
# 2093
 if(_tmp532 == 0)
return({struct _tuple13 _tmp57D;_tmp57D.f1=tq,_tmp57D.f2=t,_tmp57D.f3=0;_tmp57D;});else{
# 2096
return Cyc_Absynpp_to_tms(r,tq,_tmp532);}case 8U: _LL9: _tmp537=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FA)->f1;_tmp536=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FA)->f2;_tmp535=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FA)->f3;_tmp534=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FA)->f4;_LLA:
# 2102
 if(_tmp534 == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple13 _tmp57E;_tmp57E.f1=tq,_tmp57E.f2=t,_tmp57E.f3=0;_tmp57E;});else{
# 2105
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp534);}default: _LLB: _LLC:
# 2110
 return({struct _tuple13 _tmp57F;_tmp57F.f1=tq,_tmp57F.f2=t,_tmp57F.f3=0;_tmp57F;});}_LL0:;}
# 2114
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2116
void*_tmp538=t;void*_tmp53D;void*_tmp53C;switch(*((int*)_tmp538)){case 1U: _LL1: _tmp53C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp538)->f2;if(_tmp53C != 0){_LL2:
 return Cyc_Absynpp_is_char_ptr(_tmp53C);}else{goto _LL5;}case 3U: _LL3: _tmp53D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp538)->f1).elt_type;_LL4:
# 2119
 L: {
void*_tmp539=_tmp53D;void*_tmp53B;void*_tmp53A;switch(*((int*)_tmp539)){case 1U: _LL8: _tmp53A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp539)->f2;if(_tmp53A != 0){_LL9:
 _tmp53D=_tmp53A;goto L;}else{goto _LLE;}case 8U: _LLA: _tmp53B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp539)->f4;if(_tmp53B != 0){_LLB:
 _tmp53D=_tmp53B;goto L;}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp539)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp539)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 2126
 return 0;}_LL0:;}
# 2130
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp53E=_new_region("temp");struct _RegionHandle*temp=& _tmp53E;_push_region(temp);
{struct _tuple13 _tmp53F=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple13 _tmp540=_tmp53F;struct Cyc_Absyn_Tqual _tmp548;void*_tmp547;struct Cyc_List_List*_tmp546;_LL1: _tmp548=_tmp540.f1;_tmp547=_tmp540.f2;_tmp546=_tmp540.f3;_LL2:;
_tmp546=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp546);
if(_tmp546 == 0 && dopt == 0){
struct Cyc_PP_Doc*_tmp542=({struct Cyc_PP_Doc*_tmp541[2U];({struct Cyc_PP_Doc*_tmp8A0=Cyc_Absynpp_tqual2doc(_tmp548);_tmp541[0]=_tmp8A0;}),({struct Cyc_PP_Doc*_tmp89F=Cyc_Absynpp_ntyp2doc(_tmp547);_tmp541[1]=_tmp89F;});Cyc_PP_cat(_tag_fat(_tmp541,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp542;}else{
# 2137
struct Cyc_PP_Doc*_tmp545=({struct Cyc_PP_Doc*_tmp543[4U];({
struct Cyc_PP_Doc*_tmp8A6=Cyc_Absynpp_tqual2doc(_tmp548);_tmp543[0]=_tmp8A6;}),({
struct Cyc_PP_Doc*_tmp8A5=Cyc_Absynpp_ntyp2doc(_tmp547);_tmp543[1]=_tmp8A5;}),({
struct Cyc_PP_Doc*_tmp8A4=Cyc_PP_text(({const char*_tmp544=" ";_tag_fat(_tmp544,sizeof(char),2U);}));_tmp543[2]=_tmp8A4;}),({
struct Cyc_PP_Doc*_tmp8A3=({int _tmp8A2=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp8A1=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp8A2,_tmp8A1,_tmp546);});_tmp543[3]=_tmp8A3;});Cyc_PP_cat(_tag_fat(_tmp543,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp545;}}
# 2132
;_pop_region(temp);}
# 2145
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp549=f->requires_clause;
if((unsigned)_tmp549)
requires_doc=({struct Cyc_PP_Doc*_tmp54A[2U];({struct Cyc_PP_Doc*_tmp8A8=Cyc_PP_text(({const char*_tmp54B="@requires ";_tag_fat(_tmp54B,sizeof(char),11U);}));_tmp54A[0]=_tmp8A8;}),({struct Cyc_PP_Doc*_tmp8A7=Cyc_Absynpp_exp2doc(_tmp549);_tmp54A[1]=_tmp8A7;});Cyc_PP_cat(_tag_fat(_tmp54A,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2151
requires_doc=Cyc_PP_nil_doc();}{
# 2153
enum Cyc_Cyclone_C_Compilers _tmp54C=Cyc_Cyclone_c_compiler;if(_tmp54C == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2156
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp54D[5U];({struct Cyc_PP_Doc*_tmp8B0=({struct Cyc_Absyn_Tqual _tmp8AF=f->tq;void*_tmp8AE=f->type;Cyc_Absynpp_tqtd2doc(_tmp8AF,_tmp8AE,({struct Cyc_Core_Opt*_tmp54E=_cycalloc(sizeof(*_tmp54E));({struct Cyc_PP_Doc*_tmp8AD=Cyc_PP_textptr(f->name);_tmp54E->v=_tmp8AD;});_tmp54E;}));});_tmp54D[0]=_tmp8B0;}),({
struct Cyc_PP_Doc*_tmp8AC=Cyc_PP_text(({const char*_tmp54F=":";_tag_fat(_tmp54F,sizeof(char),2U);}));_tmp54D[1]=_tmp8AC;}),({struct Cyc_PP_Doc*_tmp8AB=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp54D[2]=_tmp8AB;}),({
struct Cyc_PP_Doc*_tmp8AA=Cyc_Absynpp_atts2doc(f->attributes);_tmp54D[3]=_tmp8AA;}),({struct Cyc_PP_Doc*_tmp8A9=Cyc_PP_text(({const char*_tmp550=";";_tag_fat(_tmp550,sizeof(char),2U);}));_tmp54D[4]=_tmp8A9;});Cyc_PP_cat(_tag_fat(_tmp54D,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2161
return({struct Cyc_PP_Doc*_tmp551[4U];({struct Cyc_PP_Doc*_tmp8B6=({struct Cyc_Absyn_Tqual _tmp8B5=f->tq;void*_tmp8B4=f->type;Cyc_Absynpp_tqtd2doc(_tmp8B5,_tmp8B4,({struct Cyc_Core_Opt*_tmp552=_cycalloc(sizeof(*_tmp552));({struct Cyc_PP_Doc*_tmp8B3=Cyc_PP_textptr(f->name);_tmp552->v=_tmp8B3;});_tmp552;}));});_tmp551[0]=_tmp8B6;}),({
struct Cyc_PP_Doc*_tmp8B2=Cyc_Absynpp_atts2doc(f->attributes);_tmp551[1]=_tmp8B2;}),_tmp551[2]=requires_doc,({struct Cyc_PP_Doc*_tmp8B1=Cyc_PP_text(({const char*_tmp553=";";_tag_fat(_tmp553,sizeof(char),2U);}));_tmp551[3]=_tmp8B1;});Cyc_PP_cat(_tag_fat(_tmp551,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2164
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp554[5U];({struct Cyc_PP_Doc*_tmp8BE=Cyc_Absynpp_atts2doc(f->attributes);_tmp554[0]=_tmp8BE;}),({
struct Cyc_PP_Doc*_tmp8BD=({struct Cyc_Absyn_Tqual _tmp8BC=f->tq;void*_tmp8BB=f->type;Cyc_Absynpp_tqtd2doc(_tmp8BC,_tmp8BB,({struct Cyc_Core_Opt*_tmp555=_cycalloc(sizeof(*_tmp555));({struct Cyc_PP_Doc*_tmp8BA=Cyc_PP_textptr(f->name);_tmp555->v=_tmp8BA;});_tmp555;}));});_tmp554[1]=_tmp8BD;}),({
struct Cyc_PP_Doc*_tmp8B9=Cyc_PP_text(({const char*_tmp556=":";_tag_fat(_tmp556,sizeof(char),2U);}));_tmp554[2]=_tmp8B9;}),({struct Cyc_PP_Doc*_tmp8B8=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp554[3]=_tmp8B8;}),({struct Cyc_PP_Doc*_tmp8B7=Cyc_PP_text(({const char*_tmp557=";";_tag_fat(_tmp557,sizeof(char),2U);}));_tmp554[4]=_tmp8B7;});Cyc_PP_cat(_tag_fat(_tmp554,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2169
return({struct Cyc_PP_Doc*_tmp558[4U];({struct Cyc_PP_Doc*_tmp8C4=Cyc_Absynpp_atts2doc(f->attributes);_tmp558[0]=_tmp8C4;}),({
struct Cyc_PP_Doc*_tmp8C3=({struct Cyc_Absyn_Tqual _tmp8C2=f->tq;void*_tmp8C1=f->type;Cyc_Absynpp_tqtd2doc(_tmp8C2,_tmp8C1,({struct Cyc_Core_Opt*_tmp559=_cycalloc(sizeof(*_tmp559));({struct Cyc_PP_Doc*_tmp8C0=Cyc_PP_textptr(f->name);_tmp559->v=_tmp8C0;});_tmp559;}));});_tmp558[1]=_tmp8C3;}),_tmp558[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp8BF=Cyc_PP_text(({const char*_tmp55A=";";_tag_fat(_tmp55A,sizeof(char),2U);}));_tmp558[3]=_tmp8BF;});Cyc_PP_cat(_tag_fat(_tmp558,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;};}
# 2176
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp8C5=({const char*_tmp55B="";_tag_fat(_tmp55B,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp8C5,fields);});}
# 2180
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp55C[3U];({struct Cyc_PP_Doc*_tmp8C9=Cyc_Absynpp_scope2doc(f->sc);_tmp55C[0]=_tmp8C9;}),({struct Cyc_PP_Doc*_tmp8C8=Cyc_Absynpp_typedef_name2doc(f->name);_tmp55C[1]=_tmp8C8;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp8C7=Cyc_PP_nil_doc();_tmp55C[2]=_tmp8C7;}):({struct Cyc_PP_Doc*_tmp8C6=Cyc_Absynpp_args2doc(f->typs);_tmp55C[2]=_tmp8C6;});Cyc_PP_cat(_tag_fat(_tmp55C,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2185
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp8CA=({const char*_tmp55D=",";_tag_fat(_tmp55D,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp8CA,fields);});}
# 2194
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp8CB=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp8CB,72,f);});
({void*_tmp55E=0U;({struct Cyc___cycFILE*_tmp8CD=f;struct _fat_ptr _tmp8CC=({const char*_tmp55F="\n";_tag_fat(_tmp55F,sizeof(char),2U);});Cyc_fprintf(_tmp8CD,_tmp8CC,_tag_fat(_tmp55E,sizeof(void*),0U));});});}}
# 2201
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _tmp8CE=({const char*_tmp560="";_tag_fat(_tmp560,sizeof(char),1U);});Cyc_PP_seql(_tmp8CE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2204
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
return s;};}
# 2218
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}

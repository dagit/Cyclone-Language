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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 694 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 877
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 886
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 891
void*Cyc_Absyn_compress_kb(void*);
# 897
int Cyc_Absyn_type2bool(int def,void*);
# 902
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1136
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1146
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1154
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
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
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
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp593=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp593,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v,_tmp1->tl=0;_tmp1;}));});}
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
return({struct _fat_ptr _tmp596=({const char*_tmp6F="";_tag_fat(_tmp6F,sizeof(char),1U);});struct _fat_ptr _tmp595=({const char*_tmp70=" ";_tag_fat(_tmp70,sizeof(char),2U);});struct _fat_ptr _tmp594=({const char*_tmp71=" ";_tag_fat(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp596,_tmp595,_tmp594,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
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
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp92=({struct Cyc_String_pa_PrintArg_struct _tmp574;_tmp574.tag=0U,({struct _fat_ptr _tmp597=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k));_tmp574.f1=_tmp597;});_tmp574;});void*_tmp90[1U];_tmp90[0]=& _tmp92;({struct _fat_ptr _tmp598=({const char*_tmp91="<=%s";_tag_fat(_tmp91,sizeof(char),5U);});Cyc_aprintf(_tmp598,_tag_fat(_tmp90,sizeof(void*),1U));});});}}_LL0:;}
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
return({struct _fat_ptr _tmp59B=({const char*_tmp99="<";_tag_fat(_tmp99,sizeof(char),2U);});struct _fat_ptr _tmp59A=({const char*_tmp9A=">";_tag_fat(_tmp9A,sizeof(char),2U);});struct _fat_ptr _tmp599=({const char*_tmp9B=",";_tag_fat(_tmp9B,sizeof(char),2U);});Cyc_PP_egroup(_tmp59B,_tmp59A,_tmp599,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
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
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp576;_tmp576.tag=0U,_tmp576.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_tmp576;});struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp575;_tmp575.tag=0U,({struct _fat_ptr _tmp59C=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(n,sizeof(char),1));_tmp575.f1=_tmp59C;});_tmp575;});void*_tmpA1[2U];_tmpA1[0]=& _tmpA3,_tmpA1[1]=& _tmpA4;({struct _fat_ptr _tmp59D=({const char*_tmpA2="`G%s%s";_tag_fat(_tmpA2,sizeof(char),7U);});Cyc_aprintf(_tmp59D,_tag_fat(_tmpA1,sizeof(void*),2U));});}));}
# 444
return Cyc_PP_text(n);}
# 447
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp5=_tmpA6;void*_tmpA7=_stmttmp5;struct Cyc_Absyn_Kind*_tmpA8;struct Cyc_Absyn_Kind*_tmpA9;switch(*((int*)_tmpA7)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpA9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmpA9;
# 452
return({struct Cyc_PP_Doc*_tmpAA[3U];({struct Cyc_PP_Doc*_tmp5A0=Cyc_Absynpp_tvar2doc(tv);_tmpAA[0]=_tmp5A0;}),({struct Cyc_PP_Doc*_tmp59F=Cyc_PP_text(({const char*_tmpAB="::";_tag_fat(_tmpAB,sizeof(char),3U);}));_tmpAA[1]=_tmp59F;}),({struct Cyc_PP_Doc*_tmp59E=Cyc_Absynpp_kind2doc(k);_tmpAA[2]=_tmp59E;});Cyc_PP_cat(_tag_fat(_tmpAA,sizeof(struct Cyc_PP_Doc*),3U));});}}default: _LL5: _tmpA8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmpA8;
# 451
_tmpA9=k;goto _LL8;}}_LL0:;}
# 456
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _tmp5A3=({const char*_tmpAC="<";_tag_fat(_tmpAC,sizeof(char),2U);});struct _fat_ptr _tmp5A2=({const char*_tmpAD=">";_tag_fat(_tmpAD,sizeof(char),2U);});struct _fat_ptr _tmp5A1=({const char*_tmpAE=",";_tag_fat(_tmpAE,sizeof(char),2U);});Cyc_PP_egroup(_tmp5A3,_tmp5A2,_tmp5A1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _tmp5A6=({const char*_tmpAF="<";_tag_fat(_tmpAF,sizeof(char),2U);});struct _fat_ptr _tmp5A5=({const char*_tmpB0=">";_tag_fat(_tmpB0,sizeof(char),2U);});struct _fat_ptr _tmp5A4=({const char*_tmpB1=",";_tag_fat(_tmpB1,sizeof(char),2U);});Cyc_PP_egroup(_tmp5A6,_tmp5A5,_tmp5A4,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 466
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple17*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 470
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp5A9=({const char*_tmpB2="(";_tag_fat(_tmpB2,sizeof(char),2U);});struct _fat_ptr _tmp5A8=({const char*_tmpB3=")";_tag_fat(_tmpB3,sizeof(char),2U);});struct _fat_ptr _tmp5A7=({const char*_tmpB4=",";_tag_fat(_tmpB4,sizeof(char),2U);});Cyc_PP_group(_tmp5A9,_tmp5A8,_tmp5A7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
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
return({struct Cyc_PP_Doc*_tmpBD[3U];({struct Cyc_PP_Doc*_tmp5AF=Cyc_PP_text(({const char*_tmpBE=" __declspec(";_tag_fat(_tmpBE,sizeof(char),13U);}));_tmpBD[0]=_tmp5AF;}),({
struct Cyc_PP_Doc*_tmp5AE=({struct _fat_ptr _tmp5AD=({const char*_tmpBF="";_tag_fat(_tmpBF,sizeof(char),1U);});struct _fat_ptr _tmp5AC=({const char*_tmpC0="";_tag_fat(_tmpC0,sizeof(char),1U);});struct _fat_ptr _tmp5AB=({const char*_tmpC1=" ";_tag_fat(_tmpC1,sizeof(char),2U);});Cyc_PP_group(_tmp5AD,_tmp5AC,_tmp5AB,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBD[1]=_tmp5AE;}),({
struct Cyc_PP_Doc*_tmp5AA=Cyc_PP_text(({const char*_tmpC2=")";_tag_fat(_tmpC2,sizeof(char),2U);}));_tmpBD[2]=_tmp5AA;});Cyc_PP_cat(_tag_fat(_tmpBD,sizeof(struct Cyc_PP_Doc*),3U));});}
# 510
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 514
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC3=Cyc_Cyclone_c_compiler;if(_tmpC3 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 519
 return({struct Cyc_PP_Doc*_tmpC4[2U];({struct Cyc_PP_Doc*_tmp5B4=Cyc_PP_text(({const char*_tmpC5=" __attribute__";_tag_fat(_tmpC5,sizeof(char),15U);}));_tmpC4[0]=_tmp5B4;}),({
struct Cyc_PP_Doc*_tmp5B3=({struct _fat_ptr _tmp5B2=({const char*_tmpC6="((";_tag_fat(_tmpC6,sizeof(char),3U);});struct _fat_ptr _tmp5B1=({const char*_tmpC7="))";_tag_fat(_tmpC7,sizeof(char),3U);});struct _fat_ptr _tmp5B0=({const char*_tmpC8=",";_tag_fat(_tmpC8,sizeof(char),2U);});Cyc_PP_group(_tmp5B2,_tmp5B1,_tmp5B0,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC4[1]=_tmp5B3;});Cyc_PP_cat(_tag_fat(_tmpC4,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}}
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
return({struct Cyc_PP_Doc*_tmpD4[3U];({struct Cyc_PP_Doc*_tmp5B7=Cyc_Absynpp_lb();_tmpD4[0]=_tmp5B7;}),({struct Cyc_PP_Doc*_tmp5B6=Cyc_PP_seq(sep,ss);_tmpD4[1]=_tmp5B6;}),({struct Cyc_PP_Doc*_tmp5B5=Cyc_Absynpp_rb();_tmpD4[2]=_tmp5B5;});Cyc_PP_cat(_tag_fat(_tmpD4,sizeof(struct Cyc_PP_Doc*),3U));});}
# 581
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD5=(void*)tms->hd;void*_stmttmp9=_tmpD5;void*_tmpD6=_stmttmp9;struct Cyc_List_List*_tmpD7;switch(*((int*)_tmpD6)){case 0U: _LL1: _LL2:
({void*_tmpD8=0U;({struct Cyc___cycFILE*_tmp5B9=Cyc_stderr;struct _fat_ptr _tmp5B8=({const char*_tmpD9="Carray_mod ";_tag_fat(_tmpD9,sizeof(char),12U);});Cyc_fprintf(_tmp5B9,_tmp5B8,_tag_fat(_tmpD8,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpDA=0U;({struct Cyc___cycFILE*_tmp5BB=Cyc_stderr;struct _fat_ptr _tmp5BA=({const char*_tmpDB="ConstArray_mod ";_tag_fat(_tmpDB,sizeof(char),16U);});Cyc_fprintf(_tmp5BB,_tmp5BA,_tag_fat(_tmpDA,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->tag == 1U){_LL5: _tmpD7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->f1;_LL6: {struct Cyc_List_List*ys=_tmpD7;
# 587
({void*_tmpDC=0U;({struct Cyc___cycFILE*_tmp5BD=Cyc_stderr;struct _fat_ptr _tmp5BC=({const char*_tmpDD="Function_mod(";_tag_fat(_tmpDD,sizeof(char),14U);});Cyc_fprintf(_tmp5BD,_tmp5BC,_tag_fat(_tmpDC,sizeof(void*),0U));});});
for(0;ys != 0;ys=ys->tl){
struct _fat_ptr*_tmpDE=(*((struct _tuple8*)ys->hd)).f1;struct _fat_ptr*v=_tmpDE;
if(v == 0)({void*_tmpDF=0U;({struct Cyc___cycFILE*_tmp5BF=Cyc_stderr;struct _fat_ptr _tmp5BE=({const char*_tmpE0="?";_tag_fat(_tmpE0,sizeof(char),2U);});Cyc_fprintf(_tmp5BF,_tmp5BE,_tag_fat(_tmpDF,sizeof(void*),0U));});});else{
({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp5C1=Cyc_stderr;struct _fat_ptr _tmp5C0=*v;Cyc_fprintf(_tmp5C1,_tmp5C0,_tag_fat(_tmpE1,sizeof(void*),0U));});});}
if(ys->tl != 0)({void*_tmpE2=0U;({struct Cyc___cycFILE*_tmp5C3=Cyc_stderr;struct _fat_ptr _tmp5C2=({const char*_tmpE3=",";_tag_fat(_tmpE3,sizeof(char),2U);});Cyc_fprintf(_tmp5C3,_tmp5C2,_tag_fat(_tmpE2,sizeof(void*),0U));});});}
# 594
({void*_tmpE4=0U;({struct Cyc___cycFILE*_tmp5C5=Cyc_stderr;struct _fat_ptr _tmp5C4=({const char*_tmpE5=") ";_tag_fat(_tmpE5,sizeof(char),3U);});Cyc_fprintf(_tmp5C5,_tmp5C4,_tag_fat(_tmpE4,sizeof(void*),0U));});});
goto _LL0;}}else{_LL7: _LL8:
# 597
({void*_tmpE6=0U;({struct Cyc___cycFILE*_tmp5C7=Cyc_stderr;struct _fat_ptr _tmp5C6=({const char*_tmpE7="Function_mod()";_tag_fat(_tmpE7,sizeof(char),15U);});Cyc_fprintf(_tmp5C7,_tmp5C6,_tag_fat(_tmpE6,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE8=0U;({struct Cyc___cycFILE*_tmp5C9=Cyc_stderr;struct _fat_ptr _tmp5C8=({const char*_tmpE9="Attributes_mod ";_tag_fat(_tmpE9,sizeof(char),16U);});Cyc_fprintf(_tmp5C9,_tmp5C8,_tag_fat(_tmpE8,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpEA=0U;({struct Cyc___cycFILE*_tmp5CB=Cyc_stderr;struct _fat_ptr _tmp5CA=({const char*_tmpEB="TypeParams_mod ";_tag_fat(_tmpEB,sizeof(char),16U);});Cyc_fprintf(_tmp5CB,_tmp5CA,_tag_fat(_tmpEA,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpEC=0U;({struct Cyc___cycFILE*_tmp5CD=Cyc_stderr;struct _fat_ptr _tmp5CC=({const char*_tmpED="Pointer_mod ";_tag_fat(_tmpED,sizeof(char),13U);});Cyc_fprintf(_tmp5CD,_tmp5CC,_tag_fat(_tmpEC,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 602
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp5CF=Cyc_stderr;struct _fat_ptr _tmp5CE=({const char*_tmpEF="\n";_tag_fat(_tmpEF,sizeof(char),2U);});Cyc_fprintf(_tmp5CF,_tmp5CE,_tag_fat(_tmpEE,sizeof(void*),0U));});});}
# 605
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 607
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp134[3U];({struct Cyc_PP_Doc*_tmp5D1=Cyc_PP_text(({const char*_tmp135="(";_tag_fat(_tmp135,sizeof(char),2U);}));_tmp134[0]=_tmp5D1;}),_tmp134[1]=rest,({struct Cyc_PP_Doc*_tmp5D0=Cyc_PP_text(({const char*_tmp136=")";_tag_fat(_tmp136,sizeof(char),2U);}));_tmp134[2]=_tmp5D0;});Cyc_PP_cat(_tag_fat(_tmp134,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpF0=(void*)tms->hd;void*_stmttmpA=_tmpF0;void*_tmpF1=_stmttmpA;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF5;void*_tmpF4;void*_tmpF3;void*_tmpF2;int _tmpF9;unsigned _tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpFA;void*_tmpFB;void*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFE;switch(*((int*)_tmpF1)){case 0U: _LL1: _tmpFE=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL2: {void*zeroterm=_tmpFE;
# 613
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpFF[2U];_tmpFF[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_tmp5D3=Cyc_PP_text(({const char*_tmp100="[]@zeroterm";_tag_fat(_tmp100,sizeof(char),12U);}));_tmpFF[1]=_tmp5D3;}):({struct Cyc_PP_Doc*_tmp5D2=Cyc_PP_text(({const char*_tmp101="[]";_tag_fat(_tmp101,sizeof(char),3U);}));_tmpFF[1]=_tmp5D2;});Cyc_PP_cat(_tag_fat(_tmpFF,sizeof(struct Cyc_PP_Doc*),2U));});}case 1U: _LL3: _tmpFC=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmpFD=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL4: {struct Cyc_Absyn_Exp*e=_tmpFC;void*zeroterm=_tmpFD;
# 617
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp102[4U];_tmp102[0]=rest,({struct Cyc_PP_Doc*_tmp5D7=Cyc_PP_text(({const char*_tmp103="[";_tag_fat(_tmp103,sizeof(char),2U);}));_tmp102[1]=_tmp5D7;}),({struct Cyc_PP_Doc*_tmp5D6=Cyc_Absynpp_exp2doc(e);_tmp102[2]=_tmp5D6;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_tmp5D5=Cyc_PP_text(({const char*_tmp104="]@zeroterm";_tag_fat(_tmp104,sizeof(char),11U);}));_tmp102[3]=_tmp5D5;}):({struct Cyc_PP_Doc*_tmp5D4=Cyc_PP_text(({const char*_tmp105="]";_tag_fat(_tmp105,sizeof(char),2U);}));_tmp102[3]=_tmp5D4;});Cyc_PP_cat(_tag_fat(_tmp102,sizeof(struct Cyc_PP_Doc*),4U));});}case 3U: _LL5: _tmpFB=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL6: {void*args=_tmpFB;
# 621
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp106=args;unsigned _tmp108;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_List_List*_tmp10D;void*_tmp10C;struct Cyc_Absyn_VarargInfo*_tmp10B;int _tmp10A;struct Cyc_List_List*_tmp109;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->tag == 1U){_LLE: _tmp109=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f1;_tmp10A=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f2;_tmp10B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f3;_tmp10C=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f4;_tmp10D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f5;_tmp10E=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f6;_tmp10F=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f7;_LLF: {struct Cyc_List_List*args2=_tmp109;int c_varargs=_tmp10A;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp10B;void*effopt=_tmp10C;struct Cyc_List_List*rgn_po=_tmp10D;struct Cyc_Absyn_Exp*req=_tmp10E;struct Cyc_Absyn_Exp*ens=_tmp10F;
# 624
return({struct Cyc_PP_Doc*_tmp110[2U];_tmp110[0]=rest,({struct Cyc_PP_Doc*_tmp5D8=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);_tmp110[1]=_tmp5D8;});Cyc_PP_cat(_tag_fat(_tmp110,sizeof(struct Cyc_PP_Doc*),2U));});}}else{_LL10: _tmp107=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp106)->f1;_tmp108=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp106)->f2;_LL11: {struct Cyc_List_List*sl=_tmp107;unsigned loc=_tmp108;
# 626
return({struct Cyc_PP_Doc*_tmp111[2U];_tmp111[0]=rest,({struct Cyc_PP_Doc*_tmp5DC=({struct _fat_ptr _tmp5DB=({const char*_tmp112="(";_tag_fat(_tmp112,sizeof(char),2U);});struct _fat_ptr _tmp5DA=({const char*_tmp113=")";_tag_fat(_tmp113,sizeof(char),2U);});struct _fat_ptr _tmp5D9=({const char*_tmp114=",";_tag_fat(_tmp114,sizeof(char),2U);});Cyc_PP_group(_tmp5DB,_tmp5DA,_tmp5D9,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,sl));});_tmp111[1]=_tmp5DC;});Cyc_PP_cat(_tag_fat(_tmp111,sizeof(struct Cyc_PP_Doc*),2U));});}}_LLD:;}}case 5U: _LL7: _tmpFA=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL8: {struct Cyc_List_List*atts=_tmpFA;
# 629
enum Cyc_Cyclone_C_Compilers _tmp115=Cyc_Cyclone_c_compiler;if(_tmp115 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 631
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp116[2U];_tmp116[0]=rest,({struct Cyc_PP_Doc*_tmp5DD=Cyc_Absynpp_atts2doc(atts);_tmp116[1]=_tmp5DD;});Cyc_PP_cat(_tag_fat(_tmp116,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 635
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp117[2U];({struct Cyc_PP_Doc*_tmp5DE=Cyc_Absynpp_callconv2doc(atts);_tmp117[0]=_tmp5DE;}),_tmp117[1]=rest;Cyc_PP_cat(_tag_fat(_tmp117,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmpF7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmpF8=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_tmpF9=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f3;_LLA: {struct Cyc_List_List*ts=_tmpF7;unsigned loc=_tmpF8;int print_kinds=_tmpF9;
# 640
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_tmp118[2U];_tmp118[0]=rest,({struct Cyc_PP_Doc*_tmp5DF=Cyc_Absynpp_ktvars2doc(ts);_tmp118[1]=_tmp5DF;});Cyc_PP_cat(_tag_fat(_tmp118,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 644
return({struct Cyc_PP_Doc*_tmp119[2U];_tmp119[0]=rest,({struct Cyc_PP_Doc*_tmp5E0=Cyc_Absynpp_tvars2doc(ts);_tmp119[1]=_tmp5E0;});Cyc_PP_cat(_tag_fat(_tmp119,sizeof(struct Cyc_PP_Doc*),2U));});}}default: _LLB: _tmpF2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).rgn;_tmpF3=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).nullable;_tmpF4=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).bounds;_tmpF5=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).zero_term;_tmpF6=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LLC: {void*rgn=_tmpF2;void*nullable=_tmpF3;void*bd=_tmpF4;void*zt=_tmpF5;struct Cyc_Absyn_Tqual tq2=_tmpF6;
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
ptr=({struct Cyc_PP_Doc*_tmp126[4U];_tmp126[0]=ptr,({struct Cyc_PP_Doc*_tmp5E3=Cyc_Absynpp_lb();_tmp126[1]=_tmp5E3;}),({struct Cyc_PP_Doc*_tmp5E2=Cyc_Absynpp_exp2doc(e);_tmp126[2]=_tmp5E2;}),({struct Cyc_PP_Doc*_tmp5E1=Cyc_Absynpp_rb();_tmp126[3]=_tmp5E1;});Cyc_PP_cat(_tag_fat(_tmp126,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}}}}else{_LL21: _LL22:
# 664
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp127="*";_tag_fat(_tmp127,sizeof(char),2U);}):({const char*_tmp128="@";_tag_fat(_tmp128,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp129[4U];_tmp129[0]=ptr,({struct Cyc_PP_Doc*_tmp5E6=Cyc_Absynpp_lb();_tmp129[1]=_tmp5E6;}),({struct Cyc_PP_Doc*_tmp5E5=Cyc_Absynpp_typ2doc(targ);_tmp129[2]=_tmp5E5;}),({struct Cyc_PP_Doc*_tmp5E4=Cyc_Absynpp_rb();_tmp129[3]=_tmp5E4;});Cyc_PP_cat(_tag_fat(_tmp129,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}_LL1E:;}
# 668
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 670
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp12A="*";_tag_fat(_tmp12A,sizeof(char),2U);}):({const char*_tmp12B="@";_tag_fat(_tmp12B,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp12C[4U];_tmp12C[0]=ptr,({struct Cyc_PP_Doc*_tmp5E9=Cyc_Absynpp_lb();_tmp12C[1]=_tmp5E9;}),({struct Cyc_PP_Doc*_tmp5E8=Cyc_Absynpp_typ2doc(bd);_tmp12C[2]=_tmp5E8;}),({struct Cyc_PP_Doc*_tmp5E7=Cyc_Absynpp_rb();_tmp12C[3]=_tmp5E7;});Cyc_PP_cat(_tag_fat(_tmp12C,sizeof(struct Cyc_PP_Doc*),4U));});
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
({struct Cyc_List_List*_tmp5EB=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_PP_Doc*_tmp5EA=Cyc_Absynpp_rgn2doc(r);_tmp140->hd=_tmp5EA;}),_tmp140->tl=*rgions;_tmp140;});*rgions=_tmp5EB;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp13E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2;_LL4: {struct Cyc_List_List*ts=_tmp13E;
# 709
for(0;ts != 0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
# 712
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp5ED=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));({struct Cyc_PP_Doc*_tmp5EC=Cyc_Absynpp_typ2doc(t);_tmp141->hd=_tmp5EC;}),_tmp141->tl=*effects;_tmp141;});*effects=_tmp5ED;});goto _LL0;}_LL0:;}
# 717
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0 && effects != 0)
return({struct _fat_ptr _tmp5F0=({const char*_tmp142="";_tag_fat(_tmp142,sizeof(char),1U);});struct _fat_ptr _tmp5EF=({const char*_tmp143="";_tag_fat(_tmp143,sizeof(char),1U);});struct _fat_ptr _tmp5EE=({const char*_tmp144="+";_tag_fat(_tmp144,sizeof(char),2U);});Cyc_PP_group(_tmp5F0,_tmp5EF,_tmp5EE,effects);});else{
# 725
struct Cyc_PP_Doc*_tmp145=({struct _fat_ptr _tmp5F1=({const char*_tmp14A=",";_tag_fat(_tmp14A,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp5F1,rgions);});struct Cyc_PP_Doc*doc1=_tmp145;
return({struct _fat_ptr _tmp5F5=({const char*_tmp146="";_tag_fat(_tmp146,sizeof(char),1U);});struct _fat_ptr _tmp5F4=({const char*_tmp147="";_tag_fat(_tmp147,sizeof(char),1U);});struct _fat_ptr _tmp5F3=({const char*_tmp148="+";_tag_fat(_tmp148,sizeof(char),2U);});Cyc_PP_group(_tmp5F5,_tmp5F4,_tmp5F3,({struct Cyc_List_List*_tmp5F2=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5F2,({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=doc1,_tmp149->tl=0;_tmp149;}));}));});}}
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
s=({struct Cyc_PP_Doc*_tmp17F[4U];_tmp17F[0]=ext,_tmp17F[1]=kw,({struct Cyc_PP_Doc*_tmp5F7=Cyc_Absynpp_qvar2doc(n);_tmp17F[2]=_tmp5F7;}),({struct Cyc_PP_Doc*_tmp5F6=Cyc_Absynpp_tps2doc(ts);_tmp17F[3]=_tmp5F6;});Cyc_PP_cat(_tag_fat(_tmp17F,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 770
goto _LL0;}case 19U: _LLF: _tmp16B=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL10: {union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp16B;struct Cyc_List_List*ts=_tmp16C;
# 772
{union Cyc_Absyn_DatatypeFieldInfo _tmp182=tuf_info;struct _tuple0*_tmp185;int _tmp184;struct _tuple0*_tmp183;int _tmp188;struct _tuple0*_tmp187;struct _tuple0*_tmp186;if((_tmp182.UnknownDatatypefield).tag == 1){_LL4D: _tmp186=((_tmp182.UnknownDatatypefield).val).datatype_name;_tmp187=((_tmp182.UnknownDatatypefield).val).field_name;_tmp188=((_tmp182.UnknownDatatypefield).val).is_extensible;_LL4E: {struct _tuple0*tname=_tmp186;struct _tuple0*fname=_tmp187;int is_x=_tmp188;
# 774
_tmp183=tname;_tmp184=is_x;_tmp185=fname;goto _LL50;}}else{_LL4F: _tmp183=(((_tmp182.KnownDatatypefield).val).f1)->name;_tmp184=(((_tmp182.KnownDatatypefield).val).f1)->is_extensible;_tmp185=(((_tmp182.KnownDatatypefield).val).f2)->name;_LL50: {struct _tuple0*tname=_tmp183;int is_x=_tmp184;struct _tuple0*fname=_tmp185;
# 777
struct Cyc_PP_Doc*_tmp189=Cyc_PP_text(is_x?({const char*_tmp18C="@extensible datatype ";_tag_fat(_tmp18C,sizeof(char),22U);}):({const char*_tmp18D="datatype ";_tag_fat(_tmp18D,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp189;
s=({struct Cyc_PP_Doc*_tmp18A[4U];_tmp18A[0]=kw,({struct Cyc_PP_Doc*_tmp5FA=Cyc_Absynpp_qvar2doc(tname);_tmp18A[1]=_tmp5FA;}),({struct Cyc_PP_Doc*_tmp5F9=Cyc_PP_text(({const char*_tmp18B=".";_tag_fat(_tmp18B,sizeof(char),2U);}));_tmp18A[2]=_tmp5F9;}),({struct Cyc_PP_Doc*_tmp5F8=Cyc_Absynpp_qvar2doc(fname);_tmp18A[3]=_tmp5F8;});Cyc_PP_cat(_tag_fat(_tmp18A,sizeof(struct Cyc_PP_Doc*),4U));});
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
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19F=({struct Cyc_String_pa_PrintArg_struct _tmp578;_tmp578.tag=0U,_tmp578.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_tmp578;});struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp577;_tmp577.tag=0U,_tmp577.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_tmp577;});void*_tmp19D[2U];_tmp19D[0]=& _tmp19F,_tmp19D[1]=& _tmp1A0;({struct _fat_ptr _tmp5FB=({const char*_tmp19E="%s%s";_tag_fat(_tmp19E,sizeof(char),5U);});Cyc_aprintf(_tmp5FB,_tag_fat(_tmp19D,sizeof(void*),2U));});}));
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
s=({struct Cyc_PP_Doc*_tmp1AA[3U];({struct Cyc_PP_Doc*_tmp5FE=Cyc_Absynpp_aggr_kind2doc(k);_tmp1AA[0]=_tmp5FE;}),({struct Cyc_PP_Doc*_tmp5FD=Cyc_Absynpp_qvar2doc(n);_tmp1AA[1]=_tmp5FD;}),({struct Cyc_PP_Doc*_tmp5FC=Cyc_Absynpp_tps2doc(ts);_tmp1AA[2]=_tmp5FC;});Cyc_PP_cat(_tag_fat(_tmp1AA,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 16U: _LL1B: _tmp165=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1C: {struct Cyc_List_List*fs=_tmp165;
# 830
s=({struct Cyc_PP_Doc*_tmp1AC[4U];({struct Cyc_PP_Doc*_tmp602=Cyc_PP_text(({const char*_tmp1AD="enum ";_tag_fat(_tmp1AD,sizeof(char),6U);}));_tmp1AC[0]=_tmp602;}),({struct Cyc_PP_Doc*_tmp601=Cyc_Absynpp_lb();_tmp1AC[1]=_tmp601;}),({struct Cyc_PP_Doc*_tmp600=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_tmp1AC[2]=_tmp600;}),({struct Cyc_PP_Doc*_tmp5FF=Cyc_Absynpp_rb();_tmp1AC[3]=_tmp5FF;});Cyc_PP_cat(_tag_fat(_tmp1AC,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1D: _tmp164=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1E: {struct _tuple0*n=_tmp164;
s=({struct Cyc_PP_Doc*_tmp1AE[2U];({struct Cyc_PP_Doc*_tmp604=Cyc_PP_text(({const char*_tmp1AF="enum ";_tag_fat(_tmp1AF,sizeof(char),6U);}));_tmp1AE[0]=_tmp604;}),({struct Cyc_PP_Doc*_tmp603=Cyc_Absynpp_qvar2doc(n);_tmp1AE[1]=_tmp603;});Cyc_PP_cat(_tag_fat(_tmp1AE,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 17U: _LL23: _tmp163=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL24: {struct _fat_ptr t=_tmp163;
# 834
s=Cyc_PP_text(t);goto _LL0;}case 3U: _LL27: _tmp162=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL28: {struct Cyc_List_List*ts=_tmp162;
# 844
s=({struct Cyc_PP_Doc*_tmp1B7[3U];({struct Cyc_PP_Doc*_tmp607=Cyc_PP_text(({const char*_tmp1B8="region_t<";_tag_fat(_tmp1B8,sizeof(char),10U);}));_tmp1B7[0]=_tmp607;}),({struct Cyc_PP_Doc*_tmp606=Cyc_Absynpp_rgn2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1B7[1]=_tmp606;}),({struct Cyc_PP_Doc*_tmp605=Cyc_PP_text(({const char*_tmp1B9=">";_tag_fat(_tmp1B9,sizeof(char),2U);}));_tmp1B7[2]=_tmp605;});Cyc_PP_cat(_tag_fat(_tmp1B7,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 4U: _LL29: _tmp161=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL2A: {struct Cyc_List_List*ts=_tmp161;
# 846
s=({struct Cyc_PP_Doc*_tmp1BA[3U];({struct Cyc_PP_Doc*_tmp60A=Cyc_PP_text(({const char*_tmp1BB="tag_t<";_tag_fat(_tmp1BB,sizeof(char),7U);}));_tmp1BA[0]=_tmp60A;}),({struct Cyc_PP_Doc*_tmp609=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1BA[1]=_tmp609;}),({struct Cyc_PP_Doc*_tmp608=Cyc_PP_text(({const char*_tmp1BC=">";_tag_fat(_tmp1BC,sizeof(char),2U);}));_tmp1BA[2]=_tmp608;});Cyc_PP_cat(_tag_fat(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: _LL31: _tmp160=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL32: {struct Cyc_List_List*ts=_tmp160;
# 851
s=({struct Cyc_PP_Doc*_tmp1BD[3U];({struct Cyc_PP_Doc*_tmp60D=Cyc_PP_text(({const char*_tmp1BE="regions(";_tag_fat(_tmp1BE,sizeof(char),9U);}));_tmp1BD[0]=_tmp60D;}),({struct Cyc_PP_Doc*_tmp60C=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1BD[1]=_tmp60C;}),({struct Cyc_PP_Doc*_tmp60B=Cyc_PP_text(({const char*_tmp1BF=")";_tag_fat(_tmp1BF,sizeof(char),2U);}));_tmp1BD[2]=_tmp60B;});Cyc_PP_cat(_tag_fat(_tmp1BD,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL33: _LL34:
 goto _LL36;case 9U: _LL35: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 857
 s=Cyc_PP_text(({const char*_tmp1C0="@true";_tag_fat(_tmp1C0,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
 s=Cyc_PP_text(({const char*_tmp1C1="@false";_tag_fat(_tmp1C1,sizeof(char),7U);}));goto _LL0;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2 != 0){_LL41: _tmp15F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2)->hd;_LL42: {void*t=_tmp15F;
# 860
s=({struct Cyc_PP_Doc*_tmp1C2[4U];({struct Cyc_PP_Doc*_tmp611=Cyc_PP_text(({const char*_tmp1C3="@thin @numelts";_tag_fat(_tmp1C3,sizeof(char),15U);}));_tmp1C2[0]=_tmp611;}),({struct Cyc_PP_Doc*_tmp610=Cyc_Absynpp_lb();_tmp1C2[1]=_tmp610;}),({struct Cyc_PP_Doc*_tmp60F=Cyc_Absynpp_typ2doc(t);_tmp1C2[2]=_tmp60F;}),({struct Cyc_PP_Doc*_tmp60E=Cyc_Absynpp_rb();_tmp1C2[3]=_tmp60E;});Cyc_PP_cat(_tag_fat(_tmp1C2,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}}else{_LL43: _LL44:
# 862
 s=Cyc_PP_text(({const char*_tmp1C4="@thin";_tag_fat(_tmp1C4,sizeof(char),6U);}));goto _LL0;}default: _LL45: _LL46:
 s=Cyc_PP_text(({const char*_tmp1C5="@fat";_tag_fat(_tmp1C5,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp15B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f1;_tmp15C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f2;_tmp15D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f3;_tmp15E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f4;_LLA: {struct Cyc_Core_Opt*k=_tmp15B;void*topt=_tmp15C;int i=_tmp15D;struct Cyc_Core_Opt*tvs=_tmp15E;
# 748
if(topt != 0)
# 750
return Cyc_Absynpp_ntyp2doc(topt);else{
# 752
struct _fat_ptr kindstring=k == 0?({const char*_tmp175="K";_tag_fat(_tmp175,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp57A;_tmp57A.tag=0U,_tmp57A.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_tmp57A;});struct Cyc_Int_pa_PrintArg_struct _tmp174=({struct Cyc_Int_pa_PrintArg_struct _tmp579;_tmp579.tag=1U,_tmp579.f1=(unsigned long)i;_tmp579;});void*_tmp171[2U];_tmp171[0]=& _tmp173,_tmp171[1]=& _tmp174;({struct _fat_ptr _tmp612=({const char*_tmp172="`E%s%d";_tag_fat(_tmp172,sizeof(char),7U);});Cyc_aprintf(_tmp612,_tag_fat(_tmp171,sizeof(void*),2U));});}));}
# 755
goto _LL0;}case 2U: _LLB: _tmp15A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->f1;_LLC: {struct Cyc_Absyn_Tvar*tv=_tmp15A;
# 757
s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp176[3U];_tmp176[0]=s,({struct Cyc_PP_Doc*_tmp614=Cyc_PP_text(({const char*_tmp177="::";_tag_fat(_tmp177,sizeof(char),3U);}));_tmp176[1]=_tmp614;}),({struct Cyc_PP_Doc*_tmp613=Cyc_Absynpp_kindbound2doc(tv->kind);_tmp176[2]=_tmp613;});Cyc_PP_cat(_tag_fat(_tmp176,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 6U: _LL15: _tmp159=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14E)->f1;_LL16: {struct Cyc_List_List*ts=_tmp159;
# 819
s=({struct Cyc_PP_Doc*_tmp1A5[2U];({struct Cyc_PP_Doc*_tmp616=Cyc_Absynpp_dollar();_tmp1A5[0]=_tmp616;}),({struct Cyc_PP_Doc*_tmp615=Cyc_Absynpp_args2doc(ts);_tmp1A5[1]=_tmp615;});Cyc_PP_cat(_tag_fat(_tmp1A5,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 7U: _LL19: _tmp157=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f1;_tmp158=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp157;struct Cyc_List_List*fs=_tmp158;
# 825
s=({struct Cyc_PP_Doc*_tmp1AB[4U];({struct Cyc_PP_Doc*_tmp61A=Cyc_Absynpp_aggr_kind2doc(k);_tmp1AB[0]=_tmp61A;}),({struct Cyc_PP_Doc*_tmp619=Cyc_Absynpp_lb();_tmp1AB[1]=_tmp619;}),({
struct Cyc_PP_Doc*_tmp618=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_tmp1AB[2]=_tmp618;}),({
struct Cyc_PP_Doc*_tmp617=Cyc_Absynpp_rb();_tmp1AB[3]=_tmp617;});Cyc_PP_cat(_tag_fat(_tmp1AB,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 9U: _LL1F: _tmp156=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp14E)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp156;
# 832
s=({struct Cyc_PP_Doc*_tmp1B0[3U];({struct Cyc_PP_Doc*_tmp61D=Cyc_PP_text(({const char*_tmp1B1="valueof_t(";_tag_fat(_tmp1B1,sizeof(char),11U);}));_tmp1B0[0]=_tmp61D;}),({struct Cyc_PP_Doc*_tmp61C=Cyc_Absynpp_exp2doc(e);_tmp1B0[1]=_tmp61C;}),({struct Cyc_PP_Doc*_tmp61B=Cyc_PP_text(({const char*_tmp1B2=")";_tag_fat(_tmp1B2,sizeof(char),2U);}));_tmp1B0[2]=_tmp61B;});Cyc_PP_cat(_tag_fat(_tmp1B0,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 11U: _LL21: _tmp155=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp14E)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp155;
s=({struct Cyc_PP_Doc*_tmp1B3[3U];({struct Cyc_PP_Doc*_tmp620=Cyc_PP_text(({const char*_tmp1B4="typeof(";_tag_fat(_tmp1B4,sizeof(char),8U);}));_tmp1B3[0]=_tmp620;}),({struct Cyc_PP_Doc*_tmp61F=Cyc_Absynpp_exp2doc(e);_tmp1B3[1]=_tmp61F;}),({struct Cyc_PP_Doc*_tmp61E=Cyc_PP_text(({const char*_tmp1B5=")";_tag_fat(_tmp1B5,sizeof(char),2U);}));_tmp1B3[2]=_tmp61E;});Cyc_PP_cat(_tag_fat(_tmp1B3,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL25: _tmp152=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f1;_tmp153=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f2;_tmp154=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f3;_LL26: {struct _tuple0*n=_tmp152;struct Cyc_List_List*ts=_tmp153;struct Cyc_Absyn_Typedefdecl*kopt=_tmp154;
# 841
s=({struct Cyc_PP_Doc*_tmp1B6[2U];({struct Cyc_PP_Doc*_tmp622=Cyc_Absynpp_qvar2doc(n);_tmp1B6[0]=_tmp622;}),({struct Cyc_PP_Doc*_tmp621=Cyc_Absynpp_tps2doc(ts);_tmp1B6[1]=_tmp621;});Cyc_PP_cat(_tag_fat(_tmp1B6,sizeof(struct Cyc_PP_Doc*),2U));});
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
return({struct Cyc_PP_Doc*_tmp1C9[3U];({struct Cyc_PP_Doc*_tmp625=Cyc_Absynpp_rgn2doc(r1);_tmp1C9[0]=_tmp625;}),({struct Cyc_PP_Doc*_tmp624=Cyc_PP_text(({const char*_tmp1CA=" > ";_tag_fat(_tmp1CA,sizeof(char),4U);}));_tmp1C9[1]=_tmp624;}),({struct Cyc_PP_Doc*_tmp623=Cyc_Absynpp_rgn2doc(r2);_tmp1C9[2]=_tmp623;});Cyc_PP_cat(_tag_fat(_tmp1C9,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 877
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _tmp628=({const char*_tmp1CB="";_tag_fat(_tmp1CB,sizeof(char),1U);});struct _fat_ptr _tmp627=({const char*_tmp1CC="";_tag_fat(_tmp1CC,sizeof(char),1U);});struct _fat_ptr _tmp626=({const char*_tmp1CD=",";_tag_fat(_tmp1CD,sizeof(char),2U);});Cyc_PP_group(_tmp628,_tmp627,_tmp626,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 881
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*_tmp1CE=(*t).f1;struct _fat_ptr*vo=_tmp1CE;
struct Cyc_Core_Opt*dopt=vo == 0?0:({struct Cyc_Core_Opt*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({struct Cyc_PP_Doc*_tmp629=Cyc_PP_text(*vo);_tmp1CF->v=_tmp629;});_tmp1CF;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 887
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 891
struct Cyc_List_List*_tmp1D0=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_List_List*arg_docs=_tmp1D0;
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_tmp62B=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp62B,({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));({struct Cyc_PP_Doc*_tmp62A=Cyc_PP_text(({const char*_tmp1D1="...";_tag_fat(_tmp1D1,sizeof(char),4U);}));_tmp1D2->hd=_tmp62A;}),_tmp1D2->tl=0;_tmp1D2;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1D3=({struct Cyc_PP_Doc*_tmp1D5[3U];({struct Cyc_PP_Doc*_tmp62F=Cyc_PP_text(({const char*_tmp1D6="...";_tag_fat(_tmp1D6,sizeof(char),4U);}));_tmp1D5[0]=_tmp62F;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp62E=Cyc_PP_text(({const char*_tmp1D7=" inject ";_tag_fat(_tmp1D7,sizeof(char),9U);}));_tmp1D5[1]=_tmp62E;}):({struct Cyc_PP_Doc*_tmp62D=Cyc_PP_text(({const char*_tmp1D8=" ";_tag_fat(_tmp1D8,sizeof(char),2U);}));_tmp1D5[1]=_tmp62D;}),({
struct Cyc_PP_Doc*_tmp62C=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->f1=cyc_varargs->name,_tmp1D9->f2=cyc_varargs->tq,_tmp1D9->f3=cyc_varargs->type;_tmp1D9;}));_tmp1D5[2]=_tmp62C;});Cyc_PP_cat(_tag_fat(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3U));});
# 896
struct Cyc_PP_Doc*varargs_doc=_tmp1D3;
# 900
arg_docs=({struct Cyc_List_List*_tmp630=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp630,({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=varargs_doc,_tmp1D4->tl=0;_tmp1D4;}));});}}{
# 902
struct Cyc_PP_Doc*_tmp1DA=({struct _fat_ptr _tmp633=({const char*_tmp1E9="";_tag_fat(_tmp1E9,sizeof(char),1U);});struct _fat_ptr _tmp632=({const char*_tmp1EA="";_tag_fat(_tmp1EA,sizeof(char),1U);});struct _fat_ptr _tmp631=({const char*_tmp1EB=",";_tag_fat(_tmp1EB,sizeof(char),2U);});Cyc_PP_group(_tmp633,_tmp632,_tmp631,arg_docs);});struct Cyc_PP_Doc*arg_doc=_tmp1DA;
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_tmp1DB[3U];_tmp1DB[0]=arg_doc,({struct Cyc_PP_Doc*_tmp635=Cyc_PP_text(({const char*_tmp1DC=";";_tag_fat(_tmp1DC,sizeof(char),2U);}));_tmp1DB[1]=_tmp635;}),({struct Cyc_PP_Doc*_tmp634=Cyc_Absynpp_eff2doc(effopt);_tmp1DB[2]=_tmp634;});Cyc_PP_cat(_tag_fat(_tmp1DB,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
arg_doc=({struct Cyc_PP_Doc*_tmp1DD[3U];_tmp1DD[0]=arg_doc,({struct Cyc_PP_Doc*_tmp637=Cyc_PP_text(({const char*_tmp1DE=":";_tag_fat(_tmp1DE,sizeof(char),2U);}));_tmp1DD[1]=_tmp637;}),({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1DD[2]=_tmp636;});Cyc_PP_cat(_tag_fat(_tmp1DD,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp1DF=({struct Cyc_PP_Doc*_tmp1E6[3U];({struct Cyc_PP_Doc*_tmp639=Cyc_PP_text(({const char*_tmp1E7="(";_tag_fat(_tmp1E7,sizeof(char),2U);}));_tmp1E6[0]=_tmp639;}),_tmp1E6[1]=arg_doc,({struct Cyc_PP_Doc*_tmp638=Cyc_PP_text(({const char*_tmp1E8=")";_tag_fat(_tmp1E8,sizeof(char),2U);}));_tmp1E6[2]=_tmp638;});Cyc_PP_cat(_tag_fat(_tmp1E6,sizeof(struct Cyc_PP_Doc*),3U));});struct Cyc_PP_Doc*res=_tmp1DF;
if(req != 0)
res=({struct Cyc_PP_Doc*_tmp1E0[4U];_tmp1E0[0]=res,({struct Cyc_PP_Doc*_tmp63C=Cyc_PP_text(({const char*_tmp1E1=" @requires(";_tag_fat(_tmp1E1,sizeof(char),12U);}));_tmp1E0[1]=_tmp63C;}),({struct Cyc_PP_Doc*_tmp63B=Cyc_Absynpp_exp2doc(req);_tmp1E0[2]=_tmp63B;}),({struct Cyc_PP_Doc*_tmp63A=Cyc_PP_text(({const char*_tmp1E2=")";_tag_fat(_tmp1E2,sizeof(char),2U);}));_tmp1E0[3]=_tmp63A;});Cyc_PP_cat(_tag_fat(_tmp1E0,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
res=({struct Cyc_PP_Doc*_tmp1E3[4U];_tmp1E3[0]=res,({struct Cyc_PP_Doc*_tmp63F=Cyc_PP_text(({const char*_tmp1E4=" @ensures(";_tag_fat(_tmp1E4,sizeof(char),11U);}));_tmp1E3[1]=_tmp63F;}),({struct Cyc_PP_Doc*_tmp63E=Cyc_Absynpp_exp2doc(ens);_tmp1E3[2]=_tmp63E;}),({struct Cyc_PP_Doc*_tmp63D=Cyc_PP_text(({const char*_tmp1E5=")";_tag_fat(_tmp1E5,sizeof(char),2U);}));_tmp1E3[3]=_tmp63D;});Cyc_PP_cat(_tag_fat(_tmp1E3,sizeof(struct Cyc_PP_Doc*),4U));});
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
return(struct _fat_ptr)({struct Cyc_List_List*_tmp641=({struct Cyc_List_List*_tmp640=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp640,({struct Cyc_List_List*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3->hd=(*q).f2,_tmp1F3->tl=0;_tmp1F3;}));});
# 936
Cyc_str_sepstr(_tmp641,({const char*_tmp1F4="_";_tag_fat(_tmp1F4,sizeof(char),2U);}));});else{
# 940
if(match)
return*(*q).f2;else{
# 943
return(struct _fat_ptr)({struct Cyc_List_List*_tmp643=({struct Cyc_List_List*_tmp642=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp642,({struct Cyc_List_List*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=(*q).f2,_tmp1F5->tl=0;_tmp1F5;}));});Cyc_str_sepstr(_tmp643,({const char*_tmp1F6="::";_tag_fat(_tmp1F6,sizeof(char),3U);}));});}}}
# 947
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 951
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr _tmp1F7=Cyc_Absynpp_qvar2string(q);struct _fat_ptr qs=_tmp1F7;
if(Cyc_PP_tex_output)
# 955
return({struct _fat_ptr _tmp646=(struct _fat_ptr)({struct _fat_ptr _tmp645=(struct _fat_ptr)({struct _fat_ptr _tmp644=({const char*_tmp1F8="\\textbf{";_tag_fat(_tmp1F8,sizeof(char),9U);});Cyc_strconcat(_tmp644,(struct _fat_ptr)qs);});Cyc_strconcat(_tmp645,({const char*_tmp1F9="}";_tag_fat(_tmp1F9,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp646,(int)
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
 return(struct _fat_ptr)({struct _fat_ptr _tmp647=({const char*_tmp1FE="/* bad namespace : */ ";_tag_fat(_tmp1FE,sizeof(char),23U);});Cyc_strconcat(_tmp647,(struct _fat_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1FD=(_tmp1FB.C_n).val;_LL6: {struct Cyc_List_List*l=_tmp1FD;
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
return({struct _fat_ptr _tmp64A=(struct _fat_ptr)({struct _fat_ptr _tmp649=(struct _fat_ptr)({struct _fat_ptr _tmp648=({const char*_tmp200="\\textbf{";_tag_fat(_tmp200,sizeof(char),9U);});Cyc_strconcat(_tmp648,(struct _fat_ptr)vs);});Cyc_strconcat(_tmp649,({const char*_tmp201="}";_tag_fat(_tmp201,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp64A,(int)
Cyc_strlen((struct _fat_ptr)vs));});else{
return Cyc_PP_text(vs);}}
# 994
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp64B=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp64B,t,0);});}
# 998
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_tmp202=f;unsigned _tmp203;struct _fat_ptr*_tmp204;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->tag == 0U){_LL1: _tmp204=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->f1;_LL2: {struct _fat_ptr*n=_tmp204;
return Cyc_PP_textptr(n);}}else{_LL3: _tmp203=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp202)->f1;_LL4: {unsigned n=_tmp203;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp207=({struct Cyc_Int_pa_PrintArg_struct _tmp57B;_tmp57B.tag=1U,_tmp57B.f1=(unsigned long)((int)n);_tmp57B;});void*_tmp205[1U];_tmp205[0]=& _tmp207;({struct _fat_ptr _tmp64C=({const char*_tmp206="%d";_tag_fat(_tmp206,sizeof(char),3U);});Cyc_aprintf(_tmp64C,_tag_fat(_tmp205,sizeof(void*),1U));});}));}}_LL0:;}
# 1012 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp208=e->r;void*_stmttmp14=_tmp208;void*_tmp209=_stmttmp14;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_Absyn_Exp*_tmp20B;enum Cyc_Absyn_Primop _tmp20C;switch(*((int*)_tmp209)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp20C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL6: {enum Cyc_Absyn_Primop p=_tmp20C;
# 1017
enum Cyc_Absyn_Primop _tmp20D=p;switch(_tmp20D){case Cyc_Absyn_Plus: _LL58: _LL59:
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
# 1082
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct _tuple19{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1086
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
struct _tuple19*_tmp20E=(struct _tuple19*)o->hd;struct _tuple19*_stmttmp15=_tmp20E;struct _tuple19*_tmp20F=_stmttmp15;struct Cyc_Absyn_Exp*_tmp211;struct _fat_ptr _tmp210;_LL1: _tmp210=_tmp20F->f1;_tmp211=_tmp20F->f2;_LL2: {struct _fat_ptr c=_tmp210;struct Cyc_Absyn_Exp*e=_tmp211;
s=({struct Cyc_PP_Doc*_tmp212[5U];({struct Cyc_PP_Doc*_tmp651=Cyc_PP_text(({const char*_tmp213="\"";_tag_fat(_tmp213,sizeof(char),2U);}));_tmp212[0]=_tmp651;}),({struct Cyc_PP_Doc*_tmp650=Cyc_PP_text(c);_tmp212[1]=_tmp650;}),({struct Cyc_PP_Doc*_tmp64F=Cyc_PP_text(({const char*_tmp214="\" (";_tag_fat(_tmp214,sizeof(char),4U);}));_tmp212[2]=_tmp64F;}),({struct Cyc_PP_Doc*_tmp64E=Cyc_Absynpp_exp2doc(e);_tmp212[3]=_tmp64E;}),({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_text(({const char*_tmp215=")";_tag_fat(_tmp215,sizeof(char),2U);}));_tmp212[4]=_tmp64D;});Cyc_PP_cat(_tag_fat(_tmp212,sizeof(struct Cyc_PP_Doc*),5U));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp216[2U];_tmp216[0]=s,({struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp217=",";_tag_fat(_tmp217,sizeof(char),2U);}));_tmp216[1]=_tmp652;});Cyc_PP_cat(_tag_fat(_tmp216,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1096
return s;}
# 1099
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp218[2U];({struct Cyc_PP_Doc*_tmp654=Cyc_PP_text(({const char*_tmp219=": ";_tag_fat(_tmp219,sizeof(char),3U);}));_tmp218[0]=_tmp654;}),({struct Cyc_PP_Doc*_tmp653=Cyc_Absynpp_asm_iolist_doc_in(o);_tmp218[1]=_tmp653;});Cyc_PP_cat(_tag_fat(_tmp218,sizeof(struct Cyc_PP_Doc*),2U));});
# 1104
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_tmp21A[3U];_tmp21A[0]=s,({struct Cyc_PP_Doc*_tmp656=Cyc_PP_text(({const char*_tmp21B=": : ";_tag_fat(_tmp21B,sizeof(char),5U);}));_tmp21A[1]=_tmp656;}),({struct Cyc_PP_Doc*_tmp655=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp21A[2]=_tmp655;});Cyc_PP_cat(_tag_fat(_tmp21A,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1108
s=({struct Cyc_PP_Doc*_tmp21C[3U];_tmp21C[0]=s,({struct Cyc_PP_Doc*_tmp658=Cyc_PP_text(({const char*_tmp21D=" : ";_tag_fat(_tmp21D,sizeof(char),4U);}));_tmp21C[1]=_tmp658;}),({struct Cyc_PP_Doc*_tmp657=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp21C[2]=_tmp657;});Cyc_PP_cat(_tag_fat(_tmp21C,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1110
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_tmp21E[2U];_tmp21E[0]=s,ncol == 0?({struct Cyc_PP_Doc*_tmp65B=Cyc_PP_text(({const char*_tmp21F=": : :";_tag_fat(_tmp21F,sizeof(char),6U);}));_tmp21E[1]=_tmp65B;}):(ncol == 1?({struct Cyc_PP_Doc*_tmp65A=Cyc_PP_text(({const char*_tmp220=" : : ";_tag_fat(_tmp220,sizeof(char),6U);}));_tmp21E[1]=_tmp65A;}):({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp221=" : ";_tag_fat(_tmp221,sizeof(char),4U);}));_tmp21E[1]=_tmp659;}));Cyc_PP_cat(_tag_fat(_tmp21E,sizeof(struct Cyc_PP_Doc*),2U));});
while(cl != 0){
s=({struct Cyc_PP_Doc*_tmp222[4U];_tmp222[0]=s,({struct Cyc_PP_Doc*_tmp65E=Cyc_PP_text(({const char*_tmp223="\"";_tag_fat(_tmp223,sizeof(char),2U);}));_tmp222[1]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_tmp222[2]=_tmp65D;}),({struct Cyc_PP_Doc*_tmp65C=Cyc_PP_text(({const char*_tmp224="\"";_tag_fat(_tmp224,sizeof(char),2U);}));_tmp222[3]=_tmp65C;});Cyc_PP_cat(_tag_fat(_tmp222,sizeof(struct Cyc_PP_Doc*),4U));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_tmp225[2U];_tmp225[0]=s,({struct Cyc_PP_Doc*_tmp65F=Cyc_PP_text(({const char*_tmp226=", ";_tag_fat(_tmp226,sizeof(char),3U);}));_tmp225[1]=_tmp65F;});Cyc_PP_cat(_tag_fat(_tmp225,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1120
return s;}
# 1123
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp227=e->r;void*_stmttmp16=_tmp227;void*_tmp228=_stmttmp16;struct Cyc_Absyn_Stmt*_tmp229;struct Cyc_List_List*_tmp22B;struct Cyc_Core_Opt*_tmp22A;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp22C;int _tmp232;struct Cyc_Absyn_Exp*_tmp231;void**_tmp230;struct Cyc_Absyn_Exp*_tmp22F;int _tmp22E;struct Cyc_Absyn_Enumfield*_tmp233;struct Cyc_Absyn_Enumfield*_tmp234;struct Cyc_Absyn_Datatypefield*_tmp236;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp237;struct Cyc_List_List*_tmp23A;struct Cyc_List_List*_tmp239;struct _tuple0*_tmp238;void*_tmp23C;struct Cyc_Absyn_Exp*_tmp23B;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Vardecl*_tmp23D;struct Cyc_List_List*_tmp240;struct Cyc_List_List*_tmp242;struct _tuple8*_tmp241;struct Cyc_List_List*_tmp243;struct Cyc_Absyn_Exp*_tmp245;struct Cyc_Absyn_Exp*_tmp244;struct _fat_ptr*_tmp247;struct Cyc_Absyn_Exp*_tmp246;struct _fat_ptr*_tmp249;struct Cyc_Absyn_Exp*_tmp248;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_List_List*_tmp24C;void*_tmp24B;struct _fat_ptr*_tmp24E;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp252;struct Cyc_List_List*_tmp251;struct _fat_ptr _tmp250;int _tmp24F;void*_tmp254;struct Cyc_Absyn_Exp*_tmp255;struct Cyc_Absyn_Exp*_tmp256;struct Cyc_Absyn_Exp*_tmp257;void*_tmp258;struct Cyc_Absyn_Exp*_tmp25A;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*_tmp25D;void*_tmp25C;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Exp*_tmp25F;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_List_List*_tmp262;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*_tmp268;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*_tmp269;enum Cyc_Absyn_Incrementor _tmp26D;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Core_Opt*_tmp26F;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_List_List*_tmp272;enum Cyc_Absyn_Primop _tmp271;struct _fat_ptr _tmp273;void*_tmp274;union Cyc_Absyn_Cnst _tmp275;switch(*((int*)_tmp228)){case 0U: _LL1: _tmp275=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp275;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1U: _LL3: _tmp274=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL4: {void*b=_tmp274;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL5: _tmp273=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL6: {struct _fat_ptr p=_tmp273;
# 1130
s=({struct Cyc_PP_Doc*_tmp276[4U];({struct Cyc_PP_Doc*_tmp663=Cyc_PP_text(({const char*_tmp277="__cyclone_pragma__";_tag_fat(_tmp277,sizeof(char),19U);}));_tmp276[0]=_tmp663;}),({struct Cyc_PP_Doc*_tmp662=Cyc_PP_text(({const char*_tmp278="(";_tag_fat(_tmp278,sizeof(char),2U);}));_tmp276[1]=_tmp662;}),({struct Cyc_PP_Doc*_tmp661=Cyc_PP_text(p);_tmp276[2]=_tmp661;}),({struct Cyc_PP_Doc*_tmp660=Cyc_PP_text(({const char*_tmp279=")";_tag_fat(_tmp279,sizeof(char),2U);}));_tmp276[3]=_tmp660;});Cyc_PP_cat(_tag_fat(_tmp276,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 3U: _LL7: _tmp271=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp272=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp271;struct Cyc_List_List*es=_tmp272;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4U: _LL9: _tmp26E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp26F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp270=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp26E;struct Cyc_Core_Opt*popt=_tmp26F;struct Cyc_Absyn_Exp*e2=_tmp270;
# 1133
s=({struct Cyc_PP_Doc*_tmp27A[5U];({struct Cyc_PP_Doc*_tmp669=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp27A[0]=_tmp669;}),({
struct Cyc_PP_Doc*_tmp668=Cyc_PP_text(({const char*_tmp27B=" ";_tag_fat(_tmp27B,sizeof(char),2U);}));_tmp27A[1]=_tmp668;}),
popt == 0?({struct Cyc_PP_Doc*_tmp667=Cyc_PP_nil_doc();_tmp27A[2]=_tmp667;}):({struct Cyc_PP_Doc*_tmp666=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_tmp27A[2]=_tmp666;}),({
struct Cyc_PP_Doc*_tmp665=Cyc_PP_text(({const char*_tmp27C="= ";_tag_fat(_tmp27C,sizeof(char),3U);}));_tmp27A[3]=_tmp665;}),({
struct Cyc_PP_Doc*_tmp664=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp27A[4]=_tmp664;});Cyc_PP_cat(_tag_fat(_tmp27A,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp26C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp26D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LLC: {struct Cyc_Absyn_Exp*e2=_tmp26C;enum Cyc_Absyn_Incrementor i=_tmp26D;
# 1140
struct Cyc_PP_Doc*_tmp27D=Cyc_Absynpp_exp2doc_prec(myprec,e2);struct Cyc_PP_Doc*es=_tmp27D;
{enum Cyc_Absyn_Incrementor _tmp27E=i;switch(_tmp27E){case Cyc_Absyn_PreInc: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp27F[2U];({struct Cyc_PP_Doc*_tmp66A=Cyc_PP_text(({const char*_tmp280="++";_tag_fat(_tmp280,sizeof(char),3U);}));_tmp27F[0]=_tmp66A;}),_tmp27F[1]=es;Cyc_PP_cat(_tag_fat(_tmp27F,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PreDec: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp281[2U];({struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp282="--";_tag_fat(_tmp282,sizeof(char),3U);}));_tmp281[0]=_tmp66B;}),_tmp281[1]=es;Cyc_PP_cat(_tag_fat(_tmp281,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PostInc: _LL5C: _LL5D:
 s=({struct Cyc_PP_Doc*_tmp283[2U];_tmp283[0]=es,({struct Cyc_PP_Doc*_tmp66C=Cyc_PP_text(({const char*_tmp284="++";_tag_fat(_tmp284,sizeof(char),3U);}));_tmp283[1]=_tmp66C;});Cyc_PP_cat(_tag_fat(_tmp283,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;case Cyc_Absyn_PostDec: _LL5E: _LL5F:
 goto _LL61;default: _LL60: _LL61:
 s=({struct Cyc_PP_Doc*_tmp285[2U];_tmp285[0]=es,({struct Cyc_PP_Doc*_tmp66D=Cyc_PP_text(({const char*_tmp286="--";_tag_fat(_tmp286,sizeof(char),3U);}));_tmp285[1]=_tmp66D;});Cyc_PP_cat(_tag_fat(_tmp285,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL57;}_LL57:;}
# 1148
goto _LL0;}case 6U: _LLD: _tmp269=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp26A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp26B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp269;struct Cyc_Absyn_Exp*e2=_tmp26A;struct Cyc_Absyn_Exp*e3=_tmp26B;
# 1150
s=({struct Cyc_PP_Doc*_tmp287[5U];({struct Cyc_PP_Doc*_tmp672=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp287[0]=_tmp672;}),({struct Cyc_PP_Doc*_tmp671=Cyc_PP_text(({const char*_tmp288=" ? ";_tag_fat(_tmp288,sizeof(char),4U);}));_tmp287[1]=_tmp671;}),({struct Cyc_PP_Doc*_tmp670=Cyc_Absynpp_exp2doc_prec(0,e2);_tmp287[2]=_tmp670;}),({
struct Cyc_PP_Doc*_tmp66F=Cyc_PP_text(({const char*_tmp289=" : ";_tag_fat(_tmp289,sizeof(char),4U);}));_tmp287[3]=_tmp66F;}),({struct Cyc_PP_Doc*_tmp66E=Cyc_Absynpp_exp2doc_prec(myprec,e3);_tmp287[4]=_tmp66E;});Cyc_PP_cat(_tag_fat(_tmp287,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 7U: _LLF: _tmp267=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp268=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp267;struct Cyc_Absyn_Exp*e2=_tmp268;
# 1154
s=({struct Cyc_PP_Doc*_tmp28A[3U];({struct Cyc_PP_Doc*_tmp675=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp28A[0]=_tmp675;}),({struct Cyc_PP_Doc*_tmp674=Cyc_PP_text(({const char*_tmp28B=" && ";_tag_fat(_tmp28B,sizeof(char),5U);}));_tmp28A[1]=_tmp674;}),({struct Cyc_PP_Doc*_tmp673=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp28A[2]=_tmp673;});Cyc_PP_cat(_tag_fat(_tmp28A,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 8U: _LL11: _tmp265=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp266=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp265;struct Cyc_Absyn_Exp*e2=_tmp266;
# 1157
s=({struct Cyc_PP_Doc*_tmp28C[3U];({struct Cyc_PP_Doc*_tmp678=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp28C[0]=_tmp678;}),({struct Cyc_PP_Doc*_tmp677=Cyc_PP_text(({const char*_tmp28D=" || ";_tag_fat(_tmp28D,sizeof(char),5U);}));_tmp28C[1]=_tmp677;}),({struct Cyc_PP_Doc*_tmp676=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp28C[2]=_tmp676;});Cyc_PP_cat(_tag_fat(_tmp28C,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 9U: _LL13: _tmp263=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp264=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp263;struct Cyc_Absyn_Exp*e2=_tmp264;
# 1162
s=({struct Cyc_PP_Doc*_tmp28E[3U];({struct Cyc_PP_Doc*_tmp67B=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_tmp28E[0]=_tmp67B;}),({struct Cyc_PP_Doc*_tmp67A=Cyc_PP_text(({const char*_tmp28F=", ";_tag_fat(_tmp28F,sizeof(char),3U);}));_tmp28E[1]=_tmp67A;}),({struct Cyc_PP_Doc*_tmp679=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_tmp28E[2]=_tmp679;});Cyc_PP_cat(_tag_fat(_tmp28E,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 10U: _LL15: _tmp261=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp262=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp261;struct Cyc_List_List*es=_tmp262;
# 1165
s=({struct Cyc_PP_Doc*_tmp290[4U];({struct Cyc_PP_Doc*_tmp67F=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp290[0]=_tmp67F;}),({
struct Cyc_PP_Doc*_tmp67E=Cyc_PP_text(({const char*_tmp291="(";_tag_fat(_tmp291,sizeof(char),2U);}));_tmp290[1]=_tmp67E;}),({
struct Cyc_PP_Doc*_tmp67D=Cyc_Absynpp_exps2doc_prec(20,es);_tmp290[2]=_tmp67D;}),({
struct Cyc_PP_Doc*_tmp67C=Cyc_PP_text(({const char*_tmp292=")";_tag_fat(_tmp292,sizeof(char),2U);}));_tmp290[3]=_tmp67C;});Cyc_PP_cat(_tag_fat(_tmp290,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 11U: _LL17: _tmp260=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp260;
s=({struct Cyc_PP_Doc*_tmp293[2U];({struct Cyc_PP_Doc*_tmp681=Cyc_PP_text(({const char*_tmp294="throw ";_tag_fat(_tmp294,sizeof(char),7U);}));_tmp293[0]=_tmp681;}),({struct Cyc_PP_Doc*_tmp680=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp293[1]=_tmp680;});Cyc_PP_cat(_tag_fat(_tmp293,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 12U: _LL19: _tmp25F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp25F;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13U: _LL1B: _tmp25E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp25E;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14U: _LL1D: _tmp25C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp25D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL1E: {void*t=_tmp25C;struct Cyc_Absyn_Exp*e1=_tmp25D;
# 1174
s=({struct Cyc_PP_Doc*_tmp295[4U];({struct Cyc_PP_Doc*_tmp685=Cyc_PP_text(({const char*_tmp296="(";_tag_fat(_tmp296,sizeof(char),2U);}));_tmp295[0]=_tmp685;}),({struct Cyc_PP_Doc*_tmp684=Cyc_Absynpp_typ2doc(t);_tmp295[1]=_tmp684;}),({struct Cyc_PP_Doc*_tmp683=Cyc_PP_text(({const char*_tmp297=")";_tag_fat(_tmp297,sizeof(char),2U);}));_tmp295[2]=_tmp683;}),({struct Cyc_PP_Doc*_tmp682=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp295[3]=_tmp682;});Cyc_PP_cat(_tag_fat(_tmp295,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1F: _tmp25B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp25B;
s=({struct Cyc_PP_Doc*_tmp298[2U];({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp299="&";_tag_fat(_tmp299,sizeof(char),2U);}));_tmp298[0]=_tmp687;}),({struct Cyc_PP_Doc*_tmp686=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp298[1]=_tmp686;});Cyc_PP_cat(_tag_fat(_tmp298,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 16U: _LL21: _tmp259=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp25A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL22: {struct Cyc_Absyn_Exp*ropt=_tmp259;struct Cyc_Absyn_Exp*e1=_tmp25A;
# 1177
if(ropt == 0)
s=({struct Cyc_PP_Doc*_tmp29A[2U];({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp29B="new ";_tag_fat(_tmp29B,sizeof(char),5U);}));_tmp29A[0]=_tmp689;}),({struct Cyc_PP_Doc*_tmp688=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp29A[1]=_tmp688;});Cyc_PP_cat(_tag_fat(_tmp29A,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1180
s=({struct Cyc_PP_Doc*_tmp29C[4U];({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp29D="rnew(";_tag_fat(_tmp29D,sizeof(char),6U);}));_tmp29C[0]=_tmp68D;}),({struct Cyc_PP_Doc*_tmp68C=Cyc_Absynpp_exp2doc(ropt);_tmp29C[1]=_tmp68C;}),({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp29E=") ";_tag_fat(_tmp29E,sizeof(char),3U);}));_tmp29C[2]=_tmp68B;}),({struct Cyc_PP_Doc*_tmp68A=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp29C[3]=_tmp68A;});Cyc_PP_cat(_tag_fat(_tmp29C,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;}case 17U: _LL23: _tmp258=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL24: {void*t=_tmp258;
s=({struct Cyc_PP_Doc*_tmp29F[3U];({struct Cyc_PP_Doc*_tmp690=Cyc_PP_text(({const char*_tmp2A0="sizeof(";_tag_fat(_tmp2A0,sizeof(char),8U);}));_tmp29F[0]=_tmp690;}),({struct Cyc_PP_Doc*_tmp68F=Cyc_Absynpp_typ2doc(t);_tmp29F[1]=_tmp68F;}),({struct Cyc_PP_Doc*_tmp68E=Cyc_PP_text(({const char*_tmp2A1=")";_tag_fat(_tmp2A1,sizeof(char),2U);}));_tmp29F[2]=_tmp68E;});Cyc_PP_cat(_tag_fat(_tmp29F,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 18U: _LL25: _tmp257=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp257;
s=({struct Cyc_PP_Doc*_tmp2A2[3U];({struct Cyc_PP_Doc*_tmp693=Cyc_PP_text(({const char*_tmp2A3="sizeof(";_tag_fat(_tmp2A3,sizeof(char),8U);}));_tmp2A2[0]=_tmp693;}),({struct Cyc_PP_Doc*_tmp692=Cyc_Absynpp_exp2doc(e1);_tmp2A2[1]=_tmp692;}),({struct Cyc_PP_Doc*_tmp691=Cyc_PP_text(({const char*_tmp2A4=")";_tag_fat(_tmp2A4,sizeof(char),2U);}));_tmp2A2[2]=_tmp691;});Cyc_PP_cat(_tag_fat(_tmp2A2,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 42U: _LL27: _tmp256=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp256;
# 1185
s=({struct Cyc_PP_Doc*_tmp2A5[3U];({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp2A6="@assert(";_tag_fat(_tmp2A6,sizeof(char),9U);}));_tmp2A5[0]=_tmp696;}),({struct Cyc_PP_Doc*_tmp695=Cyc_Absynpp_exp2doc(e);_tmp2A5[1]=_tmp695;}),({struct Cyc_PP_Doc*_tmp694=Cyc_PP_text(({const char*_tmp2A7=")";_tag_fat(_tmp2A7,sizeof(char),2U);}));_tmp2A5[2]=_tmp694;});Cyc_PP_cat(_tag_fat(_tmp2A5,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 41U: _LL29: _tmp255=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp255;
# 1187
s=({struct Cyc_PP_Doc*_tmp2A8[3U];({struct Cyc_PP_Doc*_tmp699=Cyc_PP_text(({const char*_tmp2A9="__extension__(";_tag_fat(_tmp2A9,sizeof(char),15U);}));_tmp2A8[0]=_tmp699;}),({struct Cyc_PP_Doc*_tmp698=Cyc_Absynpp_exp2doc(e);_tmp2A8[1]=_tmp698;}),({struct Cyc_PP_Doc*_tmp697=Cyc_PP_text(({const char*_tmp2AA=")";_tag_fat(_tmp2AA,sizeof(char),2U);}));_tmp2A8[2]=_tmp697;});Cyc_PP_cat(_tag_fat(_tmp2A8,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 39U: _LL2B: _tmp254=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL2C: {void*t=_tmp254;
s=({struct Cyc_PP_Doc*_tmp2AB[3U];({struct Cyc_PP_Doc*_tmp69C=Cyc_PP_text(({const char*_tmp2AC="valueof(";_tag_fat(_tmp2AC,sizeof(char),9U);}));_tmp2AB[0]=_tmp69C;}),({struct Cyc_PP_Doc*_tmp69B=Cyc_Absynpp_typ2doc(t);_tmp2AB[1]=_tmp69B;}),({struct Cyc_PP_Doc*_tmp69A=Cyc_PP_text(({const char*_tmp2AD=")";_tag_fat(_tmp2AD,sizeof(char),2U);}));_tmp2AB[2]=_tmp69A;});Cyc_PP_cat(_tag_fat(_tmp2AB,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 40U: _LL2D: _tmp24F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp250=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp251=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_tmp252=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f4;_tmp253=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp228)->f5;_LL2E: {int vol=_tmp24F;struct _fat_ptr t=_tmp250;struct Cyc_List_List*o=_tmp251;struct Cyc_List_List*i=_tmp252;struct Cyc_List_List*cl=_tmp253;
# 1190
if(vol)
s=({struct Cyc_PP_Doc*_tmp2AE[7U];({struct Cyc_PP_Doc*_tmp6A3=Cyc_PP_text(({const char*_tmp2AF="__asm__";_tag_fat(_tmp2AF,sizeof(char),8U);}));_tmp2AE[0]=_tmp6A3;}),({struct Cyc_PP_Doc*_tmp6A2=Cyc_PP_text(({const char*_tmp2B0=" volatile (";_tag_fat(_tmp2B0,sizeof(char),12U);}));_tmp2AE[1]=_tmp6A2;}),({struct Cyc_PP_Doc*_tmp6A1=Cyc_PP_text(({const char*_tmp2B1="\"";_tag_fat(_tmp2B1,sizeof(char),2U);}));_tmp2AE[2]=_tmp6A1;}),({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(t);_tmp2AE[3]=_tmp6A0;}),({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_text(({const char*_tmp2B2="\"";_tag_fat(_tmp2B2,sizeof(char),2U);}));_tmp2AE[4]=_tmp69F;}),({struct Cyc_PP_Doc*_tmp69E=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2AE[5]=_tmp69E;}),({struct Cyc_PP_Doc*_tmp69D=Cyc_PP_text(({const char*_tmp2B3=")";_tag_fat(_tmp2B3,sizeof(char),2U);}));_tmp2AE[6]=_tmp69D;});Cyc_PP_cat(_tag_fat(_tmp2AE,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1193
s=({struct Cyc_PP_Doc*_tmp2B4[6U];({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_text(({const char*_tmp2B5="__asm__(";_tag_fat(_tmp2B5,sizeof(char),9U);}));_tmp2B4[0]=_tmp6A9;}),({struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_text(({const char*_tmp2B6="\"";_tag_fat(_tmp2B6,sizeof(char),2U);}));_tmp2B4[1]=_tmp6A8;}),({struct Cyc_PP_Doc*_tmp6A7=Cyc_PP_text(t);_tmp2B4[2]=_tmp6A7;}),({struct Cyc_PP_Doc*_tmp6A6=Cyc_PP_text(({const char*_tmp2B7="\"";_tag_fat(_tmp2B7,sizeof(char),2U);}));_tmp2B4[3]=_tmp6A6;}),({struct Cyc_PP_Doc*_tmp6A5=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp2B4[4]=_tmp6A5;}),({struct Cyc_PP_Doc*_tmp6A4=Cyc_PP_text(({const char*_tmp2B8=")";_tag_fat(_tmp2B8,sizeof(char),2U);}));_tmp2B4[5]=_tmp6A4;});Cyc_PP_cat(_tag_fat(_tmp2B4,sizeof(struct Cyc_PP_Doc*),6U));});}
goto _LL0;}case 38U: _LL2F: _tmp24D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp24E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL30: {struct Cyc_Absyn_Exp*e=_tmp24D;struct _fat_ptr*f=_tmp24E;
# 1196
s=({struct Cyc_PP_Doc*_tmp2B9[5U];({struct Cyc_PP_Doc*_tmp6AE=Cyc_PP_text(({const char*_tmp2BA="tagcheck(";_tag_fat(_tmp2BA,sizeof(char),10U);}));_tmp2B9[0]=_tmp6AE;}),({struct Cyc_PP_Doc*_tmp6AD=Cyc_Absynpp_exp2doc(e);_tmp2B9[1]=_tmp6AD;}),({struct Cyc_PP_Doc*_tmp6AC=Cyc_PP_text(({const char*_tmp2BB=".";_tag_fat(_tmp2BB,sizeof(char),2U);}));_tmp2B9[2]=_tmp6AC;}),({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_textptr(f);_tmp2B9[3]=_tmp6AB;}),({struct Cyc_PP_Doc*_tmp6AA=Cyc_PP_text(({const char*_tmp2BC=")";_tag_fat(_tmp2BC,sizeof(char),2U);}));_tmp2B9[4]=_tmp6AA;});Cyc_PP_cat(_tag_fat(_tmp2B9,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 19U: _LL31: _tmp24B=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp24C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL32: {void*t=_tmp24B;struct Cyc_List_List*l=_tmp24C;
# 1199
s=({struct Cyc_PP_Doc*_tmp2BD[5U];({struct Cyc_PP_Doc*_tmp6B4=Cyc_PP_text(({const char*_tmp2BE="offsetof(";_tag_fat(_tmp2BE,sizeof(char),10U);}));_tmp2BD[0]=_tmp6B4;}),({struct Cyc_PP_Doc*_tmp6B3=Cyc_Absynpp_typ2doc(t);_tmp2BD[1]=_tmp6B3;}),({struct Cyc_PP_Doc*_tmp6B2=Cyc_PP_text(({const char*_tmp2BF=",";_tag_fat(_tmp2BF,sizeof(char),2U);}));_tmp2BD[2]=_tmp6B2;}),({
struct Cyc_PP_Doc*_tmp6B1=({struct _fat_ptr _tmp6B0=({const char*_tmp2C0=".";_tag_fat(_tmp2C0,sizeof(char),2U);});Cyc_PP_seq(_tmp6B0,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_offsetof_field_to_doc,l));});_tmp2BD[3]=_tmp6B1;}),({struct Cyc_PP_Doc*_tmp6AF=Cyc_PP_text(({const char*_tmp2C1=")";_tag_fat(_tmp2C1,sizeof(char),2U);}));_tmp2BD[4]=_tmp6AF;});Cyc_PP_cat(_tag_fat(_tmp2BD,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 20U: _LL33: _tmp24A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp24A;
s=({struct Cyc_PP_Doc*_tmp2C2[2U];({struct Cyc_PP_Doc*_tmp6B6=Cyc_PP_text(({const char*_tmp2C3="*";_tag_fat(_tmp2C3,sizeof(char),2U);}));_tmp2C2[0]=_tmp6B6;}),({struct Cyc_PP_Doc*_tmp6B5=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C2[1]=_tmp6B5;});Cyc_PP_cat(_tag_fat(_tmp2C2,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 21U: _LL35: _tmp248=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp249=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp248;struct _fat_ptr*n=_tmp249;
# 1204
s=({struct Cyc_PP_Doc*_tmp2C4[3U];({struct Cyc_PP_Doc*_tmp6B9=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C4[0]=_tmp6B9;}),({struct Cyc_PP_Doc*_tmp6B8=Cyc_PP_text(({const char*_tmp2C5=".";_tag_fat(_tmp2C5,sizeof(char),2U);}));_tmp2C4[1]=_tmp6B8;}),({struct Cyc_PP_Doc*_tmp6B7=Cyc_PP_textptr(n);_tmp2C4[2]=_tmp6B7;});Cyc_PP_cat(_tag_fat(_tmp2C4,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 22U: _LL37: _tmp246=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp247=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp246;struct _fat_ptr*n=_tmp247;
# 1206
s=({struct Cyc_PP_Doc*_tmp2C6[3U];({struct Cyc_PP_Doc*_tmp6BC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C6[0]=_tmp6BC;}),({struct Cyc_PP_Doc*_tmp6BB=Cyc_PP_text(({const char*_tmp2C7="->";_tag_fat(_tmp2C7,sizeof(char),3U);}));_tmp2C6[1]=_tmp6BB;}),({struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_textptr(n);_tmp2C6[2]=_tmp6BA;});Cyc_PP_cat(_tag_fat(_tmp2C6,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 23U: _LL39: _tmp244=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp245=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp244;struct Cyc_Absyn_Exp*e2=_tmp245;
# 1208
s=({struct Cyc_PP_Doc*_tmp2C8[4U];({struct Cyc_PP_Doc*_tmp6C0=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2C8[0]=_tmp6C0;}),({struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_text(({const char*_tmp2C9="[";_tag_fat(_tmp2C9,sizeof(char),2U);}));_tmp2C8[1]=_tmp6BF;}),({struct Cyc_PP_Doc*_tmp6BE=Cyc_Absynpp_exp2doc(e2);_tmp2C8[2]=_tmp6BE;}),({struct Cyc_PP_Doc*_tmp6BD=Cyc_PP_text(({const char*_tmp2CA="]";_tag_fat(_tmp2CA,sizeof(char),2U);}));_tmp2C8[3]=_tmp6BD;});Cyc_PP_cat(_tag_fat(_tmp2C8,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 24U: _LL3B: _tmp243=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL3C: {struct Cyc_List_List*es=_tmp243;
# 1210
s=({struct Cyc_PP_Doc*_tmp2CB[4U];({struct Cyc_PP_Doc*_tmp6C4=Cyc_Absynpp_dollar();_tmp2CB[0]=_tmp6C4;}),({struct Cyc_PP_Doc*_tmp6C3=Cyc_PP_text(({const char*_tmp2CC="(";_tag_fat(_tmp2CC,sizeof(char),2U);}));_tmp2CB[1]=_tmp6C3;}),({struct Cyc_PP_Doc*_tmp6C2=Cyc_Absynpp_exps2doc_prec(20,es);_tmp2CB[2]=_tmp6C2;}),({struct Cyc_PP_Doc*_tmp6C1=Cyc_PP_text(({const char*_tmp2CD=")";_tag_fat(_tmp2CD,sizeof(char),2U);}));_tmp2CB[3]=_tmp6C1;});Cyc_PP_cat(_tag_fat(_tmp2CB,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 25U: _LL3D: _tmp241=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp242=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL3E: {struct _tuple8*vat=_tmp241;struct Cyc_List_List*des=_tmp242;
# 1212
s=({struct Cyc_PP_Doc*_tmp2CE[4U];({struct Cyc_PP_Doc*_tmp6C9=Cyc_PP_text(({const char*_tmp2CF="(";_tag_fat(_tmp2CF,sizeof(char),2U);}));_tmp2CE[0]=_tmp6C9;}),({struct Cyc_PP_Doc*_tmp6C8=Cyc_Absynpp_typ2doc((*vat).f3);_tmp2CE[1]=_tmp6C8;}),({struct Cyc_PP_Doc*_tmp6C7=Cyc_PP_text(({const char*_tmp2D0=")";_tag_fat(_tmp2D0,sizeof(char),2U);}));_tmp2CE[2]=_tmp6C7;}),({
struct Cyc_PP_Doc*_tmp6C6=({struct _fat_ptr _tmp6C5=({const char*_tmp2D1=",";_tag_fat(_tmp2D1,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6C5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});_tmp2CE[3]=_tmp6C6;});Cyc_PP_cat(_tag_fat(_tmp2CE,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 26U: _LL3F: _tmp240=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL40: {struct Cyc_List_List*des=_tmp240;
s=({struct _fat_ptr _tmp6CA=({const char*_tmp2D2=",";_tag_fat(_tmp2D2,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6CA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 27U: _LL41: _tmp23D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp23E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp23F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL42: {struct Cyc_Absyn_Vardecl*vd=_tmp23D;struct Cyc_Absyn_Exp*e1=_tmp23E;struct Cyc_Absyn_Exp*e2=_tmp23F;
# 1217
s=({struct Cyc_PP_Doc*_tmp2D3[8U];({struct Cyc_PP_Doc*_tmp6D2=Cyc_Absynpp_lb();_tmp2D3[0]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_PP_text(({const char*_tmp2D4="for ";_tag_fat(_tmp2D4,sizeof(char),5U);}));_tmp2D3[1]=_tmp6D1;}),({
struct Cyc_PP_Doc*_tmp6D0=Cyc_PP_text(*(*vd->name).f2);_tmp2D3[2]=_tmp6D0;}),({struct Cyc_PP_Doc*_tmp6CF=Cyc_PP_text(({const char*_tmp2D5=" < ";_tag_fat(_tmp2D5,sizeof(char),4U);}));_tmp2D3[3]=_tmp6CF;}),({struct Cyc_PP_Doc*_tmp6CE=Cyc_Absynpp_exp2doc(e1);_tmp2D3[4]=_tmp6CE;}),({struct Cyc_PP_Doc*_tmp6CD=Cyc_PP_text(({const char*_tmp2D6=" : ";_tag_fat(_tmp2D6,sizeof(char),4U);}));_tmp2D3[5]=_tmp6CD;}),({
struct Cyc_PP_Doc*_tmp6CC=Cyc_Absynpp_exp2doc(e2);_tmp2D3[6]=_tmp6CC;}),({struct Cyc_PP_Doc*_tmp6CB=Cyc_Absynpp_rb();_tmp2D3[7]=_tmp6CB;});Cyc_PP_cat(_tag_fat(_tmp2D3,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 28U: _LL43: _tmp23B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp23C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL44: {struct Cyc_Absyn_Exp*e=_tmp23B;void*t=_tmp23C;
# 1222
s=({struct Cyc_PP_Doc*_tmp2D7[7U];({struct Cyc_PP_Doc*_tmp6D9=Cyc_Absynpp_lb();_tmp2D7[0]=_tmp6D9;}),({struct Cyc_PP_Doc*_tmp6D8=Cyc_PP_text(({const char*_tmp2D8="for x ";_tag_fat(_tmp2D8,sizeof(char),7U);}));_tmp2D7[1]=_tmp6D8;}),({
struct Cyc_PP_Doc*_tmp6D7=Cyc_PP_text(({const char*_tmp2D9=" < ";_tag_fat(_tmp2D9,sizeof(char),4U);}));_tmp2D7[2]=_tmp6D7;}),({
struct Cyc_PP_Doc*_tmp6D6=Cyc_Absynpp_exp2doc(e);_tmp2D7[3]=_tmp6D6;}),({
struct Cyc_PP_Doc*_tmp6D5=Cyc_PP_text(({const char*_tmp2DA=" : ";_tag_fat(_tmp2DA,sizeof(char),4U);}));_tmp2D7[4]=_tmp6D5;}),({
struct Cyc_PP_Doc*_tmp6D4=Cyc_Absynpp_typ2doc(t);_tmp2D7[5]=_tmp6D4;}),({
struct Cyc_PP_Doc*_tmp6D3=Cyc_Absynpp_rb();_tmp2D7[6]=_tmp6D3;});Cyc_PP_cat(_tag_fat(_tmp2D7,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 29U: _LL45: _tmp238=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp239=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_tmp23A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL46: {struct _tuple0*n=_tmp238;struct Cyc_List_List*ts=_tmp239;struct Cyc_List_List*des=_tmp23A;
# 1230
struct Cyc_List_List*_tmp2DB=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des);struct Cyc_List_List*des_doc=_tmp2DB;
s=({struct Cyc_PP_Doc*_tmp2DC[2U];({struct Cyc_PP_Doc*_tmp6DD=Cyc_Absynpp_qvar2doc(n);_tmp2DC[0]=_tmp6DD;}),({
struct Cyc_PP_Doc*_tmp6DC=({struct _fat_ptr _tmp6DB=({const char*_tmp2DD=",";_tag_fat(_tmp2DD,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6DB,
ts != 0?({struct Cyc_List_List*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));({struct Cyc_PP_Doc*_tmp6DA=Cyc_Absynpp_tps2doc(ts);_tmp2DE->hd=_tmp6DA;}),_tmp2DE->tl=des_doc;_tmp2DE;}): des_doc);});
# 1232
_tmp2DC[1]=_tmp6DC;});Cyc_PP_cat(_tag_fat(_tmp2DC,sizeof(struct Cyc_PP_Doc*),2U));});
# 1234
goto _LL0;}case 30U: _LL47: _tmp237=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL48: {struct Cyc_List_List*des=_tmp237;
s=({struct _fat_ptr _tmp6DE=({const char*_tmp2DF=",";_tag_fat(_tmp2DF,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6DE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 31U: _LL49: _tmp235=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp236=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL4A: {struct Cyc_List_List*es=_tmp235;struct Cyc_Absyn_Datatypefield*tuf=_tmp236;
# 1237
if(es == 0)
# 1239
s=Cyc_Absynpp_qvar2doc(tuf->name);else{
# 1241
s=({struct Cyc_PP_Doc*_tmp2E0[2U];({struct Cyc_PP_Doc*_tmp6E3=Cyc_Absynpp_qvar2doc(tuf->name);_tmp2E0[0]=_tmp6E3;}),({
struct Cyc_PP_Doc*_tmp6E2=({struct _fat_ptr _tmp6E1=({const char*_tmp2E1="(";_tag_fat(_tmp2E1,sizeof(char),2U);});struct _fat_ptr _tmp6E0=({const char*_tmp2E2=")";_tag_fat(_tmp2E2,sizeof(char),2U);});struct _fat_ptr _tmp6DF=({const char*_tmp2E3=",";_tag_fat(_tmp2E3,sizeof(char),2U);});Cyc_PP_egroup(_tmp6E1,_tmp6E0,_tmp6DF,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,es));});_tmp2E0[1]=_tmp6E2;});Cyc_PP_cat(_tag_fat(_tmp2E0,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;}case 32U: _LL4B: _tmp234=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL4C: {struct Cyc_Absyn_Enumfield*ef=_tmp234;
_tmp233=ef;goto _LL4E;}case 33U: _LL4D: _tmp233=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL4E: {struct Cyc_Absyn_Enumfield*ef=_tmp233;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 34U: _LL4F: _tmp22E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).is_calloc;_tmp22F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).rgn;_tmp230=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).elt_type;_tmp231=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).num_elts;_tmp232=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228)->f1).inline_call;_LL50: {int is_calloc=_tmp22E;struct Cyc_Absyn_Exp*rgnopt=_tmp22F;void**topt=_tmp230;struct Cyc_Absyn_Exp*e=_tmp231;int inline_call=_tmp232;
# 1249
if(is_calloc){
# 1251
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U);
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp2E4[5U];({struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_text(({const char*_tmp2E5="calloc(";_tag_fat(_tmp2E5,sizeof(char),8U);}));_tmp2E4[0]=_tmp6E8;}),({struct Cyc_PP_Doc*_tmp6E7=Cyc_Absynpp_exp2doc(e);_tmp2E4[1]=_tmp6E7;}),({struct Cyc_PP_Doc*_tmp6E6=Cyc_PP_text(({const char*_tmp2E6=",";_tag_fat(_tmp2E6,sizeof(char),2U);}));_tmp2E4[2]=_tmp6E6;}),({struct Cyc_PP_Doc*_tmp6E5=Cyc_Absynpp_exp2doc(st);_tmp2E4[3]=_tmp6E5;}),({struct Cyc_PP_Doc*_tmp6E4=Cyc_PP_text(({const char*_tmp2E7=")";_tag_fat(_tmp2E7,sizeof(char),2U);}));_tmp2E4[4]=_tmp6E4;});Cyc_PP_cat(_tag_fat(_tmp2E4,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1255
s=({struct Cyc_PP_Doc*_tmp2E8[7U];({struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_text(({const char*_tmp2E9="rcalloc(";_tag_fat(_tmp2E9,sizeof(char),9U);}));_tmp2E8[0]=_tmp6EF;}),({struct Cyc_PP_Doc*_tmp6EE=Cyc_Absynpp_exp2doc(rgnopt);_tmp2E8[1]=_tmp6EE;}),({struct Cyc_PP_Doc*_tmp6ED=Cyc_PP_text(({const char*_tmp2EA=",";_tag_fat(_tmp2EA,sizeof(char),2U);}));_tmp2E8[2]=_tmp6ED;}),({
struct Cyc_PP_Doc*_tmp6EC=Cyc_Absynpp_exp2doc(e);_tmp2E8[3]=_tmp6EC;}),({struct Cyc_PP_Doc*_tmp6EB=Cyc_PP_text(({const char*_tmp2EB=",";_tag_fat(_tmp2EB,sizeof(char),2U);}));_tmp2E8[4]=_tmp6EB;}),({struct Cyc_PP_Doc*_tmp6EA=Cyc_Absynpp_exp2doc(st);_tmp2E8[5]=_tmp6EA;}),({struct Cyc_PP_Doc*_tmp6E9=Cyc_PP_text(({const char*_tmp2EC=")";_tag_fat(_tmp2EC,sizeof(char),2U);}));_tmp2E8[6]=_tmp6E9;});Cyc_PP_cat(_tag_fat(_tmp2E8,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1258
struct Cyc_Absyn_Exp*new_e;
# 1260
if(topt == 0)
new_e=e;else{
# 1263
new_e=({struct Cyc_Absyn_Exp*_tmp6F0=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp6F0,e,0U);});}
# 1265
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp2ED[3U];({struct Cyc_PP_Doc*_tmp6F3=Cyc_PP_text(({const char*_tmp2EE="malloc(";_tag_fat(_tmp2EE,sizeof(char),8U);}));_tmp2ED[0]=_tmp6F3;}),({struct Cyc_PP_Doc*_tmp6F2=Cyc_Absynpp_exp2doc(new_e);_tmp2ED[1]=_tmp6F2;}),({struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_text(({const char*_tmp2EF=")";_tag_fat(_tmp2EF,sizeof(char),2U);}));_tmp2ED[2]=_tmp6F1;});Cyc_PP_cat(_tag_fat(_tmp2ED,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1268
if(inline_call)
s=({struct Cyc_PP_Doc*_tmp2F0[5U];({struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_text(({const char*_tmp2F1="rmalloc_inline(";_tag_fat(_tmp2F1,sizeof(char),16U);}));_tmp2F0[0]=_tmp6F8;}),({struct Cyc_PP_Doc*_tmp6F7=Cyc_Absynpp_exp2doc(rgnopt);_tmp2F0[1]=_tmp6F7;}),({struct Cyc_PP_Doc*_tmp6F6=Cyc_PP_text(({const char*_tmp2F2=",";_tag_fat(_tmp2F2,sizeof(char),2U);}));_tmp2F0[2]=_tmp6F6;}),({
struct Cyc_PP_Doc*_tmp6F5=Cyc_Absynpp_exp2doc(new_e);_tmp2F0[3]=_tmp6F5;}),({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp2F3=")";_tag_fat(_tmp2F3,sizeof(char),2U);}));_tmp2F0[4]=_tmp6F4;});Cyc_PP_cat(_tag_fat(_tmp2F0,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1272
s=({struct Cyc_PP_Doc*_tmp2F4[5U];({struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_text(({const char*_tmp2F5="rmalloc(";_tag_fat(_tmp2F5,sizeof(char),9U);}));_tmp2F4[0]=_tmp6FD;}),({struct Cyc_PP_Doc*_tmp6FC=Cyc_Absynpp_exp2doc(rgnopt);_tmp2F4[1]=_tmp6FC;}),({struct Cyc_PP_Doc*_tmp6FB=Cyc_PP_text(({const char*_tmp2F6=",";_tag_fat(_tmp2F6,sizeof(char),2U);}));_tmp2F4[2]=_tmp6FB;}),({
struct Cyc_PP_Doc*_tmp6FA=Cyc_Absynpp_exp2doc(new_e);_tmp2F4[3]=_tmp6FA;}),({struct Cyc_PP_Doc*_tmp6F9=Cyc_PP_text(({const char*_tmp2F7=")";_tag_fat(_tmp2F7,sizeof(char),2U);}));_tmp2F4[4]=_tmp6F9;});Cyc_PP_cat(_tag_fat(_tmp2F4,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1276
goto _LL0;}case 35U: _LL51: _tmp22C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp22D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp22C;struct Cyc_Absyn_Exp*e2=_tmp22D;
# 1278
s=({struct Cyc_PP_Doc*_tmp2F8[3U];({struct Cyc_PP_Doc*_tmp700=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2F8[0]=_tmp700;}),({struct Cyc_PP_Doc*_tmp6FF=Cyc_PP_text(({const char*_tmp2F9=" :=: ";_tag_fat(_tmp2F9,sizeof(char),6U);}));_tmp2F8[1]=_tmp6FF;}),({struct Cyc_PP_Doc*_tmp6FE=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2F8[2]=_tmp6FE;});Cyc_PP_cat(_tag_fat(_tmp2F8,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 36U: _LL53: _tmp22A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_tmp22B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp228)->f2;_LL54: {struct Cyc_Core_Opt*n=_tmp22A;struct Cyc_List_List*des=_tmp22B;
# 1281
s=({struct _fat_ptr _tmp701=({const char*_tmp2FA=",";_tag_fat(_tmp2FA,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp701,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _LL55: _tmp229=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp228)->f1;_LL56: {struct Cyc_Absyn_Stmt*s2=_tmp229;
# 1284
s=({struct Cyc_PP_Doc*_tmp2FB[7U];({struct Cyc_PP_Doc*_tmp708=Cyc_PP_text(({const char*_tmp2FC="(";_tag_fat(_tmp2FC,sizeof(char),2U);}));_tmp2FB[0]=_tmp708;}),({struct Cyc_PP_Doc*_tmp707=Cyc_Absynpp_lb();_tmp2FB[1]=_tmp707;}),({struct Cyc_PP_Doc*_tmp706=Cyc_PP_blank_doc();_tmp2FB[2]=_tmp706;}),({
struct Cyc_PP_Doc*_tmp705=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_tmp2FB[3]=_tmp705;}),({
struct Cyc_PP_Doc*_tmp704=Cyc_PP_blank_doc();_tmp2FB[4]=_tmp704;}),({struct Cyc_PP_Doc*_tmp703=Cyc_Absynpp_rb();_tmp2FB[5]=_tmp703;}),({struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp2FD=")";_tag_fat(_tmp2FD,sizeof(char),2U);}));_tmp2FB[6]=_tmp702;});Cyc_PP_cat(_tag_fat(_tmp2FB,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}}_LL0:;}
# 1289
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2FE[3U];({struct Cyc_PP_Doc*_tmp70A=Cyc_PP_text(({const char*_tmp2FF="(";_tag_fat(_tmp2FF,sizeof(char),2U);}));_tmp2FE[0]=_tmp70A;}),_tmp2FE[1]=s,({struct Cyc_PP_Doc*_tmp709=Cyc_PP_text(({const char*_tmp300=")";_tag_fat(_tmp300,sizeof(char),2U);}));_tmp2FE[2]=_tmp709;});Cyc_PP_cat(_tag_fat(_tmp2FE,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1294
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp301=d;struct _fat_ptr*_tmp302;struct Cyc_Absyn_Exp*_tmp303;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp301)->tag == 0U){_LL1: _tmp303=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp301)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp303;
return({struct Cyc_PP_Doc*_tmp304[3U];({struct Cyc_PP_Doc*_tmp70D=Cyc_PP_text(({const char*_tmp305=".[";_tag_fat(_tmp305,sizeof(char),3U);}));_tmp304[0]=_tmp70D;}),({struct Cyc_PP_Doc*_tmp70C=Cyc_Absynpp_exp2doc(e);_tmp304[1]=_tmp70C;}),({struct Cyc_PP_Doc*_tmp70B=Cyc_PP_text(({const char*_tmp306="]";_tag_fat(_tmp306,sizeof(char),2U);}));_tmp304[2]=_tmp70B;});Cyc_PP_cat(_tag_fat(_tmp304,sizeof(struct Cyc_PP_Doc*),3U));});}}else{_LL3: _tmp302=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp301)->f1;_LL4: {struct _fat_ptr*v=_tmp302;
return({struct Cyc_PP_Doc*_tmp307[2U];({struct Cyc_PP_Doc*_tmp70F=Cyc_PP_text(({const char*_tmp308=".";_tag_fat(_tmp308,sizeof(char),2U);}));_tmp307[0]=_tmp70F;}),({struct Cyc_PP_Doc*_tmp70E=Cyc_PP_textptr(v);_tmp307[1]=_tmp70E;});Cyc_PP_cat(_tag_fat(_tmp307,sizeof(struct Cyc_PP_Doc*),2U));});}}_LL0:;}
# 1301
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp309[2U];({struct Cyc_PP_Doc*_tmp714=({struct _fat_ptr _tmp713=({const char*_tmp30A="";_tag_fat(_tmp30A,sizeof(char),1U);});struct _fat_ptr _tmp712=({const char*_tmp30B="=";_tag_fat(_tmp30B,sizeof(char),2U);});struct _fat_ptr _tmp711=({const char*_tmp30C="=";_tag_fat(_tmp30C,sizeof(char),2U);});Cyc_PP_egroup(_tmp713,_tmp712,_tmp711,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp309[0]=_tmp714;}),({
struct Cyc_PP_Doc*_tmp710=Cyc_Absynpp_exp2doc((*de).f2);_tmp309[1]=_tmp710;});Cyc_PP_cat(_tag_fat(_tmp309,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1307
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _tmp717=({const char*_tmp30D="";_tag_fat(_tmp30D,sizeof(char),1U);});struct _fat_ptr _tmp716=({const char*_tmp30E="";_tag_fat(_tmp30E,sizeof(char),1U);});struct _fat_ptr _tmp715=({const char*_tmp30F=",";_tag_fat(_tmp30F,sizeof(char),2U);});Cyc_PP_group(_tmp717,_tmp716,_tmp715,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1311
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_tmp324=({unsigned _tmp323=28U + 1U;char*_tmp322=_cycalloc_atomic(_check_times(_tmp323,sizeof(char)));({{unsigned _tmp57C=28U;unsigned i;for(i=0;i < _tmp57C;++ i){_tmp322[i]='z';}_tmp322[_tmp57C]=0;}0;});_tmp322;});_tag_fat(_tmp324,sizeof(char),29U);});
({struct _fat_ptr _tmp310=_fat_ptr_plus(x,sizeof(char),27);char _tmp311=*((char*)_check_fat_subscript(_tmp310,sizeof(char),0U));char _tmp312='\000';if(_get_fat_size(_tmp310,sizeof(char))== 1U &&(_tmp311 == 0 && _tmp312 != 0))_throw_arraybounds();*((char*)_tmp310.curr)=_tmp312;});
({struct _fat_ptr _tmp313=_fat_ptr_plus(x,sizeof(char),26);char _tmp314=*((char*)_check_fat_subscript(_tmp313,sizeof(char),0U));char _tmp315='L';if(_get_fat_size(_tmp313,sizeof(char))== 1U &&(_tmp314 == 0 && _tmp315 != 0))_throw_arraybounds();*((char*)_tmp313.curr)=_tmp315;});
({struct _fat_ptr _tmp316=_fat_ptr_plus(x,sizeof(char),25);char _tmp317=*((char*)_check_fat_subscript(_tmp316,sizeof(char),0U));char _tmp318='L';if(_get_fat_size(_tmp316,sizeof(char))== 1U &&(_tmp317 == 0 && _tmp318 != 0))_throw_arraybounds();*((char*)_tmp316.curr)=_tmp318;});
({struct _fat_ptr _tmp319=_fat_ptr_plus(x,sizeof(char),24);char _tmp31A=*((char*)_check_fat_subscript(_tmp319,sizeof(char),0U));char _tmp31B='U';if(_get_fat_size(_tmp319,sizeof(char))== 1U &&(_tmp31A == 0 && _tmp31B != 0))_throw_arraybounds();*((char*)_tmp319.curr)=_tmp31B;});
({struct _fat_ptr _tmp31C=_fat_ptr_plus(x,sizeof(char),23);char _tmp31D=*((char*)_check_fat_subscript(_tmp31C,sizeof(char),0U));char _tmp31E='0';if(_get_fat_size(_tmp31C,sizeof(char))== 1U &&(_tmp31D == 0 && _tmp31E != 0))_throw_arraybounds();*((char*)_tmp31C.curr)=_tmp31E;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _fat_ptr _tmp31F=_fat_ptr_plus(x,sizeof(char),index);char _tmp320=*((char*)_check_fat_subscript(_tmp31F,sizeof(char),0U));char _tmp321=c;if(_get_fat_size(_tmp31F,sizeof(char))== 1U &&(_tmp320 == 0 && _tmp321 != 0))_throw_arraybounds();*((char*)_tmp31F.curr)=_tmp321;});
i=i / (unsigned long long)10;
-- index;}
# 1325
return(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1329
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp325=c;struct _fat_ptr _tmp326;struct _fat_ptr _tmp327;struct _fat_ptr _tmp328;long long _tmp32A;enum Cyc_Absyn_Sign _tmp329;int _tmp32C;enum Cyc_Absyn_Sign _tmp32B;short _tmp32E;enum Cyc_Absyn_Sign _tmp32D;struct _fat_ptr _tmp32F;char _tmp331;enum Cyc_Absyn_Sign _tmp330;switch((_tmp325.String_c).tag){case 2U: _LL1: _tmp330=((_tmp325.Char_c).val).f1;_tmp331=((_tmp325.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp330;char ch=_tmp331;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp334=({struct Cyc_String_pa_PrintArg_struct _tmp57D;_tmp57D.tag=0U,({struct _fat_ptr _tmp718=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp57D.f1=_tmp718;});_tmp57D;});void*_tmp332[1U];_tmp332[0]=& _tmp334;({struct _fat_ptr _tmp719=({const char*_tmp333="'%s'";_tag_fat(_tmp333,sizeof(char),5U);});Cyc_aprintf(_tmp719,_tag_fat(_tmp332,sizeof(void*),1U));});}));}case 3U: _LL3: _tmp32F=(_tmp325.Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp32F;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp337=({struct Cyc_String_pa_PrintArg_struct _tmp57E;_tmp57E.tag=0U,_tmp57E.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp57E;});void*_tmp335[1U];_tmp335[0]=& _tmp337;({struct _fat_ptr _tmp71A=({const char*_tmp336="L'%s'";_tag_fat(_tmp336,sizeof(char),6U);});Cyc_aprintf(_tmp71A,_tag_fat(_tmp335,sizeof(void*),1U));});}));}case 4U: _LL5: _tmp32D=((_tmp325.Short_c).val).f1;_tmp32E=((_tmp325.Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp32D;short s=_tmp32E;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp33A=({struct Cyc_Int_pa_PrintArg_struct _tmp57F;_tmp57F.tag=1U,_tmp57F.f1=(unsigned long)((int)s);_tmp57F;});void*_tmp338[1U];_tmp338[0]=& _tmp33A;({struct _fat_ptr _tmp71B=({const char*_tmp339="%d";_tag_fat(_tmp339,sizeof(char),3U);});Cyc_aprintf(_tmp71B,_tag_fat(_tmp338,sizeof(void*),1U));});}));}case 5U: _LL7: _tmp32B=((_tmp325.Int_c).val).f1;_tmp32C=((_tmp325.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign sn=_tmp32B;int i=_tmp32C;
# 1335
if((int)sn == (int)1U)return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp33D=({struct Cyc_Int_pa_PrintArg_struct _tmp580;_tmp580.tag=1U,_tmp580.f1=(unsigned)i;_tmp580;});void*_tmp33B[1U];_tmp33B[0]=& _tmp33D;({struct _fat_ptr _tmp71C=({const char*_tmp33C="%uU";_tag_fat(_tmp33C,sizeof(char),4U);});Cyc_aprintf(_tmp71C,_tag_fat(_tmp33B,sizeof(void*),1U));});}));else{
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp340=({struct Cyc_Int_pa_PrintArg_struct _tmp581;_tmp581.tag=1U,_tmp581.f1=(unsigned long)i;_tmp581;});void*_tmp33E[1U];_tmp33E[0]=& _tmp340;({struct _fat_ptr _tmp71D=({const char*_tmp33F="%d";_tag_fat(_tmp33F,sizeof(char),3U);});Cyc_aprintf(_tmp71D,_tag_fat(_tmp33E,sizeof(void*),1U));});}));}}case 6U: _LL9: _tmp329=((_tmp325.LongLong_c).val).f1;_tmp32A=((_tmp325.LongLong_c).val).f2;_LLA: {enum Cyc_Absyn_Sign sg=_tmp329;long long i=_tmp32A;
# 1339
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7U: _LLB: _tmp328=((_tmp325.Float_c).val).f1;_LLC: {struct _fat_ptr x=_tmp328;
return Cyc_PP_text(x);}case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp341="NULL";_tag_fat(_tmp341,sizeof(char),5U);}));case 8U: _LLF: _tmp327=(_tmp325.String_c).val;_LL10: {struct _fat_ptr s=_tmp327;
return({struct Cyc_PP_Doc*_tmp342[3U];({struct Cyc_PP_Doc*_tmp720=Cyc_PP_text(({const char*_tmp343="\"";_tag_fat(_tmp343,sizeof(char),2U);}));_tmp342[0]=_tmp720;}),({struct Cyc_PP_Doc*_tmp71F=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_tmp342[1]=_tmp71F;}),({struct Cyc_PP_Doc*_tmp71E=Cyc_PP_text(({const char*_tmp344="\"";_tag_fat(_tmp344,sizeof(char),2U);}));_tmp342[2]=_tmp71E;});Cyc_PP_cat(_tag_fat(_tmp342,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL11: _tmp326=(_tmp325.Wstring_c).val;_LL12: {struct _fat_ptr s=_tmp326;
return({struct Cyc_PP_Doc*_tmp345[3U];({struct Cyc_PP_Doc*_tmp723=Cyc_PP_text(({const char*_tmp346="L\"";_tag_fat(_tmp346,sizeof(char),3U);}));_tmp345[0]=_tmp723;}),({struct Cyc_PP_Doc*_tmp722=Cyc_PP_text(s);_tmp345[1]=_tmp722;}),({struct Cyc_PP_Doc*_tmp721=Cyc_PP_text(({const char*_tmp347="\"";_tag_fat(_tmp347,sizeof(char),2U);}));_tmp345[2]=_tmp721;});Cyc_PP_cat(_tag_fat(_tmp345,sizeof(struct Cyc_PP_Doc*),3U));});}}_LL0:;}
# 1347
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp726=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34A=({struct Cyc_String_pa_PrintArg_struct _tmp582;_tmp582.tag=0U,({struct _fat_ptr _tmp724=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp582.f1=_tmp724;});_tmp582;});void*_tmp348[1U];_tmp348[0]=& _tmp34A;({struct _fat_ptr _tmp725=({const char*_tmp349="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_tmp349,sizeof(char),47U);});Cyc_aprintf(_tmp725,_tag_fat(_tmp348,sizeof(void*),1U));});});_tmp34B->f1=_tmp726;});_tmp34B;}));
# 1353
return({struct Cyc_PP_Doc*_tmp34C[3U];({struct Cyc_PP_Doc*_tmp729=Cyc_PP_text(({const char*_tmp34D="numelts(";_tag_fat(_tmp34D,sizeof(char),9U);}));_tmp34C[0]=_tmp729;}),({struct Cyc_PP_Doc*_tmp728=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp34C[1]=_tmp728;}),({struct Cyc_PP_Doc*_tmp727=Cyc_PP_text(({const char*_tmp34E=")";_tag_fat(_tmp34E,sizeof(char),2U);}));_tmp34C[2]=_tmp727;});Cyc_PP_cat(_tag_fat(_tmp34C,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1355
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp72C=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp351=({struct Cyc_String_pa_PrintArg_struct _tmp583;_tmp583.tag=0U,({struct _fat_ptr _tmp72A=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp583.f1=_tmp72A;});_tmp583;});void*_tmp34F[1U];_tmp34F[0]=& _tmp351;({struct _fat_ptr _tmp72B=({const char*_tmp350="Absynpp::primapp2doc: %s with no args";_tag_fat(_tmp350,sizeof(char),38U);});Cyc_aprintf(_tmp72B,_tag_fat(_tmp34F,sizeof(void*),1U));});});_tmp352->f1=_tmp72C;});_tmp352;}));else{
# 1359
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp353[3U];_tmp353[0]=ps,({struct Cyc_PP_Doc*_tmp72D=Cyc_PP_text(({const char*_tmp354=" ";_tag_fat(_tmp354,sizeof(char),2U);}));_tmp353[1]=_tmp72D;}),_tmp353[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_tmp353,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp730=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp357=({struct Cyc_String_pa_PrintArg_struct _tmp584;_tmp584.tag=0U,({struct _fat_ptr _tmp72E=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp584.f1=_tmp72E;});_tmp584;});void*_tmp355[1U];_tmp355[0]=& _tmp357;({struct _fat_ptr _tmp72F=({const char*_tmp356="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_tmp356,sizeof(char),47U);});Cyc_aprintf(_tmp72F,_tag_fat(_tmp355,sizeof(void*),1U));});});_tmp358->f1=_tmp730;});_tmp358;}));else{
# 1365
return({struct Cyc_PP_Doc*_tmp359[5U];_tmp359[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp732=Cyc_PP_text(({const char*_tmp35A=" ";_tag_fat(_tmp35A,sizeof(char),2U);}));_tmp359[1]=_tmp732;}),_tmp359[2]=ps,({struct Cyc_PP_Doc*_tmp731=Cyc_PP_text(({const char*_tmp35B=" ";_tag_fat(_tmp35B,sizeof(char),2U);}));_tmp359[3]=_tmp731;}),_tmp359[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_tmp359,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1369
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp35C=p;switch(_tmp35C){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp35D="+";_tag_fat(_tmp35D,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp35E="*";_tag_fat(_tmp35E,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp35F="-";_tag_fat(_tmp35F,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp360="/";_tag_fat(_tmp360,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp361="\\%";_tag_fat(_tmp361,sizeof(char),3U);}):({const char*_tmp362="%";_tag_fat(_tmp362,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp363="==";_tag_fat(_tmp363,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp364="!=";_tag_fat(_tmp364,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp365=">";_tag_fat(_tmp365,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp366="<";_tag_fat(_tmp366,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp367=">=";_tag_fat(_tmp367,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp368="<=";_tag_fat(_tmp368,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp369="!";_tag_fat(_tmp369,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp36A="~";_tag_fat(_tmp36A,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp36B="&";_tag_fat(_tmp36B,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp36C="|";_tag_fat(_tmp36C,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp36D="^";_tag_fat(_tmp36D,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp36E="<<";_tag_fat(_tmp36E,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp36F=">>";_tag_fat(_tmp36F,sizeof(char),3U);});case Cyc_Absyn_Numelts: _LL25: _LL26:
 return({const char*_tmp370="numelts";_tag_fat(_tmp370,sizeof(char),8U);});default: _LL27: _LL28:
 return({const char*_tmp371="?";_tag_fat(_tmp371,sizeof(char),2U);});}_LL0:;}
# 1394
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1398
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp372=s->r;void*_stmttmp17=_tmp372;void*_tmp373=_stmttmp17;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp373)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1413 "absynpp.cyc"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_tmp374=d->r;void*_stmttmp18=_tmp374;void*_tmp375=_stmttmp18;struct Cyc_Absyn_Vardecl*_tmp376;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp375)->tag == 0U){_LL1: _tmp376=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp375)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp376;
# 1416
if(((int(*)(int(*compare)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*l,struct _tuple0*x))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple14 _tmp585;_tmp585.f1=1,({struct Cyc_List_List*_tmp733=({struct Cyc_List_List*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->hd=vd->name,_tmp377->tl=0;_tmp377;});_tmp585.f2=_tmp733;});_tmp585;});else{
# 1419
return({struct _tuple14 _tmp586;_tmp586.f1=0,({struct Cyc_List_List*_tmp734=({struct Cyc_List_List*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->hd=vd->name,_tmp378->tl=varsinblock;_tmp378;});_tmp586.f2=_tmp734;});_tmp586;});}}}else{_LL3: _LL4:
# 1421
 return({struct _tuple14 _tmp587;_tmp587.f1=0,_tmp587.f2=varsinblock;_tmp587;});}_LL0:;}
# 1425
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_tmp379[8U];({struct Cyc_PP_Doc*_tmp73C=Cyc_PP_text(({const char*_tmp37A="(";_tag_fat(_tmp37A,sizeof(char),2U);}));_tmp379[0]=_tmp73C;}),({struct Cyc_PP_Doc*_tmp73B=Cyc_Absynpp_lb();_tmp379[1]=_tmp73B;}),({struct Cyc_PP_Doc*_tmp73A=Cyc_PP_blank_doc();_tmp379[2]=_tmp73A;}),({struct Cyc_PP_Doc*_tmp739=Cyc_PP_nest(2,d);_tmp379[3]=_tmp739;}),({struct Cyc_PP_Doc*_tmp738=Cyc_PP_line_doc();_tmp379[4]=_tmp738;}),({struct Cyc_PP_Doc*_tmp737=Cyc_Absynpp_rb();_tmp379[5]=_tmp737;}),({
struct Cyc_PP_Doc*_tmp736=Cyc_PP_text(({const char*_tmp37B=");";_tag_fat(_tmp37B,sizeof(char),3U);}));_tmp379[6]=_tmp736;}),({struct Cyc_PP_Doc*_tmp735=Cyc_PP_line_doc();_tmp379[7]=_tmp735;});Cyc_PP_cat(_tag_fat(_tmp379,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1430
return({struct Cyc_PP_Doc*_tmp37C[6U];({struct Cyc_PP_Doc*_tmp742=Cyc_Absynpp_lb();_tmp37C[0]=_tmp742;}),({struct Cyc_PP_Doc*_tmp741=Cyc_PP_blank_doc();_tmp37C[1]=_tmp741;}),({struct Cyc_PP_Doc*_tmp740=Cyc_PP_nest(2,d);_tmp37C[2]=_tmp740;}),({struct Cyc_PP_Doc*_tmp73F=Cyc_PP_line_doc();_tmp37C[3]=_tmp73F;}),({struct Cyc_PP_Doc*_tmp73E=Cyc_Absynpp_rb();_tmp37C[4]=_tmp73E;}),({struct Cyc_PP_Doc*_tmp73D=Cyc_PP_line_doc();_tmp37C[5]=_tmp73D;});Cyc_PP_cat(_tag_fat(_tmp37C,sizeof(struct Cyc_PP_Doc*),6U));});}}
# 1433
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_tmp37D=st->r;void*_stmttmp19=_tmp37D;void*_tmp37E=_stmttmp19;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Stmt*_tmp37F;struct Cyc_Absyn_Exp*_tmp382;struct Cyc_Absyn_Stmt*_tmp381;struct Cyc_Absyn_Stmt*_tmp384;struct _fat_ptr*_tmp383;struct Cyc_Absyn_Stmt*_tmp386;struct Cyc_Absyn_Decl*_tmp385;struct Cyc_List_List*_tmp387;struct Cyc_List_List*_tmp389;struct Cyc_Absyn_Exp*_tmp388;struct Cyc_Absyn_Stmt*_tmp38D;struct Cyc_Absyn_Exp*_tmp38C;struct Cyc_Absyn_Exp*_tmp38B;struct Cyc_Absyn_Exp*_tmp38A;struct _fat_ptr*_tmp38E;struct Cyc_Absyn_Stmt*_tmp390;struct Cyc_Absyn_Exp*_tmp38F;struct Cyc_Absyn_Stmt*_tmp393;struct Cyc_Absyn_Stmt*_tmp392;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Stmt*_tmp396;struct Cyc_Absyn_Stmt*_tmp395;struct Cyc_Absyn_Exp*_tmp397;switch(*((int*)_tmp37E)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp398=";";_tag_fat(_tmp398,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp397=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp397;
s=({struct Cyc_PP_Doc*_tmp399[2U];({struct Cyc_PP_Doc*_tmp744=Cyc_Absynpp_exp2doc(e);_tmp399[0]=_tmp744;}),({struct Cyc_PP_Doc*_tmp743=Cyc_PP_text(({const char*_tmp39A=";";_tag_fat(_tmp39A,sizeof(char),2U);}));_tmp399[1]=_tmp743;});Cyc_PP_cat(_tag_fat(_tmp399,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 2U: _LL5: _tmp395=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_tmp396=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp395;struct Cyc_Absyn_Stmt*s2=_tmp396;
# 1439
if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp39B[2U];({struct Cyc_PP_Doc*_tmp748=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp39B[0]=_tmp748;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_tmp747=({int _tmp746=stmtexp;Cyc_Absynpp_block(_tmp746,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp39B[1]=_tmp747;}):({
struct Cyc_PP_Doc*_tmp745=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp39B[1]=_tmp745;});Cyc_PP_cat(_tag_fat(_tmp39B,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(Cyc_Absynpp_is_declaration(s2))
s=({struct Cyc_PP_Doc*_tmp39C[3U];({struct Cyc_PP_Doc*_tmp74C=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp39C[0]=_tmp74C;}),({
struct Cyc_PP_Doc*_tmp74B=Cyc_PP_line_doc();_tmp39C[1]=_tmp74B;}),({
struct Cyc_PP_Doc*_tmp74A=({int _tmp749=stmtexp;Cyc_Absynpp_block(_tmp749,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp39C[2]=_tmp74A;});Cyc_PP_cat(_tag_fat(_tmp39C,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1450
s=({struct Cyc_PP_Doc*_tmp39D[3U];({struct Cyc_PP_Doc*_tmp74F=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp39D[0]=_tmp74F;}),({struct Cyc_PP_Doc*_tmp74E=Cyc_PP_line_doc();_tmp39D[1]=_tmp74E;}),({
struct Cyc_PP_Doc*_tmp74D=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp39D[2]=_tmp74D;});Cyc_PP_cat(_tag_fat(_tmp39D,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1454
s=({struct Cyc_PP_Doc*_tmp39E[3U];({struct Cyc_PP_Doc*_tmp752=Cyc_Absynpp_stmt2doc(s1,0,0);_tmp39E[0]=_tmp752;}),({struct Cyc_PP_Doc*_tmp751=Cyc_PP_line_doc();_tmp39E[1]=_tmp751;}),({struct Cyc_PP_Doc*_tmp750=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_tmp39E[2]=_tmp750;});Cyc_PP_cat(_tag_fat(_tmp39E,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 3U: _LL7: _tmp394=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp394;
# 1457
if(eopt == 0)
s=Cyc_PP_text(({const char*_tmp39F="return;";_tag_fat(_tmp39F,sizeof(char),8U);}));else{
# 1460
s=({struct Cyc_PP_Doc*_tmp3A0[3U];({struct Cyc_PP_Doc*_tmp756=Cyc_PP_text(({const char*_tmp3A1="return ";_tag_fat(_tmp3A1,sizeof(char),8U);}));_tmp3A0[0]=_tmp756;}),
eopt == 0?({struct Cyc_PP_Doc*_tmp755=Cyc_PP_nil_doc();_tmp3A0[1]=_tmp755;}):({struct Cyc_PP_Doc*_tmp754=Cyc_Absynpp_exp2doc(eopt);_tmp3A0[1]=_tmp754;}),({
struct Cyc_PP_Doc*_tmp753=Cyc_PP_text(({const char*_tmp3A2=";";_tag_fat(_tmp3A2,sizeof(char),2U);}));_tmp3A0[2]=_tmp753;});Cyc_PP_cat(_tag_fat(_tmp3A0,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 4U: _LL9: _tmp391=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_tmp392=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2;_tmp393=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37E)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp391;struct Cyc_Absyn_Stmt*s1=_tmp392;struct Cyc_Absyn_Stmt*s2=_tmp393;
# 1465
int print_else;
{void*_tmp3A3=s2->r;void*_stmttmp1A=_tmp3A3;void*_tmp3A4=_stmttmp1A;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3A4)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1470
s=({struct Cyc_PP_Doc*_tmp3A5[5U];({struct Cyc_PP_Doc*_tmp75F=Cyc_PP_text(({const char*_tmp3A6="if (";_tag_fat(_tmp3A6,sizeof(char),5U);}));_tmp3A5[0]=_tmp75F;}),({
struct Cyc_PP_Doc*_tmp75E=Cyc_Absynpp_exp2doc(e);_tmp3A5[1]=_tmp75E;}),({
struct Cyc_PP_Doc*_tmp75D=Cyc_PP_text(({const char*_tmp3A7=") ";_tag_fat(_tmp3A7,sizeof(char),3U);}));_tmp3A5[2]=_tmp75D;}),({
struct Cyc_PP_Doc*_tmp75C=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3A5[3]=_tmp75C;}),
print_else?({
struct Cyc_PP_Doc*_tmp75B=({struct Cyc_PP_Doc*_tmp3A8[3U];({struct Cyc_PP_Doc*_tmp75A=Cyc_PP_line_doc();_tmp3A8[0]=_tmp75A;}),({
struct Cyc_PP_Doc*_tmp759=Cyc_PP_text(({const char*_tmp3A9="else ";_tag_fat(_tmp3A9,sizeof(char),6U);}));_tmp3A8[1]=_tmp759;}),({
struct Cyc_PP_Doc*_tmp758=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_tmp3A8[2]=_tmp758;});Cyc_PP_cat(_tag_fat(_tmp3A8,sizeof(struct Cyc_PP_Doc*),3U));});
# 1475
_tmp3A5[4]=_tmp75B;}):({
# 1478
struct Cyc_PP_Doc*_tmp757=Cyc_PP_nil_doc();_tmp3A5[4]=_tmp757;});Cyc_PP_cat(_tag_fat(_tmp3A5,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp38F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1).f1;_tmp390=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp38F;struct Cyc_Absyn_Stmt*s1=_tmp390;
# 1481
s=({struct Cyc_PP_Doc*_tmp3AA[4U];({struct Cyc_PP_Doc*_tmp763=Cyc_PP_text(({const char*_tmp3AB="while (";_tag_fat(_tmp3AB,sizeof(char),8U);}));_tmp3AA[0]=_tmp763;}),({
struct Cyc_PP_Doc*_tmp762=Cyc_Absynpp_exp2doc(e);_tmp3AA[1]=_tmp762;}),({
struct Cyc_PP_Doc*_tmp761=Cyc_PP_text(({const char*_tmp3AC=") ";_tag_fat(_tmp3AC,sizeof(char),3U);}));_tmp3AA[2]=_tmp761;}),({
struct Cyc_PP_Doc*_tmp760=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3AA[3]=_tmp760;});Cyc_PP_cat(_tag_fat(_tmp3AA,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp3AD="break;";_tag_fat(_tmp3AD,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp3AE="continue;";_tag_fat(_tmp3AE,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp38E=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_LL12: {struct _fat_ptr*x=_tmp38E;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B1=({struct Cyc_String_pa_PrintArg_struct _tmp588;_tmp588.tag=0U,_tmp588.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp588;});void*_tmp3AF[1U];_tmp3AF[0]=& _tmp3B1;({struct _fat_ptr _tmp764=({const char*_tmp3B0="goto %s;";_tag_fat(_tmp3B0,sizeof(char),9U);});Cyc_aprintf(_tmp764,_tag_fat(_tmp3AF,sizeof(void*),1U));});}));goto _LL0;}case 9U: _LL13: _tmp38A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_tmp38B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2).f1;_tmp38C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37E)->f3).f1;_tmp38D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37E)->f4;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp38A;struct Cyc_Absyn_Exp*e2=_tmp38B;struct Cyc_Absyn_Exp*e3=_tmp38C;struct Cyc_Absyn_Stmt*s1=_tmp38D;
# 1490
s=({struct Cyc_PP_Doc*_tmp3B2[8U];({struct Cyc_PP_Doc*_tmp76C=Cyc_PP_text(({const char*_tmp3B3="for(";_tag_fat(_tmp3B3,sizeof(char),5U);}));_tmp3B2[0]=_tmp76C;}),({
struct Cyc_PP_Doc*_tmp76B=Cyc_Absynpp_exp2doc(e1);_tmp3B2[1]=_tmp76B;}),({
struct Cyc_PP_Doc*_tmp76A=Cyc_PP_text(({const char*_tmp3B4="; ";_tag_fat(_tmp3B4,sizeof(char),3U);}));_tmp3B2[2]=_tmp76A;}),({
struct Cyc_PP_Doc*_tmp769=Cyc_Absynpp_exp2doc(e2);_tmp3B2[3]=_tmp769;}),({
struct Cyc_PP_Doc*_tmp768=Cyc_PP_text(({const char*_tmp3B5="; ";_tag_fat(_tmp3B5,sizeof(char),3U);}));_tmp3B2[4]=_tmp768;}),({
struct Cyc_PP_Doc*_tmp767=Cyc_Absynpp_exp2doc(e3);_tmp3B2[5]=_tmp767;}),({
struct Cyc_PP_Doc*_tmp766=Cyc_PP_text(({const char*_tmp3B6=") ";_tag_fat(_tmp3B6,sizeof(char),3U);}));_tmp3B2[6]=_tmp766;}),({
struct Cyc_PP_Doc*_tmp765=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3B2[7]=_tmp765;});Cyc_PP_cat(_tag_fat(_tmp3B2,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 10U: _LL15: _tmp388=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_tmp389=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp388;struct Cyc_List_List*ss=_tmp389;
# 1500
s=({struct Cyc_PP_Doc*_tmp3B7[8U];({struct Cyc_PP_Doc*_tmp774=Cyc_PP_text(({const char*_tmp3B8="switch (";_tag_fat(_tmp3B8,sizeof(char),9U);}));_tmp3B7[0]=_tmp774;}),({
struct Cyc_PP_Doc*_tmp773=Cyc_Absynpp_exp2doc(e);_tmp3B7[1]=_tmp773;}),({
struct Cyc_PP_Doc*_tmp772=Cyc_PP_text(({const char*_tmp3B9=") ";_tag_fat(_tmp3B9,sizeof(char),3U);}));_tmp3B7[2]=_tmp772;}),({
struct Cyc_PP_Doc*_tmp771=Cyc_Absynpp_lb();_tmp3B7[3]=_tmp771;}),({
struct Cyc_PP_Doc*_tmp770=Cyc_PP_line_doc();_tmp3B7[4]=_tmp770;}),({
struct Cyc_PP_Doc*_tmp76F=Cyc_Absynpp_switchclauses2doc(ss);_tmp3B7[5]=_tmp76F;}),({
struct Cyc_PP_Doc*_tmp76E=Cyc_PP_line_doc();_tmp3B7[6]=_tmp76E;}),({
struct Cyc_PP_Doc*_tmp76D=Cyc_Absynpp_rb();_tmp3B7[7]=_tmp76D;});Cyc_PP_cat(_tag_fat(_tmp3B7,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp3BA="fallthru;";_tag_fat(_tmp3BA,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp387=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_LL1A: {struct Cyc_List_List*es=_tmp387;
# 1511
s=({struct Cyc_PP_Doc*_tmp3BB[3U];({struct Cyc_PP_Doc*_tmp777=Cyc_PP_text(({const char*_tmp3BC="fallthru(";_tag_fat(_tmp3BC,sizeof(char),10U);}));_tmp3BB[0]=_tmp777;}),({struct Cyc_PP_Doc*_tmp776=Cyc_Absynpp_exps2doc_prec(20,es);_tmp3BB[1]=_tmp776;}),({struct Cyc_PP_Doc*_tmp775=Cyc_PP_text(({const char*_tmp3BD=");";_tag_fat(_tmp3BD,sizeof(char),3U);}));_tmp3BB[2]=_tmp775;});Cyc_PP_cat(_tag_fat(_tmp3BB,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 12U: _LL1B: _tmp385=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_tmp386=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp385;struct Cyc_Absyn_Stmt*s1=_tmp386;
# 1513
struct _tuple14 _tmp3BE=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple14 _stmttmp1B=_tmp3BE;struct _tuple14 _tmp3BF=_stmttmp1B;struct Cyc_List_List*_tmp3C1;int _tmp3C0;_LL29: _tmp3C0=_tmp3BF.f1;_tmp3C1=_tmp3BF.f2;_LL2A: {int newblock=_tmp3C0;struct Cyc_List_List*newvarsinblock=_tmp3C1;
s=({struct Cyc_PP_Doc*_tmp3C2[3U];({struct Cyc_PP_Doc*_tmp77A=Cyc_Absynpp_decl2doc(d);_tmp3C2[0]=_tmp77A;}),({struct Cyc_PP_Doc*_tmp779=Cyc_PP_line_doc();_tmp3C2[1]=_tmp779;}),({struct Cyc_PP_Doc*_tmp778=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_tmp3C2[2]=_tmp778;});Cyc_PP_cat(_tag_fat(_tmp3C2,sizeof(struct Cyc_PP_Doc*),3U));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}}case 13U: _LL1D: _tmp383=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_tmp384=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2;_LL1E: {struct _fat_ptr*x=_tmp383;struct Cyc_Absyn_Stmt*s1=_tmp384;
# 1518
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp3C3[3U];({struct Cyc_PP_Doc*_tmp77E=Cyc_PP_textptr(x);_tmp3C3[0]=_tmp77E;}),({struct Cyc_PP_Doc*_tmp77D=Cyc_PP_text(({const char*_tmp3C4=": ";_tag_fat(_tmp3C4,sizeof(char),3U);}));_tmp3C3[1]=_tmp77D;}),({struct Cyc_PP_Doc*_tmp77C=({int _tmp77B=stmtexp;Cyc_Absynpp_block(_tmp77B,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_tmp3C3[2]=_tmp77C;});Cyc_PP_cat(_tag_fat(_tmp3C3,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1521
s=({struct Cyc_PP_Doc*_tmp3C5[3U];({struct Cyc_PP_Doc*_tmp781=Cyc_PP_textptr(x);_tmp3C5[0]=_tmp781;}),({struct Cyc_PP_Doc*_tmp780=Cyc_PP_text(({const char*_tmp3C6=": ";_tag_fat(_tmp3C6,sizeof(char),3U);}));_tmp3C5[1]=_tmp780;}),({struct Cyc_PP_Doc*_tmp77F=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_tmp3C5[2]=_tmp77F;});Cyc_PP_cat(_tag_fat(_tmp3C5,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 14U: _LL1F: _tmp381=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_tmp382=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2).f1;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp381;struct Cyc_Absyn_Exp*e=_tmp382;
# 1524
s=({struct Cyc_PP_Doc*_tmp3C7[5U];({struct Cyc_PP_Doc*_tmp786=Cyc_PP_text(({const char*_tmp3C8="do ";_tag_fat(_tmp3C8,sizeof(char),4U);}));_tmp3C7[0]=_tmp786;}),({
struct Cyc_PP_Doc*_tmp785=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3C7[1]=_tmp785;}),({
struct Cyc_PP_Doc*_tmp784=Cyc_PP_text(({const char*_tmp3C9=" while (";_tag_fat(_tmp3C9,sizeof(char),9U);}));_tmp3C7[2]=_tmp784;}),({
struct Cyc_PP_Doc*_tmp783=Cyc_Absynpp_exp2doc(e);_tmp3C7[3]=_tmp783;}),({
struct Cyc_PP_Doc*_tmp782=Cyc_PP_text(({const char*_tmp3CA=");";_tag_fat(_tmp3CA,sizeof(char),3U);}));_tmp3C7[4]=_tmp782;});Cyc_PP_cat(_tag_fat(_tmp3C7,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}default: _LL21: _tmp37F=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp37E)->f1;_tmp380=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp37E)->f2;_LL22: {struct Cyc_Absyn_Stmt*s1=_tmp37F;struct Cyc_List_List*ss=_tmp380;
# 1531
s=({struct Cyc_PP_Doc*_tmp3CB[4U];({struct Cyc_PP_Doc*_tmp78A=Cyc_PP_text(({const char*_tmp3CC="try ";_tag_fat(_tmp3CC,sizeof(char),5U);}));_tmp3CB[0]=_tmp78A;}),({
struct Cyc_PP_Doc*_tmp789=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3CB[1]=_tmp789;}),({
struct Cyc_PP_Doc*_tmp788=Cyc_PP_text(({const char*_tmp3CD=" catch ";_tag_fat(_tmp3CD,sizeof(char),8U);}));_tmp3CB[2]=_tmp788;}),({
struct Cyc_PP_Doc*_tmp787=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_tmp3CB[3]=_tmp787;});Cyc_PP_cat(_tag_fat(_tmp3CB,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}_LL0:;}
# 1537
return s;}
# 1540
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3CE=p->r;void*_stmttmp1C=_tmp3CE;void*_tmp3CF=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp3D0;int _tmp3D3;struct Cyc_List_List*_tmp3D2;struct Cyc_Absyn_Datatypefield*_tmp3D1;struct Cyc_Absyn_Datatypefield*_tmp3D4;struct Cyc_Absyn_Enumfield*_tmp3D5;struct Cyc_Absyn_Enumfield*_tmp3D6;int _tmp3D9;struct Cyc_List_List*_tmp3D8;struct Cyc_List_List*_tmp3D7;int _tmp3DD;struct Cyc_List_List*_tmp3DC;struct Cyc_List_List*_tmp3DB;union Cyc_Absyn_AggrInfo _tmp3DA;int _tmp3E0;struct Cyc_List_List*_tmp3DF;struct _tuple0*_tmp3DE;struct _tuple0*_tmp3E1;struct Cyc_Absyn_Pat*_tmp3E3;struct Cyc_Absyn_Vardecl*_tmp3E2;struct Cyc_Absyn_Vardecl*_tmp3E4;struct Cyc_Absyn_Pat*_tmp3E5;int _tmp3E7;struct Cyc_List_List*_tmp3E6;struct Cyc_Absyn_Vardecl*_tmp3E9;struct Cyc_Absyn_Tvar*_tmp3E8;struct Cyc_Absyn_Vardecl*_tmp3EB;struct Cyc_Absyn_Tvar*_tmp3EA;struct Cyc_Absyn_Pat*_tmp3ED;struct Cyc_Absyn_Vardecl*_tmp3EC;struct Cyc_Absyn_Vardecl*_tmp3EE;struct _fat_ptr _tmp3EF;char _tmp3F0;int _tmp3F2;enum Cyc_Absyn_Sign _tmp3F1;switch(*((int*)_tmp3CF)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3F3="_";_tag_fat(_tmp3F3,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp3F4="NULL";_tag_fat(_tmp3F4,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp3F1=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_tmp3F2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp3F1;int i=_tmp3F2;
# 1546
if((int)sg != (int)1U)
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3F7=({struct Cyc_Int_pa_PrintArg_struct _tmp589;_tmp589.tag=1U,_tmp589.f1=(unsigned long)i;_tmp589;});void*_tmp3F5[1U];_tmp3F5[0]=& _tmp3F7;({struct _fat_ptr _tmp78B=({const char*_tmp3F6="%d";_tag_fat(_tmp3F6,sizeof(char),3U);});Cyc_aprintf(_tmp78B,_tag_fat(_tmp3F5,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3FA=({struct Cyc_Int_pa_PrintArg_struct _tmp58A;_tmp58A.tag=1U,_tmp58A.f1=(unsigned)i;_tmp58A;});void*_tmp3F8[1U];_tmp3F8[0]=& _tmp3FA;({struct _fat_ptr _tmp78C=({const char*_tmp3F9="%u";_tag_fat(_tmp3F9,sizeof(char),3U);});Cyc_aprintf(_tmp78C,_tag_fat(_tmp3F8,sizeof(void*),1U));});}));}
goto _LL0;}case 11U: _LL7: _tmp3F0=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_LL8: {char ch=_tmp3F0;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3FD=({struct Cyc_String_pa_PrintArg_struct _tmp58B;_tmp58B.tag=0U,({struct _fat_ptr _tmp78D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp58B.f1=_tmp78D;});_tmp58B;});void*_tmp3FB[1U];_tmp3FB[0]=& _tmp3FD;({struct _fat_ptr _tmp78E=({const char*_tmp3FC="'%s'";_tag_fat(_tmp3FC,sizeof(char),5U);});Cyc_aprintf(_tmp78E,_tag_fat(_tmp3FB,sizeof(void*),1U));});}));goto _LL0;}case 12U: _LL9: _tmp3EF=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_LLA: {struct _fat_ptr x=_tmp3EF;
s=Cyc_PP_text(x);goto _LL0;}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2)->r)->tag == 0U){_LLB: _tmp3EE=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp3EE;
# 1553
s=Cyc_Absynpp_qvar2doc(vd->name);goto _LL0;}}else{_LLD: _tmp3EC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_tmp3ED=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp3EC;struct Cyc_Absyn_Pat*p2=_tmp3ED;
# 1555
s=({struct Cyc_PP_Doc*_tmp3FE[3U];({struct Cyc_PP_Doc*_tmp791=Cyc_Absynpp_qvar2doc(vd->name);_tmp3FE[0]=_tmp791;}),({struct Cyc_PP_Doc*_tmp790=Cyc_PP_text(({const char*_tmp3FF=" as ";_tag_fat(_tmp3FF,sizeof(char),5U);}));_tmp3FE[1]=_tmp790;}),({struct Cyc_PP_Doc*_tmp78F=Cyc_Absynpp_pat2doc(p2);_tmp3FE[2]=_tmp78F;});Cyc_PP_cat(_tag_fat(_tmp3FE,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 2U: _LLF: _tmp3EA=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_tmp3EB=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp3EA;struct Cyc_Absyn_Vardecl*vd=_tmp3EB;
# 1557
s=({struct Cyc_PP_Doc*_tmp400[5U];({struct Cyc_PP_Doc*_tmp796=Cyc_PP_text(({const char*_tmp401="alias";_tag_fat(_tmp401,sizeof(char),6U);}));_tmp400[0]=_tmp796;}),({struct Cyc_PP_Doc*_tmp795=Cyc_PP_text(({const char*_tmp402=" <";_tag_fat(_tmp402,sizeof(char),3U);}));_tmp400[1]=_tmp795;}),({struct Cyc_PP_Doc*_tmp794=Cyc_Absynpp_tvar2doc(tv);_tmp400[2]=_tmp794;}),({struct Cyc_PP_Doc*_tmp793=Cyc_PP_text(({const char*_tmp403="> ";_tag_fat(_tmp403,sizeof(char),3U);}));_tmp400[3]=_tmp793;}),({
struct Cyc_PP_Doc*_tmp792=Cyc_Absynpp_vardecl2doc(vd);_tmp400[4]=_tmp792;});Cyc_PP_cat(_tag_fat(_tmp400,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 4U: _LL11: _tmp3E8=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_tmp3E9=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp3E8;struct Cyc_Absyn_Vardecl*vd=_tmp3E9;
# 1561
s=({struct Cyc_PP_Doc*_tmp404[4U];({struct Cyc_PP_Doc*_tmp79A=Cyc_Absynpp_qvar2doc(vd->name);_tmp404[0]=_tmp79A;}),({struct Cyc_PP_Doc*_tmp799=Cyc_PP_text(({const char*_tmp405="<";_tag_fat(_tmp405,sizeof(char),2U);}));_tmp404[1]=_tmp799;}),({struct Cyc_PP_Doc*_tmp798=Cyc_Absynpp_tvar2doc(tv);_tmp404[2]=_tmp798;}),({struct Cyc_PP_Doc*_tmp797=Cyc_PP_text(({const char*_tmp406=">";_tag_fat(_tmp406,sizeof(char),2U);}));_tmp404[3]=_tmp797;});Cyc_PP_cat(_tag_fat(_tmp404,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 5U: _LL13: _tmp3E6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_tmp3E7=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LL14: {struct Cyc_List_List*ts=_tmp3E6;int dots=_tmp3E7;
# 1564
s=({struct Cyc_PP_Doc*_tmp407[4U];({struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_dollar();_tmp407[0]=_tmp7A0;}),({struct Cyc_PP_Doc*_tmp79F=Cyc_PP_text(({const char*_tmp408="(";_tag_fat(_tmp408,sizeof(char),2U);}));_tmp407[1]=_tmp79F;}),({struct Cyc_PP_Doc*_tmp79E=({struct _fat_ptr _tmp79D=({const char*_tmp409=",";_tag_fat(_tmp409,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp79D,ts);});_tmp407[2]=_tmp79E;}),
dots?({struct Cyc_PP_Doc*_tmp79C=Cyc_PP_text(({const char*_tmp40A=", ...)";_tag_fat(_tmp40A,sizeof(char),7U);}));_tmp407[3]=_tmp79C;}):({struct Cyc_PP_Doc*_tmp79B=Cyc_PP_text(({const char*_tmp40B=")";_tag_fat(_tmp40B,sizeof(char),2U);}));_tmp407[3]=_tmp79B;});Cyc_PP_cat(_tag_fat(_tmp407,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LL15: _tmp3E5=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_LL16: {struct Cyc_Absyn_Pat*p2=_tmp3E5;
# 1568
s=({struct Cyc_PP_Doc*_tmp40C[2U];({struct Cyc_PP_Doc*_tmp7A2=Cyc_PP_text(({const char*_tmp40D="&";_tag_fat(_tmp40D,sizeof(char),2U);}));_tmp40C[0]=_tmp7A2;}),({struct Cyc_PP_Doc*_tmp7A1=Cyc_Absynpp_pat2doc(p2);_tmp40C[1]=_tmp7A1;});Cyc_PP_cat(_tag_fat(_tmp40C,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2)->r)->tag == 0U){_LL17: _tmp3E4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp3E4;
# 1571
s=({struct Cyc_PP_Doc*_tmp40E[2U];({struct Cyc_PP_Doc*_tmp7A4=Cyc_PP_text(({const char*_tmp40F="*";_tag_fat(_tmp40F,sizeof(char),2U);}));_tmp40E[0]=_tmp7A4;}),({struct Cyc_PP_Doc*_tmp7A3=Cyc_Absynpp_qvar2doc(vd->name);_tmp40E[1]=_tmp7A3;});Cyc_PP_cat(_tag_fat(_tmp40E,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}else{_LL19: _tmp3E2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_tmp3E3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp3E2;struct Cyc_Absyn_Pat*p2=_tmp3E3;
# 1574
s=({struct Cyc_PP_Doc*_tmp410[4U];({struct Cyc_PP_Doc*_tmp7A8=Cyc_PP_text(({const char*_tmp411="*";_tag_fat(_tmp411,sizeof(char),2U);}));_tmp410[0]=_tmp7A8;}),({struct Cyc_PP_Doc*_tmp7A7=Cyc_Absynpp_qvar2doc(vd->name);_tmp410[1]=_tmp7A7;}),({struct Cyc_PP_Doc*_tmp7A6=Cyc_PP_text(({const char*_tmp412=" as ";_tag_fat(_tmp412,sizeof(char),5U);}));_tmp410[2]=_tmp7A6;}),({struct Cyc_PP_Doc*_tmp7A5=Cyc_Absynpp_pat2doc(p2);_tmp410[3]=_tmp7A5;});Cyc_PP_cat(_tag_fat(_tmp410,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}case 15U: _LL1B: _tmp3E1=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_LL1C: {struct _tuple0*q=_tmp3E1;
# 1577
s=Cyc_Absynpp_qvar2doc(q);
goto _LL0;}case 16U: _LL1D: _tmp3DE=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_tmp3DF=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_tmp3E0=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3CF)->f3;_LL1E: {struct _tuple0*q=_tmp3DE;struct Cyc_List_List*ps=_tmp3DF;int dots=_tmp3E0;
# 1580
struct _fat_ptr term=dots?({const char*_tmp416=", ...)";_tag_fat(_tmp416,sizeof(char),7U);}):({const char*_tmp417=")";_tag_fat(_tmp417,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp413[2U];({struct Cyc_PP_Doc*_tmp7AD=Cyc_Absynpp_qvar2doc(q);_tmp413[0]=_tmp7AD;}),({struct Cyc_PP_Doc*_tmp7AC=({struct _fat_ptr _tmp7AB=({const char*_tmp414="(";_tag_fat(_tmp414,sizeof(char),2U);});struct _fat_ptr _tmp7AA=term;struct _fat_ptr _tmp7A9=({const char*_tmp415=",";_tag_fat(_tmp415,sizeof(char),2U);});Cyc_PP_group(_tmp7AB,_tmp7AA,_tmp7A9,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp413[1]=_tmp7AC;});Cyc_PP_cat(_tag_fat(_tmp413,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1 != 0){_LL1F: _tmp3DA=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_tmp3DB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_tmp3DC=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CF)->f3;_tmp3DD=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CF)->f4;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp3DA;struct Cyc_List_List*exists=_tmp3DB;struct Cyc_List_List*dps=_tmp3DC;int dots=_tmp3DD;
# 1584
struct _fat_ptr term=dots?({const char*_tmp421=", ...}";_tag_fat(_tmp421,sizeof(char),7U);}):({const char*_tmp422="}";_tag_fat(_tmp422,sizeof(char),2U);});
struct _tuple11 _tmp418=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp1D=_tmp418;struct _tuple11 _tmp419=_stmttmp1D;struct _tuple0*_tmp41A;_LL2E: _tmp41A=_tmp419.f2;_LL2F: {struct _tuple0*n=_tmp41A;
s=({struct Cyc_PP_Doc*_tmp41B[4U];({struct Cyc_PP_Doc*_tmp7B7=Cyc_Absynpp_qvar2doc(n);_tmp41B[0]=_tmp7B7;}),({struct Cyc_PP_Doc*_tmp7B6=Cyc_Absynpp_lb();_tmp41B[1]=_tmp7B6;}),({
struct Cyc_PP_Doc*_tmp7B5=({struct _fat_ptr _tmp7B4=({const char*_tmp41C="[";_tag_fat(_tmp41C,sizeof(char),2U);});struct _fat_ptr _tmp7B3=({const char*_tmp41D="]";_tag_fat(_tmp41D,sizeof(char),2U);});struct _fat_ptr _tmp7B2=({const char*_tmp41E=",";_tag_fat(_tmp41E,sizeof(char),2U);});Cyc_PP_egroup(_tmp7B4,_tmp7B3,_tmp7B2,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp41B[2]=_tmp7B5;}),({
struct Cyc_PP_Doc*_tmp7B1=({struct _fat_ptr _tmp7B0=({const char*_tmp41F="";_tag_fat(_tmp41F,sizeof(char),1U);});struct _fat_ptr _tmp7AF=term;struct _fat_ptr _tmp7AE=({const char*_tmp420=",";_tag_fat(_tmp420,sizeof(char),2U);});Cyc_PP_group(_tmp7B0,_tmp7AF,_tmp7AE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp41B[3]=_tmp7B1;});Cyc_PP_cat(_tag_fat(_tmp41B,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}}else{_LL21: _tmp3D7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_tmp3D8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CF)->f3;_tmp3D9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3CF)->f4;_LL22: {struct Cyc_List_List*exists=_tmp3D7;struct Cyc_List_List*dps=_tmp3D8;int dots=_tmp3D9;
# 1591
struct _fat_ptr term=dots?({const char*_tmp429=", ...}";_tag_fat(_tmp429,sizeof(char),7U);}):({const char*_tmp42A="}";_tag_fat(_tmp42A,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp423[3U];({struct Cyc_PP_Doc*_tmp7C0=Cyc_Absynpp_lb();_tmp423[0]=_tmp7C0;}),({
struct Cyc_PP_Doc*_tmp7BF=({struct _fat_ptr _tmp7BE=({const char*_tmp424="[";_tag_fat(_tmp424,sizeof(char),2U);});struct _fat_ptr _tmp7BD=({const char*_tmp425="]";_tag_fat(_tmp425,sizeof(char),2U);});struct _fat_ptr _tmp7BC=({const char*_tmp426=",";_tag_fat(_tmp426,sizeof(char),2U);});Cyc_PP_egroup(_tmp7BE,_tmp7BD,_tmp7BC,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp423[1]=_tmp7BF;}),({
struct Cyc_PP_Doc*_tmp7BB=({struct _fat_ptr _tmp7BA=({const char*_tmp427="";_tag_fat(_tmp427,sizeof(char),1U);});struct _fat_ptr _tmp7B9=term;struct _fat_ptr _tmp7B8=({const char*_tmp428=",";_tag_fat(_tmp428,sizeof(char),2U);});Cyc_PP_group(_tmp7BA,_tmp7B9,_tmp7B8,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp423[2]=_tmp7BB;});Cyc_PP_cat(_tag_fat(_tmp423,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp3D6=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp3D6;
_tmp3D5=ef;goto _LL26;}case 14U: _LL25: _tmp3D5=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp3D5;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CF)->f3 == 0){_LL27: _tmp3D4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_LL28: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3D4;
s=Cyc_Absynpp_qvar2doc(tuf->name);goto _LL0;}}else{_LL29: _tmp3D1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CF)->f2;_tmp3D2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CF)->f3;_tmp3D3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3CF)->f4;_LL2A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3D1;struct Cyc_List_List*ps=_tmp3D2;int dots=_tmp3D3;
# 1600
struct _fat_ptr term=dots?({const char*_tmp42E=", ...)";_tag_fat(_tmp42E,sizeof(char),7U);}):({const char*_tmp42F=")";_tag_fat(_tmp42F,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp42B[2U];({struct Cyc_PP_Doc*_tmp7C5=Cyc_Absynpp_qvar2doc(tuf->name);_tmp42B[0]=_tmp7C5;}),({struct Cyc_PP_Doc*_tmp7C4=({struct _fat_ptr _tmp7C3=({const char*_tmp42C="(";_tag_fat(_tmp42C,sizeof(char),2U);});struct _fat_ptr _tmp7C2=term;struct _fat_ptr _tmp7C1=({const char*_tmp42D=",";_tag_fat(_tmp42D,sizeof(char),2U);});Cyc_PP_egroup(_tmp7C3,_tmp7C2,_tmp7C1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp42B[1]=_tmp7C4;});Cyc_PP_cat(_tag_fat(_tmp42B,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp3D0=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3CF)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp3D0;
# 1604
s=Cyc_Absynpp_exp2doc(e);goto _LL0;}}_LL0:;}
# 1606
return s;}
# 1609
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp){
return({struct Cyc_PP_Doc*_tmp430[2U];({struct Cyc_PP_Doc*_tmp7CA=({struct _fat_ptr _tmp7C9=({const char*_tmp431="";_tag_fat(_tmp431,sizeof(char),1U);});struct _fat_ptr _tmp7C8=({const char*_tmp432="=";_tag_fat(_tmp432,sizeof(char),2U);});struct _fat_ptr _tmp7C7=({const char*_tmp433="=";_tag_fat(_tmp433,sizeof(char),2U);});Cyc_PP_egroup(_tmp7C9,_tmp7C8,_tmp7C7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp430[0]=_tmp7CA;}),({
struct Cyc_PP_Doc*_tmp7C6=Cyc_Absynpp_pat2doc((*dp).f2);_tmp430[1]=_tmp7C6;});Cyc_PP_cat(_tag_fat(_tmp430,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1614
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1616
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp434[2U];({struct Cyc_PP_Doc*_tmp7CD=Cyc_PP_text(({const char*_tmp435="default: ";_tag_fat(_tmp435,sizeof(char),10U);}));_tmp434[0]=_tmp7CD;}),({
struct Cyc_PP_Doc*_tmp7CC=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp436[2U];({struct Cyc_PP_Doc*_tmp7CB=Cyc_PP_line_doc();_tmp436[0]=_tmp7CB;}),_tmp436[1]=body;Cyc_PP_cat(_tag_fat(_tmp436,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp434[1]=_tmp7CC;});Cyc_PP_cat(_tag_fat(_tmp434,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp437[4U];({struct Cyc_PP_Doc*_tmp7D2=Cyc_PP_text(({const char*_tmp438="case ";_tag_fat(_tmp438,sizeof(char),6U);}));_tmp437[0]=_tmp7D2;}),({
struct Cyc_PP_Doc*_tmp7D1=Cyc_Absynpp_pat2doc(c->pattern);_tmp437[1]=_tmp7D1;}),({
struct Cyc_PP_Doc*_tmp7D0=Cyc_PP_text(({const char*_tmp439=": ";_tag_fat(_tmp439,sizeof(char),3U);}));_tmp437[2]=_tmp7D0;}),({
struct Cyc_PP_Doc*_tmp7CF=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43A[2U];({struct Cyc_PP_Doc*_tmp7CE=Cyc_PP_line_doc();_tmp43A[0]=_tmp7CE;}),_tmp43A[1]=body;Cyc_PP_cat(_tag_fat(_tmp43A,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp437[3]=_tmp7CF;});Cyc_PP_cat(_tag_fat(_tmp437,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1628
return({struct Cyc_PP_Doc*_tmp43B[6U];({struct Cyc_PP_Doc*_tmp7D9=Cyc_PP_text(({const char*_tmp43C="case ";_tag_fat(_tmp43C,sizeof(char),6U);}));_tmp43B[0]=_tmp7D9;}),({
struct Cyc_PP_Doc*_tmp7D8=Cyc_Absynpp_pat2doc(c->pattern);_tmp43B[1]=_tmp7D8;}),({
struct Cyc_PP_Doc*_tmp7D7=Cyc_PP_text(({const char*_tmp43D=" && ";_tag_fat(_tmp43D,sizeof(char),5U);}));_tmp43B[2]=_tmp7D7;}),({
struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp43B[3]=_tmp7D6;}),({
struct Cyc_PP_Doc*_tmp7D5=Cyc_PP_text(({const char*_tmp43E=": ";_tag_fat(_tmp43E,sizeof(char),3U);}));_tmp43B[4]=_tmp7D5;}),({
struct Cyc_PP_Doc*_tmp7D4=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43F[2U];({struct Cyc_PP_Doc*_tmp7D3=Cyc_PP_line_doc();_tmp43F[0]=_tmp7D3;}),_tmp43F[1]=body;Cyc_PP_cat(_tag_fat(_tmp43F,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp43B[5]=_tmp7D4;});Cyc_PP_cat(_tag_fat(_tmp43B,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1636
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _fat_ptr _tmp7DA=({const char*_tmp440="";_tag_fat(_tmp440,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp7DA,cs);});}
# 1640
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1644
return({struct Cyc_PP_Doc*_tmp441[3U];({struct Cyc_PP_Doc*_tmp7DD=Cyc_Absynpp_qvar2doc(f->name);_tmp441[0]=_tmp7DD;}),({struct Cyc_PP_Doc*_tmp7DC=Cyc_PP_text(({const char*_tmp442=" = ";_tag_fat(_tmp442,sizeof(char),4U);}));_tmp441[1]=_tmp7DC;}),({struct Cyc_PP_Doc*_tmp7DB=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp441[2]=_tmp7DB;});Cyc_PP_cat(_tag_fat(_tmp441,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1647
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _fat_ptr _tmp7DE=({const char*_tmp443=",";_tag_fat(_tmp443,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp7DE,fs);});}
# 1651
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1655
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _fat_ptr _tmp7DF=({const char*_tmp444=",";_tag_fat(_tmp444,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp7DF,vds);});}
# 1659
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp445=vd;struct Cyc_List_List*_tmp44C;struct Cyc_Absyn_Exp*_tmp44B;void*_tmp44A;struct Cyc_Absyn_Tqual _tmp449;unsigned _tmp448;struct _tuple0*_tmp447;enum Cyc_Absyn_Scope _tmp446;_LL1: _tmp446=_tmp445->sc;_tmp447=_tmp445->name;_tmp448=_tmp445->varloc;_tmp449=_tmp445->tq;_tmp44A=_tmp445->type;_tmp44B=_tmp445->initializer;_tmp44C=_tmp445->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp446;struct _tuple0*name=_tmp447;unsigned varloc=_tmp448;struct Cyc_Absyn_Tqual tq=_tmp449;void*type=_tmp44A;struct Cyc_Absyn_Exp*initializer=_tmp44B;struct Cyc_List_List*atts=_tmp44C;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp44D=Cyc_Cyclone_c_compiler;if(_tmp44D == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1668
{void*_tmp44E=Cyc_Tcutil_compress(type);void*_stmttmp1E=_tmp44E;void*_tmp44F=_stmttmp1E;struct Cyc_List_List*_tmp450;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->tag == 5U){_LL9: _tmp450=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).attributes;_LLA: {struct Cyc_List_List*atts2=_tmp450;
# 1670
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1674
goto _LL3;}_LL3:;}{
# 1677
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp451=Cyc_Cyclone_c_compiler;if(_tmp451 == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1682
s=({struct Cyc_PP_Doc*_tmp452[5U];_tmp452[0]=tmp_doc,({
# 1684
struct Cyc_PP_Doc*_tmp7E9=Cyc_Absynpp_scope2doc(sc);_tmp452[1]=_tmp7E9;}),({
struct Cyc_PP_Doc*_tmp7E8=({struct Cyc_Absyn_Tqual _tmp7E7=tq;void*_tmp7E6=type;Cyc_Absynpp_tqtd2doc(_tmp7E7,_tmp7E6,({struct Cyc_Core_Opt*_tmp454=_cycalloc(sizeof(*_tmp454));({struct Cyc_PP_Doc*_tmp7E5=({struct Cyc_PP_Doc*_tmp453[2U];_tmp453[0]=beforenamedoc,_tmp453[1]=sn;Cyc_PP_cat(_tag_fat(_tmp453,sizeof(struct Cyc_PP_Doc*),2U));});_tmp454->v=_tmp7E5;});_tmp454;}));});_tmp452[2]=_tmp7E8;}),
initializer == 0?({
struct Cyc_PP_Doc*_tmp7E4=Cyc_PP_nil_doc();_tmp452[3]=_tmp7E4;}):({
struct Cyc_PP_Doc*_tmp7E3=({struct Cyc_PP_Doc*_tmp455[2U];({struct Cyc_PP_Doc*_tmp7E2=Cyc_PP_text(({const char*_tmp456=" = ";_tag_fat(_tmp456,sizeof(char),4U);}));_tmp455[0]=_tmp7E2;}),({struct Cyc_PP_Doc*_tmp7E1=Cyc_Absynpp_exp2doc(initializer);_tmp455[1]=_tmp7E1;});Cyc_PP_cat(_tag_fat(_tmp455,sizeof(struct Cyc_PP_Doc*),2U));});_tmp452[3]=_tmp7E3;}),({
struct Cyc_PP_Doc*_tmp7E0=Cyc_PP_text(({const char*_tmp457=";";_tag_fat(_tmp457,sizeof(char),2U);}));_tmp452[4]=_tmp7E0;});Cyc_PP_cat(_tag_fat(_tmp452,sizeof(struct Cyc_PP_Doc*),5U));});
return s;}}}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1693
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
struct _tuple20 _tmp458=*x;struct _tuple20 _stmttmp1F=_tmp458;struct _tuple20 _tmp459=_stmttmp1F;struct _tuple0*_tmp45A;_LL1: _tmp45A=_tmp459.f2;_LL2: {struct _tuple0*v=_tmp45A;
return Cyc_Absynpp_qvar2doc(v);}}
# 1698
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp45B[4U];({struct Cyc_PP_Doc*_tmp7ED=Cyc_Absynpp_scope2doc(ad->sc);_tmp45B[0]=_tmp7ED;}),({
struct Cyc_PP_Doc*_tmp7EC=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp45B[1]=_tmp7EC;}),({
struct Cyc_PP_Doc*_tmp7EB=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp45B[2]=_tmp7EB;}),({
struct Cyc_PP_Doc*_tmp7EA=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp45B[3]=_tmp7EA;});Cyc_PP_cat(_tag_fat(_tmp45B,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1705
return({struct Cyc_PP_Doc*_tmp45C[12U];({struct Cyc_PP_Doc*_tmp7FE=Cyc_Absynpp_scope2doc(ad->sc);_tmp45C[0]=_tmp7FE;}),({
struct Cyc_PP_Doc*_tmp7FD=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp45C[1]=_tmp7FD;}),({
struct Cyc_PP_Doc*_tmp7FC=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp45C[2]=_tmp7FC;}),({
struct Cyc_PP_Doc*_tmp7FB=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp45C[3]=_tmp7FB;}),({
struct Cyc_PP_Doc*_tmp7FA=Cyc_PP_blank_doc();_tmp45C[4]=_tmp7FA;}),({struct Cyc_PP_Doc*_tmp7F9=Cyc_Absynpp_lb();_tmp45C[5]=_tmp7F9;}),({
struct Cyc_PP_Doc*_tmp7F8=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp45C[6]=_tmp7F8;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp7F7=Cyc_PP_nil_doc();_tmp45C[7]=_tmp7F7;}):({
struct Cyc_PP_Doc*_tmp7F6=({struct Cyc_PP_Doc*_tmp45D[2U];({struct Cyc_PP_Doc*_tmp7F5=Cyc_PP_text(({const char*_tmp45E=":";_tag_fat(_tmp45E,sizeof(char),2U);}));_tmp45D[0]=_tmp7F5;}),({struct Cyc_PP_Doc*_tmp7F4=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp45D[1]=_tmp7F4;});Cyc_PP_cat(_tag_fat(_tmp45D,sizeof(struct Cyc_PP_Doc*),2U));});_tmp45C[7]=_tmp7F6;}),({
struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp45F[2U];({struct Cyc_PP_Doc*_tmp7F2=Cyc_PP_line_doc();_tmp45F[0]=_tmp7F2;}),({struct Cyc_PP_Doc*_tmp7F1=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp45F[1]=_tmp7F1;});Cyc_PP_cat(_tag_fat(_tmp45F,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp45C[8]=_tmp7F3;}),({
struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_line_doc();_tmp45C[9]=_tmp7F0;}),({
struct Cyc_PP_Doc*_tmp7EF=Cyc_Absynpp_rb();_tmp45C[10]=_tmp7EF;}),({
struct Cyc_PP_Doc*_tmp7EE=Cyc_Absynpp_atts2doc(ad->attributes);_tmp45C[11]=_tmp7EE;});Cyc_PP_cat(_tag_fat(_tmp45C,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1719
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp460=dd;int _tmp465;struct Cyc_Core_Opt*_tmp464;struct Cyc_List_List*_tmp463;struct _tuple0*_tmp462;enum Cyc_Absyn_Scope _tmp461;_LL1: _tmp461=_tmp460->sc;_tmp462=_tmp460->name;_tmp463=_tmp460->tvs;_tmp464=_tmp460->fields;_tmp465=_tmp460->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp461;struct _tuple0*name=_tmp462;struct Cyc_List_List*tvs=_tmp463;struct Cyc_Core_Opt*fields=_tmp464;int is_x=_tmp465;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp466[5U];({struct Cyc_PP_Doc*_tmp805=Cyc_Absynpp_scope2doc(sc);_tmp466[0]=_tmp805;}),
is_x?({struct Cyc_PP_Doc*_tmp804=Cyc_PP_text(({const char*_tmp467="@extensible ";_tag_fat(_tmp467,sizeof(char),13U);}));_tmp466[1]=_tmp804;}):({struct Cyc_PP_Doc*_tmp803=Cyc_PP_blank_doc();_tmp466[1]=_tmp803;}),({
struct Cyc_PP_Doc*_tmp802=Cyc_PP_text(({const char*_tmp468="datatype ";_tag_fat(_tmp468,sizeof(char),10U);}));_tmp466[2]=_tmp802;}),
is_x?({struct Cyc_PP_Doc*_tmp801=Cyc_Absynpp_qvar2bolddoc(name);_tmp466[3]=_tmp801;}):({struct Cyc_PP_Doc*_tmp800=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp466[3]=_tmp800;}),({
struct Cyc_PP_Doc*_tmp7FF=Cyc_Absynpp_ktvars2doc(tvs);_tmp466[4]=_tmp7FF;});Cyc_PP_cat(_tag_fat(_tmp466,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1728
return({struct Cyc_PP_Doc*_tmp469[10U];({struct Cyc_PP_Doc*_tmp813=Cyc_Absynpp_scope2doc(sc);_tmp469[0]=_tmp813;}),
is_x?({struct Cyc_PP_Doc*_tmp812=Cyc_PP_text(({const char*_tmp46A="@extensible ";_tag_fat(_tmp46A,sizeof(char),13U);}));_tmp469[1]=_tmp812;}):({struct Cyc_PP_Doc*_tmp811=Cyc_PP_blank_doc();_tmp469[1]=_tmp811;}),({
struct Cyc_PP_Doc*_tmp810=Cyc_PP_text(({const char*_tmp46B="datatype ";_tag_fat(_tmp46B,sizeof(char),10U);}));_tmp469[2]=_tmp810;}),
is_x?({struct Cyc_PP_Doc*_tmp80F=Cyc_Absynpp_qvar2bolddoc(name);_tmp469[3]=_tmp80F;}):({struct Cyc_PP_Doc*_tmp80E=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp469[3]=_tmp80E;}),({
struct Cyc_PP_Doc*_tmp80D=Cyc_Absynpp_ktvars2doc(tvs);_tmp469[4]=_tmp80D;}),({
struct Cyc_PP_Doc*_tmp80C=Cyc_PP_blank_doc();_tmp469[5]=_tmp80C;}),({struct Cyc_PP_Doc*_tmp80B=Cyc_Absynpp_lb();_tmp469[6]=_tmp80B;}),({
struct Cyc_PP_Doc*_tmp80A=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp46C[2U];({struct Cyc_PP_Doc*_tmp809=Cyc_PP_line_doc();_tmp46C[0]=_tmp809;}),({struct Cyc_PP_Doc*_tmp808=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_tmp46C[1]=_tmp808;});Cyc_PP_cat(_tag_fat(_tmp46C,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp469[7]=_tmp80A;}),({
struct Cyc_PP_Doc*_tmp807=Cyc_PP_line_doc();_tmp469[8]=_tmp807;}),({
struct Cyc_PP_Doc*_tmp806=Cyc_Absynpp_rb();_tmp469[9]=_tmp806;});Cyc_PP_cat(_tag_fat(_tmp469,sizeof(struct Cyc_PP_Doc*),10U));});}}}
# 1739
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp46D=ed;struct Cyc_Core_Opt*_tmp470;struct _tuple0*_tmp46F;enum Cyc_Absyn_Scope _tmp46E;_LL1: _tmp46E=_tmp46D->sc;_tmp46F=_tmp46D->name;_tmp470=_tmp46D->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp46E;struct _tuple0*n=_tmp46F;struct Cyc_Core_Opt*fields=_tmp470;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp471[3U];({struct Cyc_PP_Doc*_tmp816=Cyc_Absynpp_scope2doc(sc);_tmp471[0]=_tmp816;}),({
struct Cyc_PP_Doc*_tmp815=Cyc_PP_text(({const char*_tmp472="enum ";_tag_fat(_tmp472,sizeof(char),6U);}));_tmp471[1]=_tmp815;}),({
struct Cyc_PP_Doc*_tmp814=Cyc_Absynpp_typedef_name2bolddoc(n);_tmp471[2]=_tmp814;});Cyc_PP_cat(_tag_fat(_tmp471,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1747
return({struct Cyc_PP_Doc*_tmp473[8U];({struct Cyc_PP_Doc*_tmp820=Cyc_Absynpp_scope2doc(sc);_tmp473[0]=_tmp820;}),({
struct Cyc_PP_Doc*_tmp81F=Cyc_PP_text(({const char*_tmp474="enum ";_tag_fat(_tmp474,sizeof(char),6U);}));_tmp473[1]=_tmp81F;}),({
struct Cyc_PP_Doc*_tmp81E=Cyc_Absynpp_qvar2bolddoc(n);_tmp473[2]=_tmp81E;}),({
struct Cyc_PP_Doc*_tmp81D=Cyc_PP_blank_doc();_tmp473[3]=_tmp81D;}),({struct Cyc_PP_Doc*_tmp81C=Cyc_Absynpp_lb();_tmp473[4]=_tmp81C;}),({
struct Cyc_PP_Doc*_tmp81B=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp475[2U];({struct Cyc_PP_Doc*_tmp81A=Cyc_PP_line_doc();_tmp475[0]=_tmp81A;}),({struct Cyc_PP_Doc*_tmp819=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_tmp475[1]=_tmp819;});Cyc_PP_cat(_tag_fat(_tmp475,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp473[5]=_tmp81B;}),({
struct Cyc_PP_Doc*_tmp818=Cyc_PP_line_doc();_tmp473[6]=_tmp818;}),({
struct Cyc_PP_Doc*_tmp817=Cyc_Absynpp_rb();_tmp473[7]=_tmp817;});Cyc_PP_cat(_tag_fat(_tmp473,sizeof(struct Cyc_PP_Doc*),8U));});}}}
# 1756
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp476=d->r;void*_stmttmp20=_tmp476;void*_tmp477=_stmttmp20;struct _tuple10*_tmp47B;struct Cyc_List_List*_tmp47A;struct Cyc_List_List*_tmp479;struct Cyc_List_List*_tmp478;struct Cyc_List_List*_tmp47C;struct Cyc_List_List*_tmp47E;struct _tuple0*_tmp47D;struct Cyc_List_List*_tmp480;struct _fat_ptr*_tmp47F;struct Cyc_Absyn_Typedefdecl*_tmp481;struct Cyc_Absyn_Enumdecl*_tmp482;struct Cyc_List_List*_tmp483;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Pat*_tmp484;struct Cyc_Absyn_Datatypedecl*_tmp486;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Vardecl*_tmp488;struct Cyc_Absyn_Tvar*_tmp487;struct Cyc_Absyn_Vardecl*_tmp48A;struct Cyc_Absyn_Aggrdecl*_tmp48B;struct Cyc_Absyn_Fndecl*_tmp48C;switch(*((int*)_tmp477)){case 1U: _LL1: _tmp48C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp48C;
# 1760
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->tag=5U,_tmp49D->f1=type_info;_tmp49D;});
if(fd->cached_type != 0){
void*_tmp48D=Cyc_Tcutil_compress((void*)_check_null(fd->cached_type));void*_stmttmp21=_tmp48D;void*_tmp48E=_stmttmp21;struct Cyc_Absyn_FnInfo _tmp48F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48E)->tag == 5U){_LL24: _tmp48F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48E)->f1;_LL25: {struct Cyc_Absyn_FnInfo i=_tmp48F;
# 1766
({struct Cyc_List_List*_tmp821=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp821;});goto _LL23;}}else{_LL26: _LL27:
({void*_tmp490=0U;({struct _fat_ptr _tmp822=({const char*_tmp491="function has non-function type";_tag_fat(_tmp491,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp822,_tag_fat(_tmp490,sizeof(void*),0U));});});}_LL23:;}{
# 1769
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((fd->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp492=Cyc_Cyclone_c_compiler;if(_tmp492 == Cyc_Cyclone_Gcc_c){_LL29: _LL2A:
 inlinedoc=Cyc_PP_text(({const char*_tmp493="inline ";_tag_fat(_tmp493,sizeof(char),8U);}));goto _LL28;}else{_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp494="__inline ";_tag_fat(_tmp494,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1777
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp495=Cyc_Cyclone_c_compiler;if(_tmp495 == Cyc_Cyclone_Gcc_c){_LL2E: _LL2F:
 beforenamedoc=attsdoc;goto _LL2D;}else{_LL30: _LL31:
 beforenamedoc=Cyc_Absynpp_callconv2doc((fd->i).attributes);goto _LL2D;}_LL2D:;}{
# 1784
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp825=Cyc_Absyn_empty_tqual(0U);void*_tmp824=t;Cyc_Absynpp_tqtd2doc(_tmp825,_tmp824,({struct Cyc_Core_Opt*_tmp49C=_cycalloc(sizeof(*_tmp49C));({
struct Cyc_PP_Doc*_tmp823=({struct Cyc_PP_Doc*_tmp49B[2U];_tmp49B[0]=beforenamedoc,_tmp49B[1]=namedoc;Cyc_PP_cat(_tag_fat(_tmp49B,sizeof(struct Cyc_PP_Doc*),2U));});_tmp49C->v=_tmp823;});_tmp49C;}));});
# 1792
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp499[5U];({struct Cyc_PP_Doc*_tmp82C=Cyc_PP_blank_doc();_tmp499[0]=_tmp82C;}),({struct Cyc_PP_Doc*_tmp82B=Cyc_Absynpp_lb();_tmp499[1]=_tmp82B;}),({
struct Cyc_PP_Doc*_tmp82A=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp49A[2U];({struct Cyc_PP_Doc*_tmp829=Cyc_PP_line_doc();_tmp49A[0]=_tmp829;}),({struct Cyc_PP_Doc*_tmp828=Cyc_Absynpp_stmt2doc(fd->body,0,0);_tmp49A[1]=_tmp828;});Cyc_PP_cat(_tag_fat(_tmp49A,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp499[2]=_tmp82A;}),({
struct Cyc_PP_Doc*_tmp827=Cyc_PP_line_doc();_tmp499[3]=_tmp827;}),({
struct Cyc_PP_Doc*_tmp826=Cyc_Absynpp_rb();_tmp499[4]=_tmp826;});Cyc_PP_cat(_tag_fat(_tmp499,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp496[4U];_tmp496[0]=inlinedoc,_tmp496[1]=scopedoc,_tmp496[2]=tqtddoc,_tmp496[3]=bodydoc;Cyc_PP_cat(_tag_fat(_tmp496,sizeof(struct Cyc_PP_Doc*),4U));});
# 1798
{enum Cyc_Cyclone_C_Compilers _tmp497=Cyc_Cyclone_c_compiler;if(_tmp497 == Cyc_Cyclone_Vc_c){_LL33: _LL34:
 s=({struct Cyc_PP_Doc*_tmp498[2U];_tmp498[0]=attsdoc,_tmp498[1]=s;Cyc_PP_cat(_tag_fat(_tmp498,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 1803
goto _LL0;}}}}}case 5U: _LL3: _tmp48B=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp48B;
# 1806
s=({struct Cyc_PP_Doc*_tmp49E[2U];({struct Cyc_PP_Doc*_tmp82E=Cyc_Absynpp_aggrdecl2doc(ad);_tmp49E[0]=_tmp82E;}),({struct Cyc_PP_Doc*_tmp82D=Cyc_PP_text(({const char*_tmp49F=";";_tag_fat(_tmp49F,sizeof(char),2U);}));_tmp49E[1]=_tmp82D;});Cyc_PP_cat(_tag_fat(_tmp49E,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 0U: _LL5: _tmp48A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp48A;
# 1809
s=Cyc_Absynpp_vardecl2doc(vd);
goto _LL0;}case 4U: _LL7: _tmp487=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_tmp488=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp477)->f2;_tmp489=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp477)->f3;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp487;struct Cyc_Absyn_Vardecl*vd=_tmp488;struct Cyc_Absyn_Exp*open_exp_opt=_tmp489;
# 1812
s=({struct Cyc_PP_Doc*_tmp4A0[7U];({struct Cyc_PP_Doc*_tmp839=Cyc_PP_text(({const char*_tmp4A1="region";_tag_fat(_tmp4A1,sizeof(char),7U);}));_tmp4A0[0]=_tmp839;}),({
struct Cyc_PP_Doc*_tmp838=Cyc_PP_text(({const char*_tmp4A2="<";_tag_fat(_tmp4A2,sizeof(char),2U);}));_tmp4A0[1]=_tmp838;}),({
struct Cyc_PP_Doc*_tmp837=Cyc_Absynpp_tvar2doc(tv);_tmp4A0[2]=_tmp837;}),({
struct Cyc_PP_Doc*_tmp836=Cyc_PP_text(({const char*_tmp4A3=">";_tag_fat(_tmp4A3,sizeof(char),2U);}));_tmp4A0[3]=_tmp836;}),({
struct Cyc_PP_Doc*_tmp835=Cyc_Absynpp_qvar2doc(vd->name);_tmp4A0[4]=_tmp835;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_tmp834=({struct Cyc_PP_Doc*_tmp4A4[3U];({struct Cyc_PP_Doc*_tmp833=Cyc_PP_text(({const char*_tmp4A5=" = open(";_tag_fat(_tmp4A5,sizeof(char),9U);}));_tmp4A4[0]=_tmp833;}),({struct Cyc_PP_Doc*_tmp832=Cyc_Absynpp_exp2doc(open_exp_opt);_tmp4A4[1]=_tmp832;}),({
struct Cyc_PP_Doc*_tmp831=Cyc_PP_text(({const char*_tmp4A6=")";_tag_fat(_tmp4A6,sizeof(char),2U);}));_tmp4A4[2]=_tmp831;});Cyc_PP_cat(_tag_fat(_tmp4A4,sizeof(struct Cyc_PP_Doc*),3U));});
# 1817
_tmp4A0[5]=_tmp834;}):({
struct Cyc_PP_Doc*_tmp830=Cyc_PP_nil_doc();_tmp4A0[5]=_tmp830;}),({
struct Cyc_PP_Doc*_tmp82F=Cyc_PP_text(({const char*_tmp4A7=";";_tag_fat(_tmp4A7,sizeof(char),2U);}));_tmp4A0[6]=_tmp82F;});Cyc_PP_cat(_tag_fat(_tmp4A0,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 6U: _LL9: _tmp486=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_LLA: {struct Cyc_Absyn_Datatypedecl*dd=_tmp486;
# 1822
s=({struct Cyc_PP_Doc*_tmp4A8[2U];({struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_datatypedecl2doc(dd);_tmp4A8[0]=_tmp83B;}),({struct Cyc_PP_Doc*_tmp83A=Cyc_PP_text(({const char*_tmp4A9=";";_tag_fat(_tmp4A9,sizeof(char),2U);}));_tmp4A8[1]=_tmp83A;});Cyc_PP_cat(_tag_fat(_tmp4A8,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 2U: _LLB: _tmp484=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_tmp485=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp477)->f3;_LLC: {struct Cyc_Absyn_Pat*p=_tmp484;struct Cyc_Absyn_Exp*e=_tmp485;
# 1825
s=({struct Cyc_PP_Doc*_tmp4AA[5U];({struct Cyc_PP_Doc*_tmp840=Cyc_PP_text(({const char*_tmp4AB="let ";_tag_fat(_tmp4AB,sizeof(char),5U);}));_tmp4AA[0]=_tmp840;}),({
struct Cyc_PP_Doc*_tmp83F=Cyc_Absynpp_pat2doc(p);_tmp4AA[1]=_tmp83F;}),({
struct Cyc_PP_Doc*_tmp83E=Cyc_PP_text(({const char*_tmp4AC=" = ";_tag_fat(_tmp4AC,sizeof(char),4U);}));_tmp4AA[2]=_tmp83E;}),({
struct Cyc_PP_Doc*_tmp83D=Cyc_Absynpp_exp2doc(e);_tmp4AA[3]=_tmp83D;}),({
struct Cyc_PP_Doc*_tmp83C=Cyc_PP_text(({const char*_tmp4AD=";";_tag_fat(_tmp4AD,sizeof(char),2U);}));_tmp4AA[4]=_tmp83C;});Cyc_PP_cat(_tag_fat(_tmp4AA,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 3U: _LLD: _tmp483=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_LLE: {struct Cyc_List_List*vds=_tmp483;
# 1832
s=({struct Cyc_PP_Doc*_tmp4AE[3U];({struct Cyc_PP_Doc*_tmp843=Cyc_PP_text(({const char*_tmp4AF="let ";_tag_fat(_tmp4AF,sizeof(char),5U);}));_tmp4AE[0]=_tmp843;}),({struct Cyc_PP_Doc*_tmp842=Cyc_Absynpp_ids2doc(vds);_tmp4AE[1]=_tmp842;}),({struct Cyc_PP_Doc*_tmp841=Cyc_PP_text(({const char*_tmp4B0=";";_tag_fat(_tmp4B0,sizeof(char),2U);}));_tmp4AE[2]=_tmp841;});Cyc_PP_cat(_tag_fat(_tmp4AE,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 7U: _LLF: _tmp482=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp482;
# 1835
s=({struct Cyc_PP_Doc*_tmp4B1[2U];({struct Cyc_PP_Doc*_tmp845=Cyc_Absynpp_enumdecl2doc(ed);_tmp4B1[0]=_tmp845;}),({struct Cyc_PP_Doc*_tmp844=Cyc_PP_text(({const char*_tmp4B2=";";_tag_fat(_tmp4B2,sizeof(char),2U);}));_tmp4B1[1]=_tmp844;});Cyc_PP_cat(_tag_fat(_tmp4B1,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 8U: _LL11: _tmp481=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp481;
# 1838
void*t;
if(td->defn != 0)
t=(void*)_check_null(td->defn);else{
# 1842
t=Cyc_Absyn_new_evar(td->kind,0);}
s=({struct Cyc_PP_Doc*_tmp4B3[4U];({struct Cyc_PP_Doc*_tmp84E=Cyc_PP_text(({const char*_tmp4B4="typedef ";_tag_fat(_tmp4B4,sizeof(char),9U);}));_tmp4B3[0]=_tmp84E;}),({
struct Cyc_PP_Doc*_tmp84D=({struct Cyc_Absyn_Tqual _tmp84C=td->tq;void*_tmp84B=t;Cyc_Absynpp_tqtd2doc(_tmp84C,_tmp84B,({struct Cyc_Core_Opt*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));({
# 1846
struct Cyc_PP_Doc*_tmp84A=({struct Cyc_PP_Doc*_tmp4B5[2U];({struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_typedef_name2bolddoc(td->name);_tmp4B5[0]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_Absynpp_tvars2doc(td->tvs);_tmp4B5[1]=_tmp848;});Cyc_PP_cat(_tag_fat(_tmp4B5,sizeof(struct Cyc_PP_Doc*),2U));});
# 1846
_tmp4B6->v=_tmp84A;});_tmp4B6;}));});
# 1844
_tmp4B3[1]=_tmp84D;}),({
# 1849
struct Cyc_PP_Doc*_tmp847=Cyc_Absynpp_atts2doc(td->atts);_tmp4B3[2]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp846=Cyc_PP_text(({const char*_tmp4B7=";";_tag_fat(_tmp4B7,sizeof(char),2U);}));_tmp4B3[3]=_tmp846;});Cyc_PP_cat(_tag_fat(_tmp4B3,sizeof(struct Cyc_PP_Doc*),4U));});
# 1852
goto _LL0;}case 9U: _LL13: _tmp47F=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_tmp480=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp477)->f2;_LL14: {struct _fat_ptr*v=_tmp47F;struct Cyc_List_List*tdl=_tmp480;
# 1854
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_tmp4B8[8U];({struct Cyc_PP_Doc*_tmp857=Cyc_PP_text(({const char*_tmp4B9="namespace ";_tag_fat(_tmp4B9,sizeof(char),11U);}));_tmp4B8[0]=_tmp857;}),({
struct Cyc_PP_Doc*_tmp856=Cyc_PP_textptr(v);_tmp4B8[1]=_tmp856;}),({
struct Cyc_PP_Doc*_tmp855=Cyc_PP_blank_doc();_tmp4B8[2]=_tmp855;}),({struct Cyc_PP_Doc*_tmp854=Cyc_Absynpp_lb();_tmp4B8[3]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp853=Cyc_PP_line_doc();_tmp4B8[4]=_tmp853;}),({
struct Cyc_PP_Doc*_tmp852=({struct _fat_ptr _tmp851=({const char*_tmp4BA="";_tag_fat(_tmp4BA,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp851,tdl);});_tmp4B8[5]=_tmp852;}),({
struct Cyc_PP_Doc*_tmp850=Cyc_PP_line_doc();_tmp4B8[6]=_tmp850;}),({
struct Cyc_PP_Doc*_tmp84F=Cyc_Absynpp_rb();_tmp4B8[7]=_tmp84F;});Cyc_PP_cat(_tag_fat(_tmp4B8,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10U: _LL15: _tmp47D=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_tmp47E=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp477)->f2;_LL16: {struct _tuple0*q=_tmp47D;struct Cyc_List_List*tdl=_tmp47E;
# 1865
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp4BB[8U];({struct Cyc_PP_Doc*_tmp860=Cyc_PP_text(({const char*_tmp4BC="using ";_tag_fat(_tmp4BC,sizeof(char),7U);}));_tmp4BB[0]=_tmp860;}),({
struct Cyc_PP_Doc*_tmp85F=Cyc_Absynpp_qvar2doc(q);_tmp4BB[1]=_tmp85F;}),({
struct Cyc_PP_Doc*_tmp85E=Cyc_PP_blank_doc();_tmp4BB[2]=_tmp85E;}),({struct Cyc_PP_Doc*_tmp85D=Cyc_Absynpp_lb();_tmp4BB[3]=_tmp85D;}),({
struct Cyc_PP_Doc*_tmp85C=Cyc_PP_line_doc();_tmp4BB[4]=_tmp85C;}),({
struct Cyc_PP_Doc*_tmp85B=({struct _fat_ptr _tmp85A=({const char*_tmp4BD="";_tag_fat(_tmp4BD,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp85A,tdl);});_tmp4BB[5]=_tmp85B;}),({
struct Cyc_PP_Doc*_tmp859=Cyc_PP_line_doc();_tmp4BB[6]=_tmp859;}),({
struct Cyc_PP_Doc*_tmp858=Cyc_Absynpp_rb();_tmp4BB[7]=_tmp858;});Cyc_PP_cat(_tag_fat(_tmp4BB,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1874
s=({struct Cyc_PP_Doc*_tmp4BE[11U];({struct Cyc_PP_Doc*_tmp86C=Cyc_PP_text(({const char*_tmp4BF="/* using ";_tag_fat(_tmp4BF,sizeof(char),10U);}));_tmp4BE[0]=_tmp86C;}),({
struct Cyc_PP_Doc*_tmp86B=Cyc_Absynpp_qvar2doc(q);_tmp4BE[1]=_tmp86B;}),({
struct Cyc_PP_Doc*_tmp86A=Cyc_PP_blank_doc();_tmp4BE[2]=_tmp86A;}),({
struct Cyc_PP_Doc*_tmp869=Cyc_Absynpp_lb();_tmp4BE[3]=_tmp869;}),({
struct Cyc_PP_Doc*_tmp868=Cyc_PP_text(({const char*_tmp4C0=" */";_tag_fat(_tmp4C0,sizeof(char),4U);}));_tmp4BE[4]=_tmp868;}),({
struct Cyc_PP_Doc*_tmp867=Cyc_PP_line_doc();_tmp4BE[5]=_tmp867;}),({
struct Cyc_PP_Doc*_tmp866=({struct _fat_ptr _tmp865=({const char*_tmp4C1="";_tag_fat(_tmp4C1,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp865,tdl);});_tmp4BE[6]=_tmp866;}),({
struct Cyc_PP_Doc*_tmp864=Cyc_PP_line_doc();_tmp4BE[7]=_tmp864;}),({
struct Cyc_PP_Doc*_tmp863=Cyc_PP_text(({const char*_tmp4C2="/* ";_tag_fat(_tmp4C2,sizeof(char),4U);}));_tmp4BE[8]=_tmp863;}),({
struct Cyc_PP_Doc*_tmp862=Cyc_Absynpp_rb();_tmp4BE[9]=_tmp862;}),({
struct Cyc_PP_Doc*_tmp861=Cyc_PP_text(({const char*_tmp4C3=" */";_tag_fat(_tmp4C3,sizeof(char),4U);}));_tmp4BE[10]=_tmp861;});Cyc_PP_cat(_tag_fat(_tmp4BE,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;}case 11U: _LL17: _tmp47C=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_LL18: {struct Cyc_List_List*tdl=_tmp47C;
# 1887
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp4C4[6U];({struct Cyc_PP_Doc*_tmp873=Cyc_PP_text(({const char*_tmp4C5="extern \"C\" ";_tag_fat(_tmp4C5,sizeof(char),12U);}));_tmp4C4[0]=_tmp873;}),({
struct Cyc_PP_Doc*_tmp872=Cyc_Absynpp_lb();_tmp4C4[1]=_tmp872;}),({
struct Cyc_PP_Doc*_tmp871=Cyc_PP_line_doc();_tmp4C4[2]=_tmp871;}),({
struct Cyc_PP_Doc*_tmp870=({struct _fat_ptr _tmp86F=({const char*_tmp4C6="";_tag_fat(_tmp4C6,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp86F,tdl);});_tmp4C4[3]=_tmp870;}),({
struct Cyc_PP_Doc*_tmp86E=Cyc_PP_line_doc();_tmp4C4[4]=_tmp86E;}),({
struct Cyc_PP_Doc*_tmp86D=Cyc_Absynpp_rb();_tmp4C4[5]=_tmp86D;});Cyc_PP_cat(_tag_fat(_tmp4C4,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1895
s=({struct Cyc_PP_Doc*_tmp4C7[9U];({struct Cyc_PP_Doc*_tmp87D=Cyc_PP_text(({const char*_tmp4C8="/* extern \"C\" ";_tag_fat(_tmp4C8,sizeof(char),15U);}));_tmp4C7[0]=_tmp87D;}),({
struct Cyc_PP_Doc*_tmp87C=Cyc_Absynpp_lb();_tmp4C7[1]=_tmp87C;}),({
struct Cyc_PP_Doc*_tmp87B=Cyc_PP_text(({const char*_tmp4C9=" */";_tag_fat(_tmp4C9,sizeof(char),4U);}));_tmp4C7[2]=_tmp87B;}),({
struct Cyc_PP_Doc*_tmp87A=Cyc_PP_line_doc();_tmp4C7[3]=_tmp87A;}),({
struct Cyc_PP_Doc*_tmp879=({struct _fat_ptr _tmp878=({const char*_tmp4CA="";_tag_fat(_tmp4CA,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp878,tdl);});_tmp4C7[4]=_tmp879;}),({
struct Cyc_PP_Doc*_tmp877=Cyc_PP_line_doc();_tmp4C7[5]=_tmp877;}),({
struct Cyc_PP_Doc*_tmp876=Cyc_PP_text(({const char*_tmp4CB="/* ";_tag_fat(_tmp4CB,sizeof(char),4U);}));_tmp4C7[6]=_tmp876;}),({
struct Cyc_PP_Doc*_tmp875=Cyc_Absynpp_rb();_tmp4C7[7]=_tmp875;}),({
struct Cyc_PP_Doc*_tmp874=Cyc_PP_text(({const char*_tmp4CC=" */";_tag_fat(_tmp4CC,sizeof(char),4U);}));_tmp4C7[8]=_tmp874;});Cyc_PP_cat(_tag_fat(_tmp4C7,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 12U: _LL19: _tmp478=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp477)->f1;_tmp479=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp477)->f2;_tmp47A=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp477)->f3;_tmp47B=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp477)->f4;_LL1A: {struct Cyc_List_List*tdl=_tmp478;struct Cyc_List_List*ovrs=_tmp479;struct Cyc_List_List*exs=_tmp47A;struct _tuple10*wc=_tmp47B;
# 1906
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp4CD[7U];({struct Cyc_PP_Doc*_tmp885=Cyc_Absynpp_rb();_tmp4CD[0]=_tmp885;}),({struct Cyc_PP_Doc*_tmp884=Cyc_PP_text(({const char*_tmp4CE=" export ";_tag_fat(_tmp4CE,sizeof(char),9U);}));_tmp4CD[1]=_tmp884;}),({struct Cyc_PP_Doc*_tmp883=Cyc_Absynpp_lb();_tmp4CD[2]=_tmp883;}),({
struct Cyc_PP_Doc*_tmp882=Cyc_PP_line_doc();_tmp4CD[3]=_tmp882;}),({struct Cyc_PP_Doc*_tmp881=({struct _fat_ptr _tmp880=({const char*_tmp4CF=",";_tag_fat(_tmp4CF,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple20*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp880,exs);});_tmp4CD[4]=_tmp881;}),({
struct Cyc_PP_Doc*_tmp87F=Cyc_PP_line_doc();_tmp4CD[5]=_tmp87F;}),({struct Cyc_PP_Doc*_tmp87E=Cyc_Absynpp_rb();_tmp4CD[6]=_tmp87E;});Cyc_PP_cat(_tag_fat(_tmp4CD,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1914
exs_doc=Cyc_Absynpp_rb();}
if(ovrs != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp4D0[7U];({struct Cyc_PP_Doc*_tmp88D=Cyc_Absynpp_rb();_tmp4D0[0]=_tmp88D;}),({struct Cyc_PP_Doc*_tmp88C=Cyc_PP_text(({const char*_tmp4D1=" cycdef ";_tag_fat(_tmp4D1,sizeof(char),9U);}));_tmp4D0[1]=_tmp88C;}),({struct Cyc_PP_Doc*_tmp88B=Cyc_Absynpp_lb();_tmp4D0[2]=_tmp88B;}),({
struct Cyc_PP_Doc*_tmp88A=Cyc_PP_line_doc();_tmp4D0[3]=_tmp88A;}),({struct Cyc_PP_Doc*_tmp889=({struct _fat_ptr _tmp888=({const char*_tmp4D2="";_tag_fat(_tmp4D2,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp888,ovrs);});_tmp4D0[4]=_tmp889;}),({
struct Cyc_PP_Doc*_tmp887=Cyc_PP_line_doc();_tmp4D0[5]=_tmp887;}),({struct Cyc_PP_Doc*_tmp886=Cyc_Absynpp_rb();_tmp4D0[6]=_tmp886;});Cyc_PP_cat(_tag_fat(_tmp4D0,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1920
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp4D3[6U];({struct Cyc_PP_Doc*_tmp893=Cyc_PP_text(({const char*_tmp4D4="extern \"C include\" ";_tag_fat(_tmp4D4,sizeof(char),20U);}));_tmp4D3[0]=_tmp893;}),({
struct Cyc_PP_Doc*_tmp892=Cyc_Absynpp_lb();_tmp4D3[1]=_tmp892;}),({
struct Cyc_PP_Doc*_tmp891=Cyc_PP_line_doc();_tmp4D3[2]=_tmp891;}),({
struct Cyc_PP_Doc*_tmp890=({struct _fat_ptr _tmp88F=({const char*_tmp4D5="";_tag_fat(_tmp4D5,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp88F,tdl);});_tmp4D3[3]=_tmp890;}),({
struct Cyc_PP_Doc*_tmp88E=Cyc_PP_line_doc();_tmp4D3[4]=_tmp88E;}),_tmp4D3[5]=exs_doc;Cyc_PP_cat(_tag_fat(_tmp4D3,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1928
s=({struct Cyc_PP_Doc*_tmp4D6[9U];({struct Cyc_PP_Doc*_tmp89D=Cyc_PP_text(({const char*_tmp4D7="/* extern \"C include\" ";_tag_fat(_tmp4D7,sizeof(char),23U);}));_tmp4D6[0]=_tmp89D;}),({
struct Cyc_PP_Doc*_tmp89C=Cyc_Absynpp_lb();_tmp4D6[1]=_tmp89C;}),({
struct Cyc_PP_Doc*_tmp89B=Cyc_PP_text(({const char*_tmp4D8=" */";_tag_fat(_tmp4D8,sizeof(char),4U);}));_tmp4D6[2]=_tmp89B;}),({
struct Cyc_PP_Doc*_tmp89A=Cyc_PP_line_doc();_tmp4D6[3]=_tmp89A;}),({
struct Cyc_PP_Doc*_tmp899=({struct _fat_ptr _tmp898=({const char*_tmp4D9="";_tag_fat(_tmp4D9,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp898,tdl);});_tmp4D6[4]=_tmp899;}),({
struct Cyc_PP_Doc*_tmp897=Cyc_PP_line_doc();_tmp4D6[5]=_tmp897;}),({
struct Cyc_PP_Doc*_tmp896=Cyc_PP_text(({const char*_tmp4DA="/* ";_tag_fat(_tmp4DA,sizeof(char),4U);}));_tmp4D6[6]=_tmp896;}),({
struct Cyc_PP_Doc*_tmp895=Cyc_Absynpp_rb();_tmp4D6[7]=_tmp895;}),({
struct Cyc_PP_Doc*_tmp894=Cyc_PP_text(({const char*_tmp4DB=" */";_tag_fat(_tmp4DB,sizeof(char),4U);}));_tmp4D6[8]=_tmp894;});Cyc_PP_cat(_tag_fat(_tmp4D6,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 13U: _LL1B: _LL1C:
# 1939
 s=({struct Cyc_PP_Doc*_tmp4DC[2U];({struct Cyc_PP_Doc*_tmp89F=Cyc_PP_text(({const char*_tmp4DD="__cyclone_port_on__;";_tag_fat(_tmp4DD,sizeof(char),21U);}));_tmp4DC[0]=_tmp89F;}),({struct Cyc_PP_Doc*_tmp89E=Cyc_Absynpp_lb();_tmp4DC[1]=_tmp89E;});Cyc_PP_cat(_tag_fat(_tmp4DC,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1942
 s=({struct Cyc_PP_Doc*_tmp4DE[2U];({struct Cyc_PP_Doc*_tmp8A1=Cyc_PP_text(({const char*_tmp4DF="__cyclone_port_off__;";_tag_fat(_tmp4DF,sizeof(char),22U);}));_tmp4DE[0]=_tmp8A1;}),({struct Cyc_PP_Doc*_tmp8A0=Cyc_Absynpp_lb();_tmp4DE[1]=_tmp8A0;});Cyc_PP_cat(_tag_fat(_tmp4DE,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1945
 s=({struct Cyc_PP_Doc*_tmp4E0[2U];({struct Cyc_PP_Doc*_tmp8A3=Cyc_PP_text(({const char*_tmp4E1="__tempest_on__;";_tag_fat(_tmp4E1,sizeof(char),16U);}));_tmp4E0[0]=_tmp8A3;}),({struct Cyc_PP_Doc*_tmp8A2=Cyc_Absynpp_lb();_tmp4E0[1]=_tmp8A2;});Cyc_PP_cat(_tag_fat(_tmp4E0,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1948
 s=({struct Cyc_PP_Doc*_tmp4E2[2U];({struct Cyc_PP_Doc*_tmp8A5=Cyc_PP_text(({const char*_tmp4E3="__tempest_off__;";_tag_fat(_tmp4E3,sizeof(char),17U);}));_tmp4E2[0]=_tmp8A5;}),({struct Cyc_PP_Doc*_tmp8A4=Cyc_Absynpp_lb();_tmp4E2[1]=_tmp8A4;});Cyc_PP_cat(_tag_fat(_tmp4E2,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 1952
return s;}
# 1955
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4E4=sc;switch(_tmp4E4){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4E5="static ";_tag_fat(_tmp4E5,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4E6="extern ";_tag_fat(_tmp4E6,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4E7="extern \"C\" ";_tag_fat(_tmp4E7,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4E8="abstract ";_tag_fat(_tmp4E8,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4E9="register ";_tag_fat(_tmp4E9,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;}}
# 1969
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4EA=Cyc_Tcutil_compress(t);void*_stmttmp22=_tmp4EA;void*_tmp4EB=_stmttmp22;struct Cyc_List_List*_tmp4EC;struct Cyc_Absyn_Tvar*_tmp4ED;switch(*((int*)_tmp4EB)){case 2U: _LL1: _tmp4ED=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4EB)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp4ED;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EB)->f1)->tag == 9U){_LL3: _tmp4EC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EB)->f2;_LL4: {struct Cyc_List_List*l=_tmp4EC;
return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 1981
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4EE=t;void*_tmp4F0;struct Cyc_Absyn_Typedefdecl*_tmp4EF;switch(*((int*)_tmp4EE)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EE)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp4EF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4EE)->f3;_tmp4F0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4EE)->f4;if(_tmp4F0 != 0){_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmp4EF;void*x=_tmp4F0;
# 1988
return Cyc_Absynpp_is_anon_aggrtype(x);}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1995
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1998
if(tms != 0 && tms->tl != 0){
struct _tuple18 _tmp4F1=({struct _tuple18 _tmp58C;_tmp58C.f1=(void*)tms->hd,_tmp58C.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp58C;});struct _tuple18 _stmttmp23=_tmp4F1;struct _tuple18 _tmp4F2=_stmttmp23;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4F2.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4F2.f2)->tag == 3U){_LL1: _LL2:
# 2001
 return({struct Cyc_List_List*_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4));_tmp4F4->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp8A7=({struct Cyc_List_List*_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3));_tmp4F3->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp8A6=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4F3->tl=_tmp8A6;});_tmp4F3;});_tmp4F4->tl=_tmp8A7;});_tmp4F4;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4F5=_region_malloc(r,sizeof(*_tmp4F5));_tmp4F5->hd=atts,_tmp4F5->tl=tms;_tmp4F5;});}_LL0:;}else{
# 2004
return({struct Cyc_List_List*_tmp4F6=_region_malloc(r,sizeof(*_tmp4F6));_tmp4F6->hd=atts,_tmp4F6->tl=tms;_tmp4F6;});}}
# 2007
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp4F7=({struct _fat_ptr _tmp8A8=({const char*_tmp4FD="`";_tag_fat(_tmp4FD,sizeof(char),2U);});Cyc_strconcat(_tmp8A8,(struct _fat_ptr)*t->name);});struct _fat_ptr s=_tmp4F7;
({struct _fat_ptr _tmp4F8=_fat_ptr_plus(s,sizeof(char),1);char _tmp4F9=*((char*)_check_fat_subscript(_tmp4F8,sizeof(char),0U));char _tmp4FA='t';if(_get_fat_size(_tmp4F8,sizeof(char))== 1U &&(_tmp4F9 == 0 && _tmp4FA != 0))_throw_arraybounds();*((char*)_tmp4F8.curr)=_tmp4FA;});
({struct _fat_ptr*_tmp8A9=({unsigned _tmp4FC=1;struct _fat_ptr*_tmp4FB=_cycalloc(_check_times(_tmp4FC,sizeof(struct _fat_ptr)));_tmp4FB[0]=(struct _fat_ptr)s;_tmp4FB;});t->name=_tmp8A9;});}}
# 2016
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2018
void*_tmp4FE=t;void*_tmp502;struct Cyc_Absyn_Typedefdecl*_tmp501;struct Cyc_List_List*_tmp500;struct _tuple0*_tmp4FF;int _tmp505;void*_tmp504;struct Cyc_Core_Opt*_tmp503;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_List_List*_tmp50E;struct Cyc_List_List*_tmp50D;struct Cyc_Absyn_VarargInfo*_tmp50C;int _tmp50B;struct Cyc_List_List*_tmp50A;void*_tmp509;struct Cyc_Absyn_Tqual _tmp508;void*_tmp507;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_PtrAtts _tmp513;struct Cyc_Absyn_Tqual _tmp512;void*_tmp511;unsigned _tmp518;void*_tmp517;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Tqual _tmp515;void*_tmp514;switch(*((int*)_tmp4FE)){case 4U: _LL1: _tmp514=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FE)->f1).elt_type;_tmp515=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FE)->f1).tq;_tmp516=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FE)->f1).num_elts;_tmp517=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FE)->f1).zero_term;_tmp518=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FE)->f1).zt_loc;_LL2: {void*t2=_tmp514;struct Cyc_Absyn_Tqual tq2=_tmp515;struct Cyc_Absyn_Exp*e=_tmp516;void*zeroterm=_tmp517;unsigned ztl=_tmp518;
# 2021
struct _tuple13 _tmp519=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp24=_tmp519;struct _tuple13 _tmp51A=_stmttmp24;struct Cyc_List_List*_tmp51D;void*_tmp51C;struct Cyc_Absyn_Tqual _tmp51B;_LLE: _tmp51B=_tmp51A.f1;_tmp51C=_tmp51A.f2;_tmp51D=_tmp51A.f3;_LLF: {struct Cyc_Absyn_Tqual tq3=_tmp51B;void*t3=_tmp51C;struct Cyc_List_List*tml3=_tmp51D;
void*tm;
if(e == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp51E=_region_malloc(r,sizeof(*_tmp51E));_tmp51E->tag=0U,_tmp51E->f1=zeroterm,_tmp51E->f2=ztl;_tmp51E;});else{
# 2026
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp51F=_region_malloc(r,sizeof(*_tmp51F));_tmp51F->tag=1U,_tmp51F->f1=e,_tmp51F->f2=zeroterm,_tmp51F->f3=ztl;_tmp51F;});}
return({struct _tuple13 _tmp58D;_tmp58D.f1=tq3,_tmp58D.f2=t3,({struct Cyc_List_List*_tmp8AA=({struct Cyc_List_List*_tmp520=_region_malloc(r,sizeof(*_tmp520));_tmp520->hd=tm,_tmp520->tl=tml3;_tmp520;});_tmp58D.f3=_tmp8AA;});_tmp58D;});}}case 3U: _LL3: _tmp511=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FE)->f1).elt_type;_tmp512=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FE)->f1).elt_tq;_tmp513=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FE)->f1).ptr_atts;_LL4: {void*t2=_tmp511;struct Cyc_Absyn_Tqual tq2=_tmp512;struct Cyc_Absyn_PtrAtts ptratts=_tmp513;
# 2030
struct _tuple13 _tmp521=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp25=_tmp521;struct _tuple13 _tmp522=_stmttmp25;struct Cyc_List_List*_tmp525;void*_tmp524;struct Cyc_Absyn_Tqual _tmp523;_LL11: _tmp523=_tmp522.f1;_tmp524=_tmp522.f2;_tmp525=_tmp522.f3;_LL12: {struct Cyc_Absyn_Tqual tq3=_tmp523;void*t3=_tmp524;struct Cyc_List_List*tml3=_tmp525;
tml3=({struct Cyc_List_List*_tmp527=_region_malloc(r,sizeof(*_tmp527));({void*_tmp8AB=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp526=_region_malloc(r,sizeof(*_tmp526));_tmp526->tag=2U,_tmp526->f1=ptratts,_tmp526->f2=tq;_tmp526;});_tmp527->hd=_tmp8AB;}),_tmp527->tl=tml3;_tmp527;});
return({struct _tuple13 _tmp58E;_tmp58E.f1=tq3,_tmp58E.f2=t3,_tmp58E.f3=tml3;_tmp58E;});}}case 5U: _LL5: _tmp506=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).tvars;_tmp507=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).effect;_tmp508=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).ret_tqual;_tmp509=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).ret_type;_tmp50A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).args;_tmp50B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).c_varargs;_tmp50C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).cyc_varargs;_tmp50D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).rgn_po;_tmp50E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).attributes;_tmp50F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).requires_clause;_tmp510=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE)->f1).ensures_clause;_LL6: {struct Cyc_List_List*typvars=_tmp506;void*effopt=_tmp507;struct Cyc_Absyn_Tqual t2qual=_tmp508;void*t2=_tmp509;struct Cyc_List_List*args=_tmp50A;int c_varargs=_tmp50B;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp50C;struct Cyc_List_List*rgn_po=_tmp50D;struct Cyc_List_List*fn_atts=_tmp50E;struct Cyc_Absyn_Exp*req=_tmp50F;struct Cyc_Absyn_Exp*ens=_tmp510;
# 2036
if(!Cyc_Absynpp_print_all_tvars){
# 2040
if(effopt == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2045
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2048
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2053
struct _tuple13 _tmp528=Cyc_Absynpp_to_tms(r,t2qual,t2);struct _tuple13 _stmttmp26=_tmp528;struct _tuple13 _tmp529=_stmttmp26;struct Cyc_List_List*_tmp52C;void*_tmp52B;struct Cyc_Absyn_Tqual _tmp52A;_LL14: _tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;_tmp52C=_tmp529.f3;_LL15: {struct Cyc_Absyn_Tqual tq3=_tmp52A;void*t3=_tmp52B;struct Cyc_List_List*tml3=_tmp52C;
struct Cyc_List_List*tms=tml3;
# 2064 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp52D=Cyc_Cyclone_c_compiler;if(_tmp52D == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2066
 if(fn_atts != 0)
# 2069
tms=({struct _RegionHandle*_tmp8AD=r;void*_tmp8AC=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp52E=_region_malloc(r,sizeof(*_tmp52E));_tmp52E->tag=5U,_tmp52E->f1=0U,_tmp52E->f2=fn_atts;_tmp52E;});Cyc_Absynpp_bubble_attributes(_tmp8AD,_tmp8AC,tms);});
# 2071
tms=({struct Cyc_List_List*_tmp531=_region_malloc(r,sizeof(*_tmp531));({void*_tmp8AF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp530=_region_malloc(r,sizeof(*_tmp530));
_tmp530->tag=3U,({void*_tmp8AE=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp52F=_region_malloc(r,sizeof(*_tmp52F));_tmp52F->tag=1U,_tmp52F->f1=args,_tmp52F->f2=c_varargs,_tmp52F->f3=cyc_varargs,_tmp52F->f4=effopt,_tmp52F->f5=rgn_po,_tmp52F->f6=req,_tmp52F->f7=ens;_tmp52F;});_tmp530->f1=_tmp8AE;});_tmp530;});
# 2071
_tmp531->hd=_tmp8AF;}),_tmp531->tl=tms;_tmp531;});
# 2075
goto _LL16;}else{_LL19: _LL1A:
# 2077
 tms=({struct Cyc_List_List*_tmp534=_region_malloc(r,sizeof(*_tmp534));({void*_tmp8B1=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp533=_region_malloc(r,sizeof(*_tmp533));
_tmp533->tag=3U,({void*_tmp8B0=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp532=_region_malloc(r,sizeof(*_tmp532));_tmp532->tag=1U,_tmp532->f1=args,_tmp532->f2=c_varargs,_tmp532->f3=cyc_varargs,_tmp532->f4=effopt,_tmp532->f5=rgn_po,_tmp532->f6=req,_tmp532->f7=ens;_tmp532;});_tmp533->f1=_tmp8B0;});_tmp533;});
# 2077
_tmp534->hd=_tmp8B1;}),_tmp534->tl=tms;_tmp534;});
# 2081
for(0;fn_atts != 0;fn_atts=fn_atts->tl){
void*_tmp535=(void*)fn_atts->hd;void*_stmttmp27=_tmp535;void*_tmp536=_stmttmp27;switch(*((int*)_tmp536)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2086
 tms=({struct Cyc_List_List*_tmp539=_region_malloc(r,sizeof(*_tmp539));({void*_tmp8B3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp538=_region_malloc(r,sizeof(*_tmp538));_tmp538->tag=5U,_tmp538->f1=0U,({struct Cyc_List_List*_tmp8B2=({struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537->hd=(void*)fn_atts->hd,_tmp537->tl=0;_tmp537;});_tmp538->f2=_tmp8B2;});_tmp538;});_tmp539->hd=_tmp8B3;}),_tmp539->tl=tms;_tmp539;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2090
goto _LL16;}_LL16:;}
# 2093
AfterAtts:
 if(typvars != 0)
tms=({struct Cyc_List_List*_tmp53B=_region_malloc(r,sizeof(*_tmp53B));({void*_tmp8B4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp53A=_region_malloc(r,sizeof(*_tmp53A));_tmp53A->tag=4U,_tmp53A->f1=typvars,_tmp53A->f2=0U,_tmp53A->f3=1;_tmp53A;});_tmp53B->hd=_tmp8B4;}),_tmp53B->tl=tms;_tmp53B;});
return({struct _tuple13 _tmp58F;_tmp58F.f1=tq3,_tmp58F.f2=t3,_tmp58F.f3=tms;_tmp58F;});}}}case 1U: _LL7: _tmp503=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FE)->f1;_tmp504=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FE)->f2;_tmp505=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FE)->f3;_LL8: {struct Cyc_Core_Opt*k=_tmp503;void*topt=_tmp504;int i=_tmp505;
# 2099
if(topt == 0)
return({struct _tuple13 _tmp590;_tmp590.f1=tq,_tmp590.f2=t,_tmp590.f3=0;_tmp590;});else{
# 2102
return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8U: _LL9: _tmp4FF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FE)->f1;_tmp500=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FE)->f2;_tmp501=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FE)->f3;_tmp502=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4FE)->f4;_LLA: {struct _tuple0*n=_tmp4FF;struct Cyc_List_List*ts=_tmp500;struct Cyc_Absyn_Typedefdecl*td=_tmp501;void*topt=_tmp502;
# 2108
if(topt == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple13 _tmp591;_tmp591.f1=tq,_tmp591.f2=t,_tmp591.f3=0;_tmp591;});else{
# 2111
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}}default: _LLB: _LLC:
# 2116
 return({struct _tuple13 _tmp592;_tmp592.f1=tq,_tmp592.f2=t,_tmp592.f3=0;_tmp592;});}_LL0:;}
# 2120
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2122
void*_tmp53C=t;void*_tmp53D;void*_tmp53E;switch(*((int*)_tmp53C)){case 1U: _LL1: _tmp53E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp53C)->f2;if(_tmp53E != 0){_LL2: {void*def=_tmp53E;
return Cyc_Absynpp_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp53D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->f1).elt_type;_LL4: {void*elt_typ=_tmp53D;
# 2125
L: {
void*_tmp53F=elt_typ;void*_tmp540;void*_tmp541;switch(*((int*)_tmp53F)){case 1U: _LL8: _tmp541=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp53F)->f2;if(_tmp541 != 0){_LL9: {void*t=_tmp541;
elt_typ=t;goto L;}}else{goto _LLE;}case 8U: _LLA: _tmp540=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53F)->f4;if(_tmp540 != 0){_LLB: {void*t=_tmp540;
elt_typ=t;goto L;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53F)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53F)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 2132
 return 0;}_LL0:;}
# 2136
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp542=_new_region("temp");struct _RegionHandle*temp=& _tmp542;_push_region(temp);
{struct _tuple13 _tmp543=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple13 _stmttmp28=_tmp543;struct _tuple13 _tmp544=_stmttmp28;struct Cyc_List_List*_tmp547;void*_tmp546;struct Cyc_Absyn_Tqual _tmp545;_LL1: _tmp545=_tmp544.f1;_tmp546=_tmp544.f2;_tmp547=_tmp544.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp545;void*t=_tmp546;struct Cyc_List_List*tms=_tmp547;
tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);
if(tms == 0 && dopt == 0){
struct Cyc_PP_Doc*_tmp549=({struct Cyc_PP_Doc*_tmp548[2U];({struct Cyc_PP_Doc*_tmp8B6=Cyc_Absynpp_tqual2doc(tq);_tmp548[0]=_tmp8B6;}),({struct Cyc_PP_Doc*_tmp8B5=Cyc_Absynpp_ntyp2doc(t);_tmp548[1]=_tmp8B5;});Cyc_PP_cat(_tag_fat(_tmp548,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp549;}else{
# 2143
struct Cyc_PP_Doc*_tmp54C=({struct Cyc_PP_Doc*_tmp54A[4U];({
struct Cyc_PP_Doc*_tmp8BC=Cyc_Absynpp_tqual2doc(tq);_tmp54A[0]=_tmp8BC;}),({
struct Cyc_PP_Doc*_tmp8BB=Cyc_Absynpp_ntyp2doc(t);_tmp54A[1]=_tmp8BB;}),({
struct Cyc_PP_Doc*_tmp8BA=Cyc_PP_text(({const char*_tmp54B=" ";_tag_fat(_tmp54B,sizeof(char),2U);}));_tmp54A[2]=_tmp8BA;}),({
struct Cyc_PP_Doc*_tmp8B9=({int _tmp8B8=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp8B7=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp8B8,_tmp8B7,tms);});_tmp54A[3]=_tmp8B9;});Cyc_PP_cat(_tag_fat(_tmp54A,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp54C;}}}
# 2138
;_pop_region();}
# 2151
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp54D=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp54D;
if((unsigned)req)
requires_doc=({struct Cyc_PP_Doc*_tmp54E[2U];({struct Cyc_PP_Doc*_tmp8BE=Cyc_PP_text(({const char*_tmp54F="@requires ";_tag_fat(_tmp54F,sizeof(char),11U);}));_tmp54E[0]=_tmp8BE;}),({struct Cyc_PP_Doc*_tmp8BD=Cyc_Absynpp_exp2doc(req);_tmp54E[1]=_tmp8BD;});Cyc_PP_cat(_tag_fat(_tmp54E,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2157
requires_doc=Cyc_PP_nil_doc();}{
# 2159
enum Cyc_Cyclone_C_Compilers _tmp550=Cyc_Cyclone_c_compiler;if(_tmp550 == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2162
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp551[5U];({struct Cyc_PP_Doc*_tmp8C6=({struct Cyc_Absyn_Tqual _tmp8C5=f->tq;void*_tmp8C4=f->type;Cyc_Absynpp_tqtd2doc(_tmp8C5,_tmp8C4,({struct Cyc_Core_Opt*_tmp552=_cycalloc(sizeof(*_tmp552));({struct Cyc_PP_Doc*_tmp8C3=Cyc_PP_textptr(f->name);_tmp552->v=_tmp8C3;});_tmp552;}));});_tmp551[0]=_tmp8C6;}),({
struct Cyc_PP_Doc*_tmp8C2=Cyc_PP_text(({const char*_tmp553=":";_tag_fat(_tmp553,sizeof(char),2U);}));_tmp551[1]=_tmp8C2;}),({struct Cyc_PP_Doc*_tmp8C1=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp551[2]=_tmp8C1;}),({
struct Cyc_PP_Doc*_tmp8C0=Cyc_Absynpp_atts2doc(f->attributes);_tmp551[3]=_tmp8C0;}),({struct Cyc_PP_Doc*_tmp8BF=Cyc_PP_text(({const char*_tmp554=";";_tag_fat(_tmp554,sizeof(char),2U);}));_tmp551[4]=_tmp8BF;});Cyc_PP_cat(_tag_fat(_tmp551,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2167
return({struct Cyc_PP_Doc*_tmp555[4U];({struct Cyc_PP_Doc*_tmp8CC=({struct Cyc_Absyn_Tqual _tmp8CB=f->tq;void*_tmp8CA=f->type;Cyc_Absynpp_tqtd2doc(_tmp8CB,_tmp8CA,({struct Cyc_Core_Opt*_tmp556=_cycalloc(sizeof(*_tmp556));({struct Cyc_PP_Doc*_tmp8C9=Cyc_PP_textptr(f->name);_tmp556->v=_tmp8C9;});_tmp556;}));});_tmp555[0]=_tmp8CC;}),({
struct Cyc_PP_Doc*_tmp8C8=Cyc_Absynpp_atts2doc(f->attributes);_tmp555[1]=_tmp8C8;}),_tmp555[2]=requires_doc,({struct Cyc_PP_Doc*_tmp8C7=Cyc_PP_text(({const char*_tmp557=";";_tag_fat(_tmp557,sizeof(char),2U);}));_tmp555[3]=_tmp8C7;});Cyc_PP_cat(_tag_fat(_tmp555,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2170
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp558[5U];({struct Cyc_PP_Doc*_tmp8D4=Cyc_Absynpp_atts2doc(f->attributes);_tmp558[0]=_tmp8D4;}),({
struct Cyc_PP_Doc*_tmp8D3=({struct Cyc_Absyn_Tqual _tmp8D2=f->tq;void*_tmp8D1=f->type;Cyc_Absynpp_tqtd2doc(_tmp8D2,_tmp8D1,({struct Cyc_Core_Opt*_tmp559=_cycalloc(sizeof(*_tmp559));({struct Cyc_PP_Doc*_tmp8D0=Cyc_PP_textptr(f->name);_tmp559->v=_tmp8D0;});_tmp559;}));});_tmp558[1]=_tmp8D3;}),({
struct Cyc_PP_Doc*_tmp8CF=Cyc_PP_text(({const char*_tmp55A=":";_tag_fat(_tmp55A,sizeof(char),2U);}));_tmp558[2]=_tmp8CF;}),({struct Cyc_PP_Doc*_tmp8CE=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp558[3]=_tmp8CE;}),({struct Cyc_PP_Doc*_tmp8CD=Cyc_PP_text(({const char*_tmp55B=";";_tag_fat(_tmp55B,sizeof(char),2U);}));_tmp558[4]=_tmp8CD;});Cyc_PP_cat(_tag_fat(_tmp558,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2175
return({struct Cyc_PP_Doc*_tmp55C[4U];({struct Cyc_PP_Doc*_tmp8DA=Cyc_Absynpp_atts2doc(f->attributes);_tmp55C[0]=_tmp8DA;}),({
struct Cyc_PP_Doc*_tmp8D9=({struct Cyc_Absyn_Tqual _tmp8D8=f->tq;void*_tmp8D7=f->type;Cyc_Absynpp_tqtd2doc(_tmp8D8,_tmp8D7,({struct Cyc_Core_Opt*_tmp55D=_cycalloc(sizeof(*_tmp55D));({struct Cyc_PP_Doc*_tmp8D6=Cyc_PP_textptr(f->name);_tmp55D->v=_tmp8D6;});_tmp55D;}));});_tmp55C[1]=_tmp8D9;}),_tmp55C[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp8D5=Cyc_PP_text(({const char*_tmp55E=";";_tag_fat(_tmp55E,sizeof(char),2U);}));_tmp55C[3]=_tmp8D5;});Cyc_PP_cat(_tag_fat(_tmp55C,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;}}
# 2182
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp8DB=({const char*_tmp55F="";_tag_fat(_tmp55F,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp8DB,fields);});}
# 2186
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp560[3U];({struct Cyc_PP_Doc*_tmp8DF=Cyc_Absynpp_scope2doc(f->sc);_tmp560[0]=_tmp8DF;}),({struct Cyc_PP_Doc*_tmp8DE=Cyc_Absynpp_typedef_name2doc(f->name);_tmp560[1]=_tmp8DE;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp8DD=Cyc_PP_nil_doc();_tmp560[2]=_tmp8DD;}):({struct Cyc_PP_Doc*_tmp8DC=Cyc_Absynpp_args2doc(f->typs);_tmp560[2]=_tmp8DC;});Cyc_PP_cat(_tag_fat(_tmp560,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2191
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp8E0=({const char*_tmp561=",";_tag_fat(_tmp561,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp8E0,fields);});}
# 2200
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp8E1=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp8E1,72,f);});
({void*_tmp562=0U;({struct Cyc___cycFILE*_tmp8E3=f;struct _fat_ptr _tmp8E2=({const char*_tmp563="\n";_tag_fat(_tmp563,sizeof(char),2U);});Cyc_fprintf(_tmp8E3,_tmp8E2,_tag_fat(_tmp562,sizeof(void*),0U));});});}}
# 2207
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _tmp8E4=({const char*_tmp564="";_tag_fat(_tmp564,sizeof(char),1U);});Cyc_PP_seql(_tmp8E4,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2210
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
# 2224
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
# 2229
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*t){
if(t == 0)
return(struct _fat_ptr)({const char*_tmp565="NULL";_tag_fat(_tmp565,sizeof(char),5U);});{
void*_tmp566=t;struct Cyc_Absyn_Typedefdecl*_tmp567;switch(*((int*)_tmp566)){case 0U: _LL1: _LL2:
 return(struct _fat_ptr)({const char*_tmp568="AppType";_tag_fat(_tmp568,sizeof(char),8U);});case 1U: _LL3: _LL4:
 return(struct _fat_ptr)({const char*_tmp569="Evar";_tag_fat(_tmp569,sizeof(char),5U);});case 2U: _LL5: _LL6:
 return(struct _fat_ptr)({const char*_tmp56A="Vartype";_tag_fat(_tmp56A,sizeof(char),8U);});case 3U: _LL7: _LL8:
 return(struct _fat_ptr)({const char*_tmp56B="Pointertype";_tag_fat(_tmp56B,sizeof(char),12U);});case 4U: _LL9: _LLA:
 return(struct _fat_ptr)({const char*_tmp56C="ArrayType";_tag_fat(_tmp56C,sizeof(char),10U);});case 5U: _LLB: _LLC:
 return(struct _fat_ptr)({const char*_tmp56D="FnType";_tag_fat(_tmp56D,sizeof(char),7U);});case 6U: _LLD: _LLE:
 return(struct _fat_ptr)({const char*_tmp56E="TupleType";_tag_fat(_tmp56E,sizeof(char),10U);});case 7U: _LLF: _LL10:
 return(struct _fat_ptr)({const char*_tmp56F="AnonAggrType";_tag_fat(_tmp56F,sizeof(char),13U);});case 8U: _LL11: _tmp567=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp566)->f3;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp567;
# 2242
return(struct _fat_ptr)({const char*_tmp570="Typedeftype";_tag_fat(_tmp570,sizeof(char),12U);});}case 9U: _LL13: _LL14:
 return(struct _fat_ptr)({const char*_tmp571="Valueoftype";_tag_fat(_tmp571,sizeof(char),12U);});case 10U: _LL15: _LL16:
 return(struct _fat_ptr)({const char*_tmp572="Typedecltype";_tag_fat(_tmp572,sizeof(char),13U);});default: _LL17: _LL18:
 return(struct _fat_ptr)({const char*_tmp573="Typeoftype";_tag_fat(_tmp573,sizeof(char),11U);});}_LL0:;}}

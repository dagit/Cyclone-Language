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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 691 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 874
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 883
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 888
void*Cyc_Absyn_compress_kb(void*);
# 894
int Cyc_Absyn_type2bool(int def,void*);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1127
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1137
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1145
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
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
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple13{int f1;struct Cyc_List_List*f2;};
# 93 "absynpp.h"
struct _tuple13 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 39 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple14*dp);
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
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de);
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
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp55F=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp55F,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v,_tmp1->tl=0;_tmp1;}));});}
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
struct _fat_ptr _tmpD=Cyc_Core_new_string(2U);struct _fat_ptr t=_tmpD;
({struct _fat_ptr _tmpE=_fat_ptr_plus(t,sizeof(char),0);char _tmpF=*((char*)_check_fat_subscript(_tmpE,sizeof(char),0U));char _tmp10=c;if(_get_fat_size(_tmpE,sizeof(char))== 1U &&(_tmpF == 0 && _tmp10 != 0))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _fat_ptr)t;}else{
# 278
struct _fat_ptr _tmp11=Cyc_Core_new_string(5U);struct _fat_ptr t=_tmp11;
int j=0;
({struct _fat_ptr _tmp12=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp13=*((char*)_check_fat_subscript(_tmp12,sizeof(char),0U));char _tmp14='\\';if(_get_fat_size(_tmp12,sizeof(char))== 1U &&(_tmp13 == 0 && _tmp14 != 0))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _fat_ptr _tmp15=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp16=*((char*)_check_fat_subscript(_tmp15,sizeof(char),0U));char _tmp17=(char)((int)'0' + ((int)((unsigned char)c)>> 6 & 3));if(_get_fat_size(_tmp15,sizeof(char))== 1U &&(_tmp16 == 0 && _tmp17 != 0))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _fat_ptr _tmp18=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp19=*((char*)_check_fat_subscript(_tmp18,sizeof(char),0U));char _tmp1A=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_fat_size(_tmp18,sizeof(char))== 1U &&(_tmp19 == 0 && _tmp1A != 0))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _fat_ptr _tmp1B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp1C=*((char*)_check_fat_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=(char)((int)'0' + ((int)c & 7));if(_get_fat_size(_tmp1B,sizeof(char))== 1U &&(_tmp1C == 0 && _tmp1D != 0))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _fat_ptr)t;}}_LL0:;}
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
# 319
if((int)c >= (int)' ' &&(int)c <= (int)'~')++ len;else{
len +=4;}
goto _LL0;}}_LL0:;}}{
# 324
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
# 338
if((int)c >= (int)' ' &&(int)c <= (int)'~')({struct _fat_ptr _tmp5A=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5B=*((char*)_check_fat_subscript(_tmp5A,sizeof(char),0U));char _tmp5C=c;if(_get_fat_size(_tmp5A,sizeof(char))== 1U &&(_tmp5B == 0 && _tmp5C != 0))_throw_arraybounds();*((char*)_tmp5A.curr)=_tmp5C;});else{
# 340
({struct _fat_ptr _tmp5D=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5E=*((char*)_check_fat_subscript(_tmp5D,sizeof(char),0U));char _tmp5F='\\';if(_get_fat_size(_tmp5D,sizeof(char))== 1U &&(_tmp5E == 0 && _tmp5F != 0))_throw_arraybounds();*((char*)_tmp5D.curr)=_tmp5F;});
({struct _fat_ptr _tmp60=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp61=*((char*)_check_fat_subscript(_tmp60,sizeof(char),0U));char _tmp62=(char)((int)'0' + ((int)c >> 6 & 7));if(_get_fat_size(_tmp60,sizeof(char))== 1U &&(_tmp61 == 0 && _tmp62 != 0))_throw_arraybounds();*((char*)_tmp60.curr)=_tmp62;});
({struct _fat_ptr _tmp63=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp64=*((char*)_check_fat_subscript(_tmp63,sizeof(char),0U));char _tmp65=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_fat_size(_tmp63,sizeof(char))== 1U &&(_tmp64 == 0 && _tmp65 != 0))_throw_arraybounds();*((char*)_tmp63.curr)=_tmp65;});
({struct _fat_ptr _tmp66=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp67=*((char*)_check_fat_subscript(_tmp66,sizeof(char),0U));char _tmp68=(char)((int)'0' + ((int)c & 7));if(_get_fat_size(_tmp66,sizeof(char))== 1U &&(_tmp67 == 0 && _tmp68 != 0))_throw_arraybounds();*((char*)_tmp66.curr)=_tmp68;});}
# 345
goto _LL15;}}_LL15:;}}
# 347
return(struct _fat_ptr)t;}}}}static char _tmp69[9U]="restrict";
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
return({struct _fat_ptr _tmp562=({const char*_tmp6F="";_tag_fat(_tmp6F,sizeof(char),1U);});struct _fat_ptr _tmp561=({const char*_tmp70=" ";_tag_fat(_tmp70,sizeof(char),2U);});struct _fat_ptr _tmp560=({const char*_tmp71=" ";_tag_fat(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp562,_tmp561,_tmp560,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 366
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_AliasQual _tmp74;enum Cyc_Absyn_KindQual _tmp73;_LL1: _tmp73=_tmp72->kind;_tmp74=_tmp72->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp73;enum Cyc_Absyn_AliasQual a=_tmp74;
enum Cyc_Absyn_KindQual _tmp75=k;switch(_tmp75){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 370
enum Cyc_Absyn_AliasQual _tmp76=a;switch(_tmp76){case Cyc_Absyn_Aliasable: _LL17: _LL18:
 return({const char*_tmp77="A";_tag_fat(_tmp77,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL19: _LL1A:
 return({const char*_tmp78="UA";_tag_fat(_tmp78,sizeof(char),3U);});case Cyc_Absyn_Top: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return({const char*_tmp79="TA";_tag_fat(_tmp79,sizeof(char),3U);});}_LL16:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 376
enum Cyc_Absyn_AliasQual _tmp7A=a;switch(_tmp7A){case Cyc_Absyn_Aliasable: _LL20: _LL21:
 return({const char*_tmp7B="M";_tag_fat(_tmp7B,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL22: _LL23:
 return({const char*_tmp7C="UM";_tag_fat(_tmp7C,sizeof(char),3U);});case Cyc_Absyn_Top: _LL24: _LL25:
 goto _LL27;default: _LL26: _LL27: return({const char*_tmp7D="TM";_tag_fat(_tmp7D,sizeof(char),3U);});}_LL1F:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 382
enum Cyc_Absyn_AliasQual _tmp7E=a;switch(_tmp7E){case Cyc_Absyn_Aliasable: _LL29: _LL2A:
 return({const char*_tmp7F="B";_tag_fat(_tmp7F,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL2B: _LL2C:
 return({const char*_tmp80="UB";_tag_fat(_tmp80,sizeof(char),3U);});case Cyc_Absyn_Top: _LL2D: _LL2E:
 goto _LL30;default: _LL2F: _LL30: return({const char*_tmp81="TB";_tag_fat(_tmp81,sizeof(char),3U);});}_LL28:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 388
enum Cyc_Absyn_AliasQual _tmp82=a;switch(_tmp82){case Cyc_Absyn_Aliasable: _LL32: _LL33:
 return({const char*_tmp83="R";_tag_fat(_tmp83,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL34: _LL35:
 return({const char*_tmp84="UR";_tag_fat(_tmp84,sizeof(char),3U);});case Cyc_Absyn_Top: _LL36: _LL37:
 goto _LL39;default: _LL38: _LL39: return({const char*_tmp85="TR";_tag_fat(_tmp85,sizeof(char),3U);});}_LL31:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 393
 return({const char*_tmp86="E";_tag_fat(_tmp86,sizeof(char),2U);});case Cyc_Absyn_IntKind: _LLE: _LLF:
 return({const char*_tmp87="I";_tag_fat(_tmp87,sizeof(char),2U);});case Cyc_Absyn_BoolKind: _LL10: _LL11:
 return({const char*_tmp88="BOOL";_tag_fat(_tmp88,sizeof(char),5U);});case Cyc_Absyn_PtrBndKind: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15: return({const char*_tmp89="PTRBND";_tag_fat(_tmp89,sizeof(char),7U);});}_LL3:;}}
# 399
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 401
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp8A=Cyc_Absyn_compress_kb(c);void*_stmttmp2=_tmp8A;void*_tmp8B=_stmttmp2;struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8D;switch(*((int*)_tmp8B)){case 0U: _LL1: _tmp8D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp8D;
return Cyc_Absynpp_kind2string(k);}case 1U: _LL3: _LL4:
# 405
 if(Cyc_PP_tex_output)
return({const char*_tmp8E="{?}";_tag_fat(_tmp8E,sizeof(char),4U);});else{
return({const char*_tmp8F="?";_tag_fat(_tmp8F,sizeof(char),2U);});}default: _LL5: _tmp8C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp8C;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp92=({struct Cyc_String_pa_PrintArg_struct _tmp540;_tmp540.tag=0U,({struct _fat_ptr _tmp563=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k));_tmp540.f1=_tmp563;});_tmp540;});void*_tmp90[1U];_tmp90[0]=& _tmp92;({struct _fat_ptr _tmp564=({const char*_tmp91="<=%s";_tag_fat(_tmp91,sizeof(char),5U);});Cyc_aprintf(_tmp564,_tag_fat(_tmp90,sizeof(void*),1U));});});}}_LL0:;}
# 412
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp93=Cyc_Absyn_compress_kb(c);void*_stmttmp3=_tmp93;void*_tmp94=_stmttmp3;struct Cyc_Absyn_Kind*_tmp95;struct Cyc_Absyn_Kind*_tmp96;switch(*((int*)_tmp94)){case 0U: _LL1: _tmp96=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp96;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1U: _LL3: _LL4:
# 416
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp97="{?}";_tag_fat(_tmp97,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp98="?";_tag_fat(_tmp98,sizeof(char),2U);}));}default: _LL5: _tmp95=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmp95;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}}_LL0:;}
# 423
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp567=({const char*_tmp99="<";_tag_fat(_tmp99,sizeof(char),2U);});struct _fat_ptr _tmp566=({const char*_tmp9A=">";_tag_fat(_tmp9A,sizeof(char),2U);});struct _fat_ptr _tmp565=({const char*_tmp9B=",";_tag_fat(_tmp9B,sizeof(char),2U);});Cyc_PP_egroup(_tmp567,_tmp566,_tmp565,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 427
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr _tmp9C=*tv->name;struct _fat_ptr n=_tmp9C;
# 431
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
# 433
struct _fat_ptr kstring=({const char*_tmpA5="K";_tag_fat(_tmpA5,sizeof(char),2U);});
{void*_tmp9D=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp4=_tmp9D;void*_tmp9E=_stmttmp4;struct Cyc_Absyn_Kind*_tmp9F;struct Cyc_Absyn_Kind*_tmpA0;switch(*((int*)_tmp9E)){case 2U: _LL1: _tmpA0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f2;_LL2: {struct Cyc_Absyn_Kind*k=_tmpA0;
_tmp9F=k;goto _LL4;}case 0U: _LL3: _tmp9F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9E)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_tmp9F;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 439
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,_tmp542.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_tmp542;});struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,({struct _fat_ptr _tmp568=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(n,sizeof(char),1));_tmp541.f1=_tmp568;});_tmp541;});void*_tmpA1[2U];_tmpA1[0]=& _tmpA3,_tmpA1[1]=& _tmpA4;({struct _fat_ptr _tmp569=({const char*_tmpA2="`G%s%s";_tag_fat(_tmpA2,sizeof(char),7U);});Cyc_aprintf(_tmp569,_tag_fat(_tmpA1,sizeof(void*),2U));});}));}
# 441
return Cyc_PP_text(n);}
# 444
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp5=_tmpA6;void*_tmpA7=_stmttmp5;struct Cyc_Absyn_Kind*_tmpA8;struct Cyc_Absyn_Kind*_tmpA9;switch(*((int*)_tmpA7)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpA9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmpA9;
# 449
return({struct Cyc_PP_Doc*_tmpAA[3U];({struct Cyc_PP_Doc*_tmp56C=Cyc_Absynpp_tvar2doc(tv);_tmpAA[0]=_tmp56C;}),({struct Cyc_PP_Doc*_tmp56B=Cyc_PP_text(({const char*_tmpAB="::";_tag_fat(_tmpAB,sizeof(char),3U);}));_tmpAA[1]=_tmp56B;}),({struct Cyc_PP_Doc*_tmp56A=Cyc_Absynpp_kind2doc(k);_tmpAA[2]=_tmp56A;});Cyc_PP_cat(_tag_fat(_tmpAA,sizeof(struct Cyc_PP_Doc*),3U));});}}default: _LL5: _tmpA8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;_LL6: {struct Cyc_Absyn_Kind*k=_tmpA8;
# 448
_tmpA9=k;goto _LL8;}}_LL0:;}
# 453
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _tmp56F=({const char*_tmpAC="<";_tag_fat(_tmpAC,sizeof(char),2U);});struct _fat_ptr _tmp56E=({const char*_tmpAD=">";_tag_fat(_tmpAD,sizeof(char),2U);});struct _fat_ptr _tmp56D=({const char*_tmpAE=",";_tag_fat(_tmpAE,sizeof(char),2U);});Cyc_PP_egroup(_tmp56F,_tmp56E,_tmp56D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 457
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _tmp572=({const char*_tmpAF="<";_tag_fat(_tmpAF,sizeof(char),2U);});struct _fat_ptr _tmp571=({const char*_tmpB0=">";_tag_fat(_tmpB0,sizeof(char),2U);});struct _fat_ptr _tmp570=({const char*_tmpB1=",";_tag_fat(_tmpB1,sizeof(char),2U);});Cyc_PP_egroup(_tmp572,_tmp571,_tmp570,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 463
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple16*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 467
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp575=({const char*_tmpB2="(";_tag_fat(_tmpB2,sizeof(char),2U);});struct _fat_ptr _tmp574=({const char*_tmpB3=")";_tag_fat(_tmpB3,sizeof(char),2U);});struct _fat_ptr _tmp573=({const char*_tmpB4=",";_tag_fat(_tmpB4,sizeof(char),2U);});Cyc_PP_group(_tmp575,_tmp574,_tmp573,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
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
void*_tmpB6=(void*)atts->hd;void*_stmttmp6=_tmpB6;void*_tmpB7=_stmttmp6;switch(*((int*)_tmpB7)){case 1U: _LL1: _LL2:
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
void*_tmpBB=(void*)atts2->hd;void*_stmttmp7=_tmpBB;void*_tmpBC=_stmttmp7;switch(*((int*)_tmpBC)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 500
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBD[3U];({struct Cyc_PP_Doc*_tmp57B=Cyc_PP_text(({const char*_tmpBE=" __declspec(";_tag_fat(_tmpBE,sizeof(char),13U);}));_tmpBD[0]=_tmp57B;}),({
struct Cyc_PP_Doc*_tmp57A=({struct _fat_ptr _tmp579=({const char*_tmpBF="";_tag_fat(_tmpBF,sizeof(char),1U);});struct _fat_ptr _tmp578=({const char*_tmpC0="";_tag_fat(_tmpC0,sizeof(char),1U);});struct _fat_ptr _tmp577=({const char*_tmpC1=" ";_tag_fat(_tmpC1,sizeof(char),2U);});Cyc_PP_group(_tmp579,_tmp578,_tmp577,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBD[1]=_tmp57A;}),({
struct Cyc_PP_Doc*_tmp576=Cyc_PP_text(({const char*_tmpC2=")";_tag_fat(_tmpC2,sizeof(char),2U);}));_tmpBD[2]=_tmp576;});Cyc_PP_cat(_tag_fat(_tmpBD,sizeof(struct Cyc_PP_Doc*),3U));});}
# 507
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 511
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC3=Cyc_Cyclone_c_compiler;if(_tmpC3 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 516
 return({struct Cyc_PP_Doc*_tmpC4[2U];({struct Cyc_PP_Doc*_tmp580=Cyc_PP_text(({const char*_tmpC5=" __attribute__";_tag_fat(_tmpC5,sizeof(char),15U);}));_tmpC4[0]=_tmp580;}),({
struct Cyc_PP_Doc*_tmp57F=({struct _fat_ptr _tmp57E=({const char*_tmpC6="((";_tag_fat(_tmpC6,sizeof(char),3U);});struct _fat_ptr _tmp57D=({const char*_tmpC7="))";_tag_fat(_tmpC7,sizeof(char),3U);});struct _fat_ptr _tmp57C=({const char*_tmpC8=",";_tag_fat(_tmpC8,sizeof(char),2U);});Cyc_PP_group(_tmp57E,_tmp57D,_tmp57C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC4[1]=_tmp57F;});Cyc_PP_cat(_tag_fat(_tmpC4,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}}
# 521
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC9=(void*)tms->hd;void*_stmttmp8=_tmpC9;void*_tmpCA=_stmttmp8;switch(*((int*)_tmpCA)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 526
enum Cyc_Cyclone_C_Compilers _tmpCB=Cyc_Cyclone_c_compiler;if(_tmpCB == Cyc_Cyclone_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 530
 return 0;}_LL0:;}}
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
return({struct Cyc_PP_Doc*_tmpD4[3U];({struct Cyc_PP_Doc*_tmp583=Cyc_Absynpp_lb();_tmpD4[0]=_tmp583;}),({struct Cyc_PP_Doc*_tmp582=Cyc_PP_seq(sep,ss);_tmpD4[1]=_tmp582;}),({struct Cyc_PP_Doc*_tmp581=Cyc_Absynpp_rb();_tmpD4[2]=_tmp581;});Cyc_PP_cat(_tag_fat(_tmpD4,sizeof(struct Cyc_PP_Doc*),3U));});}
# 578
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD5=(void*)tms->hd;void*_stmttmp9=_tmpD5;void*_tmpD6=_stmttmp9;struct Cyc_List_List*_tmpD7;switch(*((int*)_tmpD6)){case 0U: _LL1: _LL2:
({void*_tmpD8=0U;({struct Cyc___cycFILE*_tmp585=Cyc_stderr;struct _fat_ptr _tmp584=({const char*_tmpD9="Carray_mod ";_tag_fat(_tmpD9,sizeof(char),12U);});Cyc_fprintf(_tmp585,_tmp584,_tag_fat(_tmpD8,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpDA=0U;({struct Cyc___cycFILE*_tmp587=Cyc_stderr;struct _fat_ptr _tmp586=({const char*_tmpDB="ConstArray_mod ";_tag_fat(_tmpDB,sizeof(char),16U);});Cyc_fprintf(_tmp587,_tmp586,_tag_fat(_tmpDA,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->tag == 1U){_LL5: _tmpD7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->f1;_LL6: {struct Cyc_List_List*ys=_tmpD7;
# 584
({void*_tmpDC=0U;({struct Cyc___cycFILE*_tmp589=Cyc_stderr;struct _fat_ptr _tmp588=({const char*_tmpDD="Function_mod(";_tag_fat(_tmpDD,sizeof(char),14U);});Cyc_fprintf(_tmp589,_tmp588,_tag_fat(_tmpDC,sizeof(void*),0U));});});
for(0;ys != 0;ys=ys->tl){
struct _fat_ptr*_tmpDE=(*((struct _tuple8*)ys->hd)).f1;struct _fat_ptr*v=_tmpDE;
if(v == 0)({void*_tmpDF=0U;({struct Cyc___cycFILE*_tmp58B=Cyc_stderr;struct _fat_ptr _tmp58A=({const char*_tmpE0="?";_tag_fat(_tmpE0,sizeof(char),2U);});Cyc_fprintf(_tmp58B,_tmp58A,_tag_fat(_tmpDF,sizeof(void*),0U));});});else{
({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp58D=Cyc_stderr;struct _fat_ptr _tmp58C=*v;Cyc_fprintf(_tmp58D,_tmp58C,_tag_fat(_tmpE1,sizeof(void*),0U));});});}
if(ys->tl != 0)({void*_tmpE2=0U;({struct Cyc___cycFILE*_tmp58F=Cyc_stderr;struct _fat_ptr _tmp58E=({const char*_tmpE3=",";_tag_fat(_tmpE3,sizeof(char),2U);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_fat(_tmpE2,sizeof(void*),0U));});});}
# 591
({void*_tmpE4=0U;({struct Cyc___cycFILE*_tmp591=Cyc_stderr;struct _fat_ptr _tmp590=({const char*_tmpE5=") ";_tag_fat(_tmpE5,sizeof(char),3U);});Cyc_fprintf(_tmp591,_tmp590,_tag_fat(_tmpE4,sizeof(void*),0U));});});
goto _LL0;}}else{_LL7: _LL8:
# 594
({void*_tmpE6=0U;({struct Cyc___cycFILE*_tmp593=Cyc_stderr;struct _fat_ptr _tmp592=({const char*_tmpE7="Function_mod()";_tag_fat(_tmpE7,sizeof(char),15U);});Cyc_fprintf(_tmp593,_tmp592,_tag_fat(_tmpE6,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE8=0U;({struct Cyc___cycFILE*_tmp595=Cyc_stderr;struct _fat_ptr _tmp594=({const char*_tmpE9="Attributes_mod ";_tag_fat(_tmpE9,sizeof(char),16U);});Cyc_fprintf(_tmp595,_tmp594,_tag_fat(_tmpE8,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpEA=0U;({struct Cyc___cycFILE*_tmp597=Cyc_stderr;struct _fat_ptr _tmp596=({const char*_tmpEB="TypeParams_mod ";_tag_fat(_tmpEB,sizeof(char),16U);});Cyc_fprintf(_tmp597,_tmp596,_tag_fat(_tmpEA,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpEC=0U;({struct Cyc___cycFILE*_tmp599=Cyc_stderr;struct _fat_ptr _tmp598=({const char*_tmpED="Pointer_mod ";_tag_fat(_tmpED,sizeof(char),13U);});Cyc_fprintf(_tmp599,_tmp598,_tag_fat(_tmpEC,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 599
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp59B=Cyc_stderr;struct _fat_ptr _tmp59A=({const char*_tmpEF="\n";_tag_fat(_tmpEF,sizeof(char),2U);});Cyc_fprintf(_tmp59B,_tmp59A,_tag_fat(_tmpEE,sizeof(void*),0U));});});}
# 602
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 604
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp134[3U];({struct Cyc_PP_Doc*_tmp59D=Cyc_PP_text(({const char*_tmp135="(";_tag_fat(_tmp135,sizeof(char),2U);}));_tmp134[0]=_tmp59D;}),_tmp134[1]=rest,({struct Cyc_PP_Doc*_tmp59C=Cyc_PP_text(({const char*_tmp136=")";_tag_fat(_tmp136,sizeof(char),2U);}));_tmp134[2]=_tmp59C;});Cyc_PP_cat(_tag_fat(_tmp134,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpF0=(void*)tms->hd;void*_stmttmpA=_tmpF0;void*_tmpF1=_stmttmpA;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF5;void*_tmpF4;void*_tmpF3;void*_tmpF2;int _tmpF9;unsigned _tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpFA;void*_tmpFB;void*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFE;switch(*((int*)_tmpF1)){case 0U: _LL1: _tmpFE=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL2: {void*zeroterm=_tmpFE;
# 610
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpFF[2U];_tmpFF[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_tmp59F=Cyc_PP_text(({const char*_tmp100="[]@zeroterm";_tag_fat(_tmp100,sizeof(char),12U);}));_tmpFF[1]=_tmp59F;}):({struct Cyc_PP_Doc*_tmp59E=Cyc_PP_text(({const char*_tmp101="[]";_tag_fat(_tmp101,sizeof(char),3U);}));_tmpFF[1]=_tmp59E;});Cyc_PP_cat(_tag_fat(_tmpFF,sizeof(struct Cyc_PP_Doc*),2U));});}case 1U: _LL3: _tmpFC=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmpFD=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL4: {struct Cyc_Absyn_Exp*e=_tmpFC;void*zeroterm=_tmpFD;
# 614
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp102[4U];_tmp102[0]=rest,({struct Cyc_PP_Doc*_tmp5A3=Cyc_PP_text(({const char*_tmp103="[";_tag_fat(_tmp103,sizeof(char),2U);}));_tmp102[1]=_tmp5A3;}),({struct Cyc_PP_Doc*_tmp5A2=Cyc_Absynpp_exp2doc(e);_tmp102[2]=_tmp5A2;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_tmp5A1=Cyc_PP_text(({const char*_tmp104="]@zeroterm";_tag_fat(_tmp104,sizeof(char),11U);}));_tmp102[3]=_tmp5A1;}):({struct Cyc_PP_Doc*_tmp5A0=Cyc_PP_text(({const char*_tmp105="]";_tag_fat(_tmp105,sizeof(char),2U);}));_tmp102[3]=_tmp5A0;});Cyc_PP_cat(_tag_fat(_tmp102,sizeof(struct Cyc_PP_Doc*),4U));});}case 3U: _LL5: _tmpFB=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL6: {void*args=_tmpFB;
# 618
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp106=args;unsigned _tmp108;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_List_List*_tmp10D;void*_tmp10C;struct Cyc_Absyn_VarargInfo*_tmp10B;int _tmp10A;struct Cyc_List_List*_tmp109;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->tag == 1U){_LLE: _tmp109=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f1;_tmp10A=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f2;_tmp10B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f3;_tmp10C=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f4;_tmp10D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f5;_tmp10E=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f6;_tmp10F=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp106)->f7;_LLF: {struct Cyc_List_List*args2=_tmp109;int c_varargs=_tmp10A;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp10B;void*effopt=_tmp10C;struct Cyc_List_List*rgn_po=_tmp10D;struct Cyc_Absyn_Exp*req=_tmp10E;struct Cyc_Absyn_Exp*ens=_tmp10F;
# 621
return({struct Cyc_PP_Doc*_tmp110[2U];_tmp110[0]=rest,({struct Cyc_PP_Doc*_tmp5A4=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);_tmp110[1]=_tmp5A4;});Cyc_PP_cat(_tag_fat(_tmp110,sizeof(struct Cyc_PP_Doc*),2U));});}}else{_LL10: _tmp107=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp106)->f1;_tmp108=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp106)->f2;_LL11: {struct Cyc_List_List*sl=_tmp107;unsigned loc=_tmp108;
# 623
return({struct Cyc_PP_Doc*_tmp111[2U];_tmp111[0]=rest,({struct Cyc_PP_Doc*_tmp5A8=({struct _fat_ptr _tmp5A7=({const char*_tmp112="(";_tag_fat(_tmp112,sizeof(char),2U);});struct _fat_ptr _tmp5A6=({const char*_tmp113=")";_tag_fat(_tmp113,sizeof(char),2U);});struct _fat_ptr _tmp5A5=({const char*_tmp114=",";_tag_fat(_tmp114,sizeof(char),2U);});Cyc_PP_group(_tmp5A7,_tmp5A6,_tmp5A5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,sl));});_tmp111[1]=_tmp5A8;});Cyc_PP_cat(_tag_fat(_tmp111,sizeof(struct Cyc_PP_Doc*),2U));});}}_LLD:;}}case 5U: _LL7: _tmpFA=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL8: {struct Cyc_List_List*atts=_tmpFA;
# 626
enum Cyc_Cyclone_C_Compilers _tmp115=Cyc_Cyclone_c_compiler;if(_tmp115 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp116[2U];_tmp116[0]=rest,({struct Cyc_PP_Doc*_tmp5A9=Cyc_Absynpp_atts2doc(atts);_tmp116[1]=_tmp5A9;});Cyc_PP_cat(_tag_fat(_tmp116,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp117[2U];({struct Cyc_PP_Doc*_tmp5AA=Cyc_Absynpp_callconv2doc(atts);_tmp117[0]=_tmp5AA;}),_tmp117[1]=rest;Cyc_PP_cat(_tag_fat(_tmp117,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmpF7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmpF8=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_tmpF9=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f3;_LLA: {struct Cyc_List_List*ts=_tmpF7;unsigned loc=_tmpF8;int print_kinds=_tmpF9;
# 637
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_tmp118[2U];_tmp118[0]=rest,({struct Cyc_PP_Doc*_tmp5AB=Cyc_Absynpp_ktvars2doc(ts);_tmp118[1]=_tmp5AB;});Cyc_PP_cat(_tag_fat(_tmp118,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 641
return({struct Cyc_PP_Doc*_tmp119[2U];_tmp119[0]=rest,({struct Cyc_PP_Doc*_tmp5AC=Cyc_Absynpp_tvars2doc(ts);_tmp119[1]=_tmp5AC;});Cyc_PP_cat(_tag_fat(_tmp119,sizeof(struct Cyc_PP_Doc*),2U));});}}default: _LLB: _tmpF2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).rgn;_tmpF3=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).nullable;_tmpF4=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).bounds;_tmpF5=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).zero_term;_tmpF6=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LLC: {void*rgn=_tmpF2;void*nullable=_tmpF3;void*bd=_tmpF4;void*zt=_tmpF5;struct Cyc_Absyn_Tqual tq2=_tmpF6;
# 645
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_tmp11A=Cyc_Tcutil_compress(bd);void*_stmttmpB=_tmp11A;void*_tmp11B=_stmttmpB;void*_tmp11C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11B)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11B)->f2 != 0){_LL1A: _tmp11C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11B)->f2)->hd;_LL1B: {void*targ=_tmp11C;
# 653
{void*_tmp11D=Cyc_Tcutil_compress(targ);void*_stmttmpC=_tmp11D;void*_tmp11E=_stmttmpC;struct Cyc_Absyn_Exp*_tmp11F;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp11E)->tag == 9U){_LL1F: _tmp11F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp11E)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp11F;
# 655
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp120="*";_tag_fat(_tmp120,sizeof(char),2U);}):({const char*_tmp121="@";_tag_fat(_tmp121,sizeof(char),2U);}));{
struct _tuple11 _tmp122=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _stmttmpD=_tmp122;struct _tuple11 _tmp123=_stmttmpD;int _tmp125;unsigned _tmp124;_LL24: _tmp124=_tmp123.f1;_tmp125=_tmp123.f2;_LL25: {unsigned val=_tmp124;int known=_tmp125;
if(!known || val != (unsigned)1)
ptr=({struct Cyc_PP_Doc*_tmp126[4U];_tmp126[0]=ptr,({struct Cyc_PP_Doc*_tmp5AF=Cyc_Absynpp_lb();_tmp126[1]=_tmp5AF;}),({struct Cyc_PP_Doc*_tmp5AE=Cyc_Absynpp_exp2doc(e);_tmp126[2]=_tmp5AE;}),({struct Cyc_PP_Doc*_tmp5AD=Cyc_Absynpp_rb();_tmp126[3]=_tmp5AD;});Cyc_PP_cat(_tag_fat(_tmp126,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}}}}else{_LL21: _LL22:
# 661
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp127="*";_tag_fat(_tmp127,sizeof(char),2U);}):({const char*_tmp128="@";_tag_fat(_tmp128,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp129[4U];_tmp129[0]=ptr,({struct Cyc_PP_Doc*_tmp5B2=Cyc_Absynpp_lb();_tmp129[1]=_tmp5B2;}),({struct Cyc_PP_Doc*_tmp5B1=Cyc_Absynpp_typ2doc(targ);_tmp129[2]=_tmp5B1;}),({struct Cyc_PP_Doc*_tmp5B0=Cyc_Absynpp_rb();_tmp129[3]=_tmp5B0;});Cyc_PP_cat(_tag_fat(_tmp129,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}_LL1E:;}
# 665
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 667
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp12A="*";_tag_fat(_tmp12A,sizeof(char),2U);}):({const char*_tmp12B="@";_tag_fat(_tmp12B,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp12C[4U];_tmp12C[0]=ptr,({struct Cyc_PP_Doc*_tmp5B5=Cyc_Absynpp_lb();_tmp12C[1]=_tmp5B5;}),({struct Cyc_PP_Doc*_tmp5B4=Cyc_Absynpp_typ2doc(bd);_tmp12C[2]=_tmp5B4;}),({struct Cyc_PP_Doc*_tmp5B3=Cyc_Absynpp_rb();_tmp12C[3]=_tmp5B3;});Cyc_PP_cat(_tag_fat(_tmp12C,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 672
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr && Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp12D="@zeroterm";_tag_fat(_tmp12D,sizeof(char),10U);}));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp12E="@nozeroterm";_tag_fat(_tmp12E,sizeof(char),12U);}));}}
# 678
{void*_tmp12F=Cyc_Tcutil_compress(rgn);void*_stmttmpE=_tmp12F;void*_tmp130=_stmttmpE;switch(*((int*)_tmp130)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1)->tag == 5U){_LL27: _LL28:
 goto _LL26;}else{goto _LL2B;}case 1U: _LL29: if(Cyc_Absynpp_print_for_cycdoc){_LL2A:
 goto _LL26;}else{goto _LL2B;}default: _LL2B: _LL2C:
 rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL26:;}{
# 683
struct Cyc_PP_Doc*spacer1=tqd != mt &&(ztd != mt || rgd != mt)?Cyc_PP_text(({const char*_tmp133=" ";_tag_fat(_tmp133,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp132=" ";_tag_fat(_tmp132,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp131[7U];_tmp131[0]=ptr,_tmp131[1]=ztd,_tmp131[2]=rgd,_tmp131[3]=spacer1,_tmp131[4]=tqd,_tmp131[5]=spacer2,_tmp131[6]=rest;Cyc_PP_cat(_tag_fat(_tmp131,sizeof(struct Cyc_PP_Doc*),7U));});}}}_LL0:;}}
# 689
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_stmttmpF=_tmp137;void*_tmp138=_stmttmpF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp138)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp138)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp139="`H";_tag_fat(_tmp139,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp13A="`U";_tag_fat(_tmp13A,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp13B="`RC";_tag_fat(_tmp13B,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 699
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 703
void*_tmp13C=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp13C;void*_tmp13D=_stmttmp10;struct Cyc_List_List*_tmp13E;void*_tmp13F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2 != 0){_LL1: _tmp13F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2)->hd;_LL2: {void*r=_tmp13F;
({struct Cyc_List_List*_tmp5B7=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_PP_Doc*_tmp5B6=Cyc_Absynpp_rgn2doc(r);_tmp140->hd=_tmp5B6;}),_tmp140->tl=*rgions;_tmp140;});*rgions=_tmp5B7;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp13E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2;_LL4: {struct Cyc_List_List*ts=_tmp13E;
# 706
for(0;ts != 0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
# 709
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp5B9=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));({struct Cyc_PP_Doc*_tmp5B8=Cyc_Absynpp_typ2doc(t);_tmp141->hd=_tmp5B8;}),_tmp141->tl=*effects;_tmp141;});*effects=_tmp5B9;});goto _LL0;}_LL0:;}
# 714
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0 && effects != 0)
return({struct _fat_ptr _tmp5BC=({const char*_tmp142="";_tag_fat(_tmp142,sizeof(char),1U);});struct _fat_ptr _tmp5BB=({const char*_tmp143="";_tag_fat(_tmp143,sizeof(char),1U);});struct _fat_ptr _tmp5BA=({const char*_tmp144="+";_tag_fat(_tmp144,sizeof(char),2U);});Cyc_PP_group(_tmp5BC,_tmp5BB,_tmp5BA,effects);});else{
# 722
struct Cyc_PP_Doc*_tmp145=({struct _fat_ptr _tmp5BD=({const char*_tmp14A=",";_tag_fat(_tmp14A,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp5BD,rgions);});struct Cyc_PP_Doc*doc1=_tmp145;
return({struct _fat_ptr _tmp5C1=({const char*_tmp146="";_tag_fat(_tmp146,sizeof(char),1U);});struct _fat_ptr _tmp5C0=({const char*_tmp147="";_tag_fat(_tmp147,sizeof(char),1U);});struct _fat_ptr _tmp5BF=({const char*_tmp148="+";_tag_fat(_tmp148,sizeof(char),2U);});Cyc_PP_group(_tmp5C1,_tmp5C0,_tmp5BF,({struct Cyc_List_List*_tmp5BE=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5BE,({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=doc1,_tmp149->tl=0;_tmp149;}));}));});}}
# 727
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp14B=k;if(_tmp14B == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp14C="struct ";_tag_fat(_tmp14C,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp14D="union ";_tag_fat(_tmp14D,sizeof(char),7U);}));}_LL0:;}
# 735
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp14E=t;struct Cyc_Absyn_Datatypedecl*_tmp14F;struct Cyc_Absyn_Enumdecl*_tmp150;struct Cyc_Absyn_Aggrdecl*_tmp151;struct Cyc_Absyn_Typedefdecl*_tmp154;struct Cyc_List_List*_tmp153;struct _tuple0*_tmp152;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*_tmp158;enum Cyc_Absyn_AggrKind _tmp157;struct Cyc_List_List*_tmp159;struct Cyc_Absyn_Tvar*_tmp15A;struct Cyc_Core_Opt*_tmp15E;int _tmp15D;void*_tmp15C;struct Cyc_Core_Opt*_tmp15B;void*_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp162;struct _fat_ptr _tmp163;struct _tuple0*_tmp164;struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp167;union Cyc_Absyn_AggrInfo _tmp166;int _tmp168;enum Cyc_Absyn_Size_of _tmp16A;enum Cyc_Absyn_Sign _tmp169;struct Cyc_List_List*_tmp16C;union Cyc_Absyn_DatatypeFieldInfo _tmp16B;struct Cyc_List_List*_tmp16E;union Cyc_Absyn_DatatypeInfo _tmp16D;switch(*((int*)_tmp14E)){case 4U: _LL1: _LL2:
# 739
 return Cyc_PP_text(({const char*_tmp16F="[[[array]]]";_tag_fat(_tmp16F,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)){case 0U: _LL7: _LL8:
# 743
 s=Cyc_PP_text(({const char*_tmp170="void";_tag_fat(_tmp170,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp16D=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp16E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LLE: {union Cyc_Absyn_DatatypeInfo tu_info=_tmp16D;struct Cyc_List_List*ts=_tmp16E;
# 759
{union Cyc_Absyn_DatatypeInfo _tmp178=tu_info;int _tmp17A;struct _tuple0*_tmp179;int _tmp17C;struct _tuple0*_tmp17B;if((_tmp178.UnknownDatatype).tag == 1){_LL48: _tmp17B=((_tmp178.UnknownDatatype).val).name;_tmp17C=((_tmp178.UnknownDatatype).val).is_extensible;_LL49: {struct _tuple0*n=_tmp17B;int is_x=_tmp17C;
_tmp179=n;_tmp17A=is_x;goto _LL4B;}}else{_LL4A: _tmp179=(*(_tmp178.KnownDatatype).val)->name;_tmp17A=(*(_tmp178.KnownDatatype).val)->is_extensible;_LL4B: {struct _tuple0*n=_tmp179;int is_x=_tmp17A;
# 762
struct Cyc_PP_Doc*_tmp17D=Cyc_PP_text(({const char*_tmp181="datatype ";_tag_fat(_tmp181,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp17D;
struct Cyc_PP_Doc*_tmp17E=is_x?Cyc_PP_text(({const char*_tmp180="@extensible ";_tag_fat(_tmp180,sizeof(char),13U);})): Cyc_PP_nil_doc();struct Cyc_PP_Doc*ext=_tmp17E;
s=({struct Cyc_PP_Doc*_tmp17F[4U];_tmp17F[0]=ext,_tmp17F[1]=kw,({struct Cyc_PP_Doc*_tmp5C3=Cyc_Absynpp_qvar2doc(n);_tmp17F[2]=_tmp5C3;}),({struct Cyc_PP_Doc*_tmp5C2=Cyc_Absynpp_tps2doc(ts);_tmp17F[3]=_tmp5C2;});Cyc_PP_cat(_tag_fat(_tmp17F,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 767
goto _LL0;}case 19U: _LLF: _tmp16B=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL10: {union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp16B;struct Cyc_List_List*ts=_tmp16C;
# 769
{union Cyc_Absyn_DatatypeFieldInfo _tmp182=tuf_info;struct _tuple0*_tmp185;int _tmp184;struct _tuple0*_tmp183;int _tmp188;struct _tuple0*_tmp187;struct _tuple0*_tmp186;if((_tmp182.UnknownDatatypefield).tag == 1){_LL4D: _tmp186=((_tmp182.UnknownDatatypefield).val).datatype_name;_tmp187=((_tmp182.UnknownDatatypefield).val).field_name;_tmp188=((_tmp182.UnknownDatatypefield).val).is_extensible;_LL4E: {struct _tuple0*tname=_tmp186;struct _tuple0*fname=_tmp187;int is_x=_tmp188;
# 771
_tmp183=tname;_tmp184=is_x;_tmp185=fname;goto _LL50;}}else{_LL4F: _tmp183=(((_tmp182.KnownDatatypefield).val).f1)->name;_tmp184=(((_tmp182.KnownDatatypefield).val).f1)->is_extensible;_tmp185=(((_tmp182.KnownDatatypefield).val).f2)->name;_LL50: {struct _tuple0*tname=_tmp183;int is_x=_tmp184;struct _tuple0*fname=_tmp185;
# 774
struct Cyc_PP_Doc*_tmp189=Cyc_PP_text(is_x?({const char*_tmp18C="@extensible datatype ";_tag_fat(_tmp18C,sizeof(char),22U);}):({const char*_tmp18D="datatype ";_tag_fat(_tmp18D,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp189;
s=({struct Cyc_PP_Doc*_tmp18A[4U];_tmp18A[0]=kw,({struct Cyc_PP_Doc*_tmp5C6=Cyc_Absynpp_qvar2doc(tname);_tmp18A[1]=_tmp5C6;}),({struct Cyc_PP_Doc*_tmp5C5=Cyc_PP_text(({const char*_tmp18B=".";_tag_fat(_tmp18B,sizeof(char),2U);}));_tmp18A[2]=_tmp5C5;}),({struct Cyc_PP_Doc*_tmp5C4=Cyc_Absynpp_qvar2doc(fname);_tmp18A[3]=_tmp5C4;});Cyc_PP_cat(_tag_fat(_tmp18A,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 778
goto _LL0;}case 1U: _LL11: _tmp169=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp16A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f2;_LL12: {enum Cyc_Absyn_Sign sn=_tmp169;enum Cyc_Absyn_Size_of sz=_tmp16A;
# 780
struct _fat_ptr sns;
struct _fat_ptr ts;
{enum Cyc_Absyn_Sign _tmp18E=sn;switch(_tmp18E){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp18F="";_tag_fat(_tmp18F,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp190="unsigned ";_tag_fat(_tmp190,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 787
{enum Cyc_Absyn_Size_of _tmp191=sz;switch(_tmp191){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 789
{enum Cyc_Absyn_Sign _tmp192=sn;switch(_tmp192){case Cyc_Absyn_None: _LL66: _LL67:
 sns=({const char*_tmp193="";_tag_fat(_tmp193,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed: _LL68: _LL69:
 sns=({const char*_tmp194="signed ";_tag_fat(_tmp194,sizeof(char),8U);});goto _LL65;default: _LL6A: _LL6B:
 sns=({const char*_tmp195="unsigned ";_tag_fat(_tmp195,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 794
ts=({const char*_tmp196="char";_tag_fat(_tmp196,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp197="short";_tag_fat(_tmp197,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp198="int";_tag_fat(_tmp198,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp199="long";_tag_fat(_tmp199,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
# 801
{enum Cyc_Cyclone_C_Compilers _tmp19A=Cyc_Cyclone_c_compiler;if(_tmp19A == Cyc_Cyclone_Gcc_c){_LL6D: _LL6E:
 ts=({const char*_tmp19B="long long";_tag_fat(_tmp19B,sizeof(char),10U);});goto _LL6C;}else{_LL6F: _LL70:
 ts=({const char*_tmp19C="__int64";_tag_fat(_tmp19C,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 805
goto _LL58;}_LL58:;}
# 807
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19F=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,_tmp544.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_tmp544;});struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,_tmp543.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_tmp543;});void*_tmp19D[2U];_tmp19D[0]=& _tmp19F,_tmp19D[1]=& _tmp1A0;({struct _fat_ptr _tmp5C7=({const char*_tmp19E="%s%s";_tag_fat(_tmp19E,sizeof(char),5U);});Cyc_aprintf(_tmp5C7,_tag_fat(_tmp19D,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp168=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL14: {int i=_tmp168;
# 810
{int _tmp1A1=i;switch(_tmp1A1){case 0U: _LL72: _LL73:
 s=Cyc_PP_text(({const char*_tmp1A2="float";_tag_fat(_tmp1A2,sizeof(char),6U);}));goto _LL71;case 1U: _LL74: _LL75:
 s=Cyc_PP_text(({const char*_tmp1A3="double";_tag_fat(_tmp1A3,sizeof(char),7U);}));goto _LL71;default: _LL76: _LL77:
 s=Cyc_PP_text(({const char*_tmp1A4="long double";_tag_fat(_tmp1A4,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 815
goto _LL0;}case 20U: _LL17: _tmp166=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp167=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL18: {union Cyc_Absyn_AggrInfo info=_tmp166;struct Cyc_List_List*ts=_tmp167;
# 818
struct _tuple10 _tmp1A6=Cyc_Absyn_aggr_kinded_name(info);struct _tuple10 _stmttmp11=_tmp1A6;struct _tuple10 _tmp1A7=_stmttmp11;struct _tuple0*_tmp1A9;enum Cyc_Absyn_AggrKind _tmp1A8;_LL79: _tmp1A8=_tmp1A7.f1;_tmp1A9=_tmp1A7.f2;_LL7A: {enum Cyc_Absyn_AggrKind k=_tmp1A8;struct _tuple0*n=_tmp1A9;
s=({struct Cyc_PP_Doc*_tmp1AA[3U];({struct Cyc_PP_Doc*_tmp5CA=Cyc_Absynpp_aggr_kind2doc(k);_tmp1AA[0]=_tmp5CA;}),({struct Cyc_PP_Doc*_tmp5C9=Cyc_Absynpp_qvar2doc(n);_tmp1AA[1]=_tmp5C9;}),({struct Cyc_PP_Doc*_tmp5C8=Cyc_Absynpp_tps2doc(ts);_tmp1AA[2]=_tmp5C8;});Cyc_PP_cat(_tag_fat(_tmp1AA,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 16U: _LL1B: _tmp165=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1C: {struct Cyc_List_List*fs=_tmp165;
# 827
s=({struct Cyc_PP_Doc*_tmp1AC[4U];({struct Cyc_PP_Doc*_tmp5CE=Cyc_PP_text(({const char*_tmp1AD="enum ";_tag_fat(_tmp1AD,sizeof(char),6U);}));_tmp1AC[0]=_tmp5CE;}),({struct Cyc_PP_Doc*_tmp5CD=Cyc_Absynpp_lb();_tmp1AC[1]=_tmp5CD;}),({struct Cyc_PP_Doc*_tmp5CC=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_tmp1AC[2]=_tmp5CC;}),({struct Cyc_PP_Doc*_tmp5CB=Cyc_Absynpp_rb();_tmp1AC[3]=_tmp5CB;});Cyc_PP_cat(_tag_fat(_tmp1AC,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1D: _tmp164=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1E: {struct _tuple0*n=_tmp164;
s=({struct Cyc_PP_Doc*_tmp1AE[2U];({struct Cyc_PP_Doc*_tmp5D0=Cyc_PP_text(({const char*_tmp1AF="enum ";_tag_fat(_tmp1AF,sizeof(char),6U);}));_tmp1AE[0]=_tmp5D0;}),({struct Cyc_PP_Doc*_tmp5CF=Cyc_Absynpp_qvar2doc(n);_tmp1AE[1]=_tmp5CF;});Cyc_PP_cat(_tag_fat(_tmp1AE,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 17U: _LL23: _tmp163=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL24: {struct _fat_ptr t=_tmp163;
# 831
s=Cyc_PP_text(t);goto _LL0;}case 3U: _LL27: _tmp162=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL28: {struct Cyc_List_List*ts=_tmp162;
# 841
s=({struct Cyc_PP_Doc*_tmp1B7[3U];({struct Cyc_PP_Doc*_tmp5D3=Cyc_PP_text(({const char*_tmp1B8="region_t<";_tag_fat(_tmp1B8,sizeof(char),10U);}));_tmp1B7[0]=_tmp5D3;}),({struct Cyc_PP_Doc*_tmp5D2=Cyc_Absynpp_rgn2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1B7[1]=_tmp5D2;}),({struct Cyc_PP_Doc*_tmp5D1=Cyc_PP_text(({const char*_tmp1B9=">";_tag_fat(_tmp1B9,sizeof(char),2U);}));_tmp1B7[2]=_tmp5D1;});Cyc_PP_cat(_tag_fat(_tmp1B7,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 4U: _LL29: _tmp161=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL2A: {struct Cyc_List_List*ts=_tmp161;
# 843
s=({struct Cyc_PP_Doc*_tmp1BA[3U];({struct Cyc_PP_Doc*_tmp5D6=Cyc_PP_text(({const char*_tmp1BB="tag_t<";_tag_fat(_tmp1BB,sizeof(char),7U);}));_tmp1BA[0]=_tmp5D6;}),({struct Cyc_PP_Doc*_tmp5D5=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1BA[1]=_tmp5D5;}),({struct Cyc_PP_Doc*_tmp5D4=Cyc_PP_text(({const char*_tmp1BC=">";_tag_fat(_tmp1BC,sizeof(char),2U);}));_tmp1BA[2]=_tmp5D4;});Cyc_PP_cat(_tag_fat(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: _LL31: _tmp160=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL32: {struct Cyc_List_List*ts=_tmp160;
# 848
s=({struct Cyc_PP_Doc*_tmp1BD[3U];({struct Cyc_PP_Doc*_tmp5D9=Cyc_PP_text(({const char*_tmp1BE="regions(";_tag_fat(_tmp1BE,sizeof(char),9U);}));_tmp1BD[0]=_tmp5D9;}),({struct Cyc_PP_Doc*_tmp5D8=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(ts))->hd);_tmp1BD[1]=_tmp5D8;}),({struct Cyc_PP_Doc*_tmp5D7=Cyc_PP_text(({const char*_tmp1BF=")";_tag_fat(_tmp1BF,sizeof(char),2U);}));_tmp1BD[2]=_tmp5D7;});Cyc_PP_cat(_tag_fat(_tmp1BD,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL33: _LL34:
 goto _LL36;case 9U: _LL35: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 854
 s=Cyc_PP_text(({const char*_tmp1C0="@true";_tag_fat(_tmp1C0,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
 s=Cyc_PP_text(({const char*_tmp1C1="@false";_tag_fat(_tmp1C1,sizeof(char),7U);}));goto _LL0;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2 != 0){_LL41: _tmp15F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2)->hd;_LL42: {void*t=_tmp15F;
# 857
s=({struct Cyc_PP_Doc*_tmp1C2[4U];({struct Cyc_PP_Doc*_tmp5DD=Cyc_PP_text(({const char*_tmp1C3="@thin @numelts";_tag_fat(_tmp1C3,sizeof(char),15U);}));_tmp1C2[0]=_tmp5DD;}),({struct Cyc_PP_Doc*_tmp5DC=Cyc_Absynpp_lb();_tmp1C2[1]=_tmp5DC;}),({struct Cyc_PP_Doc*_tmp5DB=Cyc_Absynpp_typ2doc(t);_tmp1C2[2]=_tmp5DB;}),({struct Cyc_PP_Doc*_tmp5DA=Cyc_Absynpp_rb();_tmp1C2[3]=_tmp5DA;});Cyc_PP_cat(_tag_fat(_tmp1C2,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}}else{_LL43: _LL44:
# 859
 s=Cyc_PP_text(({const char*_tmp1C4="@thin";_tag_fat(_tmp1C4,sizeof(char),6U);}));goto _LL0;}default: _LL45: _LL46:
 s=Cyc_PP_text(({const char*_tmp1C5="@fat";_tag_fat(_tmp1C5,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp15B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f1;_tmp15C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f2;_tmp15D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f3;_tmp15E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f4;_LLA: {struct Cyc_Core_Opt*k=_tmp15B;void*topt=_tmp15C;int i=_tmp15D;struct Cyc_Core_Opt*tvs=_tmp15E;
# 745
if(topt != 0)
# 747
return Cyc_Absynpp_ntyp2doc(topt);else{
# 749
struct _fat_ptr kindstring=k == 0?({const char*_tmp175="K";_tag_fat(_tmp175,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,_tmp546.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_tmp546;});struct Cyc_Int_pa_PrintArg_struct _tmp174=({struct Cyc_Int_pa_PrintArg_struct _tmp545;_tmp545.tag=1U,_tmp545.f1=(unsigned long)i;_tmp545;});void*_tmp171[2U];_tmp171[0]=& _tmp173,_tmp171[1]=& _tmp174;({struct _fat_ptr _tmp5DE=({const char*_tmp172="`E%s%d";_tag_fat(_tmp172,sizeof(char),7U);});Cyc_aprintf(_tmp5DE,_tag_fat(_tmp171,sizeof(void*),2U));});}));}
# 752
goto _LL0;}case 2U: _LLB: _tmp15A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->f1;_LLC: {struct Cyc_Absyn_Tvar*tv=_tmp15A;
# 754
s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp176[3U];_tmp176[0]=s,({struct Cyc_PP_Doc*_tmp5E0=Cyc_PP_text(({const char*_tmp177="::";_tag_fat(_tmp177,sizeof(char),3U);}));_tmp176[1]=_tmp5E0;}),({struct Cyc_PP_Doc*_tmp5DF=Cyc_Absynpp_kindbound2doc(tv->kind);_tmp176[2]=_tmp5DF;});Cyc_PP_cat(_tag_fat(_tmp176,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 6U: _LL15: _tmp159=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14E)->f1;_LL16: {struct Cyc_List_List*ts=_tmp159;
# 816
s=({struct Cyc_PP_Doc*_tmp1A5[2U];({struct Cyc_PP_Doc*_tmp5E2=Cyc_Absynpp_dollar();_tmp1A5[0]=_tmp5E2;}),({struct Cyc_PP_Doc*_tmp5E1=Cyc_Absynpp_args2doc(ts);_tmp1A5[1]=_tmp5E1;});Cyc_PP_cat(_tag_fat(_tmp1A5,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 7U: _LL19: _tmp157=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f1;_tmp158=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp157;struct Cyc_List_List*fs=_tmp158;
# 822
s=({struct Cyc_PP_Doc*_tmp1AB[4U];({struct Cyc_PP_Doc*_tmp5E6=Cyc_Absynpp_aggr_kind2doc(k);_tmp1AB[0]=_tmp5E6;}),({struct Cyc_PP_Doc*_tmp5E5=Cyc_Absynpp_lb();_tmp1AB[1]=_tmp5E5;}),({
struct Cyc_PP_Doc*_tmp5E4=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_tmp1AB[2]=_tmp5E4;}),({
struct Cyc_PP_Doc*_tmp5E3=Cyc_Absynpp_rb();_tmp1AB[3]=_tmp5E3;});Cyc_PP_cat(_tag_fat(_tmp1AB,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 9U: _LL1F: _tmp156=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp14E)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp156;
# 829
s=({struct Cyc_PP_Doc*_tmp1B0[3U];({struct Cyc_PP_Doc*_tmp5E9=Cyc_PP_text(({const char*_tmp1B1="valueof_t(";_tag_fat(_tmp1B1,sizeof(char),11U);}));_tmp1B0[0]=_tmp5E9;}),({struct Cyc_PP_Doc*_tmp5E8=Cyc_Absynpp_exp2doc(e);_tmp1B0[1]=_tmp5E8;}),({struct Cyc_PP_Doc*_tmp5E7=Cyc_PP_text(({const char*_tmp1B2=")";_tag_fat(_tmp1B2,sizeof(char),2U);}));_tmp1B0[2]=_tmp5E7;});Cyc_PP_cat(_tag_fat(_tmp1B0,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 11U: _LL21: _tmp155=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp14E)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp155;
s=({struct Cyc_PP_Doc*_tmp1B3[3U];({struct Cyc_PP_Doc*_tmp5EC=Cyc_PP_text(({const char*_tmp1B4="typeof(";_tag_fat(_tmp1B4,sizeof(char),8U);}));_tmp1B3[0]=_tmp5EC;}),({struct Cyc_PP_Doc*_tmp5EB=Cyc_Absynpp_exp2doc(e);_tmp1B3[1]=_tmp5EB;}),({struct Cyc_PP_Doc*_tmp5EA=Cyc_PP_text(({const char*_tmp1B5=")";_tag_fat(_tmp1B5,sizeof(char),2U);}));_tmp1B3[2]=_tmp5EA;});Cyc_PP_cat(_tag_fat(_tmp1B3,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 8U: _LL25: _tmp152=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f1;_tmp153=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f2;_tmp154=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f3;_LL26: {struct _tuple0*n=_tmp152;struct Cyc_List_List*ts=_tmp153;struct Cyc_Absyn_Typedefdecl*kopt=_tmp154;
# 838
s=({struct Cyc_PP_Doc*_tmp1B6[2U];({struct Cyc_PP_Doc*_tmp5EE=Cyc_Absynpp_qvar2doc(n);_tmp1B6[0]=_tmp5EE;}),({struct Cyc_PP_Doc*_tmp5ED=Cyc_Absynpp_tps2doc(ts);_tmp1B6[1]=_tmp5ED;});Cyc_PP_cat(_tag_fat(_tmp1B6,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)){case 0U: _LL37: _tmp151=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL38: {struct Cyc_Absyn_Aggrdecl*d=_tmp151;
# 851
s=Cyc_Absynpp_aggrdecl2doc(d);goto _LL0;}case 1U: _LL39: _tmp150=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL3A: {struct Cyc_Absyn_Enumdecl*d=_tmp150;
s=Cyc_Absynpp_enumdecl2doc(d);goto _LL0;}default: _LL3B: _tmp14F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL3C: {struct Cyc_Absyn_Datatypedecl*d=_tmp14F;
s=Cyc_Absynpp_datatypedecl2doc(d);goto _LL0;}}}_LL0:;}
# 862
return s;}
# 865
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _fat_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple17{void*f1;void*f2;};
# 869
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple17*cmp){
struct _tuple17*_tmp1C6=cmp;void*_tmp1C8;void*_tmp1C7;_LL1: _tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C6->f2;_LL2: {void*r1=_tmp1C7;void*r2=_tmp1C8;
return({struct Cyc_PP_Doc*_tmp1C9[3U];({struct Cyc_PP_Doc*_tmp5F1=Cyc_Absynpp_rgn2doc(r1);_tmp1C9[0]=_tmp5F1;}),({struct Cyc_PP_Doc*_tmp5F0=Cyc_PP_text(({const char*_tmp1CA=" > ";_tag_fat(_tmp1CA,sizeof(char),4U);}));_tmp1C9[1]=_tmp5F0;}),({struct Cyc_PP_Doc*_tmp5EF=Cyc_Absynpp_rgn2doc(r2);_tmp1C9[2]=_tmp5EF;});Cyc_PP_cat(_tag_fat(_tmp1C9,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _tmp5F4=({const char*_tmp1CB="";_tag_fat(_tmp1CB,sizeof(char),1U);});struct _fat_ptr _tmp5F3=({const char*_tmp1CC="";_tag_fat(_tmp1CC,sizeof(char),1U);});struct _fat_ptr _tmp5F2=({const char*_tmp1CD=",";_tag_fat(_tmp1CD,sizeof(char),2U);});Cyc_PP_group(_tmp5F4,_tmp5F3,_tmp5F2,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 878
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*_tmp1CE=(*t).f1;struct _fat_ptr*vo=_tmp1CE;
struct Cyc_Core_Opt*dopt=vo == 0?0:({struct Cyc_Core_Opt*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({struct Cyc_PP_Doc*_tmp5F5=Cyc_PP_text(*vo);_tmp1CF->v=_tmp5F5;});_tmp1CF;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 884
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 888
struct Cyc_List_List*_tmp1D0=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_List_List*arg_docs=_tmp1D0;
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_tmp5F7=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5F7,({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));({struct Cyc_PP_Doc*_tmp5F6=Cyc_PP_text(({const char*_tmp1D1="...";_tag_fat(_tmp1D1,sizeof(char),4U);}));_tmp1D2->hd=_tmp5F6;}),_tmp1D2->tl=0;_tmp1D2;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1D3=({struct Cyc_PP_Doc*_tmp1D5[3U];({struct Cyc_PP_Doc*_tmp5FB=Cyc_PP_text(({const char*_tmp1D6="...";_tag_fat(_tmp1D6,sizeof(char),4U);}));_tmp1D5[0]=_tmp5FB;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp5FA=Cyc_PP_text(({const char*_tmp1D7=" inject ";_tag_fat(_tmp1D7,sizeof(char),9U);}));_tmp1D5[1]=_tmp5FA;}):({struct Cyc_PP_Doc*_tmp5F9=Cyc_PP_text(({const char*_tmp1D8=" ";_tag_fat(_tmp1D8,sizeof(char),2U);}));_tmp1D5[1]=_tmp5F9;}),({
struct Cyc_PP_Doc*_tmp5F8=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->f1=cyc_varargs->name,_tmp1D9->f2=cyc_varargs->tq,_tmp1D9->f3=cyc_varargs->type;_tmp1D9;}));_tmp1D5[2]=_tmp5F8;});Cyc_PP_cat(_tag_fat(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3U));});
# 893
struct Cyc_PP_Doc*varargs_doc=_tmp1D3;
# 897
arg_docs=({struct Cyc_List_List*_tmp5FC=arg_docs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5FC,({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=varargs_doc,_tmp1D4->tl=0;_tmp1D4;}));});}}{
# 899
struct Cyc_PP_Doc*_tmp1DA=({struct _fat_ptr _tmp5FF=({const char*_tmp1E9="";_tag_fat(_tmp1E9,sizeof(char),1U);});struct _fat_ptr _tmp5FE=({const char*_tmp1EA="";_tag_fat(_tmp1EA,sizeof(char),1U);});struct _fat_ptr _tmp5FD=({const char*_tmp1EB=",";_tag_fat(_tmp1EB,sizeof(char),2U);});Cyc_PP_group(_tmp5FF,_tmp5FE,_tmp5FD,arg_docs);});struct Cyc_PP_Doc*arg_doc=_tmp1DA;
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_tmp1DB[3U];_tmp1DB[0]=arg_doc,({struct Cyc_PP_Doc*_tmp601=Cyc_PP_text(({const char*_tmp1DC=";";_tag_fat(_tmp1DC,sizeof(char),2U);}));_tmp1DB[1]=_tmp601;}),({struct Cyc_PP_Doc*_tmp600=Cyc_Absynpp_eff2doc(effopt);_tmp1DB[2]=_tmp600;});Cyc_PP_cat(_tag_fat(_tmp1DB,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
arg_doc=({struct Cyc_PP_Doc*_tmp1DD[3U];_tmp1DD[0]=arg_doc,({struct Cyc_PP_Doc*_tmp603=Cyc_PP_text(({const char*_tmp1DE=":";_tag_fat(_tmp1DE,sizeof(char),2U);}));_tmp1DD[1]=_tmp603;}),({struct Cyc_PP_Doc*_tmp602=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1DD[2]=_tmp602;});Cyc_PP_cat(_tag_fat(_tmp1DD,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp1DF=({struct Cyc_PP_Doc*_tmp1E6[3U];({struct Cyc_PP_Doc*_tmp605=Cyc_PP_text(({const char*_tmp1E7="(";_tag_fat(_tmp1E7,sizeof(char),2U);}));_tmp1E6[0]=_tmp605;}),_tmp1E6[1]=arg_doc,({struct Cyc_PP_Doc*_tmp604=Cyc_PP_text(({const char*_tmp1E8=")";_tag_fat(_tmp1E8,sizeof(char),2U);}));_tmp1E6[2]=_tmp604;});Cyc_PP_cat(_tag_fat(_tmp1E6,sizeof(struct Cyc_PP_Doc*),3U));});struct Cyc_PP_Doc*res=_tmp1DF;
if(req != 0)
res=({struct Cyc_PP_Doc*_tmp1E0[4U];_tmp1E0[0]=res,({struct Cyc_PP_Doc*_tmp608=Cyc_PP_text(({const char*_tmp1E1=" @requires(";_tag_fat(_tmp1E1,sizeof(char),12U);}));_tmp1E0[1]=_tmp608;}),({struct Cyc_PP_Doc*_tmp607=Cyc_Absynpp_exp2doc(req);_tmp1E0[2]=_tmp607;}),({struct Cyc_PP_Doc*_tmp606=Cyc_PP_text(({const char*_tmp1E2=")";_tag_fat(_tmp1E2,sizeof(char),2U);}));_tmp1E0[3]=_tmp606;});Cyc_PP_cat(_tag_fat(_tmp1E0,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
res=({struct Cyc_PP_Doc*_tmp1E3[4U];_tmp1E3[0]=res,({struct Cyc_PP_Doc*_tmp60B=Cyc_PP_text(({const char*_tmp1E4=" @ensures(";_tag_fat(_tmp1E4,sizeof(char),11U);}));_tmp1E3[1]=_tmp60B;}),({struct Cyc_PP_Doc*_tmp60A=Cyc_Absynpp_exp2doc(ens);_tmp1E3[2]=_tmp60A;}),({struct Cyc_PP_Doc*_tmp609=Cyc_PP_text(({const char*_tmp1E5=")";_tag_fat(_tmp1E5,sizeof(char),2U);}));_tmp1E3[3]=_tmp609;});Cyc_PP_cat(_tag_fat(_tmp1E3,sizeof(struct Cyc_PP_Doc*),4U));});
return res;}}}
# 912
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _fat_ptr*v){return Cyc_PP_text(*v);}
# 914
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1EC=0;struct Cyc_List_List*prefix=_tmp1EC;
int match;
{union Cyc_Absyn_Nmspace _tmp1ED=(*q).f1;union Cyc_Absyn_Nmspace _stmttmp12=_tmp1ED;union Cyc_Absyn_Nmspace _tmp1EE=_stmttmp12;struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1F1;switch((_tmp1EE.C_n).tag){case 4U: _LL1: _LL2:
 _tmp1F1=0;goto _LL4;case 1U: _LL3: _tmp1F1=(_tmp1EE.Rel_n).val;_LL4: {struct Cyc_List_List*x=_tmp1F1;
# 920
match=0;
prefix=x;
goto _LL0;}case 3U: _LL5: _tmp1F0=(_tmp1EE.C_n).val;_LL6: {struct Cyc_List_List*x=_tmp1F0;
# 924
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
# 926
goto _LL0;}default: _LL7: _tmp1EF=(_tmp1EE.Abs_n).val;_LL8: {struct Cyc_List_List*x=_tmp1EF;
# 928
match=Cyc_Absynpp_use_curr_namespace &&((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
prefix=Cyc_Absynpp_qvar_to_Cids && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->hd=Cyc_Absynpp_cyc_stringptr,_tmp1F2->tl=x;_tmp1F2;}): x;
goto _LL0;}}_LL0:;}
# 932
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _fat_ptr)({struct Cyc_List_List*_tmp60D=({struct Cyc_List_List*_tmp60C=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp60C,({struct Cyc_List_List*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3->hd=(*q).f2,_tmp1F3->tl=0;_tmp1F3;}));});
# 933
Cyc_str_sepstr(_tmp60D,({const char*_tmp1F4="_";_tag_fat(_tmp1F4,sizeof(char),2U);}));});else{
# 937
if(match)
return*(*q).f2;else{
# 940
return(struct _fat_ptr)({struct Cyc_List_List*_tmp60F=({struct Cyc_List_List*_tmp60E=prefix;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp60E,({struct Cyc_List_List*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=(*q).f2,_tmp1F5->tl=0;_tmp1F5;}));});Cyc_str_sepstr(_tmp60F,({const char*_tmp1F6="::";_tag_fat(_tmp1F6,sizeof(char),3U);}));});}}}
# 944
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 948
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr _tmp1F7=Cyc_Absynpp_qvar2string(q);struct _fat_ptr qs=_tmp1F7;
if(Cyc_PP_tex_output)
# 952
return({struct _fat_ptr _tmp612=(struct _fat_ptr)({struct _fat_ptr _tmp611=(struct _fat_ptr)({struct _fat_ptr _tmp610=({const char*_tmp1F8="\\textbf{";_tag_fat(_tmp1F8,sizeof(char),9U);});Cyc_strconcat(_tmp610,(struct _fat_ptr)qs);});Cyc_strconcat(_tmp611,({const char*_tmp1F9="}";_tag_fat(_tmp1F9,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp612,(int)
Cyc_strlen((struct _fat_ptr)qs));});else{
return Cyc_PP_text(qs);}}
# 957
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 959
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 962
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1FA=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp13=_tmp1FA;union Cyc_Absyn_Nmspace _tmp1FB=_stmttmp13;struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FD;switch((_tmp1FB.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp1FB.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 973
 return(struct _fat_ptr)({struct _fat_ptr _tmp613=({const char*_tmp1FE="/* bad namespace : */ ";_tag_fat(_tmp1FE,sizeof(char),23U);});Cyc_strconcat(_tmp613,(struct _fat_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1FD=(_tmp1FB.C_n).val;_LL6: {struct Cyc_List_List*l=_tmp1FD;
# 966
_tmp1FC=l;goto _LL8;}default: _LL7: _tmp1FC=(_tmp1FB.Abs_n).val;_LL8: {struct Cyc_List_List*l=_tmp1FC;
# 968
if(((int(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,l,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 971
goto _LLA;}}}_LL0:;}else{
# 976
return*(*v).f2;}}
# 979
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 982
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _fat_ptr _tmp1FF=Cyc_Absynpp_typedef_name2string(v);struct _fat_ptr vs=_tmp1FF;
if(Cyc_PP_tex_output)
# 986
return({struct _fat_ptr _tmp616=(struct _fat_ptr)({struct _fat_ptr _tmp615=(struct _fat_ptr)({struct _fat_ptr _tmp614=({const char*_tmp200="\\textbf{";_tag_fat(_tmp200,sizeof(char),9U);});Cyc_strconcat(_tmp614,(struct _fat_ptr)vs);});Cyc_strconcat(_tmp615,({const char*_tmp201="}";_tag_fat(_tmp201,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp616,(int)
Cyc_strlen((struct _fat_ptr)vs));});else{
return Cyc_PP_text(vs);}}
# 991
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp617=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp617,t,0);});}
# 995
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_tmp202=f;unsigned _tmp203;struct _fat_ptr*_tmp204;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->tag == 0U){_LL1: _tmp204=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->f1;_LL2: {struct _fat_ptr*n=_tmp204;
return Cyc_PP_textptr(n);}}else{_LL3: _tmp203=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp202)->f1;_LL4: {unsigned n=_tmp203;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp207=({struct Cyc_Int_pa_PrintArg_struct _tmp547;_tmp547.tag=1U,_tmp547.f1=(unsigned long)((int)n);_tmp547;});void*_tmp205[1U];_tmp205[0]=& _tmp207;({struct _fat_ptr _tmp618=({const char*_tmp206="%d";_tag_fat(_tmp206,sizeof(char),3U);});Cyc_aprintf(_tmp618,_tag_fat(_tmp205,sizeof(void*),1U));});}));}}_LL0:;}
# 1009 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp208=e->r;void*_stmttmp14=_tmp208;void*_tmp209=_stmttmp14;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_Absyn_Exp*_tmp20B;enum Cyc_Absyn_Primop _tmp20C;switch(*((int*)_tmp209)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp20C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL6: {enum Cyc_Absyn_Primop p=_tmp20C;
# 1014
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
# 1036
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
# 1078
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1082
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp20E=e->r;void*_stmttmp15=_tmp20E;void*_tmp20F=_stmttmp15;struct Cyc_Absyn_Stmt*_tmp210;struct Cyc_List_List*_tmp212;struct Cyc_Core_Opt*_tmp211;struct Cyc_Absyn_Exp*_tmp214;struct Cyc_Absyn_Exp*_tmp213;int _tmp219;struct Cyc_Absyn_Exp*_tmp218;void**_tmp217;struct Cyc_Absyn_Exp*_tmp216;int _tmp215;struct Cyc_Absyn_Enumfield*_tmp21A;struct Cyc_Absyn_Enumfield*_tmp21B;struct Cyc_Absyn_Datatypefield*_tmp21D;struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmp221;struct Cyc_List_List*_tmp220;struct _tuple0*_tmp21F;void*_tmp223;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*_tmp225;struct Cyc_Absyn_Vardecl*_tmp224;struct Cyc_List_List*_tmp227;struct Cyc_List_List*_tmp229;struct _tuple8*_tmp228;struct Cyc_List_List*_tmp22A;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Exp*_tmp22B;struct _fat_ptr*_tmp22E;struct Cyc_Absyn_Exp*_tmp22D;struct _fat_ptr*_tmp230;struct Cyc_Absyn_Exp*_tmp22F;struct Cyc_Absyn_Exp*_tmp231;struct Cyc_List_List*_tmp233;void*_tmp232;struct _fat_ptr*_tmp235;struct Cyc_Absyn_Exp*_tmp234;struct _fat_ptr _tmp237;int _tmp236;void*_tmp238;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*_tmp23A;void*_tmp23B;struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_Exp*_tmp23C;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp240;void*_tmp23F;struct Cyc_Absyn_Exp*_tmp241;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Exp*_tmp243;struct Cyc_List_List*_tmp245;struct Cyc_Absyn_Exp*_tmp244;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Exp*_tmp249;struct Cyc_Absyn_Exp*_tmp248;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_Absyn_Exp*_tmp24C;enum Cyc_Absyn_Incrementor _tmp250;struct Cyc_Absyn_Exp*_tmp24F;struct Cyc_Absyn_Exp*_tmp253;struct Cyc_Core_Opt*_tmp252;struct Cyc_Absyn_Exp*_tmp251;struct Cyc_List_List*_tmp255;enum Cyc_Absyn_Primop _tmp254;struct _fat_ptr _tmp256;void*_tmp257;union Cyc_Absyn_Cnst _tmp258;switch(*((int*)_tmp20F)){case 0U: _LL1: _tmp258=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp258;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1U: _LL3: _tmp257=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL4: {void*b=_tmp257;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL5: _tmp256=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL6: {struct _fat_ptr p=_tmp256;
# 1089
s=({struct Cyc_PP_Doc*_tmp259[4U];({struct Cyc_PP_Doc*_tmp61C=Cyc_PP_text(({const char*_tmp25A="__cyclone_pragma__";_tag_fat(_tmp25A,sizeof(char),19U);}));_tmp259[0]=_tmp61C;}),({struct Cyc_PP_Doc*_tmp61B=Cyc_PP_text(({const char*_tmp25B="(";_tag_fat(_tmp25B,sizeof(char),2U);}));_tmp259[1]=_tmp61B;}),({struct Cyc_PP_Doc*_tmp61A=Cyc_PP_text(p);_tmp259[2]=_tmp61A;}),({struct Cyc_PP_Doc*_tmp619=Cyc_PP_text(({const char*_tmp25C=")";_tag_fat(_tmp25C,sizeof(char),2U);}));_tmp259[3]=_tmp619;});Cyc_PP_cat(_tag_fat(_tmp259,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 3U: _LL7: _tmp254=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp255=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp254;struct Cyc_List_List*es=_tmp255;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4U: _LL9: _tmp251=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp252=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_tmp253=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp20F)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp251;struct Cyc_Core_Opt*popt=_tmp252;struct Cyc_Absyn_Exp*e2=_tmp253;
# 1092
s=({struct Cyc_PP_Doc*_tmp25D[5U];({struct Cyc_PP_Doc*_tmp622=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp25D[0]=_tmp622;}),({
struct Cyc_PP_Doc*_tmp621=Cyc_PP_text(({const char*_tmp25E=" ";_tag_fat(_tmp25E,sizeof(char),2U);}));_tmp25D[1]=_tmp621;}),
popt == 0?({struct Cyc_PP_Doc*_tmp620=Cyc_PP_nil_doc();_tmp25D[2]=_tmp620;}):({struct Cyc_PP_Doc*_tmp61F=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_tmp25D[2]=_tmp61F;}),({
struct Cyc_PP_Doc*_tmp61E=Cyc_PP_text(({const char*_tmp25F="= ";_tag_fat(_tmp25F,sizeof(char),3U);}));_tmp25D[3]=_tmp61E;}),({
struct Cyc_PP_Doc*_tmp61D=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp25D[4]=_tmp61D;});Cyc_PP_cat(_tag_fat(_tmp25D,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp24F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp250=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LLC: {struct Cyc_Absyn_Exp*e2=_tmp24F;enum Cyc_Absyn_Incrementor i=_tmp250;
# 1099
struct Cyc_PP_Doc*_tmp260=Cyc_Absynpp_exp2doc_prec(myprec,e2);struct Cyc_PP_Doc*es=_tmp260;
{enum Cyc_Absyn_Incrementor _tmp261=i;switch(_tmp261){case Cyc_Absyn_PreInc: _LL56: _LL57:
 s=({struct Cyc_PP_Doc*_tmp262[2U];({struct Cyc_PP_Doc*_tmp623=Cyc_PP_text(({const char*_tmp263="++";_tag_fat(_tmp263,sizeof(char),3U);}));_tmp262[0]=_tmp623;}),_tmp262[1]=es;Cyc_PP_cat(_tag_fat(_tmp262,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PreDec: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp264[2U];({struct Cyc_PP_Doc*_tmp624=Cyc_PP_text(({const char*_tmp265="--";_tag_fat(_tmp265,sizeof(char),3U);}));_tmp264[0]=_tmp624;}),_tmp264[1]=es;Cyc_PP_cat(_tag_fat(_tmp264,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostInc: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp266[2U];_tmp266[0]=es,({struct Cyc_PP_Doc*_tmp625=Cyc_PP_text(({const char*_tmp267="++";_tag_fat(_tmp267,sizeof(char),3U);}));_tmp266[1]=_tmp625;});Cyc_PP_cat(_tag_fat(_tmp266,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostDec: _LL5C: _LL5D:
 goto _LL5F;default: _LL5E: _LL5F:
 s=({struct Cyc_PP_Doc*_tmp268[2U];_tmp268[0]=es,({struct Cyc_PP_Doc*_tmp626=Cyc_PP_text(({const char*_tmp269="--";_tag_fat(_tmp269,sizeof(char),3U);}));_tmp268[1]=_tmp626;});Cyc_PP_cat(_tag_fat(_tmp268,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;}_LL55:;}
# 1107
goto _LL0;}case 6U: _LLD: _tmp24C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp24D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_tmp24E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp20F)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp24C;struct Cyc_Absyn_Exp*e2=_tmp24D;struct Cyc_Absyn_Exp*e3=_tmp24E;
# 1109
s=({struct Cyc_PP_Doc*_tmp26A[5U];({struct Cyc_PP_Doc*_tmp62B=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp26A[0]=_tmp62B;}),({struct Cyc_PP_Doc*_tmp62A=Cyc_PP_text(({const char*_tmp26B=" ? ";_tag_fat(_tmp26B,sizeof(char),4U);}));_tmp26A[1]=_tmp62A;}),({struct Cyc_PP_Doc*_tmp629=Cyc_Absynpp_exp2doc_prec(0,e2);_tmp26A[2]=_tmp629;}),({
struct Cyc_PP_Doc*_tmp628=Cyc_PP_text(({const char*_tmp26C=" : ";_tag_fat(_tmp26C,sizeof(char),4U);}));_tmp26A[3]=_tmp628;}),({struct Cyc_PP_Doc*_tmp627=Cyc_Absynpp_exp2doc_prec(myprec,e3);_tmp26A[4]=_tmp627;});Cyc_PP_cat(_tag_fat(_tmp26A,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 7U: _LLF: _tmp24A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp24B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp24A;struct Cyc_Absyn_Exp*e2=_tmp24B;
# 1113
s=({struct Cyc_PP_Doc*_tmp26D[3U];({struct Cyc_PP_Doc*_tmp62E=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp26D[0]=_tmp62E;}),({struct Cyc_PP_Doc*_tmp62D=Cyc_PP_text(({const char*_tmp26E=" && ";_tag_fat(_tmp26E,sizeof(char),5U);}));_tmp26D[1]=_tmp62D;}),({struct Cyc_PP_Doc*_tmp62C=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp26D[2]=_tmp62C;});Cyc_PP_cat(_tag_fat(_tmp26D,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 8U: _LL11: _tmp248=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp249=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp248;struct Cyc_Absyn_Exp*e2=_tmp249;
# 1116
s=({struct Cyc_PP_Doc*_tmp26F[3U];({struct Cyc_PP_Doc*_tmp631=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp26F[0]=_tmp631;}),({struct Cyc_PP_Doc*_tmp630=Cyc_PP_text(({const char*_tmp270=" || ";_tag_fat(_tmp270,sizeof(char),5U);}));_tmp26F[1]=_tmp630;}),({struct Cyc_PP_Doc*_tmp62F=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp26F[2]=_tmp62F;});Cyc_PP_cat(_tag_fat(_tmp26F,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 9U: _LL13: _tmp246=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp247=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp246;struct Cyc_Absyn_Exp*e2=_tmp247;
# 1121
s=({struct Cyc_PP_Doc*_tmp271[3U];({struct Cyc_PP_Doc*_tmp634=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_tmp271[0]=_tmp634;}),({struct Cyc_PP_Doc*_tmp633=Cyc_PP_text(({const char*_tmp272=", ";_tag_fat(_tmp272,sizeof(char),3U);}));_tmp271[1]=_tmp633;}),({struct Cyc_PP_Doc*_tmp632=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_tmp271[2]=_tmp632;});Cyc_PP_cat(_tag_fat(_tmp271,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 10U: _LL15: _tmp244=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp245=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp244;struct Cyc_List_List*es=_tmp245;
# 1124
s=({struct Cyc_PP_Doc*_tmp273[4U];({struct Cyc_PP_Doc*_tmp638=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp273[0]=_tmp638;}),({
struct Cyc_PP_Doc*_tmp637=Cyc_PP_text(({const char*_tmp274="(";_tag_fat(_tmp274,sizeof(char),2U);}));_tmp273[1]=_tmp637;}),({
struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_exps2doc_prec(20,es);_tmp273[2]=_tmp636;}),({
struct Cyc_PP_Doc*_tmp635=Cyc_PP_text(({const char*_tmp275=")";_tag_fat(_tmp275,sizeof(char),2U);}));_tmp273[3]=_tmp635;});Cyc_PP_cat(_tag_fat(_tmp273,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 11U: _LL17: _tmp243=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp243;
s=({struct Cyc_PP_Doc*_tmp276[2U];({struct Cyc_PP_Doc*_tmp63A=Cyc_PP_text(({const char*_tmp277="throw ";_tag_fat(_tmp277,sizeof(char),7U);}));_tmp276[0]=_tmp63A;}),({struct Cyc_PP_Doc*_tmp639=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp276[1]=_tmp639;});Cyc_PP_cat(_tag_fat(_tmp276,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 12U: _LL19: _tmp242=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp242;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13U: _LL1B: _tmp241=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp241;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14U: _LL1D: _tmp23F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp240=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL1E: {void*t=_tmp23F;struct Cyc_Absyn_Exp*e1=_tmp240;
# 1133
s=({struct Cyc_PP_Doc*_tmp278[4U];({struct Cyc_PP_Doc*_tmp63E=Cyc_PP_text(({const char*_tmp279="(";_tag_fat(_tmp279,sizeof(char),2U);}));_tmp278[0]=_tmp63E;}),({struct Cyc_PP_Doc*_tmp63D=Cyc_Absynpp_typ2doc(t);_tmp278[1]=_tmp63D;}),({struct Cyc_PP_Doc*_tmp63C=Cyc_PP_text(({const char*_tmp27A=")";_tag_fat(_tmp27A,sizeof(char),2U);}));_tmp278[2]=_tmp63C;}),({struct Cyc_PP_Doc*_tmp63B=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp278[3]=_tmp63B;});Cyc_PP_cat(_tag_fat(_tmp278,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 15U: _LL1F: _tmp23E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp23E;
s=({struct Cyc_PP_Doc*_tmp27B[2U];({struct Cyc_PP_Doc*_tmp640=Cyc_PP_text(({const char*_tmp27C="&";_tag_fat(_tmp27C,sizeof(char),2U);}));_tmp27B[0]=_tmp640;}),({struct Cyc_PP_Doc*_tmp63F=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp27B[1]=_tmp63F;});Cyc_PP_cat(_tag_fat(_tmp27B,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 16U: _LL21: _tmp23C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp23D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL22: {struct Cyc_Absyn_Exp*ropt=_tmp23C;struct Cyc_Absyn_Exp*e1=_tmp23D;
# 1136
if(ropt == 0)
s=({struct Cyc_PP_Doc*_tmp27D[2U];({struct Cyc_PP_Doc*_tmp642=Cyc_PP_text(({const char*_tmp27E="new ";_tag_fat(_tmp27E,sizeof(char),5U);}));_tmp27D[0]=_tmp642;}),({struct Cyc_PP_Doc*_tmp641=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp27D[1]=_tmp641;});Cyc_PP_cat(_tag_fat(_tmp27D,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1139
s=({struct Cyc_PP_Doc*_tmp27F[4U];({struct Cyc_PP_Doc*_tmp646=Cyc_PP_text(({const char*_tmp280="rnew(";_tag_fat(_tmp280,sizeof(char),6U);}));_tmp27F[0]=_tmp646;}),({struct Cyc_PP_Doc*_tmp645=Cyc_Absynpp_exp2doc(ropt);_tmp27F[1]=_tmp645;}),({struct Cyc_PP_Doc*_tmp644=Cyc_PP_text(({const char*_tmp281=") ";_tag_fat(_tmp281,sizeof(char),3U);}));_tmp27F[2]=_tmp644;}),({struct Cyc_PP_Doc*_tmp643=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp27F[3]=_tmp643;});Cyc_PP_cat(_tag_fat(_tmp27F,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;}case 17U: _LL23: _tmp23B=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL24: {void*t=_tmp23B;
s=({struct Cyc_PP_Doc*_tmp282[3U];({struct Cyc_PP_Doc*_tmp649=Cyc_PP_text(({const char*_tmp283="sizeof(";_tag_fat(_tmp283,sizeof(char),8U);}));_tmp282[0]=_tmp649;}),({struct Cyc_PP_Doc*_tmp648=Cyc_Absynpp_typ2doc(t);_tmp282[1]=_tmp648;}),({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp284=")";_tag_fat(_tmp284,sizeof(char),2U);}));_tmp282[2]=_tmp647;});Cyc_PP_cat(_tag_fat(_tmp282,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 18U: _LL25: _tmp23A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp23A;
s=({struct Cyc_PP_Doc*_tmp285[3U];({struct Cyc_PP_Doc*_tmp64C=Cyc_PP_text(({const char*_tmp286="sizeof(";_tag_fat(_tmp286,sizeof(char),8U);}));_tmp285[0]=_tmp64C;}),({struct Cyc_PP_Doc*_tmp64B=Cyc_Absynpp_exp2doc(e1);_tmp285[1]=_tmp64B;}),({struct Cyc_PP_Doc*_tmp64A=Cyc_PP_text(({const char*_tmp287=")";_tag_fat(_tmp287,sizeof(char),2U);}));_tmp285[2]=_tmp64A;});Cyc_PP_cat(_tag_fat(_tmp285,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 41U: _LL27: _tmp239=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp239;
# 1144
s=({struct Cyc_PP_Doc*_tmp288[3U];({struct Cyc_PP_Doc*_tmp64F=Cyc_PP_text(({const char*_tmp289="__extension__(";_tag_fat(_tmp289,sizeof(char),15U);}));_tmp288[0]=_tmp64F;}),({struct Cyc_PP_Doc*_tmp64E=Cyc_Absynpp_exp2doc(e);_tmp288[1]=_tmp64E;}),({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_text(({const char*_tmp28A=")";_tag_fat(_tmp28A,sizeof(char),2U);}));_tmp288[2]=_tmp64D;});Cyc_PP_cat(_tag_fat(_tmp288,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 39U: _LL29: _tmp238=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL2A: {void*t=_tmp238;
s=({struct Cyc_PP_Doc*_tmp28B[3U];({struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp28C="valueof(";_tag_fat(_tmp28C,sizeof(char),9U);}));_tmp28B[0]=_tmp652;}),({struct Cyc_PP_Doc*_tmp651=Cyc_Absynpp_typ2doc(t);_tmp28B[1]=_tmp651;}),({struct Cyc_PP_Doc*_tmp650=Cyc_PP_text(({const char*_tmp28D=")";_tag_fat(_tmp28D,sizeof(char),2U);}));_tmp28B[2]=_tmp650;});Cyc_PP_cat(_tag_fat(_tmp28B,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 40U: _LL2B: _tmp236=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp237=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL2C: {int vol=_tmp236;struct _fat_ptr body=_tmp237;
# 1147
if(vol)
s=({struct Cyc_PP_Doc*_tmp28E[4U];({struct Cyc_PP_Doc*_tmp656=Cyc_PP_text(({const char*_tmp28F="__asm__";_tag_fat(_tmp28F,sizeof(char),8U);}));_tmp28E[0]=_tmp656;}),({struct Cyc_PP_Doc*_tmp655=Cyc_PP_text(({const char*_tmp290=" volatile (";_tag_fat(_tmp290,sizeof(char),12U);}));_tmp28E[1]=_tmp655;}),({struct Cyc_PP_Doc*_tmp654=Cyc_PP_text(body);_tmp28E[2]=_tmp654;}),({struct Cyc_PP_Doc*_tmp653=Cyc_PP_text(({const char*_tmp291=")";_tag_fat(_tmp291,sizeof(char),2U);}));_tmp28E[3]=_tmp653;});Cyc_PP_cat(_tag_fat(_tmp28E,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1150
s=({struct Cyc_PP_Doc*_tmp292[3U];({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp293="__asm__(";_tag_fat(_tmp293,sizeof(char),9U);}));_tmp292[0]=_tmp659;}),({struct Cyc_PP_Doc*_tmp658=Cyc_PP_text(body);_tmp292[1]=_tmp658;}),({struct Cyc_PP_Doc*_tmp657=Cyc_PP_text(({const char*_tmp294=")";_tag_fat(_tmp294,sizeof(char),2U);}));_tmp292[2]=_tmp657;});Cyc_PP_cat(_tag_fat(_tmp292,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 38U: _LL2D: _tmp234=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp235=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp234;struct _fat_ptr*f=_tmp235;
# 1153
s=({struct Cyc_PP_Doc*_tmp295[5U];({struct Cyc_PP_Doc*_tmp65E=Cyc_PP_text(({const char*_tmp296="tagcheck(";_tag_fat(_tmp296,sizeof(char),10U);}));_tmp295[0]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_exp2doc(e);_tmp295[1]=_tmp65D;}),({struct Cyc_PP_Doc*_tmp65C=Cyc_PP_text(({const char*_tmp297=".";_tag_fat(_tmp297,sizeof(char),2U);}));_tmp295[2]=_tmp65C;}),({struct Cyc_PP_Doc*_tmp65B=Cyc_PP_textptr(f);_tmp295[3]=_tmp65B;}),({struct Cyc_PP_Doc*_tmp65A=Cyc_PP_text(({const char*_tmp298=")";_tag_fat(_tmp298,sizeof(char),2U);}));_tmp295[4]=_tmp65A;});Cyc_PP_cat(_tag_fat(_tmp295,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 19U: _LL2F: _tmp232=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp233=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL30: {void*t=_tmp232;struct Cyc_List_List*l=_tmp233;
# 1156
s=({struct Cyc_PP_Doc*_tmp299[5U];({struct Cyc_PP_Doc*_tmp664=Cyc_PP_text(({const char*_tmp29A="offsetof(";_tag_fat(_tmp29A,sizeof(char),10U);}));_tmp299[0]=_tmp664;}),({struct Cyc_PP_Doc*_tmp663=Cyc_Absynpp_typ2doc(t);_tmp299[1]=_tmp663;}),({struct Cyc_PP_Doc*_tmp662=Cyc_PP_text(({const char*_tmp29B=",";_tag_fat(_tmp29B,sizeof(char),2U);}));_tmp299[2]=_tmp662;}),({
struct Cyc_PP_Doc*_tmp661=({struct _fat_ptr _tmp660=({const char*_tmp29C=".";_tag_fat(_tmp29C,sizeof(char),2U);});Cyc_PP_seq(_tmp660,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_offsetof_field_to_doc,l));});_tmp299[3]=_tmp661;}),({struct Cyc_PP_Doc*_tmp65F=Cyc_PP_text(({const char*_tmp29D=")";_tag_fat(_tmp29D,sizeof(char),2U);}));_tmp299[4]=_tmp65F;});Cyc_PP_cat(_tag_fat(_tmp299,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 20U: _LL31: _tmp231=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp231;
s=({struct Cyc_PP_Doc*_tmp29E[2U];({struct Cyc_PP_Doc*_tmp666=Cyc_PP_text(({const char*_tmp29F="*";_tag_fat(_tmp29F,sizeof(char),2U);}));_tmp29E[0]=_tmp666;}),({struct Cyc_PP_Doc*_tmp665=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp29E[1]=_tmp665;});Cyc_PP_cat(_tag_fat(_tmp29E,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 21U: _LL33: _tmp22F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp230=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp22F;struct _fat_ptr*n=_tmp230;
# 1161
s=({struct Cyc_PP_Doc*_tmp2A0[3U];({struct Cyc_PP_Doc*_tmp669=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2A0[0]=_tmp669;}),({struct Cyc_PP_Doc*_tmp668=Cyc_PP_text(({const char*_tmp2A1=".";_tag_fat(_tmp2A1,sizeof(char),2U);}));_tmp2A0[1]=_tmp668;}),({struct Cyc_PP_Doc*_tmp667=Cyc_PP_textptr(n);_tmp2A0[2]=_tmp667;});Cyc_PP_cat(_tag_fat(_tmp2A0,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 22U: _LL35: _tmp22D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp22E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp22D;struct _fat_ptr*n=_tmp22E;
# 1163
s=({struct Cyc_PP_Doc*_tmp2A2[3U];({struct Cyc_PP_Doc*_tmp66C=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2A2[0]=_tmp66C;}),({struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp2A3="->";_tag_fat(_tmp2A3,sizeof(char),3U);}));_tmp2A2[1]=_tmp66B;}),({struct Cyc_PP_Doc*_tmp66A=Cyc_PP_textptr(n);_tmp2A2[2]=_tmp66A;});Cyc_PP_cat(_tag_fat(_tmp2A2,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 23U: _LL37: _tmp22B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp22C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp22B;struct Cyc_Absyn_Exp*e2=_tmp22C;
# 1165
s=({struct Cyc_PP_Doc*_tmp2A4[4U];({struct Cyc_PP_Doc*_tmp670=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2A4[0]=_tmp670;}),({struct Cyc_PP_Doc*_tmp66F=Cyc_PP_text(({const char*_tmp2A5="[";_tag_fat(_tmp2A5,sizeof(char),2U);}));_tmp2A4[1]=_tmp66F;}),({struct Cyc_PP_Doc*_tmp66E=Cyc_Absynpp_exp2doc(e2);_tmp2A4[2]=_tmp66E;}),({struct Cyc_PP_Doc*_tmp66D=Cyc_PP_text(({const char*_tmp2A6="]";_tag_fat(_tmp2A6,sizeof(char),2U);}));_tmp2A4[3]=_tmp66D;});Cyc_PP_cat(_tag_fat(_tmp2A4,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 24U: _LL39: _tmp22A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL3A: {struct Cyc_List_List*es=_tmp22A;
# 1167
s=({struct Cyc_PP_Doc*_tmp2A7[4U];({struct Cyc_PP_Doc*_tmp674=Cyc_Absynpp_dollar();_tmp2A7[0]=_tmp674;}),({struct Cyc_PP_Doc*_tmp673=Cyc_PP_text(({const char*_tmp2A8="(";_tag_fat(_tmp2A8,sizeof(char),2U);}));_tmp2A7[1]=_tmp673;}),({struct Cyc_PP_Doc*_tmp672=Cyc_Absynpp_exps2doc_prec(20,es);_tmp2A7[2]=_tmp672;}),({struct Cyc_PP_Doc*_tmp671=Cyc_PP_text(({const char*_tmp2A9=")";_tag_fat(_tmp2A9,sizeof(char),2U);}));_tmp2A7[3]=_tmp671;});Cyc_PP_cat(_tag_fat(_tmp2A7,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}case 25U: _LL3B: _tmp228=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp229=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL3C: {struct _tuple8*vat=_tmp228;struct Cyc_List_List*des=_tmp229;
# 1169
s=({struct Cyc_PP_Doc*_tmp2AA[4U];({struct Cyc_PP_Doc*_tmp679=Cyc_PP_text(({const char*_tmp2AB="(";_tag_fat(_tmp2AB,sizeof(char),2U);}));_tmp2AA[0]=_tmp679;}),({struct Cyc_PP_Doc*_tmp678=Cyc_Absynpp_typ2doc((*vat).f3);_tmp2AA[1]=_tmp678;}),({struct Cyc_PP_Doc*_tmp677=Cyc_PP_text(({const char*_tmp2AC=")";_tag_fat(_tmp2AC,sizeof(char),2U);}));_tmp2AA[2]=_tmp677;}),({
struct Cyc_PP_Doc*_tmp676=({struct _fat_ptr _tmp675=({const char*_tmp2AD=",";_tag_fat(_tmp2AD,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp675,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});_tmp2AA[3]=_tmp676;});Cyc_PP_cat(_tag_fat(_tmp2AA,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 26U: _LL3D: _tmp227=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL3E: {struct Cyc_List_List*des=_tmp227;
s=({struct _fat_ptr _tmp67A=({const char*_tmp2AE=",";_tag_fat(_tmp2AE,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp67A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 27U: _LL3F: _tmp224=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp225=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_tmp226=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp20F)->f3;_LL40: {struct Cyc_Absyn_Vardecl*vd=_tmp224;struct Cyc_Absyn_Exp*e1=_tmp225;struct Cyc_Absyn_Exp*e2=_tmp226;
# 1174
s=({struct Cyc_PP_Doc*_tmp2AF[8U];({struct Cyc_PP_Doc*_tmp682=Cyc_Absynpp_lb();_tmp2AF[0]=_tmp682;}),({struct Cyc_PP_Doc*_tmp681=Cyc_PP_text(({const char*_tmp2B0="for ";_tag_fat(_tmp2B0,sizeof(char),5U);}));_tmp2AF[1]=_tmp681;}),({
struct Cyc_PP_Doc*_tmp680=Cyc_PP_text(*(*vd->name).f2);_tmp2AF[2]=_tmp680;}),({struct Cyc_PP_Doc*_tmp67F=Cyc_PP_text(({const char*_tmp2B1=" < ";_tag_fat(_tmp2B1,sizeof(char),4U);}));_tmp2AF[3]=_tmp67F;}),({struct Cyc_PP_Doc*_tmp67E=Cyc_Absynpp_exp2doc(e1);_tmp2AF[4]=_tmp67E;}),({struct Cyc_PP_Doc*_tmp67D=Cyc_PP_text(({const char*_tmp2B2=" : ";_tag_fat(_tmp2B2,sizeof(char),4U);}));_tmp2AF[5]=_tmp67D;}),({
struct Cyc_PP_Doc*_tmp67C=Cyc_Absynpp_exp2doc(e2);_tmp2AF[6]=_tmp67C;}),({struct Cyc_PP_Doc*_tmp67B=Cyc_Absynpp_rb();_tmp2AF[7]=_tmp67B;});Cyc_PP_cat(_tag_fat(_tmp2AF,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 28U: _LL41: _tmp222=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp223=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL42: {struct Cyc_Absyn_Exp*e=_tmp222;void*t=_tmp223;
# 1179
s=({struct Cyc_PP_Doc*_tmp2B3[7U];({struct Cyc_PP_Doc*_tmp689=Cyc_Absynpp_lb();_tmp2B3[0]=_tmp689;}),({struct Cyc_PP_Doc*_tmp688=Cyc_PP_text(({const char*_tmp2B4="for x ";_tag_fat(_tmp2B4,sizeof(char),7U);}));_tmp2B3[1]=_tmp688;}),({
struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp2B5=" < ";_tag_fat(_tmp2B5,sizeof(char),4U);}));_tmp2B3[2]=_tmp687;}),({
struct Cyc_PP_Doc*_tmp686=Cyc_Absynpp_exp2doc(e);_tmp2B3[3]=_tmp686;}),({
struct Cyc_PP_Doc*_tmp685=Cyc_PP_text(({const char*_tmp2B6=" : ";_tag_fat(_tmp2B6,sizeof(char),4U);}));_tmp2B3[4]=_tmp685;}),({
struct Cyc_PP_Doc*_tmp684=Cyc_Absynpp_typ2doc(t);_tmp2B3[5]=_tmp684;}),({
struct Cyc_PP_Doc*_tmp683=Cyc_Absynpp_rb();_tmp2B3[6]=_tmp683;});Cyc_PP_cat(_tag_fat(_tmp2B3,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 29U: _LL43: _tmp21F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp220=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_tmp221=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp20F)->f3;_LL44: {struct _tuple0*n=_tmp21F;struct Cyc_List_List*ts=_tmp220;struct Cyc_List_List*des=_tmp221;
# 1187
struct Cyc_List_List*_tmp2B7=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des);struct Cyc_List_List*des_doc=_tmp2B7;
s=({struct Cyc_PP_Doc*_tmp2B8[2U];({struct Cyc_PP_Doc*_tmp68D=Cyc_Absynpp_qvar2doc(n);_tmp2B8[0]=_tmp68D;}),({
struct Cyc_PP_Doc*_tmp68C=({struct _fat_ptr _tmp68B=({const char*_tmp2B9=",";_tag_fat(_tmp2B9,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp68B,
ts != 0?({struct Cyc_List_List*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));({struct Cyc_PP_Doc*_tmp68A=Cyc_Absynpp_tps2doc(ts);_tmp2BA->hd=_tmp68A;}),_tmp2BA->tl=des_doc;_tmp2BA;}): des_doc);});
# 1189
_tmp2B8[1]=_tmp68C;});Cyc_PP_cat(_tag_fat(_tmp2B8,sizeof(struct Cyc_PP_Doc*),2U));});
# 1191
goto _LL0;}case 30U: _LL45: _tmp21E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL46: {struct Cyc_List_List*des=_tmp21E;
s=({struct _fat_ptr _tmp68E=({const char*_tmp2BB=",";_tag_fat(_tmp2BB,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp68E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 31U: _LL47: _tmp21C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp21D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp20F)->f3;_LL48: {struct Cyc_List_List*es=_tmp21C;struct Cyc_Absyn_Datatypefield*tuf=_tmp21D;
# 1194
if(es == 0)
# 1196
s=Cyc_Absynpp_qvar2doc(tuf->name);else{
# 1198
s=({struct Cyc_PP_Doc*_tmp2BC[2U];({struct Cyc_PP_Doc*_tmp693=Cyc_Absynpp_qvar2doc(tuf->name);_tmp2BC[0]=_tmp693;}),({
struct Cyc_PP_Doc*_tmp692=({struct _fat_ptr _tmp691=({const char*_tmp2BD="(";_tag_fat(_tmp2BD,sizeof(char),2U);});struct _fat_ptr _tmp690=({const char*_tmp2BE=")";_tag_fat(_tmp2BE,sizeof(char),2U);});struct _fat_ptr _tmp68F=({const char*_tmp2BF=",";_tag_fat(_tmp2BF,sizeof(char),2U);});Cyc_PP_egroup(_tmp691,_tmp690,_tmp68F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,es));});_tmp2BC[1]=_tmp692;});Cyc_PP_cat(_tag_fat(_tmp2BC,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;}case 32U: _LL49: _tmp21B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL4A: {struct Cyc_Absyn_Enumfield*ef=_tmp21B;
_tmp21A=ef;goto _LL4C;}case 33U: _LL4B: _tmp21A=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL4C: {struct Cyc_Absyn_Enumfield*ef=_tmp21A;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 34U: _LL4D: _tmp215=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp20F)->f1).is_calloc;_tmp216=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp20F)->f1).rgn;_tmp217=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp20F)->f1).elt_type;_tmp218=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp20F)->f1).num_elts;_tmp219=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp20F)->f1).inline_call;_LL4E: {int is_calloc=_tmp215;struct Cyc_Absyn_Exp*rgnopt=_tmp216;void**topt=_tmp217;struct Cyc_Absyn_Exp*e=_tmp218;int inline_call=_tmp219;
# 1206
if(is_calloc){
# 1208
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U);
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp2C0[5U];({struct Cyc_PP_Doc*_tmp698=Cyc_PP_text(({const char*_tmp2C1="calloc(";_tag_fat(_tmp2C1,sizeof(char),8U);}));_tmp2C0[0]=_tmp698;}),({struct Cyc_PP_Doc*_tmp697=Cyc_Absynpp_exp2doc(e);_tmp2C0[1]=_tmp697;}),({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp2C2=",";_tag_fat(_tmp2C2,sizeof(char),2U);}));_tmp2C0[2]=_tmp696;}),({struct Cyc_PP_Doc*_tmp695=Cyc_Absynpp_exp2doc(st);_tmp2C0[3]=_tmp695;}),({struct Cyc_PP_Doc*_tmp694=Cyc_PP_text(({const char*_tmp2C3=")";_tag_fat(_tmp2C3,sizeof(char),2U);}));_tmp2C0[4]=_tmp694;});Cyc_PP_cat(_tag_fat(_tmp2C0,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1212
s=({struct Cyc_PP_Doc*_tmp2C4[7U];({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_text(({const char*_tmp2C5="rcalloc(";_tag_fat(_tmp2C5,sizeof(char),9U);}));_tmp2C4[0]=_tmp69F;}),({struct Cyc_PP_Doc*_tmp69E=Cyc_Absynpp_exp2doc(rgnopt);_tmp2C4[1]=_tmp69E;}),({struct Cyc_PP_Doc*_tmp69D=Cyc_PP_text(({const char*_tmp2C6=",";_tag_fat(_tmp2C6,sizeof(char),2U);}));_tmp2C4[2]=_tmp69D;}),({
struct Cyc_PP_Doc*_tmp69C=Cyc_Absynpp_exp2doc(e);_tmp2C4[3]=_tmp69C;}),({struct Cyc_PP_Doc*_tmp69B=Cyc_PP_text(({const char*_tmp2C7=",";_tag_fat(_tmp2C7,sizeof(char),2U);}));_tmp2C4[4]=_tmp69B;}),({struct Cyc_PP_Doc*_tmp69A=Cyc_Absynpp_exp2doc(st);_tmp2C4[5]=_tmp69A;}),({struct Cyc_PP_Doc*_tmp699=Cyc_PP_text(({const char*_tmp2C8=")";_tag_fat(_tmp2C8,sizeof(char),2U);}));_tmp2C4[6]=_tmp699;});Cyc_PP_cat(_tag_fat(_tmp2C4,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1215
struct Cyc_Absyn_Exp*new_e;
# 1217
if(topt == 0)
new_e=e;else{
# 1220
new_e=({struct Cyc_Absyn_Exp*_tmp6A0=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp6A0,e,0U);});}
# 1222
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp2C9[3U];({struct Cyc_PP_Doc*_tmp6A3=Cyc_PP_text(({const char*_tmp2CA="malloc(";_tag_fat(_tmp2CA,sizeof(char),8U);}));_tmp2C9[0]=_tmp6A3;}),({struct Cyc_PP_Doc*_tmp6A2=Cyc_Absynpp_exp2doc(new_e);_tmp2C9[1]=_tmp6A2;}),({struct Cyc_PP_Doc*_tmp6A1=Cyc_PP_text(({const char*_tmp2CB=")";_tag_fat(_tmp2CB,sizeof(char),2U);}));_tmp2C9[2]=_tmp6A1;});Cyc_PP_cat(_tag_fat(_tmp2C9,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1225
if(inline_call)
s=({struct Cyc_PP_Doc*_tmp2CC[5U];({struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_text(({const char*_tmp2CD="rmalloc_inline(";_tag_fat(_tmp2CD,sizeof(char),16U);}));_tmp2CC[0]=_tmp6A8;}),({struct Cyc_PP_Doc*_tmp6A7=Cyc_Absynpp_exp2doc(rgnopt);_tmp2CC[1]=_tmp6A7;}),({struct Cyc_PP_Doc*_tmp6A6=Cyc_PP_text(({const char*_tmp2CE=",";_tag_fat(_tmp2CE,sizeof(char),2U);}));_tmp2CC[2]=_tmp6A6;}),({
struct Cyc_PP_Doc*_tmp6A5=Cyc_Absynpp_exp2doc(new_e);_tmp2CC[3]=_tmp6A5;}),({struct Cyc_PP_Doc*_tmp6A4=Cyc_PP_text(({const char*_tmp2CF=")";_tag_fat(_tmp2CF,sizeof(char),2U);}));_tmp2CC[4]=_tmp6A4;});Cyc_PP_cat(_tag_fat(_tmp2CC,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1229
s=({struct Cyc_PP_Doc*_tmp2D0[5U];({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp2D1="rmalloc(";_tag_fat(_tmp2D1,sizeof(char),9U);}));_tmp2D0[0]=_tmp6AD;}),({struct Cyc_PP_Doc*_tmp6AC=Cyc_Absynpp_exp2doc(rgnopt);_tmp2D0[1]=_tmp6AC;}),({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_text(({const char*_tmp2D2=",";_tag_fat(_tmp2D2,sizeof(char),2U);}));_tmp2D0[2]=_tmp6AB;}),({
struct Cyc_PP_Doc*_tmp6AA=Cyc_Absynpp_exp2doc(new_e);_tmp2D0[3]=_tmp6AA;}),({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_text(({const char*_tmp2D3=")";_tag_fat(_tmp2D3,sizeof(char),2U);}));_tmp2D0[4]=_tmp6A9;});Cyc_PP_cat(_tag_fat(_tmp2D0,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1233
goto _LL0;}case 35U: _LL4F: _tmp213=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp214=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL50: {struct Cyc_Absyn_Exp*e1=_tmp213;struct Cyc_Absyn_Exp*e2=_tmp214;
# 1235
s=({struct Cyc_PP_Doc*_tmp2D4[3U];({struct Cyc_PP_Doc*_tmp6B0=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2D4[0]=_tmp6B0;}),({struct Cyc_PP_Doc*_tmp6AF=Cyc_PP_text(({const char*_tmp2D5=" :=: ";_tag_fat(_tmp2D5,sizeof(char),6U);}));_tmp2D4[1]=_tmp6AF;}),({struct Cyc_PP_Doc*_tmp6AE=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2D4[2]=_tmp6AE;});Cyc_PP_cat(_tag_fat(_tmp2D4,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 36U: _LL51: _tmp211=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_tmp212=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL52: {struct Cyc_Core_Opt*n=_tmp211;struct Cyc_List_List*des=_tmp212;
# 1238
s=({struct _fat_ptr _tmp6B1=({const char*_tmp2D6=",";_tag_fat(_tmp2D6,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6B1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _LL53: _tmp210=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL54: {struct Cyc_Absyn_Stmt*s2=_tmp210;
# 1241
s=({struct Cyc_PP_Doc*_tmp2D7[7U];({struct Cyc_PP_Doc*_tmp6B8=Cyc_PP_text(({const char*_tmp2D8="(";_tag_fat(_tmp2D8,sizeof(char),2U);}));_tmp2D7[0]=_tmp6B8;}),({struct Cyc_PP_Doc*_tmp6B7=Cyc_Absynpp_lb();_tmp2D7[1]=_tmp6B7;}),({struct Cyc_PP_Doc*_tmp6B6=Cyc_PP_blank_doc();_tmp2D7[2]=_tmp6B6;}),({
struct Cyc_PP_Doc*_tmp6B5=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_tmp2D7[3]=_tmp6B5;}),({
struct Cyc_PP_Doc*_tmp6B4=Cyc_PP_blank_doc();_tmp2D7[4]=_tmp6B4;}),({struct Cyc_PP_Doc*_tmp6B3=Cyc_Absynpp_rb();_tmp2D7[5]=_tmp6B3;}),({struct Cyc_PP_Doc*_tmp6B2=Cyc_PP_text(({const char*_tmp2D9=")";_tag_fat(_tmp2D9,sizeof(char),2U);}));_tmp2D7[6]=_tmp6B2;});Cyc_PP_cat(_tag_fat(_tmp2D7,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}}_LL0:;}
# 1246
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2DA[3U];({struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_text(({const char*_tmp2DB="(";_tag_fat(_tmp2DB,sizeof(char),2U);}));_tmp2DA[0]=_tmp6BA;}),_tmp2DA[1]=s,({struct Cyc_PP_Doc*_tmp6B9=Cyc_PP_text(({const char*_tmp2DC=")";_tag_fat(_tmp2DC,sizeof(char),2U);}));_tmp2DA[2]=_tmp6B9;});Cyc_PP_cat(_tag_fat(_tmp2DA,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1251
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2DD=d;struct _fat_ptr*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DF;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2DD)->tag == 0U){_LL1: _tmp2DF=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2DD)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2DF;
return({struct Cyc_PP_Doc*_tmp2E0[3U];({struct Cyc_PP_Doc*_tmp6BD=Cyc_PP_text(({const char*_tmp2E1=".[";_tag_fat(_tmp2E1,sizeof(char),3U);}));_tmp2E0[0]=_tmp6BD;}),({struct Cyc_PP_Doc*_tmp6BC=Cyc_Absynpp_exp2doc(e);_tmp2E0[1]=_tmp6BC;}),({struct Cyc_PP_Doc*_tmp6BB=Cyc_PP_text(({const char*_tmp2E2="]";_tag_fat(_tmp2E2,sizeof(char),2U);}));_tmp2E0[2]=_tmp6BB;});Cyc_PP_cat(_tag_fat(_tmp2E0,sizeof(struct Cyc_PP_Doc*),3U));});}}else{_LL3: _tmp2DE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2DD)->f1;_LL4: {struct _fat_ptr*v=_tmp2DE;
return({struct Cyc_PP_Doc*_tmp2E3[2U];({struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_text(({const char*_tmp2E4=".";_tag_fat(_tmp2E4,sizeof(char),2U);}));_tmp2E3[0]=_tmp6BF;}),({struct Cyc_PP_Doc*_tmp6BE=Cyc_PP_textptr(v);_tmp2E3[1]=_tmp6BE;});Cyc_PP_cat(_tag_fat(_tmp2E3,sizeof(struct Cyc_PP_Doc*),2U));});}}_LL0:;}
# 1258
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2E5[2U];({struct Cyc_PP_Doc*_tmp6C4=({struct _fat_ptr _tmp6C3=({const char*_tmp2E6="";_tag_fat(_tmp2E6,sizeof(char),1U);});struct _fat_ptr _tmp6C2=({const char*_tmp2E7="=";_tag_fat(_tmp2E7,sizeof(char),2U);});struct _fat_ptr _tmp6C1=({const char*_tmp2E8="=";_tag_fat(_tmp2E8,sizeof(char),2U);});Cyc_PP_egroup(_tmp6C3,_tmp6C2,_tmp6C1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp2E5[0]=_tmp6C4;}),({
struct Cyc_PP_Doc*_tmp6C0=Cyc_Absynpp_exp2doc((*de).f2);_tmp2E5[1]=_tmp6C0;});Cyc_PP_cat(_tag_fat(_tmp2E5,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1264
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _tmp6C7=({const char*_tmp2E9="";_tag_fat(_tmp2E9,sizeof(char),1U);});struct _fat_ptr _tmp6C6=({const char*_tmp2EA="";_tag_fat(_tmp2EA,sizeof(char),1U);});struct _fat_ptr _tmp6C5=({const char*_tmp2EB=",";_tag_fat(_tmp2EB,sizeof(char),2U);});Cyc_PP_group(_tmp6C7,_tmp6C6,_tmp6C5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1268
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_tmp300=({unsigned _tmp2FF=28U + 1U;char*_tmp2FE=_cycalloc_atomic(_check_times(_tmp2FF,sizeof(char)));({{unsigned _tmp548=28U;unsigned i;for(i=0;i < _tmp548;++ i){_tmp2FE[i]='z';}_tmp2FE[_tmp548]=0;}0;});_tmp2FE;});_tag_fat(_tmp300,sizeof(char),29U);});
({struct _fat_ptr _tmp2EC=_fat_ptr_plus(x,sizeof(char),27);char _tmp2ED=*((char*)_check_fat_subscript(_tmp2EC,sizeof(char),0U));char _tmp2EE='\000';if(_get_fat_size(_tmp2EC,sizeof(char))== 1U &&(_tmp2ED == 0 && _tmp2EE != 0))_throw_arraybounds();*((char*)_tmp2EC.curr)=_tmp2EE;});
({struct _fat_ptr _tmp2EF=_fat_ptr_plus(x,sizeof(char),26);char _tmp2F0=*((char*)_check_fat_subscript(_tmp2EF,sizeof(char),0U));char _tmp2F1='L';if(_get_fat_size(_tmp2EF,sizeof(char))== 1U &&(_tmp2F0 == 0 && _tmp2F1 != 0))_throw_arraybounds();*((char*)_tmp2EF.curr)=_tmp2F1;});
({struct _fat_ptr _tmp2F2=_fat_ptr_plus(x,sizeof(char),25);char _tmp2F3=*((char*)_check_fat_subscript(_tmp2F2,sizeof(char),0U));char _tmp2F4='L';if(_get_fat_size(_tmp2F2,sizeof(char))== 1U &&(_tmp2F3 == 0 && _tmp2F4 != 0))_throw_arraybounds();*((char*)_tmp2F2.curr)=_tmp2F4;});
({struct _fat_ptr _tmp2F5=_fat_ptr_plus(x,sizeof(char),24);char _tmp2F6=*((char*)_check_fat_subscript(_tmp2F5,sizeof(char),0U));char _tmp2F7='U';if(_get_fat_size(_tmp2F5,sizeof(char))== 1U &&(_tmp2F6 == 0 && _tmp2F7 != 0))_throw_arraybounds();*((char*)_tmp2F5.curr)=_tmp2F7;});
({struct _fat_ptr _tmp2F8=_fat_ptr_plus(x,sizeof(char),23);char _tmp2F9=*((char*)_check_fat_subscript(_tmp2F8,sizeof(char),0U));char _tmp2FA='0';if(_get_fat_size(_tmp2F8,sizeof(char))== 1U &&(_tmp2F9 == 0 && _tmp2FA != 0))_throw_arraybounds();*((char*)_tmp2F8.curr)=_tmp2FA;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _fat_ptr _tmp2FB=_fat_ptr_plus(x,sizeof(char),index);char _tmp2FC=*((char*)_check_fat_subscript(_tmp2FB,sizeof(char),0U));char _tmp2FD=c;if(_get_fat_size(_tmp2FB,sizeof(char))== 1U &&(_tmp2FC == 0 && _tmp2FD != 0))_throw_arraybounds();*((char*)_tmp2FB.curr)=_tmp2FD;});
i=i / (unsigned long long)10;
-- index;}
# 1282
return(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1286
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp301=c;struct _fat_ptr _tmp302;struct _fat_ptr _tmp303;struct _fat_ptr _tmp304;long long _tmp306;enum Cyc_Absyn_Sign _tmp305;int _tmp308;enum Cyc_Absyn_Sign _tmp307;short _tmp30A;enum Cyc_Absyn_Sign _tmp309;struct _fat_ptr _tmp30B;char _tmp30D;enum Cyc_Absyn_Sign _tmp30C;switch((_tmp301.String_c).tag){case 2U: _LL1: _tmp30C=((_tmp301.Char_c).val).f1;_tmp30D=((_tmp301.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp30C;char ch=_tmp30D;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp310=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,({struct _fat_ptr _tmp6C8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp549.f1=_tmp6C8;});_tmp549;});void*_tmp30E[1U];_tmp30E[0]=& _tmp310;({struct _fat_ptr _tmp6C9=({const char*_tmp30F="'%s'";_tag_fat(_tmp30F,sizeof(char),5U);});Cyc_aprintf(_tmp6C9,_tag_fat(_tmp30E,sizeof(void*),1U));});}));}case 3U: _LL3: _tmp30B=(_tmp301.Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp30B;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp313=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,_tmp54A.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp54A;});void*_tmp311[1U];_tmp311[0]=& _tmp313;({struct _fat_ptr _tmp6CA=({const char*_tmp312="L'%s'";_tag_fat(_tmp312,sizeof(char),6U);});Cyc_aprintf(_tmp6CA,_tag_fat(_tmp311,sizeof(void*),1U));});}));}case 4U: _LL5: _tmp309=((_tmp301.Short_c).val).f1;_tmp30A=((_tmp301.Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp309;short s=_tmp30A;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp316=({struct Cyc_Int_pa_PrintArg_struct _tmp54B;_tmp54B.tag=1U,_tmp54B.f1=(unsigned long)((int)s);_tmp54B;});void*_tmp314[1U];_tmp314[0]=& _tmp316;({struct _fat_ptr _tmp6CB=({const char*_tmp315="%d";_tag_fat(_tmp315,sizeof(char),3U);});Cyc_aprintf(_tmp6CB,_tag_fat(_tmp314,sizeof(void*),1U));});}));}case 5U: _LL7: _tmp307=((_tmp301.Int_c).val).f1;_tmp308=((_tmp301.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign sn=_tmp307;int i=_tmp308;
# 1292
if((int)sn == (int)1U)return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp319=({struct Cyc_Int_pa_PrintArg_struct _tmp54C;_tmp54C.tag=1U,_tmp54C.f1=(unsigned)i;_tmp54C;});void*_tmp317[1U];_tmp317[0]=& _tmp319;({struct _fat_ptr _tmp6CC=({const char*_tmp318="%uU";_tag_fat(_tmp318,sizeof(char),4U);});Cyc_aprintf(_tmp6CC,_tag_fat(_tmp317,sizeof(void*),1U));});}));else{
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp31C=({struct Cyc_Int_pa_PrintArg_struct _tmp54D;_tmp54D.tag=1U,_tmp54D.f1=(unsigned long)i;_tmp54D;});void*_tmp31A[1U];_tmp31A[0]=& _tmp31C;({struct _fat_ptr _tmp6CD=({const char*_tmp31B="%d";_tag_fat(_tmp31B,sizeof(char),3U);});Cyc_aprintf(_tmp6CD,_tag_fat(_tmp31A,sizeof(void*),1U));});}));}}case 6U: _LL9: _tmp305=((_tmp301.LongLong_c).val).f1;_tmp306=((_tmp301.LongLong_c).val).f2;_LLA: {enum Cyc_Absyn_Sign sg=_tmp305;long long i=_tmp306;
# 1296
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7U: _LLB: _tmp304=((_tmp301.Float_c).val).f1;_LLC: {struct _fat_ptr x=_tmp304;
return Cyc_PP_text(x);}case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp31D="NULL";_tag_fat(_tmp31D,sizeof(char),5U);}));case 8U: _LLF: _tmp303=(_tmp301.String_c).val;_LL10: {struct _fat_ptr s=_tmp303;
return({struct Cyc_PP_Doc*_tmp31E[3U];({struct Cyc_PP_Doc*_tmp6D0=Cyc_PP_text(({const char*_tmp31F="\"";_tag_fat(_tmp31F,sizeof(char),2U);}));_tmp31E[0]=_tmp6D0;}),({struct Cyc_PP_Doc*_tmp6CF=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_tmp31E[1]=_tmp6CF;}),({struct Cyc_PP_Doc*_tmp6CE=Cyc_PP_text(({const char*_tmp320="\"";_tag_fat(_tmp320,sizeof(char),2U);}));_tmp31E[2]=_tmp6CE;});Cyc_PP_cat(_tag_fat(_tmp31E,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL11: _tmp302=(_tmp301.Wstring_c).val;_LL12: {struct _fat_ptr s=_tmp302;
return({struct Cyc_PP_Doc*_tmp321[3U];({struct Cyc_PP_Doc*_tmp6D3=Cyc_PP_text(({const char*_tmp322="L\"";_tag_fat(_tmp322,sizeof(char),3U);}));_tmp321[0]=_tmp6D3;}),({struct Cyc_PP_Doc*_tmp6D2=Cyc_PP_text(s);_tmp321[1]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_PP_text(({const char*_tmp323="\"";_tag_fat(_tmp323,sizeof(char),2U);}));_tmp321[2]=_tmp6D1;});Cyc_PP_cat(_tag_fat(_tmp321,sizeof(struct Cyc_PP_Doc*),3U));});}}_LL0:;}
# 1304
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6D6=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp326=({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0U,({struct _fat_ptr _tmp6D4=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp54E.f1=_tmp6D4;});_tmp54E;});void*_tmp324[1U];_tmp324[0]=& _tmp326;({struct _fat_ptr _tmp6D5=({const char*_tmp325="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_tmp325,sizeof(char),47U);});Cyc_aprintf(_tmp6D5,_tag_fat(_tmp324,sizeof(void*),1U));});});_tmp327->f1=_tmp6D6;});_tmp327;}));
# 1310
return({struct Cyc_PP_Doc*_tmp328[3U];({struct Cyc_PP_Doc*_tmp6D9=Cyc_PP_text(({const char*_tmp329="numelts(";_tag_fat(_tmp329,sizeof(char),9U);}));_tmp328[0]=_tmp6D9;}),({struct Cyc_PP_Doc*_tmp6D8=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp328[1]=_tmp6D8;}),({struct Cyc_PP_Doc*_tmp6D7=Cyc_PP_text(({const char*_tmp32A=")";_tag_fat(_tmp32A,sizeof(char),2U);}));_tmp328[2]=_tmp6D7;});Cyc_PP_cat(_tag_fat(_tmp328,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1312
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6DC=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32D=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({struct _fat_ptr _tmp6DA=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp54F.f1=_tmp6DA;});_tmp54F;});void*_tmp32B[1U];_tmp32B[0]=& _tmp32D;({struct _fat_ptr _tmp6DB=({const char*_tmp32C="Absynpp::primapp2doc: %s with no args";_tag_fat(_tmp32C,sizeof(char),38U);});Cyc_aprintf(_tmp6DB,_tag_fat(_tmp32B,sizeof(void*),1U));});});_tmp32E->f1=_tmp6DC;});_tmp32E;}));else{
# 1316
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp32F[3U];_tmp32F[0]=ps,({struct Cyc_PP_Doc*_tmp6DD=Cyc_PP_text(({const char*_tmp330=" ";_tag_fat(_tmp330,sizeof(char),2U);}));_tmp32F[1]=_tmp6DD;}),_tmp32F[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_tmp32F,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6E0=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp333=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({struct _fat_ptr _tmp6DE=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp550.f1=_tmp6DE;});_tmp550;});void*_tmp331[1U];_tmp331[0]=& _tmp333;({struct _fat_ptr _tmp6DF=({const char*_tmp332="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_tmp332,sizeof(char),47U);});Cyc_aprintf(_tmp6DF,_tag_fat(_tmp331,sizeof(void*),1U));});});_tmp334->f1=_tmp6E0;});_tmp334;}));else{
# 1322
return({struct Cyc_PP_Doc*_tmp335[5U];_tmp335[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp6E2=Cyc_PP_text(({const char*_tmp336=" ";_tag_fat(_tmp336,sizeof(char),2U);}));_tmp335[1]=_tmp6E2;}),_tmp335[2]=ps,({struct Cyc_PP_Doc*_tmp6E1=Cyc_PP_text(({const char*_tmp337=" ";_tag_fat(_tmp337,sizeof(char),2U);}));_tmp335[3]=_tmp6E1;}),_tmp335[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_tmp335,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1326
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp338=p;switch(_tmp338){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp339="+";_tag_fat(_tmp339,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp33A="*";_tag_fat(_tmp33A,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp33B="-";_tag_fat(_tmp33B,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp33C="/";_tag_fat(_tmp33C,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp33D="\\%";_tag_fat(_tmp33D,sizeof(char),3U);}):({const char*_tmp33E="%";_tag_fat(_tmp33E,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp33F="==";_tag_fat(_tmp33F,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp340="!=";_tag_fat(_tmp340,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp341=">";_tag_fat(_tmp341,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp342="<";_tag_fat(_tmp342,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp343=">=";_tag_fat(_tmp343,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp344="<=";_tag_fat(_tmp344,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp345="!";_tag_fat(_tmp345,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp346="~";_tag_fat(_tmp346,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp347="&";_tag_fat(_tmp347,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp348="|";_tag_fat(_tmp348,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp349="^";_tag_fat(_tmp349,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp34A="<<";_tag_fat(_tmp34A,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp34B=">>";_tag_fat(_tmp34B,sizeof(char),3U);});case Cyc_Absyn_Numelts: _LL25: _LL26:
 return({const char*_tmp34C="numelts";_tag_fat(_tmp34C,sizeof(char),8U);});default: _LL27: _LL28:
 return({const char*_tmp34D="?";_tag_fat(_tmp34D,sizeof(char),2U);});}_LL0:;}
# 1351
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1355
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp34E=s->r;void*_stmttmp16=_tmp34E;void*_tmp34F=_stmttmp16;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34F)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1370 "absynpp.cyc"
struct _tuple13 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_tmp350=d->r;void*_stmttmp17=_tmp350;void*_tmp351=_stmttmp17;struct Cyc_Absyn_Vardecl*_tmp352;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp351)->tag == 0U){_LL1: _tmp352=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp351)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp352;
# 1373
if(((int(*)(int(*compare)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*l,struct _tuple0*x))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple13 _tmp551;_tmp551.f1=1,({struct Cyc_List_List*_tmp6E3=({struct Cyc_List_List*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->hd=vd->name,_tmp353->tl=0;_tmp353;});_tmp551.f2=_tmp6E3;});_tmp551;});else{
# 1376
return({struct _tuple13 _tmp552;_tmp552.f1=0,({struct Cyc_List_List*_tmp6E4=({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->hd=vd->name,_tmp354->tl=varsinblock;_tmp354;});_tmp552.f2=_tmp6E4;});_tmp552;});}}}else{_LL3: _LL4:
# 1378
 return({struct _tuple13 _tmp553;_tmp553.f1=0,_tmp553.f2=varsinblock;_tmp553;});}_LL0:;}
# 1382
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_tmp355[8U];({struct Cyc_PP_Doc*_tmp6EC=Cyc_PP_text(({const char*_tmp356="(";_tag_fat(_tmp356,sizeof(char),2U);}));_tmp355[0]=_tmp6EC;}),({struct Cyc_PP_Doc*_tmp6EB=Cyc_Absynpp_lb();_tmp355[1]=_tmp6EB;}),({struct Cyc_PP_Doc*_tmp6EA=Cyc_PP_blank_doc();_tmp355[2]=_tmp6EA;}),({struct Cyc_PP_Doc*_tmp6E9=Cyc_PP_nest(2,d);_tmp355[3]=_tmp6E9;}),({struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_line_doc();_tmp355[4]=_tmp6E8;}),({struct Cyc_PP_Doc*_tmp6E7=Cyc_Absynpp_rb();_tmp355[5]=_tmp6E7;}),({
struct Cyc_PP_Doc*_tmp6E6=Cyc_PP_text(({const char*_tmp357=");";_tag_fat(_tmp357,sizeof(char),3U);}));_tmp355[6]=_tmp6E6;}),({struct Cyc_PP_Doc*_tmp6E5=Cyc_PP_line_doc();_tmp355[7]=_tmp6E5;});Cyc_PP_cat(_tag_fat(_tmp355,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1387
return({struct Cyc_PP_Doc*_tmp358[6U];({struct Cyc_PP_Doc*_tmp6F2=Cyc_Absynpp_lb();_tmp358[0]=_tmp6F2;}),({struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_blank_doc();_tmp358[1]=_tmp6F1;}),({struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_nest(2,d);_tmp358[2]=_tmp6F0;}),({struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_line_doc();_tmp358[3]=_tmp6EF;}),({struct Cyc_PP_Doc*_tmp6EE=Cyc_Absynpp_rb();_tmp358[4]=_tmp6EE;}),({struct Cyc_PP_Doc*_tmp6ED=Cyc_PP_line_doc();_tmp358[5]=_tmp6ED;});Cyc_PP_cat(_tag_fat(_tmp358,sizeof(struct Cyc_PP_Doc*),6U));});}}
# 1390
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_tmp359=st->r;void*_stmttmp18=_tmp359;void*_tmp35A=_stmttmp18;struct Cyc_List_List*_tmp35C;struct Cyc_Absyn_Stmt*_tmp35B;struct Cyc_Absyn_Exp*_tmp35E;struct Cyc_Absyn_Stmt*_tmp35D;struct Cyc_Absyn_Stmt*_tmp360;struct _fat_ptr*_tmp35F;struct Cyc_Absyn_Stmt*_tmp362;struct Cyc_Absyn_Decl*_tmp361;struct Cyc_List_List*_tmp363;struct Cyc_List_List*_tmp365;struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Stmt*_tmp369;struct Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_Absyn_Exp*_tmp366;struct _fat_ptr*_tmp36A;struct Cyc_Absyn_Stmt*_tmp36C;struct Cyc_Absyn_Exp*_tmp36B;struct Cyc_Absyn_Stmt*_tmp36F;struct Cyc_Absyn_Stmt*_tmp36E;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Stmt*_tmp372;struct Cyc_Absyn_Stmt*_tmp371;struct Cyc_Absyn_Exp*_tmp373;switch(*((int*)_tmp35A)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp374=";";_tag_fat(_tmp374,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp373=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp373;
s=({struct Cyc_PP_Doc*_tmp375[2U];({struct Cyc_PP_Doc*_tmp6F4=Cyc_Absynpp_exp2doc(e);_tmp375[0]=_tmp6F4;}),({struct Cyc_PP_Doc*_tmp6F3=Cyc_PP_text(({const char*_tmp376=";";_tag_fat(_tmp376,sizeof(char),2U);}));_tmp375[1]=_tmp6F3;});Cyc_PP_cat(_tag_fat(_tmp375,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;}case 2U: _LL5: _tmp371=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_tmp372=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp371;struct Cyc_Absyn_Stmt*s2=_tmp372;
# 1396
if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp377[2U];({struct Cyc_PP_Doc*_tmp6F8=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp377[0]=_tmp6F8;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_tmp6F7=({int _tmp6F6=stmtexp;Cyc_Absynpp_block(_tmp6F6,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp377[1]=_tmp6F7;}):({
struct Cyc_PP_Doc*_tmp6F5=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp377[1]=_tmp6F5;});Cyc_PP_cat(_tag_fat(_tmp377,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(Cyc_Absynpp_is_declaration(s2))
s=({struct Cyc_PP_Doc*_tmp378[3U];({struct Cyc_PP_Doc*_tmp6FC=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp378[0]=_tmp6FC;}),({
struct Cyc_PP_Doc*_tmp6FB=Cyc_PP_line_doc();_tmp378[1]=_tmp6FB;}),({
struct Cyc_PP_Doc*_tmp6FA=({int _tmp6F9=stmtexp;Cyc_Absynpp_block(_tmp6F9,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp378[2]=_tmp6FA;});Cyc_PP_cat(_tag_fat(_tmp378,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1407
s=({struct Cyc_PP_Doc*_tmp379[3U];({struct Cyc_PP_Doc*_tmp6FF=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp379[0]=_tmp6FF;}),({struct Cyc_PP_Doc*_tmp6FE=Cyc_PP_line_doc();_tmp379[1]=_tmp6FE;}),({
struct Cyc_PP_Doc*_tmp6FD=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp379[2]=_tmp6FD;});Cyc_PP_cat(_tag_fat(_tmp379,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1411
s=({struct Cyc_PP_Doc*_tmp37A[3U];({struct Cyc_PP_Doc*_tmp702=Cyc_Absynpp_stmt2doc(s1,0,0);_tmp37A[0]=_tmp702;}),({struct Cyc_PP_Doc*_tmp701=Cyc_PP_line_doc();_tmp37A[1]=_tmp701;}),({struct Cyc_PP_Doc*_tmp700=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_tmp37A[2]=_tmp700;});Cyc_PP_cat(_tag_fat(_tmp37A,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 3U: _LL7: _tmp370=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp370;
# 1414
if(eopt == 0)
s=Cyc_PP_text(({const char*_tmp37B="return;";_tag_fat(_tmp37B,sizeof(char),8U);}));else{
# 1417
s=({struct Cyc_PP_Doc*_tmp37C[3U];({struct Cyc_PP_Doc*_tmp706=Cyc_PP_text(({const char*_tmp37D="return ";_tag_fat(_tmp37D,sizeof(char),8U);}));_tmp37C[0]=_tmp706;}),
eopt == 0?({struct Cyc_PP_Doc*_tmp705=Cyc_PP_nil_doc();_tmp37C[1]=_tmp705;}):({struct Cyc_PP_Doc*_tmp704=Cyc_Absynpp_exp2doc(eopt);_tmp37C[1]=_tmp704;}),({
struct Cyc_PP_Doc*_tmp703=Cyc_PP_text(({const char*_tmp37E=";";_tag_fat(_tmp37E,sizeof(char),2U);}));_tmp37C[2]=_tmp703;});Cyc_PP_cat(_tag_fat(_tmp37C,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 4U: _LL9: _tmp36D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_tmp36E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2;_tmp36F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp35A)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp36D;struct Cyc_Absyn_Stmt*s1=_tmp36E;struct Cyc_Absyn_Stmt*s2=_tmp36F;
# 1422
int print_else;
{void*_tmp37F=s2->r;void*_stmttmp19=_tmp37F;void*_tmp380=_stmttmp19;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp380)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1427
s=({struct Cyc_PP_Doc*_tmp381[5U];({struct Cyc_PP_Doc*_tmp70F=Cyc_PP_text(({const char*_tmp382="if (";_tag_fat(_tmp382,sizeof(char),5U);}));_tmp381[0]=_tmp70F;}),({
struct Cyc_PP_Doc*_tmp70E=Cyc_Absynpp_exp2doc(e);_tmp381[1]=_tmp70E;}),({
struct Cyc_PP_Doc*_tmp70D=Cyc_PP_text(({const char*_tmp383=") ";_tag_fat(_tmp383,sizeof(char),3U);}));_tmp381[2]=_tmp70D;}),({
struct Cyc_PP_Doc*_tmp70C=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp381[3]=_tmp70C;}),
print_else?({
struct Cyc_PP_Doc*_tmp70B=({struct Cyc_PP_Doc*_tmp384[3U];({struct Cyc_PP_Doc*_tmp70A=Cyc_PP_line_doc();_tmp384[0]=_tmp70A;}),({
struct Cyc_PP_Doc*_tmp709=Cyc_PP_text(({const char*_tmp385="else ";_tag_fat(_tmp385,sizeof(char),6U);}));_tmp384[1]=_tmp709;}),({
struct Cyc_PP_Doc*_tmp708=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_tmp384[2]=_tmp708;});Cyc_PP_cat(_tag_fat(_tmp384,sizeof(struct Cyc_PP_Doc*),3U));});
# 1432
_tmp381[4]=_tmp70B;}):({
# 1435
struct Cyc_PP_Doc*_tmp707=Cyc_PP_nil_doc();_tmp381[4]=_tmp707;});Cyc_PP_cat(_tag_fat(_tmp381,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 5U: _LLB: _tmp36B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1).f1;_tmp36C=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp36B;struct Cyc_Absyn_Stmt*s1=_tmp36C;
# 1438
s=({struct Cyc_PP_Doc*_tmp386[4U];({struct Cyc_PP_Doc*_tmp713=Cyc_PP_text(({const char*_tmp387="while (";_tag_fat(_tmp387,sizeof(char),8U);}));_tmp386[0]=_tmp713;}),({
struct Cyc_PP_Doc*_tmp712=Cyc_Absynpp_exp2doc(e);_tmp386[1]=_tmp712;}),({
struct Cyc_PP_Doc*_tmp711=Cyc_PP_text(({const char*_tmp388=") ";_tag_fat(_tmp388,sizeof(char),3U);}));_tmp386[2]=_tmp711;}),({
struct Cyc_PP_Doc*_tmp710=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp386[3]=_tmp710;});Cyc_PP_cat(_tag_fat(_tmp386,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp389="break;";_tag_fat(_tmp389,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp38A="continue;";_tag_fat(_tmp38A,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp36A=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_LL12: {struct _fat_ptr*x=_tmp36A;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp38D=({struct Cyc_String_pa_PrintArg_struct _tmp554;_tmp554.tag=0U,_tmp554.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp554;});void*_tmp38B[1U];_tmp38B[0]=& _tmp38D;({struct _fat_ptr _tmp714=({const char*_tmp38C="goto %s;";_tag_fat(_tmp38C,sizeof(char),9U);});Cyc_aprintf(_tmp714,_tag_fat(_tmp38B,sizeof(void*),1U));});}));goto _LL0;}case 9U: _LL13: _tmp366=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_tmp367=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2).f1;_tmp368=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp35A)->f3).f1;_tmp369=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp35A)->f4;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp366;struct Cyc_Absyn_Exp*e2=_tmp367;struct Cyc_Absyn_Exp*e3=_tmp368;struct Cyc_Absyn_Stmt*s1=_tmp369;
# 1447
s=({struct Cyc_PP_Doc*_tmp38E[8U];({struct Cyc_PP_Doc*_tmp71C=Cyc_PP_text(({const char*_tmp38F="for(";_tag_fat(_tmp38F,sizeof(char),5U);}));_tmp38E[0]=_tmp71C;}),({
struct Cyc_PP_Doc*_tmp71B=Cyc_Absynpp_exp2doc(e1);_tmp38E[1]=_tmp71B;}),({
struct Cyc_PP_Doc*_tmp71A=Cyc_PP_text(({const char*_tmp390="; ";_tag_fat(_tmp390,sizeof(char),3U);}));_tmp38E[2]=_tmp71A;}),({
struct Cyc_PP_Doc*_tmp719=Cyc_Absynpp_exp2doc(e2);_tmp38E[3]=_tmp719;}),({
struct Cyc_PP_Doc*_tmp718=Cyc_PP_text(({const char*_tmp391="; ";_tag_fat(_tmp391,sizeof(char),3U);}));_tmp38E[4]=_tmp718;}),({
struct Cyc_PP_Doc*_tmp717=Cyc_Absynpp_exp2doc(e3);_tmp38E[5]=_tmp717;}),({
struct Cyc_PP_Doc*_tmp716=Cyc_PP_text(({const char*_tmp392=") ";_tag_fat(_tmp392,sizeof(char),3U);}));_tmp38E[6]=_tmp716;}),({
struct Cyc_PP_Doc*_tmp715=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp38E[7]=_tmp715;});Cyc_PP_cat(_tag_fat(_tmp38E,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 10U: _LL15: _tmp364=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_tmp365=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp364;struct Cyc_List_List*ss=_tmp365;
# 1457
s=({struct Cyc_PP_Doc*_tmp393[8U];({struct Cyc_PP_Doc*_tmp724=Cyc_PP_text(({const char*_tmp394="switch (";_tag_fat(_tmp394,sizeof(char),9U);}));_tmp393[0]=_tmp724;}),({
struct Cyc_PP_Doc*_tmp723=Cyc_Absynpp_exp2doc(e);_tmp393[1]=_tmp723;}),({
struct Cyc_PP_Doc*_tmp722=Cyc_PP_text(({const char*_tmp395=") ";_tag_fat(_tmp395,sizeof(char),3U);}));_tmp393[2]=_tmp722;}),({
struct Cyc_PP_Doc*_tmp721=Cyc_Absynpp_lb();_tmp393[3]=_tmp721;}),({
struct Cyc_PP_Doc*_tmp720=Cyc_PP_line_doc();_tmp393[4]=_tmp720;}),({
struct Cyc_PP_Doc*_tmp71F=Cyc_Absynpp_switchclauses2doc(ss);_tmp393[5]=_tmp71F;}),({
struct Cyc_PP_Doc*_tmp71E=Cyc_PP_line_doc();_tmp393[6]=_tmp71E;}),({
struct Cyc_PP_Doc*_tmp71D=Cyc_Absynpp_rb();_tmp393[7]=_tmp71D;});Cyc_PP_cat(_tag_fat(_tmp393,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp396="fallthru;";_tag_fat(_tmp396,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp363=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_LL1A: {struct Cyc_List_List*es=_tmp363;
# 1468
s=({struct Cyc_PP_Doc*_tmp397[3U];({struct Cyc_PP_Doc*_tmp727=Cyc_PP_text(({const char*_tmp398="fallthru(";_tag_fat(_tmp398,sizeof(char),10U);}));_tmp397[0]=_tmp727;}),({struct Cyc_PP_Doc*_tmp726=Cyc_Absynpp_exps2doc_prec(20,es);_tmp397[1]=_tmp726;}),({struct Cyc_PP_Doc*_tmp725=Cyc_PP_text(({const char*_tmp399=");";_tag_fat(_tmp399,sizeof(char),3U);}));_tmp397[2]=_tmp725;});Cyc_PP_cat(_tag_fat(_tmp397,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 12U: _LL1B: _tmp361=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_tmp362=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp361;struct Cyc_Absyn_Stmt*s1=_tmp362;
# 1470
struct _tuple13 _tmp39A=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple13 _stmttmp1A=_tmp39A;struct _tuple13 _tmp39B=_stmttmp1A;struct Cyc_List_List*_tmp39D;int _tmp39C;_LL29: _tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;_LL2A: {int newblock=_tmp39C;struct Cyc_List_List*newvarsinblock=_tmp39D;
s=({struct Cyc_PP_Doc*_tmp39E[3U];({struct Cyc_PP_Doc*_tmp72A=Cyc_Absynpp_decl2doc(d);_tmp39E[0]=_tmp72A;}),({struct Cyc_PP_Doc*_tmp729=Cyc_PP_line_doc();_tmp39E[1]=_tmp729;}),({struct Cyc_PP_Doc*_tmp728=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_tmp39E[2]=_tmp728;});Cyc_PP_cat(_tag_fat(_tmp39E,sizeof(struct Cyc_PP_Doc*),3U));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}}case 13U: _LL1D: _tmp35F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_tmp360=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2;_LL1E: {struct _fat_ptr*x=_tmp35F;struct Cyc_Absyn_Stmt*s1=_tmp360;
# 1475
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp39F[3U];({struct Cyc_PP_Doc*_tmp72E=Cyc_PP_textptr(x);_tmp39F[0]=_tmp72E;}),({struct Cyc_PP_Doc*_tmp72D=Cyc_PP_text(({const char*_tmp3A0=": ";_tag_fat(_tmp3A0,sizeof(char),3U);}));_tmp39F[1]=_tmp72D;}),({struct Cyc_PP_Doc*_tmp72C=({int _tmp72B=stmtexp;Cyc_Absynpp_block(_tmp72B,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_tmp39F[2]=_tmp72C;});Cyc_PP_cat(_tag_fat(_tmp39F,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1478
s=({struct Cyc_PP_Doc*_tmp3A1[3U];({struct Cyc_PP_Doc*_tmp731=Cyc_PP_textptr(x);_tmp3A1[0]=_tmp731;}),({struct Cyc_PP_Doc*_tmp730=Cyc_PP_text(({const char*_tmp3A2=": ";_tag_fat(_tmp3A2,sizeof(char),3U);}));_tmp3A1[1]=_tmp730;}),({struct Cyc_PP_Doc*_tmp72F=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_tmp3A1[2]=_tmp72F;});Cyc_PP_cat(_tag_fat(_tmp3A1,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;}case 14U: _LL1F: _tmp35D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_tmp35E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2).f1;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp35D;struct Cyc_Absyn_Exp*e=_tmp35E;
# 1481
s=({struct Cyc_PP_Doc*_tmp3A3[5U];({struct Cyc_PP_Doc*_tmp736=Cyc_PP_text(({const char*_tmp3A4="do ";_tag_fat(_tmp3A4,sizeof(char),4U);}));_tmp3A3[0]=_tmp736;}),({
struct Cyc_PP_Doc*_tmp735=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3A3[1]=_tmp735;}),({
struct Cyc_PP_Doc*_tmp734=Cyc_PP_text(({const char*_tmp3A5=" while (";_tag_fat(_tmp3A5,sizeof(char),9U);}));_tmp3A3[2]=_tmp734;}),({
struct Cyc_PP_Doc*_tmp733=Cyc_Absynpp_exp2doc(e);_tmp3A3[3]=_tmp733;}),({
struct Cyc_PP_Doc*_tmp732=Cyc_PP_text(({const char*_tmp3A6=");";_tag_fat(_tmp3A6,sizeof(char),3U);}));_tmp3A3[4]=_tmp732;});Cyc_PP_cat(_tag_fat(_tmp3A3,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}default: _LL21: _tmp35B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_tmp35C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp35A)->f2;_LL22: {struct Cyc_Absyn_Stmt*s1=_tmp35B;struct Cyc_List_List*ss=_tmp35C;
# 1488
s=({struct Cyc_PP_Doc*_tmp3A7[4U];({struct Cyc_PP_Doc*_tmp73A=Cyc_PP_text(({const char*_tmp3A8="try ";_tag_fat(_tmp3A8,sizeof(char),5U);}));_tmp3A7[0]=_tmp73A;}),({
struct Cyc_PP_Doc*_tmp739=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp3A7[1]=_tmp739;}),({
struct Cyc_PP_Doc*_tmp738=Cyc_PP_text(({const char*_tmp3A9=" catch ";_tag_fat(_tmp3A9,sizeof(char),8U);}));_tmp3A7[2]=_tmp738;}),({
struct Cyc_PP_Doc*_tmp737=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_tmp3A7[3]=_tmp737;});Cyc_PP_cat(_tag_fat(_tmp3A7,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}_LL0:;}
# 1494
return s;}
# 1497
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3AA=p->r;void*_stmttmp1B=_tmp3AA;void*_tmp3AB=_stmttmp1B;struct Cyc_Absyn_Exp*_tmp3AC;int _tmp3AF;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Datatypefield*_tmp3AD;struct Cyc_Absyn_Datatypefield*_tmp3B0;struct Cyc_Absyn_Enumfield*_tmp3B1;struct Cyc_Absyn_Enumfield*_tmp3B2;int _tmp3B5;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List*_tmp3B3;int _tmp3B9;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List*_tmp3B7;union Cyc_Absyn_AggrInfo _tmp3B6;int _tmp3BC;struct Cyc_List_List*_tmp3BB;struct _tuple0*_tmp3BA;struct _tuple0*_tmp3BD;struct Cyc_Absyn_Pat*_tmp3BF;struct Cyc_Absyn_Vardecl*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3C0;struct Cyc_Absyn_Pat*_tmp3C1;int _tmp3C3;struct Cyc_List_List*_tmp3C2;struct Cyc_Absyn_Vardecl*_tmp3C5;struct Cyc_Absyn_Tvar*_tmp3C4;struct Cyc_Absyn_Vardecl*_tmp3C7;struct Cyc_Absyn_Tvar*_tmp3C6;struct Cyc_Absyn_Pat*_tmp3C9;struct Cyc_Absyn_Vardecl*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3CA;struct _fat_ptr _tmp3CB;char _tmp3CC;int _tmp3CE;enum Cyc_Absyn_Sign _tmp3CD;switch(*((int*)_tmp3AB)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3CF="_";_tag_fat(_tmp3CF,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp3D0="NULL";_tag_fat(_tmp3D0,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp3CD=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3CE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp3CD;int i=_tmp3CE;
# 1503
if((int)sg != (int)1U)
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D3=({struct Cyc_Int_pa_PrintArg_struct _tmp555;_tmp555.tag=1U,_tmp555.f1=(unsigned long)i;_tmp555;});void*_tmp3D1[1U];_tmp3D1[0]=& _tmp3D3;({struct _fat_ptr _tmp73B=({const char*_tmp3D2="%d";_tag_fat(_tmp3D2,sizeof(char),3U);});Cyc_aprintf(_tmp73B,_tag_fat(_tmp3D1,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D6=({struct Cyc_Int_pa_PrintArg_struct _tmp556;_tmp556.tag=1U,_tmp556.f1=(unsigned)i;_tmp556;});void*_tmp3D4[1U];_tmp3D4[0]=& _tmp3D6;({struct _fat_ptr _tmp73C=({const char*_tmp3D5="%u";_tag_fat(_tmp3D5,sizeof(char),3U);});Cyc_aprintf(_tmp73C,_tag_fat(_tmp3D4,sizeof(void*),1U));});}));}
goto _LL0;}case 11U: _LL7: _tmp3CC=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL8: {char ch=_tmp3CC;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D9=({struct Cyc_String_pa_PrintArg_struct _tmp557;_tmp557.tag=0U,({struct _fat_ptr _tmp73D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp557.f1=_tmp73D;});_tmp557;});void*_tmp3D7[1U];_tmp3D7[0]=& _tmp3D9;({struct _fat_ptr _tmp73E=({const char*_tmp3D8="'%s'";_tag_fat(_tmp3D8,sizeof(char),5U);});Cyc_aprintf(_tmp73E,_tag_fat(_tmp3D7,sizeof(void*),1U));});}));goto _LL0;}case 12U: _LL9: _tmp3CB=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LLA: {struct _fat_ptr x=_tmp3CB;
s=Cyc_PP_text(x);goto _LL0;}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2)->r)->tag == 0U){_LLB: _tmp3CA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp3CA;
# 1510
s=Cyc_Absynpp_qvar2doc(vd->name);goto _LL0;}}else{_LLD: _tmp3C8=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3C9=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmp3C8;struct Cyc_Absyn_Pat*p2=_tmp3C9;
# 1512
s=({struct Cyc_PP_Doc*_tmp3DA[3U];({struct Cyc_PP_Doc*_tmp741=Cyc_Absynpp_qvar2doc(vd->name);_tmp3DA[0]=_tmp741;}),({struct Cyc_PP_Doc*_tmp740=Cyc_PP_text(({const char*_tmp3DB=" as ";_tag_fat(_tmp3DB,sizeof(char),5U);}));_tmp3DA[1]=_tmp740;}),({struct Cyc_PP_Doc*_tmp73F=Cyc_Absynpp_pat2doc(p2);_tmp3DA[2]=_tmp73F;});Cyc_PP_cat(_tag_fat(_tmp3DA,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}}case 2U: _LLF: _tmp3C6=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3C7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp3C6;struct Cyc_Absyn_Vardecl*vd=_tmp3C7;
# 1514
s=({struct Cyc_PP_Doc*_tmp3DC[5U];({struct Cyc_PP_Doc*_tmp746=Cyc_PP_text(({const char*_tmp3DD="alias";_tag_fat(_tmp3DD,sizeof(char),6U);}));_tmp3DC[0]=_tmp746;}),({struct Cyc_PP_Doc*_tmp745=Cyc_PP_text(({const char*_tmp3DE=" <";_tag_fat(_tmp3DE,sizeof(char),3U);}));_tmp3DC[1]=_tmp745;}),({struct Cyc_PP_Doc*_tmp744=Cyc_Absynpp_tvar2doc(tv);_tmp3DC[2]=_tmp744;}),({struct Cyc_PP_Doc*_tmp743=Cyc_PP_text(({const char*_tmp3DF="> ";_tag_fat(_tmp3DF,sizeof(char),3U);}));_tmp3DC[3]=_tmp743;}),({
struct Cyc_PP_Doc*_tmp742=Cyc_Absynpp_vardecl2doc(vd);_tmp3DC[4]=_tmp742;});Cyc_PP_cat(_tag_fat(_tmp3DC,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 4U: _LL11: _tmp3C4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3C5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL12: {struct Cyc_Absyn_Tvar*tv=_tmp3C4;struct Cyc_Absyn_Vardecl*vd=_tmp3C5;
# 1518
s=({struct Cyc_PP_Doc*_tmp3E0[4U];({struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_qvar2doc(vd->name);_tmp3E0[0]=_tmp74A;}),({struct Cyc_PP_Doc*_tmp749=Cyc_PP_text(({const char*_tmp3E1="<";_tag_fat(_tmp3E1,sizeof(char),2U);}));_tmp3E0[1]=_tmp749;}),({struct Cyc_PP_Doc*_tmp748=Cyc_Absynpp_tvar2doc(tv);_tmp3E0[2]=_tmp748;}),({struct Cyc_PP_Doc*_tmp747=Cyc_PP_text(({const char*_tmp3E2=">";_tag_fat(_tmp3E2,sizeof(char),2U);}));_tmp3E0[3]=_tmp747;});Cyc_PP_cat(_tag_fat(_tmp3E0,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 5U: _LL13: _tmp3C2=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3C3=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL14: {struct Cyc_List_List*ts=_tmp3C2;int dots=_tmp3C3;
# 1521
s=({struct Cyc_PP_Doc*_tmp3E3[4U];({struct Cyc_PP_Doc*_tmp750=Cyc_Absynpp_dollar();_tmp3E3[0]=_tmp750;}),({struct Cyc_PP_Doc*_tmp74F=Cyc_PP_text(({const char*_tmp3E4="(";_tag_fat(_tmp3E4,sizeof(char),2U);}));_tmp3E3[1]=_tmp74F;}),({struct Cyc_PP_Doc*_tmp74E=({struct _fat_ptr _tmp74D=({const char*_tmp3E5=",";_tag_fat(_tmp3E5,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp74D,ts);});_tmp3E3[2]=_tmp74E;}),
dots?({struct Cyc_PP_Doc*_tmp74C=Cyc_PP_text(({const char*_tmp3E6=", ...)";_tag_fat(_tmp3E6,sizeof(char),7U);}));_tmp3E3[3]=_tmp74C;}):({struct Cyc_PP_Doc*_tmp74B=Cyc_PP_text(({const char*_tmp3E7=")";_tag_fat(_tmp3E7,sizeof(char),2U);}));_tmp3E3[3]=_tmp74B;});Cyc_PP_cat(_tag_fat(_tmp3E3,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 6U: _LL15: _tmp3C1=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL16: {struct Cyc_Absyn_Pat*p2=_tmp3C1;
# 1525
s=({struct Cyc_PP_Doc*_tmp3E8[2U];({struct Cyc_PP_Doc*_tmp752=Cyc_PP_text(({const char*_tmp3E9="&";_tag_fat(_tmp3E9,sizeof(char),2U);}));_tmp3E8[0]=_tmp752;}),({struct Cyc_PP_Doc*_tmp751=Cyc_Absynpp_pat2doc(p2);_tmp3E8[1]=_tmp751;});Cyc_PP_cat(_tag_fat(_tmp3E8,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2)->r)->tag == 0U){_LL17: _tmp3C0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp3C0;
# 1528
s=({struct Cyc_PP_Doc*_tmp3EA[2U];({struct Cyc_PP_Doc*_tmp754=Cyc_PP_text(({const char*_tmp3EB="*";_tag_fat(_tmp3EB,sizeof(char),2U);}));_tmp3EA[0]=_tmp754;}),({struct Cyc_PP_Doc*_tmp753=Cyc_Absynpp_qvar2doc(vd->name);_tmp3EA[1]=_tmp753;});Cyc_PP_cat(_tag_fat(_tmp3EA,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}else{_LL19: _tmp3BE=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3BF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp3BE;struct Cyc_Absyn_Pat*p2=_tmp3BF;
# 1531
s=({struct Cyc_PP_Doc*_tmp3EC[4U];({struct Cyc_PP_Doc*_tmp758=Cyc_PP_text(({const char*_tmp3ED="*";_tag_fat(_tmp3ED,sizeof(char),2U);}));_tmp3EC[0]=_tmp758;}),({struct Cyc_PP_Doc*_tmp757=Cyc_Absynpp_qvar2doc(vd->name);_tmp3EC[1]=_tmp757;}),({struct Cyc_PP_Doc*_tmp756=Cyc_PP_text(({const char*_tmp3EE=" as ";_tag_fat(_tmp3EE,sizeof(char),5U);}));_tmp3EC[2]=_tmp756;}),({struct Cyc_PP_Doc*_tmp755=Cyc_Absynpp_pat2doc(p2);_tmp3EC[3]=_tmp755;});Cyc_PP_cat(_tag_fat(_tmp3EC,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}case 15U: _LL1B: _tmp3BD=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL1C: {struct _tuple0*q=_tmp3BD;
# 1534
s=Cyc_Absynpp_qvar2doc(q);
goto _LL0;}case 16U: _LL1D: _tmp3BA=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3BB=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_tmp3BC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3;_LL1E: {struct _tuple0*q=_tmp3BA;struct Cyc_List_List*ps=_tmp3BB;int dots=_tmp3BC;
# 1537
struct _fat_ptr term=dots?({const char*_tmp3F2=", ...)";_tag_fat(_tmp3F2,sizeof(char),7U);}):({const char*_tmp3F3=")";_tag_fat(_tmp3F3,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3EF[2U];({struct Cyc_PP_Doc*_tmp75D=Cyc_Absynpp_qvar2doc(q);_tmp3EF[0]=_tmp75D;}),({struct Cyc_PP_Doc*_tmp75C=({struct _fat_ptr _tmp75B=({const char*_tmp3F0="(";_tag_fat(_tmp3F0,sizeof(char),2U);});struct _fat_ptr _tmp75A=term;struct _fat_ptr _tmp759=({const char*_tmp3F1=",";_tag_fat(_tmp3F1,sizeof(char),2U);});Cyc_PP_group(_tmp75B,_tmp75A,_tmp759,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp3EF[1]=_tmp75C;});Cyc_PP_cat(_tag_fat(_tmp3EF,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1 != 0){_LL1F: _tmp3B6=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3B7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_tmp3B8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3;_tmp3B9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f4;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp3B6;struct Cyc_List_List*exists=_tmp3B7;struct Cyc_List_List*dps=_tmp3B8;int dots=_tmp3B9;
# 1541
struct _fat_ptr term=dots?({const char*_tmp3FD=", ...}";_tag_fat(_tmp3FD,sizeof(char),7U);}):({const char*_tmp3FE="}";_tag_fat(_tmp3FE,sizeof(char),2U);});
struct _tuple10 _tmp3F4=Cyc_Absyn_aggr_kinded_name(info);struct _tuple10 _stmttmp1C=_tmp3F4;struct _tuple10 _tmp3F5=_stmttmp1C;struct _tuple0*_tmp3F6;_LL2E: _tmp3F6=_tmp3F5.f2;_LL2F: {struct _tuple0*n=_tmp3F6;
s=({struct Cyc_PP_Doc*_tmp3F7[4U];({struct Cyc_PP_Doc*_tmp767=Cyc_Absynpp_qvar2doc(n);_tmp3F7[0]=_tmp767;}),({struct Cyc_PP_Doc*_tmp766=Cyc_Absynpp_lb();_tmp3F7[1]=_tmp766;}),({
struct Cyc_PP_Doc*_tmp765=({struct _fat_ptr _tmp764=({const char*_tmp3F8="[";_tag_fat(_tmp3F8,sizeof(char),2U);});struct _fat_ptr _tmp763=({const char*_tmp3F9="]";_tag_fat(_tmp3F9,sizeof(char),2U);});struct _fat_ptr _tmp762=({const char*_tmp3FA=",";_tag_fat(_tmp3FA,sizeof(char),2U);});Cyc_PP_egroup(_tmp764,_tmp763,_tmp762,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp3F7[2]=_tmp765;}),({
struct Cyc_PP_Doc*_tmp761=({struct _fat_ptr _tmp760=({const char*_tmp3FB="";_tag_fat(_tmp3FB,sizeof(char),1U);});struct _fat_ptr _tmp75F=term;struct _fat_ptr _tmp75E=({const char*_tmp3FC=",";_tag_fat(_tmp3FC,sizeof(char),2U);});Cyc_PP_group(_tmp760,_tmp75F,_tmp75E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp3F7[3]=_tmp761;});Cyc_PP_cat(_tag_fat(_tmp3F7,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}}else{_LL21: _tmp3B3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_tmp3B4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3;_tmp3B5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f4;_LL22: {struct Cyc_List_List*exists=_tmp3B3;struct Cyc_List_List*dps=_tmp3B4;int dots=_tmp3B5;
# 1548
struct _fat_ptr term=dots?({const char*_tmp405=", ...}";_tag_fat(_tmp405,sizeof(char),7U);}):({const char*_tmp406="}";_tag_fat(_tmp406,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3FF[3U];({struct Cyc_PP_Doc*_tmp770=Cyc_Absynpp_lb();_tmp3FF[0]=_tmp770;}),({
struct Cyc_PP_Doc*_tmp76F=({struct _fat_ptr _tmp76E=({const char*_tmp400="[";_tag_fat(_tmp400,sizeof(char),2U);});struct _fat_ptr _tmp76D=({const char*_tmp401="]";_tag_fat(_tmp401,sizeof(char),2U);});struct _fat_ptr _tmp76C=({const char*_tmp402=",";_tag_fat(_tmp402,sizeof(char),2U);});Cyc_PP_egroup(_tmp76E,_tmp76D,_tmp76C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,exists));});_tmp3FF[1]=_tmp76F;}),({
struct Cyc_PP_Doc*_tmp76B=({struct _fat_ptr _tmp76A=({const char*_tmp403="";_tag_fat(_tmp403,sizeof(char),1U);});struct _fat_ptr _tmp769=term;struct _fat_ptr _tmp768=({const char*_tmp404=",";_tag_fat(_tmp404,sizeof(char),2U);});Cyc_PP_group(_tmp76A,_tmp769,_tmp768,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,dps));});_tmp3FF[2]=_tmp76B;});Cyc_PP_cat(_tag_fat(_tmp3FF,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp3B2=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_tmp3B2;
_tmp3B1=ef;goto _LL26;}case 14U: _LL25: _tmp3B1=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp3B1;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3 == 0){_LL27: _tmp3B0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL28: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3B0;
s=Cyc_Absynpp_qvar2doc(tuf->name);goto _LL0;}}else{_LL29: _tmp3AD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_tmp3AE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3;_tmp3AF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f4;_LL2A: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3AD;struct Cyc_List_List*ps=_tmp3AE;int dots=_tmp3AF;
# 1557
struct _fat_ptr term=dots?({const char*_tmp40A=", ...)";_tag_fat(_tmp40A,sizeof(char),7U);}):({const char*_tmp40B=")";_tag_fat(_tmp40B,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp407[2U];({struct Cyc_PP_Doc*_tmp775=Cyc_Absynpp_qvar2doc(tuf->name);_tmp407[0]=_tmp775;}),({struct Cyc_PP_Doc*_tmp774=({struct _fat_ptr _tmp773=({const char*_tmp408="(";_tag_fat(_tmp408,sizeof(char),2U);});struct _fat_ptr _tmp772=term;struct _fat_ptr _tmp771=({const char*_tmp409=",";_tag_fat(_tmp409,sizeof(char),2U);});Cyc_PP_egroup(_tmp773,_tmp772,_tmp771,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,ps));});_tmp407[1]=_tmp774;});Cyc_PP_cat(_tag_fat(_tmp407,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp3AC=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp3AC;
# 1561
s=Cyc_Absynpp_exp2doc(e);goto _LL0;}}_LL0:;}
# 1563
return s;}
# 1566
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple14*dp){
return({struct Cyc_PP_Doc*_tmp40C[2U];({struct Cyc_PP_Doc*_tmp77A=({struct _fat_ptr _tmp779=({const char*_tmp40D="";_tag_fat(_tmp40D,sizeof(char),1U);});struct _fat_ptr _tmp778=({const char*_tmp40E="=";_tag_fat(_tmp40E,sizeof(char),2U);});struct _fat_ptr _tmp777=({const char*_tmp40F="=";_tag_fat(_tmp40F,sizeof(char),2U);});Cyc_PP_egroup(_tmp779,_tmp778,_tmp777,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp40C[0]=_tmp77A;}),({
struct Cyc_PP_Doc*_tmp776=Cyc_Absynpp_pat2doc((*dp).f2);_tmp40C[1]=_tmp776;});Cyc_PP_cat(_tag_fat(_tmp40C,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1571
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1573
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp410[2U];({struct Cyc_PP_Doc*_tmp77D=Cyc_PP_text(({const char*_tmp411="default: ";_tag_fat(_tmp411,sizeof(char),10U);}));_tmp410[0]=_tmp77D;}),({
struct Cyc_PP_Doc*_tmp77C=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp412[2U];({struct Cyc_PP_Doc*_tmp77B=Cyc_PP_line_doc();_tmp412[0]=_tmp77B;}),_tmp412[1]=body;Cyc_PP_cat(_tag_fat(_tmp412,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp410[1]=_tmp77C;});Cyc_PP_cat(_tag_fat(_tmp410,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp413[4U];({struct Cyc_PP_Doc*_tmp782=Cyc_PP_text(({const char*_tmp414="case ";_tag_fat(_tmp414,sizeof(char),6U);}));_tmp413[0]=_tmp782;}),({
struct Cyc_PP_Doc*_tmp781=Cyc_Absynpp_pat2doc(c->pattern);_tmp413[1]=_tmp781;}),({
struct Cyc_PP_Doc*_tmp780=Cyc_PP_text(({const char*_tmp415=": ";_tag_fat(_tmp415,sizeof(char),3U);}));_tmp413[2]=_tmp780;}),({
struct Cyc_PP_Doc*_tmp77F=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp416[2U];({struct Cyc_PP_Doc*_tmp77E=Cyc_PP_line_doc();_tmp416[0]=_tmp77E;}),_tmp416[1]=body;Cyc_PP_cat(_tag_fat(_tmp416,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp413[3]=_tmp77F;});Cyc_PP_cat(_tag_fat(_tmp413,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1585
return({struct Cyc_PP_Doc*_tmp417[6U];({struct Cyc_PP_Doc*_tmp789=Cyc_PP_text(({const char*_tmp418="case ";_tag_fat(_tmp418,sizeof(char),6U);}));_tmp417[0]=_tmp789;}),({
struct Cyc_PP_Doc*_tmp788=Cyc_Absynpp_pat2doc(c->pattern);_tmp417[1]=_tmp788;}),({
struct Cyc_PP_Doc*_tmp787=Cyc_PP_text(({const char*_tmp419=" && ";_tag_fat(_tmp419,sizeof(char),5U);}));_tmp417[2]=_tmp787;}),({
struct Cyc_PP_Doc*_tmp786=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp417[3]=_tmp786;}),({
struct Cyc_PP_Doc*_tmp785=Cyc_PP_text(({const char*_tmp41A=": ";_tag_fat(_tmp41A,sizeof(char),3U);}));_tmp417[4]=_tmp785;}),({
struct Cyc_PP_Doc*_tmp784=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41B[2U];({struct Cyc_PP_Doc*_tmp783=Cyc_PP_line_doc();_tmp41B[0]=_tmp783;}),_tmp41B[1]=body;Cyc_PP_cat(_tag_fat(_tmp41B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp417[5]=_tmp784;});Cyc_PP_cat(_tag_fat(_tmp417,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1593
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _fat_ptr _tmp78A=({const char*_tmp41C="";_tag_fat(_tmp41C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp78A,cs);});}
# 1597
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1601
return({struct Cyc_PP_Doc*_tmp41D[3U];({struct Cyc_PP_Doc*_tmp78D=Cyc_Absynpp_qvar2doc(f->name);_tmp41D[0]=_tmp78D;}),({struct Cyc_PP_Doc*_tmp78C=Cyc_PP_text(({const char*_tmp41E=" = ";_tag_fat(_tmp41E,sizeof(char),4U);}));_tmp41D[1]=_tmp78C;}),({struct Cyc_PP_Doc*_tmp78B=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp41D[2]=_tmp78B;});Cyc_PP_cat(_tag_fat(_tmp41D,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1604
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _fat_ptr _tmp78E=({const char*_tmp41F=",";_tag_fat(_tmp41F,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp78E,fs);});}
# 1608
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1612
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _fat_ptr _tmp78F=({const char*_tmp420=",";_tag_fat(_tmp420,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp78F,vds);});}
# 1616
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp421=vd;struct Cyc_List_List*_tmp428;struct Cyc_Absyn_Exp*_tmp427;void*_tmp426;struct Cyc_Absyn_Tqual _tmp425;unsigned _tmp424;struct _tuple0*_tmp423;enum Cyc_Absyn_Scope _tmp422;_LL1: _tmp422=_tmp421->sc;_tmp423=_tmp421->name;_tmp424=_tmp421->varloc;_tmp425=_tmp421->tq;_tmp426=_tmp421->type;_tmp427=_tmp421->initializer;_tmp428=_tmp421->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp422;struct _tuple0*name=_tmp423;unsigned varloc=_tmp424;struct Cyc_Absyn_Tqual tq=_tmp425;void*type=_tmp426;struct Cyc_Absyn_Exp*initializer=_tmp427;struct Cyc_List_List*atts=_tmp428;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp429=Cyc_Cyclone_c_compiler;if(_tmp429 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1625
{void*_tmp42A=Cyc_Tcutil_compress(type);void*_stmttmp1D=_tmp42A;void*_tmp42B=_stmttmp1D;struct Cyc_List_List*_tmp42C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->tag == 5U){_LL9: _tmp42C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).attributes;_LLA: {struct Cyc_List_List*atts2=_tmp42C;
# 1627
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1631
goto _LL3;}_LL3:;}{
# 1634
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp42D=Cyc_Cyclone_c_compiler;if(_tmp42D == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1639
s=({struct Cyc_PP_Doc*_tmp42E[5U];_tmp42E[0]=tmp_doc,({
# 1641
struct Cyc_PP_Doc*_tmp799=Cyc_Absynpp_scope2doc(sc);_tmp42E[1]=_tmp799;}),({
struct Cyc_PP_Doc*_tmp798=({struct Cyc_Absyn_Tqual _tmp797=tq;void*_tmp796=type;Cyc_Absynpp_tqtd2doc(_tmp797,_tmp796,({struct Cyc_Core_Opt*_tmp430=_cycalloc(sizeof(*_tmp430));({struct Cyc_PP_Doc*_tmp795=({struct Cyc_PP_Doc*_tmp42F[2U];_tmp42F[0]=beforenamedoc,_tmp42F[1]=sn;Cyc_PP_cat(_tag_fat(_tmp42F,sizeof(struct Cyc_PP_Doc*),2U));});_tmp430->v=_tmp795;});_tmp430;}));});_tmp42E[2]=_tmp798;}),
initializer == 0?({
struct Cyc_PP_Doc*_tmp794=Cyc_PP_nil_doc();_tmp42E[3]=_tmp794;}):({
struct Cyc_PP_Doc*_tmp793=({struct Cyc_PP_Doc*_tmp431[2U];({struct Cyc_PP_Doc*_tmp792=Cyc_PP_text(({const char*_tmp432=" = ";_tag_fat(_tmp432,sizeof(char),4U);}));_tmp431[0]=_tmp792;}),({struct Cyc_PP_Doc*_tmp791=Cyc_Absynpp_exp2doc(initializer);_tmp431[1]=_tmp791;});Cyc_PP_cat(_tag_fat(_tmp431,sizeof(struct Cyc_PP_Doc*),2U));});_tmp42E[3]=_tmp793;}),({
struct Cyc_PP_Doc*_tmp790=Cyc_PP_text(({const char*_tmp433=";";_tag_fat(_tmp433,sizeof(char),2U);}));_tmp42E[4]=_tmp790;});Cyc_PP_cat(_tag_fat(_tmp42E,sizeof(struct Cyc_PP_Doc*),5U));});
return s;}}}struct _tuple18{unsigned f1;struct _tuple0*f2;int f3;};
# 1650
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple18*x){
struct _tuple18 _tmp434=*x;struct _tuple18 _stmttmp1E=_tmp434;struct _tuple18 _tmp435=_stmttmp1E;struct _tuple0*_tmp436;_LL1: _tmp436=_tmp435.f2;_LL2: {struct _tuple0*v=_tmp436;
return Cyc_Absynpp_qvar2doc(v);}}
# 1655
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp437[4U];({struct Cyc_PP_Doc*_tmp79D=Cyc_Absynpp_scope2doc(ad->sc);_tmp437[0]=_tmp79D;}),({
struct Cyc_PP_Doc*_tmp79C=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp437[1]=_tmp79C;}),({
struct Cyc_PP_Doc*_tmp79B=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp437[2]=_tmp79B;}),({
struct Cyc_PP_Doc*_tmp79A=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp437[3]=_tmp79A;});Cyc_PP_cat(_tag_fat(_tmp437,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1662
return({struct Cyc_PP_Doc*_tmp438[12U];({struct Cyc_PP_Doc*_tmp7AE=Cyc_Absynpp_scope2doc(ad->sc);_tmp438[0]=_tmp7AE;}),({
struct Cyc_PP_Doc*_tmp7AD=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp438[1]=_tmp7AD;}),({
struct Cyc_PP_Doc*_tmp7AC=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp438[2]=_tmp7AC;}),({
struct Cyc_PP_Doc*_tmp7AB=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp438[3]=_tmp7AB;}),({
struct Cyc_PP_Doc*_tmp7AA=Cyc_PP_blank_doc();_tmp438[4]=_tmp7AA;}),({struct Cyc_PP_Doc*_tmp7A9=Cyc_Absynpp_lb();_tmp438[5]=_tmp7A9;}),({
struct Cyc_PP_Doc*_tmp7A8=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp438[6]=_tmp7A8;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp7A7=Cyc_PP_nil_doc();_tmp438[7]=_tmp7A7;}):({
struct Cyc_PP_Doc*_tmp7A6=({struct Cyc_PP_Doc*_tmp439[2U];({struct Cyc_PP_Doc*_tmp7A5=Cyc_PP_text(({const char*_tmp43A=":";_tag_fat(_tmp43A,sizeof(char),2U);}));_tmp439[0]=_tmp7A5;}),({struct Cyc_PP_Doc*_tmp7A4=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp439[1]=_tmp7A4;});Cyc_PP_cat(_tag_fat(_tmp439,sizeof(struct Cyc_PP_Doc*),2U));});_tmp438[7]=_tmp7A6;}),({
struct Cyc_PP_Doc*_tmp7A3=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43B[2U];({struct Cyc_PP_Doc*_tmp7A2=Cyc_PP_line_doc();_tmp43B[0]=_tmp7A2;}),({struct Cyc_PP_Doc*_tmp7A1=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp43B[1]=_tmp7A1;});Cyc_PP_cat(_tag_fat(_tmp43B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp438[8]=_tmp7A3;}),({
struct Cyc_PP_Doc*_tmp7A0=Cyc_PP_line_doc();_tmp438[9]=_tmp7A0;}),({
struct Cyc_PP_Doc*_tmp79F=Cyc_Absynpp_rb();_tmp438[10]=_tmp79F;}),({
struct Cyc_PP_Doc*_tmp79E=Cyc_Absynpp_atts2doc(ad->attributes);_tmp438[11]=_tmp79E;});Cyc_PP_cat(_tag_fat(_tmp438,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1676
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp43C=dd;int _tmp441;struct Cyc_Core_Opt*_tmp440;struct Cyc_List_List*_tmp43F;struct _tuple0*_tmp43E;enum Cyc_Absyn_Scope _tmp43D;_LL1: _tmp43D=_tmp43C->sc;_tmp43E=_tmp43C->name;_tmp43F=_tmp43C->tvs;_tmp440=_tmp43C->fields;_tmp441=_tmp43C->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp43D;struct _tuple0*name=_tmp43E;struct Cyc_List_List*tvs=_tmp43F;struct Cyc_Core_Opt*fields=_tmp440;int is_x=_tmp441;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp442[5U];({struct Cyc_PP_Doc*_tmp7B5=Cyc_Absynpp_scope2doc(sc);_tmp442[0]=_tmp7B5;}),
is_x?({struct Cyc_PP_Doc*_tmp7B4=Cyc_PP_text(({const char*_tmp443="@extensible ";_tag_fat(_tmp443,sizeof(char),13U);}));_tmp442[1]=_tmp7B4;}):({struct Cyc_PP_Doc*_tmp7B3=Cyc_PP_blank_doc();_tmp442[1]=_tmp7B3;}),({
struct Cyc_PP_Doc*_tmp7B2=Cyc_PP_text(({const char*_tmp444="datatype ";_tag_fat(_tmp444,sizeof(char),10U);}));_tmp442[2]=_tmp7B2;}),
is_x?({struct Cyc_PP_Doc*_tmp7B1=Cyc_Absynpp_qvar2bolddoc(name);_tmp442[3]=_tmp7B1;}):({struct Cyc_PP_Doc*_tmp7B0=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp442[3]=_tmp7B0;}),({
struct Cyc_PP_Doc*_tmp7AF=Cyc_Absynpp_ktvars2doc(tvs);_tmp442[4]=_tmp7AF;});Cyc_PP_cat(_tag_fat(_tmp442,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1685
return({struct Cyc_PP_Doc*_tmp445[10U];({struct Cyc_PP_Doc*_tmp7C3=Cyc_Absynpp_scope2doc(sc);_tmp445[0]=_tmp7C3;}),
is_x?({struct Cyc_PP_Doc*_tmp7C2=Cyc_PP_text(({const char*_tmp446="@extensible ";_tag_fat(_tmp446,sizeof(char),13U);}));_tmp445[1]=_tmp7C2;}):({struct Cyc_PP_Doc*_tmp7C1=Cyc_PP_blank_doc();_tmp445[1]=_tmp7C1;}),({
struct Cyc_PP_Doc*_tmp7C0=Cyc_PP_text(({const char*_tmp447="datatype ";_tag_fat(_tmp447,sizeof(char),10U);}));_tmp445[2]=_tmp7C0;}),
is_x?({struct Cyc_PP_Doc*_tmp7BF=Cyc_Absynpp_qvar2bolddoc(name);_tmp445[3]=_tmp7BF;}):({struct Cyc_PP_Doc*_tmp7BE=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp445[3]=_tmp7BE;}),({
struct Cyc_PP_Doc*_tmp7BD=Cyc_Absynpp_ktvars2doc(tvs);_tmp445[4]=_tmp7BD;}),({
struct Cyc_PP_Doc*_tmp7BC=Cyc_PP_blank_doc();_tmp445[5]=_tmp7BC;}),({struct Cyc_PP_Doc*_tmp7BB=Cyc_Absynpp_lb();_tmp445[6]=_tmp7BB;}),({
struct Cyc_PP_Doc*_tmp7BA=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp448[2U];({struct Cyc_PP_Doc*_tmp7B9=Cyc_PP_line_doc();_tmp448[0]=_tmp7B9;}),({struct Cyc_PP_Doc*_tmp7B8=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_tmp448[1]=_tmp7B8;});Cyc_PP_cat(_tag_fat(_tmp448,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp445[7]=_tmp7BA;}),({
struct Cyc_PP_Doc*_tmp7B7=Cyc_PP_line_doc();_tmp445[8]=_tmp7B7;}),({
struct Cyc_PP_Doc*_tmp7B6=Cyc_Absynpp_rb();_tmp445[9]=_tmp7B6;});Cyc_PP_cat(_tag_fat(_tmp445,sizeof(struct Cyc_PP_Doc*),10U));});}}}
# 1696
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp449=ed;struct Cyc_Core_Opt*_tmp44C;struct _tuple0*_tmp44B;enum Cyc_Absyn_Scope _tmp44A;_LL1: _tmp44A=_tmp449->sc;_tmp44B=_tmp449->name;_tmp44C=_tmp449->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp44A;struct _tuple0*n=_tmp44B;struct Cyc_Core_Opt*fields=_tmp44C;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp44D[3U];({struct Cyc_PP_Doc*_tmp7C6=Cyc_Absynpp_scope2doc(sc);_tmp44D[0]=_tmp7C6;}),({
struct Cyc_PP_Doc*_tmp7C5=Cyc_PP_text(({const char*_tmp44E="enum ";_tag_fat(_tmp44E,sizeof(char),6U);}));_tmp44D[1]=_tmp7C5;}),({
struct Cyc_PP_Doc*_tmp7C4=Cyc_Absynpp_typedef_name2bolddoc(n);_tmp44D[2]=_tmp7C4;});Cyc_PP_cat(_tag_fat(_tmp44D,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1704
return({struct Cyc_PP_Doc*_tmp44F[8U];({struct Cyc_PP_Doc*_tmp7D0=Cyc_Absynpp_scope2doc(sc);_tmp44F[0]=_tmp7D0;}),({
struct Cyc_PP_Doc*_tmp7CF=Cyc_PP_text(({const char*_tmp450="enum ";_tag_fat(_tmp450,sizeof(char),6U);}));_tmp44F[1]=_tmp7CF;}),({
struct Cyc_PP_Doc*_tmp7CE=Cyc_Absynpp_qvar2bolddoc(n);_tmp44F[2]=_tmp7CE;}),({
struct Cyc_PP_Doc*_tmp7CD=Cyc_PP_blank_doc();_tmp44F[3]=_tmp7CD;}),({struct Cyc_PP_Doc*_tmp7CC=Cyc_Absynpp_lb();_tmp44F[4]=_tmp7CC;}),({
struct Cyc_PP_Doc*_tmp7CB=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp451[2U];({struct Cyc_PP_Doc*_tmp7CA=Cyc_PP_line_doc();_tmp451[0]=_tmp7CA;}),({struct Cyc_PP_Doc*_tmp7C9=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_tmp451[1]=_tmp7C9;});Cyc_PP_cat(_tag_fat(_tmp451,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp44F[5]=_tmp7CB;}),({
struct Cyc_PP_Doc*_tmp7C8=Cyc_PP_line_doc();_tmp44F[6]=_tmp7C8;}),({
struct Cyc_PP_Doc*_tmp7C7=Cyc_Absynpp_rb();_tmp44F[7]=_tmp7C7;});Cyc_PP_cat(_tag_fat(_tmp44F,sizeof(struct Cyc_PP_Doc*),8U));});}}}
# 1713
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp452=d->r;void*_stmttmp1F=_tmp452;void*_tmp453=_stmttmp1F;struct Cyc_List_List*_tmp456;struct Cyc_List_List*_tmp455;struct Cyc_List_List*_tmp454;struct Cyc_List_List*_tmp457;struct Cyc_List_List*_tmp459;struct _tuple0*_tmp458;struct Cyc_List_List*_tmp45B;struct _fat_ptr*_tmp45A;struct Cyc_Absyn_Typedefdecl*_tmp45C;struct Cyc_Absyn_Enumdecl*_tmp45D;struct Cyc_List_List*_tmp45E;struct Cyc_Absyn_Exp*_tmp460;struct Cyc_Absyn_Pat*_tmp45F;struct Cyc_Absyn_Datatypedecl*_tmp461;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Absyn_Vardecl*_tmp463;struct Cyc_Absyn_Tvar*_tmp462;struct Cyc_Absyn_Vardecl*_tmp465;struct Cyc_Absyn_Aggrdecl*_tmp466;struct Cyc_Absyn_Fndecl*_tmp467;switch(*((int*)_tmp453)){case 1U: _LL1: _tmp467=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp467;
# 1717
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->tag=5U,_tmp478->f1=type_info;_tmp478;});
if(fd->cached_type != 0){
void*_tmp468=Cyc_Tcutil_compress((void*)_check_null(fd->cached_type));void*_stmttmp20=_tmp468;void*_tmp469=_stmttmp20;struct Cyc_Absyn_FnInfo _tmp46A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp469)->tag == 5U){_LL24: _tmp46A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp469)->f1;_LL25: {struct Cyc_Absyn_FnInfo i=_tmp46A;
# 1723
({struct Cyc_List_List*_tmp7D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp7D1;});goto _LL23;}}else{_LL26: _LL27:
({void*_tmp46B=0U;({struct _fat_ptr _tmp7D2=({const char*_tmp46C="function has non-function type";_tag_fat(_tmp46C,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp7D2,_tag_fat(_tmp46B,sizeof(void*),0U));});});}_LL23:;}{
# 1726
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((fd->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp46D=Cyc_Cyclone_c_compiler;if(_tmp46D == Cyc_Cyclone_Gcc_c){_LL29: _LL2A:
 inlinedoc=Cyc_PP_text(({const char*_tmp46E="inline ";_tag_fat(_tmp46E,sizeof(char),8U);}));goto _LL28;}else{_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp46F="__inline ";_tag_fat(_tmp46F,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1734
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp470=Cyc_Cyclone_c_compiler;if(_tmp470 == Cyc_Cyclone_Gcc_c){_LL2E: _LL2F:
 beforenamedoc=attsdoc;goto _LL2D;}else{_LL30: _LL31:
 beforenamedoc=Cyc_Absynpp_callconv2doc((fd->i).attributes);goto _LL2D;}_LL2D:;}{
# 1741
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp7D5=Cyc_Absyn_empty_tqual(0U);void*_tmp7D4=t;Cyc_Absynpp_tqtd2doc(_tmp7D5,_tmp7D4,({struct Cyc_Core_Opt*_tmp477=_cycalloc(sizeof(*_tmp477));({
struct Cyc_PP_Doc*_tmp7D3=({struct Cyc_PP_Doc*_tmp476[2U];_tmp476[0]=beforenamedoc,_tmp476[1]=namedoc;Cyc_PP_cat(_tag_fat(_tmp476,sizeof(struct Cyc_PP_Doc*),2U));});_tmp477->v=_tmp7D3;});_tmp477;}));});
# 1749
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp474[5U];({struct Cyc_PP_Doc*_tmp7DC=Cyc_PP_blank_doc();_tmp474[0]=_tmp7DC;}),({struct Cyc_PP_Doc*_tmp7DB=Cyc_Absynpp_lb();_tmp474[1]=_tmp7DB;}),({
struct Cyc_PP_Doc*_tmp7DA=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp475[2U];({struct Cyc_PP_Doc*_tmp7D9=Cyc_PP_line_doc();_tmp475[0]=_tmp7D9;}),({struct Cyc_PP_Doc*_tmp7D8=Cyc_Absynpp_stmt2doc(fd->body,0,0);_tmp475[1]=_tmp7D8;});Cyc_PP_cat(_tag_fat(_tmp475,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp474[2]=_tmp7DA;}),({
struct Cyc_PP_Doc*_tmp7D7=Cyc_PP_line_doc();_tmp474[3]=_tmp7D7;}),({
struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_rb();_tmp474[4]=_tmp7D6;});Cyc_PP_cat(_tag_fat(_tmp474,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp471[4U];_tmp471[0]=inlinedoc,_tmp471[1]=scopedoc,_tmp471[2]=tqtddoc,_tmp471[3]=bodydoc;Cyc_PP_cat(_tag_fat(_tmp471,sizeof(struct Cyc_PP_Doc*),4U));});
# 1755
{enum Cyc_Cyclone_C_Compilers _tmp472=Cyc_Cyclone_c_compiler;if(_tmp472 == Cyc_Cyclone_Vc_c){_LL33: _LL34:
 s=({struct Cyc_PP_Doc*_tmp473[2U];_tmp473[0]=attsdoc,_tmp473[1]=s;Cyc_PP_cat(_tag_fat(_tmp473,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 1760
goto _LL0;}}}}}case 5U: _LL3: _tmp466=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp466;
# 1763
s=({struct Cyc_PP_Doc*_tmp479[2U];({struct Cyc_PP_Doc*_tmp7DE=Cyc_Absynpp_aggrdecl2doc(ad);_tmp479[0]=_tmp7DE;}),({struct Cyc_PP_Doc*_tmp7DD=Cyc_PP_text(({const char*_tmp47A=";";_tag_fat(_tmp47A,sizeof(char),2U);}));_tmp479[1]=_tmp7DD;});Cyc_PP_cat(_tag_fat(_tmp479,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 0U: _LL5: _tmp465=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp465;
# 1766
s=Cyc_Absynpp_vardecl2doc(vd);
goto _LL0;}case 4U: _LL7: _tmp462=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp463=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_tmp464=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp462;struct Cyc_Absyn_Vardecl*vd=_tmp463;struct Cyc_Absyn_Exp*open_exp_opt=_tmp464;
# 1769
s=({struct Cyc_PP_Doc*_tmp47B[7U];({struct Cyc_PP_Doc*_tmp7E9=Cyc_PP_text(({const char*_tmp47C="region";_tag_fat(_tmp47C,sizeof(char),7U);}));_tmp47B[0]=_tmp7E9;}),({
struct Cyc_PP_Doc*_tmp7E8=Cyc_PP_text(({const char*_tmp47D="<";_tag_fat(_tmp47D,sizeof(char),2U);}));_tmp47B[1]=_tmp7E8;}),({
struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_tvar2doc(tv);_tmp47B[2]=_tmp7E7;}),({
struct Cyc_PP_Doc*_tmp7E6=Cyc_PP_text(({const char*_tmp47E=">";_tag_fat(_tmp47E,sizeof(char),2U);}));_tmp47B[3]=_tmp7E6;}),({
struct Cyc_PP_Doc*_tmp7E5=Cyc_Absynpp_qvar2doc(vd->name);_tmp47B[4]=_tmp7E5;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_tmp7E4=({struct Cyc_PP_Doc*_tmp47F[3U];({struct Cyc_PP_Doc*_tmp7E3=Cyc_PP_text(({const char*_tmp480=" = open(";_tag_fat(_tmp480,sizeof(char),9U);}));_tmp47F[0]=_tmp7E3;}),({struct Cyc_PP_Doc*_tmp7E2=Cyc_Absynpp_exp2doc(open_exp_opt);_tmp47F[1]=_tmp7E2;}),({
struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_text(({const char*_tmp481=")";_tag_fat(_tmp481,sizeof(char),2U);}));_tmp47F[2]=_tmp7E1;});Cyc_PP_cat(_tag_fat(_tmp47F,sizeof(struct Cyc_PP_Doc*),3U));});
# 1774
_tmp47B[5]=_tmp7E4;}):({
struct Cyc_PP_Doc*_tmp7E0=Cyc_PP_nil_doc();_tmp47B[5]=_tmp7E0;}),({
struct Cyc_PP_Doc*_tmp7DF=Cyc_PP_text(({const char*_tmp482=";";_tag_fat(_tmp482,sizeof(char),2U);}));_tmp47B[6]=_tmp7DF;});Cyc_PP_cat(_tag_fat(_tmp47B,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}case 6U: _LL9: _tmp461=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LLA: {struct Cyc_Absyn_Datatypedecl*dd=_tmp461;
# 1779
s=({struct Cyc_PP_Doc*_tmp483[2U];({struct Cyc_PP_Doc*_tmp7EB=Cyc_Absynpp_datatypedecl2doc(dd);_tmp483[0]=_tmp7EB;}),({struct Cyc_PP_Doc*_tmp7EA=Cyc_PP_text(({const char*_tmp484=";";_tag_fat(_tmp484,sizeof(char),2U);}));_tmp483[1]=_tmp7EA;});Cyc_PP_cat(_tag_fat(_tmp483,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 2U: _LLB: _tmp45F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp460=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LLC: {struct Cyc_Absyn_Pat*p=_tmp45F;struct Cyc_Absyn_Exp*e=_tmp460;
# 1782
s=({struct Cyc_PP_Doc*_tmp485[5U];({struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_text(({const char*_tmp486="let ";_tag_fat(_tmp486,sizeof(char),5U);}));_tmp485[0]=_tmp7F0;}),({
struct Cyc_PP_Doc*_tmp7EF=Cyc_Absynpp_pat2doc(p);_tmp485[1]=_tmp7EF;}),({
struct Cyc_PP_Doc*_tmp7EE=Cyc_PP_text(({const char*_tmp487=" = ";_tag_fat(_tmp487,sizeof(char),4U);}));_tmp485[2]=_tmp7EE;}),({
struct Cyc_PP_Doc*_tmp7ED=Cyc_Absynpp_exp2doc(e);_tmp485[3]=_tmp7ED;}),({
struct Cyc_PP_Doc*_tmp7EC=Cyc_PP_text(({const char*_tmp488=";";_tag_fat(_tmp488,sizeof(char),2U);}));_tmp485[4]=_tmp7EC;});Cyc_PP_cat(_tag_fat(_tmp485,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;}case 3U: _LLD: _tmp45E=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LLE: {struct Cyc_List_List*vds=_tmp45E;
# 1789
s=({struct Cyc_PP_Doc*_tmp489[3U];({struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_text(({const char*_tmp48A="let ";_tag_fat(_tmp48A,sizeof(char),5U);}));_tmp489[0]=_tmp7F3;}),({struct Cyc_PP_Doc*_tmp7F2=Cyc_Absynpp_ids2doc(vds);_tmp489[1]=_tmp7F2;}),({struct Cyc_PP_Doc*_tmp7F1=Cyc_PP_text(({const char*_tmp48B=";";_tag_fat(_tmp48B,sizeof(char),2U);}));_tmp489[2]=_tmp7F1;});Cyc_PP_cat(_tag_fat(_tmp489,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 7U: _LLF: _tmp45D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp45D;
# 1792
s=({struct Cyc_PP_Doc*_tmp48C[2U];({struct Cyc_PP_Doc*_tmp7F5=Cyc_Absynpp_enumdecl2doc(ed);_tmp48C[0]=_tmp7F5;}),({struct Cyc_PP_Doc*_tmp7F4=Cyc_PP_text(({const char*_tmp48D=";";_tag_fat(_tmp48D,sizeof(char),2U);}));_tmp48C[1]=_tmp7F4;});Cyc_PP_cat(_tag_fat(_tmp48C,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 8U: _LL11: _tmp45C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp45C;
# 1795
void*t;
if(td->defn != 0)
t=(void*)_check_null(td->defn);else{
# 1799
t=Cyc_Absyn_new_evar(td->kind,0);}
s=({struct Cyc_PP_Doc*_tmp48E[4U];({struct Cyc_PP_Doc*_tmp7FE=Cyc_PP_text(({const char*_tmp48F="typedef ";_tag_fat(_tmp48F,sizeof(char),9U);}));_tmp48E[0]=_tmp7FE;}),({
struct Cyc_PP_Doc*_tmp7FD=({struct Cyc_Absyn_Tqual _tmp7FC=td->tq;void*_tmp7FB=t;Cyc_Absynpp_tqtd2doc(_tmp7FC,_tmp7FB,({struct Cyc_Core_Opt*_tmp491=_cycalloc(sizeof(*_tmp491));({
# 1803
struct Cyc_PP_Doc*_tmp7FA=({struct Cyc_PP_Doc*_tmp490[2U];({struct Cyc_PP_Doc*_tmp7F9=Cyc_Absynpp_typedef_name2bolddoc(td->name);_tmp490[0]=_tmp7F9;}),({
struct Cyc_PP_Doc*_tmp7F8=Cyc_Absynpp_tvars2doc(td->tvs);_tmp490[1]=_tmp7F8;});Cyc_PP_cat(_tag_fat(_tmp490,sizeof(struct Cyc_PP_Doc*),2U));});
# 1803
_tmp491->v=_tmp7FA;});_tmp491;}));});
# 1801
_tmp48E[1]=_tmp7FD;}),({
# 1806
struct Cyc_PP_Doc*_tmp7F7=Cyc_Absynpp_atts2doc(td->atts);_tmp48E[2]=_tmp7F7;}),({
struct Cyc_PP_Doc*_tmp7F6=Cyc_PP_text(({const char*_tmp492=";";_tag_fat(_tmp492,sizeof(char),2U);}));_tmp48E[3]=_tmp7F6;});Cyc_PP_cat(_tag_fat(_tmp48E,sizeof(struct Cyc_PP_Doc*),4U));});
# 1809
goto _LL0;}case 9U: _LL13: _tmp45A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp45B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL14: {struct _fat_ptr*v=_tmp45A;struct Cyc_List_List*tdl=_tmp45B;
# 1811
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_tmp493[8U];({struct Cyc_PP_Doc*_tmp807=Cyc_PP_text(({const char*_tmp494="namespace ";_tag_fat(_tmp494,sizeof(char),11U);}));_tmp493[0]=_tmp807;}),({
struct Cyc_PP_Doc*_tmp806=Cyc_PP_textptr(v);_tmp493[1]=_tmp806;}),({
struct Cyc_PP_Doc*_tmp805=Cyc_PP_blank_doc();_tmp493[2]=_tmp805;}),({struct Cyc_PP_Doc*_tmp804=Cyc_Absynpp_lb();_tmp493[3]=_tmp804;}),({
struct Cyc_PP_Doc*_tmp803=Cyc_PP_line_doc();_tmp493[4]=_tmp803;}),({
struct Cyc_PP_Doc*_tmp802=({struct _fat_ptr _tmp801=({const char*_tmp495="";_tag_fat(_tmp495,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp801,tdl);});_tmp493[5]=_tmp802;}),({
struct Cyc_PP_Doc*_tmp800=Cyc_PP_line_doc();_tmp493[6]=_tmp800;}),({
struct Cyc_PP_Doc*_tmp7FF=Cyc_Absynpp_rb();_tmp493[7]=_tmp7FF;});Cyc_PP_cat(_tag_fat(_tmp493,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10U: _LL15: _tmp458=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp459=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL16: {struct _tuple0*q=_tmp458;struct Cyc_List_List*tdl=_tmp459;
# 1822
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp496[8U];({struct Cyc_PP_Doc*_tmp810=Cyc_PP_text(({const char*_tmp497="using ";_tag_fat(_tmp497,sizeof(char),7U);}));_tmp496[0]=_tmp810;}),({
struct Cyc_PP_Doc*_tmp80F=Cyc_Absynpp_qvar2doc(q);_tmp496[1]=_tmp80F;}),({
struct Cyc_PP_Doc*_tmp80E=Cyc_PP_blank_doc();_tmp496[2]=_tmp80E;}),({struct Cyc_PP_Doc*_tmp80D=Cyc_Absynpp_lb();_tmp496[3]=_tmp80D;}),({
struct Cyc_PP_Doc*_tmp80C=Cyc_PP_line_doc();_tmp496[4]=_tmp80C;}),({
struct Cyc_PP_Doc*_tmp80B=({struct _fat_ptr _tmp80A=({const char*_tmp498="";_tag_fat(_tmp498,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp80A,tdl);});_tmp496[5]=_tmp80B;}),({
struct Cyc_PP_Doc*_tmp809=Cyc_PP_line_doc();_tmp496[6]=_tmp809;}),({
struct Cyc_PP_Doc*_tmp808=Cyc_Absynpp_rb();_tmp496[7]=_tmp808;});Cyc_PP_cat(_tag_fat(_tmp496,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1831
s=({struct Cyc_PP_Doc*_tmp499[11U];({struct Cyc_PP_Doc*_tmp81C=Cyc_PP_text(({const char*_tmp49A="/* using ";_tag_fat(_tmp49A,sizeof(char),10U);}));_tmp499[0]=_tmp81C;}),({
struct Cyc_PP_Doc*_tmp81B=Cyc_Absynpp_qvar2doc(q);_tmp499[1]=_tmp81B;}),({
struct Cyc_PP_Doc*_tmp81A=Cyc_PP_blank_doc();_tmp499[2]=_tmp81A;}),({
struct Cyc_PP_Doc*_tmp819=Cyc_Absynpp_lb();_tmp499[3]=_tmp819;}),({
struct Cyc_PP_Doc*_tmp818=Cyc_PP_text(({const char*_tmp49B=" */";_tag_fat(_tmp49B,sizeof(char),4U);}));_tmp499[4]=_tmp818;}),({
struct Cyc_PP_Doc*_tmp817=Cyc_PP_line_doc();_tmp499[5]=_tmp817;}),({
struct Cyc_PP_Doc*_tmp816=({struct _fat_ptr _tmp815=({const char*_tmp49C="";_tag_fat(_tmp49C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp815,tdl);});_tmp499[6]=_tmp816;}),({
struct Cyc_PP_Doc*_tmp814=Cyc_PP_line_doc();_tmp499[7]=_tmp814;}),({
struct Cyc_PP_Doc*_tmp813=Cyc_PP_text(({const char*_tmp49D="/* ";_tag_fat(_tmp49D,sizeof(char),4U);}));_tmp499[8]=_tmp813;}),({
struct Cyc_PP_Doc*_tmp812=Cyc_Absynpp_rb();_tmp499[9]=_tmp812;}),({
struct Cyc_PP_Doc*_tmp811=Cyc_PP_text(({const char*_tmp49E=" */";_tag_fat(_tmp49E,sizeof(char),4U);}));_tmp499[10]=_tmp811;});Cyc_PP_cat(_tag_fat(_tmp499,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;}case 11U: _LL17: _tmp457=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL18: {struct Cyc_List_List*tdl=_tmp457;
# 1844
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp49F[6U];({struct Cyc_PP_Doc*_tmp823=Cyc_PP_text(({const char*_tmp4A0="extern \"C\" ";_tag_fat(_tmp4A0,sizeof(char),12U);}));_tmp49F[0]=_tmp823;}),({
struct Cyc_PP_Doc*_tmp822=Cyc_Absynpp_lb();_tmp49F[1]=_tmp822;}),({
struct Cyc_PP_Doc*_tmp821=Cyc_PP_line_doc();_tmp49F[2]=_tmp821;}),({
struct Cyc_PP_Doc*_tmp820=({struct _fat_ptr _tmp81F=({const char*_tmp4A1="";_tag_fat(_tmp4A1,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp81F,tdl);});_tmp49F[3]=_tmp820;}),({
struct Cyc_PP_Doc*_tmp81E=Cyc_PP_line_doc();_tmp49F[4]=_tmp81E;}),({
struct Cyc_PP_Doc*_tmp81D=Cyc_Absynpp_rb();_tmp49F[5]=_tmp81D;});Cyc_PP_cat(_tag_fat(_tmp49F,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1852
s=({struct Cyc_PP_Doc*_tmp4A2[9U];({struct Cyc_PP_Doc*_tmp82D=Cyc_PP_text(({const char*_tmp4A3="/* extern \"C\" ";_tag_fat(_tmp4A3,sizeof(char),15U);}));_tmp4A2[0]=_tmp82D;}),({
struct Cyc_PP_Doc*_tmp82C=Cyc_Absynpp_lb();_tmp4A2[1]=_tmp82C;}),({
struct Cyc_PP_Doc*_tmp82B=Cyc_PP_text(({const char*_tmp4A4=" */";_tag_fat(_tmp4A4,sizeof(char),4U);}));_tmp4A2[2]=_tmp82B;}),({
struct Cyc_PP_Doc*_tmp82A=Cyc_PP_line_doc();_tmp4A2[3]=_tmp82A;}),({
struct Cyc_PP_Doc*_tmp829=({struct _fat_ptr _tmp828=({const char*_tmp4A5="";_tag_fat(_tmp4A5,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp828,tdl);});_tmp4A2[4]=_tmp829;}),({
struct Cyc_PP_Doc*_tmp827=Cyc_PP_line_doc();_tmp4A2[5]=_tmp827;}),({
struct Cyc_PP_Doc*_tmp826=Cyc_PP_text(({const char*_tmp4A6="/* ";_tag_fat(_tmp4A6,sizeof(char),4U);}));_tmp4A2[6]=_tmp826;}),({
struct Cyc_PP_Doc*_tmp825=Cyc_Absynpp_rb();_tmp4A2[7]=_tmp825;}),({
struct Cyc_PP_Doc*_tmp824=Cyc_PP_text(({const char*_tmp4A7=" */";_tag_fat(_tmp4A7,sizeof(char),4U);}));_tmp4A2[8]=_tmp824;});Cyc_PP_cat(_tag_fat(_tmp4A2,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 12U: _LL19: _tmp454=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp455=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_tmp456=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LL1A: {struct Cyc_List_List*tdl=_tmp454;struct Cyc_List_List*ovrs=_tmp455;struct Cyc_List_List*exs=_tmp456;
# 1863
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp4A8[7U];({struct Cyc_PP_Doc*_tmp835=Cyc_Absynpp_rb();_tmp4A8[0]=_tmp835;}),({struct Cyc_PP_Doc*_tmp834=Cyc_PP_text(({const char*_tmp4A9=" export ";_tag_fat(_tmp4A9,sizeof(char),9U);}));_tmp4A8[1]=_tmp834;}),({struct Cyc_PP_Doc*_tmp833=Cyc_Absynpp_lb();_tmp4A8[2]=_tmp833;}),({
struct Cyc_PP_Doc*_tmp832=Cyc_PP_line_doc();_tmp4A8[3]=_tmp832;}),({struct Cyc_PP_Doc*_tmp831=({struct _fat_ptr _tmp830=({const char*_tmp4AA=",";_tag_fat(_tmp4AA,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple18*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp830,exs);});_tmp4A8[4]=_tmp831;}),({
struct Cyc_PP_Doc*_tmp82F=Cyc_PP_line_doc();_tmp4A8[5]=_tmp82F;}),({struct Cyc_PP_Doc*_tmp82E=Cyc_Absynpp_rb();_tmp4A8[6]=_tmp82E;});Cyc_PP_cat(_tag_fat(_tmp4A8,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1871
exs_doc=Cyc_Absynpp_rb();}
if(ovrs != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp4AB[7U];({struct Cyc_PP_Doc*_tmp83D=Cyc_Absynpp_rb();_tmp4AB[0]=_tmp83D;}),({struct Cyc_PP_Doc*_tmp83C=Cyc_PP_text(({const char*_tmp4AC=" cycdef ";_tag_fat(_tmp4AC,sizeof(char),9U);}));_tmp4AB[1]=_tmp83C;}),({struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_lb();_tmp4AB[2]=_tmp83B;}),({
struct Cyc_PP_Doc*_tmp83A=Cyc_PP_line_doc();_tmp4AB[3]=_tmp83A;}),({struct Cyc_PP_Doc*_tmp839=({struct _fat_ptr _tmp838=({const char*_tmp4AD="";_tag_fat(_tmp4AD,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp838,ovrs);});_tmp4AB[4]=_tmp839;}),({
struct Cyc_PP_Doc*_tmp837=Cyc_PP_line_doc();_tmp4AB[5]=_tmp837;}),({struct Cyc_PP_Doc*_tmp836=Cyc_Absynpp_rb();_tmp4AB[6]=_tmp836;});Cyc_PP_cat(_tag_fat(_tmp4AB,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1877
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp4AE[6U];({struct Cyc_PP_Doc*_tmp843=Cyc_PP_text(({const char*_tmp4AF="extern \"C include\" ";_tag_fat(_tmp4AF,sizeof(char),20U);}));_tmp4AE[0]=_tmp843;}),({
struct Cyc_PP_Doc*_tmp842=Cyc_Absynpp_lb();_tmp4AE[1]=_tmp842;}),({
struct Cyc_PP_Doc*_tmp841=Cyc_PP_line_doc();_tmp4AE[2]=_tmp841;}),({
struct Cyc_PP_Doc*_tmp840=({struct _fat_ptr _tmp83F=({const char*_tmp4B0="";_tag_fat(_tmp4B0,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp83F,tdl);});_tmp4AE[3]=_tmp840;}),({
struct Cyc_PP_Doc*_tmp83E=Cyc_PP_line_doc();_tmp4AE[4]=_tmp83E;}),_tmp4AE[5]=exs_doc;Cyc_PP_cat(_tag_fat(_tmp4AE,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1885
s=({struct Cyc_PP_Doc*_tmp4B1[9U];({struct Cyc_PP_Doc*_tmp84D=Cyc_PP_text(({const char*_tmp4B2="/* extern \"C include\" ";_tag_fat(_tmp4B2,sizeof(char),23U);}));_tmp4B1[0]=_tmp84D;}),({
struct Cyc_PP_Doc*_tmp84C=Cyc_Absynpp_lb();_tmp4B1[1]=_tmp84C;}),({
struct Cyc_PP_Doc*_tmp84B=Cyc_PP_text(({const char*_tmp4B3=" */";_tag_fat(_tmp4B3,sizeof(char),4U);}));_tmp4B1[2]=_tmp84B;}),({
struct Cyc_PP_Doc*_tmp84A=Cyc_PP_line_doc();_tmp4B1[3]=_tmp84A;}),({
struct Cyc_PP_Doc*_tmp849=({struct _fat_ptr _tmp848=({const char*_tmp4B4="";_tag_fat(_tmp4B4,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp848,tdl);});_tmp4B1[4]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp847=Cyc_PP_line_doc();_tmp4B1[5]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp846=Cyc_PP_text(({const char*_tmp4B5="/* ";_tag_fat(_tmp4B5,sizeof(char),4U);}));_tmp4B1[6]=_tmp846;}),({
struct Cyc_PP_Doc*_tmp845=Cyc_Absynpp_rb();_tmp4B1[7]=_tmp845;}),({
struct Cyc_PP_Doc*_tmp844=Cyc_PP_text(({const char*_tmp4B6=" */";_tag_fat(_tmp4B6,sizeof(char),4U);}));_tmp4B1[8]=_tmp844;});Cyc_PP_cat(_tag_fat(_tmp4B1,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;}case 13U: _LL1B: _LL1C:
# 1896
 s=({struct Cyc_PP_Doc*_tmp4B7[2U];({struct Cyc_PP_Doc*_tmp84F=Cyc_PP_text(({const char*_tmp4B8="__cyclone_port_on__;";_tag_fat(_tmp4B8,sizeof(char),21U);}));_tmp4B7[0]=_tmp84F;}),({struct Cyc_PP_Doc*_tmp84E=Cyc_Absynpp_lb();_tmp4B7[1]=_tmp84E;});Cyc_PP_cat(_tag_fat(_tmp4B7,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1899
 s=({struct Cyc_PP_Doc*_tmp4B9[2U];({struct Cyc_PP_Doc*_tmp851=Cyc_PP_text(({const char*_tmp4BA="__cyclone_port_off__;";_tag_fat(_tmp4BA,sizeof(char),22U);}));_tmp4B9[0]=_tmp851;}),({struct Cyc_PP_Doc*_tmp850=Cyc_Absynpp_lb();_tmp4B9[1]=_tmp850;});Cyc_PP_cat(_tag_fat(_tmp4B9,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1902
 s=({struct Cyc_PP_Doc*_tmp4BB[2U];({struct Cyc_PP_Doc*_tmp853=Cyc_PP_text(({const char*_tmp4BC="__tempest_on__;";_tag_fat(_tmp4BC,sizeof(char),16U);}));_tmp4BB[0]=_tmp853;}),({struct Cyc_PP_Doc*_tmp852=Cyc_Absynpp_lb();_tmp4BB[1]=_tmp852;});Cyc_PP_cat(_tag_fat(_tmp4BB,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1905
 s=({struct Cyc_PP_Doc*_tmp4BD[2U];({struct Cyc_PP_Doc*_tmp855=Cyc_PP_text(({const char*_tmp4BE="__tempest_off__;";_tag_fat(_tmp4BE,sizeof(char),17U);}));_tmp4BD[0]=_tmp855;}),({struct Cyc_PP_Doc*_tmp854=Cyc_Absynpp_lb();_tmp4BD[1]=_tmp854;});Cyc_PP_cat(_tag_fat(_tmp4BD,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 1909
return s;}
# 1912
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4BF=sc;switch(_tmp4BF){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4C0="static ";_tag_fat(_tmp4C0,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4C1="extern ";_tag_fat(_tmp4C1,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4C2="extern \"C\" ";_tag_fat(_tmp4C2,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4C3="abstract ";_tag_fat(_tmp4C3,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4C4="register ";_tag_fat(_tmp4C4,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;}}
# 1926
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4C5=Cyc_Tcutil_compress(t);void*_stmttmp21=_tmp4C5;void*_tmp4C6=_stmttmp21;struct Cyc_List_List*_tmp4C7;struct Cyc_Absyn_Tvar*_tmp4C8;switch(*((int*)_tmp4C6)){case 2U: _LL1: _tmp4C8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4C6)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp4C8;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C6)->f1)->tag == 9U){_LL3: _tmp4C7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C6)->f2;_LL4: {struct Cyc_List_List*l=_tmp4C7;
return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 1938
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4C9=t;void*_tmp4CB;struct Cyc_Absyn_Typedefdecl*_tmp4CA;switch(*((int*)_tmp4C9)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C9)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp4CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f3;_tmp4CB=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f4;if(_tmp4CB != 0){_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmp4CA;void*x=_tmp4CB;
# 1945
return Cyc_Absynpp_is_anon_aggrtype(x);}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1952
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1955
if(tms != 0 && tms->tl != 0){
struct _tuple17 _tmp4CC=({struct _tuple17 _tmp558;_tmp558.f1=(void*)tms->hd,_tmp558.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp558;});struct _tuple17 _stmttmp22=_tmp4CC;struct _tuple17 _tmp4CD=_stmttmp22;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4CD.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4CD.f2)->tag == 3U){_LL1: _LL2:
# 1958
 return({struct Cyc_List_List*_tmp4CF=_region_malloc(r,sizeof(*_tmp4CF));_tmp4CF->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp857=({struct Cyc_List_List*_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE));_tmp4CE->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp856=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4CE->tl=_tmp856;});_tmp4CE;});_tmp4CF->tl=_tmp857;});_tmp4CF;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0));_tmp4D0->hd=atts,_tmp4D0->tl=tms;_tmp4D0;});}_LL0:;}else{
# 1961
return({struct Cyc_List_List*_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1));_tmp4D1->hd=atts,_tmp4D1->tl=tms;_tmp4D1;});}}
# 1964
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp4D2=({struct _fat_ptr _tmp858=({const char*_tmp4D8="`";_tag_fat(_tmp4D8,sizeof(char),2U);});Cyc_strconcat(_tmp858,(struct _fat_ptr)*t->name);});struct _fat_ptr s=_tmp4D2;
({struct _fat_ptr _tmp4D3=_fat_ptr_plus(s,sizeof(char),1);char _tmp4D4=*((char*)_check_fat_subscript(_tmp4D3,sizeof(char),0U));char _tmp4D5='t';if(_get_fat_size(_tmp4D3,sizeof(char))== 1U &&(_tmp4D4 == 0 && _tmp4D5 != 0))_throw_arraybounds();*((char*)_tmp4D3.curr)=_tmp4D5;});
({struct _fat_ptr*_tmp859=({unsigned _tmp4D7=1;struct _fat_ptr*_tmp4D6=_cycalloc(_check_times(_tmp4D7,sizeof(struct _fat_ptr)));_tmp4D6[0]=(struct _fat_ptr)s;_tmp4D6;});t->name=_tmp859;});}}
# 1973
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1975
void*_tmp4D9=t;void*_tmp4DD;struct Cyc_Absyn_Typedefdecl*_tmp4DC;struct Cyc_List_List*_tmp4DB;struct _tuple0*_tmp4DA;int _tmp4E0;void*_tmp4DF;struct Cyc_Core_Opt*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_List_List*_tmp4E9;struct Cyc_List_List*_tmp4E8;struct Cyc_Absyn_VarargInfo*_tmp4E7;int _tmp4E6;struct Cyc_List_List*_tmp4E5;void*_tmp4E4;struct Cyc_Absyn_Tqual _tmp4E3;void*_tmp4E2;struct Cyc_List_List*_tmp4E1;struct Cyc_Absyn_PtrAtts _tmp4EE;struct Cyc_Absyn_Tqual _tmp4ED;void*_tmp4EC;unsigned _tmp4F3;void*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Tqual _tmp4F0;void*_tmp4EF;switch(*((int*)_tmp4D9)){case 4U: _LL1: _tmp4EF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D9)->f1).elt_type;_tmp4F0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D9)->f1).tq;_tmp4F1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D9)->f1).num_elts;_tmp4F2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D9)->f1).zero_term;_tmp4F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D9)->f1).zt_loc;_LL2: {void*t2=_tmp4EF;struct Cyc_Absyn_Tqual tq2=_tmp4F0;struct Cyc_Absyn_Exp*e=_tmp4F1;void*zeroterm=_tmp4F2;unsigned ztl=_tmp4F3;
# 1978
struct _tuple12 _tmp4F4=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple12 _stmttmp23=_tmp4F4;struct _tuple12 _tmp4F5=_stmttmp23;struct Cyc_List_List*_tmp4F8;void*_tmp4F7;struct Cyc_Absyn_Tqual _tmp4F6;_LLE: _tmp4F6=_tmp4F5.f1;_tmp4F7=_tmp4F5.f2;_tmp4F8=_tmp4F5.f3;_LLF: {struct Cyc_Absyn_Tqual tq3=_tmp4F6;void*t3=_tmp4F7;struct Cyc_List_List*tml3=_tmp4F8;
void*tm;
if(e == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4F9=_region_malloc(r,sizeof(*_tmp4F9));_tmp4F9->tag=0U,_tmp4F9->f1=zeroterm,_tmp4F9->f2=ztl;_tmp4F9;});else{
# 1983
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4FA=_region_malloc(r,sizeof(*_tmp4FA));_tmp4FA->tag=1U,_tmp4FA->f1=e,_tmp4FA->f2=zeroterm,_tmp4FA->f3=ztl;_tmp4FA;});}
return({struct _tuple12 _tmp559;_tmp559.f1=tq3,_tmp559.f2=t3,({struct Cyc_List_List*_tmp85A=({struct Cyc_List_List*_tmp4FB=_region_malloc(r,sizeof(*_tmp4FB));_tmp4FB->hd=tm,_tmp4FB->tl=tml3;_tmp4FB;});_tmp559.f3=_tmp85A;});_tmp559;});}}case 3U: _LL3: _tmp4EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D9)->f1).elt_type;_tmp4ED=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D9)->f1).elt_tq;_tmp4EE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D9)->f1).ptr_atts;_LL4: {void*t2=_tmp4EC;struct Cyc_Absyn_Tqual tq2=_tmp4ED;struct Cyc_Absyn_PtrAtts ptratts=_tmp4EE;
# 1987
struct _tuple12 _tmp4FC=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple12 _stmttmp24=_tmp4FC;struct _tuple12 _tmp4FD=_stmttmp24;struct Cyc_List_List*_tmp500;void*_tmp4FF;struct Cyc_Absyn_Tqual _tmp4FE;_LL11: _tmp4FE=_tmp4FD.f1;_tmp4FF=_tmp4FD.f2;_tmp500=_tmp4FD.f3;_LL12: {struct Cyc_Absyn_Tqual tq3=_tmp4FE;void*t3=_tmp4FF;struct Cyc_List_List*tml3=_tmp500;
tml3=({struct Cyc_List_List*_tmp502=_region_malloc(r,sizeof(*_tmp502));({void*_tmp85B=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp501=_region_malloc(r,sizeof(*_tmp501));_tmp501->tag=2U,_tmp501->f1=ptratts,_tmp501->f2=tq;_tmp501;});_tmp502->hd=_tmp85B;}),_tmp502->tl=tml3;_tmp502;});
return({struct _tuple12 _tmp55A;_tmp55A.f1=tq3,_tmp55A.f2=t3,_tmp55A.f3=tml3;_tmp55A;});}}case 5U: _LL5: _tmp4E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).tvars;_tmp4E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).effect;_tmp4E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).ret_tqual;_tmp4E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).ret_type;_tmp4E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).args;_tmp4E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).c_varargs;_tmp4E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).cyc_varargs;_tmp4E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).rgn_po;_tmp4E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).attributes;_tmp4EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).requires_clause;_tmp4EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D9)->f1).ensures_clause;_LL6: {struct Cyc_List_List*typvars=_tmp4E1;void*effopt=_tmp4E2;struct Cyc_Absyn_Tqual t2qual=_tmp4E3;void*t2=_tmp4E4;struct Cyc_List_List*args=_tmp4E5;int c_varargs=_tmp4E6;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp4E7;struct Cyc_List_List*rgn_po=_tmp4E8;struct Cyc_List_List*fn_atts=_tmp4E9;struct Cyc_Absyn_Exp*req=_tmp4EA;struct Cyc_Absyn_Exp*ens=_tmp4EB;
# 1993
if(!Cyc_Absynpp_print_all_tvars){
# 1997
if(effopt == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2002
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2005
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2010
struct _tuple12 _tmp503=Cyc_Absynpp_to_tms(r,t2qual,t2);struct _tuple12 _stmttmp25=_tmp503;struct _tuple12 _tmp504=_stmttmp25;struct Cyc_List_List*_tmp507;void*_tmp506;struct Cyc_Absyn_Tqual _tmp505;_LL14: _tmp505=_tmp504.f1;_tmp506=_tmp504.f2;_tmp507=_tmp504.f3;_LL15: {struct Cyc_Absyn_Tqual tq3=_tmp505;void*t3=_tmp506;struct Cyc_List_List*tml3=_tmp507;
struct Cyc_List_List*tms=tml3;
# 2021 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp508=Cyc_Cyclone_c_compiler;if(_tmp508 == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2023
 if(fn_atts != 0)
# 2026
tms=({struct _RegionHandle*_tmp85D=r;void*_tmp85C=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp509=_region_malloc(r,sizeof(*_tmp509));_tmp509->tag=5U,_tmp509->f1=0U,_tmp509->f2=fn_atts;_tmp509;});Cyc_Absynpp_bubble_attributes(_tmp85D,_tmp85C,tms);});
# 2028
tms=({struct Cyc_List_List*_tmp50C=_region_malloc(r,sizeof(*_tmp50C));({void*_tmp85F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp50B=_region_malloc(r,sizeof(*_tmp50B));
_tmp50B->tag=3U,({void*_tmp85E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp50A=_region_malloc(r,sizeof(*_tmp50A));_tmp50A->tag=1U,_tmp50A->f1=args,_tmp50A->f2=c_varargs,_tmp50A->f3=cyc_varargs,_tmp50A->f4=effopt,_tmp50A->f5=rgn_po,_tmp50A->f6=req,_tmp50A->f7=ens;_tmp50A;});_tmp50B->f1=_tmp85E;});_tmp50B;});
# 2028
_tmp50C->hd=_tmp85F;}),_tmp50C->tl=tms;_tmp50C;});
# 2032
goto _LL16;}else{_LL19: _LL1A:
# 2034
 tms=({struct Cyc_List_List*_tmp50F=_region_malloc(r,sizeof(*_tmp50F));({void*_tmp861=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp50E=_region_malloc(r,sizeof(*_tmp50E));
_tmp50E->tag=3U,({void*_tmp860=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp50D=_region_malloc(r,sizeof(*_tmp50D));_tmp50D->tag=1U,_tmp50D->f1=args,_tmp50D->f2=c_varargs,_tmp50D->f3=cyc_varargs,_tmp50D->f4=effopt,_tmp50D->f5=rgn_po,_tmp50D->f6=req,_tmp50D->f7=ens;_tmp50D;});_tmp50E->f1=_tmp860;});_tmp50E;});
# 2034
_tmp50F->hd=_tmp861;}),_tmp50F->tl=tms;_tmp50F;});
# 2038
for(0;fn_atts != 0;fn_atts=fn_atts->tl){
void*_tmp510=(void*)fn_atts->hd;void*_stmttmp26=_tmp510;void*_tmp511=_stmttmp26;switch(*((int*)_tmp511)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2043
 tms=({struct Cyc_List_List*_tmp514=_region_malloc(r,sizeof(*_tmp514));({void*_tmp863=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp513=_region_malloc(r,sizeof(*_tmp513));_tmp513->tag=5U,_tmp513->f1=0U,({struct Cyc_List_List*_tmp862=({struct Cyc_List_List*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->hd=(void*)fn_atts->hd,_tmp512->tl=0;_tmp512;});_tmp513->f2=_tmp862;});_tmp513;});_tmp514->hd=_tmp863;}),_tmp514->tl=tms;_tmp514;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2047
goto _LL16;}_LL16:;}
# 2050
AfterAtts:
 if(typvars != 0)
tms=({struct Cyc_List_List*_tmp516=_region_malloc(r,sizeof(*_tmp516));({void*_tmp864=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp515=_region_malloc(r,sizeof(*_tmp515));_tmp515->tag=4U,_tmp515->f1=typvars,_tmp515->f2=0U,_tmp515->f3=1;_tmp515;});_tmp516->hd=_tmp864;}),_tmp516->tl=tms;_tmp516;});
return({struct _tuple12 _tmp55B;_tmp55B.f1=tq3,_tmp55B.f2=t3,_tmp55B.f3=tms;_tmp55B;});}}}case 1U: _LL7: _tmp4DE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D9)->f1;_tmp4DF=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D9)->f2;_tmp4E0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D9)->f3;_LL8: {struct Cyc_Core_Opt*k=_tmp4DE;void*topt=_tmp4DF;int i=_tmp4E0;
# 2056
if(topt == 0)
return({struct _tuple12 _tmp55C;_tmp55C.f1=tq,_tmp55C.f2=t,_tmp55C.f3=0;_tmp55C;});else{
# 2059
return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8U: _LL9: _tmp4DA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D9)->f1;_tmp4DB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D9)->f2;_tmp4DC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D9)->f3;_tmp4DD=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D9)->f4;_LLA: {struct _tuple0*n=_tmp4DA;struct Cyc_List_List*ts=_tmp4DB;struct Cyc_Absyn_Typedefdecl*td=_tmp4DC;void*topt=_tmp4DD;
# 2065
if(topt == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple12 _tmp55D;_tmp55D.f1=tq,_tmp55D.f2=t,_tmp55D.f3=0;_tmp55D;});else{
# 2068
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}}default: _LLB: _LLC:
# 2073
 return({struct _tuple12 _tmp55E;_tmp55E.f1=tq,_tmp55E.f2=t,_tmp55E.f3=0;_tmp55E;});}_LL0:;}
# 2077
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2079
void*_tmp517=t;void*_tmp518;void*_tmp519;switch(*((int*)_tmp517)){case 1U: _LL1: _tmp519=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp517)->f2;if(_tmp519 != 0){_LL2: {void*def=_tmp519;
return Cyc_Absynpp_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp518=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp517)->f1).elt_type;_LL4: {void*elt_typ=_tmp518;
# 2082
L: {
void*_tmp51A=elt_typ;void*_tmp51B;void*_tmp51C;switch(*((int*)_tmp51A)){case 1U: _LL8: _tmp51C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51A)->f2;if(_tmp51C != 0){_LL9: {void*t=_tmp51C;
elt_typ=t;goto L;}}else{goto _LLE;}case 8U: _LLA: _tmp51B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51A)->f4;if(_tmp51B != 0){_LLB: {void*t=_tmp51B;
elt_typ=t;goto L;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51A)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51A)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 2089
 return 0;}_LL0:;}
# 2093
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp51D=_new_region("temp");struct _RegionHandle*temp=& _tmp51D;_push_region(temp);
{struct _tuple12 _tmp51E=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _stmttmp27=_tmp51E;struct _tuple12 _tmp51F=_stmttmp27;struct Cyc_List_List*_tmp522;void*_tmp521;struct Cyc_Absyn_Tqual _tmp520;_LL1: _tmp520=_tmp51F.f1;_tmp521=_tmp51F.f2;_tmp522=_tmp51F.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp520;void*t=_tmp521;struct Cyc_List_List*tms=_tmp522;
tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);
if(tms == 0 && dopt == 0){
struct Cyc_PP_Doc*_tmp524=({struct Cyc_PP_Doc*_tmp523[2U];({struct Cyc_PP_Doc*_tmp866=Cyc_Absynpp_tqual2doc(tq);_tmp523[0]=_tmp866;}),({struct Cyc_PP_Doc*_tmp865=Cyc_Absynpp_ntyp2doc(t);_tmp523[1]=_tmp865;});Cyc_PP_cat(_tag_fat(_tmp523,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp524;}else{
# 2100
struct Cyc_PP_Doc*_tmp527=({struct Cyc_PP_Doc*_tmp525[4U];({
struct Cyc_PP_Doc*_tmp86C=Cyc_Absynpp_tqual2doc(tq);_tmp525[0]=_tmp86C;}),({
struct Cyc_PP_Doc*_tmp86B=Cyc_Absynpp_ntyp2doc(t);_tmp525[1]=_tmp86B;}),({
struct Cyc_PP_Doc*_tmp86A=Cyc_PP_text(({const char*_tmp526=" ";_tag_fat(_tmp526,sizeof(char),2U);}));_tmp525[2]=_tmp86A;}),({
struct Cyc_PP_Doc*_tmp869=({int _tmp868=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp867=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp868,_tmp867,tms);});_tmp525[3]=_tmp869;});Cyc_PP_cat(_tag_fat(_tmp525,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp527;}}}
# 2095
;_pop_region(temp);}
# 2108
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp528=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp528;
if((unsigned)req)
requires_doc=({struct Cyc_PP_Doc*_tmp529[2U];({struct Cyc_PP_Doc*_tmp86E=Cyc_PP_text(({const char*_tmp52A="@requires ";_tag_fat(_tmp52A,sizeof(char),11U);}));_tmp529[0]=_tmp86E;}),({struct Cyc_PP_Doc*_tmp86D=Cyc_Absynpp_exp2doc(req);_tmp529[1]=_tmp86D;});Cyc_PP_cat(_tag_fat(_tmp529,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2114
requires_doc=Cyc_PP_nil_doc();}{
# 2116
enum Cyc_Cyclone_C_Compilers _tmp52B=Cyc_Cyclone_c_compiler;if(_tmp52B == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2119
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp52C[5U];({struct Cyc_PP_Doc*_tmp876=({struct Cyc_Absyn_Tqual _tmp875=f->tq;void*_tmp874=f->type;Cyc_Absynpp_tqtd2doc(_tmp875,_tmp874,({struct Cyc_Core_Opt*_tmp52D=_cycalloc(sizeof(*_tmp52D));({struct Cyc_PP_Doc*_tmp873=Cyc_PP_textptr(f->name);_tmp52D->v=_tmp873;});_tmp52D;}));});_tmp52C[0]=_tmp876;}),({
struct Cyc_PP_Doc*_tmp872=Cyc_PP_text(({const char*_tmp52E=":";_tag_fat(_tmp52E,sizeof(char),2U);}));_tmp52C[1]=_tmp872;}),({struct Cyc_PP_Doc*_tmp871=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp52C[2]=_tmp871;}),({
struct Cyc_PP_Doc*_tmp870=Cyc_Absynpp_atts2doc(f->attributes);_tmp52C[3]=_tmp870;}),({struct Cyc_PP_Doc*_tmp86F=Cyc_PP_text(({const char*_tmp52F=";";_tag_fat(_tmp52F,sizeof(char),2U);}));_tmp52C[4]=_tmp86F;});Cyc_PP_cat(_tag_fat(_tmp52C,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2124
return({struct Cyc_PP_Doc*_tmp530[4U];({struct Cyc_PP_Doc*_tmp87C=({struct Cyc_Absyn_Tqual _tmp87B=f->tq;void*_tmp87A=f->type;Cyc_Absynpp_tqtd2doc(_tmp87B,_tmp87A,({struct Cyc_Core_Opt*_tmp531=_cycalloc(sizeof(*_tmp531));({struct Cyc_PP_Doc*_tmp879=Cyc_PP_textptr(f->name);_tmp531->v=_tmp879;});_tmp531;}));});_tmp530[0]=_tmp87C;}),({
struct Cyc_PP_Doc*_tmp878=Cyc_Absynpp_atts2doc(f->attributes);_tmp530[1]=_tmp878;}),_tmp530[2]=requires_doc,({struct Cyc_PP_Doc*_tmp877=Cyc_PP_text(({const char*_tmp532=";";_tag_fat(_tmp532,sizeof(char),2U);}));_tmp530[3]=_tmp877;});Cyc_PP_cat(_tag_fat(_tmp530,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2127
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp533[5U];({struct Cyc_PP_Doc*_tmp884=Cyc_Absynpp_atts2doc(f->attributes);_tmp533[0]=_tmp884;}),({
struct Cyc_PP_Doc*_tmp883=({struct Cyc_Absyn_Tqual _tmp882=f->tq;void*_tmp881=f->type;Cyc_Absynpp_tqtd2doc(_tmp882,_tmp881,({struct Cyc_Core_Opt*_tmp534=_cycalloc(sizeof(*_tmp534));({struct Cyc_PP_Doc*_tmp880=Cyc_PP_textptr(f->name);_tmp534->v=_tmp880;});_tmp534;}));});_tmp533[1]=_tmp883;}),({
struct Cyc_PP_Doc*_tmp87F=Cyc_PP_text(({const char*_tmp535=":";_tag_fat(_tmp535,sizeof(char),2U);}));_tmp533[2]=_tmp87F;}),({struct Cyc_PP_Doc*_tmp87E=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp533[3]=_tmp87E;}),({struct Cyc_PP_Doc*_tmp87D=Cyc_PP_text(({const char*_tmp536=";";_tag_fat(_tmp536,sizeof(char),2U);}));_tmp533[4]=_tmp87D;});Cyc_PP_cat(_tag_fat(_tmp533,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2132
return({struct Cyc_PP_Doc*_tmp537[4U];({struct Cyc_PP_Doc*_tmp88A=Cyc_Absynpp_atts2doc(f->attributes);_tmp537[0]=_tmp88A;}),({
struct Cyc_PP_Doc*_tmp889=({struct Cyc_Absyn_Tqual _tmp888=f->tq;void*_tmp887=f->type;Cyc_Absynpp_tqtd2doc(_tmp888,_tmp887,({struct Cyc_Core_Opt*_tmp538=_cycalloc(sizeof(*_tmp538));({struct Cyc_PP_Doc*_tmp886=Cyc_PP_textptr(f->name);_tmp538->v=_tmp886;});_tmp538;}));});_tmp537[1]=_tmp889;}),_tmp537[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp885=Cyc_PP_text(({const char*_tmp539=";";_tag_fat(_tmp539,sizeof(char),2U);}));_tmp537[3]=_tmp885;});Cyc_PP_cat(_tag_fat(_tmp537,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;}}
# 2139
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp88B=({const char*_tmp53A="";_tag_fat(_tmp53A,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp88B,fields);});}
# 2143
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp53B[3U];({struct Cyc_PP_Doc*_tmp88F=Cyc_Absynpp_scope2doc(f->sc);_tmp53B[0]=_tmp88F;}),({struct Cyc_PP_Doc*_tmp88E=Cyc_Absynpp_typedef_name2doc(f->name);_tmp53B[1]=_tmp88E;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp88D=Cyc_PP_nil_doc();_tmp53B[2]=_tmp88D;}):({struct Cyc_PP_Doc*_tmp88C=Cyc_Absynpp_args2doc(f->typs);_tmp53B[2]=_tmp88C;});Cyc_PP_cat(_tag_fat(_tmp53B,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2148
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _fat_ptr _tmp890=({const char*_tmp53C=",";_tag_fat(_tmp53C,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp890,fields);});}
# 2157
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp891=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp891,72,f);});
({void*_tmp53D=0U;({struct Cyc___cycFILE*_tmp893=f;struct _fat_ptr _tmp892=({const char*_tmp53E="\n";_tag_fat(_tmp53E,sizeof(char),2U);});Cyc_fprintf(_tmp893,_tmp892,_tag_fat(_tmp53D,sizeof(void*),0U));});});}}
# 2164
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _tmp894=({const char*_tmp53F="";_tag_fat(_tmp53F,sizeof(char),1U);});Cyc_PP_seql(_tmp894,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2167
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
# 2181
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}

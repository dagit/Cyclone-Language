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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 111 "core.h"
extern void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_heap_region;
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
extern struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
extern void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 305
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 312
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};
# 470
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Absyn_No_throw_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 496
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 498
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 506
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 513
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 531
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 894 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 904
void*Cyc_Absyn_compress(void*);
# 918
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 920
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 923
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 925
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 927
extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_wchar_type (void);
void*Cyc_Absyn_gen_float_type(unsigned i);
# 930
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 934
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 936
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 953
void*Cyc_Absyn_exn_type (void);
# 961
extern void*Cyc_Absyn_fat_bound_type;
# 963
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 965
void*Cyc_Absyn_bounds_one (void);
# 967
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 971
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 973
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 975
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 979
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 987
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 991
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1134
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1136
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1138
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1141
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1143
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66 "warn.h"
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 43 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_EvalOrder =11U,Cyc_Flags_CCompiler =12U,Cyc_Flags_AllPasses =13U};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 31 "tcutil.h"
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 41
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 53
int Cyc_Tcutil_is_bound_one(void*);
# 55
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 65
void*Cyc_Tcutil_pointer_region(void*);
# 68
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 78
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 99
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 104
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*);
# 111
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*,void*,struct Cyc_List_List*);
# 115
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 117
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 122
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple12{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 125
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 129
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 138
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 140
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 148
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 156
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 159
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 172
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 178
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 182
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 196
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 201
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 212
void Cyc_Tcutil_check_nonzero_bound(unsigned,void*);
# 214
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 216
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 227
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 230
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 235
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 240
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple15{int f1;void*f2;};
# 244
struct _tuple15 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 260
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 267
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 278
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 281
int Cyc_Tcutil_zeroable_type(void*);
# 285
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 288
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 290
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 76 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 93
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 120
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 124
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
# 138
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 144
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 146
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*eff);
# 151
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_List_List*po);
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 33
void*Cyc_CurRgn_instantiate(void*fntype,void*currgn);struct _tuple16{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 53
extern int Cyc_Evexp_okay_szofarg(void*t);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 34
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr msg_part);
void Cyc_Tcexp_check_consume(unsigned,void*,struct Cyc_Absyn_Exp*);
# 52 "attributes.h"
int Cyc_Atts_attribute_cmp(void*,void*);
# 47 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr ap){
# 49
Cyc_Warn_verr2(loc,ap);
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
return*topt;}
# 54
static void Cyc_Tcexp_err_and_explain(unsigned loc,struct _fat_ptr ap){
Cyc_Warn_verr2(loc,ap);
Cyc_Unify_explain_failure();}
# 59
void Cyc_Tcexp_check_consume(unsigned loc,void*t,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
# 64
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52F;_tmp52F.tag=0U,({struct _fat_ptr _tmp667=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp52F.f1=_tmp667;});_tmp52F;});void*_tmp0[1U];_tmp0[0]=& _tmp1;({unsigned _tmp668=loc;Cyc_Warn_err2(_tmp668,_tag_fat(_tmp0,sizeof(void*),1U));});});}
# 68
static struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp3;
void*_tmp4=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->v=tenv_tvs;_tmpA;}));void*t2=_tmp4;
void*_tmp5=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->v=tenv_tvs;_tmp9;}));void*rt=_tmp5;
void*_tmp6=Cyc_Tcutil_any_bool(tenv_tvs);void*nbl=_tmp6;
void*_tmp7=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp7;
void*_tmp8=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp8;
return({struct Cyc_Absyn_PtrInfo _tmp530;_tmp530.elt_type=t2,({struct Cyc_Absyn_Tqual _tmp669=Cyc_Absyn_empty_tqual(0U);_tmp530.elt_tq=_tmp669;}),(_tmp530.ptr_atts).rgn=rt,(_tmp530.ptr_atts).nullable=nbl,(_tmp530.ptr_atts).bounds=b,(_tmp530.ptr_atts).zero_term=zt,(_tmp530.ptr_atts).ptrloc=0;_tmp530;});}
# 78
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 82
if(topt == 0){
# 84
({void*_tmp66A=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp66A;});
return;}{
# 87
void*t=*topt;
void*_tmpC=Cyc_Absyn_compress(t);void*_stmttmp0=_tmpC;void*_tmpD=_stmttmp0;struct Cyc_Absyn_Tqual _tmpF;void*_tmpE;union Cyc_Absyn_AggrInfo _tmp10;switch(*((int*)_tmpD)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 20U){_LL1: _tmp10=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp10;
# 90
{union Cyc_Absyn_AggrInfo _tmp11=info;struct Cyc_Absyn_Aggrdecl*_tmp12;if((_tmp11.UnknownAggr).tag == 1){_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp531;_tmp531.tag=0U,({struct _fat_ptr _tmp66B=({const char*_tmp15="struct type improperly set";_tag_fat(_tmp15,sizeof(char),27U);});_tmp531.f1=_tmp66B;});_tmp531;});void*_tmp13[1U];_tmp13[0]=& _tmp14;({unsigned _tmp66C=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp66C,_tag_fat(_tmp13,sizeof(void*),1U));});});}else{_LLC: _tmp12=*(_tmp11.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmp12;
({void*_tmp66D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=29U,_tmp16->f1=ad->name,_tmp16->f2=0,_tmp16->f3=des,_tmp16->f4=ad;_tmp16;});e->r=_tmp66D;});}}_LL9:;}
# 94
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).elt_type;_tmpF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).tq;_LL4: {void*at=_tmpE;struct Cyc_Absyn_Tqual aq=_tmpF;
({void*_tmp66E=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=26U,_tmp17->f1=des;_tmp17;});e->r=_tmp66E;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp66F=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=30U,_tmp18->f1=t,_tmp18->f2=des;_tmp18;});e->r=_tmp66F;});goto _LL0;default: _LL7: _LL8:
({void*_tmp670=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=26U,_tmp19->f1=des;_tmp19;});e->r=_tmp670;});goto _LL0;}_LL0:;}}
# 104
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 108
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp1A=e->r;void*_stmttmp1=_tmp1A;void*_tmp1B=_stmttmp1;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->f2 == 0){_LL1: _LL2:
# 111
 if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp532;_tmp532.tag=0U,({struct _fat_ptr _tmp671=({const char*_tmp1E="assignment in test";_tag_fat(_tmp1E,sizeof(char),19U);});_tmp532.f1=_tmp671;});_tmp532;});void*_tmp1C[1U];_tmp1C[0]=& _tmp1D;({unsigned _tmp672=e->loc;Cyc_Warn_warn2(_tmp672,_tag_fat(_tmp1C,sizeof(void*),1U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 119
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp537;_tmp537.tag=0U,({struct _fat_ptr _tmp673=({const char*_tmp27="test of ";_tag_fat(_tmp27,sizeof(char),9U);});_tmp537.f1=_tmp673;});_tmp537;});struct Cyc_Warn_String_Warn_Warg_struct _tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _tmp536;_tmp536.tag=0U,_tmp536.f1=msg_part;_tmp536;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp535;_tmp535.tag=0U,({struct _fat_ptr _tmp674=({const char*_tmp26=" has type ";_tag_fat(_tmp26,sizeof(char),11U);});_tmp535.f1=_tmp674;});_tmp535;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp23=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp534;_tmp534.tag=3U,_tmp534.f1=(void*)e->topt;_tmp534;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24=({struct Cyc_Warn_String_Warn_Warg_struct _tmp533;_tmp533.tag=0U,({
struct _fat_ptr _tmp675=({const char*_tmp25=" instead of integral or pointer type";_tag_fat(_tmp25,sizeof(char),37U);});_tmp533.f1=_tmp675;});_tmp533;});void*_tmp1F[5U];_tmp1F[0]=& _tmp20,_tmp1F[1]=& _tmp21,_tmp1F[2]=& _tmp22,_tmp1F[3]=& _tmp23,_tmp1F[4]=& _tmp24;({unsigned _tmp676=e->loc;Cyc_Warn_err2(_tmp676,_tag_fat(_tmp1F,sizeof(void*),5U));});});}
# 141 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 146
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
union Cyc_Absyn_Cnst _tmp28=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _stmttmp2=_tmp28;union Cyc_Absyn_Cnst _tmp29=_stmttmp2;struct _fat_ptr _tmp2A;struct _fat_ptr _tmp2B;int _tmp2D;enum Cyc_Absyn_Sign _tmp2C;int _tmp2E;switch((_tmp29.String_c).tag){case 2U: switch(((_tmp29.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 return Cyc_Absyn_uchar_type;default: _LL5: _LL6:
 return Cyc_Absyn_char_type;}case 3U: _LL7: _LL8:
 return Cyc_Absyn_wchar_type();case 4U: if(((_tmp29.Short_c).val).f1 == Cyc_Absyn_Unsigned){_LL9: _LLA:
 return Cyc_Absyn_ushort_type;}else{_LLB: _LLC:
 return Cyc_Absyn_sshort_type;}case 6U: if(((_tmp29.LongLong_c).val).f1 == Cyc_Absyn_Unsigned){_LLD: _LLE:
 return Cyc_Absyn_ulonglong_type;}else{_LLF: _LL10:
 return Cyc_Absyn_slonglong_type;}case 7U: _LL11: _tmp2E=((_tmp29.Float_c).val).f2;_LL12: {int i=_tmp2E;
# 161
if(topt == 0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_tmp2F=Cyc_Absyn_compress(*topt);void*_stmttmp3=_tmp2F;void*_tmp30=_stmttmp3;int _tmp31;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1)->tag == 2U){_LL1C: _tmp31=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1)->f1;_LL1D: {int i=_tmp31;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E: _LL1F:
 return Cyc_Absyn_gen_float_type((unsigned)i);}_LL1B:;}}case 5U: _LL13: _tmp2C=((_tmp29.Int_c).val).f1;_tmp2D=((_tmp29.Int_c).val).f2;_LL14: {enum Cyc_Absyn_Sign csn=_tmp2C;int i=_tmp2D;
# 171
if(topt == 0)
return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;{
void*_tmp32=Cyc_Absyn_compress(*topt);void*_stmttmp4=_tmp32;void*_tmp33=_stmttmp4;void*_tmp34;enum Cyc_Absyn_Sign _tmp35;enum Cyc_Absyn_Sign _tmp36;enum Cyc_Absyn_Sign _tmp37;enum Cyc_Absyn_Sign _tmp38;switch(*((int*)_tmp33)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f2){case Cyc_Absyn_Char_sz: _LL21: _tmp38=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL22: {enum Cyc_Absyn_Sign sn=_tmp38;
# 175
({union Cyc_Absyn_Cnst _tmp677=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp677;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _LL23: _tmp37=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp37;
# 178
({union Cyc_Absyn_Cnst _tmp678=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp678;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _LL25: _tmp36=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL26: {enum Cyc_Absyn_Sign sn=_tmp36;
_tmp35=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _LL27: _tmp35=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp35;
# 184
({union Cyc_Absyn_Cnst _tmp679=Cyc_Absyn_Int_c(sn,i);*c=_tmp679;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2 != 0){_LL2B: _tmp34=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2)->hd;_LL2C: {void*t1=_tmp34;
# 193
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=9U,({struct Cyc_Absyn_Exp*_tmp67A=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp39->f1=_tmp67A;});_tmp39;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3U: _LL29: if(i == 0){_LL2A: {
# 188
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp67B=({union Cyc_Absyn_Cnst _tmp538;(_tmp538.Null_c).tag=1U,(_tmp538.Null_c).val=0;_tmp538;});*c=_tmp67B;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}}else{goto _LL2D;}default: _LL2D: _LL2E:
# 195
 return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL20:;}}case 1U: _LL15: _LL16:
# 198
 if(topt != 0){
void*_tmp3A=Cyc_Absyn_compress(*topt);void*_stmttmp5=_tmp3A;void*_tmp3B=_stmttmp5;void*_tmp3C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->tag == 3U){_LL30: _tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->f1).ptr_atts).nullable;_LL31: {void*nbl=_tmp3C;
# 201
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53A;_tmp53A.tag=0U,({struct _fat_ptr _tmp67C=({const char*_tmp40="Used NULL when expecting a value of type ";_tag_fat(_tmp40,sizeof(char),42U);});_tmp53A.f1=_tmp67C;});_tmp53A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp539;_tmp539.tag=2U,_tmp539.f1=(void*)*topt;_tmp539;});void*_tmp3D[2U];_tmp3D[0]=& _tmp3E,_tmp3D[1]=& _tmp3F;({unsigned _tmp67D=e->loc;Cyc_Warn_err2(_tmp67D,_tag_fat(_tmp3D,sizeof(void*),2U));});});
return*topt;}}else{_LL32: _LL33:
 goto _LL2F;}_LL2F:;}{
# 206
struct Cyc_Absyn_PtrInfo _tmp41=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp41;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
return Cyc_Absyn_pointer_type(pi);}case 8U: _LL17: _tmp2B=(_tmp29.String_c).val;_LL18: {struct _fat_ptr s=_tmp2B;
# 211
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp2A=s;goto _LL1A;}default: _LL19: _tmp2A=(_tmp29.Wstring_c).val;_LL1A: {struct _fat_ptr s=_tmp2A;
# 214
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 218
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp67E=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp67E,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 223
void*_tmp42=({void*_tmp682=string_elt_typ;void*_tmp681=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp680=Cyc_Absyn_const_tqual(0U);void*_tmp67F=
Cyc_Absyn_thin_bounds_exp(elen);
# 223
Cyc_Absyn_atb_type(_tmp682,_tmp681,_tmp680,_tmp67F,Cyc_Absyn_true_type);});void*t=_tmp42;
# 225
if(topt == 0)
return t;{
void*_tmp43=Cyc_Absyn_compress(*topt);void*_stmttmp6=_tmp43;void*_tmp44=_stmttmp6;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 4U: _LL35: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp45;
# 231
return({void*_tmp685=string_elt_typ;struct Cyc_Absyn_Tqual _tmp684=tq;struct Cyc_Absyn_Exp*_tmp683=elen;Cyc_Absyn_array_type(_tmp685,_tmp684,_tmp683,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL37: _LL38:
# 236
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp688=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp687=loc;void*_tmp686=t;Cyc_Tcutil_silent_castable(_tmp688,_tmp687,_tmp686,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
return*topt;}
# 241
return t;default: _LL39: _LL3A:
 return t;}_LL34:;}}}}}_LL0:;}
# 248
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp46=*((void**)_check_null(b));void*_stmttmp7=_tmp46;void*_tmp47=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp48;struct Cyc_Absyn_Vardecl*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4A;struct Cyc_Absyn_Fndecl*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;struct _tuple1*_tmp4D;switch(*((int*)_tmp47)){case 0U: _LL1: _tmp4D=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp47)->f1;_LL2: {struct _tuple1*q=_tmp4D;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B;_tmp53B.tag=0U,({struct _fat_ptr _tmp689=({const char*_tmp50="unresolved binding in tcVar";_tag_fat(_tmp50,sizeof(char),28U);});_tmp53B.f1=_tmp689;});_tmp53B;});void*_tmp4E[1U];_tmp4E[0]=& _tmp4F;({unsigned _tmp68A=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp68A,_tag_fat(_tmp4E,sizeof(void*),1U));});});}case 1U: _LL3: _tmp4C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp47)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp4C;
# 254
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2U: _LL5: _tmp4B=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp47)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmp4B;
# 261
if(fd->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5U: _LL7: _tmp4A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp47)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp4A;
_tmp49=vd;goto _LLA;}case 4U: _LL9: _tmp49=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp47)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp49;
_tmp48=vd;goto _LLC;}default: _LLB: _tmp48=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp47)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp48;
# 267
if(te->allow_valueof){
void*_tmp51=Cyc_Absyn_compress(vd->type);void*_stmttmp8=_tmp51;void*_tmp52=_stmttmp8;void*_tmp53;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2 != 0){_LLE: _tmp53=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2)->hd;_LLF: {void*i=_tmp53;
({void*_tmp68B=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=39U,_tmp54->f1=i;_tmp54;});e->r=_tmp68B;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 272
return vd->type;}}_LL0:;}
# 276
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 283
void*_tmp55=fmt->r;void*_stmttmp9=_tmp55;void*_tmp56=_stmttmp9;struct _fat_ptr _tmp57;struct _fat_ptr _tmp58;switch(*((int*)_tmp56)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56)->f1).String_c).tag == 8){_LL1: _tmp58=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp58;
_tmp57=s;goto _LL4;}}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp57=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->f1).String_c).val;_LL4: {struct _fat_ptr s=_tmp57;
# 286
struct Cyc_List_List*_tmp59=type_getter(te,(struct _fat_ptr)s,isCproto,fmt->loc);struct Cyc_List_List*desc_types=_tmp59;
if(opt_args == 0)
return;{
struct Cyc_List_List*_tmp5A=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp5A;
# 291
for(0;desc_types != 0 && args != 0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp68D=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp68C=e;Cyc_Tcutil_coerce_arg(_tmp68D,_tmp68C,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53F;_tmp53F.tag=0U,({struct _fat_ptr _tmp68E=({const char*_tmp61="descriptor has type ";_tag_fat(_tmp61,sizeof(char),21U);});_tmp53F.f1=_tmp68E;});_tmp53F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53E;_tmp53E.tag=2U,_tmp53E.f1=(void*)t;_tmp53E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53D;_tmp53D.tag=0U,({
struct _fat_ptr _tmp68F=({const char*_tmp60=" but argument has type ";_tag_fat(_tmp60,sizeof(char),24U);});_tmp53D.f1=_tmp68F;});_tmp53D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp53C;_tmp53C.tag=3U,_tmp53C.f1=(void*)e->topt;_tmp53C;});void*_tmp5B[4U];_tmp5B[0]=& _tmp5C,_tmp5B[1]=& _tmp5D,_tmp5B[2]=& _tmp5E,_tmp5B[3]=& _tmp5F;({unsigned _tmp690=e->loc;Cyc_Tcexp_err_and_explain(_tmp690,_tag_fat(_tmp5B,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp691=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=(void*)arg_cnt,_tmp62->tl=*alias_arg_exps;_tmp62;});*alias_arg_exps=_tmp691;});
Cyc_Tcexp_check_consume(e->loc,t,e);}
# 305
if(desc_types != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp540;_tmp540.tag=0U,({struct _fat_ptr _tmp692=({const char*_tmp65="too few arguments";_tag_fat(_tmp65,sizeof(char),18U);});_tmp540.f1=_tmp692;});_tmp540;});void*_tmp63[1U];_tmp63[0]=& _tmp64;({unsigned _tmp693=fmt->loc;Cyc_Warn_err2(_tmp693,_tag_fat(_tmp63,sizeof(void*),1U));});});
if(args != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp541;_tmp541.tag=0U,({struct _fat_ptr _tmp694=({const char*_tmp68="too many arguments";_tag_fat(_tmp68,sizeof(char),19U);});_tmp541.f1=_tmp694;});_tmp541;});void*_tmp66[1U];_tmp66[0]=& _tmp67;({unsigned _tmp695=((struct Cyc_Absyn_Exp*)args->hd)->loc;Cyc_Warn_err2(_tmp695,_tag_fat(_tmp66,sizeof(void*),1U));});});
# 310
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}
# 313
return;}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 317
 if(opt_args == 0)
return;
{struct Cyc_List_List*_tmp69=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp69;for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)args->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt),(struct Cyc_Absyn_Exp*)args->hd);}}
# 323
return;}_LL0:;}
# 327
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 329
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp6A=p;switch(_tmp6A){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 333
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp543;_tmp543.tag=0U,({struct _fat_ptr _tmp696=({const char*_tmp6E="expecting numeric type but found ";_tag_fat(_tmp6E,sizeof(char),34U);});_tmp543.f1=_tmp696;});_tmp543;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp542;_tmp542.tag=2U,_tmp542.f1=(void*)t;_tmp542;});void*_tmp6B[2U];_tmp6B[0]=& _tmp6C,_tmp6B[1]=& _tmp6D;({unsigned _tmp697=loc;Cyc_Warn_err2(_tmp697,_tag_fat(_tmp6B,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 337
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp545;_tmp545.tag=0U,({struct _fat_ptr _tmp698=({const char*_tmp72="expecting integral or * type but found ";_tag_fat(_tmp72,sizeof(char),40U);});_tmp545.f1=_tmp698;});_tmp545;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp71=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp544;_tmp544.tag=2U,_tmp544.f1=(void*)t;_tmp544;});void*_tmp6F[2U];_tmp6F[0]=& _tmp70,_tmp6F[1]=& _tmp71;({unsigned _tmp699=loc;Cyc_Warn_err2(_tmp699,_tag_fat(_tmp6F,sizeof(void*),2U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 342
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmp547;_tmp547.tag=0U,({struct _fat_ptr _tmp69A=({const char*_tmp76="expecting integral type but found ";_tag_fat(_tmp76,sizeof(char),35U);});_tmp547.f1=_tmp69A;});_tmp547;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp75=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp546;_tmp546.tag=2U,_tmp546.f1=(void*)t;_tmp546;});void*_tmp73[2U];_tmp73[0]=& _tmp74,_tmp73[1]=& _tmp75;({unsigned _tmp69B=loc;Cyc_Warn_err2(_tmp69B,_tag_fat(_tmp73,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 346
{void*_tmp77=t;void*_tmp78;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->tag == 3U){_LLE: _tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp78;
# 348
struct Cyc_Absyn_Exp*_tmp79=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp79;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp548;_tmp548.tag=0U,({struct _fat_ptr _tmp69C=({const char*_tmp7C="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp7C,sizeof(char),53U);});_tmp548.f1=_tmp69C;});_tmp548;});void*_tmp7A[1U];_tmp7A[0]=& _tmp7B;({unsigned _tmp69D=loc;Cyc_Warn_err2(_tmp69D,_tag_fat(_tmp7A,sizeof(void*),1U));});});
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54A;_tmp54A.tag=0U,({struct _fat_ptr _tmp69E=({const char*_tmp80="numelts requires pointer type, not ";_tag_fat(_tmp80,sizeof(char),36U);});_tmp54A.f1=_tmp69E;});_tmp54A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp549;_tmp549.tag=2U,_tmp549.f1=(void*)t;_tmp549;});void*_tmp7D[2U];_tmp7D[0]=& _tmp7E,_tmp7D[1]=& _tmp7F;({unsigned _tmp69F=loc;Cyc_Warn_err2(_tmp69F,_tag_fat(_tmp7D,sizeof(void*),2U));});});}_LLD:;}
# 354
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp82=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54B;_tmp54B.tag=0U,({struct _fat_ptr _tmp6A0=({const char*_tmp83="Non-unary primop";_tag_fat(_tmp83,sizeof(char),17U);});_tmp54B.f1=_tmp6A0;});_tmp54B;});void*_tmp81[1U];_tmp81[0]=& _tmp82;({unsigned _tmp6A1=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp6A1,_tag_fat(_tmp81,sizeof(void*),1U));});});}_LL0:;}
# 361
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp84=(void*)_check_null(e1->topt);void*t1=_tmp84;
void*_tmp85=(void*)_check_null(e2->topt);void*t2=_tmp85;
void*_tmp86=Cyc_Tcutil_max_arithmetic_type(t1,t2);void*new_typ=_tmp86;
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,Cyc_Absyn_No_coercion);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,Cyc_Absyn_No_coercion);
return new_typ;}
# 371
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 374
if(!checker(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54E;_tmp54E.tag=0U,({struct _fat_ptr _tmp6A2=({const char*_tmp8C="type ";_tag_fat(_tmp8C,sizeof(char),6U);});_tmp54E.f1=_tmp6A2;});_tmp54E;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp89=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp54D;_tmp54D.tag=3U,_tmp54D.f1=(void*)e1->topt;_tmp54D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54C;_tmp54C.tag=0U,({struct _fat_ptr _tmp6A3=({const char*_tmp8B=" cannot be used here";_tag_fat(_tmp8B,sizeof(char),21U);});_tmp54C.f1=_tmp6A3;});_tmp54C;});void*_tmp87[3U];_tmp87[0]=& _tmp88,_tmp87[1]=& _tmp89,_tmp87[2]=& _tmp8A;({struct Cyc_Tcenv_Tenv*_tmp6A5=te;unsigned _tmp6A4=e1->loc;Cyc_Tcexp_expr_err(_tmp6A5,_tmp6A4,0,_tag_fat(_tmp87,sizeof(void*),3U));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp8E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp551;_tmp551.tag=0U,({struct _fat_ptr _tmp6A6=({const char*_tmp92="type ";_tag_fat(_tmp92,sizeof(char),6U);});_tmp551.f1=_tmp6A6;});_tmp551;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp550;_tmp550.tag=3U,_tmp550.f1=(void*)e2->topt;_tmp550;});struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54F;_tmp54F.tag=0U,({struct _fat_ptr _tmp6A7=({const char*_tmp91=" cannot be used here";_tag_fat(_tmp91,sizeof(char),21U);});_tmp54F.f1=_tmp6A7;});_tmp54F;});void*_tmp8D[3U];_tmp8D[0]=& _tmp8E,_tmp8D[1]=& _tmp8F,_tmp8D[2]=& _tmp90;({struct Cyc_Tcenv_Tenv*_tmp6A9=te;unsigned _tmp6A8=e2->loc;Cyc_Tcexp_expr_err(_tmp6A9,_tmp6A8,0,_tag_fat(_tmp8D,sizeof(void*),3U));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 381
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
void*_tmp93=t1;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;struct Cyc_Absyn_Tqual _tmp95;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->tag == 3U){_LL1: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_type;_tmp95=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_tq;_tmp96=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).rgn;_tmp97=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).nullable;_tmp98=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).bounds;_tmp99=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).zero_term;_LL2: {void*et=_tmp94;struct Cyc_Absyn_Tqual tq=_tmp95;void*r=_tmp96;void*n=_tmp97;void*b=_tmp98;void*zt=_tmp99;
# 386
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp552;_tmp552.tag=0U,({struct _fat_ptr _tmp6AA=({const char*_tmp9C="can't do arithmetic on abstract pointer type";_tag_fat(_tmp9C,sizeof(char),45U);});_tmp552.f1=_tmp6AA;});_tmp552;});void*_tmp9A[1U];_tmp9A[0]=& _tmp9B;({unsigned _tmp6AB=e1->loc;Cyc_Warn_err2(_tmp6AB,_tag_fat(_tmp9A,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp553;_tmp553.tag=0U,({struct _fat_ptr _tmp6AC=({const char*_tmp9F="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmp9F,sizeof(char),49U);});_tmp553.f1=_tmp6AC;});_tmp553;});void*_tmp9D[1U];_tmp9D[0]=& _tmp9E;({unsigned _tmp6AD=e1->loc;Cyc_Warn_err2(_tmp6AD,_tag_fat(_tmp9D,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp555;_tmp555.tag=0U,({struct _fat_ptr _tmp6AE=({const char*_tmpA3="expecting int but found ";_tag_fat(_tmpA3,sizeof(char),25U);});_tmp555.f1=_tmp6AE;});_tmp555;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp554;_tmp554.tag=2U,_tmp554.f1=(void*)t2;_tmp554;});void*_tmpA0[2U];_tmpA0[0]=& _tmpA1,_tmpA0[1]=& _tmpA2;({unsigned _tmp6AF=e2->loc;Cyc_Warn_err2(_tmp6AF,_tag_fat(_tmpA0,sizeof(void*),2U));});});{
struct Cyc_Absyn_Exp*_tmpA4=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmpA4;
if(eopt == 0)
return t1;
# 397
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmpA5=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmpA5;struct _tuple16 _tmpA6=_stmttmpA;int _tmpA8;unsigned _tmpA7;_LL6: _tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_LL7: {unsigned i=_tmpA7;int known=_tmpA8;
if(known && i == (unsigned)1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp556;_tmp556.tag=0U,({struct _fat_ptr _tmp6B0=({const char*_tmpAB="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpAB,sizeof(char),70U);});_tmp556.f1=_tmp6B0;});_tmp556;});void*_tmpA9[1U];_tmpA9[0]=& _tmpAA;({unsigned _tmp6B1=e1->loc;Cyc_Warn_warn2(_tmp6B1,_tag_fat(_tmpA9,sizeof(void*),1U));});});}}{
# 408
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAC=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=3U,(_tmpAD->f1).elt_type=et,(_tmpAD->f1).elt_tq=tq,((_tmpAD->f1).ptr_atts).rgn=r,((_tmpAD->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpAD->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpAD->f1).ptr_atts).zero_term=zt,((_tmpAD->f1).ptr_atts).ptrloc=0;_tmpAD;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpAC;
# 411
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,Cyc_Absyn_Other_coercion);
return(void*)new_t1;}}}}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 418
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55C;_tmp55C.tag=0U,({struct _fat_ptr _tmp6B2=({const char*_tmpB8="pointer arithmetic on values of different ";_tag_fat(_tmpB8,sizeof(char),43U);});_tmp55C.f1=_tmp6B2;});_tmp55C;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55B;_tmp55B.tag=0U,({
struct _fat_ptr _tmp6B3=({const char*_tmpB7="types (";_tag_fat(_tmpB7,sizeof(char),8U);});_tmp55B.f1=_tmp6B3;});_tmp55B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55A;_tmp55A.tag=2U,_tmp55A.f1=(void*)t1;_tmp55A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp559;_tmp559.tag=0U,({struct _fat_ptr _tmp6B4=({const char*_tmpB6=" != ";_tag_fat(_tmpB6,sizeof(char),5U);});_tmp559.f1=_tmp6B4;});_tmp559;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp558;_tmp558.tag=2U,_tmp558.f1=(void*)t2;_tmp558;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp557;_tmp557.tag=0U,({struct _fat_ptr _tmp6B5=({const char*_tmpB5=")";_tag_fat(_tmpB5,sizeof(char),2U);});_tmp557.f1=_tmp6B5;});_tmp557;});void*_tmpAE[6U];_tmpAE[0]=& _tmpAF,_tmpAE[1]=& _tmpB0,_tmpAE[2]=& _tmpB1,_tmpAE[3]=& _tmpB2,_tmpAE[4]=& _tmpB3,_tmpAE[5]=& _tmpB4;({unsigned _tmp6B6=e1->loc;Cyc_Tcexp_err_and_explain(_tmp6B6,_tag_fat(_tmpAE,sizeof(void*),6U));});});
return Cyc_Absyn_sint_type;}
# 430
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp6B7=t1_elt;Cyc_Unify_unify(_tmp6B7,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp562;_tmp562.tag=0U,({struct _fat_ptr _tmp6B8=({const char*_tmpC3="pointer arithmetic on values of different ";_tag_fat(_tmpC3,sizeof(char),43U);});_tmp562.f1=_tmp6B8;});_tmp562;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp561;_tmp561.tag=0U,({
struct _fat_ptr _tmp6B9=({const char*_tmpC2="types(";_tag_fat(_tmpC2,sizeof(char),7U);});_tmp561.f1=_tmp6B9;});_tmp561;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp560;_tmp560.tag=2U,_tmp560.f1=(void*)t1;_tmp560;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55F;_tmp55F.tag=0U,({struct _fat_ptr _tmp6BA=({const char*_tmpC1=" != ";_tag_fat(_tmpC1,sizeof(char),5U);});_tmp55F.f1=_tmp6BA;});_tmp55F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55E;_tmp55E.tag=2U,_tmp55E.f1=(void*)t2;_tmp55E;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55D;_tmp55D.tag=0U,({struct _fat_ptr _tmp6BB=({const char*_tmpC0=")";_tag_fat(_tmpC0,sizeof(char),2U);});_tmp55D.f1=_tmp6BB;});_tmp55D;});void*_tmpB9[6U];_tmpB9[0]=& _tmpBA,_tmpB9[1]=& _tmpBB,_tmpB9[2]=& _tmpBC,_tmpB9[3]=& _tmpBD,_tmpB9[4]=& _tmpBE,_tmpB9[5]=& _tmpBF;({unsigned _tmp6BC=e1->loc;Cyc_Tcexp_err_and_explain(_tmp6BC,_tag_fat(_tmpB9,sizeof(void*),6U));});});
# 435
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp563;_tmp563.tag=0U,({struct _fat_ptr _tmp6BD=({const char*_tmpC6="coercing fat pointer to thin pointer for subtraction";_tag_fat(_tmpC6,sizeof(char),53U);});_tmp563.f1=_tmp6BD;});_tmp563;});void*_tmpC4[1U];_tmpC4[0]=& _tmpC5;({unsigned _tmp6BE=e1->loc;Cyc_Warn_warn2(_tmp6BE,_tag_fat(_tmpC4,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp6C2=e1;Cyc_Tcutil_unchecked_cast(_tmp6C2,({void*_tmp6C1=t1_elt;void*_tmp6C0=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp6BF=
Cyc_Absyn_empty_tqual(0U);
# 436
Cyc_Absyn_star_type(_tmp6C1,_tmp6C0,_tmp6BF,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 439
return Cyc_Absyn_sint_type;}
# 441
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp564;_tmp564.tag=0U,({struct _fat_ptr _tmp6C3=({const char*_tmpC9="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpC9,sizeof(char),50U);});_tmp564.f1=_tmp6C3;});_tmp564;});void*_tmpC7[1U];_tmpC7[0]=& _tmpC8;({unsigned _tmp6C4=e1->loc;Cyc_Warn_err2(_tmp6C4,_tag_fat(_tmpC7,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp565;_tmp565.tag=0U,({struct _fat_ptr _tmp6C5=({const char*_tmpCC="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpCC,sizeof(char),54U);});_tmp565.f1=_tmp6C5;});_tmp565;});void*_tmpCA[1U];_tmpCA[0]=& _tmpCB;({unsigned _tmp6C6=e1->loc;Cyc_Warn_err2(_tmp6C6,_tag_fat(_tmpCA,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp569;_tmp569.tag=0U,({struct _fat_ptr _tmp6C7=({const char*_tmpD3="expecting ";_tag_fat(_tmpD3,sizeof(char),11U);});_tmp569.f1=_tmp6C7;});_tmp569;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp568;_tmp568.tag=2U,_tmp568.f1=(void*)t1;_tmp568;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp567;_tmp567.tag=0U,({struct _fat_ptr _tmp6C8=({const char*_tmpD2=" or int but found ";_tag_fat(_tmpD2,sizeof(char),19U);});_tmp567.f1=_tmp6C8;});_tmp567;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp566;_tmp566.tag=2U,_tmp566.f1=(void*)t2;_tmp566;});void*_tmpCD[4U];_tmpCD[0]=& _tmpCE,_tmpCD[1]=& _tmpCF,_tmpCD[2]=& _tmpD0,_tmpCD[3]=& _tmpD1;({unsigned _tmp6C9=e2->loc;Cyc_Tcexp_err_and_explain(_tmp6C9,_tag_fat(_tmpCD,sizeof(void*),4U));});});
return t1;}
# 450
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp6CA=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp6CA,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56A;_tmp56A.tag=0U,({struct _fat_ptr _tmp6CB=({const char*_tmpD6="coercing fat pointer to thin pointer for subtraction";_tag_fat(_tmpD6,sizeof(char),53U);});_tmp56A.f1=_tmp6CB;});_tmp56A;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD5;({unsigned _tmp6CC=e1->loc;Cyc_Warn_warn2(_tmp6CC,_tag_fat(_tmpD4,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp6D0=e2;Cyc_Tcutil_unchecked_cast(_tmp6D0,({void*_tmp6CF=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp6CE=
Cyc_Tcutil_pointer_region(t2);
# 455
struct Cyc_Absyn_Tqual _tmp6CD=
# 457
Cyc_Absyn_empty_tqual(0U);
# 455
Cyc_Absyn_star_type(_tmp6CF,_tmp6CE,_tmp6CD,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 460
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56B;_tmp56B.tag=0U,({struct _fat_ptr _tmp6D1=({const char*_tmpD9="thin pointer subtraction!";_tag_fat(_tmpD9,sizeof(char),26U);});_tmp56B.f1=_tmp6D1;});_tmp56B;});void*_tmpD7[1U];_tmpD7[0]=& _tmpD8;({unsigned _tmp6D2=e1->loc;Cyc_Warn_warn2(_tmp6D2,_tag_fat(_tmpD7,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;}
# 463
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56C;_tmp56C.tag=0U,({struct _fat_ptr _tmp6D3=({const char*_tmpDC="coercing thin pointer to integer for subtraction";_tag_fat(_tmpDC,sizeof(char),49U);});_tmp56C.f1=_tmp6D3;});_tmp56C;});void*_tmpDA[1U];_tmpDA[0]=& _tmpDB;({unsigned _tmp6D4=e1->loc;Cyc_Warn_warn2(_tmp6D4,_tag_fat(_tmpDA,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 466
if(Cyc_Tcutil_is_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56D;_tmp56D.tag=0U,({struct _fat_ptr _tmp6D5=({const char*_tmpDF="coercing pointer to integer for subtraction";_tag_fat(_tmpDF,sizeof(char),44U);});_tmp56D.f1=_tmp6D5;});_tmp56D;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDE;({unsigned _tmp6D6=e1->loc;Cyc_Warn_warn2(_tmp6D6,_tag_fat(_tmpDD,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 471
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 474
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 482
if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind ||({
void*_tmp6D7=t1;Cyc_Unify_unify(_tmp6D7,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 487
if(({struct Cyc_RgnOrder_RgnPO*_tmp6DA=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6D9=loc;void*_tmp6D8=t2;Cyc_Tcutil_silent_castable(_tmp6DA,_tmp6D9,_tmp6D8,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 491
if(({struct Cyc_RgnOrder_RgnPO*_tmp6DD=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6DC=loc;void*_tmp6DB=t1;Cyc_Tcutil_silent_castable(_tmp6DD,_tmp6DC,_tmp6DB,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 495
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 500
{struct _tuple0 _tmpE0=({struct _tuple0 _tmp56E;({void*_tmp6DF=Cyc_Absyn_compress(t1);_tmp56E.f1=_tmp6DF;}),({void*_tmp6DE=Cyc_Absyn_compress(t2);_tmp56E.f2=_tmp6DE;});_tmp56E;});struct _tuple0 _stmttmpB=_tmpE0;struct _tuple0 _tmpE1=_stmttmpB;void*_tmpE3;void*_tmpE2;switch(*((int*)_tmpE1.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1.f2)->tag == 3U){_LL1: _tmpE2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1.f1)->f1).elt_type;_tmpE3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1.f2)->f1).elt_type;_LL2: {void*t1a=_tmpE2;void*t2a=_tmpE3;
# 502
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE1.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE1.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE1.f2)->f1)->tag == 3U){_LL3: _LL4:
# 506
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 510
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp572;_tmp572.tag=0U,({struct _fat_ptr _tmp6E0=({const char*_tmpEA="comparison not allowed between ";_tag_fat(_tmpEA,sizeof(char),32U);});_tmp572.f1=_tmp6E0;});_tmp572;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp571;_tmp571.tag=2U,_tmp571.f1=(void*)t1;_tmp571;});struct Cyc_Warn_String_Warn_Warg_struct _tmpE7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp570;_tmp570.tag=0U,({struct _fat_ptr _tmp6E1=({const char*_tmpE9=" and ";_tag_fat(_tmpE9,sizeof(char),6U);});_tmp570.f1=_tmp6E1;});_tmp570;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56F;_tmp56F.tag=2U,_tmp56F.f1=(void*)t2;_tmp56F;});void*_tmpE4[4U];_tmpE4[0]=& _tmpE5,_tmpE4[1]=& _tmpE6,_tmpE4[2]=& _tmpE7,_tmpE4[3]=& _tmpE8;({unsigned _tmp6E2=loc;Cyc_Tcexp_err_and_explain(_tmp6E2,_tag_fat(_tmpE4,sizeof(void*),4U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 516
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 518
enum Cyc_Absyn_Primop _tmpEB=p;switch(_tmpEB){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 522
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 525
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL15: _LL16:
# 534
 goto _LL18;case Cyc_Absyn_Neq: _LL17: _LL18:
 goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A:
 goto _LL1C;case Cyc_Absyn_Lt: _LL1B: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL1F: _LL20:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);default: _LL21: _LL22:
# 541
({struct Cyc_Warn_String_Warn_Warg_struct _tmpED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp573;_tmp573.tag=0U,({struct _fat_ptr _tmp6E3=({const char*_tmpEE="bad binary primop";_tag_fat(_tmpEE,sizeof(char),18U);});_tmp573.f1=_tmp6E3;});_tmp573;});void*_tmpEC[1U];_tmpEC[0]=& _tmpED;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpEC,sizeof(void*),1U));});}_LL0:;}
# 545
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 553
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpEF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpEF;
void*_tmpF0=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpF0;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp575;_tmp575.tag=0U,({struct _fat_ptr _tmp6E4=({const char*_tmpF4="expecting numeric type but found ";_tag_fat(_tmpF4,sizeof(char),34U);});_tmp575.f1=_tmp6E4;});_tmp575;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp574;_tmp574.tag=2U,_tmp574.f1=(void*)t;_tmp574;});void*_tmpF1[2U];_tmpF1[0]=& _tmpF2,_tmpF1[1]=& _tmpF3;({unsigned _tmp6E5=e->loc;Cyc_Warn_err2(_tmp6E5,_tag_fat(_tmpF1,sizeof(void*),2U));});});
return t;}
# 560
{struct Cyc_List_List*_tmpF5=es;struct Cyc_List_List*es2=_tmpF5;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _tmpF6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpF6;int _tmpF7=_stmttmpC;switch(_tmpF7){case 0U: _LL1: _LL2:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp576;_tmp576.tag=0U,({struct _fat_ptr _tmp6E6=({const char*_tmpFA="primitive operator has 0 arguments";_tag_fat(_tmpFA,sizeof(char),35U);});_tmp576.f1=_tmp6E6;});_tmp576;});void*_tmpF8[1U];_tmpF8[0]=& _tmpF9;({struct Cyc_Tcenv_Tenv*_tmp6E9=te;unsigned _tmp6E8=loc;void**_tmp6E7=topt;Cyc_Tcexp_expr_err(_tmp6E9,_tmp6E8,_tmp6E7,_tag_fat(_tmpF8,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);case 2U: _LL5: _LL6:
 return({struct Cyc_Tcenv_Tenv*_tmp6EE=te;unsigned _tmp6ED=loc;void**_tmp6EC=topt;enum Cyc_Absyn_Primop _tmp6EB=p;struct Cyc_Absyn_Exp*_tmp6EA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp6EE,_tmp6ED,_tmp6EC,_tmp6EB,_tmp6EA,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpFC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp577;_tmp577.tag=0U,({struct _fat_ptr _tmp6EF=({const char*_tmpFD="primitive operator has > 2 arguments";_tag_fat(_tmpFD,sizeof(char),37U);});_tmp577.f1=_tmp6EF;});_tmp577;});void*_tmpFB[1U];_tmpFB[0]=& _tmpFC;({struct Cyc_Tcenv_Tenv*_tmp6F2=te;unsigned _tmp6F1=loc;void**_tmp6F0=topt;Cyc_Tcexp_expr_err(_tmp6F2,_tmp6F1,_tmp6F0,_tag_fat(_tmpFB,sizeof(void*),1U));});});}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 570
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
t=Cyc_Absyn_compress(t);{
void*_tmpFE=t;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Tqual _tmp101;void*_tmp100;struct Cyc_List_List*_tmp102;struct Cyc_Absyn_Datatypefield*_tmp103;struct Cyc_Absyn_Aggrdecl*_tmp104;switch(*((int*)_tmpFE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp104=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp104;
# 574
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp106=({struct Cyc_Warn_String_Warn_Warg_struct _tmp579;_tmp579.tag=0U,({struct _fat_ptr _tmp6F3=({const char*_tmp108="attempt to write abstract ";_tag_fat(_tmp108,sizeof(char),27U);});_tmp579.f1=_tmp6F3;});_tmp579;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp107=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp578;_tmp578.tag=6U,_tmp578.f1=ad;_tmp578;});void*_tmp105[2U];_tmp105[0]=& _tmp106,_tmp105[1]=& _tmp107;({unsigned _tmp6F4=loc;Cyc_Warn_err2(_tmp6F4,_tag_fat(_tmp105,sizeof(void*),2U));});});
return 0;}
# 578
_tmp102=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmp103=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmp103;
# 590
{struct Cyc_List_List*_tmp10E=df->typs;struct Cyc_List_List*fs=_tmp10E;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp10F=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmp10F;struct _tuple17*_tmp110=_stmttmpD;void*_tmp112;struct Cyc_Absyn_Tqual _tmp111;_LLE: _tmp111=_tmp110->f1;_tmp112=_tmp110->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp111;void*t=_tmp112;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp114=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57C;_tmp57C.tag=0U,({struct _fat_ptr _tmp6F5=({const char*_tmp118="attempt to overwrite a datatype field (";_tag_fat(_tmp118,sizeof(char),40U);});_tmp57C.f1=_tmp6F5;});_tmp57C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp115=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp57B;_tmp57B.tag=1U,_tmp57B.f1=df->name;_tmp57B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp116=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57A;_tmp57A.tag=0U,({
struct _fat_ptr _tmp6F6=({const char*_tmp117=") with a const member";_tag_fat(_tmp117,sizeof(char),22U);});_tmp57A.f1=_tmp6F6;});_tmp57A;});void*_tmp113[3U];_tmp113[0]=& _tmp114,_tmp113[1]=& _tmp115,_tmp113[2]=& _tmp116;({unsigned _tmp6F7=loc;Cyc_Warn_err2(_tmp6F7,_tag_fat(_tmp113,sizeof(void*),3U));});});
return 0;}
# 597
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 599
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmp102=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFE)->f2;_LL4: {struct Cyc_List_List*fs=_tmp102;
# 580
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp109=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp109;
if((f->tq).real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57E;_tmp57E.tag=0U,({struct _fat_ptr _tmp6F8=({const char*_tmp10D="attempt to overwrite an aggregate with const member ";_tag_fat(_tmp10D,sizeof(char),53U);});_tmp57E.f1=_tmp6F8;});_tmp57E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57D;_tmp57D.tag=0U,_tmp57D.f1=*f->name;_tmp57D;});void*_tmp10A[2U];_tmp10A[0]=& _tmp10B,_tmp10A[1]=& _tmp10C;({unsigned _tmp6F9=loc;Cyc_Warn_err2(_tmp6F9,_tag_fat(_tmp10A,sizeof(void*),2U));});});
return 0;}
# 586
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 588
return 1;}case 4U: _LL7: _tmp100=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFE)->f1).elt_type;_tmp101=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFE)->f1).tq;_LL8: {void*elt_type=_tmp100;struct Cyc_Absyn_Tqual tq=_tmp101;
# 601
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57F;_tmp57F.tag=0U,({struct _fat_ptr _tmp6FA=({const char*_tmp11B="attempt to overwrite a const array";_tag_fat(_tmp11B,sizeof(char),35U);});_tmp57F.f1=_tmp6FA;});_tmp57F;});void*_tmp119[1U];_tmp119[0]=& _tmp11A;({unsigned _tmp6FB=loc;Cyc_Warn_err2(_tmp6FB,_tag_fat(_tmp119,sizeof(void*),1U));});});
return 0;}
# 605
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmpFF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFE)->f1;_LLA: {struct Cyc_List_List*fs=_tmpFF;
# 607
for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp11C=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp11C;struct _tuple17*_tmp11D=_stmttmpE;void*_tmp11F;struct Cyc_Absyn_Tqual _tmp11E;_LL11: _tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmp11E;void*t=_tmp11F;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp121=({struct Cyc_Warn_String_Warn_Warg_struct _tmp580;_tmp580.tag=0U,({struct _fat_ptr _tmp6FC=({const char*_tmp122="attempt to overwrite a tuple field with a const member";_tag_fat(_tmp122,sizeof(char),55U);});_tmp580.f1=_tmp6FC;});_tmp580;});void*_tmp120[1U];_tmp120[0]=& _tmp121;({unsigned _tmp6FD=loc;Cyc_Warn_err2(_tmp6FD,_tag_fat(_tmp120,sizeof(void*),1U));});});
return 0;}
# 613
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}
# 615
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}}
# 623
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 626
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp123=e->r;void*_stmttmpF=_tmp123;void*_tmp124=_stmttmpF;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct _fat_ptr*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct _fat_ptr*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Vardecl*_tmp12E;struct Cyc_Absyn_Vardecl*_tmp12F;struct Cyc_Absyn_Vardecl*_tmp130;struct Cyc_Absyn_Vardecl*_tmp131;switch(*((int*)_tmp124)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)){case 3U: _LL1: _tmp131=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp131;
_tmp130=vd;goto _LL4;}case 4U: _LL3: _tmp130=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp130;
_tmp12F=vd;goto _LL6;}case 5U: _LL5: _tmp12F=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp12F;
_tmp12E=vd;goto _LL8;}case 1U: _LL7: _tmp12E=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp12E;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23U: _LL9: _tmp12C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_tmp12D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp124)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp12C;struct Cyc_Absyn_Exp*e2=_tmp12D;
# 633
{void*_tmp132=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp132;void*_tmp133=_stmttmp10;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_Tqual _tmp135;struct Cyc_Absyn_Tqual _tmp136;switch(*((int*)_tmp133)){case 3U: _LL18: _tmp136=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_tq;_LL19: {struct Cyc_Absyn_Tqual tq=_tmp136;
_tmp135=tq;goto _LL1B;}case 4U: _LL1A: _tmp135=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp135;
if(!tq.real_const)return;goto _LL17;}case 6U: _LL1C: _tmp134=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp133)->f1;_LL1D: {struct Cyc_List_List*ts=_tmp134;
# 637
struct _tuple16 _tmp137=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp11=_tmp137;struct _tuple16 _tmp138=_stmttmp11;int _tmp13A;unsigned _tmp139;_LL21: _tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;_LL22: {unsigned i=_tmp139;int known=_tmp13A;
if(!known){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0U,({struct _fat_ptr _tmp6FE=({const char*_tmp13D="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp13D,sizeof(char),47U);});_tmp581.f1=_tmp6FE;});_tmp581;});void*_tmp13B[1U];_tmp13B[0]=& _tmp13C;({unsigned _tmp6FF=e->loc;Cyc_Warn_err2(_tmp6FF,_tag_fat(_tmp13B,sizeof(void*),1U));});});
return;}
# 642
{struct _handler_cons _tmp13E;_push_handler(& _tmp13E);{int _tmp140=0;if(setjmp(_tmp13E.handler))_tmp140=1;if(!_tmp140){
{struct _tuple17*_tmp141=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i);struct _tuple17*_stmttmp12=_tmp141;struct _tuple17*_tmp142=_stmttmp12;struct Cyc_Absyn_Tqual _tmp143;_LL24: _tmp143=_tmp142->f1;_LL25: {struct Cyc_Absyn_Tqual tq=_tmp143;
if(!tq.real_const){_npop_handler(0U);return;}}}
# 643
;_pop_handler();}else{void*_tmp13F=(void*)Cyc_Core_get_exn_thrown();void*_tmp144=_tmp13F;void*_tmp145;if(((struct Cyc_List_Nth_exn_struct*)_tmp144)->tag == Cyc_List_Nth){_LL27: _LL28:
# 645
 return;}else{_LL29: _tmp145=_tmp144;_LL2A: {void*exn=_tmp145;(int)_rethrow(exn);}}_LL26:;}}}
goto _LL17;}}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 649
goto _LL0;}case 21U: _LLB: _tmp12A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_tmp12B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp124)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp12A;struct _fat_ptr*f=_tmp12B;
# 651
{void*_tmp146=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp146;void*_tmp147=_stmttmp13;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_Aggrdecl**_tmp149;switch(*((int*)_tmp147)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp149=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1).KnownAggr).val;_LL2D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp149;
# 653
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp148=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp147)->f2;_LL2F: {struct Cyc_List_List*fs=_tmp148;
# 658
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 663
goto _LL0;}case 22U: _LLD: _tmp128=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_tmp129=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp124)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp128;struct _fat_ptr*f=_tmp129;
# 665
{void*_tmp14A=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp14A;void*_tmp14B=_stmttmp14;struct Cyc_Absyn_Tqual _tmp14D;void*_tmp14C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14B)->tag == 3U){_LL33: _tmp14C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14B)->f1).elt_type;_tmp14D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14B)->f1).elt_tq;_LL34: {void*elt_typ=_tmp14C;struct Cyc_Absyn_Tqual tq=_tmp14D;
# 667
if(!tq.real_const){
void*_tmp14E=Cyc_Absyn_compress(elt_typ);void*_stmttmp15=_tmp14E;void*_tmp14F=_stmttmp15;struct Cyc_List_List*_tmp150;struct Cyc_Absyn_Aggrdecl**_tmp151;switch(*((int*)_tmp14F)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14F)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14F)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp151=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14F)->f1)->f1).KnownAggr).val;_LL39: {struct Cyc_Absyn_Aggrdecl**adp=_tmp151;
# 670
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp150=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14F)->f2;_LL3B: {struct Cyc_List_List*fs=_tmp150;
# 675
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 681
goto _LL32;}}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 684
goto _LL0;}case 20U: _LLF: _tmp127=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp127;
# 686
{void*_tmp152=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp152;void*_tmp153=_stmttmp16;struct Cyc_Absyn_Tqual _tmp154;struct Cyc_Absyn_Tqual _tmp155;switch(*((int*)_tmp153)){case 3U: _LL3F: _tmp155=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp153)->f1).elt_tq;_LL40: {struct Cyc_Absyn_Tqual tq=_tmp155;
_tmp154=tq;goto _LL42;}case 4U: _LL41: _tmp154=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp153)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp154;
if(!tq.real_const)return;goto _LL3E;}default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 691
goto _LL0;}case 12U: _LL11: _tmp126=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp126;
_tmp125=e1;goto _LL14;}case 13U: _LL13: _tmp125=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp125;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 696
({struct Cyc_Warn_String_Warn_Warg_struct _tmp157=({struct Cyc_Warn_String_Warn_Warg_struct _tmp583;_tmp583.tag=0U,({struct _fat_ptr _tmp700=({const char*_tmp159="attempt to write a const location: ";_tag_fat(_tmp159,sizeof(char),36U);});_tmp583.f1=_tmp700;});_tmp583;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp158=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp582;_tmp582.tag=4U,_tmp582.f1=e;_tmp582;});void*_tmp156[2U];_tmp156[0]=& _tmp157,_tmp156[1]=& _tmp158;({unsigned _tmp701=e->loc;Cyc_Warn_err2(_tmp701,_tag_fat(_tmp156,sizeof(void*),2U));});});}
# 699
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 702
({struct Cyc_Tcenv_Tenv*_tmp702=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp702,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp15B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp584;_tmp584.tag=0U,({struct _fat_ptr _tmp703=({const char*_tmp15C="increment/decrement of non-lvalue";_tag_fat(_tmp15C,sizeof(char),34U);});_tmp584.f1=_tmp703;});_tmp584;});void*_tmp15A[1U];_tmp15A[0]=& _tmp15B;({struct Cyc_Tcenv_Tenv*_tmp706=te;unsigned _tmp705=loc;void**_tmp704=topt;Cyc_Tcexp_expr_err(_tmp706,_tmp705,_tmp704,_tag_fat(_tmp15A,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 708
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp15D=0U;({unsigned _tmp708=e->loc;struct _fat_ptr _tmp707=({const char*_tmp15E="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp15E,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp708,_tmp707,_tag_fat(_tmp15D,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp15F=0U;({unsigned _tmp70A=e->loc;struct _fat_ptr _tmp709=({const char*_tmp160="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp160,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp70A,_tmp709,_tag_fat(_tmp15F,sizeof(void*),0U));});});}else{
# 717
({struct Cyc_Warn_String_Warn_Warg_struct _tmp162=({struct Cyc_Warn_String_Warn_Warg_struct _tmp586;_tmp586.tag=0U,({struct _fat_ptr _tmp70B=({const char*_tmp164="expecting arithmetic or ? type but found ";_tag_fat(_tmp164,sizeof(char),42U);});_tmp586.f1=_tmp70B;});_tmp586;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp163=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp585;_tmp585.tag=2U,_tmp585.f1=(void*)t;_tmp585;});void*_tmp161[2U];_tmp161[0]=& _tmp162,_tmp161[1]=& _tmp163;({unsigned _tmp70C=e->loc;Cyc_Warn_err2(_tmp70C,_tag_fat(_tmp161,sizeof(void*),2U));});});}}
# 719
return t;}}
# 723
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 725
({struct Cyc_Tcenv_Tenv*_tmp70E=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp70D=e1;Cyc_Tcexp_tcTest(_tmp70E,_tmp70D,({const char*_tmp165="conditional expression";_tag_fat(_tmp165,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*_tmp166=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tako:& Cyc_Tcutil_tmko;struct Cyc_Core_Opt*ko=_tmp166;
void*_tmp167=({struct Cyc_Core_Opt*_tmp70F=ko;Cyc_Absyn_new_evar(_tmp70F,Cyc_Tcenv_lookup_opt_type_vars(te));});void*t=_tmp167;
struct Cyc_List_List _tmp168=({struct Cyc_List_List _tmp58C;_tmp58C.hd=e3,_tmp58C.tl=0;_tmp58C;});struct Cyc_List_List l1=_tmp168;
struct Cyc_List_List _tmp169=({struct Cyc_List_List _tmp58B;_tmp58B.hd=e2,_tmp58B.tl=& l1;_tmp58B;});struct Cyc_List_List l2=_tmp169;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp710=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp710,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58A;_tmp58A.tag=0U,({struct _fat_ptr _tmp711=({const char*_tmp170="conditional clause types do not match: ";_tag_fat(_tmp170,sizeof(char),40U);});_tmp58A.f1=_tmp711;});_tmp58A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp16C=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp589;_tmp589.tag=3U,_tmp589.f1=(void*)e2->topt;_tmp589;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp588;_tmp588.tag=0U,({
struct _fat_ptr _tmp712=({const char*_tmp16F=" != ";_tag_fat(_tmp16F,sizeof(char),5U);});_tmp588.f1=_tmp712;});_tmp588;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp587;_tmp587.tag=3U,_tmp587.f1=(void*)e3->topt;_tmp587;});void*_tmp16A[4U];_tmp16A[0]=& _tmp16B,_tmp16A[1]=& _tmp16C,_tmp16A[2]=& _tmp16D,_tmp16A[3]=& _tmp16E;({unsigned _tmp713=loc;Cyc_Tcexp_err_and_explain(_tmp713,_tag_fat(_tmp16A,sizeof(void*),4U));});});
return t;}}
# 739
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 741
({struct Cyc_Tcenv_Tenv*_tmp715=te;struct Cyc_Absyn_Exp*_tmp714=e1;Cyc_Tcexp_tcTest(_tmp715,_tmp714,({const char*_tmp171="logical-and expression";_tag_fat(_tmp171,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp717=te;struct Cyc_Absyn_Exp*_tmp716=e2;Cyc_Tcexp_tcTest(_tmp717,_tmp716,({const char*_tmp172="logical-and expression";_tag_fat(_tmp172,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 745
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 747
({struct Cyc_Tcenv_Tenv*_tmp719=te;struct Cyc_Absyn_Exp*_tmp718=e1;Cyc_Tcexp_tcTest(_tmp719,_tmp718,({const char*_tmp173="logical-or expression";_tag_fat(_tmp173,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp71B=te;struct Cyc_Absyn_Exp*_tmp71A=e2;Cyc_Tcexp_tcTest(_tmp71B,_tmp71A,({const char*_tmp174="logical-or expression";_tag_fat(_tmp174,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 753
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 760
({struct Cyc_Tcenv_Tenv*_tmp71C=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp71C,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 765
{void*_tmp175=Cyc_Absyn_compress(t1);void*_stmttmp17=_tmp175;void*_tmp176=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp176)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp178=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58D;_tmp58D.tag=0U,({struct _fat_ptr _tmp71D=({const char*_tmp179="cannot assign to an array";_tag_fat(_tmp179,sizeof(char),26U);});_tmp58D.f1=_tmp71D;});_tmp58D;});void*_tmp177[1U];_tmp177[0]=& _tmp178;({unsigned _tmp71E=loc;Cyc_Warn_err2(_tmp71E,_tag_fat(_tmp177,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 770
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58E;_tmp58E.tag=0U,({struct _fat_ptr _tmp71F=({const char*_tmp17C="type is abstract (can't determine size)";_tag_fat(_tmp17C,sizeof(char),40U);});_tmp58E.f1=_tmp71F;});_tmp58E;});void*_tmp17A[1U];_tmp17A[0]=& _tmp17B;({unsigned _tmp720=loc;Cyc_Warn_err2(_tmp720,_tag_fat(_tmp17A,sizeof(void*),1U));});});
# 774
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp17E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58F;_tmp58F.tag=0U,({struct _fat_ptr _tmp721=({const char*_tmp17F="assignment to non-lvalue";_tag_fat(_tmp17F,sizeof(char),25U);});_tmp58F.f1=_tmp721;});_tmp58F;});void*_tmp17D[1U];_tmp17D[0]=& _tmp17E;({struct Cyc_Tcenv_Tenv*_tmp724=te;unsigned _tmp723=loc;void**_tmp722=topt;Cyc_Tcexp_expr_err(_tmp724,_tmp723,_tmp722,_tag_fat(_tmp17D,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
Cyc_Tcexp_check_consume(e2->loc,t2,e2);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp726=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp725=e2;Cyc_Tcutil_coerce_assign(_tmp726,_tmp725,t1);})){
void*_tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp593;_tmp593.tag=0U,({struct _fat_ptr _tmp727=({const char*_tmp187="type mismatch: ";_tag_fat(_tmp187,sizeof(char),16U);});_tmp593.f1=_tmp727;});_tmp593;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp183=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp592;_tmp592.tag=2U,_tmp592.f1=(void*)t1;_tmp592;});struct Cyc_Warn_String_Warn_Warg_struct _tmp184=({struct Cyc_Warn_String_Warn_Warg_struct _tmp591;_tmp591.tag=0U,({struct _fat_ptr _tmp728=({const char*_tmp186=" != ";_tag_fat(_tmp186,sizeof(char),5U);});_tmp591.f1=_tmp728;});_tmp591;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp185=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp590;_tmp590.tag=2U,_tmp590.f1=(void*)t2;_tmp590;});void*_tmp181[4U];_tmp181[0]=& _tmp182,_tmp181[1]=& _tmp183,_tmp181[2]=& _tmp184,_tmp181[3]=& _tmp185;({struct Cyc_Tcenv_Tenv*_tmp72B=te;unsigned _tmp72A=loc;void**_tmp729=topt;Cyc_Tcexp_expr_err(_tmp72B,_tmp72A,_tmp729,_tag_fat(_tmp181,sizeof(void*),4U));});});void*result=_tmp180;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}
# 785
return t1;}{
# 787
struct Cyc_Absyn_Exp*_tmp188=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp188;
void*_tmp189=Cyc_Tcexp_tcBinPrimop(te,loc,0,(enum Cyc_Absyn_Primop)po->v,e1copy,e2);void*t_result=_tmp189;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
return t1;
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp18A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp18C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp598;_tmp598.tag=0U,({
struct _fat_ptr _tmp72C=({const char*_tmp193="Cannot use this operator in an assignment when ";_tag_fat(_tmp193,sizeof(char),48U);});_tmp598.f1=_tmp72C;});_tmp598;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp597;_tmp597.tag=0U,({
struct _fat_ptr _tmp72D=({const char*_tmp192="the arguments have types ";_tag_fat(_tmp192,sizeof(char),26U);});_tmp597.f1=_tmp72D;});_tmp597;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp18E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp596;_tmp596.tag=2U,_tmp596.f1=(void*)t1;_tmp596;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp595;_tmp595.tag=0U,({struct _fat_ptr _tmp72E=({const char*_tmp191=" and ";_tag_fat(_tmp191,sizeof(char),6U);});_tmp595.f1=_tmp72E;});_tmp595;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp190=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp594;_tmp594.tag=2U,_tmp594.f1=(void*)t2;_tmp594;});void*_tmp18B[5U];_tmp18B[0]=& _tmp18C,_tmp18B[1]=& _tmp18D,_tmp18B[2]=& _tmp18E,_tmp18B[3]=& _tmp18F,_tmp18B[4]=& _tmp190;({struct Cyc_Tcenv_Tenv*_tmp731=te;unsigned _tmp730=loc;void**_tmp72F=topt;Cyc_Tcexp_expr_err(_tmp731,_tmp730,_tmp72F,_tag_fat(_tmp18B,sizeof(void*),5U));});});
# 792
void*result=_tmp18A;
# 795
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}
# 799
return t_result;}}}}
# 803
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp732=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp732,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp734=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp733=topt;Cyc_Tcexp_tcExp(_tmp734,_tmp733,e2);});
return(void*)_check_null(e2->topt);}
# 810
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 813
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 816
{void*_tmp194=Cyc_Absyn_compress(t1);void*_stmttmp18=_tmp194;void*_tmp195=_stmttmp18;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)->f1 == 0){_LL1: _LL2:
# 818
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
# 821
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 825
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp196=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp196;struct Cyc_Absyn_Datatypefield _tmp197=_stmttmp19;enum Cyc_Absyn_Scope _tmp19B;unsigned _tmp19A;struct Cyc_List_List*_tmp199;struct _tuple1*_tmp198;_LLA: _tmp198=_tmp197.name;_tmp199=_tmp197.typs;_tmp19A=_tmp197.loc;_tmp19B=_tmp197.sc;_LLB: {struct _tuple1*n=_tmp198;struct Cyc_List_List*typs=_tmp199;unsigned loc=_tmp19A;enum Cyc_Absyn_Scope sc=_tmp19B;
# 828
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 831
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 835
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp19C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp19C;struct Cyc_Absyn_Datatypefield _tmp19D=_stmttmp1A;enum Cyc_Absyn_Scope _tmp1A1;unsigned _tmp1A0;struct Cyc_List_List*_tmp19F;struct _tuple1*_tmp19E;_LLD: _tmp19E=_tmp19D.name;_tmp19F=_tmp19D.typs;_tmp1A0=_tmp19D.loc;_tmp1A1=_tmp19D.sc;_LLE: {struct _tuple1*n=_tmp19E;struct Cyc_List_List*typs=_tmp19F;unsigned loc=_tmp1A0;enum Cyc_Absyn_Scope sc=_tmp1A1;
# 838
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 842
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp736=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp735=e;Cyc_Tcutil_coerce_arg(_tmp736,_tmp735,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 846
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59C;_tmp59C.tag=0U,({struct _fat_ptr _tmp737=({const char*_tmp1A8="can't find a field in ";_tag_fat(_tmp1A8,sizeof(char),23U);});_tmp59C.f1=_tmp737;});_tmp59C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59B;_tmp59B.tag=2U,_tmp59B.f1=(void*)tu;_tmp59B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59A;_tmp59A.tag=0U,({struct _fat_ptr _tmp738=({const char*_tmp1A7=" to inject value of type ";_tag_fat(_tmp1A7,sizeof(char),26U);});_tmp59A.f1=_tmp738;});_tmp59A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp599;_tmp599.tag=2U,_tmp599.f1=(void*)t1;_tmp599;});void*_tmp1A2[4U];_tmp1A2[0]=& _tmp1A3,_tmp1A2[1]=& _tmp1A4,_tmp1A2[2]=& _tmp1A5,_tmp1A2[3]=& _tmp1A6;({unsigned _tmp739=e->loc;Cyc_Warn_err2(_tmp739,_tag_fat(_tmp1A2,sizeof(void*),4U));});});
return 0;}
# 851
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 857
struct Cyc_List_List*_tmp1A9=args;struct Cyc_List_List*es=_tmp1A9;
int _tmp1AA=0;int arg_cnt=_tmp1AA;
struct Cyc_Tcenv_Tenv*_tmp1AB=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp1AB;
struct Cyc_Tcenv_Tenv*_tmp1AC=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp1AC;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
# 866
void*_tmp1AD=t;void*_tmp1B3;void*_tmp1B2;void*_tmp1B1;void*_tmp1B0;struct Cyc_Absyn_Tqual _tmp1AF;void*_tmp1AE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->tag == 3U){_LL1: _tmp1AE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).elt_type;_tmp1AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).elt_tq;_tmp1B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).ptr_atts).rgn;_tmp1B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).ptr_atts).nullable;_tmp1B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).ptr_atts).bounds;_tmp1B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).ptr_atts).zero_term;_LL2: {void*t1=_tmp1AE;struct Cyc_Absyn_Tqual tq=_tmp1AF;void*rgn=_tmp1B0;void*x=_tmp1B1;void*b=_tmp1B2;void*zt=_tmp1B3;
# 871
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 873
Cyc_Tcutil_check_nonzero_bound(loc,b);
# 876
t1=({void*_tmp73A=Cyc_Absyn_compress(t1);Cyc_CurRgn_instantiate(_tmp73A,Cyc_Tcenv_curr_lifo_rgn(te));});{
void*_tmp1B4=t1;struct Cyc_Absyn_Vardecl*_tmp1C2;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_List_List*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_VarargInfo*_tmp1BB;int _tmp1BA;struct Cyc_List_List*_tmp1B9;void*_tmp1B8;struct Cyc_Absyn_Tqual _tmp1B7;void*_tmp1B6;struct Cyc_List_List*_tmp1B5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->tag == 5U){_LL6: _tmp1B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).tvars;_tmp1B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).effect;_tmp1B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).ret_tqual;_tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).ret_type;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).args;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).c_varargs;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).cyc_varargs;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).rgn_po;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).attributes;_tmp1BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).requires_clause;_tmp1BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).requires_relns;_tmp1C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).ensures_clause;_tmp1C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).ensures_relns;_tmp1C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).return_value;_LL7: {struct Cyc_List_List*tvars=_tmp1B5;void*eff=_tmp1B6;struct Cyc_Absyn_Tqual res_tq=_tmp1B7;void*res_typ=_tmp1B8;struct Cyc_List_List*args_info=_tmp1B9;int c_vararg=_tmp1BA;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp1BB;struct Cyc_List_List*rgn_po=_tmp1BC;struct Cyc_List_List*atts=_tmp1BD;struct Cyc_Absyn_Exp*req=_tmp1BE;struct Cyc_List_List*req_relns=_tmp1BF;struct Cyc_Absyn_Exp*ens=_tmp1C0;struct Cyc_List_List*ens_relns=_tmp1C1;struct Cyc_Absyn_Vardecl*ret_var=_tmp1C2;
# 881
if(tvars != 0 || rgn_po != 0)
({void*_tmp1C3=0U;({unsigned _tmp73C=e->loc;struct _fat_ptr _tmp73B=({const char*_tmp1C4="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1C4,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp73C,_tmp73B,_tag_fat(_tmp1C3,sizeof(void*),0U));});});
# 885
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 887
while(es != 0 && args_info != 0){
# 889
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp73E=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp73D=e1;Cyc_Tcutil_coerce_arg(_tmp73E,_tmp73D,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1D6="actual argument has type ";_tag_fat(_tmp1D6,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1D5=" but formal has type ";_tag_fat(_tmp1D5,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp741=({unsigned long _tmp740=({unsigned long _tmp73F=Cyc_strlen((struct _fat_ptr)s0);_tmp73F + Cyc_strlen((struct _fat_ptr)s1);});_tmp740 + Cyc_strlen((struct _fat_ptr)s2);});_tmp741 + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A3;_tmp5A3.tag=0U,_tmp5A3.f1=s0;_tmp5A3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A2;_tmp5A2.tag=0U,({struct _fat_ptr _tmp742=({const char*_tmp1CF="\n\t";_tag_fat(_tmp1CF,sizeof(char),3U);});_tmp5A2.f1=_tmp742;});_tmp5A2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A1;_tmp5A1.tag=0U,_tmp5A1.f1=s1;_tmp5A1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=0U,({struct _fat_ptr _tmp743=({const char*_tmp1CE="\n";_tag_fat(_tmp1CE,sizeof(char),2U);});_tmp5A0.f1=_tmp743;});_tmp5A0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59F;_tmp59F.tag=0U,_tmp59F.f1=s2;_tmp59F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59E;_tmp59E.tag=0U,({struct _fat_ptr _tmp744=({const char*_tmp1CD="\n\t";_tag_fat(_tmp1CD,sizeof(char),3U);});_tmp59E.f1=_tmp744;});_tmp59E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59D;_tmp59D.tag=0U,_tmp59D.f1=s3;_tmp59D;});void*_tmp1C5[7U];_tmp1C5[0]=& _tmp1C6,_tmp1C5[1]=& _tmp1C7,_tmp1C5[2]=& _tmp1C8,_tmp1C5[3]=& _tmp1C9,_tmp1C5[4]=& _tmp1CA,_tmp1C5[5]=& _tmp1CB,_tmp1C5[6]=& _tmp1CC;({unsigned _tmp745=e1->loc;Cyc_Warn_err2(_tmp745,_tag_fat(_tmp1C5,sizeof(void*),7U));});});else{
# 901
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=0U,_tmp5A7.f1=s0;_tmp5A7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=0U,_tmp5A6.f1=s1;_tmp5A6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=0U,_tmp5A5.f1=s2;_tmp5A5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A4;_tmp5A4.tag=0U,_tmp5A4.f1=s3;_tmp5A4;});void*_tmp1D0[4U];_tmp1D0[0]=& _tmp1D1,_tmp1D0[1]=& _tmp1D2,_tmp1D0[2]=& _tmp1D3,_tmp1D0[3]=& _tmp1D4;({unsigned _tmp746=e1->loc;Cyc_Warn_err2(_tmp746,_tag_fat(_tmp1D0,sizeof(void*),4U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 906
if(alias_coercion)
({struct Cyc_List_List*_tmp747=({struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->hd=(void*)arg_cnt,_tmp1D7->tl=*alias_arg_exps;_tmp1D7;});*alias_arg_exps=_tmp747;});
Cyc_Tcexp_check_consume(e1->loc,t2,e1);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 916
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1D8=(void*)a->hd;void*_stmttmp1B=_tmp1D8;void*_tmp1D9=_stmttmp1B;int _tmp1DC;int _tmp1DB;enum Cyc_Absyn_Format_Type _tmp1DA;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D9)->tag == 19U){_LLB: _tmp1DA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D9)->f1;_tmp1DB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D9)->f2;_tmp1DC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D9)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp1DA;int fmt_arg_pos=_tmp1DB;int arg_start_pos=_tmp1DC;
# 920
{struct _handler_cons _tmp1DD;_push_handler(& _tmp1DD);{int _tmp1DF=0;if(setjmp(_tmp1DD.handler))_tmp1DF=1;if(!_tmp1DF){
# 922
{struct Cyc_Absyn_Exp*_tmp1E0=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1E0;
# 924
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 928
fmt_args=({struct Cyc_Core_Opt*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));({struct Cyc_List_List*_tmp748=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp1E1->v=_tmp748;});_tmp1E1;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1E2=ft;switch(_tmp1E2){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 932
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 935
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 938
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 941
goto _LLF;}_LLF:;}}
# 922
;_pop_handler();}else{void*_tmp1DE=(void*)Cyc_Core_get_exn_thrown();void*_tmp1E3=_tmp1DE;void*_tmp1E4;if(((struct Cyc_List_Nth_exn_struct*)_tmp1E3)->tag == Cyc_List_Nth){_LL17: _LL18:
# 943
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=0U,({struct _fat_ptr _tmp749=({const char*_tmp1E7="bad format arguments";_tag_fat(_tmp1E7,sizeof(char),21U);});_tmp5A8.f1=_tmp749;});_tmp5A8;});void*_tmp1E5[1U];_tmp1E5[0]=& _tmp1E6;({unsigned _tmp74A=loc;Cyc_Warn_err2(_tmp74A,_tag_fat(_tmp1E5,sizeof(void*),1U));});});goto _LL16;}else{_LL19: _tmp1E4=_tmp1E3;_LL1A: {void*exn=_tmp1E4;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 948
if(args_info != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=0U,({struct _fat_ptr _tmp74B=({const char*_tmp1EA="too few arguments for function";_tag_fat(_tmp1EA,sizeof(char),31U);});_tmp5A9.f1=_tmp74B;});_tmp5A9;});void*_tmp1E8[1U];_tmp1E8[0]=& _tmp1E9;({unsigned _tmp74C=loc;Cyc_Warn_err2(_tmp74C,_tag_fat(_tmp1E8,sizeof(void*),1U));});});else{
# 951
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=0U,({struct _fat_ptr _tmp74D=({const char*_tmp1ED="too many arguments for function";_tag_fat(_tmp1ED,sizeof(char),32U);});_tmp5AA.f1=_tmp74D;});_tmp5AA;});void*_tmp1EB[1U];_tmp1EB[0]=& _tmp1EC;({unsigned _tmp74E=loc;Cyc_Warn_err2(_tmp74E,_tag_fat(_tmp1EB,sizeof(void*),1U));});});else{
# 958
struct Cyc_Absyn_VarargInfo _tmp1EE=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1C=_tmp1EE;struct Cyc_Absyn_VarargInfo _tmp1EF=_stmttmp1C;int _tmp1F1;void*_tmp1F0;_LL1C: _tmp1F0=_tmp1EF.type;_tmp1F1=_tmp1EF.inject;_LL1D: {void*vt=_tmp1F0;int inject=_tmp1F1;
struct Cyc_Absyn_VarargCallInfo*_tmp1F2=({struct Cyc_Absyn_VarargCallInfo*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->num_varargs=0,_tmp20A->injectors=0,_tmp20A->vai=cyc_vararg;_tmp20A;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1F2;
# 962
*vararg_call_info=vci;
# 964
if(!inject)
# 966
for(0;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp750=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp74F=e1;Cyc_Tcutil_coerce_arg(_tmp750,_tmp74F,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AE;_tmp5AE.tag=0U,({struct _fat_ptr _tmp751=({const char*_tmp1F9="vararg requires type ";_tag_fat(_tmp1F9,sizeof(char),22U);});_tmp5AE.f1=_tmp751;});_tmp5AE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AD;_tmp5AD.tag=2U,_tmp5AD.f1=(void*)vt;_tmp5AD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AC;_tmp5AC.tag=0U,({
struct _fat_ptr _tmp752=({const char*_tmp1F8=" but argument has type ";_tag_fat(_tmp1F8,sizeof(char),24U);});_tmp5AC.f1=_tmp752;});_tmp5AC;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1F7=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5AB;_tmp5AB.tag=3U,_tmp5AB.f1=(void*)e1->topt;_tmp5AB;});void*_tmp1F3[4U];_tmp1F3[0]=& _tmp1F4,_tmp1F3[1]=& _tmp1F5,_tmp1F3[2]=& _tmp1F6,_tmp1F3[3]=& _tmp1F7;({unsigned _tmp753=loc;Cyc_Tcexp_err_and_explain(_tmp753,_tag_fat(_tmp1F3,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp754=({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->hd=(void*)arg_cnt,_tmp1FA->tl=*alias_arg_exps;_tmp1FA;});*alias_arg_exps=_tmp754;});
Cyc_Tcexp_check_consume(e1->loc,vt,e1);}else{
# 981
void*_tmp1FB=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1D=_tmp1FB;void*_tmp1FC=_stmttmp1D;struct Cyc_List_List*_tmp1FE;struct Cyc_Absyn_Datatypedecl*_tmp1FD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FC)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FC)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FC)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1FD=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FC)->f1)->f1).KnownDatatype).val;_tmp1FE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FC)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp1FD;struct Cyc_List_List*targs=_tmp1FE;
# 985
struct Cyc_Absyn_Datatypedecl*_tmp1FF=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp1FF;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp201=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B0;_tmp5B0.tag=0U,({struct _fat_ptr _tmp755=({const char*_tmp203="can't inject into abstract ";_tag_fat(_tmp203,sizeof(char),28U);});_tmp5B0.f1=_tmp755;});_tmp5B0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp202=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AF;_tmp5AF.tag=2U,_tmp5AF.f1=(void*)vt;_tmp5AF;});void*_tmp200[2U];_tmp200[0]=& _tmp201,_tmp200[1]=& _tmp202;({unsigned _tmp756=loc;Cyc_Warn_err2(_tmp756,_tag_fat(_tmp200,sizeof(void*),2U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 995
({void*_tmp757=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp757,Cyc_Tcenv_curr_rgn(te));});{
# 997
struct Cyc_List_List*_tmp204=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp204;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1002
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_check_consume(e1->loc,(void*)_check_null(e1->topt),e1);}{
# 1006
struct Cyc_Absyn_Datatypefield*_tmp205=({struct Cyc_Tcenv_Tenv*_tmp75B=te;struct Cyc_Absyn_Exp*_tmp75A=e1;void*_tmp759=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp758=inst;Cyc_Tcexp_tcInjection(_tmp75B,_tmp75A,_tmp759,_tmp758,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp205;
if(f != 0)
({struct Cyc_List_List*_tmp75D=({
struct Cyc_List_List*_tmp75C=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp75C,({struct Cyc_List_List*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->hd=f,_tmp206->tl=0;_tmp206;}));});
# 1008
vci->injectors=_tmp75D;});}}}
# 1011
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp208=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B1;_tmp5B1.tag=0U,({struct _fat_ptr _tmp75E=({const char*_tmp209="bad inject vararg type";_tag_fat(_tmp209,sizeof(char),23U);});_tmp5B1.f1=_tmp75E;});_tmp5B1;});void*_tmp207[1U];_tmp207[0]=& _tmp208;({unsigned _tmp75F=loc;Cyc_Warn_err2(_tmp75F,_tag_fat(_tmp207,sizeof(void*),1U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1017
if(*alias_arg_exps == 0)
# 1026 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1028
return res_typ;}}}else{_LL8: _LL9:
# 1030
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp20C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B3;_tmp5B3.tag=0U,({struct _fat_ptr _tmp760=({const char*_tmp20E="expected pointer to function but found ";_tag_fat(_tmp20E,sizeof(char),40U);});_tmp5B3.f1=_tmp760;});_tmp5B3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp20D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B2;_tmp5B2.tag=2U,_tmp5B2.f1=(void*)t;_tmp5B2;});void*_tmp20B[2U];_tmp20B[0]=& _tmp20C,_tmp20B[1]=& _tmp20D;({struct Cyc_Tcenv_Tenv*_tmp763=te;unsigned _tmp762=loc;void**_tmp761=topt;Cyc_Tcexp_expr_err(_tmp763,_tmp762,_tmp761,_tag_fat(_tmp20B,sizeof(void*),2U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1033
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp210=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B5;_tmp5B5.tag=0U,({struct _fat_ptr _tmp764=({const char*_tmp212="expected pointer to function but found ";_tag_fat(_tmp212,sizeof(char),40U);});_tmp5B5.f1=_tmp764;});_tmp5B5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp211=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B4;_tmp5B4.tag=2U,_tmp5B4.f1=(void*)t;_tmp5B4;});void*_tmp20F[2U];_tmp20F[0]=& _tmp210,_tmp20F[1]=& _tmp211;({struct Cyc_Tcenv_Tenv*_tmp767=te;unsigned _tmp766=loc;void**_tmp765=topt;Cyc_Tcexp_expr_err(_tmp767,_tmp766,_tmp765,_tag_fat(_tmp20F,sizeof(void*),2U));});});}_LL0:;}}}
# 1037
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp213=Cyc_Absyn_exn_type();void*exception_type=_tmp213;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp769=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp768=e;Cyc_Tcutil_coerce_arg(_tmp769,_tmp768,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp215=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B9;_tmp5B9.tag=0U,({struct _fat_ptr _tmp76A=({const char*_tmp21A="expected ";_tag_fat(_tmp21A,sizeof(char),10U);});_tmp5B9.f1=_tmp76A;});_tmp5B9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp216=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B8;_tmp5B8.tag=2U,_tmp5B8.f1=(void*)exception_type;_tmp5B8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp217=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B7;_tmp5B7.tag=0U,({struct _fat_ptr _tmp76B=({const char*_tmp219=" but found ";_tag_fat(_tmp219,sizeof(char),12U);});_tmp5B7.f1=_tmp76B;});_tmp5B7;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp218=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5B6;_tmp5B6.tag=3U,_tmp5B6.f1=(void*)e->topt;_tmp5B6;});void*_tmp214[4U];_tmp214[0]=& _tmp215,_tmp214[1]=& _tmp216,_tmp214[2]=& _tmp217,_tmp214[3]=& _tmp218;({unsigned _tmp76C=loc;Cyc_Warn_err2(_tmp76C,_tag_fat(_tmp214,sizeof(void*),4U));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1048
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1050
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
{void*_tmp21B=t1;struct Cyc_Absyn_PtrAtts _tmp21E;struct Cyc_Absyn_Tqual _tmp21D;void*_tmp21C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->tag == 3U){_LL1: _tmp21C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->f1).elt_type;_tmp21D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->f1).elt_tq;_tmp21E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->f1).ptr_atts;_LL2: {void*t=_tmp21C;struct Cyc_Absyn_Tqual tq=_tmp21D;struct Cyc_Absyn_PtrAtts atts=_tmp21E;
# 1053
{void*_tmp21F=Cyc_Absyn_compress(t);void*_stmttmp1E=_tmp21F;void*_tmp220=_stmttmp1E;struct Cyc_Absyn_FnInfo _tmp221;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp220)->tag == 5U){_LL6: _tmp221=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp220)->f1;_LL7: {struct Cyc_Absyn_FnInfo info=_tmp221;
# 1055
struct Cyc_List_List*_tmp222=info.tvars;struct Cyc_List_List*tvars=_tmp222;
struct Cyc_List_List*instantiation=0;
# 1058
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);
({unsigned _tmp770=loc;struct Cyc_Tcenv_Tenv*_tmp76F=te;struct Cyc_List_List*_tmp76E=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp76D=k;Cyc_Tctyp_check_type(_tmp770,_tmp76F,_tmp76E,_tmp76D,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp224=_cycalloc(sizeof(*_tmp224));({struct _tuple14*_tmp771=({struct _tuple14*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp223->f2=(void*)ts->hd;_tmp223;});_tmp224->hd=_tmp771;}),_tmp224->tl=instantiation;_tmp224;});}
# 1064
info.tvars=tvars;
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp226=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BA;_tmp5BA.tag=0U,({struct _fat_ptr _tmp772=({const char*_tmp227="too many type variables in instantiation";_tag_fat(_tmp227,sizeof(char),41U);});_tmp5BA.f1=_tmp772;});_tmp5BA;});void*_tmp225[1U];_tmp225[0]=& _tmp226;({struct Cyc_Tcenv_Tenv*_tmp775=te;unsigned _tmp774=loc;void**_tmp773=topt;Cyc_Tcexp_expr_err(_tmp775,_tmp774,_tmp773,_tag_fat(_tmp225,sizeof(void*),1U));});});
# 1070
if(tvars == 0){
({struct Cyc_List_List*_tmp776=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,info.rgn_po);info.rgn_po=_tmp776;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,info.rgn_po);
info.rgn_po=0;}{
# 1075
void*new_fn_typ=({struct Cyc_List_List*_tmp777=instantiation;Cyc_Tcutil_substitute(_tmp777,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228->tag=5U,_tmp228->f1=info;_tmp228;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp5BB;_tmp5BB.elt_type=new_fn_typ,_tmp5BB.elt_tq=tq,_tmp5BB.ptr_atts=atts;_tmp5BB;}));}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1079
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1082
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp22A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BD;_tmp5BD.tag=0U,({struct _fat_ptr _tmp778=({const char*_tmp22C="expecting polymorphic type but found ";_tag_fat(_tmp22C,sizeof(char),38U);});_tmp5BD.f1=_tmp778;});_tmp5BD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp22B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BC;_tmp5BC.tag=2U,_tmp5BC.f1=(void*)t1;_tmp5BC;});void*_tmp229[2U];_tmp229[0]=& _tmp22A,_tmp229[1]=& _tmp22B;({struct Cyc_Tcenv_Tenv*_tmp77B=te;unsigned _tmp77A=loc;void**_tmp779=topt;Cyc_Tcexp_expr_err(_tmp77B,_tmp77A,_tmp779,_tag_fat(_tmp229,sizeof(void*),2U));});});}
# 1086
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1088
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1090
({void*_tmp77C=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp77C;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1095
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1097
({unsigned _tmp780=loc;struct Cyc_Tcenv_Tenv*_tmp77F=te;struct Cyc_List_List*_tmp77E=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp77D=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1097
Cyc_Tctyp_check_type(_tmp780,_tmp77F,_tmp77E,_tmp77D,1,0,t);});
# 1099
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp783=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp782=loc;void*_tmp781=t2;Cyc_Tcutil_silent_castable(_tmp783,_tmp782,_tmp781,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1105
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp786=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp785=loc;void*_tmp784=t2;Cyc_Tcutil_castable(_tmp786,_tmp785,_tmp784,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1112
Cyc_Unify_unify(t2,t);{
void*_tmp22D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C1;_tmp5C1.tag=0U,({struct _fat_ptr _tmp787=({const char*_tmp234="cannot cast ";_tag_fat(_tmp234,sizeof(char),13U);});_tmp5C1.f1=_tmp787;});_tmp5C1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp230=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C0;_tmp5C0.tag=2U,_tmp5C0.f1=(void*)t2;_tmp5C0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp231=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BF;_tmp5BF.tag=0U,({struct _fat_ptr _tmp788=({const char*_tmp233=" to ";_tag_fat(_tmp233,sizeof(char),5U);});_tmp5BF.f1=_tmp788;});_tmp5BF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp232=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BE;_tmp5BE.tag=2U,_tmp5BE.f1=(void*)t;_tmp5BE;});void*_tmp22E[4U];_tmp22E[0]=& _tmp22F,_tmp22E[1]=& _tmp230,_tmp22E[2]=& _tmp231,_tmp22E[3]=& _tmp232;({struct Cyc_Tcenv_Tenv*_tmp78A=te;unsigned _tmp789=loc;Cyc_Tcexp_expr_err(_tmp78A,_tmp789,& t,_tag_fat(_tmp22E,sizeof(void*),4U));});});void*result=_tmp22D;
Cyc_Unify_explain_failure();
return result;}}}}{
# 1121
struct _tuple0 _tmp235=({struct _tuple0 _tmp5C2;_tmp5C2.f1=e->r,({void*_tmp78B=Cyc_Absyn_compress(t);_tmp5C2.f2=_tmp78B;});_tmp5C2;});struct _tuple0 _stmttmp1F=_tmp235;struct _tuple0 _tmp236=_stmttmp1F;void*_tmp23A;void*_tmp239;void*_tmp238;int _tmp237;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp236.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236.f2)->tag == 3U){_LL1: _tmp237=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp236.f1)->f1).fat_result;_tmp238=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236.f2)->f1).ptr_atts).nullable;_tmp239=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236.f2)->f1).ptr_atts).bounds;_tmp23A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236.f2)->f1).ptr_atts).zero_term;_LL2: {int fat_result=_tmp237;void*nbl=_tmp238;void*bds=_tmp239;void*zt=_tmp23A;
# 1125
if((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl)){
struct Cyc_Absyn_Exp*_tmp23B=({void*_tmp78C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp78C,bds);});struct Cyc_Absyn_Exp*eopt=_tmp23B;
if(eopt != 0){
if((Cyc_Evexp_eval_const_uint_exp(e)).f1 == (unsigned)1)
({void*_tmp23C=0U;({unsigned _tmp78E=loc;struct _fat_ptr _tmp78D=({const char*_tmp23D="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp23D,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp78E,_tmp78D,_tag_fat(_tmp23C,sizeof(void*),0U));});});}}
# 1131
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;}_LL0:;}}}
# 1137
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp23E=0;void**topt2=_tmp23E;
struct Cyc_Absyn_Tqual _tmp23F=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp23F;
int _tmp240=0;int nullable=_tmp240;
if(topt != 0){
void*_tmp241=Cyc_Absyn_compress(*topt);void*_stmttmp20=_tmp241;void*_tmp242=_stmttmp20;void*_tmp245;struct Cyc_Absyn_Tqual _tmp244;void**_tmp243;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp242)->tag == 3U){_LL1: _tmp243=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp242)->f1).elt_type;_tmp244=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp242)->f1).elt_tq;_tmp245=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp242)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp243;struct Cyc_Absyn_Tqual tq=_tmp244;void*n=_tmp245;
# 1144
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1158
({struct Cyc_Tcenv_Tenv*_tmp790=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp78F=topt2;Cyc_Tcexp_tcExpNoInst(_tmp790,_tmp78F,e);});
# 1160
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp247=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3;_tmp5C3.tag=0U,({struct _fat_ptr _tmp791=({const char*_tmp248="cannot take the address of an alias-free path";_tag_fat(_tmp248,sizeof(char),46U);});_tmp5C3.f1=_tmp791;});_tmp5C3;});void*_tmp246[1U];_tmp246[0]=& _tmp247;({unsigned _tmp792=e->loc;Cyc_Warn_err2(_tmp792,_tag_fat(_tmp246,sizeof(void*),1U));});});
# 1165
{void*_tmp249=e->r;void*_stmttmp21=_tmp249;void*_tmp24A=_stmttmp21;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_Absyn_Exp*_tmp24B;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24A)->tag == 23U){_LL6: _tmp24B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24A)->f1;_tmp24C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24A)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp24B;struct Cyc_Absyn_Exp*e2=_tmp24C;
# 1167
{void*_tmp24D=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp22=_tmp24D;void*_tmp24E=_stmttmp22;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24E)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1172
({void*_tmp793=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp793;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1175
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1180
{void*_tmp24F=e->r;void*_stmttmp23=_tmp24F;void*_tmp250=_stmttmp23;switch(*((int*)_tmp250)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp250)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp250)->f3 == 1){_LL12: _LL13:
# 1183
({struct Cyc_Warn_String_Warn_Warg_struct _tmp252=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C4;_tmp5C4.tag=0U,({struct _fat_ptr _tmp794=({const char*_tmp253="cannot take the address of a @tagged union member";_tag_fat(_tmp253,sizeof(char),50U);});_tmp5C4.f1=_tmp794;});_tmp5C4;});void*_tmp251[1U];_tmp251[0]=& _tmp252;({unsigned _tmp795=e->loc;Cyc_Warn_err2(_tmp795,_tag_fat(_tmp251,sizeof(void*),1U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1189
struct _tuple15 _tmp254=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp24=_tmp254;struct _tuple15 _tmp255=_stmttmp24;void*_tmp257;int _tmp256;_LL17: _tmp256=_tmp255.f1;_tmp257=_tmp255.f2;_LL18: {int is_const=_tmp256;void*rgn=_tmp257;
# 1191
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp259=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C5;_tmp5C5.tag=0U,({struct _fat_ptr _tmp796=({const char*_tmp25A="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp25A,sizeof(char),60U);});_tmp5C5.f1=_tmp796;});_tmp5C5;});void*_tmp258[1U];_tmp258[0]=& _tmp259;({unsigned _tmp797=e->loc;Cyc_Warn_err2(_tmp797,_tag_fat(_tmp258,sizeof(void*),1U));});});{
# 1194
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _tmp798=tq.real_const=1;tq.print_const=_tmp798;});
# 1198
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type);}}}}
# 1202
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1205
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _tmp79B=loc;struct Cyc_Tcenv_Tenv*_tmp79A=te;struct Cyc_List_List*_tmp799=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp79B,_tmp79A,_tmp799,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C8;_tmp5C8.tag=0U,({struct _fat_ptr _tmp79C=({const char*_tmp260="sizeof applied to type ";_tag_fat(_tmp260,sizeof(char),24U);});_tmp5C8.f1=_tmp79C;});_tmp5C8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp25D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C7;_tmp5C7.tag=2U,_tmp5C7.f1=(void*)t;_tmp5C7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C6;_tmp5C6.tag=0U,({struct _fat_ptr _tmp79D=({const char*_tmp25F=" which has unknown size here";_tag_fat(_tmp25F,sizeof(char),29U);});_tmp5C6.f1=_tmp79D;});_tmp5C6;});void*_tmp25B[3U];_tmp25B[0]=& _tmp25C,_tmp25B[1]=& _tmp25D,_tmp25B[2]=& _tmp25E;({unsigned _tmp79E=loc;Cyc_Warn_err2(_tmp79E,_tag_fat(_tmp25B,sizeof(void*),3U));});});
if(topt == 0)
return Cyc_Absyn_uint_type;{
void*_tmp261=Cyc_Absyn_compress(*topt);void*_stmttmp25=_tmp261;void*_tmp262=_stmttmp25;void*_tmp263;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp262)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp262)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp262)->f2 != 0){_LL1: _tmp263=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp262)->f2)->hd;_LL2: {void*tt=_tmp263;
# 1215
if(({void*_tmp7A0=tt;Cyc_Unify_unify(_tmp7A0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264->tag=9U,({struct Cyc_Absyn_Exp*_tmp79F=Cyc_Absyn_sizeoftype_exp(t,0U);_tmp264->f1=_tmp79F;});_tmp264;}));}))
return Cyc_Absyn_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_uint_type;}_LL0:;}}
# 1222
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)
return sf->type;
return 0;}
# 1230
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1232
({unsigned _tmp7A3=loc;struct Cyc_Tcenv_Tenv*_tmp7A2=te;struct Cyc_List_List*_tmp7A1=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7A3,_tmp7A2,_tmp7A1,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp265=fs;struct Cyc_List_List*l=_tmp265;
void*_tmp266=t_orig;void*t=_tmp266;
for(0;l != 0;l=l->tl){
void*_tmp267=(void*)l->hd;void*n=_tmp267;
void*_tmp268=n;unsigned _tmp269;struct _fat_ptr*_tmp26A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp268)->tag == 0U){_LL1: _tmp26A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp268)->f1;_LL2: {struct _fat_ptr*n=_tmp26A;
# 1240
int bad_type=1;
{void*_tmp26B=Cyc_Absyn_compress(t);void*_stmttmp26=_tmp26B;void*_tmp26C=_stmttmp26;struct Cyc_List_List*_tmp26D;struct Cyc_Absyn_Aggrdecl**_tmp26E;switch(*((int*)_tmp26C)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp26E=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp26E;
# 1243
if((*adp)->impl == 0)
goto _LL5;
_tmp26D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL9;}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp26D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp26C)->f2;_LL9: {struct Cyc_List_List*fields=_tmp26D;
# 1247
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp270=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=0U,({struct _fat_ptr _tmp7A4=({const char*_tmp272="no field of struct/union has name %s";_tag_fat(_tmp272,sizeof(char),37U);});_tmp5CA.f1=_tmp7A4;});_tmp5CA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp271=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=0U,_tmp5C9.f1=*n;_tmp5C9;});void*_tmp26F[2U];_tmp26F[0]=& _tmp270,_tmp26F[1]=& _tmp271;({unsigned _tmp7A5=loc;Cyc_Warn_err2(_tmp7A5,_tag_fat(_tmp26F,sizeof(void*),2U));});});else{
# 1251
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1256
if(bad_type){
if(l == fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp274=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5CC;_tmp5CC.tag=2U,_tmp5CC.f1=(void*)t;_tmp5CC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp275=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=0U,({struct _fat_ptr _tmp7A6=({const char*_tmp276=" is not a known struct/union type";_tag_fat(_tmp276,sizeof(char),34U);});_tmp5CB.f1=_tmp7A6;});_tmp5CB;});void*_tmp273[2U];_tmp273[0]=& _tmp274,_tmp273[1]=& _tmp275;({unsigned _tmp7A7=loc;Cyc_Warn_err2(_tmp7A7,_tag_fat(_tmp273,sizeof(void*),2U));});});else{
# 1260
struct _fat_ptr _tmp277=({struct Cyc_String_pa_PrintArg_struct _tmp28D=({struct Cyc_String_pa_PrintArg_struct _tmp5D5;_tmp5D5.tag=0U,({struct _fat_ptr _tmp7A8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp5D5.f1=_tmp7A8;});_tmp5D5;});void*_tmp28B[1U];_tmp28B[0]=& _tmp28D;({struct _fat_ptr _tmp7A9=({const char*_tmp28C="(%s)";_tag_fat(_tmp28C,sizeof(char),5U);});Cyc_aprintf(_tmp7A9,_tag_fat(_tmp28B,sizeof(void*),1U));});});struct _fat_ptr s=_tmp277;
{struct Cyc_List_List*x=fs;for(0;x != l;x=x->tl){
void*_tmp278=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp27=_tmp278;void*_tmp279=_stmttmp27;unsigned _tmp27A;struct _fat_ptr*_tmp27B;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp279)->tag == 0U){_LLD: _tmp27B=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp279)->f1;_LLE: {struct _fat_ptr*n=_tmp27B;
s=({struct Cyc_String_pa_PrintArg_struct _tmp27E=({struct Cyc_String_pa_PrintArg_struct _tmp5CE;_tmp5CE.tag=0U,_tmp5CE.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5CE;});struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp5CD;_tmp5CD.tag=0U,_tmp5CD.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5CD;});void*_tmp27C[2U];_tmp27C[0]=& _tmp27E,_tmp27C[1]=& _tmp27F;({struct _fat_ptr _tmp7AA=({const char*_tmp27D="%s.%s";_tag_fat(_tmp27D,sizeof(char),6U);});Cyc_aprintf(_tmp7AA,_tag_fat(_tmp27C,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp27A=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp279)->f1;_LL10: {unsigned n=_tmp27A;
s=({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp5D0;_tmp5D0.tag=0U,_tmp5D0.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5D0;});struct Cyc_Int_pa_PrintArg_struct _tmp283=({struct Cyc_Int_pa_PrintArg_struct _tmp5CF;_tmp5CF.tag=1U,_tmp5CF.f1=(unsigned long)((int)n);_tmp5CF;});void*_tmp280[2U];_tmp280[0]=& _tmp282,_tmp280[1]=& _tmp283;({struct _fat_ptr _tmp7AB=({const char*_tmp281="%s.%d";_tag_fat(_tmp281,sizeof(char),6U);});Cyc_aprintf(_tmp7AB,_tag_fat(_tmp280,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}}
# 1266
({struct Cyc_Warn_String_Warn_Warg_struct _tmp285=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=0U,_tmp5D4.f1=(struct _fat_ptr)s;_tmp5D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp286=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=0U,({struct _fat_ptr _tmp7AC=({const char*_tmp28A=" == ";_tag_fat(_tmp28A,sizeof(char),5U);});_tmp5D3.f1=_tmp7AC;});_tmp5D3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp287=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D2;_tmp5D2.tag=2U,_tmp5D2.f1=(void*)t;_tmp5D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp288=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D1;_tmp5D1.tag=0U,({struct _fat_ptr _tmp7AD=({const char*_tmp289=" is not a struct/union type";_tag_fat(_tmp289,sizeof(char),28U);});_tmp5D1.f1=_tmp7AD;});_tmp5D1;});void*_tmp284[4U];_tmp284[0]=& _tmp285,_tmp284[1]=& _tmp286,_tmp284[2]=& _tmp287,_tmp284[3]=& _tmp288;({unsigned _tmp7AE=loc;Cyc_Warn_err2(_tmp7AE,_tag_fat(_tmp284,sizeof(void*),4U));});});}}
# 1269
goto _LL0;}}else{_LL3: _tmp269=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp268)->f1;_LL4: {unsigned n=_tmp269;
# 1271
int bad_type=1;
{void*_tmp28E=Cyc_Absyn_compress(t);void*_stmttmp28=_tmp28E;void*_tmp28F=_stmttmp28;struct Cyc_List_List*_tmp290;struct Cyc_List_List*_tmp291;struct Cyc_Absyn_Datatypefield*_tmp292;struct Cyc_Absyn_Aggrdecl**_tmp293;switch(*((int*)_tmp28F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28F)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28F)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp293=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28F)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp293;
# 1274
if((*adp)->impl == 0)
goto _LL11;
_tmp291=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28F)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp292=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28F)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp292;
# 1293
if(({unsigned _tmp7AF=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp7AF < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp2A4=({struct Cyc_Int_pa_PrintArg_struct _tmp5D7;_tmp5D7.tag=1U,({
unsigned long _tmp7B0=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp5D7.f1=_tmp7B0;});_tmp5D7;});struct Cyc_Int_pa_PrintArg_struct _tmp2A5=({struct Cyc_Int_pa_PrintArg_struct _tmp5D6;_tmp5D6.tag=1U,_tmp5D6.f1=(unsigned long)((int)n);_tmp5D6;});void*_tmp2A2[2U];_tmp2A2[0]=& _tmp2A4,_tmp2A2[1]=& _tmp2A5;({unsigned _tmp7B2=loc;struct _fat_ptr _tmp7B1=({const char*_tmp2A3="datatype field has too few components: %d < %d";_tag_fat(_tmp2A3,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp7B2,_tmp7B1,_tag_fat(_tmp2A2,sizeof(void*),2U));});});else{
# 1297
if(n != (unsigned)0)
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp2A6=0U;({unsigned _tmp7B4=loc;struct _fat_ptr _tmp7B3=({const char*_tmp2A7="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp2A7,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp7B4,_tmp7B3,_tag_fat(_tmp2A6,sizeof(void*),0U));});});}}
# 1302
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp291=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp28F)->f2;_LL15: {struct Cyc_List_List*fields=_tmp291;
# 1278
if(({unsigned _tmp7B5=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp7B5 <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp295=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DB;_tmp5DB.tag=0U,({struct _fat_ptr _tmp7B6=({const char*_tmp29A="struct/union has too few components: ";_tag_fat(_tmp29A,sizeof(char),38U);});_tmp5DB.f1=_tmp7B6;});_tmp5DB;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp296=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5DA;_tmp5DA.tag=12U,({
int _tmp7B7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp5DA.f1=_tmp7B7;});_tmp5DA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp297=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D9;_tmp5D9.tag=0U,({struct _fat_ptr _tmp7B8=({const char*_tmp299=" <= ";_tag_fat(_tmp299,sizeof(char),5U);});_tmp5D9.f1=_tmp7B8;});_tmp5D9;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp298=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5D8;_tmp5D8.tag=12U,_tmp5D8.f1=(int)n;_tmp5D8;});void*_tmp294[4U];_tmp294[0]=& _tmp295,_tmp294[1]=& _tmp296,_tmp294[2]=& _tmp297,_tmp294[3]=& _tmp298;({unsigned _tmp7B9=loc;Cyc_Warn_err2(_tmp7B9,_tag_fat(_tmp294,sizeof(void*),4U));});});else{
# 1282
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp290=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp28F)->f1;_LL17: {struct Cyc_List_List*l=_tmp290;
# 1286
if(({unsigned _tmp7BA=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp7BA <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DF;_tmp5DF.tag=0U,({struct _fat_ptr _tmp7BB=({const char*_tmp2A1="tuple has too few components: ";_tag_fat(_tmp2A1,sizeof(char),31U);});_tmp5DF.f1=_tmp7BB;});_tmp5DF;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp29D=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5DE;_tmp5DE.tag=12U,({int _tmp7BC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp5DE.f1=_tmp7BC;});_tmp5DE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DD;_tmp5DD.tag=0U,({struct _fat_ptr _tmp7BD=({const char*_tmp2A0=" <= ";_tag_fat(_tmp2A0,sizeof(char),5U);});_tmp5DD.f1=_tmp7BD;});_tmp5DD;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp29F=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=12U,_tmp5DC.f1=(int)n;_tmp5DC;});void*_tmp29B[4U];_tmp29B[0]=& _tmp29C,_tmp29B[1]=& _tmp29D,_tmp29B[2]=& _tmp29E,_tmp29B[3]=& _tmp29F;({unsigned _tmp7BE=loc;Cyc_Warn_err2(_tmp7BE,_tag_fat(_tmp29B,sizeof(void*),4U));});});else{
# 1289
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1304
 goto _LL11;}_LL11:;}
# 1306
if(bad_type)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E1;_tmp5E1.tag=2U,_tmp5E1.f1=(void*)t;_tmp5E1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E0;_tmp5E0.tag=0U,({struct _fat_ptr _tmp7BF=({const char*_tmp2AB=" is not a known type";_tag_fat(_tmp2AB,sizeof(char),21U);});_tmp5E0.f1=_tmp7BF;});_tmp5E0;});void*_tmp2A8[2U];_tmp2A8[0]=& _tmp2A9,_tmp2A8[1]=& _tmp2AA;({unsigned _tmp7C0=loc;Cyc_Warn_err2(_tmp7C0,_tag_fat(_tmp2A8,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 1311
return Cyc_Absyn_uint_type;}}
# 1315
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
{void*_tmp2AC=t;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AC)->tag == 1U){_LL1: _LL2: {
# 1321
struct Cyc_Absyn_PtrInfo _tmp2AD=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp2AD;
({void*_tmp7C2=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));({struct Cyc_List_List*_tmp7C1=Cyc_Tcenv_lookup_type_vars(te);_tmp2AE->v=_tmp7C1;});_tmp2AE;}));pi.elt_type=_tmp7C2;});{
void*_tmp2AF=Cyc_Absyn_pointer_type(pi);void*new_typ=_tmp2AF;
Cyc_Unify_unify(t,new_typ);
t=Cyc_Absyn_compress(t);
goto _LL0;}}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1329
void*_tmp2B0=t;void*_tmp2B4;void*_tmp2B3;void*_tmp2B2;void*_tmp2B1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0)->tag == 3U){_LL6: _tmp2B1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0)->f1).elt_type;_tmp2B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0)->f1).ptr_atts).rgn;_tmp2B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0)->f1).ptr_atts).bounds;_tmp2B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0)->f1).ptr_atts).zero_term;_LL7: {void*t2=_tmp2B1;void*rt=_tmp2B2;void*b=_tmp2B3;void*zt=_tmp2B4;
# 1331
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2B5=Cyc_Absyn_compress(t2);void*_stmttmp29=_tmp2B5;void*_tmp2B6=_stmttmp29;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6)->tag == 5U){_LLB: _LLC:
# 1336
 if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E2;_tmp5E2.tag=0U,({struct _fat_ptr _tmp7C3=({const char*_tmp2B9="unnecessary dereference for function type";_tag_fat(_tmp2B9,sizeof(char),42U);});_tmp5E2.f1=_tmp7C3;});_tmp5E2;});void*_tmp2B7[1U];_tmp2B7[0]=& _tmp2B8;({unsigned _tmp7C4=loc;Cyc_Warn_warn2(_tmp7C4,_tag_fat(_tmp2B7,sizeof(void*),1U));});});
return t;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E3;_tmp5E3.tag=0U,({struct _fat_ptr _tmp7C5=({const char*_tmp2BC="cannot dereference abstract pointer type";_tag_fat(_tmp2BC,sizeof(char),41U);});_tmp5E3.f1=_tmp7C5;});_tmp5E3;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BB;({unsigned _tmp7C6=loc;Cyc_Warn_err2(_tmp7C6,_tag_fat(_tmp2BA,sizeof(void*),1U));});});}_LLA:;}
# 1341
return t2;}}else{_LL8: _LL9:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=0U,({struct _fat_ptr _tmp7C7=({const char*_tmp2C0="expecting pointer type but found ";_tag_fat(_tmp2C0,sizeof(char),34U);});_tmp5E5.f1=_tmp7C7;});_tmp5E5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=2U,_tmp5E4.f1=(void*)t;_tmp5E4;});void*_tmp2BD[2U];_tmp2BD[0]=& _tmp2BE,_tmp2BD[1]=& _tmp2BF;({struct Cyc_Tcenv_Tenv*_tmp7CA=te;unsigned _tmp7C9=loc;void**_tmp7C8=topt;Cyc_Tcexp_expr_err(_tmp7CA,_tmp7C9,_tmp7C8,_tag_fat(_tmp2BD,sizeof(void*),2U));});});}_LL5:;}}}
# 1346
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1350
({int _tmp7CB=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp7CB;});{
void*_tmp2C1=Cyc_Absyn_compress(aggr_type);void*_stmttmp2A=_tmp2C1;void*_tmp2C2=_stmttmp2A;struct Cyc_List_List*_tmp2C4;enum Cyc_Absyn_AggrKind _tmp2C3;struct Cyc_List_List*_tmp2C6;struct Cyc_Absyn_Aggrdecl*_tmp2C5;switch(*((int*)_tmp2C2)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp2C5=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)->f1).KnownAggr).val;_tmp2C6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2C5;struct Cyc_List_List*ts=_tmp2C6;
# 1353
struct Cyc_Absyn_Aggrfield*_tmp2C7=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2C7;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2C9=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=6U,_tmp5E9.f1=ad;_tmp5E9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=0U,({struct _fat_ptr _tmp7CC=({const char*_tmp2CE=" has no ";_tag_fat(_tmp2CE,sizeof(char),9U);});_tmp5E8.f1=_tmp7CC;});_tmp5E8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=0U,_tmp5E7.f1=*f;_tmp5E7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=0U,({struct _fat_ptr _tmp7CD=({const char*_tmp2CD=" member";_tag_fat(_tmp2CD,sizeof(char),8U);});_tmp5E6.f1=_tmp7CD;});_tmp5E6;});void*_tmp2C8[4U];_tmp2C8[0]=& _tmp2C9,_tmp2C8[1]=& _tmp2CA,_tmp2C8[2]=& _tmp2CB,_tmp2C8[3]=& _tmp2CC;({struct Cyc_Tcenv_Tenv*_tmp7D0=te;unsigned _tmp7CF=loc;void**_tmp7CE=topt;Cyc_Tcexp_expr_err(_tmp7D0,_tmp7CF,_tmp7CE,_tag_fat(_tmp2C8,sizeof(void*),4U));});});
# 1357
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp7D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp7D1,finfo->type);});{
struct Cyc_Absyn_Kind*_tmp2CF=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*t2_kind=_tmp2CF;
# 1365
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2D0=Cyc_Absyn_compress(t2);void*_stmttmp2B=_tmp2D0;void*_tmp2D1=_stmttmp2B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D1)->tag == 4U){_LL8: _LL9:
 goto _LL7;}else{_LLA: _LLB:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=0U,({struct _fat_ptr _tmp7D2=({const char*_tmp2D7="cannot get member ";_tag_fat(_tmp2D7,sizeof(char),19U);});_tmp5EC.f1=_tmp7D2;});_tmp5EC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=0U,_tmp5EB.f1=*f;_tmp5EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=0U,({
struct _fat_ptr _tmp7D3=({const char*_tmp2D6=" since its type is abstract";_tag_fat(_tmp2D6,sizeof(char),28U);});_tmp5EA.f1=_tmp7D3;});_tmp5EA;});void*_tmp2D2[3U];_tmp2D2[0]=& _tmp2D3,_tmp2D2[1]=& _tmp2D4,_tmp2D2[2]=& _tmp2D5;({struct Cyc_Tcenv_Tenv*_tmp7D6=te;unsigned _tmp7D5=loc;void**_tmp7D4=topt;Cyc_Tcexp_expr_err(_tmp7D6,_tmp7D5,_tmp7D4,_tag_fat(_tmp2D2,sizeof(void*),3U));});});}_LL7:;}
# 1373
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=0U,({struct _fat_ptr _tmp7D7=({const char*_tmp2DD="cannot read union member ";_tag_fat(_tmp2DD,sizeof(char),26U);});_tmp5EF.f1=_tmp7D7;});_tmp5EF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=0U,_tmp5EE.f1=*f;_tmp5EE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5ED;_tmp5ED.tag=0U,({
struct _fat_ptr _tmp7D8=({const char*_tmp2DC=" since it is not `bits-only'";_tag_fat(_tmp2DC,sizeof(char),29U);});_tmp5ED.f1=_tmp7D8;});_tmp5ED;});void*_tmp2D8[3U];_tmp2D8[0]=& _tmp2D9,_tmp2D8[1]=& _tmp2DA,_tmp2D8[2]=& _tmp2DB;({struct Cyc_Tcenv_Tenv*_tmp7DB=te;unsigned _tmp7DA=loc;void**_tmp7D9=topt;Cyc_Tcexp_expr_err(_tmp7DB,_tmp7DA,_tmp7D9,_tag_fat(_tmp2D8,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1379
if(!({void*_tmp7DC=t2;Cyc_Unify_unify(_tmp7DC,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0U,({struct _fat_ptr _tmp7DD=({const char*_tmp2E3="must use pattern-matching to access field ";_tag_fat(_tmp2E3,sizeof(char),43U);});_tmp5F2.f1=_tmp7DD;});_tmp5F2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=0U,_tmp5F1.f1=*f;_tmp5F1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0U,({struct _fat_ptr _tmp7DE=({const char*_tmp2E2="\n\tdue to existential type variables.";_tag_fat(_tmp2E2,sizeof(char),37U);});_tmp5F0.f1=_tmp7DE;});_tmp5F0;});void*_tmp2DE[3U];_tmp2DE[0]=& _tmp2DF,_tmp2DE[1]=& _tmp2E0,_tmp2DE[2]=& _tmp2E1;({struct Cyc_Tcenv_Tenv*_tmp7E1=te;unsigned _tmp7E0=loc;void**_tmp7DF=topt;Cyc_Tcexp_expr_err(_tmp7E1,_tmp7E0,_tmp7DF,_tag_fat(_tmp2DE,sizeof(void*),3U));});});}
# 1382
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp2C3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C2)->f1;_tmp2C4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C2)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp2C3;struct Cyc_List_List*fs=_tmp2C4;
# 1384
struct Cyc_Absyn_Aggrfield*_tmp2E4=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2E4;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=0U,({struct _fat_ptr _tmp7E2=({const char*_tmp2EA="type has no ";_tag_fat(_tmp2EA,sizeof(char),13U);});_tmp5F5.f1=_tmp7E2;});_tmp5F5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=0U,_tmp5F4.f1=*f;_tmp5F4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=0U,({struct _fat_ptr _tmp7E3=({const char*_tmp2E9=" member";_tag_fat(_tmp2E9,sizeof(char),8U);});_tmp5F3.f1=_tmp7E3;});_tmp5F3;});void*_tmp2E5[3U];_tmp2E5[0]=& _tmp2E6,_tmp2E5[1]=& _tmp2E7,_tmp2E5[2]=& _tmp2E8;({struct Cyc_Tcenv_Tenv*_tmp7E6=te;unsigned _tmp7E5=loc;void**_tmp7E4=topt;Cyc_Tcexp_expr_err(_tmp7E6,_tmp7E5,_tmp7E4,_tag_fat(_tmp2E5,sizeof(void*),3U));});});
# 1389
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1391
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F8;_tmp5F8.tag=0U,({struct _fat_ptr _tmp7E7=({const char*_tmp2F0="cannot read union member ";_tag_fat(_tmp2F0,sizeof(char),26U);});_tmp5F8.f1=_tmp7E7;});_tmp5F8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F7;_tmp5F7.tag=0U,_tmp5F7.f1=*f;_tmp5F7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F6;_tmp5F6.tag=0U,({struct _fat_ptr _tmp7E8=({const char*_tmp2EF=" since it is not `bits-only'";_tag_fat(_tmp2EF,sizeof(char),29U);});_tmp5F6.f1=_tmp7E8;});_tmp5F6;});void*_tmp2EB[3U];_tmp2EB[0]=& _tmp2EC,_tmp2EB[1]=& _tmp2ED,_tmp2EB[2]=& _tmp2EE;({struct Cyc_Tcenv_Tenv*_tmp7EB=te;unsigned _tmp7EA=loc;void**_tmp7E9=topt;Cyc_Tcexp_expr_err(_tmp7EB,_tmp7EA,_tmp7E9,_tag_fat(_tmp2EB,sizeof(void*),3U));});});
return finfo->type;}default: _LL5: _LL6:
# 1394
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=0U,({struct _fat_ptr _tmp7EC=({const char*_tmp2F4="expecting struct or union, found ";_tag_fat(_tmp2F4,sizeof(char),34U);});_tmp5FA.f1=_tmp7EC;});_tmp5FA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2F3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5F9;_tmp5F9.tag=2U,_tmp5F9.f1=(void*)aggr_type;_tmp5F9;});void*_tmp2F1[2U];_tmp2F1[0]=& _tmp2F2,_tmp2F1[1]=& _tmp2F3;({struct Cyc_Tcenv_Tenv*_tmp7EF=te;unsigned _tmp7EE=loc;void**_tmp7ED=topt;Cyc_Tcexp_expr_err(_tmp7EF,_tmp7EE,_tmp7ED,_tag_fat(_tmp2F1,sizeof(void*),2U));});});}_LL0:;}}
# 1399
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1403
({struct Cyc_Tcenv_Tenv*_tmp7F0=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp7F0,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,(void*)_check_null(e->topt),f,is_tagged,is_read);}
# 1408
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1412
void*t2=({struct Cyc_Tcenv_Tenv*_tmp7F2=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _tmp7F1=loc;Cyc_Tcexp_tcDeref(_tmp7F2,_tmp7F1,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1417
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1419
struct Cyc_Tcenv_Tenv*_tmp2F5=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp2F5;
({struct Cyc_Tcenv_Tenv*_tmp7F3=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F3,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp7F4=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F4,0,e2);});{
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FC;_tmp5FC.tag=0U,({struct _fat_ptr _tmp7F5=({const char*_tmp2F9="expecting int subscript, found ";_tag_fat(_tmp2F9,sizeof(char),32U);});_tmp5FC.f1=_tmp7F5;});_tmp5FC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2F8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5FB;_tmp5FB.tag=2U,_tmp5FB.f1=(void*)t2;_tmp5FB;});void*_tmp2F6[2U];_tmp2F6[0]=& _tmp2F7,_tmp2F6[1]=& _tmp2F8;({struct Cyc_Tcenv_Tenv*_tmp7F8=te;unsigned _tmp7F7=e2->loc;void**_tmp7F6=topt;Cyc_Tcexp_expr_err(_tmp7F8,_tmp7F7,_tmp7F6,_tag_fat(_tmp2F6,sizeof(void*),2U));});});{
# 1428
void*_tmp2FA=t1;struct Cyc_List_List*_tmp2FB;void*_tmp300;void*_tmp2FF;void*_tmp2FE;struct Cyc_Absyn_Tqual _tmp2FD;void*_tmp2FC;switch(*((int*)_tmp2FA)){case 3U: _LL1: _tmp2FC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).elt_type;_tmp2FD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).elt_tq;_tmp2FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).rgn;_tmp2FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).bounds;_tmp300=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp2FC;struct Cyc_Absyn_Tqual tq=_tmp2FD;void*rt=_tmp2FE;void*b=_tmp2FF;void*zt=_tmp300;
# 1432
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp301=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp301;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp302=eopt;struct Cyc_Absyn_Exp*e3=_tmp302;
struct _tuple16 _tmp303=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2C=_tmp303;struct _tuple16 _tmp304=_stmttmp2C;int _tmp306;unsigned _tmp305;_LL8: _tmp305=_tmp304.f1;_tmp306=_tmp304.f2;_LL9: {unsigned i=_tmp305;int known_i=_tmp306;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp307=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2D=_tmp307;struct _tuple16 _tmp308=_stmttmp2D;int _tmp30A;unsigned _tmp309;_LLB: _tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;_LLC: {unsigned j=_tmp309;int known_j=_tmp30A;
if(known_j){
struct _tuple16 _tmp30B=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2E=_tmp30B;struct _tuple16 _tmp30C=_stmttmp2E;int _tmp30E;unsigned _tmp30D;_LLE: _tmp30D=_tmp30C.f1;_tmp30E=_tmp30C.f2;_LLF: {unsigned j=_tmp30D;int knownj=_tmp30E;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp30F=0U;({unsigned _tmp7FA=loc;struct _fat_ptr _tmp7F9=({const char*_tmp310="subscript will fail at run-time";_tag_fat(_tmp310,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7FA,_tmp7F9,_tag_fat(_tmp30F,sizeof(void*),0U));});});}}}}}}
# 1448
if(emit_warning)
({void*_tmp311=0U;({unsigned _tmp7FC=e2->loc;struct _fat_ptr _tmp7FB=({const char*_tmp312="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp312,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp7FC,_tmp7FB,_tag_fat(_tmp311,sizeof(void*),0U));});});}else{
# 1452
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp313=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2F=_tmp313;struct _tuple16 _tmp314=_stmttmp2F;int _tmp316;unsigned _tmp315;_LL11: _tmp315=_tmp314.f1;_tmp316=_tmp314.f2;_LL12: {unsigned i=_tmp315;int known=_tmp316;
if(known)
# 1457
({unsigned _tmp7FF=loc;unsigned _tmp7FE=i;void*_tmp7FD=b;Cyc_Tcutil_check_bound(_tmp7FF,_tmp7FE,_tmp7FD,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1460
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp317=0U;({unsigned _tmp801=e1->loc;struct _fat_ptr _tmp800=({const char*_tmp318="subscript applied to pointer to one object";_tag_fat(_tmp318,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp801,_tmp800,_tag_fat(_tmp317,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1465
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp319=0U;({unsigned _tmp803=e1->loc;struct _fat_ptr _tmp802=({const char*_tmp31A="can't subscript an abstract pointer";_tag_fat(_tmp31A,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp803,_tmp802,_tag_fat(_tmp319,sizeof(void*),0U));});});
return t;}case 6U: _LL3: _tmp2FB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FA)->f1;_LL4: {struct Cyc_List_List*ts=_tmp2FB;
# 1470
struct _tuple16 _tmp31B=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp30=_tmp31B;struct _tuple16 _tmp31C=_stmttmp30;int _tmp31E;unsigned _tmp31D;_LL14: _tmp31D=_tmp31C.f1;_tmp31E=_tmp31C.f2;_LL15: {unsigned i=_tmp31D;int known=_tmp31E;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp320=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FD;_tmp5FD.tag=0U,({
struct _fat_ptr _tmp804=({const char*_tmp321="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp321,sizeof(char),47U);});_tmp5FD.f1=_tmp804;});_tmp5FD;});void*_tmp31F[1U];_tmp31F[0]=& _tmp320;({struct Cyc_Tcenv_Tenv*_tmp806=te;unsigned _tmp805=loc;Cyc_Tcexp_expr_err(_tmp806,_tmp805,0,_tag_fat(_tmp31F,sizeof(void*),1U));});});{
struct _handler_cons _tmp322;_push_handler(& _tmp322);{int _tmp324=0;if(setjmp(_tmp322.handler))_tmp324=1;if(!_tmp324){{void*_tmp325=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp325;};_pop_handler();}else{void*_tmp323=(void*)Cyc_Core_get_exn_thrown();void*_tmp326=_tmp323;void*_tmp327;if(((struct Cyc_List_Nth_exn_struct*)_tmp326)->tag == Cyc_List_Nth){_LL17: _LL18:
# 1476
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp329=({struct Cyc_Warn_String_Warn_Warg_struct _tmp602;_tmp602.tag=0U,({struct _fat_ptr _tmp807=({const char*_tmp330="index is ";_tag_fat(_tmp330,sizeof(char),10U);});_tmp602.f1=_tmp807;});_tmp602;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp32A=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp601;_tmp601.tag=12U,_tmp601.f1=(int)i;_tmp601;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp600;_tmp600.tag=0U,({struct _fat_ptr _tmp808=({const char*_tmp32F=" but tuple has only ";_tag_fat(_tmp32F,sizeof(char),21U);});_tmp600.f1=_tmp808;});_tmp600;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp32C=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5FF;_tmp5FF.tag=12U,({
int _tmp809=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp5FF.f1=_tmp809;});_tmp5FF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FE;_tmp5FE.tag=0U,({struct _fat_ptr _tmp80A=({const char*_tmp32E=" fields";_tag_fat(_tmp32E,sizeof(char),8U);});_tmp5FE.f1=_tmp80A;});_tmp5FE;});void*_tmp328[5U];_tmp328[0]=& _tmp329,_tmp328[1]=& _tmp32A,_tmp328[2]=& _tmp32B,_tmp328[3]=& _tmp32C,_tmp328[4]=& _tmp32D;({struct Cyc_Tcenv_Tenv*_tmp80C=te;unsigned _tmp80B=loc;Cyc_Tcexp_expr_err(_tmp80C,_tmp80B,0,_tag_fat(_tmp328,sizeof(void*),5U));});});}else{_LL19: _tmp327=_tmp326;_LL1A: {void*exn=_tmp327;(int)_rethrow(exn);}}_LL16:;}}}}}default: _LL5: _LL6:
# 1479
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp332=({struct Cyc_Warn_String_Warn_Warg_struct _tmp604;_tmp604.tag=0U,({struct _fat_ptr _tmp80D=({const char*_tmp334="subscript applied to ";_tag_fat(_tmp334,sizeof(char),22U);});_tmp604.f1=_tmp80D;});_tmp604;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp333=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp603;_tmp603.tag=2U,_tmp603.f1=(void*)t1;_tmp603;});void*_tmp331[2U];_tmp331[0]=& _tmp332,_tmp331[1]=& _tmp333;({struct Cyc_Tcenv_Tenv*_tmp810=te;unsigned _tmp80F=loc;void**_tmp80E=topt;Cyc_Tcexp_expr_err(_tmp810,_tmp80F,_tmp80E,_tag_fat(_tmp331,sizeof(void*),2U));});});}_LL0:;}}}
# 1484
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp335=Cyc_Absyn_compress(*topt);void*_stmttmp31=_tmp335;void*_tmp336=_stmttmp31;struct Cyc_List_List*_tmp337;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp336)->tag == 6U){_LL1: _tmp337=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp336)->f1;_LL2: {struct Cyc_List_List*ts=_tmp337;
# 1491
if(({int _tmp811=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp811 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
goto _LL0;
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp338=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp338;
({struct Cyc_Tcenv_Tenv*_tmp812=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp812,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1498
({struct Cyc_RgnOrder_RgnPO*_tmp814=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp813=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp814,_tmp813,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp33A=_cycalloc(sizeof(*_tmp33A));({struct _tuple17*_tmp815=({struct _tuple17*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp339->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp339;});_tmp33A->hd=_tmp815;}),_tmp33A->tl=fields;_tmp33A;});}
# 1501
done=1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1505
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp816=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp816,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp33C=_cycalloc(sizeof(*_tmp33C));({struct _tuple17*_tmp818=({struct _tuple17*_tmp33B=_cycalloc(sizeof(*_tmp33B));({struct Cyc_Absyn_Tqual _tmp817=Cyc_Absyn_empty_tqual(0U);_tmp33B->f1=_tmp817;}),_tmp33B->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp33B;});_tmp33C->hd=_tmp818;}),_tmp33C->tl=fields;_tmp33C;});}
# 1510
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->tag=6U,({struct Cyc_List_List*_tmp819=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp33D->f1=_tmp819;});_tmp33D;});}
# 1514
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1519
struct _tuple9*_tmp33E=targ;void*_tmp33F;_LL1: _tmp33F=_tmp33E->f3;_LL2: {void*t=_tmp33F;
({unsigned _tmp81D=loc;struct Cyc_Tcenv_Tenv*_tmp81C=te;struct Cyc_List_List*_tmp81B=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp81A=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1520
Cyc_Tctyp_check_type(_tmp81D,_tmp81C,_tmp81B,_tmp81A,1,0,t);});
# 1522
({void*_tmp81E=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340->tag=36U,_tmp340->f1=0,_tmp340->f2=des;_tmp340;});orig_exp->r=_tmp81E;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1535 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1538
void*res_t2;
int _tmp341=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp341;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp342=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp360=_cycalloc(sizeof(*_tmp360));_tmp360->tag=0U,({union Cyc_Absyn_Cnst _tmp81F=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp360->f1=_tmp81F;});_tmp360;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp342;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1546
if(zero_term){
struct Cyc_Absyn_Exp*_tmp343=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp343;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp344=0U;({unsigned _tmp821=e->loc;struct _fat_ptr _tmp820=({const char*_tmp345="zero-terminated array doesn't end with zero.";_tag_fat(_tmp345,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp821,_tmp820,_tag_fat(_tmp344,sizeof(void*),0U));});});}
# 1551
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp824=res;struct Cyc_Absyn_Tqual _tmp823=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1552
struct Cyc_Absyn_Exp*_tmp822=sz_exp;Cyc_Absyn_array_type(_tmp824,_tmp823,_tmp822,
# 1554
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1556
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1559
if(!({struct Cyc_RgnOrder_RgnPO*_tmp826=Cyc_Tcenv_curr_rgnpo(te);void*_tmp825=res;Cyc_Tcutil_coerce_list(_tmp826,_tmp825,es);}))
# 1561
({struct Cyc_Warn_String_Warn_Warg_struct _tmp347=({struct Cyc_Warn_String_Warn_Warg_struct _tmp607;_tmp607.tag=0U,({struct _fat_ptr _tmp827=({const char*_tmp34B="elements of array do not all have the same type (";_tag_fat(_tmp34B,sizeof(char),50U);});_tmp607.f1=_tmp827;});_tmp607;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp348=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp606;_tmp606.tag=2U,_tmp606.f1=(void*)res;_tmp606;});struct Cyc_Warn_String_Warn_Warg_struct _tmp349=({struct Cyc_Warn_String_Warn_Warg_struct _tmp605;_tmp605.tag=0U,({
struct _fat_ptr _tmp828=({const char*_tmp34A=")";_tag_fat(_tmp34A,sizeof(char),2U);});_tmp605.f1=_tmp828;});_tmp605;});void*_tmp346[3U];_tmp346[0]=& _tmp347,_tmp346[1]=& _tmp348,_tmp346[2]=& _tmp349;({unsigned _tmp829=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;Cyc_Warn_err2(_tmp829,_tag_fat(_tmp346,sizeof(void*),3U));});});
# 1564
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
void*_tmp34C=(void*)ds->hd;void*_stmttmp32=_tmp34C;void*_tmp34D=_stmttmp32;struct Cyc_Absyn_Exp*_tmp34E;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp34D)->tag == 1U){_LL1: _LL2:
# 1569
({struct Cyc_Warn_String_Warn_Warg_struct _tmp350=({struct Cyc_Warn_String_Warn_Warg_struct _tmp608;_tmp608.tag=0U,({struct _fat_ptr _tmp82A=({const char*_tmp351="only array index designators are supported";_tag_fat(_tmp351,sizeof(char),43U);});_tmp608.f1=_tmp82A;});_tmp608;});void*_tmp34F[1U];_tmp34F[0]=& _tmp350;({unsigned _tmp82B=loc;Cyc_Warn_err2(_tmp82B,_tag_fat(_tmp34F,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _tmp34E=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp34D)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp34E;
# 1571
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp352=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp33=_tmp352;struct _tuple16 _tmp353=_stmttmp33;int _tmp355;unsigned _tmp354;_LL6: _tmp354=_tmp353.f1;_tmp355=_tmp353.f2;_LL7: {unsigned i=_tmp354;int known=_tmp355;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp357=({struct Cyc_Warn_String_Warn_Warg_struct _tmp609;_tmp609.tag=0U,({struct _fat_ptr _tmp82C=({const char*_tmp358="index designator cannot use sizeof or offsetof";_tag_fat(_tmp358,sizeof(char),47U);});_tmp609.f1=_tmp82C;});_tmp609;});void*_tmp356[1U];_tmp356[0]=& _tmp357;({unsigned _tmp82D=e->loc;Cyc_Warn_err2(_tmp82D,_tag_fat(_tmp356,sizeof(void*),1U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp35A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D;_tmp60D.tag=0U,({struct _fat_ptr _tmp82E=({const char*_tmp35F="expecting index designator ";_tag_fat(_tmp35F,sizeof(char),28U);});_tmp60D.f1=_tmp82E;});_tmp60D;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp35B=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp60C;_tmp60C.tag=12U,_tmp60C.f1=offset;_tmp60C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp35C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60B;_tmp60B.tag=0U,({struct _fat_ptr _tmp82F=({const char*_tmp35E=" but found ";_tag_fat(_tmp35E,sizeof(char),12U);});_tmp60B.f1=_tmp82F;});_tmp60B;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp35D=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp60A;_tmp60A.tag=12U,_tmp60A.f1=(int)i;_tmp60A;});void*_tmp359[4U];_tmp359[0]=& _tmp35A,_tmp359[1]=& _tmp35B,_tmp359[2]=& _tmp35C,_tmp359[3]=& _tmp35D;({unsigned _tmp830=e->loc;Cyc_Warn_err2(_tmp830,_tag_fat(_tmp359,sizeof(void*),4U));});});}
goto _LL0;}}}}_LL0:;}}}
# 1581
return res_t2;}struct _tuple19{void**f1;struct Cyc_Absyn_Tqual*f2;void**f3;};
# 1586
static struct _tuple19 Cyc_Tcexp_ptrEltAtts(void**topt){
if(!((unsigned)topt))
return({struct _tuple19 _tmp60E;_tmp60E.f1=0,_tmp60E.f2=0,_tmp60E.f3=0;_tmp60E;});{
void*_tmp361=Cyc_Absyn_compress(*topt);void*_stmttmp34=_tmp361;void*_tmp362=_stmttmp34;struct Cyc_Absyn_ArrayInfo _tmp363;struct Cyc_Absyn_PtrInfo _tmp364;switch(*((int*)_tmp362)){case 3U: _LL1: _tmp364=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362)->f1;_LL2: {struct Cyc_Absyn_PtrInfo x=_tmp364;
# 1591
return({struct _tuple19 _tmp60F;({void**_tmp833=({void**_tmp365=_cycalloc(sizeof(*_tmp365));*_tmp365=x.elt_type;_tmp365;});_tmp60F.f1=_tmp833;}),({struct Cyc_Absyn_Tqual*_tmp832=({struct Cyc_Absyn_Tqual*_tmp366=_cycalloc(sizeof(*_tmp366));*_tmp366=x.elt_tq;_tmp366;});_tmp60F.f2=_tmp832;}),({void**_tmp831=({void**_tmp367=_cycalloc(sizeof(*_tmp367));*_tmp367=(x.ptr_atts).zero_term;_tmp367;});_tmp60F.f3=_tmp831;});_tmp60F;});}case 4U: _LL3: _tmp363=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp362)->f1;_LL4: {struct Cyc_Absyn_ArrayInfo x=_tmp363;
return({struct _tuple19 _tmp610;({void**_tmp836=({void**_tmp368=_cycalloc(sizeof(*_tmp368));*_tmp368=x.elt_type;_tmp368;});_tmp610.f1=_tmp836;}),({struct Cyc_Absyn_Tqual*_tmp835=({struct Cyc_Absyn_Tqual*_tmp369=_cycalloc(sizeof(*_tmp369));*_tmp369=x.tq;_tmp369;});_tmp610.f2=_tmp835;}),({void**_tmp834=({void**_tmp36A=_cycalloc(sizeof(*_tmp36A));*_tmp36A=x.zero_term;_tmp36A;});_tmp610.f3=_tmp834;});_tmp610;});}default: _LL5: _LL6:
 return({struct _tuple19 _tmp611;_tmp611.f1=0,_tmp611.f2=0,_tmp611.f3=0;_tmp611;});}_LL0:;}}
# 1598
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1601
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp36B=Cyc_Absyn_compress((void*)_check_null(bound->topt));void*_stmttmp35=_tmp36B;void*_tmp36C=_stmttmp35;void*_tmp36D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36C)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36C)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36C)->f2 != 0){_LL1: _tmp36D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36C)->f2)->hd;_LL2: {void*t=_tmp36D;
# 1606
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp36E=({void*_tmp837=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp837,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp36E;
b->topt=bound->topt;
bound=b;}
# 1611
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1613
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp370=({struct Cyc_Warn_String_Warn_Warg_struct _tmp613;_tmp613.tag=0U,({struct _fat_ptr _tmp838=({const char*_tmp372="expecting unsigned int, found ";_tag_fat(_tmp372,sizeof(char),31U);});_tmp613.f1=_tmp838;});_tmp613;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp371=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp612;_tmp612.tag=3U,_tmp612.f1=(void*)bound->topt;_tmp612;});void*_tmp36F[2U];_tmp36F[0]=& _tmp370,_tmp36F[1]=& _tmp371;({unsigned _tmp839=bound->loc;Cyc_Warn_err2(_tmp839,_tag_fat(_tmp36F,sizeof(void*),2U));});});}_LL0:;}
# 1617
if(!(vd->tq).real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp374=({struct Cyc_Warn_String_Warn_Warg_struct _tmp614;_tmp614.tag=0U,({struct _fat_ptr _tmp83A=({const char*_tmp375="comprehension index variable is not declared const!";_tag_fat(_tmp375,sizeof(char),52U);});_tmp614.f1=_tmp83A;});_tmp614;});void*_tmp373[1U];_tmp373[0]=& _tmp374;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp373,sizeof(void*),1U));});
# 1620
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
# 1623
struct _tuple19 _tmp376=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp36=_tmp376;struct _tuple19 _tmp377=_stmttmp36;void**_tmp37A;struct Cyc_Absyn_Tqual*_tmp379;void**_tmp378;_LL6: _tmp378=_tmp377.f1;_tmp379=_tmp377.f2;_tmp37A=_tmp377.f3;_LL7: {void**topt2=_tmp378;struct Cyc_Absyn_Tqual*tqopt=_tmp379;void**ztopt=_tmp37A;
# 1626
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1628
Cyc_Tcexp_check_consume(body->loc,t,body);
if(te->le == 0){
# 1631
if(!Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp615;_tmp615.tag=0U,({struct _fat_ptr _tmp83B=({const char*_tmp37D="bound is not constant";_tag_fat(_tmp37D,sizeof(char),22U);});_tmp615.f1=_tmp83B;});_tmp615;});void*_tmp37B[1U];_tmp37B[0]=& _tmp37C;({unsigned _tmp83C=bound->loc;Cyc_Warn_err2(_tmp83C,_tag_fat(_tmp37B,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp616;_tmp616.tag=0U,({struct _fat_ptr _tmp83D=({const char*_tmp380="body is not constant";_tag_fat(_tmp380,sizeof(char),21U);});_tmp616.f1=_tmp83D;});_tmp616;});void*_tmp37E[1U];_tmp37E[0]=& _tmp37F;({unsigned _tmp83E=body->loc;Cyc_Warn_err2(_tmp83E,_tag_fat(_tmp37E,sizeof(void*),1U));});});}
# 1636
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1638
struct Cyc_Absyn_Exp*_tmp381=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp381;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1642
Cyc_Tcexp_check_consume(body->loc,(void*)_check_null(body->topt),body);
# 1644
return({void*_tmp841=t;struct Cyc_Absyn_Tqual _tmp840=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp83F=bound;Cyc_Absyn_array_type(_tmp841,_tmp840,_tmp83F,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1649
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1652
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp382=Cyc_Absyn_compress((void*)_check_null(bound->topt));void*_stmttmp37=_tmp382;void*_tmp383=_stmttmp37;void*_tmp384;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp383)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp383)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp383)->f2 != 0){_LL1: _tmp384=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp383)->f2)->hd;_LL2: {void*t=_tmp384;
# 1657
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp385=({void*_tmp842=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp842,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp385;
b->topt=bound->topt;
bound=b;}
# 1662
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1664
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp387=({struct Cyc_Warn_String_Warn_Warg_struct _tmp618;_tmp618.tag=0U,({struct _fat_ptr _tmp843=({const char*_tmp389="expecting unsigned int, found ";_tag_fat(_tmp389,sizeof(char),31U);});_tmp618.f1=_tmp843;});_tmp618;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp388=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp617;_tmp617.tag=3U,_tmp617.f1=(void*)bound->topt;_tmp617;});void*_tmp386[2U];_tmp386[0]=& _tmp387,_tmp386[1]=& _tmp388;({unsigned _tmp844=bound->loc;Cyc_Warn_err2(_tmp844,_tag_fat(_tmp386,sizeof(void*),2U));});});}_LL0:;}{
# 1667
struct _tuple19 _tmp38A=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp38=_tmp38A;struct _tuple19 _tmp38B=_stmttmp38;void**_tmp38E;struct Cyc_Absyn_Tqual*_tmp38D;void**_tmp38C;_LL6: _tmp38C=_tmp38B.f1;_tmp38D=_tmp38B.f2;_tmp38E=_tmp38B.f3;_LL7: {void**topt2=_tmp38C;struct Cyc_Absyn_Tqual*tqopt=_tmp38D;void**ztopt=_tmp38E;
# 1670
({unsigned _tmp847=loc;struct Cyc_Tcenv_Tenv*_tmp846=te;struct Cyc_List_List*_tmp845=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp847,_tmp846,_tmp845,& Cyc_Tcutil_tmk,1,1,t);});
if(topt2 != 0)
Cyc_Unify_unify(*topt2,t);
# 1674
if(te->le == 0 && !Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp390=({struct Cyc_Warn_String_Warn_Warg_struct _tmp619;_tmp619.tag=0U,({struct _fat_ptr _tmp848=({const char*_tmp391="bound is not constant";_tag_fat(_tmp391,sizeof(char),22U);});_tmp619.f1=_tmp848;});_tmp619;});void*_tmp38F[1U];_tmp38F[0]=& _tmp390;({unsigned _tmp849=bound->loc;Cyc_Warn_err2(_tmp849,_tag_fat(_tmp38F,sizeof(void*),1U));});});
# 1678
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp393=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp84A=({const char*_tmp394="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp394,sizeof(char),74U);});_tmp61A.f1=_tmp84A;});_tmp61A;});void*_tmp392[1U];_tmp392[0]=& _tmp393;({unsigned _tmp84B=loc;Cyc_Warn_err2(_tmp84B,_tag_fat(_tmp392,sizeof(void*),1U));});});
# 1682
return({void*_tmp84E=t;struct Cyc_Absyn_Tqual _tmp84D=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp84C=bound;Cyc_Absyn_array_type(_tmp84E,_tmp84D,_tmp84C,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1686
static int Cyc_Tcexp_ensure_typqual_nothrow(void*t){
void*_tmp395=t;struct Cyc_Absyn_PtrInfo*_tmp396;struct Cyc_Absyn_FnInfo*_tmp397;switch(*((int*)_tmp395)){case 5U: _LL1: _tmp397=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp395)->f1;_LL2: {struct Cyc_Absyn_FnInfo*i=_tmp397;
# 1689
return((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Atts_attribute_cmp,i->attributes,(void*)& Cyc_Absyn_No_throw_att_val);}case 3U: _LL3: _tmp396=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp395)->f1;_LL4: {struct Cyc_Absyn_PtrInfo*pi=_tmp396;
# 1693
return Cyc_Tcexp_ensure_typqual_nothrow(pi->elt_type);}default: _LL5: _LL6:
# 1695
 return 1;}_LL0:;}
# 1699
static int Cyc_Tcexp_ensure_nothrow_initializer(struct Cyc_Absyn_Exp*e){
void*_tmp398=e->r;void*_stmttmp39=_tmp398;void*_tmp399=_stmttmp39;struct Cyc_Absyn_Exp*_tmp39A;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp399)->tag == 14U){_LL1: _tmp39A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp399)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp39A;
# 1702
return Cyc_Tcexp_ensure_nothrow_initializer(e1);}}else{_LL3: _LL4:
# 1704
 return Cyc_Tcexp_ensure_typqual_nothrow((void*)_check_null(e->topt));}_LL0:;}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1719 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1723
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp39B=_cycalloc(sizeof(*_tmp39B));*_tmp39B=ad;_tmp39B;});}else{
# 1729
{struct _handler_cons _tmp39C;_push_handler(& _tmp39C);{int _tmp39E=0;if(setjmp(_tmp39C.handler))_tmp39E=1;if(!_tmp39E){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1729
;_pop_handler();}else{void*_tmp39D=(void*)Cyc_Core_get_exn_thrown();void*_tmp39F=_tmp39D;void*_tmp3A0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp39F)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1732
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61C;_tmp61C.tag=0U,({struct _fat_ptr _tmp84F=({const char*_tmp3A4="unbound struct/union name ";_tag_fat(_tmp3A4,sizeof(char),27U);});_tmp61C.f1=_tmp84F;});_tmp61C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3A3=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp61B;_tmp61B.tag=1U,_tmp61B.f1=*tn;_tmp61B;});void*_tmp3A1[2U];_tmp3A1[0]=& _tmp3A2,_tmp3A1[1]=& _tmp3A3;({unsigned _tmp850=loc;Cyc_Warn_err2(_tmp850,_tag_fat(_tmp3A1,sizeof(void*),2U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp3A0=_tmp39F;_LL4: {void*exn=_tmp3A0;(int)_rethrow(exn);}}_LL0:;}}}
# 1735
*ad_opt=ad;
*tn=ad->name;}
# 1738
if(ad->impl == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61E;_tmp61E.tag=0U,({struct _fat_ptr _tmp851=({const char*_tmp3A8="can't construct abstract ";_tag_fat(_tmp3A8,sizeof(char),26U);});_tmp61E.f1=_tmp851;});_tmp61E;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3A7=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp61D;_tmp61D.tag=6U,_tmp61D.f1=ad;_tmp61D;});void*_tmp3A5[2U];_tmp3A5[0]=& _tmp3A6,_tmp3A5[1]=& _tmp3A7;({struct Cyc_Tcenv_Tenv*_tmp853=te;unsigned _tmp852=loc;Cyc_Tcexp_expr_err(_tmp853,_tmp852,0,_tag_fat(_tmp3A5,sizeof(void*),2U));});});{
# 1745
struct Cyc_Tcenv_Tenv*_tmp3A9=(int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNew?Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te): te;struct Cyc_Tcenv_Tenv*te2=_tmp3A9;
# 1751
struct _tuple13 _tmp3AA=({struct _tuple13 _tmp62B;({struct Cyc_List_List*_tmp854=Cyc_Tcenv_lookup_type_vars(te2);_tmp62B.f1=_tmp854;}),_tmp62B.f2=Cyc_Core_heap_region;_tmp62B;});struct _tuple13 env=_tmp3AA;
struct Cyc_List_List*_tmp3AB=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp3AB;
struct Cyc_List_List*_tmp3AC=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp3AC;
struct Cyc_List_List*_tmp3AD=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp3AD;
struct Cyc_List_List*_tmp3AE=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp3AE;
struct Cyc_List_List*_tmp3AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp3AF;
void*res_typ;
# 1762
if(topt != 0){
void*_tmp3B0=Cyc_Absyn_compress(*topt);void*_stmttmp3A=_tmp3B0;void*_tmp3B1=_stmttmp3A;struct Cyc_List_List*_tmp3B3;struct Cyc_Absyn_Aggrdecl**_tmp3B2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp3B2=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f1)->f1).KnownAggr).val;_tmp3B3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp3B2;struct Cyc_List_List*all_typs2=_tmp3B3;
# 1765
if(*adptr2 != *adptr)
goto _LL9;
{struct Cyc_List_List*_tmp3B4=all_typs;struct Cyc_List_List*ats=_tmp3B4;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1771
res_typ=*topt;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 res_typ=({union Cyc_Absyn_AggrInfo _tmp855=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp855,all_typs);});}_LL5:;}else{
# 1776
res_typ=({union Cyc_Absyn_AggrInfo _tmp856=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp856,all_typs);});}{
# 1779
struct Cyc_List_List*_tmp3B5=*ts;struct Cyc_List_List*user_ex_ts=_tmp3B5;
struct Cyc_List_List*_tmp3B6=exist_typs;struct Cyc_List_List*ex_ts=_tmp3B6;
while(user_ex_ts != 0 && ex_ts != 0){
# 1783
({unsigned _tmp859=loc;struct Cyc_Tcenv_Tenv*_tmp858=te2;struct Cyc_List_List*_tmp857=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp859,_tmp858,_tmp857,& Cyc_Tcutil_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1789
if(user_ex_ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61F;_tmp61F.tag=0U,({struct _fat_ptr _tmp85A=({const char*_tmp3B9="too many explicit witness types";_tag_fat(_tmp3B9,sizeof(char),32U);});_tmp61F.f1=_tmp85A;});_tmp61F;});void*_tmp3B7[1U];_tmp3B7[0]=& _tmp3B8;({unsigned _tmp85B=loc;Cyc_Warn_err2(_tmp85B,_tag_fat(_tmp3B7,sizeof(void*),1U));});});
# 1792
*ts=exist_typs;{
# 1795
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3BA=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3B=_tmp3BA;struct _tuple20*_tmp3BB=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Aggrfield*_tmp3BC;_LLB: _tmp3BC=_tmp3BB->f1;_tmp3BD=_tmp3BB->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp3BC;struct Cyc_Absyn_Exp*field_exp=_tmp3BD;
void*_tmp3BE=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp3BE;
void*_tmp3BF=({struct Cyc_Tcenv_Tenv*_tmp85C=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp85C,& inst_fieldtyp,field_exp);});void*ftyp=_tmp3BF;
# 1803
if(!({struct Cyc_RgnOrder_RgnPO*_tmp85E=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp85D=field_exp;Cyc_Tcutil_coerce_arg(_tmp85E,_tmp85D,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp627;_tmp627.tag=0U,({struct _fat_ptr _tmp85F=({const char*_tmp3CC="field ";_tag_fat(_tmp3CC,sizeof(char),7U);});_tmp627.f1=_tmp85F;});_tmp627;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp626;_tmp626.tag=0U,_tmp626.f1=*field->name;_tmp626;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp860=({const char*_tmp3CB=" of ";_tag_fat(_tmp3CB,sizeof(char),5U);});_tmp625.f1=_tmp860;});_tmp625;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3C4=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp624;_tmp624.tag=6U,_tmp624.f1=ad;_tmp624;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp623;_tmp623.tag=0U,({
struct _fat_ptr _tmp861=({const char*_tmp3CA=" expects type ";_tag_fat(_tmp3CA,sizeof(char),15U);});_tmp623.f1=_tmp861;});_tmp623;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3C6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp622;_tmp622.tag=2U,_tmp622.f1=(void*)inst_fieldtyp;_tmp622;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp621;_tmp621.tag=0U,({struct _fat_ptr _tmp862=({const char*_tmp3C9=" != ";_tag_fat(_tmp3C9,sizeof(char),5U);});_tmp621.f1=_tmp862;});_tmp621;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3C8=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp620;_tmp620.tag=3U,_tmp620.f1=(void*)field_exp->topt;_tmp620;});void*_tmp3C0[8U];_tmp3C0[0]=& _tmp3C1,_tmp3C0[1]=& _tmp3C2,_tmp3C0[2]=& _tmp3C3,_tmp3C0[3]=& _tmp3C4,_tmp3C0[4]=& _tmp3C5,_tmp3C0[5]=& _tmp3C6,_tmp3C0[6]=& _tmp3C7,_tmp3C0[7]=& _tmp3C8;({unsigned _tmp863=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp863,_tag_fat(_tmp3C0,sizeof(void*),8U));});});{
# 1807
int nothrow_fld=((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Atts_attribute_cmp,field->attributes,(void*)& Cyc_Absyn_No_throw_att_val);
# 1810
if(nothrow_fld && !Cyc_Tcexp_ensure_nothrow_initializer(field_exp))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62A;_tmp62A.tag=0U,({struct _fat_ptr _tmp864=({const char*_tmp3D2="field ";_tag_fat(_tmp3D2,sizeof(char),7U);});_tmp62A.f1=_tmp864;});_tmp62A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp629;_tmp629.tag=0U,_tmp629.f1=*field->name;_tmp629;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp628;_tmp628.tag=0U,({struct _fat_ptr _tmp865=({const char*_tmp3D1=" can only hold a no_throw function";_tag_fat(_tmp3D1,sizeof(char),35U);});_tmp628.f1=_tmp865;});_tmp628;});void*_tmp3CD[3U];_tmp3CD[0]=& _tmp3CE,_tmp3CD[1]=& _tmp3CF,_tmp3CD[2]=& _tmp3D0;({unsigned _tmp866=field_exp->loc;Cyc_Warn_err2(_tmp866,_tag_fat(_tmp3CD,sizeof(void*),3U));});});}}}{
# 1814
struct Cyc_List_List*_tmp3D3=0;struct Cyc_List_List*rpo_inst=_tmp3D3;
{struct Cyc_List_List*_tmp3D4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3D4;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _tuple0*_tmp869=({struct _tuple0*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({void*_tmp868=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3D5->f1=_tmp868;}),({
void*_tmp867=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3D5->f2=_tmp867;});_tmp3D5;});
# 1816
_tmp3D6->hd=_tmp869;}),_tmp3D6->tl=rpo_inst;_tmp3D6;});}}
# 1819
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}
# 1825
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1827
{void*_tmp3D7=Cyc_Absyn_compress(ts);void*_stmttmp3C=_tmp3D7;void*_tmp3D8=_stmttmp3C;struct Cyc_List_List*_tmp3DA;enum Cyc_Absyn_AggrKind _tmp3D9;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D8)->tag == 7U){_LL1: _tmp3D9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D8)->f1;_tmp3DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D8)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp3D9;struct Cyc_List_List*fs=_tmp3DA;
# 1829
if((int)k == (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62C;_tmp62C.tag=0U,({struct _fat_ptr _tmp86A=({const char*_tmp3DD="expecting struct but found union";_tag_fat(_tmp3DD,sizeof(char),33U);});_tmp62C.f1=_tmp86A;});_tmp62C;});void*_tmp3DB[1U];_tmp3DB[0]=& _tmp3DC;({unsigned _tmp86B=loc;Cyc_Warn_err2(_tmp86B,_tag_fat(_tmp3DB,sizeof(void*),1U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3DE=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3D=_tmp3DE;struct _tuple20*_tmp3DF=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Aggrfield*_tmp3E0;_LL6: _tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp3E0;struct Cyc_Absyn_Exp*field_exp=_tmp3E1;
({struct Cyc_Tcenv_Tenv*_tmp86D=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp86C=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp86D,_tmp86C,field_exp);});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp86F=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp86E=field_exp;Cyc_Tcutil_coerce_arg(_tmp86F,_tmp86E,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp632;_tmp632.tag=0U,({struct _fat_ptr _tmp870=({const char*_tmp3EB="field ";_tag_fat(_tmp3EB,sizeof(char),7U);});_tmp632.f1=_tmp870;});_tmp632;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp631;_tmp631.tag=0U,_tmp631.f1=*field->name;_tmp631;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp630;_tmp630.tag=0U,({
struct _fat_ptr _tmp871=({const char*_tmp3EA=" of struct expects type ";_tag_fat(_tmp3EA,sizeof(char),25U);});_tmp630.f1=_tmp871;});_tmp630;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3E6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62F;_tmp62F.tag=2U,_tmp62F.f1=(void*)field->type;_tmp62F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62E;_tmp62E.tag=0U,({struct _fat_ptr _tmp872=({const char*_tmp3E9=" != ";_tag_fat(_tmp3E9,sizeof(char),5U);});_tmp62E.f1=_tmp872;});_tmp62E;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3E8=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp62D;_tmp62D.tag=3U,_tmp62D.f1=(void*)field_exp->topt;_tmp62D;});void*_tmp3E2[6U];_tmp3E2[0]=& _tmp3E3,_tmp3E2[1]=& _tmp3E4,_tmp3E2[2]=& _tmp3E5,_tmp3E2[3]=& _tmp3E6,_tmp3E2[4]=& _tmp3E7,_tmp3E2[5]=& _tmp3E8;({unsigned _tmp873=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp873,_tag_fat(_tmp3E2,sizeof(void*),6U));});});}}
# 1842
goto _LL0;}}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,({struct _fat_ptr _tmp874=({const char*_tmp3EE="tcAnonStruct: wrong type";_tag_fat(_tmp3EE,sizeof(char),25U);});_tmp633.f1=_tmp874;});_tmp633;});void*_tmp3EC[1U];_tmp3EC[0]=& _tmp3ED;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3EC,sizeof(void*),1U));});}_LL0:;}
# 1845
return ts;}
# 1849
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1852
struct _tuple13 _tmp3EF=({struct _tuple13 _tmp63E;({struct Cyc_List_List*_tmp875=Cyc_Tcenv_lookup_type_vars(te);_tmp63E.f1=_tmp875;}),_tmp63E.f2=Cyc_Core_heap_region;_tmp63E;});struct _tuple13 env=_tmp3EF;
struct Cyc_List_List*_tmp3F0=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3F0;
struct Cyc_List_List*_tmp3F1=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp3F1;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp876=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp876,all_typs);});
# 1857
if(topt != 0){
void*_tmp3F2=Cyc_Absyn_compress(*topt);void*_stmttmp3E=_tmp3F2;void*_tmp3F3=_stmttmp3E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F3)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F3)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1863
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp878=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp877=e;Cyc_Tcutil_coerce_arg(_tmp878,_tmp877,t,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp639;_tmp639.tag=0U,({struct _fat_ptr _tmp879=({const char*_tmp3FE="datatype constructor ";_tag_fat(_tmp3FE,sizeof(char),22U);});_tmp639.f1=_tmp879;});_tmp639;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3F6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp638;_tmp638.tag=1U,_tmp638.f1=tuf->name;_tmp638;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp637;_tmp637.tag=0U,({
struct _fat_ptr _tmp87A=({const char*_tmp3FD=" expects argument of type ";_tag_fat(_tmp3FD,sizeof(char),27U);});_tmp637.f1=_tmp87A;});_tmp637;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp636;_tmp636.tag=2U,_tmp636.f1=(void*)t;_tmp636;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp87B=({const char*_tmp3FC=" but this argument has type ";_tag_fat(_tmp3FC,sizeof(char),29U);});_tmp635.f1=_tmp87B;});_tmp635;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp634;_tmp634.tag=0U,
e->topt == 0?({struct _fat_ptr _tmp87D=(struct _fat_ptr)({const char*_tmp3FB="?";_tag_fat(_tmp3FB,sizeof(char),2U);});_tmp634.f1=_tmp87D;}):({struct _fat_ptr _tmp87C=Cyc_Absynpp_typ2string((void*)_check_null(e->topt));_tmp634.f1=_tmp87C;});_tmp634;});void*_tmp3F4[6U];_tmp3F4[0]=& _tmp3F5,_tmp3F4[1]=& _tmp3F6,_tmp3F4[2]=& _tmp3F7,_tmp3F4[3]=& _tmp3F8,_tmp3F4[4]=& _tmp3F9,_tmp3F4[5]=& _tmp3FA;({unsigned _tmp87E=e->loc;Cyc_Warn_err2(_tmp87E,_tag_fat(_tmp3F4,sizeof(void*),6U));});});
Cyc_Unify_explain_failure();}}
# 1876
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp400=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63B;_tmp63B.tag=0U,({
struct _fat_ptr _tmp87F=({const char*_tmp402="too many arguments for datatype constructor ";_tag_fat(_tmp402,sizeof(char),45U);});_tmp63B.f1=_tmp87F;});_tmp63B;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp401=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp63A;_tmp63A.tag=1U,_tmp63A.f1=tuf->name;_tmp63A;});void*_tmp3FF[2U];_tmp3FF[0]=& _tmp400,_tmp3FF[1]=& _tmp401;({struct Cyc_Tcenv_Tenv*_tmp882=te;unsigned _tmp881=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp880=topt;Cyc_Tcexp_expr_err(_tmp882,_tmp881,_tmp880,_tag_fat(_tmp3FF,sizeof(void*),2U));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp404=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D;_tmp63D.tag=0U,({
struct _fat_ptr _tmp883=({const char*_tmp406="too few arguments for datatype constructor ";_tag_fat(_tmp406,sizeof(char),44U);});_tmp63D.f1=_tmp883;});_tmp63D;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp405=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp63C;_tmp63C.tag=1U,_tmp63C.f1=tuf->name;_tmp63C;});void*_tmp403[2U];_tmp403[0]=& _tmp404,_tmp403[1]=& _tmp405;({struct Cyc_Tcenv_Tenv*_tmp886=te;unsigned _tmp885=loc;void**_tmp884=topt;Cyc_Tcexp_expr_err(_tmp886,_tmp885,_tmp884,_tag_fat(_tmp403,sizeof(void*),2U));});});
return res;}}
# 1886
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1888
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1891
if(topt == 0)
return 0;{
void*_tmp407=Cyc_Absyn_compress(*topt);void*_stmttmp3F=_tmp407;void*_tmp408=_stmttmp3F;void*_tmp409;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->tag == 3U){_LL1: _tmp409=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).elt_type;_LL2: {void*elt_typ=_tmp409;
# 1895
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1901
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1903
enum Cyc_Absyn_AliasQual _tmp40A=(Cyc_Tcutil_type_kind(Cyc_Absyn_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp40=_tmp40A;enum Cyc_Absyn_AliasQual _tmp40B=_stmttmp40;if(_tmp40B == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 1909
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 1913
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 1917
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp40C=Cyc_Absyn_compress(handle_type);void*_stmttmp41=_tmp40C;void*_tmp40D=_stmttmp41;void*_tmp40E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f2 != 0){_LL1: _tmp40E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f2)->hd;_LL2: {void*r=_tmp40E;
# 1922
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1926
({struct Cyc_Warn_String_Warn_Warg_struct _tmp410=({struct Cyc_Warn_String_Warn_Warg_struct _tmp640;_tmp640.tag=0U,({struct _fat_ptr _tmp887=({const char*_tmp412="expecting region_t type but found ";_tag_fat(_tmp412,sizeof(char),35U);});_tmp640.f1=_tmp887;});_tmp640;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp411=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63F;_tmp63F.tag=2U,_tmp63F.f1=(void*)handle_type;_tmp63F;});void*_tmp40F[2U];_tmp40F[0]=& _tmp410,_tmp40F[1]=& _tmp411;({unsigned _tmp888=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;Cyc_Warn_err2(_tmp888,_tag_fat(_tmp40F,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}else{
# 1932
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp889=Cyc_Absyn_uniquergn_exp();*ropt=_tmp889;});}}}
# 1939
({struct Cyc_Tcenv_Tenv*_tmp88A=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp88A,& Cyc_Absyn_uint_type,*e);});{
# 1948 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp414=({struct Cyc_Warn_String_Warn_Warg_struct _tmp641;_tmp641.tag=0U,({struct _fat_ptr _tmp88B=({const char*_tmp415="calloc with empty type";_tag_fat(_tmp415,sizeof(char),23U);});_tmp641.f1=_tmp88B;});_tmp641;});void*_tmp413[1U];_tmp413[0]=& _tmp414;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp413,sizeof(void*),1U));});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp88E=loc;struct Cyc_Tcenv_Tenv*_tmp88D=te;struct Cyc_List_List*_tmp88C=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp88E,_tmp88D,_tmp88C,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp417=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0U,({struct _fat_ptr _tmp88F=({const char*_tmp41B="calloc cannot be used with type ";_tag_fat(_tmp41B,sizeof(char),33U);});_tmp644.f1=_tmp88F;});_tmp644;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp418=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp643;_tmp643.tag=2U,_tmp643.f1=(void*)elt_type;_tmp643;});struct Cyc_Warn_String_Warn_Warg_struct _tmp419=({struct Cyc_Warn_String_Warn_Warg_struct _tmp642;_tmp642.tag=0U,({
struct _fat_ptr _tmp890=({const char*_tmp41A="\n\t(type needs initialization)";_tag_fat(_tmp41A,sizeof(char),30U);});_tmp642.f1=_tmp890;});_tmp642;});void*_tmp416[3U];_tmp416[0]=& _tmp417,_tmp416[1]=& _tmp418,_tmp416[2]=& _tmp419;({unsigned _tmp891=loc;Cyc_Warn_err2(_tmp891,_tag_fat(_tmp416,sizeof(void*),3U));});});
num_elts=*e;
one_elt=0;}else{
# 1963
void*er=(*e)->r;
retry_sizeof: {
void*_tmp41C=er;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_Exp*_tmp41D;void*_tmp41F;switch(*((int*)_tmp41C)){case 17U: _LL6: _tmp41F=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp41C)->f1;_LL7: {void*t2=_tmp41F;
# 1967
elt_type=t2;
({void**_tmp892=({void**_tmp420=_cycalloc(sizeof(*_tmp420));*_tmp420=elt_type;_tmp420;});*t=_tmp892;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41C)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41C)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41C)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41C)->f2)->tl)->tl == 0){_LL8: _tmp41D=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41C)->f2)->hd;_tmp41E=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41C)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp41D;struct Cyc_Absyn_Exp*e2=_tmp41E;
# 1974
{struct _tuple0 _tmp421=({struct _tuple0 _tmp648;_tmp648.f1=e1->r,_tmp648.f2=e2->r;_tmp648;});struct _tuple0 _stmttmp42=_tmp421;struct _tuple0 _tmp422=_stmttmp42;void*_tmp423;void*_tmp424;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp422.f1)->tag == 17U){_LLD: _tmp424=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp422.f1)->f1;_LLE: {void*t1=_tmp424;
# 1976
e1=e2;
_tmp423=t1;goto _LL10;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp422.f2)->tag == 17U){_LLF: _tmp423=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp422.f2)->f1;_LL10: {void*t2=_tmp423;
# 1980
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 1983
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp426=({struct Cyc_Warn_String_Warn_Warg_struct _tmp647;_tmp647.tag=0U,({struct _fat_ptr _tmp893=({const char*_tmp42A="malloc cannot be used with type ";_tag_fat(_tmp42A,sizeof(char),33U);});_tmp647.f1=_tmp893;});_tmp647;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp427=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp646;_tmp646.tag=2U,_tmp646.f1=(void*)t2;_tmp646;});struct Cyc_Warn_String_Warn_Warg_struct _tmp428=({struct Cyc_Warn_String_Warn_Warg_struct _tmp645;_tmp645.tag=0U,({
struct _fat_ptr _tmp894=({const char*_tmp429="\n\t(type needs initialization)";_tag_fat(_tmp429,sizeof(char),30U);});_tmp645.f1=_tmp894;});_tmp645;});void*_tmp425[3U];_tmp425[0]=& _tmp426,_tmp425[1]=& _tmp427,_tmp425[2]=& _tmp428;({unsigned _tmp895=loc;Cyc_Warn_err2(_tmp895,_tag_fat(_tmp425,sizeof(void*),3U));});});else{
# 1987
*is_calloc=1;}}
# 1989
elt_type=t2;
({void**_tmp896=({void**_tmp42B=_cycalloc(sizeof(*_tmp42B));*_tmp42B=elt_type;_tmp42B;});*t=_tmp896;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 1996
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 1998
 No_sizeof: {
# 2001
struct Cyc_Absyn_Exp*_tmp42C=*e;struct Cyc_Absyn_Exp*real_e=_tmp42C;
{void*_tmp42D=real_e->r;void*_stmttmp43=_tmp42D;void*_tmp42E=_stmttmp43;struct Cyc_Absyn_Exp*_tmp42F;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42E)->tag == 14U){_LL14: _tmp42F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42E)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp42F;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2006
{void*_tmp430=Cyc_Absyn_compress((void*)_check_null(real_e->topt));void*_stmttmp44=_tmp430;void*_tmp431=_stmttmp44;void*_tmp432;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp431)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp431)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp431)->f2 != 0){_LL19: _tmp432=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp431)->f2)->hd;_LL1A: {void*tagt=_tmp432;
# 2008
{void*_tmp433=Cyc_Absyn_compress(tagt);void*_stmttmp45=_tmp433;void*_tmp434=_stmttmp45;struct Cyc_Absyn_Exp*_tmp435;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp434)->tag == 9U){_LL1E: _tmp435=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp434)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp435;
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2012
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2015
elt_type=Cyc_Absyn_char_type;
({void**_tmp897=({void**_tmp436=_cycalloc(sizeof(*_tmp436));*_tmp436=elt_type;_tmp436;});*t=_tmp897;});
num_elts=*e;
one_elt=0;}
# 2020
goto _LL5;}_LL5:;}}
# 2024
*is_fat=!one_elt;
# 2027
{void*_tmp437=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp438;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp438=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp438;
# 2029
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp649;_tmp649.tag=0U,({struct _fat_ptr _tmp898=({const char*_tmp43B="malloc with existential types not yet implemented";_tag_fat(_tmp43B,sizeof(char),50U);});_tmp649.f1=_tmp898;});_tmp649;});void*_tmp439[1U];_tmp439[0]=& _tmp43A;({unsigned _tmp899=loc;Cyc_Warn_err2(_tmp899,_tag_fat(_tmp439,sizeof(void*),1U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2036
void*(*_tmp43C)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp43C;
void*_tmp43D=Cyc_Absyn_false_type;void*zero_term=_tmp43D;
if(topt != 0){
void*_tmp43E=Cyc_Absyn_compress(*topt);void*_stmttmp46=_tmp43E;void*_tmp43F=_stmttmp46;void*_tmp442;void*_tmp441;void*_tmp440;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->tag == 3U){_LL28: _tmp440=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).nullable;_tmp441=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).bounds;_tmp442=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).zero_term;_LL29: {void*n=_tmp440;void*b=_tmp441;void*zt=_tmp442;
# 2041
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2046
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp444=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64A;_tmp64A.tag=0U,({struct _fat_ptr _tmp89A=({const char*_tmp445="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp445,sizeof(char),55U);});_tmp64A.f1=_tmp89A;});_tmp64A;});void*_tmp443[1U];_tmp443[0]=& _tmp444;({unsigned _tmp89B=loc;Cyc_Warn_warn2(_tmp89B,_tag_fat(_tmp443,sizeof(void*),1U));});});
*is_calloc=1;}{
# 2052
struct Cyc_Absyn_Exp*_tmp446=({void*_tmp89C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp89C,b);});struct Cyc_Absyn_Exp*eopt=_tmp446;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp447=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp447;
int _tmp448=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp448;
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp8A0=elt_type;void*_tmp89F=rgn;struct Cyc_Absyn_Tqual _tmp89E=Cyc_Absyn_empty_tqual(0U);void*_tmp89D=b;Cyc_Absyn_atb_type(_tmp8A0,_tmp89F,_tmp89E,_tmp89D,zero_term);});}{
# 2060
void*_tmp449=Cyc_Absyn_compress((void*)_check_null(num_elts->topt));void*_stmttmp47=_tmp449;void*_tmp44A=_stmttmp47;void*_tmp44B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44A)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44A)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44A)->f2 != 0){_LL2D: _tmp44B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44A)->f2)->hd;_LL2E: {void*tagtyp=_tmp44B;
# 2062
struct Cyc_Absyn_Exp*_tmp44C=({void*_tmp8A1=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp8A1,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp44C;
# 2064
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp8A5=elt_type;void*_tmp8A4=rgn;struct Cyc_Absyn_Tqual _tmp8A3=Cyc_Absyn_empty_tqual(0U);void*_tmp8A2=b;Cyc_Absyn_atb_type(_tmp8A5,_tmp8A4,_tmp8A3,_tmp8A2,zero_term);});}
# 2068
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2072
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2075
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp8A9)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=ptr_maker;void*_tmp8A8=elt_type;void*_tmp8A7=rgn;struct Cyc_Absyn_Tqual _tmp8A6=Cyc_Absyn_empty_tqual(0U);_tmp8A9(_tmp8A8,_tmp8A7,_tmp8A6,zero_term);});}}}
# 2080
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2082
struct Cyc_Tcenv_Tenv*_tmp44D=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp44D;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp44E=(void*)_check_null(e1->topt);void*t1=_tmp44E;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2089
{void*_tmp44F=Cyc_Absyn_compress(t1);void*_stmttmp48=_tmp44F;void*_tmp450=_stmttmp48;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp450)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp452=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64B;_tmp64B.tag=0U,({struct _fat_ptr _tmp8AA=({const char*_tmp453="cannot assign to an array";_tag_fat(_tmp453,sizeof(char),26U);});_tmp64B.f1=_tmp8AA;});_tmp64B;});void*_tmp451[1U];_tmp451[0]=& _tmp452;({unsigned _tmp8AB=loc;Cyc_Warn_err2(_tmp8AB,_tag_fat(_tmp451,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2094
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp455=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64C;_tmp64C.tag=0U,({struct _fat_ptr _tmp8AC=({const char*_tmp456="swap not allowed for non-pointer or non-word-sized types";_tag_fat(_tmp456,sizeof(char),57U);});_tmp64C.f1=_tmp8AC;});_tmp64C;});void*_tmp454[1U];_tmp454[0]=& _tmp455;({unsigned _tmp8AD=loc;Cyc_Warn_err2(_tmp8AD,_tag_fat(_tmp454,sizeof(void*),1U));});});
# 2098
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp458=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64D;_tmp64D.tag=0U,({struct _fat_ptr _tmp8AE=({const char*_tmp459="swap non-lvalue";_tag_fat(_tmp459,sizeof(char),16U);});_tmp64D.f1=_tmp8AE;});_tmp64D;});void*_tmp457[1U];_tmp457[0]=& _tmp458;({struct Cyc_Tcenv_Tenv*_tmp8B1=te;unsigned _tmp8B0=e1->loc;void**_tmp8AF=topt;Cyc_Tcexp_expr_err(_tmp8B1,_tmp8B0,_tmp8AF,_tag_fat(_tmp457,sizeof(void*),1U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp45B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64E;_tmp64E.tag=0U,({struct _fat_ptr _tmp8B2=({const char*_tmp45C="swap non-lvalue";_tag_fat(_tmp45C,sizeof(char),16U);});_tmp64E.f1=_tmp8B2;});_tmp64E;});void*_tmp45A[1U];_tmp45A[0]=& _tmp45B;({struct Cyc_Tcenv_Tenv*_tmp8B5=te;unsigned _tmp8B4=e2->loc;void**_tmp8B3=topt;Cyc_Tcexp_expr_err(_tmp8B5,_tmp8B4,_tmp8B3,_tag_fat(_tmp45A,sizeof(void*),1U));});});
# 2103
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp45E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp652;_tmp652.tag=0U,({struct _fat_ptr _tmp8B6=({const char*_tmp463="type mismatch: ";_tag_fat(_tmp463,sizeof(char),16U);});_tmp652.f1=_tmp8B6;});_tmp652;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp45F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp651;_tmp651.tag=2U,_tmp651.f1=(void*)t1;_tmp651;});struct Cyc_Warn_String_Warn_Warg_struct _tmp460=({struct Cyc_Warn_String_Warn_Warg_struct _tmp650;_tmp650.tag=0U,({struct _fat_ptr _tmp8B7=({const char*_tmp462=" != ";_tag_fat(_tmp462,sizeof(char),5U);});_tmp650.f1=_tmp8B7;});_tmp650;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp461=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64F;_tmp64F.tag=2U,_tmp64F.f1=(void*)t2;_tmp64F;});void*_tmp45D[4U];_tmp45D[0]=& _tmp45E,_tmp45D[1]=& _tmp45F,_tmp45D[2]=& _tmp460,_tmp45D[3]=& _tmp461;({struct Cyc_Tcenv_Tenv*_tmp8BA=te;unsigned _tmp8B9=loc;void**_tmp8B8=topt;Cyc_Tcexp_expr_err(_tmp8BA,_tmp8B9,_tmp8B8,_tag_fat(_tmp45D,sizeof(void*),4U));});});
return Cyc_Absyn_void_type;}}}
# 2111
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp8BB=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp8BB,s,1);});
# 2114
while(1){
void*_tmp464=s->r;void*_stmttmp49=_tmp464;void*_tmp465=_stmttmp49;struct Cyc_Absyn_Stmt*_tmp466;struct Cyc_Absyn_Stmt*_tmp467;struct Cyc_Absyn_Exp*_tmp468;switch(*((int*)_tmp465)){case 1U: _LL1: _tmp468=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp465)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp468;
# 2118
void*_tmp469=(void*)_check_null(e->topt);void*t=_tmp469;
if(!({void*_tmp8BC=t;Cyc_Unify_unify(_tmp8BC,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp46B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp654;_tmp654.tag=0U,({struct _fat_ptr _tmp8BD=({const char*_tmp46D="statement expression returns type ";_tag_fat(_tmp46D,sizeof(char),35U);});_tmp654.f1=_tmp8BD;});_tmp654;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp46C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp653;_tmp653.tag=2U,_tmp653.f1=(void*)t;_tmp653;});void*_tmp46A[2U];_tmp46A[0]=& _tmp46B,_tmp46A[1]=& _tmp46C;({unsigned _tmp8BE=loc;Cyc_Tcexp_err_and_explain(_tmp8BE,_tag_fat(_tmp46A,sizeof(void*),2U));});});
return t;}case 2U: _LL3: _tmp467=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp465)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp467;
s=s2;continue;}case 12U: _LL5: _tmp466=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp465)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp466;
s=s1;continue;}default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp46F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp655;_tmp655.tag=0U,({
struct _fat_ptr _tmp8BF=({const char*_tmp470="statement expression must end with expression";_tag_fat(_tmp470,sizeof(char),46U);});_tmp655.f1=_tmp8BF;});_tmp655;});void*_tmp46E[1U];_tmp46E[0]=& _tmp46F;({struct Cyc_Tcenv_Tenv*_tmp8C2=te;unsigned _tmp8C1=loc;void**_tmp8C0=topt;Cyc_Tcexp_expr_err(_tmp8C2,_tmp8C1,_tmp8C0,_tag_fat(_tmp46E,sizeof(void*),1U));});});}_LL0:;}}
# 2129
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Absyn_compress(({struct Cyc_Tcenv_Tenv*_tmp8C3=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp8C3,0,e);}));
{void*_tmp471=t;struct Cyc_Absyn_Aggrdecl*_tmp472;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp471)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp471)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp471)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp472=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp471)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp472;
# 2133
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp474=({struct Cyc_Warn_String_Warn_Warg_struct _tmp657;_tmp657.tag=0U,({struct _fat_ptr _tmp8C4=({const char*_tmp476="expecting @tagged union but found ";_tag_fat(_tmp476,sizeof(char),35U);});_tmp657.f1=_tmp8C4;});_tmp657;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp475=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp656;_tmp656.tag=2U,_tmp656.f1=(void*)t;_tmp656;});void*_tmp473[2U];_tmp473[0]=& _tmp474,_tmp473[1]=& _tmp475;({unsigned _tmp8C5=loc;Cyc_Warn_err2(_tmp8C5,_tag_fat(_tmp473,sizeof(void*),2U));});});goto _LL0;}_LL0:;}
# 2138
return Cyc_Absyn_uint_type;}
# 2141
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
({struct Cyc_Tcenv_Tenv*_tmp8C7=te;struct Cyc_Absyn_Exp*_tmp8C6=e;Cyc_Tcexp_tcTest(_tmp8C7,_tmp8C6,({const char*_tmp477="@assert";_tag_fat(_tmp477,sizeof(char),8U);}));});
if(!Cyc_Absyn_no_side_effects_exp(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp479=({struct Cyc_Warn_String_Warn_Warg_struct _tmp658;_tmp658.tag=0U,({struct _fat_ptr _tmp8C8=({const char*_tmp47A="@assert expression may have side effects";_tag_fat(_tmp47A,sizeof(char),41U);});_tmp658.f1=_tmp8C8;});_tmp658;});void*_tmp478[1U];_tmp478[0]=& _tmp479;({unsigned _tmp8C9=loc;Cyc_Warn_err2(_tmp8C9,_tag_fat(_tmp478,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;}
# 2149
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2153
void*rgn=Cyc_Absyn_heap_rgn_type;
te=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2158
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp47B=Cyc_Absyn_compress(handle_type);void*_stmttmp4A=_tmp47B;void*_tmp47C=_stmttmp4A;void*_tmp47D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47C)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47C)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47C)->f2 != 0){_LL1: _tmp47D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47C)->f2)->hd;_LL2: {void*r=_tmp47D;
# 2163
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2167
({struct Cyc_Warn_String_Warn_Warg_struct _tmp47F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65A;_tmp65A.tag=0U,({struct _fat_ptr _tmp8CA=({const char*_tmp481="expecting region_t type but found ";_tag_fat(_tmp481,sizeof(char),35U);});_tmp65A.f1=_tmp8CA;});_tmp65A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp480=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp659;_tmp659.tag=2U,_tmp659.f1=(void*)handle_type;_tmp659;});void*_tmp47E[2U];_tmp47E[0]=& _tmp47F,_tmp47E[1]=& _tmp480;({unsigned _tmp8CB=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;Cyc_Warn_err2(_tmp8CB,_tag_fat(_tmp47E,sizeof(void*),2U));});});
# 2169
goto _LL0;}_LL0:;}else{
# 2171
if(topt != 0){
# 2174
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)
({struct Cyc_Absyn_Exp*_tmp8CC=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp8CC;});}}}{
# 2182
void*_tmp482=e1->r;void*_stmttmp4B=_tmp482;void*_tmp483=_stmttmp4B;struct Cyc_List_List*_tmp484;struct Cyc_List_List*_tmp486;struct Cyc_Core_Opt*_tmp485;switch(*((int*)_tmp483)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2187
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp488=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65B;_tmp65B.tag=0U,({struct _fat_ptr _tmp8CD=({const char*_tmp489="tcNew: comprehension returned non-array type";_tag_fat(_tmp489,sizeof(char),45U);});_tmp65B.f1=_tmp8CD;});_tmp65B;});void*_tmp487[1U];_tmp487[0]=& _tmp488;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp487,sizeof(void*),1U));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8D0=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8CF=loc;void*_tmp8CE=res_typ;Cyc_Tcutil_silent_castable(_tmp8D0,_tmp8CF,_tmp8CE,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2199
return res_typ;}case 36U: _LLA: _tmp485=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp486=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp485;struct Cyc_List_List*des=_tmp486;
# 2201
({void*_tmp8D1=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->tag=26U,_tmp48A->f1=des;_tmp48A;});e1->r=_tmp8D1;});
_tmp484=des;goto _LLD;}case 26U: _LLC: _tmp484=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LLD: {struct Cyc_List_List*des=_tmp484;
# 2204
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp48B=Cyc_Absyn_compress(*topt);void*_stmttmp4C=_tmp48B;void*_tmp48C=_stmttmp4C;void*_tmp48E;void**_tmp48D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->tag == 3U){_LL15: _tmp48D=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).elt_type;_tmp48E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp48D;void*zt=_tmp48E;
# 2210
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2216
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp490=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65C;_tmp65C.tag=0U,({struct _fat_ptr _tmp8D2=({const char*_tmp491="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp491,sizeof(char),50U);});_tmp65C.f1=_tmp8D2;});_tmp65C;});void*_tmp48F[1U];_tmp48F[0]=& _tmp490;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp48F,sizeof(void*),1U));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2225
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8D5=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8D4=loc;void*_tmp8D3=res_typ;Cyc_Tcutil_silent_castable(_tmp8D5,_tmp8D4,_tmp8D3,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2232
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp483)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2237
void*_tmp492=({void*_tmp8D9=Cyc_Absyn_char_type;void*_tmp8D8=rgn;struct Cyc_Absyn_Tqual _tmp8D7=Cyc_Absyn_const_tqual(0U);void*_tmp8D6=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8D9,_tmp8D8,_tmp8D7,_tmp8D6,Cyc_Absyn_true_type);});void*topt2=_tmp492;
# 2239
void*_tmp493=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp493;
return({void*_tmp8DD=t;void*_tmp8DC=rgn;struct Cyc_Absyn_Tqual _tmp8DB=Cyc_Absyn_empty_tqual(0U);void*_tmp8DA=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2240
Cyc_Absyn_atb_type(_tmp8DD,_tmp8DC,_tmp8DB,_tmp8DA,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2244
void*_tmp494=({void*_tmp8E1=Cyc_Absyn_wchar_type();void*_tmp8E0=rgn;struct Cyc_Absyn_Tqual _tmp8DF=Cyc_Absyn_const_tqual(0U);void*_tmp8DE=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8E1,_tmp8E0,_tmp8DF,_tmp8DE,Cyc_Absyn_true_type);});void*topt2=_tmp494;
# 2246
void*_tmp495=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp495;
return({void*_tmp8E5=t;void*_tmp8E4=rgn;struct Cyc_Absyn_Tqual _tmp8E3=Cyc_Absyn_empty_tqual(0U);void*_tmp8E2=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2247
Cyc_Absyn_atb_type(_tmp8E5,_tmp8E4,_tmp8E3,_tmp8E2,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2253
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp496=Cyc_Absyn_compress(*topt);void*_stmttmp4D=_tmp496;void*_tmp497=_stmttmp4D;struct Cyc_Absyn_Tqual _tmp499;void**_tmp498;switch(*((int*)_tmp497)){case 3U: _LL1A: _tmp498=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->f1).elt_type;_tmp499=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp497)->f1).elt_tq;_LL1B: {void**elttype=_tmp498;struct Cyc_Absyn_Tqual tq=_tmp499;
# 2259
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp497)->f1)->tag == 18U){_LL1C: _LL1D:
# 2263
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2269
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2271
Cyc_Tcexp_check_consume(e1->loc,telt,e1);{
struct Cyc_Absyn_PtrInfo _tmp49A=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp49A;
pi.elt_type=telt;
(pi.ptr_atts).rgn=rgn;
({void*_tmp8E6=Cyc_Absyn_bounds_one();(pi.ptr_atts).bounds=_tmp8E6;});
(pi.ptr_atts).zero_term=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp8E9=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8E8=loc;void*_tmp8E7=res_typ;Cyc_Tcutil_silent_castable(_tmp8E9,_tmp8E8,_tmp8E7,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}
# 2284
return res_typ;}}}}}_LL5:;}}
# 2290
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp8EB=t=({void*_tmp8EA=t;Cyc_Tcutil_promote_array(_tmp8EA,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp8EB;});
return t;}
# 2300
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2304
Cyc_Tcexp_check_consume(e->loc,t,e);{
void*_tmp49B=e->r;void*_stmttmp4E=_tmp49B;void*_tmp49C=_stmttmp4E;switch(*((int*)_tmp49C)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49C)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2312
 if(Cyc_Tcutil_is_array_type(t))
({void*_tmp8ED=t=({void*_tmp8EC=t;Cyc_Tcutil_promote_array(_tmp8EC,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp8ED;});
return t;}_LL0:;}}
# 2325 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*t=({void*_tmp8EE=Cyc_Absyn_compress(Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0));e->topt=_tmp8EE;});
# 2329
{void*_tmp49D=e->r;void*_stmttmp4F=_tmp49D;void*_tmp49E=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp49F;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp49E)->tag == 12U){_LL1: _tmp49F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp49E)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp49F;
return t;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2334
void*_tmp4A0=t;void*_tmp4A1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->tag == 3U){_LL6: _tmp4A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).elt_type;_LL7: {void*t2=_tmp4A1;
# 2336
void*_tmp4A2=Cyc_Absyn_compress(t2);void*_stmttmp50=_tmp4A2;void*_tmp4A3=_stmttmp50;struct Cyc_Absyn_FnInfo _tmp4A4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A3)->tag == 5U){_LLB: _tmp4A4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A3)->f1;_LLC: {struct Cyc_Absyn_FnInfo info=_tmp4A4;
# 2338
struct _tuple13 _tmp4A5=({struct _tuple13 _tmp65D;({struct Cyc_List_List*_tmp8EF=Cyc_Tcenv_lookup_type_vars(te);_tmp65D.f1=_tmp8EF;}),_tmp65D.f2=Cyc_Core_heap_region;_tmp65D;});struct _tuple13 env=_tmp4A5;
struct Cyc_List_List*_tmp4A6=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);struct Cyc_List_List*inst=_tmp4A6;
struct Cyc_List_List*_tmp4A7=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*ts=_tmp4A7;
struct Cyc_Absyn_Exp*_tmp4A8=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp4A8;
({void*_tmp8F0=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->tag=13U,_tmp4A9->f1=inner,_tmp4A9->f2=ts;_tmp4A9;});e->r=_tmp8F0;});
({void*_tmp8F1=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_tmp8F1;});
return(void*)_check_null(e->topt);}}else{_LLD: _LLE:
 return t;}_LLA:;}}else{_LL8: _LL9:
# 2347
 return t;}_LL5:;}}}
# 2356
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2358
struct Cyc_List_List*_tmp4AA=0;struct Cyc_List_List*decls=_tmp4AA;
# 2360
{void*_tmp4AB=fn_exp->r;void*_stmttmp51=_tmp4AB;void*_tmp4AC=_stmttmp51;struct Cyc_List_List*_tmp4AD;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AC)->tag == 10U){_LL1: _tmp4AD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AC)->f2;_LL2: {struct Cyc_List_List*es=_tmp4AD;
# 2362
{void*_tmp4AE=e->r;void*_stmttmp52=_tmp4AE;void*_tmp4AF=_stmttmp52;struct Cyc_List_List*_tmp4B0;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AF)->tag == 10U){_LL6: _tmp4B0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AF)->f2;_LL7: {struct Cyc_List_List*es2=_tmp4B0;
# 2364
struct Cyc_List_List*_tmp4B1=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp4B1;
int _tmp4B2=0;int arg_cnt=_tmp4B2;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp662;_tmp662.tag=0U,({struct _fat_ptr _tmp8F2=({const char*_tmp4BB="bad count ";_tag_fat(_tmp4BB,sizeof(char),11U);});_tmp662.f1=_tmp8F2;});_tmp662;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4B5=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp661;_tmp661.tag=12U,_tmp661.f1=arg_cnt;_tmp661;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp660;_tmp660.tag=0U,({struct _fat_ptr _tmp8F3=({const char*_tmp4BA="/";_tag_fat(_tmp4BA,sizeof(char),2U);});_tmp660.f1=_tmp8F3;});_tmp660;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4B7=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp65F;_tmp65F.tag=12U,_tmp65F.f1=(int)arg_exps->hd;_tmp65F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65E;_tmp65E.tag=0U,({
struct _fat_ptr _tmp8F4=({const char*_tmp4B9=" for alias coercion!";_tag_fat(_tmp4B9,sizeof(char),21U);});_tmp65E.f1=_tmp8F4;});_tmp65E;});void*_tmp4B3[5U];_tmp4B3[0]=& _tmp4B4,_tmp4B3[1]=& _tmp4B5,_tmp4B3[2]=& _tmp4B6,_tmp4B3[3]=& _tmp4B7,_tmp4B3[4]=& _tmp4B8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4B3,sizeof(void*),5U));});
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2376
struct _tuple12 _tmp4BC=({struct Cyc_Absyn_Exp*_tmp8F5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp8F5,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp53=_tmp4BC;struct _tuple12 _tmp4BD=_stmttmp53;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Decl*_tmp4BE;_LLB: _tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp4BE;struct Cyc_Absyn_Exp*ve=_tmp4BF;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->hd=d,_tmp4C0->tl=decls;_tmp4C0;});
arg_exps=arg_exps->tl;}}}
# 2381
goto _LL5;}}else{_LL8: _LL9:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp663;_tmp663.tag=0U,({struct _fat_ptr _tmp8F6=({const char*_tmp4C3="not a function call!";_tag_fat(_tmp4C3,sizeof(char),21U);});_tmp663.f1=_tmp8F6;});_tmp663;});void*_tmp4C1[1U];_tmp4C1[0]=& _tmp4C2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4C1,sizeof(void*),1U));});}_LL5:;}
# 2384
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp664;_tmp664.tag=0U,({struct _fat_ptr _tmp8F7=({const char*_tmp4C6="not a function call!";_tag_fat(_tmp4C6,sizeof(char),21U);});_tmp664.f1=_tmp8F7;});_tmp664;});void*_tmp4C4[1U];_tmp4C4[0]=& _tmp4C5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4C4,sizeof(void*),1U));});}_LL0:;}
# 2388
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp4C7=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp4C7;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp8FA=({struct Cyc_Absyn_Decl*_tmp8F9=d;struct Cyc_Absyn_Stmt*_tmp8F8=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp8F9,_tmp8F8,e->loc);});Cyc_Absyn_stmt_exp(_tmp8FA,e->loc);});
decls=decls->tl;}
# 2394
e->topt=0;
e->r=fn_exp->r;}
# 2399
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2403
{void*_tmp4C8=e->r;void*_stmttmp54=_tmp4C8;void*_tmp4C9=_stmttmp54;void*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CC;struct _fat_ptr*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4CF;int*_tmp4D5;struct Cyc_Absyn_Exp**_tmp4D4;void***_tmp4D3;struct Cyc_Absyn_Exp**_tmp4D2;int*_tmp4D1;struct Cyc_Absyn_Enumfield*_tmp4D7;void*_tmp4D6;struct Cyc_Absyn_Enumfield*_tmp4D9;struct Cyc_Absyn_Enumdecl*_tmp4D8;struct Cyc_Absyn_Datatypefield*_tmp4DC;struct Cyc_Absyn_Datatypedecl*_tmp4DB;struct Cyc_List_List*_tmp4DA;struct Cyc_List_List*_tmp4DE;void*_tmp4DD;struct Cyc_Absyn_Aggrdecl**_tmp4E2;struct Cyc_List_List*_tmp4E1;struct Cyc_List_List**_tmp4E0;struct _tuple1**_tmp4DF;int*_tmp4E5;void*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;int*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Vardecl*_tmp4E6;struct Cyc_Absyn_Stmt*_tmp4EA;struct Cyc_List_List*_tmp4EB;struct Cyc_List_List*_tmp4ED;struct _tuple9*_tmp4EC;struct Cyc_List_List*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4EF;int*_tmp4F4;int*_tmp4F3;struct _fat_ptr*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;int*_tmp4F8;int*_tmp4F7;struct _fat_ptr*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_List_List*_tmp4FB;void*_tmp4FA;void*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp**_tmp4FE;struct Cyc_Absyn_Exp*_tmp500;enum Cyc_Absyn_Coercion*_tmp503;struct Cyc_Absyn_Exp*_tmp502;void*_tmp501;struct Cyc_List_List*_tmp505;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp508;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Core_Opt*_tmp511;struct Cyc_Absyn_Exp*_tmp510;enum Cyc_Absyn_Incrementor _tmp514;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_List_List*_tmp516;enum Cyc_Absyn_Primop _tmp515;void**_tmp517;union Cyc_Absyn_Cnst*_tmp518;struct Cyc_List_List*_tmp51A;struct Cyc_Core_Opt*_tmp519;struct Cyc_Absyn_VarargCallInfo**_tmp51D;struct Cyc_List_List*_tmp51C;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51E;switch(*((int*)_tmp4C9)){case 12U: _LL1: _tmp51E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp51E;
# 2408
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=(void*)_check_null(e2->topt);
return;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C9)->f4 == 0){_LL3: _LL4:
# 2412
({struct Cyc_Warn_String_Warn_Warg_struct _tmp520=({struct Cyc_Warn_String_Warn_Warg_struct _tmp665;_tmp665.tag=0U,({struct _fat_ptr _tmp8FB=({const char*_tmp521="unresolved function: tcExpNoInst";_tag_fat(_tmp521,sizeof(char),33U);});_tmp665.f1=_tmp8FB;});_tmp665;});void*_tmp51F[1U];_tmp51F[0]=& _tmp520;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp51F,sizeof(void*),1U));});}else{_LL1B: _tmp51B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp51C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp51D=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp51B;struct Cyc_List_List*es=_tmp51C;struct Cyc_Absyn_VarargCallInfo**vci=_tmp51D;
# 2435
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp522;_push_handler(& _tmp522);{int _tmp524=0;if(setjmp(_tmp522.handler))_tmp524=1;if(!_tmp524){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp523=(void*)Cyc_Core_get_exn_thrown();void*_tmp525=_tmp523;void*_tmp526;if(((struct Cyc_Core_Failure_exn_struct*)_tmp525)->tag == Cyc_Core_Failure){_LL5A: _LL5B:
# 2440
 ok=0;
fn_exp=e;
goto _LL59;}else{_LL5C: _tmp526=_tmp525;_LL5D: {void*exn=_tmp526;(int)_rethrow(exn);}}_LL59:;}}}
# 2444
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2451
goto _LL0;}}case 36U: _LL5: _tmp519=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp51A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp519;struct Cyc_List_List*des=_tmp51A;
# 2416
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp518=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp518;
# 2420
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp517=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LLA: {void**b=_tmp517;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp515=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp516=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp515;struct Cyc_List_List*es=_tmp516;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp513=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp514=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp513;enum Cyc_Absyn_Incrementor i=_tmp514;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp510=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp511=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp512=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp510;struct Cyc_Core_Opt*popt=_tmp511;struct Cyc_Absyn_Exp*e2=_tmp512;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp50D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp50E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp50F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp50D;struct Cyc_Absyn_Exp*e2=_tmp50E;struct Cyc_Absyn_Exp*e3=_tmp50F;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp50B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp50C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp50B;struct Cyc_Absyn_Exp*e2=_tmp50C;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp509=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp50A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp509;struct Cyc_Absyn_Exp*e2=_tmp50A;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp507=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp508=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp507;struct Cyc_Absyn_Exp*e2=_tmp508;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp506=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp506;
# 2452
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp504=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp505=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp504;struct Cyc_List_List*ts=_tmp505;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp501=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp502=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp503=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C9)->f4;_LL22: {void*t1=_tmp501;struct Cyc_Absyn_Exp*e1=_tmp502;enum Cyc_Absyn_Coercion*c=_tmp503;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp500=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp500;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp4FE=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4FF=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp4FE;struct Cyc_Absyn_Exp*e1=_tmp4FF;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp4FD=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp4FD;
_tmp4FC=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17U: _LL29: _tmp4FC=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL2A: {void*t1=_tmp4FC;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp4FA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4FB=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL2C: {void*t1=_tmp4FA;struct Cyc_List_List*l=_tmp4FB;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp4F9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp4F9;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp4F5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4F6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp4F7=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_tmp4F8=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C9)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp4F5;struct _fat_ptr*f=_tmp4F6;int*is_tagged=_tmp4F7;int*is_read=_tmp4F8;
# 2462
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp4F1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4F2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp4F3=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_tmp4F4=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C9)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp4F1;struct _fat_ptr*f=_tmp4F2;int*is_tagged=_tmp4F3;int*is_read=_tmp4F4;
# 2464
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp4EF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4F0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp4EF;struct Cyc_Absyn_Exp*e2=_tmp4F0;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp4EE=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL36: {struct Cyc_List_List*es=_tmp4EE;
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp4EC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4ED=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL38: {struct _tuple9*t1=_tmp4EC;struct Cyc_List_List*des=_tmp4ED;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp4EB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL3A: {struct Cyc_List_List*des=_tmp4EB;
# 2471
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp527=Cyc_Absyn_compress(*topt);void*_stmttmp55=_tmp527;void*_tmp528=_stmttmp55;void*_tmp52B;struct Cyc_Absyn_Tqual*_tmp52A;void**_tmp529;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp528)->tag == 4U){_LL5F: _tmp529=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp528)->f1).elt_type;_tmp52A=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp528)->f1).tq;_tmp52B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp528)->f1).zero_term;_LL60: {void**et=_tmp529;struct Cyc_Absyn_Tqual*etq=_tmp52A;void*zt=_tmp52B;
# 2477
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}
# 2483
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp4EA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp4EA;
# 2485
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp4E6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4E7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp4E8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_tmp4E9=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C9)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp4E6;struct Cyc_Absyn_Exp*e1=_tmp4E7;struct Cyc_Absyn_Exp*e2=_tmp4E8;int*iszeroterm=_tmp4E9;
# 2487
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp4E3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4E4=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp4E5=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp4E3;void*t1=_tmp4E4;int*iszeroterm=_tmp4E5;
# 2489
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp4DF=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4E0=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp4E1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_tmp4E2=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C9)->f4;_LL42: {struct _tuple1**tn=_tmp4DF;struct Cyc_List_List**ts=_tmp4E0;struct Cyc_List_List*args=_tmp4E1;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4E2;
# 2491
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp4DD=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4DE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL44: {void*ts=_tmp4DD;struct Cyc_List_List*args=_tmp4DE;
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp4DA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4DB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_tmp4DC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C9)->f3;_LL46: {struct Cyc_List_List*es=_tmp4DA;struct Cyc_Absyn_Datatypedecl*tud=_tmp4DB;struct Cyc_Absyn_Datatypefield*tuf=_tmp4DC;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp4D8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4D9=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp4D8;struct Cyc_Absyn_Enumfield*ef=_tmp4D9;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp4D6=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4D7=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL4A: {void*t2=_tmp4D6;struct Cyc_Absyn_Enumfield*ef=_tmp4D7;
t=t2;goto _LL0;}case 34U: _LL4B: _tmp4D1=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1).is_calloc;_tmp4D2=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1).rgn;_tmp4D3=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1).elt_type;_tmp4D4=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1).num_elts;_tmp4D5=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1).fat_result;_LL4C: {int*is_calloc=_tmp4D1;struct Cyc_Absyn_Exp**ropt=_tmp4D2;void***t2=_tmp4D3;struct Cyc_Absyn_Exp**e2=_tmp4D4;int*isfat=_tmp4D5;
# 2497
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp4CF=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4D0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp4CF;struct Cyc_Absyn_Exp*e2=_tmp4D0;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp4CD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_tmp4CE=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4C9)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp4CD;struct _fat_ptr*f=_tmp4CE;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp4CC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp4CC;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 42U: _LL53: _tmp4CB=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL54: {struct Cyc_Absyn_Exp*e=_tmp4CB;
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 39U: _LL55: _tmp4CA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4C9)->f1;_LL56: {void*t2=_tmp4CA;
# 2503
if(!te->allow_valueof)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp666;_tmp666.tag=0U,({struct _fat_ptr _tmp8FC=({const char*_tmp52E="valueof(-) can only occur within types";_tag_fat(_tmp52E,sizeof(char),39U);});_tmp666.f1=_tmp8FC;});_tmp666;});void*_tmp52C[1U];_tmp52C[0]=& _tmp52D;({unsigned _tmp8FD=e->loc;Cyc_Warn_err2(_tmp8FD,_tag_fat(_tmp52C,sizeof(void*),1U));});});
# 2511
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL57: _LL58:
# 2515
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 2517
e->topt=t;}

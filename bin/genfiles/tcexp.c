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
# 61 "/tmp/cyclone/include/list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
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
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "/tmp/cyclone/include/cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "/tmp/cyclone/include/string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "/tmp/cyclone/include/string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "/tmp/cyclone/src/absyn.h"
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
# 414 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 493
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 495
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 884 "/tmp/cyclone/src/absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 890
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 900
void*Cyc_Absyn_compress(void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 916
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 919
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 921
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 923
extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_wchar_type (void);
void*Cyc_Absyn_gen_float_type(unsigned i);
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 930
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 949
void*Cyc_Absyn_exn_type (void);
# 957
extern void*Cyc_Absyn_fat_bound_type;
# 959
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 961
void*Cyc_Absyn_bounds_one (void);
# 963
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 967
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 969
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 971
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 975
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 983
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 987
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 993
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void);
# 1065
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1130
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1132
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1134
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1137
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1139
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66 "/tmp/cyclone/src/warn.h"
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "/tmp/cyclone/src/absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "/tmp/cyclone/src/unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "/tmp/cyclone/src/relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 31 "/tmp/cyclone/src/tcutil.h"
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
# 76 "/tmp/cyclone/src/tcenv.h"
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
# 29 "/tmp/cyclone/src/currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void);
void*Cyc_CurRgn_curr_rgn_type (void);
# 35
void Cyc_CurRgn_inst_curr_rgn(struct Cyc_Tcenv_Tenv*te,struct _tuple14*instv);struct _tuple16{unsigned f1;int f2;};
# 28 "/tmp/cyclone/src/evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "/tmp/cyclone/src/evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 53
extern int Cyc_Evexp_okay_szofarg(void*t);
# 26 "/tmp/cyclone/src/tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 30 "/tmp/cyclone/src/formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 34
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 29 "/tmp/cyclone/src/tc.h"
extern int Cyc_Tc_aggressive_warn;
# 44 "/tmp/cyclone/src/tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 26 "/tmp/cyclone/src/tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr msg_part);
void Cyc_Tcexp_check_consume(unsigned,void*,struct Cyc_Absyn_Exp*);
# 45 "/tmp/cyclone/src/tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr ap){
# 47
Cyc_Warn_verr2(loc,ap);
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
return*topt;}
# 52
static void Cyc_Tcexp_err_and_explain(unsigned loc,struct _fat_ptr ap){
Cyc_Warn_verr2(loc,ap);
Cyc_Unify_explain_failure();}
# 57
void Cyc_Tcexp_check_consume(unsigned loc,void*t,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
# 62
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52C;_tmp52C.tag=0U,({struct _fat_ptr _tmp666=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp52C.f1=_tmp666;});_tmp52C;});void*_tmp0[1U];_tmp0[0]=& _tmp1;({unsigned _tmp667=loc;Cyc_Warn_err2(_tmp667,_tag_fat(_tmp0,sizeof(void*),1U));});});}
# 66
static struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp3;
void*_tmp4=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->v=tenv_tvs;_tmpA;}));void*t2=_tmp4;
void*_tmp5=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->v=tenv_tvs;_tmp9;}));void*rt=_tmp5;
void*_tmp6=Cyc_Tcutil_any_bool(tenv_tvs);void*nbl=_tmp6;
void*_tmp7=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp7;
void*_tmp8=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp8;
return({struct Cyc_Absyn_PtrInfo _tmp52D;_tmp52D.elt_type=t2,({struct Cyc_Absyn_Tqual _tmp668=Cyc_Absyn_empty_tqual(0U);_tmp52D.elt_tq=_tmp668;}),(_tmp52D.ptr_atts).rgn=rt,(_tmp52D.ptr_atts).nullable=nbl,(_tmp52D.ptr_atts).bounds=b,(_tmp52D.ptr_atts).zero_term=zt,(_tmp52D.ptr_atts).ptrloc=0;_tmp52D;});}
# 76
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 80
if(topt == 0){
# 82
({void*_tmp669=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp669;});
return;}{
# 85
void*t=*topt;
void*_tmpC=Cyc_Absyn_compress(t);void*_stmttmp0=_tmpC;void*_tmpD=_stmttmp0;struct Cyc_Absyn_Tqual _tmpF;void*_tmpE;union Cyc_Absyn_AggrInfo _tmp10;switch(*((int*)_tmpD)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 20U){_LL1: _tmp10=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp10;
# 88
{union Cyc_Absyn_AggrInfo _tmp11=info;struct Cyc_Absyn_Aggrdecl*_tmp12;if((_tmp11.UnknownAggr).tag == 1){_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52E;_tmp52E.tag=0U,({struct _fat_ptr _tmp66A=({const char*_tmp15="struct type improperly set";_tag_fat(_tmp15,sizeof(char),27U);});_tmp52E.f1=_tmp66A;});_tmp52E;});void*_tmp13[1U];_tmp13[0]=& _tmp14;({unsigned _tmp66B=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp66B,_tag_fat(_tmp13,sizeof(void*),1U));});});}else{_LLC: _tmp12=*(_tmp11.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmp12;
({void*_tmp66C=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=29U,_tmp16->f1=ad->name,_tmp16->f2=0,_tmp16->f3=des,_tmp16->f4=ad;_tmp16;});e->r=_tmp66C;});}}_LL9:;}
# 92
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).elt_type;_tmpF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).tq;_LL4: {void*at=_tmpE;struct Cyc_Absyn_Tqual aq=_tmpF;
({void*_tmp66D=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=26U,_tmp17->f1=des;_tmp17;});e->r=_tmp66D;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp66E=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=30U,_tmp18->f1=t,_tmp18->f2=des;_tmp18;});e->r=_tmp66E;});goto _LL0;default: _LL7: _LL8:
({void*_tmp66F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=26U,_tmp19->f1=des;_tmp19;});e->r=_tmp66F;});goto _LL0;}_LL0:;}}
# 102
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 106
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp1A=e->r;void*_stmttmp1=_tmp1A;void*_tmp1B=_stmttmp1;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->f2 == 0){_LL1: _LL2:
# 109
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52F;_tmp52F.tag=0U,({struct _fat_ptr _tmp670=({const char*_tmp1E="assignment in test";_tag_fat(_tmp1E,sizeof(char),19U);});_tmp52F.f1=_tmp670;});_tmp52F;});void*_tmp1C[1U];_tmp1C[0]=& _tmp1D;({unsigned _tmp671=e->loc;Cyc_Warn_warn2(_tmp671,_tag_fat(_tmp1C,sizeof(void*),1U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 117
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp534;_tmp534.tag=0U,({struct _fat_ptr _tmp672=({const char*_tmp27="test of ";_tag_fat(_tmp27,sizeof(char),9U);});_tmp534.f1=_tmp672;});_tmp534;});struct Cyc_Warn_String_Warn_Warg_struct _tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _tmp533;_tmp533.tag=0U,_tmp533.f1=msg_part;_tmp533;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp532;_tmp532.tag=0U,({struct _fat_ptr _tmp673=({const char*_tmp26=" has type ";_tag_fat(_tmp26,sizeof(char),11U);});_tmp532.f1=_tmp673;});_tmp532;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp23=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp531;_tmp531.tag=3U,_tmp531.f1=(void*)e->topt;_tmp531;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24=({struct Cyc_Warn_String_Warn_Warg_struct _tmp530;_tmp530.tag=0U,({
struct _fat_ptr _tmp674=({const char*_tmp25=" instead of integral or pointer type";_tag_fat(_tmp25,sizeof(char),37U);});_tmp530.f1=_tmp674;});_tmp530;});void*_tmp1F[5U];_tmp1F[0]=& _tmp20,_tmp1F[1]=& _tmp21,_tmp1F[2]=& _tmp22,_tmp1F[3]=& _tmp23,_tmp1F[4]=& _tmp24;({unsigned _tmp675=e->loc;Cyc_Warn_err2(_tmp675,_tag_fat(_tmp1F,sizeof(void*),5U));});});}
# 139 "/tmp/cyclone/src/tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 144
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
# 159
if(topt == 0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_tmp2F=Cyc_Absyn_compress(*topt);void*_stmttmp3=_tmp2F;void*_tmp30=_stmttmp3;int _tmp31;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1)->tag == 2U){_LL1C: _tmp31=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1)->f1;_LL1D: {int i=_tmp31;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E: _LL1F:
 return Cyc_Absyn_gen_float_type((unsigned)i);}_LL1B:;}}case 5U: _LL13: _tmp2C=((_tmp29.Int_c).val).f1;_tmp2D=((_tmp29.Int_c).val).f2;_LL14: {enum Cyc_Absyn_Sign csn=_tmp2C;int i=_tmp2D;
# 169
if(topt == 0)
return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;{
void*_tmp32=Cyc_Absyn_compress(*topt);void*_stmttmp4=_tmp32;void*_tmp33=_stmttmp4;void*_tmp34;enum Cyc_Absyn_Sign _tmp35;enum Cyc_Absyn_Sign _tmp36;enum Cyc_Absyn_Sign _tmp37;enum Cyc_Absyn_Sign _tmp38;switch(*((int*)_tmp33)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f2){case Cyc_Absyn_Char_sz: _LL21: _tmp38=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL22: {enum Cyc_Absyn_Sign sn=_tmp38;
# 173
({union Cyc_Absyn_Cnst _tmp676=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp676;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _LL23: _tmp37=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp37;
# 176
({union Cyc_Absyn_Cnst _tmp677=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp677;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _LL25: _tmp36=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL26: {enum Cyc_Absyn_Sign sn=_tmp36;
_tmp35=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _LL27: _tmp35=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp35;
# 182
({union Cyc_Absyn_Cnst _tmp678=Cyc_Absyn_Int_c(sn,i);*c=_tmp678;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2 != 0){_LL2B: _tmp34=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2)->hd;_LL2C: {void*t1=_tmp34;
# 191
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=9U,({struct Cyc_Absyn_Exp*_tmp679=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp39->f1=_tmp679;});_tmp39;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3U: _LL29: if(i == 0){_LL2A: {
# 186
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp67A=({union Cyc_Absyn_Cnst _tmp535;(_tmp535.Null_c).tag=1U,(_tmp535.Null_c).val=0;_tmp535;});*c=_tmp67A;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}}else{goto _LL2D;}default: _LL2D: _LL2E:
# 193
 return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL20:;}}case 1U: _LL15: _LL16:
# 196
 if(topt != 0){
void*_tmp3A=Cyc_Absyn_compress(*topt);void*_stmttmp5=_tmp3A;void*_tmp3B=_stmttmp5;void*_tmp3C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->tag == 3U){_LL30: _tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->f1).ptr_atts).nullable;_LL31: {void*nbl=_tmp3C;
# 199
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp537;_tmp537.tag=0U,({struct _fat_ptr _tmp67B=({const char*_tmp40="Used NULL when expecting a value of type ";_tag_fat(_tmp40,sizeof(char),42U);});_tmp537.f1=_tmp67B;});_tmp537;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp536;_tmp536.tag=2U,_tmp536.f1=(void*)*topt;_tmp536;});void*_tmp3D[2U];_tmp3D[0]=& _tmp3E,_tmp3D[1]=& _tmp3F;({unsigned _tmp67C=e->loc;Cyc_Warn_err2(_tmp67C,_tag_fat(_tmp3D,sizeof(void*),2U));});});
return*topt;}}else{_LL32: _LL33:
 goto _LL2F;}_LL2F:;}{
# 204
struct Cyc_Absyn_PtrInfo _tmp41=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp41;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
return Cyc_Absyn_pointer_type(pi);}case 8U: _LL17: _tmp2B=(_tmp29.String_c).val;_LL18: {struct _fat_ptr s=_tmp2B;
# 209
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp2A=s;goto _LL1A;}default: _LL19: _tmp2A=(_tmp29.Wstring_c).val;_LL1A: {struct _fat_ptr s=_tmp2A;
# 212
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 216
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp67D=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp67D,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 221
void*_tmp42=({void*_tmp681=string_elt_typ;void*_tmp680=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp67F=Cyc_Absyn_const_tqual(0U);void*_tmp67E=
Cyc_Absyn_thin_bounds_exp(elen);
# 221
Cyc_Absyn_atb_type(_tmp681,_tmp680,_tmp67F,_tmp67E,Cyc_Absyn_true_type);});void*t=_tmp42;
# 223
if(topt == 0)
return t;{
void*_tmp43=Cyc_Absyn_compress(*topt);void*_stmttmp6=_tmp43;void*_tmp44=_stmttmp6;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 4U: _LL35: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp45;
# 229
return({void*_tmp684=string_elt_typ;struct Cyc_Absyn_Tqual _tmp683=tq;struct Cyc_Absyn_Exp*_tmp682=elen;Cyc_Absyn_array_type(_tmp684,_tmp683,_tmp682,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL37: _LL38:
# 234
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp687=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp686=loc;void*_tmp685=t;Cyc_Tcutil_silent_castable(_tmp687,_tmp686,_tmp685,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
return*topt;}
# 239
return t;default: _LL39: _LL3A:
 return t;}_LL34:;}}}}}_LL0:;}
# 246
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp46=*((void**)_check_null(b));void*_stmttmp7=_tmp46;void*_tmp47=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp48;struct Cyc_Absyn_Vardecl*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4A;struct Cyc_Absyn_Fndecl*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;struct _tuple1*_tmp4D;switch(*((int*)_tmp47)){case 0U: _LL1: _tmp4D=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp47)->f1;_LL2: {struct _tuple1*q=_tmp4D;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp538;_tmp538.tag=0U,({struct _fat_ptr _tmp688=({const char*_tmp50="unresolved binding in tcVar";_tag_fat(_tmp50,sizeof(char),28U);});_tmp538.f1=_tmp688;});_tmp538;});void*_tmp4E[1U];_tmp4E[0]=& _tmp4F;({unsigned _tmp689=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp689,_tag_fat(_tmp4E,sizeof(void*),1U));});});}case 1U: _LL3: _tmp4C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp47)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp4C;
# 252
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2U: _LL5: _tmp4B=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp47)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmp4B;
# 259
if(fd->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5U: _LL7: _tmp4A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp47)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp4A;
_tmp49=vd;goto _LLA;}case 4U: _LL9: _tmp49=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp47)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp49;
_tmp48=vd;goto _LLC;}default: _LLB: _tmp48=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp47)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp48;
# 265
if(te->allow_valueof){
void*_tmp51=Cyc_Absyn_compress(vd->type);void*_stmttmp8=_tmp51;void*_tmp52=_stmttmp8;void*_tmp53;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2 != 0){_LLE: _tmp53=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2)->hd;_LLF: {void*i=_tmp53;
({void*_tmp68A=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=39U,_tmp54->f1=i;_tmp54;});e->r=_tmp68A;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 270
return vd->type;}}_LL0:;}
# 274
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 281
void*_tmp55=fmt->r;void*_stmttmp9=_tmp55;void*_tmp56=_stmttmp9;struct _fat_ptr _tmp57;struct _fat_ptr _tmp58;switch(*((int*)_tmp56)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56)->f1).String_c).tag == 8){_LL1: _tmp58=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp58;
_tmp57=s;goto _LL4;}}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp57=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->f1).String_c).val;_LL4: {struct _fat_ptr s=_tmp57;
# 284
struct Cyc_List_List*_tmp59=type_getter(te,(struct _fat_ptr)s,isCproto,fmt->loc);struct Cyc_List_List*desc_types=_tmp59;
if(opt_args == 0)
return;{
struct Cyc_List_List*_tmp5A=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp5A;
# 289
for(0;desc_types != 0 && args != 0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp68C=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp68B=e;Cyc_Tcutil_coerce_arg(_tmp68C,_tmp68B,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53C;_tmp53C.tag=0U,({struct _fat_ptr _tmp68D=({const char*_tmp61="descriptor has type ";_tag_fat(_tmp61,sizeof(char),21U);});_tmp53C.f1=_tmp68D;});_tmp53C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53B;_tmp53B.tag=2U,_tmp53B.f1=(void*)t;_tmp53B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53A;_tmp53A.tag=0U,({
struct _fat_ptr _tmp68E=({const char*_tmp60=" but argument has type ";_tag_fat(_tmp60,sizeof(char),24U);});_tmp53A.f1=_tmp68E;});_tmp53A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp539;_tmp539.tag=3U,_tmp539.f1=(void*)e->topt;_tmp539;});void*_tmp5B[4U];_tmp5B[0]=& _tmp5C,_tmp5B[1]=& _tmp5D,_tmp5B[2]=& _tmp5E,_tmp5B[3]=& _tmp5F;({unsigned _tmp68F=e->loc;Cyc_Tcexp_err_and_explain(_tmp68F,_tag_fat(_tmp5B,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp690=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=(void*)arg_cnt,_tmp62->tl=*alias_arg_exps;_tmp62;});*alias_arg_exps=_tmp690;});
Cyc_Tcexp_check_consume(e->loc,t,e);}
# 303
if(desc_types != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53D;_tmp53D.tag=0U,({struct _fat_ptr _tmp691=({const char*_tmp65="too few arguments";_tag_fat(_tmp65,sizeof(char),18U);});_tmp53D.f1=_tmp691;});_tmp53D;});void*_tmp63[1U];_tmp63[0]=& _tmp64;({unsigned _tmp692=fmt->loc;Cyc_Warn_err2(_tmp692,_tag_fat(_tmp63,sizeof(void*),1U));});});
if(args != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53E;_tmp53E.tag=0U,({struct _fat_ptr _tmp693=({const char*_tmp68="too many arguments";_tag_fat(_tmp68,sizeof(char),19U);});_tmp53E.f1=_tmp693;});_tmp53E;});void*_tmp66[1U];_tmp66[0]=& _tmp67;({unsigned _tmp694=((struct Cyc_Absyn_Exp*)args->hd)->loc;Cyc_Warn_err2(_tmp694,_tag_fat(_tmp66,sizeof(void*),1U));});});
# 308
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}
# 311
return;}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 315
 if(opt_args == 0)
return;
{struct Cyc_List_List*_tmp69=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp69;for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)args->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt),(struct Cyc_Absyn_Exp*)args->hd);}}
# 321
return;}_LL0:;}
# 325
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 327
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp6A=p;switch(_tmp6A){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 331
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp540;_tmp540.tag=0U,({struct _fat_ptr _tmp695=({const char*_tmp6E="expecting numeric type but found ";_tag_fat(_tmp6E,sizeof(char),34U);});_tmp540.f1=_tmp695;});_tmp540;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53F;_tmp53F.tag=2U,_tmp53F.f1=(void*)t;_tmp53F;});void*_tmp6B[2U];_tmp6B[0]=& _tmp6C,_tmp6B[1]=& _tmp6D;({unsigned _tmp696=loc;Cyc_Warn_err2(_tmp696,_tag_fat(_tmp6B,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 335
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp542;_tmp542.tag=0U,({struct _fat_ptr _tmp697=({const char*_tmp72="expecting integral or * type but found ";_tag_fat(_tmp72,sizeof(char),40U);});_tmp542.f1=_tmp697;});_tmp542;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp71=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp541;_tmp541.tag=2U,_tmp541.f1=(void*)t;_tmp541;});void*_tmp6F[2U];_tmp6F[0]=& _tmp70,_tmp6F[1]=& _tmp71;({unsigned _tmp698=loc;Cyc_Warn_err2(_tmp698,_tag_fat(_tmp6F,sizeof(void*),2U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 340
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmp544;_tmp544.tag=0U,({struct _fat_ptr _tmp699=({const char*_tmp76="expecting integral type but found ";_tag_fat(_tmp76,sizeof(char),35U);});_tmp544.f1=_tmp699;});_tmp544;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp75=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp543;_tmp543.tag=2U,_tmp543.f1=(void*)t;_tmp543;});void*_tmp73[2U];_tmp73[0]=& _tmp74,_tmp73[1]=& _tmp75;({unsigned _tmp69A=loc;Cyc_Warn_err2(_tmp69A,_tag_fat(_tmp73,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 344
{void*_tmp77=t;void*_tmp78;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->tag == 3U){_LLE: _tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp78;
# 346
struct Cyc_Absyn_Exp*_tmp79=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp79;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp545;_tmp545.tag=0U,({struct _fat_ptr _tmp69B=({const char*_tmp7C="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp7C,sizeof(char),53U);});_tmp545.f1=_tmp69B;});_tmp545;});void*_tmp7A[1U];_tmp7A[0]=& _tmp7B;({unsigned _tmp69C=loc;Cyc_Warn_err2(_tmp69C,_tag_fat(_tmp7A,sizeof(void*),1U));});});
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp547;_tmp547.tag=0U,({struct _fat_ptr _tmp69D=({const char*_tmp80="numelts requires pointer type, not ";_tag_fat(_tmp80,sizeof(char),36U);});_tmp547.f1=_tmp69D;});_tmp547;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp546;_tmp546.tag=2U,_tmp546.f1=(void*)t;_tmp546;});void*_tmp7D[2U];_tmp7D[0]=& _tmp7E,_tmp7D[1]=& _tmp7F;({unsigned _tmp69E=loc;Cyc_Warn_err2(_tmp69E,_tag_fat(_tmp7D,sizeof(void*),2U));});});}_LLD:;}
# 352
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp82=({struct Cyc_Warn_String_Warn_Warg_struct _tmp548;_tmp548.tag=0U,({struct _fat_ptr _tmp69F=({const char*_tmp83="Non-unary primop";_tag_fat(_tmp83,sizeof(char),17U);});_tmp548.f1=_tmp69F;});_tmp548;});void*_tmp81[1U];_tmp81[0]=& _tmp82;({unsigned _tmp6A0=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp6A0,_tag_fat(_tmp81,sizeof(void*),1U));});});}_LL0:;}
# 359
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp84=(void*)_check_null(e1->topt);void*t1=_tmp84;
void*_tmp85=(void*)_check_null(e2->topt);void*t2=_tmp85;
void*_tmp86=Cyc_Tcutil_max_arithmetic_type(t1,t2);void*new_typ=_tmp86;
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,Cyc_Absyn_No_coercion);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,Cyc_Absyn_No_coercion);
return new_typ;}
# 369
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 372
if(!checker(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54B;_tmp54B.tag=0U,({struct _fat_ptr _tmp6A1=({const char*_tmp8C="type ";_tag_fat(_tmp8C,sizeof(char),6U);});_tmp54B.f1=_tmp6A1;});_tmp54B;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp89=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp54A;_tmp54A.tag=3U,_tmp54A.f1=(void*)e1->topt;_tmp54A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp549;_tmp549.tag=0U,({struct _fat_ptr _tmp6A2=({const char*_tmp8B=" cannot be used here";_tag_fat(_tmp8B,sizeof(char),21U);});_tmp549.f1=_tmp6A2;});_tmp549;});void*_tmp87[3U];_tmp87[0]=& _tmp88,_tmp87[1]=& _tmp89,_tmp87[2]=& _tmp8A;({struct Cyc_Tcenv_Tenv*_tmp6A4=te;unsigned _tmp6A3=e1->loc;Cyc_Tcexp_expr_err(_tmp6A4,_tmp6A3,0,_tag_fat(_tmp87,sizeof(void*),3U));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp8E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54E;_tmp54E.tag=0U,({struct _fat_ptr _tmp6A5=({const char*_tmp92="type ";_tag_fat(_tmp92,sizeof(char),6U);});_tmp54E.f1=_tmp6A5;});_tmp54E;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp54D;_tmp54D.tag=3U,_tmp54D.f1=(void*)e2->topt;_tmp54D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54C;_tmp54C.tag=0U,({struct _fat_ptr _tmp6A6=({const char*_tmp91=" cannot be used here";_tag_fat(_tmp91,sizeof(char),21U);});_tmp54C.f1=_tmp6A6;});_tmp54C;});void*_tmp8D[3U];_tmp8D[0]=& _tmp8E,_tmp8D[1]=& _tmp8F,_tmp8D[2]=& _tmp90;({struct Cyc_Tcenv_Tenv*_tmp6A8=te;unsigned _tmp6A7=e2->loc;Cyc_Tcexp_expr_err(_tmp6A8,_tmp6A7,0,_tag_fat(_tmp8D,sizeof(void*),3U));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 379
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
void*_tmp93=t1;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;struct Cyc_Absyn_Tqual _tmp95;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->tag == 3U){_LL1: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_type;_tmp95=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_tq;_tmp96=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).rgn;_tmp97=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).nullable;_tmp98=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).bounds;_tmp99=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).zero_term;_LL2: {void*et=_tmp94;struct Cyc_Absyn_Tqual tq=_tmp95;void*r=_tmp96;void*n=_tmp97;void*b=_tmp98;void*zt=_tmp99;
# 384
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54F;_tmp54F.tag=0U,({struct _fat_ptr _tmp6A9=({const char*_tmp9C="can't do arithmetic on abstract pointer type";_tag_fat(_tmp9C,sizeof(char),45U);});_tmp54F.f1=_tmp6A9;});_tmp54F;});void*_tmp9A[1U];_tmp9A[0]=& _tmp9B;({unsigned _tmp6AA=e1->loc;Cyc_Warn_err2(_tmp6AA,_tag_fat(_tmp9A,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp550;_tmp550.tag=0U,({struct _fat_ptr _tmp6AB=({const char*_tmp9F="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmp9F,sizeof(char),49U);});_tmp550.f1=_tmp6AB;});_tmp550;});void*_tmp9D[1U];_tmp9D[0]=& _tmp9E;({unsigned _tmp6AC=e1->loc;Cyc_Warn_err2(_tmp6AC,_tag_fat(_tmp9D,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp552;_tmp552.tag=0U,({struct _fat_ptr _tmp6AD=({const char*_tmpA3="expecting int but found ";_tag_fat(_tmpA3,sizeof(char),25U);});_tmp552.f1=_tmp6AD;});_tmp552;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp551;_tmp551.tag=2U,_tmp551.f1=(void*)t2;_tmp551;});void*_tmpA0[2U];_tmpA0[0]=& _tmpA1,_tmpA0[1]=& _tmpA2;({unsigned _tmp6AE=e2->loc;Cyc_Warn_err2(_tmp6AE,_tag_fat(_tmpA0,sizeof(void*),2U));});});{
struct Cyc_Absyn_Exp*_tmpA4=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmpA4;
if(eopt == 0)
return t1;
# 395
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmpA5=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmpA5;struct _tuple16 _tmpA6=_stmttmpA;int _tmpA8;unsigned _tmpA7;_LL6: _tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_LL7: {unsigned i=_tmpA7;int known=_tmpA8;
if(known && i == (unsigned)1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp553;_tmp553.tag=0U,({struct _fat_ptr _tmp6AF=({const char*_tmpAB="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpAB,sizeof(char),70U);});_tmp553.f1=_tmp6AF;});_tmp553;});void*_tmpA9[1U];_tmpA9[0]=& _tmpAA;({unsigned _tmp6B0=e1->loc;Cyc_Warn_warn2(_tmp6B0,_tag_fat(_tmpA9,sizeof(void*),1U));});});}}{
# 406
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAC=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=3U,(_tmpAD->f1).elt_type=et,(_tmpAD->f1).elt_tq=tq,((_tmpAD->f1).ptr_atts).rgn=r,((_tmpAD->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpAD->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpAD->f1).ptr_atts).zero_term=zt,((_tmpAD->f1).ptr_atts).ptrloc=0;_tmpAD;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpAC;
# 409
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,Cyc_Absyn_Other_coercion);
return(void*)new_t1;}}}}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 416
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp559;_tmp559.tag=0U,({struct _fat_ptr _tmp6B1=({const char*_tmpB8="pointer arithmetic on values of different ";_tag_fat(_tmpB8,sizeof(char),43U);});_tmp559.f1=_tmp6B1;});_tmp559;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp558;_tmp558.tag=0U,({
struct _fat_ptr _tmp6B2=({const char*_tmpB7="types (";_tag_fat(_tmpB7,sizeof(char),8U);});_tmp558.f1=_tmp6B2;});_tmp558;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp557;_tmp557.tag=2U,_tmp557.f1=(void*)t1;_tmp557;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp556;_tmp556.tag=0U,({struct _fat_ptr _tmp6B3=({const char*_tmpB6=" != ";_tag_fat(_tmpB6,sizeof(char),5U);});_tmp556.f1=_tmp6B3;});_tmp556;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp555;_tmp555.tag=2U,_tmp555.f1=(void*)t2;_tmp555;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp554;_tmp554.tag=0U,({struct _fat_ptr _tmp6B4=({const char*_tmpB5=")";_tag_fat(_tmpB5,sizeof(char),2U);});_tmp554.f1=_tmp6B4;});_tmp554;});void*_tmpAE[6U];_tmpAE[0]=& _tmpAF,_tmpAE[1]=& _tmpB0,_tmpAE[2]=& _tmpB1,_tmpAE[3]=& _tmpB2,_tmpAE[4]=& _tmpB3,_tmpAE[5]=& _tmpB4;({unsigned _tmp6B5=e1->loc;Cyc_Tcexp_err_and_explain(_tmp6B5,_tag_fat(_tmpAE,sizeof(void*),6U));});});
return Cyc_Absyn_sint_type;}
# 428
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp6B6=t1_elt;Cyc_Unify_unify(_tmp6B6,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55F;_tmp55F.tag=0U,({struct _fat_ptr _tmp6B7=({const char*_tmpC3="pointer arithmetic on values of different ";_tag_fat(_tmpC3,sizeof(char),43U);});_tmp55F.f1=_tmp6B7;});_tmp55F;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55E;_tmp55E.tag=0U,({
struct _fat_ptr _tmp6B8=({const char*_tmpC2="types(";_tag_fat(_tmpC2,sizeof(char),7U);});_tmp55E.f1=_tmp6B8;});_tmp55E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55D;_tmp55D.tag=2U,_tmp55D.f1=(void*)t1;_tmp55D;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55C;_tmp55C.tag=0U,({struct _fat_ptr _tmp6B9=({const char*_tmpC1=" != ";_tag_fat(_tmpC1,sizeof(char),5U);});_tmp55C.f1=_tmp6B9;});_tmp55C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55B;_tmp55B.tag=2U,_tmp55B.f1=(void*)t2;_tmp55B;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55A;_tmp55A.tag=0U,({struct _fat_ptr _tmp6BA=({const char*_tmpC0=")";_tag_fat(_tmpC0,sizeof(char),2U);});_tmp55A.f1=_tmp6BA;});_tmp55A;});void*_tmpB9[6U];_tmpB9[0]=& _tmpBA,_tmpB9[1]=& _tmpBB,_tmpB9[2]=& _tmpBC,_tmpB9[3]=& _tmpBD,_tmpB9[4]=& _tmpBE,_tmpB9[5]=& _tmpBF;({unsigned _tmp6BB=e1->loc;Cyc_Tcexp_err_and_explain(_tmp6BB,_tag_fat(_tmpB9,sizeof(void*),6U));});});
# 433
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp560;_tmp560.tag=0U,({struct _fat_ptr _tmp6BC=({const char*_tmpC6="coercing fat pointer to thin pointer for subtraction";_tag_fat(_tmpC6,sizeof(char),53U);});_tmp560.f1=_tmp6BC;});_tmp560;});void*_tmpC4[1U];_tmpC4[0]=& _tmpC5;({unsigned _tmp6BD=e1->loc;Cyc_Warn_warn2(_tmp6BD,_tag_fat(_tmpC4,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp6C1=e1;Cyc_Tcutil_unchecked_cast(_tmp6C1,({void*_tmp6C0=t1_elt;void*_tmp6BF=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp6BE=
Cyc_Absyn_empty_tqual(0U);
# 434
Cyc_Absyn_star_type(_tmp6C0,_tmp6BF,_tmp6BE,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 437
return Cyc_Absyn_sint_type;}
# 439
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp561;_tmp561.tag=0U,({struct _fat_ptr _tmp6C2=({const char*_tmpC9="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpC9,sizeof(char),50U);});_tmp561.f1=_tmp6C2;});_tmp561;});void*_tmpC7[1U];_tmpC7[0]=& _tmpC8;({unsigned _tmp6C3=e1->loc;Cyc_Warn_err2(_tmp6C3,_tag_fat(_tmpC7,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp562;_tmp562.tag=0U,({struct _fat_ptr _tmp6C4=({const char*_tmpCC="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpCC,sizeof(char),54U);});_tmp562.f1=_tmp6C4;});_tmp562;});void*_tmpCA[1U];_tmpCA[0]=& _tmpCB;({unsigned _tmp6C5=e1->loc;Cyc_Warn_err2(_tmp6C5,_tag_fat(_tmpCA,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp566;_tmp566.tag=0U,({struct _fat_ptr _tmp6C6=({const char*_tmpD3="expecting ";_tag_fat(_tmpD3,sizeof(char),11U);});_tmp566.f1=_tmp6C6;});_tmp566;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp565;_tmp565.tag=2U,_tmp565.f1=(void*)t1;_tmp565;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp564;_tmp564.tag=0U,({struct _fat_ptr _tmp6C7=({const char*_tmpD2=" or int but found ";_tag_fat(_tmpD2,sizeof(char),19U);});_tmp564.f1=_tmp6C7;});_tmp564;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp563;_tmp563.tag=2U,_tmp563.f1=(void*)t2;_tmp563;});void*_tmpCD[4U];_tmpCD[0]=& _tmpCE,_tmpCD[1]=& _tmpCF,_tmpCD[2]=& _tmpD0,_tmpCD[3]=& _tmpD1;({unsigned _tmp6C8=e2->loc;Cyc_Tcexp_err_and_explain(_tmp6C8,_tag_fat(_tmpCD,sizeof(void*),4U));});});
return t1;}
# 448
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp6C9=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp6C9,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp567;_tmp567.tag=0U,({struct _fat_ptr _tmp6CA=({const char*_tmpD6="coercing fat pointer to thin pointer for subtraction";_tag_fat(_tmpD6,sizeof(char),53U);});_tmp567.f1=_tmp6CA;});_tmp567;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD5;({unsigned _tmp6CB=e1->loc;Cyc_Warn_warn2(_tmp6CB,_tag_fat(_tmpD4,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp6CF=e2;Cyc_Tcutil_unchecked_cast(_tmp6CF,({void*_tmp6CE=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp6CD=
Cyc_Tcutil_pointer_region(t2);
# 453
struct Cyc_Absyn_Tqual _tmp6CC=
# 455
Cyc_Absyn_empty_tqual(0U);
# 453
Cyc_Absyn_star_type(_tmp6CE,_tmp6CD,_tmp6CC,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 458
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp568;_tmp568.tag=0U,({struct _fat_ptr _tmp6D0=({const char*_tmpD9="thin pointer subtraction!";_tag_fat(_tmpD9,sizeof(char),26U);});_tmp568.f1=_tmp6D0;});_tmp568;});void*_tmpD7[1U];_tmpD7[0]=& _tmpD8;({unsigned _tmp6D1=e1->loc;Cyc_Warn_warn2(_tmp6D1,_tag_fat(_tmpD7,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;}
# 461
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp569;_tmp569.tag=0U,({struct _fat_ptr _tmp6D2=({const char*_tmpDC="coercing thin pointer to integer for subtraction";_tag_fat(_tmpDC,sizeof(char),49U);});_tmp569.f1=_tmp6D2;});_tmp569;});void*_tmpDA[1U];_tmpDA[0]=& _tmpDB;({unsigned _tmp6D3=e1->loc;Cyc_Warn_warn2(_tmp6D3,_tag_fat(_tmpDA,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 464
if(Cyc_Tcutil_is_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56A;_tmp56A.tag=0U,({struct _fat_ptr _tmp6D4=({const char*_tmpDF="coercing pointer to integer for subtraction";_tag_fat(_tmpDF,sizeof(char),44U);});_tmp56A.f1=_tmp6D4;});_tmp56A;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDE;({unsigned _tmp6D5=e1->loc;Cyc_Warn_warn2(_tmp6D5,_tag_fat(_tmpDD,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 469
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 472
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 480
if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind ||({
void*_tmp6D6=t1;Cyc_Unify_unify(_tmp6D6,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 485
if(({struct Cyc_RgnOrder_RgnPO*_tmp6D9=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6D8=loc;void*_tmp6D7=t2;Cyc_Tcutil_silent_castable(_tmp6D9,_tmp6D8,_tmp6D7,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 489
if(({struct Cyc_RgnOrder_RgnPO*_tmp6DC=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6DB=loc;void*_tmp6DA=t1;Cyc_Tcutil_silent_castable(_tmp6DC,_tmp6DB,_tmp6DA,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 493
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 498
{struct _tuple0 _tmpE0=({struct _tuple0 _tmp56B;({void*_tmp6DE=Cyc_Absyn_compress(t1);_tmp56B.f1=_tmp6DE;}),({void*_tmp6DD=Cyc_Absyn_compress(t2);_tmp56B.f2=_tmp6DD;});_tmp56B;});struct _tuple0 _stmttmpB=_tmpE0;struct _tuple0 _tmpE1=_stmttmpB;void*_tmpE3;void*_tmpE2;switch(*((int*)_tmpE1.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1.f2)->tag == 3U){_LL1: _tmpE2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1.f1)->f1).elt_type;_tmpE3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1.f2)->f1).elt_type;_LL2: {void*t1a=_tmpE2;void*t2a=_tmpE3;
# 500
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE1.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE1.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE1.f2)->f1)->tag == 3U){_LL3: _LL4:
# 504
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 508
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56F;_tmp56F.tag=0U,({struct _fat_ptr _tmp6DF=({const char*_tmpEA="comparison not allowed between ";_tag_fat(_tmpEA,sizeof(char),32U);});_tmp56F.f1=_tmp6DF;});_tmp56F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56E;_tmp56E.tag=2U,_tmp56E.f1=(void*)t1;_tmp56E;});struct Cyc_Warn_String_Warn_Warg_struct _tmpE7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56D;_tmp56D.tag=0U,({struct _fat_ptr _tmp6E0=({const char*_tmpE9=" and ";_tag_fat(_tmpE9,sizeof(char),6U);});_tmp56D.f1=_tmp6E0;});_tmp56D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56C;_tmp56C.tag=2U,_tmp56C.f1=(void*)t2;_tmp56C;});void*_tmpE4[4U];_tmpE4[0]=& _tmpE5,_tmpE4[1]=& _tmpE6,_tmpE4[2]=& _tmpE7,_tmpE4[3]=& _tmpE8;({unsigned _tmp6E1=loc;Cyc_Tcexp_err_and_explain(_tmp6E1,_tag_fat(_tmpE4,sizeof(void*),4U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 514
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 516
enum Cyc_Absyn_Primop _tmpEB=p;switch(_tmpEB){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 520
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 523
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL15: _LL16:
# 532
 goto _LL18;case Cyc_Absyn_Neq: _LL17: _LL18:
 goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A:
 goto _LL1C;case Cyc_Absyn_Lt: _LL1B: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL1F: _LL20:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);default: _LL21: _LL22:
# 539
({struct Cyc_Warn_String_Warn_Warg_struct _tmpED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp570;_tmp570.tag=0U,({struct _fat_ptr _tmp6E2=({const char*_tmpEE="bad binary primop";_tag_fat(_tmpEE,sizeof(char),18U);});_tmp570.f1=_tmp6E2;});_tmp570;});void*_tmpEC[1U];_tmpEC[0]=& _tmpED;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpEC,sizeof(void*),1U));});}_LL0:;}
# 543
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 551
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpEF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpEF;
void*_tmpF0=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpF0;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp572;_tmp572.tag=0U,({struct _fat_ptr _tmp6E3=({const char*_tmpF4="expecting numeric type but found ";_tag_fat(_tmpF4,sizeof(char),34U);});_tmp572.f1=_tmp6E3;});_tmp572;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp571;_tmp571.tag=2U,_tmp571.f1=(void*)t;_tmp571;});void*_tmpF1[2U];_tmpF1[0]=& _tmpF2,_tmpF1[1]=& _tmpF3;({unsigned _tmp6E4=e->loc;Cyc_Warn_err2(_tmp6E4,_tag_fat(_tmpF1,sizeof(void*),2U));});});
return t;}
# 558
{struct Cyc_List_List*_tmpF5=es;struct Cyc_List_List*es2=_tmpF5;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _tmpF6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpF6;int _tmpF7=_stmttmpC;switch(_tmpF7){case 0U: _LL1: _LL2:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp573;_tmp573.tag=0U,({struct _fat_ptr _tmp6E5=({const char*_tmpFA="primitive operator has 0 arguments";_tag_fat(_tmpFA,sizeof(char),35U);});_tmp573.f1=_tmp6E5;});_tmp573;});void*_tmpF8[1U];_tmpF8[0]=& _tmpF9;({struct Cyc_Tcenv_Tenv*_tmp6E8=te;unsigned _tmp6E7=loc;void**_tmp6E6=topt;Cyc_Tcexp_expr_err(_tmp6E8,_tmp6E7,_tmp6E6,_tag_fat(_tmpF8,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);case 2U: _LL5: _LL6:
 return({struct Cyc_Tcenv_Tenv*_tmp6ED=te;unsigned _tmp6EC=loc;void**_tmp6EB=topt;enum Cyc_Absyn_Primop _tmp6EA=p;struct Cyc_Absyn_Exp*_tmp6E9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp6ED,_tmp6EC,_tmp6EB,_tmp6EA,_tmp6E9,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpFC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp574;_tmp574.tag=0U,({struct _fat_ptr _tmp6EE=({const char*_tmpFD="primitive operator has > 2 arguments";_tag_fat(_tmpFD,sizeof(char),37U);});_tmp574.f1=_tmp6EE;});_tmp574;});void*_tmpFB[1U];_tmpFB[0]=& _tmpFC;({struct Cyc_Tcenv_Tenv*_tmp6F1=te;unsigned _tmp6F0=loc;void**_tmp6EF=topt;Cyc_Tcexp_expr_err(_tmp6F1,_tmp6F0,_tmp6EF,_tag_fat(_tmpFB,sizeof(void*),1U));});});}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 568
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
t=Cyc_Absyn_compress(t);{
void*_tmpFE=t;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Tqual _tmp101;void*_tmp100;struct Cyc_List_List*_tmp102;struct Cyc_Absyn_Datatypefield*_tmp103;struct Cyc_Absyn_Aggrdecl*_tmp104;switch(*((int*)_tmpFE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp104=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp104;
# 572
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp106=({struct Cyc_Warn_String_Warn_Warg_struct _tmp576;_tmp576.tag=0U,({struct _fat_ptr _tmp6F2=({const char*_tmp108="attempt to write abstract ";_tag_fat(_tmp108,sizeof(char),27U);});_tmp576.f1=_tmp6F2;});_tmp576;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp107=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp575;_tmp575.tag=6U,_tmp575.f1=ad;_tmp575;});void*_tmp105[2U];_tmp105[0]=& _tmp106,_tmp105[1]=& _tmp107;({unsigned _tmp6F3=loc;Cyc_Warn_err2(_tmp6F3,_tag_fat(_tmp105,sizeof(void*),2U));});});
return 0;}
# 576
_tmp102=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmp103=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFE)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmp103;
# 588
{struct Cyc_List_List*_tmp10E=df->typs;struct Cyc_List_List*fs=_tmp10E;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp10F=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmp10F;struct _tuple17*_tmp110=_stmttmpD;void*_tmp112;struct Cyc_Absyn_Tqual _tmp111;_LLE: _tmp111=_tmp110->f1;_tmp112=_tmp110->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp111;void*t=_tmp112;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp114=({struct Cyc_Warn_String_Warn_Warg_struct _tmp579;_tmp579.tag=0U,({struct _fat_ptr _tmp6F4=({const char*_tmp118="attempt to overwrite a datatype field (";_tag_fat(_tmp118,sizeof(char),40U);});_tmp579.f1=_tmp6F4;});_tmp579;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp115=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp578;_tmp578.tag=1U,_tmp578.f1=df->name;_tmp578;});struct Cyc_Warn_String_Warn_Warg_struct _tmp116=({struct Cyc_Warn_String_Warn_Warg_struct _tmp577;_tmp577.tag=0U,({
struct _fat_ptr _tmp6F5=({const char*_tmp117=") with a const member";_tag_fat(_tmp117,sizeof(char),22U);});_tmp577.f1=_tmp6F5;});_tmp577;});void*_tmp113[3U];_tmp113[0]=& _tmp114,_tmp113[1]=& _tmp115,_tmp113[2]=& _tmp116;({unsigned _tmp6F6=loc;Cyc_Warn_err2(_tmp6F6,_tag_fat(_tmp113,sizeof(void*),3U));});});
return 0;}
# 595
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 597
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmp102=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFE)->f2;_LL4: {struct Cyc_List_List*fs=_tmp102;
# 578
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp109=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp109;
if((f->tq).real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57B;_tmp57B.tag=0U,({struct _fat_ptr _tmp6F7=({const char*_tmp10D="attempt to overwrite an aggregate with const member ";_tag_fat(_tmp10D,sizeof(char),53U);});_tmp57B.f1=_tmp6F7;});_tmp57B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57A;_tmp57A.tag=0U,_tmp57A.f1=*f->name;_tmp57A;});void*_tmp10A[2U];_tmp10A[0]=& _tmp10B,_tmp10A[1]=& _tmp10C;({unsigned _tmp6F8=loc;Cyc_Warn_err2(_tmp6F8,_tag_fat(_tmp10A,sizeof(void*),2U));});});
return 0;}
# 584
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 586
return 1;}case 4U: _LL7: _tmp100=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFE)->f1).elt_type;_tmp101=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFE)->f1).tq;_LL8: {void*elt_type=_tmp100;struct Cyc_Absyn_Tqual tq=_tmp101;
# 599
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57C;_tmp57C.tag=0U,({struct _fat_ptr _tmp6F9=({const char*_tmp11B="attempt to overwrite a const array";_tag_fat(_tmp11B,sizeof(char),35U);});_tmp57C.f1=_tmp6F9;});_tmp57C;});void*_tmp119[1U];_tmp119[0]=& _tmp11A;({unsigned _tmp6FA=loc;Cyc_Warn_err2(_tmp6FA,_tag_fat(_tmp119,sizeof(void*),1U));});});
return 0;}
# 603
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmpFF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFE)->f1;_LLA: {struct Cyc_List_List*fs=_tmpFF;
# 605
for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp11C=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp11C;struct _tuple17*_tmp11D=_stmttmpE;void*_tmp11F;struct Cyc_Absyn_Tqual _tmp11E;_LL11: _tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmp11E;void*t=_tmp11F;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp121=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57D;_tmp57D.tag=0U,({struct _fat_ptr _tmp6FB=({const char*_tmp122="attempt to overwrite a tuple field with a const member";_tag_fat(_tmp122,sizeof(char),55U);});_tmp57D.f1=_tmp6FB;});_tmp57D;});void*_tmp120[1U];_tmp120[0]=& _tmp121;({unsigned _tmp6FC=loc;Cyc_Warn_err2(_tmp6FC,_tag_fat(_tmp120,sizeof(void*),1U));});});
return 0;}
# 611
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}
# 613
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}}
# 621
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 624
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp123=e->r;void*_stmttmpF=_tmp123;void*_tmp124=_stmttmpF;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct _fat_ptr*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct _fat_ptr*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Vardecl*_tmp12E;struct Cyc_Absyn_Vardecl*_tmp12F;struct Cyc_Absyn_Vardecl*_tmp130;struct Cyc_Absyn_Vardecl*_tmp131;switch(*((int*)_tmp124)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)){case 3U: _LL1: _tmp131=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp131;
_tmp130=vd;goto _LL4;}case 4U: _LL3: _tmp130=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp130;
_tmp12F=vd;goto _LL6;}case 5U: _LL5: _tmp12F=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp12F;
_tmp12E=vd;goto _LL8;}case 1U: _LL7: _tmp12E=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp124)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp12E;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23U: _LL9: _tmp12C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_tmp12D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp124)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp12C;struct Cyc_Absyn_Exp*e2=_tmp12D;
# 631
{void*_tmp132=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp132;void*_tmp133=_stmttmp10;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_Tqual _tmp135;struct Cyc_Absyn_Tqual _tmp136;switch(*((int*)_tmp133)){case 3U: _LL18: _tmp136=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_tq;_LL19: {struct Cyc_Absyn_Tqual tq=_tmp136;
_tmp135=tq;goto _LL1B;}case 4U: _LL1A: _tmp135=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp135;
if(!tq.real_const)return;goto _LL17;}case 6U: _LL1C: _tmp134=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp133)->f1;_LL1D: {struct Cyc_List_List*ts=_tmp134;
# 635
struct _tuple16 _tmp137=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp11=_tmp137;struct _tuple16 _tmp138=_stmttmp11;int _tmp13A;unsigned _tmp139;_LL21: _tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;_LL22: {unsigned i=_tmp139;int known=_tmp13A;
if(!known){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57E;_tmp57E.tag=0U,({struct _fat_ptr _tmp6FD=({const char*_tmp13D="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp13D,sizeof(char),47U);});_tmp57E.f1=_tmp6FD;});_tmp57E;});void*_tmp13B[1U];_tmp13B[0]=& _tmp13C;({unsigned _tmp6FE=e->loc;Cyc_Warn_err2(_tmp6FE,_tag_fat(_tmp13B,sizeof(void*),1U));});});
return;}
# 640
{struct _handler_cons _tmp13E;_push_handler(& _tmp13E);{int _tmp140=0;if(setjmp(_tmp13E.handler))_tmp140=1;if(!_tmp140){
{struct _tuple17*_tmp141=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i);struct _tuple17*_stmttmp12=_tmp141;struct _tuple17*_tmp142=_stmttmp12;struct Cyc_Absyn_Tqual _tmp143;_LL24: _tmp143=_tmp142->f1;_LL25: {struct Cyc_Absyn_Tqual tq=_tmp143;
if(!tq.real_const){_npop_handler(0U);return;}}}
# 641
;_pop_handler();}else{void*_tmp13F=(void*)Cyc_Core_get_exn_thrown();void*_tmp144=_tmp13F;void*_tmp145;if(((struct Cyc_List_Nth_exn_struct*)_tmp144)->tag == Cyc_List_Nth){_LL27: _LL28:
# 643
 return;}else{_LL29: _tmp145=_tmp144;_LL2A: {void*exn=_tmp145;(int)_rethrow(exn);}}_LL26:;}}}
goto _LL17;}}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 647
goto _LL0;}case 21U: _LLB: _tmp12A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_tmp12B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp124)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp12A;struct _fat_ptr*f=_tmp12B;
# 649
{void*_tmp146=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp146;void*_tmp147=_stmttmp13;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_Aggrdecl**_tmp149;switch(*((int*)_tmp147)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp149=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1).KnownAggr).val;_LL2D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp149;
# 651
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp148=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp147)->f2;_LL2F: {struct Cyc_List_List*fs=_tmp148;
# 656
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 661
goto _LL0;}case 22U: _LLD: _tmp128=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_tmp129=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp124)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp128;struct _fat_ptr*f=_tmp129;
# 663
{void*_tmp14A=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp14A;void*_tmp14B=_stmttmp14;struct Cyc_Absyn_Tqual _tmp14D;void*_tmp14C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14B)->tag == 3U){_LL33: _tmp14C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14B)->f1).elt_type;_tmp14D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14B)->f1).elt_tq;_LL34: {void*elt_typ=_tmp14C;struct Cyc_Absyn_Tqual tq=_tmp14D;
# 665
if(!tq.real_const){
void*_tmp14E=Cyc_Absyn_compress(elt_typ);void*_stmttmp15=_tmp14E;void*_tmp14F=_stmttmp15;struct Cyc_List_List*_tmp150;struct Cyc_Absyn_Aggrdecl**_tmp151;switch(*((int*)_tmp14F)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14F)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14F)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp151=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14F)->f1)->f1).KnownAggr).val;_LL39: {struct Cyc_Absyn_Aggrdecl**adp=_tmp151;
# 668
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp150=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14F)->f2;_LL3B: {struct Cyc_List_List*fs=_tmp150;
# 673
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 679
goto _LL32;}}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 682
goto _LL0;}case 20U: _LLF: _tmp127=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp127;
# 684
{void*_tmp152=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp152;void*_tmp153=_stmttmp16;struct Cyc_Absyn_Tqual _tmp154;struct Cyc_Absyn_Tqual _tmp155;switch(*((int*)_tmp153)){case 3U: _LL3F: _tmp155=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp153)->f1).elt_tq;_LL40: {struct Cyc_Absyn_Tqual tq=_tmp155;
_tmp154=tq;goto _LL42;}case 4U: _LL41: _tmp154=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp153)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp154;
if(!tq.real_const)return;goto _LL3E;}default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 689
goto _LL0;}case 12U: _LL11: _tmp126=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp126;
_tmp125=e1;goto _LL14;}case 13U: _LL13: _tmp125=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp125;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 694
({struct Cyc_Warn_String_Warn_Warg_struct _tmp157=({struct Cyc_Warn_String_Warn_Warg_struct _tmp580;_tmp580.tag=0U,({struct _fat_ptr _tmp6FF=({const char*_tmp159="attempt to write a const location: ";_tag_fat(_tmp159,sizeof(char),36U);});_tmp580.f1=_tmp6FF;});_tmp580;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp158=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp57F;_tmp57F.tag=4U,_tmp57F.f1=e;_tmp57F;});void*_tmp156[2U];_tmp156[0]=& _tmp157,_tmp156[1]=& _tmp158;({unsigned _tmp700=e->loc;Cyc_Warn_err2(_tmp700,_tag_fat(_tmp156,sizeof(void*),2U));});});}
# 697
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 700
({struct Cyc_Tcenv_Tenv*_tmp701=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp701,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp15B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0U,({struct _fat_ptr _tmp702=({const char*_tmp15C="increment/decrement of non-lvalue";_tag_fat(_tmp15C,sizeof(char),34U);});_tmp581.f1=_tmp702;});_tmp581;});void*_tmp15A[1U];_tmp15A[0]=& _tmp15B;({struct Cyc_Tcenv_Tenv*_tmp705=te;unsigned _tmp704=loc;void**_tmp703=topt;Cyc_Tcexp_expr_err(_tmp705,_tmp704,_tmp703,_tag_fat(_tmp15A,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 706
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp15D=0U;({unsigned _tmp707=e->loc;struct _fat_ptr _tmp706=({const char*_tmp15E="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp15E,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp707,_tmp706,_tag_fat(_tmp15D,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp15F=0U;({unsigned _tmp709=e->loc;struct _fat_ptr _tmp708=({const char*_tmp160="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp160,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp709,_tmp708,_tag_fat(_tmp15F,sizeof(void*),0U));});});}else{
# 715
({struct Cyc_Warn_String_Warn_Warg_struct _tmp162=({struct Cyc_Warn_String_Warn_Warg_struct _tmp583;_tmp583.tag=0U,({struct _fat_ptr _tmp70A=({const char*_tmp164="expecting arithmetic or ? type but found ";_tag_fat(_tmp164,sizeof(char),42U);});_tmp583.f1=_tmp70A;});_tmp583;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp163=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp582;_tmp582.tag=2U,_tmp582.f1=(void*)t;_tmp582;});void*_tmp161[2U];_tmp161[0]=& _tmp162,_tmp161[1]=& _tmp163;({unsigned _tmp70B=e->loc;Cyc_Warn_err2(_tmp70B,_tag_fat(_tmp161,sizeof(void*),2U));});});}}
# 717
return t;}}
# 721
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 723
({struct Cyc_Tcenv_Tenv*_tmp70D=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp70C=e1;Cyc_Tcexp_tcTest(_tmp70D,_tmp70C,({const char*_tmp165="conditional expression";_tag_fat(_tmp165,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*_tmp166=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tako:& Cyc_Tcutil_tmko;struct Cyc_Core_Opt*ko=_tmp166;
void*_tmp167=({struct Cyc_Core_Opt*_tmp70E=ko;Cyc_Absyn_new_evar(_tmp70E,Cyc_Tcenv_lookup_opt_type_vars(te));});void*t=_tmp167;
struct Cyc_List_List _tmp168=({struct Cyc_List_List _tmp589;_tmp589.hd=e3,_tmp589.tl=0;_tmp589;});struct Cyc_List_List l1=_tmp168;
struct Cyc_List_List _tmp169=({struct Cyc_List_List _tmp588;_tmp588.hd=e2,_tmp588.tl=& l1;_tmp588;});struct Cyc_List_List l2=_tmp169;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp70F=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp70F,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp587;_tmp587.tag=0U,({struct _fat_ptr _tmp710=({const char*_tmp170="conditional clause types do not match: ";_tag_fat(_tmp170,sizeof(char),40U);});_tmp587.f1=_tmp710;});_tmp587;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp16C=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp586;_tmp586.tag=3U,_tmp586.f1=(void*)e2->topt;_tmp586;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp585;_tmp585.tag=0U,({
struct _fat_ptr _tmp711=({const char*_tmp16F=" != ";_tag_fat(_tmp16F,sizeof(char),5U);});_tmp585.f1=_tmp711;});_tmp585;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp584;_tmp584.tag=3U,_tmp584.f1=(void*)e3->topt;_tmp584;});void*_tmp16A[4U];_tmp16A[0]=& _tmp16B,_tmp16A[1]=& _tmp16C,_tmp16A[2]=& _tmp16D,_tmp16A[3]=& _tmp16E;({unsigned _tmp712=loc;Cyc_Tcexp_err_and_explain(_tmp712,_tag_fat(_tmp16A,sizeof(void*),4U));});});
return t;}}
# 737
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 739
({struct Cyc_Tcenv_Tenv*_tmp714=te;struct Cyc_Absyn_Exp*_tmp713=e1;Cyc_Tcexp_tcTest(_tmp714,_tmp713,({const char*_tmp171="logical-and expression";_tag_fat(_tmp171,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp716=te;struct Cyc_Absyn_Exp*_tmp715=e2;Cyc_Tcexp_tcTest(_tmp716,_tmp715,({const char*_tmp172="logical-and expression";_tag_fat(_tmp172,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 743
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 745
({struct Cyc_Tcenv_Tenv*_tmp718=te;struct Cyc_Absyn_Exp*_tmp717=e1;Cyc_Tcexp_tcTest(_tmp718,_tmp717,({const char*_tmp173="logical-or expression";_tag_fat(_tmp173,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp71A=te;struct Cyc_Absyn_Exp*_tmp719=e2;Cyc_Tcexp_tcTest(_tmp71A,_tmp719,({const char*_tmp174="logical-or expression";_tag_fat(_tmp174,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 751
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 758
({struct Cyc_Tcenv_Tenv*_tmp71B=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp71B,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 763
{void*_tmp175=Cyc_Absyn_compress(t1);void*_stmttmp17=_tmp175;void*_tmp176=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp176)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp178=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58A;_tmp58A.tag=0U,({struct _fat_ptr _tmp71C=({const char*_tmp179="cannot assign to an array";_tag_fat(_tmp179,sizeof(char),26U);});_tmp58A.f1=_tmp71C;});_tmp58A;});void*_tmp177[1U];_tmp177[0]=& _tmp178;({unsigned _tmp71D=loc;Cyc_Warn_err2(_tmp71D,_tag_fat(_tmp177,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 768
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58B;_tmp58B.tag=0U,({struct _fat_ptr _tmp71E=({const char*_tmp17C="type is abstract (can't determine size)";_tag_fat(_tmp17C,sizeof(char),40U);});_tmp58B.f1=_tmp71E;});_tmp58B;});void*_tmp17A[1U];_tmp17A[0]=& _tmp17B;({unsigned _tmp71F=loc;Cyc_Warn_err2(_tmp71F,_tag_fat(_tmp17A,sizeof(void*),1U));});});
# 772
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp17E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58C;_tmp58C.tag=0U,({struct _fat_ptr _tmp720=({const char*_tmp17F="assignment to non-lvalue";_tag_fat(_tmp17F,sizeof(char),25U);});_tmp58C.f1=_tmp720;});_tmp58C;});void*_tmp17D[1U];_tmp17D[0]=& _tmp17E;({struct Cyc_Tcenv_Tenv*_tmp723=te;unsigned _tmp722=loc;void**_tmp721=topt;Cyc_Tcexp_expr_err(_tmp723,_tmp722,_tmp721,_tag_fat(_tmp17D,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
Cyc_Tcexp_check_consume(e2->loc,t2,e2);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp725=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp724=e2;Cyc_Tcutil_coerce_assign(_tmp725,_tmp724,t1);})){
void*_tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp590;_tmp590.tag=0U,({struct _fat_ptr _tmp726=({const char*_tmp187="type mismatch: ";_tag_fat(_tmp187,sizeof(char),16U);});_tmp590.f1=_tmp726;});_tmp590;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp183=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58F;_tmp58F.tag=2U,_tmp58F.f1=(void*)t1;_tmp58F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp184=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58E;_tmp58E.tag=0U,({struct _fat_ptr _tmp727=({const char*_tmp186=" != ";_tag_fat(_tmp186,sizeof(char),5U);});_tmp58E.f1=_tmp727;});_tmp58E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp185=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58D;_tmp58D.tag=2U,_tmp58D.f1=(void*)t2;_tmp58D;});void*_tmp181[4U];_tmp181[0]=& _tmp182,_tmp181[1]=& _tmp183,_tmp181[2]=& _tmp184,_tmp181[3]=& _tmp185;({struct Cyc_Tcenv_Tenv*_tmp72A=te;unsigned _tmp729=loc;void**_tmp728=topt;Cyc_Tcexp_expr_err(_tmp72A,_tmp729,_tmp728,_tag_fat(_tmp181,sizeof(void*),4U));});});void*result=_tmp180;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}
# 783
return t1;}{
# 785
struct Cyc_Absyn_Exp*_tmp188=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp188;
void*_tmp189=Cyc_Tcexp_tcBinPrimop(te,loc,0,(enum Cyc_Absyn_Primop)po->v,e1copy,e2);void*t_result=_tmp189;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
return t1;
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp18A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp18C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp595;_tmp595.tag=0U,({
struct _fat_ptr _tmp72B=({const char*_tmp193="Cannot use this operator in an assignment when ";_tag_fat(_tmp193,sizeof(char),48U);});_tmp595.f1=_tmp72B;});_tmp595;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp594;_tmp594.tag=0U,({
struct _fat_ptr _tmp72C=({const char*_tmp192="the arguments have types ";_tag_fat(_tmp192,sizeof(char),26U);});_tmp594.f1=_tmp72C;});_tmp594;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp18E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp593;_tmp593.tag=2U,_tmp593.f1=(void*)t1;_tmp593;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp592;_tmp592.tag=0U,({struct _fat_ptr _tmp72D=({const char*_tmp191=" and ";_tag_fat(_tmp191,sizeof(char),6U);});_tmp592.f1=_tmp72D;});_tmp592;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp190=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp591;_tmp591.tag=2U,_tmp591.f1=(void*)t2;_tmp591;});void*_tmp18B[5U];_tmp18B[0]=& _tmp18C,_tmp18B[1]=& _tmp18D,_tmp18B[2]=& _tmp18E,_tmp18B[3]=& _tmp18F,_tmp18B[4]=& _tmp190;({struct Cyc_Tcenv_Tenv*_tmp730=te;unsigned _tmp72F=loc;void**_tmp72E=topt;Cyc_Tcexp_expr_err(_tmp730,_tmp72F,_tmp72E,_tag_fat(_tmp18B,sizeof(void*),5U));});});
# 790
void*result=_tmp18A;
# 793
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}
# 797
return t_result;}}}}
# 801
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp731=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp731,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp733=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp732=topt;Cyc_Tcexp_tcExp(_tmp733,_tmp732,e2);});
return(void*)_check_null(e2->topt);}
# 808
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 811
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 814
{void*_tmp194=Cyc_Absyn_compress(t1);void*_stmttmp18=_tmp194;void*_tmp195=_stmttmp18;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)->f1 == 0){_LL1: _LL2:
# 816
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
# 819
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 823
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp196=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp196;struct Cyc_Absyn_Datatypefield _tmp197=_stmttmp19;enum Cyc_Absyn_Scope _tmp19B;unsigned _tmp19A;struct Cyc_List_List*_tmp199;struct _tuple1*_tmp198;_LLA: _tmp198=_tmp197.name;_tmp199=_tmp197.typs;_tmp19A=_tmp197.loc;_tmp19B=_tmp197.sc;_LLB: {struct _tuple1*n=_tmp198;struct Cyc_List_List*typs=_tmp199;unsigned loc=_tmp19A;enum Cyc_Absyn_Scope sc=_tmp19B;
# 826
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 829
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 833
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp19C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp19C;struct Cyc_Absyn_Datatypefield _tmp19D=_stmttmp1A;enum Cyc_Absyn_Scope _tmp1A1;unsigned _tmp1A0;struct Cyc_List_List*_tmp19F;struct _tuple1*_tmp19E;_LLD: _tmp19E=_tmp19D.name;_tmp19F=_tmp19D.typs;_tmp1A0=_tmp19D.loc;_tmp1A1=_tmp19D.sc;_LLE: {struct _tuple1*n=_tmp19E;struct Cyc_List_List*typs=_tmp19F;unsigned loc=_tmp1A0;enum Cyc_Absyn_Scope sc=_tmp1A1;
# 836
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 840
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp735=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp734=e;Cyc_Tcutil_coerce_arg(_tmp735,_tmp734,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 844
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp599;_tmp599.tag=0U,({struct _fat_ptr _tmp736=({const char*_tmp1A8="can't find a field in ";_tag_fat(_tmp1A8,sizeof(char),23U);});_tmp599.f1=_tmp736;});_tmp599;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp598;_tmp598.tag=2U,_tmp598.f1=(void*)tu;_tmp598;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp597;_tmp597.tag=0U,({struct _fat_ptr _tmp737=({const char*_tmp1A7=" to inject value of type ";_tag_fat(_tmp1A7,sizeof(char),26U);});_tmp597.f1=_tmp737;});_tmp597;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp596;_tmp596.tag=2U,_tmp596.f1=(void*)t1;_tmp596;});void*_tmp1A2[4U];_tmp1A2[0]=& _tmp1A3,_tmp1A2[1]=& _tmp1A4,_tmp1A2[2]=& _tmp1A5,_tmp1A2[3]=& _tmp1A6;({unsigned _tmp738=e->loc;Cyc_Warn_err2(_tmp738,_tag_fat(_tmp1A2,sizeof(void*),4U));});});
return 0;}
# 849
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 855
struct Cyc_List_List*_tmp1A9=args;struct Cyc_List_List*es=_tmp1A9;
int _tmp1AA=0;int arg_cnt=_tmp1AA;
struct Cyc_Tcenv_Tenv*_tmp1AB=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp1AB;
struct Cyc_Tcenv_Tenv*_tmp1AC=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp1AC;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
# 864
void*_tmp1AD=t;void*_tmp1B3;void*_tmp1B2;void*_tmp1B1;void*_tmp1B0;struct Cyc_Absyn_Tqual _tmp1AF;void*_tmp1AE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->tag == 3U){_LL1: _tmp1AE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).elt_type;_tmp1AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).elt_tq;_tmp1B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).ptr_atts).rgn;_tmp1B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).ptr_atts).nullable;_tmp1B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).ptr_atts).bounds;_tmp1B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD)->f1).ptr_atts).zero_term;_LL2: {void*t1=_tmp1AE;struct Cyc_Absyn_Tqual tq=_tmp1AF;void*rgn=_tmp1B0;void*x=_tmp1B1;void*b=_tmp1B2;void*zt=_tmp1B3;
# 869
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 871
Cyc_Tcutil_check_nonzero_bound(loc,b);{
void*_tmp1B4=Cyc_Absyn_compress(t1);void*_stmttmp1B=_tmp1B4;void*_tmp1B5=_stmttmp1B;struct Cyc_Absyn_Vardecl*_tmp1C3;struct Cyc_List_List*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_List_List*_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_VarargInfo*_tmp1BC;int _tmp1BB;struct Cyc_List_List*_tmp1BA;void*_tmp1B9;struct Cyc_Absyn_Tqual _tmp1B8;void*_tmp1B7;struct Cyc_List_List*_tmp1B6;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->tag == 5U){_LL6: _tmp1B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).tvars;_tmp1B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).effect;_tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).ret_tqual;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).ret_type;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).args;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).c_varargs;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).cyc_varargs;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).rgn_po;_tmp1BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).attributes;_tmp1BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).requires_clause;_tmp1C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).requires_relns;_tmp1C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).ensures_clause;_tmp1C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).ensures_relns;_tmp1C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B5)->f1).return_value;_LL7: {struct Cyc_List_List*tvars=_tmp1B6;void*eff=_tmp1B7;struct Cyc_Absyn_Tqual res_tq=_tmp1B8;void*res_typ=_tmp1B9;struct Cyc_List_List*args_info=_tmp1BA;int c_vararg=_tmp1BB;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp1BC;struct Cyc_List_List*rgn_po=_tmp1BD;struct Cyc_List_List*atts=_tmp1BE;struct Cyc_Absyn_Exp*req=_tmp1BF;struct Cyc_List_List*req_relns=_tmp1C0;struct Cyc_Absyn_Exp*ens=_tmp1C1;struct Cyc_List_List*ens_relns=_tmp1C2;struct Cyc_Absyn_Vardecl*ret_var=_tmp1C3;
# 876
if(tvars != 0 || rgn_po != 0)
({void*_tmp1C4=0U;({unsigned _tmp73A=e->loc;struct _fat_ptr _tmp739=({const char*_tmp1C5="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1C5,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp73A,_tmp739,_tag_fat(_tmp1C4,sizeof(void*),0U));});});
# 880
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 882
while(es != 0 && args_info != 0){
# 884
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp73C=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp73B=e1;Cyc_Tcutil_coerce_arg(_tmp73C,_tmp73B,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1D7="actual argument has type ";_tag_fat(_tmp1D7,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1D6=" but formal has type ";_tag_fat(_tmp1D6,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp73F=({unsigned long _tmp73E=({unsigned long _tmp73D=Cyc_strlen((struct _fat_ptr)s0);_tmp73D + Cyc_strlen((struct _fat_ptr)s1);});_tmp73E + Cyc_strlen((struct _fat_ptr)s2);});_tmp73F + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=0U,_tmp5A0.f1=s0;_tmp5A0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59F;_tmp59F.tag=0U,({struct _fat_ptr _tmp740=({const char*_tmp1D0="\n\t";_tag_fat(_tmp1D0,sizeof(char),3U);});_tmp59F.f1=_tmp740;});_tmp59F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59E;_tmp59E.tag=0U,_tmp59E.f1=s1;_tmp59E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59D;_tmp59D.tag=0U,({struct _fat_ptr _tmp741=({const char*_tmp1CF="\n";_tag_fat(_tmp1CF,sizeof(char),2U);});_tmp59D.f1=_tmp741;});_tmp59D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59C;_tmp59C.tag=0U,_tmp59C.f1=s2;_tmp59C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59B;_tmp59B.tag=0U,({struct _fat_ptr _tmp742=({const char*_tmp1CE="\n\t";_tag_fat(_tmp1CE,sizeof(char),3U);});_tmp59B.f1=_tmp742;});_tmp59B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59A;_tmp59A.tag=0U,_tmp59A.f1=s3;_tmp59A;});void*_tmp1C6[7U];_tmp1C6[0]=& _tmp1C7,_tmp1C6[1]=& _tmp1C8,_tmp1C6[2]=& _tmp1C9,_tmp1C6[3]=& _tmp1CA,_tmp1C6[4]=& _tmp1CB,_tmp1C6[5]=& _tmp1CC,_tmp1C6[6]=& _tmp1CD;({unsigned _tmp743=e1->loc;Cyc_Warn_err2(_tmp743,_tag_fat(_tmp1C6,sizeof(void*),7U));});});else{
# 896
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A4;_tmp5A4.tag=0U,_tmp5A4.f1=s0;_tmp5A4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A3;_tmp5A3.tag=0U,_tmp5A3.f1=s1;_tmp5A3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A2;_tmp5A2.tag=0U,_tmp5A2.f1=s2;_tmp5A2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A1;_tmp5A1.tag=0U,_tmp5A1.f1=s3;_tmp5A1;});void*_tmp1D1[4U];_tmp1D1[0]=& _tmp1D2,_tmp1D1[1]=& _tmp1D3,_tmp1D1[2]=& _tmp1D4,_tmp1D1[3]=& _tmp1D5;({unsigned _tmp744=e1->loc;Cyc_Warn_err2(_tmp744,_tag_fat(_tmp1D1,sizeof(void*),4U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 901
if(alias_coercion)
({struct Cyc_List_List*_tmp745=({struct Cyc_List_List*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->hd=(void*)arg_cnt,_tmp1D8->tl=*alias_arg_exps;_tmp1D8;});*alias_arg_exps=_tmp745;});
Cyc_Tcexp_check_consume(e1->loc,t2,e1);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 911
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1D9=(void*)a->hd;void*_stmttmp1C=_tmp1D9;void*_tmp1DA=_stmttmp1C;int _tmp1DD;int _tmp1DC;enum Cyc_Absyn_Format_Type _tmp1DB;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1DA)->tag == 19U){_LLB: _tmp1DB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1DA)->f1;_tmp1DC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1DA)->f2;_tmp1DD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1DA)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp1DB;int fmt_arg_pos=_tmp1DC;int arg_start_pos=_tmp1DD;
# 915
{struct _handler_cons _tmp1DE;_push_handler(& _tmp1DE);{int _tmp1E0=0;if(setjmp(_tmp1DE.handler))_tmp1E0=1;if(!_tmp1E0){
# 917
{struct Cyc_Absyn_Exp*_tmp1E1=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1E1;
# 919
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 923
fmt_args=({struct Cyc_Core_Opt*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));({struct Cyc_List_List*_tmp746=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp1E2->v=_tmp746;});_tmp1E2;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1E3=ft;switch(_tmp1E3){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 927
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 930
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 933
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 936
goto _LLF;}_LLF:;}}
# 917
;_pop_handler();}else{void*_tmp1DF=(void*)Cyc_Core_get_exn_thrown();void*_tmp1E4=_tmp1DF;void*_tmp1E5;if(((struct Cyc_List_Nth_exn_struct*)_tmp1E4)->tag == Cyc_List_Nth){_LL17: _LL18:
# 938
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=0U,({struct _fat_ptr _tmp747=({const char*_tmp1E8="bad format arguments";_tag_fat(_tmp1E8,sizeof(char),21U);});_tmp5A5.f1=_tmp747;});_tmp5A5;});void*_tmp1E6[1U];_tmp1E6[0]=& _tmp1E7;({unsigned _tmp748=loc;Cyc_Warn_err2(_tmp748,_tag_fat(_tmp1E6,sizeof(void*),1U));});});goto _LL16;}else{_LL19: _tmp1E5=_tmp1E4;_LL1A: {void*exn=_tmp1E5;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 943
if(args_info != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=0U,({struct _fat_ptr _tmp749=({const char*_tmp1EB="too few arguments for function";_tag_fat(_tmp1EB,sizeof(char),31U);});_tmp5A6.f1=_tmp749;});_tmp5A6;});void*_tmp1E9[1U];_tmp1E9[0]=& _tmp1EA;({unsigned _tmp74A=loc;Cyc_Warn_err2(_tmp74A,_tag_fat(_tmp1E9,sizeof(void*),1U));});});else{
# 946
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=0U,({struct _fat_ptr _tmp74B=({const char*_tmp1EE="too many arguments for function";_tag_fat(_tmp1EE,sizeof(char),32U);});_tmp5A7.f1=_tmp74B;});_tmp5A7;});void*_tmp1EC[1U];_tmp1EC[0]=& _tmp1ED;({unsigned _tmp74C=loc;Cyc_Warn_err2(_tmp74C,_tag_fat(_tmp1EC,sizeof(void*),1U));});});else{
# 953
struct Cyc_Absyn_VarargInfo _tmp1EF=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1D=_tmp1EF;struct Cyc_Absyn_VarargInfo _tmp1F0=_stmttmp1D;int _tmp1F2;void*_tmp1F1;_LL1C: _tmp1F1=_tmp1F0.type;_tmp1F2=_tmp1F0.inject;_LL1D: {void*vt=_tmp1F1;int inject=_tmp1F2;
struct Cyc_Absyn_VarargCallInfo*_tmp1F3=({struct Cyc_Absyn_VarargCallInfo*_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B->num_varargs=0,_tmp20B->injectors=0,_tmp20B->vai=cyc_vararg;_tmp20B;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1F3;
# 957
*vararg_call_info=vci;
# 959
if(!inject)
# 961
for(0;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp74E=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp74D=e1;Cyc_Tcutil_coerce_arg(_tmp74E,_tmp74D,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AB;_tmp5AB.tag=0U,({struct _fat_ptr _tmp74F=({const char*_tmp1FA="vararg requires type ";_tag_fat(_tmp1FA,sizeof(char),22U);});_tmp5AB.f1=_tmp74F;});_tmp5AB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=2U,_tmp5AA.f1=(void*)vt;_tmp5AA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=0U,({
struct _fat_ptr _tmp750=({const char*_tmp1F9=" but argument has type ";_tag_fat(_tmp1F9,sizeof(char),24U);});_tmp5A9.f1=_tmp750;});_tmp5A9;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1F8=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=3U,_tmp5A8.f1=(void*)e1->topt;_tmp5A8;});void*_tmp1F4[4U];_tmp1F4[0]=& _tmp1F5,_tmp1F4[1]=& _tmp1F6,_tmp1F4[2]=& _tmp1F7,_tmp1F4[3]=& _tmp1F8;({unsigned _tmp751=loc;Cyc_Tcexp_err_and_explain(_tmp751,_tag_fat(_tmp1F4,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp752=({struct Cyc_List_List*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->hd=(void*)arg_cnt,_tmp1FB->tl=*alias_arg_exps;_tmp1FB;});*alias_arg_exps=_tmp752;});
Cyc_Tcexp_check_consume(e1->loc,vt,e1);}else{
# 976
void*_tmp1FC=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1E=_tmp1FC;void*_tmp1FD=_stmttmp1E;struct Cyc_List_List*_tmp1FF;struct Cyc_Absyn_Datatypedecl*_tmp1FE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1FE=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f1)->f1).KnownDatatype).val;_tmp1FF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp1FE;struct Cyc_List_List*targs=_tmp1FF;
# 980
struct Cyc_Absyn_Datatypedecl*_tmp200=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp200;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp202=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AD;_tmp5AD.tag=0U,({struct _fat_ptr _tmp753=({const char*_tmp204="can't inject into abstract ";_tag_fat(_tmp204,sizeof(char),28U);});_tmp5AD.f1=_tmp753;});_tmp5AD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp203=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AC;_tmp5AC.tag=2U,_tmp5AC.f1=(void*)vt;_tmp5AC;});void*_tmp201[2U];_tmp201[0]=& _tmp202,_tmp201[1]=& _tmp203;({unsigned _tmp754=loc;Cyc_Warn_err2(_tmp754,_tag_fat(_tmp201,sizeof(void*),2U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 990
({void*_tmp755=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp755,Cyc_Tcenv_curr_rgn(te));});{
# 992
struct Cyc_List_List*_tmp205=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp205;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 997
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_check_consume(e1->loc,(void*)_check_null(e1->topt),e1);}{
# 1001
struct Cyc_Absyn_Datatypefield*_tmp206=({struct Cyc_Tcenv_Tenv*_tmp759=te;struct Cyc_Absyn_Exp*_tmp758=e1;void*_tmp757=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp756=inst;Cyc_Tcexp_tcInjection(_tmp759,_tmp758,_tmp757,_tmp756,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp206;
if(f != 0)
({struct Cyc_List_List*_tmp75B=({
struct Cyc_List_List*_tmp75A=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp75A,({struct Cyc_List_List*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->hd=f,_tmp207->tl=0;_tmp207;}));});
# 1003
vci->injectors=_tmp75B;});}}}
# 1006
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp209=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AE;_tmp5AE.tag=0U,({struct _fat_ptr _tmp75C=({const char*_tmp20A="bad inject vararg type";_tag_fat(_tmp20A,sizeof(char),23U);});_tmp5AE.f1=_tmp75C;});_tmp5AE;});void*_tmp208[1U];_tmp208[0]=& _tmp209;({unsigned _tmp75D=loc;Cyc_Warn_err2(_tmp75D,_tag_fat(_tmp208,sizeof(void*),1U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1012
if(*alias_arg_exps == 0)
# 1021 "/tmp/cyclone/src/tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1023
return res_typ;}}}else{_LL8: _LL9:
# 1025
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp20D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B0;_tmp5B0.tag=0U,({struct _fat_ptr _tmp75E=({const char*_tmp20F="expected pointer to function but found ";_tag_fat(_tmp20F,sizeof(char),40U);});_tmp5B0.f1=_tmp75E;});_tmp5B0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp20E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AF;_tmp5AF.tag=2U,_tmp5AF.f1=(void*)t;_tmp5AF;});void*_tmp20C[2U];_tmp20C[0]=& _tmp20D,_tmp20C[1]=& _tmp20E;({struct Cyc_Tcenv_Tenv*_tmp761=te;unsigned _tmp760=loc;void**_tmp75F=topt;Cyc_Tcexp_expr_err(_tmp761,_tmp760,_tmp75F,_tag_fat(_tmp20C,sizeof(void*),2U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1028
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp211=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B2;_tmp5B2.tag=0U,({struct _fat_ptr _tmp762=({const char*_tmp213="expected pointer to function but found ";_tag_fat(_tmp213,sizeof(char),40U);});_tmp5B2.f1=_tmp762;});_tmp5B2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp212=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B1;_tmp5B1.tag=2U,_tmp5B1.f1=(void*)t;_tmp5B1;});void*_tmp210[2U];_tmp210[0]=& _tmp211,_tmp210[1]=& _tmp212;({struct Cyc_Tcenv_Tenv*_tmp765=te;unsigned _tmp764=loc;void**_tmp763=topt;Cyc_Tcexp_expr_err(_tmp765,_tmp764,_tmp763,_tag_fat(_tmp210,sizeof(void*),2U));});});}_LL0:;}}}
# 1032
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp214=Cyc_Absyn_exn_type();void*exception_type=_tmp214;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp767=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp766=e;Cyc_Tcutil_coerce_arg(_tmp767,_tmp766,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp216=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B6;_tmp5B6.tag=0U,({struct _fat_ptr _tmp768=({const char*_tmp21B="expected ";_tag_fat(_tmp21B,sizeof(char),10U);});_tmp5B6.f1=_tmp768;});_tmp5B6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp217=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B5;_tmp5B5.tag=2U,_tmp5B5.f1=(void*)exception_type;_tmp5B5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp218=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B4;_tmp5B4.tag=0U,({struct _fat_ptr _tmp769=({const char*_tmp21A=" but found ";_tag_fat(_tmp21A,sizeof(char),12U);});_tmp5B4.f1=_tmp769;});_tmp5B4;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp219=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5B3;_tmp5B3.tag=3U,_tmp5B3.f1=(void*)e->topt;_tmp5B3;});void*_tmp215[4U];_tmp215[0]=& _tmp216,_tmp215[1]=& _tmp217,_tmp215[2]=& _tmp218,_tmp215[3]=& _tmp219;({unsigned _tmp76A=loc;Cyc_Warn_err2(_tmp76A,_tag_fat(_tmp215,sizeof(void*),4U));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1043
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1045
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
{void*_tmp21C=t1;struct Cyc_Absyn_PtrAtts _tmp21F;struct Cyc_Absyn_Tqual _tmp21E;void*_tmp21D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->tag == 3U){_LL1: _tmp21D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->f1).elt_type;_tmp21E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->f1).elt_tq;_tmp21F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->f1).ptr_atts;_LL2: {void*t=_tmp21D;struct Cyc_Absyn_Tqual tq=_tmp21E;struct Cyc_Absyn_PtrAtts atts=_tmp21F;
# 1048
{void*_tmp220=Cyc_Absyn_compress(t);void*_stmttmp1F=_tmp220;void*_tmp221=_stmttmp1F;struct Cyc_Absyn_FnInfo _tmp222;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp221)->tag == 5U){_LL6: _tmp222=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp221)->f1;_LL7: {struct Cyc_Absyn_FnInfo info=_tmp222;
# 1050
struct Cyc_List_List*_tmp223=info.tvars;struct Cyc_List_List*tvars=_tmp223;
struct Cyc_List_List*instantiation=0;
# 1053
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);
({unsigned _tmp76E=loc;struct Cyc_Tcenv_Tenv*_tmp76D=te;struct Cyc_List_List*_tmp76C=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp76B=k;Cyc_Tctyp_check_type(_tmp76E,_tmp76D,_tmp76C,_tmp76B,1,1,(void*)ts->hd);});
# 1058
if(({struct Cyc_Absyn_Tvar*_tmp76F=(struct Cyc_Absyn_Tvar*)tvars->hd;Cyc_Absyn_tvar_cmp(_tmp76F,Cyc_CurRgn_curr_rgn_tvar());})== 0){
if(!({void*_tmp770=Cyc_Tcenv_curr_lifo_rgn(te);Cyc_Unify_unify(_tmp770,(void*)ts->hd);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp225=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BB;_tmp5BB.tag=0U,({struct _fat_ptr _tmp771=({const char*_tmp22C="Attempt to instantiate ";_tag_fat(_tmp22C,sizeof(char),24U);});_tmp5BB.f1=_tmp771;});_tmp5BB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp226=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BA;_tmp5BA.tag=2U,({typeof((void*)Cyc_CurRgn_curr_rgn_type())_tmp772=(void*)Cyc_CurRgn_curr_rgn_type();_tmp5BA.f1=_tmp772;});_tmp5BA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp227=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B9;_tmp5B9.tag=0U,({
struct _fat_ptr _tmp773=({const char*_tmp22B=" with ";_tag_fat(_tmp22B,sizeof(char),7U);});_tmp5B9.f1=_tmp773;});_tmp5B9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp228=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B8;_tmp5B8.tag=2U,_tmp5B8.f1=(void*)ts->hd;_tmp5B8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp229=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B7;_tmp5B7.tag=0U,({struct _fat_ptr _tmp774=({const char*_tmp22A=" which is not the topmost region";_tag_fat(_tmp22A,sizeof(char),33U);});_tmp5B7.f1=_tmp774;});_tmp5B7;});void*_tmp224[5U];_tmp224[0]=& _tmp225,_tmp224[1]=& _tmp226,_tmp224[2]=& _tmp227,_tmp224[3]=& _tmp228,_tmp224[4]=& _tmp229;({unsigned _tmp775=loc;Cyc_Warn_err2(_tmp775,_tag_fat(_tmp224,sizeof(void*),5U));});});}
# 1063
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(*_tmp22E));({struct _tuple14*_tmp776=({struct _tuple14*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp22D->f2=(void*)ts->hd;_tmp22D;});_tmp22E->hd=_tmp776;}),_tmp22E->tl=instantiation;_tmp22E;});}
# 1066
info.tvars=tvars;
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp230=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BC;_tmp5BC.tag=0U,({struct _fat_ptr _tmp777=({const char*_tmp231="too many type variables in instantiation";_tag_fat(_tmp231,sizeof(char),41U);});_tmp5BC.f1=_tmp777;});_tmp5BC;});void*_tmp22F[1U];_tmp22F[0]=& _tmp230;({struct Cyc_Tcenv_Tenv*_tmp77A=te;unsigned _tmp779=loc;void**_tmp778=topt;Cyc_Tcexp_expr_err(_tmp77A,_tmp779,_tmp778,_tag_fat(_tmp22F,sizeof(void*),1U));});});
# 1072
if(tvars == 0){
({struct Cyc_List_List*_tmp77B=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,info.rgn_po);info.rgn_po=_tmp77B;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,info.rgn_po);
info.rgn_po=0;}{
# 1077
void*new_fn_typ=({struct Cyc_List_List*_tmp77C=instantiation;Cyc_Tcutil_substitute(_tmp77C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->tag=5U,_tmp232->f1=info;_tmp232;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp5BD;_tmp5BD.elt_type=new_fn_typ,_tmp5BD.elt_tq=tq,_tmp5BD.ptr_atts=atts;_tmp5BD;}));}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1081
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1084
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp234=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BF;_tmp5BF.tag=0U,({struct _fat_ptr _tmp77D=({const char*_tmp236="expecting polymorphic type but found ";_tag_fat(_tmp236,sizeof(char),38U);});_tmp5BF.f1=_tmp77D;});_tmp5BF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp235=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BE;_tmp5BE.tag=2U,_tmp5BE.f1=(void*)t1;_tmp5BE;});void*_tmp233[2U];_tmp233[0]=& _tmp234,_tmp233[1]=& _tmp235;({struct Cyc_Tcenv_Tenv*_tmp780=te;unsigned _tmp77F=loc;void**_tmp77E=topt;Cyc_Tcexp_expr_err(_tmp780,_tmp77F,_tmp77E,_tag_fat(_tmp233,sizeof(void*),2U));});});}
# 1088
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1090
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1092
({void*_tmp781=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp781;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1097
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1099
({unsigned _tmp785=loc;struct Cyc_Tcenv_Tenv*_tmp784=te;struct Cyc_List_List*_tmp783=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp782=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1099
Cyc_Tctyp_check_type(_tmp785,_tmp784,_tmp783,_tmp782,1,0,t);});
# 1101
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp788=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp787=loc;void*_tmp786=t2;Cyc_Tcutil_silent_castable(_tmp788,_tmp787,_tmp786,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1107
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp78B=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp78A=loc;void*_tmp789=t2;Cyc_Tcutil_castable(_tmp78B,_tmp78A,_tmp789,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1114
Cyc_Unify_unify(t2,t);{
void*_tmp237=({struct Cyc_Warn_String_Warn_Warg_struct _tmp239=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3;_tmp5C3.tag=0U,({struct _fat_ptr _tmp78C=({const char*_tmp23E="cannot cast ";_tag_fat(_tmp23E,sizeof(char),13U);});_tmp5C3.f1=_tmp78C;});_tmp5C3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp23A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C2;_tmp5C2.tag=2U,_tmp5C2.f1=(void*)t2;_tmp5C2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp23B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C1;_tmp5C1.tag=0U,({struct _fat_ptr _tmp78D=({const char*_tmp23D=" to ";_tag_fat(_tmp23D,sizeof(char),5U);});_tmp5C1.f1=_tmp78D;});_tmp5C1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp23C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C0;_tmp5C0.tag=2U,_tmp5C0.f1=(void*)t;_tmp5C0;});void*_tmp238[4U];_tmp238[0]=& _tmp239,_tmp238[1]=& _tmp23A,_tmp238[2]=& _tmp23B,_tmp238[3]=& _tmp23C;({struct Cyc_Tcenv_Tenv*_tmp78F=te;unsigned _tmp78E=loc;Cyc_Tcexp_expr_err(_tmp78F,_tmp78E,& t,_tag_fat(_tmp238,sizeof(void*),4U));});});void*result=_tmp237;
Cyc_Unify_explain_failure();
return result;}}}}{
# 1123
struct _tuple0 _tmp23F=({struct _tuple0 _tmp5C4;_tmp5C4.f1=e->r,({void*_tmp790=Cyc_Absyn_compress(t);_tmp5C4.f2=_tmp790;});_tmp5C4;});struct _tuple0 _stmttmp20=_tmp23F;struct _tuple0 _tmp240=_stmttmp20;void*_tmp244;void*_tmp243;void*_tmp242;int _tmp241;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp240.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240.f2)->tag == 3U){_LL1: _tmp241=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp240.f1)->f1).fat_result;_tmp242=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240.f2)->f1).ptr_atts).nullable;_tmp243=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240.f2)->f1).ptr_atts).bounds;_tmp244=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240.f2)->f1).ptr_atts).zero_term;_LL2: {int fat_result=_tmp241;void*nbl=_tmp242;void*bds=_tmp243;void*zt=_tmp244;
# 1127
if((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl)){
struct Cyc_Absyn_Exp*_tmp245=({void*_tmp791=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp791,bds);});struct Cyc_Absyn_Exp*eopt=_tmp245;
if(eopt != 0){
if((Cyc_Evexp_eval_const_uint_exp(e)).f1 == (unsigned)1)
({void*_tmp246=0U;({unsigned _tmp793=loc;struct _fat_ptr _tmp792=({const char*_tmp247="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp247,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp793,_tmp792,_tag_fat(_tmp246,sizeof(void*),0U));});});}}
# 1133
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;}_LL0:;}}}
# 1139
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp248=0;void**topt2=_tmp248;
struct Cyc_Absyn_Tqual _tmp249=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp249;
int _tmp24A=0;int nullable=_tmp24A;
if(topt != 0){
void*_tmp24B=Cyc_Absyn_compress(*topt);void*_stmttmp21=_tmp24B;void*_tmp24C=_stmttmp21;void*_tmp24F;struct Cyc_Absyn_Tqual _tmp24E;void**_tmp24D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->tag == 3U){_LL1: _tmp24D=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).elt_type;_tmp24E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).elt_tq;_tmp24F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp24D;struct Cyc_Absyn_Tqual tq=_tmp24E;void*n=_tmp24F;
# 1146
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1160
({struct Cyc_Tcenv_Tenv*_tmp795=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp794=topt2;Cyc_Tcexp_tcExpNoInst(_tmp795,_tmp794,e);});
# 1162
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp251=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C5;_tmp5C5.tag=0U,({struct _fat_ptr _tmp796=({const char*_tmp252="cannot take the address of an alias-free path";_tag_fat(_tmp252,sizeof(char),46U);});_tmp5C5.f1=_tmp796;});_tmp5C5;});void*_tmp250[1U];_tmp250[0]=& _tmp251;({unsigned _tmp797=e->loc;Cyc_Warn_err2(_tmp797,_tag_fat(_tmp250,sizeof(void*),1U));});});
# 1167
{void*_tmp253=e->r;void*_stmttmp22=_tmp253;void*_tmp254=_stmttmp22;struct Cyc_Absyn_Exp*_tmp256;struct Cyc_Absyn_Exp*_tmp255;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp254)->tag == 23U){_LL6: _tmp255=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp254)->f1;_tmp256=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp254)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp255;struct Cyc_Absyn_Exp*e2=_tmp256;
# 1169
{void*_tmp257=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp23=_tmp257;void*_tmp258=_stmttmp23;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp258)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1174
({void*_tmp798=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp798;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1177
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1182
{void*_tmp259=e->r;void*_stmttmp24=_tmp259;void*_tmp25A=_stmttmp24;switch(*((int*)_tmp25A)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25A)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp25A)->f3 == 1){_LL12: _LL13:
# 1185
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C6;_tmp5C6.tag=0U,({struct _fat_ptr _tmp799=({const char*_tmp25D="cannot take the address of a @tagged union member";_tag_fat(_tmp25D,sizeof(char),50U);});_tmp5C6.f1=_tmp799;});_tmp5C6;});void*_tmp25B[1U];_tmp25B[0]=& _tmp25C;({unsigned _tmp79A=e->loc;Cyc_Warn_err2(_tmp79A,_tag_fat(_tmp25B,sizeof(void*),1U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1191
struct _tuple15 _tmp25E=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp25=_tmp25E;struct _tuple15 _tmp25F=_stmttmp25;void*_tmp261;int _tmp260;_LL17: _tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;_LL18: {int is_const=_tmp260;void*rgn=_tmp261;
# 1193
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp263=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C7;_tmp5C7.tag=0U,({struct _fat_ptr _tmp79B=({const char*_tmp264="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp264,sizeof(char),60U);});_tmp5C7.f1=_tmp79B;});_tmp5C7;});void*_tmp262[1U];_tmp262[0]=& _tmp263;({unsigned _tmp79C=e->loc;Cyc_Warn_err2(_tmp79C,_tag_fat(_tmp262,sizeof(void*),1U));});});{
# 1196
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _tmp79D=tq.real_const=1;tq.print_const=_tmp79D;});
# 1200
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type);}}}}
# 1204
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1207
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _tmp7A0=loc;struct Cyc_Tcenv_Tenv*_tmp79F=te;struct Cyc_List_List*_tmp79E=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7A0,_tmp79F,_tmp79E,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp266=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=0U,({struct _fat_ptr _tmp7A1=({const char*_tmp26A="sizeof applied to type ";_tag_fat(_tmp26A,sizeof(char),24U);});_tmp5CA.f1=_tmp7A1;});_tmp5CA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp267=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=2U,_tmp5C9.f1=(void*)t;_tmp5C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp268=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C8;_tmp5C8.tag=0U,({struct _fat_ptr _tmp7A2=({const char*_tmp269=" which has unknown size here";_tag_fat(_tmp269,sizeof(char),29U);});_tmp5C8.f1=_tmp7A2;});_tmp5C8;});void*_tmp265[3U];_tmp265[0]=& _tmp266,_tmp265[1]=& _tmp267,_tmp265[2]=& _tmp268;({unsigned _tmp7A3=loc;Cyc_Warn_err2(_tmp7A3,_tag_fat(_tmp265,sizeof(void*),3U));});});
if(topt == 0)
return Cyc_Absyn_uint_type;{
void*_tmp26B=Cyc_Absyn_compress(*topt);void*_stmttmp26=_tmp26B;void*_tmp26C=_stmttmp26;void*_tmp26D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f2 != 0){_LL1: _tmp26D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26C)->f2)->hd;_LL2: {void*tt=_tmp26D;
# 1217
if(({void*_tmp7A5=tt;Cyc_Unify_unify(_tmp7A5,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->tag=9U,({struct Cyc_Absyn_Exp*_tmp7A4=Cyc_Absyn_sizeoftype_exp(t,0U);_tmp26E->f1=_tmp7A4;});_tmp26E;}));}))
return Cyc_Absyn_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_uint_type;}_LL0:;}}
# 1224
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)
return sf->type;
return 0;}
# 1232
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1234
({unsigned _tmp7A8=loc;struct Cyc_Tcenv_Tenv*_tmp7A7=te;struct Cyc_List_List*_tmp7A6=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7A8,_tmp7A7,_tmp7A6,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp26F=fs;struct Cyc_List_List*l=_tmp26F;
void*_tmp270=t_orig;void*t=_tmp270;
for(0;l != 0;l=l->tl){
void*_tmp271=(void*)l->hd;void*n=_tmp271;
void*_tmp272=n;unsigned _tmp273;struct _fat_ptr*_tmp274;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp272)->tag == 0U){_LL1: _tmp274=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp272)->f1;_LL2: {struct _fat_ptr*n=_tmp274;
# 1242
int bad_type=1;
{void*_tmp275=Cyc_Absyn_compress(t);void*_stmttmp27=_tmp275;void*_tmp276=_stmttmp27;struct Cyc_List_List*_tmp277;struct Cyc_Absyn_Aggrdecl**_tmp278;switch(*((int*)_tmp276)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp276)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp276)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp278=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp276)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp278;
# 1245
if((*adp)->impl == 0)
goto _LL5;
_tmp277=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL9;}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp277=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp276)->f2;_LL9: {struct Cyc_List_List*fields=_tmp277;
# 1249
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CC;_tmp5CC.tag=0U,({struct _fat_ptr _tmp7A9=({const char*_tmp27C="no field of struct/union has name %s";_tag_fat(_tmp27C,sizeof(char),37U);});_tmp5CC.f1=_tmp7A9;});_tmp5CC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=0U,_tmp5CB.f1=*n;_tmp5CB;});void*_tmp279[2U];_tmp279[0]=& _tmp27A,_tmp279[1]=& _tmp27B;({unsigned _tmp7AA=loc;Cyc_Warn_err2(_tmp7AA,_tag_fat(_tmp279,sizeof(void*),2U));});});else{
# 1253
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1258
if(bad_type){
if(l == fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp27E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5CE;_tmp5CE.tag=2U,_tmp5CE.f1=(void*)t;_tmp5CE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CD;_tmp5CD.tag=0U,({struct _fat_ptr _tmp7AB=({const char*_tmp280=" is not a known struct/union type";_tag_fat(_tmp280,sizeof(char),34U);});_tmp5CD.f1=_tmp7AB;});_tmp5CD;});void*_tmp27D[2U];_tmp27D[0]=& _tmp27E,_tmp27D[1]=& _tmp27F;({unsigned _tmp7AC=loc;Cyc_Warn_err2(_tmp7AC,_tag_fat(_tmp27D,sizeof(void*),2U));});});else{
# 1262
struct _fat_ptr _tmp281=({struct Cyc_String_pa_PrintArg_struct _tmp297=({struct Cyc_String_pa_PrintArg_struct _tmp5D7;_tmp5D7.tag=0U,({struct _fat_ptr _tmp7AD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp5D7.f1=_tmp7AD;});_tmp5D7;});void*_tmp295[1U];_tmp295[0]=& _tmp297;({struct _fat_ptr _tmp7AE=({const char*_tmp296="(%s)";_tag_fat(_tmp296,sizeof(char),5U);});Cyc_aprintf(_tmp7AE,_tag_fat(_tmp295,sizeof(void*),1U));});});struct _fat_ptr s=_tmp281;
{struct Cyc_List_List*x=fs;for(0;x != l;x=x->tl){
void*_tmp282=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp28=_tmp282;void*_tmp283=_stmttmp28;unsigned _tmp284;struct _fat_ptr*_tmp285;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp283)->tag == 0U){_LLD: _tmp285=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp283)->f1;_LLE: {struct _fat_ptr*n=_tmp285;
s=({struct Cyc_String_pa_PrintArg_struct _tmp288=({struct Cyc_String_pa_PrintArg_struct _tmp5D0;_tmp5D0.tag=0U,_tmp5D0.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5D0;});struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp5CF;_tmp5CF.tag=0U,_tmp5CF.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5CF;});void*_tmp286[2U];_tmp286[0]=& _tmp288,_tmp286[1]=& _tmp289;({struct _fat_ptr _tmp7AF=({const char*_tmp287="%s.%s";_tag_fat(_tmp287,sizeof(char),6U);});Cyc_aprintf(_tmp7AF,_tag_fat(_tmp286,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp284=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp283)->f1;_LL10: {unsigned n=_tmp284;
s=({struct Cyc_String_pa_PrintArg_struct _tmp28C=({struct Cyc_String_pa_PrintArg_struct _tmp5D2;_tmp5D2.tag=0U,_tmp5D2.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5D2;});struct Cyc_Int_pa_PrintArg_struct _tmp28D=({struct Cyc_Int_pa_PrintArg_struct _tmp5D1;_tmp5D1.tag=1U,_tmp5D1.f1=(unsigned long)((int)n);_tmp5D1;});void*_tmp28A[2U];_tmp28A[0]=& _tmp28C,_tmp28A[1]=& _tmp28D;({struct _fat_ptr _tmp7B0=({const char*_tmp28B="%s.%d";_tag_fat(_tmp28B,sizeof(char),6U);});Cyc_aprintf(_tmp7B0,_tag_fat(_tmp28A,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}}
# 1268
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D6;_tmp5D6.tag=0U,_tmp5D6.f1=(struct _fat_ptr)s;_tmp5D6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp290=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D5;_tmp5D5.tag=0U,({struct _fat_ptr _tmp7B1=({const char*_tmp294=" == ";_tag_fat(_tmp294,sizeof(char),5U);});_tmp5D5.f1=_tmp7B1;});_tmp5D5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp291=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=2U,_tmp5D4.f1=(void*)t;_tmp5D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp292=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=0U,({struct _fat_ptr _tmp7B2=({const char*_tmp293=" is not a struct/union type";_tag_fat(_tmp293,sizeof(char),28U);});_tmp5D3.f1=_tmp7B2;});_tmp5D3;});void*_tmp28E[4U];_tmp28E[0]=& _tmp28F,_tmp28E[1]=& _tmp290,_tmp28E[2]=& _tmp291,_tmp28E[3]=& _tmp292;({unsigned _tmp7B3=loc;Cyc_Warn_err2(_tmp7B3,_tag_fat(_tmp28E,sizeof(void*),4U));});});}}
# 1271
goto _LL0;}}else{_LL3: _tmp273=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp272)->f1;_LL4: {unsigned n=_tmp273;
# 1273
int bad_type=1;
{void*_tmp298=Cyc_Absyn_compress(t);void*_stmttmp29=_tmp298;void*_tmp299=_stmttmp29;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp29B;struct Cyc_Absyn_Datatypefield*_tmp29C;struct Cyc_Absyn_Aggrdecl**_tmp29D;switch(*((int*)_tmp299)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp29D=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp29D;
# 1276
if((*adp)->impl == 0)
goto _LL11;
_tmp29B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp29C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp29C;
# 1295
if(({unsigned _tmp7B4=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp7B4 < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp2AE=({struct Cyc_Int_pa_PrintArg_struct _tmp5D9;_tmp5D9.tag=1U,({
unsigned long _tmp7B5=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp5D9.f1=_tmp7B5;});_tmp5D9;});struct Cyc_Int_pa_PrintArg_struct _tmp2AF=({struct Cyc_Int_pa_PrintArg_struct _tmp5D8;_tmp5D8.tag=1U,_tmp5D8.f1=(unsigned long)((int)n);_tmp5D8;});void*_tmp2AC[2U];_tmp2AC[0]=& _tmp2AE,_tmp2AC[1]=& _tmp2AF;({unsigned _tmp7B7=loc;struct _fat_ptr _tmp7B6=({const char*_tmp2AD="datatype field has too few components: %d < %d";_tag_fat(_tmp2AD,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp7B7,_tmp7B6,_tag_fat(_tmp2AC,sizeof(void*),2U));});});else{
# 1299
if(n != (unsigned)0)
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp2B0=0U;({unsigned _tmp7B9=loc;struct _fat_ptr _tmp7B8=({const char*_tmp2B1="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp2B1,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp7B9,_tmp7B8,_tag_fat(_tmp2B0,sizeof(void*),0U));});});}}
# 1304
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp29B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp299)->f2;_LL15: {struct Cyc_List_List*fields=_tmp29B;
# 1280
if(({unsigned _tmp7BA=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp7BA <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DD;_tmp5DD.tag=0U,({struct _fat_ptr _tmp7BB=({const char*_tmp2A4="struct/union has too few components: ";_tag_fat(_tmp2A4,sizeof(char),38U);});_tmp5DD.f1=_tmp7BB;});_tmp5DD;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2A0=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=12U,({
int _tmp7BC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp5DC.f1=_tmp7BC;});_tmp5DC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DB;_tmp5DB.tag=0U,({struct _fat_ptr _tmp7BD=({const char*_tmp2A3=" <= ";_tag_fat(_tmp2A3,sizeof(char),5U);});_tmp5DB.f1=_tmp7BD;});_tmp5DB;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2A2=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5DA;_tmp5DA.tag=12U,_tmp5DA.f1=(int)n;_tmp5DA;});void*_tmp29E[4U];_tmp29E[0]=& _tmp29F,_tmp29E[1]=& _tmp2A0,_tmp29E[2]=& _tmp2A1,_tmp29E[3]=& _tmp2A2;({unsigned _tmp7BE=loc;Cyc_Warn_err2(_tmp7BE,_tag_fat(_tmp29E,sizeof(void*),4U));});});else{
# 1284
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp29A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp299)->f1;_LL17: {struct Cyc_List_List*l=_tmp29A;
# 1288
if(({unsigned _tmp7BF=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp7BF <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E1;_tmp5E1.tag=0U,({struct _fat_ptr _tmp7C0=({const char*_tmp2AB="tuple has too few components: ";_tag_fat(_tmp2AB,sizeof(char),31U);});_tmp5E1.f1=_tmp7C0;});_tmp5E1;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2A7=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5E0;_tmp5E0.tag=12U,({int _tmp7C1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp5E0.f1=_tmp7C1;});_tmp5E0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DF;_tmp5DF.tag=0U,({struct _fat_ptr _tmp7C2=({const char*_tmp2AA=" <= ";_tag_fat(_tmp2AA,sizeof(char),5U);});_tmp5DF.f1=_tmp7C2;});_tmp5DF;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2A9=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5DE;_tmp5DE.tag=12U,_tmp5DE.f1=(int)n;_tmp5DE;});void*_tmp2A5[4U];_tmp2A5[0]=& _tmp2A6,_tmp2A5[1]=& _tmp2A7,_tmp2A5[2]=& _tmp2A8,_tmp2A5[3]=& _tmp2A9;({unsigned _tmp7C3=loc;Cyc_Warn_err2(_tmp7C3,_tag_fat(_tmp2A5,sizeof(void*),4U));});});else{
# 1291
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1306
 goto _LL11;}_LL11:;}
# 1308
if(bad_type)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2B3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E3;_tmp5E3.tag=2U,_tmp5E3.f1=(void*)t;_tmp5E3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E2;_tmp5E2.tag=0U,({struct _fat_ptr _tmp7C4=({const char*_tmp2B5=" is not a known type";_tag_fat(_tmp2B5,sizeof(char),21U);});_tmp5E2.f1=_tmp7C4;});_tmp5E2;});void*_tmp2B2[2U];_tmp2B2[0]=& _tmp2B3,_tmp2B2[1]=& _tmp2B4;({unsigned _tmp7C5=loc;Cyc_Warn_err2(_tmp7C5,_tag_fat(_tmp2B2,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 1313
return Cyc_Absyn_uint_type;}}
# 1317
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress((void*)_check_null(e->topt));
{void*_tmp2B6=t;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B6)->tag == 1U){_LL1: _LL2: {
# 1323
struct Cyc_Absyn_PtrInfo _tmp2B7=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp2B7;
({void*_tmp7C7=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));({struct Cyc_List_List*_tmp7C6=Cyc_Tcenv_lookup_type_vars(te);_tmp2B8->v=_tmp7C6;});_tmp2B8;}));pi.elt_type=_tmp7C7;});{
void*_tmp2B9=Cyc_Absyn_pointer_type(pi);void*new_typ=_tmp2B9;
Cyc_Unify_unify(t,new_typ);
t=Cyc_Absyn_compress(t);
goto _LL0;}}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1331
void*_tmp2BA=t;void*_tmp2BE;void*_tmp2BD;void*_tmp2BC;void*_tmp2BB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA)->tag == 3U){_LL6: _tmp2BB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA)->f1).elt_type;_tmp2BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA)->f1).ptr_atts).rgn;_tmp2BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA)->f1).ptr_atts).bounds;_tmp2BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA)->f1).ptr_atts).zero_term;_LL7: {void*t2=_tmp2BB;void*rt=_tmp2BC;void*b=_tmp2BD;void*zt=_tmp2BE;
# 1333
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2BF=Cyc_Absyn_compress(t2);void*_stmttmp2A=_tmp2BF;void*_tmp2C0=_stmttmp2A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C0)->tag == 5U){_LLB: _LLC:
# 1338
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=0U,({struct _fat_ptr _tmp7C8=({const char*_tmp2C3="unnecessary dereference for function type";_tag_fat(_tmp2C3,sizeof(char),42U);});_tmp5E4.f1=_tmp7C8;});_tmp5E4;});void*_tmp2C1[1U];_tmp2C1[0]=& _tmp2C2;({unsigned _tmp7C9=loc;Cyc_Warn_warn2(_tmp7C9,_tag_fat(_tmp2C1,sizeof(void*),1U));});});
return t;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=0U,({struct _fat_ptr _tmp7CA=({const char*_tmp2C6="cannot dereference abstract pointer type";_tag_fat(_tmp2C6,sizeof(char),41U);});_tmp5E5.f1=_tmp7CA;});_tmp5E5;});void*_tmp2C4[1U];_tmp2C4[0]=& _tmp2C5;({unsigned _tmp7CB=loc;Cyc_Warn_err2(_tmp7CB,_tag_fat(_tmp2C4,sizeof(void*),1U));});});}_LLA:;}
# 1343
return t2;}}else{_LL8: _LL9:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=0U,({struct _fat_ptr _tmp7CC=({const char*_tmp2CA="expecting pointer type but found ";_tag_fat(_tmp2CA,sizeof(char),34U);});_tmp5E7.f1=_tmp7CC;});_tmp5E7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=2U,_tmp5E6.f1=(void*)t;_tmp5E6;});void*_tmp2C7[2U];_tmp2C7[0]=& _tmp2C8,_tmp2C7[1]=& _tmp2C9;({struct Cyc_Tcenv_Tenv*_tmp7CF=te;unsigned _tmp7CE=loc;void**_tmp7CD=topt;Cyc_Tcexp_expr_err(_tmp7CF,_tmp7CE,_tmp7CD,_tag_fat(_tmp2C7,sizeof(void*),2U));});});}_LL5:;}}}
# 1348
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1352
({int _tmp7D0=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp7D0;});{
void*_tmp2CB=Cyc_Absyn_compress(aggr_type);void*_stmttmp2B=_tmp2CB;void*_tmp2CC=_stmttmp2B;struct Cyc_List_List*_tmp2CE;enum Cyc_Absyn_AggrKind _tmp2CD;struct Cyc_List_List*_tmp2D0;struct Cyc_Absyn_Aggrdecl*_tmp2CF;switch(*((int*)_tmp2CC)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CC)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CC)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp2CF=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CC)->f1)->f1).KnownAggr).val;_tmp2D0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CC)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2CF;struct Cyc_List_List*ts=_tmp2D0;
# 1355
struct Cyc_Absyn_Aggrfield*_tmp2D1=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2D1;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2D3=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=6U,_tmp5EB.f1=ad;_tmp5EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=0U,({struct _fat_ptr _tmp7D1=({const char*_tmp2D8=" has no ";_tag_fat(_tmp2D8,sizeof(char),9U);});_tmp5EA.f1=_tmp7D1;});_tmp5EA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=0U,_tmp5E9.f1=*f;_tmp5E9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=0U,({struct _fat_ptr _tmp7D2=({const char*_tmp2D7=" member";_tag_fat(_tmp2D7,sizeof(char),8U);});_tmp5E8.f1=_tmp7D2;});_tmp5E8;});void*_tmp2D2[4U];_tmp2D2[0]=& _tmp2D3,_tmp2D2[1]=& _tmp2D4,_tmp2D2[2]=& _tmp2D5,_tmp2D2[3]=& _tmp2D6;({struct Cyc_Tcenv_Tenv*_tmp7D5=te;unsigned _tmp7D4=loc;void**_tmp7D3=topt;Cyc_Tcexp_expr_err(_tmp7D5,_tmp7D4,_tmp7D3,_tag_fat(_tmp2D2,sizeof(void*),4U));});});
# 1359
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp7D6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp7D6,finfo->type);});{
struct Cyc_Absyn_Kind*_tmp2D9=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*t2_kind=_tmp2D9;
# 1367
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2DA=Cyc_Absyn_compress(t2);void*_stmttmp2C=_tmp2DA;void*_tmp2DB=_stmttmp2C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DB)->tag == 4U){_LL8: _LL9:
 goto _LL7;}else{_LLA: _LLB:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=0U,({struct _fat_ptr _tmp7D7=({const char*_tmp2E1="cannot get member ";_tag_fat(_tmp2E1,sizeof(char),19U);});_tmp5EE.f1=_tmp7D7;});_tmp5EE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5ED;_tmp5ED.tag=0U,_tmp5ED.f1=*f;_tmp5ED;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=0U,({
struct _fat_ptr _tmp7D8=({const char*_tmp2E0=" since its type is abstract";_tag_fat(_tmp2E0,sizeof(char),28U);});_tmp5EC.f1=_tmp7D8;});_tmp5EC;});void*_tmp2DC[3U];_tmp2DC[0]=& _tmp2DD,_tmp2DC[1]=& _tmp2DE,_tmp2DC[2]=& _tmp2DF;({struct Cyc_Tcenv_Tenv*_tmp7DB=te;unsigned _tmp7DA=loc;void**_tmp7D9=topt;Cyc_Tcexp_expr_err(_tmp7DB,_tmp7DA,_tmp7D9,_tag_fat(_tmp2DC,sizeof(void*),3U));});});}_LL7:;}
# 1375
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=0U,({struct _fat_ptr _tmp7DC=({const char*_tmp2E7="cannot read union member ";_tag_fat(_tmp2E7,sizeof(char),26U);});_tmp5F1.f1=_tmp7DC;});_tmp5F1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0U,_tmp5F0.f1=*f;_tmp5F0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=0U,({
struct _fat_ptr _tmp7DD=({const char*_tmp2E6=" since it is not `bits-only'";_tag_fat(_tmp2E6,sizeof(char),29U);});_tmp5EF.f1=_tmp7DD;});_tmp5EF;});void*_tmp2E2[3U];_tmp2E2[0]=& _tmp2E3,_tmp2E2[1]=& _tmp2E4,_tmp2E2[2]=& _tmp2E5;({struct Cyc_Tcenv_Tenv*_tmp7E0=te;unsigned _tmp7DF=loc;void**_tmp7DE=topt;Cyc_Tcexp_expr_err(_tmp7E0,_tmp7DF,_tmp7DE,_tag_fat(_tmp2E2,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1381
if(!({void*_tmp7E1=t2;Cyc_Unify_unify(_tmp7E1,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=0U,({struct _fat_ptr _tmp7E2=({const char*_tmp2ED="must use pattern-matching to access field ";_tag_fat(_tmp2ED,sizeof(char),43U);});_tmp5F4.f1=_tmp7E2;});_tmp5F4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=0U,_tmp5F3.f1=*f;_tmp5F3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0U,({struct _fat_ptr _tmp7E3=({const char*_tmp2EC="\n\tdue to existential type variables.";_tag_fat(_tmp2EC,sizeof(char),37U);});_tmp5F2.f1=_tmp7E3;});_tmp5F2;});void*_tmp2E8[3U];_tmp2E8[0]=& _tmp2E9,_tmp2E8[1]=& _tmp2EA,_tmp2E8[2]=& _tmp2EB;({struct Cyc_Tcenv_Tenv*_tmp7E6=te;unsigned _tmp7E5=loc;void**_tmp7E4=topt;Cyc_Tcexp_expr_err(_tmp7E6,_tmp7E5,_tmp7E4,_tag_fat(_tmp2E8,sizeof(void*),3U));});});}
# 1384
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp2CD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CC)->f1;_tmp2CE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CC)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp2CD;struct Cyc_List_List*fs=_tmp2CE;
# 1386
struct Cyc_Absyn_Aggrfield*_tmp2EE=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2EE;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F7;_tmp5F7.tag=0U,({struct _fat_ptr _tmp7E7=({const char*_tmp2F4="type has no ";_tag_fat(_tmp2F4,sizeof(char),13U);});_tmp5F7.f1=_tmp7E7;});_tmp5F7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F6;_tmp5F6.tag=0U,_tmp5F6.f1=*f;_tmp5F6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=0U,({struct _fat_ptr _tmp7E8=({const char*_tmp2F3=" member";_tag_fat(_tmp2F3,sizeof(char),8U);});_tmp5F5.f1=_tmp7E8;});_tmp5F5;});void*_tmp2EF[3U];_tmp2EF[0]=& _tmp2F0,_tmp2EF[1]=& _tmp2F1,_tmp2EF[2]=& _tmp2F2;({struct Cyc_Tcenv_Tenv*_tmp7EB=te;unsigned _tmp7EA=loc;void**_tmp7E9=topt;Cyc_Tcexp_expr_err(_tmp7EB,_tmp7EA,_tmp7E9,_tag_fat(_tmp2EF,sizeof(void*),3U));});});
# 1391
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1393
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=0U,({struct _fat_ptr _tmp7EC=({const char*_tmp2FA="cannot read union member ";_tag_fat(_tmp2FA,sizeof(char),26U);});_tmp5FA.f1=_tmp7EC;});_tmp5FA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F9;_tmp5F9.tag=0U,_tmp5F9.f1=*f;_tmp5F9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F8;_tmp5F8.tag=0U,({struct _fat_ptr _tmp7ED=({const char*_tmp2F9=" since it is not `bits-only'";_tag_fat(_tmp2F9,sizeof(char),29U);});_tmp5F8.f1=_tmp7ED;});_tmp5F8;});void*_tmp2F5[3U];_tmp2F5[0]=& _tmp2F6,_tmp2F5[1]=& _tmp2F7,_tmp2F5[2]=& _tmp2F8;({struct Cyc_Tcenv_Tenv*_tmp7F0=te;unsigned _tmp7EF=loc;void**_tmp7EE=topt;Cyc_Tcexp_expr_err(_tmp7F0,_tmp7EF,_tmp7EE,_tag_fat(_tmp2F5,sizeof(void*),3U));});});
return finfo->type;}default: _LL5: _LL6:
# 1396
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FC;_tmp5FC.tag=0U,({struct _fat_ptr _tmp7F1=({const char*_tmp2FE="expecting struct or union, found ";_tag_fat(_tmp2FE,sizeof(char),34U);});_tmp5FC.f1=_tmp7F1;});_tmp5FC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5FB;_tmp5FB.tag=2U,_tmp5FB.f1=(void*)aggr_type;_tmp5FB;});void*_tmp2FB[2U];_tmp2FB[0]=& _tmp2FC,_tmp2FB[1]=& _tmp2FD;({struct Cyc_Tcenv_Tenv*_tmp7F4=te;unsigned _tmp7F3=loc;void**_tmp7F2=topt;Cyc_Tcexp_expr_err(_tmp7F4,_tmp7F3,_tmp7F2,_tag_fat(_tmp2FB,sizeof(void*),2U));});});}_LL0:;}}
# 1401
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1405
({struct Cyc_Tcenv_Tenv*_tmp7F5=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp7F5,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,(void*)_check_null(e->topt),f,is_tagged,is_read);}
# 1410
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1414
void*t2=({struct Cyc_Tcenv_Tenv*_tmp7F7=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _tmp7F6=loc;Cyc_Tcexp_tcDeref(_tmp7F7,_tmp7F6,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1419
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1421
struct Cyc_Tcenv_Tenv*_tmp2FF=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp2FF;
({struct Cyc_Tcenv_Tenv*_tmp7F8=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F8,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp7F9=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F9,0,e2);});{
void*t1=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Absyn_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp301=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FE;_tmp5FE.tag=0U,({struct _fat_ptr _tmp7FA=({const char*_tmp303="expecting int subscript, found ";_tag_fat(_tmp303,sizeof(char),32U);});_tmp5FE.f1=_tmp7FA;});_tmp5FE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp302=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5FD;_tmp5FD.tag=2U,_tmp5FD.f1=(void*)t2;_tmp5FD;});void*_tmp300[2U];_tmp300[0]=& _tmp301,_tmp300[1]=& _tmp302;({struct Cyc_Tcenv_Tenv*_tmp7FD=te;unsigned _tmp7FC=e2->loc;void**_tmp7FB=topt;Cyc_Tcexp_expr_err(_tmp7FD,_tmp7FC,_tmp7FB,_tag_fat(_tmp300,sizeof(void*),2U));});});{
# 1430
void*_tmp304=t1;struct Cyc_List_List*_tmp305;void*_tmp30A;void*_tmp309;void*_tmp308;struct Cyc_Absyn_Tqual _tmp307;void*_tmp306;switch(*((int*)_tmp304)){case 3U: _LL1: _tmp306=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).elt_type;_tmp307=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).elt_tq;_tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).ptr_atts).rgn;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).ptr_atts).bounds;_tmp30A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp306;struct Cyc_Absyn_Tqual tq=_tmp307;void*rt=_tmp308;void*b=_tmp309;void*zt=_tmp30A;
# 1434
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp30B=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp30B;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp30C=eopt;struct Cyc_Absyn_Exp*e3=_tmp30C;
struct _tuple16 _tmp30D=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2D=_tmp30D;struct _tuple16 _tmp30E=_stmttmp2D;int _tmp310;unsigned _tmp30F;_LL8: _tmp30F=_tmp30E.f1;_tmp310=_tmp30E.f2;_LL9: {unsigned i=_tmp30F;int known_i=_tmp310;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp311=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2E=_tmp311;struct _tuple16 _tmp312=_stmttmp2E;int _tmp314;unsigned _tmp313;_LLB: _tmp313=_tmp312.f1;_tmp314=_tmp312.f2;_LLC: {unsigned j=_tmp313;int known_j=_tmp314;
if(known_j){
struct _tuple16 _tmp315=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2F=_tmp315;struct _tuple16 _tmp316=_stmttmp2F;int _tmp318;unsigned _tmp317;_LLE: _tmp317=_tmp316.f1;_tmp318=_tmp316.f2;_LLF: {unsigned j=_tmp317;int knownj=_tmp318;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp319=0U;({unsigned _tmp7FF=loc;struct _fat_ptr _tmp7FE=({const char*_tmp31A="subscript will fail at run-time";_tag_fat(_tmp31A,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7FF,_tmp7FE,_tag_fat(_tmp319,sizeof(void*),0U));});});}}}}}}
# 1450
if(emit_warning)
({void*_tmp31B=0U;({unsigned _tmp801=e2->loc;struct _fat_ptr _tmp800=({const char*_tmp31C="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp31C,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp801,_tmp800,_tag_fat(_tmp31B,sizeof(void*),0U));});});}else{
# 1454
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp31D=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp30=_tmp31D;struct _tuple16 _tmp31E=_stmttmp30;int _tmp320;unsigned _tmp31F;_LL11: _tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;_LL12: {unsigned i=_tmp31F;int known=_tmp320;
if(known)
# 1459
({unsigned _tmp804=loc;unsigned _tmp803=i;void*_tmp802=b;Cyc_Tcutil_check_bound(_tmp804,_tmp803,_tmp802,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1462
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp321=0U;({unsigned _tmp806=e1->loc;struct _fat_ptr _tmp805=({const char*_tmp322="subscript applied to pointer to one object";_tag_fat(_tmp322,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp806,_tmp805,_tag_fat(_tmp321,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1467
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp323=0U;({unsigned _tmp808=e1->loc;struct _fat_ptr _tmp807=({const char*_tmp324="can't subscript an abstract pointer";_tag_fat(_tmp324,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp808,_tmp807,_tag_fat(_tmp323,sizeof(void*),0U));});});
return t;}case 6U: _LL3: _tmp305=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp304)->f1;_LL4: {struct Cyc_List_List*ts=_tmp305;
# 1472
struct _tuple16 _tmp325=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp31=_tmp325;struct _tuple16 _tmp326=_stmttmp31;int _tmp328;unsigned _tmp327;_LL14: _tmp327=_tmp326.f1;_tmp328=_tmp326.f2;_LL15: {unsigned i=_tmp327;int known=_tmp328;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp32A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FF;_tmp5FF.tag=0U,({
struct _fat_ptr _tmp809=({const char*_tmp32B="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp32B,sizeof(char),47U);});_tmp5FF.f1=_tmp809;});_tmp5FF;});void*_tmp329[1U];_tmp329[0]=& _tmp32A;({struct Cyc_Tcenv_Tenv*_tmp80B=te;unsigned _tmp80A=loc;Cyc_Tcexp_expr_err(_tmp80B,_tmp80A,0,_tag_fat(_tmp329,sizeof(void*),1U));});});{
struct _handler_cons _tmp32C;_push_handler(& _tmp32C);{int _tmp32E=0;if(setjmp(_tmp32C.handler))_tmp32E=1;if(!_tmp32E){{void*_tmp32F=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp32F;};_pop_handler();}else{void*_tmp32D=(void*)Cyc_Core_get_exn_thrown();void*_tmp330=_tmp32D;void*_tmp331;if(((struct Cyc_List_Nth_exn_struct*)_tmp330)->tag == Cyc_List_Nth){_LL17: _LL18:
# 1478
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp333=({struct Cyc_Warn_String_Warn_Warg_struct _tmp604;_tmp604.tag=0U,({struct _fat_ptr _tmp80C=({const char*_tmp33A="index is ";_tag_fat(_tmp33A,sizeof(char),10U);});_tmp604.f1=_tmp80C;});_tmp604;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp334=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp603;_tmp603.tag=12U,_tmp603.f1=(int)i;_tmp603;});struct Cyc_Warn_String_Warn_Warg_struct _tmp335=({struct Cyc_Warn_String_Warn_Warg_struct _tmp602;_tmp602.tag=0U,({struct _fat_ptr _tmp80D=({const char*_tmp339=" but tuple has only ";_tag_fat(_tmp339,sizeof(char),21U);});_tmp602.f1=_tmp80D;});_tmp602;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp336=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp601;_tmp601.tag=12U,({
int _tmp80E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp601.f1=_tmp80E;});_tmp601;});struct Cyc_Warn_String_Warn_Warg_struct _tmp337=({struct Cyc_Warn_String_Warn_Warg_struct _tmp600;_tmp600.tag=0U,({struct _fat_ptr _tmp80F=({const char*_tmp338=" fields";_tag_fat(_tmp338,sizeof(char),8U);});_tmp600.f1=_tmp80F;});_tmp600;});void*_tmp332[5U];_tmp332[0]=& _tmp333,_tmp332[1]=& _tmp334,_tmp332[2]=& _tmp335,_tmp332[3]=& _tmp336,_tmp332[4]=& _tmp337;({struct Cyc_Tcenv_Tenv*_tmp811=te;unsigned _tmp810=loc;Cyc_Tcexp_expr_err(_tmp811,_tmp810,0,_tag_fat(_tmp332,sizeof(void*),5U));});});}else{_LL19: _tmp331=_tmp330;_LL1A: {void*exn=_tmp331;(int)_rethrow(exn);}}_LL16:;}}}}}default: _LL5: _LL6:
# 1481
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp33C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp606;_tmp606.tag=0U,({struct _fat_ptr _tmp812=({const char*_tmp33E="subscript applied to ";_tag_fat(_tmp33E,sizeof(char),22U);});_tmp606.f1=_tmp812;});_tmp606;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp33D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp605;_tmp605.tag=2U,_tmp605.f1=(void*)t1;_tmp605;});void*_tmp33B[2U];_tmp33B[0]=& _tmp33C,_tmp33B[1]=& _tmp33D;({struct Cyc_Tcenv_Tenv*_tmp815=te;unsigned _tmp814=loc;void**_tmp813=topt;Cyc_Tcexp_expr_err(_tmp815,_tmp814,_tmp813,_tag_fat(_tmp33B,sizeof(void*),2U));});});}_LL0:;}}}
# 1486
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp33F=Cyc_Absyn_compress(*topt);void*_stmttmp32=_tmp33F;void*_tmp340=_stmttmp32;struct Cyc_List_List*_tmp341;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp340)->tag == 6U){_LL1: _tmp341=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp340)->f1;_LL2: {struct Cyc_List_List*ts=_tmp341;
# 1493
if(({int _tmp816=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp816 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
goto _LL0;
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp342=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp342;
({struct Cyc_Tcenv_Tenv*_tmp817=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp817,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1500
({struct Cyc_RgnOrder_RgnPO*_tmp819=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp818=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp819,_tmp818,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp344=_cycalloc(sizeof(*_tmp344));({struct _tuple17*_tmp81A=({struct _tuple17*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp343->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp343;});_tmp344->hd=_tmp81A;}),_tmp344->tl=fields;_tmp344;});}
# 1503
done=1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1507
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp81B=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp81B,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp346=_cycalloc(sizeof(*_tmp346));({struct _tuple17*_tmp81D=({struct _tuple17*_tmp345=_cycalloc(sizeof(*_tmp345));({struct Cyc_Absyn_Tqual _tmp81C=Cyc_Absyn_empty_tqual(0U);_tmp345->f1=_tmp81C;}),_tmp345->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp345;});_tmp346->hd=_tmp81D;}),_tmp346->tl=fields;_tmp346;});}
# 1512
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->tag=6U,({struct Cyc_List_List*_tmp81E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp347->f1=_tmp81E;});_tmp347;});}
# 1516
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1521
struct _tuple9*_tmp348=targ;void*_tmp349;_LL1: _tmp349=_tmp348->f3;_LL2: {void*t=_tmp349;
({unsigned _tmp822=loc;struct Cyc_Tcenv_Tenv*_tmp821=te;struct Cyc_List_List*_tmp820=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp81F=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1522
Cyc_Tctyp_check_type(_tmp822,_tmp821,_tmp820,_tmp81F,1,0,t);});
# 1524
({void*_tmp823=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->tag=36U,_tmp34A->f1=0,_tmp34A->f2=des;_tmp34A;});orig_exp->r=_tmp823;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1537 "/tmp/cyclone/src/tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1540
void*res_t2;
int _tmp34B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp34B;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp34C=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->tag=0U,({union Cyc_Absyn_Cnst _tmp824=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp36A->f1=_tmp824;});_tmp36A;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp34C;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1548
if(zero_term){
struct Cyc_Absyn_Exp*_tmp34D=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp34D;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp34E=0U;({unsigned _tmp826=e->loc;struct _fat_ptr _tmp825=({const char*_tmp34F="zero-terminated array doesn't end with zero.";_tag_fat(_tmp34F,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp826,_tmp825,_tag_fat(_tmp34E,sizeof(void*),0U));});});}
# 1553
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp829=res;struct Cyc_Absyn_Tqual _tmp828=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1554
struct Cyc_Absyn_Exp*_tmp827=sz_exp;Cyc_Absyn_array_type(_tmp829,_tmp828,_tmp827,
# 1556
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1558
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1561
if(!({struct Cyc_RgnOrder_RgnPO*_tmp82B=Cyc_Tcenv_curr_rgnpo(te);void*_tmp82A=res;Cyc_Tcutil_coerce_list(_tmp82B,_tmp82A,es);}))
# 1563
({struct Cyc_Warn_String_Warn_Warg_struct _tmp351=({struct Cyc_Warn_String_Warn_Warg_struct _tmp609;_tmp609.tag=0U,({struct _fat_ptr _tmp82C=({const char*_tmp355="elements of array do not all have the same type (";_tag_fat(_tmp355,sizeof(char),50U);});_tmp609.f1=_tmp82C;});_tmp609;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp352=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp608;_tmp608.tag=2U,_tmp608.f1=(void*)res;_tmp608;});struct Cyc_Warn_String_Warn_Warg_struct _tmp353=({struct Cyc_Warn_String_Warn_Warg_struct _tmp607;_tmp607.tag=0U,({
struct _fat_ptr _tmp82D=({const char*_tmp354=")";_tag_fat(_tmp354,sizeof(char),2U);});_tmp607.f1=_tmp82D;});_tmp607;});void*_tmp350[3U];_tmp350[0]=& _tmp351,_tmp350[1]=& _tmp352,_tmp350[2]=& _tmp353;({unsigned _tmp82E=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;Cyc_Warn_err2(_tmp82E,_tag_fat(_tmp350,sizeof(void*),3U));});});
# 1566
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
void*_tmp356=(void*)ds->hd;void*_stmttmp33=_tmp356;void*_tmp357=_stmttmp33;struct Cyc_Absyn_Exp*_tmp358;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp357)->tag == 1U){_LL1: _LL2:
# 1571
({struct Cyc_Warn_String_Warn_Warg_struct _tmp35A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60A;_tmp60A.tag=0U,({struct _fat_ptr _tmp82F=({const char*_tmp35B="only array index designators are supported";_tag_fat(_tmp35B,sizeof(char),43U);});_tmp60A.f1=_tmp82F;});_tmp60A;});void*_tmp359[1U];_tmp359[0]=& _tmp35A;({unsigned _tmp830=loc;Cyc_Warn_err2(_tmp830,_tag_fat(_tmp359,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _tmp358=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp357)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp358;
# 1573
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp35C=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp34=_tmp35C;struct _tuple16 _tmp35D=_stmttmp34;int _tmp35F;unsigned _tmp35E;_LL6: _tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;_LL7: {unsigned i=_tmp35E;int known=_tmp35F;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp361=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60B;_tmp60B.tag=0U,({struct _fat_ptr _tmp831=({const char*_tmp362="index designator cannot use sizeof or offsetof";_tag_fat(_tmp362,sizeof(char),47U);});_tmp60B.f1=_tmp831;});_tmp60B;});void*_tmp360[1U];_tmp360[0]=& _tmp361;({unsigned _tmp832=e->loc;Cyc_Warn_err2(_tmp832,_tag_fat(_tmp360,sizeof(void*),1U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp364=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60F;_tmp60F.tag=0U,({struct _fat_ptr _tmp833=({const char*_tmp369="expecting index designator ";_tag_fat(_tmp369,sizeof(char),28U);});_tmp60F.f1=_tmp833;});_tmp60F;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp365=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp60E;_tmp60E.tag=12U,_tmp60E.f1=offset;_tmp60E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp366=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D;_tmp60D.tag=0U,({struct _fat_ptr _tmp834=({const char*_tmp368=" but found ";_tag_fat(_tmp368,sizeof(char),12U);});_tmp60D.f1=_tmp834;});_tmp60D;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp367=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp60C;_tmp60C.tag=12U,_tmp60C.f1=(int)i;_tmp60C;});void*_tmp363[4U];_tmp363[0]=& _tmp364,_tmp363[1]=& _tmp365,_tmp363[2]=& _tmp366,_tmp363[3]=& _tmp367;({unsigned _tmp835=e->loc;Cyc_Warn_err2(_tmp835,_tag_fat(_tmp363,sizeof(void*),4U));});});}
goto _LL0;}}}}_LL0:;}}}
# 1583
return res_t2;}struct _tuple19{void**f1;struct Cyc_Absyn_Tqual*f2;void**f3;};
# 1588
static struct _tuple19 Cyc_Tcexp_ptrEltAtts(void**topt){
if(!((unsigned)topt))
return({struct _tuple19 _tmp610;_tmp610.f1=0,_tmp610.f2=0,_tmp610.f3=0;_tmp610;});{
void*_tmp36B=Cyc_Absyn_compress(*topt);void*_stmttmp35=_tmp36B;void*_tmp36C=_stmttmp35;struct Cyc_Absyn_ArrayInfo _tmp36D;struct Cyc_Absyn_PtrInfo _tmp36E;switch(*((int*)_tmp36C)){case 3U: _LL1: _tmp36E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36C)->f1;_LL2: {struct Cyc_Absyn_PtrInfo x=_tmp36E;
# 1593
return({struct _tuple19 _tmp611;({void**_tmp838=({void**_tmp36F=_cycalloc(sizeof(*_tmp36F));*_tmp36F=x.elt_type;_tmp36F;});_tmp611.f1=_tmp838;}),({struct Cyc_Absyn_Tqual*_tmp837=({struct Cyc_Absyn_Tqual*_tmp370=_cycalloc(sizeof(*_tmp370));*_tmp370=x.elt_tq;_tmp370;});_tmp611.f2=_tmp837;}),({void**_tmp836=({void**_tmp371=_cycalloc(sizeof(*_tmp371));*_tmp371=(x.ptr_atts).zero_term;_tmp371;});_tmp611.f3=_tmp836;});_tmp611;});}case 4U: _LL3: _tmp36D=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1;_LL4: {struct Cyc_Absyn_ArrayInfo x=_tmp36D;
return({struct _tuple19 _tmp612;({void**_tmp83B=({void**_tmp372=_cycalloc(sizeof(*_tmp372));*_tmp372=x.elt_type;_tmp372;});_tmp612.f1=_tmp83B;}),({struct Cyc_Absyn_Tqual*_tmp83A=({struct Cyc_Absyn_Tqual*_tmp373=_cycalloc(sizeof(*_tmp373));*_tmp373=x.tq;_tmp373;});_tmp612.f2=_tmp83A;}),({void**_tmp839=({void**_tmp374=_cycalloc(sizeof(*_tmp374));*_tmp374=x.zero_term;_tmp374;});_tmp612.f3=_tmp839;});_tmp612;});}default: _LL5: _LL6:
 return({struct _tuple19 _tmp613;_tmp613.f1=0,_tmp613.f2=0,_tmp613.f3=0;_tmp613;});}_LL0:;}}
# 1600
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1603
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp375=Cyc_Absyn_compress((void*)_check_null(bound->topt));void*_stmttmp36=_tmp375;void*_tmp376=_stmttmp36;void*_tmp377;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376)->f2 != 0){_LL1: _tmp377=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376)->f2)->hd;_LL2: {void*t=_tmp377;
# 1608
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp378=({void*_tmp83C=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp83C,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp378;
b->topt=bound->topt;
bound=b;}
# 1613
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1615
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp615;_tmp615.tag=0U,({struct _fat_ptr _tmp83D=({const char*_tmp37C="expecting unsigned int, found ";_tag_fat(_tmp37C,sizeof(char),31U);});_tmp615.f1=_tmp83D;});_tmp615;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp37B=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp614;_tmp614.tag=3U,_tmp614.f1=(void*)bound->topt;_tmp614;});void*_tmp379[2U];_tmp379[0]=& _tmp37A,_tmp379[1]=& _tmp37B;({unsigned _tmp83E=bound->loc;Cyc_Warn_err2(_tmp83E,_tag_fat(_tmp379,sizeof(void*),2U));});});}_LL0:;}
# 1619
if(!(vd->tq).real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp616;_tmp616.tag=0U,({struct _fat_ptr _tmp83F=({const char*_tmp37F="comprehension index variable is not declared const!";_tag_fat(_tmp37F,sizeof(char),52U);});_tmp616.f1=_tmp83F;});_tmp616;});void*_tmp37D[1U];_tmp37D[0]=& _tmp37E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp37D,sizeof(void*),1U));});
# 1622
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
# 1625
struct _tuple19 _tmp380=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp37=_tmp380;struct _tuple19 _tmp381=_stmttmp37;void**_tmp384;struct Cyc_Absyn_Tqual*_tmp383;void**_tmp382;_LL6: _tmp382=_tmp381.f1;_tmp383=_tmp381.f2;_tmp384=_tmp381.f3;_LL7: {void**topt2=_tmp382;struct Cyc_Absyn_Tqual*tqopt=_tmp383;void**ztopt=_tmp384;
# 1628
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1630
Cyc_Tcexp_check_consume(body->loc,t,body);
if(te->le == 0){
# 1633
if(!Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp386=({struct Cyc_Warn_String_Warn_Warg_struct _tmp617;_tmp617.tag=0U,({struct _fat_ptr _tmp840=({const char*_tmp387="bound is not constant";_tag_fat(_tmp387,sizeof(char),22U);});_tmp617.f1=_tmp840;});_tmp617;});void*_tmp385[1U];_tmp385[0]=& _tmp386;({unsigned _tmp841=bound->loc;Cyc_Warn_err2(_tmp841,_tag_fat(_tmp385,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp389=({struct Cyc_Warn_String_Warn_Warg_struct _tmp618;_tmp618.tag=0U,({struct _fat_ptr _tmp842=({const char*_tmp38A="body is not constant";_tag_fat(_tmp38A,sizeof(char),21U);});_tmp618.f1=_tmp842;});_tmp618;});void*_tmp388[1U];_tmp388[0]=& _tmp389;({unsigned _tmp843=body->loc;Cyc_Warn_err2(_tmp843,_tag_fat(_tmp388,sizeof(void*),1U));});});}
# 1638
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1640
struct Cyc_Absyn_Exp*_tmp38B=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp38B;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1644
Cyc_Tcexp_check_consume(body->loc,(void*)_check_null(body->topt),body);
# 1646
return({void*_tmp846=t;struct Cyc_Absyn_Tqual _tmp845=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp844=bound;Cyc_Absyn_array_type(_tmp846,_tmp845,_tmp844,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1651
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1654
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp38C=Cyc_Absyn_compress((void*)_check_null(bound->topt));void*_stmttmp38=_tmp38C;void*_tmp38D=_stmttmp38;void*_tmp38E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38D)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38D)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38D)->f2 != 0){_LL1: _tmp38E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38D)->f2)->hd;_LL2: {void*t=_tmp38E;
# 1659
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp38F=({void*_tmp847=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp847,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp38F;
b->topt=bound->topt;
bound=b;}
# 1664
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1666
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp391=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp848=({const char*_tmp393="expecting unsigned int, found ";_tag_fat(_tmp393,sizeof(char),31U);});_tmp61A.f1=_tmp848;});_tmp61A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp392=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp619;_tmp619.tag=3U,_tmp619.f1=(void*)bound->topt;_tmp619;});void*_tmp390[2U];_tmp390[0]=& _tmp391,_tmp390[1]=& _tmp392;({unsigned _tmp849=bound->loc;Cyc_Warn_err2(_tmp849,_tag_fat(_tmp390,sizeof(void*),2U));});});}_LL0:;}{
# 1669
struct _tuple19 _tmp394=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp39=_tmp394;struct _tuple19 _tmp395=_stmttmp39;void**_tmp398;struct Cyc_Absyn_Tqual*_tmp397;void**_tmp396;_LL6: _tmp396=_tmp395.f1;_tmp397=_tmp395.f2;_tmp398=_tmp395.f3;_LL7: {void**topt2=_tmp396;struct Cyc_Absyn_Tqual*tqopt=_tmp397;void**ztopt=_tmp398;
# 1672
({unsigned _tmp84C=loc;struct Cyc_Tcenv_Tenv*_tmp84B=te;struct Cyc_List_List*_tmp84A=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp84C,_tmp84B,_tmp84A,& Cyc_Tcutil_tmk,1,1,t);});
if(topt2 != 0)
Cyc_Unify_unify(*topt2,t);
# 1676
if(te->le == 0 && !Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp39A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61B;_tmp61B.tag=0U,({struct _fat_ptr _tmp84D=({const char*_tmp39B="bound is not constant";_tag_fat(_tmp39B,sizeof(char),22U);});_tmp61B.f1=_tmp84D;});_tmp61B;});void*_tmp399[1U];_tmp399[0]=& _tmp39A;({unsigned _tmp84E=bound->loc;Cyc_Warn_err2(_tmp84E,_tag_fat(_tmp399,sizeof(void*),1U));});});
# 1680
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp39D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61C;_tmp61C.tag=0U,({struct _fat_ptr _tmp84F=({const char*_tmp39E="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp39E,sizeof(char),74U);});_tmp61C.f1=_tmp84F;});_tmp61C;});void*_tmp39C[1U];_tmp39C[0]=& _tmp39D;({unsigned _tmp850=loc;Cyc_Warn_err2(_tmp850,_tag_fat(_tmp39C,sizeof(void*),1U));});});
# 1684
return({void*_tmp853=t;struct Cyc_Absyn_Tqual _tmp852=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp851=bound;Cyc_Absyn_array_type(_tmp853,_tmp852,_tmp851,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1698 "/tmp/cyclone/src/tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1702
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp39F=_cycalloc(sizeof(*_tmp39F));*_tmp39F=ad;_tmp39F;});}else{
# 1708
{struct _handler_cons _tmp3A0;_push_handler(& _tmp3A0);{int _tmp3A2=0;if(setjmp(_tmp3A0.handler))_tmp3A2=1;if(!_tmp3A2){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1708
;_pop_handler();}else{void*_tmp3A1=(void*)Cyc_Core_get_exn_thrown();void*_tmp3A3=_tmp3A1;void*_tmp3A4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3A3)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1711
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61E;_tmp61E.tag=0U,({struct _fat_ptr _tmp854=({const char*_tmp3A8="unbound struct/union name ";_tag_fat(_tmp3A8,sizeof(char),27U);});_tmp61E.f1=_tmp854;});_tmp61E;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3A7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp61D;_tmp61D.tag=1U,_tmp61D.f1=*tn;_tmp61D;});void*_tmp3A5[2U];_tmp3A5[0]=& _tmp3A6,_tmp3A5[1]=& _tmp3A7;({unsigned _tmp855=loc;Cyc_Warn_err2(_tmp855,_tag_fat(_tmp3A5,sizeof(void*),2U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp3A4=_tmp3A3;_LL4: {void*exn=_tmp3A4;(int)_rethrow(exn);}}_LL0:;}}}
# 1714
*ad_opt=ad;
*tn=ad->name;}
# 1717
if(ad->impl == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp620;_tmp620.tag=0U,({struct _fat_ptr _tmp856=({const char*_tmp3AC="can't construct abstract ";_tag_fat(_tmp3AC,sizeof(char),26U);});_tmp620.f1=_tmp856;});_tmp620;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3AB=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp61F;_tmp61F.tag=6U,_tmp61F.f1=ad;_tmp61F;});void*_tmp3A9[2U];_tmp3A9[0]=& _tmp3AA,_tmp3A9[1]=& _tmp3AB;({struct Cyc_Tcenv_Tenv*_tmp858=te;unsigned _tmp857=loc;Cyc_Tcexp_expr_err(_tmp858,_tmp857,0,_tag_fat(_tmp3A9,sizeof(void*),2U));});});{
# 1724
struct Cyc_Tcenv_Tenv*_tmp3AD=(int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNew?Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te): te;struct Cyc_Tcenv_Tenv*te2=_tmp3AD;
# 1730
struct _tuple13 _tmp3AE=({struct _tuple13 _tmp62A;({struct Cyc_List_List*_tmp859=Cyc_Tcenv_lookup_type_vars(te2);_tmp62A.f1=_tmp859;}),_tmp62A.f2=Cyc_Core_heap_region;_tmp62A;});struct _tuple13 env=_tmp3AE;
struct Cyc_List_List*_tmp3AF=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp3AF;
struct Cyc_List_List*_tmp3B0=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp3B0;
struct Cyc_List_List*_tmp3B1=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp3B1;
struct Cyc_List_List*_tmp3B2=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp3B2;
struct Cyc_List_List*_tmp3B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp3B3;
void*res_typ;
# 1741
if(topt != 0){
void*_tmp3B4=Cyc_Absyn_compress(*topt);void*_stmttmp3A=_tmp3B4;void*_tmp3B5=_stmttmp3A;struct Cyc_List_List*_tmp3B7;struct Cyc_Absyn_Aggrdecl**_tmp3B6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp3B6=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->f1)->f1).KnownAggr).val;_tmp3B7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp3B6;struct Cyc_List_List*all_typs2=_tmp3B7;
# 1744
if(*adptr2 != *adptr)
goto _LL9;
{struct Cyc_List_List*_tmp3B8=all_typs;struct Cyc_List_List*ats=_tmp3B8;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1750
res_typ=*topt;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 res_typ=({union Cyc_Absyn_AggrInfo _tmp85A=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp85A,all_typs);});}_LL5:;}else{
# 1755
res_typ=({union Cyc_Absyn_AggrInfo _tmp85B=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp85B,all_typs);});}{
# 1758
struct Cyc_List_List*_tmp3B9=*ts;struct Cyc_List_List*user_ex_ts=_tmp3B9;
struct Cyc_List_List*_tmp3BA=exist_typs;struct Cyc_List_List*ex_ts=_tmp3BA;
while(user_ex_ts != 0 && ex_ts != 0){
# 1762
({unsigned _tmp85E=loc;struct Cyc_Tcenv_Tenv*_tmp85D=te2;struct Cyc_List_List*_tmp85C=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp85E,_tmp85D,_tmp85C,& Cyc_Tcutil_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1768
if(user_ex_ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp621;_tmp621.tag=0U,({struct _fat_ptr _tmp85F=({const char*_tmp3BD="too many explicit witness types";_tag_fat(_tmp3BD,sizeof(char),32U);});_tmp621.f1=_tmp85F;});_tmp621;});void*_tmp3BB[1U];_tmp3BB[0]=& _tmp3BC;({unsigned _tmp860=loc;Cyc_Warn_err2(_tmp860,_tag_fat(_tmp3BB,sizeof(void*),1U));});});
# 1771
*ts=exist_typs;{
# 1774
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3BE=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3B=_tmp3BE;struct _tuple20*_tmp3BF=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_Aggrfield*_tmp3C0;_LLB: _tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp3C0;struct Cyc_Absyn_Exp*field_exp=_tmp3C1;
void*_tmp3C2=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp3C2;
({struct Cyc_Tcenv_Tenv*_tmp861=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp861,& inst_fieldtyp,field_exp);});
# 1782
if(!({struct Cyc_RgnOrder_RgnPO*_tmp863=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp862=field_exp;Cyc_Tcutil_coerce_arg(_tmp863,_tmp862,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp629;_tmp629.tag=0U,({struct _fat_ptr _tmp864=({const char*_tmp3CF="field ";_tag_fat(_tmp3CF,sizeof(char),7U);});_tmp629.f1=_tmp864;});_tmp629;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp628;_tmp628.tag=0U,_tmp628.f1=*field->name;_tmp628;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp627;_tmp627.tag=0U,({struct _fat_ptr _tmp865=({const char*_tmp3CE=" of ";_tag_fat(_tmp3CE,sizeof(char),5U);});_tmp627.f1=_tmp865;});_tmp627;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3C7=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp626;_tmp626.tag=6U,_tmp626.f1=ad;_tmp626;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp625;_tmp625.tag=0U,({
struct _fat_ptr _tmp866=({const char*_tmp3CD=" expects type ";_tag_fat(_tmp3CD,sizeof(char),15U);});_tmp625.f1=_tmp866;});_tmp625;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3C9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp624;_tmp624.tag=2U,_tmp624.f1=(void*)inst_fieldtyp;_tmp624;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp623;_tmp623.tag=0U,({struct _fat_ptr _tmp867=({const char*_tmp3CC=" != ";_tag_fat(_tmp3CC,sizeof(char),5U);});_tmp623.f1=_tmp867;});_tmp623;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3CB=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp622;_tmp622.tag=3U,_tmp622.f1=(void*)field_exp->topt;_tmp622;});void*_tmp3C3[8U];_tmp3C3[0]=& _tmp3C4,_tmp3C3[1]=& _tmp3C5,_tmp3C3[2]=& _tmp3C6,_tmp3C3[3]=& _tmp3C7,_tmp3C3[4]=& _tmp3C8,_tmp3C3[5]=& _tmp3C9,_tmp3C3[6]=& _tmp3CA,_tmp3C3[7]=& _tmp3CB;({unsigned _tmp868=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp868,_tag_fat(_tmp3C3,sizeof(void*),8U));});});}}{
# 1787
struct Cyc_List_List*_tmp3D0=0;struct Cyc_List_List*rpo_inst=_tmp3D0;
{struct Cyc_List_List*_tmp3D1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3D1;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct _tuple0*_tmp86B=({struct _tuple0*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({void*_tmp86A=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3D2->f1=_tmp86A;}),({
void*_tmp869=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3D2->f2=_tmp869;});_tmp3D2;});
# 1789
_tmp3D3->hd=_tmp86B;}),_tmp3D3->tl=rpo_inst;_tmp3D3;});}}
# 1792
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}
# 1798
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1800
{void*_tmp3D4=Cyc_Absyn_compress(ts);void*_stmttmp3C=_tmp3D4;void*_tmp3D5=_stmttmp3C;struct Cyc_List_List*_tmp3D7;enum Cyc_Absyn_AggrKind _tmp3D6;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D5)->tag == 7U){_LL1: _tmp3D6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D5)->f1;_tmp3D7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D5)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp3D6;struct Cyc_List_List*fs=_tmp3D7;
# 1802
if((int)k == (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62B;_tmp62B.tag=0U,({struct _fat_ptr _tmp86C=({const char*_tmp3DA="expecting struct but found union";_tag_fat(_tmp3DA,sizeof(char),33U);});_tmp62B.f1=_tmp86C;});_tmp62B;});void*_tmp3D8[1U];_tmp3D8[0]=& _tmp3D9;({unsigned _tmp86D=loc;Cyc_Warn_err2(_tmp86D,_tag_fat(_tmp3D8,sizeof(void*),1U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3DB=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3D=_tmp3DB;struct _tuple20*_tmp3DC=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Aggrfield*_tmp3DD;_LL6: _tmp3DD=_tmp3DC->f1;_tmp3DE=_tmp3DC->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp3DD;struct Cyc_Absyn_Exp*field_exp=_tmp3DE;
({struct Cyc_Tcenv_Tenv*_tmp86F=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp86E=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp86F,_tmp86E,field_exp);});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp871=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp870=field_exp;Cyc_Tcutil_coerce_arg(_tmp871,_tmp870,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp631;_tmp631.tag=0U,({struct _fat_ptr _tmp872=({const char*_tmp3E8="field ";_tag_fat(_tmp3E8,sizeof(char),7U);});_tmp631.f1=_tmp872;});_tmp631;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp630;_tmp630.tag=0U,_tmp630.f1=*field->name;_tmp630;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62F;_tmp62F.tag=0U,({
struct _fat_ptr _tmp873=({const char*_tmp3E7=" of struct expects type ";_tag_fat(_tmp3E7,sizeof(char),25U);});_tmp62F.f1=_tmp873;});_tmp62F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3E3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62E;_tmp62E.tag=2U,_tmp62E.f1=(void*)field->type;_tmp62E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62D;_tmp62D.tag=0U,({struct _fat_ptr _tmp874=({const char*_tmp3E6=" != ";_tag_fat(_tmp3E6,sizeof(char),5U);});_tmp62D.f1=_tmp874;});_tmp62D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3E5=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp62C;_tmp62C.tag=3U,_tmp62C.f1=(void*)field_exp->topt;_tmp62C;});void*_tmp3DF[6U];_tmp3DF[0]=& _tmp3E0,_tmp3DF[1]=& _tmp3E1,_tmp3DF[2]=& _tmp3E2,_tmp3DF[3]=& _tmp3E3,_tmp3DF[4]=& _tmp3E4,_tmp3DF[5]=& _tmp3E5;({unsigned _tmp875=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp875,_tag_fat(_tmp3DF,sizeof(void*),6U));});});}}
# 1815
goto _LL0;}}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp632;_tmp632.tag=0U,({struct _fat_ptr _tmp876=({const char*_tmp3EB="tcAnonStruct: wrong type";_tag_fat(_tmp3EB,sizeof(char),25U);});_tmp632.f1=_tmp876;});_tmp632;});void*_tmp3E9[1U];_tmp3E9[0]=& _tmp3EA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3E9,sizeof(void*),1U));});}_LL0:;}
# 1818
return ts;}
# 1822
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1825
struct _tuple13 _tmp3EC=({struct _tuple13 _tmp63D;({struct Cyc_List_List*_tmp877=Cyc_Tcenv_lookup_type_vars(te);_tmp63D.f1=_tmp877;}),_tmp63D.f2=Cyc_Core_heap_region;_tmp63D;});struct _tuple13 env=_tmp3EC;
struct Cyc_List_List*_tmp3ED=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3ED;
struct Cyc_List_List*_tmp3EE=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp3EE;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp878=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp878,all_typs);});
# 1830
if(topt != 0){
void*_tmp3EF=Cyc_Absyn_compress(*topt);void*_stmttmp3E=_tmp3EF;void*_tmp3F0=_stmttmp3E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F0)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F0)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1836
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp87A=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp879=e;Cyc_Tcutil_coerce_arg(_tmp87A,_tmp879,t,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp638;_tmp638.tag=0U,({struct _fat_ptr _tmp87B=({const char*_tmp3FB="datatype constructor ";_tag_fat(_tmp3FB,sizeof(char),22U);});_tmp638.f1=_tmp87B;});_tmp638;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3F3=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp637;_tmp637.tag=1U,_tmp637.f1=tuf->name;_tmp637;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp636;_tmp636.tag=0U,({
struct _fat_ptr _tmp87C=({const char*_tmp3FA=" expects argument of type ";_tag_fat(_tmp3FA,sizeof(char),27U);});_tmp636.f1=_tmp87C;});_tmp636;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp635;_tmp635.tag=2U,_tmp635.f1=(void*)t;_tmp635;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp634;_tmp634.tag=0U,({struct _fat_ptr _tmp87D=({const char*_tmp3F9=" but this argument has type ";_tag_fat(_tmp3F9,sizeof(char),29U);});_tmp634.f1=_tmp87D;});_tmp634;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,
e->topt == 0?({struct _fat_ptr _tmp87F=(struct _fat_ptr)({const char*_tmp3F8="?";_tag_fat(_tmp3F8,sizeof(char),2U);});_tmp633.f1=_tmp87F;}):({struct _fat_ptr _tmp87E=Cyc_Absynpp_typ2string((void*)_check_null(e->topt));_tmp633.f1=_tmp87E;});_tmp633;});void*_tmp3F1[6U];_tmp3F1[0]=& _tmp3F2,_tmp3F1[1]=& _tmp3F3,_tmp3F1[2]=& _tmp3F4,_tmp3F1[3]=& _tmp3F5,_tmp3F1[4]=& _tmp3F6,_tmp3F1[5]=& _tmp3F7;({unsigned _tmp880=e->loc;Cyc_Warn_err2(_tmp880,_tag_fat(_tmp3F1,sizeof(void*),6U));});});
Cyc_Unify_explain_failure();}}
# 1849
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A;_tmp63A.tag=0U,({
struct _fat_ptr _tmp881=({const char*_tmp3FF="too many arguments for datatype constructor ";_tag_fat(_tmp3FF,sizeof(char),45U);});_tmp63A.f1=_tmp881;});_tmp63A;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3FE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp639;_tmp639.tag=1U,_tmp639.f1=tuf->name;_tmp639;});void*_tmp3FC[2U];_tmp3FC[0]=& _tmp3FD,_tmp3FC[1]=& _tmp3FE;({struct Cyc_Tcenv_Tenv*_tmp884=te;unsigned _tmp883=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp882=topt;Cyc_Tcexp_expr_err(_tmp884,_tmp883,_tmp882,_tag_fat(_tmp3FC,sizeof(void*),2U));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp401=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63C;_tmp63C.tag=0U,({
struct _fat_ptr _tmp885=({const char*_tmp403="too few arguments for datatype constructor ";_tag_fat(_tmp403,sizeof(char),44U);});_tmp63C.f1=_tmp885;});_tmp63C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp402=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp63B;_tmp63B.tag=1U,_tmp63B.f1=tuf->name;_tmp63B;});void*_tmp400[2U];_tmp400[0]=& _tmp401,_tmp400[1]=& _tmp402;({struct Cyc_Tcenv_Tenv*_tmp888=te;unsigned _tmp887=loc;void**_tmp886=topt;Cyc_Tcexp_expr_err(_tmp888,_tmp887,_tmp886,_tag_fat(_tmp400,sizeof(void*),2U));});});
return res;}}
# 1859
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1861
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1864
if(topt == 0)
return 0;{
void*_tmp404=Cyc_Absyn_compress(*topt);void*_stmttmp3F=_tmp404;void*_tmp405=_stmttmp3F;void*_tmp406;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405)->tag == 3U){_LL1: _tmp406=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405)->f1).elt_type;_LL2: {void*elt_typ=_tmp406;
# 1868
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1874
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1876
enum Cyc_Absyn_AliasQual _tmp407=(Cyc_Tcutil_type_kind(Cyc_Absyn_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp40=_tmp407;enum Cyc_Absyn_AliasQual _tmp408=_stmttmp40;if(_tmp408 == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 1882
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 1886
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 1890
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp409=Cyc_Absyn_compress(handle_type);void*_stmttmp41=_tmp409;void*_tmp40A=_stmttmp41;void*_tmp40B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40A)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40A)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40A)->f2 != 0){_LL1: _tmp40B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40A)->f2)->hd;_LL2: {void*r=_tmp40B;
# 1895
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1899
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63F;_tmp63F.tag=0U,({struct _fat_ptr _tmp889=({const char*_tmp40F="expecting region_t type but found ";_tag_fat(_tmp40F,sizeof(char),35U);});_tmp63F.f1=_tmp889;});_tmp63F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp40E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63E;_tmp63E.tag=2U,_tmp63E.f1=(void*)handle_type;_tmp63E;});void*_tmp40C[2U];_tmp40C[0]=& _tmp40D,_tmp40C[1]=& _tmp40E;({unsigned _tmp88A=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;Cyc_Warn_err2(_tmp88A,_tag_fat(_tmp40C,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}else{
# 1905
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp88B=Cyc_Absyn_uniquergn_exp();*ropt=_tmp88B;});}}}
# 1912
({struct Cyc_Tcenv_Tenv*_tmp88C=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp88C,& Cyc_Absyn_uint_type,*e);});{
# 1921 "/tmp/cyclone/src/tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp411=({struct Cyc_Warn_String_Warn_Warg_struct _tmp640;_tmp640.tag=0U,({struct _fat_ptr _tmp88D=({const char*_tmp412="calloc with empty type";_tag_fat(_tmp412,sizeof(char),23U);});_tmp640.f1=_tmp88D;});_tmp640;});void*_tmp410[1U];_tmp410[0]=& _tmp411;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp410,sizeof(void*),1U));});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp890=loc;struct Cyc_Tcenv_Tenv*_tmp88F=te;struct Cyc_List_List*_tmp88E=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp890,_tmp88F,_tmp88E,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp414=({struct Cyc_Warn_String_Warn_Warg_struct _tmp643;_tmp643.tag=0U,({struct _fat_ptr _tmp891=({const char*_tmp418="calloc cannot be used with type ";_tag_fat(_tmp418,sizeof(char),33U);});_tmp643.f1=_tmp891;});_tmp643;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp415=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp642;_tmp642.tag=2U,_tmp642.f1=(void*)elt_type;_tmp642;});struct Cyc_Warn_String_Warn_Warg_struct _tmp416=({struct Cyc_Warn_String_Warn_Warg_struct _tmp641;_tmp641.tag=0U,({
struct _fat_ptr _tmp892=({const char*_tmp417="\n\t(type needs initialization)";_tag_fat(_tmp417,sizeof(char),30U);});_tmp641.f1=_tmp892;});_tmp641;});void*_tmp413[3U];_tmp413[0]=& _tmp414,_tmp413[1]=& _tmp415,_tmp413[2]=& _tmp416;({unsigned _tmp893=loc;Cyc_Warn_err2(_tmp893,_tag_fat(_tmp413,sizeof(void*),3U));});});
num_elts=*e;
one_elt=0;}else{
# 1936
void*er=(*e)->r;
retry_sizeof: {
void*_tmp419=er;struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_Absyn_Exp*_tmp41A;void*_tmp41C;switch(*((int*)_tmp419)){case 17U: _LL6: _tmp41C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp419)->f1;_LL7: {void*t2=_tmp41C;
# 1940
elt_type=t2;
({void**_tmp894=({void**_tmp41D=_cycalloc(sizeof(*_tmp41D));*_tmp41D=elt_type;_tmp41D;});*t=_tmp894;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp419)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp419)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp419)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp419)->f2)->tl)->tl == 0){_LL8: _tmp41A=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp419)->f2)->hd;_tmp41B=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp419)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp41A;struct Cyc_Absyn_Exp*e2=_tmp41B;
# 1947
{struct _tuple0 _tmp41E=({struct _tuple0 _tmp647;_tmp647.f1=e1->r,_tmp647.f2=e2->r;_tmp647;});struct _tuple0 _stmttmp42=_tmp41E;struct _tuple0 _tmp41F=_stmttmp42;void*_tmp420;void*_tmp421;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp41F.f1)->tag == 17U){_LLD: _tmp421=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp41F.f1)->f1;_LLE: {void*t1=_tmp421;
# 1949
e1=e2;
_tmp420=t1;goto _LL10;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp41F.f2)->tag == 17U){_LLF: _tmp420=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp41F.f2)->f1;_LL10: {void*t2=_tmp420;
# 1953
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 1956
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp423=({struct Cyc_Warn_String_Warn_Warg_struct _tmp646;_tmp646.tag=0U,({struct _fat_ptr _tmp895=({const char*_tmp427="malloc cannot be used with type ";_tag_fat(_tmp427,sizeof(char),33U);});_tmp646.f1=_tmp895;});_tmp646;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp424=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp645;_tmp645.tag=2U,_tmp645.f1=(void*)t2;_tmp645;});struct Cyc_Warn_String_Warn_Warg_struct _tmp425=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0U,({
struct _fat_ptr _tmp896=({const char*_tmp426="\n\t(type needs initialization)";_tag_fat(_tmp426,sizeof(char),30U);});_tmp644.f1=_tmp896;});_tmp644;});void*_tmp422[3U];_tmp422[0]=& _tmp423,_tmp422[1]=& _tmp424,_tmp422[2]=& _tmp425;({unsigned _tmp897=loc;Cyc_Warn_err2(_tmp897,_tag_fat(_tmp422,sizeof(void*),3U));});});else{
# 1960
*is_calloc=1;}}
# 1962
elt_type=t2;
({void**_tmp898=({void**_tmp428=_cycalloc(sizeof(*_tmp428));*_tmp428=elt_type;_tmp428;});*t=_tmp898;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 1969
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 1971
 No_sizeof: {
# 1974
struct Cyc_Absyn_Exp*_tmp429=*e;struct Cyc_Absyn_Exp*real_e=_tmp429;
{void*_tmp42A=real_e->r;void*_stmttmp43=_tmp42A;void*_tmp42B=_stmttmp43;struct Cyc_Absyn_Exp*_tmp42C;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42B)->tag == 14U){_LL14: _tmp42C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42B)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp42C;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 1979
{void*_tmp42D=Cyc_Absyn_compress((void*)_check_null(real_e->topt));void*_stmttmp44=_tmp42D;void*_tmp42E=_stmttmp44;void*_tmp42F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42E)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42E)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42E)->f2 != 0){_LL19: _tmp42F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42E)->f2)->hd;_LL1A: {void*tagt=_tmp42F;
# 1981
{void*_tmp430=Cyc_Absyn_compress(tagt);void*_stmttmp45=_tmp430;void*_tmp431=_stmttmp45;struct Cyc_Absyn_Exp*_tmp432;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp431)->tag == 9U){_LL1E: _tmp432=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp431)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp432;
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 1985
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 1988
elt_type=Cyc_Absyn_char_type;
({void**_tmp899=({void**_tmp433=_cycalloc(sizeof(*_tmp433));*_tmp433=elt_type;_tmp433;});*t=_tmp899;});
num_elts=*e;
one_elt=0;}
# 1993
goto _LL5;}_LL5:;}}
# 1997
*is_fat=!one_elt;
# 2000
{void*_tmp434=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp435;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp435=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp435;
# 2002
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp437=({struct Cyc_Warn_String_Warn_Warg_struct _tmp648;_tmp648.tag=0U,({struct _fat_ptr _tmp89A=({const char*_tmp438="malloc with existential types not yet implemented";_tag_fat(_tmp438,sizeof(char),50U);});_tmp648.f1=_tmp89A;});_tmp648;});void*_tmp436[1U];_tmp436[0]=& _tmp437;({unsigned _tmp89B=loc;Cyc_Warn_err2(_tmp89B,_tag_fat(_tmp436,sizeof(void*),1U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2009
void*(*_tmp439)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp439;
void*_tmp43A=Cyc_Absyn_false_type;void*zero_term=_tmp43A;
if(topt != 0){
void*_tmp43B=Cyc_Absyn_compress(*topt);void*_stmttmp46=_tmp43B;void*_tmp43C=_stmttmp46;void*_tmp43F;void*_tmp43E;void*_tmp43D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43C)->tag == 3U){_LL28: _tmp43D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43C)->f1).ptr_atts).nullable;_tmp43E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43C)->f1).ptr_atts).bounds;_tmp43F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43C)->f1).ptr_atts).zero_term;_LL29: {void*n=_tmp43D;void*b=_tmp43E;void*zt=_tmp43F;
# 2014
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2019
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp441=({struct Cyc_Warn_String_Warn_Warg_struct _tmp649;_tmp649.tag=0U,({struct _fat_ptr _tmp89C=({const char*_tmp442="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp442,sizeof(char),55U);});_tmp649.f1=_tmp89C;});_tmp649;});void*_tmp440[1U];_tmp440[0]=& _tmp441;({unsigned _tmp89D=loc;Cyc_Warn_warn2(_tmp89D,_tag_fat(_tmp440,sizeof(void*),1U));});});
*is_calloc=1;}{
# 2025
struct Cyc_Absyn_Exp*_tmp443=({void*_tmp89E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp89E,b);});struct Cyc_Absyn_Exp*eopt=_tmp443;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp444=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp444;
int _tmp445=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp445;
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp8A2=elt_type;void*_tmp8A1=rgn;struct Cyc_Absyn_Tqual _tmp8A0=Cyc_Absyn_empty_tqual(0U);void*_tmp89F=b;Cyc_Absyn_atb_type(_tmp8A2,_tmp8A1,_tmp8A0,_tmp89F,zero_term);});}{
# 2033
void*_tmp446=Cyc_Absyn_compress((void*)_check_null(num_elts->topt));void*_stmttmp47=_tmp446;void*_tmp447=_stmttmp47;void*_tmp448;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f2 != 0){_LL2D: _tmp448=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f2)->hd;_LL2E: {void*tagtyp=_tmp448;
# 2035
struct Cyc_Absyn_Exp*_tmp449=({void*_tmp8A3=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp8A3,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp449;
# 2037
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp8A7=elt_type;void*_tmp8A6=rgn;struct Cyc_Absyn_Tqual _tmp8A5=Cyc_Absyn_empty_tqual(0U);void*_tmp8A4=b;Cyc_Absyn_atb_type(_tmp8A7,_tmp8A6,_tmp8A5,_tmp8A4,zero_term);});}
# 2041
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2045
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2048
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp8AB)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=ptr_maker;void*_tmp8AA=elt_type;void*_tmp8A9=rgn;struct Cyc_Absyn_Tqual _tmp8A8=Cyc_Absyn_empty_tqual(0U);_tmp8AB(_tmp8AA,_tmp8A9,_tmp8A8,zero_term);});}}}
# 2053
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2055
struct Cyc_Tcenv_Tenv*_tmp44A=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp44A;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp44B=(void*)_check_null(e1->topt);void*t1=_tmp44B;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2062
{void*_tmp44C=Cyc_Absyn_compress(t1);void*_stmttmp48=_tmp44C;void*_tmp44D=_stmttmp48;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44D)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64A;_tmp64A.tag=0U,({struct _fat_ptr _tmp8AC=({const char*_tmp450="cannot assign to an array";_tag_fat(_tmp450,sizeof(char),26U);});_tmp64A.f1=_tmp8AC;});_tmp64A;});void*_tmp44E[1U];_tmp44E[0]=& _tmp44F;({unsigned _tmp8AD=loc;Cyc_Warn_err2(_tmp8AD,_tag_fat(_tmp44E,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2067
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp452=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64B;_tmp64B.tag=0U,({struct _fat_ptr _tmp8AE=({const char*_tmp453="swap not allowed for non-pointer or non-word-sized types";_tag_fat(_tmp453,sizeof(char),57U);});_tmp64B.f1=_tmp8AE;});_tmp64B;});void*_tmp451[1U];_tmp451[0]=& _tmp452;({unsigned _tmp8AF=loc;Cyc_Warn_err2(_tmp8AF,_tag_fat(_tmp451,sizeof(void*),1U));});});
# 2071
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp455=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64C;_tmp64C.tag=0U,({struct _fat_ptr _tmp8B0=({const char*_tmp456="swap non-lvalue";_tag_fat(_tmp456,sizeof(char),16U);});_tmp64C.f1=_tmp8B0;});_tmp64C;});void*_tmp454[1U];_tmp454[0]=& _tmp455;({struct Cyc_Tcenv_Tenv*_tmp8B3=te;unsigned _tmp8B2=e1->loc;void**_tmp8B1=topt;Cyc_Tcexp_expr_err(_tmp8B3,_tmp8B2,_tmp8B1,_tag_fat(_tmp454,sizeof(void*),1U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp458=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64D;_tmp64D.tag=0U,({struct _fat_ptr _tmp8B4=({const char*_tmp459="swap non-lvalue";_tag_fat(_tmp459,sizeof(char),16U);});_tmp64D.f1=_tmp8B4;});_tmp64D;});void*_tmp457[1U];_tmp457[0]=& _tmp458;({struct Cyc_Tcenv_Tenv*_tmp8B7=te;unsigned _tmp8B6=e2->loc;void**_tmp8B5=topt;Cyc_Tcexp_expr_err(_tmp8B7,_tmp8B6,_tmp8B5,_tag_fat(_tmp457,sizeof(void*),1U));});});
# 2076
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp45B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp651;_tmp651.tag=0U,({struct _fat_ptr _tmp8B8=({const char*_tmp460="type mismatch: ";_tag_fat(_tmp460,sizeof(char),16U);});_tmp651.f1=_tmp8B8;});_tmp651;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp45C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp650;_tmp650.tag=2U,_tmp650.f1=(void*)t1;_tmp650;});struct Cyc_Warn_String_Warn_Warg_struct _tmp45D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64F;_tmp64F.tag=0U,({struct _fat_ptr _tmp8B9=({const char*_tmp45F=" != ";_tag_fat(_tmp45F,sizeof(char),5U);});_tmp64F.f1=_tmp8B9;});_tmp64F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp45E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64E;_tmp64E.tag=2U,_tmp64E.f1=(void*)t2;_tmp64E;});void*_tmp45A[4U];_tmp45A[0]=& _tmp45B,_tmp45A[1]=& _tmp45C,_tmp45A[2]=& _tmp45D,_tmp45A[3]=& _tmp45E;({struct Cyc_Tcenv_Tenv*_tmp8BC=te;unsigned _tmp8BB=loc;void**_tmp8BA=topt;Cyc_Tcexp_expr_err(_tmp8BC,_tmp8BB,_tmp8BA,_tag_fat(_tmp45A,sizeof(void*),4U));});});
return Cyc_Absyn_void_type;}}}
# 2084
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp8BD=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp8BD,s,1);});
# 2087
while(1){
void*_tmp461=s->r;void*_stmttmp49=_tmp461;void*_tmp462=_stmttmp49;struct Cyc_Absyn_Stmt*_tmp463;struct Cyc_Absyn_Stmt*_tmp464;struct Cyc_Absyn_Exp*_tmp465;switch(*((int*)_tmp462)){case 1U: _LL1: _tmp465=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp462)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp465;
# 2091
void*_tmp466=(void*)_check_null(e->topt);void*t=_tmp466;
if(!({void*_tmp8BE=t;Cyc_Unify_unify(_tmp8BE,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp468=({struct Cyc_Warn_String_Warn_Warg_struct _tmp653;_tmp653.tag=0U,({struct _fat_ptr _tmp8BF=({const char*_tmp46A="statement expression returns type ";_tag_fat(_tmp46A,sizeof(char),35U);});_tmp653.f1=_tmp8BF;});_tmp653;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp469=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp652;_tmp652.tag=2U,_tmp652.f1=(void*)t;_tmp652;});void*_tmp467[2U];_tmp467[0]=& _tmp468,_tmp467[1]=& _tmp469;({unsigned _tmp8C0=loc;Cyc_Tcexp_err_and_explain(_tmp8C0,_tag_fat(_tmp467,sizeof(void*),2U));});});
return t;}case 2U: _LL3: _tmp464=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp462)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp464;
s=s2;continue;}case 12U: _LL5: _tmp463=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp462)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp463;
s=s1;continue;}default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp46C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp654;_tmp654.tag=0U,({
struct _fat_ptr _tmp8C1=({const char*_tmp46D="statement expression must end with expression";_tag_fat(_tmp46D,sizeof(char),46U);});_tmp654.f1=_tmp8C1;});_tmp654;});void*_tmp46B[1U];_tmp46B[0]=& _tmp46C;({struct Cyc_Tcenv_Tenv*_tmp8C4=te;unsigned _tmp8C3=loc;void**_tmp8C2=topt;Cyc_Tcexp_expr_err(_tmp8C4,_tmp8C3,_tmp8C2,_tag_fat(_tmp46B,sizeof(void*),1U));});});}_LL0:;}}
# 2102
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Absyn_compress(({struct Cyc_Tcenv_Tenv*_tmp8C5=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp8C5,0,e);}));
{void*_tmp46E=t;struct Cyc_Absyn_Aggrdecl*_tmp46F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46E)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46E)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp46F=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46E)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp46F;
# 2106
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp471=({struct Cyc_Warn_String_Warn_Warg_struct _tmp656;_tmp656.tag=0U,({struct _fat_ptr _tmp8C6=({const char*_tmp473="expecting @tagged union but found ";_tag_fat(_tmp473,sizeof(char),35U);});_tmp656.f1=_tmp8C6;});_tmp656;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp472=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp655;_tmp655.tag=2U,_tmp655.f1=(void*)t;_tmp655;});void*_tmp470[2U];_tmp470[0]=& _tmp471,_tmp470[1]=& _tmp472;({unsigned _tmp8C7=loc;Cyc_Warn_err2(_tmp8C7,_tag_fat(_tmp470,sizeof(void*),2U));});});goto _LL0;}_LL0:;}
# 2111
return Cyc_Absyn_uint_type;}
# 2114
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
({struct Cyc_Tcenv_Tenv*_tmp8C9=te;struct Cyc_Absyn_Exp*_tmp8C8=e;Cyc_Tcexp_tcTest(_tmp8C9,_tmp8C8,({const char*_tmp474="@assert";_tag_fat(_tmp474,sizeof(char),8U);}));});
if(!Cyc_Absyn_no_side_effects_exp(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp476=({struct Cyc_Warn_String_Warn_Warg_struct _tmp657;_tmp657.tag=0U,({struct _fat_ptr _tmp8CA=({const char*_tmp477="@assert expression may have side effects";_tag_fat(_tmp477,sizeof(char),41U);});_tmp657.f1=_tmp8CA;});_tmp657;});void*_tmp475[1U];_tmp475[0]=& _tmp476;({unsigned _tmp8CB=loc;Cyc_Warn_err2(_tmp8CB,_tag_fat(_tmp475,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;}
# 2122
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2126
void*rgn=Cyc_Absyn_heap_rgn_type;
te=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2131
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp478=Cyc_Absyn_compress(handle_type);void*_stmttmp4A=_tmp478;void*_tmp479=_stmttmp4A;void*_tmp47A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479)->f2 != 0){_LL1: _tmp47A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479)->f2)->hd;_LL2: {void*r=_tmp47A;
# 2136
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2140
({struct Cyc_Warn_String_Warn_Warg_struct _tmp47C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp659;_tmp659.tag=0U,({struct _fat_ptr _tmp8CC=({const char*_tmp47E="expecting region_t type but found ";_tag_fat(_tmp47E,sizeof(char),35U);});_tmp659.f1=_tmp8CC;});_tmp659;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp47D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp658;_tmp658.tag=2U,_tmp658.f1=(void*)handle_type;_tmp658;});void*_tmp47B[2U];_tmp47B[0]=& _tmp47C,_tmp47B[1]=& _tmp47D;({unsigned _tmp8CD=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;Cyc_Warn_err2(_tmp8CD,_tag_fat(_tmp47B,sizeof(void*),2U));});});
# 2142
goto _LL0;}_LL0:;}else{
# 2144
if(topt != 0){
# 2147
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)
({struct Cyc_Absyn_Exp*_tmp8CE=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp8CE;});}}}{
# 2155
void*_tmp47F=e1->r;void*_stmttmp4B=_tmp47F;void*_tmp480=_stmttmp4B;struct Cyc_List_List*_tmp481;struct Cyc_List_List*_tmp483;struct Cyc_Core_Opt*_tmp482;switch(*((int*)_tmp480)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2160
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp485=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65A;_tmp65A.tag=0U,({struct _fat_ptr _tmp8CF=({const char*_tmp486="tcNew: comprehension returned non-array type";_tag_fat(_tmp486,sizeof(char),45U);});_tmp65A.f1=_tmp8CF;});_tmp65A;});void*_tmp484[1U];_tmp484[0]=& _tmp485;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp484,sizeof(void*),1U));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8D2=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8D1=loc;void*_tmp8D0=res_typ;Cyc_Tcutil_silent_castable(_tmp8D2,_tmp8D1,_tmp8D0,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2172
return res_typ;}case 36U: _LLA: _tmp482=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp480)->f1;_tmp483=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp480)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp482;struct Cyc_List_List*des=_tmp483;
# 2174
({void*_tmp8D3=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487->tag=26U,_tmp487->f1=des;_tmp487;});e1->r=_tmp8D3;});
_tmp481=des;goto _LLD;}case 26U: _LLC: _tmp481=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp480)->f1;_LLD: {struct Cyc_List_List*des=_tmp481;
# 2177
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp488=Cyc_Absyn_compress(*topt);void*_stmttmp4C=_tmp488;void*_tmp489=_stmttmp4C;void*_tmp48B;void**_tmp48A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->tag == 3U){_LL15: _tmp48A=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->f1).elt_type;_tmp48B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp48A;void*zt=_tmp48B;
# 2183
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2189
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp48D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65B;_tmp65B.tag=0U,({struct _fat_ptr _tmp8D4=({const char*_tmp48E="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp48E,sizeof(char),50U);});_tmp65B.f1=_tmp8D4;});_tmp65B;});void*_tmp48C[1U];_tmp48C[0]=& _tmp48D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp48C,sizeof(void*),1U));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2198
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8D7=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8D6=loc;void*_tmp8D5=res_typ;Cyc_Tcutil_silent_castable(_tmp8D7,_tmp8D6,_tmp8D5,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2205
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp480)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2210
void*_tmp48F=({void*_tmp8DB=Cyc_Absyn_char_type;void*_tmp8DA=rgn;struct Cyc_Absyn_Tqual _tmp8D9=Cyc_Absyn_const_tqual(0U);void*_tmp8D8=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8DB,_tmp8DA,_tmp8D9,_tmp8D8,Cyc_Absyn_true_type);});void*topt2=_tmp48F;
# 2212
void*_tmp490=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp490;
return({void*_tmp8DF=t;void*_tmp8DE=rgn;struct Cyc_Absyn_Tqual _tmp8DD=Cyc_Absyn_empty_tqual(0U);void*_tmp8DC=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2213
Cyc_Absyn_atb_type(_tmp8DF,_tmp8DE,_tmp8DD,_tmp8DC,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2217
void*_tmp491=({void*_tmp8E3=Cyc_Absyn_wchar_type();void*_tmp8E2=rgn;struct Cyc_Absyn_Tqual _tmp8E1=Cyc_Absyn_const_tqual(0U);void*_tmp8E0=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8E3,_tmp8E2,_tmp8E1,_tmp8E0,Cyc_Absyn_true_type);});void*topt2=_tmp491;
# 2219
void*_tmp492=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp492;
return({void*_tmp8E7=t;void*_tmp8E6=rgn;struct Cyc_Absyn_Tqual _tmp8E5=Cyc_Absyn_empty_tqual(0U);void*_tmp8E4=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2220
Cyc_Absyn_atb_type(_tmp8E7,_tmp8E6,_tmp8E5,_tmp8E4,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2226
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp493=Cyc_Absyn_compress(*topt);void*_stmttmp4D=_tmp493;void*_tmp494=_stmttmp4D;struct Cyc_Absyn_Tqual _tmp496;void**_tmp495;switch(*((int*)_tmp494)){case 3U: _LL1A: _tmp495=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp494)->f1).elt_type;_tmp496=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp494)->f1).elt_tq;_LL1B: {void**elttype=_tmp495;struct Cyc_Absyn_Tqual tq=_tmp496;
# 2232
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp494)->f1)->tag == 18U){_LL1C: _LL1D:
# 2236
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2242
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2244
Cyc_Tcexp_check_consume(e1->loc,telt,e1);{
struct Cyc_Absyn_PtrInfo _tmp497=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp497;
pi.elt_type=telt;
(pi.ptr_atts).rgn=rgn;
({void*_tmp8E8=Cyc_Absyn_bounds_one();(pi.ptr_atts).bounds=_tmp8E8;});
(pi.ptr_atts).zero_term=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp8EB=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8EA=loc;void*_tmp8E9=res_typ;Cyc_Tcutil_silent_castable(_tmp8EB,_tmp8EA,_tmp8E9,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}
# 2257
return res_typ;}}}}}_LL5:;}}
# 2263
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp8ED=t=({void*_tmp8EC=t;Cyc_Tcutil_promote_array(_tmp8EC,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp8ED;});
return t;}
# 2273
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2277
Cyc_Tcexp_check_consume(e->loc,t,e);{
void*_tmp498=e->r;void*_stmttmp4E=_tmp498;void*_tmp499=_stmttmp4E;switch(*((int*)_tmp499)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp499)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2285
 if(Cyc_Tcutil_is_array_type(t))
({void*_tmp8EF=t=({void*_tmp8EE=t;Cyc_Tcutil_promote_array(_tmp8EE,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp8EF;});
return t;}_LL0:;}}
# 2298 "/tmp/cyclone/src/tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*t=({void*_tmp8F0=Cyc_Absyn_compress(Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0));e->topt=_tmp8F0;});
# 2302
{void*_tmp49A=e->r;void*_stmttmp4F=_tmp49A;void*_tmp49B=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp49C;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp49B)->tag == 12U){_LL1: _tmp49C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp49B)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp49C;
return t;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2307
void*_tmp49D=t;void*_tmp49E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->tag == 3U){_LL6: _tmp49E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->f1).elt_type;_LL7: {void*t2=_tmp49E;
# 2309
void*_tmp49F=Cyc_Absyn_compress(t2);void*_stmttmp50=_tmp49F;void*_tmp4A0=_stmttmp50;struct Cyc_Absyn_FnInfo _tmp4A1;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A0)->tag == 5U){_LLB: _tmp4A1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A0)->f1;_LLC: {struct Cyc_Absyn_FnInfo info=_tmp4A1;
# 2311
struct _tuple13 _tmp4A2=({struct _tuple13 _tmp65C;({struct Cyc_List_List*_tmp8F1=Cyc_Tcenv_lookup_type_vars(te);_tmp65C.f1=_tmp8F1;}),_tmp65C.f2=Cyc_Core_heap_region;_tmp65C;});struct _tuple13 env=_tmp4A2;
struct Cyc_List_List*_tmp4A3=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);struct Cyc_List_List*inst=_tmp4A3;
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple14*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CurRgn_inst_curr_rgn,te,inst);{
struct Cyc_List_List*_tmp4A4=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*ts=_tmp4A4;
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp4A5;
({void*_tmp8F2=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->tag=13U,_tmp4A6->f1=inner,_tmp4A6->f2=ts;_tmp4A6;});e->r=_tmp8F2;});
({void*_tmp8F3=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_tmp8F3;});
return(void*)_check_null(e->topt);}}}else{_LLD: _LLE:
 return t;}_LLA:;}}else{_LL8: _LL9:
# 2321
 return t;}_LL5:;}}}
# 2330
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2332
struct Cyc_List_List*_tmp4A7=0;struct Cyc_List_List*decls=_tmp4A7;
# 2334
{void*_tmp4A8=fn_exp->r;void*_stmttmp51=_tmp4A8;void*_tmp4A9=_stmttmp51;struct Cyc_List_List*_tmp4AA;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4A9)->tag == 10U){_LL1: _tmp4AA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4A9)->f2;_LL2: {struct Cyc_List_List*es=_tmp4AA;
# 2336
{void*_tmp4AB=e->r;void*_stmttmp52=_tmp4AB;void*_tmp4AC=_stmttmp52;struct Cyc_List_List*_tmp4AD;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AC)->tag == 10U){_LL6: _tmp4AD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AC)->f2;_LL7: {struct Cyc_List_List*es2=_tmp4AD;
# 2338
struct Cyc_List_List*_tmp4AE=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp4AE;
int _tmp4AF=0;int arg_cnt=_tmp4AF;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp661;_tmp661.tag=0U,({struct _fat_ptr _tmp8F4=({const char*_tmp4B8="bad count ";_tag_fat(_tmp4B8,sizeof(char),11U);});_tmp661.f1=_tmp8F4;});_tmp661;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4B2=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp660;_tmp660.tag=12U,_tmp660.f1=arg_cnt;_tmp660;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4B3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65F;_tmp65F.tag=0U,({struct _fat_ptr _tmp8F5=({const char*_tmp4B7="/";_tag_fat(_tmp4B7,sizeof(char),2U);});_tmp65F.f1=_tmp8F5;});_tmp65F;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4B4=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp65E;_tmp65E.tag=12U,_tmp65E.f1=(int)arg_exps->hd;_tmp65E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65D;_tmp65D.tag=0U,({
struct _fat_ptr _tmp8F6=({const char*_tmp4B6=" for alias coercion!";_tag_fat(_tmp4B6,sizeof(char),21U);});_tmp65D.f1=_tmp8F6;});_tmp65D;});void*_tmp4B0[5U];_tmp4B0[0]=& _tmp4B1,_tmp4B0[1]=& _tmp4B2,_tmp4B0[2]=& _tmp4B3,_tmp4B0[3]=& _tmp4B4,_tmp4B0[4]=& _tmp4B5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4B0,sizeof(void*),5U));});
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2350
struct _tuple12 _tmp4B9=({struct Cyc_Absyn_Exp*_tmp8F7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp8F7,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp53=_tmp4B9;struct _tuple12 _tmp4BA=_stmttmp53;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Decl*_tmp4BB;_LLB: _tmp4BB=_tmp4BA.f1;_tmp4BC=_tmp4BA.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp4BB;struct Cyc_Absyn_Exp*ve=_tmp4BC;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->hd=d,_tmp4BD->tl=decls;_tmp4BD;});
arg_exps=arg_exps->tl;}}}
# 2355
goto _LL5;}}else{_LL8: _LL9:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp662;_tmp662.tag=0U,({struct _fat_ptr _tmp8F8=({const char*_tmp4C0="not a function call!";_tag_fat(_tmp4C0,sizeof(char),21U);});_tmp662.f1=_tmp8F8;});_tmp662;});void*_tmp4BE[1U];_tmp4BE[0]=& _tmp4BF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4BE,sizeof(void*),1U));});}_LL5:;}
# 2358
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp663;_tmp663.tag=0U,({struct _fat_ptr _tmp8F9=({const char*_tmp4C3="not a function call!";_tag_fat(_tmp4C3,sizeof(char),21U);});_tmp663.f1=_tmp8F9;});_tmp663;});void*_tmp4C1[1U];_tmp4C1[0]=& _tmp4C2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4C1,sizeof(void*),1U));});}_LL0:;}
# 2362
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp4C4=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp4C4;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp8FC=({struct Cyc_Absyn_Decl*_tmp8FB=d;struct Cyc_Absyn_Stmt*_tmp8FA=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp8FB,_tmp8FA,e->loc);});Cyc_Absyn_stmt_exp(_tmp8FC,e->loc);});
decls=decls->tl;}
# 2368
e->topt=0;
e->r=fn_exp->r;}
# 2373
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2377
{void*_tmp4C5=e->r;void*_stmttmp54=_tmp4C5;void*_tmp4C6=_stmttmp54;void*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C9;struct _fat_ptr*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;int*_tmp4D2;struct Cyc_Absyn_Exp**_tmp4D1;void***_tmp4D0;struct Cyc_Absyn_Exp**_tmp4CF;int*_tmp4CE;struct Cyc_Absyn_Enumfield*_tmp4D4;void*_tmp4D3;struct Cyc_Absyn_Enumfield*_tmp4D6;struct Cyc_Absyn_Enumdecl*_tmp4D5;struct Cyc_Absyn_Datatypefield*_tmp4D9;struct Cyc_Absyn_Datatypedecl*_tmp4D8;struct Cyc_List_List*_tmp4D7;struct Cyc_List_List*_tmp4DB;void*_tmp4DA;struct Cyc_Absyn_Aggrdecl**_tmp4DF;struct Cyc_List_List*_tmp4DE;struct Cyc_List_List**_tmp4DD;struct _tuple1**_tmp4DC;int*_tmp4E2;void*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;int*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Vardecl*_tmp4E3;struct Cyc_Absyn_Stmt*_tmp4E7;struct Cyc_List_List*_tmp4E8;struct Cyc_List_List*_tmp4EA;struct _tuple9*_tmp4E9;struct Cyc_List_List*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;int*_tmp4F1;int*_tmp4F0;struct _fat_ptr*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;int*_tmp4F5;int*_tmp4F4;struct _fat_ptr*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_List_List*_tmp4F8;void*_tmp4F7;void*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Exp**_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FD;enum Cyc_Absyn_Coercion*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;void*_tmp4FE;struct Cyc_List_List*_tmp502;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp*_tmp508;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Core_Opt*_tmp50E;struct Cyc_Absyn_Exp*_tmp50D;enum Cyc_Absyn_Incrementor _tmp511;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_List_List*_tmp513;enum Cyc_Absyn_Primop _tmp512;void**_tmp514;union Cyc_Absyn_Cnst*_tmp515;struct Cyc_List_List*_tmp517;struct Cyc_Core_Opt*_tmp516;struct Cyc_Absyn_VarargCallInfo**_tmp51A;struct Cyc_List_List*_tmp519;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp51B;switch(*((int*)_tmp4C6)){case 12U: _LL1: _tmp51B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp51B;
# 2382
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=(void*)_check_null(e2->topt);
return;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C6)->f4 == 0){_LL3: _LL4:
# 2386
({struct Cyc_Warn_String_Warn_Warg_struct _tmp51D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp664;_tmp664.tag=0U,({struct _fat_ptr _tmp8FD=({const char*_tmp51E="unresolved function: tcExpNoInst";_tag_fat(_tmp51E,sizeof(char),33U);});_tmp664.f1=_tmp8FD;});_tmp664;});void*_tmp51C[1U];_tmp51C[0]=& _tmp51D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp51C,sizeof(void*),1U));});}else{_LL1B: _tmp518=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp519=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp51A=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp518;struct Cyc_List_List*es=_tmp519;struct Cyc_Absyn_VarargCallInfo**vci=_tmp51A;
# 2409
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp51F;_push_handler(& _tmp51F);{int _tmp521=0;if(setjmp(_tmp51F.handler))_tmp521=1;if(!_tmp521){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp520=(void*)Cyc_Core_get_exn_thrown();void*_tmp522=_tmp520;void*_tmp523;if(((struct Cyc_Core_Failure_exn_struct*)_tmp522)->tag == Cyc_Core_Failure){_LL5A: _LL5B:
# 2414
 ok=0;
fn_exp=e;
goto _LL59;}else{_LL5C: _tmp523=_tmp522;_LL5D: {void*exn=_tmp523;(int)_rethrow(exn);}}_LL59:;}}}
# 2418
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2425
goto _LL0;}}case 36U: _LL5: _tmp516=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp517=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp516;struct Cyc_List_List*des=_tmp517;
# 2390
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp515=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp515;
# 2394
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp514=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LLA: {void**b=_tmp514;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp512=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp513=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp512;struct Cyc_List_List*es=_tmp513;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp510=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp511=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp510;enum Cyc_Absyn_Incrementor i=_tmp511;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp50D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp50E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp50F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp50D;struct Cyc_Core_Opt*popt=_tmp50E;struct Cyc_Absyn_Exp*e2=_tmp50F;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp50A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp50B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp50C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp50A;struct Cyc_Absyn_Exp*e2=_tmp50B;struct Cyc_Absyn_Exp*e3=_tmp50C;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp508=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp509=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp508;struct Cyc_Absyn_Exp*e2=_tmp509;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp506=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp507=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp506;struct Cyc_Absyn_Exp*e2=_tmp507;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp504=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp505=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp504;struct Cyc_Absyn_Exp*e2=_tmp505;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp503=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp503;
# 2426
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp501=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp502=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp501;struct Cyc_List_List*ts=_tmp502;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp4FE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4FF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp500=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C6)->f4;_LL22: {void*t1=_tmp4FE;struct Cyc_Absyn_Exp*e1=_tmp4FF;enum Cyc_Absyn_Coercion*c=_tmp500;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp4FD=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp4FD;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp4FB=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4FC=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp4FB;struct Cyc_Absyn_Exp*e1=_tmp4FC;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp4FA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp4FA;
_tmp4F9=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17U: _LL29: _tmp4F9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL2A: {void*t1=_tmp4F9;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp4F7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4F8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL2C: {void*t1=_tmp4F7;struct Cyc_List_List*l=_tmp4F8;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp4F6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp4F6;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp4F2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4F3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp4F4=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_tmp4F5=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C6)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp4F2;struct _fat_ptr*f=_tmp4F3;int*is_tagged=_tmp4F4;int*is_read=_tmp4F5;
# 2436
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp4EE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4EF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp4F0=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_tmp4F1=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C6)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp4EE;struct _fat_ptr*f=_tmp4EF;int*is_tagged=_tmp4F0;int*is_read=_tmp4F1;
# 2438
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp4EC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4ED=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp4EC;struct Cyc_Absyn_Exp*e2=_tmp4ED;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp4EB=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL36: {struct Cyc_List_List*es=_tmp4EB;
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp4E9=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4EA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL38: {struct _tuple9*t1=_tmp4E9;struct Cyc_List_List*des=_tmp4EA;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp4E8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL3A: {struct Cyc_List_List*des=_tmp4E8;
# 2445
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp524=Cyc_Absyn_compress(*topt);void*_stmttmp55=_tmp524;void*_tmp525=_stmttmp55;void*_tmp528;struct Cyc_Absyn_Tqual*_tmp527;void**_tmp526;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp525)->tag == 4U){_LL5F: _tmp526=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp525)->f1).elt_type;_tmp527=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp525)->f1).tq;_tmp528=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp525)->f1).zero_term;_LL60: {void**et=_tmp526;struct Cyc_Absyn_Tqual*etq=_tmp527;void*zt=_tmp528;
# 2451
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}
# 2457
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp4E7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp4E7;
# 2459
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp4E3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4E4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp4E5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_tmp4E6=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C6)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp4E3;struct Cyc_Absyn_Exp*e1=_tmp4E4;struct Cyc_Absyn_Exp*e2=_tmp4E5;int*iszeroterm=_tmp4E6;
# 2461
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp4E0=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4E1=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp4E2=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp4E0;void*t1=_tmp4E1;int*iszeroterm=_tmp4E2;
# 2463
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp4DC=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4DD=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp4DE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_tmp4DF=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C6)->f4;_LL42: {struct _tuple1**tn=_tmp4DC;struct Cyc_List_List**ts=_tmp4DD;struct Cyc_List_List*args=_tmp4DE;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4DF;
# 2465
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp4DA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4DB=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL44: {void*ts=_tmp4DA;struct Cyc_List_List*args=_tmp4DB;
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp4D7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4D8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_tmp4D9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C6)->f3;_LL46: {struct Cyc_List_List*es=_tmp4D7;struct Cyc_Absyn_Datatypedecl*tud=_tmp4D8;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D9;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp4D5=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4D6=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp4D5;struct Cyc_Absyn_Enumfield*ef=_tmp4D6;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp4D3=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4D4=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL4A: {void*t2=_tmp4D3;struct Cyc_Absyn_Enumfield*ef=_tmp4D4;
t=t2;goto _LL0;}case 34U: _LL4B: _tmp4CE=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1).is_calloc;_tmp4CF=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1).rgn;_tmp4D0=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1).elt_type;_tmp4D1=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1).num_elts;_tmp4D2=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1).fat_result;_LL4C: {int*is_calloc=_tmp4CE;struct Cyc_Absyn_Exp**ropt=_tmp4CF;void***t2=_tmp4D0;struct Cyc_Absyn_Exp**e2=_tmp4D1;int*isfat=_tmp4D2;
# 2471
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp4CC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4CD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp4CC;struct Cyc_Absyn_Exp*e2=_tmp4CD;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp4CA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_tmp4CB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4C6)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp4CA;struct _fat_ptr*f=_tmp4CB;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp4C9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp4C9;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 42U: _LL53: _tmp4C8=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL54: {struct Cyc_Absyn_Exp*e=_tmp4C8;
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 39U: _LL55: _tmp4C7=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL56: {void*t2=_tmp4C7;
# 2477
if(!te->allow_valueof)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp665;_tmp665.tag=0U,({struct _fat_ptr _tmp8FE=({const char*_tmp52B="valueof(-) can only occur within types";_tag_fat(_tmp52B,sizeof(char),39U);});_tmp665.f1=_tmp8FE;});_tmp665;});void*_tmp529[1U];_tmp529[0]=& _tmp52A;({unsigned _tmp8FF=e->loc;Cyc_Warn_err2(_tmp8FF,_tag_fat(_tmp529,sizeof(void*),1U));});});
# 2485
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL57: _LL58:
# 2489
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 2491
e->topt=t;}

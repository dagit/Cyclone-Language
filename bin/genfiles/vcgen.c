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
# 126 "core.h"
extern int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145
extern void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
extern int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 37 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 914 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*));struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 27 "vcgen.h"
void Cyc_Vcgen_vcgen_topdecls(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 60
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 65
void*Cyc_Tcutil_pointer_elt_type(void*);
# 108
void*Cyc_Tcutil_compress(void*);
# 183
int Cyc_Tcutil_typecmp(void*,void*);
# 275
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 47 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 126 "dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 193
extern struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct Cyc_Vcgen_Env;struct Cyc_Vcgen_Const_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Vcgen_Var_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;};struct Cyc_Vcgen_Primop_Vcgen_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Vcgen_Cast_Vcgen_Term_struct{int tag;void*f1;void*f2;};
# 126 "vcgen.cyc"
struct _fat_ptr Cyc_Vcgen_term2string(void*t){
void*_tmp0=t;void*_tmp2;void*_tmp1;void*_tmp4;enum Cyc_Absyn_Primop _tmp3;void*_tmp7;void*_tmp6;enum Cyc_Absyn_Primop _tmp5;int _tmp9;struct Cyc_Absyn_Vardecl*_tmp8;int _tmpA;struct Cyc_Absyn_Vardecl*_tmpB;struct Cyc_Absyn_Exp*_tmpC;switch(*((int*)_tmp0)){case 0U: _LL1: _tmpC=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp0)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpC;
return Cyc_Absynpp_exp2string(e);}case 1U: _LL3: _tmpB=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp0)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpB;
return Cyc_Absynpp_qvar2string(vd->name);}case 2U: if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f1 == 0){_LL5: _tmpA=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f2;_LL6: {int i=_tmpA;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmp380;_tmp380.tag=1U,_tmp380.f1=(unsigned long)i;_tmp380;});void*_tmpD[1U];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp3F0=({const char*_tmpE="_X%d";_tag_fat(_tmpE,sizeof(char),5U);});Cyc_aprintf(_tmp3F0,_tag_fat(_tmpD,sizeof(void*),1U));});});}}else{_LL7: _tmp8=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f1;_tmp9=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f2;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp8;int i=_tmp9;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp382;_tmp382.tag=0U,({struct _fat_ptr _tmp3F1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(((struct Cyc_Absyn_Vardecl*)_check_null(vd))->name));_tmp382.f1=_tmp3F1;});_tmp382;});struct Cyc_Int_pa_PrintArg_struct _tmp13=({struct Cyc_Int_pa_PrintArg_struct _tmp381;_tmp381.tag=1U,_tmp381.f1=(unsigned long)i;_tmp381;});void*_tmp10[2U];_tmp10[0]=& _tmp12,_tmp10[1]=& _tmp13;({struct _fat_ptr _tmp3F2=({const char*_tmp11="_%s%d";_tag_fat(_tmp11,sizeof(char),6U);});Cyc_aprintf(_tmp3F2,_tag_fat(_tmp10,sizeof(void*),2U));});});}}case 3U: if(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl)->tl == 0){_LL9: _tmp5=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f1;_tmp6=(void*)(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->hd;_tmp7=(void*)((((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl)->hd;_LLA: {enum Cyc_Absyn_Primop p=_tmp5;void*t1=_tmp6;void*t2=_tmp7;
# 133
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp385;_tmp385.tag=0U,({struct _fat_ptr _tmp3F3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t1));_tmp385.f1=_tmp3F3;});_tmp385;});struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp384;_tmp384.tag=0U,({
struct _fat_ptr _tmp3F4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp384.f1=_tmp3F4;});_tmp384;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp383;_tmp383.tag=0U,({struct _fat_ptr _tmp3F5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t2));_tmp383.f1=_tmp3F5;});_tmp383;});void*_tmp14[3U];_tmp14[0]=& _tmp16,_tmp14[1]=& _tmp17,_tmp14[2]=& _tmp18;({struct _fat_ptr _tmp3F6=({const char*_tmp15="(%s%s%s)";_tag_fat(_tmp15,sizeof(char),9U);});Cyc_aprintf(_tmp3F6,_tag_fat(_tmp14,sizeof(void*),3U));});});}}else{goto _LLB;}}else{_LLB: _tmp3=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f1;_tmp4=(void*)(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->hd;_LLC: {enum Cyc_Absyn_Primop p=_tmp3;void*t=_tmp4;
# 136
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B=({struct Cyc_String_pa_PrintArg_struct _tmp387;_tmp387.tag=0U,({struct _fat_ptr _tmp3F7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp387.f1=_tmp3F7;});_tmp387;});struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp386;_tmp386.tag=0U,({struct _fat_ptr _tmp3F8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t));_tmp386.f1=_tmp3F8;});_tmp386;});void*_tmp19[2U];_tmp19[0]=& _tmp1B,_tmp19[1]=& _tmp1C;({struct _fat_ptr _tmp3F9=({const char*_tmp1A="%s(%s)";_tag_fat(_tmp1A,sizeof(char),7U);});Cyc_aprintf(_tmp3F9,_tag_fat(_tmp19,sizeof(void*),2U));});});}}}else{_LLF: _LL10:
# 139
({void*_tmp21=0U;({struct _fat_ptr _tmp3FA=({const char*_tmp22="term2string primop args";_tag_fat(_tmp22,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp3FA,_tag_fat(_tmp21,sizeof(void*),0U));});});}default: _LLD: _tmp1=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp0)->f2;_LLE: {void*tp=_tmp1;void*tm=_tmp2;
# 138
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp389;_tmp389.tag=0U,({struct _fat_ptr _tmp3FB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_tmp389.f1=_tmp3FB;});_tmp389;});struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp388;_tmp388.tag=0U,({struct _fat_ptr _tmp3FC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t));_tmp388.f1=_tmp3FC;});_tmp388;});void*_tmp1D[2U];_tmp1D[0]=& _tmp1F,_tmp1D[1]=& _tmp20;({struct _fat_ptr _tmp3FD=({const char*_tmp1E="(%s)%s";_tag_fat(_tmp1E,sizeof(char),7U);});Cyc_aprintf(_tmp3FD,_tag_fat(_tmp1D,sizeof(void*),2U));});});}}_LL0:;}
# 143
static void*Cyc_Vcgen_cnst(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Vcgen_Const_Vcgen_Term_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=0U,_tmp23->f1=e;_tmp23;});}
static void*Cyc_Vcgen_zero (void){
static void*z=0;
if(z == 0){
struct Cyc_Absyn_Exp*_tmp24=Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U);struct Cyc_Absyn_Exp*e=_tmp24;
({void*_tmp3FE=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp3FE;});{
void*_tmp25=Cyc_Vcgen_cnst(e);void*r=_tmp25;
z=r;
return r;}}else{
return(void*)_check_null(z);}}
# 154
static void*Cyc_Vcgen_one (void){
static void*n=0;
if(n == 0){
struct Cyc_Absyn_Exp*_tmp26=Cyc_Absyn_int_exp(Cyc_Absyn_None,1,0U);struct Cyc_Absyn_Exp*e=_tmp26;
({void*_tmp3FF=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp3FF;});{
void*_tmp27=Cyc_Vcgen_cnst(e);void*r=_tmp27;
n=r;
return r;}}else{
return(void*)_check_null(n);}}
# 165
static void*Cyc_Vcgen_var(struct Cyc_Absyn_Vardecl*vd){return(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=1U,_tmp28->f1=vd;_tmp28;});}
static void*Cyc_Vcgen_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts){
return(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=3U,_tmp29->f1=p,_tmp29->f2=ts;_tmp29;});}
# 169
static void*Cyc_Vcgen_cast(void*tp,void*tm){
return(void*)({struct Cyc_Vcgen_Cast_Vcgen_Term_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=4U,_tmp2A->f1=tp,_tmp2A->f2=tm;_tmp2A;});}
# 172
static void*Cyc_Vcgen_fresh_var (void){
static int counter=0;
return(void*)({struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=2U,_tmp2B->f1=0,_tmp2B->f2=counter ++;_tmp2B;});}struct _tuple11{void*f1;void*f2;};
# 177
static int Cyc_Vcgen_cmp_term(void*t1,void*t2){
struct _tuple11 _tmp2C=({struct _tuple11 _tmp38A;_tmp38A.f1=t1,_tmp38A.f2=t2;_tmp38A;});struct _tuple11 _stmttmp0=_tmp2C;struct _tuple11 _tmp2D=_stmttmp0;struct Cyc_List_List*_tmp31;enum Cyc_Absyn_Primop _tmp30;struct Cyc_List_List*_tmp2F;enum Cyc_Absyn_Primop _tmp2E;void*_tmp35;void*_tmp34;void*_tmp33;void*_tmp32;int _tmp39;struct Cyc_Absyn_Vardecl*_tmp38;int _tmp37;struct Cyc_Absyn_Vardecl*_tmp36;struct Cyc_Absyn_Vardecl*_tmp3B;struct Cyc_Absyn_Vardecl*_tmp3A;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;if(((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2D.f1)->tag == 0U){if(((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2D.f2)->tag == 0U){_LL1: _tmp3C=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2D.f1)->f1;_tmp3D=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2D.f2)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3C;struct Cyc_Absyn_Exp*e2=_tmp3D;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2D.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2D.f1)->tag == 1U){if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2D.f2)->tag == 1U){_LL7: _tmp3A=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2D.f1)->f1;_tmp3B=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2D.f2)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd1=_tmp3A;struct Cyc_Absyn_Vardecl*vd2=_tmp3B;
return(int)vd1 - (int)vd2;}}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2D.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2D.f1)->tag == 2U){if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2D.f2)->tag == 2U){_LLD: _tmp36=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2D.f1)->f1;_tmp37=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2D.f1)->f2;_tmp38=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2D.f2)->f1;_tmp39=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2D.f2)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd1=_tmp36;int i=_tmp37;struct Cyc_Absyn_Vardecl*vd2=_tmp38;int j=_tmp39;
# 186
if(vd1 == vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{_LLF: _LL10:
 return - 1;}}else{if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2D.f2)->tag == 2U){_LL11: _LL12:
 return 1;}else{if(((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2D.f1)->tag == 4U){if(((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2D.f2)->tag == 4U){_LL13: _tmp32=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2D.f1)->f1;_tmp33=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2D.f1)->f2;_tmp34=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2D.f2)->f1;_tmp35=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2D.f2)->f2;_LL14: {void*tp1=_tmp32;void*tm1=_tmp33;void*tp2=_tmp34;void*tm2=_tmp35;
# 191
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c != 0)return c;
return Cyc_Vcgen_cmp_term(tm1,tm2);}}else{_LL15: _LL16:
 return - 1;}}else{if(((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2D.f2)->tag == 4U){_LL17: _LL18:
 return 1;}else{_LL19: _tmp2E=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp2D.f1)->f1;_tmp2F=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp2D.f1)->f2;_tmp30=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp2D.f2)->f1;_tmp31=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp2D.f2)->f2;_LL1A: {enum Cyc_Absyn_Primop p1=_tmp2E;struct Cyc_List_List*ts1=_tmp2F;enum Cyc_Absyn_Primop p2=_tmp30;struct Cyc_List_List*ts2=_tmp31;
# 197
if((int)p1 != (int)p2)return(int)p1 - (int)p2;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Vcgen_cmp_term,ts1,ts2);}}}}}}}}}_LL0:;}
# 202
static int Cyc_Vcgen_same_term(void*t1,void*t2){return Cyc_Vcgen_cmp_term(t1,t2)== 0;}
# 207
enum Cyc_Vcgen_Primreln{Cyc_Vcgen_Eq =0U,Cyc_Vcgen_Neq =1U,Cyc_Vcgen_SLt =2U,Cyc_Vcgen_SLte =3U,Cyc_Vcgen_ULt =4U,Cyc_Vcgen_ULte =5U};struct Cyc_Vcgen_True_Vcgen_Assn_struct{int tag;};struct Cyc_Vcgen_False_Vcgen_Assn_struct{int tag;};struct Cyc_Vcgen_Prim_Vcgen_Assn_struct{int tag;void*f1;enum Cyc_Vcgen_Primreln f2;void*f3;};struct Cyc_Vcgen_And_Vcgen_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_Vcgen_Or_Vcgen_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_Vcgen_Subst_Vcgen_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_Vcgen_Kill_Vcgen_Assn_struct{int tag;void*f1;};
# 221
static unsigned Cyc_Vcgen_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 223
LOOP:
 if(((int(*)(struct Cyc_List_List*l,void*x))Cyc_List_memq)(*prev,a))return c;
({struct Cyc_List_List*_tmp400=({struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->hd=a,_tmp3E->tl=*prev;_tmp3E;});*prev=_tmp400;});{
void*_tmp3F=a;void*_tmp40;void*_tmp43;void*_tmp42;struct Cyc_Absyn_Vardecl*_tmp41;void*_tmp45;void*_tmp44;void*_tmp47;void*_tmp46;switch(*((int*)_tmp3F)){case 0U: _LL1: _LL2:
 return c;case 1U: _LL3: _LL4:
 return c;case 2U: _LL5: _LL6:
 return(unsigned)1 + c;case 3U: _LL7: _tmp46=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp3F)->f1;_tmp47=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp3F)->f2;_LL8: {void*a1=_tmp46;void*a2=_tmp47;
_tmp44=a1;_tmp45=a2;goto _LLA;}case 4U: _LL9: _tmp44=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp3F)->f1;_tmp45=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp3F)->f2;_LLA: {void*a1=_tmp44;void*a2=_tmp45;
# 232
c=Cyc_Vcgen_assn_size_loop(a2,(unsigned)1 + c,prev);
a=a1;
goto LOOP;}case 5U: _LLB: _tmp41=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp3F)->f1;_tmp42=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp3F)->f2;_tmp43=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp3F)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp41;void*t=_tmp42;void*a1=_tmp43;
# 236
++ c;
a=a1;
goto LOOP;}default: _LLD: _tmp40=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp3F)->f1;_LLE: {void*a1=_tmp40;
# 240
++ c;
a=a1;
goto LOOP;}}_LL0:;}}
# 247
static unsigned Cyc_Vcgen_max_assn_size=0U;
# 250
static unsigned Cyc_Vcgen_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned _tmp48=Cyc_Vcgen_assn_size_loop(a,0U,& prev);unsigned c=_tmp48;
if(c > Cyc_Vcgen_max_assn_size)
Cyc_Vcgen_max_assn_size=c;
# 256
return c;}
# 261
static struct _fat_ptr Cyc_Vcgen_a2string(void*a,int prec,int*size){
void*_tmp49=a;void*_tmp4A;void*_tmp4D;void*_tmp4C;struct Cyc_Absyn_Vardecl*_tmp4B;void*_tmp4F;void*_tmp4E;void*_tmp51;void*_tmp50;void*_tmp54;enum Cyc_Vcgen_Primreln _tmp53;void*_tmp52;switch(*((int*)_tmp49)){case 0U: _LL1: _LL2:
 return({const char*_tmp55="true";_tag_fat(_tmp55,sizeof(char),5U);});case 1U: _LL3: _LL4:
 return({const char*_tmp56="false";_tag_fat(_tmp56,sizeof(char),6U);});case 2U: _LL5: _tmp52=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp49)->f1;_tmp53=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp49)->f2;_tmp54=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp49)->f3;_LL6: {void*t1=_tmp52;enum Cyc_Vcgen_Primreln p=_tmp53;void*t2=_tmp54;
# 266
*size +=1;{
struct _fat_ptr ps;
{enum Cyc_Vcgen_Primreln _tmp57=p;switch(_tmp57){case Cyc_Vcgen_Eq: _LL10: _LL11:
 ps=({const char*_tmp58="==";_tag_fat(_tmp58,sizeof(char),3U);});goto _LLF;case Cyc_Vcgen_Neq: _LL12: _LL13:
 ps=({const char*_tmp59="!=";_tag_fat(_tmp59,sizeof(char),3U);});goto _LLF;case Cyc_Vcgen_SLt: _LL14: _LL15:
 ps=({const char*_tmp5A="S<";_tag_fat(_tmp5A,sizeof(char),3U);});goto _LLF;case Cyc_Vcgen_SLte: _LL16: _LL17:
 ps=({const char*_tmp5B="S<=";_tag_fat(_tmp5B,sizeof(char),4U);});goto _LLF;case Cyc_Vcgen_ULt: _LL18: _LL19:
 ps=({const char*_tmp5C="U<";_tag_fat(_tmp5C,sizeof(char),3U);});goto _LLF;case Cyc_Vcgen_ULte: _LL1A: _LL1B:
 ps=({const char*_tmp5D="U<=";_tag_fat(_tmp5D,sizeof(char),4U);});goto _LLF;default: _LL1C: _LL1D:
({void*_tmp5E=0U;({struct _fat_ptr _tmp401=({const char*_tmp5F="assn2string primop";_tag_fat(_tmp5F,sizeof(char),19U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp401,_tag_fat(_tmp5E,sizeof(void*),0U));});});}_LLF:;}
# 277
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp38D;_tmp38D.tag=0U,({struct _fat_ptr _tmp402=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t1));_tmp38D.f1=_tmp402;});_tmp38D;});struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp38C;_tmp38C.tag=0U,_tmp38C.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_tmp38C;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0U,({struct _fat_ptr _tmp403=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t2));_tmp38B.f1=_tmp403;});_tmp38B;});void*_tmp60[3U];_tmp60[0]=& _tmp62,_tmp60[1]=& _tmp63,_tmp60[2]=& _tmp64;({struct _fat_ptr _tmp404=({const char*_tmp61="%s%s%s";_tag_fat(_tmp61,sizeof(char),7U);});Cyc_aprintf(_tmp404,_tag_fat(_tmp60,sizeof(void*),3U));});});}}case 4U: _LL7: _tmp50=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp49)->f1;_tmp51=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp49)->f2;_LL8: {void*a1=_tmp50;void*a2=_tmp51;
# 279
*size +=1;
if(prec <= 5)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U,({struct _fat_ptr _tmp405=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,5,size));_tmp38F.f1=_tmp405;});_tmp38F;});struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0U,({struct _fat_ptr _tmp406=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,5,size));_tmp38E.f1=_tmp406;});_tmp38E;});void*_tmp65[2U];_tmp65[0]=& _tmp67,_tmp65[1]=& _tmp68;({struct _fat_ptr _tmp407=({const char*_tmp66="%s || %s";_tag_fat(_tmp66,sizeof(char),9U);});Cyc_aprintf(_tmp407,_tag_fat(_tmp65,sizeof(void*),2U));});});else{
# 283
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp391;_tmp391.tag=0U,({struct _fat_ptr _tmp408=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,5,size));_tmp391.f1=_tmp408;});_tmp391;});struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp390;_tmp390.tag=0U,({struct _fat_ptr _tmp409=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,5,size));_tmp390.f1=_tmp409;});_tmp390;});void*_tmp69[2U];_tmp69[0]=& _tmp6B,_tmp69[1]=& _tmp6C;({struct _fat_ptr _tmp40A=({const char*_tmp6A="(%s || %s)";_tag_fat(_tmp6A,sizeof(char),11U);});Cyc_aprintf(_tmp40A,_tag_fat(_tmp69,sizeof(void*),2U));});});}}case 3U: _LL9: _tmp4E=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp49)->f1;_tmp4F=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp49)->f2;_LLA: {void*a1=_tmp4E;void*a2=_tmp4F;
# 285
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp393;_tmp393.tag=0U,({struct _fat_ptr _tmp40B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,10,size));_tmp393.f1=_tmp40B;});_tmp393;});struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0U,({struct _fat_ptr _tmp40C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,10,size));_tmp392.f1=_tmp40C;});_tmp392;});void*_tmp6D[2U];_tmp6D[0]=& _tmp6F,_tmp6D[1]=& _tmp70;({struct _fat_ptr _tmp40D=({const char*_tmp6E="%s && %s";_tag_fat(_tmp6E,sizeof(char),9U);});Cyc_aprintf(_tmp40D,_tag_fat(_tmp6D,sizeof(void*),2U));});});}case 5U: _LLB: _tmp4B=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp49)->f1;_tmp4C=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp49)->f2;_tmp4D=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp49)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp4B;void*t=_tmp4C;void*a=_tmp4D;
# 288
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmp396;_tmp396.tag=0U,({struct _fat_ptr _tmp40E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp396.f1=_tmp40E;});_tmp396;});struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmp395;_tmp395.tag=0U,({
struct _fat_ptr _tmp40F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t));_tmp395.f1=_tmp40F;});_tmp395;});struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp394;_tmp394.tag=0U,({struct _fat_ptr _tmp410=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a,0,size));_tmp394.f1=_tmp410;});_tmp394;});void*_tmp71[3U];_tmp71[0]=& _tmp73,_tmp71[1]=& _tmp74,_tmp71[2]=& _tmp75;({struct _fat_ptr _tmp411=({const char*_tmp72="Subst[%s:=%s](%s)";_tag_fat(_tmp72,sizeof(char),18U);});Cyc_aprintf(_tmp411,_tag_fat(_tmp71,sizeof(void*),3U));});});}default: _LLD: _tmp4A=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp49)->f1;_LLE: {void*a=_tmp4A;
# 292
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp78=({struct Cyc_String_pa_PrintArg_struct _tmp397;_tmp397.tag=0U,({struct _fat_ptr _tmp412=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a,0,size));_tmp397.f1=_tmp412;});_tmp397;});void*_tmp76[1U];_tmp76[0]=& _tmp78;({struct _fat_ptr _tmp413=({const char*_tmp77="Kill(%s)";_tag_fat(_tmp77,sizeof(char),9U);});Cyc_aprintf(_tmp413,_tag_fat(_tmp76,sizeof(void*),1U));});});}}_LL0:;}
# 297
static int Cyc_Vcgen_tree_size=0;
# 300
struct _fat_ptr Cyc_Vcgen_assn2string(void*a){
Cyc_Vcgen_tree_size=0;
return Cyc_Vcgen_a2string(a,0,& Cyc_Vcgen_tree_size);}
# 306
static struct Cyc_Vcgen_True_Vcgen_Assn_struct Cyc_Vcgen_true_assn={0U};
static struct Cyc_Vcgen_False_Vcgen_Assn_struct Cyc_Vcgen_false_assn={1U};
# 317 "vcgen.cyc"
static int Cyc_Vcgen_assnhash(void*a){
void*_tmp79=a;void*_tmp7A;void*_tmp7D;void*_tmp7C;struct Cyc_Absyn_Vardecl*_tmp7B;void*_tmp7F;void*_tmp7E;void*_tmp81;void*_tmp80;void*_tmp84;enum Cyc_Vcgen_Primreln _tmp83;void*_tmp82;switch(*((int*)_tmp79)){case 1U: _LL1: _LL2:
 return 0;case 0U: _LL3: _LL4:
 return 1;case 2U: _LL5: _tmp82=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp79)->f1;_tmp83=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp79)->f2;_tmp84=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp79)->f3;_LL6: {void*t1=_tmp82;enum Cyc_Vcgen_Primreln p=_tmp83;void*t2=_tmp84;
# 322
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3U: _LL7: _tmp80=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp79)->f1;_tmp81=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp79)->f2;_LL8: {void*a1=_tmp80;void*a2=_tmp81;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4U: _LL9: _tmp7E=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp79)->f1;_tmp7F=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp79)->f2;_LLA: {void*a1=_tmp7E;void*a2=_tmp7F;
return(int)((unsigned)a1 ^ (unsigned)a2 | (unsigned)1);}case 5U: _LLB: _tmp7B=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp79)->f1;_tmp7C=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp79)->f2;_tmp7D=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp79)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp7B;void*t=_tmp7C;void*a=_tmp7D;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | (unsigned)3);}default: _LLD: _tmp7A=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp79)->f1;_LLE: {void*a=_tmp7A;
# 327
return(int)((unsigned)a + (unsigned)37);}}_LL0:;}
# 333
static int Cyc_Vcgen_assncmp(void*a1,void*a2){
LOOP:
 if(a1 == a2)return 0;{
struct _tuple11 _tmp85=({struct _tuple11 _tmp398;_tmp398.f1=a1,_tmp398.f2=a2;_tmp398;});struct _tuple11 _stmttmp1=_tmp85;struct _tuple11 _tmp86=_stmttmp1;void*_tmp8C;void*_tmp8B;struct Cyc_Absyn_Vardecl*_tmp8A;void*_tmp89;void*_tmp88;struct Cyc_Absyn_Vardecl*_tmp87;void*_tmp8E;void*_tmp8D;void*_tmp94;enum Cyc_Vcgen_Primreln _tmp93;void*_tmp92;void*_tmp91;enum Cyc_Vcgen_Primreln _tmp90;void*_tmp8F;void*_tmp98;void*_tmp97;void*_tmp96;void*_tmp95;void*_tmp9C;void*_tmp9B;void*_tmp9A;void*_tmp99;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp86.f1)->tag == 0U){if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp86.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp86.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp86.f1)->tag == 1U){if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp86.f2)->tag == 1U){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp86.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{switch(*((int*)_tmp86.f1)){case 4U: if(((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp86.f2)->tag == 4U){_LLD: _tmp99=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp86.f1)->f1;_tmp9A=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp86.f1)->f2;_tmp9B=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp86.f2)->f1;_tmp9C=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp86.f2)->f2;_LLE: {void*a11=_tmp99;void*a12=_tmp9A;void*a21=_tmp9B;void*a22=_tmp9C;
_tmp95=a11;_tmp96=a12;_tmp97=a21;_tmp98=a22;goto _LL10;}}else{_LL11: _LL12:
# 350
 return - 1;}case 3U: switch(*((int*)_tmp86.f2)){case 3U: _LLF: _tmp95=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp86.f1)->f1;_tmp96=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp86.f1)->f2;_tmp97=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp86.f2)->f1;_tmp98=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp86.f2)->f2;_LL10: {void*a11=_tmp95;void*a12=_tmp96;void*a21=_tmp97;void*a22=_tmp98;
# 345
int c=Cyc_Vcgen_assncmp(a11,a21);
if(c != 0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4U: goto _LL13;default: _LL15: _LL16:
# 352
 return - 1;}default: switch(*((int*)_tmp86.f2)){case 4U: _LL13: _LL14:
# 351
 return 1;case 3U: _LL17: _LL18:
# 353
 return 1;default: if(((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f1)->tag == 2U){if(((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f2)->tag == 2U){_LL19: _tmp8F=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f1)->f1;_tmp90=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f1)->f2;_tmp91=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f1)->f3;_tmp92=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f2)->f1;_tmp93=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f2)->f2;_tmp94=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f2)->f3;_LL1A: {void*t11=_tmp8F;enum Cyc_Vcgen_Primreln p1=_tmp90;void*t12=_tmp91;void*t21=_tmp92;enum Cyc_Vcgen_Primreln p2=_tmp93;void*t22=_tmp94;
# 355
int c=(int)p1 - (int)p2;
if(c != 0)return c;
c=Cyc_Vcgen_cmp_term(t11,t21);
if(c != 0)return c;
return Cyc_Vcgen_cmp_term(t12,t22);}}else{_LL1B: _LL1C:
 return - 1;}}else{if(((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp86.f2)->tag == 2U){_LL1D: _LL1E:
 return 1;}else{if(((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp86.f1)->tag == 6U){if(((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp86.f2)->tag == 6U){_LL1F: _tmp8D=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp86.f1)->f1;_tmp8E=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp86.f2)->f1;_LL20: {void*a1=_tmp8D;void*a2=_tmp8E;
return Cyc_Vcgen_assncmp(a1,a2);}}else{_LL21: _LL22:
 return - 1;}}else{if(((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp86.f2)->tag == 6U){_LL23: _LL24:
 return 1;}else{_LL25: _tmp87=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp86.f1)->f1;_tmp88=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp86.f1)->f2;_tmp89=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp86.f1)->f3;_tmp8A=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp86.f2)->f1;_tmp8B=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp86.f2)->f2;_tmp8C=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp86.f2)->f3;_LL26: {struct Cyc_Absyn_Vardecl*vd1=_tmp87;void*t1=_tmp88;void*a11=_tmp89;struct Cyc_Absyn_Vardecl*vd2=_tmp8A;void*t2=_tmp8B;void*a22=_tmp8C;
# 366
int c=(int)vd1 - (int)vd2;
if(c != 0)return c;
c=Cyc_Vcgen_cmp_term(t1,t2);
if(c != 0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}}_LL0:;}}
# 379
static struct Cyc_Hashtable_Table*Cyc_Vcgen_hash_cons_table=0;struct _tuple12{void*f1;int f2;};
# 384
static int Cyc_Vcgen_is_reduced(void*a){
if(Cyc_Vcgen_hash_cons_table == 0)return 0;{
struct _tuple12**_tmp9D=((struct _tuple12**(*)(struct Cyc_Hashtable_Table*t,void*key))Cyc_Hashtable_lookup_opt)((struct Cyc_Hashtable_Table*)_check_null(Cyc_Vcgen_hash_cons_table),a);struct _tuple12**resopt=_tmp9D;
if(resopt == 0)return 0;{
struct _tuple12*_tmp9E=*resopt;struct _tuple12*_stmttmp2=_tmp9E;struct _tuple12*_tmp9F=_stmttmp2;int _tmpA0;_LL1: _tmpA0=_tmp9F->f2;_LL2: {int b=_tmpA0;
return b;}}}}
# 395
static struct _tuple12 Cyc_Vcgen_copy_assn(void*a){
void*_tmpA1=a;void*_tmpA2;void*_tmpA5;void*_tmpA4;struct Cyc_Absyn_Vardecl*_tmpA3;void*_tmpA8;enum Cyc_Vcgen_Primreln _tmpA7;void*_tmpA6;void*_tmpAA;void*_tmpA9;void*_tmpAC;void*_tmpAB;switch(*((int*)_tmpA1)){case 0U: _LL1: _LL2:
 return({struct _tuple12 _tmp399;_tmp399.f1=(void*)& Cyc_Vcgen_true_assn,_tmp399.f2=1;_tmp399;});case 1U: _LL3: _LL4:
 return({struct _tuple12 _tmp39A;_tmp39A.f1=(void*)& Cyc_Vcgen_false_assn,_tmp39A.f2=0;_tmp39A;});case 3U: _LL5: _tmpAB=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpA1)->f1;_tmpAC=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpA1)->f2;_LL6: {void*a1=_tmpAB;void*a2=_tmpAC;
# 400
return({struct _tuple12 _tmp39B;({void*_tmp415=(void*)({struct Cyc_Vcgen_And_Vcgen_Assn_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=3U,_tmpAD->f1=a1,_tmpAD->f2=a2;_tmpAD;});_tmp39B.f1=_tmp415;}),({int _tmp414=Cyc_Vcgen_is_reduced(a1)&& Cyc_Vcgen_is_reduced(a2);_tmp39B.f2=_tmp414;});_tmp39B;});}case 4U: _LL7: _tmpA9=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpA1)->f1;_tmpAA=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpA1)->f2;_LL8: {void*a1=_tmpA9;void*a2=_tmpAA;
# 402
return({struct _tuple12 _tmp39C;({void*_tmp417=(void*)({struct Cyc_Vcgen_Or_Vcgen_Assn_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=4U,_tmpAE->f1=a1,_tmpAE->f2=a2;_tmpAE;});_tmp39C.f1=_tmp417;}),({int _tmp416=Cyc_Vcgen_is_reduced(a1)&& Cyc_Vcgen_is_reduced(a2);_tmp39C.f2=_tmp416;});_tmp39C;});}case 2U: _LL9: _tmpA6=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA1)->f1;_tmpA7=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA1)->f2;_tmpA8=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA1)->f3;_LLA: {void*t1=_tmpA6;enum Cyc_Vcgen_Primreln p=_tmpA7;void*t2=_tmpA8;
return({struct _tuple12 _tmp39D;({void*_tmp418=(void*)({struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=2U,_tmpAF->f1=t1,_tmpAF->f2=p,_tmpAF->f3=t2;_tmpAF;});_tmp39D.f1=_tmp418;}),_tmp39D.f2=1;_tmp39D;});}case 5U: _LLB: _tmpA3=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpA1)->f1;_tmpA4=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpA1)->f2;_tmpA5=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpA1)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd1=_tmpA3;void*t1=_tmpA4;void*a1=_tmpA5;
return({struct _tuple12 _tmp39E;({void*_tmp419=(void*)({struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=5U,_tmpB0->f1=vd1,_tmpB0->f2=t1,_tmpB0->f3=a1;_tmpB0;});_tmp39E.f1=_tmp419;}),_tmp39E.f2=0;_tmp39E;});}default: _LLD: _tmpA2=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmpA1)->f1;_LLE: {void*a=_tmpA2;
return({struct _tuple12 _tmp39F;({void*_tmp41A=(void*)({struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=6U,_tmpB1->f1=a;_tmpB1;});_tmp39F.f1=_tmp41A;}),_tmp39F.f2=0;_tmp39F;});}}_LL0:;}
# 413
static void*Cyc_Vcgen_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_Vcgen_hash_cons_table == 0){
h=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_create)(123,Cyc_Vcgen_assncmp,Cyc_Vcgen_assnhash);
Cyc_Vcgen_hash_cons_table=h;
# 420
({struct Cyc_Hashtable_Table*_tmp41B=h;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple12*val))Cyc_Hashtable_insert)(_tmp41B,(void*)& Cyc_Vcgen_true_assn,({struct _tuple12*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->f1=(void*)& Cyc_Vcgen_true_assn,_tmpB2->f2=1;_tmpB2;}));});
({struct Cyc_Hashtable_Table*_tmp41C=h;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple12*val))Cyc_Hashtable_insert)(_tmp41C,(void*)& Cyc_Vcgen_false_assn,({struct _tuple12*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->f1=(void*)& Cyc_Vcgen_false_assn,_tmpB3->f2=1;_tmpB3;}));});}else{
# 423
h=(struct Cyc_Hashtable_Table*)_check_null(Cyc_Vcgen_hash_cons_table);}{
struct _tuple12**_tmpB4=((struct _tuple12**(*)(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_lookup_other_opt)(h,a,Cyc_Vcgen_assncmp,Cyc_Vcgen_assnhash);struct _tuple12**resopt=_tmpB4;
if(resopt == 0){
struct _tuple12 _tmpB5=Cyc_Vcgen_copy_assn(a);struct _tuple12 _stmttmp3=_tmpB5;struct _tuple12 _tmpB6=_stmttmp3;int _tmpB8;void*_tmpB7;_LL1: _tmpB7=_tmpB6.f1;_tmpB8=_tmpB6.f2;_LL2: {void*res=_tmpB7;int reduced=_tmpB8;
({struct Cyc_Hashtable_Table*_tmp41E=h;void*_tmp41D=res;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple12*val))Cyc_Hashtable_insert)(_tmp41E,_tmp41D,({struct _tuple12*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->f1=res,_tmpB9->f2=reduced;_tmpB9;}));});
return res;}}else{
# 430
struct _tuple12*_tmpBA=*resopt;struct _tuple12*_stmttmp4=_tmpBA;struct _tuple12*_tmpBB=_stmttmp4;void*_tmpBC;_LL4: _tmpBC=_tmpBB->f1;_LL5: {void*res=_tmpBC;
return res;}}}}
# 436
static void*Cyc_Vcgen_kill(void*a){
void*_tmpBD=a;switch(*((int*)_tmpBD)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 goto _LL6;case 6U: _LL5: _LL6:
 return a;default: _LL7: _LL8: {
# 442
struct Cyc_Vcgen_Kill_Vcgen_Assn_struct _tmpBE=({struct Cyc_Vcgen_Kill_Vcgen_Assn_struct _tmp3A0;_tmp3A0.tag=6U,_tmp3A0.f1=a;_tmp3A0;});struct Cyc_Vcgen_Kill_Vcgen_Assn_struct p=_tmpBE;
return Cyc_Vcgen_hash_cons((void*)& p);}}_LL0:;}
# 448
static void*Cyc_Vcgen_and(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmpBF=({struct _tuple11 _tmp3A2;_tmp3A2.f1=a1,_tmp3A2.f2=a2;_tmp3A2;});struct _tuple11 _stmttmp5=_tmpBF;struct _tuple11 _tmpC0=_stmttmp5;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmpC0.f1)->tag == 0U){_LL1: _LL2:
 return a2;}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmpC0.f2)->tag == 0U){_LL3: _LL4:
 return a1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmpC0.f1)->tag == 1U){_LL5: _LL6:
 return a1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmpC0.f2)->tag == 1U){_LL7: _LL8:
 return a2;}else{_LL9: _LLA:
# 459
 if((int)a1 > (int)a2)
({void*_tmpC1=a1;void*_tmpC2=a2;a1=_tmpC2;a2=_tmpC1;});{
struct Cyc_Vcgen_And_Vcgen_Assn_struct _tmpC3=({struct Cyc_Vcgen_And_Vcgen_Assn_struct _tmp3A1;_tmp3A1.tag=3U,_tmp3A1.f1=a1,_tmp3A1.f2=a2;_tmp3A1;});struct Cyc_Vcgen_And_Vcgen_Assn_struct a=_tmpC3;
return Cyc_Vcgen_hash_cons((void*)& a);}}}}}_LL0:;}}
# 467
static void*Cyc_Vcgen_or(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmpC4=({struct _tuple11 _tmp3A4;_tmp3A4.f1=a1,_tmp3A4.f2=a2;_tmp3A4;});struct _tuple11 _stmttmp6=_tmpC4;struct _tuple11 _tmpC5=_stmttmp6;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmpC5.f1)->tag == 0U){_LL1: _LL2:
 return a1;}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmpC5.f2)->tag == 0U){_LL3: _LL4:
 return a2;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmpC5.f1)->tag == 1U){_LL5: _LL6:
 return a2;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmpC5.f2)->tag == 1U){_LL7: _LL8:
 return a1;}else{_LL9: _LLA:
# 476
 if((int)a1 > (int)a2)
({void*_tmpC6=a1;void*_tmpC7=a2;a1=_tmpC7;a2=_tmpC6;});{
struct Cyc_Vcgen_Or_Vcgen_Assn_struct _tmpC8=({struct Cyc_Vcgen_Or_Vcgen_Assn_struct _tmp3A3;_tmp3A3.tag=4U,_tmp3A3.f1=a1,_tmp3A3.f2=a2;_tmp3A3;});struct Cyc_Vcgen_Or_Vcgen_Assn_struct a=_tmpC8;
return Cyc_Vcgen_hash_cons((void*)& a);}}}}}_LL0:;}}
# 484
static void*Cyc_Vcgen_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
void*_tmpC9=a;switch(*((int*)_tmpC9)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return a;default: _LL5: _LL6: {
# 489
struct Cyc_Vcgen_Subst_Vcgen_Assn_struct _tmpCA=({struct Cyc_Vcgen_Subst_Vcgen_Assn_struct _tmp3A5;_tmp3A5.tag=5U,_tmp3A5.f1=x,_tmp3A5.f2=newx,_tmp3A5.f3=a;_tmp3A5;});struct Cyc_Vcgen_Subst_Vcgen_Assn_struct p=_tmpCA;
return Cyc_Vcgen_hash_cons((void*)& p);}}_LL0:;}
# 495
static void*Cyc_Vcgen_prim(void*t1,enum Cyc_Vcgen_Primreln p,void*t2){
struct Cyc_Vcgen_Prim_Vcgen_Assn_struct _tmpCB=({struct Cyc_Vcgen_Prim_Vcgen_Assn_struct _tmp3A6;_tmp3A6.tag=2U,_tmp3A6.f1=t1,_tmp3A6.f2=p,_tmp3A6.f3=t2;_tmp3A6;});struct Cyc_Vcgen_Prim_Vcgen_Assn_struct ptr=_tmpCB;
return Cyc_Vcgen_hash_cons((void*)& ptr);}
# 505
static void*Cyc_Vcgen_eq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_Eq,t2);}
static void*Cyc_Vcgen_neq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_Neq,t2);}
static void*Cyc_Vcgen_slt(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_SLt,t2);}
static void*Cyc_Vcgen_slte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_SLte,t2);}
static void*Cyc_Vcgen_ult(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_ULt,t2);}
static void*Cyc_Vcgen_ulte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_ULte,t2);}
# 517
static void*Cyc_Vcgen_reduce(void*a);
# 522
static void*Cyc_Vcgen_not(void*a){
void*_tmpCC=a;void*_tmpCF;enum Cyc_Vcgen_Primreln _tmpCE;void*_tmpCD;void*_tmpD1;void*_tmpD0;void*_tmpD3;void*_tmpD2;switch(*((int*)_tmpCC)){case 0U: _LL1: _LL2:
 return(void*)& Cyc_Vcgen_false_assn;case 1U: _LL3: _LL4:
 return(void*)& Cyc_Vcgen_true_assn;case 3U: _LL5: _tmpD2=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpCC)->f1;_tmpD3=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpCC)->f2;_LL6: {void*a1=_tmpD2;void*a2=_tmpD3;
return({void*_tmp41F=Cyc_Vcgen_not(a1);Cyc_Vcgen_or(_tmp41F,Cyc_Vcgen_not(a2));});}case 4U: _LL7: _tmpD0=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpCC)->f1;_tmpD1=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpCC)->f2;_LL8: {void*a1=_tmpD0;void*a2=_tmpD1;
return({void*_tmp420=Cyc_Vcgen_not(a1);Cyc_Vcgen_and(_tmp420,Cyc_Vcgen_not(a2));});}case 2U: _LL9: _tmpCD=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpCC)->f1;_tmpCE=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpCC)->f2;_tmpCF=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpCC)->f3;_LLA: {void*t1=_tmpCD;enum Cyc_Vcgen_Primreln p=_tmpCE;void*t2=_tmpCF;
# 529
enum Cyc_Vcgen_Primreln _tmpD4=p;switch(_tmpD4){case Cyc_Vcgen_Eq: _LL10: _LL11:
 return Cyc_Vcgen_neq(t1,t2);case Cyc_Vcgen_Neq: _LL12: _LL13:
 return Cyc_Vcgen_eq(t1,t2);case Cyc_Vcgen_SLt: _LL14: _LL15:
 return Cyc_Vcgen_slte(t2,t1);case Cyc_Vcgen_SLte: _LL16: _LL17:
 return Cyc_Vcgen_slt(t2,t1);case Cyc_Vcgen_ULt: _LL18: _LL19:
 return Cyc_Vcgen_ulte(t2,t1);case Cyc_Vcgen_ULte: _LL1A: _LL1B:
 return Cyc_Vcgen_ult(t2,t1);default: _LL1C: _LL1D:
({void*_tmpD5=0U;({struct _fat_ptr _tmp421=({const char*_tmpD6="Vcgen::invert: non-primop";_tag_fat(_tmpD6,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp421,_tag_fat(_tmpD5,sizeof(void*),0U));});});}_LLF:;}case 5U: _LLB: _LLC:
# 538
 return Cyc_Vcgen_not(Cyc_Vcgen_reduce(a));default: _LLD: _LLE:
 return Cyc_Vcgen_not(Cyc_Vcgen_reduce(a));}_LL0:;}struct _tuple13{struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 561 "vcgen.cyc"
static void*Cyc_Vcgen_reduce_term(void*t,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 564
void*_tmpD7=t;struct Cyc_Absyn_Vardecl*_tmpD8;struct Cyc_List_List*_tmpDA;enum Cyc_Absyn_Primop _tmpD9;void*_tmpDC;void*_tmpDB;switch(*((int*)_tmpD7)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return t;case 4U: _LL5: _tmpDB=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmpD7)->f1;_tmpDC=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmpD7)->f2;_LL6: {void*tp=_tmpDB;void*tm=_tmpDC;
# 568
void*_tmpDD=Cyc_Vcgen_reduce_term(tm,non_esc_subst,esc_subst);void*tm2=_tmpDD;
if(tm != tm2)return Cyc_Vcgen_cast(tp,tm2);else{
return t;}}case 3U: _LL7: _tmpD9=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmpD7)->f1;_tmpDA=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmpD7)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmpD9;struct Cyc_List_List*ts=_tmpDA;
# 572
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpDE=ts;struct Cyc_List_List*ptr=_tmpDE;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpDF=(void*)ptr->hd;void*old_term=_tmpDF;
void*_tmpE0=Cyc_Vcgen_reduce_term(old_term,non_esc_subst,esc_subst);void*new_term=_tmpE0;
res=({struct Cyc_List_List*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=new_term,_tmpE1->tl=res;_tmpE1;});
if(new_term != old_term)change=1;}}
# 580
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp422=p;Cyc_Vcgen_primop(_tmp422,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res));});}default: _LL9: _tmpD8=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmpD7)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpD8;
# 583
if(vd->escapes && esc_subst != 0){
# 586
struct Cyc_List_List*_tmpE2=*esc_subst;struct Cyc_List_List*s=_tmpE2;
for(0;s != 0;s=s->tl){
struct _tuple13 _tmpE3=*((struct _tuple13*)s->hd);struct _tuple13 _stmttmp7=_tmpE3;struct _tuple13 _tmpE4=_stmttmp7;void*_tmpE6;struct Cyc_Absyn_Vardecl*_tmpE5;_LLC: _tmpE5=_tmpE4.f1;_tmpE6=_tmpE4.f2;_LLD: {struct Cyc_Absyn_Vardecl*vd2=_tmpE5;void*t2=_tmpE6;
if(vd2 == vd)return t;}}{
# 593
void*_tmpE7=Cyc_Vcgen_fresh_var();void*t=_tmpE7;
({struct Cyc_List_List*_tmp424=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));({struct _tuple13*_tmp423=({struct _tuple13*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->f1=vd,_tmpE8->f2=t;_tmpE8;});_tmpE9->hd=_tmp423;}),_tmpE9->tl=*esc_subst;_tmpE9;});*esc_subst=_tmp424;});
return Cyc_Vcgen_fresh_var();}}else{
if(!vd->escapes)
# 598
for(0;non_esc_subst != 0;non_esc_subst=non_esc_subst->tl){
struct _tuple13 _tmpEA=*((struct _tuple13*)non_esc_subst->hd);struct _tuple13 _stmttmp8=_tmpEA;struct _tuple13 _tmpEB=_stmttmp8;void*_tmpED;struct Cyc_Absyn_Vardecl*_tmpEC;_LLF: _tmpEC=_tmpEB.f1;_tmpED=_tmpEB.f2;_LL10: {struct Cyc_Absyn_Vardecl*vd2=_tmpEC;void*t2=_tmpED;
if(vd == vd2)return t2;}}}
# 603
return t;}}_LL0:;}
# 607
static void*Cyc_Vcgen_reduce_assn(void*a,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 610
LOOP: {
void*_tmpEE=a;void*_tmpF1;void*_tmpF0;struct Cyc_Absyn_Vardecl*_tmpEF;void*_tmpF2;void*_tmpF4;void*_tmpF3;void*_tmpF6;void*_tmpF5;void*_tmpF9;enum Cyc_Vcgen_Primreln _tmpF8;void*_tmpF7;switch(*((int*)_tmpEE)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return a;case 2U: _LL5: _tmpF7=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpEE)->f1;_tmpF8=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpEE)->f2;_tmpF9=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpEE)->f3;_LL6: {void*t1=_tmpF7;enum Cyc_Vcgen_Primreln p=_tmpF8;void*t2=_tmpF9;
# 615
void*_tmpFA=Cyc_Vcgen_reduce_term(t1,non_esc_subst,esc_subst);void*newt1=_tmpFA;
void*_tmpFB=Cyc_Vcgen_reduce_term(t2,non_esc_subst,esc_subst);void*newt2=_tmpFB;
if(t1 == newt1 && t2 == newt2)return a;
return Cyc_Vcgen_prim(newt1,p,newt2);}case 3U: _LL7: _tmpF5=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpEE)->f1;_tmpF6=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpEE)->f2;_LL8: {void*a1=_tmpF5;void*a2=_tmpF6;
# 620
void*_tmpFC=Cyc_Vcgen_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpFC;
void*_tmpFD=Cyc_Vcgen_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpFD;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_Vcgen_and(newa1,newa2);}case 4U: _LL9: _tmpF3=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpEE)->f1;_tmpF4=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpEE)->f2;_LLA: {void*a1=_tmpF3;void*a2=_tmpF4;
# 625
void*_tmpFE=Cyc_Vcgen_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpFE;
void*_tmpFF=Cyc_Vcgen_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpFF;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_Vcgen_or(newa1,newa2);}case 6U: _LLB: _tmpF2=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmpEE)->f1;_LLC: {void*a=_tmpF2;
# 632
struct Cyc_List_List*new_esc_subst=0;
return Cyc_Vcgen_reduce_assn(a,non_esc_subst,& new_esc_subst);}default: _LLD: _tmpEF=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpEE)->f1;_tmpF0=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpEE)->f2;_tmpF1=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpEE)->f3;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmpEF;void*t=_tmpF0;void*a1=_tmpF1;
# 636
t=Cyc_Vcgen_reduce_term(t,non_esc_subst,esc_subst);
non_esc_subst=({struct Cyc_List_List*_tmp101=_cycalloc(sizeof(*_tmp101));({struct _tuple13*_tmp425=({struct _tuple13*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->f1=vd,_tmp100->f2=t;_tmp100;});_tmp101->hd=_tmp425;}),_tmp101->tl=non_esc_subst;_tmp101;});
a=a1;
goto LOOP;}}_LL0:;}}
# 644
static void*Cyc_Vcgen_reduce(void*a){
# 646
if(Cyc_Vcgen_is_reduced(a))return a;
return Cyc_Vcgen_reduce_assn(a,0,0);}
# 651
static void*Cyc_Vcgen_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_tmp102=t;void*_tmp104;void*_tmp103;struct Cyc_List_List*_tmp106;enum Cyc_Absyn_Primop _tmp105;struct Cyc_Absyn_Vardecl*_tmp107;switch(*((int*)_tmp102)){case 1U: _LL1: _tmp107=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp102)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp107;
# 654
if(vd == x)return newx;
return t;}case 3U: _LL3: _tmp105=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp102)->f1;_tmp106=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp102)->f2;_LL4: {enum Cyc_Absyn_Primop p=_tmp105;struct Cyc_List_List*ts=_tmp106;
# 657
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmp108=ts;struct Cyc_List_List*ptr=_tmp108;for(0;ptr != 0;ptr=ptr->tl){
void*_tmp109=(void*)ptr->hd;void*old_term=_tmp109;
void*_tmp10A=Cyc_Vcgen_subst_term(old_term,x,newx);void*new_term=_tmp10A;
res=({struct Cyc_List_List*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=new_term,_tmp10B->tl=res;_tmp10B;});
if(new_term != old_term)change=1;}}
# 665
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp426=p;Cyc_Vcgen_primop(_tmp426,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res));});}case 4U: _LL5: _tmp103=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp102)->f1;_tmp104=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp102)->f2;_LL6: {void*tp=_tmp103;void*tm=_tmp104;
# 668
void*_tmp10C=Cyc_Vcgen_subst_term(tm,x,newx);void*tm2=_tmp10C;
if(tm != tm2)return Cyc_Vcgen_cast(tp,tm2);else{
return t;}}case 0U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 return t;}_LL0:;}
# 677
static void*Cyc_Vcgen_kill_mem_term(void*t){
struct Cyc_List_List*_tmp10D=0;struct Cyc_List_List*esc_subst=_tmp10D;
return Cyc_Vcgen_reduce_term(t,0,& esc_subst);}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 692 "vcgen.cyc"
static void Cyc_Vcgen_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 696
while(1){
struct _tuple14 _tmp10E=({struct _tuple14 _tmp3A7;_tmp3A7.f1=a1s,_tmp3A7.f2=a2s;_tmp3A7;});struct _tuple14 _stmttmp9=_tmp10E;struct _tuple14 _tmp10F=_stmttmp9;if(_tmp10F.f1 == 0){if(_tmp10F.f2 == 0){_LL1: _LL2:
 return;}else{_LL5: _LL6:
# 700
({struct Cyc_List_List*_tmp427=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(a2s,*res2);*res2=_tmp427;});return;}}else{if(_tmp10F.f2 == 0){_LL3: _LL4:
# 699
({struct Cyc_List_List*_tmp428=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(a1s,*res1);*res1=_tmp428;});return;}else{_LL7: _LL8: {
# 702
int c=({void*_tmp429=(void*)((struct Cyc_List_List*)_check_null(a1s))->hd;Cyc_Vcgen_assncmp(_tmp429,(void*)((struct Cyc_List_List*)_check_null(a2s))->hd);});
if(c == 0){
({struct Cyc_List_List*_tmp42A=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->hd=(void*)a1s->hd,_tmp110->tl=*common;_tmp110;});*common=_tmp42A;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_tmp42B=({struct Cyc_List_List*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->hd=(void*)a1s->hd,_tmp111->tl=*res1;_tmp111;});*res1=_tmp42B;});
a1s=a1s->tl;}else{
# 711
({struct Cyc_List_List*_tmp42C=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->hd=(void*)a2s->hd,_tmp112->tl=*res2;_tmp112;});*res2=_tmp42C;});
a2s=a2s->tl;}}}}}_LL0:;}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 718
static struct _tuple15 Cyc_Vcgen_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 720
struct Cyc_List_List*_tmp113=0;struct Cyc_List_List*common=_tmp113;
struct Cyc_List_List*_tmp114=0;struct Cyc_List_List*res1=_tmp114;
struct Cyc_List_List*_tmp115=0;struct Cyc_List_List*res2=_tmp115;
Cyc_Vcgen_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple15 _tmp3A8;({struct Cyc_List_List*_tmp42F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(common);_tmp3A8.f1=_tmp42F;}),({struct Cyc_List_List*_tmp42E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res1);_tmp3A8.f2=_tmp42E;}),({struct Cyc_List_List*_tmp42D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res2);_tmp3A8.f3=_tmp42D;});_tmp3A8;});}struct _tuple16{struct Cyc_List_List*f1;void*f2;};
# 729
static struct _tuple16 Cyc_Vcgen_factor(void*a){
void*_tmp116=a;void*_tmp118;void*_tmp117;void*_tmp11A;void*_tmp119;switch(*((int*)_tmp116)){case 0U: _LL1: _LL2:
 return({struct _tuple16 _tmp3A9;_tmp3A9.f1=0,_tmp3A9.f2=(void*)& Cyc_Vcgen_true_assn;_tmp3A9;});case 1U: _LL3: _LL4:
 return({struct _tuple16 _tmp3AA;_tmp3AA.f1=0,_tmp3AA.f2=(void*)& Cyc_Vcgen_false_assn;_tmp3AA;});case 2U: _LL5: _LL6:
 return({struct _tuple16 _tmp3AB;({struct Cyc_List_List*_tmp430=({struct Cyc_List_List*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->hd=a,_tmp11B->tl=0;_tmp11B;});_tmp3AB.f1=_tmp430;}),_tmp3AB.f2=(void*)& Cyc_Vcgen_true_assn;_tmp3AB;});case 3U: _LL7: _tmp119=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp116)->f1;_tmp11A=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp116)->f2;_LL8: {void*a1=_tmp119;void*a2=_tmp11A;
# 735
struct _tuple16 _tmp11C=Cyc_Vcgen_factor(a1);struct _tuple16 _stmttmpA=_tmp11C;struct _tuple16 _tmp11D=_stmttmpA;void*_tmp11F;struct Cyc_List_List*_tmp11E;_LLE: _tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;_LLF: {struct Cyc_List_List*c1s=_tmp11E;void*a1=_tmp11F;
struct _tuple16 _tmp120=Cyc_Vcgen_factor(a2);struct _tuple16 _stmttmpB=_tmp120;struct _tuple16 _tmp121=_stmttmpB;void*_tmp123;struct Cyc_List_List*_tmp122;_LL11: _tmp122=_tmp121.f1;_tmp123=_tmp121.f2;_LL12: {struct Cyc_List_List*c2s=_tmp122;void*a2=_tmp123;
return({struct _tuple16 _tmp3AC;({struct Cyc_List_List*_tmp432=((struct Cyc_List_List*(*)(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_Vcgen_assncmp,c1s,c2s);_tmp3AC.f1=_tmp432;}),({void*_tmp431=Cyc_Vcgen_and(a1,a2);_tmp3AC.f2=_tmp431;});_tmp3AC;});}}}case 4U: _LL9: _tmp117=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp116)->f1;_tmp118=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp116)->f2;_LLA: {void*a1=_tmp117;void*a2=_tmp118;
# 739
struct _tuple16 _tmp124=Cyc_Vcgen_factor(a1);struct _tuple16 _stmttmpC=_tmp124;struct _tuple16 _tmp125=_stmttmpC;void*_tmp127;struct Cyc_List_List*_tmp126;_LL14: _tmp126=_tmp125.f1;_tmp127=_tmp125.f2;_LL15: {struct Cyc_List_List*c1s=_tmp126;void*a1=_tmp127;
struct _tuple16 _tmp128=Cyc_Vcgen_factor(a2);struct _tuple16 _stmttmpD=_tmp128;struct _tuple16 _tmp129=_stmttmpD;void*_tmp12B;struct Cyc_List_List*_tmp12A;_LL17: _tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;_LL18: {struct Cyc_List_List*c2s=_tmp12A;void*a2=_tmp12B;
struct _tuple15 _tmp12C=Cyc_Vcgen_intersect_assns(c1s,c2s);struct _tuple15 _stmttmpE=_tmp12C;struct _tuple15 _tmp12D=_stmttmpE;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp12E;_LL1A: _tmp12E=_tmp12D.f1;_tmp12F=_tmp12D.f2;_tmp130=_tmp12D.f3;_LL1B: {struct Cyc_List_List*cs=_tmp12E;struct Cyc_List_List*c1s=_tmp12F;struct Cyc_List_List*c2s=_tmp130;
return({struct _tuple16 _tmp3AD;_tmp3AD.f1=cs,({void*_tmp434=({void*_tmp433=((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Vcgen_and,a1,c1s);Cyc_Vcgen_or(_tmp433,((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Vcgen_and,a2,c2s));});_tmp3AD.f2=_tmp434;});_tmp3AD;});}}}}default: _LLB: _LLC:
 return({struct _tuple16 _tmp3AE;_tmp3AE.f1=0,_tmp3AE.f2=a;_tmp3AE;});}_LL0:;}
# 747
static void*Cyc_Vcgen_factor_assn(void*a){
# 749
struct _tuple16 _tmp131=Cyc_Vcgen_factor(a);struct _tuple16 _stmttmpF=_tmp131;struct _tuple16 _tmp132=_stmttmpF;void*_tmp134;struct Cyc_List_List*_tmp133;_LL1: _tmp133=_tmp132.f1;_tmp134=_tmp132.f2;_LL2: {struct Cyc_List_List*cs=_tmp133;void*a=_tmp134;
a=((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Vcgen_and,a,cs);
# 754
return a;}}struct _tuple17{enum Cyc_Vcgen_Primreln f1;enum Cyc_Vcgen_Primreln f2;};
# 761
static int Cyc_Vcgen_find_ctxt(void*ctxt,void*t1,enum Cyc_Vcgen_Primreln p1,void*u1){
LOOP: {
void*_tmp135=ctxt;void*_tmp138;enum Cyc_Vcgen_Primreln _tmp137;void*_tmp136;void*_tmp13A;void*_tmp139;void*_tmp13C;void*_tmp13B;switch(*((int*)_tmp135)){case 1U: _LL1: _LL2:
 return 1;case 0U: _LL3: _LL4:
 return 0;case 3U: _LL5: _tmp13B=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp135)->f1;_tmp13C=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp135)->f2;_LL6: {void*a1=_tmp13B;void*a2=_tmp13C;
# 767
if(Cyc_Vcgen_find_ctxt(a1,t1,p1,u1))return 1;
ctxt=a2;
goto LOOP;}case 4U: _LL7: _tmp139=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp135)->f1;_tmp13A=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp135)->f2;_LL8: {void*a1=_tmp139;void*a2=_tmp13A;
# 771
if(!Cyc_Vcgen_find_ctxt(a1,t1,p1,u1))return 0;
ctxt=a2;
goto LOOP;}case 2U: _LL9: _tmp136=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp135)->f1;_tmp137=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp135)->f2;_tmp138=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp135)->f3;_LLA: {void*t2=_tmp136;enum Cyc_Vcgen_Primreln p2=_tmp137;void*u2=_tmp138;
# 775
struct _tuple17 _tmp13D=({struct _tuple17 _tmp3AF;_tmp3AF.f1=p2,_tmp3AF.f2=p1;_tmp3AF;});struct _tuple17 _stmttmp10=_tmp13D;struct _tuple17 _tmp13E=_stmttmp10;switch(_tmp13E.f1){case Cyc_Vcgen_Neq: if(_tmp13E.f2 == Cyc_Vcgen_Neq){_LLE: _LLF:
 goto _LL11;}else{goto _LL26;}case Cyc_Vcgen_Eq: switch(_tmp13E.f2){case Cyc_Vcgen_Eq: _LL10: _LL11:
 goto _LL13;case Cyc_Vcgen_SLte: _LL12: _LL13:
 goto _LL15;case Cyc_Vcgen_ULte: _LL14: _LL15:
# 780
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2)||
 Cyc_Vcgen_same_term(t1,u2)&& Cyc_Vcgen_same_term(u1,t2);default: goto _LL26;}case Cyc_Vcgen_SLt: switch(_tmp13E.f2){case Cyc_Vcgen_Neq: _LL16: _LL17:
 goto _LL19;case Cyc_Vcgen_SLt: _LL1A: _LL1B:
# 786
 goto _LL1D;case Cyc_Vcgen_SLte: _LL1E: _LL1F:
# 788
 goto _LL21;default: goto _LL26;}case Cyc_Vcgen_ULt: switch(_tmp13E.f2){case Cyc_Vcgen_Neq: _LL18: _LL19:
# 784
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2)||
 Cyc_Vcgen_same_term(t1,u2)&& Cyc_Vcgen_same_term(u1,t2);case Cyc_Vcgen_ULt: _LL1C: _LL1D:
# 787
 goto _LL1F;case Cyc_Vcgen_ULte: _LL20: _LL21:
# 789
 goto _LL23;default: goto _LL26;}case Cyc_Vcgen_SLte: if(_tmp13E.f2 == Cyc_Vcgen_SLte){_LL22: _LL23:
 goto _LL25;}else{goto _LL26;}case Cyc_Vcgen_ULte: if(_tmp13E.f2 == Cyc_Vcgen_ULte){_LL24: _LL25:
# 792
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2);}else{goto _LL26;}default: _LL26: _LL27:
 return 0;}_LLD:;}default: _LLB: _LLC:
# 796
 ctxt=Cyc_Vcgen_reduce(ctxt);
goto LOOP;}_LL0:;}}
# 803
static int Cyc_Vcgen_simple_prove(void*ctxt,void*a){
LOOP:
 if(ctxt == a)return 1;{
void*_tmp13F=a;void*_tmp142;enum Cyc_Vcgen_Primreln _tmp141;void*_tmp140;void*_tmp144;void*_tmp143;void*_tmp146;void*_tmp145;switch(*((int*)_tmp13F)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 return 0;case 3U: _LL5: _tmp145=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp13F)->f1;_tmp146=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp13F)->f2;_LL6: {void*a1=_tmp145;void*a2=_tmp146;
# 810
if(!Cyc_Vcgen_simple_prove(ctxt,a1))return 0;
a=a2;
goto LOOP;}case 4U: _LL7: _tmp143=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp13F)->f1;_tmp144=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp13F)->f2;_LL8: {void*a1=_tmp143;void*a2=_tmp144;
# 814
if(Cyc_Vcgen_simple_prove(ctxt,a1))return 1;
a=a2;
goto LOOP;}case 2U: _LL9: _tmp140=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp13F)->f1;_tmp141=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp13F)->f2;_tmp142=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp13F)->f3;_LLA: {void*t1=_tmp140;enum Cyc_Vcgen_Primreln p=_tmp141;void*t2=_tmp142;
return Cyc_Vcgen_find_ctxt(ctxt,t1,p,t2);}default: _LLB: _LLC:
# 819
 a=Cyc_Vcgen_reduce(a);
goto LOOP;}_LL0:;}}struct Cyc_Vcgen_Distance{struct Cyc_Vcgen_Distance*next;void*t1;void*t2;int dist;};struct Cyc_Vcgen_Graph{struct Cyc_Vcgen_Graph*next;struct Cyc_List_List*nodes;struct Cyc_Vcgen_Distance*distance;};
# 855 "vcgen.cyc"
static void Cyc_Vcgen_print_graphs(struct Cyc_Vcgen_Graph*gs){
({void*_tmp147=0U;({struct Cyc___cycFILE*_tmp436=Cyc_stderr;struct _fat_ptr _tmp435=({const char*_tmp148="Graphs:\n";_tag_fat(_tmp148,sizeof(char),9U);});Cyc_fprintf(_tmp436,_tmp435,_tag_fat(_tmp147,sizeof(void*),0U));});});
for(0;gs != 0;gs=gs->next){
({void*_tmp149=0U;({struct Cyc___cycFILE*_tmp438=Cyc_stderr;struct _fat_ptr _tmp437=({const char*_tmp14A="\t{";_tag_fat(_tmp14A,sizeof(char),3U);});Cyc_fprintf(_tmp438,_tmp437,_tag_fat(_tmp149,sizeof(void*),0U));});});
{struct Cyc_Vcgen_Distance*_tmp14B=gs->distance;struct Cyc_Vcgen_Distance*ds=_tmp14B;for(0;ds != 0;ds=ds->next){
({struct Cyc_String_pa_PrintArg_struct _tmp14E=({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0U,({struct _fat_ptr _tmp439=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t1));_tmp3B2.f1=_tmp439;});_tmp3B2;});struct Cyc_String_pa_PrintArg_struct _tmp14F=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0U,({struct _fat_ptr _tmp43A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t2));_tmp3B1.f1=_tmp43A;});_tmp3B1;});struct Cyc_Int_pa_PrintArg_struct _tmp150=({struct Cyc_Int_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=1U,_tmp3B0.f1=(unsigned long)ds->dist;_tmp3B0;});void*_tmp14C[3U];_tmp14C[0]=& _tmp14E,_tmp14C[1]=& _tmp14F,_tmp14C[2]=& _tmp150;({struct Cyc___cycFILE*_tmp43C=Cyc_stderr;struct _fat_ptr _tmp43B=({const char*_tmp14D="%s - %s <= %d";_tag_fat(_tmp14D,sizeof(char),14U);});Cyc_fprintf(_tmp43C,_tmp43B,_tag_fat(_tmp14C,sizeof(void*),3U));});});
# 862
if(ds->next != 0)({void*_tmp151=0U;({struct Cyc___cycFILE*_tmp43E=Cyc_stderr;struct _fat_ptr _tmp43D=({const char*_tmp152=",";_tag_fat(_tmp152,sizeof(char),2U);});Cyc_fprintf(_tmp43E,_tmp43D,_tag_fat(_tmp151,sizeof(void*),0U));});});}}
# 864
({void*_tmp153=0U;({struct Cyc___cycFILE*_tmp440=Cyc_stderr;struct _fat_ptr _tmp43F=({const char*_tmp154="}\n";_tag_fat(_tmp154,sizeof(char),3U);});Cyc_fprintf(_tmp440,_tmp43F,_tag_fat(_tmp153,sizeof(void*),0U));});});}}
# 869
static unsigned Cyc_Vcgen_num_graphs(struct Cyc_Vcgen_Graph*gs){
unsigned n=0U;
for(0;gs != 0;gs=gs->next){
++ n;}
return n;}
# 878
static struct Cyc_Vcgen_Graph*Cyc_Vcgen_true_graph (void){return({struct Cyc_Vcgen_Graph*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->next=0,_tmp155->nodes=0,_tmp155->distance=0;_tmp155;});}
# 881
static struct Cyc_Vcgen_Distance*Cyc_Vcgen_copy_distance(struct Cyc_Vcgen_Distance*ds){
struct Cyc_Vcgen_Distance*res=0;
for(0;ds != 0;ds=ds->next){
res=({struct Cyc_Vcgen_Distance*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->next=res,_tmp156->t1=ds->t1,_tmp156->t2=ds->t2,_tmp156->dist=ds->dist;_tmp156;});}
# 886
return res;}
# 890
static struct Cyc_Vcgen_Graph*Cyc_Vcgen_copy_graph(struct Cyc_Vcgen_Graph*g){
struct Cyc_Vcgen_Graph*res=0;
for(0;g != 0;g=g->next){
res=({struct Cyc_Vcgen_Graph*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->next=res,_tmp157->nodes=g->nodes,({struct Cyc_Vcgen_Distance*_tmp441=Cyc_Vcgen_copy_distance(g->distance);_tmp157->distance=_tmp441;});_tmp157;});}
# 895
return res;}
# 900
static struct Cyc_Vcgen_Graph*Cyc_Vcgen_graph_append(struct Cyc_Vcgen_Graph*g1,struct Cyc_Vcgen_Graph*g2){
if(g1 == 0)return g2;
if(g2 == 0)return g1;{
struct Cyc_Vcgen_Graph*p=g1;
{struct Cyc_Vcgen_Graph*_tmp158=p->next;struct Cyc_Vcgen_Graph*x=_tmp158;for(0;x != 0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}
# 910
static void Cyc_Vcgen_add_node(struct Cyc_Vcgen_Graph*g,void*n){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Vcgen_cmp_term,g->nodes,n))
({struct Cyc_List_List*_tmp442=({struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->hd=n,_tmp159->tl=g->nodes;_tmp159;});g->nodes=_tmp442;});}
# 917
static int*Cyc_Vcgen_lookup_distance(struct Cyc_Vcgen_Graph*g,void*i,void*j){
static int zero=0;
if(Cyc_Vcgen_cmp_term(i,j)== 0)return& zero;
{struct Cyc_Vcgen_Distance*_tmp15A=g->distance;struct Cyc_Vcgen_Distance*ds=_tmp15A;for(0;ds != 0;ds=ds->next){
if(Cyc_Vcgen_cmp_term(i,ds->t1)== 0 && Cyc_Vcgen_cmp_term(j,ds->t2)== 0)return& ds->dist;}}
# 923
return 0;}
# 929
static int Cyc_Vcgen_add_edge(struct Cyc_Vcgen_Graph*g,void*i,void*j,int dist){
Cyc_Vcgen_add_node(g,i);
Cyc_Vcgen_add_node(g,j);{
int*ij_dist=Cyc_Vcgen_lookup_distance(g,i,j);
# 934
if(ij_dist != 0 &&*ij_dist < dist)
return 1;{
int*ji_dist=Cyc_Vcgen_lookup_distance(g,j,i);
# 938
if(ji_dist != 0 &&*ji_dist + dist < 0)
return 0;
# 941
{struct Cyc_List_List*_tmp15B=g->nodes;struct Cyc_List_List*ks=_tmp15B;for(0;ks != 0;ks=ks->tl){
void*_tmp15C=(void*)ks->hd;void*k=_tmp15C;
int*_tmp15D=Cyc_Vcgen_lookup_distance(g,k,i);int*ki_dist=_tmp15D;
if(ki_dist == 0)continue;{
struct Cyc_List_List*_tmp15E=g->nodes;struct Cyc_List_List*ls=_tmp15E;for(0;ls != 0;ls=ls->tl){
void*_tmp15F=(void*)ls->hd;void*l=_tmp15F;
int*_tmp160=Cyc_Vcgen_lookup_distance(g,j,l);int*jl_dist=_tmp160;
if(jl_dist != 0){
int*_tmp161=Cyc_Vcgen_lookup_distance(g,k,l);int*kl_dist=_tmp161;
int _tmp162=(*ki_dist + dist)+ *jl_dist;int new_dist=_tmp162;
if(kl_dist == 0)
({struct Cyc_Vcgen_Distance*_tmp443=({struct Cyc_Vcgen_Distance*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->next=g->distance,_tmp163->t1=k,_tmp163->t2=l,_tmp163->dist=new_dist;_tmp163;});g->distance=_tmp443;});else{
if(*kl_dist > new_dist)
*kl_dist=new_dist;}}}}}}
# 959
return 1;}}}
# 964
static int Cyc_Vcgen_add_constraint(struct Cyc_Vcgen_Graph*g,void*t1,enum Cyc_Vcgen_Primreln p,void*t2){
# 967
enum Cyc_Vcgen_Primreln _tmp164=p;switch(_tmp164){case Cyc_Vcgen_Eq: _LL1: _LL2:
# 970
 if(!Cyc_Vcgen_add_edge(g,t1,t2,0))return 0;
return Cyc_Vcgen_add_edge(g,t2,t1,0);case Cyc_Vcgen_ULt: _LL3: _LL4:
# 974
 return Cyc_Vcgen_add_edge(g,t1,t2,- 1);case Cyc_Vcgen_ULte: _LL5: _LL6:
# 977
 return Cyc_Vcgen_add_edge(g,t1,t2,0);case Cyc_Vcgen_SLt: _LL7: _LL8:
 goto _LLA;case Cyc_Vcgen_SLte: _LL9: _LLA:
# 981
 return 1;default: _LLB: _LLC:
({void*_tmp165=0U;({struct _fat_ptr _tmp444=({const char*_tmp166="Vcgen:found bad primop in add_constraint";_tag_fat(_tmp166,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp444,_tag_fat(_tmp165,sizeof(void*),0U));});});}_LL0:;}char Cyc_Vcgen_TooLarge[9U]="TooLarge";struct Cyc_Vcgen_TooLarge_exn_struct{char*tag;};
# 987
struct Cyc_Vcgen_TooLarge_exn_struct Cyc_Vcgen_too_large={Cyc_Vcgen_TooLarge};
# 990
unsigned Cyc_Vcgen_max_paths=33U;
unsigned Cyc_Vcgen_max_paths_seen=0U;
# 999
static struct Cyc_Vcgen_Graph*Cyc_Vcgen_cgraph(struct Cyc_Vcgen_Graph*gs,void*a){
LOOP:
 if(gs == 0)return gs;
{void*_tmp167=a;void*_tmp16A;enum Cyc_Vcgen_Primreln _tmp169;void*_tmp168;void*_tmp16C;void*_tmp16B;void*_tmp16E;void*_tmp16D;void*_tmp170;void*_tmp16F;switch(*((int*)_tmp167)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 gs=0;goto _LL0;case 3U: _LL5: _tmp16F=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp167)->f1;_tmp170=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp167)->f2;_LL6: {void*a1=_tmp16F;void*a2=_tmp170;
# 1006
gs=Cyc_Vcgen_cgraph(gs,a2);
a=a1;
goto LOOP;}case 4U: _LL7: _tmp16D=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp167)->f1;_tmp16E=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp167)->f2;_LL8: {void*a1=_tmp16D;void*a2=_tmp16E;
# 1010
unsigned n=Cyc_Vcgen_num_graphs(gs);
if(Cyc_Vcgen_max_paths_seen < n)Cyc_Vcgen_max_paths_seen=n;
# 1013
if(n >= Cyc_Vcgen_max_paths)(int)_throw((void*)& Cyc_Vcgen_too_large);{
# 1015
struct Cyc_Vcgen_Graph*gs1=gs;
struct Cyc_Vcgen_Graph*gs2=Cyc_Vcgen_copy_graph(gs);
# 1018
gs1=Cyc_Vcgen_cgraph(gs1,a1);
# 1020
gs2=Cyc_Vcgen_cgraph(gs2,a2);
# 1022
gs=Cyc_Vcgen_graph_append(gs1,gs2);
goto _LL0;}}case 2U: if(((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp167)->f2 == Cyc_Vcgen_Neq){_LL9: _tmp16B=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp167)->f1;_tmp16C=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp167)->f3;_LLA: {void*t1=_tmp16B;void*t2=_tmp16C;
# 1026
a=({void*_tmp445=Cyc_Vcgen_ult(t1,t2);Cyc_Vcgen_or(_tmp445,Cyc_Vcgen_ult(t2,t1));});
goto LOOP;}}else{_LLB: _tmp168=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp167)->f1;_tmp169=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp167)->f2;_tmp16A=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp167)->f3;_LLC: {void*t1=_tmp168;enum Cyc_Vcgen_Primreln p=_tmp169;void*t2=_tmp16A;
# 1031
struct Cyc_Vcgen_Graph*prev=0;
{struct Cyc_Vcgen_Graph*_tmp171=gs;struct Cyc_Vcgen_Graph*g=_tmp171;for(0;g != 0;g=g->next){
if(!Cyc_Vcgen_add_constraint(g,t1,p,t2)){
if(prev == 0)
gs=g->next;else{
# 1037
prev->next=g->next;}}else{
# 1040
prev=g;}}}
# 1043
goto _LL0;}}default: _LLD: _LLE:
# 1046
 a=Cyc_Vcgen_reduce(a);
goto LOOP;}_LL0:;}
# 1050
return gs;}
# 1056
static int Cyc_Vcgen_consistent(void*a){
struct _handler_cons _tmp172;_push_handler(& _tmp172);{int _tmp174=0;if(setjmp(_tmp172.handler))_tmp174=1;if(!_tmp174){
# 1059
{struct _fat_ptr gs=_tag_fat(({struct Cyc_Vcgen_Graph*_tmp446=Cyc_Vcgen_true_graph();Cyc_Vcgen_cgraph(_tmp446,a);}),sizeof(struct Cyc_Vcgen_Graph),1U);
# 1062
int _tmp175=({struct Cyc_Vcgen_Graph*_tmp447=(struct Cyc_Vcgen_Graph*)gs.curr;_tmp447 != (struct Cyc_Vcgen_Graph*)(_tag_fat(0,0,0)).curr;});_npop_handler(0U);return _tmp175;}
# 1059
;_pop_handler();}else{void*_tmp173=(void*)Cyc_Core_get_exn_thrown();void*_tmp176=_tmp173;void*_tmp177;if(((struct Cyc_Vcgen_TooLarge_exn_struct*)_tmp176)->tag == Cyc_Vcgen_TooLarge){_LL1: _LL2:
# 1066
 return 1;}else{_LL3: _tmp177=_tmp176;_LL4: {void*exn=_tmp177;(int)_rethrow(exn);}}_LL0:;}}}
# 1071
static int Cyc_Vcgen_constraint_prove(void*ctxt,void*a){
void*b=({void*_tmp448=ctxt;Cyc_Vcgen_and(_tmp448,Cyc_Vcgen_not(a));});
return !Cyc_Vcgen_consistent(b);}
# 1078
static int Cyc_Vcgen_implies_noreduce(void*a1,void*a2){
return Cyc_Vcgen_simple_prove(a1,a2)|| Cyc_Vcgen_constraint_prove(a1,a2);}
# 1084
static int Cyc_Vcgen_implies(void**a1,void**a2){
({void*_tmp449=Cyc_Vcgen_reduce(*a1);*a1=_tmp449;});
({void*_tmp44A=Cyc_Vcgen_reduce(*a2);*a2=_tmp44A;});
return Cyc_Vcgen_implies_noreduce(*a1,*a2);}
# 1095
static void*Cyc_Vcgen_weaken_it(void*ctxt,void*a){
if(Cyc_Vcgen_simple_prove(ctxt,a))return a;{
void*res;
{void*_tmp178=a;void*_tmp17A;void*_tmp179;void*_tmp17C;void*_tmp17B;switch(*((int*)_tmp178)){case 3U: _LL1: _tmp17B=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp178)->f1;_tmp17C=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp178)->f2;_LL2: {void*a1=_tmp17B;void*a2=_tmp17C;
# 1100
void*_tmp17D=Cyc_Vcgen_weaken_it(ctxt,a1);void*b1=_tmp17D;
void*_tmp17E=Cyc_Vcgen_weaken_it(ctxt,a2);void*b2=_tmp17E;
# 1104
res=Cyc_Vcgen_and(b1,b2);
goto _LL0;}case 4U: _LL3: _tmp179=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp178)->f1;_tmp17A=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp178)->f2;_LL4: {void*a1=_tmp179;void*a2=_tmp17A;
# 1107
void*_tmp17F=Cyc_Vcgen_weaken_it(a2,a1);void*b1=_tmp17F;
void*_tmp180=Cyc_Vcgen_weaken_it(a1,a2);void*b2=_tmp180;
res=({void*_tmp44B=ctxt;Cyc_Vcgen_weaken_it(_tmp44B,Cyc_Vcgen_and(b1,b2));});
goto _LL0;}case 0U: _LL5: _LL6:
 goto _LL8;case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA:
# 1114
 res=(void*)& Cyc_Vcgen_true_assn;
goto _LL0;default: _LLB: _LLC:
 return({void*_tmp44C=ctxt;Cyc_Vcgen_weaken_it(_tmp44C,Cyc_Vcgen_reduce(a));});}_LL0:;}
# 1118
return res;}}
# 1127
static void*Cyc_Vcgen_weaken_assn(void*ctxt,void*a){
if(Cyc_Vcgen_implies(& ctxt,& a))return a;
return Cyc_Vcgen_weaken_it(ctxt,a);}
# 1133
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd);
# 1136
static void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_tmp181=tgt;struct Cyc_Absyn_Vardecl*_tmp182;if(_tmp181 == 0){_LL1: _LL2:
# 1141
 return Cyc_Vcgen_kill(a);}else{if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp181)->tag == 1U){_LL3: _tmp182=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp181)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp182;
# 1147
if(vd->escapes){
a=Cyc_Vcgen_kill(a);
src=src == 0?0: Cyc_Vcgen_kill_mem_term(src);}else{
# 1151
void*_tmp183=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmp183;
a=Cyc_Vcgen_subst(vd,v2,a);
src=src == 0?0: Cyc_Vcgen_subst_term(src,vd,v2);}
# 1156
return({void*_tmp44D=a;Cyc_Vcgen_and(_tmp44D,Cyc_Vcgen_eq(tgt,src));});}}else{_LL5: _LL6:
({void*_tmp184=0U;({struct _fat_ptr _tmp44E=({const char*_tmp185="vcgen do_assign";_tag_fat(_tmp185,sizeof(char),16U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp44E,_tag_fat(_tmp184,sizeof(void*),0U));});});}}_LL0:;}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void*try_assn;void**exp_stmt;void*res_term;void*res_assn;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;};
# 1195 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 1197
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
# 1199
struct Cyc_Hashtable_Table*_tmp186=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);struct Cyc_Hashtable_Table*succ_table=_tmp186;
struct Cyc_Hashtable_Table*_tmp187=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_table=_tmp187;
struct Cyc_Vcgen_SharedEnv*_tmp188=({struct Cyc_Vcgen_SharedEnv*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->assn_table=assn_table,_tmp18B->succ_table=succ_table,_tmp18B->try_assn=(void*)& Cyc_Vcgen_false_assn,_tmp18B->exp_stmt=0,_tmp18B->res_term=0,_tmp18B->res_assn=(void*)& Cyc_Vcgen_false_assn;_tmp18B;});struct Cyc_Vcgen_SharedEnv*senv=_tmp188;
# 1207
struct Cyc_Dict_Dict _tmp189=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty)((int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp);struct Cyc_Dict_Dict d=_tmp189;
return({struct Cyc_Vcgen_Env*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A->shared=senv,_tmp18A->state_counter=d;_tmp18A;});}
# 1211
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_tmp18C=_cycalloc(sizeof(*_tmp18C));*_tmp18C=*env;_tmp18C;});}
# 1217
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 1221
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _tmp44F=((struct Cyc_Dict_Dict(*)(int(*f)(int,struct Cyc_Absyn_Vardecl*,int,int),int env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_tmp44F;});}
# 1229
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Vardecl*k,int*ans))Cyc_Dict_lookup_bool)(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _tmp450=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Vardecl*k,int v))Cyc_Dict_insert)(env->state_counter,vd,res + 1);env->state_counter=_tmp450;});
return(void*)({struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->tag=2U,_tmp18D->f1=vd,_tmp18D->f2=res;_tmp18D;});}
# 1238
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***_tmp18E=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);void***popt=_tmp18E;
if(popt != 0){
void**p=*popt;
void*_tmp18F=Cyc_Vcgen_or(ain,*p);void*new_precondition=_tmp18F;
*p=new_precondition;
return new_precondition;}else{
# 1246
return ain;}}
# 1250
static void**Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
static void*false_pointer=(void*)& Cyc_Vcgen_false_assn;
void***popt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(popt != 0)
return*popt;else{
# 1256
return& false_pointer;}}
# 1261
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp190=*aprevopt;void**aprev=_tmp190;
ain=Cyc_Vcgen_weaken_assn(ain,*aprev);
*aprev=ain;}else{
# 1268
({struct Cyc_Hashtable_Table*_tmp452=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp451=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,void**val))Cyc_Hashtable_insert)(_tmp452,_tmp451,({void**_tmp191=_cycalloc(sizeof(*_tmp191));*_tmp191=ain;_tmp191;}));});}
# 1270
return ain;}
# 1275
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp192=*aprevopt;void**aprev=_tmp192;
ain=Cyc_Vcgen_or(ain,*aprev);
*aprev=ain;}else{
# 1282
({struct Cyc_Hashtable_Table*_tmp454=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp453=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,void**val))Cyc_Hashtable_insert)(_tmp454,_tmp453,({void**_tmp193=_cycalloc(sizeof(*_tmp193));*_tmp193=ain;_tmp193;}));});}
# 1284
return ain;}
# 1289
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
({void*_tmp455=Cyc_Vcgen_or((env->shared)->try_assn,a);(env->shared)->try_assn=_tmp455;});}
# 1294
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a){
struct Cyc_Vcgen_SharedEnv*_tmp194=env->shared;struct Cyc_Vcgen_SharedEnv*shared=_tmp194;
if(shared->res_term != 0 || t != 0){
void*_tmp195=Cyc_Vcgen_fresh_var();void*v=_tmp195;
shared->res_term=v;
({void*_tmp459=({
void*_tmp458=({void*_tmp456=a;Cyc_Vcgen_and(_tmp456,Cyc_Vcgen_eq(v,t));});Cyc_Vcgen_or(_tmp458,({void*_tmp457=shared->res_assn;Cyc_Vcgen_and(_tmp457,Cyc_Vcgen_eq(v,shared->res_term));}));});
# 1299
shared->res_assn=_tmp459;});}else{
# 1302
({void*_tmp45A=Cyc_Vcgen_or(a,shared->res_assn);shared->res_assn=_tmp45A;});}}
# 1311
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain);
# 1315
static struct _tuple11 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 1317
static struct _tuple11 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 1319
static struct _tuple11 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 1322
static struct _tuple11 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);struct _tuple18{struct Cyc_List_List**f1;void*f2;};
# 1326
static struct _tuple18 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a);
# 1329
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a);
# 1331
static struct _tuple16 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain);
# 1333
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain);
# 1335
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a);
# 1337
static struct _fat_ptr Cyc_Vcgen_stmt2shortstring(struct Cyc_Absyn_Stmt*s){
void*_tmp196=s->r;void*_stmttmp11=_tmp196;void*_tmp197=_stmttmp11;struct Cyc_Absyn_Stmt*_tmp198;struct Cyc_Absyn_Stmt*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;switch(*((int*)_tmp197)){case 4U: _LL1: _tmp1A0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp197)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp1A0;
# 1340
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A3=({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0U,({struct _fat_ptr _tmp45B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3B3.f1=_tmp45B;});_tmp3B3;});void*_tmp1A1[1U];_tmp1A1[0]=& _tmp1A3;({struct _fat_ptr _tmp45C=({const char*_tmp1A2="if (%s) ...";_tag_fat(_tmp1A2,sizeof(char),12U);});Cyc_aprintf(_tmp45C,_tag_fat(_tmp1A1,sizeof(void*),1U));});});}case 5U: _LL3: _tmp19F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp197)->f1).f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp19F;
# 1342
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A6=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0U,({struct _fat_ptr _tmp45D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3B4.f1=_tmp45D;});_tmp3B4;});void*_tmp1A4[1U];_tmp1A4[0]=& _tmp1A6;({struct _fat_ptr _tmp45E=({const char*_tmp1A5="while (%s) ...";_tag_fat(_tmp1A5,sizeof(char),15U);});Cyc_aprintf(_tmp45E,_tag_fat(_tmp1A4,sizeof(void*),1U));});});}case 9U: _LL5: _tmp19C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp197)->f1;_tmp19D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp197)->f2).f1;_tmp19E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp197)->f3).f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp19C;struct Cyc_Absyn_Exp*e2=_tmp19D;struct Cyc_Absyn_Exp*e3=_tmp19E;
# 1344
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A9=({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0U,({struct _fat_ptr _tmp45F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp3B7.f1=_tmp45F;});_tmp3B7;});struct Cyc_String_pa_PrintArg_struct _tmp1AA=({struct Cyc_String_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=0U,({
struct _fat_ptr _tmp460=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e2));_tmp3B6.f1=_tmp460;});_tmp3B6;});struct Cyc_String_pa_PrintArg_struct _tmp1AB=({struct Cyc_String_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=0U,({struct _fat_ptr _tmp461=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e3));_tmp3B5.f1=_tmp461;});_tmp3B5;});void*_tmp1A7[3U];_tmp1A7[0]=& _tmp1A9,_tmp1A7[1]=& _tmp1AA,_tmp1A7[2]=& _tmp1AB;({struct _fat_ptr _tmp462=({const char*_tmp1A8="for (%s;%s;%s) ...";_tag_fat(_tmp1A8,sizeof(char),19U);});Cyc_aprintf(_tmp462,_tag_fat(_tmp1A7,sizeof(void*),3U));});});}case 10U: _LL7: _tmp19B=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp197)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp19B;
# 1347
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1AE=({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0U,({struct _fat_ptr _tmp463=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3B8.f1=_tmp463;});_tmp3B8;});void*_tmp1AC[1U];_tmp1AC[0]=& _tmp1AE;({struct _fat_ptr _tmp464=({const char*_tmp1AD="switch (%s) ...";_tag_fat(_tmp1AD,sizeof(char),16U);});Cyc_aprintf(_tmp464,_tag_fat(_tmp1AC,sizeof(void*),1U));});});}case 14U: _LL9: _tmp19A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp197)->f2).f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp19A;
# 1349
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0U,({struct _fat_ptr _tmp465=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3B9.f1=_tmp465;});_tmp3B9;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _fat_ptr _tmp466=({const char*_tmp1B0="do ... while (%s)";_tag_fat(_tmp1B0,sizeof(char),18U);});Cyc_aprintf(_tmp466,_tag_fat(_tmp1AF,sizeof(void*),1U));});});}case 15U: _LLB: _tmp199=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp197)->f1;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp199;
# 1351
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0U,({struct _fat_ptr _tmp467=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_stmt2shortstring(s));_tmp3BA.f1=_tmp467;});_tmp3BA;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _fat_ptr _tmp468=({const char*_tmp1B3="try %s catch ...";_tag_fat(_tmp1B3,sizeof(char),17U);});Cyc_aprintf(_tmp468,_tag_fat(_tmp1B2,sizeof(void*),1U));});});}case 2U: _LLD: _tmp198=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp197)->f1;_LLE: {struct Cyc_Absyn_Stmt*s=_tmp198;
return Cyc_Vcgen_stmt2shortstring(s);}default: _LLF: _LL10: {
# 1354
struct _fat_ptr _tmp1B5=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_tmp1B5;
unsigned max=_get_fat_size(s,sizeof(char))< (unsigned)11?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _tmp1B9=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_tmp1B9),1U,_tmp1B9);});
{int i=0;for(0;(unsigned)i < max - (unsigned)1;++ i){
({char _tmp469=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_tmp469;});}}
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B8=({struct Cyc_String_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=0U,_tmp3BB.f1=(struct _fat_ptr)((struct _fat_ptr)res);_tmp3BB;});void*_tmp1B6[1U];_tmp1B6[0]=& _tmp1B8;({struct _fat_ptr _tmp46A=({const char*_tmp1B7="%s...";_tag_fat(_tmp1B7,sizeof(char),6U);});Cyc_aprintf(_tmp46A,_tag_fat(_tmp1B6,sizeof(void*),1U));});});}}_LL0:;}struct _tuple19{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 1413 "vcgen.cyc"
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 1416
LOOP:
 ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);
# 1421
{void*_tmp1BA=ain;if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp1BA)->tag == 1U){_LL1: _LL2:
 return ain;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1425
void*_tmp1BB=s->r;void*_stmttmp12=_tmp1BB;void*_tmp1BC=_stmttmp12;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_Stmt*_tmp1BD;struct Cyc_Absyn_Stmt*_tmp1BF;struct Cyc_Absyn_Stmt*_tmp1C1;struct Cyc_Absyn_Decl*_tmp1C0;struct Cyc_Absyn_Switch_clause*_tmp1C3;struct Cyc_List_List*_tmp1C2;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Stmt*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_Absyn_Stmt*_tmp1C6;struct Cyc_Absyn_Stmt*_tmp1CE;struct Cyc_Absyn_Stmt*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_Absyn_Stmt*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_Absyn_Stmt*_tmp1D1;struct Cyc_Absyn_Stmt*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_Absyn_Stmt*_tmp1D4;struct Cyc_Absyn_Stmt*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Stmt*_tmp1D7;struct Cyc_Absyn_Stmt*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D8;switch(*((int*)_tmp1BC)){case 0U: _LL6: _LL7:
 return ain;case 1U: _LL8: _tmp1D8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_LL9: {struct Cyc_Absyn_Exp*e=_tmp1D8;
# 1428
struct _tuple11 _tmp1D9=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp13=_tmp1D9;struct _tuple11 _tmp1DA=_stmttmp13;void*_tmp1DC;void*_tmp1DB;_LL2B: _tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;_LL2C: {void*t=_tmp1DB;void*aout=_tmp1DC;
# 1431
if((env->shared)->exp_stmt != 0)
*((void**)_check_null((env->shared)->exp_stmt))=t;
return aout;}}case 2U: _LLA: _tmp1D6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1D7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2;_LLB: {struct Cyc_Absyn_Stmt*s1=_tmp1D6;struct Cyc_Absyn_Stmt*s2=_tmp1D7;
# 1435
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain);
s=s2;
goto LOOP;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1 == 0){_LLC: _LLD:
# 1439
 Cyc_Vcgen_update_return(env,0,ain);
return(void*)& Cyc_Vcgen_false_assn;}else{_LLE: _tmp1D5=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_LLF: {struct Cyc_Absyn_Exp*e=_tmp1D5;
# 1442
struct _tuple11 _tmp1DD=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(e),ain);struct _tuple11 _stmttmp14=_tmp1DD;struct _tuple11 _tmp1DE=_stmttmp14;void*_tmp1E0;void*_tmp1DF;_LL2E: _tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;_LL2F: {void*t=_tmp1DF;void*aout=_tmp1E0;
Cyc_Vcgen_update_return(env,t,aout);
return(void*)& Cyc_Vcgen_false_assn;}}}case 4U: _LL10: _tmp1D2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1D3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2;_tmp1D4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f3;_LL11: {struct Cyc_Absyn_Exp*e1=_tmp1D2;struct Cyc_Absyn_Stmt*s1=_tmp1D3;struct Cyc_Absyn_Stmt*s2=_tmp1D4;
# 1446
struct _tuple11 _tmp1E1=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp15=_tmp1E1;struct _tuple11 _tmp1E2=_stmttmp15;void*_tmp1E4;void*_tmp1E3;_LL31: _tmp1E3=_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;_LL32: {void*at=_tmp1E3;void*af=_tmp1E4;
struct Cyc_Vcgen_Env*_tmp1E5=Cyc_Vcgen_copy_env(env);struct Cyc_Vcgen_Env*env2=_tmp1E5;
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return Cyc_Vcgen_or(at,af);}}case 5U: _LL12: _tmp1CF=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1).f1;_tmp1D0=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1).f2;_tmp1D1=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2;_LL13: {struct Cyc_Absyn_Exp*e=_tmp1CF;struct Cyc_Absyn_Stmt*dummy=_tmp1D0;struct Cyc_Absyn_Stmt*s=_tmp1D1;
# 1458
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);{
struct _tuple11 _tmp1E6=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple11 _stmttmp16=_tmp1E6;struct _tuple11 _tmp1E7=_stmttmp16;void*_tmp1E9;void*_tmp1E8;_LL34: _tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;_LL35: {void*at=_tmp1E8;void*af=_tmp1E9;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7U: _LL14: _LL15:
# 1464
 goto _LL17;{
# 1467
struct Cyc_Absyn_Stmt*_tmp1EA=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp1EA;
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_Vcgen_false_assn;}case 6U: _LL16: _LL17: {
# 1474
struct Cyc_Absyn_Stmt*_tmp1EB=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp1EB;
if(dest == 0)
Cyc_Vcgen_update_return(env,0,ain);else{
# 1478
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_Vcgen_false_assn;}case 9U: _LL18: _tmp1C9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1CA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2).f1;_tmp1CB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2).f2;_tmp1CC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f3).f1;_tmp1CD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f3).f2;_tmp1CE=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f4;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp1C9;struct Cyc_Absyn_Exp*e2=_tmp1CA;struct Cyc_Absyn_Stmt*dummy2=_tmp1CB;struct Cyc_Absyn_Exp*e3=_tmp1CC;struct Cyc_Absyn_Stmt*dummy3=_tmp1CD;struct Cyc_Absyn_Stmt*s=_tmp1CE;
# 1481
struct _tuple11 _tmp1EC=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,ain);struct _tuple11 _stmttmp17=_tmp1EC;struct _tuple11 _tmp1ED=_stmttmp17;void*_tmp1EE;_LL37: _tmp1EE=_tmp1ED.f2;_LL38: {void*a=_tmp1EE;
ain=a;
# 1484
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);{
struct _tuple11 _tmp1EF=Cyc_Vcgen_vcgen_test(env,e2,ain);struct _tuple11 _stmttmp18=_tmp1EF;struct _tuple11 _tmp1F0=_stmttmp18;void*_tmp1F2;void*_tmp1F1;_LL3A: _tmp1F1=_tmp1F0.f1;_tmp1F2=_tmp1F0.f2;_LL3B: {void*at=_tmp1F1;void*af=_tmp1F2;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 1489
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain)).f2;
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2)))return af;}}}}}case 14U: _LL1A: _tmp1C6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1C7=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2).f1;_tmp1C8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2).f2;_LL1B: {struct Cyc_Absyn_Stmt*s=_tmp1C6;struct Cyc_Absyn_Exp*e=_tmp1C7;struct Cyc_Absyn_Stmt*dummy=_tmp1C8;
# 1495
while(1){
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple11 _tmp1F3=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple11 _stmttmp19=_tmp1F3;struct _tuple11 _tmp1F4=_stmttmp19;void*_tmp1F6;void*_tmp1F5;_LL3D: _tmp1F5=_tmp1F4.f1;_tmp1F6=_tmp1F4.f2;_LL3E: {void*at=_tmp1F5;void*af=_tmp1F6;
ain=({void*_tmp46B=*Cyc_Vcgen_get_assn(env,dummy);Cyc_Vcgen_weaken_assn(_tmp46B,ain);});
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10U: _LL1C: _tmp1C4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1C5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2;_LL1D: {struct Cyc_Absyn_Exp*e=_tmp1C4;struct Cyc_List_List*scs=_tmp1C5;
# 1508
struct _tuple11 _tmp1F7=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp1A=_tmp1F7;struct _tuple11 _tmp1F8=_stmttmp1A;void*_tmp1FA;void*_tmp1F9;_LL40: _tmp1F9=_tmp1F8.f1;_tmp1FA=_tmp1F8.f2;_LL41: {void*t=_tmp1F9;void*a=_tmp1FA;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2 == 0){_LL1E: _LL1F:
({void*_tmp1FB=0U;({struct _fat_ptr _tmp46C=({const char*_tmp1FC="vcgen fallthru";_tag_fat(_tmp1FC,sizeof(char),15U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp46C,_tag_fat(_tmp1FB,sizeof(void*),0U));});});}else{_LL20: _tmp1C2=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1C3=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2;_LL21: {struct Cyc_List_List*es=_tmp1C2;struct Cyc_Absyn_Switch_clause*dest=_tmp1C3;
# 1513
struct _tuple16 _tmp1FD=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple16 _stmttmp1B=_tmp1FD;struct _tuple16 _tmp1FE=_stmttmp1B;void*_tmp200;struct Cyc_List_List*_tmp1FF;_LL43: _tmp1FF=_tmp1FE.f1;_tmp200=_tmp1FE.f2;_LL44: {struct Cyc_List_List*ts=_tmp1FF;void*a=_tmp200;
# 1515
{struct Cyc_List_List*_tmp201=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(dest->pat_vars))->v;struct Cyc_List_List*x=_tmp201;for(0;x != 0;(x=x->tl,ts=((struct Cyc_List_List*)_check_null(ts))->tl)){
struct _tuple19*_tmp202=(struct _tuple19*)x->hd;struct _tuple19*_stmttmp1C=_tmp202;struct _tuple19*_tmp203=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Vardecl**_tmp204;_LL46: _tmp204=_tmp203->f1;_tmp205=_tmp203->f2;_LL47: {struct Cyc_Absyn_Vardecl**vd=_tmp204;struct Cyc_Absyn_Exp*ve=_tmp205;
if(vd != 0)
a=({struct Cyc_Vcgen_Env*_tmp46F=env;void*_tmp46E=a;void*_tmp46D=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->tag=1U,_tmp206->f1=*vd;_tmp206;});Cyc_Vcgen_do_assign(_tmp46F,_tmp46E,_tmp46D,(void*)((struct Cyc_List_List*)_check_null(ts))->hd);});}}}
# 1520
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_Vcgen_false_assn;}}}case 12U: _LL22: _tmp1C0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1C1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2;_LL23: {struct Cyc_Absyn_Decl*d=_tmp1C0;struct Cyc_Absyn_Stmt*s1=_tmp1C1;
# 1523
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13U: _LL24: _tmp1BF=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2;_LL25: {struct Cyc_Absyn_Stmt*s=_tmp1BF;
# 1531
return Cyc_Vcgen_vcgen_stmt_nodebug(env,s,(void*)& Cyc_Vcgen_true_assn);}case 8U: _LL26: _LL27:
 return(void*)& Cyc_Vcgen_false_assn;default: _LL28: _tmp1BD=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1BE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2;_LL29: {struct Cyc_Absyn_Stmt*s=_tmp1BD;struct Cyc_List_List*scs=_tmp1BE;
# 1535
void*_tmp207=(env->shared)->try_assn;void*old_try_assn=_tmp207;
# 1537
(env->shared)->try_assn=(void*)& Cyc_Vcgen_false_assn;
# 1540
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 1542
void*_tmp208=(env->shared)->try_assn;void*new_try_assn=_tmp208;
(env->shared)->try_assn=old_try_assn;
# 1546
return({void*_tmp470=ain;Cyc_Vcgen_or(_tmp470,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}}_LL5:;}}
# 1550
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 1554
void*res=(void*)& Cyc_Vcgen_false_assn;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp209=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmp209;
void*_tmp20A=ain;void*a=_tmp20A;
{struct Cyc_List_List*_tmp20B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sc->pat_vars))->v;struct Cyc_List_List*pvs=_tmp20B;for(0;pvs != 0;pvs=pvs->tl){
struct _tuple19*_tmp20C=(struct _tuple19*)pvs->hd;struct _tuple19*_stmttmp1D=_tmp20C;struct _tuple19*_tmp20D=_stmttmp1D;struct Cyc_Absyn_Exp*_tmp20F;struct Cyc_Absyn_Vardecl**_tmp20E;_LL1: _tmp20E=_tmp20D->f1;_tmp20F=_tmp20D->f2;_LL2: {struct Cyc_Absyn_Vardecl**vdopt=_tmp20E;struct Cyc_Absyn_Exp*eopt=_tmp20F;
if(vdopt != 0){
if(eopt != 0)
a=({struct Cyc_Vcgen_Env*_tmp473=env;void*_tmp472=a;void*_tmp471=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->tag=1U,_tmp210->f1=*vdopt;_tmp210;});Cyc_Vcgen_do_assign(_tmp473,_tmp472,_tmp471,(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a)).f1);});else{
# 1564
a=({struct Cyc_Vcgen_Env*_tmp476=env;void*_tmp475=a;void*_tmp474=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->tag=1U,_tmp211->f1=*vdopt;_tmp211;});Cyc_Vcgen_do_assign(_tmp476,_tmp475,_tmp474,Cyc_Vcgen_fresh_var());});}}}}}
# 1567
if(sc->where_clause != 0)
a=(Cyc_Vcgen_vcgen_test(env,(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause),a)).f1;
res=({void*_tmp477=res;Cyc_Vcgen_or(_tmp477,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 1571
return res;}
# 1574
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 1577
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp212=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->tag=1U,_tmp217->f1=vd;_tmp217;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*tv=_tmp212;
a=({struct Cyc_Vcgen_Env*_tmp47A=env;void*_tmp479=a;void*_tmp478=(void*)tv;Cyc_Vcgen_do_assign(_tmp47A,_tmp479,_tmp478,Cyc_Vcgen_fresh_var());});
if(vd->initializer != 0){
struct _tuple11 _tmp213=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),a);struct _tuple11 _stmttmp1E=_tmp213;struct _tuple11 _tmp214=_stmttmp1E;void*_tmp216;void*_tmp215;_LL1: _tmp215=_tmp214.f1;_tmp216=_tmp214.f2;_LL2: {void*t=_tmp215;void*a1=_tmp216;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 1583
return a;}
# 1586
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_tmp218=d->r;void*_stmttmp1F=_tmp218;void*_tmp219=_stmttmp1F;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Vardecl*_tmp21A;struct Cyc_List_List*_tmp21C;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Core_Opt*_tmp21E;struct Cyc_Absyn_Pat*_tmp21D;struct Cyc_Absyn_Vardecl*_tmp220;switch(*((int*)_tmp219)){case 0U: _LL1: _tmp220=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp219)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp220;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2U: _LL3: _tmp21D=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp219)->f1;_tmp21E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp219)->f2;_tmp21F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp219)->f3;_LL4: {struct Cyc_Absyn_Pat*p=_tmp21D;struct Cyc_Core_Opt*vds=_tmp21E;struct Cyc_Absyn_Exp*e=_tmp21F;
# 1590
struct _tuple11 _tmp221=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple11 _stmttmp20=_tmp221;struct _tuple11 _tmp222=_stmttmp20;void*_tmp223;_LLC: _tmp223=_tmp222.f2;_LLD: {void*a=_tmp223;
{struct Cyc_List_List*_tmp224=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds))->v;struct Cyc_List_List*vds=_tmp224;for(0;vds != 0;vds=vds->tl){
struct _tuple19 _tmp225=*((struct _tuple19*)vds->hd);struct _tuple19 _stmttmp21=_tmp225;struct _tuple19 _tmp226=_stmttmp21;struct Cyc_Absyn_Exp*_tmp228;struct Cyc_Absyn_Vardecl**_tmp227;_LLF: _tmp227=_tmp226.f1;_tmp228=_tmp226.f2;_LL10: {struct Cyc_Absyn_Vardecl**vdopt=_tmp227;struct Cyc_Absyn_Exp*eopt=_tmp228;
if(vdopt != 0){
if(eopt == 0)a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);else{
# 1596
struct _tuple11 _tmp229=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple11 _stmttmp22=_tmp229;struct _tuple11 _tmp22A=_stmttmp22;void*_tmp22C;void*_tmp22B;_LL12: _tmp22B=_tmp22A.f1;_tmp22C=_tmp22A.f2;_LL13: {void*t=_tmp22B;void*a1=_tmp22C;
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp22D=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->tag=1U,_tmp22E->f1=*vdopt;_tmp22E;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*tv=_tmp22D;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}}}}}
# 1602
return a;}}case 3U: _LL5: _tmp21C=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp219)->f1;_LL6: {struct Cyc_List_List*vds=_tmp21C;
# 1604
for(0;vds != 0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4U: _LL7: _tmp21A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp219)->f2;_tmp21B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp219)->f3;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp21A;struct Cyc_Absyn_Exp*eopt=_tmp21B;
# 1608
if(eopt != 0){
struct _tuple11 _tmp22F=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple11 _stmttmp23=_tmp22F;struct _tuple11 _tmp230=_stmttmp23;void*_tmp232;void*_tmp231;_LL15: _tmp231=_tmp230.f1;_tmp232=_tmp230.f2;_LL16: {void*t=_tmp231;void*a1=_tmp232;
a=a1;}}
# 1613
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default: _LL9: _LLA:
 return a;}_LL0:;}
# 1621
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp233=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*t1=_tmp233;
void*_tmp234=Cyc_Tcutil_compress((void*)_check_null(e2->topt));void*t2=_tmp234;
struct _tuple11 _tmp235=({struct _tuple11 _tmp3BC;_tmp3BC.f1=t1,_tmp3BC.f2=t2;_tmp3BC;});struct _tuple11 _stmttmp24=_tmp235;struct _tuple11 _tmp236=_stmttmp24;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f1)->f1)->f1 == Cyc_Absyn_Unsigned){_LL1: _LL2:
 return 1;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3: _LL4:
 return 1;}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236.f1)->tag == 3U)goto _LL7;else{goto _LL7;}}}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236.f1)->tag == 3U)goto _LL7;else{goto _LL7;}}}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236.f2)->tag == 3U){_LL5: _LL6:
 return 1;}else{goto _LL7;}}else{_LL7: _LL8:
 return 0;}}}_LL0:;}
# 1632
static struct _tuple11 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp237=e->r;void*_stmttmp25=_tmp237;void*_tmp238=_stmttmp25;struct Cyc_Absyn_Exp*_tmp23B;struct Cyc_Absyn_Exp*_tmp23A;enum Cyc_Absyn_Primop _tmp239;struct Cyc_Absyn_Exp*_tmp23C;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_Exp*_tmp240;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Exp*_tmp241;struct Cyc_Absyn_Exp*_tmp245;struct Cyc_Absyn_Exp*_tmp244;struct Cyc_Absyn_Exp*_tmp243;switch(*((int*)_tmp238)){case 6U: _LL1: _tmp243=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp244=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_tmp245=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp238)->f3;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp243;struct Cyc_Absyn_Exp*e2=_tmp244;struct Cyc_Absyn_Exp*e3=_tmp245;
# 1635
struct _tuple11 _tmp246=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp26=_tmp246;struct _tuple11 _tmp247=_stmttmp26;void*_tmp249;void*_tmp248;_LL10: _tmp248=_tmp247.f1;_tmp249=_tmp247.f2;_LL11: {void*a1t=_tmp248;void*a1f=_tmp249;
struct _tuple11 _tmp24A=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple11 _stmttmp27=_tmp24A;struct _tuple11 _tmp24B=_stmttmp27;void*_tmp24D;void*_tmp24C;_LL13: _tmp24C=_tmp24B.f1;_tmp24D=_tmp24B.f2;_LL14: {void*a2t=_tmp24C;void*a2f=_tmp24D;
struct _tuple11 _tmp24E=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct _tuple11 _stmttmp28=_tmp24E;struct _tuple11 _tmp24F=_stmttmp28;void*_tmp251;void*_tmp250;_LL16: _tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;_LL17: {void*a3t=_tmp250;void*a3f=_tmp251;
return({struct _tuple11 _tmp3BD;({void*_tmp47C=Cyc_Vcgen_or(a2t,a3t);_tmp3BD.f1=_tmp47C;}),({void*_tmp47B=Cyc_Vcgen_or(a2f,a3f);_tmp3BD.f2=_tmp47B;});_tmp3BD;});}}}}case 7U: _LL3: _tmp241=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp242=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp241;struct Cyc_Absyn_Exp*e2=_tmp242;
# 1640
struct _tuple11 _tmp252=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp29=_tmp252;struct _tuple11 _tmp253=_stmttmp29;void*_tmp255;void*_tmp254;_LL19: _tmp254=_tmp253.f1;_tmp255=_tmp253.f2;_LL1A: {void*a1t=_tmp254;void*a1f=_tmp255;
struct _tuple11 _tmp256=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple11 _stmttmp2A=_tmp256;struct _tuple11 _tmp257=_stmttmp2A;void*_tmp259;void*_tmp258;_LL1C: _tmp258=_tmp257.f1;_tmp259=_tmp257.f2;_LL1D: {void*a2t=_tmp258;void*a2f=_tmp259;
return({struct _tuple11 _tmp3BE;_tmp3BE.f1=a2t,({void*_tmp47D=Cyc_Vcgen_or(a1f,a2f);_tmp3BE.f2=_tmp47D;});_tmp3BE;});}}}case 8U: _LL5: _tmp23F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp240=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp23F;struct Cyc_Absyn_Exp*e2=_tmp240;
# 1644
struct _tuple11 _tmp25A=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp2B=_tmp25A;struct _tuple11 _tmp25B=_stmttmp2B;void*_tmp25D;void*_tmp25C;_LL1F: _tmp25C=_tmp25B.f1;_tmp25D=_tmp25B.f2;_LL20: {void*a1t=_tmp25C;void*a1f=_tmp25D;
struct _tuple11 _tmp25E=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct _tuple11 _stmttmp2C=_tmp25E;struct _tuple11 _tmp25F=_stmttmp2C;void*_tmp261;void*_tmp260;_LL22: _tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;_LL23: {void*a2t=_tmp260;void*a2f=_tmp261;
return({struct _tuple11 _tmp3BF;({void*_tmp47E=Cyc_Vcgen_or(a1t,a2t);_tmp3BF.f1=_tmp47E;}),_tmp3BF.f2=a2f;_tmp3BF;});}}}case 9U: _LL7: _tmp23D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp23E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp23D;struct Cyc_Absyn_Exp*e2=_tmp23E;
# 1648
struct _tuple11 _tmp262=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp2D=_tmp262;struct _tuple11 _tmp263=_stmttmp2D;void*_tmp264;_LL25: _tmp264=_tmp263.f2;_LL26: {void*ain=_tmp264;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f2 != 0){_LL9: _tmp23C=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f2)->hd;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp23C;
# 1651
struct _tuple11 _tmp265=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp2E=_tmp265;struct _tuple11 _tmp266=_stmttmp2E;void*_tmp268;void*_tmp267;_LL28: _tmp267=_tmp266.f1;_tmp268=_tmp266.f2;_LL29: {void*at=_tmp267;void*af=_tmp268;
return({struct _tuple11 _tmp3C0;_tmp3C0.f1=af,_tmp3C0.f2=at;_tmp3C0;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f2)->tl)->tl == 0){_LLB: _tmp239=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp23A=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f2)->hd;_tmp23B=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp238)->f2)->tl)->hd;_LLC: {enum Cyc_Absyn_Primop p=_tmp239;struct Cyc_Absyn_Exp*e1=_tmp23A;struct Cyc_Absyn_Exp*e2=_tmp23B;
# 1654
struct _tuple11 _tmp269=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp2F=_tmp269;struct _tuple11 _tmp26A=_stmttmp2F;void*_tmp26C;void*_tmp26B;_LL2B: _tmp26B=_tmp26A.f1;_tmp26C=_tmp26A.f2;_LL2C: {void*t1=_tmp26B;void*a1=_tmp26C;
struct _tuple11 _tmp26D=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple11 _stmttmp30=_tmp26D;struct _tuple11 _tmp26E=_stmttmp30;void*_tmp270;void*_tmp26F;_LL2E: _tmp26F=_tmp26E.f1;_tmp270=_tmp26E.f2;_LL2F: {void*t2=_tmp26F;void*a2=_tmp270;
void*_tmp271=a2;void*at=_tmp271;
void*_tmp272=a2;void*af=_tmp272;
{enum Cyc_Absyn_Primop _tmp273=p;switch(_tmp273){case Cyc_Absyn_Eq: _LL31: _LL32:
 at=({void*_tmp47F=at;Cyc_Vcgen_and(_tmp47F,Cyc_Vcgen_eq(t1,t2));});af=({void*_tmp480=af;Cyc_Vcgen_and(_tmp480,Cyc_Vcgen_neq(t1,t2));});goto _LL30;case Cyc_Absyn_Neq: _LL33: _LL34:
 at=({void*_tmp481=at;Cyc_Vcgen_and(_tmp481,Cyc_Vcgen_neq(t1,t2));});af=({void*_tmp482=af;Cyc_Vcgen_and(_tmp482,Cyc_Vcgen_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Lt: _LL35: _LL36:
# 1662
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp483=at;Cyc_Vcgen_and(_tmp483,Cyc_Vcgen_ult(t1,t2));});af=({void*_tmp484=af;Cyc_Vcgen_and(_tmp484,Cyc_Vcgen_ulte(t2,t1));});}else{
# 1665
at=({void*_tmp485=at;Cyc_Vcgen_and(_tmp485,Cyc_Vcgen_slt(t1,t2));});af=({void*_tmp486=af;Cyc_Vcgen_and(_tmp486,Cyc_Vcgen_slte(t2,t1));});}
# 1667
goto _LL30;case Cyc_Absyn_Lte: _LL37: _LL38:
# 1669
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp487=at;Cyc_Vcgen_and(_tmp487,Cyc_Vcgen_ulte(t1,t2));});af=({void*_tmp488=af;Cyc_Vcgen_and(_tmp488,Cyc_Vcgen_ult(t2,t1));});}else{
# 1672
at=({void*_tmp489=at;Cyc_Vcgen_and(_tmp489,Cyc_Vcgen_slte(t1,t2));});af=({void*_tmp48A=af;Cyc_Vcgen_and(_tmp48A,Cyc_Vcgen_slt(t2,t1));});}
# 1674
goto _LL30;case Cyc_Absyn_Gt: _LL39: _LL3A:
# 1676
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp48B=at;Cyc_Vcgen_and(_tmp48B,Cyc_Vcgen_ult(t2,t1));});af=({void*_tmp48C=af;Cyc_Vcgen_and(_tmp48C,Cyc_Vcgen_ulte(t1,t2));});}else{
# 1679
at=({void*_tmp48D=at;Cyc_Vcgen_and(_tmp48D,Cyc_Vcgen_slt(t2,t1));});af=({void*_tmp48E=af;Cyc_Vcgen_and(_tmp48E,Cyc_Vcgen_slte(t1,t2));});}
# 1681
goto _LL30;case Cyc_Absyn_Gte: _LL3B: _LL3C:
# 1683
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp48F=at;Cyc_Vcgen_and(_tmp48F,Cyc_Vcgen_ulte(t2,t1));});af=({void*_tmp490=af;Cyc_Vcgen_and(_tmp490,Cyc_Vcgen_ult(t1,t2));});}else{
# 1686
at=({void*_tmp491=at;Cyc_Vcgen_and(_tmp491,Cyc_Vcgen_slte(t2,t1));});af=({void*_tmp492=af;Cyc_Vcgen_and(_tmp492,Cyc_Vcgen_slt(t1,t2));});}
# 1688
goto _LL30;default: _LL3D: _LL3E:
 goto _LL30;}_LL30:;}
# 1691
return({struct _tuple11 _tmp3C1;_tmp3C1.f1=at,_tmp3C1.f2=af;_tmp3C1;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: _LLE: {
# 1693
struct _tuple11 _tmp274=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp31=_tmp274;struct _tuple11 _tmp275=_stmttmp31;void*_tmp277;void*_tmp276;_LL40: _tmp276=_tmp275.f1;_tmp277=_tmp275.f2;_LL41: {void*t=_tmp276;void*aout=_tmp277;
return({struct _tuple11 _tmp3C2;({void*_tmp498=({void*_tmp497=aout;Cyc_Vcgen_and(_tmp497,({void*_tmp496=t;Cyc_Vcgen_neq(_tmp496,Cyc_Vcgen_zero());}));});_tmp3C2.f1=_tmp498;}),({void*_tmp495=({void*_tmp494=aout;Cyc_Vcgen_and(_tmp494,({void*_tmp493=t;Cyc_Vcgen_eq(_tmp493,Cyc_Vcgen_zero());}));});_tmp3C2.f2=_tmp495;});_tmp3C2;});}}}_LL0:;}
# 1698
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(0;es != 0;es=es->tl){
a=(Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a)).f2;}
return a;}
# 1704
static struct _tuple18 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(0;es != 0;es=es->tl){
struct _tuple11 _tmp278=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);struct _tuple11 _stmttmp32=_tmp278;struct _tuple11 _tmp279=_stmttmp32;void*_tmp27B;void*_tmp27A;_LL1: _tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;_LL2: {void*t=_tmp27A;void*aout=_tmp27B;
a=aout;
if(valid && t != 0)
res=({struct Cyc_List_List*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->hd=t,_tmp27C->tl=res;_tmp27C;});else{
# 1713
valid=0;}}}
# 1716
return({struct _tuple18 _tmp3C3;valid?({struct Cyc_List_List**_tmp49A=({struct Cyc_List_List**_tmp27D=_cycalloc(sizeof(*_tmp27D));({struct Cyc_List_List*_tmp499=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);*_tmp27D=_tmp499;});_tmp27D;});_tmp3C3.f1=_tmp49A;}):(_tmp3C3.f1=0),_tmp3C3.f2=a;_tmp3C3;});}
# 1719
static struct _tuple11 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple11 _tmp27E=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp33=_tmp27E;struct _tuple11 _tmp27F=_stmttmp33;void*_tmp281;void*_tmp280;_LL1: _tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;_LL2: {void*topt=_tmp280;void*a=_tmp281;
return({struct _tuple11 _tmp3C4;topt == 0?({void*_tmp49B=Cyc_Vcgen_fresh_var();_tmp3C4.f1=_tmp49B;}):(_tmp3C4.f1=topt),_tmp3C4.f2=a;_tmp3C4;});}}
# 1724
static struct _tuple16 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(0;es != 0;es=es->tl){
struct _tuple11 _tmp282=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);struct _tuple11 _stmttmp34=_tmp282;struct _tuple11 _tmp283=_stmttmp34;void*_tmp285;void*_tmp284;_LL1: _tmp284=_tmp283.f1;_tmp285=_tmp283.f2;_LL2: {void*t=_tmp284;void*a=_tmp285;
ain=a;
res=({struct Cyc_List_List*_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->hd=t,_tmp286->tl=res;_tmp286;});}}
# 1731
return({struct _tuple16 _tmp3C5;_tmp3C5.f1=res,_tmp3C5.f2=ain;_tmp3C5;});}
# 1734
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_tmp287=b;struct Cyc_Absyn_Vardecl*_tmp288;struct Cyc_Absyn_Vardecl*_tmp289;struct Cyc_Absyn_Vardecl*_tmp28A;struct Cyc_Absyn_Fndecl*_tmp28B;struct Cyc_Absyn_Vardecl*_tmp28C;switch(*((int*)_tmp287)){case 1U: _LL1: _tmp28C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp287)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp28C;
return vd;}case 2U: _LL3: _tmp28B=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp287)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp28B;
return(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);}case 0U: _LL5: _LL6:
({void*_tmp28D=0U;({struct _fat_ptr _tmp49C=({const char*_tmp28E="vcgen bind2vardecl";_tag_fat(_tmp28E,sizeof(char),19U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp49C,_tag_fat(_tmp28D,sizeof(void*),0U));});});case 3U: _LL7: _tmp28A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp287)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp28A;
return vd;}case 4U: _LL9: _tmp289=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp287)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp289;
return vd;}default: _LLB: _tmp288=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp287)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp288;
return vd;}}_LL0:;}
# 1745
static struct _tuple11 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp28F=e->r;void*_stmttmp35=_tmp28F;void*_tmp290=_stmttmp35;void*_tmp291;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp290)->tag == 1U){_LL1: _tmp291=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp290)->f1;_LL2: {void*b=_tmp291;
# 1748
return({struct _tuple11 _tmp3C6;({void*_tmp49E=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp49D=Cyc_Vcgen_bind2vardecl(b);_tmp292->f1=_tmp49D;});_tmp292;});_tmp3C6.f1=_tmp49E;}),_tmp3C6.f2=ain;_tmp3C6;});}}else{_LL3: _LL4:
 return({struct _tuple11 _tmp3C7;_tmp3C7.f1=0,({void*_tmp49F=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp3C7.f2=_tmp49F;});_tmp3C7;});}_LL0:;}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Vardecl*f4;};
# 1755
static struct _tuple20 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple20 _tmp3C8;_tmp3C8.f1=fi->args,_tmp3C8.f2=fi->requires_clause,_tmp3C8.f3=fi->ensures_clause,_tmp3C8.f4=fi->return_value;_tmp3C8;});}
# 1759
static struct _tuple20 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt != 0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_tmp293=t;struct Cyc_Absyn_FnInfo*_tmp294;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp293)->tag == 5U){_LL1: _tmp294=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp293)->f1;_LL2: {struct Cyc_Absyn_FnInfo*fi=_tmp294;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1767
return({struct _tuple20 _tmp3C9;_tmp3C9.f1=0,_tmp3C9.f2=0,_tmp3C9.f3=0,_tmp3C9.f4=0;_tmp3C9;});}
# 1770
static void Cyc_Vcgen_check_requires(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*args,void*a,struct Cyc_Absyn_Exp*reqopt){
# 1773
if(reqopt != 0){
struct Cyc_Absyn_Exp*req=reqopt;
# 1777
for(0;args != 0 && inputs != 0;(args=args->tl,inputs=inputs->tl)){
struct _tuple8*_tmp295=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp36=_tmp295;struct _tuple8*_tmp296=_stmttmp36;void*_tmp299;struct Cyc_Absyn_Tqual _tmp298;struct _fat_ptr*_tmp297;_LL1: _tmp297=_tmp296->f1;_tmp298=_tmp296->f2;_tmp299=_tmp296->f3;_LL2: {struct _fat_ptr*vopt=_tmp297;struct Cyc_Absyn_Tqual tq=_tmp298;void*t=_tmp299;
if(vopt != 0){
# 1781
struct _tuple0*qv=({struct _tuple0*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->f1=Cyc_Absyn_Loc_n,_tmp29D->f2=vopt;_tmp29D;});
struct Cyc_Absyn_Vardecl*_tmp29A=({struct Cyc_Absyn_Vardecl*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->sc=Cyc_Absyn_Public,_tmp29C->name=qv,_tmp29C->varloc=0U,_tmp29C->tq=tq,_tmp29C->type=t,_tmp29C->initializer=0,_tmp29C->rgn=0,_tmp29C->attributes=0,_tmp29C->escapes=0,_tmp29C->is_proto=0;_tmp29C;});struct Cyc_Absyn_Vardecl*vd=_tmp29A;
# 1787
a=({void*_tmp4A1=a;Cyc_Vcgen_and(_tmp4A1,({void*_tmp4A0=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->tag=1U,_tmp29B->f1=vd;_tmp29B;});Cyc_Vcgen_eq(_tmp4A0,(void*)inputs->hd);}));});}}}{
# 1791
struct _tuple11 _tmp29E=Cyc_Vcgen_vcgen_test(env,reqopt,(void*)& Cyc_Vcgen_true_assn);struct _tuple11 _stmttmp37=_tmp29E;struct _tuple11 _tmp29F=_stmttmp37;void*_tmp2A0;_LL4: _tmp2A0=_tmp29F.f1;_LL5: {void*at=_tmp2A0;
if(!Cyc_Vcgen_implies(& a,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp2A3=({struct Cyc_String_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=0U,({
struct _fat_ptr _tmp4A2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(at)));_tmp3CB.f1=_tmp4A2;});_tmp3CB;});struct Cyc_String_pa_PrintArg_struct _tmp2A4=({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0U,({struct _fat_ptr _tmp4A3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(a)));_tmp3CA.f1=_tmp4A3;});_tmp3CA;});void*_tmp2A1[2U];_tmp2A1[0]=& _tmp2A3,_tmp2A1[1]=& _tmp2A4;({unsigned _tmp4A5=req->loc;struct _fat_ptr _tmp4A4=({const char*_tmp2A2="cannot prove @requires %s from %s";_tag_fat(_tmp2A2,sizeof(char),34U);});Cyc_Warn_err(_tmp4A5,_tmp4A4,_tag_fat(_tmp2A1,sizeof(void*),2U));});});}}}}
# 1798
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1802
static struct _tuple11 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e))return({struct _tuple11 _tmp3CC;({void*_tmp4A6=Cyc_Vcgen_cnst(e);_tmp3CC.f1=_tmp4A6;}),_tmp3CC.f2=ain;_tmp3CC;});{
void*_tmp2A5=e->r;void*_stmttmp38=_tmp2A5;void*_tmp2A6=_stmttmp38;struct Cyc_Absyn_Exp*_tmp2A7;struct _fat_ptr _tmp2A8;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Stmt*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_MallocInfo _tmp2AD;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_Absyn_Vardecl*_tmp2AF;struct Cyc_List_List*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;enum Cyc_Absyn_Coercion _tmp2C4;struct Cyc_Absyn_Exp*_tmp2C3;void*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C7;struct Cyc_List_List*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;enum Cyc_Absyn_Incrementor _tmp2D4;struct Cyc_Absyn_Exp*_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Core_Opt*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_List_List*_tmp2D9;enum Cyc_Absyn_Primop _tmp2D8;void*_tmp2DA;union Cyc_Absyn_Cnst _tmp2DB;switch(*((int*)_tmp2A6)){case 0U: _LL1: _tmp2DB=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp2DB;
# 1807
return({struct _tuple11 _tmp3CD;({void*_tmp4A7=(void*)({struct Cyc_Vcgen_Const_Vcgen_Term_struct*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));_tmp2DC->tag=0U,_tmp2DC->f1=e;_tmp2DC;});_tmp3CD.f1=_tmp4A7;}),_tmp3CD.f2=ain;_tmp3CD;});}case 1U: _LL3: _tmp2DA=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL4: {void*b=_tmp2DA;
return({struct _tuple11 _tmp3CE;({void*_tmp4A9=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp4A8=Cyc_Vcgen_bind2vardecl(b);_tmp2DD->f1=_tmp4A8;});_tmp2DD;});_tmp3CE.f1=_tmp4A9;}),_tmp3CE.f2=ain;_tmp3CE;});}case 3U: _LL5: _tmp2D8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2D9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL6: {enum Cyc_Absyn_Primop p=_tmp2D8;struct Cyc_List_List*es=_tmp2D9;
# 1810
struct _tuple18 _tmp2DE=Cyc_Vcgen_vcgen_rexps(env,es,ain);struct _tuple18 _stmttmp39=_tmp2DE;struct _tuple18 _tmp2DF=_stmttmp39;void*_tmp2E1;struct Cyc_List_List**_tmp2E0;_LL58: _tmp2E0=_tmp2DF.f1;_tmp2E1=_tmp2DF.f2;_LL59: {struct Cyc_List_List**ts=_tmp2E0;void*aout=_tmp2E1;
void*t=0;
if(ts != 0)t=Cyc_Vcgen_primop(p,*ts);
return({struct _tuple11 _tmp3CF;_tmp3CF.f1=t,_tmp3CF.f2=aout;_tmp3CF;});}}case 4U: _LL7: _tmp2D5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2D6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_tmp2D7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2A6)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp2D5;struct Cyc_Core_Opt*popt=_tmp2D6;struct Cyc_Absyn_Exp*e2=_tmp2D7;
# 1815
struct _tuple11 _tmp2E2=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple11 _stmttmp3A=_tmp2E2;struct _tuple11 _tmp2E3=_stmttmp3A;void*_tmp2E5;void*_tmp2E4;_LL5B: _tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;_LL5C: {void*lt=_tmp2E4;void*a1=_tmp2E5;
struct _tuple11 _tmp2E6=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple11 _stmttmp3B=_tmp2E6;struct _tuple11 _tmp2E7=_stmttmp3B;void*_tmp2E9;void*_tmp2E8;_LL5E: _tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;_LL5F: {void*t2=_tmp2E8;void*a2=_tmp2E9;
if(popt != 0){
void*_tmp2EA=Cyc_Vcgen_deref_lterm(env,lt,a2);void*t1=_tmp2EA;
if(t1 == 0 || t2 == 0)
t2=0;else{
# 1822
t2=({enum Cyc_Absyn_Primop _tmp4AA=(enum Cyc_Absyn_Primop)popt->v;Cyc_Vcgen_primop(_tmp4AA,({void*_tmp2EB[2U];_tmp2EB[0]=t1,_tmp2EB[1]=t2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2EB,sizeof(void*),2U));}));});}}{
# 1824
void*_tmp2EC=Cyc_Vcgen_do_assign(env,a2,lt,t2);void*a=_tmp2EC;
return({struct _tuple11 _tmp3D0;_tmp3D0.f1=lt,_tmp3D0.f2=a;_tmp3D0;});}}}}case 5U: _LL9: _tmp2D3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2D4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp2D3;enum Cyc_Absyn_Incrementor i=_tmp2D4;
# 1827
struct _tuple11 _tmp2ED=Cyc_Vcgen_vcgen_lexp(env,e,ain);struct _tuple11 _stmttmp3C=_tmp2ED;struct _tuple11 _tmp2EE=_stmttmp3C;void*_tmp2F0;void*_tmp2EF;_LL61: _tmp2EF=_tmp2EE.f1;_tmp2F0=_tmp2EE.f2;_LL62: {void*lt=_tmp2EF;void*a1=_tmp2F0;
void*_tmp2F1=Cyc_Vcgen_deref_lterm(env,lt,a1);void*t1=_tmp2F1;
void*res=0;
void*assn=0;
{enum Cyc_Absyn_Incrementor _tmp2F2=i;switch(_tmp2F2){case Cyc_Absyn_PreInc: _LL64: _LL65:
# 1833
 if(t1 != 0)assn=Cyc_Vcgen_primop(Cyc_Absyn_Plus,({void*_tmp2F3[2U];_tmp2F3[0]=t1,({void*_tmp4AB=Cyc_Vcgen_one();_tmp2F3[1]=_tmp4AB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2F3,sizeof(void*),2U));}));
res=assn;
goto _LL63;case Cyc_Absyn_PostInc: _LL66: _LL67:
# 1837
 res=t1;
if(t1 != 0)assn=Cyc_Vcgen_primop(Cyc_Absyn_Plus,({void*_tmp2F4[2U];_tmp2F4[0]=t1,({void*_tmp4AC=Cyc_Vcgen_one();_tmp2F4[1]=_tmp4AC;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2F4,sizeof(void*),2U));}));
goto _LL63;case Cyc_Absyn_PreDec: _LL68: _LL69:
# 1841
 if(t1 != 0)assn=Cyc_Vcgen_primop(Cyc_Absyn_Minus,({void*_tmp2F5[2U];_tmp2F5[0]=t1,({void*_tmp4AD=Cyc_Vcgen_one();_tmp2F5[1]=_tmp4AD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2F5,sizeof(void*),2U));}));
res=assn;
goto _LL63;case Cyc_Absyn_PostDec: _LL6A: _LL6B:
# 1845
 res=t1;
if(t1 != 0)assn=Cyc_Vcgen_primop(Cyc_Absyn_Minus,({void*_tmp2F6[2U];_tmp2F6[0]=t1,({void*_tmp4AE=Cyc_Vcgen_one();_tmp2F6[1]=_tmp4AE;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2F6,sizeof(void*),2U));}));
goto _LL63;default: _LL6C: _LL6D:
({void*_tmp2F7=0U;({struct _fat_ptr _tmp4AF=({const char*_tmp2F8="vcgen vcgen_rexp increment_e";_tag_fat(_tmp2F8,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4AF,_tag_fat(_tmp2F7,sizeof(void*),0U));});});}_LL63:;}{
# 1850
void*_tmp2F9=Cyc_Vcgen_do_assign(env,a1,lt,assn);void*a=_tmp2F9;
return({struct _tuple11 _tmp3D1;_tmp3D1.f1=res,_tmp3D1.f2=a;_tmp3D1;});}}}case 6U: _LLB: _tmp2D0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2D1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_tmp2D2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2A6)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp2D0;struct Cyc_Absyn_Exp*e2=_tmp2D1;struct Cyc_Absyn_Exp*e3=_tmp2D2;
# 1853
struct _tuple11 _tmp2FA=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp3D=_tmp2FA;struct _tuple11 _tmp2FB=_stmttmp3D;void*_tmp2FD;void*_tmp2FC;_LL6F: _tmp2FC=_tmp2FB.f1;_tmp2FD=_tmp2FB.f2;_LL70: {void*at=_tmp2FC;void*af=_tmp2FD;
struct _tuple11 _tmp2FE=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple11 _stmttmp3E=_tmp2FE;struct _tuple11 _tmp2FF=_stmttmp3E;void*_tmp301;void*_tmp300;_LL72: _tmp300=_tmp2FF.f1;_tmp301=_tmp2FF.f2;_LL73: {void*t1=_tmp300;void*at=_tmp301;
struct _tuple11 _tmp302=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);struct _tuple11 _stmttmp3F=_tmp302;struct _tuple11 _tmp303=_stmttmp3F;void*_tmp305;void*_tmp304;_LL75: _tmp304=_tmp303.f1;_tmp305=_tmp303.f2;_LL76: {void*t2=_tmp304;void*af=_tmp305;
if(t1 != 0 && t2 != 0){
void*_tmp306=Cyc_Vcgen_fresh_var();void*v=_tmp306;
at=({void*_tmp4B0=at;Cyc_Vcgen_and(_tmp4B0,Cyc_Vcgen_eq(v,t1));});
af=({void*_tmp4B1=af;Cyc_Vcgen_and(_tmp4B1,Cyc_Vcgen_eq(v,t2));});
return({struct _tuple11 _tmp3D2;_tmp3D2.f1=v,({void*_tmp4B2=Cyc_Vcgen_or(at,af);_tmp3D2.f2=_tmp4B2;});_tmp3D2;});}else{
# 1862
return({struct _tuple11 _tmp3D3;_tmp3D3.f1=0,({void*_tmp4B3=Cyc_Vcgen_or(at,af);_tmp3D3.f2=_tmp4B3;});_tmp3D3;});}}}}}case 7U: _LLD: _tmp2CE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2CF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp2CE;struct Cyc_Absyn_Exp*e2=_tmp2CF;
# 1864
struct _tuple11 _tmp307=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp40=_tmp307;struct _tuple11 _tmp308=_stmttmp40;void*_tmp30A;void*_tmp309;_LL78: _tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;_LL79: {void*at=_tmp309;void*af=_tmp30A;
struct _tuple11 _tmp30B=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple11 _stmttmp41=_tmp30B;struct _tuple11 _tmp30C=_stmttmp41;void*_tmp30E;void*_tmp30D;_LL7B: _tmp30D=_tmp30C.f1;_tmp30E=_tmp30C.f2;_LL7C: {void*t=_tmp30D;void*at=_tmp30E;
void*_tmp30F=Cyc_Vcgen_fresh_var();void*v=_tmp30F;
at=({void*_tmp4B4=at;Cyc_Vcgen_and(_tmp4B4,Cyc_Vcgen_eq(v,t));});
af=({void*_tmp4B6=af;Cyc_Vcgen_and(_tmp4B6,({void*_tmp4B5=v;Cyc_Vcgen_eq(_tmp4B5,Cyc_Vcgen_zero());}));});
return({struct _tuple11 _tmp3D4;_tmp3D4.f1=v,({void*_tmp4B7=Cyc_Vcgen_or(at,af);_tmp3D4.f2=_tmp4B7;});_tmp3D4;});}}}case 8U: _LLF: _tmp2CC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2CD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp2CC;struct Cyc_Absyn_Exp*e2=_tmp2CD;
# 1871
struct _tuple11 _tmp310=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp42=_tmp310;struct _tuple11 _tmp311=_stmttmp42;void*_tmp313;void*_tmp312;_LL7E: _tmp312=_tmp311.f1;_tmp313=_tmp311.f2;_LL7F: {void*at=_tmp312;void*af=_tmp313;
struct _tuple11 _tmp314=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);struct _tuple11 _stmttmp43=_tmp314;struct _tuple11 _tmp315=_stmttmp43;void*_tmp317;void*_tmp316;_LL81: _tmp316=_tmp315.f1;_tmp317=_tmp315.f2;_LL82: {void*t=_tmp316;void*af=_tmp317;
void*_tmp318=Cyc_Vcgen_fresh_var();void*v=_tmp318;
at=({void*_tmp4B9=at;Cyc_Vcgen_and(_tmp4B9,({void*_tmp4B8=v;Cyc_Vcgen_eq(_tmp4B8,Cyc_Vcgen_one());}));});
af=({void*_tmp4BA=af;Cyc_Vcgen_and(_tmp4BA,Cyc_Vcgen_eq(v,t));});
return({struct _tuple11 _tmp3D5;_tmp3D5.f1=v,({void*_tmp4BB=Cyc_Vcgen_or(at,af);_tmp3D5.f2=_tmp4BB;});_tmp3D5;});}}}case 9U: _LL11: _tmp2CA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2CB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp2CA;struct Cyc_Absyn_Exp*e2=_tmp2CB;
# 1878
struct _tuple11 _tmp319=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp44=_tmp319;struct _tuple11 _tmp31A=_stmttmp44;void*_tmp31B;_LL84: _tmp31B=_tmp31A.f2;_LL85: {void*a=_tmp31B;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10U: _LL13: _tmp2C8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2C9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_tmp2C8;struct Cyc_List_List*es=_tmp2C9;
# 1881
struct _tuple16 _tmp31C=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple16 _stmttmp45=_tmp31C;struct _tuple16 _tmp31D=_stmttmp45;void*_tmp31F;struct Cyc_List_List*_tmp31E;_LL87: _tmp31E=_tmp31D.f1;_tmp31F=_tmp31D.f2;_LL88: {struct Cyc_List_List*ts=_tmp31E;void*a=_tmp31F;
struct _tuple11 _tmp320=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple11 _stmttmp46=_tmp320;struct _tuple11 _tmp321=_stmttmp46;void*_tmp322;_LL8A: _tmp322=_tmp321.f2;_LL8B: {void*a2=_tmp322;
# 1884
struct _tuple20 _tmp323=Cyc_Vcgen_get_requires_and_ensures(e->topt);struct _tuple20 _stmttmp47=_tmp323;struct _tuple20 _tmp324=_stmttmp47;struct Cyc_Absyn_Vardecl*_tmp328;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_List_List*_tmp325;_LL8D: _tmp325=_tmp324.f1;_tmp326=_tmp324.f2;_tmp327=_tmp324.f3;_tmp328=_tmp324.f4;_LL8E: {struct Cyc_List_List*args=_tmp325;struct Cyc_Absyn_Exp*reqopt=_tmp326;struct Cyc_Absyn_Exp*ensopt=_tmp327;struct Cyc_Absyn_Vardecl*ret_value=_tmp328;
Cyc_Vcgen_check_requires(env,ts,args,a2,reqopt);
# 1888
a2=Cyc_Vcgen_kill(a2);
Cyc_Vcgen_update_try_assn(env,a2);
# 1891
if(Cyc_Tcutil_is_noreturn_fn_type((void*)_check_null(e->topt)))a2=(void*)& Cyc_Vcgen_false_assn;
if(ensopt == 0)
return({struct _tuple11 _tmp3D6;_tmp3D6.f1=0,_tmp3D6.f2=a2;_tmp3D6;});{
struct Cyc_Absyn_Exp*ensures=ensopt;
# 1896
struct _tuple11 _tmp329=Cyc_Vcgen_vcgen_test(env,ensures,a2);struct _tuple11 _stmttmp48=_tmp329;struct _tuple11 _tmp32A=_stmttmp48;void*_tmp32B;_LL90: _tmp32B=_tmp32A.f1;_LL91: {void*at=_tmp32B;
if(ret_value != 0){
void*_tmp32C=Cyc_Vcgen_fresh_var();void*v=_tmp32C;
# 1900
at=Cyc_Vcgen_subst(ret_value,v,at);
return({struct _tuple11 _tmp3D7;_tmp3D7.f1=v,_tmp3D7.f2=at;_tmp3D7;});}
# 1903
return({struct _tuple11 _tmp3D8;_tmp3D8.f1=0,_tmp3D8.f2=at;_tmp3D8;});}}}}}}case 11U: _LL15: _tmp2C7=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp2C7;
# 1905
struct _tuple11 _tmp32D=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp49=_tmp32D;struct _tuple11 _tmp32E=_stmttmp49;void*_tmp32F;_LL93: _tmp32F=_tmp32E.f2;_LL94: {void*a=_tmp32F;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple11 _tmp3D9;_tmp3D9.f1=0,_tmp3D9.f2=(void*)& Cyc_Vcgen_false_assn;_tmp3D9;});}}case 12U: _LL17: _tmp2C6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp2C6;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13U: _LL19: _tmp2C5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp2C5;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14U: _LL1B: _tmp2C2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2C3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_tmp2C4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2A6)->f4;_LL1C: {void*tp=_tmp2C2;struct Cyc_Absyn_Exp*e=_tmp2C3;enum Cyc_Absyn_Coercion c=_tmp2C4;
# 1913
struct _tuple11 _tmp330=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp4A=_tmp330;struct _tuple11 _tmp331=_stmttmp4A;void*_tmp333;void*_tmp332;_LL96: _tmp332=_tmp331.f1;_tmp333=_tmp331.f2;_LL97: {void*t=_tmp332;void*a=_tmp333;
if((int)c == (int)1U)
return({struct _tuple11 _tmp3DA;t == 0?_tmp3DA.f1=0:({void*_tmp4BC=Cyc_Vcgen_cast(tp,t);_tmp3DA.f1=_tmp4BC;}),_tmp3DA.f2=a;_tmp3DA;});else{
# 1917
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple11 _tmp3DB;_tmp3DB.f1=0,_tmp3DB.f2=a;_tmp3DB;});}}}case 16U: _LL1D: _tmp2C0=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2C1=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL1E: {struct Cyc_Absyn_Exp*eopt=_tmp2C0;struct Cyc_Absyn_Exp*e=_tmp2C1;
# 1921
if(eopt != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain)).f2;
return({struct _tuple11 _tmp3DC;_tmp3DC.f1=0,({void*_tmp4BD=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp3DC.f2=_tmp4BD;});_tmp3DC;});}case 32U: _LL1F: _LL20:
# 1926
 goto _LL22;case 33U: _LL21: _LL22:
 goto _LL24;case 17U: _LL23: _LL24:
 goto _LL26;case 18U: _LL25: _LL26:
 goto _LL28;case 39U: _LL27: _LL28:
 goto _LL2A;case 19U: _LL29: _LL2A:
 return({struct _tuple11 _tmp3DD;_tmp3DD.f1=0,_tmp3DD.f2=ain;_tmp3DD;});case 20U: _LL2B: _tmp2BF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp2BF;
# 1934
_tmp2BE=e;goto _LL2E;}case 21U: _LL2D: _tmp2BE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp2BE;
_tmp2BD=e;goto _LL30;}case 22U: _LL2F: _tmp2BD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp2BD;
# 1937
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple11 _tmp3DE;_tmp3DE.f1=0,_tmp3DE.f2=ain;_tmp3DE;});}case 38U: _LL31: _tmp2BC=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp2BC;
# 1941
_tmp2BB=e;goto _LL34;}case 15U: _LL33: _tmp2BB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL34: {struct Cyc_Absyn_Exp*e=_tmp2BB;
# 1943
return({struct _tuple11 _tmp3DF;_tmp3DF.f1=0,({void*_tmp4BE=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp3DF.f2=_tmp4BE;});_tmp3DF;});}case 23U: _LL35: _tmp2B9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2BA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp2B9;struct Cyc_Absyn_Exp*e2=_tmp2BA;
# 1946
struct _tuple11 _tmp334=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp4B=_tmp334;struct _tuple11 _tmp335=_stmttmp4B;void*_tmp337;void*_tmp336;_LL99: _tmp336=_tmp335.f1;_tmp337=_tmp335.f2;_LL9A: {void*t1=_tmp336;void*a1=_tmp337;
struct _tuple11 _tmp338=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple11 _stmttmp4C=_tmp338;struct _tuple11 _tmp339=_stmttmp4C;void*_tmp33B;void*_tmp33A;_LL9C: _tmp33A=_tmp339.f1;_tmp33B=_tmp339.f2;_LL9D: {void*t2=_tmp33A;void*a2=_tmp33B;
{void*_tmp33C=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp4D=_tmp33C;void*_tmp33D=_stmttmp4D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp33D)->tag == 6U){_LL9F: _LLA0:
 goto _LL9E;}else{_LLA1: _LLA2:
# 1951
 if(t1 != 0){
Cyc_Vcgen_update_try_assn(env,ain);
a2=({void*_tmp4C0=a2;Cyc_Vcgen_and(_tmp4C0,({void*_tmp4BF=t2;Cyc_Vcgen_ult(_tmp4BF,Cyc_Vcgen_primop(Cyc_Absyn_Numelts,({void*_tmp33E[1U];_tmp33E[0]=t1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp33E,sizeof(void*),1U));})));}));});}}_LL9E:;}
# 1956
return({struct _tuple11 _tmp3E0;_tmp3E0.f1=0,_tmp3E0.f2=a2;_tmp3E0;});}}}case 31U: _LL37: _tmp2B8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL38: {struct Cyc_List_List*es=_tmp2B8;
# 1958
_tmp2B7=es;goto _LL3A;}case 24U: _LL39: _tmp2B7=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL3A: {struct Cyc_List_List*es=_tmp2B7;
return({struct _tuple11 _tmp3E1;_tmp3E1.f1=0,({void*_tmp4C1=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_tmp3E1.f2=_tmp4C1;});_tmp3E1;});}case 25U: _LL3B: _tmp2B6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp2B6;
# 1961
_tmp2B5=dles;goto _LL3E;}case 26U: _LL3D: _tmp2B5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL3E: {struct Cyc_List_List*dles=_tmp2B5;
_tmp2B4=dles;goto _LL40;}case 29U: _LL3F: _tmp2B4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2A6)->f3;_LL40: {struct Cyc_List_List*dles=_tmp2B4;
_tmp2B3=dles;goto _LL42;}case 36U: _LL41: _tmp2B3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL42: {struct Cyc_List_List*dles=_tmp2B3;
_tmp2B2=dles;goto _LL44;}case 30U: _LL43: _tmp2B2=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL44: {struct Cyc_List_List*dles=_tmp2B2;
# 1966
for(0;dles != 0;dles=dles->tl){
struct _tuple21*_tmp33F=(struct _tuple21*)dles->hd;struct _tuple21*_stmttmp4E=_tmp33F;struct _tuple21*_tmp340=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp341;_LLA4: _tmp341=_tmp340->f2;_LLA5: {struct Cyc_Absyn_Exp*e=_tmp341;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;}}
# 1970
return({struct _tuple11 _tmp3E2;_tmp3E2.f1=0,_tmp3E2.f2=ain;_tmp3E2;});}case 27U: _LL45: _tmp2AF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2B0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_tmp2B1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2A6)->f3;_LL46: {struct Cyc_Absyn_Vardecl*vd=_tmp2AF;struct Cyc_Absyn_Exp*e1=_tmp2B0;struct Cyc_Absyn_Exp*e2=_tmp2B1;
# 1974
struct _tuple11 _tmp342=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp4F=_tmp342;struct _tuple11 _tmp343=_stmttmp4F;void*_tmp345;void*_tmp344;_LLA7: _tmp344=_tmp343.f1;_tmp345=_tmp343.f2;_LLA8: {void*t1=_tmp344;void*a=_tmp345;
void*_tmp346=t1 == 0?Cyc_Vcgen_fresh_var(): t1;{void*t1=_tmp346;
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp347=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->tag=1U,_tmp34E->f1=vd;_tmp34E;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*v=_tmp347;
# 1978
a=({struct Cyc_Vcgen_Env*_tmp4C4=env;void*_tmp4C3=a;void*_tmp4C2=(void*)v;Cyc_Vcgen_do_assign(_tmp4C4,_tmp4C3,_tmp4C2,Cyc_Vcgen_zero());});
while(1){
void*_tmp348=({void*_tmp4C5=a;Cyc_Vcgen_and(_tmp4C5,Cyc_Vcgen_ult((void*)v,t1));});void*at=_tmp348;
void*_tmp349=({void*_tmp4C6=a;Cyc_Vcgen_and(_tmp4C6,Cyc_Vcgen_ulte(t1,(void*)v));});void*af=_tmp349;
struct _tuple11 _tmp34A=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple11 _stmttmp50=_tmp34A;struct _tuple11 _tmp34B=_stmttmp50;void*_tmp34C;_LLAA: _tmp34C=_tmp34B.f2;_LLAB: {void*abody=_tmp34C;
abody=({struct Cyc_Vcgen_Env*_tmp4CA=env;void*_tmp4C9=abody;void*_tmp4C8=(void*)v;Cyc_Vcgen_do_assign(_tmp4CA,_tmp4C9,_tmp4C8,Cyc_Vcgen_primop(Cyc_Absyn_Plus,({void*_tmp34D[2U];_tmp34D[0]=(void*)v,({void*_tmp4C7=Cyc_Vcgen_one();_tmp34D[1]=_tmp4C7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp34D,sizeof(void*),2U));})));});
if(Cyc_Vcgen_implies(& abody,& a)){a=af;break;}
a=Cyc_Vcgen_weaken_assn(abody,a);}}
# 1987
return({struct _tuple11 _tmp3E3;_tmp3E3.f1=0,_tmp3E3.f2=a;_tmp3E3;});}}}case 28U: _LL47: _tmp2AE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL48: {struct Cyc_Absyn_Exp*e=_tmp2AE;
# 1989
return({struct _tuple11 _tmp3E4;_tmp3E4.f1=0,({void*_tmp4CB=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp3E4.f2=_tmp4CB;});_tmp3E4;});}case 34U: _LL49: _tmp2AD=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL4A: {struct Cyc_Absyn_MallocInfo mi=_tmp2AD;
# 1991
if(mi.rgn != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain)).f2;
return({struct _tuple11 _tmp3E5;_tmp3E5.f1=0,({void*_tmp4CC=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain)).f2;_tmp3E5.f2=_tmp4CC;});_tmp3E5;});}case 35U: _LL4B: _tmp2AB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_tmp2AC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2A6)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmp2AB;struct Cyc_Absyn_Exp*e2=_tmp2AC;
# 1995
struct _tuple11 _tmp34F=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple11 _stmttmp51=_tmp34F;struct _tuple11 _tmp350=_stmttmp51;void*_tmp352;void*_tmp351;_LLAD: _tmp351=_tmp350.f1;_tmp352=_tmp350.f2;_LLAE: {void*lt1=_tmp351;void*a1=_tmp352;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple11 _tmp353=Cyc_Vcgen_vcgen_lexp(env,e2,a1);struct _tuple11 _stmttmp52=_tmp353;struct _tuple11 _tmp354=_stmttmp52;void*_tmp356;void*_tmp355;_LLB0: _tmp355=_tmp354.f1;_tmp356=_tmp354.f2;_LLB1: {void*lt2=_tmp355;void*a2=_tmp356;
Cyc_Vcgen_update_try_assn(env,a2);{
void*_tmp357=Cyc_Vcgen_deref_lterm(env,lt1,a2);void*t1=_tmp357;
void*_tmp358=Cyc_Vcgen_deref_lterm(env,lt2,a2);void*t2=_tmp358;
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple11 _tmp3E6;_tmp3E6.f1=0,_tmp3E6.f2=a2;_tmp3E6;});}}}}}case 37U: _LL4D: _tmp2AA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL4E: {struct Cyc_Absyn_Stmt*s=_tmp2AA;
# 2005
void**_tmp359=(env->shared)->exp_stmt;void**old_exp_stmt=_tmp359;
({void**_tmp4CD=({void**_tmp35A=_cycalloc(sizeof(*_tmp35A));*_tmp35A=0;_tmp35A;});(env->shared)->exp_stmt=_tmp4CD;});{
void*_tmp35B=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);void*aout=_tmp35B;
void*_tmp35C=*((void**)_check_null((env->shared)->exp_stmt));void*t=_tmp35C;
(env->shared)->exp_stmt=old_exp_stmt;
return({struct _tuple11 _tmp3E7;_tmp3E7.f1=t,_tmp3E7.f2=aout;_tmp3E7;});}}case 40U: _LL4F: _LL50:
 return({struct _tuple11 _tmp3E8;_tmp3E8.f1=0,_tmp3E8.f2=ain;_tmp3E8;});case 41U: _LL51: _tmp2A9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL52: {struct Cyc_Absyn_Exp*e=_tmp2A9;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2U: _LL53: _tmp2A8=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL54: {struct _fat_ptr s=_tmp2A8;
# 2014
if(({struct _fat_ptr _tmp4CE=(struct _fat_ptr)s;Cyc_strcmp(_tmp4CE,({const char*_tmp35D="print_assn";_tag_fat(_tmp35D,sizeof(char),11U);}));})){
struct _fat_ptr _tmp35E=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp35E;
({struct Cyc_String_pa_PrintArg_struct _tmp361=({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0U,_tmp3EA.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp3EA;});struct Cyc_String_pa_PrintArg_struct _tmp362=({struct Cyc_String_pa_PrintArg_struct _tmp3E9;_tmp3E9.tag=0U,({struct _fat_ptr _tmp4CF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(ain));_tmp3E9.f1=_tmp4CF;});_tmp3E9;});void*_tmp35F[2U];_tmp35F[0]=& _tmp361,_tmp35F[1]=& _tmp362;({struct Cyc___cycFILE*_tmp4D1=Cyc_stderr;struct _fat_ptr _tmp4D0=({const char*_tmp360="%s: current assn is %s\n";_tag_fat(_tmp360,sizeof(char),24U);});Cyc_fprintf(_tmp4D1,_tmp4D0,_tag_fat(_tmp35F,sizeof(void*),2U));});});}
# 2018
return({struct _tuple11 _tmp3EB;({void*_tmp4D2=Cyc_Vcgen_zero();_tmp3EB.f1=_tmp4D2;}),_tmp3EB.f2=ain;_tmp3EB;});}default: _LL55: _tmp2A7=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp2A6)->f1;_LL56: {struct Cyc_Absyn_Exp*e1=_tmp2A7;
# 2020
struct _tuple11 _tmp363=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_Vcgen_true_assn);struct _tuple11 _stmttmp53=_tmp363;struct _tuple11 _tmp364=_stmttmp53;void*_tmp365;_LLB3: _tmp365=_tmp364.f1;_LLB4: {void*at=_tmp365;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp368=({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0U,({
struct _fat_ptr _tmp4D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(ain)));_tmp3EC.f1=_tmp4D3;});_tmp3EC;});void*_tmp366[1U];_tmp366[0]=& _tmp368;({unsigned _tmp4D5=e->loc;struct _fat_ptr _tmp4D4=({const char*_tmp367="cannot prove @assert from %s";_tag_fat(_tmp367,sizeof(char),29U);});Cyc_Warn_err(_tmp4D5,_tmp4D4,_tag_fat(_tmp366,sizeof(void*),1U));});});
return({struct _tuple11 _tmp3ED;({void*_tmp4D7=Cyc_Vcgen_one();_tmp3ED.f1=_tmp4D7;}),({void*_tmp4D6=Cyc_Vcgen_and(ain,at);_tmp3ED.f2=_tmp4D6;});_tmp3ED;});}}}_LL0:;}}
# 2029
void Cyc_Vcgen_vcgen_topdecls(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp369=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_stmttmp54=_tmp369;void*_tmp36A=_stmttmp54;struct Cyc_List_List*_tmp36B;struct Cyc_List_List*_tmp36C;struct Cyc_Absyn_Fndecl*_tmp36D;switch(*((int*)_tmp36A)){case 1U: _LL1: _tmp36D=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp36A)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp36D;
# 2035
struct Cyc_Vcgen_Env*_tmp36E=Cyc_Vcgen_initial_env(tables,fd);struct Cyc_Vcgen_Env*env=_tmp36E;
void*a=(void*)& Cyc_Vcgen_true_assn;
# 2038
struct _tuple20 _tmp36F=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);struct _tuple20 _stmttmp55=_tmp36F;struct _tuple20 _tmp370=_stmttmp55;struct Cyc_Absyn_Vardecl*_tmp374;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_Absyn_Exp*_tmp372;struct Cyc_List_List*_tmp371;_LLA: _tmp371=_tmp370.f1;_tmp372=_tmp370.f2;_tmp373=_tmp370.f3;_tmp374=_tmp370.f4;_LLB: {struct Cyc_List_List*args=_tmp371;struct Cyc_Absyn_Exp*reqopt=_tmp372;struct Cyc_Absyn_Exp*ensopt=_tmp373;struct Cyc_Absyn_Vardecl*ret_value=_tmp374;
if(reqopt != 0)
# 2041
a=(Cyc_Vcgen_vcgen_test(env,reqopt,a)).f1;
# 2043
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 2045
if(ensopt != 0){
struct Cyc_Absyn_Exp*ens=ensopt;
struct _tuple11 _tmp375=Cyc_Vcgen_vcgen_test(env,ens,(void*)& Cyc_Vcgen_true_assn);struct _tuple11 _stmttmp56=_tmp375;struct _tuple11 _tmp376=_stmttmp56;void*_tmp377;_LLD: _tmp377=_tmp376.f1;_LLE: {void*at=_tmp377;
struct Cyc_Vcgen_SharedEnv*_tmp378=env->shared;struct Cyc_Vcgen_SharedEnv*senv=_tmp378;
void*_tmp379=senv->res_assn;void*res_assn_opt=_tmp379;
void*res_assn=(void*)& Cyc_Vcgen_false_assn;
if(res_assn_opt != 0)res_assn=res_assn_opt;{
struct _fat_ptr str=({const char*_tmp37F="NULL";_tag_fat(_tmp37F,sizeof(char),5U);});
if(senv->res_term != 0)str=Cyc_Vcgen_term2string((void*)_check_null(senv->res_term));
if(ret_value != 0 &&(unsigned)senv->res_term)
res_assn=({void*_tmp4D9=res_assn;Cyc_Vcgen_and(_tmp4D9,({void*_tmp4D8=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->tag=1U,_tmp37A->f1=ret_value;_tmp37A;});Cyc_Vcgen_eq(_tmp4D8,senv->res_term);}));});
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp37D=({struct Cyc_String_pa_PrintArg_struct _tmp3EF;_tmp3EF.tag=0U,({
struct _fat_ptr _tmp4DA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(at)));_tmp3EF.f1=_tmp4DA;});_tmp3EF;});struct Cyc_String_pa_PrintArg_struct _tmp37E=({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0U,({struct _fat_ptr _tmp4DB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(res_assn)));_tmp3EE.f1=_tmp4DB;});_tmp3EE;});void*_tmp37B[2U];_tmp37B[0]=& _tmp37D,_tmp37B[1]=& _tmp37E;({unsigned _tmp4DD=ens->loc;struct _fat_ptr _tmp4DC=({const char*_tmp37C="cannot prove the @ensures clause %s from %s";_tag_fat(_tmp37C,sizeof(char),44U);});Cyc_Warn_err(_tmp4DD,_tmp4DC,_tag_fat(_tmp37B,sizeof(void*),2U));});});}}}
# 2060
Cyc_Vcgen_hash_cons_table=0;
goto _LL0;}}case 9U: _LL3: _tmp36C=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp36A)->f2;_LL4: {struct Cyc_List_List*ds=_tmp36C;
_tmp36B=ds;goto _LL6;}case 10U: _LL5: _tmp36B=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp36A)->f2;_LL6: {struct Cyc_List_List*ds=_tmp36B;
Cyc_Vcgen_vcgen_topdecls(tables,ds);goto _LL0;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}

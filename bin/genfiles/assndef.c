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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145 "list.h"
extern void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
extern int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 383
extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*));struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 61 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*t);
# 63
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e);
# 70
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts);
void*Cyc_AssnDef_cast(void*tp,void*tm);
void*Cyc_AssnDef_fresh_var (void);
# 74
int Cyc_AssnDef_cmp_term(void*t1,void*t2);
# 79
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 96
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 116 "assndef.h"
void*Cyc_AssnDef_and(void*a1,void*a2);
void*Cyc_AssnDef_or(void*a1,void*a2);
# 120
void*Cyc_AssnDef_eq(void*,void*);
void*Cyc_AssnDef_neq(void*,void*);
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 128
void*Cyc_AssnDef_reduce(void*a);
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx);
void*Cyc_AssnDef_not(void*a);
int Cyc_AssnDef_simple_prove(void*ctxt,void*a);
# 41 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
void*_tmp0=t;void*_tmp2;void*_tmp1;void*_tmp4;enum Cyc_Absyn_Primop _tmp3;void*_tmp7;void*_tmp6;enum Cyc_Absyn_Primop _tmp5;int _tmp9;struct Cyc_Absyn_Vardecl*_tmp8;int _tmpA;struct Cyc_Absyn_Vardecl*_tmpB;struct Cyc_Absyn_Exp*_tmpC;switch(*((int*)_tmp0)){case 0U: _LL1: _tmpC=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp0)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpC;
return Cyc_Absynpp_exp2string(e);}case 1U: _LL3: _tmpB=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp0)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpB;
return Cyc_Absynpp_qvar2string(vd->name);}case 2U: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f1 == 0){_LL5: _tmpA=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f2;_LL6: {int i=_tmpA;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmp148;_tmp148.tag=1U,_tmp148.f1=(unsigned long)i;_tmp148;});void*_tmpD[1U];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp178=({const char*_tmpE="_X%d";_tag_fat(_tmpE,sizeof(char),5U);});Cyc_aprintf(_tmp178,_tag_fat(_tmpD,sizeof(void*),1U));});});}}else{_LL7: _tmp8=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f1;_tmp9=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f2;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp8;int i=_tmp9;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0U,({struct _fat_ptr _tmp179=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(((struct Cyc_Absyn_Vardecl*)_check_null(vd))->name));_tmp14A.f1=_tmp179;});_tmp14A;});struct Cyc_Int_pa_PrintArg_struct _tmp13=({struct Cyc_Int_pa_PrintArg_struct _tmp149;_tmp149.tag=1U,_tmp149.f1=(unsigned long)i;_tmp149;});void*_tmp10[2U];_tmp10[0]=& _tmp12,_tmp10[1]=& _tmp13;({struct _fat_ptr _tmp17A=({const char*_tmp11="_%s%d";_tag_fat(_tmp11,sizeof(char),6U);});Cyc_aprintf(_tmp17A,_tag_fat(_tmp10,sizeof(void*),2U));});});}}case 3U: if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl)->tl == 0){_LL9: _tmp5=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f1;_tmp6=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->hd;_tmp7=(void*)((((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl)->hd;_LLA: {enum Cyc_Absyn_Primop p=_tmp5;void*t1=_tmp6;void*t2=_tmp7;
# 48
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0U,({struct _fat_ptr _tmp17B=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp14D.f1=_tmp17B;});_tmp14D;});struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0U,({
struct _fat_ptr _tmp17C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp14C.f1=_tmp17C;});_tmp14C;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0U,({struct _fat_ptr _tmp17D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp14B.f1=_tmp17D;});_tmp14B;});void*_tmp14[3U];_tmp14[0]=& _tmp16,_tmp14[1]=& _tmp17,_tmp14[2]=& _tmp18;({struct _fat_ptr _tmp17E=({const char*_tmp15="(%s%s%s)";_tag_fat(_tmp15,sizeof(char),9U);});Cyc_aprintf(_tmp17E,_tag_fat(_tmp14,sizeof(void*),3U));});});}}else{goto _LLB;}}else{_LLB: _tmp3=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f1;_tmp4=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->hd;_LLC: {enum Cyc_Absyn_Primop p=_tmp3;void*t=_tmp4;
# 51
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B=({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0U,({struct _fat_ptr _tmp17F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp14F.f1=_tmp17F;});_tmp14F;});struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0U,({struct _fat_ptr _tmp180=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp14E.f1=_tmp180;});_tmp14E;});void*_tmp19[2U];_tmp19[0]=& _tmp1B,_tmp19[1]=& _tmp1C;({struct _fat_ptr _tmp181=({const char*_tmp1A="%s(%s)";_tag_fat(_tmp1A,sizeof(char),7U);});Cyc_aprintf(_tmp181,_tag_fat(_tmp19,sizeof(void*),2U));});});}}}else{_LLF: _LL10:
# 54
({void*_tmp21=0U;({struct _fat_ptr _tmp182=({const char*_tmp22="term2string primop args";_tag_fat(_tmp22,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp182,_tag_fat(_tmp21,sizeof(void*),0U));});});}default: _LLD: _tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp0)->f2;_LLE: {void*tp=_tmp1;void*tm=_tmp2;
# 53
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0U,({struct _fat_ptr _tmp183=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_tmp151.f1=_tmp183;});_tmp151;});struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0U,({struct _fat_ptr _tmp184=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp150.f1=_tmp184;});_tmp150;});void*_tmp1D[2U];_tmp1D[0]=& _tmp1F,_tmp1D[1]=& _tmp20;({struct _fat_ptr _tmp185=({const char*_tmp1E="(%s)%s";_tag_fat(_tmp1E,sizeof(char),7U);});Cyc_aprintf(_tmp185,_tag_fat(_tmp1D,sizeof(void*),2U));});});}}_LL0:;}
# 58
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=0U,_tmp23->f1=e;_tmp23;});}
void*Cyc_AssnDef_zero (void){
static void*z=0;
if(z == 0){
struct Cyc_Absyn_Exp*_tmp24=Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U);struct Cyc_Absyn_Exp*e=_tmp24;
({void*_tmp186=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp186;});{
void*_tmp25=Cyc_AssnDef_cnst(e);void*r=_tmp25;
z=r;
return r;}}else{
return(void*)_check_null(z);}}
# 69
void*Cyc_AssnDef_one (void){
static void*n=0;
if(n == 0){
struct Cyc_Absyn_Exp*_tmp26=Cyc_Absyn_int_exp(Cyc_Absyn_None,1,0U);struct Cyc_Absyn_Exp*e=_tmp26;
({void*_tmp187=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp187;});{
void*_tmp27=Cyc_AssnDef_cnst(e);void*r=_tmp27;
n=r;
return r;}}else{
return(void*)_check_null(n);}}
# 80
void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*vd){return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=1U,_tmp28->f1=vd;_tmp28;});}
# 82
inline static int Cyc_AssnDef_is_commutative(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp29=p;switch(_tmp29){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Times: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Eq: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Neq: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Bitand: _LL9: _LLA:
 goto _LLC;case Cyc_Absyn_Bitor: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitxor: _LLD: _LLE:
 return 1;default: _LLF: _LL10:
 return 0;}_LL0:;}
# 94
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts){
return(void*)({struct Cyc_AssnDef_Primop_AssnDef_Term_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=3U,_tmp2A->f1=p,_tmp2A->f2=ts;_tmp2A;});}
# 97
void*Cyc_AssnDef_cast(void*tp,void*tm){
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=4U,_tmp2B->f1=tp,_tmp2B->f2=tm;_tmp2B;});}
# 100
void*Cyc_AssnDef_fresh_var (void){
static int counter=0;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=2U,_tmp2C->f1=0,_tmp2C->f2=counter ++;_tmp2C;});}struct _tuple11{void*f1;void*f2;};
# 105
int Cyc_AssnDef_cmp_term(void*t1,void*t2){
struct _tuple11 _tmp2D=({struct _tuple11 _tmp152;_tmp152.f1=t1,_tmp152.f2=t2;_tmp152;});struct _tuple11 _stmttmp0=_tmp2D;struct _tuple11 _tmp2E=_stmttmp0;struct Cyc_List_List*_tmp32;enum Cyc_Absyn_Primop _tmp31;struct Cyc_List_List*_tmp30;enum Cyc_Absyn_Primop _tmp2F;void*_tmp36;void*_tmp35;void*_tmp34;void*_tmp33;int _tmp3A;struct Cyc_Absyn_Vardecl*_tmp39;int _tmp38;struct Cyc_Absyn_Vardecl*_tmp37;struct Cyc_Absyn_Vardecl*_tmp3C;struct Cyc_Absyn_Vardecl*_tmp3B;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp3D;if(((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2E.f1)->tag == 0U){if(((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2E.f2)->tag == 0U){_LL1: _tmp3D=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2E.f1)->f1;_tmp3E=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2E.f2)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3D;struct Cyc_Absyn_Exp*e2=_tmp3E;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2E.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2E.f1)->tag == 1U){if(((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2E.f2)->tag == 1U){_LL7: _tmp3B=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2E.f1)->f1;_tmp3C=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2E.f2)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd1=_tmp3B;struct Cyc_Absyn_Vardecl*vd2=_tmp3C;
return(int)vd1 - (int)vd2;}}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2E.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2E.f1)->tag == 2U){if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2E.f2)->tag == 2U){_LLD: _tmp37=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2E.f1)->f1;_tmp38=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2E.f1)->f2;_tmp39=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2E.f2)->f1;_tmp3A=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2E.f2)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd1=_tmp37;int i=_tmp38;struct Cyc_Absyn_Vardecl*vd2=_tmp39;int j=_tmp3A;
# 114
if(vd1 == vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{_LLF: _LL10:
 return - 1;}}else{if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2E.f2)->tag == 2U){_LL11: _LL12:
 return 1;}else{if(((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2E.f1)->tag == 4U){if(((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2E.f2)->tag == 4U){_LL13: _tmp33=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2E.f1)->f1;_tmp34=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2E.f1)->f2;_tmp35=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2E.f2)->f1;_tmp36=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2E.f2)->f2;_LL14: {void*tp1=_tmp33;void*tm1=_tmp34;void*tp2=_tmp35;void*tm2=_tmp36;
# 119
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c != 0)return c;
return Cyc_AssnDef_cmp_term(tm1,tm2);}}else{_LL15: _LL16:
 return - 1;}}else{if(((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2E.f2)->tag == 4U){_LL17: _LL18:
 return 1;}else{_LL19: _tmp2F=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2E.f1)->f1;_tmp30=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2E.f1)->f2;_tmp31=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2E.f2)->f1;_tmp32=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2E.f2)->f2;_LL1A: {enum Cyc_Absyn_Primop p1=_tmp2F;struct Cyc_List_List*ts1=_tmp30;enum Cyc_Absyn_Primop p2=_tmp31;struct Cyc_List_List*ts2=_tmp32;
# 125
if((int)p1 != (int)p2)return(int)p1 - (int)p2;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_AssnDef_cmp_term,ts1,ts2);}}}}}}}}}_LL0:;}
# 130
static int Cyc_AssnDef_same_term(void*t1,void*t2){return Cyc_AssnDef_cmp_term(t1,t2)== 0;}
# 148
static unsigned Cyc_AssnDef_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 150
LOOP:
 if(((int(*)(struct Cyc_List_List*l,void*x))Cyc_List_memq)(*prev,a))return c;
({struct Cyc_List_List*_tmp188=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->hd=a,_tmp3F->tl=*prev;_tmp3F;});*prev=_tmp188;});{
void*_tmp40=a;void*_tmp41;void*_tmp44;void*_tmp43;struct Cyc_Absyn_Vardecl*_tmp42;void*_tmp46;void*_tmp45;void*_tmp48;void*_tmp47;switch(*((int*)_tmp40)){case 0U: _LL1: _LL2:
 return c;case 1U: _LL3: _LL4:
 return c;case 2U: _LL5: _LL6:
 return(unsigned)1 + c;case 3U: _LL7: _tmp47=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp40)->f1;_tmp48=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp40)->f2;_LL8: {void*a1=_tmp47;void*a2=_tmp48;
_tmp45=a1;_tmp46=a2;goto _LLA;}case 4U: _LL9: _tmp45=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp40)->f1;_tmp46=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp40)->f2;_LLA: {void*a1=_tmp45;void*a2=_tmp46;
# 159
c=Cyc_AssnDef_assn_size_loop(a2,(unsigned)1 + c,prev);
a=a1;
goto LOOP;}case 5U: _LLB: _tmp42=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp40)->f1;_tmp43=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp40)->f2;_tmp44=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp40)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp42;void*t=_tmp43;void*a1=_tmp44;
# 163
++ c;
a=a1;
goto LOOP;}default: _LLD: _tmp41=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp40)->f1;_LLE: {void*a1=_tmp41;
# 167
++ c;
a=a1;
goto LOOP;}}_LL0:;}}
# 174
static unsigned Cyc_AssnDef_max_assn_size=0U;
# 177
static unsigned Cyc_AssnDef_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned _tmp49=Cyc_AssnDef_assn_size_loop(a,0U,& prev);unsigned c=_tmp49;
if(c > Cyc_AssnDef_max_assn_size)
Cyc_AssnDef_max_assn_size=c;
# 183
return c;}
# 188
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec,int*size){
void*_tmp4A=a;void*_tmp4B;void*_tmp4E;void*_tmp4D;struct Cyc_Absyn_Vardecl*_tmp4C;void*_tmp50;void*_tmp4F;void*_tmp52;void*_tmp51;void*_tmp55;enum Cyc_AssnDef_Primreln _tmp54;void*_tmp53;switch(*((int*)_tmp4A)){case 0U: _LL1: _LL2:
 return({const char*_tmp56="true";_tag_fat(_tmp56,sizeof(char),5U);});case 1U: _LL3: _LL4:
 return({const char*_tmp57="false";_tag_fat(_tmp57,sizeof(char),6U);});case 2U: _LL5: _tmp53=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp4A)->f1;_tmp54=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp4A)->f2;_tmp55=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp4A)->f3;_LL6: {void*t1=_tmp53;enum Cyc_AssnDef_Primreln p=_tmp54;void*t2=_tmp55;
# 193
*size +=1;{
struct _fat_ptr ps;
{enum Cyc_AssnDef_Primreln _tmp58=p;switch(_tmp58){case Cyc_AssnDef_Eq: _LL10: _LL11:
 ps=({const char*_tmp59="==";_tag_fat(_tmp59,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_Neq: _LL12: _LL13:
 ps=({const char*_tmp5A="!=";_tag_fat(_tmp5A,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_SLt: _LL14: _LL15:
 ps=({const char*_tmp5B="S<";_tag_fat(_tmp5B,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_SLte: _LL16: _LL17:
 ps=({const char*_tmp5C="S<=";_tag_fat(_tmp5C,sizeof(char),4U);});goto _LLF;case Cyc_AssnDef_ULt: _LL18: _LL19:
 ps=({const char*_tmp5D="U<";_tag_fat(_tmp5D,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_ULte: _LL1A: _LL1B:
 ps=({const char*_tmp5E="U<=";_tag_fat(_tmp5E,sizeof(char),4U);});goto _LLF;default: _LL1C: _LL1D:
({void*_tmp5F=0U;({struct _fat_ptr _tmp189=({const char*_tmp60="assn2string primop";_tag_fat(_tmp60,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp189,_tag_fat(_tmp5F,sizeof(void*),0U));});});}_LLF:;}
# 204
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0U,({struct _fat_ptr _tmp18A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp155.f1=_tmp18A;});_tmp155;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U,_tmp154.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_tmp154;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0U,({struct _fat_ptr _tmp18B=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp153.f1=_tmp18B;});_tmp153;});void*_tmp61[3U];_tmp61[0]=& _tmp63,_tmp61[1]=& _tmp64,_tmp61[2]=& _tmp65;({struct _fat_ptr _tmp18C=({const char*_tmp62="%s%s%s";_tag_fat(_tmp62,sizeof(char),7U);});Cyc_aprintf(_tmp18C,_tag_fat(_tmp61,sizeof(void*),3U));});});}}case 4U: _LL7: _tmp51=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp4A)->f1;_tmp52=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp4A)->f2;_LL8: {void*a1=_tmp51;void*a2=_tmp52;
# 206
*size +=1;
if(prec <= 5)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0U,({struct _fat_ptr _tmp18D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_tmp157.f1=_tmp18D;});_tmp157;});struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U,({struct _fat_ptr _tmp18E=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_tmp156.f1=_tmp18E;});_tmp156;});void*_tmp66[2U];_tmp66[0]=& _tmp68,_tmp66[1]=& _tmp69;({struct _fat_ptr _tmp18F=({const char*_tmp67="%s || %s";_tag_fat(_tmp67,sizeof(char),9U);});Cyc_aprintf(_tmp18F,_tag_fat(_tmp66,sizeof(void*),2U));});});else{
# 210
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0U,({struct _fat_ptr _tmp190=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_tmp159.f1=_tmp190;});_tmp159;});struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0U,({struct _fat_ptr _tmp191=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_tmp158.f1=_tmp191;});_tmp158;});void*_tmp6A[2U];_tmp6A[0]=& _tmp6C,_tmp6A[1]=& _tmp6D;({struct _fat_ptr _tmp192=({const char*_tmp6B="(%s || %s)";_tag_fat(_tmp6B,sizeof(char),11U);});Cyc_aprintf(_tmp192,_tag_fat(_tmp6A,sizeof(void*),2U));});});}}case 3U: _LL9: _tmp4F=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp4A)->f1;_tmp50=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp4A)->f2;_LLA: {void*a1=_tmp4F;void*a2=_tmp50;
# 212
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0U,({struct _fat_ptr _tmp193=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_tmp15B.f1=_tmp193;});_tmp15B;});struct Cyc_String_pa_PrintArg_struct _tmp71=({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0U,({struct _fat_ptr _tmp194=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_tmp15A.f1=_tmp194;});_tmp15A;});void*_tmp6E[2U];_tmp6E[0]=& _tmp70,_tmp6E[1]=& _tmp71;({struct _fat_ptr _tmp195=({const char*_tmp6F="%s && %s";_tag_fat(_tmp6F,sizeof(char),9U);});Cyc_aprintf(_tmp195,_tag_fat(_tmp6E,sizeof(void*),2U));});});}case 5U: _LLB: _tmp4C=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp4A)->f1;_tmp4D=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp4A)->f2;_tmp4E=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp4A)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp4C;void*t=_tmp4D;void*a=_tmp4E;
# 215
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0U,({struct _fat_ptr _tmp196=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp15E.f1=_tmp196;});_tmp15E;});struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp15D;_tmp15D.tag=0U,({
struct _fat_ptr _tmp197=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp15D.f1=_tmp197;});_tmp15D;});struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0U,({struct _fat_ptr _tmp198=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_tmp15C.f1=_tmp198;});_tmp15C;});void*_tmp72[3U];_tmp72[0]=& _tmp74,_tmp72[1]=& _tmp75,_tmp72[2]=& _tmp76;({struct _fat_ptr _tmp199=({const char*_tmp73="Subst[%s:=%s](%s)";_tag_fat(_tmp73,sizeof(char),18U);});Cyc_aprintf(_tmp199,_tag_fat(_tmp72,sizeof(void*),3U));});});}default: _LLD: _tmp4B=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp4A)->f1;_LLE: {void*a=_tmp4B;
# 219
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0U,({struct _fat_ptr _tmp19A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_tmp15F.f1=_tmp19A;});_tmp15F;});void*_tmp77[1U];_tmp77[0]=& _tmp79;({struct _fat_ptr _tmp19B=({const char*_tmp78="Kill(%s)";_tag_fat(_tmp78,sizeof(char),9U);});Cyc_aprintf(_tmp19B,_tag_fat(_tmp77,sizeof(void*),1U));});});}}_LL0:;}
# 224
int Cyc_AssnDef_tree_size=0;
# 227
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
Cyc_AssnDef_tree_size=0;
return Cyc_AssnDef_a2string(a,0,& Cyc_AssnDef_tree_size);}
# 233
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0U};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1U};
# 244 "assndef.cyc"
static int Cyc_AssnDef_assnhash(void*a){
void*_tmp7A=a;void*_tmp7B;void*_tmp7E;void*_tmp7D;struct Cyc_Absyn_Vardecl*_tmp7C;void*_tmp80;void*_tmp7F;void*_tmp82;void*_tmp81;void*_tmp85;enum Cyc_AssnDef_Primreln _tmp84;void*_tmp83;switch(*((int*)_tmp7A)){case 1U: _LL1: _LL2:
 return 0;case 0U: _LL3: _LL4:
 return 1;case 2U: _LL5: _tmp83=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp7A)->f1;_tmp84=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp7A)->f2;_tmp85=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp7A)->f3;_LL6: {void*t1=_tmp83;enum Cyc_AssnDef_Primreln p=_tmp84;void*t2=_tmp85;
# 249
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3U: _LL7: _tmp81=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp7A)->f1;_tmp82=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp7A)->f2;_LL8: {void*a1=_tmp81;void*a2=_tmp82;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4U: _LL9: _tmp7F=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp7A)->f1;_tmp80=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp7A)->f2;_LLA: {void*a1=_tmp7F;void*a2=_tmp80;
return(int)((unsigned)a1 ^ (unsigned)a2 | (unsigned)1);}case 5U: _LLB: _tmp7C=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp7A)->f1;_tmp7D=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp7A)->f2;_tmp7E=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp7A)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp7C;void*t=_tmp7D;void*a=_tmp7E;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | (unsigned)3);}default: _LLD: _tmp7B=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp7A)->f1;_LLE: {void*a=_tmp7B;
# 254
return(int)((unsigned)a + (unsigned)37);}}_LL0:;}
# 260
static int Cyc_AssnDef_assncmp(void*a1,void*a2){
LOOP:
 if(a1 == a2)return 0;{
struct _tuple11 _tmp86=({struct _tuple11 _tmp160;_tmp160.f1=a1,_tmp160.f2=a2;_tmp160;});struct _tuple11 _stmttmp1=_tmp86;struct _tuple11 _tmp87=_stmttmp1;void*_tmp8D;void*_tmp8C;struct Cyc_Absyn_Vardecl*_tmp8B;void*_tmp8A;void*_tmp89;struct Cyc_Absyn_Vardecl*_tmp88;void*_tmp8F;void*_tmp8E;void*_tmp95;enum Cyc_AssnDef_Primreln _tmp94;void*_tmp93;void*_tmp92;enum Cyc_AssnDef_Primreln _tmp91;void*_tmp90;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;void*_tmp9D;void*_tmp9C;void*_tmp9B;void*_tmp9A;if(((struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_tmp87.f1)->tag == 0U){if(((struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_tmp87.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_tmp87.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_tmp87.f1)->tag == 1U){if(((struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_tmp87.f2)->tag == 1U){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_tmp87.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{switch(*((int*)_tmp87.f1)){case 4U: if(((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp87.f2)->tag == 4U){_LLD: _tmp9A=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp87.f1)->f1;_tmp9B=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp87.f1)->f2;_tmp9C=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp87.f2)->f1;_tmp9D=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp87.f2)->f2;_LLE: {void*a11=_tmp9A;void*a12=_tmp9B;void*a21=_tmp9C;void*a22=_tmp9D;
_tmp96=a11;_tmp97=a12;_tmp98=a21;_tmp99=a22;goto _LL10;}}else{_LL11: _LL12:
# 277
 return - 1;}case 3U: switch(*((int*)_tmp87.f2)){case 3U: _LLF: _tmp96=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp87.f1)->f1;_tmp97=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp87.f1)->f2;_tmp98=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp87.f2)->f1;_tmp99=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp87.f2)->f2;_LL10: {void*a11=_tmp96;void*a12=_tmp97;void*a21=_tmp98;void*a22=_tmp99;
# 272
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c != 0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4U: goto _LL13;default: _LL15: _LL16:
# 279
 return - 1;}default: switch(*((int*)_tmp87.f2)){case 4U: _LL13: _LL14:
# 278
 return 1;case 3U: _LL17: _LL18:
# 280
 return 1;default: if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f1)->tag == 2U){if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f2)->tag == 2U){_LL19: _tmp90=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f1)->f1;_tmp91=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f1)->f2;_tmp92=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f1)->f3;_tmp93=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f2)->f1;_tmp94=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f2)->f2;_tmp95=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f2)->f3;_LL1A: {void*t11=_tmp90;enum Cyc_AssnDef_Primreln p1=_tmp91;void*t12=_tmp92;void*t21=_tmp93;enum Cyc_AssnDef_Primreln p2=_tmp94;void*t22=_tmp95;
# 282
int c=(int)p1 - (int)p2;
if(c != 0)return c;
c=Cyc_AssnDef_cmp_term(t11,t21);
if(c != 0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{_LL1B: _LL1C:
 return - 1;}}else{if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp87.f2)->tag == 2U){_LL1D: _LL1E:
 return 1;}else{if(((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp87.f1)->tag == 6U){if(((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp87.f2)->tag == 6U){_LL1F: _tmp8E=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp87.f1)->f1;_tmp8F=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp87.f2)->f1;_LL20: {void*a1=_tmp8E;void*a2=_tmp8F;
return Cyc_AssnDef_assncmp(a1,a2);}}else{_LL21: _LL22:
 return - 1;}}else{if(((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp87.f2)->tag == 6U){_LL23: _LL24:
 return 1;}else{_LL25: _tmp88=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp87.f1)->f1;_tmp89=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp87.f1)->f2;_tmp8A=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp87.f1)->f3;_tmp8B=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp87.f2)->f1;_tmp8C=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp87.f2)->f2;_tmp8D=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp87.f2)->f3;_LL26: {struct Cyc_Absyn_Vardecl*vd1=_tmp88;void*t1=_tmp89;void*a11=_tmp8A;struct Cyc_Absyn_Vardecl*vd2=_tmp8B;void*t2=_tmp8C;void*a22=_tmp8D;
# 293
int c=(int)vd1 - (int)vd2;
if(c != 0)return c;
c=Cyc_AssnDef_cmp_term(t1,t2);
if(c != 0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}}_LL0:;}}
# 306
static struct Cyc_Hashtable_Table*Cyc_AssnDef_hash_cons_table=0;
# 309
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_hash_cons_table=0;}struct _tuple12{void*f1;int f2;};
# 314
int Cyc_AssnDef_is_reduced(void*a){
if(Cyc_AssnDef_hash_cons_table == 0)return 0;{
struct _tuple12**_tmp9E=((struct _tuple12**(*)(struct Cyc_Hashtable_Table*t,void*key))Cyc_Hashtable_lookup_opt)((struct Cyc_Hashtable_Table*)_check_null(Cyc_AssnDef_hash_cons_table),a);struct _tuple12**resopt=_tmp9E;
if(resopt == 0)return 0;{
struct _tuple12*_tmp9F=*resopt;struct _tuple12*_stmttmp2=_tmp9F;struct _tuple12*_tmpA0=_stmttmp2;int _tmpA1;_LL1: _tmpA1=_tmpA0->f2;_LL2: {int b=_tmpA1;
return b;}}}}
# 325
static struct _tuple12 Cyc_AssnDef_copy_assn(void*a){
void*_tmpA2=a;void*_tmpA3;void*_tmpA6;void*_tmpA5;struct Cyc_Absyn_Vardecl*_tmpA4;void*_tmpA9;enum Cyc_AssnDef_Primreln _tmpA8;void*_tmpA7;void*_tmpAB;void*_tmpAA;void*_tmpAD;void*_tmpAC;switch(*((int*)_tmpA2)){case 0U: _LL1: _LL2:
 return({struct _tuple12 _tmp161;_tmp161.f1=(void*)& Cyc_AssnDef_true_assn,_tmp161.f2=1;_tmp161;});case 1U: _LL3: _LL4:
 return({struct _tuple12 _tmp162;_tmp162.f1=(void*)& Cyc_AssnDef_false_assn,_tmp162.f2=0;_tmp162;});case 3U: _LL5: _tmpAC=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpA2)->f1;_tmpAD=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpA2)->f2;_LL6: {void*a1=_tmpAC;void*a2=_tmpAD;
# 330
return({struct _tuple12 _tmp163;({void*_tmp19D=(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=3U,_tmpAE->f1=a1,_tmpAE->f2=a2;_tmpAE;});_tmp163.f1=_tmp19D;}),({int _tmp19C=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_tmp163.f2=_tmp19C;});_tmp163;});}case 4U: _LL7: _tmpAA=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpA2)->f1;_tmpAB=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpA2)->f2;_LL8: {void*a1=_tmpAA;void*a2=_tmpAB;
# 332
return({struct _tuple12 _tmp164;({void*_tmp19F=(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=4U,_tmpAF->f1=a1,_tmpAF->f2=a2;_tmpAF;});_tmp164.f1=_tmp19F;}),({int _tmp19E=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_tmp164.f2=_tmp19E;});_tmp164;});}case 2U: _LL9: _tmpA7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpA2)->f1;_tmpA8=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpA2)->f2;_tmpA9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpA2)->f3;_LLA: {void*t1=_tmpA7;enum Cyc_AssnDef_Primreln p=_tmpA8;void*t2=_tmpA9;
return({struct _tuple12 _tmp165;({void*_tmp1A0=(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=2U,_tmpB0->f1=t1,_tmpB0->f2=p,_tmpB0->f3=t2;_tmpB0;});_tmp165.f1=_tmp1A0;}),_tmp165.f2=1;_tmp165;});}case 5U: _LLB: _tmpA4=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpA2)->f1;_tmpA5=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpA2)->f2;_tmpA6=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpA2)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd1=_tmpA4;void*t1=_tmpA5;void*a1=_tmpA6;
return({struct _tuple12 _tmp166;({void*_tmp1A1=(void*)({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=5U,_tmpB1->f1=vd1,_tmpB1->f2=t1,_tmpB1->f3=a1;_tmpB1;});_tmp166.f1=_tmp1A1;}),_tmp166.f2=0;_tmp166;});}default: _LLD: _tmpA3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmpA2)->f1;_LLE: {void*a=_tmpA3;
return({struct _tuple12 _tmp167;({void*_tmp1A2=(void*)({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=6U,_tmpB2->f1=a;_tmpB2;});_tmp167.f1=_tmp1A2;}),_tmp167.f2=0;_tmp167;});}}_LL0:;}
# 343
static void*Cyc_AssnDef_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_hash_cons_table == 0){
h=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_create)(123,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_hash_cons_table=h;
# 350
({struct Cyc_Hashtable_Table*_tmp1A3=h;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple12*val))Cyc_Hashtable_insert)(_tmp1A3,(void*)& Cyc_AssnDef_true_assn,({struct _tuple12*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->f1=(void*)& Cyc_AssnDef_true_assn,_tmpB3->f2=1;_tmpB3;}));});
({struct Cyc_Hashtable_Table*_tmp1A4=h;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple12*val))Cyc_Hashtable_insert)(_tmp1A4,(void*)& Cyc_AssnDef_false_assn,({struct _tuple12*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=(void*)& Cyc_AssnDef_false_assn,_tmpB4->f2=1;_tmpB4;}));});}else{
# 353
h=(struct Cyc_Hashtable_Table*)_check_null(Cyc_AssnDef_hash_cons_table);}{
struct _tuple12**_tmpB5=((struct _tuple12**(*)(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_lookup_other_opt)(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);struct _tuple12**resopt=_tmpB5;
if(resopt == 0){
struct _tuple12 _tmpB6=Cyc_AssnDef_copy_assn(a);struct _tuple12 _stmttmp3=_tmpB6;struct _tuple12 _tmpB7=_stmttmp3;int _tmpB9;void*_tmpB8;_LL1: _tmpB8=_tmpB7.f1;_tmpB9=_tmpB7.f2;_LL2: {void*res=_tmpB8;int reduced=_tmpB9;
({struct Cyc_Hashtable_Table*_tmp1A6=h;void*_tmp1A5=res;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple12*val))Cyc_Hashtable_insert)(_tmp1A6,_tmp1A5,({struct _tuple12*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=res,_tmpBA->f2=reduced;_tmpBA;}));});
return res;}}else{
# 360
struct _tuple12*_tmpBB=*resopt;struct _tuple12*_stmttmp4=_tmpBB;struct _tuple12*_tmpBC=_stmttmp4;void*_tmpBD;_LL4: _tmpBD=_tmpBC->f1;_LL5: {void*res=_tmpBD;
return res;}}}}
# 366
void*Cyc_AssnDef_kill(void*a){
void*_tmpBE=a;switch(*((int*)_tmpBE)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 goto _LL6;case 6U: _LL5: _LL6:
 return a;default: _LL7: _LL8: {
# 372
struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _tmpBF=({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _tmp168;_tmp168.tag=6U,_tmp168.f1=a;_tmp168;});struct Cyc_AssnDef_Kill_AssnDef_Assn_struct p=_tmpBF;
return Cyc_AssnDef_hash_cons((void*)& p);}}_LL0:;}
# 378
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmpC0=({struct _tuple11 _tmp16A;_tmp16A.f1=a1,_tmp16A.f2=a2;_tmp16A;});struct _tuple11 _stmttmp5=_tmpC0;struct _tuple11 _tmpC1=_stmttmp5;if(((struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_tmpC1.f1)->tag == 0U){_LL1: _LL2:
 return a2;}else{if(((struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_tmpC1.f2)->tag == 0U){_LL3: _LL4:
 return a1;}else{if(((struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_tmpC1.f1)->tag == 1U){_LL5: _LL6:
 return a1;}else{if(((struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_tmpC1.f2)->tag == 1U){_LL7: _LL8:
 return a2;}else{_LL9: _LLA:
# 389
 if((int)a1 > (int)a2)
({void*_tmpC2=a1;void*_tmpC3=a2;a1=_tmpC3;a2=_tmpC2;});{
struct Cyc_AssnDef_And_AssnDef_Assn_struct _tmpC4=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _tmp169;_tmp169.tag=3U,_tmp169.f1=a1,_tmp169.f2=a2;_tmp169;});struct Cyc_AssnDef_And_AssnDef_Assn_struct a=_tmpC4;
return Cyc_AssnDef_hash_cons((void*)& a);}}}}}_LL0:;}}
# 397
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmpC5=({struct _tuple11 _tmp16C;_tmp16C.f1=a1,_tmp16C.f2=a2;_tmp16C;});struct _tuple11 _stmttmp6=_tmpC5;struct _tuple11 _tmpC6=_stmttmp6;if(((struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_tmpC6.f1)->tag == 0U){_LL1: _LL2:
 return a1;}else{if(((struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_tmpC6.f2)->tag == 0U){_LL3: _LL4:
 return a2;}else{if(((struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_tmpC6.f1)->tag == 1U){_LL5: _LL6:
 return a2;}else{if(((struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_tmpC6.f2)->tag == 1U){_LL7: _LL8:
 return a1;}else{_LL9: _LLA:
# 406
 if((int)a1 > (int)a2)
({void*_tmpC7=a1;void*_tmpC8=a2;a1=_tmpC8;a2=_tmpC7;});{
struct Cyc_AssnDef_Or_AssnDef_Assn_struct _tmpC9=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _tmp16B;_tmp16B.tag=4U,_tmp16B.f1=a1,_tmp16B.f2=a2;_tmp16B;});struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=_tmpC9;
return Cyc_AssnDef_hash_cons((void*)& a);}}}}}_LL0:;}}
# 414
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
void*_tmpCA=a;switch(*((int*)_tmpCA)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return a;default: _LL5: _LL6: {
# 419
struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _tmpCB=({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _tmp16D;_tmp16D.tag=5U,_tmp16D.f1=x,_tmp16D.f2=newx,_tmp16D.f3=a;_tmp16D;});struct Cyc_AssnDef_Subst_AssnDef_Assn_struct p=_tmpCB;
return Cyc_AssnDef_hash_cons((void*)& p);}}_LL0:;}
# 425
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _tmpCC=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _tmp16E;_tmp16E.tag=2U,_tmp16E.f1=t1,_tmp16E.f2=p,_tmp16E.f3=t2;_tmp16E;});struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=_tmpCC;
return Cyc_AssnDef_hash_cons((void*)& ptr);}
# 435
void*Cyc_AssnDef_eq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_Eq,t2);}
void*Cyc_AssnDef_neq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_Neq,t2);}
void*Cyc_AssnDef_slt(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_SLt,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_SLte,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_ULt,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_ULte,t2);}
# 452 "assndef.cyc"
void*Cyc_AssnDef_not(void*a){
void*_tmpCD=a;void*_tmpD0;enum Cyc_AssnDef_Primreln _tmpCF;void*_tmpCE;void*_tmpD2;void*_tmpD1;void*_tmpD4;void*_tmpD3;switch(*((int*)_tmpCD)){case 0U: _LL1: _LL2:
 return(void*)& Cyc_AssnDef_false_assn;case 1U: _LL3: _LL4:
 return(void*)& Cyc_AssnDef_true_assn;case 3U: _LL5: _tmpD3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpCD)->f1;_tmpD4=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpCD)->f2;_LL6: {void*a1=_tmpD3;void*a2=_tmpD4;
return({void*_tmp1A7=Cyc_AssnDef_not(a1);Cyc_AssnDef_or(_tmp1A7,Cyc_AssnDef_not(a2));});}case 4U: _LL7: _tmpD1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpCD)->f1;_tmpD2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpCD)->f2;_LL8: {void*a1=_tmpD1;void*a2=_tmpD2;
return({void*_tmp1A8=Cyc_AssnDef_not(a1);Cyc_AssnDef_and(_tmp1A8,Cyc_AssnDef_not(a2));});}case 2U: _LL9: _tmpCE=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpCD)->f1;_tmpCF=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpCD)->f2;_tmpD0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpCD)->f3;_LLA: {void*t1=_tmpCE;enum Cyc_AssnDef_Primreln p=_tmpCF;void*t2=_tmpD0;
# 459
enum Cyc_AssnDef_Primreln _tmpD5=p;switch(_tmpD5){case Cyc_AssnDef_Eq: _LL10: _LL11:
 return Cyc_AssnDef_neq(t1,t2);case Cyc_AssnDef_Neq: _LL12: _LL13:
 return Cyc_AssnDef_eq(t1,t2);case Cyc_AssnDef_SLt: _LL14: _LL15:
 return Cyc_AssnDef_slte(t2,t1);case Cyc_AssnDef_SLte: _LL16: _LL17:
 return Cyc_AssnDef_slt(t2,t1);case Cyc_AssnDef_ULt: _LL18: _LL19:
 return Cyc_AssnDef_ulte(t2,t1);case Cyc_AssnDef_ULte: _LL1A: _LL1B:
 return Cyc_AssnDef_ult(t2,t1);default: _LL1C: _LL1D:
({void*_tmpD6=0U;({struct _fat_ptr _tmp1A9=({const char*_tmpD7="Vcgen::invert: non-primop";_tag_fat(_tmpD7,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1A9,_tag_fat(_tmpD6,sizeof(void*),0U));});});}_LLF:;}case 5U: _LLB: _LLC:
# 468
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));default: _LLD: _LLE:
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));}_LL0:;}struct _tuple13{struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 491 "assndef.cyc"
static void*Cyc_AssnDef_reduce_term(void*t,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 494
void*_tmpD8=t;struct Cyc_Absyn_Vardecl*_tmpD9;struct Cyc_List_List*_tmpDB;enum Cyc_Absyn_Primop _tmpDA;void*_tmpDD;void*_tmpDC;switch(*((int*)_tmpD8)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return t;case 4U: _LL5: _tmpDC=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpD8)->f1;_tmpDD=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpD8)->f2;_LL6: {void*tp=_tmpDC;void*tm=_tmpDD;
# 498
void*_tmpDE=Cyc_AssnDef_reduce_term(tm,non_esc_subst,esc_subst);void*tm2=_tmpDE;
if(tm != tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 3U: _LL7: _tmpDA=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpD8)->f1;_tmpDB=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpD8)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmpDA;struct Cyc_List_List*ts=_tmpDB;
# 502
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpDF=ts;struct Cyc_List_List*ptr=_tmpDF;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpE0=(void*)ptr->hd;void*old_term=_tmpE0;
void*_tmpE1=Cyc_AssnDef_reduce_term(old_term,non_esc_subst,esc_subst);void*new_term=_tmpE1;
res=({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->hd=new_term,_tmpE2->tl=res;_tmpE2;});
if(new_term != old_term)change=1;}}
# 510
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp1AA=p;Cyc_AssnDef_primop(_tmp1AA,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res));});}default: _LL9: _tmpD9=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmpD8)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpD9;
# 513
if(vd->escapes && esc_subst != 0){
# 516
struct Cyc_List_List*_tmpE3=*esc_subst;struct Cyc_List_List*s=_tmpE3;
for(0;s != 0;s=s->tl){
struct _tuple13 _tmpE4=*((struct _tuple13*)s->hd);struct _tuple13 _stmttmp7=_tmpE4;struct _tuple13 _tmpE5=_stmttmp7;void*_tmpE7;struct Cyc_Absyn_Vardecl*_tmpE6;_LLC: _tmpE6=_tmpE5.f1;_tmpE7=_tmpE5.f2;_LLD: {struct Cyc_Absyn_Vardecl*vd2=_tmpE6;void*t2=_tmpE7;
if(vd2 == vd)return t;}}{
# 523
void*_tmpE8=Cyc_AssnDef_fresh_var();void*t=_tmpE8;
({struct Cyc_List_List*_tmp1AC=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));({struct _tuple13*_tmp1AB=({struct _tuple13*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->f1=vd,_tmpE9->f2=t;_tmpE9;});_tmpEA->hd=_tmp1AB;}),_tmpEA->tl=*esc_subst;_tmpEA;});*esc_subst=_tmp1AC;});
return Cyc_AssnDef_fresh_var();}}else{
if(!vd->escapes)
# 528
for(0;non_esc_subst != 0;non_esc_subst=non_esc_subst->tl){
struct _tuple13 _tmpEB=*((struct _tuple13*)non_esc_subst->hd);struct _tuple13 _stmttmp8=_tmpEB;struct _tuple13 _tmpEC=_stmttmp8;void*_tmpEE;struct Cyc_Absyn_Vardecl*_tmpED;_LLF: _tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;_LL10: {struct Cyc_Absyn_Vardecl*vd2=_tmpED;void*t2=_tmpEE;
if(vd == vd2)return t2;}}}
# 533
return t;}}_LL0:;}
# 537
static void*Cyc_AssnDef_reduce_assn(void*a,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 540
LOOP: {
void*_tmpEF=a;void*_tmpF2;void*_tmpF1;struct Cyc_Absyn_Vardecl*_tmpF0;void*_tmpF3;void*_tmpF5;void*_tmpF4;void*_tmpF7;void*_tmpF6;void*_tmpFA;enum Cyc_AssnDef_Primreln _tmpF9;void*_tmpF8;switch(*((int*)_tmpEF)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return a;case 2U: _LL5: _tmpF8=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpEF)->f1;_tmpF9=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpEF)->f2;_tmpFA=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpEF)->f3;_LL6: {void*t1=_tmpF8;enum Cyc_AssnDef_Primreln p=_tmpF9;void*t2=_tmpFA;
# 545
void*_tmpFB=Cyc_AssnDef_reduce_term(t1,non_esc_subst,esc_subst);void*newt1=_tmpFB;
void*_tmpFC=Cyc_AssnDef_reduce_term(t2,non_esc_subst,esc_subst);void*newt2=_tmpFC;
if(t1 == newt1 && t2 == newt2)return a;
return Cyc_AssnDef_prim(newt1,p,newt2);}case 3U: _LL7: _tmpF6=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpEF)->f1;_tmpF7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpEF)->f2;_LL8: {void*a1=_tmpF6;void*a2=_tmpF7;
# 550
void*_tmpFD=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpFD;
void*_tmpFE=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpFE;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_AssnDef_and(newa1,newa2);}case 4U: _LL9: _tmpF4=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpEF)->f1;_tmpF5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpEF)->f2;_LLA: {void*a1=_tmpF4;void*a2=_tmpF5;
# 555
void*_tmpFF=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpFF;
void*_tmp100=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmp100;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_AssnDef_or(newa1,newa2);}case 6U: _LLB: _tmpF3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmpEF)->f1;_LLC: {void*a=_tmpF3;
# 562
struct Cyc_List_List*new_esc_subst=0;
return Cyc_AssnDef_reduce_assn(a,non_esc_subst,& new_esc_subst);}default: _LLD: _tmpF0=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpEF)->f1;_tmpF1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpEF)->f2;_tmpF2=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpEF)->f3;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmpF0;void*t=_tmpF1;void*a1=_tmpF2;
# 566
t=Cyc_AssnDef_reduce_term(t,non_esc_subst,esc_subst);
non_esc_subst=({struct Cyc_List_List*_tmp102=_cycalloc(sizeof(*_tmp102));({struct _tuple13*_tmp1AD=({struct _tuple13*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->f1=vd,_tmp101->f2=t;_tmp101;});_tmp102->hd=_tmp1AD;}),_tmp102->tl=non_esc_subst;_tmp102;});
a=a1;
goto LOOP;}}_LL0:;}}
# 574
void*Cyc_AssnDef_reduce(void*a){
# 576
if(Cyc_AssnDef_is_reduced(a))return a;
return Cyc_AssnDef_reduce_assn(a,0,0);}
# 581
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_tmp103=t;void*_tmp105;void*_tmp104;struct Cyc_List_List*_tmp107;enum Cyc_Absyn_Primop _tmp106;struct Cyc_Absyn_Vardecl*_tmp108;switch(*((int*)_tmp103)){case 1U: _LL1: _tmp108=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp103)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp108;
# 584
if(vd == x)return newx;
return t;}case 3U: _LL3: _tmp106=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp103)->f1;_tmp107=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp103)->f2;_LL4: {enum Cyc_Absyn_Primop p=_tmp106;struct Cyc_List_List*ts=_tmp107;
# 587
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmp109=ts;struct Cyc_List_List*ptr=_tmp109;for(0;ptr != 0;ptr=ptr->tl){
void*_tmp10A=(void*)ptr->hd;void*old_term=_tmp10A;
void*_tmp10B=Cyc_AssnDef_subst_term(old_term,x,newx);void*new_term=_tmp10B;
res=({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=new_term,_tmp10C->tl=res;_tmp10C;});
if(new_term != old_term)change=1;}}
# 595
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp1AE=p;Cyc_AssnDef_primop(_tmp1AE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res));});}case 4U: _LL5: _tmp104=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp103)->f1;_tmp105=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp103)->f2;_LL6: {void*tp=_tmp104;void*tm=_tmp105;
# 598
void*_tmp10D=Cyc_AssnDef_subst_term(tm,x,newx);void*tm2=_tmp10D;
if(tm != tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 0U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 return t;}_LL0:;}
# 607
void*Cyc_AssnDef_kill_mem_term(void*t){
struct Cyc_List_List*_tmp10E=0;struct Cyc_List_List*esc_subst=_tmp10E;
return Cyc_AssnDef_reduce_term(t,0,& esc_subst);}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 622 "assndef.cyc"
static void Cyc_AssnDef_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 626
while(1){
struct _tuple14 _tmp10F=({struct _tuple14 _tmp16F;_tmp16F.f1=a1s,_tmp16F.f2=a2s;_tmp16F;});struct _tuple14 _stmttmp9=_tmp10F;struct _tuple14 _tmp110=_stmttmp9;if(_tmp110.f1 == 0){if(_tmp110.f2 == 0){_LL1: _LL2:
 return;}else{_LL5: _LL6:
# 630
({struct Cyc_List_List*_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(a2s,*res2);*res2=_tmp1AF;});return;}}else{if(_tmp110.f2 == 0){_LL3: _LL4:
# 629
({struct Cyc_List_List*_tmp1B0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(a1s,*res1);*res1=_tmp1B0;});return;}else{_LL7: _LL8: {
# 632
int c=({void*_tmp1B1=(void*)((struct Cyc_List_List*)_check_null(a1s))->hd;Cyc_AssnDef_assncmp(_tmp1B1,(void*)((struct Cyc_List_List*)_check_null(a2s))->hd);});
if(c == 0){
({struct Cyc_List_List*_tmp1B2=({struct Cyc_List_List*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->hd=(void*)a1s->hd,_tmp111->tl=*common;_tmp111;});*common=_tmp1B2;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_tmp1B3=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->hd=(void*)a1s->hd,_tmp112->tl=*res1;_tmp112;});*res1=_tmp1B3;});
a1s=a1s->tl;}else{
# 641
({struct Cyc_List_List*_tmp1B4=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->hd=(void*)a2s->hd,_tmp113->tl=*res2;_tmp113;});*res2=_tmp1B4;});
a2s=a2s->tl;}}}}}_LL0:;}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 648
struct _tuple15 Cyc_AssnDef_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 650
struct Cyc_List_List*_tmp114=0;struct Cyc_List_List*common=_tmp114;
struct Cyc_List_List*_tmp115=0;struct Cyc_List_List*res1=_tmp115;
struct Cyc_List_List*_tmp116=0;struct Cyc_List_List*res2=_tmp116;
Cyc_AssnDef_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple15 _tmp170;({struct Cyc_List_List*_tmp1B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(common);_tmp170.f1=_tmp1B7;}),({struct Cyc_List_List*_tmp1B6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res1);_tmp170.f2=_tmp1B6;}),({struct Cyc_List_List*_tmp1B5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res2);_tmp170.f3=_tmp1B5;});_tmp170;});}struct _tuple16{struct Cyc_List_List*f1;void*f2;};
# 659
struct _tuple16 Cyc_AssnDef_factor(void*a){
void*_tmp117=a;void*_tmp119;void*_tmp118;void*_tmp11B;void*_tmp11A;switch(*((int*)_tmp117)){case 0U: _LL1: _LL2:
 return({struct _tuple16 _tmp171;_tmp171.f1=0,_tmp171.f2=(void*)& Cyc_AssnDef_true_assn;_tmp171;});case 1U: _LL3: _LL4:
 return({struct _tuple16 _tmp172;_tmp172.f1=0,_tmp172.f2=(void*)& Cyc_AssnDef_false_assn;_tmp172;});case 2U: _LL5: _LL6:
 return({struct _tuple16 _tmp173;({struct Cyc_List_List*_tmp1B8=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->hd=a,_tmp11C->tl=0;_tmp11C;});_tmp173.f1=_tmp1B8;}),_tmp173.f2=(void*)& Cyc_AssnDef_true_assn;_tmp173;});case 3U: _LL7: _tmp11A=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp117)->f1;_tmp11B=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp117)->f2;_LL8: {void*a1=_tmp11A;void*a2=_tmp11B;
# 665
struct _tuple16 _tmp11D=Cyc_AssnDef_factor(a1);struct _tuple16 _stmttmpA=_tmp11D;struct _tuple16 _tmp11E=_stmttmpA;void*_tmp120;struct Cyc_List_List*_tmp11F;_LLE: _tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;_LLF: {struct Cyc_List_List*c1s=_tmp11F;void*a1=_tmp120;
struct _tuple16 _tmp121=Cyc_AssnDef_factor(a2);struct _tuple16 _stmttmpB=_tmp121;struct _tuple16 _tmp122=_stmttmpB;void*_tmp124;struct Cyc_List_List*_tmp123;_LL11: _tmp123=_tmp122.f1;_tmp124=_tmp122.f2;_LL12: {struct Cyc_List_List*c2s=_tmp123;void*a2=_tmp124;
return({struct _tuple16 _tmp174;({struct Cyc_List_List*_tmp1BA=((struct Cyc_List_List*(*)(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_AssnDef_assncmp,c1s,c2s);_tmp174.f1=_tmp1BA;}),({void*_tmp1B9=Cyc_AssnDef_and(a1,a2);_tmp174.f2=_tmp1B9;});_tmp174;});}}}case 4U: _LL9: _tmp118=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp117)->f1;_tmp119=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp117)->f2;_LLA: {void*a1=_tmp118;void*a2=_tmp119;
# 669
struct _tuple16 _tmp125=Cyc_AssnDef_factor(a1);struct _tuple16 _stmttmpC=_tmp125;struct _tuple16 _tmp126=_stmttmpC;void*_tmp128;struct Cyc_List_List*_tmp127;_LL14: _tmp127=_tmp126.f1;_tmp128=_tmp126.f2;_LL15: {struct Cyc_List_List*c1s=_tmp127;void*a1=_tmp128;
struct _tuple16 _tmp129=Cyc_AssnDef_factor(a2);struct _tuple16 _stmttmpD=_tmp129;struct _tuple16 _tmp12A=_stmttmpD;void*_tmp12C;struct Cyc_List_List*_tmp12B;_LL17: _tmp12B=_tmp12A.f1;_tmp12C=_tmp12A.f2;_LL18: {struct Cyc_List_List*c2s=_tmp12B;void*a2=_tmp12C;
struct _tuple15 _tmp12D=Cyc_AssnDef_intersect_assns(c1s,c2s);struct _tuple15 _stmttmpE=_tmp12D;struct _tuple15 _tmp12E=_stmttmpE;struct Cyc_List_List*_tmp131;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp12F;_LL1A: _tmp12F=_tmp12E.f1;_tmp130=_tmp12E.f2;_tmp131=_tmp12E.f3;_LL1B: {struct Cyc_List_List*cs=_tmp12F;struct Cyc_List_List*c1s=_tmp130;struct Cyc_List_List*c2s=_tmp131;
return({struct _tuple16 _tmp175;_tmp175.f1=cs,({void*_tmp1BC=({void*_tmp1BB=((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_AssnDef_and,a1,c1s);Cyc_AssnDef_or(_tmp1BB,((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_AssnDef_and,a2,c2s));});_tmp175.f2=_tmp1BC;});_tmp175;});}}}}default: _LLB: _LLC:
 return({struct _tuple16 _tmp176;_tmp176.f1=0,_tmp176.f2=a;_tmp176;});}_LL0:;}
# 677
void*Cyc_AssnDef_factor_assn(void*a){
# 679
struct _tuple16 _tmp132=Cyc_AssnDef_factor(a);struct _tuple16 _stmttmpF=_tmp132;struct _tuple16 _tmp133=_stmttmpF;void*_tmp135;struct Cyc_List_List*_tmp134;_LL1: _tmp134=_tmp133.f1;_tmp135=_tmp133.f2;_LL2: {struct Cyc_List_List*cs=_tmp134;void*a=_tmp135;
a=((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_AssnDef_and,a,cs);
# 684
return a;}}struct _tuple17{enum Cyc_AssnDef_Primreln f1;enum Cyc_AssnDef_Primreln f2;};
# 691
int Cyc_AssnDef_find_ctxt(void*ctxt,void*t1,enum Cyc_AssnDef_Primreln p1,void*u1){
LOOP: {
void*_tmp136=ctxt;void*_tmp139;enum Cyc_AssnDef_Primreln _tmp138;void*_tmp137;void*_tmp13B;void*_tmp13A;void*_tmp13D;void*_tmp13C;switch(*((int*)_tmp136)){case 1U: _LL1: _LL2:
 return 1;case 0U: _LL3: _LL4:
 return 0;case 3U: _LL5: _tmp13C=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp136)->f1;_tmp13D=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp136)->f2;_LL6: {void*a1=_tmp13C;void*a2=_tmp13D;
# 697
if(Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 1;
ctxt=a2;
goto LOOP;}case 4U: _LL7: _tmp13A=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp136)->f1;_tmp13B=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp136)->f2;_LL8: {void*a1=_tmp13A;void*a2=_tmp13B;
# 701
if(!Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 0;
ctxt=a2;
goto LOOP;}case 2U: _LL9: _tmp137=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp136)->f1;_tmp138=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp136)->f2;_tmp139=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp136)->f3;_LLA: {void*t2=_tmp137;enum Cyc_AssnDef_Primreln p2=_tmp138;void*u2=_tmp139;
# 705
struct _tuple17 _tmp13E=({struct _tuple17 _tmp177;_tmp177.f1=p2,_tmp177.f2=p1;_tmp177;});struct _tuple17 _stmttmp10=_tmp13E;struct _tuple17 _tmp13F=_stmttmp10;switch(_tmp13F.f1){case Cyc_AssnDef_Neq: if(_tmp13F.f2 == Cyc_AssnDef_Neq){_LLE: _LLF:
 goto _LL11;}else{goto _LL26;}case Cyc_AssnDef_Eq: switch(_tmp13F.f2){case Cyc_AssnDef_Eq: _LL10: _LL11:
 goto _LL13;case Cyc_AssnDef_SLte: _LL12: _LL13:
 goto _LL15;case Cyc_AssnDef_ULte: _LL14: _LL15:
# 710
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);default: goto _LL26;}case Cyc_AssnDef_SLt: switch(_tmp13F.f2){case Cyc_AssnDef_Neq: _LL16: _LL17:
 goto _LL19;case Cyc_AssnDef_SLt: _LL1A: _LL1B:
# 716
 goto _LL1D;case Cyc_AssnDef_SLte: _LL1E: _LL1F:
# 718
 goto _LL21;default: goto _LL26;}case Cyc_AssnDef_ULt: switch(_tmp13F.f2){case Cyc_AssnDef_Neq: _LL18: _LL19:
# 714
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);case Cyc_AssnDef_ULt: _LL1C: _LL1D:
# 717
 goto _LL1F;case Cyc_AssnDef_ULte: _LL20: _LL21:
# 719
 goto _LL23;default: goto _LL26;}case Cyc_AssnDef_SLte: if(_tmp13F.f2 == Cyc_AssnDef_SLte){_LL22: _LL23:
 goto _LL25;}else{goto _LL26;}case Cyc_AssnDef_ULte: if(_tmp13F.f2 == Cyc_AssnDef_ULte){_LL24: _LL25:
# 722
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2);}else{goto _LL26;}default: _LL26: _LL27:
 return 0;}_LLD:;}default: _LLB: _LLC:
# 726
 ctxt=Cyc_AssnDef_reduce(ctxt);
goto LOOP;}_LL0:;}}
# 733
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
LOOP:
 if(ctxt == a)return 1;{
void*_tmp140=a;void*_tmp143;enum Cyc_AssnDef_Primreln _tmp142;void*_tmp141;void*_tmp145;void*_tmp144;void*_tmp147;void*_tmp146;switch(*((int*)_tmp140)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 return 0;case 3U: _LL5: _tmp146=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp140)->f1;_tmp147=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp140)->f2;_LL6: {void*a1=_tmp146;void*a2=_tmp147;
# 740
if(!Cyc_AssnDef_simple_prove(ctxt,a1))return 0;
a=a2;
goto LOOP;}case 4U: _LL7: _tmp144=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp140)->f1;_tmp145=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp140)->f2;_LL8: {void*a1=_tmp144;void*a2=_tmp145;
# 744
if(Cyc_AssnDef_simple_prove(ctxt,a1))return 1;
a=a2;
goto LOOP;}case 2U: _LL9: _tmp141=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp140)->f1;_tmp142=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp140)->f2;_tmp143=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp140)->f3;_LLA: {void*t1=_tmp141;enum Cyc_AssnDef_Primreln p=_tmp142;void*t2=_tmp143;
return Cyc_AssnDef_find_ctxt(ctxt,t1,p,t2);}default: _LLB: _LLC:
# 749
 a=Cyc_AssnDef_reduce(a);
goto LOOP;}_LL0:;}}

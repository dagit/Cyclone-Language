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
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 349
extern struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*,void*);
# 354
extern struct Cyc_List_List*Cyc_List_delete_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 858 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 895
extern void*Cyc_Absyn_void_type;
# 936
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1062
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1107
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1113
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);
# 1115
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 34 "cifc.h"
void Cyc_Cifc_merge_sys_user_decl(unsigned,struct Cyc_Tcenv_Tenv*,int,struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 30 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*);
# 76
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc_Hashtable_Table;
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 82 "attributes.h"
struct Cyc_List_List*Cyc_Atts_merge_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 41 "cifc.cyc"
static int Cyc_Cifc_glob_tvar_id=0;
# 44
struct _fat_ptr Cyc_Cifc_list2string(struct Cyc_List_List*l,struct _fat_ptr(*tostr)(void*)){
struct _fat_ptr _tmp0=({void*_tmp8=0U;({struct _fat_ptr _tmp175=({const char*_tmp9="(";_tag_fat(_tmp9,sizeof(char),2U);});Cyc_aprintf(_tmp175,_tag_fat(_tmp8,sizeof(void*),0));});});struct _fat_ptr ret=_tmp0;
while((unsigned)l){
ret=({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0,_tmp14B.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_tmp14B;});struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0,({struct _fat_ptr _tmp176=(struct _fat_ptr)((struct _fat_ptr)((struct _fat_ptr(*)(void*))_check_null(tostr))(l->hd));_tmp14A.f1=_tmp176;});_tmp14A;});void*_tmp1[2];_tmp1[0]=& _tmp3,_tmp1[1]=& _tmp4;({struct _fat_ptr _tmp177=({const char*_tmp2="%s %s,";_tag_fat(_tmp2,sizeof(char),7U);});Cyc_aprintf(_tmp177,_tag_fat(_tmp1,sizeof(void*),2));});});
l=l->tl;}
# 50
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0,_tmp14C.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_tmp14C;});void*_tmp5[1];_tmp5[0]=& _tmp7;({struct _fat_ptr _tmp178=({const char*_tmp6="%s)";_tag_fat(_tmp6,sizeof(char),4U);});Cyc_aprintf(_tmp178,_tag_fat(_tmp5,sizeof(void*),1));});});}
# 55
static void Cyc_Cifc_fail_merge(int warn,unsigned loc,int is_buildlib,struct _tuple0*n,struct _fat_ptr s){
# 57
if(is_buildlib){
struct _fat_ptr preamble=warn?({const char*_tmpF="Warning: user-defined";_tag_fat(_tmpF,sizeof(char),22U);}):({const char*_tmp10="User-defined";_tag_fat(_tmp10,sizeof(char),13U);});
({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0,_tmp14F.f1=(struct _fat_ptr)((struct _fat_ptr)preamble);_tmp14F;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0,({
# 61
struct _fat_ptr _tmp179=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n));_tmp14E.f1=_tmp179;});_tmp14E;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0,_tmp14D.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp14D;});void*_tmpA[3];_tmpA[0]=& _tmpC,_tmpA[1]=& _tmpD,_tmpA[2]=& _tmpE;({struct Cyc___cycFILE*_tmp17B=Cyc_stderr;struct _fat_ptr _tmp17A=({const char*_tmpB="%s type for %s incompatible with system version %s\n";_tag_fat(_tmpB,sizeof(char),52U);});Cyc_fprintf(_tmp17B,_tmp17A,_tag_fat(_tmpA,sizeof(void*),3));});});}else{
# 63
if(warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp153;_tmp153.tag=0,({struct _fat_ptr _tmp17C=({const char*_tmp17="User-defined type for ";_tag_fat(_tmp17,sizeof(char),23U);});_tmp153.f1=_tmp17C;});_tmp153;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp13=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp152;_tmp152.tag=1,_tmp152.f1=n;_tmp152;});struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp151;_tmp151.tag=0,({struct _fat_ptr _tmp17D=({const char*_tmp16=" incompatible with system version ";_tag_fat(_tmp16,sizeof(char),35U);});_tmp151.f1=_tmp17D;});_tmp151;});struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp150;_tmp150.tag=0,_tmp150.f1=s;_tmp150;});void*_tmp11[4];_tmp11[0]=& _tmp12,_tmp11[1]=& _tmp13,_tmp11[2]=& _tmp14,_tmp11[3]=& _tmp15;({unsigned _tmp17E=loc;Cyc_Warn_warn2(_tmp17E,_tag_fat(_tmp11,sizeof(void*),4));});});else{
# 66
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _tmp157;_tmp157.tag=0,({struct _fat_ptr _tmp17F=({const char*_tmp1E="User-defined type for ";_tag_fat(_tmp1E,sizeof(char),23U);});_tmp157.f1=_tmp17F;});_tmp157;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp156;_tmp156.tag=1,_tmp156.f1=n;_tmp156;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp155;_tmp155.tag=0,({struct _fat_ptr _tmp180=({const char*_tmp1D=" incompatible with system version ";_tag_fat(_tmp1D,sizeof(char),35U);});_tmp155.f1=_tmp180;});_tmp155;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp154;_tmp154.tag=0,_tmp154.f1=s;_tmp154;});void*_tmp18[4];_tmp18[0]=& _tmp19,_tmp18[1]=& _tmp1A,_tmp18[2]=& _tmp1B,_tmp18[3]=& _tmp1C;({unsigned _tmp181=loc;Cyc_Warn_err2(_tmp181,_tag_fat(_tmp18,sizeof(void*),4));});});}}}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 71
static void*Cyc_Cifc_expand_c_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp){
# 73
void*_tmp1F=ctyp;void*_tmp21;enum Cyc_Absyn_AggrKind _tmp22;void*_tmp20;switch(*((int*)_tmp1F)){case 8:
# 76
 Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
{void*_tmp23=ctyp;void*_tmp24;if(*((int*)_tmp23)== 8){_tmp24=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23)->f4;{void*to=_tmp24;
# 80
return(void*)_check_null(to);}}else{
# 82
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26=({struct Cyc_Warn_String_Warn_Warg_struct _tmp158;_tmp158.tag=0,({struct _fat_ptr _tmp182=({const char*_tmp27="Impos";_tag_fat(_tmp27,sizeof(char),6U);});_tmp158.f1=_tmp182;});_tmp158;});void*_tmp25[1];_tmp25[0]=& _tmp26;({unsigned _tmp183=loc;Cyc_Warn_err2(_tmp183,_tag_fat(_tmp25,sizeof(void*),1));});});
goto _LL19;}_LL19:;}
# 85
return ctyp;case 5: _tmp20=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_FnInfo*finfo=_tmp20;
# 87
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 89
({void*_tmp186=({unsigned _tmp185=loc;struct Cyc_Tcenv_Tenv*_tmp184=te;Cyc_Cifc_expand_c_type(_tmp185,_tmp184,Cyc_Toc_typ_to_c(finfo->ret_type));});finfo->ret_type=_tmp186;});{
struct Cyc_List_List*_tmp28=finfo->args;struct Cyc_List_List*args=_tmp28;
while((unsigned)args){
struct _tuple8*_tmp29=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp0=_tmp29;struct _tuple8*_tmp2A=_stmttmp0;void*_tmp2B;_tmp2B=(void**)& _tmp2A->f3;{void**argType=(void**)_tmp2B;
({void*_tmp189=({unsigned _tmp188=loc;struct Cyc_Tcenv_Tenv*_tmp187=te;Cyc_Cifc_expand_c_type(_tmp188,_tmp187,Cyc_Toc_typ_to_c(*argType));});*argType=_tmp189;});
args=args->tl;}}
# 97
finfo->tvars=0;
finfo->effect=0;
return ctyp;}}case 3: _tmp20=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_PtrInfo*pinfo=_tmp20;
# 103
({void*_tmp18C=({unsigned _tmp18B=loc;struct Cyc_Tcenv_Tenv*_tmp18A=te;Cyc_Cifc_expand_c_type(_tmp18B,_tmp18A,Cyc_Toc_typ_to_c(pinfo->elt_type));});pinfo->elt_type=_tmp18C;});
return ctyp;}case 4: _tmp20=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_ArrayInfo*ainfo=_tmp20;
# 106
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 108
({void*_tmp18F=({unsigned _tmp18E=loc;struct Cyc_Tcenv_Tenv*_tmp18D=te;Cyc_Cifc_expand_c_type(_tmp18E,_tmp18D,Cyc_Toc_typ_to_c(ainfo->elt_type));});ainfo->elt_type=_tmp18F;});
return ctyp;}case 6: _tmp20=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_List_List*tlst=_tmp20;
# 111
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 113
while((unsigned)tlst){
struct _tuple11*_tmp2C=(struct _tuple11*)tlst->hd;struct _tuple11*_stmttmp1=_tmp2C;struct _tuple11*_tmp2D=_stmttmp1;void*_tmp2F;struct Cyc_Absyn_Tqual _tmp2E;_tmp2E=_tmp2D->f1;_tmp2F=(void**)& _tmp2D->f2;{struct Cyc_Absyn_Tqual tq=_tmp2E;void**elt_type=(void**)_tmp2F;
({void*_tmp192=({unsigned _tmp191=loc;struct Cyc_Tcenv_Tenv*_tmp190=te;Cyc_Cifc_expand_c_type(_tmp191,_tmp190,Cyc_Toc_typ_to_c(*elt_type));});*elt_type=_tmp192;});
tlst=tlst->tl;}}
# 119
return ctyp;}case 7: _tmp22=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F)->f1;_tmp20=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F)->f2;{enum Cyc_Absyn_AggrKind knd=_tmp22;struct Cyc_List_List*flst=_tmp20;
# 122
return ctyp;}case 10: _tmp20=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F)->f1;_tmp21=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F)->f2;{struct Cyc_Absyn_TypeDecl*td=_tmp20;void**tptr=_tmp21;
# 125
return ctyp;}case 9: _tmp20=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_Exp*e=_tmp20;
# 128
return ctyp;}case 11: _tmp20=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_Exp*e=_tmp20;
# 131
return ctyp;}case 0:
# 134
 return ctyp;case 1:
# 137
 return ctyp;default:
# 140
 return ctyp;};}
# 144
static int Cyc_Cifc_is_boxed_kind(void*t){
# 146
void*_tmp30=t;void*_tmp32;void*_tmp31;switch(*((int*)_tmp30)){case 0: _tmp31=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1;_tmp32=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f2;{void*tc=_tmp31;struct Cyc_List_List*ts=_tmp32;
# 148
{void*_tmp33=tc;enum Cyc_Absyn_Size_of _tmp34;switch(*((int*)_tmp33)){case 1: _tmp34=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp33)->f2;{enum Cyc_Absyn_Size_of sz=_tmp34;
# 150
return(int)sz == (int)2U ||(int)sz == (int)Cyc_Absyn_Long_sz;}case 3:
 goto _LLF;case 4: _LLF:
# 153
 return 1;default:
# 155
 return 0;};}
# 157
goto _LL0;}case 3: _tmp31=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30)->f1;{struct Cyc_Absyn_PtrInfo*pi=_tmp31;
# 159
{void*_tmp35=(pi->ptr_atts).bounds;void*_stmttmp2=_tmp35;void*_tmp36=_stmttmp2;void*_tmp37;if(*((int*)_tmp36)== 0){_tmp37=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36)->f1;{void*tc=_tmp37;
# 161
void*_tmp38=tc;if(*((int*)_tmp38)== 14)
# 163
return 0;else{
# 165
return 1;};}}else{
# 168
({void*_tmp39=0U;({struct _fat_ptr _tmp193=({const char*_tmp3A="ptrbound_t must have an AppType";_tag_fat(_tmp3A,sizeof(char),32U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp193,_tag_fat(_tmp39,sizeof(void*),0));});});};}
# 170
goto _LL0;}case 9: _tmp31=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp30)->f1;{struct Cyc_Absyn_Exp*e=_tmp31;
# 172
return 1;}default:
# 174
 return 0;}_LL0:;}
# 182
static int Cyc_Cifc_c_types_ok(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp,void*cyctyp){
struct Cyc_Absyn_Tqual tq;
# 185
void*_tmp3B=Cyc_Tcutil_copy_type(ctyp);void*ctyp_cp=_tmp3B;
void*_tmp3C=Cyc_Tcutil_copy_type(cyctyp);void*utyp_cp=_tmp3C;
# 188
void*_tmp3D=Cyc_Toc_typ_to_c(ctyp_cp);void*c_typ=_tmp3D;
void*_tmp3E=Cyc_Toc_typ_to_c(utyp_cp);void*u_typ=_tmp3E;
if(!Cyc_Unify_unify(c_typ,u_typ)){
c_typ=Cyc_Cifc_expand_c_type(loc,te,c_typ);
u_typ=Cyc_Cifc_expand_c_type(loc,te,u_typ);
if(!Cyc_Unify_unify(c_typ,u_typ))
return Cyc_Cifc_is_boxed_kind(c_typ)&& Cyc_Cifc_is_boxed_kind(u_typ);}
# 197
return 1;}struct _tuple12{void*f1;void*f2;};struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 212 "cifc.cyc"
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,struct Cyc_Tcenv_Tenv*te,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
# 214
struct _tuple12 _tmp3F=({struct _tuple12 _tmp169;_tmp169.f1=c_decl->r,_tmp169.f2=user_decl->r;_tmp169;});struct _tuple12 _stmttmp3=_tmp3F;struct _tuple12 _tmp40=_stmttmp3;void*_tmp42;void*_tmp41;switch(*((int*)_tmp40.f1)){case 0: if(*((int*)_tmp40.f2)== 0){_tmp41=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp40.f1)->f1;_tmp42=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp40.f2)->f1;{struct Cyc_Absyn_Vardecl*cd=_tmp41;struct Cyc_Absyn_Vardecl*ud=_tmp42;
# 216
if(!Cyc_Cifc_c_types_ok(loc,te,cd->type,ud->type)){
({unsigned _tmp199=loc;int _tmp198=is_buildlib;struct _tuple0*_tmp197=cd->name;Cyc_Cifc_fail_merge(0,_tmp199,_tmp198,_tmp197,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp45=({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0,({struct _fat_ptr _tmp194=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_tmp15A.f1=_tmp194;});_tmp15A;});struct Cyc_String_pa_PrintArg_struct _tmp46=({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0,({struct _fat_ptr _tmp195=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cd->type));_tmp159.f1=_tmp195;});_tmp159;});void*_tmp43[2];_tmp43[0]=& _tmp45,_tmp43[1]=& _tmp46;({struct _fat_ptr _tmp196=({const char*_tmp44=": type %s != %s";_tag_fat(_tmp44,sizeof(char),16U);});Cyc_aprintf(_tmp196,_tag_fat(_tmp43,sizeof(void*),2));});}));});if(!0)return;}else{
# 224
if(ud->attributes != 0)
({struct Cyc_List_List*_tmp19A=Cyc_Atts_merge_attributes(cd->attributes,ud->attributes);cd->attributes=_tmp19A;});
cd->type=ud->type;
cd->tq=ud->tq;}
# 229
goto _LL0;}}else{goto _LLB;}case 1: if(*((int*)_tmp40.f2)== 0){_tmp41=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp40.f1)->f1;_tmp42=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp40.f2)->f1;{struct Cyc_Absyn_Fndecl*cd=_tmp41;struct Cyc_Absyn_Vardecl*ud=_tmp42;
# 232
if(!Cyc_Tcutil_is_function_type(ud->type)){
({unsigned _tmp19D=loc;int _tmp19C=is_buildlib;struct _tuple0*_tmp19B=ud->name;Cyc_Cifc_fail_merge(0,_tmp19D,_tmp19C,_tmp19B,({const char*_tmp47=": type must be a function type to match decl\n";_tag_fat(_tmp47,sizeof(char),46U);}));});if(!0)return;}{
# 235
void*cdtype;
if(cd->cached_type != 0)
cdtype=(void*)_check_null(cd->cached_type);else{
# 239
cdtype=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=5,_tmp48->f1=cd->i;_tmp48;});}
if(!Cyc_Cifc_c_types_ok(loc,te,cdtype,ud->type)){
({unsigned _tmp1A3=loc;int _tmp1A2=is_buildlib;struct _tuple0*_tmp1A1=ud->name;Cyc_Cifc_fail_merge(0,_tmp1A3,_tmp1A2,_tmp1A1,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4B=({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0,({struct _fat_ptr _tmp19E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cdtype));_tmp15C.f1=_tmp19E;});_tmp15C;});struct Cyc_String_pa_PrintArg_struct _tmp4C=({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0,({struct _fat_ptr _tmp19F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_tmp15B.f1=_tmp19F;});_tmp15B;});void*_tmp49[2];_tmp49[0]=& _tmp4B,_tmp49[1]=& _tmp4C;({struct _fat_ptr _tmp1A0=({const char*_tmp4A=": type %s != %s";_tag_fat(_tmp4A,sizeof(char),16U);});Cyc_aprintf(_tmp1A0,_tag_fat(_tmp49,sizeof(void*),2));});}));});if(!0)return;}else{
# 248
void*_tmp4D=ud->type;void*_stmttmp4=_tmp4D;void*_tmp4E=_stmttmp4;struct Cyc_Absyn_FnInfo _tmp4F;if(*((int*)_tmp4E)== 5){_tmp4F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4E)->f1;{struct Cyc_Absyn_FnInfo fi=_tmp4F;
# 250
struct Cyc_List_List*_tmp50=fi.tvars;struct Cyc_List_List*old_tvars=_tmp50;
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ud->type);
if((cd->i).attributes != 0)
({struct Cyc_List_List*_tmp1A4=Cyc_Atts_merge_attributes(fi.attributes,(cd->i).attributes);fi.attributes=_tmp1A4;});
cd->i=fi;
goto _LLD;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15D;_tmp15D.tag=0,({struct _fat_ptr _tmp1A5=({const char*_tmp53="oops!\n";_tag_fat(_tmp53,sizeof(char),7U);});_tmp15D.f1=_tmp1A5;});_tmp15D;});void*_tmp51[1];_tmp51[0]=& _tmp52;Cyc_Warn_err2(0U,_tag_fat(_tmp51,sizeof(void*),1));});}_LLD:;}
# 259
goto _LL0;}}}else{goto _LLB;}case 5: if(*((int*)_tmp40.f2)== 5){_tmp41=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp40.f1)->f1;_tmp42=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp40.f2)->f1;{struct Cyc_Absyn_Aggrdecl*cd=_tmp41;struct Cyc_Absyn_Aggrdecl*ud=_tmp42;
# 267
if((int)ud->sc != (int)cd->sc){
({unsigned _tmp1A8=loc;int _tmp1A7=is_buildlib;struct _tuple0*_tmp1A6=cd->name;Cyc_Cifc_fail_merge(1,_tmp1A8,_tmp1A7,_tmp1A6,({const char*_tmp54=": scopes don't match (ignoring)";_tag_fat(_tmp54,sizeof(char),32U);}));});if(!1)return;}
# 270
if(ud->impl == 0){
({unsigned _tmp1AB=loc;int _tmp1AA=is_buildlib;struct _tuple0*_tmp1A9=cd->name;Cyc_Cifc_fail_merge(0,_tmp1AB,_tmp1AA,_tmp1A9,({const char*_tmp55=": no user definition";_tag_fat(_tmp55,sizeof(char),21U);}));});if(!0)return;}
if(cd->impl == 0){
({unsigned _tmp1AE=loc;int _tmp1AD=is_buildlib;struct _tuple0*_tmp1AC=cd->name;Cyc_Cifc_fail_merge(1,_tmp1AE,_tmp1AD,_tmp1AC,({const char*_tmp56=": no definition for system version";_tag_fat(_tmp56,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 277
{struct _tuple13 _tmp57=({struct _tuple13 _tmp163;_tmp163.f1=cd->impl,_tmp163.f2=ud->impl;_tmp163;});struct _tuple13 _stmttmp5=_tmp57;struct _tuple13 _tmp58=_stmttmp5;int _tmp5F;void*_tmp5E;void*_tmp5D;void*_tmp5C;void*_tmp5B;void*_tmp5A;void*_tmp59;if(_tmp58.f1 != 0){if(_tmp58.f2 != 0){_tmp59=(struct Cyc_List_List**)&(_tmp58.f1)->exist_vars;_tmp5A=(struct Cyc_List_List**)&(_tmp58.f1)->rgn_po;_tmp5B=(_tmp58.f1)->fields;_tmp5C=(_tmp58.f2)->exist_vars;_tmp5D=(_tmp58.f2)->rgn_po;_tmp5E=(_tmp58.f2)->fields;_tmp5F=(_tmp58.f2)->tagged;{struct Cyc_List_List**tvarsC=(struct Cyc_List_List**)_tmp59;struct Cyc_List_List**rgnpoC=(struct Cyc_List_List**)_tmp5A;struct Cyc_List_List*cfields=_tmp5B;struct Cyc_List_List*tvars=_tmp5C;struct Cyc_List_List*rgnpo=_tmp5D;struct Cyc_List_List*ufields=_tmp5E;int tagged=_tmp5F;
# 280
if(tagged){
({unsigned _tmp1B1=loc;int _tmp1B0=is_buildlib;struct _tuple0*_tmp1AF=cd->name;Cyc_Cifc_fail_merge(0,_tmp1B1,_tmp1B0,_tmp1AF,({const char*_tmp60=": user @tagged annotation not allowed (ignoring)";_tag_fat(_tmp60,sizeof(char),49U);}));});if(!0)return;}{
# 283
struct Cyc_List_List*_tmp61=cfields;struct Cyc_List_List*x=_tmp61;
# 285
while(x != 0){
struct Cyc_Absyn_Aggrfield*_tmp62=(struct Cyc_Absyn_Aggrfield*)x->hd;struct Cyc_Absyn_Aggrfield*cfield=_tmp62;
struct Cyc_Absyn_Aggrfield*_tmp63=Cyc_Absyn_lookup_field(ufields,cfield->name);struct Cyc_Absyn_Aggrfield*ufield=_tmp63;
# 289
if(ufield != 0){
# 292
if(!Cyc_Cifc_c_types_ok(loc,te,cfield->type,ufield->type)){
({unsigned _tmp1B7=loc;int _tmp1B6=is_buildlib;struct _tuple0*_tmp1B5=cd->name;Cyc_Cifc_fail_merge(0,_tmp1B7,_tmp1B6,_tmp1B5,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0,({struct _fat_ptr _tmp1B2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_tmp160.f1=_tmp1B2;});_tmp160;});struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0,_tmp15F.f1=(struct _fat_ptr)((struct _fat_ptr)*cfield->name);_tmp15F;});struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0,({struct _fat_ptr _tmp1B3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cfield->type));_tmp15E.f1=_tmp1B3;});_tmp15E;});void*_tmp64[3];_tmp64[0]=& _tmp66,_tmp64[1]=& _tmp67,_tmp64[2]=& _tmp68;({struct _fat_ptr _tmp1B4=({const char*_tmp65=": type %s of user definition of field %s != %s";_tag_fat(_tmp65,sizeof(char),47U);});Cyc_aprintf(_tmp1B4,_tag_fat(_tmp64,sizeof(void*),3));});}));});if(!0)return;}else{
# 301
if(ufield->width != 0){
({unsigned _tmp1BC=loc;int _tmp1BB=is_buildlib;struct _tuple0*_tmp1BA=cd->name;Cyc_Cifc_fail_merge(1,_tmp1BC,_tmp1BB,_tmp1BA,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0,({struct _fat_ptr _tmp1B8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_tmp161.f1=_tmp1B8;});_tmp161;});void*_tmp69[1];_tmp69[0]=& _tmp6B;({struct _fat_ptr _tmp1B9=({const char*_tmp6A=": ignoring width of user definition of field %s";_tag_fat(_tmp6A,sizeof(char),48U);});Cyc_aprintf(_tmp1B9,_tag_fat(_tmp69,sizeof(void*),1));});}));});if(!1)return;}
# 306
if(ufield->attributes != 0)
({struct Cyc_List_List*_tmp1BD=Cyc_Atts_merge_attributes(cfield->attributes,ufield->attributes);cfield->attributes=_tmp1BD;});
# 310
cfield->type=ufield->type;
cfield->tq=ufield->tq;
cfield->requires_clause=ufield->requires_clause;}}
# 315
x=x->tl;}
# 319
if(ud->tvs != 0)cd->tvs=ud->tvs;
if((unsigned)tvars)*tvarsC=tvars;
if((unsigned)rgnpo)*rgnpoC=rgnpo;
# 324
x=ufields;{
int missing_fields=0;
while(x != 0){
struct Cyc_Absyn_Aggrfield*_tmp6C=Cyc_Absyn_lookup_field(cfields,((struct Cyc_Absyn_Aggrfield*)x->hd)->name);struct Cyc_Absyn_Aggrfield*cfield=_tmp6C;
if(cfield == 0){
missing_fields=1;
({unsigned _tmp1C1=loc;int _tmp1C0=is_buildlib;struct _tuple0*_tmp1BF=cd->name;Cyc_Cifc_fail_merge(1,_tmp1C1,_tmp1C0,_tmp1BF,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0,_tmp162.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)x->hd)->name);_tmp162;});void*_tmp6D[1];_tmp6D[0]=& _tmp6F;({struct _fat_ptr _tmp1BE=({const char*_tmp6E=": no definition of field %s in system version";_tag_fat(_tmp6E,sizeof(char),46U);});Cyc_aprintf(_tmp1BE,_tag_fat(_tmp6D,sizeof(void*),1));});}));});if(!1)return;}
# 334
x=x->tl;}
# 336
goto _LL12;}}}}else{goto _LL15;}}else{_LL15:
# 338
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp1C2=({const char*_tmp70="Internal Error: NULL cases not possible";_tag_fat(_tmp70,sizeof(char),40U);});_tmp71->f1=_tmp1C2;});_tmp71;}));}_LL12:;}
# 340
goto _LL0;}}else{goto _LLB;}case 7: if(*((int*)_tmp40.f2)== 7){_tmp41=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp40.f1)->f1;_tmp42=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp40.f2)->f1;{struct Cyc_Absyn_Enumdecl*cd=_tmp41;struct Cyc_Absyn_Enumdecl*ud=_tmp42;
# 343
({unsigned _tmp1C5=loc;int _tmp1C4=is_buildlib;struct _tuple0*_tmp1C3=cd->name;Cyc_Cifc_fail_merge(0,_tmp1C5,_tmp1C4,_tmp1C3,({const char*_tmp72=": enum merging not currently supported";_tag_fat(_tmp72,sizeof(char),39U);}));});if(!0)return;}}else{goto _LLB;}case 8: if(*((int*)_tmp40.f2)== 8){_tmp41=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp40.f1)->f1;_tmp42=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp40.f2)->f1;{struct Cyc_Absyn_Typedefdecl*cd=_tmp41;struct Cyc_Absyn_Typedefdecl*ud=_tmp42;
# 346
if(ud->defn == 0){
({unsigned _tmp1C8=loc;int _tmp1C7=is_buildlib;struct _tuple0*_tmp1C6=cd->name;Cyc_Cifc_fail_merge(0,_tmp1C8,_tmp1C7,_tmp1C6,({const char*_tmp73=": no user definition";_tag_fat(_tmp73,sizeof(char),21U);}));});if(!0)return;}
if(cd->defn == 0){
({unsigned _tmp1CB=loc;int _tmp1CA=is_buildlib;struct _tuple0*_tmp1C9=cd->name;Cyc_Cifc_fail_merge(1,_tmp1CB,_tmp1CA,_tmp1C9,({const char*_tmp74=": no definition for system version";_tag_fat(_tmp74,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 354
if(!({unsigned _tmp1CE=loc;struct Cyc_Tcenv_Tenv*_tmp1CD=te;void*_tmp1CC=(void*)_check_null(cd->defn);Cyc_Cifc_c_types_ok(_tmp1CE,_tmp1CD,_tmp1CC,(void*)_check_null(ud->defn));})){
({unsigned _tmp1D4=loc;int _tmp1D3=is_buildlib;struct _tuple0*_tmp1D2=cd->name;Cyc_Cifc_fail_merge(0,_tmp1D4,_tmp1D3,_tmp1D2,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp77=({struct Cyc_String_pa_PrintArg_struct _tmp165;_tmp165.tag=0,({struct _fat_ptr _tmp1CF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(ud->defn)));_tmp165.f1=_tmp1CF;});_tmp165;});struct Cyc_String_pa_PrintArg_struct _tmp78=({struct Cyc_String_pa_PrintArg_struct _tmp164;_tmp164.tag=0,({struct _fat_ptr _tmp1D0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(cd->defn)));_tmp164.f1=_tmp1D0;});_tmp164;});void*_tmp75[2];_tmp75[0]=& _tmp77,_tmp75[1]=& _tmp78;({struct _fat_ptr _tmp1D1=({const char*_tmp76=": type definition %s of user definition != %s";_tag_fat(_tmp76,sizeof(char),46U);});Cyc_aprintf(_tmp1D1,_tag_fat(_tmp75,sizeof(void*),2));});}));});if(!0)return;}else{
# 361
cd->tvs=ud->tvs;
cd->defn=ud->defn;
cd->tq=ud->tq;
if(ud->atts != 0)
({struct Cyc_List_List*_tmp1D5=Cyc_Atts_merge_attributes(cd->atts,ud->atts);cd->atts=_tmp1D5;});}
# 368
goto _LL0;}}else{goto _LLB;}default: _LLB:
# 371
 if(is_buildlib)
({struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp166;_tmp166.tag=0,({
struct _fat_ptr _tmp1D6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp7C[1];_tmp7C[0]=user_decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp7C,sizeof(struct Cyc_Absyn_Decl*),1));})));_tmp166.f1=_tmp1D6;});_tmp166;});void*_tmp79[1];_tmp79[0]=& _tmp7B;({struct Cyc___cycFILE*_tmp1D8=Cyc_stderr;struct _fat_ptr _tmp1D7=({const char*_tmp7A="Error in .cys file: bad (or unsupported) user-defined type %s\n";_tag_fat(_tmp7A,sizeof(char),63U);});Cyc_fprintf(_tmp1D8,_tmp1D7,_tag_fat(_tmp79,sizeof(void*),1));});});else{
# 375
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp168;_tmp168.tag=0,({struct _fat_ptr _tmp1D9=({const char*_tmp81="bad (or unsupported) user-defined type %s";_tag_fat(_tmp81,sizeof(char),42U);});_tmp168.f1=_tmp1D9;});_tmp168;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp167;_tmp167.tag=0,({
struct _fat_ptr _tmp1DA=Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp80[1];_tmp80[0]=user_decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp80,sizeof(struct Cyc_Absyn_Decl*),1));}));_tmp167.f1=_tmp1DA;});_tmp167;});void*_tmp7D[2];_tmp7D[0]=& _tmp7E,_tmp7D[1]=& _tmp7F;({unsigned _tmp1DB=loc;Cyc_Warn_err2(_tmp1DB,_tag_fat(_tmp7D,sizeof(void*),2));});});}
return;}_LL0:;}
# 381
static int Cyc_Cifc_contains_type_vars(struct Cyc_Absyn_Decl*ud){
{void*_tmp82=ud->r;void*_stmttmp6=_tmp82;void*_tmp83=_stmttmp6;void*_tmp84;if(*((int*)_tmp83)== 5){_tmp84=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp83)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp84;
# 384
return ad->tvs != 0;}}else{
# 386
goto _LL0;}_LL0:;}
# 388
return 0;}
# 391
static struct Cyc_Absyn_Decl*Cyc_Cifc_make_abstract_decl(struct Cyc_Absyn_Decl*ud){
void*_tmp85=ud->r;void*_stmttmp7=_tmp85;void*_tmp86=_stmttmp7;void*_tmp87;if(*((int*)_tmp86)== 5){_tmp87=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp86)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp87;
# 394
struct Cyc_Absyn_Aggrdecl*_tmp88=({struct Cyc_Absyn_Aggrdecl*_tmp8B=_cycalloc(sizeof(*_tmp8B));*_tmp8B=*ad;_tmp8B;});struct Cyc_Absyn_Aggrdecl*absad=_tmp88;
absad->impl=0;{
struct Cyc_Absyn_Decl*_tmp89=({void*_tmp1DC=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=5,_tmp8A->f1=absad;_tmp8A;});Cyc_Absyn_new_decl(_tmp1DC,ud->loc);});struct Cyc_Absyn_Decl*nd=_tmp89;
return nd;}}}else{
# 399
({void*_tmp8C=0U;({struct _fat_ptr _tmp1DD=({const char*_tmp8D="Only aggrdecls";_tag_fat(_tmp8D,sizeof(char),15U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp1DD,_tag_fat(_tmp8C,sizeof(void*),0));});});};}
# 404
static int Cyc_Cifc_kindbound_subsumed(void*kb1,void*kb2){
void*_tmp8E=kb1;void*_tmp90;void*_tmp8F;switch(*((int*)_tmp8E)){case 0: _tmp8F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8E)->f1;{struct Cyc_Absyn_Kind*k1=_tmp8F;
# 407
void*_tmp91=kb2;void*_tmp92;if(*((int*)_tmp91)== 0){_tmp92=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp91)->f1;{struct Cyc_Absyn_Kind*k2=_tmp92;
# 409
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{
# 411
return 0;};}case 1: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8E)->f1 != 0){_tmp8F=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8E)->f1)->v;{void*kbb1=_tmp8F;
# 414
void*_tmp93=kb2;void*_tmp94;if(*((int*)_tmp93)== 1){if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp93)->f1 != 0){_tmp94=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp93)->f1)->v;{void*kbb2=_tmp94;
# 416
return Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL13;}}else{_LL13:
# 418
 return 1;};}}else{
# 421
return 1;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8E)->f1 != 0){_tmp8F=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8E)->f1)->v;_tmp90=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8E)->f2;{void*kbb1=_tmp8F;struct Cyc_Absyn_Kind*k1=_tmp90;
# 423
void*_tmp95=kb2;void*_tmp97;void*_tmp96;if(*((int*)_tmp95)== 2){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp95)->f1 != 0){_tmp96=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp95)->f1)->v;_tmp97=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp95)->f2;{void*kbb2=_tmp96;struct Cyc_Absyn_Kind*k2=_tmp97;
# 425
return((int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual)&& Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL18;}}else{_LL18:
# 427
 return 0;};}}else{_tmp8F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8E)->f2;{struct Cyc_Absyn_Kind*k1=_tmp8F;
# 430
void*_tmp98=kb2;void*_tmp99;if(*((int*)_tmp98)== 2){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98)->f1 == 0){_tmp99=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98)->f2;{struct Cyc_Absyn_Kind*k2=_tmp99;
# 432
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{goto _LL1D;}}else{_LL1D:
# 434
 return 0;};}}};}
# 439
static int Cyc_Cifc_find_and_remove(struct Cyc_List_List**lst,void*kind){
struct Cyc_List_List*_tmp9A=*lst;struct Cyc_List_List*cur=_tmp9A;
struct Cyc_List_List*prev=0;
while((unsigned)cur){
void*_tmp9B=(void*)cur->hd;void*t=_tmp9B;
{void*_tmp9C=t;void*_tmp9D;if(*((int*)_tmp9C)== 2){_tmp9D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9C)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp9D;
# 446
if(Cyc_Cifc_kindbound_subsumed(tv->kind,kind)){
if((unsigned)prev)
prev->tl=cur->tl;else{
# 450
*lst=cur->tl;}
cur->tl=0;
return 1;}
# 454
goto _LL0;}}else{
# 456
({struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp16B;_tmp16B.tag=0,({struct _fat_ptr _tmp1DE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp16B.f1=_tmp1DE;});_tmp16B;});struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp16A;_tmp16A.tag=0,({struct _fat_ptr _tmp1DF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_get_type_kind(t));_tmp16A.f1=_tmp1DF;});_tmp16A;});void*_tmp9E[2];_tmp9E[0]=& _tmpA0,_tmp9E[1]=& _tmpA1;({struct _fat_ptr _tmp1E0=({const char*_tmp9F="expects a VarType list only -- got %s(%s)";_tag_fat(_tmp9F,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp1E0,_tag_fat(_tmp9E,sizeof(void*),2));});});}_LL0:;}
# 458
prev=cur;
cur=cur->tl;}
# 461
return 0;}
# 464
static struct Cyc_List_List*Cyc_Cifc_get_tvar_difference(struct Cyc_List_List*tvs,struct Cyc_List_List*remove){
struct Cyc_List_List*ret=0;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmpA2=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*can=_tmpA2;
if(!Cyc_Cifc_find_and_remove(& remove,can->kind))
ret=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=can,_tmpA3->tl=ret;_tmpA3;});
tvs=tvs->tl;}
# 472
return ret;}char Cyc_Cifc_Contains_nontvar[17U]="Contains_nontvar";struct Cyc_Cifc_Contains_nontvar_exn_struct{char*tag;};
# 477
struct Cyc_Cifc_Contains_nontvar_exn_struct Cyc_Cifc_Contains_nontvar_val={Cyc_Cifc_Contains_nontvar};
# 479
static struct Cyc_List_List*Cyc_Cifc_extract_tvars(struct Cyc_List_List*ts){
struct Cyc_List_List*res=0;
while((unsigned)ts){
void*_tmpA4=(void*)ts->hd;void*t=_tmpA4;
{void*_tmpA5=t;void*_tmpA6;if(*((int*)_tmpA5)== 2){_tmpA6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA5)->f1;{struct Cyc_Absyn_Tvar*tv=_tmpA6;
# 485
res=({struct Cyc_List_List*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->hd=tv,_tmpA7->tl=res;_tmpA7;});
goto _LL0;}}else{
# 488
(int)_throw((void*)& Cyc_Cifc_Contains_nontvar_val);}_LL0:;}
# 490
ts=ts->tl;}
# 492
return res;}
# 495
static void*Cyc_Cifc_instantiate_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
{void*_tmpA8=tv->kind;void*_stmttmp8=_tmpA8;void*_tmpA9=_stmttmp8;void*_tmpAA;switch(*((int*)_tmpA9)){case 0: _tmpAA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA9)->f1;{struct Cyc_Absyn_Kind*k=_tmpAA;
_tmpAA=k;goto _LL4;}case 2: _tmpAA=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA9)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmpAA;
# 499
if((int)k->kind == (int)Cyc_Absyn_BoxKind ||(int)k->kind == (int)Cyc_Absyn_AnyKind)
return({void*_tmp1E1=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_tmp1E1,Cyc_Absyn_empty_tqual(loc));});
goto _LL0;}default:
# 503
 return({void*_tmp1E2=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_tmp1E2,Cyc_Absyn_empty_tqual(loc));});}_LL0:;}
# 505
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16D;_tmp16D.tag=0,({struct _fat_ptr _tmp1E3=({const char*_tmpAE="Unable to instantiate tvar %s";_tag_fat(_tmpAE,sizeof(char),30U);});_tmp16D.f1=_tmp1E3;});_tmp16D;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16C;_tmp16C.tag=0,({struct _fat_ptr _tmp1E4=Cyc_Absynpp_tvar2string(tv);_tmp16C.f1=_tmp1E4;});_tmp16C;});void*_tmpAB[2];_tmpAB[0]=& _tmpAC,_tmpAB[1]=& _tmpAD;({unsigned _tmp1E5=loc;Cyc_Warn_err2(_tmp1E5,_tag_fat(_tmpAB,sizeof(void*),2));});});
return Cyc_Absyn_void_type;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 509
static struct _tuple14*Cyc_Cifc_update_tvars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,void*t,struct Cyc_Absyn_Decl*enclosing_decl,int instantiate){
# 513
{void*_tmpAF=t;struct Cyc_Absyn_FnInfo _tmpB0;void*_tmpB2;void*_tmpB1;switch(*((int*)_tmpAF)){case 0: _tmpB1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAF)->f1;_tmpB2=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAF)->f2;{void*tc=_tmpB1;struct Cyc_List_List**ts=(struct Cyc_List_List**)_tmpB2;
# 515
{void*_tmpB3=tc;union Cyc_Absyn_AggrInfo _tmpB4;if(*((int*)_tmpB3)== 20){_tmpB4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpB3)->f1;{union Cyc_Absyn_AggrInfo ai=_tmpB4;
# 517
union Cyc_Absyn_AggrInfo _tmpB5=ai;struct _tuple2 _tmpB6;void*_tmpB7;if((_tmpB5.KnownAggr).tag == 2){_tmpB7=*(_tmpB5.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmpB7;
# 519
struct _tuple0*_tmpB8=ad->name;struct _tuple0*_stmttmp9=_tmpB8;struct _tuple0*_tmpB9=_stmttmp9;void*_tmpBB;union Cyc_Absyn_Nmspace _tmpBA;_tmpBA=_tmpB9->f1;_tmpBB=_tmpB9->f2;{union Cyc_Absyn_Nmspace ns=_tmpBA;struct _fat_ptr*name=_tmpBB;
# 522
struct Cyc_Absyn_Decl*_tmpBC=Cyc_Absyn_lookup_decl(tv_ovrs,name);struct Cyc_Absyn_Decl*ovd=_tmpBC;
if((unsigned)ovd){
# 526
void*_tmpBD=ovd->r;void*_stmttmpA=_tmpBD;void*_tmpBE=_stmttmpA;void*_tmpBF;if(*((int*)_tmpBE)== 5){_tmpBF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpBE)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpBF;
# 528
struct Cyc_List_List*removed_tvars=0;
{struct _handler_cons _tmpC0;_push_handler(& _tmpC0);{int _tmpC2=0;if(setjmp(_tmpC0.handler))_tmpC2=1;if(!_tmpC2){
removed_tvars=Cyc_Cifc_extract_tvars(*ts);;_pop_handler();}else{void*_tmpC1=(void*)Cyc_Core_get_exn_thrown();void*_tmpC3=_tmpC1;void*_tmpC4;if(((struct Cyc_Cifc_Contains_nontvar_exn_struct*)_tmpC3)->tag == Cyc_Cifc_Contains_nontvar)
# 534
return 0;else{_tmpC4=_tmpC3;{void*exn=_tmpC4;(int)_rethrow(exn);}};}}}
# 536
*ts=0;{
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*_tmpC5=ad->tvs;struct Cyc_List_List*tvs=_tmpC5;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmpC6=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmpC6;
if(enclosing_decl == ovd)
({struct Cyc_List_List*_tmp1E7=({struct Cyc_List_List*_tmpC8=_cycalloc(sizeof(*_tmpC8));({void*_tmp1E6=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->tag=2,_tmpC7->f1=tv;_tmpC7;});_tmpC8->hd=_tmp1E6;}),_tmpC8->tl=*ts;_tmpC8;});*ts=_tmp1E7;});else{
# 545
if(instantiate)
({struct Cyc_List_List*_tmp1E9=({struct Cyc_List_List*_tmpC9=_cycalloc(sizeof(*_tmpC9));({void*_tmp1E8=Cyc_Cifc_instantiate_tvar(enclosing_decl->loc,tv);_tmpC9->hd=_tmp1E8;}),_tmpC9->tl=*ts;_tmpC9;});*ts=_tmp1E9;});else{
# 549
struct Cyc_Absyn_Tvar*_tmpCA=({struct Cyc_Absyn_Tvar*_tmpD3=_cycalloc(sizeof(*_tmpD3));*_tmpD3=*tv;_tmpD3;});struct Cyc_Absyn_Tvar*tvcpy=_tmpCA;
struct _fat_ptr*_tmpCB=({struct _fat_ptr*_tmpD2=_cycalloc(sizeof(*_tmpD2));({struct _fat_ptr _tmp1EB=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD1=({struct Cyc_Int_pa_PrintArg_struct _tmp16E;_tmp16E.tag=1,_tmp16E.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_tmp16E;});void*_tmpCF[1];_tmpCF[0]=& _tmpD1;({struct _fat_ptr _tmp1EA=({const char*_tmpD0="`ovr_%d";_tag_fat(_tmpD0,sizeof(char),8U);});Cyc_aprintf(_tmp1EA,_tag_fat(_tmpCF,sizeof(void*),1));});});*_tmpD2=_tmp1EB;});_tmpD2;});struct _fat_ptr*tvn=_tmpCB;
tvcpy->name=tvn;
added_tvars=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=tvcpy,_tmpCC->tl=added_tvars;_tmpCC;});
({struct Cyc_List_List*_tmp1ED=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));({void*_tmp1EC=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->tag=2,_tmpCD->f1=tvcpy;_tmpCD;});_tmpCE->hd=_tmp1EC;}),_tmpCE->tl=*ts;_tmpCE;});*ts=_tmp1ED;});}}
# 556
tvs=tvs->tl;}
# 558
({struct Cyc_List_List*_tmp1EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(*ts);*ts=_tmp1EE;});
return enclosing_decl == ovd?0:({struct _tuple14*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=added_tvars,_tmpD4->f2=removed_tvars;_tmpD4;});}}}else{
# 561
({void*_tmpD5=0U;({struct _fat_ptr _tmp1EF=({const char*_tmpD6="ovd must be an aggr type";_tag_fat(_tmpD6,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp1EF,_tag_fat(_tmpD5,sizeof(void*),0));});});};}else{
# 565
return 0;}}}}else{_tmpB6=(_tmpB5.UnknownAggr).val;{struct _tuple2 ua=_tmpB6;
# 567
return 0;}};}}else{
# 570
goto _LL9;}_LL9:;}
# 572
return 0;}case 3: _tmpB1=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAF)->f1;{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_tmpB1;
# 574
return Cyc_Cifc_update_tvars(te,tv_ovrs,pi->elt_type,enclosing_decl,instantiate);}case 5: _tmpB0=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAF)->f1;{struct Cyc_Absyn_FnInfo fi=_tmpB0;
# 576
struct Cyc_List_List*_tmpD7=0;struct Cyc_List_List*added_tvars=_tmpD7;
struct Cyc_List_List*_tmpD8=0;struct Cyc_List_List*removed_tvars=_tmpD8;
struct Cyc_List_List*_tmpD9=fi.args;struct Cyc_List_List*argit=_tmpD9;
while((unsigned)argit){
struct _tuple8*_tmpDA=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmpB=_tmpDA;struct _tuple8*_tmpDB=_stmttmpB;void*_tmpDC;_tmpDC=_tmpDB->f3;{void*at=_tmpDC;
struct _tuple14*_tmpDD=Cyc_Cifc_update_tvars(te,tv_ovrs,at,enclosing_decl,instantiate);struct _tuple14*ar_tup=_tmpDD;
if((unsigned)ar_tup){
if((unsigned)(*ar_tup).f1)
added_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)((*ar_tup).f1,added_tvars);
if((unsigned)(*ar_tup).f2)
removed_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)((*ar_tup).f2,removed_tvars);}
# 588
argit=argit->tl;}}
# 590
if((unsigned)added_tvars ||(unsigned)removed_tvars)
return({struct _tuple14*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->f1=added_tvars,_tmpDE->f2=removed_tvars;_tmpDE;});
return 0;}default:
# 594
 return 0;};}
# 596
return 0;}
# 599
static int Cyc_Cifc_update_fninfo_usage(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,struct Cyc_Absyn_FnInfo*fi,struct Cyc_Absyn_Decl*enclosing_decl){
struct _tuple14*_tmpDF=Cyc_Cifc_update_tvars(te,tv_ovrs,((struct Cyc_Absyn_FnInfo*)_check_null(fi))->ret_type,enclosing_decl,0);struct _tuple14*ad=_tmpDF;
# 602
struct Cyc_List_List*_tmpE0=fi->args;struct Cyc_List_List*argit=_tmpE0;
struct Cyc_List_List*added_tvars=(unsigned)ad?(*ad).f1: 0;
struct Cyc_List_List*removed_tvars=(unsigned)ad?(*ad).f2: 0;
int change=0;
while((unsigned)argit){
struct _tuple8*_tmpE1=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmpC=_tmpE1;struct _tuple8*_tmpE2=_stmttmpC;void*_tmpE3;_tmpE3=(void**)& _tmpE2->f3;{void**at=(void**)_tmpE3;
struct _tuple14*_tmpE4=Cyc_Cifc_update_tvars(te,tv_ovrs,*at,enclosing_decl,0);struct _tuple14*ad=_tmpE4;
if((unsigned)ad){
added_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)((*ad).f1,added_tvars);
removed_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)((*ad).f2,removed_tvars);}
# 613
argit=argit->tl;}}
# 615
while((unsigned)removed_tvars){
change=1;{
struct Cyc_Absyn_Tvar*_tmpE5=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;struct Cyc_Absyn_Tvar*rtv=_tmpE5;
{struct _handler_cons _tmpE6;_push_handler(& _tmpE6);{int _tmpE8=0;if(setjmp(_tmpE6.handler))_tmpE8=1;if(!_tmpE8){
({struct Cyc_List_List*_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete)(fi->tvars,rtv);fi->tvars=_tmp1F0;});;_pop_handler();}else{void*_tmpE7=(void*)Cyc_Core_get_exn_thrown();void*_tmpE9=_tmpE7;void*_tmpEA;if(((struct Cyc_Core_Not_found_exn_struct*)_tmpE9)->tag == Cyc_Core_Not_found){
# 623
({struct Cyc_String_pa_PrintArg_struct _tmpED=({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0,({struct _fat_ptr _tmp1F1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_tmp16F.f1=_tmp1F1;});_tmp16F;});void*_tmpEB[1];_tmpEB[0]=& _tmpED;({unsigned _tmp1F3=loc;struct _fat_ptr _tmp1F2=({const char*_tmpEC="Removed tvar %s not found";_tag_fat(_tmpEC,sizeof(char),26U);});Cyc_Warn_warn(_tmp1F3,_tmp1F2,_tag_fat(_tmpEB,sizeof(void*),1));});});
goto _LL3;}else{_tmpEA=_tmpE9;{void*exn=_tmpEA;(int)_rethrow(exn);}}_LL3:;}}}
# 626
removed_tvars=removed_tvars->tl;}}
# 628
if((unsigned)added_tvars){
change=1;
({struct Cyc_List_List*_tmp1F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(added_tvars,fi->tvars);fi->tvars=_tmp1F4;});}
# 633
fi->effect=0;
return change;}
# 637
static struct Cyc_List_List*Cyc_Cifc_remove_cur(struct Cyc_List_List*head,struct Cyc_List_List*cur,struct Cyc_List_List*prev){
if((unsigned)prev)
prev->tl=((struct Cyc_List_List*)_check_null(cur))->tl;else{
# 641
head=((struct Cyc_List_List*)_check_null(cur))->tl;}
# 643
return head;}
# 646
static int Cyc_Cifc_decl_name_cmp(struct Cyc_Absyn_Decl*a,struct Cyc_Absyn_Decl*b){
struct _fat_ptr*_tmpEE=Cyc_Absyn_decl_name(a);struct _fat_ptr*na=_tmpEE;
struct _fat_ptr*_tmpEF=Cyc_Absyn_decl_name(b);struct _fat_ptr*nb=_tmpEF;
if(na == 0 ^ nb == 0)
return 1;
if((unsigned)na)
return Cyc_strcmp((struct _fat_ptr)*na,(struct _fat_ptr)*((struct _fat_ptr*)_check_null(nb)));
return 1;}
# 659
static struct Cyc_List_List*Cyc_Cifc_update_usages(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*in_tv_ovrs,struct Cyc_List_List*dsmm){
unsigned niter=0U;
struct Cyc_List_List*abs_decls=0;
int some_change=0;
struct Cyc_List_List*tv_ovrs=in_tv_ovrs;
struct Cyc_List_List*_tmpF0=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_copy)(dsmm);struct Cyc_List_List*dsm=_tmpF0;
struct Cyc_List_List*changed_decls=0;
do{
changed_decls=0;{
struct Cyc_List_List*_tmpF1=dsm;struct Cyc_List_List*ds=_tmpF1;
struct Cyc_List_List*_tmpF2=0;struct Cyc_List_List*prev=_tmpF2;
some_change=0;
while((unsigned)ds){
int changed=0;
struct Cyc_Absyn_Decl*_tmpF3=(struct Cyc_Absyn_Decl*)ds->hd;struct Cyc_Absyn_Decl*d=_tmpF3;
struct _fat_ptr*_tmpF4=Cyc_Absyn_decl_name(d);struct _fat_ptr*dname=_tmpF4;
# 677
{void*_tmpF5=d->r;void*_stmttmpD=_tmpF5;void*_tmpF6=_stmttmpD;void*_tmpF7;switch(*((int*)_tmpF6)){case 0: _tmpF7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpF6)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpF7;
# 679
{void*_tmpF8=vd->type;void*_stmttmpE=_tmpF8;void*_tmpF9=_stmttmpE;void*_tmpFB;void*_tmpFA;switch(*((int*)_tmpF9)){case 5: _tmpFA=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF9)->f1;{struct Cyc_Absyn_FnInfo*fi=_tmpFA;
# 681
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,fi,d);
# 687
goto _LLB;}case 0: _tmpFA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f1;_tmpFB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f2;{void*tc=_tmpFA;struct Cyc_List_List*ts=_tmpFB;
# 689
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,vd->type,d,1)!= 0;
goto _LLB;}case 3: _tmpFA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF9)->f1).elt_type;{void*et=_tmpFA;
_tmpFA=et;goto _LL13;}case 4: _tmpFA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF9)->f1).elt_type;_LL13: {void*et=_tmpFA;
# 693
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,et,d,1)!= 0;
goto _LLB;}default:
# 696
 goto _LLB;}_LLB:;}
# 698
goto _LL0;}case 1: _tmpF7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpF6)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpF7;
# 700
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,& fd->i,d);
# 704
goto _LL0;}case 5: _tmpF7=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpF6)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpF7;
# 706
if((unsigned)ad->impl){
struct Cyc_List_List*_tmpFC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fit=_tmpFC;
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
while((unsigned)fit){
struct Cyc_Absyn_Aggrfield*_tmpFD=(struct Cyc_Absyn_Aggrfield*)fit->hd;struct Cyc_Absyn_Aggrfield*fld=_tmpFD;
struct _tuple14*_tmpFE=Cyc_Cifc_update_tvars(te,tv_ovrs,fld->type,d,0);struct _tuple14*tvtup=_tmpFE;
if((unsigned)tvtup){
changed=1;{
struct Cyc_List_List*_tmpFF=(*tvtup).f1;struct Cyc_List_List*ad=_tmpFF;
if((unsigned)ad)
added_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ad,added_tvars);{
struct Cyc_List_List*_tmp100=(*tvtup).f2;struct Cyc_List_List*rm=_tmp100;
if((unsigned)rm)
removed_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(rm,removed_tvars);}}}
# 722
fit=fit->tl;}
# 724
while((unsigned)removed_tvars){
struct Cyc_Absyn_Tvar*_tmp101=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;struct Cyc_Absyn_Tvar*rtv=_tmp101;
{struct _handler_cons _tmp102;_push_handler(& _tmp102);{int _tmp104=0;if(setjmp(_tmp102.handler))_tmp104=1;if(!_tmp104){
({struct Cyc_List_List*_tmp1F5=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete)(ad->tvs,rtv);ad->tvs=_tmp1F5;});;_pop_handler();}else{void*_tmp103=(void*)Cyc_Core_get_exn_thrown();void*_tmp105=_tmp103;void*_tmp106;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp105)->tag == Cyc_Core_Not_found){
# 731
({struct Cyc_String_pa_PrintArg_struct _tmp109=({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0,({struct _fat_ptr _tmp1F6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_tmp170.f1=_tmp1F6;});_tmp170;});void*_tmp107[1];_tmp107[0]=& _tmp109;({unsigned _tmp1F8=loc;struct _fat_ptr _tmp1F7=({const char*_tmp108="Removed tvar %s not found";_tag_fat(_tmp108,sizeof(char),26U);});Cyc_Warn_warn(_tmp1F8,_tmp1F7,_tag_fat(_tmp107,sizeof(void*),1));});});
goto _LL16;}else{_tmp106=_tmp105;{void*exn=_tmp106;(int)_rethrow(exn);}}_LL16:;}}}
# 734
removed_tvars=removed_tvars->tl;}
# 736
if((unsigned)added_tvars){
({struct Cyc_List_List*_tmp1F9=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(added_tvars,ad->tvs);ad->tvs=_tmp1F9;});{
struct Cyc_Absyn_Decl*_tmp10A=Cyc_Cifc_make_abstract_decl(d);struct Cyc_Absyn_Decl*absdecl=_tmp10A;
{struct _handler_cons _tmp10B;_push_handler(& _tmp10B);{int _tmp10D=0;if(setjmp(_tmp10B.handler))_tmp10D=1;if(!_tmp10D){
abs_decls=((struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_Absyn_Decl*))Cyc_List_delete_cmp)(Cyc_Cifc_decl_name_cmp,abs_decls,absdecl);;_pop_handler();}else{void*_tmp10C=(void*)Cyc_Core_get_exn_thrown();void*_tmp10E=_tmp10C;void*_tmp10F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp10E)->tag == Cyc_Core_Not_found)
# 743
goto _LL1B;else{_tmp10F=_tmp10E;{void*exn=_tmp10F;(int)_rethrow(exn);}}_LL1B:;}}}
# 745
abs_decls=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->hd=absdecl,_tmp110->tl=abs_decls;_tmp110;});}}
# 747
if(changed)
changed_decls=({struct Cyc_List_List*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->hd=d,_tmp111->tl=changed_decls;_tmp111;});}else{
# 751
struct _tuple0*_tmp112=ad->name;struct _tuple0*_stmttmpF=_tmp112;struct _tuple0*_tmp113=_stmttmpF;void*_tmp114;_tmp114=_tmp113->f2;{struct _fat_ptr*name=_tmp114;
struct Cyc_Absyn_Decl*_tmp115=Cyc_Absyn_lookup_decl(tv_ovrs,name);struct Cyc_Absyn_Decl*ovd=_tmp115;
if((unsigned)ovd){
changed=1;{
void*_tmp116=ovd->r;void*_stmttmp10=_tmp116;void*_tmp117=_stmttmp10;void*_tmp118;if(*((int*)_tmp117)== 5){_tmp118=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp117)->f1;{struct Cyc_Absyn_Aggrdecl*ovd_ad=_tmp118;
# 757
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*_tmp119=ovd_ad->tvs;struct Cyc_List_List*tvs=_tmp119;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmp11A=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp11A;
struct Cyc_Absyn_Tvar*_tmp11B=({struct Cyc_Absyn_Tvar*_tmp122=_cycalloc(sizeof(*_tmp122));*_tmp122=*tv;_tmp122;});struct Cyc_Absyn_Tvar*tvcpy=_tmp11B;
struct _fat_ptr*_tmp11C=({struct _fat_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _fat_ptr _tmp1FB=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp171;_tmp171.tag=1,_tmp171.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_tmp171;});void*_tmp11E[1];_tmp11E[0]=& _tmp120;({struct _fat_ptr _tmp1FA=({const char*_tmp11F="`ovr_%d";_tag_fat(_tmp11F,sizeof(char),8U);});Cyc_aprintf(_tmp1FA,_tag_fat(_tmp11E,sizeof(void*),1));});});*_tmp121=_tmp1FB;});_tmp121;});struct _fat_ptr*tvn=_tmp11C;
tvcpy->name=tvn;
({struct Cyc_List_List*_tmp1FC=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->hd=tvcpy,_tmp11D->tl=ad->tvs;_tmp11D;});ad->tvs=_tmp1FC;});
tvs=tvs->tl;}
# 767
({struct Cyc_List_List*_tmp1FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ad->tvs);ad->tvs=_tmp1FD;});
goto _LL23;}}else{
# 770
({void*_tmp123=0U;({struct _fat_ptr _tmp1FE=({const char*_tmp124="ovd must be an aggr type";_tag_fat(_tmp124,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp1FE,_tag_fat(_tmp123,sizeof(void*),0));});});}_LL23:;}}}}
# 776
goto _LL0;}case 8: _tmpF7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpF6)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmpF7;
# 778
goto _LL0;}default:
# 780
 goto _LL0;}_LL0:;}
# 782
prev=ds;
ds=ds->tl;
if(changed)
some_change=1;}
# 787
tv_ovrs=changed_decls;}}while(
some_change && ++ niter < (unsigned)10);
if(niter >= (unsigned)10){
({struct Cyc_String_pa_PrintArg_struct _tmp127=({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0,({
struct _fat_ptr _tmp1FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(changed_decls));_tmp172.f1=_tmp1FF;});_tmp172;});void*_tmp125[1];_tmp125[0]=& _tmp127;({struct Cyc___cycFILE*_tmp201=Cyc_stderr;struct _fat_ptr _tmp200=({const char*_tmp126="Suspected mutually recursive structs involving the following decls \n %s";_tag_fat(_tmp126,sizeof(char),72U);});Cyc_fprintf(_tmp201,_tmp200,_tag_fat(_tmp125,sizeof(void*),1));});});
({struct Cyc_Warn_String_Warn_Warg_struct _tmp129=({struct Cyc_Warn_String_Warn_Warg_struct _tmp173;_tmp173.tag=0,({struct _fat_ptr _tmp202=({const char*_tmp12A="Suspected mutually recursive structs -- abandoning cifc";_tag_fat(_tmp12A,sizeof(char),56U);});_tmp173.f1=_tmp202;});_tmp173;});void*_tmp128[1];_tmp128[0]=& _tmp129;({unsigned _tmp203=loc;Cyc_Warn_err2(_tmp203,_tag_fat(_tmp128,sizeof(void*),1));});});}
# 794
return abs_decls;}
# 797
static void Cyc_Cifc_i_clear_vartype_ids(void*t){
void*_tmp12B=t;void*_tmp12D;struct Cyc_Absyn_FnInfo _tmp12E;struct Cyc_Absyn_ArrayInfo _tmp12F;struct Cyc_Absyn_PtrInfo _tmp130;void*_tmp12C;switch(*((int*)_tmp12B)){case 2: _tmp12C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp12B)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp12C;
# 800
tv->identity=- 1;
goto _LL0;}case 0: _tmp12C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12B)->f2;{struct Cyc_List_List*ts=_tmp12C;
# 803
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 807
goto _LL0;}case 3: _tmp130=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12B)->f1;{struct Cyc_Absyn_PtrInfo pi=_tmp130;
# 809
Cyc_Cifc_i_clear_vartype_ids(pi.elt_type);
goto _LL0;}case 4: _tmp12F=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12B)->f1;{struct Cyc_Absyn_ArrayInfo ai=_tmp12F;
# 812
Cyc_Cifc_i_clear_vartype_ids(ai.elt_type);
goto _LL0;}case 5: _tmp12E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12B)->f1;{struct Cyc_Absyn_FnInfo fi=_tmp12E;
# 815
Cyc_Cifc_i_clear_vartype_ids(fi.ret_type);{
struct Cyc_List_List*_tmp131=fi.args;struct Cyc_List_List*argit=_tmp131;
while((unsigned)argit){
struct _tuple8*_tmp132=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmp11=_tmp132;struct _tuple8*_tmp133=_stmttmp11;void*_tmp134;_tmp134=_tmp133->f3;{void*at=_tmp134;
Cyc_Cifc_i_clear_vartype_ids(at);
argit=argit->tl;}}
# 822
goto _LL0;}}case 6: _tmp12C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp12B)->f1;{struct Cyc_List_List*ts=_tmp12C;
# 824
while((unsigned)ts){
struct _tuple11*_tmp135=(struct _tuple11*)ts->hd;struct _tuple11*_stmttmp12=_tmp135;struct _tuple11*_tmp136=_stmttmp12;void*_tmp137;_tmp137=_tmp136->f2;{void*et=_tmp137;
Cyc_Cifc_i_clear_vartype_ids(et);
ts=ts->tl;}}
# 829
goto _LL0;}case 8: _tmp12C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp12B)->f2;_tmp12D=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp12B)->f4;{struct Cyc_List_List*ts=_tmp12C;void*to=_tmp12D;
# 834
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 838
goto _LL0;}default:
# 843
 goto _LL0;}_LL0:;}
# 847
static void Cyc_Cifc_clear_vartype_ids(struct Cyc_Absyn_Decl*d){
void*_tmp138=d->r;void*_stmttmp13=_tmp138;void*_tmp139=_stmttmp13;void*_tmp13A;switch(*((int*)_tmp139)){case 0: _tmp13A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp139)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp13A;
# 850
Cyc_Cifc_i_clear_vartype_ids(vd->type);
goto _LL0;}case 1: _tmp13A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp139)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp13A;
# 853
Cyc_Cifc_i_clear_vartype_ids((fd->i).ret_type);{
struct Cyc_List_List*_tmp13B=(fd->i).args;struct Cyc_List_List*ai=_tmp13B;
while((unsigned)ai){
Cyc_Cifc_i_clear_vartype_ids((*((struct _tuple8*)ai->hd)).f3);}
# 858
(fd->i).tvars=0;
goto _LL0;}}case 5: _tmp13A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp139)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp13A;
# 861
goto _LL0;}case 8: _tmp13A=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp139)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp13A;
# 863
goto _LL0;}default:
# 866
 goto _LL0;}_LL0:;}
# 870
static int Cyc_Cifc_eq(void*a,void*b){
return a == b;}
# 874
static int Cyc_Cifc_decl_not_present(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*a){
return !((int(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_Absyn_Decl*,struct Cyc_List_List*))Cyc_List_exists_c)((int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*))Cyc_Cifc_eq,a,l);}
# 879
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List**ds,struct Cyc_List_List*ovrs){
struct Cyc_List_List*type_overrides=0;
struct Cyc_List_List*overriden_decls=0;
struct Cyc_List_List*abs_decls=0;
{struct Cyc_List_List*_tmp13C=ovrs;struct Cyc_List_List*x=_tmp13C;for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Decl*_tmp13D=(struct Cyc_Absyn_Decl*)x->hd;struct Cyc_Absyn_Decl*ud=_tmp13D;
struct _fat_ptr*_tmp13E=Cyc_Absyn_decl_name(ud);struct _fat_ptr*un=_tmp13E;
if(!((unsigned)un))({void*_tmp13F=0U;({unsigned _tmp205=ud->loc;struct _fat_ptr _tmp204=({const char*_tmp140="Overriding decl without a name\n";_tag_fat(_tmp140,sizeof(char),32U);});Cyc_Warn_warn(_tmp205,_tmp204,_tag_fat(_tmp13F,sizeof(void*),0));});});else{
# 888
struct Cyc_Absyn_Decl*_tmp141=Cyc_Absyn_lookup_decl(*((struct Cyc_List_List**)_check_null(ds)),un);struct Cyc_Absyn_Decl*d=_tmp141;
if(!((unsigned)d))({struct Cyc_String_pa_PrintArg_struct _tmp144=({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0,_tmp174.f1=(struct _fat_ptr)((struct _fat_ptr)*un);_tmp174;});void*_tmp142[1];_tmp142[0]=& _tmp144;({unsigned _tmp207=ud->loc;struct _fat_ptr _tmp206=({const char*_tmp143="%s is overridden but not defined";_tag_fat(_tmp143,sizeof(char),33U);});Cyc_Warn_warn(_tmp207,_tmp206,_tag_fat(_tmp142,sizeof(void*),1));});});else{
# 891
overriden_decls=({struct Cyc_List_List*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->hd=d,_tmp145->tl=overriden_decls;_tmp145;});{
int _tmp146=Cyc_Cifc_contains_type_vars(d);int pre_tvars_d=_tmp146;
Cyc_Cifc_merge_sys_user_decl(loc,te,0,ud,d);
Cyc_Cifc_clear_vartype_ids(ud);
if(Cyc_Cifc_contains_type_vars(ud)&& !pre_tvars_d){
abs_decls=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));({struct Cyc_Absyn_Decl*_tmp208=Cyc_Cifc_make_abstract_decl(ud);_tmp147->hd=_tmp208;}),_tmp147->tl=abs_decls;_tmp147;});
type_overrides=({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->hd=d,_tmp148->tl=type_overrides;_tmp148;});}}}}}}{
# 903
struct Cyc_List_List*_tmp149=((struct Cyc_List_List*(*)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_filter_c)(Cyc_Cifc_decl_not_present,overriden_decls,*((struct Cyc_List_List**)_check_null(ds)));struct Cyc_List_List*unoverriden_decls=_tmp149;
abs_decls=({struct Cyc_List_List*_tmp209=abs_decls;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp209,
Cyc_Cifc_update_usages(loc,te,type_overrides,unoverriden_decls));});
if((unsigned)abs_decls)
({struct Cyc_List_List*_tmp20A=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(abs_decls,*ds);*ds=_tmp20A;});}}

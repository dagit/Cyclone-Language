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
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 261
extern int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 349
extern struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x);
# 354
extern struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 891 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 929
extern void*Cyc_Absyn_void_type;
# 970
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1086
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1134
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1140
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name);
# 1142
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl);
# 1146
int Cyc_Absyn_equal_att(void*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 34 "cifc.h"
void Cyc_Cifc_merge_sys_user_decl(unsigned,struct Cyc_Tcenv_Tenv*,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 30 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*t);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
# 76
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc_Hashtable_Table;
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
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
# 95
void*Cyc_Tcutil_copy_type(void*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
# 38 "cifc.cyc"
static int Cyc_Cifc_glob_tvar_id=0;
# 41
struct _fat_ptr Cyc_Cifc_list2string(struct Cyc_List_List*l,struct _fat_ptr(*tostr)(void*)){
struct _fat_ptr _tmp0=({void*_tmp8=0U;({struct _fat_ptr _tmp18B=({const char*_tmp9="(";_tag_fat(_tmp9,sizeof(char),2U);});Cyc_aprintf(_tmp18B,_tag_fat(_tmp8,sizeof(void*),0U));});});struct _fat_ptr ret=_tmp0;
while((unsigned)l){
ret=({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0U,_tmp161.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_tmp161;});struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0U,({struct _fat_ptr _tmp18C=(struct _fat_ptr)((struct _fat_ptr)((struct _fat_ptr(*)(void*))_check_null(tostr))(l->hd));_tmp160.f1=_tmp18C;});_tmp160;});void*_tmp1[2U];_tmp1[0]=& _tmp3,_tmp1[1]=& _tmp4;({struct _fat_ptr _tmp18D=({const char*_tmp2="%s %s,";_tag_fat(_tmp2,sizeof(char),7U);});Cyc_aprintf(_tmp18D,_tag_fat(_tmp1,sizeof(void*),2U));});});
l=l->tl;}
# 47
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0U,_tmp162.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_tmp162;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct _fat_ptr _tmp18E=({const char*_tmp6="%s)";_tag_fat(_tmp6,sizeof(char),4U);});Cyc_aprintf(_tmp18E,_tag_fat(_tmp5,sizeof(void*),1U));});});}
# 52
static void Cyc_Cifc_fail_merge(int warn,unsigned loc,int is_buildlib,struct _tuple0*n,struct _fat_ptr s){
# 54
if(is_buildlib){
struct _fat_ptr preamble=warn?({const char*_tmpF="Warning: user-defined";_tag_fat(_tmpF,sizeof(char),22U);}):({const char*_tmp10="User-defined";_tag_fat(_tmp10,sizeof(char),13U);});
({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp165;_tmp165.tag=0U,_tmp165.f1=(struct _fat_ptr)((struct _fat_ptr)preamble);_tmp165;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp164;_tmp164.tag=0U,({
# 58
struct _fat_ptr _tmp18F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n));_tmp164.f1=_tmp18F;});_tmp164;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0U,_tmp163.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp163;});void*_tmpA[3U];_tmpA[0]=& _tmpC,_tmpA[1]=& _tmpD,_tmpA[2]=& _tmpE;({struct Cyc___cycFILE*_tmp191=Cyc_stderr;struct _fat_ptr _tmp190=({const char*_tmpB="%s type for %s incompatible with system version %s\n";_tag_fat(_tmpB,sizeof(char),52U);});Cyc_fprintf(_tmp191,_tmp190,_tag_fat(_tmpA,sizeof(void*),3U));});});}else{
# 60
if(warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp169;_tmp169.tag=0U,({struct _fat_ptr _tmp192=({const char*_tmp17="User-defined type for ";_tag_fat(_tmp17,sizeof(char),23U);});_tmp169.f1=_tmp192;});_tmp169;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp13=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp168;_tmp168.tag=1U,_tmp168.f1=n;_tmp168;});struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp167;_tmp167.tag=0U,({struct _fat_ptr _tmp193=({const char*_tmp16=" incompatible with system version ";_tag_fat(_tmp16,sizeof(char),35U);});_tmp167.f1=_tmp193;});_tmp167;});struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp166;_tmp166.tag=0U,_tmp166.f1=s;_tmp166;});void*_tmp11[4U];_tmp11[0]=& _tmp12,_tmp11[1]=& _tmp13,_tmp11[2]=& _tmp14,_tmp11[3]=& _tmp15;({unsigned _tmp194=loc;Cyc_Warn_warn2(_tmp194,_tag_fat(_tmp11,sizeof(void*),4U));});});else{
# 63
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16D;_tmp16D.tag=0U,({struct _fat_ptr _tmp195=({const char*_tmp1E="User-defined type for ";_tag_fat(_tmp1E,sizeof(char),23U);});_tmp16D.f1=_tmp195;});_tmp16D;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp16C;_tmp16C.tag=1U,_tmp16C.f1=n;_tmp16C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16B;_tmp16B.tag=0U,({struct _fat_ptr _tmp196=({const char*_tmp1D=" incompatible with system version ";_tag_fat(_tmp1D,sizeof(char),35U);});_tmp16B.f1=_tmp196;});_tmp16B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16A;_tmp16A.tag=0U,_tmp16A.f1=s;_tmp16A;});void*_tmp18[4U];_tmp18[0]=& _tmp19,_tmp18[1]=& _tmp1A,_tmp18[2]=& _tmp1B,_tmp18[3]=& _tmp1C;({unsigned _tmp197=loc;Cyc_Warn_err2(_tmp197,_tag_fat(_tmp18,sizeof(void*),4U));});});}}}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 68
static void*Cyc_Cifc_expand_c_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp){
# 70
void*_tmp1F=ctyp;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp21;void**_tmp23;struct Cyc_Absyn_TypeDecl*_tmp22;struct Cyc_List_List*_tmp25;enum Cyc_Absyn_AggrKind _tmp24;struct Cyc_List_List*_tmp26;struct Cyc_Absyn_ArrayInfo*_tmp27;struct Cyc_Absyn_PtrInfo*_tmp28;struct Cyc_Absyn_FnInfo*_tmp29;switch(*((int*)_tmp1F)){case 8U: _LL1: _LL2:
# 73
 Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
{void*_tmp2A=ctyp;void*_tmp2B;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2A)->tag == 8U){_LL1A: _tmp2B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2A)->f4;_LL1B: {void*to=_tmp2B;
# 77
return(void*)_check_null(to);}}else{_LL1C: _LL1D:
# 79
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E;_tmp16E.tag=0U,({struct _fat_ptr _tmp198=({const char*_tmp2E="Impos";_tag_fat(_tmp2E,sizeof(char),6U);});_tmp16E.f1=_tmp198;});_tmp16E;});void*_tmp2C[1U];_tmp2C[0]=& _tmp2D;({unsigned _tmp199=loc;Cyc_Warn_err2(_tmp199,_tag_fat(_tmp2C,sizeof(void*),1U));});});
goto _LL19;}_LL19:;}
# 82
return ctyp;case 5U: _LL3: _tmp29=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F)->f1;_LL4: {struct Cyc_Absyn_FnInfo*finfo=_tmp29;
# 84
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 86
({void*_tmp19C=({unsigned _tmp19B=loc;struct Cyc_Tcenv_Tenv*_tmp19A=te;Cyc_Cifc_expand_c_type(_tmp19B,_tmp19A,Cyc_Toc_typ_to_c(finfo->ret_type));});finfo->ret_type=_tmp19C;});{
struct Cyc_List_List*_tmp2F=finfo->args;struct Cyc_List_List*args=_tmp2F;
while((unsigned)args){
struct _tuple8*_tmp30=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp0=_tmp30;struct _tuple8*_tmp31=_stmttmp0;void**_tmp32;_LL1F: _tmp32=(void**)& _tmp31->f3;_LL20: {void**argType=_tmp32;
({void*_tmp19F=({unsigned _tmp19E=loc;struct Cyc_Tcenv_Tenv*_tmp19D=te;Cyc_Cifc_expand_c_type(_tmp19E,_tmp19D,Cyc_Toc_typ_to_c(*argType));});*argType=_tmp19F;});
args=args->tl;}}
# 94
finfo->tvars=0;
finfo->effect=0;
return ctyp;}}case 3U: _LL5: _tmp28=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1;_LL6: {struct Cyc_Absyn_PtrInfo*pinfo=_tmp28;
# 100
({void*_tmp1A2=({unsigned _tmp1A1=loc;struct Cyc_Tcenv_Tenv*_tmp1A0=te;Cyc_Cifc_expand_c_type(_tmp1A1,_tmp1A0,Cyc_Toc_typ_to_c(pinfo->elt_type));});pinfo->elt_type=_tmp1A2;});
return ctyp;}case 4U: _LL7: _tmp27=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F)->f1;_LL8: {struct Cyc_Absyn_ArrayInfo*ainfo=_tmp27;
# 103
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 105
({void*_tmp1A5=({unsigned _tmp1A4=loc;struct Cyc_Tcenv_Tenv*_tmp1A3=te;Cyc_Cifc_expand_c_type(_tmp1A4,_tmp1A3,Cyc_Toc_typ_to_c(ainfo->elt_type));});ainfo->elt_type=_tmp1A5;});
return ctyp;}case 6U: _LL9: _tmp26=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F)->f1;_LLA: {struct Cyc_List_List*tlst=_tmp26;
# 108
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 110
while((unsigned)tlst){
struct _tuple11*_tmp33=(struct _tuple11*)tlst->hd;struct _tuple11*_stmttmp1=_tmp33;struct _tuple11*_tmp34=_stmttmp1;void**_tmp36;struct Cyc_Absyn_Tqual _tmp35;_LL22: _tmp35=_tmp34->f1;_tmp36=(void**)& _tmp34->f2;_LL23: {struct Cyc_Absyn_Tqual tq=_tmp35;void**elt_type=_tmp36;
({void*_tmp1A8=({unsigned _tmp1A7=loc;struct Cyc_Tcenv_Tenv*_tmp1A6=te;Cyc_Cifc_expand_c_type(_tmp1A7,_tmp1A6,Cyc_Toc_typ_to_c(*elt_type));});*elt_type=_tmp1A8;});
tlst=tlst->tl;}}
# 116
return ctyp;}case 7U: _LLB: _tmp24=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F)->f1;_tmp25=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F)->f2;_LLC: {enum Cyc_Absyn_AggrKind knd=_tmp24;struct Cyc_List_List*flst=_tmp25;
# 119
return ctyp;}case 10U: _LLD: _tmp22=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F)->f1;_tmp23=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F)->f2;_LLE: {struct Cyc_Absyn_TypeDecl*td=_tmp22;void**tptr=_tmp23;
# 122
return ctyp;}case 9U: _LLF: _tmp21=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1F)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp21;
# 125
return ctyp;}case 11U: _LL11: _tmp20=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1F)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp20;
# 128
return ctyp;}case 0U: _LL13: _LL14:
# 131
 return ctyp;case 1U: _LL15: _LL16:
# 134
 return ctyp;default: _LL17: _LL18:
# 137
 return ctyp;}_LL0:;}
# 145
static int Cyc_Cifc_c_types_ok(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp,void*cyctyp){
struct Cyc_Absyn_Tqual tq;
# 148
void*_tmp37=Cyc_Tcutil_copy_type(ctyp);void*ctyp_cp=_tmp37;
void*_tmp38=Cyc_Tcutil_copy_type(cyctyp);void*utyp_cp=_tmp38;
# 151
void*_tmp39=Cyc_Toc_typ_to_c(ctyp_cp);void*c_typ=_tmp39;
void*_tmp3A=Cyc_Toc_typ_to_c(utyp_cp);void*u_typ=_tmp3A;
if(!Cyc_Unify_unify(c_typ,u_typ)){
c_typ=Cyc_Cifc_expand_c_type(loc,te,c_typ);
u_typ=Cyc_Cifc_expand_c_type(loc,te,u_typ);
return Cyc_Unify_unify(c_typ,u_typ);}
# 158
return 1;}
# 171 "cifc.cyc"
static struct Cyc_List_List*Cyc_Cifc_merge_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
struct Cyc_List_List*x=0;
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))x=({struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->hd=(void*)a->hd,_tmp3B->tl=x;_tmp3B;});}}
return({struct Cyc_List_List*_tmp1A9=x;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1A9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(a2));});}struct _tuple12{void*f1;void*f2;};struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 181
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,struct Cyc_Tcenv_Tenv*te,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
# 183
struct _tuple12 _tmp3C=({struct _tuple12 _tmp17F;_tmp17F.f1=c_decl->r,_tmp17F.f2=user_decl->r;_tmp17F;});struct _tuple12 _stmttmp2=_tmp3C;struct _tuple12 _tmp3D=_stmttmp2;struct Cyc_Absyn_Typedefdecl*_tmp3F;struct Cyc_Absyn_Typedefdecl*_tmp3E;struct Cyc_Absyn_Enumdecl*_tmp41;struct Cyc_Absyn_Enumdecl*_tmp40;struct Cyc_Absyn_Aggrdecl*_tmp43;struct Cyc_Absyn_Aggrdecl*_tmp42;struct Cyc_Absyn_Vardecl*_tmp45;struct Cyc_Absyn_Fndecl*_tmp44;struct Cyc_Absyn_Vardecl*_tmp47;struct Cyc_Absyn_Vardecl*_tmp46;switch(*((int*)_tmp3D.f1)){case 0U: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->tag == 0U){_LL1: _tmp46=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3D.f1)->f1;_tmp47=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->f1;_LL2: {struct Cyc_Absyn_Vardecl*cd=_tmp46;struct Cyc_Absyn_Vardecl*ud=_tmp47;
# 185
if(!Cyc_Cifc_c_types_ok(loc,te,cd->type,ud->type)){
({unsigned _tmp1AF=loc;int _tmp1AE=is_buildlib;struct _tuple0*_tmp1AD=cd->name;Cyc_Cifc_fail_merge(0,_tmp1AF,_tmp1AE,_tmp1AD,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4A=({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0U,({struct _fat_ptr _tmp1AA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_tmp170.f1=_tmp1AA;});_tmp170;});struct Cyc_String_pa_PrintArg_struct _tmp4B=({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0U,({struct _fat_ptr _tmp1AB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cd->type));_tmp16F.f1=_tmp1AB;});_tmp16F;});void*_tmp48[2U];_tmp48[0]=& _tmp4A,_tmp48[1]=& _tmp4B;({struct _fat_ptr _tmp1AC=({const char*_tmp49=": type %s != %s";_tag_fat(_tmp49,sizeof(char),16U);});Cyc_aprintf(_tmp1AC,_tag_fat(_tmp48,sizeof(void*),2U));});}));});if(!0)return;}else{
# 193
if(ud->attributes != 0)
({struct Cyc_List_List*_tmp1B0=Cyc_Cifc_merge_attributes(cd->attributes,ud->attributes);cd->attributes=_tmp1B0;});
cd->type=ud->type;
cd->tq=ud->tq;}
# 198
goto _LL0;}}else{goto _LLB;}case 1U: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->tag == 0U){_LL3: _tmp44=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp3D.f1)->f1;_tmp45=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->f1;_LL4: {struct Cyc_Absyn_Fndecl*cd=_tmp44;struct Cyc_Absyn_Vardecl*ud=_tmp45;
# 201
if(!Cyc_Tcutil_is_function_type(ud->type)){
({unsigned _tmp1B3=loc;int _tmp1B2=is_buildlib;struct _tuple0*_tmp1B1=ud->name;Cyc_Cifc_fail_merge(0,_tmp1B3,_tmp1B2,_tmp1B1,({const char*_tmp4C=": type must be a function type to match decl\n";_tag_fat(_tmp4C,sizeof(char),46U);}));});if(!0)return;}{
# 204
void*cdtype;
if(cd->cached_type != 0)
cdtype=(void*)_check_null(cd->cached_type);else{
# 208
cdtype=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=5U,_tmp4D->f1=cd->i;_tmp4D;});}
if(!Cyc_Cifc_c_types_ok(loc,te,cdtype,ud->type)){
({unsigned _tmp1B9=loc;int _tmp1B8=is_buildlib;struct _tuple0*_tmp1B7=ud->name;Cyc_Cifc_fail_merge(0,_tmp1B9,_tmp1B8,_tmp1B7,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0U,({struct _fat_ptr _tmp1B4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cdtype));_tmp172.f1=_tmp1B4;});_tmp172;});struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0U,({struct _fat_ptr _tmp1B5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_tmp171.f1=_tmp1B5;});_tmp171;});void*_tmp4E[2U];_tmp4E[0]=& _tmp50,_tmp4E[1]=& _tmp51;({struct _fat_ptr _tmp1B6=({const char*_tmp4F=": type %s != %s";_tag_fat(_tmp4F,sizeof(char),16U);});Cyc_aprintf(_tmp1B6,_tag_fat(_tmp4E,sizeof(void*),2U));});}));});if(!0)return;}else{
# 217
void*_tmp52=ud->type;void*_stmttmp3=_tmp52;void*_tmp53=_stmttmp3;struct Cyc_Absyn_FnInfo _tmp54;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53)->tag == 5U){_LLE: _tmp54=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53)->f1;_LLF: {struct Cyc_Absyn_FnInfo fi=_tmp54;
# 219
struct Cyc_List_List*_tmp55=fi.tvars;struct Cyc_List_List*old_tvars=_tmp55;
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ud->type);
if((cd->i).attributes != 0)
({struct Cyc_List_List*_tmp1BA=Cyc_Cifc_merge_attributes(fi.attributes,(cd->i).attributes);fi.attributes=_tmp1BA;});
cd->i=fi;
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp57=({struct Cyc_Warn_String_Warn_Warg_struct _tmp173;_tmp173.tag=0U,({struct _fat_ptr _tmp1BB=({const char*_tmp58="oops!\n";_tag_fat(_tmp58,sizeof(char),7U);});_tmp173.f1=_tmp1BB;});_tmp173;});void*_tmp56[1U];_tmp56[0]=& _tmp57;Cyc_Warn_err2(0U,_tag_fat(_tmp56,sizeof(void*),1U));});}_LLD:;}
# 228
goto _LL0;}}}else{goto _LLB;}case 5U: if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->tag == 5U){_LL5: _tmp42=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3D.f1)->f1;_tmp43=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*cd=_tmp42;struct Cyc_Absyn_Aggrdecl*ud=_tmp43;
# 236
if((int)ud->sc != (int)cd->sc){
({unsigned _tmp1BE=loc;int _tmp1BD=is_buildlib;struct _tuple0*_tmp1BC=cd->name;Cyc_Cifc_fail_merge(1,_tmp1BE,_tmp1BD,_tmp1BC,({const char*_tmp59=": scopes don't match (ignoring)";_tag_fat(_tmp59,sizeof(char),32U);}));});if(!1)return;}
# 239
if(ud->impl == 0){
({unsigned _tmp1C1=loc;int _tmp1C0=is_buildlib;struct _tuple0*_tmp1BF=cd->name;Cyc_Cifc_fail_merge(0,_tmp1C1,_tmp1C0,_tmp1BF,({const char*_tmp5A=": no user definition";_tag_fat(_tmp5A,sizeof(char),21U);}));});if(!0)return;}
if(cd->impl == 0){
({unsigned _tmp1C4=loc;int _tmp1C3=is_buildlib;struct _tuple0*_tmp1C2=cd->name;Cyc_Cifc_fail_merge(1,_tmp1C4,_tmp1C3,_tmp1C2,({const char*_tmp5B=": no definition for system version";_tag_fat(_tmp5B,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 246
{struct _tuple13 _tmp5C=({struct _tuple13 _tmp179;_tmp179.f1=cd->impl,_tmp179.f2=ud->impl;_tmp179;});struct _tuple13 _stmttmp4=_tmp5C;struct _tuple13 _tmp5D=_stmttmp4;int _tmp64;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp62;struct Cyc_List_List*_tmp61;struct Cyc_List_List*_tmp60;struct Cyc_List_List**_tmp5F;struct Cyc_List_List**_tmp5E;if(_tmp5D.f1 != 0){if(_tmp5D.f2 != 0){_LL13: _tmp5E=(struct Cyc_List_List**)&(_tmp5D.f1)->exist_vars;_tmp5F=(struct Cyc_List_List**)&(_tmp5D.f1)->rgn_po;_tmp60=(_tmp5D.f1)->fields;_tmp61=(_tmp5D.f2)->exist_vars;_tmp62=(_tmp5D.f2)->rgn_po;_tmp63=(_tmp5D.f2)->fields;_tmp64=(_tmp5D.f2)->tagged;_LL14: {struct Cyc_List_List**tvarsC=_tmp5E;struct Cyc_List_List**rgnpoC=_tmp5F;struct Cyc_List_List*cfields=_tmp60;struct Cyc_List_List*tvars=_tmp61;struct Cyc_List_List*rgnpo=_tmp62;struct Cyc_List_List*ufields=_tmp63;int tagged=_tmp64;
# 249
if(tagged){
({unsigned _tmp1C7=loc;int _tmp1C6=is_buildlib;struct _tuple0*_tmp1C5=cd->name;Cyc_Cifc_fail_merge(0,_tmp1C7,_tmp1C6,_tmp1C5,({const char*_tmp65=": user @tagged annotation not allowed (ignoring)";_tag_fat(_tmp65,sizeof(char),49U);}));});if(!0)return;}{
# 252
struct Cyc_List_List*_tmp66=cfields;struct Cyc_List_List*x=_tmp66;
# 254
while(x != 0){
struct Cyc_Absyn_Aggrfield*_tmp67=(struct Cyc_Absyn_Aggrfield*)x->hd;struct Cyc_Absyn_Aggrfield*cfield=_tmp67;
struct Cyc_Absyn_Aggrfield*_tmp68=Cyc_Absyn_lookup_field(ufields,cfield->name);struct Cyc_Absyn_Aggrfield*ufield=_tmp68;
# 258
if(ufield != 0){
# 261
if(!Cyc_Cifc_c_types_ok(loc,te,cfield->type,ufield->type)){
({unsigned _tmp1CD=loc;int _tmp1CC=is_buildlib;struct _tuple0*_tmp1CB=cd->name;Cyc_Cifc_fail_merge(0,_tmp1CD,_tmp1CC,_tmp1CB,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp176;_tmp176.tag=0U,({struct _fat_ptr _tmp1C8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_tmp176.f1=_tmp1C8;});_tmp176;});struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0U,_tmp175.f1=(struct _fat_ptr)((struct _fat_ptr)*cfield->name);_tmp175;});struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0U,({struct _fat_ptr _tmp1C9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cfield->type));_tmp174.f1=_tmp1C9;});_tmp174;});void*_tmp69[3U];_tmp69[0]=& _tmp6B,_tmp69[1]=& _tmp6C,_tmp69[2]=& _tmp6D;({struct _fat_ptr _tmp1CA=({const char*_tmp6A=": type %s of user definition of field %s != %s";_tag_fat(_tmp6A,sizeof(char),47U);});Cyc_aprintf(_tmp1CA,_tag_fat(_tmp69,sizeof(void*),3U));});}));});if(!0)return;}else{
# 270
if(ufield->width != 0){
({unsigned _tmp1D2=loc;int _tmp1D1=is_buildlib;struct _tuple0*_tmp1D0=cd->name;Cyc_Cifc_fail_merge(1,_tmp1D2,_tmp1D1,_tmp1D0,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp177;_tmp177.tag=0U,({struct _fat_ptr _tmp1CE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_tmp177.f1=_tmp1CE;});_tmp177;});void*_tmp6E[1U];_tmp6E[0]=& _tmp70;({struct _fat_ptr _tmp1CF=({const char*_tmp6F=": ignoring width of user definition of field %s";_tag_fat(_tmp6F,sizeof(char),48U);});Cyc_aprintf(_tmp1CF,_tag_fat(_tmp6E,sizeof(void*),1U));});}));});if(!1)return;}
# 275
if(ufield->attributes != 0)
({struct Cyc_List_List*_tmp1D3=Cyc_Cifc_merge_attributes(cfield->attributes,ufield->attributes);cfield->attributes=_tmp1D3;});
# 279
cfield->type=ufield->type;
cfield->tq=ufield->tq;
cfield->requires_clause=ufield->requires_clause;}}
# 284
x=x->tl;}
# 288
if(ud->tvs != 0)cd->tvs=ud->tvs;
if((unsigned)tvars)*tvarsC=tvars;
if((unsigned)rgnpo)*rgnpoC=rgnpo;
# 293
x=ufields;{
int missing_fields=0;
while(x != 0){
struct Cyc_Absyn_Aggrfield*_tmp71=Cyc_Absyn_lookup_field(cfields,((struct Cyc_Absyn_Aggrfield*)x->hd)->name);struct Cyc_Absyn_Aggrfield*cfield=_tmp71;
if(cfield == 0){
missing_fields=1;
({unsigned _tmp1D7=loc;int _tmp1D6=is_buildlib;struct _tuple0*_tmp1D5=cd->name;Cyc_Cifc_fail_merge(1,_tmp1D7,_tmp1D6,_tmp1D5,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0U,_tmp178.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)x->hd)->name);_tmp178;});void*_tmp72[1U];_tmp72[0]=& _tmp74;({struct _fat_ptr _tmp1D4=({const char*_tmp73=": no definition of field %s in system version";_tag_fat(_tmp73,sizeof(char),46U);});Cyc_aprintf(_tmp1D4,_tag_fat(_tmp72,sizeof(void*),1U));});}));});if(!1)return;}
# 303
x=x->tl;}
# 305
goto _LL12;}}}}else{goto _LL15;}}else{_LL15: _LL16:
# 307
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp1D8=({const char*_tmp75="Internal Error: NULL cases not possible";_tag_fat(_tmp75,sizeof(char),40U);});_tmp76->f1=_tmp1D8;});_tmp76;}));}_LL12:;}
# 309
goto _LL0;}}else{goto _LLB;}case 7U: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->tag == 7U){_LL7: _tmp40=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3D.f1)->f1;_tmp41=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*cd=_tmp40;struct Cyc_Absyn_Enumdecl*ud=_tmp41;
# 312
({unsigned _tmp1DB=loc;int _tmp1DA=is_buildlib;struct _tuple0*_tmp1D9=cd->name;Cyc_Cifc_fail_merge(0,_tmp1DB,_tmp1DA,_tmp1D9,({const char*_tmp77=": enum merging not currently supported";_tag_fat(_tmp77,sizeof(char),39U);}));});if(!0)return;}}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->tag == 8U){_LL9: _tmp3E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3D.f1)->f1;_tmp3F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3D.f2)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*cd=_tmp3E;struct Cyc_Absyn_Typedefdecl*ud=_tmp3F;
# 315
if(ud->defn == 0){
({unsigned _tmp1DE=loc;int _tmp1DD=is_buildlib;struct _tuple0*_tmp1DC=cd->name;Cyc_Cifc_fail_merge(0,_tmp1DE,_tmp1DD,_tmp1DC,({const char*_tmp78=": no user definition";_tag_fat(_tmp78,sizeof(char),21U);}));});if(!0)return;}
if(cd->defn == 0){
({unsigned _tmp1E1=loc;int _tmp1E0=is_buildlib;struct _tuple0*_tmp1DF=cd->name;Cyc_Cifc_fail_merge(1,_tmp1E1,_tmp1E0,_tmp1DF,({const char*_tmp79=": no definition for system version";_tag_fat(_tmp79,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 323
if(!({unsigned _tmp1E4=loc;struct Cyc_Tcenv_Tenv*_tmp1E3=te;void*_tmp1E2=(void*)_check_null(cd->defn);Cyc_Cifc_c_types_ok(_tmp1E4,_tmp1E3,_tmp1E2,(void*)_check_null(ud->defn));})){
({unsigned _tmp1EA=loc;int _tmp1E9=is_buildlib;struct _tuple0*_tmp1E8=cd->name;Cyc_Cifc_fail_merge(0,_tmp1EA,_tmp1E9,_tmp1E8,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7C=({struct Cyc_String_pa_PrintArg_struct _tmp17B;_tmp17B.tag=0U,({struct _fat_ptr _tmp1E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(ud->defn)));_tmp17B.f1=_tmp1E5;});_tmp17B;});struct Cyc_String_pa_PrintArg_struct _tmp7D=({struct Cyc_String_pa_PrintArg_struct _tmp17A;_tmp17A.tag=0U,({struct _fat_ptr _tmp1E6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(cd->defn)));_tmp17A.f1=_tmp1E6;});_tmp17A;});void*_tmp7A[2U];_tmp7A[0]=& _tmp7C,_tmp7A[1]=& _tmp7D;({struct _fat_ptr _tmp1E7=({const char*_tmp7B=": type definition %s of user definition != %s";_tag_fat(_tmp7B,sizeof(char),46U);});Cyc_aprintf(_tmp1E7,_tag_fat(_tmp7A,sizeof(void*),2U));});}));});if(!0)return;}else{
# 330
cd->tvs=ud->tvs;
cd->defn=ud->defn;
cd->tq=ud->tq;
if(ud->atts != 0)
({struct Cyc_List_List*_tmp1EB=Cyc_Cifc_merge_attributes(cd->atts,ud->atts);cd->atts=_tmp1EB;});}
# 337
goto _LL0;}}else{goto _LLB;}default: _LLB: _LLC:
# 340
 if(is_buildlib)
({struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmp17C;_tmp17C.tag=0U,({
struct _fat_ptr _tmp1EC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp81[1U];_tmp81[0]=user_decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp81,sizeof(struct Cyc_Absyn_Decl*),1U));})));_tmp17C.f1=_tmp1EC;});_tmp17C;});void*_tmp7E[1U];_tmp7E[0]=& _tmp80;({struct Cyc___cycFILE*_tmp1EE=Cyc_stderr;struct _fat_ptr _tmp1ED=({const char*_tmp7F="Error in .cys file: bad (or unsupported) user-defined type %s\n";_tag_fat(_tmp7F,sizeof(char),63U);});Cyc_fprintf(_tmp1EE,_tmp1ED,_tag_fat(_tmp7E,sizeof(void*),1U));});});else{
# 344
({struct Cyc_Warn_String_Warn_Warg_struct _tmp83=({struct Cyc_Warn_String_Warn_Warg_struct _tmp17E;_tmp17E.tag=0U,({struct _fat_ptr _tmp1EF=({const char*_tmp86="bad (or unsupported) user-defined type %s";_tag_fat(_tmp86,sizeof(char),42U);});_tmp17E.f1=_tmp1EF;});_tmp17E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp17D;_tmp17D.tag=0U,({
struct _fat_ptr _tmp1F0=Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp85[1U];_tmp85[0]=user_decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp85,sizeof(struct Cyc_Absyn_Decl*),1U));}));_tmp17D.f1=_tmp1F0;});_tmp17D;});void*_tmp82[2U];_tmp82[0]=& _tmp83,_tmp82[1]=& _tmp84;({unsigned _tmp1F1=loc;Cyc_Warn_err2(_tmp1F1,_tag_fat(_tmp82,sizeof(void*),2U));});});}
return;}_LL0:;}
# 350
static int Cyc_Cifc_contains_type_vars(struct Cyc_Absyn_Decl*ud){
{void*_tmp87=ud->r;void*_stmttmp5=_tmp87;void*_tmp88=_stmttmp5;struct Cyc_Absyn_Aggrdecl*_tmp89;if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp88)->tag == 5U){_LL1: _tmp89=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp88)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp89;
# 353
return ad->tvs != 0;}}else{_LL3: _LL4:
# 355
 goto _LL0;}_LL0:;}
# 357
return 0;}
# 360
static struct Cyc_Absyn_Decl*Cyc_Cifc_make_abstract_decl(struct Cyc_Absyn_Decl*ud){
void*_tmp8A=ud->r;void*_stmttmp6=_tmp8A;void*_tmp8B=_stmttmp6;struct Cyc_Absyn_Aggrdecl*_tmp8C;if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8B)->tag == 5U){_LL1: _tmp8C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8B)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp8C;
# 363
struct Cyc_Absyn_Aggrdecl*_tmp8D=({struct Cyc_Absyn_Aggrdecl*_tmp90=_cycalloc(sizeof(*_tmp90));*_tmp90=*ad;_tmp90;});struct Cyc_Absyn_Aggrdecl*absad=_tmp8D;
absad->impl=0;{
struct Cyc_Absyn_Decl*_tmp8E=({void*_tmp1F2=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=5U,_tmp8F->f1=absad;_tmp8F;});Cyc_Absyn_new_decl(_tmp1F2,ud->loc);});struct Cyc_Absyn_Decl*nd=_tmp8E;
return nd;}}}else{_LL3: _LL4:
# 368
({void*_tmp91=0U;({struct _fat_ptr _tmp1F3=({const char*_tmp92="Only aggrdecls";_tag_fat(_tmp92,sizeof(char),15U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1F3,_tag_fat(_tmp91,sizeof(void*),0U));});});}_LL0:;}
# 373
static int Cyc_Cifc_kindbound_subsumed(void*kb1,void*kb2){
void*_tmp93=kb1;struct Cyc_Absyn_Kind*_tmp94;struct Cyc_Absyn_Kind*_tmp96;void*_tmp95;void*_tmp97;struct Cyc_Absyn_Kind*_tmp98;switch(*((int*)_tmp93)){case 0U: _LL1: _tmp98=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp93)->f1;_LL2: {struct Cyc_Absyn_Kind*k1=_tmp98;
# 376
void*_tmp99=kb2;struct Cyc_Absyn_Kind*_tmp9A;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99)->tag == 0U){_LLC: _tmp9A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99)->f1;_LLD: {struct Cyc_Absyn_Kind*k2=_tmp9A;
# 378
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{_LLE: _LLF:
# 380
 return 0;}_LLB:;}case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp93)->f1 != 0){_LL3: _tmp97=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp93)->f1)->v;_LL4: {void*kbb1=_tmp97;
# 383
void*_tmp9B=kb2;void*_tmp9C;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9B)->tag == 1U){if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9B)->f1 != 0){_LL11: _tmp9C=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9B)->f1)->v;_LL12: {void*kbb2=_tmp9C;
# 385
return Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL13;}}else{_LL13: _LL14:
# 387
 return 1;}_LL10:;}}else{_LL5: _LL6:
# 390
 return 1;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93)->f1 != 0){_LL7: _tmp95=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93)->f1)->v;_tmp96=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93)->f2;_LL8: {void*kbb1=_tmp95;struct Cyc_Absyn_Kind*k1=_tmp96;
# 392
void*_tmp9D=kb2;struct Cyc_Absyn_Kind*_tmp9F;void*_tmp9E;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D)->tag == 2U){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D)->f1 != 0){_LL16: _tmp9E=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D)->f1)->v;_tmp9F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D)->f2;_LL17: {void*kbb2=_tmp9E;struct Cyc_Absyn_Kind*k2=_tmp9F;
# 394
return((int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual)&& Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL18;}}else{_LL18: _LL19:
# 396
 return 0;}_LL15:;}}else{_LL9: _tmp94=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93)->f2;_LLA: {struct Cyc_Absyn_Kind*k1=_tmp94;
# 399
void*_tmpA0=kb2;struct Cyc_Absyn_Kind*_tmpA1;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA0)->tag == 2U){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA0)->f1 == 0){_LL1B: _tmpA1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA0)->f2;_LL1C: {struct Cyc_Absyn_Kind*k2=_tmpA1;
# 401
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{goto _LL1D;}}else{_LL1D: _LL1E:
# 403
 return 0;}_LL1A:;}}}_LL0:;}
# 408
static int Cyc_Cifc_find_and_remove(struct Cyc_List_List**lst,void*kind){
struct Cyc_List_List*_tmpA2=*lst;struct Cyc_List_List*cur=_tmpA2;
struct Cyc_List_List*prev=0;
while((unsigned)cur){
void*_tmpA3=(void*)cur->hd;void*t=_tmpA3;
{void*_tmpA4=t;struct Cyc_Absyn_Tvar*_tmpA5;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA4)->tag == 2U){_LL1: _tmpA5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA4)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmpA5;
# 415
if(Cyc_Cifc_kindbound_subsumed(tv->kind,kind)){
if((unsigned)prev)
prev->tl=cur->tl;else{
# 419
*lst=cur->tl;}
cur->tl=0;
return 1;}
# 423
goto _LL0;}}else{_LL3: _LL4:
# 425
({struct Cyc_String_pa_PrintArg_struct _tmpA8=({struct Cyc_String_pa_PrintArg_struct _tmp181;_tmp181.tag=0U,({struct _fat_ptr _tmp1F4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp181.f1=_tmp1F4;});_tmp181;});struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp180;_tmp180.tag=0U,({struct _fat_ptr _tmp1F5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_get_type_kind(t));_tmp180.f1=_tmp1F5;});_tmp180;});void*_tmpA6[2U];_tmpA6[0]=& _tmpA8,_tmpA6[1]=& _tmpA9;({struct _fat_ptr _tmp1F6=({const char*_tmpA7="expects a VarType list only -- got %s(%s)";_tag_fat(_tmpA7,sizeof(char),42U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1F6,_tag_fat(_tmpA6,sizeof(void*),2U));});});}_LL0:;}
# 427
prev=cur;
cur=cur->tl;}
# 430
return 0;}
# 433
static struct Cyc_List_List*Cyc_Cifc_get_tvar_difference(struct Cyc_List_List*tvs,struct Cyc_List_List*remove){
struct Cyc_List_List*ret=0;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmpAA=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*can=_tmpAA;
if(!Cyc_Cifc_find_and_remove(& remove,can->kind))
ret=({struct Cyc_List_List*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->hd=can,_tmpAB->tl=ret;_tmpAB;});
tvs=tvs->tl;}
# 441
return ret;}char Cyc_Cifc_Contains_nontvar[17U]="Contains_nontvar";struct Cyc_Cifc_Contains_nontvar_exn_struct{char*tag;};
# 446
struct Cyc_Cifc_Contains_nontvar_exn_struct Cyc_Cifc_Contains_nontvar_val={Cyc_Cifc_Contains_nontvar};
# 448
static struct Cyc_List_List*Cyc_Cifc_extract_tvars(struct Cyc_List_List*ts){
struct Cyc_List_List*res=0;
while((unsigned)ts){
void*_tmpAC=(void*)ts->hd;void*t=_tmpAC;
{void*_tmpAD=t;struct Cyc_Absyn_Tvar*_tmpAE;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAD)->tag == 2U){_LL1: _tmpAE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAD)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmpAE;
# 454
res=({struct Cyc_List_List*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->hd=tv,_tmpAF->tl=res;_tmpAF;});
goto _LL0;}}else{_LL3: _LL4:
# 457
(int)_throw((void*)& Cyc_Cifc_Contains_nontvar_val);}_LL0:;}
# 459
ts=ts->tl;}
# 461
return res;}
# 464
static void*Cyc_Cifc_instantiate_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
{void*_tmpB0=tv->kind;void*_stmttmp7=_tmpB0;void*_tmpB1=_stmttmp7;struct Cyc_Absyn_Kind*_tmpB2;struct Cyc_Absyn_Kind*_tmpB3;switch(*((int*)_tmpB1)){case 0U: _LL1: _tmpB3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB1)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmpB3;
_tmpB2=k;goto _LL4;}case 2U: _LL3: _tmpB2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB1)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmpB2;
# 468
if((int)k->kind == (int)Cyc_Absyn_BoxKind ||(int)k->kind == (int)Cyc_Absyn_AnyKind)
return({void*_tmp1F7=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_tmp1F7,Cyc_Absyn_empty_tqual(loc));});
goto _LL0;}default: _LL5: _LL6:
# 472
 return({void*_tmp1F8=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_tmp1F8,Cyc_Absyn_empty_tqual(loc));});}_LL0:;}
# 474
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp183;_tmp183.tag=0U,({struct _fat_ptr _tmp1F9=({const char*_tmpB7="Unable to instantiate tvar %s";_tag_fat(_tmpB7,sizeof(char),30U);});_tmp183.f1=_tmp1F9;});_tmp183;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp182;_tmp182.tag=0U,({struct _fat_ptr _tmp1FA=Cyc_Absynpp_tvar2string(tv);_tmp182.f1=_tmp1FA;});_tmp182;});void*_tmpB4[2U];_tmpB4[0]=& _tmpB5,_tmpB4[1]=& _tmpB6;({unsigned _tmp1FB=loc;Cyc_Warn_err2(_tmp1FB,_tag_fat(_tmpB4,sizeof(void*),2U));});});
return Cyc_Absyn_void_type;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 478
static struct _tuple14*Cyc_Cifc_update_tvars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,void*t,struct Cyc_Absyn_Decl*enclosing_decl,int instantiate){
# 482
{void*_tmpB8=t;struct Cyc_Absyn_FnInfo _tmpB9;struct Cyc_Absyn_PtrInfo*_tmpBA;struct Cyc_List_List**_tmpBC;void*_tmpBB;switch(*((int*)_tmpB8)){case 0U: _LL1: _tmpBB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB8)->f1;_tmpBC=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB8)->f2;_LL2: {void*tc=_tmpBB;struct Cyc_List_List**ts=_tmpBC;
# 484
{void*_tmpBD=tc;union Cyc_Absyn_AggrInfo _tmpBE;if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpBD)->tag == 20U){_LLA: _tmpBE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpBD)->f1;_LLB: {union Cyc_Absyn_AggrInfo ai=_tmpBE;
# 486
union Cyc_Absyn_AggrInfo _tmpBF=ai;struct _tuple2 _tmpC0;struct Cyc_Absyn_Aggrdecl*_tmpC1;if((_tmpBF.KnownAggr).tag == 2){_LLF: _tmpC1=*(_tmpBF.KnownAggr).val;_LL10: {struct Cyc_Absyn_Aggrdecl*ad=_tmpC1;
# 488
struct _tuple0*_tmpC2=ad->name;struct _tuple0*_stmttmp8=_tmpC2;struct _tuple0*_tmpC3=_stmttmp8;struct _fat_ptr*_tmpC5;union Cyc_Absyn_Nmspace _tmpC4;_LL14: _tmpC4=_tmpC3->f1;_tmpC5=_tmpC3->f2;_LL15: {union Cyc_Absyn_Nmspace ns=_tmpC4;struct _fat_ptr*name=_tmpC5;
# 491
struct Cyc_Absyn_Decl*_tmpC6=Cyc_Absyn_lookup_decl(tv_ovrs,name);struct Cyc_Absyn_Decl*ovd=_tmpC6;
if((unsigned)ovd){
# 495
void*_tmpC7=ovd->r;void*_stmttmp9=_tmpC7;void*_tmpC8=_stmttmp9;struct Cyc_Absyn_Aggrdecl*_tmpC9;if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC8)->tag == 5U){_LL17: _tmpC9=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC8)->f1;_LL18: {struct Cyc_Absyn_Aggrdecl*ad=_tmpC9;
# 497
struct Cyc_List_List*removed_tvars=0;
{struct _handler_cons _tmpCA;_push_handler(& _tmpCA);{int _tmpCC=0;if(setjmp(_tmpCA.handler))_tmpCC=1;if(!_tmpCC){
removed_tvars=Cyc_Cifc_extract_tvars(*ts);;_pop_handler();}else{void*_tmpCB=(void*)Cyc_Core_get_exn_thrown();void*_tmpCD=_tmpCB;void*_tmpCE;if(((struct Cyc_Cifc_Contains_nontvar_exn_struct*)_tmpCD)->tag == Cyc_Cifc_Contains_nontvar){_LL1C: _LL1D:
# 503
 return 0;}else{_LL1E: _tmpCE=_tmpCD;_LL1F: {void*exn=_tmpCE;(int)_rethrow(exn);}}_LL1B:;}}}
# 505
*ts=0;{
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*_tmpCF=ad->tvs;struct Cyc_List_List*tvs=_tmpCF;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmpD0=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmpD0;
if(enclosing_decl == ovd)
({struct Cyc_List_List*_tmp1FD=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));({void*_tmp1FC=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->tag=2U,_tmpD1->f1=tv;_tmpD1;});_tmpD2->hd=_tmp1FC;}),_tmpD2->tl=*ts;_tmpD2;});*ts=_tmp1FD;});else{
# 514
if(instantiate)
({struct Cyc_List_List*_tmp1FF=({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));({void*_tmp1FE=Cyc_Cifc_instantiate_tvar(enclosing_decl->loc,tv);_tmpD3->hd=_tmp1FE;}),_tmpD3->tl=*ts;_tmpD3;});*ts=_tmp1FF;});else{
# 518
struct Cyc_Absyn_Tvar*_tmpD4=({struct Cyc_Absyn_Tvar*_tmpDD=_cycalloc(sizeof(*_tmpDD));*_tmpDD=*tv;_tmpDD;});struct Cyc_Absyn_Tvar*tvcpy=_tmpD4;
struct _fat_ptr*_tmpD5=({struct _fat_ptr*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct _fat_ptr _tmp201=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDB=({struct Cyc_Int_pa_PrintArg_struct _tmp184;_tmp184.tag=1U,_tmp184.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_tmp184;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({struct _fat_ptr _tmp200=({const char*_tmpDA="`ovr_%d";_tag_fat(_tmpDA,sizeof(char),8U);});Cyc_aprintf(_tmp200,_tag_fat(_tmpD9,sizeof(void*),1U));});});*_tmpDC=_tmp201;});_tmpDC;});struct _fat_ptr*tvn=_tmpD5;
tvcpy->name=tvn;
added_tvars=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->hd=tvcpy,_tmpD6->tl=added_tvars;_tmpD6;});
({struct Cyc_List_List*_tmp203=({struct Cyc_List_List*_tmpD8=_cycalloc(sizeof(*_tmpD8));({void*_tmp202=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->tag=2U,_tmpD7->f1=tvcpy;_tmpD7;});_tmpD8->hd=_tmp202;}),_tmpD8->tl=*ts;_tmpD8;});*ts=_tmp203;});}}
# 525
tvs=tvs->tl;}
# 527
({struct Cyc_List_List*_tmp204=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(*ts);*ts=_tmp204;});
return enclosing_decl == ovd?0:({struct _tuple14*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->f1=added_tvars,_tmpDE->f2=removed_tvars;_tmpDE;});}}}else{_LL19: _LL1A:
# 530
({void*_tmpDF=0U;({struct _fat_ptr _tmp205=({const char*_tmpE0="ovd must be an aggr type";_tag_fat(_tmpE0,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp205,_tag_fat(_tmpDF,sizeof(void*),0U));});});}_LL16:;}else{
# 534
return 0;}}}}else{_LL11: _tmpC0=(_tmpBF.UnknownAggr).val;_LL12: {struct _tuple2 ua=_tmpC0;
# 536
return 0;}}_LLE:;}}else{_LLC: _LLD:
# 539
 goto _LL9;}_LL9:;}
# 541
return 0;}case 3U: _LL3: _tmpBA=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB8)->f1;_LL4: {struct Cyc_Absyn_PtrInfo*pi=_tmpBA;
# 543
return Cyc_Cifc_update_tvars(te,tv_ovrs,pi->elt_type,enclosing_decl,instantiate);}case 5U: _LL5: _tmpB9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB8)->f1;_LL6: {struct Cyc_Absyn_FnInfo fi=_tmpB9;
# 545
struct Cyc_List_List*_tmpE1=0;struct Cyc_List_List*added_tvars=_tmpE1;
struct Cyc_List_List*_tmpE2=0;struct Cyc_List_List*removed_tvars=_tmpE2;
struct Cyc_List_List*_tmpE3=fi.args;struct Cyc_List_List*argit=_tmpE3;
while((unsigned)argit){
struct _tuple8*_tmpE4=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmpA=_tmpE4;struct _tuple8*_tmpE5=_stmttmpA;void*_tmpE6;_LL21: _tmpE6=_tmpE5->f3;_LL22: {void*at=_tmpE6;
struct _tuple14*_tmpE7=Cyc_Cifc_update_tvars(te,tv_ovrs,at,enclosing_decl,instantiate);struct _tuple14*ar_tup=_tmpE7;
if((unsigned)ar_tup){
if((unsigned)(*ar_tup).f1)
added_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*ar_tup).f1,added_tvars);
if((unsigned)(*ar_tup).f2)
removed_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*ar_tup).f2,removed_tvars);}
# 557
argit=argit->tl;}}
# 559
if((unsigned)added_tvars ||(unsigned)removed_tvars)
return({struct _tuple14*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->f1=added_tvars,_tmpE8->f2=removed_tvars;_tmpE8;});
return 0;}default: _LL7: _LL8:
# 563
 return 0;}_LL0:;}
# 565
return 0;}
# 568
static int Cyc_Cifc_update_fninfo_usage(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,struct Cyc_Absyn_FnInfo*fi,struct Cyc_Absyn_Decl*enclosing_decl){
struct _tuple14*_tmpE9=Cyc_Cifc_update_tvars(te,tv_ovrs,((struct Cyc_Absyn_FnInfo*)_check_null(fi))->ret_type,enclosing_decl,0);struct _tuple14*ad=_tmpE9;
# 571
struct Cyc_List_List*_tmpEA=fi->args;struct Cyc_List_List*argit=_tmpEA;
struct Cyc_List_List*added_tvars=(unsigned)ad?(*ad).f1: 0;
struct Cyc_List_List*removed_tvars=(unsigned)ad?(*ad).f2: 0;
int change=0;
while((unsigned)argit){
struct _tuple8*_tmpEB=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmpB=_tmpEB;struct _tuple8*_tmpEC=_stmttmpB;void**_tmpED;_LL1: _tmpED=(void**)& _tmpEC->f3;_LL2: {void**at=_tmpED;
struct _tuple14*_tmpEE=Cyc_Cifc_update_tvars(te,tv_ovrs,*at,enclosing_decl,0);struct _tuple14*ad=_tmpEE;
if((unsigned)ad){
added_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*ad).f1,added_tvars);
removed_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*ad).f2,removed_tvars);}
# 582
argit=argit->tl;}}
# 584
while((unsigned)removed_tvars){
change=1;{
struct Cyc_Absyn_Tvar*_tmpEF=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;struct Cyc_Absyn_Tvar*rtv=_tmpEF;
{struct _handler_cons _tmpF0;_push_handler(& _tmpF0);{int _tmpF2=0;if(setjmp(_tmpF0.handler))_tmpF2=1;if(!_tmpF2){
({struct Cyc_List_List*_tmp206=((struct Cyc_List_List*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_delete)(fi->tvars,rtv);fi->tvars=_tmp206;});;_pop_handler();}else{void*_tmpF1=(void*)Cyc_Core_get_exn_thrown();void*_tmpF3=_tmpF1;void*_tmpF4;if(((struct Cyc_Core_Not_found_exn_struct*)_tmpF3)->tag == Cyc_Core_Not_found){_LL4: _LL5:
# 592
({struct Cyc_String_pa_PrintArg_struct _tmpF7=({struct Cyc_String_pa_PrintArg_struct _tmp185;_tmp185.tag=0U,({struct _fat_ptr _tmp207=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_tmp185.f1=_tmp207;});_tmp185;});void*_tmpF5[1U];_tmpF5[0]=& _tmpF7;({unsigned _tmp209=loc;struct _fat_ptr _tmp208=({const char*_tmpF6="Removed tvar %s not found";_tag_fat(_tmpF6,sizeof(char),26U);});Cyc_Warn_warn(_tmp209,_tmp208,_tag_fat(_tmpF5,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmpF4=_tmpF3;_LL7: {void*exn=_tmpF4;(int)_rethrow(exn);}}_LL3:;}}}
# 595
removed_tvars=removed_tvars->tl;}}
# 597
if((unsigned)added_tvars){
change=1;
({struct Cyc_List_List*_tmp20A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(added_tvars,fi->tvars);fi->tvars=_tmp20A;});}
# 602
fi->effect=0;
return change;}
# 606
static struct Cyc_List_List*Cyc_Cifc_remove_cur(struct Cyc_List_List*head,struct Cyc_List_List*cur,struct Cyc_List_List*prev){
if((unsigned)prev)
prev->tl=((struct Cyc_List_List*)_check_null(cur))->tl;else{
# 610
head=((struct Cyc_List_List*)_check_null(cur))->tl;}
# 612
return head;}
# 615
static int Cyc_Cifc_decl_name_cmp(struct Cyc_Absyn_Decl*a,struct Cyc_Absyn_Decl*b){
struct _fat_ptr*_tmpF8=Cyc_Absyn_decl_name(a);struct _fat_ptr*na=_tmpF8;
struct _fat_ptr*_tmpF9=Cyc_Absyn_decl_name(b);struct _fat_ptr*nb=_tmpF9;
if(na == 0 ^ nb == 0)
return 1;
if((unsigned)na)
return Cyc_strcmp((struct _fat_ptr)*na,(struct _fat_ptr)*((struct _fat_ptr*)_check_null(nb)));
return 1;}
# 628
static struct Cyc_List_List*Cyc_Cifc_update_usages(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*in_tv_ovrs,struct Cyc_List_List*dsmm){
unsigned niter=0U;
struct Cyc_List_List*abs_decls=0;
int some_change=0;
struct Cyc_List_List*tv_ovrs=in_tv_ovrs;
struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(dsmm);struct Cyc_List_List*dsm=_tmpFA;
struct Cyc_List_List*changed_decls=0;
do{
changed_decls=0;{
struct Cyc_List_List*_tmpFB=dsm;struct Cyc_List_List*ds=_tmpFB;
struct Cyc_List_List*_tmpFC=0;struct Cyc_List_List*prev=_tmpFC;
some_change=0;
while((unsigned)ds){
int changed=0;
struct Cyc_Absyn_Decl*_tmpFD=(struct Cyc_Absyn_Decl*)ds->hd;struct Cyc_Absyn_Decl*d=_tmpFD;
struct _fat_ptr*_tmpFE=Cyc_Absyn_decl_name(d);struct _fat_ptr*dname=_tmpFE;
# 646
{void*_tmpFF=d->r;void*_stmttmpC=_tmpFF;void*_tmp100=_stmttmpC;struct Cyc_Absyn_Typedefdecl*_tmp101;struct Cyc_Absyn_Aggrdecl*_tmp102;struct Cyc_Absyn_Fndecl*_tmp103;struct Cyc_Absyn_Vardecl*_tmp104;switch(*((int*)_tmp100)){case 0U: _LL1: _tmp104=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp100)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp104;
# 648
{void*_tmp105=vd->type;void*_stmttmpD=_tmp105;void*_tmp106=_stmttmpD;void*_tmp107;void*_tmp108;struct Cyc_List_List*_tmp10A;void*_tmp109;struct Cyc_Absyn_FnInfo*_tmp10B;switch(*((int*)_tmp106)){case 5U: _LLC: _tmp10B=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp106)->f1;_LLD: {struct Cyc_Absyn_FnInfo*fi=_tmp10B;
# 650
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,fi,d);
# 656
goto _LLB;}case 0U: _LLE: _tmp109=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1;_tmp10A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f2;_LLF: {void*tc=_tmp109;struct Cyc_List_List*ts=_tmp10A;
# 658
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,vd->type,d,1)!= 0;
goto _LLB;}case 3U: _LL10: _tmp108=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp106)->f1).elt_type;_LL11: {void*et=_tmp108;
_tmp107=et;goto _LL13;}case 4U: _LL12: _tmp107=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp106)->f1).elt_type;_LL13: {void*et=_tmp107;
# 662
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,et,d,1)!= 0;
goto _LLB;}default: _LL14: _LL15:
# 665
 goto _LLB;}_LLB:;}
# 667
goto _LL0;}case 1U: _LL3: _tmp103=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp100)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp103;
# 669
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,& fd->i,d);
# 673
goto _LL0;}case 5U: _LL5: _tmp102=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp100)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp102;
# 675
if((unsigned)ad->impl){
struct Cyc_List_List*_tmp10C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fit=_tmp10C;
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
while((unsigned)fit){
struct Cyc_Absyn_Aggrfield*_tmp10D=(struct Cyc_Absyn_Aggrfield*)fit->hd;struct Cyc_Absyn_Aggrfield*fld=_tmp10D;
struct _tuple14*_tmp10E=Cyc_Cifc_update_tvars(te,tv_ovrs,fld->type,d,0);struct _tuple14*tvtup=_tmp10E;
if((unsigned)tvtup){
changed=1;{
struct Cyc_List_List*_tmp10F=(*tvtup).f1;struct Cyc_List_List*ad=_tmp10F;
if((unsigned)ad)
added_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ad,added_tvars);{
struct Cyc_List_List*_tmp110=(*tvtup).f2;struct Cyc_List_List*rm=_tmp110;
if((unsigned)rm)
removed_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(rm,removed_tvars);}}}
# 691
fit=fit->tl;}
# 693
while((unsigned)removed_tvars){
struct Cyc_Absyn_Tvar*_tmp111=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;struct Cyc_Absyn_Tvar*rtv=_tmp111;
{struct _handler_cons _tmp112;_push_handler(& _tmp112);{int _tmp114=0;if(setjmp(_tmp112.handler))_tmp114=1;if(!_tmp114){
({struct Cyc_List_List*_tmp20B=((struct Cyc_List_List*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_delete)(ad->tvs,rtv);ad->tvs=_tmp20B;});;_pop_handler();}else{void*_tmp113=(void*)Cyc_Core_get_exn_thrown();void*_tmp115=_tmp113;void*_tmp116;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp115)->tag == Cyc_Core_Not_found){_LL17: _LL18:
# 700
({struct Cyc_String_pa_PrintArg_struct _tmp119=({struct Cyc_String_pa_PrintArg_struct _tmp186;_tmp186.tag=0U,({struct _fat_ptr _tmp20C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_tmp186.f1=_tmp20C;});_tmp186;});void*_tmp117[1U];_tmp117[0]=& _tmp119;({unsigned _tmp20E=loc;struct _fat_ptr _tmp20D=({const char*_tmp118="Removed tvar %s not found";_tag_fat(_tmp118,sizeof(char),26U);});Cyc_Warn_warn(_tmp20E,_tmp20D,_tag_fat(_tmp117,sizeof(void*),1U));});});
goto _LL16;}else{_LL19: _tmp116=_tmp115;_LL1A: {void*exn=_tmp116;(int)_rethrow(exn);}}_LL16:;}}}
# 703
removed_tvars=removed_tvars->tl;}
# 705
if((unsigned)added_tvars){
({struct Cyc_List_List*_tmp20F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(added_tvars,ad->tvs);ad->tvs=_tmp20F;});{
struct Cyc_Absyn_Decl*_tmp11A=Cyc_Cifc_make_abstract_decl(d);struct Cyc_Absyn_Decl*absdecl=_tmp11A;
{struct _handler_cons _tmp11B;_push_handler(& _tmp11B);{int _tmp11D=0;if(setjmp(_tmp11B.handler))_tmp11D=1;if(!_tmp11D){
abs_decls=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*l,struct Cyc_Absyn_Decl*x))Cyc_List_delete_cmp)(Cyc_Cifc_decl_name_cmp,abs_decls,absdecl);;_pop_handler();}else{void*_tmp11C=(void*)Cyc_Core_get_exn_thrown();void*_tmp11E=_tmp11C;void*_tmp11F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp11E)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
# 712
 goto _LL1B;}else{_LL1E: _tmp11F=_tmp11E;_LL1F: {void*exn=_tmp11F;(int)_rethrow(exn);}}_LL1B:;}}}
# 714
abs_decls=({struct Cyc_List_List*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->hd=absdecl,_tmp120->tl=abs_decls;_tmp120;});}}
# 716
if(changed)
changed_decls=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=d,_tmp121->tl=changed_decls;_tmp121;});}else{
# 720
struct _tuple0*_tmp122=ad->name;struct _tuple0*_stmttmpE=_tmp122;struct _tuple0*_tmp123=_stmttmpE;struct _fat_ptr*_tmp124;_LL21: _tmp124=_tmp123->f2;_LL22: {struct _fat_ptr*name=_tmp124;
struct Cyc_Absyn_Decl*_tmp125=Cyc_Absyn_lookup_decl(tv_ovrs,name);struct Cyc_Absyn_Decl*ovd=_tmp125;
if((unsigned)ovd){
changed=1;{
void*_tmp126=ovd->r;void*_stmttmpF=_tmp126;void*_tmp127=_stmttmpF;struct Cyc_Absyn_Aggrdecl*_tmp128;if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp127)->tag == 5U){_LL24: _tmp128=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp127)->f1;_LL25: {struct Cyc_Absyn_Aggrdecl*ovd_ad=_tmp128;
# 726
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*_tmp129=ovd_ad->tvs;struct Cyc_List_List*tvs=_tmp129;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmp12A=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp12A;
struct Cyc_Absyn_Tvar*_tmp12B=({struct Cyc_Absyn_Tvar*_tmp132=_cycalloc(sizeof(*_tmp132));*_tmp132=*tv;_tmp132;});struct Cyc_Absyn_Tvar*tvcpy=_tmp12B;
struct _fat_ptr*_tmp12C=({struct _fat_ptr*_tmp131=_cycalloc(sizeof(*_tmp131));({struct _fat_ptr _tmp211=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp130=({struct Cyc_Int_pa_PrintArg_struct _tmp187;_tmp187.tag=1U,_tmp187.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_tmp187;});void*_tmp12E[1U];_tmp12E[0]=& _tmp130;({struct _fat_ptr _tmp210=({const char*_tmp12F="`ovr_%d";_tag_fat(_tmp12F,sizeof(char),8U);});Cyc_aprintf(_tmp210,_tag_fat(_tmp12E,sizeof(void*),1U));});});*_tmp131=_tmp211;});_tmp131;});struct _fat_ptr*tvn=_tmp12C;
tvcpy->name=tvn;
({struct Cyc_List_List*_tmp212=({struct Cyc_List_List*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->hd=tvcpy,_tmp12D->tl=ad->tvs;_tmp12D;});ad->tvs=_tmp212;});
tvs=tvs->tl;}
# 736
({struct Cyc_List_List*_tmp213=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ad->tvs);ad->tvs=_tmp213;});
goto _LL23;}}else{_LL26: _LL27:
# 739
({void*_tmp133=0U;({struct _fat_ptr _tmp214=({const char*_tmp134="ovd must be an aggr type";_tag_fat(_tmp134,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp214,_tag_fat(_tmp133,sizeof(void*),0U));});});}_LL23:;}}}}
# 745
goto _LL0;}case 8U: _LL7: _tmp101=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp100)->f1;_LL8: {struct Cyc_Absyn_Typedefdecl*td=_tmp101;
# 747
goto _LL0;}default: _LL9: _LLA:
# 749
 goto _LL0;}_LL0:;}
# 751
prev=ds;
ds=ds->tl;
if(changed)
some_change=1;}
# 756
tv_ovrs=changed_decls;}}while(
some_change && ++ niter < (unsigned)10);
if(niter >= (unsigned)10){
({struct Cyc_String_pa_PrintArg_struct _tmp137=({struct Cyc_String_pa_PrintArg_struct _tmp188;_tmp188.tag=0U,({
struct _fat_ptr _tmp215=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(changed_decls));_tmp188.f1=_tmp215;});_tmp188;});void*_tmp135[1U];_tmp135[0]=& _tmp137;({struct Cyc___cycFILE*_tmp217=Cyc_stderr;struct _fat_ptr _tmp216=({const char*_tmp136="Suspected mutually recursive structs involving the following decls \n %s";_tag_fat(_tmp136,sizeof(char),72U);});Cyc_fprintf(_tmp217,_tmp216,_tag_fat(_tmp135,sizeof(void*),1U));});});
({struct Cyc_Warn_String_Warn_Warg_struct _tmp139=({struct Cyc_Warn_String_Warn_Warg_struct _tmp189;_tmp189.tag=0U,({struct _fat_ptr _tmp218=({const char*_tmp13A="Suspected mutually recursive structs -- abandoning cifc";_tag_fat(_tmp13A,sizeof(char),56U);});_tmp189.f1=_tmp218;});_tmp189;});void*_tmp138[1U];_tmp138[0]=& _tmp139;({unsigned _tmp219=loc;Cyc_Warn_err2(_tmp219,_tag_fat(_tmp138,sizeof(void*),1U));});});}
# 763
return abs_decls;}
# 766
static void Cyc_Cifc_i_clear_vartype_ids(void*t){
void*_tmp13B=t;void*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_FnInfo _tmp13F;struct Cyc_Absyn_ArrayInfo _tmp140;struct Cyc_Absyn_PtrInfo _tmp141;struct Cyc_List_List*_tmp142;struct Cyc_Absyn_Tvar*_tmp143;switch(*((int*)_tmp13B)){case 2U: _LL1: _tmp143=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13B)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp143;
# 769
tv->identity=- 1;
goto _LL0;}case 0U: _LL3: _tmp142=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13B)->f2;_LL4: {struct Cyc_List_List*ts=_tmp142;
# 772
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 776
goto _LL0;}case 3U: _LL5: _tmp141=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B)->f1;_LL6: {struct Cyc_Absyn_PtrInfo pi=_tmp141;
# 778
Cyc_Cifc_i_clear_vartype_ids(pi.elt_type);
goto _LL0;}case 4U: _LL7: _tmp140=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13B)->f1;_LL8: {struct Cyc_Absyn_ArrayInfo ai=_tmp140;
# 781
Cyc_Cifc_i_clear_vartype_ids(ai.elt_type);
goto _LL0;}case 5U: _LL9: _tmp13F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13B)->f1;_LLA: {struct Cyc_Absyn_FnInfo fi=_tmp13F;
# 784
Cyc_Cifc_i_clear_vartype_ids(fi.ret_type);{
struct Cyc_List_List*_tmp144=fi.args;struct Cyc_List_List*argit=_tmp144;
while((unsigned)argit){
struct _tuple8*_tmp145=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmp10=_tmp145;struct _tuple8*_tmp146=_stmttmp10;void*_tmp147;_LL12: _tmp147=_tmp146->f3;_LL13: {void*at=_tmp147;
Cyc_Cifc_i_clear_vartype_ids(at);
argit=argit->tl;}}
# 791
goto _LL0;}}case 6U: _LLB: _tmp13E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13B)->f1;_LLC: {struct Cyc_List_List*ts=_tmp13E;
# 793
while((unsigned)ts){
struct _tuple11*_tmp148=(struct _tuple11*)ts->hd;struct _tuple11*_stmttmp11=_tmp148;struct _tuple11*_tmp149=_stmttmp11;void*_tmp14A;_LL15: _tmp14A=_tmp149->f2;_LL16: {void*et=_tmp14A;
Cyc_Cifc_i_clear_vartype_ids(et);
ts=ts->tl;}}
# 798
goto _LL0;}case 8U: _LLD: _tmp13C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13B)->f2;_tmp13D=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13B)->f4;_LLE: {struct Cyc_List_List*ts=_tmp13C;void*to=_tmp13D;
# 803
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 807
goto _LL0;}default: _LLF: _LL10:
# 812
 goto _LL0;}_LL0:;}
# 816
static void Cyc_Cifc_clear_vartype_ids(struct Cyc_Absyn_Decl*d){
void*_tmp14B=d->r;void*_stmttmp12=_tmp14B;void*_tmp14C=_stmttmp12;struct Cyc_Absyn_Typedefdecl*_tmp14D;struct Cyc_Absyn_Aggrdecl*_tmp14E;struct Cyc_Absyn_Fndecl*_tmp14F;struct Cyc_Absyn_Vardecl*_tmp150;switch(*((int*)_tmp14C)){case 0U: _LL1: _tmp150=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp14C)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp150;
# 819
Cyc_Cifc_i_clear_vartype_ids(vd->type);
goto _LL0;}case 1U: _LL3: _tmp14F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp14C)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp14F;
# 822
Cyc_Cifc_i_clear_vartype_ids((fd->i).ret_type);{
struct Cyc_List_List*_tmp151=(fd->i).args;struct Cyc_List_List*ai=_tmp151;
while((unsigned)ai){
Cyc_Cifc_i_clear_vartype_ids((*((struct _tuple8*)ai->hd)).f3);}
# 827
(fd->i).tvars=0;
goto _LL0;}}case 5U: _LL5: _tmp14E=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp14C)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp14E;
# 830
goto _LL0;}case 8U: _LL7: _tmp14D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp14C)->f1;_LL8: {struct Cyc_Absyn_Typedefdecl*td=_tmp14D;
# 832
goto _LL0;}default: _LL9: _LLA:
# 835
 goto _LL0;}_LL0:;}
# 839
static int Cyc_Cifc_eq(void*a,void*b){
return a == b;}
# 843
static int Cyc_Cifc_decl_not_present(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*a){
return !((int(*)(int(*pred)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_Absyn_Decl*env,struct Cyc_List_List*x))Cyc_List_exists_c)((int(*)(struct Cyc_Absyn_Decl*a,struct Cyc_Absyn_Decl*b))Cyc_Cifc_eq,a,l);}
# 847
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List**ds,struct Cyc_List_List*ovrs){
struct Cyc_List_List*type_overrides=0;
struct Cyc_List_List*overriden_decls=0;
struct Cyc_List_List*abs_decls=0;
{struct Cyc_List_List*_tmp152=ovrs;struct Cyc_List_List*x=_tmp152;for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Decl*_tmp153=(struct Cyc_Absyn_Decl*)x->hd;struct Cyc_Absyn_Decl*ud=_tmp153;
struct _fat_ptr*_tmp154=Cyc_Absyn_decl_name(ud);struct _fat_ptr*un=_tmp154;
if(!((unsigned)un))({void*_tmp155=0U;({unsigned _tmp21B=ud->loc;struct _fat_ptr _tmp21A=({const char*_tmp156="Overriding decl without a name\n";_tag_fat(_tmp156,sizeof(char),32U);});Cyc_Warn_warn(_tmp21B,_tmp21A,_tag_fat(_tmp155,sizeof(void*),0U));});});else{
# 856
struct Cyc_Absyn_Decl*_tmp157=Cyc_Absyn_lookup_decl(*((struct Cyc_List_List**)_check_null(ds)),un);struct Cyc_Absyn_Decl*d=_tmp157;
if(!((unsigned)d))({struct Cyc_String_pa_PrintArg_struct _tmp15A=({struct Cyc_String_pa_PrintArg_struct _tmp18A;_tmp18A.tag=0U,_tmp18A.f1=(struct _fat_ptr)((struct _fat_ptr)*un);_tmp18A;});void*_tmp158[1U];_tmp158[0]=& _tmp15A;({unsigned _tmp21D=ud->loc;struct _fat_ptr _tmp21C=({const char*_tmp159="%s is overridden but not defined";_tag_fat(_tmp159,sizeof(char),33U);});Cyc_Warn_warn(_tmp21D,_tmp21C,_tag_fat(_tmp158,sizeof(void*),1U));});});else{
# 859
overriden_decls=({struct Cyc_List_List*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B->hd=d,_tmp15B->tl=overriden_decls;_tmp15B;});{
int _tmp15C=Cyc_Cifc_contains_type_vars(d);int pre_tvars_d=_tmp15C;
Cyc_Cifc_merge_sys_user_decl(loc,te,0,ud,d);
Cyc_Cifc_clear_vartype_ids(ud);
if(Cyc_Cifc_contains_type_vars(ud)&& !pre_tvars_d){
abs_decls=({struct Cyc_List_List*_tmp15D=_cycalloc(sizeof(*_tmp15D));({struct Cyc_Absyn_Decl*_tmp21E=Cyc_Cifc_make_abstract_decl(ud);_tmp15D->hd=_tmp21E;}),_tmp15D->tl=abs_decls;_tmp15D;});
type_overrides=({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->hd=d,_tmp15E->tl=type_overrides;_tmp15E;});}}}}}}{
# 871
struct Cyc_List_List*_tmp15F=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Cifc_decl_not_present,overriden_decls,*((struct Cyc_List_List**)_check_null(ds)));struct Cyc_List_List*unoverriden_decls=_tmp15F;
abs_decls=({struct Cyc_List_List*_tmp21F=abs_decls;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp21F,
Cyc_Cifc_update_usages(loc,te,type_overrides,unoverriden_decls));});
if((unsigned)abs_decls)
({struct Cyc_List_List*_tmp220=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(abs_decls,*ds);*ds=_tmp220;});}}

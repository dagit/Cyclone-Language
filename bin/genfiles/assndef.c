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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145 "list.h"
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
# 880 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 972
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
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 62 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 64
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
# 71
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
void*Cyc_AssnDef_cast(void*,void*);
void*Cyc_AssnDef_fresh_var(void*);
# 75
int Cyc_AssnDef_cmp_term(void*,void*);
# 80
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 92
int Cyc_AssnDef_assncmp(void*,void*);
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 117 "assndef.h"
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
# 121
void*Cyc_AssnDef_prim(void*,enum Cyc_AssnDef_Primreln,void*);
void*Cyc_AssnDef_eq(void*,void*);
void*Cyc_AssnDef_neq(void*,void*);
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 130
void*Cyc_AssnDef_reduce(void*);
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
void*Cyc_AssnDef_not(void*);
int Cyc_AssnDef_simple_prove(void*,void*);
# 42 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
void*_tmp0=t;void*_tmp2;enum Cyc_Absyn_Primop _tmp3;int _tmp4;void*_tmp1;unsigned _tmp5;switch(*((int*)_tmp0)){case 0: _tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_tmp0)->f1;{unsigned i=_tmp5;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp8=({struct Cyc_Int_pa_PrintArg_struct _tmpFE;_tmpFE.tag=1,_tmpFE.f1=(unsigned long)((int)i);_tmpFE;});void*_tmp6[1];_tmp6[0]=& _tmp8;({struct _fat_ptr _tmp12F=({const char*_tmp7="%d";_tag_fat(_tmp7,sizeof(char),3U);});Cyc_aprintf(_tmp12F,_tag_fat(_tmp6,sizeof(void*),1));});});}case 1: _tmp1=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp0)->f1;{struct Cyc_Absyn_Exp*e=_tmp1;
return Cyc_Absynpp_exp2string(e);}case 2: _tmp1=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp1;
return Cyc_Absynpp_qvar2string(vd->name);}case 3: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f1 == 0){_tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f2;{int i=_tmp4;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpB=({struct Cyc_Int_pa_PrintArg_struct _tmpFF;_tmpFF.tag=1,_tmpFF.f1=(unsigned long)i;_tmpFF;});void*_tmp9[1];_tmp9[0]=& _tmpB;({struct _fat_ptr _tmp130=({const char*_tmpA="_X%d";_tag_fat(_tmpA,sizeof(char),5U);});Cyc_aprintf(_tmp130,_tag_fat(_tmp9,sizeof(void*),1));});});}}else{_tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f1;_tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp1;int i=_tmp4;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0,({struct _fat_ptr _tmp131=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(((struct Cyc_Absyn_Vardecl*)_check_null(vd))->name));_tmp101.f1=_tmp131;});_tmp101;});struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmp100;_tmp100.tag=1,_tmp100.f1=(unsigned long)i;_tmp100;});void*_tmpC[2];_tmpC[0]=& _tmpE,_tmpC[1]=& _tmpF;({struct _fat_ptr _tmp132=({const char*_tmpD="_%s%d";_tag_fat(_tmpD,sizeof(char),6U);});Cyc_aprintf(_tmp132,_tag_fat(_tmpC,sizeof(void*),2));});});}}case 4: if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl)->tl == 0){_tmp3=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f1;_tmp1=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->hd;_tmp2=(void*)((((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmp3;void*t1=_tmp1;void*t2=_tmp2;
# 50
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp104;_tmp104.tag=0,({struct _fat_ptr _tmp133=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp104.f1=_tmp133;});_tmp104;});struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0,({
struct _fat_ptr _tmp134=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp103.f1=_tmp134;});_tmp103;});struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp102;_tmp102.tag=0,({struct _fat_ptr _tmp135=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp102.f1=_tmp135;});_tmp102;});void*_tmp10[3];_tmp10[0]=& _tmp12,_tmp10[1]=& _tmp13,_tmp10[2]=& _tmp14;({struct _fat_ptr _tmp136=({const char*_tmp11="(%s%s%s)";_tag_fat(_tmp11,sizeof(char),9U);});Cyc_aprintf(_tmp136,_tag_fat(_tmp10,sizeof(void*),3));});});}}else{goto _LLD;}}else{_LLD: _tmp3=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f1;_tmp1=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->hd;{enum Cyc_Absyn_Primop p=_tmp3;void*t=_tmp1;
# 53
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp106;_tmp106.tag=0,({struct _fat_ptr _tmp137=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp106.f1=_tmp137;});_tmp106;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp105;_tmp105.tag=0,({struct _fat_ptr _tmp138=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp105.f1=_tmp138;});_tmp105;});void*_tmp15[2];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18;({struct _fat_ptr _tmp139=({const char*_tmp16="%s(%s)";_tag_fat(_tmp16,sizeof(char),7U);});Cyc_aprintf(_tmp139,_tag_fat(_tmp15,sizeof(void*),2));});});}}}else{
# 56
({void*_tmp1D=0U;({int(*_tmp13B)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp13A=({const char*_tmp1E="term2string primop args";_tag_fat(_tmp1E,sizeof(char),24U);});_tmp13B(_tmp13A,_tag_fat(_tmp1D,sizeof(void*),0));});});}default: _tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp0)->f2;{void*tp=_tmp1;void*tm=_tmp2;
# 55
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B=({struct Cyc_String_pa_PrintArg_struct _tmp108;_tmp108.tag=0,({struct _fat_ptr _tmp13C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_tmp108.f1=_tmp13C;});_tmp108;});struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0,({struct _fat_ptr _tmp13D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(tm));_tmp107.f1=_tmp13D;});_tmp107;});void*_tmp19[2];_tmp19[0]=& _tmp1B,_tmp19[1]=& _tmp1C;({struct _fat_ptr _tmp13E=({const char*_tmp1A="(%s)%s";_tag_fat(_tmp1A,sizeof(char),7U);});Cyc_aprintf(_tmp13E,_tag_fat(_tmp19,sizeof(void*),2));});});}};}
# 60
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=1,_tmp1F->f1=e;_tmp1F;});}
void*Cyc_AssnDef_zero (void){
static void*z=0;
if(z == 0){
struct Cyc_Absyn_Exp*_tmp20=Cyc_Absyn_int_exp(2U,0,0U);struct Cyc_Absyn_Exp*e=_tmp20;
({void*_tmp13F=Cyc_Absyn_int_type(2U,2U);e->topt=_tmp13F;});{
void*_tmp21=Cyc_AssnDef_cnst(e);void*r=_tmp21;
z=r;
return r;}}else{
return(void*)_check_null(z);}}
# 71
void*Cyc_AssnDef_one (void){
static void*n=0;
if(n == 0){
struct Cyc_Absyn_Exp*_tmp22=Cyc_Absyn_int_exp(2U,1,0U);struct Cyc_Absyn_Exp*e=_tmp22;
({void*_tmp140=Cyc_Absyn_int_type(2U,2U);e->topt=_tmp140;});{
void*_tmp23=Cyc_AssnDef_cnst(e);void*r=_tmp23;
n=r;
return r;}}else{
return(void*)_check_null(n);}}
# 82
void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*vd){return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=2,_tmp24->f1=vd;_tmp24;});}
# 84
inline static int Cyc_AssnDef_is_commutative(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp25=p;switch((int)_tmp25){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
 goto _LL6;case Cyc_Absyn_Eq: _LL6:
 goto _LL8;case Cyc_Absyn_Neq: _LL8:
 goto _LLA;case Cyc_Absyn_Bitand: _LLA:
 goto _LLC;case Cyc_Absyn_Bitor: _LLC:
 goto _LLE;case Cyc_Absyn_Bitxor: _LLE:
 return 1;default:
 return 0;};}
# 97
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts){
return(void*)({struct Cyc_AssnDef_Primop_AssnDef_Term_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->tag=4,_tmp26->f1=p,_tmp26->f2=ts;_tmp26;});}
# 100
void*Cyc_AssnDef_cast(void*tp,void*tm){
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->tag=5,_tmp27->f1=tp,_tmp27->f2=tm;_tmp27;});}
# 103
void*Cyc_AssnDef_fresh_var(void*t){
static int counter=0;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=3,_tmp28->f1=0,_tmp28->f2=counter ++,_tmp28->f3=t;_tmp28;});}struct _tuple11{void*f1;void*f2;};
# 108
int Cyc_AssnDef_cmp_term(void*t1,void*t2){
struct _tuple11 _tmp29=({struct _tuple11 _tmp109;_tmp109.f1=t1,_tmp109.f2=t2;_tmp109;});struct _tuple11 _stmttmp0=_tmp29;struct _tuple11 _tmp2A=_stmttmp0;enum Cyc_Absyn_Primop _tmp2D;enum Cyc_Absyn_Primop _tmp2B;void*_tmp30;void*_tmp2F;int _tmp32;int _tmp31;void*_tmp2E;void*_tmp2C;unsigned _tmp34;unsigned _tmp33;if(*((int*)_tmp2A.f1)== 0){if(*((int*)_tmp2A.f2)== 0){_tmp33=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_tmp2A.f1)->f1;_tmp34=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_tmp2A.f2)->f1;{unsigned i1=_tmp33;unsigned i2=_tmp34;
return(int)(i1 - i2);}}else{
return -1;}}else{if(*((int*)_tmp2A.f2)== 0)
return 1;else{if(*((int*)_tmp2A.f1)== 1){if(*((int*)_tmp2A.f2)== 1){_tmp2C=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2A.f1)->f1;_tmp2E=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2A.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2C;struct Cyc_Absyn_Exp*e2=_tmp2E;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{
return -1;}}else{if(*((int*)_tmp2A.f2)== 1)
return 1;else{if(*((int*)_tmp2A.f1)== 2){if(*((int*)_tmp2A.f2)== 2){_tmp2C=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2A.f1)->f1;_tmp2E=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2A.f2)->f1;{struct Cyc_Absyn_Vardecl*vd1=_tmp2C;struct Cyc_Absyn_Vardecl*vd2=_tmp2E;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_tmp2A.f2)== 2)
return 1;else{if(*((int*)_tmp2A.f1)== 3){if(*((int*)_tmp2A.f2)== 3){_tmp2C=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2A.f1)->f1;_tmp31=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2A.f1)->f2;_tmp2E=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2A.f2)->f1;_tmp32=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2A.f2)->f2;{struct Cyc_Absyn_Vardecl*vd1=_tmp2C;int i=_tmp31;struct Cyc_Absyn_Vardecl*vd2=_tmp2E;int j=_tmp32;
# 120
if(vd1 == vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_tmp2A.f2)== 3)
return 1;else{if(*((int*)_tmp2A.f1)== 5){if(*((int*)_tmp2A.f2)== 5){_tmp2C=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2A.f1)->f1;_tmp2E=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2A.f1)->f2;_tmp2F=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2A.f2)->f1;_tmp30=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2A.f2)->f2;{void*tp1=_tmp2C;void*tm1=_tmp2E;void*tp2=_tmp2F;void*tm2=_tmp30;
# 125
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c != 0)return c;
return Cyc_AssnDef_cmp_term(tm1,tm2);}}else{
return -1;}}else{if(*((int*)_tmp2A.f2)== 5)
return 1;else{_tmp2B=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2A.f1)->f1;_tmp2C=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2A.f1)->f2;_tmp2D=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2A.f2)->f1;_tmp2E=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2A.f2)->f2;{enum Cyc_Absyn_Primop p1=_tmp2B;struct Cyc_List_List*ts1=_tmp2C;enum Cyc_Absyn_Primop p2=_tmp2D;struct Cyc_List_List*ts2=_tmp2E;
# 131
if((int)p1 != (int)p2)return(int)p1 - (int)p2;
return({(int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_AssnDef_cmp_term,ts1,ts2);}}}}}}}}}}};}
# 136
static int Cyc_AssnDef_same_term(void*t1,void*t2){return Cyc_AssnDef_cmp_term(t1,t2)== 0;}
# 155
static unsigned Cyc_AssnDef_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 157
LOOP:
 if(({(int(*)(struct Cyc_List_List*,void*))Cyc_List_memq;})(*prev,a))return c;
({struct Cyc_List_List*_tmp141=({struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=a,_tmp35->tl=*prev;_tmp35;});*prev=_tmp141;});{
void*_tmp36=a;void*_tmp39;void*_tmp38;void*_tmp37;switch(*((int*)_tmp36)){case 0:
 return c;case 1:
 return c;case 2:
 return 1U + c;case 3: _tmp37=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp36)->f1;_tmp38=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp36)->f2;{void*a1=_tmp37;void*a2=_tmp38;
_tmp37=a1;_tmp38=a2;goto _LLA;}case 4: _tmp37=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp36)->f1;_tmp38=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp36)->f2;_LLA: {void*a1=_tmp37;void*a2=_tmp38;
# 166
c=Cyc_AssnDef_assn_size_loop(a2,1U + c,prev);
a=a1;
goto LOOP;}case 5: _tmp37=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp36)->f1;_tmp38=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp36)->f2;_tmp39=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp36)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp37;void*t=_tmp38;void*a1=_tmp39;
# 170
++ c;
a=a1;
goto LOOP;}default: _tmp37=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp36)->f1;{void*a1=_tmp37;
# 174
++ c;
a=a1;
goto LOOP;}};}}
# 183
static unsigned Cyc_AssnDef_max_assn_size=0U;
# 186
static unsigned Cyc_AssnDef_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned _tmp3A=Cyc_AssnDef_assn_size_loop(a,0U,& prev);unsigned c=_tmp3A;
if(c > Cyc_AssnDef_max_assn_size)
Cyc_AssnDef_max_assn_size=c;
# 192
return c;}
# 197
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec,int*size){
void*_tmp3B=a;void*_tmp3E;void*_tmp3D;enum Cyc_AssnDef_Primreln _tmp3F;void*_tmp3C;switch(*((int*)_tmp3B)){case 0:
 return({const char*_tmp40="true";_tag_fat(_tmp40,sizeof(char),5U);});case 1:
 return({const char*_tmp41="false";_tag_fat(_tmp41,sizeof(char),6U);});case 2: _tmp3C=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp3B)->f1;_tmp3F=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp3B)->f2;_tmp3D=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp3B)->f3;{void*t1=_tmp3C;enum Cyc_AssnDef_Primreln p=_tmp3F;void*t2=_tmp3D;
# 202
*size +=1;{
struct _fat_ptr ps;
{enum Cyc_AssnDef_Primreln _tmp42=p;switch((int)_tmp42){case Cyc_AssnDef_Eq:
 ps=({const char*_tmp43="==";_tag_fat(_tmp43,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_Neq:
 ps=({const char*_tmp44="!=";_tag_fat(_tmp44,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_SLt:
 ps=({const char*_tmp45=" S< ";_tag_fat(_tmp45,sizeof(char),5U);});goto _LLF;case Cyc_AssnDef_SLte:
 ps=({const char*_tmp46=" S<= ";_tag_fat(_tmp46,sizeof(char),6U);});goto _LLF;case Cyc_AssnDef_ULt:
 ps=({const char*_tmp47=" U< ";_tag_fat(_tmp47,sizeof(char),5U);});goto _LLF;case Cyc_AssnDef_ULte:
 ps=({const char*_tmp48=" U<= ";_tag_fat(_tmp48,sizeof(char),6U);});goto _LLF;default:
({void*_tmp49=0U;({int(*_tmp143)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp142=({const char*_tmp4A="assn2string primop";_tag_fat(_tmp4A,sizeof(char),19U);});_tmp143(_tmp142,_tag_fat(_tmp49,sizeof(void*),0));});});}_LLF:;}
# 213
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4D=({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0,({struct _fat_ptr _tmp144=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp10C.f1=_tmp144;});_tmp10C;});struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp10B;_tmp10B.tag=0,_tmp10B.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_tmp10B;});struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp10A;_tmp10A.tag=0,({struct _fat_ptr _tmp145=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp10A.f1=_tmp145;});_tmp10A;});void*_tmp4B[3];_tmp4B[0]=& _tmp4D,_tmp4B[1]=& _tmp4E,_tmp4B[2]=& _tmp4F;({struct _fat_ptr _tmp146=({const char*_tmp4C="%s%s%s";_tag_fat(_tmp4C,sizeof(char),7U);});Cyc_aprintf(_tmp146,_tag_fat(_tmp4B,sizeof(void*),3));});});}}case 4: _tmp3C=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp3B)->f1;_tmp3D=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp3B)->f2;{void*a1=_tmp3C;void*a2=_tmp3D;
# 215
*size +=1;
if(prec <= 5)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp52=({struct Cyc_String_pa_PrintArg_struct _tmp10E;_tmp10E.tag=0,({struct _fat_ptr _tmp147=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_tmp10E.f1=_tmp147;});_tmp10E;});struct Cyc_String_pa_PrintArg_struct _tmp53=({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0,({struct _fat_ptr _tmp148=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_tmp10D.f1=_tmp148;});_tmp10D;});void*_tmp50[2];_tmp50[0]=& _tmp52,_tmp50[1]=& _tmp53;({struct _fat_ptr _tmp149=({const char*_tmp51="%s || %s";_tag_fat(_tmp51,sizeof(char),9U);});Cyc_aprintf(_tmp149,_tag_fat(_tmp50,sizeof(void*),2));});});else{
# 219
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp56=({struct Cyc_String_pa_PrintArg_struct _tmp110;_tmp110.tag=0,({struct _fat_ptr _tmp14A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_tmp110.f1=_tmp14A;});_tmp110;});struct Cyc_String_pa_PrintArg_struct _tmp57=({struct Cyc_String_pa_PrintArg_struct _tmp10F;_tmp10F.tag=0,({struct _fat_ptr _tmp14B=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_tmp10F.f1=_tmp14B;});_tmp10F;});void*_tmp54[2];_tmp54[0]=& _tmp56,_tmp54[1]=& _tmp57;({struct _fat_ptr _tmp14C=({const char*_tmp55="(%s || %s)";_tag_fat(_tmp55,sizeof(char),11U);});Cyc_aprintf(_tmp14C,_tag_fat(_tmp54,sizeof(void*),2));});});}}case 3: _tmp3C=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp3B)->f1;_tmp3D=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp3B)->f2;{void*a1=_tmp3C;void*a2=_tmp3D;
# 221
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp112;_tmp112.tag=0,({struct _fat_ptr _tmp14D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_tmp112.f1=_tmp14D;});_tmp112;});struct Cyc_String_pa_PrintArg_struct _tmp5B=({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0,({struct _fat_ptr _tmp14E=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_tmp111.f1=_tmp14E;});_tmp111;});void*_tmp58[2];_tmp58[0]=& _tmp5A,_tmp58[1]=& _tmp5B;({struct _fat_ptr _tmp14F=({const char*_tmp59="%s && %s";_tag_fat(_tmp59,sizeof(char),9U);});Cyc_aprintf(_tmp14F,_tag_fat(_tmp58,sizeof(void*),2));});});}case 5: _tmp3C=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp3B)->f1;_tmp3D=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp3B)->f2;_tmp3E=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp3B)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp3C;void*t=_tmp3D;void*a=_tmp3E;
# 224
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5E=({struct Cyc_String_pa_PrintArg_struct _tmp115;_tmp115.tag=0,({struct _fat_ptr _tmp150=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp115.f1=_tmp150;});_tmp115;});struct Cyc_String_pa_PrintArg_struct _tmp5F=({struct Cyc_String_pa_PrintArg_struct _tmp114;_tmp114.tag=0,({
struct _fat_ptr _tmp151=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp114.f1=_tmp151;});_tmp114;});struct Cyc_String_pa_PrintArg_struct _tmp60=({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0,({struct _fat_ptr _tmp152=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_tmp113.f1=_tmp152;});_tmp113;});void*_tmp5C[3];_tmp5C[0]=& _tmp5E,_tmp5C[1]=& _tmp5F,_tmp5C[2]=& _tmp60;({struct _fat_ptr _tmp153=({const char*_tmp5D="Subst[%s:=%s](%s)";_tag_fat(_tmp5D,sizeof(char),18U);});Cyc_aprintf(_tmp153,_tag_fat(_tmp5C,sizeof(void*),3));});});}default: _tmp3C=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp3B)->f1;{void*a=_tmp3C;
# 228
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp116;_tmp116.tag=0,({struct _fat_ptr _tmp154=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_tmp116.f1=_tmp154;});_tmp116;});void*_tmp61[1];_tmp61[0]=& _tmp63;({struct _fat_ptr _tmp155=({const char*_tmp62="Kill(%s)";_tag_fat(_tmp62,sizeof(char),9U);});Cyc_aprintf(_tmp155,_tag_fat(_tmp61,sizeof(void*),1));});});}};}
# 236
int Cyc_AssnDef_tree_size=0;
# 239
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
Cyc_AssnDef_tree_size=0;
return Cyc_AssnDef_a2string(a,0,& Cyc_AssnDef_tree_size);}
# 245
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 256 "assndef.cyc"
static int Cyc_AssnDef_assnhash(void*a){
void*_tmp64=a;void*_tmp67;void*_tmp66;enum Cyc_AssnDef_Primreln _tmp68;void*_tmp65;switch(*((int*)_tmp64)){case 1:
 return 0;case 0:
 return 1;case 2: _tmp65=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64)->f1;_tmp68=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64)->f2;_tmp66=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64)->f3;{void*t1=_tmp65;enum Cyc_AssnDef_Primreln p=_tmp68;void*t2=_tmp66;
# 261
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3: _tmp65=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp64)->f1;_tmp66=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp64)->f2;{void*a1=_tmp65;void*a2=_tmp66;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4: _tmp65=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp64)->f1;_tmp66=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp64)->f2;{void*a1=_tmp65;void*a2=_tmp66;
return(int)((unsigned)a1 ^ (unsigned)a2 | 1U);}case 5: _tmp65=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64)->f1;_tmp66=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64)->f2;_tmp67=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp65;void*t=_tmp66;void*a=_tmp67;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | 3U);}default: _tmp65=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp64)->f1;{void*a=_tmp65;
# 266
return(int)((unsigned)a + 37U);}};}
# 273
int Cyc_AssnDef_assncmp(void*a1,void*a2){
LOOP:
 if(a1 == a2)return 0;{
struct _tuple11 _tmp69=({struct _tuple11 _tmp117;_tmp117.f1=a1,_tmp117.f2=a2;_tmp117;});struct _tuple11 _stmttmp1=_tmp69;struct _tuple11 _tmp6A=_stmttmp1;void*_tmp70;void*_tmp6F;enum Cyc_AssnDef_Primreln _tmp72;enum Cyc_AssnDef_Primreln _tmp71;void*_tmp6E;void*_tmp6D;void*_tmp6C;void*_tmp6B;if(*((int*)_tmp6A.f1)== 0){if(*((int*)_tmp6A.f2)== 0)
return 0;else{
return -1;}}else{if(*((int*)_tmp6A.f2)== 0)
return 1;else{if(*((int*)_tmp6A.f1)== 1){if(*((int*)_tmp6A.f2)== 1)
return 0;else{
return -1;}}else{if(*((int*)_tmp6A.f2)== 1)
return 1;else{switch(*((int*)_tmp6A.f1)){case 4: if(*((int*)_tmp6A.f2)== 4){_tmp6B=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp6A.f1)->f1;_tmp6C=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp6A.f1)->f2;_tmp6D=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp6A.f2)->f1;_tmp6E=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp6A.f2)->f2;{void*a11=_tmp6B;void*a12=_tmp6C;void*a21=_tmp6D;void*a22=_tmp6E;
_tmp6B=a11;_tmp6C=a12;_tmp6D=a21;_tmp6E=a22;goto _LL10;}}else{
# 290
return -1;}case 3: switch(*((int*)_tmp6A.f2)){case 3: _tmp6B=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp6A.f1)->f1;_tmp6C=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp6A.f1)->f2;_tmp6D=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp6A.f2)->f1;_tmp6E=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp6A.f2)->f2;_LL10: {void*a11=_tmp6B;void*a12=_tmp6C;void*a21=_tmp6D;void*a22=_tmp6E;
# 285
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c != 0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4: goto _LL13;default:
# 292
 return -1;}default: switch(*((int*)_tmp6A.f2)){case 4: _LL13:
# 291
 return 1;case 3:
# 293
 return 1;default: if(*((int*)_tmp6A.f1)== 2){if(*((int*)_tmp6A.f2)== 2){_tmp6B=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6A.f1)->f1;_tmp71=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6A.f1)->f2;_tmp6C=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6A.f1)->f3;_tmp6D=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6A.f2)->f1;_tmp72=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6A.f2)->f2;_tmp6E=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6A.f2)->f3;{void*t11=_tmp6B;enum Cyc_AssnDef_Primreln p1=_tmp71;void*t12=_tmp6C;void*t21=_tmp6D;enum Cyc_AssnDef_Primreln p2=_tmp72;void*t22=_tmp6E;
# 295
int c=(int)p1 - (int)p2;
if(c != 0)return c;
c=Cyc_AssnDef_cmp_term(t11,t21);
if(c != 0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{
return -1;}}else{if(*((int*)_tmp6A.f2)== 2)
return 1;else{if(*((int*)_tmp6A.f1)== 6){if(*((int*)_tmp6A.f2)== 6){_tmp6B=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp6A.f1)->f1;_tmp6C=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp6A.f2)->f1;{void*a1=_tmp6B;void*a2=_tmp6C;
return Cyc_AssnDef_assncmp(a1,a2);}}else{
return -1;}}else{if(*((int*)_tmp6A.f2)== 6)
return 1;else{_tmp6B=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6A.f1)->f1;_tmp6C=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6A.f1)->f2;_tmp6D=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6A.f1)->f3;_tmp6E=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6A.f2)->f1;_tmp6F=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6A.f2)->f2;_tmp70=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6A.f2)->f3;{struct Cyc_Absyn_Vardecl*vd1=_tmp6B;void*t1=_tmp6C;void*a11=_tmp6D;struct Cyc_Absyn_Vardecl*vd2=_tmp6E;void*t2=_tmp6F;void*a22=_tmp70;
# 306
int c=(int)vd1 - (int)vd2;
if(c != 0)return c;
c=Cyc_AssnDef_cmp_term(t1,t2);
if(c != 0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}};}}
# 322
static struct Cyc_Hashtable_Table*Cyc_AssnDef_hash_cons_table=0;
# 325
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_hash_cons_table=0;}struct _tuple12{void*f1;int f2;};
# 330
int Cyc_AssnDef_is_reduced(void*a){
if(Cyc_AssnDef_hash_cons_table == 0)return 0;{
struct _tuple12**_tmp73=({(struct _tuple12**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;})((struct Cyc_Hashtable_Table*)_check_null(Cyc_AssnDef_hash_cons_table),a);struct _tuple12**resopt=_tmp73;
if(resopt == 0)return 0;{
struct _tuple12*_tmp74=*resopt;struct _tuple12*_stmttmp2=_tmp74;struct _tuple12*_tmp75=_stmttmp2;int _tmp76;_tmp76=_tmp75->f2;{int b=_tmp76;
return b;}}}}
# 341
static struct _tuple12 Cyc_AssnDef_copy_assn(void*a){
void*_tmp77=a;void*_tmp7A;enum Cyc_AssnDef_Primreln _tmp7B;void*_tmp79;void*_tmp78;switch(*((int*)_tmp77)){case 0:
 return({struct _tuple12 _tmp118;_tmp118.f1=(void*)& Cyc_AssnDef_true_assn,_tmp118.f2=1;_tmp118;});case 1:
 return({struct _tuple12 _tmp119;_tmp119.f1=(void*)& Cyc_AssnDef_false_assn,_tmp119.f2=0;_tmp119;});case 3: _tmp78=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp77)->f1;_tmp79=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp77)->f2;{void*a1=_tmp78;void*a2=_tmp79;
# 346
return({struct _tuple12 _tmp11A;({void*_tmp157=(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=3,_tmp7C->f1=a1,_tmp7C->f2=a2;_tmp7C;});_tmp11A.f1=_tmp157;}),({int _tmp156=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_tmp11A.f2=_tmp156;});_tmp11A;});}case 4: _tmp78=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp77)->f1;_tmp79=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp77)->f2;{void*a1=_tmp78;void*a2=_tmp79;
# 348
return({struct _tuple12 _tmp11B;({void*_tmp159=(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=4,_tmp7D->f1=a1,_tmp7D->f2=a2;_tmp7D;});_tmp11B.f1=_tmp159;}),({int _tmp158=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_tmp11B.f2=_tmp158;});_tmp11B;});}case 2: _tmp78=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f1;_tmp7B=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f2;_tmp79=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f3;{void*t1=_tmp78;enum Cyc_AssnDef_Primreln p=_tmp7B;void*t2=_tmp79;
return({struct _tuple12 _tmp11C;({void*_tmp15A=(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=2,_tmp7E->f1=t1,_tmp7E->f2=p,_tmp7E->f3=t2;_tmp7E;});_tmp11C.f1=_tmp15A;}),_tmp11C.f2=1;_tmp11C;});}case 5: _tmp78=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp77)->f1;_tmp79=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp77)->f2;_tmp7A=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp77)->f3;{struct Cyc_Absyn_Vardecl*vd1=_tmp78;void*t1=_tmp79;void*a1=_tmp7A;
return({struct _tuple12 _tmp11D;({void*_tmp15B=(void*)({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=5,_tmp7F->f1=vd1,_tmp7F->f2=t1,_tmp7F->f3=a1;_tmp7F;});_tmp11D.f1=_tmp15B;}),_tmp11D.f2=0;_tmp11D;});}default: _tmp78=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp77)->f1;{void*a=_tmp78;
return({struct _tuple12 _tmp11E;({void*_tmp15C=(void*)({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=6,_tmp80->f1=a;_tmp80;});_tmp11E.f1=_tmp15C;}),_tmp11E.f2=0;_tmp11E;});}};}
# 359
static void*Cyc_AssnDef_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_hash_cons_table == 0){
h=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_create;})(123,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_hash_cons_table=h;
# 366
({void(*_tmp15E)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*)=({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp15D=h;_tmp15E(_tmp15D,(void*)& Cyc_AssnDef_true_assn,({struct _tuple12*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->f1=(void*)& Cyc_AssnDef_true_assn,_tmp81->f2=1;_tmp81;}));});
({void(*_tmp160)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*)=({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp15F=h;_tmp160(_tmp15F,(void*)& Cyc_AssnDef_false_assn,({struct _tuple12*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(void*)& Cyc_AssnDef_false_assn,_tmp82->f2=1;_tmp82;}));});}else{
# 369
h=(struct Cyc_Hashtable_Table*)_check_null(Cyc_AssnDef_hash_cons_table);}{
struct _tuple12**_tmp83=({(struct _tuple12**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);struct _tuple12**resopt=_tmp83;
if(resopt == 0){
struct _tuple12 _tmp84=Cyc_AssnDef_copy_assn(a);struct _tuple12 _stmttmp3=_tmp84;struct _tuple12 _tmp85=_stmttmp3;int _tmp87;void*_tmp86;_tmp86=_tmp85.f1;_tmp87=_tmp85.f2;{void*res=_tmp86;int reduced=_tmp87;
({void(*_tmp163)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*)=({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp162=h;void*_tmp161=res;_tmp163(_tmp162,_tmp161,({struct _tuple12*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=res,_tmp88->f2=reduced;_tmp88;}));});
return res;}}else{
# 376
struct _tuple12*_tmp89=*resopt;struct _tuple12*_stmttmp4=_tmp89;struct _tuple12*_tmp8A=_stmttmp4;void*_tmp8B;_tmp8B=_tmp8A->f1;{void*res=_tmp8B;
return res;}}}}
# 382
void*Cyc_AssnDef_kill(void*a){
void*_tmp8C=a;switch(*((int*)_tmp8C)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 6: _LL6:
 return a;default:  {
# 388
struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _tmp8D=({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _tmp11F;_tmp11F.tag=6,_tmp11F.f1=a;_tmp11F;});struct Cyc_AssnDef_Kill_AssnDef_Assn_struct p=_tmp8D;
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 394
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmp8E=({struct _tuple11 _tmp121;_tmp121.f1=a1,_tmp121.f2=a2;_tmp121;});struct _tuple11 _stmttmp5=_tmp8E;struct _tuple11 _tmp8F=_stmttmp5;if(*((int*)_tmp8F.f1)== 0)
return a2;else{if(*((int*)_tmp8F.f2)== 0)
return a1;else{if(*((int*)_tmp8F.f1)== 1)
return a1;else{if(*((int*)_tmp8F.f2)== 1)
return a2;else{
# 405
if((int)a1 > (int)a2)
({void*_tmp90=a1;void*_tmp91=a2;a1=_tmp91;a2=_tmp90;});{
struct Cyc_AssnDef_And_AssnDef_Assn_struct _tmp92=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _tmp120;_tmp120.tag=3,_tmp120.f1=a1,_tmp120.f2=a2;_tmp120;});struct Cyc_AssnDef_And_AssnDef_Assn_struct a=_tmp92;
return Cyc_AssnDef_hash_cons((void*)& a);}}}}};}}
# 413
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmp93=({struct _tuple11 _tmp123;_tmp123.f1=a1,_tmp123.f2=a2;_tmp123;});struct _tuple11 _stmttmp6=_tmp93;struct _tuple11 _tmp94=_stmttmp6;if(*((int*)_tmp94.f1)== 0)
return a1;else{if(*((int*)_tmp94.f2)== 0)
return a2;else{if(*((int*)_tmp94.f1)== 1)
return a2;else{if(*((int*)_tmp94.f2)== 1)
return a1;else{
# 422
if((int)a1 > (int)a2)
({void*_tmp95=a1;void*_tmp96=a2;a1=_tmp96;a2=_tmp95;});{
struct Cyc_AssnDef_Or_AssnDef_Assn_struct _tmp97=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _tmp122;_tmp122.tag=4,_tmp122.f1=a1,_tmp122.f2=a2;_tmp122;});struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=_tmp97;
void*_tmp98=Cyc_AssnDef_hash_cons((void*)& a);void*res=_tmp98;
return res;}}}}};}}
# 431
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
void*_tmp99=a;switch(*((int*)_tmp99)){case 0:
 goto _LL4;case 1: _LL4:
 return a;default:  {
# 436
struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _tmp9A=({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _tmp124;_tmp124.tag=5,_tmp124.f1=x,_tmp124.f2=newx,_tmp124.f3=a;_tmp124;});struct Cyc_AssnDef_Subst_AssnDef_Assn_struct p=_tmp9A;
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 442
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _tmp9B=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _tmp125;_tmp125.tag=2,_tmp125.f1=t1,_tmp125.f2=p,_tmp125.f3=t2;_tmp125;});struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=_tmp9B;
return Cyc_AssnDef_hash_cons((void*)& ptr);}
# 452
void*Cyc_AssnDef_eq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,0U,t2);}
void*Cyc_AssnDef_neq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,1U,t2);}
void*Cyc_AssnDef_slt(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,2U,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,3U,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,4U,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,5U,t2);}
# 469 "assndef.cyc"
void*Cyc_AssnDef_not(void*a){
void*_tmp9C=a;enum Cyc_AssnDef_Primreln _tmp9E;void*_tmp9F;void*_tmp9D;switch(*((int*)_tmp9C)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 3: _tmp9D=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp9C)->f1;_tmp9F=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp9C)->f2;{void*a1=_tmp9D;void*a2=_tmp9F;
return({void*_tmp164=Cyc_AssnDef_not(a1);Cyc_AssnDef_or(_tmp164,Cyc_AssnDef_not(a2));});}case 4: _tmp9D=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp9C)->f1;_tmp9F=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp9C)->f2;{void*a1=_tmp9D;void*a2=_tmp9F;
return({void*_tmp165=Cyc_AssnDef_not(a1);Cyc_AssnDef_and(_tmp165,Cyc_AssnDef_not(a2));});}case 2: _tmp9D=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp9C)->f1;_tmp9E=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp9C)->f2;_tmp9F=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp9C)->f3;{void*t1=_tmp9D;enum Cyc_AssnDef_Primreln p=_tmp9E;void*t2=_tmp9F;
# 476
enum Cyc_AssnDef_Primreln _tmpA0=p;switch((int)_tmpA0){case Cyc_AssnDef_Eq:
 return Cyc_AssnDef_neq(t1,t2);case Cyc_AssnDef_Neq:
 return Cyc_AssnDef_eq(t1,t2);case Cyc_AssnDef_SLt:
 return Cyc_AssnDef_slte(t2,t1);case Cyc_AssnDef_SLte:
 return Cyc_AssnDef_slt(t2,t1);case Cyc_AssnDef_ULt:
 return Cyc_AssnDef_ulte(t2,t1);case Cyc_AssnDef_ULte:
 return Cyc_AssnDef_ult(t2,t1);default:
({void*_tmpA1=0U;({int(*_tmp167)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp166=({const char*_tmpA2="Vcgen::invert: non-primop";_tag_fat(_tmpA2,sizeof(char),26U);});_tmp167(_tmp166,_tag_fat(_tmpA1,sizeof(void*),0));});});};}case 5:
# 485
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));default:
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));};}struct _tuple13{struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 508 "assndef.cyc"
static void*Cyc_AssnDef_reduce_term(void*t,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 511
void*_tmpA3=t;enum Cyc_Absyn_Primop _tmpA5;void*_tmpA6;void*_tmpA4;switch(*((int*)_tmpA3)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 3: _LL6:
 return t;case 5: _tmpA4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpA3)->f1;_tmpA6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpA3)->f2;{void*tp=_tmpA4;void*tm=_tmpA6;
# 516
void*_tmpA7=Cyc_AssnDef_reduce_term(tm,non_esc_subst,esc_subst);void*tm2=_tmpA7;
if(tm != tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 4: _tmpA5=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpA3)->f1;_tmpA4=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpA3)->f2;{enum Cyc_Absyn_Primop p=_tmpA5;struct Cyc_List_List*ts=_tmpA4;
# 520
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpA8=ts;struct Cyc_List_List*ptr=_tmpA8;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpA9=(void*)ptr->hd;void*old_term=_tmpA9;
void*_tmpAA=Cyc_AssnDef_reduce_term(old_term,non_esc_subst,esc_subst);void*new_term=_tmpAA;
res=({struct Cyc_List_List*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->hd=new_term,_tmpAB->tl=res;_tmpAB;});
if(new_term != old_term)change=1;}}
# 528
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp168=p;Cyc_AssnDef_primop(_tmp168,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res));});}default: _tmpA4=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmpA3)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpA4;
# 531
if(vd->escapes && esc_subst != 0){
# 534
struct Cyc_List_List*_tmpAC=*esc_subst;struct Cyc_List_List*s=_tmpAC;
for(1;s != 0;s=s->tl){
struct _tuple13 _tmpAD=*((struct _tuple13*)s->hd);struct _tuple13 _stmttmp7=_tmpAD;struct _tuple13 _tmpAE=_stmttmp7;void*_tmpB0;void*_tmpAF;_tmpAF=_tmpAE.f1;_tmpB0=_tmpAE.f2;{struct Cyc_Absyn_Vardecl*vd2=_tmpAF;void*t2=_tmpB0;
# 538
if(vd2 == vd)return t2;}}{
# 542
void*_tmpB1=Cyc_AssnDef_fresh_var(vd->type);void*t=_tmpB1;
({struct Cyc_List_List*_tmp16A=({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));({struct _tuple13*_tmp169=({struct _tuple13*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->f1=vd,_tmpB2->f2=t;_tmpB2;});_tmpB3->hd=_tmp169;}),_tmpB3->tl=*esc_subst;_tmpB3;});*esc_subst=_tmp16A;});
# 547
return t;}}else{
if(!vd->escapes)
# 550
for(1;non_esc_subst != 0;non_esc_subst=non_esc_subst->tl){
struct _tuple13 _tmpB4=*((struct _tuple13*)non_esc_subst->hd);struct _tuple13 _stmttmp8=_tmpB4;struct _tuple13 _tmpB5=_stmttmp8;void*_tmpB7;void*_tmpB6;_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;{struct Cyc_Absyn_Vardecl*vd2=_tmpB6;void*t2=_tmpB7;
if(vd == vd2)return t2;}}}
# 555
return t;}};}
# 559
static void*Cyc_AssnDef_reduce_assn(void*a,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 562
LOOP: {
void*_tmpB8=a;void*_tmpBB;void*_tmpBA;enum Cyc_AssnDef_Primreln _tmpBC;void*_tmpB9;switch(*((int*)_tmpB8)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 2: _tmpB9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpB8)->f1;_tmpBC=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpB8)->f2;_tmpBA=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpB8)->f3;{void*t1=_tmpB9;enum Cyc_AssnDef_Primreln p=_tmpBC;void*t2=_tmpBA;
# 567
void*_tmpBD=Cyc_AssnDef_reduce_term(t1,non_esc_subst,esc_subst);void*newt1=_tmpBD;
void*_tmpBE=Cyc_AssnDef_reduce_term(t2,non_esc_subst,esc_subst);void*newt2=_tmpBE;
if(t1 == newt1 && t2 == newt2)return a;
return Cyc_AssnDef_prim(newt1,p,newt2);}case 3: _tmpB9=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpB8)->f1;_tmpBA=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpB8)->f2;{void*a1=_tmpB9;void*a2=_tmpBA;
# 572
void*_tmpBF=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpBF;
void*_tmpC0=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpC0;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_AssnDef_and(newa1,newa2);}case 4: _tmpB9=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpB8)->f1;_tmpBA=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpB8)->f2;{void*a1=_tmpB9;void*a2=_tmpBA;
# 577
void*_tmpC1=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpC1;
void*_tmpC2=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpC2;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_AssnDef_or(newa1,newa2);}case 6: _tmpB9=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmpB8)->f1;{void*a=_tmpB9;
# 584
struct Cyc_List_List*new_esc_subst=0;
return Cyc_AssnDef_reduce_assn(a,non_esc_subst,& new_esc_subst);}default: _tmpB9=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpB8)->f1;_tmpBA=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpB8)->f2;_tmpBB=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpB8)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmpB9;void*t=_tmpBA;void*a1=_tmpBB;
# 588
t=Cyc_AssnDef_reduce_term(t,non_esc_subst,esc_subst);
non_esc_subst=({struct Cyc_List_List*_tmpC4=_cycalloc(sizeof(*_tmpC4));({struct _tuple13*_tmp16B=({struct _tuple13*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=vd,_tmpC3->f2=t;_tmpC3;});_tmpC4->hd=_tmp16B;}),_tmpC4->tl=non_esc_subst;_tmpC4;});
a=a1;
goto LOOP;}};}}
# 596
void*Cyc_AssnDef_reduce(void*a){
# 598
if(Cyc_AssnDef_is_reduced(a))return a;
return Cyc_AssnDef_reduce_assn(a,0,0);}
# 603
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_tmpC5=t;void*_tmpC7;enum Cyc_Absyn_Primop _tmpC8;void*_tmpC6;switch(*((int*)_tmpC5)){case 2: _tmpC6=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmpC5)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpC6;
# 606
if(vd == x)return newx;
return t;}case 4: _tmpC8=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpC5)->f1;_tmpC6=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpC5)->f2;{enum Cyc_Absyn_Primop p=_tmpC8;struct Cyc_List_List*ts=_tmpC6;
# 609
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpC9=ts;struct Cyc_List_List*ptr=_tmpC9;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpCA=(void*)ptr->hd;void*old_term=_tmpCA;
void*_tmpCB=Cyc_AssnDef_subst_term(old_term,x,newx);void*new_term=_tmpCB;
res=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=new_term,_tmpCC->tl=res;_tmpCC;});
if(new_term != old_term)change=1;}}
# 617
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp16C=p;Cyc_AssnDef_primop(_tmp16C,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res));});}case 5: _tmpC6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpC5)->f1;_tmpC7=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpC5)->f2;{void*tp=_tmpC6;void*tm=_tmpC7;
# 620
void*_tmpCD=Cyc_AssnDef_subst_term(tm,x,newx);void*tm2=_tmpCD;
if(tm != tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 0:
 goto _LLA;case 1: _LLA:
 goto _LLC;default: _LLC:
 return t;};}
# 630
void*Cyc_AssnDef_kill_mem_term(void*t){
struct Cyc_List_List*_tmpCE=0;struct Cyc_List_List*esc_subst=_tmpCE;
return Cyc_AssnDef_reduce_term(t,0,& esc_subst);}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 645 "assndef.cyc"
static void Cyc_AssnDef_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 649
while(1){
struct _tuple14 _tmpCF=({struct _tuple14 _tmp126;_tmp126.f1=a1s,_tmp126.f2=a2s;_tmp126;});struct _tuple14 _stmttmp9=_tmpCF;struct _tuple14 _tmpD0=_stmttmp9;if(_tmpD0.f1 == 0){if(_tmpD0.f2 == 0)
return;else{
# 653
({struct Cyc_List_List*_tmp16D=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_revappend;})(a2s,*res2);*res2=_tmp16D;});return;}}else{if(_tmpD0.f2 == 0){
# 652
({struct Cyc_List_List*_tmp16E=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_revappend;})(a1s,*res1);*res1=_tmp16E;});return;}else{
# 655
int c=({void*_tmp16F=(void*)((struct Cyc_List_List*)_check_null(a1s))->hd;Cyc_AssnDef_assncmp(_tmp16F,(void*)((struct Cyc_List_List*)_check_null(a2s))->hd);});
if(c == 0){
({struct Cyc_List_List*_tmp170=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=(void*)a1s->hd,_tmpD1->tl=*common;_tmpD1;});*common=_tmp170;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_tmp171=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=(void*)a1s->hd,_tmpD2->tl=*res1;_tmpD2;});*res1=_tmp171;});
a1s=a1s->tl;}else{
# 664
({struct Cyc_List_List*_tmp172=({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=(void*)a2s->hd,_tmpD3->tl=*res2;_tmpD3;});*res2=_tmp172;});
a2s=a2s->tl;}}}};}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 671
struct _tuple15 Cyc_AssnDef_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 673
struct Cyc_List_List*_tmpD4=0;struct Cyc_List_List*common=_tmpD4;
struct Cyc_List_List*_tmpD5=0;struct Cyc_List_List*res1=_tmpD5;
struct Cyc_List_List*_tmpD6=0;struct Cyc_List_List*res2=_tmpD6;
Cyc_AssnDef_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple15 _tmp127;({struct Cyc_List_List*_tmp175=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(common);_tmp127.f1=_tmp175;}),({struct Cyc_List_List*_tmp174=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res1);_tmp127.f2=_tmp174;}),({struct Cyc_List_List*_tmp173=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res2);_tmp127.f3=_tmp173;});_tmp127;});}struct _tuple16{struct Cyc_List_List*f1;void*f2;};
# 682
struct _tuple16 Cyc_AssnDef_factor(void*a){
void*_tmpD7=a;void*_tmpD9;void*_tmpD8;switch(*((int*)_tmpD7)){case 0:
 return({struct _tuple16 _tmp128;_tmp128.f1=0,_tmp128.f2=(void*)& Cyc_AssnDef_true_assn;_tmp128;});case 1:
 return({struct _tuple16 _tmp129;_tmp129.f1=0,_tmp129.f2=(void*)& Cyc_AssnDef_false_assn;_tmp129;});case 2:
 return({struct _tuple16 _tmp12A;({struct Cyc_List_List*_tmp176=({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->hd=a,_tmpDA->tl=0;_tmpDA;});_tmp12A.f1=_tmp176;}),_tmp12A.f2=(void*)& Cyc_AssnDef_true_assn;_tmp12A;});case 3: _tmpD8=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpD7)->f1;_tmpD9=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpD7)->f2;{void*a1=_tmpD8;void*a2=_tmpD9;
# 688
struct _tuple16 _tmpDB=Cyc_AssnDef_factor(a1);struct _tuple16 _stmttmpA=_tmpDB;struct _tuple16 _tmpDC=_stmttmpA;void*_tmpDE;void*_tmpDD;_tmpDD=_tmpDC.f1;_tmpDE=_tmpDC.f2;{struct Cyc_List_List*c1s=_tmpDD;void*a1=_tmpDE;
struct _tuple16 _tmpDF=Cyc_AssnDef_factor(a2);struct _tuple16 _stmttmpB=_tmpDF;struct _tuple16 _tmpE0=_stmttmpB;void*_tmpE2;void*_tmpE1;_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;{struct Cyc_List_List*c2s=_tmpE1;void*a2=_tmpE2;
return({struct _tuple16 _tmp12B;({struct Cyc_List_List*_tmp178=({(struct Cyc_List_List*(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_merge;})(Cyc_AssnDef_assncmp,c1s,c2s);_tmp12B.f1=_tmp178;}),({void*_tmp177=Cyc_AssnDef_and(a1,a2);_tmp12B.f2=_tmp177;});_tmp12B;});}}}case 4: _tmpD8=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpD7)->f1;_tmpD9=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpD7)->f2;{void*a1=_tmpD8;void*a2=_tmpD9;
# 692
struct _tuple16 _tmpE3=Cyc_AssnDef_factor(a1);struct _tuple16 _stmttmpC=_tmpE3;struct _tuple16 _tmpE4=_stmttmpC;void*_tmpE6;void*_tmpE5;_tmpE5=_tmpE4.f1;_tmpE6=_tmpE4.f2;{struct Cyc_List_List*c1s=_tmpE5;void*a1=_tmpE6;
struct _tuple16 _tmpE7=Cyc_AssnDef_factor(a2);struct _tuple16 _stmttmpD=_tmpE7;struct _tuple16 _tmpE8=_stmttmpD;void*_tmpEA;void*_tmpE9;_tmpE9=_tmpE8.f1;_tmpEA=_tmpE8.f2;{struct Cyc_List_List*c2s=_tmpE9;void*a2=_tmpEA;
struct _tuple15 _tmpEB=Cyc_AssnDef_intersect_assns(c1s,c2s);struct _tuple15 _stmttmpE=_tmpEB;struct _tuple15 _tmpEC=_stmttmpE;void*_tmpEF;void*_tmpEE;void*_tmpED;_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;_tmpEF=_tmpEC.f3;{struct Cyc_List_List*cs=_tmpED;struct Cyc_List_List*c1s=_tmpEE;struct Cyc_List_List*c2s=_tmpEF;
return({struct _tuple16 _tmp12C;_tmp12C.f1=cs,({void*_tmp17A=({void*_tmp179=({(void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left;})(Cyc_AssnDef_and,a1,c1s);Cyc_AssnDef_or(_tmp179,({(void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left;})(Cyc_AssnDef_and,a2,c2s));});_tmp12C.f2=_tmp17A;});_tmp12C;});}}}}default:
 return({struct _tuple16 _tmp12D;_tmp12D.f1=0,_tmp12D.f2=a;_tmp12D;});};}
# 700
void*Cyc_AssnDef_factor_assn(void*a){
# 702
struct _tuple16 _tmpF0=Cyc_AssnDef_factor(a);struct _tuple16 _stmttmpF=_tmpF0;struct _tuple16 _tmpF1=_stmttmpF;void*_tmpF3;void*_tmpF2;_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;{struct Cyc_List_List*cs=_tmpF2;void*a=_tmpF3;
a=({(void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left;})(Cyc_AssnDef_and,a,cs);
# 707
return a;}}struct _tuple17{enum Cyc_AssnDef_Primreln f1;enum Cyc_AssnDef_Primreln f2;};
# 714
int Cyc_AssnDef_find_ctxt(void*ctxt,void*t1,enum Cyc_AssnDef_Primreln p1,void*u1){
LOOP: {
void*_tmpF4=ctxt;enum Cyc_AssnDef_Primreln _tmpF6;void*_tmpF7;void*_tmpF5;switch(*((int*)_tmpF4)){case 1:
 return 1;case 0:
 return 0;case 3: _tmpF5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpF4)->f1;_tmpF7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpF4)->f2;{void*a1=_tmpF5;void*a2=_tmpF7;
# 720
if(Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 1;
ctxt=a2;
goto LOOP;}case 4: _tmpF5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpF4)->f1;_tmpF7=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpF4)->f2;{void*a1=_tmpF5;void*a2=_tmpF7;
# 724
if(!Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 0;
ctxt=a2;
goto LOOP;}case 2: _tmpF5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF4)->f1;_tmpF6=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF4)->f2;_tmpF7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF4)->f3;{void*t2=_tmpF5;enum Cyc_AssnDef_Primreln p2=_tmpF6;void*u2=_tmpF7;
# 728
struct _tuple17 _tmpF8=({struct _tuple17 _tmp12E;_tmp12E.f1=p2,_tmp12E.f2=p1;_tmp12E;});struct _tuple17 _stmttmp10=_tmpF8;struct _tuple17 _tmpF9=_stmttmp10;switch((int)_tmpF9.f1){case Cyc_AssnDef_Neq: if(_tmpF9.f2 == Cyc_AssnDef_Neq)
goto _LL11;else{goto _LL26;}case Cyc_AssnDef_Eq: switch((int)_tmpF9.f2){case Cyc_AssnDef_Eq: _LL11:
 goto _LL13;case Cyc_AssnDef_SLte: _LL13:
 goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 733
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);default: goto _LL26;}case Cyc_AssnDef_SLt: switch((int)_tmpF9.f2){case Cyc_AssnDef_Neq:
 goto _LL19;case Cyc_AssnDef_SLt:
# 739
 goto _LL1D;case Cyc_AssnDef_SLte: _LL1F:
# 741
 goto _LL21;default: goto _LL26;}case Cyc_AssnDef_ULt: switch((int)_tmpF9.f2){case Cyc_AssnDef_Neq: _LL19:
# 737
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);case Cyc_AssnDef_ULt: _LL1D:
# 740
 goto _LL1F;case Cyc_AssnDef_ULte: _LL21:
# 742
 goto _LL23;default: goto _LL26;}case Cyc_AssnDef_SLte: if(_tmpF9.f2 == Cyc_AssnDef_SLte){_LL23:
 goto _LL25;}else{goto _LL26;}case Cyc_AssnDef_ULte: if(_tmpF9.f2 == Cyc_AssnDef_ULte){_LL25:
# 745
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2);}else{goto _LL26;}default: _LL26:
 return 0;};}default:
# 749
 ctxt=Cyc_AssnDef_reduce(ctxt);
goto LOOP;};}}
# 756
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
LOOP:
 if(ctxt == a)return 1;{
void*_tmpFA=a;enum Cyc_AssnDef_Primreln _tmpFC;void*_tmpFD;void*_tmpFB;switch(*((int*)_tmpFA)){case 0:
 return 1;case 1:
 return 0;case 3: _tmpFB=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpFA)->f1;_tmpFD=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpFA)->f2;{void*a1=_tmpFB;void*a2=_tmpFD;
# 763
if(!Cyc_AssnDef_simple_prove(ctxt,a1))return 0;
a=a2;
goto LOOP;}case 4: _tmpFB=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpFA)->f1;_tmpFD=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpFA)->f2;{void*a1=_tmpFB;void*a2=_tmpFD;
# 767
if(Cyc_AssnDef_simple_prove(ctxt,a1))return 1;
a=a2;
goto LOOP;}case 2: _tmpFB=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpFA)->f1;_tmpFC=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpFA)->f2;_tmpFD=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpFA)->f3;{void*t1=_tmpFB;enum Cyc_AssnDef_Primreln p=_tmpFC;void*t2=_tmpFD;
return Cyc_AssnDef_find_ctxt(ctxt,t1,p,t2);}default:
# 772
 a=Cyc_AssnDef_reduce(a);
goto LOOP;};}}

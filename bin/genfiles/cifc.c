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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 349
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1079 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1127
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1133
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name);
# 1135
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl);
# 1139
int Cyc_Absyn_equal_att(void*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
# 94
void*Cyc_Tcutil_copy_type(void*);
# 37 "cifc.cyc"
static int Cyc_Cifc_glob_tvar_id=0;
# 40
struct _fat_ptr Cyc_Cifc_list2string(struct Cyc_List_List*l,struct _fat_ptr(*tostr)(void*)){
struct _fat_ptr _tmp0=({void*_tmp8=0U;({struct _fat_ptr _tmp131=({const char*_tmp9="(";_tag_fat(_tmp9,sizeof(char),2U);});Cyc_aprintf(_tmp131,_tag_fat(_tmp8,sizeof(void*),0U));});});struct _fat_ptr ret=_tmp0;
while((unsigned)l){
ret=({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0U,_tmp10D.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_tmp10D;});struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0U,({struct _fat_ptr _tmp132=(struct _fat_ptr)((struct _fat_ptr)((struct _fat_ptr(*)(void*))_check_null(tostr))(l->hd));_tmp10C.f1=_tmp132;});_tmp10C;});void*_tmp1[2U];_tmp1[0]=& _tmp3,_tmp1[1]=& _tmp4;({struct _fat_ptr _tmp133=({const char*_tmp2="%s %s,";_tag_fat(_tmp2,sizeof(char),7U);});Cyc_aprintf(_tmp133,_tag_fat(_tmp1,sizeof(void*),2U));});});
l=l->tl;}
# 46
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp10E;_tmp10E.tag=0U,_tmp10E.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_tmp10E;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct _fat_ptr _tmp134=({const char*_tmp6="%s)";_tag_fat(_tmp6,sizeof(char),4U);});Cyc_aprintf(_tmp134,_tag_fat(_tmp5,sizeof(void*),1U));});});}
# 51
static void Cyc_Cifc_fail_merge(int warn,unsigned loc,int is_buildlib,struct _tuple0*n,struct _fat_ptr s){
# 53
if(is_buildlib){
struct _fat_ptr preamble=warn?({const char*_tmpF="Warning: user-defined";_tag_fat(_tmpF,sizeof(char),22U);}):({const char*_tmp10="User-defined";_tag_fat(_tmp10,sizeof(char),13U);});
({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0U,_tmp111.f1=(struct _fat_ptr)((struct _fat_ptr)preamble);_tmp111;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp110;_tmp110.tag=0U,({
# 57
struct _fat_ptr _tmp135=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n));_tmp110.f1=_tmp135;});_tmp110;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp10F;_tmp10F.tag=0U,_tmp10F.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp10F;});void*_tmpA[3U];_tmpA[0]=& _tmpC,_tmpA[1]=& _tmpD,_tmpA[2]=& _tmpE;({struct Cyc___cycFILE*_tmp137=Cyc_stderr;struct _fat_ptr _tmp136=({const char*_tmpB="%s type for %s incompatible with system version %s\n";_tag_fat(_tmpB,sizeof(char),52U);});Cyc_fprintf(_tmp137,_tmp136,_tag_fat(_tmpA,sizeof(void*),3U));});});}else{
# 59
if(warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp115;_tmp115.tag=0U,({struct _fat_ptr _tmp138=({const char*_tmp17="User-defined type for ";_tag_fat(_tmp17,sizeof(char),23U);});_tmp115.f1=_tmp138;});_tmp115;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp13=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp114;_tmp114.tag=1U,_tmp114.f1=n;_tmp114;});struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp113;_tmp113.tag=0U,({struct _fat_ptr _tmp139=({const char*_tmp16=" incompatible with system version ";_tag_fat(_tmp16,sizeof(char),35U);});_tmp113.f1=_tmp139;});_tmp113;});struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp112;_tmp112.tag=0U,_tmp112.f1=s;_tmp112;});void*_tmp11[4U];_tmp11[0]=& _tmp12,_tmp11[1]=& _tmp13,_tmp11[2]=& _tmp14,_tmp11[3]=& _tmp15;({unsigned _tmp13A=loc;Cyc_Warn_warn2(_tmp13A,_tag_fat(_tmp11,sizeof(void*),4U));});});else{
# 62
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _tmp119;_tmp119.tag=0U,({struct _fat_ptr _tmp13B=({const char*_tmp1E="User-defined type for ";_tag_fat(_tmp1E,sizeof(char),23U);});_tmp119.f1=_tmp13B;});_tmp119;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp118;_tmp118.tag=1U,_tmp118.f1=n;_tmp118;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp117;_tmp117.tag=0U,({struct _fat_ptr _tmp13C=({const char*_tmp1D=" incompatible with system version ";_tag_fat(_tmp1D,sizeof(char),35U);});_tmp117.f1=_tmp13C;});_tmp117;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp116;_tmp116.tag=0U,_tmp116.f1=s;_tmp116;});void*_tmp18[4U];_tmp18[0]=& _tmp19,_tmp18[1]=& _tmp1A,_tmp18[2]=& _tmp1B,_tmp18[3]=& _tmp1C;({unsigned _tmp13D=loc;Cyc_Warn_err2(_tmp13D,_tag_fat(_tmp18,sizeof(void*),4U));});});}}}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 67
static void*Cyc_Cifc_expand_c_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp){
# 69
void*_tmp1F=ctyp;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp21;void**_tmp23;struct Cyc_Absyn_TypeDecl*_tmp22;struct Cyc_List_List*_tmp25;enum Cyc_Absyn_AggrKind _tmp24;struct Cyc_List_List*_tmp26;struct Cyc_Absyn_ArrayInfo*_tmp27;struct Cyc_Absyn_PtrInfo*_tmp28;struct Cyc_Absyn_FnInfo*_tmp29;switch(*((int*)_tmp1F)){case 8U: _LL1: _LL2:
# 72
 Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
{void*_tmp2A=ctyp;void*_tmp2B;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2A)->tag == 8U){_LL1A: _tmp2B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2A)->f4;_LL1B: {void*to=_tmp2B;
# 76
return(void*)_check_null(to);}}else{_LL1C: _LL1D:
# 78
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp11A;_tmp11A.tag=0U,({struct _fat_ptr _tmp13E=({const char*_tmp2E="Impos";_tag_fat(_tmp2E,sizeof(char),6U);});_tmp11A.f1=_tmp13E;});_tmp11A;});void*_tmp2C[1U];_tmp2C[0]=& _tmp2D;({unsigned _tmp13F=loc;Cyc_Warn_err2(_tmp13F,_tag_fat(_tmp2C,sizeof(void*),1U));});});
goto _LL19;}_LL19:;}
# 81
return ctyp;case 5U: _LL3: _tmp29=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F)->f1;_LL4: {struct Cyc_Absyn_FnInfo*finfo=_tmp29;
# 83
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 85
({void*_tmp142=({unsigned _tmp141=loc;struct Cyc_Tcenv_Tenv*_tmp140=te;Cyc_Cifc_expand_c_type(_tmp141,_tmp140,Cyc_Toc_typ_to_c(finfo->ret_type));});finfo->ret_type=_tmp142;});{
struct Cyc_List_List*_tmp2F=finfo->args;struct Cyc_List_List*args=_tmp2F;
while((unsigned)args){
struct _tuple8*_tmp30=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp0=_tmp30;struct _tuple8*_tmp31=_stmttmp0;void**_tmp32;_LL1F: _tmp32=(void**)& _tmp31->f3;_LL20: {void**argType=_tmp32;
({void*_tmp145=({unsigned _tmp144=loc;struct Cyc_Tcenv_Tenv*_tmp143=te;Cyc_Cifc_expand_c_type(_tmp144,_tmp143,Cyc_Toc_typ_to_c(*argType));});*argType=_tmp145;});
args=args->tl;}}
# 93
finfo->tvars=0;
finfo->effect=0;
return ctyp;}}case 3U: _LL5: _tmp28=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1;_LL6: {struct Cyc_Absyn_PtrInfo*pinfo=_tmp28;
# 99
({void*_tmp148=({unsigned _tmp147=loc;struct Cyc_Tcenv_Tenv*_tmp146=te;Cyc_Cifc_expand_c_type(_tmp147,_tmp146,Cyc_Toc_typ_to_c(pinfo->elt_type));});pinfo->elt_type=_tmp148;});
return ctyp;}case 4U: _LL7: _tmp27=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F)->f1;_LL8: {struct Cyc_Absyn_ArrayInfo*ainfo=_tmp27;
# 102
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 104
({void*_tmp14B=({unsigned _tmp14A=loc;struct Cyc_Tcenv_Tenv*_tmp149=te;Cyc_Cifc_expand_c_type(_tmp14A,_tmp149,Cyc_Toc_typ_to_c(ainfo->elt_type));});ainfo->elt_type=_tmp14B;});
return ctyp;}case 6U: _LL9: _tmp26=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F)->f1;_LLA: {struct Cyc_List_List*tlst=_tmp26;
# 107
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 109
while((unsigned)tlst){
struct _tuple11*_tmp33=(struct _tuple11*)tlst->hd;struct _tuple11*_stmttmp1=_tmp33;struct _tuple11*_tmp34=_stmttmp1;void**_tmp36;struct Cyc_Absyn_Tqual _tmp35;_LL22: _tmp35=_tmp34->f1;_tmp36=(void**)& _tmp34->f2;_LL23: {struct Cyc_Absyn_Tqual tq=_tmp35;void**elt_type=_tmp36;
({void*_tmp14E=({unsigned _tmp14D=loc;struct Cyc_Tcenv_Tenv*_tmp14C=te;Cyc_Cifc_expand_c_type(_tmp14D,_tmp14C,Cyc_Toc_typ_to_c(*elt_type));});*elt_type=_tmp14E;});
tlst=tlst->tl;}}
# 115
return ctyp;}case 7U: _LLB: _tmp24=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F)->f1;_tmp25=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F)->f2;_LLC: {enum Cyc_Absyn_AggrKind knd=_tmp24;struct Cyc_List_List*flst=_tmp25;
# 118
return ctyp;}case 10U: _LLD: _tmp22=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F)->f1;_tmp23=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F)->f2;_LLE: {struct Cyc_Absyn_TypeDecl*td=_tmp22;void**tptr=_tmp23;
# 121
return ctyp;}case 9U: _LLF: _tmp21=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1F)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp21;
# 124
return ctyp;}case 11U: _LL11: _tmp20=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1F)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp20;
# 127
return ctyp;}case 0U: _LL13: _LL14:
# 130
 return ctyp;case 1U: _LL15: _LL16:
# 133
 return ctyp;default: _LL17: _LL18:
# 136
 return ctyp;}_LL0:;}
# 144
static int Cyc_Cifc_c_types_ok(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp,void*cyctyp){
struct Cyc_Absyn_Tqual tq;
# 147
void*_tmp37=Cyc_Toc_typ_to_c(Cyc_Tcutil_copy_type(ctyp));void*c_typ=_tmp37;
void*_tmp38=Cyc_Toc_typ_to_c(Cyc_Tcutil_copy_type(cyctyp));void*u_typ=_tmp38;
if(Cyc_Tcutil_is_function_type(c_typ)&& Cyc_Tcutil_is_function_type(u_typ)){
c_typ=Cyc_Cifc_expand_c_type(loc,te,c_typ);
u_typ=Cyc_Cifc_expand_c_type(loc,te,u_typ);}
# 158
return Cyc_Unify_unify(c_typ,u_typ);}
# 162
static struct Cyc_List_List*Cyc_Cifc_merge_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
struct Cyc_List_List*x=0;
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))x=({struct Cyc_List_List*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->hd=(void*)a->hd,_tmp39->tl=x;_tmp39;});}}
return({struct Cyc_List_List*_tmp14F=x;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp14F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(a2));});}struct _tuple12{void*f1;void*f2;};struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 172
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,struct Cyc_Tcenv_Tenv*te,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
# 174
struct _tuple12 _tmp3A=({struct _tuple12 _tmp12B;_tmp12B.f1=c_decl->r,_tmp12B.f2=user_decl->r;_tmp12B;});struct _tuple12 _stmttmp2=_tmp3A;struct _tuple12 _tmp3B=_stmttmp2;struct Cyc_Absyn_Typedefdecl*_tmp3D;struct Cyc_Absyn_Typedefdecl*_tmp3C;struct Cyc_Absyn_Enumdecl*_tmp3F;struct Cyc_Absyn_Enumdecl*_tmp3E;struct Cyc_Absyn_Aggrdecl*_tmp41;struct Cyc_Absyn_Aggrdecl*_tmp40;struct Cyc_Absyn_Vardecl*_tmp43;struct Cyc_Absyn_Fndecl*_tmp42;struct Cyc_Absyn_Vardecl*_tmp45;struct Cyc_Absyn_Vardecl*_tmp44;switch(*((int*)_tmp3B.f1)){case 0U: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->tag == 0U){_LL1: _tmp44=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3B.f1)->f1;_tmp45=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->f1;_LL2: {struct Cyc_Absyn_Vardecl*cd=_tmp44;struct Cyc_Absyn_Vardecl*ud=_tmp45;
# 176
if(!Cyc_Cifc_c_types_ok(loc,te,cd->type,ud->type)){
({unsigned _tmp155=loc;int _tmp154=is_buildlib;struct _tuple0*_tmp153=cd->name;Cyc_Cifc_fail_merge(0,_tmp155,_tmp154,_tmp153,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp48=({struct Cyc_String_pa_PrintArg_struct _tmp11C;_tmp11C.tag=0U,({struct _fat_ptr _tmp150=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_tmp11C.f1=_tmp150;});_tmp11C;});struct Cyc_String_pa_PrintArg_struct _tmp49=({struct Cyc_String_pa_PrintArg_struct _tmp11B;_tmp11B.tag=0U,({struct _fat_ptr _tmp151=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cd->type));_tmp11B.f1=_tmp151;});_tmp11B;});void*_tmp46[2U];_tmp46[0]=& _tmp48,_tmp46[1]=& _tmp49;({struct _fat_ptr _tmp152=({const char*_tmp47=": type %s != %s";_tag_fat(_tmp47,sizeof(char),16U);});Cyc_aprintf(_tmp152,_tag_fat(_tmp46,sizeof(void*),2U));});}));});if(!0)return;}else{
# 184
if(ud->attributes != 0)
({struct Cyc_List_List*_tmp156=Cyc_Cifc_merge_attributes(cd->attributes,ud->attributes);cd->attributes=_tmp156;});
cd->type=ud->type;
cd->tq=ud->tq;}
# 189
goto _LL0;}}else{goto _LLB;}case 1U: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->tag == 0U){_LL3: _tmp42=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp3B.f1)->f1;_tmp43=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->f1;_LL4: {struct Cyc_Absyn_Fndecl*cd=_tmp42;struct Cyc_Absyn_Vardecl*ud=_tmp43;
# 192
if(!Cyc_Tcutil_is_function_type(ud->type)){
({unsigned _tmp159=loc;int _tmp158=is_buildlib;struct _tuple0*_tmp157=ud->name;Cyc_Cifc_fail_merge(0,_tmp159,_tmp158,_tmp157,({const char*_tmp4A=": type must be a function type to match decl\n";_tag_fat(_tmp4A,sizeof(char),46U);}));});if(!0)return;}{
# 195
void*cdtype;
if(cd->cached_type != 0)
cdtype=(void*)_check_null(cd->cached_type);else{
# 199
cdtype=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=5U,_tmp4B->f1=cd->i;_tmp4B;});}
if(!Cyc_Cifc_c_types_ok(loc,te,cdtype,ud->type)){
({unsigned _tmp15F=loc;int _tmp15E=is_buildlib;struct _tuple0*_tmp15D=ud->name;Cyc_Cifc_fail_merge(0,_tmp15F,_tmp15E,_tmp15D,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0U,({struct _fat_ptr _tmp15A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cdtype));_tmp11E.f1=_tmp15A;});_tmp11E;});struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0U,({struct _fat_ptr _tmp15B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_tmp11D.f1=_tmp15B;});_tmp11D;});void*_tmp4C[2U];_tmp4C[0]=& _tmp4E,_tmp4C[1]=& _tmp4F;({struct _fat_ptr _tmp15C=({const char*_tmp4D=": type %s != %s";_tag_fat(_tmp4D,sizeof(char),16U);});Cyc_aprintf(_tmp15C,_tag_fat(_tmp4C,sizeof(void*),2U));});}));});if(!0)return;}else{
# 208
void*_tmp50=ud->type;void*_stmttmp3=_tmp50;void*_tmp51=_stmttmp3;struct Cyc_Absyn_FnInfo _tmp52;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp51)->tag == 5U){_LLE: _tmp52=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp51)->f1;_LLF: {struct Cyc_Absyn_FnInfo fi=_tmp52;
# 210
struct Cyc_List_List*_tmp53=fi.tvars;struct Cyc_List_List*old_tvars=_tmp53;
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ud->type);
if((cd->i).attributes != 0)
({struct Cyc_List_List*_tmp160=Cyc_Cifc_merge_attributes(fi.attributes,(cd->i).attributes);fi.attributes=_tmp160;});
cd->i=fi;
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp55=({struct Cyc_Warn_String_Warn_Warg_struct _tmp11F;_tmp11F.tag=0U,({struct _fat_ptr _tmp161=({const char*_tmp56="oops!\n";_tag_fat(_tmp56,sizeof(char),7U);});_tmp11F.f1=_tmp161;});_tmp11F;});void*_tmp54[1U];_tmp54[0]=& _tmp55;Cyc_Warn_err2(0U,_tag_fat(_tmp54,sizeof(void*),1U));});}_LLD:;}
# 219
goto _LL0;}}}else{goto _LLB;}case 5U: if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->tag == 5U){_LL5: _tmp40=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3B.f1)->f1;_tmp41=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*cd=_tmp40;struct Cyc_Absyn_Aggrdecl*ud=_tmp41;
# 227
if((int)ud->sc != (int)cd->sc){
({unsigned _tmp164=loc;int _tmp163=is_buildlib;struct _tuple0*_tmp162=cd->name;Cyc_Cifc_fail_merge(1,_tmp164,_tmp163,_tmp162,({const char*_tmp57=": scopes don't match (ignoring)";_tag_fat(_tmp57,sizeof(char),32U);}));});if(!1)return;}
# 230
if(ud->impl == 0){
({unsigned _tmp167=loc;int _tmp166=is_buildlib;struct _tuple0*_tmp165=cd->name;Cyc_Cifc_fail_merge(0,_tmp167,_tmp166,_tmp165,({const char*_tmp58=": no user definition";_tag_fat(_tmp58,sizeof(char),21U);}));});if(!0)return;}
if(cd->impl == 0){
({unsigned _tmp16A=loc;int _tmp169=is_buildlib;struct _tuple0*_tmp168=cd->name;Cyc_Cifc_fail_merge(1,_tmp16A,_tmp169,_tmp168,({const char*_tmp59=": no definition for system version";_tag_fat(_tmp59,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 237
{struct _tuple13 _tmp5A=({struct _tuple13 _tmp125;_tmp125.f1=cd->impl,_tmp125.f2=ud->impl;_tmp125;});struct _tuple13 _stmttmp4=_tmp5A;struct _tuple13 _tmp5B=_stmttmp4;int _tmp62;struct Cyc_List_List*_tmp61;struct Cyc_List_List*_tmp60;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*_tmp5E;struct Cyc_List_List**_tmp5D;struct Cyc_List_List**_tmp5C;if(_tmp5B.f1 != 0){if(_tmp5B.f2 != 0){_LL13: _tmp5C=(struct Cyc_List_List**)&(_tmp5B.f1)->exist_vars;_tmp5D=(struct Cyc_List_List**)&(_tmp5B.f1)->rgn_po;_tmp5E=(_tmp5B.f1)->fields;_tmp5F=(_tmp5B.f2)->exist_vars;_tmp60=(_tmp5B.f2)->rgn_po;_tmp61=(_tmp5B.f2)->fields;_tmp62=(_tmp5B.f2)->tagged;_LL14: {struct Cyc_List_List**tvarsC=_tmp5C;struct Cyc_List_List**rgnpoC=_tmp5D;struct Cyc_List_List*cfields=_tmp5E;struct Cyc_List_List*tvars=_tmp5F;struct Cyc_List_List*rgnpo=_tmp60;struct Cyc_List_List*ufields=_tmp61;int tagged=_tmp62;
# 240
if(tagged){
({unsigned _tmp16D=loc;int _tmp16C=is_buildlib;struct _tuple0*_tmp16B=cd->name;Cyc_Cifc_fail_merge(0,_tmp16D,_tmp16C,_tmp16B,({const char*_tmp63=": user @tagged annotation not allowed (ignoring)";_tag_fat(_tmp63,sizeof(char),49U);}));});if(!0)return;}{
# 243
struct Cyc_List_List*_tmp64=cfields;struct Cyc_List_List*x=_tmp64;
# 245
while(x != 0){
struct Cyc_Absyn_Aggrfield*_tmp65=(struct Cyc_Absyn_Aggrfield*)x->hd;struct Cyc_Absyn_Aggrfield*cfield=_tmp65;
struct Cyc_Absyn_Aggrfield*_tmp66=Cyc_Absyn_lookup_field(ufields,cfield->name);struct Cyc_Absyn_Aggrfield*ufield=_tmp66;
# 249
if(ufield != 0){
# 252
if(!Cyc_Cifc_c_types_ok(loc,te,cfield->type,ufield->type)){
({unsigned _tmp173=loc;int _tmp172=is_buildlib;struct _tuple0*_tmp171=cd->name;Cyc_Cifc_fail_merge(0,_tmp173,_tmp172,_tmp171,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0U,({struct _fat_ptr _tmp16E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_tmp122.f1=_tmp16E;});_tmp122;});struct Cyc_String_pa_PrintArg_struct _tmp6A=({struct Cyc_String_pa_PrintArg_struct _tmp121;_tmp121.tag=0U,_tmp121.f1=(struct _fat_ptr)((struct _fat_ptr)*cfield->name);_tmp121;});struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp120;_tmp120.tag=0U,({struct _fat_ptr _tmp16F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cfield->type));_tmp120.f1=_tmp16F;});_tmp120;});void*_tmp67[3U];_tmp67[0]=& _tmp69,_tmp67[1]=& _tmp6A,_tmp67[2]=& _tmp6B;({struct _fat_ptr _tmp170=({const char*_tmp68=": type %s of user definition of field %s != %s";_tag_fat(_tmp68,sizeof(char),47U);});Cyc_aprintf(_tmp170,_tag_fat(_tmp67,sizeof(void*),3U));});}));});if(!0)return;}else{
# 261
if(ufield->width != 0){
({unsigned _tmp178=loc;int _tmp177=is_buildlib;struct _tuple0*_tmp176=cd->name;Cyc_Cifc_fail_merge(1,_tmp178,_tmp177,_tmp176,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp123;_tmp123.tag=0U,({struct _fat_ptr _tmp174=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_tmp123.f1=_tmp174;});_tmp123;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6E;({struct _fat_ptr _tmp175=({const char*_tmp6D=": ignoring width of user definition of field %s";_tag_fat(_tmp6D,sizeof(char),48U);});Cyc_aprintf(_tmp175,_tag_fat(_tmp6C,sizeof(void*),1U));});}));});if(!1)return;}
# 266
if(ufield->attributes != 0)
({struct Cyc_List_List*_tmp179=Cyc_Cifc_merge_attributes(cfield->attributes,ufield->attributes);cfield->attributes=_tmp179;});
# 270
cfield->type=ufield->type;
cfield->tq=ufield->tq;
cfield->requires_clause=ufield->requires_clause;}}
# 275
x=x->tl;}
# 279
if(ud->tvs != 0)cd->tvs=ud->tvs;
if((unsigned)tvars)*tvarsC=tvars;
if((unsigned)rgnpo)*rgnpoC=rgnpo;
# 284
x=ufields;{
int missing_fields=0;
while(x != 0){
struct Cyc_Absyn_Aggrfield*_tmp6F=Cyc_Absyn_lookup_field(cfields,((struct Cyc_Absyn_Aggrfield*)x->hd)->name);struct Cyc_Absyn_Aggrfield*cfield=_tmp6F;
if(cfield == 0){
missing_fields=1;
({unsigned _tmp17D=loc;int _tmp17C=is_buildlib;struct _tuple0*_tmp17B=cd->name;Cyc_Cifc_fail_merge(1,_tmp17D,_tmp17C,_tmp17B,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp72=({struct Cyc_String_pa_PrintArg_struct _tmp124;_tmp124.tag=0U,_tmp124.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)x->hd)->name);_tmp124;});void*_tmp70[1U];_tmp70[0]=& _tmp72;({struct _fat_ptr _tmp17A=({const char*_tmp71=": no definition of field %s in system version";_tag_fat(_tmp71,sizeof(char),46U);});Cyc_aprintf(_tmp17A,_tag_fat(_tmp70,sizeof(void*),1U));});}));});if(!1)return;}
# 294
x=x->tl;}
# 296
goto _LL12;}}}}else{goto _LL15;}}else{_LL15: _LL16:
# 298
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp17E=({const char*_tmp73="Internal Error: NULL cases not possible";_tag_fat(_tmp73,sizeof(char),40U);});_tmp74->f1=_tmp17E;});_tmp74;}));}_LL12:;}
# 300
goto _LL0;}}else{goto _LLB;}case 7U: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->tag == 7U){_LL7: _tmp3E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3B.f1)->f1;_tmp3F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*cd=_tmp3E;struct Cyc_Absyn_Enumdecl*ud=_tmp3F;
# 303
({unsigned _tmp181=loc;int _tmp180=is_buildlib;struct _tuple0*_tmp17F=cd->name;Cyc_Cifc_fail_merge(0,_tmp181,_tmp180,_tmp17F,({const char*_tmp75=": enum merging not currently supported";_tag_fat(_tmp75,sizeof(char),39U);}));});if(!0)return;}}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->tag == 8U){_LL9: _tmp3C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3B.f1)->f1;_tmp3D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3B.f2)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*cd=_tmp3C;struct Cyc_Absyn_Typedefdecl*ud=_tmp3D;
# 306
if(ud->defn == 0){
({unsigned _tmp184=loc;int _tmp183=is_buildlib;struct _tuple0*_tmp182=cd->name;Cyc_Cifc_fail_merge(0,_tmp184,_tmp183,_tmp182,({const char*_tmp76=": no user definition";_tag_fat(_tmp76,sizeof(char),21U);}));});if(!0)return;}
if(cd->defn == 0){
({unsigned _tmp187=loc;int _tmp186=is_buildlib;struct _tuple0*_tmp185=cd->name;Cyc_Cifc_fail_merge(1,_tmp187,_tmp186,_tmp185,({const char*_tmp77=": no definition for system version";_tag_fat(_tmp77,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 314
if(!({unsigned _tmp18A=loc;struct Cyc_Tcenv_Tenv*_tmp189=te;void*_tmp188=(void*)_check_null(cd->defn);Cyc_Cifc_c_types_ok(_tmp18A,_tmp189,_tmp188,(void*)_check_null(ud->defn));})){
({unsigned _tmp190=loc;int _tmp18F=is_buildlib;struct _tuple0*_tmp18E=cd->name;Cyc_Cifc_fail_merge(0,_tmp190,_tmp18F,_tmp18E,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp127;_tmp127.tag=0U,({struct _fat_ptr _tmp18B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(ud->defn)));_tmp127.f1=_tmp18B;});_tmp127;});struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp126;_tmp126.tag=0U,({struct _fat_ptr _tmp18C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(cd->defn)));_tmp126.f1=_tmp18C;});_tmp126;});void*_tmp78[2U];_tmp78[0]=& _tmp7A,_tmp78[1]=& _tmp7B;({struct _fat_ptr _tmp18D=({const char*_tmp79=": type definition %s of user definition != %s";_tag_fat(_tmp79,sizeof(char),46U);});Cyc_aprintf(_tmp18D,_tag_fat(_tmp78,sizeof(void*),2U));});}));});if(!0)return;}else{
# 321
cd->tvs=ud->tvs;
cd->defn=ud->defn;
cd->tq=ud->tq;
if(ud->atts != 0)
({struct Cyc_List_List*_tmp191=Cyc_Cifc_merge_attributes(cd->atts,ud->atts);cd->atts=_tmp191;});}
# 328
goto _LL0;}}else{goto _LLB;}default: _LLB: _LLC:
# 331
 if(is_buildlib)
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp128;_tmp128.tag=0U,({
struct _fat_ptr _tmp192=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp7F[1U];_tmp7F[0]=user_decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp7F,sizeof(struct Cyc_Absyn_Decl*),1U));})));_tmp128.f1=_tmp192;});_tmp128;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({struct Cyc___cycFILE*_tmp194=Cyc_stderr;struct _fat_ptr _tmp193=({const char*_tmp7D="Error in .cys file: bad (or unsupported) user-defined type %s\n";_tag_fat(_tmp7D,sizeof(char),63U);});Cyc_fprintf(_tmp194,_tmp193,_tag_fat(_tmp7C,sizeof(void*),1U));});});else{
# 335
({struct Cyc_Warn_String_Warn_Warg_struct _tmp81=({struct Cyc_Warn_String_Warn_Warg_struct _tmp12A;_tmp12A.tag=0U,({struct _fat_ptr _tmp195=({const char*_tmp84="bad (or unsupported) user-defined type %s";_tag_fat(_tmp84,sizeof(char),42U);});_tmp12A.f1=_tmp195;});_tmp12A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp82=({struct Cyc_Warn_String_Warn_Warg_struct _tmp129;_tmp129.tag=0U,({
struct _fat_ptr _tmp196=Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp83[1U];_tmp83[0]=user_decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp83,sizeof(struct Cyc_Absyn_Decl*),1U));}));_tmp129.f1=_tmp196;});_tmp129;});void*_tmp80[2U];_tmp80[0]=& _tmp81,_tmp80[1]=& _tmp82;({unsigned _tmp197=loc;Cyc_Warn_err2(_tmp197,_tag_fat(_tmp80,sizeof(void*),2U));});});}
return;}_LL0:;}
# 341
static int Cyc_Cifc_contains_type_vars(struct Cyc_Absyn_Decl*ud){
{void*_tmp85=ud->r;void*_stmttmp5=_tmp85;void*_tmp86=_stmttmp5;struct Cyc_Absyn_Aggrdecl*_tmp87;if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp86)->tag == 5U){_LL1: _tmp87=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp86)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp87;
# 344
return ad->tvs != 0;}}else{_LL3: _LL4:
# 346
 goto _LL0;}_LL0:;}
# 348
return 0;}
# 351
static struct Cyc_Absyn_Decl*Cyc_Cifc_make_abstract_decl(struct Cyc_Absyn_Decl*ud){
void*_tmp88=ud->r;void*_stmttmp6=_tmp88;void*_tmp89=_stmttmp6;struct Cyc_Absyn_Aggrdecl*_tmp8A;if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp89)->tag == 5U){_LL1: _tmp8A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp89)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp8A;
# 354
struct Cyc_Absyn_Aggrdecl*_tmp8B=({struct Cyc_Absyn_Aggrdecl*_tmp8E=_cycalloc(sizeof(*_tmp8E));*_tmp8E=*ad;_tmp8E;});struct Cyc_Absyn_Aggrdecl*absad=_tmp8B;
absad->impl=0;{
struct Cyc_Absyn_Decl*_tmp8C=({void*_tmp198=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=5U,_tmp8D->f1=absad;_tmp8D;});Cyc_Absyn_new_decl(_tmp198,ud->loc);});struct Cyc_Absyn_Decl*nd=_tmp8C;
return nd;}}}else{_LL3: _LL4:
# 359
({void*_tmp8F=0U;({struct _fat_ptr _tmp199=({const char*_tmp90="Only aggrdecls";_tag_fat(_tmp90,sizeof(char),15U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp199,_tag_fat(_tmp8F,sizeof(void*),0U));});});}_LL0:;}
# 364
static int Cyc_Cifc_kindbound_subsumed(void*kb1,void*kb2){
void*_tmp91=kb1;struct Cyc_Absyn_Kind*_tmp92;struct Cyc_Absyn_Kind*_tmp94;void*_tmp93;void*_tmp95;struct Cyc_Absyn_Kind*_tmp96;switch(*((int*)_tmp91)){case 0U: _LL1: _tmp96=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp91)->f1;_LL2: {struct Cyc_Absyn_Kind*k1=_tmp96;
# 367
void*_tmp97=kb2;struct Cyc_Absyn_Kind*_tmp98;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97)->tag == 0U){_LLC: _tmp98=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97)->f1;_LLD: {struct Cyc_Absyn_Kind*k2=_tmp98;
# 369
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{_LLE: _LLF:
# 371
 return 0;}_LLB:;}case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp91)->f1 != 0){_LL3: _tmp95=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp91)->f1)->v;_LL4: {void*kbb1=_tmp95;
# 374
void*_tmp99=kb2;void*_tmp9A;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99)->tag == 1U){if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99)->f1 != 0){_LL11: _tmp9A=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99)->f1)->v;_LL12: {void*kbb2=_tmp9A;
# 376
return Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL13;}}else{_LL13: _LL14:
# 378
 return 1;}_LL10:;}}else{_LL5: _LL6:
# 381
 return 1;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91)->f1 != 0){_LL7: _tmp93=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91)->f1)->v;_tmp94=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91)->f2;_LL8: {void*kbb1=_tmp93;struct Cyc_Absyn_Kind*k1=_tmp94;
# 383
void*_tmp9B=kb2;struct Cyc_Absyn_Kind*_tmp9D;void*_tmp9C;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9B)->tag == 2U){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9B)->f1 != 0){_LL16: _tmp9C=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9B)->f1)->v;_tmp9D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9B)->f2;_LL17: {void*kbb2=_tmp9C;struct Cyc_Absyn_Kind*k2=_tmp9D;
# 385
return((int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual)&& Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL18;}}else{_LL18: _LL19:
# 387
 return 0;}_LL15:;}}else{_LL9: _tmp92=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91)->f2;_LLA: {struct Cyc_Absyn_Kind*k1=_tmp92;
# 390
void*_tmp9E=kb2;struct Cyc_Absyn_Kind*_tmp9F;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->tag == 2U){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f1 == 0){_LL1B: _tmp9F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f2;_LL1C: {struct Cyc_Absyn_Kind*k2=_tmp9F;
# 392
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{goto _LL1D;}}else{_LL1D: _LL1E:
# 394
 return 0;}_LL1A:;}}}_LL0:;}
# 399
static int Cyc_Cifc_find_and_remove(struct Cyc_List_List**lst,void*kind){
struct Cyc_List_List*_tmpA0=*lst;struct Cyc_List_List*cur=_tmpA0;
struct Cyc_List_List*prev=0;
while((unsigned)cur){
void*_tmpA1=(void*)cur->hd;void*t=_tmpA1;
{void*_tmpA2=t;struct Cyc_Absyn_Tvar*_tmpA3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2)->tag == 2U){_LL1: _tmpA3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmpA3;
# 406
if(Cyc_Cifc_kindbound_subsumed(tv->kind,kind)){
if((unsigned)prev)
prev->tl=cur->tl;else{
# 410
*lst=cur->tl;}
cur->tl=0;
return 1;}
# 414
goto _LL0;}}else{_LL3: _LL4:
# 416
({struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp12D;_tmp12D.tag=0U,({struct _fat_ptr _tmp19A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp12D.f1=_tmp19A;});_tmp12D;});struct Cyc_String_pa_PrintArg_struct _tmpA7=({struct Cyc_String_pa_PrintArg_struct _tmp12C;_tmp12C.tag=0U,({struct _fat_ptr _tmp19B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_get_type_kind(t));_tmp12C.f1=_tmp19B;});_tmp12C;});void*_tmpA4[2U];_tmpA4[0]=& _tmpA6,_tmpA4[1]=& _tmpA7;({struct _fat_ptr _tmp19C=({const char*_tmpA5="expects a VarType list only -- got %s(%s)";_tag_fat(_tmpA5,sizeof(char),42U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp19C,_tag_fat(_tmpA4,sizeof(void*),2U));});});}_LL0:;}
# 418
prev=cur;
cur=cur->tl;}
# 421
return 0;}
# 424
static struct Cyc_List_List*Cyc_Cifc_get_tvar_difference(struct Cyc_List_List*tvs,struct Cyc_List_List*remove){
struct Cyc_List_List*ret=0;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmpA8=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*can=_tmpA8;
if(!Cyc_Cifc_find_and_remove(& remove,can->kind))
ret=({struct Cyc_List_List*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->hd=can,_tmpA9->tl=ret;_tmpA9;});
tvs=tvs->tl;}
# 432
return ret;}char Cyc_Cifc_Contains_nontvar[17U]="Contains_nontvar";struct Cyc_Cifc_Contains_nontvar_exn_struct{char*tag;};
# 437
struct Cyc_Cifc_Contains_nontvar_exn_struct Cyc_Cifc_Contains_nontvar_val={Cyc_Cifc_Contains_nontvar};
# 439
static struct Cyc_List_List*Cyc_Cifc_extract_tvars(struct Cyc_List_List*ts){
struct Cyc_List_List*res=0;
while((unsigned)ts){
void*_tmpAA=(void*)ts->hd;void*t=_tmpAA;
{void*_tmpAB=t;struct Cyc_Absyn_Tvar*_tmpAC;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB)->tag == 2U){_LL1: _tmpAC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmpAC;
# 445
res=({struct Cyc_List_List*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->hd=tv,_tmpAD->tl=res;_tmpAD;});
goto _LL0;}}else{_LL3: _LL4:
# 448
(int)_throw((void*)& Cyc_Cifc_Contains_nontvar_val);}_LL0:;}
# 450
ts=ts->tl;}
# 452
return res;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 455
static struct _tuple14*Cyc_Cifc_update_tvars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,void*t){
# 457
{void*_tmpAE=t;struct Cyc_Absyn_PtrInfo*_tmpAF;struct Cyc_List_List**_tmpB1;void*_tmpB0;switch(*((int*)_tmpAE)){case 0U: _LL1: _tmpB0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAE)->f1;_tmpB1=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAE)->f2;_LL2: {void*tc=_tmpB0;struct Cyc_List_List**ts=_tmpB1;
# 459
{void*_tmpB2=tc;union Cyc_Absyn_AggrInfo _tmpB3;if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpB2)->tag == 20U){_LL8: _tmpB3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpB2)->f1;_LL9: {union Cyc_Absyn_AggrInfo ai=_tmpB3;
# 461
union Cyc_Absyn_AggrInfo _tmpB4=ai;struct _tuple2 _tmpB5;struct Cyc_Absyn_Aggrdecl*_tmpB6;if((_tmpB4.KnownAggr).tag == 2){_LLD: _tmpB6=*(_tmpB4.KnownAggr).val;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmpB6;
# 463
struct _tuple0*_tmpB7=ad->name;struct _tuple0*_stmttmp7=_tmpB7;struct _tuple0*_tmpB8=_stmttmp7;struct _fat_ptr*_tmpBA;union Cyc_Absyn_Nmspace _tmpB9;_LL12: _tmpB9=_tmpB8->f1;_tmpBA=_tmpB8->f2;_LL13: {union Cyc_Absyn_Nmspace ns=_tmpB9;struct _fat_ptr*name=_tmpBA;
# 466
struct Cyc_Absyn_Decl*_tmpBB=Cyc_Absyn_lookup_decl(tv_ovrs,name);struct Cyc_Absyn_Decl*ovd=_tmpBB;
if((unsigned)ovd){
# 470
void*_tmpBC=ovd->r;void*_stmttmp8=_tmpBC;void*_tmpBD=_stmttmp8;struct Cyc_Absyn_Aggrdecl*_tmpBE;if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpBD)->tag == 5U){_LL15: _tmpBE=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpBD)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmpBE;
# 478
struct Cyc_List_List*removed_tvars=0;
{struct _handler_cons _tmpBF;_push_handler(& _tmpBF);{int _tmpC1=0;if(setjmp(_tmpBF.handler))_tmpC1=1;if(!_tmpC1){
removed_tvars=Cyc_Cifc_extract_tvars(*ts);;_pop_handler();}else{void*_tmpC0=(void*)Cyc_Core_get_exn_thrown();void*_tmpC2=_tmpC0;void*_tmpC3;if(((struct Cyc_Cifc_Contains_nontvar_exn_struct*)_tmpC2)->tag == Cyc_Cifc_Contains_nontvar){_LL1A: _LL1B:
# 484
 return 0;}else{_LL1C: _tmpC3=_tmpC2;_LL1D: {void*exn=_tmpC3;(int)_rethrow(exn);}}_LL19:;}}}
# 486
if((unsigned)*ts)
*ts=0;{
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*_tmpC4=ad->tvs;struct Cyc_List_List*tvs=_tmpC4;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmpC5=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmpC5;
struct Cyc_Absyn_Tvar*_tmpC6=({struct Cyc_Absyn_Tvar*_tmpCF=_cycalloc(sizeof(*_tmpCF));*_tmpCF=*tv;_tmpCF;});struct Cyc_Absyn_Tvar*tvcpy=_tmpC6;
struct _fat_ptr*_tmpC7=({struct _fat_ptr*_tmpCE=_cycalloc(sizeof(*_tmpCE));({struct _fat_ptr _tmp19E=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpCD=({struct Cyc_Int_pa_PrintArg_struct _tmp12E;_tmp12E.tag=1U,_tmp12E.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_tmp12E;});void*_tmpCB[1U];_tmpCB[0]=& _tmpCD;({struct _fat_ptr _tmp19D=({const char*_tmpCC="`ovr_%d";_tag_fat(_tmpCC,sizeof(char),8U);});Cyc_aprintf(_tmp19D,_tag_fat(_tmpCB,sizeof(void*),1U));});});*_tmpCE=_tmp19E;});_tmpCE;});struct _fat_ptr*tvn=_tmpC7;
tvcpy->name=tvn;
added_tvars=({struct Cyc_List_List*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->hd=tvcpy,_tmpC8->tl=added_tvars;_tmpC8;});
({struct Cyc_List_List*_tmp1A0=({struct Cyc_List_List*_tmpCA=_cycalloc(sizeof(*_tmpCA));({void*_tmp19F=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=2U,_tmpC9->f1=tvcpy;_tmpC9;});_tmpCA->hd=_tmp19F;}),_tmpCA->tl=*ts;_tmpCA;});*ts=_tmp1A0;});
tvs=tvs->tl;}
# 499
({struct Cyc_List_List*_tmp1A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(*ts);*ts=_tmp1A1;});
return({struct _tuple14*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->f1=added_tvars,_tmpD0->f2=removed_tvars;_tmpD0;});}}}else{_LL17: _LL18:
# 502
({void*_tmpD1=0U;({struct _fat_ptr _tmp1A2=({const char*_tmpD2="ovd must be an aggr type";_tag_fat(_tmpD2,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1A2,_tag_fat(_tmpD1,sizeof(void*),0U));});});}_LL14:;}else{
# 506
return 0;}}}}else{_LLF: _tmpB5=(_tmpB4.UnknownAggr).val;_LL10: {struct _tuple2 ua=_tmpB5;
# 508
return 0;}}_LLC:;}}else{_LLA: _LLB:
# 511
 goto _LL7;}_LL7:;}
# 513
return 0;}case 3U: _LL3: _tmpAF=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE)->f1;_LL4: {struct Cyc_Absyn_PtrInfo*pi=_tmpAF;
# 515
return Cyc_Cifc_update_tvars(te,tv_ovrs,pi->elt_type);}default: _LL5: _LL6:
# 517
 return 0;}_LL0:;}
# 519
return 0;}
# 525
static void Cyc_Cifc_update_usages(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,struct Cyc_List_List*ds){
while((unsigned)ds){
struct Cyc_Absyn_Decl*_tmpD3=(struct Cyc_Absyn_Decl*)ds->hd;struct Cyc_Absyn_Decl*d=_tmpD3;
{void*_tmpD4=d->r;void*_stmttmp9=_tmpD4;void*_tmpD5=_stmttmp9;struct Cyc_Absyn_Typedefdecl*_tmpD6;struct Cyc_Absyn_Aggrdecl*_tmpD7;struct Cyc_Absyn_Fndecl*_tmpD8;struct Cyc_Absyn_Vardecl*_tmpD9;switch(*((int*)_tmpD5)){case 0U: _LL1: _tmpD9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD5)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmpD9;
# 530
goto _LL0;}case 1U: _LL3: _tmpD8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpD5)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmpD8;
# 532
Cyc_Cifc_update_tvars(te,tv_ovrs,(fd->i).ret_type);{
struct Cyc_List_List*_tmpDA=(fd->i).args;struct Cyc_List_List*argit=_tmpDA;
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
while((unsigned)argit){
struct _tuple8*_tmpDB=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmpA=_tmpDB;struct _tuple8*_tmpDC=_stmttmpA;void**_tmpDD;_LLC: _tmpDD=(void**)& _tmpDC->f3;_LLD: {void**at=_tmpDD;
struct _tuple14*_tmpDE=Cyc_Cifc_update_tvars(te,tv_ovrs,*at);struct _tuple14*ad=_tmpDE;
if((unsigned)ad){
added_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*ad).f1,added_tvars);
removed_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*ad).f2,removed_tvars);}
# 543
argit=argit->tl;}}
# 545
while((unsigned)removed_tvars){
struct Cyc_Absyn_Tvar*_tmpDF=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;struct Cyc_Absyn_Tvar*rtv=_tmpDF;
{struct _handler_cons _tmpE0;_push_handler(& _tmpE0);{int _tmpE2=0;if(setjmp(_tmpE0.handler))_tmpE2=1;if(!_tmpE2){
({struct Cyc_List_List*_tmp1A3=((struct Cyc_List_List*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_delete)((fd->i).tvars,rtv);(fd->i).tvars=_tmp1A3;});;_pop_handler();}else{void*_tmpE1=(void*)Cyc_Core_get_exn_thrown();void*_tmpE3=_tmpE1;void*_tmpE4;if(((struct Cyc_Core_Not_found_exn_struct*)_tmpE3)->tag == Cyc_Core_Not_found){_LLF: _LL10:
# 552
({struct Cyc_String_pa_PrintArg_struct _tmpE7=({struct Cyc_String_pa_PrintArg_struct _tmp12F;_tmp12F.tag=0U,({struct _fat_ptr _tmp1A4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_tmp12F.f1=_tmp1A4;});_tmp12F;});void*_tmpE5[1U];_tmpE5[0]=& _tmpE7;({unsigned _tmp1A6=loc;struct _fat_ptr _tmp1A5=({const char*_tmpE6="Removed tvar %s not found";_tag_fat(_tmpE6,sizeof(char),26U);});Cyc_Warn_warn(_tmp1A6,_tmp1A5,_tag_fat(_tmpE5,sizeof(void*),1U));});});
goto _LLE;}else{_LL11: _tmpE4=_tmpE3;_LL12: {void*exn=_tmpE4;(int)_rethrow(exn);}}_LLE:;}}}
# 555
removed_tvars=removed_tvars->tl;}
# 557
if((unsigned)added_tvars)
({struct Cyc_List_List*_tmp1A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(added_tvars,(fd->i).tvars);(fd->i).tvars=_tmp1A7;});
# 560
(fd->i).effect=0;
# 562
goto _LL0;}}case 5U: _LL5: _tmpD7=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpD5)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmpD7;
# 564
goto _LL0;}case 8U: _LL7: _tmpD6=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD5)->f1;_LL8: {struct Cyc_Absyn_Typedefdecl*td=_tmpD6;
# 566
goto _LL0;}default: _LL9: _LLA:
# 568
 goto _LL0;}_LL0:;}
# 570
ds=ds->tl;}}
# 574
static void Cyc_Cifc_i_clear_vartype_ids(void*t){
void*_tmpE8=t;void*_tmpEA;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*_tmpEB;struct Cyc_Absyn_FnInfo _tmpEC;struct Cyc_Absyn_ArrayInfo _tmpED;struct Cyc_Absyn_PtrInfo _tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Tvar*_tmpF0;switch(*((int*)_tmpE8)){case 2U: _LL1: _tmpF0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpE8)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmpF0;
# 577
tv->identity=- 1;
goto _LL0;}case 0U: _LL3: _tmpEF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE8)->f2;_LL4: {struct Cyc_List_List*ts=_tmpEF;
# 580
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 584
goto _LL0;}case 3U: _LL5: _tmpEE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE8)->f1;_LL6: {struct Cyc_Absyn_PtrInfo pi=_tmpEE;
# 586
Cyc_Cifc_i_clear_vartype_ids(pi.elt_type);
goto _LL0;}case 4U: _LL7: _tmpED=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE8)->f1;_LL8: {struct Cyc_Absyn_ArrayInfo ai=_tmpED;
# 589
Cyc_Cifc_i_clear_vartype_ids(ai.elt_type);
goto _LL0;}case 5U: _LL9: _tmpEC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE8)->f1;_LLA: {struct Cyc_Absyn_FnInfo fi=_tmpEC;
# 592
Cyc_Cifc_i_clear_vartype_ids(fi.ret_type);{
struct Cyc_List_List*_tmpF1=fi.args;struct Cyc_List_List*argit=_tmpF1;
while((unsigned)argit){
struct _tuple8*_tmpF2=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmpB=_tmpF2;struct _tuple8*_tmpF3=_stmttmpB;void*_tmpF4;_LL12: _tmpF4=_tmpF3->f3;_LL13: {void*at=_tmpF4;
Cyc_Cifc_i_clear_vartype_ids(at);
argit=argit->tl;}}
# 599
goto _LL0;}}case 6U: _LLB: _tmpEB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE8)->f1;_LLC: {struct Cyc_List_List*ts=_tmpEB;
# 601
while((unsigned)ts){
struct _tuple11*_tmpF5=(struct _tuple11*)ts->hd;struct _tuple11*_stmttmpC=_tmpF5;struct _tuple11*_tmpF6=_stmttmpC;void*_tmpF7;_LL15: _tmpF7=_tmpF6->f2;_LL16: {void*et=_tmpF7;
Cyc_Cifc_i_clear_vartype_ids(et);
ts=ts->tl;}}
# 606
goto _LL0;}case 8U: _LLD: _tmpE9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpE8)->f2;_tmpEA=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpE8)->f4;_LLE: {struct Cyc_List_List*ts=_tmpE9;void*to=_tmpEA;
# 611
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 615
goto _LL0;}default: _LLF: _LL10:
# 620
 goto _LL0;}_LL0:;}
# 624
static void Cyc_Cifc_clear_vartype_ids(struct Cyc_Absyn_Decl*d){
void*_tmpF8=d->r;void*_stmttmpD=_tmpF8;void*_tmpF9=_stmttmpD;struct Cyc_Absyn_Typedefdecl*_tmpFA;struct Cyc_Absyn_Aggrdecl*_tmpFB;struct Cyc_Absyn_Fndecl*_tmpFC;struct Cyc_Absyn_Vardecl*_tmpFD;switch(*((int*)_tmpF9)){case 0U: _LL1: _tmpFD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpF9)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmpFD;
# 627
Cyc_Cifc_i_clear_vartype_ids(vd->type);
goto _LL0;}case 1U: _LL3: _tmpFC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpF9)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmpFC;
# 630
Cyc_Cifc_i_clear_vartype_ids((fd->i).ret_type);{
struct Cyc_List_List*_tmpFE=(fd->i).args;struct Cyc_List_List*ai=_tmpFE;
while((unsigned)ai){
Cyc_Cifc_i_clear_vartype_ids((*((struct _tuple8*)ai->hd)).f3);}
# 635
(fd->i).tvars=0;
goto _LL0;}}case 5U: _LL5: _tmpFB=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpF9)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmpFB;
# 638
goto _LL0;}case 8U: _LL7: _tmpFA=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpF9)->f1;_LL8: {struct Cyc_Absyn_Typedefdecl*td=_tmpFA;
# 640
goto _LL0;}default: _LL9: _LLA:
# 643
 goto _LL0;}_LL0:;}
# 647
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List**ds,struct Cyc_List_List*ovrs){
struct Cyc_List_List*type_overrides=0;
{struct Cyc_List_List*_tmpFF=ovrs;struct Cyc_List_List*x=_tmpFF;for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Decl*_tmp100=(struct Cyc_Absyn_Decl*)x->hd;struct Cyc_Absyn_Decl*ud=_tmp100;
struct _fat_ptr*_tmp101=Cyc_Absyn_decl_name(ud);struct _fat_ptr*un=_tmp101;
if(!((unsigned)un))({void*_tmp102=0U;({unsigned _tmp1A9=ud->loc;struct _fat_ptr _tmp1A8=({const char*_tmp103="Overriding decl without a name\n";_tag_fat(_tmp103,sizeof(char),32U);});Cyc_Warn_warn(_tmp1A9,_tmp1A8,_tag_fat(_tmp102,sizeof(void*),0U));});});else{
# 654
struct Cyc_Absyn_Decl*_tmp104=Cyc_Absyn_lookup_decl(*((struct Cyc_List_List**)_check_null(ds)),un);struct Cyc_Absyn_Decl*d=_tmp104;
if(!((unsigned)d))({struct Cyc_String_pa_PrintArg_struct _tmp107=({struct Cyc_String_pa_PrintArg_struct _tmp130;_tmp130.tag=0U,_tmp130.f1=(struct _fat_ptr)((struct _fat_ptr)*un);_tmp130;});void*_tmp105[1U];_tmp105[0]=& _tmp107;({unsigned _tmp1AB=ud->loc;struct _fat_ptr _tmp1AA=({const char*_tmp106="%s is overridden but not defined";_tag_fat(_tmp106,sizeof(char),33U);});Cyc_Warn_warn(_tmp1AB,_tmp1AA,_tag_fat(_tmp105,sizeof(void*),1U));});});else{
# 657
int _tmp108=Cyc_Cifc_contains_type_vars(d);int pre_tvars_d=_tmp108;
Cyc_Cifc_merge_sys_user_decl(loc,te,0,ud,d);
Cyc_Cifc_clear_vartype_ids(ud);
if(Cyc_Cifc_contains_type_vars(ud)&& !pre_tvars_d){
struct Cyc_Absyn_Decl*_tmp109=Cyc_Cifc_make_abstract_decl(ud);struct Cyc_Absyn_Decl*absdecl=_tmp109;
type_overrides=({struct Cyc_List_List*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->hd=d,_tmp10A->tl=type_overrides;_tmp10A;});
({struct Cyc_List_List*_tmp1AC=({struct Cyc_List_List*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=absdecl,_tmp10B->tl=*ds;_tmp10B;});*ds=_tmp1AC;});}}}}}
# 668
Cyc_Cifc_update_usages(loc,te,type_overrides,*((struct Cyc_List_List**)_check_null(ds)));}

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

/* Pools */
struct _pool; // defined in runtime_memory.c
struct _PoolHandle {
  struct _RuntimeStack s;
  struct _pool *p;
};
struct _PoolHandle _new_pool(void);
void _free_pool(struct _PoolHandle *h);
void _push_pool(struct _PoolHandle * r);
void _pop_pool(void);

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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line);struct Cyc_Hashtable_Table;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 889 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 902
void*Cyc_Absyn_compress(void*);
# 934
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void);
void*Cyc_CurRgn_curr_rgn_type (void);
# 33
struct _fat_ptr Cyc_CurRgn_curr_poolrgn_name;
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_poolrgn_tvar (void);
void*Cyc_CurRgn_curr_poolrgn_type (void);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 33 "currgn.cyc"
static struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_tvar=0;
static void*Cyc_CurRgn_curr_rgn_typ=0;
# 36
static struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_pool_tvar=0;
static void*Cyc_CurRgn_curr_poolrgn_typ=0;static char _tmp0[3U]="`C";
# 39
struct _fat_ptr Cyc_CurRgn_curr_rgn_name={_tmp0,_tmp0,_tmp0 + 3U};static char _tmp1[3U]="`A";
struct _fat_ptr Cyc_CurRgn_curr_poolrgn_name={_tmp1,_tmp1,_tmp1 + 3U};
# 42
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void){
if(Cyc_CurRgn_curr_tvar == 0){
Cyc_CurRgn_curr_tvar=({struct Cyc_Absyn_Tvar*_tmp3=_cycalloc(sizeof(*_tmp3));({struct _fat_ptr*_tmp112=({struct _fat_ptr*_tmp2=_cycalloc(sizeof(*_tmp2));*_tmp2=Cyc_CurRgn_curr_rgn_name;_tmp2;});_tmp3->name=_tmp112;}),({
int _tmp111=Cyc_Tcutil_new_tvar_id();_tmp3->identity=_tmp111;}),({
void*_tmp110=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3->kind=_tmp110;});_tmp3;});
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar));}
# 49
return(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar);}
# 51
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_poolrgn_tvar (void){
if(Cyc_CurRgn_curr_pool_tvar == 0){
Cyc_CurRgn_curr_pool_tvar=({struct Cyc_Absyn_Tvar*_tmp5=_cycalloc(sizeof(*_tmp5));({struct _fat_ptr*_tmp115=({struct _fat_ptr*_tmp4=_cycalloc(sizeof(*_tmp4));*_tmp4=Cyc_CurRgn_curr_poolrgn_name;_tmp4;});_tmp5->name=_tmp115;}),({
int _tmp114=Cyc_Tcutil_new_tvar_id();_tmp5->identity=_tmp114;}),({
# 56
void*_tmp113=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp5->kind=_tmp113;});_tmp5;});
Cyc_CurRgn_curr_poolrgn_typ=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar));}
# 59
return(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar);}
# 62
void*Cyc_CurRgn_curr_rgn_type (void){
Cyc_CurRgn_curr_rgn_tvar();
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar));
return(void*)_check_null(Cyc_CurRgn_curr_rgn_typ);}
# 67
void*Cyc_CurRgn_curr_poolrgn_type (void){
Cyc_CurRgn_curr_poolrgn_tvar();
Cyc_CurRgn_curr_poolrgn_typ=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar));
return(void*)_check_null(Cyc_CurRgn_curr_poolrgn_typ);}struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 73
void Cyc_CurRgn_subst_inst(struct Cyc_List_List*inst,struct Cyc_Absyn_Tvar*tv,void*t){
while(inst != 0){
struct _tuple11*_tmp6=(struct _tuple11*)inst->hd;struct _tuple11*_stmttmp0=_tmp6;struct _tuple11*_tmp7=_stmttmp0;void**_tmp9;struct Cyc_Absyn_Tvar**_tmp8;_LL1: _tmp8=(struct Cyc_Absyn_Tvar**)& _tmp7->f1;_tmp9=(void**)& _tmp7->f2;_LL2: {struct Cyc_Absyn_Tvar**tvi=_tmp8;void**ti=_tmp9;
if(Cyc_Absyn_tvar_cmp(*tvi,tv)== 0){
*ti=t;
return;}
# 80
inst=inst->tl;}}}
# 87
static struct _tuple8*Cyc_CurRgn_argtype(struct _tuple8*arg){
struct _tuple8*_tmpA=arg;void*_tmpD;struct Cyc_Absyn_Tqual _tmpC;struct _fat_ptr*_tmpB;_LL1: _tmpB=_tmpA->f1;_tmpC=_tmpA->f2;_tmpD=_tmpA->f3;_LL2: {struct _fat_ptr*vopt=_tmpB;struct Cyc_Absyn_Tqual tq=_tmpC;void*t=_tmpD;
{void*_tmpE=t;void*_tmpF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE)->tag == 3U){_LL4: _tmpF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE)->f1).elt_type;_LL5: {void*t1=_tmpF;
# 91
if(!Cyc_Tcutil_is_function_type(t1))return arg;
goto _LL3;}}else{_LL6: _LL7:
 return arg;}_LL3:;}{
# 95
void*_tmp10=Cyc_Tcutil_copy_type(t);void*t2=_tmp10;
void*_tmp11=Cyc_Absyn_compress(t2);void*_stmttmp1=_tmp11;void*_tmp12=_stmttmp1;void*_tmp13;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->tag == 3U){_LL9: _tmp13=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).elt_type;_LLA: {void*t1=_tmp13;
# 98
void*_tmp14=Cyc_Absyn_compress(t1);void*_stmttmp2=_tmp14;void*_tmp15=_stmttmp2;struct Cyc_Absyn_Vardecl*_tmp23;struct Cyc_List_List*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_List_List*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1D;struct Cyc_Absyn_VarargInfo*_tmp1C;int _tmp1B;struct Cyc_List_List*_tmp1A;void*_tmp19;struct Cyc_Absyn_Tqual _tmp18;void*_tmp17;struct Cyc_List_List**_tmp16;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LLE: _tmp16=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).tvars;_tmp17=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).effect;_tmp18=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).ret_tqual;_tmp19=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).ret_type;_tmp1A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).args;_tmp1B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).c_varargs;_tmp1C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).cyc_varargs;_tmp1D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).rgn_po;_tmp1E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).attributes;_tmp1F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).requires_clause;_tmp20=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).requires_relns;_tmp21=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).ensures_clause;_tmp22=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).ensures_relns;_tmp23=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->f1).return_value;_LLF: {struct Cyc_List_List**tvars=_tmp16;void*eff=_tmp17;struct Cyc_Absyn_Tqual res_tq=_tmp18;void*res_typ=_tmp19;struct Cyc_List_List*args_info=_tmp1A;int c_vararg=_tmp1B;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp1C;struct Cyc_List_List*rgn_po=_tmp1D;struct Cyc_List_List*atts=_tmp1E;struct Cyc_Absyn_Exp*req=_tmp1F;struct Cyc_List_List*req_relns=_tmp20;struct Cyc_Absyn_Exp*ens=_tmp21;struct Cyc_List_List*ens_relns=_tmp22;struct Cyc_Absyn_Vardecl*ret_var=_tmp23;
# 104
struct Cyc_List_List*_tmp24=*tvars;struct Cyc_List_List*tvars2=_tmp24;
({struct Cyc_List_List*_tmp117=({struct Cyc_List_List*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->hd=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),({struct Cyc_List_List*_tmp116=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->hd=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar),_tmp25->tl=tvars2;_tmp25;});_tmp26->tl=_tmp116;});_tmp26;});*tvars=_tmp117;});
return({struct _tuple8*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->f1=vopt,_tmp27->f2=tq,_tmp27->f3=t2;_tmp27;});}}else{_LL10: _LL11:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp118=({const char*_tmp28="internal compiler error: not a function type";_tag_fat(_tmp28,sizeof(char),45U);});_tmp29->f1=_tmp118;});_tmp29;}));}_LLD:;}}else{_LLB: _LLC:
# 109
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp119=({const char*_tmp2A="internal compiler error: not a pointer type";_tag_fat(_tmp2A,sizeof(char),44U);});_tmp2B->f1=_tmp119;});_tmp2B;}));}_LL8:;}}}
# 114
void*Cyc_CurRgn_instantiate(void*fntype,void*currgn,void*currpoolrgn){
void*_tmp2C=fntype;struct Cyc_Absyn_Vardecl*_tmp3A;struct Cyc_List_List*_tmp39;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_List_List*_tmp37;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp34;struct Cyc_Absyn_VarargInfo*_tmp33;int _tmp32;struct Cyc_List_List**_tmp31;void*_tmp30;struct Cyc_Absyn_Tqual _tmp2F;void*_tmp2E;struct Cyc_List_List*_tmp2D;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->tag == 5U){_LL1: _tmp2D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).tvars;_tmp2E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).effect;_tmp2F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).ret_tqual;_tmp30=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).ret_type;_tmp31=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).args;_tmp32=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).c_varargs;_tmp33=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).cyc_varargs;_tmp34=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).rgn_po;_tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).attributes;_tmp36=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).requires_clause;_tmp37=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).requires_relns;_tmp38=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).ensures_clause;_tmp39=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).ensures_relns;_tmp3A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C)->f1).return_value;_LL2: {struct Cyc_List_List*tvars=_tmp2D;void*eff=_tmp2E;struct Cyc_Absyn_Tqual res_tq=_tmp2F;void*res_typ=_tmp30;struct Cyc_List_List**args_info=_tmp31;int c_vararg=_tmp32;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp33;struct Cyc_List_List*rgn_po=_tmp34;struct Cyc_List_List*atts=_tmp35;struct Cyc_Absyn_Exp*req=_tmp36;struct Cyc_List_List*req_relns=_tmp37;struct Cyc_Absyn_Exp*ens=_tmp38;struct Cyc_List_List*ens_relns=_tmp39;struct Cyc_Absyn_Vardecl*ret_var=_tmp3A;
# 122
struct Cyc_List_List*_tmp3B=*args_info;struct Cyc_List_List*old_args_info=_tmp3B;
# 124
({struct Cyc_List_List*_tmp11A=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_CurRgn_argtype,*args_info);*args_info=_tmp11A;});{
# 126
struct Cyc_List_List*_tmp3C=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));({struct _tuple11*_tmp11D=({struct _tuple11*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),_tmp40->f2=currgn;_tmp40;});_tmp43->hd=_tmp11D;}),({
struct Cyc_List_List*_tmp11C=({struct Cyc_List_List*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _tuple11*_tmp11B=({struct _tuple11*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar),_tmp41->f2=currpoolrgn;_tmp41;});_tmp42->hd=_tmp11B;}),_tmp42->tl=0;_tmp42;});_tmp43->tl=_tmp11C;});_tmp43;});
# 126
struct Cyc_List_List*inst=_tmp3C;
# 128
void*_tmp3D=Cyc_Tcutil_substitute(inst,fntype);void*fntype2=_tmp3D;
# 130
{void*_tmp3E=fntype2;struct Cyc_List_List**_tmp3F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E)->tag == 5U){_LL6: _tmp3F=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E)->f1).args;_LL7: {struct Cyc_List_List**args_info2=_tmp3F;
# 132
*args_info2=old_args_info;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 136
*args_info=old_args_info;
return fntype2;}}}else{_LL3: _LL4:
 return fntype;}_LL0:;}
# 143
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*v){
return
 Cyc_Absyn_tvar_cmp(v,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))!= 0 &&
 Cyc_Absyn_tvar_cmp(v,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar))!= 0;}
# 150
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*v){
return
 Cyc_Absyn_tvar_cmp(v,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))== 0 ||
 Cyc_Absyn_tvar_cmp(v,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar))== 0;}struct Cyc_CurRgn_rgnsubenv{struct Cyc_List_List*inst;int in_externC;int toplevel;};
# 162
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Stmt*s);
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e);
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e);
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds);
# 167
static void Cyc_CurRgn_rgnsubdtdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Datatypedecl*tud);
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Aggrdecl*ad);
# 171
static void Cyc_CurRgn_check_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
if(Cyc_Absyn_tvar_cmp(tv,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp45=({struct Cyc_Warn_String_Warn_Warg_struct _tmp10E;_tmp10E.tag=0U,({struct _fat_ptr _tmp11E=({const char*_tmp47="Cannot abstract special type variable ";_tag_fat(_tmp47,sizeof(char),39U);});_tmp10E.f1=_tmp11E;});_tmp10E;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp46=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp10D;_tmp10D.tag=7U,_tmp10D.f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar);_tmp10D;});void*_tmp44[2U];_tmp44[0]=& _tmp45,_tmp44[1]=& _tmp46;({unsigned _tmp11F=loc;Cyc_Warn_err2(_tmp11F,_tag_fat(_tmp44,sizeof(void*),2U));});});}
# 175
static void Cyc_CurRgn_check_tvars(unsigned loc,struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}}
# 180
static void Cyc_CurRgn_rgnsubtypes(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ts);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 183
static void Cyc_CurRgn_rgnsubtype(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,void*type){
void*_tmp48=Cyc_Absyn_compress(type);void*_stmttmp3=_tmp48;void*_tmp49=_stmttmp3;void**_tmp4B;struct Cyc_Absyn_TypeDecl*_tmp4A;struct Cyc_Absyn_Exp*_tmp4C;struct Cyc_Absyn_Exp*_tmp4D;struct Cyc_List_List*_tmp4F;void*_tmp4E;void*_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;void*_tmp56;void*_tmp55;void*_tmp54;void*_tmp53;void*_tmp59;struct Cyc_Absyn_Exp*_tmp58;void*_tmp57;struct Cyc_List_List*_tmp5A;struct Cyc_Absyn_Tvar*_tmp5B;switch(*((int*)_tmp49)){case 2U: _LL1: _tmp5B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp49)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp5B;
# 186
if(env.toplevel)Cyc_CurRgn_check_tvar(loc,v);return;}case 8U: _LL3: _tmp5A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp49)->f2;_LL4: {struct Cyc_List_List*ts=_tmp5A;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 4U: _LL5: _tmp57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49)->f1).elt_type;_tmp58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49)->f1).num_elts;_tmp59=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49)->f1).zero_term;_LL6: {void*t1=_tmp57;struct Cyc_Absyn_Exp*e=_tmp58;void*zt=_tmp59;
# 189
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubexpopt(env,e);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
return;}case 3U: _LL7: _tmp53=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).elt_type;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).rgn;_tmp55=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).bounds;_tmp56=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp53;void*r=_tmp54;void*b=_tmp55;void*zt=_tmp56;
# 194
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubtype(loc,env,r);
Cyc_CurRgn_rgnsubtype(loc,env,b);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
return;}case 5U: _LL9: _LLA:
# 200
 return;case 6U: _LLB: _tmp52=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp49)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp52;
# 202
{struct Cyc_List_List*_tmp5C=tqts;struct Cyc_List_List*ts1=_tmp5C;for(0;ts1 != 0;ts1=ts1->tl){
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple12*)ts1->hd)).f2);}}
# 205
return;}case 7U: _LLD: _tmp51=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp49)->f2;_LLE: {struct Cyc_List_List*fs=_tmp51;
# 207
for(0;fs != 0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}
# 211
return;}case 1U: _LLF: _tmp50=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp49)->f2;_LL10: {void*r=_tmp50;
# 213
if(r != 0)Cyc_CurRgn_rgnsubtype(loc,env,r);
return;}case 0U: _LL11: _tmp4E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f1;_tmp4F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f2;_LL12: {void*c=_tmp4E;struct Cyc_List_List*ts=_tmp4F;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 9U: _LL13: _tmp4D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp49)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp4D;
Cyc_CurRgn_rgnsubexp(env,e);return;}case 11U: _LL15: _tmp4C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp49)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp4C;
Cyc_CurRgn_rgnsubexp(env,e);return;}default: _LL17: _tmp4A=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp49)->f1;_tmp4B=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp49)->f2;_LL18: {struct Cyc_Absyn_TypeDecl*td=_tmp4A;void**topt=_tmp4B;
# 219
topt == 0?0:({struct _fat_ptr _tmp120=({const char*_tmp5D="topt == NULL";_tag_fat(_tmp5D,sizeof(char),13U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp120,({const char*_tmp5E="currgn.cyc";_tag_fat(_tmp5E,sizeof(char),11U);}),219U);});
{void*_tmp5F=td->r;void*_stmttmp4=_tmp5F;void*_tmp60=_stmttmp4;struct Cyc_Absyn_Datatypedecl*_tmp61;struct Cyc_Absyn_Aggrdecl*_tmp62;switch(*((int*)_tmp60)){case 0U: _LL1A: _tmp62=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp60)->f1;_LL1B: {struct Cyc_Absyn_Aggrdecl*ad=_tmp62;
Cyc_CurRgn_rgnsubaggrdecl(loc,env,ad);return;}case 2U: _LL1C: _tmp61=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp60)->f1;_LL1D: {struct Cyc_Absyn_Datatypedecl*dtd=_tmp61;
Cyc_CurRgn_rgnsubdtdecl(loc,env,dtd);return;}default: _LL1E: _LL1F:
 return;}_LL19:;}
# 225
return;}}_LL0:;}
# 229
static void Cyc_CurRgn_rgnsubtypes(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ts){
# 231
for(0;ts != 0;ts=ts->tl){Cyc_CurRgn_rgnsubtype(loc,env,(void*)ts->hd);}}
# 236
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
{void*_tmp63=e->r;void*_stmttmp5=_tmp63;void*_tmp64=_stmttmp5;struct Cyc_Absyn_Stmt*_tmp65;struct Cyc_Absyn_Exp*_tmp68;void**_tmp67;struct Cyc_Absyn_Exp*_tmp66;struct Cyc_List_List*_tmp6A;void**_tmp69;struct Cyc_List_List*_tmp6C;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6D;struct Cyc_List_List*_tmp6E;struct Cyc_List_List*_tmp70;struct _tuple8*_tmp6F;struct Cyc_Absyn_Exp*_tmp72;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_Exp*_tmp73;int _tmp78;struct Cyc_Absyn_VarargCallInfo*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_Absyn_Exp*_tmp75;void**_tmp79;struct Cyc_Absyn_Exp*_tmp7A;struct Cyc_Absyn_Exp*_tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Exp*_tmp7D;struct Cyc_Absyn_Exp*_tmp7E;struct Cyc_Absyn_Exp*_tmp7F;struct Cyc_Absyn_Exp*_tmp80;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Exp*_tmp86;void**_tmp85;void**_tmp88;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_Absyn_Datatypefield*_tmp8D;struct Cyc_Absyn_Datatypedecl*_tmp8C;struct Cyc_List_List*_tmp8B;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp98;int _tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Vardecl*_tmp9A;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmp9E;void**_tmpA1;struct Cyc_List_List*_tmpA3;void**_tmpA2;switch(*((int*)_tmp64)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 40U: _LL9: _LLA:
 goto _LLC;case 1U: _LLB: _LLC:
 goto _LL0;case 19U: _LLD: _tmpA2=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmpA3=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LLE: {void**t=_tmpA2;struct Cyc_List_List*f=_tmpA3;
# 245
_tmpA1=t;goto _LL10;}case 17U: _LLF: _tmpA1=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL10: {void**t=_tmpA1;
# 247
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmp121=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmp121;});
goto _LL0;}case 6U: _LL11: _tmp9E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp9F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_tmpA0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp64)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp9E;struct Cyc_Absyn_Exp*e2=_tmp9F;struct Cyc_Absyn_Exp*e3=_tmpA0;
# 253
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);Cyc_CurRgn_rgnsubexp(env,e3);goto _LL0;}case 27U: _LL13: _tmp9A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp9B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_tmp9C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp64)->f3;_tmp9D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp64)->f4;_LL14: {struct Cyc_Absyn_Vardecl*vd=_tmp9A;struct Cyc_Absyn_Exp*e1=_tmp9B;struct Cyc_Absyn_Exp*e2=_tmp9C;int res=_tmp9D;
# 255
_tmp98=e1;_tmp99=e2;goto _LL16;}case 4U: _LL15: _tmp98=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp99=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp64)->f3;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp98;struct Cyc_Absyn_Exp*e2=_tmp99;
_tmp96=e1;_tmp97=e2;goto _LL18;}case 35U: _LL17: _tmp96=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp97=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp96;struct Cyc_Absyn_Exp*e2=_tmp97;
_tmp94=e1;_tmp95=e2;goto _LL1A;}case 7U: _LL19: _tmp94=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp95=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp94;struct Cyc_Absyn_Exp*e2=_tmp95;
_tmp92=e1;_tmp93=e2;goto _LL1C;}case 8U: _LL1B: _tmp92=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp93=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp92;struct Cyc_Absyn_Exp*e2=_tmp93;
_tmp90=e1;_tmp91=e2;goto _LL1E;}case 23U: _LL1D: _tmp90=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp91=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp90;struct Cyc_Absyn_Exp*e2=_tmp91;
_tmp8E=e1;_tmp8F=e2;goto _LL20;}case 9U: _LL1F: _tmp8E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp8F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp8E;struct Cyc_Absyn_Exp*e2=_tmp8F;
# 262
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);goto _LL0;}case 31U: _LL21: _tmp8B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp8C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_tmp8D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp64)->f3;_LL22: {struct Cyc_List_List*es=_tmp8B;struct Cyc_Absyn_Datatypedecl*dtd=_tmp8C;struct Cyc_Absyn_Datatypefield*dtfd=_tmp8D;
# 264
_tmp8A=es;goto _LL24;}case 3U: _LL23: _tmp8A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL24: {struct Cyc_List_List*es=_tmp8A;
# 266
_tmp89=es;goto _LL26;}case 24U: _LL25: _tmp89=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL26: {struct Cyc_List_List*es=_tmp89;
# 268
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 272
goto _LL0;}case 28U: _LL27: _tmp87=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp88=(void**)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp87;void**t=_tmp88;
# 274
_tmp85=t;_tmp86=e1;goto _LL2A;}case 14U: _LL29: _tmp85=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp86=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL2A: {void**t=_tmp85;struct Cyc_Absyn_Exp*e1=_tmp86;
# 276
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmp122=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmp122;});
_tmp84=e1;goto _LL2C;}case 21U: _LL2B: _tmp84=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp84;
# 281
_tmp83=e1;goto _LL2E;}case 22U: _LL2D: _tmp83=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp83;
_tmp82=e1;goto _LL30;}case 38U: _LL2F: _tmp82=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp82;
_tmp81=e1;goto _LL32;}case 5U: _LL31: _tmp81=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp81;
_tmp80=e1;goto _LL34;}case 42U: _LL33: _tmp80=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp80;
_tmp7F=e1;goto _LL36;}case 20U: _LL35: _tmp7F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7E=e1;goto _LL38;}case 15U: _LL37: _tmp7E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp7E;
_tmp7D=e1;goto _LL3A;}case 18U: _LL39: _tmp7D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp7D;
_tmp7C=e1;goto _LL3C;}case 41U: _LL3B: _tmp7C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmp7C;
_tmp7B=e1;goto _LL3E;}case 11U: _LL3D: _tmp7B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmp7B;
_tmp7A=e1;goto _LL40;}case 12U: _LL3F: _tmp7A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp7A;
Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 39U: _LL41: _tmp79=(void**)&((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL42: {void**t=_tmp79;
# 293
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel){
void*_tmpA4=Cyc_Tcutil_substitute_nofun(env.inst,*t);void*new_typ=_tmpA4;
if(new_typ == *t)goto _LL0;{
# 298
void*_tmpA5=Cyc_Absyn_compress(new_typ);void*_stmttmp6=_tmpA5;void*_tmpA6=_stmttmp6;struct Cyc_Absyn_Exp*_tmpA7;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpA6)->tag == 9U){_LL58: _tmpA7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpA6)->f1;_LL59: {struct Cyc_Absyn_Exp*e1=_tmpA7;
# 300
e->r=e1->r;
e->topt=e1->topt;
e->annot=e1->annot;
goto _LL57;}}else{_LL5A: _LL5B:
*t=new_typ;goto _LL57;}_LL57:;}}
# 307
goto _LL0;}case 10U: _LL43: _tmp75=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp76=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_tmp77=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp64)->f3;_tmp78=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp64)->f4;_LL44: {struct Cyc_Absyn_Exp*e1=_tmp75;struct Cyc_List_List*es=_tmp76;struct Cyc_Absyn_VarargCallInfo*vararg=_tmp77;int resv=_tmp78;
# 309
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 313
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 13U: _LL45: _tmp73=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp74=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL46: {struct Cyc_Absyn_Exp*e1=_tmp73;struct Cyc_List_List*ts=_tmp74;
# 316
while(ts != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_tmp123=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_tmp123;});
ts=ts->tl;}
# 322
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 16U: _LL47: _tmp71=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp72=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL48: {struct Cyc_Absyn_Exp*eopt=_tmp71;struct Cyc_Absyn_Exp*e1=_tmp72;
# 326
Cyc_CurRgn_rgnsubexpopt(env,eopt);Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 25U: _LL49: _tmp6F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp70=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL4A: {struct _tuple8*i=_tmp6F;struct Cyc_List_List*ds=_tmp70;
# 329
Cyc_CurRgn_rgnsubtype(e->loc,env,(*i).f3);
if(!env.toplevel)
({void*_tmp124=Cyc_Tcutil_substitute_nofun(env.inst,(*i).f3);(*i).f3=_tmp124;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 36U: _LL4B: _tmp6E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL4C: {struct Cyc_List_List*ds=_tmp6E;
# 335
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 26U: _LL4D: _tmp6D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL4E: {struct Cyc_List_List*ds=_tmp6D;
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 29U: _LL4F: _tmp6B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_tmp6C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp64)->f3;_LL50: {struct Cyc_List_List*ts=_tmp6B;struct Cyc_List_List*ds=_tmp6C;
# 339
for(0;ts != 0;ts=ts->tl){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_tmp125=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_tmp125;});}
# 344
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 30U: _LL51: _tmp69=(void**)&((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_tmp6A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp64)->f2;_LL52: {void**t=_tmp69;struct Cyc_List_List*ds=_tmp6A;
# 348
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmp126=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmp126;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 34U: _LL53: _tmp66=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp64)->f1).rgn;_tmp67=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp64)->f1).elt_type;_tmp68=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp64)->f1).num_elts;_LL54: {struct Cyc_Absyn_Exp*rgnopt=_tmp66;void**elt_t=_tmp67;struct Cyc_Absyn_Exp*e1=_tmp68;
# 355
!env.toplevel?0:({struct _fat_ptr _tmp127=({const char*_tmpA8="!env.toplevel";_tag_fat(_tmpA8,sizeof(char),14U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp127,({const char*_tmpA9="currgn.cyc";_tag_fat(_tmpA9,sizeof(char),11U);}),355U);});
Cyc_CurRgn_rgnsubexpopt(env,rgnopt);
if(elt_t != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,*elt_t);
({void*_tmp128=Cyc_Tcutil_substitute_nofun(env.inst,*elt_t);*elt_t=_tmp128;});}
# 361
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}default: _LL55: _tmp65=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp64)->f1;_LL56: {struct Cyc_Absyn_Stmt*s=_tmp65;
# 364
Cyc_CurRgn_rgnsubstmt(env,s);goto _LL0;}}_LL0:;}
# 366
if(e->topt != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)_check_null(e->topt));
if(!env.toplevel)
({void*_tmp129=Cyc_Tcutil_substitute_nofun(env.inst,(void*)_check_null(e->topt));e->topt=_tmp129;});}}
# 383 "currgn.cyc"
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
if(e == 0)return;else{
Cyc_CurRgn_rgnsubexp(env,e);}}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 388
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
# 390
for(0;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubexp(env,(*((struct _tuple13*)ds->hd)).f2);}}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 394
static void Cyc_CurRgn_rgnsubpat(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Pat*p){
!env.toplevel?0:({struct _fat_ptr _tmp12A=({const char*_tmpAA="!env.toplevel";_tag_fat(_tmpAA,sizeof(char),14U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp12A,({const char*_tmpAB="currgn.cyc";_tag_fat(_tmpAB,sizeof(char),11U);}),395U);});{
void*_tmpAC=p->r;void*_stmttmp7=_tmpAC;void*_tmpAD=_stmttmp7;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB3;struct Cyc_List_List*_tmpB2;struct Cyc_Absyn_Pat*_tmpB4;struct Cyc_Absyn_Vardecl*_tmpB6;struct Cyc_Absyn_Tvar*_tmpB5;struct Cyc_Absyn_Vardecl*_tmpB8;struct Cyc_Absyn_Tvar*_tmpB7;struct Cyc_Absyn_Pat*_tmpBA;struct Cyc_Absyn_Vardecl*_tmpB9;struct Cyc_Absyn_Pat*_tmpBC;struct Cyc_Absyn_Vardecl*_tmpBB;switch(*((int*)_tmpAD)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmpBB=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmpAD)->f1;_tmpBC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmpAD)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpBB;struct Cyc_Absyn_Pat*p1=_tmpBC;
# 399
_tmpB9=vd;_tmpBA=p1;goto _LL6;}case 3U: _LL5: _tmpB9=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmpAD)->f1;_tmpBA=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmpAD)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpB9;struct Cyc_Absyn_Pat*p1=_tmpBA;
# 402
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmp12B=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmp12B;});}
# 406
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 2U: _LL7: _tmpB7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmpAD)->f1;_tmpB8=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmpAD)->f2;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmpB7;struct Cyc_Absyn_Vardecl*vd=_tmpB8;
# 410
Cyc_CurRgn_check_tvar(p->loc,tv);
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmp12C=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmp12C;});}
# 415
return;}case 4U: _LL9: _tmpB5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmpAD)->f1;_tmpB6=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmpAD)->f2;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmpB5;struct Cyc_Absyn_Vardecl*vd=_tmpB6;
# 419
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmp12D=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmp12D;});}
# 423
Cyc_CurRgn_check_tvar(p->loc,tv);
return;}case 6U: _LLB: _tmpB4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmpAD)->f1;_LLC: {struct Cyc_Absyn_Pat*p1=_tmpB4;
# 427
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 7U: _LLD: _tmpB2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmpAD)->f2;_tmpB3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmpAD)->f3;_LLE: {struct Cyc_List_List*tvs=_tmpB2;struct Cyc_List_List*ds=_tmpB3;
# 431
for(0;tvs != 0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(p->loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}
for(0;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubpat(env,(*((struct _tuple14*)ds->hd)).f2);}
# 436
return;}case 8U: _LLF: _tmpB1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmpAD)->f3;_LL10: {struct Cyc_List_List*ps=_tmpB1;
# 439
_tmpB0=ps;goto _LL12;}case 5U: _LL11: _tmpB0=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmpAD)->f1;_LL12: {struct Cyc_List_List*ps=_tmpB0;
_tmpAF=ps;goto _LL14;}case 16U: _LL13: _tmpAF=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmpAD)->f2;_LL14: {struct Cyc_List_List*ps=_tmpAF;
# 442
for(0;ps != 0;ps=ps->tl){
Cyc_CurRgn_rgnsubpat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 9U: _LL15: _LL16:
# 446
 goto _LL18;case 10U: _LL17: _LL18:
 goto _LL1A;case 11U: _LL19: _LL1A:
 goto _LL1C;case 12U: _LL1B: _LL1C:
 goto _LL1E;case 13U: _LL1D: _LL1E:
 goto _LL20;case 14U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return;default: _LL23: _tmpAE=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmpAD)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmpAE;
# 455
Cyc_CurRgn_rgnsubexp(env,e);
return;}}_LL0:;}}struct _tuple15{void*f1;void*f2;};
# 460
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Aggrdecl*ad){
Cyc_CurRgn_check_tvars(loc,ad->tvs);
if(ad->impl != 0){
Cyc_CurRgn_check_tvars(loc,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);{
struct Cyc_List_List*_tmpBD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*ps=_tmpBD;
for(0;ps != 0;ps=ps->tl){
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple15*)ps->hd)).f1);
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple15*)ps->hd)).f2);}{
# 469
struct Cyc_List_List*_tmpBE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmpBE;
for(0;fs != 0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}}}}}
# 477
static void Cyc_CurRgn_rgnsubdtdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Datatypedecl*tud){
Cyc_CurRgn_check_tvars(loc,tud->tvs);
if(tud->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_List_List*_tmpBF=((struct Cyc_Absyn_Datatypefield*)fs->hd)->typs;struct Cyc_List_List*ts=_tmpBF;
while(ts != 0){
struct _tuple12*_tmpC0=(struct _tuple12*)ts->hd;struct _tuple12*_stmttmp8=_tmpC0;struct _tuple12*_tmpC1=_stmttmp8;void**_tmpC2;_LL1: _tmpC2=(void**)& _tmpC1->f2;_LL2: {void**typ=_tmpC2;
Cyc_CurRgn_rgnsubtype(loc,env,*typ);
ts=ts->tl;}}}}}
# 491
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds);
static void Cyc_CurRgn_rgnsubdecl(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Decl*d){
void*_tmpC3=d->r;void*_stmttmp9=_tmpC3;void*_tmpC4=_stmttmp9;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Pat*_tmpC6;struct Cyc_List_List*_tmpC8;struct Cyc_List_List*_tmpCA;struct _fat_ptr*_tmpC9;struct Cyc_Absyn_Datatypedecl*_tmpCB;struct Cyc_Absyn_Aggrdecl*_tmpCC;struct Cyc_Absyn_Typedefdecl*_tmpCD;struct Cyc_Absyn_Fndecl*_tmpCE;struct Cyc_Absyn_Vardecl*_tmpCF;switch(*((int*)_tmpC4)){case 0U: _LL1: _tmpCF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmpCF;
# 495
struct Cyc_Absyn_Vardecl*_tmpD0=vd;struct Cyc_Absyn_Exp*_tmpD2;void**_tmpD1;_LL26: _tmpD1=(void**)& _tmpD0->type;_tmpD2=_tmpD0->initializer;_LL27: {void**t=_tmpD1;struct Cyc_Absyn_Exp*initializer=_tmpD2;
Cyc_CurRgn_rgnsubexpopt(env,initializer);
Cyc_CurRgn_rgnsubtype(d->loc,env,*t);
if(!env.toplevel)
({void*_tmp12E=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmp12E;});
return;}}case 1U: _LL3: _tmpCE=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmpCE;
# 508
env.toplevel=0;
({struct Cyc_List_List*_tmp132=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct _tuple11*_tmp131=({struct _tuple11*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),_tmpD3->f2=(void*)_check_null(Cyc_CurRgn_curr_rgn_typ);_tmpD3;});_tmpD6->hd=_tmp131;}),({
# 511
struct Cyc_List_List*_tmp130=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct _tuple11*_tmp12F=({struct _tuple11*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar),_tmpD4->f2=(void*)_check_null(Cyc_CurRgn_curr_poolrgn_typ);_tmpD4;});_tmpD5->hd=_tmp12F;}),_tmpD5->tl=0;_tmpD5;});_tmpD6->tl=_tmp130;});_tmpD6;});
# 509
env.inst=_tmp132;});
# 513
Cyc_CurRgn_rgnsubstmt(env,fd->body);
# 516
return;}case 9U: _LL5: _tmpCD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmpCD;
# 519
env.toplevel?0:({struct _fat_ptr _tmp133=({const char*_tmpD7="env.toplevel";_tag_fat(_tmpD7,sizeof(char),13U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp133,({const char*_tmpD8="currgn.cyc";_tag_fat(_tmpD8,sizeof(char),11U);}),519U);});
Cyc_CurRgn_check_tvars(d->loc,td->tvs);
if(td->defn != 0)Cyc_CurRgn_rgnsubtype(d->loc,env,(void*)_check_null(td->defn));
return;}case 6U: _LL7: _tmpCC=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL8: {struct Cyc_Absyn_Aggrdecl*ad=_tmpCC;
# 525
env.toplevel?0:({struct _fat_ptr _tmp134=({const char*_tmpD9="env.toplevel";_tag_fat(_tmpD9,sizeof(char),13U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp134,({const char*_tmpDA="currgn.cyc";_tag_fat(_tmpDA,sizeof(char),11U);}),525U);});
Cyc_CurRgn_rgnsubaggrdecl(d->loc,env,ad);
return;}case 7U: _LL9: _tmpCB=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LLA: {struct Cyc_Absyn_Datatypedecl*tud=_tmpCB;
# 530
env.toplevel?0:({struct _fat_ptr _tmp135=({const char*_tmpDB="env.toplevel";_tag_fat(_tmpDB,sizeof(char),13U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp135,({const char*_tmpDC="currgn.cyc";_tag_fat(_tmpDC,sizeof(char),11U);}),530U);});
Cyc_CurRgn_rgnsubdtdecl(d->loc,env,tud);
return;}case 12U: _LLB: _LLC:
# 536
 goto _LLE;case 13U: _LLD: _LLE:
 goto _LL10;case 8U: _LLF: _LL10:
 goto _LL12;case 14U: _LL11: _LL12:
 goto _LL14;case 15U: _LL13: _LL14:
 goto _LL16;case 16U: _LL15: _LL16:
 goto _LL18;case 17U: _LL17: _LL18:
 return;case 10U: _LL19: _tmpC9=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_tmpCA=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC4)->f2;_LL1A: {struct _fat_ptr*v=_tmpC9;struct Cyc_List_List*ds2=_tmpCA;
# 544
_tmpC8=ds2;goto _LL1C;}case 11U: _LL1B: _tmpC8=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpC4)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmpC8;
Cyc_CurRgn_rgnsubdecls(env,ds2);return;}case 2U: _LL1D: _tmpC6=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_tmpC7=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC4)->f3;_LL1E: {struct Cyc_Absyn_Pat*p=_tmpC6;struct Cyc_Absyn_Exp*e=_tmpC7;
# 548
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexp(env,e);
return;}case 3U: _LL1F: _tmpC5=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL20: {struct Cyc_List_List*vds=_tmpC5;
# 553
!env.toplevel?0:({struct _fat_ptr _tmp136=({const char*_tmpDD="!env.toplevel";_tag_fat(_tmpDD,sizeof(char),14U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp136,({const char*_tmpDE="currgn.cyc";_tag_fat(_tmpDE,sizeof(char),11U);}),553U);});
for(0;vds != 0;vds=vds->tl){
Cyc_CurRgn_rgnsubtype(d->loc,env,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);
({void*_tmp137=Cyc_Tcutil_substitute_nofun(env.inst,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);((struct Cyc_Absyn_Vardecl*)vds->hd)->type=_tmp137;});}
# 558
return;}case 5U: _LL21: _LL22:
 goto _LL24;default: _LL23: _LL24:
# 561
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp138=({const char*_tmpDF="internal compiler error: region or pool at toplevel";_tag_fat(_tmpDF,sizeof(char),52U);});_tmpE0->f1=_tmp138;});_tmpE0;}));}_LL0:;}
# 575 "currgn.cyc"
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Stmt*s){
void*_tmpE1=s->r;void*_stmttmpA=_tmpE1;void*_tmpE2=_stmttmpA;struct Cyc_Absyn_Stmt*_tmpE4;struct Cyc_Absyn_Decl*_tmpE3;struct Cyc_List_List*_tmpE6;struct Cyc_Absyn_Stmt*_tmpE5;struct Cyc_List_List*_tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_List_List*_tmpE9;struct Cyc_Absyn_Stmt*_tmpEB;struct _fat_ptr*_tmpEA;struct Cyc_Absyn_Stmt*_tmpF1;struct Cyc_Absyn_Stmt*_tmpF0;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Stmt*_tmpEE;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Stmt*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Stmt*_tmpF4;struct Cyc_Absyn_Stmt*_tmpF8;struct Cyc_Absyn_Stmt*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Stmt*_tmpFB;struct Cyc_Absyn_Stmt*_tmpFA;struct Cyc_Absyn_Exp*_tmpFC;switch(*((int*)_tmpE2)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 goto _LL8;case 8U: _LL7: _LL8:
 return;case 1U: _LL9: _tmpFC=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmpFC;
# 582
Cyc_CurRgn_rgnsubexp(env,e);return;}case 2U: _LLB: _tmpFA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_tmpFB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2;_LLC: {struct Cyc_Absyn_Stmt*s1=_tmpFA;struct Cyc_Absyn_Stmt*s2=_tmpFB;
Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 3U: _LLD: _tmpF9=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_LLE: {struct Cyc_Absyn_Exp*eopt=_tmpF9;
Cyc_CurRgn_rgnsubexpopt(env,eopt);return;}case 4U: _LLF: _tmpF6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_tmpF7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2;_tmpF8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpE2)->f3;_LL10: {struct Cyc_Absyn_Exp*e=_tmpF6;struct Cyc_Absyn_Stmt*s1=_tmpF7;struct Cyc_Absyn_Stmt*s2=_tmpF8;
# 587
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 14U: _LL11: _tmpF4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_tmpF5=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmpF4;struct Cyc_Absyn_Exp*e=_tmpF5;
# 589
_tmpF2=e;_tmpF3=s;goto _LL14;}case 5U: _LL13: _tmpF2=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1).f1;_tmpF3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_tmpF2;struct Cyc_Absyn_Stmt*s1=_tmpF3;
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 9U: _LL15: _tmpEC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_tmpED=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2).f1;_tmpEE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2).f2;_tmpEF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpE2)->f3).f1;_tmpF0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpE2)->f3).f2;_tmpF1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpE2)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpEC;struct Cyc_Absyn_Exp*e2=_tmpED;struct Cyc_Absyn_Stmt*guard_s=_tmpEE;struct Cyc_Absyn_Exp*e3=_tmpEF;struct Cyc_Absyn_Stmt*cont_s=_tmpF0;struct Cyc_Absyn_Stmt*s1=_tmpF1;
# 593
Cyc_CurRgn_rgnsubexp(env,e1);
Cyc_CurRgn_rgnsubexp(env,e2);
Cyc_CurRgn_rgnsubexp(env,e3);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;}case 13U: _LL17: _tmpEA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_tmpEB=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2;_LL18: {struct _fat_ptr*l=_tmpEA;struct Cyc_Absyn_Stmt*s1=_tmpEB;
# 599
Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 11U: _LL19: _tmpE9=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_LL1A: {struct Cyc_List_List*es=_tmpE9;
# 602
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 606
return;}case 10U: _LL1B: _tmpE7=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_tmpE8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmpE7;struct Cyc_List_List*scs=_tmpE8;
# 609
Cyc_CurRgn_rgnsubexp(env,e);
while(scs != 0){
struct Cyc_Absyn_Switch_clause*_tmpFD=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpB=_tmpFD;struct Cyc_Absyn_Switch_clause*_tmpFE=_stmttmpB;struct Cyc_Absyn_Stmt*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Pat*_tmpFF;_LL22: _tmpFF=_tmpFE->pattern;_tmp100=_tmpFE->where_clause;_tmp101=_tmpFE->body;_LL23: {struct Cyc_Absyn_Pat*p=_tmpFF;struct Cyc_Absyn_Exp*eopt=_tmp100;struct Cyc_Absyn_Stmt*s1=_tmp101;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 617
return;}case 15U: _LL1D: _tmpE5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_tmpE6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2;_LL1E: {struct Cyc_Absyn_Stmt*s1=_tmpE5;struct Cyc_List_List*scs=_tmpE6;
# 620
while(scs != 0){
struct Cyc_Absyn_Switch_clause*_tmp102=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpC=_tmp102;struct Cyc_Absyn_Switch_clause*_tmp103=_stmttmpC;struct Cyc_Absyn_Stmt*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Pat*_tmp104;_LL25: _tmp104=_tmp103->pattern;_tmp105=_tmp103->where_clause;_tmp106=_tmp103->body;_LL26: {struct Cyc_Absyn_Pat*p=_tmp104;struct Cyc_Absyn_Exp*eopt=_tmp105;struct Cyc_Absyn_Stmt*s1=_tmp106;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 627
return;}default: _LL1F: _tmpE3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpE2)->f1;_tmpE4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpE2)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmpE3;struct Cyc_Absyn_Stmt*s1=_tmpE4;
# 630
void*_tmp107=d->r;void*_stmttmpD=_tmp107;void*_tmp108=_stmttmpD;struct Cyc_Absyn_Tvar*_tmp109;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Vardecl*_tmp10B;struct Cyc_Absyn_Tvar*_tmp10A;switch(*((int*)_tmp108)){case 4U: _LL28: _tmp10A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp108)->f1;_tmp10B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp108)->f2;_tmp10C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp108)->f3;_LL29: {struct Cyc_Absyn_Tvar*tv=_tmp10A;struct Cyc_Absyn_Vardecl*vd=_tmp10B;struct Cyc_Absyn_Exp*open_exp_opt=_tmp10C;
# 632
Cyc_CurRgn_check_tvar(d->loc,tv);
Cyc_CurRgn_rgnsubexpopt(env,open_exp_opt);
# 637
if(open_exp_opt == 0){
({struct Cyc_List_List*_tmp13A=env.inst;struct Cyc_Absyn_Tvar*_tmp139=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar);Cyc_CurRgn_subst_inst(_tmp13A,_tmp139,Cyc_Absyn_var_type(tv));});
Cyc_CurRgn_rgnsubstmt(env,s1);}else{
# 642
Cyc_CurRgn_rgnsubstmt(env,s1);}
return;}case 5U: _LL2A: _tmp109=((struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct*)_tmp108)->f1;_LL2B: {struct Cyc_Absyn_Tvar*tv=_tmp109;
# 646
Cyc_CurRgn_check_tvar(d->loc,tv);
({struct Cyc_List_List*_tmp13C=env.inst;struct Cyc_Absyn_Tvar*_tmp13B=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_pool_tvar);Cyc_CurRgn_subst_inst(_tmp13C,_tmp13B,Cyc_Absyn_var_type(tv));});
Cyc_CurRgn_rgnsubstmt(env,s1);
return;}default: _LL2C: _LL2D:
# 652
 Cyc_CurRgn_rgnsubdecl(env,d);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;}_LL27:;}}_LL0:;}
# 669 "currgn.cyc"
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}}
# 675
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*tds){
Cyc_CurRgn_curr_poolrgn_type();Cyc_CurRgn_curr_rgn_type();{
struct Cyc_CurRgn_rgnsubenv env=({struct Cyc_CurRgn_rgnsubenv _tmp10F;_tmp10F.inst=0,_tmp10F.in_externC=0,_tmp10F.toplevel=1;_tmp10F;});
Cyc_CurRgn_rgnsubdecls(env,tds);}}

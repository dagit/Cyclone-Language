#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator  The corresponding definitions are in file lib/runtime_*.c */
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
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
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
void* _region_vmalloc(struct _RegionHandle*, unsigned);
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

void* Cyc_Core_get_exn_thrown();
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
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

#ifdef CYC_GC_PTHREAD_REDIRECTS
# define pthread_create GC_pthread_create
# define pthread_sigmask GC_pthread_sigmask
# define pthread_join GC_pthread_join
# define pthread_detach GC_pthread_detach
# define dlopen GC_dlopen
#endif
/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);

#if(defined(__linux__) && defined(__KERNEL__))
void *cyc_vmalloc(unsigned);
void cyc_vfree(void *);
#endif
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

static inline unsigned int _check_times(unsigned x, unsigned y) {
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 173
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 181
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 465 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 504
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 853 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 870
void*Cyc_Absyn_compress(void*);
# 901
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];extern char Cyc_Tcenv_Env_error[10U];
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void);
void*Cyc_CurRgn_curr_rgn_type (void);
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*,void*);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 33 "currgn.cyc"
static struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_tvar=0;
static void*Cyc_CurRgn_curr_rgn_typ=0;static char _TmpG0[3U]="`C";
# 36
struct _fat_ptr Cyc_CurRgn_curr_rgn_name={_TmpG0,_TmpG0,_TmpG0 + 3U};
# 38
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void){
if(Cyc_CurRgn_curr_tvar == 0){
Cyc_CurRgn_curr_tvar=({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=Cyc_CurRgn_curr_rgn_name;_Tmp2;});_Tmp0->name=_Tmp1;}),({
int _Tmp1=Cyc_Tcutil_new_tvar_id();_Tmp0->identity=_Tmp1;}),({
void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_Tmp0->kind=_Tmp1;});_Tmp0;});
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type(_check_null(Cyc_CurRgn_curr_tvar));}
# 45
return _check_null(Cyc_CurRgn_curr_tvar);}
# 48
void*Cyc_CurRgn_curr_rgn_type (void){
Cyc_CurRgn_curr_rgn_tvar();
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type(_check_null(Cyc_CurRgn_curr_tvar));
return _check_null(Cyc_CurRgn_curr_rgn_typ);}struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 54
void Cyc_CurRgn_subst_inst(struct Cyc_List_List*inst,struct Cyc_Absyn_Tvar*tv,void*t){
while(inst != 0){
struct _tuple11*_stmttmp0=(struct _tuple11*)inst->hd;struct _tuple11*_Tmp0=_stmttmp0;void*_Tmp1;void*_Tmp2;_Tmp2=(struct Cyc_Absyn_Tvar**)& _Tmp0->f1;_Tmp1=(void**)& _Tmp0->f2;{struct Cyc_Absyn_Tvar**tvi=(struct Cyc_Absyn_Tvar**)_Tmp2;void**ti=(void**)_Tmp1;
if(Cyc_Absyn_tvar_cmp(*tvi,tv)== 0){
*ti=t;
return;}
# 61
inst=inst->tl;}}}
# 68
static struct _tuple8*Cyc_CurRgn_argtype(struct _tuple8*arg){
struct _tuple8*_Tmp0=arg;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{struct _fat_ptr*vopt=_Tmp3;struct Cyc_Absyn_Tqual tq=_Tmp2;void*t=_Tmp1;
{void*_Tmp4=t;void*_Tmp5;if(*((int*)_Tmp4)== 3){_Tmp5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1).elt_type;{void*t1=_Tmp5;
# 72
if(!Cyc_Tcutil_is_function_type(t1))return arg;
goto _LL3;}}else{
return arg;}_LL3:;}{
# 76
void*t2=Cyc_Tcutil_copy_type(t);
void*_stmttmp1=Cyc_Absyn_compress(t2);void*_Tmp4=_stmttmp1;void*_Tmp5;if(*((int*)_Tmp4)== 3){_Tmp5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1).elt_type;{void*t1=_Tmp5;
# 79
void*_stmttmp2=Cyc_Absyn_compress(t1);void*_Tmp6=_stmttmp2;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;int _TmpF;void*_Tmp10;void*_Tmp11;struct Cyc_Absyn_Tqual _Tmp12;void*_Tmp13;void*_Tmp14;if(*((int*)_Tmp6)== 5){_Tmp14=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).tvars;_Tmp13=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).effect;_Tmp12=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).ret_tqual;_Tmp11=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).ret_type;_Tmp10=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).args;_TmpF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).c_varargs;_TmpE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).cyc_varargs;_TmpD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).rgn_po;_TmpC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).attributes;_TmpB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).requires_clause;_TmpA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).requires_relns;_Tmp9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).ensures_clause;_Tmp8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).ensures_relns;_Tmp7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp6)->f1).return_value;{struct Cyc_List_List**tvars=(struct Cyc_List_List**)_Tmp14;void*eff=_Tmp13;struct Cyc_Absyn_Tqual res_tq=_Tmp12;void*res_typ=_Tmp11;struct Cyc_List_List*args_info=_Tmp10;int c_vararg=_TmpF;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpE;struct Cyc_List_List*rgn_po=_TmpD;struct Cyc_List_List*atts=_TmpC;struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_List_List*req_relns=_TmpA;struct Cyc_Absyn_Exp*ens=_Tmp9;struct Cyc_List_List*ens_relns=_Tmp8;struct Cyc_Absyn_Vardecl*ret_var=_Tmp7;
# 85
struct Cyc_List_List*tvars2=*tvars;
({struct Cyc_List_List*_Tmp15=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));_Tmp16->hd=_check_null(Cyc_CurRgn_curr_tvar),_Tmp16->tl=tvars2;_Tmp16;});*tvars=_Tmp15;});
return({struct _tuple8*_Tmp15=_cycalloc(sizeof(struct _tuple8));_Tmp15->f1=vopt,_Tmp15->f2=tq,_Tmp15->f3=t2;_Tmp15;});}}else{
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_Tmp15->tag=Cyc_Core_Impossible,_Tmp15->f1=({const char*_Tmp16="internal compiler error: not a function type";_tag_fat(_Tmp16,sizeof(char),45U);});_Tmp15;}));};}}else{
# 90
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_Tmp6->tag=Cyc_Core_Impossible,_Tmp6->f1=({const char*_Tmp7="internal compiler error: not a pointer type";_tag_fat(_Tmp7,sizeof(char),44U);});_Tmp6;}));};}}}
# 95
void*Cyc_CurRgn_instantiate(void*fntype,void*currgn){
void*_Tmp0=fntype;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;int _Tmp9;void*_TmpA;void*_TmpB;struct Cyc_Absyn_Tqual _TmpC;void*_TmpD;void*_TmpE;if(*((int*)_Tmp0)== 5){_TmpE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).tvars;_TmpD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).effect;_TmpC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ret_tqual;_TmpB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ret_type;_TmpA=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).args;_Tmp9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).c_varargs;_Tmp8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).cyc_varargs;_Tmp7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).rgn_po;_Tmp6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).attributes;_Tmp5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).requires_clause;_Tmp4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).requires_relns;_Tmp3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ensures_clause;_Tmp2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ensures_relns;_Tmp1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).return_value;{struct Cyc_List_List*tvars=_TmpE;void*eff=_TmpD;struct Cyc_Absyn_Tqual res_tq=_TmpC;void*res_typ=_TmpB;struct Cyc_List_List**args_info=(struct Cyc_List_List**)_TmpA;int c_vararg=_Tmp9;struct Cyc_Absyn_VarargInfo*cyc_vararg=_Tmp8;struct Cyc_List_List*rgn_po=_Tmp7;struct Cyc_List_List*atts=_Tmp6;struct Cyc_Absyn_Exp*req=_Tmp5;struct Cyc_List_List*req_relns=_Tmp4;struct Cyc_Absyn_Exp*ens=_Tmp3;struct Cyc_List_List*ens_relns=_Tmp2;struct Cyc_Absyn_Vardecl*ret_var=_Tmp1;
# 103
struct Cyc_List_List*old_args_info=*args_info;
# 105
({struct Cyc_List_List*_TmpF=({(struct Cyc_List_List*(*)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_CurRgn_argtype,*args_info);*args_info=_TmpF;});{
# 107
struct Cyc_List_List*inst=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple11*_Tmp10=({struct _tuple11*_Tmp11=_cycalloc(sizeof(struct _tuple11));_Tmp11->f1=_check_null(Cyc_CurRgn_curr_tvar),_Tmp11->f2=currgn;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=0;_TmpF;});
void*fntype2=Cyc_Tcutil_substitute(inst,fntype);
# 110
{void*_TmpF=fntype2;void*_Tmp10;if(*((int*)_TmpF)== 5){_Tmp10=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TmpF)->f1).args;{struct Cyc_List_List**args_info2=(struct Cyc_List_List**)_Tmp10;
# 112
*args_info2=old_args_info;
goto _LL5;}}else{
goto _LL5;}_LL5:;}
# 116
*args_info=old_args_info;
return fntype2;}}}else{
return fntype;};}
# 123
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*v){
return Cyc_Absyn_tvar_cmp(v,_check_null(Cyc_CurRgn_curr_tvar))!= 0;}
# 128
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*v){
return Cyc_Absyn_tvar_cmp(v,_check_null(Cyc_CurRgn_curr_tvar))== 0;}struct Cyc_CurRgn_rgnsubenv{struct Cyc_List_List*inst;int in_externC;int toplevel;};
# 138
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Stmt*);
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Exp*);
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Exp*);
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);
# 143
static void Cyc_CurRgn_rgnsubdtdecl(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Datatypedecl*);
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Aggrdecl*);
# 147
static void Cyc_CurRgn_check_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
if(Cyc_Absyn_tvar_cmp(tv,_check_null(Cyc_CurRgn_curr_tvar))== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="Cannot abstract special type variable ";_tag_fat(_Tmp2,sizeof(char),39U);});_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=_check_null(Cyc_CurRgn_curr_tvar);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});}
# 151
static void Cyc_CurRgn_check_tvars(unsigned loc,struct Cyc_List_List*tvs){
for(1;tvs != 0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}}
# 156
static void Cyc_CurRgn_rgnsubtypes(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 159
static void Cyc_CurRgn_rgnsubtype(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,void*type){
void*_stmttmp3=Cyc_Absyn_compress(type);void*_Tmp0=_stmttmp3;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 2: _Tmp5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*v=_Tmp5;
# 162
if(env.toplevel)Cyc_CurRgn_check_tvar(loc,v);return;}case 8: _Tmp5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 4: _Tmp5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).num_elts;_Tmp3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zero_term;{void*t1=_Tmp5;struct Cyc_Absyn_Exp*e=_Tmp4;void*zt=_Tmp3;
# 165
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubexpopt(env,e);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
return;}case 3: _Tmp5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).rgn;_Tmp3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).bounds;_Tmp2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).zero_term;_Tmp1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).autoreleased;{void*t1=_Tmp5;void*r=_Tmp4;void*b=_Tmp3;void*zt=_Tmp2;void*rel=_Tmp1;
# 170
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubtype(loc,env,r);
Cyc_CurRgn_rgnsubtype(loc,env,b);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
Cyc_CurRgn_rgnsubtype(loc,env,rel);
return;}case 5:
# 177
 return;case 6: _Tmp5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqts=_Tmp5;
# 179
{struct Cyc_List_List*ts1=tqts;for(0;ts1 != 0;ts1=ts1->tl){
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple12*)ts1->hd)).f2);}}
# 182
return;}case 7: _Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*fs=_Tmp5;
# 184
for(1;fs != 0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}
# 188
return;}case 1: _Tmp5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;{void*r=_Tmp5;
# 190
if(r != 0)Cyc_CurRgn_rgnsubtype(loc,env,r);
return;}case 0: _Tmp5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 9: _Tmp5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
Cyc_CurRgn_rgnsubexp(env,e);return;}case 11: _Tmp5=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
Cyc_CurRgn_rgnsubexp(env,e);return;}default: _Tmp5=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_TypeDecl*td=_Tmp5;void**topt=_Tmp4;
# 196
topt == 0?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp6="topt == NULL";_tag_fat(_Tmp6,sizeof(char),13U);}),({const char*_Tmp6="currgn.cyc";_tag_fat(_Tmp6,sizeof(char),11U);}),196U);
{void*_stmttmp4=td->r;void*_Tmp6=_stmttmp4;void*_Tmp7;switch(*((int*)_Tmp6)){case 0: _Tmp7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_Tmp6)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
Cyc_CurRgn_rgnsubaggrdecl(loc,env,ad);return;}case 2: _Tmp7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_Tmp6)->f1;{struct Cyc_Absyn_Datatypedecl*dtd=_Tmp7;
Cyc_CurRgn_rgnsubdtdecl(loc,env,dtd);return;}default:
 return;};}
# 202
return;}};}
# 206
static void Cyc_CurRgn_rgnsubtypes(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ts){
# 208
for(1;ts != 0;ts=ts->tl){Cyc_CurRgn_rgnsubtype(loc,env,(void*)ts->hd);}}
# 213
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
{void*_stmttmp5=e->r;void*_Tmp0=_stmttmp5;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 33: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 40: _LLA:
 goto _LLC;case 1: _LLC:
 goto _LL0;case 19: _Tmp4=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void**t=_Tmp4;struct Cyc_List_List*f=_Tmp3;
# 222
_Tmp4=t;goto _LL10;}case 17: _Tmp4=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL10: {void**t=_Tmp4;
# 224
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_Tmp5=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_Tmp5;});
goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 230
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);Cyc_CurRgn_rgnsubexp(env,e3);goto _LL0;}case 27: _Tmp4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;int res=_Tmp1;
# 232
_Tmp4=e1;_Tmp3=e2;goto _LL16;}case 4: _Tmp4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL16: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp4=e1;_Tmp3=e2;goto _LL18;}case 35: _Tmp4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp4=e1;_Tmp3=e2;goto _LL1A;}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp4=e1;_Tmp3=e2;goto _LL1C;}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp4=e1;_Tmp3=e2;goto _LL1E;}case 23: _Tmp4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp4=e1;_Tmp3=e2;goto _LL20;}case 9: _Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL20: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 239
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);goto _LL0;}case 31: _Tmp4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp4;struct Cyc_Absyn_Datatypedecl*dtd=_Tmp3;struct Cyc_Absyn_Datatypefield*dtfd=_Tmp2;
# 241
_Tmp4=es;goto _LL24;}case 3: _Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL24: {struct Cyc_List_List*es=_Tmp4;
# 243
_Tmp4=es;goto _LL26;}case 24: _Tmp4=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL26: {struct Cyc_List_List*es=_Tmp4;
# 245
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 249
goto _LL0;}case 28: _Tmp4=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=(void**)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;void**t=_Tmp3;
# 251
_Tmp4=t;_Tmp3=e1;goto _LL2A;}case 14: _Tmp4=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL2A: {void**t=_Tmp4;struct Cyc_Absyn_Exp*e1=_Tmp3;
# 253
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_Tmp5=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_Tmp5;});
_Tmp4=e1;goto _LL2C;}case 21: _Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_Tmp4;
# 258
_Tmp4=e1;goto _LL2E;}case 22: _Tmp4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL30;}case 38: _Tmp4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL32;}case 5: _Tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL34;}case 42: _Tmp4=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL36;}case 20: _Tmp4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL38;}case 15: _Tmp4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL3A;}case 18: _Tmp4=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL3C;}case 41: _Tmp4=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL3E;}case 11: _Tmp4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3E: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL40;}case 12: _Tmp4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL40: {struct Cyc_Absyn_Exp*e1=_Tmp4;
Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 39: _Tmp4=(void**)&((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void**t=_Tmp4;
# 270
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel){
void*new_typ=Cyc_Tcutil_substitute_nofun(env.inst,*t);
if(new_typ == *t)goto _LL0;{
# 275
void*_stmttmp6=Cyc_Absyn_compress(new_typ);void*_Tmp5=_stmttmp6;void*_Tmp6;if(*((int*)_Tmp5)== 9){_Tmp6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 277
e->r=e1->r;
e->topt=e1->topt;
e->annot=e1->annot;
goto _LL57;}}else{
*t=new_typ;goto _LL57;}_LL57:;}}
# 284
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_List_List*es=_Tmp3;struct Cyc_Absyn_VarargCallInfo*vararg=_Tmp2;int resv=_Tmp1;
# 286
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 290
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 13: _Tmp4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 293
while(ts != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_Tmp5=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_Tmp5;});
ts=ts->tl;}
# 299
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 16: _Tmp4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*eopt=_Tmp4;struct Cyc_Absyn_Exp*e1=_Tmp3;
# 303
Cyc_CurRgn_rgnsubexpopt(env,eopt);Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 25: _Tmp4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple8*i=_Tmp4;struct Cyc_List_List*ds=_Tmp3;
# 306
Cyc_CurRgn_rgnsubtype(e->loc,env,(*i).f3);
if(!env.toplevel)
({void*_Tmp5=Cyc_Tcutil_substitute_nofun(env.inst,(*i).f3);(*i).f3=_Tmp5;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 36: _Tmp4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*ds=_Tmp4;
# 312
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 26: _Tmp4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds=_Tmp4;
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 29: _Tmp4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*ts=_Tmp4;struct Cyc_List_List*ds=_Tmp3;
# 316
for(1;ts != 0;ts=ts->tl){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_Tmp5=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_Tmp5;});}
# 321
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 30: _Tmp4=(void**)&((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void**t=_Tmp4;struct Cyc_List_List*ds=_Tmp3;
# 325
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_Tmp5=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_Tmp5;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 34: _Tmp4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).rgn;_Tmp3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).elt_type;_Tmp2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).num_elts;{struct Cyc_Absyn_Exp*rgnopt=_Tmp4;void**elt_t=_Tmp3;struct Cyc_Absyn_Exp*e1=_Tmp2;
# 332
!env.toplevel?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp5="!env.toplevel";_tag_fat(_Tmp5,sizeof(char),14U);}),({const char*_Tmp5="currgn.cyc";_tag_fat(_Tmp5,sizeof(char),11U);}),332U);
Cyc_CurRgn_rgnsubexpopt(env,rgnopt);
if(elt_t != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,*elt_t);
({void*_Tmp5=Cyc_Tcutil_substitute_nofun(env.inst,*elt_t);*elt_t=_Tmp5;});}
# 338
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}default: _Tmp4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp4;
# 341
Cyc_CurRgn_rgnsubstmt(env,s);goto _LL0;}}_LL0:;}
# 343
if(e->topt != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,_check_null(e->topt));
if(!env.toplevel)
({void*_Tmp0=Cyc_Tcutil_substitute_nofun(env.inst,_check_null(e->topt));e->topt=_Tmp0;});}}
# 360 "currgn.cyc"
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
if(e == 0)return;else{
Cyc_CurRgn_rgnsubexp(env,e);}}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 365
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
# 367
for(1;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubexp(env,(*((struct _tuple13*)ds->hd)).f2);}}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 371
static void Cyc_CurRgn_rgnsubpat(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Pat*p){
!env.toplevel?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp0="!env.toplevel";_tag_fat(_Tmp0,sizeof(char),14U);}),({const char*_Tmp0="currgn.cyc";_tag_fat(_Tmp0,sizeof(char),11U);}),372U);{
void*_stmttmp7=p->r;void*_Tmp0=_stmttmp7;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0:
 return;case 1: _Tmp2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;struct Cyc_Absyn_Pat*p1=_Tmp1;
# 376
_Tmp2=vd;_Tmp1=p1;goto _LL6;}case 3: _Tmp2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;struct Cyc_Absyn_Pat*p1=_Tmp1;
# 379
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_Tmp3=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_Tmp3;});}
# 383
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 2: _Tmp2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp2;struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 387
Cyc_CurRgn_check_tvar(p->loc,tv);
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_Tmp3=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_Tmp3;});}
# 392
return;}case 4: _Tmp2=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp2;struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 396
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_Tmp3=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_Tmp3;});}
# 400
Cyc_CurRgn_check_tvar(p->loc,tv);
return;}case 6: _Tmp2=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p1=_Tmp2;
# 404
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 7: _Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*ds=_Tmp1;
# 408
for(1;tvs != 0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(p->loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}
for(1;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubpat(env,(*((struct _tuple14*)ds->hd)).f2);}
# 413
return;}case 8: _Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_List_List*ps=_Tmp2;
# 416
_Tmp2=ps;goto _LL12;}case 5: _Tmp2=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_LL12: {struct Cyc_List_List*ps=_Tmp2;
_Tmp2=ps;goto _LL14;}case 16: _Tmp2=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL14: {struct Cyc_List_List*ps=_Tmp2;
# 419
for(1;ps != 0;ps=ps->tl){
Cyc_CurRgn_rgnsubpat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 9:
# 423
 goto _LL18;case 10: _LL18:
 goto _LL1A;case 11: _LL1A:
 goto _LL1C;case 12: _LL1C:
 goto _LL1E;case 13: _LL1E:
 goto _LL20;case 14: _LL20:
 goto _LL22;case 15: _LL22:
 return;default: _Tmp2=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
# 432
Cyc_CurRgn_rgnsubexp(env,e);
return;}};}}struct _tuple15{void*f1;void*f2;};
# 437
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Aggrdecl*ad){
Cyc_CurRgn_check_tvars(loc,ad->tvs);
if(ad->impl != 0){
Cyc_CurRgn_check_tvars(loc,(_check_null(ad->impl))->exist_vars);{
struct Cyc_List_List*ps=(_check_null(ad->impl))->rgn_po;
for(1;ps != 0;ps=ps->tl){
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple15*)ps->hd)).f1);
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple15*)ps->hd)).f2);}{
# 446
struct Cyc_List_List*fs=(_check_null(ad->impl))->fields;
for(1;fs != 0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}}}}}
# 454
static void Cyc_CurRgn_rgnsubdtdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Datatypedecl*tud){
Cyc_CurRgn_check_tvars(loc,tud->tvs);
if(tud->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)(_check_null(tud->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_List_List*ts=((struct Cyc_Absyn_Datatypefield*)fs->hd)->typs;
while(ts != 0){
struct _tuple12*_stmttmp8=(struct _tuple12*)ts->hd;struct _tuple12*_Tmp0=_stmttmp8;void*_Tmp1;_Tmp1=(void**)& _Tmp0->f2;{void**typ=(void**)_Tmp1;
Cyc_CurRgn_rgnsubtype(loc,env,*typ);
ts=ts->tl;}}}}}
# 468
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);
static void Cyc_CurRgn_rgnsubdecl(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Decl*d){
void*_stmttmp9=d->r;void*_Tmp0=_stmttmp9;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 472
struct Cyc_Absyn_Vardecl*_Tmp3=vd;void*_Tmp4;void*_Tmp5;_Tmp5=(void**)& _Tmp3->type;_Tmp4=_Tmp3->initializer;{void**t=(void**)_Tmp5;struct Cyc_Absyn_Exp*initializer=_Tmp4;
Cyc_CurRgn_rgnsubexpopt(env,initializer);
Cyc_CurRgn_rgnsubtype(d->loc,env,*t);
if(!env.toplevel)
({void*_Tmp6=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_Tmp6;});
return;}}case 1: _Tmp2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp2;
# 485
env.toplevel=0;
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple11*_Tmp5=({struct _tuple11*_Tmp6=_cycalloc(sizeof(struct _tuple11));_Tmp6->f1=_check_null(Cyc_CurRgn_curr_tvar),_Tmp6->f2=
_check_null(Cyc_CurRgn_curr_rgn_typ);_Tmp6;});
# 486
_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;_Tmp4;});env.inst=_Tmp3;});
# 488
Cyc_CurRgn_rgnsubstmt(env,fd->body);
# 491
return;}case 8: _Tmp2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp2;
# 494
env.toplevel?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp3="env.toplevel";_tag_fat(_Tmp3,sizeof(char),13U);}),({const char*_Tmp3="currgn.cyc";_tag_fat(_Tmp3,sizeof(char),11U);}),494U);
Cyc_CurRgn_check_tvars(d->loc,td->tvs);
if(td->defn != 0)Cyc_CurRgn_rgnsubtype(d->loc,env,_check_null(td->defn));
return;}case 5: _Tmp2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp2;
# 500
env.toplevel?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp3="env.toplevel";_tag_fat(_Tmp3,sizeof(char),13U);}),({const char*_Tmp3="currgn.cyc";_tag_fat(_Tmp3,sizeof(char),11U);}),500U);
Cyc_CurRgn_rgnsubaggrdecl(d->loc,env,ad);
return;}case 6: _Tmp2=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp2;
# 505
env.toplevel?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp3="env.toplevel";_tag_fat(_Tmp3,sizeof(char),13U);}),({const char*_Tmp3="currgn.cyc";_tag_fat(_Tmp3,sizeof(char),11U);}),505U);
Cyc_CurRgn_rgnsubdtdecl(d->loc,env,tud);
return;}case 11:
# 511
 goto _LLE;case 12: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 13: _LL12:
 goto _LL14;case 14: _LL14:
 goto _LL16;case 15: _LL16:
 goto _LL18;case 16: _LL18:
 return;case 9: _Tmp2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp2;struct Cyc_List_List*ds2=_Tmp1;
# 519
_Tmp2=ds2;goto _LL1C;}case 10: _Tmp2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_List_List*ds2=_Tmp2;
Cyc_CurRgn_rgnsubdecls(env,ds2);return;}case 2: _Tmp2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
# 523
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexp(env,e);
return;}case 3: _Tmp2=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp2;
# 528
!env.toplevel?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp3="!env.toplevel";_tag_fat(_Tmp3,sizeof(char),14U);}),({const char*_Tmp3="currgn.cyc";_tag_fat(_Tmp3,sizeof(char),11U);}),528U);
for(1;vds != 0;vds=vds->tl){
Cyc_CurRgn_rgnsubtype(d->loc,env,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);
({void*_Tmp3=Cyc_Tcutil_substitute_nofun(env.inst,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);((struct Cyc_Absyn_Vardecl*)vds->hd)->type=_Tmp3;});}
# 533
return;}default:
# 535
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp3->tag=Cyc_Core_Failure,_Tmp3->f1=({const char*_Tmp4="internal compiler error: region or pool at toplevel";_tag_fat(_Tmp4,sizeof(char),52U);});_Tmp3;}));};}
# 549 "currgn.cyc"
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Stmt*s){
void*_stmttmpA=s->r;void*_Tmp0=_stmttmpA;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 1: _Tmp6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 556
Cyc_CurRgn_rgnsubexp(env,e);return;}case 2: _Tmp6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_Absyn_Stmt*s2=_Tmp5;
Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 3: _Tmp6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp6;
Cyc_CurRgn_rgnsubexpopt(env,eopt);return;}case 4: _Tmp6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 561
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 14: _Tmp6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp5;
# 563
_Tmp6=e;_Tmp5=s;goto _LL14;}case 5: _Tmp6=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1).f1;_Tmp5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 9: _Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;_Tmp4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f2;_Tmp3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3).f1;_Tmp2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3).f2;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Stmt*guard_s=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;struct Cyc_Absyn_Stmt*cont_s=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 567
Cyc_CurRgn_rgnsubexp(env,e1);
Cyc_CurRgn_rgnsubexp(env,e2);
Cyc_CurRgn_rgnsubexp(env,e3);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;}case 13: _Tmp6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*l=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
# 573
Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 11: _Tmp6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp6;
# 576
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 580
return;}case 10: _Tmp6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_List_List*scs=_Tmp5;
# 583
Cyc_CurRgn_rgnsubexp(env,e);
while(scs != 0){
struct Cyc_Absyn_Switch_clause*_stmttmpB=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_Tmp7=_stmttmpB;void*_Tmp8;void*_Tmp9;void*_TmpA;_TmpA=_Tmp7->pattern;_Tmp9=_Tmp7->where_clause;_Tmp8=_Tmp7->body;{struct Cyc_Absyn_Pat*p=_TmpA;struct Cyc_Absyn_Exp*eopt=_Tmp9;struct Cyc_Absyn_Stmt*s1=_Tmp8;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 591
return;}case 15: _Tmp6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_List_List*scs=_Tmp5;
# 594
while(scs != 0){
struct Cyc_Absyn_Switch_clause*_stmttmpC=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_Tmp7=_stmttmpC;void*_Tmp8;void*_Tmp9;void*_TmpA;_TmpA=_Tmp7->pattern;_Tmp9=_Tmp7->where_clause;_Tmp8=_Tmp7->body;{struct Cyc_Absyn_Pat*p=_TmpA;struct Cyc_Absyn_Exp*eopt=_Tmp9;struct Cyc_Absyn_Stmt*s1=_Tmp8;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 601
return;}default: _Tmp6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
# 604
void*_stmttmpD=d->r;void*_Tmp7=_stmttmpD;void*_Tmp8;void*_Tmp9;void*_TmpA;if(*((int*)_Tmp7)== 4){_TmpA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp7)->f1;_Tmp9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp7)->f2;_Tmp8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp7)->f3;{struct Cyc_Absyn_Tvar*tv=_TmpA;struct Cyc_Absyn_Vardecl*vd=_Tmp9;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp8;
# 606
Cyc_CurRgn_check_tvar(d->loc,tv);
Cyc_CurRgn_rgnsubexpopt(env,open_exp_opt);
# 611
if(open_exp_opt == 0){
({struct Cyc_List_List*_TmpB=env.inst;struct Cyc_Absyn_Tvar*_TmpC=_check_null(Cyc_CurRgn_curr_tvar);Cyc_CurRgn_subst_inst(_TmpB,_TmpC,Cyc_Absyn_var_type(tv));});
Cyc_CurRgn_rgnsubstmt(env,s1);}else{
# 616
Cyc_CurRgn_rgnsubstmt(env,s1);}
return;}}else{
# 620
Cyc_CurRgn_rgnsubdecl(env,d);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;};}};}
# 637 "currgn.cyc"
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
for(1;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}}
# 643
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*tds){
Cyc_CurRgn_curr_rgn_type();{
struct Cyc_CurRgn_rgnsubenv env=({struct Cyc_CurRgn_rgnsubenv _Tmp0;_Tmp0.inst=0,_Tmp0.in_externC=0,_Tmp0.toplevel=1;_Tmp0;});
Cyc_CurRgn_rgnsubdecls(env,tds);}}

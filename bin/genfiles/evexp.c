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
/* should be size_t but int is fine */
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
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
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
      _curr != (unsigned char*)0) \
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
void cyc_vfree(void*);
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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 181
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 850 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 870
void*Cyc_Absyn_compress(void*);
# 888
extern void*Cyc_Absyn_uint_type;
# 960
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 962
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 973
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1133
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);struct _tuple11{unsigned f1;int f2;};
# 46 "evexp.h"
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct _tuple12{union Cyc_Absyn_Cnst f1;int f2;};
# 33 "evexp.cyc"
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 35
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
short _Tmp0;char _Tmp1;enum Cyc_Absyn_Sign _Tmp2;switch(cn.Short_c.tag){case 2: _Tmp2=cn.Char_c.val.f1;_Tmp1=cn.Char_c.val.f2;{enum Cyc_Absyn_Sign sn=_Tmp2;char c=_Tmp1;
return Cyc_Absyn_Int_c(sn,(int)c);}case 4: _Tmp2=cn.Short_c.val.f1;_Tmp0=cn.Short_c.val.f2;{enum Cyc_Absyn_Sign sn=_Tmp2;short s=_Tmp0;
# 39
return Cyc_Absyn_Int_c(sn,(int)s);}default:
 return cn;};}
# 44
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_exp(e);int _Tmp1;union Cyc_Absyn_Cnst _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_Absyn_Cnst cn=_Tmp2;int known=_Tmp1;
if(!known)
return({struct _tuple11 _Tmp3;_Tmp3.f1=0U,_Tmp3.f2=0;_Tmp3;});{
union Cyc_Absyn_Cnst _Tmp3=Cyc_Evexp_promote_const(cn);long long _Tmp4;int _Tmp5;switch(_Tmp3.Null_c.tag){case 5: _Tmp5=_Tmp3.Int_c.val.f2;{int i=_Tmp5;
return({struct _tuple11 _Tmp6;_Tmp6.f1=(unsigned)i,_Tmp6.f2=1;_Tmp6;});}case 6: _Tmp4=_Tmp3.LongLong_c.val.f2;{long long x=_Tmp4;
# 52
unsigned long long y=(unsigned long long)x >> 32U;
if(y!=4294967295U && y!=0U)
return({struct _tuple11 _Tmp6;_Tmp6.f1=0U,_Tmp6.f2=0;_Tmp6;});
return({struct _tuple11 _Tmp6;_Tmp6.f1=(unsigned)x,_Tmp6.f2=1;_Tmp6;});}case 7:
 return({struct _tuple11 _Tmp6;_Tmp6.f1=0U,_Tmp6.f2=0;_Tmp6;});case 1:
 return({struct _tuple11 _Tmp6;_Tmp6.f1=0U,_Tmp6.f2=1;_Tmp6;});default:
 return({struct _tuple11 _Tmp6;_Tmp6.f1=0U,_Tmp6.f2=0;_Tmp6;});};}}}struct _tuple13{int f1;int f2;};
# 62
static struct _tuple13 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_exp(e);int _Tmp1;union Cyc_Absyn_Cnst _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_Absyn_Cnst cn=_Tmp2;int known=_Tmp1;
if(!known)
return({struct _tuple13 _Tmp3;_Tmp3.f1=0,_Tmp3.f2=0;_Tmp3;});{
union Cyc_Absyn_Cnst _Tmp3=Cyc_Evexp_promote_const(cn);long long _Tmp4;int _Tmp5;switch(_Tmp3.Float_c.tag){case 5: _Tmp5=_Tmp3.Int_c.val.f2;{int b=_Tmp5;
return({struct _tuple13 _Tmp6;_Tmp6.f1=b!=0,_Tmp6.f2=1;_Tmp6;});}case 6: _Tmp4=_Tmp3.LongLong_c.val.f2;{long long b=_Tmp4;
return({struct _tuple13 _Tmp6;_Tmp6.f1=b!=0,_Tmp6.f2=1;_Tmp6;});}case 1:
 return({struct _tuple13 _Tmp6;_Tmp6.f1=0,_Tmp6.f2=0;_Tmp6;});case 7:
 return({struct _tuple13 _Tmp6;_Tmp6.f1=0,_Tmp6.f2=1;_Tmp6;});default:
 return({struct _tuple13 _Tmp6;_Tmp6.f1=0,_Tmp6.f2=0;_Tmp6;});};}}}struct _tuple14{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 75
static struct _tuple12 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_exp(e);int _Tmp1;union Cyc_Absyn_Cnst _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_Absyn_Cnst cn=_Tmp2;int known=_Tmp1;
if(!known)
return({struct _tuple12 _Tmp3;_Tmp3.f1=cn,_Tmp3.f2=0;_Tmp3;});
{struct _tuple14 _Tmp3=({struct _tuple14 _Tmp4;_Tmp4.f1=p,_Tmp4.f2=cn;_Tmp4;});long long _Tmp4;char _Tmp5;short _Tmp6;int _Tmp7;enum Cyc_Absyn_Sign _Tmp8;switch((int)_Tmp3.f1){case Cyc_Absyn_Plus:
 goto _LL3;case Cyc_Absyn_Minus: switch(_Tmp3.f2.LongLong_c.tag){case 5: _Tmp8=_Tmp3.f2.Int_c.val.f1;_Tmp7=_Tmp3.f2.Int_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp8;int i=_Tmp7;
cn=Cyc_Absyn_Int_c(0U,- i);goto _LL3;}case 4: _Tmp8=_Tmp3.f2.Short_c.val.f1;_Tmp6=_Tmp3.f2.Short_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp8;short i=_Tmp6;
cn=Cyc_Absyn_Short_c(0U,- i);goto _LL3;}case 2: _Tmp8=_Tmp3.f2.Char_c.val.f1;_Tmp5=_Tmp3.f2.Char_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp8;char i=_Tmp5;
cn=Cyc_Absyn_Char_c(0U,- i);goto _LL3;}case 6: _Tmp8=_Tmp3.f2.LongLong_c.val.f1;_Tmp4=_Tmp3.f2.LongLong_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp8;long long i=_Tmp4;
cn=Cyc_Absyn_LongLong_c(0U,- i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Bitnot: switch(_Tmp3.f2.LongLong_c.tag){case 5: _Tmp8=_Tmp3.f2.Int_c.val.f1;_Tmp7=_Tmp3.f2.Int_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp8;int i=_Tmp7;
cn=Cyc_Absyn_Int_c(1U,~ i);goto _LL3;}case 4: _Tmp8=_Tmp3.f2.Short_c.val.f1;_Tmp6=_Tmp3.f2.Short_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp8;short i=_Tmp6;
cn=Cyc_Absyn_Short_c(1U,~ i);goto _LL3;}case 2: _Tmp8=_Tmp3.f2.Char_c.val.f1;_Tmp5=_Tmp3.f2.Char_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp8;char i=_Tmp5;
cn=Cyc_Absyn_Char_c(1U,~ i);goto _LL3;}case 6: _Tmp8=_Tmp3.f2.LongLong_c.val.f1;_Tmp4=_Tmp3.f2.LongLong_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp8;long long i=_Tmp4;
cn=Cyc_Absyn_LongLong_c(1U,~ i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Not: switch(_Tmp3.f2.LongLong_c.tag){case 5: _Tmp7=_Tmp3.f2.Int_c.val.f2;{int i=_Tmp7;
cn=Cyc_Absyn_Int_c(0U,i==0?1: 0);goto _LL3;}case 4: _Tmp6=_Tmp3.f2.Short_c.val.f2;{short i=_Tmp6;
cn=Cyc_Absyn_Short_c(0U,(int)i==0?1: 0);goto _LL3;}case 2: _Tmp5=_Tmp3.f2.Char_c.val.f2;{char i=_Tmp5;
cn=Cyc_Absyn_Char_c(0U,(int)i==0?'\001':'\000');goto _LL3;}case 1:
 cn=Cyc_Absyn_Int_c(0U,1);goto _LL3;case 6: _Tmp4=_Tmp3.f2.LongLong_c.val.f2;{long long i=_Tmp4;
# 94
cn=Cyc_Absyn_LongLong_c(0U,(long long)(i==0?1: 0));goto _LL3;}default: goto _LL20;}default: _LL20:
 return({struct _tuple12 _Tmp9;_Tmp9.f1=cn,_Tmp9.f2=0;_Tmp9;});}_LL3:;}
# 97
return({struct _tuple12 _Tmp3;_Tmp3.f1=cn,_Tmp3.f2=1;_Tmp3;});}}struct _tuple15{enum Cyc_Absyn_Primop f1;int f2;};
# 101
static struct _tuple12 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_exp(e1);int _Tmp1;union Cyc_Absyn_Cnst _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_Absyn_Cnst cn1=_Tmp2;int known1=_Tmp1;
struct _tuple12 _Tmp3=Cyc_Evexp_eval_const_exp(e2);int _Tmp4;union Cyc_Absyn_Cnst _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{union Cyc_Absyn_Cnst cn2=_Tmp5;int known2=_Tmp4;
if(!known1 || !known2)
return({struct _tuple12 _Tmp6;_Tmp6.f1=cn1,_Tmp6.f2=0;_Tmp6;});
cn1=Cyc_Evexp_promote_const(cn1);
cn2=Cyc_Evexp_promote_const(cn2);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{int _Tmp6;enum Cyc_Absyn_Sign _Tmp7;if(cn1.Int_c.tag==5){_Tmp7=cn1.Int_c.val.f1;_Tmp6=cn1.Int_c.val.f2;{enum Cyc_Absyn_Sign x=_Tmp7;int y=_Tmp6;
s1=x;i1=y;goto _LL6;}}else{
return({struct _tuple12 _Tmp8;_Tmp8.f1=cn1,_Tmp8.f2=0;_Tmp8;});}_LL6:;}
# 114
{int _Tmp6;enum Cyc_Absyn_Sign _Tmp7;if(cn2.Int_c.tag==5){_Tmp7=cn2.Int_c.val.f1;_Tmp6=cn2.Int_c.val.f2;{enum Cyc_Absyn_Sign x=_Tmp7;int y=_Tmp6;
s2=x;i2=y;goto _LLB;}}else{
return({struct _tuple12 _Tmp8;_Tmp8.f1=cn1,_Tmp8.f2=0;_Tmp8;});}_LLB:;}
# 118
switch((int)p){case Cyc_Absyn_Div:
 goto _LL14;case Cyc_Absyn_Mod: _LL14:
# 121
 if(i2!=0)
goto _LL10;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="division by zero in constant expression";_tag_fat(_Tmp8,sizeof(char),40U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(e2->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
return({struct _tuple12 _Tmp6;_Tmp6.f1=cn1,_Tmp6.f2=1;_Tmp6;});default:
 goto _LL10;}_LL10:;{
# 127
int has_u_arg=(int)s1==1 ||(int)s2==1;
unsigned u1=(unsigned)i1;
unsigned u2=(unsigned)i2;
int i3=0;
unsigned u3=0U;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple15 _Tmp6=({struct _tuple15 _Tmp7;_Tmp7.f1=p,_Tmp7.f2=has_u_arg;_Tmp7;});switch((int)_Tmp6.f1){case Cyc_Absyn_Plus: switch((int)_Tmp6.f2){case 0:
 i3=i1 + i2;use_i3=1;goto _LL17;case 1:
# 142
 u3=u1 + u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Times: switch((int)_Tmp6.f2){case 0:
# 138
 i3=i1 * i2;use_i3=1;goto _LL17;case 1:
# 143
 u3=u1 * u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Minus: switch((int)_Tmp6.f2){case 0:
# 139
 i3=i1 - i2;use_i3=1;goto _LL17;case 1:
# 144
 u3=u1 - u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Div: switch((int)_Tmp6.f2){case 0:
# 140
 i3=i1 / i2;use_i3=1;goto _LL17;case 1:
# 145
 u3=u1 / u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Mod: switch((int)_Tmp6.f2){case 0:
# 141
 i3=i1 % i2;use_i3=1;goto _LL17;case 1:
# 146
 u3=u1 % u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Eq:
 b3=i1==i2;use_b3=1;goto _LL17;case Cyc_Absyn_Neq:
 b3=i1!=i2;use_b3=1;goto _LL17;case Cyc_Absyn_Gt: switch((int)_Tmp6.f2){case 0:
 b3=i1 > i2;use_b3=1;goto _LL17;case 1:
# 153
 b3=u1 > u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lt: switch((int)_Tmp6.f2){case 0:
# 150
 b3=i1 < i2;use_b3=1;goto _LL17;case 1:
# 154
 b3=u1 < u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Gte: switch((int)_Tmp6.f2){case 0:
# 151
 b3=i1 >= i2;use_b3=1;goto _LL17;case 1:
# 155
 b3=u1 >= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lte: switch((int)_Tmp6.f2){case 0:
# 152
 b3=i1 <= i2;use_b3=1;goto _LL17;case 1:
# 156
 b3=u1 <= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Bitand:
 u3=u1 & u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitor:
 u3=u1 | u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitxor:
 u3=u1 ^ u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlshift:
 u3=u1 << u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlrshift:
 u3=u1 >> u2;use_u3=1;goto _LL17;default: _LL4A:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="bad constant expression";_tag_fat(_Tmp9,sizeof(char),24U);});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp8,sizeof(void*),1));});return({struct _tuple12 _Tmp7;_Tmp7.f1=cn1,_Tmp7.f2=1;_Tmp7;});}_LL17:;}
# 164
if(use_i3)return({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(0U,i3);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=1;_Tmp6;});
if(use_u3)return({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(1U,(int)u3);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=1;_Tmp6;});
if(use_b3)return({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(0U,b3?1: 0);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=1;_Tmp6;});
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="Evexp::eval_const_binop";_tag_fat(_Tmp8,sizeof(char),24U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp7,sizeof(void*),1));});}}}}}struct _tuple16{void*f1;union Cyc_Absyn_Cnst f2;};
# 172
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 ans;
{void*_Tmp0=e->r;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;union Cyc_Absyn_Cnst _Tmp5;switch(*((int*)_Tmp0)){case 1:
# 176
 return({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(0U,0);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=0;_Tmp6;});case 0: _Tmp5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_Tmp5;
return({struct _tuple12 _Tmp6;_Tmp6.f1=c,_Tmp6.f2=1;_Tmp6;});}case 2:
 return({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(0U,0);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=1;_Tmp6;});case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 180
struct _tuple13 _Tmp6=Cyc_Evexp_eval_const_bool_exp(e1);int _Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int bool1=_Tmp8;int known1=_Tmp7;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
Cyc_Evexp_eval_const_exp(e3);
return({struct _tuple12 _Tmp9;({union Cyc_Absyn_Cnst _TmpA=Cyc_Absyn_Int_c(0U,0);_Tmp9.f1=_TmpA;}),_Tmp9.f2=0;_Tmp9;});}
# 186
ans=bool1?Cyc_Evexp_eval_const_exp(e2): Cyc_Evexp_eval_const_exp(e3);
goto _LL0;}}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 189
struct _tuple13 _Tmp6=Cyc_Evexp_eval_const_bool_exp(e1);int _Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int bool1=_Tmp8;int known1=_Tmp7;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple12 _Tmp9;({union Cyc_Absyn_Cnst _TmpA=Cyc_Absyn_Int_c(0U,0);_Tmp9.f1=_TmpA;}),_Tmp9.f2=0;_Tmp9;});}
# 194
ans=bool1?Cyc_Evexp_eval_const_exp(e2):({struct _tuple12 _Tmp9;({union Cyc_Absyn_Cnst _TmpA=Cyc_Absyn_Int_c(0U,0);_Tmp9.f1=_TmpA;}),_Tmp9.f2=1;_Tmp9;});
goto _LL0;}}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 197
struct _tuple13 _Tmp6=Cyc_Evexp_eval_const_bool_exp(e1);int _Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int bool1=_Tmp8;int known1=_Tmp7;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple12 _Tmp9;({union Cyc_Absyn_Cnst _TmpA=Cyc_Absyn_Int_c(0U,0);_Tmp9.f1=_TmpA;}),_Tmp9.f2=0;_Tmp9;});}
# 202
ans=bool1?({struct _tuple12 _Tmp9;({union Cyc_Absyn_Cnst _TmpA=Cyc_Absyn_Int_c(0U,1);_Tmp9.f1=_TmpA;}),_Tmp9.f2=1;_Tmp9;}): Cyc_Evexp_eval_const_exp(e2);
goto _LL0;}}case 3: _Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_List_List*es=_Tmp4;
# 205
if(es==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="bad static expression (no args to primop)";_tag_fat(_Tmp8,sizeof(char),42U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
return({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(0U,0);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=1;_Tmp6;});}else{
if(es->tl==0)
ans=Cyc_Evexp_eval_const_unprimop(p,(struct Cyc_Absyn_Exp*)es->hd);else{
if(_check_null(es->tl)->tl==0)
ans=Cyc_Evexp_eval_const_binprimop(p,(struct Cyc_Absyn_Exp*)es->hd,(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd);else{
# 213
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="bad static expression (too many args to primop)";_tag_fat(_Tmp8,sizeof(char),48U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
return({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(0U,0);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=1;_Tmp6;});}}}
# 216
goto _LL0;}case 39: _Tmp4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp4;
# 219
{void*_Tmp6=Cyc_Absyn_compress(t);void*_Tmp7;if(*((int*)_Tmp6)==9){_Tmp7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp6)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp7;
# 221
e->r=e2->r;
return Cyc_Evexp_eval_const_exp(e2);}}else{
goto _LL28;}_LL28:;}
# 225
goto _LL12;}case 17: _LL12:
 goto _LL14;case 18: _LL14:
 goto _LL16;case 19: _LL16:
 ans=({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(0U,0);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=0;_Tmp6;});goto _LL0;case 14: _Tmp4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 231
ans=Cyc_Evexp_eval_const_exp(e2);
if(ans.f2){
struct _tuple16 _Tmp6=({struct _tuple16 _Tmp7;({void*_Tmp8=Cyc_Absyn_compress(t);_Tmp7.f1=_Tmp8;}),_Tmp7.f2=ans.f1;_Tmp7;});void*_Tmp7;int _Tmp8;short _Tmp9;char _TmpA;enum Cyc_Absyn_Sign _TmpB;enum Cyc_Absyn_Size_of _TmpC;enum Cyc_Absyn_Sign _TmpD;if(*((int*)_Tmp6.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f1)){case 1: switch(_Tmp6.f2.Int_c.tag){case 2: _TmpD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f1)->f1;_TmpC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f1)->f2;_TmpB=_Tmp6.f2.Char_c.val.f1;_TmpA=_Tmp6.f2.Char_c.val.f2;{enum Cyc_Absyn_Sign sn=_TmpD;enum Cyc_Absyn_Size_of sz=_TmpC;enum Cyc_Absyn_Sign sn2=_TmpB;char x=_TmpA;
# 236
_TmpD=sn;_TmpC=sz;_TmpB=sn2;_Tmp9=(short)x;goto _LL31;}case 4: _TmpD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f1)->f1;_TmpC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f1)->f2;_TmpB=_Tmp6.f2.Short_c.val.f1;_Tmp9=_Tmp6.f2.Short_c.val.f2;_LL31: {enum Cyc_Absyn_Sign sn=_TmpD;enum Cyc_Absyn_Size_of sz=_TmpC;enum Cyc_Absyn_Sign sn2=_TmpB;short x=_Tmp9;
# 238
_TmpD=sn;_TmpC=sz;_TmpB=sn2;_Tmp8=(int)x;goto _LL33;}case 5: _TmpD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f1)->f1;_TmpC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f1)->f2;_TmpB=_Tmp6.f2.Int_c.val.f1;_Tmp8=_Tmp6.f2.Int_c.val.f2;_LL33: {enum Cyc_Absyn_Sign sn=_TmpD;enum Cyc_Absyn_Size_of sz=_TmpC;enum Cyc_Absyn_Sign sn2=_TmpB;int x=_Tmp8;
# 240
if((int)sn!=(int)sn2)
({union Cyc_Absyn_Cnst _TmpE=Cyc_Absyn_Int_c(sn,x);ans.f1=_TmpE;});
goto _LL2D;}default: goto _LL3A;}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f2!=0)switch(_Tmp6.f2.Int_c.tag){case 2: _Tmp7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f2->hd;_TmpD=_Tmp6.f2.Char_c.val.f1;_TmpA=_Tmp6.f2.Char_c.val.f2;{void*it=_Tmp7;enum Cyc_Absyn_Sign sn2=_TmpD;char x=_TmpA;
# 245
_Tmp7=it;_TmpD=sn2;_Tmp9=(short)x;goto _LL37;}case 4: _Tmp7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f2->hd;_TmpD=_Tmp6.f2.Short_c.val.f1;_Tmp9=_Tmp6.f2.Short_c.val.f2;_LL37: {void*it=_Tmp7;enum Cyc_Absyn_Sign sn2=_TmpD;short x=_Tmp9;
# 247
_Tmp7=it;_TmpD=sn2;_Tmp8=(int)x;goto _LL39;}case 5: _Tmp7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6.f1)->f2->hd;_TmpD=_Tmp6.f2.Int_c.val.f1;_Tmp8=_Tmp6.f2.Int_c.val.f2;_LL39: {void*it=_Tmp7;enum Cyc_Absyn_Sign sn2=_TmpD;int x=_Tmp8;
# 249
if(x < 0)
return({struct _tuple12 _TmpE;({union Cyc_Absyn_Cnst _TmpF=Cyc_Absyn_Int_c(0U,0);_TmpE.f1=_TmpF;}),_TmpE.f2=0;_TmpE;});
({void*_TmpE=it;Cyc_Unify_unify(_TmpE,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_TmpF->tag=9,({struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_const_exp(ans.f1,0U);_TmpF->f1=_Tmp10;});_TmpF;}));});
({union Cyc_Absyn_Cnst _TmpE=Cyc_Absyn_Int_c(1U,x);ans.f1=_TmpE;});
goto _LL2D;}default: goto _LL3A;}else{goto _LL3A;}default: goto _LL3A;}else{_LL3A:
 return({struct _tuple12 _TmpE;({union Cyc_Absyn_Cnst _TmpF=Cyc_Absyn_Int_c(0U,0);_TmpE.f1=_TmpF;}),_TmpE.f2=0;_TmpE;});}_LL2D:;}
# 256
goto _LL0;}case 33: _Tmp4=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp4;
_Tmp4=ef;goto _LL1C;}case 32: _Tmp4=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_Absyn_Enumfield*ef=_Tmp4;
ans=Cyc_Evexp_eval_const_exp(_check_null(ef->tag));goto _LL0;}default:
 return({struct _tuple12 _Tmp6;({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(0U,0);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=0;_Tmp6;});}_LL0:;}
# 261
if(ans.f2){
void*c;
{union Cyc_Absyn_Cnst c=ans.f1;
({void*_Tmp0=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct));_Tmp1->tag=0,_Tmp1->f1=c;_Tmp1;});e->r=_Tmp0;});}}
# 266
return ans;}
# 269
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 33:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 17: _LL6:
 goto _LL8;case 18: _LL8:
 goto _LLA;case 19: _LLA:
 goto _LLC;case 0: _LLC:
 return 1;case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 278
return(Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2))&& Cyc_Evexp_c_can_eval(e3);}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp4=e1;_Tmp3=e2;goto _LL12;}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
return Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2);}case 3: _Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_List_List*es=_Tmp4;
# 282
for(1;es!=0;es=es->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}case 39:
 return 0;case 14: _Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;
return Cyc_Evexp_c_can_eval(e);}default:
 return 0;};}
# 293
static struct Cyc_Absyn_Exp*Cyc_Evexp_strip_cast(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==14){_Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 296
if(Cyc_Unify_unify(t,Cyc_Absyn_uint_type)){
void*_Tmp3=e2->r;if(*((int*)_Tmp3)==39)
return e2;else{
goto _LL5;}_LL5:;}
# 301
return e;}}else{
return e;};}
# 306
static unsigned long long Cyc_Evexp_integral_const_to_long_long(union Cyc_Absyn_Cnst c){
unsigned long long res;
{long long _Tmp0;int _Tmp1;short _Tmp2;char _Tmp3;switch(c.LongLong_c.tag){case 2: _Tmp3=c.Char_c.val.f2;{char c=_Tmp3;
res=(unsigned long long)c;goto _LL0;}case 4: _Tmp2=c.Short_c.val.f2;{short s=_Tmp2;
res=(unsigned long long)s;goto _LL0;}case 5: _Tmp1=c.Int_c.val.f2;{int i=_Tmp1;
res=(unsigned long long)i;goto _LL0;}case 6: _Tmp0=c.LongLong_c.val.f2;{long long x=_Tmp0;
res=(unsigned long long)x;goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="Evexp::integral_const_to_long_long";_tag_fat(_Tmp6,sizeof(char),35U);});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});}_LL0:;}
# 315
return res;}struct _tuple17{union Cyc_Absyn_Cnst f1;union Cyc_Absyn_Cnst f2;};
# 318
static int Cyc_Evexp_cmp_cnst(union Cyc_Absyn_Cnst c1,union Cyc_Absyn_Cnst c2){
struct _tuple17 _Tmp0=({struct _tuple17 _Tmp1;_Tmp1.f1=c1,_Tmp1.f2=c2;_Tmp1;});int _Tmp1;int _Tmp2;struct _fat_ptr _Tmp3;struct _fat_ptr _Tmp4;if(_Tmp0.f1.Null_c.tag==1){if(_Tmp0.f2.Null_c.tag==1)
return 0;else{
return -1;}}else{if(_Tmp0.f2.Null_c.tag==1)
return 1;else{if(_Tmp0.f1.Wchar_c.tag==3){if(_Tmp0.f2.Wchar_c.tag==3){_Tmp4=_Tmp0.f1.Wchar_c.val;_Tmp3=_Tmp0.f2.Wchar_c.val;{struct _fat_ptr s1=_Tmp4;struct _fat_ptr s2=_Tmp3;
return Cyc_strcmp(s1,s2);}}else{
return -1;}}else{if(_Tmp0.f2.Wchar_c.tag==3)
return 1;else{if(_Tmp0.f1.String_c.tag==8){if(_Tmp0.f2.String_c.tag==8){_Tmp4=_Tmp0.f1.String_c.val;_Tmp3=_Tmp0.f2.String_c.val;{struct _fat_ptr s1=_Tmp4;struct _fat_ptr s2=_Tmp3;
return Cyc_strcmp(s1,s2);}}else{
return -1;}}else{if(_Tmp0.f2.String_c.tag==8)
return 1;else{if(_Tmp0.f1.Wstring_c.tag==9){if(_Tmp0.f2.Wstring_c.tag==9){_Tmp4=_Tmp0.f1.Wstring_c.val;_Tmp3=_Tmp0.f2.Wstring_c.val;{struct _fat_ptr s1=_Tmp4;struct _fat_ptr s2=_Tmp3;
return Cyc_strcmp(s1,s2);}}else{_Tmp4=_Tmp0.f1.Wstring_c.val;{struct _fat_ptr s1=_Tmp4;
return -1;}}}else{if(_Tmp0.f2.Wstring_c.tag==9){_Tmp4=_Tmp0.f2.Wstring_c.val;{struct _fat_ptr s1=_Tmp4;
return 1;}}else{if(_Tmp0.f1.Float_c.tag==7){if(_Tmp0.f2.Float_c.tag==7){_Tmp4=_Tmp0.f1.Float_c.val.f1;_Tmp2=_Tmp0.f1.Float_c.val.f2;_Tmp3=_Tmp0.f2.Float_c.val.f1;_Tmp1=_Tmp0.f2.Float_c.val.f2;{struct _fat_ptr s1=_Tmp4;int i1=_Tmp2;struct _fat_ptr s2=_Tmp3;int i2=_Tmp1;
# 333
if(i1!=i2)return i1 - i2;
return Cyc_strcmp(s1,s2);}}else{
return -1;}}else{if(_Tmp0.f2.Float_c.tag==7)
return 1;else{
# 338
unsigned long long i1=Cyc_Evexp_integral_const_to_long_long(c1);
unsigned long long i2=Cyc_Evexp_integral_const_to_long_long(c2);
long long diff=(long long)(i1 - i2);
return diff==0?0:(diff < 0?-1: 1);}}}}}}}}}};}struct _tuple18{void*f1;void*f2;};
# 351
static int Cyc_Evexp_designator_cmp(void*d1,void*d2){
struct _tuple18 _Tmp0=({struct _tuple18 _Tmp1;_Tmp1.f1=d1,_Tmp1.f2=d2;_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp2=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{_Tmp2=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
return -1;}}}else{if(*((int*)_Tmp0.f2)==0){_Tmp2=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
return 1;}}else{_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp0.f2)->f1;{struct _fat_ptr*v1=_Tmp2;struct _fat_ptr*v2=_Tmp1;
return Cyc_strcmp(*v1,*v2);}}};}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 360
static int Cyc_Evexp_designator_exp_cmp(struct _tuple19*d1,struct _tuple19*d2){
# 362
struct _tuple19 _Tmp0=*d1;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*des1=_Tmp2;struct Cyc_Absyn_Exp*e1=_Tmp1;
struct _tuple19 _Tmp3=*d2;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{struct Cyc_List_List*des2=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
int c1=Cyc_Evexp_exp_cmp(e1,e2);
if(c1!=0)return c1;
return Cyc_List_list_cmp(Cyc_Evexp_designator_cmp,des1,des2);}}}
# 369
static struct Cyc_Absyn_Vardecl*Cyc_Evexp_binding2vardecl(void*b){
void*_Tmp0;switch(*((int*)b)){case 0:
 return 0;case 1: _Tmp0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}case 2: _Tmp0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp0;
return fd->fn_vardecl;}case 3: _Tmp0=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}case 4: _Tmp0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}default: _Tmp0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}};}struct _tuple20{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};
# 380
static int Cyc_Evexp_cmp_binding(void*b1,void*b2){
struct Cyc_Absyn_Vardecl*vdopt1=Cyc_Evexp_binding2vardecl(b1);
struct Cyc_Absyn_Vardecl*vdopt2=Cyc_Evexp_binding2vardecl(b2);
struct _tuple20 _Tmp0=({struct _tuple20 _Tmp1;_Tmp1.f1=vdopt1,_Tmp1.f2=vdopt2;_Tmp1;});if(_Tmp0.f1==0){if(_Tmp0.f2==0)
return({struct _tuple0*_Tmp1=Cyc_Absyn_binding2qvar(b1);Cyc_Absyn_qvar_cmp(_Tmp1,Cyc_Absyn_binding2qvar(b2));});else{
return -1;}}else{if(_Tmp0.f2==0)
return 1;else{
return(int)vdopt1 - (int)vdopt2;}};}
# 393
static unsigned Cyc_Evexp_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;switch(*((int*)_Tmp0)){case 0:
 return 0U;case 1:
 return 1U;case 2:
 return 2U;case 3:
 return 3U;case 4:
 return 4U;case 5:
 return 5U;case 6:
 return 6U;case 7:
 return 7U;case 8:
 return 8U;case 9:
 return 9U;case 10:
 return 10U;case 11:
 return 11U;case 12:
 return 12U;case 13:
 return 13U;case 14:
 return 14U;case 15:
 return 15U;case 16:
 return 16U;case 17:
 return 17U;case 18:
 return 18U;case 19:
 return 19U;case 20:
 return 20U;case 21:
 return 21U;case 22:
 return 22U;case 23:
 return 23U;case 24:
 return 24U;case 25:
 return 25U;case 26:
 return 26U;case 27:
 return 27U;case 28:
 return 28U;case 29:
 return 29U;case 30:
 return 30U;case 31:
 return 31U;case 32:
 return 32U;case 33:
 return 33U;case 34:
 return 34U;case 35:
 return 35U;case 36:
 return 36U;case 37:
 return 37U;case 38:
 return 38U;case 39:
 return 39U;case 40:
 return 40U;case 41:
 return 41U;default:
 return 42U;};}
# 443
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_exp(e1);int _Tmp1;union Cyc_Absyn_Cnst _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_Absyn_Cnst c1=_Tmp2;int known1=_Tmp1;
struct _tuple12 _Tmp3=Cyc_Evexp_eval_const_exp(e2);int _Tmp4;union Cyc_Absyn_Cnst _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{union Cyc_Absyn_Cnst c2=_Tmp5;int known2=_Tmp4;
if(known1 && known2)return Cyc_Evexp_cmp_cnst(c1,c2);{
unsigned e1case=Cyc_Evexp_exp_case_number(e1);
unsigned e2case=Cyc_Evexp_exp_case_number(e2);
if(e1case!=e2case)
return(int)(e1case - e2case);{
struct _tuple18 _Tmp6=({struct _tuple18 _Tmp7;_Tmp7.f1=e1->r,_Tmp7.f2=e2->r;_Tmp7;});int _Tmp7;int _Tmp8;enum Cyc_Absyn_Primop _Tmp9;enum Cyc_Absyn_Primop _TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;struct _fat_ptr _TmpF;struct _fat_ptr _Tmp10;void*_Tmp11;void*_Tmp12;union Cyc_Absyn_Cnst _Tmp13;union Cyc_Absyn_Cnst _Tmp14;switch(*((int*)_Tmp6.f1)){case 0: if(*((int*)_Tmp6.f2)==0){_Tmp14=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp13=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{union Cyc_Absyn_Cnst c1=_Tmp14;union Cyc_Absyn_Cnst c2=_Tmp13;
# 453
return Cyc_Evexp_cmp_cnst(c1,c2);}}else{goto _LL3F;}case 1: if(*((int*)_Tmp6.f2)==1){_Tmp12=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{void*b1=_Tmp12;void*b2=_Tmp11;
return Cyc_Evexp_cmp_binding(b1,b2);}}else{goto _LL3F;}case 2: if(*((int*)_Tmp6.f2)==2){_Tmp10=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_TmpF=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{struct _fat_ptr s1=_Tmp10;struct _fat_ptr s2=_TmpF;
return Cyc_strcmp(s1,s2);}}else{goto _LL3F;}case 6: if(*((int*)_Tmp6.f2)==6){_Tmp12=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f3;_TmpD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;_TmpB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f3;{struct Cyc_Absyn_Exp*e11=_Tmp12;struct Cyc_Absyn_Exp*e12=_Tmp11;struct Cyc_Absyn_Exp*e13=_TmpE;struct Cyc_Absyn_Exp*e21=_TmpD;struct Cyc_Absyn_Exp*e22=_TmpC;struct Cyc_Absyn_Exp*e23=_TmpB;
# 459
int c1=Cyc_Evexp_exp_cmp(e11,e21);
if(c1!=0)return c1;
c1=Cyc_Evexp_exp_cmp(e12,e22);
if(c1!=0)return c1;
return Cyc_Evexp_exp_cmp(e13,e23);}}else{goto _LL3F;}case 27: if(*((int*)_Tmp6.f2)==27){_Tmp12=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f3;_TmpD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;_TmpB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f3;{struct Cyc_Absyn_Vardecl*vd1=_Tmp12;struct Cyc_Absyn_Exp*e11=_Tmp11;struct Cyc_Absyn_Exp*e12=_TmpE;struct Cyc_Absyn_Vardecl*vd2=_TmpD;struct Cyc_Absyn_Exp*e21=_TmpC;struct Cyc_Absyn_Exp*e22=_TmpB;
# 465
if(vd1!=vd2)return(int)vd1 - (int)vd2;
_Tmp12=e11;_Tmp11=e12;_TmpE=e21;_TmpD=e22;goto _LL12;}}else{goto _LL3F;}case 7: if(*((int*)_Tmp6.f2)==7){_Tmp12=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;_LL12: {struct Cyc_Absyn_Exp*e11=_Tmp12;struct Cyc_Absyn_Exp*e12=_Tmp11;struct Cyc_Absyn_Exp*e21=_TmpE;struct Cyc_Absyn_Exp*e22=_TmpD;
_Tmp12=e11;_Tmp11=e12;_TmpE=e21;_TmpD=e22;goto _LL14;}}else{goto _LL3F;}case 8: if(*((int*)_Tmp6.f2)==8){_Tmp12=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;_LL14: {struct Cyc_Absyn_Exp*e11=_Tmp12;struct Cyc_Absyn_Exp*e12=_Tmp11;struct Cyc_Absyn_Exp*e21=_TmpE;struct Cyc_Absyn_Exp*e22=_TmpD;
_Tmp12=e11;_Tmp11=e12;_TmpE=e21;_TmpD=e22;goto _LL16;}}else{goto _LL3F;}case 9: if(*((int*)_Tmp6.f2)==9){_Tmp12=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;_LL16: {struct Cyc_Absyn_Exp*e11=_Tmp12;struct Cyc_Absyn_Exp*e12=_Tmp11;struct Cyc_Absyn_Exp*e21=_TmpE;struct Cyc_Absyn_Exp*e22=_TmpD;
# 470
int c1=Cyc_Evexp_exp_cmp(e11,e21);
if(c1!=0)return c1;
return Cyc_Evexp_exp_cmp(e12,e22);}}else{goto _LL3F;}case 3: if(*((int*)_Tmp6.f2)==3){_TmpA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp12=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_Tmp9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_Tmp11=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;{enum Cyc_Absyn_Primop p1=_TmpA;struct Cyc_List_List*es1=_Tmp12;enum Cyc_Absyn_Primop p2=_Tmp9;struct Cyc_List_List*es2=_Tmp11;
# 474
int c1=(int)p1 - (int)p2;
if(c1!=0)return c1;
for(1;es1!=0 && es2!=0;(es1=es1->tl,es2=es2->tl)){
int c2=Cyc_Evexp_exp_cmp((struct Cyc_Absyn_Exp*)es1->hd,(struct Cyc_Absyn_Exp*)es2->hd);
if(c2!=0)
return c2;}
# 481
return 0;}}else{goto _LL3F;}case 17: switch(*((int*)_Tmp6.f2)){case 17: _Tmp12=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{void*t1=_Tmp12;void*t2=_Tmp11;
# 483
return Cyc_Tcutil_typecmp(t1,t2);}case 18: _Tmp12=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{void*t1=_Tmp12;struct Cyc_Absyn_Exp*e2a=_Tmp11;
# 485
void*e2atopt=e2a->topt;
if(e2atopt==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=({const char*_Tmp17="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_Tmp17,sizeof(char),51U);});_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(e2->loc,_tag_fat(_Tmp16,sizeof(void*),1));});
return 0;}
# 490
return Cyc_Tcutil_typecmp(t1,e2atopt);}default: goto _LL3F;}case 18: switch(*((int*)_Tmp6.f2)){case 17: _Tmp12=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{struct Cyc_Absyn_Exp*e1a=_Tmp12;void*t2=_Tmp11;
# 492
void*e1atopt=e1a->topt;
if(e1atopt==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=({const char*_Tmp17="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_Tmp17,sizeof(char),51U);});_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp16,sizeof(void*),1));});
return 0;}
# 497
return Cyc_Tcutil_typecmp(e1atopt,t2);}case 18: _Tmp12=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{struct Cyc_Absyn_Exp*e1a=_Tmp12;struct Cyc_Absyn_Exp*e2a=_Tmp11;
# 499
void*e1atopt=e1a->topt;
void*e2atopt=e2a->topt;
if(e1atopt==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=({const char*_Tmp17="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_Tmp17,sizeof(char),51U);});_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp16,sizeof(void*),1));});
return 0;}else{
if(e2atopt==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=({const char*_Tmp17="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_Tmp17,sizeof(char),51U);});_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(e2->loc,_tag_fat(_Tmp16,sizeof(void*),1));});
return 0;}}
# 508
return Cyc_Tcutil_typecmp(e1atopt,e2atopt);}default: goto _LL3F;}case 19: if(*((int*)_Tmp6.f2)==19){_Tmp12=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpD=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;{void*t1=_Tmp12;struct Cyc_List_List*f1=_Tmp11;void*t2=_TmpE;struct Cyc_List_List*f2=_TmpD;
# 510
int c1=Cyc_Tcutil_typecmp(t1,t2);
if(c1!=0)return c1;{
int l1=Cyc_List_length(f1);
int l2=Cyc_List_length(f2);
if(l1 < l2)return -1;
if(l2 < l1)return 1;
for(1;f1!=0 && f2!=0;(f1=f1->tl,f2=f2->tl)){
struct _tuple18 _Tmp15=({struct _tuple18 _Tmp16;_Tmp16.f1=(void*)f1->hd,_Tmp16.f2=(void*)f2->hd;_Tmp16;});unsigned _Tmp16;unsigned _Tmp17;void*_Tmp18;void*_Tmp19;if(*((int*)_Tmp15.f1)==0){if(*((int*)_Tmp15.f2)==1)
return -1;else{_Tmp19=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_Tmp15.f1)->f1;_Tmp18=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_Tmp15.f2)->f1;{struct _fat_ptr*fn1=_Tmp19;struct _fat_ptr*fn2=_Tmp18;
# 521
int c=Cyc_strptrcmp(fn1,fn2);
if(c!=0)return c;
goto _LL41;}}}else{if(*((int*)_Tmp15.f2)==0)
# 519
return 1;else{_Tmp17=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_Tmp15.f1)->f1;_Tmp16=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_Tmp15.f2)->f1;{unsigned i1=_Tmp17;unsigned i2=_Tmp16;
# 525
int c=(int)(i1 - i2);
if(c!=0)return c;
goto _LL41;}}}_LL41:;}
# 529
return 0;}}}else{goto _LL3F;}case 14: if(*((int*)_Tmp6.f2)==14){_Tmp12=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;{void*t1=_Tmp12;struct Cyc_Absyn_Exp*e1a=_Tmp11;void*t2=_TmpE;struct Cyc_Absyn_Exp*e2a=_TmpD;
# 531
int c1=Cyc_Tcutil_typecmp(t1,t2);
if(c1!=0)return c1;
return Cyc_Evexp_exp_cmp(e1a,e2a);}}else{goto _LL3F;}case 39: if(*((int*)_Tmp6.f2)==39){_Tmp12=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{void*t1=_Tmp12;void*t2=_Tmp11;
# 535
if(Cyc_Unify_unify(t1,t2))return 0;
return Cyc_Tcutil_typecmp(t1,t2);}}else{goto _LL3F;}case 41: if(*((int*)_Tmp6.f2)==41){_Tmp12=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp12;struct Cyc_Absyn_Exp*e2=_Tmp11;
_Tmp12=e1;_Tmp11=e2;goto _LL2A;}}else{goto _LL3F;}case 12: if(*((int*)_Tmp6.f2)==12){_Tmp12=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_Tmp12;struct Cyc_Absyn_Exp*e2=_Tmp11;
# 539
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 13: if(*((int*)_Tmp6.f2)==13){_Tmp12=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp12;struct Cyc_List_List*t1=_Tmp11;struct Cyc_Absyn_Exp*e2=_TmpE;struct Cyc_List_List*t2=_TmpD;
# 541
int c1=Cyc_Evexp_exp_cmp(e1,e2);
if(c1!=0)return c1;
return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,t1,t2);}}else{goto _LL3F;}case 15: if(*((int*)_Tmp6.f2)==15){_Tmp12=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp12;struct Cyc_Absyn_Exp*e2=_Tmp11;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 28: if(*((int*)_Tmp6.f2)==28){_Tmp12=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_Tmp8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f3;_TmpE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpD=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;_Tmp7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp12;void*t1=_Tmp11;int b1=_Tmp8;struct Cyc_Absyn_Exp*e2=_TmpE;void*t2=_TmpD;int b2=_Tmp7;
# 546
int c1=b1 - b2;
if(c1!=0)return c1;
c1=Cyc_Tcutil_typecmp(t1,t2);
if(c1!=0)return c1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 30: if(*((int*)_Tmp6.f2)==30){_Tmp12=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpD=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;{void*t1=_Tmp12;struct Cyc_List_List*des1=_Tmp11;void*t2=_TmpE;struct Cyc_List_List*des2=_TmpD;
# 552
int c1=Cyc_Tcutil_typecmp(t1,t2);
if(c1!=0)return c1;
_Tmp12=des1;_Tmp11=des2;goto _LL34;}}else{goto _LL3F;}case 26: if(*((int*)_Tmp6.f2)==26){_Tmp12=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_LL34: {struct Cyc_List_List*des1=_Tmp12;struct Cyc_List_List*des2=_Tmp11;
# 556
return({(int(*)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Evexp_designator_exp_cmp,des1,des2);}}else{goto _LL3F;}case 29: if(*((int*)_Tmp6.f2)==29){_Tmp12=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_Tmp11=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f3;_TmpE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f4;_TmpD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;_TmpC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f3;_TmpB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f4;{struct Cyc_List_List*ts1=_Tmp12;struct Cyc_List_List*des1=_Tmp11;struct Cyc_Absyn_Aggrdecl*ad1=_TmpE;struct Cyc_List_List*ts2=_TmpD;struct Cyc_List_List*des2=_TmpC;struct Cyc_Absyn_Aggrdecl*ad2=_TmpB;
# 558
int c1=(int)ad1 - (int)ad2;
if(c1!=0)return c1;
c1=Cyc_List_list_cmp(Cyc_Tcutil_typecmp,ts1,ts2);
if(c1!=0)return c1;
return({(int(*)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Evexp_designator_exp_cmp,des1,des1);}}else{goto _LL3F;}case 24: if(*((int*)_Tmp6.f2)==24){_Tmp12=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;{struct Cyc_List_List*es1=_Tmp12;struct Cyc_List_List*es2=_Tmp11;
# 564
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Evexp_exp_cmp,es1,es2);}}else{goto _LL3F;}case 31: if(*((int*)_Tmp6.f2)==31){_Tmp12=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f1;_Tmp11=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f2;_TmpE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp6.f1)->f3;_TmpD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f1;_TmpC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f2;_TmpB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp6.f2)->f3;{struct Cyc_List_List*es1=_Tmp12;struct Cyc_Absyn_Datatypedecl*dd1=_Tmp11;struct Cyc_Absyn_Datatypefield*df1=_TmpE;struct Cyc_List_List*es2=_TmpD;struct Cyc_Absyn_Datatypedecl*dd2=_TmpC;struct Cyc_Absyn_Datatypefield*df2=_TmpB;
# 566
int c1=(int)dd1 - (int)dd2;
if(c1!=0)return c1;
c1=(int)df1 - (int)df2;
if(c1!=0)return c1;
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Evexp_exp_cmp,es1,es2);}}else{goto _LL3F;}case 32: if(*((int*)_Tmp6.f2)==32)
# 572
goto _LL3E;else{goto _LL3F;}case 33: if(*((int*)_Tmp6.f2)==33){_LL3E:
 goto _LL40;}else{goto _LL3F;}default: _LL3F: _LL40:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=({const char*_Tmp17="Evexp::exp_cmp, unexpected case";_tag_fat(_Tmp17,sizeof(char),32U);});_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp16,sizeof(void*),1));});};}}}}}
# 578
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 580
e1=Cyc_Evexp_strip_cast(e1);
e2=Cyc_Evexp_strip_cast(e2);{
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_exp(e1);int _Tmp1;union Cyc_Absyn_Cnst _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_Absyn_Cnst c1=_Tmp2;int known1=_Tmp1;
struct _tuple12 _Tmp3=Cyc_Evexp_eval_const_exp(e2);int _Tmp4;union Cyc_Absyn_Cnst _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{union Cyc_Absyn_Cnst c2=_Tmp5;int known2=_Tmp4;
if(known1 && known2){
struct _tuple11 _Tmp6=Cyc_Evexp_eval_const_uint_exp(e1);unsigned _Tmp7;_Tmp7=_Tmp6.f1;{unsigned i1=_Tmp7;
struct _tuple11 _Tmp8=Cyc_Evexp_eval_const_uint_exp(e2);unsigned _Tmp9;_Tmp9=_Tmp8.f1;{unsigned i2=_Tmp9;
return(int)(i1 - i2);}}}
# 589
return Cyc_Evexp_exp_cmp(e1,e2);}}}}
# 592
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_uint_exp_cmp(e1,e2)==0;}
# 595
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e1);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned i1=_Tmp2;int known1=_Tmp1;
struct _tuple11 _Tmp3=Cyc_Evexp_eval_const_uint_exp(e2);int _Tmp4;unsigned _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{unsigned i2=_Tmp5;int known2=_Tmp4;
if(known1 && known2)
return i1 <= i2;
return Cyc_Evexp_same_uint_const_exp(e1,e2);}}}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 605
int Cyc_Evexp_okay_szofarg(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp2;struct Cyc_List_List*ts=_Tmp1;
# 608
void*_Tmp3;switch(*((int*)c)){case 19:
 return 0;case 4:
 goto _LL21;case 1: _LL21:
 goto _LL23;case 2: _LL23:
 goto _LL25;case 17: _LL25:
 goto _LL27;case 15: _LL27:
 goto _LL29;case 3: _LL29:
 goto _LL2B;case 18: _LL2B:
 goto _LL2D;case 16: _LL2D:
 return 1;case 0:
# 619
 goto _LL31;case 5: _LL31:
 goto _LL33;case 7: _LL33:
 goto _LL35;case 6: _LL35:
 goto _LL37;case 9: _LL37:
 goto _LL39;case 8: _LL39:
 goto _LL3B;case 11: _LL3B:
 goto _LL3D;case 12: _LL3D:
 goto _LL3F;case 13: _LL3F:
 goto _LL41;case 14: _LL41:
 goto _LL43;case 10: _LL43:
 return 0;default: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.UnknownAggr.tag==1)
# 631
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="szof on unchecked StructType";_tag_fat(_Tmp6,sizeof(char),29U);});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});else{_Tmp3=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp3;
# 633
if(ad->impl==0)return 0;{
struct _RegionHandle _Tmp4=_new_region("temp");struct _RegionHandle*temp=& _Tmp4;_push_region(temp);
{struct Cyc_List_List*inst=Cyc_List_rzip(temp,temp,ad->tvs,ts);
{struct Cyc_List_List*fs=_check_null(ad->impl)->fields;for(0;fs!=0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _Tmp5=0;_npop_handler(0);return _Tmp5;}}}{
int _Tmp5=1;_npop_handler(0);return _Tmp5;}}
# 635
;_pop_region();}}}};}case 2: _Tmp2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp2;
# 641
return(int)Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk)->kind==2;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1!=0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1)->v)->kind==Cyc_Absyn_BoxKind)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}case 6: _Tmp2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqs=_Tmp2;
# 646
for(1;tqs!=0;tqs=tqs->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple21*)tqs->hd)).f2))
return 0;}
return 1;}case 3:
# 652
 return 1;case 4: _Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;{void*t2=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
# 655
return e!=0;}case 5:
 return 0;case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*fs=_Tmp2;
# 658
for(1;fs!=0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}case 10:
 goto _LL16;case 9: _LL16:
 goto _LL18;case 11: _LL18:
 return 0;default: _Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Typedefdecl*td=_Tmp2;
# 667
if(td==0 || td->kind==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="szof typedeftype ";_tag_fat(_Tmp5,sizeof(char),18U);});_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),2));});
return(int)((struct Cyc_Absyn_Kind*)_check_null(td->kind)->v)->kind==2;}};}

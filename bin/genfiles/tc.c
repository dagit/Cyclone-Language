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
 struct Cyc_Core_Opt{void*v;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 854 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 864
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 867
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 874
void*Cyc_Absyn_compress(void*);
# 887
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 903
extern void*Cyc_Absyn_false_type;
# 927
void*Cyc_Absyn_string_type(void*);
void*Cyc_Absyn_const_string_type(void*);
# 948
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 956
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1042
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 43 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);
# 84 "relations-ap.h"
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 46
int Cyc_Tcutil_is_array_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 107
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 156
void Cyc_Tcutil_check_bitfield(unsigned,void*,struct Cyc_Absyn_Exp*,struct _fat_ptr*);
# 159
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 205
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 214
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 231
int Cyc_Tcutil_zeroable_type(void*);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tbk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 62
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 70
void Cyc_Atts_check_fndecl_atts(unsigned,struct Cyc_List_List*,int);
void Cyc_Atts_check_variable_atts(unsigned,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*);
void Cyc_Atts_check_field_atts(unsigned,struct _fat_ptr*,struct Cyc_List_List*);
# 74
void Cyc_Atts_fnTypeAttsOK(unsigned,void*);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 83 "dict.h"
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 70 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned,struct Cyc_Absyn_Fndecl*);
# 79
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple0*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*);
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 153
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv_dicts(struct Cyc_Tcenv_Tenv*);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 54 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*);
# 57
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*);
# 59
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*);
# 66
void*Cyc_Tcdecl_merge_binding(void*,void*,unsigned,struct _fat_ptr*);
# 74
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*,int*,struct _fat_ptr*,unsigned,struct _fat_ptr*);
# 92 "graph.h"
extern struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict);
# 30 "callgraph.h"
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*);
# 36 "cifc.h"
void Cyc_Cifc_user_overrides(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List**,struct Cyc_List_List*);
# 47 "toc.h"
void Cyc_Toc_init (void);
void Cyc_Toc_finish (void);static char _TmpG0[1U]="";
# 46 "tc.cyc"
static struct _fat_ptr Cyc_Tc_tc_msg_c={_TmpG0,_TmpG0,_TmpG0 + 1U};
static struct _fat_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned f1;struct _tuple0*f2;int f3;};
# 49
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(1;exports!=0;exports=exports->tl){
struct _tuple12*p=(struct _tuple12*)exports->hd;
if(!Cyc_Absyn_qvar_cmp(x,(*p).f2)==0)
continue;
# 56
(*p).f3=1;
return 1;}
# 59
return 0;}struct _tuple13{void*f1;int f2;};
# 62
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 64
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_Scope _Tmp5;_Tmp5=vd->sc;_Tmp4=vd->name;_Tmp3=vd->type;_Tmp2=vd->initializer;_Tmp1=vd->attributes;_Tmp0=(int*)& vd->is_proto;{enum Cyc_Absyn_Scope sc=_Tmp5;struct _tuple0*q=_Tmp4;void*t=_Tmp3;struct Cyc_Absyn_Exp*initializer=_Tmp2;struct Cyc_List_List*atts=_Tmp1;int*is_proto=(int*)_Tmp0;
# 71
{void*_Tmp6=Cyc_Absyn_compress(t);unsigned _Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;void*_TmpA;if(*((int*)_Tmp6)==4){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.num_elts==0){_TmpA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.elt_type;_Tmp9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.tq;_Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.zero_term;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.zt_loc;if(initializer!=0){void*telt=_TmpA;struct Cyc_Absyn_Tqual tq=_Tmp9;void*zt=_Tmp8;unsigned ztl=_Tmp7;
# 73
{void*_TmpB=initializer->r;void*_TmpC;struct _fat_ptr _TmpD;switch(*((int*)_TmpB)){case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TmpB)->f1.Wstring_c.tag){case 8: _TmpD=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TmpB)->f1.String_c.val;{struct _fat_ptr s=_TmpD;
# 75
t=({void*_TmpE=({void*_TmpF=telt;struct Cyc_Absyn_Tqual _Tmp10=tq;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_Tmp12=zt;Cyc_Absyn_array_type(_TmpF,_Tmp10,_Tmp11,_Tmp12,ztl);});vd->type=_TmpE;});goto _LL8;}case 9: _TmpD=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TmpB)->f1.Wstring_c.val;{struct _fat_ptr s=_TmpD;
# 77
t=({void*_TmpE=({void*_TmpF=telt;struct Cyc_Absyn_Tqual _Tmp10=tq;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_uint_exp(1U,0U);void*_Tmp12=zt;Cyc_Absyn_array_type(_TmpF,_Tmp10,_Tmp11,_Tmp12,ztl);});vd->type=_TmpE;});goto _LL8;}default: goto _LL15;}case 27: _TmpC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TmpB)->f2;{struct Cyc_Absyn_Exp*e=_TmpC;
_TmpC=e;goto _LL10;}case 28: _TmpC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_TmpB)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_TmpC;
# 80
t=({void*_TmpE=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_TmpE;});goto _LL8;}case 36: _TmpC=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_TmpB)->f2;{struct Cyc_List_List*es=_TmpC;
_TmpC=es;goto _LL14;}case 26: _TmpC=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_TmpB)->f1;_LL14: {struct Cyc_List_List*es=_TmpC;
# 83
t=({void*_TmpE=({void*_TmpF=telt;struct Cyc_Absyn_Tqual _Tmp10=tq;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_uint_exp((unsigned)Cyc_List_length(es),0U);void*_Tmp12=zt;Cyc_Absyn_array_type(_TmpF,_Tmp10,_Tmp11,_Tmp12,ztl);});vd->type=_TmpE;});
goto _LL8;}default: _LL15:
 goto _LL8;}_LL8:;}
# 87
goto _LL3;}else{goto _LL6;}}else{goto _LL6;}}else{_LL6:
 goto _LL3;}_LL3:;}
# 91
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
# 93
({int _Tmp6=Cyc_Tcutil_extract_const_from_typedef(loc,vd->tq.print_const,t);vd->tq.real_const=_Tmp6;});
# 96
({int _Tmp6=!Cyc_Tcutil_is_array_type(t);vd->escapes=_Tmp6;});
# 98
if(Cyc_Tcutil_is_function_type(t)){
*is_proto=0;
atts=Cyc_Atts_transfer_fn_type_atts(t,atts);
Cyc_Atts_fnTypeAttsOK(loc,t);}
# 104
if((int)sc==3 ||(int)sc==4){
if(initializer!=0 && !in_cinclude)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="extern declaration should not have initializer";_tag_fat(_Tmp8,sizeof(char),47U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),1));});}else{
if(!Cyc_Tcutil_is_function_type(t)){
# 111
Cyc_Atts_check_variable_atts(loc,vd,atts);
if(initializer==0 || in_cinclude){
if((check_var_init && !in_cinclude)&& !Cyc_Tcutil_zeroable_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="initializer required for variable ";_tag_fat(_Tmp8,sizeof(char),35U);});_Tmp7;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp8;_Tmp8.tag=11,_Tmp8.f1=vd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA=" of type ";_tag_fat(_TmpA,sizeof(char),10U);});_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)t;_TmpA;});void*_TmpA[4];_TmpA[0]=& _Tmp6,_TmpA[1]=& _Tmp7,_TmpA[2]=& _Tmp8,_TmpA[3]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),4));});}else{
# 119
struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){
{void*t2=Cyc_Tcexp_tcExpInitializer(te,& t,initializer);
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp8=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_Tmp9=initializer;Cyc_Tcutil_coerce_assign(_Tmp8,_Tmp9,t);})){
struct _fat_ptr s0=Cyc_Absynpp_qvar2string(vd->name);
const char*s1=" declared with type ";
struct _fat_ptr s2=Cyc_Absynpp_typ2string(t);
const char*s3=" but initialized with type ";
struct _fat_ptr s4=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _Tmp8=({unsigned long _Tmp9=({unsigned long _TmpA=({unsigned long _TmpB=Cyc_strlen(s0);_TmpB + Cyc_strlen(({const char*_TmpC=s1;_tag_fat(_TmpC,sizeof(char),21U);}));});_TmpA + Cyc_strlen(s2);});_Tmp9 + Cyc_strlen(({const char*_TmpA=s3;_tag_fat(_TmpA,sizeof(char),28U);}));});_Tmp8 + Cyc_strlen(s4);})> 70U)
({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=s0;_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB=s1;_tag_fat(_TmpB,sizeof(char),21U);});_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=s2;_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD=s3;_tag_fat(_TmpD,sizeof(char),28U);});_TmpC;});struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=s4;_TmpD;});void*_TmpD[5];_TmpD[0]=& _Tmp8,_TmpD[1]=& _Tmp9,_TmpD[2]=& _TmpA,_TmpD[3]=& _TmpB,_TmpD[4]=& _TmpC;Cyc_Warn_err(loc,({const char*_TmpE="%s%s\n\t%s\n%s\n\t%s";_tag_fat(_TmpE,sizeof(char),16U);}),_tag_fat(_TmpD,sizeof(void*),5));});else{
# 130
({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=s0;_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB=s1;_tag_fat(_TmpB,sizeof(char),21U);});_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=s2;_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD=s3;_tag_fat(_TmpD,sizeof(char),28U);});_TmpC;});struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=s4;_TmpD;});void*_TmpD[5];_TmpD[0]=& _Tmp8,_TmpD[1]=& _Tmp9,_TmpD[2]=& _TmpA,_TmpD[3]=& _TmpB,_TmpD[4]=& _TmpC;Cyc_Warn_err(loc,({const char*_TmpE="%s%s%s%s%s";_tag_fat(_TmpE,sizeof(char),11U);}),_tag_fat(_TmpD,sizeof(void*),5));});}
Cyc_Unify_explain_failure();}
# 134
if(!Cyc_Tcutil_is_const_exp(initializer))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="initializer is not a constant expression";_tag_fat(_TmpA,sizeof(char),41U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),1));});}
# 120
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_Tmp8)->tag==Cyc_Tcenv_Env_error){
# 137
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="initializer is not a constant expression";_tag_fat(_TmpC,sizeof(char),41U);});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(loc,_tag_fat(_TmpB,sizeof(void*),1));});goto _LL17;}else{_Tmp9=_Tmp8;{void*exn=_Tmp9;(void*)_rethrow(exn);}}_LL17:;}}}}else{
# 141
Cyc_Atts_check_fndecl_atts(loc,atts,0);}}
# 144
{struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){
{struct _tuple13*ans=({(struct _tuple13*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})(te->ae->ordinaries,q);
void*b0=(*ans).f1;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*b1;b1=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct)),b1->tag=1,b1->f1=vd;{
void*b=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);
if(b==0){_npop_handler(0);return;}
# 151
if(exports==0 || Cyc_Tc_export_member(vd->name,*exports)){
if(b!=b0 ||(*ans).f2)
# 154
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict _Tmp9=te->ae->ordinaries;struct _tuple0*_TmpA=q;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple13*))Cyc_Dict_insert;})(_Tmp9,_TmpA,({struct _tuple13*_TmpB=_cycalloc(sizeof(struct _tuple13));
_TmpB->f1=b,_TmpB->f2=(*ans).f2;_TmpB;}));});
# 154
te->ae->ordinaries=_Tmp8;});}
# 156
_npop_handler(0);return;}}
# 145
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp8)->tag==Cyc_Dict_Absent)
# 157
goto _LL1C;else{_Tmp9=_Tmp8;{void*exn=_Tmp9;(void*)_rethrow(exn);}}_LL1C:;}}}
if(exports==0 || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict _Tmp7=te->ae->ordinaries;struct _tuple0*_Tmp8=q;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple13*))Cyc_Dict_insert;})(_Tmp7,_Tmp8,({struct _tuple13*_Tmp9=_cycalloc(sizeof(struct _tuple13));
({void*_TmpA=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_TmpB->tag=1,_TmpB->f1=vd;_TmpB;});_Tmp9->f1=_TmpA;}),_Tmp9->f2=0;_Tmp9;}));});
# 159
te->ae->ordinaries=_Tmp6;});}}
# 163
static int Cyc_Tc_is_main(struct _tuple0*n){
void*_Tmp0;union Cyc_Absyn_Nmspace _Tmp1;_Tmp1=n->f1;_Tmp0=n->f2;{union Cyc_Absyn_Nmspace nms=_Tmp1;struct _fat_ptr*v=_Tmp0;
if(nms.Abs_n.tag==2){if(nms.Abs_n.val==0)
return Cyc_strcmp(*v,({const char*_Tmp2="main";_tag_fat(_Tmp2,sizeof(char),5U);}))==0;else{goto _LL6;}}else{_LL6:
 return 0;};}}
# 171
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 173
struct _tuple0*q=fd->name;
# 177
if((int)fd->sc==4 && !te->in_extern_c_include)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="extern \"C\" functions cannot be implemented in Cyclone";_tag_fat(_Tmp2,sizeof(char),54U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 181
Cyc_Atts_check_fndecl_atts(loc,fd->i.attributes,1);
# 184
if(te->in_extern_c_inc_repeat)
fd->cached_type=0;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
# 189
int nothrow=Cyc_List_mem(Cyc_Atts_attribute_cmp,fd->i.attributes,(void*)& Cyc_Atts_No_throw_att_val);
# 192
({struct Cyc_List_List*_Tmp0=Cyc_Atts_transfer_fn_type_atts(t,fd->i.attributes);fd->i.attributes=_Tmp0;});
Cyc_Atts_fnTypeAttsOK(loc,t);
# 196
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple13*ans=({(struct _tuple13*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})(te->ae->ordinaries,q);
void*b0=(*ans).f1;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b1;b1=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct)),b1->tag=2,b1->f1=fd;{
void*b=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);
if(b!=0){
# 203
if(exports==0 || Cyc_Tc_export_member(q,*exports)){
if(!(b==b0 &&(*ans).f2))
({struct Cyc_Dict_Dict _Tmp2=({struct Cyc_Dict_Dict _Tmp3=te->ae->ordinaries;struct _tuple0*_Tmp4=q;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple13*))Cyc_Dict_insert;})(_Tmp3,_Tmp4,({struct _tuple13*_Tmp5=_cycalloc(sizeof(struct _tuple13));
_Tmp5->f1=b,_Tmp5->f2=(*ans).f2;_Tmp5;}));});
# 205
te->ae->ordinaries=_Tmp2;});}}}}
# 197
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 208
if(exports==0 || Cyc_Tc_export_member(fd->name,*exports))
({struct Cyc_Dict_Dict _Tmp4=({struct Cyc_Dict_Dict _Tmp5=te->ae->ordinaries;struct _tuple0*_Tmp6=q;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple13*))Cyc_Dict_insert;})(_Tmp5,_Tmp6,({struct _tuple13*_Tmp7=_cycalloc(sizeof(struct _tuple13));
({void*_Tmp8=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_Tmp9->tag=2,_Tmp9->f1=fd;_Tmp9;});_Tmp7->f1=_Tmp8;}),_Tmp7->f2=0;_Tmp7;}));});
# 209
te->ae->ordinaries=_Tmp4;});
# 211
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}}_LL0:;}}}
# 215
if(te->in_extern_c_include)return;{
# 220
struct Cyc_Tcenv_Fenv*fenv=Cyc_Tcenv_new_fenv(loc,fd);
struct Cyc_Tcenv_Tenv*_Tmp0;_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),_Tmp0->ns=te->ns,_Tmp0->ae=te->ae,_Tmp0->le=fenv,_Tmp0->allow_valueof=0,_Tmp0->in_extern_c_include=0,_Tmp0->in_tempest=te->in_tempest,_Tmp0->tempest_generalize=te->tempest_generalize,_Tmp0->in_extern_c_inc_repeat=0;{struct Cyc_Tcenv_Tenv*te=_Tmp0;
# 223
Cyc_Tcstmt_tcStmt(te,fd->body,0);
# 226
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
# 229
if(te->in_tempest){
te->tempest_generalize=1;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);
te->tempest_generalize=0;}
# 234
if(nothrow && !Cyc_Tcstmt_ensure_no_throw_stmt(fd->body))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="Function ";_tag_fat(_Tmp3,sizeof(char),10U);});_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=fd->name;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5=" has attribute no_throw but may throw an exception";_tag_fat(_Tmp5,sizeof(char),51U);});_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),3));});
# 237
if(Cyc_Tc_is_main(q)){
# 239
{void*_Tmp1=Cyc_Absyn_compress(fd->i.ret_type);if(*((int*)_Tmp1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)){case 0:
# 241
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="main declared with return type void";_tag_fat(_Tmp4,sizeof(char),36U);});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),1));});goto _LL5;case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)->f2){case Cyc_Absyn_Int_sz:
 goto _LLB;case Cyc_Absyn_Long_sz: _LLB:
 goto _LL5;default: goto _LLC;}default: goto _LLC;}else{_LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="main declared with return type ";_tag_fat(_Tmp4,sizeof(char),32U);});_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)fd->i.ret_type;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=" instead of int or void";_tag_fat(_Tmp6,sizeof(char),24U);});_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),3));});}_LL5:;}
# 247
if(fd->i.c_varargs || fd->i.cyc_varargs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="main declared with varargs";_tag_fat(_Tmp3,sizeof(char),27U);});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});{
struct Cyc_List_List*args=fd->i.args;
if(args!=0){
struct _tuple8*_Tmp1=(struct _tuple8*)args->hd;void*_Tmp2;_Tmp2=_Tmp1->f3;{void*t1=_Tmp2;
{void*_Tmp3=Cyc_Absyn_compress(t1);if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f2){case Cyc_Absyn_Int_sz:
 goto _LL15;case Cyc_Absyn_Long_sz: _LL15:
 goto _LL11;default: goto _LL16;}else{goto _LL16;}}else{_LL16:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="main declared with first argument of type ";_tag_fat(_Tmp6,sizeof(char),43U);});_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t1;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=" instead of int";_tag_fat(_Tmp8,sizeof(char),16U);});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),3));});}_LL11:;}
# 258
args=args->tl;
if(args!=0){
struct _tuple8*_Tmp3=(struct _tuple8*)args->hd;void*_Tmp4;_Tmp4=_Tmp3->f3;{void*t2=_Tmp4;
args=args->tl;
if(args!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="main declared with too many arguments";_tag_fat(_Tmp7,sizeof(char),38U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});{
struct Cyc_Core_Opt*tvs;tvs=_cycalloc(sizeof(struct Cyc_Core_Opt)),tvs->v=fd->i.tvars;
if(((!({void*_Tmp5=t2;Cyc_Unify_unify(_Tmp5,({void*_Tmp6=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp8->v=& Cyc_Kinds_rk;_Tmp8;});Cyc_Absyn_new_evar(_Tmp7,tvs);}));void*_Tmp7=({
struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_rk;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});
# 265
struct Cyc_Absyn_Tqual _Tmp8=
# 267
Cyc_Absyn_empty_tqual(0U);
# 265
void*_Tmp9=
# 267
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 265
Cyc_Absyn_fatptr_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Absyn_false_type);}));})&& !({
# 268
void*_Tmp5=t2;Cyc_Unify_unify(_Tmp5,({void*_Tmp6=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp8->v=& Cyc_Kinds_rk;_Tmp8;});Cyc_Absyn_new_evar(_Tmp7,tvs);}));void*_Tmp7=({
# 270
struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_rk;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});
# 268
struct Cyc_Absyn_Tqual _Tmp8=
# 271
Cyc_Absyn_empty_tqual(0U);
# 268
void*_Tmp9=
# 271
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 268
Cyc_Absyn_fatptr_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Absyn_false_type);}));}))&& !({
# 272
void*_Tmp5=t2;Cyc_Unify_unify(_Tmp5,({void*_Tmp6=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp8->v=& Cyc_Kinds_rk;_Tmp8;});Cyc_Absyn_new_evar(_Tmp7,tvs);}));void*_Tmp7=({
struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_rk;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});
# 272
struct Cyc_Absyn_Tqual _Tmp8=
# 274
Cyc_Absyn_const_tqual(0U);
# 272
void*_Tmp9=
# 274
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 272
Cyc_Absyn_fatptr_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Absyn_false_type);}));}))&& !({
# 275
void*_Tmp5=t2;Cyc_Unify_unify(_Tmp5,({void*_Tmp6=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp8->v=& Cyc_Kinds_rk;_Tmp8;});Cyc_Absyn_new_evar(_Tmp7,tvs);}));void*_Tmp7=({
# 277
struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_rk;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});
# 275
struct Cyc_Absyn_Tqual _Tmp8=
# 278
Cyc_Absyn_const_tqual(0U);
# 275
void*_Tmp9=
# 278
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 275
Cyc_Absyn_fatptr_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Absyn_false_type);}));}))
# 279
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="second argument of main has type ";_tag_fat(_Tmp7,sizeof(char),34U);});_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)t2;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9=" instead of char??";_tag_fat(_Tmp9,sizeof(char),19U);});_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),3));});}}}}}}}}}}}
# 287
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 293
if(({(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_member;})(te->ae->typedefs,q)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="redeclaration of typedef ";_tag_fat(_Tmp2,sizeof(char),26U);});_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=q;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return;}
# 298
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn!=0){
Cyc_Tctyp_check_type(loc,te,td->tvs,& Cyc_Kinds_tak,0,1,_check_null(td->defn));
({int _Tmp0=
Cyc_Tcutil_extract_const_from_typedef(loc,td->tq.print_const,_check_null(td->defn));
# 302
td->tq.real_const=_Tmp0;});}
# 307
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs!=0;tvs=tvs->tl){
void*_Tmp0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp2;
# 310
if(td->defn!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="type variable ";_tag_fat(_Tmp5,sizeof(char),15U);});_Tmp4;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp5;_Tmp5.tag=7,_Tmp5.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7=" is not used in typedef";_tag_fat(_Tmp7,sizeof(char),24U);});_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_Warn_warn2(loc,_tag_fat(_Tmp6,sizeof(void*),3));});
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_tbk);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});goto _LL0;}case 2: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp2;struct Cyc_Absyn_Kind*k=_Tmp1;
# 319
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(k);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});
goto _LL0;}default:
 continue;}_LL0:;}}
# 324
({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*))Cyc_Dict_insert;})(te->ae->typedefs,q,td);te->ae->typedefs=_Tmp0;});}struct _tuple14{void*f1;void*f2;};
# 327
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 334
struct _RegionHandle _Tmp0=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _Tmp0;_push_region(uprev_rgn);
# 336
for(1;rpo!=0;rpo=rpo->tl){
struct _tuple14*_Tmp1=(struct _tuple14*)rpo->hd;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{void*e=_Tmp3;void*r=_Tmp2;
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Kinds_ek,0,0,e);
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Kinds_trk,0,0,r);}}{
# 342
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 345
struct Cyc_List_List*fs=fields;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_Tmp1=(struct Cyc_Absyn_Aggrfield*)fs->hd;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->name;_Tmp6=_Tmp1->tq;_Tmp5=_Tmp1->type;_Tmp4=_Tmp1->width;_Tmp3=_Tmp1->attributes;_Tmp2=_Tmp1->requires_clause;{struct _fat_ptr*fn=_Tmp7;struct Cyc_Absyn_Tqual tq=_Tmp6;void*t=_Tmp5;struct Cyc_Absyn_Exp*width=_Tmp4;struct Cyc_List_List*atts=_Tmp3;struct Cyc_Absyn_Exp*requires_clause=_Tmp2;
# 348
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="duplicate member ";_tag_fat(_TmpA,sizeof(char),18U);});_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=*fn;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),2));});
# 352
if(Cyc_strcmp(*fn,({const char*_Tmp8="";_tag_fat(_Tmp8,sizeof(char),1U);}))!=0)
prev_fields=({struct Cyc_List_List*_Tmp8=_region_malloc(uprev_rgn,sizeof(struct Cyc_List_List));_Tmp8->hd=fn,_Tmp8->tl=prev_fields;_Tmp8;});{
# 355
struct Cyc_Absyn_Kind*field_kind=& Cyc_Kinds_tmk;
# 359
if((int)str_or_union==1 ||
 fs->tl==0 &&(int)str_or_union==0)
field_kind=& Cyc_Kinds_tak;
Cyc_Tctyp_check_type(loc,te,tvs,field_kind,0,0,t);
# 364
({int _Tmp8=Cyc_Tcutil_extract_const_from_typedef(loc,((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq.print_const,t);((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq.real_const=_Tmp8;});
# 367
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
# 369
if((unsigned)requires_clause){
if((int)str_or_union!=1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="@requires clauses are allowed only on union members";_tag_fat(_TmpA,sizeof(char),52U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),1));});{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_allow_valueof(te);
Cyc_Tcexp_tcExp(te2,0,requires_clause);
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="@requires clause has type ";_tag_fat(_TmpA,sizeof(char),27U);});_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)
_check_null(requires_clause->topt);_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC=" instead of integral type";_tag_fat(_TmpC,sizeof(char),26U);});_TmpB;});void*_TmpB[3];_TmpB[0]=& _Tmp8,_TmpB[1]=& _Tmp9,_TmpB[2]=& _TmpA;Cyc_Warn_err2(requires_clause->loc,_tag_fat(_TmpB,sizeof(void*),3));});else{
# 379
({unsigned _Tmp8=requires_clause->loc;struct Cyc_Tcenv_Tenv*_Tmp9=te;struct Cyc_List_List*_TmpA=tvs;Cyc_Tctyp_check_type(_Tmp8,_Tmp9,_TmpA,& Cyc_Kinds_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_TmpB->tag=9,_TmpB->f1=requires_clause;_TmpB;}));});{
# 381
struct Cyc_List_List*relns=Cyc_Relations_exp2relns(uprev_rgn,requires_clause);
# 384
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="@requires clause may be unsatisfiable";_tag_fat(_TmpA,sizeof(char),38U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(requires_clause->loc,_tag_fat(_Tmp9,sizeof(void*),1));});
# 390
{struct Cyc_List_List*p=prev_relations;for(0;p!=0;p=p->tl){
if(Cyc_Relations_consistent_relations(Cyc_List_rappend(uprev_rgn,relns,(struct Cyc_List_List*)p->hd)))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="@requires clause may overlap with previous clauses";_tag_fat(_TmpA,sizeof(char),51U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(requires_clause->loc,_tag_fat(_Tmp9,sizeof(void*),1));});}}
# 394
prev_relations=({struct Cyc_List_List*_Tmp8=_region_malloc(uprev_rgn,sizeof(struct Cyc_List_List));_Tmp8->hd=relns,_Tmp8->tl=prev_relations;_Tmp8;});}}}}else{
# 397
if(prev_relations!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="if one field has a @requires clause, they all must";_tag_fat(_TmpA,sizeof(char),51U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),1));});}}}}}
# 336
;_pop_region();}
# 402
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 404
struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
void*_Tmp0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_Tmp1;enum Cyc_Absyn_AliasQual _Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp3;
# 407
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});continue;}case 2: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->kind){case Cyc_Absyn_MemKind: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2->aliasqual;{struct Cyc_Core_Opt**f=_Tmp3;enum Cyc_Absyn_AliasQual a=_Tmp2;
# 409
if(constrain_top_kind &&(int)a==2)
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(({struct Cyc_Absyn_Kind*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_Tmp7->kind=2U,_Tmp7->aliasqual=0U;_Tmp7;}));_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});else{
# 412
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(({struct Cyc_Absyn_Kind*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_Tmp7->kind=2U,_Tmp7->aliasqual=a;_Tmp7;}));_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});}
continue;}case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->aliasqual==Cyc_Absyn_Top){_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;if(constrain_top_kind){struct Cyc_Core_Opt**f=_Tmp3;
# 415
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(({struct Cyc_Absyn_Kind*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_Tmp7->kind=2U,_Tmp7->aliasqual=0U;_Tmp7;}));_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});
continue;}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp3;struct Cyc_Absyn_Kind*k=_Tmp1;
# 418
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(k);_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});continue;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_MemKind){_Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1->aliasqual;{enum Cyc_Absyn_AliasQual a=_Tmp2;
# 420
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="type ";_tag_fat(_Tmp6,sizeof(char),6U);});_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=n;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=" attempts to abstract type variable ";_tag_fat(_Tmp8,sizeof(char),37U);});_Tmp7;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp8;_Tmp8.tag=7,_Tmp8.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA=" of kind ";_tag_fat(_TmpA,sizeof(char),10U);});_Tmp9;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Kind_Warn_Warg_struct _TmpA;_TmpA.tag=9,({
struct Cyc_Absyn_Kind*_TmpB=({struct Cyc_Absyn_Kind*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_TmpC->kind=1U,_TmpC->aliasqual=a;_TmpC;});_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[6];_TmpA[0]=& _Tmp4,_TmpA[1]=& _Tmp5,_TmpA[2]=& _Tmp6,_TmpA[3]=& _Tmp7,_TmpA[4]=& _Tmp8,_TmpA[5]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),6));});
continue;}}else{
continue;}};}}
# 426
static void Cyc_Tc_rule_out_mem_and_unique(unsigned loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
void*_Tmp0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);enum Cyc_Absyn_AliasQual _Tmp1;enum Cyc_Absyn_KindQual _Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp3;
_Tmp3=f;goto _LL4;}case 2: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->kind){case Cyc_Absyn_MemKind: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->aliasqual){case Cyc_Absyn_Top: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL4: {struct Cyc_Core_Opt**f=_Tmp3;
# 431
_Tmp3=f;goto _LL6;}case Cyc_Absyn_Aliasable: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL6: {struct Cyc_Core_Opt**f=_Tmp3;
# 433
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_AnyKind: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->aliasqual){case Cyc_Absyn_Top: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp3;
_Tmp3=f;goto _LLA;}case Cyc_Absyn_Aliasable: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LLA: {struct Cyc_Core_Opt**f=_Tmp3;
# 436
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ak);_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_RgnKind: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->aliasqual){case Cyc_Absyn_Top: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp3;
# 438
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->aliasqual==Cyc_Absyn_Unique){_LLF: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2->kind;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp3;enum Cyc_Absyn_KindQual k=_Tmp2;
# 442
_Tmp2=k;goto _LL12;}}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_RgnKind)switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->aliasqual){case Cyc_Absyn_Top:
# 440
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="type ";_tag_fat(_Tmp6,sizeof(char),6U);});_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=q;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=" attempts to abstract type variable ";_tag_fat(_Tmp8,sizeof(char),37U);});_Tmp7;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp8;_Tmp8.tag=7,_Tmp8.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA=" of kind TR";_tag_fat(_TmpA,sizeof(char),12U);});_Tmp9;});void*_Tmp9[5];_Tmp9[0]=& _Tmp4,_Tmp9[1]=& _Tmp5,_Tmp9[2]=& _Tmp6,_Tmp9[3]=& _Tmp7,_Tmp9[4]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),5));});
goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->aliasqual==Cyc_Absyn_Unique){_LL11: _Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1->kind;_LL12: {enum Cyc_Absyn_KindQual k=_Tmp2;
# 444
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="type ";_tag_fat(_Tmp6,sizeof(char),6U);});_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=q;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=" attempts to abstract type variable ";_tag_fat(_Tmp8,sizeof(char),37U);});_Tmp7;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp8;_Tmp8.tag=7,_Tmp8.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA=" of kind ";_tag_fat(_TmpA,sizeof(char),10U);});_Tmp9;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Kind_Warn_Warg_struct _TmpA;_TmpA.tag=9,({
struct Cyc_Absyn_Kind*_TmpB=({struct Cyc_Absyn_Kind*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_TmpC->kind=k,_TmpC->aliasqual=1U;_TmpC;});_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[6];_TmpA[0]=& _Tmp4,_TmpA[1]=& _Tmp5,_TmpA[2]=& _Tmp6,_TmpA[3]=& _Tmp7,_TmpA[4]=& _Tmp8,_TmpA[5]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),6));});goto _LL0;}}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_MemKind){_Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1->aliasqual;{enum Cyc_Absyn_AliasQual a=_Tmp1;
# 447
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="type ";_tag_fat(_Tmp6,sizeof(char),6U);});_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=q;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=" attempts to abstract type variable ";_tag_fat(_Tmp8,sizeof(char),37U);});_Tmp7;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp8;_Tmp8.tag=7,_Tmp8.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA=" of kind ";_tag_fat(_TmpA,sizeof(char),10U);});_Tmp9;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Kind_Warn_Warg_struct _TmpA;_TmpA.tag=9,({
struct Cyc_Absyn_Kind*_TmpB=({struct Cyc_Absyn_Kind*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_TmpC->kind=1U,_TmpC->aliasqual=a;_TmpC;});_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[6];_TmpA[0]=& _Tmp4,_TmpA[1]=& _Tmp5,_TmpA[2]=& _Tmp6,_TmpA[3]=& _Tmp7,_TmpA[4]=& _Tmp8,_TmpA[5]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),6));});goto _LL0;}}else{_LL15:
 goto _LL0;}}}}_LL0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 455
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*q=ad->name;
# 461
Cyc_Atts_check_field_atts(loc,(*q).f2,ad->attributes);{
# 463
struct Cyc_List_List*tvs=ad->tvs;
# 466
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 471
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_Tmp2=({(struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt;})(te->ae->aggrdecls,q);_Tmp1.f2=_Tmp2;});_Tmp1;});void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;if(_Tmp0.f1==0){if(_Tmp0.f2==0){
# 474
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 476
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict _Tmp7=te->ae->aggrdecls;struct _tuple0*_Tmp8=q;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**))Cyc_Dict_insert;})(_Tmp7,_Tmp8,({struct Cyc_Absyn_Aggrdecl**_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp9=ad;_Tmp9;}));});te->ae->aggrdecls=_Tmp6;});
goto _LL0;}else{_Tmp5=*_Tmp0.f2;_LL8: {struct Cyc_Absyn_Aggrdecl**adp=_Tmp5;
# 535
struct Cyc_Absyn_Aggrdecl*ad2=Cyc_Tcdecl_merge_aggrdecl(*adp,ad,loc,Cyc_Tc_tc_msg);
if(ad2==0)
return;
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 541
if(ad->impl!=0)
Cyc_Tc_rule_out_memkind(loc,q,_check_null(ad->impl)->exist_vars,1);
*adp=ad2;}}}else{if(_Tmp0.f2==0){_Tmp5=_Tmp0.f1->exist_vars;_Tmp4=_Tmp0.f1->rgn_po;_Tmp3=_Tmp0.f1->fields;_Tmp2=_Tmp0.f1->tagged;{struct Cyc_List_List*exist_vars=_Tmp5;struct Cyc_List_List*rgn_po=_Tmp4;struct Cyc_List_List*fs=_Tmp3;int tagged=_Tmp2;
# 481
struct Cyc_Absyn_Aggrdecl**adp;adp=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*)),({struct Cyc_Absyn_Aggrdecl*_Tmp6=({struct Cyc_Absyn_Aggrdecl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp7->kind=ad->kind,_Tmp7->sc=3U,_Tmp7->name=ad->name,_Tmp7->tvs=tvs,_Tmp7->impl=0,_Tmp7->attributes=ad->attributes,_Tmp7->expected_mem_kind=0;_Tmp7;});*adp=_Tmp6;});
# 483
({struct Cyc_Dict_Dict _Tmp6=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**))Cyc_Dict_insert;})(te->ae->aggrdecls,q,adp);te->ae->aggrdecls=_Tmp6;});
# 488
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 492
if(tagged &&(int)ad->kind==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="@tagged is allowed only on union declarations";_tag_fat(_Tmp8,sizeof(char),46U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),1));});
({struct Cyc_Tcenv_Tenv*_Tmp6=te;unsigned _Tmp7=loc;enum Cyc_Absyn_AggrKind _Tmp8=ad->kind;struct Cyc_List_List*_Tmp9=Cyc_List_append(tvs,exist_vars);struct Cyc_List_List*_TmpA=rgn_po;Cyc_Tc_tcAggrImpl(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,fs);});
# 496
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 499
Cyc_Tc_rule_out_memkind(loc,q,exist_vars,1);
# 501
if((int)ad->kind==1 && !tagged){
# 504
struct Cyc_List_List*f=fs;for(0;f!=0;f=f->tl){
if((Cyc_Flags_tc_aggressive_warn && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))&&((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="member ";_tag_fat(_Tmp8,sizeof(char),8U);});_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=*((struct Cyc_Absyn_Aggrfield*)f->hd)->name;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA=" of union ";_tag_fat(_TmpA,sizeof(char),11U);});_Tmp9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=q;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC=" has type ";_tag_fat(_TmpC,sizeof(char),11U);});_TmpB;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpC;_TmpC.tag=2,_TmpC.f1=(void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type;_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE=" so it can only be written and not read";_tag_fat(_TmpE,sizeof(char),40U);});_TmpD;});void*_TmpD[7];_TmpD[0]=& _Tmp6,_TmpD[1]=& _Tmp7,_TmpD[2]=& _Tmp8,_TmpD[3]=& _Tmp9,_TmpD[4]=& _TmpA,_TmpD[5]=& _TmpB,_TmpD[6]=& _TmpC;Cyc_Warn_warn2(loc,_tag_fat(_TmpD,sizeof(void*),7));});}}
# 509
*adp=ad;
goto _LL0;}}else{_Tmp5=_Tmp0.f1->exist_vars;_Tmp4=_Tmp0.f1->rgn_po;_Tmp3=_Tmp0.f1->fields;_Tmp2=_Tmp0.f1->tagged;_Tmp1=*_Tmp0.f2;{struct Cyc_List_List*exist_vars=_Tmp5;struct Cyc_List_List*rgn_po=_Tmp4;struct Cyc_List_List*fs=_Tmp3;int tagged=_Tmp2;struct Cyc_Absyn_Aggrdecl**adp=_Tmp1;
# 513
if((int)ad->kind!=(int)(*adp)->kind)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="cannot reuse struct names for unions and vice-versa";_tag_fat(_Tmp8,sizeof(char),52U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),1));});{
# 516
struct Cyc_Absyn_Aggrdecl*ad0=*adp;
# 518
({struct Cyc_Absyn_Aggrdecl*_Tmp6=({struct Cyc_Absyn_Aggrdecl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp7->kind=ad->kind,_Tmp7->sc=3U,_Tmp7->name=ad->name,_Tmp7->tvs=tvs,_Tmp7->impl=0,_Tmp7->attributes=ad->attributes,_Tmp7->expected_mem_kind=0;_Tmp7;});*adp=_Tmp6;});
# 524
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 527
if(tagged &&(int)ad->kind==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="@tagged is allowed only on union declarations";_tag_fat(_Tmp8,sizeof(char),46U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),1));});
({struct Cyc_Tcenv_Tenv*_Tmp6=te;unsigned _Tmp7=loc;enum Cyc_Absyn_AggrKind _Tmp8=ad->kind;struct Cyc_List_List*_Tmp9=Cyc_List_append(tvs,exist_vars);struct Cyc_List_List*_TmpA=rgn_po;Cyc_Tc_tcAggrImpl(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,fs);});
# 531
*adp=ad0;
_Tmp5=adp;goto _LL8;}}}}_LL0:;}}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 547
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _fat_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 555
{struct Cyc_List_List*fs=fields;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct Cyc_List_List*typs=f->typs;for(0;typs!=0;typs=typs->tl){
Cyc_Tctyp_check_type(f->loc,te,tvs,& Cyc_Kinds_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);
# 560
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="noalias pointers in datatypes are not allowed: ";_tag_fat(_Tmp2,sizeof(char),48U);});_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=f->name;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp2,sizeof(void*),2));});
# 563
({int _Tmp0=
Cyc_Tcutil_extract_const_from_typedef(f->loc,(*((struct _tuple16*)typs->hd)).f1.print_const,(*((struct _tuple16*)typs->hd)).f2);
# 563
(*((struct _tuple16*)typs->hd)).f1.real_const=_Tmp0;});}}}
# 569
if(is_extensible){
# 571
int res=1;
struct Cyc_List_List*fs=Cyc_Tcdecl_sort_xdatatype_fields(fields,& res,(*name).f2,loc,Cyc_Tc_tc_msg);
return res?fs: 0;}{
# 575
struct _RegionHandle _Tmp0=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _Tmp0;_push_region(uprev_rgn);
# 577
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="duplicate field ";_tag_fat(_Tmp3,sizeof(char),17U);});_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=f->name;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5=" in ";_tag_fat(_Tmp5,sizeof(char),5U);});_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=obj;_Tmp5;});void*_Tmp5[4];_Tmp5[0]=& _Tmp1,_Tmp5[1]=& _Tmp2,_Tmp5[2]=& _Tmp3,_Tmp5[3]=& _Tmp4;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp5,sizeof(void*),4));});else{
# 583
prev_fields=({struct Cyc_List_List*_Tmp1=_region_malloc(uprev_rgn,sizeof(struct Cyc_List_List));_Tmp1->hd=(*f->name).f2,_Tmp1->tl=prev_fields;_Tmp1;});}
# 585
if((int)f->sc!=2){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="ignoring scope of field ";_tag_fat(_Tmp3,sizeof(char),25U);});_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=f->name;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});
f->sc=2U;}}}{
# 590
struct Cyc_List_List*_Tmp1=fields;_npop_handler(0);return _Tmp1;}}
# 577
;_pop_region();}}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 593
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _fat_ptr obj=tud->is_extensible?({const char*_Tmp0="@extensible datatype";_tag_fat(_Tmp0,sizeof(char),21U);}):({const char*_Tmp0="datatype";_tag_fat(_Tmp0,sizeof(char),9U);});
# 599
struct Cyc_List_List*tvs=tud->tvs;
# 602
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 608
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 618 "tc.cyc"
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);
if(tud_opt==0 && !tud->is_extensible)(void*)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Dict_Absent_exn_struct));_Tmp2->tag=Cyc_Dict_Absent;_Tmp2;}));
if(tud_opt!=0)
tud->name=(*(*tud_opt))->name;else{
# 624
({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_Tmp2;});}
# 619
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 626
struct Cyc_Absyn_Datatypedecl***tdopt=({(struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt;})(te->ae->datatypedecls,q);
tud_opt=(unsigned)tdopt?({struct Cyc_Absyn_Datatypedecl***_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl**));*_Tmp4=*tdopt;_Tmp4;}): 0;
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}}_LL0:;}}}{
# 633
struct _tuple17 _Tmp0=({struct _tuple17 _Tmp1;_Tmp1.f1=tud->fields,_Tmp1.f2=tud_opt;_Tmp1;});void*_Tmp1;void*_Tmp2;if(_Tmp0.f1==0){if(_Tmp0.f2==0){
# 636
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict _Tmp4=te->ae->datatypedecls;struct _tuple0*_Tmp5=q;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**))Cyc_Dict_insert;})(_Tmp4,_Tmp5,({struct Cyc_Absyn_Datatypedecl**_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));*_Tmp6=tud;_Tmp6;}));});te->ae->datatypedecls=_Tmp3;});
goto _LL5;}else{_Tmp2=*_Tmp0.f2;_LLD: {struct Cyc_Absyn_Datatypedecl**tudp=_Tmp2;
# 669
struct Cyc_Absyn_Datatypedecl*tud2=Cyc_Tcdecl_merge_datatypedecl(*tudp,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(tud2!=0)
*tudp=tud2;
goto _LL5;}}}else{if(_Tmp0.f2==0){_Tmp2=(struct Cyc_List_List**)& _Tmp0.f1->v;{struct Cyc_List_List**fs=_Tmp2;
# 641
struct Cyc_Absyn_Datatypedecl**tudp;tudp=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*)),({struct Cyc_Absyn_Datatypedecl*_Tmp3=({struct Cyc_Absyn_Datatypedecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_Tmp4->sc=3U,_Tmp4->name=tud->name,_Tmp4->tvs=tvs,_Tmp4->fields=0,_Tmp4->is_extensible=tud->is_extensible;_Tmp4;});*tudp=_Tmp3;});
# 643
({struct Cyc_Dict_Dict _Tmp3=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**))Cyc_Dict_insert;})(te->ae->datatypedecls,q,tudp);te->ae->datatypedecls=_Tmp3;});
# 646
({struct Cyc_List_List*_Tmp3=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_Tmp3;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*tudp=tud;
goto _LL5;}}else{_Tmp2=(struct Cyc_List_List**)& _Tmp0.f1->v;_Tmp1=*_Tmp0.f2;{struct Cyc_List_List**fs=(struct Cyc_List_List**)_Tmp2;struct Cyc_Absyn_Datatypedecl**tudp=_Tmp1;
# 651
struct Cyc_Absyn_Datatypedecl*tud0=*tudp;
# 654
if((!tud->is_extensible &&(unsigned)tud0)&& tud0->is_extensible)
tud->is_extensible=1;
# 657
({struct Cyc_Absyn_Datatypedecl*_Tmp3=({struct Cyc_Absyn_Datatypedecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_Tmp4->sc=3U,_Tmp4->name=tud->name,_Tmp4->tvs=tvs,_Tmp4->fields=0,_Tmp4->is_extensible=tud->is_extensible;_Tmp4;});*tudp=_Tmp3;});
# 661
({struct Cyc_List_List*_Tmp3=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_Tmp3;});
# 664
*tudp=tud0;
_Tmp2=tudp;goto _LLD;}}}_LL5:;}}}
# 677
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 683
if(ed->fields!=0){
struct _RegionHandle _Tmp0=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _Tmp0;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(ed->fields)->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 690
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="duplicate enum constructor ";_tag_fat(_Tmp3,sizeof(char),28U);});_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=f->name;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp3,sizeof(void*),2));});else{
# 693
prev_fields=({struct Cyc_List_List*_Tmp1=_region_malloc(uprev_rgn,sizeof(struct Cyc_List_List));_Tmp1->hd=(*f->name).f2,_Tmp1->tl=prev_fields;_Tmp1;});}
# 695
if(({(struct _tuple13**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt;})(te->ae->ordinaries,f->name)!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="enum field name ";_tag_fat(_Tmp3,sizeof(char),17U);});_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=f->name;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5=" shadows global name";_tag_fat(_Tmp5,sizeof(char),21U);});_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp4,sizeof(void*),3));});
if(f->tag==0)
({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_uint_exp(tag_count ++,f->loc);f->tag=_Tmp1;});else{
# 700
if(Cyc_Tcutil_is_const_exp(_check_null(f->tag))){
struct _tuple11 _Tmp1=Cyc_Evexp_eval_const_uint_exp(_check_null(f->tag));int _Tmp2;unsigned _Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{unsigned t1=_Tmp3;int known=_Tmp2;
if(known)
tag_count=t1 + 1U;}}}}}
# 685
;_pop_region();}
# 709
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_Absyn_Enumdecl**edp=({(struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})(te->ae->enumdecls,q);
struct Cyc_Absyn_Enumdecl*ed2=Cyc_Tcdecl_merge_enumdecl(*edp,ed,loc,Cyc_Tc_tc_msg);
if(ed2==0){_npop_handler(0);return;}
*edp=ed2;}
# 710
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 715
struct Cyc_Absyn_Enumdecl**edp;edp=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl*)),*edp=ed;
({struct Cyc_Dict_Dict _Tmp4=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl**))Cyc_Dict_insert;})(te->ae->enumdecls,q,edp);te->ae->enumdecls=_Tmp4;});
goto _LL3;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}}_LL3:;}}}
# 721
if(ed->fields!=0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(ed->fields)->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,_check_null(f->tag));
if(!Cyc_Tcutil_is_const_exp(_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="enum ";_tag_fat(_Tmp2,sizeof(char),6U);});_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=q;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=", field ";_tag_fat(_Tmp4,sizeof(char),9U);});_Tmp3;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=f->name;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=": expression is not constant";_tag_fat(_Tmp6,sizeof(char),29U);});_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),5));});}}}
# 731
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include,int in_inc_rep){
switch((int)sc){case Cyc_Absyn_Static:
# 734
 if(!in_include)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="static declaration within extern \"C\"";_tag_fat(_Tmp2,sizeof(char),37U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return 0;case Cyc_Absyn_Abstract:
# 738
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="abstract declaration within extern \"C\"";_tag_fat(_Tmp2,sizeof(char),39U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return 0;case Cyc_Absyn_Public:
 goto _LL8;case Cyc_Absyn_Register: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LLA:
 return 1;case Cyc_Absyn_ExternC:
 goto _LLE;default: _LLE:
# 745
 if(!in_inc_rep)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="nested extern \"C\" declaration";_tag_fat(_Tmp2,sizeof(char),30U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;};}
# 751
static void Cyc_Tc_set_scopes(struct Cyc_List_List*ovrs,enum Cyc_Absyn_Scope sc){
for(1;ovrs!=0;ovrs=ovrs->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)ovrs->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
vd->sc=sc;goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
fd->sc=sc;goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
ad->sc=sc;goto _LL0;}case 6: _Tmp1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp1;
tud->sc=sc;goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
ed->sc=sc;goto _LL0;}default:
 goto _LL0;}_LL0:;}}
# 768
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 772
struct Cyc_List_List*ds=ds0;for(0;ds!=0;ds=ds->tl){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
unsigned loc=d->loc;
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 2:
 goto _LL4;case 3: _LL4:
# 778
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="top level let-declarations are not implemented";_tag_fat(_Tmp7,sizeof(char),47U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;case 4:
# 780
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="top level region declarations are not implemented";_tag_fat(_Tmp7,sizeof(char),50U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 782
if(in_externC && Cyc_Tc_okay_externC(d->loc,vd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
vd->sc=4U;
Cyc_Tc_tcVardecl(te,loc,vd,check_var_init,te->in_extern_c_include,exports);
goto _LL0;}case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 787
if(in_externC && Cyc_Tc_okay_externC(d->loc,fd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
fd->sc=4U;
if(te->in_extern_c_include)
fd->orig_scope=4U;
Cyc_Tc_tcFndecl(te,loc,fd,exports);
goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 794
td->extern_c=te->in_extern_c_include;
# 798
Cyc_Tc_tcTypedefdecl(te,loc,td);
goto _LL0;}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 801
if(in_externC && Cyc_Tc_okay_externC(d->loc,ad->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ad->sc=4U;
Cyc_Tc_tcAggrdecl(te,loc,ad);
goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;
# 806
if(in_externC && Cyc_Tc_okay_externC(d->loc,tud->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
tud->sc=4U;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
# 811
if(in_externC && Cyc_Tc_okay_externC(d->loc,ed->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ed->sc=4U;
Cyc_Tc_tcEnumdecl(te,loc,ed);
goto _LL0;}case 13:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="spurious __cyclone_port_on__";_tag_fat(_Tmp7,sizeof(char),29U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(d->loc,_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;case 14:
 goto _LL0;case 15:
 te=Cyc_Tcenv_enter_tempest(te);goto _LL0;case 16:
 te=Cyc_Tcenv_clear_tempest(te);goto _LL0;case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*ds2=_Tmp3;
# 821
struct Cyc_List_List*ns=te->ns;
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=ns;Cyc_List_append(_Tmp6,({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=v,_Tmp7->tl=0;_Tmp7;}));});te->ns=_Tmp5;});
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);
te->ns=ns;
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*ds2=_Tmp4;
# 828
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds2=_Tmp4;
# 831
Cyc_Tc_tc_decls(te,ds2,1,check_var_init,exports);goto _LL0;}default: _Tmp4=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List**ds2=(struct Cyc_List_List**)_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exports2=_Tmp2;struct _tuple10*wc=_Tmp1;
# 836
if((unsigned)ovrs){
struct Cyc_Tcenv_Tenv*tecpy=Cyc_Tcenv_copy_tenv_dicts(te);
tecpy->in_extern_c_include=1;
Cyc_Tc_tc_decls(tecpy,*ds2,1,check_var_init,0);
Cyc_Toc_init();
Cyc_Tc_set_scopes(ovrs,4U);
Cyc_Cifc_user_overrides(d->loc,tecpy,ds2,ovrs);
Cyc_Toc_finish();}{
# 845
struct Cyc_List_List*newexs=
Cyc_List_append(exports2,(unsigned)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_enter_extern_c_include(te);
te2->in_extern_c_inc_repeat=ovrs!=0;
Cyc_Tc_tc_decls(te2,*ds2,1,check_var_init,& newexs);
# 852
for(1;exports2!=0;exports2=exports2->tl){
struct _tuple12*exp=(struct _tuple12*)exports2->hd;
if(!(*exp).f3)
({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(*exp).f2;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=" is exported but not defined";_tag_fat(_Tmp8,sizeof(char),29U);});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_Warn_warn2((*exp).f1,_tag_fat(_Tmp7,sizeof(void*),2));});}
# 857
goto _LL0;}}}_LL0:;}}
# 862
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);{
struct Cyc_Dict_Dict cg=Cyc_Callgraph_compute_callgraph(ds);
# 867
struct Cyc_Dict_Dict scc=Cyc_Graph_scc(cg);
# 870
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 888 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*);
# 890
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*,struct Cyc_List_List*))Cyc_List_filter_c;})(Cyc_Tc_vardecl_needed,env,ds);}
# 894
static int Cyc_Tc_is_extern(struct Cyc_Absyn_Vardecl*vd){
if((int)vd->sc==3 ||(int)vd->sc==4)
return 1;{
void*_Tmp0=Cyc_Absyn_compress(vd->type);if(*((int*)_Tmp0)==5)
return 1;else{
return 0;};}}
# 903
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 907
if((env->in_cinclude || !Cyc_Tc_is_extern(vd))|| !({
struct _tuple0*_Tmp2=vd->name;Cyc_Absyn_qvar_cmp(_Tmp2,Cyc_Absyn_uniquergn_qvar());}))
return 1;
return(*({(struct _tuple13*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})(env->ordinaries,vd->name)).f2;}case 11: _Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List**ds2p=_Tmp1;
_Tmp1=ds2p;goto _LL6;}case 10: _Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL6: {struct Cyc_List_List**ds2p=_Tmp1;
_Tmp1=ds2p;goto _LL8;}case 9: _Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL8: {struct Cyc_List_List**ds2p=_Tmp1;
({struct Cyc_List_List*_Tmp2=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_Tmp2;});return 1;}case 12: _Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List**ds2p=(struct Cyc_List_List**)_Tmp1;
# 915
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_Tmp2=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_Tmp2;});
env->in_cinclude=in_cinclude;
return 1;}default:
 return 1;};}
# 924
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv env=({struct Cyc_Tc_TreeshakeEnv _Tmp0;_Tmp0.in_cinclude=0,_Tmp0.ordinaries=te->ae->ordinaries;_Tmp0;});
return Cyc_Tc_treeshake_f(& env,ds);}

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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 123
int Cyc_Core_ptrcmp(void*,void*);
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;
# 292 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 36 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);
# 46
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 504
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 870 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 901
extern void*Cyc_Absyn_void_type;
# 979
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
# 1113
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1115
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);
# 1127
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1129
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1131
int Cyc_Absyn_is_require_union_type(void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
extern union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);extern union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 64
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*);
# 68
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation);
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*,union Cyc_Relations_RelnOp*);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*,union Cyc_Relations_RelnOp,enum Cyc_Relations_Relation,union Cyc_Relations_RelnOp,struct Cyc_List_List*);
# 100
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 103
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 108
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 112
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 118
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 121
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp);
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation);
struct _fat_ptr Cyc_Relations_relns2string(struct Cyc_List_List*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 36
int Cyc_Tcutil_is_void_type(void*);
# 38
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 51
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 53
int Cyc_Tcutil_is_bound_one(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 137
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 173
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**,int*,void**);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*,int);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 221
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 113
extern void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict,int(*)(void*,void*),void*);
# 126 "dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict,void*,void**);
# 149
extern void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned,struct _fat_ptr,struct _fat_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
extern union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
extern union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void);struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;int f4;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple15{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 142 "cf_flowinfo.h"
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void);
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 157
extern struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void);
# 159
extern int Cyc_CfFlowInfo_get_field_index(void*,struct _fat_ptr*);
extern int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*,struct _fat_ptr*);
# 162
extern int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 165
extern struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int,void*);
extern void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*,int,void*);
extern void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*,void*,struct Cyc_Absyn_Exp*,int,void*,int);
extern int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*,int,void*,int*);
extern void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*,void*);struct _tuple16{void*f1;struct Cyc_List_List*f2;};
extern struct _tuple16 Cyc_CfFlowInfo_unname_rval(void*);
# 172
extern enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
extern void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*);
# 175
extern int Cyc_CfFlowInfo_is_init_pointer(void*);
extern int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 178
extern void Cyc_CfFlowInfo_print_absrval(void*);
# 185
extern void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo);
# 201 "cf_flowinfo.h"
extern struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
extern struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*,unsigned,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*,void*);
# 206
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_tryflow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple17{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
extern struct _tuple17 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple17,struct _tuple17);
# 112 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 54 "flags.h"
extern int Cyc_Flags_warn_lose_unique;
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 53 "new_control_flow.cyc"
static int Cyc_NewControlFlow_strcmp(struct _fat_ptr s1,struct _fat_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned sz1=_get_fat_size(s1,sizeof(char));
unsigned sz2=_get_fat_size(s2,sizeof(char));
unsigned minsz=sz1 < sz2?sz1: sz2;
# 61
while((unsigned)i < minsz){
char c1=*((const char*)_check_fat_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_fat_subscript(s2,sizeof(char),i));
if((int)c1 == 0){
if((int)c2 == 0)return 0;else{
return -1;}}else{
if((int)c2 == 0)return 1;else{
# 69
int diff=(int)c1 - (int)c2;
if(diff != 0)return diff;}}
# 72
++ i;}
# 74
if(sz1 == sz2)return 0;
if(minsz < sz2){
if((int)*((const char*)_check_fat_subscript(s2,sizeof(char),i))== 0)return 0;else{
return -1;}}else{
# 79
if((int)*((const char*)_check_fat_subscript(s1,sizeof(char),i))== 0)return 0;else{
return 1;}}}}struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};struct _tuple18{void*f1;int f2;};
# 125 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple18*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int,int,struct Cyc_Absyn_Exp*);
static struct _tuple17 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*,int);struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple20 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
# 131
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_List_List*);
# 135
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_stmttmp0=s->annot;void*_Tmp0=_stmttmp0;void*_Tmp1;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_NewControlFlow_CFAnnot){_Tmp1=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->f1;{struct Cyc_NewControlFlow_CFStmtAnnot*x=(struct Cyc_NewControlFlow_CFStmtAnnot*)_Tmp1;
return x;}}else{
({void*_Tmp2=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct));_Tmp3->tag=Cyc_NewControlFlow_CFAnnot,(_Tmp3->f1).visited=0;_Tmp3;});s->annot=_Tmp2;});return Cyc_NewControlFlow_get_stmt_annot(s);};}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=({(union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res;res=_cycalloc(sizeof(union Cyc_CfFlowInfo_FlowInfo)),({union Cyc_CfFlowInfo_FlowInfo _Tmp0=Cyc_CfFlowInfo_BottomFL();*res=_Tmp0;});
({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,union Cyc_CfFlowInfo_FlowInfo*))Cyc_Hashtable_insert;})(env->flow_table,s,res);
return res;}
# 149
return*sflow;}
# 152
static struct Cyc_List_List*Cyc_NewControlFlow_flowrelns(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _Tmp0=f;void*_Tmp1;if((_Tmp0.BottomFL).tag == 1)
return 0;else{_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_List_List*r=_Tmp1;
return r;}};}struct _tuple21{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 159
static struct _tuple21 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*annot=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*sflow=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 163
({union Cyc_CfFlowInfo_FlowInfo _Tmp0=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*sflow);*sflow=_Tmp0;});
# 169
annot->visited=env->iteration_num;
return({struct _tuple21 _Tmp0;_Tmp0.f1=annot,_Tmp0.f2=sflow;_Tmp0;});}
# 179
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 187
({union Cyc_CfFlowInfo_FlowInfo _Tmp0=Cyc_CfFlowInfo_join_tryflow(env->fenv,new_flow,env->tryflow);env->tryflow=_Tmp0;});}struct _tuple22{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned f2;struct Cyc_Dict_Dict f3;};
# 194
static void Cyc_NewControlFlow_check_unique_root(struct _tuple22*ckenv,void*root,void*rval){
# 196
struct _tuple22*_Tmp0=ckenv;struct Cyc_Dict_Dict _Tmp1;unsigned _Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{struct Cyc_NewControlFlow_AnalEnv*env=_Tmp3;unsigned loc=_Tmp2;struct Cyc_Dict_Dict new_fd=_Tmp1;
void*_Tmp4=root;void*_Tmp5;if(*((int*)_Tmp4)== 0){_Tmp5=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;
# 199
if(!Cyc_Dict_lookup_bool(new_fd,root,& rval)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
retry: {void*_Tmp6=rval;void*_Tmp7;switch(*((int*)_Tmp6)){case 8: _Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp6)->f2;{void*r=_Tmp7;
rval=r;goto retry;}case 7:
 goto _LLE;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp6)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE:
 goto _LL10;}else{goto _LL11;}case 0: _LL10:
 goto _LL8;default: _LL11:
# 208
({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_qvar2string(vd->name);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_warn(loc,({const char*_TmpA="alias-free pointer(s) reachable from %s may become unreachable.";_tag_fat(_TmpA,sizeof(char),64U);}),_tag_fat(_Tmp9,sizeof(void*),1));});
goto _LL8;}_LL8:;}}
# 212
goto _LL3;}}else{
goto _LL3;}_LL3:;}}
# 220
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*annot=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*sflow=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo new_flow=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*sflow);
# 226
if(Cyc_Flags_warn_lose_unique){
struct _tuple20 _stmttmp1=({struct _tuple20 _Tmp0;_Tmp0.f1=flow,_Tmp0.f2=new_flow;_Tmp0;});struct _tuple20 _Tmp0=_stmttmp1;struct Cyc_Dict_Dict _Tmp1;struct Cyc_Dict_Dict _Tmp2;if(((_Tmp0.f1).ReachableFL).tag == 2){if(((_Tmp0.f2).ReachableFL).tag == 2){_Tmp2=(((_Tmp0.f1).ReachableFL).val).f1;_Tmp1=(((_Tmp0.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict fd=_Tmp2;struct Cyc_Dict_Dict new_fd=_Tmp1;
# 229
struct _tuple22 ckenv=({struct _tuple22 _Tmp3;_Tmp3.f1=env,_Tmp3.f2=s->loc,_Tmp3.f3=new_fd;_Tmp3;});
({(void(*)(void(*)(struct _tuple22*,void*,void*),struct _tuple22*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_NewControlFlow_check_unique_root,& ckenv,fd);
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 234
if(!Cyc_CfFlowInfo_flow_lessthan_approx(new_flow,*sflow)){
# 241
*sflow=new_flow;
# 245
if(annot->visited == env->iteration_num)
# 247
env->iterate_again=1;}}
# 252
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned loc,int nameit){
# 256
union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 259
for(1;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct)),root->tag=0,root->f1=(struct Cyc_Absyn_Vardecl*)vds->hd;{
void*rval=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp3->tag=8,_Tmp3->f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_Tmp3->f2=rval;_Tmp3;});
# 266
d=Cyc_Dict_insert(d,(void*)root,rval);}}
# 268
return Cyc_CfFlowInfo_ReachableFL(d,relns);}};}
# 272
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 279
for(1;(unsigned)req;req=req->tl){
struct Cyc_Relations_Reln*neg_rln=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=neg_rln,_Tmp0->tl=assume;_Tmp0;})))
return 0;}
# 284
return 1;}
# 287
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_stmttmp2=e->r;void*_Tmp0=_stmttmp2;void*_Tmp1;if(*((int*)_Tmp0)== 14){_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return e1;}}else{
return e;};}
# 294
static void Cyc_NewControlFlow_check_union_requires(unsigned loc,void*t,struct _fat_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 296
union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;void*_Tmp1;if((_Tmp0.BottomFL).tag == 1)
return;else{_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_List_List*relns=_Tmp1;
# 299
{void*_stmttmp3=Cyc_Absyn_compress(t);void*_Tmp2=_stmttmp3;void*_Tmp3;union Cyc_Absyn_AggrInfo _Tmp4;switch(*((int*)_Tmp2)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)== 20){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2;{union Cyc_Absyn_AggrInfo ainfo=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 301
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);
struct Cyc_Absyn_Aggrfield*field=Cyc_Absyn_lookup_decl_field(ad,f);
struct Cyc_Absyn_Exp*req=(_check_null(field))->requires_clause;
if(req != 0){
struct _RegionHandle _Tmp5=_new_region("temp");struct _RegionHandle*temp=& _Tmp5;_push_region(temp);
{struct Cyc_Absyn_Exp*ireq=({struct _RegionHandle*_Tmp6=temp;struct Cyc_List_List*_Tmp7=Cyc_List_rzip(temp,temp,ad->tvs,ts);Cyc_Tcutil_rsubsexp(_Tmp6,_Tmp7,req);});
# 308
if(!({struct Cyc_List_List*_Tmp6=relns;Cyc_NewControlFlow_relns_ok(_Tmp6,Cyc_Relations_exp2relns(temp,ireq));})){
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(ireq));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=*f;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp9="unable to prove %s, required to access %s";_tag_fat(_Tmp9,sizeof(char),42U);}),_tag_fat(_Tmp8,sizeof(void*),2));});
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="  [recorded facts on path: ";_tag_fat(_Tmp6,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_Relations_print_relns(Cyc_stderr,relns);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="]\n";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));}}
# 306
;_pop_region();}
# 315
goto _LL5;}}else{goto _LLA;}case 7: _Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp2)->f2;{struct Cyc_List_List*fs=_Tmp3;
# 317
struct Cyc_Absyn_Aggrfield*field=Cyc_Absyn_lookup_field(fs,f);
struct Cyc_Absyn_Exp*req=(_check_null(field))->requires_clause;
if(req != 0){
struct _RegionHandle _Tmp5=_new_region("temp");struct _RegionHandle*temp=& _Tmp5;_push_region(temp);
if(!({struct Cyc_List_List*_Tmp6=relns;Cyc_NewControlFlow_relns_ok(_Tmp6,Cyc_Relations_exp2relns(temp,req));})){
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(req));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=*f;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp9="unable to prove %s, required to access %s";_tag_fat(_Tmp9,sizeof(char),42U);}),_tag_fat(_Tmp8,sizeof(void*),2));});
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="  [recorded facts on path: ";_tag_fat(_Tmp6,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_Relations_print_relns(Cyc_stderr,relns);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="]\n";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));}
# 321
;_pop_region();}
# 328
goto _LL5;}default: _LLA:
 goto _LL5;}_LL5:;}
# 331
goto _LL0;}}_LL0:;}
# 335
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 339
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r)!= 1)
Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp3="expression may not be fully initialized";_tag_fat(_Tmp3,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);
if(d.t == ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo ans=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}};}struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 349
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,void*r){
struct _tuple0 _stmttmp4=({struct _tuple0 _Tmp0;({void*_Tmp1=Cyc_Absyn_compress(t);_Tmp0.f1=_Tmp1;}),_Tmp0.f2=r;_Tmp0;});struct _tuple0 _Tmp0=_stmttmp4;enum Cyc_Absyn_AggrKind _Tmp1;union Cyc_Absyn_AggrInfo _Tmp2;struct _fat_ptr _Tmp3;void*_Tmp4;switch(*((int*)_Tmp0.f2)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1 == Cyc_CfFlowInfo_NoneIL)
return;else{switch(*((int*)_Tmp0.f1)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 20: goto _LL13;default: goto _LL13;}case 6: goto _LL13;case 7: goto _LL13;case 3: goto _LL11;default: goto _LL13;}}case 0:
 return;case 7:
 return;case 8: _Tmp4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{void*r=_Tmp4;
Cyc_NewControlFlow_check_nounique(env,loc,t,r);return;}default: switch(*((int*)_Tmp0.f1)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(*((int*)_Tmp0.f2)== 6){_Tmp4=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1).KnownDatatypefield).val).f2;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;struct _fat_ptr ad=_Tmp3;
# 356
if(tuf->typs == 0)
return;
_Tmp4=tuf->typs;_Tmp3=ad;goto _LLC;}}else{goto _LL13;}}else{goto _LL13;}case 20: if(*((int*)_Tmp0.f2)== 6){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp2;struct Cyc_List_List*targs=_Tmp4;struct _fat_ptr d=_Tmp3;
# 367
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl == 0)return;{
struct Cyc_List_List*fs=(_check_null(ad->impl))->fields;
struct _RegionHandle _Tmp5=_new_region("temp");struct _RegionHandle*temp=& _Tmp5;_push_region(temp);
{struct Cyc_List_List*inst=Cyc_List_rzip(temp,temp,ad->tvs,targs);
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)(_check_null(fs))->hd)->type;
if(inst != 0)t=Cyc_Tcutil_rsubstitute(temp,inst,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)d.curr)[i]);}}
# 377
_npop_handler(0);return;}
# 371
;_pop_region();}}}else{goto _LL13;}default: goto _LL13;}case 6: if(*((int*)_Tmp0.f2)== 6){_Tmp4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;_LLC: {struct Cyc_List_List*tqts=_Tmp4;struct _fat_ptr ad=_Tmp3;
# 360
unsigned sz=(unsigned)Cyc_List_length(tqts);
{int i=0;for(0;(unsigned)i < sz;(i ++,tqts=tqts->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_Tmp5=env;unsigned _Tmp6=loc;void*_Tmp7=(*((struct _tuple23*)(_check_null(tqts))->hd)).f2;Cyc_NewControlFlow_check_nounique(_Tmp5,_Tmp6,_Tmp7,*((void**)_check_fat_subscript(ad,sizeof(void*),i)));});}}
# 364
return;}}else{goto _LL13;}case 7: if(*((int*)_Tmp0.f2)== 6){_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp4;struct _fat_ptr d=_Tmp3;
# 380
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)(_check_null(fs))->hd)->type,((void**)d.curr)[i]);}}
return;}}else{goto _LL13;}case 3: _LL11:
# 384
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
Cyc_Warn_warn(loc,({const char*_Tmp5="argument may still contain alias-free pointers";_tag_fat(_Tmp5,sizeof(char),47U);}),_tag_fat(0U,sizeof(void*),0));
return;default: _LL13:
 return;}};}
# 391
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 395
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp3="noliveunique attribute requires alias-free pointer";_tag_fat(_Tmp3,sizeof(char),51U);}),_tag_fat(0U,sizeof(void*),0));{
void*elt_type=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_Tmp3=r;void*_Tmp4;switch(*((int*)_Tmp3)){case 8: _Tmp4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp3)->f2;{void*r2=_Tmp4;
r=r2;goto retry;}case 4: _Tmp4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp3)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp4;
_Tmp4=Cyc_CfFlowInfo_lookup_place(d,p);goto _LLB;}case 5: _Tmp4=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp3)->f1;_LLB: {void*r=_Tmp4;
Cyc_NewControlFlow_check_nounique(env,loc,elt_type,r);goto _LL5;}default:
# 403
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(elt_type))
Cyc_Warn_warn(loc,({const char*_Tmp5="argument may contain live alias-free pointers";_tag_fat(_Tmp5,sizeof(char),46U);}),_tag_fat(0U,sizeof(void*),0));
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 407
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);
if(d.t == ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo ans=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}}};}struct _tuple24{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 417
static struct _tuple24 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 423
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple24 _Tmp0;_Tmp0.f1=inflow,_Tmp0.f2=0;_Tmp0;});
do{
struct _tuple17 _stmttmp5=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd,0);struct _tuple17 _Tmp0=_stmttmp5;void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp2;void*r=_Tmp1;
inflow=f;
rvals=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=r,_Tmp3->tl=rvals;_Tmp3;});
es=es->tl;
first_is_copy=others_are_copy;}}while(es != 0);
# 434
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple24 _Tmp0;_Tmp0.f1=inflow,({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(rvals);_Tmp0.f2=_Tmp1;});_Tmp0;});}
# 440
static struct _tuple17 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 442
struct _tuple24 _stmttmp6=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 442
struct _tuple24 _Tmp0=_stmttmp6;void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_CfFlowInfo_FlowInfo afterflow=_Tmp2;struct Cyc_List_List*rvals=_Tmp1;
# 444
{union Cyc_CfFlowInfo_FlowInfo _Tmp3=afterflow;struct Cyc_Dict_Dict _Tmp4;if((_Tmp3.ReachableFL).tag == 2){_Tmp4=((_Tmp3.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_Tmp4;
# 446
for(1;rvals != 0;(rvals=rvals->tl,es=(_check_null(es))->tl)){
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,(void*)rvals->hd)== 0)
Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)(_check_null(es))->hd)->loc,({const char*_Tmp5="expression may not be initialized";_tag_fat(_Tmp5,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));}
goto _LL3;}}else{
goto _LL3;}_LL3:;}
# 452
return({struct _tuple17 _Tmp3;_Tmp3.f1=afterflow,_Tmp3.f2=(env->fenv)->unknown_all;_Tmp3;});}}
# 459
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 466
int needs_unconsume=0;
void*old_rval=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume))
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp0="consume_zero_rval";_tag_fat(_Tmp0,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));else{
# 471
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,_check_null(e->topt),e,env->iteration_num,new_rval,0);else{
# 475
return new_rval;}}}
# 488 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 493
union Cyc_CfFlowInfo_FlowInfo _Tmp0=outflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return outflow;else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 496
union Cyc_CfFlowInfo_AbsLVal _stmttmp7=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _Tmp3=_stmttmp7;void*_Tmp4;if((_Tmp3.UnknownL).tag == 2)
# 500
return outflow;else{_Tmp4=(_Tmp3.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_Tmp4;
# 504
void*nzval=(int)il == 1?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(1;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=8,_Tmp5->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp5->f2=nzval;_Tmp5;});}
# 508
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo outflow=({struct Cyc_Dict_Dict _Tmp5=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval);Cyc_CfFlowInfo_ReachableFL(_Tmp5,relns);});
# 513
return outflow;}}};}};}
# 522
static struct _tuple20 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 524
union Cyc_CfFlowInfo_FlowInfo _Tmp0=outflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return({struct _tuple20 _Tmp3;_Tmp3.f1=outflow,_Tmp3.f2=outflow;_Tmp3;});else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 527
union Cyc_CfFlowInfo_AbsLVal _stmttmp8=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _Tmp3=_stmttmp8;void*_Tmp4;if((_Tmp3.UnknownL).tag == 2)
return({struct _tuple20 _Tmp5;_Tmp5.f1=outflow,_Tmp5.f2=outflow;_Tmp5;});else{_Tmp4=(_Tmp3.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_Tmp4;
# 530
void*nzval=(int)il == 1?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(1;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=8,_Tmp5->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp5->f2=nzval;_Tmp5;});
zval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=8,_Tmp5->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp5->f2=zval;_Tmp5;});}
# 536
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,zval);
return({struct _tuple20 _Tmp5;({
union Cyc_CfFlowInfo_FlowInfo _Tmp6=({struct Cyc_Dict_Dict _Tmp7=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval);Cyc_CfFlowInfo_ReachableFL(_Tmp7,relns);});_Tmp5.f1=_Tmp6;}),({
union Cyc_CfFlowInfo_FlowInfo _Tmp6=({struct Cyc_Dict_Dict _Tmp7=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,zval);Cyc_CfFlowInfo_ReachableFL(_Tmp7,relns);});_Tmp5.f2=_Tmp6;});_Tmp5;});}};}};}
# 545
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 548
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
# 551
{void*_stmttmp9=e->annot;void*_Tmp0=_stmttmp9;void*_Tmp1;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_CfFlowInfo_UnknownZ){_Tmp1=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp0)->f1;{struct Cyc_List_List*relns2=_Tmp1;
# 553
if(possibly_null && Cyc_Relations_same_relns(relns,relns2))return;
goto _LL0;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_CfFlowInfo_NotZero){_Tmp1=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_Tmp0)->f1;{struct Cyc_List_List*relns2=_Tmp1;
# 556
if(!possibly_null && Cyc_Relations_same_relns(relns,relns2))return;
goto _LL0;}}else{
goto _LL0;}}_LL0:;}
# 560
if(possibly_null)
({void*_Tmp0=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct));_Tmp1->tag=Cyc_CfFlowInfo_UnknownZ,_Tmp1->f1=relns;_Tmp1;});e->annot=_Tmp0;});else{
# 563
({void*_Tmp0=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct));_Tmp1->tag=Cyc_CfFlowInfo_NotZero,_Tmp1->f1=relns;_Tmp1;});e->annot=_Tmp0;});}}
# 571
static struct _tuple17 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_Absyn_Exp*index){
# 575
void*_stmttmpA=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp0=_stmttmpA;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)== 3){_Tmp2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).bounds;{void*elttype=_Tmp2;void*bd=_Tmp1;
# 577
union Cyc_CfFlowInfo_FlowInfo _Tmp3=f;void*_Tmp4;struct Cyc_Dict_Dict _Tmp5;if((_Tmp3.BottomFL).tag == 1)
# 579
return({struct _tuple17 _Tmp6;_Tmp6.f1=f,({void*_Tmp7=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_Tmp6.f2=_Tmp7;});_Tmp6;});else{_Tmp5=((_Tmp3.ReachableFL).val).f1;_Tmp4=((_Tmp3.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_Tmp5;struct Cyc_List_List*relns=_Tmp4;
# 581
struct _tuple16 _stmttmpB=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _Tmp6=_stmttmpB;void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{void*r=_Tmp8;struct Cyc_List_List*names=_Tmp7;
{void*_Tmp9=r;enum Cyc_CfFlowInfo_InitLevel _TmpA;void*_TmpB;void*_TmpC;switch(*((int*)_Tmp9)){case 8: _TmpC=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp9)->f1;_TmpB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp9)->f2;{struct Cyc_Absyn_Vardecl*n=_TmpC;void*r2=_TmpB;
# 584
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpD="named location in anal_derefR";_tag_fat(_TmpD,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));}case 1:
# 586
 Cyc_NewControlFlow_update_relns(e,0,relns);
goto _LLD;case 4: _TmpC=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp9)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpC;
# 589
Cyc_NewControlFlow_update_relns(e,0,relns);
if(index == 0 && Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple17 _TmpD;_TmpD.f1=f,({void*_TmpE=Cyc_CfFlowInfo_lookup_place(outdict,p);_TmpD.f2=_TmpE;});_TmpD;});
goto _LLD;}case 5: _TmpC=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp9)->f1;{void*r=_TmpC;
# 595
Cyc_NewControlFlow_update_relns(e,1,relns);
if(index == 0 && Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple17 _TmpD;_TmpD.f1=f,_TmpD.f2=r;_TmpD;});
goto _LLD;}case 0:
# 600
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple17 _TmpD;({union Cyc_CfFlowInfo_FlowInfo _TmpE=Cyc_CfFlowInfo_BottomFL();_TmpD.f1=_TmpE;}),({void*_TmpE=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_TmpD.f2=_TmpE;});_TmpD;});case 2: _TmpA=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp9)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_TmpA;
# 604
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
goto _LL1B;}default: _LL1B:
# 607
 Cyc_NewControlFlow_update_relns(e,1,relns);
goto _LLD;}_LLD:;}{
# 610
enum Cyc_CfFlowInfo_InitLevel _stmttmpC=Cyc_CfFlowInfo_initlevel(env->fenv,outdict,r);enum Cyc_CfFlowInfo_InitLevel _Tmp9=_stmttmpC;if(_Tmp9 == Cyc_CfFlowInfo_NoneIL){
# 612
struct _tuple16 _stmttmpD=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _TmpA=_stmttmpD;void*_TmpB;_TmpB=_TmpA.f1;{void*r=_TmpB;
{void*_TmpC=r;if(*((int*)_TmpC)== 7){
# 615
Cyc_CfFlowInfo_aerr(e->loc,({const char*_TmpD="attempt to dereference a consumed alias-free pointer";_tag_fat(_TmpD,sizeof(char),53U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL24;}else{
# 618
Cyc_CfFlowInfo_aerr(e->loc,({const char*_TmpD="dereference of possibly uninitialized pointer";_tag_fat(_TmpD,sizeof(char),46U);}),_tag_fat(0U,sizeof(void*),0));}_LL24:;}
# 620
goto _LL20;}}else{_LL20:
# 622
 return({struct _tuple17 _TmpA;_TmpA.f1=f,({void*_TmpB=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_TmpA.f2=_TmpB;});_TmpA;});};}}}};}}else{
# 625
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp3="right deref of non-pointer-type";_tag_fat(_Tmp3,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));};}
# 632
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 634
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 637
{void*_stmttmpE=e1->r;void*_Tmp0=_stmttmpE;void*_Tmp1;if(*((int*)_Tmp0)== 1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b1=_Tmp1;
# 639
struct Cyc_Absyn_Vardecl*x=Cyc_Tcutil_nonesc_vardecl(b1);
if(x != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(x);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,3U,n1,relns);}
# 645
goto _LL0;}}else{
goto _LL0;}_LL0:;}{
# 649
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,3U,rbound,relns);}
# 655
return relns;}}
# 663
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned loc,void*old_rval){
# 668
struct _tuple19 _stmttmpF=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple19 _Tmp0=_stmttmpF;union Cyc_CfFlowInfo_AbsLVal _Tmp1;_Tmp1=_Tmp0.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_Tmp1;
{struct _tuple19 _stmttmp10=({struct _tuple19 _Tmp2;_Tmp2.f1=inflow,_Tmp2.f2=lval;_Tmp2;});struct _tuple19 _Tmp2=_stmttmp10;void*_Tmp3;void*_Tmp4;struct Cyc_Dict_Dict _Tmp5;if(((_Tmp2.f1).ReachableFL).tag == 2){if(((_Tmp2.f2).PlaceL).tag == 1){_Tmp5=(((_Tmp2.f1).ReachableFL).val).f1;_Tmp4=(((_Tmp2.f1).ReachableFL).val).f2;_Tmp3=((_Tmp2.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_Tmp5;struct Cyc_List_List*relns=_Tmp4;struct Cyc_CfFlowInfo_Place*p=_Tmp3;
# 671
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 673
struct _tuple16 _stmttmp11=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple16 _Tmp6=_stmttmp11;void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{void*old_rval=_Tmp8;struct Cyc_List_List*names=_Tmp7;
for(1;names != 0;names=names->tl){
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp9->tag=8,_Tmp9->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp9->f2=new_rval;_Tmp9;});}
# 678
fd=Cyc_CfFlowInfo_assign_place(env->fenv,loc,fd,p,new_rval);
inflow=Cyc_CfFlowInfo_ReachableFL(fd,relns);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 684
goto _LL3;}}}else{
# 687
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_exp2string(exp);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp8="noconsume parameter %s must be l-values";_tag_fat(_Tmp8,sizeof(char),40U);}),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL3;}}else{
# 685
goto _LL3;}_LL3:;}
# 690
return inflow;}}
# 695
static struct _tuple17 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 702
union Cyc_CfFlowInfo_FlowInfo _Tmp0=outflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
# 704
return({struct _tuple17 _Tmp3;({union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_BottomFL();_Tmp3.f1=_Tmp4;}),_Tmp3.f2=rval;_Tmp3;});else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict indict=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 706
union Cyc_CfFlowInfo_AbsLVal _Tmp3=lval;void*_Tmp4;if((_Tmp3.PlaceL).tag == 1){_Tmp4=(_Tmp3.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_Tmp4;
# 714
struct Cyc_Dict_Dict outdict=Cyc_CfFlowInfo_assign_place(fenv,loc,indict,p,rval);
relns=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,relns,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
if(Cyc_Flags_warn_lose_unique &&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(_check_null(lexp->topt))){
# 720
struct _tuple16 _stmttmp12=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(indict,p));struct _tuple16 _Tmp5=_stmttmp12;void*_Tmp6;_Tmp6=_Tmp5.f1;{void*rv=_Tmp6;
void*_Tmp7=rv;switch(*((int*)_Tmp7)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp7)->f1 == Cyc_CfFlowInfo_NoneIL)
goto _LL11;else{goto _LL14;}case 7: _LL11:
 goto _LL13;case 0: _LL13:
 goto _LLD;default: _LL14:
# 726
 Cyc_Warn_warn(lexp->loc,({const char*_Tmp8="assignment may overwrite alias-free pointer(s)";_tag_fat(_Tmp8,sizeof(char),47U);}),_tag_fat(0U,sizeof(void*),0));
goto _LLD;}_LLD:;}}
# 731
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple17 _Tmp5;_Tmp5.f1=outflow,_Tmp5.f2=rval;_Tmp5;});}}else{
# 737
return({struct _tuple17 _Tmp5;({union Cyc_CfFlowInfo_FlowInfo _Tmp6=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_Tmp5.f1=_Tmp6;}),_Tmp5.f2=rval;_Tmp5;});};}};}
# 744
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 750
union Cyc_CfFlowInfo_FlowInfo _Tmp0=f;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 755
outdict=({struct Cyc_CfFlowInfo_FlowEnv*_Tmp3=fenv;unsigned _Tmp4=loc;struct Cyc_Dict_Dict _Tmp5=outdict;struct Cyc_CfFlowInfo_Place*_Tmp6=({struct Cyc_CfFlowInfo_Place*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));
({void*_Tmp8=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_Tmp9->tag=0,_Tmp9->f1=vd;_Tmp9;});_Tmp7->root=_Tmp8;}),_Tmp7->path=0;_Tmp7;});
# 755
Cyc_CfFlowInfo_assign_place(_Tmp3,_Tmp4,_Tmp5,_Tmp6,rval);});
# 758
relns=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,relns,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
Cyc_NewControlFlow_update_tryflow(env,outflow);
# 763
return outflow;}}};}struct _tuple25{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 767
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned pat_loc,unsigned exp_loc,int isAliasPat){
# 774
if(vds == 0)return inflow;{
# 777
struct Cyc_List_List*vs=Cyc_Tcutil_filter_nulls((Cyc_List_split(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple25*)x->hd)).f1 == 0)es=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=_check_null((*((struct _tuple25*)x->hd)).f2),_Tmp0->tl=es;_Tmp0;});}}
# 783
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,vs,fenv->unknown_all,pat_loc,name_locs);
# 785
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 789
struct _tuple17 _stmttmp13=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd,0);struct _tuple17 _Tmp0=_stmttmp13;void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp2;void*r=_Tmp1;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,f,r);}}}{
# 797
struct Cyc_List_List*_Tmp0=Cyc_List_rev(vds);struct Cyc_List_List*vds=_Tmp0;
for(1;vds != 0;vds=vds->tl){
struct _tuple25*_stmttmp14=(struct _tuple25*)vds->hd;struct _tuple25*_Tmp1=_stmttmp14;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_Absyn_Vardecl**vd=_Tmp3;struct Cyc_Absyn_Exp*ve=_Tmp2;
if(vd != 0 && ve != 0){
if(ve->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(ve);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({
# 802
(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp6="oops! pattern init expr %s has no type!\n";_tag_fat(_Tmp6,sizeof(char),41U);}),_tag_fat(_Tmp5,sizeof(void*),1));});{
# 811
struct Cyc_List_List l=({struct Cyc_List_List _Tmp4;_Tmp4.hd=ve,_Tmp4.tl=0;_Tmp4;});
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple19 _stmttmp15=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,ve);struct _tuple19 _Tmp4=_stmttmp15;union Cyc_CfFlowInfo_AbsLVal _Tmp5;_Tmp5=_Tmp4.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_Tmp5;
struct _tuple17 _stmttmp16=Cyc_NewControlFlow_anal_Rexp(env,1,f,ve,isAliasPat);struct _tuple17 _Tmp6=_stmttmp16;void*_Tmp7;union Cyc_CfFlowInfo_FlowInfo _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp8;void*rval=_Tmp7;
union Cyc_CfFlowInfo_FlowInfo _Tmp9=f;void*_TmpA;struct Cyc_Dict_Dict _TmpB;if((_Tmp9.ReachableFL).tag == 2){_TmpB=((_Tmp9.ReachableFL).val).f1;_TmpA=((_Tmp9.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_TmpB;struct Cyc_List_List*relns=_TmpA;
# 817
if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _TmpC=lval;void*_TmpD;if((_TmpC.PlaceL).tag == 1){_TmpD=(_TmpC.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_TmpD;
# 820
rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TmpE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_TmpE->tag=8,_TmpE->f1=*vd,_TmpE->f2=rval;_TmpE;});{
# 823
void*new_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TmpE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_TmpE->tag=8,_TmpE->f1=*vd,_TmpE->f2=new_rval;_TmpE;});
fd=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd,relns);
goto _LL11;}}}else{
# 830
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_check_null(ve->topt))&& !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*vd)->type))
({struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({
struct _fat_ptr _Tmp10=Cyc_Absynpp_exp2string(ve);_TmpF.f1=_Tmp10;});_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_CfFlowInfo_aerr(exp_loc,({const char*_Tmp10="aliased pattern expression not an l-value: %s";_tag_fat(_Tmp10,sizeof(char),46U);}),_tag_fat(_TmpF,sizeof(void*),1));});}_LL11:;}
# 840
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,f,*vd,ve,rval,pat_loc);
goto _LLC;}}else{
# 843
goto _LLC;}_LLC:;}}}}}}
# 848
return inflow;}}}
# 851
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 853
void*_stmttmp17=e->r;void*_Tmp0=_stmttmp17;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 4:
 goto _LL4;case 3: _LL4:
 goto _LL6;case 5: _LL6:
 return 1;default: goto _LL11;}case 20: _Tmp1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
_Tmp1=e;goto _LLA;}case 21: _Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_Tmp1;
_Tmp1=e;goto _LLC;}case 22: _Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_Tmp1;
# 860
return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);}case 23: _Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 862
void*_stmttmp18=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp2=_stmttmp18;if(*((int*)_Tmp2)== 6)
return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);else{
return 0;};}case 14: _Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 867
if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);else{
return 0;}}default: _LL11:
 return 0;};}
# 873
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _stmttmp19=*rop;union Cyc_Relations_RelnOp _Tmp0=_stmttmp19;unsigned _Tmp1;switch((_Tmp0.RParamNumelts).tag){case 5: _Tmp1=(_Tmp0.RParam).val;{unsigned i=_Tmp1;
# 876
struct Cyc_Absyn_Exp*e=({(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(exps,(int)i);
return Cyc_Relations_exp2relnop(e,rop);}case 6: _Tmp1=(_Tmp0.RParamNumelts).val;{unsigned i=_Tmp1;
# 879
struct Cyc_Absyn_Exp*e=({(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(exps,(int)i);
return({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Absyn_prim1_exp(18U,e,0U);Cyc_Relations_exp2relnop(_Tmp2,rop);});}default:
 return 1;};}
# 885
static struct _fat_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _Tmp0=rop;unsigned _Tmp1;switch((_Tmp0.RParamNumelts).tag){case 5: _Tmp1=(_Tmp0.RParam).val;{unsigned i=_Tmp1;
# 888
return Cyc_Absynpp_exp2string(({(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(exps,(int)i));}case 6: _Tmp1=(_Tmp0.RParamNumelts).val;{unsigned i=_Tmp1;
# 890
return Cyc_Absynpp_exp2string(({(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(exps,(int)i));}default:
 return Cyc_Relations_rop2string(rop);};}
# 895
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned loc){
# 898
union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return;else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 901
for(1;req != 0;req=req->tl){
struct Cyc_Relations_Reln*reln=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=reln->rop1;
union Cyc_Relations_RelnOp rop2=reln->rop2;
enum Cyc_Relations_Relation r=Cyc_Relations_flip_relation(reln->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1)|| !Cyc_NewControlFlow_subst_param(exps,& rop2))||
 Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,r,rop1,relns))){
struct _fat_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _fat_ptr s2=Cyc_Relations_relation2string(reln->relation);
struct _fat_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s1;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s2;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s3;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=Cyc_Relations_relns2string(relns);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[4];_Tmp7[0]=& _Tmp3,_Tmp7[1]=& _Tmp4,_Tmp7[2]=& _Tmp5,_Tmp7[3]=& _Tmp6;Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp8="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_fat(_Tmp8,sizeof(char),77U);}),_tag_fat(_Tmp7,sizeof(void*),4));});
break;}}
# 916
goto _LL0;}}_LL0:;}struct _tuple26{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple27{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 921
static struct _tuple17 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,int local_alias){
# 925
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 938
{union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return({struct _tuple17 _Tmp3;({union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_BottomFL();_Tmp3.f1=_Tmp4;}),_Tmp3.f2=fenv->unknown_all;_Tmp3;});else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_Tmp2;struct Cyc_List_List*relns2=_Tmp1;
d=d2;relns=relns2;}};}
# 953 "new_control_flow.cyc"
if((copy_ctxt && Cyc_NewControlFlow_is_local_var_rooted_path(e,0))&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(_check_null(e->topt))){
# 974 "new_control_flow.cyc"
struct _tuple19 _stmttmp1A=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple19 _Tmp0=_stmttmp1A;union Cyc_CfFlowInfo_AbsLVal _Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_CfFlowInfo_FlowInfo flval=_Tmp2;union Cyc_CfFlowInfo_AbsLVal lval=_Tmp1;
struct _tuple19 _stmttmp1B=({struct _tuple19 _Tmp3;_Tmp3.f1=flval,_Tmp3.f2=lval;_Tmp3;});struct _tuple19 _Tmp3=_stmttmp1B;void*_Tmp4;void*_Tmp5;struct Cyc_Dict_Dict _Tmp6;if(((_Tmp3.f1).ReachableFL).tag == 2){if(((_Tmp3.f2).PlaceL).tag == 1){_Tmp6=(((_Tmp3.f1).ReachableFL).val).f1;_Tmp5=(((_Tmp3.f1).ReachableFL).val).f2;_Tmp4=((_Tmp3.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_Tmp6;struct Cyc_List_List*r=_Tmp5;struct Cyc_CfFlowInfo_Place*p=_Tmp4;
# 977
void*old_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume)){
Cyc_CfFlowInfo_aerr(e->loc,({const char*_Tmp7="expression attempts to copy a consumed alias-free value";_tag_fat(_Tmp7,sizeof(char),56U);}),_tag_fat(0U,sizeof(void*),0));
return({struct _tuple17 _Tmp7;({union Cyc_CfFlowInfo_FlowInfo _Tmp8=Cyc_CfFlowInfo_BottomFL();_Tmp7.f1=_Tmp8;}),_Tmp7.f2=fenv->unknown_all;_Tmp7;});}else{
# 983
if(needs_unconsume)
# 985
return({struct _tuple17 _Tmp7;_Tmp7.f1=flval,({void*_Tmp8=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,old_rval);_Tmp7.f2=_Tmp8;});_Tmp7;});else{
# 988
void*new_rval=Cyc_CfFlowInfo_make_unique_consumed(fenv,_check_null(e->topt),e,env->iteration_num,old_rval,local_alias);
struct Cyc_Dict_Dict outdict=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,new_rval);
# 1000
return({struct _tuple17 _Tmp7;({union Cyc_CfFlowInfo_FlowInfo _Tmp8=Cyc_CfFlowInfo_ReachableFL(outdict,r);_Tmp7.f1=_Tmp8;}),_Tmp7.f2=old_rval;_Tmp7;});}}}}else{goto _LLB;}}else{_LLB:
# 1004
 goto _LL8;}_LL8:;}}{
# 1007
void*_stmttmp1C=e->r;void*_Tmp0=_stmttmp1C;enum Cyc_Absyn_AggrKind _Tmp1;int _Tmp2;void*_Tmp3;enum Cyc_Absyn_MallocKind _Tmp4;enum Cyc_Absyn_Primop _Tmp5;void*_Tmp6;struct _fat_ptr _Tmp7;void*_Tmp8;switch(*((int*)_Tmp0)){case 14: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4 == Cyc_Absyn_Null_to_NonNull){_Tmp8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;
# 1011
struct _tuple17 _stmttmp1D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp1D;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_TmpB;void*r1=_TmpA;
struct _tuple17 _stmttmp1E=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);struct _tuple17 _TmpC=_stmttmp1E;void*_TmpD;union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_TmpE;void*r2=_TmpD;
return({struct _tuple17 _TmpF;_TmpF.f1=f2,_TmpF.f2=r1;_TmpF;});}}}}else{_Tmp8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;
# 1017
_Tmp8=e1;goto _LL13;}}case 12: _Tmp8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_Tmp8;
_Tmp8=e1;goto _LL15;}case 41: _Tmp8=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL15: {struct Cyc_Absyn_Exp*e1=_Tmp8;
_Tmp8=e1;goto _LL17;}case 13: _Tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_Tmp8;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);}case 2: _Tmp7=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr p=_Tmp7;
# 1023
if(!Cyc_NewControlFlow_strcmp(p,({const char*_Tmp9="print_flow";_tag_fat(_Tmp9,sizeof(char),11U);}))){
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=seg_str;_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_fprintf(Cyc_stderr,({const char*_TmpB="%s: current flow is\n";_tag_fat(_TmpB,sizeof(char),21U);}),_tag_fat(_TmpA,sizeof(void*),1));});
Cyc_CfFlowInfo_print_flow(inflow);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp9="\n";_tag_fat(_Tmp9,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}else{
# 1029
if(!Cyc_NewControlFlow_strcmp(p,({const char*_Tmp9="debug_on";_tag_fat(_Tmp9,sizeof(char),9U);})));else{
# 1034
if(!Cyc_NewControlFlow_strcmp(p,({const char*_Tmp9="debug_off";_tag_fat(_Tmp9,sizeof(char),10U);})));}}
# 1042
return({struct _tuple17 _Tmp9;_Tmp9.f1=inflow,_Tmp9.f2=fenv->zero;_Tmp9;});}case 40:
# 1044
 goto _LL1D;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Wstring_c).tag){case 1: _LL1D:
 goto _LL1F;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Int_c).val).f2 == 0){_LL1F:
 return({struct _tuple17 _Tmp9;_Tmp9.f1=inflow,_Tmp9.f2=fenv->zero;_Tmp9;});}else{
goto _LL23;}case 8: _LL23:
 goto _LL25;case 9: _LL25:
 goto _LL27;default: _LL2B:
# 1053
 goto _LL2D;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 2: _LL27:
# 1050
 return({struct _tuple17 _Tmp9;_Tmp9.f1=inflow,_Tmp9.f2=fenv->notzeroall;_Tmp9;});case 1:
# 1062
 return({struct _tuple17 _Tmp9;_Tmp9.f1=inflow,({void*_TmpA=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_Tmp9.f2=_TmpA;});_Tmp9;});case 4: _Tmp8=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp8;
# 1067
if((int)vd->sc == 0)
return({struct _tuple17 _Tmp9;_Tmp9.f1=inflow,({void*_TmpA=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_Tmp9.f2=_TmpA;});_Tmp9;});
# 1070
_Tmp8=vd;goto _LL3D;}case 3: _Tmp8=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL3D: {struct Cyc_Absyn_Vardecl*vd=_Tmp8;
_Tmp8=vd;goto _LL3F;}case 5: _Tmp8=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL3F: {struct Cyc_Absyn_Vardecl*vd=_Tmp8;
# 1074
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=vd;_Tmp9;});
return({struct _tuple17 _Tmp9;_Tmp9.f1=inflow,({void*_TmpA=({struct Cyc_Dict_Dict _TmpB=d;Cyc_Dict_lookup_other(_TmpB,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_TmpC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));*_TmpC=vdroot;_TmpC;}));});_Tmp9.f2=_TmpA;});_Tmp9;});}default:
# 1678
 goto _LL7B;}case 31: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1 == 0)
# 1052
goto _LL2B;else{_Tmp8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp8;struct Cyc_Absyn_Datatypedecl*tud=_Tmp6;
# 1540
_Tmp8=es;goto _LL69;}}case 42: _LL2D:
# 1054
 goto _LL2F;case 18: _LL2F:
 goto _LL31;case 17: _LL31:
 goto _LL33;case 19: _LL33:
 goto _LL35;case 33: _LL35:
 goto _LL37;case 32: _LL37:
 return({struct _tuple17 _Tmp9;_Tmp9.f1=inflow,_Tmp9.f2=fenv->unknown_all;_Tmp9;});case 3: _Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp5;struct Cyc_List_List*es=_Tmp8;
# 1081
struct _tuple17 _stmttmp1F=Cyc_NewControlFlow_anal_use_ints(env,inflow,es);struct _tuple17 _Tmp9=_stmttmp1F;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;void*r=_TmpA;
return({struct _tuple17 _TmpC;_TmpC.f1=f,_TmpC.f2=r;_TmpC;});}}case 5: _Tmp8=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
# 1085
struct Cyc_List_List arg=({struct Cyc_List_List _Tmp9;_Tmp9.hd=e1,_Tmp9.tl=0;_Tmp9;});
struct _tuple17 _stmttmp20=Cyc_NewControlFlow_anal_use_ints(env,inflow,& arg);struct _tuple17 _Tmp9=_stmttmp20;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp9.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpA;
struct _tuple19 _stmttmp21=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,e1);struct _tuple19 _TmpB=_stmttmp21;union Cyc_CfFlowInfo_AbsLVal _TmpC;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_TmpC;
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type(_check_null(e1->topt));
if(iszt){
# 1092
struct _tuple17 _stmttmp22=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _TmpD=_stmttmp22;void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{union Cyc_CfFlowInfo_FlowInfo g=_TmpF;void*r=_TmpE;
Cyc_NewControlFlow_anal_derefR(env,inflow,g,e1,r,0);}}
# 1095
{struct _tuple26 _stmttmp23=({struct _tuple26 _TmpD;_TmpD.f1=lval,_TmpD.f2=f;_TmpD;});struct _tuple26 _TmpD=_stmttmp23;void*_TmpE;struct Cyc_Dict_Dict _TmpF;void*_Tmp10;if(((_TmpD.f1).PlaceL).tag == 1){if(((_TmpD.f2).ReachableFL).tag == 2){_Tmp10=((_TmpD.f1).PlaceL).val;_TmpF=(((_TmpD.f2).ReachableFL).val).f1;_TmpE=(((_TmpD.f2).ReachableFL).val).f2;{struct Cyc_CfFlowInfo_Place*p=_Tmp10;struct Cyc_Dict_Dict outdict=_TmpF;struct Cyc_List_List*relns=_TmpE;
# 1097
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e1);
f=({struct Cyc_Dict_Dict _Tmp11=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,p,fenv->unknown_all);Cyc_CfFlowInfo_ReachableFL(_Tmp11,relns);});
# 1102
goto _LL92;}}else{goto _LL95;}}else{_LL95:
 goto _LL92;}_LL92:;}
# 1106
if(iszt)
return({struct _tuple17 _TmpD;_TmpD.f1=f,_TmpD.f2=fenv->notzeroall;_TmpD;});else{
return({struct _tuple17 _TmpD;_TmpD.f1=f,_TmpD.f2=fenv->unknown_all;_TmpD;});}}}}case 4: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2 != 0){_Tmp8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*l=_Tmp8;struct Cyc_Absyn_Exp*r=_Tmp6;
# 1111
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr(_check_null(e->topt))){
Cyc_CfFlowInfo_aerr(e->loc,({const char*_Tmp9="cannot track alias-free pointers through multiple assignments";_tag_fat(_Tmp9,sizeof(char),62U);}),_tag_fat(0U,sizeof(void*),0));
return({struct _tuple17 _Tmp9;({union Cyc_CfFlowInfo_FlowInfo _TmpA=Cyc_CfFlowInfo_BottomFL();_Tmp9.f1=_TmpA;}),_Tmp9.f2=fenv->unknown_all;_Tmp9;});}{
# 1115
struct _tuple17 _stmttmp24=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,r,local_alias);struct _tuple17 _Tmp9=_stmttmp24;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo right_out=_TmpB;void*rval=_TmpA;
struct _tuple19 _stmttmp25=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,l);struct _tuple19 _TmpC=_stmttmp25;union Cyc_CfFlowInfo_AbsLVal _TmpD;union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpE;union Cyc_CfFlowInfo_AbsLVal lval=_TmpD;
{union Cyc_CfFlowInfo_FlowInfo _TmpF=f;void*_Tmp10;struct Cyc_Dict_Dict _Tmp11;if((_TmpF.ReachableFL).tag == 2){_Tmp11=((_TmpF.ReachableFL).val).f1;_Tmp10=((_TmpF.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_Tmp11;struct Cyc_List_List*relns=_Tmp10;
# 1119
{union Cyc_CfFlowInfo_AbsLVal _Tmp12=lval;void*_Tmp13;if((_Tmp12.PlaceL).tag == 1){_Tmp13=(_Tmp12.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_Tmp13;
# 1123
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,l);
outdict=Cyc_CfFlowInfo_assign_place(fenv,l->loc,outdict,p,fenv->unknown_all);
# 1126
f=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
# 1130
goto _LLA2;}}else{
# 1133
goto _LLA2;}_LLA2:;}
# 1135
goto _LL9D;}}else{
goto _LL9D;}_LL9D:;}
# 1138
return({struct _tuple17 _TmpF;_TmpF.f1=f,_TmpF.f2=fenv->unknown_all;_TmpF;});}}}}}else{_Tmp8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1141
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr(_check_null(e->topt))){
Cyc_CfFlowInfo_aerr(e->loc,({const char*_Tmp9="cannot track alias-free pointers through multiple assignments";_tag_fat(_Tmp9,sizeof(char),62U);}),_tag_fat(0U,sizeof(void*),0));
return({struct _tuple17 _Tmp9;({union Cyc_CfFlowInfo_FlowInfo _TmpA=Cyc_CfFlowInfo_BottomFL();_Tmp9.f1=_TmpA;}),_Tmp9.f2=fenv->unknown_all;_Tmp9;});}
# 1147
inflow=({struct Cyc_NewControlFlow_AnalEnv*_Tmp9=env;union Cyc_CfFlowInfo_FlowInfo _TmpA=inflow;Cyc_NewControlFlow_expand_unique_places(_Tmp9,_TmpA,({struct Cyc_Absyn_Exp*_TmpB[2];_TmpB[0]=e1,_TmpB[1]=e2;Cyc_List_list(_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),2));}));});{
# 1149
struct _tuple17 _stmttmp26=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);struct _tuple17 _Tmp9=_stmttmp26;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo right_out=_TmpB;void*rval=_TmpA;
struct _tuple19 _stmttmp27=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,e1);struct _tuple19 _TmpC=_stmttmp27;union Cyc_CfFlowInfo_AbsLVal _TmpD;union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_TmpE;union Cyc_CfFlowInfo_AbsLVal lval=_TmpD;
struct _tuple17 _stmttmp28=Cyc_NewControlFlow_do_assign(fenv,env,outflow,e1,lval,e2,rval,e->loc);struct _tuple17 _TmpF=_stmttmp28;void*_Tmp10;union Cyc_CfFlowInfo_FlowInfo _Tmp11;_Tmp11=_TmpF.f1;_Tmp10=_TmpF.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_Tmp11;void*rval=_Tmp10;
# 1155
return({struct _tuple17 _Tmp12;_Tmp12.f1=outflow,_Tmp12.f2=rval;_Tmp12;});}}}}}}case 9: _Tmp8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1158
struct _tuple17 _stmttmp29=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp29;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;void*r=_TmpA;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f,e2,local_alias);}}case 11: _Tmp8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
# 1162
struct _tuple17 _stmttmp2A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp2A;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;void*r=_TmpA;
Cyc_NewControlFlow_use_Rval(env,e1->loc,f,r);
return({struct _tuple17 _TmpC;({union Cyc_CfFlowInfo_FlowInfo _TmpD=Cyc_CfFlowInfo_BottomFL();_TmpC.f1=_TmpD;}),({void*_TmpD=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_TmpC.f2=_TmpD;});_TmpC;});}}case 10: _Tmp8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_List_List*es=_Tmp6;
# 1167
struct Cyc_List_List*orig_es=es;
struct _tuple17 _stmttmp2B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp2B;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_TmpB;void*r1=_TmpA;
# 1170
Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);{
# 1173
struct _tuple24 _stmttmp2C=({struct Cyc_NewControlFlow_AnalEnv*_TmpC=env;union Cyc_CfFlowInfo_FlowInfo _TmpD=f1;Cyc_NewControlFlow_anal_Rexps(_TmpC,_TmpD,Cyc_List_copy(es),1,1);});struct _tuple24 _TmpC=_stmttmp2C;void*_TmpD;union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{union Cyc_CfFlowInfo_FlowInfo fst_outflow=_TmpE;struct Cyc_List_List*rvals=_TmpD;
# 1175
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_NewControlFlow_use_Rval(env,e1->loc,fst_outflow,r1);
# 1177
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*consume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_stmttmp2D=Cyc_Absyn_compress(_check_null(e1->topt));void*_TmpF=_stmttmp2D;void*_Tmp10;if(*((int*)_TmpF)== 3){_Tmp10=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpF)->f1).elt_type;{void*t=_Tmp10;
# 1184
{void*_stmttmp2E=Cyc_Absyn_compress(t);void*_Tmp11=_stmttmp2E;void*_Tmp12;void*_Tmp13;void*_Tmp14;if(*((int*)_Tmp11)== 5){_Tmp14=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp11)->f1).attributes;_Tmp13=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp11)->f1).requires_relns;_Tmp12=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp11)->f1).ensures_relns;{struct Cyc_List_List*atts=_Tmp14;struct Cyc_List_List*req=_Tmp13;struct Cyc_List_List*ens=_Tmp12;
# 1186
requires=req;
ensures=ens;
for(1;atts != 0;atts=atts->tl){
# 1190
void*_stmttmp2F=(void*)atts->hd;void*_Tmp15=_stmttmp2F;int _Tmp16;switch(*((int*)_Tmp15)){case 20: _Tmp16=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_Tmp15)->f1;{int i=_Tmp16;
# 1192
init_params=({struct Cyc_List_List*_Tmp17=_cycalloc(sizeof(struct Cyc_List_List));_Tmp17->hd=(void*)i,_Tmp17->tl=init_params;_Tmp17;});goto _LLC6;}case 21: _Tmp16=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_Tmp15)->f1;{int i=_Tmp16;
# 1194
nolive_unique_params=({struct Cyc_List_List*_Tmp17=_cycalloc(sizeof(struct Cyc_List_List));_Tmp17->hd=(void*)i,_Tmp17->tl=nolive_unique_params;_Tmp17;});
consume_params=({struct Cyc_List_List*_Tmp17=_cycalloc(sizeof(struct Cyc_List_List));_Tmp17->hd=(void*)i,_Tmp17->tl=consume_params;_Tmp17;});
goto _LLC6;}case 22: _Tmp16=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_Tmp15)->f1;{int i=_Tmp16;
# 1198
consume_params=({struct Cyc_List_List*_Tmp17=_cycalloc(sizeof(struct Cyc_List_List));_Tmp17->hd=(void*)i,_Tmp17->tl=consume_params;_Tmp17;});
goto _LLC6;}default:
 goto _LLC6;}_LLC6:;}
# 1202
goto _LLC1;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp15="anal_Rexp: bad function type";_tag_fat(_Tmp15,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));}_LLC1:;}
# 1205
goto _LLBC;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp11="anal_Rexp: bad function type";_tag_fat(_Tmp11,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));}_LLBC:;}
# 1208
{int i=1;for(0;rvals != 0;(rvals=rvals->tl,es=(_check_null(es))->tl,++ i)){
if(({(int(*)(struct Cyc_List_List*,int))Cyc_List_memq;})(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _TmpF=fst_outflow;struct Cyc_Dict_Dict _Tmp10;if((_TmpF.BottomFL).tag == 1)
goto _LLCF;else{_Tmp10=((_TmpF.ReachableFL).val).f1;{struct Cyc_Dict_Dict fst_d=_Tmp10;
# 1213
struct _tuple16 _stmttmp30=Cyc_CfFlowInfo_unname_rval((void*)rvals->hd);struct _tuple16 _Tmp11=_stmttmp30;void*_Tmp12;_Tmp12=_Tmp11.f1;{void*r=_Tmp12;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,fst_d,(void*)rvals->hd)== 0 && !Cyc_CfFlowInfo_is_init_pointer((void*)rvals->hd))
Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)(_check_null(es))->hd)->loc,({const char*_Tmp13="expression may not be initialized";_tag_fat(_Tmp13,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));
{union Cyc_CfFlowInfo_FlowInfo _Tmp13=outflow;void*_Tmp14;struct Cyc_Dict_Dict _Tmp15;if((_Tmp13.BottomFL).tag == 1)
goto _LLD7;else{_Tmp15=((_Tmp13.ReachableFL).val).f1;_Tmp14=((_Tmp13.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_Tmp15;struct Cyc_List_List*relns=_Tmp14;
# 1221
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(fenv,d,(void*)rvals->hd);
{void*_stmttmp31=(void*)rvals->hd;void*_Tmp16=_stmttmp31;void*_Tmp17;switch(*((int*)_Tmp16)){case 4: _Tmp17=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp16)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp17;
# 1224
{void*_stmttmp32=Cyc_Absyn_compress(_check_null(((struct Cyc_Absyn_Exp*)(_check_null(es))->hd)->topt));void*_Tmp18=_stmttmp32;void*_Tmp19;if(*((int*)_Tmp18)== 3){_Tmp19=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp18)->f1).elt_type;{void*t=_Tmp19;
# 1226
ans_d=({struct Cyc_CfFlowInfo_FlowEnv*_Tmp1A=fenv;unsigned _Tmp1B=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct Cyc_Dict_Dict _Tmp1C=ans_d;struct Cyc_CfFlowInfo_Place*_Tmp1D=p;Cyc_CfFlowInfo_assign_place(_Tmp1A,_Tmp1B,_Tmp1C,_Tmp1D,
Cyc_CfFlowInfo_typ_to_absrval(fenv,t,0,fenv->esc_all));});
# 1230
goto _LLE3;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp1A="anal_Rexp:bad type for initialized arg";_tag_fat(_Tmp1A,sizeof(char),39U);}),_tag_fat(0U,sizeof(void*),0));}_LLE3:;}
# 1233
goto _LLDC;}case 5:
# 1235
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp18="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_fat(_Tmp18,sizeof(char),68U);}),_tag_fat(0U,sizeof(void*),0));default:
# 1237
 goto _LLDC;}_LLDC:;}
# 1239
outflow=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);
goto _LLD7;}}_LLD7:;}
# 1242
goto _LLCF;}}}_LLCF:;}else{
# 1245
if(({(int(*)(struct Cyc_List_List*,int))Cyc_List_memq;})(nolive_unique_params,i))
# 1250
outflow=({struct Cyc_NewControlFlow_AnalEnv*_TmpF=env;unsigned _Tmp10=((struct Cyc_Absyn_Exp*)(_check_null(es))->hd)->loc;void*_Tmp11=_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _Tmp12=outflow;Cyc_NewControlFlow_use_nounique_Rval(_TmpF,_Tmp10,_Tmp11,_Tmp12,(void*)rvals->hd);});else{
# 1260
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)(_check_null(es))->hd)->loc,outflow,(void*)rvals->hd);
if(!({(int(*)(struct Cyc_List_List*,int))Cyc_List_memq;})(consume_params,i)&&
 Cyc_Tcutil_is_noalias_pointer(_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),0))
outflow=Cyc_NewControlFlow_restore_noconsume_arg(env,outflow,(struct Cyc_Absyn_Exp*)es->hd,((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)rvals->hd);}}}}
# 1270
if(Cyc_Atts_is_noreturn_fn_type(_check_null(e1->topt)))
return({struct _tuple17 _TmpF;({union Cyc_CfFlowInfo_FlowInfo _Tmp10=Cyc_CfFlowInfo_BottomFL();_TmpF.f1=_Tmp10;}),({void*_Tmp10=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_TmpF.f2=_Tmp10;});_TmpF;});else{
# 1273
return({struct _tuple17 _TmpF;_TmpF.f1=outflow,({void*_Tmp10=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_TmpF.f2=_Tmp10;});_TmpF;});}}}}}case 34: _Tmp4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).mknd;_Tmp8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).rgn;_Tmp6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).elt_type;_Tmp3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).num_elts;_Tmp2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).fat_result;{enum Cyc_Absyn_MallocKind mknd=_Tmp4;struct Cyc_Absyn_Exp*eopt=_Tmp8;void**topt=_Tmp6;struct Cyc_Absyn_Exp*exp=_Tmp3;int isfat=_Tmp2;
# 1276
void*place_val;
if(isfat)place_val=fenv->notzeroall;else{
if((int)mknd == 1)place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*_check_null(topt),0,fenv->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*_check_null(topt),0,fenv->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(eopt != 0){
struct _tuple24 _stmttmp33=({struct Cyc_NewControlFlow_AnalEnv*_Tmp9=env;union Cyc_CfFlowInfo_FlowInfo _TmpA=inflow;Cyc_NewControlFlow_anal_Rexps(_Tmp9,_TmpA,({struct Cyc_Absyn_Exp*_TmpB[2];_TmpB[0]=eopt,_TmpB[1]=exp;Cyc_List_list(_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);});struct _tuple24 _Tmp9=_stmttmp33;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;struct Cyc_List_List*rvals=_TmpA;
outflow=f;}}else{
# 1285
struct _tuple17 _stmttmp34=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);struct _tuple17 _Tmp9=_stmttmp34;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp9.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpA;
outflow=f;}}
# 1290
if(Cyc_Tcutil_is_noalias_pointer(_check_null(e->topt),1))
return({struct _tuple17 _Tmp9;_Tmp9.f1=outflow,({void*_TmpA=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_TmpB->tag=5,_TmpB->f1=place_val;_TmpB;});_Tmp9.f2=_TmpA;});_Tmp9;});else{
# 1294
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_Tmp9->tag=1,_Tmp9->f1=exp,_Tmp9->f2=_check_null(e->topt);_Tmp9;});
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),place->root=root,place->path=0;{
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp9->tag=4,_Tmp9->f1=place;_Tmp9;});
union Cyc_CfFlowInfo_FlowInfo _Tmp9=outflow;void*_TmpA;struct Cyc_Dict_Dict _TmpB;if((_Tmp9.BottomFL).tag == 1)
return({struct _tuple17 _TmpC;_TmpC.f1=outflow,_TmpC.f2=rval;_TmpC;});else{_TmpB=((_Tmp9.ReachableFL).val).f1;_TmpA=((_Tmp9.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_TmpB;struct Cyc_List_List*relns=_TmpA;
# 1300
return({struct _tuple17 _TmpC;({union Cyc_CfFlowInfo_FlowInfo _TmpD=({struct Cyc_Dict_Dict _TmpE=Cyc_Dict_insert(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_TmpE,relns);});_TmpC.f1=_TmpD;}),_TmpC.f2=rval;_TmpC;});}};}}}}case 35: _Tmp8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1305
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1311
struct _tuple24 _stmttmp35=({struct Cyc_NewControlFlow_AnalEnv*_Tmp9=env;union Cyc_CfFlowInfo_FlowInfo _TmpA=inflow;Cyc_NewControlFlow_anal_Rexps(_Tmp9,_TmpA,({struct Cyc_Absyn_Exp*_TmpB[2];_TmpB[0]=e1,_TmpB[1]=e2;Cyc_List_list(_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),2));}),0,0);});struct _tuple24 _Tmp9=_stmttmp35;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;struct Cyc_List_List*rvals=_TmpA;
left_rval=(void*)(_check_null(rvals))->hd;
right_rval=(void*)(_check_null(rvals->tl))->hd;
outflow=f;{
# 1317
void*t_ign1=Cyc_Absyn_void_type;void*t_ign2=Cyc_Absyn_void_type;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
void*_TmpC=right_rval;if(*((int*)_TmpC)== 0)
goto _LLF6;else{
Cyc_CfFlowInfo_aerr(e1->loc,({const char*_TmpD="cannot swap value into zeroterm array not known to be 0";_tag_fat(_TmpD,sizeof(char),56U);}),_tag_fat(0U,sizeof(void*),0));}_LLF6:;}else{
# 1328
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
void*_TmpC=left_rval;if(*((int*)_TmpC)== 0)
goto _LLFB;else{
Cyc_CfFlowInfo_aerr(e2->loc,({const char*_TmpD="cannot swap value into zeroterm array not known to be 0";_tag_fat(_TmpD,sizeof(char),56U);}),_tag_fat(0U,sizeof(void*),0));}_LLFB:;}}{
# 1336
struct _tuple19 _stmttmp36=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e1);struct _tuple19 _TmpC=_stmttmp36;union Cyc_CfFlowInfo_AbsLVal _TmpD;_TmpD=_TmpC.f2;{union Cyc_CfFlowInfo_AbsLVal left_lval=_TmpD;
struct _tuple19 _stmttmp37=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e2);struct _tuple19 _TmpE=_stmttmp37;union Cyc_CfFlowInfo_AbsLVal _TmpF;_TmpF=_TmpE.f2;{union Cyc_CfFlowInfo_AbsLVal right_lval=_TmpF;
{union Cyc_CfFlowInfo_FlowInfo _Tmp10=outflow;void*_Tmp11;struct Cyc_Dict_Dict _Tmp12;if((_Tmp10.ReachableFL).tag == 2){_Tmp12=((_Tmp10.ReachableFL).val).f1;_Tmp11=((_Tmp10.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_Tmp12;struct Cyc_List_List*relns=_Tmp11;
# 1340
{union Cyc_CfFlowInfo_AbsLVal _Tmp13=left_lval;void*_Tmp14;if((_Tmp13.PlaceL).tag == 1){_Tmp14=(_Tmp13.PlaceL).val;{struct Cyc_CfFlowInfo_Place*lp=_Tmp14;
# 1342
outdict=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,lp,right_rval);
goto _LL10B;}}else{
# 1348
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,right_rval)!= 1)
Cyc_CfFlowInfo_aerr(e2->loc,({const char*_Tmp15="expression may not be fully initialized";_tag_fat(_Tmp15,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL10B;}_LL10B:;}
# 1352
{union Cyc_CfFlowInfo_AbsLVal _Tmp13=right_lval;void*_Tmp14;if((_Tmp13.PlaceL).tag == 1){_Tmp14=(_Tmp13.PlaceL).val;{struct Cyc_CfFlowInfo_Place*rp=_Tmp14;
# 1354
outdict=Cyc_CfFlowInfo_assign_place(fenv,e2->loc,outdict,rp,left_rval);
goto _LL110;}}else{
# 1357
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,left_rval)!= 1)
Cyc_CfFlowInfo_aerr(e1->loc,({const char*_Tmp15="expression may not be fully initialized";_tag_fat(_Tmp15,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL110;}_LL110:;}
# 1362
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e1);
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e2);
# 1365
outflow=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL106;}}else{
goto _LL106;}_LL106:;}
# 1372
return({struct _tuple17 _Tmp10;_Tmp10.f1=outflow,_Tmp10.f2=fenv->unknown_all;_Tmp10;});}}}}}}case 16: _Tmp8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*eopt=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1375
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(eopt != 0){
struct _tuple24 _stmttmp38=({struct Cyc_NewControlFlow_AnalEnv*_Tmp9=env;union Cyc_CfFlowInfo_FlowInfo _TmpA=inflow;Cyc_NewControlFlow_anal_Rexps(_Tmp9,_TmpA,({struct Cyc_Absyn_Exp*_TmpB[2];_TmpB[0]=eopt,_TmpB[1]=e2;Cyc_List_list(_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);});struct _tuple24 _Tmp9=_stmttmp38;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;struct Cyc_List_List*rvals=_TmpA;
outflow=f;
place_val=(void*)(_check_null((_check_null(rvals))->tl))->hd;}}else{
# 1382
struct _tuple17 _stmttmp39=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);struct _tuple17 _Tmp9=_stmttmp39;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;void*r=_TmpA;
outflow=f;
place_val=r;}}
# 1387
if(Cyc_Tcutil_is_noalias_pointer(_check_null(e->topt),1))
return({struct _tuple17 _Tmp9;_Tmp9.f1=outflow,({void*_TmpA=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_TmpB->tag=5,_TmpB->f1=place_val;_TmpB;});_Tmp9.f2=_TmpA;});_Tmp9;});else{
# 1391
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_Tmp9->tag=1,_Tmp9->f1=e2,_Tmp9->f2=_check_null(e->topt);_Tmp9;});
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),place->root=root,place->path=0;{
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp9->tag=4,_Tmp9->f1=place;_Tmp9;});
union Cyc_CfFlowInfo_FlowInfo _Tmp9=outflow;void*_TmpA;struct Cyc_Dict_Dict _TmpB;if((_Tmp9.BottomFL).tag == 1)
return({struct _tuple17 _TmpC;_TmpC.f1=outflow,_TmpC.f2=rval;_TmpC;});else{_TmpB=((_Tmp9.ReachableFL).val).f1;_TmpA=((_Tmp9.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_TmpB;struct Cyc_List_List*relns=_TmpA;
# 1397
return({struct _tuple17 _TmpC;({union Cyc_CfFlowInfo_FlowInfo _TmpD=({struct Cyc_Dict_Dict _TmpE=Cyc_Dict_insert(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_TmpE,relns);});_TmpC.f1=_TmpD;}),_TmpC.f2=rval;_TmpC;});}};}}}case 15: _Tmp8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
# 1402
struct _tuple19 _stmttmp3A=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e1);struct _tuple19 _Tmp9=_stmttmp3A;union Cyc_CfFlowInfo_AbsLVal _TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;union Cyc_CfFlowInfo_AbsLVal l=_TmpA;
union Cyc_CfFlowInfo_AbsLVal _TmpC=l;void*_TmpD;if((_TmpC.UnknownL).tag == 2)
return({struct _tuple17 _TmpE;_TmpE.f1=f,_TmpE.f2=fenv->notzeroall;_TmpE;});else{_TmpD=(_TmpC.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_TmpD;
return({struct _tuple17 _TmpE;_TmpE.f1=f,({void*_TmpF=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp10->tag=4,_Tmp10->f1=p;_Tmp10;});_TmpE.f2=_TmpF;});_TmpE;});}};}}case 20: _Tmp8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
# 1409
struct _tuple17 _stmttmp3B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp3B;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;void*r=_TmpA;
return Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,r,0);}}case 21: _Tmp8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct _fat_ptr*field=_Tmp6;
# 1413
struct _tuple17 _stmttmp3C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp3C;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;void*r=_TmpA;
void*e1_type=_check_null(e1->topt);
# 1416
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type))
return({struct _tuple17 _TmpC;_TmpC.f1=f,({void*_TmpD=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_TmpC.f2=_TmpD;});_TmpC;});
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,field,f);{
struct _tuple16 _stmttmp3D=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _TmpC=_stmttmp3D;void*_TmpD;_TmpD=_TmpC.f1;{void*r=_TmpD;
void*_TmpE=r;struct _fat_ptr _TmpF;int _Tmp10;int _Tmp11;if(*((int*)_TmpE)== 6){_Tmp11=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TmpE)->f1).is_union;_Tmp10=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TmpE)->f1).fieldnum;_TmpF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TmpE)->f2;{int is_union=_Tmp11;int fnum=_Tmp10;struct _fat_ptr rdict=_TmpF;
# 1423
int field_no=Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),field);
if((is_union && fnum != -1)&& fnum != field_no)
return({struct _tuple17 _Tmp12;_Tmp12.f1=f,({void*_Tmp13=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),1,fenv->unknown_none);_Tmp12.f2=_Tmp13;});_Tmp12;});
return({struct _tuple17 _Tmp12;_Tmp12.f1=f,_Tmp12.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));_Tmp12;});}}else{
# 1428
Cyc_fprintf(Cyc_stderr,({const char*_Tmp12="the bad rexp is :";_tag_fat(_Tmp12,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_CfFlowInfo_print_absrval(r);
({struct Cyc_String_pa_PrintArg_struct _Tmp12=({struct Cyc_String_pa_PrintArg_struct _Tmp13;_Tmp13.tag=0,({struct _fat_ptr _Tmp14=Cyc_Absynpp_exp2string(e);_Tmp13.f1=_Tmp14;});_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp14="\nanal_Rexp: AggrMember: %s";_tag_fat(_Tmp14,sizeof(char),27U);}),_tag_fat(_Tmp13,sizeof(void*),1));});};}}}}case 38: _Tmp8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct _fat_ptr*field=_Tmp6;
# 1436
struct _tuple17 _stmttmp3E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp3E;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;void*r=_TmpA;
# 1438
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_check_null(e1->topt)))
return({struct _tuple17 _TmpC;_TmpC.f1=f,_TmpC.f2=fenv->unknown_all;_TmpC;});{
struct _tuple16 _stmttmp3F=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _TmpC=_stmttmp3F;void*_TmpD;_TmpD=_TmpC.f1;{void*r=_TmpD;
void*_TmpE=r;struct _fat_ptr _TmpF;int _Tmp10;int _Tmp11;if(*((int*)_TmpE)== 6){_Tmp11=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TmpE)->f1).is_union;_Tmp10=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TmpE)->f1).fieldnum;_TmpF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TmpE)->f2;{int is_union=_Tmp11;int fnum=_Tmp10;struct _fat_ptr rdict=_TmpF;
# 1443
int field_no=Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),field);
if(is_union && fnum != -1){
if(fnum != field_no)
return({struct _tuple17 _Tmp12;_Tmp12.f1=f,_Tmp12.f2=fenv->zero;_Tmp12;});else{
# 1448
return({struct _tuple17 _Tmp12;_Tmp12.f1=f,_Tmp12.f2=fenv->notzeroall;_Tmp12;});}}else{
# 1450
return({struct _tuple17 _Tmp12;_Tmp12.f1=f,_Tmp12.f2=fenv->unknown_all;_Tmp12;});}}}else{
# 1452
({struct Cyc_String_pa_PrintArg_struct _Tmp12=({struct Cyc_String_pa_PrintArg_struct _Tmp13;_Tmp13.tag=0,({struct _fat_ptr _Tmp14=Cyc_Absynpp_exp2string(e);_Tmp13.f1=_Tmp14;});_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp14="anal_Rexp: TagCheck_e: %s";_tag_fat(_Tmp14,sizeof(char),26U);}),_tag_fat(_Tmp13,sizeof(void*),1));});};}}}}case 22: _Tmp8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct _fat_ptr*field=_Tmp6;
# 1456
struct _tuple17 _stmttmp40=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp40;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_TmpB;void*r1=_TmpA;
# 1459
struct _tuple17 _stmttmp41=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);struct _tuple17 _TmpC=_stmttmp41;void*_TmpD;union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_TmpE;void*r2=_TmpD;
# 1462
void*_stmttmp42=Cyc_Absyn_compress(_check_null(e1->topt));void*_TmpF=_stmttmp42;void*_Tmp10;if(*((int*)_TmpF)== 3){_Tmp10=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpF)->f1).elt_type;{void*t2=_Tmp10;
# 1464
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t2))
# 1466
return({struct _tuple17 _Tmp11;_Tmp11.f1=f2,({void*_Tmp12=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_Tmp11.f2=_Tmp12;});_Tmp11;});
# 1468
if(Cyc_Absyn_is_require_union_type(t2))
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,field,f1);{
# 1471
struct _tuple16 _stmttmp43=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple16 _Tmp11=_stmttmp43;void*_Tmp12;_Tmp12=_Tmp11.f1;{void*r2=_Tmp12;
void*_Tmp13=r2;struct _fat_ptr _Tmp14;int _Tmp15;int _Tmp16;if(*((int*)_Tmp13)== 6){_Tmp16=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp13)->f1).is_union;_Tmp15=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp13)->f1).fieldnum;_Tmp14=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp13)->f2;{int is_union=_Tmp16;int fnum=_Tmp15;struct _fat_ptr rdict=_Tmp14;
# 1474
int field_no=Cyc_CfFlowInfo_get_field_index(t2,field);
if((is_union && fnum != -1)&& fnum != field_no)
return({struct _tuple17 _Tmp17;_Tmp17.f1=f2,({void*_Tmp18=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),1,fenv->unknown_none);_Tmp17.f2=_Tmp18;});_Tmp17;});
return({struct _tuple17 _Tmp17;_Tmp17.f1=f2,_Tmp17.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));_Tmp17;});}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp17="anal_Rexp: AggrArrow";_tag_fat(_Tmp17,sizeof(char),21U);}),_tag_fat(0U,sizeof(void*),0));};}}}}else{
# 1480
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp11="anal_Rexp: AggrArrow ptr";_tag_fat(_Tmp11,sizeof(char),25U);}),_tag_fat(0U,sizeof(void*),0));};}}}case 6: _Tmp8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;struct Cyc_Absyn_Exp*e3=_Tmp3;
# 1484
struct _tuple20 _stmttmp44=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _Tmp9=_stmttmp44;union Cyc_CfFlowInfo_FlowInfo _TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpB;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpA;
struct _tuple17 pr2=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);
struct _tuple17 pr3=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e3,local_alias);
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}case 7: _Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1490
struct _tuple20 _stmttmp45=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _Tmp9=_stmttmp45;union Cyc_CfFlowInfo_FlowInfo _TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpB;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpA;
struct _tuple17 _stmttmp46=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);struct _tuple17 _TmpC=_stmttmp46;void*_TmpD;union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpE;void*f2r=_TmpD;
struct _tuple17 pr2=({struct _tuple17 _TmpF;_TmpF.f1=f2t,_TmpF.f2=f2r;_TmpF;});
struct _tuple17 pr3=({struct _tuple17 _TmpF;_TmpF.f1=f1f,_TmpF.f2=fenv->zero;_TmpF;});
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 8: _Tmp8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1497
struct _tuple20 _stmttmp47=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _Tmp9=_stmttmp47;union Cyc_CfFlowInfo_FlowInfo _TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpB;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpA;
struct _tuple17 _stmttmp48=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e2,local_alias);struct _tuple17 _TmpC=_stmttmp48;void*_TmpD;union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpE;void*f2r=_TmpD;
struct _tuple17 pr2=({struct _tuple17 _TmpF;_TmpF.f1=f2t,_TmpF.f2=f2r;_TmpF;});
struct _tuple17 pr3=({struct _tuple17 _TmpF;_TmpF.f1=f1t,_TmpF.f2=fenv->notzeroall;_TmpF;});
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 23: _Tmp8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1504
struct _tuple24 _stmttmp49=({struct Cyc_NewControlFlow_AnalEnv*_Tmp9=env;union Cyc_CfFlowInfo_FlowInfo _TmpA=inflow;Cyc_NewControlFlow_anal_Rexps(_Tmp9,_TmpA,({struct Cyc_Absyn_Exp*_TmpB[2];_TmpB[0]=e1,_TmpB[1]=e2;Cyc_List_list(_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});struct _tuple24 _Tmp9=_stmttmp49;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;struct Cyc_List_List*rvals=_TmpA;
# 1508
union Cyc_CfFlowInfo_FlowInfo f2=f;
{union Cyc_CfFlowInfo_FlowInfo _TmpC=f;void*_TmpD;struct Cyc_Dict_Dict _TmpE;if((_TmpC.ReachableFL).tag == 2){_TmpE=((_TmpC.ReachableFL).val).f1;_TmpD=((_TmpC.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_TmpE;struct Cyc_List_List*relns=_TmpD;
# 1513
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d2,(void*)(_check_null((_check_null(rvals))->tl))->hd)== 0)
Cyc_CfFlowInfo_aerr(e2->loc,({const char*_TmpF="expression may not be initialized";_tag_fat(_TmpF,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*new_relns=Cyc_NewControlFlow_add_subscript_reln(relns,e1,e2);
if(relns != new_relns)
f2=Cyc_CfFlowInfo_ReachableFL(d2,new_relns);
goto _LL166;}}}else{
goto _LL166;}_LL166:;}{
# 1521
void*_stmttmp4A=Cyc_Absyn_compress(_check_null(e1->topt));void*_TmpC=_stmttmp4A;void*_TmpD;switch(*((int*)_TmpC)){case 6: _TmpD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_TmpC)->f1;{struct Cyc_List_List*tqts=_TmpD;
# 1523
struct _tuple16 _stmttmp4B=Cyc_CfFlowInfo_unname_rval((void*)(_check_null(rvals))->hd);struct _tuple16 _TmpE=_stmttmp4B;void*_TmpF;_TmpF=_TmpE.f1;{void*r=_TmpF;
void*_Tmp10=r;struct _fat_ptr _Tmp11;if(*((int*)_Tmp10)== 6){_Tmp11=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp10)->f2;{struct _fat_ptr rdict=_Tmp11;
# 1526
unsigned i=(Cyc_Evexp_eval_const_uint_exp(e2)).f1;
return({struct _tuple17 _Tmp12;_Tmp12.f1=f2,_Tmp12.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),(int)i));_Tmp12;});}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp12="anal_Rexp: Subscript";_tag_fat(_Tmp12,sizeof(char),21U);}),_tag_fat(0U,sizeof(void*),0));};}}case 3:  {
# 1531
struct _tuple17 _stmttmp4C=Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,(void*)(_check_null(rvals))->hd,e2);struct _tuple17 _TmpE=_stmttmp4C;void*_TmpF;union Cyc_CfFlowInfo_FlowInfo _Tmp10;_Tmp10=_TmpE.f1;_TmpF=_TmpE.f2;{union Cyc_CfFlowInfo_FlowInfo f3=_Tmp10;void*r=_TmpF;
union Cyc_CfFlowInfo_FlowInfo _Tmp11=f3;if((_Tmp11.BottomFL).tag == 1)
return({struct _tuple17 _Tmp12;_Tmp12.f1=f3,_Tmp12.f2=r;_Tmp12;});else{
return({struct _tuple17 _Tmp12;_Tmp12.f1=f2,_Tmp12.f2=r;_Tmp12;});};}}default:
# 1536
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpE="anal_Rexp: Subscript -- bad type";_tag_fat(_TmpE,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));};}}}case 24: _Tmp8=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL69: {struct Cyc_List_List*es=_Tmp8;
# 1542
struct _tuple24 _stmttmp4D=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _Tmp9=_stmttmp4D;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;struct Cyc_List_List*rvals=_TmpA;
struct _fat_ptr aggrdict=({unsigned _TmpC=(unsigned)
Cyc_List_length(es);_tag_fat(({void**_TmpD=_cycalloc(_check_times(_TmpC,sizeof(void*)));({{unsigned _TmpE=(unsigned)Cyc_List_length(es);unsigned i;for(i=0;i < _TmpE;++ i){({void*_TmpF=({void*temp=(void*)(_check_null(rvals))->hd;rvals=rvals->tl;temp;});_TmpD[i]=_TmpF;});}}0;});_TmpD;}),sizeof(void*),_TmpC);});
# 1549
return({struct _tuple17 _TmpC;_TmpC.f1=f,({void*_TmpD=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_TmpE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_TmpE->tag=6,(_TmpE->f1).is_union=0,(_TmpE->f1).fieldnum=-1,_TmpE->f2=aggrdict;_TmpE;});_TmpC.f2=_TmpD;});_TmpC;});}}case 30: _Tmp8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*des=_Tmp8;
# 1552
struct Cyc_List_List*fs;
{void*_stmttmp4E=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp9=_stmttmp4E;void*_TmpA;if(*((int*)_Tmp9)== 7){_TmpA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp9)->f2;{struct Cyc_List_List*f=_TmpA;
fs=f;goto _LL185;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpB="anal_Rexp:anon struct has bad type";_tag_fat(_TmpB,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));}_LL185:;}
# 1557
_Tmp8=des;_Tmp1=0U;_Tmp6=fs;goto _LL6D;}case 29: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4)->impl != 0){_Tmp8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4)->kind;_Tmp6=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4)->impl)->fields;_LL6D: {struct Cyc_List_List*des=_Tmp8;enum Cyc_Absyn_AggrKind kind=_Tmp1;struct Cyc_List_List*fs=_Tmp6;
# 1559
void*exp_type=_check_null(e->topt);
struct _tuple24 _stmttmp4F=({struct Cyc_NewControlFlow_AnalEnv*_Tmp9=env;union Cyc_CfFlowInfo_FlowInfo _TmpA=inflow;Cyc_NewControlFlow_anal_Rexps(_Tmp9,_TmpA,({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple27*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Exp*(*)(struct _tuple27*))Cyc_Core_snd;}),des),1,1);});struct _tuple24 _Tmp9=_stmttmp4F;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;struct Cyc_List_List*rvals=_TmpA;
struct _fat_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)kind == 1,fenv->unknown_all);
# 1564
int field_no=-1;
{int i=0;for(0;rvals != 0;(rvals=rvals->tl,des=des->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple27*)(_check_null(des))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_stmttmp50=(void*)ds->hd;void*_TmpC=_stmttmp50;void*_TmpD;if(*((int*)_TmpC)== 0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpE="anal_Rexp:Aggregate_e";_tag_fat(_TmpE,sizeof(char),22U);}),_tag_fat(0U,sizeof(void*),0));else{_TmpD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_TmpC)->f1;{struct _fat_ptr*fld=_TmpD;
# 1571
field_no=Cyc_CfFlowInfo_get_field_index_fs(fs,fld);
*((void**)_check_fat_subscript(aggrdict,sizeof(void*),field_no))=(void*)rvals->hd;
# 1574
if((int)kind == 1){
struct Cyc_Absyn_Exp*e=(*((struct _tuple27*)des->hd)).f2;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,(void*)rvals->hd);
# 1578
Cyc_NewControlFlow_check_union_requires(e->loc,exp_type,fld,f);}}};}}}{
# 1581
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*res;res=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct)),res->tag=6,(res->f1).is_union=(int)kind == 1,(res->f1).fieldnum=field_no,res->f2=aggrdict;
return({struct _tuple17 _TmpC;_TmpC.f1=f,_TmpC.f2=(void*)res;_TmpC;});}}}}else{goto _LL6E;}}else{_LL6E:
# 1585
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_Absynpp_exp2string(e);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpB="anal_Rexp:missing aggrdeclimpl in %s";_tag_fat(_TmpB,sizeof(char),37U);}),_tag_fat(_TmpA,sizeof(void*),1));});}case 26: _Tmp8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp8;
# 1587
struct Cyc_List_List*es=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple27*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Exp*(*)(struct _tuple27*))Cyc_Core_snd;}),dles);
struct _tuple24 _stmttmp51=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _Tmp9=_stmttmp51;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_TmpB;struct Cyc_List_List*rvals=_TmpA;
for(1;rvals != 0;(rvals=rvals->tl,es=es->tl)){
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)(_check_null(es))->hd)->loc,outflow,(void*)rvals->hd);}
# 1592
return({struct _tuple17 _TmpC;_TmpC.f1=outflow,({void*_TmpD=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->notzeroall);_TmpC.f2=_TmpD;});_TmpC;});}}case 27: _Tmp8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp3;int zt=_Tmp2;
# 1596
struct _tuple17 _stmttmp52=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);struct _tuple17 _Tmp9=_stmttmp52;void*_TmpA;union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_TmpB;void*r1=_TmpA;
union Cyc_CfFlowInfo_FlowInfo _TmpC=f1;void*_TmpD;struct Cyc_Dict_Dict _TmpE;if((_TmpC.BottomFL).tag == 1)
return({struct _tuple17 _TmpF;_TmpF.f1=f1,_TmpF.f2=fenv->unknown_all;_TmpF;});else{_TmpE=((_TmpC.ReachableFL).val).f1;_TmpD=((_TmpC.ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_TmpE;struct Cyc_List_List*relns=_TmpD;
# 1600
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d1,r1)== 0)
Cyc_CfFlowInfo_aerr(e1->loc,({const char*_TmpF="expression may not be initialized";_tag_fat(_TmpF,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));{
# 1604
struct Cyc_List_List*new_relns=relns;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(vd);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e1,& n2))
new_relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,3U,n2,relns);
if(relns != new_relns)
f1=Cyc_CfFlowInfo_ReachableFL(d1,new_relns);{
# 1613
void*_TmpF=r1;switch(*((int*)_TmpF)){case 0:
 return({struct _tuple17 _Tmp10;_Tmp10.f1=f1,_Tmp10.f2=fenv->unknown_all;_Tmp10;});case 1:
 goto _LL1A3;case 4: _LL1A3: {
# 1617
struct Cyc_List_List l=({struct Cyc_List_List _Tmp10;_Tmp10.hd=vd,_Tmp10.tl=0;_Tmp10;});
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
# 1620
struct _tuple17 _stmttmp53=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);struct _tuple17 _Tmp10=_stmttmp53;void*_Tmp11;union Cyc_CfFlowInfo_FlowInfo _Tmp12;_Tmp12=_Tmp10.f1;_Tmp11=_Tmp10.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp12;void*r2=_Tmp11;
{union Cyc_CfFlowInfo_FlowInfo _Tmp13=f2;struct Cyc_Dict_Dict _Tmp14;if((_Tmp13.BottomFL).tag == 1)
return({struct _tuple17 _Tmp15;_Tmp15.f1=f2,_Tmp15.f2=fenv->unknown_all;_Tmp15;});else{_Tmp14=((_Tmp13.ReachableFL).val).f1;{struct Cyc_Dict_Dict d2=_Tmp14;
# 1624
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!= 1){
Cyc_CfFlowInfo_aerr(e1->loc,({const char*_Tmp15="expression may not be initialized";_tag_fat(_Tmp15,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));
return({struct _tuple17 _Tmp15;({union Cyc_CfFlowInfo_FlowInfo _Tmp16=Cyc_CfFlowInfo_BottomFL();_Tmp15.f1=_Tmp16;}),_Tmp15.f2=fenv->unknown_all;_Tmp15;});}}};}
# 1629
f1=f2;
goto _LL1A5;}}}default: _LL1A5:
# 1632
 while(1){
struct Cyc_List_List l=({struct Cyc_List_List _Tmp10;_Tmp10.hd=vd,_Tmp10.tl=0;_Tmp10;});
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
struct _tuple17 _stmttmp54=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);struct _tuple17 _Tmp10=_stmttmp54;void*_Tmp11;union Cyc_CfFlowInfo_FlowInfo _Tmp12;_Tmp12=_Tmp10.f1;_Tmp11=_Tmp10.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp12;void*r2=_Tmp11;
{union Cyc_CfFlowInfo_FlowInfo _Tmp13=f2;struct Cyc_Dict_Dict _Tmp14;if((_Tmp13.BottomFL).tag == 1)
goto _LL1B1;else{_Tmp14=((_Tmp13.ReachableFL).val).f1;{struct Cyc_Dict_Dict d2=_Tmp14;
# 1639
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!= 1){
Cyc_CfFlowInfo_aerr(e1->loc,({const char*_Tmp15="expression may not be initialized";_tag_fat(_Tmp15,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));
return({struct _tuple17 _Tmp15;({union Cyc_CfFlowInfo_FlowInfo _Tmp16=Cyc_CfFlowInfo_BottomFL();_Tmp15.f1=_Tmp16;}),_Tmp15.f2=fenv->unknown_all;_Tmp15;});}}}_LL1B1:;}{
# 1644
union Cyc_CfFlowInfo_FlowInfo newflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);
if(Cyc_CfFlowInfo_flow_lessthan_approx(newflow,f1))
break;
f1=newflow;}}}}
# 1649
return({struct _tuple17 _Tmp10;_Tmp10.f1=f1,_Tmp10.f2=fenv->notzeroall;_Tmp10;});};}}}};}}case 28: _Tmp8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*exp=_Tmp8;void*typ=_Tmp6;int iszeroterm=_Tmp2;
# 1655
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_Tmp9->tag=1,_Tmp9->f1=exp,_Tmp9->f2=_check_null(e->topt);_Tmp9;});
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),place->root=root,place->path=0;{
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp9->tag=4,_Tmp9->f1=place;_Tmp9;});
void*place_val;
# 1663
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,typ,0,fenv->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple17 _stmttmp55=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);struct _tuple17 _Tmp9=_stmttmp55;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp9.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpA;
outflow=f;{
union Cyc_CfFlowInfo_FlowInfo _TmpB=outflow;void*_TmpC;struct Cyc_Dict_Dict _TmpD;if((_TmpB.BottomFL).tag == 1)
return({struct _tuple17 _TmpE;_TmpE.f1=outflow,_TmpE.f2=rval;_TmpE;});else{_TmpD=((_TmpB.ReachableFL).val).f1;_TmpC=((_TmpB.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_TmpD;struct Cyc_List_List*relns=_TmpC;
# 1670
return({struct _tuple17 _TmpE;({union Cyc_CfFlowInfo_FlowInfo _TmpF=({struct Cyc_Dict_Dict _Tmp10=Cyc_Dict_insert(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_Tmp10,relns);});_TmpE.f1=_TmpF;}),_TmpE.f2=rval;_TmpE;});}};}}}}}case 37: _Tmp8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp8;
# 1674
struct _tuple18 rval_opt=({struct _tuple18 _Tmp9;_Tmp9.f1=(env->fenv)->unknown_all,_Tmp9.f2=copy_ctxt;_Tmp9;});
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_anal_stmt(env,inflow,s,& rval_opt);
return({struct _tuple17 _Tmp9;_Tmp9.f1=f,_Tmp9.f2=rval_opt.f1;_Tmp9;});}case 36: _LL7B:
# 1679
 goto _LL7D;case 25: _LL7D:
 goto _LL7F;default: _LL7F:
# 1682
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp9="anal_Rexp, unexpected exp form";_tag_fat(_Tmp9,sizeof(char),31U);}),_tag_fat(0U,sizeof(void*),0));};}}
# 1694 "new_control_flow.cyc"
static struct _tuple19 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1703
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
void*_stmttmp56=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp0=_stmttmp56;void*_Tmp1;if(*((int*)_Tmp0)== 3){_Tmp1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).bounds;{void*bd=_Tmp1;
# 1706
union Cyc_CfFlowInfo_FlowInfo _Tmp2=f;void*_Tmp3;struct Cyc_Dict_Dict _Tmp4;if((_Tmp2.BottomFL).tag == 1)
return({struct _tuple19 _Tmp5;_Tmp5.f1=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp6=Cyc_CfFlowInfo_UnknownL();_Tmp5.f2=_Tmp6;});_Tmp5;});else{_Tmp4=((_Tmp2.ReachableFL).val).f1;_Tmp3=((_Tmp2.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_Tmp4;struct Cyc_List_List*relns=_Tmp3;
# 1710
struct _tuple16 _stmttmp57=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _Tmp5=_stmttmp57;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{void*r=_Tmp7;struct Cyc_List_List*names=_Tmp6;
retry: {void*_Tmp8=r;enum Cyc_CfFlowInfo_InitLevel _Tmp9;void*_TmpA;void*_TmpB;switch(*((int*)_Tmp8)){case 8:
# 1713
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpC="named location in anal_derefL";_tag_fat(_TmpC,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));case 1:
# 1715
({void*_TmpC=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_TmpD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct));_TmpD->tag=Cyc_CfFlowInfo_NotZero,_TmpD->f1=relns;_TmpD;});e->annot=_TmpC;});goto _LLD;case 5: _TmpB=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp8)->f1;{void*rv=_TmpB;
# 1718
if(expanded_place_possibly_null)
# 1721
({void*_TmpC=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_TmpD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct));_TmpD->tag=Cyc_CfFlowInfo_UnknownZ,_TmpD->f1=relns;_TmpD;});e->annot=_TmpC;});else{
# 1724
void*_stmttmp58=e->annot;void*_TmpC=_stmttmp58;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_TmpC)->tag == Cyc_CfFlowInfo_UnknownZ){
# 1728
({void*_TmpD=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_TmpE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct));_TmpE->tag=Cyc_CfFlowInfo_UnknownZ,_TmpE->f1=relns;_TmpE;});e->annot=_TmpD;});
goto _LL1E;}else{
# 1731
({void*_TmpD=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_TmpE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct));_TmpE->tag=Cyc_CfFlowInfo_NotZero,_TmpE->f1=relns;_TmpE;});e->annot=_TmpD;});
goto _LL1E;}_LL1E:;}
# 1736
if(Cyc_Tcutil_is_bound_one(bd))return({struct _tuple19 _TmpC;_TmpC.f1=f,({union Cyc_CfFlowInfo_AbsLVal _TmpD=Cyc_CfFlowInfo_UnknownL();_TmpC.f2=_TmpD;});_TmpC;});
goto _LLD;}case 4: _TmpB=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp8)->f1)->root;_TmpA=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp8)->f1)->path;{void*root=_TmpB;struct Cyc_List_List*path2=_TmpA;
# 1740
({void*_TmpC=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_TmpD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct));_TmpD->tag=Cyc_CfFlowInfo_NotZero,_TmpD->f1=relns;_TmpD;});e->annot=_TmpC;});
if(Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple19 _TmpC;_TmpC.f1=f,({union Cyc_CfFlowInfo_AbsLVal _TmpD=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_TmpE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_TmpE->root=root,({struct Cyc_List_List*_TmpF=Cyc_List_append(path2,path);_TmpE->path=_TmpF;});_TmpE;}));_TmpC.f2=_TmpD;});_TmpC;});
goto _LLD;}case 0:
# 1745
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple19 _TmpC;({union Cyc_CfFlowInfo_FlowInfo _TmpD=Cyc_CfFlowInfo_BottomFL();_TmpC.f1=_TmpD;}),({union Cyc_CfFlowInfo_AbsLVal _TmpD=Cyc_CfFlowInfo_UnknownL();_TmpC.f2=_TmpD;});_TmpC;});case 2: _Tmp9=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp8)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp9;
# 1750
if(Cyc_Tcutil_is_bound_one(bd))
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
({void*_TmpC=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_TmpD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct));_TmpD->tag=Cyc_CfFlowInfo_UnknownZ,_TmpD->f1=relns;_TmpD;});e->annot=_TmpC;});
goto _LLD;}case 7: _TmpB=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp8)->f3;{void*r2=_TmpB;
# 1755
if(passthrough_consumes){
r=r2;goto retry;}
# 1758
goto _LL1D;}default: _LL1D:
# 1760
({void*_TmpC=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_TmpD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct));_TmpD->tag=Cyc_CfFlowInfo_UnknownZ,_TmpD->f1=relns;_TmpD;});e->annot=_TmpC;});
goto _LLD;}_LLD:;}
# 1763
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,r)== 0){
struct _tuple16 _stmttmp59=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _Tmp8=_stmttmp59;void*_Tmp9;_Tmp9=_Tmp8.f1;{void*r=_Tmp9;
void*_TmpA=r;if(*((int*)_TmpA)== 7){
# 1767
Cyc_CfFlowInfo_aerr(e->loc,({const char*_TmpB="attempt to dereference an alias-free that has already been copied";_tag_fat(_TmpB,sizeof(char),66U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL26;}else{
# 1770
Cyc_CfFlowInfo_aerr(e->loc,({const char*_TmpB="dereference of possibly uninitialized pointer";_tag_fat(_TmpB,sizeof(char),46U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL26;}_LL26:;}}
# 1774
return({struct _tuple19 _Tmp8;_Tmp8.f1=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp9=Cyc_CfFlowInfo_UnknownL();_Tmp8.f2=_Tmp9;});_Tmp8;});}}};}}else{
# 1776
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp2="left deref of non-pointer-type";_tag_fat(_Tmp2,sizeof(char),31U);}),_tag_fat(0U,sizeof(void*),0));};}
# 1782
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_Tmp0=p;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->root;_Tmp1=_Tmp0->path;{void*proot=_Tmp2;struct Cyc_List_List*ppath=_Tmp1;
path=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct));_Tmp5->tag=1;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=path;_Tmp3;});
return({struct Cyc_CfFlowInfo_Place*_Tmp3=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_Tmp3->root=proot,({struct Cyc_List_List*_Tmp4=Cyc_List_append(ppath,path);_Tmp3->path=_Tmp4;});_Tmp3;});}}
# 1792
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1796
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1)
return({struct _tuple19 _Tmp3;({union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_BottomFL();_Tmp3.f1=_Tmp4;}),({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f2=_Tmp4;});_Tmp3;});else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 1801
d=d2;}};}{
# 1805
void*_stmttmp5A=e->r;void*_Tmp0=_stmttmp5A;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 14: _Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;
_Tmp2=e1;goto _LL9;}case 12: _Tmp2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL9: {struct Cyc_Absyn_Exp*e1=_Tmp2;
_Tmp2=e1;goto _LLB;}case 13: _Tmp2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLB: {struct Cyc_Absyn_Exp*e1=_Tmp2;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e1,path);}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 4: _Tmp2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1811
if((int)vd->sc == 0)
return({struct _tuple19 _Tmp3;_Tmp3.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f2=_Tmp4;});_Tmp3;});
_Tmp2=vd;goto _LLF;}case 3: _Tmp2=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LLF: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL11;}case 5: _Tmp2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL11: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1816
return({struct _tuple19 _Tmp3;_Tmp3.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));({void*_Tmp6=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_Tmp7->tag=0,_Tmp7->f1=vd;_Tmp7;});_Tmp5->root=_Tmp6;}),_Tmp5->path=path;_Tmp5;}));_Tmp3.f2=_Tmp4;});_Tmp3;});}case 1: _Tmp2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1818
return({struct _tuple19 _Tmp3;_Tmp3.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f2=_Tmp4;});_Tmp3;});}default: goto _LL1C;}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 1821
{void*_stmttmp5B=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp3=_stmttmp5B;void*_Tmp4;if(*((int*)_Tmp3)== 3){_Tmp4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1).elt_type;{void*t2=_Tmp4;
# 1823
if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(t2)){
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,f,inflow);
path=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp6=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct));_Tmp7->tag=0,({int _Tmp8=Cyc_CfFlowInfo_get_field_index(t2,f);_Tmp7->f1=_Tmp8;});_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=path;_Tmp5;});}
# 1827
goto _LL1E;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp5="anal_Lexp: AggrArrow ptr";_tag_fat(_Tmp5,sizeof(char),25U);}),_tag_fat(0U,sizeof(void*),0));}_LL1E:;}
# 1830
_Tmp2=e1;goto _LL17;}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_Tmp2;
# 1834
if(Cyc_Tcutil_is_noalias_pointer(_check_null(e1->topt),1)){
# 1836
struct _tuple19 _stmttmp5C=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,e1);struct _tuple19 _Tmp3=_stmttmp5C;union Cyc_CfFlowInfo_AbsLVal _Tmp4;union Cyc_CfFlowInfo_FlowInfo _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp5;union Cyc_CfFlowInfo_AbsLVal lval=_Tmp4;
struct _tuple19 _stmttmp5D=({struct _tuple19 _Tmp6;_Tmp6.f1=f,_Tmp6.f2=lval;_Tmp6;});struct _tuple19 _Tmp6=_stmttmp5D;void*_Tmp7;void*_Tmp8;struct Cyc_Dict_Dict _Tmp9;if(((_Tmp6.f1).ReachableFL).tag == 2){if(((_Tmp6.f2).PlaceL).tag == 1){_Tmp9=(((_Tmp6.f1).ReachableFL).val).f1;_Tmp8=(((_Tmp6.f1).ReachableFL).val).f2;_Tmp7=((_Tmp6.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_Tmp9;struct Cyc_List_List*r=_Tmp8;struct Cyc_CfFlowInfo_Place*p=_Tmp7;
# 1839
void*old_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
struct _tuple16 _stmttmp5E=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple16 _TmpA=_stmttmp5E;void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*rval=_TmpC;struct Cyc_List_List*names=_TmpB;
if(expand_unique){
int possibly_null=1;
void*_TmpD=rval;void*_TmpE;void*_TmpF;switch(*((int*)_TmpD)){case 8: _TmpF=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TmpD)->f1;_TmpE=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TmpD)->f2;{struct Cyc_Absyn_Vardecl*n=_TmpF;void*r=_TmpE;
# 1845
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp10="bad named location in anal_Lexp:deref";_tag_fat(_Tmp10,sizeof(char),38U);}),_tag_fat(0U,sizeof(void*),0));}case 7: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_TmpD)->f3)){case 4:
 goto _LL34;case 5:
# 1849
 goto _LL38;default: goto _LL3B;}case 4: _LL34:
# 1848
 return Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);case 5: _LL38: {
# 1853
struct _tuple19 _stmttmp5F=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);struct _tuple19 _Tmp10=_stmttmp5F;union Cyc_CfFlowInfo_FlowInfo _Tmp11;_Tmp11=_Tmp10.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp11;
return({struct _tuple19 _Tmp12;_Tmp12.f1=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp13=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_Tmp12.f2=_Tmp13;});_Tmp12;});}}case 1:
# 1856
 possibly_null=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1863
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(fenv,fd,rval);
void*leaf=(int)il == 1?fenv->unknown_all: fenv->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_Tmp10->tag=5,({void*_Tmp11=({struct Cyc_CfFlowInfo_FlowEnv*_Tmp12=fenv;void*_Tmp13=Cyc_Tcutil_pointer_elt_type(_check_null(e1->topt));Cyc_CfFlowInfo_typ_to_absrval(_Tmp12,_Tmp13,0,leaf);});_Tmp10->f1=_Tmp11;});_Tmp10;});
for(1;names != 0;names=names->tl){
res=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp10->tag=8,_Tmp10->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp10->f2=res;_Tmp10;});}
fd=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,res);{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_ReachableFL(fd,r);
# 1876
struct _tuple19 _stmttmp60=Cyc_NewControlFlow_anal_derefL(env,outflow,e1,outflow,res,passthrough_consumes,possibly_null,path);struct _tuple19 _Tmp10=_stmttmp60;union Cyc_CfFlowInfo_FlowInfo _Tmp11;_Tmp11=_Tmp10.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp11;
# 1878
return({struct _tuple19 _Tmp12;_Tmp12.f1=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp13=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_Tmp12.f2=_Tmp13;});_Tmp12;});}}}};}else{
# 1882
void*_TmpD=rval;switch(*((int*)_TmpD)){case 7: if(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_TmpD)->f3)== 5)
goto _LL47;else{goto _LL48;}case 5: _LL47: {
# 1887
struct _tuple19 _stmttmp61=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);struct _tuple19 _TmpE=_stmttmp61;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpE.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpF;
return({struct _tuple19 _Tmp10;_Tmp10.f1=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp11=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_Tmp10.f2=_Tmp11;});_Tmp10;});}}default: _LL48:
# 1891
 goto _LL43;}_LL43:;}
# 1894
goto _LL26;}}}else{goto _LL29;}}else{_LL29:
 goto _LL26;}_LL26:;}}{
# 1899
struct _tuple17 _stmttmp62=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);struct _tuple17 _Tmp3=_stmttmp62;void*_Tmp4;union Cyc_CfFlowInfo_FlowInfo _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp5;void*r=_Tmp4;
# 1901
return Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,r,passthrough_consumes,0,path);}}}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1904
void*_stmttmp63=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp3=_stmttmp63;switch(*((int*)_Tmp3)){case 6:  {
# 1906
unsigned fld=(Cyc_Evexp_eval_const_uint_exp(e2)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_Tmp4=env;union Cyc_CfFlowInfo_FlowInfo _Tmp5=inflow;int _Tmp6=expand_unique;int _Tmp7=passthrough_consumes;struct Cyc_Absyn_Exp*_Tmp8=e1;Cyc_NewControlFlow_anal_Lexp_rec(_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));
({void*_TmpA=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct));_TmpB->tag=0,_TmpB->f1=(int)fld;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=path;_Tmp9;}));});}case 3:  {
# 1910
struct _tuple24 _stmttmp64=({struct Cyc_NewControlFlow_AnalEnv*_Tmp4=env;union Cyc_CfFlowInfo_FlowInfo _Tmp5=inflow;Cyc_NewControlFlow_anal_Rexps(_Tmp4,_Tmp5,({struct Cyc_Absyn_Exp*_Tmp6[2];_Tmp6[0]=e1,_Tmp6[1]=e2;Cyc_List_list(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});struct _tuple24 _Tmp4=_stmttmp64;void*_Tmp5;union Cyc_CfFlowInfo_FlowInfo _Tmp6;_Tmp6=_Tmp4.f1;_Tmp5=_Tmp4.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp6;struct Cyc_List_List*rvals=_Tmp5;
union Cyc_CfFlowInfo_FlowInfo f2=f;
{union Cyc_CfFlowInfo_FlowInfo _Tmp7=f;void*_Tmp8;struct Cyc_Dict_Dict _Tmp9;if((_Tmp7.ReachableFL).tag == 2){_Tmp9=((_Tmp7.ReachableFL).val).f1;_Tmp8=((_Tmp7.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_Tmp9;struct Cyc_List_List*relns=_Tmp8;
# 1914
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,(void*)(_check_null((_check_null(rvals))->tl))->hd)== 0)
Cyc_CfFlowInfo_aerr(e2->loc,({const char*_TmpA="expression may not be initialized";_tag_fat(_TmpA,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*new_relns=Cyc_NewControlFlow_add_subscript_reln(relns,e1,e2);
if(relns != new_relns)
f2=Cyc_CfFlowInfo_ReachableFL(d2,new_relns);
goto _LL5A;}}}else{
goto _LL5A;}_LL5A:;}{
# 1923
struct _tuple19 _stmttmp65=Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,(void*)(_check_null(rvals))->hd,passthrough_consumes,0,path);struct _tuple19 _Tmp7=_stmttmp65;union Cyc_CfFlowInfo_AbsLVal _Tmp8;union Cyc_CfFlowInfo_FlowInfo _Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{union Cyc_CfFlowInfo_FlowInfo f3=_Tmp9;union Cyc_CfFlowInfo_AbsLVal r=_Tmp8;
union Cyc_CfFlowInfo_FlowInfo _TmpA=f3;if((_TmpA.BottomFL).tag == 1)
return({struct _tuple19 _TmpB;_TmpB.f1=f3,_TmpB.f2=r;_TmpB;});else{
return({struct _tuple19 _TmpB;_TmpB.f1=f2,_TmpB.f2=r;_TmpB;});};}}}}default:
# 1928
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp4="anal_Lexp: Subscript -- bad type";_tag_fat(_Tmp4,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));};}case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*fld=_Tmp1;
# 1932
void*e1_type=_check_null(e1->topt);
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,fld,inflow);
# 1936
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type))
return({struct _tuple19 _Tmp3;_Tmp3.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f2=_Tmp4;});_Tmp3;});
# 1939
return({struct Cyc_NewControlFlow_AnalEnv*_Tmp3=env;union Cyc_CfFlowInfo_FlowInfo _Tmp4=inflow;int _Tmp5=expand_unique;int _Tmp6=passthrough_consumes;struct Cyc_Absyn_Exp*_Tmp7=e1;Cyc_NewControlFlow_anal_Lexp_rec(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));
({void*_Tmp9=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct));_TmpA->tag=0,({int _TmpB=Cyc_CfFlowInfo_get_field_index(e1_type,fld);_TmpA->f1=_TmpB;});_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=path;_Tmp8;}));});}default: _LL1C:
# 1943
 return({struct _tuple19 _Tmp3;({union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_BottomFL();_Tmp3.f1=_Tmp4;}),({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f2=_Tmp4;});_Tmp3;});};}}
# 1947
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1951
struct _tuple19 _stmttmp66=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple19 _Tmp0=_stmttmp66;union Cyc_CfFlowInfo_AbsLVal _Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp2;union Cyc_CfFlowInfo_AbsLVal r=_Tmp1;
return({struct _tuple19 _Tmp3;_Tmp3.f1=f,_Tmp3.f2=r;_Tmp3;});}}
# 1958
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1961
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 1965
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
# 1968
struct _tuple19 _stmttmp67=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple19 _Tmp0=_stmttmp67;union Cyc_CfFlowInfo_FlowInfo _Tmp1;_Tmp1=_Tmp0.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp1;
inflow=f;}}}}
# 1973
return inflow;}struct _tuple28{enum Cyc_Absyn_Primop f1;void*f2;void*f3;};
# 1979
static struct _tuple20 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1981
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
# 1984
struct _tuple17 _stmttmp68=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)(_check_null(es))->hd,0);struct _tuple17 _Tmp0=_stmttmp68;void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp2;void*r1=_Tmp1;
struct _tuple17 _stmttmp69=Cyc_NewControlFlow_anal_Rexp(env,0,f1,(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd,0);struct _tuple17 _Tmp3=_stmttmp69;void*_Tmp4;union Cyc_CfFlowInfo_FlowInfo _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp5;void*r2=_Tmp4;
union Cyc_CfFlowInfo_FlowInfo f=f2;
# 1989
union Cyc_CfFlowInfo_FlowInfo _Tmp6=f;void*_Tmp7;struct Cyc_Dict_Dict _Tmp8;if((_Tmp6.BottomFL).tag == 1)
return({struct _tuple20 _Tmp9;_Tmp9.f1=f,_Tmp9.f2=f;_Tmp9;});else{_Tmp8=((_Tmp6.ReachableFL).val).f1;_Tmp7=((_Tmp6.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_Tmp8;struct Cyc_List_List*relns=_Tmp7;
# 1992
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r1)== 0 && !Cyc_CfFlowInfo_is_init_pointer(r1))
Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,({const char*_Tmp9="expression may not be initialized";_tag_fat(_Tmp9,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r2)== 0 && !Cyc_CfFlowInfo_is_init_pointer(r1))
Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd)->loc,({const char*_Tmp9="expression may not be initialized";_tag_fat(_Tmp9,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));{
# 1999
union Cyc_CfFlowInfo_FlowInfo ft=f;
union Cyc_CfFlowInfo_FlowInfo ff=f;
# 2004
if((int)p == 5 ||(int)p == 6){
struct _tuple16 _stmttmp6A=Cyc_CfFlowInfo_unname_rval(r1);struct _tuple16 _Tmp9=_stmttmp6A;void*_TmpA;void*_TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{void*r1=_TmpB;struct Cyc_List_List*r1n=_TmpA;
struct _tuple16 _stmttmp6B=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple16 _TmpC=_stmttmp6B;void*_TmpD;void*_TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{void*r2=_TmpE;struct Cyc_List_List*r2n=_TmpD;
struct _tuple0 _stmttmp6C=({struct _tuple0 _TmpF;_TmpF.f1=r1,_TmpF.f2=r2;_TmpF;});struct _tuple0 _TmpF=_stmttmp6C;enum Cyc_CfFlowInfo_InitLevel _Tmp10;switch(*((int*)_TmpF.f1)){case 2: if(*((int*)_TmpF.f2)== 0){_Tmp10=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TmpF.f1)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp10;
# 2009
struct _tuple20 _stmttmp6D=Cyc_NewControlFlow_splitzero(env,inflow,f,e1,il,r1n);struct _tuple20 _Tmp11=_stmttmp6D;union Cyc_CfFlowInfo_FlowInfo _Tmp12;union Cyc_CfFlowInfo_FlowInfo _Tmp13;_Tmp13=_Tmp11.f1;_Tmp12=_Tmp11.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp13;union Cyc_CfFlowInfo_FlowInfo f2=_Tmp12;
{enum Cyc_Absyn_Primop _Tmp14=p;switch((int)_Tmp14){case Cyc_Absyn_Eq:
 ft=f2;ff=f1;goto _LL25;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL25;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp15="anal_test, zero-split";_tag_fat(_Tmp15,sizeof(char),22U);}),_tag_fat(0U,sizeof(void*),0));}_LL25:;}
# 2015
goto _LL11;}}}else{goto _LL20;}case 0: switch(*((int*)_TmpF.f2)){case 2: _Tmp10=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TmpF.f2)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp10;
# 2017
struct _tuple20 _stmttmp6E=Cyc_NewControlFlow_splitzero(env,f2,f,e2,il,r2n);struct _tuple20 _Tmp11=_stmttmp6E;union Cyc_CfFlowInfo_FlowInfo _Tmp12;union Cyc_CfFlowInfo_FlowInfo _Tmp13;_Tmp13=_Tmp11.f1;_Tmp12=_Tmp11.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp13;union Cyc_CfFlowInfo_FlowInfo f2=_Tmp12;
{enum Cyc_Absyn_Primop _Tmp14=p;switch((int)_Tmp14){case Cyc_Absyn_Eq:
 ft=f2;ff=f1;goto _LL2F;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL2F;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp15="anal_test, zero-split";_tag_fat(_Tmp15,sizeof(char),22U);}),_tag_fat(0U,sizeof(void*),0));}_LL2F:;}
# 2023
goto _LL11;}}case 0:
# 2025
 if((int)p == 5)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;case 1:
 goto _LL1B;case 4: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1: if(*((int*)_TmpF.f2)== 0){_LL1D:
 goto _LL1F;}else{goto _LL20;}case 4: if(*((int*)_TmpF.f2)== 0){_LL1F:
# 2032
 if((int)p == 6)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;}else{goto _LL20;}default: _LL20:
 goto _LL11;}_LL11:;}}}
# 2044
{struct _tuple28 _stmttmp6F=({struct _tuple28 _Tmp9;_Tmp9.f1=p,({
void*_TmpA=Cyc_Absyn_compress(_check_null(e1->topt));_Tmp9.f2=_TmpA;}),({
void*_TmpA=Cyc_Absyn_compress(_check_null(e2->topt));_Tmp9.f3=_TmpA;});_Tmp9;});
# 2044
struct _tuple28 _Tmp9=_stmttmp6F;switch((int)_Tmp9.f1){case Cyc_Absyn_Eq:
# 2047
 goto _LL3A;case Cyc_Absyn_Neq: _LL3A:
 goto _LL3C;default: if(*((int*)_Tmp9.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3C:
 goto _LL3E;}else{if(*((int*)_Tmp9.f3)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f3)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{goto _LL43;}case 4: goto _LL41;default: goto _LL43;}else{goto _LL43;}}}else{if(*((int*)_Tmp9.f3)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f3)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f2)->f1)== 4)goto _LL3F;else{goto _LL43;}}}else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f2)->f1)== 4)goto _LL3F;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f3)->f1)== 4)goto _LL41;else{goto _LL43;}}}}else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f2)->f1)== 4){_LL3F: _LL40:
# 2051
 goto _LL42;}else{goto _LL43;}}}}else{if(*((int*)_Tmp9.f3)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f3)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9.f3)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3D: _LL3E:
# 2050
 goto _LL40;}else{goto _LL43;}case 4: _LL41: _LL42:
# 2052
 goto _LL36;default: goto _LL43;}else{_LL43:
 return({struct _tuple20 _TmpA;_TmpA.f1=ft,_TmpA.f2=ff;_TmpA;});}}}_LL36:;}
# 2057
{enum Cyc_Absyn_Primop _Tmp9=p;switch((int)_Tmp9){case Cyc_Absyn_Eq:
 goto _LL49;case Cyc_Absyn_Neq: _LL49: goto _LL4B;case Cyc_Absyn_Gt: _LL4B: goto _LL4D;case Cyc_Absyn_Gte: _LL4D: goto _LL4F;case Cyc_Absyn_Lt: _LL4F: goto _LL51;case Cyc_Absyn_Lte: _LL51: goto _LL45;default:
 return({struct _tuple20 _TmpA;_TmpA.f1=ft,_TmpA.f2=ff;_TmpA;});}_LL45:;}{
# 2062
struct _RegionHandle*r=Cyc_Core_heap_region;
struct _tuple13 _stmttmp70=Cyc_Relations_primop2relation(e1,p,e2);struct _tuple13 _Tmp9=_stmttmp70;void*_TmpA;enum Cyc_Relations_Relation _TmpB;void*_TmpC;_TmpC=_Tmp9.f1;_TmpB=_Tmp9.f2;_TmpA=_Tmp9.f3;{struct Cyc_Absyn_Exp*e1=_TmpC;enum Cyc_Relations_Relation relation=_TmpB;struct Cyc_Absyn_Exp*e2=_TmpA;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2067
if(Cyc_Relations_exp2relnop(e1,& n1)&& Cyc_Relations_exp2relnop(e2,& n2)){
# 2069
struct Cyc_List_List*relns_true=Cyc_Relations_add_relation(r,n1,relation,n2,relns);
# 2073
struct Cyc_List_List*relns_false=({struct _RegionHandle*_TmpD=r;union Cyc_Relations_RelnOp _TmpE=n2;enum Cyc_Relations_Relation _TmpF=Cyc_Relations_flip_relation(relation);union Cyc_Relations_RelnOp _Tmp10=n1;Cyc_Relations_add_relation(_TmpD,_TmpE,_TmpF,_Tmp10,relns);});
struct _tuple20 _stmttmp71=({struct _tuple20 _TmpD;_TmpD.f1=ft,_TmpD.f2=ff;_TmpD;});struct _tuple20 _TmpD=_stmttmp71;struct Cyc_Dict_Dict _TmpE;struct Cyc_Dict_Dict _TmpF;if(((_TmpD.f1).ReachableFL).tag == 2){if(((_TmpD.f2).ReachableFL).tag == 2){_TmpF=(((_TmpD.f1).ReachableFL).val).f1;_TmpE=(((_TmpD.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict dt=_TmpF;struct Cyc_Dict_Dict df=_TmpE;
# 2076
return({struct _tuple20 _Tmp10;({union Cyc_CfFlowInfo_FlowInfo _Tmp11=Cyc_CfFlowInfo_ReachableFL(dt,relns_true);_Tmp10.f1=_Tmp11;}),({union Cyc_CfFlowInfo_FlowInfo _Tmp11=Cyc_CfFlowInfo_ReachableFL(df,relns_false);_Tmp10.f2=_Tmp11;});_Tmp10;});}}else{_TmpF=(((_TmpD.f1).ReachableFL).val).f1;{struct Cyc_Dict_Dict dt=_TmpF;
# 2080
return({struct _tuple20 _Tmp10;({union Cyc_CfFlowInfo_FlowInfo _Tmp11=Cyc_CfFlowInfo_ReachableFL(dt,relns_true);_Tmp10.f1=_Tmp11;}),_Tmp10.f2=ff;_Tmp10;});}}}else{if(((_TmpD.f2).ReachableFL).tag == 2){_TmpF=(((_TmpD.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict df=_TmpF;
# 2078
return({struct _tuple20 _Tmp10;_Tmp10.f1=ft,({union Cyc_CfFlowInfo_FlowInfo _Tmp11=Cyc_CfFlowInfo_ReachableFL(df,relns_false);_Tmp10.f2=_Tmp11;});_Tmp10;});}}else{
# 2082
return({struct _tuple20 _Tmp10;_Tmp10.f1=ft,_Tmp10.f2=ff;_Tmp10;});}};}else{
# 2085
return({struct _tuple20 _TmpD;_TmpD.f1=ft,_TmpD.f2=ff;_TmpD;});}}}}}};}}}
# 2091
static struct _tuple20 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
void*_stmttmp72=e->r;void*_Tmp0=_stmttmp72;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 2095
struct _tuple20 _stmttmp73=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _Tmp5=_stmttmp73;union Cyc_CfFlowInfo_FlowInfo _Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_Tmp7;union Cyc_CfFlowInfo_FlowInfo f1f=_Tmp6;
struct _tuple20 _stmttmp74=Cyc_NewControlFlow_anal_test(env,f1t,e2);struct _tuple20 _Tmp8=_stmttmp74;union Cyc_CfFlowInfo_FlowInfo _Tmp9;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpA;union Cyc_CfFlowInfo_FlowInfo f2f=_Tmp9;
struct _tuple20 _stmttmp75=Cyc_NewControlFlow_anal_test(env,f1f,e3);struct _tuple20 _TmpB=_stmttmp75;union Cyc_CfFlowInfo_FlowInfo _TmpC;union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_FlowInfo f3t=_TmpD;union Cyc_CfFlowInfo_FlowInfo f3f=_TmpC;
return({struct _tuple20 _TmpE;({union Cyc_CfFlowInfo_FlowInfo _TmpF=Cyc_CfFlowInfo_join_flow(fenv,f2t,f3t);_TmpE.f1=_TmpF;}),({
union Cyc_CfFlowInfo_FlowInfo _TmpF=Cyc_CfFlowInfo_join_flow(fenv,f2f,f3f);_TmpE.f2=_TmpF;});_TmpE;});}}}}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 2101
struct _tuple20 _stmttmp76=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _Tmp5=_stmttmp76;union Cyc_CfFlowInfo_FlowInfo _Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_Tmp7;union Cyc_CfFlowInfo_FlowInfo f1f=_Tmp6;
struct _tuple20 _stmttmp77=Cyc_NewControlFlow_anal_test(env,f1t,e2);struct _tuple20 _Tmp8=_stmttmp77;union Cyc_CfFlowInfo_FlowInfo _Tmp9;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpA;union Cyc_CfFlowInfo_FlowInfo f2f=_Tmp9;
return({struct _tuple20 _TmpB;_TmpB.f1=f2t,({union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_CfFlowInfo_join_flow(fenv,f1f,f2f);_TmpB.f2=_TmpC;});_TmpB;});}}}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 2105
struct _tuple20 _stmttmp78=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _Tmp5=_stmttmp78;union Cyc_CfFlowInfo_FlowInfo _Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_Tmp7;union Cyc_CfFlowInfo_FlowInfo f1f=_Tmp6;
struct _tuple20 _stmttmp79=Cyc_NewControlFlow_anal_test(env,f1f,e2);struct _tuple20 _Tmp8=_stmttmp79;union Cyc_CfFlowInfo_FlowInfo _Tmp9;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpA;union Cyc_CfFlowInfo_FlowInfo f2f=_Tmp9;
return({struct _tuple20 _TmpB;({union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_CfFlowInfo_join_flow(fenv,f1t,f2t);_TmpB.f1=_TmpC;}),_TmpB.f2=f2f;_TmpB;});}}}case 9: _Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 2109
struct _tuple17 _stmttmp7A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);struct _tuple17 _Tmp5=_stmttmp7A;void*_Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp7;void*r=_Tmp6;
return Cyc_NewControlFlow_anal_test(env,f,e2);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl == 0){_Tmp4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->hd;{struct Cyc_Absyn_Exp*e1=_Tmp4;
# 2112
struct _tuple20 _stmttmp7B=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _Tmp5=_stmttmp7B;union Cyc_CfFlowInfo_FlowInfo _Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp7;union Cyc_CfFlowInfo_FlowInfo f2=_Tmp6;
return({struct _tuple20 _Tmp8;_Tmp8.f1=f2,_Tmp8.f2=f1;_Tmp8;});}}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_List_List*es=_Tmp4;
# 2115
return Cyc_NewControlFlow_anal_primop_test(env,inflow,p,es);}}default:  {
# 2119
struct _tuple17 _stmttmp7C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0);struct _tuple17 _Tmp5=_stmttmp7C;void*_Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp7;void*r=_Tmp6;
union Cyc_CfFlowInfo_FlowInfo _Tmp8=f;struct Cyc_Dict_Dict _Tmp9;if((_Tmp8.BottomFL).tag == 1)
return({struct _tuple20 _TmpA;_TmpA.f1=f,_TmpA.f2=f;_TmpA;});else{_Tmp9=((_Tmp8.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_Tmp9;
# 2123
struct _tuple16 _stmttmp7D=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _TmpA=_stmttmp7D;void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*r=_TmpC;struct Cyc_List_List*names=_TmpB;
void*_TmpD=r;enum Cyc_CfFlowInfo_InitLevel _TmpE;void*_TmpF;void*_Tmp10;switch(*((int*)_TmpD)){case 8: _Tmp10=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TmpD)->f1;_TmpF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TmpD)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp10;void*r2=_TmpF;
# 2126
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp11="anal_test: bad namedlocation";_tag_fat(_Tmp11,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));}case 0:
 return({struct _tuple20 _Tmp11;({union Cyc_CfFlowInfo_FlowInfo _Tmp12=Cyc_CfFlowInfo_BottomFL();_Tmp11.f1=_Tmp12;}),_Tmp11.f2=f;_Tmp11;});case 1:
 goto _LL3D;case 5: _LL3D:
 goto _LL3F;case 4: _LL3F:
 return({struct _tuple20 _Tmp11;_Tmp11.f1=f,({union Cyc_CfFlowInfo_FlowInfo _Tmp12=Cyc_CfFlowInfo_BottomFL();_Tmp11.f2=_Tmp12;});_Tmp11;});case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TmpD)->f1 == Cyc_CfFlowInfo_NoneIL)
goto _LL43;else{_TmpE=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TmpD)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_TmpE;
# 2136
return Cyc_NewControlFlow_splitzero(env,inflow,f,e,il,names);}}case 3: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_TmpD)->f1 == Cyc_CfFlowInfo_NoneIL){_LL43:
# 2132
 goto _LL45;}else{
# 2137
return({struct _tuple20 _Tmp11;_Tmp11.f1=f,_Tmp11.f2=f;_Tmp11;});}case 7: _LL45:
# 2134
 Cyc_CfFlowInfo_aerr(e->loc,({const char*_Tmp11="expression may not be initialized";_tag_fat(_Tmp11,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));
return({struct _tuple20 _Tmp11;({union Cyc_CfFlowInfo_FlowInfo _Tmp12=Cyc_CfFlowInfo_BottomFL();_Tmp11.f1=_Tmp12;}),({union Cyc_CfFlowInfo_FlowInfo _Tmp12=Cyc_CfFlowInfo_BottomFL();_Tmp11.f2=_Tmp12;});_Tmp11;});default:
# 2138
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp11="anal_test";_tag_fat(_Tmp11,sizeof(char),10U);}),_tag_fat(0U,sizeof(void*),0));};}}};}}};}struct _tuple29{unsigned f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2144
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple29*ckenv,void*root,void*rval){
# 2146
struct _tuple29*_Tmp0=ckenv;struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;unsigned _Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{unsigned loc=_Tmp3;struct Cyc_NewControlFlow_AnalEnv*env=_Tmp2;struct Cyc_Dict_Dict fd=_Tmp1;
void*_Tmp4=root;void*_Tmp5;if(*((int*)_Tmp4)== 0){_Tmp5=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;
# 2149
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
struct _tuple16 _stmttmp7E=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple16 _Tmp6=_stmttmp7E;void*_Tmp7;_Tmp7=_Tmp6.f1;{void*rval=_Tmp7;
void*_Tmp8=rval;switch(*((int*)_Tmp8)){case 7:
 goto _LLF;case 0: _LLF:
 goto _LL11;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp8)->f1 == Cyc_CfFlowInfo_NoneIL){_LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12:
# 2156
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_Absynpp_qvar2string(vd->name);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_warn(loc,({const char*_TmpB="unique pointers reachable from %s may become unreachable";_tag_fat(_TmpB,sizeof(char),57U);}),_tag_fat(_TmpA,sizeof(void*),1));});
goto _LLB;}_LLB:;}}
# 2160
goto _LL3;}}else{
goto _LL3;}_LL3:;}}
# 2165
static void Cyc_NewControlFlow_check_init_params(unsigned loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _Tmp0=flow;struct Cyc_Dict_Dict _Tmp1;if((_Tmp0.BottomFL).tag == 1)
return;else{_Tmp1=((_Tmp0.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_Tmp1;
# 2169
{struct Cyc_List_List*inits=env->param_roots;for(0;inits != 0;inits=inits->tl){
if((int)({struct Cyc_CfFlowInfo_FlowEnv*_Tmp2=env->fenv;struct Cyc_Dict_Dict _Tmp3=d;Cyc_CfFlowInfo_initlevel(_Tmp2,_Tmp3,Cyc_CfFlowInfo_lookup_place(d,(struct Cyc_CfFlowInfo_Place*)inits->hd));})!= 1)
Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp2="function may not initialize all the parameters with attribute 'initializes'";_tag_fat(_Tmp2,sizeof(char),76U);}),_tag_fat(0U,sizeof(void*),0));}}
# 2173
if(Cyc_Flags_warn_lose_unique){
struct _tuple29 check_env=({struct _tuple29 _Tmp2;_Tmp2.f1=loc,_Tmp2.f2=env,_Tmp2.f3=d;_Tmp2;});
({(void(*)(void(*)(struct _tuple29*,void*,void*),struct _tuple29*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_NewControlFlow_check_for_unused_unique,& check_env,d);}
# 2177
return;}};}
# 2186
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2188
struct Cyc_List_List*roots=0;
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple25*_stmttmp7F=(struct _tuple25*)x->hd;struct _tuple25*_Tmp0=_stmttmp7F;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct Cyc_Absyn_Vardecl**vopt=_Tmp2;struct Cyc_Absyn_Exp*eopt=_Tmp1;
if((vopt != 0 && eopt != 0)&&
 Cyc_Tcutil_is_noalias_pointer(_check_null(eopt->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct)),root->tag=0,root->f1=*vopt;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)root,rp->path=0;
roots=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=rp,_Tmp3->tl=roots;_Tmp3;});
es=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=eopt,_Tmp3->tl=es;_Tmp3;});}}}}}
# 2200
return({struct _tuple1 _Tmp0;_Tmp0.f1=roots,_Tmp0.f2=es;_Tmp0;});}struct _tuple30{int f1;void*f2;};
# 2206
static struct _tuple30 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2213
union Cyc_CfFlowInfo_FlowInfo _Tmp0=flow;struct Cyc_Dict_Dict _Tmp1;if((_Tmp0.BottomFL).tag == 1)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp2="noconsume_place_ok: flow became Bottom!";_tag_fat(_Tmp2,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));else{_Tmp1=((_Tmp0.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_Tmp1;
# 2221
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_uint_exp(1U,0U);
int bogus_bool=0;
int bogus_int=1;
void*curr_rval=Cyc_CfFlowInfo_lookup_place(d,place);
void*rval=curr_rval;
# 2233
int varok=0;
{void*_Tmp2=curr_rval;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp2)== 8){_Tmp4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp2)->f1;_Tmp3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp4;void*r=_Tmp3;
# 2236
if(vd == n){
rval=r;
# 2239
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2241
if(Cyc_CfFlowInfo_is_unique_consumed(bogus_exp,bogus_int,rval,& bogus_bool)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
# 2245
struct _fat_ptr _Tmp7=Cyc_Absynpp_qvar2string(vd->name);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp7="function consumes parameter %s which is does not have the 'consume' attribute";_tag_fat(_Tmp7,sizeof(char),78U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}else{
# 2248
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,rval)!= 1 && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
# 2251
struct _fat_ptr _Tmp7=Cyc_Absynpp_qvar2string(vd->name);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp7="function consumes value pointed to by parameter %s, which does not have the 'consume' attribute";_tag_fat(_Tmp7,sizeof(char),96U);}),_tag_fat(_Tmp6,sizeof(void*),1));});else{
# 2253
varok=1;}}}else{
# 2256
varok=1;}}else{
# 2259
goto _LL9;}
goto _LL5;}}else{_LL9:
# 2263
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
# 2268
struct _fat_ptr _Tmp7=Cyc_Absynpp_qvar2string(vd->name);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp7="function parameter %s without 'consume' attribute no longer set to its original value";_tag_fat(_Tmp7,sizeof(char),86U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}
goto _LL5;}_LL5:;}
# 2275
return({struct _tuple30 _Tmp2;_Tmp2.f1=varok,_Tmp2.f2=rval;_Tmp2;});}};}
# 2281
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_Tmp0=p;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->root;_Tmp1=_Tmp0->path;{void*root=_Tmp2;struct Cyc_List_List*fs=_Tmp1;
if(fs != 0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp3="unconsume_params: param to unconsume is non-variable";_tag_fat(_Tmp3,sizeof(char),53U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_Absyn_Vardecl*vd;
void*_Tmp3=root;void*_Tmp4;if(*((int*)_Tmp3)== 0){_Tmp4=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return vd;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp5="unconsume_params: root is not a varroot";_tag_fat(_Tmp5,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));};}}}
# 2300 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2308
{union Cyc_CfFlowInfo_FlowInfo _Tmp0=flow;struct Cyc_Dict_Dict _Tmp1;if((_Tmp0.BottomFL).tag == 1)
return flow;else{_Tmp1=((_Tmp0.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_Tmp1;
# 2315
struct _tuple19 _stmttmp80=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple19 _Tmp2=_stmttmp80;union Cyc_CfFlowInfo_AbsLVal _Tmp3;union Cyc_CfFlowInfo_FlowInfo _Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp4;union Cyc_CfFlowInfo_AbsLVal lval=_Tmp3;
# 2318
{union Cyc_CfFlowInfo_AbsLVal _Tmp5=lval;void*_Tmp6;if((_Tmp5.PlaceL).tag == 1){_Tmp6=(_Tmp5.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_Tmp6;
# 2322
void*old_rval=Cyc_CfFlowInfo_lookup_place(d,p);
{void*_Tmp7=old_rval;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp7)== 8){_Tmp9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp7)->f1;_Tmp8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Vardecl*old_vd=_Tmp9;void*r=_Tmp8;
# 2328
void*new_rval;
if(old_vd == vd){
# 2332
if(varok){
# 2334
old_rval=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,r);
# 2339
if(ropt != 0){
# 2345
struct _tuple17 _stmttmp81=({
struct Cyc_CfFlowInfo_FlowEnv*_TmpA=env->fenv;struct _tuple17 _TmpB=({struct _tuple17 _TmpC;_TmpC.f1=f,_TmpC.f2=old_rval;_TmpC;});Cyc_CfFlowInfo_join_flow_and_rval(_TmpA,_TmpB,({struct _tuple17 _TmpC;_TmpC.f1=f,_TmpC.f2=ropt;_TmpC;}));});
# 2345
struct _tuple17 _TmpA=_stmttmp81;void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_TmpC;void*new_rval2=_TmpB;
# 2349
f=f2;new_rval=new_rval2;}}else{
# 2354
new_rval=old_rval;}}else{
# 2357
new_rval=r;}
# 2359
{union Cyc_CfFlowInfo_FlowInfo _TmpA=f;void*_TmpB;struct Cyc_Dict_Dict _TmpC;if((_TmpA.ReachableFL).tag == 2){_TmpC=((_TmpA.ReachableFL).val).f1;_TmpB=((_TmpA.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_TmpC;struct Cyc_List_List*relns=_TmpB;
# 2361
flow=({struct Cyc_Dict_Dict _TmpD=Cyc_CfFlowInfo_assign_place(env->fenv,loc,d2,p,new_rval);Cyc_CfFlowInfo_ReachableFL(_TmpD,relns);});
# 2365
goto _LL15;}}else{
# 2367
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpD="unconsume_params: joined flow became bot!";_tag_fat(_TmpD,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));}_LL15:;}
# 2369
goto _LLD;}else{
# 2371
goto _LL11;}
goto _LLD;}}else{_LL11:
# 2378
 if(ropt != 0 && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_Absynpp_exp2string(unconsume_exp);_TmpB.f1=_TmpC;});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_CfFlowInfo_aerr(loc,({const char*_TmpC="aliased expression %s was overwritten";_tag_fat(_TmpC,sizeof(char),38U);}),_tag_fat(_TmpB,sizeof(void*),1));});
# 2395 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2397
goto _LL8;}}else{
# 2403
goto _LL8;}_LL8:;}
# 2405
goto _LL0;}}}_LL0:;}
# 2407
return flow;}
# 2420 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2426
{union Cyc_CfFlowInfo_FlowInfo _Tmp0=flow;if((_Tmp0.BottomFL).tag == 1)
return flow;else{
goto _LL0;}_LL0:;}{
# 2430
int do_unconsume=unconsume_exps != 0;
struct Cyc_List_List*_Tmp0=Cyc_List_rev(consumed_vals);struct Cyc_List_List*consumed_vals=_Tmp0;
struct Cyc_List_List*_Tmp1=Cyc_List_rev(unconsume_exps);struct Cyc_List_List*unconsume_exps=_Tmp1;
{struct Cyc_List_List*params=consumed_vals;for(0;params != 0;(
params=params->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2439
struct Cyc_Absyn_Vardecl*vd=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)params->hd);
struct _tuple30 _stmttmp82=
is_region_open?({struct _tuple30 _Tmp2;_Tmp2.f1=1,_Tmp2.f2=0;_Tmp2;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)params->hd,do_unconsume,vd,flow,loc);
# 2440
struct _tuple30 _Tmp2=_stmttmp82;void*_Tmp3;int _Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{int varok=_Tmp4;void*rval=_Tmp3;
# 2446
if(do_unconsume)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)(_check_null(unconsume_exps))->hd,vd,varok,rval,flow,loc);}}}
# 2449
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;}}struct _tuple31{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2453
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned exp_loc){
# 2455
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_stmttmp83=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_Tmp0=_stmttmp83;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp0->pat_vars;_Tmp3=_Tmp0->where_clause;_Tmp2=_Tmp0->body;_Tmp1=_Tmp0->loc;{struct Cyc_Core_Opt*vds_opt=_Tmp4;struct Cyc_Absyn_Exp*where_opt=_Tmp3;struct Cyc_Absyn_Stmt*body=_Tmp2;unsigned loc=_Tmp1;
struct _tuple1 _stmttmp84=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)(_check_null(vds_opt))->v);struct _tuple1 _Tmp5=_stmttmp84;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{struct Cyc_List_List*roots=_Tmp7;struct Cyc_List_List*es=_Tmp6;
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)vds_opt->v,roots != 0,loc,exp_loc,0);
# 2463
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*old_unique_pat_vars=env->unique_pat_vars;
# 2466
if(Cyc_Tcpat_has_vars(vds_opt))
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple31*_TmpA=({struct _tuple31*_TmpB=_cycalloc(sizeof(struct _tuple31));_TmpB->f1=0,_TmpB->f2=body,_TmpB->f3=roots,_TmpB->f4=es;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=old_unique_pat_vars;_Tmp9;});
# 2467
env->unique_pat_vars=_Tmp8;});
# 2470
if(where_opt != 0){
struct Cyc_Absyn_Exp*wexp=where_opt;
struct _tuple20 _stmttmp85=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple20 _Tmp8=_stmttmp85;union Cyc_CfFlowInfo_FlowInfo _Tmp9;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{union Cyc_CfFlowInfo_FlowInfo ft=_TmpA;union Cyc_CfFlowInfo_FlowInfo ff=_Tmp9;
inflow=ff;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,ft,body,0);}}else{
# 2476
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,body,0);}
# 2478
env->unique_pat_vars=old_unique_pat_vars;{
union Cyc_CfFlowInfo_FlowInfo _Tmp8=clause_outflow;if((_Tmp8.BottomFL).tag == 1)
goto _LL9;else{
# 2483
clause_outflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,clause_outflow,loc);
# 2485
if(scs->tl == 0)
return clause_outflow;else{
# 2490
if((struct Cyc_List_List*)(_check_null(((struct Cyc_Absyn_Switch_clause*)(_check_null(scs->tl))->hd)->pat_vars))->v != 0)
Cyc_CfFlowInfo_aerr(body->loc,({const char*_Tmp9="switch clause may implicitly fallthru";_tag_fat(_Tmp9,sizeof(char),38U);}),_tag_fat(0U,sizeof(void*),0));else{
# 2493
Cyc_Warn_warn(body->loc,({const char*_Tmp9="switch clause may implicitly fallthru";_tag_fat(_Tmp9,sizeof(char),38U);}),_tag_fat(0U,sizeof(void*),0));}
# 2495
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)(_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2497
goto _LL9;}_LL9:;}}}}
# 2500
return Cyc_CfFlowInfo_BottomFL();}struct _tuple32{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned f3;};
# 2505
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple32*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2507
struct _tuple32*_Tmp0=vdenv;unsigned _Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{struct Cyc_NewControlFlow_AnalEnv*env=_Tmp3;struct Cyc_Dict_Dict fd=_Tmp2;unsigned loc=_Tmp1;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2510
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=vd;_Tmp4;});
struct _tuple16 _stmttmp86=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _Tmp4=fd;Cyc_Dict_lookup_other(_Tmp4,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));*_Tmp5=vdroot;_Tmp5;}));}));struct _tuple16 _Tmp4=_stmttmp86;void*_Tmp5;_Tmp5=_Tmp4.f1;{void*rval=_Tmp5;
void*_Tmp6=rval;switch(*((int*)_Tmp6)){case 7:
 goto _LLA;case 0: _LLA:
 goto _LLC;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp6)->f1 == Cyc_CfFlowInfo_NoneIL){_LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD:
# 2517
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_qvar2string(vd->name);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_warn(loc,({const char*_Tmp9="unique pointers may still exist after variable %s goes out of scope";_tag_fat(_Tmp9,sizeof(char),68U);}),_tag_fat(_Tmp8,sizeof(void*),1));});
# 2519
goto _LL6;}_LL6:;}}}}
# 2524
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;struct Cyc_Dict_Dict _Tmp1;if((_Tmp0.ReachableFL).tag == 2){_Tmp1=((_Tmp0.ReachableFL).val).f1;{struct Cyc_Dict_Dict fd=_Tmp1;
# 2527
struct _tuple32 vdenv=({struct _tuple32 _Tmp2;_Tmp2.f1=env,_Tmp2.f2=fd,_Tmp2.f3=decl->loc;_Tmp2;});
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
{void*_stmttmp87=decl->r;void*_Tmp2=_stmttmp87;void*_Tmp3;switch(*((int*)_Tmp2)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 2531
Cyc_NewControlFlow_check_dropped_unique_vd(& vdenv,vd);
goto _LL5;}case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp2)->f2 != 0){_Tmp3=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp2)->f2)->v;{struct Cyc_List_List*vds=_Tmp3;
# 2534
struct _tuple1 _stmttmp88=Cyc_List_split(vds);struct _tuple1 _Tmp4=_stmttmp88;void*_Tmp5;_Tmp5=_Tmp4.f1;{struct Cyc_List_List*vs=_Tmp5;
struct Cyc_List_List*_Tmp6=Cyc_Tcutil_filter_nulls(vs);{struct Cyc_List_List*vs=_Tmp6;
_Tmp3=vs;goto _LLB;}}}}else{goto _LLC;}case 3: _Tmp3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;_LLB: {struct Cyc_List_List*vds=_Tmp3;
# 2538
({(void(*)(void(*)(struct _tuple32*,struct Cyc_Absyn_Vardecl*),struct _tuple32*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_NewControlFlow_check_dropped_unique_vd,& vdenv,vds);
goto _LL5;}default: _LLC:
 goto _LL5;}_LL5:;}
# 2542
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 2545
return;}
# 2551
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2555
int num_pop=({(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple31*_stmttmp89=(struct _tuple31*)(_check_null(x))->hd;struct _tuple31*_Tmp0=_stmttmp89;void*_Tmp1;void*_Tmp2;void*_Tmp3;int _Tmp4;_Tmp4=_Tmp0->f1;_Tmp3=_Tmp0->f2;_Tmp2=_Tmp0->f3;_Tmp1=_Tmp0->f4;{int is_open=_Tmp4;struct Cyc_Absyn_Stmt*pat_stmt=_Tmp3;struct Cyc_List_List*roots=_Tmp2;struct Cyc_List_List*es=_Tmp1;
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,is_open,inflow,dest->loc);}}}
# 2560
return inflow;}
# 2566
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple18*rval_opt){
# 2568
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple21 _stmttmp8A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple21 _Tmp0=_stmttmp8A;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_NewControlFlow_CFStmtAnnot*annot=_Tmp2;union Cyc_CfFlowInfo_FlowInfo*sflow=_Tmp1;
inflow=*sflow;{
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
# 2575
void*_stmttmp8B=s->r;void*_Tmp3=_stmttmp8B;unsigned _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp3)){case 0:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1 == 0){
# 2579
if(env->noreturn)
Cyc_CfFlowInfo_aerr(s->loc,({const char*_TmpB="`noreturn' function might return";_tag_fat(_TmpB,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_TmpA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 2585
if(env->noreturn)
Cyc_CfFlowInfo_aerr(s->loc,({const char*_TmpB="`noreturn' function might return";_tag_fat(_TmpB,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));{
struct _tuple17 _stmttmp8C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_check_null(e),0);struct _tuple17 _TmpB=_stmttmp8C;void*_TmpC;union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpD;void*r=_TmpC;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,r);
Cyc_NewControlFlow_check_init_params(s->loc,env,f);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,f,s->loc);
return Cyc_CfFlowInfo_BottomFL();}}}}case 1: _TmpA=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 2594
struct _tuple18*_TmpB=rval_opt;int _TmpC;void*_TmpD;if(_TmpB != 0){_TmpD=(void**)& _TmpB->f1;_TmpC=_TmpB->f2;{void**rval=(void**)_TmpD;int copy_ctxt=_TmpC;
# 2596
struct _tuple17 _stmttmp8D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e,0);struct _tuple17 _TmpE=_stmttmp8D;void*_TmpF;union Cyc_CfFlowInfo_FlowInfo _Tmp10;_Tmp10=_TmpE.f1;_TmpF=_TmpE.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp10;void*r=_TmpF;
*rval=r;
return f;}}}else{
# 2600
return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0)).f1;};}case 2: _TmpA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Stmt*s1=_TmpA;struct Cyc_Absyn_Stmt*s2=_Tmp9;
# 2604
return({struct Cyc_NewControlFlow_AnalEnv*_TmpB=env;union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);struct Cyc_Absyn_Stmt*_TmpD=s2;Cyc_NewControlFlow_anal_stmt(_TmpB,_TmpC,_TmpD,rval_opt);});}case 4: _TmpA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;_Tmp8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3;{struct Cyc_Absyn_Exp*e=_TmpA;struct Cyc_Absyn_Stmt*s1=_Tmp9;struct Cyc_Absyn_Stmt*s2=_Tmp8;
# 2607
struct _tuple20 _stmttmp8E=Cyc_NewControlFlow_anal_test(env,inflow,e);struct _tuple20 _TmpB=_stmttmp8E;union Cyc_CfFlowInfo_FlowInfo _TmpC;union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpD;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpC;
# 2614
union Cyc_CfFlowInfo_FlowInfo ff=Cyc_NewControlFlow_anal_stmt(env,f1f,s2,0);
union Cyc_CfFlowInfo_FlowInfo ft=Cyc_NewControlFlow_anal_stmt(env,f1t,s1,0);
return Cyc_CfFlowInfo_join_flow(fenv,ft,ff);}}case 5: _TmpA=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1).f1;_Tmp9=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1).f2;_Tmp8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Exp*e=_TmpA;struct Cyc_Absyn_Stmt*cont=_Tmp9;struct Cyc_Absyn_Stmt*body=_Tmp8;
# 2622
struct _tuple21 _stmttmp8F=Cyc_NewControlFlow_pre_stmt_check(env,inflow,cont);struct _tuple21 _TmpB=_stmttmp8F;void*_TmpC;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_TmpC;
union Cyc_CfFlowInfo_FlowInfo e_inflow=*eflow_ptr;
struct _tuple20 _stmttmp90=Cyc_NewControlFlow_anal_test(env,e_inflow,e);struct _tuple20 _TmpD=_stmttmp90;union Cyc_CfFlowInfo_FlowInfo _TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpF;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpE;
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,f1t,body,0);
Cyc_NewControlFlow_update_flow(env,cont,body_outflow);
return f1f;}}}case 14: _TmpA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2).f1;_Tmp8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2).f2;{struct Cyc_Absyn_Stmt*body=_TmpA;struct Cyc_Absyn_Exp*e=_Tmp9;struct Cyc_Absyn_Stmt*cont=_Tmp8;
# 2632
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,body,0);
struct _tuple21 _stmttmp91=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);struct _tuple21 _TmpB=_stmttmp91;void*_TmpC;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_TmpC;
union Cyc_CfFlowInfo_FlowInfo e_inflow=*eflow_ptr;
struct _tuple20 _stmttmp92=Cyc_NewControlFlow_anal_test(env,e_inflow,e);struct _tuple20 _TmpD=_stmttmp92;union Cyc_CfFlowInfo_FlowInfo _TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpF;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpE;
Cyc_NewControlFlow_update_flow(env,body,f1t);
return f1f;}}}case 9: _TmpA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2).f1;_Tmp8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2).f2;_Tmp7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3).f1;_Tmp6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3).f2;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp9;struct Cyc_Absyn_Stmt*guard=_Tmp8;struct Cyc_Absyn_Exp*e3=_Tmp7;struct Cyc_Absyn_Stmt*cont=_Tmp6;struct Cyc_Absyn_Stmt*body=_Tmp5;
# 2641
union Cyc_CfFlowInfo_FlowInfo e1_outflow=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0)).f1;
struct _tuple21 _stmttmp93=Cyc_NewControlFlow_pre_stmt_check(env,e1_outflow,guard);struct _tuple21 _TmpB=_stmttmp93;void*_TmpC;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_FlowInfo*e2flow_ptr=_TmpC;
union Cyc_CfFlowInfo_FlowInfo e2_inflow=*e2flow_ptr;
struct _tuple20 _stmttmp94=Cyc_NewControlFlow_anal_test(env,e2_inflow,e2);struct _tuple20 _TmpD=_stmttmp94;union Cyc_CfFlowInfo_FlowInfo _TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpF;union Cyc_CfFlowInfo_FlowInfo f2f=_TmpE;
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,f2t,body,0);
struct _tuple21 _stmttmp95=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);struct _tuple21 _Tmp10=_stmttmp95;void*_Tmp11;_Tmp11=_Tmp10.f2;{union Cyc_CfFlowInfo_FlowInfo*e3flow_ptr=_Tmp11;
union Cyc_CfFlowInfo_FlowInfo e3_inflow=*e3flow_ptr;
union Cyc_CfFlowInfo_FlowInfo e3_outflow=(Cyc_NewControlFlow_anal_Rexp(env,0,e3_inflow,e3,0)).f1;
Cyc_NewControlFlow_update_flow(env,guard,e3_outflow);
return f2f;}}}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2 != 0){_TmpA=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_List_List*es=_TmpA;struct Cyc_Absyn_Switch_clause*destclause=_Tmp9;
# 2653
struct _tuple24 _stmttmp96=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _TmpB=_stmttmp96;void*_TmpC;union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_FlowInfo f=_TmpD;struct Cyc_List_List*rvals=_TmpC;
# 2655
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,destclause->body);{
# 2657
struct Cyc_List_List*vds=Cyc_Tcutil_filter_nulls((Cyc_List_split((struct Cyc_List_List*)(_check_null(destclause->pat_vars))->v)).f1);
f=Cyc_NewControlFlow_add_vars(fenv,f,vds,fenv->unknown_all,s->loc,0);
# 2660
{struct Cyc_List_List*x=(struct Cyc_List_List*)(_check_null(destclause->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple25*_stmttmp97=(struct _tuple25*)x->hd;struct _tuple25*_TmpE=_stmttmp97;void*_TmpF;void*_Tmp10;_Tmp10=_TmpE->f1;_TmpF=_TmpE->f2;{struct Cyc_Absyn_Vardecl**vd=_Tmp10;struct Cyc_Absyn_Exp*ve=_TmpF;
if(vd != 0){
f=({struct Cyc_CfFlowInfo_FlowEnv*_Tmp11=fenv;struct Cyc_NewControlFlow_AnalEnv*_Tmp12=env;union Cyc_CfFlowInfo_FlowInfo _Tmp13=f;struct Cyc_Absyn_Vardecl*_Tmp14=*vd;struct Cyc_Absyn_Exp*_Tmp15=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;void*_Tmp16=(void*)(_check_null(rvals))->hd;Cyc_NewControlFlow_do_initialize_var(_Tmp11,_Tmp12,_Tmp13,_Tmp14,_Tmp15,_Tmp16,s->loc);});
rvals=rvals->tl;
es=es->tl;}}}}
# 2668
Cyc_NewControlFlow_update_flow(env,destclause->body,f);
return Cyc_CfFlowInfo_BottomFL();}}}}else{
# 2839
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpB="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_fat(_TmpB,sizeof(char),54U);}),_tag_fat(0U,sizeof(void*),0));}case 6:
# 2674
 if(({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _TmpB=inflow;if((_TmpB.ReachableFL).tag == 2){
# 2677
if(!Cyc_Tcutil_is_void_type(env->return_type)){
if(Cyc_Tcutil_is_any_float_type(env->return_type)||
 Cyc_Tcutil_is_any_int_type(env->return_type))
Cyc_Warn_warn(s->loc,({const char*_TmpC="break may cause function not to return a value";_tag_fat(_TmpC,sizeof(char),47U);}),_tag_fat(0U,sizeof(void*),0));else{
# 2682
Cyc_CfFlowInfo_aerr(s->loc,({const char*_TmpC="break may cause function not to return a value";_tag_fat(_TmpC,sizeof(char),47U);}),_tag_fat(0U,sizeof(void*),0));}}
# 2684
goto _LL55;}else{
goto _LL55;}_LL55:;}
# 2687
if(env->noreturn)
Cyc_CfFlowInfo_aerr(s->loc,({const char*_TmpB="`noreturn' function might return";_tag_fat(_TmpB,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2693
goto _LL1B;case 7: _LL1B:
 goto _LL1D;case 8: _LL1D: {
# 2697
struct Cyc_Absyn_Stmt*dest=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(env->succ_table,s);
if(dest == 0)
Cyc_CfFlowInfo_aerr(s->loc,({const char*_TmpB="jump has no target (should be impossible)";_tag_fat(_TmpB,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_check_null(dest));
# 2702
Cyc_NewControlFlow_update_flow(env,dest,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _TmpA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;_Tmp8=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3;{struct Cyc_Absyn_Exp*e=_TmpA;struct Cyc_List_List*scs=_Tmp9;void*dec_tree=_Tmp8;
# 2708
return Cyc_NewControlFlow_anal_scs(env,inflow,scs,e->loc);}case 15: _TmpA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;_Tmp8=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3;{struct Cyc_Absyn_Stmt*s1=_TmpA;struct Cyc_List_List*scs=_Tmp9;void*dec_tree=_Tmp8;
# 2713
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2721
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2724
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
# 2726
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,scs,0U);
{union Cyc_CfFlowInfo_FlowInfo _TmpB=scs_outflow;if((_TmpB.BottomFL).tag == 1)
goto _LL5A;else{
Cyc_CfFlowInfo_aerr(s->loc,({const char*_TmpC="last catch clause may implicitly fallthru";_tag_fat(_TmpC,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));}_LL5A:;}
# 2731
outflow=s1_outflow;
# 2733
return outflow;}}}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)->f2 != 0){_TmpA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)->f1;_Tmp9=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)->f2)->v;_Tmp8=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)->f3;_Tmp4=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->loc;_Tmp7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Pat*letpat=_TmpA;struct Cyc_List_List*vds=_Tmp9;struct Cyc_Absyn_Exp*e=_Tmp8;unsigned loc=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp7;
# 2738
Cyc_fprintf(Cyc_stderr,({const char*_TmpB="";_tag_fat(_TmpB,sizeof(char),1U);}),_tag_fat(0U,sizeof(void*),0));{
# 2744
int isAliasPat=0;
{void*_stmttmp98=letpat->r;void*_TmpB=_stmttmp98;if(*((int*)_TmpB)== 2){
isAliasPat=1;goto _LL5F;}else{
goto _LL5F;}_LL5F:;}{
# 2749
struct _tuple1 _stmttmp99=Cyc_NewControlFlow_get_unconsume_pat_vars(vds);struct _tuple1 _TmpB=_stmttmp99;void*_TmpC;void*_TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{struct Cyc_List_List*roots=_TmpD;struct Cyc_List_List*es=_TmpC;
inflow=Cyc_NewControlFlow_initialize_pat_vars(fenv,env,inflow,vds,roots != 0,loc,e->loc,isAliasPat);{
struct Cyc_List_List*old_unique_pat_vars=env->unique_pat_vars;
# 2753
({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple31*_Tmp10=({struct _tuple31*_Tmp11=_cycalloc(sizeof(struct _tuple31));_Tmp11->f1=0,_Tmp11->f2=s,_Tmp11->f3=roots,_Tmp11->f4=es;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=old_unique_pat_vars;_TmpF;});
# 2753
env->unique_pat_vars=_TmpE;});
# 2758
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2762
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,inflow,loc);
# 2766
return inflow;}}}}}}else{goto _LL26;}case 4: _TmpA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)->f2;_Tmp9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)->f3;_Tmp4=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->loc;_Tmp8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;if((struct Cyc_Absyn_Exp*)_Tmp9 != 0){struct Cyc_Absyn_Vardecl*vd=_TmpA;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp9;unsigned loc=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp8;
# 2778
struct Cyc_List_List l=({struct Cyc_List_List _TmpB;_TmpB.hd=open_exp_opt,_TmpB.tl=0;_TmpB;});
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple19 _stmttmp9A=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,open_exp_opt);struct _tuple19 _TmpB=_stmttmp9A;union Cyc_CfFlowInfo_AbsLVal _TmpC;_TmpC=_TmpB.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_TmpC;
struct _tuple17 _stmttmp9B=Cyc_NewControlFlow_anal_Rexp(env,1,f,open_exp_opt,0);struct _tuple17 _TmpD=_stmttmp9B;union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpE;
struct Cyc_List_List*roots=0;
struct Cyc_List_List*es=0;
{union Cyc_CfFlowInfo_FlowInfo _TmpF=f;void*_Tmp10;struct Cyc_Dict_Dict _Tmp11;if((_TmpF.ReachableFL).tag == 2){_Tmp11=((_TmpF.ReachableFL).val).f1;_Tmp10=((_TmpF.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_Tmp11;struct Cyc_List_List*relns=_Tmp10;
# 2786
{union Cyc_CfFlowInfo_AbsLVal _Tmp12=lval;void*_Tmp13;if((_Tmp12.PlaceL).tag == 1){_Tmp13=(_Tmp12.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_Tmp13;
# 2790
void*new_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp14=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp14->tag=8,_Tmp14->f1=vd,_Tmp14->f2=new_rval;_Tmp14;});
fd=Cyc_CfFlowInfo_assign_place(fenv,open_exp_opt->loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd,relns);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct)),root->tag=0,root->f1=vd;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)root,rp->path=0;
roots=({struct Cyc_List_List*_Tmp14=_cycalloc(sizeof(struct Cyc_List_List));_Tmp14->hd=rp,_Tmp14->tl=roots;_Tmp14;});
es=({struct Cyc_List_List*_Tmp14=_cycalloc(sizeof(struct Cyc_List_List));_Tmp14->hd=open_exp_opt,_Tmp14->tl=es;_Tmp14;});
goto _LL72;}}}}else{
# 2805
goto _LL72;}_LL72:;}
# 2807
goto _LL6D;}}else{
# 2809
goto _LL6D;}_LL6D:;}{
# 2812
struct Cyc_List_List vds=({struct Cyc_List_List _TmpF;_TmpF.hd=vd,_TmpF.tl=0;_TmpF;});
f=Cyc_NewControlFlow_add_vars(fenv,f,& vds,fenv->unknown_all,loc,0);{
# 2816
struct Cyc_List_List*old_unique_pat_vars=env->unique_pat_vars;
({struct Cyc_List_List*_TmpF=({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple31*_Tmp11=({struct _tuple31*_Tmp12=_cycalloc(sizeof(struct _tuple31));_Tmp12->f1=1,_Tmp12->f2=s,_Tmp12->f3=roots,_Tmp12->f4=es;_Tmp12;});_Tmp10->hd=_Tmp11;}),_Tmp10->tl=old_unique_pat_vars;_Tmp10;});env->unique_pat_vars=_TmpF;});
# 2821
f=Cyc_NewControlFlow_anal_stmt(env,f,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2825
f=Cyc_NewControlFlow_unconsume_params(env,roots,es,1,f,loc);
# 2829
return f;}}}}}else{goto _LL26;}default: _LL26: _TmpA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Decl*d=_TmpA;struct Cyc_Absyn_Stmt*s=_Tmp9;
# 2832
outflow=({struct Cyc_NewControlFlow_AnalEnv*_TmpB=env;union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_NewControlFlow_anal_decl(env,inflow,d);struct Cyc_Absyn_Stmt*_TmpD=s;Cyc_NewControlFlow_anal_stmt(_TmpB,_TmpC,_TmpD,rval_opt);});
if(Cyc_Flags_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,d);
return outflow;}}default: _TmpA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Stmt*s=_TmpA;
# 2837
return Cyc_NewControlFlow_anal_stmt(env,inflow,s,rval_opt);}};}}}
# 2843
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Fndecl*);
# 2847
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
void*_stmttmp9C=decl->r;void*_Tmp0=_stmttmp9C;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 2855
if((int)vd->sc == 0)
return inflow;{
# 2860
struct Cyc_List_List vds=({struct Cyc_List_List _Tmp4;_Tmp4.hd=vd,_Tmp4.tl=0;_Tmp4;});
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*e=vd->initializer;
if(e == 0)
return inflow;{
struct _tuple17 _stmttmp9D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e,0);struct _tuple17 _Tmp4=_stmttmp9D;void*_Tmp5;union Cyc_CfFlowInfo_FlowInfo _Tmp6;_Tmp6=_Tmp4.f1;_Tmp5=_Tmp4.f2;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp6;void*r=_Tmp5;
return Cyc_NewControlFlow_do_initialize_var(fenv,env,f,vd,e,r,decl->loc);}}}}}case 3: _Tmp3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp3;
# 2869
return Cyc_NewControlFlow_add_vars(fenv,inflow,vds,fenv->unknown_none,decl->loc,0);}case 1: _Tmp3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp3;
# 2872
Cyc_NewControlFlow_check_nested_fun(env->all_tables,fenv,inflow,fd);{
struct Cyc_Absyn_Vardecl*vd=_check_null(fd->fn_vardecl);
# 2877
return({struct Cyc_CfFlowInfo_FlowEnv*_Tmp4=fenv;union Cyc_CfFlowInfo_FlowInfo _Tmp5=inflow;struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=vd,_Tmp7->tl=0;_Tmp7;});void*_Tmp7=fenv->unknown_all;Cyc_NewControlFlow_add_vars(_Tmp4,_Tmp5,_Tmp6,_Tmp7,decl->loc,0);});}}case 4: _Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp3;struct Cyc_Absyn_Vardecl*vd=_Tmp2;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp1;
# 2880
if(open_exp_opt != 0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp4="found open expression in declaration!";_tag_fat(_Tmp4,sizeof(char),38U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List vds=({struct Cyc_List_List _Tmp4;_Tmp4.hd=vd,_Tmp4.tl=0;_Tmp4;});
return Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_all,decl->loc,0);}}default:
# 2886
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp4="anal_decl: unexpected decl variant";_tag_fat(_Tmp4,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));};}
# 2894
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_Tmp2=tables;struct Cyc_CfFlowInfo_FlowEnv*_Tmp3=fenv;union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_Tmp2,_Tmp3,_Tmp4,fd);});}
# 2896
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3=_Tmp2;void*_Tmp4;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp3)->tag == Cyc_Dict_Absent){
# 2902
if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Dict_Absent_exn_struct));_Tmp5->tag=Cyc_Dict_Absent;_Tmp5;}));}}else{_Tmp4=_Tmp3;{void*exn=_Tmp4;(void*)_rethrow(exn);}}_LL0:;}}}
# 2908
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2913
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _Tmp0=r;unsigned _Tmp1;if((_Tmp0.RParam).tag == 5){_Tmp1=(_Tmp0.RParam).val;{unsigned i=_Tmp1;
# 2916
struct Cyc_Absyn_Vardecl*vd=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(vds,(int)i);
if(!vd->escapes)
return Cyc_Relations_RVar(vd);
return r;}}else{
return r;};}
# 2927
static struct Cyc_List_List*Cyc_NewControlFlow_get_noconsume_params(struct Cyc_List_List*param_vardecls,struct Cyc_List_List*atts){
# 2929
struct _RegionHandle _Tmp0=_new_region("r");struct _RegionHandle*r=& _Tmp0;_push_region(r);
{int len=Cyc_List_length(param_vardecls);
struct _fat_ptr cons=({unsigned _Tmp1=(unsigned)len;_tag_fat(({int*_Tmp2=({struct _RegionHandle*_Tmp3=r;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(int)));});({{unsigned _Tmp3=(unsigned)len;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]=0;}}0;});_Tmp2;}),sizeof(int),_Tmp1);});
for(1;atts != 0;atts=atts->tl){
void*_stmttmp9E=(void*)atts->hd;void*_Tmp1=_stmttmp9E;int _Tmp2;switch(*((int*)_Tmp1)){case 22: _Tmp2=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_Tmp1)->f1;{int i=_Tmp2;
_Tmp2=i;goto _LL4;}case 21: _Tmp2=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_Tmp1)->f1;_LL4: {int i=_Tmp2;
*((int*)_check_fat_subscript(cons,sizeof(int),i - 1))=1;goto _LL0;}default:
 goto _LL0;}_LL0:;}{
# 2939
struct Cyc_List_List*noconsume_roots=0;
{int i=0;for(0;param_vardecls != 0;(param_vardecls=param_vardecls->tl,i ++)){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)param_vardecls->hd;
if(Cyc_Tcutil_is_noalias_pointer(vd->type,0)&& !(*((int*)_check_fat_subscript(cons,sizeof(int),i)))){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct)),root->tag=0,root->f1=vd;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)root,rp->path=0;
noconsume_roots=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=rp,_Tmp1->tl=noconsume_roots;_Tmp1;});}}}}{
# 2948
struct Cyc_List_List*_Tmp1=noconsume_roots;_npop_handler(0);return _Tmp1;}}}
# 2930
;_pop_region();}
# 2951
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2955
unsigned loc=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v,fenv->unknown_all,loc,1);{
# 2960
struct Cyc_List_List*param_roots=0;
struct _tuple15 _stmttmp9F=({union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;if((_Tmp0.ReachableFL).tag != 2)_throw_match();(_Tmp0.ReachableFL).val;});struct _tuple15 _Tmp0=_stmttmp9F;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_Dict_Dict d=_Tmp2;struct Cyc_List_List*relns=_Tmp1;
# 2964
struct Cyc_List_List*vardecls=(struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*reqs=(fd->i).requires_relns;for(0;reqs != 0;reqs=reqs->tl){
struct Cyc_Relations_Reln*req=(struct Cyc_Relations_Reln*)reqs->hd;
relns=({struct _RegionHandle*_Tmp3=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _Tmp4=Cyc_NewControlFlow_translate_rop(vardecls,req->rop1);enum Cyc_Relations_Relation _Tmp5=req->relation;union Cyc_Relations_RelnOp _Tmp6=
Cyc_NewControlFlow_translate_rop(vardecls,req->rop2);
# 2967
Cyc_Relations_add_relation(_Tmp3,_Tmp4,_Tmp5,_Tmp6,relns);});}}{
# 2973
struct Cyc_List_List*atts;
{void*_stmttmpA0=Cyc_Absyn_compress(_check_null(fd->cached_type));void*_Tmp3=_stmttmpA0;void*_Tmp4;if(*((int*)_Tmp3)== 5){_Tmp4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp3)->f1).attributes;{struct Cyc_List_List*as=_Tmp4;
atts=as;goto _LL3;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp5="check_fun: non-function type cached with fndecl_t";_tag_fat(_Tmp5,sizeof(char),50U);}),_tag_fat(0U,sizeof(void*),0));}_LL3:;}{
# 2978
struct Cyc_List_List*noconsume_roots=Cyc_NewControlFlow_get_noconsume_params((struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v,atts);
# 2981
for(1;atts != 0;atts=atts->tl){
void*_stmttmpA1=(void*)atts->hd;void*_Tmp3=_stmttmpA1;int _Tmp4;switch(*((int*)_Tmp3)){case 21: _Tmp4=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_Tmp3)->f1;{int i=_Tmp4;
# 2984
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(-1,0U);
struct Cyc_Absyn_Vardecl*vd=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})((struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v,i - 1);
void*t=Cyc_Absyn_compress(vd->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*rval=({
struct Cyc_CfFlowInfo_FlowEnv*_Tmp5=fenv;void*_Tmp6=elttype;struct Cyc_Absyn_Exp*_Tmp7=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_Tmp5,_Tmp6,_Tmp7,-1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all),0);});
# 2992
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r;r=_cycalloc(sizeof(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct)),r->tag=2,r->f1=i,r->f2=t;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)r,rp->path=0;
d=Cyc_Dict_insert(d,(void*)r,rval);
d=({struct Cyc_Dict_Dict _Tmp5=d;void*_Tmp6=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_Tmp7->tag=0,_Tmp7->f1=vd;_Tmp7;});Cyc_Dict_insert(_Tmp5,_Tmp6,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=4,_Tmp7->f1=rp;_Tmp7;}));});
goto _LL8;}}case 20: _Tmp4=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_Tmp3)->f1;{int i=_Tmp4;
# 2998
struct Cyc_Absyn_Vardecl*vd=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})((struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v,i - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(vd->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r;r=_cycalloc(sizeof(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct)),r->tag=2,r->f1=i,r->f2=elttype;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)r,rp->path=0;
d=({struct Cyc_Dict_Dict _Tmp5=d;void*_Tmp6=(void*)r;Cyc_Dict_insert(_Tmp5,_Tmp6,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});
d=({struct Cyc_Dict_Dict _Tmp5=d;void*_Tmp6=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_Tmp7->tag=0,_Tmp7->f1=vd;_Tmp7;});Cyc_Dict_insert(_Tmp5,_Tmp6,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=4,_Tmp7->f1=rp;_Tmp7;}));});
param_roots=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=rp,_Tmp5->tl=param_roots;_Tmp5;});
goto _LL8;}}default:
 goto _LL8;}_LL8:;}
# 3009
inflow=Cyc_CfFlowInfo_ReachableFL(d,relns);{
# 3011
int noreturn=Cyc_Atts_is_noreturn_fn_type(Cyc_Tcutil_fndecl2type(fd));
struct Cyc_Hashtable_Table*flow_table=
({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;})(33,({(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Stmt*))Cyc_NewControlFlow_hash_ptr;}));
struct Cyc_NewControlFlow_AnalEnv*env;
env=_cycalloc(sizeof(struct Cyc_NewControlFlow_AnalEnv)),env->all_tables=tables,({
struct Cyc_Hashtable_Table*_Tmp3=({(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;})(tables->succ_tables,fd);env->succ_table=_Tmp3;}),({
struct Cyc_Hashtable_Table*_Tmp3=({(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;})(tables->pat_pop_tables,fd);env->pat_pop_table=_Tmp3;}),env->fenv=fenv,env->iterate_again=1,env->iteration_num=0,env->in_try=0,env->tryflow=inflow,env->noreturn=noreturn,env->return_type=(fd->i).ret_type,env->unique_pat_vars=0,env->param_roots=param_roots,env->noconsume_params=noconsume_roots,env->flow_table=flow_table,env->return_relns=(fd->i).ensures_relns;{
# 3021
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3027
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3030
union Cyc_CfFlowInfo_FlowInfo _Tmp3=outflow;if((_Tmp3.BottomFL).tag == 1)
goto _LLF;else{
# 3033
Cyc_NewControlFlow_check_init_params(loc,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,loc);
# 3037
if(noreturn)
Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp4="`noreturn' function might (implicitly) return";_tag_fat(_Tmp4,sizeof(char),46U);}),_tag_fat(0U,sizeof(void*),0));else{
if(!Cyc_Tcutil_is_void_type((fd->i).ret_type)){
if(Cyc_Tcutil_is_any_float_type((fd->i).ret_type)||
 Cyc_Tcutil_is_any_int_type((fd->i).ret_type))
Cyc_Warn_warn(loc,({const char*_Tmp4="function may not return a value";_tag_fat(_Tmp4,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));else{
# 3044
Cyc_CfFlowInfo_aerr(loc,({const char*_Tmp4="function may not return a value";_tag_fat(_Tmp4,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));}goto _LLF;}}
# 3046
goto _LLF;}_LLF:;}}}}}}}}
# 3050
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(1;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_stmttmpA2=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_Tmp0=_stmttmpA2;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
Cyc_NewControlFlow_check_fun(tables,fd);goto _LL0;}case 11: _Tmp1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds2=_Tmp1;
# 3056
_Tmp1=ds2;goto _LL6;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL6: {struct Cyc_List_List*ds2=_Tmp1;
_Tmp1=ds2;goto _LL8;}case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL8: {struct Cyc_List_List*ds2=_Tmp1;
Cyc_NewControlFlow_cf_check(tables,ds2);goto _LL0;}case 12:
 goto _LL0;default:
 goto _LL0;}_LL0:;}}}

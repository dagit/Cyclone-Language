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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 349
extern struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*,void*);
# 354
extern struct Cyc_List_List*Cyc_List_delete_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 857 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 894
extern void*Cyc_Absyn_void_type;
# 935
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1061
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1106
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1112
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);
# 1114
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 34 "cifc.h"
void Cyc_Cifc_merge_sys_user_decl(unsigned,struct Cyc_Tcenv_Tenv*,int,struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*);
# 30 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*);
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*);
# 76
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_NewInfo[8U];
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
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
int Cyc_Cifc_opt_inst_tvar=0;
# 44
void Cyc_Cifc_set_inst_tvar (void){
Cyc_Cifc_opt_inst_tvar=1;}
# 48
struct _fat_ptr Cyc_Cifc_list2string(struct Cyc_List_List*l,struct _fat_ptr(*tostr)(void*)){
struct _fat_ptr ret=Cyc_aprintf(({const char*_Tmp0="(";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
while((unsigned)l){
ret=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=ret;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(_check_null(tostr))(l->hd);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(({const char*_Tmp3="%s %s,";_tag_fat(_Tmp3,sizeof(char),7U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
l=l->tl;}
# 54
return({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=ret;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="%s)";_tag_fat(_Tmp2,sizeof(char),4U);}),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 59
static void Cyc_Cifc_fail_merge(int warn,unsigned loc,int is_buildlib,struct _tuple0*n,struct _fat_ptr s){
# 61
if(is_buildlib){
struct _fat_ptr preamble=warn?({const char*_Tmp0="Warning: user-defined";_tag_fat(_Tmp0,sizeof(char),22U);}):({const char*_Tmp0="User-defined";_tag_fat(_Tmp0,sizeof(char),13U);});
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=preamble;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
# 65
struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(n);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=s;_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="%s type for %s incompatible with system version %s\n";_tag_fat(_Tmp4,sizeof(char),52U);}),_tag_fat(_Tmp3,sizeof(void*),3));});}else{
# 67
if(warn)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="User-defined type for ";_tag_fat(_Tmp2,sizeof(char),23U);});_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=n;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=" incompatible with system version ";_tag_fat(_Tmp4,sizeof(char),35U);});_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_warn2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});else{
# 70
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="User-defined type for ";_tag_fat(_Tmp2,sizeof(char),23U);});_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=n;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=" incompatible with system version ";_tag_fat(_Tmp4,sizeof(char),35U);});_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});}}}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 75
static void*Cyc_Cifc_expand_c_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp){
# 77
void*_Tmp0=ctyp;void*_Tmp1;enum Cyc_Absyn_AggrKind _Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 8:
# 80
 Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
{void*_Tmp4=ctyp;void*_Tmp5;if(*((int*)_Tmp4)== 8){_Tmp5=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp4)->f4;{void*to=_Tmp5;
# 84
return _check_null(to);}}else{
# 86
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="Impos";_tag_fat(_Tmp8,sizeof(char),6U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL19;}_LL19:;}
# 89
return ctyp;case 5: _Tmp3=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_FnInfo*finfo=_Tmp3;
# 91
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 93
({void*_Tmp4=({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Cifc_expand_c_type(_Tmp5,_Tmp6,Cyc_Toc_typ_to_c(finfo->ret_type));});finfo->ret_type=_Tmp4;});{
struct Cyc_List_List*args=finfo->args;
while((unsigned)args){
struct _tuple8*_stmttmp0=(struct _tuple8*)args->hd;struct _tuple8*_Tmp4=_stmttmp0;void*_Tmp5;_Tmp5=(void**)& _Tmp4->f3;{void**argType=(void**)_Tmp5;
({void*_Tmp6=({unsigned _Tmp7=loc;struct Cyc_Tcenv_Tenv*_Tmp8=te;Cyc_Cifc_expand_c_type(_Tmp7,_Tmp8,Cyc_Toc_typ_to_c(*argType));});*argType=_Tmp6;});
args=args->tl;}}
# 101
finfo->tvars=0;
finfo->effect=0;
return ctyp;}}case 3: _Tmp3=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo*pinfo=_Tmp3;
# 107
({void*_Tmp4=({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Cifc_expand_c_type(_Tmp5,_Tmp6,Cyc_Toc_typ_to_c(pinfo->elt_type));});pinfo->elt_type=_Tmp4;});
return ctyp;}case 4: _Tmp3=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_ArrayInfo*ainfo=_Tmp3;
# 110
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 112
({void*_Tmp4=({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Cifc_expand_c_type(_Tmp5,_Tmp6,Cyc_Toc_typ_to_c(ainfo->elt_type));});ainfo->elt_type=_Tmp4;});
return ctyp;}case 6: _Tmp3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tlst=_Tmp3;
# 115
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 117
while((unsigned)tlst){
struct _tuple11*_stmttmp1=(struct _tuple11*)tlst->hd;struct _tuple11*_Tmp4=_stmttmp1;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;_Tmp6=_Tmp4->f1;_Tmp5=(void**)& _Tmp4->f2;{struct Cyc_Absyn_Tqual tq=_Tmp6;void**elt_type=(void**)_Tmp5;
({void*_Tmp7=({unsigned _Tmp8=loc;struct Cyc_Tcenv_Tenv*_Tmp9=te;Cyc_Cifc_expand_c_type(_Tmp8,_Tmp9,Cyc_Toc_typ_to_c(*elt_type));});*elt_type=_Tmp7;});
tlst=tlst->tl;}}
# 123
return ctyp;}case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{enum Cyc_Absyn_AggrKind knd=_Tmp2;struct Cyc_List_List*flst=_Tmp3;
# 126
return ctyp;}case 10: _Tmp3=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_TypeDecl*td=_Tmp3;void**tptr=_Tmp1;
# 129
return ctyp;}case 9: _Tmp3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 132
return ctyp;}case 11: _Tmp3=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 135
return ctyp;}case 0:
# 138
 return ctyp;case 1:
# 141
 return ctyp;default:
# 144
 return ctyp;};}
# 148
static int Cyc_Cifc_is_boxed_kind(void*t){
# 150
void*_Tmp0=t;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*tc=_Tmp2;struct Cyc_List_List*ts=_Tmp1;
# 152
{void*_Tmp3=tc;enum Cyc_Absyn_Size_of _Tmp4;switch(*((int*)_Tmp3)){case 1: _Tmp4=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3)->f2;{enum Cyc_Absyn_Size_of sz=_Tmp4;
# 154
return(int)sz == 2 ||(int)sz == 3;}case 3:
 goto _LLF;case 4: _LLF:
# 157
 return 1;default:
# 159
 return 0;};}
# 161
goto _LL0;}case 3: _Tmp2=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo*pi=_Tmp2;
# 163
{void*_stmttmp2=(pi->ptr_atts).bounds;void*_Tmp3=_stmttmp2;void*_Tmp4;if(*((int*)_Tmp3)== 0){_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1;{void*tc=_Tmp4;
# 165
void*_Tmp5=tc;if(*((int*)_Tmp5)== 14)
# 167
return 0;else{
# 169
return 1;};}}else{
# 172
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp5="ptrbound_t must have an AppType";_tag_fat(_Tmp5,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));};}
# 174
goto _LL0;}case 9: _Tmp2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
# 176
return 1;}default:
# 178
 return 0;}_LL0:;}
# 183
static int Cyc_Cifc_check_fntype_kinds(unsigned,struct Cyc_Tcenv_Tenv*,void*,void*);
# 189
static int Cyc_Cifc_c_types_ok(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp,void*cyctyp){
struct Cyc_Absyn_Tqual tq;
# 192
void*ctyp_cp=Cyc_Tcutil_copy_type(ctyp);
void*utyp_cp=Cyc_Tcutil_copy_type(cyctyp);
# 195
void*c_typ=Cyc_Toc_typ_to_c(ctyp_cp);
void*u_typ=Cyc_Toc_typ_to_c(utyp_cp);
# 200
if(!Cyc_Unify_unify(c_typ,u_typ)){
c_typ=Cyc_Cifc_expand_c_type(loc,te,c_typ);
u_typ=Cyc_Cifc_expand_c_type(loc,te,u_typ);
if(!Cyc_Unify_unify(c_typ,u_typ)){
# 206
if(Cyc_Tcutil_is_function_type(c_typ))
return Cyc_Cifc_check_fntype_kinds(loc,te,c_typ,u_typ);
return Cyc_Cifc_is_boxed_kind(c_typ)&& Cyc_Cifc_is_boxed_kind(u_typ);}}
# 211
return 1;}struct _tuple12{void*f1;void*f2;};
# 223 "cifc.cyc"
static int Cyc_Cifc_check_fntype_kinds(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*c_typ,void*u_typ){
struct _tuple12 _stmttmp3=({struct _tuple12 _Tmp0;_Tmp0.f1=c_typ,_Tmp0.f2=u_typ;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp3;struct Cyc_Absyn_FnInfo _Tmp1;struct Cyc_Absyn_FnInfo _Tmp2;if(*((int*)_Tmp0.f1)== 5){if(*((int*)_Tmp0.f2)== 5){_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_FnInfo cfi=_Tmp2;struct Cyc_Absyn_FnInfo ufi=_Tmp1;
# 226
int typesOk=Cyc_Cifc_c_types_ok(loc,te,cfi.ret_type,ufi.ret_type);
if(typesOk){
struct Cyc_List_List*ca=cfi.args;
struct Cyc_List_List*ua=ufi.args;
while((unsigned)ca && typesOk){
if(!((unsigned)ua))
return 0;
typesOk=Cyc_Cifc_c_types_ok(loc,te,(*((struct _tuple8*)ca->hd)).f3,(*((struct _tuple8*)ua->hd)).f3);
# 235
ca=ca->tl;
ua=ua->tl;}}
# 239
return typesOk;
goto _LL0;}}else{goto _LL3;}}else{_LL3:
# 242
 return 0;}_LL0:;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 249
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,struct Cyc_Tcenv_Tenv*te,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
# 251
struct _tuple12 _stmttmp4=({struct _tuple12 _Tmp0;_Tmp0.f1=c_decl->r,_Tmp0.f2=user_decl->r;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp4;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0.f1)){case 0: if(*((int*)_Tmp0.f2)== 0){_Tmp2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Vardecl*cd=_Tmp2;struct Cyc_Absyn_Vardecl*ud=_Tmp1;
# 253
if(!Cyc_Cifc_c_types_ok(loc,te,cd->type,ud->type)){
({unsigned _Tmp3=loc;int _Tmp4=is_buildlib;struct _tuple0*_Tmp5=cd->name;Cyc_Cifc_fail_merge(0,_Tmp3,_Tmp4,_Tmp5,({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_typ2string(ud->type);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_typ2string(cd->type);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9=": type %s != %s";_tag_fat(_Tmp9,sizeof(char),16U);}),_tag_fat(_Tmp8,sizeof(void*),2));}));});if(!0)return;}else{
# 261
if(ud->attributes != 0)
({struct Cyc_List_List*_Tmp3=Cyc_Atts_merge_attributes(cd->attributes,ud->attributes);cd->attributes=_Tmp3;});
cd->type=ud->type;
cd->tq=ud->tq;}
# 266
goto _LL0;}}else{goto _LLB;}case 1: if(*((int*)_Tmp0.f2)== 0){_Tmp2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Fndecl*cd=_Tmp2;struct Cyc_Absyn_Vardecl*ud=_Tmp1;
# 269
if(!Cyc_Tcutil_is_function_type(ud->type)){
Cyc_Cifc_fail_merge(0,loc,is_buildlib,ud->name,({const char*_Tmp3=": type must be a function type to match decl\n";_tag_fat(_Tmp3,sizeof(char),46U);}));if(!0)return;}{
# 272
void*cdtype;
if(cd->cached_type != 0)
cdtype=_check_null(cd->cached_type);else{
# 276
cdtype=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp3->tag=5,_Tmp3->f1=cd->i;_Tmp3;});}
if(!Cyc_Cifc_c_types_ok(loc,te,cdtype,ud->type)){
({unsigned _Tmp3=loc;int _Tmp4=is_buildlib;struct _tuple0*_Tmp5=ud->name;Cyc_Cifc_fail_merge(0,_Tmp3,_Tmp4,_Tmp5,({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_typ2string(cdtype);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_typ2string(ud->type);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9=": type %s != %s";_tag_fat(_Tmp9,sizeof(char),16U);}),_tag_fat(_Tmp8,sizeof(void*),2));}));});if(!0)return;}else{
# 285
void*_stmttmp5=ud->type;void*_Tmp3=_stmttmp5;struct Cyc_Absyn_FnInfo _Tmp4;if(*((int*)_Tmp3)== 5){_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp3)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp4;
# 287
struct Cyc_List_List*old_tvars=fi.tvars;
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ud->type);
if((cd->i).attributes != 0)
({struct Cyc_List_List*_Tmp5=Cyc_Atts_merge_attributes(fi.attributes,(cd->i).attributes);fi.attributes=_Tmp5;});
cd->i=fi;
goto _LLD;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="oops!\n";_tag_fat(_Tmp7,sizeof(char),7U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(0U,_tag_fat(_Tmp6,sizeof(void*),1));});}_LLD:;}
# 296
goto _LL0;}}}else{goto _LLB;}case 5: if(*((int*)_Tmp0.f2)== 5){_Tmp2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Aggrdecl*cd=_Tmp2;struct Cyc_Absyn_Aggrdecl*ud=_Tmp1;
# 304
if((int)ud->sc != (int)cd->sc){
Cyc_Cifc_fail_merge(1,loc,is_buildlib,cd->name,({const char*_Tmp3=": scopes don't match (ignoring)";_tag_fat(_Tmp3,sizeof(char),32U);}));if(!1)return;}
# 307
if(ud->impl == 0){
Cyc_Cifc_fail_merge(0,loc,is_buildlib,cd->name,({const char*_Tmp3=": no user definition";_tag_fat(_Tmp3,sizeof(char),21U);}));if(!0)return;}
if(cd->impl == 0){
Cyc_Cifc_fail_merge(1,loc,is_buildlib,cd->name,({const char*_Tmp3=": no definition for system version";_tag_fat(_Tmp3,sizeof(char),35U);}));if(!1)return;
c_decl->r=user_decl->r;
return;}
# 314
{struct _tuple13 _stmttmp6=({struct _tuple13 _Tmp3;_Tmp3.f1=cd->impl,_Tmp3.f2=ud->impl;_Tmp3;});struct _tuple13 _Tmp3=_stmttmp6;int _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;if(_Tmp3.f1 != 0){if(_Tmp3.f2 != 0){_TmpA=(struct Cyc_List_List**)&(_Tmp3.f1)->exist_vars;_Tmp9=(struct Cyc_List_List**)&(_Tmp3.f1)->rgn_po;_Tmp8=(_Tmp3.f1)->fields;_Tmp7=(_Tmp3.f2)->exist_vars;_Tmp6=(_Tmp3.f2)->rgn_po;_Tmp5=(_Tmp3.f2)->fields;_Tmp4=(_Tmp3.f2)->tagged;{struct Cyc_List_List**tvarsC=(struct Cyc_List_List**)_TmpA;struct Cyc_List_List**rgnpoC=(struct Cyc_List_List**)_Tmp9;struct Cyc_List_List*cfields=_Tmp8;struct Cyc_List_List*tvars=_Tmp7;struct Cyc_List_List*rgnpo=_Tmp6;struct Cyc_List_List*ufields=_Tmp5;int tagged=_Tmp4;
# 317
if(tagged){
Cyc_Cifc_fail_merge(0,loc,is_buildlib,cd->name,({const char*_TmpB=": user @tagged annotation not allowed (ignoring)";_tag_fat(_TmpB,sizeof(char),49U);}));if(!0)return;}{
# 320
struct Cyc_List_List*x=cfields;
# 322
while(x != 0){
struct Cyc_Absyn_Aggrfield*cfield=(struct Cyc_Absyn_Aggrfield*)x->hd;
struct Cyc_Absyn_Aggrfield*ufield=Cyc_Absyn_lookup_field(ufields,cfield->name);
# 326
if(ufield != 0){
# 329
if(!Cyc_Cifc_c_types_ok(loc,te,cfield->type,ufield->type)){
({unsigned _TmpB=loc;int _TmpC=is_buildlib;struct _tuple0*_TmpD=cd->name;Cyc_Cifc_fail_merge(0,_TmpB,_TmpC,_TmpD,({struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({struct _fat_ptr _Tmp10=Cyc_Absynpp_typ2string(ufield->type);_TmpF.f1=_Tmp10;});_TmpF;});struct Cyc_String_pa_PrintArg_struct _TmpF=({struct Cyc_String_pa_PrintArg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=*cfield->name;_Tmp10;});struct Cyc_String_pa_PrintArg_struct _Tmp10=({struct Cyc_String_pa_PrintArg_struct _Tmp11;_Tmp11.tag=0,({struct _fat_ptr _Tmp12=Cyc_Absynpp_typ2string(cfield->type);_Tmp11.f1=_Tmp12;});_Tmp11;});void*_Tmp11[3];_Tmp11[0]=& _TmpE,_Tmp11[1]=& _TmpF,_Tmp11[2]=& _Tmp10;Cyc_aprintf(({const char*_Tmp12=": type %s of user definition of field %s != %s";_tag_fat(_Tmp12,sizeof(char),47U);}),_tag_fat(_Tmp11,sizeof(void*),3));}));});if(!0)return;}else{
# 338
if(ufield->width != 0){
({unsigned _TmpB=loc;int _TmpC=is_buildlib;struct _tuple0*_TmpD=cd->name;Cyc_Cifc_fail_merge(1,_TmpB,_TmpC,_TmpD,({struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({struct _fat_ptr _Tmp10=Cyc_Absynpp_typ2string(ufield->type);_TmpF.f1=_Tmp10;});_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_aprintf(({const char*_Tmp10=": ignoring width of user definition of field %s";_tag_fat(_Tmp10,sizeof(char),48U);}),_tag_fat(_TmpF,sizeof(void*),1));}));});if(!1)return;}
# 343
if(ufield->attributes != 0)
({struct Cyc_List_List*_TmpB=Cyc_Atts_merge_attributes(cfield->attributes,ufield->attributes);cfield->attributes=_TmpB;});
# 347
cfield->type=ufield->type;
cfield->tq=ufield->tq;
cfield->requires_clause=ufield->requires_clause;}}
# 352
x=x->tl;}
# 356
if(ud->tvs != 0)cd->tvs=ud->tvs;
if((unsigned)tvars)*tvarsC=tvars;
if((unsigned)rgnpo)*rgnpoC=rgnpo;
# 361
x=ufields;{
int missing_fields=0;
while(x != 0){
struct Cyc_Absyn_Aggrfield*cfield=Cyc_Absyn_lookup_field(cfields,((struct Cyc_Absyn_Aggrfield*)x->hd)->name);
if(cfield == 0){
missing_fields=1;
({unsigned _TmpB=loc;int _TmpC=is_buildlib;struct _tuple0*_TmpD=cd->name;Cyc_Cifc_fail_merge(1,_TmpB,_TmpC,_TmpD,({struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=*((struct Cyc_Absyn_Aggrfield*)x->hd)->name;_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_aprintf(({const char*_Tmp10=": no definition of field %s in system version";_tag_fat(_Tmp10,sizeof(char),46U);}),_tag_fat(_TmpF,sizeof(void*),1));}));});if(!1)return;}
# 371
x=x->tl;}
# 373
goto _LL12;}}}}else{goto _LL15;}}else{_LL15:
# 375
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_TmpB->tag=Cyc_Core_Impossible,_TmpB->f1=({const char*_TmpC="Internal Error: NULL cases not possible";_tag_fat(_TmpC,sizeof(char),40U);});_TmpB;}));}_LL12:;}
# 377
goto _LL0;}}else{goto _LLB;}case 7: if(*((int*)_Tmp0.f2)== 7){_Tmp2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Enumdecl*cd=_Tmp2;struct Cyc_Absyn_Enumdecl*ud=_Tmp1;
# 380
Cyc_Cifc_fail_merge(0,loc,is_buildlib,cd->name,({const char*_Tmp3=": enum merging not currently supported";_tag_fat(_Tmp3,sizeof(char),39U);}));if(!0)return;}}else{goto _LLB;}case 8: if(*((int*)_Tmp0.f2)== 8){_Tmp2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Typedefdecl*cd=_Tmp2;struct Cyc_Absyn_Typedefdecl*ud=_Tmp1;
# 383
if(ud->defn == 0){
Cyc_Cifc_fail_merge(0,loc,is_buildlib,cd->name,({const char*_Tmp3=": no user definition";_tag_fat(_Tmp3,sizeof(char),21U);}));if(!0)return;}
if(cd->defn == 0){
Cyc_Cifc_fail_merge(1,loc,is_buildlib,cd->name,({const char*_Tmp3=": no definition for system version";_tag_fat(_Tmp3,sizeof(char),35U);}));if(!1)return;
c_decl->r=user_decl->r;
return;}
# 391
if(!({unsigned _Tmp3=loc;struct Cyc_Tcenv_Tenv*_Tmp4=te;void*_Tmp5=_check_null(cd->defn);Cyc_Cifc_c_types_ok(_Tmp3,_Tmp4,_Tmp5,_check_null(ud->defn));})){
({unsigned _Tmp3=loc;int _Tmp4=is_buildlib;struct _tuple0*_Tmp5=cd->name;Cyc_Cifc_fail_merge(0,_Tmp3,_Tmp4,_Tmp5,({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_typ2string(_check_null(ud->defn));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_typ2string(_check_null(cd->defn));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9=": type definition %s of user definition != %s";_tag_fat(_Tmp9,sizeof(char),46U);}),_tag_fat(_Tmp8,sizeof(void*),2));}));});if(!0)return;}else{
# 398
cd->tvs=ud->tvs;
cd->defn=ud->defn;
cd->tq=ud->tq;
if(ud->atts != 0)
({struct Cyc_List_List*_Tmp3=Cyc_Atts_merge_attributes(cd->atts,ud->atts);cd->atts=_Tmp3;});}
# 405
goto _LL0;}}else{goto _LLB;}default: _LLB:
# 408
 if(is_buildlib)
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
struct _fat_ptr _Tmp5=Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_Tmp6[1];_Tmp6[0]=user_decl;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Decl*),1));}));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error in .cys file: bad (or unsupported) user-defined type %s\n";_tag_fat(_Tmp5,sizeof(char),63U);}),_tag_fat(_Tmp4,sizeof(void*),1));});else{
# 412
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="bad (or unsupported) user-defined type %s";_tag_fat(_Tmp5,sizeof(char),42U);});_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_Tmp7[1];_Tmp7[0]=user_decl;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp7,sizeof(struct Cyc_Absyn_Decl*),1));}));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),2));});}
return;}_LL0:;}
# 418
static int Cyc_Cifc_contains_type_vars(struct Cyc_Absyn_Decl*ud){
{void*_stmttmp7=ud->r;void*_Tmp0=_stmttmp7;void*_Tmp1;if(*((int*)_Tmp0)== 5){_Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 421
return ad->tvs != 0;}}else{
# 423
goto _LL0;}_LL0:;}
# 425
return 0;}
# 428
static struct Cyc_Absyn_Decl*Cyc_Cifc_make_abstract_decl(struct Cyc_Absyn_Decl*ud){
void*_stmttmp8=ud->r;void*_Tmp0=_stmttmp8;void*_Tmp1;if(*((int*)_Tmp0)== 5){_Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 431
struct Cyc_Absyn_Aggrdecl*absad=({struct Cyc_Absyn_Aggrdecl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));*_Tmp2=*ad;_Tmp2;});
absad->impl=0;{
struct Cyc_Absyn_Decl*nd=({void*_Tmp2=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp3->tag=5,_Tmp3->f1=absad;_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,ud->loc);});
return nd;}}}else{
# 436
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp2="Only aggrdecls";_tag_fat(_Tmp2,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));};}
# 441
static int Cyc_Cifc_kindbound_subsumed(void*kb1,void*kb2){
void*_Tmp0=kb1;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k1=_Tmp2;
# 444
void*_Tmp3=kb2;void*_Tmp4;if(*((int*)_Tmp3)== 0){_Tmp4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Kind*k2=_Tmp4;
# 446
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{
# 448
return 0;};}case 1: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1 != 0){_Tmp2=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->v;{void*kbb1=_Tmp2;
# 451
void*_Tmp3=kb2;void*_Tmp4;if(*((int*)_Tmp3)== 1){if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp3)->f1 != 0){_Tmp4=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp3)->f1)->v;{void*kbb2=_Tmp4;
# 453
return Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL13;}}else{_LL13:
# 455
 return 1;};}}else{
# 458
return 1;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1 != 0){_Tmp2=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->v;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{void*kbb1=_Tmp2;struct Cyc_Absyn_Kind*k1=_Tmp1;
# 460
void*_Tmp3=kb2;void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp3)== 2){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp3)->f1 != 0){_Tmp5=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp3)->f1)->v;_Tmp4=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp3)->f2;{void*kbb2=_Tmp5;struct Cyc_Absyn_Kind*k2=_Tmp4;
# 462
return((int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual)&& Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL18;}}else{_LL18:
# 464
 return 0;};}}else{_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k1=_Tmp2;
# 467
void*_Tmp3=kb2;void*_Tmp4;if(*((int*)_Tmp3)== 2){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp3)->f1 == 0){_Tmp4=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Kind*k2=_Tmp4;
# 469
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{goto _LL1D;}}else{_LL1D:
# 471
 return 0;};}}};}
# 476
static int Cyc_Cifc_find_and_remove(struct Cyc_List_List**lst,void*kind){
struct Cyc_List_List*cur=*lst;
struct Cyc_List_List*prev=0;
while((unsigned)cur){
void*t=(void*)cur->hd;
{void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
# 483
if(Cyc_Cifc_kindbound_subsumed(tv->kind,kind)){
if((unsigned)prev)
prev->tl=cur->tl;else{
# 487
*lst=cur->tl;}
cur->tl=0;
return 1;}
# 491
goto _LL0;}}else{
# 493
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_typ2string(t);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_get_type_kind(t);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp5="expects a VarType list only -- got %s(%s)";_tag_fat(_Tmp5,sizeof(char),42U);}),_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}
# 495
prev=cur;
cur=cur->tl;}
# 498
return 0;}
# 501
static struct Cyc_List_List*Cyc_Cifc_get_tvar_difference(struct Cyc_List_List*tvs,struct Cyc_List_List*remove){
struct Cyc_List_List*ret=0;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*can=(struct Cyc_Absyn_Tvar*)tvs->hd;
if(!Cyc_Cifc_find_and_remove(& remove,can->kind))
ret=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=can,_Tmp0->tl=ret;_Tmp0;});
tvs=tvs->tl;}
# 509
return ret;}char Cyc_Cifc_Contains_nontvar[17U]="Contains_nontvar";struct Cyc_Cifc_Contains_nontvar_exn_struct{char*tag;};
# 514
struct Cyc_Cifc_Contains_nontvar_exn_struct Cyc_Cifc_Contains_nontvar_val={Cyc_Cifc_Contains_nontvar};
# 516
static struct Cyc_List_List*Cyc_Cifc_extract_tvars(struct Cyc_List_List*ts){
struct Cyc_List_List*res=0;
while((unsigned)ts){
void*t=(void*)ts->hd;
{void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
# 522
res=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=tv,_Tmp2->tl=res;_Tmp2;});
goto _LL0;}}else{
# 525
(int)_throw((void*)& Cyc_Cifc_Contains_nontvar_val);}_LL0:;}
# 527
ts=ts->tl;}
# 529
return res;}
# 532
static void*Cyc_Cifc_instantiate_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
{void*_stmttmp9=tv->kind;void*_Tmp0=_stmttmp9;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
_Tmp1=k;goto _LL4;}case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_Tmp1;
# 536
if((int)k->kind == 2 ||(int)k->kind == 0)
return({void*_Tmp2=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_Tmp2,Cyc_Absyn_empty_tqual(loc));});else{
if((int)k->kind == 3)
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp2->tag=0,({void*_Tmp3=(void*)({struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct));_Tmp4->tag=5;_Tmp4;});_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;});}
goto _LL0;}default:
# 542
 return({void*_Tmp2=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_Tmp2,Cyc_Absyn_empty_tqual(loc));});}_LL0:;}
# 544
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="Unable to instantiate tvar ";_tag_fat(_Tmp2,sizeof(char),28U);});_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_tvar2string(tv);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_void_type;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 554
static struct _tuple14*Cyc_Cifc_update_tvars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,void*t,struct Cyc_Absyn_Decl*enclosing_decl,int instantiate){
# 558
{void*_Tmp0=t;struct Cyc_Absyn_FnInfo _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: _Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*tc=_Tmp3;struct Cyc_List_List**ts=(struct Cyc_List_List**)_Tmp2;
# 560
{void*_Tmp4=tc;union Cyc_Absyn_AggrInfo _Tmp5;if(*((int*)_Tmp4)== 20){_Tmp5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_Tmp4)->f1;{union Cyc_Absyn_AggrInfo ai=_Tmp5;
# 562
union Cyc_Absyn_AggrInfo _Tmp6=ai;struct _tuple2 _Tmp7;void*_Tmp8;if((_Tmp6.KnownAggr).tag == 2){_Tmp8=*(_Tmp6.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp8;
# 564
struct _tuple0*_stmttmpA=ad->name;struct _tuple0*_Tmp9=_stmttmpA;void*_TmpA;union Cyc_Absyn_Nmspace _TmpB;_TmpB=_Tmp9->f1;_TmpA=_Tmp9->f2;{union Cyc_Absyn_Nmspace ns=_TmpB;struct _fat_ptr*name=_TmpA;
# 567
struct Cyc_Absyn_Decl*ovd=Cyc_Absyn_lookup_decl(tv_ovrs,name);
if((unsigned)ovd){
# 571
void*_stmttmpB=ovd->r;void*_TmpC=_stmttmpB;void*_TmpD;if(*((int*)_TmpC)== 5){_TmpD=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_TmpC)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_TmpD;
# 573
struct Cyc_List_List*removed_tvars=0;
{struct _handler_cons _TmpE;_push_handler(& _TmpE);{int _TmpF=0;if(setjmp(_TmpE.handler))_TmpF=1;if(!_TmpF){
removed_tvars=Cyc_Cifc_extract_tvars(*ts);;_pop_handler();}else{void*_Tmp10=(void*)Cyc_Core_get_exn_thrown();void*_Tmp11=_Tmp10;void*_Tmp12;if(((struct Cyc_Cifc_Contains_nontvar_exn_struct*)_Tmp11)->tag == Cyc_Cifc_Contains_nontvar)
# 580
return 0;else{_Tmp12=_Tmp11;{void*exn=_Tmp12;(int)_rethrow(exn);}};}}}
# 582
*ts=0;{
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*tvs=ad->tvs;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
if(enclosing_decl == ovd)
({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp10=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp11->tag=2,_Tmp11->f1=tv;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=*ts;_TmpF;});*ts=_TmpE;});else{
# 591
if(instantiate || Cyc_Cifc_opt_inst_tvar)
# 593
({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp10=Cyc_Cifc_instantiate_tvar(enclosing_decl->loc,tv);_TmpF->hd=_Tmp10;}),_TmpF->tl=*ts;_TmpF;});*ts=_TmpE;});else{
# 596
struct Cyc_Absyn_Tvar*tvcpy=({struct Cyc_Absyn_Tvar*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));*_TmpE=*tv;_TmpE;});
struct _fat_ptr*tvn=({struct _fat_ptr*_TmpE=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _TmpF=({struct Cyc_Int_pa_PrintArg_struct _Tmp10=({struct Cyc_Int_pa_PrintArg_struct _Tmp11;_Tmp11.tag=1,_Tmp11.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_Tmp11;});void*_Tmp11[1];_Tmp11[0]=& _Tmp10;Cyc_aprintf(({const char*_Tmp12="`ovr_%d";_tag_fat(_Tmp12,sizeof(char),8U);}),_tag_fat(_Tmp11,sizeof(void*),1));});*_TmpE=_TmpF;});_TmpE;});
tvcpy->name=tvn;
added_tvars=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));_TmpE->hd=tvcpy,_TmpE->tl=added_tvars;_TmpE;});
({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp10=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp11->tag=2,_Tmp11->f1=tvcpy;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=*ts;_TmpF;});*ts=_TmpE;});}}
# 603
tvs=tvs->tl;}
# 605
({struct Cyc_List_List*_TmpE=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(*ts);*ts=_TmpE;});
return((enclosing_decl == ovd || Cyc_Cifc_opt_inst_tvar)|| instantiate)?0:({struct _tuple14*_TmpE=_cycalloc(sizeof(struct _tuple14));_TmpE->f1=added_tvars,_TmpE->f2=removed_tvars;_TmpE;});}}}else{
# 608
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpE="ovd must be an aggr type";_tag_fat(_TmpE,sizeof(char),25U);}),_tag_fat(0U,sizeof(void*),0));};}else{
# 612
return 0;}}}}else{_Tmp7=(_Tmp6.UnknownAggr).val;{struct _tuple2 ua=_Tmp7;
# 614
return 0;}};}}else{
# 617
goto _LLB;}_LLB:;}
# 619
return 0;}case 3: _Tmp3=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo*pi=_Tmp3;
# 621
return Cyc_Cifc_update_tvars(te,tv_ovrs,pi->elt_type,enclosing_decl,instantiate);}case 5: _Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp1;
# 623
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
if(Cyc_Cifc_opt_inst_tvar)
Cyc_Cifc_update_tvars(te,tv_ovrs,fi.ret_type,enclosing_decl,instantiate);{
# 628
struct Cyc_List_List*argit=fi.args;
while((unsigned)argit){
struct _tuple8*_stmttmpC=(struct _tuple8*)argit->hd;struct _tuple8*_Tmp4=_stmttmpC;void*_Tmp5;_Tmp5=_Tmp4->f3;{void*at=_Tmp5;
struct _tuple14*ar_tup=Cyc_Cifc_update_tvars(te,tv_ovrs,at,enclosing_decl,instantiate);
if((unsigned)ar_tup){
if((unsigned)(*ar_tup).f1)
added_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*ar_tup).f1,added_tvars);
if((unsigned)(*ar_tup).f2)
removed_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*ar_tup).f2,removed_tvars);}
# 638
argit=argit->tl;}}
# 640
if((unsigned)added_tvars ||(unsigned)removed_tvars)
return({struct _tuple14*_Tmp4=_cycalloc(sizeof(struct _tuple14));_Tmp4->f1=added_tvars,_Tmp4->f2=removed_tvars;_Tmp4;});
return 0;}}case 4: _Tmp3=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_ArrayInfo*ai=(struct Cyc_Absyn_ArrayInfo*)_Tmp3;
# 644
return Cyc_Cifc_update_tvars(te,tv_ovrs,ai->elt_type,enclosing_decl,instantiate);}default:
# 646
 return 0;};}
# 648
return 0;}
# 651
static int Cyc_Cifc_update_fninfo_usage(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,struct Cyc_Absyn_FnInfo*fi,struct Cyc_Absyn_Decl*enclosing_decl){
struct _tuple14*ad=Cyc_Cifc_update_tvars(te,tv_ovrs,(_check_null(fi))->ret_type,enclosing_decl,0);
# 654
struct Cyc_List_List*argit=fi->args;
struct Cyc_List_List*added_tvars=(unsigned)ad?(*ad).f1: 0;
struct Cyc_List_List*removed_tvars=(unsigned)ad?(*ad).f2: 0;
int change=0;
while((unsigned)argit){
struct _tuple8*_stmttmpD=(struct _tuple8*)argit->hd;struct _tuple8*_Tmp0=_stmttmpD;void*_Tmp1;_Tmp1=(void**)& _Tmp0->f3;{void**at=(void**)_Tmp1;
struct _tuple14*ad=Cyc_Cifc_update_tvars(te,tv_ovrs,*at,enclosing_decl,0);
if((unsigned)ad){
added_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*ad).f1,added_tvars);
removed_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*ad).f2,removed_tvars);}
# 665
argit=argit->tl;}}
# 667
while((unsigned)removed_tvars){
change=1;{
struct Cyc_Absyn_Tvar*rtv=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
({struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete;})(fi->tvars,rtv);fi->tvars=_Tmp2;});;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3=_Tmp2;void*_Tmp4;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp3)->tag == Cyc_Core_Not_found){
# 675
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_tvar2string(rtv);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn(loc,({const char*_Tmp7="Removed tvar %s not found";_tag_fat(_Tmp7,sizeof(char),26U);}),_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL3;}else{_Tmp4=_Tmp3;{void*exn=_Tmp4;(int)_rethrow(exn);}}_LL3:;}}}
# 678
removed_tvars=removed_tvars->tl;}}
# 680
if((unsigned)added_tvars){
change=1;
({struct Cyc_List_List*_Tmp0=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(added_tvars,fi->tvars);fi->tvars=_Tmp0;});}
# 685
fi->effect=0;
return change;}
# 689
static struct Cyc_List_List*Cyc_Cifc_remove_cur(struct Cyc_List_List*head,struct Cyc_List_List*cur,struct Cyc_List_List*prev){
if((unsigned)prev)
prev->tl=(_check_null(cur))->tl;else{
# 693
head=(_check_null(cur))->tl;}
# 695
return head;}
# 698
static int Cyc_Cifc_decl_name_cmp(struct Cyc_Absyn_Decl*a,struct Cyc_Absyn_Decl*b){
struct _fat_ptr*na=Cyc_Absyn_decl_name(a);
struct _fat_ptr*nb=Cyc_Absyn_decl_name(b);
if(na == 0 ^ nb == 0)
return 1;
if((unsigned)na)
return Cyc_strcmp(*na,*_check_null(nb));
return 1;}
# 712
static struct Cyc_List_List*Cyc_Cifc_update_usages(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*in_tv_ovrs,struct Cyc_List_List*dsmm){
unsigned niter=0U;
struct Cyc_List_List*abs_decls=0;
int some_change=0;
struct Cyc_List_List*tv_ovrs=in_tv_ovrs;
struct Cyc_List_List*dsm=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_copy;})(dsmm);
struct Cyc_List_List*changed_decls=0;
do{
changed_decls=0;{
struct Cyc_List_List*ds=dsm;
struct Cyc_List_List*prev=0;
some_change=0;
while((unsigned)ds){
int changed=0;
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
struct _fat_ptr*dname=Cyc_Absyn_decl_name(d);
# 730
{void*_stmttmpE=d->r;void*_Tmp0=_stmttmpE;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 732
{void*_stmttmpF=vd->type;void*_Tmp2=_stmttmpF;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp2)){case 5: _Tmp4=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_FnInfo*fi=_Tmp4;
# 734
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,fi,d);
# 740
goto _LLB;}case 0: _Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2;{void*tc=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 742
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,vd->type,d,1)!= 0;
goto _LLB;}case 3: _Tmp4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1).elt_type;{void*et=_Tmp4;
_Tmp4=et;goto _LL13;}case 4: _Tmp4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1).elt_type;_LL13: {void*et=_Tmp4;
# 746
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,et,d,1)!= 0;
goto _LLB;}default:
# 749
 goto _LLB;}_LLB:;}
# 751
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 753
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,& fd->i,d);
# 757
goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 759
if((unsigned)ad->impl){
struct Cyc_List_List*fit=(_check_null(ad->impl))->fields;
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
while((unsigned)fit){
struct Cyc_Absyn_Aggrfield*fld=(struct Cyc_Absyn_Aggrfield*)fit->hd;
struct _tuple14*tvtup=Cyc_Cifc_update_tvars(te,tv_ovrs,fld->type,d,0);
if((unsigned)tvtup){
changed=1;{
struct Cyc_List_List*ad=(*tvtup).f1;
if((unsigned)ad)
added_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(ad,added_tvars);{
struct Cyc_List_List*rm=(*tvtup).f2;
if((unsigned)rm)
removed_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(rm,removed_tvars);}}}
# 775
fit=fit->tl;}
# 777
while((unsigned)removed_tvars){
struct Cyc_Absyn_Tvar*rtv=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;
{struct _handler_cons _Tmp2;_push_handler(& _Tmp2);{int _Tmp3=0;if(setjmp(_Tmp2.handler))_Tmp3=1;if(!_Tmp3){
({struct Cyc_List_List*_Tmp4=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete;})(ad->tvs,rtv);ad->tvs=_Tmp4;});;_pop_handler();}else{void*_Tmp4=(void*)Cyc_Core_get_exn_thrown();void*_Tmp5=_Tmp4;void*_Tmp6;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp5)->tag == Cyc_Core_Not_found){
# 784
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_tvar2string(rtv);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_warn(loc,({const char*_Tmp9="Removed tvar %s not found";_tag_fat(_Tmp9,sizeof(char),26U);}),_tag_fat(_Tmp8,sizeof(void*),1));});
goto _LL16;}else{_Tmp6=_Tmp5;{void*exn=_Tmp6;(int)_rethrow(exn);}}_LL16:;}}}
# 787
removed_tvars=removed_tvars->tl;}
# 789
if((unsigned)added_tvars){
({struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(added_tvars,ad->tvs);ad->tvs=_Tmp2;});{
struct Cyc_Absyn_Decl*absdecl=Cyc_Cifc_make_abstract_decl(d);
{struct _handler_cons _Tmp2;_push_handler(& _Tmp2);{int _Tmp3=0;if(setjmp(_Tmp2.handler))_Tmp3=1;if(!_Tmp3){
abs_decls=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_Absyn_Decl*))Cyc_List_delete_cmp;})(Cyc_Cifc_decl_name_cmp,abs_decls,absdecl);;_pop_handler();}else{void*_Tmp4=(void*)Cyc_Core_get_exn_thrown();void*_Tmp5=_Tmp4;void*_Tmp6;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp5)->tag == Cyc_Core_Not_found)
# 796
goto _LL1B;else{_Tmp6=_Tmp5;{void*exn=_Tmp6;(int)_rethrow(exn);}}_LL1B:;}}}
# 798
abs_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=absdecl,_Tmp2->tl=abs_decls;_Tmp2;});}}
# 800
if(changed)
changed_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=changed_decls;_Tmp2;});}else{
# 804
struct _tuple0*_stmttmp10=ad->name;struct _tuple0*_Tmp2=_stmttmp10;void*_Tmp3;_Tmp3=_Tmp2->f2;{struct _fat_ptr*name=_Tmp3;
struct Cyc_Absyn_Decl*ovd=Cyc_Absyn_lookup_decl(tv_ovrs,name);
if((unsigned)ovd){
changed=1;{
void*_stmttmp11=ovd->r;void*_Tmp4=_stmttmp11;void*_Tmp5;if(*((int*)_Tmp4)== 5){_Tmp5=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Aggrdecl*ovd_ad=_Tmp5;
# 810
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*tvs=ovd_ad->tvs;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
struct Cyc_Absyn_Tvar*tvcpy=({struct Cyc_Absyn_Tvar*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));*_Tmp6=*tv;_Tmp6;});
struct _fat_ptr*tvn=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="`ovr_%d";_tag_fat(_TmpA,sizeof(char),8U);}),_tag_fat(_Tmp9,sizeof(void*),1));});*_Tmp6=_Tmp7;});_Tmp6;});
tvcpy->name=tvn;
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=tvcpy,_Tmp7->tl=ad->tvs;_Tmp7;});ad->tvs=_Tmp6;});
tvs=tvs->tl;}
# 820
({struct Cyc_List_List*_Tmp6=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(ad->tvs);ad->tvs=_Tmp6;});
goto _LL23;}}else{
# 823
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp6="ovd must be an aggr type";_tag_fat(_Tmp6,sizeof(char),25U);}),_tag_fat(0U,sizeof(void*),0));}_LL23:;}}}}
# 829
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 831
goto _LL0;}default:
# 833
 goto _LL0;}_LL0:;}
# 835
prev=ds;
ds=ds->tl;
if(changed)
some_change=1;}
# 840
tv_ovrs=changed_decls;}}while(
some_change && ++ niter < 10U);
if(niter >= 10U){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=Cyc_Absynpp_decllist2string(changed_decls);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Suspected mutually recursive structs involving the following decls \n %s";_tag_fat(_Tmp2,sizeof(char),72U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="Suspected mutually recursive structs -- abandoning cifc";_tag_fat(_Tmp2,sizeof(char),56U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});}
# 847
return abs_decls;}
# 850
static void Cyc_Cifc_i_clear_vartype_ids(void*t){
void*_Tmp0=t;void*_Tmp1;struct Cyc_Absyn_FnInfo _Tmp2;struct Cyc_Absyn_ArrayInfo _Tmp3;struct Cyc_Absyn_PtrInfo _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 2: _Tmp5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp5;
# 853
tv->identity=-1;
goto _LL0;}case 0: _Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;
# 856
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 860
goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo pi=_Tmp4;
# 862
Cyc_Cifc_i_clear_vartype_ids(pi.elt_type);
goto _LL0;}case 4: _Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp3;
# 865
Cyc_Cifc_i_clear_vartype_ids(ai.elt_type);
goto _LL0;}case 5: _Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp2;
# 868
Cyc_Cifc_i_clear_vartype_ids(fi.ret_type);{
struct Cyc_List_List*argit=fi.args;
while((unsigned)argit){
struct _tuple8*_stmttmp12=(struct _tuple8*)argit->hd;struct _tuple8*_Tmp6=_stmttmp12;void*_Tmp7;_Tmp7=_Tmp6->f3;{void*at=_Tmp7;
Cyc_Cifc_i_clear_vartype_ids(at);
argit=argit->tl;}}
# 875
goto _LL0;}}case 6: _Tmp5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*ts=_Tmp5;
# 877
while((unsigned)ts){
struct _tuple11*_stmttmp13=(struct _tuple11*)ts->hd;struct _tuple11*_Tmp6=_stmttmp13;void*_Tmp7;_Tmp7=_Tmp6->f2;{void*et=_Tmp7;
Cyc_Cifc_i_clear_vartype_ids(et);
ts=ts->tl;}}
# 882
goto _LL0;}case 8: _Tmp5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;_Tmp1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f4;{struct Cyc_List_List*ts=_Tmp5;void*to=_Tmp1;
# 887
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 891
goto _LL0;}default:
# 896
 goto _LL0;}_LL0:;}
# 900
static void Cyc_Cifc_clear_vartype_ids(struct Cyc_Absyn_Decl*d){
void*_stmttmp14=d->r;void*_Tmp0=_stmttmp14;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 903
Cyc_Cifc_i_clear_vartype_ids(vd->type);
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 906
Cyc_Cifc_i_clear_vartype_ids((fd->i).ret_type);{
struct Cyc_List_List*ai=(fd->i).args;
while((unsigned)ai){
Cyc_Cifc_i_clear_vartype_ids((*((struct _tuple8*)ai->hd)).f3);}
# 911
(fd->i).tvars=0;
goto _LL0;}}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 914
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 916
goto _LL0;}default:
# 919
 goto _LL0;}_LL0:;}
# 923
static int Cyc_Cifc_eq(void*a,void*b){
return a == b;}
# 927
static int Cyc_Cifc_decl_not_present(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*a){
return !({(int(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_Absyn_Decl*,struct Cyc_List_List*))Cyc_List_exists_c;})(({(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*))Cyc_Cifc_eq;}),a,l);}
# 932
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List**ds,struct Cyc_List_List*ovrs){
struct Cyc_List_List*type_overrides=0;
struct Cyc_List_List*overriden_decls=0;
struct Cyc_List_List*abs_decls=0;
{struct Cyc_List_List*x=ovrs;for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Decl*ud=(struct Cyc_Absyn_Decl*)x->hd;
struct _fat_ptr*un=Cyc_Absyn_decl_name(ud);
if(!((unsigned)un))Cyc_Warn_warn(ud->loc,({const char*_Tmp0="Overriding decl without a name\n";_tag_fat(_Tmp0,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));else{
# 941
struct Cyc_Absyn_Decl*d=Cyc_Absyn_lookup_decl(*_check_null(ds),un);
if(!((unsigned)d))({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=*un;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn(ud->loc,({const char*_Tmp2="%s is overridden but not defined";_tag_fat(_Tmp2,sizeof(char),33U);}),_tag_fat(_Tmp1,sizeof(void*),1));});else{
# 944
overriden_decls=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=d,_Tmp0->tl=overriden_decls;_Tmp0;});{
int pre_tvars_d=Cyc_Cifc_contains_type_vars(d);
Cyc_Cifc_merge_sys_user_decl(loc,te,0,ud,d);
Cyc_Cifc_clear_vartype_ids(ud);
if(Cyc_Cifc_contains_type_vars(ud)&& !pre_tvars_d){
abs_decls=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Cifc_make_abstract_decl(ud);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=abs_decls;_Tmp0;});
type_overrides=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=d,_Tmp0->tl=type_overrides;_Tmp0;});}}}}}}{
# 956
struct Cyc_List_List*unoverriden_decls=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_filter_c;})(Cyc_Cifc_decl_not_present,overriden_decls,*_check_null(ds));
abs_decls=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_Tmp1=abs_decls;_Tmp0(_Tmp1,
Cyc_Cifc_update_usages(loc,te,type_overrides,unoverriden_decls));});
if((unsigned)abs_decls)
({struct Cyc_List_List*_Tmp0=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(abs_decls,*ds);*ds=_Tmp0;});}}

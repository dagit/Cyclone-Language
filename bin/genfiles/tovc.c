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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 72 "list.h"
extern struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 69
extern struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*,struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];
# 141
extern struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*,struct Cyc_Set_Set*);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];
# 68 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 277 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 843 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 856
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 863
void*Cyc_Absyn_compress(void*);
# 894
extern void*Cyc_Absyn_void_type;
# 909
extern struct _tuple0*Cyc_Absyn_exn_name;
# 935
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 963
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 969
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
# 973
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 979
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1035
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
# 1037
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1048
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1061
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1126
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};
# 146 "tcutil.h"
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 41 "toc.h"
struct _tuple0*Cyc_Toc_temp_var (void);
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
# 73 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 101
static struct Cyc_Absyn_Exp*Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;void*_tmp4;int _tmp6;int _tmp5;void*_tmp3;void*_tmp2;switch(*((int*)_tmp1)){case 1: _tmp2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{void*b=_tmp2;
# 105
struct _tuple0*_tmp7=Cyc_Absyn_binding2qvar(b);struct _tuple0*_stmttmp1=_tmp7;struct _tuple0*_tmp8=_stmttmp1;void*_tmp9;if(((((struct _tuple0*)_tmp8)->f1).Loc_n).tag == 4){_tmp9=_tmp8->f2;{struct _fat_ptr*v=_tmp9;
# 109
if(({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(env.varmap,v)){
({void*_tmpD6=(({(struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(env.varmap,v))->r;e->r=_tmpD6;});
continue;}
# 114
{int i=0;for(0;(unsigned)i < _get_fat_size(Cyc_Toc_globals,sizeof(struct _fat_ptr*));++ i){
if(Cyc_strptrcmp(v,((struct _fat_ptr**)Cyc_Toc_globals.curr)[i])== 0)
return 0;}}
# 118
for(1;env.boundvars != 0;env.boundvars=(env.boundvars)->tl){
if(Cyc_strptrcmp(v,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)
return 0;}
# 122
for(1;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmpA=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;struct Cyc_Absyn_Vardecl*vd=_tmpA;
if(Cyc_strptrcmp(v,(*vd->name).f2)== 0){
({struct Cyc_Set_Set*_tmpD7=({(struct Cyc_Set_Set*(*)(struct _RegionHandle*,struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))Cyc_Set_rinsert;})(env.rgn,*env.freevars,vd);*env.freevars=_tmpD7;});{
# 127
void*_tmpB=vd->type;void*_stmttmp2=_tmpB;void*_tmpC=_stmttmp2;if(*((int*)_tmpC)== 4)
return 0;else{
return Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(vd->name,0U),0U);};}}}
# 136
for(1;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
if(Cyc_strptrcmp(v,(*((struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd)->name).f2)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC5;_tmpC5.tag=0,({struct _fat_ptr _tmpD8=({const char*_tmp12="unbound variable ";_tag_fat(_tmp12,sizeof(char),18U);});_tmpC5.f1=_tmpD8;});_tmpC5;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC4;_tmpC4.tag=0,_tmpC4.f1=*v;_tmpC4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC3;_tmpC3.tag=0,({struct _fat_ptr _tmpD9=({const char*_tmp11=" found in box_free_vars_exp";_tag_fat(_tmp11,sizeof(char),28U);});_tmpC3.f1=_tmpD9;});_tmpC3;});void*_tmpD[3];_tmpD[0]=& _tmpE,_tmpD[1]=& _tmpF,_tmpD[2]=& _tmp10;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmpD,sizeof(void*),3));});}
# 142
return 0;}}else{
return 0;};}case 23: _tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2;struct Cyc_Absyn_Exp*e2=_tmp3;
# 149
struct Cyc_Absyn_Exp*_tmp13=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp13;
struct Cyc_Absyn_Exp*_tmp14=Cyc_Tovc_box_free_vars_exp(env,e2);struct Cyc_Absyn_Exp*e2b=_tmp14;
if(e1b != 0){
if(e2b != 0){
struct Cyc_Absyn_Exp*_tmp15=Cyc_Absyn_subscript_exp(e1b,e2b,e->loc);struct Cyc_Absyn_Exp*eb=_tmp15;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}else{
# 159
struct Cyc_Absyn_Exp*_tmp16=Cyc_Absyn_subscript_exp(e1b,e2,e->loc);struct Cyc_Absyn_Exp*eb=_tmp16;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}}else{
# 164
if(e2b != 0){
struct Cyc_Absyn_Exp*_tmp17=Cyc_Absyn_subscript_exp(e1,e2b,e->loc);struct Cyc_Absyn_Exp*eb=_tmp17;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}}
# 170
return 0;}case 15: _tmp2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2;
# 173
struct Cyc_Absyn_Exp*_tmp18=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp18;
if(e1b != 0){
struct Cyc_Absyn_Exp*_tmp19=Cyc_Absyn_address_exp(e1b,e->loc);struct Cyc_Absyn_Exp*eb=_tmp19;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 180
return 0;}case 20: _tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2;
# 183
struct Cyc_Absyn_Exp*_tmp1A=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp1A;
if(e1b != 0){
struct Cyc_Absyn_Exp*_tmp1B=Cyc_Absyn_deref_exp(e1b,e->loc);struct Cyc_Absyn_Exp*eb=_tmp1B;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 190
return 0;}case 21: _tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2;struct _fat_ptr*fn=_tmp3;int ist=_tmp5;int isr=_tmp6;
# 193
struct Cyc_Absyn_Exp*_tmp1C=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp1C;
if(e1b != 0){
struct Cyc_Absyn_Exp*_tmp1D=({void*_tmpDA=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->tag=21,_tmp1E->f1=e1b,_tmp1E->f2=fn,_tmp1E->f3=ist,_tmp1E->f4=isr;_tmp1E;});Cyc_Absyn_new_exp(_tmpDA,e->loc);});struct Cyc_Absyn_Exp*eb=_tmp1D;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 200
return 0;}case 22: _tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2;struct _fat_ptr*fn=_tmp3;int ist=_tmp5;int isr=_tmp6;
# 203
struct Cyc_Absyn_Exp*_tmp1F=Cyc_Tovc_box_free_vars_exp(env,e1);struct Cyc_Absyn_Exp*e1b=_tmp1F;
if(e1b != 0){
struct Cyc_Absyn_Exp*_tmp20=({void*_tmpDB=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=22,_tmp21->f1=e1b,_tmp21->f2=fn,_tmp21->f3=ist,_tmp21->f4=isr;_tmp21;});Cyc_Absyn_new_exp(_tmpDB,e->loc);});struct Cyc_Absyn_Exp*eb=_tmp20;
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 210
return 0;}case 0:
# 213
 return 0;case 10: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f2;{struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_List_List*es=_tmp3;
# 215
{struct Cyc_Absyn_Exp*_tmp22=Cyc_Tovc_box_free_vars_exp(env,*e1);struct Cyc_Absyn_Exp*__ei=_tmp22;if(__ei != 0)*e1=__ei;}
_tmp2=es;goto _LL12;}case 3: _tmp2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL12: {struct Cyc_List_List*es=_tmp2;
# 218
for(1;es != 0;es=es->tl){
struct Cyc_Absyn_Exp*_tmp23=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)es->hd);struct Cyc_Absyn_Exp*__ei=_tmp23;if(__ei != 0)es->hd=(void*)__ei;}
# 221
return 0;}case 6: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f3;{struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=_tmp3;struct Cyc_Absyn_Exp**e3=(struct Cyc_Absyn_Exp**)_tmp4;
# 224
{struct Cyc_Absyn_Exp*_tmp24=Cyc_Tovc_box_free_vars_exp(env,*e3);struct Cyc_Absyn_Exp*__ei=_tmp24;if(__ei != 0)*e3=__ei;}
_tmp2=e1;_tmp3=e2;goto _LL16;}case 7: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL16: {struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=_tmp3;
_tmp2=e1;_tmp3=e2;goto _LL18;}case 8: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL18: {struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=_tmp3;
_tmp2=e1;_tmp3=e2;goto _LL1A;}case 9: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL1A: {struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=_tmp3;
_tmp2=e1;_tmp3=e2;goto _LL1C;}case 4: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL1C: {struct Cyc_Absyn_Exp**e1=_tmp2;struct Cyc_Absyn_Exp**e2=(struct Cyc_Absyn_Exp**)_tmp3;
# 230
{struct Cyc_Absyn_Exp*_tmp25=Cyc_Tovc_box_free_vars_exp(env,*e2);struct Cyc_Absyn_Exp*__ei=_tmp25;if(__ei != 0)*e2=__ei;}
_tmp2=e1;goto _LL1E;}case 12: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL1E: {struct Cyc_Absyn_Exp**e1=_tmp2;
# 233
_tmp2=e1;goto _LL20;}case 13: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL20: {struct Cyc_Absyn_Exp**e1=_tmp2;
_tmp2=e1;goto _LL22;}case 14: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL22: {struct Cyc_Absyn_Exp**e1=_tmp2;
_tmp2=e1;goto _LL24;}case 18: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL24: {struct Cyc_Absyn_Exp**e1=_tmp2;
_tmp2=e1;goto _LL26;}case 5: _tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL26: {struct Cyc_Absyn_Exp**e1=_tmp2;
# 238
{struct Cyc_Absyn_Exp*_tmp26=Cyc_Tovc_box_free_vars_exp(env,*e1);struct Cyc_Absyn_Exp*__ei=_tmp26;if(__ei != 0)*e1=__ei;}
return 0;}case 17:
# 241
 goto _LL2A;case 19: _LL2A:
 goto _LL2C;case 32: _LL2C:
 return 0;case 36: _tmp2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1)->f2;{struct Cyc_List_List*dles=_tmp2;
# 246
for(1;dles != 0;dles=dles->tl){
struct Cyc_Absyn_Exp*_tmp27=Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple11*)dles->hd)).f2);struct Cyc_Absyn_Exp*__ei=_tmp27;if(__ei != 0)(*((struct _tuple11*)dles->hd)).f2=__ei;}
# 249
return 0;}default:
# 251
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC6;_tmpC6.tag=0,({struct _fat_ptr _tmpDC=({const char*_tmp2A="bad exp form in Tocv::box_free_vars_exp";_tag_fat(_tmp2A,sizeof(char),40U);});_tmpC6.f1=_tmpDC;});_tmpC6;});void*_tmp28[1];_tmp28[0]=& _tmp29;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp28,sizeof(void*),1));});};}}
# 255
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 259
while(1){
void*_tmp2B=s->r;void*_stmttmp3=_tmp2B;void*_tmp2C=_stmttmp3;void*_tmp30;void*_tmp2F;void*_tmp2E;void*_tmp2D;switch(*((int*)_tmp2C)){case 0:
 return;case 6:
# 263
({void*_tmp31=0U;({unsigned _tmpDE=s->loc;struct _fat_ptr _tmpDD=({const char*_tmp32="break in eliminated statement expression";_tag_fat(_tmp32,sizeof(char),41U);});Cyc_Warn_warn(_tmpDE,_tmpDD,_tag_fat(_tmp31,sizeof(void*),0));});});
return;case 7:
# 266
({void*_tmp33=0U;({unsigned _tmpE0=s->loc;struct _fat_ptr _tmpDF=({const char*_tmp34="continue in eliminated statement expression";_tag_fat(_tmp34,sizeof(char),44U);});Cyc_Warn_warn(_tmpE0,_tmpDF,_tag_fat(_tmp33,sizeof(void*),0));});});
# 269
return;case 8:
# 271
({void*_tmp35=0U;({unsigned _tmpE2=s->loc;struct _fat_ptr _tmpE1=({const char*_tmp36="goto in eliminated statement expression";_tag_fat(_tmp36,sizeof(char),40U);});Cyc_Warn_warn(_tmpE2,_tmpE1,_tag_fat(_tmp35,sizeof(void*),0));});});
return;case 13: _tmp2D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp2D;
# 274
s=s2;
continue;}case 3: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;{struct Cyc_Absyn_Exp**eopt=_tmp2D;
# 277
({void*_tmp37=0U;({unsigned _tmpE4=s->loc;struct _fat_ptr _tmpE3=({const char*_tmp38="return in eliminated statement expression";_tag_fat(_tmp38,sizeof(char),42U);});Cyc_Warn_warn(_tmpE4,_tmpE3,_tag_fat(_tmp37,sizeof(void*),0));});});
# 279
if(*eopt != 0){
struct Cyc_Absyn_Exp*_tmp39=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(*eopt));struct Cyc_Absyn_Exp*ei=_tmp39;
if(ei != 0)*eopt=ei;}
# 283
return;}case 1: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;{struct Cyc_Absyn_Exp**e=_tmp2D;
# 285
{struct Cyc_Absyn_Exp*_tmp3A=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp3A;if(__ei != 0)*e=__ei;}
return;}case 2: _tmp2D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp2D;struct Cyc_Absyn_Stmt*s2=_tmp2E;
# 288
Cyc_Tovc_box_free_vars_stmt(env,s1);
s=s2;
continue;}case 4: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;_tmp2F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2C)->f3;{struct Cyc_Absyn_Exp**e=_tmp2D;struct Cyc_Absyn_Stmt*s1=_tmp2E;struct Cyc_Absyn_Stmt*s2=_tmp2F;
# 292
{struct Cyc_Absyn_Exp*_tmp3B=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp3B;if(__ei != 0)*e=__ei;}
Cyc_Tovc_box_free_vars_stmt(env,s1);
s=s2;
continue;}case 5: _tmp2D=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1).f1;_tmp2E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Exp**e=_tmp2D;struct Cyc_Absyn_Stmt*s2=_tmp2E;
# 297
{struct Cyc_Absyn_Exp*_tmp3C=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp3C;if(__ei != 0)*e=__ei;}
s=s2;
continue;}case 9: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2).f1;_tmp2F=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2C)->f3).f1;_tmp30=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2C)->f4;{struct Cyc_Absyn_Exp**e1=_tmp2D;struct Cyc_Absyn_Exp**e2=_tmp2E;struct Cyc_Absyn_Exp**e3=(struct Cyc_Absyn_Exp**)_tmp2F;struct Cyc_Absyn_Stmt*s2=_tmp30;
# 301
{struct Cyc_Absyn_Exp*_tmp3D=Cyc_Tovc_box_free_vars_exp(env,*e1);struct Cyc_Absyn_Exp*__ei=_tmp3D;if(__ei != 0)*e1=__ei;}
{struct Cyc_Absyn_Exp*_tmp3E=Cyc_Tovc_box_free_vars_exp(env,*e2);struct Cyc_Absyn_Exp*__ei=_tmp3E;if(__ei != 0)*e2=__ei;}
{struct Cyc_Absyn_Exp*_tmp3F=Cyc_Tovc_box_free_vars_exp(env,*e3);struct Cyc_Absyn_Exp*__ei=_tmp3F;if(__ei != 0)*e3=__ei;}
s=s2;
continue;}case 14: _tmp2D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2).f1;{struct Cyc_Absyn_Stmt*s2=_tmp2D;struct Cyc_Absyn_Exp**e=_tmp2E;
# 307
{struct Cyc_Absyn_Exp*_tmp40=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp40;if(__ei != 0)*e=__ei;}
s=s2;
continue;}case 10: _tmp2D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Exp**e=_tmp2D;struct Cyc_List_List*scs=_tmp2E;
# 312
{struct Cyc_Absyn_Exp*_tmp41=Cyc_Tovc_box_free_vars_exp(env,*e);struct Cyc_Absyn_Exp*__ei=_tmp41;if(__ei != 0)*e=__ei;}
for(1;scs != 0;scs=scs->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _tmp2D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2C)->f2;{struct Cyc_Absyn_Decl*d=_tmp2D;struct Cyc_Absyn_Stmt*s2=_tmp2E;
# 317
{void*_tmp42=d->r;void*_stmttmp4=_tmp42;void*_tmp43=_stmttmp4;void*_tmp44;if(*((int*)_tmp43)== 0){_tmp44=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp43)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp44;
# 319
({struct Cyc_List_List*_tmpE5=({struct Cyc_List_List*_tmp45=_region_malloc(env.rgn,sizeof(*_tmp45));_tmp45->hd=vd,_tmp45->tl=env.boundvars;_tmp45;});env.boundvars=_tmpE5;});
({struct Cyc_Dict_Dict _tmpE6=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_rdelete_same;})(env.varmap,(*vd->name).f2);env.varmap=_tmpE6;});
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*_tmp46=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));struct Cyc_Absyn_Exp*ei=_tmp46;
if(ei != 0)vd->initializer=ei;}
# 325
s=s2;
continue;}}else{
goto _LL1F;}_LL1F:;}
# 329
goto _LL1E;}default: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp48=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC7;_tmpC7.tag=0,({struct _fat_ptr _tmpE7=({const char*_tmp49="bad stmt after xlation to C";_tag_fat(_tmp49,sizeof(char),28U);});_tmpC7.f1=_tmpE7;});_tmpC7;});void*_tmp47[1];_tmp47[0]=& _tmp48;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp47,sizeof(void*),1));});};}}
# 340
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 344
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 347
struct Cyc_Set_Set*freevars=({(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty;})(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp4A=({struct Cyc_Tovc_BoxingEnv _tmpC8;_tmpC8.rgn=env.rgn,_tmpC8.all_locals=env.all_locals,_tmpC8.varmap=env.varmap,_tmpC8.boundvars=0,_tmpC8.freevars=& freevars;_tmpC8;});struct Cyc_Tovc_BoxingEnv boxenv=_tmp4A;
Cyc_Tovc_box_free_vars_stmt(boxenv,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 353
struct Cyc_Iter_Iter iter=({(struct Cyc_Iter_Iter(*)(struct _RegionHandle*,struct Cyc_Set_Set*))Cyc_Set_make_iter;})(Cyc_Core_heap_region,freevars);
# 355
struct Cyc_Absyn_Vardecl*_tmp4B=({struct Cyc_Absyn_Vardecl*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->sc=2U,_tmp58->name=Cyc_Absyn_exn_name,_tmp58->varloc=0U,({struct Cyc_Absyn_Tqual _tmpE8=Cyc_Absyn_empty_tqual(0U);_tmp58->tq=_tmpE8;}),_tmp58->type=Cyc_Absyn_void_type,_tmp58->initializer=0,_tmp58->rgn=0,_tmp58->attributes=0,_tmp58->escapes=0,_tmp58->is_proto=0;_tmp58;});struct Cyc_Absyn_Vardecl*vd=_tmp4B;
# 358
while(({(int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next;})(iter,& vd)){
void*_tmp4C=vd->type;void*_stmttmp5=_tmp4C;void*_tmp4D=_stmttmp5;void*_tmp4E;if(*((int*)_tmp4D)== 4){_tmp4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D)->f1).elt_type;{void*elt_type=_tmp4E;
# 361
params=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _tuple8*_tmpEB=({struct _tuple8*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=(*vd->name).f2,_tmp4F->f2=vd->tq,({
void*_tmpEA=({void*_tmpE9=elt_type;Cyc_Absyn_cstar_type(_tmpE9,Cyc_Absyn_empty_tqual(0U));});_tmp4F->f3=_tmpEA;});_tmp4F;});
# 361
_tmp50->hd=_tmpEB;}),_tmp50->tl=params;_tmp50;});
# 364
args=({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));({struct Cyc_Absyn_Exp*_tmpEC=Cyc_Absyn_var_exp(vd->name,0U);_tmp51->hd=_tmpEC;}),_tmp51->tl=args;_tmp51;});
goto _LL0;}}else{
# 367
params=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));({struct _tuple8*_tmpEF=({struct _tuple8*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->f1=(*vd->name).f2,_tmp52->f2=vd->tq,({
void*_tmpEE=({void*_tmpED=vd->type;Cyc_Absyn_cstar_type(_tmpED,Cyc_Absyn_empty_tqual(0U));});_tmp52->f3=_tmpEE;});_tmp52;});
# 367
_tmp53->hd=_tmpEF;}),_tmp53->tl=params;_tmp53;});
# 370
args=({struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));({struct Cyc_Absyn_Exp*_tmpF0=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(vd->name,0U),0U);_tmp54->hd=_tmpF0;}),_tmp54->tl=args;_tmp54;});
goto _LL0;}_LL0:;}{
# 373
struct _tuple0*funname=Cyc_Toc_temp_var();
({struct Cyc_List_List*_tmpF4=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));
({struct Cyc_Absyn_Decl*_tmpF3=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=1,({struct Cyc_Absyn_Fndecl*_tmpF2=({struct Cyc_Absyn_Fndecl*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->sc=0U,_tmp55->is_inline=0,_tmp55->name=funname,_tmp55->body=s,
(_tmp55->i).tvars=0,(_tmp55->i).effect=0,({struct Cyc_Absyn_Tqual _tmpF1=Cyc_Absyn_empty_tqual(0U);(_tmp55->i).ret_tqual=_tmpF1;}),(_tmp55->i).ret_type=rettype,(_tmp55->i).args=params,(_tmp55->i).c_varargs=0,(_tmp55->i).cyc_varargs=0,(_tmp55->i).rgn_po=0,(_tmp55->i).attributes=0,(_tmp55->i).requires_clause=0,(_tmp55->i).requires_relns=0,(_tmp55->i).ensures_clause=0,(_tmp55->i).ensures_relns=0,(_tmp55->i).return_value=0,_tmp55->cached_type=0,_tmp55->param_vardecls=0,_tmp55->fn_vardecl=0,_tmp55->orig_scope=0U;_tmp55;});
# 375
_tmp56->f1=_tmpF2;});_tmp56;}),0U);_tmp57->hd=_tmpF3;}),_tmp57->tl=*env.gen_ds;_tmp57;});
# 374
*env.gen_ds=_tmpF4;});
# 382
return({struct Cyc_Absyn_Exp*_tmpF5=Cyc_Absyn_var_exp(funname,0U);Cyc_Absyn_fncall_exp(_tmpF5,args,0U);});}}}
# 385
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_tmp59=e->r;void*_stmttmp6=_tmp59;void*_tmp5A=_stmttmp6;void*_tmp5D;void*_tmp5C;void*_tmp5B;switch(*((int*)_tmp5A)){case 1: _tmp5B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;{void*b=_tmp5B;
# 388
struct _tuple0*_tmp5E=Cyc_Absyn_binding2qvar(b);struct _tuple0*_stmttmp7=_tmp5E;struct _tuple0*_tmp5F=_stmttmp7;void*_tmp60;if(((((struct _tuple0*)_tmp5F)->f1).Loc_n).tag == 4){_tmp60=_tmp5F->f2;{struct _fat_ptr*v=_tmp60;
# 390
if(({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(varmap,v))
({void*_tmpF6=(({(struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(varmap,v))->r;e->r=_tmpF6;});
return;}}else{
return;};}case 0:
# 397
 return;case 10: _tmp5B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp5C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;{struct Cyc_Absyn_Exp*e1=_tmp5B;struct Cyc_List_List*es=_tmp5C;
# 399
Cyc_Tovc_apply_varmap(varmap,e1);_tmp5B=es;goto _LL8;}case 3: _tmp5B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL8: {struct Cyc_List_List*es=_tmp5B;
# 401
for(1;es != 0;es=es->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 6: _tmp5B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp5C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_tmp5D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;{struct Cyc_Absyn_Exp*e1=_tmp5B;struct Cyc_Absyn_Exp*e2=_tmp5C;struct Cyc_Absyn_Exp*e3=_tmp5D;
# 406
Cyc_Tovc_apply_varmap(varmap,e3);_tmp5B=e1;_tmp5C=e2;goto _LLC;}case 7: _tmp5B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp5C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp5B;struct Cyc_Absyn_Exp*e2=_tmp5C;
_tmp5B=e1;_tmp5C=e2;goto _LLE;}case 8: _tmp5B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp5C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp5B;struct Cyc_Absyn_Exp*e2=_tmp5C;
_tmp5B=e1;_tmp5C=e2;goto _LL10;}case 23: _tmp5B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp5C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp5B;struct Cyc_Absyn_Exp*e2=_tmp5C;
_tmp5B=e1;_tmp5C=e2;goto _LL12;}case 9: _tmp5B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp5C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5B;struct Cyc_Absyn_Exp*e2=_tmp5C;
_tmp5B=e1;_tmp5C=e2;goto _LL14;}case 4: _tmp5B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp5C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5B;struct Cyc_Absyn_Exp*e2=_tmp5C;
# 412
Cyc_Tovc_apply_varmap(varmap,e2);_tmp5B=e1;goto _LL16;}case 12: _tmp5B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp5B;
# 414
_tmp5B=e1;goto _LL18;}case 13: _tmp5B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp5B;
_tmp5B=e1;goto _LL1A;}case 14: _tmp5B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp5B;
_tmp5B=e1;goto _LL1C;}case 15: _tmp5B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp5B;
_tmp5B=e1;goto _LL1E;}case 18: _tmp5B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp5B;
_tmp5B=e1;goto _LL20;}case 20: _tmp5B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp5B;
_tmp5B=e1;goto _LL22;}case 21: _tmp5B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp5B;
_tmp5B=e1;goto _LL24;}case 22: _tmp5B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp5B;
_tmp5B=e1;goto _LL26;}case 5: _tmp5B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp5B;
# 423
Cyc_Tovc_apply_varmap(varmap,e1);
return;}case 17:
# 426
 goto _LL2A;case 19: _LL2A:
 goto _LL2C;case 32: _LL2C:
 return;case 36: _tmp5B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;{struct Cyc_List_List*dles=_tmp5B;
# 431
for(1;dles != 0;dles=dles->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple11*)dles->hd)).f2);}
return;}default:
# 435
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC9;_tmpC9.tag=0,({struct _fat_ptr _tmpF7=({const char*_tmp63="bad exp form in Tocv::apply_varmap";_tag_fat(_tmp63,sizeof(char),35U);});_tmpC9.f1=_tmpF7;});_tmpC9;});void*_tmp61[1];_tmp61[0]=& _tmp62;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp61,sizeof(void*),1));});};}
# 439
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_tmp64=s->r;void*_stmttmp8=_tmp64;void*_tmp65=_stmttmp8;void*_tmp68;void*_tmp67;void*_tmp66;switch(*((int*)_tmp65)){case 0:
 return Cyc_Absyn_true_exp(0U);case 1: _tmp66=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp65)->f1;{struct Cyc_Absyn_Exp*e=_tmp66;
# 443
Cyc_Tovc_apply_varmap(env.varmap,e);
return e;}case 2: _tmp66=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp65)->f1;_tmp67=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp65)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp66;struct Cyc_Absyn_Stmt*s2=_tmp67;
# 447
struct Cyc_Absyn_Exp*_tmp69=Cyc_Tovc_stmt_to_exp(env,s1);struct Cyc_Absyn_Exp*s1_new=_tmp69;
struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tovc_stmt_to_exp(env,s2);struct Cyc_Absyn_Exp*s2_new=_tmp6A;
return Cyc_Absyn_seq_exp(s1_new,s2_new,0U);}case 4: _tmp66=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp65)->f1;_tmp67=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp65)->f2;_tmp68=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp65)->f3;{struct Cyc_Absyn_Exp*e=_tmp66;struct Cyc_Absyn_Stmt*s1=_tmp67;struct Cyc_Absyn_Stmt*s2=_tmp68;
# 451
Cyc_Tovc_apply_varmap(env.varmap,e);{
# 453
struct Cyc_Absyn_Exp*_tmp6B=Cyc_Tovc_stmt_to_exp(env,s1);struct Cyc_Absyn_Exp*s1_new=_tmp6B;
struct Cyc_Absyn_Exp*_tmp6C=Cyc_Tovc_stmt_to_exp(env,s2);struct Cyc_Absyn_Exp*s2_new=_tmp6C;
return Cyc_Absyn_conditional_exp(e,s1_new,s2_new,0U);}}case 12: _tmp66=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp65)->f1;_tmp67=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp65)->f2;{struct Cyc_Absyn_Decl*d=_tmp66;struct Cyc_Absyn_Stmt*s2=_tmp67;
# 457
void*_tmp6D=d->r;void*_stmttmp9=_tmp6D;void*_tmp6E=_stmttmp9;void*_tmp6F;if(*((int*)_tmp6E)== 0){_tmp6F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp6F;
# 460
struct _fat_ptr*_tmp70=(*vd->name).f2;struct _fat_ptr*oldname=_tmp70;
struct _tuple0*_tmp71=Cyc_Toc_temp_var();struct _tuple0*newqvar=_tmp71;
vd->name=newqvar;
({struct Cyc_Dict_Dict _tmpFB=({struct Cyc_Dict_Dict(*_tmpFA)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Exp*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Exp*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _tmpF9=env.varmap;struct _fat_ptr*_tmpF8=oldname;_tmpFA(_tmpF9,_tmpF8,Cyc_Absyn_var_exp(newqvar,0U));});env.varmap=_tmpFB;});{
struct Cyc_List_List*_tmp72=0;struct Cyc_List_List*unresolvedmem_inits=_tmp72;
if(vd->initializer != 0){
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));{
void*_tmp73=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmpA=_tmp73;void*_tmp74=_stmttmpA;void*_tmp75;if(*((int*)_tmp74)== 36){_tmp75=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp74)->f2;{struct Cyc_List_List*dles=_tmp75;
# 479 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmp76=Cyc_Absyn_var_exp(vd->name,0U);struct Cyc_Absyn_Exp*var=_tmp76;
{unsigned i=0U;for(0;dles != 0;(dles=dles->tl,++ i)){
unresolvedmem_inits=({struct Cyc_List_List*_tmp77=_cycalloc(sizeof(*_tmp77));({struct Cyc_Absyn_Exp*_tmpFE=({struct Cyc_Absyn_Exp*_tmpFD=({struct Cyc_Absyn_Exp*_tmpFC=var;Cyc_Absyn_subscript_exp(_tmpFC,Cyc_Absyn_uint_exp(i,0U),0U);});Cyc_Absyn_assign_exp(_tmpFD,(*((struct _tuple11*)dles->hd)).f2,0U);});_tmp77->hd=_tmpFE;}),_tmp77->tl=unresolvedmem_inits;_tmp77;});}}
# 485
vd->initializer=0;
goto _LL12;}}else{
goto _LL12;}_LL12:;}}
# 490
({struct Cyc_List_List*_tmpFF=({struct Cyc_List_List*_tmp78=_region_malloc(env.rgn,sizeof(*_tmp78));_tmp78->hd=vd,_tmp78->tl=env.all_locals;_tmp78;});env.all_locals=_tmpFF;});
# 492
({void*_tmp101=({struct Cyc_Absyn_Decl*_tmp100=d;Cyc_Absyn_decl_stmt(_tmp100,Cyc_Absyn_new_stmt((env.encloser)->r,0U),0U);})->r;(env.encloser)->r=_tmp101;});{
struct Cyc_Absyn_Exp*_tmp79=Cyc_Tovc_stmt_to_exp(env,s2);struct Cyc_Absyn_Exp*e=_tmp79;
if(vd->initializer != 0){
e=({struct Cyc_Absyn_Exp*_tmp103=({struct Cyc_Absyn_Exp*_tmp102=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_assign_exp(_tmp102,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),0U);});Cyc_Absyn_seq_exp(_tmp103,e,0U);});
# 497
vd->initializer=0;}
# 499
for(1;unresolvedmem_inits != 0;unresolvedmem_inits=unresolvedmem_inits->tl){
e=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)unresolvedmem_inits->hd,e,0U);}
return e;}}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCA;_tmpCA.tag=0,({struct _fat_ptr _tmp104=({const char*_tmp7C="bad local decl in Tovc::stmt_to_exp";_tag_fat(_tmp7C,sizeof(char),36U);});_tmpCA.f1=_tmp104;});_tmpCA;});void*_tmp7A[1];_tmp7A[0]=& _tmp7B;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp7A,sizeof(void*),1));});};}default:
# 505
 return Cyc_Tovc_stmt_to_fun(env,s,Cyc_Absyn_void_type);};}
# 509
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv,struct Cyc_Absyn_Stmt*);
# 511
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
struct _fat_ptr bad_form=({const char*_tmp9B="";_tag_fat(_tmp9B,sizeof(char),1U);});
{void*_tmp7D=e->r;void*_stmttmpB=_tmp7D;void*_tmp7E=_stmttmpB;void*_tmp81;void*_tmp80;void*_tmp7F;switch(*((int*)_tmp7E)){case 0:
 return;case 1:
 return;case 10: _tmp7F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;{struct Cyc_Absyn_Exp*e1=_tmp7F;struct Cyc_List_List*es=_tmp80;
# 518
Cyc_Tovc_exp_to_vc(env,e1);_tmp7F=es;goto _LL8;}case 3: _tmp7F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;_LL8: {struct Cyc_List_List*es=_tmp7F;
# 520
for(1;es != 0;es=es->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 6: _tmp7F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;_tmp81=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E)->f3;{struct Cyc_Absyn_Exp*e1=_tmp7F;struct Cyc_Absyn_Exp*e2=_tmp80;struct Cyc_Absyn_Exp*e3=_tmp81;
# 525
Cyc_Tovc_exp_to_vc(env,e3);_tmp7F=e1;_tmp80=e2;goto _LLC;}case 7: _tmp7F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp7F;struct Cyc_Absyn_Exp*e2=_tmp80;
_tmp7F=e1;_tmp80=e2;goto _LLE;}case 8: _tmp7F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp7F;struct Cyc_Absyn_Exp*e2=_tmp80;
_tmp7F=e1;_tmp80=e2;goto _LL10;}case 23: _tmp7F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp7F;struct Cyc_Absyn_Exp*e2=_tmp80;
_tmp7F=e1;_tmp80=e2;goto _LL12;}case 9: _tmp7F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp7F;struct Cyc_Absyn_Exp*e2=_tmp80;
_tmp7F=e1;_tmp80=e2;goto _LL14;}case 4: _tmp7F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_tmp80=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7E)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp7F;struct Cyc_Absyn_Exp*e2=_tmp80;
# 531
Cyc_Tovc_exp_to_vc(env,e2);_tmp7F=e1;goto _LL16;}case 41: _tmp7F=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp7F;
# 533
_tmp7F=e1;goto _LL18;}case 12: _tmp7F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7F=e1;goto _LL1A;}case 13: _tmp7F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7F=e1;goto _LL1C;}case 14: _tmp7F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7F=e1;goto _LL1E;}case 15: _tmp7F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7F=e1;goto _LL20;}case 18: _tmp7F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7F=e1;goto _LL22;}case 20: _tmp7F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7F=e1;goto _LL24;}case 21: _tmp7F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7F=e1;goto _LL26;}case 22: _tmp7F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp7F;
_tmp7F=e1;goto _LL28;}case 5: _tmp7F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp7F;
# 543
Cyc_Tovc_exp_to_vc(env,e1);
return;}case 17:
# 546
 goto _LL2C;case 19: _LL2C:
 goto _LL2E;case 33: _LL2E:
 goto _LL30;case 32: _LL30:
 return;case 36: _tmp7F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp7E)->f2;{struct Cyc_List_List*dles=_tmp7F;
# 555
for(1;dles != 0;dles=dles->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}case 37: _tmp7F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp7E)->f1;{struct Cyc_Absyn_Stmt*s=_tmp7F;
# 563
Cyc_Tovc_stmt_to_vc(env,s);
# 565
({void*_tmp105=(Cyc_Tovc_stmt_to_exp(env,s))->r;e->r=_tmp105;});
return;}case 40:
# 568
 return;case 42:
# 570
 bad_form=({const char*_tmp82="Assert_e";_tag_fat(_tmp82,sizeof(char),9U);});goto _LL0;case 2:
 bad_form=({const char*_tmp83="Pragma_e";_tag_fat(_tmp83,sizeof(char),9U);});goto _LL0;case 11:
 bad_form=({const char*_tmp84="Throw_e";_tag_fat(_tmp84,sizeof(char),8U);});goto _LL0;case 16:
 bad_form=({const char*_tmp85="New_e";_tag_fat(_tmp85,sizeof(char),6U);});goto _LL0;case 24:
 bad_form=({const char*_tmp86="Tuple_e";_tag_fat(_tmp86,sizeof(char),8U);});goto _LL0;case 25:
 bad_form=({const char*_tmp87="CompoundLit_e";_tag_fat(_tmp87,sizeof(char),14U);});goto _LL0;case 26:
 bad_form=({const char*_tmp88="Array_e";_tag_fat(_tmp88,sizeof(char),8U);});goto _LL0;case 27:
 bad_form=({const char*_tmp89="Comprehension_e";_tag_fat(_tmp89,sizeof(char),16U);});goto _LL0;case 28:
 bad_form=({const char*_tmp8A="ComprehensionNoinit_e";_tag_fat(_tmp8A,sizeof(char),22U);});goto _LL0;case 29:
 bad_form=({const char*_tmp8B="Aggregate_e";_tag_fat(_tmp8B,sizeof(char),12U);});goto _LL0;case 30:
 bad_form=({const char*_tmp8C="AnonStruct_e";_tag_fat(_tmp8C,sizeof(char),13U);});goto _LL0;case 31:
# 582
 bad_form=({const char*_tmp8D="Datatype_e";_tag_fat(_tmp8D,sizeof(char),11U);});goto _LL0;case 34:
 bad_form=({const char*_tmp8E="Malloc_e";_tag_fat(_tmp8E,sizeof(char),9U);});goto _LL0;case 35:
 bad_form=({const char*_tmp8F="Swap_e";_tag_fat(_tmp8F,sizeof(char),7U);});goto _LL0;case 38:
 bad_form=({const char*_tmp90="Tagcheck_e";_tag_fat(_tmp90,sizeof(char),11U);});goto _LL0;default:
 bad_form=({const char*_tmp91="Valueof_e";_tag_fat(_tmp91,sizeof(char),10U);});goto _LL0;}_LL0:;}
# 588
({struct Cyc_Warn_String_Warn_Warg_struct _tmp93=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCF;_tmpCF.tag=0,({struct _fat_ptr _tmp106=({const char*_tmp9A="bad exp form ";_tag_fat(_tmp9A,sizeof(char),14U);});_tmpCF.f1=_tmp106;});_tmpCF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp94=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCE;_tmpCE.tag=0,_tmpCE.f1=bad_form;_tmpCE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp95=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCD;_tmpCD.tag=0,({struct _fat_ptr _tmp107=({const char*_tmp99="(exp=|";_tag_fat(_tmp99,sizeof(char),7U);});_tmpCD.f1=_tmp107;});_tmpCD;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp96=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmpCC;_tmpCC.tag=4,_tmpCC.f1=e;_tmpCC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp97=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCB;_tmpCB.tag=0,({struct _fat_ptr _tmp108=({const char*_tmp98="|) after xlation to C";_tag_fat(_tmp98,sizeof(char),22U);});_tmpCB.f1=_tmp108;});_tmpCB;});void*_tmp92[5];_tmp92[0]=& _tmp93,_tmp92[1]=& _tmp94,_tmp92[2]=& _tmp95,_tmp92[3]=& _tmp96,_tmp92[4]=& _tmp97;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp92,sizeof(void*),5));});}
# 591
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 593
while(1){
env.encloser=s;{
void*_tmp9C=s->r;void*_stmttmpC=_tmp9C;void*_tmp9D=_stmttmpC;void*_tmpA1;void*_tmpA0;void*_tmp9F;void*_tmp9E;switch(*((int*)_tmp9D)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 13: _tmp9E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9D)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp9E;
# 601
s=s2;
continue;}case 3: _tmp9E=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp9E;
# 604
if(eopt == 0)
return;
_tmp9E=eopt;goto _LLE;}case 1: _tmp9E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp9E;
# 608
Cyc_Tovc_exp_to_vc(env,e);
return;}case 2: _tmp9E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1;_tmp9F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9D)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp9E;struct Cyc_Absyn_Stmt*s2=_tmp9F;
# 611
Cyc_Tovc_stmt_to_vc(env,s1);
s=s2;
continue;}case 4: _tmp9E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1;_tmp9F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9D)->f2;_tmpA0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9D)->f3;{struct Cyc_Absyn_Exp*e=_tmp9E;struct Cyc_Absyn_Stmt*s1=_tmp9F;struct Cyc_Absyn_Stmt*s2=_tmpA0;
# 615
Cyc_Tovc_exp_to_vc(env,e);
Cyc_Tovc_stmt_to_vc(env,s1);
s=s2;
continue;}case 5: _tmp9E=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1).f1;_tmp9F=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9D)->f2;{struct Cyc_Absyn_Exp*e=_tmp9E;struct Cyc_Absyn_Stmt*s2=_tmp9F;
# 620
Cyc_Tovc_exp_to_vc(env,e);
s=s2;
continue;}case 9: _tmp9E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1;_tmp9F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9D)->f2).f1;_tmpA0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9D)->f3).f1;_tmpA1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9D)->f4;{struct Cyc_Absyn_Exp*e1=_tmp9E;struct Cyc_Absyn_Exp*e2=_tmp9F;struct Cyc_Absyn_Exp*e3=_tmpA0;struct Cyc_Absyn_Stmt*s2=_tmpA1;
# 624
Cyc_Tovc_exp_to_vc(env,e1);
Cyc_Tovc_exp_to_vc(env,e2);
Cyc_Tovc_exp_to_vc(env,e3);
s=s2;
continue;}case 14: _tmp9E=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1;_tmp9F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9D)->f2).f1;{struct Cyc_Absyn_Stmt*s2=_tmp9E;struct Cyc_Absyn_Exp*e=_tmp9F;
# 630
Cyc_Tovc_exp_to_vc(env,e);
s=s2;
continue;}case 10: _tmp9E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1;_tmp9F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9D)->f2;{struct Cyc_Absyn_Exp*e=_tmp9E;struct Cyc_List_List*scs=_tmp9F;
# 636
Cyc_Tovc_exp_to_vc(env,e);
for(1;scs != 0;scs=scs->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _tmp9E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9D)->f1;_tmp9F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9D)->f2;{struct Cyc_Absyn_Decl*d=_tmp9E;struct Cyc_Absyn_Stmt*s2=_tmp9F;
# 641
{void*_tmpA2=d->r;void*_stmttmpD=_tmpA2;void*_tmpA3=_stmttmpD;void*_tmpA4;if(*((int*)_tmpA3)== 0){_tmpA4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA3)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpA4;
# 643
if(Cyc_Tovc_elim_array_initializers){
# 648
void*_tmpA5=Cyc_Absyn_compress(vd->type);void*_stmttmpE=_tmpA5;void*_tmpA6=_stmttmpE;if(*((int*)_tmpA6)== 4){
# 650
if(vd->initializer != 0){
void*_tmpA7=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmpF=_tmpA7;void*_tmpA8=_stmttmpF;void*_tmpA9;if(*((int*)_tmpA8)== 36){_tmpA9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA8)->f2;{struct Cyc_List_List*dles=_tmpA9;
# 653
vd->initializer=0;{
# 655
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(vd->name,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(s2->r,0U);
{unsigned i=0U;for(0;dles != 0;(dles=dles->tl,++ i)){
s3=({struct Cyc_Absyn_Stmt*_tmp10B=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp10A=({struct Cyc_Absyn_Exp*_tmp109=arr;Cyc_Absyn_subscript_exp(_tmp109,
Cyc_Absyn_uint_exp(i,0U),0U);});
# 658
Cyc_Absyn_assign_exp(_tmp10A,(*((struct _tuple11*)dles->hd)).f2,0U);}),0U);Cyc_Absyn_seq_stmt(_tmp10B,s3,0U);});}}
# 663
s2->r=s3->r;
goto _LL29;}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAB=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD2;_tmpD2.tag=0,({struct _fat_ptr _tmp10C=({const char*_tmpAF="|";_tag_fat(_tmpAF,sizeof(char),2U);});_tmpD2.f1=_tmp10C;});_tmpD2;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmpAC=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmpD1;_tmpD1.tag=5,_tmpD1.f1=s;_tmpD1;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAD=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD0;_tmpD0.tag=0,({struct _fat_ptr _tmp10D=({const char*_tmpAE="|: array type bad initializer";_tag_fat(_tmpAE,sizeof(char),30U);});_tmpD0.f1=_tmp10D;});_tmpD0;});void*_tmpAA[3];_tmpAA[0]=& _tmpAB,_tmpAA[1]=& _tmpAC,_tmpAA[2]=& _tmpAD;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmpAA,sizeof(void*),3));});}_LL29:;}
# 667
goto _LL24;}else{
goto _LL24;}_LL24:;}
# 671
if(vd->initializer != 0)
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
({struct Cyc_List_List*_tmp10E=({struct Cyc_List_List*_tmpB0=_region_malloc(env.rgn,sizeof(*_tmpB0));_tmpB0->hd=vd,_tmpB0->tl=env.all_locals;_tmpB0;});env.all_locals=_tmp10E;});
s=s2;
continue;}}else{
goto _LL1F;}_LL1F:;}
# 678
goto _LL1E;}default: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD3;_tmpD3.tag=0,({struct _fat_ptr _tmp10F=({const char*_tmpB3="bad stmt after xlation to C";_tag_fat(_tmpB3,sizeof(char),28U);});_tmpD3.f1=_tmp10F;});_tmpD3;});void*_tmpB1[1];_tmpB1[0]=& _tmpB2;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmpB1,sizeof(void*),1));});};}}}
# 684
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*_tmpB4=0;struct Cyc_List_List*new_ds=_tmpB4;
for(1;old_ds != 0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*_tmpB5=(struct Cyc_Absyn_Decl*)old_ds->hd;struct Cyc_Absyn_Decl*next_d=_tmpB5;
void*_tmpB6=next_d->r;void*_stmttmp10=_tmpB6;void*_tmpB7=_stmttmp10;void*_tmpB8;switch(*((int*)_tmpB7)){case 1: _tmpB8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpB7)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpB8;
# 693
fd->cached_type=0;
# 699
if(fd->param_vardecls != 0){
struct _RegionHandle _tmpB9=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB9;_push_region(rgn);
{struct Cyc_List_List**_tmpBA=({struct Cyc_List_List**_tmpBE=_region_malloc(rgn,sizeof(*_tmpBE));*_tmpBE=0;_tmpBE;});struct Cyc_List_List**gen_ds=_tmpBA;
({struct Cyc_Tovc_ToExpEnv _tmp112=({struct Cyc_Tovc_ToExpEnv _tmpD4;_tmpD4.rgn=rgn,({
struct Cyc_List_List*_tmp111=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rcopy;})(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v);_tmpD4.all_locals=_tmp111;}),({
struct Cyc_Dict_Dict _tmp110=({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_rempty;})(rgn,Cyc_strptrcmp);_tmpD4.varmap=_tmp110;}),_tmpD4.encloser=fd->body,_tmpD4.gen_ds=gen_ds;_tmpD4;});
# 702
Cyc_Tovc_stmt_to_vc(_tmp112,fd->body);});
# 708
if(*gen_ds != 0){
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->sc=fd->sc,_tmpBD->name=fd->name,_tmpBD->varloc=0U,({struct Cyc_Absyn_Tqual _tmp114=Cyc_Absyn_const_tqual(0U);_tmpBD->tq=_tmp114;}),({
void*_tmp113=Cyc_Tcutil_fndecl2type(fd);_tmpBD->type=_tmp113;}),_tmpBD->initializer=0,_tmpBD->rgn=0,_tmpBD->attributes=(fd->i).attributes,_tmpBD->escapes=0,_tmpBD->is_proto=fd->body != 0;_tmpBD;});
# 712
new_ds=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct Cyc_Absyn_Decl*_tmp115=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->tag=0,_tmpBB->f1=vd;_tmpBB;}),0U);_tmpBC->hd=_tmp115;}),_tmpBC->tl=new_ds;_tmpBC;});
new_ds=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(*gen_ds,new_ds);}}
# 701
;_pop_region();}
# 716
goto _LL4;}case 0: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 5: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 new_ds=({struct Cyc_List_List*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->hd=next_d,_tmpBF->tl=new_ds;_tmpBF;});goto _LL0;case 9:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
# 733
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC1=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD5;_tmpD5.tag=0,({struct _fat_ptr _tmp116=({const char*_tmpC2="nested translation unit after translation to C";_tag_fat(_tmpC2,sizeof(char),47U);});_tmpD5.f1=_tmp116;});_tmpD5;});void*_tmpC0[1];_tmpC0[0]=& _tmpC1;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmpC0,sizeof(void*),1));});}_LL0:;}
# 736
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(new_ds);}

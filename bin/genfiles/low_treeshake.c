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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 66
extern void Cyc_Set_imp_insert(struct Cyc_Set_Set*,void*);
# 88
extern void*Cyc_Set_imp_delete(struct Cyc_Set_Set*,void*);
# 97
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];
# 137
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 172
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 464 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 843 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_hash_qvar(struct _tuple0*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1118
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_LowTreeShake_Env{struct Cyc_Hashtable_Table*aggrdecls;struct Cyc_Set_Set*aggr_worklist;struct Cyc_Set_Set*aggr_keepers;struct Cyc_Set_Set*enum_keepers;};
# 45 "low_treeshake.cyc"
static void Cyc_LowTreeShake_build_tables_stmt(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*);
static void Cyc_LowTreeShake_build_tables_exp(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_LowTreeShake_build_tables_type(struct Cyc_LowTreeShake_Env*env,void*t){
void*_tmp0=t;struct Cyc_Absyn_FnInfo _tmp2;void*_tmp3;void*_tmp1;union Cyc_Absyn_AggrInfo _tmp4;switch(*((int*)_tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp0)->f1)){case 20: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp0)->f2 == 0){_tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo ai=_tmp4;
# 50
struct _tuple0*_tmp5=(Cyc_Absyn_aggr_kinded_name(ai)).f2;struct _tuple0*n=_tmp5;
if(!({(int(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_member;})(env->aggr_keepers,n))
({(void(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_insert;})(env->aggr_worklist,n);
goto _LL0;}}else{goto _LL13;}case 15: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp0)->f2 == 0){_tmp1=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp0)->f1)->f1;{struct _tuple0*n=_tmp1;
# 55
({(void(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_insert;})(env->enum_keepers,n);goto _LL0;}}else{goto _LL13;}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp0)->f2 == 0)
goto _LL0;else{goto _LL13;}}case 1: _tmp1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp0)->f2;{void*topt=_tmp1;
# 58
_tmp1=topt;goto _LLA;}case 8: _tmp1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp0)->f4;_LLA: {void*topt=_tmp1;
if((unsigned)topt){_tmp1=topt;goto _LLC;}goto _LL0;}case 3: _tmp1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp0)->f1).elt_type;_LLC: {void*t1=_tmp1;
Cyc_LowTreeShake_build_tables_type(env,t1);goto _LL0;}case 4: _tmp1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp0)->f1).elt_type;_tmp3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp0)->f1).num_elts;{void*t1=_tmp1;struct Cyc_Absyn_Exp*eo=_tmp3;
# 63
if((unsigned)eo)Cyc_LowTreeShake_build_tables_exp(env,eo);
Cyc_LowTreeShake_build_tables_type(env,t1);
goto _LL0;}case 5: _tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp0)->f1;{struct Cyc_Absyn_FnInfo i=_tmp2;
# 68
Cyc_LowTreeShake_build_tables_type(env,i.ret_type);
{struct Cyc_List_List*_tmp6=i.args;struct Cyc_List_List*args=_tmp6;for(0;args != 0;args=args->tl){
Cyc_LowTreeShake_build_tables_type(env,(*((struct _tuple8*)args->hd)).f3);}}
goto _LL0;}case 11: _tmp1=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp0)->f1;{struct Cyc_Absyn_Exp*e=_tmp1;
# 73
Cyc_LowTreeShake_build_tables_exp(env,e);goto _LL0;}default: _LL13:
# 75
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp37;_tmp37.tag=0,({struct _fat_ptr _tmp3D=({const char*_tmpA="LowTreeshake: unexpected type";_tag_fat(_tmpA,sizeof(char),30U);});_tmp37.f1=_tmp3D;});_tmp37;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp36;_tmp36.tag=2,_tmp36.f1=(void*)t;_tmp36;});void*_tmp7[2];_tmp7[0]=& _tmp8,_tmp7[1]=& _tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp7,sizeof(void*),2));});}_LL0:;}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple13{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 78
static void Cyc_LowTreeShake_build_tables_exp(struct Cyc_LowTreeShake_Env*env,struct Cyc_Absyn_Exp*e){
# 80
void*_tmpB=e->r;void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;void*_tmpF;void*_tmpE;void*_tmpD;switch(*((int*)_tmpC)){case 0:
 goto _LL4;case 33: _LL4:
 goto _LL6;case 1: _LL6:
 goto _LL0;case 32: _tmpD=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpC)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmpD;
# 85
({(void(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_insert;})(env->enum_keepers,ed->name);goto _LL0;}case 14: _tmpD=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_tmpE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC)->f2;{void*t=_tmpD;struct Cyc_Absyn_Exp*e1=_tmpE;
# 88
Cyc_LowTreeShake_build_tables_type(env,t);
Cyc_LowTreeShake_build_tables_exp(env,e1);
goto _LL0;}case 19: _tmpD=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC)->f1;{void*t=_tmpD;
_tmpD=t;goto _LLE;}case 17: _tmpD=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LLE: {void*t=_tmpD;
Cyc_LowTreeShake_build_tables_type(env,t);goto _LL0;}case 12: _tmpD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpC)->f1;{struct Cyc_Absyn_Exp*e1=_tmpD;
# 94
_tmpD=e1;goto _LL12;}case 13: _tmpD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpD;
_tmpD=e1;goto _LL14;}case 41: _tmpD=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpD;
_tmpD=e1;goto _LL16;}case 5: _tmpD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpD;
_tmpD=e1;goto _LL18;}case 18: _tmpD=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpD;
_tmpD=e1;goto _LL1A;}case 15: _tmpD=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpD;
_tmpD=e1;goto _LL1C;}case 20: _tmpD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpD;
_tmpD=e1;goto _LL1E;}case 21: _tmpD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpD;
_tmpD=e1;goto _LL20;}case 22: _tmpD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmpD;
Cyc_LowTreeShake_build_tables_exp(env,e1);goto _LL0;}case 10: _tmpD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_tmpE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpC)->f2;{struct Cyc_Absyn_Exp*e1=_tmpD;struct Cyc_List_List*es=_tmpE;
# 105
Cyc_LowTreeShake_build_tables_exp(env,e1);
_tmpD=es;goto _LL24;}case 3: _tmpD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC)->f2;_LL24: {struct Cyc_List_List*es=_tmpD;
# 108
for(1;es != 0;es=es->tl){
Cyc_LowTreeShake_build_tables_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 6: _tmpD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_tmpE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC)->f2;_tmpF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC)->f3;{struct Cyc_Absyn_Exp*e1=_tmpD;struct Cyc_Absyn_Exp*e2=_tmpE;struct Cyc_Absyn_Exp*e3=_tmpF;
# 113
Cyc_LowTreeShake_build_tables_exp(env,e1);
_tmpD=e2;_tmpE=e3;goto _LL28;}case 4: _tmpD=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_tmpE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpC)->f3;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpD;struct Cyc_Absyn_Exp*e2=_tmpE;
_tmpD=e1;_tmpE=e2;goto _LL2A;}case 7: _tmpD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_tmpE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpD;struct Cyc_Absyn_Exp*e2=_tmpE;
_tmpD=e1;_tmpE=e2;goto _LL2C;}case 8: _tmpD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_tmpE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpD;struct Cyc_Absyn_Exp*e2=_tmpE;
_tmpD=e1;_tmpE=e2;goto _LL2E;}case 9: _tmpD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_tmpE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC)->f2;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpD;struct Cyc_Absyn_Exp*e2=_tmpE;
_tmpD=e1;_tmpE=e2;goto _LL30;}case 23: _tmpD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_tmpE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmpD;struct Cyc_Absyn_Exp*e2=_tmpE;
# 120
Cyc_LowTreeShake_build_tables_exp(env,e1);
Cyc_LowTreeShake_build_tables_exp(env,e2);
goto _LL0;}case 36: if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC)->f1 == 0){_tmpD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC)->f2;{struct Cyc_List_List*ldt=_tmpD;
# 124
_tmpD=ldt;goto _LL34;}}else{goto _LL39;}case 26: _tmpD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpC)->f1;_LL34: {struct Cyc_List_List*ldt=_tmpD;
# 126
for(1;ldt != 0;ldt=ldt->tl){
Cyc_LowTreeShake_build_tables_exp(env,(*((struct _tuple12*)ldt->hd)).f2);}
goto _LL0;}case 37: _tmpD=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC)->f1;{struct Cyc_Absyn_Stmt*s=_tmpD;
# 130
return Cyc_LowTreeShake_build_tables_stmt(env,s);}case 40: _tmpD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpC)->f3;_tmpE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpC)->f4;{struct Cyc_List_List*sl1=_tmpD;struct Cyc_List_List*sl2=_tmpE;
# 133
for(1;sl1 != 0;sl1=sl1->tl){
Cyc_LowTreeShake_build_tables_exp(env,(*((struct _tuple13*)sl1->hd)).f2);}
for(1;sl2 != 0;sl2=sl2->tl){
Cyc_LowTreeShake_build_tables_exp(env,(*((struct _tuple13*)sl2->hd)).f2);}
goto _LL0;}default: _LL39:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _tmp39;_tmp39.tag=0,({struct _fat_ptr _tmp3E=({const char*_tmp13="LowTreeshake: unexpected expression form";_tag_fat(_tmp13,sizeof(char),41U);});_tmp39.f1=_tmp3E;});_tmp39;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp12=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp38;_tmp38.tag=4,_tmp38.f1=e;_tmp38;});void*_tmp10[2];_tmp10[0]=& _tmp11,_tmp10[1]=& _tmp12;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp10,sizeof(void*),2));});}_LL0:;}
# 141
static void Cyc_LowTreeShake_build_tables_stmt(struct Cyc_LowTreeShake_Env*env,struct Cyc_Absyn_Stmt*s){
# 143
void*_tmp14=s->r;void*_stmttmp1=_tmp14;void*_tmp15=_stmttmp1;void*_tmp19;void*_tmp18;void*_tmp17;void*_tmp16;switch(*((int*)_tmp15)){case 3: _tmp16=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp15)->f1;{struct Cyc_Absyn_Exp*eo=_tmp16;
if((unsigned)eo){_tmp16=eo;goto _LL4;}goto _LL0;}case 1: _tmp16=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp15)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp16;
Cyc_LowTreeShake_build_tables_exp(env,e);goto _LL0;}case 4: _tmp16=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp15)->f2;_tmp18=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp15)->f3;{struct Cyc_Absyn_Exp*e1=_tmp16;struct Cyc_Absyn_Stmt*s1=_tmp17;struct Cyc_Absyn_Stmt*s2=_tmp18;
# 147
Cyc_LowTreeShake_build_tables_exp(env,e1);
_tmp16=s1;_tmp17=s2;goto _LL8;}case 2: _tmp16=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp15)->f2;_LL8: {struct Cyc_Absyn_Stmt*s1=_tmp16;struct Cyc_Absyn_Stmt*s2=_tmp17;
# 150
Cyc_LowTreeShake_build_tables_stmt(env,s1);
Cyc_LowTreeShake_build_tables_stmt(env,s2);
goto _LL0;}case 13: _tmp16=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp15)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp16;
Cyc_LowTreeShake_build_tables_stmt(env,s1);goto _LL0;}case 14: _tmp16=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp15)->f1;_tmp17=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp15)->f2).f1;{struct Cyc_Absyn_Stmt*s1=_tmp16;struct Cyc_Absyn_Exp*e1=_tmp17;
_tmp16=e1;_tmp17=s1;goto _LLE;}case 5: _tmp16=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp15)->f1).f1;_tmp17=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp15)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp16;struct Cyc_Absyn_Stmt*s1=_tmp17;
# 156
Cyc_LowTreeShake_build_tables_exp(env,e1);
Cyc_LowTreeShake_build_tables_stmt(env,s1);
goto _LL0;}case 9: _tmp16=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp15)->f1;_tmp17=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp15)->f2).f1;_tmp18=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp15)->f3).f1;_tmp19=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp15)->f4;{struct Cyc_Absyn_Exp*e1=_tmp16;struct Cyc_Absyn_Exp*e2=_tmp17;struct Cyc_Absyn_Exp*e3=_tmp18;struct Cyc_Absyn_Stmt*s1=_tmp19;
# 160
Cyc_LowTreeShake_build_tables_exp(env,e1);
Cyc_LowTreeShake_build_tables_exp(env,e2);
Cyc_LowTreeShake_build_tables_exp(env,e3);
Cyc_LowTreeShake_build_tables_stmt(env,s1);
goto _LL0;}case 10: _tmp16=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp15)->f2;{struct Cyc_Absyn_Exp*e1=_tmp16;struct Cyc_List_List*scs=_tmp17;
# 166
Cyc_LowTreeShake_build_tables_exp(env,e1);
for(1;scs != 0;scs=scs->tl){
Cyc_LowTreeShake_build_tables_stmt(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12: _tmp16=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp15)->f2;{struct Cyc_Absyn_Decl*d=_tmp16;struct Cyc_Absyn_Stmt*s1=_tmp17;
# 171
{void*_tmp1A=d->r;void*_stmttmp2=_tmp1A;void*_tmp1B=_stmttmp2;void*_tmp1C;switch(*((int*)_tmp1B)){case 0: _tmp1C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1B)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp1C;
# 173
Cyc_LowTreeShake_build_tables_type(env,vd->type);
if(vd->initializer != 0)
Cyc_LowTreeShake_build_tables_exp(env,_check_null(vd->initializer));
goto _LL17;}case 1: _tmp1C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1B)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp1C;
# 178
Cyc_LowTreeShake_build_tables_type(env,(fd->i).ret_type);
{struct Cyc_List_List*_tmp1D=(fd->i).args;struct Cyc_List_List*args=_tmp1D;for(0;args != 0;args=args->tl){
Cyc_LowTreeShake_build_tables_type(env,(*((struct _tuple8*)args->hd)).f3);}}
Cyc_LowTreeShake_build_tables_stmt(env,fd->body);
goto _LL17;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A;_tmp3A.tag=0,({struct _fat_ptr _tmp3F=({const char*_tmp20="LowTreeShake: local decl not a var or function";_tag_fat(_tmp20,sizeof(char),47U);});_tmp3A.f1=_tmp3F;});_tmp3A;});void*_tmp1E[1];_tmp1E[0]=& _tmp1F;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp1E,sizeof(void*),1));});}_LL17:;}
# 185
Cyc_LowTreeShake_build_tables_stmt(env,s1);
goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 191
static void Cyc_LowTreeShake_add_to_table(struct Cyc_Hashtable_Table*tab,struct _tuple0*qv,void*d){
# 193
struct Cyc_List_List**_tmp21=({(struct Cyc_List_List**(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))Cyc_Hashtable_lookup_opt;})(tab,qv);struct Cyc_List_List**lst_opt=_tmp21;
struct Cyc_List_List*_tmp22=({struct Cyc_List_List*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->hd=d,(unsigned)lst_opt?_tmp23->tl=*lst_opt:(_tmp23->tl=0);_tmp23;});struct Cyc_List_List*lst=_tmp22;
({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_List_List*))Cyc_Hashtable_insert;})(tab,qv,lst);}
# 198
static void Cyc_LowTreeShake_build_tables(struct Cyc_LowTreeShake_Env*env,struct Cyc_List_List*tds){
for(1;tds != 0;tds=tds->tl){
void*_tmp24=((struct Cyc_Absyn_Decl*)tds->hd)->r;void*_stmttmp3=_tmp24;void*_tmp25=_stmttmp3;void*_tmp26;switch(*((int*)_tmp25)){case 0: _tmp26=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp25)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp26;
# 202
Cyc_LowTreeShake_build_tables_type(env,vd->type);
if(vd->initializer != 0)
Cyc_LowTreeShake_build_tables_exp(env,_check_null(vd->initializer));
goto _LL0;}case 1: _tmp26=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp25)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp26;
# 207
Cyc_LowTreeShake_build_tables_type(env,(fd->i).ret_type);
{struct Cyc_List_List*_tmp27=(fd->i).args;struct Cyc_List_List*args=_tmp27;for(0;args != 0;args=args->tl){
Cyc_LowTreeShake_build_tables_type(env,(*((struct _tuple8*)args->hd)).f3);}}
Cyc_LowTreeShake_build_tables_stmt(env,fd->body);
goto _LL0;}case 5: _tmp26=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp25)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp26;
({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_LowTreeShake_add_to_table;})(env->aggrdecls,ad->name,ad);goto _LL0;}case 7: _tmp26=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp25)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp26;
# 215
if(ed->fields != 0){
struct Cyc_List_List*_tmp28=(struct Cyc_List_List*)(_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp28;for(0;fs != 0;fs=fs->tl){
if(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag != 0)
Cyc_LowTreeShake_build_tables_exp(env,_check_null(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag));}}
goto _LL0;}case 8: _tmp26=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp25)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp26;
# 221
Cyc_LowTreeShake_build_tables_type(env,_check_null(td->defn));goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B;_tmp3B.tag=0,({struct _fat_ptr _tmp40=({const char*_tmp2B="LowTreeShake: top-level Cyclone decl";_tag_fat(_tmp2B,sizeof(char),37U);});_tmp3B.f1=_tmp40;});_tmp3B;});void*_tmp29[1];_tmp29[0]=& _tmp2A;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp29,sizeof(void*),1));});}_LL0:;}}
# 226
static void Cyc_LowTreeShake_process_worklist(struct Cyc_LowTreeShake_Env*env){
# 228
while(!({(int(*)(struct Cyc_Set_Set*))Cyc_Set_is_empty;})(env->aggr_worklist)){
struct _tuple0*_tmp2C=({(struct _tuple0*(*)(struct Cyc_Set_Set*))Cyc_Set_choose;})(env->aggr_worklist);struct _tuple0*n=_tmp2C;
({(struct _tuple0*(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_delete;})(env->aggr_worklist,n);
({(void(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_insert;})(env->aggr_keepers,n);{
# 233
struct Cyc_List_List**_tmp2D=({(struct Cyc_List_List**(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))Cyc_Hashtable_lookup_opt;})(env->aggrdecls,n);struct Cyc_List_List**ads_opt=_tmp2D;
if(!((unsigned)ads_opt))
continue;{
struct Cyc_List_List*_tmp2E=*ads_opt;struct Cyc_List_List*ads=_tmp2E;for(0;ads != 0;ads=ads->tl){
if(((struct Cyc_Absyn_Aggrdecl*)ads->hd)->impl != 0){
struct Cyc_List_List*_tmp2F=(_check_null(((struct Cyc_Absyn_Aggrdecl*)ads->hd)->impl))->fields;struct Cyc_List_List*fs=_tmp2F;for(0;fs != 0;fs=fs->tl){
Cyc_LowTreeShake_build_tables_type(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);}}}}}}}
# 243
static struct Cyc_List_List*Cyc_LowTreeShake_trim_decls(struct Cyc_LowTreeShake_Env*env,struct Cyc_List_List*tds){
struct Cyc_List_List*_tmp30=0;struct Cyc_List_List*ans=_tmp30;
for(1;tds != 0;tds=tds->tl){
int keep;
{void*_tmp31=((struct Cyc_Absyn_Decl*)tds->hd)->r;void*_stmttmp4=_tmp31;void*_tmp32=_stmttmp4;void*_tmp33;switch(*((int*)_tmp32)){case 5: _tmp33=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp32)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp33;
keep=({(int(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_member;})(env->aggr_keepers,ad->name);goto _LL0;}case 7: _tmp33=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp32)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp33;
keep=({(int(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_member;})(env->enum_keepers,ed->name);goto _LL0;}default:
 keep=1;goto _LL0;}_LL0:;}
# 252
if(keep)
ans=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->hd=(struct Cyc_Absyn_Decl*)tds->hd,_tmp34->tl=ans;_tmp34;});}
# 255
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(ans);}
# 258
struct Cyc_List_List*Cyc_LowTreeShake_shake(struct Cyc_List_List*tds){
struct Cyc_LowTreeShake_Env _tmp35=({struct Cyc_LowTreeShake_Env _tmp3C;({struct Cyc_Hashtable_Table*_tmp44=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))Cyc_Hashtable_create;})(37,Cyc_Absyn_qvar_cmp,Cyc_Absyn_hash_qvar);_tmp3C.aggrdecls=_tmp44;}),({
struct Cyc_Set_Set*_tmp43=({(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Set_empty;})(Cyc_Absyn_qvar_cmp);_tmp3C.aggr_worklist=_tmp43;}),({
struct Cyc_Set_Set*_tmp42=({(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Set_empty;})(Cyc_Absyn_qvar_cmp);_tmp3C.aggr_keepers=_tmp42;}),({
struct Cyc_Set_Set*_tmp41=({(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Set_empty;})(Cyc_Absyn_qvar_cmp);_tmp3C.enum_keepers=_tmp41;});_tmp3C;});
# 259
struct Cyc_LowTreeShake_Env env=_tmp35;
# 263
Cyc_LowTreeShake_build_tables(& env,tds);
Cyc_LowTreeShake_process_worklist(& env);
return Cyc_LowTreeShake_trim_decls(& env,tds);}

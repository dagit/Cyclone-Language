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
 struct Cyc_Core_Opt{void*v;};
# 123 "core.h"
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 37 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 851 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 864
void*Cyc_Absyn_compress(void*);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 63 "tcutil.h"
void*Cyc_Tcutil_pointer_elt_type(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 61 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 63
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
void*Cyc_AssnDef_zero (void);
void*Cyc_AssnDef_one (void);
# 70
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
void*Cyc_AssnDef_cast(void*,void*);
void*Cyc_AssnDef_fresh_var(void*);
# 79
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 91
int Cyc_AssnDef_assncmp(void*,void*);
# 93
struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 96
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 107 "assndef.h"
void Cyc_AssnDef_reset_hash_cons_table (void);
# 114
void*Cyc_AssnDef_kill(void*);
# 116
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*,void*,void*);
# 121
void*Cyc_AssnDef_eq(void*,void*);
# 123
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 129
void*Cyc_AssnDef_reduce(void*);
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
# 132
int Cyc_AssnDef_simple_prove(void*,void*);
# 134
void*Cyc_AssnDef_kill_mem_term(void*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 126 "dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict,void*,void**);
# 193
extern struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 79
extern struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 114
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Vcgen_Env;
# 112 "vcgen.cyc"
static int Cyc_Vcgen_simple_implies(void**a1,void**a2){
({void*_tmp1ED=Cyc_AssnDef_reduce(*a1);*a1=_tmp1ED;});
({void*_tmp1EE=Cyc_AssnDef_reduce(*a2);*a2=_tmp1EE;});
return Cyc_AssnDef_simple_prove(*a1,*a2);}
# 119
static int Cyc_Vcgen_implies(void**a1,void**a2){
return Cyc_Vcgen_simple_implies(a1,a2)|| Cyc_PrattProver_constraint_prove(*a1,*a2);}
# 134
static struct Cyc_Set_Set*Cyc_Vcgen_widen_it(struct Cyc_Set_Set*s,void*a){
if(s == 0)return 0;{
void*_tmp0=a;enum Cyc_AssnDef_Primreln _tmp3;void*_tmp2;void*_tmp1;switch(*((int*)_tmp0)){case 2: if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f2 == Cyc_AssnDef_Eq){_tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f3;{void*t1=_tmp1;void*t2=_tmp2;
# 138
return((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert)(s,a);}}else{_tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f1;_tmp3=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f2;_tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f3;{void*t1=_tmp1;enum Cyc_AssnDef_Primreln p=_tmp3;void*t2=_tmp2;
return((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert)(s,a);}}case 3: _tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp0)->f2;{void*a1=_tmp1;void*a2=_tmp2;
return({struct Cyc_Set_Set*_tmp1EF=Cyc_Vcgen_widen_it(s,a1);Cyc_Vcgen_widen_it(_tmp1EF,a2);});}case 4: _tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp0)->f2;{void*a1=_tmp1;void*a2=_tmp2;
# 142
struct Cyc_Set_Set*_tmp4=Cyc_Vcgen_widen_it(s,a1);struct Cyc_Set_Set*s1=_tmp4;
struct Cyc_Set_Set*_tmp5=Cyc_Vcgen_widen_it(s,a2);struct Cyc_Set_Set*s2=_tmp5;
if(s1 == 0 || s2 == 0)return 0;
return((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_intersect)(s1,s2);}case 0:
 return s;case 1:
 return 0;default:
 return({struct Cyc_Set_Set*_tmp1F0=s;Cyc_Vcgen_widen_it(_tmp1F0,Cyc_AssnDef_reduce(a));});};}}
# 152
static void*Cyc_Vcgen_widen(void*a){
struct Cyc_Set_Set*_tmp6=({struct Cyc_Set_Set*_tmp1F1=((struct Cyc_Set_Set*(*)(int(*)(void*,void*)))Cyc_Set_empty)(Cyc_AssnDef_assncmp);Cyc_Vcgen_widen_it(_tmp1F1,a);});struct Cyc_Set_Set*sopt=_tmp6;
if(sopt == 0)return(void*)& Cyc_AssnDef_false_assn;
return((void*(*)(void*(*)(void*,void*),struct Cyc_Set_Set*,void*))Cyc_Set_fold)(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);}
# 159
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Vardecl*);
# 162
static void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_tmp7=tgt;void*_tmp8;if(_tmp7 == 0)
# 167
return Cyc_AssnDef_kill(a);else{if(*((int*)_tmp7)== 1){_tmp8=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp7)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp8;
# 173
if(vd->escapes){
a=Cyc_AssnDef_kill(a);
src=src == 0?0: Cyc_AssnDef_kill_mem_term(src);}else{
# 177
void*_tmp9=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmp9;
a=Cyc_AssnDef_subst(vd,v2,a);
src=src == 0?0: Cyc_AssnDef_subst_term(src,vd,v2);}
# 182
return({void*_tmp1F2=a;Cyc_AssnDef_and(_tmp1F2,Cyc_AssnDef_eq(tgt,src));});}}else{
({void*_tmpA=0U;({struct _fat_ptr _tmp1F3=({const char*_tmpB="vcgen do_assign";_tag_fat(_tmpB,sizeof(char),16U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp1F3,_tag_fat(_tmpA,sizeof(void*),0));});});}};}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void**try_assn;void**exp_stmt;void*res_term;void**res_assn;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;struct Cyc_Hashtable_Table**assn_info;};
# 223 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 225
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,void**res_assn){
# 227
struct Cyc_Hashtable_Table*_tmpC=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup)(tables->succ_tables,fd);struct Cyc_Hashtable_Table*succ_table=_tmpC;
struct Cyc_Hashtable_Table*_tmpD=((struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_table=_tmpD;
struct Cyc_Vcgen_SharedEnv*_tmpE=({struct Cyc_Vcgen_SharedEnv*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->assn_table=assn_table,_tmp11->succ_table=succ_table,_tmp11->try_assn=0,_tmp11->exp_stmt=0,_tmp11->res_term=0,_tmp11->res_assn=res_assn;_tmp11;});struct Cyc_Vcgen_SharedEnv*senv=_tmpE;
# 235
struct Cyc_Dict_Dict _tmpF=((struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty)((int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp);struct Cyc_Dict_Dict d=_tmpF;
return({struct Cyc_Vcgen_Env*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->shared=senv,_tmp10->state_counter=d,_tmp10->assn_info=assn_info;_tmp10;});}
# 239
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_tmp12=_cycalloc(sizeof(*_tmp12));*_tmp12=*env;_tmp12;});}
# 245
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 249
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _tmp1F4=((struct Cyc_Dict_Dict(*)(int(*)(int,struct Cyc_Absyn_Vardecl*,int,int),int,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_union_two_c)(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_tmp1F4;});}
# 257
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
((int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int*))Cyc_Dict_lookup_bool)(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _tmp1F5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int))Cyc_Dict_insert)(env->state_counter,vd,res + 1);env->state_counter=_tmp1F5;});
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->tag=2,_tmp13->f1=vd,_tmp13->f2=res,_tmp13->f3=vd->type;_tmp13;});}
# 266
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***_tmp14=((void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);void***popt=_tmp14;
if(popt != 0){
void**p=*popt;
void*_tmp15=Cyc_AssnDef_or(ain,*p);void*new_precondition=_tmp15;
*p=new_precondition;
return new_precondition;}else{
# 274
return ain;}}
# 278
static void**Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
static void*false_pointer=(void*)& Cyc_AssnDef_false_assn;
void***popt=((void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(popt != 0)
return*popt;else{
# 284
return& false_pointer;}}
# 289
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp16=*aprevopt;void**aprev=_tmp16;
ain=Cyc_Vcgen_widen(Cyc_AssnDef_or(ain,*aprev));
*aprev=ain;}else{
# 296
({struct Cyc_Hashtable_Table*_tmp1F7=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp1F6=s;((void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert)(_tmp1F7,_tmp1F6,({void**_tmp17=_cycalloc(sizeof(*_tmp17));*_tmp17=ain;_tmp17;}));});}
# 298
return ain;}
# 303
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp18=*aprevopt;void**aprev=_tmp18;
ain=Cyc_Vcgen_widen(Cyc_AssnDef_or(ain,*aprev));
*aprev=ain;}else{
# 310
({struct Cyc_Hashtable_Table*_tmp1F9=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp1F8=s;((void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert)(_tmp1F9,_tmp1F8,({void**_tmp19=_cycalloc(sizeof(*_tmp19));*_tmp19=ain;_tmp19;}));});}
# 312
return ain;}
# 317
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
if((env->shared)->try_assn == 0)return;
({void*_tmp1FA=Cyc_Vcgen_widen(Cyc_AssnDef_or(*((void**)_check_null((env->shared)->try_assn)),a));*((void**)_check_null((env->shared)->try_assn))=_tmp1FA;});}
# 323
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a,void*topt){
struct Cyc_Vcgen_SharedEnv*_tmp1A=env->shared;struct Cyc_Vcgen_SharedEnv*shared=_tmp1A;
if(shared->res_assn == 0)return;
if(shared->res_term != 0 || t != 0){
void*_tmp1B=Cyc_AssnDef_fresh_var(topt);void*v=_tmp1B;
shared->res_term=v;
({void*_tmp1FE=({
void*_tmp1FD=({void*_tmp1FB=a;Cyc_AssnDef_and(_tmp1FB,Cyc_AssnDef_eq(v,t));});Cyc_AssnDef_or(_tmp1FD,({void*_tmp1FC=*((void**)_check_null(shared->res_assn));Cyc_AssnDef_and(_tmp1FC,Cyc_AssnDef_eq(v,shared->res_term));}));});
# 329
*((void**)_check_null(shared->res_assn))=_tmp1FE;});}else{
# 332
({void*_tmp1FF=Cyc_AssnDef_or(a,*((void**)_check_null(shared->res_assn)));*((void**)_check_null(shared->res_assn))=_tmp1FF;});}}
# 337
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void){
struct Cyc_Hashtable_Table*_tmp1C=((struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_info=_tmp1C;
return({struct Cyc_Hashtable_Table**_tmp1D=_cycalloc(sizeof(*_tmp1D));*_tmp1D=assn_info;_tmp1D;});}struct _tuple12{void*f1;void*f2;};
# 342
struct _tuple12 Cyc_Vcgen_exp2ctxt_assn(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){
struct _tuple12**_tmp1E=((struct _tuple12**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_lookup_opt)(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e);struct _tuple12**result=_tmp1E;
if(result == 0)
# 347
return({struct _tuple12 _tmp1AA;_tmp1AA.f1=(void*)& Cyc_AssnDef_true_assn,_tmp1AA.f2=(void*)& Cyc_AssnDef_false_assn;_tmp1AA;});else{
# 349
return*(*result);}}
# 353
static void Cyc_Vcgen_insert_assn_info(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct _tuple12*ctxt_assn){
# 361
((void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct _tuple12*))Cyc_Hashtable_insert)(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e,ctxt_assn);
return;}
# 365
static struct Cyc_Absyn_Exp*Cyc_Vcgen_shared_zero_exp (void){
static struct Cyc_Absyn_Exp*ans=0;
if(!((unsigned)ans))
ans=Cyc_Absyn_uint_exp(0U,0U);
return(struct Cyc_Absyn_Exp*)_check_null(ans);}
# 377
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Stmt*,void*);
# 381
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 383
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 385
static struct _tuple12 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 388
static struct _tuple12 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);struct _tuple13{struct Cyc_List_List**f1;void*f2;};
# 392
static struct _tuple13 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 395
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);struct _tuple14{struct Cyc_List_List*f1;void*f2;};
# 397
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 399
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 401
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Decl*,void*);
# 403
static struct _fat_ptr Cyc_Vcgen_stmt2shortstring(struct Cyc_Absyn_Stmt*s){
void*_tmp1F=s->r;void*_stmttmp0=_tmp1F;void*_tmp20=_stmttmp0;void*_tmp23;void*_tmp22;void*_tmp21;switch(*((int*)_tmp20)){case 4: _tmp21=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp20)->f1;{struct Cyc_Absyn_Exp*e=_tmp21;
# 406
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp26=({struct Cyc_String_pa_PrintArg_struct _tmp1AB;_tmp1AB.tag=0,({struct _fat_ptr _tmp200=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1AB.f1=_tmp200;});_tmp1AB;});void*_tmp24[1];_tmp24[0]=& _tmp26;({struct _fat_ptr _tmp201=({const char*_tmp25="if (%s) ...";_tag_fat(_tmp25,sizeof(char),12U);});Cyc_aprintf(_tmp201,_tag_fat(_tmp24,sizeof(void*),1));});});}case 5: _tmp21=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp20)->f1).f1;{struct Cyc_Absyn_Exp*e=_tmp21;
# 408
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp29=({struct Cyc_String_pa_PrintArg_struct _tmp1AC;_tmp1AC.tag=0,({struct _fat_ptr _tmp202=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1AC.f1=_tmp202;});_tmp1AC;});void*_tmp27[1];_tmp27[0]=& _tmp29;({struct _fat_ptr _tmp203=({const char*_tmp28="while (%s) ...";_tag_fat(_tmp28,sizeof(char),15U);});Cyc_aprintf(_tmp203,_tag_fat(_tmp27,sizeof(void*),1));});});}case 9: _tmp21=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp20)->f1;_tmp22=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp20)->f2).f1;_tmp23=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp20)->f3).f1;{struct Cyc_Absyn_Exp*e1=_tmp21;struct Cyc_Absyn_Exp*e2=_tmp22;struct Cyc_Absyn_Exp*e3=_tmp23;
# 410
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C=({struct Cyc_String_pa_PrintArg_struct _tmp1AF;_tmp1AF.tag=0,({struct _fat_ptr _tmp204=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp1AF.f1=_tmp204;});_tmp1AF;});struct Cyc_String_pa_PrintArg_struct _tmp2D=({struct Cyc_String_pa_PrintArg_struct _tmp1AE;_tmp1AE.tag=0,({
struct _fat_ptr _tmp205=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e2));_tmp1AE.f1=_tmp205;});_tmp1AE;});struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp1AD;_tmp1AD.tag=0,({struct _fat_ptr _tmp206=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e3));_tmp1AD.f1=_tmp206;});_tmp1AD;});void*_tmp2A[3];_tmp2A[0]=& _tmp2C,_tmp2A[1]=& _tmp2D,_tmp2A[2]=& _tmp2E;({struct _fat_ptr _tmp207=({const char*_tmp2B="for (%s;%s;%s) ...";_tag_fat(_tmp2B,sizeof(char),19U);});Cyc_aprintf(_tmp207,_tag_fat(_tmp2A,sizeof(void*),3));});});}case 10: _tmp21=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp20)->f1;{struct Cyc_Absyn_Exp*e=_tmp21;
# 413
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp1B0;_tmp1B0.tag=0,({struct _fat_ptr _tmp208=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1B0.f1=_tmp208;});_tmp1B0;});void*_tmp2F[1];_tmp2F[0]=& _tmp31;({struct _fat_ptr _tmp209=({const char*_tmp30="switch (%s) ...";_tag_fat(_tmp30,sizeof(char),16U);});Cyc_aprintf(_tmp209,_tag_fat(_tmp2F,sizeof(void*),1));});});}case 14: _tmp21=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp20)->f2).f1;{struct Cyc_Absyn_Exp*e=_tmp21;
# 415
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34=({struct Cyc_String_pa_PrintArg_struct _tmp1B1;_tmp1B1.tag=0,({struct _fat_ptr _tmp20A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1B1.f1=_tmp20A;});_tmp1B1;});void*_tmp32[1];_tmp32[0]=& _tmp34;({struct _fat_ptr _tmp20B=({const char*_tmp33="do ... while (%s)";_tag_fat(_tmp33,sizeof(char),18U);});Cyc_aprintf(_tmp20B,_tag_fat(_tmp32,sizeof(void*),1));});});}case 15: _tmp21=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp20)->f1;{struct Cyc_Absyn_Stmt*s=_tmp21;
# 417
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37=({struct Cyc_String_pa_PrintArg_struct _tmp1B2;_tmp1B2.tag=0,({struct _fat_ptr _tmp20C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_stmt2shortstring(s));_tmp1B2.f1=_tmp20C;});_tmp1B2;});void*_tmp35[1];_tmp35[0]=& _tmp37;({struct _fat_ptr _tmp20D=({const char*_tmp36="try %s catch ...";_tag_fat(_tmp36,sizeof(char),17U);});Cyc_aprintf(_tmp20D,_tag_fat(_tmp35,sizeof(void*),1));});});}case 2: _tmp21=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp20)->f1;{struct Cyc_Absyn_Stmt*s=_tmp21;
return Cyc_Vcgen_stmt2shortstring(s);}default:  {
# 420
struct _fat_ptr _tmp38=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_tmp38;
unsigned max=_get_fat_size(s,sizeof(char))< (unsigned)11?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _tmp3C=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_tmp3C),1U,_tmp3C);});
{int i=0;for(0;(unsigned)i < max - 1U;++ i){
({char _tmp20E=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_tmp20E;});}}
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B=({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0,_tmp1B3.f1=(struct _fat_ptr)((struct _fat_ptr)res);_tmp1B3;});void*_tmp39[1];_tmp39[0]=& _tmp3B;({struct _fat_ptr _tmp20F=({const char*_tmp3A="%s...";_tag_fat(_tmp3A,sizeof(char),6U);});Cyc_aprintf(_tmp20F,_tag_fat(_tmp39,sizeof(void*),1));});});}};}
# 480 "vcgen.cyc"
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 483
LOOP:
 ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);
# 488
{void*_tmp3D=ain;if(*((int*)_tmp3D)== 1)
return ain;else{
goto _LL0;}_LL0:;}{
# 492
void*_tmp3E=s->r;void*_stmttmp1=_tmp3E;void*_tmp3F=_stmttmp1;void*_tmp45;void*_tmp44;void*_tmp43;void*_tmp42;void*_tmp41;void*_tmp40;switch(*((int*)_tmp3F)){case 0:
 return ain;case 1: _tmp40=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;{struct Cyc_Absyn_Exp*e=_tmp40;
# 495
struct _tuple12 _tmp46=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp2=_tmp46;struct _tuple12 _tmp47=_stmttmp2;void*_tmp49;void*_tmp48;_tmp48=_tmp47.f1;_tmp49=_tmp47.f2;{void*t=_tmp48;void*aout=_tmp49;
# 498
if((env->shared)->exp_stmt != 0)
*((void**)_check_null((env->shared)->exp_stmt))=t;
return aout;}}case 2: _tmp40=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp40;struct Cyc_Absyn_Stmt*s2=_tmp41;
# 502
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain);
s=s2;
goto LOOP;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1 == 0){
# 506
Cyc_Vcgen_update_return(env,0,ain,0);
return(void*)& Cyc_AssnDef_false_assn;}else{_tmp40=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;{struct Cyc_Absyn_Exp*e=_tmp40;
# 509
struct _tuple12 _tmp4A=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(e),ain);struct _tuple12 _stmttmp3=_tmp4A;struct _tuple12 _tmp4B=_stmttmp3;void*_tmp4D;void*_tmp4C;_tmp4C=_tmp4B.f1;_tmp4D=_tmp4B.f2;{void*t=_tmp4C;void*aout=_tmp4D;
Cyc_Vcgen_update_return(env,t,aout,e->topt);
return(void*)& Cyc_AssnDef_false_assn;}}}case 4: _tmp40=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;_tmp42=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3F)->f3;{struct Cyc_Absyn_Exp*e1=_tmp40;struct Cyc_Absyn_Stmt*s1=_tmp41;struct Cyc_Absyn_Stmt*s2=_tmp42;
# 513
struct _tuple12 _tmp4E=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp4=_tmp4E;struct _tuple12 _tmp4F=_stmttmp4;void*_tmp51;void*_tmp50;_tmp50=_tmp4F.f1;_tmp51=_tmp4F.f2;{void*at=_tmp50;void*af=_tmp51;
struct Cyc_Vcgen_Env*_tmp52=Cyc_Vcgen_copy_env(env);struct Cyc_Vcgen_Env*env2=_tmp52;
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return Cyc_Vcgen_widen(Cyc_AssnDef_or(at,af));}}case 5: _tmp40=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1).f1;_tmp41=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1).f2;_tmp42=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Exp*e=_tmp40;struct Cyc_Absyn_Stmt*dummy=_tmp41;struct Cyc_Absyn_Stmt*s=_tmp42;
# 525
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);{
struct _tuple12 _tmp53=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple12 _stmttmp5=_tmp53;struct _tuple12 _tmp54=_stmttmp5;void*_tmp56;void*_tmp55;_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;{void*at=_tmp55;void*af=_tmp56;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7:
# 531
 goto _LL17;{
# 534
struct Cyc_Absyn_Stmt*_tmp57=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp57;
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_AssnDef_false_assn;}case 6: _LL17: {
# 541
struct Cyc_Absyn_Stmt*_tmp58=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp58;
if(dest == 0)
Cyc_Vcgen_update_return(env,0,ain,0);else{
# 545
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_AssnDef_false_assn;}case 9: _tmp40=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f1;_tmp42=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f2;_tmp43=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f3).f1;_tmp44=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f3).f2;_tmp45=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f4;{struct Cyc_Absyn_Exp*e1=_tmp40;struct Cyc_Absyn_Exp*e2=_tmp41;struct Cyc_Absyn_Stmt*dummy2=_tmp42;struct Cyc_Absyn_Exp*e3=_tmp43;struct Cyc_Absyn_Stmt*dummy3=_tmp44;struct Cyc_Absyn_Stmt*s=_tmp45;
# 548
struct _tuple12 _tmp59=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp6=_tmp59;struct _tuple12 _tmp5A=_stmttmp6;void*_tmp5B;_tmp5B=_tmp5A.f2;{void*a=_tmp5B;
ain=a;
# 551
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);{
struct _tuple12 _tmp5C=Cyc_Vcgen_vcgen_test(env,e2,ain);struct _tuple12 _stmttmp7=_tmp5C;struct _tuple12 _tmp5D=_stmttmp7;void*_tmp5F;void*_tmp5E;_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;{void*at=_tmp5E;void*af=_tmp5F;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 556
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain)).f2;
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2)))return af;}}}}}case 14: _tmp40=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f1;_tmp42=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f2;{struct Cyc_Absyn_Stmt*s=_tmp40;struct Cyc_Absyn_Exp*e=_tmp41;struct Cyc_Absyn_Stmt*dummy=_tmp42;
# 562
while(1){
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple12 _tmp60=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple12 _stmttmp8=_tmp60;struct _tuple12 _tmp61=_stmttmp8;void*_tmp63;void*_tmp62;_tmp62=_tmp61.f1;_tmp63=_tmp61.f2;{void*at=_tmp62;void*af=_tmp63;
ain=Cyc_Vcgen_widen(({void*_tmp210=*Cyc_Vcgen_get_assn(env,dummy);Cyc_AssnDef_or(_tmp210,ain);}));
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10: _tmp40=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Exp*e=_tmp40;struct Cyc_List_List*scs=_tmp41;
# 575
struct _tuple12 _tmp64=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp9=_tmp64;struct _tuple12 _tmp65=_stmttmp9;void*_tmp67;void*_tmp66;_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{void*t=_tmp66;void*a=_tmp67;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2 == 0)
({void*_tmp68=0U;({struct _fat_ptr _tmp211=({const char*_tmp69="vcgen fallthru";_tag_fat(_tmp69,sizeof(char),15U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp211,_tag_fat(_tmp68,sizeof(void*),0));});});else{_tmp40=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_List_List*es=_tmp40;struct Cyc_Absyn_Switch_clause*dest=_tmp41;
# 580
struct _tuple14 _tmp6A=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmpA=_tmp6A;struct _tuple14 _tmp6B=_stmttmpA;void*_tmp6D;void*_tmp6C;_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;{struct Cyc_List_List*ts=_tmp6C;void*a=_tmp6D;
if(ts != 0){
# 583
struct Cyc_List_List*_tmp6E=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(dest->pat_vars))->v)).f1);struct Cyc_List_List*x=_tmp6E;
# 585
for(1;x != 0;(x=x->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*_tmp6F=(struct Cyc_Absyn_Vardecl*)x->hd;struct Cyc_Absyn_Vardecl*vd=_tmp6F;
if(vd != 0)
a=({struct Cyc_Vcgen_Env*_tmp214=env;void*_tmp213=a;void*_tmp212=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=1,_tmp70->f1=vd;_tmp70;});Cyc_Vcgen_do_assign(_tmp214,_tmp213,_tmp212,(void*)((struct Cyc_List_List*)_check_null(ts))->hd);});}}
# 591
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_AssnDef_false_assn;}}}case 12: _tmp40=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Decl*d=_tmp40;struct Cyc_Absyn_Stmt*s1=_tmp41;
# 594
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13: _tmp40=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Stmt*s=_tmp40;
# 602
return Cyc_Vcgen_vcgen_stmt_nodebug(env,s,(void*)& Cyc_AssnDef_true_assn);}case 8:
 return(void*)& Cyc_AssnDef_false_assn;default: _tmp40=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Stmt*s=_tmp40;struct Cyc_List_List*scs=_tmp41;
# 606
void**_tmp71=(env->shared)->try_assn;void**old_try_assn=_tmp71;
# 608
({void**_tmp215=({void**_tmp72=_cycalloc(sizeof(*_tmp72));*_tmp72=(void*)& Cyc_AssnDef_false_assn;_tmp72;});(env->shared)->try_assn=_tmp215;});
# 611
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 613
void*_tmp73=*((void**)_check_null((env->shared)->try_assn));void*new_try_assn=_tmp73;
(env->shared)->try_assn=old_try_assn;
# 617
return({void*_tmp216=ain;Cyc_AssnDef_or(_tmp216,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}};}}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 621
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 625
void*res=(void*)& Cyc_AssnDef_false_assn;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp74=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmp74;
void*_tmp75=ain;void*a=_tmp75;
{struct Cyc_List_List*_tmp76=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sc->pat_vars))->v;struct Cyc_List_List*pvs=_tmp76;for(0;pvs != 0;pvs=pvs->tl){
struct _tuple15*_tmp77=(struct _tuple15*)pvs->hd;struct _tuple15*_stmttmpB=_tmp77;struct _tuple15*_tmp78=_stmttmpB;void*_tmp7A;void*_tmp79;_tmp79=_tmp78->f1;_tmp7A=_tmp78->f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmp79;struct Cyc_Absyn_Exp*eopt=_tmp7A;
if(vdopt != 0)
# 636
a=({struct Cyc_Vcgen_Env*_tmp219=env;void*_tmp218=a;void*_tmp217=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=1,_tmp7B->f1=*vdopt;_tmp7B;});Cyc_Vcgen_do_assign(_tmp219,_tmp218,_tmp217,Cyc_AssnDef_fresh_var((*vdopt)->type));});}}}
# 639
if(sc->where_clause != 0)
a=(Cyc_Vcgen_vcgen_test(env,(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause),a)).f1;
res=({void*_tmp21A=res;Cyc_AssnDef_or(_tmp21A,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 643
return res;}
# 646
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 649
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp7C=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=1,_tmp81->f1=vd;_tmp81;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv=_tmp7C;
a=({struct Cyc_Vcgen_Env*_tmp21D=env;void*_tmp21C=a;void*_tmp21B=(void*)tv;Cyc_Vcgen_do_assign(_tmp21D,_tmp21C,_tmp21B,Cyc_AssnDef_fresh_var(vd->type));});
if(vd->initializer != 0){
struct _tuple12 _tmp7D=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),a);struct _tuple12 _stmttmpC=_tmp7D;struct _tuple12 _tmp7E=_stmttmpC;void*_tmp80;void*_tmp7F;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;{void*t=_tmp7F;void*a1=_tmp80;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 655
return a;}
# 658
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_tmp82=d->r;void*_stmttmpD=_tmp82;void*_tmp83=_stmttmpD;void*_tmp86;void*_tmp85;void*_tmp84;switch(*((int*)_tmp83)){case 0: _tmp84=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp83)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp84;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2: _tmp84=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp83)->f1;_tmp85=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp83)->f2;_tmp86=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp83)->f3;{struct Cyc_Absyn_Pat*p=_tmp84;struct Cyc_Core_Opt*vds=_tmp85;struct Cyc_Absyn_Exp*e=_tmp86;
# 662
struct _tuple12 _tmp87=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple12 _stmttmpE=_tmp87;struct _tuple12 _tmp88=_stmttmpE;void*_tmp89;_tmp89=_tmp88.f2;{void*a=_tmp89;
{struct Cyc_List_List*_tmp8A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds))->v;struct Cyc_List_List*vds=_tmp8A;for(0;vds != 0;vds=vds->tl){
struct _tuple15 _tmp8B=*((struct _tuple15*)vds->hd);struct _tuple15 _stmttmpF=_tmp8B;struct _tuple15 _tmp8C=_stmttmpF;void*_tmp8E;void*_tmp8D;_tmp8D=_tmp8C.f1;_tmp8E=_tmp8C.f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmp8D;struct Cyc_Absyn_Exp*eopt=_tmp8E;
if(vdopt != 0)
# 667
a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);}}}
# 678 "vcgen.cyc"
return a;}}case 3: _tmp84=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp83)->f1;{struct Cyc_List_List*vds=_tmp84;
# 680
for(1;vds != 0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4: _tmp84=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp83)->f2;_tmp85=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp83)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp84;struct Cyc_Absyn_Exp*eopt=_tmp85;
# 684
if(eopt != 0){
struct _tuple12 _tmp8F=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple12 _stmttmp10=_tmp8F;struct _tuple12 _tmp90=_stmttmp10;void*_tmp92;void*_tmp91;_tmp91=_tmp90.f1;_tmp92=_tmp90.f2;{void*t=_tmp91;void*a1=_tmp92;
a=a1;}}
# 689
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default:
 return a;};}
# 697
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp93=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*t1=_tmp93;
void*_tmp94=Cyc_Absyn_compress((void*)_check_null(e2->topt));void*t2=_tmp94;
struct _tuple12 _tmp95=({struct _tuple12 _tmp1B4;_tmp1B4.f1=t1,_tmp1B4.f2=t2;_tmp1B4;});struct _tuple12 _stmttmp11=_tmp95;struct _tuple12 _tmp96=_stmttmp11;if(*((int*)_tmp96.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96.f1)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96.f1)->f1)->f1 == Cyc_Absyn_Unsigned)
return 1;else{if(*((int*)_tmp96.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_tmp96.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_tmp96.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3:
 return 1;}else{if(*((int*)_tmp96.f1)== 3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_tmp96.f1)== 3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_tmp96.f1)== 3){if(*((int*)_tmp96.f2)== 3)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}}};}
# 713
static struct _tuple12 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp97=e->r;void*_stmttmp12=_tmp97;void*_tmp98=_stmttmp12;enum Cyc_Absyn_Primop _tmp99;void*_tmp9C;void*_tmp9B;void*_tmp9A;switch(*((int*)_tmp98)){case 6: _tmp9A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_tmp9B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp98)->f2;_tmp9C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp98)->f3;{struct Cyc_Absyn_Exp*e1=_tmp9A;struct Cyc_Absyn_Exp*e2=_tmp9B;struct Cyc_Absyn_Exp*e3=_tmp9C;
# 716
struct _tuple12 _tmp9D=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp13=_tmp9D;struct _tuple12 _tmp9E=_stmttmp13;void*_tmpA0;void*_tmp9F;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;{void*a1t=_tmp9F;void*a1f=_tmpA0;
struct _tuple12 _tmpA1=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple12 _stmttmp14=_tmpA1;struct _tuple12 _tmpA2=_stmttmp14;void*_tmpA4;void*_tmpA3;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;{void*a2t=_tmpA3;void*a2f=_tmpA4;
struct _tuple12 _tmpA5=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct _tuple12 _stmttmp15=_tmpA5;struct _tuple12 _tmpA6=_stmttmp15;void*_tmpA8;void*_tmpA7;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;{void*a3t=_tmpA7;void*a3f=_tmpA8;
return({struct _tuple12 _tmp1B5;({void*_tmp21F=Cyc_AssnDef_or(a2t,a3t);_tmp1B5.f1=_tmp21F;}),({void*_tmp21E=Cyc_AssnDef_or(a2f,a3f);_tmp1B5.f2=_tmp21E;});_tmp1B5;});}}}}case 7: _tmp9A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_tmp9B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp98)->f2;{struct Cyc_Absyn_Exp*e1=_tmp9A;struct Cyc_Absyn_Exp*e2=_tmp9B;
# 721
struct _tuple12 _tmpA9=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp16=_tmpA9;struct _tuple12 _tmpAA=_stmttmp16;void*_tmpAC;void*_tmpAB;_tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;{void*a1t=_tmpAB;void*a1f=_tmpAC;
struct _tuple12 _tmpAD=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple12 _stmttmp17=_tmpAD;struct _tuple12 _tmpAE=_stmttmp17;void*_tmpB0;void*_tmpAF;_tmpAF=_tmpAE.f1;_tmpB0=_tmpAE.f2;{void*a2t=_tmpAF;void*a2f=_tmpB0;
return({struct _tuple12 _tmp1B6;_tmp1B6.f1=a2t,({void*_tmp220=Cyc_AssnDef_or(a1f,a2f);_tmp1B6.f2=_tmp220;});_tmp1B6;});}}}case 8: _tmp9A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_tmp9B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp98)->f2;{struct Cyc_Absyn_Exp*e1=_tmp9A;struct Cyc_Absyn_Exp*e2=_tmp9B;
# 725
struct _tuple12 _tmpB1=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp18=_tmpB1;struct _tuple12 _tmpB2=_stmttmp18;void*_tmpB4;void*_tmpB3;_tmpB3=_tmpB2.f1;_tmpB4=_tmpB2.f2;{void*a1t=_tmpB3;void*a1f=_tmpB4;
struct _tuple12 _tmpB5=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct _tuple12 _stmttmp19=_tmpB5;struct _tuple12 _tmpB6=_stmttmp19;void*_tmpB8;void*_tmpB7;_tmpB7=_tmpB6.f1;_tmpB8=_tmpB6.f2;{void*a2t=_tmpB7;void*a2f=_tmpB8;
return({struct _tuple12 _tmp1B7;({void*_tmp221=Cyc_AssnDef_or(a1t,a2t);_tmp1B7.f1=_tmp221;}),_tmp1B7.f2=a2f;_tmp1B7;});}}}case 9: _tmp9A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_tmp9B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp98)->f2;{struct Cyc_Absyn_Exp*e1=_tmp9A;struct Cyc_Absyn_Exp*e2=_tmp9B;
# 729
struct _tuple12 _tmpB9=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp1A=_tmpB9;struct _tuple12 _tmpBA=_stmttmp1A;void*_tmpBB;_tmpBB=_tmpBA.f2;{void*ain=_tmpBB;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2 != 0){_tmp9A=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd;{struct Cyc_Absyn_Exp*e1=_tmp9A;
# 732
struct _tuple12 _tmpBC=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp1B=_tmpBC;struct _tuple12 _tmpBD=_stmttmp1B;void*_tmpBF;void*_tmpBE;_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;{void*at=_tmpBE;void*af=_tmpBF;
return({struct _tuple12 _tmp1B8;_tmp1B8.f1=af,_tmp1B8.f2=at;_tmp1B8;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->tl == 0){_tmp99=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_tmp9A=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->hd;_tmp9B=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp98)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmp99;struct Cyc_Absyn_Exp*e1=_tmp9A;struct Cyc_Absyn_Exp*e2=_tmp9B;
# 735
struct _tuple12 _tmpC0=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp1C=_tmpC0;struct _tuple12 _tmpC1=_stmttmp1C;void*_tmpC3;void*_tmpC2;_tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;{void*t1=_tmpC2;void*a1=_tmpC3;
struct _tuple12 _tmpC4=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp1D=_tmpC4;struct _tuple12 _tmpC5=_stmttmp1D;void*_tmpC7;void*_tmpC6;_tmpC6=_tmpC5.f1;_tmpC7=_tmpC5.f2;{void*t2=_tmpC6;void*a2=_tmpC7;
void*_tmpC8=a2;void*at=_tmpC8;
void*_tmpC9=a2;void*af=_tmpC9;
{enum Cyc_Absyn_Primop _tmpCA=p;switch((int)_tmpCA){case Cyc_Absyn_Eq:
# 743
 at=({void*_tmp222=at;Cyc_AssnDef_and(_tmp222,Cyc_AssnDef_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Neq:
 af=({void*_tmp223=af;Cyc_AssnDef_and(_tmp223,Cyc_AssnDef_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Lt:
# 746
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp224=at;Cyc_AssnDef_and(_tmp224,Cyc_AssnDef_ult(t1,t2));});af=({void*_tmp225=af;Cyc_AssnDef_and(_tmp225,Cyc_AssnDef_ulte(t2,t1));});}else{
# 749
at=({void*_tmp226=at;Cyc_AssnDef_and(_tmp226,Cyc_AssnDef_slt(t1,t2));});af=({void*_tmp227=af;Cyc_AssnDef_and(_tmp227,Cyc_AssnDef_slte(t2,t1));});}
# 751
goto _LL30;case Cyc_Absyn_Lte:
# 753
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp228=at;Cyc_AssnDef_and(_tmp228,Cyc_AssnDef_ulte(t1,t2));});af=({void*_tmp229=af;Cyc_AssnDef_and(_tmp229,Cyc_AssnDef_ult(t2,t1));});}else{
# 756
at=({void*_tmp22A=at;Cyc_AssnDef_and(_tmp22A,Cyc_AssnDef_slte(t1,t2));});af=({void*_tmp22B=af;Cyc_AssnDef_and(_tmp22B,Cyc_AssnDef_slt(t2,t1));});}
# 758
goto _LL30;case Cyc_Absyn_Gt:
# 760
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp22C=at;Cyc_AssnDef_and(_tmp22C,Cyc_AssnDef_ult(t2,t1));});af=({void*_tmp22D=af;Cyc_AssnDef_and(_tmp22D,Cyc_AssnDef_ulte(t1,t2));});}else{
# 763
at=({void*_tmp22E=at;Cyc_AssnDef_and(_tmp22E,Cyc_AssnDef_slt(t2,t1));});af=({void*_tmp22F=af;Cyc_AssnDef_and(_tmp22F,Cyc_AssnDef_slte(t1,t2));});}
# 765
goto _LL30;case Cyc_Absyn_Gte:
# 767
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp230=at;Cyc_AssnDef_and(_tmp230,Cyc_AssnDef_ulte(t2,t1));});af=({void*_tmp231=af;Cyc_AssnDef_and(_tmp231,Cyc_AssnDef_ult(t1,t2));});}else{
# 770
at=({void*_tmp232=at;Cyc_AssnDef_and(_tmp232,Cyc_AssnDef_slte(t2,t1));});af=({void*_tmp233=af;Cyc_AssnDef_and(_tmp233,Cyc_AssnDef_slt(t1,t2));});}
# 772
goto _LL30;default:
 goto _LL30;}_LL30:;}
# 775
return({struct _tuple12 _tmp1B9;_tmp1B9.f1=at,_tmp1B9.f2=af;_tmp1B9;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: {
# 777
struct _tuple12 _tmpCB=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp1E=_tmpCB;struct _tuple12 _tmpCC=_stmttmp1E;void*_tmpCE;void*_tmpCD;_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;{void*t=_tmpCD;void*aout=_tmpCE;
return({struct _tuple12 _tmp1BA;({void*_tmp239=({void*_tmp238=aout;Cyc_AssnDef_and(_tmp238,({void*_tmp237=Cyc_AssnDef_one();Cyc_AssnDef_ulte(_tmp237,t);}));});_tmp1BA.f1=_tmp239;}),({void*_tmp236=({void*_tmp235=aout;Cyc_AssnDef_and(_tmp235,({void*_tmp234=t;Cyc_AssnDef_eq(_tmp234,Cyc_AssnDef_zero());}));});_tmp1BA.f2=_tmp236;});_tmp1BA;});}}};}
# 782
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(1;es != 0;es=es->tl){
a=(Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a)).f2;}
return a;}
# 788
static struct _tuple13 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(1;es != 0;es=es->tl){
struct _tuple12 _tmpCF=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);struct _tuple12 _stmttmp1F=_tmpCF;struct _tuple12 _tmpD0=_stmttmp1F;void*_tmpD2;void*_tmpD1;_tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;{void*t=_tmpD1;void*aout=_tmpD2;
a=aout;
if(valid && t != 0)
res=({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=t,_tmpD3->tl=res;_tmpD3;});else{
# 797
valid=0;}}}
# 800
return({struct _tuple13 _tmp1BB;valid?({struct Cyc_List_List**_tmp23B=({struct Cyc_List_List**_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct Cyc_List_List*_tmp23A=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res);*_tmpD4=_tmp23A;});_tmpD4;});_tmp1BB.f1=_tmp23B;}):(_tmp1BB.f1=0),_tmp1BB.f2=a;_tmp1BB;});}
# 803
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple12 _tmpD5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp20=_tmpD5;struct _tuple12 _tmpD6=_stmttmp20;void*_tmpD8;void*_tmpD7;_tmpD7=_tmpD6.f1;_tmpD8=_tmpD6.f2;{void*topt=_tmpD7;void*a=_tmpD8;
return({struct _tuple12 _tmp1BC;topt == 0?({void*_tmp23C=Cyc_AssnDef_fresh_var(e->topt);_tmp1BC.f1=_tmp23C;}):(_tmp1BC.f1=topt),_tmp1BC.f2=a;_tmp1BC;});}}
# 808
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(1;es != 0;es=es->tl){
struct _tuple12 _tmpD9=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);struct _tuple12 _stmttmp21=_tmpD9;struct _tuple12 _tmpDA=_stmttmp21;void*_tmpDC;void*_tmpDB;_tmpDB=_tmpDA.f1;_tmpDC=_tmpDA.f2;{void*t=_tmpDB;void*a=_tmpDC;
ain=a;
res=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->hd=t,_tmpDD->tl=res;_tmpDD;});}}
# 815
return({struct _tuple14 _tmp1BD;({struct Cyc_List_List*_tmp23D=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res);_tmp1BD.f1=_tmp23D;}),_tmp1BD.f2=ain;_tmp1BD;});}
# 818
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_tmpDE=b;void*_tmpDF;switch(*((int*)_tmpDE)){case 1: _tmpDF=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpDE)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDF;
return vd;}case 2: _tmpDF=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpDE)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpDF;
return(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);}case 0:
({void*_tmpE0=0U;({struct _fat_ptr _tmp23E=({const char*_tmpE1="vcgen bind2vardecl";_tag_fat(_tmpE1,sizeof(char),19U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp23E,_tag_fat(_tmpE0,sizeof(void*),0));});});case 3: _tmpDF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpDE)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDF;
return vd;}case 4: _tmpDF=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpDE)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDF;
return vd;}default: _tmpDF=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpDE)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDF;
return vd;}};}
# 829
static struct _tuple12 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmpE2=e->r;void*_stmttmp22=_tmpE2;void*_tmpE3=_stmttmp22;void*_tmpE4;if(*((int*)_tmpE3)== 1){_tmpE4=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;{void*b=_tmpE4;
# 832
return({struct _tuple12 _tmp1BE;({void*_tmp240=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->tag=1,({struct Cyc_Absyn_Vardecl*_tmp23F=Cyc_Vcgen_bind2vardecl(b);_tmpE5->f1=_tmp23F;});_tmpE5;});_tmp1BE.f1=_tmp240;}),_tmp1BE.f2=ain;_tmp1BE;});}}else{
return({struct _tuple12 _tmp1BF;_tmp1BF.f1=0,({void*_tmp241=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp1BF.f2=_tmp241;});_tmp1BF;});};}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Vardecl*f4;};
# 839
static struct _tuple16 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple16 _tmp1C0;_tmp1C0.f1=fi->args,_tmp1C0.f2=fi->requires_clause,_tmp1C0.f3=fi->ensures_clause,_tmp1C0.f4=fi->return_value;_tmp1C0;});}
# 843
static struct _tuple16 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt != 0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_tmpE6=t;void*_tmpE7;if(*((int*)_tmpE6)== 5){_tmpE7=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE6)->f1;{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_tmpE7;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{
goto _LL0;}_LL0:;}
# 851
return({struct _tuple16 _tmp1C1;_tmp1C1.f1=0,_tmp1C1.f2=0,_tmp1C1.f3=0,_tmp1C1.f4=0;_tmp1C1;});}
# 854
static void Cyc_Vcgen_check_requires(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*args,void*a,struct Cyc_Absyn_Exp*reqopt){
# 857
if(reqopt != 0){
struct Cyc_Absyn_Exp*req=reqopt;
# 861
for(1;args != 0 && inputs != 0;(args=args->tl,inputs=inputs->tl)){
struct _tuple9*_tmpE8=(struct _tuple9*)args->hd;struct _tuple9*_stmttmp23=_tmpE8;struct _tuple9*_tmpE9=_stmttmp23;void*_tmpEC;struct Cyc_Absyn_Tqual _tmpEB;void*_tmpEA;_tmpEA=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEC=_tmpE9->f3;{struct _fat_ptr*vopt=_tmpEA;struct Cyc_Absyn_Tqual tq=_tmpEB;void*t=_tmpEC;
if(vopt != 0){
# 865
struct _tuple1*qv=({struct _tuple1*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->f1=Cyc_Absyn_Loc_n,_tmpF0->f2=vopt;_tmpF0;});
struct Cyc_Absyn_Vardecl*_tmpED=({struct Cyc_Absyn_Vardecl*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->sc=Cyc_Absyn_Public,_tmpEF->name=qv,_tmpEF->varloc=0U,_tmpEF->tq=tq,_tmpEF->type=t,_tmpEF->initializer=0,_tmpEF->rgn=0,_tmpEF->attributes=0,_tmpEF->escapes=0,_tmpEF->is_proto=0;_tmpEF;});struct Cyc_Absyn_Vardecl*vd=_tmpED;
# 871
a=({void*_tmp243=a;Cyc_AssnDef_and(_tmp243,({void*_tmp242=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=1,_tmpEE->f1=vd;_tmpEE;});Cyc_AssnDef_eq(_tmp242,(void*)inputs->hd);}));});}}}{
# 875
struct _tuple12 _tmpF1=Cyc_Vcgen_vcgen_test(env,reqopt,(void*)& Cyc_AssnDef_true_assn);struct _tuple12 _stmttmp24=_tmpF1;struct _tuple12 _tmpF2=_stmttmp24;void*_tmpF3;_tmpF3=_tmpF2.f1;{void*at=_tmpF3;
if(!Cyc_Vcgen_implies(& a,& at))
({struct Cyc_String_pa_PrintArg_struct _tmpF6=({struct Cyc_String_pa_PrintArg_struct _tmp1C3;_tmp1C3.tag=0,({
struct _fat_ptr _tmp244=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp1C3.f1=_tmp244;});_tmp1C3;});struct Cyc_String_pa_PrintArg_struct _tmpF7=({struct Cyc_String_pa_PrintArg_struct _tmp1C2;_tmp1C2.tag=0,({struct _fat_ptr _tmp245=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(a)));_tmp1C2.f1=_tmp245;});_tmp1C2;});void*_tmpF4[2];_tmpF4[0]=& _tmpF6,_tmpF4[1]=& _tmpF7;({unsigned _tmp247=req->loc;struct _fat_ptr _tmp246=({const char*_tmpF5="cannot prove @requires %s from %s";_tag_fat(_tmpF5,sizeof(char),34U);});Cyc_Warn_err(_tmp247,_tmp246,_tag_fat(_tmpF4,sizeof(void*),2));});});}}}}
# 882
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 886
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e))return({struct _tuple12 _tmp1C4;({void*_tmp248=Cyc_AssnDef_cnst(e);_tmp1C4.f1=_tmp248;}),_tmp1C4.f2=ain;_tmp1C4;});{
void*_tmpF8=e->r;void*_stmttmp25=_tmpF8;void*_tmpF9=_stmttmp25;struct _fat_ptr _tmpFB;struct Cyc_Absyn_MallocInfo _tmpFD;enum Cyc_Absyn_Coercion _tmpFF;enum Cyc_Absyn_Incrementor _tmp100;void*_tmpFE;void*_tmpFC;enum Cyc_Absyn_Primop _tmp101;void*_tmpFA;union Cyc_Absyn_Cnst _tmp102;switch(*((int*)_tmpF9)){case 0: _tmp102=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{union Cyc_Absyn_Cnst c=_tmp102;
# 891
return({struct _tuple12 _tmp1C5;({void*_tmp249=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=0,_tmp103->f1=e;_tmp103;});_tmp1C5.f1=_tmp249;}),_tmp1C5.f2=ain;_tmp1C5;});}case 1: _tmpFA=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{void*b=_tmpFA;
return({struct _tuple12 _tmp1C6;({void*_tmp24B=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=1,({struct Cyc_Absyn_Vardecl*_tmp24A=Cyc_Vcgen_bind2vardecl(b);_tmp104->f1=_tmp24A;});_tmp104;});_tmp1C6.f1=_tmp24B;}),_tmp1C6.f2=ain;_tmp1C6;});}case 3: _tmp101=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{enum Cyc_Absyn_Primop p=_tmp101;struct Cyc_List_List*es=_tmpFA;
# 894
struct _tuple13 _tmp105=Cyc_Vcgen_vcgen_rexps(env,es,ain);struct _tuple13 _stmttmp26=_tmp105;struct _tuple13 _tmp106=_stmttmp26;void*_tmp108;void*_tmp107;_tmp107=_tmp106.f1;_tmp108=_tmp106.f2;{struct Cyc_List_List**ts=_tmp107;void*aout=_tmp108;
void*t=0;
if(ts != 0)t=Cyc_AssnDef_primop(p,*ts);
return({struct _tuple12 _tmp1C7;_tmp1C7.f1=t,_tmp1C7.f2=aout;_tmp1C7;});}}case 4: _tmpFA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;_tmpFE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF9)->f3;{struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_Core_Opt*popt=_tmpFC;struct Cyc_Absyn_Exp*e2=_tmpFE;
# 899
struct _tuple12 _tmp109=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple12 _stmttmp27=_tmp109;struct _tuple12 _tmp10A=_stmttmp27;void*_tmp10C;void*_tmp10B;_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;{void*lt=_tmp10B;void*a1=_tmp10C;
struct _tuple12 _tmp10D=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp28=_tmp10D;struct _tuple12 _tmp10E=_stmttmp28;void*_tmp110;void*_tmp10F;_tmp10F=_tmp10E.f1;_tmp110=_tmp10E.f2;{void*t2=_tmp10F;void*a2=_tmp110;
if(popt != 0){
void*_tmp111=Cyc_Vcgen_deref_lterm(env,lt,a2);void*t1=_tmp111;
if(t1 == 0 || t2 == 0)
t2=0;else{
# 906
t2=({enum Cyc_Absyn_Primop _tmp24C=(enum Cyc_Absyn_Primop)popt->v;Cyc_AssnDef_primop(_tmp24C,({void*_tmp112[2];_tmp112[0]=t1,_tmp112[1]=t2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp112,sizeof(void*),2));}));});}}{
# 908
void*_tmp113=Cyc_Vcgen_do_assign(env,a2,lt,t2);void*a=_tmp113;
return({struct _tuple12 _tmp1C8;_tmp1C8.f1=lt,_tmp1C8.f2=a;_tmp1C8;});}}}}case 5: _tmpFA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmp100=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{struct Cyc_Absyn_Exp*e=_tmpFA;enum Cyc_Absyn_Incrementor i=_tmp100;
# 911
struct _tuple12 _tmp114=Cyc_Vcgen_vcgen_lexp(env,e,ain);struct _tuple12 _stmttmp29=_tmp114;struct _tuple12 _tmp115=_stmttmp29;void*_tmp117;void*_tmp116;_tmp116=_tmp115.f1;_tmp117=_tmp115.f2;{void*lt=_tmp116;void*a1=_tmp117;
void*_tmp118=Cyc_Vcgen_deref_lterm(env,lt,a1);void*t1=_tmp118;
void*res=0;
void*assn=0;
{enum Cyc_Absyn_Incrementor _tmp119=i;switch((int)_tmp119){case Cyc_Absyn_PreInc:
# 917
 if(t1 != 0)assn=Cyc_AssnDef_primop(Cyc_Absyn_Plus,({void*_tmp11A[2];_tmp11A[0]=t1,({void*_tmp24D=Cyc_AssnDef_one();_tmp11A[1]=_tmp24D;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp11A,sizeof(void*),2));}));
res=assn;
goto _LL63;case Cyc_Absyn_PostInc:
# 921
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(Cyc_Absyn_Plus,({void*_tmp11B[2];_tmp11B[0]=t1,({void*_tmp24E=Cyc_AssnDef_one();_tmp11B[1]=_tmp24E;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp11B,sizeof(void*),2));}));
goto _LL63;case Cyc_Absyn_PreDec:
# 925
 if(t1 != 0)assn=Cyc_AssnDef_primop(Cyc_Absyn_Minus,({void*_tmp11C[2];_tmp11C[0]=t1,({void*_tmp24F=Cyc_AssnDef_one();_tmp11C[1]=_tmp24F;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp11C,sizeof(void*),2));}));
res=assn;
goto _LL63;case Cyc_Absyn_PostDec:
# 929
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(Cyc_Absyn_Minus,({void*_tmp11D[2];_tmp11D[0]=t1,({void*_tmp250=Cyc_AssnDef_one();_tmp11D[1]=_tmp250;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp11D,sizeof(void*),2));}));
goto _LL63;default:
({void*_tmp11E=0U;({struct _fat_ptr _tmp251=({const char*_tmp11F="vcgen vcgen_rexp increment_e";_tag_fat(_tmp11F,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp251,_tag_fat(_tmp11E,sizeof(void*),0));});});}_LL63:;}{
# 934
void*_tmp120=Cyc_Vcgen_do_assign(env,a1,lt,assn);void*a=_tmp120;
return({struct _tuple12 _tmp1C9;_tmp1C9.f1=res,_tmp1C9.f2=a;_tmp1C9;});}}}case 6: _tmpFA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;_tmpFE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF9)->f3;{struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_Absyn_Exp*e2=_tmpFC;struct Cyc_Absyn_Exp*e3=_tmpFE;
# 937
struct _tuple12 _tmp121=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp2A=_tmp121;struct _tuple12 _tmp122=_stmttmp2A;void*_tmp124;void*_tmp123;_tmp123=_tmp122.f1;_tmp124=_tmp122.f2;{void*at=_tmp123;void*af=_tmp124;
struct _tuple12 _tmp125=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp2B=_tmp125;struct _tuple12 _tmp126=_stmttmp2B;void*_tmp128;void*_tmp127;_tmp127=_tmp126.f1;_tmp128=_tmp126.f2;{void*t1=_tmp127;void*at=_tmp128;
struct _tuple12 _tmp129=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);struct _tuple12 _stmttmp2C=_tmp129;struct _tuple12 _tmp12A=_stmttmp2C;void*_tmp12C;void*_tmp12B;_tmp12B=_tmp12A.f1;_tmp12C=_tmp12A.f2;{void*t2=_tmp12B;void*af=_tmp12C;
if(t1 != 0 && t2 != 0){
void*_tmp12D=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp12D;
at=({void*_tmp252=at;Cyc_AssnDef_and(_tmp252,Cyc_AssnDef_eq(v,t1));});
af=({void*_tmp253=af;Cyc_AssnDef_and(_tmp253,Cyc_AssnDef_eq(v,t2));});
return({struct _tuple12 _tmp1CA;_tmp1CA.f1=v,({void*_tmp254=Cyc_Vcgen_widen(Cyc_AssnDef_or(at,af));_tmp1CA.f2=_tmp254;});_tmp1CA;});}else{
# 946
return({struct _tuple12 _tmp1CB;_tmp1CB.f1=0,({void*_tmp255=Cyc_AssnDef_or(at,af);_tmp1CB.f2=_tmp255;});_tmp1CB;});}}}}}case 7: _tmpFA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_Absyn_Exp*e2=_tmpFC;
# 948
struct _tuple12 _tmp12E=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp2D=_tmp12E;struct _tuple12 _tmp12F=_stmttmp2D;void*_tmp131;void*_tmp130;_tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;{void*at=_tmp130;void*af=_tmp131;
struct _tuple12 _tmp132=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp2E=_tmp132;struct _tuple12 _tmp133=_stmttmp2E;void*_tmp135;void*_tmp134;_tmp134=_tmp133.f1;_tmp135=_tmp133.f2;{void*t=_tmp134;void*at=_tmp135;
void*_tmp136=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp136;
at=({void*_tmp256=at;Cyc_AssnDef_and(_tmp256,Cyc_AssnDef_eq(v,t));});
af=({void*_tmp258=af;Cyc_AssnDef_and(_tmp258,({void*_tmp257=v;Cyc_AssnDef_eq(_tmp257,Cyc_AssnDef_zero());}));});
return({struct _tuple12 _tmp1CC;_tmp1CC.f1=v,({void*_tmp259=Cyc_Vcgen_widen(Cyc_AssnDef_or(at,af));_tmp1CC.f2=_tmp259;});_tmp1CC;});}}}case 8: _tmpFA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_Absyn_Exp*e2=_tmpFC;
# 955
struct _tuple12 _tmp137=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp2F=_tmp137;struct _tuple12 _tmp138=_stmttmp2F;void*_tmp13A;void*_tmp139;_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;{void*at=_tmp139;void*af=_tmp13A;
struct _tuple12 _tmp13B=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);struct _tuple12 _stmttmp30=_tmp13B;struct _tuple12 _tmp13C=_stmttmp30;void*_tmp13E;void*_tmp13D;_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;{void*t=_tmp13D;void*af=_tmp13E;
void*_tmp13F=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp13F;
at=({void*_tmp25B=at;Cyc_AssnDef_and(_tmp25B,({void*_tmp25A=v;Cyc_AssnDef_eq(_tmp25A,Cyc_AssnDef_one());}));});
af=({void*_tmp25C=af;Cyc_AssnDef_and(_tmp25C,Cyc_AssnDef_eq(v,t));});
return({struct _tuple12 _tmp1CD;_tmp1CD.f1=v,({void*_tmp25D=Cyc_Vcgen_widen(Cyc_AssnDef_or(at,af));_tmp1CD.f2=_tmp25D;});_tmp1CD;});}}}case 9: _tmpFA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_Absyn_Exp*e2=_tmpFC;
# 962
struct _tuple12 _tmp140=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp31=_tmp140;struct _tuple12 _tmp141=_stmttmp31;void*_tmp142;_tmp142=_tmp141.f2;{void*a=_tmp142;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10: _tmpFA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{struct Cyc_Absyn_Exp*e=_tmpFA;struct Cyc_List_List*es=_tmpFC;
# 965
struct _tuple14 _tmp143=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp32=_tmp143;struct _tuple14 _tmp144=_stmttmp32;void*_tmp146;void*_tmp145;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct Cyc_List_List*ts=_tmp145;void*a=_tmp146;
struct _tuple12 _tmp147=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple12 _stmttmp33=_tmp147;struct _tuple12 _tmp148=_stmttmp33;void*_tmp149;_tmp149=_tmp148.f2;{void*a2=_tmp149;
# 968
struct _tuple16 _tmp14A=Cyc_Vcgen_get_requires_and_ensures(e->topt);struct _tuple16 _stmttmp34=_tmp14A;struct _tuple16 _tmp14B=_stmttmp34;void*_tmp14F;void*_tmp14E;void*_tmp14D;void*_tmp14C;_tmp14C=_tmp14B.f1;_tmp14D=_tmp14B.f2;_tmp14E=_tmp14B.f3;_tmp14F=_tmp14B.f4;{struct Cyc_List_List*args=_tmp14C;struct Cyc_Absyn_Exp*reqopt=_tmp14D;struct Cyc_Absyn_Exp*ensopt=_tmp14E;struct Cyc_Absyn_Vardecl*ret_value=_tmp14F;
Cyc_Vcgen_check_requires(env,ts,args,a2,reqopt);
# 972
a2=Cyc_AssnDef_kill(a2);
Cyc_Vcgen_update_try_assn(env,a2);
# 975
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e->topt)))a2=(void*)& Cyc_AssnDef_false_assn;
if(ensopt == 0)
return({struct _tuple12 _tmp1CE;_tmp1CE.f1=0,_tmp1CE.f2=a2;_tmp1CE;});{
struct Cyc_Absyn_Exp*ensures=ensopt;
# 980
struct _tuple12 _tmp150=Cyc_Vcgen_vcgen_test(env,ensures,a2);struct _tuple12 _stmttmp35=_tmp150;struct _tuple12 _tmp151=_stmttmp35;void*_tmp152;_tmp152=_tmp151.f1;{void*at=_tmp152;
if(ret_value != 0){
void*_tmp153=Cyc_AssnDef_fresh_var(ret_value->type);void*v=_tmp153;
# 984
at=Cyc_AssnDef_subst(ret_value,v,at);
return({struct _tuple12 _tmp1CF;_tmp1CF.f1=v,_tmp1CF.f2=at;_tmp1CF;});}
# 987
return({struct _tuple12 _tmp1D0;_tmp1D0.f1=0,_tmp1D0.f2=at;_tmp1D0;});}}}}}}case 11: _tmpFA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Exp*e=_tmpFA;
# 989
struct _tuple12 _tmp154=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp36=_tmp154;struct _tuple12 _tmp155=_stmttmp36;void*_tmp156;_tmp156=_tmp155.f2;{void*a=_tmp156;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple12 _tmp1D1;_tmp1D1.f1=0,_tmp1D1.f2=(void*)& Cyc_AssnDef_false_assn;_tmp1D1;});}}case 12: _tmpFA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Exp*e=_tmpFA;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13: _tmpFA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Exp*e=_tmpFA;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14: _tmpFA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;_tmpFF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF9)->f4;{void*tp=_tmpFA;struct Cyc_Absyn_Exp*e=_tmpFC;enum Cyc_Absyn_Coercion c=_tmpFF;
# 997
struct _tuple12 _tmp157=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp37=_tmp157;struct _tuple12 _tmp158=_stmttmp37;void*_tmp15A;void*_tmp159;_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;{void*topt=_tmp159;void*a=_tmp15A;
if((int)c == (int)1U){
if(topt == 0)return({struct _tuple12 _tmp1D2;_tmp1D2.f1=0,_tmp1D2.f2=a;_tmp1D2;});{
void*t=topt;
# 1011 "vcgen.cyc"
{struct _tuple12 _tmp15B=({struct _tuple12 _tmp1D6;({void*_tmp25F=Cyc_Absyn_compress(tp);_tmp1D6.f1=_tmp25F;}),({void*_tmp25E=Cyc_Absyn_compress((void*)_check_null(e->topt));_tmp1D6.f2=_tmp25E;});_tmp1D6;});struct _tuple12 _stmttmp38=_tmp15B;struct _tuple12 _tmp15C=_stmttmp38;enum Cyc_Absyn_Size_of _tmp160;enum Cyc_Absyn_Sign _tmp15F;enum Cyc_Absyn_Size_of _tmp15E;enum Cyc_Absyn_Sign _tmp15D;if(*((int*)_tmp15C.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f1)->f1)== 1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f1)->f1)->f2){case Cyc_Absyn_Int_sz: if(*((int*)_tmp15C.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f2)->f1)){case 4:
# 1015
 return({struct _tuple12 _tmp1D3;_tmp1D3.f1=t,_tmp1D3.f2=a;_tmp1D3;});case 1: goto _LL9D;default: goto _LL9F;}else{goto _LL9F;}case Cyc_Absyn_Long_sz: if(*((int*)_tmp15C.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f2)->f1)){case 4:
# 1017
 return({struct _tuple12 _tmp1D4;_tmp1D4.f1=t,_tmp1D4.f2=a;_tmp1D4;});case 1: goto _LL9D;default: goto _LL9F;}else{goto _LL9F;}default: if(*((int*)_tmp15C.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f2)->f1)== 1){_LL9D: _tmp15D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f1)->f1)->f1;_tmp15E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f1)->f1)->f2;_tmp15F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f2)->f1)->f1;_tmp160=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15C.f2)->f1)->f2;{enum Cyc_Absyn_Sign u1=_tmp15D;enum Cyc_Absyn_Size_of s1=_tmp15E;enum Cyc_Absyn_Sign u2=_tmp15F;enum Cyc_Absyn_Size_of s2=_tmp160;
# 1025
s1=(int)s1 == (int)3U?Cyc_Absyn_Int_sz: s1;
s2=(int)s2 == (int)3U?Cyc_Absyn_Int_sz: s1;
if((int)s1 == (int)s2)return({struct _tuple12 _tmp1D5;_tmp1D5.f1=t,_tmp1D5.f2=a;_tmp1D5;});
goto _LL98;}}else{goto _LL9F;}}else{goto _LL9F;}}else{goto _LL9F;}}else{_LL9F:
 goto _LL98;}_LL98:;}
# 1031
return({struct _tuple12 _tmp1D7;({void*_tmp260=Cyc_AssnDef_cast(tp,t);_tmp1D7.f1=_tmp260;}),_tmp1D7.f2=a;_tmp1D7;});}}else{
# 1034
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple12 _tmp1D8;topt == 0?_tmp1D8.f1=0:({void*_tmp261=Cyc_AssnDef_cast(tp,topt);_tmp1D8.f1=_tmp261;}),_tmp1D8.f2=a;_tmp1D8;});}}}case 16: _tmpFA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{struct Cyc_Absyn_Exp*eopt=_tmpFA;struct Cyc_Absyn_Exp*e=_tmpFC;
# 1038
if(eopt != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain)).f2;
return({struct _tuple12 _tmp1D9;_tmp1D9.f1=0,({void*_tmp262=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp1D9.f2=_tmp262;});_tmp1D9;});}case 32:
# 1043
 goto _LL22;case 33: _LL22:
 goto _LL24;case 17: _LL24:
 goto _LL26;case 18: _LL26:
 goto _LL28;case 39: _LL28:
 goto _LL2A;case 19: _LL2A:
 return({struct _tuple12 _tmp1DA;_tmp1DA.f1=0,_tmp1DA.f2=ain;_tmp1DA;});case 21: _tmpFA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Exp*e=_tmpFA;
# 1051
_tmpFA=e;goto _LL2E;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple12 _tmp1DB;_tmp1DB.f1=0,_tmp1DB.f2=ain;_tmp1DB;});}case 38: _tmpFA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmpFA;
# 1056
_tmpFA=e;goto _LL30;}case 15: _tmpFA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmpFA;
# 1058
return({struct _tuple12 _tmp1DC;_tmp1DC.f1=0,({void*_tmp263=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp1DC.f2=_tmp263;});_tmp1DC;});}case 20: _tmpFA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Exp*e=_tmpFA;
# 1060
_tmpFA=e;goto _LL34;}case 22: _tmpFA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_LL34: {struct Cyc_Absyn_Exp*e=_tmpFA;
_tmpFA=e;_tmpFC=Cyc_Vcgen_shared_zero_exp();goto _LL36;}case 23: _tmpFA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_Absyn_Exp*e2=_tmpFC;
# 1063
struct _tuple12 _tmp161=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);struct _tuple12 _stmttmp39=_tmp161;struct _tuple12 _tmp162=_stmttmp39;void*_tmp164;void*_tmp163;_tmp163=_tmp162.f1;_tmp164=_tmp162.f2;{void*t1=_tmp163;void*a1=_tmp164;
struct _tuple12 _tmp165=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp3A=_tmp165;struct _tuple12 _tmp166=_stmttmp3A;void*_tmp168;void*_tmp167;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;{void*t2=_tmp167;void*a2=_tmp168;
{void*_tmp169=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp3B=_tmp169;void*_tmp16A=_stmttmp3B;if(*((int*)_tmp16A)== 6)
goto _LLA7;else{
# 1070
Cyc_Vcgen_update_try_assn(env,a2);{
void*term_numelts;void*term_typebound;
# 1074
if(t2 == 0)goto _LLA7;
term_numelts=Cyc_AssnDef_primop(Cyc_Absyn_Numelts,({void*_tmp16B[1];_tmp16B[0]=t1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp16B,sizeof(void*),1));}));{
# 1086 "vcgen.cyc"
void*_tmp16C=Cyc_AssnDef_ult(t2,term_numelts);void*inbound_assn=_tmp16C;
({struct Cyc_Hashtable_Table**_tmp265=env->assn_info;struct Cyc_Absyn_Exp*_tmp264=e1;Cyc_Vcgen_insert_assn_info(_tmp265,_tmp264,({struct _tuple12*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->f1=a2,_tmp16D->f2=inbound_assn;_tmp16D;}));});
a2=Cyc_AssnDef_and(a2,inbound_assn);}}}_LLA7:;}
# 1090
return({struct _tuple12 _tmp1DD;_tmp1DD.f1=0,_tmp1DD.f2=a2;_tmp1DD;});}}}case 31: _tmpFA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_List_List*es=_tmpFA;
# 1092
_tmpFA=es;goto _LL3A;}case 24: _tmpFA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_LL3A: {struct Cyc_List_List*es=_tmpFA;
return({struct _tuple12 _tmp1DE;_tmp1DE.f1=0,({void*_tmp266=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_tmp1DE.f2=_tmp266;});_tmp1DE;});}case 25: _tmpFA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{struct Cyc_List_List*dles=_tmpFA;
# 1095
_tmpFA=dles;goto _LL3E;}case 26: _tmpFA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_LL3E: {struct Cyc_List_List*dles=_tmpFA;
_tmpFA=dles;goto _LL40;}case 29: _tmpFA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF9)->f3;_LL40: {struct Cyc_List_List*dles=_tmpFA;
_tmpFA=dles;goto _LL42;}case 36: _tmpFA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;_LL42: {struct Cyc_List_List*dles=_tmpFA;
_tmpFA=dles;goto _LL44;}case 30: _tmpFA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;_LL44: {struct Cyc_List_List*dles=_tmpFA;
# 1100
for(1;dles != 0;dles=dles->tl){
struct _tuple17*_tmp16E=(struct _tuple17*)dles->hd;struct _tuple17*_stmttmp3C=_tmp16E;struct _tuple17*_tmp16F=_stmttmp3C;void*_tmp170;_tmp170=_tmp16F->f2;{struct Cyc_Absyn_Exp*e=_tmp170;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;}}
# 1104
return({struct _tuple12 _tmp1DF;_tmp1DF.f1=0,_tmp1DF.f2=ain;_tmp1DF;});}case 27: _tmpFA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;_tmpFE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF9)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmpFA;struct Cyc_Absyn_Exp*e1=_tmpFC;struct Cyc_Absyn_Exp*e2=_tmpFE;
# 1108
struct _tuple12 _tmp171=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);struct _tuple12 _stmttmp3D=_tmp171;struct _tuple12 _tmp172=_stmttmp3D;void*_tmp174;void*_tmp173;_tmp173=_tmp172.f1;_tmp174=_tmp172.f2;{void*t1=_tmp173;void*a=_tmp174;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp175=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->tag=1,_tmp17C->f1=vd;_tmp17C;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*v=_tmp175;
# 1111
a=({struct Cyc_Vcgen_Env*_tmp269=env;void*_tmp268=a;void*_tmp267=(void*)v;Cyc_Vcgen_do_assign(_tmp269,_tmp268,_tmp267,Cyc_AssnDef_zero());});
while(1){
void*_tmp176=({void*_tmp26A=a;Cyc_AssnDef_and(_tmp26A,Cyc_AssnDef_ult((void*)v,t1));});void*at=_tmp176;
void*_tmp177=({void*_tmp26B=a;Cyc_AssnDef_and(_tmp26B,Cyc_AssnDef_ulte(t1,(void*)v));});void*af=_tmp177;
struct _tuple12 _tmp178=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp3E=_tmp178;struct _tuple12 _tmp179=_stmttmp3E;void*_tmp17A;_tmp17A=_tmp179.f2;{void*abody=_tmp17A;
abody=({struct Cyc_Vcgen_Env*_tmp26F=env;void*_tmp26E=abody;void*_tmp26D=(void*)v;Cyc_Vcgen_do_assign(_tmp26F,_tmp26E,_tmp26D,Cyc_AssnDef_primop(Cyc_Absyn_Plus,({void*_tmp17B[2];_tmp17B[0]=(void*)v,({void*_tmp26C=Cyc_AssnDef_one();_tmp17B[1]=_tmp26C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17B,sizeof(void*),2));})));});
if(Cyc_Vcgen_simple_implies(& abody,& a)){a=af;break;}
a=Cyc_Vcgen_widen(Cyc_AssnDef_or(abody,a));}}
# 1120
return({struct _tuple12 _tmp1E0;_tmp1E0.f1=0,_tmp1E0.f2=a;_tmp1E0;});}}case 28: _tmpFA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Exp*e=_tmpFA;
# 1122
return({struct _tuple12 _tmp1E1;_tmp1E1.f1=0,({void*_tmp270=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp1E1.f2=_tmp270;});_tmp1E1;});}case 34: _tmpFD=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_MallocInfo mi=_tmpFD;
# 1124
if(mi.rgn != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain)).f2;
return({struct _tuple12 _tmp1E2;_tmp1E2.f1=0,({void*_tmp271=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain)).f2;_tmp1E2.f2=_tmp271;});_tmp1E2;});}case 35: _tmpFA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;_tmpFC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF9)->f2;{struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_Absyn_Exp*e2=_tmpFC;
# 1128
struct _tuple12 _tmp17D=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple12 _stmttmp3F=_tmp17D;struct _tuple12 _tmp17E=_stmttmp3F;void*_tmp180;void*_tmp17F;_tmp17F=_tmp17E.f1;_tmp180=_tmp17E.f2;{void*lt1=_tmp17F;void*a1=_tmp180;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple12 _tmp181=Cyc_Vcgen_vcgen_lexp(env,e2,a1);struct _tuple12 _stmttmp40=_tmp181;struct _tuple12 _tmp182=_stmttmp40;void*_tmp184;void*_tmp183;_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;{void*lt2=_tmp183;void*a2=_tmp184;
Cyc_Vcgen_update_try_assn(env,a2);{
void*_tmp185=Cyc_Vcgen_deref_lterm(env,lt1,a2);void*t1=_tmp185;
void*_tmp186=Cyc_Vcgen_deref_lterm(env,lt2,a2);void*t2=_tmp186;
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple12 _tmp1E3;_tmp1E3.f1=0,_tmp1E3.f2=a2;_tmp1E3;});}}}}}case 37: _tmpFA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Stmt*s=_tmpFA;
# 1138
void**_tmp187=(env->shared)->exp_stmt;void**old_exp_stmt=_tmp187;
({void**_tmp272=({void**_tmp188=_cycalloc(sizeof(*_tmp188));*_tmp188=0;_tmp188;});(env->shared)->exp_stmt=_tmp272;});{
void*_tmp189=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);void*aout=_tmp189;
void*_tmp18A=*((void**)_check_null((env->shared)->exp_stmt));void*t=_tmp18A;
(env->shared)->exp_stmt=old_exp_stmt;
return({struct _tuple12 _tmp1E4;_tmp1E4.f1=t,_tmp1E4.f2=aout;_tmp1E4;});}}case 40:
 return({struct _tuple12 _tmp1E5;_tmp1E5.f1=0,_tmp1E5.f2=ain;_tmp1E5;});case 41: _tmpFA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Exp*e=_tmpFA;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2: _tmpFB=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct _fat_ptr s=_tmpFB;
# 1147
if(({struct _fat_ptr _tmp273=(struct _fat_ptr)s;Cyc_strcmp(_tmp273,({const char*_tmp18B="print_assn";_tag_fat(_tmp18B,sizeof(char),11U);}));})){
struct _fat_ptr _tmp18C=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp18C;
({struct Cyc_String_pa_PrintArg_struct _tmp18F=({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0,_tmp1E7.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp1E7;});struct Cyc_String_pa_PrintArg_struct _tmp190=({struct Cyc_String_pa_PrintArg_struct _tmp1E6;_tmp1E6.tag=0,({struct _fat_ptr _tmp274=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_tmp1E6.f1=_tmp274;});_tmp1E6;});void*_tmp18D[2];_tmp18D[0]=& _tmp18F,_tmp18D[1]=& _tmp190;({struct Cyc___cycFILE*_tmp276=Cyc_stderr;struct _fat_ptr _tmp275=({const char*_tmp18E="%s: current assn is %s\n";_tag_fat(_tmp18E,sizeof(char),24U);});Cyc_fprintf(_tmp276,_tmp275,_tag_fat(_tmp18D,sizeof(void*),2));});});}
# 1151
return({struct _tuple12 _tmp1E8;({void*_tmp277=Cyc_AssnDef_zero();_tmp1E8.f1=_tmp277;}),_tmp1E8.f2=ain;_tmp1E8;});}default: _tmpFA=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpF9)->f1;{struct Cyc_Absyn_Exp*e1=_tmpFA;
# 1153
struct _tuple12 _tmp191=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_AssnDef_true_assn);struct _tuple12 _stmttmp41=_tmp191;struct _tuple12 _tmp192=_stmttmp41;void*_tmp193;_tmp193=_tmp192.f1;{void*at=_tmp193;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp196=({struct Cyc_String_pa_PrintArg_struct _tmp1E9;_tmp1E9.tag=0,({
struct _fat_ptr _tmp278=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_tmp1E9.f1=_tmp278;});_tmp1E9;});void*_tmp194[1];_tmp194[0]=& _tmp196;({unsigned _tmp27A=e->loc;struct _fat_ptr _tmp279=({const char*_tmp195="cannot prove @assert from %s";_tag_fat(_tmp195,sizeof(char),29U);});Cyc_Warn_err(_tmp27A,_tmp279,_tag_fat(_tmp194,sizeof(void*),1));});});
return({struct _tuple12 _tmp1EA;({void*_tmp27C=Cyc_AssnDef_one();_tmp1EA.f1=_tmp27C;}),({void*_tmp27B=Cyc_AssnDef_and(ain,at);_tmp1EA.f2=_tmp27B;});_tmp1EA;});}}};}}
# 1163
void Cyc_Vcgen_vcgen_fundecl(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
void*a=(void*)& Cyc_AssnDef_true_assn;
# 1166
struct _tuple16 _tmp197=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);struct _tuple16 _stmttmp42=_tmp197;struct _tuple16 _tmp198=_stmttmp42;void*_tmp19C;void*_tmp19B;void*_tmp19A;void*_tmp199;_tmp199=_tmp198.f1;_tmp19A=_tmp198.f2;_tmp19B=_tmp198.f3;_tmp19C=_tmp198.f4;{struct Cyc_List_List*args=_tmp199;struct Cyc_Absyn_Exp*reqopt=_tmp19A;struct Cyc_Absyn_Exp*ensopt=_tmp19B;struct Cyc_Absyn_Vardecl*ret_value=_tmp19C;
void**result_assn=ensopt == 0?0:({void**_tmp1A9=_cycalloc(sizeof(*_tmp1A9));*_tmp1A9=(void*)& Cyc_AssnDef_false_assn;_tmp1A9;});
struct Cyc_Vcgen_Env*_tmp19D=Cyc_Vcgen_initial_env(tables,fd,assn_info,result_assn);struct Cyc_Vcgen_Env*env=_tmp19D;
if(reqopt != 0)
# 1171
a=(Cyc_Vcgen_vcgen_test(env,reqopt,a)).f1;
# 1173
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 1175
if(ensopt != 0){
struct Cyc_Absyn_Exp*ens=ensopt;
struct _tuple12 _tmp19E=Cyc_Vcgen_vcgen_test(env,ens,(void*)& Cyc_AssnDef_true_assn);struct _tuple12 _stmttmp43=_tmp19E;struct _tuple12 _tmp19F=_stmttmp43;void*_tmp1A0;_tmp1A0=_tmp19F.f1;{void*at=_tmp1A0;
struct Cyc_Vcgen_SharedEnv*_tmp1A1=env->shared;struct Cyc_Vcgen_SharedEnv*senv=_tmp1A1;
void*_tmp1A2=*((void**)_check_null(senv->res_assn));void*res_assn_opt=_tmp1A2;
void*res_assn=(void*)& Cyc_AssnDef_false_assn;
if(res_assn_opt != 0)res_assn=res_assn_opt;{
struct _fat_ptr str=({const char*_tmp1A8="NULL";_tag_fat(_tmp1A8,sizeof(char),5U);});
if(senv->res_term != 0)str=Cyc_AssnDef_term2string((void*)_check_null(senv->res_term));
if(ret_value != 0 &&(unsigned)senv->res_term)
res_assn=({void*_tmp27E=res_assn;Cyc_AssnDef_and(_tmp27E,({void*_tmp27D=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->tag=1,_tmp1A3->f1=ret_value;_tmp1A3;});Cyc_AssnDef_eq(_tmp27D,senv->res_term);}));});
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp1A6=({struct Cyc_String_pa_PrintArg_struct _tmp1EC;_tmp1EC.tag=0,({
struct _fat_ptr _tmp27F=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp1EC.f1=_tmp27F;});_tmp1EC;});struct Cyc_String_pa_PrintArg_struct _tmp1A7=({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0,({struct _fat_ptr _tmp280=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(res_assn)));_tmp1EB.f1=_tmp280;});_tmp1EB;});void*_tmp1A4[2];_tmp1A4[0]=& _tmp1A6,_tmp1A4[1]=& _tmp1A7;({unsigned _tmp282=ens->loc;struct _fat_ptr _tmp281=({const char*_tmp1A5="cannot prove the @ensures clause %s from %s";_tag_fat(_tmp1A5,sizeof(char),44U);});Cyc_Warn_err(_tmp282,_tmp281,_tag_fat(_tmp1A4,sizeof(void*),2));});});}}}
# 1190
Cyc_AssnDef_reset_hash_cons_table();}}

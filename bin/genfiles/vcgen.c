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
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 63 "tcutil.h"
void*Cyc_Tcutil_pointer_elt_type(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 62 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 65
void*Cyc_AssnDef_zero (void);
void*Cyc_AssnDef_one (void);
# 71
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
void*Cyc_AssnDef_cast(void*,void*);
void*Cyc_AssnDef_fresh_var(void*);
# 80
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 92
int Cyc_AssnDef_assncmp(void*,void*);
# 94
struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 108 "assndef.h"
void Cyc_AssnDef_reset_hash_cons_table (void);
# 115
void*Cyc_AssnDef_kill(void*);
# 117
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*,void*,void*);
# 122
void*Cyc_AssnDef_eq(void*,void*);
# 124
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 130
void*Cyc_AssnDef_reduce(void*);
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
# 133
int Cyc_AssnDef_simple_prove(void*,void*);
# 135
void*Cyc_AssnDef_kill_mem_term(void*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 71 "flags.h"
extern int Cyc_Flags_allpaths;
# 78
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
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
# 120 "vcgen.cyc"
static int Cyc_Vcgen_simple_implies(void**a1,void**a2){
({void*_tmp206=Cyc_AssnDef_reduce(*a1);*a1=_tmp206;});
({void*_tmp207=Cyc_AssnDef_reduce(*a2);*a2=_tmp207;});
return Cyc_AssnDef_simple_prove(*a1,*a2);}
# 127
static int Cyc_Vcgen_implies(void**a1,void**a2){
return Cyc_Vcgen_simple_implies(a1,a2)|| Cyc_PrattProver_constraint_prove(*a1,*a2);}
# 142
static struct Cyc_Set_Set*Cyc_Vcgen_widen_it(struct Cyc_Set_Set*s,void*a){
if(s == 0)return 0;{
void*_tmp0=a;enum Cyc_AssnDef_Primreln _tmp3;void*_tmp2;void*_tmp1;switch(*((int*)_tmp0)){case 2: if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f2 == Cyc_AssnDef_Eq){_tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f3;{void*t1=_tmp1;void*t2=_tmp2;
# 146
return((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert)(s,a);}}else{_tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f1;_tmp3=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f2;_tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f3;{void*t1=_tmp1;enum Cyc_AssnDef_Primreln p=_tmp3;void*t2=_tmp2;
return((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert)(s,a);}}case 3: _tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp0)->f2;{void*a1=_tmp1;void*a2=_tmp2;
return({struct Cyc_Set_Set*_tmp208=Cyc_Vcgen_widen_it(s,a1);Cyc_Vcgen_widen_it(_tmp208,a2);});}case 4: _tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp0)->f2;{void*a1=_tmp1;void*a2=_tmp2;
# 150
struct Cyc_Set_Set*_tmp4=Cyc_Vcgen_widen_it(s,a1);struct Cyc_Set_Set*s1=_tmp4;
struct Cyc_Set_Set*_tmp5=Cyc_Vcgen_widen_it(s,a2);struct Cyc_Set_Set*s2=_tmp5;
if(s1 == 0 || s2 == 0)return 0;
return((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_intersect)(s1,s2);}case 0:
 return s;case 1:
 return 0;default:
 return({struct Cyc_Set_Set*_tmp209=s;Cyc_Vcgen_widen_it(_tmp209,Cyc_AssnDef_reduce(a));});};}}
# 160
static void*Cyc_Vcgen_widen(void*a){
struct Cyc_Set_Set*_tmp6=({struct Cyc_Set_Set*_tmp20A=((struct Cyc_Set_Set*(*)(int(*)(void*,void*)))Cyc_Set_empty)(Cyc_AssnDef_assncmp);Cyc_Vcgen_widen_it(_tmp20A,a);});struct Cyc_Set_Set*sopt=_tmp6;
if(sopt == 0)return(void*)& Cyc_AssnDef_false_assn;
return((void*(*)(void*(*)(void*,void*),struct Cyc_Set_Set*,void*))Cyc_Set_fold)(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);}
# 167
static void*Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*,void*);
# 170
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Vardecl*);
# 172
static void*Cyc_Vcgen_kill_var(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
if(vd->escapes)
a=Cyc_AssnDef_kill(a);else{
# 176
void*_tmp7=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmp7;
a=Cyc_AssnDef_subst(vd,v2,a);}
# 179
return a;}
# 183
static void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_tmp8=tgt;void*_tmp9;if(_tmp8 == 0)
# 188
return Cyc_AssnDef_kill(a);else{if(*((int*)_tmp8)== 2){_tmp9=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp8)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp9;
# 194
if(vd->escapes){
a=Cyc_AssnDef_kill(a);
src=src == 0?0: Cyc_AssnDef_kill_mem_term(src);}else{
# 198
void*_tmpA=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmpA;
a=Cyc_AssnDef_subst(vd,v2,a);
src=src == 0?0: Cyc_AssnDef_subst_term(src,vd,v2);}
# 203
return({void*_tmp20B=a;Cyc_AssnDef_and(_tmp20B,Cyc_AssnDef_eq(tgt,src));});}}else{
({void*_tmpB=0U;({struct _fat_ptr _tmp20C=({const char*_tmpC="vcgen do_assign";_tag_fat(_tmpC,sizeof(char),16U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp20C,_tag_fat(_tmpB,sizeof(void*),0));});});}};}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void**try_assn;void**exp_stmt;void*res_term;void**res_assn;int widen_paths;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;struct Cyc_Hashtable_Table**assn_info;};
# 246 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 248
static void*Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*env,void*a){
return(env->shared)->widen_paths?Cyc_Vcgen_widen(a): a;}
# 252
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,void**res_assn){
# 254
struct Cyc_Hashtable_Table*_tmpD=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup)(tables->succ_tables,fd);struct Cyc_Hashtable_Table*succ_table=_tmpD;
struct Cyc_Hashtable_Table*_tmpE=((struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_table=_tmpE;
struct Cyc_Vcgen_SharedEnv*_tmpF=({struct Cyc_Vcgen_SharedEnv*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->assn_table=assn_table,_tmp12->succ_table=succ_table,_tmp12->try_assn=0,_tmp12->exp_stmt=0,_tmp12->res_term=0,_tmp12->res_assn=res_assn,_tmp12->widen_paths=!Cyc_Flags_allpaths;_tmp12;});struct Cyc_Vcgen_SharedEnv*senv=_tmpF;
# 263
struct Cyc_Dict_Dict _tmp10=((struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty)((int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp);struct Cyc_Dict_Dict d=_tmp10;
return({struct Cyc_Vcgen_Env*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->shared=senv,_tmp11->state_counter=d,_tmp11->assn_info=assn_info;_tmp11;});}
# 267
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_tmp13=_cycalloc(sizeof(*_tmp13));*_tmp13=*env;_tmp13;});}
# 273
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 277
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _tmp20D=((struct Cyc_Dict_Dict(*)(int(*)(int,struct Cyc_Absyn_Vardecl*,int,int),int,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_union_two_c)(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_tmp20D;});}
# 285
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
((int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int*))Cyc_Dict_lookup_bool)(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _tmp20E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int))Cyc_Dict_insert)(env->state_counter,vd,res + 1);env->state_counter=_tmp20E;});
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=3,_tmp14->f1=vd,_tmp14->f2=res,_tmp14->f3=vd->type;_tmp14;});}
# 294
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***_tmp15=((void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);void***popt=_tmp15;
if(popt != 0){
void**p=*popt;
void*_tmp16=Cyc_AssnDef_or(ain,*p);void*new_precondition=_tmp16;
*p=new_precondition;
return new_precondition;}else{
# 302
return ain;}}
# 306
static void**Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
static void*false_pointer=(void*)& Cyc_AssnDef_false_assn;
void***popt=((void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(popt != 0)
return*popt;else{
# 312
return& false_pointer;}}
# 317
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp17=*aprevopt;void**aprev=_tmp17;
# 322
ain=Cyc_Vcgen_widen(Cyc_AssnDef_or(ain,*aprev));
*aprev=ain;}else{
# 325
({struct Cyc_Hashtable_Table*_tmp210=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp20F=s;((void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert)(_tmp210,_tmp20F,({void**_tmp18=_cycalloc(sizeof(*_tmp18));*_tmp18=ain;_tmp18;}));});}
# 327
return ain;}
# 332
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp19=*aprevopt;void**aprev=_tmp19;
ain=({struct Cyc_Vcgen_Env*_tmp211=env;Cyc_Vcgen_may_widen(_tmp211,Cyc_AssnDef_or(ain,*aprev));});
*aprev=ain;}else{
# 339
({struct Cyc_Hashtable_Table*_tmp213=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp212=s;((void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert)(_tmp213,_tmp212,({void**_tmp1A=_cycalloc(sizeof(*_tmp1A));*_tmp1A=ain;_tmp1A;}));});}
# 341
return ain;}
# 346
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
if((env->shared)->try_assn == 0)return;
a=({struct Cyc_Vcgen_Env*_tmp214=env;Cyc_Vcgen_may_widen(_tmp214,Cyc_AssnDef_or(*((void**)_check_null((env->shared)->try_assn)),a));});
*((void**)_check_null((env->shared)->try_assn))=a;}
# 353
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a,void*topt){
struct Cyc_Vcgen_SharedEnv*_tmp1B=env->shared;struct Cyc_Vcgen_SharedEnv*shared=_tmp1B;
if(shared->res_assn == 0)return;
if(shared->res_term != 0 || t != 0){
void*_tmp1C=Cyc_AssnDef_fresh_var(topt);void*v=_tmp1C;
shared->res_term=v;
({void*_tmp218=({
void*_tmp217=({void*_tmp215=a;Cyc_AssnDef_and(_tmp215,Cyc_AssnDef_eq(v,t));});Cyc_AssnDef_or(_tmp217,({void*_tmp216=*((void**)_check_null(shared->res_assn));Cyc_AssnDef_and(_tmp216,Cyc_AssnDef_eq(v,shared->res_term));}));});
# 359
*((void**)_check_null(shared->res_assn))=_tmp218;});}else{
# 362
({void*_tmp219=Cyc_AssnDef_or(a,*((void**)_check_null(shared->res_assn)));*((void**)_check_null(shared->res_assn))=_tmp219;});}}
# 367
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void){
struct Cyc_Hashtable_Table*_tmp1D=((struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_info=_tmp1D;
return({struct Cyc_Hashtable_Table**_tmp1E=_cycalloc(sizeof(*_tmp1E));*_tmp1E=assn_info;_tmp1E;});}struct _tuple13{void*f1;void*f2;};
# 372
struct _tuple13 Cyc_Vcgen_exp2ctxt_assn(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){
struct _tuple13**_tmp1F=((struct _tuple13**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_lookup_opt)(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e);struct _tuple13**result=_tmp1F;
if(result == 0)
# 377
return({struct _tuple13 _tmp1C0;_tmp1C0.f1=(void*)& Cyc_AssnDef_true_assn,_tmp1C0.f2=(void*)& Cyc_AssnDef_false_assn;_tmp1C0;});else{
# 379
return*(*result);}}
# 383
static void Cyc_Vcgen_insert_assn_info(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct _tuple13*ctxt_assn){
# 391
((void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct _tuple13*))Cyc_Hashtable_insert)(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e,ctxt_assn);
return;}
# 395
static struct Cyc_Absyn_Exp*Cyc_Vcgen_shared_zero_exp (void){
static struct Cyc_Absyn_Exp*ans=0;
if(!((unsigned)ans))
ans=Cyc_Absyn_uint_exp(0U,0U);
return(struct Cyc_Absyn_Exp*)_check_null(ans);}
# 407
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Stmt*,void*);
# 411
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 413
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 415
static struct _tuple13 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 418
static struct _tuple13 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 425
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);struct _tuple14{struct Cyc_List_List*f1;void*f2;};
# 427
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 429
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 431
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Decl*,void*);
# 433
static struct _fat_ptr Cyc_Vcgen_stmt2shortstring(struct Cyc_Absyn_Stmt*s){
void*_tmp20=s->r;void*_stmttmp0=_tmp20;void*_tmp21=_stmttmp0;void*_tmp24;void*_tmp23;void*_tmp22;switch(*((int*)_tmp21)){case 4: _tmp22=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;{struct Cyc_Absyn_Exp*e=_tmp22;
# 436
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp1C1;_tmp1C1.tag=0,({struct _fat_ptr _tmp21A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1C1.f1=_tmp21A;});_tmp1C1;});void*_tmp25[1];_tmp25[0]=& _tmp27;({struct _fat_ptr _tmp21B=({const char*_tmp26="if (%s) ...";_tag_fat(_tmp26,sizeof(char),12U);});Cyc_aprintf(_tmp21B,_tag_fat(_tmp25,sizeof(void*),1));});});}case 5: _tmp22=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp21)->f1).f1;{struct Cyc_Absyn_Exp*e=_tmp22;
# 438
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp1C2;_tmp1C2.tag=0,({struct _fat_ptr _tmp21C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1C2.f1=_tmp21C;});_tmp1C2;});void*_tmp28[1];_tmp28[0]=& _tmp2A;({struct _fat_ptr _tmp21D=({const char*_tmp29="while (%s) ...";_tag_fat(_tmp29,sizeof(char),15U);});Cyc_aprintf(_tmp21D,_tag_fat(_tmp28,sizeof(void*),1));});});}case 9: _tmp22=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp23=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f1;_tmp24=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f3).f1;{struct Cyc_Absyn_Exp*e1=_tmp22;struct Cyc_Absyn_Exp*e2=_tmp23;struct Cyc_Absyn_Exp*e3=_tmp24;
# 440
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D=({struct Cyc_String_pa_PrintArg_struct _tmp1C5;_tmp1C5.tag=0,({struct _fat_ptr _tmp21E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp1C5.f1=_tmp21E;});_tmp1C5;});struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp1C4;_tmp1C4.tag=0,({
struct _fat_ptr _tmp21F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e2));_tmp1C4.f1=_tmp21F;});_tmp1C4;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp1C3;_tmp1C3.tag=0,({struct _fat_ptr _tmp220=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e3));_tmp1C3.f1=_tmp220;});_tmp1C3;});void*_tmp2B[3];_tmp2B[0]=& _tmp2D,_tmp2B[1]=& _tmp2E,_tmp2B[2]=& _tmp2F;({struct _fat_ptr _tmp221=({const char*_tmp2C="for (%s;%s;%s) ...";_tag_fat(_tmp2C,sizeof(char),19U);});Cyc_aprintf(_tmp221,_tag_fat(_tmp2B,sizeof(void*),3));});});}case 10: _tmp22=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;{struct Cyc_Absyn_Exp*e=_tmp22;
# 443
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp1C6;_tmp1C6.tag=0,({struct _fat_ptr _tmp222=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1C6.f1=_tmp222;});_tmp1C6;});void*_tmp30[1];_tmp30[0]=& _tmp32;({struct _fat_ptr _tmp223=({const char*_tmp31="switch (%s) ...";_tag_fat(_tmp31,sizeof(char),16U);});Cyc_aprintf(_tmp223,_tag_fat(_tmp30,sizeof(void*),1));});});}case 14: _tmp22=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f1;{struct Cyc_Absyn_Exp*e=_tmp22;
# 445
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp1C7;_tmp1C7.tag=0,({struct _fat_ptr _tmp224=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1C7.f1=_tmp224;});_tmp1C7;});void*_tmp33[1];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp225=({const char*_tmp34="do ... while (%s)";_tag_fat(_tmp34,sizeof(char),18U);});Cyc_aprintf(_tmp225,_tag_fat(_tmp33,sizeof(void*),1));});});}case 15: _tmp22=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;{struct Cyc_Absyn_Stmt*s=_tmp22;
# 447
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp1C8;_tmp1C8.tag=0,({struct _fat_ptr _tmp226=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_stmt2shortstring(s));_tmp1C8.f1=_tmp226;});_tmp1C8;});void*_tmp36[1];_tmp36[0]=& _tmp38;({struct _fat_ptr _tmp227=({const char*_tmp37="try %s catch ...";_tag_fat(_tmp37,sizeof(char),17U);});Cyc_aprintf(_tmp227,_tag_fat(_tmp36,sizeof(void*),1));});});}case 2: _tmp22=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;{struct Cyc_Absyn_Stmt*s=_tmp22;
return Cyc_Vcgen_stmt2shortstring(s);}default:  {
# 450
struct _fat_ptr _tmp39=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_tmp39;
unsigned max=_get_fat_size(s,sizeof(char))< 11U?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _tmp3D=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_tmp3D),1U,_tmp3D);});
{int i=0;for(0;(unsigned)i < max - 1U;++ i){
({char _tmp228=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_tmp228;});}}
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3C=({struct Cyc_String_pa_PrintArg_struct _tmp1C9;_tmp1C9.tag=0,_tmp1C9.f1=(struct _fat_ptr)((struct _fat_ptr)res);_tmp1C9;});void*_tmp3A[1];_tmp3A[0]=& _tmp3C;({struct _fat_ptr _tmp229=({const char*_tmp3B="%s...";_tag_fat(_tmp3B,sizeof(char),6U);});Cyc_aprintf(_tmp229,_tag_fat(_tmp3A,sizeof(void*),1));});});}};}
# 510 "vcgen.cyc"
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 513
LOOP:
 ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);{
# 525 "vcgen.cyc"
void*_tmp3E=s->r;void*_stmttmp1=_tmp3E;void*_tmp3F=_stmttmp1;void*_tmp45;void*_tmp44;void*_tmp43;void*_tmp42;void*_tmp41;void*_tmp40;switch(*((int*)_tmp3F)){case 0:
 return ain;case 1: _tmp40=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;{struct Cyc_Absyn_Exp*e=_tmp40;
# 528
struct _tuple13 _tmp46=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp2=_tmp46;struct _tuple13 _tmp47=_stmttmp2;void*_tmp49;void*_tmp48;_tmp48=_tmp47.f1;_tmp49=_tmp47.f2;{void*t=_tmp48;void*aout=_tmp49;
# 531
if((env->shared)->exp_stmt != 0)
*((void**)_check_null((env->shared)->exp_stmt))=t;
return aout;}}case 2: _tmp40=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp40;struct Cyc_Absyn_Stmt*s2=_tmp41;
# 535
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain);
s=s2;
goto LOOP;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1 == 0){
# 539
Cyc_Vcgen_update_return(env,0,ain,0);
return(void*)& Cyc_AssnDef_false_assn;}else{_tmp40=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;{struct Cyc_Absyn_Exp*e=_tmp40;
# 542
struct _tuple13 _tmp4A=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(e),ain);struct _tuple13 _stmttmp3=_tmp4A;struct _tuple13 _tmp4B=_stmttmp3;void*_tmp4D;void*_tmp4C;_tmp4C=_tmp4B.f1;_tmp4D=_tmp4B.f2;{void*t=_tmp4C;void*aout=_tmp4D;
Cyc_Vcgen_update_return(env,t,aout,e->topt);
return(void*)& Cyc_AssnDef_false_assn;}}}case 4: _tmp40=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;_tmp42=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3F)->f3;{struct Cyc_Absyn_Exp*e1=_tmp40;struct Cyc_Absyn_Stmt*s1=_tmp41;struct Cyc_Absyn_Stmt*s2=_tmp42;
# 546
struct _tuple13 _tmp4E=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp4=_tmp4E;struct _tuple13 _tmp4F=_stmttmp4;void*_tmp51;void*_tmp50;_tmp50=_tmp4F.f1;_tmp51=_tmp4F.f2;{void*at=_tmp50;void*af=_tmp51;
struct Cyc_Vcgen_Env*_tmp52=Cyc_Vcgen_copy_env(env);struct Cyc_Vcgen_Env*env2=_tmp52;
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return({struct Cyc_Vcgen_Env*_tmp22A=env;Cyc_Vcgen_may_widen(_tmp22A,Cyc_AssnDef_or(at,af));});}}case 5: _tmp40=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1).f1;_tmp41=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1).f2;_tmp42=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Exp*e=_tmp40;struct Cyc_Absyn_Stmt*dummy=_tmp41;struct Cyc_Absyn_Stmt*s=_tmp42;
# 558
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);{
struct _tuple13 _tmp53=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple13 _stmttmp5=_tmp53;struct _tuple13 _tmp54=_stmttmp5;void*_tmp56;void*_tmp55;_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;{void*at=_tmp55;void*af=_tmp56;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7:
# 564
 goto _LL12;{
# 567
struct Cyc_Absyn_Stmt*_tmp57=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp57;
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_AssnDef_false_assn;}case 6: _LL12: {
# 574
struct Cyc_Absyn_Stmt*_tmp58=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp58;
if(dest == 0)
Cyc_Vcgen_update_return(env,0,ain,0);else{
# 578
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_AssnDef_false_assn;}case 9: _tmp40=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f1;_tmp42=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f2;_tmp43=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f3).f1;_tmp44=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f3).f2;_tmp45=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f4;{struct Cyc_Absyn_Exp*e1=_tmp40;struct Cyc_Absyn_Exp*e2=_tmp41;struct Cyc_Absyn_Stmt*dummy2=_tmp42;struct Cyc_Absyn_Exp*e3=_tmp43;struct Cyc_Absyn_Stmt*dummy3=_tmp44;struct Cyc_Absyn_Stmt*s=_tmp45;
# 581
struct _tuple13 _tmp59=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp6=_tmp59;struct _tuple13 _tmp5A=_stmttmp6;void*_tmp5B;_tmp5B=_tmp5A.f2;{void*a=_tmp5B;
ain=a;
# 584
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);{
struct _tuple13 _tmp5C=Cyc_Vcgen_vcgen_test(env,e2,ain);struct _tuple13 _stmttmp7=_tmp5C;struct _tuple13 _tmp5D=_stmttmp7;void*_tmp5F;void*_tmp5E;_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;{void*at=_tmp5E;void*af=_tmp5F;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 589
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain)).f2;
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2)))return af;}}}}}case 14: _tmp40=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f1;_tmp42=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f2;{struct Cyc_Absyn_Stmt*s=_tmp40;struct Cyc_Absyn_Exp*e=_tmp41;struct Cyc_Absyn_Stmt*dummy=_tmp42;
# 595
while(1){
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple13 _tmp60=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple13 _stmttmp8=_tmp60;struct _tuple13 _tmp61=_stmttmp8;void*_tmp63;void*_tmp62;_tmp62=_tmp61.f1;_tmp63=_tmp61.f2;{void*at=_tmp62;void*af=_tmp63;
ain=({struct Cyc_Vcgen_Env*_tmp22C=env;Cyc_Vcgen_may_widen(_tmp22C,({void*_tmp22B=*Cyc_Vcgen_get_assn(env,dummy);Cyc_AssnDef_or(_tmp22B,ain);}));});
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10: _tmp40=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Exp*e=_tmp40;struct Cyc_List_List*scs=_tmp41;
# 608
struct _tuple13 _tmp64=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp9=_tmp64;struct _tuple13 _tmp65=_stmttmp9;void*_tmp67;void*_tmp66;_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{void*t=_tmp66;void*a=_tmp67;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2 == 0)
({void*_tmp68=0U;({struct _fat_ptr _tmp22D=({const char*_tmp69="vcgen fallthru";_tag_fat(_tmp69,sizeof(char),15U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp22D,_tag_fat(_tmp68,sizeof(void*),0));});});else{_tmp40=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_List_List*es=_tmp40;struct Cyc_Absyn_Switch_clause*dest=_tmp41;
# 613
struct _tuple14 _tmp6A=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmpA=_tmp6A;struct _tuple14 _tmp6B=_stmttmpA;void*_tmp6D;void*_tmp6C;_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;{struct Cyc_List_List*ts=_tmp6C;void*a=_tmp6D;
if(ts != 0){
# 616
struct Cyc_List_List*_tmp6E=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(dest->pat_vars))->v)).f1);struct Cyc_List_List*x=_tmp6E;
# 618
for(1;x != 0;(x=x->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*_tmp6F=(struct Cyc_Absyn_Vardecl*)x->hd;struct Cyc_Absyn_Vardecl*vd=_tmp6F;
if(vd != 0)
a=({struct Cyc_Vcgen_Env*_tmp230=env;void*_tmp22F=a;void*_tmp22E=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=2,_tmp70->f1=vd;_tmp70;});Cyc_Vcgen_do_assign(_tmp230,_tmp22F,_tmp22E,(void*)((struct Cyc_List_List*)_check_null(ts))->hd);});}}
# 624
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_AssnDef_false_assn;}}}case 12: _tmp40=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Decl*d=_tmp40;struct Cyc_Absyn_Stmt*s1=_tmp41;
# 627
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13: _tmp40=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp40;
# 635
ain=(void*)& Cyc_AssnDef_true_assn;
s=s1;
goto LOOP;}case 8:
 return(void*)& Cyc_AssnDef_false_assn;default: _tmp40=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Stmt*s=_tmp40;struct Cyc_List_List*scs=_tmp41;
# 641
void**_tmp71=(env->shared)->try_assn;void**old_try_assn=_tmp71;
# 643
({void**_tmp231=({void**_tmp72=_cycalloc(sizeof(*_tmp72));*_tmp72=(void*)& Cyc_AssnDef_false_assn;_tmp72;});(env->shared)->try_assn=_tmp231;});
# 646
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 648
void*_tmp73=*((void**)_check_null((env->shared)->try_assn));void*new_try_assn=_tmp73;
(env->shared)->try_assn=old_try_assn;
# 652
return({void*_tmp232=ain;Cyc_AssnDef_or(_tmp232,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}};}}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 656
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 660
void*res=(void*)& Cyc_AssnDef_false_assn;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp74=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmp74;
void*_tmp75=ain;void*a=_tmp75;
{struct Cyc_List_List*_tmp76=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sc->pat_vars))->v;struct Cyc_List_List*pvs=_tmp76;for(0;pvs != 0;pvs=pvs->tl){
struct _tuple15*_tmp77=(struct _tuple15*)pvs->hd;struct _tuple15*_stmttmpB=_tmp77;struct _tuple15*_tmp78=_stmttmpB;void*_tmp7A;void*_tmp79;_tmp79=_tmp78->f1;_tmp7A=_tmp78->f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmp79;struct Cyc_Absyn_Exp*eopt=_tmp7A;
if(vdopt != 0)
# 671
a=Cyc_Vcgen_kill_var(env,*vdopt,a);}}}
# 675
if(sc->where_clause != 0)
a=(Cyc_Vcgen_vcgen_test(env,(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause),a)).f1;
res=({void*_tmp233=res;Cyc_AssnDef_or(_tmp233,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 679
return res;}
# 682
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 685
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp7B=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=2,_tmp80->f1=vd;_tmp80;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv=_tmp7B;
a=Cyc_Vcgen_kill_var(env,vd,a);
if(vd->initializer != 0){
struct _tuple13 _tmp7C=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),a);struct _tuple13 _stmttmpC=_tmp7C;struct _tuple13 _tmp7D=_stmttmpC;void*_tmp7F;void*_tmp7E;_tmp7E=_tmp7D.f1;_tmp7F=_tmp7D.f2;{void*t=_tmp7E;void*a1=_tmp7F;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 691
return a;}
# 694
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_tmp81=d->r;void*_stmttmpD=_tmp81;void*_tmp82=_stmttmpD;void*_tmp85;void*_tmp84;void*_tmp83;switch(*((int*)_tmp82)){case 0: _tmp83=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp82)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp83;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2: _tmp83=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp82)->f1;_tmp84=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp82)->f2;_tmp85=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp82)->f3;{struct Cyc_Absyn_Pat*p=_tmp83;struct Cyc_Core_Opt*vds=_tmp84;struct Cyc_Absyn_Exp*e=_tmp85;
# 698
struct _tuple13 _tmp86=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple13 _stmttmpE=_tmp86;struct _tuple13 _tmp87=_stmttmpE;void*_tmp88;_tmp88=_tmp87.f2;{void*a=_tmp88;
{struct Cyc_List_List*_tmp89=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds))->v;struct Cyc_List_List*vds=_tmp89;for(0;vds != 0;vds=vds->tl){
struct _tuple15 _tmp8A=*((struct _tuple15*)vds->hd);struct _tuple15 _stmttmpF=_tmp8A;struct _tuple15 _tmp8B=_stmttmpF;void*_tmp8D;void*_tmp8C;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmp8C;struct Cyc_Absyn_Exp*eopt=_tmp8D;
if(vdopt != 0)
# 703
a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);}}}
# 714 "vcgen.cyc"
return a;}}case 3: _tmp83=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp82)->f1;{struct Cyc_List_List*vds=_tmp83;
# 716
for(1;vds != 0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4: _tmp83=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp82)->f2;_tmp84=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp82)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp83;struct Cyc_Absyn_Exp*eopt=_tmp84;
# 720
if(eopt != 0){
struct _tuple13 _tmp8E=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple13 _stmttmp10=_tmp8E;struct _tuple13 _tmp8F=_stmttmp10;void*_tmp91;void*_tmp90;_tmp90=_tmp8F.f1;_tmp91=_tmp8F.f2;{void*t=_tmp90;void*a1=_tmp91;
a=a1;}}
# 725
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default:
 return a;};}
# 733
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp92=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*t1=_tmp92;
void*_tmp93=Cyc_Absyn_compress((void*)_check_null(e2->topt));void*t2=_tmp93;
struct _tuple13 _tmp94=({struct _tuple13 _tmp1CA;_tmp1CA.f1=t1,_tmp1CA.f2=t2;_tmp1CA;});struct _tuple13 _stmttmp11=_tmp94;struct _tuple13 _tmp95=_stmttmp11;if(*((int*)_tmp95.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95.f1)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95.f1)->f1)->f1 == Cyc_Absyn_Unsigned)
return 1;else{if(*((int*)_tmp95.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_tmp95.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_tmp95.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3:
 return 1;}else{if(*((int*)_tmp95.f1)== 3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_tmp95.f1)== 3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_tmp95.f1)== 3){if(*((int*)_tmp95.f2)== 3)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}}};}
# 749
static struct _tuple13 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp96=e->r;void*_stmttmp12=_tmp96;void*_tmp97=_stmttmp12;enum Cyc_Absyn_Primop _tmp98;void*_tmp9B;void*_tmp9A;void*_tmp99;switch(*((int*)_tmp97)){case 6: _tmp99=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp97)->f1;_tmp9A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp97)->f2;_tmp9B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp97)->f3;{struct Cyc_Absyn_Exp*e1=_tmp99;struct Cyc_Absyn_Exp*e2=_tmp9A;struct Cyc_Absyn_Exp*e3=_tmp9B;
# 752
struct _tuple13 _tmp9C=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp13=_tmp9C;struct _tuple13 _tmp9D=_stmttmp13;void*_tmp9F;void*_tmp9E;_tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;{void*a1t=_tmp9E;void*a1f=_tmp9F;
struct _tuple13 _tmpA0=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple13 _stmttmp14=_tmpA0;struct _tuple13 _tmpA1=_stmttmp14;void*_tmpA3;void*_tmpA2;_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;{void*a2t=_tmpA2;void*a2f=_tmpA3;
struct _tuple13 _tmpA4=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct _tuple13 _stmttmp15=_tmpA4;struct _tuple13 _tmpA5=_stmttmp15;void*_tmpA7;void*_tmpA6;_tmpA6=_tmpA5.f1;_tmpA7=_tmpA5.f2;{void*a3t=_tmpA6;void*a3f=_tmpA7;
return({struct _tuple13 _tmp1CB;({void*_tmp235=Cyc_AssnDef_or(a2t,a3t);_tmp1CB.f1=_tmp235;}),({void*_tmp234=Cyc_AssnDef_or(a2f,a3f);_tmp1CB.f2=_tmp234;});_tmp1CB;});}}}}case 7: _tmp99=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp97)->f1;_tmp9A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp97)->f2;{struct Cyc_Absyn_Exp*e1=_tmp99;struct Cyc_Absyn_Exp*e2=_tmp9A;
# 757
struct _tuple13 _tmpA8=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp16=_tmpA8;struct _tuple13 _tmpA9=_stmttmp16;void*_tmpAB;void*_tmpAA;_tmpAA=_tmpA9.f1;_tmpAB=_tmpA9.f2;{void*a1t=_tmpAA;void*a1f=_tmpAB;
struct _tuple13 _tmpAC=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple13 _stmttmp17=_tmpAC;struct _tuple13 _tmpAD=_stmttmp17;void*_tmpAF;void*_tmpAE;_tmpAE=_tmpAD.f1;_tmpAF=_tmpAD.f2;{void*a2t=_tmpAE;void*a2f=_tmpAF;
return({struct _tuple13 _tmp1CC;_tmp1CC.f1=a2t,({void*_tmp236=Cyc_AssnDef_or(a1f,a2f);_tmp1CC.f2=_tmp236;});_tmp1CC;});}}}case 8: _tmp99=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp97)->f1;_tmp9A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp97)->f2;{struct Cyc_Absyn_Exp*e1=_tmp99;struct Cyc_Absyn_Exp*e2=_tmp9A;
# 761
struct _tuple13 _tmpB0=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp18=_tmpB0;struct _tuple13 _tmpB1=_stmttmp18;void*_tmpB3;void*_tmpB2;_tmpB2=_tmpB1.f1;_tmpB3=_tmpB1.f2;{void*a1t=_tmpB2;void*a1f=_tmpB3;
struct _tuple13 _tmpB4=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct _tuple13 _stmttmp19=_tmpB4;struct _tuple13 _tmpB5=_stmttmp19;void*_tmpB7;void*_tmpB6;_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;{void*a2t=_tmpB6;void*a2f=_tmpB7;
return({struct _tuple13 _tmp1CD;({void*_tmp237=Cyc_AssnDef_or(a1t,a2t);_tmp1CD.f1=_tmp237;}),_tmp1CD.f2=a2f;_tmp1CD;});}}}case 9: _tmp99=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp97)->f1;_tmp9A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp97)->f2;{struct Cyc_Absyn_Exp*e1=_tmp99;struct Cyc_Absyn_Exp*e2=_tmp9A;
# 765
struct _tuple13 _tmpB8=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp1A=_tmpB8;struct _tuple13 _tmpB9=_stmttmp1A;void*_tmpBA;_tmpBA=_tmpB9.f2;{void*ain=_tmpBA;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f2 != 0){_tmp99=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f2)->hd;{struct Cyc_Absyn_Exp*e1=_tmp99;
# 768
struct _tuple13 _tmpBB=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp1B=_tmpBB;struct _tuple13 _tmpBC=_stmttmp1B;void*_tmpBE;void*_tmpBD;_tmpBD=_tmpBC.f1;_tmpBE=_tmpBC.f2;{void*at=_tmpBD;void*af=_tmpBE;
return({struct _tuple13 _tmp1CE;_tmp1CE.f1=af,_tmp1CE.f2=at;_tmp1CE;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f2)->tl)->tl == 0){_tmp98=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f1;_tmp99=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f2)->hd;_tmp9A=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmp98;struct Cyc_Absyn_Exp*e1=_tmp99;struct Cyc_Absyn_Exp*e2=_tmp9A;
# 771
struct _tuple13 _tmpBF=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp1C=_tmpBF;struct _tuple13 _tmpC0=_stmttmp1C;void*_tmpC2;void*_tmpC1;_tmpC1=_tmpC0.f1;_tmpC2=_tmpC0.f2;{void*t1=_tmpC1;void*a1=_tmpC2;
struct _tuple13 _tmpC3=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp1D=_tmpC3;struct _tuple13 _tmpC4=_stmttmp1D;void*_tmpC6;void*_tmpC5;_tmpC5=_tmpC4.f1;_tmpC6=_tmpC4.f2;{void*t2=_tmpC5;void*a2=_tmpC6;
void*_tmpC7=a2;void*at=_tmpC7;
void*_tmpC8=a2;void*af=_tmpC8;
{enum Cyc_Absyn_Primop _tmpC9=p;switch((int)_tmpC9){case Cyc_Absyn_Eq:
# 779
 at=({void*_tmp238=at;Cyc_AssnDef_and(_tmp238,Cyc_AssnDef_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Neq:
 af=({void*_tmp239=af;Cyc_AssnDef_and(_tmp239,Cyc_AssnDef_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Lt:
# 782
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp23A=at;Cyc_AssnDef_and(_tmp23A,Cyc_AssnDef_ult(t1,t2));});af=({void*_tmp23B=af;Cyc_AssnDef_and(_tmp23B,Cyc_AssnDef_ulte(t2,t1));});}else{
# 785
at=({void*_tmp23C=at;Cyc_AssnDef_and(_tmp23C,Cyc_AssnDef_slt(t1,t2));});af=({void*_tmp23D=af;Cyc_AssnDef_and(_tmp23D,Cyc_AssnDef_slte(t2,t1));});}
# 787
goto _LL30;case Cyc_Absyn_Lte:
# 789
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp23E=at;Cyc_AssnDef_and(_tmp23E,Cyc_AssnDef_ulte(t1,t2));});af=({void*_tmp23F=af;Cyc_AssnDef_and(_tmp23F,Cyc_AssnDef_ult(t2,t1));});}else{
# 792
at=({void*_tmp240=at;Cyc_AssnDef_and(_tmp240,Cyc_AssnDef_slte(t1,t2));});af=({void*_tmp241=af;Cyc_AssnDef_and(_tmp241,Cyc_AssnDef_slt(t2,t1));});}
# 794
goto _LL30;case Cyc_Absyn_Gt:
# 796
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp242=at;Cyc_AssnDef_and(_tmp242,Cyc_AssnDef_ult(t2,t1));});af=({void*_tmp243=af;Cyc_AssnDef_and(_tmp243,Cyc_AssnDef_ulte(t1,t2));});}else{
# 799
at=({void*_tmp244=at;Cyc_AssnDef_and(_tmp244,Cyc_AssnDef_slt(t2,t1));});af=({void*_tmp245=af;Cyc_AssnDef_and(_tmp245,Cyc_AssnDef_slte(t1,t2));});}
# 801
goto _LL30;case Cyc_Absyn_Gte:
# 803
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp246=at;Cyc_AssnDef_and(_tmp246,Cyc_AssnDef_ulte(t2,t1));});af=({void*_tmp247=af;Cyc_AssnDef_and(_tmp247,Cyc_AssnDef_ult(t1,t2));});}else{
# 806
at=({void*_tmp248=at;Cyc_AssnDef_and(_tmp248,Cyc_AssnDef_slte(t2,t1));});af=({void*_tmp249=af;Cyc_AssnDef_and(_tmp249,Cyc_AssnDef_slt(t1,t2));});}
# 808
goto _LL30;default:
 goto _LL30;}_LL30:;}
# 811
return({struct _tuple13 _tmp1CF;_tmp1CF.f1=at,_tmp1CF.f2=af;_tmp1CF;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: {
# 813
struct _tuple13 _tmpCA=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp1E=_tmpCA;struct _tuple13 _tmpCB=_stmttmp1E;void*_tmpCD;void*_tmpCC;_tmpCC=_tmpCB.f1;_tmpCD=_tmpCB.f2;{void*t=_tmpCC;void*aout=_tmpCD;
return({struct _tuple13 _tmp1D0;({void*_tmp24F=({void*_tmp24E=aout;Cyc_AssnDef_and(_tmp24E,({void*_tmp24D=Cyc_AssnDef_one();Cyc_AssnDef_ulte(_tmp24D,t);}));});_tmp1D0.f1=_tmp24F;}),({void*_tmp24C=({void*_tmp24B=aout;Cyc_AssnDef_and(_tmp24B,({void*_tmp24A=t;Cyc_AssnDef_eq(_tmp24A,Cyc_AssnDef_zero());}));});_tmp1D0.f2=_tmp24C;});_tmp1D0;});}}};}
# 818
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(1;es != 0;es=es->tl){
a=(Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a)).f2;}
return a;}struct _tuple16{struct Cyc_List_List**f1;void*f2;};
# 824
static struct _tuple16 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(1;es != 0;es=es->tl){
struct _tuple13 _tmpCE=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);struct _tuple13 _stmttmp1F=_tmpCE;struct _tuple13 _tmpCF=_stmttmp1F;void*_tmpD1;void*_tmpD0;_tmpD0=_tmpCF.f1;_tmpD1=_tmpCF.f2;{void*t=_tmpD0;void*aout=_tmpD1;
a=aout;
if(valid && t != 0)
res=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=t,_tmpD2->tl=res;_tmpD2;});else{
# 833
valid=0;}}}
# 836
return({struct _tuple16 _tmp1D1;valid?({struct Cyc_List_List**_tmp251=({struct Cyc_List_List**_tmpD3=_cycalloc(sizeof(*_tmpD3));({struct Cyc_List_List*_tmp250=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res);*_tmpD3=_tmp250;});_tmpD3;});_tmp1D1.f1=_tmp251;}):(_tmp1D1.f1=0),_tmp1D1.f2=a;_tmp1D1;});}
# 839
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple13 _tmpD4=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp20=_tmpD4;struct _tuple13 _tmpD5=_stmttmp20;void*_tmpD7;void*_tmpD6;_tmpD6=_tmpD5.f1;_tmpD7=_tmpD5.f2;{void*topt=_tmpD6;void*a=_tmpD7;
return({struct _tuple13 _tmp1D2;topt == 0?({void*_tmp252=Cyc_AssnDef_fresh_var(e->topt);_tmp1D2.f1=_tmp252;}):(_tmp1D2.f1=topt),_tmp1D2.f2=a;_tmp1D2;});}}
# 844
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(1;es != 0;es=es->tl){
struct _tuple13 _tmpD8=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);struct _tuple13 _stmttmp21=_tmpD8;struct _tuple13 _tmpD9=_stmttmp21;void*_tmpDB;void*_tmpDA;_tmpDA=_tmpD9.f1;_tmpDB=_tmpD9.f2;{void*t=_tmpDA;void*a=_tmpDB;
ain=a;
res=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=t,_tmpDC->tl=res;_tmpDC;});}}
# 851
return({struct _tuple14 _tmp1D3;({struct Cyc_List_List*_tmp253=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res);_tmp1D3.f1=_tmp253;}),_tmp1D3.f2=ain;_tmp1D3;});}
# 854
static struct Cyc_Absyn_Vardecl**Cyc_Vcgen_bind2vardecl(void*b){
void*_tmpDD=b;void*_tmpDE;switch(*((int*)_tmpDD)){case 1: _tmpDE=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpDD)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDE;
return({struct Cyc_Absyn_Vardecl**_tmpDF=_cycalloc(sizeof(*_tmpDF));*_tmpDF=vd;_tmpDF;});}case 2: _tmpDE=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpDD)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpDE;
# 858
if(fd->fn_vardecl == 0)return 0;else{
return({struct Cyc_Absyn_Vardecl**_tmpE0=_cycalloc(sizeof(*_tmpE0));*_tmpE0=(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);_tmpE0;});}}case 0:
({void*_tmpE1=0U;({struct _fat_ptr _tmp254=({const char*_tmpE2="vcgen bind2vardecl";_tag_fat(_tmpE2,sizeof(char),19U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp254,_tag_fat(_tmpE1,sizeof(void*),0));});});case 3: _tmpDE=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpDD)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDE;
return({struct Cyc_Absyn_Vardecl**_tmpE3=_cycalloc(sizeof(*_tmpE3));*_tmpE3=vd;_tmpE3;});}case 4: _tmpDE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpDD)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDE;
return({struct Cyc_Absyn_Vardecl**_tmpE4=_cycalloc(sizeof(*_tmpE4));*_tmpE4=vd;_tmpE4;});}default: _tmpDE=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpDD)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDE;
return({struct Cyc_Absyn_Vardecl**_tmpE5=_cycalloc(sizeof(*_tmpE5));*_tmpE5=vd;_tmpE5;});}};}
# 867
static struct _tuple13 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmpE6=e->r;void*_stmttmp22=_tmpE6;void*_tmpE7=_stmttmp22;void*_tmpE8;if(*((int*)_tmpE7)== 1){_tmpE8=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE7)->f1;{void*b=_tmpE8;
# 870
struct Cyc_Absyn_Vardecl**_tmpE9=Cyc_Vcgen_bind2vardecl(b);struct Cyc_Absyn_Vardecl**newb=_tmpE9;
if(newb == 0)return({struct _tuple13 _tmp1D4;_tmp1D4.f1=0,_tmp1D4.f2=ain;_tmp1D4;});else{
return({struct _tuple13 _tmp1D5;({void*_tmp255=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->tag=2,_tmpEA->f1=*((struct Cyc_Absyn_Vardecl**)_check_null(Cyc_Vcgen_bind2vardecl(b)));_tmpEA;});_tmp1D5.f1=_tmp255;}),_tmp1D5.f2=ain;_tmp1D5;});}}}else{
return({struct _tuple13 _tmp1D6;_tmp1D6.f1=0,({void*_tmp256=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp1D6.f2=_tmp256;});_tmp1D6;});};}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Vardecl*f4;};
# 879
static struct _tuple17 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple17 _tmp1D7;_tmp1D7.f1=fi->args,_tmp1D7.f2=fi->requires_clause,_tmp1D7.f3=fi->ensures_clause,_tmp1D7.f4=fi->return_value;_tmp1D7;});}
# 883
static struct _tuple17 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt != 0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_tmpEB=t;void*_tmpEC;if(*((int*)_tmpEB)== 5){_tmpEC=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1;{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_tmpEC;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{
goto _LL0;}_LL0:;}
# 891
return({struct _tuple17 _tmp1D8;_tmp1D8.f1=0,_tmp1D8.f2=0,_tmp1D8.f3=0,_tmp1D8.f4=0;_tmp1D8;});}
# 894
static void Cyc_Vcgen_check_requires(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*args,void*a,struct Cyc_Absyn_Exp*reqopt){
# 897
if(reqopt != 0){
struct Cyc_Absyn_Exp*req=reqopt;
# 901
for(1;args != 0 && inputs != 0;(args=args->tl,inputs=inputs->tl)){
struct _tuple9*_tmpED=(struct _tuple9*)args->hd;struct _tuple9*_stmttmp23=_tmpED;struct _tuple9*_tmpEE=_stmttmp23;void*_tmpF1;struct Cyc_Absyn_Tqual _tmpF0;void*_tmpEF;_tmpEF=_tmpEE->f1;_tmpF0=_tmpEE->f2;_tmpF1=_tmpEE->f3;{struct _fat_ptr*vopt=_tmpEF;struct Cyc_Absyn_Tqual tq=_tmpF0;void*t=_tmpF1;
if(vopt != 0){
# 905
struct _tuple1*qv=({struct _tuple1*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->f1=Cyc_Absyn_Loc_n,_tmpF5->f2=vopt;_tmpF5;});
struct Cyc_Absyn_Vardecl*_tmpF2=({struct Cyc_Absyn_Vardecl*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->sc=2U,_tmpF4->name=qv,_tmpF4->varloc=0U,_tmpF4->tq=tq,_tmpF4->type=t,_tmpF4->initializer=0,_tmpF4->rgn=0,_tmpF4->attributes=0,_tmpF4->escapes=0,_tmpF4->is_proto=0;_tmpF4;});struct Cyc_Absyn_Vardecl*vd=_tmpF2;
# 911
a=({void*_tmp258=a;Cyc_AssnDef_and(_tmp258,({void*_tmp257=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=2,_tmpF3->f1=vd;_tmpF3;});Cyc_AssnDef_eq(_tmp257,(void*)inputs->hd);}));});}}}{
# 915
struct _tuple13 _tmpF6=Cyc_Vcgen_vcgen_test(env,reqopt,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp24=_tmpF6;struct _tuple13 _tmpF7=_stmttmp24;void*_tmpF8;_tmpF8=_tmpF7.f1;{void*at=_tmpF8;
if(!Cyc_Vcgen_implies(& a,& at))
({struct Cyc_String_pa_PrintArg_struct _tmpFB=({struct Cyc_String_pa_PrintArg_struct _tmp1DA;_tmp1DA.tag=0,({
struct _fat_ptr _tmp259=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp1DA.f1=_tmp259;});_tmp1DA;});struct Cyc_String_pa_PrintArg_struct _tmpFC=({struct Cyc_String_pa_PrintArg_struct _tmp1D9;_tmp1D9.tag=0,({struct _fat_ptr _tmp25A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(a)));_tmp1D9.f1=_tmp25A;});_tmp1D9;});void*_tmpF9[2];_tmpF9[0]=& _tmpFB,_tmpF9[1]=& _tmpFC;({unsigned _tmp25C=req->loc;struct _fat_ptr _tmp25B=({const char*_tmpFA="cannot prove @requires %s from %s";_tag_fat(_tmpFA,sizeof(char),34U);});Cyc_Warn_err(_tmp25C,_tmp25B,_tag_fat(_tmpF9,sizeof(void*),2));});});}}}}
# 922
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}
# 926
static int Cyc_Vcgen_is_unsigned_int_exp(struct Cyc_Absyn_Exp*e){
void*t=e->topt;
if(t == 0)return 0;{
void*_tmpFD=t;enum Cyc_Absyn_Size_of _tmpFE;if(_tmpFD != 0){if(*((int*)_tmpFD)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFD)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFD)->f1)->f1 == Cyc_Absyn_Unsigned){_tmpFE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFD)->f1)->f2;{enum Cyc_Absyn_Size_of sz=_tmpFE;
# 932
if((int)sz == 3 ||(int)sz == 2)
return 1;else{
return 0;}}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 939
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e)){
# 942
{void*_tmpFF=e->r;void*_stmttmp25=_tmpFF;void*_tmp100=_stmttmp25;struct _fat_ptr _tmp101;if(*((int*)_tmp100)== 2){_tmp101=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp100)->f1;{struct _fat_ptr s=_tmp101;
# 944
if(({struct _fat_ptr _tmp25D=(struct _fat_ptr)s;Cyc_strcmp(_tmp25D,({const char*_tmp102="print_assn";_tag_fat(_tmp102,sizeof(char),11U);}));})== 0){
struct _fat_ptr _tmp103=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp103;
({struct Cyc_String_pa_PrintArg_struct _tmp106=({struct Cyc_String_pa_PrintArg_struct _tmp1DC;_tmp1DC.tag=0,_tmp1DC.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp1DC;});struct Cyc_String_pa_PrintArg_struct _tmp107=({struct Cyc_String_pa_PrintArg_struct _tmp1DB;_tmp1DB.tag=0,({struct _fat_ptr _tmp25E=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_tmp1DB.f1=_tmp25E;});_tmp1DB;});void*_tmp104[2];_tmp104[0]=& _tmp106,_tmp104[1]=& _tmp107;({struct Cyc___cycFILE*_tmp260=Cyc_stderr;struct _fat_ptr _tmp25F=({const char*_tmp105="%s: current assn is %s\n";_tag_fat(_tmp105,sizeof(char),24U);});Cyc_fprintf(_tmp260,_tmp25F,_tag_fat(_tmp104,sizeof(void*),2));});});}
# 948
if(({struct _fat_ptr _tmp261=(struct _fat_ptr)s;Cyc_strcmp(_tmp261,({const char*_tmp108="all_paths";_tag_fat(_tmp108,sizeof(char),10U);}));})== 0)
(env->shared)->widen_paths=0;
# 951
if(({struct _fat_ptr _tmp262=(struct _fat_ptr)s;Cyc_strcmp(_tmp262,({const char*_tmp109="one_path";_tag_fat(_tmp109,sizeof(char),9U);}));})== 0)
(env->shared)->widen_paths=1;
# 954
return({struct _tuple13 _tmp1DD;({void*_tmp263=Cyc_AssnDef_zero();_tmp1DD.f1=_tmp263;}),_tmp1DD.f2=ain;_tmp1DD;});}}else{
goto _LL0;}_LL0:;}{
# 957
struct _tuple12 _tmp10A=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp26=_tmp10A;struct _tuple12 _tmp10B=_stmttmp26;int _tmp10D;unsigned _tmp10C;_tmp10C=_tmp10B.f1;_tmp10D=_tmp10B.f2;{unsigned cn=_tmp10C;int known=_tmp10D;
if(known)return({struct _tuple13 _tmp1DE;({void*_tmp264=(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=0,_tmp10E->f1=cn;_tmp10E;});_tmp1DE.f1=_tmp264;}),_tmp1DE.f2=ain;_tmp1DE;});}}}{
# 960
void*_tmp10F=e->r;void*_stmttmp27=_tmp10F;void*_tmp110=_stmttmp27;struct _fat_ptr _tmp112;struct Cyc_Absyn_MallocInfo _tmp114;enum Cyc_Absyn_Coercion _tmp116;enum Cyc_Absyn_Incrementor _tmp117;void*_tmp115;void*_tmp113;enum Cyc_Absyn_Primop _tmp118;void*_tmp111;union Cyc_Absyn_Cnst _tmp119;switch(*((int*)_tmp110)){case 0: _tmp119=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{union Cyc_Absyn_Cnst c=_tmp119;
# 963
return({struct _tuple13 _tmp1DF;({void*_tmp265=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=1,_tmp11A->f1=e;_tmp11A;});_tmp1DF.f1=_tmp265;}),_tmp1DF.f2=ain;_tmp1DF;});}case 1: _tmp111=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{void*b=_tmp111;
# 965
struct Cyc_Absyn_Vardecl**_tmp11B=Cyc_Vcgen_bind2vardecl(b);struct Cyc_Absyn_Vardecl**newb=_tmp11B;
if(newb == 0)return({struct _tuple13 _tmp1E0;_tmp1E0.f1=0,_tmp1E0.f2=ain;_tmp1E0;});else{
return({struct _tuple13 _tmp1E1;({void*_tmp266=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=2,_tmp11C->f1=*((struct Cyc_Absyn_Vardecl**)_check_null(Cyc_Vcgen_bind2vardecl(b)));_tmp11C;});_tmp1E1.f1=_tmp266;}),_tmp1E1.f2=ain;_tmp1E1;});}}case 3: _tmp118=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp111=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{enum Cyc_Absyn_Primop p=_tmp118;struct Cyc_List_List*es=_tmp111;
# 969
struct _tuple14 _tmp11D=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp28=_tmp11D;struct _tuple14 _tmp11E=_stmttmp28;void*_tmp120;void*_tmp11F;_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;{struct Cyc_List_List*ts=_tmp11F;void*aout=_tmp120;
void*t=0;
if(ts != 0)t=Cyc_AssnDef_primop(p,ts);
# 974
if((int)p == 4 && Cyc_Vcgen_is_unsigned_int_exp(e)){
# 976
struct Cyc_List_List*_tmp121=ts;void*_tmp123;void*_tmp122;if(_tmp121 != 0){if(((struct Cyc_List_List*)_tmp121)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp121)->tl)->tl == 0){_tmp122=(void*)_tmp121->hd;_tmp123=(void*)(_tmp121->tl)->hd;{void*t1=_tmp122;void*t2=_tmp123;
# 978
aout=({void*_tmp267=aout;Cyc_AssnDef_and(_tmp267,Cyc_AssnDef_ult(t,t2));});
goto _LL62;}}else{goto _LL65;}}else{goto _LL65;}}else{_LL65:
 goto _LL62;}_LL62:;}
# 983
return({struct _tuple13 _tmp1E2;_tmp1E2.f1=t,_tmp1E2.f2=aout;_tmp1E2;});}}case 4: _tmp111=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_tmp115=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp110)->f3;{struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Core_Opt*popt=_tmp113;struct Cyc_Absyn_Exp*e2=_tmp115;
# 985
struct _tuple13 _tmp124=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple13 _stmttmp29=_tmp124;struct _tuple13 _tmp125=_stmttmp29;void*_tmp127;void*_tmp126;_tmp126=_tmp125.f1;_tmp127=_tmp125.f2;{void*lt=_tmp126;void*a1=_tmp127;
struct _tuple13 _tmp128=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp2A=_tmp128;struct _tuple13 _tmp129=_stmttmp2A;void*_tmp12B;void*_tmp12A;_tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;{void*t2=_tmp12A;void*a2=_tmp12B;
if(popt != 0){
void*_tmp12C=Cyc_Vcgen_deref_lterm(env,lt,a2);void*t1=_tmp12C;
if(t1 == 0 || t2 == 0)
t2=0;else{
# 992
t2=({enum Cyc_Absyn_Primop _tmp268=(enum Cyc_Absyn_Primop)popt->v;Cyc_AssnDef_primop(_tmp268,({void*_tmp12D[2];_tmp12D[0]=t1,_tmp12D[1]=t2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp12D,sizeof(void*),2));}));});}}{
# 994
void*_tmp12E=Cyc_Vcgen_do_assign(env,a2,lt,t2);void*a=_tmp12E;
return({struct _tuple13 _tmp1E3;_tmp1E3.f1=lt,_tmp1E3.f2=a;_tmp1E3;});}}}}case 5: _tmp111=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp117=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{struct Cyc_Absyn_Exp*e=_tmp111;enum Cyc_Absyn_Incrementor i=_tmp117;
# 997
struct _tuple13 _tmp12F=Cyc_Vcgen_vcgen_lexp(env,e,ain);struct _tuple13 _stmttmp2B=_tmp12F;struct _tuple13 _tmp130=_stmttmp2B;void*_tmp132;void*_tmp131;_tmp131=_tmp130.f1;_tmp132=_tmp130.f2;{void*lt=_tmp131;void*a1=_tmp132;
void*_tmp133=Cyc_Vcgen_deref_lterm(env,lt,a1);void*t1=_tmp133;
void*res=0;
void*assn=0;
{enum Cyc_Absyn_Incrementor _tmp134=i;switch((int)_tmp134){case Cyc_Absyn_PreInc:
# 1003
 if(t1 != 0)assn=Cyc_AssnDef_primop(0U,({void*_tmp135[2];_tmp135[0]=t1,({void*_tmp269=Cyc_AssnDef_one();_tmp135[1]=_tmp269;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp135,sizeof(void*),2));}));
res=assn;
goto _LL70;case Cyc_Absyn_PostInc:
# 1007
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(0U,({void*_tmp136[2];_tmp136[0]=t1,({void*_tmp26A=Cyc_AssnDef_one();_tmp136[1]=_tmp26A;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp136,sizeof(void*),2));}));
goto _LL70;case Cyc_Absyn_PreDec:
# 1011
 if(t1 != 0)assn=Cyc_AssnDef_primop(2U,({void*_tmp137[2];_tmp137[0]=t1,({void*_tmp26B=Cyc_AssnDef_one();_tmp137[1]=_tmp26B;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp137,sizeof(void*),2));}));
res=assn;
goto _LL70;case Cyc_Absyn_PostDec:
# 1015
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(2U,({void*_tmp138[2];_tmp138[0]=t1,({void*_tmp26C=Cyc_AssnDef_one();_tmp138[1]=_tmp26C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp138,sizeof(void*),2));}));
goto _LL70;default:
({void*_tmp139=0U;({struct _fat_ptr _tmp26D=({const char*_tmp13A="vcgen vcgen_rexp increment_e";_tag_fat(_tmp13A,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp26D,_tag_fat(_tmp139,sizeof(void*),0));});});}_LL70:;}{
# 1020
void*_tmp13B=Cyc_Vcgen_do_assign(env,a1,lt,assn);void*a=_tmp13B;
return({struct _tuple13 _tmp1E4;_tmp1E4.f1=res,_tmp1E4.f2=a;_tmp1E4;});}}}case 6: _tmp111=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_tmp115=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp110)->f3;{struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp113;struct Cyc_Absyn_Exp*e3=_tmp115;
# 1023
struct _tuple13 _tmp13C=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp2C=_tmp13C;struct _tuple13 _tmp13D=_stmttmp2C;void*_tmp13F;void*_tmp13E;_tmp13E=_tmp13D.f1;_tmp13F=_tmp13D.f2;{void*at=_tmp13E;void*af=_tmp13F;
struct _tuple13 _tmp140=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp2D=_tmp140;struct _tuple13 _tmp141=_stmttmp2D;void*_tmp143;void*_tmp142;_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;{void*t1=_tmp142;void*at=_tmp143;
struct _tuple13 _tmp144=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);struct _tuple13 _stmttmp2E=_tmp144;struct _tuple13 _tmp145=_stmttmp2E;void*_tmp147;void*_tmp146;_tmp146=_tmp145.f1;_tmp147=_tmp145.f2;{void*t2=_tmp146;void*af=_tmp147;
if(t1 != 0 && t2 != 0){
void*_tmp148=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp148;
at=({void*_tmp26E=at;Cyc_AssnDef_and(_tmp26E,Cyc_AssnDef_eq(v,t1));});
af=({void*_tmp26F=af;Cyc_AssnDef_and(_tmp26F,Cyc_AssnDef_eq(v,t2));});
return({struct _tuple13 _tmp1E5;_tmp1E5.f1=v,({void*_tmp271=({struct Cyc_Vcgen_Env*_tmp270=env;Cyc_Vcgen_may_widen(_tmp270,Cyc_AssnDef_or(at,af));});_tmp1E5.f2=_tmp271;});_tmp1E5;});}else{
# 1032
return({struct _tuple13 _tmp1E6;_tmp1E6.f1=0,({void*_tmp273=({struct Cyc_Vcgen_Env*_tmp272=env;Cyc_Vcgen_may_widen(_tmp272,Cyc_AssnDef_or(at,af));});_tmp1E6.f2=_tmp273;});_tmp1E6;});}}}}}case 7: _tmp111=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp113;
# 1034
struct _tuple13 _tmp149=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp2F=_tmp149;struct _tuple13 _tmp14A=_stmttmp2F;void*_tmp14C;void*_tmp14B;_tmp14B=_tmp14A.f1;_tmp14C=_tmp14A.f2;{void*at=_tmp14B;void*af=_tmp14C;
struct _tuple13 _tmp14D=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp30=_tmp14D;struct _tuple13 _tmp14E=_stmttmp30;void*_tmp150;void*_tmp14F;_tmp14F=_tmp14E.f1;_tmp150=_tmp14E.f2;{void*t=_tmp14F;void*at=_tmp150;
void*_tmp151=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp151;
at=({void*_tmp274=at;Cyc_AssnDef_and(_tmp274,Cyc_AssnDef_eq(v,t));});
af=({void*_tmp276=af;Cyc_AssnDef_and(_tmp276,({void*_tmp275=v;Cyc_AssnDef_eq(_tmp275,Cyc_AssnDef_zero());}));});
return({struct _tuple13 _tmp1E7;_tmp1E7.f1=v,({void*_tmp278=({struct Cyc_Vcgen_Env*_tmp277=env;Cyc_Vcgen_may_widen(_tmp277,Cyc_AssnDef_or(at,af));});_tmp1E7.f2=_tmp278;});_tmp1E7;});}}}case 8: _tmp111=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp113;
# 1041
struct _tuple13 _tmp152=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp31=_tmp152;struct _tuple13 _tmp153=_stmttmp31;void*_tmp155;void*_tmp154;_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;{void*at=_tmp154;void*af=_tmp155;
struct _tuple13 _tmp156=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);struct _tuple13 _stmttmp32=_tmp156;struct _tuple13 _tmp157=_stmttmp32;void*_tmp159;void*_tmp158;_tmp158=_tmp157.f1;_tmp159=_tmp157.f2;{void*t=_tmp158;void*af=_tmp159;
void*_tmp15A=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp15A;
at=({void*_tmp27A=at;Cyc_AssnDef_and(_tmp27A,({void*_tmp279=v;Cyc_AssnDef_eq(_tmp279,Cyc_AssnDef_one());}));});
af=({void*_tmp27B=af;Cyc_AssnDef_and(_tmp27B,Cyc_AssnDef_eq(v,t));});
return({struct _tuple13 _tmp1E8;_tmp1E8.f1=v,({void*_tmp27D=({struct Cyc_Vcgen_Env*_tmp27C=env;Cyc_Vcgen_may_widen(_tmp27C,Cyc_AssnDef_or(at,af));});_tmp1E8.f2=_tmp27D;});_tmp1E8;});}}}case 9: _tmp111=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp113;
# 1048
struct _tuple13 _tmp15B=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp33=_tmp15B;struct _tuple13 _tmp15C=_stmttmp33;void*_tmp15D;_tmp15D=_tmp15C.f2;{void*a=_tmp15D;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10: _tmp111=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{struct Cyc_Absyn_Exp*e=_tmp111;struct Cyc_List_List*es=_tmp113;
# 1051
struct _tuple14 _tmp15E=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp34=_tmp15E;struct _tuple14 _tmp15F=_stmttmp34;void*_tmp161;void*_tmp160;_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;{struct Cyc_List_List*ts=_tmp160;void*a=_tmp161;
struct _tuple13 _tmp162=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple13 _stmttmp35=_tmp162;struct _tuple13 _tmp163=_stmttmp35;void*_tmp164;_tmp164=_tmp163.f2;{void*a2=_tmp164;
# 1054
struct _tuple17 _tmp165=Cyc_Vcgen_get_requires_and_ensures(e->topt);struct _tuple17 _stmttmp36=_tmp165;struct _tuple17 _tmp166=_stmttmp36;void*_tmp16A;void*_tmp169;void*_tmp168;void*_tmp167;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;_tmp169=_tmp166.f3;_tmp16A=_tmp166.f4;{struct Cyc_List_List*args=_tmp167;struct Cyc_Absyn_Exp*reqopt=_tmp168;struct Cyc_Absyn_Exp*ensopt=_tmp169;struct Cyc_Absyn_Vardecl*ret_value=_tmp16A;
Cyc_Vcgen_check_requires(env,ts,args,a2,reqopt);
# 1058
a2=Cyc_AssnDef_kill(a2);
Cyc_Vcgen_update_try_assn(env,a2);
# 1061
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e->topt)))a2=(void*)& Cyc_AssnDef_false_assn;
if(ensopt == 0)
return({struct _tuple13 _tmp1E9;_tmp1E9.f1=0,_tmp1E9.f2=a2;_tmp1E9;});{
struct Cyc_Absyn_Exp*ensures=ensopt;
# 1066
struct _tuple13 _tmp16B=Cyc_Vcgen_vcgen_test(env,ensures,a2);struct _tuple13 _stmttmp37=_tmp16B;struct _tuple13 _tmp16C=_stmttmp37;void*_tmp16D;_tmp16D=_tmp16C.f1;{void*at=_tmp16D;
if(ret_value != 0){
void*_tmp16E=Cyc_AssnDef_fresh_var(ret_value->type);void*v=_tmp16E;
# 1070
at=Cyc_AssnDef_subst(ret_value,v,at);
return({struct _tuple13 _tmp1EA;_tmp1EA.f1=v,_tmp1EA.f2=at;_tmp1EA;});}
# 1073
return({struct _tuple13 _tmp1EB;_tmp1EB.f1=0,_tmp1EB.f2=at;_tmp1EB;});}}}}}}case 11: _tmp111=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Exp*e=_tmp111;
# 1075
struct _tuple13 _tmp16F=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp38=_tmp16F;struct _tuple13 _tmp170=_stmttmp38;void*_tmp171;_tmp171=_tmp170.f2;{void*a=_tmp171;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple13 _tmp1EC;_tmp1EC.f1=0,_tmp1EC.f2=(void*)& Cyc_AssnDef_false_assn;_tmp1EC;});}}case 12: _tmp111=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Exp*e=_tmp111;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13: _tmp111=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Exp*e=_tmp111;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14: _tmp111=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_tmp116=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp110)->f4;{void*tp=_tmp111;struct Cyc_Absyn_Exp*e1=_tmp113;enum Cyc_Absyn_Coercion c=_tmp116;
# 1083
struct _tuple13 _tmp172=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp39=_tmp172;struct _tuple13 _tmp173=_stmttmp39;void*_tmp175;void*_tmp174;_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;{void*topt=_tmp174;void*a=_tmp175;
# 1091
{struct _tuple13 _tmp176=({struct _tuple13 _tmp1F0;({void*_tmp27F=Cyc_Absyn_compress(tp);_tmp1F0.f1=_tmp27F;}),({void*_tmp27E=Cyc_Absyn_compress((void*)_check_null(e->topt));_tmp1F0.f2=_tmp27E;});_tmp1F0;});struct _tuple13 _stmttmp3A=_tmp176;struct _tuple13 _tmp177=_stmttmp3A;enum Cyc_Absyn_Size_of _tmp17B;enum Cyc_Absyn_Sign _tmp17A;enum Cyc_Absyn_Size_of _tmp179;enum Cyc_Absyn_Sign _tmp178;if(*((int*)_tmp177.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f1)->f1)== 1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f1)->f1)->f2){case Cyc_Absyn_Int_sz: if(*((int*)_tmp177.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f2)->f1)){case 4:
# 1095
 return({struct _tuple13 _tmp1ED;_tmp1ED.f1=topt,_tmp1ED.f2=a;_tmp1ED;});case 1: goto _LLAA;default: goto _LLAC;}else{goto _LLAC;}case Cyc_Absyn_Long_sz: if(*((int*)_tmp177.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f2)->f1)){case 4:
# 1097
 return({struct _tuple13 _tmp1EE;_tmp1EE.f1=topt,_tmp1EE.f2=a;_tmp1EE;});case 1: goto _LLAA;default: goto _LLAC;}else{goto _LLAC;}default: if(*((int*)_tmp177.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f2)->f1)== 1){_LLAA: _tmp178=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f1)->f1)->f1;_tmp179=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f1)->f1)->f2;_tmp17A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f2)->f1)->f1;_tmp17B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177.f2)->f1)->f2;{enum Cyc_Absyn_Sign u1=_tmp178;enum Cyc_Absyn_Size_of s1=_tmp179;enum Cyc_Absyn_Sign u2=_tmp17A;enum Cyc_Absyn_Size_of s2=_tmp17B;
# 1103
s1=(int)s1 == 3?2U: s1;
s2=(int)s2 == 3?2U: s2;
if((int)s1 == (int)s2)return({struct _tuple13 _tmp1EF;_tmp1EF.f1=topt,_tmp1EF.f2=a;_tmp1EF;});
goto _LLA5;}}else{goto _LLAC;}}else{goto _LLAC;}}else{goto _LLAC;}}else{_LLAC:
 goto _LLA5;}_LLA5:;}
# 1109
if((int)c == 1){
if(topt == 0)return({struct _tuple13 _tmp1F1;_tmp1F1.f1=0,_tmp1F1.f2=a;_tmp1F1;});{
void*t=topt;
return({struct _tuple13 _tmp1F2;({void*_tmp280=Cyc_AssnDef_cast(tp,t);_tmp1F2.f1=_tmp280;}),_tmp1F2.f2=a;_tmp1F2;});}}else{
# 1115
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple13 _tmp1F3;topt == 0?_tmp1F3.f1=0:({void*_tmp281=Cyc_AssnDef_cast(tp,topt);_tmp1F3.f1=_tmp281;}),_tmp1F3.f2=a;_tmp1F3;});}}}case 16: _tmp111=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{struct Cyc_Absyn_Exp*eopt=_tmp111;struct Cyc_Absyn_Exp*e=_tmp113;
# 1119
if(eopt != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain)).f2;
return({struct _tuple13 _tmp1F4;_tmp1F4.f1=0,({void*_tmp282=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp1F4.f2=_tmp282;});_tmp1F4;});}case 32:
# 1124
 goto _LL2A;case 33: _LL2A:
 goto _LL2C;case 17: _LL2C:
 goto _LL2E;case 18: _LL2E:
 goto _LL30;case 39: _LL30:
 goto _LL32;case 19: _LL32:
 return({struct _tuple13 _tmp1F5;({void*_tmp283=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->tag=1,_tmp17C->f1=e;_tmp17C;});_tmp1F5.f1=_tmp283;}),_tmp1F5.f2=ain;_tmp1F5;});case 21: _tmp111=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Exp*e=_tmp111;
# 1132
_tmp111=e;goto _LL36;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple13 _tmp1F6;_tmp1F6.f1=0,_tmp1F6.f2=ain;_tmp1F6;});}case 38: _tmp111=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp111;
# 1137
_tmp111=e;goto _LL38;}case 15: _tmp111=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp111;
# 1139
return({struct _tuple13 _tmp1F7;_tmp1F7.f1=0,({void*_tmp284=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp1F7.f2=_tmp284;});_tmp1F7;});}case 20: _tmp111=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Exp*e=_tmp111;
# 1141
_tmp111=e;goto _LL3C;}case 22: _tmp111=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp111;
_tmp111=e;_tmp113=Cyc_Vcgen_shared_zero_exp();goto _LL3E;}case 23: _tmp111=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp113;
# 1144
struct _tuple13 _tmp17D=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);struct _tuple13 _stmttmp3B=_tmp17D;struct _tuple13 _tmp17E=_stmttmp3B;void*_tmp180;void*_tmp17F;_tmp17F=_tmp17E.f1;_tmp180=_tmp17E.f2;{void*t1=_tmp17F;void*a1=_tmp180;
struct _tuple13 _tmp181=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp3C=_tmp181;struct _tuple13 _tmp182=_stmttmp3C;void*_tmp184;void*_tmp183;_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;{void*t2=_tmp183;void*a2=_tmp184;
{void*_tmp185=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp3D=_tmp185;void*_tmp186=_stmttmp3D;if(*((int*)_tmp186)== 6)
goto _LLB4;else{
# 1151
Cyc_Vcgen_update_try_assn(env,a2);{
void*term_numelts;void*term_typebound;
# 1155
if(t2 == 0)goto _LLB4;
term_numelts=Cyc_AssnDef_primop(18U,({void*_tmp187[1];_tmp187[0]=t1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp187,sizeof(void*),1));}));{
# 1167 "vcgen.cyc"
void*_tmp188=Cyc_AssnDef_ult(t2,term_numelts);void*inbound_assn=_tmp188;
({struct Cyc_Hashtable_Table**_tmp286=env->assn_info;struct Cyc_Absyn_Exp*_tmp285=e1;Cyc_Vcgen_insert_assn_info(_tmp286,_tmp285,({struct _tuple13*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->f1=a2,_tmp189->f2=inbound_assn;_tmp189;}));});
a2=Cyc_AssnDef_and(a2,inbound_assn);}}}_LLB4:;}
# 1171
return({struct _tuple13 _tmp1F8;_tmp1F8.f1=0,_tmp1F8.f2=a2;_tmp1F8;});}}}case 31: _tmp111=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_List_List*es=_tmp111;
# 1173
_tmp111=es;goto _LL42;}case 24: _tmp111=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_LL42: {struct Cyc_List_List*es=_tmp111;
return({struct _tuple13 _tmp1F9;_tmp1F9.f1=0,({void*_tmp287=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_tmp1F9.f2=_tmp287;});_tmp1F9;});}case 25: _tmp111=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{struct Cyc_List_List*dles=_tmp111;
# 1176
_tmp111=dles;goto _LL46;}case 26: _tmp111=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_LL46: {struct Cyc_List_List*dles=_tmp111;
_tmp111=dles;goto _LL48;}case 29: _tmp111=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp110)->f3;_LL48: {struct Cyc_List_List*dles=_tmp111;
_tmp111=dles;goto _LL4A;}case 36: _tmp111=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_LL4A: {struct Cyc_List_List*dles=_tmp111;
_tmp111=dles;goto _LL4C;}case 30: _tmp111=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_LL4C: {struct Cyc_List_List*dles=_tmp111;
# 1181
for(1;dles != 0;dles=dles->tl){
struct _tuple18*_tmp18A=(struct _tuple18*)dles->hd;struct _tuple18*_stmttmp3E=_tmp18A;struct _tuple18*_tmp18B=_stmttmp3E;void*_tmp18C;_tmp18C=_tmp18B->f2;{struct Cyc_Absyn_Exp*e=_tmp18C;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;}}
# 1185
return({struct _tuple13 _tmp1FA;_tmp1FA.f1=0,_tmp1FA.f2=ain;_tmp1FA;});}case 27: _tmp111=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_tmp115=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp110)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp111;struct Cyc_Absyn_Exp*e1=_tmp113;struct Cyc_Absyn_Exp*e2=_tmp115;
# 1189
struct _tuple13 _tmp18D=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);struct _tuple13 _stmttmp3F=_tmp18D;struct _tuple13 _tmp18E=_stmttmp3F;void*_tmp190;void*_tmp18F;_tmp18F=_tmp18E.f1;_tmp190=_tmp18E.f2;{void*t1=_tmp18F;void*a=_tmp190;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp191=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->tag=2,_tmp198->f1=vd;_tmp198;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*v=_tmp191;
# 1192
a=({struct Cyc_Vcgen_Env*_tmp28A=env;void*_tmp289=a;void*_tmp288=(void*)v;Cyc_Vcgen_do_assign(_tmp28A,_tmp289,_tmp288,Cyc_AssnDef_zero());});
while(1){
void*_tmp192=({void*_tmp28B=a;Cyc_AssnDef_and(_tmp28B,Cyc_AssnDef_ult((void*)v,t1));});void*at=_tmp192;
void*_tmp193=({void*_tmp28C=a;Cyc_AssnDef_and(_tmp28C,Cyc_AssnDef_ulte(t1,(void*)v));});void*af=_tmp193;
struct _tuple13 _tmp194=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp40=_tmp194;struct _tuple13 _tmp195=_stmttmp40;void*_tmp196;_tmp196=_tmp195.f2;{void*abody=_tmp196;
abody=({struct Cyc_Vcgen_Env*_tmp290=env;void*_tmp28F=abody;void*_tmp28E=(void*)v;Cyc_Vcgen_do_assign(_tmp290,_tmp28F,_tmp28E,Cyc_AssnDef_primop(0U,({void*_tmp197[2];_tmp197[0]=(void*)v,({void*_tmp28D=Cyc_AssnDef_one();_tmp197[1]=_tmp28D;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp197,sizeof(void*),2));})));});
if(Cyc_Vcgen_simple_implies(& abody,& a)){a=af;break;}
# 1200
a=Cyc_Vcgen_widen(Cyc_AssnDef_or(abody,a));}}
# 1202
return({struct _tuple13 _tmp1FB;_tmp1FB.f1=0,_tmp1FB.f2=a;_tmp1FB;});}}case 28: _tmp111=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Exp*e=_tmp111;
# 1204
return({struct _tuple13 _tmp1FC;_tmp1FC.f1=0,({void*_tmp291=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp1FC.f2=_tmp291;});_tmp1FC;});}case 34: _tmp114=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_MallocInfo mi=_tmp114;
# 1206
if(mi.rgn != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain)).f2;
return({struct _tuple13 _tmp1FD;_tmp1FD.f1=0,({void*_tmp292=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain)).f2;_tmp1FD.f2=_tmp292;});_tmp1FD;});}case 35: _tmp111=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp113=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp110)->f2;{struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp113;
# 1210
struct _tuple13 _tmp199=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple13 _stmttmp41=_tmp199;struct _tuple13 _tmp19A=_stmttmp41;void*_tmp19C;void*_tmp19B;_tmp19B=_tmp19A.f1;_tmp19C=_tmp19A.f2;{void*lt1=_tmp19B;void*a1=_tmp19C;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple13 _tmp19D=Cyc_Vcgen_vcgen_lexp(env,e2,a1);struct _tuple13 _stmttmp42=_tmp19D;struct _tuple13 _tmp19E=_stmttmp42;void*_tmp1A0;void*_tmp19F;_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19E.f2;{void*lt2=_tmp19F;void*a2=_tmp1A0;
Cyc_Vcgen_update_try_assn(env,a2);{
void*_tmp1A1=Cyc_Vcgen_deref_lterm(env,lt1,a2);void*t1=_tmp1A1;
void*_tmp1A2=Cyc_Vcgen_deref_lterm(env,lt2,a2);void*t2=_tmp1A2;
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple13 _tmp1FE;_tmp1FE.f1=0,_tmp1FE.f2=a2;_tmp1FE;});}}}}}case 37: _tmp111=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Stmt*s=_tmp111;
# 1220
void**_tmp1A3=(env->shared)->exp_stmt;void**old_exp_stmt=_tmp1A3;
({void**_tmp293=({void**_tmp1A4=_cycalloc(sizeof(*_tmp1A4));*_tmp1A4=0;_tmp1A4;});(env->shared)->exp_stmt=_tmp293;});{
void*_tmp1A5=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);void*aout=_tmp1A5;
void*_tmp1A6=*((void**)_check_null((env->shared)->exp_stmt));void*t=_tmp1A6;
(env->shared)->exp_stmt=old_exp_stmt;
return({struct _tuple13 _tmp1FF;_tmp1FF.f1=t,_tmp1FF.f2=aout;_tmp1FF;});}}case 40:
 return({struct _tuple13 _tmp200;_tmp200.f1=0,_tmp200.f2=ain;_tmp200;});case 41: _tmp111=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Exp*e=_tmp111;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2: _tmp112=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct _fat_ptr s=_tmp112;
return({struct _tuple13 _tmp201;({void*_tmp294=Cyc_AssnDef_zero();_tmp201.f1=_tmp294;}),_tmp201.f2=ain;_tmp201;});}default: _tmp111=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp110)->f1;{struct Cyc_Absyn_Exp*e1=_tmp111;
# 1230
struct _tuple13 _tmp1A7=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp43=_tmp1A7;struct _tuple13 _tmp1A8=_stmttmp43;void*_tmp1A9;_tmp1A9=_tmp1A8.f1;{void*at=_tmp1A9;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp1AC=({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0,({
struct _fat_ptr _tmp295=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_tmp202.f1=_tmp295;});_tmp202;});void*_tmp1AA[1];_tmp1AA[0]=& _tmp1AC;({unsigned _tmp297=e->loc;struct _fat_ptr _tmp296=({const char*_tmp1AB="cannot prove @assert from %s";_tag_fat(_tmp1AB,sizeof(char),29U);});Cyc_Warn_err(_tmp297,_tmp296,_tag_fat(_tmp1AA,sizeof(void*),1));});});
return({struct _tuple13 _tmp203;({void*_tmp299=Cyc_AssnDef_one();_tmp203.f1=_tmp299;}),({void*_tmp298=Cyc_AssnDef_and(ain,at);_tmp203.f2=_tmp298;});_tmp203;});}}};}}
# 1240
void Cyc_Vcgen_vcgen_fundecl(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
void*a=(void*)& Cyc_AssnDef_true_assn;
# 1243
struct _tuple17 _tmp1AD=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);struct _tuple17 _stmttmp44=_tmp1AD;struct _tuple17 _tmp1AE=_stmttmp44;void*_tmp1B2;void*_tmp1B1;void*_tmp1B0;void*_tmp1AF;_tmp1AF=_tmp1AE.f1;_tmp1B0=_tmp1AE.f2;_tmp1B1=_tmp1AE.f3;_tmp1B2=_tmp1AE.f4;{struct Cyc_List_List*args=_tmp1AF;struct Cyc_Absyn_Exp*reqopt=_tmp1B0;struct Cyc_Absyn_Exp*ensopt=_tmp1B1;struct Cyc_Absyn_Vardecl*ret_value=_tmp1B2;
void**result_assn=ensopt == 0?0:({void**_tmp1BF=_cycalloc(sizeof(*_tmp1BF));*_tmp1BF=(void*)& Cyc_AssnDef_false_assn;_tmp1BF;});
struct Cyc_Vcgen_Env*_tmp1B3=Cyc_Vcgen_initial_env(tables,fd,assn_info,result_assn);struct Cyc_Vcgen_Env*env=_tmp1B3;
if(reqopt != 0)
# 1248
a=(Cyc_Vcgen_vcgen_test(env,reqopt,a)).f1;
# 1250
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 1252
if(ensopt != 0){
struct Cyc_Absyn_Exp*ens=ensopt;
struct _tuple13 _tmp1B4=Cyc_Vcgen_vcgen_test(env,ens,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp45=_tmp1B4;struct _tuple13 _tmp1B5=_stmttmp45;void*_tmp1B6;_tmp1B6=_tmp1B5.f1;{void*at=_tmp1B6;
struct Cyc_Vcgen_SharedEnv*_tmp1B7=env->shared;struct Cyc_Vcgen_SharedEnv*senv=_tmp1B7;
void*_tmp1B8=*((void**)_check_null(senv->res_assn));void*res_assn_opt=_tmp1B8;
void*res_assn=(void*)& Cyc_AssnDef_false_assn;
if(res_assn_opt != 0)res_assn=res_assn_opt;{
struct _fat_ptr str=({const char*_tmp1BE="NULL";_tag_fat(_tmp1BE,sizeof(char),5U);});
if(senv->res_term != 0)str=Cyc_AssnDef_term2string((void*)_check_null(senv->res_term));
if(ret_value != 0 &&(unsigned)senv->res_term)
res_assn=({void*_tmp29B=res_assn;Cyc_AssnDef_and(_tmp29B,({void*_tmp29A=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9->tag=2,_tmp1B9->f1=ret_value;_tmp1B9;});Cyc_AssnDef_eq(_tmp29A,senv->res_term);}));});
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp1BC=({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0,({
struct _fat_ptr _tmp29C=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp205.f1=_tmp29C;});_tmp205;});struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0,({struct _fat_ptr _tmp29D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(res_assn)));_tmp204.f1=_tmp29D;});_tmp204;});void*_tmp1BA[2];_tmp1BA[0]=& _tmp1BC,_tmp1BA[1]=& _tmp1BD;({unsigned _tmp29F=ens->loc;struct _fat_ptr _tmp29E=({const char*_tmp1BB="cannot prove the @ensures clause %s from %s";_tag_fat(_tmp1BB,sizeof(char),44U);});Cyc_Warn_err(_tmp29F,_tmp29E,_tag_fat(_tmp1BA,sizeof(void*),2));});});}}}
# 1267
Cyc_AssnDef_reset_hash_cons_table();}}

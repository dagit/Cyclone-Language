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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
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
void*Cyc_AssnDef_neq(void*,void*);
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
({void*_tmp241=Cyc_AssnDef_reduce(*a1);*a1=_tmp241;});
({void*_tmp242=Cyc_AssnDef_reduce(*a2);*a2=_tmp242;});
return Cyc_AssnDef_simple_prove(*a1,*a2);}
# 127
static int Cyc_Vcgen_implies(void**a1,void**a2){
return Cyc_Vcgen_simple_implies(a1,a2)|| Cyc_PrattProver_constraint_prove(*a1,*a2);}
# 142
static struct Cyc_Set_Set*Cyc_Vcgen_widen_it(struct Cyc_Set_Set*s,void*a){
if(s == 0)return 0;{
void*_tmp0=a;enum Cyc_AssnDef_Primreln _tmp3;void*_tmp2;void*_tmp1;switch(*((int*)_tmp0)){case 2: if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f2 == Cyc_AssnDef_Eq){_tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f3;{void*t1=_tmp1;void*t2=_tmp2;
# 146
return({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;})(s,a);}}else{_tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f1;_tmp3=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f2;_tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp0)->f3;{void*t1=_tmp1;enum Cyc_AssnDef_Primreln p=_tmp3;void*t2=_tmp2;
return({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;})(s,a);}}case 3: _tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp0)->f2;{void*a1=_tmp1;void*a2=_tmp2;
return({struct Cyc_Set_Set*_tmp243=Cyc_Vcgen_widen_it(s,a1);Cyc_Vcgen_widen_it(_tmp243,a2);});}case 4: _tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp0)->f2;{void*a1=_tmp1;void*a2=_tmp2;
# 150
struct Cyc_Set_Set*_tmp4=Cyc_Vcgen_widen_it(s,a1);struct Cyc_Set_Set*s1=_tmp4;
struct Cyc_Set_Set*_tmp5=Cyc_Vcgen_widen_it(s,a2);struct Cyc_Set_Set*s2=_tmp5;
if(s1 == 0 || s2 == 0)return 0;
return({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_intersect;})(s1,s2);}case 0:
 return s;case 1:
 return 0;default:
 return({struct Cyc_Set_Set*_tmp244=s;Cyc_Vcgen_widen_it(_tmp244,Cyc_AssnDef_reduce(a));});};}}
# 160
static void*Cyc_Vcgen_widen(void*a){
struct Cyc_Set_Set*_tmp6=({struct Cyc_Set_Set*_tmp245=({(struct Cyc_Set_Set*(*)(int(*)(void*,void*)))Cyc_Set_empty;})(Cyc_AssnDef_assncmp);Cyc_Vcgen_widen_it(_tmp245,a);});struct Cyc_Set_Set*sopt=_tmp6;
if(sopt == 0)return(void*)& Cyc_AssnDef_false_assn;
return({(void*(*)(void*(*)(void*,void*),struct Cyc_Set_Set*,void*))Cyc_Set_fold;})(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);}
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
return({void*_tmp246=a;Cyc_AssnDef_and(_tmp246,Cyc_AssnDef_eq(tgt,src));});}}else{
({void*_tmpB=0U;({int(*_tmp248)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp247=({const char*_tmpC="vcgen do_assign";_tag_fat(_tmpC,sizeof(char),16U);});_tmp248(_tmp247,_tag_fat(_tmpB,sizeof(void*),0));});});}};}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void**try_assn;void**exp_stmt;void*res_term;void**res_assn;int widen_paths;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;struct Cyc_Hashtable_Table**assn_info;};
# 246 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 248
static void*Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*env,void*a){
return(env->shared)->widen_paths?Cyc_Vcgen_widen(a): a;}
# 252
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,void**res_assn){
# 254
struct Cyc_Hashtable_Table*_tmpD=({(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;})(tables->succ_tables,fd);struct Cyc_Hashtable_Table*succ_table=_tmpD;
struct Cyc_Hashtable_Table*_tmpE=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;})(57,({(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;}));struct Cyc_Hashtable_Table*assn_table=_tmpE;
struct Cyc_Vcgen_SharedEnv*_tmpF=({struct Cyc_Vcgen_SharedEnv*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->assn_table=assn_table,_tmp12->succ_table=succ_table,_tmp12->try_assn=0,_tmp12->exp_stmt=0,_tmp12->res_term=0,_tmp12->res_assn=res_assn,_tmp12->widen_paths=!Cyc_Flags_allpaths;_tmp12;});struct Cyc_Vcgen_SharedEnv*senv=_tmpF;
# 263
struct Cyc_Dict_Dict _tmp10=({(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty;})(({(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp;}));struct Cyc_Dict_Dict d=_tmp10;
return({struct Cyc_Vcgen_Env*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->shared=senv,_tmp11->state_counter=d,_tmp11->assn_info=assn_info;_tmp11;});}
# 267
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_tmp13=_cycalloc(sizeof(*_tmp13));*_tmp13=*env;_tmp13;});}
# 273
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 277
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _tmp249=({(struct Cyc_Dict_Dict(*)(int(*)(int,struct Cyc_Absyn_Vardecl*,int,int),int,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_union_two_c;})(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_tmp249;});}
# 285
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int*))Cyc_Dict_lookup_bool;})(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _tmp24A=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int))Cyc_Dict_insert;})(env->state_counter,vd,res + 1);env->state_counter=_tmp24A;});
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=3,_tmp14->f1=vd,_tmp14->f2=res,_tmp14->f3=vd->type;_tmp14;});}
# 294
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***_tmp15=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})((env->shared)->assn_table,s);void***popt=_tmp15;
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
void***popt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})((env->shared)->assn_table,s);
if(popt != 0)
return*popt;else{
# 312
return& false_pointer;}}
# 317
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp17=*aprevopt;void**aprev=_tmp17;
# 322
ain=Cyc_Vcgen_widen(Cyc_AssnDef_or(ain,*aprev));
*aprev=ain;}else{
# 325
({void(*_tmp24D)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**)=({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp24C=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp24B=s;_tmp24D(_tmp24C,_tmp24B,({void**_tmp18=_cycalloc(sizeof(*_tmp18));*_tmp18=ain;_tmp18;}));});}
# 327
return ain;}
# 332
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp19=*aprevopt;void**aprev=_tmp19;
ain=({struct Cyc_Vcgen_Env*_tmp24E=env;Cyc_Vcgen_may_widen(_tmp24E,Cyc_AssnDef_or(ain,*aprev));});
*aprev=ain;}else{
# 339
({void(*_tmp251)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**)=({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp250=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp24F=s;_tmp251(_tmp250,_tmp24F,({void**_tmp1A=_cycalloc(sizeof(*_tmp1A));*_tmp1A=ain;_tmp1A;}));});}
# 341
return ain;}
# 346
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
if((env->shared)->try_assn == 0)return;
a=({struct Cyc_Vcgen_Env*_tmp252=env;Cyc_Vcgen_may_widen(_tmp252,Cyc_AssnDef_or(*((void**)_check_null((env->shared)->try_assn)),a));});
*((void**)_check_null((env->shared)->try_assn))=a;}
# 353
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a,void*topt){
struct Cyc_Vcgen_SharedEnv*_tmp1B=env->shared;struct Cyc_Vcgen_SharedEnv*shared=_tmp1B;
if(shared->res_assn == 0)return;
if(shared->res_term != 0 || t != 0){
void*_tmp1C=Cyc_AssnDef_fresh_var(topt);void*v=_tmp1C;
shared->res_term=v;
({void*_tmp256=({
void*_tmp255=({void*_tmp253=a;Cyc_AssnDef_and(_tmp253,Cyc_AssnDef_eq(v,t));});Cyc_AssnDef_or(_tmp255,({void*_tmp254=*((void**)_check_null(shared->res_assn));Cyc_AssnDef_and(_tmp254,Cyc_AssnDef_eq(v,shared->res_term));}));});
# 359
*((void**)_check_null(shared->res_assn))=_tmp256;});}else{
# 362
({void*_tmp257=Cyc_AssnDef_or(a,*((void**)_check_null(shared->res_assn)));*((void**)_check_null(shared->res_assn))=_tmp257;});}}
# 367
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void){
struct Cyc_Hashtable_Table*_tmp1D=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;})(57,({(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr;}));struct Cyc_Hashtable_Table*assn_info=_tmp1D;
return({struct Cyc_Hashtable_Table**_tmp1E=_cycalloc(sizeof(*_tmp1E));*_tmp1E=assn_info;_tmp1E;});}struct _tuple13{void*f1;void*f2;};
# 372
struct _tuple13 Cyc_Vcgen_exp2ctxt_assn(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){
struct _tuple13**_tmp1F=({(struct _tuple13**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_lookup_opt;})(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e);struct _tuple13**result=_tmp1F;
if(result == 0)
# 377
return({struct _tuple13 _tmp1F4;_tmp1F4.f1=(void*)& Cyc_AssnDef_true_assn,_tmp1F4.f2=(void*)& Cyc_AssnDef_false_assn;_tmp1F4;});else{
# 379
return*(*result);}}
# 383
static void Cyc_Vcgen_insert_assn_info(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct _tuple13*ctxt_assn){
# 391
({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct _tuple13*))Cyc_Hashtable_insert;})(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e,ctxt_assn);
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
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp1F5;_tmp1F5.tag=0,({struct _fat_ptr _tmp258=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1F5.f1=_tmp258;});_tmp1F5;});void*_tmp25[1];_tmp25[0]=& _tmp27;({struct _fat_ptr _tmp259=({const char*_tmp26="if (%s) ...";_tag_fat(_tmp26,sizeof(char),12U);});Cyc_aprintf(_tmp259,_tag_fat(_tmp25,sizeof(void*),1));});});}case 5: _tmp22=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp21)->f1).f1;{struct Cyc_Absyn_Exp*e=_tmp22;
# 438
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp1F6;_tmp1F6.tag=0,({struct _fat_ptr _tmp25A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1F6.f1=_tmp25A;});_tmp1F6;});void*_tmp28[1];_tmp28[0]=& _tmp2A;({struct _fat_ptr _tmp25B=({const char*_tmp29="while (%s) ...";_tag_fat(_tmp29,sizeof(char),15U);});Cyc_aprintf(_tmp25B,_tag_fat(_tmp28,sizeof(void*),1));});});}case 9: _tmp22=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp23=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f1;_tmp24=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f3).f1;{struct Cyc_Absyn_Exp*e1=_tmp22;struct Cyc_Absyn_Exp*e2=_tmp23;struct Cyc_Absyn_Exp*e3=_tmp24;
# 440
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D=({struct Cyc_String_pa_PrintArg_struct _tmp1F9;_tmp1F9.tag=0,({struct _fat_ptr _tmp25C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp1F9.f1=_tmp25C;});_tmp1F9;});struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp1F8;_tmp1F8.tag=0,({
struct _fat_ptr _tmp25D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e2));_tmp1F8.f1=_tmp25D;});_tmp1F8;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0,({struct _fat_ptr _tmp25E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e3));_tmp1F7.f1=_tmp25E;});_tmp1F7;});void*_tmp2B[3];_tmp2B[0]=& _tmp2D,_tmp2B[1]=& _tmp2E,_tmp2B[2]=& _tmp2F;({struct _fat_ptr _tmp25F=({const char*_tmp2C="for (%s;%s;%s) ...";_tag_fat(_tmp2C,sizeof(char),19U);});Cyc_aprintf(_tmp25F,_tag_fat(_tmp2B,sizeof(void*),3));});});}case 10: _tmp22=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;{struct Cyc_Absyn_Exp*e=_tmp22;
# 443
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0,({struct _fat_ptr _tmp260=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1FA.f1=_tmp260;});_tmp1FA;});void*_tmp30[1];_tmp30[0]=& _tmp32;({struct _fat_ptr _tmp261=({const char*_tmp31="switch (%s) ...";_tag_fat(_tmp31,sizeof(char),16U);});Cyc_aprintf(_tmp261,_tag_fat(_tmp30,sizeof(void*),1));});});}case 14: _tmp22=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f1;{struct Cyc_Absyn_Exp*e=_tmp22;
# 445
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0,({struct _fat_ptr _tmp262=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1FB.f1=_tmp262;});_tmp1FB;});void*_tmp33[1];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp263=({const char*_tmp34="do ... while (%s)";_tag_fat(_tmp34,sizeof(char),18U);});Cyc_aprintf(_tmp263,_tag_fat(_tmp33,sizeof(void*),1));});});}case 15: _tmp22=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;{struct Cyc_Absyn_Stmt*s=_tmp22;
# 447
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0,({struct _fat_ptr _tmp264=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_stmt2shortstring(s));_tmp1FC.f1=_tmp264;});_tmp1FC;});void*_tmp36[1];_tmp36[0]=& _tmp38;({struct _fat_ptr _tmp265=({const char*_tmp37="try %s catch ...";_tag_fat(_tmp37,sizeof(char),17U);});Cyc_aprintf(_tmp265,_tag_fat(_tmp36,sizeof(void*),1));});});}case 2: _tmp22=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;{struct Cyc_Absyn_Stmt*s=_tmp22;
return Cyc_Vcgen_stmt2shortstring(s);}default:  {
# 450
struct _fat_ptr _tmp39=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_tmp39;
unsigned max=_get_fat_size(s,sizeof(char))< 11U?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _tmp3D=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_tmp3D),1U,_tmp3D);});
{int i=0;for(0;(unsigned)i < max - 1U;++ i){
({char _tmp266=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_tmp266;});}}
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3C=({struct Cyc_String_pa_PrintArg_struct _tmp1FD;_tmp1FD.tag=0,_tmp1FD.f1=(struct _fat_ptr)((struct _fat_ptr)res);_tmp1FD;});void*_tmp3A[1];_tmp3A[0]=& _tmp3C;({struct _fat_ptr _tmp267=({const char*_tmp3B="%s...";_tag_fat(_tmp3B,sizeof(char),6U);});Cyc_aprintf(_tmp267,_tag_fat(_tmp3A,sizeof(void*),1));});});}};}
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
return({struct Cyc_Vcgen_Env*_tmp268=env;Cyc_Vcgen_may_widen(_tmp268,Cyc_AssnDef_or(at,af));});}}case 5: _tmp40=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1).f1;_tmp41=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1).f2;_tmp42=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Exp*e=_tmp40;struct Cyc_Absyn_Stmt*dummy=_tmp41;struct Cyc_Absyn_Stmt*s=_tmp42;
# 558
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_tmp53=e->r;void*_stmttmp5=_tmp53;void*_tmp54=_stmttmp5;void*_tmp56;void*_tmp55;if(*((int*)_tmp54)== 7){_tmp55=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp54)->f1;_tmp56=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp54)->f2;{struct Cyc_Absyn_Exp*eleft=_tmp55;struct Cyc_Absyn_Exp*eright=_tmp56;
# 563
{void*_tmp57=eleft->r;void*_stmttmp6=_tmp57;void*_tmp58=_stmttmp6;void*_tmp59;if(*((int*)_tmp58)== 42){_tmp59=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp58)->f1;{struct Cyc_Absyn_Exp*eassn=_tmp59;
# 565
struct _tuple13 _tmp5A=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);struct _tuple13 _stmttmp7=_tmp5A;struct _tuple13 _tmp5B=_stmttmp7;void*_tmp5D;void*_tmp5C;_tmp5C=_tmp5B.f1;_tmp5D=_tmp5B.f2;{void*tm=_tmp5C;void*an=_tmp5D;
loop_inv=(Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn)).f1;
e=eright;
ain=an;
goto _LL33;}}}else{
# 571
goto _LL33;}_LL33:;}
# 573
goto _LL2E;}}else{
# 575
goto _LL2E;}_LL2E:;}
# 577
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);
ain=Cyc_AssnDef_and(ain,loop_inv);{
struct _tuple13 _tmp5E=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple13 _stmttmp8=_tmp5E;struct _tuple13 _tmp5F=_stmttmp8;void*_tmp61;void*_tmp60;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;{void*at=_tmp60;void*af=_tmp61;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 584
({struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0,({
struct _fat_ptr _tmp269=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_tmp1FF.f1=_tmp269;});_tmp1FF;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp1FE;_tmp1FE.tag=0,({struct _fat_ptr _tmp26A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_tmp1FE.f1=_tmp26A;});_tmp1FE;});void*_tmp62[2];_tmp62[0]=& _tmp64,_tmp62[1]=& _tmp65;({unsigned _tmp26C=e->loc;struct _fat_ptr _tmp26B=({const char*_tmp63="can not prove loop invariance %s from %s\n";_tag_fat(_tmp63,sizeof(char),42U);});Cyc_Warn_err(_tmp26C,_tmp26B,_tag_fat(_tmp62,sizeof(void*),2));});});
return(void*)& Cyc_AssnDef_true_assn;}
# 588
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7:
# 590
 goto _LL12;{
# 593
struct Cyc_Absyn_Stmt*_tmp66=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp66;
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_AssnDef_false_assn;}case 6: _LL12: {
# 600
struct Cyc_Absyn_Stmt*_tmp67=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp67;
if(dest == 0)
Cyc_Vcgen_update_return(env,0,ain,0);else{
# 604
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_AssnDef_false_assn;}case 9: _tmp40=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f1;_tmp42=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f2;_tmp43=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f3).f1;_tmp44=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f3).f2;_tmp45=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F)->f4;{struct Cyc_Absyn_Exp*e1=_tmp40;struct Cyc_Absyn_Exp*e2=_tmp41;struct Cyc_Absyn_Stmt*dummy2=_tmp42;struct Cyc_Absyn_Exp*e3=_tmp43;struct Cyc_Absyn_Stmt*dummy3=_tmp44;struct Cyc_Absyn_Stmt*s=_tmp45;
# 607
struct _tuple13 _tmp68=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp9=_tmp68;struct _tuple13 _tmp69=_stmttmp9;void*_tmp6A;_tmp6A=_tmp69.f2;{void*a=_tmp6A;
ain=a;{
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_tmp6B=e2->r;void*_stmttmpA=_tmp6B;void*_tmp6C=_stmttmpA;void*_tmp6E;void*_tmp6D;if(*((int*)_tmp6C)== 7){_tmp6D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6C)->f1;_tmp6E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6C)->f2;{struct Cyc_Absyn_Exp*eleft=_tmp6D;struct Cyc_Absyn_Exp*eright=_tmp6E;
# 614
{void*_tmp6F=eleft->r;void*_stmttmpB=_tmp6F;void*_tmp70=_stmttmpB;void*_tmp71;if(*((int*)_tmp70)== 42){_tmp71=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp70)->f1;{struct Cyc_Absyn_Exp*eassn=_tmp71;
# 616
struct _tuple13 _tmp72=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);struct _tuple13 _stmttmpC=_tmp72;struct _tuple13 _tmp73=_stmttmpC;void*_tmp75;void*_tmp74;_tmp74=_tmp73.f1;_tmp75=_tmp73.f2;{void*tm=_tmp74;void*an=_tmp75;
loop_inv=(Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn)).f1;
e2=eright;
ain=an;
goto _LL46;}}}else{
# 622
goto _LL46;}_LL46:;}
# 624
goto _LL41;}}else{
# 626
goto _LL41;}_LL41:;}
# 628
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);
ain=Cyc_AssnDef_and(ain,loop_inv);{
struct _tuple13 _tmp76=Cyc_Vcgen_vcgen_test(env,e2,ain);struct _tuple13 _stmttmpD=_tmp76;struct _tuple13 _tmp77=_stmttmpD;void*_tmp79;void*_tmp78;_tmp78=_tmp77.f1;_tmp79=_tmp77.f2;{void*at=_tmp78;void*af=_tmp79;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 634
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain)).f2;
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 638
({struct Cyc_String_pa_PrintArg_struct _tmp7C=({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0,({
struct _fat_ptr _tmp26D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_tmp201.f1=_tmp26D;});_tmp201;});struct Cyc_String_pa_PrintArg_struct _tmp7D=({struct Cyc_String_pa_PrintArg_struct _tmp200;_tmp200.tag=0,({struct _fat_ptr _tmp26E=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_tmp200.f1=_tmp26E;});_tmp200;});void*_tmp7A[2];_tmp7A[0]=& _tmp7C,_tmp7A[1]=& _tmp7D;({unsigned _tmp270=e2->loc;struct _fat_ptr _tmp26F=({const char*_tmp7B="can not prove loop invariance %s from %s\n";_tag_fat(_tmp7B,sizeof(char),42U);});Cyc_Warn_err(_tmp270,_tmp26F,_tag_fat(_tmp7A,sizeof(void*),2));});});
return(void*)& Cyc_AssnDef_true_assn;}
# 642
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2))&&
 Cyc_Vcgen_implies(& ain,& loop_inv))
return Cyc_AssnDef_and(loop_inv,af);}}}}}}case 14: _tmp40=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f1;_tmp42=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2).f2;{struct Cyc_Absyn_Stmt*s=_tmp40;struct Cyc_Absyn_Exp*e=_tmp41;struct Cyc_Absyn_Stmt*dummy=_tmp42;
# 649
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_tmp7E=e->r;void*_stmttmpE=_tmp7E;void*_tmp7F=_stmttmpE;void*_tmp81;void*_tmp80;if(*((int*)_tmp7F)== 7){_tmp80=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7F)->f1;_tmp81=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7F)->f2;{struct Cyc_Absyn_Exp*eleft=_tmp80;struct Cyc_Absyn_Exp*eright=_tmp81;
# 654
{void*_tmp82=eleft->r;void*_stmttmpF=_tmp82;void*_tmp83=_stmttmpF;void*_tmp84;if(*((int*)_tmp83)== 42){_tmp84=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp83)->f1;{struct Cyc_Absyn_Exp*eassn=_tmp84;
# 656
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple13 _tmp85=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);struct _tuple13 _stmttmp10=_tmp85;struct _tuple13 _tmp86=_stmttmp10;void*_tmp88;void*_tmp87;_tmp87=_tmp86.f1;_tmp88=_tmp86.f2;{void*tm=_tmp87;void*an=_tmp88;
loop_inv=(Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn)).f1;
e=eright;
ain=an;
goto _LL56;}}}}else{
# 663
goto _LL56;}_LL56:;}
# 665
goto _LL51;}}else{
# 667
goto _LL51;}_LL51:;}
# 669
while(1){
ain=Cyc_AssnDef_and(ain,loop_inv);
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 674
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp203;_tmp203.tag=0,({
struct _fat_ptr _tmp271=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_tmp203.f1=_tmp271;});_tmp203;});struct Cyc_String_pa_PrintArg_struct _tmp8C=({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0,({struct _fat_ptr _tmp272=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_tmp202.f1=_tmp272;});_tmp202;});void*_tmp89[2];_tmp89[0]=& _tmp8B,_tmp89[1]=& _tmp8C;({unsigned _tmp274=e->loc;struct _fat_ptr _tmp273=({const char*_tmp8A="can not prove loop invariance %s from %s\n";_tag_fat(_tmp8A,sizeof(char),42U);});Cyc_Warn_err(_tmp274,_tmp273,_tag_fat(_tmp89,sizeof(void*),2));});});
return(void*)& Cyc_AssnDef_true_assn;}{
# 678
struct _tuple13 _tmp8D=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple13 _stmttmp11=_tmp8D;struct _tuple13 _tmp8E=_stmttmp11;void*_tmp90;void*_tmp8F;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;{void*at=_tmp8F;void*af=_tmp90;
ain=({struct Cyc_Vcgen_Env*_tmp276=env;Cyc_Vcgen_may_widen(_tmp276,({void*_tmp275=*Cyc_Vcgen_get_assn(env,dummy);Cyc_AssnDef_or(_tmp275,ain);}));});
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10: _tmp40=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Exp*e=_tmp40;struct Cyc_List_List*scs=_tmp41;
# 689
struct _tuple13 _tmp91=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp12=_tmp91;struct _tuple13 _tmp92=_stmttmp12;void*_tmp94;void*_tmp93;_tmp93=_tmp92.f1;_tmp94=_tmp92.f2;{void*t=_tmp93;void*a=_tmp94;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2 == 0)
({void*_tmp95=0U;({int(*_tmp278)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp277=({const char*_tmp96="vcgen fallthru";_tag_fat(_tmp96,sizeof(char),15U);});_tmp278(_tmp277,_tag_fat(_tmp95,sizeof(void*),0));});});else{_tmp40=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_List_List*es=_tmp40;struct Cyc_Absyn_Switch_clause*dest=_tmp41;
# 694
struct _tuple14 _tmp97=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp13=_tmp97;struct _tuple14 _tmp98=_stmttmp13;void*_tmp9A;void*_tmp99;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;{struct Cyc_List_List*ts=_tmp99;void*a=_tmp9A;
if(ts != 0){
# 697
struct Cyc_List_List*_tmp9B=({struct Cyc_List_List*(*_tmp279)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls;});_tmp279((({(struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split;})((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(dest->pat_vars))->v)).f1);});struct Cyc_List_List*x=_tmp9B;
# 699
for(1;x != 0;(x=x->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*_tmp9C=(struct Cyc_Absyn_Vardecl*)x->hd;struct Cyc_Absyn_Vardecl*vd=_tmp9C;
if(vd != 0)
a=({struct Cyc_Vcgen_Env*_tmp27C=env;void*_tmp27B=a;void*_tmp27A=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=2,_tmp9D->f1=vd;_tmp9D;});Cyc_Vcgen_do_assign(_tmp27C,_tmp27B,_tmp27A,(void*)((struct Cyc_List_List*)_check_null(ts))->hd);});}}
# 705
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_AssnDef_false_assn;}}}case 12: _tmp40=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Decl*d=_tmp40;struct Cyc_Absyn_Stmt*s1=_tmp41;
# 708
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13: _tmp40=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp40;
# 716
ain=(void*)& Cyc_AssnDef_true_assn;
s=s1;
goto LOOP;}case 8:
 return(void*)& Cyc_AssnDef_false_assn;default: _tmp40=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3F)->f2;{struct Cyc_Absyn_Stmt*s=_tmp40;struct Cyc_List_List*scs=_tmp41;
# 722
void**_tmp9E=(env->shared)->try_assn;void**old_try_assn=_tmp9E;
# 724
({void**_tmp27D=({void**_tmp9F=_cycalloc(sizeof(*_tmp9F));*_tmp9F=(void*)& Cyc_AssnDef_false_assn;_tmp9F;});(env->shared)->try_assn=_tmp27D;});
# 727
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 729
void*_tmpA0=*((void**)_check_null((env->shared)->try_assn));void*new_try_assn=_tmpA0;
(env->shared)->try_assn=old_try_assn;
# 733
return({void*_tmp27E=ain;Cyc_AssnDef_or(_tmp27E,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}};}}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 737
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 741
void*res=(void*)& Cyc_AssnDef_false_assn;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmpA1=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmpA1;
void*_tmpA2=ain;void*a=_tmpA2;
{struct Cyc_List_List*_tmpA3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sc->pat_vars))->v;struct Cyc_List_List*pvs=_tmpA3;for(0;pvs != 0;pvs=pvs->tl){
struct _tuple15*_tmpA4=(struct _tuple15*)pvs->hd;struct _tuple15*_stmttmp14=_tmpA4;struct _tuple15*_tmpA5=_stmttmp14;void*_tmpA7;void*_tmpA6;_tmpA6=_tmpA5->f1;_tmpA7=_tmpA5->f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmpA6;struct Cyc_Absyn_Exp*eopt=_tmpA7;
if(vdopt != 0)
# 752
a=Cyc_Vcgen_kill_var(env,*vdopt,a);}}}
# 756
if(sc->where_clause != 0)
a=(Cyc_Vcgen_vcgen_test(env,(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause),a)).f1;
res=({void*_tmp27F=res;Cyc_AssnDef_or(_tmp27F,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 760
return res;}
# 763
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 766
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpA8=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=2,_tmpAD->f1=vd;_tmpAD;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv=_tmpA8;
a=Cyc_Vcgen_kill_var(env,vd,a);
if(vd->initializer != 0){
struct _tuple13 _tmpA9=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),a);struct _tuple13 _stmttmp15=_tmpA9;struct _tuple13 _tmpAA=_stmttmp15;void*_tmpAC;void*_tmpAB;_tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;{void*t=_tmpAB;void*a1=_tmpAC;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 772
return a;}
# 775
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_tmpAE=d->r;void*_stmttmp16=_tmpAE;void*_tmpAF=_stmttmp16;void*_tmpB2;void*_tmpB1;void*_tmpB0;switch(*((int*)_tmpAF)){case 0: _tmpB0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpAF)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpB0;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2: _tmpB0=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpAF)->f1;_tmpB1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpAF)->f2;_tmpB2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpAF)->f3;{struct Cyc_Absyn_Pat*p=_tmpB0;struct Cyc_Core_Opt*vds=_tmpB1;struct Cyc_Absyn_Exp*e=_tmpB2;
# 779
struct _tuple13 _tmpB3=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple13 _stmttmp17=_tmpB3;struct _tuple13 _tmpB4=_stmttmp17;void*_tmpB5;_tmpB5=_tmpB4.f2;{void*a=_tmpB5;
{struct Cyc_List_List*_tmpB6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds))->v;struct Cyc_List_List*vds=_tmpB6;for(0;vds != 0;vds=vds->tl){
struct _tuple15 _tmpB7=*((struct _tuple15*)vds->hd);struct _tuple15 _stmttmp18=_tmpB7;struct _tuple15 _tmpB8=_stmttmp18;void*_tmpBA;void*_tmpB9;_tmpB9=_tmpB8.f1;_tmpBA=_tmpB8.f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmpB9;struct Cyc_Absyn_Exp*eopt=_tmpBA;
if(vdopt != 0){
if(eopt == 0)a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);else{
# 785
struct _tuple13 _tmpBB=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple13 _stmttmp19=_tmpBB;struct _tuple13 _tmpBC=_stmttmp19;void*_tmpBE;void*_tmpBD;_tmpBD=_tmpBC.f1;_tmpBE=_tmpBC.f2;{void*t=_tmpBD;void*a1=_tmpBE;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpBF=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->tag=2,_tmpC0->f1=*vdopt;_tmpC0;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv=_tmpBF;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}}}}}
# 791
return a;}}case 3: _tmpB0=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpAF)->f1;{struct Cyc_List_List*vds=_tmpB0;
# 793
for(1;vds != 0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4: _tmpB0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpAF)->f2;_tmpB1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpAF)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmpB0;struct Cyc_Absyn_Exp*eopt=_tmpB1;
# 797
if(eopt != 0){
struct _tuple13 _tmpC1=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple13 _stmttmp1A=_tmpC1;struct _tuple13 _tmpC2=_stmttmp1A;void*_tmpC4;void*_tmpC3;_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;{void*t=_tmpC3;void*a1=_tmpC4;
a=a1;}}
# 802
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default:
 return a;};}
# 810
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpC5=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*t1=_tmpC5;
void*_tmpC6=Cyc_Absyn_compress((void*)_check_null(e2->topt));void*t2=_tmpC6;
struct _tuple13 _tmpC7=({struct _tuple13 _tmp204;_tmp204.f1=t1,_tmp204.f2=t2;_tmp204;});struct _tuple13 _stmttmp1B=_tmpC7;struct _tuple13 _tmpC8=_stmttmp1B;if(*((int*)_tmpC8.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC8.f1)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC8.f1)->f1)->f1 == Cyc_Absyn_Unsigned)
return 1;else{if(*((int*)_tmpC8.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC8.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC8.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_tmpC8.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC8.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC8.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_tmpC8.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC8.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC8.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3:
 return 1;}else{if(*((int*)_tmpC8.f1)== 3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_tmpC8.f1)== 3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_tmpC8.f1)== 3){if(*((int*)_tmpC8.f2)== 3)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}}};}
# 826
static struct _tuple13 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmpC9=e->r;void*_stmttmp1C=_tmpC9;void*_tmpCA=_stmttmp1C;enum Cyc_Absyn_Primop _tmpCB;void*_tmpCE;void*_tmpCD;void*_tmpCC;switch(*((int*)_tmpCA)){case 6: _tmpCC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_tmpCE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f3;{struct Cyc_Absyn_Exp*e1=_tmpCC;struct Cyc_Absyn_Exp*e2=_tmpCD;struct Cyc_Absyn_Exp*e3=_tmpCE;
# 829
struct _tuple13 _tmpCF=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp1D=_tmpCF;struct _tuple13 _tmpD0=_stmttmp1D;void*_tmpD2;void*_tmpD1;_tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;{void*a1t=_tmpD1;void*a1f=_tmpD2;
struct _tuple13 _tmpD3=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple13 _stmttmp1E=_tmpD3;struct _tuple13 _tmpD4=_stmttmp1E;void*_tmpD6;void*_tmpD5;_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;{void*a2t=_tmpD5;void*a2f=_tmpD6;
struct _tuple13 _tmpD7=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct _tuple13 _stmttmp1F=_tmpD7;struct _tuple13 _tmpD8=_stmttmp1F;void*_tmpDA;void*_tmpD9;_tmpD9=_tmpD8.f1;_tmpDA=_tmpD8.f2;{void*a3t=_tmpD9;void*a3f=_tmpDA;
return({struct _tuple13 _tmp205;({void*_tmp281=Cyc_AssnDef_or(a2t,a3t);_tmp205.f1=_tmp281;}),({void*_tmp280=Cyc_AssnDef_or(a2f,a3f);_tmp205.f2=_tmp280;});_tmp205;});}}}}case 7: _tmpCC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;{struct Cyc_Absyn_Exp*e1=_tmpCC;struct Cyc_Absyn_Exp*e2=_tmpCD;
# 834
struct _tuple13 _tmpDB=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp20=_tmpDB;struct _tuple13 _tmpDC=_stmttmp20;void*_tmpDE;void*_tmpDD;_tmpDD=_tmpDC.f1;_tmpDE=_tmpDC.f2;{void*a1t=_tmpDD;void*a1f=_tmpDE;
struct _tuple13 _tmpDF=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple13 _stmttmp21=_tmpDF;struct _tuple13 _tmpE0=_stmttmp21;void*_tmpE2;void*_tmpE1;_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;{void*a2t=_tmpE1;void*a2f=_tmpE2;
return({struct _tuple13 _tmp206;_tmp206.f1=a2t,({void*_tmp282=Cyc_AssnDef_or(a1f,a2f);_tmp206.f2=_tmp282;});_tmp206;});}}}case 8: _tmpCC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;{struct Cyc_Absyn_Exp*e1=_tmpCC;struct Cyc_Absyn_Exp*e2=_tmpCD;
# 838
struct _tuple13 _tmpE3=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp22=_tmpE3;struct _tuple13 _tmpE4=_stmttmp22;void*_tmpE6;void*_tmpE5;_tmpE5=_tmpE4.f1;_tmpE6=_tmpE4.f2;{void*a1t=_tmpE5;void*a1f=_tmpE6;
struct _tuple13 _tmpE7=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct _tuple13 _stmttmp23=_tmpE7;struct _tuple13 _tmpE8=_stmttmp23;void*_tmpEA;void*_tmpE9;_tmpE9=_tmpE8.f1;_tmpEA=_tmpE8.f2;{void*a2t=_tmpE9;void*a2f=_tmpEA;
return({struct _tuple13 _tmp207;({void*_tmp283=Cyc_AssnDef_or(a1t,a2t);_tmp207.f1=_tmp283;}),_tmp207.f2=a2f;_tmp207;});}}}case 9: _tmpCC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;{struct Cyc_Absyn_Exp*e1=_tmpCC;struct Cyc_Absyn_Exp*e2=_tmpCD;
# 842
struct _tuple13 _tmpEB=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp24=_tmpEB;struct _tuple13 _tmpEC=_stmttmp24;void*_tmpED;_tmpED=_tmpEC.f2;{void*ain=_tmpED;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2 != 0){_tmpCC=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2)->hd;{struct Cyc_Absyn_Exp*e1=_tmpCC;
# 845
struct _tuple13 _tmpEE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp25=_tmpEE;struct _tuple13 _tmpEF=_stmttmp25;void*_tmpF1;void*_tmpF0;_tmpF0=_tmpEF.f1;_tmpF1=_tmpEF.f2;{void*at=_tmpF0;void*af=_tmpF1;
return({struct _tuple13 _tmp208;_tmp208.f1=af,_tmp208.f2=at;_tmp208;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2)->tl)->tl == 0){_tmpCB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpCC=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2)->hd;_tmpCD=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmpCB;struct Cyc_Absyn_Exp*e1=_tmpCC;struct Cyc_Absyn_Exp*e2=_tmpCD;
# 848
struct _tuple13 _tmpF2=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp26=_tmpF2;struct _tuple13 _tmpF3=_stmttmp26;void*_tmpF5;void*_tmpF4;_tmpF4=_tmpF3.f1;_tmpF5=_tmpF3.f2;{void*t1=_tmpF4;void*a1=_tmpF5;
struct _tuple13 _tmpF6=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp27=_tmpF6;struct _tuple13 _tmpF7=_stmttmp27;void*_tmpF9;void*_tmpF8;_tmpF8=_tmpF7.f1;_tmpF9=_tmpF7.f2;{void*t2=_tmpF8;void*a2=_tmpF9;
void*_tmpFA=a2;void*at=_tmpFA;
void*_tmpFB=a2;void*af=_tmpFB;
{enum Cyc_Absyn_Primop _tmpFC=p;switch((int)_tmpFC){case Cyc_Absyn_Eq:
# 857
 at=({void*_tmp284=at;Cyc_AssnDef_and(_tmp284,Cyc_AssnDef_eq(t1,t2));});
af=({void*_tmp285=at;Cyc_AssnDef_and(_tmp285,Cyc_AssnDef_neq(t1,t2));});
goto _LL30;case Cyc_Absyn_Neq:
# 861
 at=({void*_tmp286=af;Cyc_AssnDef_and(_tmp286,Cyc_AssnDef_neq(t1,t2));});
af=({void*_tmp287=af;Cyc_AssnDef_and(_tmp287,Cyc_AssnDef_eq(t1,t2));});
goto _LL30;case Cyc_Absyn_Lt:
# 865
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp288=at;Cyc_AssnDef_and(_tmp288,Cyc_AssnDef_ult(t1,t2));});af=({void*_tmp289=af;Cyc_AssnDef_and(_tmp289,Cyc_AssnDef_ulte(t2,t1));});}else{
# 868
at=({void*_tmp28A=at;Cyc_AssnDef_and(_tmp28A,Cyc_AssnDef_slt(t1,t2));});af=({void*_tmp28B=af;Cyc_AssnDef_and(_tmp28B,Cyc_AssnDef_slte(t2,t1));});}
# 870
goto _LL30;case Cyc_Absyn_Lte:
# 872
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp28C=at;Cyc_AssnDef_and(_tmp28C,Cyc_AssnDef_ulte(t1,t2));});af=({void*_tmp28D=af;Cyc_AssnDef_and(_tmp28D,Cyc_AssnDef_ult(t2,t1));});}else{
# 875
at=({void*_tmp28E=at;Cyc_AssnDef_and(_tmp28E,Cyc_AssnDef_slte(t1,t2));});af=({void*_tmp28F=af;Cyc_AssnDef_and(_tmp28F,Cyc_AssnDef_slt(t2,t1));});}
# 877
goto _LL30;case Cyc_Absyn_Gt:
# 879
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp290=at;Cyc_AssnDef_and(_tmp290,Cyc_AssnDef_ult(t2,t1));});af=({void*_tmp291=af;Cyc_AssnDef_and(_tmp291,Cyc_AssnDef_ulte(t1,t2));});}else{
# 882
at=({void*_tmp292=at;Cyc_AssnDef_and(_tmp292,Cyc_AssnDef_slt(t2,t1));});af=({void*_tmp293=af;Cyc_AssnDef_and(_tmp293,Cyc_AssnDef_slte(t1,t2));});}
# 884
goto _LL30;case Cyc_Absyn_Gte:
# 886
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp294=at;Cyc_AssnDef_and(_tmp294,Cyc_AssnDef_ulte(t2,t1));});af=({void*_tmp295=af;Cyc_AssnDef_and(_tmp295,Cyc_AssnDef_ult(t1,t2));});}else{
# 889
at=({void*_tmp296=at;Cyc_AssnDef_and(_tmp296,Cyc_AssnDef_slte(t2,t1));});af=({void*_tmp297=af;Cyc_AssnDef_and(_tmp297,Cyc_AssnDef_slt(t1,t2));});}
# 891
goto _LL30;default:
 goto _LL30;}_LL30:;}
# 894
return({struct _tuple13 _tmp209;_tmp209.f1=at,_tmp209.f2=af;_tmp209;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: {
# 896
struct _tuple13 _tmpFD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp28=_tmpFD;struct _tuple13 _tmpFE=_stmttmp28;void*_tmp100;void*_tmpFF;_tmpFF=_tmpFE.f1;_tmp100=_tmpFE.f2;{void*t=_tmpFF;void*aout=_tmp100;
return({struct _tuple13 _tmp20A;({void*_tmp29D=({void*_tmp29C=aout;Cyc_AssnDef_and(_tmp29C,({void*_tmp29B=Cyc_AssnDef_one();Cyc_AssnDef_ulte(_tmp29B,t);}));});_tmp20A.f1=_tmp29D;}),({void*_tmp29A=({void*_tmp299=aout;Cyc_AssnDef_and(_tmp299,({void*_tmp298=t;Cyc_AssnDef_eq(_tmp298,Cyc_AssnDef_zero());}));});_tmp20A.f2=_tmp29A;});_tmp20A;});}}};}
# 901
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(1;es != 0;es=es->tl){
a=(Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a)).f2;}
return a;}struct _tuple16{struct Cyc_List_List**f1;void*f2;};
# 907
static struct _tuple16 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(1;es != 0;es=es->tl){
struct _tuple13 _tmp101=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);struct _tuple13 _stmttmp29=_tmp101;struct _tuple13 _tmp102=_stmttmp29;void*_tmp104;void*_tmp103;_tmp103=_tmp102.f1;_tmp104=_tmp102.f2;{void*t=_tmp103;void*aout=_tmp104;
a=aout;
if(valid && t != 0)
res=({struct Cyc_List_List*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->hd=t,_tmp105->tl=res;_tmp105;});else{
# 916
valid=0;}}}
# 919
return({struct _tuple16 _tmp20B;valid?({struct Cyc_List_List**_tmp29F=({struct Cyc_List_List**_tmp106=_cycalloc(sizeof(*_tmp106));({struct Cyc_List_List*_tmp29E=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res);*_tmp106=_tmp29E;});_tmp106;});_tmp20B.f1=_tmp29F;}):(_tmp20B.f1=0),_tmp20B.f2=a;_tmp20B;});}
# 922
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple13 _tmp107=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp2A=_tmp107;struct _tuple13 _tmp108=_stmttmp2A;void*_tmp10A;void*_tmp109;_tmp109=_tmp108.f1;_tmp10A=_tmp108.f2;{void*topt=_tmp109;void*a=_tmp10A;
return({struct _tuple13 _tmp20C;topt == 0?({void*_tmp2A0=Cyc_AssnDef_fresh_var(e->topt);_tmp20C.f1=_tmp2A0;}):(_tmp20C.f1=topt),_tmp20C.f2=a;_tmp20C;});}}
# 927
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(1;es != 0;es=es->tl){
struct _tuple13 _tmp10B=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);struct _tuple13 _stmttmp2B=_tmp10B;struct _tuple13 _tmp10C=_stmttmp2B;void*_tmp10E;void*_tmp10D;_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;{void*t=_tmp10D;void*a=_tmp10E;
ain=a;
res=({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->hd=t,_tmp10F->tl=res;_tmp10F;});}}
# 934
return({struct _tuple14 _tmp20D;({struct Cyc_List_List*_tmp2A1=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res);_tmp20D.f1=_tmp2A1;}),_tmp20D.f2=ain;_tmp20D;});}
# 937
static struct Cyc_Absyn_Vardecl**Cyc_Vcgen_bind2vardecl(void*b){
void*_tmp110=b;void*_tmp111;switch(*((int*)_tmp110)){case 1: _tmp111=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp110)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp111;
return({struct Cyc_Absyn_Vardecl**_tmp112=_cycalloc(sizeof(*_tmp112));*_tmp112=vd;_tmp112;});}case 2: _tmp111=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp110)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp111;
# 941
if(fd->fn_vardecl == 0)return 0;else{
return({struct Cyc_Absyn_Vardecl**_tmp113=_cycalloc(sizeof(*_tmp113));*_tmp113=(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);_tmp113;});}}case 0:
({void*_tmp114=0U;({int(*_tmp2A3)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp2A2=({const char*_tmp115="vcgen bind2vardecl";_tag_fat(_tmp115,sizeof(char),19U);});_tmp2A3(_tmp2A2,_tag_fat(_tmp114,sizeof(void*),0));});});case 3: _tmp111=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp110)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp111;
return({struct Cyc_Absyn_Vardecl**_tmp116=_cycalloc(sizeof(*_tmp116));*_tmp116=vd;_tmp116;});}case 4: _tmp111=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp110)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp111;
return({struct Cyc_Absyn_Vardecl**_tmp117=_cycalloc(sizeof(*_tmp117));*_tmp117=vd;_tmp117;});}default: _tmp111=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp110)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp111;
return({struct Cyc_Absyn_Vardecl**_tmp118=_cycalloc(sizeof(*_tmp118));*_tmp118=vd;_tmp118;});}};}
# 950
static struct _tuple13 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp119=e->r;void*_stmttmp2C=_tmp119;void*_tmp11A=_stmttmp2C;void*_tmp11B;if(*((int*)_tmp11A)== 1){_tmp11B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11A)->f1;{void*b=_tmp11B;
# 953
struct Cyc_Absyn_Vardecl**_tmp11C=Cyc_Vcgen_bind2vardecl(b);struct Cyc_Absyn_Vardecl**newb=_tmp11C;
if(newb == 0)return({struct _tuple13 _tmp20E;_tmp20E.f1=0,_tmp20E.f2=ain;_tmp20E;});else{
return({struct _tuple13 _tmp20F;({void*_tmp2A4=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=2,_tmp11D->f1=*((struct Cyc_Absyn_Vardecl**)_check_null(Cyc_Vcgen_bind2vardecl(b)));_tmp11D;});_tmp20F.f1=_tmp2A4;}),_tmp20F.f2=ain;_tmp20F;});}}}else{
return({struct _tuple13 _tmp210;_tmp210.f1=0,({void*_tmp2A5=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp210.f2=_tmp2A5;});_tmp210;});};}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Vardecl*f4;};
# 962
static struct _tuple17 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple17 _tmp211;_tmp211.f1=fi->args,_tmp211.f2=fi->requires_clause,_tmp211.f3=fi->ensures_clause,_tmp211.f4=fi->return_value;_tmp211;});}
# 966
static struct _tuple17 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt != 0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_tmp11E=t;void*_tmp11F;if(*((int*)_tmp11E)== 5){_tmp11F=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11E)->f1;{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_tmp11F;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{
goto _LL0;}_LL0:;}
# 974
return({struct _tuple17 _tmp212;_tmp212.f1=0,_tmp212.f2=0,_tmp212.f3=0,_tmp212.f4=0;_tmp212;});}
# 977
static void Cyc_Vcgen_check_requires(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*args,void*a,struct Cyc_Absyn_Exp*reqopt){
# 980
if(reqopt != 0){
struct Cyc_Absyn_Exp*req=reqopt;
# 984
for(1;args != 0 && inputs != 0;(args=args->tl,inputs=inputs->tl)){
struct _tuple9*_tmp120=(struct _tuple9*)args->hd;struct _tuple9*_stmttmp2D=_tmp120;struct _tuple9*_tmp121=_stmttmp2D;void*_tmp124;struct Cyc_Absyn_Tqual _tmp123;void*_tmp122;_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;_tmp124=_tmp121->f3;{struct _fat_ptr*vopt=_tmp122;struct Cyc_Absyn_Tqual tq=_tmp123;void*t=_tmp124;
if(vopt != 0){
# 988
struct _tuple1*qv=({struct _tuple1*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->f1=Cyc_Absyn_Loc_n,_tmp128->f2=vopt;_tmp128;});
struct Cyc_Absyn_Vardecl*_tmp125=({struct Cyc_Absyn_Vardecl*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->sc=2U,_tmp127->name=qv,_tmp127->varloc=0U,_tmp127->tq=tq,_tmp127->type=t,_tmp127->initializer=0,_tmp127->rgn=0,_tmp127->attributes=0,_tmp127->escapes=0,_tmp127->is_proto=0;_tmp127;});struct Cyc_Absyn_Vardecl*vd=_tmp125;
# 994
a=({void*_tmp2A7=a;Cyc_AssnDef_and(_tmp2A7,({void*_tmp2A6=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=2,_tmp126->f1=vd;_tmp126;});Cyc_AssnDef_eq(_tmp2A6,(void*)inputs->hd);}));});}}}{
# 998
struct _tuple13 _tmp129=Cyc_Vcgen_vcgen_test(env,reqopt,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp2E=_tmp129;struct _tuple13 _tmp12A=_stmttmp2E;void*_tmp12B;_tmp12B=_tmp12A.f1;{void*at=_tmp12B;
if(!Cyc_Vcgen_implies(& a,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp12E=({struct Cyc_String_pa_PrintArg_struct _tmp214;_tmp214.tag=0,({
struct _fat_ptr _tmp2A8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp214.f1=_tmp2A8;});_tmp214;});struct Cyc_String_pa_PrintArg_struct _tmp12F=({struct Cyc_String_pa_PrintArg_struct _tmp213;_tmp213.tag=0,({struct _fat_ptr _tmp2A9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(a)));_tmp213.f1=_tmp2A9;});_tmp213;});void*_tmp12C[2];_tmp12C[0]=& _tmp12E,_tmp12C[1]=& _tmp12F;({unsigned _tmp2AB=req->loc;struct _fat_ptr _tmp2AA=({const char*_tmp12D="cannot prove @requires %s from %s";_tag_fat(_tmp12D,sizeof(char),34U);});Cyc_Warn_err(_tmp2AB,_tmp2AA,_tag_fat(_tmp12C,sizeof(void*),2));});});}}}}
# 1005
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}
# 1009
static int Cyc_Vcgen_is_unsigned_int_exp(struct Cyc_Absyn_Exp*e){
void*t=e->topt;
if(t == 0)return 0;{
void*_tmp130=t;enum Cyc_Absyn_Size_of _tmp131;if(_tmp130 != 0){if(*((int*)_tmp130)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1)->f1 == Cyc_Absyn_Unsigned){_tmp131=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1)->f2;{enum Cyc_Absyn_Size_of sz=_tmp131;
# 1015
if((int)sz == 3 ||(int)sz == 2)
return 1;else{
return 0;}}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1022
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e)){
# 1025
{void*_tmp132=e->r;void*_stmttmp2F=_tmp132;void*_tmp133=_stmttmp2F;struct _fat_ptr _tmp134;if(*((int*)_tmp133)== 2){_tmp134=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp133)->f1;{struct _fat_ptr s=_tmp134;
# 1027
if(({struct _fat_ptr _tmp2AC=(struct _fat_ptr)s;Cyc_strcmp(_tmp2AC,({const char*_tmp135="print_assn";_tag_fat(_tmp135,sizeof(char),11U);}));})== 0){
struct _fat_ptr _tmp136=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp136;
({struct Cyc_String_pa_PrintArg_struct _tmp139=({struct Cyc_String_pa_PrintArg_struct _tmp216;_tmp216.tag=0,_tmp216.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp216;});struct Cyc_String_pa_PrintArg_struct _tmp13A=({struct Cyc_String_pa_PrintArg_struct _tmp215;_tmp215.tag=0,({struct _fat_ptr _tmp2AD=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_tmp215.f1=_tmp2AD;});_tmp215;});void*_tmp137[2];_tmp137[0]=& _tmp139,_tmp137[1]=& _tmp13A;({struct Cyc___cycFILE*_tmp2AF=Cyc_stderr;struct _fat_ptr _tmp2AE=({const char*_tmp138="%s: current assn is %s\n";_tag_fat(_tmp138,sizeof(char),24U);});Cyc_fprintf(_tmp2AF,_tmp2AE,_tag_fat(_tmp137,sizeof(void*),2));});});}
# 1031
if(({struct _fat_ptr _tmp2B0=(struct _fat_ptr)s;Cyc_strcmp(_tmp2B0,({const char*_tmp13B="all_paths";_tag_fat(_tmp13B,sizeof(char),10U);}));})== 0)
(env->shared)->widen_paths=0;
# 1034
if(({struct _fat_ptr _tmp2B1=(struct _fat_ptr)s;Cyc_strcmp(_tmp2B1,({const char*_tmp13C="one_path";_tag_fat(_tmp13C,sizeof(char),9U);}));})== 0)
(env->shared)->widen_paths=1;
# 1037
return({struct _tuple13 _tmp217;({void*_tmp2B2=Cyc_AssnDef_zero();_tmp217.f1=_tmp2B2;}),_tmp217.f2=ain;_tmp217;});}}else{
goto _LL0;}_LL0:;}{
# 1040
struct _tuple12 _tmp13D=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp30=_tmp13D;struct _tuple12 _tmp13E=_stmttmp30;int _tmp140;unsigned _tmp13F;_tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;{unsigned cn=_tmp13F;int known=_tmp140;
if(known)return({struct _tuple13 _tmp218;({void*_tmp2B3=(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->tag=0,_tmp141->f1=cn;_tmp141;});_tmp218.f1=_tmp2B3;}),_tmp218.f2=ain;_tmp218;});}}}{
# 1043
void*_tmp142=e->r;void*_stmttmp31=_tmp142;void*_tmp143=_stmttmp31;struct _fat_ptr _tmp145;struct Cyc_Absyn_MallocInfo _tmp147;enum Cyc_Absyn_Coercion _tmp149;enum Cyc_Absyn_Incrementor _tmp14A;void*_tmp148;void*_tmp146;enum Cyc_Absyn_Primop _tmp14B;void*_tmp144;union Cyc_Absyn_Cnst _tmp14C;switch(*((int*)_tmp143)){case 0: _tmp14C=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{union Cyc_Absyn_Cnst c=_tmp14C;
# 1046
return({struct _tuple13 _tmp219;({void*_tmp2B4=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->tag=1,_tmp14D->f1=e;_tmp14D;});_tmp219.f1=_tmp2B4;}),_tmp219.f2=ain;_tmp219;});}case 1: _tmp144=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{void*b=_tmp144;
# 1048
struct Cyc_Absyn_Vardecl**_tmp14E=Cyc_Vcgen_bind2vardecl(b);struct Cyc_Absyn_Vardecl**newb=_tmp14E;
if(newb == 0)return({struct _tuple13 _tmp21A;_tmp21A.f1=0,_tmp21A.f2=ain;_tmp21A;});else{
return({struct _tuple13 _tmp21B;({void*_tmp2B5=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->tag=2,_tmp14F->f1=*((struct Cyc_Absyn_Vardecl**)_check_null(Cyc_Vcgen_bind2vardecl(b)));_tmp14F;});_tmp21B.f1=_tmp2B5;}),_tmp21B.f2=ain;_tmp21B;});}}case 3: _tmp14B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp144=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{enum Cyc_Absyn_Primop p=_tmp14B;struct Cyc_List_List*es=_tmp144;
# 1052
struct _tuple14 _tmp150=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp32=_tmp150;struct _tuple14 _tmp151=_stmttmp32;void*_tmp153;void*_tmp152;_tmp152=_tmp151.f1;_tmp153=_tmp151.f2;{struct Cyc_List_List*ts=_tmp152;void*aout=_tmp153;
void*t=0;
if(ts != 0)t=Cyc_AssnDef_primop(p,ts);
# 1057
if((int)p == 4 && Cyc_Vcgen_is_unsigned_int_exp(e)){
# 1059
struct Cyc_List_List*_tmp154=ts;void*_tmp156;void*_tmp155;if(_tmp154 != 0){if(((struct Cyc_List_List*)_tmp154)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp154)->tl)->tl == 0){_tmp155=(void*)_tmp154->hd;_tmp156=(void*)(_tmp154->tl)->hd;{void*t1=_tmp155;void*t2=_tmp156;
# 1061
aout=({void*_tmp2B6=aout;Cyc_AssnDef_and(_tmp2B6,Cyc_AssnDef_ult(t,t2));});
goto _LL62;}}else{goto _LL65;}}else{goto _LL65;}}else{_LL65:
 goto _LL62;}_LL62:;}
# 1066
return({struct _tuple13 _tmp21C;_tmp21C.f1=t,_tmp21C.f2=aout;_tmp21C;});}}case 4: _tmp144=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp143)->f2;_tmp148=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp143)->f3;{struct Cyc_Absyn_Exp*e1=_tmp144;struct Cyc_Core_Opt*popt=_tmp146;struct Cyc_Absyn_Exp*e2=_tmp148;
# 1068
struct _tuple13 _tmp157=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple13 _stmttmp33=_tmp157;struct _tuple13 _tmp158=_stmttmp33;void*_tmp15A;void*_tmp159;_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;{void*lt=_tmp159;void*a1=_tmp15A;
struct _tuple13 _tmp15B=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp34=_tmp15B;struct _tuple13 _tmp15C=_stmttmp34;void*_tmp15E;void*_tmp15D;_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;{void*t2=_tmp15D;void*a2=_tmp15E;
if(popt != 0){
void*_tmp15F=Cyc_Vcgen_deref_lterm(env,lt,a2);void*t1=_tmp15F;
if(t1 == 0 || t2 == 0)
t2=0;else{
# 1075
t2=({enum Cyc_Absyn_Primop _tmp2B8=(enum Cyc_Absyn_Primop)popt->v;Cyc_AssnDef_primop(_tmp2B8,({void*_tmp160[2];_tmp160[0]=t1,_tmp160[1]=t2;({struct Cyc_List_List*(*_tmp2B7)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2B7(_tag_fat(_tmp160,sizeof(void*),2));});}));});}}{
# 1077
void*_tmp161=Cyc_Vcgen_do_assign(env,a2,lt,t2);void*a=_tmp161;
return({struct _tuple13 _tmp21D;_tmp21D.f1=lt,_tmp21D.f2=a;_tmp21D;});}}}}case 5: _tmp144=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp14A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{struct Cyc_Absyn_Exp*e=_tmp144;enum Cyc_Absyn_Incrementor i=_tmp14A;
# 1080
struct _tuple13 _tmp162=Cyc_Vcgen_vcgen_lexp(env,e,ain);struct _tuple13 _stmttmp35=_tmp162;struct _tuple13 _tmp163=_stmttmp35;void*_tmp165;void*_tmp164;_tmp164=_tmp163.f1;_tmp165=_tmp163.f2;{void*lt=_tmp164;void*a1=_tmp165;
void*_tmp166=Cyc_Vcgen_deref_lterm(env,lt,a1);void*t1=_tmp166;
void*res=0;
void*assn=0;
{enum Cyc_Absyn_Incrementor _tmp167=i;switch((int)_tmp167){case Cyc_Absyn_PreInc:
# 1086
 if(t1 != 0)assn=Cyc_AssnDef_primop(0U,({void*_tmp168[2];_tmp168[0]=t1,({void*_tmp2B9=Cyc_AssnDef_one();_tmp168[1]=_tmp2B9;});({struct Cyc_List_List*(*_tmp2BA)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2BA(_tag_fat(_tmp168,sizeof(void*),2));});}));
res=assn;
goto _LL70;case Cyc_Absyn_PostInc:
# 1090
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(0U,({void*_tmp169[2];_tmp169[0]=t1,({void*_tmp2BB=Cyc_AssnDef_one();_tmp169[1]=_tmp2BB;});({struct Cyc_List_List*(*_tmp2BC)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2BC(_tag_fat(_tmp169,sizeof(void*),2));});}));
goto _LL70;case Cyc_Absyn_PreDec:
# 1094
 if(t1 != 0)assn=Cyc_AssnDef_primop(2U,({void*_tmp16A[2];_tmp16A[0]=t1,({void*_tmp2BD=Cyc_AssnDef_one();_tmp16A[1]=_tmp2BD;});({struct Cyc_List_List*(*_tmp2BE)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2BE(_tag_fat(_tmp16A,sizeof(void*),2));});}));
res=assn;
goto _LL70;case Cyc_Absyn_PostDec:
# 1098
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(2U,({void*_tmp16B[2];_tmp16B[0]=t1,({void*_tmp2BF=Cyc_AssnDef_one();_tmp16B[1]=_tmp2BF;});({struct Cyc_List_List*(*_tmp2C0)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2C0(_tag_fat(_tmp16B,sizeof(void*),2));});}));
goto _LL70;default:
({void*_tmp16C=0U;({int(*_tmp2C2)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp2C1=({const char*_tmp16D="vcgen vcgen_rexp increment_e";_tag_fat(_tmp16D,sizeof(char),29U);});_tmp2C2(_tmp2C1,_tag_fat(_tmp16C,sizeof(void*),0));});});}_LL70:;}{
# 1103
void*_tmp16E=Cyc_Vcgen_do_assign(env,a1,lt,assn);void*a=_tmp16E;
return({struct _tuple13 _tmp21E;_tmp21E.f1=res,_tmp21E.f2=a;_tmp21E;});}}}case 6: _tmp144=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp143)->f2;_tmp148=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp143)->f3;{struct Cyc_Absyn_Exp*e1=_tmp144;struct Cyc_Absyn_Exp*e2=_tmp146;struct Cyc_Absyn_Exp*e3=_tmp148;
# 1106
struct _tuple13 _tmp16F=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp36=_tmp16F;struct _tuple13 _tmp170=_stmttmp36;void*_tmp172;void*_tmp171;_tmp171=_tmp170.f1;_tmp172=_tmp170.f2;{void*at=_tmp171;void*af=_tmp172;
struct _tuple13 _tmp173=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp37=_tmp173;struct _tuple13 _tmp174=_stmttmp37;void*_tmp176;void*_tmp175;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{void*t1=_tmp175;void*at=_tmp176;
struct _tuple13 _tmp177=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);struct _tuple13 _stmttmp38=_tmp177;struct _tuple13 _tmp178=_stmttmp38;void*_tmp17A;void*_tmp179;_tmp179=_tmp178.f1;_tmp17A=_tmp178.f2;{void*t2=_tmp179;void*af=_tmp17A;
if(t1 != 0 && t2 != 0){
void*_tmp17B=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp17B;
at=({void*_tmp2C3=at;Cyc_AssnDef_and(_tmp2C3,Cyc_AssnDef_eq(v,t1));});
af=({void*_tmp2C4=af;Cyc_AssnDef_and(_tmp2C4,Cyc_AssnDef_eq(v,t2));});
return({struct _tuple13 _tmp21F;_tmp21F.f1=v,({void*_tmp2C6=({struct Cyc_Vcgen_Env*_tmp2C5=env;Cyc_Vcgen_may_widen(_tmp2C5,Cyc_AssnDef_or(at,af));});_tmp21F.f2=_tmp2C6;});_tmp21F;});}else{
# 1115
return({struct _tuple13 _tmp220;_tmp220.f1=0,({void*_tmp2C8=({struct Cyc_Vcgen_Env*_tmp2C7=env;Cyc_Vcgen_may_widen(_tmp2C7,Cyc_AssnDef_or(at,af));});_tmp220.f2=_tmp2C8;});_tmp220;});}}}}}case 7: _tmp144=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{struct Cyc_Absyn_Exp*e1=_tmp144;struct Cyc_Absyn_Exp*e2=_tmp146;
# 1117
struct _tuple13 _tmp17C=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp39=_tmp17C;struct _tuple13 _tmp17D=_stmttmp39;void*_tmp17F;void*_tmp17E;_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;{void*at=_tmp17E;void*af=_tmp17F;
struct _tuple13 _tmp180=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp3A=_tmp180;struct _tuple13 _tmp181=_stmttmp3A;void*_tmp183;void*_tmp182;_tmp182=_tmp181.f1;_tmp183=_tmp181.f2;{void*t=_tmp182;void*at=_tmp183;
void*_tmp184=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp184;
at=({void*_tmp2C9=at;Cyc_AssnDef_and(_tmp2C9,Cyc_AssnDef_eq(v,t));});
af=({void*_tmp2CB=af;Cyc_AssnDef_and(_tmp2CB,({void*_tmp2CA=v;Cyc_AssnDef_eq(_tmp2CA,Cyc_AssnDef_zero());}));});
return({struct _tuple13 _tmp221;_tmp221.f1=v,({void*_tmp2CD=({struct Cyc_Vcgen_Env*_tmp2CC=env;Cyc_Vcgen_may_widen(_tmp2CC,Cyc_AssnDef_or(at,af));});_tmp221.f2=_tmp2CD;});_tmp221;});}}}case 8: _tmp144=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{struct Cyc_Absyn_Exp*e1=_tmp144;struct Cyc_Absyn_Exp*e2=_tmp146;
# 1124
struct _tuple13 _tmp185=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp3B=_tmp185;struct _tuple13 _tmp186=_stmttmp3B;void*_tmp188;void*_tmp187;_tmp187=_tmp186.f1;_tmp188=_tmp186.f2;{void*at=_tmp187;void*af=_tmp188;
struct _tuple13 _tmp189=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);struct _tuple13 _stmttmp3C=_tmp189;struct _tuple13 _tmp18A=_stmttmp3C;void*_tmp18C;void*_tmp18B;_tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;{void*t=_tmp18B;void*af=_tmp18C;
void*_tmp18D=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp18D;
at=({void*_tmp2CF=at;Cyc_AssnDef_and(_tmp2CF,({void*_tmp2CE=v;Cyc_AssnDef_eq(_tmp2CE,Cyc_AssnDef_one());}));});
af=({void*_tmp2D0=af;Cyc_AssnDef_and(_tmp2D0,Cyc_AssnDef_eq(v,t));});
return({struct _tuple13 _tmp222;_tmp222.f1=v,({void*_tmp2D2=({struct Cyc_Vcgen_Env*_tmp2D1=env;Cyc_Vcgen_may_widen(_tmp2D1,Cyc_AssnDef_or(at,af));});_tmp222.f2=_tmp2D2;});_tmp222;});}}}case 9: _tmp144=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{struct Cyc_Absyn_Exp*e1=_tmp144;struct Cyc_Absyn_Exp*e2=_tmp146;
# 1131
struct _tuple13 _tmp18E=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp3D=_tmp18E;struct _tuple13 _tmp18F=_stmttmp3D;void*_tmp190;_tmp190=_tmp18F.f2;{void*a=_tmp190;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10: _tmp144=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{struct Cyc_Absyn_Exp*e=_tmp144;struct Cyc_List_List*es=_tmp146;
# 1134
struct _tuple14 _tmp191=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp3E=_tmp191;struct _tuple14 _tmp192=_stmttmp3E;void*_tmp194;void*_tmp193;_tmp193=_tmp192.f1;_tmp194=_tmp192.f2;{struct Cyc_List_List*ts=_tmp193;void*a=_tmp194;
struct _tuple13 _tmp195=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple13 _stmttmp3F=_tmp195;struct _tuple13 _tmp196=_stmttmp3F;void*_tmp197;_tmp197=_tmp196.f2;{void*a2=_tmp197;
# 1137
struct _tuple17 _tmp198=Cyc_Vcgen_get_requires_and_ensures(e->topt);struct _tuple17 _stmttmp40=_tmp198;struct _tuple17 _tmp199=_stmttmp40;void*_tmp19D;void*_tmp19C;void*_tmp19B;void*_tmp19A;_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;_tmp19C=_tmp199.f3;_tmp19D=_tmp199.f4;{struct Cyc_List_List*args=_tmp19A;struct Cyc_Absyn_Exp*reqopt=_tmp19B;struct Cyc_Absyn_Exp*ensopt=_tmp19C;struct Cyc_Absyn_Vardecl*ret_value=_tmp19D;
Cyc_Vcgen_check_requires(env,ts,args,a2,reqopt);
# 1141
a2=Cyc_AssnDef_kill(a2);
Cyc_Vcgen_update_try_assn(env,a2);
# 1144
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e->topt)))a2=(void*)& Cyc_AssnDef_false_assn;
if(ensopt == 0)
return({struct _tuple13 _tmp223;_tmp223.f1=0,_tmp223.f2=a2;_tmp223;});{
struct Cyc_Absyn_Exp*ensures=ensopt;
# 1149
struct _tuple13 _tmp19E=Cyc_Vcgen_vcgen_test(env,ensures,a2);struct _tuple13 _stmttmp41=_tmp19E;struct _tuple13 _tmp19F=_stmttmp41;void*_tmp1A0;_tmp1A0=_tmp19F.f1;{void*at=_tmp1A0;
if(ret_value != 0){
void*_tmp1A1=Cyc_AssnDef_fresh_var(ret_value->type);void*v=_tmp1A1;
# 1153
at=Cyc_AssnDef_subst(ret_value,v,at);
return({struct _tuple13 _tmp224;_tmp224.f1=v,_tmp224.f2=at;_tmp224;});}
# 1156
return({struct _tuple13 _tmp225;_tmp225.f1=0,_tmp225.f2=at;_tmp225;});}}}}}}case 11: _tmp144=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Exp*e=_tmp144;
# 1158
struct _tuple13 _tmp1A2=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp42=_tmp1A2;struct _tuple13 _tmp1A3=_stmttmp42;void*_tmp1A4;_tmp1A4=_tmp1A3.f2;{void*a=_tmp1A4;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple13 _tmp226;_tmp226.f1=0,_tmp226.f2=(void*)& Cyc_AssnDef_false_assn;_tmp226;});}}case 12: _tmp144=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Exp*e=_tmp144;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13: _tmp144=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Exp*e=_tmp144;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14: _tmp144=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp143)->f2;_tmp149=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp143)->f4;{void*tp=_tmp144;struct Cyc_Absyn_Exp*e1=_tmp146;enum Cyc_Absyn_Coercion c=_tmp149;
# 1166
struct _tuple13 _tmp1A5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp43=_tmp1A5;struct _tuple13 _tmp1A6=_stmttmp43;void*_tmp1A8;void*_tmp1A7;_tmp1A7=_tmp1A6.f1;_tmp1A8=_tmp1A6.f2;{void*topt=_tmp1A7;void*a=_tmp1A8;
# 1174
{struct _tuple13 _tmp1A9=({struct _tuple13 _tmp22A;({void*_tmp2D4=Cyc_Absyn_compress(tp);_tmp22A.f1=_tmp2D4;}),({void*_tmp2D3=Cyc_Absyn_compress((void*)_check_null(e->topt));_tmp22A.f2=_tmp2D3;});_tmp22A;});struct _tuple13 _stmttmp44=_tmp1A9;struct _tuple13 _tmp1AA=_stmttmp44;enum Cyc_Absyn_Size_of _tmp1AE;enum Cyc_Absyn_Sign _tmp1AD;enum Cyc_Absyn_Size_of _tmp1AC;enum Cyc_Absyn_Sign _tmp1AB;if(*((int*)_tmp1AA.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f1)->f1)== 1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f1)->f1)->f2){case Cyc_Absyn_Int_sz: if(*((int*)_tmp1AA.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f2)->f1)){case 4:
# 1178
 return({struct _tuple13 _tmp227;_tmp227.f1=topt,_tmp227.f2=a;_tmp227;});case 1: goto _LLAA;default: goto _LLAC;}else{goto _LLAC;}case Cyc_Absyn_Long_sz: if(*((int*)_tmp1AA.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f2)->f1)){case 4:
# 1180
 return({struct _tuple13 _tmp228;_tmp228.f1=topt,_tmp228.f2=a;_tmp228;});case 1: goto _LLAA;default: goto _LLAC;}else{goto _LLAC;}default: if(*((int*)_tmp1AA.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f2)->f1)== 1){_LLAA: _tmp1AB=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f1)->f1)->f1;_tmp1AC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f1)->f1)->f2;_tmp1AD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f2)->f1)->f1;_tmp1AE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA.f2)->f1)->f2;{enum Cyc_Absyn_Sign u1=_tmp1AB;enum Cyc_Absyn_Size_of s1=_tmp1AC;enum Cyc_Absyn_Sign u2=_tmp1AD;enum Cyc_Absyn_Size_of s2=_tmp1AE;
# 1186
s1=(int)s1 == 3?2U: s1;
s2=(int)s2 == 3?2U: s2;
if((int)s1 == (int)s2)return({struct _tuple13 _tmp229;_tmp229.f1=topt,_tmp229.f2=a;_tmp229;});
goto _LLA5;}}else{goto _LLAC;}}else{goto _LLAC;}}else{goto _LLAC;}}else{_LLAC:
 goto _LLA5;}_LLA5:;}
# 1192
if((int)c == 1){
if(topt == 0)return({struct _tuple13 _tmp22B;_tmp22B.f1=0,_tmp22B.f2=a;_tmp22B;});{
void*t=topt;
return({struct _tuple13 _tmp22C;({void*_tmp2D5=Cyc_AssnDef_cast(tp,t);_tmp22C.f1=_tmp2D5;}),_tmp22C.f2=a;_tmp22C;});}}else{
# 1198
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple13 _tmp22D;topt == 0?_tmp22D.f1=0:({void*_tmp2D6=Cyc_AssnDef_cast(tp,topt);_tmp22D.f1=_tmp2D6;}),_tmp22D.f2=a;_tmp22D;});}}}case 16: _tmp144=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{struct Cyc_Absyn_Exp*eopt=_tmp144;struct Cyc_Absyn_Exp*e=_tmp146;
# 1202
if(eopt != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain)).f2;
return({struct _tuple13 _tmp22E;_tmp22E.f1=0,({void*_tmp2D7=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp22E.f2=_tmp2D7;});_tmp22E;});}case 32:
# 1207
 goto _LL2A;case 33: _LL2A:
 goto _LL2C;case 17: _LL2C:
 goto _LL2E;case 18: _LL2E:
 goto _LL30;case 39: _LL30:
 goto _LL32;case 19: _LL32:
 return({struct _tuple13 _tmp22F;({void*_tmp2D8=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->tag=1,_tmp1AF->f1=e;_tmp1AF;});_tmp22F.f1=_tmp2D8;}),_tmp22F.f2=ain;_tmp22F;});case 21: _tmp144=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Exp*e=_tmp144;
# 1215
_tmp144=e;goto _LL36;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple13 _tmp230;_tmp230.f1=0,_tmp230.f2=ain;_tmp230;});}case 38: _tmp144=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp144;
# 1220
_tmp144=e;goto _LL38;}case 15: _tmp144=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp144;
# 1222
return({struct _tuple13 _tmp231;_tmp231.f1=0,({void*_tmp2D9=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp231.f2=_tmp2D9;});_tmp231;});}case 20: _tmp144=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Exp*e=_tmp144;
# 1224
_tmp144=e;goto _LL3C;}case 22: _tmp144=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp144;
_tmp144=e;_tmp146=Cyc_Vcgen_shared_zero_exp();goto _LL3E;}case 23: _tmp144=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp143)->f2;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmp144;struct Cyc_Absyn_Exp*e2=_tmp146;
# 1227
struct _tuple13 _tmp1B0=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);struct _tuple13 _stmttmp45=_tmp1B0;struct _tuple13 _tmp1B1=_stmttmp45;void*_tmp1B3;void*_tmp1B2;_tmp1B2=_tmp1B1.f1;_tmp1B3=_tmp1B1.f2;{void*t1=_tmp1B2;void*a1=_tmp1B3;
struct _tuple13 _tmp1B4=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp46=_tmp1B4;struct _tuple13 _tmp1B5=_stmttmp46;void*_tmp1B7;void*_tmp1B6;_tmp1B6=_tmp1B5.f1;_tmp1B7=_tmp1B5.f2;{void*t2=_tmp1B6;void*a2=_tmp1B7;
{void*_tmp1B8=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp47=_tmp1B8;void*_tmp1B9=_stmttmp47;if(*((int*)_tmp1B9)== 6)
goto _LLB4;else{
# 1234
Cyc_Vcgen_update_try_assn(env,a2);{
void*term_numelts;void*term_typebound;
# 1238
if(t2 == 0)goto _LLB4;
term_numelts=Cyc_AssnDef_primop(18U,({void*_tmp1BA[1];_tmp1BA[0]=t1;({struct Cyc_List_List*(*_tmp2DA)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2DA(_tag_fat(_tmp1BA,sizeof(void*),1));});}));{
# 1250 "vcgen.cyc"
void*_tmp1BB=Cyc_AssnDef_ult(t2,term_numelts);void*inbound_assn=_tmp1BB;
({struct Cyc_Hashtable_Table**_tmp2DC=env->assn_info;struct Cyc_Absyn_Exp*_tmp2DB=e1;Cyc_Vcgen_insert_assn_info(_tmp2DC,_tmp2DB,({struct _tuple13*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->f1=a2,_tmp1BC->f2=inbound_assn;_tmp1BC;}));});
a2=Cyc_AssnDef_and(a2,inbound_assn);}}}_LLB4:;}
# 1254
return({struct _tuple13 _tmp232;_tmp232.f1=0,_tmp232.f2=a2;_tmp232;});}}}case 31: _tmp144=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_List_List*es=_tmp144;
# 1256
_tmp144=es;goto _LL42;}case 24: _tmp144=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_LL42: {struct Cyc_List_List*es=_tmp144;
return({struct _tuple13 _tmp233;_tmp233.f1=0,({void*_tmp2DD=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_tmp233.f2=_tmp2DD;});_tmp233;});}case 25: _tmp144=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{struct Cyc_List_List*dles=_tmp144;
# 1259
_tmp144=dles;goto _LL46;}case 26: _tmp144=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_LL46: {struct Cyc_List_List*dles=_tmp144;
_tmp144=dles;goto _LL48;}case 29: _tmp144=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp143)->f3;_LL48: {struct Cyc_List_List*dles=_tmp144;
_tmp144=dles;goto _LL4A;}case 36: _tmp144=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp143)->f2;_LL4A: {struct Cyc_List_List*dles=_tmp144;
_tmp144=dles;goto _LL4C;}case 30: _tmp144=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp143)->f2;_LL4C: {struct Cyc_List_List*dles=_tmp144;
# 1264
for(1;dles != 0;dles=dles->tl){
struct _tuple18*_tmp1BD=(struct _tuple18*)dles->hd;struct _tuple18*_stmttmp48=_tmp1BD;struct _tuple18*_tmp1BE=_stmttmp48;void*_tmp1BF;_tmp1BF=_tmp1BE->f2;{struct Cyc_Absyn_Exp*e=_tmp1BF;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;}}
# 1268
return({struct _tuple13 _tmp234;_tmp234.f1=0,_tmp234.f2=ain;_tmp234;});}case 27: _tmp144=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp143)->f2;_tmp148=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp143)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp144;struct Cyc_Absyn_Exp*e1=_tmp146;struct Cyc_Absyn_Exp*e2=_tmp148;
# 1272
struct _tuple13 _tmp1C0=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);struct _tuple13 _stmttmp49=_tmp1C0;struct _tuple13 _tmp1C1=_stmttmp49;void*_tmp1C3;void*_tmp1C2;_tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;{void*t1=_tmp1C2;void*a=_tmp1C3;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1C4=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->tag=2,_tmp1CB->f1=vd;_tmp1CB;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*v=_tmp1C4;
# 1275
a=({struct Cyc_Vcgen_Env*_tmp2E0=env;void*_tmp2DF=a;void*_tmp2DE=(void*)v;Cyc_Vcgen_do_assign(_tmp2E0,_tmp2DF,_tmp2DE,Cyc_AssnDef_zero());});
while(1){
void*_tmp1C5=({void*_tmp2E1=a;Cyc_AssnDef_and(_tmp2E1,Cyc_AssnDef_ult((void*)v,t1));});void*at=_tmp1C5;
void*_tmp1C6=({void*_tmp2E2=a;Cyc_AssnDef_and(_tmp2E2,Cyc_AssnDef_ulte(t1,(void*)v));});void*af=_tmp1C6;
struct _tuple13 _tmp1C7=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp4A=_tmp1C7;struct _tuple13 _tmp1C8=_stmttmp4A;void*_tmp1C9;_tmp1C9=_tmp1C8.f2;{void*abody=_tmp1C9;
abody=({struct Cyc_Vcgen_Env*_tmp2E7=env;void*_tmp2E6=abody;void*_tmp2E5=(void*)v;Cyc_Vcgen_do_assign(_tmp2E7,_tmp2E6,_tmp2E5,Cyc_AssnDef_primop(0U,({void*_tmp1CA[2];_tmp1CA[0]=(void*)v,({void*_tmp2E3=Cyc_AssnDef_one();_tmp1CA[1]=_tmp2E3;});({struct Cyc_List_List*(*_tmp2E4)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2E4(_tag_fat(_tmp1CA,sizeof(void*),2));});})));});
if(Cyc_Vcgen_simple_implies(& abody,& a)){a=af;break;}
# 1283
a=Cyc_Vcgen_widen(Cyc_AssnDef_or(abody,a));}}
# 1285
return({struct _tuple13 _tmp235;_tmp235.f1=0,_tmp235.f2=a;_tmp235;});}}case 28: _tmp144=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Exp*e=_tmp144;
# 1287
return({struct _tuple13 _tmp236;_tmp236.f1=0,({void*_tmp2E8=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp236.f2=_tmp2E8;});_tmp236;});}case 34: _tmp147=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_MallocInfo mi=_tmp147;
# 1289
if(mi.rgn != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain)).f2;
return({struct _tuple13 _tmp237;_tmp237.f1=0,({void*_tmp2E9=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain)).f2;_tmp237.f2=_tmp2E9;});_tmp237;});}case 35: _tmp144=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp143)->f1;_tmp146=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp143)->f2;{struct Cyc_Absyn_Exp*e1=_tmp144;struct Cyc_Absyn_Exp*e2=_tmp146;
# 1293
struct _tuple13 _tmp1CC=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple13 _stmttmp4B=_tmp1CC;struct _tuple13 _tmp1CD=_stmttmp4B;void*_tmp1CF;void*_tmp1CE;_tmp1CE=_tmp1CD.f1;_tmp1CF=_tmp1CD.f2;{void*lt1=_tmp1CE;void*a1=_tmp1CF;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple13 _tmp1D0=Cyc_Vcgen_vcgen_lexp(env,e2,a1);struct _tuple13 _stmttmp4C=_tmp1D0;struct _tuple13 _tmp1D1=_stmttmp4C;void*_tmp1D3;void*_tmp1D2;_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;{void*lt2=_tmp1D2;void*a2=_tmp1D3;
Cyc_Vcgen_update_try_assn(env,a2);{
void*_tmp1D4=Cyc_Vcgen_deref_lterm(env,lt1,a2);void*t1=_tmp1D4;
void*_tmp1D5=Cyc_Vcgen_deref_lterm(env,lt2,a2);void*t2=_tmp1D5;
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple13 _tmp238;_tmp238.f1=0,_tmp238.f2=a2;_tmp238;});}}}}}case 37: _tmp144=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Stmt*s=_tmp144;
# 1303
void**_tmp1D6=(env->shared)->exp_stmt;void**old_exp_stmt=_tmp1D6;
({void**_tmp2EA=({void**_tmp1D7=_cycalloc(sizeof(*_tmp1D7));*_tmp1D7=0;_tmp1D7;});(env->shared)->exp_stmt=_tmp2EA;});{
void*_tmp1D8=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);void*aout=_tmp1D8;
void*_tmp1D9=*((void**)_check_null((env->shared)->exp_stmt));void*t=_tmp1D9;
(env->shared)->exp_stmt=old_exp_stmt;
return({struct _tuple13 _tmp239;_tmp239.f1=t,_tmp239.f2=aout;_tmp239;});}}case 40:
 return({struct _tuple13 _tmp23A;_tmp23A.f1=0,_tmp23A.f2=ain;_tmp23A;});case 41: _tmp144=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Exp*e=_tmp144;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2: _tmp145=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct _fat_ptr s=_tmp145;
return({struct _tuple13 _tmp23B;({void*_tmp2EB=Cyc_AssnDef_zero();_tmp23B.f1=_tmp2EB;}),_tmp23B.f2=ain;_tmp23B;});}default: _tmp144=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp143)->f1;{struct Cyc_Absyn_Exp*e1=_tmp144;
# 1313
struct _tuple13 _tmp1DA=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp4D=_tmp1DA;struct _tuple13 _tmp1DB=_stmttmp4D;void*_tmp1DC;_tmp1DC=_tmp1DB.f1;{void*at=_tmp1DC;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp1DF=({struct Cyc_String_pa_PrintArg_struct _tmp23D;_tmp23D.tag=0,({
struct _fat_ptr _tmp2EC=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(at));_tmp23D.f1=_tmp2EC;});_tmp23D;});struct Cyc_String_pa_PrintArg_struct _tmp1E0=({struct Cyc_String_pa_PrintArg_struct _tmp23C;_tmp23C.tag=0,({struct _fat_ptr _tmp2ED=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_tmp23C.f1=_tmp2ED;});_tmp23C;});void*_tmp1DD[2];_tmp1DD[0]=& _tmp1DF,_tmp1DD[1]=& _tmp1E0;({unsigned _tmp2EF=e->loc;struct _fat_ptr _tmp2EE=({const char*_tmp1DE="cannot prove @assert(%s) \n\nfrom %s";_tag_fat(_tmp1DE,sizeof(char),35U);});Cyc_Warn_err(_tmp2EF,_tmp2EE,_tag_fat(_tmp1DD,sizeof(void*),2));});});
return({struct _tuple13 _tmp23E;({void*_tmp2F1=Cyc_AssnDef_one();_tmp23E.f1=_tmp2F1;}),({void*_tmp2F0=Cyc_AssnDef_and(ain,at);_tmp23E.f2=_tmp2F0;});_tmp23E;});}}};}}
# 1323
void Cyc_Vcgen_vcgen_fundecl(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
void*a=(void*)& Cyc_AssnDef_true_assn;
# 1326
struct _tuple17 _tmp1E1=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);struct _tuple17 _stmttmp4E=_tmp1E1;struct _tuple17 _tmp1E2=_stmttmp4E;void*_tmp1E6;void*_tmp1E5;void*_tmp1E4;void*_tmp1E3;_tmp1E3=_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;_tmp1E5=_tmp1E2.f3;_tmp1E6=_tmp1E2.f4;{struct Cyc_List_List*args=_tmp1E3;struct Cyc_Absyn_Exp*reqopt=_tmp1E4;struct Cyc_Absyn_Exp*ensopt=_tmp1E5;struct Cyc_Absyn_Vardecl*ret_value=_tmp1E6;
void**result_assn=ensopt == 0?0:({void**_tmp1F3=_cycalloc(sizeof(*_tmp1F3));*_tmp1F3=(void*)& Cyc_AssnDef_false_assn;_tmp1F3;});
struct Cyc_Vcgen_Env*_tmp1E7=Cyc_Vcgen_initial_env(tables,fd,assn_info,result_assn);struct Cyc_Vcgen_Env*env=_tmp1E7;
if(reqopt != 0)
# 1331
a=(Cyc_Vcgen_vcgen_test(env,reqopt,a)).f1;
# 1333
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 1335
if(ensopt != 0){
struct Cyc_Absyn_Exp*ens=ensopt;
struct _tuple13 _tmp1E8=Cyc_Vcgen_vcgen_test(env,ens,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp4F=_tmp1E8;struct _tuple13 _tmp1E9=_stmttmp4F;void*_tmp1EA;_tmp1EA=_tmp1E9.f1;{void*at=_tmp1EA;
struct Cyc_Vcgen_SharedEnv*_tmp1EB=env->shared;struct Cyc_Vcgen_SharedEnv*senv=_tmp1EB;
void*_tmp1EC=*((void**)_check_null(senv->res_assn));void*res_assn_opt=_tmp1EC;
void*res_assn=(void*)& Cyc_AssnDef_false_assn;
if(res_assn_opt != 0)res_assn=res_assn_opt;{
struct _fat_ptr str=({const char*_tmp1F2="NULL";_tag_fat(_tmp1F2,sizeof(char),5U);});
if(senv->res_term != 0)str=Cyc_AssnDef_term2string((void*)_check_null(senv->res_term));
if(ret_value != 0 &&(unsigned)senv->res_term)
res_assn=({void*_tmp2F3=res_assn;Cyc_AssnDef_and(_tmp2F3,({void*_tmp2F2=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->tag=2,_tmp1ED->f1=ret_value;_tmp1ED;});Cyc_AssnDef_eq(_tmp2F2,senv->res_term);}));});
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp1F0=({struct Cyc_String_pa_PrintArg_struct _tmp240;_tmp240.tag=0,({
struct _fat_ptr _tmp2F4=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp240.f1=_tmp2F4;});_tmp240;});struct Cyc_String_pa_PrintArg_struct _tmp1F1=({struct Cyc_String_pa_PrintArg_struct _tmp23F;_tmp23F.tag=0,({struct _fat_ptr _tmp2F5=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(res_assn)));_tmp23F.f1=_tmp2F5;});_tmp23F;});void*_tmp1EE[2];_tmp1EE[0]=& _tmp1F0,_tmp1EE[1]=& _tmp1F1;({unsigned _tmp2F7=ens->loc;struct _fat_ptr _tmp2F6=({const char*_tmp1EF="cannot prove the @ensures clause %s from %s";_tag_fat(_tmp1EF,sizeof(char),44U);});Cyc_Warn_err(_tmp2F7,_tmp2F6,_tag_fat(_tmp1EE,sizeof(void*),2));});});}}}
# 1350
Cyc_AssnDef_reset_hash_cons_table();}}

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
  /* MWH: wish we didn't have to include the stuff below ... */
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
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

/* Pools */
struct _pool; // defined in runtime_memory.c
struct _PoolHandle {
  struct _RuntimeStack s;
  struct _pool *p;
};
struct _PoolHandle _new_pool(void);
void _free_pool(struct _PoolHandle *h);
void _push_pool(struct _PoolHandle * r);
void _pop_pool(void);

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
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);
# 37 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 415 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 514
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 532
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 903 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 1004
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
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 63 "tcutil.h"
void*Cyc_Tcutil_pointer_elt_type(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 61 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*t);
# 63
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e);
void*Cyc_AssnDef_zero (void);
void*Cyc_AssnDef_one (void);
# 70
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts);
void*Cyc_AssnDef_cast(void*tp,void*tm);
void*Cyc_AssnDef_fresh_var (void);
# 79
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 93
struct _fat_ptr Cyc_AssnDef_assn2string(void*a);
# 96
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 107 "assndef.h"
void Cyc_AssnDef_reset_hash_cons_table (void);
# 114
void*Cyc_AssnDef_kill(void*a);
# 116
void*Cyc_AssnDef_and(void*a1,void*a2);
void*Cyc_AssnDef_or(void*a1,void*a2);
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a);
# 120
void*Cyc_AssnDef_eq(void*,void*);
void*Cyc_AssnDef_neq(void*,void*);
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 128
void*Cyc_AssnDef_reduce(void*a);
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx);
# 131
int Cyc_AssnDef_simple_prove(void*ctxt,void*a);
# 133
void*Cyc_AssnDef_kill_mem_term(void*t);
# 25 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 126 "dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 193
extern struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct Cyc_Vcgen_Env;
# 110 "vcgen.cyc"
static int Cyc_Vcgen_implies_noreduce(void*a1,void*a2){
return Cyc_AssnDef_simple_prove(a1,a2)|| Cyc_PrattProver_constraint_prove(a1,a2);}
# 116
static int Cyc_Vcgen_implies(void**a1,void**a2){
({void*_tmp24B=Cyc_AssnDef_reduce(*a1);*a1=_tmp24B;});
({void*_tmp24C=Cyc_AssnDef_reduce(*a2);*a2=_tmp24C;});
return Cyc_Vcgen_implies_noreduce(*a1,*a2);}
# 127
static void*Cyc_Vcgen_weaken_it(void*ctxt,void*a){
if(Cyc_AssnDef_simple_prove(ctxt,a))return a;{
void*res;
{void*_tmp0=a;void*_tmp2;void*_tmp1;void*_tmp4;void*_tmp3;switch(*((int*)_tmp0)){case 3U: _LL1: _tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp0)->f1;_tmp4=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp0)->f2;_LL2: {void*a1=_tmp3;void*a2=_tmp4;
# 132
void*_tmp5=Cyc_Vcgen_weaken_it(ctxt,a1);void*b1=_tmp5;
void*_tmp6=Cyc_Vcgen_weaken_it(ctxt,a2);void*b2=_tmp6;
# 136
res=Cyc_AssnDef_and(b1,b2);
goto _LL0;}case 4U: _LL3: _tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp0)->f2;_LL4: {void*a1=_tmp1;void*a2=_tmp2;
# 139
void*_tmp7=Cyc_Vcgen_weaken_it(a2,a1);void*b1=_tmp7;
void*_tmp8=Cyc_Vcgen_weaken_it(a1,a2);void*b2=_tmp8;
res=({void*_tmp24D=ctxt;Cyc_Vcgen_weaken_it(_tmp24D,Cyc_AssnDef_and(b1,b2));});
goto _LL0;}case 0U: _LL5: _LL6:
 goto _LL8;case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA:
# 146
 res=(void*)& Cyc_AssnDef_true_assn;
goto _LL0;default: _LLB: _LLC:
 return({void*_tmp24E=ctxt;Cyc_Vcgen_weaken_it(_tmp24E,Cyc_AssnDef_reduce(a));});}_LL0:;}
# 150
return res;}}
# 160 "vcgen.cyc"
static void*Cyc_Vcgen_weaken_assn(void*ctxt,void*a){
if(Cyc_Vcgen_implies(& ctxt,& a))return a;
return Cyc_Vcgen_weaken_it(ctxt,a);}
# 166
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd);
# 169
static void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_tmp9=tgt;struct Cyc_Absyn_Vardecl*_tmpA;if(_tmp9 == 0){_LL1: _LL2:
# 174
 return Cyc_AssnDef_kill(a);}else{if(((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp9)->tag == 1U){_LL3: _tmpA=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp9)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpA;
# 180
if(vd->escapes){
a=Cyc_AssnDef_kill(a);
src=src == 0?0: Cyc_AssnDef_kill_mem_term(src);}else{
# 184
void*_tmpB=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmpB;
a=Cyc_AssnDef_subst(vd,v2,a);
src=src == 0?0: Cyc_AssnDef_subst_term(src,vd,v2);}
# 189
return({void*_tmp24F=a;Cyc_AssnDef_and(_tmp24F,Cyc_AssnDef_eq(tgt,src));});}}else{_LL5: _LL6:
({void*_tmpC=0U;({struct _fat_ptr _tmp250=({const char*_tmpD="vcgen do_assign";_tag_fat(_tmpD,sizeof(char),16U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp250,_tag_fat(_tmpC,sizeof(void*),0U));});});}}_LL0:;}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void*try_assn;void**exp_stmt;void*res_term;void*res_assn;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;struct Cyc_Hashtable_Table**assn_info;};
# 229 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 231
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
# 233
struct Cyc_Hashtable_Table*_tmpE=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);struct Cyc_Hashtable_Table*succ_table=_tmpE;
struct Cyc_Hashtable_Table*_tmpF=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_table=_tmpF;
struct Cyc_Vcgen_SharedEnv*_tmp10=({struct Cyc_Vcgen_SharedEnv*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->assn_table=assn_table,_tmp13->succ_table=succ_table,_tmp13->try_assn=(void*)& Cyc_AssnDef_false_assn,_tmp13->exp_stmt=0,_tmp13->res_term=0,_tmp13->res_assn=(void*)& Cyc_AssnDef_false_assn;_tmp13;});struct Cyc_Vcgen_SharedEnv*senv=_tmp10;
# 241
struct Cyc_Dict_Dict _tmp11=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty)((int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp);struct Cyc_Dict_Dict d=_tmp11;
return({struct Cyc_Vcgen_Env*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->shared=senv,_tmp12->state_counter=d,_tmp12->assn_info=assn_info;_tmp12;});}
# 245
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_tmp14=_cycalloc(sizeof(*_tmp14));*_tmp14=*env;_tmp14;});}
# 251
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 255
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _tmp251=((struct Cyc_Dict_Dict(*)(int(*f)(int,struct Cyc_Absyn_Vardecl*,int,int),int env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_tmp251;});}
# 263
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Vardecl*k,int*ans))Cyc_Dict_lookup_bool)(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _tmp252=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Vardecl*k,int v))Cyc_Dict_insert)(env->state_counter,vd,res + 1);env->state_counter=_tmp252;});
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=2U,_tmp15->f1=vd,_tmp15->f2=res;_tmp15;});}
# 272
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***_tmp16=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);void***popt=_tmp16;
if(popt != 0){
void**p=*popt;
void*_tmp17=Cyc_AssnDef_or(ain,*p);void*new_precondition=_tmp17;
*p=new_precondition;
return new_precondition;}else{
# 280
return ain;}}
# 284
static void**Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
static void*false_pointer=(void*)& Cyc_AssnDef_false_assn;
void***popt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(popt != 0)
return*popt;else{
# 290
return& false_pointer;}}
# 295
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp18=*aprevopt;void**aprev=_tmp18;
ain=Cyc_Vcgen_weaken_assn(ain,*aprev);
*aprev=ain;}else{
# 302
({struct Cyc_Hashtable_Table*_tmp254=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp253=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,void**val))Cyc_Hashtable_insert)(_tmp254,_tmp253,({void**_tmp19=_cycalloc(sizeof(*_tmp19));*_tmp19=ain;_tmp19;}));});}
# 304
return ain;}
# 309
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp1A=*aprevopt;void**aprev=_tmp1A;
ain=Cyc_AssnDef_or(ain,*aprev);
*aprev=ain;}else{
# 316
({struct Cyc_Hashtable_Table*_tmp256=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp255=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,void**val))Cyc_Hashtable_insert)(_tmp256,_tmp255,({void**_tmp1B=_cycalloc(sizeof(*_tmp1B));*_tmp1B=ain;_tmp1B;}));});}
# 318
return ain;}
# 323
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
({void*_tmp257=Cyc_AssnDef_or((env->shared)->try_assn,a);(env->shared)->try_assn=_tmp257;});}
# 328
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a){
struct Cyc_Vcgen_SharedEnv*_tmp1C=env->shared;struct Cyc_Vcgen_SharedEnv*shared=_tmp1C;
if(shared->res_term != 0 || t != 0){
void*_tmp1D=Cyc_AssnDef_fresh_var();void*v=_tmp1D;
shared->res_term=v;
({void*_tmp25B=({
void*_tmp25A=({void*_tmp258=a;Cyc_AssnDef_and(_tmp258,Cyc_AssnDef_eq(v,t));});Cyc_AssnDef_or(_tmp25A,({void*_tmp259=shared->res_assn;Cyc_AssnDef_and(_tmp259,Cyc_AssnDef_eq(v,shared->res_term));}));});
# 333
shared->res_assn=_tmp25B;});}else{
# 336
({void*_tmp25C=Cyc_AssnDef_or(a,shared->res_assn);shared->res_assn=_tmp25C;});}}
# 341
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void){
struct Cyc_Hashtable_Table*_tmp1E=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*hash)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Exp*s))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_info=_tmp1E;
return({struct Cyc_Hashtable_Table**_tmp1F=_cycalloc(sizeof(*_tmp1F));*_tmp1F=assn_info;_tmp1F;});}struct _tuple12{void*f1;void*f2;};
# 346
struct _tuple12 Cyc_Vcgen_exp2ctxt_assn(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){
struct _tuple12**_tmp20=((struct _tuple12**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Exp*key))Cyc_Hashtable_lookup_opt)(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e);struct _tuple12**result=_tmp20;
if(result == 0)
return({struct _tuple12 _tmp20A;_tmp20A.f1=(void*)& Cyc_AssnDef_true_assn,_tmp20A.f2=(void*)& Cyc_AssnDef_false_assn;_tmp20A;});else{
# 351
return*(*result);}}
# 355
static void Cyc_Vcgen_insert_assn_info(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct _tuple12*ctxt_assn){
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Exp*key,struct _tuple12*val))Cyc_Hashtable_insert)(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e,ctxt_assn);
return;}
# 360
static struct Cyc_Absyn_Exp*Cyc_Vcgen_shared_zero_exp (void){
static struct Cyc_Absyn_Exp*ans=0;
if(!((unsigned)ans))
ans=Cyc_Absyn_uint_exp(0U,0U);
return(struct Cyc_Absyn_Exp*)_check_null(ans);}
# 372
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain);
# 376
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 378
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 380
static struct _tuple12 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 383
static struct _tuple12 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);struct _tuple13{struct Cyc_List_List**f1;void*f2;};
# 387
static struct _tuple13 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a);
# 390
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a);struct _tuple14{struct Cyc_List_List*f1;void*f2;};
# 392
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain);
# 394
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain);
# 396
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a);
# 398
static struct _fat_ptr Cyc_Vcgen_stmt2shortstring(struct Cyc_Absyn_Stmt*s){
void*_tmp21=s->r;void*_stmttmp0=_tmp21;void*_tmp22=_stmttmp0;struct Cyc_Absyn_Stmt*_tmp23;struct Cyc_Absyn_Stmt*_tmp24;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;switch(*((int*)_tmp22)){case 4U: _LL1: _tmp2B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp22)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2B;
# 401
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp20B;_tmp20B.tag=0U,({struct _fat_ptr _tmp25D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp20B.f1=_tmp25D;});_tmp20B;});void*_tmp2C[1U];_tmp2C[0]=& _tmp2E;({struct _fat_ptr _tmp25E=({const char*_tmp2D="if (%s) ...";_tag_fat(_tmp2D,sizeof(char),12U);});Cyc_aprintf(_tmp25E,_tag_fat(_tmp2C,sizeof(void*),1U));});});}case 5U: _LL3: _tmp2A=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp22)->f1).f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp2A;
# 403
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp20C;_tmp20C.tag=0U,({struct _fat_ptr _tmp25F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp20C.f1=_tmp25F;});_tmp20C;});void*_tmp2F[1U];_tmp2F[0]=& _tmp31;({struct _fat_ptr _tmp260=({const char*_tmp30="while (%s) ...";_tag_fat(_tmp30,sizeof(char),15U);});Cyc_aprintf(_tmp260,_tag_fat(_tmp2F,sizeof(void*),1U));});});}case 9U: _LL5: _tmp27=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp22)->f1;_tmp28=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp22)->f2).f1;_tmp29=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp22)->f3).f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp27;struct Cyc_Absyn_Exp*e2=_tmp28;struct Cyc_Absyn_Exp*e3=_tmp29;
# 405
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34=({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0U,({struct _fat_ptr _tmp261=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp20F.f1=_tmp261;});_tmp20F;});struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0U,({
struct _fat_ptr _tmp262=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e2));_tmp20E.f1=_tmp262;});_tmp20E;});struct Cyc_String_pa_PrintArg_struct _tmp36=({struct Cyc_String_pa_PrintArg_struct _tmp20D;_tmp20D.tag=0U,({struct _fat_ptr _tmp263=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e3));_tmp20D.f1=_tmp263;});_tmp20D;});void*_tmp32[3U];_tmp32[0]=& _tmp34,_tmp32[1]=& _tmp35,_tmp32[2]=& _tmp36;({struct _fat_ptr _tmp264=({const char*_tmp33="for (%s;%s;%s) ...";_tag_fat(_tmp33,sizeof(char),19U);});Cyc_aprintf(_tmp264,_tag_fat(_tmp32,sizeof(void*),3U));});});}case 10U: _LL7: _tmp26=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp22)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp26;
# 408
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp210;_tmp210.tag=0U,({struct _fat_ptr _tmp265=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp210.f1=_tmp265;});_tmp210;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({struct _fat_ptr _tmp266=({const char*_tmp38="switch (%s) ...";_tag_fat(_tmp38,sizeof(char),16U);});Cyc_aprintf(_tmp266,_tag_fat(_tmp37,sizeof(void*),1U));});});}case 14U: _LL9: _tmp25=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp22)->f2).f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp25;
# 410
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3C=({struct Cyc_String_pa_PrintArg_struct _tmp211;_tmp211.tag=0U,({struct _fat_ptr _tmp267=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp211.f1=_tmp267;});_tmp211;});void*_tmp3A[1U];_tmp3A[0]=& _tmp3C;({struct _fat_ptr _tmp268=({const char*_tmp3B="do ... while (%s)";_tag_fat(_tmp3B,sizeof(char),18U);});Cyc_aprintf(_tmp268,_tag_fat(_tmp3A,sizeof(void*),1U));});});}case 15U: _LLB: _tmp24=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp22)->f1;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp24;
# 412
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3F=({struct Cyc_String_pa_PrintArg_struct _tmp212;_tmp212.tag=0U,({struct _fat_ptr _tmp269=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_stmt2shortstring(s));_tmp212.f1=_tmp269;});_tmp212;});void*_tmp3D[1U];_tmp3D[0]=& _tmp3F;({struct _fat_ptr _tmp26A=({const char*_tmp3E="try %s catch ...";_tag_fat(_tmp3E,sizeof(char),17U);});Cyc_aprintf(_tmp26A,_tag_fat(_tmp3D,sizeof(void*),1U));});});}case 2U: _LLD: _tmp23=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp22)->f1;_LLE: {struct Cyc_Absyn_Stmt*s=_tmp23;
return Cyc_Vcgen_stmt2shortstring(s);}default: _LLF: _LL10: {
# 415
struct _fat_ptr _tmp40=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_tmp40;
unsigned max=_get_fat_size(s,sizeof(char))< (unsigned)11?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _tmp44=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_tmp44),1U,_tmp44);});
{int i=0;for(0;(unsigned)i < max - (unsigned)1;++ i){
({char _tmp26B=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_tmp26B;});}}
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp43=({struct Cyc_String_pa_PrintArg_struct _tmp213;_tmp213.tag=0U,_tmp213.f1=(struct _fat_ptr)((struct _fat_ptr)res);_tmp213;});void*_tmp41[1U];_tmp41[0]=& _tmp43;({struct _fat_ptr _tmp26C=({const char*_tmp42="%s...";_tag_fat(_tmp42,sizeof(char),6U);});Cyc_aprintf(_tmp26C,_tag_fat(_tmp41,sizeof(void*),1U));});});}}_LL0:;}
# 475 "vcgen.cyc"
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 478
LOOP:
 ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);
# 483
{void*_tmp45=ain;if(((struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_tmp45)->tag == 1U){_LL1: _LL2:
 return ain;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 487
void*_tmp46=s->r;void*_stmttmp1=_tmp46;void*_tmp47=_stmttmp1;struct Cyc_List_List*_tmp49;struct Cyc_Absyn_Stmt*_tmp48;struct Cyc_Absyn_Stmt*_tmp4A;struct Cyc_Absyn_Stmt*_tmp4C;struct Cyc_Absyn_Decl*_tmp4B;struct Cyc_Absyn_Switch_clause*_tmp4E;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp50;struct Cyc_Absyn_Exp*_tmp4F;struct Cyc_Absyn_Stmt*_tmp53;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Stmt*_tmp59;struct Cyc_Absyn_Stmt*_tmp58;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Stmt*_tmp56;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Stmt*_tmp5C;struct Cyc_Absyn_Stmt*_tmp5B;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_Absyn_Stmt*_tmp5F;struct Cyc_Absyn_Stmt*_tmp5E;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_Absyn_Exp*_tmp60;struct Cyc_Absyn_Stmt*_tmp62;struct Cyc_Absyn_Stmt*_tmp61;struct Cyc_Absyn_Exp*_tmp63;switch(*((int*)_tmp47)){case 0U: _LL6: _LL7:
 return ain;case 1U: _LL8: _tmp63=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_LL9: {struct Cyc_Absyn_Exp*e=_tmp63;
# 490
struct _tuple12 _tmp64=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp2=_tmp64;struct _tuple12 _tmp65=_stmttmp2;void*_tmp67;void*_tmp66;_LL2B: _tmp66=_tmp65.f1;_tmp67=_tmp65.f2;_LL2C: {void*t=_tmp66;void*aout=_tmp67;
# 493
if((env->shared)->exp_stmt != 0)
*((void**)_check_null((env->shared)->exp_stmt))=t;
return aout;}}case 2U: _LLA: _tmp61=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp62=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LLB: {struct Cyc_Absyn_Stmt*s1=_tmp61;struct Cyc_Absyn_Stmt*s2=_tmp62;
# 497
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain);
s=s2;
goto LOOP;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp47)->f1 == 0){_LLC: _LLD:
# 501
 Cyc_Vcgen_update_return(env,0,ain);
return(void*)& Cyc_AssnDef_false_assn;}else{_LLE: _tmp60=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_LLF: {struct Cyc_Absyn_Exp*e=_tmp60;
# 504
struct _tuple12 _tmp68=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(e),ain);struct _tuple12 _stmttmp3=_tmp68;struct _tuple12 _tmp69=_stmttmp3;void*_tmp6B;void*_tmp6A;_LL2E: _tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;_LL2F: {void*t=_tmp6A;void*aout=_tmp6B;
Cyc_Vcgen_update_return(env,t,aout);
return(void*)& Cyc_AssnDef_false_assn;}}}case 4U: _LL10: _tmp5D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp5E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_tmp5F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f3;_LL11: {struct Cyc_Absyn_Exp*e1=_tmp5D;struct Cyc_Absyn_Stmt*s1=_tmp5E;struct Cyc_Absyn_Stmt*s2=_tmp5F;
# 508
struct _tuple12 _tmp6C=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp4=_tmp6C;struct _tuple12 _tmp6D=_stmttmp4;void*_tmp6F;void*_tmp6E;_LL31: _tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;_LL32: {void*at=_tmp6E;void*af=_tmp6F;
struct Cyc_Vcgen_Env*_tmp70=Cyc_Vcgen_copy_env(env);struct Cyc_Vcgen_Env*env2=_tmp70;
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return Cyc_AssnDef_or(at,af);}}case 5U: _LL12: _tmp5A=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp47)->f1).f1;_tmp5B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp47)->f1).f2;_tmp5C=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL13: {struct Cyc_Absyn_Exp*e=_tmp5A;struct Cyc_Absyn_Stmt*dummy=_tmp5B;struct Cyc_Absyn_Stmt*s=_tmp5C;
# 520
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);{
struct _tuple12 _tmp71=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple12 _stmttmp5=_tmp71;struct _tuple12 _tmp72=_stmttmp5;void*_tmp74;void*_tmp73;_LL34: _tmp73=_tmp72.f1;_tmp74=_tmp72.f2;_LL35: {void*at=_tmp73;void*af=_tmp74;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7U: _LL14: _LL15:
# 526
 goto _LL17;{
# 529
struct Cyc_Absyn_Stmt*_tmp75=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp75;
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_AssnDef_false_assn;}case 6U: _LL16: _LL17: {
# 536
struct Cyc_Absyn_Stmt*_tmp76=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp76;
if(dest == 0)
Cyc_Vcgen_update_return(env,0,ain);else{
# 540
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_AssnDef_false_assn;}case 9U: _LL18: _tmp54=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp55=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f1;_tmp56=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f2;_tmp57=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f3).f1;_tmp58=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f3).f2;_tmp59=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f4;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp54;struct Cyc_Absyn_Exp*e2=_tmp55;struct Cyc_Absyn_Stmt*dummy2=_tmp56;struct Cyc_Absyn_Exp*e3=_tmp57;struct Cyc_Absyn_Stmt*dummy3=_tmp58;struct Cyc_Absyn_Stmt*s=_tmp59;
# 543
struct _tuple12 _tmp77=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,ain);struct _tuple12 _stmttmp6=_tmp77;struct _tuple12 _tmp78=_stmttmp6;void*_tmp79;_LL37: _tmp79=_tmp78.f2;_LL38: {void*a=_tmp79;
ain=a;
# 546
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);{
struct _tuple12 _tmp7A=Cyc_Vcgen_vcgen_test(env,e2,ain);struct _tuple12 _stmttmp7=_tmp7A;struct _tuple12 _tmp7B=_stmttmp7;void*_tmp7D;void*_tmp7C;_LL3A: _tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;_LL3B: {void*at=_tmp7C;void*af=_tmp7D;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 551
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain)).f2;
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2)))return af;}}}}}case 14U: _LL1A: _tmp51=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp52=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f1;_tmp53=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f2;_LL1B: {struct Cyc_Absyn_Stmt*s=_tmp51;struct Cyc_Absyn_Exp*e=_tmp52;struct Cyc_Absyn_Stmt*dummy=_tmp53;
# 557
while(1){
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple12 _tmp7E=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple12 _stmttmp8=_tmp7E;struct _tuple12 _tmp7F=_stmttmp8;void*_tmp81;void*_tmp80;_LL3D: _tmp80=_tmp7F.f1;_tmp81=_tmp7F.f2;_LL3E: {void*at=_tmp80;void*af=_tmp81;
ain=({void*_tmp26D=*Cyc_Vcgen_get_assn(env,dummy);Cyc_Vcgen_weaken_assn(_tmp26D,ain);});
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10U: _LL1C: _tmp4F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp50=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL1D: {struct Cyc_Absyn_Exp*e=_tmp4F;struct Cyc_List_List*scs=_tmp50;
# 570
struct _tuple12 _tmp82=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp9=_tmp82;struct _tuple12 _tmp83=_stmttmp9;void*_tmp85;void*_tmp84;_LL40: _tmp84=_tmp83.f1;_tmp85=_tmp83.f2;_LL41: {void*t=_tmp84;void*a=_tmp85;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp47)->f2 == 0){_LL1E: _LL1F:
({void*_tmp86=0U;({struct _fat_ptr _tmp26E=({const char*_tmp87="vcgen fallthru";_tag_fat(_tmp87,sizeof(char),15U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp26E,_tag_fat(_tmp86,sizeof(void*),0U));});});}else{_LL20: _tmp4D=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp4E=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL21: {struct Cyc_List_List*es=_tmp4D;struct Cyc_Absyn_Switch_clause*dest=_tmp4E;
# 575
struct _tuple14 _tmp88=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmpA=_tmp88;struct _tuple14 _tmp89=_stmttmpA;void*_tmp8B;struct Cyc_List_List*_tmp8A;_LL43: _tmp8A=_tmp89.f1;_tmp8B=_tmp89.f2;_LL44: {struct Cyc_List_List*ts=_tmp8A;void*a=_tmp8B;
if(ts != 0){
# 578
struct Cyc_List_List*_tmp8C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(dest->pat_vars))->v)).f1);struct Cyc_List_List*x=_tmp8C;
# 580
for(0;x != 0;(x=x->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*_tmp8D=(struct Cyc_Absyn_Vardecl*)x->hd;struct Cyc_Absyn_Vardecl*vd=_tmp8D;
if(vd != 0)
a=({struct Cyc_Vcgen_Env*_tmp271=env;void*_tmp270=a;void*_tmp26F=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=1U,_tmp8E->f1=vd;_tmp8E;});Cyc_Vcgen_do_assign(_tmp271,_tmp270,_tmp26F,(void*)((struct Cyc_List_List*)_check_null(ts))->hd);});}}
# 586
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_AssnDef_false_assn;}}}case 12U: _LL22: _tmp4B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp4C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL23: {struct Cyc_Absyn_Decl*d=_tmp4B;struct Cyc_Absyn_Stmt*s1=_tmp4C;
# 589
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13U: _LL24: _tmp4A=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL25: {struct Cyc_Absyn_Stmt*s=_tmp4A;
# 597
return Cyc_Vcgen_vcgen_stmt_nodebug(env,s,(void*)& Cyc_AssnDef_true_assn);}case 8U: _LL26: _LL27:
 return(void*)& Cyc_AssnDef_false_assn;default: _LL28: _tmp48=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp49=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL29: {struct Cyc_Absyn_Stmt*s=_tmp48;struct Cyc_List_List*scs=_tmp49;
# 601
void*_tmp8F=(env->shared)->try_assn;void*old_try_assn=_tmp8F;
# 603
(env->shared)->try_assn=(void*)& Cyc_AssnDef_false_assn;
# 606
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 608
void*_tmp90=(env->shared)->try_assn;void*new_try_assn=_tmp90;
(env->shared)->try_assn=old_try_assn;
# 612
return({void*_tmp272=ain;Cyc_AssnDef_or(_tmp272,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}}_LL5:;}}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 616
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 620
void*res=(void*)& Cyc_AssnDef_false_assn;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp91=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmp91;
void*_tmp92=ain;void*a=_tmp92;
{struct Cyc_List_List*_tmp93=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sc->pat_vars))->v;struct Cyc_List_List*pvs=_tmp93;for(0;pvs != 0;pvs=pvs->tl){
struct _tuple15*_tmp94=(struct _tuple15*)pvs->hd;struct _tuple15*_stmttmpB=_tmp94;struct _tuple15*_tmp95=_stmttmpB;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Vardecl**_tmp96;_LL1: _tmp96=_tmp95->f1;_tmp97=_tmp95->f2;_LL2: {struct Cyc_Absyn_Vardecl**vdopt=_tmp96;struct Cyc_Absyn_Exp*eopt=_tmp97;
if(vdopt != 0){
if(eopt != 0)
a=({struct Cyc_Vcgen_Env*_tmp275=env;void*_tmp274=a;void*_tmp273=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=1U,_tmp98->f1=*vdopt;_tmp98;});Cyc_Vcgen_do_assign(_tmp275,_tmp274,_tmp273,(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a)).f1);});else{
# 630
a=({struct Cyc_Vcgen_Env*_tmp278=env;void*_tmp277=a;void*_tmp276=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=1U,_tmp99->f1=*vdopt;_tmp99;});Cyc_Vcgen_do_assign(_tmp278,_tmp277,_tmp276,Cyc_AssnDef_fresh_var());});}}}}}
# 633
if(sc->where_clause != 0)
a=(Cyc_Vcgen_vcgen_test(env,(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause),a)).f1;
res=({void*_tmp279=res;Cyc_AssnDef_or(_tmp279,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 637
return res;}
# 640
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 643
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp9A=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=1U,_tmp9F->f1=vd;_tmp9F;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv=_tmp9A;
a=({struct Cyc_Vcgen_Env*_tmp27C=env;void*_tmp27B=a;void*_tmp27A=(void*)tv;Cyc_Vcgen_do_assign(_tmp27C,_tmp27B,_tmp27A,Cyc_AssnDef_fresh_var());});
if(vd->initializer != 0){
struct _tuple12 _tmp9B=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),a);struct _tuple12 _stmttmpC=_tmp9B;struct _tuple12 _tmp9C=_stmttmpC;void*_tmp9E;void*_tmp9D;_LL1: _tmp9D=_tmp9C.f1;_tmp9E=_tmp9C.f2;_LL2: {void*t=_tmp9D;void*a1=_tmp9E;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 649
return a;}
# 652
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_tmpA0=d->r;void*_stmttmpD=_tmpA0;void*_tmpA1=_stmttmpD;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Vardecl*_tmpA2;struct Cyc_List_List*_tmpA4;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Core_Opt*_tmpA6;struct Cyc_Absyn_Pat*_tmpA5;struct Cyc_Absyn_Vardecl*_tmpA8;switch(*((int*)_tmpA1)){case 0U: _LL1: _tmpA8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmpA8;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2U: _LL3: _tmpA5=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmpA6=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_tmpA7=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f3;_LL4: {struct Cyc_Absyn_Pat*p=_tmpA5;struct Cyc_Core_Opt*vds=_tmpA6;struct Cyc_Absyn_Exp*e=_tmpA7;
# 656
struct _tuple12 _tmpA9=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple12 _stmttmpE=_tmpA9;struct _tuple12 _tmpAA=_stmttmpE;void*_tmpAB;_LLC: _tmpAB=_tmpAA.f2;_LLD: {void*a=_tmpAB;
{struct Cyc_List_List*_tmpAC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds))->v;struct Cyc_List_List*vds=_tmpAC;for(0;vds != 0;vds=vds->tl){
struct _tuple15 _tmpAD=*((struct _tuple15*)vds->hd);struct _tuple15 _stmttmpF=_tmpAD;struct _tuple15 _tmpAE=_stmttmpF;struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Vardecl**_tmpAF;_LLF: _tmpAF=_tmpAE.f1;_tmpB0=_tmpAE.f2;_LL10: {struct Cyc_Absyn_Vardecl**vdopt=_tmpAF;struct Cyc_Absyn_Exp*eopt=_tmpB0;
if(vdopt != 0){
if(eopt == 0)a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);else{
# 662
struct _tuple12 _tmpB1=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple12 _stmttmp10=_tmpB1;struct _tuple12 _tmpB2=_stmttmp10;void*_tmpB4;void*_tmpB3;_LL12: _tmpB3=_tmpB2.f1;_tmpB4=_tmpB2.f2;_LL13: {void*t=_tmpB3;void*a1=_tmpB4;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpB5=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->tag=1U,_tmpB6->f1=*vdopt;_tmpB6;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv=_tmpB5;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}}}}}
# 668
return a;}}case 3U: _LL5: _tmpA4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL6: {struct Cyc_List_List*vds=_tmpA4;
# 670
for(0;vds != 0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4U: _LL7: _tmpA2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_tmpA3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f3;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpA2;struct Cyc_Absyn_Exp*eopt=_tmpA3;
# 674
if(eopt != 0){
struct _tuple12 _tmpB7=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple12 _stmttmp11=_tmpB7;struct _tuple12 _tmpB8=_stmttmp11;void*_tmpBA;void*_tmpB9;_LL15: _tmpB9=_tmpB8.f1;_tmpBA=_tmpB8.f2;_LL16: {void*t=_tmpB9;void*a1=_tmpBA;
a=a1;}}
# 679
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default: _LL9: _LLA:
 return a;}_LL0:;}
# 687
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpBB=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*t1=_tmpBB;
void*_tmpBC=Cyc_Absyn_compress((void*)_check_null(e2->topt));void*t2=_tmpBC;
struct _tuple12 _tmpBD=({struct _tuple12 _tmp214;_tmp214.f1=t1,_tmp214.f2=t2;_tmp214;});struct _tuple12 _stmttmp12=_tmpBD;struct _tuple12 _tmpBE=_stmttmp12;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f1)->f1)->f1 == Cyc_Absyn_Unsigned){_LL1: _LL2:
 return 1;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3: _LL4:
 return 1;}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f1)->tag == 3U)goto _LL7;else{goto _LL7;}}}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f1)->tag == 3U)goto _LL7;else{goto _LL7;}}}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f2)->tag == 3U){_LL5: _LL6:
 return 1;}else{goto _LL7;}}else{_LL7: _LL8:
 return 0;}}}_LL0:;}
# 703
static struct _tuple12 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmpBF=e->r;void*_stmttmp13=_tmpBF;void*_tmpC0=_stmttmp13;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC2;enum Cyc_Absyn_Primop _tmpC1;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;switch(*((int*)_tmpC0)){case 6U: _LL1: _tmpCB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC0)->f1;_tmpCC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC0)->f2;_tmpCD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC0)->f3;_LL2: {struct Cyc_Absyn_Exp*e1=_tmpCB;struct Cyc_Absyn_Exp*e2=_tmpCC;struct Cyc_Absyn_Exp*e3=_tmpCD;
# 706
struct _tuple12 _tmpCE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp14=_tmpCE;struct _tuple12 _tmpCF=_stmttmp14;void*_tmpD1;void*_tmpD0;_LL10: _tmpD0=_tmpCF.f1;_tmpD1=_tmpCF.f2;_LL11: {void*a1t=_tmpD0;void*a1f=_tmpD1;
struct _tuple12 _tmpD2=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple12 _stmttmp15=_tmpD2;struct _tuple12 _tmpD3=_stmttmp15;void*_tmpD5;void*_tmpD4;_LL13: _tmpD4=_tmpD3.f1;_tmpD5=_tmpD3.f2;_LL14: {void*a2t=_tmpD4;void*a2f=_tmpD5;
struct _tuple12 _tmpD6=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct _tuple12 _stmttmp16=_tmpD6;struct _tuple12 _tmpD7=_stmttmp16;void*_tmpD9;void*_tmpD8;_LL16: _tmpD8=_tmpD7.f1;_tmpD9=_tmpD7.f2;_LL17: {void*a3t=_tmpD8;void*a3f=_tmpD9;
return({struct _tuple12 _tmp215;({void*_tmp27E=Cyc_AssnDef_or(a2t,a3t);_tmp215.f1=_tmp27E;}),({void*_tmp27D=Cyc_AssnDef_or(a2f,a3f);_tmp215.f2=_tmp27D;});_tmp215;});}}}}case 7U: _LL3: _tmpC9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC0)->f1;_tmpCA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC0)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmpC9;struct Cyc_Absyn_Exp*e2=_tmpCA;
# 711
struct _tuple12 _tmpDA=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp17=_tmpDA;struct _tuple12 _tmpDB=_stmttmp17;void*_tmpDD;void*_tmpDC;_LL19: _tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_LL1A: {void*a1t=_tmpDC;void*a1f=_tmpDD;
struct _tuple12 _tmpDE=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple12 _stmttmp18=_tmpDE;struct _tuple12 _tmpDF=_stmttmp18;void*_tmpE1;void*_tmpE0;_LL1C: _tmpE0=_tmpDF.f1;_tmpE1=_tmpDF.f2;_LL1D: {void*a2t=_tmpE0;void*a2f=_tmpE1;
return({struct _tuple12 _tmp216;_tmp216.f1=a2t,({void*_tmp27F=Cyc_AssnDef_or(a1f,a2f);_tmp216.f2=_tmp27F;});_tmp216;});}}}case 8U: _LL5: _tmpC7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC0)->f1;_tmpC8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC0)->f2;_LL6: {struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Absyn_Exp*e2=_tmpC8;
# 715
struct _tuple12 _tmpE2=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp19=_tmpE2;struct _tuple12 _tmpE3=_stmttmp19;void*_tmpE5;void*_tmpE4;_LL1F: _tmpE4=_tmpE3.f1;_tmpE5=_tmpE3.f2;_LL20: {void*a1t=_tmpE4;void*a1f=_tmpE5;
struct _tuple12 _tmpE6=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct _tuple12 _stmttmp1A=_tmpE6;struct _tuple12 _tmpE7=_stmttmp1A;void*_tmpE9;void*_tmpE8;_LL22: _tmpE8=_tmpE7.f1;_tmpE9=_tmpE7.f2;_LL23: {void*a2t=_tmpE8;void*a2f=_tmpE9;
return({struct _tuple12 _tmp217;({void*_tmp280=Cyc_AssnDef_or(a1t,a2t);_tmp217.f1=_tmp280;}),_tmp217.f2=a2f;_tmp217;});}}}case 9U: _LL7: _tmpC5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC0)->f1;_tmpC6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC0)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmpC5;struct Cyc_Absyn_Exp*e2=_tmpC6;
# 719
struct _tuple12 _tmpEA=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp1B=_tmpEA;struct _tuple12 _tmpEB=_stmttmp1B;void*_tmpEC;_LL25: _tmpEC=_tmpEB.f2;_LL26: {void*ain=_tmpEC;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f2 != 0){_LL9: _tmpC4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f2)->hd;_LLA: {struct Cyc_Absyn_Exp*e1=_tmpC4;
# 722
struct _tuple12 _tmpED=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp1C=_tmpED;struct _tuple12 _tmpEE=_stmttmp1C;void*_tmpF0;void*_tmpEF;_LL28: _tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;_LL29: {void*at=_tmpEF;void*af=_tmpF0;
return({struct _tuple12 _tmp218;_tmp218.f1=af,_tmp218.f2=at;_tmp218;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f2)->tl)->tl == 0){_LLB: _tmpC1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f1;_tmpC2=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f2)->hd;_tmpC3=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0)->f2)->tl)->hd;_LLC: {enum Cyc_Absyn_Primop p=_tmpC1;struct Cyc_Absyn_Exp*e1=_tmpC2;struct Cyc_Absyn_Exp*e2=_tmpC3;
# 725
struct _tuple12 _tmpF1=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp1D=_tmpF1;struct _tuple12 _tmpF2=_stmttmp1D;void*_tmpF4;void*_tmpF3;_LL2B: _tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;_LL2C: {void*t1=_tmpF3;void*a1=_tmpF4;
struct _tuple12 _tmpF5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp1E=_tmpF5;struct _tuple12 _tmpF6=_stmttmp1E;void*_tmpF8;void*_tmpF7;_LL2E: _tmpF7=_tmpF6.f1;_tmpF8=_tmpF6.f2;_LL2F: {void*t2=_tmpF7;void*a2=_tmpF8;
void*_tmpF9=a2;void*at=_tmpF9;
void*_tmpFA=a2;void*af=_tmpFA;
{enum Cyc_Absyn_Primop _tmpFB=p;switch(_tmpFB){case Cyc_Absyn_Eq: _LL31: _LL32:
 at=({void*_tmp281=at;Cyc_AssnDef_and(_tmp281,Cyc_AssnDef_eq(t1,t2));});af=({void*_tmp282=af;Cyc_AssnDef_and(_tmp282,Cyc_AssnDef_neq(t1,t2));});goto _LL30;case Cyc_Absyn_Neq: _LL33: _LL34:
 at=({void*_tmp283=at;Cyc_AssnDef_and(_tmp283,Cyc_AssnDef_neq(t1,t2));});af=({void*_tmp284=af;Cyc_AssnDef_and(_tmp284,Cyc_AssnDef_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Lt: _LL35: _LL36:
# 733
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp285=at;Cyc_AssnDef_and(_tmp285,Cyc_AssnDef_ult(t1,t2));});af=({void*_tmp286=af;Cyc_AssnDef_and(_tmp286,Cyc_AssnDef_ulte(t2,t1));});}else{
# 736
at=({void*_tmp287=at;Cyc_AssnDef_and(_tmp287,Cyc_AssnDef_slt(t1,t2));});af=({void*_tmp288=af;Cyc_AssnDef_and(_tmp288,Cyc_AssnDef_slte(t2,t1));});}
# 738
goto _LL30;case Cyc_Absyn_Lte: _LL37: _LL38:
# 740
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp289=at;Cyc_AssnDef_and(_tmp289,Cyc_AssnDef_ulte(t1,t2));});af=({void*_tmp28A=af;Cyc_AssnDef_and(_tmp28A,Cyc_AssnDef_ult(t2,t1));});}else{
# 743
at=({void*_tmp28B=at;Cyc_AssnDef_and(_tmp28B,Cyc_AssnDef_slte(t1,t2));});af=({void*_tmp28C=af;Cyc_AssnDef_and(_tmp28C,Cyc_AssnDef_slt(t2,t1));});}
# 745
goto _LL30;case Cyc_Absyn_Gt: _LL39: _LL3A:
# 747
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp28D=at;Cyc_AssnDef_and(_tmp28D,Cyc_AssnDef_ult(t2,t1));});af=({void*_tmp28E=af;Cyc_AssnDef_and(_tmp28E,Cyc_AssnDef_ulte(t1,t2));});}else{
# 750
at=({void*_tmp28F=at;Cyc_AssnDef_and(_tmp28F,Cyc_AssnDef_slt(t2,t1));});af=({void*_tmp290=af;Cyc_AssnDef_and(_tmp290,Cyc_AssnDef_slte(t1,t2));});}
# 752
goto _LL30;case Cyc_Absyn_Gte: _LL3B: _LL3C:
# 754
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp291=at;Cyc_AssnDef_and(_tmp291,Cyc_AssnDef_ulte(t2,t1));});af=({void*_tmp292=af;Cyc_AssnDef_and(_tmp292,Cyc_AssnDef_ult(t1,t2));});}else{
# 757
at=({void*_tmp293=at;Cyc_AssnDef_and(_tmp293,Cyc_AssnDef_slte(t2,t1));});af=({void*_tmp294=af;Cyc_AssnDef_and(_tmp294,Cyc_AssnDef_slt(t1,t2));});}
# 759
goto _LL30;default: _LL3D: _LL3E:
 goto _LL30;}_LL30:;}
# 762
return({struct _tuple12 _tmp219;_tmp219.f1=at,_tmp219.f2=af;_tmp219;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: _LLE: {
# 764
struct _tuple12 _tmpFC=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp1F=_tmpFC;struct _tuple12 _tmpFD=_stmttmp1F;void*_tmpFF;void*_tmpFE;_LL40: _tmpFE=_tmpFD.f1;_tmpFF=_tmpFD.f2;_LL41: {void*t=_tmpFE;void*aout=_tmpFF;
return({struct _tuple12 _tmp21A;({void*_tmp29A=({void*_tmp299=aout;Cyc_AssnDef_and(_tmp299,({void*_tmp298=t;Cyc_AssnDef_neq(_tmp298,Cyc_AssnDef_zero());}));});_tmp21A.f1=_tmp29A;}),({void*_tmp297=({void*_tmp296=aout;Cyc_AssnDef_and(_tmp296,({void*_tmp295=t;Cyc_AssnDef_eq(_tmp295,Cyc_AssnDef_zero());}));});_tmp21A.f2=_tmp297;});_tmp21A;});}}}_LL0:;}
# 769
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(0;es != 0;es=es->tl){
a=(Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a)).f2;}
return a;}
# 775
static struct _tuple13 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(0;es != 0;es=es->tl){
struct _tuple12 _tmp100=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);struct _tuple12 _stmttmp20=_tmp100;struct _tuple12 _tmp101=_stmttmp20;void*_tmp103;void*_tmp102;_LL1: _tmp102=_tmp101.f1;_tmp103=_tmp101.f2;_LL2: {void*t=_tmp102;void*aout=_tmp103;
a=aout;
if(valid && t != 0)
res=({struct Cyc_List_List*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->hd=t,_tmp104->tl=res;_tmp104;});else{
# 784
valid=0;}}}
# 787
return({struct _tuple13 _tmp21B;valid?({struct Cyc_List_List**_tmp29C=({struct Cyc_List_List**_tmp105=_cycalloc(sizeof(*_tmp105));({struct Cyc_List_List*_tmp29B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);*_tmp105=_tmp29B;});_tmp105;});_tmp21B.f1=_tmp29C;}):(_tmp21B.f1=0),_tmp21B.f2=a;_tmp21B;});}
# 790
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple12 _tmp106=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp21=_tmp106;struct _tuple12 _tmp107=_stmttmp21;void*_tmp109;void*_tmp108;_LL1: _tmp108=_tmp107.f1;_tmp109=_tmp107.f2;_LL2: {void*topt=_tmp108;void*a=_tmp109;
return({struct _tuple12 _tmp21C;topt == 0?({void*_tmp29D=Cyc_AssnDef_fresh_var();_tmp21C.f1=_tmp29D;}):(_tmp21C.f1=topt),_tmp21C.f2=a;_tmp21C;});}}
# 795
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(0;es != 0;es=es->tl){
struct _tuple12 _tmp10A=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);struct _tuple12 _stmttmp22=_tmp10A;struct _tuple12 _tmp10B=_stmttmp22;void*_tmp10D;void*_tmp10C;_LL1: _tmp10C=_tmp10B.f1;_tmp10D=_tmp10B.f2;_LL2: {void*t=_tmp10C;void*a=_tmp10D;
ain=a;
res=({struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->hd=t,_tmp10E->tl=res;_tmp10E;});}}
# 802
return({struct _tuple14 _tmp21D;({struct Cyc_List_List*_tmp29E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);_tmp21D.f1=_tmp29E;}),_tmp21D.f2=ain;_tmp21D;});}
# 805
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_tmp10F=b;struct Cyc_Absyn_Vardecl*_tmp110;struct Cyc_Absyn_Vardecl*_tmp111;struct Cyc_Absyn_Vardecl*_tmp112;struct Cyc_Absyn_Fndecl*_tmp113;struct Cyc_Absyn_Vardecl*_tmp114;switch(*((int*)_tmp10F)){case 1U: _LL1: _tmp114=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp10F)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp114;
return vd;}case 2U: _LL3: _tmp113=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp10F)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp113;
return(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);}case 0U: _LL5: _LL6:
({void*_tmp115=0U;({struct _fat_ptr _tmp29F=({const char*_tmp116="vcgen bind2vardecl";_tag_fat(_tmp116,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp29F,_tag_fat(_tmp115,sizeof(void*),0U));});});case 3U: _LL7: _tmp112=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp10F)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp112;
return vd;}case 4U: _LL9: _tmp111=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp10F)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp111;
return vd;}default: _LLB: _tmp110=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp10F)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp110;
return vd;}}_LL0:;}
# 816
static struct _tuple12 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp117=e->r;void*_stmttmp23=_tmp117;void*_tmp118=_stmttmp23;void*_tmp119;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp118)->tag == 1U){_LL1: _tmp119=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp118)->f1;_LL2: {void*b=_tmp119;
# 819
return({struct _tuple12 _tmp21E;({void*_tmp2A1=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp2A0=Cyc_Vcgen_bind2vardecl(b);_tmp11A->f1=_tmp2A0;});_tmp11A;});_tmp21E.f1=_tmp2A1;}),_tmp21E.f2=ain;_tmp21E;});}}else{_LL3: _LL4:
 return({struct _tuple12 _tmp21F;_tmp21F.f1=0,({void*_tmp2A2=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp21F.f2=_tmp2A2;});_tmp21F;});}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Vardecl*f4;};
# 826
static struct _tuple16 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple16 _tmp220;_tmp220.f1=fi->args,_tmp220.f2=fi->requires_clause,_tmp220.f3=fi->ensures_clause,_tmp220.f4=fi->return_value;_tmp220;});}
# 830
static struct _tuple16 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt != 0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_tmp11B=t;struct Cyc_Absyn_FnInfo*_tmp11C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11B)->tag == 5U){_LL1: _tmp11C=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11B)->f1;_LL2: {struct Cyc_Absyn_FnInfo*fi=_tmp11C;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 838
return({struct _tuple16 _tmp221;_tmp221.f1=0,_tmp221.f2=0,_tmp221.f3=0,_tmp221.f4=0;_tmp221;});}
# 841
static void Cyc_Vcgen_check_requires(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*args,void*a,struct Cyc_Absyn_Exp*reqopt){
# 844
if(reqopt != 0){
struct Cyc_Absyn_Exp*req=reqopt;
# 848
for(0;args != 0 && inputs != 0;(args=args->tl,inputs=inputs->tl)){
struct _tuple9*_tmp11D=(struct _tuple9*)args->hd;struct _tuple9*_stmttmp24=_tmp11D;struct _tuple9*_tmp11E=_stmttmp24;void*_tmp121;struct Cyc_Absyn_Tqual _tmp120;struct _fat_ptr*_tmp11F;_LL1: _tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;_tmp121=_tmp11E->f3;_LL2: {struct _fat_ptr*vopt=_tmp11F;struct Cyc_Absyn_Tqual tq=_tmp120;void*t=_tmp121;
if(vopt != 0){
# 852
struct _tuple1*qv=({struct _tuple1*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=Cyc_Absyn_Loc_n,_tmp125->f2=vopt;_tmp125;});
struct Cyc_Absyn_Vardecl*_tmp122=({struct Cyc_Absyn_Vardecl*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->sc=Cyc_Absyn_Public,_tmp124->name=qv,_tmp124->varloc=0U,_tmp124->tq=tq,_tmp124->type=t,_tmp124->initializer=0,_tmp124->rgn=0,_tmp124->attributes=0,_tmp124->escapes=0,_tmp124->is_proto=0;_tmp124;});struct Cyc_Absyn_Vardecl*vd=_tmp122;
# 858
a=({void*_tmp2A4=a;Cyc_AssnDef_and(_tmp2A4,({void*_tmp2A3=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=1U,_tmp123->f1=vd;_tmp123;});Cyc_AssnDef_eq(_tmp2A3,(void*)inputs->hd);}));});}}}{
# 862
struct _tuple12 _tmp126=Cyc_Vcgen_vcgen_test(env,reqopt,(void*)& Cyc_AssnDef_true_assn);struct _tuple12 _stmttmp25=_tmp126;struct _tuple12 _tmp127=_stmttmp25;void*_tmp128;_LL4: _tmp128=_tmp127.f1;_LL5: {void*at=_tmp128;
if(!Cyc_Vcgen_implies(& a,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp12B=({struct Cyc_String_pa_PrintArg_struct _tmp223;_tmp223.tag=0U,({
struct _fat_ptr _tmp2A5=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp223.f1=_tmp2A5;});_tmp223;});struct Cyc_String_pa_PrintArg_struct _tmp12C=({struct Cyc_String_pa_PrintArg_struct _tmp222;_tmp222.tag=0U,({struct _fat_ptr _tmp2A6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(a)));_tmp222.f1=_tmp2A6;});_tmp222;});void*_tmp129[2U];_tmp129[0]=& _tmp12B,_tmp129[1]=& _tmp12C;({unsigned _tmp2A8=req->loc;struct _fat_ptr _tmp2A7=({const char*_tmp12A="cannot prove @requires %s from %s";_tag_fat(_tmp12A,sizeof(char),34U);});Cyc_Warn_err(_tmp2A8,_tmp2A7,_tag_fat(_tmp129,sizeof(void*),2U));});});}}}}
# 869
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 873
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e))return({struct _tuple12 _tmp224;({void*_tmp2A9=Cyc_AssnDef_cnst(e);_tmp224.f1=_tmp2A9;}),_tmp224.f2=ain;_tmp224;});{
void*_tmp12D=e->r;void*_stmttmp26=_tmp12D;void*_tmp12E=_stmttmp26;struct Cyc_Absyn_Exp*_tmp12F;struct _fat_ptr _tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Stmt*_tmp132;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_MallocInfo _tmp135;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Vardecl*_tmp137;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*_tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp148;enum Cyc_Absyn_Coercion _tmp14C;struct Cyc_Absyn_Exp*_tmp14B;void*_tmp14A;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;enum Cyc_Absyn_Incrementor _tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Core_Opt*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_List_List*_tmp161;enum Cyc_Absyn_Primop _tmp160;void*_tmp162;union Cyc_Absyn_Cnst _tmp163;switch(*((int*)_tmp12E)){case 0U: _LL1: _tmp163=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp163;
# 878
return({struct _tuple12 _tmp225;({void*_tmp2AA=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->tag=0U,_tmp164->f1=e;_tmp164;});_tmp225.f1=_tmp2AA;}),_tmp225.f2=ain;_tmp225;});}case 1U: _LL3: _tmp162=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL4: {void*b=_tmp162;
return({struct _tuple12 _tmp226;({void*_tmp2AC=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp2AB=Cyc_Vcgen_bind2vardecl(b);_tmp165->f1=_tmp2AB;});_tmp165;});_tmp226.f1=_tmp2AC;}),_tmp226.f2=ain;_tmp226;});}case 3U: _LL5: _tmp160=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp161=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL6: {enum Cyc_Absyn_Primop p=_tmp160;struct Cyc_List_List*es=_tmp161;
# 881
struct _tuple13 _tmp166=Cyc_Vcgen_vcgen_rexps(env,es,ain);struct _tuple13 _stmttmp27=_tmp166;struct _tuple13 _tmp167=_stmttmp27;void*_tmp169;struct Cyc_List_List**_tmp168;_LL58: _tmp168=_tmp167.f1;_tmp169=_tmp167.f2;_LL59: {struct Cyc_List_List**ts=_tmp168;void*aout=_tmp169;
void*t=0;
if(ts != 0)t=Cyc_AssnDef_primop(p,*ts);
return({struct _tuple12 _tmp227;_tmp227.f1=t,_tmp227.f2=aout;_tmp227;});}}case 4U: _LL7: _tmp15D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp15E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_tmp15F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12E)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp15D;struct Cyc_Core_Opt*popt=_tmp15E;struct Cyc_Absyn_Exp*e2=_tmp15F;
# 886
struct _tuple12 _tmp16A=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple12 _stmttmp28=_tmp16A;struct _tuple12 _tmp16B=_stmttmp28;void*_tmp16D;void*_tmp16C;_LL5B: _tmp16C=_tmp16B.f1;_tmp16D=_tmp16B.f2;_LL5C: {void*lt=_tmp16C;void*a1=_tmp16D;
struct _tuple12 _tmp16E=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp29=_tmp16E;struct _tuple12 _tmp16F=_stmttmp29;void*_tmp171;void*_tmp170;_LL5E: _tmp170=_tmp16F.f1;_tmp171=_tmp16F.f2;_LL5F: {void*t2=_tmp170;void*a2=_tmp171;
if(popt != 0){
void*_tmp172=Cyc_Vcgen_deref_lterm(env,lt,a2);void*t1=_tmp172;
if(t1 == 0 || t2 == 0)
t2=0;else{
# 893
t2=({enum Cyc_Absyn_Primop _tmp2AD=(enum Cyc_Absyn_Primop)popt->v;Cyc_AssnDef_primop(_tmp2AD,({void*_tmp173[2U];_tmp173[0]=t1,_tmp173[1]=t2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp173,sizeof(void*),2U));}));});}}{
# 895
void*_tmp174=Cyc_Vcgen_do_assign(env,a2,lt,t2);void*a=_tmp174;
return({struct _tuple12 _tmp228;_tmp228.f1=lt,_tmp228.f2=a;_tmp228;});}}}}case 5U: _LL9: _tmp15B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp15C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp15B;enum Cyc_Absyn_Incrementor i=_tmp15C;
# 898
struct _tuple12 _tmp175=Cyc_Vcgen_vcgen_lexp(env,e,ain);struct _tuple12 _stmttmp2A=_tmp175;struct _tuple12 _tmp176=_stmttmp2A;void*_tmp178;void*_tmp177;_LL61: _tmp177=_tmp176.f1;_tmp178=_tmp176.f2;_LL62: {void*lt=_tmp177;void*a1=_tmp178;
void*_tmp179=Cyc_Vcgen_deref_lterm(env,lt,a1);void*t1=_tmp179;
void*res=0;
void*assn=0;
{enum Cyc_Absyn_Incrementor _tmp17A=i;switch(_tmp17A){case Cyc_Absyn_PreInc: _LL64: _LL65:
# 904
 if(t1 != 0)assn=Cyc_AssnDef_primop(Cyc_Absyn_Plus,({void*_tmp17B[2U];_tmp17B[0]=t1,({void*_tmp2AE=Cyc_AssnDef_one();_tmp17B[1]=_tmp2AE;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17B,sizeof(void*),2U));}));
res=assn;
goto _LL63;case Cyc_Absyn_PostInc: _LL66: _LL67:
# 908
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(Cyc_Absyn_Plus,({void*_tmp17C[2U];_tmp17C[0]=t1,({void*_tmp2AF=Cyc_AssnDef_one();_tmp17C[1]=_tmp2AF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17C,sizeof(void*),2U));}));
goto _LL63;case Cyc_Absyn_PreDec: _LL68: _LL69:
# 912
 if(t1 != 0)assn=Cyc_AssnDef_primop(Cyc_Absyn_Minus,({void*_tmp17D[2U];_tmp17D[0]=t1,({void*_tmp2B0=Cyc_AssnDef_one();_tmp17D[1]=_tmp2B0;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17D,sizeof(void*),2U));}));
res=assn;
goto _LL63;case Cyc_Absyn_PostDec: _LL6A: _LL6B:
# 916
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(Cyc_Absyn_Minus,({void*_tmp17E[2U];_tmp17E[0]=t1,({void*_tmp2B1=Cyc_AssnDef_one();_tmp17E[1]=_tmp2B1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17E,sizeof(void*),2U));}));
goto _LL63;default: _LL6C: _LL6D:
({void*_tmp17F=0U;({struct _fat_ptr _tmp2B2=({const char*_tmp180="vcgen vcgen_rexp increment_e";_tag_fat(_tmp180,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp2B2,_tag_fat(_tmp17F,sizeof(void*),0U));});});}_LL63:;}{
# 921
void*_tmp181=Cyc_Vcgen_do_assign(env,a1,lt,assn);void*a=_tmp181;
return({struct _tuple12 _tmp229;_tmp229.f1=res,_tmp229.f2=a;_tmp229;});}}}case 6U: _LLB: _tmp158=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp159=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_tmp15A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12E)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp158;struct Cyc_Absyn_Exp*e2=_tmp159;struct Cyc_Absyn_Exp*e3=_tmp15A;
# 924
struct _tuple12 _tmp182=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp2B=_tmp182;struct _tuple12 _tmp183=_stmttmp2B;void*_tmp185;void*_tmp184;_LL6F: _tmp184=_tmp183.f1;_tmp185=_tmp183.f2;_LL70: {void*at=_tmp184;void*af=_tmp185;
struct _tuple12 _tmp186=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp2C=_tmp186;struct _tuple12 _tmp187=_stmttmp2C;void*_tmp189;void*_tmp188;_LL72: _tmp188=_tmp187.f1;_tmp189=_tmp187.f2;_LL73: {void*t1=_tmp188;void*at=_tmp189;
struct _tuple12 _tmp18A=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);struct _tuple12 _stmttmp2D=_tmp18A;struct _tuple12 _tmp18B=_stmttmp2D;void*_tmp18D;void*_tmp18C;_LL75: _tmp18C=_tmp18B.f1;_tmp18D=_tmp18B.f2;_LL76: {void*t2=_tmp18C;void*af=_tmp18D;
if(t1 != 0 && t2 != 0){
void*_tmp18E=Cyc_AssnDef_fresh_var();void*v=_tmp18E;
at=({void*_tmp2B3=at;Cyc_AssnDef_and(_tmp2B3,Cyc_AssnDef_eq(v,t1));});
af=({void*_tmp2B4=af;Cyc_AssnDef_and(_tmp2B4,Cyc_AssnDef_eq(v,t2));});
return({struct _tuple12 _tmp22A;_tmp22A.f1=v,({void*_tmp2B5=Cyc_AssnDef_or(at,af);_tmp22A.f2=_tmp2B5;});_tmp22A;});}else{
# 933
return({struct _tuple12 _tmp22B;_tmp22B.f1=0,({void*_tmp2B6=Cyc_AssnDef_or(at,af);_tmp22B.f2=_tmp2B6;});_tmp22B;});}}}}}case 7U: _LLD: _tmp156=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp157=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp156;struct Cyc_Absyn_Exp*e2=_tmp157;
# 935
struct _tuple12 _tmp18F=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp2E=_tmp18F;struct _tuple12 _tmp190=_stmttmp2E;void*_tmp192;void*_tmp191;_LL78: _tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_LL79: {void*at=_tmp191;void*af=_tmp192;
struct _tuple12 _tmp193=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp2F=_tmp193;struct _tuple12 _tmp194=_stmttmp2F;void*_tmp196;void*_tmp195;_LL7B: _tmp195=_tmp194.f1;_tmp196=_tmp194.f2;_LL7C: {void*t=_tmp195;void*at=_tmp196;
void*_tmp197=Cyc_AssnDef_fresh_var();void*v=_tmp197;
at=({void*_tmp2B7=at;Cyc_AssnDef_and(_tmp2B7,Cyc_AssnDef_eq(v,t));});
af=({void*_tmp2B9=af;Cyc_AssnDef_and(_tmp2B9,({void*_tmp2B8=v;Cyc_AssnDef_eq(_tmp2B8,Cyc_AssnDef_zero());}));});
return({struct _tuple12 _tmp22C;_tmp22C.f1=v,({void*_tmp2BA=Cyc_AssnDef_or(at,af);_tmp22C.f2=_tmp2BA;});_tmp22C;});}}}case 8U: _LLF: _tmp154=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp155=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp154;struct Cyc_Absyn_Exp*e2=_tmp155;
# 942
struct _tuple12 _tmp198=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp30=_tmp198;struct _tuple12 _tmp199=_stmttmp30;void*_tmp19B;void*_tmp19A;_LL7E: _tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;_LL7F: {void*at=_tmp19A;void*af=_tmp19B;
struct _tuple12 _tmp19C=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);struct _tuple12 _stmttmp31=_tmp19C;struct _tuple12 _tmp19D=_stmttmp31;void*_tmp19F;void*_tmp19E;_LL81: _tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;_LL82: {void*t=_tmp19E;void*af=_tmp19F;
void*_tmp1A0=Cyc_AssnDef_fresh_var();void*v=_tmp1A0;
at=({void*_tmp2BC=at;Cyc_AssnDef_and(_tmp2BC,({void*_tmp2BB=v;Cyc_AssnDef_eq(_tmp2BB,Cyc_AssnDef_one());}));});
af=({void*_tmp2BD=af;Cyc_AssnDef_and(_tmp2BD,Cyc_AssnDef_eq(v,t));});
return({struct _tuple12 _tmp22D;_tmp22D.f1=v,({void*_tmp2BE=Cyc_AssnDef_or(at,af);_tmp22D.f2=_tmp2BE;});_tmp22D;});}}}case 9U: _LL11: _tmp152=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp153=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp152;struct Cyc_Absyn_Exp*e2=_tmp153;
# 949
struct _tuple12 _tmp1A1=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp32=_tmp1A1;struct _tuple12 _tmp1A2=_stmttmp32;void*_tmp1A3;_LL84: _tmp1A3=_tmp1A2.f2;_LL85: {void*a=_tmp1A3;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10U: _LL13: _tmp150=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp151=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_tmp150;struct Cyc_List_List*es=_tmp151;
# 952
struct _tuple14 _tmp1A4=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp33=_tmp1A4;struct _tuple14 _tmp1A5=_stmttmp33;void*_tmp1A7;struct Cyc_List_List*_tmp1A6;_LL87: _tmp1A6=_tmp1A5.f1;_tmp1A7=_tmp1A5.f2;_LL88: {struct Cyc_List_List*ts=_tmp1A6;void*a=_tmp1A7;
struct _tuple12 _tmp1A8=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple12 _stmttmp34=_tmp1A8;struct _tuple12 _tmp1A9=_stmttmp34;void*_tmp1AA;_LL8A: _tmp1AA=_tmp1A9.f2;_LL8B: {void*a2=_tmp1AA;
# 955
struct _tuple16 _tmp1AB=Cyc_Vcgen_get_requires_and_ensures(e->topt);struct _tuple16 _stmttmp35=_tmp1AB;struct _tuple16 _tmp1AC=_stmttmp35;struct Cyc_Absyn_Vardecl*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_List_List*_tmp1AD;_LL8D: _tmp1AD=_tmp1AC.f1;_tmp1AE=_tmp1AC.f2;_tmp1AF=_tmp1AC.f3;_tmp1B0=_tmp1AC.f4;_LL8E: {struct Cyc_List_List*args=_tmp1AD;struct Cyc_Absyn_Exp*reqopt=_tmp1AE;struct Cyc_Absyn_Exp*ensopt=_tmp1AF;struct Cyc_Absyn_Vardecl*ret_value=_tmp1B0;
Cyc_Vcgen_check_requires(env,ts,args,a2,reqopt);
# 959
a2=Cyc_AssnDef_kill(a2);
Cyc_Vcgen_update_try_assn(env,a2);
# 962
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e->topt)))a2=(void*)& Cyc_AssnDef_false_assn;
if(ensopt == 0)
return({struct _tuple12 _tmp22E;_tmp22E.f1=0,_tmp22E.f2=a2;_tmp22E;});{
struct Cyc_Absyn_Exp*ensures=ensopt;
# 967
struct _tuple12 _tmp1B1=Cyc_Vcgen_vcgen_test(env,ensures,a2);struct _tuple12 _stmttmp36=_tmp1B1;struct _tuple12 _tmp1B2=_stmttmp36;void*_tmp1B3;_LL90: _tmp1B3=_tmp1B2.f1;_LL91: {void*at=_tmp1B3;
if(ret_value != 0){
void*_tmp1B4=Cyc_AssnDef_fresh_var();void*v=_tmp1B4;
# 971
at=Cyc_AssnDef_subst(ret_value,v,at);
return({struct _tuple12 _tmp22F;_tmp22F.f1=v,_tmp22F.f2=at;_tmp22F;});}
# 974
return({struct _tuple12 _tmp230;_tmp230.f1=0,_tmp230.f2=at;_tmp230;});}}}}}}case 11U: _LL15: _tmp14F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp14F;
# 976
struct _tuple12 _tmp1B5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp37=_tmp1B5;struct _tuple12 _tmp1B6=_stmttmp37;void*_tmp1B7;_LL93: _tmp1B7=_tmp1B6.f2;_LL94: {void*a=_tmp1B7;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple12 _tmp231;_tmp231.f1=0,_tmp231.f2=(void*)& Cyc_AssnDef_false_assn;_tmp231;});}}case 12U: _LL17: _tmp14E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp14E;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13U: _LL19: _tmp14D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp14D;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14U: _LL1B: _tmp14A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp14B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_tmp14C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12E)->f4;_LL1C: {void*tp=_tmp14A;struct Cyc_Absyn_Exp*e=_tmp14B;enum Cyc_Absyn_Coercion c=_tmp14C;
# 984
struct _tuple12 _tmp1B8=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp38=_tmp1B8;struct _tuple12 _tmp1B9=_stmttmp38;void*_tmp1BB;void*_tmp1BA;_LL96: _tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;_LL97: {void*topt=_tmp1BA;void*a=_tmp1BB;
if((int)c == (int)1U){
if(topt == 0)return({struct _tuple12 _tmp232;_tmp232.f1=0,_tmp232.f2=a;_tmp232;});{
void*t=topt;
# 994
{struct _tuple12 _tmp1BC=({struct _tuple12 _tmp234;({void*_tmp2C0=Cyc_Absyn_compress(tp);_tmp234.f1=_tmp2C0;}),({void*_tmp2BF=Cyc_Absyn_compress((void*)_check_null(e->topt));_tmp234.f2=_tmp2BF;});_tmp234;});struct _tuple12 _stmttmp39=_tmp1BC;struct _tuple12 _tmp1BD=_stmttmp39;enum Cyc_Absyn_Size_of _tmp1C1;enum Cyc_Absyn_Sign _tmp1C0;enum Cyc_Absyn_Size_of _tmp1BF;enum Cyc_Absyn_Sign _tmp1BE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BD.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BD.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BD.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BD.f2)->f1)->tag == 1U){_LL99: _tmp1BE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BD.f1)->f1)->f1;_tmp1BF=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BD.f1)->f1)->f2;_tmp1C0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BD.f2)->f1)->f1;_tmp1C1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BD.f2)->f1)->f2;_LL9A: {enum Cyc_Absyn_Sign u1=_tmp1BE;enum Cyc_Absyn_Size_of s1=_tmp1BF;enum Cyc_Absyn_Sign u2=_tmp1C0;enum Cyc_Absyn_Size_of s2=_tmp1C1;
# 996
if((int)s1 == (int)s2)return({struct _tuple12 _tmp233;_tmp233.f1=t,_tmp233.f2=a;_tmp233;});
goto _LL98;}}else{goto _LL9B;}}else{goto _LL9B;}}else{goto _LL9B;}}else{_LL9B: _LL9C:
 goto _LL98;}_LL98:;}
# 1000
return({struct _tuple12 _tmp235;({void*_tmp2C1=Cyc_AssnDef_cast(tp,t);_tmp235.f1=_tmp2C1;}),_tmp235.f2=a;_tmp235;});}}else{
# 1003
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple12 _tmp236;topt == 0?_tmp236.f1=0:({void*_tmp2C2=Cyc_AssnDef_cast(tp,topt);_tmp236.f1=_tmp2C2;}),_tmp236.f2=a;_tmp236;});}}}case 16U: _LL1D: _tmp148=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp149=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL1E: {struct Cyc_Absyn_Exp*eopt=_tmp148;struct Cyc_Absyn_Exp*e=_tmp149;
# 1007
if(eopt != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain)).f2;
return({struct _tuple12 _tmp237;_tmp237.f1=0,({void*_tmp2C3=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp237.f2=_tmp2C3;});_tmp237;});}case 32U: _LL1F: _LL20:
# 1012
 goto _LL22;case 33U: _LL21: _LL22:
 goto _LL24;case 17U: _LL23: _LL24:
 goto _LL26;case 18U: _LL25: _LL26:
 goto _LL28;case 39U: _LL27: _LL28:
 goto _LL2A;case 19U: _LL29: _LL2A:
 return({struct _tuple12 _tmp238;_tmp238.f1=0,_tmp238.f2=ain;_tmp238;});case 21U: _LL2B: _tmp147=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp147;
# 1020
_tmp146=e;goto _LL2E;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple12 _tmp239;_tmp239.f1=0,_tmp239.f2=ain;_tmp239;});}case 38U: _LL2D: _tmp146=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp146;
# 1025
_tmp145=e;goto _LL30;}case 15U: _LL2F: _tmp145=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp145;
# 1027
return({struct _tuple12 _tmp23A;_tmp23A.f1=0,({void*_tmp2C4=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp23A.f2=_tmp2C4;});_tmp23A;});}case 20U: _LL31: _tmp144=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp144;
# 1029
_tmp143=e;goto _LL34;}case 22U: _LL33: _tmp143=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL34: {struct Cyc_Absyn_Exp*e=_tmp143;
_tmp141=e;_tmp142=Cyc_Vcgen_shared_zero_exp();goto _LL36;}case 23U: _LL35: _tmp141=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp142=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp141;struct Cyc_Absyn_Exp*e2=_tmp142;
# 1032
struct _tuple12 _tmp1C2=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp3A=_tmp1C2;struct _tuple12 _tmp1C3=_stmttmp3A;void*_tmp1C5;void*_tmp1C4;_LL9E: _tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;_LL9F: {void*t1=_tmp1C4;void*a1=_tmp1C5;
struct _tuple12 _tmp1C6=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp3B=_tmp1C6;struct _tuple12 _tmp1C7=_stmttmp3B;void*_tmp1C9;void*_tmp1C8;_LLA1: _tmp1C8=_tmp1C7.f1;_tmp1C9=_tmp1C7.f2;_LLA2: {void*t2=_tmp1C8;void*a2=_tmp1C9;
{void*_tmp1CA=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp3C=_tmp1CA;void*_tmp1CB=_stmttmp3C;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1CB)->tag == 6U){_LLA4: _LLA5:
 goto _LLA3;}else{_LLA6: _LLA7:
# 1037
 Cyc_Vcgen_update_try_assn(env,ain);{
void*additional;
if(t1 == 0)
additional=(void*)& Cyc_AssnDef_true_assn;else{
# 1042
additional=({void*_tmp2C5=t2;Cyc_AssnDef_ult(_tmp2C5,Cyc_AssnDef_primop(Cyc_Absyn_Numelts,({void*_tmp1CC[1U];_tmp1CC[0]=t1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CC,sizeof(void*),1U));})));});}
# 1044
({struct Cyc_Hashtable_Table**_tmp2C7=env->assn_info;struct Cyc_Absyn_Exp*_tmp2C6=e1;Cyc_Vcgen_insert_assn_info(_tmp2C7,_tmp2C6,({struct _tuple12*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->f1=a2,_tmp1CD->f2=additional;_tmp1CD;}));});
# 1046
a2=Cyc_AssnDef_and(a2,additional);}}_LLA3:;}
# 1048
return({struct _tuple12 _tmp23B;_tmp23B.f1=0,_tmp23B.f2=a2;_tmp23B;});}}}case 31U: _LL37: _tmp140=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL38: {struct Cyc_List_List*es=_tmp140;
# 1050
_tmp13F=es;goto _LL3A;}case 24U: _LL39: _tmp13F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL3A: {struct Cyc_List_List*es=_tmp13F;
return({struct _tuple12 _tmp23C;_tmp23C.f1=0,({void*_tmp2C8=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_tmp23C.f2=_tmp2C8;});_tmp23C;});}case 25U: _LL3B: _tmp13E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp13E;
# 1053
_tmp13D=dles;goto _LL3E;}case 26U: _LL3D: _tmp13D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL3E: {struct Cyc_List_List*dles=_tmp13D;
_tmp13C=dles;goto _LL40;}case 29U: _LL3F: _tmp13C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12E)->f3;_LL40: {struct Cyc_List_List*dles=_tmp13C;
_tmp13B=dles;goto _LL42;}case 36U: _LL41: _tmp13B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL42: {struct Cyc_List_List*dles=_tmp13B;
_tmp13A=dles;goto _LL44;}case 30U: _LL43: _tmp13A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL44: {struct Cyc_List_List*dles=_tmp13A;
# 1058
for(0;dles != 0;dles=dles->tl){
struct _tuple17*_tmp1CE=(struct _tuple17*)dles->hd;struct _tuple17*_stmttmp3D=_tmp1CE;struct _tuple17*_tmp1CF=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp1D0;_LLA9: _tmp1D0=_tmp1CF->f2;_LLAA: {struct Cyc_Absyn_Exp*e=_tmp1D0;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;}}
# 1062
return({struct _tuple12 _tmp23D;_tmp23D.f1=0,_tmp23D.f2=ain;_tmp23D;});}case 27U: _LL45: _tmp137=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp138=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_tmp139=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12E)->f3;_LL46: {struct Cyc_Absyn_Vardecl*vd=_tmp137;struct Cyc_Absyn_Exp*e1=_tmp138;struct Cyc_Absyn_Exp*e2=_tmp139;
# 1066
struct _tuple12 _tmp1D1=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp3E=_tmp1D1;struct _tuple12 _tmp1D2=_stmttmp3E;void*_tmp1D4;void*_tmp1D3;_LLAC: _tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;_LLAD: {void*t1=_tmp1D3;void*a=_tmp1D4;
void*_tmp1D5=t1 == 0?Cyc_AssnDef_fresh_var(): t1;{void*t1=_tmp1D5;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1D6=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->tag=1U,_tmp1DD->f1=vd;_tmp1DD;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*v=_tmp1D6;
# 1070
a=({struct Cyc_Vcgen_Env*_tmp2CB=env;void*_tmp2CA=a;void*_tmp2C9=(void*)v;Cyc_Vcgen_do_assign(_tmp2CB,_tmp2CA,_tmp2C9,Cyc_AssnDef_zero());});
while(1){
void*_tmp1D7=({void*_tmp2CC=a;Cyc_AssnDef_and(_tmp2CC,Cyc_AssnDef_ult((void*)v,t1));});void*at=_tmp1D7;
void*_tmp1D8=({void*_tmp2CD=a;Cyc_AssnDef_and(_tmp2CD,Cyc_AssnDef_ulte(t1,(void*)v));});void*af=_tmp1D8;
struct _tuple12 _tmp1D9=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp3F=_tmp1D9;struct _tuple12 _tmp1DA=_stmttmp3F;void*_tmp1DB;_LLAF: _tmp1DB=_tmp1DA.f2;_LLB0: {void*abody=_tmp1DB;
abody=({struct Cyc_Vcgen_Env*_tmp2D1=env;void*_tmp2D0=abody;void*_tmp2CF=(void*)v;Cyc_Vcgen_do_assign(_tmp2D1,_tmp2D0,_tmp2CF,Cyc_AssnDef_primop(Cyc_Absyn_Plus,({void*_tmp1DC[2U];_tmp1DC[0]=(void*)v,({void*_tmp2CE=Cyc_AssnDef_one();_tmp1DC[1]=_tmp2CE;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1DC,sizeof(void*),2U));})));});
if(Cyc_Vcgen_implies(& abody,& a)){a=af;break;}
a=Cyc_Vcgen_weaken_assn(abody,a);}}
# 1079
return({struct _tuple12 _tmp23E;_tmp23E.f1=0,_tmp23E.f2=a;_tmp23E;});}}}case 28U: _LL47: _tmp136=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL48: {struct Cyc_Absyn_Exp*e=_tmp136;
# 1081
return({struct _tuple12 _tmp23F;_tmp23F.f1=0,({void*_tmp2D2=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp23F.f2=_tmp2D2;});_tmp23F;});}case 34U: _LL49: _tmp135=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL4A: {struct Cyc_Absyn_MallocInfo mi=_tmp135;
# 1083
if(mi.rgn != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain)).f2;
return({struct _tuple12 _tmp240;_tmp240.f1=0,({void*_tmp2D3=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain)).f2;_tmp240.f2=_tmp2D3;});_tmp240;});}case 35U: _LL4B: _tmp133=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_tmp134=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12E)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmp133;struct Cyc_Absyn_Exp*e2=_tmp134;
# 1087
struct _tuple12 _tmp1DE=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple12 _stmttmp40=_tmp1DE;struct _tuple12 _tmp1DF=_stmttmp40;void*_tmp1E1;void*_tmp1E0;_LLB2: _tmp1E0=_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;_LLB3: {void*lt1=_tmp1E0;void*a1=_tmp1E1;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple12 _tmp1E2=Cyc_Vcgen_vcgen_lexp(env,e2,a1);struct _tuple12 _stmttmp41=_tmp1E2;struct _tuple12 _tmp1E3=_stmttmp41;void*_tmp1E5;void*_tmp1E4;_LLB5: _tmp1E4=_tmp1E3.f1;_tmp1E5=_tmp1E3.f2;_LLB6: {void*lt2=_tmp1E4;void*a2=_tmp1E5;
Cyc_Vcgen_update_try_assn(env,a2);{
void*_tmp1E6=Cyc_Vcgen_deref_lterm(env,lt1,a2);void*t1=_tmp1E6;
void*_tmp1E7=Cyc_Vcgen_deref_lterm(env,lt2,a2);void*t2=_tmp1E7;
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple12 _tmp241;_tmp241.f1=0,_tmp241.f2=a2;_tmp241;});}}}}}case 37U: _LL4D: _tmp132=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL4E: {struct Cyc_Absyn_Stmt*s=_tmp132;
# 1097
void**_tmp1E8=(env->shared)->exp_stmt;void**old_exp_stmt=_tmp1E8;
({void**_tmp2D4=({void**_tmp1E9=_cycalloc(sizeof(*_tmp1E9));*_tmp1E9=0;_tmp1E9;});(env->shared)->exp_stmt=_tmp2D4;});{
void*_tmp1EA=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);void*aout=_tmp1EA;
void*_tmp1EB=*((void**)_check_null((env->shared)->exp_stmt));void*t=_tmp1EB;
(env->shared)->exp_stmt=old_exp_stmt;
return({struct _tuple12 _tmp242;_tmp242.f1=t,_tmp242.f2=aout;_tmp242;});}}case 40U: _LL4F: _LL50:
 return({struct _tuple12 _tmp243;_tmp243.f1=0,_tmp243.f2=ain;_tmp243;});case 41U: _LL51: _tmp131=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL52: {struct Cyc_Absyn_Exp*e=_tmp131;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2U: _LL53: _tmp130=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL54: {struct _fat_ptr s=_tmp130;
# 1106
if(({struct _fat_ptr _tmp2D5=(struct _fat_ptr)s;Cyc_strcmp(_tmp2D5,({const char*_tmp1EC="print_assn";_tag_fat(_tmp1EC,sizeof(char),11U);}));})){
struct _fat_ptr _tmp1ED=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp1ED;
({struct Cyc_String_pa_PrintArg_struct _tmp1F0=({struct Cyc_String_pa_PrintArg_struct _tmp245;_tmp245.tag=0U,_tmp245.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp245;});struct Cyc_String_pa_PrintArg_struct _tmp1F1=({struct Cyc_String_pa_PrintArg_struct _tmp244;_tmp244.tag=0U,({struct _fat_ptr _tmp2D6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_tmp244.f1=_tmp2D6;});_tmp244;});void*_tmp1EE[2U];_tmp1EE[0]=& _tmp1F0,_tmp1EE[1]=& _tmp1F1;({struct Cyc___cycFILE*_tmp2D8=Cyc_stderr;struct _fat_ptr _tmp2D7=({const char*_tmp1EF="%s: current assn is %s\n";_tag_fat(_tmp1EF,sizeof(char),24U);});Cyc_fprintf(_tmp2D8,_tmp2D7,_tag_fat(_tmp1EE,sizeof(void*),2U));});});}
# 1110
return({struct _tuple12 _tmp246;({void*_tmp2D9=Cyc_AssnDef_zero();_tmp246.f1=_tmp2D9;}),_tmp246.f2=ain;_tmp246;});}default: _LL55: _tmp12F=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp12E)->f1;_LL56: {struct Cyc_Absyn_Exp*e1=_tmp12F;
# 1112
struct _tuple12 _tmp1F2=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_AssnDef_true_assn);struct _tuple12 _stmttmp42=_tmp1F2;struct _tuple12 _tmp1F3=_stmttmp42;void*_tmp1F4;_LLB8: _tmp1F4=_tmp1F3.f1;_LLB9: {void*at=_tmp1F4;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp1F7=({struct Cyc_String_pa_PrintArg_struct _tmp247;_tmp247.tag=0U,({
struct _fat_ptr _tmp2DA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_tmp247.f1=_tmp2DA;});_tmp247;});void*_tmp1F5[1U];_tmp1F5[0]=& _tmp1F7;({unsigned _tmp2DC=e->loc;struct _fat_ptr _tmp2DB=({const char*_tmp1F6="cannot prove @assert from %s";_tag_fat(_tmp1F6,sizeof(char),29U);});Cyc_Warn_err(_tmp2DC,_tmp2DB,_tag_fat(_tmp1F5,sizeof(void*),1U));});});
return({struct _tuple12 _tmp248;({void*_tmp2DE=Cyc_AssnDef_one();_tmp248.f1=_tmp2DE;}),({void*_tmp2DD=Cyc_AssnDef_and(ain,at);_tmp248.f2=_tmp2DD;});_tmp248;});}}}_LL0:;}}
# 1122
void Cyc_Vcgen_vcgen_fundecl(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
struct Cyc_Vcgen_Env*_tmp1F8=Cyc_Vcgen_initial_env(tables,fd,assn_info);struct Cyc_Vcgen_Env*env=_tmp1F8;
void*a=(void*)& Cyc_AssnDef_true_assn;
# 1126
struct _tuple16 _tmp1F9=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);struct _tuple16 _stmttmp43=_tmp1F9;struct _tuple16 _tmp1FA=_stmttmp43;struct Cyc_Absyn_Vardecl*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_List_List*_tmp1FB;_LL1: _tmp1FB=_tmp1FA.f1;_tmp1FC=_tmp1FA.f2;_tmp1FD=_tmp1FA.f3;_tmp1FE=_tmp1FA.f4;_LL2: {struct Cyc_List_List*args=_tmp1FB;struct Cyc_Absyn_Exp*reqopt=_tmp1FC;struct Cyc_Absyn_Exp*ensopt=_tmp1FD;struct Cyc_Absyn_Vardecl*ret_value=_tmp1FE;
if(reqopt != 0)
# 1129
a=(Cyc_Vcgen_vcgen_test(env,reqopt,a)).f1;
# 1131
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 1133
if(ensopt != 0){
struct Cyc_Absyn_Exp*ens=ensopt;
struct _tuple12 _tmp1FF=Cyc_Vcgen_vcgen_test(env,ens,(void*)& Cyc_AssnDef_true_assn);struct _tuple12 _stmttmp44=_tmp1FF;struct _tuple12 _tmp200=_stmttmp44;void*_tmp201;_LL4: _tmp201=_tmp200.f1;_LL5: {void*at=_tmp201;
struct Cyc_Vcgen_SharedEnv*_tmp202=env->shared;struct Cyc_Vcgen_SharedEnv*senv=_tmp202;
void*_tmp203=senv->res_assn;void*res_assn_opt=_tmp203;
void*res_assn=(void*)& Cyc_AssnDef_false_assn;
if(res_assn_opt != 0)res_assn=res_assn_opt;{
struct _fat_ptr str=({const char*_tmp209="NULL";_tag_fat(_tmp209,sizeof(char),5U);});
if(senv->res_term != 0)str=Cyc_AssnDef_term2string((void*)_check_null(senv->res_term));
if(ret_value != 0 &&(unsigned)senv->res_term)
res_assn=({void*_tmp2E0=res_assn;Cyc_AssnDef_and(_tmp2E0,({void*_tmp2DF=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->tag=1U,_tmp204->f1=ret_value;_tmp204;});Cyc_AssnDef_eq(_tmp2DF,senv->res_term);}));});
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp207=({struct Cyc_String_pa_PrintArg_struct _tmp24A;_tmp24A.tag=0U,({
struct _fat_ptr _tmp2E1=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp24A.f1=_tmp2E1;});_tmp24A;});struct Cyc_String_pa_PrintArg_struct _tmp208=({struct Cyc_String_pa_PrintArg_struct _tmp249;_tmp249.tag=0U,({struct _fat_ptr _tmp2E2=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(res_assn)));_tmp249.f1=_tmp2E2;});_tmp249;});void*_tmp205[2U];_tmp205[0]=& _tmp207,_tmp205[1]=& _tmp208;({unsigned _tmp2E4=ens->loc;struct _fat_ptr _tmp2E3=({const char*_tmp206="cannot prove the @ensures clause %s from %s";_tag_fat(_tmp206,sizeof(char),44U);});Cyc_Warn_err(_tmp2E4,_tmp2E3,_tag_fat(_tmp205,sizeof(void*),2U));});});}}}
# 1148
Cyc_AssnDef_reset_hash_cons_table();}}

#include <setjmp.h>
/* This is a C header used by the output of the Cyclone to
   C translator.  Corresponding definitions are in file lib/runtime_*.c */
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
#define offsetof(t,n) ((int)(&(((t*)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Regions */
struct _RegionPage
{ 
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];
};

struct _pool;
struct bget_region_key;
struct _RegionAllocFunctions;

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  struct _RegionAllocFunctions *fcns;
  char               *offset;
  char               *last_plus_one;
  struct _pool *released_ptrs;
  struct bget_region_key *key;
#ifdef CYC_REGION_PROFILE
  const char *name;
#endif
  unsigned used_bytes;
  unsigned wasted_bytes;
};


// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

/* Alias qualifier stuff */
typedef unsigned int _AliasQualHandle_t; // must match aqualt_type() in toc.cyc

struct _RegionHandle _new_region(unsigned int, const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons*);
void _push_region(struct _RegionHandle*);
void _npop_handler(int);
void _pop_handler();
void _pop_region();


#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_assert_fn(const char *,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw_assert() (_throw_assert_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
struct Cyc_Assert_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];
extern char Cyc_Assert[];

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
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_other_fn(t_sz,orig_x,orig_sz,orig_i,f,l)((orig_x)+(orig_i))
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
void* _zero_arr_plus_other_fn(unsigned,void*,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
unsigned _get_zero_arr_size_char(const char*,unsigned);
unsigned _get_zero_arr_size_other(unsigned,const void*,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
// note: must cast result in toc.cyc
void* _zero_arr_inplace_plus_other_fn(unsigned,void**,int,const char*,unsigned);
void* _zero_arr_inplace_plus_post_other_fn(unsigned,void**,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char**)(x),(i),__FILE__,__LINE__)
#define _zero_arr_plus_other(t,x,s,i) \
  (_zero_arr_plus_other_fn(t,x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_other(t,x,i) \
  _zero_arr_inplace_plus_other_fn(t,(void**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_other(t,x,i) \
  _zero_arr_inplace_plus_post_other_fn(t,(void**)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char*)0) \
    _throw_arraybounds(); \
  _curr; })
#define _check_fat_at_base(arr) ({ \
  struct _fat_ptr _arr = (arr); \
  if (_arr.base != _arr.curr) _throw_arraybounds(); \
  _arr; })
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

#define _CYC_MAX_REGION_CONST 0
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, _AliasQualHandle_t aq, unsigned orig_s) {  
  if (r > (struct _RegionHandle*)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
  return _region_malloc(r,aq,orig_s); 
}

//doesn't make sense to fast malloc with reaps
#ifndef DISABLE_REAPS
#define _fast_region_malloc _region_malloc
#endif

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,unsigned,const char *,const char*,int);
void * _profile_aqual_malloc(_AliasQualHandle_t aq, unsigned int s,const char *file, const char *func, int lineno);
void * _profile_aqual_calloc(_AliasQualHandle_t aq, unsigned int t1,unsigned int t2,const char *file, const char *func, int lineno);
struct _RegionHandle _profile_new_region(unsigned int i, const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(i,n) _profile_new_region(i,n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_aqual_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_aqual_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif //CYC_REGION_PROFILE
#endif //_CYC_INCLUDE_H
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f0;void*f1;};
# 115 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);
# 321 "core.h"
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
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 37 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 531
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 563
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;int f3;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 939 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 974
extern void*Cyc_Absyn_void_type;
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1229
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1231
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1233
int Cyc_Absyn_is_require_union_type(void*);
# 30 "tcutil.h"
int Cyc_Tcutil_is_void_type(void*);
# 32
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 47
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 49
int Cyc_Tcutil_is_bound_one(void*,int);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 134 "tcutil.h"
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 159
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 186
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**,int*,void**);
# 195
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*,void*,int);
# 205
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*,void*);
# 242
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 268
struct Cyc_List_List*Cyc_Tcutil_get_aquals_bounds(struct Cyc_Absyn_Fndecl*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
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
# 40 "cf_flowinfo.h"
extern int Cyc_CfFlowInfo_anal_error;
extern void Cyc_CfFlowInfo_aerr(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 73
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;};
# 86
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;
extern struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_NotZero_val;
extern struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_UnknownZ_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 95
extern union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
extern union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void);struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;int f4;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _union_FlowInfo_ReachableFL{int tag;struct Cyc_Dict_Dict val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 143 "cf_flowinfo.h"
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void);
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 158
extern struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void);
# 160
extern int Cyc_CfFlowInfo_get_field_index(void*,struct _fat_ptr*);
extern int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*,struct _fat_ptr*);
# 163
extern int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 166
extern struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int,void*);
extern void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*,int,void*);
extern void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*,struct Cyc_Absyn_Exp*,int,void*,int);
extern int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*,int,void*,int*);
extern void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*,void*);struct _tuple13{void*f0;struct Cyc_List_List*f1;};
extern struct _tuple13 Cyc_CfFlowInfo_unname_rval(void*);
# 173
extern enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
extern void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*);
# 176
extern int Cyc_CfFlowInfo_is_init_pointer(void*);
extern int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 179
extern void Cyc_CfFlowInfo_print_absrval(void*);
# 186
extern void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo);
# 202 "cf_flowinfo.h"
extern struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
extern struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*,unsigned,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*,void*);
# 205
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_tryflow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f0;void*f1;};
extern struct _tuple14 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple14,struct _tuple14);
# 117 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*);
# 30 "warn.h"
extern int Cyc_Warn_num_errors;
# 37
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 66 "flags.h"
extern int Cyc_Flags_warn_lose_unique;
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 51 "new_control_flow.cyc"
static int Cyc_NewControlFlow_strcmp(struct _fat_ptr s1,struct _fat_ptr s2){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct _fat_ptr _T3;unsigned char*_T4;char*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;unsigned _T8;int _T9;unsigned _TA;unsigned _TB;struct _fat_ptr _TC;int _TD;char*_TE;const char*_TF;struct _fat_ptr _T10;int _T11;char*_T12;const char*_T13;char _T14;int _T15;char _T16;int _T17;char _T18;int _T19;char _T1A;int _T1B;char _T1C;int _T1D;int _T1E;struct _fat_ptr _T1F;int _T20;char*_T21;const char*_T22;char _T23;int _T24;struct _fat_ptr _T25;int _T26;char*_T27;const char*_T28;char _T29;int _T2A;_T0=s1;_T1=_T0.curr;_T2=(char*)_T1;_T3=s2;_T4=_T3.curr;_T5=(char*)_T4;
if(_T2!=_T5)goto _TL0;
return 0;_TL0: {
int i=0;_T6=s1;{
unsigned sz1=_get_fat_size(_T6,sizeof(char));_T7=s2;{
unsigned sz2=_get_fat_size(_T7,sizeof(char));
if(sz1 >= sz2)goto _TL2;_T8=sz1;goto _TL3;_TL2: _T8=sz2;_TL3: {unsigned minsz=_T8;
# 59
_TL4: _T9=i;_TA=(unsigned)_T9;_TB=minsz;if(_TA < _TB)goto _TL5;else{goto _TL6;}
_TL5: _TC=s1;_TD=i;_TE=_check_fat_subscript(_TC,sizeof(char),_TD);_TF=(const char*)_TE;{char c1=*_TF;_T10=s2;_T11=i;_T12=_check_fat_subscript(_T10,sizeof(char),_T11);_T13=(const char*)_T12;{
char c2=*_T13;_T14=c1;_T15=(int)_T14;
if(_T15!=0)goto _TL7;_T16=c2;_T17=(int)_T16;
if(_T17!=0)goto _TL9;return 0;
_TL9: return -1;
_TL7: _T18=c2;_T19=(int)_T18;if(_T19!=0)goto _TLB;return 1;
# 67
_TLB: _T1A=c1;_T1B=(int)_T1A;_T1C=c2;_T1D=(int)_T1C;{int diff=_T1B - _T1D;
if(diff==0)goto _TLD;_T1E=diff;return _T1E;_TLD:;}
# 70
i=i + 1;}}goto _TL4;_TL6:
# 72
 if(sz1!=sz2)goto _TLF;return 0;_TLF:
 if(minsz >= sz2)goto _TL11;_T1F=s2;_T20=i;_T21=_check_fat_subscript(_T1F,sizeof(char),_T20);_T22=(const char*)_T21;_T23=*_T22;_T24=(int)_T23;
if(_T24!=0)goto _TL13;return 0;
_TL13: return -1;
# 77
_TL11: _T25=s1;_T26=i;_T27=_check_fat_subscript(_T25,sizeof(char),_T26);_T28=(const char*)_T27;_T29=*_T28;_T2A=(int)_T29;if(_T2A!=0)goto _TL15;return 0;
_TL15: return 1;}}}}}struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*aquals_bounds;};struct _tuple15{void*f0;int f1;};
# 123 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple15*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f0;union Cyc_CfFlowInfo_AbsLVal f1;};
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int,int,struct Cyc_Absyn_Exp*);
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*,int);struct _tuple17{union Cyc_CfFlowInfo_FlowInfo f0;union Cyc_CfFlowInfo_FlowInfo f1;};
static struct _tuple17 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
# 129
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_List_List*);
# 133
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_T1;char*_T2;char*_T3;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_T4;struct Cyc_NewControlFlow_CFStmtAnnot*_T5;struct Cyc_NewControlFlow_CFStmtAnnot*_T6;struct Cyc_Absyn_Stmt*_T7;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_T8;struct Cyc_NewControlFlow_CFStmtAnnot*_T9;_T0=s;{
void*_TA=_T0->annot;void*_TB;_T1=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_TA;_T2=_T1->tag;_T3=Cyc_NewControlFlow_CFAnnot;if(_T2!=_T3)goto _TL17;{struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_TC=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_TA;_T4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_TA;_T5=& _T4->f1;_TB=(struct Cyc_NewControlFlow_CFStmtAnnot*)_T5;}{struct Cyc_NewControlFlow_CFStmtAnnot*x=(struct Cyc_NewControlFlow_CFStmtAnnot*)_TB;_T6=x;
return _T6;}_TL17: _T7=s;{struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_TC=_cycalloc(sizeof(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct));_TC->tag=Cyc_NewControlFlow_CFAnnot;
_TC->f1.visited=0;_T8=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_TC;}_T7->annot=(void*)_T8;_T9=Cyc_NewControlFlow_get_stmt_annot(s);return _T9;;}}
# 140
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_NewControlFlow_AnalEnv*_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Stmt*_T4;union Cyc_CfFlowInfo_FlowInfo*_T5;void(*_T6)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,union Cyc_CfFlowInfo_FlowInfo*);void(*_T7)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_NewControlFlow_AnalEnv*_T8;struct Cyc_Hashtable_Table*_T9;struct Cyc_Absyn_Stmt*_TA;union Cyc_CfFlowInfo_FlowInfo*_TB;union Cyc_CfFlowInfo_FlowInfo*_TC;union Cyc_CfFlowInfo_FlowInfo**_TD;union Cyc_CfFlowInfo_FlowInfo*_TE;_T1=Cyc_Hashtable_lookup_opt;{
union Cyc_CfFlowInfo_FlowInfo**(*_TF)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T1;_T0=_TF;}_T2=env;_T3=_T2->flow_table;_T4=s;{union Cyc_CfFlowInfo_FlowInfo**sflow=_T0(_T3,_T4);
if(sflow!=0)goto _TL19;{
union Cyc_CfFlowInfo_FlowInfo*res;res=_cycalloc(sizeof(union Cyc_CfFlowInfo_FlowInfo));_T5=res;*_T5=Cyc_CfFlowInfo_BottomFL();_T7=Cyc_Hashtable_insert;{
void(*_TF)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,union Cyc_CfFlowInfo_FlowInfo*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,union Cyc_CfFlowInfo_FlowInfo*))_T7;_T6=_TF;}_T8=env;_T9=_T8->flow_table;_TA=s;_TB=res;_T6(_T9,_TA,_TB);_TC=res;
return _TC;}_TL19: _TD=sflow;_TE=*_TD;
# 147
return _TE;}}struct _tuple18{struct Cyc_NewControlFlow_CFStmtAnnot*f0;union Cyc_CfFlowInfo_FlowInfo*f1;};
# 151
static struct _tuple18 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo*_T0;struct Cyc_NewControlFlow_AnalEnv*_T1;struct Cyc_CfFlowInfo_FlowEnv*_T2;union Cyc_CfFlowInfo_FlowInfo _T3;union Cyc_CfFlowInfo_FlowInfo*_T4;union Cyc_CfFlowInfo_FlowInfo _T5;struct Cyc_NewControlFlow_CFStmtAnnot*_T6;struct Cyc_NewControlFlow_AnalEnv*_T7;struct _tuple18 _T8;
struct Cyc_NewControlFlow_CFStmtAnnot*annot=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*sflow=Cyc_NewControlFlow_get_stmt_flow(env,s);_T0=sflow;_T1=env;_T2=_T1->fenv;_T3=inflow;_T4=sflow;_T5=*_T4;
*_T0=Cyc_CfFlowInfo_join_flow(_T2,_T3,_T5);_T6=annot;_T7=env;
# 160
_T6->visited=_T7->iteration_num;{struct _tuple18 _T9;
_T9.f0=annot;_T9.f1=sflow;_T8=_T9;}return _T8;}
# 170
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){struct Cyc_NewControlFlow_AnalEnv*_T0;int _T1;struct Cyc_NewControlFlow_AnalEnv*_T2;struct Cyc_NewControlFlow_AnalEnv*_T3;struct Cyc_CfFlowInfo_FlowEnv*_T4;union Cyc_CfFlowInfo_FlowInfo _T5;struct Cyc_NewControlFlow_AnalEnv*_T6;union Cyc_CfFlowInfo_FlowInfo _T7;_T0=env;_T1=_T0->in_try;
if(!_T1)goto _TL1B;_T2=env;_T3=env;_T4=_T3->fenv;_T5=new_flow;_T6=env;_T7=_T6->tryflow;
# 178
_T2->tryflow=Cyc_CfFlowInfo_join_tryflow(_T4,_T5,_T7);goto _TL1C;_TL1B: _TL1C:;}struct _tuple19{struct Cyc_NewControlFlow_AnalEnv*f0;unsigned f1;struct Cyc_Dict_Dict f2;};
# 185
static void Cyc_NewControlFlow_check_unique_root(struct _tuple19*ckenv,void*root,void*rval){struct _tuple19*_T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Dict_Dict _T5;void*_T6;void**_T7;int _T8;struct Cyc_NewControlFlow_AnalEnv*_T9;struct Cyc_List_List*_TA;struct Cyc_Absyn_Vardecl*_TB;void*_TC;int _TD;void*_TE;int*_TF;unsigned _T10;void*_T11;void*_T12;void*_T13;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T14;enum Cyc_CfFlowInfo_InitLevel _T15;struct Cyc_String_pa_PrintArg_struct _T16;struct Cyc_Absyn_Vardecl*_T17;struct _tuple2*_T18;void**_T19;unsigned _T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;struct Cyc_Dict_Dict _T1D;unsigned _T1E;struct Cyc_NewControlFlow_AnalEnv*_T1F;_T0=ckenv;{struct _tuple19 _T20=*_T0;_T1F=_T20.f0;_T1E=_T20.f1;_T1D=_T20.f2;}{struct Cyc_NewControlFlow_AnalEnv*env=_T1F;unsigned loc=_T1E;struct Cyc_Dict_Dict new_fd=_T1D;struct Cyc_Absyn_Vardecl*_T20;_T1=root;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL1D;_T4=root;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T21=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_T4;_T20=_T21->f1;}{struct Cyc_Absyn_Vardecl*vd=_T20;_T5=new_fd;_T6=root;_T7=& rval;_T8=
# 190
Cyc_Dict_lookup_bool(_T5,_T6,_T7);if(_T8)goto _TL1F;else{goto _TL21;}_TL21: _T9=env;_TA=_T9->aquals_bounds;_TB=vd;_TC=_TB->type;_TD=
Cyc_Tcutil_is_noalias_pointer_or_aggr(_TA,_TC);
# 190
if(!_TD)goto _TL1F;
# 192
retry:{void*_T21;_TE=rval;_TF=(int*)_TE;_T10=*_TF;switch(_T10){case 8: _T11=rval;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T22=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T11;_T12=_T22->f2;_T21=(void*)_T12;}{void*r=_T21;
rval=r;goto retry;}case 7: goto _LLE;case 2: _T13=rval;_T14=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T13;_T15=_T14->f1;if(_T15!=Cyc_CfFlowInfo_NoneIL)goto _TL23;_LLE: goto _LL10;_TL23: goto _LL11;case 0: _LL10: goto _LL8;default: _LL11:{struct Cyc_String_pa_PrintArg_struct _T22;_T22.tag=0;_T17=vd;_T18=_T17->name;
# 199
_T22.f1=Cyc_Absynpp_qvar2string(_T18);_T16=_T22;}{struct Cyc_String_pa_PrintArg_struct _T22=_T16;void*_T23[1];_T19=_T23 + 0;*_T19=& _T22;_T1A=loc;_T1B=_tag_fat("alias-free pointer(s) reachable from %s may become unreachable.",sizeof(char),64U);_T1C=_tag_fat(_T23,sizeof(void*),1);Cyc_Warn_warn(_T1A,_T1B,_T1C);}goto _LL8;}_LL8:;}goto _TL20;_TL1F: _TL20: goto _LL3;}_TL1D: goto _LL3;_LL3:;}}
# 211
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){struct Cyc_NewControlFlow_AnalEnv*_T0;struct Cyc_CfFlowInfo_FlowEnv*_T1;union Cyc_CfFlowInfo_FlowInfo _T2;union Cyc_CfFlowInfo_FlowInfo*_T3;union Cyc_CfFlowInfo_FlowInfo _T4;int _T5;struct _tuple17 _T6;union Cyc_CfFlowInfo_FlowInfo _T7;struct _union_FlowInfo_ReachableFL _T8;unsigned _T9;union Cyc_CfFlowInfo_FlowInfo _TA;struct _union_FlowInfo_ReachableFL _TB;unsigned _TC;union Cyc_CfFlowInfo_FlowInfo _TD;struct _union_FlowInfo_ReachableFL _TE;union Cyc_CfFlowInfo_FlowInfo _TF;struct _union_FlowInfo_ReachableFL _T10;struct _tuple19 _T11;struct Cyc_Absyn_Stmt*_T12;void(*_T13)(void(*)(struct _tuple19*,void*,void*),struct _tuple19*,struct Cyc_Dict_Dict);void(*_T14)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple19*_T15;struct _tuple19*_T16;struct Cyc_Dict_Dict _T17;union Cyc_CfFlowInfo_FlowInfo _T18;union Cyc_CfFlowInfo_FlowInfo*_T19;union Cyc_CfFlowInfo_FlowInfo _T1A;int _T1B;union Cyc_CfFlowInfo_FlowInfo*_T1C;struct Cyc_NewControlFlow_CFStmtAnnot*_T1D;int _T1E;struct Cyc_NewControlFlow_AnalEnv*_T1F;int _T20;struct Cyc_NewControlFlow_AnalEnv*_T21;
struct Cyc_NewControlFlow_CFStmtAnnot*annot=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*sflow=Cyc_NewControlFlow_get_stmt_flow(env,s);_T0=env;_T1=_T0->fenv;_T2=flow;_T3=sflow;_T4=*_T3;{
union Cyc_CfFlowInfo_FlowInfo new_flow=Cyc_CfFlowInfo_join_flow(_T1,_T2,_T4);_T5=Cyc_Flags_warn_lose_unique;
# 217
if(!_T5)goto _TL25;{struct _tuple17 _T22;
_T22.f0=flow;_T22.f1=new_flow;_T6=_T22;}{struct _tuple17 _T22=_T6;struct Cyc_Dict_Dict _T23;struct Cyc_Dict_Dict _T24;_T7=_T22.f0;_T8=_T7.ReachableFL;_T9=_T8.tag;if(_T9!=2)goto _TL27;_TA=_T22.f1;_TB=_TA.ReachableFL;_TC=_TB.tag;if(_TC!=2)goto _TL29;_TD=_T22.f0;_TE=_TD.ReachableFL;_T24=_TE.val;_TF=_T22.f1;_T10=_TF.ReachableFL;_T23=_T10.val;{struct Cyc_Dict_Dict fd=_T24;struct Cyc_Dict_Dict new_fd=_T23;{struct _tuple19 _T25;
# 220
_T25.f0=env;_T12=s;_T25.f1=_T12->loc;_T25.f2=new_fd;_T11=_T25;}{struct _tuple19 ckenv=_T11;_T14=Cyc_Dict_iter_c;{
void(*_T25)(void(*)(struct _tuple19*,void*,void*),struct _tuple19*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple19*,void*,void*),struct _tuple19*,struct Cyc_Dict_Dict))_T14;_T13=_T25;}_T15=& ckenv;_T16=(struct _tuple19*)_T15;_T17=fd;_T13(Cyc_NewControlFlow_check_unique_root,_T16,_T17);goto _LL0;}}_TL29: goto _LL3;_TL27: _LL3: goto _LL0;_LL0:;}goto _TL26;_TL25: _TL26: _T18=new_flow;_T19=sflow;_T1A=*_T19;_T1B=
# 225
Cyc_CfFlowInfo_flow_lessthan_approx(_T18,_T1A);if(_T1B)goto _TL2B;else{goto _TL2D;}
# 232
_TL2D: _T1C=sflow;*_T1C=new_flow;_T1D=annot;_T1E=_T1D->visited;_T1F=env;_T20=_T1F->iteration_num;
# 236
if(_T1E!=_T20)goto _TL2E;_T21=env;
# 238
_T21->iterate_again=1;goto _TL2F;_TL2E: _TL2F: goto _TL2C;_TL2B: _TL2C:;}}
# 243
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned loc,int nameit){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_ReachableFL _T5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T6;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_CfFlowInfo_FlowEnv*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;void*_TF;int _T10;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T11;struct Cyc_List_List*_T12;void*_T13;struct Cyc_Dict_Dict _T14;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T15;void*_T16;void*_T17;struct Cyc_List_List*_T18;union Cyc_CfFlowInfo_FlowInfo _T19;struct Cyc_Dict_Dict _T1A;_T0=inflow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL30;_T3=
# 248
Cyc_CfFlowInfo_BottomFL();return _T3;_TL30: _T4=inflow;_T5=_T4.ReachableFL;_T1A=_T5.val;{struct Cyc_Dict_Dict d=_T1A;
# 250
_TL35: if(vds!=0)goto _TL33;else{goto _TL34;}
_TL33:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_T6=root;_T6->tag=0;_T7=root;_T8=vds;_T9=_T8->hd;_T7->f1=(struct Cyc_Absyn_Vardecl*)_T9;_TA=fenv;_TB=vds;_TC=_TB->hd;_TD=(struct Cyc_Absyn_Vardecl*)_TC;_TE=_TD->type;_TF=leafval;{
void*rval=Cyc_CfFlowInfo_typ_to_absrval(_TA,_TE,0,_TF);_T10=nameit;
if(!_T10)goto _TL36;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T1B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T1B->tag=8;_T12=vds;_T13=_T12->hd;
_T1B->f1=(struct Cyc_Absyn_Vardecl*)_T13;_T1B->f2=rval;_T11=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T1B;}rval=(void*)_T11;goto _TL37;_TL36: _TL37: _T14=d;_T15=root;_T16=(void*)_T15;_T17=rval;
# 257
d=Cyc_Dict_insert(_T14,_T16,_T17);}}_T18=vds;
# 250
vds=_T18->tl;goto _TL35;_TL34: _T19=
# 259
Cyc_CfFlowInfo_ReachableFL(d);return _T19;};}
# 263
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_Absyn_Exp*_T4;_T0=e;{
void*_T5=_T0->r;struct Cyc_Absyn_Exp*_T6;_T1=(int*)_T5;_T2=*_T1;if(_T2!=14)goto _TL38;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T5;_T6=_T7->f2;}{struct Cyc_Absyn_Exp*e1=_T6;_T3=e1;
return _T3;}_TL38: _T4=e;
return _T4;;}}
# 270
static void Cyc_NewControlFlow_check_union_requires(unsigned loc,void*t,struct _fat_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;_T0=inflow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL3A;
# 273
return;_TL3A:
# 275
 return;;}
# 279
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_ReachableFL _T5;struct Cyc_NewControlFlow_AnalEnv*_T6;struct Cyc_CfFlowInfo_FlowEnv*_T7;struct Cyc_Dict_Dict _T8;void*_T9;enum Cyc_CfFlowInfo_InitLevel _TA;int _TB;unsigned _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_NewControlFlow_AnalEnv*_TF;struct Cyc_CfFlowInfo_FlowEnv*_T10;struct Cyc_Dict_Dict _T11;void*_T12;struct Cyc_Dict_Dict _T13;const struct Cyc_Dict_T*_T14;struct Cyc_Dict_Dict _T15;const struct Cyc_Dict_T*_T16;union Cyc_CfFlowInfo_FlowInfo _T17;union Cyc_CfFlowInfo_FlowInfo _T18;struct Cyc_Dict_Dict _T19;_T0=inflow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL3C;_T3=
# 281
Cyc_CfFlowInfo_BottomFL();return _T3;_TL3C: _T4=inflow;_T5=_T4.ReachableFL;_T19=_T5.val;{struct Cyc_Dict_Dict d=_T19;_T6=env;_T7=_T6->fenv;_T8=d;_T9=r;_TA=
# 283
Cyc_CfFlowInfo_initlevel(_T7,_T8,_T9);_TB=(int)_TA;if(_TB==1)goto _TL3E;_TC=loc;_TD=
_tag_fat("expression may not be fully initialized",sizeof(char),40U);_TE=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_TC,_TD,_TE);goto _TL3F;_TL3E: _TL3F: _TF=env;_T10=_TF->fenv;_T11=d;_T12=r;{
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(_T10,_T11,_T12);_T13=d;_T14=_T13.t;_T15=ans_d;_T16=_T15.t;
if(_T14!=_T16)goto _TL40;_T17=inflow;return _T17;_TL40: {
union Cyc_CfFlowInfo_FlowInfo ans=Cyc_CfFlowInfo_ReachableFL(ans_d);
Cyc_NewControlFlow_update_tryflow(env,ans);_T18=ans;
return _T18;}}};}struct _tuple20{struct Cyc_Absyn_Tqual f0;void*f1;};
# 293
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,void*r){struct _tuple0 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T5;enum Cyc_CfFlowInfo_InitLevel _T6;void*_T7;int*_T8;unsigned _T9;void*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;void*_TC;int*_TD;unsigned _TE;void*_TF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10;void*_T11;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T12;union Cyc_Absyn_DatatypeFieldInfo _T13;struct _union_DatatypeFieldInfo_KnownDatatypefield _T14;unsigned _T15;void*_T16;void*_T17;void*_T18;int*_T19;unsigned _T1A;void*_T1B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1C;void*_T1D;int*_T1E;unsigned _T1F;void*_T20;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T21;void*_T22;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T23;union Cyc_Absyn_DatatypeFieldInfo _T24;struct _union_DatatypeFieldInfo_KnownDatatypefield _T25;unsigned _T26;void*_T27;int*_T28;int _T29;void*_T2A;void*_T2B;union Cyc_Absyn_DatatypeFieldInfo _T2C;struct _union_DatatypeFieldInfo_KnownDatatypefield _T2D;struct _tuple3 _T2E;union Cyc_Absyn_DatatypeFieldInfo _T2F;struct _union_DatatypeFieldInfo_KnownDatatypefield _T30;struct _tuple3 _T31;void*_T32;struct Cyc_Absyn_Datatypefield*_T33;struct Cyc_List_List*_T34;struct _RegionHandle*_T35;struct _RegionHandle*_T36;struct Cyc_Absyn_Datatypedecl*_T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;struct Cyc_Absyn_Datatypefield*_T3A;int _T3B;unsigned _T3C;struct _fat_ptr _T3D;unsigned _T3E;struct Cyc_List_List*_T3F;void*_T40;struct Cyc_NewControlFlow_AnalEnv*_T41;unsigned _T42;void*_T43;struct _fat_ptr _T44;unsigned char*_T45;void**_T46;int _T47;void*_T48;struct Cyc_List_List*_T49;void*_T4A;int*_T4B;int _T4C;void*_T4D;void*_T4E;void*_T4F;struct Cyc_Absyn_Aggrdecl*_T50;struct Cyc_Absyn_AggrdeclImpl*_T51;struct Cyc_Absyn_Aggrdecl*_T52;struct Cyc_Absyn_AggrdeclImpl*_T53;struct _RegionHandle*_T54;struct _RegionHandle*_T55;struct Cyc_Absyn_Aggrdecl*_T56;struct Cyc_List_List*_T57;struct Cyc_List_List*_T58;int _T59;unsigned _T5A;struct _fat_ptr _T5B;unsigned _T5C;struct Cyc_List_List*_T5D;void*_T5E;struct Cyc_Absyn_Aggrfield*_T5F;struct Cyc_NewControlFlow_AnalEnv*_T60;unsigned _T61;void*_T62;struct _fat_ptr _T63;unsigned char*_T64;void**_T65;int _T66;void*_T67;struct Cyc_List_List*_T68;void*_T69;int*_T6A;int _T6B;void*_T6C;void*_T6D;int _T6E;unsigned _T6F;struct _fat_ptr _T70;unsigned _T71;struct Cyc_NewControlFlow_AnalEnv*_T72;unsigned _T73;struct Cyc_List_List*_T74;void*_T75;struct Cyc_Absyn_Aggrfield*_T76;void*_T77;struct _fat_ptr _T78;unsigned char*_T79;void**_T7A;int _T7B;void*_T7C;struct Cyc_List_List*_T7D;struct Cyc_NewControlFlow_AnalEnv*_T7E;struct Cyc_List_List*_T7F;void*_T80;int _T81;unsigned _T82;struct _fat_ptr _T83;struct _fat_ptr _T84;{struct _tuple0 _T85;
_T85.f0=Cyc_Absyn_compress(t);_T85.f1=r;_T0=_T85;}{struct _tuple0 _T85=_T0;enum Cyc_Absyn_AggrKind _T86;union Cyc_Absyn_AggrInfo _T87;struct _fat_ptr _T88;struct Cyc_List_List*_T89;struct Cyc_Absyn_Datatypefield*_T8A;struct Cyc_Absyn_Datatypedecl*_T8B;void*_T8C;_T1=_T85.f1;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 2: _T4=_T85.f1;_T5=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T4;_T6=_T5->f1;if(_T6!=Cyc_CfFlowInfo_NoneIL)goto _TL43;
return;_TL43: _T7=_T85.f0;_T8=(int*)_T7;_T9=*_T8;switch(_T9){case 0: _TA=_T85.f0;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA;_TC=_TB->f1;_TD=(int*)_TC;_TE=*_TD;switch(_TE){case 23: _TF=_T85.f0;_T10=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF;_T11=_T10->f1;_T12=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T11;_T13=_T12->f1;_T14=_T13.KnownDatatypefield;_T15=_T14.tag;if(_T15!=2)goto _TL47;goto _LL11;_TL47: goto _LL11;case 24: goto _LL11;default: goto _LL11;};case 7: goto _LL11;case 4: goto _LLF;default: goto _LL11;};case 0:
 return;case 7:
 return;case 8: _T16=_T85.f1;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T8D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T16;_T17=_T8D->f2;_T8C=(void*)_T17;}{void*r=_T8C;
Cyc_NewControlFlow_check_nounique(env,loc,t,r);return;}default: _T18=_T85.f0;_T19=(int*)_T18;_T1A=*_T19;switch(_T1A){case 0: _T1B=_T85.f0;_T1C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1B;_T1D=_T1C->f1;_T1E=(int*)_T1D;_T1F=*_T1E;switch(_T1F){case 23: _T20=_T85.f0;_T21=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T20;_T22=_T21->f1;_T23=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T22;_T24=_T23->f1;_T25=_T24.KnownDatatypefield;_T26=_T25.tag;if(_T26!=2)goto _TL4B;_T27=_T85.f1;_T28=(int*)_T27;_T29=*_T28;if(_T29!=6)goto _TL4D;_T2A=_T85.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2A;_T2B=_T8D->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T8E=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T2B;_T2C=_T8E->f1;_T2D=_T2C.KnownDatatypefield;_T2E=_T2D.val;_T8B=_T2E.f0;_T2F=_T8E->f1;_T30=_T2F.KnownDatatypefield;_T31=_T30.val;_T8A=_T31.f1;}_T89=_T8D->f2;}_T32=_T85.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T8D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T32;_T88=_T8D->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T8B;struct Cyc_Absyn_Datatypefield*tuf=_T8A;struct Cyc_List_List*targs=_T89;struct _fat_ptr ad=_T88;_T33=tuf;_T34=_T33->typs;
# 300
if(_T34!=0)goto _TL4F;
return;_TL4F: {struct _RegionHandle _T8D=_new_region(0U,"temp");struct _RegionHandle*temp=& _T8D;_push_region(temp);_T35=temp;_T36=temp;_T37=tud;_T38=_T37->tvs;_T39=targs;{
# 303
struct Cyc_List_List*inst=Cyc_List_rzip(_T35,_T36,_T38,_T39);_T3A=tuf;{
struct Cyc_List_List*fs=_T3A->typs;{
int i=0;_TL54: _T3B=i;_T3C=(unsigned)_T3B;_T3D=ad;_T3E=_get_fat_size(_T3D,sizeof(void*));if(_T3C < _T3E)goto _TL52;else{goto _TL53;}
_TL52: _T3F=_check_null(fs);_T40=_T3F->hd;{struct _tuple20*_T8E=(struct _tuple20*)_T40;void*_T8F;{struct _tuple20 _T90=*_T8E;_T8F=_T90.f1;}{void*t=_T8F;
if(inst==0)goto _TL55;t=Cyc_Tcutil_rsubstitute(temp,inst,t);goto _TL56;_TL55: _TL56: _T41=env;_T42=loc;_T43=t;_T44=ad;_T45=_T44.curr;_T46=(void**)_T45;_T47=i;_T48=_T46[_T47];
Cyc_NewControlFlow_check_nounique(_T41,_T42,_T43,_T48);}}
# 305
i=i + 1;_T49=fs;fs=_T49->tl;goto _TL54;_TL53:;}_npop_handler(0);return;}}_pop_region();}}goto _TL4E;_TL4D: goto _LL11;_TL4E: goto _TL4C;_TL4B: goto _LL11;_TL4C:;case 24: _T4A=_T85.f1;_T4B=(int*)_T4A;_T4C=*_T4B;if(_T4C!=6)goto _TL57;_T4D=_T85.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4D;_T4E=_T8D->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T8E=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4E;_T87=_T8E->f1;}_T89=_T8D->f2;}_T4F=_T85.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T8D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T4F;_T88=_T8D->f2;}{union Cyc_Absyn_AggrInfo info=_T87;struct Cyc_List_List*targs=_T89;struct _fat_ptr d=_T88;
# 314
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T50=ad;_T51=_T50->impl;
if(_T51!=0)goto _TL59;return;_TL59: _T52=ad;_T53=_T52->impl;{
struct Cyc_List_List*fs=_T53->fields;struct _RegionHandle _T8D=_new_region(0U,"temp");struct _RegionHandle*temp=& _T8D;_push_region(temp);_T54=temp;_T55=temp;_T56=ad;_T57=_T56->tvs;_T58=targs;{
# 318
struct Cyc_List_List*inst=Cyc_List_rzip(_T54,_T55,_T57,_T58);{
int i=0;_TL5E: _T59=i;_T5A=(unsigned)_T59;_T5B=d;_T5C=_get_fat_size(_T5B,sizeof(void*));if(_T5A < _T5C)goto _TL5C;else{goto _TL5D;}
_TL5C: _T5D=_check_null(fs);_T5E=_T5D->hd;_T5F=(struct Cyc_Absyn_Aggrfield*)_T5E;{void*t=_T5F->type;
if(inst==0)goto _TL5F;t=Cyc_Tcutil_rsubstitute(temp,inst,t);goto _TL60;_TL5F: _TL60: _T60=env;_T61=loc;_T62=t;_T63=d;_T64=_T63.curr;_T65=(void**)_T64;_T66=i;_T67=_T65[_T66];
Cyc_NewControlFlow_check_nounique(_T60,_T61,_T62,_T67);}
# 319
i=i + 1;_T68=fs;fs=_T68->tl;goto _TL5E;_TL5D:;}_npop_handler(0);return;}_pop_region();}}goto _TL58;_TL57: goto _LL11;_TL58:;default: goto _LL11;};case 7: _T69=_T85.f1;_T6A=(int*)_T69;_T6B=*_T6A;if(_T6B!=6)goto _TL61;_T6C=_T85.f0;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T6C;_T86=_T8D->f1;_T89=_T8D->f3;}_T6D=_T85.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T8D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T6D;_T88=_T8D->f2;}{enum Cyc_Absyn_AggrKind k=_T86;struct Cyc_List_List*fs=_T89;struct _fat_ptr d=_T88;{
# 327
int i=0;_TL66: _T6E=i;_T6F=(unsigned)_T6E;_T70=d;_T71=_get_fat_size(_T70,sizeof(void*));if(_T6F < _T71)goto _TL64;else{goto _TL65;}
_TL64: _T72=env;_T73=loc;_T74=_check_null(fs);_T75=_T74->hd;_T76=(struct Cyc_Absyn_Aggrfield*)_T75;_T77=_T76->type;_T78=d;_T79=_T78.curr;_T7A=(void**)_T79;_T7B=i;_T7C=_T7A[_T7B];Cyc_NewControlFlow_check_nounique(_T72,_T73,_T77,_T7C);
# 327
i=i + 1;_T7D=fs;fs=_T7D->tl;goto _TL66;_TL65:;}
# 329
return;}_TL61: goto _LL11;case 4: _LLF: _T7E=env;_T7F=_T7E->aquals_bounds;_T80=t;_T81=
# 331
Cyc_Tcutil_is_noalias_pointer(_T7F,_T80,0);if(!_T81)goto _TL67;_T82=loc;_T83=
_tag_fat("argument may still contain alias-free pointers",sizeof(char),47U);_T84=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T82,_T83,_T84);goto _TL68;_TL67: _TL68:
 return;default: _LL11:
 return;};};}}
# 338
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_ReachableFL _T5;struct Cyc_NewControlFlow_AnalEnv*_T6;struct Cyc_List_List*_T7;void*_T8;int _T9;int(*_TA)(struct _fat_ptr,struct _fat_ptr);void*(*_TB)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC;struct _fat_ptr _TD;void*_TE;int*_TF;unsigned _T10;void*_T11;void*_T12;void*_T13;void*_T14;void*_T15;struct Cyc_NewControlFlow_AnalEnv*_T16;struct Cyc_List_List*_T17;void*_T18;int _T19;unsigned _T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;union Cyc_CfFlowInfo_FlowInfo _T1D;struct Cyc_NewControlFlow_AnalEnv*_T1E;struct Cyc_CfFlowInfo_FlowEnv*_T1F;struct Cyc_Dict_Dict _T20;void*_T21;struct Cyc_Dict_Dict _T22;const struct Cyc_Dict_T*_T23;struct Cyc_Dict_Dict _T24;const struct Cyc_Dict_T*_T25;union Cyc_CfFlowInfo_FlowInfo _T26;union Cyc_CfFlowInfo_FlowInfo _T27;struct Cyc_Dict_Dict _T28;_T0=inflow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL69;_T3=
# 340
Cyc_CfFlowInfo_BottomFL();return _T3;_TL69: _T4=inflow;_T5=_T4.ReachableFL;_T28=_T5.val;{struct Cyc_Dict_Dict d=_T28;_T6=env;_T7=_T6->aquals_bounds;_T8=t;_T9=
# 342
Cyc_Tcutil_is_noalias_pointer(_T7,_T8,0);if(_T9)goto _TL6B;else{goto _TL6D;}
_TL6D: _TB=Cyc_Warn_impos;{int(*_T29)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB;_TA=_T29;}_TC=_tag_fat("noliveunique attribute requires alias-free pointer",sizeof(char),51U);_TD=_tag_fat(0U,sizeof(void*),0);_TA(_TC,_TD);goto _TL6C;_TL6B: _TL6C: {
void*elt_type=Cyc_Tcutil_pointer_elt_type(t);
retry:{void*_T29;_TE=r;_TF=(int*)_TE;_T10=*_TF;switch(_T10){case 8: _T11=r;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T2A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T11;_T12=_T2A->f2;_T29=(void*)_T12;}{void*r2=_T29;
r=r2;goto retry;}case 4: _T13=r;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T2A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T13;_T29=_T2A->f1;}{struct Cyc_CfFlowInfo_Place*p=_T29;_T29=
Cyc_CfFlowInfo_lookup_place(d,p);goto _LLB;}case 5: _T14=r;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T2A=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T14;_T15=_T2A->f1;_T29=(void*)_T15;}_LLB: {void*r=_T29;
Cyc_NewControlFlow_check_nounique(env,loc,elt_type,r);goto _LL5;}default: _T16=env;_T17=_T16->aquals_bounds;_T18=elt_type;_T19=
# 350
Cyc_Tcutil_is_noalias_pointer_or_aggr(_T17,_T18);if(!_T19)goto _TL6F;_T1A=loc;_T1B=
_tag_fat("argument may contain live alias-free pointers",sizeof(char),46U);_T1C=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T1A,_T1B,_T1C);goto _TL70;_TL6F: _TL70: _T1D=
Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);return _T1D;}_LL5:;}_T1E=env;_T1F=_T1E->fenv;_T20=d;_T21=r;{
# 354
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(_T1F,_T20,_T21);_T22=d;_T23=_T22.t;_T24=ans_d;_T25=_T24.t;
if(_T23!=_T25)goto _TL71;_T26=inflow;return _T26;_TL71: {
union Cyc_CfFlowInfo_FlowInfo ans=Cyc_CfFlowInfo_ReachableFL(ans_d);
Cyc_NewControlFlow_update_tryflow(env,ans);_T27=ans;
return _T27;}}}};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f0;struct Cyc_List_List*f1;};
# 364
static struct _tuple21 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){struct _tuple21 _T0;struct Cyc_NewControlFlow_AnalEnv*_T1;int _T2;union Cyc_CfFlowInfo_FlowInfo _T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct _tuple21 _T9;
# 370
struct Cyc_List_List*rvals=0;
if(es!=0)goto _TL73;{struct _tuple21 _TA;
_TA.f0=inflow;_TA.f1=0;_T0=_TA;}return _T0;_TL73:
# 374
 _TL75: _T1=env;_T2=first_is_copy;_T3=inflow;_T4=es;_T5=_T4->hd;_T6=(struct Cyc_Absyn_Exp*)_T5;{struct _tuple14 _TA=Cyc_NewControlFlow_anal_Rexp(_T1,_T2,_T3,_T6,0);void*_TB;union Cyc_CfFlowInfo_FlowInfo _TC;_TC=_TA.f0;_TB=_TA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TC;void*r=_TB;
inflow=f;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));
_TD->hd=r;_TD->tl=rvals;_T7=(struct Cyc_List_List*)_TD;}rvals=_T7;_T8=es;
es=_T8->tl;
first_is_copy=others_are_copy;}}
# 373
if(es!=0)goto _TL75;else{goto _TL76;}_TL76:
# 381
 Cyc_NewControlFlow_update_tryflow(env,inflow);{struct _tuple21 _TA;
_TA.f0=inflow;_TA.f1=Cyc_List_imp_rev(rvals);_T9=_TA;}return _T9;}
# 387
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_ReachableFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;struct _union_FlowInfo_ReachableFL _T4;struct Cyc_NewControlFlow_AnalEnv*_T5;struct Cyc_CfFlowInfo_FlowEnv*_T6;struct Cyc_Dict_Dict _T7;struct Cyc_List_List*_T8;void*_T9;enum Cyc_CfFlowInfo_InitLevel _TA;int _TB;struct Cyc_List_List*_TC;void*_TD;struct Cyc_Absyn_Exp*_TE;unsigned _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct _tuple14 _T14;struct Cyc_NewControlFlow_AnalEnv*_T15;struct Cyc_CfFlowInfo_FlowEnv*_T16;
# 389
struct _tuple21 _T17=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);struct Cyc_List_List*_T18;union Cyc_CfFlowInfo_FlowInfo _T19;_T19=_T17.f0;_T18=_T17.f1;{union Cyc_CfFlowInfo_FlowInfo afterflow=_T19;struct Cyc_List_List*rvals=_T18;{struct Cyc_Dict_Dict _T1A;_T0=afterflow;_T1=_T0.ReachableFL;_T2=_T1.tag;if(_T2!=2)goto _TL77;_T3=afterflow;_T4=_T3.ReachableFL;_T1A=_T4.val;{struct Cyc_Dict_Dict d=_T1A;
# 393
_TL7C: if(rvals!=0)goto _TL7A;else{goto _TL7B;}
_TL7A: _T5=env;_T6=_T5->fenv;_T7=d;_T8=rvals;_T9=_T8->hd;_TA=Cyc_CfFlowInfo_initlevel(_T6,_T7,_T9);_TB=(int)_TA;if(_TB!=0)goto _TL7D;_TC=
_check_null(es);_TD=_TC->hd;_TE=(struct Cyc_Absyn_Exp*)_TD;_TF=_TE->loc;_T10=_tag_fat("expression may not be initialized",sizeof(char),34U);_T11=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_TF,_T10,_T11);goto _TL7E;_TL7D: _TL7E: _T12=rvals;
# 393
rvals=_T12->tl;_T13=_check_null(es);es=_T13->tl;goto _TL7C;_TL7B: goto _LL3;}_TL77: goto _LL3;_LL3:;}{struct _tuple14 _T1A;
# 399
_T1A.f0=afterflow;_T15=env;_T16=_T15->fenv;_T1A.f1=_T16->unknown_all;_T14=_T1A;}return _T14;}}
# 406
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){struct Cyc_Absyn_Exp*_T0;struct Cyc_NewControlFlow_AnalEnv*_T1;int _T2;void*_T3;int*_T4;int _T5;int(*_T6)(struct _fat_ptr,struct _fat_ptr);void*(*_T7)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;struct Cyc_NewControlFlow_AnalEnv*_TB;struct Cyc_CfFlowInfo_FlowEnv*_TC;struct Cyc_NewControlFlow_AnalEnv*_TD;struct Cyc_List_List*_TE;struct Cyc_Absyn_Exp*_TF;void*_T10;void*_T11;struct Cyc_Absyn_Exp*_T12;struct Cyc_NewControlFlow_AnalEnv*_T13;int _T14;void*_T15;void*_T16;void*_T17;
# 413
int needs_unconsume=0;
void*old_rval=Cyc_CfFlowInfo_lookup_place(new_dict,p);_T0=e;_T1=env;_T2=_T1->iteration_num;_T3=old_rval;_T4=& needs_unconsume;_T5=
Cyc_CfFlowInfo_is_unique_consumed(_T0,_T2,_T3,_T4);if(!_T5)goto _TL7F;_T7=Cyc_Warn_impos;{
int(*_T18)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7;_T6=_T18;}_T8=_tag_fat("consume_zero_rval",sizeof(char),18U);_T9=_tag_fat(0U,sizeof(void*),0);_T6(_T8,_T9);goto _TL80;
# 418
_TL7F: _TA=needs_unconsume;if(!_TA)goto _TL81;_TB=env;_TC=_TB->fenv;_TD=env;_TE=_TD->aquals_bounds;_TF=e;_T10=_TF->topt;_T11=
_check_null(_T10);_T12=e;_T13=env;_T14=_T13->iteration_num;_T15=new_rval;_T16=Cyc_CfFlowInfo_make_unique_consumed(_TC,_TE,_T11,_T12,_T14,_T15,0);return _T16;
# 422
_TL81: _T17=new_rval;return _T17;_TL80:;}
# 435 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_ReachableFL _T5;struct _tuple16 _T6;struct _union_AbsLVal_UnknownL _T7;unsigned _T8;union Cyc_CfFlowInfo_FlowInfo _T9;struct _union_AbsLVal_PlaceL _TA;void*_TB;enum Cyc_CfFlowInfo_InitLevel _TC;int _TD;struct Cyc_NewControlFlow_AnalEnv*_TE;struct Cyc_CfFlowInfo_FlowEnv*_TF;struct Cyc_NewControlFlow_AnalEnv*_T10;struct Cyc_CfFlowInfo_FlowEnv*_T11;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_List_List*_T15;struct Cyc_NewControlFlow_AnalEnv*_T16;struct Cyc_CfFlowInfo_FlowEnv*_T17;struct Cyc_Absyn_Exp*_T18;unsigned _T19;struct Cyc_Dict_Dict _T1A;struct Cyc_CfFlowInfo_Place*_T1B;void*_T1C;struct Cyc_Dict_Dict _T1D;union Cyc_CfFlowInfo_FlowInfo _T1E;struct Cyc_Dict_Dict _T1F;_T0=outflow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL83;_T3=outflow;
# 441
return _T3;_TL83: _T4=outflow;_T5=_T4.ReachableFL;_T1F=_T5.val;{struct Cyc_Dict_Dict d=_T1F;_T6=
# 443
Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e);{union Cyc_CfFlowInfo_AbsLVal _T20=_T6.f1;struct Cyc_CfFlowInfo_Place*_T21;_T7=_T20.UnknownL;_T8=_T7.tag;if(_T8!=2)goto _TL85;_T9=outflow;
# 447
return _T9;_TL85: _TA=_T20.PlaceL;_T21=_TA.val;{struct Cyc_CfFlowInfo_Place*p=_T21;_TC=il;_TD=(int)_TC;
# 451
if(_TD!=1)goto _TL87;_TE=env;_TF=_TE->fenv;_TB=_TF->notzeroall;goto _TL88;_TL87: _T10=env;_T11=_T10->fenv;_TB=_T11->unknown_none;_TL88: {void*nzval=_TB;
_TL8C: if(names!=0)goto _TL8A;else{goto _TL8B;}
_TL8A:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T22=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T22->tag=8;_T13=names;_T14=_T13->hd;_T22->f1=(struct Cyc_Absyn_Vardecl*)_T14;_T22->f2=nzval;_T12=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T22;}nzval=(void*)_T12;_T15=names;
# 452
names=_T15->tl;goto _TL8C;_TL8B:
# 455
 nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);_T16=env;_T17=_T16->fenv;_T18=e;_T19=_T18->loc;_T1A=d;_T1B=p;_T1C=nzval;_T1D=
Cyc_CfFlowInfo_assign_place(_T17,_T19,_T1A,_T1B,_T1C);{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_ReachableFL(_T1D);_T1E=outflow;
# 460
return _T1E;}}};}};}
# 469
static struct _tuple17 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;struct _tuple17 _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_ReachableFL _T5;struct _tuple16 _T6;struct _union_AbsLVal_UnknownL _T7;unsigned _T8;struct _tuple17 _T9;struct _union_AbsLVal_PlaceL _TA;void*_TB;enum Cyc_CfFlowInfo_InitLevel _TC;int _TD;struct Cyc_NewControlFlow_AnalEnv*_TE;struct Cyc_CfFlowInfo_FlowEnv*_TF;struct Cyc_NewControlFlow_AnalEnv*_T10;struct Cyc_CfFlowInfo_FlowEnv*_T11;struct Cyc_NewControlFlow_AnalEnv*_T12;struct Cyc_CfFlowInfo_FlowEnv*_T13;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T17;struct Cyc_List_List*_T18;void*_T19;struct Cyc_List_List*_T1A;struct _tuple17 _T1B;struct Cyc_NewControlFlow_AnalEnv*_T1C;struct Cyc_CfFlowInfo_FlowEnv*_T1D;struct Cyc_Absyn_Exp*_T1E;unsigned _T1F;struct Cyc_Dict_Dict _T20;struct Cyc_CfFlowInfo_Place*_T21;void*_T22;struct Cyc_Dict_Dict _T23;struct Cyc_NewControlFlow_AnalEnv*_T24;struct Cyc_CfFlowInfo_FlowEnv*_T25;struct Cyc_Absyn_Exp*_T26;unsigned _T27;struct Cyc_Dict_Dict _T28;struct Cyc_CfFlowInfo_Place*_T29;void*_T2A;struct Cyc_Dict_Dict _T2B;struct Cyc_Dict_Dict _T2C;_T0=outflow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL8D;{struct _tuple17 _T2D;
# 472
_T2D.f0=outflow;_T2D.f1=outflow;_T3=_T2D;}return _T3;_TL8D: _T4=outflow;_T5=_T4.ReachableFL;_T2C=_T5.val;{struct Cyc_Dict_Dict d=_T2C;_T6=
# 474
Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e);{union Cyc_CfFlowInfo_AbsLVal _T2D=_T6.f1;struct Cyc_CfFlowInfo_Place*_T2E;_T7=_T2D.UnknownL;_T8=_T7.tag;if(_T8!=2)goto _TL8F;{struct _tuple17 _T2F;
_T2F.f0=outflow;_T2F.f1=outflow;_T9=_T2F;}return _T9;_TL8F: _TA=_T2D.PlaceL;_T2E=_TA.val;{struct Cyc_CfFlowInfo_Place*p=_T2E;_TC=il;_TD=(int)_TC;
# 477
if(_TD!=1)goto _TL91;_TE=env;_TF=_TE->fenv;_TB=_TF->notzeroall;goto _TL92;_TL91: _T10=env;_T11=_T10->fenv;_TB=_T11->unknown_none;_TL92: {void*nzval=_TB;_T12=env;_T13=_T12->fenv;{
void*zval=_T13->zero;
_TL96: if(names!=0)goto _TL94;else{goto _TL95;}
_TL94:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T2F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T2F->tag=8;_T15=names;_T16=_T15->hd;_T2F->f1=(struct Cyc_Absyn_Vardecl*)_T16;_T2F->f2=nzval;_T14=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T2F;}nzval=(void*)_T14;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T2F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T2F->tag=8;_T18=names;_T19=_T18->hd;
_T2F->f1=(struct Cyc_Absyn_Vardecl*)_T19;_T2F->f2=zval;_T17=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T2F;}zval=(void*)_T17;_T1A=names;
# 479
names=_T1A->tl;goto _TL96;_TL95:
# 483
 nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,zval);{struct _tuple17 _T2F;_T1C=env;_T1D=_T1C->fenv;_T1E=e;_T1F=_T1E->loc;_T20=d;_T21=p;_T22=nzval;_T23=
# 486
Cyc_CfFlowInfo_assign_place(_T1D,_T1F,_T20,_T21,_T22);_T2F.f0=Cyc_CfFlowInfo_ReachableFL(_T23);_T24=env;_T25=_T24->fenv;_T26=e;_T27=_T26->loc;_T28=d;_T29=p;_T2A=zval;_T2B=
Cyc_CfFlowInfo_assign_place(_T25,_T27,_T28,_T29,_T2A);_T2F.f1=Cyc_CfFlowInfo_ReachableFL(_T2B);_T1B=_T2F;}
# 485
return _T1B;}}};}};}
# 492
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ};
# 501
static struct _tuple14 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_Absyn_Exp*index){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;int*_T3;int _T4;struct Cyc_Absyn_PtrInfo _T5;struct Cyc_Absyn_PtrInfo _T6;struct Cyc_Absyn_PtrAtts _T7;union Cyc_CfFlowInfo_FlowInfo _T8;struct _union_FlowInfo_BottomFL _T9;unsigned _TA;struct _tuple14 _TB;struct Cyc_NewControlFlow_AnalEnv*_TC;struct Cyc_CfFlowInfo_FlowEnv*_TD;void*_TE;struct Cyc_NewControlFlow_AnalEnv*_TF;struct Cyc_CfFlowInfo_FlowEnv*_T10;void*_T11;union Cyc_CfFlowInfo_FlowInfo _T12;struct _union_FlowInfo_ReachableFL _T13;void*_T14;int*_T15;unsigned _T16;void*_T17;void*_T18;int(*_T19)(struct _fat_ptr,struct _fat_ptr);void*(*_T1A)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T1B;struct _fat_ptr _T1C;struct Cyc_Absyn_Exp*_T1D;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T1E;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T1F;void*_T20;struct Cyc_Absyn_Exp*_T21;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T22;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T23;int _T24;struct _tuple14 _T25;void*_T26;void*_T27;struct Cyc_Absyn_Exp*_T28;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T29;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T2A;int _T2B;struct _tuple14 _T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_T2E;struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_T2F;struct _tuple14 _T30;struct Cyc_NewControlFlow_AnalEnv*_T31;struct Cyc_CfFlowInfo_FlowEnv*_T32;void*_T33;struct Cyc_NewControlFlow_AnalEnv*_T34;struct Cyc_CfFlowInfo_FlowEnv*_T35;void*_T36;void*_T37;struct Cyc_Absyn_Exp*_T38;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T39;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T3A;struct Cyc_NewControlFlow_AnalEnv*_T3B;struct Cyc_CfFlowInfo_FlowEnv*_T3C;struct Cyc_Dict_Dict _T3D;void*_T3E;void*_T3F;int*_T40;int _T41;struct Cyc_Absyn_Exp*_T42;unsigned _T43;struct _fat_ptr _T44;struct _fat_ptr _T45;struct Cyc_Absyn_Exp*_T46;unsigned _T47;struct _fat_ptr _T48;struct _fat_ptr _T49;struct _tuple14 _T4A;struct Cyc_NewControlFlow_AnalEnv*_T4B;struct Cyc_CfFlowInfo_FlowEnv*_T4C;void*_T4D;struct Cyc_NewControlFlow_AnalEnv*_T4E;struct Cyc_CfFlowInfo_FlowEnv*_T4F;void*_T50;int(*_T51)(struct _fat_ptr,struct _fat_ptr);void*(*_T52)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T53;struct _fat_ptr _T54;_T0=e;_T1=_T0->topt;_T2=
# 505
_check_null(_T1);{void*_T55=Cyc_Absyn_compress(_T2);void*_T56;void*_T57;_T3=(int*)_T55;_T4=*_T3;if(_T4!=4)goto _TL97;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T58=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T55;_T5=_T58->f1;_T57=_T5.elt_type;_T6=_T58->f1;_T7=_T6.ptr_atts;_T56=_T7.bounds;}{void*elttype=_T57;void*bd=_T56;struct Cyc_Dict_Dict _T58;_T8=f;_T9=_T8.BottomFL;_TA=_T9.tag;if(_TA!=1)goto _TL99;{struct _tuple14 _T59;
# 509
_T59.f0=f;_TC=env;_TD=_TC->fenv;_TE=elttype;_TF=env;_T10=_TF->fenv;_T11=_T10->unknown_all;_T59.f1=Cyc_CfFlowInfo_typ_to_absrval(_TD,_TE,0,_T11);_TB=_T59;}return _TB;_TL99: _T12=f;_T13=_T12.ReachableFL;_T58=_T13.val;{struct Cyc_Dict_Dict outdict=_T58;
# 511
struct _tuple13 _T59=Cyc_CfFlowInfo_unname_rval(r);struct Cyc_List_List*_T5A;void*_T5B;_T5B=_T59.f0;_T5A=_T59.f1;{void*r=_T5B;struct Cyc_List_List*names=_T5A;{enum Cyc_CfFlowInfo_InitLevel _T5C;void*_T5D;void*_T5E;_T14=r;_T15=(int*)_T14;_T16=*_T15;switch(_T16){case 8: _T17=r;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T5F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T17;_T5E=_T5F->f1;_T18=_T5F->f2;_T5D=(void*)_T18;}{struct Cyc_Absyn_Vardecl*n=_T5E;void*r2=_T5D;_T1A=Cyc_Warn_impos;{
# 514
int(*_T5F)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T1A;_T19=_T5F;}_T1B=_tag_fat("named location in anal_derefR",sizeof(char),30U);_T1C=_tag_fat(0U,sizeof(void*),0);_T19(_T1B,_T1C);}case 1: _T1D=e;_T1E=& Cyc_CfFlowInfo_NotZero_val;_T1F=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_T1E;
# 516
_T1D->annot=(void*)_T1F;goto _LLD;case 4: _T20=r;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T5F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T20;_T5E=_T5F->f1;}{struct Cyc_CfFlowInfo_Place*p=_T5E;_T21=e;_T22=& Cyc_CfFlowInfo_NotZero_val;_T23=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_T22;
# 519
_T21->annot=(void*)_T23;
if(index!=0)goto _TL9C;_T24=Cyc_Tcutil_is_bound_one(bd,0);if(!_T24)goto _TL9C;{struct _tuple14 _T5F;
_T5F.f0=f;_T5F.f1=Cyc_CfFlowInfo_lookup_place(outdict,p);_T25=_T5F;}return _T25;_TL9C: goto _LLD;}case 5: _T26=r;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T5F=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T26;_T27=_T5F->f1;_T5E=(void*)_T27;}{void*r=_T5E;_T28=e;_T29=& Cyc_CfFlowInfo_UnknownZ_val;_T2A=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_T29;
# 525
_T28->annot=(void*)_T2A;
if(index!=0)goto _TL9E;_T2B=Cyc_Tcutil_is_bound_one(bd,0);if(!_T2B)goto _TL9E;{struct _tuple14 _T5F;
_T5F.f0=f;_T5F.f1=r;_T2C=_T5F;}return _T2C;_TL9E: goto _LLD;}case 0: _T2D=e;_T2E=& Cyc_CfFlowInfo_IsZero_val;_T2F=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_T2E;
# 530
_T2D->annot=(void*)_T2F;{struct _tuple14 _T5F;
_T5F.f0=Cyc_CfFlowInfo_BottomFL();_T31=env;_T32=_T31->fenv;_T33=elttype;_T34=env;_T35=_T34->fenv;_T36=_T35->unknown_all;_T5F.f1=Cyc_CfFlowInfo_typ_to_absrval(_T32,_T33,0,_T36);_T30=_T5F;}return _T30;case 2: _T37=r;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T5F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T37;_T5C=_T5F->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_T5C;
# 534
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);goto _LL1B;}default: _LL1B: _T38=e;_T39=& Cyc_CfFlowInfo_UnknownZ_val;_T3A=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_T39;
# 537
_T38->annot=(void*)_T3A;goto _LLD;}_LLD:;}_T3B=env;_T3C=_T3B->fenv;_T3D=outdict;_T3E=r;{
# 540
enum Cyc_CfFlowInfo_InitLevel _T5C=Cyc_CfFlowInfo_initlevel(_T3C,_T3D,_T3E);if(_T5C!=Cyc_CfFlowInfo_NoneIL)goto _TLA0;{
# 542
struct _tuple13 _T5D=Cyc_CfFlowInfo_unname_rval(r);void*_T5E;_T5E=_T5D.f0;{void*r=_T5E;_T3F=r;_T40=(int*)_T3F;_T41=*_T40;if(_T41!=7)goto _TLA2;_T42=e;_T43=_T42->loc;_T44=
# 545
_tag_fat("attempt to dereference a consumed alias-free pointer",sizeof(char),53U);_T45=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T43,_T44,_T45);goto _LL24;_TLA2: _T46=e;_T47=_T46->loc;_T48=
# 548
_tag_fat("dereference of possibly uninitialized pointer",sizeof(char),46U);_T49=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T47,_T48,_T49);_LL24: goto _LL20;}}_TLA0: _LL20:{struct _tuple14 _T5D;
# 552
_T5D.f0=f;_T4B=env;_T4C=_T4B->fenv;_T4D=elttype;_T4E=env;_T4F=_T4E->fenv;_T50=_T4F->unknown_all;_T5D.f1=Cyc_CfFlowInfo_typ_to_absrval(_T4C,_T4D,0,_T50);_T4A=_T5D;}return _T4A;;}}};}goto _TL98;_TL97: _T52=Cyc_Warn_impos;{
# 555
int(*_T58)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T52;_T51=_T58;}_T53=_tag_fat("right deref of non-pointer-type",sizeof(char),32U);_T54=_tag_fat(0U,sizeof(void*),0);_T51(_T53,_T54);_TL98:;}}
# 565
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned loc,void*old_rval){struct _tuple16 _T0;union Cyc_CfFlowInfo_FlowInfo _T1;struct _union_FlowInfo_ReachableFL _T2;unsigned _T3;union Cyc_CfFlowInfo_AbsLVal _T4;struct _union_AbsLVal_PlaceL _T5;unsigned _T6;union Cyc_CfFlowInfo_FlowInfo _T7;struct _union_FlowInfo_ReachableFL _T8;union Cyc_CfFlowInfo_AbsLVal _T9;struct _union_AbsLVal_PlaceL _TA;struct Cyc_NewControlFlow_AnalEnv*_TB;struct Cyc_CfFlowInfo_FlowEnv*_TC;struct Cyc_Absyn_Exp*_TD;void*_TE;void*_TF;struct Cyc_NewControlFlow_AnalEnv*_T10;struct Cyc_CfFlowInfo_FlowEnv*_T11;void*_T12;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_List_List*_T16;struct Cyc_NewControlFlow_AnalEnv*_T17;struct Cyc_CfFlowInfo_FlowEnv*_T18;unsigned _T19;struct Cyc_Dict_Dict _T1A;struct Cyc_CfFlowInfo_Place*_T1B;void*_T1C;union Cyc_CfFlowInfo_AbsLVal _T1D;struct _union_AbsLVal_UnknownL _T1E;union Cyc_CfFlowInfo_FlowInfo _T1F;
# 570
struct _tuple16 _T20=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _T21;_T21=_T20.f1;{union Cyc_CfFlowInfo_AbsLVal lval=_T21;{struct _tuple16 _T22;
_T22.f0=inflow;_T22.f1=lval;_T0=_T22;}{struct _tuple16 _T22=_T0;int _T23;struct Cyc_CfFlowInfo_Place*_T24;struct Cyc_Dict_Dict _T25;_T1=_T22.f0;_T2=_T1.ReachableFL;_T3=_T2.tag;if(_T3!=2)goto _TLA4;_T4=_T22.f1;_T5=_T4.PlaceL;_T6=_T5.tag;if(_T6!=1)goto _TLA6;_T7=_T22.f0;_T8=_T7.ReachableFL;_T25=_T8.val;_T9=_T22.f1;_TA=_T9.PlaceL;_T24=_TA.val;{struct Cyc_Dict_Dict fd=_T25;struct Cyc_CfFlowInfo_Place*p=_T24;_TB=env;_TC=_TB->fenv;_TD=exp;_TE=_TD->topt;_TF=
# 573
_check_null(_TE);_T10=env;_T11=_T10->fenv;_T12=_T11->unknown_all;{void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_TC,_TF,0,_T12);
# 575
struct _tuple13 _T26=Cyc_CfFlowInfo_unname_rval(old_rval);struct Cyc_List_List*_T27;void*_T28;_T28=_T26.f0;_T27=_T26.f1;{void*old_rval=_T28;struct Cyc_List_List*names=_T27;
_TLAB: if(names!=0)goto _TLA9;else{goto _TLAA;}
_TLA9:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T29=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T29->tag=8;_T14=names;_T15=_T14->hd;_T29->f1=(struct Cyc_Absyn_Vardecl*)_T15;_T29->f2=new_rval;_T13=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T29;}new_rval=(void*)_T13;_T16=names;
# 576
names=_T16->tl;goto _TLAB;_TLAA: _T17=env;_T18=_T17->fenv;_T19=loc;_T1A=fd;_T1B=p;_T1C=new_rval;
# 580
fd=Cyc_CfFlowInfo_assign_place(_T18,_T19,_T1A,_T1B,_T1C);
inflow=Cyc_CfFlowInfo_ReachableFL(fd);
Cyc_NewControlFlow_update_tryflow(env,inflow);goto _LL3;}}}_TLA6: _T1D=_T22.f1;_T1E=_T1D.UnknownL;_T23=_T1E.val;{int u=_T23;goto _LL3;}_TLA4: goto _LL3;_LL3:;}_T1F=inflow;
# 596
return _T1F;}}
# 601
static struct _tuple14 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;struct _tuple14 _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_ReachableFL _T5;union Cyc_CfFlowInfo_AbsLVal _T6;struct _union_AbsLVal_PlaceL _T7;unsigned _T8;union Cyc_CfFlowInfo_AbsLVal _T9;struct _union_AbsLVal_PlaceL _TA;int _TB;struct Cyc_NewControlFlow_AnalEnv*_TC;struct Cyc_List_List*_TD;struct Cyc_Absyn_Exp*_TE;void*_TF;int _T10;void*_T11;void*_T12;int*_T13;unsigned _T14;void*_T15;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T16;enum Cyc_CfFlowInfo_InitLevel _T17;struct Cyc_Absyn_Exp*_T18;unsigned _T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;struct _tuple14 _T1C;struct _tuple14 _T1D;struct Cyc_NewControlFlow_AnalEnv*_T1E;struct Cyc_Absyn_Exp*_T1F;unsigned _T20;union Cyc_CfFlowInfo_FlowInfo _T21;void*_T22;struct Cyc_Dict_Dict _T23;_T0=outflow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TLAC;{struct _tuple14 _T24;
# 610
_T24.f0=Cyc_CfFlowInfo_BottomFL();_T24.f1=rval;_T3=_T24;}return _T3;_TLAC: _T4=outflow;_T5=_T4.ReachableFL;_T23=_T5.val;{struct Cyc_Dict_Dict indict=_T23;struct Cyc_CfFlowInfo_Place*_T24;_T6=lval;_T7=_T6.PlaceL;_T8=_T7.tag;if(_T8!=1)goto _TLAE;_T9=lval;_TA=_T9.PlaceL;_T24=_TA.val;{struct Cyc_CfFlowInfo_Place*p=_T24;
# 620
struct Cyc_Dict_Dict outdict=Cyc_CfFlowInfo_assign_place(fenv,loc,indict,p,rval);
outflow=Cyc_CfFlowInfo_ReachableFL(outdict);_TB=Cyc_Flags_warn_lose_unique;
if(!_TB)goto _TLB0;_TC=env;_TD=_TC->aquals_bounds;_TE=lexp;_TF=_TE->topt;_T10=
Cyc_Tcutil_is_noalias_pointer_or_aggr(_TD,_TF);
# 622
if(!_T10)goto _TLB0;_T11=
# 625
Cyc_CfFlowInfo_lookup_place(indict,p);{struct _tuple13 _T25=Cyc_CfFlowInfo_unname_rval(_T11);void*_T26;_T26=_T25.f0;{void*rv=_T26;_T12=rv;_T13=(int*)_T12;_T14=*_T13;switch(_T14){case 2: _T15=rv;_T16=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T15;_T17=_T16->f1;if(_T17!=Cyc_CfFlowInfo_NoneIL)goto _TLB3;goto _LL11;_TLB3: goto _LL14;case 7: _LL11: goto _LL13;case 0: _LL13: goto _LLD;default: _LL14: _T18=lexp;_T19=_T18->loc;_T1A=
# 631
_tag_fat("assignment may overwrite alias-free pointer(s)",sizeof(char),47U);_T1B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T19,_T1A,_T1B);goto _LLD;}_LLD:;}}goto _TLB1;_TLB0: _TLB1:
# 636
 Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple14 _T25;
_T25.f0=outflow;_T25.f1=rval;_T1C=_T25;}return _T1C;}_TLAE:{struct _tuple14 _T25;_T1E=env;_T1F=rexp;_T20=_T1F->loc;_T21=outflow;_T22=rval;
# 642
_T25.f0=Cyc_NewControlFlow_use_Rval(_T1E,_T20,_T21,_T22);_T25.f1=rval;_T1D=_T25;}return _T1D;;};}
# 649
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_ReachableFL _T5;struct Cyc_CfFlowInfo_FlowEnv*_T6;unsigned _T7;struct Cyc_Dict_Dict _T8;struct Cyc_CfFlowInfo_Place*_T9;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_TA;void*_TB;union Cyc_CfFlowInfo_FlowInfo _TC;struct Cyc_Dict_Dict _TD;_T0=f;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TLB5;_T3=
# 656
Cyc_CfFlowInfo_BottomFL();return _T3;_TLB5: _T4=f;_T5=_T4.ReachableFL;_TD=_T5.val;{struct Cyc_Dict_Dict outdict=_TD;_T6=fenv;_T7=loc;_T8=outdict;{struct Cyc_CfFlowInfo_Place*_TE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_TF=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_TF->tag=0;
# 661
_TF->f1=vd;_TA=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_TF;}_TE->root=(void*)_TA;_TE->path=0;_T9=(struct Cyc_CfFlowInfo_Place*)_TE;}_TB=rval;
# 660
outdict=Cyc_CfFlowInfo_assign_place(_T6,_T7,_T8,_T9,_TB);{
# 663
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_ReachableFL(outdict);
Cyc_NewControlFlow_update_tryflow(env,outflow);_TC=outflow;
# 666
return _TC;}};}struct _tuple22{struct Cyc_Absyn_Vardecl**f0;struct Cyc_Absyn_Exp*f1;};
# 670
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned pat_loc,unsigned exp_loc,int isAliasPat){union Cyc_CfFlowInfo_FlowInfo _T0;struct _tuple1 _T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;void*_T4;struct _tuple22*_T5;struct _tuple22 _T6;struct Cyc_Absyn_Vardecl**_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple22*_TB;struct _tuple22 _TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_List_List*_TE;struct Cyc_CfFlowInfo_FlowEnv*_TF;union Cyc_CfFlowInfo_FlowInfo _T10;struct Cyc_List_List*_T11;struct Cyc_CfFlowInfo_FlowEnv*_T12;void*_T13;unsigned _T14;int _T15;struct Cyc_NewControlFlow_AnalEnv*_T16;union Cyc_CfFlowInfo_FlowInfo _T17;struct Cyc_List_List*_T18;void*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;void*_T1D;struct Cyc_Absyn_Exp*_T1E;void*_T1F;struct Cyc_String_pa_PrintArg_struct _T20;void**_T21;int(*_T22)(struct _fat_ptr,struct _fat_ptr);void*(*_T23)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_List_List _T26;struct Cyc_NewControlFlow_AnalEnv*_T27;union Cyc_CfFlowInfo_FlowInfo _T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;union Cyc_CfFlowInfo_FlowInfo _T2B;struct _union_FlowInfo_ReachableFL _T2C;unsigned _T2D;union Cyc_CfFlowInfo_FlowInfo _T2E;struct _union_FlowInfo_ReachableFL _T2F;int _T30;union Cyc_CfFlowInfo_AbsLVal _T31;struct _union_AbsLVal_PlaceL _T32;unsigned _T33;union Cyc_CfFlowInfo_AbsLVal _T34;struct _union_AbsLVal_PlaceL _T35;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T36;struct Cyc_Absyn_Vardecl**_T37;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T38;struct Cyc_Absyn_Vardecl**_T39;struct Cyc_NewControlFlow_AnalEnv*_T3A;struct Cyc_List_List*_T3B;struct Cyc_Absyn_Exp*_T3C;void*_T3D;void*_T3E;int _T3F;struct Cyc_NewControlFlow_AnalEnv*_T40;struct Cyc_List_List*_T41;struct Cyc_Absyn_Vardecl**_T42;struct Cyc_Absyn_Vardecl*_T43;void*_T44;int _T45;struct Cyc_String_pa_PrintArg_struct _T46;void**_T47;unsigned _T48;struct _fat_ptr _T49;struct _fat_ptr _T4A;struct Cyc_CfFlowInfo_FlowEnv*_T4B;struct Cyc_NewControlFlow_AnalEnv*_T4C;union Cyc_CfFlowInfo_FlowInfo _T4D;struct Cyc_Absyn_Vardecl**_T4E;struct Cyc_Absyn_Vardecl*_T4F;struct Cyc_Absyn_Exp*_T50;void*_T51;unsigned _T52;struct Cyc_List_List*_T53;union Cyc_CfFlowInfo_FlowInfo _T54;
# 677
if(vds!=0)goto _TLB7;_T0=inflow;return _T0;_TLB7: _T1=
# 680
Cyc_List_split(vds);_T2=_T1.f0;{struct Cyc_List_List*vs=Cyc_Tcutil_filter_nulls(_T2);
struct Cyc_List_List*es=0;{
struct Cyc_List_List*x=vds;_TLBC: if(x!=0)goto _TLBA;else{goto _TLBB;}
_TLBA: _T3=x;_T4=_T3->hd;_T5=(struct _tuple22*)_T4;_T6=*_T5;_T7=_T6.f0;if(_T7!=0)goto _TLBD;{struct Cyc_List_List*_T55=_cycalloc(sizeof(struct Cyc_List_List));_T9=x;_TA=_T9->hd;_TB=(struct _tuple22*)_TA;_TC=*_TB;_TD=_TC.f1;_T55->hd=_check_null(_TD);_T55->tl=es;_T8=(struct Cyc_List_List*)_T55;}es=_T8;goto _TLBE;_TLBD: _TLBE: _TE=x;
# 682
x=_TE->tl;goto _TLBC;_TLBB:;}_TF=fenv;_T10=inflow;_T11=vs;_T12=fenv;_T13=_T12->unknown_all;_T14=pat_loc;_T15=name_locs;
# 686
inflow=Cyc_NewControlFlow_add_vars(_TF,_T10,_T11,_T13,_T14,_T15);
# 688
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);{
struct Cyc_List_List*x=es;_TLC2: if(x!=0)goto _TLC0;else{goto _TLC1;}
# 692
_TLC0: _T16=env;_T17=inflow;_T18=x;_T19=_T18->hd;_T1A=(struct Cyc_Absyn_Exp*)_T19;{struct _tuple14 _T55=Cyc_NewControlFlow_anal_Rexp(_T16,1,_T17,_T1A,0);void*_T56;union Cyc_CfFlowInfo_FlowInfo _T57;_T57=_T55.f0;_T56=_T55.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T57;void*r=_T56;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,f,r);}}_T1B=x;
# 689
x=_T1B->tl;goto _TLC2;_TLC1:;}{
# 700
struct Cyc_List_List*_T55=Cyc_List_rev(vds);struct Cyc_List_List*vds=_T55;
_TLC6: if(vds!=0)goto _TLC4;else{goto _TLC5;}
_TLC4: _T1C=vds;_T1D=_T1C->hd;{struct _tuple22*_T56=(struct _tuple22*)_T1D;struct Cyc_Absyn_Exp*_T57;struct Cyc_Absyn_Vardecl**_T58;{struct _tuple22 _T59=*_T56;_T58=_T59.f0;_T57=_T59.f1;}{struct Cyc_Absyn_Vardecl**vd=_T58;struct Cyc_Absyn_Exp*ve=_T57;
if(vd==0)goto _TLC7;if(ve==0)goto _TLC7;_T1E=ve;_T1F=_T1E->topt;
if(_T1F!=0)goto _TLC9;{struct Cyc_String_pa_PrintArg_struct _T59;_T59.tag=0;
# 706
_T59.f1=Cyc_Absynpp_exp2string(ve);_T20=_T59;}{struct Cyc_String_pa_PrintArg_struct _T59=_T20;void*_T5A[1];_T21=_T5A + 0;*_T21=& _T59;_T23=Cyc_Warn_impos;{
# 705
int(*_T5B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T23;_T22=_T5B;}_T24=_tag_fat("oops! pattern init expr %s has no type!\n",sizeof(char),41U);_T25=_tag_fat(_T5A,sizeof(void*),1);_T22(_T24,_T25);}goto _TLCA;_TLC9: _TLCA:{struct Cyc_List_List _T59;
# 714
_T59.hd=ve;_T59.tl=0;_T26=_T59;}{struct Cyc_List_List l=_T26;_T27=env;_T28=inflow;_T29=& l;_T2A=(struct Cyc_List_List*)_T29;{
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_expand_unique_places(_T27,_T28,_T2A);
struct _tuple16 _T59=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,ve);union Cyc_CfFlowInfo_AbsLVal _T5A;_T5A=_T59.f1;{union Cyc_CfFlowInfo_AbsLVal lval=_T5A;
struct _tuple14 _T5B=Cyc_NewControlFlow_anal_Rexp(env,1,f,ve,isAliasPat);void*_T5C;union Cyc_CfFlowInfo_FlowInfo _T5D;_T5D=_T5B.f0;_T5C=_T5B.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T5D;void*rval=_T5C;struct Cyc_Dict_Dict _T5E;_T2B=f;_T2C=_T2B.ReachableFL;_T2D=_T2C.tag;if(_T2D!=2)goto _TLCB;_T2E=f;_T2F=_T2E.ReachableFL;_T5E=_T2F.val;{struct Cyc_Dict_Dict fd=_T5E;_T30=name_locs;
# 720
if(!_T30)goto _TLCD;{struct Cyc_CfFlowInfo_Place*_T5F;_T31=lval;_T32=_T31.PlaceL;_T33=_T32.tag;if(_T33!=1)goto _TLCF;_T34=lval;_T35=_T34.PlaceL;_T5F=_T35.val;{struct Cyc_CfFlowInfo_Place*p=_T5F;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T60=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T60->tag=8;_T37=vd;
# 723
_T60->f1=*_T37;_T60->f2=rval;_T36=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T60;}rval=(void*)_T36;{
# 726
void*new_rval=Cyc_CfFlowInfo_lookup_place(fd,p);{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T60=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T60->tag=8;_T39=vd;
_T60->f1=*_T39;_T60->f2=new_rval;_T38=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T60;}new_rval=(void*)_T38;
fd=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd);goto _LL11;}}_TLCF: _T3A=env;_T3B=_T3A->aquals_bounds;_T3C=ve;_T3D=_T3C->topt;_T3E=
# 733
_check_null(_T3D);_T3F=Cyc_Tcutil_is_noalias_pointer_or_aggr(_T3B,_T3E);if(!_T3F)goto _TLD1;_T40=env;_T41=_T40->aquals_bounds;_T42=vd;_T43=*_T42;_T44=_T43->type;_T45=
Cyc_Tcutil_is_noalias_pointer_or_aggr(_T41,_T44);
# 733
if(_T45)goto _TLD1;else{goto _TLD3;}
# 735
_TLD3:{struct Cyc_String_pa_PrintArg_struct _T60;_T60.tag=0;
_T60.f1=Cyc_Absynpp_exp2string(ve);_T46=_T60;}{struct Cyc_String_pa_PrintArg_struct _T60=_T46;void*_T61[1];_T47=_T61 + 0;*_T47=& _T60;_T48=exp_loc;_T49=
# 735
_tag_fat("aliased pattern expression not an l-value: %s",sizeof(char),46U);_T4A=_tag_fat(_T61,sizeof(void*),1);Cyc_CfFlowInfo_aerr(_T48,_T49,_T4A);}goto _TLD2;_TLD1: _TLD2: _LL11:;}goto _TLCE;_TLCD: _TLCE: _T4B=fenv;_T4C=env;_T4D=f;_T4E=vd;_T4F=*_T4E;_T50=ve;_T51=rval;_T52=pat_loc;
# 743
inflow=Cyc_NewControlFlow_do_initialize_var(_T4B,_T4C,_T4D,_T4F,_T50,_T51,_T52);goto _LLC;}_TLCB: goto _LLC;_LLC:;}}}}goto _TLC8;_TLC7: _TLC8:;}}_T53=vds;
# 701
vds=_T53->tl;goto _TLC6;_TLC5: _T54=inflow;
# 751
return _T54;}}}
# 754
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;int _T7;int _T8;int _T9;_T0=e;{
# 756
void*_TA=_T0->r;struct Cyc_Absyn_Exp*_TB;_T1=(int*)_TA;_T2=*_T1;switch(_T2){case 1: _T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TA;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 4: goto _LL4;case 3: _LL4: goto _LL6;case 5: _LL6:
# 759
 return 1;default: goto _LL11;};case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_TC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_TA;_TB=_TC->f1;}{struct Cyc_Absyn_Exp*e=_TB;_TB=e;goto _LLA;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_TC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_TA;_TB=_TC->f1;}_LLA: {struct Cyc_Absyn_Exp*e=_TB;_TB=e;goto _LLC;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_TC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_TA;_TB=_TC->f1;}_LLC: {struct Cyc_Absyn_Exp*e=_TB;_T7=
# 763
Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);return _T7;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_TC=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_TA;_TB=_TC->f1;}{struct Cyc_Absyn_Exp*e=_TB;
# 765
return 0;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TC=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TA;_TB=_TC->f2;}{struct Cyc_Absyn_Exp*e=_TB;_T8=cast_ok;
# 767
if(!_T8)goto _TLD6;_T9=Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);return _T9;
_TLD6: return 0;}default: _LL11:
 return 0;};}}struct _tuple23{union Cyc_CfFlowInfo_AbsLVal f0;union Cyc_CfFlowInfo_FlowInfo f1;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 774
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,int local_alias){struct Cyc_NewControlFlow_AnalEnv*_T0;union Cyc_CfFlowInfo_FlowInfo _T1;struct _union_FlowInfo_BottomFL _T2;unsigned _T3;struct _tuple14 _T4;struct Cyc_CfFlowInfo_FlowEnv*_T5;union Cyc_CfFlowInfo_FlowInfo _T6;struct _union_FlowInfo_ReachableFL _T7;int _T8;int _T9;struct Cyc_NewControlFlow_AnalEnv*_TA;struct Cyc_List_List*_TB;struct Cyc_Absyn_Exp*_TC;void*_TD;int _TE;struct _tuple16 _TF;union Cyc_CfFlowInfo_FlowInfo _T10;struct _union_FlowInfo_ReachableFL _T11;unsigned _T12;union Cyc_CfFlowInfo_AbsLVal _T13;struct _union_AbsLVal_PlaceL _T14;unsigned _T15;union Cyc_CfFlowInfo_FlowInfo _T16;struct _union_FlowInfo_ReachableFL _T17;union Cyc_CfFlowInfo_AbsLVal _T18;struct _union_AbsLVal_PlaceL _T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_NewControlFlow_AnalEnv*_T1B;int _T1C;void*_T1D;int*_T1E;int _T1F;struct Cyc_Absyn_Exp*_T20;unsigned _T21;struct _fat_ptr _T22;struct _fat_ptr _T23;struct _tuple14 _T24;struct Cyc_CfFlowInfo_FlowEnv*_T25;int _T26;struct _tuple14 _T27;struct Cyc_CfFlowInfo_FlowEnv*_T28;struct Cyc_NewControlFlow_AnalEnv*_T29;struct Cyc_List_List*_T2A;struct Cyc_Absyn_Exp*_T2B;void*_T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_NewControlFlow_AnalEnv*_T2E;int _T2F;void*_T30;int _T31;struct Cyc_CfFlowInfo_FlowEnv*_T32;struct Cyc_Absyn_Exp*_T33;unsigned _T34;struct Cyc_Dict_Dict _T35;struct Cyc_CfFlowInfo_Place*_T36;void*_T37;struct _tuple14 _T38;struct Cyc_Absyn_Exp*_T39;int*_T3A;unsigned _T3B;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T3C;enum Cyc_Absyn_Coercion _T3D;struct _tuple14 _T3E;struct _tuple14 _T3F;struct _fat_ptr _T40;struct _fat_ptr _T41;int _T42;struct Cyc_Absyn_Exp*_T43;unsigned _T44;struct Cyc_String_pa_PrintArg_struct _T45;void**_T46;struct Cyc___cycFILE*_T47;struct _fat_ptr _T48;struct _fat_ptr _T49;struct Cyc___cycFILE*_T4A;struct _fat_ptr _T4B;struct _fat_ptr _T4C;struct _fat_ptr _T4D;struct _fat_ptr _T4E;int _T4F;struct _fat_ptr _T50;struct _fat_ptr _T51;int _T52;struct _tuple14 _T53;struct Cyc_CfFlowInfo_FlowEnv*_T54;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T55;union Cyc_Absyn_Cnst _T56;struct _union_Cnst_Wstring_c _T57;unsigned _T58;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T59;union Cyc_Absyn_Cnst _T5A;struct _union_Cnst_Int_c _T5B;struct _tuple7 _T5C;int _T5D;struct _tuple14 _T5E;struct Cyc_CfFlowInfo_FlowEnv*_T5F;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T60;void*_T61;int*_T62;unsigned _T63;struct _tuple14 _T64;struct Cyc_CfFlowInfo_FlowEnv*_T65;struct _tuple14 _T66;struct Cyc_CfFlowInfo_FlowEnv*_T67;struct Cyc_Absyn_Exp*_T68;void*_T69;void*_T6A;struct Cyc_CfFlowInfo_FlowEnv*_T6B;void*_T6C;void*_T6D;struct Cyc_Absyn_Vardecl*_T6E;enum Cyc_Absyn_Scope _T6F;int _T70;struct _tuple14 _T71;struct Cyc_CfFlowInfo_FlowEnv*_T72;struct Cyc_Absyn_Exp*_T73;void*_T74;void*_T75;struct Cyc_CfFlowInfo_FlowEnv*_T76;void*_T77;void*_T78;void*_T79;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _T7A;struct _tuple14 _T7B;struct Cyc_Dict_Dict _T7C;int(*_T7D)(void*,void*);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T7E;void*_T7F;int _T80;struct _tuple14 _T81;struct Cyc_CfFlowInfo_FlowEnv*_T82;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T83;struct Cyc_List_List*_T84;int _T85;int _T86;struct _fat_ptr _T87;void**_T88;unsigned _T89;void**_T8A;unsigned _T8B;struct Cyc_CfFlowInfo_FlowEnv*_T8C;struct _fat_ptr _T8D;char*_T8E;void**_T8F;struct Cyc_CfFlowInfo_FlowEnv*_T90;void**_T91;unsigned char*_T92;void**_T93;unsigned _T94;int _T95;struct Cyc_List_List*_T96;struct Cyc_List_List*_T97;struct _tuple14 _T98;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T99;struct _tuple14 _T9A;struct Cyc_CfFlowInfo_FlowEnv*_T9B;enum Cyc_Absyn_Primop _T9C;int _T9D;struct Cyc_NewControlFlow_AnalEnv*_T9E;union Cyc_CfFlowInfo_FlowInfo _T9F;struct Cyc_List_List*_TA0;void*_TA1;struct Cyc_Absyn_Exp*_TA2;int _TA3;struct Cyc_NewControlFlow_AnalEnv*_TA4;union Cyc_CfFlowInfo_FlowInfo _TA5;union Cyc_CfFlowInfo_FlowInfo _TA6;struct Cyc_List_List*_TA7;void*_TA8;struct Cyc_Absyn_Exp*_TA9;void*_TAA;struct Cyc_NewControlFlow_AnalEnv*_TAB;struct Cyc_List_List*_TAC;void*_TAD;struct Cyc_Absyn_Exp*_TAE;unsigned _TAF;union Cyc_CfFlowInfo_FlowInfo _TB0;void*_TB1;struct _tuple14 _TB2;struct Cyc_NewControlFlow_AnalEnv*_TB3;struct Cyc_CfFlowInfo_FlowEnv*_TB4;struct _tuple14 _TB5;struct Cyc_List_List _TB6;struct Cyc_NewControlFlow_AnalEnv*_TB7;union Cyc_CfFlowInfo_FlowInfo _TB8;struct Cyc_List_List*_TB9;struct Cyc_List_List*_TBA;struct Cyc_Absyn_Exp*_TBB;void*_TBC;void*_TBD;int _TBE;struct _tuple23 _TBF;union Cyc_CfFlowInfo_AbsLVal _TC0;struct _union_AbsLVal_PlaceL _TC1;unsigned _TC2;union Cyc_CfFlowInfo_FlowInfo _TC3;struct _union_FlowInfo_ReachableFL _TC4;unsigned _TC5;union Cyc_CfFlowInfo_AbsLVal _TC6;struct _union_AbsLVal_PlaceL _TC7;union Cyc_CfFlowInfo_FlowInfo _TC8;struct _union_FlowInfo_ReachableFL _TC9;struct Cyc_CfFlowInfo_FlowEnv*_TCA;struct Cyc_Absyn_Exp*_TCB;unsigned _TCC;struct Cyc_Dict_Dict _TCD;struct Cyc_CfFlowInfo_Place*_TCE;struct Cyc_CfFlowInfo_FlowEnv*_TCF;void*_TD0;struct Cyc_Dict_Dict _TD1;int _TD2;struct _tuple14 _TD3;struct Cyc_CfFlowInfo_FlowEnv*_TD4;struct _tuple14 _TD5;struct Cyc_CfFlowInfo_FlowEnv*_TD6;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_TD7;struct Cyc_Core_Opt*_TD8;int _TD9;struct Cyc_NewControlFlow_AnalEnv*_TDA;struct Cyc_List_List*_TDB;struct Cyc_Absyn_Exp*_TDC;void*_TDD;void*_TDE;int _TDF;struct Cyc_Absyn_Exp*_TE0;unsigned _TE1;struct _fat_ptr _TE2;struct _fat_ptr _TE3;struct _tuple14 _TE4;struct Cyc_CfFlowInfo_FlowEnv*_TE5;union Cyc_CfFlowInfo_FlowInfo _TE6;struct _union_FlowInfo_ReachableFL _TE7;unsigned _TE8;union Cyc_CfFlowInfo_FlowInfo _TE9;struct _union_FlowInfo_ReachableFL _TEA;union Cyc_CfFlowInfo_AbsLVal _TEB;struct _union_AbsLVal_PlaceL _TEC;unsigned _TED;union Cyc_CfFlowInfo_AbsLVal _TEE;struct _union_AbsLVal_PlaceL _TEF;struct Cyc_CfFlowInfo_FlowEnv*_TF0;struct Cyc_Absyn_Exp*_TF1;unsigned _TF2;struct Cyc_Dict_Dict _TF3;struct Cyc_CfFlowInfo_Place*_TF4;struct Cyc_CfFlowInfo_FlowEnv*_TF5;void*_TF6;struct _tuple14 _TF7;struct Cyc_CfFlowInfo_FlowEnv*_TF8;int _TF9;struct Cyc_NewControlFlow_AnalEnv*_TFA;struct Cyc_List_List*_TFB;struct Cyc_Absyn_Exp*_TFC;void*_TFD;void*_TFE;int _TFF;struct Cyc_Absyn_Exp*_T100;unsigned _T101;struct _fat_ptr _T102;struct _fat_ptr _T103;struct _tuple14 _T104;struct Cyc_CfFlowInfo_FlowEnv*_T105;struct Cyc_NewControlFlow_AnalEnv*_T106;union Cyc_CfFlowInfo_FlowInfo _T107;struct Cyc_List_List*_T108;struct Cyc_Absyn_Exp**_T109;struct Cyc_Absyn_Exp**_T10A;struct _fat_ptr _T10B;struct Cyc_CfFlowInfo_FlowEnv*_T10C;struct Cyc_NewControlFlow_AnalEnv*_T10D;union Cyc_CfFlowInfo_FlowInfo _T10E;struct Cyc_Absyn_Exp*_T10F;union Cyc_CfFlowInfo_AbsLVal _T110;struct Cyc_Absyn_Exp*_T111;void*_T112;struct Cyc_Absyn_Exp*_T113;unsigned _T114;struct _tuple14 _T115;struct _tuple14 _T116;struct Cyc_NewControlFlow_AnalEnv*_T117;struct Cyc_Absyn_Exp*_T118;unsigned _T119;union Cyc_CfFlowInfo_FlowInfo _T11A;void*_T11B;struct _tuple14 _T11C;struct Cyc_CfFlowInfo_FlowEnv*_T11D;struct Cyc_Absyn_Exp*_T11E;void*_T11F;void*_T120;struct Cyc_CfFlowInfo_FlowEnv*_T121;void*_T122;struct Cyc_NewControlFlow_AnalEnv*_T123;union Cyc_CfFlowInfo_FlowInfo _T124;struct Cyc_List_List*_T125;struct Cyc_NewControlFlow_AnalEnv*_T126;struct Cyc_Absyn_Exp*_T127;unsigned _T128;union Cyc_CfFlowInfo_FlowInfo _T129;void*_T12A;struct Cyc_Absyn_Exp*_T12B;void*_T12C;void*_T12D;int*_T12E;int _T12F;struct Cyc_Absyn_PtrInfo _T130;int*_T131;int _T132;struct Cyc_Absyn_FnInfo _T133;struct Cyc_List_List*_T134;int*_T135;unsigned _T136;struct Cyc_List_List*_T137;int _T138;struct Cyc_List_List*_T139;int _T13A;struct Cyc_List_List*_T13B;int _T13C;struct Cyc_List_List*_T13D;int _T13E;struct Cyc_List_List*_T13F;int(*_T140)(struct _fat_ptr,struct _fat_ptr);void*(*_T141)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T142;struct _fat_ptr _T143;int(*_T144)(struct _fat_ptr,struct _fat_ptr);void*(*_T145)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T146;struct _fat_ptr _T147;int(*_T148)(struct Cyc_List_List*,int);int(*_T149)(struct Cyc_List_List*,void*);int _T14A;union Cyc_CfFlowInfo_FlowInfo _T14B;struct _union_FlowInfo_BottomFL _T14C;unsigned _T14D;union Cyc_CfFlowInfo_FlowInfo _T14E;struct _union_FlowInfo_ReachableFL _T14F;struct Cyc_List_List*_T150;void*_T151;struct Cyc_NewControlFlow_AnalEnv*_T152;struct Cyc_CfFlowInfo_FlowEnv*_T153;struct Cyc_Dict_Dict _T154;struct Cyc_List_List*_T155;void*_T156;enum Cyc_CfFlowInfo_InitLevel _T157;int _T158;struct Cyc_List_List*_T159;void*_T15A;int _T15B;struct Cyc_List_List*_T15C;void*_T15D;struct Cyc_Absyn_Exp*_T15E;unsigned _T15F;struct _fat_ptr _T160;struct _fat_ptr _T161;union Cyc_CfFlowInfo_FlowInfo _T162;struct _union_FlowInfo_BottomFL _T163;unsigned _T164;union Cyc_CfFlowInfo_FlowInfo _T165;struct _union_FlowInfo_ReachableFL _T166;struct Cyc_CfFlowInfo_FlowEnv*_T167;struct Cyc_Dict_Dict _T168;struct Cyc_List_List*_T169;void*_T16A;struct Cyc_List_List*_T16B;int*_T16C;unsigned _T16D;struct Cyc_List_List*_T16E;void*_T16F;struct Cyc_Absyn_Exp*_T170;void*_T171;int*_T172;int _T173;struct Cyc_Absyn_PtrInfo _T174;struct Cyc_CfFlowInfo_FlowEnv*_T175;struct Cyc_List_List*_T176;void*_T177;struct Cyc_Absyn_Exp*_T178;unsigned _T179;struct Cyc_Dict_Dict _T17A;struct Cyc_CfFlowInfo_Place*_T17B;struct Cyc_CfFlowInfo_FlowEnv*_T17C;void*_T17D;struct Cyc_CfFlowInfo_FlowEnv*_T17E;void*_T17F;void*_T180;int(*_T181)(struct _fat_ptr,struct _fat_ptr);void*(*_T182)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T183;struct _fat_ptr _T184;int(*_T185)(struct _fat_ptr,struct _fat_ptr);void*(*_T186)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T187;struct _fat_ptr _T188;int(*_T189)(struct Cyc_List_List*,int);int(*_T18A)(struct Cyc_List_List*,void*);int _T18B;struct Cyc_NewControlFlow_AnalEnv*_T18C;struct Cyc_List_List*_T18D;void*_T18E;struct Cyc_Absyn_Exp*_T18F;unsigned _T190;struct Cyc_List_List*_T191;void*_T192;struct Cyc_Absyn_Exp*_T193;void*_T194;void*_T195;union Cyc_CfFlowInfo_FlowInfo _T196;struct Cyc_List_List*_T197;void*_T198;struct Cyc_NewControlFlow_AnalEnv*_T199;struct Cyc_List_List*_T19A;void*_T19B;struct Cyc_Absyn_Exp*_T19C;unsigned _T19D;union Cyc_CfFlowInfo_FlowInfo _T19E;struct Cyc_List_List*_T19F;void*_T1A0;int(*_T1A1)(struct Cyc_List_List*,int);int(*_T1A2)(struct Cyc_List_List*,void*);int _T1A3;struct Cyc_NewControlFlow_AnalEnv*_T1A4;struct Cyc_List_List*_T1A5;struct Cyc_List_List*_T1A6;void*_T1A7;struct Cyc_Absyn_Exp*_T1A8;void*_T1A9;void*_T1AA;int _T1AB;struct Cyc_NewControlFlow_AnalEnv*_T1AC;union Cyc_CfFlowInfo_FlowInfo _T1AD;struct Cyc_List_List*_T1AE;void*_T1AF;struct Cyc_Absyn_Exp*_T1B0;struct Cyc_List_List*_T1B1;void*_T1B2;struct Cyc_Absyn_Exp*_T1B3;unsigned _T1B4;struct Cyc_List_List*_T1B5;void*_T1B6;struct Cyc_List_List*_T1B7;struct Cyc_List_List*_T1B8;struct Cyc_Absyn_Exp*_T1B9;void*_T1BA;void*_T1BB;int _T1BC;struct _tuple14 _T1BD;struct Cyc_CfFlowInfo_FlowEnv*_T1BE;struct Cyc_Absyn_Exp*_T1BF;void*_T1C0;void*_T1C1;struct Cyc_CfFlowInfo_FlowEnv*_T1C2;void*_T1C3;struct _tuple14 _T1C4;struct Cyc_CfFlowInfo_FlowEnv*_T1C5;struct Cyc_Absyn_Exp*_T1C6;void*_T1C7;void*_T1C8;struct Cyc_CfFlowInfo_FlowEnv*_T1C9;void*_T1CA;struct Cyc_Absyn_MallocInfo _T1CB;struct Cyc_Absyn_MallocInfo _T1CC;struct Cyc_Absyn_MallocInfo _T1CD;struct Cyc_Absyn_MallocInfo _T1CE;struct Cyc_Absyn_MallocInfo _T1CF;struct Cyc_Absyn_MallocInfo _T1D0;int _T1D1;struct Cyc_CfFlowInfo_FlowEnv*_T1D2;enum Cyc_Absyn_MallocKind _T1D3;int _T1D4;struct Cyc_CfFlowInfo_FlowEnv*_T1D5;void**_T1D6;void*_T1D7;struct Cyc_CfFlowInfo_FlowEnv*_T1D8;void*_T1D9;struct Cyc_CfFlowInfo_FlowEnv*_T1DA;void**_T1DB;void*_T1DC;struct Cyc_CfFlowInfo_FlowEnv*_T1DD;void*_T1DE;struct Cyc_List_List*_T1DF;struct Cyc_List_List*_T1E0;struct Cyc_Absyn_Exp*_T1E1;unsigned _T1E2;struct Cyc_List_List*_T1E3;struct Cyc_Absyn_Exp*_T1E4;unsigned _T1E5;struct Cyc_List_List*_T1E6;struct Cyc_NewControlFlow_AnalEnv*_T1E7;struct Cyc_List_List*_T1E8;struct Cyc_Absyn_Exp*_T1E9;void*_T1EA;void*_T1EB;int _T1EC;struct _tuple14 _T1ED;struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T1EE;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T1EF;struct Cyc_Absyn_Exp*_T1F0;void*_T1F1;struct Cyc_CfFlowInfo_Place*_T1F2;struct Cyc_CfFlowInfo_Place*_T1F3;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T1F4;union Cyc_CfFlowInfo_FlowInfo _T1F5;struct _union_FlowInfo_BottomFL _T1F6;unsigned _T1F7;struct _tuple14 _T1F8;union Cyc_CfFlowInfo_FlowInfo _T1F9;struct _union_FlowInfo_ReachableFL _T1FA;struct _tuple14 _T1FB;struct Cyc_Dict_Dict _T1FC;struct Cyc_NewControlFlow_AnalEnv*_T1FD;union Cyc_CfFlowInfo_FlowInfo _T1FE;struct Cyc_List_List*_T1FF;struct Cyc_Absyn_Exp**_T200;struct Cyc_Absyn_Exp**_T201;struct _fat_ptr _T202;struct Cyc_List_List*_T203;struct Cyc_List_List*_T204;struct Cyc_List_List*_T205;struct Cyc_List_List*_T206;struct Cyc_Absyn_Exp*_T207;void**_T208;int*_T209;void**_T20A;int _T20B;void*_T20C;int*_T20D;int _T20E;struct Cyc_Absyn_Exp*_T20F;unsigned _T210;struct _fat_ptr _T211;struct _fat_ptr _T212;struct Cyc_Absyn_Exp*_T213;void**_T214;int*_T215;void**_T216;int _T217;void*_T218;int*_T219;int _T21A;struct Cyc_Absyn_Exp*_T21B;unsigned _T21C;struct _fat_ptr _T21D;struct _fat_ptr _T21E;union Cyc_CfFlowInfo_FlowInfo _T21F;struct _union_FlowInfo_ReachableFL _T220;unsigned _T221;union Cyc_CfFlowInfo_FlowInfo _T222;struct _union_FlowInfo_ReachableFL _T223;union Cyc_CfFlowInfo_AbsLVal _T224;struct _union_AbsLVal_PlaceL _T225;unsigned _T226;union Cyc_CfFlowInfo_AbsLVal _T227;struct _union_AbsLVal_PlaceL _T228;struct Cyc_CfFlowInfo_FlowEnv*_T229;struct Cyc_Absyn_Exp*_T22A;unsigned _T22B;struct Cyc_Dict_Dict _T22C;struct Cyc_CfFlowInfo_Place*_T22D;void*_T22E;enum Cyc_CfFlowInfo_InitLevel _T22F;int _T230;struct Cyc_Absyn_Exp*_T231;unsigned _T232;struct _fat_ptr _T233;struct _fat_ptr _T234;union Cyc_CfFlowInfo_AbsLVal _T235;struct _union_AbsLVal_PlaceL _T236;unsigned _T237;union Cyc_CfFlowInfo_AbsLVal _T238;struct _union_AbsLVal_PlaceL _T239;struct Cyc_CfFlowInfo_FlowEnv*_T23A;struct Cyc_Absyn_Exp*_T23B;unsigned _T23C;struct Cyc_Dict_Dict _T23D;struct Cyc_CfFlowInfo_Place*_T23E;void*_T23F;enum Cyc_CfFlowInfo_InitLevel _T240;int _T241;struct Cyc_Absyn_Exp*_T242;unsigned _T243;struct _fat_ptr _T244;struct _fat_ptr _T245;struct _tuple14 _T246;struct Cyc_CfFlowInfo_FlowEnv*_T247;struct Cyc_List_List*_T248;struct Cyc_List_List*_T249;struct Cyc_Absyn_Exp*_T24A;unsigned _T24B;struct Cyc_List_List*_T24C;struct Cyc_Absyn_Exp*_T24D;unsigned _T24E;struct Cyc_List_List*_T24F;void*_T250;struct Cyc_List_List*_T251;struct Cyc_List_List*_T252;struct Cyc_List_List*_T253;struct Cyc_List_List*_T254;struct Cyc_List_List*_T255;struct Cyc_List_List*_T256;struct Cyc_List_List*_T257;struct Cyc_List_List*_T258;struct Cyc_NewControlFlow_AnalEnv*_T259;struct Cyc_List_List*_T25A;struct Cyc_Absyn_Exp*_T25B;void*_T25C;void*_T25D;int _T25E;struct _tuple14 _T25F;struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T260;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T261;struct Cyc_Absyn_Exp*_T262;void*_T263;struct Cyc_CfFlowInfo_Place*_T264;struct Cyc_CfFlowInfo_Place*_T265;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T266;union Cyc_CfFlowInfo_FlowInfo _T267;struct _union_FlowInfo_BottomFL _T268;unsigned _T269;struct _tuple14 _T26A;union Cyc_CfFlowInfo_FlowInfo _T26B;struct _union_FlowInfo_ReachableFL _T26C;struct _tuple14 _T26D;struct Cyc_Dict_Dict _T26E;union Cyc_CfFlowInfo_AbsLVal _T26F;struct _union_AbsLVal_UnknownL _T270;unsigned _T271;struct _tuple14 _T272;struct Cyc_CfFlowInfo_FlowEnv*_T273;union Cyc_CfFlowInfo_AbsLVal _T274;struct _union_AbsLVal_PlaceL _T275;struct _tuple14 _T276;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T277;struct _tuple14 _T278;struct Cyc_Absyn_Exp*_T279;void*_T27A;int _T27B;struct _tuple14 _T27C;struct Cyc_CfFlowInfo_FlowEnv*_T27D;struct Cyc_Absyn_Exp*_T27E;void*_T27F;void*_T280;struct Cyc_CfFlowInfo_FlowEnv*_T281;void*_T282;int _T283;struct Cyc_Absyn_Exp*_T284;unsigned _T285;void*_T286;struct _fat_ptr*_T287;union Cyc_CfFlowInfo_FlowInfo _T288;void*_T289;int*_T28A;int _T28B;void*_T28C;struct Cyc_CfFlowInfo_UnionRInfo _T28D;struct Cyc_CfFlowInfo_UnionRInfo _T28E;struct Cyc_Absyn_Exp*_T28F;void*_T290;void*_T291;struct _fat_ptr*_T292;int _T293;struct _tuple14 _T294;struct Cyc_CfFlowInfo_FlowEnv*_T295;struct Cyc_Absyn_Exp*_T296;void*_T297;void*_T298;struct Cyc_CfFlowInfo_FlowEnv*_T299;void*_T29A;struct _tuple14 _T29B;struct _fat_ptr _T29C;int _T29D;char*_T29E;void**_T29F;struct Cyc___cycFILE*_T2A0;struct _fat_ptr _T2A1;struct _fat_ptr _T2A2;struct Cyc_String_pa_PrintArg_struct _T2A3;void**_T2A4;int(*_T2A5)(struct _fat_ptr,struct _fat_ptr);void*(*_T2A6)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2A7;struct _fat_ptr _T2A8;struct Cyc_Absyn_Exp*_T2A9;void*_T2AA;void*_T2AB;int _T2AC;struct _tuple14 _T2AD;struct Cyc_CfFlowInfo_FlowEnv*_T2AE;void*_T2AF;int*_T2B0;int _T2B1;void*_T2B2;struct Cyc_CfFlowInfo_UnionRInfo _T2B3;struct Cyc_CfFlowInfo_UnionRInfo _T2B4;struct Cyc_Absyn_Exp*_T2B5;void*_T2B6;void*_T2B7;struct _fat_ptr*_T2B8;int _T2B9;struct _tuple14 _T2BA;struct Cyc_CfFlowInfo_FlowEnv*_T2BB;struct _tuple14 _T2BC;struct Cyc_CfFlowInfo_FlowEnv*_T2BD;struct _tuple14 _T2BE;struct Cyc_CfFlowInfo_FlowEnv*_T2BF;struct Cyc_String_pa_PrintArg_struct _T2C0;void**_T2C1;int(*_T2C2)(struct _fat_ptr,struct _fat_ptr);void*(*_T2C3)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2C4;struct _fat_ptr _T2C5;struct Cyc_Absyn_Exp*_T2C6;void*_T2C7;void*_T2C8;int*_T2C9;int _T2CA;struct Cyc_Absyn_PtrInfo _T2CB;int _T2CC;struct _tuple14 _T2CD;struct Cyc_CfFlowInfo_FlowEnv*_T2CE;struct Cyc_Absyn_Exp*_T2CF;void*_T2D0;void*_T2D1;struct Cyc_CfFlowInfo_FlowEnv*_T2D2;void*_T2D3;int _T2D4;struct Cyc_Absyn_Exp*_T2D5;unsigned _T2D6;void*_T2D7;struct _fat_ptr*_T2D8;union Cyc_CfFlowInfo_FlowInfo _T2D9;void*_T2DA;int*_T2DB;int _T2DC;void*_T2DD;struct Cyc_CfFlowInfo_UnionRInfo _T2DE;struct Cyc_CfFlowInfo_UnionRInfo _T2DF;int _T2E0;struct _tuple14 _T2E1;struct Cyc_CfFlowInfo_FlowEnv*_T2E2;struct Cyc_Absyn_Exp*_T2E3;void*_T2E4;void*_T2E5;struct Cyc_CfFlowInfo_FlowEnv*_T2E6;void*_T2E7;struct _tuple14 _T2E8;struct _fat_ptr _T2E9;int _T2EA;char*_T2EB;void**_T2EC;int(*_T2ED)(struct _fat_ptr,struct _fat_ptr);void*(*_T2EE)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2EF;struct _fat_ptr _T2F0;int(*_T2F1)(struct _fat_ptr,struct _fat_ptr);void*(*_T2F2)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2F3;struct _fat_ptr _T2F4;struct _tuple14 _T2F5;struct _tuple14 _T2F6;struct _tuple14 _T2F7;struct Cyc_CfFlowInfo_FlowEnv*_T2F8;struct _tuple14 _T2F9;struct _tuple14 _T2FA;struct _tuple14 _T2FB;struct Cyc_CfFlowInfo_FlowEnv*_T2FC;struct _tuple14 _T2FD;struct Cyc_NewControlFlow_AnalEnv*_T2FE;union Cyc_CfFlowInfo_FlowInfo _T2FF;struct Cyc_List_List*_T300;struct Cyc_Absyn_Exp**_T301;struct Cyc_Absyn_Exp**_T302;struct _fat_ptr _T303;union Cyc_CfFlowInfo_FlowInfo _T304;struct _union_FlowInfo_ReachableFL _T305;unsigned _T306;union Cyc_CfFlowInfo_FlowInfo _T307;struct _union_FlowInfo_ReachableFL _T308;struct Cyc_NewControlFlow_AnalEnv*_T309;struct Cyc_CfFlowInfo_FlowEnv*_T30A;struct Cyc_Dict_Dict _T30B;struct Cyc_List_List*_T30C;struct Cyc_List_List*_T30D;struct Cyc_List_List*_T30E;void*_T30F;enum Cyc_CfFlowInfo_InitLevel _T310;int _T311;struct Cyc_Absyn_Exp*_T312;unsigned _T313;struct _fat_ptr _T314;struct _fat_ptr _T315;struct Cyc_NewControlFlow_AnalEnv*_T316;union Cyc_CfFlowInfo_FlowInfo _T317;union Cyc_CfFlowInfo_FlowInfo _T318;struct Cyc_Absyn_Exp*_T319;struct Cyc_List_List*_T31A;void*_T31B;struct Cyc_Absyn_Exp*_T31C;union Cyc_CfFlowInfo_FlowInfo _T31D;struct _union_FlowInfo_BottomFL _T31E;unsigned _T31F;struct _tuple14 _T320;struct _tuple14 _T321;struct Cyc_Absyn_Exp*_T322;void*_T323;void*_T324;int*_T325;int _T326;int(*_T327)(struct _fat_ptr,struct _fat_ptr);void*(*_T328)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T329;struct _fat_ptr _T32A;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T32B;struct Cyc_Absyn_Aggrdecl*_T32C;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T32D;struct Cyc_Absyn_Aggrdecl*_T32E;struct Cyc_Absyn_Aggrdecl*_T32F;struct Cyc_Absyn_AggrdeclImpl*_T330;struct Cyc_Absyn_Aggrdecl*_T331;struct Cyc_Absyn_AggrdeclImpl*_T332;struct Cyc_Absyn_Exp*_T333;void*_T334;struct Cyc_NewControlFlow_AnalEnv*_T335;union Cyc_CfFlowInfo_FlowInfo _T336;struct Cyc_List_List*(*_T337)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*);struct Cyc_List_List*(*_T338)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Exp*(*_T339)(struct _tuple24*);void*(*_T33A)(struct _tuple0*);struct Cyc_List_List*_T33B;struct Cyc_List_List*_T33C;struct Cyc_CfFlowInfo_FlowEnv*_T33D;struct Cyc_List_List*_T33E;enum Cyc_Absyn_AggrKind _T33F;int _T340;int _T341;struct Cyc_CfFlowInfo_FlowEnv*_T342;void*_T343;struct Cyc_List_List*_T344;void*_T345;struct _tuple24*_T346;struct _tuple24 _T347;struct Cyc_List_List*_T348;int*_T349;int _T34A;int(*_T34B)(struct _fat_ptr,struct _fat_ptr);void*(*_T34C)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T34D;struct _fat_ptr _T34E;struct _fat_ptr _T34F;int _T350;char*_T351;void**_T352;struct Cyc_List_List*_T353;enum Cyc_Absyn_AggrKind _T354;int _T355;struct Cyc_List_List*_T356;void*_T357;struct _tuple24*_T358;struct _tuple24 _T359;struct Cyc_NewControlFlow_AnalEnv*_T35A;struct Cyc_Absyn_Exp*_T35B;unsigned _T35C;union Cyc_CfFlowInfo_FlowInfo _T35D;struct Cyc_List_List*_T35E;void*_T35F;int _T360;struct Cyc_Absyn_Exp*_T361;unsigned _T362;void*_T363;struct _fat_ptr*_T364;union Cyc_CfFlowInfo_FlowInfo _T365;struct Cyc_List_List*_T366;struct Cyc_List_List*_T367;struct Cyc_List_List*_T368;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T369;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T36A;enum Cyc_Absyn_AggrKind _T36B;int _T36C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T36D;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T36E;struct _tuple14 _T36F;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T370;struct Cyc_String_pa_PrintArg_struct _T371;void**_T372;int(*_T373)(struct _fat_ptr,struct _fat_ptr);void*(*_T374)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T375;struct _fat_ptr _T376;struct Cyc_List_List*(*_T377)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*);struct Cyc_List_List*(*_T378)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Exp*(*_T379)(struct _tuple24*);void*(*_T37A)(struct _tuple0*);struct Cyc_List_List*_T37B;struct Cyc_NewControlFlow_AnalEnv*_T37C;struct Cyc_List_List*_T37D;void*_T37E;struct Cyc_Absyn_Exp*_T37F;unsigned _T380;union Cyc_CfFlowInfo_FlowInfo _T381;struct Cyc_List_List*_T382;void*_T383;struct Cyc_List_List*_T384;struct Cyc_List_List*_T385;struct _tuple14 _T386;struct Cyc_CfFlowInfo_FlowEnv*_T387;struct Cyc_Absyn_Exp*_T388;void*_T389;void*_T38A;struct Cyc_CfFlowInfo_FlowEnv*_T38B;void*_T38C;union Cyc_CfFlowInfo_FlowInfo _T38D;struct _union_FlowInfo_BottomFL _T38E;unsigned _T38F;struct _tuple14 _T390;struct Cyc_CfFlowInfo_FlowEnv*_T391;union Cyc_CfFlowInfo_FlowInfo _T392;struct _union_FlowInfo_ReachableFL _T393;struct Cyc_NewControlFlow_AnalEnv*_T394;struct Cyc_CfFlowInfo_FlowEnv*_T395;struct Cyc_Dict_Dict _T396;void*_T397;enum Cyc_CfFlowInfo_InitLevel _T398;int _T399;struct Cyc_Absyn_Exp*_T39A;unsigned _T39B;struct _fat_ptr _T39C;struct _fat_ptr _T39D;void*_T39E;int*_T39F;unsigned _T3A0;struct _tuple14 _T3A1;struct Cyc_CfFlowInfo_FlowEnv*_T3A2;struct Cyc_List_List _T3A3;struct Cyc_CfFlowInfo_FlowEnv*_T3A4;union Cyc_CfFlowInfo_FlowInfo _T3A5;struct Cyc_List_List*_T3A6;struct Cyc_List_List*_T3A7;struct Cyc_CfFlowInfo_FlowEnv*_T3A8;void*_T3A9;struct Cyc_Absyn_Exp*_T3AA;unsigned _T3AB;union Cyc_CfFlowInfo_FlowInfo _T3AC;struct _union_FlowInfo_BottomFL _T3AD;unsigned _T3AE;struct _tuple14 _T3AF;struct Cyc_CfFlowInfo_FlowEnv*_T3B0;union Cyc_CfFlowInfo_FlowInfo _T3B1;struct _union_FlowInfo_ReachableFL _T3B2;enum Cyc_CfFlowInfo_InitLevel _T3B3;int _T3B4;struct Cyc_Absyn_Exp*_T3B5;unsigned _T3B6;struct _fat_ptr _T3B7;struct _fat_ptr _T3B8;struct _tuple14 _T3B9;struct Cyc_CfFlowInfo_FlowEnv*_T3BA;struct Cyc_List_List _T3BB;struct Cyc_CfFlowInfo_FlowEnv*_T3BC;union Cyc_CfFlowInfo_FlowInfo _T3BD;struct Cyc_List_List*_T3BE;struct Cyc_List_List*_T3BF;struct Cyc_CfFlowInfo_FlowEnv*_T3C0;void*_T3C1;struct Cyc_Absyn_Exp*_T3C2;unsigned _T3C3;union Cyc_CfFlowInfo_FlowInfo _T3C4;struct _union_FlowInfo_BottomFL _T3C5;unsigned _T3C6;union Cyc_CfFlowInfo_FlowInfo _T3C7;struct _union_FlowInfo_ReachableFL _T3C8;enum Cyc_CfFlowInfo_InitLevel _T3C9;int _T3CA;struct Cyc_Absyn_Exp*_T3CB;unsigned _T3CC;struct _fat_ptr _T3CD;struct _fat_ptr _T3CE;struct _tuple14 _T3CF;struct Cyc_CfFlowInfo_FlowEnv*_T3D0;int _T3D1;struct _tuple14 _T3D2;struct Cyc_CfFlowInfo_FlowEnv*_T3D3;void*_T3D4;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T3D5;struct Cyc_Absyn_Exp*_T3D6;void*_T3D7;struct Cyc_CfFlowInfo_Place*_T3D8;struct Cyc_CfFlowInfo_Place*_T3D9;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T3DA;struct Cyc_CfFlowInfo_FlowEnv*_T3DB;void*_T3DC;struct Cyc_CfFlowInfo_FlowEnv*_T3DD;void*_T3DE;union Cyc_CfFlowInfo_FlowInfo _T3DF;struct _union_FlowInfo_BottomFL _T3E0;unsigned _T3E1;struct _tuple14 _T3E2;union Cyc_CfFlowInfo_FlowInfo _T3E3;struct _union_FlowInfo_ReachableFL _T3E4;struct _tuple14 _T3E5;struct Cyc_Dict_Dict _T3E6;struct _tuple15 _T3E7;struct Cyc_NewControlFlow_AnalEnv*_T3E8;struct Cyc_CfFlowInfo_FlowEnv*_T3E9;struct Cyc_NewControlFlow_AnalEnv*_T3EA;union Cyc_CfFlowInfo_FlowInfo _T3EB;struct Cyc_Absyn_Stmt*_T3EC;struct _tuple15*_T3ED;struct _tuple14 _T3EE;struct _tuple15 _T3EF;int(*_T3F0)(struct _fat_ptr,struct _fat_ptr);void*(*_T3F1)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T3F2;struct _fat_ptr _T3F3;_T0=env;{
# 778
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T0->fenv;
struct Cyc_Dict_Dict d;{struct Cyc_Dict_Dict _T3F4;_T1=inflow;_T2=_T1.BottomFL;_T3=_T2.tag;if(_T3!=1)goto _TLD8;{struct _tuple14 _T3F5;
# 791
_T3F5.f0=Cyc_CfFlowInfo_BottomFL();_T5=fenv;_T3F5.f1=_T5->unknown_all;_T4=_T3F5;}return _T4;_TLD8: _T6=inflow;_T7=_T6.ReachableFL;_T3F4=_T7.val;{struct Cyc_Dict_Dict d2=_T3F4;
d=d2;};}_T8=copy_ctxt;
# 804 "new_control_flow.cyc"
if(!_T8)goto _TLDA;_T9=Cyc_NewControlFlow_is_local_var_rooted_path(e,0);if(!_T9)goto _TLDA;_TA=env;_TB=_TA->aquals_bounds;_TC=e;_TD=_TC->topt;_TE=
Cyc_Tcutil_is_noalias_pointer_or_aggr(_TB,_TD);
# 804
if(!_TE)goto _TLDA;{
# 825 "new_control_flow.cyc"
struct _tuple16 _T3F4=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_AbsLVal _T3F5;union Cyc_CfFlowInfo_FlowInfo _T3F6;_T3F6=_T3F4.f0;_T3F5=_T3F4.f1;{union Cyc_CfFlowInfo_FlowInfo flval=_T3F6;union Cyc_CfFlowInfo_AbsLVal lval=_T3F5;{struct _tuple16 _T3F7;
_T3F7.f0=flval;_T3F7.f1=lval;_TF=_T3F7;}{struct _tuple16 _T3F7=_TF;struct Cyc_CfFlowInfo_Place*_T3F8;struct Cyc_Dict_Dict _T3F9;_T10=_T3F7.f0;_T11=_T10.ReachableFL;_T12=_T11.tag;if(_T12!=2)goto _TLDC;_T13=_T3F7.f1;_T14=_T13.PlaceL;_T15=_T14.tag;if(_T15!=1)goto _TLDE;_T16=_T3F7.f0;_T17=_T16.ReachableFL;_T3F9=_T17.val;_T18=_T3F7.f1;_T19=_T18.PlaceL;_T3F8=_T19.val;{struct Cyc_Dict_Dict fd=_T3F9;struct Cyc_CfFlowInfo_Place*p=_T3F8;
# 828
void*old_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
int needs_unconsume=0;_T1A=e;_T1B=env;_T1C=_T1B->iteration_num;_T1D=old_rval;_T1E=& needs_unconsume;_T1F=
Cyc_CfFlowInfo_is_unique_consumed(_T1A,_T1C,_T1D,_T1E);if(!_T1F)goto _TLE0;_T20=e;_T21=_T20->loc;_T22=
_tag_fat("expression attempts to copy a consumed alias-free value",sizeof(char),56U);_T23=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T21,_T22,_T23);{struct _tuple14 _T3FA;
_T3FA.f0=Cyc_CfFlowInfo_BottomFL();_T25=fenv;_T3FA.f1=_T25->unknown_all;_T24=_T3FA;}return _T24;
# 834
_TLE0: _T26=needs_unconsume;if(!_T26)goto _TLE2;{struct _tuple14 _T3FA;
# 836
_T3FA.f0=flval;_T3FA.f1=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,old_rval);_T27=_T3FA;}return _T27;
# 839
_TLE2: _T28=fenv;_T29=env;_T2A=_T29->aquals_bounds;_T2B=e;_T2C=_T2B->topt;_T2D=e;_T2E=env;_T2F=_T2E->iteration_num;_T30=old_rval;_T31=local_alias;{void*new_rval=Cyc_CfFlowInfo_make_unique_consumed(_T28,_T2A,_T2C,_T2D,_T2F,_T30,_T31);_T32=fenv;_T33=e;_T34=_T33->loc;_T35=fd;_T36=p;_T37=new_rval;{
struct Cyc_Dict_Dict outdict=Cyc_CfFlowInfo_assign_place(_T32,_T34,_T35,_T36,_T37);{struct _tuple14 _T3FA;
# 851
_T3FA.f0=Cyc_CfFlowInfo_ReachableFL(outdict);_T3FA.f1=old_rval;_T38=_T3FA;}return _T38;}}}_TLDE: goto _LLB;_TLDC: _LLB: goto _LL8;_LL8:;}}}goto _TLDB;_TLDA: _TLDB: _T39=e;{
# 858
void*_T3F4=_T39->r;struct Cyc_Absyn_Stmt*_T3F5;enum Cyc_Absyn_AggrKind _T3F6;struct _fat_ptr*_T3F7;struct Cyc_Absyn_Exp*_T3F8;void**_T3F9;enum Cyc_Absyn_MallocKind _T3FA;struct Cyc_List_List*_T3FB;struct Cyc_Absyn_Exp*_T3FC;enum Cyc_Absyn_Primop _T3FD;struct Cyc_Absyn_Datatypedecl*_T3FE;int _T3FF;struct _fat_ptr _T400;void*_T401;_T3A=(int*)_T3F4;_T3B=*_T3A;switch(_T3B){case 14: _T3C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T3F4;_T3D=_T3C->f4;if(_T3D!=Cyc_Absyn_Null_to_NonNull)goto _TLE5;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;
# 862
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T404;void*r1=_T403;
struct _tuple14 _T405=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);void*_T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_T407;void*r2=_T406;{struct _tuple14 _T408;
_T408.f0=f2;_T408.f1=r1;_T3E=_T408;}return _T3E;}}}_TLE5:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;_T401=e1;goto _LL13;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;}_LL13: {struct Cyc_Absyn_Exp*e1=_T401;_T401=e1;goto _LL15;}case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;}_LL15: {struct Cyc_Absyn_Exp*e1=_T401;_T401=e1;goto _LL17;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;}_LL17: {struct Cyc_Absyn_Exp*e1=_T401;_T3F=
# 871
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);return _T3F;}case 2:{struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_T3F4;_T400=_T402->f1;}{struct _fat_ptr p=_T400;_T40=p;_T41=
# 874
_tag_fat("print_flow",sizeof(char),11U);_T42=Cyc_NewControlFlow_strcmp(_T40,_T41);if(_T42)goto _TLE7;else{goto _TLE9;}
_TLE9: _T43=e;_T44=_T43->loc;{struct _fat_ptr seg_str=Cyc_Position_string_of_segment(_T44);{struct Cyc_String_pa_PrintArg_struct _T402;_T402.tag=0;
_T402.f1=seg_str;_T45=_T402;}{struct Cyc_String_pa_PrintArg_struct _T402=_T45;void*_T403[1];_T46=_T403 + 0;*_T46=& _T402;_T47=Cyc_stderr;_T48=_tag_fat("%s: current flow is\n",sizeof(char),21U);_T49=_tag_fat(_T403,sizeof(void*),1);Cyc_fprintf(_T47,_T48,_T49);}
Cyc_CfFlowInfo_print_flow(inflow);_T4A=Cyc_stderr;_T4B=
_tag_fat("\n",sizeof(char),2U);_T4C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4A,_T4B,_T4C);}goto _TLE8;
# 880
_TLE7: _T4D=p;_T4E=_tag_fat("debug_on",sizeof(char),9U);_T4F=Cyc_NewControlFlow_strcmp(_T4D,_T4E);if(_T4F)goto _TLEA;else{goto _TLEC;}_TLEC: goto _TLEB;
# 885
_TLEA: _T50=p;_T51=_tag_fat("debug_off",sizeof(char),10U);_T52=Cyc_NewControlFlow_strcmp(_T50,_T51);if(_T52)goto _TLED;else{goto _TLEF;}_TLEF: goto _TLEE;_TLED: _TLEE: _TLEB: _TLE8:{struct _tuple14 _T402;
# 893
_T402.f0=inflow;_T54=fenv;_T402.f1=_T54->zero;_T53=_T402;}return _T53;}case 39: goto _LL1D;case 0: _T55=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T3F4;_T56=_T55->f1;_T57=_T56.Wstring_c;_T58=_T57.tag;switch(_T58){case 1: _LL1D: goto _LL1F;case 5: _T59=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T3F4;_T5A=_T59->f1;_T5B=_T5A.Int_c;_T5C=_T5B.val;_T5D=_T5C.f1;if(_T5D!=0)goto _TLF1;_LL1F:{struct _tuple14 _T402;
# 897
_T402.f0=inflow;_T5F=fenv;_T402.f1=_T5F->zero;_T5E=_T402;}return _T5E;_TLF1: goto _LL23;case 8: _LL23: goto _LL25;case 9: _LL25: goto _LL27;default: _LL2D: goto _LL2F;};case 1: _T60=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T3F4;_T61=_T60->f1;_T62=(int*)_T61;_T63=*_T62;switch(_T63){case 2: _LL27:{struct _tuple14 _T402;
# 901
_T402.f0=inflow;_T65=fenv;_T402.f1=_T65->notzeroall;_T64=_T402;}return _T64;case 1:{struct _tuple14 _T402;
# 918
_T402.f0=inflow;_T67=fenv;_T68=e;_T69=_T68->topt;_T6A=_check_null(_T69);_T6B=fenv;_T6C=_T6B->unknown_all;_T402.f1=Cyc_CfFlowInfo_typ_to_absrval(_T67,_T6A,0,_T6C);_T66=_T402;}return _T66;case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T3F4;_T6D=_T402->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T403=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T6D;_T401=_T403->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T401;_T6E=vd;_T6F=_T6E->sc;_T70=(int)_T6F;
# 923
if(_T70!=0)goto _TLF4;{struct _tuple14 _T402;
_T402.f0=inflow;_T72=fenv;_T73=e;_T74=_T73->topt;_T75=_check_null(_T74);_T76=fenv;_T77=_T76->unknown_all;_T402.f1=Cyc_CfFlowInfo_typ_to_absrval(_T72,_T75,0,_T77);_T71=_T402;}return _T71;_TLF4: _T401=vd;goto _LL3F;}case 3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T3F4;_T78=_T402->f1;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T403=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T78;_T401=_T403->f1;}}_LL3F: {struct Cyc_Absyn_Vardecl*vd=_T401;_T401=vd;goto _LL41;}case 5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T3F4;_T79=_T402->f1;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T403=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T79;_T401=_T403->f1;}}_LL41: {struct Cyc_Absyn_Vardecl*vd=_T401;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _T402;_T402.tag=0;
# 930
_T402.f1=vd;_T7A=_T402;}{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=_T7A;{struct _tuple14 _T402;
_T402.f0=inflow;_T7C=d;_T7D=Cyc_CfFlowInfo_root_cmp;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T403=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));*_T403=vdroot;_T7E=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_T403;}_T7F=(void*)_T7E;_T402.f1=Cyc_Dict_lookup_other(_T7C,_T7D,_T7F);_T7B=_T402;}return _T7B;}}default: goto _LL7B;};case 41:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FF=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;int static_only=_T3FF;_T80=static_only;
# 904
if(!_T80)goto _TLF6;goto _LL2B;_TLF6: {
struct _tuple17 _T402=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T404;union Cyc_CfFlowInfo_FlowInfo f1f=_T403;{struct _tuple14 _T405;
_T405.f0=f1t;_T82=fenv;_T405.f1=_T82->notzeroall;_T81=_T405;}return _T81;}}}case 30: _T83=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_T3F4;_T84=_T83->f1;if(_T84!=0)goto _TLF8;_LL2B: goto _LL2D;_TLF8:{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FE=_T402->f2;}{struct Cyc_List_List*es=_T401;struct Cyc_Absyn_Datatypedecl*tud=_T3FE;
# 1399
struct _tuple21 _T402=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct Cyc_List_List*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;struct Cyc_List_List*rvals=_T403;_T85=
Cyc_List_length(es);_T86=_T85 + 1;{unsigned sz=(unsigned)_T86;{unsigned _T405=sz;_T89=_check_times(_T405,sizeof(void*));{void**_T406=_cycalloc(_T89);{unsigned _T407=sz;unsigned i;i=0;_TLFD: if(i < _T407)goto _TLFB;else{goto _TLFC;}_TLFB: _T8B=i;_T8A=_T406 + _T8B;_T8C=fenv;
*_T8A=_T8C->unknown_none;i=i + 1;goto _TLFD;_TLFC:;}_T88=(void**)_T406;}_T87=_tag_fat(_T88,sizeof(void*),_T405);}{struct _fat_ptr aggrdict=_T87;_T8D=aggrdict;_T8E=_check_fat_subscript(_T8D,sizeof(void*),0);_T8F=(void**)_T8E;_T90=fenv;
*_T8F=_T90->unknown_all;{
unsigned i=1U;_TL101: if(i < sz)goto _TLFF;else{goto _TL100;}
_TLFF: _T92=aggrdict.curr;_T93=(void**)_T92;_T94=i;_T95=(int)_T94;_T91=_T93 + _T95;_T96=_check_null(rvals);*_T91=_T96->hd;
# 1403
i=i + 1;_T97=rvals;rvals=_T97->tl;goto _TL101;_TL100:;}{struct _tuple14 _T405;
# 1405
_T405.f0=f;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T406=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T406->tag=6;_T406->f1.is_union=0;_T406->f1.fieldnum=-1;_T406->f2=aggrdict;_T99=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T406;}_T405.f1=(void*)_T99;_T98=_T405;}return _T98;}}}}case 42: _LL2F: goto _LL31;case 18: _LL31: goto _LL33;case 17: _LL33: goto _LL35;case 19: _LL35: goto _LL37;case 32: _LL37: goto _LL39;case 31: _LL39:{struct _tuple14 _T402;
# 915
_T402.f0=inflow;_T9B=fenv;_T402.f1=_T9B->unknown_all;_T9A=_T402;}return _T9A;case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T3F4;_T3FD=_T402->f1;_T401=_T402->f2;}{enum Cyc_Absyn_Primop p=_T3FD;struct Cyc_List_List*es=_T401;_T9C=p;_T9D=(int)_T9C;
# 937
if(_T9D!=19)goto _TL102;_T9E=env;_T9F=inflow;_TA0=
# 940
_check_null(es);_TA1=_TA0->hd;_TA2=(struct Cyc_Absyn_Exp*)_TA1;_TA3=local_alias;{struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(_T9E,0,_T9F,_TA2,_TA3);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;void*r=_T403;_TA4=env;_TA5=inflow;_TA6=f;_TA7=es;_TA8=_TA7->hd;_TA9=(struct Cyc_Absyn_Exp*)_TA8;_TAA=r;{
struct _tuple14 _T405=Cyc_NewControlFlow_anal_derefR(_TA4,_TA5,_TA6,_TA9,_TAA,0);void*_T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_T407;void*r2=_T406;_TAB=env;_TAC=es;_TAD=_TAC->hd;_TAE=(struct Cyc_Absyn_Exp*)_TAD;_TAF=_TAE->loc;_TB0=f2;_TB1=r;
f2=Cyc_NewControlFlow_use_Rval(_TAB,_TAF,_TB0,_TB1);{struct _tuple14 _T408;
_T408.f0=f2;_TB3=env;_TB4=_TB3->fenv;_T408.f1=_TB4->unknown_all;_TB2=_T408;}return _TB2;}}}}
# 945
_TL102: {struct _tuple14 _T402=Cyc_NewControlFlow_anal_use_ints(env,inflow,es);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;void*r=_T403;{struct _tuple14 _T405;
_T405.f0=f;_T405.f1=r;_TB5=_T405;}return _TB5;}}}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;}{struct Cyc_Absyn_Exp*e1=_T401;{struct Cyc_List_List _T402;
# 949
_T402.hd=e1;_T402.tl=0;_TB6=_T402;}{struct Cyc_List_List arg=_TB6;_TB7=env;_TB8=inflow;_TB9=& arg;_TBA=(struct Cyc_List_List*)_TB9;{
struct _tuple14 _T402=Cyc_NewControlFlow_anal_use_ints(_TB7,_TB8,_TBA);union Cyc_CfFlowInfo_FlowInfo _T403;_T403=_T402.f0;{union Cyc_CfFlowInfo_FlowInfo f=_T403;
struct _tuple16 _T404=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,e1);union Cyc_CfFlowInfo_AbsLVal _T405;_T405=_T404.f1;{union Cyc_CfFlowInfo_AbsLVal lval=_T405;_TBB=e1;_TBC=_TBB->topt;_TBD=
_check_null(_TBC);{int iszt=Cyc_Tcutil_is_zeroterm_pointer_type(_TBD);_TBE=iszt;
if(!_TBE)goto _TL104;{
# 956
struct _tuple14 _T406=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_T407;union Cyc_CfFlowInfo_FlowInfo _T408;_T408=_T406.f0;_T407=_T406.f1;{union Cyc_CfFlowInfo_FlowInfo g=_T408;void*r=_T407;
Cyc_NewControlFlow_anal_derefR(env,inflow,g,e1,r,0);}}goto _TL105;_TL104: _TL105:{struct _tuple23 _T406;
# 959
_T406.f0=lval;_T406.f1=f;_TBF=_T406;}{struct _tuple23 _T406=_TBF;struct Cyc_Dict_Dict _T407;struct Cyc_CfFlowInfo_Place*_T408;_TC0=_T406.f0;_TC1=_TC0.PlaceL;_TC2=_TC1.tag;if(_TC2!=1)goto _TL106;_TC3=_T406.f1;_TC4=_TC3.ReachableFL;_TC5=_TC4.tag;if(_TC5!=2)goto _TL108;_TC6=_T406.f0;_TC7=_TC6.PlaceL;_T408=_TC7.val;_TC8=_T406.f1;_TC9=_TC8.ReachableFL;_T407=_TC9.val;{struct Cyc_CfFlowInfo_Place*p=_T408;struct Cyc_Dict_Dict outdict=_T407;_TCA=fenv;_TCB=e1;_TCC=_TCB->loc;_TCD=outdict;_TCE=p;_TCF=fenv;_TD0=_TCF->unknown_all;_TD1=
# 961
Cyc_CfFlowInfo_assign_place(_TCA,_TCC,_TCD,_TCE,_TD0);f=Cyc_CfFlowInfo_ReachableFL(_TD1);goto _LL9B;}_TL108: goto _LL9E;_TL106: _LL9E: goto _LL9B;_LL9B:;}_TD2=iszt;
# 968
if(!_TD2)goto _TL10A;{struct _tuple14 _T406;
_T406.f0=f;_TD4=fenv;_T406.f1=_TD4->notzeroall;_TD3=_T406;}return _TD3;
_TL10A:{struct _tuple14 _T406;_T406.f0=f;_TD6=fenv;_T406.f1=_TD6->unknown_all;_TD5=_T406;}return _TD5;}}}}}}case 4: _TD7=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T3F4;_TD8=_TD7->f2;if(_TD8==0)goto _TL10C;{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f3;}{struct Cyc_Absyn_Exp*l=_T401;struct Cyc_Absyn_Exp*r=_T3FC;_TD9=copy_ctxt;
# 973
if(!_TD9)goto _TL10E;_TDA=env;_TDB=_TDA->aquals_bounds;_TDC=e;_TDD=_TDC->topt;_TDE=_check_null(_TDD);_TDF=Cyc_Tcutil_is_noalias_pointer_or_aggr(_TDB,_TDE);if(!_TDF)goto _TL10E;_TE0=e;_TE1=_TE0->loc;_TE2=
_tag_fat("cannot track alias-free pointers through multiple assignments",sizeof(char),62U);_TE3=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_TE1,_TE2,_TE3);{struct _tuple14 _T402;
_T402.f0=Cyc_CfFlowInfo_BottomFL();_TE5=fenv;_T402.f1=_TE5->unknown_all;_TE4=_T402;}return _TE4;_TL10E: {
# 977
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,r,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo right_out=_T404;void*rval=_T403;
struct _tuple16 _T405=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,l);union Cyc_CfFlowInfo_AbsLVal _T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T407;union Cyc_CfFlowInfo_AbsLVal lval=_T406;{struct Cyc_Dict_Dict _T408;_TE6=f;_TE7=_TE6.ReachableFL;_TE8=_TE7.tag;if(_TE8!=2)goto _TL110;_TE9=f;_TEA=_TE9.ReachableFL;_T408=_TEA.val;{struct Cyc_Dict_Dict outdict=_T408;{struct Cyc_CfFlowInfo_Place*_T409;_TEB=lval;_TEC=_TEB.PlaceL;_TED=_TEC.tag;if(_TED!=1)goto _TL112;_TEE=lval;_TEF=_TEE.PlaceL;_T409=_TEF.val;{struct Cyc_CfFlowInfo_Place*p=_T409;_TF0=fenv;_TF1=l;_TF2=_TF1->loc;_TF3=outdict;_TF4=p;_TF5=fenv;_TF6=_TF5->unknown_all;
# 985
outdict=Cyc_CfFlowInfo_assign_place(_TF0,_TF2,_TF3,_TF4,_TF6);
# 987
f=Cyc_CfFlowInfo_ReachableFL(outdict);goto _LLAB;}_TL112: goto _LLAB;_LLAB:;}goto _LLA6;}_TL110: goto _LLA6;_LLA6:;}{struct _tuple14 _T408;
# 999
_T408.f0=f;_TF8=fenv;_T408.f1=_TF8->unknown_all;_TF7=_T408;}return _TF7;}}}}_TL10C:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f3;}{struct Cyc_Absyn_Exp*e1=_T401;struct Cyc_Absyn_Exp*e2=_T3FC;_TF9=copy_ctxt;
# 1002
if(!_TF9)goto _TL114;_TFA=env;_TFB=_TFA->aquals_bounds;_TFC=e;_TFD=_TFC->topt;_TFE=_check_null(_TFD);_TFF=Cyc_Tcutil_is_noalias_pointer_or_aggr(_TFB,_TFE);if(!_TFF)goto _TL114;_T100=e;_T101=_T100->loc;_T102=
_tag_fat("cannot track alias-free pointers through multiple assignments",sizeof(char),62U);_T103=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T101,_T102,_T103);{struct _tuple14 _T402;
_T402.f0=Cyc_CfFlowInfo_BottomFL();_T105=fenv;_T402.f1=_T105->unknown_all;_T104=_T402;}return _T104;_TL114: _T106=env;_T107=inflow;{struct Cyc_Absyn_Exp*_T402[2];_T109=_T402 + 0;
# 1008
*_T109=e1;_T10A=_T402 + 1;*_T10A=e2;_T10B=_tag_fat(_T402,sizeof(struct Cyc_Absyn_Exp*),2);_T108=Cyc_List_list(_T10B);}inflow=Cyc_NewControlFlow_expand_unique_places(_T106,_T107,_T108);{
# 1010
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo right_out=_T404;void*rval=_T403;
struct _tuple16 _T405=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,e1);union Cyc_CfFlowInfo_AbsLVal _T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo outflow=_T407;union Cyc_CfFlowInfo_AbsLVal lval=_T406;_T10C=fenv;_T10D=env;_T10E=outflow;_T10F=e1;_T110=lval;_T111=e2;_T112=rval;_T113=e;_T114=_T113->loc;{
struct _tuple14 _T408=Cyc_NewControlFlow_do_assign(_T10C,_T10D,_T10E,_T10F,_T110,_T111,_T112,_T114);void*_T409;union Cyc_CfFlowInfo_FlowInfo _T40A;_T40A=_T408.f0;_T409=_T408.f1;{union Cyc_CfFlowInfo_FlowInfo outflow=_T40A;void*rval=_T409;{struct _tuple14 _T40B;
# 1016
_T40B.f0=outflow;_T40B.f1=rval;_T115=_T40B;}return _T115;}}}}}}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct Cyc_Absyn_Exp*e2=_T3FC;
# 1019
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;void*r=_T403;_T116=
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f,e2,local_alias);return _T116;}}case 11:{struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;}{struct Cyc_Absyn_Exp*e1=_T401;
# 1023
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;void*r=_T403;_T117=env;_T118=e1;_T119=_T118->loc;_T11A=f;_T11B=r;
Cyc_NewControlFlow_use_Rval(_T117,_T119,_T11A,_T11B);{struct _tuple14 _T405;
_T405.f0=Cyc_CfFlowInfo_BottomFL();_T11D=fenv;_T11E=e;_T11F=_T11E->topt;_T120=_check_null(_T11F);_T121=fenv;_T122=_T121->unknown_all;_T405.f1=Cyc_CfFlowInfo_typ_to_absrval(_T11D,_T120,0,_T122);_T11C=_T405;}return _T11C;}}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FB=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct Cyc_List_List*es=_T3FB;
# 1028
struct Cyc_List_List*orig_es=es;
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T404;void*r1=_T403;
# 1031
Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);_T123=env;_T124=f1;_T125=
# 1034
Cyc_List_copy(es);{struct _tuple21 _T405=Cyc_NewControlFlow_anal_Rexps(_T123,_T124,_T125,1,1);struct Cyc_List_List*_T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo fst_outflow=_T407;struct Cyc_List_List*rvals=_T406;_T126=env;_T127=e1;_T128=_T127->loc;_T129=fst_outflow;_T12A=r1;{
# 1036
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_NewControlFlow_use_Rval(_T126,_T128,_T129,_T12A);
# 1038
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*consume_params=0;_T12B=e1;_T12C=_T12B->topt;_T12D=
_check_null(_T12C);{void*_T408=Cyc_Absyn_compress(_T12D);void*_T409;_T12E=(int*)_T408;_T12F=*_T12E;if(_T12F!=4)goto _TL116;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T40A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T408;_T130=_T40A->f1;_T409=_T130.elt_type;}{void*t=_T409;{
# 1043
void*_T40A=Cyc_Absyn_compress(t);struct Cyc_List_List*_T40B;_T131=(int*)_T40A;_T132=*_T131;if(_T132!=6)goto _TL118;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T40C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T40A;_T133=_T40C->f1;_T40B=_T133.attributes;}{struct Cyc_List_List*atts=_T40B;
# 1045
_TL11D: if(atts!=0)goto _TL11B;else{goto _TL11C;}
# 1047
_TL11B: _T134=atts;{void*_T40C=_T134->hd;int _T40D;_T135=(int*)_T40C;_T136=*_T135;switch(_T136){case 20:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_T40E=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_T40C;_T40D=_T40E->f1;}{int i=_T40D;{struct Cyc_List_List*_T40E=_cycalloc(sizeof(struct Cyc_List_List));_T138=i;
# 1049
_T40E->hd=(void*)_T138;_T40E->tl=init_params;_T137=(struct Cyc_List_List*)_T40E;}init_params=_T137;goto _LLCF;}case 21:{struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_T40E=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_T40C;_T40D=_T40E->f1;}{int i=_T40D;{struct Cyc_List_List*_T40E=_cycalloc(sizeof(struct Cyc_List_List));_T13A=i;
# 1051
_T40E->hd=(void*)_T13A;_T40E->tl=nolive_unique_params;_T139=(struct Cyc_List_List*)_T40E;}nolive_unique_params=_T139;{struct Cyc_List_List*_T40E=_cycalloc(sizeof(struct Cyc_List_List));_T13C=i;
_T40E->hd=(void*)_T13C;_T40E->tl=consume_params;_T13B=(struct Cyc_List_List*)_T40E;}consume_params=_T13B;goto _LLCF;}case 22:{struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_T40E=(struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_T40C;_T40D=_T40E->f1;}{int i=_T40D;{struct Cyc_List_List*_T40E=_cycalloc(sizeof(struct Cyc_List_List));_T13E=i;
# 1055
_T40E->hd=(void*)_T13E;_T40E->tl=consume_params;_T13D=(struct Cyc_List_List*)_T40E;}consume_params=_T13D;goto _LLCF;}default: goto _LLCF;}_LLCF:;}_T13F=atts;
# 1045
atts=_T13F->tl;goto _TL11D;_TL11C: goto _LLCA;}_TL118: _T141=Cyc_Warn_impos;{
# 1060
int(*_T40C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T141;_T140=_T40C;}_T142=_tag_fat("anal_Rexp: bad function type",sizeof(char),29U);_T143=_tag_fat(0U,sizeof(void*),0);_T140(_T142,_T143);_LLCA:;}goto _LLC5;}_TL116: _T145=Cyc_Warn_impos;{
# 1063
int(*_T40A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T145;_T144=_T40A;}_T146=_tag_fat("anal_Rexp: bad function type",sizeof(char),29U);_T147=_tag_fat(0U,sizeof(void*),0);_T144(_T146,_T147);_LLC5:;}{
# 1065
int i=1;_TL122: if(rvals!=0)goto _TL120;else{goto _TL121;}
_TL120: _T149=Cyc_List_memq;{int(*_T408)(struct Cyc_List_List*,int)=(int(*)(struct Cyc_List_List*,int))_T149;_T148=_T408;}_T14A=_T148(init_params,i);if(!_T14A)goto _TL123;{struct Cyc_Dict_Dict _T408;_T14B=fst_outflow;_T14C=_T14B.BottomFL;_T14D=_T14C.tag;if(_T14D!=1)goto _TL125;goto _LLD8;_TL125: _T14E=fst_outflow;_T14F=_T14E.ReachableFL;_T408=_T14F.val;{struct Cyc_Dict_Dict fst_d=_T408;_T150=rvals;_T151=_T150->hd;{
# 1070
struct _tuple13 _T409=Cyc_CfFlowInfo_unname_rval(_T151);void*_T40A;_T40A=_T409.f0;{void*r=_T40A;_T152=env;_T153=_T152->fenv;_T154=fst_d;_T155=rvals;_T156=_T155->hd;_T157=
Cyc_CfFlowInfo_initlevel(_T153,_T154,_T156);_T158=(int)_T157;if(_T158!=0)goto _TL127;_T159=rvals;_T15A=_T159->hd;_T15B=Cyc_CfFlowInfo_is_init_pointer(_T15A);if(_T15B)goto _TL127;else{goto _TL129;}
_TL129: _T15C=es;_T15D=_T15C->hd;_T15E=(struct Cyc_Absyn_Exp*)_T15D;_T15F=_T15E->loc;_T160=_tag_fat("expression may not be initialized",sizeof(char),34U);_T161=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T15F,_T160,_T161);goto _TL128;_TL127: _TL128:{struct Cyc_Dict_Dict _T40B;_T162=outflow;_T163=_T162.BottomFL;_T164=_T163.tag;if(_T164!=1)goto _TL12A;goto _LLE0;_TL12A: _T165=outflow;_T166=_T165.ReachableFL;_T40B=_T166.val;{struct Cyc_Dict_Dict d=_T40B;_T167=fenv;_T168=d;_T169=rvals;_T16A=_T169->hd;{
# 1078
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(_T167,_T168,_T16A);_T16B=rvals;{
void*_T40C=_T16B->hd;struct Cyc_CfFlowInfo_Place*_T40D;_T16C=(int*)_T40C;_T16D=*_T16C;switch(_T16D){case 4:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T40E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T40C;_T40D=_T40E->f1;}{struct Cyc_CfFlowInfo_Place*p=_T40D;_T16E=es;_T16F=_T16E->hd;_T170=(struct Cyc_Absyn_Exp*)_T16F;_T171=_T170->topt;{
# 1081
void*_T40E=Cyc_Absyn_compress(_T171);void*_T40F;_T172=(int*)_T40E;_T173=*_T172;if(_T173!=4)goto _TL12D;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T410=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T40E;_T174=_T410->f1;_T40F=_T174.elt_type;}{void*t=_T40F;_T175=fenv;_T176=es;_T177=_T176->hd;_T178=(struct Cyc_Absyn_Exp*)_T177;_T179=_T178->loc;_T17A=ans_d;_T17B=p;_T17C=fenv;_T17D=t;_T17E=fenv;_T17F=_T17E->esc_all;_T180=
# 1084
Cyc_CfFlowInfo_typ_to_absrval(_T17C,_T17D,0,_T17F);
# 1083
ans_d=Cyc_CfFlowInfo_assign_place(_T175,_T179,_T17A,_T17B,_T180);goto _LLEC;}_TL12D: _T182=Cyc_Warn_impos;{
# 1088
int(*_T410)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T182;_T181=_T410;}_T183=_tag_fat("anal_Rexp:bad type for initialized arg",sizeof(char),39U);_T184=_tag_fat(0U,sizeof(void*),0);_T181(_T183,_T184);_LLEC:;}goto _LLE5;}case 5: _T186=Cyc_Warn_impos;{
# 1092
int(*_T40E)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T186;_T185=_T40E;}_T187=
_tag_fat("anal_Rexp:initialize attribute on unique pointers not yet supported",sizeof(char),68U);_T188=_tag_fat(0U,sizeof(void*),0);
# 1092
_T185(_T187,_T188);default: goto _LLE5;}_LLE5:;}
# 1096
outflow=Cyc_CfFlowInfo_ReachableFL(ans_d);goto _LLE0;}}_LLE0:;}goto _LLD8;}}}_LLD8:;}goto _TL124;
# 1102
_TL123: _T18A=Cyc_List_memq;{int(*_T408)(struct Cyc_List_List*,int)=(int(*)(struct Cyc_List_List*,int))_T18A;_T189=_T408;}_T18B=_T189(nolive_unique_params,i);if(!_T18B)goto _TL12F;_T18C=env;_T18D=
# 1107
_check_null(es);_T18E=_T18D->hd;_T18F=(struct Cyc_Absyn_Exp*)_T18E;_T190=_T18F->loc;_T191=es;_T192=_T191->hd;_T193=(struct Cyc_Absyn_Exp*)_T192;_T194=_T193->topt;_T195=_check_null(_T194);_T196=outflow;_T197=rvals;_T198=_T197->hd;outflow=Cyc_NewControlFlow_use_nounique_Rval(_T18C,_T190,_T195,_T196,_T198);goto _TL130;
# 1117
_TL12F: _T199=env;_T19A=_check_null(es);_T19B=_T19A->hd;_T19C=(struct Cyc_Absyn_Exp*)_T19B;_T19D=_T19C->loc;_T19E=outflow;_T19F=rvals;_T1A0=_T19F->hd;outflow=Cyc_NewControlFlow_use_Rval(_T199,_T19D,_T19E,_T1A0);_T1A2=Cyc_List_memq;{
int(*_T408)(struct Cyc_List_List*,int)=(int(*)(struct Cyc_List_List*,int))_T1A2;_T1A1=_T408;}_T1A3=_T1A1(consume_params,i);if(_T1A3)goto _TL131;else{goto _TL133;}_TL133: _T1A4=env;_T1A5=_T1A4->aquals_bounds;_T1A6=es;_T1A7=_T1A6->hd;_T1A8=(struct Cyc_Absyn_Exp*)_T1A7;_T1A9=_T1A8->topt;_T1AA=
_check_null(_T1A9);_T1AB=Cyc_Tcutil_is_noalias_pointer(_T1A5,_T1AA,0);
# 1118
if(!_T1AB)goto _TL131;_T1AC=env;_T1AD=outflow;_T1AE=es;_T1AF=_T1AE->hd;_T1B0=(struct Cyc_Absyn_Exp*)_T1AF;_T1B1=es;_T1B2=_T1B1->hd;_T1B3=(struct Cyc_Absyn_Exp*)_T1B2;_T1B4=_T1B3->loc;_T1B5=rvals;_T1B6=_T1B5->hd;
# 1120
outflow=Cyc_NewControlFlow_restore_noconsume_arg(_T1AC,_T1AD,_T1B0,_T1B4,_T1B6);goto _TL132;_TL131: _TL132: _TL130: _TL124: _T1B7=rvals;
# 1065
rvals=_T1B7->tl;_T1B8=es;es=_T1B8->tl;i=i + 1;goto _TL122;_TL121:;}_T1B9=e1;_T1BA=_T1B9->topt;_T1BB=
# 1127
_check_null(_T1BA);_T1BC=Cyc_Atts_is_noreturn_fn_type(_T1BB);if(!_T1BC)goto _TL134;{struct _tuple14 _T408;
_T408.f0=Cyc_CfFlowInfo_BottomFL();_T1BE=fenv;_T1BF=e;_T1C0=_T1BF->topt;_T1C1=_check_null(_T1C0);_T1C2=fenv;_T1C3=_T1C2->unknown_all;_T408.f1=Cyc_CfFlowInfo_typ_to_absrval(_T1BE,_T1C1,0,_T1C3);_T1BD=_T408;}return _T1BD;
# 1130
_TL134:{struct _tuple14 _T408;_T408.f0=outflow;_T1C5=fenv;_T1C6=e;_T1C7=_T1C6->topt;_T1C8=_check_null(_T1C7);_T1C9=fenv;_T1CA=_T1C9->unknown_all;_T408.f1=Cyc_CfFlowInfo_typ_to_absrval(_T1C5,_T1C8,0,_T1CA);_T1C4=_T408;}return _T1C4;}}}}}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T3F4;_T1CB=_T402->f1;_T3FA=_T1CB.mknd;_T1CC=_T402->f1;_T401=_T1CC.rgn;_T1CD=_T402->f1;_T3FC=_T1CD.aqual;_T1CE=_T402->f1;_T3F9=_T1CE.elt_type;_T1CF=_T402->f1;_T3F8=_T1CF.num_elts;_T1D0=_T402->f1;_T3FF=_T1D0.fat_result;}{enum Cyc_Absyn_MallocKind mknd=_T3FA;struct Cyc_Absyn_Exp*ropt=_T401;struct Cyc_Absyn_Exp*aqopt=_T3FC;void**topt=_T3F9;struct Cyc_Absyn_Exp*exp=_T3F8;int isfat=_T3FF;
# 1133
void*place_val;_T1D1=isfat;
if(!_T1D1)goto _TL136;_T1D2=fenv;place_val=_T1D2->notzeroall;goto _TL137;
_TL136: _T1D3=mknd;_T1D4=(int)_T1D3;if(_T1D4!=1)goto _TL138;_T1D5=fenv;_T1D6=_check_null(topt);_T1D7=*_T1D6;_T1D8=fenv;_T1D9=_T1D8->zero;place_val=Cyc_CfFlowInfo_typ_to_absrval(_T1D5,_T1D7,0,_T1D9);goto _TL139;
_TL138: _T1DA=fenv;_T1DB=_check_null(topt);_T1DC=*_T1DB;_T1DD=fenv;_T1DE=_T1DD->unknown_none;place_val=Cyc_CfFlowInfo_typ_to_absrval(_T1DA,_T1DC,0,_T1DE);_TL139: _TL137: {
union Cyc_CfFlowInfo_FlowInfo outflow;
if(ropt!=0)goto _TL13A;if(aqopt!=0)goto _TL13A;{
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);union Cyc_CfFlowInfo_FlowInfo _T403;_T403=_T402.f0;{union Cyc_CfFlowInfo_FlowInfo f=_T403;
outflow=f;}}goto _TL13B;
# 1143
_TL13A:{struct Cyc_List_List*expl;expl=_cycalloc(sizeof(struct Cyc_List_List));_T1DF=expl;_T1DF->hd=exp;_T1E0=expl;_T1E0->tl=0;_T1E1=aqopt;_T1E2=(unsigned)_T1E1;
if(!_T1E2)goto _TL13C;{struct Cyc_List_List*_T402=_cycalloc(sizeof(struct Cyc_List_List));
_T402->hd=aqopt;_T402->tl=expl;_T1E3=(struct Cyc_List_List*)_T402;}expl=_T1E3;goto _TL13D;_TL13C: _TL13D: _T1E4=ropt;_T1E5=(unsigned)_T1E4;
# 1147
if(!_T1E5)goto _TL13E;{struct Cyc_List_List*_T402=_cycalloc(sizeof(struct Cyc_List_List));
_T402->hd=ropt;_T402->tl=expl;_T1E6=(struct Cyc_List_List*)_T402;}expl=_T1E6;goto _TL13F;_TL13E: _TL13F: {
# 1150
struct _tuple21 _T402=Cyc_NewControlFlow_anal_Rexps(env,inflow,expl,1,1);struct Cyc_List_List*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;struct Cyc_List_List*rvals=_T403;
outflow=f;}}}_TL13B: _T1E7=env;_T1E8=_T1E7->aquals_bounds;_T1E9=e;_T1EA=_T1E9->topt;_T1EB=
# 1155
_check_null(_T1EA);_T1EC=Cyc_Tcutil_is_noalias_pointer(_T1E8,_T1EB,1);if(!_T1EC)goto _TL140;{struct _tuple14 _T402;
_T402.f0=outflow;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T403=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_T403->tag=5;_T403->f1=place_val;_T1EE=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T403;}_T402.f1=(void*)_T1EE;_T1ED=_T402;}return _T1ED;
# 1159
_TL140:{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T402=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_T402->tag=1;_T402->f1=exp;_T1F0=e;_T1F1=_T1F0->topt;_T402->f2=_check_null(_T1F1);_T1EF=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_T402;}{void*root=(void*)_T1EF;
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T1F2=place;_T1F2->root=root;_T1F3=place;_T1F3->path=0;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T402=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_T402->tag=4;
_T402->f1=place;_T1F4=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T402;}{void*rval=(void*)_T1F4;struct Cyc_Dict_Dict _T402;_T1F5=outflow;_T1F6=_T1F5.BottomFL;_T1F7=_T1F6.tag;if(_T1F7!=1)goto _TL142;{struct _tuple14 _T403;
# 1163
_T403.f0=outflow;_T403.f1=rval;_T1F8=_T403;}return _T1F8;_TL142: _T1F9=outflow;_T1FA=_T1F9.ReachableFL;_T402=_T1FA.val;{struct Cyc_Dict_Dict d2=_T402;{struct _tuple14 _T403;_T1FC=
# 1165
Cyc_Dict_insert(d2,root,place_val);_T403.f0=Cyc_CfFlowInfo_ReachableFL(_T1FC);_T403.f1=rval;_T1FB=_T403;}return _T1FB;};}};}}case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct Cyc_Absyn_Exp*e2=_T3FC;
# 1170
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;_T1FD=env;_T1FE=inflow;{struct Cyc_Absyn_Exp*_T402[2];_T200=_T402 + 0;
# 1176
*_T200=e1;_T201=_T402 + 1;*_T201=e2;_T202=_tag_fat(_T402,sizeof(struct Cyc_Absyn_Exp*),2);_T1FF=Cyc_List_list(_T202);}{struct _tuple21 _T402=Cyc_NewControlFlow_anal_Rexps(_T1FD,_T1FE,_T1FF,0,0);struct Cyc_List_List*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;struct Cyc_List_List*rvals=_T403;_T203=
_check_null(rvals);left_rval=_T203->hd;_T204=rvals;_T205=_T204->tl;_T206=
_check_null(_T205);right_rval=_T206->hd;
outflow=f;{
# 1182
void*t_ign1=Cyc_Absyn_void_type;void*t_ign2=Cyc_Absyn_void_type;
int b_ign1=0;_T207=e1;_T208=& t_ign1;_T209=& b_ign1;_T20A=& t_ign2;_T20B=
Cyc_Tcutil_is_zero_ptr_deref(_T207,_T208,_T209,_T20A);if(!_T20B)goto _TL144;_T20C=right_rval;_T20D=(int*)_T20C;_T20E=*_T20D;if(_T20E!=0)goto _TL146;goto _LLFF;_TL146: _T20F=e1;_T210=_T20F->loc;_T211=
# 1188
_tag_fat("cannot swap value into zeroterm array not known to be 0",sizeof(char),56U);_T212=_tag_fat(0U,sizeof(void*),0);
# 1187
Cyc_CfFlowInfo_aerr(_T210,_T211,_T212);_LLFF: goto _TL145;
# 1193
_TL144: _T213=e2;_T214=& t_ign1;_T215=& b_ign1;_T216=& t_ign2;_T217=Cyc_Tcutil_is_zero_ptr_deref(_T213,_T214,_T215,_T216);if(!_T217)goto _TL148;_T218=left_rval;_T219=(int*)_T218;_T21A=*_T219;if(_T21A!=0)goto _TL14A;goto _LL104;_TL14A: _T21B=e2;_T21C=_T21B->loc;_T21D=
# 1197
_tag_fat("cannot swap value into zeroterm array not known to be 0",sizeof(char),56U);_T21E=_tag_fat(0U,sizeof(void*),0);
# 1196
Cyc_CfFlowInfo_aerr(_T21C,_T21D,_T21E);_LL104: goto _TL149;_TL148: _TL149: _TL145: {
# 1201
struct _tuple16 _T405=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e1);union Cyc_CfFlowInfo_AbsLVal _T406;_T406=_T405.f1;{union Cyc_CfFlowInfo_AbsLVal left_lval=_T406;
struct _tuple16 _T407=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e2);union Cyc_CfFlowInfo_AbsLVal _T408;_T408=_T407.f1;{union Cyc_CfFlowInfo_AbsLVal right_lval=_T408;{struct Cyc_Dict_Dict _T409;_T21F=outflow;_T220=_T21F.ReachableFL;_T221=_T220.tag;if(_T221!=2)goto _TL14C;_T222=outflow;_T223=_T222.ReachableFL;_T409=_T223.val;{struct Cyc_Dict_Dict outdict=_T409;{struct Cyc_CfFlowInfo_Place*_T40A;_T224=left_lval;_T225=_T224.PlaceL;_T226=_T225.tag;if(_T226!=1)goto _TL14E;_T227=left_lval;_T228=_T227.PlaceL;_T40A=_T228.val;{struct Cyc_CfFlowInfo_Place*lp=_T40A;_T229=fenv;_T22A=e1;_T22B=_T22A->loc;_T22C=outdict;_T22D=lp;_T22E=right_rval;
# 1207
outdict=Cyc_CfFlowInfo_assign_place(_T229,_T22B,_T22C,_T22D,_T22E);goto _LL114;}_TL14E: _T22F=
# 1213
Cyc_CfFlowInfo_initlevel(fenv,outdict,right_rval);_T230=(int)_T22F;if(_T230==1)goto _TL150;_T231=e2;_T232=_T231->loc;_T233=
_tag_fat("expression may not be fully initialized",sizeof(char),40U);_T234=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T232,_T233,_T234);goto _TL151;_TL150: _TL151: goto _LL114;_LL114:;}{struct Cyc_CfFlowInfo_Place*_T40A;_T235=right_lval;_T236=_T235.PlaceL;_T237=_T236.tag;if(_T237!=1)goto _TL152;_T238=right_lval;_T239=_T238.PlaceL;_T40A=_T239.val;{struct Cyc_CfFlowInfo_Place*rp=_T40A;_T23A=fenv;_T23B=e2;_T23C=_T23B->loc;_T23D=outdict;_T23E=rp;_T23F=left_rval;
# 1219
outdict=Cyc_CfFlowInfo_assign_place(_T23A,_T23C,_T23D,_T23E,_T23F);goto _LL119;}_TL152: _T240=
# 1222
Cyc_CfFlowInfo_initlevel(fenv,outdict,left_rval);_T241=(int)_T240;if(_T241==1)goto _TL154;_T242=e1;_T243=_T242->loc;_T244=
_tag_fat("expression may not be fully initialized",sizeof(char),40U);_T245=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T243,_T244,_T245);goto _TL155;_TL154: _TL155: goto _LL119;_LL119:;}
# 1228
outflow=Cyc_CfFlowInfo_ReachableFL(outdict);
Cyc_NewControlFlow_update_tryflow(env,outflow);goto _LL10F;}_TL14C: goto _LL10F;_LL10F:;}{struct _tuple14 _T409;
# 1235
_T409.f0=outflow;_T247=fenv;_T409.f1=_T247->unknown_all;_T246=_T409;}return _T246;}}}}}}}case 16:{struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f2;_T3F8=_T402->f3;}{struct Cyc_Absyn_Exp*eopt=_T401;struct Cyc_Absyn_Exp*e2=_T3FC;struct Cyc_Absyn_Exp*qopt=_T3F8;
# 1238
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(eopt!=0)goto _TL156;if(qopt!=0)goto _TL156;{
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;void*r=_T403;
outflow=f;
place_val=r;}}goto _TL157;
# 1246
_TL156:{struct Cyc_List_List*expl;expl=_cycalloc(sizeof(struct Cyc_List_List));_T248=expl;_T248->hd=e2;_T249=expl;_T249->tl=0;{
int depth=0;_T24A=qopt;_T24B=(unsigned)_T24A;
if(!_T24B)goto _TL158;{struct Cyc_List_List*_T402=_cycalloc(sizeof(struct Cyc_List_List));
_T402->hd=qopt;_T402->tl=expl;_T24C=(struct Cyc_List_List*)_T402;}expl=_T24C;
depth=depth + 1;goto _TL159;_TL158: _TL159: _T24D=eopt;_T24E=(unsigned)_T24D;
# 1252
if(!_T24E)goto _TL15A;{struct Cyc_List_List*_T402=_cycalloc(sizeof(struct Cyc_List_List));
_T402->hd=eopt;_T402->tl=expl;_T24F=(struct Cyc_List_List*)_T402;}expl=_T24F;
depth=depth + 1;goto _TL15B;_TL15A: _TL15B: {
# 1256
struct _tuple21 _T402=Cyc_NewControlFlow_anal_Rexps(env,inflow,expl,1,1);struct Cyc_List_List*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;struct Cyc_List_List*rvals=_T403;
outflow=f;
if(depth!=1)goto _TL15C;_T251=_check_null(rvals);_T252=_T251->tl;_T253=_check_null(_T252);_T250=_T253->hd;goto _TL15D;_TL15C: _T254=_check_null(rvals);_T255=_T254->tl;_T256=_check_null(_T255);_T257=_T256->tl;_T258=_check_null(_T257);_T250=_T258->hd;_TL15D: place_val=_T250;}}}}_TL157: _T259=env;_T25A=_T259->aquals_bounds;_T25B=e;_T25C=_T25B->topt;_T25D=
# 1261
_check_null(_T25C);_T25E=Cyc_Tcutil_is_noalias_pointer(_T25A,_T25D,1);if(!_T25E)goto _TL15E;{struct _tuple14 _T402;
_T402.f0=outflow;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T403=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_T403->tag=5;_T403->f1=place_val;_T260=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T403;}_T402.f1=(void*)_T260;_T25F=_T402;}return _T25F;
# 1265
_TL15E:{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T402=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_T402->tag=1;_T402->f1=e2;_T262=e;_T263=_T262->topt;_T402->f2=_check_null(_T263);_T261=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_T402;}{void*root=(void*)_T261;
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T264=place;_T264->root=root;_T265=place;_T265->path=0;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T402=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_T402->tag=4;
_T402->f1=place;_T266=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T402;}{void*rval=(void*)_T266;struct Cyc_Dict_Dict _T402;_T267=outflow;_T268=_T267.BottomFL;_T269=_T268.tag;if(_T269!=1)goto _TL160;{struct _tuple14 _T403;
# 1269
_T403.f0=outflow;_T403.f1=rval;_T26A=_T403;}return _T26A;_TL160: _T26B=outflow;_T26C=_T26B.ReachableFL;_T402=_T26C.val;{struct Cyc_Dict_Dict d2=_T402;{struct _tuple14 _T403;_T26E=
# 1271
Cyc_Dict_insert(d2,root,place_val);_T403.f0=Cyc_CfFlowInfo_ReachableFL(_T26E);_T403.f1=rval;_T26D=_T403;}return _T26D;};}};}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;}{struct Cyc_Absyn_Exp*e1=_T401;
# 1276
struct _tuple16 _T402=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e1);union Cyc_CfFlowInfo_AbsLVal _T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;union Cyc_CfFlowInfo_AbsLVal l=_T403;struct Cyc_CfFlowInfo_Place*_T405;_T26F=l;_T270=_T26F.UnknownL;_T271=_T270.tag;if(_T271!=2)goto _TL162;{struct _tuple14 _T406;
# 1278
_T406.f0=f;_T273=fenv;_T406.f1=_T273->notzeroall;_T272=_T406;}return _T272;_TL162: _T274=l;_T275=_T274.PlaceL;_T405=_T275.val;{struct Cyc_CfFlowInfo_Place*p=_T405;{struct _tuple14 _T406;
_T406.f0=f;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T407=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_T407->tag=4;_T407->f1=p;_T277=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T407;}_T406.f1=(void*)_T277;_T276=_T406;}return _T276;};}}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;}{struct Cyc_Absyn_Exp*e1=_T401;
# 1283
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;void*r=_T403;_T278=
Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,r,0);return _T278;}}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3F7=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct _fat_ptr*field=_T3F7;
# 1287
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;void*r=_T403;_T279=e1;_T27A=_T279->topt;{
void*e1_type=_check_null(_T27A);_T27B=
# 1290
Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type);if(!_T27B)goto _TL164;{struct _tuple14 _T405;
_T405.f0=f;_T27D=fenv;_T27E=e;_T27F=_T27E->topt;_T280=_check_null(_T27F);_T281=fenv;_T282=_T281->unknown_all;_T405.f1=Cyc_CfFlowInfo_typ_to_absrval(_T27D,_T280,0,_T282);_T27C=_T405;}return _T27C;_TL164: _T283=
Cyc_Absyn_is_require_union_type(e1_type);if(!_T283)goto _TL166;_T284=e1;_T285=_T284->loc;_T286=e1_type;_T287=field;_T288=f;
Cyc_NewControlFlow_check_union_requires(_T285,_T286,_T287,_T288);goto _TL167;_TL166: _TL167: {
struct _tuple13 _T405=Cyc_CfFlowInfo_unname_rval(r);void*_T406;_T406=_T405.f0;{void*r=_T406;struct _fat_ptr _T407;int _T408;int _T409;_T289=r;_T28A=(int*)_T289;_T28B=*_T28A;if(_T28B!=6)goto _TL168;_T28C=r;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T40A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T28C;_T28D=_T40A->f1;_T409=_T28D.is_union;_T28E=_T40A->f1;_T408=_T28E.fieldnum;_T407=_T40A->f2;}{int is_union=_T409;int fnum=_T408;struct _fat_ptr rdict=_T407;_T28F=e1;_T290=_T28F->topt;_T291=
# 1297
_check_null(_T290);_T292=field;{int field_no=Cyc_CfFlowInfo_get_field_index(_T291,_T292);_T293=is_union;
if(!_T293)goto _TL16A;if(fnum==-1)goto _TL16A;if(fnum==field_no)goto _TL16A;{struct _tuple14 _T40A;
_T40A.f0=f;_T295=fenv;_T296=e;_T297=_T296->topt;_T298=_check_null(_T297);_T299=fenv;_T29A=_T299->unknown_none;_T40A.f1=Cyc_CfFlowInfo_typ_to_absrval(_T295,_T298,1,_T29A);_T294=_T40A;}return _T294;_TL16A:{struct _tuple14 _T40A;
_T40A.f0=f;_T29C=rdict;_T29D=field_no;_T29E=_check_fat_subscript(_T29C,sizeof(void*),_T29D);_T29F=(void**)_T29E;_T40A.f1=*_T29F;_T29B=_T40A;}return _T29B;}}_TL168: _T2A0=Cyc_stderr;_T2A1=
# 1302
_tag_fat("the bad rexp is :",sizeof(char),18U);_T2A2=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2A0,_T2A1,_T2A2);
Cyc_CfFlowInfo_print_absrval(r);{struct Cyc_String_pa_PrintArg_struct _T40A;_T40A.tag=0;
_T40A.f1=Cyc_Absynpp_exp2string(e);_T2A3=_T40A;}{struct Cyc_String_pa_PrintArg_struct _T40A=_T2A3;void*_T40B[1];_T2A4=_T40B + 0;*_T2A4=& _T40A;_T2A6=Cyc_Warn_impos;{int(*_T40C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2A6;_T2A5=_T40C;}_T2A7=_tag_fat("\nanal_Rexp: AggrMember: %s",sizeof(char),27U);_T2A8=_tag_fat(_T40B,sizeof(void*),1);_T2A5(_T2A7,_T2A8);};}}}}}case 37:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3F7=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct _fat_ptr*field=_T3F7;
# 1310
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;void*r=_T403;_T2A9=e1;_T2AA=_T2A9->topt;_T2AB=
# 1312
_check_null(_T2AA);_T2AC=Cyc_Absyn_is_nontagged_nonrequire_union_type(_T2AB);if(!_T2AC)goto _TL16C;{struct _tuple14 _T405;
_T405.f0=f;_T2AE=fenv;_T405.f1=_T2AE->unknown_all;_T2AD=_T405;}return _T2AD;_TL16C: {
struct _tuple13 _T405=Cyc_CfFlowInfo_unname_rval(r);void*_T406;_T406=_T405.f0;{void*r=_T406;struct _fat_ptr _T407;int _T408;int _T409;_T2AF=r;_T2B0=(int*)_T2AF;_T2B1=*_T2B0;if(_T2B1!=6)goto _TL16E;_T2B2=r;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T40A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T2B2;_T2B3=_T40A->f1;_T409=_T2B3.is_union;_T2B4=_T40A->f1;_T408=_T2B4.fieldnum;_T407=_T40A->f2;}{int is_union=_T409;int fnum=_T408;struct _fat_ptr rdict=_T407;_T2B5=e1;_T2B6=_T2B5->topt;_T2B7=
# 1317
_check_null(_T2B6);_T2B8=field;{int field_no=Cyc_CfFlowInfo_get_field_index(_T2B7,_T2B8);_T2B9=is_union;
if(!_T2B9)goto _TL170;if(fnum==-1)goto _TL170;
if(fnum==field_no)goto _TL172;{struct _tuple14 _T40A;
_T40A.f0=f;_T2BB=fenv;_T40A.f1=_T2BB->zero;_T2BA=_T40A;}return _T2BA;
# 1322
_TL172:{struct _tuple14 _T40A;_T40A.f0=f;_T2BD=fenv;_T40A.f1=_T2BD->notzeroall;_T2BC=_T40A;}return _T2BC;
# 1324
_TL170:{struct _tuple14 _T40A;_T40A.f0=f;_T2BF=fenv;_T40A.f1=_T2BF->unknown_all;_T2BE=_T40A;}return _T2BE;}}_TL16E:{struct Cyc_String_pa_PrintArg_struct _T40A;_T40A.tag=0;
# 1326
_T40A.f1=Cyc_Absynpp_exp2string(e);_T2C0=_T40A;}{struct Cyc_String_pa_PrintArg_struct _T40A=_T2C0;void*_T40B[1];_T2C1=_T40B + 0;*_T2C1=& _T40A;_T2C3=Cyc_Warn_impos;{int(*_T40C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2C3;_T2C2=_T40C;}_T2C4=_tag_fat("anal_Rexp: TagCheck_e: %s",sizeof(char),26U);_T2C5=_tag_fat(_T40B,sizeof(void*),1);_T2C2(_T2C4,_T2C5);};}}}}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3F7=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct _fat_ptr*field=_T3F7;
# 1330
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T404;void*r1=_T403;
# 1333
struct _tuple14 _T405=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);void*_T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_T407;void*r2=_T406;_T2C6=e1;_T2C7=_T2C6->topt;_T2C8=
# 1336
_check_null(_T2C7);{void*_T408=Cyc_Absyn_compress(_T2C8);void*_T409;_T2C9=(int*)_T408;_T2CA=*_T2C9;if(_T2CA!=4)goto _TL174;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T40A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T408;_T2CB=_T40A->f1;_T409=_T2CB.elt_type;}{void*t2=_T409;_T2CC=
# 1338
Cyc_Absyn_is_nontagged_nonrequire_union_type(t2);if(!_T2CC)goto _TL176;{struct _tuple14 _T40A;
# 1340
_T40A.f0=f2;_T2CE=fenv;_T2CF=e;_T2D0=_T2CF->topt;_T2D1=_check_null(_T2D0);_T2D2=fenv;_T2D3=_T2D2->unknown_all;_T40A.f1=Cyc_CfFlowInfo_typ_to_absrval(_T2CE,_T2D1,0,_T2D3);_T2CD=_T40A;}return _T2CD;_TL176: _T2D4=
# 1342
Cyc_Absyn_is_require_union_type(t2);if(!_T2D4)goto _TL178;_T2D5=e1;_T2D6=_T2D5->loc;_T2D7=t2;_T2D8=field;_T2D9=f1;
Cyc_NewControlFlow_check_union_requires(_T2D6,_T2D7,_T2D8,_T2D9);goto _TL179;_TL178: _TL179: {
# 1345
struct _tuple13 _T40A=Cyc_CfFlowInfo_unname_rval(r2);void*_T40B;_T40B=_T40A.f0;{void*r2=_T40B;struct _fat_ptr _T40C;int _T40D;int _T40E;_T2DA=r2;_T2DB=(int*)_T2DA;_T2DC=*_T2DB;if(_T2DC!=6)goto _TL17A;_T2DD=r2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T40F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T2DD;_T2DE=_T40F->f1;_T40E=_T2DE.is_union;_T2DF=_T40F->f1;_T40D=_T2DF.fieldnum;_T40C=_T40F->f2;}{int is_union=_T40E;int fnum=_T40D;struct _fat_ptr rdict=_T40C;
# 1348
int field_no=Cyc_CfFlowInfo_get_field_index(t2,field);_T2E0=is_union;
if(!_T2E0)goto _TL17C;if(fnum==-1)goto _TL17C;if(fnum==field_no)goto _TL17C;{struct _tuple14 _T40F;
_T40F.f0=f2;_T2E2=fenv;_T2E3=e;_T2E4=_T2E3->topt;_T2E5=_check_null(_T2E4);_T2E6=fenv;_T2E7=_T2E6->unknown_none;_T40F.f1=Cyc_CfFlowInfo_typ_to_absrval(_T2E2,_T2E5,1,_T2E7);_T2E1=_T40F;}return _T2E1;_TL17C:{struct _tuple14 _T40F;
_T40F.f0=f2;_T2E9=rdict;_T2EA=field_no;_T2EB=_check_fat_subscript(_T2E9,sizeof(void*),_T2EA);_T2EC=(void**)_T2EB;_T40F.f1=*_T2EC;_T2E8=_T40F;}return _T2E8;}_TL17A: _T2EE=Cyc_Warn_impos;{
int(*_T40F)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2EE;_T2ED=_T40F;}_T2EF=_tag_fat("anal_Rexp: AggrArrow",sizeof(char),21U);_T2F0=_tag_fat(0U,sizeof(void*),0);_T2ED(_T2EF,_T2F0);;}}}goto _TL175;_TL174: _T2F2=Cyc_Warn_impos;{
# 1354
int(*_T40A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2F2;_T2F1=_T40A;}_T2F3=_tag_fat("anal_Rexp: AggrArrow ptr",sizeof(char),25U);_T2F4=_tag_fat(0U,sizeof(void*),0);_T2F1(_T2F3,_T2F4);_TL175:;}}}}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f2;_T3F8=_T402->f3;}{struct Cyc_Absyn_Exp*e1=_T401;struct Cyc_Absyn_Exp*e2=_T3FC;struct Cyc_Absyn_Exp*e3=_T3F8;
# 1358
struct _tuple17 _T402=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T404;union Cyc_CfFlowInfo_FlowInfo f1f=_T403;
struct _tuple14 pr2=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);
struct _tuple14 pr3=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e3,local_alias);_T2F5=
Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);return _T2F5;}}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct Cyc_Absyn_Exp*e2=_T3FC;
# 1364
struct _tuple17 _T402=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T404;union Cyc_CfFlowInfo_FlowInfo f1f=_T403;
struct _tuple14 _T405=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);void*_T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_T407;void*f2r=_T406;{struct _tuple14 _T408;
_T408.f0=f2t;_T408.f1=f2r;_T2F6=_T408;}{struct _tuple14 pr2=_T2F6;{struct _tuple14 _T408;
_T408.f0=f1f;_T2F8=fenv;_T408.f1=_T2F8->zero;_T2F7=_T408;}{struct _tuple14 pr3=_T2F7;_T2F9=
Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);return _T2F9;}}}}}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct Cyc_Absyn_Exp*e2=_T3FC;
# 1371
struct _tuple17 _T402=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T404;union Cyc_CfFlowInfo_FlowInfo f1f=_T403;
struct _tuple14 _T405=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e2,local_alias);void*_T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_T407;void*f2r=_T406;{struct _tuple14 _T408;
_T408.f0=f2t;_T408.f1=f2r;_T2FA=_T408;}{struct _tuple14 pr2=_T2FA;{struct _tuple14 _T408;
_T408.f0=f1t;_T2FC=fenv;_T408.f1=_T2FC->notzeroall;_T2FB=_T408;}{struct _tuple14 pr3=_T2FB;_T2FD=
Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);return _T2FD;}}}}}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f2;}{struct Cyc_Absyn_Exp*e1=_T401;struct Cyc_Absyn_Exp*e2=_T3FC;_T2FE=env;_T2FF=inflow;{struct Cyc_Absyn_Exp*_T402[2];_T301=_T402 + 0;
# 1378
*_T301=e1;_T302=_T402 + 1;*_T302=e2;_T303=_tag_fat(_T402,sizeof(struct Cyc_Absyn_Exp*),2);_T300=Cyc_List_list(_T303);}{struct _tuple21 _T402=Cyc_NewControlFlow_anal_Rexps(_T2FE,_T2FF,_T300,0,1);struct Cyc_List_List*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;struct Cyc_List_List*rvals=_T403;
# 1382
union Cyc_CfFlowInfo_FlowInfo f2=f;{struct Cyc_Dict_Dict _T405;_T304=f;_T305=_T304.ReachableFL;_T306=_T305.tag;if(_T306!=2)goto _TL17E;_T307=f;_T308=_T307.ReachableFL;_T405=_T308.val;{struct Cyc_Dict_Dict d2=_T405;_T309=env;_T30A=_T309->fenv;_T30B=d2;_T30C=
# 1386
_check_null(rvals);_T30D=_T30C->tl;_T30E=_check_null(_T30D);_T30F=_T30E->hd;_T310=Cyc_CfFlowInfo_initlevel(_T30A,_T30B,_T30F);_T311=(int)_T310;if(_T311!=0)goto _TL180;_T312=e2;_T313=_T312->loc;_T314=
_tag_fat("expression may not be initialized",sizeof(char),34U);_T315=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T313,_T314,_T315);goto _TL181;_TL180: _TL181:
 f2=Cyc_CfFlowInfo_ReachableFL(d2);goto _LL16F;}_TL17E: goto _LL16F;_LL16F:;}_T316=env;_T317=inflow;_T318=f;_T319=e1;_T31A=
# 1392
_check_null(rvals);_T31B=_T31A->hd;_T31C=e2;{struct _tuple14 _T405=Cyc_NewControlFlow_anal_derefR(_T316,_T317,_T318,_T319,_T31B,_T31C);void*_T406;union Cyc_CfFlowInfo_FlowInfo _T407;_T407=_T405.f0;_T406=_T405.f1;{union Cyc_CfFlowInfo_FlowInfo f3=_T407;void*r=_T406;_T31D=f3;_T31E=_T31D.BottomFL;_T31F=_T31E.tag;if(_T31F!=1)goto _TL182;{struct _tuple14 _T408;
# 1394
_T408.f0=f3;_T408.f1=r;_T320=_T408;}return _T320;_TL182:{struct _tuple14 _T408;
_T408.f0=f2;_T408.f1=r;_T321=_T408;}return _T321;;}}}}}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f3;}{struct Cyc_List_List*des=_T401;
# 1408
struct Cyc_List_List*fs;_T322=e;_T323=_T322->topt;_T324=
_check_null(_T323);{void*_T402=Cyc_Absyn_compress(_T324);struct Cyc_List_List*_T403;_T325=(int*)_T402;_T326=*_T325;if(_T326!=7)goto _TL184;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T404=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T402;_T403=_T404->f3;}{struct Cyc_List_List*f=_T403;
fs=f;goto _LL17F;}_TL184: _T328=Cyc_Warn_impos;{
int(*_T404)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T328;_T327=_T404;}_T329=_tag_fat("anal_Rexp:anon struct has bad type",sizeof(char),35U);_T32A=_tag_fat(0U,sizeof(void*),0);_T327(_T329,_T32A);_LL17F:;}_T401=des;_T3F6=0U;_T3FB=fs;goto _LL6D;}case 28: _T32B=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T3F4;_T32C=_T32B->f4;if(_T32C==0)goto _TL186;_T32D=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T3F4;_T32E=_T32D->f4;_T32F=(struct Cyc_Absyn_Aggrdecl*)_T32E;_T330=_T32F->impl;if(_T330==0)goto _TL188;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f3;_T331=_T402->f4;{struct Cyc_Absyn_Aggrdecl _T403=*_T331;_T3F6=_T403.kind;_T332=_T403.impl;{struct Cyc_Absyn_AggrdeclImpl _T404=*_T332;_T3FB=_T404.fields;}}}_LL6D:{struct Cyc_List_List*des=_T401;enum Cyc_Absyn_AggrKind kind=_T3F6;struct Cyc_List_List*fs=_T3FB;_T333=e;_T334=_T333->topt;{
# 1415
void*exp_type=_check_null(_T334);_T335=env;_T336=inflow;_T338=Cyc_List_map;{
struct Cyc_List_List*(*_T402)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*))_T338;_T337=_T402;}_T33A=Cyc_Core_snd;{struct Cyc_Absyn_Exp*(*_T402)(struct _tuple24*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple24*))_T33A;_T339=_T402;}_T33B=des;_T33C=_T337(_T339,_T33B);{struct _tuple21 _T402=Cyc_NewControlFlow_anal_Rexps(_T335,_T336,_T33C,1,1);struct Cyc_List_List*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T404;struct Cyc_List_List*rvals=_T403;_T33D=fenv;_T33E=fs;_T33F=kind;_T340=(int)_T33F;_T341=_T340==1;_T342=fenv;_T343=_T342->unknown_all;{
struct _fat_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_T33D,_T33E,_T341,_T343);
# 1420
int field_no=-1;{
int i=0;_TL18D: if(rvals!=0)goto _TL18B;else{goto _TL18C;}
_TL18B: _T344=_check_null(des);_T345=_T344->hd;_T346=(struct _tuple24*)_T345;_T347=*_T346;{struct Cyc_List_List*ds=_T347.f0;_TL191: if(ds!=0)goto _TL18F;else{goto _TL190;}
_TL18F: _T348=ds;{void*_T405=_T348->hd;struct _fat_ptr*_T406;_T349=(int*)_T405;_T34A=*_T349;if(_T34A!=0)goto _TL192;_T34C=Cyc_Warn_impos;{
int(*_T407)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T34C;_T34B=_T407;}_T34D=_tag_fat("anal_Rexp:Aggregate_e",sizeof(char),22U);_T34E=_tag_fat(0U,sizeof(void*),0);_T34B(_T34D,_T34E);goto _TL193;_TL192:{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T407=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T405;_T406=_T407->f1;}{struct _fat_ptr*fld=_T406;
# 1427
field_no=Cyc_CfFlowInfo_get_field_index_fs(fs,fld);_T34F=aggrdict;_T350=field_no;_T351=_check_fat_subscript(_T34F,sizeof(void*),_T350);_T352=(void**)_T351;_T353=rvals;
*_T352=_T353->hd;_T354=kind;_T355=(int)_T354;
# 1430
if(_T355!=1)goto _TL194;_T356=des;_T357=_T356->hd;_T358=(struct _tuple24*)_T357;_T359=*_T358;{
struct Cyc_Absyn_Exp*e=_T359.f1;_T35A=env;_T35B=e;_T35C=_T35B->loc;_T35D=f;_T35E=rvals;_T35F=_T35E->hd;
f=Cyc_NewControlFlow_use_Rval(_T35A,_T35C,_T35D,_T35F);_T360=
# 1434
Cyc_Absyn_is_require_union_type(exp_type);if(!_T360)goto _TL196;_T361=e;_T362=_T361->loc;_T363=exp_type;_T364=fld;_T365=f;
Cyc_NewControlFlow_check_union_requires(_T362,_T363,_T364,_T365);goto _TL197;_TL196: _TL197:;}goto _TL195;_TL194: _TL195:;}_TL193:;}_T366=ds;
# 1422
ds=_T366->tl;goto _TL191;_TL190:;}_T367=rvals;
# 1421
rvals=_T367->tl;_T368=des;des=_T368->tl;i=i + 1;goto _TL18D;_TL18C:;}{
# 1439
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*res;res=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T369=res;_T369->tag=6;_T36A=res;_T36B=kind;_T36C=(int)_T36B;_T36A->f1.is_union=_T36C==1;_T36D=res;_T36D->f1.fieldnum=field_no;_T36E=res;_T36E->f2=aggrdict;{struct _tuple14 _T405;
_T405.f0=f;_T370=res;_T405.f1=(void*)_T370;_T36F=_T405;}return _T36F;}}}}}}_TL188: goto _LL6E;_TL186: _LL6E:{struct Cyc_String_pa_PrintArg_struct _T402;_T402.tag=0;
# 1443
_T402.f1=Cyc_Absynpp_exp2string(e);_T371=_T402;}{struct Cyc_String_pa_PrintArg_struct _T402=_T371;void*_T403[1];_T372=_T403 + 0;*_T372=& _T402;_T374=Cyc_Warn_impos;{int(*_T404)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T374;_T373=_T404;}_T375=_tag_fat("anal_Rexp:missing aggrdeclimpl in %s",sizeof(char),37U);_T376=_tag_fat(_T403,sizeof(void*),1);_T373(_T375,_T376);};case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;}{struct Cyc_List_List*dles=_T401;_T378=Cyc_List_map;{
# 1445
struct Cyc_List_List*(*_T402)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*))_T378;_T377=_T402;}_T37A=Cyc_Core_snd;{struct Cyc_Absyn_Exp*(*_T402)(struct _tuple24*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple24*))_T37A;_T379=_T402;}_T37B=dles;{struct Cyc_List_List*es=_T377(_T379,_T37B);
struct _tuple21 _T402=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct Cyc_List_List*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo outflow=_T404;struct Cyc_List_List*rvals=_T403;
_TL19B: if(rvals!=0)goto _TL199;else{goto _TL19A;}
_TL199: _T37C=env;_T37D=_check_null(es);_T37E=_T37D->hd;_T37F=(struct Cyc_Absyn_Exp*)_T37E;_T380=_T37F->loc;_T381=outflow;_T382=rvals;_T383=_T382->hd;outflow=Cyc_NewControlFlow_use_Rval(_T37C,_T380,_T381,_T383);_T384=rvals;
# 1447
rvals=_T384->tl;_T385=es;es=_T385->tl;goto _TL19B;_TL19A:{struct _tuple14 _T405;
# 1450
_T405.f0=outflow;_T387=fenv;_T388=e;_T389=_T388->topt;_T38A=_check_null(_T389);_T38B=fenv;_T38C=_T38B->notzeroall;_T405.f1=Cyc_CfFlowInfo_typ_to_absrval(_T387,_T38A,0,_T38C);_T386=_T405;}return _T386;}}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T3F4;_T401=_T402->f1;_T3FC=_T402->f2;_T3F8=_T402->f3;_T3FF=_T402->f4;}{struct Cyc_Absyn_Vardecl*vd=_T401;struct Cyc_Absyn_Exp*e1=_T3FC;struct Cyc_Absyn_Exp*e2=_T3F8;int zt=_T3FF;
# 1454
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);void*_T403;union Cyc_CfFlowInfo_FlowInfo _T404;_T404=_T402.f0;_T403=_T402.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T404;void*r1=_T403;struct Cyc_Dict_Dict _T405;_T38D=f1;_T38E=_T38D.BottomFL;_T38F=_T38E.tag;if(_T38F!=1)goto _TL19C;{struct _tuple14 _T406;
# 1456
_T406.f0=f1;_T391=fenv;_T406.f1=_T391->unknown_all;_T390=_T406;}return _T390;_TL19C: _T392=f1;_T393=_T392.ReachableFL;_T405=_T393.val;{struct Cyc_Dict_Dict d1=_T405;_T394=env;_T395=_T394->fenv;_T396=d1;_T397=r1;_T398=
# 1458
Cyc_CfFlowInfo_initlevel(_T395,_T396,_T397);_T399=(int)_T398;if(_T399!=0)goto _TL19E;_T39A=e1;_T39B=_T39A->loc;_T39C=
_tag_fat("expression may not be initialized",sizeof(char),34U);_T39D=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T39B,_T39C,_T39D);goto _TL19F;_TL19E: _TL19F:
# 1462
 f1=Cyc_CfFlowInfo_ReachableFL(d1);_T39E=r1;_T39F=(int*)_T39E;_T3A0=*_T39F;switch(_T3A0){case 0:{struct _tuple14 _T406;
# 1466
_T406.f0=f1;_T3A2=fenv;_T406.f1=_T3A2->unknown_all;_T3A1=_T406;}return _T3A1;case 1: goto _LL19D;case 4: _LL19D:{struct Cyc_List_List _T406;
# 1469
_T406.hd=vd;_T406.tl=0;_T3A3=_T406;}{struct Cyc_List_List l=_T3A3;_T3A4=fenv;_T3A5=f1;_T3A6=& l;_T3A7=(struct Cyc_List_List*)_T3A6;_T3A8=fenv;_T3A9=_T3A8->unknown_all;_T3AA=e;_T3AB=_T3AA->loc;
f1=Cyc_NewControlFlow_add_vars(_T3A4,_T3A5,_T3A7,_T3A9,_T3AB,0);{
# 1472
struct _tuple14 _T406=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);void*_T407;union Cyc_CfFlowInfo_FlowInfo _T408;_T408=_T406.f0;_T407=_T406.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_T408;void*r2=_T407;{struct Cyc_Dict_Dict _T409;_T3AC=f2;_T3AD=_T3AC.BottomFL;_T3AE=_T3AD.tag;if(_T3AE!=1)goto _TL1A1;{struct _tuple14 _T40A;
# 1474
_T40A.f0=f2;_T3B0=fenv;_T40A.f1=_T3B0->unknown_all;_T3AF=_T40A;}return _T3AF;_TL1A1: _T3B1=f2;_T3B2=_T3B1.ReachableFL;_T409=_T3B2.val;{struct Cyc_Dict_Dict d2=_T409;_T3B3=
# 1476
Cyc_CfFlowInfo_initlevel(fenv,d2,r2);_T3B4=(int)_T3B3;if(_T3B4==1)goto _TL1A3;_T3B5=e1;_T3B6=_T3B5->loc;_T3B7=
_tag_fat("expression may not be initialized",sizeof(char),34U);_T3B8=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T3B6,_T3B7,_T3B8);{struct _tuple14 _T40A;
_T40A.f0=Cyc_CfFlowInfo_BottomFL();_T3BA=fenv;_T40A.f1=_T3BA->unknown_all;_T3B9=_T40A;}return _T3B9;_TL1A3:;};}
# 1481
f1=f2;goto _LL19F;}}}default: _LL19F:
# 1484
 _TL1A5: if(1)goto _TL1A6;else{goto _TL1A7;}
_TL1A6:{struct Cyc_List_List _T406;_T406.hd=vd;_T406.tl=0;_T3BB=_T406;}{struct Cyc_List_List l=_T3BB;_T3BC=fenv;_T3BD=f1;_T3BE=& l;_T3BF=(struct Cyc_List_List*)_T3BE;_T3C0=fenv;_T3C1=_T3C0->unknown_all;_T3C2=e;_T3C3=_T3C2->loc;
f1=Cyc_NewControlFlow_add_vars(_T3BC,_T3BD,_T3BF,_T3C1,_T3C3,0);{
struct _tuple14 _T406=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);void*_T407;union Cyc_CfFlowInfo_FlowInfo _T408;_T408=_T406.f0;_T407=_T406.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_T408;void*r2=_T407;{struct Cyc_Dict_Dict _T409;_T3C4=f2;_T3C5=_T3C4.BottomFL;_T3C6=_T3C5.tag;if(_T3C6!=1)goto _TL1A8;goto _LL1AB;_TL1A8: _T3C7=f2;_T3C8=_T3C7.ReachableFL;_T409=_T3C8.val;{struct Cyc_Dict_Dict d2=_T409;_T3C9=
# 1491
Cyc_CfFlowInfo_initlevel(fenv,d2,r2);_T3CA=(int)_T3C9;if(_T3CA==1)goto _TL1AA;_T3CB=e1;_T3CC=_T3CB->loc;_T3CD=
_tag_fat("expression may not be initialized",sizeof(char),34U);_T3CE=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T3CC,_T3CD,_T3CE);{struct _tuple14 _T40A;
_T40A.f0=Cyc_CfFlowInfo_BottomFL();_T3D0=fenv;_T40A.f1=_T3D0->unknown_all;_T3CF=_T40A;}return _T3CF;_TL1AA:;}_LL1AB:;}{
# 1496
union Cyc_CfFlowInfo_FlowInfo newflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);_T3D1=
Cyc_CfFlowInfo_flow_lessthan_approx(newflow,f1);if(!_T3D1)goto _TL1AC;goto _TL1A7;_TL1AC:
# 1499
 f1=newflow;}}}}goto _TL1A5;_TL1A7:{struct _tuple14 _T406;
# 1501
_T406.f0=f1;_T3D3=fenv;_T406.f1=_T3D3->notzeroall;_T3D2=_T406;}return _T3D2;};};}}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T3F4;_T3FC=_T402->f1;_T3D4=_T402->f2;_T401=(void*)_T3D4;_T3FF=_T402->f3;}{struct Cyc_Absyn_Exp*exp=_T3FC;void*typ=_T401;int iszeroterm=_T3FF;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T402=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_T402->tag=1;
# 1507
_T402->f1=exp;_T3D6=e;_T3D7=_T3D6->topt;_T402->f2=_check_null(_T3D7);_T3D5=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_T402;}{void*root=(void*)_T3D5;
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T3D8=place;_T3D8->root=root;_T3D9=place;_T3D9->path=0;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T402=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_T402->tag=4;
_T402->f1=place;_T3DA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T402;}{void*rval=(void*)_T3DA;
void*place_val;_T3DB=fenv;_T3DC=typ;_T3DD=fenv;_T3DE=_T3DD->unknown_none;
# 1515
place_val=Cyc_CfFlowInfo_typ_to_absrval(_T3DB,_T3DC,0,_T3DE);{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple14 _T402=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);union Cyc_CfFlowInfo_FlowInfo _T403;_T403=_T402.f0;{union Cyc_CfFlowInfo_FlowInfo f=_T403;
outflow=f;{struct Cyc_Dict_Dict _T404;_T3DF=outflow;_T3E0=_T3DF.BottomFL;_T3E1=_T3E0.tag;if(_T3E1!=1)goto _TL1AE;{struct _tuple14 _T405;
# 1520
_T405.f0=outflow;_T405.f1=rval;_T3E2=_T405;}return _T3E2;_TL1AE: _T3E3=outflow;_T3E4=_T3E3.ReachableFL;_T404=_T3E4.val;{struct Cyc_Dict_Dict d2=_T404;{struct _tuple14 _T405;_T3E6=
# 1522
Cyc_Dict_insert(d2,root,place_val);_T405.f0=Cyc_CfFlowInfo_ReachableFL(_T3E6);_T405.f1=rval;_T3E5=_T405;}return _T3E5;};}}}}}}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T402=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T3F4;_T3F5=_T402->f1;}{struct Cyc_Absyn_Stmt*s=_T3F5;{struct _tuple15 _T402;_T3E8=env;_T3E9=_T3E8->fenv;
# 1526
_T402.f0=_T3E9->unknown_all;_T402.f1=copy_ctxt;_T3E7=_T402;}{struct _tuple15 rval_opt=_T3E7;_T3EA=env;_T3EB=inflow;_T3EC=s;_T3ED=& rval_opt;{
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_anal_stmt(_T3EA,_T3EB,_T3EC,_T3ED);{struct _tuple14 _T402;
_T402.f0=f;_T3EF=rval_opt;_T402.f1=_T3EF.f0;_T3EE=_T402;}return _T3EE;}}}case 35: _LL7B: goto _LL7D;case 24: _LL7D: goto _LL7F;default: _LL7F: _T3F1=Cyc_Warn_impos;{
# 1534
int(*_T402)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T3F1;_T3F0=_T402;}_T3F2=_tag_fat("anal_Rexp, unexpected exp form",sizeof(char),31U);_T3F3=_tag_fat(0U,sizeof(void*),0);_T3F0(_T3F2,_T3F3);};}}}
# 1546 "new_control_flow.cyc"
static struct _tuple16 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){struct Cyc_NewControlFlow_AnalEnv*_T0;struct Cyc_Absyn_Exp*_T1;void*_T2;void*_T3;int*_T4;int _T5;struct Cyc_Absyn_PtrInfo _T6;struct Cyc_Absyn_PtrAtts _T7;union Cyc_CfFlowInfo_FlowInfo _T8;struct _union_FlowInfo_BottomFL _T9;unsigned _TA;struct _tuple16 _TB;union Cyc_CfFlowInfo_FlowInfo _TC;struct _union_FlowInfo_ReachableFL _TD;void*_TE;int*_TF;unsigned _T10;int(*_T11)(struct _fat_ptr,struct _fat_ptr);void*(*_T12)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T13;struct _fat_ptr _T14;struct Cyc_Absyn_Exp*_T15;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T16;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T17;void*_T18;void*_T19;int _T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T1C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T1D;struct Cyc_Absyn_Exp*_T1E;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T1F;char*_T20;char*_T21;struct Cyc_Absyn_Exp*_T22;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T23;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T24;struct Cyc_Absyn_Exp*_T25;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T26;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T27;int _T28;struct _tuple16 _T29;void*_T2A;struct Cyc_CfFlowInfo_Place*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T2D;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_T2E;int _T2F;struct _tuple16 _T30;struct Cyc_CfFlowInfo_Place*_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_T33;struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_T34;struct _tuple16 _T35;void*_T36;int _T37;struct Cyc_Absyn_Exp*_T38;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T39;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T3A;void*_T3B;void*_T3C;int _T3D;struct Cyc_Absyn_Exp*_T3E;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T3F;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_T40;enum Cyc_CfFlowInfo_InitLevel _T41;int _T42;void*_T43;int*_T44;int _T45;struct Cyc_Absyn_Exp*_T46;unsigned _T47;struct _fat_ptr _T48;struct _fat_ptr _T49;struct Cyc_Absyn_Exp*_T4A;unsigned _T4B;struct _fat_ptr _T4C;struct _fat_ptr _T4D;struct _tuple16 _T4E;int(*_T4F)(struct _fat_ptr,struct _fat_ptr);void*(*_T50)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T51;struct _fat_ptr _T52;_T0=env;{
# 1555
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T0->fenv;_T1=e;_T2=_T1->topt;_T3=
_check_null(_T2);{void*_T53=Cyc_Absyn_compress(_T3);void*_T54;_T4=(int*)_T53;_T5=*_T4;if(_T5!=4)goto _TL1B0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T55=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T53;_T6=_T55->f1;_T7=_T6.ptr_atts;_T54=_T7.bounds;}{void*bd=_T54;struct Cyc_Dict_Dict _T55;_T8=f;_T9=_T8.BottomFL;_TA=_T9.tag;if(_TA!=1)goto _TL1B2;{struct _tuple16 _T56;
# 1559
_T56.f0=f;_T56.f1=Cyc_CfFlowInfo_UnknownL();_TB=_T56;}return _TB;_TL1B2: _TC=f;_TD=_TC.ReachableFL;_T55=_TD.val;{struct Cyc_Dict_Dict outdict=_T55;
# 1562
struct _tuple13 _T56=Cyc_CfFlowInfo_unname_rval(r);struct Cyc_List_List*_T57;void*_T58;_T58=_T56.f0;_T57=_T56.f1;{void*r=_T58;struct Cyc_List_List*names=_T57;
retry:{enum Cyc_CfFlowInfo_InitLevel _T59;struct Cyc_List_List*_T5A;void*_T5B;_TE=r;_TF=(int*)_TE;_T10=*_TF;switch(_T10){case 8: _T12=Cyc_Warn_impos;{
# 1565
int(*_T5C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T12;_T11=_T5C;}_T13=_tag_fat("named location in anal_derefL",sizeof(char),30U);_T14=_tag_fat(0U,sizeof(void*),0);_T11(_T13,_T14);case 1: _T15=e;_T16=& Cyc_CfFlowInfo_NotZero_val;_T17=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_T16;
# 1567
_T15->annot=(void*)_T17;goto _LLD;case 5: _T18=r;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T5C=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T18;_T19=_T5C->f1;_T5B=(void*)_T19;}{void*rv=_T5B;_T1A=expanded_place_possibly_null;
# 1570
if(!_T1A)goto _TL1B5;_T1B=e;_T1C=& Cyc_CfFlowInfo_UnknownZ_val;_T1D=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_T1C;
# 1573
_T1B->annot=(void*)_T1D;goto _TL1B6;
# 1576
_TL1B5: _T1E=e;{void*_T5C=_T1E->annot;_T1F=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_T5C;_T20=_T1F->tag;_T21=Cyc_CfFlowInfo_UnknownZ;if(_T20!=_T21)goto _TL1B7;_T22=e;_T23=& Cyc_CfFlowInfo_UnknownZ_val;_T24=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_T23;
# 1580
_T22->annot=(void*)_T24;goto _LL1E;_TL1B7: _T25=e;_T26=& Cyc_CfFlowInfo_NotZero_val;_T27=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_T26;
# 1583
_T25->annot=(void*)_T27;goto _LL1E;_LL1E:;}_TL1B6: _T28=
# 1588
Cyc_Tcutil_is_bound_one(bd,1);if(!_T28)goto _TL1B9;{struct _tuple16 _T5C;_T5C.f0=f;_T5C.f1=Cyc_CfFlowInfo_UnknownL();_T29=_T5C;}return _T29;_TL1B9: goto _LLD;}case 4: _T2A=r;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T5C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T2A;_T2B=_T5C->f1;{struct Cyc_CfFlowInfo_Place _T5D=*_T2B;_T5B=_T5D.root;_T5A=_T5D.path;}}{void*root=_T5B;struct Cyc_List_List*path2=_T5A;_T2C=e;_T2D=& Cyc_CfFlowInfo_NotZero_val;_T2E=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_T2D;
# 1592
_T2C->annot=(void*)_T2E;_T2F=
Cyc_Tcutil_is_bound_one(bd,0);if(!_T2F)goto _TL1BB;{struct _tuple16 _T5C;
_T5C.f0=f;{struct Cyc_CfFlowInfo_Place*_T5D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T5D->root=root;_T5D->path=Cyc_List_append(path2,path);_T31=(struct Cyc_CfFlowInfo_Place*)_T5D;}_T5C.f1=Cyc_CfFlowInfo_PlaceL(_T31);_T30=_T5C;}return _T30;_TL1BB: goto _LLD;}case 0: _T32=e;_T33=& Cyc_CfFlowInfo_IsZero_val;_T34=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_T33;
# 1597
_T32->annot=(void*)_T34;{struct _tuple16 _T5C;
_T5C.f0=Cyc_CfFlowInfo_BottomFL();_T5C.f1=Cyc_CfFlowInfo_UnknownL();_T35=_T5C;}return _T35;case 2: _T36=r;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T5C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T36;_T59=_T5C->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_T59;_T37=
# 1602
Cyc_Tcutil_is_bound_one(bd,0);if(!_T37)goto _TL1BD;
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);goto _TL1BE;_TL1BD: _TL1BE: _T38=e;_T39=& Cyc_CfFlowInfo_UnknownZ_val;_T3A=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_T39;
_T38->annot=(void*)_T3A;goto _LLD;}case 7: _T3B=r;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T5C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T3B;_T3C=_T5C->f3;_T5B=(void*)_T3C;}{void*r2=_T5B;_T3D=passthrough_consumes;
# 1607
if(!_T3D)goto _TL1BF;
r=r2;goto retry;_TL1BF: goto _LL1D;}default: _LL1D: _T3E=e;_T3F=& Cyc_CfFlowInfo_UnknownZ_val;_T40=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_T3F;
# 1612
_T3E->annot=(void*)_T40;goto _LLD;}_LLD:;}_T41=
# 1615
Cyc_CfFlowInfo_initlevel(fenv,outdict,r);_T42=(int)_T41;if(_T42!=0)goto _TL1C1;{
struct _tuple13 _T59=Cyc_CfFlowInfo_unname_rval(r);void*_T5A;_T5A=_T59.f0;{void*r=_T5A;_T43=r;_T44=(int*)_T43;_T45=*_T44;if(_T45!=7)goto _TL1C3;_T46=e;_T47=_T46->loc;_T48=
# 1619
_tag_fat("attempt to dereference an alias-free that has already been copied",sizeof(char),66U);_T49=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T47,_T48,_T49);goto _LL26;_TL1C3: _T4A=e;_T4B=_T4A->loc;_T4C=
# 1622
_tag_fat("dereference of possibly uninitialized pointer",sizeof(char),46U);_T4D=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T4B,_T4C,_T4D);goto _LL26;_LL26:;}}goto _TL1C2;_TL1C1: _TL1C2:{struct _tuple16 _T59;
# 1626
_T59.f0=f;_T59.f1=Cyc_CfFlowInfo_UnknownL();_T4E=_T59;}return _T4E;}};}goto _TL1B1;_TL1B0: _T50=Cyc_Warn_impos;{
# 1628
int(*_T55)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T50;_T4F=_T55;}_T51=_tag_fat("left deref of non-pointer-type",sizeof(char),31U);_T52=_tag_fat(0U,sizeof(void*),0);_T4F(_T51,_T52);_TL1B1:;}}}
# 1634
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){struct Cyc_CfFlowInfo_Place*_T0;struct Cyc_List_List*_T1;struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_T2;struct Cyc_CfFlowInfo_Place*_T3;struct Cyc_List_List*_T4;void*_T5;_T0=p;{struct Cyc_CfFlowInfo_Place _T6=*_T0;_T5=_T6.root;_T4=_T6.path;}{void*proot=_T5;struct Cyc_List_List*ppath=_T4;{struct Cyc_List_List*_T6=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_T7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct));
# 1636
_T7->tag=1;_T2=(struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_T7;}_T6->hd=(void*)_T2;_T6->tl=path;_T1=(struct Cyc_List_List*)_T6;}path=_T1;{struct Cyc_CfFlowInfo_Place*_T6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));
_T6->root=proot;_T6->path=Cyc_List_append(ppath,path);_T3=(struct Cyc_CfFlowInfo_Place*)_T6;}return _T3;}}
# 1644
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){struct Cyc_NewControlFlow_AnalEnv*_T0;union Cyc_CfFlowInfo_FlowInfo _T1;struct _union_FlowInfo_BottomFL _T2;unsigned _T3;struct _tuple16 _T4;union Cyc_CfFlowInfo_FlowInfo _T5;struct _union_FlowInfo_ReachableFL _T6;struct Cyc_Absyn_Exp*_T7;int*_T8;unsigned _T9;struct _tuple16 _TA;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB;void*_TC;int*_TD;unsigned _TE;void*_TF;struct Cyc_Absyn_Vardecl*_T10;enum Cyc_Absyn_Scope _T11;int _T12;struct _tuple16 _T13;void*_T14;void*_T15;struct _tuple16 _T16;struct Cyc_CfFlowInfo_Place*_T17;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T18;void*_T19;struct _tuple16 _T1A;struct Cyc_Absyn_Exp*_T1B;void*_T1C;void*_T1D;int*_T1E;int _T1F;struct Cyc_Absyn_PtrInfo _T20;int _T21;int _T22;struct Cyc_Absyn_Exp*_T23;unsigned _T24;void*_T25;struct _fat_ptr*_T26;union Cyc_CfFlowInfo_FlowInfo _T27;struct Cyc_List_List*_T28;struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_T29;int(*_T2A)(struct _fat_ptr,struct _fat_ptr);void*(*_T2B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2C;struct _fat_ptr _T2D;struct Cyc_NewControlFlow_AnalEnv*_T2E;struct Cyc_List_List*_T2F;struct Cyc_Absyn_Exp*_T30;void*_T31;void*_T32;int _T33;struct _tuple16 _T34;union Cyc_CfFlowInfo_FlowInfo _T35;struct _union_FlowInfo_ReachableFL _T36;unsigned _T37;union Cyc_CfFlowInfo_AbsLVal _T38;struct _union_AbsLVal_PlaceL _T39;unsigned _T3A;union Cyc_CfFlowInfo_FlowInfo _T3B;struct _union_FlowInfo_ReachableFL _T3C;union Cyc_CfFlowInfo_AbsLVal _T3D;struct _union_AbsLVal_PlaceL _T3E;int _T3F;void*_T40;int*_T41;unsigned _T42;void*_T43;void*_T44;int(*_T45)(struct _fat_ptr,struct _fat_ptr);void*(*_T46)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T47;struct _fat_ptr _T48;void*_T49;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T4A;void*_T4B;int*_T4C;unsigned _T4D;struct _tuple16 _T4E;struct _tuple16 _T4F;struct Cyc_CfFlowInfo_Place*_T50;void*_T51;enum Cyc_CfFlowInfo_InitLevel _T52;int _T53;struct Cyc_CfFlowInfo_FlowEnv*_T54;struct Cyc_CfFlowInfo_FlowEnv*_T55;struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T56;struct Cyc_CfFlowInfo_FlowEnv*_T57;struct Cyc_Absyn_Exp*_T58;void*_T59;void*_T5A;void*_T5B;void*_T5C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T5D;struct Cyc_List_List*_T5E;void*_T5F;struct Cyc_List_List*_T60;struct Cyc_CfFlowInfo_FlowEnv*_T61;struct Cyc_Absyn_Exp*_T62;unsigned _T63;struct Cyc_Dict_Dict _T64;struct Cyc_CfFlowInfo_Place*_T65;void*_T66;struct _tuple16 _T67;struct Cyc_CfFlowInfo_Place*_T68;void*_T69;int*_T6A;unsigned _T6B;void*_T6C;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T6D;void*_T6E;int*_T6F;int _T70;struct _tuple16 _T71;struct Cyc_CfFlowInfo_Place*_T72;struct _tuple16 _T73;struct Cyc_NewControlFlow_AnalEnv*_T74;union Cyc_CfFlowInfo_FlowInfo _T75;struct Cyc_List_List*_T76;struct Cyc_Absyn_Exp**_T77;struct Cyc_Absyn_Exp**_T78;struct _fat_ptr _T79;union Cyc_CfFlowInfo_FlowInfo _T7A;struct _union_FlowInfo_ReachableFL _T7B;unsigned _T7C;union Cyc_CfFlowInfo_FlowInfo _T7D;struct _union_FlowInfo_ReachableFL _T7E;struct Cyc_CfFlowInfo_FlowEnv*_T7F;struct Cyc_Dict_Dict _T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;struct Cyc_List_List*_T83;void*_T84;enum Cyc_CfFlowInfo_InitLevel _T85;int _T86;struct Cyc_Absyn_Exp*_T87;unsigned _T88;struct _fat_ptr _T89;struct _fat_ptr _T8A;struct Cyc_NewControlFlow_AnalEnv*_T8B;union Cyc_CfFlowInfo_FlowInfo _T8C;struct Cyc_Absyn_Exp*_T8D;union Cyc_CfFlowInfo_FlowInfo _T8E;struct Cyc_List_List*_T8F;void*_T90;int _T91;struct Cyc_List_List*_T92;union Cyc_CfFlowInfo_FlowInfo _T93;struct _union_FlowInfo_BottomFL _T94;unsigned _T95;struct _tuple16 _T96;struct _tuple16 _T97;struct Cyc_Absyn_Exp*_T98;void*_T99;int _T9A;struct Cyc_Absyn_Exp*_T9B;unsigned _T9C;void*_T9D;struct _fat_ptr*_T9E;union Cyc_CfFlowInfo_FlowInfo _T9F;int _TA0;struct _tuple16 _TA1;struct Cyc_NewControlFlow_AnalEnv*_TA2;union Cyc_CfFlowInfo_FlowInfo _TA3;int _TA4;int _TA5;struct Cyc_Absyn_Exp*_TA6;struct Cyc_List_List*_TA7;struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_TA8;struct _tuple16 _TA9;struct _tuple16 _TAA;
# 1648
struct Cyc_Dict_Dict d;_T0=env;{
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T0->fenv;{struct Cyc_Dict_Dict _TAB;_T1=inflow;_T2=_T1.BottomFL;_T3=_T2.tag;if(_T3!=1)goto _TL1C5;{struct _tuple16 _TAC;
# 1651
_TAC.f0=Cyc_CfFlowInfo_BottomFL();_TAC.f1=Cyc_CfFlowInfo_UnknownL();_T4=_TAC;}return _T4;_TL1C5: _T5=inflow;_T6=_T5.ReachableFL;_TAB=_T6.val;{struct Cyc_Dict_Dict d2=_TAB;
# 1653
d=d2;};}_T7=e;{
# 1656
void*_TAB=_T7->r;struct Cyc_Absyn_Exp*_TAC;struct _fat_ptr*_TAD;struct Cyc_Absyn_Vardecl*_TAE;struct Cyc_Absyn_Exp*_TAF;_T8=(int*)_TAB;_T9=*_T8;switch(_T9){case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TAB;_TAF=_TB0->f2;}{struct Cyc_Absyn_Exp*e1=_TAF;_TAF=e1;goto _LL9;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_TAB;_TAF=_TB0->f1;}_LL9: {struct Cyc_Absyn_Exp*e1=_TAF;_TAF=e1;goto _LLB;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_TAB;_TAF=_TB0->f1;}_LLB: {struct Cyc_Absyn_Exp*e1=_TAF;_TA=
# 1659
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e1,path);return _TA;}case 1: _TB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TAB;_TC=_TB->f1;_TD=(int*)_TC;_TE=*_TD;switch(_TE){case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TAB;_TF=_TB0->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TB1=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TF;_TAE=_TB1->f1;}}{struct Cyc_Absyn_Vardecl*vd=_TAE;_T10=vd;_T11=_T10->sc;_T12=(int)_T11;
# 1662
if(_T12!=0)goto _TL1C9;{struct _tuple16 _TB0;
_TB0.f0=inflow;_TB0.f1=Cyc_CfFlowInfo_UnknownL();_T13=_TB0;}return _T13;_TL1C9: _TAE=vd;goto _LLF;}case 3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TAB;_T14=_TB0->f1;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_TB1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T14;_TAE=_TB1->f1;}}_LLF: {struct Cyc_Absyn_Vardecl*vd=_TAE;_TAE=vd;goto _LL11;}case 5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TAB;_T15=_TB0->f1;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_TB1=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T15;_TAE=_TB1->f1;}}_LL11: {struct Cyc_Absyn_Vardecl*vd=_TAE;{struct _tuple16 _TB0;
# 1667
_TB0.f0=inflow;{struct Cyc_CfFlowInfo_Place*_TB1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_TB2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_TB2->tag=0;_TB2->f1=vd;_T18=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_TB2;}_TB1->root=(void*)_T18;_TB1->path=path;_T17=(struct Cyc_CfFlowInfo_Place*)_TB1;}_TB0.f1=Cyc_CfFlowInfo_PlaceL(_T17);_T16=_TB0;}return _T16;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TAB;_T19=_TB0->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_TB1=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T19;_TAE=_TB1->f1;}}{struct Cyc_Absyn_Vardecl*vd=_TAE;{struct _tuple16 _TB0;
# 1669
_TB0.f0=inflow;_TB0.f1=Cyc_CfFlowInfo_UnknownL();_T1A=_TB0;}return _T1A;}default: goto _LL1C;};case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_TAB;_TAF=_TB0->f1;_TAD=_TB0->f2;}{struct Cyc_Absyn_Exp*e1=_TAF;struct _fat_ptr*f=_TAD;_T1B=e1;_T1C=_T1B->topt;_T1D=
# 1672
_check_null(_T1C);{void*_TB0=Cyc_Absyn_compress(_T1D);void*_TB1;_T1E=(int*)_TB0;_T1F=*_T1E;if(_T1F!=4)goto _TL1CB;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TB2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TB0;_T20=_TB2->f1;_TB1=_T20.elt_type;}{void*t2=_TB1;_T21=
# 1674
Cyc_Absyn_is_nontagged_nonrequire_union_type(t2);if(_T21)goto _TL1CD;else{goto _TL1CF;}
_TL1CF: _T22=Cyc_Absyn_is_require_union_type(t2);if(!_T22)goto _TL1D0;_T23=e1;_T24=_T23->loc;_T25=t2;_T26=f;_T27=inflow;
Cyc_NewControlFlow_check_union_requires(_T24,_T25,_T26,_T27);goto _TL1D1;_TL1D0: _TL1D1:{struct Cyc_List_List*_TB2=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_TB3=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct));_TB3->tag=0;
# 1678
_TB3->f1=Cyc_CfFlowInfo_get_field_index(t2,f);_T29=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_TB3;}_TB2->hd=(void*)_T29;_TB2->tl=path;_T28=(struct Cyc_List_List*)_TB2;}path=_T28;goto _TL1CE;_TL1CD: _TL1CE: goto _LL1E;}_TL1CB: _T2B=Cyc_Warn_impos;{
# 1681
int(*_TB2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2B;_T2A=_TB2;}_T2C=_tag_fat("anal_Lexp: AggrArrow ptr",sizeof(char),25U);_T2D=_tag_fat(0U,sizeof(void*),0);_T2A(_T2C,_T2D);_LL1E:;}_TAF=e1;goto _LL17;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_TAB;_TAF=_TB0->f1;}_LL17: {struct Cyc_Absyn_Exp*e1=_TAF;_T2E=env;_T2F=_T2E->aquals_bounds;_T30=e1;_T31=_T30->topt;_T32=
# 1687
_check_null(_T31);_T33=Cyc_Tcutil_is_noalias_pointer(_T2F,_T32,1);if(!_T33)goto _TL1D2;{
# 1689
struct _tuple16 _TB0=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,e1);union Cyc_CfFlowInfo_AbsLVal _TB1;union Cyc_CfFlowInfo_FlowInfo _TB2;_TB2=_TB0.f0;_TB1=_TB0.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TB2;union Cyc_CfFlowInfo_AbsLVal lval=_TB1;{struct _tuple16 _TB3;
_TB3.f0=f;_TB3.f1=lval;_T34=_TB3;}{struct _tuple16 _TB3=_T34;struct Cyc_CfFlowInfo_Place*_TB4;struct Cyc_Dict_Dict _TB5;_T35=_TB3.f0;_T36=_T35.ReachableFL;_T37=_T36.tag;if(_T37!=2)goto _TL1D4;_T38=_TB3.f1;_T39=_T38.PlaceL;_T3A=_T39.tag;if(_T3A!=1)goto _TL1D6;_T3B=_TB3.f0;_T3C=_T3B.ReachableFL;_TB5=_T3C.val;_T3D=_TB3.f1;_T3E=_T3D.PlaceL;_TB4=_T3E.val;{struct Cyc_Dict_Dict fd=_TB5;struct Cyc_CfFlowInfo_Place*p=_TB4;
# 1692
void*old_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
struct _tuple13 _TB6=Cyc_CfFlowInfo_unname_rval(old_rval);struct Cyc_List_List*_TB7;void*_TB8;_TB8=_TB6.f0;_TB7=_TB6.f1;{void*rval=_TB8;struct Cyc_List_List*names=_TB7;_T3F=expand_unique;
if(!_T3F)goto _TL1D8;{
int possibly_null=1;void*_TB9;struct Cyc_Absyn_Vardecl*_TBA;_T40=rval;_T41=(int*)_T40;_T42=*_T41;switch(_T42){case 8: _T43=rval;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TBB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T43;_TBA=_TBB->f1;_T44=_TBB->f2;_TB9=(void*)_T44;}{struct Cyc_Absyn_Vardecl*n=_TBA;void*r=_TB9;_T46=Cyc_Warn_impos;{
# 1698
int(*_TBB)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T46;_T45=_TBB;}_T47=_tag_fat("bad named location in anal_Lexp:deref",sizeof(char),38U);_T48=_tag_fat(0U,sizeof(void*),0);_T45(_T47,_T48);}case 7: _T49=rval;_T4A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T49;_T4B=_T4A->f3;_T4C=(int*)_T4B;_T4D=*_T4C;switch(_T4D){case 4: goto _LL34;case 5: goto _LL38;default: goto _LL3B;};case 4: _LL34: _T4E=
# 1701
Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);return _T4E;case 5: _LL38: {
# 1706
struct _tuple16 _TBB=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);union Cyc_CfFlowInfo_FlowInfo _TBC;_TBC=_TBB.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TBC;{struct _tuple16 _TBD;
_TBD.f0=f;_T50=Cyc_NewControlFlow_make_expanded_place(p,path);_TBD.f1=Cyc_CfFlowInfo_PlaceL(_T50);_T4F=_TBD;}return _T4F;}}case 1:
# 1709
 possibly_null=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1716
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(fenv,fd,rval);_T52=il;_T53=(int)_T52;
if(_T53!=1)goto _TL1DC;_T54=fenv;_T51=_T54->unknown_all;goto _TL1DD;_TL1DC: _T55=fenv;_T51=_T55->unknown_none;_TL1DD: {void*leaf=_T51;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TBB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_TBB->tag=5;_T57=fenv;_T58=e1;_T59=_T58->topt;_T5A=
_check_null(_T59);_T5B=Cyc_Tcutil_pointer_elt_type(_T5A);_T5C=leaf;_TBB->f1=Cyc_CfFlowInfo_typ_to_absrval(_T57,_T5B,0,_T5C);_T56=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_TBB;}{void*res=(void*)_T56;
_TL1E1: if(names!=0)goto _TL1DF;else{goto _TL1E0;}
_TL1DF:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TBB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_TBB->tag=8;_T5E=names;_T5F=_T5E->hd;_TBB->f1=(struct Cyc_Absyn_Vardecl*)_T5F;_TBB->f2=res;_T5D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TBB;}res=(void*)_T5D;_T60=names;
# 1719
names=_T60->tl;goto _TL1E1;_TL1E0: _T61=fenv;_T62=e;_T63=_T62->loc;_T64=fd;_T65=p;_T66=res;
# 1721
fd=Cyc_CfFlowInfo_assign_place(_T61,_T63,_T64,_T65,_T66);{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_ReachableFL(fd);
# 1729
struct _tuple16 _TBB=Cyc_NewControlFlow_anal_derefL(env,outflow,e1,outflow,res,passthrough_consumes,possibly_null,path);union Cyc_CfFlowInfo_FlowInfo _TBC;_TBC=_TBB.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TBC;{struct _tuple16 _TBD;
# 1731
_TBD.f0=f;_T68=Cyc_NewControlFlow_make_expanded_place(p,path);_TBD.f1=Cyc_CfFlowInfo_PlaceL(_T68);_T67=_TBD;}return _T67;}}}}}};}goto _TL1D9;
# 1735
_TL1D8: _T69=rval;_T6A=(int*)_T69;_T6B=*_T6A;switch(_T6B){case 7: _T6C=rval;_T6D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T6C;_T6E=_T6D->f3;_T6F=(int*)_T6E;_T70=*_T6F;if(_T70!=5)goto _TL1E3;goto _LL47;_TL1E3: goto _LL48;case 5: _LL47: {
# 1740
struct _tuple16 _TB9=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);union Cyc_CfFlowInfo_FlowInfo _TBA;_TBA=_TB9.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TBA;{struct _tuple16 _TBB;
_TBB.f0=f;_T72=Cyc_NewControlFlow_make_expanded_place(p,path);_TBB.f1=Cyc_CfFlowInfo_PlaceL(_T72);_T71=_TBB;}return _T71;}}default: _LL48: goto _LL43;}_LL43: _TL1D9: goto _LL26;}}_TL1D6: goto _LL29;_TL1D4: _LL29: goto _LL26;_LL26:;}}}goto _TL1D3;_TL1D2: _TL1D3: {
# 1752
struct _tuple14 _TB0=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);void*_TB1;union Cyc_CfFlowInfo_FlowInfo _TB2;_TB2=_TB0.f0;_TB1=_TB0.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TB2;void*r=_TB1;_T73=
# 1754
Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,r,passthrough_consumes,0,path);return _T73;}}}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_TAB;_TAF=_TB0->f1;_TAC=_TB0->f2;}{struct Cyc_Absyn_Exp*e1=_TAF;struct Cyc_Absyn_Exp*e2=_TAC;_T74=env;_T75=inflow;{struct Cyc_Absyn_Exp*_TB0[2];_T77=_TB0 + 0;
# 1756
*_T77=e1;_T78=_TB0 + 1;*_T78=e2;_T79=_tag_fat(_TB0,sizeof(struct Cyc_Absyn_Exp*),2);_T76=Cyc_List_list(_T79);}{struct _tuple21 _TB0=Cyc_NewControlFlow_anal_Rexps(_T74,_T75,_T76,0,1);struct Cyc_List_List*_TB1;union Cyc_CfFlowInfo_FlowInfo _TB2;_TB2=_TB0.f0;_TB1=_TB0.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TB2;struct Cyc_List_List*rvals=_TB1;
union Cyc_CfFlowInfo_FlowInfo f2=f;{struct Cyc_Dict_Dict _TB3;_T7A=f;_T7B=_T7A.ReachableFL;_T7C=_T7B.tag;if(_T7C!=2)goto _TL1E5;_T7D=f;_T7E=_T7D.ReachableFL;_TB3=_T7E.val;{struct Cyc_Dict_Dict d2=_TB3;_T7F=fenv;_T80=d2;_T81=
# 1760
_check_null(rvals);_T82=_T81->tl;_T83=_check_null(_T82);_T84=_T83->hd;_T85=Cyc_CfFlowInfo_initlevel(_T7F,_T80,_T84);_T86=(int)_T85;if(_T86!=0)goto _TL1E7;_T87=e2;_T88=_T87->loc;_T89=
_tag_fat("expression may not be initialized",sizeof(char),34U);_T8A=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T88,_T89,_T8A);goto _TL1E8;_TL1E7: _TL1E8:
 f2=Cyc_CfFlowInfo_ReachableFL(d2);goto _LL53;}_TL1E5: goto _LL53;_LL53:;}_T8B=env;_T8C=inflow;_T8D=e1;_T8E=f;_T8F=
# 1767
_check_null(rvals);_T90=_T8F->hd;_T91=passthrough_consumes;_T92=path;{struct _tuple16 _TB3=Cyc_NewControlFlow_anal_derefL(_T8B,_T8C,_T8D,_T8E,_T90,_T91,0,_T92);union Cyc_CfFlowInfo_AbsLVal _TB4;union Cyc_CfFlowInfo_FlowInfo _TB5;_TB5=_TB3.f0;_TB4=_TB3.f1;{union Cyc_CfFlowInfo_FlowInfo f3=_TB5;union Cyc_CfFlowInfo_AbsLVal r=_TB4;_T93=f3;_T94=_T93.BottomFL;_T95=_T94.tag;if(_T95!=1)goto _TL1E9;{struct _tuple16 _TB6;
# 1769
_TB6.f0=f3;_TB6.f1=r;_T96=_TB6;}return _T96;_TL1E9:{struct _tuple16 _TB6;
_TB6.f0=f2;_TB6.f1=r;_T97=_TB6;}return _T97;;}}}}}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_TB0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_TAB;_TAF=_TB0->f1;_TAD=_TB0->f2;}{struct Cyc_Absyn_Exp*e1=_TAF;struct _fat_ptr*fld=_TAD;_T98=e1;_T99=_T98->topt;{
# 1774
void*e1_type=_check_null(_T99);_T9A=
Cyc_Absyn_is_require_union_type(e1_type);if(!_T9A)goto _TL1EB;_T9B=e1;_T9C=_T9B->loc;_T9D=e1_type;_T9E=fld;_T9F=inflow;
Cyc_NewControlFlow_check_union_requires(_T9C,_T9D,_T9E,_T9F);goto _TL1EC;_TL1EB: _TL1EC: _TA0=
# 1778
Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type);if(!_TA0)goto _TL1ED;{struct _tuple16 _TB0;
_TB0.f0=inflow;_TB0.f1=Cyc_CfFlowInfo_UnknownL();_TA1=_TB0;}return _TA1;_TL1ED: _TA2=env;_TA3=inflow;_TA4=expand_unique;_TA5=passthrough_consumes;_TA6=e1;{struct Cyc_List_List*_TB0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_TB1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct));_TB1->tag=0;
# 1782
_TB1->f1=Cyc_CfFlowInfo_get_field_index(e1_type,fld);_TA8=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_TB1;}_TB0->hd=(void*)_TA8;
_TB0->tl=path;_TA7=(struct Cyc_List_List*)_TB0;}_TA9=
# 1781
Cyc_NewControlFlow_anal_Lexp_rec(_TA2,_TA3,_TA4,_TA5,_TA6,_TA7);return _TA9;}}default: _LL1C:{struct _tuple16 _TB0;
# 1785
_TB0.f0=Cyc_CfFlowInfo_BottomFL();_TB0.f1=Cyc_CfFlowInfo_UnknownL();_TAA=_TB0;}return _TAA;};}}}
# 1789
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){struct _tuple16 _T0;
# 1793
struct _tuple16 _T1=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_AbsLVal _T2;union Cyc_CfFlowInfo_FlowInfo _T3;_T3=_T1.f0;_T2=_T1.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T3;union Cyc_CfFlowInfo_AbsLVal r=_T2;{struct _tuple16 _T4;
_T4.f0=f;_T4.f1=r;_T0=_T4;}return _T0;}}
# 1800
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;int _T3;struct Cyc_NewControlFlow_AnalEnv*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;void*_T9;void*_TA;int _TB;struct Cyc_NewControlFlow_AnalEnv*_TC;union Cyc_CfFlowInfo_FlowInfo _TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_List_List*_T11;union Cyc_CfFlowInfo_FlowInfo _T12;{
# 1803
struct Cyc_List_List*x=es;_TL1F2: if(x!=0)goto _TL1F0;else{goto _TL1F1;}
# 1807
_TL1F0: _T0=x;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Exp*)_T1;_T3=Cyc_NewControlFlow_is_local_var_rooted_path(_T2,1);if(!_T3)goto _TL1F3;_T4=env;_T5=_T4->aquals_bounds;_T6=x;_T7=_T6->hd;_T8=(struct Cyc_Absyn_Exp*)_T7;_T9=_T8->topt;_TA=
_check_null(_T9);_TB=Cyc_Tcutil_is_noalias_pointer_or_aggr(_T5,_TA);
# 1807
if(!_TB)goto _TL1F3;_TC=env;_TD=inflow;_TE=x;_TF=_TE->hd;_T10=(struct Cyc_Absyn_Exp*)_TF;{
# 1810
struct _tuple16 _T13=Cyc_NewControlFlow_anal_Lexp(_TC,_TD,1,0,_T10);union Cyc_CfFlowInfo_FlowInfo _T14;_T14=_T13.f0;{union Cyc_CfFlowInfo_FlowInfo f=_T14;
inflow=f;}}goto _TL1F4;_TL1F3: _TL1F4: _T11=x;
# 1803
x=_T11->tl;goto _TL1F2;_TL1F1:;}_T12=inflow;
# 1815
return _T12;}
# 1821
static struct _tuple17 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){struct Cyc_NewControlFlow_AnalEnv*_T0;struct Cyc_NewControlFlow_AnalEnv*_T1;union Cyc_CfFlowInfo_FlowInfo _T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Exp*_T5;struct Cyc_NewControlFlow_AnalEnv*_T6;union Cyc_CfFlowInfo_FlowInfo _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Absyn_Exp*_TC;union Cyc_CfFlowInfo_FlowInfo _TD;struct _union_FlowInfo_BottomFL _TE;unsigned _TF;struct _tuple17 _T10;union Cyc_CfFlowInfo_FlowInfo _T11;struct _union_FlowInfo_ReachableFL _T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_NewControlFlow_AnalEnv*_T18;struct Cyc_CfFlowInfo_FlowEnv*_T19;struct Cyc_Dict_Dict _T1A;void*_T1B;enum Cyc_CfFlowInfo_InitLevel _T1C;int _T1D;int _T1E;struct Cyc_List_List*_T1F;void*_T20;struct Cyc_Absyn_Exp*_T21;unsigned _T22;struct _fat_ptr _T23;struct _fat_ptr _T24;struct Cyc_NewControlFlow_AnalEnv*_T25;struct Cyc_CfFlowInfo_FlowEnv*_T26;struct Cyc_Dict_Dict _T27;void*_T28;enum Cyc_CfFlowInfo_InitLevel _T29;int _T2A;int _T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;void*_T2E;struct Cyc_Absyn_Exp*_T2F;unsigned _T30;struct _fat_ptr _T31;struct _fat_ptr _T32;enum Cyc_Absyn_Primop _T33;int _T34;enum Cyc_Absyn_Primop _T35;int _T36;struct _tuple0 _T37;void*_T38;int*_T39;unsigned _T3A;void*_T3B;int*_T3C;int _T3D;void*_T3E;enum Cyc_Absyn_Primop _T3F;int _T40;int(*_T41)(struct _fat_ptr,struct _fat_ptr);void*(*_T42)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T43;struct _fat_ptr _T44;void*_T45;int*_T46;unsigned _T47;void*_T48;enum Cyc_Absyn_Primop _T49;int _T4A;int(*_T4B)(struct _fat_ptr,struct _fat_ptr);void*(*_T4C)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T4D;struct _fat_ptr _T4E;enum Cyc_Absyn_Primop _T4F;int _T50;void*_T51;int*_T52;int _T53;void*_T54;int*_T55;int _T56;enum Cyc_Absyn_Primop _T57;int _T58;struct _tuple17 _T59;_T0=env;{
# 1823
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T0->fenv;_T1=env;_T2=inflow;_T3=
# 1826
_check_null(es);_T4=_T3->hd;_T5=(struct Cyc_Absyn_Exp*)_T4;{struct _tuple14 _T5A=Cyc_NewControlFlow_anal_Rexp(_T1,0,_T2,_T5,0);void*_T5B;union Cyc_CfFlowInfo_FlowInfo _T5C;_T5C=_T5A.f0;_T5B=_T5A.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T5C;void*r1=_T5B;_T6=env;_T7=f1;_T8=es;_T9=_T8->tl;_TA=
_check_null(_T9);_TB=_TA->hd;_TC=(struct Cyc_Absyn_Exp*)_TB;{struct _tuple14 _T5D=Cyc_NewControlFlow_anal_Rexp(_T6,0,_T7,_TC,0);void*_T5E;union Cyc_CfFlowInfo_FlowInfo _T5F;_T5F=_T5D.f0;_T5E=_T5D.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_T5F;void*r2=_T5E;
union Cyc_CfFlowInfo_FlowInfo f=f2;struct Cyc_Dict_Dict _T60;_TD=f;_TE=_TD.BottomFL;_TF=_TE.tag;if(_TF!=1)goto _TL1F5;{struct _tuple17 _T61;
# 1832
_T61.f0=f;_T61.f1=f;_T10=_T61;}return _T10;_TL1F5: _T11=f;_T12=_T11.ReachableFL;_T60=_T12.val;{struct Cyc_Dict_Dict d=_T60;_T13=es;_T14=_T13->hd;{
# 1834
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_T14;_T15=es;_T16=_T15->tl;_T17=_T16->hd;{
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)_T17;_T18=env;_T19=_T18->fenv;_T1A=d;_T1B=r1;_T1C=
Cyc_CfFlowInfo_initlevel(_T19,_T1A,_T1B);_T1D=(int)_T1C;if(_T1D!=0)goto _TL1F7;_T1E=Cyc_CfFlowInfo_is_init_pointer(r1);if(_T1E)goto _TL1F7;else{goto _TL1F9;}
_TL1F9: _T1F=es;_T20=_T1F->hd;_T21=(struct Cyc_Absyn_Exp*)_T20;_T22=_T21->loc;_T23=_tag_fat("expression may not be initialized",sizeof(char),34U);_T24=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T22,_T23,_T24);goto _TL1F8;_TL1F7: _TL1F8: _T25=env;_T26=_T25->fenv;_T27=d;_T28=r2;_T29=
Cyc_CfFlowInfo_initlevel(_T26,_T27,_T28);_T2A=(int)_T29;if(_T2A!=0)goto _TL1FA;_T2B=Cyc_CfFlowInfo_is_init_pointer(r1);if(_T2B)goto _TL1FA;else{goto _TL1FC;}
_TL1FC: _T2C=es;_T2D=_T2C->tl;_T2E=_T2D->hd;_T2F=(struct Cyc_Absyn_Exp*)_T2E;_T30=_T2F->loc;_T31=_tag_fat("expression may not be initialized",sizeof(char),34U);_T32=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T30,_T31,_T32);goto _TL1FB;_TL1FA: _TL1FB: {
# 1841
union Cyc_CfFlowInfo_FlowInfo ft=f;
union Cyc_CfFlowInfo_FlowInfo ff=f;_T33=p;_T34=(int)_T33;
# 1846
if(_T34==5)goto _TL1FF;else{goto _TL200;}_TL200: _T35=p;_T36=(int)_T35;if(_T36==6)goto _TL1FF;else{goto _TL1FD;}
_TL1FF:{struct _tuple13 _T61=Cyc_CfFlowInfo_unname_rval(r1);struct Cyc_List_List*_T62;void*_T63;_T63=_T61.f0;_T62=_T61.f1;{void*r1=_T63;struct Cyc_List_List*r1n=_T62;
struct _tuple13 _T64=Cyc_CfFlowInfo_unname_rval(r2);struct Cyc_List_List*_T65;void*_T66;_T66=_T64.f0;_T65=_T64.f1;{void*r2=_T66;struct Cyc_List_List*r2n=_T65;{struct _tuple0 _T67;
_T67.f0=r1;_T67.f1=r2;_T37=_T67;}{struct _tuple0 _T67=_T37;enum Cyc_CfFlowInfo_InitLevel _T68;_T38=_T67.f0;_T39=(int*)_T38;_T3A=*_T39;switch(_T3A){case 2: _T3B=_T67.f1;_T3C=(int*)_T3B;_T3D=*_T3C;if(_T3D!=0)goto _TL202;_T3E=_T67.f0;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T69=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T3E;_T68=_T69->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_T68;
# 1851
struct _tuple17 _T69=Cyc_NewControlFlow_splitzero(env,inflow,f,e1,il,r1n);union Cyc_CfFlowInfo_FlowInfo _T6A;union Cyc_CfFlowInfo_FlowInfo _T6B;_T6B=_T69.f0;_T6A=_T69.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T6B;union Cyc_CfFlowInfo_FlowInfo f2=_T6A;_T3F=p;_T40=(int)_T3F;switch(_T40){case Cyc_Absyn_Eq:
# 1853
 ft=f2;ff=f1;goto _LL25;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL25;default: _T42=Cyc_Warn_impos;{
int(*_T6C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T42;_T41=_T6C;}_T43=_tag_fat("anal_test, zero-split",sizeof(char),22U);_T44=_tag_fat(0U,sizeof(void*),0);_T41(_T43,_T44);}_LL25: goto _LL11;}}_TL202: goto _LL20;case 0: _T45=_T67.f1;_T46=(int*)_T45;_T47=*_T46;switch(_T47){case 2: _T48=_T67.f1;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T69=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T48;_T68=_T69->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_T68;
# 1859
struct _tuple17 _T69=Cyc_NewControlFlow_splitzero(env,f2,f,e2,il,r2n);union Cyc_CfFlowInfo_FlowInfo _T6A;union Cyc_CfFlowInfo_FlowInfo _T6B;_T6B=_T69.f0;_T6A=_T69.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T6B;union Cyc_CfFlowInfo_FlowInfo f2=_T6A;_T49=p;_T4A=(int)_T49;switch(_T4A){case Cyc_Absyn_Eq:
# 1861
 ft=f2;ff=f1;goto _LL2F;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL2F;default: _T4C=Cyc_Warn_impos;{
int(*_T6C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T4C;_T4B=_T6C;}_T4D=_tag_fat("anal_test, zero-split",sizeof(char),22U);_T4E=_tag_fat(0U,sizeof(void*),0);_T4B(_T4D,_T4E);}_LL2F: goto _LL11;}}case 0: _T4F=p;_T50=(int)_T4F;
# 1867
if(_T50!=5)goto _TL207;ff=Cyc_CfFlowInfo_BottomFL();goto _TL208;
_TL207: ft=Cyc_CfFlowInfo_BottomFL();_TL208: goto _LL11;case 1: goto _LL1B;case 4: _LL1B: goto _LL1D;default: goto _LL20;};case 1: _T51=_T67.f1;_T52=(int*)_T51;_T53=*_T52;if(_T53!=0)goto _TL209;_LL1D: goto _LL1F;_TL209: goto _LL20;case 4: _T54=_T67.f1;_T55=(int*)_T54;_T56=*_T55;if(_T56!=0)goto _TL20B;_LL1F: _T57=p;_T58=(int)_T57;
# 1874
if(_T58!=6)goto _TL20D;ff=Cyc_CfFlowInfo_BottomFL();goto _TL20E;
_TL20D: ft=Cyc_CfFlowInfo_BottomFL();_TL20E: goto _LL11;_TL20B: goto _LL20;default: _LL20: goto _LL11;}_LL11:;}}}}goto _TL1FE;_TL1FD: _TL1FE:{struct _tuple17 _T61;
# 1881
_T61.f0=ft;_T61.f1=ff;_T59=_T61;}return _T59;}}}};}}}}}}
# 1887
static struct _tuple17 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_NewControlFlow_AnalEnv*_T0;struct Cyc_Absyn_Exp*_T1;int*_T2;unsigned _T3;struct _tuple17 _T4;struct _tuple17 _T5;struct _tuple17 _T6;struct _tuple17 _T7;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T8;enum Cyc_Absyn_Primop _T9;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TA;struct Cyc_List_List*_TB;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;void*_T11;struct _tuple17 _T12;struct _tuple17 _T13;union Cyc_CfFlowInfo_FlowInfo _T14;struct _union_FlowInfo_BottomFL _T15;unsigned _T16;struct _tuple17 _T17;union Cyc_CfFlowInfo_FlowInfo _T18;struct _union_FlowInfo_ReachableFL _T19;void*_T1A;int*_T1B;unsigned _T1C;void*_T1D;void*_T1E;int(*_T1F)(struct _fat_ptr,struct _fat_ptr);void*(*_T20)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T21;struct _fat_ptr _T22;struct _tuple17 _T23;struct _tuple17 _T24;void*_T25;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T26;enum Cyc_CfFlowInfo_InitLevel _T27;void*_T28;struct _tuple17 _T29;void*_T2A;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_T2B;enum Cyc_CfFlowInfo_InitLevel _T2C;struct _tuple17 _T2D;struct Cyc_Absyn_Exp*_T2E;unsigned _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct _tuple17 _T32;int(*_T33)(struct _fat_ptr,struct _fat_ptr);void*(*_T34)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T35;struct _fat_ptr _T36;_T0=env;{
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T0->fenv;_T1=e;{
void*_T37=_T1->r;struct Cyc_List_List*_T38;enum Cyc_Absyn_Primop _T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_Absyn_Exp*_T3B;struct Cyc_Absyn_Exp*_T3C;_T2=(int*)_T37;_T3=*_T2;switch(_T3){case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T3D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T37;_T3C=_T3D->f1;_T3B=_T3D->f2;_T3A=_T3D->f3;}{struct Cyc_Absyn_Exp*e1=_T3C;struct Cyc_Absyn_Exp*e2=_T3B;struct Cyc_Absyn_Exp*e3=_T3A;
# 1891
struct _tuple17 _T3D=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;_T3F=_T3D.f0;_T3E=_T3D.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T3F;union Cyc_CfFlowInfo_FlowInfo f1f=_T3E;
struct _tuple17 _T40=Cyc_NewControlFlow_anal_test(env,f1t,e2);union Cyc_CfFlowInfo_FlowInfo _T41;union Cyc_CfFlowInfo_FlowInfo _T42;_T42=_T40.f0;_T41=_T40.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_T42;union Cyc_CfFlowInfo_FlowInfo f2f=_T41;
struct _tuple17 _T43=Cyc_NewControlFlow_anal_test(env,f1f,e3);union Cyc_CfFlowInfo_FlowInfo _T44;union Cyc_CfFlowInfo_FlowInfo _T45;_T45=_T43.f0;_T44=_T43.f1;{union Cyc_CfFlowInfo_FlowInfo f3t=_T45;union Cyc_CfFlowInfo_FlowInfo f3f=_T44;{struct _tuple17 _T46;
_T46.f0=Cyc_CfFlowInfo_join_flow(fenv,f2t,f3t);
_T46.f1=Cyc_CfFlowInfo_join_flow(fenv,f2f,f3f);_T4=_T46;}
# 1894
return _T4;}}}}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T3D=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T37;_T3C=_T3D->f1;_T3B=_T3D->f2;}{struct Cyc_Absyn_Exp*e1=_T3C;struct Cyc_Absyn_Exp*e2=_T3B;
# 1897
struct _tuple17 _T3D=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;_T3F=_T3D.f0;_T3E=_T3D.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T3F;union Cyc_CfFlowInfo_FlowInfo f1f=_T3E;
struct _tuple17 _T40=Cyc_NewControlFlow_anal_test(env,f1t,e2);union Cyc_CfFlowInfo_FlowInfo _T41;union Cyc_CfFlowInfo_FlowInfo _T42;_T42=_T40.f0;_T41=_T40.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_T42;union Cyc_CfFlowInfo_FlowInfo f2f=_T41;{struct _tuple17 _T43;
_T43.f0=f2t;_T43.f1=Cyc_CfFlowInfo_join_flow(fenv,f1f,f2f);_T5=_T43;}return _T5;}}}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T3D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T37;_T3C=_T3D->f1;_T3B=_T3D->f2;}{struct Cyc_Absyn_Exp*e1=_T3C;struct Cyc_Absyn_Exp*e2=_T3B;
# 1901
struct _tuple17 _T3D=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;_T3F=_T3D.f0;_T3E=_T3D.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T3F;union Cyc_CfFlowInfo_FlowInfo f1f=_T3E;
struct _tuple17 _T40=Cyc_NewControlFlow_anal_test(env,f1f,e2);union Cyc_CfFlowInfo_FlowInfo _T41;union Cyc_CfFlowInfo_FlowInfo _T42;_T42=_T40.f0;_T41=_T40.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_T42;union Cyc_CfFlowInfo_FlowInfo f2f=_T41;{struct _tuple17 _T43;
_T43.f0=Cyc_CfFlowInfo_join_flow(fenv,f1t,f2t);_T43.f1=f2f;_T6=_T43;}return _T6;}}}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T3D=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T37;_T3C=_T3D->f1;_T3B=_T3D->f2;}{struct Cyc_Absyn_Exp*e1=_T3C;struct Cyc_Absyn_Exp*e2=_T3B;
# 1905
struct _tuple14 _T3D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);void*_T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;_T3F=_T3D.f0;_T3E=_T3D.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T3F;void*r=_T3E;_T7=
Cyc_NewControlFlow_anal_test(env,f,e2);return _T7;}}case 3: _T8=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T37;_T9=_T8->f1;if(_T9!=Cyc_Absyn_Not)goto _TL210;_TA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T37;_TB=_TA->f2;if(_TB==0)goto _TL212;_TC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T37;_TD=_TC->f2;_TE=(struct Cyc_List_List*)_TD;_TF=_TE->tl;if(_TF!=0)goto _TL214;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T3D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T37;_T10=_T3D->f2;{struct Cyc_List_List _T3E=*_T10;_T11=_T3E.hd;_T3C=(struct Cyc_Absyn_Exp*)_T11;}}{struct Cyc_Absyn_Exp*e1=_T3C;
# 1908
struct _tuple17 _T3D=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;_T3F=_T3D.f0;_T3E=_T3D.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T3F;union Cyc_CfFlowInfo_FlowInfo f2=_T3E;{struct _tuple17 _T40;
_T40.f0=f2;_T40.f1=f1;_T12=_T40;}return _T12;}}_TL214: goto _LLB;_TL212: goto _LLB;_TL210: _LLB:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T3D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T37;_T39=_T3D->f1;_T38=_T3D->f2;}{enum Cyc_Absyn_Primop p=_T39;struct Cyc_List_List*es=_T38;_T13=
# 1911
Cyc_NewControlFlow_anal_primop_test(env,inflow,p,es);return _T13;}default:  {
# 1915
struct _tuple14 _T3D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0);void*_T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;_T3F=_T3D.f0;_T3E=_T3D.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T3F;void*r=_T3E;struct Cyc_Dict_Dict _T40;_T14=f;_T15=_T14.BottomFL;_T16=_T15.tag;if(_T16!=1)goto _TL216;{struct _tuple17 _T41;
# 1917
_T41.f0=f;_T41.f1=f;_T17=_T41;}return _T17;_TL216: _T18=f;_T19=_T18.ReachableFL;_T40=_T19.val;{struct Cyc_Dict_Dict d=_T40;
# 1919
struct _tuple13 _T41=Cyc_CfFlowInfo_unname_rval(r);struct Cyc_List_List*_T42;void*_T43;_T43=_T41.f0;_T42=_T41.f1;{void*r=_T43;struct Cyc_List_List*names=_T42;enum Cyc_CfFlowInfo_InitLevel _T44;void*_T45;struct Cyc_Absyn_Vardecl*_T46;_T1A=r;_T1B=(int*)_T1A;_T1C=*_T1B;switch(_T1C){case 8: _T1D=r;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T47=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T1D;_T46=_T47->f1;_T1E=_T47->f2;_T45=(void*)_T1E;}{struct Cyc_Absyn_Vardecl*n=_T46;void*r2=_T45;_T20=Cyc_Warn_impos;{
# 1922
int(*_T47)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T20;_T1F=_T47;}_T21=_tag_fat("anal_test: bad namedlocation",sizeof(char),29U);_T22=_tag_fat(0U,sizeof(void*),0);_T1F(_T21,_T22);}case 0:{struct _tuple17 _T47;
_T47.f0=Cyc_CfFlowInfo_BottomFL();_T47.f1=f;_T23=_T47;}return _T23;case 1: goto _LL3D;case 5: _LL3D: goto _LL3F;case 4: _LL3F:{struct _tuple17 _T47;
# 1926
_T47.f0=f;_T47.f1=Cyc_CfFlowInfo_BottomFL();_T24=_T47;}return _T24;case 2: _T25=r;_T26=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T25;_T27=_T26->f1;if(_T27!=Cyc_CfFlowInfo_NoneIL)goto _TL219;goto _LL43;_TL219: _T28=r;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T47=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T28;_T44=_T47->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_T44;_T29=
# 1932
Cyc_NewControlFlow_splitzero(env,inflow,f,e,il,names);return _T29;}case 3: _T2A=r;_T2B=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_T2A;_T2C=_T2B->f1;if(_T2C!=Cyc_CfFlowInfo_NoneIL)goto _TL21B;_LL43: goto _LL45;_TL21B:{struct _tuple17 _T47;
_T47.f0=f;_T47.f1=f;_T2D=_T47;}return _T2D;case 7: _LL45: _T2E=e;_T2F=_T2E->loc;_T30=
# 1930
_tag_fat("expression may not be initialized",sizeof(char),34U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T2F,_T30,_T31);{struct _tuple17 _T47;
_T47.f0=Cyc_CfFlowInfo_BottomFL();_T47.f1=Cyc_CfFlowInfo_BottomFL();_T32=_T47;}return _T32;default: _T34=Cyc_Warn_impos;{
# 1934
int(*_T47)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T34;_T33=_T47;}_T35=_tag_fat("anal_test",sizeof(char),10U);_T36=_tag_fat(0U,sizeof(void*),0);_T33(_T35,_T36);};}};}}};}}}struct _tuple25{unsigned f0;struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;};
# 1940
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple25*ckenv,void*root,void*rval){struct _tuple25*_T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_NewControlFlow_AnalEnv*_T5;struct Cyc_List_List*_T6;struct Cyc_Absyn_Vardecl*_T7;void*_T8;int _T9;void*_TA;int*_TB;unsigned _TC;void*_TD;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_TE;enum Cyc_CfFlowInfo_InitLevel _TF;struct Cyc_String_pa_PrintArg_struct _T10;struct Cyc_Absyn_Vardecl*_T11;struct _tuple2*_T12;void**_T13;unsigned _T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc_Dict_Dict _T17;struct Cyc_NewControlFlow_AnalEnv*_T18;unsigned _T19;_T0=ckenv;{struct _tuple25 _T1A=*_T0;_T19=_T1A.f0;_T18=_T1A.f1;_T17=_T1A.f2;}{unsigned loc=_T19;struct Cyc_NewControlFlow_AnalEnv*env=_T18;struct Cyc_Dict_Dict fd=_T17;struct Cyc_Absyn_Vardecl*_T1A;_T1=root;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL21D;_T4=root;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T1B=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_T4;_T1A=_T1B->f1;}{struct Cyc_Absyn_Vardecl*vd=_T1A;_T5=env;_T6=_T5->aquals_bounds;_T7=vd;_T8=_T7->type;_T9=
# 1945
Cyc_Tcutil_is_noalias_pointer_or_aggr(_T6,_T8);if(!_T9)goto _TL21F;{
struct _tuple13 _T1B=Cyc_CfFlowInfo_unname_rval(rval);void*_T1C;_T1C=_T1B.f0;{void*rval=_T1C;_TA=rval;_TB=(int*)_TA;_TC=*_TB;switch(_TC){case 7: goto _LLF;case 0: _LLF: goto _LL11;case 2: _TD=rval;_TE=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TD;_TF=_TE->f1;if(_TF!=Cyc_CfFlowInfo_NoneIL)goto _TL222;_LL11: goto _LLB;_TL222: goto _LL12;default: _LL12:{struct Cyc_String_pa_PrintArg_struct _T1D;_T1D.tag=0;_T11=vd;_T12=_T11->name;
# 1952
_T1D.f1=Cyc_Absynpp_qvar2string(_T12);_T10=_T1D;}{struct Cyc_String_pa_PrintArg_struct _T1D=_T10;void*_T1E[1];_T13=_T1E + 0;*_T13=& _T1D;_T14=loc;_T15=_tag_fat("unique pointers reachable from %s may become unreachable",sizeof(char),57U);_T16=_tag_fat(_T1E,sizeof(void*),1);Cyc_Warn_warn(_T14,_T15,_T16);}goto _LLB;}_LLB:;}}goto _TL220;_TL21F: _TL220: goto _LL3;}_TL21D: goto _LL3;_LL3:;}}
# 1961
static void Cyc_NewControlFlow_check_init_params(unsigned loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;struct _union_FlowInfo_ReachableFL _T4;struct Cyc_NewControlFlow_AnalEnv*_T5;struct Cyc_NewControlFlow_AnalEnv*_T6;struct Cyc_CfFlowInfo_FlowEnv*_T7;struct Cyc_Dict_Dict _T8;struct Cyc_Dict_Dict _T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_CfFlowInfo_Place*_TC;void*_TD;enum Cyc_CfFlowInfo_InitLevel _TE;int _TF;unsigned _T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct Cyc_List_List*_T13;int _T14;struct _tuple25 _T15;void(*_T16)(void(*)(struct _tuple25*,void*,void*),struct _tuple25*,struct Cyc_Dict_Dict);void(*_T17)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple25*_T18;struct _tuple25*_T19;struct Cyc_Dict_Dict _T1A;struct Cyc_Dict_Dict _T1B;_T0=flow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL224;
# 1963
return;_TL224: _T3=flow;_T4=_T3.ReachableFL;_T1B=_T4.val;{struct Cyc_Dict_Dict d=_T1B;_T5=env;{
# 1965
struct Cyc_List_List*inits=_T5->param_roots;_TL229: if(inits!=0)goto _TL227;else{goto _TL228;}
_TL227: _T6=env;_T7=_T6->fenv;_T8=d;_T9=d;_TA=inits;_TB=_TA->hd;_TC=(struct Cyc_CfFlowInfo_Place*)_TB;_TD=Cyc_CfFlowInfo_lookup_place(_T9,_TC);_TE=Cyc_CfFlowInfo_initlevel(_T7,_T8,_TD);_TF=(int)_TE;if(_TF==1)goto _TL22A;_T10=loc;_T11=
# 1968
_tag_fat("function may not initialize all the parameters with attribute 'initializes'",sizeof(char),76U);_T12=_tag_fat(0U,sizeof(void*),0);
# 1967
Cyc_CfFlowInfo_aerr(_T10,_T11,_T12);goto _TL22B;_TL22A: _TL22B: _T13=inits;
# 1965
inits=_T13->tl;goto _TL229;_TL228:;}_T14=Cyc_Flags_warn_lose_unique;
# 1969
if(!_T14)goto _TL22C;{struct _tuple25 _T1C;
_T1C.f0=loc;_T1C.f1=env;_T1C.f2=d;_T15=_T1C;}{struct _tuple25 check_env=_T15;_T17=Cyc_Dict_iter_c;{
void(*_T1C)(void(*)(struct _tuple25*,void*,void*),struct _tuple25*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple25*,void*,void*),struct _tuple25*,struct Cyc_Dict_Dict))_T17;_T16=_T1C;}_T18=& check_env;_T19=(struct _tuple25*)_T18;_T1A=d;_T16(Cyc_NewControlFlow_check_for_unused_unique,_T19,_T1A);}goto _TL22D;_TL22C: _TL22D:
# 1973
 return;};}
# 1982
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*vds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_NewControlFlow_AnalEnv*_T2;struct Cyc_List_List*_T3;struct Cyc_Absyn_Exp*_T4;void*_T5;void*_T6;int _T7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T8;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T9;struct Cyc_Absyn_Vardecl**_TA;struct Cyc_CfFlowInfo_Place*_TB;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_TC;struct Cyc_CfFlowInfo_Place*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct _tuple1 _T11;
# 1984
struct Cyc_List_List*roots=0;
struct Cyc_List_List*es=0;{
struct Cyc_List_List*x=vds;_TL231: if(x!=0)goto _TL22F;else{goto _TL230;}
_TL22F: _T0=x;_T1=_T0->hd;{struct _tuple22*_T12=(struct _tuple22*)_T1;struct Cyc_Absyn_Exp*_T13;struct Cyc_Absyn_Vardecl**_T14;{struct _tuple22 _T15=*_T12;_T14=_T15.f0;_T13=_T15.f1;}{struct Cyc_Absyn_Vardecl**vopt=_T14;struct Cyc_Absyn_Exp*eopt=_T13;
if(vopt==0)goto _TL232;if(eopt==0)goto _TL232;_T2=env;_T3=_T2->aquals_bounds;_T4=eopt;_T5=_T4->topt;_T6=
_check_null(_T5);_T7=Cyc_Tcutil_is_noalias_pointer(_T3,_T6,0);
# 1988
if(!_T7)goto _TL232;{
# 1990
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_T8=root;_T8->tag=0;_T9=root;_TA=vopt;_T9->f1=*_TA;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_TB=rp;_TC=root;_TB->root=(void*)_TC;_TD=rp;_TD->path=0;{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));
_T15->hd=rp;_T15->tl=roots;_TE=(struct Cyc_List_List*)_T15;}roots=_TE;{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));
_T15->hd=eopt;_T15->tl=es;_TF=(struct Cyc_List_List*)_T15;}es=_TF;}}goto _TL233;_TL232: _TL233:;}}_T10=x;
# 1986
x=_T10->tl;goto _TL231;_TL230:;}{struct _tuple1 _T12;
# 1996
_T12.f0=roots;_T12.f1=es;_T11=_T12;}return _T11;}struct _tuple26{int f0;void*f1;};
# 2002
static struct _tuple26 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;int(*_T3)(struct _fat_ptr,struct _fat_ptr);void*(*_T4)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T5;struct _fat_ptr _T6;union Cyc_CfFlowInfo_FlowInfo _T7;struct _union_FlowInfo_ReachableFL _T8;void*_T9;int*_TA;int _TB;void*_TC;void*_TD;struct Cyc_NewControlFlow_AnalEnv*_TE;struct Cyc_List_List*_TF;struct Cyc_Absyn_Vardecl*_T10;void*_T11;int _T12;struct Cyc_Absyn_Exp*_T13;int _T14;void*_T15;int*_T16;int _T17;int _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct Cyc_Absyn_Vardecl*_T1A;struct _tuple2*_T1B;void**_T1C;unsigned _T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;struct Cyc_NewControlFlow_AnalEnv*_T20;struct Cyc_CfFlowInfo_FlowEnv*_T21;struct Cyc_Dict_Dict _T22;void*_T23;enum Cyc_CfFlowInfo_InitLevel _T24;int _T25;int _T26;struct Cyc_String_pa_PrintArg_struct _T27;struct Cyc_Absyn_Vardecl*_T28;struct _tuple2*_T29;void**_T2A;unsigned _T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;struct Cyc_NewControlFlow_AnalEnv*_T2E;struct Cyc_List_List*_T2F;struct Cyc_Absyn_Vardecl*_T30;void*_T31;int _T32;int _T33;struct Cyc_String_pa_PrintArg_struct _T34;struct Cyc_Absyn_Vardecl*_T35;struct _tuple2*_T36;void**_T37;unsigned _T38;struct _fat_ptr _T39;struct _fat_ptr _T3A;struct _tuple26 _T3B;struct Cyc_Dict_Dict _T3C;_T0=flow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL234;_T4=Cyc_Warn_impos;{
# 2010
int(*_T3D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T4;_T3=_T3D;}_T5=_tag_fat("noconsume_place_ok: flow became Bottom!",sizeof(char),40U);_T6=_tag_fat(0U,sizeof(void*),0);_T3(_T5,_T6);goto _TL235;_TL234: _T7=flow;_T8=_T7.ReachableFL;_T3C=_T8.val;{struct Cyc_Dict_Dict d=_T3C;
# 2017
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_uint_exp(1U,0U);
int bogus_bool=0;
int bogus_int=1;
void*curr_rval=Cyc_CfFlowInfo_lookup_place(d,place);
void*rval=curr_rval;
# 2029
int varok=0;{void*_T3D;struct Cyc_Absyn_Vardecl*_T3E;_T9=curr_rval;_TA=(int*)_T9;_TB=*_TA;if(_TB!=8)goto _TL236;_TC=curr_rval;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T3F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TC;_T3E=_T3F->f1;_TD=_T3F->f2;_T3D=(void*)_TD;}{struct Cyc_Absyn_Vardecl*n=_T3E;void*r=_T3D;
# 2032
if(vd!=n)goto _TL238;
rval=r;_TE=env;_TF=_TE->aquals_bounds;_T10=vd;_T11=_T10->type;_T12=
# 2035
Cyc_Tcutil_is_noalias_pointer_or_aggr(_TF,_T11);if(!_T12)goto _TL23A;_T13=bogus_exp;_T14=bogus_int;_T15=rval;_T16=& bogus_bool;_T17=
# 2037
Cyc_CfFlowInfo_is_unique_consumed(_T13,_T14,_T15,_T16);if(!_T17)goto _TL23C;_T18=do_unconsume;
if(_T18)goto _TL23E;else{goto _TL240;}
_TL240:{struct Cyc_String_pa_PrintArg_struct _T3F;_T3F.tag=0;_T1A=vd;_T1B=_T1A->name;
# 2041
_T3F.f1=Cyc_Absynpp_qvar2string(_T1B);_T19=_T3F;}{struct Cyc_String_pa_PrintArg_struct _T3F=_T19;void*_T40[1];_T1C=_T40 + 0;*_T1C=& _T3F;_T1D=loc;_T1E=
# 2040
_tag_fat("function consumes parameter %s which is does not have the 'consume' attribute",sizeof(char),78U);_T1F=_tag_fat(_T40,sizeof(void*),1);Cyc_CfFlowInfo_aerr(_T1D,_T1E,_T1F);}goto _TL23F;_TL23E: _TL23F: goto _TL23D;
# 2044
_TL23C: _T20=env;_T21=_T20->fenv;_T22=d;_T23=rval;_T24=Cyc_CfFlowInfo_initlevel(_T21,_T22,_T23);_T25=(int)_T24;if(_T25==1)goto _TL241;_T26=do_unconsume;if(_T26)goto _TL241;else{goto _TL243;}
_TL243:{struct Cyc_String_pa_PrintArg_struct _T3F;_T3F.tag=0;_T28=vd;_T29=_T28->name;
# 2047
_T3F.f1=Cyc_Absynpp_qvar2string(_T29);_T27=_T3F;}{struct Cyc_String_pa_PrintArg_struct _T3F=_T27;void*_T40[1];_T2A=_T40 + 0;*_T2A=& _T3F;_T2B=loc;_T2C=
# 2046
_tag_fat("function consumes value pointed to by parameter %s, which does not have the 'consume' attribute",sizeof(char),96U);_T2D=_tag_fat(_T40,sizeof(void*),1);Cyc_CfFlowInfo_aerr(_T2B,_T2C,_T2D);}goto _TL242;
# 2049
_TL241: varok=1;_TL242: _TL23D: goto _TL23B;
# 2052
_TL23A: varok=1;_TL23B: goto _TL239;
# 2055
_TL238: goto _LL9;_TL239: goto _LL5;}_TL236: _LL9: _T2E=env;_T2F=_T2E->aquals_bounds;_T30=vd;_T31=_T30->type;_T32=
# 2059
Cyc_Tcutil_is_noalias_pointer_or_aggr(_T2F,_T31);if(_T32)goto _TL244;else{goto _TL246;}
_TL246: varok=1;goto _TL245;
_TL244: _T33=do_unconsume;if(_T33)goto _TL247;else{goto _TL249;}
_TL249:{struct Cyc_String_pa_PrintArg_struct _T3F;_T3F.tag=0;_T35=vd;_T36=_T35->name;
# 2064
_T3F.f1=Cyc_Absynpp_qvar2string(_T36);_T34=_T3F;}{struct Cyc_String_pa_PrintArg_struct _T3F=_T34;void*_T40[1];_T37=_T40 + 0;*_T37=& _T3F;_T38=loc;_T39=
# 2063
_tag_fat("function parameter %s without 'consume' attribute no longer set to its original value",sizeof(char),86U);_T3A=_tag_fat(_T40,sizeof(void*),1);Cyc_CfFlowInfo_aerr(_T38,_T39,_T3A);}goto _TL248;_TL247: _TL248: _TL245: goto _LL5;_LL5:;}{struct _tuple26 _T3D;
# 2071
_T3D.f0=varok;_T3D.f1=rval;_T3B=_T3D;}return _T3B;}_TL235:;}
# 2077
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){struct Cyc_CfFlowInfo_Place*_T0;int(*_T1)(struct _fat_ptr,struct _fat_ptr);void*(*_T2)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T3;struct _fat_ptr _T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_Absyn_Vardecl*_T9;int(*_TA)(struct _fat_ptr,struct _fat_ptr);void*(*_TB)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC;struct _fat_ptr _TD;struct Cyc_List_List*_TE;void*_TF;_T0=p;{struct Cyc_CfFlowInfo_Place _T10=*_T0;_TF=_T10.root;_TE=_T10.path;}{void*root=_TF;struct Cyc_List_List*fs=_TE;
# 2079
if(fs==0)goto _TL24A;_T2=Cyc_Warn_impos;{
int(*_T10)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2;_T1=_T10;}_T3=_tag_fat("unconsume_params: param to unconsume is non-variable",sizeof(char),53U);_T4=_tag_fat(0U,sizeof(void*),0);_T1(_T3,_T4);goto _TL24B;_TL24A: _TL24B: {
struct Cyc_Absyn_Vardecl*vd;struct Cyc_Absyn_Vardecl*_T10;_T5=root;_T6=(int*)_T5;_T7=*_T6;if(_T7!=0)goto _TL24C;_T8=root;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T11=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_T8;_T10=_T11->f1;}{struct Cyc_Absyn_Vardecl*vd=_T10;_T9=vd;
# 2083
return _T9;}_TL24C: _TB=Cyc_Warn_impos;{
int(*_T11)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB;_TA=_T11;}_TC=_tag_fat("unconsume_params: root is not a varroot",sizeof(char),40U);_TD=_tag_fat(0U,sizeof(void*),0);_TA(_TC,_TD);;}}}
# 2096 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_ReachableFL _T5;union Cyc_CfFlowInfo_AbsLVal _T6;struct _union_AbsLVal_PlaceL _T7;unsigned _T8;union Cyc_CfFlowInfo_AbsLVal _T9;struct _union_AbsLVal_PlaceL _TA;void*_TB;int*_TC;int _TD;void*_TE;void*_TF;int _T10;struct Cyc_NewControlFlow_AnalEnv*_T11;struct Cyc_CfFlowInfo_FlowEnv*_T12;void*_T13;struct Cyc_NewControlFlow_AnalEnv*_T14;struct Cyc_CfFlowInfo_FlowEnv*_T15;struct _tuple14 _T16;struct _tuple14 _T17;union Cyc_CfFlowInfo_FlowInfo _T18;struct _union_FlowInfo_ReachableFL _T19;unsigned _T1A;union Cyc_CfFlowInfo_FlowInfo _T1B;struct _union_FlowInfo_ReachableFL _T1C;struct Cyc_NewControlFlow_AnalEnv*_T1D;struct Cyc_CfFlowInfo_FlowEnv*_T1E;unsigned _T1F;struct Cyc_Dict_Dict _T20;struct Cyc_CfFlowInfo_Place*_T21;void*_T22;struct Cyc_Dict_Dict _T23;int(*_T24)(struct _fat_ptr,struct _fat_ptr);void*(*_T25)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T26;struct _fat_ptr _T27;struct Cyc_NewControlFlow_AnalEnv*_T28;struct Cyc_List_List*_T29;struct Cyc_Absyn_Vardecl*_T2A;void*_T2B;int _T2C;struct Cyc_String_pa_PrintArg_struct _T2D;void**_T2E;unsigned _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;union Cyc_CfFlowInfo_FlowInfo _T32;{struct Cyc_Dict_Dict _T33;_T0=flow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL24E;_T3=flow;
# 2105
return _T3;_TL24E: _T4=flow;_T5=_T4.ReachableFL;_T33=_T5.val;{struct Cyc_Dict_Dict d=_T33;
# 2111
struct _tuple16 _T34=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);union Cyc_CfFlowInfo_AbsLVal _T35;union Cyc_CfFlowInfo_FlowInfo _T36;_T36=_T34.f0;_T35=_T34.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T36;union Cyc_CfFlowInfo_AbsLVal lval=_T35;{struct Cyc_CfFlowInfo_Place*_T37;_T6=lval;_T7=_T6.PlaceL;_T8=_T7.tag;if(_T8!=1)goto _TL250;_T9=lval;_TA=_T9.PlaceL;_T37=_TA.val;{struct Cyc_CfFlowInfo_Place*p=_T37;
# 2118
void*old_rval=Cyc_CfFlowInfo_lookup_place(d,p);{void*_T38;struct Cyc_Absyn_Vardecl*_T39;_TB=old_rval;_TC=(int*)_TB;_TD=*_TC;if(_TD!=8)goto _TL252;_TE=old_rval;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T3A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TE;_T39=_T3A->f1;_TF=_T3A->f2;_T38=(void*)_TF;}{struct Cyc_Absyn_Vardecl*old_vd=_T39;void*r=_T38;
# 2124
void*new_rval;
if(old_vd!=vd)goto _TL254;_T10=varok;
# 2128
if(!_T10)goto _TL256;_T11=env;_T12=_T11->fenv;_T13=r;
# 2130
old_rval=Cyc_CfFlowInfo_make_unique_unconsumed(_T12,_T13);
# 2135
if(ropt==0)goto _TL258;_T14=env;_T15=_T14->fenv;{struct _tuple14 _T3A;
# 2143
_T3A.f0=f;_T3A.f1=old_rval;_T16=_T3A;}{struct _tuple14 _T3A;
_T3A.f0=f;_T3A.f1=ropt;_T17=_T3A;}{
# 2141
struct _tuple14 _T3A=
Cyc_CfFlowInfo_join_flow_and_rval(_T15,_T16,_T17);void*_T3B;union Cyc_CfFlowInfo_FlowInfo _T3C;_T3C=_T3A.f0;_T3B=_T3A.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_T3C;void*new_rval2=_T3B;
# 2145
f=f2;new_rval=new_rval2;}}goto _TL259;
# 2150
_TL258: new_rval=old_rval;_TL259: goto _TL257;
# 2153
_TL256: new_rval=r;_TL257:{struct Cyc_Dict_Dict _T3A;_T18=f;_T19=_T18.ReachableFL;_T1A=_T19.tag;if(_T1A!=2)goto _TL25A;_T1B=f;_T1C=_T1B.ReachableFL;_T3A=_T1C.val;{struct Cyc_Dict_Dict d2=_T3A;_T1D=env;_T1E=_T1D->fenv;_T1F=loc;_T20=d2;_T21=p;_T22=new_rval;_T23=
# 2157
Cyc_CfFlowInfo_assign_place(_T1E,_T1F,_T20,_T21,_T22);flow=Cyc_CfFlowInfo_ReachableFL(_T23);goto _LL15;}_TL25A: _T25=Cyc_Warn_impos;{
# 2163
int(*_T3B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T25;_T24=_T3B;}_T26=_tag_fat("unconsume_params: joined flow became bot!",sizeof(char),42U);_T27=_tag_fat(0U,sizeof(void*),0);_T24(_T26,_T27);_LL15:;}goto _LLD;
# 2167
_TL254: goto _LL11;goto _LLD;}_TL252: _LL11:
# 2174
 if(ropt==0)goto _TL25C;_T28=env;_T29=_T28->aquals_bounds;_T2A=vd;_T2B=_T2A->type;_T2C=
Cyc_Tcutil_is_noalias_pointer_or_aggr(_T29,_T2B);
# 2174
if(_T2C)goto _TL25C;else{goto _TL25E;}
# 2176
_TL25E:{struct Cyc_String_pa_PrintArg_struct _T3A;_T3A.tag=0;
_T3A.f1=Cyc_Absynpp_exp2string(unconsume_exp);_T2D=_T3A;}{struct Cyc_String_pa_PrintArg_struct _T3A=_T2D;void*_T3B[1];_T2E=_T3B + 0;*_T2E=& _T3A;_T2F=loc;_T30=
# 2176
_tag_fat("aliased expression %s was overwritten",sizeof(char),38U);_T31=_tag_fat(_T3B,sizeof(void*),1);Cyc_CfFlowInfo_aerr(_T2F,_T30,_T31);}goto _TL25D;_TL25C: _TL25D: goto _LLD;_LLD:;}goto _LL8;}_TL250: goto _LL8;_LL8:;}goto _LL0;}}_LL0:;}_T32=flow;
# 2203 "new_control_flow.cyc"
return _T32;}
# 2216 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_CfFlowInfo_Place*_T6;struct _tuple26 _T7;int _T8;struct _tuple26 _T9;struct Cyc_NewControlFlow_AnalEnv*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_CfFlowInfo_Place*_TD;int _TE;struct Cyc_Absyn_Vardecl*_TF;union Cyc_CfFlowInfo_FlowInfo _T10;unsigned _T11;int _T12;struct Cyc_NewControlFlow_AnalEnv*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Vardecl*_T17;int _T18;void*_T19;union Cyc_CfFlowInfo_FlowInfo _T1A;unsigned _T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;union Cyc_CfFlowInfo_FlowInfo _T1E;_T0=flow;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL25F;_T3=flow;
# 2223
return _T3;_TL25F: goto _LL0;_LL0: {
# 2226
int do_unconsume=unconsume_exps!=0;
struct Cyc_List_List*_T1F=Cyc_List_rev(consumed_vals);struct Cyc_List_List*consumed_vals=_T1F;
struct Cyc_List_List*_T20=Cyc_List_rev(unconsume_exps);struct Cyc_List_List*unconsume_exps=_T20;{
struct Cyc_List_List*params=consumed_vals;_TL264: if(params!=0)goto _TL262;else{goto _TL263;}
# 2235
_TL262: _T4=params;_T5=_T4->hd;_T6=(struct Cyc_CfFlowInfo_Place*)_T5;{struct Cyc_Absyn_Vardecl*vd=Cyc_NewControlFlow_get_vd_from_place(_T6);_T8=is_region_open;
# 2237
if(!_T8)goto _TL265;{struct _tuple26 _T21;_T21.f0=1;_T21.f1=0;_T9=_T21;}_T7=_T9;goto _TL266;_TL265: _TA=env;_TB=params;_TC=_TB->hd;_TD=(struct Cyc_CfFlowInfo_Place*)_TC;_TE=do_unconsume;_TF=vd;_T10=flow;_T11=loc;_T7=
Cyc_NewControlFlow_noconsume_place_ok(_TA,_TD,_TE,_TF,_T10,_T11);_TL266: {
# 2236
struct _tuple26 _T21=_T7;void*_T22;int _T23;_T23=_T21.f0;_T22=_T21.f1;{int varok=_T23;void*rval=_T22;_T12=do_unconsume;
# 2242
if(!_T12)goto _TL267;_T13=env;_T14=unconsume_exps;_T15=_T14->hd;_T16=(struct Cyc_Absyn_Exp*)_T15;_T17=vd;_T18=varok;_T19=rval;_T1A=flow;_T1B=loc;
flow=Cyc_NewControlFlow_unconsume_exp(_T13,_T16,_T17,_T18,_T19,_T1A,_T1B);goto _TL268;_TL267: _TL268:;}}}_T1C=params;
# 2230
params=_T1C->tl;
if(unconsume_exps==0)goto _TL269;_T1D=unconsume_exps;unconsume_exps=_T1D->tl;goto _TL26A;_TL269: _TL26A: goto _TL264;_TL263:;}
# 2245
Cyc_NewControlFlow_update_tryflow(env,flow);_T1E=flow;
return _T1E;}}struct _tuple27{int f0;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2249
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned exp_loc){struct Cyc_NewControlFlow_AnalEnv*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_NewControlFlow_AnalEnv*_T3;struct Cyc_Core_Opt*_T4;void*_T5;struct Cyc_List_List*_T6;struct Cyc_NewControlFlow_AnalEnv*_T7;struct Cyc_CfFlowInfo_FlowEnv*_T8;struct Cyc_NewControlFlow_AnalEnv*_T9;union Cyc_CfFlowInfo_FlowInfo _TA;struct Cyc_Core_Opt*_TB;void*_TC;struct Cyc_List_List*_TD;int _TE;unsigned _TF;unsigned _T10;struct Cyc_NewControlFlow_AnalEnv*_T11;int _T12;struct Cyc_NewControlFlow_AnalEnv*_T13;struct Cyc_List_List*_T14;struct _tuple27*_T15;struct Cyc_NewControlFlow_AnalEnv*_T16;union Cyc_CfFlowInfo_FlowInfo _T17;struct _union_FlowInfo_BottomFL _T18;unsigned _T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;union Cyc_CfFlowInfo_FlowInfo _T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_Absyn_Switch_clause*_T20;struct Cyc_Core_Opt*_T21;void*_T22;struct Cyc_List_List*_T23;struct Cyc_Absyn_Stmt*_T24;unsigned _T25;struct _fat_ptr _T26;struct _fat_ptr _T27;struct Cyc_Absyn_Stmt*_T28;unsigned _T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;struct Cyc_NewControlFlow_AnalEnv*_T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;void*_T2F;struct Cyc_Absyn_Switch_clause*_T30;struct Cyc_Absyn_Stmt*_T31;union Cyc_CfFlowInfo_FlowInfo _T32;struct Cyc_List_List*_T33;union Cyc_CfFlowInfo_FlowInfo _T34;_T0=env;{
# 2251
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T0->fenv;
_TL26E: if(scs!=0)goto _TL26C;else{goto _TL26D;}
_TL26C: _T1=scs;_T2=_T1->hd;{struct Cyc_Absyn_Switch_clause*_T35=(struct Cyc_Absyn_Switch_clause*)_T2;unsigned _T36;struct Cyc_Absyn_Stmt*_T37;struct Cyc_Absyn_Exp*_T38;struct Cyc_Core_Opt*_T39;{struct Cyc_Absyn_Switch_clause _T3A=*_T35;_T39=_T3A.pat_vars;_T38=_T3A.where_clause;_T37=_T3A.body;_T36=_T3A.loc;}{struct Cyc_Core_Opt*vds_opt=_T39;struct Cyc_Absyn_Exp*where_opt=_T38;struct Cyc_Absyn_Stmt*body=_T37;unsigned loc=_T36;_T3=env;_T4=
_check_null(vds_opt);_T5=_T4->v;_T6=(struct Cyc_List_List*)_T5;{struct _tuple1 _T3A=Cyc_NewControlFlow_get_unconsume_pat_vars(_T3,_T6);struct Cyc_List_List*_T3B;struct Cyc_List_List*_T3C;_T3C=_T3A.f0;_T3B=_T3A.f1;{struct Cyc_List_List*roots=_T3C;struct Cyc_List_List*es=_T3B;_T7=env;_T8=_T7->fenv;_T9=env;_TA=inflow;_TB=vds_opt;_TC=_TB->v;_TD=(struct Cyc_List_List*)_TC;_TE=roots!=0;_TF=loc;_T10=exp_loc;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(_T8,_T9,_TA,_TD,_TE,_TF,_T10,0);
# 2259
union Cyc_CfFlowInfo_FlowInfo clause_outflow;_T11=env;{
struct Cyc_List_List*old_unique_pat_vars=_T11->unique_pat_vars;_T12=
# 2262
Cyc_Tcpat_has_vars(vds_opt);if(!_T12)goto _TL26F;_T13=env;{struct Cyc_List_List*_T3D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple27*_T3E=_cycalloc(sizeof(struct _tuple27));
# 2264
_T3E->f0=0;_T3E->f1=body;_T3E->f2=roots;_T3E->f3=es;_T15=(struct _tuple27*)_T3E;}_T3D->hd=_T15;_T3D->tl=old_unique_pat_vars;_T14=(struct Cyc_List_List*)_T3D;}
# 2263
_T13->unique_pat_vars=_T14;goto _TL270;_TL26F: _TL270:
# 2266
 if(where_opt==0)goto _TL271;{
struct Cyc_Absyn_Exp*wexp=where_opt;
struct _tuple17 _T3D=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;_T3F=_T3D.f0;_T3E=_T3D.f1;{union Cyc_CfFlowInfo_FlowInfo ft=_T3F;union Cyc_CfFlowInfo_FlowInfo ff=_T3E;
inflow=ff;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,ft,body,0);}}goto _TL272;
# 2272
_TL271: clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,body,0);_TL272: _T16=env;
# 2274
_T16->unique_pat_vars=old_unique_pat_vars;_T17=clause_outflow;_T18=_T17.BottomFL;_T19=_T18.tag;if(_T19!=1)goto _TL273;goto _LL9;_TL273:
# 2279
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,clause_outflow,loc);_T1A=scs;_T1B=_T1A->tl;
# 2281
if(_T1B!=0)goto _TL275;_T1C=clause_outflow;
return _T1C;
# 2286
_TL275: _T1D=scs;_T1E=_T1D->tl;_T1F=_T1E->hd;_T20=(struct Cyc_Absyn_Switch_clause*)_T1F;_T21=_T20->pat_vars;_T22=_T21->v;_T23=(struct Cyc_List_List*)_T22;if(_T23==0)goto _TL277;_T24=body;_T25=_T24->loc;_T26=
_tag_fat("switch clause may implicitly fallthru",sizeof(char),38U);_T27=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T25,_T26,_T27);goto _TL278;
# 2289
_TL277: _T28=body;_T29=_T28->loc;_T2A=_tag_fat("switch clause may implicitly fallthru",sizeof(char),38U);_T2B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T29,_T2A,_T2B);_TL278: _T2C=env;_T2D=scs;_T2E=_T2D->tl;_T2F=_T2E->hd;_T30=(struct Cyc_Absyn_Switch_clause*)_T2F;_T31=_T30->body;_T32=clause_outflow;
# 2291
Cyc_NewControlFlow_update_flow(_T2C,_T31,_T32);goto _LL9;_LL9:;}}}}}}_T33=scs;
# 2252
scs=_T33->tl;goto _TL26E;_TL26D: _T34=
# 2296
Cyc_CfFlowInfo_BottomFL();return _T34;}}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f0;struct Cyc_Dict_Dict f1;unsigned f2;};
# 2301
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){struct _tuple28*_T0;struct Cyc_NewControlFlow_AnalEnv*_T1;struct Cyc_List_List*_T2;struct Cyc_Absyn_Vardecl*_T3;void*_T4;int _T5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _T6;struct Cyc_Dict_Dict _T7;int(*_T8)(void*,void*);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T9;void*_TA;void*_TB;void*_TC;int*_TD;unsigned _TE;void*_TF;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T10;enum Cyc_CfFlowInfo_InitLevel _T11;struct Cyc_String_pa_PrintArg_struct _T12;struct Cyc_Absyn_Vardecl*_T13;struct _tuple2*_T14;void**_T15;unsigned _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;unsigned _T19;struct Cyc_Dict_Dict _T1A;struct Cyc_NewControlFlow_AnalEnv*_T1B;_T0=vdenv;{struct _tuple28 _T1C=*_T0;_T1B=_T1C.f0;_T1A=_T1C.f1;_T19=_T1C.f2;}{struct Cyc_NewControlFlow_AnalEnv*env=_T1B;struct Cyc_Dict_Dict fd=_T1A;unsigned loc=_T19;_T1=env;_T2=_T1->aquals_bounds;_T3=vd;_T4=_T3->type;_T5=
# 2304
Cyc_Tcutil_is_noalias_pointer_or_aggr(_T2,_T4);if(!_T5)goto _TL279;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _T1C;_T1C.tag=0;
# 2306
_T1C.f1=vd;_T6=_T1C;}{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=_T6;_T7=fd;_T8=Cyc_CfFlowInfo_root_cmp;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T1C=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));
*_T1C=vdroot;_T9=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_T1C;}_TA=(void*)_T9;_TB=Cyc_Dict_lookup_other(_T7,_T8,_TA);{struct _tuple13 _T1C=Cyc_CfFlowInfo_unname_rval(_TB);void*_T1D;_T1D=_T1C.f0;{void*rval=_T1D;_TC=rval;_TD=(int*)_TC;_TE=*_TD;switch(_TE){case 7: goto _LLA;case 0: _LLA: goto _LLC;case 2: _TF=rval;_T10=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TF;_T11=_T10->f1;if(_T11!=Cyc_CfFlowInfo_NoneIL)goto _TL27C;_LLC: goto _LL6;_TL27C: goto _LLD;default: _LLD:{struct Cyc_String_pa_PrintArg_struct _T1E;_T1E.tag=0;_T13=vd;_T14=_T13->name;
# 2313
_T1E.f1=Cyc_Absynpp_qvar2string(_T14);_T12=_T1E;}{struct Cyc_String_pa_PrintArg_struct _T1E=_T12;void*_T1F[1];_T15=_T1F + 0;*_T15=& _T1E;_T16=loc;_T17=_tag_fat("unique pointers may still exist after variable %s goes out of scope",sizeof(char),68U);_T18=_tag_fat(_T1F,sizeof(void*),1);Cyc_Warn_warn(_T16,_T17,_T18);}goto _LL6;}_LL6:;}}}goto _TL27A;_TL279: _TL27A:;}}
# 2320
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_ReachableFL _T1;unsigned _T2;union Cyc_CfFlowInfo_FlowInfo _T3;struct _union_FlowInfo_ReachableFL _T4;struct _tuple28 _T5;struct Cyc_Absyn_Decl*_T6;struct Cyc_NewControlFlow_AnalEnv*_T7;struct Cyc_Absyn_Decl*_T8;int*_T9;unsigned _TA;struct _tuple28*_TB;struct Cyc_Absyn_Vardecl*_TC;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_TD;struct Cyc_Core_Opt*_TE;struct Cyc_Core_Opt*_TF;void*_T10;void(*_T11)(void(*)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*,struct Cyc_List_List*);void(*_T12)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple28*_T13;struct Cyc_List_List*_T14;{struct Cyc_Dict_Dict _T15;_T0=inflow;_T1=_T0.ReachableFL;_T2=_T1.tag;if(_T2!=2)goto _TL27E;_T3=inflow;_T4=_T3.ReachableFL;_T15=_T4.val;{struct Cyc_Dict_Dict fd=_T15;{struct _tuple28 _T16;
# 2323
_T16.f0=env;_T16.f1=fd;_T6=decl;_T16.f2=_T6->loc;_T5=_T16;}{struct _tuple28 vdenv=_T5;_T7=env;{
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T7->fenv;_T8=decl;{
void*_T16=_T8->r;struct Cyc_List_List*_T17;struct Cyc_Absyn_Vardecl*_T18;_T9=(int*)_T16;_TA=*_T9;switch(_TA){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T19=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T16;_T18=_T19->f1;}{struct Cyc_Absyn_Vardecl*vd=_T18;_TB=& vdenv;_TC=vd;
# 2327
Cyc_NewControlFlow_check_dropped_unique_vd(_TB,_TC);goto _LL5;}case 2: _TD=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T16;_TE=_TD->f2;if(_TE==0)goto _TL281;{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T19=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T16;_TF=_T19->f2;{struct Cyc_Core_Opt _T1A=*_TF;_T10=_T1A.v;_T17=(struct Cyc_List_List*)_T10;}}{struct Cyc_List_List*vds=_T17;
# 2330
struct _tuple1 _T19=Cyc_List_split(vds);struct Cyc_List_List*_T1A;_T1A=_T19.f0;{struct Cyc_List_List*vs=_T1A;
struct Cyc_List_List*_T1B=Cyc_Tcutil_filter_nulls(vs);{struct Cyc_List_List*vs=_T1B;_T17=vs;goto _LLB;}}}_TL281: goto _LLC;case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T19=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T16;_T17=_T19->f1;}_LLB: {struct Cyc_List_List*vds=_T17;_T12=Cyc_List_iter_c;{
# 2334
void(*_T19)(void(*)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*,struct Cyc_List_List*)=(void(*)(void(*)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*,struct Cyc_List_List*))_T12;_T11=_T19;}_T13=& vdenv;_T14=vds;_T11(Cyc_NewControlFlow_check_dropped_unique_vd,_T13,_T14);goto _LL5;}default: _LLC: goto _LL5;}_LL5:;}goto _LL0;}}}_TL27E: goto _LL0;_LL0:;}
# 2341
return;}
# 2347
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){int(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_NewControlFlow_AnalEnv*_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Stmt*_T4;struct Cyc_NewControlFlow_AnalEnv*_T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_NewControlFlow_AnalEnv*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;int _TB;union Cyc_CfFlowInfo_FlowInfo _TC;struct Cyc_Absyn_Stmt*_TD;unsigned _TE;struct Cyc_List_List*_TF;union Cyc_CfFlowInfo_FlowInfo _T10;_T1=Cyc_Hashtable_lookup;{
# 2351
int(*_T11)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T11;}_T2=env;_T3=_T2->pat_pop_table;_T4=src;{int num_pop=_T0(_T3,_T4);_T5=env;{
struct Cyc_List_List*x=_T5->unique_pat_vars;_TL286: if(num_pop > 0)goto _TL284;else{goto _TL285;}
_TL284: _T6=_check_null(x);_T7=_T6->hd;{struct _tuple27*_T11=(struct _tuple27*)_T7;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_Absyn_Stmt*_T14;int _T15;{struct _tuple27 _T16=*_T11;_T15=_T16.f0;_T14=_T16.f1;_T13=_T16.f2;_T12=_T16.f3;}{int is_open=_T15;struct Cyc_Absyn_Stmt*pat_stmt=_T14;struct Cyc_List_List*roots=_T13;struct Cyc_List_List*es=_T12;_T8=env;_T9=roots;_TA=es;_TB=is_open;_TC=inflow;_TD=dest;_TE=_TD->loc;
inflow=Cyc_NewControlFlow_unconsume_params(_T8,_T9,_TA,_TB,_TC,_TE);}}_TF=x;
# 2352
x=_TF->tl;num_pop=num_pop + -1;goto _TL286;_TL285:;}_T10=inflow;
# 2356
return _T10;}}
# 2362
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple15*rval_opt){union Cyc_CfFlowInfo_FlowInfo*_T0;struct Cyc_NewControlFlow_AnalEnv*_T1;struct Cyc_Absyn_Stmt*_T2;int*_T3;unsigned _T4;union Cyc_CfFlowInfo_FlowInfo _T5;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_NewControlFlow_AnalEnv*_T8;int _T9;struct Cyc_Absyn_Stmt*_TA;unsigned _TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct Cyc_Absyn_Stmt*_TE;unsigned _TF;struct Cyc_NewControlFlow_AnalEnv*_T10;union Cyc_CfFlowInfo_FlowInfo _T11;struct Cyc_NewControlFlow_AnalEnv*_T12;struct Cyc_NewControlFlow_AnalEnv*_T13;struct Cyc_List_List*_T14;union Cyc_CfFlowInfo_FlowInfo _T15;struct Cyc_Absyn_Stmt*_T16;unsigned _T17;union Cyc_CfFlowInfo_FlowInfo _T18;struct Cyc_NewControlFlow_AnalEnv*_T19;int _T1A;struct Cyc_Absyn_Stmt*_T1B;unsigned _T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;struct Cyc_NewControlFlow_AnalEnv*_T1F;struct Cyc_Absyn_Exp*_T20;unsigned _T21;union Cyc_CfFlowInfo_FlowInfo _T22;void*_T23;struct Cyc_Absyn_Stmt*_T24;unsigned _T25;struct Cyc_NewControlFlow_AnalEnv*_T26;union Cyc_CfFlowInfo_FlowInfo _T27;struct Cyc_NewControlFlow_AnalEnv*_T28;struct Cyc_NewControlFlow_AnalEnv*_T29;struct Cyc_List_List*_T2A;union Cyc_CfFlowInfo_FlowInfo _T2B;struct Cyc_Absyn_Stmt*_T2C;unsigned _T2D;union Cyc_CfFlowInfo_FlowInfo _T2E;struct _tuple15*_T2F;struct _tuple15*_T30;void**_T31;void**_T32;union Cyc_CfFlowInfo_FlowInfo _T33;struct _tuple14 _T34;union Cyc_CfFlowInfo_FlowInfo _T35;struct Cyc_NewControlFlow_AnalEnv*_T36;union Cyc_CfFlowInfo_FlowInfo _T37;struct Cyc_Absyn_Stmt*_T38;struct _tuple15*_T39;union Cyc_CfFlowInfo_FlowInfo _T3A;union Cyc_CfFlowInfo_FlowInfo _T3B;struct _tuple11 _T3C;struct _tuple11 _T3D;union Cyc_CfFlowInfo_FlowInfo*_T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;struct _tuple11 _T40;struct _tuple11 _T41;union Cyc_CfFlowInfo_FlowInfo*_T42;union Cyc_CfFlowInfo_FlowInfo _T43;struct _tuple11 _T44;struct _tuple11 _T45;struct _tuple11 _T46;struct _tuple11 _T47;struct _tuple14 _T48;union Cyc_CfFlowInfo_FlowInfo*_T49;union Cyc_CfFlowInfo_FlowInfo*_T4A;struct _tuple14 _T4B;union Cyc_CfFlowInfo_FlowInfo _T4C;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T4D;struct Cyc_Absyn_Switch_clause**_T4E;struct Cyc_Absyn_Switch_clause**_T4F;struct Cyc_NewControlFlow_AnalEnv*_T50;struct Cyc_Absyn_Stmt*_T51;union Cyc_CfFlowInfo_FlowInfo _T52;struct Cyc_Absyn_Switch_clause*_T53;struct Cyc_Absyn_Stmt*_T54;struct Cyc_Absyn_Switch_clause*_T55;struct Cyc_Core_Opt*_T56;struct Cyc_Core_Opt*_T57;void*_T58;struct Cyc_List_List*_T59;struct _tuple1 _T5A;struct Cyc_List_List*_T5B;struct Cyc_CfFlowInfo_FlowEnv*_T5C;union Cyc_CfFlowInfo_FlowInfo _T5D;struct Cyc_List_List*_T5E;struct Cyc_CfFlowInfo_FlowEnv*_T5F;void*_T60;struct Cyc_Absyn_Stmt*_T61;unsigned _T62;struct Cyc_Absyn_Switch_clause*_T63;struct Cyc_Core_Opt*_T64;struct Cyc_Core_Opt*_T65;void*_T66;struct Cyc_List_List*_T67;void*_T68;struct Cyc_CfFlowInfo_FlowEnv*_T69;struct Cyc_NewControlFlow_AnalEnv*_T6A;union Cyc_CfFlowInfo_FlowInfo _T6B;struct Cyc_Absyn_Vardecl**_T6C;struct Cyc_Absyn_Vardecl*_T6D;struct Cyc_List_List*_T6E;void*_T6F;struct Cyc_Absyn_Exp*_T70;struct Cyc_List_List*_T71;void*_T72;struct Cyc_Absyn_Stmt*_T73;unsigned _T74;struct Cyc_List_List*_T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;struct Cyc_NewControlFlow_AnalEnv*_T78;struct Cyc_Absyn_Switch_clause*_T79;struct Cyc_Absyn_Stmt*_T7A;union Cyc_CfFlowInfo_FlowInfo _T7B;union Cyc_CfFlowInfo_FlowInfo _T7C;int(*_T7D)(struct _fat_ptr,struct _fat_ptr);void*(*_T7E)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7F;struct _fat_ptr _T80;struct Cyc_Absyn_Stmt*(*_T81)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T82)(struct Cyc_Hashtable_Table*,void*);struct Cyc_NewControlFlow_AnalEnv*_T83;struct Cyc_Hashtable_Table*_T84;struct Cyc_Absyn_Stmt*_T85;struct Cyc_Absyn_Stmt*_T86;union Cyc_CfFlowInfo_FlowInfo _T87;struct _union_FlowInfo_ReachableFL _T88;unsigned _T89;struct Cyc_NewControlFlow_AnalEnv*_T8A;void*_T8B;int _T8C;struct Cyc_NewControlFlow_AnalEnv*_T8D;void*_T8E;int _T8F;struct Cyc_NewControlFlow_AnalEnv*_T90;void*_T91;int _T92;struct Cyc_Absyn_Stmt*_T93;unsigned _T94;struct _fat_ptr _T95;struct _fat_ptr _T96;struct Cyc_Absyn_Stmt*_T97;unsigned _T98;struct _fat_ptr _T99;struct _fat_ptr _T9A;struct Cyc_NewControlFlow_AnalEnv*_T9B;int _T9C;struct Cyc_Absyn_Stmt*_T9D;unsigned _T9E;struct _fat_ptr _T9F;struct _fat_ptr _TA0;struct Cyc_Absyn_Stmt*_TA1;unsigned _TA2;struct Cyc_NewControlFlow_AnalEnv*_TA3;union Cyc_CfFlowInfo_FlowInfo _TA4;struct Cyc_NewControlFlow_AnalEnv*_TA5;struct Cyc_NewControlFlow_AnalEnv*_TA6;struct Cyc_List_List*_TA7;union Cyc_CfFlowInfo_FlowInfo _TA8;struct Cyc_Absyn_Stmt*_TA9;unsigned _TAA;union Cyc_CfFlowInfo_FlowInfo _TAB;struct Cyc_Absyn_Stmt*(*_TAC)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_TAD)(struct Cyc_Hashtable_Table*,void*);struct Cyc_NewControlFlow_AnalEnv*_TAE;struct Cyc_Hashtable_Table*_TAF;struct Cyc_Absyn_Stmt*_TB0;struct Cyc_Absyn_Stmt*_TB1;unsigned _TB2;struct _fat_ptr _TB3;struct _fat_ptr _TB4;struct Cyc_NewControlFlow_AnalEnv*_TB5;struct Cyc_Absyn_Stmt*_TB6;union Cyc_CfFlowInfo_FlowInfo _TB7;struct Cyc_Absyn_Stmt*_TB8;union Cyc_CfFlowInfo_FlowInfo _TB9;void*_TBA;struct Cyc_NewControlFlow_AnalEnv*_TBB;union Cyc_CfFlowInfo_FlowInfo _TBC;struct Cyc_List_List*_TBD;struct Cyc_Absyn_Exp*_TBE;unsigned _TBF;union Cyc_CfFlowInfo_FlowInfo _TC0;void*_TC1;struct Cyc_NewControlFlow_AnalEnv*_TC2;struct Cyc_NewControlFlow_AnalEnv*_TC3;struct Cyc_NewControlFlow_AnalEnv*_TC4;struct Cyc_NewControlFlow_AnalEnv*_TC5;struct Cyc_NewControlFlow_AnalEnv*_TC6;struct Cyc_NewControlFlow_AnalEnv*_TC7;struct Cyc_NewControlFlow_AnalEnv*_TC8;union Cyc_CfFlowInfo_FlowInfo _TC9;struct _union_FlowInfo_BottomFL _TCA;unsigned _TCB;struct Cyc_Absyn_Stmt*_TCC;unsigned _TCD;struct _fat_ptr _TCE;struct _fat_ptr _TCF;union Cyc_CfFlowInfo_FlowInfo _TD0;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_TD1;struct Cyc_Absyn_Decl*_TD2;struct Cyc_Absyn_Decl*_TD3;void*_TD4;int*_TD5;unsigned _TD6;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_TD7;struct Cyc_Absyn_Decl*_TD8;struct Cyc_Absyn_Decl*_TD9;void*_TDA;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_TDB;struct Cyc_Core_Opt*_TDC;struct Cyc_Absyn_Decl*_TDD;void*_TDE;struct Cyc_Core_Opt*_TDF;void*_TE0;struct Cyc___cycFILE*_TE1;struct _fat_ptr _TE2;struct _fat_ptr _TE3;struct Cyc_Absyn_Pat*_TE4;int*_TE5;int _TE6;struct Cyc_CfFlowInfo_FlowEnv*_TE7;struct Cyc_NewControlFlow_AnalEnv*_TE8;union Cyc_CfFlowInfo_FlowInfo _TE9;struct Cyc_List_List*_TEA;int _TEB;unsigned _TEC;struct Cyc_Absyn_Exp*_TED;unsigned _TEE;int _TEF;struct Cyc_NewControlFlow_AnalEnv*_TF0;struct Cyc_NewControlFlow_AnalEnv*_TF1;struct Cyc_List_List*_TF2;struct _tuple27*_TF3;struct Cyc_NewControlFlow_AnalEnv*_TF4;union Cyc_CfFlowInfo_FlowInfo _TF5;struct Cyc_Absyn_Decl*_TF6;void*_TF7;struct Cyc_Absyn_Exp*_TF8;struct Cyc_List_List _TF9;struct Cyc_NewControlFlow_AnalEnv*_TFA;union Cyc_CfFlowInfo_FlowInfo _TFB;struct Cyc_List_List*_TFC;struct Cyc_List_List*_TFD;union Cyc_CfFlowInfo_FlowInfo _TFE;struct _union_FlowInfo_ReachableFL _TFF;unsigned _T100;union Cyc_CfFlowInfo_FlowInfo _T101;struct _union_FlowInfo_ReachableFL _T102;union Cyc_CfFlowInfo_AbsLVal _T103;struct _union_AbsLVal_PlaceL _T104;unsigned _T105;union Cyc_CfFlowInfo_AbsLVal _T106;struct _union_AbsLVal_PlaceL _T107;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T108;struct Cyc_CfFlowInfo_FlowEnv*_T109;struct Cyc_Absyn_Exp*_T10A;unsigned _T10B;struct Cyc_Dict_Dict _T10C;struct Cyc_CfFlowInfo_Place*_T10D;void*_T10E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T10F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T110;struct Cyc_CfFlowInfo_Place*_T111;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T112;struct Cyc_CfFlowInfo_Place*_T113;struct Cyc_List_List*_T114;struct Cyc_List_List*_T115;struct Cyc_List_List _T116;struct Cyc_CfFlowInfo_FlowEnv*_T117;union Cyc_CfFlowInfo_FlowInfo _T118;struct Cyc_List_List*_T119;struct Cyc_List_List*_T11A;struct Cyc_CfFlowInfo_FlowEnv*_T11B;void*_T11C;unsigned _T11D;struct Cyc_NewControlFlow_AnalEnv*_T11E;struct Cyc_NewControlFlow_AnalEnv*_T11F;struct Cyc_List_List*_T120;struct _tuple27*_T121;struct Cyc_NewControlFlow_AnalEnv*_T122;union Cyc_CfFlowInfo_FlowInfo _T123;struct Cyc_NewControlFlow_AnalEnv*_T124;union Cyc_CfFlowInfo_FlowInfo _T125;struct Cyc_Absyn_Stmt*_T126;struct _tuple15*_T127;int _T128;union Cyc_CfFlowInfo_FlowInfo _T129;union Cyc_CfFlowInfo_FlowInfo _T12A;
# 2364
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple18 _T12B=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);union Cyc_CfFlowInfo_FlowInfo*_T12C;struct Cyc_NewControlFlow_CFStmtAnnot*_T12D;_T12D=_T12B.f0;_T12C=_T12B.f1;{struct Cyc_NewControlFlow_CFStmtAnnot*annot=_T12D;union Cyc_CfFlowInfo_FlowInfo*sflow=_T12C;_T0=sflow;
inflow=*_T0;_T1=env;{
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T1->fenv;_T2=s;{
# 2371
void*_T12E=_T2->r;struct Cyc_Absyn_Decl*_T12F;struct Cyc_Absyn_Vardecl*_T130;unsigned _T131;struct Cyc_Absyn_Pat*_T132;struct Cyc_List_List*_T133;struct Cyc_Absyn_Stmt*_T134;struct Cyc_Absyn_Stmt*_T135;struct Cyc_Absyn_Exp*_T136;struct Cyc_Absyn_Stmt*_T137;void*_T138;void*_T139;_T3=(int*)_T12E;_T4=*_T3;switch(_T4){case 0: _T5=inflow;
return _T5;case 3: _T6=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T12E;_T7=_T6->f1;if(_T7!=0)goto _TL288;_T8=env;_T9=_T8->noreturn;
# 2375
if(!_T9)goto _TL28A;_TA=s;_TB=_TA->loc;_TC=
_tag_fat("`noreturn' function might return",sizeof(char),33U);_TD=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_TB,_TC,_TD);goto _TL28B;_TL28A: _TL28B: _TE=s;_TF=_TE->loc;_T10=env;_T11=inflow;
Cyc_NewControlFlow_check_init_params(_TF,_T10,_T11);_T12=env;_T13=env;_T14=_T13->noconsume_params;_T15=inflow;_T16=s;_T17=_T16->loc;
Cyc_NewControlFlow_unconsume_params(_T12,_T14,0,0,_T15,_T17);_T18=
Cyc_CfFlowInfo_BottomFL();return _T18;_TL288:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T12E;_T139=_T13A->f1;}{struct Cyc_Absyn_Exp*e=_T139;_T19=env;_T1A=_T19->noreturn;
# 2381
if(!_T1A)goto _TL28C;_T1B=s;_T1C=_T1B->loc;_T1D=
_tag_fat("`noreturn' function might return",sizeof(char),33U);_T1E=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T1C,_T1D,_T1E);goto _TL28D;_TL28C: _TL28D: {
struct _tuple14 _T13A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e,0);void*_T13B;union Cyc_CfFlowInfo_FlowInfo _T13C;_T13C=_T13A.f0;_T13B=_T13A.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T13C;void*r=_T13B;_T1F=env;_T20=e;_T21=_T20->loc;_T22=f;_T23=r;
f=Cyc_NewControlFlow_use_Rval(_T1F,_T21,_T22,_T23);_T24=s;_T25=_T24->loc;_T26=env;_T27=f;
Cyc_NewControlFlow_check_init_params(_T25,_T26,_T27);_T28=env;_T29=env;_T2A=_T29->noconsume_params;_T2B=f;_T2C=s;_T2D=_T2C->loc;
Cyc_NewControlFlow_unconsume_params(_T28,_T2A,0,0,_T2B,_T2D);_T2E=
Cyc_CfFlowInfo_BottomFL();return _T2E;}}}case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T12E;_T139=_T13A->f1;}{struct Cyc_Absyn_Exp*e=_T139;int _T13A;void*_T13B;if(rval_opt==0)goto _TL28E;_T2F=rval_opt;{struct _tuple15 _T13C=*_T2F;_T30=rval_opt;_T31=& _T30->f0;_T13B=(void**)_T31;_T13A=_T13C.f1;}{void**rval=(void**)_T13B;int copy_ctxt=_T13A;
# 2392
struct _tuple14 _T13C=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e,0);void*_T13D;union Cyc_CfFlowInfo_FlowInfo _T13E;_T13E=_T13C.f0;_T13D=_T13C.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T13E;void*r=_T13D;_T32=rval;
*_T32=r;_T33=f;
return _T33;}}_TL28E: _T34=
# 2396
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0);_T35=_T34.f0;return _T35;;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T12E;_T139=_T13A->f1;_T138=_T13A->f2;}{struct Cyc_Absyn_Stmt*s1=_T139;struct Cyc_Absyn_Stmt*s2=_T138;_T36=env;_T37=
# 2400
Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);_T38=s2;_T39=rval_opt;_T3A=Cyc_NewControlFlow_anal_stmt(_T36,_T37,_T38,_T39);return _T3A;}case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T12E;_T139=_T13A->f1;_T138=_T13A->f2;_T137=_T13A->f3;}{struct Cyc_Absyn_Exp*e=_T139;struct Cyc_Absyn_Stmt*s1=_T138;struct Cyc_Absyn_Stmt*s2=_T137;
# 2403
struct _tuple17 _T13A=Cyc_NewControlFlow_anal_test(env,inflow,e);union Cyc_CfFlowInfo_FlowInfo _T13B;union Cyc_CfFlowInfo_FlowInfo _T13C;_T13C=_T13A.f0;_T13B=_T13A.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T13C;union Cyc_CfFlowInfo_FlowInfo f1f=_T13B;
# 2410
union Cyc_CfFlowInfo_FlowInfo ff=Cyc_NewControlFlow_anal_stmt(env,f1f,s2,0);
union Cyc_CfFlowInfo_FlowInfo ft=Cyc_NewControlFlow_anal_stmt(env,f1t,s1,0);_T3B=
Cyc_CfFlowInfo_join_flow(fenv,ft,ff);return _T3B;}}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T12E;_T3C=_T13A->f1;_T139=_T3C.f0;_T3D=_T13A->f1;_T138=_T3D.f1;_T137=_T13A->f2;}{struct Cyc_Absyn_Exp*e=_T139;struct Cyc_Absyn_Stmt*cont=_T138;struct Cyc_Absyn_Stmt*body=_T137;
# 2418
struct _tuple18 _T13A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,cont);union Cyc_CfFlowInfo_FlowInfo*_T13B;_T13B=_T13A.f1;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_T13B;_T3E=eflow_ptr;{
union Cyc_CfFlowInfo_FlowInfo e_inflow=*_T3E;
struct _tuple17 _T13C=Cyc_NewControlFlow_anal_test(env,e_inflow,e);union Cyc_CfFlowInfo_FlowInfo _T13D;union Cyc_CfFlowInfo_FlowInfo _T13E;_T13E=_T13C.f0;_T13D=_T13C.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T13E;union Cyc_CfFlowInfo_FlowInfo f1f=_T13D;
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,f1t,body,0);
Cyc_NewControlFlow_update_flow(env,cont,body_outflow);_T3F=f1f;
return _T3F;}}}}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T12E;_T139=_T13A->f1;_T40=_T13A->f2;_T138=_T40.f0;_T41=_T13A->f2;_T137=_T41.f1;}{struct Cyc_Absyn_Stmt*body=_T139;struct Cyc_Absyn_Exp*e=_T138;struct Cyc_Absyn_Stmt*cont=_T137;
# 2428
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,body,0);
struct _tuple18 _T13A=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);union Cyc_CfFlowInfo_FlowInfo*_T13B;_T13B=_T13A.f1;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_T13B;_T42=eflow_ptr;{
union Cyc_CfFlowInfo_FlowInfo e_inflow=*_T42;
struct _tuple17 _T13C=Cyc_NewControlFlow_anal_test(env,e_inflow,e);union Cyc_CfFlowInfo_FlowInfo _T13D;union Cyc_CfFlowInfo_FlowInfo _T13E;_T13E=_T13C.f0;_T13D=_T13C.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_T13E;union Cyc_CfFlowInfo_FlowInfo f1f=_T13D;
Cyc_NewControlFlow_update_flow(env,body,f1t);_T43=f1f;
return _T43;}}}}case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T12E;_T139=_T13A->f1;_T44=_T13A->f2;_T138=_T44.f0;_T45=_T13A->f2;_T137=_T45.f1;_T46=_T13A->f3;_T136=_T46.f0;_T47=_T13A->f3;_T135=_T47.f1;_T134=_T13A->f4;}{struct Cyc_Absyn_Exp*e1=_T139;struct Cyc_Absyn_Exp*e2=_T138;struct Cyc_Absyn_Stmt*guard=_T137;struct Cyc_Absyn_Exp*e3=_T136;struct Cyc_Absyn_Stmt*cont=_T135;struct Cyc_Absyn_Stmt*body=_T134;_T48=
# 2437
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);{union Cyc_CfFlowInfo_FlowInfo e1_outflow=_T48.f0;
struct _tuple18 _T13A=Cyc_NewControlFlow_pre_stmt_check(env,e1_outflow,guard);union Cyc_CfFlowInfo_FlowInfo*_T13B;_T13B=_T13A.f1;{union Cyc_CfFlowInfo_FlowInfo*e2flow_ptr=_T13B;_T49=e2flow_ptr;{
union Cyc_CfFlowInfo_FlowInfo e2_inflow=*_T49;
struct _tuple17 _T13C=Cyc_NewControlFlow_anal_test(env,e2_inflow,e2);union Cyc_CfFlowInfo_FlowInfo _T13D;union Cyc_CfFlowInfo_FlowInfo _T13E;_T13E=_T13C.f0;_T13D=_T13C.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_T13E;union Cyc_CfFlowInfo_FlowInfo f2f=_T13D;
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,f2t,body,0);
struct _tuple18 _T13F=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);union Cyc_CfFlowInfo_FlowInfo*_T140;_T140=_T13F.f1;{union Cyc_CfFlowInfo_FlowInfo*e3flow_ptr=_T140;_T4A=e3flow_ptr;{
union Cyc_CfFlowInfo_FlowInfo e3_inflow=*_T4A;_T4B=
Cyc_NewControlFlow_anal_Rexp(env,0,e3_inflow,e3,0);{union Cyc_CfFlowInfo_FlowInfo e3_outflow=_T4B.f0;
Cyc_NewControlFlow_update_flow(env,guard,e3_outflow);_T4C=f2f;
return _T4C;}}}}}}}}case 11: _T4D=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T12E;_T4E=_T4D->f2;if(_T4E==0)goto _TL290;{struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T12E;_T139=_T13A->f1;_T4F=_T13A->f2;{struct Cyc_Absyn_Switch_clause*_T13B=*_T4F;_T138=_T13B;}}{struct Cyc_List_List*es=_T139;struct Cyc_Absyn_Switch_clause*destclause=_T138;
# 2449
struct _tuple21 _T13A=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct Cyc_List_List*_T13B;union Cyc_CfFlowInfo_FlowInfo _T13C;_T13C=_T13A.f0;_T13B=_T13A.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T13C;struct Cyc_List_List*rvals=_T13B;_T50=env;_T51=s;_T52=inflow;_T53=destclause;_T54=_T53->body;
# 2451
inflow=Cyc_NewControlFlow_unconsume_pat_vars(_T50,_T51,_T52,_T54);_T55=destclause;_T56=_T55->pat_vars;_T57=
# 2453
_check_null(_T56);_T58=_T57->v;_T59=(struct Cyc_List_List*)_T58;_T5A=Cyc_List_split(_T59);_T5B=_T5A.f0;{struct Cyc_List_List*vds=Cyc_Tcutil_filter_nulls(_T5B);_T5C=fenv;_T5D=f;_T5E=vds;_T5F=fenv;_T60=_T5F->unknown_all;_T61=s;_T62=_T61->loc;
f=Cyc_NewControlFlow_add_vars(_T5C,_T5D,_T5E,_T60,_T62,0);_T63=destclause;_T64=_T63->pat_vars;_T65=
# 2456
_check_null(_T64);_T66=_T65->v;{struct Cyc_List_List*x=(struct Cyc_List_List*)_T66;_TL295: if(x!=0)goto _TL293;else{goto _TL294;}
_TL293: _T67=x;_T68=_T67->hd;{struct _tuple22*_T13D=(struct _tuple22*)_T68;struct Cyc_Absyn_Exp*_T13E;struct Cyc_Absyn_Vardecl**_T13F;{struct _tuple22 _T140=*_T13D;_T13F=_T140.f0;_T13E=_T140.f1;}{struct Cyc_Absyn_Vardecl**vd=_T13F;struct Cyc_Absyn_Exp*ve=_T13E;
if(vd==0)goto _TL296;_T69=fenv;_T6A=env;_T6B=f;_T6C=vd;_T6D=*_T6C;_T6E=
_check_null(es);_T6F=_T6E->hd;_T70=(struct Cyc_Absyn_Exp*)_T6F;_T71=_check_null(rvals);_T72=_T71->hd;_T73=s;_T74=_T73->loc;f=Cyc_NewControlFlow_do_initialize_var(_T69,_T6A,_T6B,_T6D,_T70,_T72,_T74);_T75=rvals;
rvals=_T75->tl;_T76=es;
es=_T76->tl;goto _TL297;_TL296: _TL297:;}}_T77=x;
# 2456
x=_T77->tl;goto _TL295;_TL294:;}_T78=env;_T79=destclause;_T7A=_T79->body;_T7B=f;
# 2464
Cyc_NewControlFlow_update_flow(_T78,_T7A,_T7B);_T7C=
Cyc_CfFlowInfo_BottomFL();return _T7C;}}}_TL290: _T7E=Cyc_Warn_impos;{
# 2635
int(*_T13A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7E;_T7D=_T13A;}_T7F=_tag_fat("anal_stmt: bad stmt syntax or unimplemented stmt form",sizeof(char),54U);_T80=_tag_fat(0U,sizeof(void*),0);_T7D(_T7F,_T80);;case 6: _T82=Cyc_Hashtable_lookup;{
# 2470
struct Cyc_Absyn_Stmt*(*_T13A)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T82;_T81=_T13A;}_T83=env;_T84=_T83->succ_table;_T85=s;_T86=_T81(_T84,_T85);if(_T86!=0)goto _TL298;_T87=inflow;_T88=_T87.ReachableFL;_T89=_T88.tag;if(_T89!=2)goto _TL29A;_T8A=env;_T8B=_T8A->return_type;_T8C=
# 2473
Cyc_Tcutil_is_void_type(_T8B);if(_T8C)goto _TL29C;else{goto _TL29E;}
_TL29E: _T8D=env;_T8E=_T8D->return_type;_T8F=Cyc_Tcutil_is_any_float_type(_T8E);if(_T8F)goto _TL2A1;else{goto _TL2A2;}_TL2A2: _T90=env;_T91=_T90->return_type;_T92=
Cyc_Tcutil_is_any_int_type(_T91);
# 2474
if(_T92)goto _TL2A1;else{goto _TL29F;}
# 2476
_TL2A1: _T93=s;_T94=_T93->loc;_T95=_tag_fat("break may cause function not to return a value",sizeof(char),47U);_T96=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T94,_T95,_T96);goto _TL2A0;
# 2478
_TL29F: _T97=s;_T98=_T97->loc;_T99=_tag_fat("break may cause function not to return a value",sizeof(char),47U);_T9A=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T98,_T99,_T9A);_TL2A0: goto _TL29D;_TL29C: _TL29D: goto _LL55;_TL29A: goto _LL55;_LL55: _T9B=env;_T9C=_T9B->noreturn;
# 2483
if(!_T9C)goto _TL2A3;_T9D=s;_T9E=_T9D->loc;_T9F=
_tag_fat("`noreturn' function might return",sizeof(char),33U);_TA0=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T9E,_T9F,_TA0);goto _TL2A4;_TL2A3: _TL2A4: _TA1=s;_TA2=_TA1->loc;_TA3=env;_TA4=inflow;
Cyc_NewControlFlow_check_init_params(_TA2,_TA3,_TA4);_TA5=env;_TA6=env;_TA7=_TA6->noconsume_params;_TA8=inflow;_TA9=s;_TAA=_TA9->loc;
Cyc_NewControlFlow_unconsume_params(_TA5,_TA7,0,0,_TA8,_TAA);_TAB=
Cyc_CfFlowInfo_BottomFL();return _TAB;_TL298: goto _LL1B;case 7: _LL1B: goto _LL1D;case 8: _LL1D: _TAD=Cyc_Hashtable_lookup;{
# 2493
struct Cyc_Absyn_Stmt*(*_T13A)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_TAD;_TAC=_T13A;}_TAE=env;_TAF=_TAE->succ_table;_TB0=s;{struct Cyc_Absyn_Stmt*dest=_TAC(_TAF,_TB0);
if(dest!=0)goto _TL2A5;_TB1=s;_TB2=_TB1->loc;_TB3=
_tag_fat("jump has no target (should be impossible)",sizeof(char),42U);_TB4=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_TB2,_TB3,_TB4);goto _TL2A6;_TL2A5: _TL2A6: _TB5=env;_TB6=s;_TB7=inflow;_TB8=
_check_null(dest);inflow=Cyc_NewControlFlow_unconsume_pat_vars(_TB5,_TB6,_TB7,_TB8);
# 2498
Cyc_NewControlFlow_update_flow(env,dest,inflow);_TB9=
Cyc_CfFlowInfo_BottomFL();return _TB9;}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T12E;_T139=_T13A->f1;_T133=_T13A->f2;_TBA=_T13A->f3;_T138=(void*)_TBA;}{struct Cyc_Absyn_Exp*e=_T139;struct Cyc_List_List*scs=_T133;void*dec_tree=_T138;_TBB=env;_TBC=inflow;_TBD=scs;_TBE=e;_TBF=_TBE->loc;_TC0=
# 2504
Cyc_NewControlFlow_anal_scs(_TBB,_TBC,_TBD,_TBF);return _TC0;}case 15:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T12E;_T137=_T13A->f1;_T133=_T13A->f2;_TC1=_T13A->f3;_T139=(void*)_TC1;}{struct Cyc_Absyn_Stmt*s1=_T137;struct Cyc_List_List*scs=_T133;void*dec_tree=_T139;_TC2=env;{
# 2509
int old_in_try=_TC2->in_try;_TC3=env;{
union Cyc_CfFlowInfo_FlowInfo old_tryflow=_TC3->tryflow;_TC4=env;
_TC4->in_try=1;_TC5=env;
_TC5->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);_TC6=env;{
union Cyc_CfFlowInfo_FlowInfo scs_inflow=_TC6->tryflow;_TC7=env;
# 2517
_TC7->in_try=old_in_try;_TC8=env;
_TC8->tryflow=old_tryflow;
# 2520
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
# 2522
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,scs,0U);_TC9=scs_outflow;_TCA=_TC9.BottomFL;_TCB=_TCA.tag;if(_TCB!=1)goto _TL2A7;goto _LL5A;_TL2A7: _TCC=s;_TCD=_TCC->loc;_TCE=
# 2525
_tag_fat("last catch clause may implicitly fallthru",sizeof(char),42U);_TCF=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_TCD,_TCE,_TCF);_LL5A:
# 2527
 outflow=s1_outflow;_TD0=outflow;
# 2529
return _TD0;}}}}}}case 12: _TD1=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T12E;_TD2=_TD1->f1;_TD3=(struct Cyc_Absyn_Decl*)_TD2;_TD4=_TD3->r;_TD5=(int*)_TD4;_TD6=*_TD5;switch(_TD6){case 2: _TD7=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T12E;_TD8=_TD7->f1;_TD9=(struct Cyc_Absyn_Decl*)_TD8;_TDA=_TD9->r;_TDB=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_TDA;_TDC=_TDB->f2;if(_TDC==0)goto _TL2AA;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T12E;_TDD=_T13A->f1;{struct Cyc_Absyn_Decl _T13B=*_TDD;_TDE=_T13B.r;{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T13C=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_TDE;_T132=_T13C->f1;_TDF=_T13C->f2;{struct Cyc_Core_Opt _T13D=*_TDF;_TE0=_T13D.v;_T133=(struct Cyc_List_List*)_TE0;}_T136=_T13C->f3;}_T131=_T13B.loc;}_T137=_T13A->f2;}{struct Cyc_Absyn_Pat*letpat=_T132;struct Cyc_List_List*vds=_T133;struct Cyc_Absyn_Exp*e=_T136;unsigned loc=_T131;struct Cyc_Absyn_Stmt*s1=_T137;_TE1=Cyc_stderr;_TE2=
# 2534
_tag_fat("",sizeof(char),1U);_TE3=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TE1,_TE2,_TE3);{
# 2540
int isAliasPat=0;_TE4=letpat;{
void*_T13A=_TE4->r;_TE5=(int*)_T13A;_TE6=*_TE5;if(_TE6!=2)goto _TL2AC;
isAliasPat=1;goto _LL5F;_TL2AC: goto _LL5F;_LL5F:;}{
# 2545
struct _tuple1 _T13A=Cyc_NewControlFlow_get_unconsume_pat_vars(env,vds);struct Cyc_List_List*_T13B;struct Cyc_List_List*_T13C;_T13C=_T13A.f0;_T13B=_T13A.f1;{struct Cyc_List_List*roots=_T13C;struct Cyc_List_List*es=_T13B;_TE7=fenv;_TE8=env;_TE9=inflow;_TEA=vds;_TEB=roots!=0;_TEC=loc;_TED=e;_TEE=_TED->loc;_TEF=isAliasPat;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_TE7,_TE8,_TE9,_TEA,_TEB,_TEC,_TEE,_TEF);_TF0=env;{
struct Cyc_List_List*old_unique_pat_vars=_TF0->unique_pat_vars;_TF1=env;{struct Cyc_List_List*_T13D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple27*_T13E=_cycalloc(sizeof(struct _tuple27));
# 2550
_T13E->f0=0;_T13E->f1=s;_T13E->f2=roots;_T13E->f3=es;_TF3=(struct _tuple27*)_T13E;}_T13D->hd=_TF3;_T13D->tl=old_unique_pat_vars;_TF2=(struct Cyc_List_List*)_T13D;}
# 2549
_TF1->unique_pat_vars=_TF2;
# 2554
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,rval_opt);_TF4=env;
_TF4->unique_pat_vars=old_unique_pat_vars;
# 2558
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,inflow,loc);_TF5=inflow;
# 2562
return _TF5;}}}}}_TL2AA: goto _LL26;case 4:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T12E;_TF6=_T13A->f1;{struct Cyc_Absyn_Decl _T13B=*_TF6;_TF7=_T13B.r;{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T13C=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_TF7;_T130=_T13C->f2;_T136=_T13C->f3;}_T131=_T13B.loc;}_T137=_T13A->f2;}_TF8=(struct Cyc_Absyn_Exp*)_T136;if(_TF8==0)goto _TL2AE;{struct Cyc_Absyn_Vardecl*vd=_T130;struct Cyc_Absyn_Exp*open_exp_opt=_T136;unsigned loc=_T131;struct Cyc_Absyn_Stmt*s1=_T137;{struct Cyc_List_List _T13A;
# 2574
_T13A.hd=open_exp_opt;_T13A.tl=0;_TF9=_T13A;}{struct Cyc_List_List l=_TF9;_TFA=env;_TFB=inflow;_TFC=& l;_TFD=(struct Cyc_List_List*)_TFC;{
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_expand_unique_places(_TFA,_TFB,_TFD);
struct _tuple16 _T13A=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,open_exp_opt);union Cyc_CfFlowInfo_AbsLVal _T13B;_T13B=_T13A.f1;{union Cyc_CfFlowInfo_AbsLVal lval=_T13B;
struct _tuple14 _T13C=Cyc_NewControlFlow_anal_Rexp(env,1,f,open_exp_opt,0);union Cyc_CfFlowInfo_FlowInfo _T13D;_T13D=_T13C.f0;{union Cyc_CfFlowInfo_FlowInfo f=_T13D;
struct Cyc_List_List*roots=0;
struct Cyc_List_List*es=0;{struct Cyc_Dict_Dict _T13E;_TFE=f;_TFF=_TFE.ReachableFL;_T100=_TFF.tag;if(_T100!=2)goto _TL2B0;_T101=f;_T102=_T101.ReachableFL;_T13E=_T102.val;{struct Cyc_Dict_Dict fd=_T13E;{struct Cyc_CfFlowInfo_Place*_T13F;_T103=lval;_T104=_T103.PlaceL;_T105=_T104.tag;if(_T105!=1)goto _TL2B2;_T106=lval;_T107=_T106.PlaceL;_T13F=_T107.val;{struct Cyc_CfFlowInfo_Place*p=_T13F;
# 2586
void*new_rval=Cyc_CfFlowInfo_lookup_place(fd,p);{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T140=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T140->tag=8;
_T140->f1=vd;_T140->f2=new_rval;_T108=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T140;}new_rval=(void*)_T108;_T109=fenv;_T10A=open_exp_opt;_T10B=_T10A->loc;_T10C=fd;_T10D=p;_T10E=new_rval;
fd=Cyc_CfFlowInfo_assign_place(_T109,_T10B,_T10C,_T10D,_T10E);
f=Cyc_CfFlowInfo_ReachableFL(fd);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_T10F=root;_T10F->tag=0;_T110=root;_T110->f1=vd;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T111=rp;_T112=root;_T111->root=(void*)_T112;_T113=rp;_T113->path=0;{struct Cyc_List_List*_T140=_cycalloc(sizeof(struct Cyc_List_List));
_T140->hd=rp;_T140->tl=roots;_T114=(struct Cyc_List_List*)_T140;}roots=_T114;{struct Cyc_List_List*_T140=_cycalloc(sizeof(struct Cyc_List_List));
_T140->hd=open_exp_opt;_T140->tl=es;_T115=(struct Cyc_List_List*)_T140;}es=_T115;goto _LL72;}}}_TL2B2: goto _LL72;_LL72:;}goto _LL6D;}_TL2B0: goto _LL6D;_LL6D:;}{struct Cyc_List_List _T13E;
# 2608
_T13E.hd=vd;_T13E.tl=0;_T116=_T13E;}{struct Cyc_List_List vds=_T116;_T117=fenv;_T118=f;_T119=& vds;_T11A=(struct Cyc_List_List*)_T119;_T11B=fenv;_T11C=_T11B->unknown_all;_T11D=loc;
f=Cyc_NewControlFlow_add_vars(_T117,_T118,_T11A,_T11C,_T11D,0);_T11E=env;{
# 2612
struct Cyc_List_List*old_unique_pat_vars=_T11E->unique_pat_vars;_T11F=env;{struct Cyc_List_List*_T13E=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple27*_T13F=_cycalloc(sizeof(struct _tuple27));
_T13F->f0=1;_T13F->f1=s;_T13F->f2=roots;_T13F->f3=es;_T121=(struct _tuple27*)_T13F;}_T13E->hd=_T121;_T13E->tl=old_unique_pat_vars;_T120=(struct Cyc_List_List*)_T13E;}_T11F->unique_pat_vars=_T120;
# 2617
f=Cyc_NewControlFlow_anal_stmt(env,f,s1,rval_opt);_T122=env;
_T122->unique_pat_vars=old_unique_pat_vars;
# 2621
f=Cyc_NewControlFlow_unconsume_params(env,roots,es,1,f,loc);_T123=f;
# 2625
return _T123;}}}}}}}_TL2AE: goto _LL26;default: _LL26:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T12E;_T12F=_T13A->f1;_T137=_T13A->f2;}{struct Cyc_Absyn_Decl*d=_T12F;struct Cyc_Absyn_Stmt*s=_T137;_T124=env;_T125=
# 2628
Cyc_NewControlFlow_anal_decl(env,inflow,d);_T126=s;_T127=rval_opt;outflow=Cyc_NewControlFlow_anal_stmt(_T124,_T125,_T126,_T127);_T128=Cyc_Flags_warn_lose_unique;
if(!_T128)goto _TL2B4;
Cyc_NewControlFlow_check_dropped_unique(env,outflow,d);goto _TL2B5;_TL2B4: _TL2B5: _T129=outflow;
return _T129;}};default:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T13A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T12E;_T137=_T13A->f2;}{struct Cyc_Absyn_Stmt*s=_T137;_T12A=
# 2633
Cyc_NewControlFlow_anal_stmt(env,inflow,s,rval_opt);return _T12A;}};}}}}
# 2639
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Fndecl*);
# 2643
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_NewControlFlow_AnalEnv*_T0;struct Cyc_Absyn_Decl*_T1;int*_T2;unsigned _T3;struct Cyc_Absyn_Vardecl*_T4;enum Cyc_Absyn_Scope _T5;int _T6;union Cyc_CfFlowInfo_FlowInfo _T7;struct Cyc_List_List _T8;struct Cyc_CfFlowInfo_FlowEnv*_T9;union Cyc_CfFlowInfo_FlowInfo _TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_CfFlowInfo_FlowEnv*_TD;void*_TE;struct Cyc_Absyn_Decl*_TF;unsigned _T10;struct Cyc_Absyn_Vardecl*_T11;union Cyc_CfFlowInfo_FlowInfo _T12;struct Cyc_CfFlowInfo_FlowEnv*_T13;struct Cyc_NewControlFlow_AnalEnv*_T14;union Cyc_CfFlowInfo_FlowInfo _T15;struct Cyc_Absyn_Vardecl*_T16;struct Cyc_Absyn_Exp*_T17;void*_T18;struct Cyc_Absyn_Decl*_T19;unsigned _T1A;union Cyc_CfFlowInfo_FlowInfo _T1B;struct Cyc_CfFlowInfo_FlowEnv*_T1C;union Cyc_CfFlowInfo_FlowInfo _T1D;struct Cyc_List_List*_T1E;struct Cyc_CfFlowInfo_FlowEnv*_T1F;void*_T20;struct Cyc_Absyn_Decl*_T21;unsigned _T22;union Cyc_CfFlowInfo_FlowInfo _T23;struct Cyc_NewControlFlow_AnalEnv*_T24;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T25;struct Cyc_CfFlowInfo_FlowEnv*_T26;union Cyc_CfFlowInfo_FlowInfo _T27;struct Cyc_Absyn_Fndecl*_T28;struct Cyc_Absyn_Fndecl*_T29;struct Cyc_CfFlowInfo_FlowEnv*_T2A;union Cyc_CfFlowInfo_FlowInfo _T2B;struct Cyc_List_List*_T2C;struct Cyc_CfFlowInfo_FlowEnv*_T2D;void*_T2E;struct Cyc_Absyn_Decl*_T2F;unsigned _T30;union Cyc_CfFlowInfo_FlowInfo _T31;int(*_T32)(struct _fat_ptr,struct _fat_ptr);void*(*_T33)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T34;struct _fat_ptr _T35;struct Cyc_List_List _T36;struct Cyc_CfFlowInfo_FlowEnv*_T37;union Cyc_CfFlowInfo_FlowInfo _T38;struct Cyc_List_List*_T39;struct Cyc_List_List*_T3A;struct Cyc_CfFlowInfo_FlowEnv*_T3B;void*_T3C;struct Cyc_Absyn_Decl*_T3D;unsigned _T3E;union Cyc_CfFlowInfo_FlowInfo _T3F;int(*_T40)(struct _fat_ptr,struct _fat_ptr);void*(*_T41)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T42;struct _fat_ptr _T43;_T0=env;{
struct Cyc_CfFlowInfo_FlowEnv*fenv=_T0->fenv;_T1=decl;{
void*_T44=_T1->r;struct Cyc_Absyn_Exp*_T45;struct Cyc_Absyn_Tvar*_T46;struct Cyc_Absyn_Fndecl*_T47;struct Cyc_List_List*_T48;struct Cyc_Absyn_Vardecl*_T49;_T2=(int*)_T44;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T44;_T49=_T4A->f1;}{struct Cyc_Absyn_Vardecl*vd=_T49;_T4=vd;_T5=_T4->sc;_T6=(int)_T5;
# 2651
if(_T6!=0)goto _TL2B7;_T7=inflow;
return _T7;_TL2B7:{struct Cyc_List_List _T4A;
# 2656
_T4A.hd=vd;_T4A.tl=0;_T8=_T4A;}{struct Cyc_List_List vds=_T8;_T9=fenv;_TA=inflow;_TB=& vds;_TC=(struct Cyc_List_List*)_TB;_TD=fenv;_TE=_TD->unknown_none;_TF=decl;_T10=_TF->loc;
inflow=Cyc_NewControlFlow_add_vars(_T9,_TA,_TC,_TE,_T10,0);_T11=vd;{
struct Cyc_Absyn_Exp*e=_T11->initializer;
if(e!=0)goto _TL2B9;_T12=inflow;
return _T12;_TL2B9: {
struct _tuple14 _T4A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e,0);void*_T4B;union Cyc_CfFlowInfo_FlowInfo _T4C;_T4C=_T4A.f0;_T4B=_T4A.f1;{union Cyc_CfFlowInfo_FlowInfo f=_T4C;void*r=_T4B;_T13=fenv;_T14=env;_T15=f;_T16=vd;_T17=e;_T18=r;_T19=decl;_T1A=_T19->loc;_T1B=
Cyc_NewControlFlow_do_initialize_var(_T13,_T14,_T15,_T16,_T17,_T18,_T1A);return _T1B;}}}}}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T44;_T48=_T4A->f1;}{struct Cyc_List_List*vds=_T48;_T1C=fenv;_T1D=inflow;_T1E=vds;_T1F=fenv;_T20=_T1F->unknown_none;_T21=decl;_T22=_T21->loc;_T23=
# 2665
Cyc_NewControlFlow_add_vars(_T1C,_T1D,_T1E,_T20,_T22,0);return _T23;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T44;_T47=_T4A->f1;}{struct Cyc_Absyn_Fndecl*fd=_T47;_T24=env;_T25=_T24->all_tables;_T26=fenv;_T27=inflow;_T28=fd;
# 2668
Cyc_NewControlFlow_check_nested_fun(_T25,_T26,_T27,_T28);_T29=fd;{
struct Cyc_Absyn_Vardecl*vd=_T29->fn_vardecl;_T2A=fenv;_T2B=inflow;{struct Cyc_List_List*_T4A=_cycalloc(sizeof(struct Cyc_List_List));
# 2673
_T4A->hd=_check_null(vd);_T4A->tl=0;_T2C=(struct Cyc_List_List*)_T4A;}_T2D=fenv;_T2E=_T2D->unknown_all;_T2F=decl;_T30=_T2F->loc;_T31=Cyc_NewControlFlow_add_vars(_T2A,_T2B,_T2C,_T2E,_T30,0);return _T31;}}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T44;_T46=_T4A->f1;_T49=_T4A->f2;_T45=_T4A->f3;}{struct Cyc_Absyn_Tvar*tv=_T46;struct Cyc_Absyn_Vardecl*vd=_T49;struct Cyc_Absyn_Exp*open_exp_opt=_T45;
# 2676
if(open_exp_opt==0)goto _TL2BB;_T33=Cyc_Warn_impos;{
int(*_T4A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T33;_T32=_T4A;}_T34=_tag_fat("found open expression in declaration!",sizeof(char),38U);_T35=_tag_fat(0U,sizeof(void*),0);_T32(_T34,_T35);goto _TL2BC;_TL2BB: _TL2BC:{struct Cyc_List_List _T4A;
_T4A.hd=vd;_T4A.tl=0;_T36=_T4A;}{struct Cyc_List_List vds=_T36;_T37=fenv;_T38=inflow;_T39=& vds;_T3A=(struct Cyc_List_List*)_T39;_T3B=fenv;_T3C=_T3B->unknown_all;_T3D=decl;_T3E=_T3D->loc;_T3F=
Cyc_NewControlFlow_add_vars(_T37,_T38,_T3A,_T3C,_T3E,0);return _T3F;}}default: _T41=Cyc_Warn_impos;{
# 2682
int(*_T4A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T41;_T40=_T4A;}_T42=_tag_fat("anal_decl: unexpected decl variant",sizeof(char),35U);_T43=_tag_fat(0U,sizeof(void*),0);_T40(_T42,_T43);};}}}
# 2690
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){struct _handler_cons*_T0;int*_T1;int _T2;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T3;struct Cyc_CfFlowInfo_FlowEnv*_T4;struct Cyc_CfFlowInfo_FlowEnv*_T5;struct Cyc_Dict_Dict _T6;union Cyc_CfFlowInfo_FlowInfo _T7;struct Cyc_Absyn_Fndecl*_T8;void*_T9;struct Cyc_Dict_Absent_exn_struct*_TA;char*_TB;char*_TC;struct Cyc_Dict_Absent_exn_struct*_TD;void*_TE;struct _handler_cons _TF;_T0=& _TF;_push_handler(_T0);{int _T10=0;_T1=_TF.handler;_T2=setjmp(_T1);if(!_T2)goto _TL2BD;_T10=1;goto _TL2BE;_TL2BD: _TL2BE: if(_T10)goto _TL2BF;else{goto _TL2C1;}_TL2C1:{
# 2692
struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();_T3=tables;_T4=fenv;_T5=fenv;_T6=_T5->mt_flowdict;_T7=
Cyc_CfFlowInfo_ReachableFL(_T6);_T8=fd;Cyc_NewControlFlow_check_nested_fun(_T3,_T4,_T7,_T8);}_pop_handler();goto _TL2C0;_TL2BF: _T9=Cyc_Core_get_exn_thrown();{void*_T11=(void*)_T9;void*_T12;_TA=(struct Cyc_Dict_Absent_exn_struct*)_T11;_TB=_TA->tag;_TC=Cyc_Dict_Absent;if(_TB!=_TC)goto _TL2C2;
# 2698
if(Cyc_Warn_num_errors <= 0)goto _TL2C4;goto _LL0;
# 2700
_TL2C4:{struct Cyc_Dict_Absent_exn_struct*_T13=_cycalloc(sizeof(struct Cyc_Dict_Absent_exn_struct));_T13->tag=Cyc_Dict_Absent;_TD=(struct Cyc_Dict_Absent_exn_struct*)_T13;}_TE=(void*)_TD;Cyc_Core_rethrow(_TE);goto _TL2C3;_TL2C2: _T12=_T11;{void*exn=_T12;_rethrow(exn);}_TL2C3: _LL0:;}_TL2C0:;}}
# 2704
static int Cyc_NewControlFlow_hash_ptr(void*s){void*_T0;int _T1;_T0=s;_T1=(int)_T0;
return _T1;}
# 2711
static struct Cyc_List_List*Cyc_NewControlFlow_get_noconsume_params(struct Cyc_List_List*aqb,struct Cyc_List_List*param_vardecls,struct Cyc_List_List*atts){struct _fat_ptr _T0;int _T1;int*_T2;struct _RegionHandle*_T3;unsigned _T4;int _T5;int*_T6;unsigned _T7;struct Cyc_List_List*_T8;int*_T9;unsigned _TA;struct _fat_ptr _TB;int _TC;char*_TD;int*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;void*_T11;struct Cyc_List_List*_T12;struct Cyc_Absyn_Vardecl*_T13;void*_T14;int _T15;struct _fat_ptr _T16;int _T17;char*_T18;int*_T19;int _T1A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T1B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T1C;struct Cyc_CfFlowInfo_Place*_T1D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T1E;struct Cyc_CfFlowInfo_Place*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;struct _RegionHandle _T22=_new_region(0U,"r");struct _RegionHandle*r=& _T22;_push_region(r);{
# 2715
int len=Cyc_List_length(param_vardecls);_T1=len;{unsigned _T23=(unsigned)_T1;_T3=r;_T4=_check_times(_T23,sizeof(int));{int*_T24=_region_malloc(_T3,0U,_T4);_T5=len;{unsigned _T25=(unsigned)_T5;unsigned i;i=0;_TL2C9: if(i < _T25)goto _TL2C7;else{goto _TL2C8;}_TL2C7: _T7=i;_T6=_T24 + _T7;
*_T6=0;i=i + 1;goto _TL2C9;_TL2C8:;}_T2=(int*)_T24;}_T0=_tag_fat(_T2,sizeof(int),_T23);}{struct _fat_ptr cons=_T0;
_TL2CD: if(atts!=0)goto _TL2CB;else{goto _TL2CC;}
_TL2CB: _T8=atts;{void*_T23=_T8->hd;int _T24;_T9=(int*)_T23;_TA=*_T9;switch(_TA){case 22:{struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_T25=(struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_T23;_T24=_T25->f1;}{int i=_T24;_T24=i;goto _LL4;}case 21:{struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_T25=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_T23;_T24=_T25->f1;}_LL4: {int i=_T24;_TB=cons;_TC=i - 1;_TD=_check_fat_subscript(_TB,sizeof(int),_TC);_TE=(int*)_TD;
# 2720
*_TE=1;goto _LL0;}default: goto _LL0;}_LL0:;}_TF=atts;
# 2717
atts=_TF->tl;goto _TL2CD;_TL2CC: {
# 2724
struct Cyc_List_List*noconsume_roots=0;{
int i=0;_TL2D2: if(param_vardecls!=0)goto _TL2D0;else{goto _TL2D1;}
_TL2D0: _T10=param_vardecls;_T11=_T10->hd;{struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)_T11;_T12=aqb;_T13=vd;_T14=_T13->type;_T15=
Cyc_Tcutil_is_noalias_pointer(_T12,_T14,0);if(!_T15)goto _TL2D3;_T16=cons;_T17=i;_T18=_check_fat_subscript(_T16,sizeof(int),_T17);_T19=(int*)_T18;_T1A=*_T19;if(_T1A)goto _TL2D3;else{goto _TL2D5;}
_TL2D5:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_T1B=root;_T1B->tag=0;_T1C=root;_T1C->f1=vd;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T1D=rp;_T1E=root;_T1D->root=(void*)_T1E;_T1F=rp;_T1F->path=0;{struct Cyc_List_List*_T23=_cycalloc(sizeof(struct Cyc_List_List));
_T23->hd=rp;_T23->tl=noconsume_roots;_T20=(struct Cyc_List_List*)_T23;}noconsume_roots=_T20;}}goto _TL2D4;_TL2D3: _TL2D4:;}_T21=param_vardecls;
# 2725
param_vardecls=_T21->tl;i=i + 1;goto _TL2D2;_TL2D1:;}{struct Cyc_List_List*_T23=noconsume_roots;_npop_handler(0);return _T23;}}}}_pop_region();}
# 2736
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Absyn_Fndecl*_T0;struct Cyc_Absyn_Stmt*_T1;struct Cyc_CfFlowInfo_FlowEnv*_T2;union Cyc_CfFlowInfo_FlowInfo _T3;struct Cyc_Absyn_Fndecl*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;void*_T7;struct Cyc_List_List*_T8;struct Cyc_CfFlowInfo_FlowEnv*_T9;void*_TA;unsigned _TB;struct Cyc_Dict_Dict _TC;struct _union_FlowInfo_ReachableFL _TD;unsigned _TE;struct _union_FlowInfo_ReachableFL _TF;struct Cyc_Absyn_Fndecl*_T10;void*_T11;void*_T12;int*_T13;int _T14;struct Cyc_Absyn_FnInfo _T15;int(*_T16)(struct _fat_ptr,struct _fat_ptr);void*(*_T17)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T18;struct _fat_ptr _T19;struct Cyc_List_List*_T1A;struct Cyc_Absyn_Fndecl*_T1B;struct Cyc_Core_Opt*_T1C;struct Cyc_Core_Opt*_T1D;void*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;int*_T22;unsigned _T23;struct Cyc_Absyn_Vardecl*(*_T24)(struct Cyc_List_List*,int);void*(*_T25)(struct Cyc_List_List*,int);struct Cyc_Absyn_Fndecl*_T26;struct Cyc_Core_Opt*_T27;void*_T28;struct Cyc_List_List*_T29;int _T2A;struct Cyc_Absyn_Vardecl*_T2B;void*_T2C;struct Cyc_CfFlowInfo_FlowEnv*_T2D;struct Cyc_List_List*_T2E;void*_T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_CfFlowInfo_FlowEnv*_T31;void*_T32;struct Cyc_CfFlowInfo_FlowEnv*_T33;void*_T34;void*_T35;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T36;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T37;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T38;struct Cyc_CfFlowInfo_Place*_T39;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T3A;struct Cyc_CfFlowInfo_Place*_T3B;struct Cyc_Dict_Dict _T3C;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T3D;void*_T3E;void*_T3F;struct Cyc_Dict_Dict _T40;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T41;void*_T42;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T43;void*_T44;struct Cyc_Absyn_Vardecl*(*_T45)(struct Cyc_List_List*,int);void*(*_T46)(struct Cyc_List_List*,int);struct Cyc_Absyn_Fndecl*_T47;struct Cyc_Core_Opt*_T48;struct Cyc_Core_Opt*_T49;void*_T4A;struct Cyc_List_List*_T4B;int _T4C;struct Cyc_Absyn_Vardecl*_T4D;void*_T4E;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T4F;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T50;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T51;struct Cyc_CfFlowInfo_Place*_T52;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T53;struct Cyc_CfFlowInfo_Place*_T54;struct Cyc_Dict_Dict _T55;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T56;void*_T57;struct Cyc_CfFlowInfo_FlowEnv*_T58;void*_T59;struct Cyc_CfFlowInfo_FlowEnv*_T5A;void*_T5B;void*_T5C;struct Cyc_Dict_Dict _T5D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T5E;void*_T5F;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T60;void*_T61;struct Cyc_List_List*_T62;struct Cyc_List_List*_T63;void*_T64;struct Cyc_Hashtable_Table*(*_T65)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*));struct Cyc_Hashtable_Table*(*_T66)(int,int(*)(void*,void*),int(*)(void*));int(*_T67)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*);int(*_T68)(void*,void*);int(*_T69)(struct Cyc_Absyn_Stmt*);struct Cyc_NewControlFlow_AnalEnv*_T6A;struct Cyc_NewControlFlow_AnalEnv*_T6B;struct Cyc_Hashtable_Table*(*_T6C)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*);void*(*_T6D)(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result*_T6E;struct Cyc_Hashtable_Table*_T6F;struct Cyc_Absyn_Fndecl*_T70;struct Cyc_NewControlFlow_AnalEnv*_T71;struct Cyc_Hashtable_Table*(*_T72)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*);void*(*_T73)(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result*_T74;struct Cyc_Hashtable_Table*_T75;struct Cyc_Absyn_Fndecl*_T76;struct Cyc_NewControlFlow_AnalEnv*_T77;struct Cyc_NewControlFlow_AnalEnv*_T78;struct Cyc_NewControlFlow_AnalEnv*_T79;struct Cyc_NewControlFlow_AnalEnv*_T7A;struct Cyc_NewControlFlow_AnalEnv*_T7B;struct Cyc_NewControlFlow_AnalEnv*_T7C;struct Cyc_NewControlFlow_AnalEnv*_T7D;struct Cyc_Absyn_Fndecl*_T7E;struct Cyc_Absyn_FnInfo _T7F;struct Cyc_NewControlFlow_AnalEnv*_T80;struct Cyc_NewControlFlow_AnalEnv*_T81;struct Cyc_NewControlFlow_AnalEnv*_T82;struct Cyc_NewControlFlow_AnalEnv*_T83;struct Cyc_NewControlFlow_AnalEnv*_T84;struct Cyc_NewControlFlow_AnalEnv*_T85;int _T86;int _T87;struct Cyc_NewControlFlow_AnalEnv*_T88;struct Cyc_NewControlFlow_AnalEnv*_T89;struct Cyc_NewControlFlow_AnalEnv*_T8A;union Cyc_CfFlowInfo_FlowInfo _T8B;struct Cyc_Absyn_Fndecl*_T8C;struct Cyc_Absyn_Stmt*_T8D;union Cyc_CfFlowInfo_FlowInfo _T8E;struct _union_FlowInfo_BottomFL _T8F;unsigned _T90;struct Cyc_NewControlFlow_AnalEnv*_T91;struct Cyc_NewControlFlow_AnalEnv*_T92;struct Cyc_List_List*_T93;union Cyc_CfFlowInfo_FlowInfo _T94;unsigned _T95;int _T96;unsigned _T97;struct _fat_ptr _T98;struct _fat_ptr _T99;struct Cyc_Absyn_Fndecl*_T9A;struct Cyc_Absyn_FnInfo _T9B;void*_T9C;int _T9D;struct Cyc_Absyn_Fndecl*_T9E;struct Cyc_Absyn_FnInfo _T9F;void*_TA0;int _TA1;struct Cyc_Absyn_Fndecl*_TA2;struct Cyc_Absyn_FnInfo _TA3;void*_TA4;int _TA5;unsigned _TA6;struct _fat_ptr _TA7;struct _fat_ptr _TA8;unsigned _TA9;struct _fat_ptr _TAA;struct _fat_ptr _TAB;_T0=fd;_T1=_T0->body;{
# 2740
unsigned loc=_T1->loc;_T2=fenv;_T3=inflow;_T4=fd;_T5=_T4->param_vardecls;_T6=
_check_null(_T5);_T7=_T6->v;_T8=(struct Cyc_List_List*)_T7;_T9=fenv;_TA=_T9->unknown_all;_TB=loc;inflow=Cyc_NewControlFlow_add_vars(_T2,_T3,_T8,_TA,_TB,1);{
# 2745
struct Cyc_List_List*param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _TAC=inflow;_TD=_TAC.ReachableFL;_TE=_TD.tag;if(_TE==2)goto _TL2D6;_throw_match();goto _TL2D7;_TL2D6: _TL2D7: _TF=_TAC.ReachableFL;_TC=_TF.val;}{
struct Cyc_Dict_Dict d=_TC;
# 2753
struct Cyc_List_List*aquals_bounds=Cyc_Tcutil_get_aquals_bounds(fd);
# 2756
struct Cyc_List_List*atts;_T10=fd;_T11=_T10->cached_type;_T12=
_check_null(_T11);{void*_TAC=Cyc_Absyn_compress(_T12);struct Cyc_List_List*_TAD;_T13=(int*)_TAC;_T14=*_T13;if(_T14!=6)goto _TL2D8;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_TAE=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TAC;_T15=_TAE->f1;_TAD=_T15.attributes;}{struct Cyc_List_List*as=_TAD;
atts=as;goto _LL0;}_TL2D8: _T17=Cyc_Warn_impos;{
int(*_TAE)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T17;_T16=_TAE;}_T18=_tag_fat("check_fun: non-function type cached with fndecl_t",sizeof(char),50U);_T19=_tag_fat(0U,sizeof(void*),0);_T16(_T18,_T19);_LL0:;}_T1A=aquals_bounds;_T1B=fd;_T1C=_T1B->param_vardecls;_T1D=
# 2761
_check_null(_T1C);_T1E=_T1D->v;_T1F=(struct Cyc_List_List*)_T1E;_T20=atts;{struct Cyc_List_List*noconsume_roots=Cyc_NewControlFlow_get_noconsume_params(_T1A,_T1F,_T20);
# 2764
_TL2DD: if(atts!=0)goto _TL2DB;else{goto _TL2DC;}
_TL2DB: _T21=atts;{void*_TAC=_T21->hd;int _TAD;_T22=(int*)_TAC;_T23=*_T22;switch(_T23){case 21:{struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_TAE=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_TAC;_TAD=_TAE->f1;}{int i=_TAD;
# 2767
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(-1,0U);_T25=Cyc_List_nth;{
struct Cyc_Absyn_Vardecl*(*_TAE)(struct Cyc_List_List*,int)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))_T25;_T24=_TAE;}_T26=fd;_T27=_T26->param_vardecls;_T28=_T27->v;_T29=(struct Cyc_List_List*)_T28;_T2A=i - 1;{struct Cyc_Absyn_Vardecl*vd=_T24(_T29,_T2A);_T2B=vd;_T2C=_T2B->type;{
void*t=Cyc_Absyn_compress(_T2C);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);_T2D=fenv;_T2E=aquals_bounds;_T2F=elttype;_T30=bogus_exp;_T31=fenv;_T32=elttype;_T33=fenv;_T34=_T33->unknown_all;_T35=
# 2773
Cyc_CfFlowInfo_typ_to_absrval(_T31,_T32,0,_T34);{
# 2771
void*rval=
Cyc_CfFlowInfo_make_unique_consumed(_T2D,_T2E,_T2F,_T30,-1,_T35,0);
# 2775
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r;r=_cycalloc(sizeof(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct));_T36=r;_T36->tag=2;_T37=r;_T37->f1=i;_T38=r;_T38->f2=t;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T39=rp;_T3A=r;_T39->root=(void*)_T3A;_T3B=rp;_T3B->path=0;_T3C=d;_T3D=r;_T3E=(void*)_T3D;_T3F=rval;
d=Cyc_Dict_insert(_T3C,_T3E,_T3F);_T40=d;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_TAE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_TAE->tag=0;
_TAE->f1=vd;_T41=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_TAE;}_T42=(void*)_T41;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_TAE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_TAE->tag=4;_TAE->f1=rp;_T43=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_TAE;}_T44=(void*)_T43;d=Cyc_Dict_insert(_T40,_T42,_T44);goto _LL5;}}}}}case 20:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_TAE=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_TAC;_TAD=_TAE->f1;}{int i=_TAD;_T46=Cyc_List_nth;{
# 2781
struct Cyc_Absyn_Vardecl*(*_TAE)(struct Cyc_List_List*,int)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))_T46;_T45=_TAE;}_T47=fd;_T48=_T47->param_vardecls;_T49=_check_null(_T48);_T4A=_T49->v;_T4B=(struct Cyc_List_List*)_T4A;_T4C=i - 1;{struct Cyc_Absyn_Vardecl*vd=_T45(_T4B,_T4C);_T4D=vd;_T4E=_T4D->type;{
void*elttype=Cyc_Tcutil_pointer_elt_type(_T4E);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r;r=_cycalloc(sizeof(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct));_T4F=r;_T4F->tag=2;_T50=r;_T50->f1=i;_T51=r;_T51->f2=elttype;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T52=rp;_T53=r;_T52->root=(void*)_T53;_T54=rp;_T54->path=0;_T55=d;_T56=r;_T57=(void*)_T56;_T58=fenv;_T59=elttype;_T5A=fenv;_T5B=_T5A->esc_none;_T5C=
Cyc_CfFlowInfo_typ_to_absrval(_T58,_T59,0,_T5B);d=Cyc_Dict_insert(_T55,_T57,_T5C);_T5D=d;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_TAE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_TAE->tag=0;
_TAE->f1=vd;_T5E=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_TAE;}_T5F=(void*)_T5E;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_TAE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_TAE->tag=4;_TAE->f1=rp;_T60=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_TAE;}_T61=(void*)_T60;d=Cyc_Dict_insert(_T5D,_T5F,_T61);{struct Cyc_List_List*_TAE=_cycalloc(sizeof(struct Cyc_List_List));
_TAE->hd=rp;_TAE->tl=param_roots;_T62=(struct Cyc_List_List*)_TAE;}param_roots=_T62;goto _LL5;}}}}default: goto _LL5;}_LL5:;}_T63=atts;
# 2764
atts=_T63->tl;goto _TL2DD;_TL2DC:
# 2792
 inflow=Cyc_CfFlowInfo_ReachableFL(d);_T64=
# 2794
Cyc_Tcutil_fndecl2type(fd);{int noreturn=Cyc_Atts_is_noreturn_fn_type(_T64);_T66=Cyc_Hashtable_create;{
# 2796
struct Cyc_Hashtable_Table*(*_TAC)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))_T66;_T65=_TAC;}_T68=Cyc_Core_ptrcmp;{int(*_TAC)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))_T68;_T67=_TAC;}{int(*_TAC)(struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*))Cyc_NewControlFlow_hash_ptr;_T69=_TAC;}{
# 2795
struct Cyc_Hashtable_Table*flow_table=
_T65(33,_T67,_T69);
struct Cyc_NewControlFlow_AnalEnv*env;env=_cycalloc(sizeof(struct Cyc_NewControlFlow_AnalEnv));_T6A=env;
_T6A->all_tables=tables;_T6B=env;_T6D=Cyc_Hashtable_lookup;{
struct Cyc_Hashtable_Table*(*_TAC)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))_T6D;_T6C=_TAC;}_T6E=tables;_T6F=_T6E->succ_tables;_T70=fd;_T6B->succ_table=_T6C(_T6F,_T70);_T71=env;_T73=Cyc_Hashtable_lookup;{
struct Cyc_Hashtable_Table*(*_TAC)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))_T73;_T72=_TAC;}_T74=tables;_T75=_T74->pat_pop_tables;_T76=fd;_T71->pat_pop_table=_T72(_T75,_T76);_T77=env;
_T77->fenv=fenv;_T78=env;_T78->iterate_again=1;_T79=env;_T79->iteration_num=0;_T7A=env;_T7A->in_try=0;_T7B=env;_T7B->tryflow=inflow;_T7C=env;_T7C->noreturn=noreturn;_T7D=env;_T7E=fd;_T7F=_T7E->i;_T7D->return_type=_T7F.ret_type;_T80=env;
_T80->unique_pat_vars=0;_T81=env;_T81->param_roots=param_roots;_T82=env;_T82->noconsume_params=noconsume_roots;_T83=env;
_T83->flow_table=flow_table;_T84=env;_T84->aquals_bounds=aquals_bounds;{
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
_TL2DF: _T85=env;_T86=_T85->iterate_again;if(_T86)goto _TL2E2;else{goto _TL2E1;}_TL2E2: _T87=Cyc_CfFlowInfo_anal_error;if(_T87)goto _TL2E1;else{goto _TL2E0;}
_TL2E0: _T88=env;_T88->iteration_num=_T88->iteration_num + 1;_T89=env;
# 2810
_T89->iterate_again=0;_T8A=env;_T8B=inflow;_T8C=fd;_T8D=_T8C->body;
outflow=Cyc_NewControlFlow_anal_stmt(_T8A,_T8B,_T8D,0);goto _TL2DF;_TL2E1: _T8E=outflow;_T8F=_T8E.BottomFL;_T90=_T8F.tag;if(_T90!=1)goto _TL2E3;goto _LLC;_TL2E3:
# 2816
 Cyc_NewControlFlow_check_init_params(loc,env,outflow);_T91=env;_T92=env;_T93=_T92->noconsume_params;_T94=outflow;_T95=loc;
Cyc_NewControlFlow_unconsume_params(_T91,_T93,0,0,_T94,_T95);_T96=noreturn;
# 2820
if(!_T96)goto _TL2E5;_T97=loc;_T98=
_tag_fat("`noreturn' function might (implicitly) return",sizeof(char),46U);_T99=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_T97,_T98,_T99);goto _TL2E6;
_TL2E5: _T9A=fd;_T9B=_T9A->i;_T9C=_T9B.ret_type;_T9D=Cyc_Tcutil_is_void_type(_T9C);if(_T9D)goto _TL2E7;else{goto _TL2E9;}
_TL2E9: _T9E=fd;_T9F=_T9E->i;_TA0=_T9F.ret_type;_TA1=Cyc_Tcutil_is_any_float_type(_TA0);if(_TA1)goto _TL2EC;else{goto _TL2ED;}_TL2ED: _TA2=fd;_TA3=_TA2->i;_TA4=_TA3.ret_type;_TA5=
Cyc_Tcutil_is_any_int_type(_TA4);
# 2823
if(_TA5)goto _TL2EC;else{goto _TL2EA;}
# 2825
_TL2EC: _TA6=loc;_TA7=_tag_fat("function may not return a value",sizeof(char),32U);_TA8=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TA6,_TA7,_TA8);goto _TL2EB;
# 2827
_TL2EA: _TA9=loc;_TAA=_tag_fat("function may not return a value",sizeof(char),32U);_TAB=_tag_fat(0U,sizeof(void*),0);Cyc_CfFlowInfo_aerr(_TA9,_TAA,_TAB);_TL2EB: goto _LLC;_TL2E7: _TL2E6: goto _LLC;_LLC:;}}}}}}}}
# 2833
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_List_List*_T5;
_TL2F1: if(ds!=0)goto _TL2EF;else{goto _TL2F0;}
_TL2EF: Cyc_CfFlowInfo_anal_error=0;_T0=ds;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{
void*_T6=_T2->r;struct Cyc_List_List*_T7;struct Cyc_Absyn_Fndecl*_T8;_T3=(int*)_T6;_T4=*_T3;switch(_T4){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T9=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T6;_T8=_T9->f1;}{struct Cyc_Absyn_Fndecl*fd=_T8;
Cyc_NewControlFlow_check_fun(tables,fd);goto _LL0;}case 11:{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T9=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T6;_T7=_T9->f1;}{struct Cyc_List_List*ds2=_T7;_T7=ds2;goto _LL6;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T9=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T6;_T7=_T9->f2;}_LL6: {struct Cyc_List_List*ds2=_T7;_T7=ds2;goto _LL8;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T9=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T6;_T7=_T9->f2;}_LL8: {struct Cyc_List_List*ds2=_T7;
# 2841
Cyc_NewControlFlow_cf_check(tables,ds2);goto _LL0;}case 12: goto _LL0;default: goto _LL0;}_LL0:;}_T5=ds;
# 2834
ds=_T5->tl;goto _TL2F1;_TL2F0:;}

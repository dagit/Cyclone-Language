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
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
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
# 1226
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1228
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1230
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
static int Cyc_NewControlFlow_strcmp(struct _fat_ptr s1,struct _fat_ptr s2){
if((char*)s1.curr==(char*)s2.curr)
return 0;{
int i=0;
unsigned sz1=_get_fat_size(s1,sizeof(char));
unsigned sz2=_get_fat_size(s2,sizeof(char));
unsigned minsz=sz1 < sz2?sz1: sz2;
# 59
while((unsigned)i < minsz){
{char c1=*((const char*)_check_fat_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_fat_subscript(s2,sizeof(char),i));
if((int)c1==0){
if((int)c2==0)return 0;else{
return -1;}}else{
if((int)c2==0)return 1;else{
# 67
int diff=(int)c1 - (int)c2;
if(diff!=0)return diff;}}
# 70
++ i;}
# 60
1U;}
# 72
if(sz1==sz2)return 0;
if(minsz < sz2){
if((int)*((const char*)_check_fat_subscript(s2,sizeof(char),i))==0)return 0;else{
return -1;}}else{
# 77
if((int)*((const char*)_check_fat_subscript(s1,sizeof(char),i))==0)return 0;else{
return 1;}}}}struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*aquals_bounds;};struct _tuple15{void*f0;int f1;};
# 123 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple15*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f0;union Cyc_CfFlowInfo_AbsLVal f1;};
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int,int,struct Cyc_Absyn_Exp*);
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*,int);struct _tuple17{union Cyc_CfFlowInfo_FlowInfo f0;union Cyc_CfFlowInfo_FlowInfo f1;};
static struct _tuple17 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
# 129
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_List_List*);
# 133
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->annot;void*_Tmp1;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_NewControlFlow_CFAnnot){_Tmp1=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->f1;{struct Cyc_NewControlFlow_CFStmtAnnot*x=(struct Cyc_NewControlFlow_CFStmtAnnot*)_Tmp1;
return x;}}else{
({void*_Tmp2=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct));_Tmp3->tag=Cyc_NewControlFlow_CFAnnot,_Tmp3->f1.visited=0;_Tmp3;});s->annot=_Tmp2;});return Cyc_NewControlFlow_get_stmt_annot(s);};}
# 140
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=({union Cyc_CfFlowInfo_FlowInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;_Tmp0;})(env->flow_table,s);
if(sflow==0){
union Cyc_CfFlowInfo_FlowInfo*res;res=_cycalloc(sizeof(union Cyc_CfFlowInfo_FlowInfo)),({union Cyc_CfFlowInfo_FlowInfo _Tmp0=Cyc_CfFlowInfo_BottomFL();*res=_Tmp0;});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,union Cyc_CfFlowInfo_FlowInfo*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,union Cyc_CfFlowInfo_FlowInfo*))Cyc_Hashtable_insert;_Tmp0;})(env->flow_table,s,res);
return res;}
# 147
return*sflow;}struct _tuple18{struct Cyc_NewControlFlow_CFStmtAnnot*f0;union Cyc_CfFlowInfo_FlowInfo*f1;};
# 151
static struct _tuple18 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*annot=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*sflow=Cyc_NewControlFlow_get_stmt_flow(env,s);
({union Cyc_CfFlowInfo_FlowInfo _Tmp0=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*sflow);*sflow=_Tmp0;});
# 160
annot->visited=env->iteration_num;{
struct _tuple18 _Tmp0;_Tmp0.f0=annot,_Tmp0.f1=sflow;return _Tmp0;}}
# 170
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 178
({union Cyc_CfFlowInfo_FlowInfo _Tmp0=Cyc_CfFlowInfo_join_tryflow(env->fenv,new_flow,env->tryflow);env->tryflow=_Tmp0;});}struct _tuple19{struct Cyc_NewControlFlow_AnalEnv*f0;unsigned f1;struct Cyc_Dict_Dict f2;};
# 185
static void Cyc_NewControlFlow_check_unique_root(struct _tuple19*ckenv,void*root,void*rval){
# 187
struct Cyc_Dict_Dict _Tmp0;unsigned _Tmp1;void*_Tmp2;_Tmp2=ckenv->f0;_Tmp1=ckenv->f1;_Tmp0=ckenv->f2;{struct Cyc_NewControlFlow_AnalEnv*env=_Tmp2;unsigned loc=_Tmp1;struct Cyc_Dict_Dict new_fd=_Tmp0;
void*_Tmp3;if(*((int*)root)==0){_Tmp3=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)root)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 190
if(!Cyc_Dict_lookup_bool(new_fd,root,& rval)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,vd->type)){
retry: {void*_Tmp4;switch(*((int*)rval)){case 8: _Tmp4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rval)->f2;{void*r=_Tmp4;
rval=r;goto retry;}case 7:
 goto _LLE;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)rval)->f1==Cyc_CfFlowInfo_NoneIL){_LLE:
 goto _LL10;}else{goto _LL11;}case 0: _LL10:
 goto _LL8;default: _LL11:
# 199
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_qvar2string(vd->name);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn(loc,_tag_fat("alias-free pointer(s) reachable from %s may become unreachable.",sizeof(char),64U),_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL8;}_LL8:;}}
# 203
goto _LL3;}}else{
goto _LL3;}_LL3:;}}
# 211
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*annot=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*sflow=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo new_flow=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*sflow);
# 217
if(Cyc_Flags_warn_lose_unique){
struct _tuple17 _Tmp0=({struct _tuple17 _Tmp1;_Tmp1.f0=flow,_Tmp1.f1=new_flow;_Tmp1;});struct Cyc_Dict_Dict _Tmp1;struct Cyc_Dict_Dict _Tmp2;if(_Tmp0.f0.ReachableFL.tag==2){if(_Tmp0.f1.ReachableFL.tag==2){_Tmp2=_Tmp0.f0.ReachableFL.val;_Tmp1=_Tmp0.f1.ReachableFL.val;{struct Cyc_Dict_Dict fd=_Tmp2;struct Cyc_Dict_Dict new_fd=_Tmp1;
# 220
struct _tuple19 ckenv=({struct _tuple19 _Tmp3;_Tmp3.f0=env,_Tmp3.f1=s->loc,_Tmp3.f2=new_fd;_Tmp3;});
({void(*_Tmp3)(void(*)(struct _tuple19*,void*,void*),struct _tuple19*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple19*,void*,void*),struct _tuple19*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp3;})(Cyc_NewControlFlow_check_unique_root,& ckenv,fd);
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 225
if(!Cyc_CfFlowInfo_flow_lessthan_approx(new_flow,*sflow)){
# 232
*sflow=new_flow;
# 236
if(annot->visited==env->iteration_num)
# 238
env->iterate_again=1;}}
# 243
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned loc,int nameit){
# 247
struct Cyc_Dict_Dict _Tmp0;if(inflow.BottomFL.tag==1)
return Cyc_CfFlowInfo_BottomFL();else{_Tmp0=inflow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp0;
# 250
for(1;vds!=0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct)),root->tag=0,root->f1=(struct Cyc_Absyn_Vardecl*)vds->hd;{
void*rval=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp1->tag=8,_Tmp1->f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_Tmp1->f2=rval;_Tmp1;});
# 257
d=Cyc_Dict_insert(d,(void*)root,rval);}}
# 259
return Cyc_CfFlowInfo_ReachableFL(d);}};}
# 263
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==14){_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return e1;}}else{
return e;};}
# 270
static void Cyc_NewControlFlow_check_union_requires(unsigned loc,void*t,struct _fat_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 272
if(inflow.BottomFL.tag==1)
return;else{
# 275
return;};}
# 279
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
struct Cyc_Dict_Dict _Tmp0;if(inflow.BottomFL.tag==1)
return Cyc_CfFlowInfo_BottomFL();else{_Tmp0=inflow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp0;
# 283
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r)!=1)
Cyc_CfFlowInfo_aerr(loc,_tag_fat("expression may not be fully initialized",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);
if(d.t==ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo ans=Cyc_CfFlowInfo_ReachableFL(ans_d);
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}};}struct _tuple20{struct Cyc_Absyn_Tqual f0;void*f1;};
# 293
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,void*r){
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=r;_Tmp1;});enum Cyc_Absyn_AggrKind _Tmp1;union Cyc_Absyn_AggrInfo _Tmp2;struct _fat_ptr _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0.f1)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1==Cyc_CfFlowInfo_NoneIL)
return;else{switch(*((int*)_Tmp0.f0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.tag==2)goto _LL11;else{goto _LL11;}case 24: goto _LL11;default: goto _LL11;}case 7: goto _LL11;case 4: goto _LLF;default: goto _LL11;}}case 0:
 return;case 7:
 return;case 8: _Tmp6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{void*r=_Tmp6;
Cyc_NewControlFlow_check_nounique(env,loc,t,r);return;}default: switch(*((int*)_Tmp0.f0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.tag==2){if(*((int*)_Tmp0.f1)==6){_Tmp6=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f0;_Tmp5=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp6;struct Cyc_Absyn_Datatypefield*tuf=_Tmp5;struct Cyc_List_List*targs=_Tmp4;struct _fat_ptr ad=_Tmp3;
# 300
if(tuf->typs==0)
return;{
struct _RegionHandle _Tmp7=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp7;_push_region(temp);
{struct Cyc_List_List*inst=Cyc_List_rzip(temp,temp,tud->tvs,targs);
struct Cyc_List_List*fs=tuf->typs;
{int i=0;for(0;(unsigned)i < _get_fat_size(ad,sizeof(void*));(i ++,fs=fs->tl)){
struct _tuple20*_Tmp8=(struct _tuple20*)_check_null(fs)->hd;void*_Tmp9;_Tmp9=_Tmp8->f1;{void*t=_Tmp9;
if(inst!=0)t=Cyc_Tcutil_rsubstitute(temp,inst,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)ad.curr)[i]);}}}
# 310
_npop_handler(0);return;}
# 303
;_pop_region();}}}else{goto _LL11;}}else{goto _LL11;}case 24: if(*((int*)_Tmp0.f1)==6){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1;_Tmp6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp2;struct Cyc_List_List*targs=_Tmp6;struct _fat_ptr d=_Tmp3;
# 314
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl==0)return;{
struct Cyc_List_List*fs=ad->impl->fields;
struct _RegionHandle _Tmp7=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp7;_push_region(temp);
{struct Cyc_List_List*inst=Cyc_List_rzip(temp,temp,ad->tvs,targs);
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)_check_null(fs)->hd)->type;
if(inst!=0)t=Cyc_Tcutil_rsubstitute(temp,inst,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)d.curr)[i]);}}
# 324
_npop_handler(0);return;}
# 318
;_pop_region();}}}else{goto _LL11;}default: goto _LL11;}case 7: if(*((int*)_Tmp0.f1)==6){_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f0)->f3;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp6;struct _fat_ptr d=_Tmp3;
# 327
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)_check_null(fs)->hd)->type,((void**)d.curr)[i]);}}
return;}}else{goto _LL11;}case 4: _LLF:
# 331
 if(Cyc_Tcutil_is_noalias_pointer(env->aquals_bounds,t,0))
Cyc_Warn_warn(loc,_tag_fat("argument may still contain alias-free pointers",sizeof(char),47U),_tag_fat(0U,sizeof(void*),0));
return;default: _LL11:
 return;}};}
# 338
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
struct Cyc_Dict_Dict _Tmp0;if(inflow.BottomFL.tag==1)
return Cyc_CfFlowInfo_BottomFL();else{_Tmp0=inflow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp0;
# 342
if(!Cyc_Tcutil_is_noalias_pointer(env->aquals_bounds,t,0))
({int(*_Tmp1)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp1;})(_tag_fat("noliveunique attribute requires alias-free pointer",sizeof(char),51U),_tag_fat(0U,sizeof(void*),0));{
void*elt_type=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_Tmp1;switch(*((int*)r)){case 8: _Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f2;{void*r2=_Tmp1;
r=r2;goto retry;}case 4: _Tmp1=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)r)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp1;
_Tmp1=Cyc_CfFlowInfo_lookup_place(d,p);goto _LLB;}case 5: _Tmp1=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)r)->f1;_LLB: {void*r=_Tmp1;
Cyc_NewControlFlow_check_nounique(env,loc,elt_type,r);goto _LL5;}default:
# 350
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,elt_type))
Cyc_Warn_warn(loc,_tag_fat("argument may contain live alias-free pointers",sizeof(char),46U),_tag_fat(0U,sizeof(void*),0));
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 354
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);
if(d.t==ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo ans=Cyc_CfFlowInfo_ReachableFL(ans_d);
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}}};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f0;struct Cyc_List_List*f1;};
# 364
static struct _tuple21 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 370
struct Cyc_List_List*rvals=0;
if(es==0){
struct _tuple21 _Tmp0;_Tmp0.f0=inflow,_Tmp0.f1=0;return _Tmp0;}
do{
{struct _tuple14 _Tmp0=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd,0);void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp2;void*r=_Tmp1;
inflow=f;
rvals=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=r,_Tmp3->tl=rvals;_Tmp3;});
es=es->tl;
first_is_copy=others_are_copy;}}
# 374
1U;}while(es!=0);
# 381
Cyc_NewControlFlow_update_tryflow(env,inflow);{
struct _tuple21 _Tmp0;_Tmp0.f0=inflow,({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(rvals);_Tmp0.f1=_Tmp1;});return _Tmp0;}}
# 387
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 389
struct _tuple21 _Tmp0=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 389
void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{union Cyc_CfFlowInfo_FlowInfo afterflow=_Tmp2;struct Cyc_List_List*rvals=_Tmp1;
# 391
{struct Cyc_Dict_Dict _Tmp3;if(afterflow.ReachableFL.tag==2){_Tmp3=afterflow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp3;
# 393
for(1;rvals!=0;(rvals=rvals->tl,es=_check_null(es)->tl)){
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,(void*)rvals->hd)==0)
Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)_check_null(es)->hd)->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));}
goto _LL3;}}else{
goto _LL3;}_LL3:;}{
# 399
struct _tuple14 _Tmp3;_Tmp3.f0=afterflow,_Tmp3.f1=env->fenv->unknown_all;return _Tmp3;}}}
# 406
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 413
int needs_unconsume=0;
void*old_rval=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume))
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("consume_zero_rval",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));else{
# 418
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,env->aquals_bounds,_check_null(e->topt),e,env->iteration_num,new_rval,0);else{
# 422
return new_rval;}}}
# 435 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 440
struct Cyc_Dict_Dict _Tmp0;if(outflow.BottomFL.tag==1)
return outflow;else{_Tmp0=outflow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp0;
# 443
union Cyc_CfFlowInfo_AbsLVal _Tmp1=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e).f1;void*_Tmp2;if(_Tmp1.UnknownL.tag==2)
# 447
return outflow;else{_Tmp2=_Tmp1.PlaceL.val;{struct Cyc_CfFlowInfo_Place*p=_Tmp2;
# 451
void*nzval=(int)il==1?env->fenv->notzeroall: env->fenv->unknown_none;
for(1;names!=0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp3->tag=8,_Tmp3->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp3->f2=nzval;_Tmp3;});}
# 455
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval));
# 460
return outflow;}}};}};}
# 469
static struct _tuple17 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 471
struct Cyc_Dict_Dict _Tmp0;if(outflow.BottomFL.tag==1){
struct _tuple17 _Tmp1;_Tmp1.f0=outflow,_Tmp1.f1=outflow;return _Tmp1;}else{_Tmp0=outflow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp0;
# 474
union Cyc_CfFlowInfo_AbsLVal _Tmp1=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e).f1;void*_Tmp2;if(_Tmp1.UnknownL.tag==2){
struct _tuple17 _Tmp3;_Tmp3.f0=outflow,_Tmp3.f1=outflow;return _Tmp3;}else{_Tmp2=_Tmp1.PlaceL.val;{struct Cyc_CfFlowInfo_Place*p=_Tmp2;
# 477
void*nzval=(int)il==1?env->fenv->notzeroall: env->fenv->unknown_none;
void*zval=env->fenv->zero;
for(1;names!=0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp3->tag=8,_Tmp3->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp3->f2=nzval;_Tmp3;});
zval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp3->tag=8,_Tmp3->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp3->f2=zval;_Tmp3;});}
# 483
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,zval);{
struct _tuple17 _Tmp3;({
union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval));_Tmp3.f0=_Tmp4;}),({
union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,zval));_Tmp3.f1=_Tmp4;});return _Tmp3;}}};}};}
# 492
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ};
# 501
static struct _tuple14 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_Absyn_Exp*index){
# 505
void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==4){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;{void*elttype=_Tmp2;void*bd=_Tmp1;
# 507
struct Cyc_Dict_Dict _Tmp3;if(f.BottomFL.tag==1){
# 509
struct _tuple14 _Tmp4;_Tmp4.f0=f,({void*_Tmp5=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,env->fenv->unknown_all);_Tmp4.f1=_Tmp5;});return _Tmp4;}else{_Tmp3=f.ReachableFL.val;{struct Cyc_Dict_Dict outdict=_Tmp3;
# 511
struct _tuple13 _Tmp4=Cyc_CfFlowInfo_unname_rval(r);void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{void*r=_Tmp6;struct Cyc_List_List*names=_Tmp5;
{enum Cyc_CfFlowInfo_InitLevel _Tmp7;void*_Tmp8;void*_Tmp9;switch(*((int*)r)){case 8: _Tmp9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f1;_Tmp8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp9;void*r2=_Tmp8;
# 514
({int(*_TmpA)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpA;})(_tag_fat("named location in anal_derefR",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));}case 1:
# 516
 e->annot=(void*)& Cyc_CfFlowInfo_NotZero_val;
goto _LLD;case 4: _Tmp9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)r)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp9;
# 519
e->annot=(void*)& Cyc_CfFlowInfo_NotZero_val;
if(index==0 && Cyc_Tcutil_is_bound_one(bd,0)){
struct _tuple14 _TmpA;_TmpA.f0=f,({void*_TmpB=Cyc_CfFlowInfo_lookup_place(outdict,p);_TmpA.f1=_TmpB;});return _TmpA;}
goto _LLD;}case 5: _Tmp9=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)r)->f1;{void*r=_Tmp9;
# 525
e->annot=(void*)& Cyc_CfFlowInfo_UnknownZ_val;
if(index==0 && Cyc_Tcutil_is_bound_one(bd,0)){
struct _tuple14 _TmpA;_TmpA.f0=f,_TmpA.f1=r;return _TmpA;}
goto _LLD;}case 0:
# 530
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _TmpA;({union Cyc_CfFlowInfo_FlowInfo _TmpB=Cyc_CfFlowInfo_BottomFL();_TmpA.f0=_TmpB;}),({void*_TmpB=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,env->fenv->unknown_all);_TmpA.f1=_TmpB;});return _TmpA;}case 2: _Tmp7=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)r)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp7;
# 534
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
goto _LL1B;}default: _LL1B:
# 537
 e->annot=(void*)& Cyc_CfFlowInfo_UnknownZ_val;
goto _LLD;}_LLD:;}{
# 540
enum Cyc_CfFlowInfo_InitLevel _Tmp7=Cyc_CfFlowInfo_initlevel(env->fenv,outdict,r);if(_Tmp7==Cyc_CfFlowInfo_NoneIL){
# 542
struct _tuple13 _Tmp8=Cyc_CfFlowInfo_unname_rval(r);void*_Tmp9;_Tmp9=_Tmp8.f0;{void*r=_Tmp9;
if(*((int*)r)==7){
# 545
Cyc_CfFlowInfo_aerr(e->loc,_tag_fat("attempt to dereference a consumed alias-free pointer",sizeof(char),53U),_tag_fat(0U,sizeof(void*),0));
goto _LL24;}else{
# 548
Cyc_CfFlowInfo_aerr(e->loc,_tag_fat("dereference of possibly uninitialized pointer",sizeof(char),46U),_tag_fat(0U,sizeof(void*),0));}_LL24:;
# 550
goto _LL20;}}else{_LL20: {
# 552
struct _tuple14 _Tmp8;_Tmp8.f0=f,({void*_Tmp9=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,env->fenv->unknown_all);_Tmp8.f1=_Tmp9;});return _Tmp8;}};}}}};}}else{
# 555
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("right deref of non-pointer-type",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));};}
# 565
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned loc,void*old_rval){
# 570
struct _tuple16 _Tmp0=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _Tmp1;_Tmp1=_Tmp0.f1;{union Cyc_CfFlowInfo_AbsLVal lval=_Tmp1;
{struct _tuple16 _Tmp2=({struct _tuple16 _Tmp3;_Tmp3.f0=inflow,_Tmp3.f1=lval;_Tmp3;});int _Tmp3;void*_Tmp4;struct Cyc_Dict_Dict _Tmp5;if(_Tmp2.f0.ReachableFL.tag==2){if(_Tmp2.f1.PlaceL.tag==1){_Tmp5=_Tmp2.f0.ReachableFL.val;_Tmp4=_Tmp2.f1.PlaceL.val;{struct Cyc_Dict_Dict fd=_Tmp5;struct Cyc_CfFlowInfo_Place*p=_Tmp4;
# 573
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_check_null(exp->topt),0,env->fenv->unknown_all);
# 575
struct _tuple13 _Tmp6=Cyc_CfFlowInfo_unname_rval(old_rval);void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{void*old_rval=_Tmp8;struct Cyc_List_List*names=_Tmp7;
for(1;names!=0;names=names->tl){
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp9->tag=8,_Tmp9->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_Tmp9->f2=new_rval;_Tmp9;});}
# 580
fd=Cyc_CfFlowInfo_assign_place(env->fenv,loc,fd,p,new_rval);
inflow=Cyc_CfFlowInfo_ReachableFL(fd);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 586
goto _LL3;}}}else{_Tmp3=_Tmp2.f1.UnknownL.val;{int u=_Tmp3;
# 591
goto _LL3;}}}else{
# 587
goto _LL3;}_LL3:;}
# 596
return inflow;}}
# 601
static struct _tuple14 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 608
struct Cyc_Dict_Dict _Tmp0;if(outflow.BottomFL.tag==1){
# 610
struct _tuple14 _Tmp1;({union Cyc_CfFlowInfo_FlowInfo _Tmp2=Cyc_CfFlowInfo_BottomFL();_Tmp1.f0=_Tmp2;}),_Tmp1.f1=rval;return _Tmp1;}else{_Tmp0=outflow.ReachableFL.val;{struct Cyc_Dict_Dict indict=_Tmp0;
# 612
void*_Tmp1;if(lval.PlaceL.tag==1){_Tmp1=lval.PlaceL.val;{struct Cyc_CfFlowInfo_Place*p=_Tmp1;
# 620
struct Cyc_Dict_Dict outdict=Cyc_CfFlowInfo_assign_place(fenv,loc,indict,p,rval);
outflow=Cyc_CfFlowInfo_ReachableFL(outdict);
if(Cyc_Flags_warn_lose_unique &&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,lexp->topt)){
# 625
struct _tuple13 _Tmp2=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(indict,p));void*_Tmp3;_Tmp3=_Tmp2.f0;{void*rv=_Tmp3;
switch(*((int*)rv)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)rv)->f1==Cyc_CfFlowInfo_NoneIL)
goto _LL11;else{goto _LL14;}case 7: _LL11:
 goto _LL13;case 0: _LL13:
 goto _LLD;default: _LL14:
# 631
 Cyc_Warn_warn(lexp->loc,_tag_fat("assignment may overwrite alias-free pointer(s)",sizeof(char),47U),_tag_fat(0U,sizeof(void*),0));
goto _LLD;}_LLD:;}}
# 636
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple14 _Tmp2;_Tmp2.f0=outflow,_Tmp2.f1=rval;return _Tmp2;}}}else{
# 642
struct _tuple14 _Tmp2;({union Cyc_CfFlowInfo_FlowInfo _Tmp3=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_Tmp2.f0=_Tmp3;}),_Tmp2.f1=rval;return _Tmp2;};}};}
# 649
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 655
struct Cyc_Dict_Dict _Tmp0;if(f.BottomFL.tag==1)
return Cyc_CfFlowInfo_BottomFL();else{_Tmp0=f.ReachableFL.val;{struct Cyc_Dict_Dict outdict=_Tmp0;
# 660
outdict=({struct Cyc_CfFlowInfo_FlowEnv*_Tmp1=fenv;unsigned _Tmp2=loc;struct Cyc_Dict_Dict _Tmp3=outdict;struct Cyc_CfFlowInfo_Place*_Tmp4=({struct Cyc_CfFlowInfo_Place*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));
({void*_Tmp6=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_Tmp7->tag=0,_Tmp7->f1=vd;_Tmp7;});_Tmp5->root=_Tmp6;}),_Tmp5->path=0;_Tmp5;});
# 660
Cyc_CfFlowInfo_assign_place(_Tmp1,_Tmp2,_Tmp3,_Tmp4,rval);});{
# 663
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_ReachableFL(outdict);
Cyc_NewControlFlow_update_tryflow(env,outflow);
# 666
return outflow;}}};}struct _tuple22{struct Cyc_Absyn_Vardecl**f0;struct Cyc_Absyn_Exp*f1;};
# 670
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned pat_loc,unsigned exp_loc,int isAliasPat){
# 677
if(vds==0)return inflow;{
# 680
struct Cyc_List_List*vs=Cyc_Tcutil_filter_nulls(Cyc_List_split(vds).f0);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x!=0;x=x->tl){
if((*((struct _tuple22*)x->hd)).f0==0)es=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=_check_null((*((struct _tuple22*)x->hd)).f1),_Tmp0->tl=es;_Tmp0;});}}
# 686
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,vs,fenv->unknown_all,pat_loc,name_locs);
# 688
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x!=0;x=x->tl){
# 692
struct _tuple14 _Tmp0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd,0);void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp2;void*r=_Tmp1;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,f,r);}}}{
# 700
struct Cyc_List_List*_Tmp0=Cyc_List_rev(vds);struct Cyc_List_List*vds=_Tmp0;
for(1;vds!=0;vds=vds->tl){
struct _tuple22*_Tmp1=(struct _tuple22*)vds->hd;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_Absyn_Vardecl**vd=_Tmp3;struct Cyc_Absyn_Exp*ve=_Tmp2;
if(vd!=0 && ve!=0){
if(ve->topt==0)
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(ve);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({
# 705
int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("oops! pattern init expr %s has no type!\n",sizeof(char),41U),_tag_fat(_Tmp5,sizeof(void*),1));});{
# 714
struct Cyc_List_List l=({struct Cyc_List_List _Tmp4;_Tmp4.hd=ve,_Tmp4.tl=0;_Tmp4;});
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple16 _Tmp4=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,ve);union Cyc_CfFlowInfo_AbsLVal _Tmp5;_Tmp5=_Tmp4.f1;{union Cyc_CfFlowInfo_AbsLVal lval=_Tmp5;
struct _tuple14 _Tmp6=Cyc_NewControlFlow_anal_Rexp(env,1,f,ve,isAliasPat);void*_Tmp7;union Cyc_CfFlowInfo_FlowInfo _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp8;void*rval=_Tmp7;
struct Cyc_Dict_Dict _Tmp9;if(f.ReachableFL.tag==2){_Tmp9=f.ReachableFL.val;{struct Cyc_Dict_Dict fd=_Tmp9;
# 720
if(name_locs){
void*_TmpA;if(lval.PlaceL.tag==1){_TmpA=lval.PlaceL.val;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 723
rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_TmpB->tag=8,_TmpB->f1=*vd,_TmpB->f2=rval;_TmpB;});{
# 726
void*new_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_TmpB->tag=8,_TmpB->f1=*vd,_TmpB->f2=new_rval;_TmpB;});
fd=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd);
goto _LL11;}}}else{
# 733
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,_check_null(ve->topt))&& !
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,(*vd)->type))
({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({
struct _fat_ptr _TmpD=Cyc_Absynpp_exp2string(ve);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_CfFlowInfo_aerr(exp_loc,_tag_fat("aliased pattern expression not an l-value: %s",sizeof(char),46U),_tag_fat(_TmpC,sizeof(void*),1));});}_LL11:;}
# 743
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,f,*vd,ve,rval,pat_loc);
goto _LLC;}}else{
# 746
goto _LLC;}_LLC:;}}}}}}
# 751
return inflow;}}}
# 754
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 756
void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 4:
 goto _LL4;case 3: _LL4:
 goto _LL6;case 5: _LL6:
 return 1;default: goto _LL11;}case 20: _Tmp1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
_Tmp1=e;goto _LLA;}case 21: _Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_Tmp1;
_Tmp1=e;goto _LLC;}case 22: _Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_Tmp1;
# 763
return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);}case 23: _Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 765
return 0;}case 14: _Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 767
if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);else{
return 0;}}default: _LL11:
 return 0;};}struct _tuple23{union Cyc_CfFlowInfo_AbsLVal f0;union Cyc_CfFlowInfo_FlowInfo f1;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 774
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,int local_alias){
# 778
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
struct Cyc_Dict_Dict d;
# 790
{struct Cyc_Dict_Dict _Tmp0;if(inflow.BottomFL.tag==1){
struct _tuple14 _Tmp1;({union Cyc_CfFlowInfo_FlowInfo _Tmp2=Cyc_CfFlowInfo_BottomFL();_Tmp1.f0=_Tmp2;}),_Tmp1.f1=fenv->unknown_all;return _Tmp1;}else{_Tmp0=inflow.ReachableFL.val;{struct Cyc_Dict_Dict d2=_Tmp0;
d=d2;}};}
# 804 "new_control_flow.cyc"
if((copy_ctxt && Cyc_NewControlFlow_is_local_var_rooted_path(e,0))&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,e->topt)){
# 825 "new_control_flow.cyc"
struct _tuple16 _Tmp0=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_AbsLVal _Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{union Cyc_CfFlowInfo_FlowInfo flval=_Tmp2;union Cyc_CfFlowInfo_AbsLVal lval=_Tmp1;
struct _tuple16 _Tmp3=({struct _tuple16 _Tmp4;_Tmp4.f0=flval,_Tmp4.f1=lval;_Tmp4;});void*_Tmp4;struct Cyc_Dict_Dict _Tmp5;if(_Tmp3.f0.ReachableFL.tag==2){if(_Tmp3.f1.PlaceL.tag==1){_Tmp5=_Tmp3.f0.ReachableFL.val;_Tmp4=_Tmp3.f1.PlaceL.val;{struct Cyc_Dict_Dict fd=_Tmp5;struct Cyc_CfFlowInfo_Place*p=_Tmp4;
# 828
void*old_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume)){
Cyc_CfFlowInfo_aerr(e->loc,_tag_fat("expression attempts to copy a consumed alias-free value",sizeof(char),56U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple14 _Tmp6;({union Cyc_CfFlowInfo_FlowInfo _Tmp7=Cyc_CfFlowInfo_BottomFL();_Tmp6.f0=_Tmp7;}),_Tmp6.f1=fenv->unknown_all;return _Tmp6;}}else{
# 834
if(needs_unconsume){
# 836
struct _tuple14 _Tmp6;_Tmp6.f0=flval,({void*_Tmp7=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,old_rval);_Tmp6.f1=_Tmp7;});return _Tmp6;}else{
# 839
void*new_rval=Cyc_CfFlowInfo_make_unique_consumed(fenv,env->aquals_bounds,e->topt,e,env->iteration_num,old_rval,local_alias);
struct Cyc_Dict_Dict outdict=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,new_rval);
# 851
struct _tuple14 _Tmp6;({union Cyc_CfFlowInfo_FlowInfo _Tmp7=Cyc_CfFlowInfo_ReachableFL(outdict);_Tmp6.f0=_Tmp7;}),_Tmp6.f1=old_rval;return _Tmp6;}}}}else{goto _LLB;}}else{_LLB:
# 855
 goto _LL8;}_LL8:;}}{
# 858
void*_Tmp0=e->r;enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;void*_Tmp3;enum Cyc_Absyn_MallocKind _Tmp4;enum Cyc_Absyn_Primop _Tmp5;void*_Tmp6;int _Tmp7;struct _fat_ptr _Tmp8;void*_Tmp9;switch(*((int*)_Tmp0)){case 14: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4==Cyc_Absyn_Null_to_NonNull){_Tmp9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 862
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_TmpC;void*r1=_TmpB;
struct _tuple14 _TmpD=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_TmpF;void*r2=_TmpE;
struct _tuple14 _Tmp10;_Tmp10.f0=f2,_Tmp10.f1=r1;return _Tmp10;}}}}else{_Tmp9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 868
_Tmp9=e1;goto _LL13;}}case 12: _Tmp9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_Tmp9;
_Tmp9=e1;goto _LL15;}case 40: _Tmp9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL15: {struct Cyc_Absyn_Exp*e1=_Tmp9;
_Tmp9=e1;goto _LL17;}case 13: _Tmp9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_Tmp9;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);}case 2: _Tmp8=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr p=_Tmp8;
# 874
if(!Cyc_NewControlFlow_strcmp(p,_tag_fat("print_flow",sizeof(char),11U))){
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=seg_str;_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_fprintf(Cyc_stderr,_tag_fat("%s: current flow is\n",sizeof(char),21U),_tag_fat(_TmpB,sizeof(void*),1));});
Cyc_CfFlowInfo_print_flow(inflow);
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}else{
# 880
if(!Cyc_NewControlFlow_strcmp(p,_tag_fat("debug_on",sizeof(char),9U)));else{
# 885
if(!Cyc_NewControlFlow_strcmp(p,_tag_fat("debug_off",sizeof(char),10U)));}}{
# 893
struct _tuple14 _TmpA;_TmpA.f0=inflow,_TmpA.f1=fenv->zero;return _TmpA;}}case 39:
# 895
 goto _LL1D;case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wstring_c.tag){case 1: _LL1D:
 goto _LL1F;case 5: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f1==0){_LL1F: {
struct _tuple14 _TmpA;_TmpA.f0=inflow,_TmpA.f1=fenv->zero;return _TmpA;}}else{
goto _LL23;}case 8: _LL23:
 goto _LL25;case 9: _LL25:
 goto _LL27;default: _LL2D:
# 909
 goto _LL2F;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 2: _LL27: {
# 901
struct _tuple14 _TmpA;_TmpA.f0=inflow,_TmpA.f1=fenv->notzeroall;return _TmpA;}case 1:  {
# 918
struct _tuple14 _TmpA;_TmpA.f0=inflow,({void*_TmpB=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_TmpA.f1=_TmpB;});return _TmpA;}case 4: _Tmp9=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp9;
# 923
if((int)vd->sc==0){
struct _tuple14 _TmpA;_TmpA.f0=inflow,({void*_TmpB=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_TmpA.f1=_TmpB;});return _TmpA;}
# 926
_Tmp9=vd;goto _LL3F;}case 3: _Tmp9=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL3F: {struct Cyc_Absyn_Vardecl*vd=_Tmp9;
_Tmp9=vd;goto _LL41;}case 5: _Tmp9=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL41: {struct Cyc_Absyn_Vardecl*vd=_Tmp9;
# 930
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _TmpA;_TmpA.tag=0,_TmpA.f1=vd;_TmpA;});
struct _tuple14 _TmpA;_TmpA.f0=inflow,({void*_TmpB=({struct Cyc_Dict_Dict _TmpC=d;Cyc_Dict_lookup_other(_TmpC,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_TmpD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));*_TmpD=vdroot;_TmpD;}));});_TmpA.f1=_TmpB;});return _TmpA;}default:
# 1530
 goto _LL7B;}case 41: _Tmp9=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;int static_only=_Tmp7;
# 904
if(static_only)goto _LL2B;{
struct _tuple17 _TmpA=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpC;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpB;
struct _tuple14 _TmpD;_TmpD.f0=f1t,_TmpD.f1=fenv->notzeroall;return _TmpD;}}}case 30: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==0){_LL2B:
# 908
 goto _LL2D;}else{_Tmp9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp9;struct Cyc_Absyn_Datatypedecl*tud=_Tmp6;
# 1399
struct _tuple21 _TmpA=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;struct Cyc_List_List*rvals=_TmpB;
unsigned sz=(unsigned)(Cyc_List_length(es)+ 1);
struct _fat_ptr aggrdict=({unsigned _TmpD=sz;_tag_fat(({void**_TmpE=_cycalloc(_check_times(_TmpD,sizeof(void*)));({{unsigned _TmpF=sz;unsigned i;for(i=0;i < _TmpF;++ i){_TmpE[i]=fenv->unknown_none;}}0;});_TmpE;}),sizeof(void*),_TmpD);});
*((void**)_check_fat_subscript(aggrdict,sizeof(void*),0))=fenv->unknown_all;
{unsigned i=1U;for(0;i < sz;(i ++,rvals=rvals->tl)){
((void**)aggrdict.curr)[(int)i]=(void*)_check_null(rvals)->hd;}}{
struct _tuple14 _TmpD;_TmpD.f0=f,({void*_TmpE=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_TmpF=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_TmpF->tag=6,_TmpF->f1.is_union=0,_TmpF->f1.fieldnum=-1,_TmpF->f2=aggrdict;_TmpF;});_TmpD.f1=_TmpE;});return _TmpD;}}}}case 42: _LL2F:
# 910
 goto _LL31;case 18: _LL31:
 goto _LL33;case 17: _LL33:
 goto _LL35;case 19: _LL35:
 goto _LL37;case 32: _LL37:
 goto _LL39;case 31: _LL39: {
struct _tuple14 _TmpA;_TmpA.f0=inflow,_TmpA.f1=fenv->unknown_all;return _TmpA;}case 3: _Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp5;struct Cyc_List_List*es=_Tmp9;
# 937
if((int)p==19){
# 940
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)_check_null(es)->hd,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;void*r=_TmpB;
struct _tuple14 _TmpD=Cyc_NewControlFlow_anal_derefR(env,inflow,f,(struct Cyc_Absyn_Exp*)es->hd,r,0);void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_TmpF;void*r2=_TmpE;
f2=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)es->hd)->loc,f2,r);{
struct _tuple14 _Tmp10;_Tmp10.f0=f2,_Tmp10.f1=env->fenv->unknown_all;return _Tmp10;}}}}else{
# 945
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_use_ints(env,inflow,es);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;void*r=_TmpB;
struct _tuple14 _TmpD;_TmpD.f0=f,_TmpD.f1=r;return _TmpD;}}}case 5: _Tmp9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 949
struct Cyc_List_List arg=({struct Cyc_List_List _TmpA;_TmpA.hd=e1,_TmpA.tl=0;_TmpA;});
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_use_ints(env,inflow,& arg);union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_TmpA.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;
struct _tuple16 _TmpC=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,e1);union Cyc_CfFlowInfo_AbsLVal _TmpD;_TmpD=_TmpC.f1;{union Cyc_CfFlowInfo_AbsLVal lval=_TmpD;
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type(_check_null(e1->topt));
if(iszt){
# 956
struct _tuple14 _TmpE=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_TmpF;union Cyc_CfFlowInfo_FlowInfo _Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{union Cyc_CfFlowInfo_FlowInfo g=_Tmp10;void*r=_TmpF;
Cyc_NewControlFlow_anal_derefR(env,inflow,g,e1,r,0);}}
# 959
{struct _tuple23 _TmpE=({struct _tuple23 _TmpF;_TmpF.f0=lval,_TmpF.f1=f;_TmpF;});struct Cyc_Dict_Dict _TmpF;void*_Tmp10;if(_TmpE.f0.PlaceL.tag==1){if(_TmpE.f1.ReachableFL.tag==2){_Tmp10=_TmpE.f0.PlaceL.val;_TmpF=_TmpE.f1.ReachableFL.val;{struct Cyc_CfFlowInfo_Place*p=_Tmp10;struct Cyc_Dict_Dict outdict=_TmpF;
# 961
f=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,p,fenv->unknown_all));
# 964
goto _LL9B;}}else{goto _LL9E;}}else{_LL9E:
 goto _LL9B;}_LL9B:;}
# 968
if(iszt){
struct _tuple14 _TmpE;_TmpE.f0=f,_TmpE.f1=fenv->notzeroall;return _TmpE;}else{
struct _tuple14 _TmpE;_TmpE.f0=f,_TmpE.f1=fenv->unknown_all;return _TmpE;}}}}case 4: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){_Tmp9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*l=_Tmp9;struct Cyc_Absyn_Exp*r=_Tmp6;
# 973
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,_check_null(e->topt))){
Cyc_CfFlowInfo_aerr(e->loc,_tag_fat("cannot track alias-free pointers through multiple assignments",sizeof(char),62U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple14 _TmpA;({union Cyc_CfFlowInfo_FlowInfo _TmpB=Cyc_CfFlowInfo_BottomFL();_TmpA.f0=_TmpB;}),_TmpA.f1=fenv->unknown_all;return _TmpA;}}{
# 977
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,r,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo right_out=_TmpC;void*rval=_TmpB;
struct _tuple16 _TmpD=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,l);union Cyc_CfFlowInfo_AbsLVal _TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpF;union Cyc_CfFlowInfo_AbsLVal lval=_TmpE;
{struct Cyc_Dict_Dict _Tmp10;if(f.ReachableFL.tag==2){_Tmp10=f.ReachableFL.val;{struct Cyc_Dict_Dict outdict=_Tmp10;
# 981
{void*_Tmp11;if(lval.PlaceL.tag==1){_Tmp11=lval.PlaceL.val;{struct Cyc_CfFlowInfo_Place*p=_Tmp11;
# 985
outdict=Cyc_CfFlowInfo_assign_place(fenv,l->loc,outdict,p,fenv->unknown_all);
# 987
f=Cyc_CfFlowInfo_ReachableFL(outdict);
# 991
goto _LLAB;}}else{
# 994
goto _LLAB;}_LLAB:;}
# 996
goto _LLA6;}}else{
goto _LLA6;}_LLA6:;}{
# 999
struct _tuple14 _Tmp10;_Tmp10.f0=f,_Tmp10.f1=fenv->unknown_all;return _Tmp10;}}}}}}else{_Tmp9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1002
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,_check_null(e->topt))){
Cyc_CfFlowInfo_aerr(e->loc,_tag_fat("cannot track alias-free pointers through multiple assignments",sizeof(char),62U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple14 _TmpA;({union Cyc_CfFlowInfo_FlowInfo _TmpB=Cyc_CfFlowInfo_BottomFL();_TmpA.f0=_TmpB;}),_TmpA.f1=fenv->unknown_all;return _TmpA;}}
# 1008
inflow=({struct Cyc_NewControlFlow_AnalEnv*_TmpA=env;union Cyc_CfFlowInfo_FlowInfo _TmpB=inflow;Cyc_NewControlFlow_expand_unique_places(_TmpA,_TmpB,({struct Cyc_Absyn_Exp*_TmpC[2];_TmpC[0]=e1,_TmpC[1]=e2;Cyc_List_list(_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),2));}));});{
# 1010
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo right_out=_TmpC;void*rval=_TmpB;
struct _tuple16 _TmpD=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,e1);union Cyc_CfFlowInfo_AbsLVal _TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo outflow=_TmpF;union Cyc_CfFlowInfo_AbsLVal lval=_TmpE;
struct _tuple14 _Tmp10=Cyc_NewControlFlow_do_assign(fenv,env,outflow,e1,lval,e2,rval,e->loc);void*_Tmp11;union Cyc_CfFlowInfo_FlowInfo _Tmp12;_Tmp12=_Tmp10.f0;_Tmp11=_Tmp10.f1;{union Cyc_CfFlowInfo_FlowInfo outflow=_Tmp12;void*rval=_Tmp11;
# 1016
struct _tuple14 _Tmp13;_Tmp13.f0=outflow,_Tmp13.f1=rval;return _Tmp13;}}}}}}case 9: _Tmp9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1019
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;void*r=_TmpB;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f,e2,local_alias);}}case 11: _Tmp9=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 1023
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;void*r=_TmpB;
Cyc_NewControlFlow_use_Rval(env,e1->loc,f,r);{
struct _tuple14 _TmpD;({union Cyc_CfFlowInfo_FlowInfo _TmpE=Cyc_CfFlowInfo_BottomFL();_TmpD.f0=_TmpE;}),({void*_TmpE=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_TmpD.f1=_TmpE;});return _TmpD;}}}case 10: _Tmp9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_List_List*es=_Tmp6;
# 1028
struct Cyc_List_List*orig_es=es;
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_TmpC;void*r1=_TmpB;
# 1031
Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);{
# 1034
struct _tuple21 _TmpD=({struct Cyc_NewControlFlow_AnalEnv*_TmpE=env;union Cyc_CfFlowInfo_FlowInfo _TmpF=f1;Cyc_NewControlFlow_anal_Rexps(_TmpE,_TmpF,Cyc_List_copy(es),1,1);});void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo fst_outflow=_TmpF;struct Cyc_List_List*rvals=_TmpE;
# 1036
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_NewControlFlow_use_Rval(env,e1->loc,fst_outflow,r1);
# 1038
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*consume_params=0;
{void*_Tmp10=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp11;if(*((int*)_Tmp10)==4){_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp10)->f1.elt_type;{void*t=_Tmp11;
# 1043
{void*_Tmp12=Cyc_Absyn_compress(t);void*_Tmp13;if(*((int*)_Tmp12)==6){_Tmp13=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp12)->f1.attributes;{struct Cyc_List_List*atts=_Tmp13;
# 1045
for(1;atts!=0;atts=atts->tl){
# 1047
void*_Tmp14=(void*)atts->hd;int _Tmp15;switch(*((int*)_Tmp14)){case 20: _Tmp15=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_Tmp14)->f1;{int i=_Tmp15;
# 1049
init_params=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));_Tmp16->hd=(void*)i,_Tmp16->tl=init_params;_Tmp16;});goto _LLCF;}case 21: _Tmp15=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_Tmp14)->f1;{int i=_Tmp15;
# 1051
nolive_unique_params=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));_Tmp16->hd=(void*)i,_Tmp16->tl=nolive_unique_params;_Tmp16;});
consume_params=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));_Tmp16->hd=(void*)i,_Tmp16->tl=consume_params;_Tmp16;});
goto _LLCF;}case 22: _Tmp15=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_Tmp14)->f1;{int i=_Tmp15;
# 1055
consume_params=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));_Tmp16->hd=(void*)i,_Tmp16->tl=consume_params;_Tmp16;});
goto _LLCF;}default:
 goto _LLCF;}_LLCF:;}
# 1059
goto _LLCA;}}else{
({int(*_Tmp14)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp14;})(_tag_fat("anal_Rexp: bad function type",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}_LLCA:;}
# 1062
goto _LLC5;}}else{
({int(*_Tmp12)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp12;})(_tag_fat("anal_Rexp: bad function type",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}_LLC5:;}
# 1065
{int i=1;for(0;rvals!=0;(rvals=rvals->tl,es=es->tl,++ i)){
if(({int(*_Tmp10)(struct Cyc_List_List*,int)=(int(*)(struct Cyc_List_List*,int))Cyc_List_memq;_Tmp10;})(init_params,i)){
struct Cyc_Dict_Dict _Tmp10;if(fst_outflow.BottomFL.tag==1)
goto _LLD8;else{_Tmp10=fst_outflow.ReachableFL.val;{struct Cyc_Dict_Dict fst_d=_Tmp10;
# 1070
struct _tuple13 _Tmp11=Cyc_CfFlowInfo_unname_rval((void*)rvals->hd);void*_Tmp12;_Tmp12=_Tmp11.f0;{void*r=_Tmp12;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,fst_d,(void*)rvals->hd)==0 && !Cyc_CfFlowInfo_is_init_pointer((void*)rvals->hd))
Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
{struct Cyc_Dict_Dict _Tmp13;if(outflow.BottomFL.tag==1)
goto _LLE0;else{_Tmp13=outflow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp13;
# 1078
struct Cyc_Dict_Dict ans_d=Cyc_CfFlowInfo_escape_deref(fenv,d,(void*)rvals->hd);
{void*_Tmp14=(void*)rvals->hd;void*_Tmp15;switch(*((int*)_Tmp14)){case 4: _Tmp15=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp14)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp15;
# 1081
{void*_Tmp16=Cyc_Absyn_compress(((struct Cyc_Absyn_Exp*)es->hd)->topt);void*_Tmp17;if(*((int*)_Tmp16)==4){_Tmp17=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp16)->f1.elt_type;{void*t=_Tmp17;
# 1083
ans_d=({struct Cyc_CfFlowInfo_FlowEnv*_Tmp18=fenv;unsigned _Tmp19=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct Cyc_Dict_Dict _Tmp1A=ans_d;struct Cyc_CfFlowInfo_Place*_Tmp1B=p;Cyc_CfFlowInfo_assign_place(_Tmp18,_Tmp19,_Tmp1A,_Tmp1B,
Cyc_CfFlowInfo_typ_to_absrval(fenv,t,0,fenv->esc_all));});
# 1087
goto _LLEC;}}else{
({int(*_Tmp18)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp18;})(_tag_fat("anal_Rexp:bad type for initialized arg",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));}_LLEC:;}
# 1090
goto _LLE5;}case 5:
# 1092
({int(*_Tmp16)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp16;})(
_tag_fat("anal_Rexp:initialize attribute on unique pointers not yet supported",sizeof(char),68U),_tag_fat(0U,sizeof(void*),0));default:
 goto _LLE5;}_LLE5:;}
# 1096
outflow=Cyc_CfFlowInfo_ReachableFL(ans_d);
goto _LLE0;}}_LLE0:;}
# 1099
goto _LLD8;}}}_LLD8:;}else{
# 1102
if(({int(*_Tmp10)(struct Cyc_List_List*,int)=(int(*)(struct Cyc_List_List*,int))Cyc_List_memq;_Tmp10;})(nolive_unique_params,i))
# 1107
outflow=({struct Cyc_NewControlFlow_AnalEnv*_Tmp10=env;unsigned _Tmp11=((struct Cyc_Absyn_Exp*)_check_null(es)->hd)->loc;void*_Tmp12=_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _Tmp13=outflow;Cyc_NewControlFlow_use_nounique_Rval(_Tmp10,_Tmp11,_Tmp12,_Tmp13,(void*)rvals->hd);});else{
# 1117
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_check_null(es)->hd)->loc,outflow,(void*)rvals->hd);
if(!({int(*_Tmp10)(struct Cyc_List_List*,int)=(int(*)(struct Cyc_List_List*,int))Cyc_List_memq;_Tmp10;})(consume_params,i)&&
 Cyc_Tcutil_is_noalias_pointer(env->aquals_bounds,_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),0))
outflow=Cyc_NewControlFlow_restore_noconsume_arg(env,outflow,(struct Cyc_Absyn_Exp*)es->hd,((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)rvals->hd);}}}}
# 1127
if(Cyc_Atts_is_noreturn_fn_type(_check_null(e1->topt))){
struct _tuple14 _Tmp10;({union Cyc_CfFlowInfo_FlowInfo _Tmp11=Cyc_CfFlowInfo_BottomFL();_Tmp10.f0=_Tmp11;}),({void*_Tmp11=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_Tmp10.f1=_Tmp11;});return _Tmp10;}else{
# 1130
struct _tuple14 _Tmp10;_Tmp10.f0=outflow,({void*_Tmp11=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_Tmp10.f1=_Tmp11;});return _Tmp10;}}}}}case 33: _Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_Tmp9=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp6=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp3=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp7=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.fat_result;{enum Cyc_Absyn_MallocKind mknd=_Tmp4;struct Cyc_Absyn_Exp*ropt=_Tmp9;struct Cyc_Absyn_Exp*aqopt=_Tmp6;void**topt=_Tmp3;struct Cyc_Absyn_Exp*exp=_Tmp2;int isfat=_Tmp7;
# 1133
void*place_val;
if(isfat)place_val=fenv->notzeroall;else{
if((int)mknd==1)place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*_check_null(topt),0,fenv->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*_check_null(topt),0,fenv->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(ropt==0 && aqopt==0){
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_TmpA.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;
outflow=f;}}else{
# 1143
struct Cyc_List_List*expl;expl=_cycalloc(sizeof(struct Cyc_List_List)),expl->hd=exp,expl->tl=0;
if((unsigned)aqopt)
expl=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));_TmpA->hd=aqopt,_TmpA->tl=expl;_TmpA;});
# 1147
if((unsigned)ropt)
expl=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));_TmpA->hd=ropt,_TmpA->tl=expl;_TmpA;});{
# 1150
struct _tuple21 _TmpA=Cyc_NewControlFlow_anal_Rexps(env,inflow,expl,1,1);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;struct Cyc_List_List*rvals=_TmpB;
outflow=f;}}}
# 1155
if(Cyc_Tcutil_is_noalias_pointer(env->aquals_bounds,_check_null(e->topt),1)){
struct _tuple14 _TmpA;_TmpA.f0=outflow,({void*_TmpB=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TmpC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_TmpC->tag=5,_TmpC->f1=place_val;_TmpC;});_TmpA.f1=_TmpB;});return _TmpA;}else{
# 1159
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_TmpA->tag=1,_TmpA->f1=exp,_TmpA->f2=_check_null(e->topt);_TmpA;});
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),place->root=root,place->path=0;{
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_TmpA->tag=4,_TmpA->f1=place;_TmpA;});
struct Cyc_Dict_Dict _TmpA;if(outflow.BottomFL.tag==1){
struct _tuple14 _TmpB;_TmpB.f0=outflow,_TmpB.f1=rval;return _TmpB;}else{_TmpA=outflow.ReachableFL.val;{struct Cyc_Dict_Dict d2=_TmpA;
# 1165
struct _tuple14 _TmpB;({union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_CfFlowInfo_ReachableFL(Cyc_Dict_insert(d2,root,place_val));_TmpB.f0=_TmpC;}),_TmpB.f1=rval;return _TmpB;}};}}}}case 34: _Tmp9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1170
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1176
struct _tuple21 _TmpA=({struct Cyc_NewControlFlow_AnalEnv*_TmpB=env;union Cyc_CfFlowInfo_FlowInfo _TmpC=inflow;Cyc_NewControlFlow_anal_Rexps(_TmpB,_TmpC,({struct Cyc_Absyn_Exp*_TmpD[2];_TmpD[0]=e1,_TmpD[1]=e2;Cyc_List_list(_tag_fat(_TmpD,sizeof(struct Cyc_Absyn_Exp*),2));}),0,0);});void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;struct Cyc_List_List*rvals=_TmpB;
left_rval=(void*)_check_null(rvals)->hd;
right_rval=(void*)_check_null(rvals->tl)->hd;
outflow=f;{
# 1182
void*t_ign1=Cyc_Absyn_void_type;void*t_ign2=Cyc_Absyn_void_type;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
if(*((int*)right_rval)==0)
goto _LLFF;else{
Cyc_CfFlowInfo_aerr(e1->loc,
_tag_fat("cannot swap value into zeroterm array not known to be 0",sizeof(char),56U),_tag_fat(0U,sizeof(void*),0));}_LLFF:;}else{
# 1193
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
if(*((int*)left_rval)==0)
goto _LL104;else{
Cyc_CfFlowInfo_aerr(e2->loc,
_tag_fat("cannot swap value into zeroterm array not known to be 0",sizeof(char),56U),_tag_fat(0U,sizeof(void*),0));}_LL104:;}}{
# 1201
struct _tuple16 _TmpD=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e1);union Cyc_CfFlowInfo_AbsLVal _TmpE;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_AbsLVal left_lval=_TmpE;
struct _tuple16 _TmpF=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e2);union Cyc_CfFlowInfo_AbsLVal _Tmp10;_Tmp10=_TmpF.f1;{union Cyc_CfFlowInfo_AbsLVal right_lval=_Tmp10;
{struct Cyc_Dict_Dict _Tmp11;if(outflow.ReachableFL.tag==2){_Tmp11=outflow.ReachableFL.val;{struct Cyc_Dict_Dict outdict=_Tmp11;
# 1205
{void*_Tmp12;if(left_lval.PlaceL.tag==1){_Tmp12=left_lval.PlaceL.val;{struct Cyc_CfFlowInfo_Place*lp=_Tmp12;
# 1207
outdict=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,lp,right_rval);
goto _LL114;}}else{
# 1213
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,right_rval)!=1)
Cyc_CfFlowInfo_aerr(e2->loc,_tag_fat("expression may not be fully initialized",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
goto _LL114;}_LL114:;}
# 1217
{void*_Tmp12;if(right_lval.PlaceL.tag==1){_Tmp12=right_lval.PlaceL.val;{struct Cyc_CfFlowInfo_Place*rp=_Tmp12;
# 1219
outdict=Cyc_CfFlowInfo_assign_place(fenv,e2->loc,outdict,rp,left_rval);
goto _LL119;}}else{
# 1222
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,left_rval)!=1)
Cyc_CfFlowInfo_aerr(e1->loc,_tag_fat("expression may not be fully initialized",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
goto _LL119;}_LL119:;}
# 1228
outflow=Cyc_CfFlowInfo_ReachableFL(outdict);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL10F;}}else{
goto _LL10F;}_LL10F:;}{
# 1235
struct _tuple14 _Tmp11;_Tmp11.f0=outflow,_Tmp11.f1=fenv->unknown_all;return _Tmp11;}}}}}}}case 16: _Tmp9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*eopt=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;struct Cyc_Absyn_Exp*qopt=_Tmp3;
# 1238
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(eopt==0 && qopt==0){
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;void*r=_TmpB;
outflow=f;
place_val=r;}}else{
# 1246
struct Cyc_List_List*expl;expl=_cycalloc(sizeof(struct Cyc_List_List)),expl->hd=e2,expl->tl=0;{
int depth=0;
if((unsigned)qopt){
expl=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));_TmpA->hd=qopt,_TmpA->tl=expl;_TmpA;});
++ depth;}
# 1252
if((unsigned)eopt){
expl=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));_TmpA->hd=eopt,_TmpA->tl=expl;_TmpA;});
++ depth;}{
# 1256
struct _tuple21 _TmpA=Cyc_NewControlFlow_anal_Rexps(env,inflow,expl,1,1);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;struct Cyc_List_List*rvals=_TmpB;
outflow=f;
place_val=depth==1?(void*)_check_null(_check_null(rvals)->tl)->hd:(void*)_check_null(_check_null(_check_null(rvals)->tl)->tl)->hd;}}}}
# 1261
if(Cyc_Tcutil_is_noalias_pointer(env->aquals_bounds,_check_null(e->topt),1)){
struct _tuple14 _TmpA;_TmpA.f0=outflow,({void*_TmpB=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TmpC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_TmpC->tag=5,_TmpC->f1=place_val;_TmpC;});_TmpA.f1=_TmpB;});return _TmpA;}else{
# 1265
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_TmpA->tag=1,_TmpA->f1=e2,_TmpA->f2=_check_null(e->topt);_TmpA;});
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),place->root=root,place->path=0;{
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_TmpA->tag=4,_TmpA->f1=place;_TmpA;});
struct Cyc_Dict_Dict _TmpA;if(outflow.BottomFL.tag==1){
struct _tuple14 _TmpB;_TmpB.f0=outflow,_TmpB.f1=rval;return _TmpB;}else{_TmpA=outflow.ReachableFL.val;{struct Cyc_Dict_Dict d2=_TmpA;
# 1271
struct _tuple14 _TmpB;({union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_CfFlowInfo_ReachableFL(Cyc_Dict_insert(d2,root,place_val));_TmpB.f0=_TmpC;}),_TmpB.f1=rval;return _TmpB;}};}}}case 15: _Tmp9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 1276
struct _tuple16 _TmpA=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e1);union Cyc_CfFlowInfo_AbsLVal _TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;union Cyc_CfFlowInfo_AbsLVal l=_TmpB;
void*_TmpD;if(l.UnknownL.tag==2){
struct _tuple14 _TmpE;_TmpE.f0=f,_TmpE.f1=fenv->notzeroall;return _TmpE;}else{_TmpD=l.PlaceL.val;{struct Cyc_CfFlowInfo_Place*p=_TmpD;
struct _tuple14 _TmpE;_TmpE.f0=f,({void*_TmpF=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp10->tag=4,_Tmp10->f1=p;_Tmp10;});_TmpE.f1=_TmpF;});return _TmpE;}};}}case 20: _Tmp9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 1283
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;void*r=_TmpB;
return Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,r,0);}}case 21: _Tmp9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*field=_Tmp6;
# 1287
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;void*r=_TmpB;
void*e1_type=_check_null(e1->topt);
# 1290
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type)){
struct _tuple14 _TmpD;_TmpD.f0=f,({void*_TmpE=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_TmpD.f1=_TmpE;});return _TmpD;}
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,field,f);{
struct _tuple13 _TmpD=Cyc_CfFlowInfo_unname_rval(r);void*_TmpE;_TmpE=_TmpD.f0;{void*r=_TmpE;
struct _fat_ptr _TmpF;int _Tmp10;int _Tmp11;if(*((int*)r)==6){_Tmp11=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1.is_union;_Tmp10=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1.fieldnum;_TmpF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f2;{int is_union=_Tmp11;int fnum=_Tmp10;struct _fat_ptr rdict=_TmpF;
# 1297
int field_no=Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),field);
if((is_union && fnum!=-1)&& fnum!=field_no){
struct _tuple14 _Tmp12;_Tmp12.f0=f,({void*_Tmp13=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),1,fenv->unknown_none);_Tmp12.f1=_Tmp13;});return _Tmp12;}{
struct _tuple14 _Tmp12;_Tmp12.f0=f,_Tmp12.f1=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));return _Tmp12;}}}else{
# 1302
Cyc_fprintf(Cyc_stderr,_tag_fat("the bad rexp is :",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));
Cyc_CfFlowInfo_print_absrval(r);
({struct Cyc_String_pa_PrintArg_struct _Tmp12=({struct Cyc_String_pa_PrintArg_struct _Tmp13;_Tmp13.tag=0,({struct _fat_ptr _Tmp14=Cyc_Absynpp_exp2string(e);_Tmp13.f1=_Tmp14;});_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;({int(*_Tmp14)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp14;})(_tag_fat("\nanal_Rexp: AggrMember: %s",sizeof(char),27U),_tag_fat(_Tmp13,sizeof(void*),1));});};}}}}case 37: _Tmp9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*field=_Tmp6;
# 1310
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;void*r=_TmpB;
# 1312
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_check_null(e1->topt))){
struct _tuple14 _TmpD;_TmpD.f0=f,_TmpD.f1=fenv->unknown_all;return _TmpD;}{
struct _tuple13 _TmpD=Cyc_CfFlowInfo_unname_rval(r);void*_TmpE;_TmpE=_TmpD.f0;{void*r=_TmpE;
struct _fat_ptr _TmpF;int _Tmp10;int _Tmp11;if(*((int*)r)==6){_Tmp11=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1.is_union;_Tmp10=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1.fieldnum;_TmpF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f2;{int is_union=_Tmp11;int fnum=_Tmp10;struct _fat_ptr rdict=_TmpF;
# 1317
int field_no=Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),field);
if(is_union && fnum!=-1){
if(fnum!=field_no){
struct _tuple14 _Tmp12;_Tmp12.f0=f,_Tmp12.f1=fenv->zero;return _Tmp12;}else{
# 1322
struct _tuple14 _Tmp12;_Tmp12.f0=f,_Tmp12.f1=fenv->notzeroall;return _Tmp12;}}else{
# 1324
struct _tuple14 _Tmp12;_Tmp12.f0=f,_Tmp12.f1=fenv->unknown_all;return _Tmp12;}}}else{
# 1326
({struct Cyc_String_pa_PrintArg_struct _Tmp12=({struct Cyc_String_pa_PrintArg_struct _Tmp13;_Tmp13.tag=0,({struct _fat_ptr _Tmp14=Cyc_Absynpp_exp2string(e);_Tmp13.f1=_Tmp14;});_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;({int(*_Tmp14)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp14;})(_tag_fat("anal_Rexp: TagCheck_e: %s",sizeof(char),26U),_tag_fat(_Tmp13,sizeof(void*),1));});};}}}}case 22: _Tmp9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*field=_Tmp6;
# 1330
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_TmpC;void*r1=_TmpB;
# 1333
struct _tuple14 _TmpD=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_TmpF;void*r2=_TmpE;
# 1336
void*_Tmp10=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp11;if(*((int*)_Tmp10)==4){_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp10)->f1.elt_type;{void*t2=_Tmp11;
# 1338
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t2)){
# 1340
struct _tuple14 _Tmp12;_Tmp12.f0=f2,({void*_Tmp13=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->unknown_all);_Tmp12.f1=_Tmp13;});return _Tmp12;}
# 1342
if(Cyc_Absyn_is_require_union_type(t2))
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,field,f1);{
# 1345
struct _tuple13 _Tmp12=Cyc_CfFlowInfo_unname_rval(r2);void*_Tmp13;_Tmp13=_Tmp12.f0;{void*r2=_Tmp13;
struct _fat_ptr _Tmp14;int _Tmp15;int _Tmp16;if(*((int*)r2)==6){_Tmp16=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r2)->f1.is_union;_Tmp15=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r2)->f1.fieldnum;_Tmp14=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r2)->f2;{int is_union=_Tmp16;int fnum=_Tmp15;struct _fat_ptr rdict=_Tmp14;
# 1348
int field_no=Cyc_CfFlowInfo_get_field_index(t2,field);
if((is_union && fnum!=-1)&& fnum!=field_no){
struct _tuple14 _Tmp17;_Tmp17.f0=f2,({void*_Tmp18=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),1,fenv->unknown_none);_Tmp17.f1=_Tmp18;});return _Tmp17;}{
struct _tuple14 _Tmp17;_Tmp17.f0=f2,_Tmp17.f1=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));return _Tmp17;}}}else{
({int(*_Tmp17)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp17;})(_tag_fat("anal_Rexp: AggrArrow",sizeof(char),21U),_tag_fat(0U,sizeof(void*),0));};}}}}else{
# 1354
({int(*_Tmp12)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp12;})(_tag_fat("anal_Rexp: AggrArrow ptr",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));};}}}case 6: _Tmp9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;struct Cyc_Absyn_Exp*e3=_Tmp3;
# 1358
struct _tuple17 _TmpA=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpC;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpB;
struct _tuple14 pr2=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);
struct _tuple14 pr3=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e3,local_alias);
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}case 7: _Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1364
struct _tuple17 _TmpA=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpC;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpB;
struct _tuple14 _TmpD=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpF;void*f2r=_TmpE;
struct _tuple14 pr2=({struct _tuple14 _Tmp10;_Tmp10.f0=f2t,_Tmp10.f1=f2r;_Tmp10;});
struct _tuple14 pr3=({struct _tuple14 _Tmp10;_Tmp10.f0=f1f,_Tmp10.f1=fenv->zero;_Tmp10;});
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 8: _Tmp9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1371
struct _tuple17 _TmpA=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpC;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpB;
struct _tuple14 _TmpD=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e2,local_alias);void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpF;void*f2r=_TmpE;
struct _tuple14 pr2=({struct _tuple14 _Tmp10;_Tmp10.f0=f2t,_Tmp10.f1=f2r;_Tmp10;});
struct _tuple14 pr3=({struct _tuple14 _Tmp10;_Tmp10.f0=f1t,_Tmp10.f1=fenv->notzeroall;_Tmp10;});
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 23: _Tmp9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1378
struct _tuple21 _TmpA=({struct Cyc_NewControlFlow_AnalEnv*_TmpB=env;union Cyc_CfFlowInfo_FlowInfo _TmpC=inflow;Cyc_NewControlFlow_anal_Rexps(_TmpB,_TmpC,({struct Cyc_Absyn_Exp*_TmpD[2];_TmpD[0]=e1,_TmpD[1]=e2;Cyc_List_list(_tag_fat(_TmpD,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;struct Cyc_List_List*rvals=_TmpB;
# 1382
union Cyc_CfFlowInfo_FlowInfo f2=f;
{struct Cyc_Dict_Dict _TmpD;if(f.ReachableFL.tag==2){_TmpD=f.ReachableFL.val;{struct Cyc_Dict_Dict d2=_TmpD;
# 1386
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d2,(void*)_check_null(_check_null(rvals)->tl)->hd)==0)
Cyc_CfFlowInfo_aerr(e2->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
f2=Cyc_CfFlowInfo_ReachableFL(d2);
goto _LL16F;}}else{
goto _LL16F;}_LL16F:;}{
# 1392
struct _tuple14 _TmpD=Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,(void*)_check_null(rvals)->hd,e2);void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f3=_TmpF;void*r=_TmpE;
if(f3.BottomFL.tag==1){
struct _tuple14 _Tmp10;_Tmp10.f0=f3,_Tmp10.f1=r;return _Tmp10;}else{
struct _tuple14 _Tmp10;_Tmp10.f0=f2,_Tmp10.f1=r;return _Tmp10;};}}}}case 29: _Tmp9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*des=_Tmp9;
# 1408
struct Cyc_List_List*fs;
{void*_TmpA=Cyc_Absyn_compress(_check_null(e->topt));void*_TmpB;if(*((int*)_TmpA)==7){_TmpB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_TmpA)->f3;{struct Cyc_List_List*f=_TmpB;
fs=f;goto _LL17F;}}else{
({int(*_TmpC)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpC;})(_tag_fat("anal_Rexp:anon struct has bad type",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));}_LL17F:;}
# 1413
_Tmp9=des;_Tmp1=0U;_Tmp6=fs;goto _LL6D;}case 28: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4!=0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4)->impl!=0){_Tmp9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4->kind;_Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4->impl->fields;_LL6D: {struct Cyc_List_List*des=_Tmp9;enum Cyc_Absyn_AggrKind kind=_Tmp1;struct Cyc_List_List*fs=_Tmp6;
# 1415
void*exp_type=_check_null(e->topt);
struct _tuple21 _TmpA=({struct Cyc_NewControlFlow_AnalEnv*_TmpB=env;union Cyc_CfFlowInfo_FlowInfo _TmpC=inflow;Cyc_NewControlFlow_anal_Rexps(_TmpB,_TmpC,({struct Cyc_List_List*(*_TmpD)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*))Cyc_List_map;_TmpD;})(({struct Cyc_Absyn_Exp*(*_TmpD)(struct _tuple24*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple24*))Cyc_Core_snd;_TmpD;}),des),1,1);});void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpC;struct Cyc_List_List*rvals=_TmpB;
struct _fat_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)kind==1,fenv->unknown_all);
# 1420
int field_no=-1;
{int i=0;for(0;rvals!=0;(rvals=rvals->tl,des=des->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple24*)_check_null(des)->hd)).f0;for(0;ds!=0;ds=ds->tl){
void*_TmpD=(void*)ds->hd;void*_TmpE;if(*((int*)_TmpD)==0)
({int(*_TmpF)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpF;})(_tag_fat("anal_Rexp:Aggregate_e",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));else{_TmpE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_TmpD)->f1;{struct _fat_ptr*fld=_TmpE;
# 1427
field_no=Cyc_CfFlowInfo_get_field_index_fs(fs,fld);
*((void**)_check_fat_subscript(aggrdict,sizeof(void*),field_no))=(void*)rvals->hd;
# 1430
if((int)kind==1){
struct Cyc_Absyn_Exp*e=(*((struct _tuple24*)des->hd)).f1;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,(void*)rvals->hd);
# 1434
if(Cyc_Absyn_is_require_union_type(exp_type))
Cyc_NewControlFlow_check_union_requires(e->loc,exp_type,fld,f);}}};}}}{
# 1439
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*res;res=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct)),res->tag=6,res->f1.is_union=(int)kind==1,res->f1.fieldnum=field_no,res->f2=aggrdict;{
struct _tuple14 _TmpD;_TmpD.f0=f,_TmpD.f1=(void*)res;return _TmpD;}}}}}else{goto _LL6E;}}else{_LL6E:
# 1443
({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({struct _fat_ptr _TmpC=Cyc_Absynpp_exp2string(e);_TmpB.f1=_TmpC;});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({int(*_TmpC)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpC;})(_tag_fat("anal_Rexp:missing aggrdeclimpl in %s",sizeof(char),37U),_tag_fat(_TmpB,sizeof(void*),1));});}case 25: _Tmp9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp9;
# 1445
struct Cyc_List_List*es=({struct Cyc_List_List*(*_TmpA)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple24*),struct Cyc_List_List*))Cyc_List_map;_TmpA;})(({struct Cyc_Absyn_Exp*(*_TmpA)(struct _tuple24*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple24*))Cyc_Core_snd;_TmpA;}),dles);
struct _tuple21 _TmpA=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo outflow=_TmpC;struct Cyc_List_List*rvals=_TmpB;
for(1;rvals!=0;(rvals=rvals->tl,es=es->tl)){
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_check_null(es)->hd)->loc,outflow,(void*)rvals->hd);}{
# 1450
struct _tuple14 _TmpD;_TmpD.f0=outflow,({void*_TmpE=Cyc_CfFlowInfo_typ_to_absrval(fenv,_check_null(e->topt),0,fenv->notzeroall);_TmpD.f1=_TmpE;});return _TmpD;}}}case 26: _Tmp9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp3;int zt=_Tmp7;
# 1454
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);void*_TmpB;union Cyc_CfFlowInfo_FlowInfo _TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_TmpC;void*r1=_TmpB;
struct Cyc_Dict_Dict _TmpD;if(f1.BottomFL.tag==1){
struct _tuple14 _TmpE;_TmpE.f0=f1,_TmpE.f1=fenv->unknown_all;return _TmpE;}else{_TmpD=f1.ReachableFL.val;{struct Cyc_Dict_Dict d1=_TmpD;
# 1458
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d1,r1)==0)
Cyc_CfFlowInfo_aerr(e1->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
# 1462
f1=Cyc_CfFlowInfo_ReachableFL(d1);
# 1465
switch(*((int*)r1)){case 0:  {
struct _tuple14 _TmpE;_TmpE.f0=f1,_TmpE.f1=fenv->unknown_all;return _TmpE;}case 1:
 goto _LL19D;case 4: _LL19D: {
# 1469
struct Cyc_List_List l=({struct Cyc_List_List _TmpE;_TmpE.hd=vd,_TmpE.tl=0;_TmpE;});
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
# 1472
struct _tuple14 _TmpE=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);void*_TmpF;union Cyc_CfFlowInfo_FlowInfo _Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp10;void*r2=_TmpF;
{struct Cyc_Dict_Dict _Tmp11;if(f2.BottomFL.tag==1){
struct _tuple14 _Tmp12;_Tmp12.f0=f2,_Tmp12.f1=fenv->unknown_all;return _Tmp12;}else{_Tmp11=f2.ReachableFL.val;{struct Cyc_Dict_Dict d2=_Tmp11;
# 1476
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!=1){
Cyc_CfFlowInfo_aerr(e1->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple14 _Tmp12;({union Cyc_CfFlowInfo_FlowInfo _Tmp13=Cyc_CfFlowInfo_BottomFL();_Tmp12.f0=_Tmp13;}),_Tmp12.f1=fenv->unknown_all;return _Tmp12;}}}};}
# 1481
f1=f2;
goto _LL19F;}}}default: _LL19F:
# 1484
 while(1){
{struct Cyc_List_List l=({struct Cyc_List_List _TmpE;_TmpE.hd=vd,_TmpE.tl=0;_TmpE;});
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
struct _tuple14 _TmpE=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);void*_TmpF;union Cyc_CfFlowInfo_FlowInfo _Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp10;void*r2=_TmpF;
{struct Cyc_Dict_Dict _Tmp11;if(f2.BottomFL.tag==1)
goto _LL1AB;else{_Tmp11=f2.ReachableFL.val;{struct Cyc_Dict_Dict d2=_Tmp11;
# 1491
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!=1){
Cyc_CfFlowInfo_aerr(e1->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple14 _Tmp12;({union Cyc_CfFlowInfo_FlowInfo _Tmp13=Cyc_CfFlowInfo_BottomFL();_Tmp12.f0=_Tmp13;}),_Tmp12.f1=fenv->unknown_all;return _Tmp12;}}}}_LL1AB:;}{
# 1496
union Cyc_CfFlowInfo_FlowInfo newflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);
if(Cyc_CfFlowInfo_flow_lessthan_approx(newflow,f1))
break;
f1=newflow;}}}}
# 1485
1U;}{
# 1501
struct _tuple14 _TmpE;_TmpE.f0=f1,_TmpE.f1=fenv->notzeroall;return _TmpE;}};}};}}case 27: _Tmp9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*exp=_Tmp9;void*typ=_Tmp6;int iszeroterm=_Tmp7;
# 1507
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_TmpA->tag=1,_TmpA->f1=exp,_TmpA->f2=_check_null(e->topt);_TmpA;});
struct Cyc_CfFlowInfo_Place*place;place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),place->root=root,place->path=0;{
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_TmpA->tag=4,_TmpA->f1=place;_TmpA;});
void*place_val;
# 1515
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,typ,0,fenv->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple14 _TmpA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);union Cyc_CfFlowInfo_FlowInfo _TmpB;_TmpB=_TmpA.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TmpB;
outflow=f;{
struct Cyc_Dict_Dict _TmpC;if(outflow.BottomFL.tag==1){
struct _tuple14 _TmpD;_TmpD.f0=outflow,_TmpD.f1=rval;return _TmpD;}else{_TmpC=outflow.ReachableFL.val;{struct Cyc_Dict_Dict d2=_TmpC;
# 1522
struct _tuple14 _TmpD;({union Cyc_CfFlowInfo_FlowInfo _TmpE=Cyc_CfFlowInfo_ReachableFL(Cyc_Dict_insert(d2,root,place_val));_TmpD.f0=_TmpE;}),_TmpD.f1=rval;return _TmpD;}};}}}}}case 36: _Tmp9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp9;
# 1526
struct _tuple15 rval_opt=({struct _tuple15 _TmpA;_TmpA.f0=env->fenv->unknown_all,_TmpA.f1=copy_ctxt;_TmpA;});
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_anal_stmt(env,inflow,s,& rval_opt);
struct _tuple14 _TmpA;_TmpA.f0=f,_TmpA.f1=rval_opt.f0;return _TmpA;}case 35: _LL7B:
# 1531
 goto _LL7D;case 24: _LL7D:
 goto _LL7F;default: _LL7F:
# 1534
({int(*_TmpA)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpA;})(_tag_fat("anal_Rexp, unexpected exp form",sizeof(char),31U),_tag_fat(0U,sizeof(void*),0));};}}
# 1546 "new_control_flow.cyc"
static struct _tuple16 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1555
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp1;if(*((int*)_Tmp0)==4){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;{void*bd=_Tmp1;
# 1558
struct Cyc_Dict_Dict _Tmp2;if(f.BottomFL.tag==1){
struct _tuple16 _Tmp3;_Tmp3.f0=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f1=_Tmp4;});return _Tmp3;}else{_Tmp2=f.ReachableFL.val;{struct Cyc_Dict_Dict outdict=_Tmp2;
# 1562
struct _tuple13 _Tmp3=Cyc_CfFlowInfo_unname_rval(r);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{void*r=_Tmp5;struct Cyc_List_List*names=_Tmp4;
retry: {enum Cyc_CfFlowInfo_InitLevel _Tmp6;void*_Tmp7;void*_Tmp8;switch(*((int*)r)){case 8:
# 1565
({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp9;})(_tag_fat("named location in anal_derefL",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));case 1:
# 1567
 e->annot=(void*)& Cyc_CfFlowInfo_NotZero_val;goto _LLD;case 5: _Tmp8=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)r)->f1;{void*rv=_Tmp8;
# 1570
if(expanded_place_possibly_null)
# 1573
e->annot=(void*)& Cyc_CfFlowInfo_UnknownZ_val;else{
# 1576
void*_Tmp9=e->annot;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp9)->tag==Cyc_CfFlowInfo_UnknownZ){
# 1580
e->annot=(void*)& Cyc_CfFlowInfo_UnknownZ_val;
goto _LL1E;}else{
# 1583
e->annot=(void*)& Cyc_CfFlowInfo_NotZero_val;
goto _LL1E;}_LL1E:;}
# 1588
if(Cyc_Tcutil_is_bound_one(bd,1)){struct _tuple16 _Tmp9;_Tmp9.f0=f,({union Cyc_CfFlowInfo_AbsLVal _TmpA=Cyc_CfFlowInfo_UnknownL();_Tmp9.f1=_TmpA;});return _Tmp9;}
goto _LLD;}case 4: _Tmp8=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)r)->f1->root;_Tmp7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)r)->f1->path;{void*root=_Tmp8;struct Cyc_List_List*path2=_Tmp7;
# 1592
e->annot=(void*)& Cyc_CfFlowInfo_NotZero_val;
if(Cyc_Tcutil_is_bound_one(bd,0)){
struct _tuple16 _Tmp9;_Tmp9.f0=f,({union Cyc_CfFlowInfo_AbsLVal _TmpA=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_TmpB->root=root,({struct Cyc_List_List*_TmpC=Cyc_List_append(path2,path);_TmpB->path=_TmpC;});_TmpB;}));_Tmp9.f1=_TmpA;});return _Tmp9;}
goto _LLD;}case 0:
# 1597
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple16 _Tmp9;({union Cyc_CfFlowInfo_FlowInfo _TmpA=Cyc_CfFlowInfo_BottomFL();_Tmp9.f0=_TmpA;}),({union Cyc_CfFlowInfo_AbsLVal _TmpA=Cyc_CfFlowInfo_UnknownL();_Tmp9.f1=_TmpA;});return _Tmp9;}case 2: _Tmp6=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)r)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp6;
# 1602
if(Cyc_Tcutil_is_bound_one(bd,0))
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
e->annot=(void*)& Cyc_CfFlowInfo_UnknownZ_val;
goto _LLD;}case 7: _Tmp8=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f3;{void*r2=_Tmp8;
# 1607
if(passthrough_consumes){
r=r2;goto retry;}
# 1610
goto _LL1D;}default: _LL1D:
# 1612
 e->annot=(void*)& Cyc_CfFlowInfo_UnknownZ_val;
goto _LLD;}_LLD:;}
# 1615
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,r)==0){
struct _tuple13 _Tmp6=Cyc_CfFlowInfo_unname_rval(r);void*_Tmp7;_Tmp7=_Tmp6.f0;{void*r=_Tmp7;
if(*((int*)r)==7){
# 1619
Cyc_CfFlowInfo_aerr(e->loc,_tag_fat("attempt to dereference an alias-free that has already been copied",sizeof(char),66U),_tag_fat(0U,sizeof(void*),0));
goto _LL26;}else{
# 1622
Cyc_CfFlowInfo_aerr(e->loc,_tag_fat("dereference of possibly uninitialized pointer",sizeof(char),46U),_tag_fat(0U,sizeof(void*),0));
goto _LL26;}_LL26:;}}{
# 1626
struct _tuple16 _Tmp6;_Tmp6.f0=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp7=Cyc_CfFlowInfo_UnknownL();_Tmp6.f1=_Tmp7;});return _Tmp6;}}}};}}else{
# 1628
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("left deref of non-pointer-type",sizeof(char),31U),_tag_fat(0U,sizeof(void*),0));};}
# 1634
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
void*_Tmp0;void*_Tmp1;_Tmp1=p->root;_Tmp0=p->path;{void*proot=_Tmp1;struct Cyc_List_List*ppath=_Tmp0;
path=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct));_Tmp4->tag=1;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=path;_Tmp2;});{
struct Cyc_CfFlowInfo_Place*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_Tmp2->root=proot,({struct Cyc_List_List*_Tmp3=Cyc_List_append(ppath,path);_Tmp2->path=_Tmp3;});return _Tmp2;}}}
# 1644
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1648
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
{struct Cyc_Dict_Dict _Tmp0;if(inflow.BottomFL.tag==1){
struct _tuple16 _Tmp1;({union Cyc_CfFlowInfo_FlowInfo _Tmp2=Cyc_CfFlowInfo_BottomFL();_Tmp1.f0=_Tmp2;}),({union Cyc_CfFlowInfo_AbsLVal _Tmp2=Cyc_CfFlowInfo_UnknownL();_Tmp1.f1=_Tmp2;});return _Tmp1;}else{_Tmp0=inflow.ReachableFL.val;{struct Cyc_Dict_Dict d2=_Tmp0;
# 1653
d=d2;}};}{
# 1656
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 14: _Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;
_Tmp2=e1;goto _LL9;}case 12: _Tmp2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL9: {struct Cyc_Absyn_Exp*e1=_Tmp2;
_Tmp2=e1;goto _LLB;}case 13: _Tmp2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLB: {struct Cyc_Absyn_Exp*e1=_Tmp2;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e1,path);}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 4: _Tmp2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1662
if((int)vd->sc==0){
struct _tuple16 _Tmp3;_Tmp3.f0=inflow,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f1=_Tmp4;});return _Tmp3;}
_Tmp2=vd;goto _LLF;}case 3: _Tmp2=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LLF: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL11;}case 5: _Tmp2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL11: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1667
struct _tuple16 _Tmp3;_Tmp3.f0=inflow,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));({void*_Tmp6=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_Tmp7->tag=0,_Tmp7->f1=vd;_Tmp7;});_Tmp5->root=_Tmp6;}),_Tmp5->path=path;_Tmp5;}));_Tmp3.f1=_Tmp4;});return _Tmp3;}case 1: _Tmp2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1669
struct _tuple16 _Tmp3;_Tmp3.f0=inflow,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f1=_Tmp4;});return _Tmp3;}default: goto _LL1C;}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 1672
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp4;if(*((int*)_Tmp3)==4){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;{void*t2=_Tmp4;
# 1674
if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(t2)){
if(Cyc_Absyn_is_require_union_type(t2))
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,f,inflow);
# 1678
path=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp6=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct));_Tmp7->tag=0,({int _Tmp8=Cyc_CfFlowInfo_get_field_index(t2,f);_Tmp7->f1=_Tmp8;});_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=path;_Tmp5;});}
# 1680
goto _LL1E;}}else{
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("anal_Lexp: AggrArrow ptr",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));}_LL1E:;}
# 1683
_Tmp2=e1;goto _LL17;}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_Tmp2;
# 1687
if(Cyc_Tcutil_is_noalias_pointer(env->aquals_bounds,_check_null(e1->topt),1)){
# 1689
struct _tuple16 _Tmp3=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,e1);union Cyc_CfFlowInfo_AbsLVal _Tmp4;union Cyc_CfFlowInfo_FlowInfo _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp5;union Cyc_CfFlowInfo_AbsLVal lval=_Tmp4;
struct _tuple16 _Tmp6=({struct _tuple16 _Tmp7;_Tmp7.f0=f,_Tmp7.f1=lval;_Tmp7;});void*_Tmp7;struct Cyc_Dict_Dict _Tmp8;if(_Tmp6.f0.ReachableFL.tag==2){if(_Tmp6.f1.PlaceL.tag==1){_Tmp8=_Tmp6.f0.ReachableFL.val;_Tmp7=_Tmp6.f1.PlaceL.val;{struct Cyc_Dict_Dict fd=_Tmp8;struct Cyc_CfFlowInfo_Place*p=_Tmp7;
# 1692
void*old_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
struct _tuple13 _Tmp9=Cyc_CfFlowInfo_unname_rval(old_rval);void*_TmpA;void*_TmpB;_TmpB=_Tmp9.f0;_TmpA=_Tmp9.f1;{void*rval=_TmpB;struct Cyc_List_List*names=_TmpA;
if(expand_unique){
int possibly_null=1;
void*_TmpC;void*_TmpD;switch(*((int*)rval)){case 8: _TmpD=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rval)->f1;_TmpC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rval)->f2;{struct Cyc_Absyn_Vardecl*n=_TmpD;void*r=_TmpC;
# 1698
({int(*_TmpE)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpE;})(_tag_fat("bad named location in anal_Lexp:deref",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));}case 7: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)rval)->f3)){case 4:
 goto _LL34;case 5:
# 1702
 goto _LL38;default: goto _LL3B;}case 4: _LL34:
# 1701
 return Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);case 5: _LL38: {
# 1706
struct _tuple16 _TmpE=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpE.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TmpF;
struct _tuple16 _Tmp10;_Tmp10.f0=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp11=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_Tmp10.f1=_Tmp11;});return _Tmp10;}}case 1:
# 1709
 possibly_null=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1716
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(fenv,fd,rval);
void*leaf=(int)il==1?fenv->unknown_all: fenv->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TmpE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_TmpE->tag=5,({void*_TmpF=({struct Cyc_CfFlowInfo_FlowEnv*_Tmp10=fenv;void*_Tmp11=Cyc_Tcutil_pointer_elt_type(_check_null(e1->topt));Cyc_CfFlowInfo_typ_to_absrval(_Tmp10,_Tmp11,0,leaf);});_TmpE->f1=_TmpF;});_TmpE;});
for(1;names!=0;names=names->tl){
res=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TmpE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_TmpE->tag=8,_TmpE->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_TmpE->f2=res;_TmpE;});}
fd=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,res);{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_ReachableFL(fd);
# 1729
struct _tuple16 _TmpE=Cyc_NewControlFlow_anal_derefL(env,outflow,e1,outflow,res,passthrough_consumes,possibly_null,path);union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpE.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TmpF;
# 1731
struct _tuple16 _Tmp10;_Tmp10.f0=f,({union Cyc_CfFlowInfo_AbsLVal _Tmp11=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_Tmp10.f1=_Tmp11;});return _Tmp10;}}}};}else{
# 1735
switch(*((int*)rval)){case 7: if(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)rval)->f3)==5)
goto _LL47;else{goto _LL48;}case 5: _LL47: {
# 1740
struct _tuple16 _TmpC=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpC.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TmpD;
struct _tuple16 _TmpE;_TmpE.f0=f,({union Cyc_CfFlowInfo_AbsLVal _TmpF=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_TmpE.f1=_TmpF;});return _TmpE;}}default: _LL48:
# 1744
 goto _LL43;}_LL43:;}
# 1747
goto _LL26;}}}else{goto _LL29;}}else{_LL29:
 goto _LL26;}_LL26:;}}{
# 1752
struct _tuple14 _Tmp3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);void*_Tmp4;union Cyc_CfFlowInfo_FlowInfo _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp5;void*r=_Tmp4;
# 1754
return Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,r,passthrough_consumes,0,path);}}}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1756
struct _tuple21 _Tmp3=({struct Cyc_NewControlFlow_AnalEnv*_Tmp4=env;union Cyc_CfFlowInfo_FlowInfo _Tmp5=inflow;Cyc_NewControlFlow_anal_Rexps(_Tmp4,_Tmp5,({struct Cyc_Absyn_Exp*_Tmp6[2];_Tmp6[0]=e1,_Tmp6[1]=e2;Cyc_List_list(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});void*_Tmp4;union Cyc_CfFlowInfo_FlowInfo _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp5;struct Cyc_List_List*rvals=_Tmp4;
union Cyc_CfFlowInfo_FlowInfo f2=f;
{struct Cyc_Dict_Dict _Tmp6;if(f.ReachableFL.tag==2){_Tmp6=f.ReachableFL.val;{struct Cyc_Dict_Dict d2=_Tmp6;
# 1760
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,(void*)_check_null(_check_null(rvals)->tl)->hd)==0)
Cyc_CfFlowInfo_aerr(e2->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
f2=Cyc_CfFlowInfo_ReachableFL(d2);
goto _LL53;}}else{
goto _LL53;}_LL53:;}{
# 1767
struct _tuple16 _Tmp6=Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,(void*)_check_null(rvals)->hd,passthrough_consumes,0,path);union Cyc_CfFlowInfo_AbsLVal _Tmp7;union Cyc_CfFlowInfo_FlowInfo _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{union Cyc_CfFlowInfo_FlowInfo f3=_Tmp8;union Cyc_CfFlowInfo_AbsLVal r=_Tmp7;
if(f3.BottomFL.tag==1){
struct _tuple16 _Tmp9;_Tmp9.f0=f3,_Tmp9.f1=r;return _Tmp9;}else{
struct _tuple16 _Tmp9;_Tmp9.f0=f2,_Tmp9.f1=r;return _Tmp9;};}}}}case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*fld=_Tmp1;
# 1774
void*e1_type=_check_null(e1->topt);
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,fld,inflow);
# 1778
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type)){
struct _tuple16 _Tmp3;_Tmp3.f0=inflow,({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f1=_Tmp4;});return _Tmp3;}{
# 1781
struct Cyc_NewControlFlow_AnalEnv*_Tmp3=env;union Cyc_CfFlowInfo_FlowInfo _Tmp4=inflow;int _Tmp5=expand_unique;int _Tmp6=passthrough_consumes;struct Cyc_Absyn_Exp*_Tmp7=e1;return Cyc_NewControlFlow_anal_Lexp_rec(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));
({void*_Tmp9=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct));_TmpA->tag=0,({int _TmpB=Cyc_CfFlowInfo_get_field_index(e1_type,fld);_TmpA->f1=_TmpB;});_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=path;_Tmp8;}));}}default: _LL1C: {
# 1785
struct _tuple16 _Tmp3;({union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_BottomFL();_Tmp3.f0=_Tmp4;}),({union Cyc_CfFlowInfo_AbsLVal _Tmp4=Cyc_CfFlowInfo_UnknownL();_Tmp3.f1=_Tmp4;});return _Tmp3;}};}}
# 1789
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1793
struct _tuple16 _Tmp0=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_AbsLVal _Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp2;union Cyc_CfFlowInfo_AbsLVal r=_Tmp1;
struct _tuple16 _Tmp3;_Tmp3.f0=f,_Tmp3.f1=r;return _Tmp3;}}
# 1800
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1803
{struct Cyc_List_List*x=es;for(0;x!=0;x=x->tl){
# 1807
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
# 1810
struct _tuple16 _Tmp0=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _Tmp1;_Tmp1=_Tmp0.f0;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp1;
inflow=f;}}}}
# 1815
return inflow;}
# 1821
static struct _tuple17 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1823
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
# 1826
struct _tuple14 _Tmp0=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)_check_null(es)->hd,0);void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp2;void*r1=_Tmp1;
struct _tuple14 _Tmp3=Cyc_NewControlFlow_anal_Rexp(env,0,f1,(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd,0);void*_Tmp4;union Cyc_CfFlowInfo_FlowInfo _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp5;void*r2=_Tmp4;
union Cyc_CfFlowInfo_FlowInfo f=f2;
# 1831
struct Cyc_Dict_Dict _Tmp6;if(f.BottomFL.tag==1){
struct _tuple17 _Tmp7;_Tmp7.f0=f,_Tmp7.f1=f;return _Tmp7;}else{_Tmp6=f.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp6;
# 1834
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)es->tl->hd;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r1)==0 && !Cyc_CfFlowInfo_is_init_pointer(r1))
Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r2)==0 && !Cyc_CfFlowInfo_is_init_pointer(r1))
Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->tl->hd)->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));{
# 1841
union Cyc_CfFlowInfo_FlowInfo ft=f;
union Cyc_CfFlowInfo_FlowInfo ff=f;
# 1846
if((int)p==5 ||(int)p==6){
struct _tuple13 _Tmp7=Cyc_CfFlowInfo_unname_rval(r1);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*r1=_Tmp9;struct Cyc_List_List*r1n=_Tmp8;
struct _tuple13 _TmpA=Cyc_CfFlowInfo_unname_rval(r2);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{void*r2=_TmpC;struct Cyc_List_List*r2n=_TmpB;
struct _tuple0 _TmpD=({struct _tuple0 _TmpE;_TmpE.f0=r1,_TmpE.f1=r2;_TmpE;});enum Cyc_CfFlowInfo_InitLevel _TmpE;switch(*((int*)_TmpD.f0)){case 2: if(*((int*)_TmpD.f1)==0){_TmpE=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TmpD.f0)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_TmpE;
# 1851
struct _tuple17 _TmpF=Cyc_NewControlFlow_splitzero(env,inflow,f,e1,il,r1n);union Cyc_CfFlowInfo_FlowInfo _Tmp10;union Cyc_CfFlowInfo_FlowInfo _Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp11;union Cyc_CfFlowInfo_FlowInfo f2=_Tmp10;
switch((int)p){case Cyc_Absyn_Eq:
 ft=f2;ff=f1;goto _LL25;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL25;default:
({int(*_Tmp12)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp12;})(_tag_fat("anal_test, zero-split",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));}_LL25:;
# 1857
goto _LL11;}}}else{goto _LL20;}case 0: switch(*((int*)_TmpD.f1)){case 2: _TmpE=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TmpD.f1)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_TmpE;
# 1859
struct _tuple17 _TmpF=Cyc_NewControlFlow_splitzero(env,f2,f,e2,il,r2n);union Cyc_CfFlowInfo_FlowInfo _Tmp10;union Cyc_CfFlowInfo_FlowInfo _Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp11;union Cyc_CfFlowInfo_FlowInfo f2=_Tmp10;
switch((int)p){case Cyc_Absyn_Eq:
 ft=f2;ff=f1;goto _LL2F;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL2F;default:
({int(*_Tmp12)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp12;})(_tag_fat("anal_test, zero-split",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));}_LL2F:;
# 1865
goto _LL11;}}case 0:
# 1867
 if((int)p==5)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;case 1:
 goto _LL1B;case 4: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1: if(*((int*)_TmpD.f1)==0){_LL1D:
 goto _LL1F;}else{goto _LL20;}case 4: if(*((int*)_TmpD.f1)==0){_LL1F:
# 1874
 if((int)p==6)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;}else{goto _LL20;}default: _LL20:
 goto _LL11;}_LL11:;}}}{
# 1881
struct _tuple17 _Tmp7;_Tmp7.f0=ft,_Tmp7.f1=ff;return _Tmp7;}}}};}}}
# 1887
static struct _tuple17 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
void*_Tmp0=e->r;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 1891
struct _tuple17 _Tmp5=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_Tmp7;union Cyc_CfFlowInfo_FlowInfo f1f=_Tmp6;
struct _tuple17 _Tmp8=Cyc_NewControlFlow_anal_test(env,f1t,e2);union Cyc_CfFlowInfo_FlowInfo _Tmp9;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpA;union Cyc_CfFlowInfo_FlowInfo f2f=_Tmp9;
struct _tuple17 _TmpB=Cyc_NewControlFlow_anal_test(env,f1f,e3);union Cyc_CfFlowInfo_FlowInfo _TmpC;union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{union Cyc_CfFlowInfo_FlowInfo f3t=_TmpD;union Cyc_CfFlowInfo_FlowInfo f3f=_TmpC;
struct _tuple17 _TmpE;({union Cyc_CfFlowInfo_FlowInfo _TmpF=Cyc_CfFlowInfo_join_flow(fenv,f2t,f3t);_TmpE.f0=_TmpF;}),({
union Cyc_CfFlowInfo_FlowInfo _TmpF=Cyc_CfFlowInfo_join_flow(fenv,f2f,f3f);_TmpE.f1=_TmpF;});return _TmpE;}}}}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1897
struct _tuple17 _Tmp5=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_Tmp7;union Cyc_CfFlowInfo_FlowInfo f1f=_Tmp6;
struct _tuple17 _Tmp8=Cyc_NewControlFlow_anal_test(env,f1t,e2);union Cyc_CfFlowInfo_FlowInfo _Tmp9;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpA;union Cyc_CfFlowInfo_FlowInfo f2f=_Tmp9;
struct _tuple17 _TmpB;_TmpB.f0=f2t,({union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_CfFlowInfo_join_flow(fenv,f1f,f2f);_TmpB.f1=_TmpC;});return _TmpB;}}}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1901
struct _tuple17 _Tmp5=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_Tmp7;union Cyc_CfFlowInfo_FlowInfo f1f=_Tmp6;
struct _tuple17 _Tmp8=Cyc_NewControlFlow_anal_test(env,f1f,e2);union Cyc_CfFlowInfo_FlowInfo _Tmp9;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpA;union Cyc_CfFlowInfo_FlowInfo f2f=_Tmp9;
struct _tuple17 _TmpB;({union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_CfFlowInfo_join_flow(fenv,f1t,f2t);_TmpB.f0=_TmpC;}),_TmpB.f1=f2f;return _TmpB;}}}case 9: _Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1905
struct _tuple14 _Tmp5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);void*_Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp7;void*r=_Tmp6;
return Cyc_NewControlFlow_anal_test(env,f,e2);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl==0){_Tmp4=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;{struct Cyc_Absyn_Exp*e1=_Tmp4;
# 1908
struct _tuple17 _Tmp5=Cyc_NewControlFlow_anal_test(env,inflow,e1);union Cyc_CfFlowInfo_FlowInfo _Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp7;union Cyc_CfFlowInfo_FlowInfo f2=_Tmp6;
struct _tuple17 _Tmp8;_Tmp8.f0=f2,_Tmp8.f1=f1;return _Tmp8;}}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_List_List*es=_Tmp4;
# 1911
return Cyc_NewControlFlow_anal_primop_test(env,inflow,p,es);}}default:  {
# 1915
struct _tuple14 _Tmp5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0);void*_Tmp6;union Cyc_CfFlowInfo_FlowInfo _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp7;void*r=_Tmp6;
struct Cyc_Dict_Dict _Tmp8;if(f.BottomFL.tag==1){
struct _tuple17 _Tmp9;_Tmp9.f0=f,_Tmp9.f1=f;return _Tmp9;}else{_Tmp8=f.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp8;
# 1919
struct _tuple13 _Tmp9=Cyc_CfFlowInfo_unname_rval(r);void*_TmpA;void*_TmpB;_TmpB=_Tmp9.f0;_TmpA=_Tmp9.f1;{void*r=_TmpB;struct Cyc_List_List*names=_TmpA;
enum Cyc_CfFlowInfo_InitLevel _TmpC;void*_TmpD;void*_TmpE;switch(*((int*)r)){case 8: _TmpE=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f1;_TmpD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f2;{struct Cyc_Absyn_Vardecl*n=_TmpE;void*r2=_TmpD;
# 1922
({int(*_TmpF)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpF;})(_tag_fat("anal_test: bad namedlocation",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}case 0:  {
struct _tuple17 _TmpF;({union Cyc_CfFlowInfo_FlowInfo _Tmp10=Cyc_CfFlowInfo_BottomFL();_TmpF.f0=_Tmp10;}),_TmpF.f1=f;return _TmpF;}case 1:
 goto _LL3D;case 5: _LL3D:
 goto _LL3F;case 4: _LL3F: {
struct _tuple17 _TmpF;_TmpF.f0=f,({union Cyc_CfFlowInfo_FlowInfo _Tmp10=Cyc_CfFlowInfo_BottomFL();_TmpF.f1=_Tmp10;});return _TmpF;}case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)r)->f1==Cyc_CfFlowInfo_NoneIL)
goto _LL43;else{_TmpC=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)r)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_TmpC;
# 1932
return Cyc_NewControlFlow_splitzero(env,inflow,f,e,il,names);}}case 3: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)r)->f1==Cyc_CfFlowInfo_NoneIL){_LL43:
# 1928
 goto _LL45;}else{
# 1933
struct _tuple17 _TmpF;_TmpF.f0=f,_TmpF.f1=f;return _TmpF;}case 7: _LL45:
# 1930
 Cyc_CfFlowInfo_aerr(e->loc,_tag_fat("expression may not be initialized",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple17 _TmpF;({union Cyc_CfFlowInfo_FlowInfo _Tmp10=Cyc_CfFlowInfo_BottomFL();_TmpF.f0=_Tmp10;}),({union Cyc_CfFlowInfo_FlowInfo _Tmp10=Cyc_CfFlowInfo_BottomFL();_TmpF.f1=_Tmp10;});return _TmpF;}default:
# 1934
({int(*_TmpF)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpF;})(_tag_fat("anal_test",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));};}}};}}};}struct _tuple25{unsigned f0;struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;};
# 1940
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple25*ckenv,void*root,void*rval){
# 1942
struct Cyc_Dict_Dict _Tmp0;void*_Tmp1;unsigned _Tmp2;_Tmp2=ckenv->f0;_Tmp1=ckenv->f1;_Tmp0=ckenv->f2;{unsigned loc=_Tmp2;struct Cyc_NewControlFlow_AnalEnv*env=_Tmp1;struct Cyc_Dict_Dict fd=_Tmp0;
void*_Tmp3;if(*((int*)root)==0){_Tmp3=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)root)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1945
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,vd->type)){
struct _tuple13 _Tmp4=Cyc_CfFlowInfo_unname_rval(rval);void*_Tmp5;_Tmp5=_Tmp4.f0;{void*rval=_Tmp5;
switch(*((int*)rval)){case 7:
 goto _LLF;case 0: _LLF:
 goto _LL11;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)rval)->f1==Cyc_CfFlowInfo_NoneIL){_LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12:
# 1952
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_qvar2string(vd->name);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_warn(loc,_tag_fat("unique pointers reachable from %s may become unreachable",sizeof(char),57U),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LLB;}_LLB:;}}
# 1956
goto _LL3;}}else{
goto _LL3;}_LL3:;}}
# 1961
static void Cyc_NewControlFlow_check_init_params(unsigned loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_Dict_Dict _Tmp0;if(flow.BottomFL.tag==1)
return;else{_Tmp0=flow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp0;
# 1965
{struct Cyc_List_List*inits=env->param_roots;for(0;inits!=0;inits=inits->tl){
if((int)({struct Cyc_CfFlowInfo_FlowEnv*_Tmp1=env->fenv;struct Cyc_Dict_Dict _Tmp2=d;Cyc_CfFlowInfo_initlevel(_Tmp1,_Tmp2,Cyc_CfFlowInfo_lookup_place(d,(struct Cyc_CfFlowInfo_Place*)inits->hd));})!=1)
Cyc_CfFlowInfo_aerr(loc,
_tag_fat("function may not initialize all the parameters with attribute 'initializes'",sizeof(char),76U),_tag_fat(0U,sizeof(void*),0));}}
if(Cyc_Flags_warn_lose_unique){
struct _tuple25 check_env=({struct _tuple25 _Tmp1;_Tmp1.f0=loc,_Tmp1.f1=env,_Tmp1.f2=d;_Tmp1;});
({void(*_Tmp1)(void(*)(struct _tuple25*,void*,void*),struct _tuple25*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple25*,void*,void*),struct _tuple25*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp1;})(Cyc_NewControlFlow_check_for_unused_unique,& check_env,d);}
# 1973
return;}};}
# 1982
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*vds){
# 1984
struct Cyc_List_List*roots=0;
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x!=0;x=x->tl){
struct _tuple22*_Tmp0=(struct _tuple22*)x->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_Absyn_Vardecl**vopt=_Tmp2;struct Cyc_Absyn_Exp*eopt=_Tmp1;
if((vopt!=0 && eopt!=0)&&
 Cyc_Tcutil_is_noalias_pointer(env->aquals_bounds,_check_null(eopt->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct)),root->tag=0,root->f1=*vopt;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)root,rp->path=0;
roots=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=rp,_Tmp3->tl=roots;_Tmp3;});
es=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=eopt,_Tmp3->tl=es;_Tmp3;});}}}}}{
# 1996
struct _tuple1 _Tmp0;_Tmp0.f0=roots,_Tmp0.f1=es;return _Tmp0;}}struct _tuple26{int f0;void*f1;};
# 2002
static struct _tuple26 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2009
struct Cyc_Dict_Dict _Tmp0;if(flow.BottomFL.tag==1)
({int(*_Tmp1)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp1;})(_tag_fat("noconsume_place_ok: flow became Bottom!",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));else{_Tmp0=flow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp0;
# 2017
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_uint_exp(1U,0U);
int bogus_bool=0;
int bogus_int=1;
void*curr_rval=Cyc_CfFlowInfo_lookup_place(d,place);
void*rval=curr_rval;
# 2029
int varok=0;
{void*_Tmp1;void*_Tmp2;if(*((int*)curr_rval)==8){_Tmp2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)curr_rval)->f1;_Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)curr_rval)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp2;void*r=_Tmp1;
# 2032
if(vd==n){
rval=r;
# 2035
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,vd->type)){
# 2037
if(Cyc_CfFlowInfo_is_unique_consumed(bogus_exp,bogus_int,rval,& bogus_bool)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
# 2041
struct _fat_ptr _Tmp5=Cyc_Absynpp_qvar2string(vd->name);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_CfFlowInfo_aerr(loc,_tag_fat("function consumes parameter %s which is does not have the 'consume' attribute",sizeof(char),78U),_tag_fat(_Tmp4,sizeof(void*),1));});}else{
# 2044
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,rval)!=1 && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
# 2047
struct _fat_ptr _Tmp5=Cyc_Absynpp_qvar2string(vd->name);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_CfFlowInfo_aerr(loc,_tag_fat("function consumes value pointed to by parameter %s, which does not have the 'consume' attribute",sizeof(char),96U),_tag_fat(_Tmp4,sizeof(void*),1));});else{
# 2049
varok=1;}}}else{
# 2052
varok=1;}}else{
# 2055
goto _LL9;}
goto _LL5;}}else{_LL9:
# 2059
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
# 2064
struct _fat_ptr _Tmp5=Cyc_Absynpp_qvar2string(vd->name);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_CfFlowInfo_aerr(loc,_tag_fat("function parameter %s without 'consume' attribute no longer set to its original value",sizeof(char),86U),_tag_fat(_Tmp4,sizeof(void*),1));});}
goto _LL5;}_LL5:;}{
# 2071
struct _tuple26 _Tmp1;_Tmp1.f0=varok,_Tmp1.f1=rval;return _Tmp1;}}};}
# 2077
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
void*_Tmp0;void*_Tmp1;_Tmp1=p->root;_Tmp0=p->path;{void*root=_Tmp1;struct Cyc_List_List*fs=_Tmp0;
if(fs!=0)
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("unconsume_params: param to unconsume is non-variable",sizeof(char),53U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_Absyn_Vardecl*vd;
void*_Tmp2;if(*((int*)root)==0){_Tmp2=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)root)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
return vd;}}else{
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("unconsume_params: root is not a varroot",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));};}}}
# 2096 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2104
{struct Cyc_Dict_Dict _Tmp0;if(flow.BottomFL.tag==1)
return flow;else{_Tmp0=flow.ReachableFL.val;{struct Cyc_Dict_Dict d=_Tmp0;
# 2111
struct _tuple16 _Tmp1=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);union Cyc_CfFlowInfo_AbsLVal _Tmp2;union Cyc_CfFlowInfo_FlowInfo _Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp3;union Cyc_CfFlowInfo_AbsLVal lval=_Tmp2;
# 2114
{void*_Tmp4;if(lval.PlaceL.tag==1){_Tmp4=lval.PlaceL.val;{struct Cyc_CfFlowInfo_Place*p=_Tmp4;
# 2118
void*old_rval=Cyc_CfFlowInfo_lookup_place(d,p);
{void*_Tmp5;void*_Tmp6;if(*((int*)old_rval)==8){_Tmp6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)old_rval)->f1;_Tmp5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)old_rval)->f2;{struct Cyc_Absyn_Vardecl*old_vd=_Tmp6;void*r=_Tmp5;
# 2124
void*new_rval;
if(old_vd==vd){
# 2128
if(varok){
# 2130
old_rval=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,r);
# 2135
if(ropt!=0){
# 2141
struct _tuple14 _Tmp7=({
struct Cyc_CfFlowInfo_FlowEnv*_Tmp8=env->fenv;struct _tuple14 _Tmp9=({struct _tuple14 _TmpA;_TmpA.f0=f,_TmpA.f1=old_rval;_TmpA;});Cyc_CfFlowInfo_join_flow_and_rval(_Tmp8,_Tmp9,({struct _tuple14 _TmpA;_TmpA.f0=f,_TmpA.f1=ropt;_TmpA;}));});
# 2141
void*_Tmp8;union Cyc_CfFlowInfo_FlowInfo _Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp9;void*new_rval2=_Tmp8;
# 2145
f=f2;new_rval=new_rval2;}}else{
# 2150
new_rval=old_rval;}}else{
# 2153
new_rval=r;}
# 2155
{struct Cyc_Dict_Dict _Tmp7;if(f.ReachableFL.tag==2){_Tmp7=f.ReachableFL.val;{struct Cyc_Dict_Dict d2=_Tmp7;
# 2157
flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,d2,p,new_rval));
# 2161
goto _LL15;}}else{
# 2163
({int(*_Tmp8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp8;})(_tag_fat("unconsume_params: joined flow became bot!",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));}_LL15:;}
# 2165
goto _LLD;}else{
# 2167
goto _LL11;}
goto _LLD;}}else{_LL11:
# 2174
 if(ropt!=0 && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,vd->type))
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=Cyc_Absynpp_exp2string(unconsume_exp);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_CfFlowInfo_aerr(loc,_tag_fat("aliased expression %s was overwritten",sizeof(char),38U),_tag_fat(_Tmp8,sizeof(void*),1));});
# 2191 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2193
goto _LL8;}}else{
# 2199
goto _LL8;}_LL8:;}
# 2201
goto _LL0;}}}_LL0:;}
# 2203
return flow;}
# 2216 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2222
if(flow.BottomFL.tag==1)
return flow;else{
goto _LL0;}_LL0:;{
# 2226
int do_unconsume=unconsume_exps!=0;
struct Cyc_List_List*_Tmp0=Cyc_List_rev(consumed_vals);struct Cyc_List_List*consumed_vals=_Tmp0;
struct Cyc_List_List*_Tmp1=Cyc_List_rev(unconsume_exps);struct Cyc_List_List*unconsume_exps=_Tmp1;
{struct Cyc_List_List*params=consumed_vals;for(0;params!=0;(
params=params->tl,
unconsume_exps!=0?unconsume_exps=unconsume_exps->tl: 0)){
# 2235
struct Cyc_Absyn_Vardecl*vd=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)params->hd);
struct _tuple26 _Tmp2=
is_region_open?({struct _tuple26 _Tmp3;_Tmp3.f0=1,_Tmp3.f1=0;_Tmp3;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)params->hd,do_unconsume,vd,flow,loc);
# 2236
void*_Tmp3;int _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{int varok=_Tmp4;void*rval=_Tmp3;
# 2242
if(do_unconsume)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)unconsume_exps->hd,vd,varok,rval,flow,loc);}}}
# 2245
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;}}struct _tuple27{int f0;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2249
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned exp_loc){
# 2251
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
for(1;scs!=0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_Tmp0=(struct Cyc_Absyn_Switch_clause*)scs->hd;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp0->pat_vars;_Tmp3=_Tmp0->where_clause;_Tmp2=_Tmp0->body;_Tmp1=_Tmp0->loc;{struct Cyc_Core_Opt*vds_opt=_Tmp4;struct Cyc_Absyn_Exp*where_opt=_Tmp3;struct Cyc_Absyn_Stmt*body=_Tmp2;unsigned loc=_Tmp1;
struct _tuple1 _Tmp5=Cyc_NewControlFlow_get_unconsume_pat_vars(env,(struct Cyc_List_List*)_check_null(vds_opt)->v);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*roots=_Tmp7;struct Cyc_List_List*es=_Tmp6;
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)vds_opt->v,roots!=0,loc,exp_loc,0);
# 2259
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*old_unique_pat_vars=env->unique_pat_vars;
# 2262
if(Cyc_Tcpat_has_vars(vds_opt))
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple27*_TmpA=({struct _tuple27*_TmpB=_cycalloc(sizeof(struct _tuple27));_TmpB->f0=0,_TmpB->f1=body,_TmpB->f2=roots,_TmpB->f3=es;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=old_unique_pat_vars;_Tmp9;});
# 2263
env->unique_pat_vars=_Tmp8;});
# 2266
if(where_opt!=0){
struct Cyc_Absyn_Exp*wexp=where_opt;
struct _tuple17 _Tmp8=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _Tmp9;union Cyc_CfFlowInfo_FlowInfo _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{union Cyc_CfFlowInfo_FlowInfo ft=_TmpA;union Cyc_CfFlowInfo_FlowInfo ff=_Tmp9;
inflow=ff;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,ft,body,0);}}else{
# 2272
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,body,0);}
# 2274
env->unique_pat_vars=old_unique_pat_vars;
if(clause_outflow.BottomFL.tag==1)
goto _LL9;else{
# 2279
clause_outflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,clause_outflow,loc);
# 2281
if(scs->tl==0)
return clause_outflow;else{
# 2286
if((struct Cyc_List_List*)((struct Cyc_Absyn_Switch_clause*)scs->tl->hd)->pat_vars->v!=0)
Cyc_CfFlowInfo_aerr(body->loc,_tag_fat("switch clause may implicitly fallthru",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));else{
# 2289
Cyc_Warn_warn(body->loc,_tag_fat("switch clause may implicitly fallthru",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));}
# 2291
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)scs->tl->hd)->body,clause_outflow);}
# 2293
goto _LL9;}_LL9:;}}}
# 2296
return Cyc_CfFlowInfo_BottomFL();}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f0;struct Cyc_Dict_Dict f1;unsigned f2;};
# 2301
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2303
unsigned _Tmp0;struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;_Tmp2=vdenv->f0;_Tmp1=vdenv->f1;_Tmp0=vdenv->f2;{struct Cyc_NewControlFlow_AnalEnv*env=_Tmp2;struct Cyc_Dict_Dict fd=_Tmp1;unsigned loc=_Tmp0;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->aquals_bounds,vd->type)){
# 2306
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=vd;_Tmp3;});
struct _tuple13 _Tmp3=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _Tmp4=fd;Cyc_Dict_lookup_other(_Tmp4,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));*_Tmp5=vdroot;_Tmp5;}));}));void*_Tmp4;_Tmp4=_Tmp3.f0;{void*rval=_Tmp4;
switch(*((int*)rval)){case 7:
 goto _LLA;case 0: _LLA:
 goto _LLC;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)rval)->f1==Cyc_CfFlowInfo_NoneIL){_LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD:
# 2313
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_qvar2string(vd->name);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn(loc,_tag_fat("unique pointers may still exist after variable %s goes out of scope",sizeof(char),68U),_tag_fat(_Tmp6,sizeof(void*),1));});
# 2315
goto _LL6;}_LL6:;}}}}
# 2320
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{struct Cyc_Dict_Dict _Tmp0;if(inflow.ReachableFL.tag==2){_Tmp0=inflow.ReachableFL.val;{struct Cyc_Dict_Dict fd=_Tmp0;
# 2323
struct _tuple28 vdenv=({struct _tuple28 _Tmp1;_Tmp1.f0=env,_Tmp1.f1=fd,_Tmp1.f2=decl->loc;_Tmp1;});
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
{void*_Tmp1=decl->r;void*_Tmp2;switch(*((int*)_Tmp1)){case 0: _Tmp2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 2327
Cyc_NewControlFlow_check_dropped_unique_vd(& vdenv,vd);
goto _LL5;}case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp1)->f2!=0){_Tmp2=(struct Cyc_List_List*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp1)->f2->v;{struct Cyc_List_List*vds=_Tmp2;
# 2330
struct _tuple1 _Tmp3=Cyc_List_split(vds);void*_Tmp4;_Tmp4=_Tmp3.f0;{struct Cyc_List_List*vs=_Tmp4;
struct Cyc_List_List*_Tmp5=Cyc_Tcutil_filter_nulls(vs);{struct Cyc_List_List*vs=_Tmp5;
_Tmp2=vs;goto _LLB;}}}}else{goto _LLC;}case 3: _Tmp2=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp1)->f1;_LLB: {struct Cyc_List_List*vds=_Tmp2;
# 2334
({void(*_Tmp3)(void(*)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*,struct Cyc_List_List*)=(void(*)(void(*)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*,struct Cyc_List_List*))Cyc_List_iter_c;_Tmp3;})(Cyc_NewControlFlow_check_dropped_unique_vd,& vdenv,vds);
goto _LL5;}default: _LLC:
 goto _LL5;}_LL5:;}
# 2338
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 2341
return;}
# 2347
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2351
int num_pop=({int(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;_Tmp0;})(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple27*_Tmp0=(struct _tuple27*)_check_null(x)->hd;void*_Tmp1;void*_Tmp2;void*_Tmp3;int _Tmp4;_Tmp4=_Tmp0->f0;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{int is_open=_Tmp4;struct Cyc_Absyn_Stmt*pat_stmt=_Tmp3;struct Cyc_List_List*roots=_Tmp2;struct Cyc_List_List*es=_Tmp1;
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,is_open,inflow,dest->loc);}}}
# 2356
return inflow;}
# 2362
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple15*rval_opt){
# 2364
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple18 _Tmp0=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_NewControlFlow_CFStmtAnnot*annot=_Tmp2;union Cyc_CfFlowInfo_FlowInfo*sflow=_Tmp1;
inflow=*sflow;{
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
# 2371
void*_Tmp3=s->r;unsigned _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp3)){case 0:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1==0){
# 2375
if(env->noreturn)
Cyc_CfFlowInfo_aerr(s->loc,_tag_fat("`noreturn' function might return",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_TmpA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 2381
if(env->noreturn)
Cyc_CfFlowInfo_aerr(s->loc,_tag_fat("`noreturn' function might return",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple14 _TmpB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e,0);void*_TmpC;union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpD;void*r=_TmpC;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,r);
Cyc_NewControlFlow_check_init_params(s->loc,env,f);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,f,s->loc);
return Cyc_CfFlowInfo_BottomFL();}}}}case 1: _TmpA=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 2390
int _TmpB;void*_TmpC;if(rval_opt!=0){_TmpC=(void**)& rval_opt->f0;_TmpB=rval_opt->f1;{void**rval=(void**)_TmpC;int copy_ctxt=_TmpB;
# 2392
struct _tuple14 _TmpD=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e,0);void*_TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpF;void*r=_TmpE;
*rval=r;
return f;}}}else{
# 2396
return Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0).f0;};}case 2: _TmpA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Stmt*s1=_TmpA;struct Cyc_Absyn_Stmt*s2=_Tmp9;
# 2400
struct Cyc_NewControlFlow_AnalEnv*_TmpB=env;union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);struct Cyc_Absyn_Stmt*_TmpD=s2;return Cyc_NewControlFlow_anal_stmt(_TmpB,_TmpC,_TmpD,rval_opt);}case 4: _TmpA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;_Tmp8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3;{struct Cyc_Absyn_Exp*e=_TmpA;struct Cyc_Absyn_Stmt*s1=_Tmp9;struct Cyc_Absyn_Stmt*s2=_Tmp8;
# 2403
struct _tuple17 _TmpB=Cyc_NewControlFlow_anal_test(env,inflow,e);union Cyc_CfFlowInfo_FlowInfo _TmpC;union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpD;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpC;
# 2410
union Cyc_CfFlowInfo_FlowInfo ff=Cyc_NewControlFlow_anal_stmt(env,f1f,s2,0);
union Cyc_CfFlowInfo_FlowInfo ft=Cyc_NewControlFlow_anal_stmt(env,f1t,s1,0);
return Cyc_CfFlowInfo_join_flow(fenv,ft,ff);}}case 5: _TmpA=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1.f0;_Tmp9=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1.f1;_Tmp8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Exp*e=_TmpA;struct Cyc_Absyn_Stmt*cont=_Tmp9;struct Cyc_Absyn_Stmt*body=_Tmp8;
# 2418
struct _tuple18 _TmpB=Cyc_NewControlFlow_pre_stmt_check(env,inflow,cont);void*_TmpC;_TmpC=_TmpB.f1;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_TmpC;
union Cyc_CfFlowInfo_FlowInfo e_inflow=*eflow_ptr;
struct _tuple17 _TmpD=Cyc_NewControlFlow_anal_test(env,e_inflow,e);union Cyc_CfFlowInfo_FlowInfo _TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpF;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpE;
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,f1t,body,0);
Cyc_NewControlFlow_update_flow(env,cont,body_outflow);
return f1f;}}}case 14: _TmpA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2.f0;_Tmp8=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2.f1;{struct Cyc_Absyn_Stmt*body=_TmpA;struct Cyc_Absyn_Exp*e=_Tmp9;struct Cyc_Absyn_Stmt*cont=_Tmp8;
# 2428
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,body,0);
struct _tuple18 _TmpB=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);void*_TmpC;_TmpC=_TmpB.f1;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_TmpC;
union Cyc_CfFlowInfo_FlowInfo e_inflow=*eflow_ptr;
struct _tuple17 _TmpD=Cyc_NewControlFlow_anal_test(env,e_inflow,e);union Cyc_CfFlowInfo_FlowInfo _TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f1t=_TmpF;union Cyc_CfFlowInfo_FlowInfo f1f=_TmpE;
Cyc_NewControlFlow_update_flow(env,body,f1t);
return f1f;}}}case 9: _TmpA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2.f0;_Tmp8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2.f1;_Tmp7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3.f0;_Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3.f1;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp3)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp9;struct Cyc_Absyn_Stmt*guard=_Tmp8;struct Cyc_Absyn_Exp*e3=_Tmp7;struct Cyc_Absyn_Stmt*cont=_Tmp6;struct Cyc_Absyn_Stmt*body=_Tmp5;
# 2437
union Cyc_CfFlowInfo_FlowInfo e1_outflow=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0).f0;
struct _tuple18 _TmpB=Cyc_NewControlFlow_pre_stmt_check(env,e1_outflow,guard);void*_TmpC;_TmpC=_TmpB.f1;{union Cyc_CfFlowInfo_FlowInfo*e2flow_ptr=_TmpC;
union Cyc_CfFlowInfo_FlowInfo e2_inflow=*e2flow_ptr;
struct _tuple17 _TmpD=Cyc_NewControlFlow_anal_test(env,e2_inflow,e2);union Cyc_CfFlowInfo_FlowInfo _TmpE;union Cyc_CfFlowInfo_FlowInfo _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{union Cyc_CfFlowInfo_FlowInfo f2t=_TmpF;union Cyc_CfFlowInfo_FlowInfo f2f=_TmpE;
union Cyc_CfFlowInfo_FlowInfo body_outflow=Cyc_NewControlFlow_anal_stmt(env,f2t,body,0);
struct _tuple18 _Tmp10=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);void*_Tmp11;_Tmp11=_Tmp10.f1;{union Cyc_CfFlowInfo_FlowInfo*e3flow_ptr=_Tmp11;
union Cyc_CfFlowInfo_FlowInfo e3_inflow=*e3flow_ptr;
union Cyc_CfFlowInfo_FlowInfo e3_outflow=Cyc_NewControlFlow_anal_Rexp(env,0,e3_inflow,e3,0).f0;
Cyc_NewControlFlow_update_flow(env,guard,e3_outflow);
return f2f;}}}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2!=0){_TmpA=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_List_List*es=_TmpA;struct Cyc_Absyn_Switch_clause*destclause=_Tmp9;
# 2449
struct _tuple21 _TmpB=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);void*_TmpC;union Cyc_CfFlowInfo_FlowInfo _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{union Cyc_CfFlowInfo_FlowInfo f=_TmpD;struct Cyc_List_List*rvals=_TmpC;
# 2451
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,destclause->body);{
# 2453
struct Cyc_List_List*vds=Cyc_Tcutil_filter_nulls(Cyc_List_split((struct Cyc_List_List*)_check_null(destclause->pat_vars)->v).f0);
f=Cyc_NewControlFlow_add_vars(fenv,f,vds,fenv->unknown_all,s->loc,0);
# 2456
{struct Cyc_List_List*x=(struct Cyc_List_List*)_check_null(destclause->pat_vars)->v;for(0;x!=0;x=x->tl){
struct _tuple22*_TmpE=(struct _tuple22*)x->hd;void*_TmpF;void*_Tmp10;_Tmp10=_TmpE->f0;_TmpF=_TmpE->f1;{struct Cyc_Absyn_Vardecl**vd=_Tmp10;struct Cyc_Absyn_Exp*ve=_TmpF;
if(vd!=0){
f=({struct Cyc_CfFlowInfo_FlowEnv*_Tmp11=fenv;struct Cyc_NewControlFlow_AnalEnv*_Tmp12=env;union Cyc_CfFlowInfo_FlowInfo _Tmp13=f;struct Cyc_Absyn_Vardecl*_Tmp14=*vd;struct Cyc_Absyn_Exp*_Tmp15=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;void*_Tmp16=(void*)_check_null(rvals)->hd;Cyc_NewControlFlow_do_initialize_var(_Tmp11,_Tmp12,_Tmp13,_Tmp14,_Tmp15,_Tmp16,s->loc);});
rvals=rvals->tl;
es=es->tl;}}}}
# 2464
Cyc_NewControlFlow_update_flow(env,destclause->body,f);
return Cyc_CfFlowInfo_BottomFL();}}}}else{
# 2635
({int(*_TmpB)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpB;})(_tag_fat("anal_stmt: bad stmt syntax or unimplemented stmt form",sizeof(char),54U),_tag_fat(0U,sizeof(void*),0));}case 6:
# 2470
 if(({struct Cyc_Absyn_Stmt*(*_TmpB)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;_TmpB;})(env->succ_table,s)==0){
if(inflow.ReachableFL.tag==2){
# 2473
if(!Cyc_Tcutil_is_void_type(env->return_type)){
if(Cyc_Tcutil_is_any_float_type(env->return_type)||
 Cyc_Tcutil_is_any_int_type(env->return_type))
Cyc_Warn_warn(s->loc,_tag_fat("break may cause function not to return a value",sizeof(char),47U),_tag_fat(0U,sizeof(void*),0));else{
# 2478
Cyc_CfFlowInfo_aerr(s->loc,_tag_fat("break may cause function not to return a value",sizeof(char),47U),_tag_fat(0U,sizeof(void*),0));}}
# 2480
goto _LL55;}else{
goto _LL55;}_LL55:;
# 2483
if(env->noreturn)
Cyc_CfFlowInfo_aerr(s->loc,_tag_fat("`noreturn' function might return",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2489
goto _LL1B;case 7: _LL1B:
 goto _LL1D;case 8: _LL1D: {
# 2493
struct Cyc_Absyn_Stmt*dest=({struct Cyc_Absyn_Stmt*(*_TmpB)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;_TmpB;})(env->succ_table,s);
if(dest==0)
Cyc_CfFlowInfo_aerr(s->loc,_tag_fat("jump has no target (should be impossible)",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_check_null(dest));
# 2498
Cyc_NewControlFlow_update_flow(env,dest,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _TmpA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;_Tmp8=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3;{struct Cyc_Absyn_Exp*e=_TmpA;struct Cyc_List_List*scs=_Tmp9;void*dec_tree=_Tmp8;
# 2504
return Cyc_NewControlFlow_anal_scs(env,inflow,scs,e->loc);}case 15: _TmpA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;_Tmp8=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp3)->f3;{struct Cyc_Absyn_Stmt*s1=_TmpA;struct Cyc_List_List*scs=_Tmp9;void*dec_tree=_Tmp8;
# 2509
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2517
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2520
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
# 2522
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,scs,0U);
if(scs_outflow.BottomFL.tag==1)
goto _LL5A;else{
Cyc_CfFlowInfo_aerr(s->loc,_tag_fat("last catch clause may implicitly fallthru",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));}_LL5A:;
# 2527
outflow=s1_outflow;
# 2529
return outflow;}}}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1)->r)->f2!=0){_TmpA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1->r)->f1;_Tmp9=(struct Cyc_List_List*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1->r)->f2->v;_Tmp8=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1->r)->f3;_Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1->loc;_Tmp7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Pat*letpat=_TmpA;struct Cyc_List_List*vds=_Tmp9;struct Cyc_Absyn_Exp*e=_Tmp8;unsigned loc=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp7;
# 2534
Cyc_fprintf(Cyc_stderr,_tag_fat("",sizeof(char),1U),_tag_fat(0U,sizeof(void*),0));{
# 2540
int isAliasPat=0;
{void*_TmpB=letpat->r;if(*((int*)_TmpB)==2){
isAliasPat=1;goto _LL5F;}else{
goto _LL5F;}_LL5F:;}{
# 2545
struct _tuple1 _TmpB=Cyc_NewControlFlow_get_unconsume_pat_vars(env,vds);void*_TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct Cyc_List_List*roots=_TmpD;struct Cyc_List_List*es=_TmpC;
inflow=Cyc_NewControlFlow_initialize_pat_vars(fenv,env,inflow,vds,roots!=0,loc,e->loc,isAliasPat);{
struct Cyc_List_List*old_unique_pat_vars=env->unique_pat_vars;
# 2549
({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple27*_Tmp10=({struct _tuple27*_Tmp11=_cycalloc(sizeof(struct _tuple27));_Tmp11->f0=0,_Tmp11->f1=s,_Tmp11->f2=roots,_Tmp11->f3=es;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=old_unique_pat_vars;_TmpF;});
# 2549
env->unique_pat_vars=_TmpE;});
# 2554
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2558
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,inflow,loc);
# 2562
return inflow;}}}}}}else{goto _LL26;}case 4: _TmpA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1->r)->f2;_Tmp9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1->r)->f3;_Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1->loc;_Tmp8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;if((struct Cyc_Absyn_Exp*)_Tmp9!=0){struct Cyc_Absyn_Vardecl*vd=_TmpA;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp9;unsigned loc=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp8;
# 2574
struct Cyc_List_List l=({struct Cyc_List_List _TmpB;_TmpB.hd=open_exp_opt,_TmpB.tl=0;_TmpB;});
union Cyc_CfFlowInfo_FlowInfo f=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple16 _TmpB=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,open_exp_opt);union Cyc_CfFlowInfo_AbsLVal _TmpC;_TmpC=_TmpB.f1;{union Cyc_CfFlowInfo_AbsLVal lval=_TmpC;
struct _tuple14 _TmpD=Cyc_NewControlFlow_anal_Rexp(env,1,f,open_exp_opt,0);union Cyc_CfFlowInfo_FlowInfo _TmpE;_TmpE=_TmpD.f0;{union Cyc_CfFlowInfo_FlowInfo f=_TmpE;
struct Cyc_List_List*roots=0;
struct Cyc_List_List*es=0;
{struct Cyc_Dict_Dict _TmpF;if(f.ReachableFL.tag==2){_TmpF=f.ReachableFL.val;{struct Cyc_Dict_Dict fd=_TmpF;
# 2582
{void*_Tmp10;if(lval.PlaceL.tag==1){_Tmp10=lval.PlaceL.val;{struct Cyc_CfFlowInfo_Place*p=_Tmp10;
# 2586
void*new_rval=Cyc_CfFlowInfo_lookup_place(fd,p);
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp11->tag=8,_Tmp11->f1=vd,_Tmp11->f2=new_rval;_Tmp11;});
fd=Cyc_CfFlowInfo_assign_place(fenv,open_exp_opt->loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct)),root->tag=0,root->f1=vd;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)root,rp->path=0;
roots=({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));_Tmp11->hd=rp,_Tmp11->tl=roots;_Tmp11;});
es=({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));_Tmp11->hd=open_exp_opt,_Tmp11->tl=es;_Tmp11;});
goto _LL72;}}}}else{
# 2601
goto _LL72;}_LL72:;}
# 2603
goto _LL6D;}}else{
# 2605
goto _LL6D;}_LL6D:;}{
# 2608
struct Cyc_List_List vds=({struct Cyc_List_List _TmpF;_TmpF.hd=vd,_TmpF.tl=0;_TmpF;});
f=Cyc_NewControlFlow_add_vars(fenv,f,& vds,fenv->unknown_all,loc,0);{
# 2612
struct Cyc_List_List*old_unique_pat_vars=env->unique_pat_vars;
({struct Cyc_List_List*_TmpF=({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple27*_Tmp11=({struct _tuple27*_Tmp12=_cycalloc(sizeof(struct _tuple27));_Tmp12->f0=1,_Tmp12->f1=s,_Tmp12->f2=roots,_Tmp12->f3=es;_Tmp12;});_Tmp10->hd=_Tmp11;}),_Tmp10->tl=old_unique_pat_vars;_Tmp10;});env->unique_pat_vars=_TmpF;});
# 2617
f=Cyc_NewControlFlow_anal_stmt(env,f,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2621
f=Cyc_NewControlFlow_unconsume_params(env,roots,es,1,f,loc);
# 2625
return f;}}}}}else{goto _LL26;}default: _LL26: _TmpA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Decl*d=_TmpA;struct Cyc_Absyn_Stmt*s=_Tmp9;
# 2628
outflow=({struct Cyc_NewControlFlow_AnalEnv*_TmpB=env;union Cyc_CfFlowInfo_FlowInfo _TmpC=Cyc_NewControlFlow_anal_decl(env,inflow,d);struct Cyc_Absyn_Stmt*_TmpD=s;Cyc_NewControlFlow_anal_stmt(_TmpB,_TmpC,_TmpD,rval_opt);});
if(Cyc_Flags_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,d);
return outflow;}}default: _TmpA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Stmt*s=_TmpA;
# 2633
return Cyc_NewControlFlow_anal_stmt(env,inflow,s,rval_opt);}};}}}
# 2639
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Fndecl*);
# 2643
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*fenv=env->fenv;
void*_Tmp0=decl->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 2651
if((int)vd->sc==0)
return inflow;{
# 2656
struct Cyc_List_List vds=({struct Cyc_List_List _Tmp4;_Tmp4.hd=vd,_Tmp4.tl=0;_Tmp4;});
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*e=vd->initializer;
if(e==0)
return inflow;{
struct _tuple14 _Tmp4=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e,0);void*_Tmp5;union Cyc_CfFlowInfo_FlowInfo _Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{union Cyc_CfFlowInfo_FlowInfo f=_Tmp6;void*r=_Tmp5;
return Cyc_NewControlFlow_do_initialize_var(fenv,env,f,vd,e,r,decl->loc);}}}}}case 3: _Tmp3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp3;
# 2665
return Cyc_NewControlFlow_add_vars(fenv,inflow,vds,fenv->unknown_none,decl->loc,0);}case 1: _Tmp3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp3;
# 2668
Cyc_NewControlFlow_check_nested_fun(env->all_tables,fenv,inflow,fd);{
struct Cyc_Absyn_Vardecl*vd=fd->fn_vardecl;
# 2673
struct Cyc_CfFlowInfo_FlowEnv*_Tmp4=fenv;union Cyc_CfFlowInfo_FlowInfo _Tmp5=inflow;struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=_check_null(vd),_Tmp7->tl=0;_Tmp7;});void*_Tmp7=fenv->unknown_all;return Cyc_NewControlFlow_add_vars(_Tmp4,_Tmp5,_Tmp6,_Tmp7,decl->loc,0);}}case 4: _Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp3;struct Cyc_Absyn_Vardecl*vd=_Tmp2;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp1;
# 2676
if(open_exp_opt!=0)
({int(*_Tmp4)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp4;})(_tag_fat("found open expression in declaration!",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List vds=({struct Cyc_List_List _Tmp4;_Tmp4.hd=vd,_Tmp4.tl=0;_Tmp4;});
return Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_all,decl->loc,0);}}default:
# 2682
({int(*_Tmp4)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp4;})(_tag_fat("anal_decl: unexpected decl variant",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));};}
# 2690
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_Tmp2=tables;struct Cyc_CfFlowInfo_FlowEnv*_Tmp3=fenv;union Cyc_CfFlowInfo_FlowInfo _Tmp4=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict);Cyc_NewControlFlow_check_nested_fun(_Tmp2,_Tmp3,_Tmp4,fd);});}
# 2692
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 2698
if(Cyc_Warn_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Dict_Absent_exn_struct));_Tmp4->tag=Cyc_Dict_Absent;_Tmp4;}));}}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}
# 2704
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2711
static struct Cyc_List_List*Cyc_NewControlFlow_get_noconsume_params(struct Cyc_List_List*aqb,struct Cyc_List_List*param_vardecls,struct Cyc_List_List*atts){
# 2714
struct _RegionHandle _Tmp0=_new_region(0U,"r");struct _RegionHandle*r=& _Tmp0;_push_region(r);
{int len=Cyc_List_length(param_vardecls);
struct _fat_ptr cons=({unsigned _Tmp1=(unsigned)len;_tag_fat(({int*_Tmp2=({struct _RegionHandle*_Tmp3=r;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(int)));});({{unsigned _Tmp3=(unsigned)len;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]=0;}}0;});_Tmp2;}),sizeof(int),_Tmp1);});
for(1;atts!=0;atts=atts->tl){
void*_Tmp1=(void*)atts->hd;int _Tmp2;switch(*((int*)_Tmp1)){case 22: _Tmp2=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_Tmp1)->f1;{int i=_Tmp2;
_Tmp2=i;goto _LL4;}case 21: _Tmp2=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_Tmp1)->f1;_LL4: {int i=_Tmp2;
*((int*)_check_fat_subscript(cons,sizeof(int),i - 1))=1;goto _LL0;}default:
 goto _LL0;}_LL0:;}{
# 2724
struct Cyc_List_List*noconsume_roots=0;
{int i=0;for(0;param_vardecls!=0;(param_vardecls=param_vardecls->tl,i ++)){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)param_vardecls->hd;
if(Cyc_Tcutil_is_noalias_pointer(aqb,vd->type,0)&& !(*((int*)_check_fat_subscript(cons,sizeof(int),i)))){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root;root=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct)),root->tag=0,root->f1=vd;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)root,rp->path=0;
noconsume_roots=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=rp,_Tmp1->tl=noconsume_roots;_Tmp1;});}}}}{
# 2733
struct Cyc_List_List*_Tmp1=noconsume_roots;_npop_handler(0);return _Tmp1;}}}
# 2715
;_pop_region();}
# 2736
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2740
unsigned loc=fd->body->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)_check_null(fd->param_vardecls)->v,fenv->unknown_all,loc,1);{
# 2745
struct Cyc_List_List*param_roots=0;
struct Cyc_Dict_Dict d=({union Cyc_CfFlowInfo_FlowInfo _Tmp0=inflow;if(_Tmp0.ReachableFL.tag!=2)_throw_match();_Tmp0.ReachableFL.val;});
# 2753
struct Cyc_List_List*aquals_bounds=Cyc_Tcutil_get_aquals_bounds(fd);
# 2756
struct Cyc_List_List*atts;
{void*_Tmp0=Cyc_Absyn_compress(_check_null(fd->cached_type));void*_Tmp1;if(*((int*)_Tmp0)==6){_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.attributes;{struct Cyc_List_List*as=_Tmp1;
atts=as;goto _LL0;}}else{
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("check_fun: non-function type cached with fndecl_t",sizeof(char),50U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}{
# 2761
struct Cyc_List_List*noconsume_roots=Cyc_NewControlFlow_get_noconsume_params(aquals_bounds,(struct Cyc_List_List*)_check_null(fd->param_vardecls)->v,atts);
# 2764
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;int _Tmp1;switch(*((int*)_Tmp0)){case 21: _Tmp1=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_Tmp0)->f1;{int i=_Tmp1;
# 2767
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(-1,0U);
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*(*_Tmp2)(struct Cyc_List_List*,int)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth;_Tmp2;})((struct Cyc_List_List*)fd->param_vardecls->v,i - 1);
void*t=Cyc_Absyn_compress(vd->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*rval=({
struct Cyc_CfFlowInfo_FlowEnv*_Tmp2=fenv;struct Cyc_List_List*_Tmp3=aquals_bounds;void*_Tmp4=elttype;struct Cyc_Absyn_Exp*_Tmp5=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_Tmp2,_Tmp3,_Tmp4,_Tmp5,-1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all),0);});
# 2775
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r;r=_cycalloc(sizeof(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct)),r->tag=2,r->f1=i,r->f2=t;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)r,rp->path=0;
d=Cyc_Dict_insert(d,(void*)r,rval);
d=({struct Cyc_Dict_Dict _Tmp2=d;void*_Tmp3=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_Tmp4->tag=0,_Tmp4->f1=vd;_Tmp4;});Cyc_Dict_insert(_Tmp2,_Tmp3,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp4->tag=4,_Tmp4->f1=rp;_Tmp4;}));});
goto _LL5;}}case 20: _Tmp1=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_Tmp0)->f1;{int i=_Tmp1;
# 2781
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*(*_Tmp2)(struct Cyc_List_List*,int)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth;_Tmp2;})((struct Cyc_List_List*)_check_null(fd->param_vardecls)->v,i - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(vd->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r;r=_cycalloc(sizeof(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct)),r->tag=2,r->f1=i,r->f2=elttype;{
struct Cyc_CfFlowInfo_Place*rp;rp=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),rp->root=(void*)r,rp->path=0;
d=({struct Cyc_Dict_Dict _Tmp2=d;void*_Tmp3=(void*)r;Cyc_Dict_insert(_Tmp2,_Tmp3,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});
d=({struct Cyc_Dict_Dict _Tmp2=d;void*_Tmp3=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct));_Tmp4->tag=0,_Tmp4->f1=vd;_Tmp4;});Cyc_Dict_insert(_Tmp2,_Tmp3,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct));_Tmp4->tag=4,_Tmp4->f1=rp;_Tmp4;}));});
param_roots=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=rp,_Tmp2->tl=param_roots;_Tmp2;});
goto _LL5;}}default:
 goto _LL5;}_LL5:;}
# 2792
inflow=Cyc_CfFlowInfo_ReachableFL(d);{
# 2794
int noreturn=Cyc_Atts_is_noreturn_fn_type(Cyc_Tcutil_fndecl2type(fd));
struct Cyc_Hashtable_Table*flow_table=
({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;_Tmp0;})(33,({int(*_Tmp0)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;_Tmp0;}),({int(*_Tmp0)(struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*))Cyc_NewControlFlow_hash_ptr;_Tmp0;}));
struct Cyc_NewControlFlow_AnalEnv*env;
env=_cycalloc(sizeof(struct Cyc_NewControlFlow_AnalEnv)),env->all_tables=tables,({
struct Cyc_Hashtable_Table*_Tmp0=({struct Cyc_Hashtable_Table*(*_Tmp1)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;_Tmp1;})(tables->succ_tables,fd);env->succ_table=_Tmp0;}),({
struct Cyc_Hashtable_Table*_Tmp0=({struct Cyc_Hashtable_Table*(*_Tmp1)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;_Tmp1;})(tables->pat_pop_tables,fd);env->pat_pop_table=_Tmp0;}),env->fenv=fenv,env->iterate_again=1,env->iteration_num=0,env->in_try=0,env->tryflow=inflow,env->noreturn=noreturn,env->return_type=fd->i.ret_type,env->unique_pat_vars=0,env->param_roots=param_roots,env->noconsume_params=noconsume_roots,env->flow_table=flow_table,env->aquals_bounds=aquals_bounds;{
# 2804
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2810
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);
# 2806
1U;}
# 2813
if(outflow.BottomFL.tag==1)
goto _LLC;else{
# 2816
Cyc_NewControlFlow_check_init_params(loc,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,loc);
# 2820
if(noreturn)
Cyc_CfFlowInfo_aerr(loc,_tag_fat("`noreturn' function might (implicitly) return",sizeof(char),46U),_tag_fat(0U,sizeof(void*),0));else{
if(!Cyc_Tcutil_is_void_type(fd->i.ret_type)){
if(Cyc_Tcutil_is_any_float_type(fd->i.ret_type)||
 Cyc_Tcutil_is_any_int_type(fd->i.ret_type))
Cyc_Warn_warn(loc,_tag_fat("function may not return a value",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));else{
# 2827
Cyc_CfFlowInfo_aerr(loc,_tag_fat("function may not return a value",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));}goto _LLC;}}
# 2829
goto _LLC;}_LLC:;}}}}}
# 2833
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(1;ds!=0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_Tmp0=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
Cyc_NewControlFlow_check_fun(tables,fd);goto _LL0;}case 11: _Tmp1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds2=_Tmp1;
# 2839
_Tmp1=ds2;goto _LL6;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL6: {struct Cyc_List_List*ds2=_Tmp1;
_Tmp1=ds2;goto _LL8;}case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL8: {struct Cyc_List_List*ds2=_Tmp1;
Cyc_NewControlFlow_cf_check(tables,ds2);goto _LL0;}case 12:
 goto _LL0;default:
 goto _LL0;}_LL0:;}}}

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
 struct Cyc_Core_Opt{void*v;};struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 68
extern void Cyc_Set_imp_insert(struct Cyc_Set_Set*,void*);
# 90
extern void*Cyc_Set_imp_delete(struct Cyc_Set_Set*,void*);
# 99
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 139
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 549 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_hash_qvar(struct _tuple0*);struct _tuple11{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;};
# 1227
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1241
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 79 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_LowTreeShake_Env{struct Cyc_Hashtable_Table*aggrdecls;struct Cyc_Set_Set*aggr_worklist;struct Cyc_Set_Set*aggr_keepers;struct Cyc_Set_Set*enum_keepers;struct Cyc_Hashtable_Table*global_keepers;struct Cyc_Hashtable_Table*funs_defined;};
# 47 "low_treeshake.cyc"
static void Cyc_LowTreeShake_build_tables_exp(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_LowTreeShake_build_tables_type(struct Cyc_LowTreeShake_Env*env,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;struct Cyc_List_List*_TA;void*_TB;void*_TC;struct _tuple11 _TD;int(*_TE)(struct Cyc_Set_Set*,struct _tuple0*);int(*_TF)(struct Cyc_Set_Set*,void*);struct Cyc_LowTreeShake_Env*_T10;struct Cyc_Set_Set*_T11;struct _tuple0*_T12;int _T13;void(*_T14)(struct Cyc_Set_Set*,struct _tuple0*);void(*_T15)(struct Cyc_Set_Set*,void*);struct Cyc_LowTreeShake_Env*_T16;struct Cyc_Set_Set*_T17;struct _tuple0*_T18;void*_T19;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1A;struct Cyc_List_List*_T1B;void*_T1C;void*_T1D;void(*_T1E)(struct Cyc_Set_Set*,struct _tuple0*);void(*_T1F)(struct Cyc_Set_Set*,void*);struct Cyc_LowTreeShake_Env*_T20;struct Cyc_Set_Set*_T21;struct _tuple0*_T22;void*_T23;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T24;struct Cyc_List_List*_T25;void*_T26;void*_T27;void*_T28;void*_T29;void*_T2A;unsigned _T2B;void*_T2C;struct Cyc_Absyn_PtrInfo _T2D;void*_T2E;struct Cyc_Absyn_ArrayInfo _T2F;struct Cyc_Absyn_ArrayInfo _T30;struct Cyc_Absyn_Exp*_T31;unsigned _T32;void*_T33;struct Cyc_LowTreeShake_Env*_T34;struct Cyc_Absyn_FnInfo _T35;void*_T36;struct Cyc_Absyn_FnInfo _T37;struct Cyc_LowTreeShake_Env*_T38;struct Cyc_List_List*_T39;void*_T3A;struct _tuple8*_T3B;struct _tuple8 _T3C;void*_T3D;struct Cyc_List_List*_T3E;void*_T3F;struct Cyc_Warn_String_Warn_Warg_struct _T40;struct Cyc_Warn_Typ_Warn_Warg_struct _T41;void**_T42;void**_T43;int(*_T44)(struct _fat_ptr);void*(*_T45)(struct _fat_ptr);struct _fat_ptr _T46;struct Cyc_Absyn_FnInfo _T47;struct Cyc_Absyn_Exp*_T48;void*_T49;union Cyc_Absyn_AggrInfo _T4A;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 24: _T8=t;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_TA=_T9->f2;if(_TA!=0)goto _TL2;_TB=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB;_TC=_T4B->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T4C=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TC;_T4A=_T4C->f1;}}{union Cyc_Absyn_AggrInfo ai=_T4A;_TD=
# 51
Cyc_Absyn_aggr_kinded_name(ai);{struct _tuple0*n=_TD.f1;_TF=Cyc_Set_member;{
int(*_T4B)(struct Cyc_Set_Set*,struct _tuple0*)=(int(*)(struct Cyc_Set_Set*,struct _tuple0*))_TF;_TE=_T4B;}_T10=env;_T11=_T10->aggr_keepers;_T12=n;_T13=_TE(_T11,_T12);if(_T13)goto _TL4;else{goto _TL6;}
_TL6: _T15=Cyc_Set_imp_insert;{void(*_T4B)(struct Cyc_Set_Set*,struct _tuple0*)=(void(*)(struct Cyc_Set_Set*,struct _tuple0*))_T15;_T14=_T4B;}_T16=env;_T17=_T16->aggr_worklist;_T18=n;_T14(_T17,_T18);goto _TL5;_TL4: _TL5: goto _LL0;}}_TL2: goto _LL15;case 19: _T19=t;_T1A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T19;_T1B=_T1A->f2;if(_T1B!=0)goto _TL7;_T1C=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1C;_T1D=_T4B->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T4C=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T1D;_T49=_T4C->f1;}}{struct _tuple0*n=_T49;_T1F=Cyc_Set_imp_insert;{
# 56
void(*_T4B)(struct Cyc_Set_Set*,struct _tuple0*)=(void(*)(struct Cyc_Set_Set*,struct _tuple0*))_T1F;_T1E=_T4B;}_T20=env;_T21=_T20->enum_keepers;_T22=n;_T1E(_T21,_T22);goto _LL0;}_TL7: goto _LL15;case 3: goto _LL8;default: _T23=t;_T24=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T23;_T25=_T24->f2;if(_T25!=0)goto _TL9;_LL8: goto _LL0;_TL9: goto _LL15;};case 1: _T26=t;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T4B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T26;_T27=_T4B->f2;_T49=(void*)_T27;}{void*topt=_T49;_T49=topt;goto _LLC;}case 8: _T28=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T4B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T28;_T29=_T4B->f4;_T49=(void*)_T29;}_LLC: {void*topt=_T49;_T2A=topt;_T2B=(unsigned)_T2A;
# 61
if(!_T2B)goto _TLB;_T49=topt;goto _LLE;_TLB: goto _LL0;}case 4: _T2C=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2C;_T2D=_T4B->f1;_T49=_T2D.elt_type;}_LLE: {void*t1=_T49;
Cyc_LowTreeShake_build_tables_type(env,t1);goto _LL0;}case 5: _T2E=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T4B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T2E;_T2F=_T4B->f1;_T49=_T2F.elt_type;_T30=_T4B->f1;_T48=_T30.num_elts;}{void*t1=_T49;struct Cyc_Absyn_Exp*eo=_T48;_T31=eo;_T32=(unsigned)_T31;
# 65
if(!_T32)goto _TLD;Cyc_LowTreeShake_build_tables_exp(env,eo);goto _TLE;_TLD: _TLE:
 Cyc_LowTreeShake_build_tables_type(env,t1);goto _LL0;}case 6: _T33=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T33;_T47=_T4B->f1;}{struct Cyc_Absyn_FnInfo i=_T47;_T34=env;_T35=i;_T36=_T35.ret_type;
# 70
Cyc_LowTreeShake_build_tables_type(_T34,_T36);_T37=i;{
struct Cyc_List_List*args=_T37.args;_TL12: if(args!=0)goto _TL10;else{goto _TL11;}
_TL10: _T38=env;_T39=args;_T3A=_T39->hd;_T3B=(struct _tuple8*)_T3A;_T3C=*_T3B;_T3D=_T3C.f2;Cyc_LowTreeShake_build_tables_type(_T38,_T3D);_T3E=args;
# 71
args=_T3E->tl;goto _TL12;_TL11:;}goto _LL0;}case 11: _T3F=t;{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T4B=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T3F;_T48=_T4B->f1;}{struct Cyc_Absyn_Exp*e=_T48;
# 75
Cyc_LowTreeShake_build_tables_exp(env,e);goto _LL0;}default: _LL15:{struct Cyc_Warn_String_Warn_Warg_struct _T4B;_T4B.tag=0;
# 77
_T4B.f1=_tag_fat("LowTreeshake: unexpected type ",sizeof(char),31U);_T40=_T4B;}{struct Cyc_Warn_String_Warn_Warg_struct _T4B=_T40;{struct Cyc_Warn_Typ_Warn_Warg_struct _T4C;_T4C.tag=2;_T4C.f1=t;_T41=_T4C;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T4C=_T41;void*_T4D[2];_T42=_T4D + 0;*_T42=& _T4B;_T43=_T4D + 1;*_T43=& _T4C;_T45=Cyc_Warn_impos2;{int(*_T4E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T45;_T44=_T4E;}_T46=_tag_fat(_T4D,sizeof(void*),2);_T44(_T46);}}}_LL0:;}
# 81
static int Cyc_LowTreeShake_build_tables_visit_f1(struct Cyc_LowTreeShake_Env*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;struct Cyc_Absyn_Vardecl*_T8;void*_T9;struct Cyc_Absyn_Fndecl*_TA;void*_TB;int*(*_TC)(struct Cyc_Hashtable_Table*,struct _tuple0*);void**(*_TD)(struct Cyc_Hashtable_Table*,void*);struct Cyc_LowTreeShake_Env*_TE;struct Cyc_Hashtable_Table*_TF;struct _tuple0*_T10;int*_T11;unsigned _T12;int*(*_T13)(struct Cyc_Hashtable_Table*,struct _tuple0*);void**(*_T14)(struct Cyc_Hashtable_Table*,void*);struct Cyc_LowTreeShake_Env*_T15;struct Cyc_Hashtable_Table*_T16;struct _tuple0*_T17;int*_T18;unsigned _T19;void(*_T1A)(struct Cyc_Hashtable_Table*,struct _tuple0*,int);void(*_T1B)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_LowTreeShake_Env*_T1C;struct Cyc_Hashtable_Table*_T1D;struct _tuple0*_T1E;void(*_T1F)(struct Cyc_Set_Set*,struct _tuple0*);void(*_T20)(struct Cyc_Set_Set*,void*);struct Cyc_LowTreeShake_Env*_T21;struct Cyc_Set_Set*_T22;struct Cyc_Absyn_Enumdecl*_T23;struct _tuple0*_T24;void*_T25;void*_T26;void*_T27;_T0=e;{
void*_T28=_T0->r;void*_T29;_T1=(int*)_T28;_T2=*_T1;switch(_T2){case 1: _T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T28;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T2A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T28;_T7=_T2A->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T7;_T29=_T2B->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T29;_T8=vd;_T29=_T8->name;goto _LL4;}case 2:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T2A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T28;_T9=_T2A->f1;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T9;_TA=_T2B->f1;{struct Cyc_Absyn_Fndecl _T2C=*_TA;_T29=_T2C.name;}}}_LL4: {struct _tuple0*qv=_T29;_T29=qv;goto _LL6;}case 0:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T2A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T28;_TB=_T2A->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_TB;_T29=_T2B->f1;}}_LL6: {struct _tuple0*qv=_T29;_TD=Cyc_Hashtable_lookup_opt;{
# 86
int*(*_T2A)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(int*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))_TD;_TC=_T2A;}_TE=env;_TF=_TE->funs_defined;_T10=qv;_T11=_TC(_TF,_T10);_T12=(unsigned)_T11;if(_T12)goto _TL15;else{goto _TL17;}_TL17: _T14=Cyc_Hashtable_lookup_opt;{
int*(*_T2A)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(int*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))_T14;_T13=_T2A;}_T15=env;_T16=_T15->global_keepers;_T17=qv;_T18=_T13(_T16,_T17);_T19=(unsigned)_T18;
# 86
if(_T19)goto _TL15;else{goto _TL18;}
# 88
_TL18: _T1B=Cyc_Hashtable_insert;{void(*_T2A)(struct Cyc_Hashtable_Table*,struct _tuple0*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,int))_T1B;_T1A=_T2A;}_T1C=env;_T1D=_T1C->global_keepers;_T1E=qv;_T1A(_T1D,_T1E,1);goto _TL16;_TL15: _TL16:
 return 0;}default: goto _LLF;};case 31:{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T2A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_T28;_T29=_T2A->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T29;_T20=Cyc_Set_imp_insert;{
void(*_T2A)(struct Cyc_Set_Set*,struct _tuple0*)=(void(*)(struct Cyc_Set_Set*,struct _tuple0*))_T20;_T1F=_T2A;}_T21=env;_T22=_T21->enum_keepers;_T23=ed;_T24=_T23->name;_T1F(_T22,_T24);return 0;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T2A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T28;_T25=_T2A->f1;_T29=(void*)_T25;}{void*t=_T29;_T29=t;goto _LLC;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T2A=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T28;_T26=_T2A->f1;_T29=(void*)_T26;}_LLC: {void*t=_T29;_T29=t;goto _LLE;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T2A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T28;_T27=_T2A->f1;_T29=(void*)_T27;}_LLE: {void*t=_T29;
# 93
Cyc_LowTreeShake_build_tables_type(env,t);return 1;}default: _LLF:
 return 1;};}}
# 97
static int Cyc_LowTreeShake_build_tables_visit_f2(struct Cyc_LowTreeShake_Env*env,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;int _T2;struct Cyc_Absyn_Decl*_T3;int*_T4;unsigned _T5;struct Cyc_LowTreeShake_Env*_T6;struct Cyc_Absyn_Vardecl*_T7;void*_T8;struct Cyc_LowTreeShake_Env*_T9;struct Cyc_Absyn_Fndecl*_TA;struct Cyc_Absyn_FnInfo _TB;void*_TC;struct Cyc_Absyn_Fndecl*_TD;struct Cyc_Absyn_FnInfo _TE;struct Cyc_LowTreeShake_Env*_TF;struct Cyc_List_List*_T10;void*_T11;struct _tuple8*_T12;struct _tuple8 _T13;void*_T14;struct Cyc_List_List*_T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;void**_T17;int(*_T18)(struct _fat_ptr);void*(*_T19)(struct _fat_ptr);struct _fat_ptr _T1A;_T0=s;{
void*_T1B=_T0->r;struct Cyc_Absyn_Stmt*_T1C;struct Cyc_Absyn_Decl*_T1D;_T1=(int*)_T1B;_T2=*_T1;if(_T2!=12)goto _TL19;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T1E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T1B;_T1D=_T1E->f1;_T1C=_T1E->f2;}{struct Cyc_Absyn_Decl*d=_T1D;struct Cyc_Absyn_Stmt*s1=_T1C;_T3=d;{
# 100
void*_T1E=_T3->r;struct Cyc_Absyn_Fndecl*_T1F;struct Cyc_Absyn_Vardecl*_T20;_T4=(int*)_T1E;_T5=*_T4;switch(_T5){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T21=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1E;_T20=_T21->f1;}{struct Cyc_Absyn_Vardecl*vd=_T20;_T6=env;_T7=vd;_T8=_T7->type;
# 102
Cyc_LowTreeShake_build_tables_type(_T6,_T8);
return 1;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T21=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1E;_T1F=_T21->f1;}{struct Cyc_Absyn_Fndecl*fd=_T1F;_T9=env;_TA=fd;_TB=_TA->i;_TC=_TB.ret_type;
# 105
Cyc_LowTreeShake_build_tables_type(_T9,_TC);_TD=fd;_TE=_TD->i;{
struct Cyc_List_List*args=_TE.args;_TL1F: if(args!=0)goto _TL1D;else{goto _TL1E;}
_TL1D: _TF=env;_T10=args;_T11=_T10->hd;_T12=(struct _tuple8*)_T11;_T13=*_T12;_T14=_T13.f2;Cyc_LowTreeShake_build_tables_type(_TF,_T14);_T15=args;
# 106
args=_T15->tl;goto _TL1F;_TL1E:;}
# 108
return 1;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T21;_T21.tag=0;
_T21.f1=_tag_fat("LowTreeShake: local decl not a var or function",sizeof(char),47U);_T16=_T21;}{struct Cyc_Warn_String_Warn_Warg_struct _T21=_T16;void*_T22[1];_T17=_T22 + 0;*_T17=& _T21;_T19=Cyc_Warn_impos2;{int(*_T23)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T19;_T18=_T23;}_T1A=_tag_fat(_T22,sizeof(void*),1);_T18(_T1A);}};}}goto _TL1A;_TL19:
# 111
 return 1;_TL1A:;}}
# 114
static void Cyc_LowTreeShake_build_tables_exp(struct Cyc_LowTreeShake_Env*env,struct Cyc_Absyn_Exp*e){void(*_T0)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_LowTreeShake_Env*_T2;struct Cyc_Absyn_Exp*_T3;_T1=Cyc_Absyn_visit_exp;{
void(*_T4)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*))_T1;_T0=_T4;}_T2=env;_T3=e;_T0(Cyc_LowTreeShake_build_tables_visit_f1,Cyc_LowTreeShake_build_tables_visit_f2,_T2,_T3);}
# 118
static void Cyc_LowTreeShake_add_to_table(struct Cyc_Hashtable_Table*tab,struct _tuple0*qv,void*d){struct Cyc_List_List**(*_T0)(struct Cyc_Hashtable_Table*,struct _tuple0*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_List_List*_T2;struct Cyc_List_List**_T3;unsigned _T4;struct Cyc_List_List*_T5;struct Cyc_List_List**_T6;struct Cyc_List_List*_T7;void(*_T8)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_List_List*);void(*_T9)(struct Cyc_Hashtable_Table*,void*,void*);_T1=Cyc_Hashtable_lookup_opt;{
# 120
struct Cyc_List_List**(*_TA)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(struct Cyc_List_List**(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))_T1;_T0=_TA;}{struct Cyc_List_List**lst_opt=_T0(tab,qv);
struct Cyc_List_List*lst;lst=_cycalloc(sizeof(struct Cyc_List_List));_T2=lst;_T2->hd=d;_T3=lst_opt;_T4=(unsigned)_T3;if(!_T4)goto _TL20;_T5=lst;_T6=lst_opt;_T5->tl=*_T6;goto _TL21;_TL20: _T7=lst;_T7->tl=0;_TL21: _T9=Cyc_Hashtable_insert;{
void(*_TA)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_List_List*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_List_List*))_T9;_T8=_TA;}_T8(tab,qv,lst);}}
# 125
static void Cyc_LowTreeShake_build_tables(struct Cyc_LowTreeShake_Env*env,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_LowTreeShake_Env*_T5;struct Cyc_Absyn_Vardecl*_T6;void*_T7;struct Cyc_Absyn_Vardecl*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_LowTreeShake_Env*_TA;struct Cyc_Absyn_Vardecl*_TB;struct Cyc_Absyn_Exp*_TC;void(*_TD)(struct Cyc_Hashtable_Table*,struct _tuple0*,int);void(*_TE)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_LowTreeShake_Env*_TF;struct Cyc_Hashtable_Table*_T10;struct Cyc_Absyn_Fndecl*_T11;struct _tuple0*_T12;struct Cyc_LowTreeShake_Env*_T13;struct Cyc_Absyn_Fndecl*_T14;struct Cyc_Absyn_FnInfo _T15;void*_T16;struct Cyc_Absyn_Fndecl*_T17;struct Cyc_Absyn_FnInfo _T18;struct Cyc_LowTreeShake_Env*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct _tuple8*_T1C;struct _tuple8 _T1D;void*_T1E;struct Cyc_List_List*_T1F;void(*_T20)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*);void(*_T21)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_LowTreeShake_Env*_T22;struct Cyc_Absyn_Fndecl*_T23;struct Cyc_Absyn_Stmt*_T24;void(*_T25)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*);struct Cyc_LowTreeShake_Env*_T26;struct Cyc_Hashtable_Table*_T27;struct Cyc_Absyn_Aggrdecl*_T28;struct _tuple0*_T29;struct Cyc_Absyn_Aggrdecl*_T2A;struct Cyc_Absyn_Enumdecl*_T2B;struct Cyc_Core_Opt*_T2C;struct Cyc_Absyn_Enumdecl*_T2D;struct Cyc_Core_Opt*_T2E;void*_T2F;struct Cyc_List_List*_T30;void*_T31;struct Cyc_Absyn_Enumfield*_T32;struct Cyc_Absyn_Exp*_T33;struct Cyc_LowTreeShake_Env*_T34;struct Cyc_List_List*_T35;void*_T36;struct Cyc_Absyn_Enumfield*_T37;struct Cyc_Absyn_Exp*_T38;struct Cyc_List_List*_T39;struct Cyc_LowTreeShake_Env*_T3A;struct Cyc_Absyn_Typedefdecl*_T3B;void*_T3C;void*_T3D;struct Cyc_Warn_String_Warn_Warg_struct _T3E;void**_T3F;int(*_T40)(struct _fat_ptr);void*(*_T41)(struct _fat_ptr);struct _fat_ptr _T42;struct Cyc_List_List*_T43;
_TL25: if(tds!=0)goto _TL23;else{goto _TL24;}
_TL23: _T0=tds;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_T44=_T2->r;struct Cyc_Absyn_Typedefdecl*_T45;struct Cyc_Absyn_Enumdecl*_T46;struct Cyc_Absyn_Aggrdecl*_T47;struct Cyc_Absyn_Fndecl*_T48;struct Cyc_Absyn_Vardecl*_T49;_T3=(int*)_T44;_T4=*_T3;switch(_T4){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T44;_T49=_T4A->f1;}{struct Cyc_Absyn_Vardecl*vd=_T49;_T5=env;_T6=vd;_T7=_T6->type;
# 129
Cyc_LowTreeShake_build_tables_type(_T5,_T7);_T8=vd;_T9=_T8->initializer;
if(_T9==0)goto _TL27;_TA=env;_TB=vd;_TC=_TB->initializer;
Cyc_LowTreeShake_build_tables_exp(_TA,_TC);goto _TL28;_TL27: _TL28: goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T44;_T48=_T4A->f1;}{struct Cyc_Absyn_Fndecl*fd=_T48;_TE=Cyc_Hashtable_insert;{
# 134
void(*_T4A)(struct Cyc_Hashtable_Table*,struct _tuple0*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,int))_TE;_TD=_T4A;}_TF=env;_T10=_TF->funs_defined;_T11=fd;_T12=_T11->name;_TD(_T10,_T12,1);_T13=env;_T14=fd;_T15=_T14->i;_T16=_T15.ret_type;
Cyc_LowTreeShake_build_tables_type(_T13,_T16);_T17=fd;_T18=_T17->i;{
struct Cyc_List_List*args=_T18.args;_TL2C: if(args!=0)goto _TL2A;else{goto _TL2B;}
_TL2A: _T19=env;_T1A=args;_T1B=_T1A->hd;_T1C=(struct _tuple8*)_T1B;_T1D=*_T1C;_T1E=_T1D.f2;Cyc_LowTreeShake_build_tables_type(_T19,_T1E);_T1F=args;
# 136
args=_T1F->tl;goto _TL2C;_TL2B:;}_T21=Cyc_Absyn_visit_stmt;{
# 138
void(*_T4A)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*))_T21;_T20=_T4A;}_T22=env;_T23=fd;_T24=_T23->body;_T20(Cyc_LowTreeShake_build_tables_visit_f1,Cyc_LowTreeShake_build_tables_visit_f2,_T22,_T24);goto _LL0;}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T44;_T47=_T4A->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T47;{
# 140
void(*_T4A)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_LowTreeShake_add_to_table;_T25=_T4A;}_T26=env;_T27=_T26->aggrdecls;_T28=ad;_T29=_T28->name;_T2A=ad;_T25(_T27,_T29,_T2A);goto _LL0;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T44;_T46=_T4A->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T46;_T2B=ed;_T2C=_T2B->fields;
# 143
if(_T2C==0)goto _TL2D;_T2D=ed;_T2E=_T2D->fields;_T2F=_T2E->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T2F;_TL32: if(fs!=0)goto _TL30;else{goto _TL31;}
_TL30: _T30=fs;_T31=_T30->hd;_T32=(struct Cyc_Absyn_Enumfield*)_T31;_T33=_T32->tag;if(_T33==0)goto _TL33;_T34=env;_T35=fs;_T36=_T35->hd;_T37=(struct Cyc_Absyn_Enumfield*)_T36;_T38=_T37->tag;
Cyc_LowTreeShake_build_tables_exp(_T34,_T38);goto _TL34;_TL33: _TL34: _T39=fs;
# 144
fs=_T39->tl;goto _TL32;_TL31:;}goto _TL2E;_TL2D: _TL2E: goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T4A=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T44;_T45=_T4A->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T45;_T3A=env;_T3B=td;_T3C=_T3B->defn;_T3D=
# 149
_check_null(_T3C);Cyc_LowTreeShake_build_tables_type(_T3A,_T3D);goto _LL0;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T4A;_T4A.tag=0;
_T4A.f1=_tag_fat("LowTreeShake: top-level Cyclone decl",sizeof(char),37U);_T3E=_T4A;}{struct Cyc_Warn_String_Warn_Warg_struct _T4A=_T3E;void*_T4B[1];_T3F=_T4B + 0;*_T3F=& _T4A;_T41=Cyc_Warn_impos2;{int(*_T4C)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T41;_T40=_T4C;}_T42=_tag_fat(_T4B,sizeof(void*),1);_T40(_T42);}}_LL0:;}_T43=tds;
# 126
tds=_T43->tl;goto _TL25;_TL24:;}
# 154
static void Cyc_LowTreeShake_process_worklist(struct Cyc_LowTreeShake_Env*env){struct Cyc_LowTreeShake_Env*_T0;struct Cyc_Set_Set*_T1;int _T2;struct _tuple0*(*_T3)(struct Cyc_Set_Set*);void*(*_T4)(struct Cyc_Set_Set*);struct Cyc_LowTreeShake_Env*_T5;struct Cyc_Set_Set*_T6;struct _tuple0*(*_T7)(struct Cyc_Set_Set*,struct _tuple0*);void*(*_T8)(struct Cyc_Set_Set*,void*);struct Cyc_LowTreeShake_Env*_T9;struct Cyc_Set_Set*_TA;struct _tuple0*_TB;void(*_TC)(struct Cyc_Set_Set*,struct _tuple0*);void(*_TD)(struct Cyc_Set_Set*,void*);struct Cyc_LowTreeShake_Env*_TE;struct Cyc_Set_Set*_TF;struct _tuple0*_T10;struct Cyc_List_List**(*_T11)(struct Cyc_Hashtable_Table*,struct _tuple0*);void**(*_T12)(struct Cyc_Hashtable_Table*,void*);struct Cyc_LowTreeShake_Env*_T13;struct Cyc_Hashtable_Table*_T14;struct _tuple0*_T15;struct Cyc_List_List**_T16;unsigned _T17;struct Cyc_List_List**_T18;struct Cyc_List_List*_T19;void*_T1A;struct Cyc_Absyn_Aggrdecl*_T1B;struct Cyc_Absyn_AggrdeclImpl*_T1C;struct Cyc_List_List*_T1D;void*_T1E;struct Cyc_Absyn_Aggrdecl*_T1F;struct Cyc_Absyn_AggrdeclImpl*_T20;struct Cyc_LowTreeShake_Env*_T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_Absyn_Aggrfield*_T24;void*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;
# 156
_TL35: _T0=env;_T1=_T0->aggr_worklist;_T2=Cyc_Set_is_empty(_T1);if(_T2)goto _TL37;else{goto _TL36;}
_TL36: _T4=Cyc_Set_choose;{struct _tuple0*(*_T28)(struct Cyc_Set_Set*)=(struct _tuple0*(*)(struct Cyc_Set_Set*))_T4;_T3=_T28;}_T5=env;_T6=_T5->aggr_worklist;{struct _tuple0*n=_T3(_T6);_T8=Cyc_Set_imp_delete;{
struct _tuple0*(*_T28)(struct Cyc_Set_Set*,struct _tuple0*)=(struct _tuple0*(*)(struct Cyc_Set_Set*,struct _tuple0*))_T8;_T7=_T28;}_T9=env;_TA=_T9->aggr_worklist;_TB=n;_T7(_TA,_TB);_TD=Cyc_Set_imp_insert;{
void(*_T28)(struct Cyc_Set_Set*,struct _tuple0*)=(void(*)(struct Cyc_Set_Set*,struct _tuple0*))_TD;_TC=_T28;}_TE=env;_TF=_TE->aggr_keepers;_T10=n;_TC(_TF,_T10);_T12=Cyc_Hashtable_lookup_opt;{
# 161
struct Cyc_List_List**(*_T28)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(struct Cyc_List_List**(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))_T12;_T11=_T28;}_T13=env;_T14=_T13->aggrdecls;_T15=n;{struct Cyc_List_List**ads_opt=_T11(_T14,_T15);_T16=ads_opt;_T17=(unsigned)_T16;
if(_T17)goto _TL38;else{goto _TL3A;}
_TL3A: goto _TL35;_TL38: _T18=ads_opt;{
struct Cyc_List_List*ads=*_T18;_TL3E: if(ads!=0)goto _TL3C;else{goto _TL3D;}
_TL3C: _T19=ads;_T1A=_T19->hd;_T1B=(struct Cyc_Absyn_Aggrdecl*)_T1A;_T1C=_T1B->impl;if(_T1C==0)goto _TL3F;_T1D=ads;_T1E=_T1D->hd;_T1F=(struct Cyc_Absyn_Aggrdecl*)_T1E;_T20=_T1F->impl;{
struct Cyc_List_List*fs=_T20->fields;_TL44: if(fs!=0)goto _TL42;else{goto _TL43;}
_TL42: _T21=env;_T22=fs;_T23=_T22->hd;_T24=(struct Cyc_Absyn_Aggrfield*)_T23;_T25=_T24->type;Cyc_LowTreeShake_build_tables_type(_T21,_T25);_T26=fs;
# 166
fs=_T26->tl;goto _TL44;_TL43:;}goto _TL40;_TL3F: _TL40: _T27=ads;
# 164
ads=_T27->tl;goto _TL3E;_TL3D:;}}}goto _TL35;_TL37:;}
# 171
static struct Cyc_List_List*Cyc_LowTreeShake_trim_decls(struct Cyc_LowTreeShake_Env*env,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;int _T5;struct Cyc_Absyn_Vardecl*_T6;enum Cyc_Absyn_Scope _T7;int _T8;struct Cyc_Absyn_Vardecl*_T9;int _TA;int*(*_TB)(struct Cyc_Hashtable_Table*,struct _tuple0*);void**(*_TC)(struct Cyc_Hashtable_Table*,void*);struct Cyc_LowTreeShake_Env*_TD;struct Cyc_Hashtable_Table*_TE;struct Cyc_Absyn_Vardecl*_TF;struct _tuple0*_T10;int*_T11;int(*_T12)(struct Cyc_Set_Set*,struct _tuple0*);int(*_T13)(struct Cyc_Set_Set*,void*);struct Cyc_LowTreeShake_Env*_T14;struct Cyc_Set_Set*_T15;struct Cyc_Absyn_Aggrdecl*_T16;struct _tuple0*_T17;int(*_T18)(struct Cyc_Set_Set*,struct _tuple0*);int(*_T19)(struct Cyc_Set_Set*,void*);struct Cyc_LowTreeShake_Env*_T1A;struct Cyc_Set_Set*_T1B;struct Cyc_Absyn_Enumdecl*_T1C;struct _tuple0*_T1D;int _T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;
struct Cyc_List_List*ans=0;
_TL48: if(tds!=0)goto _TL46;else{goto _TL47;}
_TL46:{int keep;_T0=tds;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{
void*_T24=_T2->r;struct Cyc_Absyn_Enumdecl*_T25;struct Cyc_Absyn_Aggrdecl*_T26;struct Cyc_Absyn_Vardecl*_T27;_T3=(int*)_T24;_T4=*_T3;switch(_T4){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T28=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T24;_T27=_T28->f1;}{struct Cyc_Absyn_Vardecl*vd=_T27;_T6=vd;_T7=_T6->sc;_T8=(int)_T7;
if(_T8==3)goto _TL4A;_T9=vd;_TA=_T9->is_proto;if(_TA==1)goto _TL4A;_T5=1;goto _TL4B;_TL4A: _TC=Cyc_Hashtable_lookup_opt;{
int*(*_T28)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(int*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))_TC;_TB=_T28;}_TD=env;_TE=_TD->global_keepers;_TF=vd;_T10=_TF->name;_T11=_TB(_TE,_T10);_T5=(unsigned)_T11;_TL4B:
# 176
 keep=_T5;goto _LL0;}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T28=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T24;_T26=_T28->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T26;_T13=Cyc_Set_member;{
# 179
int(*_T28)(struct Cyc_Set_Set*,struct _tuple0*)=(int(*)(struct Cyc_Set_Set*,struct _tuple0*))_T13;_T12=_T28;}_T14=env;_T15=_T14->aggr_keepers;_T16=ad;_T17=_T16->name;keep=_T12(_T15,_T17);goto _LL0;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T28=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T24;_T25=_T28->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T25;_T19=Cyc_Set_member;{
int(*_T28)(struct Cyc_Set_Set*,struct _tuple0*)=(int(*)(struct Cyc_Set_Set*,struct _tuple0*))_T19;_T18=_T28;}_T1A=env;_T1B=_T1A->enum_keepers;_T1C=ed;_T1D=_T1C->name;keep=_T18(_T1B,_T1D);goto _LL0;}default:
 keep=1;goto _LL0;}_LL0:;}_T1E=keep;
# 183
if(!_T1E)goto _TL4C;{struct Cyc_List_List*_T24=_cycalloc(sizeof(struct Cyc_List_List));_T20=tds;_T21=_T20->hd;
_T24->hd=(struct Cyc_Absyn_Decl*)_T21;_T24->tl=ans;_T1F=(struct Cyc_List_List*)_T24;}ans=_T1F;goto _TL4D;_TL4C: _TL4D:;}_T22=tds;
# 173
tds=_T22->tl;goto _TL48;_TL47: _T23=
# 186
Cyc_List_imp_rev(ans);return _T23;}
# 189
struct Cyc_List_List*Cyc_LowTreeShake_shake(struct Cyc_List_List*tds){struct Cyc_LowTreeShake_Env _T0;struct Cyc_Hashtable_Table*(*_T1)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*));struct Cyc_Hashtable_Table*(*_T2)(int,int(*)(void*,void*),int(*)(void*));int(*_T3)(struct _tuple0*,struct _tuple0*);int(*_T4)(struct _tuple0*);struct Cyc_Set_Set*(*_T5)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Set_Set*(*_T6)(int(*)(void*,void*));int(*_T7)(struct _tuple0*,struct _tuple0*);struct Cyc_Set_Set*(*_T8)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Set_Set*(*_T9)(int(*)(void*,void*));int(*_TA)(struct _tuple0*,struct _tuple0*);struct Cyc_Set_Set*(*_TB)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Set_Set*(*_TC)(int(*)(void*,void*));int(*_TD)(struct _tuple0*,struct _tuple0*);struct Cyc_Hashtable_Table*(*_TE)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*));struct Cyc_Hashtable_Table*(*_TF)(int,int(*)(void*,void*),int(*)(void*));int(*_T10)(struct _tuple0*,struct _tuple0*);int(*_T11)(struct _tuple0*);struct Cyc_Hashtable_Table*(*_T12)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*));struct Cyc_Hashtable_Table*(*_T13)(int,int(*)(void*,void*),int(*)(void*));int(*_T14)(struct _tuple0*,struct _tuple0*);int(*_T15)(struct _tuple0*);struct Cyc_LowTreeShake_Env*_T16;struct Cyc_LowTreeShake_Env*_T17;struct Cyc_List_List*_T18;struct Cyc_LowTreeShake_Env*_T19;struct Cyc_LowTreeShake_Env*_T1A;struct Cyc_LowTreeShake_Env*_T1B;struct Cyc_LowTreeShake_Env*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;{struct Cyc_LowTreeShake_Env _T1F;_T2=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T20)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))_T2;_T1=_T20;}_T3=Cyc_Absyn_qvar_cmp;_T4=Cyc_Absyn_hash_qvar;_T1F.aggrdecls=_T1(37,_T3,_T4);_T6=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T20)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T6;_T5=_T20;}_T7=Cyc_Absyn_qvar_cmp;_T1F.aggr_worklist=_T5(_T7);_T9=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T20)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T9;_T8=_T20;}_TA=Cyc_Absyn_qvar_cmp;_T1F.aggr_keepers=_T8(_TA);_TC=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T20)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))_TC;_TB=_T20;}_TD=Cyc_Absyn_qvar_cmp;_T1F.enum_keepers=_TB(_TD);_TF=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T20)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))_TF;_TE=_T20;}_T10=Cyc_Absyn_qvar_cmp;_T11=Cyc_Absyn_hash_qvar;_T1F.global_keepers=_TE(37,_T10,_T11);_T13=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T20)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))_T13;_T12=_T20;}_T14=Cyc_Absyn_qvar_cmp;_T15=Cyc_Absyn_hash_qvar;_T1F.funs_defined=_T12(37,_T14,_T15);_T0=_T1F;}{
# 190
struct Cyc_LowTreeShake_Env env=_T0;_T16=& env;_T17=(struct Cyc_LowTreeShake_Env*)_T16;_T18=tds;
# 196
Cyc_LowTreeShake_build_tables(_T17,_T18);_T19=& env;_T1A=(struct Cyc_LowTreeShake_Env*)_T19;
Cyc_LowTreeShake_process_worklist(_T1A);_T1B=& env;_T1C=(struct Cyc_LowTreeShake_Env*)_T1B;_T1D=tds;_T1E=
Cyc_LowTreeShake_trim_decls(_T1C,_T1D);return _T1E;}}

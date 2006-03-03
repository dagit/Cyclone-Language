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
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 168
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 554 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 923 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_hash_qvar(struct _tuple0*);struct _tuple11{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;};
# 1229
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1243
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_LowTreeShake_Env{struct Cyc_Hashtable_Table*aggrdecls;struct Cyc_Set_Set*aggr_worklist;struct Cyc_Set_Set*aggr_keepers;struct Cyc_Set_Set*enum_keepers;struct Cyc_Hashtable_Table*global_keepers;struct Cyc_Hashtable_Table*funs_defined;};
# 47 "low_treeshake.cyc"
static void Cyc_LowTreeShake_build_tables_exp(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_LowTreeShake_build_tables_type(struct Cyc_LowTreeShake_Env*env,void*t){
struct Cyc_Absyn_FnInfo _Tmp0;void*_Tmp1;void*_Tmp2;union Cyc_Absyn_AggrInfo _Tmp3;switch(*((int*)t)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 24: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2==0){_Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{union Cyc_Absyn_AggrInfo ai=_Tmp3;
# 51
struct _tuple0*n=Cyc_Absyn_aggr_kinded_name(ai).f1;
if(!({int(*_Tmp4)(struct Cyc_Set_Set*,struct _tuple0*)=(int(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_member;_Tmp4;})(env->aggr_keepers,n))
({void(*_Tmp4)(struct Cyc_Set_Set*,struct _tuple0*)=(void(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_insert;_Tmp4;})(env->aggr_worklist,n);
goto _LL0;}}else{goto _LL15;}case 19: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2==0){_Tmp2=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct _tuple0*n=_Tmp2;
# 56
({void(*_Tmp4)(struct Cyc_Set_Set*,struct _tuple0*)=(void(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_insert;_Tmp4;})(env->enum_keepers,n);goto _LL0;}}else{goto _LL15;}case 3:
 goto _LL8;default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2==0){_LL8:
 goto _LL0;}else{goto _LL15;}}case 1: _Tmp2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;{void*topt=_Tmp2;
# 60
_Tmp2=topt;goto _LLC;}case 8: _Tmp2=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;_LLC: {void*topt=_Tmp2;
if((unsigned)topt){_Tmp2=topt;goto _LLE;}goto _LL0;}case 4: _Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_LLE: {void*t1=_Tmp2;
Cyc_LowTreeShake_build_tables_type(env,t1);goto _LL0;}case 5: _Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;{void*t1=_Tmp2;struct Cyc_Absyn_Exp*eo=_Tmp1;
# 65
if((unsigned)eo)Cyc_LowTreeShake_build_tables_exp(env,eo);
Cyc_LowTreeShake_build_tables_type(env,t1);
goto _LL0;}case 6: _Tmp0=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp0;
# 70
Cyc_LowTreeShake_build_tables_type(env,i.ret_type);
{struct Cyc_List_List*args=i.args;for(0;args!=0;args=args->tl){
Cyc_LowTreeShake_build_tables_type(env,(*((struct _tuple8*)args->hd)).f2);}}
goto _LL0;}case 11: _Tmp2=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
# 75
Cyc_LowTreeShake_build_tables_exp(env,e);goto _LL0;}default: _LL15:
# 77
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("LowTreeshake: unexpected type ",sizeof(char),31U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),2));});}_LL0:;}
# 81
static int Cyc_LowTreeShake_build_tables_visit_f1(struct Cyc_LowTreeShake_Env*env,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 1: _Tmp1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
_Tmp1=vd->name;goto _LL4;}case 2: _Tmp1=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1->name;_LL4: {struct _tuple0*qv=_Tmp1;
_Tmp1=qv;goto _LL6;}case 0: _Tmp1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL6: {struct _tuple0*qv=_Tmp1;
# 86
if(!((unsigned)({int*(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(int*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))Cyc_Hashtable_lookup_opt;_Tmp2;})(env->funs_defined,qv))&& !((unsigned)
({int*(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(int*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))Cyc_Hashtable_lookup_opt;_Tmp2;})(env->global_keepers,qv)))
({void(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _tuple0*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,int))Cyc_Hashtable_insert;_Tmp2;})(env->global_keepers,qv,1);
return 0;}default: goto _LLF;}case 31: _Tmp1=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
({void(*_Tmp2)(struct Cyc_Set_Set*,struct _tuple0*)=(void(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_insert;_Tmp2;})(env->enum_keepers,ed->name);return 0;}case 19: _Tmp1=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp1;
_Tmp1=t;goto _LLC;}case 17: _Tmp1=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLC: {void*t=_Tmp1;
_Tmp1=t;goto _LLE;}case 14: _Tmp1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLE: {void*t=_Tmp1;
Cyc_LowTreeShake_build_tables_type(env,t);return 1;}default: _LLF:
 return 1;};}
# 97
static int Cyc_LowTreeShake_build_tables_visit_f2(struct Cyc_LowTreeShake_Env*env,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==12){_Tmp2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 100
void*_Tmp3=d->r;void*_Tmp4;switch(*((int*)_Tmp3)){case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 102
Cyc_LowTreeShake_build_tables_type(env,vd->type);
return 1;}case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 105
Cyc_LowTreeShake_build_tables_type(env,fd->i.ret_type);
{struct Cyc_List_List*args=fd->i.args;for(0;args!=0;args=args->tl){
Cyc_LowTreeShake_build_tables_type(env,(*((struct _tuple8*)args->hd)).f2);}}
return 1;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("LowTreeShake: local decl not a var or function",sizeof(char),47U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),1));});};}}else{
# 111
return 1;};}
# 114
static void Cyc_LowTreeShake_build_tables_exp(struct Cyc_LowTreeShake_Env*env,struct Cyc_Absyn_Exp*e){
({void(*_Tmp0)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp0;})(Cyc_LowTreeShake_build_tables_visit_f1,Cyc_LowTreeShake_build_tables_visit_f2,env,e);}
# 118
static void Cyc_LowTreeShake_add_to_table(struct Cyc_Hashtable_Table*tab,struct _tuple0*qv,void*d){
# 120
struct Cyc_List_List**lst_opt=({struct Cyc_List_List**(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(struct Cyc_List_List**(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))Cyc_Hashtable_lookup_opt;_Tmp0;})(tab,qv);
struct Cyc_List_List*lst;lst=_cycalloc(sizeof(struct Cyc_List_List)),lst->hd=d,(unsigned)lst_opt?lst->tl=*lst_opt:(lst->tl=0);
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_List_List*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_List_List*))Cyc_Hashtable_insert;_Tmp0;})(tab,qv,lst);}
# 125
static void Cyc_LowTreeShake_build_tables(struct Cyc_LowTreeShake_Env*env,struct Cyc_List_List*tds){
for(1;tds!=0;tds=tds->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)tds->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 129
Cyc_LowTreeShake_build_tables_type(env,vd->type);
if(vd->initializer!=0)
Cyc_LowTreeShake_build_tables_exp(env,vd->initializer);
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 134
({void(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _tuple0*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,int))Cyc_Hashtable_insert;_Tmp2;})(env->funs_defined,fd->name,1);
Cyc_LowTreeShake_build_tables_type(env,fd->i.ret_type);
{struct Cyc_List_List*args=fd->i.args;for(0;args!=0;args=args->tl){
Cyc_LowTreeShake_build_tables_type(env,(*((struct _tuple8*)args->hd)).f2);}}
({void(*_Tmp2)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_LowTreeShake_Env*,struct Cyc_Absyn_Stmt*))Cyc_Absyn_visit_stmt;_Tmp2;})(Cyc_LowTreeShake_build_tables_visit_f1,Cyc_LowTreeShake_build_tables_visit_f2,env,fd->body);
goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
({void(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_LowTreeShake_add_to_table;_Tmp2;})(env->aggrdecls,ad->name,ad);goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
# 143
if(ed->fields!=0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)ed->fields->v;for(0;fs!=0;fs=fs->tl){
if(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag!=0)
Cyc_LowTreeShake_build_tables_exp(env,((struct Cyc_Absyn_Enumfield*)fs->hd)->tag);}}
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 149
Cyc_LowTreeShake_build_tables_type(env,_check_null(td->defn));goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("LowTreeShake: top-level Cyclone decl",sizeof(char),37U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});}_LL0:;}}
# 154
static void Cyc_LowTreeShake_process_worklist(struct Cyc_LowTreeShake_Env*env){
# 156
while(!Cyc_Set_is_empty(env->aggr_worklist)){
{struct _tuple0*n=({struct _tuple0*(*_Tmp0)(struct Cyc_Set_Set*)=(struct _tuple0*(*)(struct Cyc_Set_Set*))Cyc_Set_choose;_Tmp0;})(env->aggr_worklist);
({struct _tuple0*(*_Tmp0)(struct Cyc_Set_Set*,struct _tuple0*)=(struct _tuple0*(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_delete;_Tmp0;})(env->aggr_worklist,n);
({void(*_Tmp0)(struct Cyc_Set_Set*,struct _tuple0*)=(void(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_imp_insert;_Tmp0;})(env->aggr_keepers,n);{
# 161
struct Cyc_List_List**ads_opt=({struct Cyc_List_List**(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(struct Cyc_List_List**(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))Cyc_Hashtable_lookup_opt;_Tmp0;})(env->aggrdecls,n);
if(!((unsigned)ads_opt))
continue;{
struct Cyc_List_List*ads=*ads_opt;for(0;ads!=0;ads=ads->tl){
if(((struct Cyc_Absyn_Aggrdecl*)ads->hd)->impl!=0){
struct Cyc_List_List*fs=((struct Cyc_Absyn_Aggrdecl*)ads->hd)->impl->fields;for(0;fs!=0;fs=fs->tl){
Cyc_LowTreeShake_build_tables_type(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);}}}}}}
# 157
1U;}}
# 171
static struct Cyc_List_List*Cyc_LowTreeShake_trim_decls(struct Cyc_LowTreeShake_Env*env,struct Cyc_List_List*tds){
struct Cyc_List_List*ans=0;
for(1;tds!=0;tds=tds->tl){
int keep;
{void*_Tmp0=((struct Cyc_Absyn_Decl*)tds->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
keep=(int)vd->sc!=3 && vd->is_proto!=1 ||(unsigned)
({int*(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(int*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))Cyc_Hashtable_lookup_opt;_Tmp2;})(env->global_keepers,vd->name);
goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
keep=({int(*_Tmp2)(struct Cyc_Set_Set*,struct _tuple0*)=(int(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_member;_Tmp2;})(env->aggr_keepers,ad->name);goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
keep=({int(*_Tmp2)(struct Cyc_Set_Set*,struct _tuple0*)=(int(*)(struct Cyc_Set_Set*,struct _tuple0*))Cyc_Set_member;_Tmp2;})(env->enum_keepers,ed->name);goto _LL0;}default:
 keep=1;goto _LL0;}_LL0:;}
# 183
if(keep)
ans=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(struct Cyc_Absyn_Decl*)tds->hd,_Tmp0->tl=ans;_Tmp0;});}
# 186
return Cyc_List_imp_rev(ans);}
# 189
struct Cyc_List_List*Cyc_LowTreeShake_shake(struct Cyc_List_List*tds){
struct Cyc_LowTreeShake_Env env=({struct Cyc_LowTreeShake_Env _Tmp0;({struct Cyc_Hashtable_Table*_Tmp1=({struct Cyc_Hashtable_Table*(*_Tmp2)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))Cyc_Hashtable_create;_Tmp2;})(37,Cyc_Absyn_qvar_cmp,Cyc_Absyn_hash_qvar);_Tmp0.aggrdecls=_Tmp1;}),({
struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Set_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0.aggr_worklist=_Tmp1;}),({
struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Set_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0.aggr_keepers=_Tmp1;}),({
struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Set_Set*(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Set_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0.enum_keepers=_Tmp1;}),({
struct Cyc_Hashtable_Table*_Tmp1=({struct Cyc_Hashtable_Table*(*_Tmp2)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))Cyc_Hashtable_create;_Tmp2;})(37,Cyc_Absyn_qvar_cmp,Cyc_Absyn_hash_qvar);_Tmp0.global_keepers=_Tmp1;}),({
struct Cyc_Hashtable_Table*_Tmp1=({struct Cyc_Hashtable_Table*(*_Tmp2)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))Cyc_Hashtable_create;_Tmp2;})(37,Cyc_Absyn_qvar_cmp,Cyc_Absyn_hash_qvar);_Tmp0.funs_defined=_Tmp1;});_Tmp0;});
Cyc_LowTreeShake_build_tables(& env,tds);
Cyc_LowTreeShake_process_worklist(& env);
return Cyc_LowTreeShake_trim_decls(& env,tds);}

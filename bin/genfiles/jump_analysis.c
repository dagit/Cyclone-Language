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
 struct Cyc_Core_Opt{void*v;};
# 128 "core.h"
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 1249 "absyn.h"
void Cyc_Absyn_do_nested_statement(struct Cyc_Absyn_Exp*,void*,void(*)(void*,struct Cyc_Absyn_Stmt*),int);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 75 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 34 "flags.h"
extern int Cyc_Flags_no_regions;
# 117 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct{int tag;struct Cyc_Absyn_Switch_clause*f1;};struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct{int tag;struct _fat_ptr*f1;};
# 54 "jump_analysis.cyc"
struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_NotLoop_j_val={0};
struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_CaseEnd_j_val={1};
struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_FnEnd_j_val={2};
struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_Return_j_val={5};struct Cyc_JumpAnalysis_SaveEnv{int try_depth;int pat_var_depth;struct Cyc_List_List*enclosers;};struct Cyc_JumpAnalysis_FnEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*tables;struct Cyc_Hashtable_Table*stmt_info;struct Cyc_List_List*jumpers;struct Cyc_Hashtable_Table*labels;};struct Cyc_JumpAnalysis_StmtEnv{struct Cyc_JumpAnalysis_FnEnv*fenv;void*continue_stmt;void*break_stmt;void*next_stmt;struct Cyc_JumpAnalysis_SaveEnv save_env;};
# 81
static void Cyc_JumpAnalysis_anal_exp(struct Cyc_JumpAnalysis_StmtEnv,struct Cyc_Absyn_Exp*);
static void Cyc_JumpAnalysis_anal_stmt(struct Cyc_JumpAnalysis_StmtEnv,struct Cyc_Absyn_Stmt*);
static void Cyc_JumpAnalysis_anal_fd(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_Absyn_Fndecl*);
# 85
static int Cyc_JumpAnalysis_hash_ptr(void*s){void*_T0;int _T1;_T0=s;_T1=(int)_T0;
return _T1;}
# 88
static struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_make_ptr_table (void){int(*_T0)(void*,void*);struct Cyc_Hashtable_Table*_T1;_T0=Cyc_Core_ptrcmp;_T1=
Cyc_Hashtable_create(33,_T0,Cyc_JumpAnalysis_hash_ptr);return _T1;}
# 91
static struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_make_var_table (void){struct Cyc_Hashtable_Table*(*_T0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T1)(int,int(*)(void*,void*),int(*)(void*));int(*_T2)(struct _fat_ptr*,struct _fat_ptr*);int(*_T3)(struct _fat_ptr*);struct Cyc_Hashtable_Table*_T4;_T1=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T5)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T1;_T0=_T5;}_T2=Cyc_strptrcmp;_T3=Cyc_Hashtable_hash_stringptr;_T4=_T0(33,_T2,_T3);return _T4;}
# 94
static struct Cyc_JumpAnalysis_StmtEnv Cyc_JumpAnalysis_add_encloser(struct Cyc_Absyn_Stmt*s,struct Cyc_JumpAnalysis_StmtEnv env){struct Cyc_List_List*_T0;struct Cyc_JumpAnalysis_StmtEnv _T1;struct Cyc_JumpAnalysis_SaveEnv _T2;struct Cyc_JumpAnalysis_StmtEnv _T3;
struct Cyc_JumpAnalysis_StmtEnv env1=env;{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));
_T4->hd=s;_T1=env1;_T2=_T1.save_env;_T4->tl=_T2.enclosers;_T0=(struct Cyc_List_List*)_T4;}env1.save_env.enclosers=_T0;_T3=env1;
return _T3;}
# 99
static void Cyc_JumpAnalysis_anal_exps(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_List_List*es){struct Cyc_JumpAnalysis_StmtEnv _T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_List_List*_T4;
_TL3: if(es!=0)goto _TL1;else{goto _TL2;}
_TL1: _T0=env;_T1=es;_T2=_T1->hd;_T3=(struct Cyc_Absyn_Exp*)_T2;Cyc_JumpAnalysis_anal_exp(_T0,_T3);_T4=es;
# 100
es=_T4->tl;goto _TL3;_TL2:;}
# 103
static void Cyc_JumpAnalysis_anal_stmt_exp(struct Cyc_JumpAnalysis_StmtEnv*env,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;struct Cyc_JumpAnalysis_StmtEnv*_T1;struct Cyc_JumpAnalysis_StmtEnv _T2;struct Cyc_JumpAnalysis_StmtEnv _T3;struct Cyc_Absyn_Stmt*_T4;_T0=s;_T1=env;_T2=*_T1;_T3=
Cyc_JumpAnalysis_add_encloser(_T0,_T2);_T4=s;Cyc_JumpAnalysis_anal_stmt(_T3,_T4);}
# 106
static void Cyc_JumpAnalysis_anal_exp(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Exp*e){void(*_T0)(struct Cyc_Absyn_Exp*,struct Cyc_JumpAnalysis_StmtEnv*,void(*)(struct Cyc_JumpAnalysis_StmtEnv*,struct Cyc_Absyn_Stmt*),int);void(*_T1)(struct Cyc_Absyn_Exp*,void*,void(*)(void*,struct Cyc_Absyn_Stmt*),int);struct Cyc_Absyn_Exp*_T2;struct Cyc_JumpAnalysis_StmtEnv*_T3;struct Cyc_JumpAnalysis_StmtEnv*_T4;_T1=Cyc_Absyn_do_nested_statement;{
void(*_T5)(struct Cyc_Absyn_Exp*,struct Cyc_JumpAnalysis_StmtEnv*,void(*)(struct Cyc_JumpAnalysis_StmtEnv*,struct Cyc_Absyn_Stmt*),int)=(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_JumpAnalysis_StmtEnv*,void(*)(struct Cyc_JumpAnalysis_StmtEnv*,struct Cyc_Absyn_Stmt*),int))_T1;_T0=_T5;}_T2=e;_T3=& env;_T4=(struct Cyc_JumpAnalysis_StmtEnv*)_T3;_T0(_T2,_T4,Cyc_JumpAnalysis_anal_stmt_exp,0);}
# 109
static void Cyc_JumpAnalysis_anal_scs(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_List_List*scs){struct Cyc_JumpAnalysis_StmtEnv _T0;struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct*_T1;struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Switch_clause*_T5;struct Cyc_Absyn_Switch_clause*_T6;struct Cyc_Core_Opt*_T7;int _T8;struct Cyc_Absyn_Switch_clause*_T9;struct Cyc_Absyn_Stmt*_TA;struct Cyc_JumpAnalysis_StmtEnv _TB;struct Cyc_JumpAnalysis_StmtEnv _TC;struct Cyc_Absyn_Switch_clause*_TD;struct Cyc_Absyn_Stmt*_TE;struct Cyc_List_List*_TF;_T0=env;
# 112
env.break_stmt=_T0.next_stmt;_T1=& Cyc_JumpAnalysis_CaseEnd_j_val;_T2=(struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct*)_T1;
env.next_stmt=(void*)_T2;
_TL7: if(scs!=0)goto _TL5;else{goto _TL6;}
_TL5: _T3=scs;_T4=_T3->hd;{struct Cyc_Absyn_Switch_clause*sc=(struct Cyc_Absyn_Switch_clause*)_T4;_T5=sc;{
struct Cyc_Absyn_Exp*wc=_T5->where_clause;
if(wc==0)goto _TL8;
Cyc_JumpAnalysis_anal_exp(env,wc);goto _TL9;_TL8: _TL9: {
struct Cyc_JumpAnalysis_StmtEnv env1=env;_T6=sc;_T7=_T6->pat_vars;_T8=
Cyc_Tcpat_has_vars(_T7);if(!_T8)goto _TLA;_T9=sc;_TA=_T9->body;_TB=env1;
env1=Cyc_JumpAnalysis_add_encloser(_TA,_TB);
env1.save_env.pat_var_depth=env1.save_env.pat_var_depth + 1;goto _TLB;_TLA: _TLB: _TC=env1;_TD=sc;_TE=_TD->body;
# 124
Cyc_JumpAnalysis_anal_stmt(_TC,_TE);}}}_TF=scs;
# 114
scs=_TF->tl;goto _TL7;_TL6:;}struct _tuple12{struct Cyc_Absyn_Stmt*f0;void*f1;};
# 127
static void Cyc_JumpAnalysis_anal_stmt(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Stmt*s){void(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*);void(*_T1)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_StmtEnv _T2;struct Cyc_JumpAnalysis_FnEnv*_T3;struct Cyc_Hashtable_Table*_T4;struct Cyc_Absyn_Stmt*_T5;struct Cyc_JumpAnalysis_SaveEnv*_T6;struct Cyc_JumpAnalysis_StmtEnv _T7;struct Cyc_Absyn_Stmt*_T8;int*_T9;unsigned _TA;struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_TB;struct Cyc_JumpAnalysis_StmtEnv _TC;struct Cyc_JumpAnalysis_FnEnv*_TD;struct Cyc_List_List*_TE;struct _tuple12*_TF;struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct*_T10;struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct*_T11;struct Cyc_JumpAnalysis_StmtEnv _T12;struct Cyc_JumpAnalysis_FnEnv*_T13;struct _tuple9 _T14;struct _tuple9 _T15;void(*_T16)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*);void(*_T17)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_StmtEnv _T18;struct Cyc_JumpAnalysis_FnEnv*_T19;struct Cyc_Hashtable_Table*_T1A;struct Cyc_Absyn_Stmt*_T1B;struct Cyc_JumpAnalysis_SaveEnv*_T1C;struct Cyc_JumpAnalysis_StmtEnv _T1D;struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_T1E;struct Cyc_JumpAnalysis_StmtEnv _T1F;struct Cyc_JumpAnalysis_StmtEnv _T20;struct _tuple9 _T21;struct _tuple9 _T22;struct _tuple9 _T23;struct _tuple9 _T24;void(*_T25)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*);void(*_T26)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_StmtEnv _T27;struct Cyc_JumpAnalysis_FnEnv*_T28;struct Cyc_Hashtable_Table*_T29;struct Cyc_Absyn_Stmt*_T2A;struct Cyc_JumpAnalysis_SaveEnv*_T2B;struct Cyc_JumpAnalysis_StmtEnv _T2C;void(*_T2D)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*);void(*_T2E)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_StmtEnv _T2F;struct Cyc_JumpAnalysis_FnEnv*_T30;struct Cyc_Hashtable_Table*_T31;struct Cyc_Absyn_Stmt*_T32;struct Cyc_JumpAnalysis_SaveEnv*_T33;struct Cyc_JumpAnalysis_StmtEnv _T34;struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_T35;struct Cyc_JumpAnalysis_StmtEnv _T36;struct Cyc_JumpAnalysis_StmtEnv _T37;struct _tuple9 _T38;struct _tuple9 _T39;void(*_T3A)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*);void(*_T3B)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_StmtEnv _T3C;struct Cyc_JumpAnalysis_FnEnv*_T3D;struct Cyc_Hashtable_Table*_T3E;struct Cyc_Absyn_Stmt*_T3F;struct Cyc_JumpAnalysis_SaveEnv*_T40;struct Cyc_JumpAnalysis_StmtEnv _T41;struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_T42;struct Cyc_JumpAnalysis_StmtEnv _T43;struct Cyc_JumpAnalysis_StmtEnv _T44;struct Cyc_JumpAnalysis_StmtEnv _T45;struct Cyc_JumpAnalysis_FnEnv*_T46;struct Cyc_List_List*_T47;struct _tuple12*_T48;struct Cyc_JumpAnalysis_StmtEnv _T49;struct Cyc_JumpAnalysis_StmtEnv _T4A;struct Cyc_JumpAnalysis_FnEnv*_T4B;struct Cyc_JumpAnalysis_StmtEnv _T4C;struct Cyc_JumpAnalysis_FnEnv*_T4D;struct Cyc_List_List*_T4E;struct _tuple12*_T4F;struct Cyc_JumpAnalysis_StmtEnv _T50;struct Cyc_JumpAnalysis_StmtEnv _T51;struct Cyc_JumpAnalysis_FnEnv*_T52;struct Cyc_JumpAnalysis_StmtEnv _T53;struct Cyc_JumpAnalysis_FnEnv*_T54;struct Cyc_List_List*_T55;struct _tuple12*_T56;struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*_T57;struct Cyc_JumpAnalysis_StmtEnv _T58;struct Cyc_JumpAnalysis_FnEnv*_T59;struct _handler_cons*_T5A;int _T5B;struct Cyc_Absyn_Stmt*(*_T5C)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_T5D)(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_StmtEnv _T5E;struct Cyc_JumpAnalysis_FnEnv*_T5F;struct Cyc_Hashtable_Table*_T60;struct _fat_ptr*_T61;struct Cyc_Warn_String_Warn_Warg_struct _T62;struct Cyc_Absyn_Stmt*_T63;unsigned _T64;struct _fat_ptr _T65;void*_T66;struct Cyc_Core_Not_found_exn_struct*_T67;char*_T68;char*_T69;void(*_T6A)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Absyn_Stmt*);void(*_T6B)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_StmtEnv _T6C;struct Cyc_JumpAnalysis_FnEnv*_T6D;struct Cyc_Hashtable_Table*_T6E;struct _fat_ptr*_T6F;struct Cyc_Absyn_Stmt*_T70;struct Cyc_JumpAnalysis_StmtEnv _T71;struct Cyc_JumpAnalysis_FnEnv*_T72;struct Cyc_List_List*_T73;struct _tuple12*_T74;struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*_T75;struct Cyc_Absyn_Switch_clause**_T76;struct Cyc_JumpAnalysis_StmtEnv _T77;struct Cyc_JumpAnalysis_FnEnv*_T78;struct Cyc_Absyn_Decl*_T79;int*_T7A;unsigned _T7B;int _T7C;struct Cyc_Absyn_Vardecl*_T7D;struct Cyc_JumpAnalysis_StmtEnv _T7E;struct Cyc_JumpAnalysis_FnEnv*_T7F;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T80;struct Cyc_Absyn_Fndecl*_T81;_T1=Cyc_Hashtable_insert;{
void(*_T82)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*))_T1;_T0=_T82;}_T2=env;_T3=_T2.fenv;_T4=_T3->stmt_info;_T5=s;{struct Cyc_JumpAnalysis_SaveEnv*_T82=_cycalloc(sizeof(struct Cyc_JumpAnalysis_SaveEnv));_T7=env;*_T82=_T7.save_env;_T6=(struct Cyc_JumpAnalysis_SaveEnv*)_T82;}_T0(_T4,_T5,_T6);_T8=s;{
void*_T82=_T8->r;struct Cyc_Absyn_Decl*_T83;struct Cyc_Absyn_Switch_clause**_T84;struct Cyc_List_List*_T85;struct _fat_ptr*_T86;struct Cyc_Absyn_Stmt*_T87;struct Cyc_Absyn_Exp*_T88;struct Cyc_Absyn_Exp*_T89;struct Cyc_Absyn_Stmt*_T8A;struct Cyc_Absyn_Stmt*_T8B;struct Cyc_Absyn_Exp*_T8C;_T9=(int*)_T82;_TA=*_T9;switch(_TA){case 0: goto _LL0;case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T82;_T8C=_T8D->f1;}{struct Cyc_Absyn_Exp*e=_T8C;
# 131
Cyc_JumpAnalysis_anal_exp(env,e);goto _LL0;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T82;_T8B=_T8D->f1;_T8A=_T8D->f2;}{struct Cyc_Absyn_Stmt*s1=_T8B;struct Cyc_Absyn_Stmt*s2=_T8A;
# 133
struct Cyc_JumpAnalysis_StmtEnv env1=env;{struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_T8D=_cycalloc(sizeof(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct));_T8D->tag=3;
_T8D->f1=s2;_TB=(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*)_T8D;}env1.next_stmt=(void*)_TB;
Cyc_JumpAnalysis_anal_stmt(env1,s1);
Cyc_JumpAnalysis_anal_stmt(env,s2);goto _LL0;}case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T82;_T8C=_T8D->f1;_T8B=_T8D->f2;_T8A=_T8D->f3;}{struct Cyc_Absyn_Exp*e=_T8C;struct Cyc_Absyn_Stmt*s1=_T8B;struct Cyc_Absyn_Stmt*s2=_T8A;
# 139
Cyc_JumpAnalysis_anal_exp(env,e);
Cyc_JumpAnalysis_anal_stmt(env,s1);
Cyc_JumpAnalysis_anal_stmt(env,s2);goto _LL0;}case 3:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T82;_T8C=_T8D->f1;}{struct Cyc_Absyn_Exp*eopt=_T8C;
# 145
if(eopt==0)goto _TLD;Cyc_JumpAnalysis_anal_exp(env,eopt);goto _TLE;_TLD: _TLE: _TC=env;_TD=_TC.fenv;{struct Cyc_List_List*_T8D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T8E=_cycalloc(sizeof(struct _tuple12));
_T8E->f0=s;_T10=& Cyc_JumpAnalysis_Return_j_val;_T11=(struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct*)_T10;_T8E->f1=(void*)_T11;_TF=(struct _tuple12*)_T8E;}_T8D->hd=_TF;_T12=env;_T13=_T12.fenv;_T8D->tl=_T13->jumpers;_TE=(struct Cyc_List_List*)_T8D;}_TD->jumpers=_TE;goto _LL0;}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T82;_T14=_T8D->f1;_T8C=_T14.f0;_T15=_T8D->f1;_T8B=_T15.f1;_T8A=_T8D->f2;}{struct Cyc_Absyn_Exp*e=_T8C;struct Cyc_Absyn_Stmt*cont_s=_T8B;struct Cyc_Absyn_Stmt*s=_T8A;_T17=Cyc_Hashtable_insert;{
# 150
void(*_T8D)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*))_T17;_T16=_T8D;}_T18=env;_T19=_T18.fenv;_T1A=_T19->stmt_info;_T1B=cont_s;{struct Cyc_JumpAnalysis_SaveEnv*_T8D=_cycalloc(sizeof(struct Cyc_JumpAnalysis_SaveEnv));_T1D=env;*_T8D=_T1D.save_env;_T1C=(struct Cyc_JumpAnalysis_SaveEnv*)_T8D;}_T16(_T1A,_T1B,_T1C);
Cyc_JumpAnalysis_anal_exp(env,e);{
struct Cyc_JumpAnalysis_StmtEnv env_body=env;{struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_T8D=_cycalloc(sizeof(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct));_T8D->tag=3;
_T8D->f1=cont_s;_T1E=(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*)_T8D;}env_body.continue_stmt=(void*)_T1E;_T1F=env_body;
env_body.next_stmt=_T1F.continue_stmt;_T20=env;
env_body.break_stmt=_T20.next_stmt;
Cyc_JumpAnalysis_anal_stmt(env_body,s);goto _LL0;}}case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T82;_T8C=_T8D->f1;_T21=_T8D->f2;_T89=_T21.f0;_T22=_T8D->f2;_T8B=_T22.f1;_T23=_T8D->f3;_T88=_T23.f0;_T24=_T8D->f3;_T8A=_T24.f1;_T87=_T8D->f4;}{struct Cyc_Absyn_Exp*e1=_T8C;struct Cyc_Absyn_Exp*e2=_T89;struct Cyc_Absyn_Stmt*guard_s=_T8B;struct Cyc_Absyn_Exp*e3=_T88;struct Cyc_Absyn_Stmt*cont_s=_T8A;struct Cyc_Absyn_Stmt*s=_T87;_T26=Cyc_Hashtable_insert;{
# 159
void(*_T8D)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*))_T26;_T25=_T8D;}_T27=env;_T28=_T27.fenv;_T29=_T28->stmt_info;_T2A=guard_s;{struct Cyc_JumpAnalysis_SaveEnv*_T8D=_cycalloc(sizeof(struct Cyc_JumpAnalysis_SaveEnv));_T2C=env;*_T8D=_T2C.save_env;_T2B=(struct Cyc_JumpAnalysis_SaveEnv*)_T8D;}_T25(_T29,_T2A,_T2B);_T2E=Cyc_Hashtable_insert;{
void(*_T8D)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*))_T2E;_T2D=_T8D;}_T2F=env;_T30=_T2F.fenv;_T31=_T30->stmt_info;_T32=cont_s;{struct Cyc_JumpAnalysis_SaveEnv*_T8D=_cycalloc(sizeof(struct Cyc_JumpAnalysis_SaveEnv));_T34=env;*_T8D=_T34.save_env;_T33=(struct Cyc_JumpAnalysis_SaveEnv*)_T8D;}_T2D(_T31,_T32,_T33);
Cyc_JumpAnalysis_anal_exp(env,e1);
Cyc_JumpAnalysis_anal_exp(env,e2);{
# 164
struct Cyc_JumpAnalysis_StmtEnv env_body=env;{struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_T8D=_cycalloc(sizeof(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct));_T8D->tag=3;
_T8D->f1=cont_s;_T35=(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*)_T8D;}env_body.continue_stmt=(void*)_T35;_T36=env_body;
env_body.next_stmt=_T36.continue_stmt;_T37=env;
env_body.break_stmt=_T37.next_stmt;
Cyc_JumpAnalysis_anal_stmt(env_body,s);
Cyc_JumpAnalysis_anal_exp(env_body,e3);goto _LL0;}}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T82;_T8B=_T8D->f1;_T38=_T8D->f2;_T8C=_T38.f0;_T39=_T8D->f2;_T8A=_T39.f1;}{struct Cyc_Absyn_Stmt*s=_T8B;struct Cyc_Absyn_Exp*e=_T8C;struct Cyc_Absyn_Stmt*cont_s=_T8A;_T3B=Cyc_Hashtable_insert;{
# 172
void(*_T8D)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_JumpAnalysis_SaveEnv*))_T3B;_T3A=_T8D;}_T3C=env;_T3D=_T3C.fenv;_T3E=_T3D->stmt_info;_T3F=cont_s;{struct Cyc_JumpAnalysis_SaveEnv*_T8D=_cycalloc(sizeof(struct Cyc_JumpAnalysis_SaveEnv));_T41=env;*_T8D=_T41.save_env;_T40=(struct Cyc_JumpAnalysis_SaveEnv*)_T8D;}_T3A(_T3E,_T3F,_T40);{
struct Cyc_JumpAnalysis_StmtEnv env_body=env;{struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_T8D=_cycalloc(sizeof(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct));_T8D->tag=3;
_T8D->f1=cont_s;_T42=(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*)_T8D;}env_body.continue_stmt=(void*)_T42;_T43=env_body;
env_body.next_stmt=_T43.continue_stmt;_T44=env;
env_body.break_stmt=_T44.next_stmt;
Cyc_JumpAnalysis_anal_stmt(env_body,s);
Cyc_JumpAnalysis_anal_exp(env,e);goto _LL0;}}case 6: _T45=env;_T46=_T45.fenv;{struct Cyc_List_List*_T8D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T8E=_cycalloc(sizeof(struct _tuple12));
# 182
_T8E->f0=s;_T49=env;_T8E->f1=_T49.break_stmt;_T48=(struct _tuple12*)_T8E;}_T8D->hd=_T48;_T4A=env;_T4B=_T4A.fenv;_T8D->tl=_T4B->jumpers;_T47=(struct Cyc_List_List*)_T8D;}_T46->jumpers=_T47;goto _LL0;case 7: _T4C=env;_T4D=_T4C.fenv;{struct Cyc_List_List*_T8D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T8E=_cycalloc(sizeof(struct _tuple12));
# 185
_T8E->f0=s;_T50=env;_T8E->f1=_T50.continue_stmt;_T4F=(struct _tuple12*)_T8E;}_T8D->hd=_T4F;_T51=env;_T52=_T51.fenv;_T8D->tl=_T52->jumpers;_T4E=(struct Cyc_List_List*)_T8D;}_T4D->jumpers=_T4E;goto _LL0;case 8:{struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_T82;_T86=_T8D->f1;}{struct _fat_ptr*lab=_T86;_T53=env;_T54=_T53.fenv;{struct Cyc_List_List*_T8D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T8E=_cycalloc(sizeof(struct _tuple12));
# 188
_T8E->f0=s;{struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*_T8F=_cycalloc(sizeof(struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct));_T8F->tag=6;_T8F->f1=lab;_T57=(struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*)_T8F;}_T8E->f1=(void*)_T57;_T56=(struct _tuple12*)_T8E;}_T8D->hd=_T56;_T58=env;_T59=_T58.fenv;_T8D->tl=_T59->jumpers;_T55=(struct Cyc_List_List*)_T8D;}_T54->jumpers=_T55;goto _LL0;}case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T82;_T86=_T8D->f1;_T8B=_T8D->f2;}{struct _fat_ptr*lab=_T86;struct Cyc_Absyn_Stmt*s1=_T8B;{struct _handler_cons _T8D;_T5A=& _T8D;_push_handler(_T5A);{int _T8E=0;_T5B=setjmp(_T8D.handler);if(!_T5B)goto _TLF;_T8E=1;goto _TL10;_TLF: _TL10: if(_T8E)goto _TL11;else{goto _TL13;}_TL13: _T5D=Cyc_Hashtable_lookup;{
# 193
struct Cyc_Absyn_Stmt*(*_T8F)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_T5D;_T5C=_T8F;}_T5E=env;_T5F=_T5E.fenv;_T60=_T5F->labels;_T61=lab;_T5C(_T60,_T61);{struct Cyc_Warn_String_Warn_Warg_struct _T8F;_T8F.tag=0;
_T8F.f1=_tag_fat("repated label",sizeof(char),14U);_T62=_T8F;}{struct Cyc_Warn_String_Warn_Warg_struct _T8F=_T62;void*_T90[1];_T90[0]=& _T8F;_T63=s;_T64=_T63->loc;_T65=_tag_fat(_T90,sizeof(void*),1);Cyc_Warn_err2(_T64,_T65);}_pop_handler();goto _TL12;_TL11: _T66=Cyc_Core_get_exn_thrown();{void*_T8F=(void*)_T66;void*_T90;_T67=(struct Cyc_Core_Not_found_exn_struct*)_T8F;_T68=_T67->tag;_T69=Cyc_Core_Not_found;if(_T68!=_T69)goto _TL14;goto _LL21;_TL14: _T90=_T8F;{void*exn=_T90;_rethrow(exn);}_LL21:;}_TL12:;}}_T6B=Cyc_Hashtable_insert;{
# 196
void(*_T8D)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Absyn_Stmt*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Absyn_Stmt*))_T6B;_T6A=_T8D;}_T6C=env;_T6D=_T6C.fenv;_T6E=_T6D->labels;_T6F=lab;_T70=s;_T6A(_T6E,_T6F,_T70);
Cyc_JumpAnalysis_anal_stmt(env,s1);goto _LL0;}case 11:{struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T82;_T85=_T8D->f1;_T84=_T8D->f2;}{struct Cyc_List_List*es=_T85;struct Cyc_Absyn_Switch_clause**clauseopt=_T84;
# 202
Cyc_JumpAnalysis_anal_exps(env,es);_T71=env;_T72=_T71.fenv;{struct Cyc_List_List*_T8D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T8E=_cycalloc(sizeof(struct _tuple12));
_T8E->f0=s;{struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*_T8F=_cycalloc(sizeof(struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct));_T8F->tag=4;_T76=_check_null(clauseopt);_T8F->f1=*_T76;_T75=(struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*)_T8F;}_T8E->f1=(void*)_T75;_T74=(struct _tuple12*)_T8E;}_T8D->hd=_T74;_T77=env;_T78=_T77.fenv;
_T8D->tl=_T78->jumpers;_T73=(struct Cyc_List_List*)_T8D;}
# 203
_T72->jumpers=_T73;goto _LL0;}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T82;_T8C=_T8D->f1;_T85=_T8D->f2;}{struct Cyc_Absyn_Exp*e=_T8C;struct Cyc_List_List*scs=_T85;
# 207
Cyc_JumpAnalysis_anal_exp(env,e);
Cyc_JumpAnalysis_anal_scs(env,scs);goto _LL0;}case 15:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T82;_T8B=_T8D->f1;_T85=_T8D->f2;}{struct Cyc_Absyn_Stmt*s=_T8B;struct Cyc_List_List*scs=_T85;
# 211
struct Cyc_JumpAnalysis_StmtEnv env1=Cyc_JumpAnalysis_add_encloser(s,env);
env1.save_env.try_depth=env1.save_env.try_depth + 1;
Cyc_JumpAnalysis_anal_stmt(env1,s);
Cyc_JumpAnalysis_anal_scs(env,scs);goto _LL0;}default:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T8D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T82;_T83=_T8D->f1;_T8B=_T8D->f2;}{struct Cyc_Absyn_Decl*d=_T83;struct Cyc_Absyn_Stmt*s1=_T8B;
# 220
env=Cyc_JumpAnalysis_add_encloser(s,env);_T79=d;{
void*_T8D=_T79->r;struct Cyc_Absyn_Fndecl*_T8E;struct Cyc_Absyn_Exp*_T8F;_T7A=(int*)_T8D;_T7B=*_T7A;switch(_T7B){case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T90=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T8D;_T8F=_T90->f3;}{struct Cyc_Absyn_Exp*eo=_T8F;
# 226
if(eo==0)goto _TL17;
Cyc_JumpAnalysis_anal_exp(env,eo);
env.save_env.pat_var_depth=env.save_env.pat_var_depth + 1;goto _TL18;
# 231
_TL17: _T7C=Cyc_Flags_no_regions;if(_T7C)goto _TL19;else{goto _TL1B;}
_TL1B: env.save_env.try_depth=env.save_env.try_depth + 1;goto _TL1A;_TL19: _TL1A: _TL18: goto _LL26;}case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T90=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T8D;_T7D=_T90->f1;{struct Cyc_Absyn_Vardecl _T91=*_T7D;_T8F=_T91.initializer;}}{struct Cyc_Absyn_Exp*eo=_T8F;
# 235
if(eo==0)goto _TL1C;Cyc_JumpAnalysis_anal_exp(env,eo);goto _TL1D;_TL1C: _TL1D: goto _LL26;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T90=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T8D;_T8E=_T90->f1;}{struct Cyc_Absyn_Fndecl*fd=_T8E;_T7E=env;_T7F=_T7E.fenv;_T80=_T7F->tables;_T81=fd;
# 238
Cyc_JumpAnalysis_anal_fd(_T80,_T81);goto _LL26;}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T90=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T8D;_T8F=_T90->f3;}{struct Cyc_Absyn_Exp*e=_T8F;
# 241
Cyc_JumpAnalysis_anal_exp(env,e);
env.save_env.pat_var_depth=env.save_env.pat_var_depth + 1;goto _LL26;}case 3: goto _LL32;case 8: _LL32: goto _LL34;case 5: _LL34: goto _LL36;case 6: _LL36: goto _LL38;case 7: _LL38: goto _LL3A;case 13: _LL3A: goto _LL3C;case 14: _LL3C: goto _LL3E;case 15: _LL3E: goto _LL40;case 16: _LL40: goto _LL42;case 11: _LL42: goto _LL44;case 12: _LL44: goto _LL46;case 9: _LL46: goto _LL48;default: _LL48: goto _LL26;}_LL26:;}
# 259
Cyc_JumpAnalysis_anal_stmt(env,s1);goto _LL0;}}_LL0:;}}
# 264
static void Cyc_JumpAnalysis_anal_fd(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){struct Cyc_JumpAnalysis_FnEnv*_T0;struct Cyc_JumpAnalysis_FnEnv*_T1;struct Cyc_JumpAnalysis_FnEnv*_T2;struct Cyc_JumpAnalysis_FnEnv*_T3;struct Cyc_JumpAnalysis_StmtEnv _T4;struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct*_T5;struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct*_T6;struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct*_T7;struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct*_T8;struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct*_T9;struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct*_TA;struct Cyc_JumpAnalysis_StmtEnv _TB;struct Cyc_Absyn_Fndecl*_TC;struct Cyc_Absyn_Stmt*_TD;struct Cyc_JumpAnalysis_FnEnv*_TE;struct Cyc_JumpAnalysis_FnEnv*_TF;struct Cyc_List_List*_T10;void*_T11;struct Cyc_JumpAnalysis_SaveEnv*(*_T12)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T13)(struct Cyc_Hashtable_Table*,void*);void*_T14;int*_T15;unsigned _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_Absyn_Stmt*_T18;unsigned _T19;struct _fat_ptr _T1A;struct Cyc_Warn_String_Warn_Warg_struct _T1B;struct Cyc_Absyn_Stmt*_T1C;unsigned _T1D;struct _fat_ptr _T1E;void(*_T1F)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*);void(*_T20)(struct Cyc_Hashtable_Table*,void*,void*);void(*_T21)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int);void(*_T22)(struct Cyc_Hashtable_Table*,void*,void*);void(*_T23)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int);void(*_T24)(struct Cyc_Hashtable_Table*,void*,void*);void*_T25;struct _handler_cons*_T26;int _T27;struct Cyc_Absyn_Stmt*(*_T28)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_T29)(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_FnEnv*_T2A;struct Cyc_Hashtable_Table*_T2B;struct _fat_ptr*_T2C;void*_T2D;struct Cyc_Core_Not_found_exn_struct*_T2E;char*_T2F;char*_T30;struct Cyc_Warn_String_Warn_Warg_struct _T31;struct Cyc_Warn_String_Warn_Warg_struct _T32;struct _fat_ptr*_T33;struct Cyc_Warn_String_Warn_Warg_struct _T34;struct Cyc_Absyn_Stmt*_T35;unsigned _T36;struct _fat_ptr _T37;struct Cyc_JumpAnalysis_SaveEnv*(*_T38)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T39)(struct Cyc_Hashtable_Table*,void*);struct Cyc_List_List*_T3A;void*_T3B;struct Cyc_List_List*_T3C;void*_T3D;struct Cyc_Absyn_Stmt*_T3E;struct Cyc_Absyn_Stmt*_T3F;struct Cyc_List_List*_T40;struct Cyc_Warn_String_Warn_Warg_struct _T41;struct Cyc_Absyn_Stmt*_T42;unsigned _T43;struct _fat_ptr _T44;void*_T45;struct Cyc_JumpAnalysis_SaveEnv*(*_T46)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T47)(struct Cyc_Hashtable_Table*,void*);void(*_T48)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int);void(*_T49)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T4A;struct Cyc_Absyn_Stmt*_T4B;int _T4C;void(*_T4D)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int);void(*_T4E)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T4F;struct Cyc_Absyn_Stmt*_T50;int _T51;void(*_T52)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*);void(*_T53)(struct Cyc_Hashtable_Table*,void*,void*);void*_T54;struct Cyc_JumpAnalysis_SaveEnv*(*_T55)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T56)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_T57;struct Cyc_Absyn_Switch_clause*_T58;struct Cyc_Absyn_Stmt*_T59;void(*_T5A)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int);void(*_T5B)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T5C;struct Cyc_Absyn_Stmt*_T5D;int _T5E;struct Cyc_Absyn_Switch_clause*_T5F;struct Cyc_Core_Opt*_T60;int _T61;int _T62;int _T63;void(*_T64)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int);void(*_T65)(struct Cyc_Hashtable_Table*,void*,void*);void(*_T66)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*);void(*_T67)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T68;struct Cyc_Absyn_Stmt*_T69;struct Cyc_Absyn_Switch_clause*_T6A;struct Cyc_Absyn_Stmt*_T6B;struct Cyc_List_List*_T6C;void(*_T6D)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*);void(*_T6E)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result*_T6F;struct Cyc_Hashtable_Table*_T70;struct Cyc_Absyn_Fndecl*_T71;struct Cyc_Hashtable_Table*_T72;void(*_T73)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*);void(*_T74)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result*_T75;struct Cyc_Hashtable_Table*_T76;struct Cyc_Absyn_Fndecl*_T77;struct Cyc_Hashtable_Table*_T78;void(*_T79)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*);void(*_T7A)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result*_T7B;struct Cyc_Hashtable_Table*_T7C;struct Cyc_Absyn_Fndecl*_T7D;struct Cyc_Hashtable_Table*_T7E;
struct Cyc_JumpAnalysis_FnEnv*fenv;fenv=_cycalloc(sizeof(struct Cyc_JumpAnalysis_FnEnv));_T0=fenv;_T0->tables=tables;_T1=fenv;
_T1->stmt_info=Cyc_JumpAnalysis_make_ptr_table();_T2=fenv;
_T2->jumpers=0;_T3=fenv;
_T3->labels=Cyc_JumpAnalysis_make_var_table();{struct Cyc_JumpAnalysis_StmtEnv _T7F;
_T7F.fenv=fenv;_T5=& Cyc_JumpAnalysis_NotLoop_j_val;_T6=(struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct*)_T5;
_T7F.continue_stmt=(void*)_T6;_T7=& Cyc_JumpAnalysis_NotLoop_j_val;_T8=(struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct*)_T7;
_T7F.break_stmt=(void*)_T8;_T9=& Cyc_JumpAnalysis_FnEnd_j_val;_TA=(struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct*)_T9;
_T7F.next_stmt=(void*)_TA;
_T7F.save_env.try_depth=0;
_T7F.save_env.pat_var_depth=0;
_T7F.save_env.enclosers=0;_T4=_T7F;}{
# 269
struct Cyc_JumpAnalysis_StmtEnv senv=_T4;_TB=senv;_TC=fd;_TD=_TC->body;
# 276
Cyc_JumpAnalysis_anal_stmt(_TB,_TD);{
# 278
struct Cyc_Hashtable_Table*pop_table=Cyc_JumpAnalysis_make_ptr_table();
struct Cyc_Hashtable_Table*succ_table=Cyc_JumpAnalysis_make_ptr_table();
struct Cyc_Hashtable_Table*pat_pop_table=Cyc_JumpAnalysis_make_ptr_table();_TE=fenv;{
# 282
struct Cyc_Hashtable_Table*stmt_info=_TE->stmt_info;_TF=fenv;{
struct Cyc_List_List*jumpers=_TF->jumpers;_TL21: if(jumpers!=0)goto _TL1F;else{goto _TL20;}
_TL1F: _T10=jumpers;_T11=_T10->hd;{struct _tuple12*_T7F=(struct _tuple12*)_T11;void*_T80;struct Cyc_Absyn_Stmt*_T81;{struct _tuple12 _T82=*_T7F;_T81=_T82.f0;_T80=_T82.f1;}{struct Cyc_Absyn_Stmt*src_stmt=_T81;void*dest=_T80;_T13=Cyc_Hashtable_lookup;{
# 287
struct Cyc_JumpAnalysis_SaveEnv*(*_T82)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T13;_T12=_T82;}{
# 285
struct Cyc_JumpAnalysis_SaveEnv*_T82=
# 287
_T12(stmt_info,src_stmt);struct Cyc_List_List*_T83;int _T84;int _T85;{struct Cyc_JumpAnalysis_SaveEnv _T86=*_T82;_T85=_T86.try_depth;_T84=_T86.pat_var_depth;_T83=_T86.enclosers;}{int src_trydepth=_T85;int src_pvdepth=_T84;struct Cyc_List_List*src_enclose=_T83;struct Cyc_Absyn_Switch_clause*_T86;struct Cyc_Absyn_Stmt*_T87;struct _fat_ptr*_T88;_T14=dest;_T15=(int*)_T14;_T16=*_T15;switch(_T16){case 0:{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;
# 290
_T89.f1=_tag_fat("jump has no target",sizeof(char),19U);_T17=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T17;void*_T8A[1];_T8A[0]=& _T89;_T18=src_stmt;_T19=_T18->loc;_T1A=_tag_fat(_T8A,sizeof(void*),1);Cyc_Warn_err2(_T19,_T1A);}goto _LL6;case 1:{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;
# 294
_T89.f1=_tag_fat("break causes outer switch to implicitly fallthru",sizeof(char),49U);_T1B=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T1B;void*_T8A[1];_T8A[0]=& _T89;_T1C=src_stmt;_T1D=_T1C->loc;_T1E=_tag_fat(_T8A,sizeof(void*),1);Cyc_Warn_err2(_T1D,_T1E);}goto _LL6;case 2: _T20=Cyc_Hashtable_insert;{
# 297
void(*_T89)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))_T20;_T1F=_T89;}_T1F(succ_table,src_stmt,0);goto _LLE;case 5: _LLE: _T22=Cyc_Hashtable_insert;{
# 300
void(*_T89)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int))_T22;_T21=_T89;}_T21(pop_table,src_stmt,src_trydepth);_T24=Cyc_Hashtable_insert;{
void(*_T89)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int))_T24;_T23=_T89;}_T23(pat_pop_table,src_stmt,src_pvdepth);goto _LL6;case 6: _T25=dest;{struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*_T89=(struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*)_T25;_T88=_T89->f1;}{struct _fat_ptr*lab=_T88;
# 304
struct Cyc_Absyn_Stmt*dest_stmt;{struct _handler_cons _T89;_T26=& _T89;_push_handler(_T26);{int _T8A=0;_T27=setjmp(_T89.handler);if(!_T27)goto _TL23;_T8A=1;goto _TL24;_TL23: _TL24: if(_T8A)goto _TL25;else{goto _TL27;}_TL27: _T29=Cyc_Hashtable_lookup;{
struct Cyc_Absyn_Stmt*(*_T8B)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_T29;_T28=_T8B;}_T2A=fenv;_T2B=_T2A->labels;_T2C=lab;dest_stmt=_T28(_T2B,_T2C);_pop_handler();goto _TL26;_TL25: _T2D=Cyc_Core_get_exn_thrown();{void*_T8B=(void*)_T2D;void*_T8C;_T2E=(struct Cyc_Core_Not_found_exn_struct*)_T8B;_T2F=_T2E->tag;_T30=Cyc_Core_Not_found;if(_T2F!=_T30)goto _TL28;{struct Cyc_Warn_String_Warn_Warg_struct _T8D;_T8D.tag=0;
# 307
_T8D.f1=_tag_fat("label ",sizeof(char),7U);_T31=_T8D;}{struct Cyc_Warn_String_Warn_Warg_struct _T8D=_T31;{struct Cyc_Warn_String_Warn_Warg_struct _T8E;_T8E.tag=0;_T33=lab;_T8E.f1=*_T33;_T32=_T8E;}{struct Cyc_Warn_String_Warn_Warg_struct _T8E=_T32;{struct Cyc_Warn_String_Warn_Warg_struct _T8F;_T8F.tag=0;_T8F.f1=_tag_fat(" undefined",sizeof(char),11U);_T34=_T8F;}{struct Cyc_Warn_String_Warn_Warg_struct _T8F=_T34;void*_T90[3];_T90[0]=& _T8D;_T90[1]=& _T8E;_T90[2]=& _T8F;_T35=src_stmt;_T36=_T35->loc;_T37=_tag_fat(_T90,sizeof(void*),3);Cyc_Warn_err2(_T36,_T37);}}}goto L;_TL28: _T8C=_T8B;{void*exn=_T8C;_rethrow(exn);};}_TL26:;}}_T39=Cyc_Hashtable_lookup;{
# 311
struct Cyc_JumpAnalysis_SaveEnv*(*_T89)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T39;_T38=_T89;}{
# 310
struct Cyc_JumpAnalysis_SaveEnv*_T89=
_T38(stmt_info,dest_stmt);struct Cyc_List_List*_T8A;{struct Cyc_JumpAnalysis_SaveEnv _T8B=*_T89;_T8A=_T8B.enclosers;}{struct Cyc_List_List*dest_enclose=_T8A;
if(dest_enclose==0)goto _TL2A;_T3A=dest_enclose;_T3B=_T3A->hd;{
struct Cyc_Absyn_Stmt*dest_scope=(struct Cyc_Absyn_Stmt*)_T3B;
_TL2F: if(src_enclose!=0)goto _TL2D;else{goto _TL2E;}
_TL2D: _T3C=src_enclose;_T3D=_T3C->hd;_T3E=(struct Cyc_Absyn_Stmt*)_T3D;_T3F=dest_scope;if(_T3E!=_T3F)goto _TL30;goto _TL2E;_TL30: _T40=src_enclose;
# 314
src_enclose=_T40->tl;goto _TL2F;_TL2E:
# 317
 if(src_enclose!=0)goto _TL32;{struct Cyc_Warn_String_Warn_Warg_struct _T8B;_T8B.tag=0;
# 319
_T8B.f1=_tag_fat("goto enters local scope or exception handler",sizeof(char),45U);_T41=_T8B;}{struct Cyc_Warn_String_Warn_Warg_struct _T8B=_T41;void*_T8C[1];_T8C[0]=& _T8B;_T42=src_stmt;_T43=_T42->loc;_T44=_tag_fat(_T8C,sizeof(void*),1);Cyc_Warn_err2(_T43,_T44);}goto _LL6;_TL32:;}goto _TL2B;_TL2A: _TL2B: _T87=dest_stmt;goto _LL12;}}}case 3: _T45=dest;{struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_T89=(struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*)_T45;_T87=_T89->f1;}_LL12: {struct Cyc_Absyn_Stmt*dest_stmt=_T87;_T47=Cyc_Hashtable_lookup;{
# 326
struct Cyc_JumpAnalysis_SaveEnv*(*_T89)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T47;_T46=_T89;}{
# 325
struct Cyc_JumpAnalysis_SaveEnv*_T89=
_T46(stmt_info,dest_stmt);int _T8A;int _T8B;{struct Cyc_JumpAnalysis_SaveEnv _T8C=*_T89;_T8B=_T8C.try_depth;_T8A=_T8C.pat_var_depth;}{int dest_trydepth=_T8B;int dest_pvdepth=_T8A;_T49=Cyc_Hashtable_insert;{
void(*_T8C)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int))_T49;_T48=_T8C;}_T4A=pop_table;_T4B=src_stmt;_T4C=src_trydepth - dest_trydepth;_T48(_T4A,_T4B,_T4C);_T4E=Cyc_Hashtable_insert;{
void(*_T8C)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int))_T4E;_T4D=_T8C;}_T4F=pat_pop_table;_T50=src_stmt;_T51=src_pvdepth - dest_pvdepth;_T4D(_T4F,_T50,_T51);_T53=Cyc_Hashtable_insert;{
void(*_T8C)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))_T53;_T52=_T8C;}_T52(succ_table,src_stmt,dest_stmt);goto _LL6;}}}default: _T54=dest;{struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*_T89=(struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*)_T54;_T86=_T89->f1;}{struct Cyc_Absyn_Switch_clause*dest_clause=_T86;_T56=Cyc_Hashtable_lookup;{
# 333
struct Cyc_JumpAnalysis_SaveEnv*(*_T89)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T56;_T55=_T89;}_T57=stmt_info;_T58=dest_clause;_T59=_T58->body;{
# 332
struct Cyc_JumpAnalysis_SaveEnv*_T89=
_T55(_T57,_T59);int _T8A;int _T8B;{struct Cyc_JumpAnalysis_SaveEnv _T8C=*_T89;_T8B=_T8C.try_depth;_T8A=_T8C.pat_var_depth;}{int dest_trydepth=_T8B;int dest_pvdepth=_T8A;_T5B=Cyc_Hashtable_insert;{
void(*_T8C)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int))_T5B;_T5A=_T8C;}_T5C=pop_table;_T5D=src_stmt;_T5E=src_trydepth - dest_trydepth;_T5A(_T5C,_T5D,_T5E);{
int pv_pop;_T5F=dest_clause;_T60=_T5F->pat_vars;_T61=
Cyc_Tcpat_has_vars(_T60);if(!_T61)goto _TL34;_T62=1 + src_pvdepth;_T63=dest_pvdepth;
pv_pop=_T62 - _T63;goto _TL35;
# 339
_TL34: pv_pop=src_pvdepth - dest_pvdepth;_TL35: _T65=Cyc_Hashtable_insert;{
void(*_T8C)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,int))_T65;_T64=_T8C;}_T64(pat_pop_table,src_stmt,pv_pop);_T67=Cyc_Hashtable_insert;{
void(*_T8C)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))_T67;_T66=_T8C;}_T68=succ_table;_T69=src_stmt;_T6A=dest_clause;_T6B=_T6A->body;_T66(_T68,_T69,_T6B);goto _LL6;}}}}}_LL6:;}}}}_T6C=jumpers;
# 283
jumpers=_T6C->tl;goto _TL21;_TL20:;}
# 345
L: _T6E=Cyc_Hashtable_insert;{void(*_T7F)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*))_T6E;_T6D=_T7F;}_T6F=tables;_T70=_T6F->pop_tables;_T71=fd;_T72=pop_table;_T6D(_T70,_T71,_T72);_T74=Cyc_Hashtable_insert;{
void(*_T7F)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*))_T74;_T73=_T7F;}_T75=tables;_T76=_T75->succ_tables;_T77=fd;_T78=succ_table;_T73(_T76,_T77,_T78);_T7A=Cyc_Hashtable_insert;{
void(*_T7F)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table*))_T7A;_T79=_T7F;}_T7B=tables;_T7C=_T7B->pat_pop_tables;_T7D=fd;_T7E=pat_pop_table;_T79(_T7C,_T7D,_T7E);}}}}
# 350
static void Cyc_JumpAnalysis_jump_analysis_iter(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_List_List*_T5;
# 353
_TL39: if(tds!=0)goto _TL37;else{goto _TL38;}
_TL37: _T0=tds;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_T6=_T2->r;struct Cyc_Absyn_Fndecl*_T7;struct Cyc_List_List*_T8;_T3=(int*)_T6;_T4=*_T3;switch(_T4){case 2: goto _LL4;case 3: _LL4: goto _LL6;case 4: _LL6: goto _LL8;case 0: _LL8: goto _LLA;case 8: _LLA: goto _LLC;case 5: _LLC: goto _LLE;case 6: _LLE: goto _LL10;case 7: _LL10: goto _LL12;case 13: _LL12: goto _LL14;case 14: _LL14: goto _LL16;case 15: _LL16: goto _LL18;case 16: _LL18: goto _LL1A;case 11: _LL1A: goto _LL1C;case 12: _LL1C: goto _TL36;case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T9=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T6;_T8=_T9->f2;}{struct Cyc_List_List*tds2=_T8;_T8=tds2;goto _LL20;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T9=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T6;_T8=_T9->f2;}_LL20: {struct Cyc_List_List*tds2=_T8;
# 370
Cyc_JumpAnalysis_jump_analysis_iter(tables,tds2);goto _LL0;}default:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T9=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T6;_T7=_T9->f1;}{struct Cyc_Absyn_Fndecl*fd=_T7;
Cyc_JumpAnalysis_anal_fd(tables,fd);goto _LL0;}}_LL0:;}_TL36: _T5=tds;
# 353
tds=_T5->tl;goto _TL39;_TL38:;}
# 375
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds){struct Cyc_JumpAnalysis_Jump_Anal_Result*_T0;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T1;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T2;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T3;
struct Cyc_JumpAnalysis_Jump_Anal_Result*tables;tables=_cycalloc(sizeof(struct Cyc_JumpAnalysis_Jump_Anal_Result));_T0=tables;_T0->pop_tables=Cyc_JumpAnalysis_make_ptr_table();_T1=tables;
_T1->succ_tables=Cyc_JumpAnalysis_make_ptr_table();_T2=tables;
_T2->pat_pop_tables=Cyc_JumpAnalysis_make_ptr_table();
Cyc_JumpAnalysis_jump_analysis_iter(tables,tds);_T3=tables;
return _T3;}

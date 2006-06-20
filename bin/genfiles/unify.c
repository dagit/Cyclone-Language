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
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 254
extern int Cyc_List_forall_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 165
enum Cyc_Absyn_AliasQualVal{Cyc_Absyn_Aliasable_qual =0U,Cyc_Absyn_Unique_qual =1U,Cyc_Absyn_Refcnt_qual =2U,Cyc_Absyn_Restricted_qual =3U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 939 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 974
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 1009
void*Cyc_Absyn_bounds_one (void);
# 1013
void*Cyc_Absyn_thinconst (void);
# 30 "warn.h"
extern int Cyc_Warn_num_errors;
extern int Cyc_Warn_max_errors;struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 77
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 99 "tcutil.h"
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 134
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 146
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 212
void*Cyc_Tcutil_normalize_effect(void*);
# 262
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 267
int Cyc_Tcutil_tycon_cmp(void*,void*);
# 274
int Cyc_Tcutil_cmp_effect_constraints(struct Cyc_List_List*,struct Cyc_List_List*);
# 277
int Cyc_Tcutil_is_cvar_type(void*);
void*Cyc_Tcutil_ptrbnd_cvar_equivalent(void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 36
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 81 "kinds.h"
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*);
# 83
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 89
void*Cyc_Kinds_compress_kb(void*);
# 93
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 54 "attributes.h"
int Cyc_Atts_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 58
int Cyc_Atts_equiv_fn_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Set_Set;
# 189 "assndef.h"
extern void*Cyc_AssnDef_fresh_var(struct Cyc_Absyn_Vardecl*,void*);struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};
# 233 "assndef.h"
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};
# 283
extern void*Cyc_AssnDef_existassnfn2assn(struct Cyc_AssnDef_ExistAssnFn*,struct Cyc_List_List*);
# 315
extern int Cyc_AssnDef_simple_prove(void*,void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 17 "bansheeif.h"
void*Cyc_BansheeIf_equality_constraint(void*,void*);
# 28
int Cyc_BansheeIf_add_constraint(unsigned,void*);char Cyc_Unify_Unify[6U]="Unify";struct Cyc_Unify_Unify_exn_struct{char*tag;};
# 38 "unify.cyc"
struct Cyc_Unify_Unify_exn_struct Cyc_Unify_Unify_val={Cyc_Unify_Unify};struct _tuple11{void*f0;void*f1;};
# 41
static struct _tuple11 Cyc_Unify_ts_failure={.f0=0,.f1=0};struct _tuple12{int f0;int f1;};
static struct _tuple12 Cyc_Unify_tqs_const={.f0=0,.f1=0};
static const char*Cyc_Unify_failure_reason=0;
# 45
static void Cyc_Unify_fail_because(const char*reason){struct Cyc_Unify_Unify_exn_struct*_T0;struct Cyc_Unify_Unify_exn_struct*_T1;
Cyc_Unify_failure_reason=reason;_T0=& Cyc_Unify_Unify_val;_T1=(struct Cyc_Unify_Unify_exn_struct*)_T0;_throw(_T1);}
# 53
void Cyc_Unify_explain_failure (void){struct _fat_ptr _T0;struct _fat_ptr _T1;void*_T2;void*_T3;unsigned _T4;int _T5;struct Cyc_String_pa_PrintArg_struct _T6;struct _fat_ptr _T7;void*_T8;void*_T9;unsigned _TA;struct Cyc___cycFILE*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;void*_T10;void*_T11;unsigned _T12;int _T13;struct Cyc_Absynpp_Params*_T14;struct Cyc_Absynpp_Params*_T15;struct _tuple11 _T16;struct _tuple11 _T17;struct _fat_ptr _T18;void*_T19;unsigned _T1A;struct _fat_ptr _T1B;void*_T1C;unsigned _T1D;struct Cyc_String_pa_PrintArg_struct _T1E;struct Cyc___cycFILE*_T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct _fat_ptr _T22;unsigned _T23;int _T24;struct Cyc___cycFILE*_T25;struct _fat_ptr _T26;struct _fat_ptr _T27;struct Cyc___cycFILE*_T28;struct _fat_ptr _T29;struct _fat_ptr _T2A;struct Cyc___cycFILE*_T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;int _T2E;unsigned _T2F;struct _fat_ptr _T30;unsigned _T31;unsigned _T32;struct Cyc___cycFILE*_T33;struct _fat_ptr _T34;struct _fat_ptr _T35;struct Cyc_String_pa_PrintArg_struct _T36;struct Cyc___cycFILE*_T37;struct _fat_ptr _T38;struct _fat_ptr _T39;struct _fat_ptr _T3A;unsigned _T3B;unsigned _T3C;int _T3D;struct Cyc___cycFILE*_T3E;struct _fat_ptr _T3F;struct _fat_ptr _T40;struct Cyc___cycFILE*_T41;struct _fat_ptr _T42;struct _fat_ptr _T43;int _T44;unsigned long _T45;struct _fat_ptr _T46;void*_T47;void*_T48;unsigned _T49;unsigned long _T4A;unsigned long _T4B;struct Cyc___cycFILE*_T4C;struct _fat_ptr _T4D;struct _fat_ptr _T4E;struct Cyc_String_pa_PrintArg_struct _T4F;struct _fat_ptr _T50;void*_T51;void*_T52;unsigned _T53;struct Cyc___cycFILE*_T54;struct _fat_ptr _T55;struct _fat_ptr _T56;struct Cyc___cycFILE*_T57;struct _fat_ptr _T58;struct _fat_ptr _T59;
if(Cyc_Warn_num_errors < Cyc_Warn_max_errors)goto _TL0;
return;_TL0:
 Cyc_fflush(Cyc_stderr);_T0=
# 59
_tag_fat("qualifiers don't match",sizeof(char),23U);{const char*_T5A=Cyc_Unify_failure_reason;_T2=(void*)_T5A;_T3=(void*)_T5A;_T4=_get_zero_arr_size_char(_T3,1U);_T1=_tag_fat(_T2,sizeof(char),_T4);}_T5=Cyc_strcmp(_T0,_T1);if(_T5!=0)goto _TL2;{struct Cyc_String_pa_PrintArg_struct _T5A;_T5A.tag=0;{const char*_T5B=Cyc_Unify_failure_reason;_T8=(void*)_T5B;_T9=(void*)_T5B;_TA=_get_zero_arr_size_char(_T9,1U);_T7=_tag_fat(_T8,sizeof(char),_TA);}
_T5A.f1=_T7;_T6=_T5A;}{struct Cyc_String_pa_PrintArg_struct _T5A=_T6;void*_T5B[1];_T5B[0]=& _T5A;_TB=Cyc_stderr;_TC=_tag_fat("  (%s)\n",sizeof(char),8U);_TD=_tag_fat(_T5B,sizeof(void*),1);Cyc_fprintf(_TB,_TC,_TD);}
return;_TL2: _TE=
# 64
_tag_fat("function effects do not match",sizeof(char),30U);{const char*_T5A=Cyc_Unify_failure_reason;_T10=(void*)_T5A;_T11=(void*)_T5A;_T12=_get_zero_arr_size_char(_T11,1U);_TF=_tag_fat(_T10,sizeof(char),_T12);}_T13=Cyc_strcmp(_TE,_TF);if(_T13!=0)goto _TL4;{
struct Cyc_Absynpp_Params p=Cyc_Absynpp_tc_params_r;
p.print_all_effects=1;_T14=& p;_T15=(struct Cyc_Absynpp_Params*)_T14;
Cyc_Absynpp_set_params(_T15);}goto _TL5;_TL4: _TL5: {void*_T5A;void*_T5B;_T16=Cyc_Unify_ts_failure;_T5B=_T16.f0;_T17=Cyc_Unify_ts_failure;_T5A=_T17.f1;{void*t1f=_T5B;void*t2f=_T5A;_T19=t1f;_T1A=(unsigned)_T19;
# 70
if(!_T1A)goto _TL6;_T18=Cyc_Absynpp_typ2string(t1f);goto _TL7;_TL6: _T18=_tag_fat("<?>",sizeof(char),4U);_TL7: {struct _fat_ptr s1=_T18;_T1C=t2f;_T1D=(unsigned)_T1C;
if(!_T1D)goto _TL8;_T1B=Cyc_Absynpp_typ2string(t2f);goto _TL9;_TL8: _T1B=_tag_fat("<?>",sizeof(char),4U);_TL9: {struct _fat_ptr s2=_T1B;
int pos=2;{struct Cyc_String_pa_PrintArg_struct _T5C;_T5C.tag=0;
_T5C.f1=s1;_T1E=_T5C;}{struct Cyc_String_pa_PrintArg_struct _T5C=_T1E;void*_T5D[1];_T5D[0]=& _T5C;_T1F=Cyc_stderr;_T20=_tag_fat("  %s",sizeof(char),5U);_T21=_tag_fat(_T5D,sizeof(void*),1);Cyc_fprintf(_T1F,_T20,_T21);}_T22=s1;_T23=
_get_fat_size(_T22,sizeof(char));pos=pos + _T23;_T24=pos + 5;
if(_T24 < 80)goto _TLA;_T25=Cyc_stderr;_T26=
_tag_fat("\n\t",sizeof(char),3U);_T27=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T25,_T26,_T27);
pos=8;goto _TLB;
# 79
_TLA: _T28=Cyc_stderr;_T29=_tag_fat(" ",sizeof(char),2U);_T2A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T28,_T29,_T2A);
pos=pos + 1;_TLB: _T2B=Cyc_stderr;_T2C=
# 82
_tag_fat("and ",sizeof(char),5U);_T2D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2B,_T2C,_T2D);
pos=pos + 4;_T2E=pos;_T2F=(unsigned)_T2E;_T30=s2;_T31=
_get_fat_size(_T30,sizeof(char));_T32=_T2F + _T31;if(_T32 < 80U)goto _TLC;_T33=Cyc_stderr;_T34=
_tag_fat("\n\t",sizeof(char),3U);_T35=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T33,_T34,_T35);
pos=8;goto _TLD;_TLC: _TLD:{struct Cyc_String_pa_PrintArg_struct _T5C;_T5C.tag=0;
# 88
_T5C.f1=s2;_T36=_T5C;}{struct Cyc_String_pa_PrintArg_struct _T5C=_T36;void*_T5D[1];_T5D[0]=& _T5C;_T37=Cyc_stderr;_T38=_tag_fat("%s ",sizeof(char),4U);_T39=_tag_fat(_T5D,sizeof(void*),1);Cyc_fprintf(_T37,_T38,_T39);}_T3A=s2;_T3B=
_get_fat_size(_T3A,sizeof(char));_T3C=_T3B + 1U;pos=pos + _T3C;_T3D=pos + 17;
if(_T3D < 80)goto _TLE;_T3E=Cyc_stderr;_T3F=
_tag_fat("\n\t",sizeof(char),3U);_T40=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3E,_T3F,_T40);
pos=8;goto _TLF;_TLE: _TLF: _T41=Cyc_stderr;_T42=
# 94
_tag_fat("are not compatible. ",sizeof(char),21U);_T43=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T41,_T42,_T43);
pos=pos + 17;
if(Cyc_Unify_failure_reason==0)goto _TL10;_T44=pos;_T45=(unsigned long)_T44;{const char*_T5C=Cyc_Unify_failure_reason;_T47=(void*)_T5C;_T48=(void*)_T5C;_T49=_get_zero_arr_size_char(_T48,1U);_T46=_tag_fat(_T47,sizeof(char),_T49);}_T4A=
Cyc_strlen(_T46);_T4B=_T45 + _T4A;if(_T4B < 80U)goto _TL12;_T4C=Cyc_stderr;_T4D=
_tag_fat("\n\t",sizeof(char),3U);_T4E=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4C,_T4D,_T4E);goto _TL13;_TL12: _TL13:{struct Cyc_String_pa_PrintArg_struct _T5C;_T5C.tag=0;{const char*_T5D=Cyc_Unify_failure_reason;_T51=(void*)_T5D;_T52=(void*)_T5D;_T53=_get_zero_arr_size_char(_T52,1U);_T50=_tag_fat(_T51,sizeof(char),_T53);}
_T5C.f1=_T50;_T4F=_T5C;}{struct Cyc_String_pa_PrintArg_struct _T5C=_T4F;void*_T5D[1];_T5D[0]=& _T5C;_T54=Cyc_stderr;_T55=_tag_fat("(%s)",sizeof(char),5U);_T56=_tag_fat(_T5D,sizeof(void*),1);Cyc_fprintf(_T54,_T55,_T56);}goto _TL11;_TL10: _TL11: _T57=Cyc_stderr;_T58=
# 101
_tag_fat("\n",sizeof(char),2U);_T59=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T57,_T58,_T59);
Cyc_fflush(Cyc_stderr);}}}}}
# 107
static int Cyc_Unify_check_logical_equivalence(struct Cyc_List_List*terms,struct Cyc_AssnDef_ExistAssnFn*eaf1opt,struct Cyc_AssnDef_ExistAssnFn*eaf2opt){void*_T0;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T1;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2;void*_T3;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T4;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T5;int _T6;int _T7;int _T8;int _T9;int _TA;
# 110
if(eaf1opt!=0)goto _TL14;_T1=& Cyc_AssnDef_true_assn;_T2=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T1;_T0=(void*)_T2;goto _TL15;_TL14: _T0=Cyc_AssnDef_existassnfn2assn(eaf1opt,terms);_TL15: {void*a1=_T0;
if(eaf2opt!=0)goto _TL16;_T4=& Cyc_AssnDef_true_assn;_T5=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T4;_T3=(void*)_T5;goto _TL17;_TL16: _T3=Cyc_AssnDef_existassnfn2assn(eaf2opt,terms);_TL17: {void*a2=_T3;_T7=
Cyc_AssnDef_simple_prove(a1,a2);if(_T7)goto _TL1A;else{goto _TL1B;}_TL1B: _T8=
Cyc_PrattProver_constraint_prove(a1,a2);
# 112
if(_T8)goto _TL1A;else{goto _TL18;}_TL1A: _TA=
# 115
Cyc_AssnDef_simple_prove(a2,a1);if(!_TA)goto _TL1C;_T9=1;goto _TL1D;_TL1C: _T9=
Cyc_PrattProver_constraint_prove(a2,a1);_TL1D: _T6=_T9;goto _TL19;_TL18: _T6=0;_TL19:
# 112
 return _T6;}}}
# 119
int Cyc_Unify_unify_kindbound(void*kb1,void*kb2){struct _tuple11 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;int _T9;void*_TA;void*_TB;void*_TC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TD;struct Cyc_Core_Opt**_TE;int _TF;struct Cyc_Core_Opt**_T10;struct Cyc_Core_Opt*_T11;void*_T12;int*_T13;unsigned _T14;int _T15;void*_T16;void*_T17;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T18;struct Cyc_Core_Opt**_T19;void*_T1A;void*_T1B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T1C;struct Cyc_Core_Opt**_T1D;int _T1E;struct Cyc_Core_Opt**_T1F;struct Cyc_Core_Opt*_T20;void*_T21;void*_T22;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T23;struct Cyc_Core_Opt**_T24;struct Cyc_Core_Opt**_T25;struct Cyc_Core_Opt*_T26;void*_T27;void*_T28;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T29;struct Cyc_Core_Opt**_T2A;{struct _tuple11 _T2B;
_T2B.f0=Cyc_Kinds_compress_kb(kb1);_T2B.f1=Cyc_Kinds_compress_kb(kb2);_T0=_T2B;}{struct _tuple11 _T2B=_T0;struct Cyc_Absyn_Kind*_T2C;void*_T2D;void*_T2E;void*_T2F;_T1=_T2B.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 0: _T4=_T2B.f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 0: _T7=_T2B.f0;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T7;_T2F=_T30->f1;}_T8=_T2B.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T8;_T2E=_T30->f1;}{struct Cyc_Absyn_Kind*k1=_T2F;struct Cyc_Absyn_Kind*k2=_T2E;_T9=k1==k2;
return _T9;}case 2: _TA=_T2B.f0;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_TA;_T2F=_T30->f1;}_TB=_T2B.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_TB;_TC=_T2B.f1;_TD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_TC;_TE=& _TD->f1;_T2E=(struct Cyc_Core_Opt**)_TE;_T2D=_T30->f2;}_LL8: {struct Cyc_Absyn_Kind*k1=_T2F;struct Cyc_Core_Opt**x=_T2E;struct Cyc_Absyn_Kind*k2=_T2D;_TF=
# 130
Cyc_Kinds_kind_leq(k1,k2);if(_TF)goto _TL20;else{goto _TL22;}
_TL22: return 0;_TL20: _T10=x;{struct Cyc_Core_Opt*_T30=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T30->v=kb1;_T11=(struct Cyc_Core_Opt*)_T30;}*_T10=_T11;
return 1;}default: goto _LLB;};case 2: _T12=_T2B.f1;_T13=(int*)_T12;_T14=*_T13;switch(_T14){case 0: _T15=
# 122
Cyc_Unify_unify_kindbound(kb2,kb1);return _T15;case 2: _T16=_T2B.f0;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T16;_T17=_T2B.f0;_T18=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T17;_T19=& _T18->f1;_T2F=(struct Cyc_Core_Opt**)_T19;_T2E=_T30->f2;}_T1A=_T2B.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T1A;_T1B=_T2B.f1;_T1C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T1B;_T1D=& _T1C->f1;_T2D=(struct Cyc_Core_Opt**)_T1D;_T2C=_T30->f2;}{struct Cyc_Core_Opt**y=_T2F;struct Cyc_Absyn_Kind*k1=_T2E;struct Cyc_Core_Opt**x=(struct Cyc_Core_Opt**)_T2D;struct Cyc_Absyn_Kind*k2=_T2C;_T1E=
# 124
Cyc_Kinds_kind_leq(k2,k1);if(!_T1E)goto _TL24;_T1F=y;{struct Cyc_Core_Opt*_T30=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T30->v=kb2;_T20=(struct Cyc_Core_Opt*)_T30;}*_T1F=_T20;
return 1;_TL24: _T2F=k1;_T2E=x;_T2D=k2;goto _LL8;}default: _LLB: _T2F=_T2B.f0;_T21=_T2B.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T21;_T22=_T2B.f1;_T23=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T22;_T24=& _T23->f1;_T2E=(struct Cyc_Core_Opt**)_T24;}_LLC: {void*y=_T2F;struct Cyc_Core_Opt**x=_T2E;_T25=x;{struct Cyc_Core_Opt*_T30=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 136
_T30->v=y;_T26=(struct Cyc_Core_Opt*)_T30;}*_T25=_T26;
return 1;}};default: _T27=_T2B.f0;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T27;_T28=_T2B.f0;_T29=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T28;_T2A=& _T29->f1;_T2F=(struct Cyc_Core_Opt**)_T2A;}_T2E=_T2B.f1;{struct Cyc_Core_Opt**x=(struct Cyc_Core_Opt**)_T2F;void*y=_T2E;_T2F=y;_T2E=x;goto _LLC;}};}}
# 143
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;int(*_T4)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);int(*_T5)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T6)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T7;struct Cyc_Absyn_Tvar*_T8;int _T9;void*_TA;void*_TB;void*_TC;struct Cyc_Absyn_Evar_Absyn_Type_struct*_TD;struct Cyc_Core_Opt**_TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;int(*_T10)(struct _fat_ptr);void*(*_T11)(struct _fat_ptr);struct _fat_ptr _T12;struct Cyc_Core_Opt**_T13;struct Cyc_Core_Opt*_T14;struct Cyc_Core_Opt*_T15;void*_T16;int _T17;int(*_T18)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);int(*_T19)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T1A)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;void*_T1D;struct Cyc_Absyn_Tvar*_T1E;int _T1F;struct Cyc_List_List*_T20;int _T21;struct Cyc_Core_Opt**_T22;struct Cyc_Core_Opt*_T23;struct Cyc_Core_Opt*_T24;void*_T25;int(*_T26)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);int(*_T27)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T28)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;void*_T2B;struct Cyc_Absyn_Tvar*_T2C;int _T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;void*_T30;struct Cyc_List_List*_T31;struct Cyc_Core_Opt**_T32;struct Cyc_Core_Opt*_T33;void*_T34;void*_T35;struct _RegionHandle*_T36;struct Cyc_List_List*_T37;struct Cyc_Absyn_PtrInfo _T38;void*_T39;void*_T3A;struct _RegionHandle*_T3B;struct Cyc_List_List*_T3C;struct Cyc_Absyn_PtrInfo _T3D;struct Cyc_Absyn_PtrAtts _T3E;void*_T3F;void*_T40;struct _RegionHandle*_T41;struct Cyc_List_List*_T42;struct Cyc_Absyn_PtrInfo _T43;struct Cyc_Absyn_PtrAtts _T44;void*_T45;void*_T46;struct _RegionHandle*_T47;struct Cyc_List_List*_T48;struct Cyc_Absyn_PtrInfo _T49;struct Cyc_Absyn_PtrAtts _T4A;void*_T4B;void*_T4C;struct _RegionHandle*_T4D;struct Cyc_List_List*_T4E;struct Cyc_Absyn_PtrInfo _T4F;struct Cyc_Absyn_PtrAtts _T50;void*_T51;void*_T52;struct _RegionHandle*_T53;struct Cyc_List_List*_T54;struct Cyc_Absyn_PtrInfo _T55;struct Cyc_Absyn_PtrAtts _T56;void*_T57;void*_T58;struct Cyc_Absyn_ArrayInfo _T59;struct Cyc_Absyn_ArrayInfo _T5A;void*_T5B;struct Cyc_Absyn_FnInfo _T5C;struct Cyc_Absyn_FnInfo _T5D;struct Cyc_Absyn_FnInfo _T5E;struct Cyc_Absyn_FnInfo _T5F;struct Cyc_Absyn_FnInfo _T60;struct Cyc_Absyn_FnInfo _T61;struct Cyc_Absyn_FnInfo _T62;void*_T63;struct _RegionHandle*_T64;struct Cyc_List_List*_T65;struct Cyc_List_List*_T66;void*_T67;struct _tuple8*_T68;struct _tuple8 _T69;void*_T6A;struct Cyc_List_List*_T6B;void*_T6C;struct _RegionHandle*_T6D;struct Cyc_List_List*_T6E;struct Cyc_Absyn_VarargInfo*_T6F;void*_T70;struct Cyc_List_List*_T71;void*_T72;struct Cyc_List_List*_T73;void*_T74;void*_T75;struct _RegionHandle*_T76;struct Cyc_List_List*_T77;struct Cyc_List_List*_T78;void*_T79;struct Cyc_Absyn_Aggrfield*_T7A;void*_T7B;struct Cyc_List_List*_T7C;void*_T7D;void*_T7E;void*_T7F;struct _RegionHandle*_T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;void*_T83;struct Cyc_List_List*_T84;
t=Cyc_Absyn_compress(t);{struct Cyc_List_List*_T85;struct Cyc_Absyn_VarargInfo*_T86;struct Cyc_List_List*_T87;struct Cyc_Absyn_Tqual _T88;struct Cyc_List_List*_T89;struct Cyc_Absyn_PtrInfo _T8A;void*_T8B;void*_T8C;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 2: _T3=t;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T3;_T8C=_T8D->f1;}{struct Cyc_Absyn_Tvar*tv=_T8C;_T5=Cyc_List_mem;{
# 147
int(*_T8D)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T5;_T4=_T8D;}_T6=Cyc_Tcutil_fast_tvar_cmp;_T7=env;_T8=tv;_T9=_T4(_T6,_T7,_T8);if(_T9)goto _TL27;else{goto _TL29;}
_TL29: Cyc_Unify_fail_because("type variable would escape scope");goto _TL28;_TL27: _TL28: goto _LL0;}case 1: _TA=t;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TA;_TB=_T8D->f2;_T8C=(void*)_TB;_TC=t;_TD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TC;_TE=& _TD->f4;_T8B=(struct Cyc_Core_Opt**)_TE;}{void*rg=_T8C;struct Cyc_Core_Opt**sopt=_T8B;
# 151
if(t!=evar)goto _TL2A;
Cyc_Unify_fail_because("occurs check");goto _TL2B;_TL2A: _TL2B:
 if(rg==0)goto _TL2C;{struct Cyc_Warn_String_Warn_Warg_struct _T8D;_T8D.tag=0;
_T8D.f1=_tag_fat("occurs check: constrained Evar in compressed type",sizeof(char),50U);_TF=_T8D;}{struct Cyc_Warn_String_Warn_Warg_struct _T8D=_TF;void*_T8E[1];_T8E[0]=& _T8D;_T11=Cyc_Warn_impos2;{int(*_T8F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T11;_T10=_T8F;}_T12=_tag_fat(_T8E,sizeof(void*),1);_T10(_T12);}goto _TL2D;_TL2C: _TL2D: {
# 156
int problem=0;_T13=sopt;_T14=*_T13;_T15=
_check_null(_T14);_T16=_T15->v;{struct Cyc_List_List*s=(struct Cyc_List_List*)_T16;_TL31: if(s!=0)goto _TL32;else{goto _TL30;}_TL32: _T17=problem;if(_T17)goto _TL30;else{goto _TL2F;}
_TL2F: _T19=Cyc_List_mem;{int(*_T8D)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T19;_T18=_T8D;}_T1A=Cyc_Tcutil_fast_tvar_cmp;_T1B=env;_T1C=s;_T1D=_T1C->hd;_T1E=(struct Cyc_Absyn_Tvar*)_T1D;_T1F=_T18(_T1A,_T1B,_T1E);if(_T1F)goto _TL33;else{goto _TL35;}
_TL35: problem=1;goto _TL34;_TL33: _TL34: _T20=s;
# 157
s=_T20->tl;goto _TL31;_TL30:;}_T21=problem;
# 161
if(!_T21)goto _TL36;{
struct Cyc_List_List*result=0;_T22=sopt;_T23=*_T22;_T24=
_check_null(_T23);_T25=_T24->v;{struct Cyc_List_List*s=(struct Cyc_List_List*)_T25;_TL3B: if(s!=0)goto _TL39;else{goto _TL3A;}
_TL39: _T27=Cyc_List_mem;{int(*_T8D)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T27;_T26=_T8D;}_T28=Cyc_Tcutil_fast_tvar_cmp;_T29=env;_T2A=s;_T2B=_T2A->hd;_T2C=(struct Cyc_Absyn_Tvar*)_T2B;_T2D=_T26(_T28,_T29,_T2C);if(!_T2D)goto _TL3C;{struct Cyc_List_List*_T8D=_cycalloc(sizeof(struct Cyc_List_List));_T2F=s;_T30=_T2F->hd;
_T8D->hd=(struct Cyc_Absyn_Tvar*)_T30;_T8D->tl=result;_T2E=(struct Cyc_List_List*)_T8D;}result=_T2E;goto _TL3D;_TL3C: _TL3D: _T31=s;
# 163
s=_T31->tl;goto _TL3B;_TL3A:;}_T32=sopt;{struct Cyc_Core_Opt*_T8D=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 166
_T8D->v=result;_T33=(struct Cyc_Core_Opt*)_T8D;}*_T32=_T33;}goto _TL37;_TL36: _TL37: goto _LL0;}}case 4: _T34=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T34;_T8A=_T8D->f1;}{struct Cyc_Absyn_PtrInfo pinfo=_T8A;_T35=evar;_T36=r;_T37=env;_T38=pinfo;_T39=_T38.elt_type;
# 170
Cyc_Unify_occurs(_T35,_T36,_T37,_T39);_T3A=evar;_T3B=r;_T3C=env;_T3D=pinfo;_T3E=_T3D.ptr_atts;_T3F=_T3E.eff;
Cyc_Unify_occurs(_T3A,_T3B,_T3C,_T3F);_T40=evar;_T41=r;_T42=env;_T43=pinfo;_T44=_T43.ptr_atts;_T45=_T44.nullable;
Cyc_Unify_occurs(_T40,_T41,_T42,_T45);_T46=evar;_T47=r;_T48=env;_T49=pinfo;_T4A=_T49.ptr_atts;_T4B=_T4A.bounds;
Cyc_Unify_occurs(_T46,_T47,_T48,_T4B);_T4C=evar;_T4D=r;_T4E=env;_T4F=pinfo;_T50=_T4F.ptr_atts;_T51=_T50.zero_term;
Cyc_Unify_occurs(_T4C,_T4D,_T4E,_T51);_T52=evar;_T53=r;_T54=env;_T55=pinfo;_T56=_T55.ptr_atts;_T57=_T56.autoreleased;
Cyc_Unify_occurs(_T52,_T53,_T54,_T57);goto _LL0;}case 5: _T58=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T58;_T59=_T8D->f1;_T8C=_T59.elt_type;_T5A=_T8D->f1;_T8B=_T5A.zero_term;}{void*t2=_T8C;void*zt=_T8B;
# 179
Cyc_Unify_occurs(evar,r,env,t2);
Cyc_Unify_occurs(evar,r,env,zt);goto _LL0;}case 6: _T5B=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T5B;_T5C=_T8D->f1;_T89=_T5C.tvars;_T5D=_T8D->f1;_T8C=_T5D.effect;_T5E=_T8D->f1;_T88=_T5E.ret_tqual;_T5F=_T8D->f1;_T8B=_T5F.ret_type;_T60=_T8D->f1;_T87=_T60.args;_T61=_T8D->f1;_T86=_T61.cyc_varargs;_T62=_T8D->f1;_T85=_T62.qual_bnd;}{struct Cyc_List_List*tvs=_T89;void*eff=_T8C;struct Cyc_Absyn_Tqual rt_tq=_T88;void*rt=_T8B;struct Cyc_List_List*args=_T87;struct Cyc_Absyn_VarargInfo*cyc_varargs=_T86;struct Cyc_List_List*rgn_po=_T85;
# 184
env=Cyc_List_rappend(r,tvs,env);
if(eff==0)goto _TL3E;
Cyc_Unify_occurs(evar,r,env,eff);goto _TL3F;_TL3E: _TL3F:
 Cyc_Unify_occurs(evar,r,env,rt);
_TL43: if(args!=0)goto _TL41;else{goto _TL42;}
_TL41: _T63=evar;_T64=r;_T65=env;_T66=args;_T67=_T66->hd;_T68=(struct _tuple8*)_T67;_T69=*_T68;_T6A=_T69.f2;Cyc_Unify_occurs(_T63,_T64,_T65,_T6A);_T6B=args;
# 188
args=_T6B->tl;goto _TL43;_TL42:
# 190
 if(cyc_varargs==0)goto _TL44;_T6C=evar;_T6D=r;_T6E=env;_T6F=cyc_varargs;_T70=_T6F->type;
Cyc_Unify_occurs(_T6C,_T6D,_T6E,_T70);goto _TL45;_TL44: _TL45:
 _TL49: if(rgn_po!=0)goto _TL47;else{goto _TL48;}
_TL47: _T71=rgn_po;_T72=_T71->hd;{struct _tuple11*_T8D=(struct _tuple11*)_T72;void*_T8E;void*_T8F;{struct _tuple11 _T90=*_T8D;_T8F=_T90.f0;_T8E=_T90.f1;}{void*r1=_T8F;void*r2=_T8E;
Cyc_Unify_occurs(evar,r,env,r1);
Cyc_Unify_occurs(evar,r,env,r2);}}_T73=rgn_po;
# 192
rgn_po=_T73->tl;goto _TL49;_TL48: goto _LL0;}case 7: _T74=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T74;_T89=_T8D->f3;}{struct Cyc_List_List*fs=_T89;
# 200
_TL4D: if(fs!=0)goto _TL4B;else{goto _TL4C;}
_TL4B: _T75=evar;_T76=r;_T77=env;_T78=fs;_T79=_T78->hd;_T7A=(struct Cyc_Absyn_Aggrfield*)_T79;_T7B=_T7A->type;Cyc_Unify_occurs(_T75,_T76,_T77,_T7B);_T7C=fs;
# 200
fs=_T7C->tl;goto _TL4D;_TL4C: goto _LL0;}case 8: _T7D=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T7D;_T89=_T8D->f2;}{struct Cyc_List_List*ts=_T89;_T89=ts;goto _LL10;}case 0: _T7E=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7E;_T89=_T8D->f2;}_LL10: {struct Cyc_List_List*ts=_T89;
# 205
_TL51: if(ts!=0)goto _TL4F;else{goto _TL50;}
_TL4F: _T7F=evar;_T80=r;_T81=env;_T82=ts;_T83=_T82->hd;Cyc_Unify_occurs(_T7F,_T80,_T81,_T83);_T84=ts;
# 205
ts=_T84->tl;goto _TL51;_TL50: goto _LL0;}default: goto _LL0;}_LL0:;}}
# 214
static void Cyc_Unify_unify_it(void*,void*,int(*)(void*,void*,void*),void*);
# 218
static int Cyc_Unify_cvar_default(int a,void*t1,void*t2){
return 0;}
# 222
int Cyc_Unify_unify(void*t1,void*t2){struct _handler_cons*_T0;int _T1;void(*_T2)(void*,void*,int(*)(int,void*,void*),int);void(*_T3)(void*,void*,int(*)(void*,void*,void*),void*);void*_T4;void*_T5;void*_T6;struct Cyc_Unify_Unify_exn_struct*_T7;char*_T8;char*_T9;struct _handler_cons _TA;_T0=& _TA;_push_handler(_T0);{int _TB=0;_T1=setjmp(_TA.handler);if(!_T1)goto _TL52;_TB=1;goto _TL53;_TL52: _TL53: if(_TB)goto _TL54;else{goto _TL56;}_TL56: _T3=Cyc_Unify_unify_it;{
# 224
void(*_TC)(void*,void*,int(*)(int,void*,void*),int)=(void(*)(void*,void*,int(*)(int,void*,void*),int))_T3;_T2=_TC;}_T4=t1;_T5=t2;_T2(_T4,_T5,Cyc_Unify_cvar_default,0);{int _TC=1;_npop_handler(0);return _TC;}_pop_handler();goto _TL55;_TL54: _T6=Cyc_Core_get_exn_thrown();{void*_TC=(void*)_T6;void*_TD;_T7=(struct Cyc_Unify_Unify_exn_struct*)_TC;_T8=_T7->tag;_T9=Cyc_Unify_Unify;if(_T8!=_T9)goto _TL57;
# 232
return 0;_TL57: _TD=_TC;{void*exn=_TD;_rethrow(exn);};}_TL55:;}}
# 236
int Cyc_Unify_unify_c(void*t1,void*t2,int(*f)(void*,void*,void*),void*e){struct _handler_cons*_T0;int _T1;void*_T2;void*_T3;int(*_T4)(void*,void*,void*);void*_T5;void*_T6;struct Cyc_Unify_Unify_exn_struct*_T7;char*_T8;char*_T9;struct _handler_cons _TA;_T0=& _TA;_push_handler(_T0);{int _TB=0;_T1=setjmp(_TA.handler);if(!_T1)goto _TL59;_TB=1;goto _TL5A;_TL59: _TL5A: if(_TB)goto _TL5B;else{goto _TL5D;}_TL5D: _T2=t1;_T3=t2;_T4=f;_T5=e;
# 240
Cyc_Unify_unify_it(_T2,_T3,_T4,_T5);{int _TC=1;_npop_handler(0);return _TC;}_pop_handler();goto _TL5C;_TL5B: _T6=Cyc_Core_get_exn_thrown();{void*_TC=(void*)_T6;void*_TD;_T7=(struct Cyc_Unify_Unify_exn_struct*)_TC;_T8=_T7->tag;_T9=Cyc_Unify_Unify;if(_T8!=_T9)goto _TL5E;
# 248
return 0;_TL5E: _TD=_TC;{void*exn=_TD;_rethrow(exn);};}_TL5C:;}}
# 253
static int Cyc_Unify_unify_cvar(struct Cyc_List_List**env,void*t1,void*t2){int _T0;int _T1;void*_T2;unsigned _T3;void*_T4;unsigned _T5;struct Cyc_List_List**_T6;struct Cyc_List_List*_T7;struct Cyc_List_List**_T8;_T0=
Cyc_Tcutil_is_cvar_type(t1);if(_T0)goto _TL62;else{goto _TL63;}_TL63: _T1=Cyc_Tcutil_is_cvar_type(t2);if(_T1)goto _TL62;else{goto _TL60;}
_TL62:{void*cv1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*cv2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);_T2=cv1;_T3=(unsigned)_T2;
if(!_T3)goto _TL64;_T4=cv2;_T5=(unsigned)_T4;if(!_T5)goto _TL64;_T6=env;{struct Cyc_List_List*_T9=_cycalloc(sizeof(struct Cyc_List_List));
_T9->hd=Cyc_BansheeIf_equality_constraint(cv1,cv2);_T8=_check_null(env);_T9->tl=*_T8;_T7=(struct Cyc_List_List*)_T9;}*_T6=_T7;
return 1;_TL64:;}goto _TL61;_TL60: _TL61:
# 262
 return 0;}
# 265
int Cyc_Unify_unify_and_constrain(unsigned loc,void*t1,void*t2){int(*_T0)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**);void*_T1;void*_T2;struct Cyc_List_List**_T3;int _T4;int(*_T5)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*);int(*_T6)(int(*)(void*,void*),void*,struct Cyc_List_List*);int(*_T7)(unsigned,void*);unsigned _T8;struct Cyc_List_List*_T9;int _TA;
struct Cyc_List_List*constraints=0;{
int(*_TB)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**)=(int(*)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**))Cyc_Unify_unify_c;_T0=_TB;}_T1=t1;_T2=t2;_T3=& constraints;_T4=_T0(_T1,_T2,Cyc_Unify_unify_cvar,_T3);if(!_T4)goto _TL66;_T6=Cyc_List_forall_c;{
int(*_TB)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(int(*)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T6;_T5=_TB;}_T7=Cyc_BansheeIf_add_constraint;_T8=loc;_T9=constraints;_TA=_T5(_T7,_T8,_T9);return _TA;_TL66:
# 270
 return 0;}
# 275
static void Cyc_Unify_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2,int(*f)(void*,void*,void*),void*e){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;int(*_T4)(void*,void*,void*);void*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Unify_Unify_exn_struct*_T8;struct Cyc_Unify_Unify_exn_struct*_T9;
# 278
_TL6B: if(t1!=0)goto _TL6C;else{goto _TL6A;}_TL6C: if(t2!=0)goto _TL69;else{goto _TL6A;}
_TL69: _T0=t1;_T1=_T0->hd;_T2=t2;_T3=_T2->hd;_T4=f;_T5=e;Cyc_Unify_unify_it(_T1,_T3,_T4,_T5);_T6=t1;
# 278
t1=_T6->tl;_T7=t2;t2=_T7->tl;goto _TL6B;_TL6A:
# 280
 if(t1!=0)goto _TL6F;else{goto _TL70;}_TL70: if(t2!=0)goto _TL6F;else{goto _TL6D;}
_TL6F: _T8=& Cyc_Unify_Unify_val;_T9=(struct Cyc_Unify_Unify_exn_struct*)_T8;_throw(_T9);goto _TL6E;_TL6D: _TL6E:;}
# 285
static void Cyc_Unify_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){struct Cyc_Absyn_Tqual _T0;int _T1;struct Cyc_Absyn_Tqual _T2;int _T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;int(*_T5)(struct _fat_ptr);void*(*_T6)(struct _fat_ptr);struct _fat_ptr _T7;struct Cyc_Absyn_Tqual _T8;int _T9;struct Cyc_Absyn_Tqual _TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;int(*_TD)(struct _fat_ptr);void*(*_TE)(struct _fat_ptr);struct _fat_ptr _TF;struct Cyc_Absyn_Tqual _T10;int _T11;struct Cyc_Absyn_Tqual _T12;int _T13;struct Cyc_Absyn_Tqual _T14;int _T15;struct Cyc_Absyn_Tqual _T16;int _T17;struct Cyc_Absyn_Tqual _T18;int _T19;struct Cyc_Absyn_Tqual _T1A;int _T1B;struct _tuple11 _T1C;struct _tuple12 _T1D;struct Cyc_Absyn_Tqual _T1E;struct Cyc_Absyn_Tqual _T1F;struct Cyc_Unify_Unify_exn_struct*_T20;struct Cyc_Unify_Unify_exn_struct*_T21;struct _tuple12 _T22;_T0=tq1;_T1=_T0.print_const;
if(!_T1)goto _TL71;_T2=tq1;_T3=_T2.real_const;if(_T3)goto _TL71;else{goto _TL73;}
_TL73:{struct Cyc_Warn_String_Warn_Warg_struct _T23;_T23.tag=0;_T23.f1=_tag_fat("tq1 real_const not set.",sizeof(char),24U);_T4=_T23;}{struct Cyc_Warn_String_Warn_Warg_struct _T23=_T4;void*_T24[1];_T24[0]=& _T23;_T6=Cyc_Warn_impos2;{int(*_T25)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T6;_T5=_T25;}_T7=_tag_fat(_T24,sizeof(void*),1);_T5(_T7);}goto _TL72;_TL71: _TL72: _T8=tq2;_T9=_T8.print_const;
if(!_T9)goto _TL74;_TA=tq2;_TB=_TA.real_const;if(_TB)goto _TL74;else{goto _TL76;}
_TL76:{struct Cyc_Warn_String_Warn_Warg_struct _T23;_T23.tag=0;_T23.f1=_tag_fat("tq2 real_const not set.",sizeof(char),24U);_TC=_T23;}{struct Cyc_Warn_String_Warn_Warg_struct _T23=_TC;void*_T24[1];_T24[0]=& _T23;_TE=Cyc_Warn_impos2;{int(*_T25)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TE;_TD=_T25;}_TF=_tag_fat(_T24,sizeof(void*),1);_TD(_TF);}goto _TL75;_TL74: _TL75: _T10=tq1;_T11=_T10.real_const;_T12=tq2;_T13=_T12.real_const;
# 291
if(_T11!=_T13)goto _TL79;else{goto _TL7B;}_TL7B: _T14=tq1;_T15=_T14.q_volatile;_T16=tq2;_T17=_T16.q_volatile;if(_T15!=_T17)goto _TL79;else{goto _TL7A;}_TL7A: _T18=tq1;_T19=_T18.q_restrict;_T1A=tq2;_T1B=_T1A.q_restrict;if(_T19!=_T1B)goto _TL79;else{goto _TL77;}
# 294
_TL79:{struct _tuple11 _T23;_T23.f0=t1;_T23.f1=t2;_T1C=_T23;}Cyc_Unify_ts_failure=_T1C;{struct _tuple12 _T23;_T1E=tq1;
_T23.f0=_T1E.real_const;_T1F=tq2;_T23.f1=_T1F.real_const;_T1D=_T23;}Cyc_Unify_tqs_const=_T1D;
Cyc_Unify_failure_reason="qualifiers don't match";_T20=& Cyc_Unify_Unify_val;_T21=(struct Cyc_Unify_Unify_exn_struct*)_T20;_throw(_T21);goto _TL78;_TL77: _TL78:{struct _tuple12 _T23;
# 300
_T23.f0=0;_T23.f1=0;_T22=_T23;}Cyc_Unify_tqs_const=_T22;}
# 314 "unify.cyc"
static int Cyc_Unify_unify_effect(void*e1,void*e2){int _T0;int _T1;int _T2;int _T3;
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);_T0=
Cyc_Tcutil_subset_effect(0,e1,e2);if(!_T0)goto _TL7C;_T1=Cyc_Tcutil_subset_effect(0,e2,e1);if(!_T1)goto _TL7C;
return 1;_TL7C: _T2=
Cyc_Tcutil_subset_effect(1,e1,e2);if(!_T2)goto _TL7E;_T3=Cyc_Tcutil_subset_effect(1,e2,e1);if(!_T3)goto _TL7E;
return 1;_TL7E:
 return 0;}
# 324
static int Cyc_Unify_unify_const_exp_opt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int _T0;
if(e1!=0)goto _TL80;if(e2!=0)goto _TL80;
return 1;_TL80:
 if(e1==0)goto _TL84;else{goto _TL85;}_TL85: if(e2==0)goto _TL84;else{goto _TL82;}
_TL84: return 0;_TL82: _T0=
Cyc_Evexp_same_uint_const_exp(e1,e2);return _T0;}struct _tuple13{struct Cyc_Absyn_Tvar*f0;void*f1;};struct _tuple14{struct Cyc_Absyn_VarargInfo*f0;struct Cyc_Absyn_VarargInfo*f1;};
# 333
static void Cyc_Unify_unify_it(void*t1,void*t2,int(*cvaruf)(void*,void*,void*),void*env){struct _tuple11 _T0;struct _tuple11 _T1;void*_T2;int*_T3;int _T4;void*_T5;int*_T6;int _T7;void*_T8;void*_T9;struct Cyc_Absyn_Evar_Absyn_Type_struct*_TA;struct Cyc_Core_Opt**_TB;void*_TC;struct Cyc_Absyn_Evar_Absyn_Type_struct*_TD;void**_TE;void*_TF;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T10;struct Cyc_Core_Opt**_T11;void*_T12;void*_T13;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T14;struct Cyc_Core_Opt**_T15;void*_T16;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T17;struct Cyc_Core_Opt**_T18;struct Cyc_Core_Opt**_T19;struct Cyc_Core_Opt*_T1A;struct Cyc_Core_Opt*_T1B;void*_T1C;struct Cyc_Absyn_Kind*_T1D;struct Cyc_Core_Opt**_T1E;struct Cyc_Core_Opt*_T1F;struct Cyc_Core_Opt*_T20;void*_T21;struct Cyc_Absyn_Kind*_T22;int _T23;struct Cyc_Core_Opt**_T24;struct Cyc_Core_Opt**_T25;struct Cyc_Core_Opt**_T26;struct Cyc_Core_Opt*_T27;struct Cyc_Core_Opt*_T28;void*_T29;struct Cyc_Absyn_Kind*_T2A;struct Cyc_Core_Opt**_T2B;struct Cyc_Core_Opt*_T2C;struct Cyc_Core_Opt*_T2D;void*_T2E;struct Cyc_Absyn_Kind*_T2F;int _T30;struct Cyc_Core_Opt**_T31;struct Cyc_Core_Opt**_T32;void**_T33;struct Cyc_Core_Opt**_T34;struct Cyc_Core_Opt*_T35;struct Cyc_Core_Opt*_T36;void*_T37;struct Cyc_Core_Opt**_T38;struct Cyc_Core_Opt*_T39;struct Cyc_Core_Opt*_T3A;void*_T3B;int(*_T3C)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);int(*_T3D)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T3E)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;void*_T41;struct Cyc_Absyn_Tvar*_T42;int _T43;struct Cyc_List_List*_T44;struct Cyc_Core_Opt**_T45;struct Cyc_Core_Opt**_T46;struct Cyc_Core_Opt**_T47;struct Cyc_Core_Opt*_T48;struct Cyc_Core_Opt*_T49;void*_T4A;int(*_T4B)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);int(*_T4C)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T4D)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;void*_T50;struct Cyc_Absyn_Tvar*_T51;int _T52;struct Cyc_List_List*_T53;struct Cyc_Core_Opt**_T54;struct Cyc_Core_Opt**_T55;struct Cyc_Core_Opt**_T56;struct Cyc_Core_Opt*_T57;struct Cyc_Core_Opt*_T58;void*_T59;int(*_T5A)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);int(*_T5B)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T5C)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T5D;struct Cyc_List_List*_T5E;void*_T5F;struct Cyc_Absyn_Tvar*_T60;int _T61;struct Cyc_List_List*_T62;struct Cyc_List_List*_T63;void*_T64;struct Cyc_List_List*_T65;struct Cyc_Core_Opt**_T66;struct Cyc_Core_Opt**_T67;struct Cyc_Core_Opt*_T68;void*_T69;void*_T6A;int(*_T6B)(void*,void*,void*);void*_T6C;void*_T6D;int*_T6E;int _T6F;void*_T70;void*_T71;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T72;void**_T73;void*_T74;struct _RegionHandle*_T75;struct Cyc_Core_Opt*_T76;void*_T77;struct Cyc_List_List*_T78;void*_T79;struct Cyc_Absyn_Kind*_T7A;struct Cyc_Core_Opt*_T7B;void*_T7C;struct Cyc_Absyn_Kind*_T7D;int _T7E;void**_T7F;void*_T80;int*_T81;int _T82;void*_T83;struct Cyc_Core_Opt*_T84;void*_T85;struct Cyc_Absyn_Kind*_T86;enum Cyc_Absyn_KindQual _T87;int _T88;struct Cyc_Absyn_PtrInfo _T89;struct Cyc_Absyn_PtrAtts _T8A;void*_T8B;void*_T8C;int*_T8D;unsigned _T8E;void*_T8F;void*_T90;void*_T91;void*_T92;void*_T93;int(*_T94)(void*,void*,void*);void*_T95;int _T96;void**_T97;struct _tuple11 _T98;struct _tuple11 _T99;void*_T9A;int*_T9B;int _T9C;void*_T9D;int*_T9E;int _T9F;int _TA0;void*_TA1;int*_TA2;int _TA3;void*_TA4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA5;void*_TA6;int*_TA7;int _TA8;void*_TA9;int*_TAA;int _TAB;void*_TAC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAD;void*_TAE;int*_TAF;int _TB0;void*_TB1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB2;void*_TB3;int*_TB4;int _TB5;void*_TB6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB7;void*_TB8;int*_TB9;int _TBA;void*_TBB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TBC;void*_TBD;int*_TBE;unsigned _TBF;void*_TC0;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC1;void*_TC2;int*_TC3;unsigned _TC4;void*_TC5;void*_TC6;void*_TC7;void*_TC8;enum Cyc_Absyn_AliasQualVal _TC9;int _TCA;enum Cyc_Absyn_AliasQualVal _TCB;int _TCC;struct _tuple11 _TCD;void*_TCE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TCF;struct Cyc_List_List*_TD0;void*_TD1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD2;struct Cyc_List_List*_TD3;struct Cyc_List_List*_TD4;void*_TD5;int*_TD6;unsigned _TD7;void*_TD8;void*_TD9;int(*_TDA)(void*,void*,void*);void*_TDB;void*_TDC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TDD;struct Cyc_List_List*_TDE;void*_TDF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE0;struct Cyc_List_List*_TE1;struct Cyc_List_List*_TE2;void*_TE3;int*_TE4;unsigned _TE5;void*_TE6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE7;void*_TE8;int*_TE9;unsigned _TEA;void*_TEB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TEC;struct Cyc_List_List*_TED;void*_TEE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TEF;struct Cyc_List_List*_TF0;struct Cyc_List_List*_TF1;void*_TF2;int*_TF3;unsigned _TF4;void*_TF5;struct Cyc_List_List*_TF6;void*_TF7;void*_TF8;struct Cyc_List_List*_TF9;void*_TFA;struct Cyc_Absyn_Tvar*_TFB;int _TFC;struct Cyc_Absyn_Tvar*_TFD;int _TFE;void*_TFF;void*_T100;int(*_T101)(void*,void*,void*);void*_T102;void*_T103;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T104;void*_T105;int*_T106;unsigned _T107;void*_T108;struct Cyc_List_List*_T109;void*_T10A;void*_T10B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10C;struct Cyc_List_List*_T10D;void*_T10E;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T10F;void**_T110;void*_T111;void*_T112;struct Cyc_Core_Opt*_T113;void*_T114;struct Cyc_Absyn_Kind*_T115;struct Cyc_Absyn_Kind*_T116;struct Cyc_Absyn_Kind*_T117;int _T118;void**_T119;void*_T11A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11B;struct Cyc_List_List*_T11C;void*_T11D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11E;struct Cyc_List_List*_T11F;struct Cyc_List_List*_T120;void*_T121;int*_T122;unsigned _T123;void*_T124;struct Cyc_List_List*_T125;void*_T126;void*_T127;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T128;struct Cyc_List_List*_T129;void*_T12A;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T12B;void**_T12C;void*_T12D;struct Cyc_List_List*_T12E;void*_T12F;void*_T130;struct _RegionHandle*_T131;struct Cyc_Core_Opt*_T132;void*_T133;struct Cyc_List_List*_T134;void*_T135;struct Cyc_Core_Opt*_T136;void*_T137;struct Cyc_Absyn_Kind*_T138;struct Cyc_Absyn_Kind*_T139;int _T13A;void**_T13B;void*_T13C;struct Cyc_List_List*_T13D;void*_T13E;void*_T13F;struct Cyc_List_List*_T140;void*_T141;void*_T142;void*_T143;int(*_T144)(void*,void*,void*);void*_T145;void*_T146;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T147;struct Cyc_List_List*_T148;struct Cyc_List_List*_T149;void*_T14A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T14B;void*_T14C;int*_T14D;int _T14E;void*_T14F;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T150;struct Cyc_List_List*_T151;struct Cyc_List_List*_T152;void*_T153;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T154;struct Cyc_List_List*_T155;void*_T156;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T157;void*_T158;int*_T159;int _T15A;void*_T15B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15C;struct Cyc_List_List*_T15D;void*_T15E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15F;struct Cyc_List_List*_T160;struct Cyc_List_List*_T161;void*_T162;int*_T163;int _T164;void*_T165;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T166;struct Cyc_List_List*_T167;struct Cyc_List_List*_T168;void*_T169;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T16A;void*_T16B;int*_T16C;int _T16D;void*_T16E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T16F;struct Cyc_List_List*_T170;struct Cyc_List_List*_T171;void*_T172;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T173;struct Cyc_List_List*_T174;void*_T175;struct Cyc_List_List*_T176;void*_T177;struct Cyc_List_List*_T178;void*_T179;void*_T17A;struct Cyc_List_List*_T17B;void*_T17C;struct Cyc_List_List*_T17D;void*_T17E;int _T17F;void*_T180;void*_T181;void*_T182;void*_T183;int _T184;struct Cyc_List_List*_T185;struct Cyc_List_List*_T186;int(*_T187)(void*,void*,void*);void*_T188;void*_T189;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18A;void*_T18B;int*_T18C;unsigned _T18D;void*_T18E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18F;struct Cyc_List_List*_T190;void*_T191;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T192;struct Cyc_List_List*_T193;struct Cyc_List_List*_T194;void*_T195;int*_T196;unsigned _T197;void*_T198;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T199;struct Cyc_List_List*_T19A;struct Cyc_List_List*_T19B;void*_T19C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T19D;void*_T19E;int*_T19F;int _T1A0;void*_T1A1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1A2;struct Cyc_List_List*_T1A3;struct Cyc_List_List*_T1A4;void*_T1A5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1A6;struct Cyc_List_List*_T1A7;void*_T1A8;int*_T1A9;int _T1AA;void*_T1AB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1AC;void*_T1AD;int*_T1AE;unsigned _T1AF;int _T1B0;void*_T1B1;int*_T1B2;unsigned _T1B3;void*_T1B4;int*_T1B5;unsigned _T1B6;void*_T1B7;int*_T1B8;int _T1B9;void*_T1BA;void*_T1BB;struct Cyc_Absyn_Tvar*_T1BC;int _T1BD;struct Cyc_Absyn_Tvar*_T1BE;int _T1BF;void*_T1C0;int*_T1C1;int _T1C2;void*_T1C3;struct Cyc_Absyn_PtrInfo _T1C4;struct Cyc_Absyn_PtrInfo _T1C5;struct Cyc_Absyn_PtrInfo _T1C6;struct Cyc_Absyn_PtrAtts _T1C7;struct Cyc_Absyn_PtrInfo _T1C8;struct Cyc_Absyn_PtrAtts _T1C9;struct Cyc_Absyn_PtrInfo _T1CA;struct Cyc_Absyn_PtrAtts _T1CB;struct Cyc_Absyn_PtrInfo _T1CC;struct Cyc_Absyn_PtrAtts _T1CD;struct Cyc_Absyn_PtrInfo _T1CE;struct Cyc_Absyn_PtrAtts _T1CF;struct Cyc_Absyn_PtrInfo _T1D0;struct Cyc_Absyn_PtrAtts _T1D1;void*_T1D2;struct Cyc_Absyn_PtrInfo _T1D3;struct Cyc_Absyn_PtrInfo _T1D4;struct Cyc_Absyn_PtrInfo _T1D5;struct Cyc_Absyn_PtrAtts _T1D6;struct Cyc_Absyn_PtrInfo _T1D7;struct Cyc_Absyn_PtrAtts _T1D8;struct Cyc_Absyn_PtrInfo _T1D9;struct Cyc_Absyn_PtrAtts _T1DA;struct Cyc_Absyn_PtrInfo _T1DB;struct Cyc_Absyn_PtrAtts _T1DC;struct Cyc_Absyn_PtrInfo _T1DD;struct Cyc_Absyn_PtrAtts _T1DE;struct Cyc_Absyn_PtrInfo _T1DF;struct Cyc_Absyn_PtrAtts _T1E0;void*_T1E1;void*_T1E2;int(*_T1E3)(void*,void*,void*);void*_T1E4;void*_T1E5;void*_T1E6;int(*_T1E7)(void*,void*,void*);void*_T1E8;void*_T1E9;void*_T1EA;int(*_T1EB)(void*,void*,void*);void*_T1EC;void*_T1ED;void*_T1EE;int(*_T1EF)(void*,void*,void*);void*_T1F0;int _T1F1;struct _tuple11 _T1F2;void*_T1F3;void*_T1F4;int(*_T1F5)(void*,void*,void*);void*_T1F6;int _T1F7;struct _tuple11 _T1F8;void*_T1F9;void*_T1FA;int(*_T1FB)(void*,void*,void*);void*_T1FC;int _T1FD;struct _tuple11 _T1FE;int*_T1FF;int _T200;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T201;void*_T202;int*_T203;int _T204;void*_T205;void*_T206;int(*_T207)(void*,void*,void*);void*_T208;void*_T209;int*_T20A;int _T20B;void*_T20C;void*_T20D;int _T20E;void*_T20F;int*_T210;int _T211;void*_T212;struct Cyc_Absyn_ArrayInfo _T213;struct Cyc_Absyn_ArrayInfo _T214;struct Cyc_Absyn_ArrayInfo _T215;struct Cyc_Absyn_ArrayInfo _T216;void*_T217;struct Cyc_Absyn_ArrayInfo _T218;struct Cyc_Absyn_ArrayInfo _T219;struct Cyc_Absyn_ArrayInfo _T21A;struct Cyc_Absyn_ArrayInfo _T21B;void*_T21C;void*_T21D;int(*_T21E)(void*,void*,void*);void*_T21F;void*_T220;void*_T221;int(*_T222)(void*,void*,void*);void*_T223;int _T224;void*_T225;int*_T226;int _T227;void*_T228;struct Cyc_Absyn_FnInfo _T229;struct Cyc_Absyn_FnInfo _T22A;struct Cyc_Absyn_FnInfo _T22B;struct Cyc_Absyn_FnInfo _T22C;struct Cyc_Absyn_FnInfo _T22D;struct Cyc_Absyn_FnInfo _T22E;struct Cyc_Absyn_FnInfo _T22F;struct Cyc_Absyn_FnInfo _T230;struct Cyc_Absyn_FnInfo _T231;struct Cyc_Absyn_FnInfo _T232;struct Cyc_Absyn_FnInfo _T233;struct Cyc_Absyn_FnInfo _T234;struct Cyc_Absyn_FnInfo _T235;struct Cyc_Absyn_FnInfo _T236;struct Cyc_Absyn_FnInfo _T237;struct Cyc_Absyn_FnInfo _T238;struct Cyc_Absyn_FnInfo _T239;struct Cyc_Absyn_FnInfo _T23A;struct Cyc_Absyn_FnInfo _T23B;struct Cyc_Absyn_FnInfo _T23C;void*_T23D;struct Cyc_Absyn_FnInfo _T23E;struct Cyc_Absyn_FnInfo _T23F;struct Cyc_Absyn_FnInfo _T240;struct Cyc_Absyn_FnInfo _T241;struct Cyc_Absyn_FnInfo _T242;struct Cyc_Absyn_FnInfo _T243;struct Cyc_Absyn_FnInfo _T244;struct Cyc_Absyn_FnInfo _T245;struct Cyc_Absyn_FnInfo _T246;struct Cyc_Absyn_FnInfo _T247;struct Cyc_Absyn_FnInfo _T248;struct Cyc_Absyn_FnInfo _T249;struct Cyc_Absyn_FnInfo _T24A;struct Cyc_Absyn_FnInfo _T24B;struct Cyc_Absyn_FnInfo _T24C;struct Cyc_Absyn_FnInfo _T24D;struct Cyc_Absyn_FnInfo _T24E;struct Cyc_Absyn_FnInfo _T24F;struct Cyc_Absyn_FnInfo _T250;struct Cyc_Absyn_FnInfo _T251;struct Cyc_List_List*_T252;void*_T253;struct Cyc_Absyn_Tvar*_T254;struct Cyc_List_List*_T255;void*_T256;struct Cyc_Absyn_Tvar*_T257;int _T258;struct _fat_ptr _T259;struct Cyc_String_pa_PrintArg_struct _T25A;struct Cyc_List_List*_T25B;void*_T25C;struct Cyc_Absyn_Tvar*_T25D;struct Cyc_String_pa_PrintArg_struct _T25E;struct Cyc_List_List*_T25F;void*_T260;struct Cyc_Absyn_Tvar*_T261;struct Cyc_Absyn_Kind*_T262;struct Cyc_Absyn_Kind*_T263;struct Cyc_Absyn_Kind*_T264;struct Cyc_String_pa_PrintArg_struct _T265;struct Cyc_List_List*_T266;void*_T267;struct Cyc_Absyn_Tvar*_T268;struct Cyc_Absyn_Kind*_T269;struct Cyc_Absyn_Kind*_T26A;struct Cyc_Absyn_Kind*_T26B;struct _fat_ptr _T26C;struct _fat_ptr _T26D;unsigned char*_T26E;const char*_T26F;struct Cyc_List_List*_T270;struct _RegionHandle*_T271;struct _tuple13*_T272;struct _RegionHandle*_T273;struct Cyc_List_List*_T274;void*_T275;struct Cyc_List_List*_T276;void*_T277;struct Cyc_Absyn_Tvar*_T278;struct Cyc_List_List*_T279;struct Cyc_List_List*_T27A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T27B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T27C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T27D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T27E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T27F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T280;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T281;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T282;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T283;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T284;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T285;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T286;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T287;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T288;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T289;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T28A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T28B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T28C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T28D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T28E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T28F;struct _RegionHandle*_T290;struct Cyc_List_List*_T291;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T292;void*_T293;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T294;void*_T295;void*_T296;int(*_T297)(void*,void*,void*);void*_T298;void*_T299;void*_T29A;int(*_T29B)(void*,void*,void*);void*_T29C;struct Cyc_List_List*_T29D;void*_T29E;struct _tuple8*_T29F;struct Cyc_List_List*_T2A0;void*_T2A1;struct _tuple8*_T2A2;void*_T2A3;void*_T2A4;int(*_T2A5)(void*,void*,void*);void*_T2A6;struct Cyc_List_List*_T2A7;struct Cyc_List_List*_T2A8;struct _tuple11 _T2A9;struct _tuple14 _T2AA;struct Cyc_Absyn_VarargInfo*_T2AB;struct Cyc_Absyn_VarargInfo*_T2AC;struct Cyc_Absyn_VarargInfo*_T2AD;struct Cyc_Absyn_VarargInfo*_T2AE;struct Cyc_Absyn_VarargInfo*_T2AF;void*_T2B0;void*_T2B1;int(*_T2B2)(void*,void*,void*);void*_T2B3;int _T2B4;struct _tuple11 _T2B5;int _T2B6;struct _fat_ptr _T2B7;struct Cyc_String_pa_PrintArg_struct _T2B8;void*_T2B9;unsigned _T2BA;struct _fat_ptr _T2BB;struct Cyc_String_pa_PrintArg_struct _T2BC;void*_T2BD;unsigned _T2BE;struct _fat_ptr _T2BF;struct _fat_ptr _T2C0;struct _fat_ptr _T2C1;unsigned char*_T2C2;const char*_T2C3;int _T2C4;int _T2C5;struct Cyc_List_List*_T2C6;struct Cyc_Absyn_Vardecl*_T2C7;struct Cyc_List_List*_T2C8;void*_T2C9;struct Cyc_Absyn_Vardecl*_T2CA;struct Cyc_List_List*_T2CB;void*_T2CC;void*_T2CD;struct Cyc_List_List*_T2CE;void*_T2CF;struct Cyc_Absyn_Vardecl*_T2D0;struct Cyc_List_List*_T2D1;void*_T2D2;struct Cyc_Absyn_Vardecl*_T2D3;struct Cyc_List_List*_T2D4;struct Cyc_List_List*_T2D5;int _T2D6;int _T2D7;struct Cyc_List_List*_T2D8;struct Cyc_List_List*_T2D9;struct Cyc_AssnDef_ExistAssnFn*_T2DA;struct Cyc_AssnDef_ExistAssnFn*_T2DB;int _T2DC;int _T2DD;void*_T2DE;int*_T2DF;int _T2E0;void*_T2E1;void*_T2E2;enum Cyc_Absyn_AggrKind _T2E3;int _T2E4;enum Cyc_Absyn_AggrKind _T2E5;int _T2E6;struct Cyc_List_List*_T2E7;void*_T2E8;struct Cyc_List_List*_T2E9;void*_T2EA;struct Cyc_Absyn_Aggrfield*_T2EB;struct _fat_ptr*_T2EC;struct Cyc_Absyn_Aggrfield*_T2ED;struct _fat_ptr*_T2EE;int _T2EF;struct Cyc_Absyn_Aggrfield*_T2F0;void*_T2F1;struct Cyc_Absyn_Aggrfield*_T2F2;void*_T2F3;int(*_T2F4)(void*,void*,void*);void*_T2F5;struct Cyc_Absyn_Aggrfield*_T2F6;struct Cyc_Absyn_Tqual _T2F7;struct Cyc_Absyn_Aggrfield*_T2F8;void*_T2F9;struct Cyc_Absyn_Aggrfield*_T2FA;struct Cyc_Absyn_Tqual _T2FB;struct Cyc_Absyn_Aggrfield*_T2FC;void*_T2FD;struct _tuple11 _T2FE;struct Cyc_Absyn_Aggrfield*_T2FF;struct Cyc_List_List*_T300;struct Cyc_Absyn_Aggrfield*_T301;struct Cyc_List_List*_T302;int _T303;struct Cyc_Absyn_Aggrfield*_T304;struct Cyc_Absyn_Exp*_T305;struct Cyc_Absyn_Aggrfield*_T306;struct Cyc_Absyn_Exp*_T307;int _T308;struct Cyc_Absyn_Aggrfield*_T309;struct Cyc_Absyn_Exp*_T30A;struct Cyc_Absyn_Aggrfield*_T30B;struct Cyc_Absyn_Exp*_T30C;int _T30D;struct Cyc_List_List*_T30E;struct Cyc_List_List*_T30F;void*_T310;int*_T311;int _T312;void*_T313;void*_T314;struct Cyc_List_List*_T315;struct Cyc_List_List*_T316;int(*_T317)(void*,void*,void*);void*_T318;struct Cyc_Unify_Unify_exn_struct*_T319;struct Cyc_Unify_Unify_exn_struct*_T31A;{struct _tuple11 _T31B;
# 336
_T31B.f0=t1;_T31B.f1=t2;_T0=_T31B;}Cyc_Unify_ts_failure=_T0;
Cyc_Unify_failure_reason=0;
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1!=t2)goto _TL86;return;_TL86:{struct _tuple11 _T31B;
_T31B.f0=t2;_T31B.f1=t1;_T1=_T31B;}{struct _tuple11 _T31B=_T1;struct Cyc_Absyn_Typedefdecl*_T31C;struct Cyc_Absyn_Typedefdecl*_T31D;enum Cyc_Absyn_AggrKind _T31E;enum Cyc_Absyn_AggrKind _T31F;struct Cyc_List_List*_T320;struct Cyc_List_List*_T321;struct Cyc_Absyn_Vardecl*_T322;struct Cyc_AssnDef_ExistAssnFn*_T323;struct Cyc_Absyn_Exp*_T324;struct Cyc_AssnDef_ExistAssnFn*_T325;struct Cyc_Absyn_Exp*_T326;struct Cyc_AssnDef_ExistAssnFn*_T327;struct Cyc_Absyn_Exp*_T328;struct Cyc_AssnDef_ExistAssnFn*_T329;struct Cyc_Absyn_Exp*_T32A;struct Cyc_List_List*_T32B;struct Cyc_List_List*_T32C;struct Cyc_Absyn_VarargInfo*_T32D;int _T32E;struct Cyc_List_List*_T32F;struct Cyc_List_List*_T330;struct Cyc_List_List*_T331;struct Cyc_List_List*_T332;struct Cyc_Absyn_Vardecl*_T333;struct Cyc_AssnDef_ExistAssnFn*_T334;struct Cyc_Absyn_Exp*_T335;struct Cyc_AssnDef_ExistAssnFn*_T336;struct Cyc_Absyn_Exp*_T337;struct Cyc_AssnDef_ExistAssnFn*_T338;struct Cyc_Absyn_Exp*_T339;struct Cyc_AssnDef_ExistAssnFn*_T33A;struct Cyc_List_List*_T33B;struct Cyc_List_List*_T33C;struct Cyc_Absyn_VarargInfo*_T33D;int _T33E;struct Cyc_List_List*_T33F;struct Cyc_List_List*_T340;struct Cyc_Absyn_Exp*_T341;void*_T342;void*_T343;void*_T344;void*_T345;void*_T346;void*_T347;struct Cyc_Absyn_Tqual _T348;void*_T349;void*_T34A;void*_T34B;struct Cyc_Absyn_Tqual _T34C;enum Cyc_Absyn_AliasQualVal _T34D;enum Cyc_Absyn_AliasQualVal _T34E;void*_T34F;void*_T350;void*_T351;void*_T352;void*_T353;_T2=_T31B.f0;_T3=(int*)_T2;_T4=*_T3;if(_T4!=1)goto _TL88;_T5=_T31B.f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=1)goto _TL8A;_T8=_T31B.f0;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T354=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T8;_T9=_T31B.f0;_TA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T9;_TB=& _TA->f1;_T353=(struct Cyc_Core_Opt**)_TB;_TC=_T31B.f0;_TD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TC;_TE=& _TD->f2;_T352=(void**)_TE;_TF=_T31B.f0;_T10=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TF;_T11=& _T10->f4;_T351=(struct Cyc_Core_Opt**)_T11;}_T12=_T31B.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T354=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T12;_T13=_T31B.f1;_T14=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T13;_T15=& _T14->f1;_T350=(struct Cyc_Core_Opt**)_T15;_T16=_T31B.f1;_T17=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T16;_T18=& _T17->f4;_T34F=(struct Cyc_Core_Opt**)_T18;}{struct Cyc_Core_Opt**kind1opt=_T353;void**t2r=_T352;struct Cyc_Core_Opt**s1opt=_T351;struct Cyc_Core_Opt**kind2opt=_T350;struct Cyc_Core_Opt**s2opt=_T34F;_T19=kind1opt;_T1A=*_T19;_T1B=
# 343
_check_null(_T1A);_T1C=_T1B->v;_T1D=(struct Cyc_Absyn_Kind*)_T1C;_T1E=kind2opt;_T1F=*_T1E;_T20=_check_null(_T1F);_T21=_T20->v;_T22=(struct Cyc_Absyn_Kind*)_T21;_T23=Cyc_Kinds_kind_leq(_T1D,_T22);if(!_T23)goto _TL8C;_T24=kind2opt;_T25=kind1opt;
*_T24=*_T25;goto _TL8D;
_TL8C: _T26=kind2opt;_T27=*_T26;_T28=_check_null(_T27);_T29=_T28->v;_T2A=(struct Cyc_Absyn_Kind*)_T29;_T2B=kind1opt;_T2C=*_T2B;_T2D=_check_null(_T2C);_T2E=_T2D->v;_T2F=(struct Cyc_Absyn_Kind*)_T2E;_T30=Cyc_Kinds_kind_leq(_T2A,_T2F);if(!_T30)goto _TL8E;_T31=kind1opt;_T32=kind2opt;
*_T31=*_T32;goto _TL8F;
# 348
_TL8E: Cyc_Unify_fail_because("kinds are incompatible");_TL8F: _TL8D: _T33=t2r;
*_T33=t1;_T34=s1opt;_T35=*_T34;_T36=
# 351
_check_null(_T35);_T37=_T36->v;{struct Cyc_List_List*s1=(struct Cyc_List_List*)_T37;_T38=s2opt;_T39=*_T38;_T3A=
_check_null(_T39);_T3B=_T3A->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)_T3B;
if(s1!=s2)goto _TL90;return;_TL90:
 _TL95: if(s1!=0)goto _TL93;else{goto _TL94;}
_TL93: _T3D=Cyc_List_mem;{int(*_T354)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T3D;_T3C=_T354;}_T3E=Cyc_Tcutil_fast_tvar_cmp;_T3F=s2;_T40=s1;_T41=_T40->hd;_T42=(struct Cyc_Absyn_Tvar*)_T41;_T43=_T3C(_T3E,_T3F,_T42);if(_T43)goto _TL96;else{goto _TL98;}
_TL98: goto _TL94;_TL96: _T44=s1;
# 354
s1=_T44->tl;goto _TL95;_TL94:
# 357
 if(s1!=0)goto _TL99;_T45=s2opt;_T46=s1opt;
*_T45=*_T46;
return;_TL99: _T47=s1opt;_T48=*_T47;_T49=
# 361
_check_null(_T48);_T4A=_T49->v;s1=(struct Cyc_List_List*)_T4A;
_TL9E: if(s2!=0)goto _TL9C;else{goto _TL9D;}
_TL9C: _T4C=Cyc_List_mem;{int(*_T354)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T4C;_T4B=_T354;}_T4D=Cyc_Tcutil_fast_tvar_cmp;_T4E=s1;_T4F=s2;_T50=_T4F->hd;_T51=(struct Cyc_Absyn_Tvar*)_T50;_T52=_T4B(_T4D,_T4E,_T51);if(_T52)goto _TL9F;else{goto _TLA1;}
_TLA1: goto _TL9D;_TL9F: _T53=s2;
# 362
s2=_T53->tl;goto _TL9E;_TL9D:
# 365
 if(s2!=0)goto _TLA2;_T54=s1opt;_T55=s2opt;
*_T54=*_T55;
return;_TLA2: _T56=s2opt;_T57=*_T56;_T58=
# 369
_check_null(_T57);_T59=_T58->v;s2=(struct Cyc_List_List*)_T59;{
struct Cyc_List_List*ans=0;
_TLA7: if(s2!=0)goto _TLA5;else{goto _TLA6;}
_TLA5: _T5B=Cyc_List_mem;{int(*_T354)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T5B;_T5A=_T354;}_T5C=Cyc_Tcutil_fast_tvar_cmp;_T5D=s1;_T5E=s2;_T5F=_T5E->hd;_T60=(struct Cyc_Absyn_Tvar*)_T5F;_T61=_T5A(_T5C,_T5D,_T60);if(!_T61)goto _TLA8;{struct Cyc_List_List*_T354=_cycalloc(sizeof(struct Cyc_List_List));_T63=s2;_T64=_T63->hd;
_T354->hd=(struct Cyc_Absyn_Tvar*)_T64;_T354->tl=ans;_T62=(struct Cyc_List_List*)_T354;}ans=_T62;goto _TLA9;_TLA8: _TLA9: _T65=s2;
# 371
s2=_T65->tl;goto _TLA7;_TLA6: _T66=s1opt;_T67=s2opt;{struct Cyc_Core_Opt*_T354=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 374
_T354->v=ans;_T68=(struct Cyc_Core_Opt*)_T354;}*_T67=_T68;*_T66=*_T67;
return;}}}}_TL8A: _T69=t2;_T6A=t1;_T6B=cvaruf;_T6C=env;
# 412
Cyc_Unify_unify_it(_T69,_T6A,_T6B,_T6C);return;_TL88: _T6D=_T31B.f1;_T6E=(int*)_T6D;_T6F=*_T6E;if(_T6F!=1)goto _TLAA;_T70=_T31B.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T354=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T70;_T353=_T354->f1;_T71=_T31B.f1;_T72=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T71;_T73=& _T72->f2;_T352=(void**)_T73;_T351=_T354->f4;}{struct Cyc_Core_Opt*kind1=_T353;void**ref1_ref=_T352;struct Cyc_Core_Opt*s1opt=_T351;_T74=t1;_T75=Cyc_Core_heap_region;_T76=
# 380
_check_null(s1opt);_T77=_T76->v;_T78=(struct Cyc_List_List*)_T77;_T79=t2;Cyc_Unify_occurs(_T74,_T75,_T78,_T79);{
struct Cyc_Absyn_Kind*kind2=Cyc_Tcutil_type_kind(t2);_T7A=kind2;_T7B=
# 384
_check_null(kind1);_T7C=_T7B->v;_T7D=(struct Cyc_Absyn_Kind*)_T7C;_T7E=Cyc_Kinds_kind_leq(_T7A,_T7D);if(!_T7E)goto _TLAC;_T7F=ref1_ref;
*_T7F=t2;
return;_TLAC: {struct Cyc_Absyn_PtrInfo _T354;_T80=t2;_T81=(int*)_T80;_T82=*_T81;if(_T82!=4)goto _TLAE;_T83=t2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T355=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T83;_T354=_T355->f1;}_T84=kind1;_T85=_T84->v;_T86=(struct Cyc_Absyn_Kind*)_T85;_T87=_T86->kind;_T88=(int)_T87;if(_T88!=2)goto _TLB0;{struct Cyc_Absyn_PtrInfo pinfo=_T354;_T89=pinfo;_T8A=_T89.ptr_atts;_T8B=_T8A.bounds;{
# 393
void*c=Cyc_Absyn_compress(_T8B);_T8C=c;_T8D=(int*)_T8C;_T8E=*_T8D;switch(_T8E){case 3: _T8F=c;_T90=
# 396
Cyc_Absyn_thinconst();_T91=Cyc_BansheeIf_equality_constraint(_T8F,_T90);Cyc_BansheeIf_add_constraint(0U,_T91);goto _LL42;case 1: _LL42: _T92=c;_T93=
# 399
Cyc_Absyn_bounds_one();_T94=cvaruf;_T95=env;_T96=Cyc_Unify_unify_c(_T92,_T93,_T94,_T95);if(_T96)goto _TLB3;else{goto _TLB5;}_TLB5: goto _LL44;_TLB3: _T97=ref1_ref;
*_T97=t2;
return;default: _LL44:{struct _tuple11 _T355;
# 403
_T355.f0=t1;_T355.f1=t2;_T98=_T355;}Cyc_Unify_ts_failure=_T98;
Cyc_Unify_fail_because("kinds are incompatible");};}}goto _TLB1;_TLB0: goto _LL3C;_TLB1: goto _TLAF;_TLAE: _LL3C:{struct _tuple11 _T355;
# 407
_T355.f0=t1;_T355.f1=t2;_T99=_T355;}Cyc_Unify_ts_failure=_T99;
Cyc_Unify_fail_because("kinds are incompatible");_TLAF:;}}}goto _TLAB;_TLAA: _T9A=_T31B.f0;_T9B=(int*)_T9A;_T9C=*_T9B;if(_T9C!=3)goto _TLB6;goto _LLA;_TLB6: _T9D=_T31B.f1;_T9E=(int*)_T9D;_T9F=*_T9E;if(_T9F!=3)goto _TLB8;_LLA: _TA0=
# 415
cvaruf(env,t2,t1);if(_TA0)goto _TLBA;else{goto _TLBC;}
_TLBC: Cyc_Unify_fail_because("constraint variables could not be unified");goto _TLBB;_TLBA: _TLBB: goto _LL0;_TLB8: _TA1=_T31B.f0;_TA2=(int*)_TA1;_TA3=*_TA2;if(_TA3!=0)goto _TLBD;_TA4=_T31B.f0;_TA5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA4;_TA6=_TA5->f1;_TA7=(int*)_TA6;_TA8=*_TA7;if(_TA8!=9)goto _TLBF;goto _LLE;_TLBF: _TA9=_T31B.f1;_TAA=(int*)_TA9;_TAB=*_TAA;if(_TAB!=0)goto _TLC1;_TAC=_T31B.f1;_TAD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TAC;_TAE=_TAD->f1;_TAF=(int*)_TAE;_TB0=*_TAF;if(_TB0!=9)goto _TLC3;goto _LLD;_TLC3: _TB1=_T31B.f0;_TB2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB1;_TB3=_TB2->f1;_TB4=(int*)_TB3;_TB5=*_TB4;if(_TB5!=10)goto _TLC5;goto _LLF;_TLC5: _TB6=_T31B.f1;_TB7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB6;_TB8=_TB7->f1;_TB9=(int*)_TB8;_TBA=*_TB9;if(_TBA!=10)goto _TLC7;goto _LL11;_TLC7: _TBB=_T31B.f0;_TBC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TBB;_TBD=_TBC->f1;_TBE=(int*)_TBD;_TBF=*_TBE;switch(_TBF){case 16: _TC0=_T31B.f1;_TC1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC0;_TC2=_TC1->f1;_TC3=(int*)_TC2;_TC4=*_TC3;switch(_TC4){case 16: _TC5=_T31B.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC5;_TC6=_T354->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T355=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_TC6;_T34E=_T355->f1;}}_TC7=_T31B.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC7;_TC8=_T354->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T355=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_TC8;_T34D=_T355->f1;}}{enum Cyc_Absyn_AliasQualVal aqv1=_T34E;enum Cyc_Absyn_AliasQualVal aqv2=_T34D;_TC9=aqv1;_TCA=(int)_TC9;_TCB=aqv2;_TCC=(int)_TCB;
# 430
if(_TCA==_TCC)goto _TLCB;{struct _tuple11 _T354;
_T354.f0=t1;_T354.f1=t2;_TCD=_T354;}Cyc_Unify_ts_failure=_TCD;
Cyc_Unify_fail_because("(different alias qualifiers)");goto _TLCC;_TLCB: _TLCC: goto _LL0;}case 17: _TCE=_T31B.f1;_TCF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TCE;_TD0=_TCF->f2;if(_TD0==0)goto _TLCD;_TD1=_T31B.f1;_TD2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD1;_TD3=_TD2->f2;_TD4=(struct Cyc_List_List*)_TD3;_TD5=_TD4->hd;_TD6=(int*)_TD5;_TD7=*_TD6;switch(_TD7){case 2: _LL18:
# 437
 Cyc_Unify_fail_because("(abstracted type variable doesn't unify with constant)");case 1: _TD8=t2;_TD9=t1;_TDA=cvaruf;_TDB=env;
# 439
Cyc_Unify_unify_it(_TD8,_TD9,_TDA,_TDB);goto _LL0;default: goto _LL27;}goto _TLCE;_TLCD: goto _LL27;_TLCE:;default: goto _LL27;};case 17: _TDC=_T31B.f0;_TDD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TDC;_TDE=_TDD->f2;if(_TDE==0)goto _TLD0;_TDF=_T31B.f0;_TE0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TDF;_TE1=_TE0->f2;_TE2=(struct Cyc_List_List*)_TE1;_TE3=_TE2->hd;_TE4=(int*)_TE3;_TE5=*_TE4;switch(_TE5){case 2: _TE6=_T31B.f1;_TE7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE6;_TE8=_TE7->f1;_TE9=(int*)_TE8;_TEA=*_TE9;switch(_TEA){case 16: goto _LL18;case 17: _TEB=_T31B.f1;_TEC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TEB;_TED=_TEC->f2;if(_TED==0)goto _TLD4;_TEE=_T31B.f1;_TEF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TEE;_TF0=_TEF->f2;_TF1=(struct Cyc_List_List*)_TF0;_TF2=_TF1->hd;_TF3=(int*)_TF2;_TF4=*_TF3;switch(_TF4){case 2: _TF5=_T31B.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF5;_TF6=_T354->f2;{struct Cyc_List_List _T355=*_TF6;_TF7=_T355.hd;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T356=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TF7;_T353=_T356->f1;}}}_TF8=_T31B.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF8;_TF9=_T354->f2;{struct Cyc_List_List _T355=*_TF9;_TFA=_T355.hd;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T356=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TFA;_T352=_T356->f1;}}}{struct Cyc_Absyn_Tvar*tv2=_T353;struct Cyc_Absyn_Tvar*tv1=_T352;_TFB=tv2;_TFC=_TFB->identity;_TFD=tv1;_TFE=_TFD->identity;
# 447
if(_TFC==_TFE)goto _TLD7;
Cyc_Unify_fail_because("(variable types are not the same)");goto _TLD8;_TLD7: _TLD8: goto _LL0;}case 1: _TFF=t2;_T100=t1;_T101=cvaruf;_T102=env;
# 455
Cyc_Unify_unify_it(_TFF,_T100,_T101,_T102);goto _LL0;default: goto _LL27;}goto _TLD5;_TLD4: goto _LL27;_TLD5:;default: goto _LL27;};case 1: _T103=_T31B.f1;_T104=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T103;_T105=_T104->f1;_T106=(int*)_T105;_T107=*_T106;switch(_T107){case 16: _T108=_T31B.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T108;_T109=_T354->f2;{struct Cyc_List_List _T355=*_T109;_T10A=_T355.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T356=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T10A;_T353=_T356->f1;_T10B=_T31B.f0;_T10C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10B;_T10D=_T10C->f2;_T10E=_T10D->hd;_T10F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T10E;_T110=& _T10F->f2;_T352=(void**)_T110;}_T351=_T355.tl;}}_T111=_T31B.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T111;_T112=_T354->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T355=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T112;_T34E=_T355->f1;}}{struct Cyc_Core_Opt*k=_T353;void**ref=_T352;struct Cyc_List_List*bnd=_T351;enum Cyc_Absyn_AliasQualVal aqv=_T34E;_T113=
# 442
_check_null(k);_T114=_T113->v;_T115=(struct Cyc_Absyn_Kind*)_T114;_T116=& Cyc_Kinds_aqk;_T117=(struct Cyc_Absyn_Kind*)_T116;_T118=Cyc_Kinds_kind_eq(_T115,_T117);if(_T118)goto _TLDA;else{goto _TLDC;}
_TLDC: Cyc_Unify_fail_because("(incompatible kind)");goto _TLDB;_TLDA: _TLDB: _T119=ref;
*_T119=t1;goto _LL0;}case 17: _T11A=_T31B.f1;_T11B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11A;_T11C=_T11B->f2;if(_T11C==0)goto _TLDD;_T11D=_T31B.f1;_T11E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11D;_T11F=_T11E->f2;_T120=(struct Cyc_List_List*)_T11F;_T121=_T120->hd;_T122=(int*)_T121;_T123=*_T122;switch(_T123){case 2: _T124=_T31B.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T124;_T125=_T354->f2;{struct Cyc_List_List _T355=*_T125;_T126=_T355.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T356=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T126;_T353=_T356->f1;_T127=_T31B.f0;_T128=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T127;_T129=_T128->f2;_T12A=_T129->hd;_T12B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T12A;_T12C=& _T12B->f2;_T352=(void**)_T12C;_T351=_T356->f4;}}}_T12D=_T31B.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12D;_T12E=_T354->f2;{struct Cyc_List_List _T355=*_T12E;_T12F=_T355.hd;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T356=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T12F;_T350=_T356->f1;}_T34F=_T355.tl;}}{struct Cyc_Core_Opt*k=_T353;void**ref=_T352;struct Cyc_Core_Opt*s2_opt=_T351;struct Cyc_Absyn_Tvar*tv=_T350;struct Cyc_List_List*bnd=_T34F;_T130=t1;_T131=Cyc_Core_heap_region;_T132=
# 459
_check_null(s2_opt);_T133=_T132->v;_T134=(struct Cyc_List_List*)_T133;_T135=t2;Cyc_Unify_occurs(_T130,_T131,_T134,_T135);_T136=
# 461
_check_null(k);_T137=_T136->v;_T138=(struct Cyc_Absyn_Kind*)_T137;_T139=Cyc_Tcutil_type_kind(t1);_T13A=Cyc_Kinds_kind_eq(_T138,_T139);if(_T13A)goto _TLE0;else{goto _TLE2;}
_TLE2: Cyc_Unify_fail_because("(incompatible kinds");goto _TLE1;_TLE0: _TLE1: _T13B=ref;
*_T13B=t1;goto _LL0;}case 1: _T13C=_T31B.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13C;_T13D=_T354->f2;{struct Cyc_List_List _T355=*_T13D;_T13E=_T355.hd;_T353=(void*)_T13E;}}_T13F=_T31B.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13F;_T140=_T354->f2;{struct Cyc_List_List _T355=*_T140;_T141=_T355.hd;_T352=(void*)_T141;}}{void*ev2=_T353;void*ev1=_T352;_T142=ev1;_T143=ev2;_T144=cvaruf;_T145=env;
# 466
Cyc_Unify_unify_it(_T142,_T143,_T144,_T145);goto _LL0;}default: goto _LL27;}goto _TLDE;_TLDD: goto _LL27;_TLDE:;default: goto _LL27;};case 0: _T146=_T31B.f0;_T147=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T146;_T148=_T147->f2;_T149=(struct Cyc_List_List*)_T148;_T14A=_T149->hd;_T14B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T14A;_T14C=_T14B->f1;_T14D=(int*)_T14C;_T14E=*_T14D;if(_T14E!=15)goto _TLE3;_T14F=_T31B.f0;_T150=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T14F;_T151=_T150->f2;_T152=(struct Cyc_List_List*)_T151;_T153=_T152->hd;_T154=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T153;_T155=_T154->f2;if(_T155==0)goto _TLE5;_T156=_T31B.f1;_T157=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T156;_T158=_T157->f1;_T159=(int*)_T158;_T15A=*_T159;if(_T15A!=17)goto _TLE7;_T15B=_T31B.f1;_T15C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T15B;_T15D=_T15C->f2;if(_T15D==0)goto _TLE9;_T15E=_T31B.f1;_T15F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T15E;_T160=_T15F->f2;_T161=(struct Cyc_List_List*)_T160;_T162=_T161->hd;_T163=(int*)_T162;_T164=*_T163;if(_T164!=0)goto _TLEB;_T165=_T31B.f1;_T166=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T165;_T167=_T166->f2;_T168=(struct Cyc_List_List*)_T167;_T169=_T168->hd;_T16A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T169;_T16B=_T16A->f1;_T16C=(int*)_T16B;_T16D=*_T16C;if(_T16D!=15)goto _TLED;_T16E=_T31B.f1;_T16F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T16E;_T170=_T16F->f2;_T171=(struct Cyc_List_List*)_T170;_T172=_T171->hd;_T173=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T172;_T174=_T173->f2;if(_T174==0)goto _TLEF;_T175=_T31B.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T175;_T176=_T354->f2;{struct Cyc_List_List _T355=*_T176;_T177=_T355.hd;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T356=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T177;_T178=_T356->f2;{struct Cyc_List_List _T357=*_T178;_T179=_T357.hd;_T353=(void*)_T179;}}}}_T17A=_T31B.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T17A;_T17B=_T354->f2;{struct Cyc_List_List _T355=*_T17B;_T17C=_T355.hd;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T356=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T17C;_T17D=_T356->f2;{struct Cyc_List_List _T357=*_T17D;_T17E=_T357.hd;_T352=(void*)_T17E;}}}}{void*tv2=_T353;void*tv1=_T352;_T17F=
# 451
Cyc_Tcutil_typecmp(tv2,tv1);if(!_T17F)goto _TLF1;
Cyc_Unify_fail_because("(aquals(`a) variables are not the same)");goto _TLF2;_TLF1: _TLF2: goto _LL0;}_TLEF: goto _LL27;_TLED: goto _LL27;_TLEB: goto _LL27;_TLE9: goto _LL27;_TLE7: goto _LL27;_TLE5: goto _LL27;_TLE3: goto _LL27;default: goto _LL27;}goto _TLD1;_TLD0: goto _LL27;_TLD1:;default: _LL27: _T180=_T31B.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T180;_T181=_T354->f1;_T353=(void*)_T181;_T352=_T354->f2;}_T182=_T31B.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T182;_T183=_T354->f1;_T351=(void*)_T183;_T350=_T354->f2;}{void*c1=_T353;struct Cyc_List_List*ts1=_T352;void*c2=_T351;struct Cyc_List_List*ts2=_T350;_T184=
# 474
Cyc_Tcutil_tycon_cmp(c1,c2);if(_T184==0)goto _TLF3;
Cyc_Unify_fail_because("different type constructors");goto _TLF4;_TLF3: _TLF4: _T185=ts1;_T186=ts2;_T187=cvaruf;_T188=env;
Cyc_Unify_unify_list(_T185,_T186,_T187,_T188);
return;}}goto _TLC2;_TLC1: _T189=_T31B.f0;_T18A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T189;_T18B=_T18A->f1;_T18C=(int*)_T18B;_T18D=*_T18C;switch(_T18D){case 10: _LLF: _LL10: goto _LL12;case 16: goto _LL37;case 17: _T18E=_T31B.f0;_T18F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18E;_T190=_T18F->f2;if(_T190==0)goto _TLF6;_T191=_T31B.f0;_T192=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T191;_T193=_T192->f2;_T194=(struct Cyc_List_List*)_T193;_T195=_T194->hd;_T196=(int*)_T195;_T197=*_T196;switch(_T197){case 2: goto _LL37;case 1: goto _LL37;case 0: _T198=_T31B.f0;_T199=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T198;_T19A=_T199->f2;_T19B=(struct Cyc_List_List*)_T19A;_T19C=_T19B->hd;_T19D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T19C;_T19E=_T19D->f1;_T19F=(int*)_T19E;_T1A0=*_T19F;if(_T1A0!=15)goto _TLF9;_T1A1=_T31B.f0;_T1A2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1A1;_T1A3=_T1A2->f2;_T1A4=(struct Cyc_List_List*)_T1A3;_T1A5=_T1A4->hd;_T1A6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1A5;_T1A7=_T1A6->f2;if(_T1A7==0)goto _TLFB;goto _LL37;_TLFB: goto _LL37;_TLF9: goto _LL37;default: goto _LL37;}goto _TLF7;_TLF6: goto _LL37;_TLF7:;default: goto _LL37;}_TLC2: goto _TLBE;_TLBD: _T1A8=_T31B.f1;_T1A9=(int*)_T1A8;_T1AA=*_T1A9;if(_T1AA!=0)goto _TLFD;_T1AB=_T31B.f1;_T1AC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1AB;_T1AD=_T1AC->f1;_T1AE=(int*)_T1AD;_T1AF=*_T1AE;switch(_T1AF){case 9: _LLD: _LLE: goto _LL10;case 10: _LL11: _LL12: _T1B0=
# 425
Cyc_Unify_unify_effect(t1,t2);if(_T1B0)goto _TL100;else{goto _TL102;}
_TL102: Cyc_Unify_fail_because("effects don't unify");goto _TL101;_TL100: _TL101:
 return;default: _T1B1=_T31B.f0;_T1B2=(int*)_T1B1;_T1B3=*_T1B2;switch(_T1B3){case 2: goto _LL37;case 4: goto _LL37;case 9: goto _LL37;case 5: goto _LL37;case 6: goto _LL37;case 7: goto _LL37;case 8: goto _LL37;default: goto _LL37;};}goto _TLFE;_TLFD: _T1B4=_T31B.f0;_T1B5=(int*)_T1B4;_T1B6=*_T1B5;switch(_T1B6){case 2: _T1B7=_T31B.f1;_T1B8=(int*)_T1B7;_T1B9=*_T1B8;if(_T1B9!=2)goto _TL105;_T1BA=_T31B.f0;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T1BA;_T353=_T354->f1;}_T1BB=_T31B.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T1BB;_T352=_T354->f1;}{struct Cyc_Absyn_Tvar*tv2=_T353;struct Cyc_Absyn_Tvar*tv1=_T352;_T1BC=tv2;_T1BD=_T1BC->identity;_T1BE=tv1;_T1BF=_T1BE->identity;
# 481
if(_T1BD==_T1BF)goto _TL107;
Cyc_Unify_fail_because("variable types are not the same");goto _TL108;_TL107: _TL108:
 return;}_TL105: goto _LL37;case 4: _T1C0=_T31B.f1;_T1C1=(int*)_T1C0;_T1C2=*_T1C1;if(_T1C2!=4)goto _TL109;_T1C3=_T31B.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T1C3;_T1C4=_T354->f1;_T353=_T1C4.elt_type;_T1C5=_T354->f1;_T34C=_T1C5.elt_tq;_T1C6=_T354->f1;_T1C7=_T1C6.ptr_atts;_T352=_T1C7.eff;_T1C8=_T354->f1;_T1C9=_T1C8.ptr_atts;_T351=_T1C9.nullable;_T1CA=_T354->f1;_T1CB=_T1CA.ptr_atts;_T350=_T1CB.bounds;_T1CC=_T354->f1;_T1CD=_T1CC.ptr_atts;_T34F=_T1CD.zero_term;_T1CE=_T354->f1;_T1CF=_T1CE.ptr_atts;_T34B=_T1CF.autoreleased;_T1D0=_T354->f1;_T1D1=_T1D0.ptr_atts;_T34A=_T1D1.aqual;}_T1D2=_T31B.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T1D2;_T1D3=_T354->f1;_T349=_T1D3.elt_type;_T1D4=_T354->f1;_T348=_T1D4.elt_tq;_T1D5=_T354->f1;_T1D6=_T1D5.ptr_atts;_T347=_T1D6.eff;_T1D7=_T354->f1;_T1D8=_T1D7.ptr_atts;_T346=_T1D8.nullable;_T1D9=_T354->f1;_T1DA=_T1D9.ptr_atts;_T345=_T1DA.bounds;_T1DB=_T354->f1;_T1DC=_T1DB.ptr_atts;_T344=_T1DC.zero_term;_T1DD=_T354->f1;_T1DE=_T1DD.ptr_atts;_T343=_T1DE.autoreleased;_T1DF=_T354->f1;_T1E0=_T1DF.ptr_atts;_T342=_T1E0.aqual;}{void*t2a=_T353;struct Cyc_Absyn_Tqual tqual2a=_T34C;void*rgn2=_T352;void*null2a=_T351;void*b2=_T350;void*zt2=_T34F;void*rel2=_T34B;void*aq2=_T34A;void*t1a=_T349;struct Cyc_Absyn_Tqual tqual1a=_T348;void*rgn1=_T347;void*null1a=_T346;void*b1=_T345;void*zt1=_T344;void*rel1=_T343;void*aq1=_T342;_T1E1=t1a;_T1E2=t2a;_T1E3=cvaruf;_T1E4=env;
# 487
Cyc_Unify_unify_it(_T1E1,_T1E2,_T1E3,_T1E4);_T1E5=rgn2;_T1E6=rgn1;_T1E7=cvaruf;_T1E8=env;
Cyc_Unify_unify_it(_T1E5,_T1E6,_T1E7,_T1E8);{
const char*orig_failure=Cyc_Unify_failure_reason;_T1E9=aq2;_T1EA=aq1;_T1EB=cvaruf;_T1EC=env;
Cyc_Unify_unify_it(_T1E9,_T1EA,_T1EB,_T1EC);_T1ED=zt1;_T1EE=zt2;_T1EF=cvaruf;_T1F0=env;_T1F1=
# 492
Cyc_Unify_unify_c(_T1ED,_T1EE,_T1EF,_T1F0);if(_T1F1)goto _TL10B;else{goto _TL10D;}
_TL10D:{struct _tuple11 _T354;_T354.f0=t1;_T354.f1=t2;_T1F2=_T354;}Cyc_Unify_ts_failure=_T1F2;
Cyc_Unify_fail_because("not both zero terminated");goto _TL10C;_TL10B: _TL10C: _T1F3=rel1;_T1F4=rel2;_T1F5=cvaruf;_T1F6=env;_T1F7=
# 496
Cyc_Unify_unify_c(_T1F3,_T1F4,_T1F5,_T1F6);if(_T1F7)goto _TL10E;else{goto _TL110;}
_TL110:{struct _tuple11 _T354;_T354.f0=t1;_T354.f1=t2;_T1F8=_T354;}Cyc_Unify_ts_failure=_T1F8;
Cyc_Unify_fail_because("not both autoreleased");goto _TL10F;_TL10E: _TL10F:
# 500
 Cyc_Unify_unify_tqual(tqual1a,t1a,tqual2a,t2a);_T1F9=b1;_T1FA=b2;_T1FB=cvaruf;_T1FC=env;_T1FD=
Cyc_Unify_unify_c(_T1F9,_T1FA,_T1FB,_T1FC);if(_T1FD)goto _TL111;else{goto _TL113;}
_TL113:{struct _tuple11 _T354;_T354.f0=t1;_T354.f1=t2;_T1FE=_T354;}Cyc_Unify_ts_failure=_T1FE;
Cyc_Unify_fail_because("different pointer bounds");goto _TL112;_TL111: _TL112: {
# 506
void*_T354=Cyc_Absyn_compress(b1);_T1FF=(int*)_T354;_T200=*_T1FF;if(_T200!=0)goto _TL114;_T201=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T354;_T202=_T201->f1;_T203=(int*)_T202;_T204=*_T203;if(_T204!=14)goto _TL116;
# 508
Cyc_Unify_failure_reason=orig_failure;
return;_TL116: goto _LL48;_TL114: _LL48:
# 511
 Cyc_Unify_failure_reason="incompatible pointer types";_T205=null1a;_T206=null2a;_T207=cvaruf;_T208=env;
Cyc_Unify_unify_it(_T205,_T206,_T207,_T208);
return;;}}}goto _TL10A;_TL109: goto _LL37;_TL10A:;case 9: _T209=_T31B.f1;_T20A=(int*)_T209;_T20B=*_T20A;if(_T20B!=9)goto _TL118;_T20C=_T31B.f0;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T20C;_T353=_T354->f1;}_T20D=_T31B.f1;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T20D;_T352=_T354->f1;}{struct Cyc_Absyn_Exp*e1=_T353;struct Cyc_Absyn_Exp*e2=_T352;_T20E=
# 517
Cyc_Evexp_same_uint_const_exp(e1,e2);if(_T20E)goto _TL11A;else{goto _TL11C;}
_TL11C: Cyc_Unify_fail_because("cannot prove expressions are the same");goto _TL11B;_TL11A: _TL11B:
 return;}_TL118: goto _LL37;case 5: _T20F=_T31B.f1;_T210=(int*)_T20F;_T211=*_T210;if(_T211!=5)goto _TL11D;_T212=_T31B.f0;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T212;_T213=_T354->f1;_T353=_T213.elt_type;_T214=_T354->f1;_T34C=_T214.tq;_T215=_T354->f1;_T352=_T215.num_elts;_T216=_T354->f1;_T351=_T216.zero_term;}_T217=_T31B.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T217;_T218=_T354->f1;_T350=_T218.elt_type;_T219=_T354->f1;_T348=_T219.tq;_T21A=_T354->f1;_T341=_T21A.num_elts;_T21B=_T354->f1;_T34F=_T21B.zero_term;}{void*t2a=_T353;struct Cyc_Absyn_Tqual tq2a=_T34C;struct Cyc_Absyn_Exp*e1=_T352;void*zt1=_T351;void*t1a=_T350;struct Cyc_Absyn_Tqual tq1a=_T348;struct Cyc_Absyn_Exp*e2=_T341;void*zt2=_T34F;_T21C=t1a;_T21D=t2a;_T21E=cvaruf;_T21F=env;
# 523
Cyc_Unify_unify_it(_T21C,_T21D,_T21E,_T21F);
Cyc_Unify_unify_tqual(tq1a,t1a,tq2a,t2a);
Cyc_Unify_failure_reason="not both zero terminated";_T220=zt1;_T221=zt2;_T222=cvaruf;_T223=env;
Cyc_Unify_unify_it(_T220,_T221,_T222,_T223);_T224=
Cyc_Unify_unify_const_exp_opt(e1,e2);if(_T224)goto _TL11F;else{goto _TL121;}
_TL121: Cyc_Unify_fail_because("different array sizes");goto _TL120;_TL11F: _TL120:
 return;}_TL11D: goto _LL37;case 6: _T225=_T31B.f1;_T226=(int*)_T225;_T227=*_T226;if(_T227!=6)goto _TL122;_T228=_T31B.f0;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T228;_T229=_T354->f1;_T340=_T229.tvars;_T22A=_T354->f1;_T353=_T22A.effect;_T22B=_T354->f1;_T34C=_T22B.ret_tqual;_T22C=_T354->f1;_T352=_T22C.ret_type;_T22D=_T354->f1;_T33F=_T22D.args;_T22E=_T354->f1;_T33E=_T22E.c_varargs;_T22F=_T354->f1;_T33D=_T22F.cyc_varargs;_T230=_T354->f1;_T33C=_T230.qual_bnd;_T231=_T354->f1;_T33B=_T231.attributes;_T232=_T354->f1;_T341=_T232.checks_clause;_T233=_T354->f1;_T33A=_T233.checks_assn;_T234=_T354->f1;_T339=_T234.requires_clause;_T235=_T354->f1;_T338=_T235.requires_assn;_T236=_T354->f1;_T337=_T236.ensures_clause;_T237=_T354->f1;_T336=_T237.ensures_assn;_T238=_T354->f1;_T335=_T238.throws_clause;_T239=_T354->f1;_T334=_T239.throws_assn;_T23A=_T354->f1;_T333=_T23A.return_value;_T23B=_T354->f1;_T332=_T23B.arg_vardecls;_T23C=_T354->f1;_T331=_T23C.effconstr;}_T23D=_T31B.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T23D;_T23E=_T354->f1;_T330=_T23E.tvars;_T23F=_T354->f1;_T351=_T23F.effect;_T240=_T354->f1;_T348=_T240.ret_tqual;_T241=_T354->f1;_T350=_T241.ret_type;_T242=_T354->f1;_T32F=_T242.args;_T243=_T354->f1;_T32E=_T243.c_varargs;_T244=_T354->f1;_T32D=_T244.cyc_varargs;_T245=_T354->f1;_T32C=_T245.qual_bnd;_T246=_T354->f1;_T32B=_T246.attributes;_T247=_T354->f1;_T32A=_T247.checks_clause;_T248=_T354->f1;_T329=_T248.checks_assn;_T249=_T354->f1;_T328=_T249.requires_clause;_T24A=_T354->f1;_T327=_T24A.requires_assn;_T24B=_T354->f1;_T326=_T24B.ensures_clause;_T24C=_T354->f1;_T325=_T24C.ensures_assn;_T24D=_T354->f1;_T324=_T24D.throws_clause;_T24E=_T354->f1;_T323=_T24E.throws_assn;_T24F=_T354->f1;_T322=_T24F.return_value;_T250=_T354->f1;_T321=_T250.arg_vardecls;_T251=_T354->f1;_T320=_T251.effconstr;}{struct Cyc_List_List*tvs2=_T340;void*eff2=_T353;struct Cyc_Absyn_Tqual rt_tq2=_T34C;void*rt2=_T352;struct Cyc_List_List*args2=_T33F;int c_vararg2=_T33E;struct Cyc_Absyn_VarargInfo*cyc_vararg2=_T33D;struct Cyc_List_List*qb2=_T33C;struct Cyc_List_List*atts2=_T33B;struct Cyc_Absyn_Exp*chk2=_T341;struct Cyc_AssnDef_ExistAssnFn*chk_relns2=_T33A;struct Cyc_Absyn_Exp*req2=_T339;struct Cyc_AssnDef_ExistAssnFn*req_relns2=_T338;struct Cyc_Absyn_Exp*ens2=_T337;struct Cyc_AssnDef_ExistAssnFn*ens_relns2=_T336;struct Cyc_Absyn_Exp*thrws2=_T335;struct Cyc_AssnDef_ExistAssnFn*thrws_relns2=_T334;struct Cyc_Absyn_Vardecl*return_value2=_T333;struct Cyc_List_List*argvds2=_T332;struct Cyc_List_List*effc2=_T331;struct Cyc_List_List*tvs1=_T330;void*eff1=_T351;struct Cyc_Absyn_Tqual rt_tq1=_T348;void*rt1=_T350;struct Cyc_List_List*args1=_T32F;int c_vararg1=_T32E;struct Cyc_Absyn_VarargInfo*cyc_vararg1=_T32D;struct Cyc_List_List*qb1=_T32C;struct Cyc_List_List*atts1=_T32B;struct Cyc_Absyn_Exp*chk1=_T32A;struct Cyc_AssnDef_ExistAssnFn*chk_relns1=_T329;struct Cyc_Absyn_Exp*req1=_T328;struct Cyc_AssnDef_ExistAssnFn*req_relns1=_T327;struct Cyc_Absyn_Exp*ens1=_T326;struct Cyc_AssnDef_ExistAssnFn*ens_relns1=_T325;struct Cyc_Absyn_Exp*thrws1=_T324;struct Cyc_AssnDef_ExistAssnFn*thrws_relns1=_T323;struct Cyc_Absyn_Vardecl*return_value1=_T322;struct Cyc_List_List*argvds1=_T321;struct Cyc_List_List*effc1=_T320;{struct _RegionHandle _T354=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _T354;_push_region(rgn);{
# 534
struct Cyc_List_List*inst=0;
_TL124: if(tvs1!=0)goto _TL125;else{goto _TL126;}
_TL125: if(tvs2!=0)goto _TL127;
Cyc_Unify_fail_because("second function type has too few type variables");goto _TL128;_TL127: _TL128: _T252=tvs1;_T253=_T252->hd;_T254=(struct Cyc_Absyn_Tvar*)_T253;{
void*kb1=_T254->kind;_T255=tvs2;_T256=_T255->hd;_T257=(struct Cyc_Absyn_Tvar*)_T256;{
void*kb2=_T257->kind;_T258=
Cyc_Unify_unify_kindbound(kb1,kb2);if(_T258)goto _TL129;else{goto _TL12B;}
_TL12B:{struct Cyc_String_pa_PrintArg_struct _T355;_T355.tag=0;_T25B=tvs1;_T25C=_T25B->hd;_T25D=(struct Cyc_Absyn_Tvar*)_T25C;
# 543
_T355.f1=Cyc_Absynpp_tvar2string(_T25D);_T25A=_T355;}{struct Cyc_String_pa_PrintArg_struct _T355=_T25A;{struct Cyc_String_pa_PrintArg_struct _T356;_T356.tag=0;_T25F=tvs1;_T260=_T25F->hd;_T261=(struct Cyc_Absyn_Tvar*)_T260;_T262=& Cyc_Kinds_bk;_T263=(struct Cyc_Absyn_Kind*)_T262;_T264=
Cyc_Kinds_tvar_kind(_T261,_T263);_T356.f1=Cyc_Kinds_kind2string(_T264);_T25E=_T356;}{struct Cyc_String_pa_PrintArg_struct _T356=_T25E;{struct Cyc_String_pa_PrintArg_struct _T357;_T357.tag=0;_T266=tvs2;_T267=_T266->hd;_T268=(struct Cyc_Absyn_Tvar*)_T267;_T269=& Cyc_Kinds_bk;_T26A=(struct Cyc_Absyn_Kind*)_T269;_T26B=
# 546
Cyc_Kinds_tvar_kind(_T268,_T26A);_T357.f1=Cyc_Kinds_kind2string(_T26B);_T265=_T357;}{struct Cyc_String_pa_PrintArg_struct _T357=_T265;void*_T358[3];_T358[0]=& _T355;_T358[1]=& _T356;_T358[2]=& _T357;_T26C=
# 542
_tag_fat("type var %s has different kinds %s and %s",sizeof(char),42U);_T26D=_tag_fat(_T358,sizeof(void*),3);_T259=Cyc_aprintf(_T26C,_T26D);}}}_T26E=_untag_fat_ptr_check_bound(_T259,sizeof(char),1U);_T26F=(const char*)_T26E;
# 541
Cyc_Unify_fail_because(_T26F);goto _TL12A;_TL129: _TL12A: _T271=rgn;{struct Cyc_List_List*_T355=_region_malloc(_T271,0U,sizeof(struct Cyc_List_List));_T273=rgn;{struct _tuple13*_T356=_region_malloc(_T273,0U,sizeof(struct _tuple13));_T274=tvs2;_T275=_T274->hd;
# 548
_T356->f0=(struct Cyc_Absyn_Tvar*)_T275;_T276=tvs1;_T277=_T276->hd;_T278=(struct Cyc_Absyn_Tvar*)_T277;_T356->f1=Cyc_Absyn_var_type(_T278);_T272=(struct _tuple13*)_T356;}_T355->hd=_T272;_T355->tl=inst;_T270=(struct Cyc_List_List*)_T355;}inst=_T270;_T279=tvs1;
tvs1=_T279->tl;_T27A=tvs2;
tvs2=_T27A->tl;}}goto _TL124;_TL126:
# 552
 if(tvs2==0)goto _TL12C;
Cyc_Unify_fail_because("second function type has too many type variables");goto _TL12D;_TL12C: _TL12D:
 if(inst==0)goto _TL12E;{
struct Cyc_Absyn_FnType_Absyn_Type_struct*newftype1;newftype1=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T27B=newftype1;_T27B->tag=6;_T27C=newftype1;_T27C->f1.tvars=0;_T27D=newftype1;_T27D->f1.effect=eff1;_T27E=newftype1;_T27E->f1.ret_tqual=rt_tq1;_T27F=newftype1;_T27F->f1.ret_type=rt1;_T280=newftype1;_T280->f1.args=args1;_T281=newftype1;_T281->f1.c_varargs=c_vararg1;_T282=newftype1;
_T282->f1.cyc_varargs=cyc_vararg1;_T283=newftype1;_T283->f1.qual_bnd=qb1;_T284=newftype1;_T284->f1.attributes=atts1;_T285=newftype1;_T285->f1.checks_clause=chk1;_T286=newftype1;_T286->f1.checks_assn=chk_relns1;_T287=newftype1;_T287->f1.requires_clause=req1;_T288=newftype1;_T288->f1.requires_assn=req_relns1;_T289=newftype1;_T289->f1.ensures_clause=ens1;_T28A=newftype1;
_T28A->f1.ensures_assn=ens_relns1;_T28B=newftype1;_T28B->f1.throws_clause=thrws1;_T28C=newftype1;_T28C->f1.throws_assn=thrws_relns1;_T28D=newftype1;_T28D->f1.return_value=return_value1;_T28E=newftype1;_T28E->f1.arg_vardecls=argvds1;_T28F=newftype1;_T28F->f1.effconstr=effc1;_T290=rgn;_T291=inst;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T355=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T355->tag=6;
# 559
_T355->f1.tvars=0;_T355->f1.effect=eff2;_T355->f1.ret_tqual=rt_tq2;_T355->f1.ret_type=rt2;_T355->f1.args=args2;
_T355->f1.c_varargs=c_vararg2;_T355->f1.cyc_varargs=cyc_vararg2;_T355->f1.qual_bnd=qb2;
_T355->f1.attributes=atts2;_T355->f1.checks_clause=chk2;_T355->f1.checks_assn=chk_relns2;_T355->f1.requires_clause=req2;_T355->f1.requires_assn=req_relns2;
_T355->f1.ensures_clause=ens2;_T355->f1.ensures_assn=ens_relns2;_T355->f1.throws_clause=thrws2;_T355->f1.throws_assn=thrws_relns2;_T355->f1.return_value=return_value2;_T355->f1.arg_vardecls=argvds2;_T355->f1.effconstr=effc2;_T292=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T355;}_T293=(void*)_T292;{
# 558
void*newftype2=Cyc_Tcutil_rsubstitute(_T290,_T291,_T293);_T294=newftype1;_T295=(void*)_T294;_T296=newftype2;_T297=cvaruf;_T298=env;
# 563
Cyc_Unify_unify_it(_T295,_T296,_T297,_T298);_npop_handler(0);return;}}_TL12E:;}_pop_region();}_T299=rt1;_T29A=rt2;_T29B=cvaruf;_T29C=env;
# 567
Cyc_Unify_unify_it(_T299,_T29A,_T29B,_T29C);
Cyc_Unify_unify_tqual(rt_tq1,rt1,rt_tq2,rt2);
_TL133: if(args1!=0)goto _TL134;else{goto _TL132;}_TL134: if(args2!=0)goto _TL131;else{goto _TL132;}
_TL131: _T29D=args1;_T29E=_T29D->hd;_T29F=(struct _tuple8*)_T29E;{struct _tuple8 _T354=*_T29F;void*_T355;struct Cyc_Absyn_Tqual _T356;_T356=_T354.f1;_T355=_T354.f2;{struct Cyc_Absyn_Tqual tqa=_T356;void*ta=_T355;_T2A0=args2;_T2A1=_T2A0->hd;_T2A2=(struct _tuple8*)_T2A1;{
struct _tuple8 _T357=*_T2A2;void*_T358;struct Cyc_Absyn_Tqual _T359;_T359=_T357.f1;_T358=_T357.f2;{struct Cyc_Absyn_Tqual tqb=_T359;void*tb=_T358;_T2A3=ta;_T2A4=tb;_T2A5=cvaruf;_T2A6=env;
Cyc_Unify_unify_it(_T2A3,_T2A4,_T2A5,_T2A6);
Cyc_Unify_unify_tqual(tqa,ta,tqb,tb);}}}}_T2A7=args1;
# 569
args1=_T2A7->tl;_T2A8=args2;args2=_T2A8->tl;goto _TL133;_TL132:{struct _tuple11 _T354;
# 575
_T354.f0=t1;_T354.f1=t2;_T2A9=_T354;}Cyc_Unify_ts_failure=_T2A9;
if(args1!=0)goto _TL137;else{goto _TL138;}_TL138: if(args2!=0)goto _TL137;else{goto _TL135;}
_TL137: Cyc_Unify_fail_because("function types have different number of arguments");goto _TL136;_TL135: _TL136:
 if(c_vararg1==c_vararg2)goto _TL139;
Cyc_Unify_fail_because("only one function type takes C varargs");goto _TL13A;_TL139: _TL13A:{struct _tuple14 _T354;
# 581
_T354.f0=cyc_vararg1;_T354.f1=cyc_vararg2;_T2AA=_T354;}{struct _tuple14 _T354=_T2AA;int _T355;void*_T356;struct Cyc_Absyn_Tqual _T357;struct _fat_ptr*_T358;int _T359;void*_T35A;struct Cyc_Absyn_Tqual _T35B;struct _fat_ptr*_T35C;_T2AB=_T354.f0;if(_T2AB!=0)goto _TL13B;_T2AC=_T354.f1;if(_T2AC!=0)goto _TL13D;goto _LL50;_TL13D: goto _LL56;_TL13B: _T2AD=_T354.f1;if(_T2AD!=0)goto _TL13F;_LL56:
# 584
 Cyc_Unify_fail_because("only one function type takes varargs");goto _TL140;_TL13F: _T2AE=_T354.f0;{struct Cyc_Absyn_VarargInfo _T35D=*_T2AE;_T35C=_T35D.name;_T35B=_T35D.tq;_T35A=_T35D.type;_T359=_T35D.inject;}_T2AF=_T354.f1;{struct Cyc_Absyn_VarargInfo _T35D=*_T2AF;_T358=_T35D.name;_T357=_T35D.tq;_T356=_T35D.type;_T355=_T35D.inject;}{struct _fat_ptr*n1=_T35C;struct Cyc_Absyn_Tqual tq1=_T35B;void*tp1=_T35A;int i1=_T359;struct _fat_ptr*n2=_T358;struct Cyc_Absyn_Tqual tq2=_T357;void*tp2=_T356;int i2=_T355;_T2B0=tp1;_T2B1=tp2;_T2B2=cvaruf;_T2B3=env;
# 586
Cyc_Unify_unify_it(_T2B0,_T2B1,_T2B2,_T2B3);
Cyc_Unify_unify_tqual(tq1,tp1,tq2,tp2);
if(i1==i2)goto _TL141;
Cyc_Unify_fail_because("only one function type injects varargs");goto _TL142;_TL141: _TL142:;}_TL140: _LL50:;}{
# 593
int bad_effect;
if(eff1!=0)goto _TL143;if(eff2!=0)goto _TL143;
bad_effect=0;goto _TL144;
_TL143: if(eff1==0)goto _TL147;else{goto _TL148;}_TL148: if(eff2==0)goto _TL147;else{goto _TL145;}
_TL147: bad_effect=1;goto _TL146;
# 599
_TL145: _T2B4=Cyc_Unify_unify_effect(eff1,eff2);bad_effect=!_T2B4;_TL146: _TL144:{struct _tuple11 _T354;
_T354.f0=t1;_T354.f1=t2;_T2B5=_T354;}Cyc_Unify_ts_failure=_T2B5;_T2B6=bad_effect;
if(!_T2B6)goto _TL149;{struct Cyc_String_pa_PrintArg_struct _T354;_T354.tag=0;_T2B9=eff1;_T2BA=(unsigned)_T2B9;
if(!_T2BA)goto _TL14B;_T354.f1=Cyc_Absynpp_typ2string(eff1);goto _TL14C;_TL14B: _T2BB=_tag_fat("-",sizeof(char),2U);_T354.f1=_T2BB;_TL14C: _T2B8=_T354;}{struct Cyc_String_pa_PrintArg_struct _T354=_T2B8;{struct Cyc_String_pa_PrintArg_struct _T355;_T355.tag=0;_T2BD=eff2;_T2BE=(unsigned)_T2BD;
if(!_T2BE)goto _TL14D;_T355.f1=Cyc_Absynpp_typ2string(eff2);goto _TL14E;_TL14D: _T2BF=_tag_fat("-",sizeof(char),2U);_T355.f1=_T2BF;_TL14E: _T2BC=_T355;}{struct Cyc_String_pa_PrintArg_struct _T355=_T2BC;void*_T356[2];_T356[0]=& _T354;_T356[1]=& _T355;_T2C0=
# 602
_tag_fat("function effects (%s,%s) do not match",sizeof(char),38U);_T2C1=_tag_fat(_T356,sizeof(void*),2);_T2B7=Cyc_aprintf(_T2C0,_T2C1);}}_T2C2=_untag_fat_ptr_check_bound(_T2B7,sizeof(char),1U);_T2C3=(const char*)_T2C2;Cyc_Unify_fail_because(_T2C3);goto _TL14A;_TL149: _TL14A: _T2C4=
# 604
Cyc_Atts_equiv_fn_atts(atts2,atts1);if(_T2C4)goto _TL14F;else{goto _TL151;}
_TL151: Cyc_Unify_fail_because("function types have different attributes");goto _TL150;_TL14F: _TL150: _T2C5=
# 607
Cyc_Tcutil_cmp_effect_constraints(effc1,effc2);if(!_T2C5)goto _TL152;
Cyc_Unify_fail_because("function types have different effect constraints");goto _TL153;_TL152: _TL153: {
# 611
struct Cyc_List_List*terms=0;{
struct Cyc_List_List*vds=argvds1;_TL157: if(vds!=0)goto _TL155;else{goto _TL156;}
_TL155:{struct Cyc_List_List*_T354=_cycalloc(sizeof(struct Cyc_List_List));_T2C8=vds;_T2C9=_T2C8->hd;_T2CA=(struct Cyc_Absyn_Vardecl*)_T2C9;if(_T2CA!=0)goto _TL158;_T2C7=0;goto _TL159;_TL158: _T2CB=vds;_T2CC=_T2CB->hd;_T2C7=(struct Cyc_Absyn_Vardecl*)_T2CC;_TL159: _T2CE=vds;_T2CF=_T2CE->hd;_T2D0=(struct Cyc_Absyn_Vardecl*)_T2CF;if(_T2D0!=0)goto _TL15A;_T2CD=0;goto _TL15B;_TL15A: _T2D1=vds;_T2D2=_T2D1->hd;_T2D3=(struct Cyc_Absyn_Vardecl*)_T2D2;_T2CD=_T2D3->type;_TL15B: _T354->hd=Cyc_AssnDef_fresh_var(_T2C7,_T2CD);_T354->tl=terms;_T2C6=(struct Cyc_List_List*)_T354;}terms=_T2C6;_T2D4=vds;
# 612
vds=_T2D4->tl;goto _TL157;_TL156:;}
# 614
terms=Cyc_List_imp_rev(terms);{struct Cyc_List_List*_T354=_cycalloc(sizeof(struct Cyc_List_List));
# 616
_T354->hd=Cyc_AssnDef_fresh_var(0,0);_T354->tl=terms;_T2D5=(struct Cyc_List_List*)_T354;}terms=_T2D5;_T2D6=
# 621
Cyc_Unify_check_logical_equivalence(terms,chk_relns1,chk_relns2);if(_T2D6)goto _TL15C;else{goto _TL15E;}
_TL15E: Cyc_Unify_fail_because("@check clauses not equivalent");goto _TL15D;_TL15C: _TL15D: _T2D7=
Cyc_Unify_check_logical_equivalence(terms,req_relns1,req_relns2);if(_T2D7)goto _TL15F;else{goto _TL161;}
_TL161: Cyc_Unify_fail_because("@requires clauses not equivalent");goto _TL160;_TL15F: _TL160:{struct Cyc_List_List*_T354=_cycalloc(sizeof(struct Cyc_List_List));
# 626
_T354->hd=Cyc_AssnDef_fresh_var(0,0);_T354->tl=terms;_T2D8=(struct Cyc_List_List*)_T354;}terms=_T2D8;{struct Cyc_List_List*_T354=_cycalloc(sizeof(struct Cyc_List_List));
_T354->hd=Cyc_AssnDef_fresh_var(0,0);_T354->tl=terms;_T2D9=(struct Cyc_List_List*)_T354;}_T2DA=ens_relns1;_T2DB=ens_relns2;_T2DC=Cyc_Unify_check_logical_equivalence(_T2D9,_T2DA,_T2DB);if(_T2DC)goto _TL162;else{goto _TL164;}
_TL164: Cyc_Unify_fail_because("@ensures clauses not equivalent");goto _TL163;_TL162: _TL163: _T2DD=
Cyc_Unify_check_logical_equivalence(terms,thrws_relns1,thrws_relns2);if(_T2DD)goto _TL165;else{goto _TL167;}
_TL167: Cyc_Unify_fail_because("@throws clauses not equivalent");goto _TL166;_TL165: _TL166:
 return;}}}_TL122: goto _LL37;case 7: _T2DE=_T31B.f1;_T2DF=(int*)_T2DE;_T2E0=*_T2DF;if(_T2E0!=7)goto _TL168;_T2E1=_T31B.f0;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T2E1;_T31F=_T354->f1;_T33E=_T354->f2;_T340=_T354->f3;}_T2E2=_T31B.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T2E2;_T31E=_T354->f1;_T32E=_T354->f2;_T33F=_T354->f3;}{enum Cyc_Absyn_AggrKind k2=_T31F;int tup2=_T33E;struct Cyc_List_List*fs2=_T340;enum Cyc_Absyn_AggrKind k1=_T31E;int tup1=_T32E;struct Cyc_List_List*fs1=_T33F;_T2E3=k1;_T2E4=(int)_T2E3;_T2E5=k2;_T2E6=(int)_T2E5;
# 634
if(_T2E4==_T2E6)goto _TL16A;
Cyc_Unify_fail_because("struct and union type");goto _TL16B;_TL16A: _TL16B:
 _TL16F: if(fs1!=0)goto _TL170;else{goto _TL16E;}_TL170: if(fs2!=0)goto _TL16D;else{goto _TL16E;}
_TL16D: _T2E7=fs1;_T2E8=_T2E7->hd;{struct Cyc_Absyn_Aggrfield*f1=(struct Cyc_Absyn_Aggrfield*)_T2E8;_T2E9=fs2;_T2EA=_T2E9->hd;{
struct Cyc_Absyn_Aggrfield*f2=(struct Cyc_Absyn_Aggrfield*)_T2EA;_T2EB=f1;_T2EC=_T2EB->name;_T2ED=f2;_T2EE=_T2ED->name;_T2EF=
Cyc_strptrcmp(_T2EC,_T2EE);if(_T2EF==0)goto _TL171;
Cyc_Unify_fail_because("different member names");goto _TL172;_TL171: _TL172: _T2F0=f1;_T2F1=_T2F0->type;_T2F2=f2;_T2F3=_T2F2->type;_T2F4=cvaruf;_T2F5=env;
Cyc_Unify_unify_it(_T2F1,_T2F3,_T2F4,_T2F5);_T2F6=f1;_T2F7=_T2F6->tq;_T2F8=f1;_T2F9=_T2F8->type;_T2FA=f2;_T2FB=_T2FA->tq;_T2FC=f2;_T2FD=_T2FC->type;
Cyc_Unify_unify_tqual(_T2F7,_T2F9,_T2FB,_T2FD);{struct _tuple11 _T354;
_T354.f0=t1;_T354.f1=t2;_T2FE=_T354;}Cyc_Unify_ts_failure=_T2FE;_T2FF=f1;_T300=_T2FF->attributes;_T301=f2;_T302=_T301->attributes;_T303=
Cyc_Atts_same_atts(_T300,_T302);if(_T303)goto _TL173;else{goto _TL175;}
_TL175: Cyc_Unify_fail_because("different attributes on member");goto _TL174;_TL173: _TL174: _T304=f1;_T305=_T304->width;_T306=f2;_T307=_T306->width;_T308=
Cyc_Unify_unify_const_exp_opt(_T305,_T307);if(_T308)goto _TL176;else{goto _TL178;}
_TL178: Cyc_Unify_fail_because("different bitfield widths on member");goto _TL177;_TL176: _TL177: _T309=f1;_T30A=_T309->requires_clause;_T30B=f2;_T30C=_T30B->requires_clause;_T30D=
Cyc_Unify_unify_const_exp_opt(_T30A,_T30C);if(_T30D)goto _TL179;else{goto _TL17B;}
_TL17B: Cyc_Unify_fail_because("different @requires clauses on member");goto _TL17A;_TL179: _TL17A:;}}_T30E=fs1;
# 636
fs1=_T30E->tl;_T30F=fs2;fs2=_T30F->tl;goto _TL16F;_TL16E:
# 651
 if(fs1!=0)goto _TL17E;else{goto _TL17F;}_TL17F: if(fs2!=0)goto _TL17E;else{goto _TL17C;}
_TL17E: Cyc_Unify_fail_because("different number of members");goto _TL17D;_TL17C: _TL17D:
 return;}_TL168: goto _LL37;case 8: _T310=_T31B.f1;_T311=(int*)_T310;_T312=*_T311;if(_T312!=8)goto _TL180;_T313=_T31B.f0;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T313;_T340=_T354->f2;_T31D=_T354->f3;}_T314=_T31B.f1;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T354=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T314;_T33F=_T354->f2;_T31C=_T354->f3;}{struct Cyc_List_List*ts1=_T340;struct Cyc_Absyn_Typedefdecl*td1=_T31D;struct Cyc_List_List*ts2=_T33F;struct Cyc_Absyn_Typedefdecl*td2=_T31C;
# 656
if(td1==td2)goto _TL182;
Cyc_Unify_fail_because("different abstract typedefs");goto _TL183;_TL182: _TL183:
 Cyc_Unify_failure_reason="type parameters to typedef differ";_T315=ts1;_T316=ts2;_T317=cvaruf;_T318=env;
Cyc_Unify_unify_list(_T315,_T316,_T317,_T318);
return;}_TL180: goto _LL37;default: _LL37: _T319=& Cyc_Unify_Unify_val;_T31A=(struct Cyc_Unify_Unify_exn_struct*)_T319;_throw(_T31A);}_TLFE: _TLBE: _TLAB: _LL0:;}}

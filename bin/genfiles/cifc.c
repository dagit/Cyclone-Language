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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 349
extern struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*,void*);
# 354
extern struct Cyc_List_List*Cyc_List_delete_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 921 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 933
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 966
extern void*Cyc_Absyn_heap_rgn_type;
# 974
extern void*Cyc_Absyn_void_type;
# 1025
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1163
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1215
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1221
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);
# 1223
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*);
# 77
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 37 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
# 39 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 92
void*Cyc_Tcutil_copy_type(void*);
# 82 "attributes.h"
struct Cyc_List_List*Cyc_Atts_merge_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 94 "kinds.h"
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 42 "cifc.cyc"
static int Cyc_Cifc_glob_tvar_id=0;
int Cyc_Cifc_opt_inst_tvar=0;
# 45
static struct _fat_ptr Cyc_Cifc_get_type_kind(void*t){void*_T0;int*_T1;unsigned _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 3: _T3=
# 47
_tag_fat("Cvar",sizeof(char),5U);return _T3;case 0: _T4=
_tag_fat("AppType",sizeof(char),8U);return _T4;case 1: _T5=
_tag_fat("Evar",sizeof(char),5U);return _T5;case 2: _T6=
_tag_fat("Vartype",sizeof(char),8U);return _T6;case 4: _T7=
_tag_fat("Pointertype",sizeof(char),12U);return _T7;case 5: _T8=
_tag_fat("ArrayType",sizeof(char),10U);return _T8;case 6: _T9=
_tag_fat("FnType",sizeof(char),7U);return _T9;case 7: _TA=
_tag_fat("AnonAggrType",sizeof(char),13U);return _TA;case 8: _TB=
_tag_fat("Typedeftype",sizeof(char),12U);return _TB;case 9: _TC=
_tag_fat("Valueoftype",sizeof(char),12U);return _TC;case 10: _TD=
_tag_fat("Typedecltype",sizeof(char),13U);return _TD;default: _TE=
_tag_fat("Typeoftype",sizeof(char),11U);return _TE;};}
# 62
static int Cyc_Cifc_is_concrete(struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Aggrdecl*_T3;struct Cyc_Absyn_AggrdeclImpl*_T4;int _T5;struct Cyc_Absyn_Enumdecl*_T6;struct Cyc_Core_Opt*_T7;int _T8;_T0=d;{
void*_T9=_T0->r;struct Cyc_Absyn_Enumdecl*_TA;struct Cyc_Absyn_Aggrdecl*_TB;_T1=(int*)_T9;_T2=*_T1;switch(_T2){case 1: goto _LL4;case 8: _LL4:
# 65
 return 1;case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TC=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T9;_TB=_TC->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_TB;_T3=ad;_T4=_T3->impl;_T5=_T4!=0;
return _T5;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TC=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T9;_TA=_TC->f1;}{struct Cyc_Absyn_Enumdecl*ed=_TA;_T6=ed;_T7=_T6->fields;_T8=_T7!=0;
return _T8;}default:
 return 0;};}}
# 73
static struct Cyc_Absyn_Decl*Cyc_Cifc_lookup_concrete_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;struct _fat_ptr*_T3;unsigned _T4;int _T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_Absyn_Decl*_T8;int _T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Absyn_Decl*_TC;struct Cyc_List_List*_TD;struct Cyc_Absyn_Decl*_TE;
# 75
struct Cyc_List_List*indecls=decls;
_TL5: if(decls!=0)goto _TL3;else{goto _TL4;}
_TL3: _T0=decls;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{struct _fat_ptr*dname=Cyc_Absyn_decl_name(_T2);_T3=dname;_T4=(unsigned)_T3;
if(!_T4)goto _TL6;_T5=Cyc_strptrcmp(dname,name);if(_T5)goto _TL6;else{goto _TL8;}_TL8: _T6=decls;_T7=_T6->hd;_T8=(struct Cyc_Absyn_Decl*)_T7;_T9=Cyc_Cifc_is_concrete(_T8);if(!_T9)goto _TL6;_TA=decls;_TB=_TA->hd;_TC=(struct Cyc_Absyn_Decl*)_TB;
return _TC;_TL6:;}_TD=decls;
# 76
decls=_TD->tl;goto _TL5;_TL4: _TE=
# 81
Cyc_Absyn_lookup_decl(indecls,name);return _TE;}
# 85
void Cyc_Cifc_set_inst_tvar (void){
Cyc_Cifc_opt_inst_tvar=1;}
# 89
struct _fat_ptr Cyc_Cifc_list2string(struct Cyc_List_List*l,struct _fat_ptr(*tostr)(void*)){struct _fat_ptr _T0;struct _fat_ptr _T1;struct Cyc_List_List*_T2;unsigned _T3;struct _fat_ptr _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct Cyc_String_pa_PrintArg_struct _T6;struct _fat_ptr(*_T7)(void*);struct _fat_ptr(*_T8)(void*);struct Cyc_List_List*_T9;void*_TA;void**_TB;void**_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_List_List*_TF;struct _fat_ptr _T10;struct Cyc_String_pa_PrintArg_struct _T11;void**_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;_T0=
_tag_fat("(",sizeof(char),2U);_T1=_tag_fat(0U,sizeof(void*),0);{struct _fat_ptr ret=Cyc_aprintf(_T0,_T1);
_TLC: _T2=l;_T3=(unsigned)_T2;if(_T3)goto _TLA;else{goto _TLB;}
_TLA:{struct Cyc_String_pa_PrintArg_struct _T15;_T15.tag=0;_T15.f1=ret;_T5=_T15;}{struct Cyc_String_pa_PrintArg_struct _T15=_T5;{struct Cyc_String_pa_PrintArg_struct _T16;_T16.tag=0;_T7=tostr;_T8=_check_null(_T7);_T9=l;_TA=_T9->hd;_T16.f1=_T8(_TA);_T6=_T16;}{struct Cyc_String_pa_PrintArg_struct _T16=_T6;void*_T17[2];_TB=_T17 + 0;*_TB=& _T15;_TC=_T17 + 1;*_TC=& _T16;_TD=_tag_fat("%s %s,",sizeof(char),7U);_TE=_tag_fat(_T17,sizeof(void*),2);_T4=Cyc_aprintf(_TD,_TE);}}ret=_T4;_TF=l;
# 91
l=_TF->tl;goto _TLC;_TLB:{struct Cyc_String_pa_PrintArg_struct _T15;_T15.tag=0;
# 93
_T15.f1=ret;_T11=_T15;}{struct Cyc_String_pa_PrintArg_struct _T15=_T11;void*_T16[1];_T12=_T16 + 0;*_T12=& _T15;_T13=_tag_fat("%s)",sizeof(char),4U);_T14=_tag_fat(_T16,sizeof(void*),1);_T10=Cyc_aprintf(_T13,_T14);}return _T10;}}
# 98
static void Cyc_Cifc_fail_merge(int warn,unsigned loc,int is_buildlib,struct _tuple0*n,struct _fat_ptr s){int _T0;struct _fat_ptr _T1;int _T2;struct Cyc_String_pa_PrintArg_struct _T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc_String_pa_PrintArg_struct _T5;void**_T6;void**_T7;void**_T8;struct Cyc___cycFILE*_T9;struct _fat_ptr _TA;struct _fat_ptr _TB;int _TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;struct Cyc_Warn_Qvar_Warn_Warg_struct _TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;void**_T11;void**_T12;void**_T13;void**_T14;unsigned _T15;struct _fat_ptr _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_Warn_Qvar_Warn_Warg_struct _T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;struct Cyc_Warn_String_Warn_Warg_struct _T1A;void**_T1B;void**_T1C;void**_T1D;void**_T1E;unsigned _T1F;struct _fat_ptr _T20;_T0=is_buildlib;
# 100
if(!_T0)goto _TLD;_T2=warn;
if(!_T2)goto _TLF;_T1=_tag_fat("Warning: user-defined",sizeof(char),22U);goto _TL10;_TLF: _T1=_tag_fat("User-defined",sizeof(char),13U);_TL10:{struct _fat_ptr preamble=_T1;{struct Cyc_String_pa_PrintArg_struct _T21;_T21.tag=0;
# 104
_T21.f1=preamble;_T3=_T21;}{struct Cyc_String_pa_PrintArg_struct _T21=_T3;{struct Cyc_String_pa_PrintArg_struct _T22;_T22.tag=0;_T22.f1=Cyc_Absynpp_qvar2string(n);_T4=_T22;}{struct Cyc_String_pa_PrintArg_struct _T22=_T4;{struct Cyc_String_pa_PrintArg_struct _T23;_T23.tag=0;_T23.f1=s;_T5=_T23;}{struct Cyc_String_pa_PrintArg_struct _T23=_T5;void*_T24[3];_T6=_T24 + 0;*_T6=& _T21;_T7=_T24 + 1;*_T7=& _T22;_T8=_T24 + 2;*_T8=& _T23;_T9=Cyc_stderr;_TA=
# 103
_tag_fat("%s type for %s incompatible with system version %s\n",sizeof(char),52U);_TB=_tag_fat(_T24,sizeof(void*),3);Cyc_fprintf(_T9,_TA,_TB);}}}}goto _TLE;
# 106
_TLD: _TC=warn;if(!_TC)goto _TL11;{struct Cyc_Warn_String_Warn_Warg_struct _T21;_T21.tag=0;
_T21.f1=_tag_fat("User-defined type for ",sizeof(char),23U);_TD=_T21;}{struct Cyc_Warn_String_Warn_Warg_struct _T21=_TD;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T22;_T22.tag=1;_T22.f1=n;_TE=_T22;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T22=_TE;{struct Cyc_Warn_String_Warn_Warg_struct _T23;_T23.tag=0;_T23.f1=_tag_fat(" incompatible with system version ",sizeof(char),35U);_TF=_T23;}{struct Cyc_Warn_String_Warn_Warg_struct _T23=_TF;{struct Cyc_Warn_String_Warn_Warg_struct _T24;_T24.tag=0;_T24.f1=s;_T10=_T24;}{struct Cyc_Warn_String_Warn_Warg_struct _T24=_T10;void*_T25[4];_T11=_T25 + 0;*_T11=& _T21;_T12=_T25 + 1;*_T12=& _T22;_T13=_T25 + 2;*_T13=& _T23;_T14=_T25 + 3;*_T14=& _T24;_T15=loc;_T16=_tag_fat(_T25,sizeof(void*),4);Cyc_Warn_warn2(_T15,_T16);}}}}goto _TL12;
# 109
_TL11:{struct Cyc_Warn_String_Warn_Warg_struct _T21;_T21.tag=0;_T21.f1=_tag_fat("User-defined type for ",sizeof(char),23U);_T17=_T21;}{struct Cyc_Warn_String_Warn_Warg_struct _T21=_T17;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T22;_T22.tag=1;_T22.f1=n;_T18=_T22;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T22=_T18;{struct Cyc_Warn_String_Warn_Warg_struct _T23;_T23.tag=0;_T23.f1=_tag_fat(" incompatible with system version ",sizeof(char),35U);_T19=_T23;}{struct Cyc_Warn_String_Warn_Warg_struct _T23=_T19;{struct Cyc_Warn_String_Warn_Warg_struct _T24;_T24.tag=0;_T24.f1=s;_T1A=_T24;}{struct Cyc_Warn_String_Warn_Warg_struct _T24=_T1A;void*_T25[4];_T1B=_T25 + 0;*_T1B=& _T21;_T1C=_T25 + 1;*_T1C=& _T22;_T1D=_T25 + 2;*_T1D=& _T23;_T1E=_T25 + 3;*_T1E=& _T24;_T1F=loc;_T20=_tag_fat(_T25,sizeof(void*),4);Cyc_Warn_err2(_T1F,_T20);}}}}_TL12: _TLE:;}
# 114
static void*Cyc_Cifc_expand_c_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp){void*_T0;int*_T1;unsigned _T2;void*_T3;int*_T4;int _T5;void*_T6;void*_T7;void*_T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;void**_TA;unsigned _TB;struct _fat_ptr _TC;void*_TD;void*_TE;void*_TF;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T10;struct Cyc_Absyn_FnInfo*_T11;struct Cyc_Absyn_FnInfo*_T12;unsigned _T13;struct Cyc_Tcenv_Tenv*_T14;struct Cyc_Absyn_FnInfo*_T15;void*_T16;void*_T17;struct Cyc_Absyn_FnInfo*_T18;struct Cyc_List_List*_T19;unsigned _T1A;struct Cyc_List_List*_T1B;void*_T1C;void**_T1D;void**_T1E;unsigned _T1F;struct Cyc_Tcenv_Tenv*_T20;void**_T21;void*_T22;void*_T23;struct Cyc_List_List*_T24;struct Cyc_Absyn_FnInfo*_T25;struct Cyc_Absyn_FnInfo*_T26;void*_T27;void*_T28;void*_T29;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2A;struct Cyc_Absyn_PtrInfo*_T2B;struct Cyc_Absyn_PtrInfo*_T2C;unsigned _T2D;struct Cyc_Tcenv_Tenv*_T2E;struct Cyc_Absyn_PtrInfo*_T2F;void*_T30;void*_T31;void*_T32;void*_T33;void*_T34;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T35;struct Cyc_Absyn_ArrayInfo*_T36;struct Cyc_Absyn_ArrayInfo*_T37;unsigned _T38;struct Cyc_Tcenv_Tenv*_T39;struct Cyc_Absyn_ArrayInfo*_T3A;void*_T3B;void*_T3C;void*_T3D;void*_T3E;int _T3F;struct Cyc_List_List*_T40;void*_T41;struct Cyc_Absyn_Aggrfield*_T42;void**_T43;unsigned _T44;struct Cyc_Tcenv_Tenv*_T45;void**_T46;void*_T47;void*_T48;struct Cyc_List_List*_T49;struct Cyc_List_List*_T4A;void*_T4B;void*_T4C;void*_T4D;void*_T4E;void*_T4F;void*_T50;void*_T51;void*_T52;void*_T53;void*_T54;void*_T55;struct Cyc_Absyn_Exp*_T56;void**_T57;struct Cyc_Absyn_TypeDecl*_T58;struct Cyc_List_List*_T59;int _T5A;enum Cyc_Absyn_AggrKind _T5B;void*_T5C;_T0=ctyp;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 8:
# 119
 Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);{void*_T5D;_T3=ctyp;_T4=(int*)_T3;_T5=*_T4;if(_T5!=8)goto _TL14;_T6=ctyp;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T5E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T6;_T7=_T5E->f4;_T5D=(void*)_T7;}{void*to=_T5D;_T8=
# 123
_check_null(to);return _T8;}_TL14:{struct Cyc_Warn_String_Warn_Warg_struct _T5E;_T5E.tag=0;
# 125
_T5E.f1=_tag_fat("Impos",sizeof(char),6U);_T9=_T5E;}{struct Cyc_Warn_String_Warn_Warg_struct _T5E=_T9;void*_T5F[1];_TA=_T5F + 0;*_TA=& _T5E;_TB=loc;_TC=_tag_fat(_T5F,sizeof(void*),1);Cyc_Warn_err2(_TB,_TC);}goto _LL19;_LL19:;}_TD=ctyp;
# 128
return _TD;case 6: _TE=ctyp;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T5D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TE;_TF=ctyp;_T10=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TF;_T11=& _T10->f1;_T5C=(struct Cyc_Absyn_FnInfo*)_T11;}{struct Cyc_Absyn_FnInfo*finfo=_T5C;
# 130
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);_T12=finfo;_T13=loc;_T14=te;_T15=finfo;_T16=_T15->ret_type;_T17=
# 132
Cyc_Toc_typ_to_c(_T16);_T12->ret_type=Cyc_Cifc_expand_c_type(_T13,_T14,_T17);_T18=finfo;{
struct Cyc_List_List*args=_T18->args;
_TL16: _T19=args;_T1A=(unsigned)_T19;if(_T1A)goto _TL17;else{goto _TL18;}
_TL17: _T1B=args;_T1C=_T1B->hd;{struct _tuple8*_T5D=(struct _tuple8*)_T1C;void*_T5E;{struct _tuple8 _T5F=*_T5D;_T1D=& _T5D->f2;_T5E=(void**)_T1D;}{void**argType=(void**)_T5E;_T1E=argType;_T1F=loc;_T20=te;_T21=argType;_T22=*_T21;_T23=
Cyc_Toc_typ_to_c(_T22);*_T1E=Cyc_Cifc_expand_c_type(_T1F,_T20,_T23);_T24=args;
args=_T24->tl;}}goto _TL16;_TL18: _T25=finfo;
# 140
_T25->tvars=0;_T26=finfo;
_T26->effect=0;_T27=ctyp;
return _T27;}}case 4: _T28=ctyp;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T5D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T28;_T29=ctyp;_T2A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T29;_T2B=& _T2A->f1;_T5C=(struct Cyc_Absyn_PtrInfo*)_T2B;}{struct Cyc_Absyn_PtrInfo*pinfo=_T5C;_T2C=pinfo;_T2D=loc;_T2E=te;_T2F=pinfo;_T30=_T2F->elt_type;_T31=
# 146
Cyc_Toc_typ_to_c(_T30);_T2C->elt_type=Cyc_Cifc_expand_c_type(_T2D,_T2E,_T31);_T32=ctyp;
return _T32;}case 5: _T33=ctyp;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T5D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T33;_T34=ctyp;_T35=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T34;_T36=& _T35->f1;_T5C=(struct Cyc_Absyn_ArrayInfo*)_T36;}{struct Cyc_Absyn_ArrayInfo*ainfo=(struct Cyc_Absyn_ArrayInfo*)_T5C;
# 149
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);_T37=ainfo;_T38=loc;_T39=te;_T3A=ainfo;_T3B=_T3A->elt_type;_T3C=
# 151
Cyc_Toc_typ_to_c(_T3B);_T37->elt_type=Cyc_Cifc_expand_c_type(_T38,_T39,_T3C);_T3D=ctyp;
return _T3D;}case 7: _T3E=ctyp;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T5D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T3E;_T5B=_T5D->f1;_T5A=_T5D->f2;_T59=_T5D->f3;}{enum Cyc_Absyn_AggrKind knd=_T5B;int is_tuple=_T5A;struct Cyc_List_List*flst=_T59;_T3F=is_tuple;
# 155
if(!_T3F)goto _TL19;
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
_TL1E: if(flst!=0)goto _TL1C;else{goto _TL1D;}
_TL1C: _T40=flst;_T41=_T40->hd;_T42=(struct Cyc_Absyn_Aggrfield*)_T41;{void**elt_type=& _T42->type;_T43=elt_type;_T44=loc;_T45=te;_T46=elt_type;_T47=*_T46;_T48=
Cyc_Toc_typ_to_c(_T47);*_T43=Cyc_Cifc_expand_c_type(_T44,_T45,_T48);_T49=flst;
flst=_T49->tl;}_T4A=
# 157
_check_null(flst);flst=_T4A->tl;goto _TL1E;_TL1D: goto _TL1A;_TL19: _TL1A: _T4B=ctyp;
# 163
return _T4B;}case 10: _T4C=ctyp;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T5D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T4C;_T58=_T5D->f1;_T57=_T5D->f2;}{struct Cyc_Absyn_TypeDecl*td=_T58;void**tptr=_T57;_T4D=ctyp;
# 166
return _T4D;}case 9: _T4E=ctyp;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T5D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T4E;_T56=_T5D->f1;}{struct Cyc_Absyn_Exp*e=_T56;_T4F=ctyp;
# 169
return _T4F;}case 11: _T50=ctyp;{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T5D=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T50;_T56=_T5D->f1;}{struct Cyc_Absyn_Exp*e=_T56;_T51=ctyp;
# 172
return _T51;}case 0: _T52=ctyp;
# 175
return _T52;case 1: _T53=ctyp;
# 178
return _T53;case 2: _T54=ctyp;
# 181
return _T54;default: _T55=ctyp;
# 183
return _T55;};}
# 187
static int Cyc_Cifc_is_boxed_kind(void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;int _T9;enum Cyc_Absyn_Size_of _TA;int _TB;enum Cyc_Absyn_Size_of _TC;int _TD;void*_TE;void*_TF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T10;struct Cyc_Absyn_PtrInfo*_T11;struct Cyc_Absyn_PtrInfo*_T12;struct Cyc_Absyn_PtrAtts _T13;int*_T14;int _T15;void*_T16;void*_T17;int*_T18;int _T19;int(*_T1A)(struct _fat_ptr,struct _fat_ptr);void*(*_T1B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T1C;struct _fat_ptr _T1D;void*_T1E;struct Cyc_Absyn_Exp*_T1F;struct Cyc_List_List*_T20;void*_T21;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T4=_T22->f1;_T21=(void*)_T4;_T20=_T22->f2;}{void*tc=_T21;struct Cyc_List_List*ts=_T20;{enum Cyc_Absyn_Size_of _T22;_T5=tc;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 1: _T8=tc;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T23=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T8;_T22=_T23->f2;}{enum Cyc_Absyn_Size_of sz=_T22;_TA=sz;_TB=(int)_TA;
# 193
if(_TB!=2)goto _TL21;_T9=1;goto _TL22;_TL21: _TC=sz;_TD=(int)_TC;_T9=_TD==3;_TL22: return _T9;}case 4: goto _LLF;case 5: _LLF:
# 196
 return 1;default:
# 198
 return 0;};}goto _LL0;}case 4: _TE=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T22=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TE;_TF=t;_T10=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TF;_T11=& _T10->f1;_T21=(struct Cyc_Absyn_PtrInfo*)_T11;}{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_T21;_T12=pi;_T13=_T12->ptr_atts;{
# 202
void*_T22=_T13.bounds;void*_T23;_T14=(int*)_T22;_T15=*_T14;if(_T15!=0)goto _TL23;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T24=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T22;_T16=_T24->f1;_T23=(void*)_T16;}{void*tc=_T23;_T17=tc;_T18=(int*)_T17;_T19=*_T18;if(_T19!=14)goto _TL25;
# 206
return 0;_TL25:
# 208
 return 1;;}goto _TL24;_TL23: _T1B=Cyc_Warn_impos;{
# 211
int(*_T24)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T1B;_T1A=_T24;}_T1C=_tag_fat("ptrbound_t must have an AppType",sizeof(char),32U);_T1D=_tag_fat(0U,sizeof(void*),0);_T1A(_T1C,_T1D);_TL24:;}goto _LL0;}case 9: _T1E=t;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T22=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T1E;_T1F=_T22->f1;}{struct Cyc_Absyn_Exp*e=_T1F;
# 215
return 1;}default:
# 217
 return 0;}_LL0:;}
# 222
static int Cyc_Cifc_check_fntype_kinds(unsigned,struct Cyc_Tcenv_Tenv*,void*,void*);
# 228
static int Cyc_Cifc_c_types_ok(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp,void*cyctyp){int _T0;int _T1;int _T2;int _T3;int _T4;int _T5;
struct Cyc_Absyn_Tqual tq;
# 231
void*ctyp_cp=Cyc_Tcutil_copy_type(ctyp);
void*utyp_cp=Cyc_Tcutil_copy_type(cyctyp);
# 234
void*c_typ=Cyc_Toc_typ_to_c(ctyp_cp);
void*u_typ=Cyc_Toc_typ_to_c(utyp_cp);_T0=
# 239
Cyc_Unify_unify(c_typ,u_typ);if(_T0)goto _TL27;else{goto _TL29;}
_TL29: c_typ=Cyc_Cifc_expand_c_type(loc,te,c_typ);
u_typ=Cyc_Cifc_expand_c_type(loc,te,u_typ);_T1=
Cyc_Unify_unify(c_typ,u_typ);if(_T1)goto _TL2A;else{goto _TL2C;}
# 245
_TL2C: _T2=Cyc_Tcutil_is_function_type(c_typ);if(!_T2)goto _TL2D;_T3=
Cyc_Cifc_check_fntype_kinds(loc,te,c_typ,u_typ);return _T3;_TL2D: _T5=
Cyc_Cifc_is_boxed_kind(c_typ);if(!_T5)goto _TL2F;_T4=Cyc_Cifc_is_boxed_kind(u_typ);goto _TL30;_TL2F: _T4=0;_TL30: return _T4;_TL2A: goto _TL28;_TL27: _TL28:
# 250
 return 1;}struct _tuple11{void*f0;void*f1;};
# 262 "cifc.cyc"
static int Cyc_Cifc_check_fntype_kinds(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*c_typ,void*u_typ){struct _tuple11 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;unsigned _T9;struct Cyc_Tcenv_Tenv*_TA;struct Cyc_Absyn_FnInfo _TB;void*_TC;struct Cyc_Absyn_FnInfo _TD;void*_TE;int _TF;struct Cyc_Absyn_FnInfo _T10;struct Cyc_Absyn_FnInfo _T11;struct Cyc_List_List*_T12;unsigned _T13;int _T14;struct Cyc_List_List*_T15;unsigned _T16;unsigned _T17;struct Cyc_Tcenv_Tenv*_T18;struct Cyc_List_List*_T19;void*_T1A;struct _tuple8*_T1B;struct _tuple8 _T1C;void*_T1D;struct Cyc_List_List*_T1E;void*_T1F;struct _tuple8*_T20;struct _tuple8 _T21;void*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;int _T25;{struct _tuple11 _T26;
_T26.f0=c_typ;_T26.f1=u_typ;_T0=_T26;}{struct _tuple11 _T26=_T0;struct Cyc_Absyn_FnInfo _T27;struct Cyc_Absyn_FnInfo _T28;_T1=_T26.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=6)goto _TL31;_T4=_T26.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=6)goto _TL33;_T7=_T26.f0;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T29=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T7;_T28=_T29->f1;}_T8=_T26.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T29=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T8;_T27=_T29->f1;}{struct Cyc_Absyn_FnInfo cfi=_T28;struct Cyc_Absyn_FnInfo ufi=_T27;_T9=loc;_TA=te;_TB=cfi;_TC=_TB.ret_type;_TD=ufi;_TE=_TD.ret_type;{
# 265
int typesOk=Cyc_Cifc_c_types_ok(_T9,_TA,_TC,_TE);_TF=typesOk;
if(!_TF)goto _TL35;_T10=cfi;{
struct Cyc_List_List*ca=_T10.args;_T11=ufi;{
struct Cyc_List_List*ua=_T11.args;
_TL37: _T12=ca;_T13=(unsigned)_T12;if(_T13)goto _TL3A;else{goto _TL39;}_TL3A: _T14=typesOk;if(_T14)goto _TL38;else{goto _TL39;}
_TL38: _T15=ua;_T16=(unsigned)_T15;if(_T16)goto _TL3B;else{goto _TL3D;}
_TL3D: return 0;_TL3B: _T17=loc;_T18=te;_T19=ca;_T1A=_T19->hd;_T1B=(struct _tuple8*)_T1A;_T1C=*_T1B;_T1D=_T1C.f2;_T1E=ua;_T1F=_T1E->hd;_T20=(struct _tuple8*)_T1F;_T21=*_T20;_T22=_T21.f2;
typesOk=Cyc_Cifc_c_types_ok(_T17,_T18,_T1D,_T22);_T23=ca;
# 274
ca=_T23->tl;_T24=ua;
ua=_T24->tl;goto _TL37;_TL39:;}}goto _TL36;_TL35: _TL36: _T25=typesOk;
# 278
return _T25;goto _LL0;}}_TL33: goto _LL3;_TL31: _LL3:
# 281
 return 0;_LL0:;}}struct _tuple12{struct Cyc_Absyn_AggrdeclImpl*f0;struct Cyc_Absyn_AggrdeclImpl*f1;};
# 288
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,struct Cyc_Tcenv_Tenv*te,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){struct _tuple11 _T0;struct Cyc_Absyn_Decl*_T1;struct Cyc_Absyn_Decl*_T2;void*_T3;int*_T4;unsigned _T5;void*_T6;int*_T7;int _T8;void*_T9;void*_TA;unsigned _TB;struct Cyc_Tcenv_Tenv*_TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;struct Cyc_Absyn_Vardecl*_TF;void*_T10;int _T11;unsigned _T12;int _T13;struct Cyc_Absyn_Vardecl*_T14;struct _tuple0*_T15;struct _fat_ptr _T16;struct Cyc_String_pa_PrintArg_struct _T17;struct Cyc_Absyn_Vardecl*_T18;void*_T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct Cyc_Absyn_Vardecl*_T1B;void*_T1C;void**_T1D;void**_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct Cyc_Absyn_Vardecl*_T21;struct Cyc_List_List*_T22;struct Cyc_Absyn_Vardecl*_T23;struct Cyc_Absyn_Vardecl*_T24;struct Cyc_List_List*_T25;struct Cyc_Absyn_Vardecl*_T26;struct Cyc_List_List*_T27;struct Cyc_Absyn_Vardecl*_T28;struct Cyc_Absyn_Vardecl*_T29;struct Cyc_Absyn_Vardecl*_T2A;struct Cyc_Absyn_Vardecl*_T2B;void*_T2C;int*_T2D;int _T2E;void*_T2F;void*_T30;struct Cyc_Absyn_Vardecl*_T31;void*_T32;int _T33;unsigned _T34;int _T35;struct Cyc_Absyn_Vardecl*_T36;struct _tuple0*_T37;struct _fat_ptr _T38;struct Cyc_Absyn_Fndecl*_T39;void*_T3A;struct Cyc_Absyn_Fndecl*_T3B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T3C;struct Cyc_Absyn_Fndecl*_T3D;unsigned _T3E;struct Cyc_Tcenv_Tenv*_T3F;void*_T40;struct Cyc_Absyn_Vardecl*_T41;void*_T42;int _T43;unsigned _T44;int _T45;struct Cyc_Absyn_Vardecl*_T46;struct _tuple0*_T47;struct _fat_ptr _T48;struct Cyc_String_pa_PrintArg_struct _T49;struct Cyc_String_pa_PrintArg_struct _T4A;struct Cyc_Absyn_Vardecl*_T4B;void*_T4C;void**_T4D;void**_T4E;struct _fat_ptr _T4F;struct _fat_ptr _T50;struct Cyc_Absyn_Vardecl*_T51;int*_T52;int _T53;struct Cyc_Absyn_FnInfo _T54;unsigned _T55;struct Cyc_Tcenv_Tenv*_T56;struct Cyc_Absyn_Vardecl*_T57;void*_T58;struct Cyc_Absyn_Fndecl*_T59;struct Cyc_Absyn_FnInfo _T5A;struct Cyc_List_List*_T5B;struct Cyc_Absyn_FnInfo _T5C;struct Cyc_List_List*_T5D;struct Cyc_Absyn_Fndecl*_T5E;struct Cyc_Absyn_FnInfo _T5F;struct Cyc_List_List*_T60;struct Cyc_Absyn_Fndecl*_T61;struct Cyc_Warn_String_Warn_Warg_struct _T62;void**_T63;struct _fat_ptr _T64;void*_T65;int*_T66;int _T67;void*_T68;void*_T69;struct Cyc_Absyn_Aggrdecl*_T6A;enum Cyc_Absyn_Scope _T6B;int _T6C;struct Cyc_Absyn_Aggrdecl*_T6D;enum Cyc_Absyn_Scope _T6E;int _T6F;unsigned _T70;int _T71;struct Cyc_Absyn_Aggrdecl*_T72;struct _tuple0*_T73;struct _fat_ptr _T74;struct Cyc_Absyn_Aggrdecl*_T75;struct Cyc_Absyn_AggrdeclImpl*_T76;unsigned _T77;int _T78;struct Cyc_Absyn_Aggrdecl*_T79;struct _tuple0*_T7A;struct _fat_ptr _T7B;struct Cyc_Absyn_Aggrdecl*_T7C;struct Cyc_Absyn_AggrdeclImpl*_T7D;unsigned _T7E;int _T7F;struct Cyc_Absyn_Aggrdecl*_T80;struct _tuple0*_T81;struct _fat_ptr _T82;struct Cyc_Absyn_Decl*_T83;struct Cyc_Absyn_Decl*_T84;struct _tuple12 _T85;struct Cyc_Absyn_Aggrdecl*_T86;struct Cyc_Absyn_Aggrdecl*_T87;struct Cyc_Absyn_AggrdeclImpl*_T88;struct Cyc_Absyn_AggrdeclImpl*_T89;struct Cyc_Absyn_AggrdeclImpl*_T8A;struct Cyc_Absyn_AggrdeclImpl*_T8B;struct Cyc_List_List**_T8C;struct Cyc_Absyn_AggrdeclImpl*_T8D;struct Cyc_List_List**_T8E;struct Cyc_Absyn_AggrdeclImpl*_T8F;struct Cyc_List_List**_T90;struct Cyc_Absyn_AggrdeclImpl*_T91;int _T92;unsigned _T93;int _T94;struct Cyc_Absyn_Aggrdecl*_T95;struct _tuple0*_T96;struct _fat_ptr _T97;struct Cyc_List_List*_T98;void*_T99;struct Cyc_List_List*_T9A;struct Cyc_Absyn_Aggrfield*_T9B;struct _fat_ptr*_T9C;unsigned _T9D;struct Cyc_Tcenv_Tenv*_T9E;struct Cyc_Absyn_Aggrfield*_T9F;void*_TA0;struct Cyc_Absyn_Aggrfield*_TA1;void*_TA2;int _TA3;unsigned _TA4;int _TA5;struct Cyc_Absyn_Aggrdecl*_TA6;struct _tuple0*_TA7;struct _fat_ptr _TA8;struct Cyc_String_pa_PrintArg_struct _TA9;struct Cyc_Absyn_Aggrfield*_TAA;void*_TAB;struct Cyc_String_pa_PrintArg_struct _TAC;struct Cyc_Absyn_Aggrfield*_TAD;struct _fat_ptr*_TAE;struct Cyc_String_pa_PrintArg_struct _TAF;struct Cyc_Absyn_Aggrfield*_TB0;void*_TB1;void**_TB2;void**_TB3;void**_TB4;struct _fat_ptr _TB5;struct _fat_ptr _TB6;struct Cyc_Absyn_Aggrfield*_TB7;struct Cyc_Absyn_Exp*_TB8;unsigned _TB9;int _TBA;struct Cyc_Absyn_Aggrdecl*_TBB;struct _tuple0*_TBC;struct _fat_ptr _TBD;struct Cyc_String_pa_PrintArg_struct _TBE;struct Cyc_Absyn_Aggrfield*_TBF;void*_TC0;void**_TC1;struct _fat_ptr _TC2;struct _fat_ptr _TC3;struct Cyc_Absyn_Aggrfield*_TC4;struct Cyc_List_List*_TC5;struct Cyc_Absyn_Aggrfield*_TC6;struct Cyc_Absyn_Aggrfield*_TC7;struct Cyc_List_List*_TC8;struct Cyc_Absyn_Aggrfield*_TC9;struct Cyc_List_List*_TCA;struct Cyc_Absyn_Aggrfield*_TCB;struct Cyc_Absyn_Aggrfield*_TCC;struct Cyc_Absyn_Aggrfield*_TCD;struct Cyc_Absyn_Aggrfield*_TCE;struct Cyc_Absyn_Aggrfield*_TCF;struct Cyc_Absyn_Aggrfield*_TD0;struct Cyc_List_List*_TD1;struct Cyc_Absyn_Aggrdecl*_TD2;struct Cyc_List_List*_TD3;struct Cyc_Absyn_Aggrdecl*_TD4;struct Cyc_Absyn_Aggrdecl*_TD5;struct Cyc_List_List*_TD6;unsigned _TD7;struct Cyc_List_List**_TD8;struct Cyc_List_List*_TD9;unsigned _TDA;struct Cyc_List_List**_TDB;struct Cyc_List_List*_TDC;unsigned _TDD;struct Cyc_List_List**_TDE;struct Cyc_List_List*_TDF;struct Cyc_List_List*_TE0;void*_TE1;struct Cyc_Absyn_Aggrfield*_TE2;struct _fat_ptr*_TE3;unsigned _TE4;int _TE5;struct Cyc_Absyn_Aggrdecl*_TE6;struct _tuple0*_TE7;struct _fat_ptr _TE8;struct Cyc_String_pa_PrintArg_struct _TE9;struct Cyc_List_List*_TEA;void*_TEB;struct Cyc_Absyn_Aggrfield*_TEC;struct _fat_ptr*_TED;void**_TEE;struct _fat_ptr _TEF;struct _fat_ptr _TF0;struct Cyc_List_List*_TF1;struct Cyc_Core_Impossible_exn_struct*_TF2;void*_TF3;void*_TF4;int*_TF5;int _TF6;void*_TF7;void*_TF8;unsigned _TF9;int _TFA;struct Cyc_Absyn_Enumdecl*_TFB;struct _tuple0*_TFC;struct _fat_ptr _TFD;void*_TFE;int*_TFF;int _T100;void*_T101;void*_T102;struct Cyc_Absyn_Typedefdecl*_T103;void*_T104;unsigned _T105;int _T106;struct Cyc_Absyn_Typedefdecl*_T107;struct _tuple0*_T108;struct _fat_ptr _T109;struct Cyc_Absyn_Typedefdecl*_T10A;void*_T10B;unsigned _T10C;int _T10D;struct Cyc_Absyn_Typedefdecl*_T10E;struct _tuple0*_T10F;struct _fat_ptr _T110;struct Cyc_Absyn_Decl*_T111;struct Cyc_Absyn_Decl*_T112;unsigned _T113;struct Cyc_Tcenv_Tenv*_T114;struct Cyc_Absyn_Typedefdecl*_T115;void*_T116;struct Cyc_Absyn_Typedefdecl*_T117;void*_T118;int _T119;unsigned _T11A;int _T11B;struct Cyc_Absyn_Typedefdecl*_T11C;struct _tuple0*_T11D;struct _fat_ptr _T11E;struct Cyc_String_pa_PrintArg_struct _T11F;struct Cyc_Absyn_Typedefdecl*_T120;void*_T121;void*_T122;struct Cyc_String_pa_PrintArg_struct _T123;struct Cyc_Absyn_Typedefdecl*_T124;void*_T125;void*_T126;void**_T127;void**_T128;struct _fat_ptr _T129;struct _fat_ptr _T12A;struct Cyc_Absyn_Typedefdecl*_T12B;struct Cyc_Absyn_Typedefdecl*_T12C;struct Cyc_Absyn_Typedefdecl*_T12D;struct Cyc_Absyn_Typedefdecl*_T12E;struct Cyc_Absyn_Typedefdecl*_T12F;struct Cyc_Absyn_Typedefdecl*_T130;struct Cyc_Absyn_Typedefdecl*_T131;struct Cyc_List_List*_T132;struct Cyc_Absyn_Typedefdecl*_T133;struct Cyc_Absyn_Typedefdecl*_T134;struct Cyc_List_List*_T135;struct Cyc_Absyn_Typedefdecl*_T136;struct Cyc_List_List*_T137;int _T138;struct Cyc_String_pa_PrintArg_struct _T139;struct Cyc_List_List*_T13A;struct Cyc_Absyn_Decl**_T13B;struct _fat_ptr _T13C;void**_T13D;struct Cyc___cycFILE*_T13E;struct _fat_ptr _T13F;struct _fat_ptr _T140;struct Cyc_Warn_String_Warn_Warg_struct _T141;struct Cyc_Warn_String_Warn_Warg_struct _T142;struct Cyc_List_List*_T143;struct Cyc_Absyn_Decl**_T144;struct _fat_ptr _T145;void**_T146;void**_T147;unsigned _T148;struct _fat_ptr _T149;{struct _tuple11 _T14A;_T1=c_decl;
# 290
_T14A.f0=_T1->r;_T2=user_decl;_T14A.f1=_T2->r;_T0=_T14A;}{struct _tuple11 _T14A=_T0;struct Cyc_Absyn_Typedefdecl*_T14B;struct Cyc_Absyn_Typedefdecl*_T14C;struct Cyc_Absyn_Enumdecl*_T14D;struct Cyc_Absyn_Enumdecl*_T14E;struct Cyc_Absyn_Aggrdecl*_T14F;struct Cyc_Absyn_Aggrdecl*_T150;struct Cyc_Absyn_Fndecl*_T151;struct Cyc_Absyn_Vardecl*_T152;struct Cyc_Absyn_Vardecl*_T153;_T3=_T14A.f0;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 0: _T6=_T14A.f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=0)goto _TL3F;_T9=_T14A.f0;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T9;_T153=_T154->f1;}_TA=_T14A.f1;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_TA;_T152=_T154->f1;}{struct Cyc_Absyn_Vardecl*cd=_T153;struct Cyc_Absyn_Vardecl*ud=_T152;_TB=loc;_TC=te;_TD=cd;_TE=_TD->type;_TF=ud;_T10=_TF->type;_T11=
# 292
Cyc_Cifc_c_types_ok(_TB,_TC,_TE,_T10);if(_T11)goto _TL41;else{goto _TL43;}
_TL43: _T12=loc;_T13=is_buildlib;_T14=cd;_T15=_T14->name;{struct Cyc_String_pa_PrintArg_struct _T154;_T154.tag=0;_T18=ud;_T19=_T18->type;_T154.f1=Cyc_Absynpp_typ2string(_T19);_T17=_T154;}{struct Cyc_String_pa_PrintArg_struct _T154=_T17;{struct Cyc_String_pa_PrintArg_struct _T155;_T155.tag=0;_T1B=cd;_T1C=_T1B->type;_T155.f1=Cyc_Absynpp_typ2string(_T1C);_T1A=_T155;}{struct Cyc_String_pa_PrintArg_struct _T155=_T1A;void*_T156[2];_T1D=_T156 + 0;*_T1D=& _T154;_T1E=_T156 + 1;*_T1E=& _T155;_T1F=_tag_fat(": type %s != %s",sizeof(char),16U);_T20=_tag_fat(_T156,sizeof(void*),2);_T16=Cyc_aprintf(_T1F,_T20);}}Cyc_Cifc_fail_merge(0,_T12,_T13,_T15,_T16);if(0)goto _TL44;else{goto _TL46;}_TL46: return;_TL44: goto _TL42;
# 300
_TL41: _T21=ud;_T22=_T21->attributes;if(_T22==0)goto _TL47;_T23=cd;_T24=cd;_T25=_T24->attributes;_T26=ud;_T27=_T26->attributes;
_T23->attributes=Cyc_Atts_merge_attributes(_T25,_T27);goto _TL48;_TL47: _TL48: _T28=cd;_T29=ud;
_T28->type=_T29->type;_T2A=cd;_T2B=ud;
_T2A->tq=_T2B->tq;_TL42: goto _LL0;}_TL3F: goto _LLB;case 1: _T2C=_T14A.f1;_T2D=(int*)_T2C;_T2E=*_T2D;if(_T2E!=0)goto _TL49;_T2F=_T14A.f0;{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T2F;_T151=_T154->f1;}_T30=_T14A.f1;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T30;_T153=_T154->f1;}{struct Cyc_Absyn_Fndecl*cd=_T151;struct Cyc_Absyn_Vardecl*ud=_T153;_T31=ud;_T32=_T31->type;_T33=
# 308
Cyc_Tcutil_is_function_type(_T32);if(_T33)goto _TL4B;else{goto _TL4D;}
_TL4D: _T34=loc;_T35=is_buildlib;_T36=ud;_T37=_T36->name;_T38=_tag_fat(": type must be a function type to match decl\n",sizeof(char),46U);Cyc_Cifc_fail_merge(0,_T34,_T35,_T37,_T38);if(0)goto _TL4E;else{goto _TL50;}_TL50: return;_TL4E: goto _TL4C;_TL4B: _TL4C: {
# 311
void*cdtype;_T39=cd;_T3A=_T39->cached_type;
if(_T3A==0)goto _TL51;_T3B=cd;
cdtype=_T3B->cached_type;goto _TL52;
# 315
_TL51:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T154=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T154->tag=6;_T3D=cd;_T154->f1=_T3D->i;_T3C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T154;}cdtype=(void*)_T3C;_TL52: _T3E=loc;_T3F=te;_T40=cdtype;_T41=ud;_T42=_T41->type;_T43=
Cyc_Cifc_c_types_ok(_T3E,_T3F,_T40,_T42);if(_T43)goto _TL53;else{goto _TL55;}
_TL55: _T44=loc;_T45=is_buildlib;_T46=ud;_T47=_T46->name;{struct Cyc_String_pa_PrintArg_struct _T154;_T154.tag=0;_T154.f1=Cyc_Absynpp_typ2string(cdtype);_T49=_T154;}{struct Cyc_String_pa_PrintArg_struct _T154=_T49;{struct Cyc_String_pa_PrintArg_struct _T155;_T155.tag=0;_T4B=ud;_T4C=_T4B->type;_T155.f1=Cyc_Absynpp_typ2string(_T4C);_T4A=_T155;}{struct Cyc_String_pa_PrintArg_struct _T155=_T4A;void*_T156[2];_T4D=_T156 + 0;*_T4D=& _T154;_T4E=_T156 + 1;*_T4E=& _T155;_T4F=_tag_fat(": type %s != %s",sizeof(char),16U);_T50=_tag_fat(_T156,sizeof(void*),2);_T48=Cyc_aprintf(_T4F,_T50);}}Cyc_Cifc_fail_merge(0,_T44,_T45,_T47,_T48);if(0)goto _TL56;else{goto _TL58;}_TL58: return;_TL56: goto _TL54;
# 324
_TL53: _T51=ud;{void*_T154=_T51->type;struct Cyc_Absyn_FnInfo _T155;_T52=(int*)_T154;_T53=*_T52;if(_T53!=6)goto _TL59;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T156=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T154;_T155=_T156->f1;}{struct Cyc_Absyn_FnInfo fi=_T155;_T54=fi;{
# 326
struct Cyc_List_List*old_tvars=_T54.tvars;_T55=loc;_T56=te;_T57=ud;_T58=_T57->type;
Cyc_Tctyp_check_valid_toplevel_type(_T55,_T56,_T58);_T59=cd;_T5A=_T59->i;_T5B=_T5A.attributes;
if(_T5B==0)goto _TL5B;_T5C=fi;_T5D=_T5C.attributes;_T5E=cd;_T5F=_T5E->i;_T60=_T5F.attributes;
fi.attributes=Cyc_Atts_merge_attributes(_T5D,_T60);goto _TL5C;_TL5B: _TL5C: _T61=cd;
_T61->i=fi;goto _LLD;}}_TL59:{struct Cyc_Warn_String_Warn_Warg_struct _T156;_T156.tag=0;
# 332
_T156.f1=_tag_fat("oops!\n",sizeof(char),7U);_T62=_T156;}{struct Cyc_Warn_String_Warn_Warg_struct _T156=_T62;void*_T157[1];_T63=_T157 + 0;*_T63=& _T156;_T64=_tag_fat(_T157,sizeof(void*),1);Cyc_Warn_err2(0U,_T64);}_LLD:;}_TL54: goto _LL0;}}_TL49: goto _LLB;case 5: _T65=_T14A.f1;_T66=(int*)_T65;_T67=*_T66;if(_T67!=5)goto _TL5D;_T68=_T14A.f0;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T68;_T150=_T154->f1;}_T69=_T14A.f1;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T69;_T14F=_T154->f1;}{struct Cyc_Absyn_Aggrdecl*cd=_T150;struct Cyc_Absyn_Aggrdecl*ud=_T14F;_T6A=ud;_T6B=_T6A->sc;_T6C=(int)_T6B;_T6D=cd;_T6E=_T6D->sc;_T6F=(int)_T6E;
# 343
if(_T6C==_T6F)goto _TL5F;_T70=loc;_T71=is_buildlib;_T72=cd;_T73=_T72->name;_T74=
_tag_fat(": scopes don't match (ignoring)",sizeof(char),32U);Cyc_Cifc_fail_merge(1,_T70,_T71,_T73,_T74);if(1)goto _TL61;else{goto _TL63;}_TL63: return;_TL61: goto _TL60;_TL5F: _TL60: _T75=ud;_T76=_T75->impl;
# 346
if(_T76!=0)goto _TL64;_T77=loc;_T78=is_buildlib;_T79=cd;_T7A=_T79->name;_T7B=
_tag_fat(": no user definition",sizeof(char),21U);Cyc_Cifc_fail_merge(0,_T77,_T78,_T7A,_T7B);if(0)goto _TL66;else{goto _TL68;}_TL68: return;_TL66: goto _TL65;_TL64: _TL65: _T7C=cd;_T7D=_T7C->impl;
if(_T7D!=0)goto _TL69;_T7E=loc;_T7F=is_buildlib;_T80=cd;_T81=_T80->name;_T82=
_tag_fat(": no definition for system version",sizeof(char),35U);Cyc_Cifc_fail_merge(1,_T7E,_T7F,_T81,_T82);if(1)goto _TL6B;else{goto _TL6D;}_TL6D: return;_TL6B: _T83=c_decl;_T84=user_decl;
_T83->r=_T84->r;
return;_TL69:{struct _tuple12 _T154;_T86=cd;
# 353
_T154.f0=_T86->impl;_T87=ud;_T154.f1=_T87->impl;_T85=_T154;}{struct _tuple12 _T154=_T85;struct Cyc_List_List*_T155;int _T156;struct Cyc_List_List*_T157;struct Cyc_List_List*_T158;struct Cyc_List_List*_T159;void*_T15A;struct Cyc_List_List*_T15B;void*_T15C;void*_T15D;_T88=_T154.f0;if(_T88==0)goto _TL6E;_T89=_T154.f1;if(_T89==0)goto _TL70;_T8A=_T154.f0;{struct Cyc_Absyn_AggrdeclImpl _T15E=*_T8A;_T8B=_T154.f0;_T8C=& _T8B->exist_vars;_T15D=(struct Cyc_List_List**)_T8C;_T8D=_T154.f0;_T8E=& _T8D->qual_bnd;_T15C=(struct Cyc_List_List**)_T8E;_T15B=_T15E.fields;_T8F=_T154.f0;_T90=& _T8F->effconstr;_T15A=(struct Cyc_List_List**)_T90;}_T91=_T154.f1;{struct Cyc_Absyn_AggrdeclImpl _T15E=*_T91;_T159=_T15E.exist_vars;_T158=_T15E.qual_bnd;_T157=_T15E.fields;_T156=_T15E.tagged;_T155=_T15E.effconstr;}{struct Cyc_List_List**tvarsC=(struct Cyc_List_List**)_T15D;struct Cyc_List_List**qbC=(struct Cyc_List_List**)_T15C;struct Cyc_List_List*cfields=_T15B;struct Cyc_List_List**effcC=(struct Cyc_List_List**)_T15A;struct Cyc_List_List*tvars=_T159;struct Cyc_List_List*qb=_T158;struct Cyc_List_List*ufields=_T157;int tagged=_T156;struct Cyc_List_List*effc=_T155;_T92=tagged;
# 356
if(!_T92)goto _TL72;_T93=loc;_T94=is_buildlib;_T95=cd;_T96=_T95->name;_T97=
_tag_fat(": user @tagged annotation not allowed (ignoring)",sizeof(char),49U);Cyc_Cifc_fail_merge(0,_T93,_T94,_T96,_T97);if(0)goto _TL74;else{goto _TL76;}_TL76: return;_TL74: goto _TL73;_TL72: _TL73: {
# 359
struct Cyc_List_List*x=cfields;
# 361
_TL77: if(x!=0)goto _TL78;else{goto _TL79;}
_TL78: _T98=x;_T99=_T98->hd;{struct Cyc_Absyn_Aggrfield*cfield=(struct Cyc_Absyn_Aggrfield*)_T99;_T9A=ufields;_T9B=cfield;_T9C=_T9B->name;{
struct Cyc_Absyn_Aggrfield*ufield=Cyc_Absyn_lookup_field(_T9A,_T9C);
# 365
if(ufield==0)goto _TL7A;_T9D=loc;_T9E=te;_T9F=cfield;_TA0=_T9F->type;_TA1=ufield;_TA2=_TA1->type;_TA3=
# 368
Cyc_Cifc_c_types_ok(_T9D,_T9E,_TA0,_TA2);if(_TA3)goto _TL7C;else{goto _TL7E;}
_TL7E: _TA4=loc;_TA5=is_buildlib;_TA6=cd;_TA7=_TA6->name;{struct Cyc_String_pa_PrintArg_struct _T15E;_T15E.tag=0;_TAA=ufield;_TAB=_TAA->type;_T15E.f1=Cyc_Absynpp_typ2string(_TAB);_TA9=_T15E;}{struct Cyc_String_pa_PrintArg_struct _T15E=_TA9;{struct Cyc_String_pa_PrintArg_struct _T15F;_T15F.tag=0;_TAD=cfield;_TAE=_TAD->name;_T15F.f1=*_TAE;_TAC=_T15F;}{struct Cyc_String_pa_PrintArg_struct _T15F=_TAC;{struct Cyc_String_pa_PrintArg_struct _T160;_T160.tag=0;_TB0=cfield;_TB1=_TB0->type;_T160.f1=Cyc_Absynpp_typ2string(_TB1);_TAF=_T160;}{struct Cyc_String_pa_PrintArg_struct _T160=_TAF;void*_T161[3];_TB2=_T161 + 0;*_TB2=& _T15E;_TB3=_T161 + 1;*_TB3=& _T15F;_TB4=_T161 + 2;*_TB4=& _T160;_TB5=_tag_fat(": type %s of user definition of field %s != %s",sizeof(char),47U);_TB6=_tag_fat(_T161,sizeof(void*),3);_TA8=Cyc_aprintf(_TB5,_TB6);}}}Cyc_Cifc_fail_merge(0,_TA4,_TA5,_TA7,_TA8);if(0)goto _TL7F;else{goto _TL81;}_TL81: return;_TL7F: goto _TL7D;
# 377
_TL7C: _TB7=ufield;_TB8=_TB7->width;if(_TB8==0)goto _TL82;_TB9=loc;_TBA=is_buildlib;_TBB=cd;_TBC=_TBB->name;{struct Cyc_String_pa_PrintArg_struct _T15E;_T15E.tag=0;_TBF=ufield;_TC0=_TBF->type;
_T15E.f1=Cyc_Absynpp_typ2string(_TC0);_TBE=_T15E;}{struct Cyc_String_pa_PrintArg_struct _T15E=_TBE;void*_T15F[1];_TC1=_T15F + 0;*_TC1=& _T15E;_TC2=_tag_fat(": ignoring width of user definition of field %s",sizeof(char),48U);_TC3=_tag_fat(_T15F,sizeof(void*),1);_TBD=Cyc_aprintf(_TC2,_TC3);}Cyc_Cifc_fail_merge(1,_TB9,_TBA,_TBC,_TBD);if(1)goto _TL84;else{goto _TL86;}_TL86: return;_TL84: goto _TL83;_TL82: _TL83: _TC4=ufield;_TC5=_TC4->attributes;
# 382
if(_TC5==0)goto _TL87;_TC6=cfield;_TC7=cfield;_TC8=_TC7->attributes;_TC9=ufield;_TCA=_TC9->attributes;
_TC6->attributes=Cyc_Atts_merge_attributes(_TC8,_TCA);goto _TL88;_TL87: _TL88: _TCB=cfield;_TCC=ufield;
# 386
_TCB->type=_TCC->type;_TCD=cfield;_TCE=ufield;
_TCD->tq=_TCE->tq;_TCF=cfield;_TD0=ufield;
_TCF->requires_clause=_TD0->requires_clause;_TL7D: goto _TL7B;_TL7A: _TL7B: _TD1=x;
# 391
x=_TD1->tl;}}goto _TL77;_TL79: _TD2=ud;_TD3=_TD2->tvs;
# 395
if(_TD3==0)goto _TL89;_TD4=cd;_TD5=ud;_TD4->tvs=_TD5->tvs;goto _TL8A;_TL89: _TL8A: _TD6=tvars;_TD7=(unsigned)_TD6;
if(!_TD7)goto _TL8B;_TD8=tvarsC;*_TD8=tvars;goto _TL8C;_TL8B: _TL8C: _TD9=effc;_TDA=(unsigned)_TD9;
if(!_TDA)goto _TL8D;_TDB=effcC;*_TDB=effc;goto _TL8E;_TL8D: _TL8E: _TDC=qb;_TDD=(unsigned)_TDC;
if(!_TDD)goto _TL8F;_TDE=qbC;*_TDE=qb;goto _TL90;_TL8F: _TL90:
# 401
 x=ufields;{
int missing_fields=0;
_TL91: if(x!=0)goto _TL92;else{goto _TL93;}
_TL92: _TDF=cfields;_TE0=x;_TE1=_TE0->hd;_TE2=(struct Cyc_Absyn_Aggrfield*)_TE1;_TE3=_TE2->name;{struct Cyc_Absyn_Aggrfield*cfield=Cyc_Absyn_lookup_field(_TDF,_TE3);
if(cfield!=0)goto _TL94;
missing_fields=1;_TE4=loc;_TE5=is_buildlib;_TE6=cd;_TE7=_TE6->name;{struct Cyc_String_pa_PrintArg_struct _T15E;_T15E.tag=0;_TEA=x;_TEB=_TEA->hd;_TEC=(struct Cyc_Absyn_Aggrfield*)_TEB;_TED=_TEC->name;
_T15E.f1=*_TED;_TE9=_T15E;}{struct Cyc_String_pa_PrintArg_struct _T15E=_TE9;void*_T15F[1];_TEE=_T15F + 0;*_TEE=& _T15E;_TEF=_tag_fat(": no definition of field %s in system version",sizeof(char),46U);_TF0=_tag_fat(_T15F,sizeof(void*),1);_TE8=Cyc_aprintf(_TEF,_TF0);}Cyc_Cifc_fail_merge(1,_TE4,_TE5,_TE7,_TE8);if(1)goto _TL96;else{goto _TL98;}_TL98: return;_TL96: goto _TL95;_TL94: _TL95: _TF1=x;
# 411
x=_TF1->tl;}goto _TL91;_TL93: goto _LL12;}}}_TL70: goto _LL15;_TL6E: _LL15:{struct Cyc_Core_Impossible_exn_struct*_T15E=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T15E->tag=Cyc_Core_Impossible;
# 415
_T15E->f1=_tag_fat("Internal Error: NULL cases not possible",sizeof(char),40U);_TF2=(struct Cyc_Core_Impossible_exn_struct*)_T15E;}_TF3=(void*)_TF2;_throw(_TF3);_LL12:;}goto _LL0;}_TL5D: goto _LLB;case 7: _TF4=_T14A.f1;_TF5=(int*)_TF4;_TF6=*_TF5;if(_TF6!=7)goto _TL99;_TF7=_T14A.f0;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TF7;_T14E=_T154->f1;}_TF8=_T14A.f1;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TF8;_T14D=_T154->f1;}{struct Cyc_Absyn_Enumdecl*cd=_T14E;struct Cyc_Absyn_Enumdecl*ud=_T14D;_TF9=loc;_TFA=is_buildlib;_TFB=cd;_TFC=_TFB->name;_TFD=
# 420
_tag_fat(": enum merging not currently supported",sizeof(char),39U);Cyc_Cifc_fail_merge(0,_TF9,_TFA,_TFC,_TFD);if(0)goto _TL9B;else{goto _TL9D;}_TL9D: return;_TL9B:;}goto _TL9A;_TL99: goto _LLB;_TL9A:;case 8: _TFE=_T14A.f1;_TFF=(int*)_TFE;_T100=*_TFF;if(_T100!=8)goto _TL9E;_T101=_T14A.f0;{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T101;_T14C=_T154->f1;}_T102=_T14A.f1;{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T154=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T102;_T14B=_T154->f1;}{struct Cyc_Absyn_Typedefdecl*cd=_T14C;struct Cyc_Absyn_Typedefdecl*ud=_T14B;_T103=ud;_T104=_T103->defn;
# 423
if(_T104!=0)goto _TLA0;_T105=loc;_T106=is_buildlib;_T107=cd;_T108=_T107->name;_T109=
_tag_fat(": no user definition",sizeof(char),21U);Cyc_Cifc_fail_merge(0,_T105,_T106,_T108,_T109);if(0)goto _TLA2;else{goto _TLA4;}_TLA4: return;_TLA2: goto _TLA1;_TLA0: _TLA1: _T10A=cd;_T10B=_T10A->defn;
if(_T10B!=0)goto _TLA5;_T10C=loc;_T10D=is_buildlib;_T10E=cd;_T10F=_T10E->name;_T110=
_tag_fat(": no definition for system version",sizeof(char),35U);Cyc_Cifc_fail_merge(1,_T10C,_T10D,_T10F,_T110);if(1)goto _TLA7;else{goto _TLA9;}_TLA9: return;_TLA7: _T111=c_decl;_T112=user_decl;
_T111->r=_T112->r;
return;_TLA5: _T113=loc;_T114=te;_T115=cd;_T116=_T115->defn;_T117=ud;_T118=_T117->defn;_T119=
# 431
Cyc_Cifc_c_types_ok(_T113,_T114,_T116,_T118);if(_T119)goto _TLAA;else{goto _TLAC;}
_TLAC: _T11A=loc;_T11B=is_buildlib;_T11C=cd;_T11D=_T11C->name;{struct Cyc_String_pa_PrintArg_struct _T154;_T154.tag=0;_T120=ud;_T121=_T120->defn;_T122=_check_null(_T121);_T154.f1=Cyc_Absynpp_typ2string(_T122);_T11F=_T154;}{struct Cyc_String_pa_PrintArg_struct _T154=_T11F;{struct Cyc_String_pa_PrintArg_struct _T155;_T155.tag=0;_T124=cd;_T125=_T124->defn;_T126=_check_null(_T125);_T155.f1=Cyc_Absynpp_typ2string(_T126);_T123=_T155;}{struct Cyc_String_pa_PrintArg_struct _T155=_T123;void*_T156[2];_T127=_T156 + 0;*_T127=& _T154;_T128=_T156 + 1;*_T128=& _T155;_T129=_tag_fat(": type definition %s of user definition != %s",sizeof(char),46U);_T12A=_tag_fat(_T156,sizeof(void*),2);_T11E=Cyc_aprintf(_T129,_T12A);}}Cyc_Cifc_fail_merge(0,_T11A,_T11B,_T11D,_T11E);if(0)goto _TLAD;else{goto _TLAF;}_TLAF: return;_TLAD: goto _TLAB;
# 438
_TLAA: _T12B=cd;_T12C=ud;_T12B->tvs=_T12C->tvs;_T12D=cd;_T12E=ud;
_T12D->defn=_T12E->defn;_T12F=cd;_T130=ud;
_T12F->tq=_T130->tq;_T131=ud;_T132=_T131->atts;
if(_T132==0)goto _TLB0;_T133=cd;_T134=cd;_T135=_T134->atts;_T136=ud;_T137=_T136->atts;
_T133->atts=Cyc_Atts_merge_attributes(_T135,_T137);goto _TLB1;_TLB0: _TLB1: _TLAB: goto _LL0;}_TL9E: goto _LLB;default: _LLB: _T138=is_buildlib;
# 448
if(!_T138)goto _TLB2;{struct Cyc_String_pa_PrintArg_struct _T154;_T154.tag=0;{struct Cyc_Absyn_Decl*_T155[1];_T13B=_T155 + 0;*_T13B=user_decl;_T13C=_tag_fat(_T155,sizeof(struct Cyc_Absyn_Decl*),1);_T13A=Cyc_List_list(_T13C);}
# 450
_T154.f1=Cyc_Absynpp_decllist2string(_T13A);_T139=_T154;}{struct Cyc_String_pa_PrintArg_struct _T154=_T139;void*_T155[1];_T13D=_T155 + 0;*_T13D=& _T154;_T13E=Cyc_stderr;_T13F=
# 449
_tag_fat("Error in .cys file: bad (or unsupported) user-defined type %s\n",sizeof(char),63U);_T140=_tag_fat(_T155,sizeof(void*),1);Cyc_fprintf(_T13E,_T13F,_T140);}goto _TLB3;
# 452
_TLB2:{struct Cyc_Warn_String_Warn_Warg_struct _T154;_T154.tag=0;_T154.f1=_tag_fat("bad (or unsupported) user-defined type %s",sizeof(char),42U);_T141=_T154;}{struct Cyc_Warn_String_Warn_Warg_struct _T154=_T141;{struct Cyc_Warn_String_Warn_Warg_struct _T155;_T155.tag=0;{struct Cyc_Absyn_Decl*_T156[1];_T144=_T156 + 0;*_T144=user_decl;_T145=_tag_fat(_T156,sizeof(struct Cyc_Absyn_Decl*),1);_T143=Cyc_List_list(_T145);}
_T155.f1=Cyc_Absynpp_decllist2string(_T143);_T142=_T155;}{struct Cyc_Warn_String_Warn_Warg_struct _T155=_T142;void*_T156[2];_T146=_T156 + 0;*_T146=& _T154;_T147=_T156 + 1;*_T147=& _T155;_T148=loc;_T149=_tag_fat(_T156,sizeof(void*),2);Cyc_Warn_err2(_T148,_T149);}}_TLB3:
 return;}_LL0:;}}
# 458
static int Cyc_Cifc_contains_type_vars(struct Cyc_Absyn_Decl*ud){struct Cyc_Absyn_Decl*_T0;int*_T1;int _T2;struct Cyc_Absyn_Aggrdecl*_T3;struct Cyc_List_List*_T4;int _T5;_T0=ud;{
void*_T6=_T0->r;struct Cyc_Absyn_Aggrdecl*_T7;_T1=(int*)_T6;_T2=*_T1;if(_T2!=5)goto _TLB4;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T8=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T6;_T7=_T8->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T7;_T3=ad;_T4=_T3->tvs;_T5=_T4!=0;
# 461
return _T5;}_TLB4: goto _LL0;_LL0:;}
# 465
return 0;}
# 468
static struct Cyc_Absyn_Decl*Cyc_Cifc_make_abstract_decl(struct Cyc_Absyn_Decl*ud){struct Cyc_Absyn_Decl*_T0;int*_T1;int _T2;struct Cyc_Absyn_Aggrdecl*_T3;struct Cyc_Absyn_Aggrdecl*_T4;struct Cyc_Absyn_Aggrdecl*_T5;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T6;void*_T7;struct Cyc_Absyn_Decl*_T8;unsigned _T9;struct Cyc_Absyn_Decl*_TA;int(*_TB)(struct _fat_ptr,struct _fat_ptr);void*(*_TC)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TD;struct _fat_ptr _TE;_T0=ud;{
void*_TF=_T0->r;struct Cyc_Absyn_Aggrdecl*_T10;_T1=(int*)_TF;_T2=*_T1;if(_T2!=5)goto _TLB6;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_TF;_T10=_T11->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T10;
# 471
struct Cyc_Absyn_Aggrdecl*absad;absad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T3=absad;_T4=ad;*_T3=*_T4;_T5=absad;
_T5->impl=0;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T11=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T11->tag=5;
_T11->f1=absad;_T6=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T11;}_T7=(void*)_T6;_T8=ud;_T9=_T8->loc;{struct Cyc_Absyn_Decl*nd=Cyc_Absyn_new_decl(_T7,_T9);_TA=nd;
return _TA;}}_TLB6: _TC=Cyc_Warn_impos;{
# 476
int(*_T11)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TC;_TB=_T11;}_TD=_tag_fat("Only aggrdecls",sizeof(char),15U);_TE=_tag_fat(0U,sizeof(void*),0);_TB(_TD,_TE);;}}
# 481
static int Cyc_Cifc_kindbound_subsumed(void*kb1,void*kb2){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;int*_T5;int _T6;void*_T7;int _T8;void*_T9;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_TA;struct Cyc_Core_Opt*_TB;void*_TC;struct Cyc_Core_Opt*_TD;void*_TE;void*_TF;int*_T10;int _T11;void*_T12;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T13;struct Cyc_Core_Opt*_T14;void*_T15;struct Cyc_Core_Opt*_T16;void*_T17;int _T18;void*_T19;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T1A;struct Cyc_Core_Opt*_T1B;void*_T1C;struct Cyc_Core_Opt*_T1D;void*_T1E;void*_T1F;int*_T20;int _T21;void*_T22;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T23;struct Cyc_Core_Opt*_T24;void*_T25;struct Cyc_Core_Opt*_T26;void*_T27;int _T28;int _T29;void*_T2A;void*_T2B;int*_T2C;int _T2D;void*_T2E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2F;struct Cyc_Core_Opt*_T30;void*_T31;int _T32;struct Cyc_Absyn_Kind*_T33;void*_T34;_T0=kb1;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=kb1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T35=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3;_T34=_T35->f1;}{struct Cyc_Absyn_Kind*k1=_T34;struct Cyc_Absyn_Kind*_T35;_T4=kb2;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TLB9;_T7=kb2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T36=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T7;_T35=_T36->f1;}{struct Cyc_Absyn_Kind*k2=_T35;_T8=
# 486
Cyc_Kinds_kind_eq(k1,k2);return _T8;}_TLB9:
# 488
 return 0;;}case 1: _T9=kb1;_TA=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T9;_TB=_TA->f1;if(_TB==0)goto _TLBB;_TC=kb1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T35=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_TC;_TD=_T35->f1;{struct Cyc_Core_Opt _T36=*_TD;_TE=_T36.v;_T34=(void*)_TE;}}{void*kbb1=_T34;void*_T35;_TF=kb2;_T10=(int*)_TF;_T11=*_T10;if(_T11!=1)goto _TLBD;_T12=kb2;_T13=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T12;_T14=_T13->f1;if(_T14==0)goto _TLBF;_T15=kb2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T36=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T15;_T16=_T36->f1;{struct Cyc_Core_Opt _T37=*_T16;_T17=_T37.v;_T35=(void*)_T17;}}{void*kbb2=_T35;_T18=
# 493
Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);return _T18;}_TLBF: goto _LL13;_TLBD: _LL13:
# 495
 return 1;;}goto _TLBC;_TLBB:
# 498
 return 1;_TLBC:;default: _T19=kb1;_T1A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T19;_T1B=_T1A->f1;if(_T1B==0)goto _TLC1;_T1C=kb1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T35=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T1C;_T1D=_T35->f1;{struct Cyc_Core_Opt _T36=*_T1D;_T1E=_T36.v;_T34=(void*)_T1E;}_T33=_T35->f2;}{void*kbb1=_T34;struct Cyc_Absyn_Kind*k1=_T33;struct Cyc_Absyn_Kind*_T35;void*_T36;_T1F=kb2;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=2)goto _TLC3;_T22=kb2;_T23=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T22;_T24=_T23->f1;if(_T24==0)goto _TLC5;_T25=kb2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T37=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T25;_T26=_T37->f1;{struct Cyc_Core_Opt _T38=*_T26;_T27=_T38.v;_T36=(void*)_T27;}_T35=_T37->f2;}{void*kbb2=_T36;struct Cyc_Absyn_Kind*k2=_T35;_T29=
# 502
Cyc_Kinds_kind_eq(k1,k2);if(!_T29)goto _TLC7;_T28=Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);goto _TLC8;_TLC7: _T28=0;_TLC8: return _T28;}_TLC5: goto _LL18;_TLC3: _LL18:
# 504
 return 0;;}goto _TLC2;_TLC1: _T2A=kb1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T35=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T2A;_T33=_T35->f2;}{struct Cyc_Absyn_Kind*k1=_T33;struct Cyc_Absyn_Kind*_T35;_T2B=kb2;_T2C=(int*)_T2B;_T2D=*_T2C;if(_T2D!=2)goto _TLC9;_T2E=kb2;_T2F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T2E;_T30=_T2F->f1;if(_T30!=0)goto _TLCB;_T31=kb2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T36=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T31;_T35=_T36->f2;}{struct Cyc_Absyn_Kind*k2=_T35;_T32=
# 509
Cyc_Kinds_kind_eq(k1,k2);return _T32;}_TLCB: goto _LL1D;_TLC9: _LL1D:
# 511
 return 0;;}_TLC2:;};}
# 516
static int Cyc_Cifc_find_and_remove(struct Cyc_List_List**lst,void*kind){struct Cyc_List_List**_T0;struct Cyc_List_List*_T1;unsigned _T2;struct Cyc_List_List*_T3;void*_T4;int*_T5;int _T6;void*_T7;struct Cyc_Absyn_Tvar*_T8;void*_T9;void*_TA;int _TB;struct Cyc_List_List*_TC;unsigned _TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List**_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_String_pa_PrintArg_struct _T13;struct Cyc_String_pa_PrintArg_struct _T14;void**_T15;void**_T16;int(*_T17)(struct _fat_ptr,struct _fat_ptr);void*(*_T18)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T19;struct _fat_ptr _T1A;struct Cyc_List_List*_T1B;_T0=lst;{
struct Cyc_List_List*cur=*_T0;
struct Cyc_List_List*prev=0;
_TLCD: _T1=cur;_T2=(unsigned)_T1;if(_T2)goto _TLCE;else{goto _TLCF;}
_TLCE: _T3=cur;{void*t=_T3->hd;{struct Cyc_Absyn_Tvar*_T1C;_T4=t;_T5=(int*)_T4;_T6=*_T5;if(_T6!=2)goto _TLD0;_T7=t;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T1D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T7;_T1C=_T1D->f1;}{struct Cyc_Absyn_Tvar*tv=_T1C;_T8=tv;_T9=_T8->kind;_TA=kind;_TB=
# 523
Cyc_Cifc_kindbound_subsumed(_T9,_TA);if(!_TB)goto _TLD2;_TC=prev;_TD=(unsigned)_TC;
if(!_TD)goto _TLD4;_TE=prev;_TF=cur;
_TE->tl=_TF->tl;goto _TLD5;
# 527
_TLD4: _T10=lst;_T11=cur;*_T10=_T11->tl;_TLD5: _T12=cur;
_T12->tl=0;
return 1;_TLD2: goto _LL0;}_TLD0:{struct Cyc_String_pa_PrintArg_struct _T1D;_T1D.tag=0;
# 533
_T1D.f1=Cyc_Absynpp_typ2string(t);_T13=_T1D;}{struct Cyc_String_pa_PrintArg_struct _T1D=_T13;{struct Cyc_String_pa_PrintArg_struct _T1E;_T1E.tag=0;_T1E.f1=Cyc_Cifc_get_type_kind(t);_T14=_T1E;}{struct Cyc_String_pa_PrintArg_struct _T1E=_T14;void*_T1F[2];_T15=_T1F + 0;*_T15=& _T1D;_T16=_T1F + 1;*_T16=& _T1E;_T18=Cyc_Warn_impos;{int(*_T20)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T18;_T17=_T20;}_T19=_tag_fat("expects a VarType list only -- got %s(%s)",sizeof(char),42U);_T1A=_tag_fat(_T1F,sizeof(void*),2);_T17(_T19,_T1A);}}_LL0:;}
# 535
prev=cur;_T1B=cur;
cur=_T1B->tl;}goto _TLCD;_TLCF:
# 538
 return 0;}}
# 541
static struct Cyc_List_List*Cyc_Cifc_get_tvar_difference(struct Cyc_List_List*tvs,struct Cyc_List_List*remove){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List**_T4;struct Cyc_Absyn_Tvar*_T5;void*_T6;int _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;
struct Cyc_List_List*ret=0;
_TLD6: _T0=tvs;_T1=(unsigned)_T0;if(_T1)goto _TLD7;else{goto _TLD8;}
_TLD7: _T2=tvs;_T3=_T2->hd;{struct Cyc_Absyn_Tvar*can=(struct Cyc_Absyn_Tvar*)_T3;_T4=& remove;_T5=can;_T6=_T5->kind;_T7=
Cyc_Cifc_find_and_remove(_T4,_T6);if(_T7)goto _TLD9;else{goto _TLDB;}
_TLDB:{struct Cyc_List_List*_TB=_cycalloc(sizeof(struct Cyc_List_List));_TB->hd=can;_TB->tl=ret;_T8=(struct Cyc_List_List*)_TB;}ret=_T8;goto _TLDA;_TLD9: _TLDA: _T9=tvs;
tvs=_T9->tl;}goto _TLD6;_TLD8: _TA=ret;
# 549
return _TA;}char Cyc_Cifc_Contains_nontvar[17U]="Contains_nontvar";struct Cyc_Cifc_Contains_nontvar_exn_struct{char*tag;};
# 554
struct Cyc_Cifc_Contains_nontvar_exn_struct Cyc_Cifc_Contains_nontvar_val={Cyc_Cifc_Contains_nontvar};
# 556
static struct Cyc_List_List*Cyc_Cifc_extract_tvars(struct Cyc_List_List*ts){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_List_List*_T7;struct Cyc_Cifc_Contains_nontvar_exn_struct*_T8;struct Cyc_Cifc_Contains_nontvar_exn_struct*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;
struct Cyc_List_List*res=0;
_TLDC: _T0=ts;_T1=(unsigned)_T0;if(_T1)goto _TLDD;else{goto _TLDE;}
_TLDD: _T2=ts;{void*t=_T2->hd;{struct Cyc_Absyn_Tvar*_TC;_T3=t;_T4=(int*)_T3;_T5=*_T4;if(_T5!=2)goto _TLDF;_T6=t;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T6;_TC=_TD->f1;}{struct Cyc_Absyn_Tvar*tv=_TC;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));
# 561
_TD->hd=tv;_TD->tl=res;_T7=(struct Cyc_List_List*)_TD;}res=_T7;goto _LL0;}_TLDF: _T8=& Cyc_Cifc_Contains_nontvar_val;_T9=(struct Cyc_Cifc_Contains_nontvar_exn_struct*)_T8;_throw(_T9);_LL0:;}_TA=ts;
# 564
ts=_TA->tl;}goto _TLDC;_TLDE: _TB=res;
# 566
return _TB;}
# 569
static void*Cyc_Cifc_instantiate_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Kind*_T3;enum Cyc_Absyn_KindQual _T4;int _T5;struct Cyc_Absyn_Kind*_T6;enum Cyc_Absyn_KindQual _T7;int _T8;void*_T9;struct Cyc_Absyn_Tqual _TA;void*_TB;struct Cyc_Absyn_Kind*_TC;enum Cyc_Absyn_KindQual _TD;int _TE;void*_TF;void*_T10;struct Cyc_Absyn_Tqual _T11;void*_T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;void**_T15;void**_T16;unsigned _T17;struct _fat_ptr _T18;void*_T19;_T0=tv;{
void*_T1A=_T0->kind;struct Cyc_Absyn_Kind*_T1B;_T1=(int*)_T1A;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1A;_T1B=_T1C->f1;}{struct Cyc_Absyn_Kind*k=_T1B;_T1B=k;goto _LL4;}case 2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T1C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T1A;_T1B=_T1C->f2;}_LL4: {struct Cyc_Absyn_Kind*k=_T1B;_T3=k;_T4=_T3->kind;_T5=(int)_T4;
# 573
if(_T5==2)goto _TLE4;else{goto _TLE5;}_TLE5: _T6=k;_T7=_T6->kind;_T8=(int)_T7;if(_T8==0)goto _TLE4;else{goto _TLE2;}
_TLE4: _T9=Cyc_Absyn_void_type;_TA=Cyc_Absyn_empty_tqual(loc);_TB=Cyc_Absyn_cstar_type(_T9,_TA);return _TB;
_TLE2: _TC=k;_TD=_TC->kind;_TE=(int)_TD;if(_TE!=3)goto _TLE6;_TF=Cyc_Absyn_heap_rgn_type;
return _TF;_TLE6: goto _LL0;}default: _T10=Cyc_Absyn_void_type;_T11=
# 579
Cyc_Absyn_empty_tqual(loc);_T12=Cyc_Absyn_cstar_type(_T10,_T11);return _T12;}_LL0:;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;
# 581
_T1A.f1=_tag_fat("Unable to instantiate tvar ",sizeof(char),28U);_T13=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_T13;{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;_T1B.f1=Cyc_Absynpp_tvar2string(tv);_T14=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_T14;void*_T1C[2];_T15=_T1C + 0;*_T15=& _T1A;_T16=_T1C + 1;*_T16=& _T1B;_T17=loc;_T18=_tag_fat(_T1C,sizeof(void*),2);Cyc_Warn_err2(_T17,_T18);}}_T19=Cyc_Absyn_void_type;
return _T19;}struct _tuple13{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 591
static struct _tuple13*Cyc_Cifc_update_tvars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,void*t,struct Cyc_Absyn_Decl*enclosing_decl,int instantiate){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6;struct Cyc_List_List**_T7;void*_T8;int*_T9;int _TA;void*_TB;union Cyc_Absyn_AggrInfo _TC;struct _union_AggrInfo_KnownAggr _TD;unsigned _TE;union Cyc_Absyn_AggrInfo _TF;struct _union_AggrInfo_KnownAggr _T10;struct Cyc_Absyn_Aggrdecl**_T11;struct Cyc_Absyn_Aggrdecl*_T12;struct Cyc_Absyn_Decl*_T13;unsigned _T14;struct Cyc_Absyn_Decl*_T15;int*_T16;int _T17;struct _handler_cons*_T18;int*_T19;int _T1A;struct Cyc_List_List**_T1B;struct Cyc_List_List*_T1C;void*_T1D;struct Cyc_Cifc_Contains_nontvar_exn_struct*_T1E;char*_T1F;char*_T20;struct Cyc_List_List**_T21;struct Cyc_Absyn_Aggrdecl*_T22;struct Cyc_List_List*_T23;unsigned _T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_List_List**_T27;struct Cyc_List_List*_T28;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T29;struct Cyc_List_List**_T2A;int _T2B;int _T2C;struct Cyc_List_List**_T2D;struct Cyc_List_List*_T2E;struct Cyc_Absyn_Decl*_T2F;unsigned _T30;struct Cyc_Absyn_Tvar*_T31;struct Cyc_List_List**_T32;struct Cyc_Absyn_Tvar*_T33;struct Cyc_Absyn_Tvar*_T34;struct _fat_ptr*_T35;struct _fat_ptr _T36;struct Cyc_Int_pa_PrintArg_struct _T37;int _T38;void**_T39;struct _fat_ptr _T3A;struct _fat_ptr _T3B;struct Cyc_Absyn_Tvar*_T3C;struct Cyc_List_List*_T3D;struct Cyc_List_List**_T3E;struct Cyc_List_List*_T3F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T40;struct Cyc_List_List**_T41;struct Cyc_List_List*_T42;struct Cyc_List_List**_T43;struct Cyc_List_List**_T44;struct Cyc_List_List*_T45;struct _tuple13*_T46;int _T47;int _T48;struct _tuple13*_T49;int(*_T4A)(struct _fat_ptr,struct _fat_ptr);void*(*_T4B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T4C;struct _fat_ptr _T4D;union Cyc_Absyn_AggrInfo _T4E;struct _union_AggrInfo_UnknownAggr _T4F;void*_T50;void*_T51;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T52;struct Cyc_Absyn_PtrInfo*_T53;struct Cyc_Tcenv_Tenv*_T54;struct Cyc_List_List*_T55;struct Cyc_Absyn_PtrInfo*_T56;void*_T57;struct Cyc_Absyn_Decl*_T58;int _T59;struct _tuple13*_T5A;void*_T5B;int _T5C;struct Cyc_Tcenv_Tenv*_T5D;struct Cyc_List_List*_T5E;struct Cyc_Absyn_FnInfo _T5F;void*_T60;struct Cyc_Absyn_Decl*_T61;int _T62;struct Cyc_Absyn_FnInfo _T63;struct Cyc_List_List*_T64;unsigned _T65;struct Cyc_List_List*_T66;void*_T67;struct _tuple13*_T68;unsigned _T69;struct _tuple13*_T6A;struct _tuple13 _T6B;struct Cyc_List_List*_T6C;unsigned _T6D;struct _tuple13*_T6E;struct _tuple13 _T6F;struct Cyc_List_List*_T70;struct Cyc_List_List*_T71;struct _tuple13*_T72;struct _tuple13 _T73;struct Cyc_List_List*_T74;unsigned _T75;struct _tuple13*_T76;struct _tuple13 _T77;struct Cyc_List_List*_T78;struct Cyc_List_List*_T79;struct Cyc_List_List*_T7A;struct Cyc_List_List*_T7B;unsigned _T7C;struct Cyc_List_List*_T7D;unsigned _T7E;struct _tuple13*_T7F;void*_T80;void*_T81;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T82;struct Cyc_Absyn_ArrayInfo*_T83;struct Cyc_Tcenv_Tenv*_T84;struct Cyc_List_List*_T85;struct Cyc_Absyn_ArrayInfo*_T86;void*_T87;struct Cyc_Absyn_Decl*_T88;int _T89;struct _tuple13*_T8A;{struct Cyc_Absyn_FnInfo _T8B;void*_T8C;void*_T8D;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T4=_T8E->f1;_T8D=(void*)_T4;_T5=t;_T6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T5;_T7=& _T6->f2;_T8C=(struct Cyc_List_List**)_T7;}{void*tc=_T8D;struct Cyc_List_List**ts=(struct Cyc_List_List**)_T8C;{union Cyc_Absyn_AggrInfo _T8E;_T8=tc;_T9=(int*)_T8;_TA=*_T9;if(_TA!=24)goto _TLE9;_TB=tc;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T8F=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TB;_T8E=_T8F->f1;}{union Cyc_Absyn_AggrInfo ai=_T8E;struct _tuple2 _T8F;struct Cyc_Absyn_Aggrdecl*_T90;_TC=ai;_TD=_TC.KnownAggr;_TE=_TD.tag;if(_TE!=2)goto _TLEB;_TF=ai;_T10=_TF.KnownAggr;_T11=_T10.val;{struct Cyc_Absyn_Aggrdecl*_T91=*_T11;_T90=_T91;}{struct Cyc_Absyn_Aggrdecl*ad=_T90;_T12=ad;{
# 601
struct _tuple0*_T91=_T12->name;struct _fat_ptr*_T92;union Cyc_Absyn_Nmspace _T93;{struct _tuple0 _T94=*_T91;_T93=_T94.f0;_T92=_T94.f1;}{union Cyc_Absyn_Nmspace ns=_T93;struct _fat_ptr*name=_T92;
# 604
struct Cyc_Absyn_Decl*ovd=Cyc_Absyn_lookup_decl(tv_ovrs,name);_T13=ovd;_T14=(unsigned)_T13;
if(!_T14)goto _TLED;_T15=ovd;{
# 608
void*_T94=_T15->r;struct Cyc_Absyn_Aggrdecl*_T95;_T16=(int*)_T94;_T17=*_T16;if(_T17!=5)goto _TLEF;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T96=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T94;_T95=_T96->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T95;
# 610
struct Cyc_List_List*removed_tvars=0;{struct _handler_cons _T96;_T18=& _T96;_push_handler(_T18);{int _T97=0;_T19=_T96.handler;_T1A=setjmp(_T19);if(!_T1A)goto _TLF1;_T97=1;goto _TLF2;_TLF1: _TLF2: if(_T97)goto _TLF3;else{goto _TLF5;}_TLF5: _T1B=ts;_T1C=*_T1B;
# 612
removed_tvars=Cyc_Cifc_extract_tvars(_T1C);_pop_handler();goto _TLF4;_TLF3: _T1D=Cyc_Core_get_exn_thrown();{void*_T98=(void*)_T1D;void*_T99;_T1E=(struct Cyc_Cifc_Contains_nontvar_exn_struct*)_T98;_T1F=_T1E->tag;_T20=Cyc_Cifc_Contains_nontvar;if(_T1F!=_T20)goto _TLF6;
# 617
return 0;_TLF6: _T99=_T98;{void*exn=_T99;_rethrow(exn);};}_TLF4:;}}_T21=ts;
# 619
*_T21=0;{
struct Cyc_List_List*added_tvars=0;_T22=ad;{
struct Cyc_List_List*tvs=_T22->tvs;
_TLF8: _T23=tvs;_T24=(unsigned)_T23;if(_T24)goto _TLF9;else{goto _TLFA;}
_TLF9: _T25=tvs;_T26=_T25->hd;{struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_T26;
if(enclosing_decl!=ovd)goto _TLFB;_T27=ts;{struct Cyc_List_List*_T96=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T97=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T97->tag=2;
_T97->f1=tv;_T29=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T97;}_T96->hd=(void*)_T29;_T2A=ts;_T96->tl=*_T2A;_T28=(struct Cyc_List_List*)_T96;}*_T27=_T28;goto _TLFC;
# 628
_TLFB: _T2B=instantiate;if(_T2B)goto _TLFF;else{goto _TL100;}_TL100: _T2C=Cyc_Cifc_opt_inst_tvar;if(_T2C)goto _TLFF;else{goto _TLFD;}
# 630
_TLFF: _T2D=ts;{struct Cyc_List_List*_T96=_cycalloc(sizeof(struct Cyc_List_List));_T2F=enclosing_decl;_T30=_T2F->loc;_T31=tv;_T96->hd=Cyc_Cifc_instantiate_tvar(_T30,_T31);_T32=ts;_T96->tl=*_T32;_T2E=(struct Cyc_List_List*)_T96;}*_T2D=_T2E;goto _TLFE;
# 633
_TLFD:{struct Cyc_Absyn_Tvar*tvcpy;tvcpy=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T33=tvcpy;_T34=tv;*_T33=*_T34;{
struct _fat_ptr*tvn;tvn=_cycalloc(sizeof(struct _fat_ptr));_T35=tvn;{struct Cyc_Int_pa_PrintArg_struct _T96;_T96.tag=1;Cyc_Cifc_glob_tvar_id=Cyc_Cifc_glob_tvar_id + 1;_T38=Cyc_Cifc_glob_tvar_id;_T96.f1=(unsigned long)_T38;_T37=_T96;}{struct Cyc_Int_pa_PrintArg_struct _T96=_T37;void*_T97[1];_T39=_T97 + 0;*_T39=& _T96;_T3A=_tag_fat("`ovr_%d",sizeof(char),8U);_T3B=_tag_fat(_T97,sizeof(void*),1);_T36=Cyc_aprintf(_T3A,_T3B);}*_T35=_T36;_T3C=tvcpy;
_T3C->name=tvn;{struct Cyc_List_List*_T96=_cycalloc(sizeof(struct Cyc_List_List));
_T96->hd=tvcpy;_T96->tl=added_tvars;_T3D=(struct Cyc_List_List*)_T96;}added_tvars=_T3D;_T3E=ts;{struct Cyc_List_List*_T96=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T97=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T97->tag=2;
_T97->f1=tvcpy;_T40=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T97;}_T96->hd=(void*)_T40;_T41=ts;_T96->tl=*_T41;_T3F=(struct Cyc_List_List*)_T96;}*_T3E=_T3F;}}_TLFE: _TLFC: _T42=tvs;
# 640
tvs=_T42->tl;}goto _TLF8;_TLFA: _T43=ts;_T44=ts;_T45=*_T44;
# 642
*_T43=Cyc_List_imp_rev(_T45);
if(enclosing_decl==ovd)goto _TL103;else{goto _TL105;}_TL105: _T47=Cyc_Cifc_opt_inst_tvar;if(_T47)goto _TL103;else{goto _TL104;}_TL104: _T48=instantiate;if(_T48)goto _TL103;else{goto _TL101;}_TL103: _T46=0;goto _TL102;_TL101:{struct _tuple13*_T96=_cycalloc(sizeof(struct _tuple13));_T96->f0=added_tvars;_T96->f1=removed_tvars;_T49=(struct _tuple13*)_T96;}_T46=_T49;_TL102: return _T46;}}}_TLEF: _T4B=Cyc_Warn_impos;{
# 645
int(*_T96)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T4B;_T4A=_T96;}_T4C=_tag_fat("ovd must be an aggr type",sizeof(char),25U);_T4D=_tag_fat(0U,sizeof(void*),0);_T4A(_T4C,_T4D);;}goto _TLEE;
# 649
_TLED: return 0;_TLEE:;}}}goto _TLEC;_TLEB: _T4E=ai;_T4F=_T4E.UnknownAggr;_T8F=_T4F.val;{struct _tuple2 ua=_T8F;
# 651
return 0;}_TLEC:;}goto _TLEA;_TLE9: goto _LLB;_TLEA: _LLB:;}
# 656
return 0;}case 4: _T50=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T50;_T51=t;_T52=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T51;_T53=& _T52->f1;_T8D=(struct Cyc_Absyn_PtrInfo*)_T53;}{struct Cyc_Absyn_PtrInfo*pi=_T8D;_T54=te;_T55=tv_ovrs;_T56=pi;_T57=_T56->elt_type;_T58=enclosing_decl;_T59=instantiate;_T5A=
# 658
Cyc_Cifc_update_tvars(_T54,_T55,_T57,_T58,_T59);return _T5A;}case 6: _T5B=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T8E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T5B;_T8B=_T8E->f1;}{struct Cyc_Absyn_FnInfo fi=_T8B;
# 660
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;_T5C=Cyc_Cifc_opt_inst_tvar;
if(!_T5C)goto _TL106;_T5D=te;_T5E=tv_ovrs;_T5F=fi;_T60=_T5F.ret_type;_T61=enclosing_decl;_T62=instantiate;
Cyc_Cifc_update_tvars(_T5D,_T5E,_T60,_T61,_T62);goto _TL107;_TL106: _TL107: _T63=fi;{
# 665
struct Cyc_List_List*argit=_T63.args;
_TL108: _T64=argit;_T65=(unsigned)_T64;if(_T65)goto _TL109;else{goto _TL10A;}
_TL109: _T66=argit;_T67=_T66->hd;{struct _tuple8*_T8E=(struct _tuple8*)_T67;void*_T8F;{struct _tuple8 _T90=*_T8E;_T8F=_T90.f2;}{void*at=_T8F;
struct _tuple13*ar_tup=Cyc_Cifc_update_tvars(te,tv_ovrs,at,enclosing_decl,instantiate);_T68=ar_tup;_T69=(unsigned)_T68;
if(!_T69)goto _TL10B;_T6A=ar_tup;_T6B=*_T6A;_T6C=_T6B.f0;_T6D=(unsigned)_T6C;
if(!_T6D)goto _TL10D;_T6E=ar_tup;_T6F=*_T6E;_T70=_T6F.f0;_T71=added_tvars;
added_tvars=Cyc_List_append(_T70,_T71);goto _TL10E;_TL10D: _TL10E: _T72=ar_tup;_T73=*_T72;_T74=_T73.f1;_T75=(unsigned)_T74;
if(!_T75)goto _TL10F;_T76=ar_tup;_T77=*_T76;_T78=_T77.f1;_T79=removed_tvars;
removed_tvars=Cyc_List_append(_T78,_T79);goto _TL110;_TL10F: _TL110: goto _TL10C;_TL10B: _TL10C: _T7A=argit;
# 675
argit=_T7A->tl;}}goto _TL108;_TL10A: _T7B=added_tvars;_T7C=(unsigned)_T7B;
# 677
if(_T7C)goto _TL113;else{goto _TL114;}_TL114: _T7D=removed_tvars;_T7E=(unsigned)_T7D;if(_T7E)goto _TL113;else{goto _TL111;}
_TL113:{struct _tuple13*_T8E=_cycalloc(sizeof(struct _tuple13));_T8E->f0=added_tvars;_T8E->f1=removed_tvars;_T7F=(struct _tuple13*)_T8E;}return _T7F;_TL111:
 return 0;}}case 5: _T80=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T8E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T80;_T81=t;_T82=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T81;_T83=& _T82->f1;_T8D=(struct Cyc_Absyn_ArrayInfo*)_T83;}{struct Cyc_Absyn_ArrayInfo*ai=(struct Cyc_Absyn_ArrayInfo*)_T8D;_T84=te;_T85=tv_ovrs;_T86=ai;_T87=_T86->elt_type;_T88=enclosing_decl;_T89=instantiate;_T8A=
# 681
Cyc_Cifc_update_tvars(_T84,_T85,_T87,_T88,_T89);return _T8A;}default:
# 683
 return 0;};}
# 685
return 0;}
# 688
static int Cyc_Cifc_update_fninfo_usage(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,struct Cyc_Absyn_FnInfo*fi,struct Cyc_Absyn_Decl*enclosing_decl){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_List_List*_T1;struct Cyc_Absyn_FnInfo*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;struct Cyc_Absyn_FnInfo*_T5;struct Cyc_List_List*_T6;struct _tuple13*_T7;unsigned _T8;struct _tuple13*_T9;struct _tuple13 _TA;struct Cyc_List_List*_TB;struct _tuple13*_TC;unsigned _TD;struct _tuple13*_TE;struct _tuple13 _TF;struct Cyc_List_List*_T10;unsigned _T11;struct Cyc_List_List*_T12;void*_T13;void**_T14;struct Cyc_Tcenv_Tenv*_T15;struct Cyc_List_List*_T16;void**_T17;void*_T18;struct Cyc_Absyn_Decl*_T19;struct _tuple13*_T1A;unsigned _T1B;struct _tuple13*_T1C;struct _tuple13 _T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct _tuple13*_T20;struct _tuple13 _T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;unsigned _T26;struct Cyc_List_List*_T27;void*_T28;struct _handler_cons*_T29;int*_T2A;int _T2B;struct Cyc_Absyn_FnInfo*_T2C;struct Cyc_List_List*(*_T2D)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*(*_T2E)(struct Cyc_List_List*,void*);struct Cyc_Absyn_FnInfo*_T2F;struct Cyc_List_List*_T30;struct Cyc_Absyn_Tvar*_T31;void*_T32;struct Cyc_Core_Not_found_exn_struct*_T33;char*_T34;char*_T35;struct Cyc_String_pa_PrintArg_struct _T36;void**_T37;unsigned _T38;struct _fat_ptr _T39;struct _fat_ptr _T3A;struct Cyc_List_List*_T3B;struct Cyc_List_List*_T3C;unsigned _T3D;struct Cyc_Absyn_FnInfo*_T3E;struct Cyc_List_List*_T3F;struct Cyc_Absyn_FnInfo*_T40;struct Cyc_List_List*_T41;struct Cyc_Absyn_FnInfo*_T42;int _T43;_T0=te;_T1=tv_ovrs;_T2=
_check_null(fi);_T3=_T2->ret_type;_T4=enclosing_decl;{struct _tuple13*ad=Cyc_Cifc_update_tvars(_T0,_T1,_T3,_T4,0);_T5=fi;{
# 691
struct Cyc_List_List*argit=_T5->args;_T7=ad;_T8=(unsigned)_T7;
if(!_T8)goto _TL115;_T9=ad;_TA=*_T9;_T6=_TA.f0;goto _TL116;_TL115: _T6=0;_TL116: {struct Cyc_List_List*added_tvars=_T6;_TC=ad;_TD=(unsigned)_TC;
if(!_TD)goto _TL117;_TE=ad;_TF=*_TE;_TB=_TF.f1;goto _TL118;_TL117: _TB=0;_TL118: {struct Cyc_List_List*removed_tvars=_TB;
int change=0;
_TL119: _T10=argit;_T11=(unsigned)_T10;if(_T11)goto _TL11A;else{goto _TL11B;}
_TL11A: _T12=argit;_T13=_T12->hd;{struct _tuple8*_T44=(struct _tuple8*)_T13;void*_T45;{struct _tuple8 _T46=*_T44;_T14=& _T44->f2;_T45=(void**)_T14;}{void**at=(void**)_T45;_T15=te;_T16=tv_ovrs;_T17=at;_T18=*_T17;_T19=enclosing_decl;{
struct _tuple13*ad=Cyc_Cifc_update_tvars(_T15,_T16,_T18,_T19,0);_T1A=ad;_T1B=(unsigned)_T1A;
if(!_T1B)goto _TL11C;_T1C=ad;_T1D=*_T1C;_T1E=_T1D.f0;_T1F=added_tvars;
added_tvars=Cyc_List_append(_T1E,_T1F);_T20=ad;_T21=*_T20;_T22=_T21.f1;_T23=removed_tvars;
removed_tvars=Cyc_List_append(_T22,_T23);goto _TL11D;_TL11C: _TL11D: _T24=argit;
# 702
argit=_T24->tl;}}}goto _TL119;_TL11B:
# 704
 _TL11E: _T25=removed_tvars;_T26=(unsigned)_T25;if(_T26)goto _TL11F;else{goto _TL120;}
_TL11F: change=1;_T27=removed_tvars;_T28=_T27->hd;{
struct Cyc_Absyn_Tvar*rtv=(struct Cyc_Absyn_Tvar*)_T28;{struct _handler_cons _T44;_T29=& _T44;_push_handler(_T29);{int _T45=0;_T2A=_T44.handler;_T2B=setjmp(_T2A);if(!_T2B)goto _TL121;_T45=1;goto _TL122;_TL121: _TL122: if(_T45)goto _TL123;else{goto _TL125;}_TL125: _T2C=fi;_T2E=Cyc_List_delete;{
# 708
struct Cyc_List_List*(*_T46)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T2E;_T2D=_T46;}_T2F=fi;_T30=_T2F->tvars;_T31=rtv;_T2C->tvars=_T2D(_T30,_T31);_pop_handler();goto _TL124;_TL123: _T32=Cyc_Core_get_exn_thrown();{void*_T46=(void*)_T32;void*_T47;_T33=(struct Cyc_Core_Not_found_exn_struct*)_T46;_T34=_T33->tag;_T35=Cyc_Core_Not_found;if(_T34!=_T35)goto _TL126;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;
# 712
_T48.f1=Cyc_Absynpp_tvar2string(rtv);_T36=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T36;void*_T49[1];_T37=_T49 + 0;*_T37=& _T48;_T38=loc;_T39=_tag_fat("Removed tvar %s not found",sizeof(char),26U);_T3A=_tag_fat(_T49,sizeof(void*),1);Cyc_Warn_warn(_T38,_T39,_T3A);}goto _LL3;_TL126: _T47=_T46;{void*exn=_T47;_rethrow(exn);}_LL3:;}_TL124:;}}_T3B=removed_tvars;
# 715
removed_tvars=_T3B->tl;}goto _TL11E;_TL120: _T3C=added_tvars;_T3D=(unsigned)_T3C;
# 717
if(!_T3D)goto _TL128;
change=1;_T3E=fi;_T3F=added_tvars;_T40=fi;_T41=_T40->tvars;
_T3E->tvars=Cyc_List_append(_T3F,_T41);goto _TL129;_TL128: _TL129: _T42=fi;
# 722
_T42->effect=0;_T43=change;
return _T43;}}}}}
# 726
static struct Cyc_List_List*Cyc_Cifc_remove_cur(struct Cyc_List_List*head,struct Cyc_List_List*cur,struct Cyc_List_List*prev){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;_T0=prev;_T1=(unsigned)_T0;
if(!_T1)goto _TL12A;_T2=prev;_T3=
_check_null(cur);_T2->tl=_T3->tl;goto _TL12B;
# 730
_TL12A: _T4=_check_null(cur);head=_T4->tl;_TL12B: _T5=head;
# 732
return _T5;}
# 735
static int Cyc_Cifc_decl_name_cmp(struct Cyc_Absyn_Decl*a,struct Cyc_Absyn_Decl*b){int _T0;int _T1;struct _fat_ptr*_T2;unsigned _T3;struct _fat_ptr*_T4;struct _fat_ptr _T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;int _T8;
struct _fat_ptr*na=Cyc_Absyn_decl_name(a);
struct _fat_ptr*nb=Cyc_Absyn_decl_name(b);_T0=na==0;_T1=nb==0;
if(!(_T0 ^ _T1))goto _TL12C;
return 1;_TL12C: _T2=na;_T3=(unsigned)_T2;
if(!_T3)goto _TL12E;_T4=na;_T5=*_T4;_T6=
_check_null(nb);_T7=*_T6;_T8=Cyc_strcmp(_T5,_T7);return _T8;_TL12E:
 return 1;}
# 746
static void Cyc_Cifc_deconstruct_type(void*t){struct Cyc_String_pa_PrintArg_struct _T0;void**_T1;struct Cyc___cycFILE*_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;void*_T5;int*_T6;unsigned _T7;void*_T8;void*_T9;struct Cyc___cycFILE*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc_List_List*_TD;unsigned _TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_List_List*_T11;struct Cyc___cycFILE*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;struct Cyc___cycFILE*_T15;struct _fat_ptr _T16;struct _fat_ptr _T17;struct Cyc___cycFILE*_T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;void*_T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct Cyc_Absyn_Tvar*_T1D;void*_T1E;void**_T1F;struct Cyc___cycFILE*_T20;struct _fat_ptr _T21;struct _fat_ptr _T22;void*_T23;void*_T24;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T25;struct Cyc_Absyn_PtrInfo*_T26;struct Cyc___cycFILE*_T27;struct _fat_ptr _T28;struct _fat_ptr _T29;struct Cyc_Absyn_PtrInfo*_T2A;void*_T2B;struct Cyc___cycFILE*_T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;void*_T2F;void*_T30;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T31;struct Cyc_Absyn_FnInfo*_T32;struct Cyc___cycFILE*_T33;struct _fat_ptr _T34;struct _fat_ptr _T35;struct Cyc_Absyn_FnInfo*_T36;struct Cyc_List_List*_T37;unsigned _T38;struct Cyc_String_pa_PrintArg_struct _T39;struct Cyc_List_List*_T3A;void*_T3B;struct Cyc_Absyn_Tvar*_T3C;void**_T3D;struct Cyc___cycFILE*_T3E;struct _fat_ptr _T3F;struct _fat_ptr _T40;struct Cyc_List_List*_T41;struct Cyc_Absyn_FnInfo*_T42;struct Cyc_List_List*_T43;unsigned _T44;struct Cyc_List_List*_T45;void*_T46;struct Cyc___cycFILE*_T47;struct _fat_ptr _T48;struct _fat_ptr _T49;struct Cyc_List_List*_T4A;struct Cyc___cycFILE*_T4B;struct _fat_ptr _T4C;struct _fat_ptr _T4D;void*_T4E;void*_T4F;struct Cyc___cycFILE*_T50;struct _fat_ptr _T51;struct _fat_ptr _T52;struct Cyc_List_List*_T53;unsigned _T54;struct Cyc_String_pa_PrintArg_struct _T55;struct Cyc_List_List*_T56;void*_T57;void**_T58;struct Cyc___cycFILE*_T59;struct _fat_ptr _T5A;struct _fat_ptr _T5B;struct Cyc_List_List*_T5C;void*_T5D;unsigned _T5E;struct Cyc_String_pa_PrintArg_struct _T5F;void**_T60;struct Cyc___cycFILE*_T61;struct _fat_ptr _T62;struct _fat_ptr _T63;struct Cyc___cycFILE*_T64;struct _fat_ptr _T65;struct _fat_ptr _T66;struct Cyc___cycFILE*_T67;struct _fat_ptr _T68;struct _fat_ptr _T69;struct Cyc___cycFILE*_T6A;struct _fat_ptr _T6B;struct _fat_ptr _T6C;{struct Cyc_String_pa_PrintArg_struct _T6D;_T6D.tag=0;
_T6D.f1=Cyc_Absynpp_typ2string(t);_T0=_T6D;}{struct Cyc_String_pa_PrintArg_struct _T6D=_T0;void*_T6E[1];_T1=_T6E + 0;*_T1=& _T6D;_T2=Cyc_stderr;_T3=_tag_fat("Type is %s\n",sizeof(char),12U);_T4=_tag_fat(_T6E,sizeof(void*),1);Cyc_fprintf(_T2,_T3,_T4);}{struct Cyc_Absyn_Typedefdecl*_T6D;struct _tuple0*_T6E;struct Cyc_List_List*_T6F;void*_T70;_T5=t;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 0: _T8=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_T9=_T71->f1;_T70=(void*)_T9;_T6F=_T71->f2;}{void*tc=_T70;struct Cyc_List_List*ts=_T6F;_TA=Cyc_stderr;_TB=
# 750
_tag_fat("Got AppType ... <\n",sizeof(char),19U);_TC=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TA,_TB,_TC);
# 752
_TL131: _TD=ts;_TE=(unsigned)_TD;if(_TE)goto _TL132;else{goto _TL133;}
_TL132: _TF=ts;_T10=_TF->hd;Cyc_Cifc_deconstruct_type(_T10);_T11=ts;
ts=_T11->tl;goto _TL131;_TL133: _T12=Cyc_stderr;_T13=
# 756
_tag_fat(">\n",sizeof(char),3U);_T14=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T12,_T13,_T14);goto _LL0;}case 3: _T15=Cyc_stderr;_T16=
# 758
_tag_fat("Got Cvar\n",sizeof(char),10U);_T17=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T15,_T16,_T17);goto _LL0;case 1: _T18=Cyc_stderr;_T19=
# 760
_tag_fat("Got Evar\n",sizeof(char),10U);_T1A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T18,_T19,_T1A);goto _LL0;case 2: _T1B=t;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T1B;_T70=_T71->f1;}{struct Cyc_Absyn_Tvar*tv=_T70;{struct Cyc_String_pa_PrintArg_struct _T71;_T71.tag=0;_T1D=tv;_T1E=_T1D->kind;
# 762
_T71.f1=Cyc_Absynpp_kindbound2string(_T1E);_T1C=_T71;}{struct Cyc_String_pa_PrintArg_struct _T71=_T1C;void*_T72[1];_T1F=_T72 + 0;*_T1F=& _T71;_T20=Cyc_stderr;_T21=_tag_fat("Got VarType -- kindbnd is %s\n",sizeof(char),30U);_T22=_tag_fat(_T72,sizeof(void*),1);Cyc_fprintf(_T20,_T21,_T22);}goto _LL0;}case 4: _T23=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T23;_T24=t;_T25=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T24;_T26=& _T25->f1;_T70=(struct Cyc_Absyn_PtrInfo*)_T26;}{struct Cyc_Absyn_PtrInfo*pi=_T70;_T27=Cyc_stderr;_T28=
# 764
_tag_fat("Got PointerType\n",sizeof(char),17U);_T29=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T27,_T28,_T29);_T2A=pi;_T2B=_T2A->elt_type;
Cyc_Cifc_deconstruct_type(_T2B);goto _LL0;}case 5: _T2C=Cyc_stderr;_T2D=
# 768
_tag_fat("Got ArrayType\n",sizeof(char),15U);_T2E=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2C,_T2D,_T2E);goto _LL0;case 6: _T2F=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T2F;_T30=t;_T31=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T30;_T32=& _T31->f1;_T70=(struct Cyc_Absyn_FnInfo*)_T32;}{struct Cyc_Absyn_FnInfo*fi=_T70;_T33=Cyc_stderr;_T34=
# 770
_tag_fat("Got FnType\n",sizeof(char),12U);_T35=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T33,_T34,_T35);_T36=fi;{
struct Cyc_List_List*tvit=_T36->tvars;
_TL134: _T37=tvit;_T38=(unsigned)_T37;if(_T38)goto _TL135;else{goto _TL136;}
_TL135:{struct Cyc_String_pa_PrintArg_struct _T71;_T71.tag=0;_T3A=tvit;_T3B=_T3A->hd;_T3C=(struct Cyc_Absyn_Tvar*)_T3B;_T71.f1=Cyc_Absynpp_tvar2string(_T3C);_T39=_T71;}{struct Cyc_String_pa_PrintArg_struct _T71=_T39;void*_T72[1];_T3D=_T72 + 0;*_T3D=& _T71;_T3E=Cyc_stderr;_T3F=_tag_fat("tvar(%s), ",sizeof(char),11U);_T40=_tag_fat(_T72,sizeof(void*),1);Cyc_fprintf(_T3E,_T3F,_T40);}_T41=tvit;
tvit=_T41->tl;goto _TL134;_TL136: _T42=fi;{
# 776
struct Cyc_List_List*argit=_T42->args;
_TL137: _T43=argit;_T44=(unsigned)_T43;if(_T44)goto _TL138;else{goto _TL139;}
_TL138: _T45=argit;_T46=_T45->hd;{struct _tuple8*_T71=(struct _tuple8*)_T46;void*_T72;{struct _tuple8 _T73=*_T71;_T72=_T73.f2;}{void*t=_T72;_T47=Cyc_stderr;_T48=
_tag_fat("Arg::",sizeof(char),6U);_T49=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T47,_T48,_T49);
Cyc_Cifc_deconstruct_type(t);_T4A=argit;
argit=_T4A->tl;}}goto _TL137;_TL139: goto _LL0;}}}case 7: _T4B=Cyc_stderr;_T4C=
# 785
_tag_fat("Got AnonAggrType\n",sizeof(char),18U);_T4D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4B,_T4C,_T4D);goto _LL0;case 8: _T4E=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T4E;_T6E=_T71->f1;_T6F=_T71->f2;_T6D=_T71->f3;_T4F=_T71->f4;_T70=(void*)_T4F;}{struct _tuple0*n=_T6E;struct Cyc_List_List*ts=_T6F;struct Cyc_Absyn_Typedefdecl*d=_T6D;void*topt=_T70;_T50=Cyc_stderr;_T51=
# 787
_tag_fat("Got TypedefType\n",sizeof(char),17U);_T52=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T50,_T51,_T52);
_TL13A: _T53=ts;_T54=(unsigned)_T53;if(_T54)goto _TL13B;else{goto _TL13C;}
_TL13B:{struct Cyc_String_pa_PrintArg_struct _T71;_T71.tag=0;_T56=ts;_T57=_T56->hd;_T71.f1=Cyc_Absynpp_typ2string(_T57);_T55=_T71;}{struct Cyc_String_pa_PrintArg_struct _T71=_T55;void*_T72[1];_T58=_T72 + 0;*_T58=& _T71;_T59=Cyc_stderr;_T5A=_tag_fat("Types(i) = %s\n",sizeof(char),15U);_T5B=_tag_fat(_T72,sizeof(void*),1);Cyc_fprintf(_T59,_T5A,_T5B);}_T5C=ts;
ts=_T5C->tl;goto _TL13A;_TL13C: _T5D=topt;_T5E=(unsigned)_T5D;
# 792
if(!_T5E)goto _TL13D;{struct Cyc_String_pa_PrintArg_struct _T71;_T71.tag=0;
_T71.f1=Cyc_Absynpp_typ2string(topt);_T5F=_T71;}{struct Cyc_String_pa_PrintArg_struct _T71=_T5F;void*_T72[1];_T60=_T72 + 0;*_T60=& _T71;_T61=Cyc_stderr;_T62=_tag_fat("topt = %s\n",sizeof(char),11U);_T63=_tag_fat(_T72,sizeof(void*),1);Cyc_fprintf(_T61,_T62,_T63);}goto _TL13E;_TL13D: _TL13E: goto _LL0;}case 9: _T64=Cyc_stderr;_T65=
# 797
_tag_fat("Got ValueofType\n",sizeof(char),17U);_T66=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T64,_T65,_T66);goto _LL0;case 10: _T67=Cyc_stderr;_T68=
# 799
_tag_fat("Got TypeDeclType\n",sizeof(char),18U);_T69=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T67,_T68,_T69);goto _LL0;default: _T6A=Cyc_stderr;_T6B=
# 801
_tag_fat("Got TypeofType\n",sizeof(char),16U);_T6C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6A,_T6B,_T6C);goto _LL0;}_LL0:;}}
# 805
static struct _tuple13*Cyc_Cifc_add_tvars_to_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Decl*encloser,void*t,struct Cyc_List_List*tv_ovrs){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;struct _tuple13*_T5;void*_T6;void*_T7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8;struct Cyc_Absyn_PtrInfo*_T9;struct Cyc_Tcenv_Tenv*_TA;struct Cyc_Absyn_Decl*_TB;struct Cyc_Absyn_PtrInfo*_TC;void*_TD;struct Cyc_List_List*_TE;struct _tuple13*_TF;void*_T10;void*_T11;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T12;struct Cyc_Absyn_FnInfo*_T13;struct Cyc_Absyn_FnInfo*_T14;struct Cyc_List_List*_T15;struct Cyc_Absyn_Decl*_T16;unsigned _T17;struct Cyc_Tcenv_Tenv*_T18;struct Cyc_List_List*_T19;struct Cyc_Absyn_FnInfo*_T1A;struct Cyc_Absyn_Decl*_T1B;int _T1C;struct _tuple13*_T1D;struct Cyc_Absyn_FnInfo*_T1E;void*_T1F;void*_T20;void*_T21;{struct Cyc_Absyn_Typedefdecl*_T22;struct _tuple0*_T23;int _T24;enum Cyc_Absyn_AggrKind _T25;struct Cyc_List_List*_T26;void*_T27;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T28=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T4=_T28->f1;_T27=(void*)_T4;_T26=_T28->f2;}{void*tc=_T27;struct Cyc_List_List*ts=_T26;_T5=
# 811
Cyc_Cifc_update_tvars(te,tv_ovrs,t,encloser,0);return _T5;}case 4: _T6=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T28=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6;_T7=t;_T8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T7;_T9=& _T8->f1;_T27=(struct Cyc_Absyn_PtrInfo*)_T9;}{struct Cyc_Absyn_PtrInfo*pi=_T27;_TA=te;_TB=encloser;_TC=pi;_TD=_TC->elt_type;_TE=tv_ovrs;_TF=
# 813
Cyc_Cifc_add_tvars_to_type(_TA,_TB,_TD,_TE);return _TF;}case 5: goto _LL0;case 6: _T10=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T28=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T10;_T11=t;_T12=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T11;_T13=& _T12->f1;_T27=(struct Cyc_Absyn_FnInfo*)_T13;}{struct Cyc_Absyn_FnInfo*fi=_T27;_T14=fi;_T15=_T14->tvars;{
# 817
struct Cyc_List_List*old_tvars=Cyc_List_copy(_T15);_T16=encloser;_T17=_T16->loc;_T18=te;_T19=tv_ovrs;_T1A=fi;_T1B=encloser;{
int changed=Cyc_Cifc_update_fninfo_usage(_T17,_T18,_T19,_T1A,_T1B);_T1C=changed;
if(!_T1C)goto _TL140;{struct _tuple13*_T28=_cycalloc(sizeof(struct _tuple13));_T1E=fi;
_T28->f0=_T1E->tvars;_T28->f1=old_tvars;_T1D=(struct _tuple13*)_T28;}return _T1D;_TL140: goto _LL0;}}}case 7: _T1F=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T28=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T1F;_T25=_T28->f1;_T24=_T28->f2;_T26=_T28->f3;}{enum Cyc_Absyn_AggrKind knd=_T25;int is_tuple=_T24;struct Cyc_List_List*flst=_T26;goto _LL0;}case 8: _T20=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T28=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T20;_T23=_T28->f1;_T26=_T28->f2;_T22=_T28->f3;_T21=_T28->f4;_T27=(void*)_T21;}{struct _tuple0*n=_T23;struct Cyc_List_List*ts=_T26;struct Cyc_Absyn_Typedefdecl*d=_T22;void*topt=_T27;goto _LL0;}case 10: goto _LL0;default:
# 838 "cifc.cyc"
 return 0;}_LL0:;}
# 840
return 0;}
# 843
static int Cyc_Cifc_update_typedef_decl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Decl*encloser,struct Cyc_List_List*tv_ovrs){struct Cyc_Absyn_Decl*_T0;int*_T1;int _T2;struct Cyc_Tcenv_Tenv*_T3;struct Cyc_Absyn_Decl*_T4;struct Cyc_Absyn_Typedefdecl*_T5;void*_T6;void*_T7;struct Cyc_List_List*_T8;struct _tuple13*_T9;unsigned _TA;struct _tuple13*_TB;struct Cyc_List_List*_TC;unsigned _TD;struct Cyc_Absyn_Typedefdecl*_TE;struct Cyc_List_List*(*_TF)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*(*_T10)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T11)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Typedefdecl*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_Absyn_Tvar*_T16;struct Cyc_List_List*_T17;struct Cyc_Absyn_Typedefdecl*_T18;struct Cyc_Absyn_Typedefdecl*_T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;int(*_T1C)(struct _fat_ptr,struct _fat_ptr);void*(*_T1D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T1E;struct _fat_ptr _T1F;int _T20;
# 845
int opt_inst_tvar_sav=Cyc_Cifc_opt_inst_tvar;
Cyc_Cifc_opt_inst_tvar=0;{
int changed=0;_T0=encloser;{
void*_T21=_T0->r;struct Cyc_Absyn_Typedefdecl*_T22;_T1=(int*)_T21;_T2=*_T1;if(_T2!=8)goto _TL142;{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T23=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T21;_T22=_T23->f1;}{struct Cyc_Absyn_Typedefdecl*td_decl=_T22;_T3=te;_T4=encloser;_T5=td_decl;_T6=_T5->defn;_T7=
# 850
_check_null(_T6);_T8=tv_ovrs;{struct _tuple13*new_old_tvars=Cyc_Cifc_add_tvars_to_type(_T3,_T4,_T7,_T8);_T9=new_old_tvars;_TA=(unsigned)_T9;
if(!_TA)goto _TL144;{struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;if(new_old_tvars==0)goto _TL146;_TB=new_old_tvars;{struct _tuple13 _T25=*_TB;_T24=_T25.f0;_T23=_T25.f1;}{struct Cyc_List_List*nt=_T24;struct Cyc_List_List*ot=_T23;
# 853
_TL148: _TC=ot;_TD=(unsigned)_TC;if(_TD)goto _TL149;else{goto _TL14A;}
_TL149: _TE=td_decl;_T10=Cyc_List_delete_cmp;{struct Cyc_List_List*(*_T25)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T10;_TF=_T25;}_T11=Cyc_Absyn_tvar_cmp;_T12=td_decl;_T13=_T12->tvs;_T14=ot;_T15=_T14->hd;_T16=(struct Cyc_Absyn_Tvar*)_T15;_TE->tvs=_TF(_T11,_T13,_T16);_T17=ot;
ot=_T17->tl;goto _TL148;_TL14A: _T18=td_decl;_T19=td_decl;_T1A=_T19->tvs;_T1B=nt;
# 857
_T18->tvs=Cyc_List_append(_T1A,_T1B);
changed=1;}goto _TL147;_TL146: _throw_match();_TL147:;}goto _TL145;_TL144: _TL145: goto _LL0;}}_TL142: _T1D=Cyc_Warn_impos;{
# 862
int(*_T23)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T1D;_T1C=_T23;}_T1E=_tag_fat("expect only a typedef_d",sizeof(char),24U);_T1F=_tag_fat(0U,sizeof(void*),0);_T1C(_T1E,_T1F);_LL0:;}
# 864
Cyc_Cifc_opt_inst_tvar=opt_inst_tvar_sav;_T20=changed;
return _T20;}}
# 873
static struct Cyc_List_List*Cyc_Cifc_update_usages(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*in_tv_ovrs,struct Cyc_List_List*dsmm){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct Cyc_Absyn_Vardecl*_T7;int*_T8;unsigned _T9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_TA;struct Cyc_Absyn_FnInfo*_TB;void*_TC;struct Cyc_Tcenv_Tenv*_TD;struct Cyc_List_List*_TE;struct Cyc_Absyn_Vardecl*_TF;void*_T10;struct Cyc_Absyn_Decl*_T11;struct _tuple13*_T12;struct Cyc_Absyn_PtrInfo _T13;struct Cyc_Absyn_ArrayInfo _T14;struct _tuple13*_T15;unsigned _T16;struct Cyc_Tcenv_Tenv*_T17;struct Cyc_List_List*_T18;struct Cyc_Absyn_Fndecl*_T19;struct Cyc_Absyn_FnInfo*_T1A;struct Cyc_Absyn_FnInfo*_T1B;struct Cyc_Absyn_Decl*_T1C;struct Cyc_Absyn_Aggrdecl*_T1D;struct Cyc_Absyn_AggrdeclImpl*_T1E;unsigned _T1F;struct Cyc_Absyn_Aggrdecl*_T20;struct Cyc_Absyn_AggrdeclImpl*_T21;struct Cyc_List_List*_T22;unsigned _T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc_Tcenv_Tenv*_T26;struct Cyc_List_List*_T27;struct Cyc_Absyn_Aggrfield*_T28;void*_T29;struct Cyc_Absyn_Decl*_T2A;struct _tuple13*_T2B;unsigned _T2C;struct _tuple13*_T2D;struct _tuple13 _T2E;struct Cyc_List_List*_T2F;unsigned _T30;struct _tuple13*_T31;struct _tuple13 _T32;struct Cyc_List_List*_T33;unsigned _T34;struct Cyc_List_List*_T35;struct Cyc_List_List*_T36;unsigned _T37;struct Cyc_List_List*_T38;void*_T39;struct _handler_cons*_T3A;int*_T3B;int _T3C;struct Cyc_Absyn_Aggrdecl*_T3D;struct Cyc_List_List*(*_T3E)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*(*_T3F)(struct Cyc_List_List*,void*);struct Cyc_Absyn_Aggrdecl*_T40;struct Cyc_List_List*_T41;struct Cyc_Absyn_Tvar*_T42;void*_T43;struct Cyc_Core_Not_found_exn_struct*_T44;char*_T45;char*_T46;struct Cyc_String_pa_PrintArg_struct _T47;void**_T48;unsigned _T49;struct _fat_ptr _T4A;struct _fat_ptr _T4B;struct Cyc_List_List*_T4C;struct Cyc_List_List*_T4D;unsigned _T4E;struct Cyc_Absyn_Aggrdecl*_T4F;struct Cyc_List_List*_T50;struct Cyc_Absyn_Aggrdecl*_T51;struct Cyc_List_List*_T52;struct _handler_cons*_T53;int*_T54;int _T55;struct Cyc_List_List*(*_T56)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_Absyn_Decl*);struct Cyc_List_List*(*_T57)(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_List_List*_T58;struct Cyc_Absyn_Decl*_T59;void*_T5A;struct Cyc_Core_Not_found_exn_struct*_T5B;char*_T5C;char*_T5D;struct Cyc_List_List*_T5E;int _T5F;struct Cyc_List_List*_T60;struct Cyc_Absyn_Aggrdecl*_T61;struct Cyc_Absyn_Decl*_T62;unsigned _T63;struct Cyc_Absyn_Decl*_T64;int*_T65;int _T66;struct Cyc_Absyn_Aggrdecl*_T67;struct Cyc_List_List*_T68;unsigned _T69;struct Cyc_List_List*_T6A;void*_T6B;struct Cyc_Absyn_Tvar*_T6C;struct Cyc_Absyn_Tvar*_T6D;struct _fat_ptr*_T6E;struct _fat_ptr _T6F;struct Cyc_Int_pa_PrintArg_struct _T70;int _T71;void**_T72;struct _fat_ptr _T73;struct _fat_ptr _T74;struct Cyc_Absyn_Tvar*_T75;struct Cyc_Absyn_Aggrdecl*_T76;struct Cyc_List_List*_T77;struct Cyc_Absyn_Aggrdecl*_T78;struct Cyc_List_List*_T79;struct Cyc_Absyn_Aggrdecl*_T7A;struct Cyc_Absyn_Aggrdecl*_T7B;struct Cyc_List_List*_T7C;int(*_T7D)(struct _fat_ptr,struct _fat_ptr);void*(*_T7E)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7F;struct _fat_ptr _T80;struct Cyc_List_List*_T81;int _T82;int _T83;unsigned _T84;struct Cyc_String_pa_PrintArg_struct _T85;void**_T86;struct Cyc___cycFILE*_T87;struct _fat_ptr _T88;struct _fat_ptr _T89;struct Cyc_Warn_String_Warn_Warg_struct _T8A;void**_T8B;unsigned _T8C;struct _fat_ptr _T8D;struct Cyc_List_List*_T8E;
unsigned niter=0U;
struct Cyc_List_List*abs_decls=0;
int some_change=0;
struct Cyc_List_List*tv_ovrs=in_tv_ovrs;
struct Cyc_List_List*dsm=Cyc_List_copy(dsmm);
struct Cyc_List_List*changed_decls=0;
# 881
_TL14B: changed_decls=0;{
struct Cyc_List_List*ds=dsm;
struct Cyc_List_List*prev=0;
some_change=0;
_TL14D: _T0=ds;_T1=(unsigned)_T0;if(_T1)goto _TL14E;else{goto _TL14F;}
_TL14E:{int changed=0;_T2=ds;_T3=_T2->hd;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_T3;
struct _fat_ptr*dname=Cyc_Absyn_decl_name(d);_T4=d;{
# 891
void*_T8F=_T4->r;struct Cyc_Absyn_Typedefdecl*_T90;struct Cyc_Absyn_Aggrdecl*_T91;struct Cyc_Absyn_Fndecl*_T92;struct Cyc_Absyn_Vardecl*_T93;_T5=(int*)_T8F;_T6=*_T5;switch(_T6){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T94=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T8F;_T93=_T94->f1;}{struct Cyc_Absyn_Vardecl*vd=_T93;_T7=vd;{
# 893
void*_T94=_T7->type;struct Cyc_List_List*_T95;void*_T96;_T8=(int*)_T94;_T9=*_T8;switch(_T9){case 6:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T97=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T94;_TA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T94;_TB=& _TA->f1;_T96=(struct Cyc_Absyn_FnInfo*)_TB;}{struct Cyc_Absyn_FnInfo*fi=_T96;
# 895
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,fi,d);goto _LLB;}case 0:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T97=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T94;_TC=_T97->f1;_T96=(void*)_TC;_T95=_T97->f2;}{void*tc=_T96;struct Cyc_List_List*ts=_T95;_TD=te;_TE=tv_ovrs;_TF=vd;_T10=_TF->type;_T11=d;_T12=
# 903
Cyc_Cifc_update_tvars(_TD,_TE,_T10,_T11,1);changed=_T12!=0;goto _LLB;}case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T97=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T94;_T13=_T97->f1;_T96=_T13.elt_type;}{void*et=_T96;_T96=et;goto _LL13;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T97=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T94;_T14=_T97->f1;_T96=_T14.elt_type;}_LL13: {void*et=_T96;_T15=
# 907
Cyc_Cifc_update_tvars(te,tv_ovrs,et,d,1);changed=_T15!=0;goto _LLB;}default: goto _LLB;}_LLB:;}goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T94=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T8F;_T92=_T94->f1;}{struct Cyc_Absyn_Fndecl*fd=_T92;_T16=loc;_T17=te;_T18=tv_ovrs;_T19=fd;_T1A=& _T19->i;_T1B=(struct Cyc_Absyn_FnInfo*)_T1A;_T1C=d;
# 914
changed=Cyc_Cifc_update_fninfo_usage(_T16,_T17,_T18,_T1B,_T1C);goto _LL0;}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T94=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T8F;_T91=_T94->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T91;_T1D=ad;_T1E=_T1D->impl;_T1F=(unsigned)_T1E;
# 920
if(!_T1F)goto _TL152;_T20=ad;_T21=_T20->impl;{
struct Cyc_List_List*fit=_T21->fields;
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
_TL154: _T22=fit;_T23=(unsigned)_T22;if(_T23)goto _TL155;else{goto _TL156;}
_TL155: _T24=fit;_T25=_T24->hd;{struct Cyc_Absyn_Aggrfield*fld=(struct Cyc_Absyn_Aggrfield*)_T25;_T26=te;_T27=tv_ovrs;_T28=fld;_T29=_T28->type;_T2A=d;{
struct _tuple13*tvtup=Cyc_Cifc_update_tvars(_T26,_T27,_T29,_T2A,0);_T2B=tvtup;_T2C=(unsigned)_T2B;
if(!_T2C)goto _TL157;
changed=1;_T2D=tvtup;_T2E=*_T2D;{
struct Cyc_List_List*ad=_T2E.f0;_T2F=ad;_T30=(unsigned)_T2F;
if(!_T30)goto _TL159;
added_tvars=Cyc_List_append(ad,added_tvars);goto _TL15A;_TL159: _TL15A: _T31=tvtup;_T32=*_T31;{
struct Cyc_List_List*rm=_T32.f1;_T33=rm;_T34=(unsigned)_T33;
if(!_T34)goto _TL15B;
removed_tvars=Cyc_List_append(rm,removed_tvars);goto _TL15C;_TL15B: _TL15C:;}}goto _TL158;_TL157: _TL158: _T35=fit;
# 936
fit=_T35->tl;}}goto _TL154;_TL156:
# 938
 _TL15D: _T36=removed_tvars;_T37=(unsigned)_T36;if(_T37)goto _TL15E;else{goto _TL15F;}
_TL15E: _T38=removed_tvars;_T39=_T38->hd;{struct Cyc_Absyn_Tvar*rtv=(struct Cyc_Absyn_Tvar*)_T39;{struct _handler_cons _T94;_T3A=& _T94;_push_handler(_T3A);{int _T95=0;_T3B=_T94.handler;_T3C=setjmp(_T3B);if(!_T3C)goto _TL160;_T95=1;goto _TL161;_TL160: _TL161: if(_T95)goto _TL162;else{goto _TL164;}_TL164: _T3D=ad;_T3F=Cyc_List_delete;{
# 941
struct Cyc_List_List*(*_T96)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T3F;_T3E=_T96;}_T40=ad;_T41=_T40->tvs;_T42=rtv;_T3D->tvs=_T3E(_T41,_T42);_pop_handler();goto _TL163;_TL162: _T43=Cyc_Core_get_exn_thrown();{void*_T96=(void*)_T43;void*_T97;_T44=(struct Cyc_Core_Not_found_exn_struct*)_T96;_T45=_T44->tag;_T46=Cyc_Core_Not_found;if(_T45!=_T46)goto _TL165;{struct Cyc_String_pa_PrintArg_struct _T98;_T98.tag=0;
# 945
_T98.f1=Cyc_Absynpp_tvar2string(rtv);_T47=_T98;}{struct Cyc_String_pa_PrintArg_struct _T98=_T47;void*_T99[1];_T48=_T99 + 0;*_T48=& _T98;_T49=loc;_T4A=_tag_fat("Removed tvar %s not found",sizeof(char),26U);_T4B=_tag_fat(_T99,sizeof(void*),1);Cyc_Warn_warn(_T49,_T4A,_T4B);}goto _LL16;_TL165: _T97=_T96;{void*exn=_T97;_rethrow(exn);}_LL16:;}_TL163:;}}_T4C=removed_tvars;
# 948
removed_tvars=_T4C->tl;}goto _TL15D;_TL15F: _T4D=added_tvars;_T4E=(unsigned)_T4D;
# 950
if(!_T4E)goto _TL167;_T4F=ad;_T50=added_tvars;_T51=ad;_T52=_T51->tvs;
_T4F->tvs=Cyc_List_append(_T50,_T52);{
struct Cyc_Absyn_Decl*absdecl=Cyc_Cifc_make_abstract_decl(d);{struct _handler_cons _T94;_T53=& _T94;_push_handler(_T53);{int _T95=0;_T54=_T94.handler;_T55=setjmp(_T54);if(!_T55)goto _TL169;_T95=1;goto _TL16A;_TL169: _TL16A: if(_T95)goto _TL16B;else{goto _TL16D;}_TL16D: _T57=Cyc_List_delete_cmp;{
# 954
struct Cyc_List_List*(*_T96)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_Absyn_Decl*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_Absyn_Decl*))_T57;_T56=_T96;}_T58=abs_decls;_T59=absdecl;abs_decls=_T56(Cyc_Cifc_decl_name_cmp,_T58,_T59);_pop_handler();goto _TL16C;_TL16B: _T5A=Cyc_Core_get_exn_thrown();{void*_T96=(void*)_T5A;void*_T97;_T5B=(struct Cyc_Core_Not_found_exn_struct*)_T96;_T5C=_T5B->tag;_T5D=Cyc_Core_Not_found;if(_T5C!=_T5D)goto _TL16E;goto _LL1B;_TL16E: _T97=_T96;{void*exn=_T97;_rethrow(exn);}_LL1B:;}_TL16C:;}}{struct Cyc_List_List*_T94=_cycalloc(sizeof(struct Cyc_List_List));
# 959
_T94->hd=absdecl;_T94->tl=abs_decls;_T5E=(struct Cyc_List_List*)_T94;}abs_decls=_T5E;}goto _TL168;_TL167: _TL168: _T5F=changed;
# 961
if(!_T5F)goto _TL170;{struct Cyc_List_List*_T94=_cycalloc(sizeof(struct Cyc_List_List));
_T94->hd=d;_T94->tl=changed_decls;_T60=(struct Cyc_List_List*)_T94;}changed_decls=_T60;goto _TL171;_TL170: _TL171:;}goto _TL153;
# 965
_TL152: _T61=ad;{struct _tuple0*_T94=_T61->name;struct _fat_ptr*_T95;{struct _tuple0 _T96=*_T94;_T95=_T96.f1;}{struct _fat_ptr*name=_T95;
struct Cyc_Absyn_Decl*ovd=Cyc_Absyn_lookup_decl(tv_ovrs,name);_T62=ovd;_T63=(unsigned)_T62;
if(!_T63)goto _TL172;
changed=1;_T64=ovd;{
void*_T96=_T64->r;struct Cyc_Absyn_Aggrdecl*_T97;_T65=(int*)_T96;_T66=*_T65;if(_T66!=5)goto _TL174;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T98=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T96;_T97=_T98->f1;}{struct Cyc_Absyn_Aggrdecl*ovd_ad=_T97;
# 971
struct Cyc_List_List*added_tvars=0;_T67=ovd_ad;{
struct Cyc_List_List*tvs=_T67->tvs;
_TL176: _T68=tvs;_T69=(unsigned)_T68;if(_T69)goto _TL177;else{goto _TL178;}
_TL177: _T6A=tvs;_T6B=_T6A->hd;{struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_T6B;
struct Cyc_Absyn_Tvar*tvcpy;tvcpy=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T6C=tvcpy;_T6D=tv;*_T6C=*_T6D;{
struct _fat_ptr*tvn;tvn=_cycalloc(sizeof(struct _fat_ptr));_T6E=tvn;{struct Cyc_Int_pa_PrintArg_struct _T98;_T98.tag=1;Cyc_Cifc_glob_tvar_id=Cyc_Cifc_glob_tvar_id + 1;_T71=Cyc_Cifc_glob_tvar_id;_T98.f1=(unsigned long)_T71;_T70=_T98;}{struct Cyc_Int_pa_PrintArg_struct _T98=_T70;void*_T99[1];_T72=_T99 + 0;*_T72=& _T98;_T73=_tag_fat("`ovr_%d",sizeof(char),8U);_T74=_tag_fat(_T99,sizeof(void*),1);_T6F=Cyc_aprintf(_T73,_T74);}*_T6E=_T6F;_T75=tvcpy;
_T75->name=tvn;_T76=ad;{struct Cyc_List_List*_T98=_cycalloc(sizeof(struct Cyc_List_List));
_T98->hd=tvcpy;_T78=ad;_T98->tl=_T78->tvs;_T77=(struct Cyc_List_List*)_T98;}_T76->tvs=_T77;_T79=tvs;
tvs=_T79->tl;}}goto _TL176;_TL178: _T7A=ad;_T7B=ad;_T7C=_T7B->tvs;
# 981
_T7A->tvs=Cyc_List_imp_rev(_T7C);goto _LL23;}}_TL174: _T7E=Cyc_Warn_impos;{
# 984
int(*_T98)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7E;_T7D=_T98;}_T7F=_tag_fat("ovd must be an aggr type",sizeof(char),25U);_T80=_tag_fat(0U,sizeof(void*),0);_T7D(_T7F,_T80);_LL23:;}goto _TL173;_TL172: _TL173:;}}_TL153: goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T94=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T8F;_T90=_T94->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T90;
# 1000 "cifc.cyc"
changed=Cyc_Cifc_update_typedef_decl(te,d,tv_ovrs);goto _LL0;}default: goto _LL0;}_LL0:;}
# 1005
prev=ds;_T81=ds;
ds=_T81->tl;_T82=changed;
if(!_T82)goto _TL179;
some_change=1;goto _TL17A;_TL179: _TL17A:;}}goto _TL14D;_TL14F:
# 1010
 tv_ovrs=changed_decls;}_T83=some_change;
# 880 "cifc.cyc"
if(_T83)goto _TL17B;else{goto _TL14C;}_TL17B:
# 1011 "cifc.cyc"
 niter=niter + 1;_T84=niter;
# 880 "cifc.cyc"
if(_T84 < 10U)goto _TL14B;else{goto _TL14C;}_TL14C:
# 1012 "cifc.cyc"
 if(niter < 10U)goto _TL17C;{struct Cyc_String_pa_PrintArg_struct _T8F;_T8F.tag=0;
# 1014
_T8F.f1=Cyc_Absynpp_decllist2string(changed_decls);_T85=_T8F;}{struct Cyc_String_pa_PrintArg_struct _T8F=_T85;void*_T90[1];_T86=_T90 + 0;*_T86=& _T8F;_T87=Cyc_stderr;_T88=
# 1013
_tag_fat("Suspected mutually recursive structs involving the following decls \n %s",sizeof(char),72U);_T89=_tag_fat(_T90,sizeof(void*),1);Cyc_fprintf(_T87,_T88,_T89);}{struct Cyc_Warn_String_Warn_Warg_struct _T8F;_T8F.tag=0;
# 1015
_T8F.f1=_tag_fat("Suspected mutually recursive structs -- abandoning cifc",sizeof(char),56U);_T8A=_T8F;}{struct Cyc_Warn_String_Warn_Warg_struct _T8F=_T8A;void*_T90[1];_T8B=_T90 + 0;*_T8B=& _T8F;_T8C=loc;_T8D=_tag_fat(_T90,sizeof(void*),1);Cyc_Warn_err2(_T8C,_T8D);}goto _TL17D;_TL17C: _TL17D: _T8E=abs_decls;
# 1017
return _T8E;}
# 1020
static void Cyc_Cifc_i_clear_vartype_ids(void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_Tvar*_T4;void*_T5;struct Cyc_List_List*_T6;unsigned _T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Absyn_PtrInfo _TC;void*_TD;void*_TE;struct Cyc_Absyn_ArrayInfo _TF;void*_T10;void*_T11;struct Cyc_Absyn_FnInfo _T12;void*_T13;struct Cyc_Absyn_FnInfo _T14;struct Cyc_List_List*_T15;unsigned _T16;struct Cyc_List_List*_T17;void*_T18;struct Cyc_List_List*_T19;void*_T1A;void*_T1B;struct Cyc_List_List*_T1C;unsigned _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_List_List*_T20;struct Cyc_Absyn_FnInfo _T21;struct Cyc_Absyn_ArrayInfo _T22;struct Cyc_Absyn_PtrInfo _T23;struct Cyc_List_List*_T24;void*_T25;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 2: _T3=t;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T3;_T25=_T26->f1;}{struct Cyc_Absyn_Tvar*tv=_T25;_T4=tv;
# 1023
_T4->identity=-1;goto _LL0;}case 0: _T5=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T5;_T24=_T26->f2;}{struct Cyc_List_List*ts=_T24;
# 1026
_TL17F: _T6=ts;_T7=(unsigned)_T6;if(_T7)goto _TL180;else{goto _TL181;}
_TL180: _T8=ts;_T9=_T8->hd;Cyc_Cifc_i_clear_vartype_ids(_T9);_TA=ts;
ts=_TA->tl;goto _TL17F;_TL181: goto _LL0;}case 4: _TB=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TB;_T23=_T26->f1;}{struct Cyc_Absyn_PtrInfo pi=_T23;_TC=pi;_TD=_TC.elt_type;
# 1032
Cyc_Cifc_i_clear_vartype_ids(_TD);goto _LL0;}case 5: _TE=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TE;_T22=_T26->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T22;_TF=ai;_T10=_TF.elt_type;
# 1035
Cyc_Cifc_i_clear_vartype_ids(_T10);goto _LL0;}case 6: _T11=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T11;_T21=_T26->f1;}{struct Cyc_Absyn_FnInfo fi=_T21;_T12=fi;_T13=_T12.ret_type;
# 1038
Cyc_Cifc_i_clear_vartype_ids(_T13);_T14=fi;{
struct Cyc_List_List*argit=_T14.args;
_TL182: _T15=argit;_T16=(unsigned)_T15;if(_T16)goto _TL183;else{goto _TL184;}
_TL183: _T17=argit;_T18=_T17->hd;{struct _tuple8*_T26=(struct _tuple8*)_T18;void*_T27;{struct _tuple8 _T28=*_T26;_T27=_T28.f2;}{void*at=_T27;
Cyc_Cifc_i_clear_vartype_ids(at);_T19=argit;
argit=_T19->tl;}}goto _TL182;_TL184: goto _LL0;}}case 8: _T1A=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T1A;_T24=_T26->f2;_T1B=_T26->f4;_T25=(void*)_T1B;}{struct Cyc_List_List*ts=_T24;void*to=_T25;
# 1050
_TL185: _T1C=ts;_T1D=(unsigned)_T1C;if(_T1D)goto _TL186;else{goto _TL187;}
_TL186: _T1E=ts;_T1F=_T1E->hd;Cyc_Cifc_i_clear_vartype_ids(_T1F);_T20=ts;
ts=_T20->tl;goto _TL185;_TL187: goto _LL0;}default: goto _LL0;}_LL0:;}
# 1063
static void Cyc_Cifc_clear_vartype_ids(struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Vardecl*_T3;void*_T4;struct Cyc_Absyn_Fndecl*_T5;struct Cyc_Absyn_FnInfo _T6;void*_T7;struct Cyc_Absyn_Fndecl*_T8;struct Cyc_Absyn_FnInfo _T9;struct Cyc_List_List*_TA;unsigned _TB;struct Cyc_List_List*_TC;void*_TD;struct _tuple8*_TE;struct _tuple8 _TF;void*_T10;struct Cyc_Absyn_Fndecl*_T11;_T0=d;{
void*_T12=_T0->r;struct Cyc_Absyn_Typedefdecl*_T13;struct Cyc_Absyn_Aggrdecl*_T14;struct Cyc_Absyn_Fndecl*_T15;struct Cyc_Absyn_Vardecl*_T16;_T1=(int*)_T12;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T17=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T12;_T16=_T17->f1;}{struct Cyc_Absyn_Vardecl*vd=_T16;_T3=vd;_T4=_T3->type;
# 1066
Cyc_Cifc_i_clear_vartype_ids(_T4);goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T17=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T12;_T15=_T17->f1;}{struct Cyc_Absyn_Fndecl*fd=_T15;_T5=fd;_T6=_T5->i;_T7=_T6.ret_type;
# 1069
Cyc_Cifc_i_clear_vartype_ids(_T7);_T8=fd;_T9=_T8->i;{
struct Cyc_List_List*ai=_T9.args;
_TL189: _TA=ai;_TB=(unsigned)_TA;if(_TB)goto _TL18A;else{goto _TL18B;}
_TL18A: _TC=ai;_TD=_TC->hd;_TE=(struct _tuple8*)_TD;_TF=*_TE;_T10=_TF.f2;Cyc_Cifc_i_clear_vartype_ids(_T10);goto _TL189;_TL18B: _T11=fd;
# 1074
_T11->i.tvars=0;goto _LL0;}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T17=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T12;_T14=_T17->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T14;goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T17=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T12;_T13=_T17->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T13;goto _LL0;}default: goto _LL0;}_LL0:;}}
# 1086
static int Cyc_Cifc_eq(void*a,void*b){int _T0;_T0=a==b;
return _T0;}
# 1090
static int Cyc_Cifc_decl_not_present(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*a){int(*_T0)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_Absyn_Decl*,struct Cyc_List_List*);int(*_T1)(int(*)(void*,void*),void*,struct Cyc_List_List*);int(*_T2)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*);struct Cyc_Absyn_Decl*_T3;struct Cyc_List_List*_T4;int _T5;int _T6;_T1=Cyc_List_exists_c;{
int(*_T7)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_Absyn_Decl*,struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_Absyn_Decl*,struct Cyc_List_List*))_T1;_T0=_T7;}{int(*_T7)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*)=(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*))Cyc_Cifc_eq;_T2=_T7;}_T3=a;_T4=l;_T5=_T0(_T2,_T3,_T4);_T6=!_T5;return _T6;}
# 1095
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List**ds,struct Cyc_List_List*ovrs){struct Cyc_List_List*_T0;void*_T1;struct _fat_ptr*_T2;unsigned _T3;struct Cyc_Absyn_Decl*_T4;unsigned _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_List_List**_T8;struct Cyc_List_List*_T9;struct _fat_ptr*_TA;struct Cyc_Absyn_Decl*_TB;unsigned _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct _fat_ptr*_TE;void**_TF;struct Cyc_Absyn_Decl*_T10;unsigned _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_List_List*_T14;int _T15;int _T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*(*_T1A)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*(*_T1B)(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T1C;struct Cyc_List_List**_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;unsigned _T22;struct Cyc_List_List**_T23;struct Cyc_List_List*_T24;struct Cyc_List_List**_T25;struct Cyc_List_List*_T26;
struct Cyc_List_List*type_overrides=0;
struct Cyc_List_List*overriden_decls=0;
struct Cyc_List_List*abs_decls=0;{
struct Cyc_List_List*x=ovrs;_TL18F: if(x!=0)goto _TL18D;else{goto _TL18E;}
_TL18D: _T0=x;_T1=_T0->hd;{struct Cyc_Absyn_Decl*ud=(struct Cyc_Absyn_Decl*)_T1;
struct _fat_ptr*un=Cyc_Absyn_decl_name(ud);_T2=un;_T3=(unsigned)_T2;
if(_T3)goto _TL190;else{goto _TL192;}_TL192: _T4=ud;_T5=_T4->loc;_T6=_tag_fat("Overriding decl without a name\n",sizeof(char),32U);_T7=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T5,_T6,_T7);goto _TL191;
# 1104
_TL190: _T8=_check_null(ds);_T9=*_T8;_TA=un;{struct Cyc_Absyn_Decl*d=Cyc_Cifc_lookup_concrete_decl(_T9,_TA);_TB=d;_TC=(unsigned)_TB;
if(_TC)goto _TL193;else{goto _TL195;}_TL195:{struct Cyc_String_pa_PrintArg_struct _T27;_T27.tag=0;_TE=un;_T27.f1=*_TE;_TD=_T27;}{struct Cyc_String_pa_PrintArg_struct _T27=_TD;void*_T28[1];_TF=_T28 + 0;*_TF=& _T27;_T10=ud;_T11=_T10->loc;_T12=_tag_fat("%s is overridden but not defined",sizeof(char),33U);_T13=_tag_fat(_T28,sizeof(void*),1);Cyc_Warn_warn(_T11,_T12,_T13);}goto _TL194;
# 1107
_TL193:{struct Cyc_List_List*_T27=_cycalloc(sizeof(struct Cyc_List_List));_T27->hd=d;_T27->tl=overriden_decls;_T14=(struct Cyc_List_List*)_T27;}overriden_decls=_T14;{
int pre_tvars_d=Cyc_Cifc_contains_type_vars(d);
Cyc_Cifc_merge_sys_user_decl(loc,te,0,ud,d);
Cyc_Cifc_clear_vartype_ids(ud);_T15=
Cyc_Cifc_contains_type_vars(ud);if(!_T15)goto _TL196;_T16=pre_tvars_d;if(_T16)goto _TL196;else{goto _TL198;}
_TL198:{struct Cyc_List_List*_T27=_cycalloc(sizeof(struct Cyc_List_List));_T27->hd=Cyc_Cifc_make_abstract_decl(ud);_T27->tl=abs_decls;_T17=(struct Cyc_List_List*)_T27;}abs_decls=_T17;{struct Cyc_List_List*_T27=_cycalloc(sizeof(struct Cyc_List_List));
_T27->hd=d;_T27->tl=type_overrides;_T18=(struct Cyc_List_List*)_T27;}type_overrides=_T18;goto _TL197;_TL196: _TL197:;}_TL194:;}_TL191:;}_T19=x;
# 1099
x=_T19->tl;goto _TL18F;_TL18E:;}_T1B=Cyc_List_filter_c;{
# 1119
struct Cyc_List_List*(*_T27)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))_T1B;_T1A=_T27;}_T1C=overriden_decls;_T1D=_check_null(ds);_T1E=*_T1D;{struct Cyc_List_List*unoverriden_decls=_T1A(Cyc_Cifc_decl_not_present,_T1C,_T1E);_T1F=abs_decls;_T20=
# 1121
Cyc_Cifc_update_usages(loc,te,type_overrides,unoverriden_decls);
# 1120
abs_decls=Cyc_List_append(_T1F,_T20);_T21=abs_decls;_T22=(unsigned)_T21;
# 1122
if(!_T22)goto _TL199;_T23=ds;_T24=abs_decls;_T25=ds;_T26=*_T25;
*_T23=Cyc_List_append(_T24,_T26);goto _TL19A;_TL199: _TL19A:;}}

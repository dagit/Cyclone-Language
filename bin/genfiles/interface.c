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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Core_Opt{void*v;};
# 119 "core.h"
void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 145
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 131 "dict.h"
extern void*Cyc_Dict_fold(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);
# 149
extern void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 927
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 933
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 939
void*Cyc_Absyn_compress(void*);
# 995
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 1163
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1168
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1229
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 68 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);struct Cyc_Interface_I;struct _tuple11{struct _fat_ptr f0;struct _fat_ptr f1;};
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 43 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*,int,struct Cyc_List_List*);
# 25 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);
# 39 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 77 "kinds.h"
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 38 "tcdecl.h"
void Cyc_Tcdecl_merr(unsigned,struct _fat_ptr*,struct _fat_ptr,struct _fat_ptr ap);
# 54 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*);
# 57
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*);
# 59
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*);
# 61
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*);
# 63
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*);
# 68
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*);
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);
# 39 "interface.cyc"
void Cyc_Lex_lex_init (void);struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};struct Cyc_Interface_I{struct Cyc_Interface_Ienv*imports;struct Cyc_Interface_Ienv*exports;struct Cyc_List_List*tds;};
# 70
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv (void){struct Cyc_Interface_Ienv*_T0;struct Cyc_Dict_Dict(*_T1)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T2)(int(*)(void*,void*));int(*_T3)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict(*_T4)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T5)(int(*)(void*,void*));int(*_T6)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict(*_T7)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T8)(int(*)(void*,void*));int(*_T9)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict(*_TA)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_TB)(int(*)(void*,void*));int(*_TC)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict(*_TD)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_TE)(int(*)(void*,void*));int(*_TF)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict(*_T10)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T11)(int(*)(void*,void*));int(*_T12)(struct _tuple0*,struct _tuple0*);{struct Cyc_Interface_Ienv*_T13=_cycalloc(sizeof(struct Cyc_Interface_Ienv));_T2=Cyc_Dict_empty;{
# 72
struct Cyc_Dict_Dict(*_T14)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T2;_T1=_T14;}_T3=Cyc_Absyn_qvar_cmp;_T13->aggrdecls=_T1(_T3);_T5=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T14)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T5;_T4=_T14;}_T6=Cyc_Absyn_qvar_cmp;_T13->datatypedecls=_T4(_T6);_T8=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T14)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T8;_T7=_T14;}_T9=Cyc_Absyn_qvar_cmp;_T13->enumdecls=_T7(_T9);_TB=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T14)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_TB;_TA=_T14;}_TC=Cyc_Absyn_qvar_cmp;_T13->typedefdecls=_TA(_TC);_TE=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T14)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_TE;_TD=_T14;}_TF=Cyc_Absyn_qvar_cmp;_T13->vardecls=_TD(_TF);_T11=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T14)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T11;_T10=_T14;}_T12=Cyc_Absyn_qvar_cmp;_T13->xdatatypefielddecls=_T10(_T12);_T0=(struct Cyc_Interface_Ienv*)_T13;}
# 71
return _T0;}
# 80
struct Cyc_Interface_I*Cyc_Interface_empty (void){struct Cyc_Interface_I*_T0;{struct Cyc_Interface_I*_T1=_cycalloc(sizeof(struct Cyc_Interface_I));
_T1->imports=Cyc_Interface_new_ienv();_T1->exports=Cyc_Interface_new_ienv();_T1->tds=0;_T0=(struct Cyc_Interface_I*)_T1;}return _T0;}
# 85
struct Cyc_Interface_I*Cyc_Interface_final (void){struct Cyc_Absyn_Datatypedecl*_T0;struct Cyc_Absyn_Datatypedecl*_T1;struct Cyc_Absyn_Datatypedecl*_T2;struct Cyc_Interface_I*_T3;struct Cyc_Interface_Ienv*_T4;struct Cyc_Dict_Dict(*_T5)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*);struct Cyc_Dict_Dict(*_T6)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_I*_T7;struct Cyc_Interface_Ienv*_T8;struct Cyc_Dict_Dict _T9;struct _tuple0*_TA;struct Cyc_Absyn_Datatypedecl*_TB;struct Cyc_Absyn_Datatypedecl*_TC;struct Cyc_Core_Opt*_TD;struct Cyc_Core_Opt*_TE;void*_TF;struct Cyc_Absyn_Datatypefield*_T10;struct Cyc_List_List*_T11;void*_T12;struct Cyc_Absyn_Datatypefield*_T13;struct Cyc_Absyn_Datatypefield*_T14;struct Cyc_Tcdecl_Xdatatypefielddecl*_T15;struct Cyc_Tcdecl_Xdatatypefielddecl*_T16;struct Cyc_Interface_I*_T17;struct Cyc_Interface_Ienv*_T18;struct Cyc_Dict_Dict(*_T19)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*);struct Cyc_Dict_Dict(*_T1A)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_I*_T1B;struct Cyc_Interface_Ienv*_T1C;struct Cyc_Dict_Dict _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_Absyn_Datatypefield*_T20;struct _tuple0*_T21;struct Cyc_Tcdecl_Xdatatypefielddecl*_T22;struct Cyc_List_List*_T23;struct Cyc_Interface_I*_T24;struct Cyc_Interface_I*_T25;struct Cyc_Interface_I*_T26;
struct Cyc_Interface_I*i=Cyc_Interface_empty();
# 88
struct Cyc_Absyn_Datatypedecl*exn_d;exn_d=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T0=exn_d;_T1=Cyc_Absyn_exn_tud();*_T0=*_T1;_T2=exn_d;
_T2->sc=2U;_T3=i;_T4=_T3->exports;_T6=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T27)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))_T6;_T5=_T27;}_T7=i;_T8=_T7->exports;_T9=_T8->datatypedecls;_TA=Cyc_Absyn_exn_name;_TB=exn_d;_T4->datatypedecls=_T5(_T9,_TA,_TB);_TC=
# 92
Cyc_Absyn_exn_tud();_TD=_TC->fields;_TE=_check_null(_TD);_TF=_TE->v;{struct Cyc_List_List*tufs=(struct Cyc_List_List*)_TF;_TL3: if(tufs!=0)goto _TL1;else{goto _TL2;}
_TL1:{struct Cyc_Absyn_Datatypefield*exn_f;exn_f=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T10=exn_f;_T11=tufs;_T12=_T11->hd;_T13=(struct Cyc_Absyn_Datatypefield*)_T12;*_T10=*_T13;_T14=exn_f;
_T14->sc=2U;{
struct Cyc_Tcdecl_Xdatatypefielddecl*exn_fd;exn_fd=_cycalloc(sizeof(struct Cyc_Tcdecl_Xdatatypefielddecl));_T15=exn_fd;_T15->base=exn_d;_T16=exn_fd;_T16->field=exn_f;_T17=i;_T18=_T17->exports;_T1A=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T27)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*))_T1A;_T19=_T27;}_T1B=i;_T1C=_T1B->exports;_T1D=_T1C->xdatatypefielddecls;_T1E=tufs;_T1F=_T1E->hd;_T20=(struct Cyc_Absyn_Datatypefield*)_T1F;_T21=_T20->name;_T22=exn_fd;_T18->xdatatypefielddecls=_T19(_T1D,_T21,_T22);}}_T23=tufs;
# 92
tufs=_T23->tl;goto _TL3;_TL2:;}_T24=i;_T25=i;
# 118 "interface.cyc"
_T24->imports=_T25->exports;_T26=i;
return _T26;}
# 138 "interface.cyc"
static void Cyc_Interface_err(struct _fat_ptr msg){struct _fat_ptr _T0;struct _fat_ptr _T1;_T0=msg;_T1=_tag_fat(0U,sizeof(void*),0);
Cyc_Warn_err(0U,_T0,_T1);}
# 141
static void*Cyc_Interface_invalid_arg(struct _fat_ptr s){struct Cyc_Core_Invalid_argument_exn_struct*_T0;void*_T1;{struct Cyc_Core_Invalid_argument_exn_struct*_T2=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_T2->tag=Cyc_Core_Invalid_argument;
_T2->f1=s;_T0=(struct Cyc_Core_Invalid_argument_exn_struct*)_T2;}_T1=(void*)_T0;_throw(_T1);}
# 144
static void Cyc_Interface_fields_err(struct _fat_ptr sc,struct _fat_ptr t,struct _tuple0*n){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_String_pa_PrintArg_struct _T3;void**_T4;void**_T5;void**_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;
# 146
_T9.f1=sc;_T1=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T1;{struct Cyc_String_pa_PrintArg_struct _TA;_TA.tag=0;_TA.f1=t;_T2=_TA;}{struct Cyc_String_pa_PrintArg_struct _TA=_T2;{struct Cyc_String_pa_PrintArg_struct _TB;_TB.tag=0;_TB.f1=Cyc_Absynpp_qvar2string(n);_T3=_TB;}{struct Cyc_String_pa_PrintArg_struct _TB=_T3;void*_TC[3];_T4=_TC + 0;*_T4=& _T9;_T5=_TC + 1;*_T5=& _TA;_T6=_TC + 2;*_T6=& _TB;_T7=
# 145
_tag_fat("fields of %s %s %s have never been defined",sizeof(char),43U);_T8=_tag_fat(_TC,sizeof(void*),3);_T0=Cyc_aprintf(_T7,_T8);}}}Cyc_Interface_err(_T0);}
# 148
static void Cyc_Interface_body_err(struct _fat_ptr sc,struct _tuple0*n){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct Cyc_String_pa_PrintArg_struct _T2;void**_T3;void**_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;{struct Cyc_String_pa_PrintArg_struct _T7;_T7.tag=0;
# 150
_T7.f1=sc;_T1=_T7;}{struct Cyc_String_pa_PrintArg_struct _T7=_T1;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;_T8.f1=Cyc_Absynpp_qvar2string(n);_T2=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T2;void*_T9[2];_T3=_T9 + 0;*_T3=& _T7;_T4=_T9 + 1;*_T4=& _T8;_T5=
# 149
_tag_fat("the body of %s function %s has never been defined",sizeof(char),50U);_T6=_tag_fat(_T9,sizeof(void*),2);_T0=Cyc_aprintf(_T5,_T6);}}Cyc_Interface_err(_T0);}
# 154
static void Cyc_Interface_static_err(struct _fat_ptr obj1,struct _tuple0*name1,struct _fat_ptr obj2,struct _tuple0*name2){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct _fat_ptr _T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct Cyc_String_pa_PrintArg_struct _T6;struct Cyc_String_pa_PrintArg_struct _T7;void**_T8;void**_T9;void**_TA;void**_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;_T0=obj1;_T1=_T0.curr;_T2=(char*)_T1;
if(_T2==0)goto _TL4;{struct Cyc_String_pa_PrintArg_struct _TE;_TE.tag=0;
# 157
_TE.f1=obj1;_T4=_TE;}{struct Cyc_String_pa_PrintArg_struct _TE=_T4;{struct Cyc_String_pa_PrintArg_struct _TF;_TF.tag=0;_TF.f1=Cyc_Absynpp_qvar2string(name1);_T5=_TF;}{struct Cyc_String_pa_PrintArg_struct _TF=_T5;{struct Cyc_String_pa_PrintArg_struct _T10;_T10.tag=0;
_T10.f1=obj2;_T6=_T10;}{struct Cyc_String_pa_PrintArg_struct _T10=_T6;{struct Cyc_String_pa_PrintArg_struct _T11;_T11.tag=0;_T11.f1=Cyc_Absynpp_qvar2string(name2);_T7=_T11;}{struct Cyc_String_pa_PrintArg_struct _T11=_T7;void*_T12[4];_T8=_T12 + 0;*_T8=& _TE;_T9=_T12 + 1;*_T9=& _TF;_TA=_T12 + 2;*_TA=& _T10;_TB=_T12 + 3;*_TB=& _T11;_TC=
# 156
_tag_fat("declaration of %s %s relies on static %s %s",sizeof(char),44U);_TD=_tag_fat(_T12,sizeof(void*),4);_T3=Cyc_aprintf(_TC,_TD);}}}}Cyc_Interface_err(_T3);goto _TL5;_TL4: _TL5:;}
# 160
static void Cyc_Interface_abstract_err(struct _fat_ptr obj1,struct _tuple0*name1,struct _fat_ptr obj2,struct _tuple0*name2){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct _fat_ptr _T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct Cyc_String_pa_PrintArg_struct _T6;struct Cyc_String_pa_PrintArg_struct _T7;void**_T8;void**_T9;void**_TA;void**_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;_T0=obj1;_T1=_T0.curr;_T2=(char*)_T1;
if(_T2==0)goto _TL6;{struct Cyc_String_pa_PrintArg_struct _TE;_TE.tag=0;
# 163
_TE.f1=obj1;_T4=_TE;}{struct Cyc_String_pa_PrintArg_struct _TE=_T4;{struct Cyc_String_pa_PrintArg_struct _TF;_TF.tag=0;_TF.f1=Cyc_Absynpp_qvar2string(name1);_T5=_TF;}{struct Cyc_String_pa_PrintArg_struct _TF=_T5;{struct Cyc_String_pa_PrintArg_struct _T10;_T10.tag=0;
_T10.f1=obj2;_T6=_T10;}{struct Cyc_String_pa_PrintArg_struct _T10=_T6;{struct Cyc_String_pa_PrintArg_struct _T11;_T11.tag=0;_T11.f1=Cyc_Absynpp_qvar2string(name2);_T7=_T11;}{struct Cyc_String_pa_PrintArg_struct _T11=_T7;void*_T12[4];_T8=_T12 + 0;*_T8=& _TE;_T9=_T12 + 1;*_T9=& _TF;_TA=_T12 + 2;*_TA=& _T10;_TB=_T12 + 3;*_TB=& _T11;_TC=
# 162
_tag_fat("declaration of %s %s relies on fields of abstract %s %s",sizeof(char),56U);_TD=_tag_fat(_T12,sizeof(void*),4);_T3=Cyc_aprintf(_TC,_TD);}}}}Cyc_Interface_err(_T3);goto _TL7;_TL6: _TL7:;}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 176
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen (void){struct Cyc_Interface_Seen*_T0;struct Cyc_Dict_Dict(*_T1)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T2)(int(*)(void*,void*));int(*_T3)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict(*_T4)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T5)(int(*)(void*,void*));int(*_T6)(struct _tuple0*,struct _tuple0*);{struct Cyc_Interface_Seen*_T7=_cycalloc(sizeof(struct Cyc_Interface_Seen));_T2=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T8)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T2;_T1=_T8;}_T3=Cyc_Absyn_qvar_cmp;_T7->aggrs=_T1(_T3);_T5=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T8)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T5;_T4=_T8;}_T6=Cyc_Absyn_qvar_cmp;_T7->datatypes=_T4(_T6);_T0=(struct Cyc_Interface_Seen*)_T7;}return _T0;}
# 181
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*);
# 183
static int Cyc_Interface_check_public_type_list(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _fat_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){struct Cyc_Tcenv_Genv*_T0;struct Cyc_Interface_Seen*_T1;struct _fat_ptr _T2;struct _tuple0*_T3;struct Cyc_List_List*_T4;void*_T5;void*_T6;int _T7;struct Cyc_List_List*_T8;int _T9;
# 185
int res=1;
_TLB: if(l!=0)goto _TL9;else{goto _TLA;}
_TL9: _T0=ae;_T1=seen;_T2=obj;_T3=name;_T4=l;_T5=_T4->hd;_T6=f(_T5);_T7=Cyc_Interface_check_public_type(_T0,_T1,_T2,_T3,_T6);if(_T7)goto _TLC;else{goto _TLE;}
_TLE: res=0;goto _TLD;_TLC: _TLD: _T8=l;
# 186
l=_T8->tl;goto _TLB;_TLA: _T9=res;
# 189
return _T9;}
# 192
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){struct _handler_cons*_T0;int*_T1;int _T2;int(*_T3)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T4)(struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Seen*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_Absyn_Aggrdecl*_T7;struct _tuple0*_T8;void*_T9;struct Cyc_Dict_Absent_exn_struct*_TA;char*_TB;char*_TC;struct Cyc_Interface_Seen*_TD;struct Cyc_Dict_Dict(*_TE)(struct Cyc_Dict_Dict,struct _tuple0*,int);struct Cyc_Dict_Dict(*_TF)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Seen*_T10;struct Cyc_Dict_Dict _T11;struct Cyc_Absyn_Aggrdecl*_T12;struct _tuple0*_T13;int _T14;struct Cyc_Absyn_Aggrdecl*_T15;struct Cyc_Absyn_AggrdeclImpl*_T16;struct Cyc_Absyn_Aggrdecl*_T17;struct Cyc_Absyn_AggrdeclImpl*_T18;struct Cyc_Tcenv_Genv*_T19;struct Cyc_Interface_Seen*_T1A;struct _fat_ptr _T1B;struct Cyc_Absyn_Aggrdecl*_T1C;struct _tuple0*_T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_Absyn_Aggrfield*_T20;void*_T21;int _T22;struct Cyc_List_List*_T23;struct Cyc_Interface_Seen*_T24;struct Cyc_Dict_Dict(*_T25)(struct Cyc_Dict_Dict,struct _tuple0*,int);struct Cyc_Dict_Dict(*_T26)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Seen*_T27;struct Cyc_Dict_Dict _T28;struct Cyc_Absyn_Aggrdecl*_T29;struct _tuple0*_T2A;int _T2B;int _T2C;{struct _handler_cons _T2D;_T0=& _T2D;_push_handler(_T0);{int _T2E=0;_T1=_T2D.handler;_T2=setjmp(_T1);if(!_T2)goto _TLF;_T2E=1;goto _TL10;_TLF: _TL10: if(_T2E)goto _TL11;else{goto _TL13;}_TL13: _T4=Cyc_Dict_lookup;{
int(*_T2F)(struct Cyc_Dict_Dict,struct _tuple0*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T4;_T3=_T2F;}_T5=seen;_T6=_T5->aggrs;_T7=d;_T8=_T7->name;{int _T2F=_T3(_T6,_T8);_npop_handler(0);return _T2F;}_pop_handler();goto _TL12;_TL11: _T9=Cyc_Core_get_exn_thrown();{void*_T2F=(void*)_T9;void*_T30;_TA=(struct Cyc_Dict_Absent_exn_struct*)_T2F;_TB=_TA->tag;_TC=Cyc_Dict_Absent;if(_TB!=_TC)goto _TL14;goto _LL0;_TL14: _T30=_T2F;{void*exn=_T30;_rethrow(exn);}_LL0:;}_TL12:;}}{
# 195
int res=1;_TD=seen;_TF=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T2D)(struct Cyc_Dict_Dict,struct _tuple0*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))_TF;_TE=_T2D;}_T10=seen;_T11=_T10->aggrs;_T12=d;_T13=_T12->name;_T14=res;_TD->aggrs=_TE(_T11,_T13,_T14);_T15=d;_T16=_T15->impl;
if(_T16==0)goto _TL16;_T17=d;_T18=_T17->impl;{
struct Cyc_List_List*fs=_T18->fields;_TL1B: if(fs!=0)goto _TL19;else{goto _TL1A;}
_TL19: _T19=ae;_T1A=seen;_T1B=_tag_fat("type",sizeof(char),5U);_T1C=d;_T1D=_T1C->name;_T1E=fs;_T1F=_T1E->hd;_T20=(struct Cyc_Absyn_Aggrfield*)_T1F;_T21=_T20->type;_T22=Cyc_Interface_check_public_type(_T19,_T1A,_T1B,_T1D,_T21);if(_T22)goto _TL1C;else{goto _TL1E;}
_TL1E: res=0;goto _TL1D;_TL1C: _TL1D: _T23=fs;
# 198
fs=_T23->tl;goto _TL1B;_TL1A:;}goto _TL17;_TL16: _TL17: _T24=seen;_T26=Cyc_Dict_insert;{
# 202
struct Cyc_Dict_Dict(*_T2D)(struct Cyc_Dict_Dict,struct _tuple0*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))_T26;_T25=_T2D;}_T27=seen;_T28=_T27->aggrs;_T29=d;_T2A=_T29->name;_T2B=res;_T24->aggrs=_T25(_T28,_T2A,_T2B);_T2C=res;
return _T2C;}}struct _tuple12{struct Cyc_Absyn_Tqual f0;void*f1;};
# 206
static void*Cyc_Interface_get_type1(struct _tuple12*x){struct _tuple12*_T0;struct _tuple12 _T1;void*_T2;_T0=x;_T1=*_T0;_T2=_T1.f1;
return _T2;}
# 209
static void*Cyc_Interface_get_type2(struct _tuple8*x){struct _tuple8*_T0;struct _tuple8 _T1;void*_T2;_T0=x;_T1=*_T0;_T2=_T1.f2;
return _T2;}
# 213
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){struct _handler_cons*_T0;int*_T1;int _T2;int(*_T3)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T4)(struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Seen*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_Absyn_Datatypedecl*_T7;struct _tuple0*_T8;void*_T9;struct Cyc_Dict_Absent_exn_struct*_TA;char*_TB;char*_TC;struct Cyc_Interface_Seen*_TD;struct Cyc_Dict_Dict(*_TE)(struct Cyc_Dict_Dict,struct _tuple0*,int);struct Cyc_Dict_Dict(*_TF)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Seen*_T10;struct Cyc_Dict_Dict _T11;struct Cyc_Absyn_Datatypedecl*_T12;struct _tuple0*_T13;int _T14;struct Cyc_Absyn_Datatypedecl*_T15;struct Cyc_Core_Opt*_T16;struct Cyc_Absyn_Datatypedecl*_T17;struct Cyc_Core_Opt*_T18;void*_T19;int(*_T1A)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_Tcenv_Genv*_T1B;struct Cyc_Interface_Seen*_T1C;struct _fat_ptr _T1D;struct Cyc_Absyn_Datatypedecl*_T1E;struct _tuple0*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_Absyn_Datatypefield*_T22;struct Cyc_List_List*_T23;int _T24;struct Cyc_List_List*_T25;struct Cyc_Interface_Seen*_T26;struct Cyc_Dict_Dict(*_T27)(struct Cyc_Dict_Dict,struct _tuple0*,int);struct Cyc_Dict_Dict(*_T28)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Seen*_T29;struct Cyc_Dict_Dict _T2A;struct Cyc_Absyn_Datatypedecl*_T2B;struct _tuple0*_T2C;int _T2D;int _T2E;{struct _handler_cons _T2F;_T0=& _T2F;_push_handler(_T0);{int _T30=0;_T1=_T2F.handler;_T2=setjmp(_T1);if(!_T2)goto _TL1F;_T30=1;goto _TL20;_TL1F: _TL20: if(_T30)goto _TL21;else{goto _TL23;}_TL23: _T4=Cyc_Dict_lookup;{
int(*_T31)(struct Cyc_Dict_Dict,struct _tuple0*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T4;_T3=_T31;}_T5=seen;_T6=_T5->datatypes;_T7=d;_T8=_T7->name;{int _T31=_T3(_T6,_T8);_npop_handler(0);return _T31;}_pop_handler();goto _TL22;_TL21: _T9=Cyc_Core_get_exn_thrown();{void*_T31=(void*)_T9;void*_T32;_TA=(struct Cyc_Dict_Absent_exn_struct*)_T31;_TB=_TA->tag;_TC=Cyc_Dict_Absent;if(_TB!=_TC)goto _TL24;goto _LL0;_TL24: _T32=_T31;{void*exn=_T32;_rethrow(exn);}_LL0:;}_TL22:;}}{
# 216
int res=1;_TD=seen;_TF=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T2F)(struct Cyc_Dict_Dict,struct _tuple0*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))_TF;_TE=_T2F;}_T10=seen;_T11=_T10->datatypes;_T12=d;_T13=_T12->name;_T14=res;_TD->datatypes=_TE(_T11,_T13,_T14);_T15=d;_T16=_T15->fields;
if(_T16==0)goto _TL26;_T17=d;_T18=_T17->fields;_T19=_T18->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T19;_TL2B: if(fs!=0)goto _TL29;else{goto _TL2A;}
_TL29:{int(*_T2F)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple12*),struct Cyc_List_List*)=(int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list;_T1A=_T2F;}_T1B=ae;_T1C=seen;_T1D=_tag_fat("datatype",sizeof(char),9U);_T1E=d;_T1F=_T1E->name;_T20=fs;_T21=_T20->hd;_T22=(struct Cyc_Absyn_Datatypefield*)_T21;_T23=_T22->typs;_T24=_T1A(_T1B,_T1C,_T1D,_T1F,Cyc_Interface_get_type1,_T23);if(_T24)goto _TL2C;else{goto _TL2E;}
# 222
_TL2E: res=0;goto _TL2D;_TL2C: _TL2D: _T25=fs;
# 219
fs=_T25->tl;goto _TL2B;_TL2A:;}goto _TL27;_TL26: _TL27: _T26=seen;_T28=Cyc_Dict_insert;{
# 224
struct Cyc_Dict_Dict(*_T2F)(struct Cyc_Dict_Dict,struct _tuple0*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))_T28;_T27=_T2F;}_T29=seen;_T2A=_T29->datatypes;_T2B=d;_T2C=_T2B->name;_T2D=res;_T26->datatypes=_T27(_T2A,_T2C,_T2D);_T2E=res;
return _T2E;}}
# 228
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 232
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){struct Cyc_Absyn_Typedefdecl*_T0;void*_T1;struct Cyc_Tcenv_Genv*_T2;struct Cyc_Interface_Seen*_T3;struct _fat_ptr _T4;struct Cyc_Absyn_Typedefdecl*_T5;struct _tuple0*_T6;struct Cyc_Absyn_Typedefdecl*_T7;void*_T8;int _T9;_T0=d;_T1=_T0->defn;
if(_T1==0)goto _TL2F;_T2=ae;_T3=seen;_T4=
_tag_fat(0,0,0);_T5=d;_T6=_T5->name;_T7=d;_T8=_T7->defn;_T9=Cyc_Interface_check_public_type(_T2,_T3,_T4,_T6,_T8);return _T9;_TL2F:
 return 1;}
# 239
static int Cyc_Interface_check_public_vardecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){struct Cyc_Tcenv_Genv*_T0;struct Cyc_Interface_Seen*_T1;struct _fat_ptr _T2;struct Cyc_Absyn_Vardecl*_T3;struct _tuple0*_T4;struct Cyc_Absyn_Vardecl*_T5;void*_T6;int _T7;_T0=ae;_T1=seen;_T2=
_tag_fat("variable",sizeof(char),9U);_T3=d;_T4=_T3->name;_T5=d;_T6=_T5->type;_T7=Cyc_Interface_check_public_type(_T0,_T1,_T2,_T4,_T6);return _T7;}
# 243
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _fat_ptr obj,struct _tuple0*name,void*t){int*_T0;unsigned _T1;struct Cyc_Absyn_PtrInfo _T2;struct Cyc_Absyn_ArrayInfo _T3;void*_T4;void*_T5;int _T6;struct Cyc_Absyn_FnInfo _T7;struct Cyc_Absyn_FnInfo _T8;struct Cyc_Absyn_FnInfo _T9;int _TA;int(*_TB)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple8*),struct Cyc_List_List*);struct Cyc_Tcenv_Genv*_TC;struct Cyc_Interface_Seen*_TD;struct _fat_ptr _TE;struct _tuple0*_TF;struct Cyc_List_List*_T10;int _T11;struct Cyc_Absyn_VarargInfo*_T12;int _T13;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T14;void*_T15;int*_T16;unsigned _T17;void*_T18;struct Cyc_Absyn_Aggrdecl*_T19;enum Cyc_Absyn_Scope _T1A;int _T1B;struct _fat_ptr _T1C;struct _tuple0*_T1D;struct _fat_ptr _T1E;struct Cyc_Absyn_Aggrdecl*_T1F;struct _tuple0*_T20;int _T21;struct Cyc_Tcenv_Genv*_T22;struct Cyc_Interface_Seen*_T23;struct _fat_ptr _T24;struct _tuple0*_T25;void*(*_T26)(void*);struct Cyc_List_List*_T27;int _T28;void*_T29;struct _handler_cons*_T2A;int*_T2B;int _T2C;struct Cyc_Absyn_Enumdecl**(*_T2D)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T2E)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Genv*_T2F;struct Cyc_Dict_Dict _T30;struct _tuple0*_T31;struct Cyc_Absyn_Enumdecl**_T32;void*_T33;struct Cyc_Dict_Absent_exn_struct*_T34;char*_T35;char*_T36;int(*_T37)(struct _fat_ptr);struct _fat_ptr _T38;struct Cyc_String_pa_PrintArg_struct _T39;void**_T3A;struct _fat_ptr _T3B;struct _fat_ptr _T3C;struct Cyc_Absyn_Enumdecl*_T3D;enum Cyc_Absyn_Scope _T3E;int _T3F;struct _fat_ptr _T40;struct _tuple0*_T41;struct _fat_ptr _T42;struct Cyc_Absyn_Enumdecl*_T43;struct _tuple0*_T44;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T45;void*_T46;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T47;union Cyc_Absyn_DatatypeInfo _T48;struct _union_DatatypeInfo_KnownDatatype _T49;unsigned _T4A;void*_T4B;union Cyc_Absyn_DatatypeInfo _T4C;struct _union_DatatypeInfo_KnownDatatype _T4D;struct Cyc_Absyn_Datatypedecl**_T4E;struct _handler_cons*_T4F;int*_T50;int _T51;struct Cyc_Absyn_Datatypedecl**(*_T52)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T53)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Genv*_T54;struct Cyc_Dict_Dict _T55;struct Cyc_Absyn_Datatypedecl*_T56;struct _tuple0*_T57;struct Cyc_Absyn_Datatypedecl**_T58;void*_T59;struct Cyc_Dict_Absent_exn_struct*_T5A;char*_T5B;char*_T5C;int(*_T5D)(struct _fat_ptr);struct _fat_ptr _T5E;struct Cyc_String_pa_PrintArg_struct _T5F;struct Cyc_Absyn_Datatypedecl*_T60;struct _tuple0*_T61;void**_T62;struct _fat_ptr _T63;struct _fat_ptr _T64;struct Cyc_Absyn_Datatypedecl*_T65;enum Cyc_Absyn_Scope _T66;int _T67;struct _fat_ptr _T68;struct _tuple0*_T69;struct _fat_ptr _T6A;struct Cyc_Absyn_Datatypedecl*_T6B;struct _tuple0*_T6C;int _T6D;struct Cyc_Tcenv_Genv*_T6E;struct Cyc_Interface_Seen*_T6F;struct _fat_ptr _T70;struct _tuple0*_T71;void*(*_T72)(void*);struct Cyc_List_List*_T73;int _T74;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T75;void*_T76;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T77;union Cyc_Absyn_DatatypeFieldInfo _T78;struct _union_DatatypeFieldInfo_KnownDatatypefield _T79;unsigned _T7A;void*_T7B;union Cyc_Absyn_DatatypeFieldInfo _T7C;struct _union_DatatypeFieldInfo_KnownDatatypefield _T7D;struct _tuple1 _T7E;union Cyc_Absyn_DatatypeFieldInfo _T7F;struct _union_DatatypeFieldInfo_KnownDatatypefield _T80;struct _tuple1 _T81;struct _handler_cons*_T82;int*_T83;int _T84;struct Cyc_Absyn_Datatypedecl**(*_T85)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T86)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Genv*_T87;struct Cyc_Dict_Dict _T88;struct Cyc_Absyn_Datatypedecl*_T89;struct _tuple0*_T8A;struct Cyc_Absyn_Datatypedecl**_T8B;void*_T8C;struct Cyc_Dict_Absent_exn_struct*_T8D;char*_T8E;char*_T8F;int(*_T90)(struct _fat_ptr);struct _fat_ptr _T91;struct Cyc_String_pa_PrintArg_struct _T92;struct Cyc_Absyn_Datatypedecl*_T93;struct _tuple0*_T94;void**_T95;struct _fat_ptr _T96;struct _fat_ptr _T97;struct Cyc_Absyn_Datatypedecl*_T98;struct Cyc_Core_Opt*_T99;int(*_T9A)(struct _fat_ptr);struct _fat_ptr _T9B;struct Cyc_String_pa_PrintArg_struct _T9C;struct Cyc_Absyn_Datatypedecl*_T9D;struct _tuple0*_T9E;void**_T9F;struct _fat_ptr _TA0;struct _fat_ptr _TA1;struct Cyc_Absyn_Datatypedecl*_TA2;struct Cyc_Core_Opt*_TA3;void*_TA4;struct Cyc_Absyn_Datatypefield*_TA5;struct _tuple0*_TA6;struct _tuple0 _TA7;struct _fat_ptr*_TA8;struct Cyc_List_List*_TA9;void*_TAA;struct Cyc_Absyn_Datatypefield*_TAB;struct _tuple0*_TAC;struct _tuple0 _TAD;struct _fat_ptr*_TAE;int _TAF;struct Cyc_List_List*_TB0;void*_TB1;struct Cyc_List_List*_TB2;int(*_TB3)(struct _fat_ptr);struct _fat_ptr _TB4;struct Cyc_String_pa_PrintArg_struct _TB5;struct Cyc_Absyn_Datatypefield*_TB6;struct _tuple0*_TB7;void**_TB8;struct _fat_ptr _TB9;struct _fat_ptr _TBA;struct Cyc_Absyn_Datatypedecl*_TBB;enum Cyc_Absyn_Scope _TBC;int _TBD;struct _fat_ptr _TBE;struct _tuple0*_TBF;struct _fat_ptr _TC0;struct Cyc_Absyn_Datatypedecl*_TC1;struct _tuple0*_TC2;struct Cyc_Absyn_Datatypedecl*_TC3;enum Cyc_Absyn_Scope _TC4;int _TC5;struct _fat_ptr _TC6;struct _tuple0*_TC7;struct _fat_ptr _TC8;struct Cyc_Absyn_Datatypedecl*_TC9;struct _tuple0*_TCA;struct Cyc_Absyn_Datatypefield*_TCB;enum Cyc_Absyn_Scope _TCC;int _TCD;struct _fat_ptr _TCE;struct _tuple0*_TCF;struct _fat_ptr _TD0;struct Cyc_String_pa_PrintArg_struct _TD1;struct Cyc_Absyn_Datatypefield*_TD2;struct _tuple0*_TD3;void**_TD4;struct _fat_ptr _TD5;struct _fat_ptr _TD6;struct Cyc_Absyn_Datatypedecl*_TD7;struct _tuple0*_TD8;int _TD9;struct Cyc_Tcenv_Genv*_TDA;struct Cyc_Interface_Seen*_TDB;struct _fat_ptr _TDC;struct _tuple0*_TDD;void*(*_TDE)(void*);struct Cyc_List_List*_TDF;int _TE0;
void*_TE1=Cyc_Absyn_compress(t);struct Cyc_Absyn_Datatypefield*_TE2;struct Cyc_Absyn_Datatypedecl*_TE3;struct _tuple0*_TE4;union Cyc_Absyn_AggrInfo _TE5;struct Cyc_Absyn_VarargInfo*_TE6;struct Cyc_List_List*_TE7;void*_TE8;_T0=(int*)_TE1;_T1=*_T0;switch(_T1){case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TE9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TE1;_T2=_TE9->f1;_TE8=_T2.elt_type;}{void*t=_TE8;_TE8=t;goto _LL4;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TE9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TE1;_T3=_TE9->f1;_TE8=_T3.elt_type;}_LL4: {void*t=_TE8;_TE8=t;goto _LL6;}case 8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TE9=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TE1;_T4=_TE9->f4;_TE8=(void*)_T4;}_T5=(void*)_TE8;if(_T5==0)goto _TL32;_LL6:{void*t=_TE8;_T6=
# 248
Cyc_Interface_check_public_type(ae,seen,obj,name,t);return _T6;}_TL32: goto _LL11;case 6:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_TE9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TE1;_T7=_TE9->f1;_TE8=_T7.ret_type;_T8=_TE9->f1;_TE7=_T8.args;_T9=_TE9->f1;_TE6=_T9.cyc_varargs;}{void*ret=_TE8;struct Cyc_List_List*args=_TE7;struct Cyc_Absyn_VarargInfo*cyc_varargs=_TE6;{
# 252
int(*_TE9)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple8*),struct Cyc_List_List*)=(int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list;_TB=_TE9;}_TC=ae;_TD=seen;_TE=obj;_TF=name;_T10=args;_T11=_TB(_TC,_TD,_TE,_TF,Cyc_Interface_get_type2,_T10);if(!_T11)goto _TL34;_TA=
Cyc_Interface_check_public_type(ae,seen,obj,name,ret);goto _TL35;_TL34: _TA=0;_TL35: {
# 252
int b=_TA;
# 254
if(cyc_varargs==0)goto _TL36;_T12=cyc_varargs;{
struct Cyc_Absyn_VarargInfo _TE9=*_T12;void*_TEA;_TEA=_TE9.type;{void*vt=_TEA;
b=Cyc_Interface_check_public_type(ae,seen,obj,name,vt);}}goto _TL37;_TL36: _TL37: _T13=b;
# 258
return _T13;}}case 0: _T14=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE1;_T15=_T14->f1;_T16=(int*)_T15;_T17=*_T16;switch(_T17){case 24:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE1;_T18=_TE9->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TEA=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T18;_TE5=_TEA->f1;}_TE7=_TE9->f2;}{union Cyc_Absyn_AggrInfo info=_TE5;struct Cyc_List_List*targs=_TE7;
# 261
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T19=ad;_T1A=_T19->sc;_T1B=(int)_T1A;
if(_T1B!=0)goto _TL39;_T1C=obj;_T1D=name;_T1E=
_tag_fat("type",sizeof(char),5U);_T1F=ad;_T20=_T1F->name;Cyc_Interface_static_err(_T1C,_T1D,_T1E,_T20);
return 0;_TL39: _T22=ae;_T23=seen;_T24=obj;_T25=name;_T26=Cyc_Core_identity;_T27=targs;_T28=
# 266
Cyc_Interface_check_public_type_list(_T22,_T23,_T24,_T25,_T26,_T27);if(!_T28)goto _TL3B;_T21=
Cyc_Interface_check_public_aggrdecl(ae,seen,ad);goto _TL3C;_TL3B: _T21=0;_TL3C:
# 266
 return _T21;}case 19:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE1;_T29=_TE9->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_TEA=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T29;_TE4=_TEA->f1;}}{struct _tuple0*name=_TE4;
# 270
struct Cyc_Absyn_Enumdecl*ed;{struct _handler_cons _TE9;_T2A=& _TE9;_push_handler(_T2A);{int _TEA=0;_T2B=_TE9.handler;_T2C=setjmp(_T2B);if(!_T2C)goto _TL3D;_TEA=1;goto _TL3E;_TL3D: _TL3E: if(_TEA)goto _TL3F;else{goto _TL41;}_TL41: _T2E=Cyc_Dict_lookup;{
struct Cyc_Absyn_Enumdecl**(*_TEB)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T2E;_T2D=_TEB;}_T2F=ae;_T30=_T2F->enumdecls;_T31=name;_T32=_T2D(_T30,_T31);ed=*_T32;_pop_handler();goto _TL40;_TL3F: _T33=Cyc_Core_get_exn_thrown();{void*_TEB=(void*)_T33;void*_TEC;_T34=(struct Cyc_Dict_Absent_exn_struct*)_TEB;_T35=_T34->tag;_T36=Cyc_Dict_Absent;if(_T35!=_T36)goto _TL42;{
# 273
int(*_TED)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T37=_TED;}{struct Cyc_String_pa_PrintArg_struct _TED;_TED.tag=0;
_TED.f1=Cyc_Absynpp_qvar2string(name);_T39=_TED;}{struct Cyc_String_pa_PrintArg_struct _TED=_T39;void*_TEE[1];_T3A=_TEE + 0;*_T3A=& _TED;_T3B=
# 273
_tag_fat("check_public_type (can't find enum %s)",sizeof(char),39U);_T3C=_tag_fat(_TEE,sizeof(void*),1);_T38=Cyc_aprintf(_T3B,_T3C);}_T37(_T38);goto _TL43;_TL42: _TEC=_TEB;{void*exn=_TEC;_rethrow(exn);}_TL43:;}_TL40:;}}_T3D=ed;_T3E=_T3D->sc;_T3F=(int)_T3E;
# 276
if(_T3F!=0)goto _TL44;_T40=obj;_T41=name;_T42=
_tag_fat("enum",sizeof(char),5U);_T43=ed;_T44=_T43->name;Cyc_Interface_static_err(_T40,_T41,_T42,_T44);
return 0;_TL44:
# 280
 return 1;}case 22: _T45=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE1;_T46=_T45->f1;_T47=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T46;_T48=_T47->f1;_T49=_T48.KnownDatatype;_T4A=_T49.tag;if(_T4A!=2)goto _TL46;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE1;_T4B=_TE9->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TEA=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T4B;_T4C=_TEA->f1;_T4D=_T4C.KnownDatatype;_T4E=_T4D.val;{struct Cyc_Absyn_Datatypedecl*_TEB=*_T4E;_TE3=_TEB;}}_TE7=_TE9->f2;}{struct Cyc_Absyn_Datatypedecl*tud0=_TE3;struct Cyc_List_List*targs=_TE7;
# 283
struct Cyc_Absyn_Datatypedecl*tud;{struct _handler_cons _TE9;_T4F=& _TE9;_push_handler(_T4F);{int _TEA=0;_T50=_TE9.handler;_T51=setjmp(_T50);if(!_T51)goto _TL48;_TEA=1;goto _TL49;_TL48: _TL49: if(_TEA)goto _TL4A;else{goto _TL4C;}_TL4C: _T53=Cyc_Dict_lookup;{
struct Cyc_Absyn_Datatypedecl**(*_TEB)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T53;_T52=_TEB;}_T54=ae;_T55=_T54->datatypedecls;_T56=tud0;_T57=_T56->name;_T58=_T52(_T55,_T57);tud=*_T58;_pop_handler();goto _TL4B;_TL4A: _T59=Cyc_Core_get_exn_thrown();{void*_TEB=(void*)_T59;void*_TEC;_T5A=(struct Cyc_Dict_Absent_exn_struct*)_TEB;_T5B=_T5A->tag;_T5C=Cyc_Dict_Absent;if(_T5B!=_T5C)goto _TL4D;{
# 286
int(*_TED)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T5D=_TED;}{struct Cyc_String_pa_PrintArg_struct _TED;_TED.tag=0;_T60=tud0;_T61=_T60->name;
_TED.f1=Cyc_Absynpp_qvar2string(_T61);_T5F=_TED;}{struct Cyc_String_pa_PrintArg_struct _TED=_T5F;void*_TEE[1];_T62=_TEE + 0;*_T62=& _TED;_T63=
# 286
_tag_fat("check_public_type (can't find datatype %s)",sizeof(char),43U);_T64=_tag_fat(_TEE,sizeof(void*),1);_T5E=Cyc_aprintf(_T63,_T64);}_T5D(_T5E);goto _TL4E;_TL4D: _TEC=_TEB;{void*exn=_TEC;_rethrow(exn);}_TL4E:;}_TL4B:;}}_T65=tud;_T66=_T65->sc;_T67=(int)_T66;
# 289
if(_T67!=0)goto _TL4F;_T68=obj;_T69=name;_T6A=
_tag_fat("datatype",sizeof(char),9U);_T6B=tud;_T6C=_T6B->name;Cyc_Interface_static_err(_T68,_T69,_T6A,_T6C);
return 0;_TL4F: _T6E=ae;_T6F=seen;_T70=obj;_T71=name;_T72=Cyc_Core_identity;_T73=targs;_T74=
# 293
Cyc_Interface_check_public_type_list(_T6E,_T6F,_T70,_T71,_T72,_T73);if(!_T74)goto _TL51;_T6D=
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);goto _TL52;_TL51: _T6D=0;_TL52:
# 293
 return _T6D;}_TL46: goto _LL11;case 23: _T75=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE1;_T76=_T75->f1;_T77=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T76;_T78=_T77->f1;_T79=_T78.KnownDatatypefield;_T7A=_T79.tag;if(_T7A!=2)goto _TL53;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE1;_T7B=_TE9->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TEA=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T7B;_T7C=_TEA->f1;_T7D=_T7C.KnownDatatypefield;_T7E=_T7D.val;_TE3=_T7E.f0;_T7F=_TEA->f1;_T80=_T7F.KnownDatatypefield;_T81=_T80.val;_TE2=_T81.f1;}_TE7=_TE9->f2;}{struct Cyc_Absyn_Datatypedecl*tud0=_TE3;struct Cyc_Absyn_Datatypefield*tuf0=_TE2;struct Cyc_List_List*targs=_TE7;
# 297
struct Cyc_Absyn_Datatypedecl*tud;{struct _handler_cons _TE9;_T82=& _TE9;_push_handler(_T82);{int _TEA=0;_T83=_TE9.handler;_T84=setjmp(_T83);if(!_T84)goto _TL55;_TEA=1;goto _TL56;_TL55: _TL56: if(_TEA)goto _TL57;else{goto _TL59;}_TL59: _T86=Cyc_Dict_lookup;{
struct Cyc_Absyn_Datatypedecl**(*_TEB)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T86;_T85=_TEB;}_T87=ae;_T88=_T87->datatypedecls;_T89=tud0;_T8A=_T89->name;_T8B=_T85(_T88,_T8A);tud=*_T8B;_pop_handler();goto _TL58;_TL57: _T8C=Cyc_Core_get_exn_thrown();{void*_TEB=(void*)_T8C;void*_TEC;_T8D=(struct Cyc_Dict_Absent_exn_struct*)_TEB;_T8E=_T8D->tag;_T8F=Cyc_Dict_Absent;if(_T8E!=_T8F)goto _TL5A;{
# 300
int(*_TED)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T90=_TED;}{struct Cyc_String_pa_PrintArg_struct _TED;_TED.tag=0;_T93=tud0;_T94=_T93->name;
_TED.f1=Cyc_Absynpp_qvar2string(_T94);_T92=_TED;}{struct Cyc_String_pa_PrintArg_struct _TED=_T92;void*_TEE[1];_T95=_TEE + 0;*_T95=& _TED;_T96=
# 300
_tag_fat("check_public_type (can't find datatype %s and search its fields)",sizeof(char),65U);_T97=_tag_fat(_TEE,sizeof(void*),1);_T91=Cyc_aprintf(_T96,_T97);}_T90(_T91);goto _TL5B;_TL5A: _TEC=_TEB;{void*exn=_TEC;_rethrow(exn);}_TL5B:;}_TL58:;}}_T98=tud;_T99=_T98->fields;
# 303
if(_T99!=0)goto _TL5C;{
int(*_TE9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T9A=_TE9;}{struct Cyc_String_pa_PrintArg_struct _TE9;_TE9.tag=0;_T9D=tud;_T9E=_T9D->name;
_TE9.f1=Cyc_Absynpp_qvar2string(_T9E);_T9C=_TE9;}{struct Cyc_String_pa_PrintArg_struct _TE9=_T9C;void*_TEA[1];_T9F=_TEA + 0;*_T9F=& _TE9;_TA0=
# 304
_tag_fat("check_public_type (datatype %s has no fields)",sizeof(char),46U);_TA1=_tag_fat(_TEA,sizeof(void*),1);_T9B=Cyc_aprintf(_TA0,_TA1);}_T9A(_T9B);goto _TL5D;_TL5C: _TL5D: {
# 307
struct Cyc_Absyn_Datatypefield*tuf1=0;_TA2=tud;_TA3=_TA2->fields;_TA4=_TA3->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_TA4;_TL61: if(fs!=0)goto _TL5F;else{goto _TL60;}
_TL5F: _TA5=tuf0;_TA6=_TA5->name;_TA7=*_TA6;_TA8=_TA7.f1;_TA9=fs;_TAA=_TA9->hd;_TAB=(struct Cyc_Absyn_Datatypefield*)_TAA;_TAC=_TAB->name;_TAD=*_TAC;_TAE=_TAD.f1;_TAF=Cyc_strptrcmp(_TA8,_TAE);if(_TAF!=0)goto _TL62;_TB0=fs;_TB1=_TB0->hd;
tuf1=(struct Cyc_Absyn_Datatypefield*)_TB1;goto _TL60;_TL62: _TB2=fs;
# 308
fs=_TB2->tl;goto _TL61;_TL60:;}
# 314
if(tuf1!=0)goto _TL64;{
int(*_TE9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_TB3=_TE9;}{struct Cyc_String_pa_PrintArg_struct _TE9;_TE9.tag=0;_TB6=tuf0;_TB7=_TB6->name;
_TE9.f1=Cyc_Absynpp_qvar2string(_TB7);_TB5=_TE9;}{struct Cyc_String_pa_PrintArg_struct _TE9=_TB5;void*_TEA[1];_TB8=_TEA + 0;*_TB8=& _TE9;_TB9=
# 315
_tag_fat("check_public_type (can't find datatypefield %s)",sizeof(char),48U);_TBA=_tag_fat(_TEA,sizeof(void*),1);_TB4=Cyc_aprintf(_TB9,_TBA);}_TB3(_TB4);goto _TL65;_TL64: _TL65: _TBB=tud;_TBC=_TBB->sc;_TBD=(int)_TBC;
# 318
if(_TBD!=0)goto _TL66;_TBE=obj;_TBF=name;_TC0=
_tag_fat("datatype",sizeof(char),9U);_TC1=tud;_TC2=_TC1->name;Cyc_Interface_static_err(_TBE,_TBF,_TC0,_TC2);
return 0;_TL66: _TC3=tud;_TC4=_TC3->sc;_TC5=(int)_TC4;
# 322
if(_TC5!=1)goto _TL68;_TC6=obj;_TC7=name;_TC8=
_tag_fat("datatype",sizeof(char),9U);_TC9=tud;_TCA=_TC9->name;Cyc_Interface_abstract_err(_TC6,_TC7,_TC8,_TCA);
return 0;_TL68: _TCB=tuf1;_TCC=_TCB->sc;_TCD=(int)_TCC;
# 326
if(_TCD!=0)goto _TL6A;_TCE=obj;_TCF=name;{struct Cyc_String_pa_PrintArg_struct _TE9;_TE9.tag=0;_TD2=tuf1;_TD3=_TD2->name;
_TE9.f1=Cyc_Absynpp_qvar2string(_TD3);_TD1=_TE9;}{struct Cyc_String_pa_PrintArg_struct _TE9=_TD1;void*_TEA[1];_TD4=_TEA + 0;*_TD4=& _TE9;_TD5=_tag_fat("field %s of",sizeof(char),12U);_TD6=_tag_fat(_TEA,sizeof(void*),1);_TD0=Cyc_aprintf(_TD5,_TD6);}_TD7=tud;_TD8=_TD7->name;Cyc_Interface_static_err(_TCE,_TCF,_TD0,_TD8);
return 0;_TL6A: _TDA=ae;_TDB=seen;_TDC=obj;_TDD=name;_TDE=Cyc_Core_identity;_TDF=targs;_TE0=
# 330
Cyc_Interface_check_public_type_list(_TDA,_TDB,_TDC,_TDD,_TDE,_TDF);if(!_TE0)goto _TL6C;_TD9=
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);goto _TL6D;_TL6C: _TD9=0;_TL6D:
# 330
 return _TD9;}}_TL53: goto _LL11;default: goto _LL11;};default: _LL11:
# 333
 return 1;};}struct _tuple13{struct Cyc_Interface_Ienv*f0;struct Cyc_Interface_Ienv*f1;int f2;struct Cyc_Tcenv_Genv*f3;struct Cyc_Interface_Seen*f4;struct Cyc_Interface_I*f5;};
# 339
static void Cyc_Interface_extract_aggrdecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Aggrdecl**dp){struct _tuple13*_T0;struct Cyc_Absyn_Aggrdecl**_T1;struct Cyc_Absyn_Aggrdecl*_T2;int _T3;int _T4;struct Cyc_Absyn_Aggrdecl*_T5;struct Cyc_Absyn_AggrdeclImpl*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_Absyn_Aggrdecl*_T9;struct _tuple0*_TA;struct Cyc_Absyn_Aggrdecl*_TB;struct Cyc_Absyn_AggrdeclImpl*_TC;int _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Absyn_Aggrdecl*_T10;struct _tuple0*_T11;struct Cyc_Absyn_Aggrdecl*_T12;struct Cyc_Absyn_Aggrdecl*_T13;struct Cyc_Absyn_Aggrdecl*_T14;int _T15;struct Cyc_Interface_Ienv*_T16;struct Cyc_Dict_Dict(*_T17)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*);struct Cyc_Dict_Dict(*_T18)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T19;struct Cyc_Dict_Dict _T1A;struct _tuple0*_T1B;struct Cyc_Absyn_Aggrdecl*_T1C;struct Cyc_Absyn_Aggrdecl*_T1D;struct Cyc_Absyn_AggrdeclImpl*_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct Cyc_Absyn_Aggrdecl*_T21;struct _tuple0*_T22;struct Cyc_Absyn_Aggrdecl*_T23;struct Cyc_Absyn_Aggrdecl*_T24;struct Cyc_Absyn_Aggrdecl*_T25;int _T26;struct Cyc_Interface_Ienv*_T27;struct Cyc_Dict_Dict(*_T28)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*);struct Cyc_Dict_Dict(*_T29)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T2A;struct Cyc_Dict_Dict _T2B;struct _tuple0*_T2C;struct Cyc_Absyn_Aggrdecl*_T2D;int _T2E;struct Cyc_Interface_Ienv*_T2F;struct Cyc_Dict_Dict(*_T30)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*);struct Cyc_Dict_Dict(*_T31)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T32;struct Cyc_Dict_Dict _T33;struct _tuple0*_T34;struct Cyc_Absyn_Aggrdecl*_T35;int(*_T36)(struct _fat_ptr);struct _fat_ptr _T37;struct Cyc_Interface_Seen*_T38;struct Cyc_Tcenv_Genv*_T39;int _T3A;struct Cyc_Interface_Ienv*_T3B;struct Cyc_Interface_Ienv*_T3C;_T0=env;{struct _tuple13 _T3D=*_T0;_T3C=_T3D.f0;_T3B=_T3D.f1;_T3A=_T3D.f2;_T39=_T3D.f3;_T38=_T3D.f4;}{struct Cyc_Interface_Ienv*imp=_T3C;struct Cyc_Interface_Ienv*exp=_T3B;int check_complete_defs=_T3A;struct Cyc_Tcenv_Genv*ae=_T39;struct Cyc_Interface_Seen*seen=_T38;_T1=dp;{
# 342
struct Cyc_Absyn_Aggrdecl*d=*_T1;_T2=d;{
enum Cyc_Absyn_Scope _T3D=_T2->sc;_T3=(int)_T3D;switch(_T3){case Cyc_Absyn_Static: _T4=check_complete_defs;
# 345
if(!_T4)goto _TL6F;_T5=d;_T6=_T5->impl;if(_T6!=0)goto _TL6F;_T7=
_tag_fat("static",sizeof(char),7U);_T8=_tag_fat("struct/union",sizeof(char),13U);_T9=d;_TA=_T9->name;Cyc_Interface_fields_err(_T7,_T8,_TA);goto _TL70;_TL6F: _TL70: goto _LL3;case Cyc_Absyn_Abstract: _TB=d;_TC=_TB->impl;
# 349
if(_TC!=0)goto _TL71;_TD=check_complete_defs;
if(!_TD)goto _TL73;_TE=
_tag_fat("abstract",sizeof(char),9U);_TF=_tag_fat("struct/union",sizeof(char),13U);_T10=d;_T11=_T10->name;Cyc_Interface_fields_err(_TE,_TF,_T11);goto _TL74;_TL73: _TL74: goto _TL72;
# 353
_TL71:{struct Cyc_Absyn_Aggrdecl*_T3E=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T13=d;*_T3E=*_T13;_T12=(struct Cyc_Absyn_Aggrdecl*)_T3E;}d=_T12;_T14=d;
_T14->impl=0;_TL72: _T15=
# 356
Cyc_Interface_check_public_aggrdecl(ae,seen,d);if(!_T15)goto _TL75;_T16=exp;_T18=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T3E)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))_T18;_T17=_T3E;}_T19=exp;_T1A=_T19->aggrdecls;_T1B=x;_T1C=d;_T16->aggrdecls=_T17(_T1A,_T1B,_T1C);goto _TL76;_TL75: _TL76: goto _LL3;case Cyc_Absyn_Public: _T1D=d;_T1E=_T1D->impl;
# 360
if(_T1E!=0)goto _TL77;_T1F=
_tag_fat("public",sizeof(char),7U);_T20=_tag_fat("struct/union",sizeof(char),13U);_T21=d;_T22=_T21->name;Cyc_Interface_fields_err(_T1F,_T20,_T22);{struct Cyc_Absyn_Aggrdecl*_T3E=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T24=d;
*_T3E=*_T24;_T23=(struct Cyc_Absyn_Aggrdecl*)_T3E;}d=_T23;_T25=d;
_T25->sc=1U;goto _TL78;_TL77: _TL78: _T26=
# 365
Cyc_Interface_check_public_aggrdecl(ae,seen,d);if(!_T26)goto _TL79;_T27=exp;_T29=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T3E)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))_T29;_T28=_T3E;}_T2A=exp;_T2B=_T2A->aggrdecls;_T2C=x;_T2D=d;_T27->aggrdecls=_T28(_T2B,_T2C,_T2D);goto _TL7A;_TL79: _TL7A: goto _LL3;case Cyc_Absyn_ExternC: goto _LLD;case Cyc_Absyn_Extern: _LLD: _T2E=
# 370
Cyc_Interface_check_public_aggrdecl(ae,seen,d);if(!_T2E)goto _TL7B;_T2F=imp;_T31=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T3E)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))_T31;_T30=_T3E;}_T32=imp;_T33=_T32->aggrdecls;_T34=x;_T35=d;_T2F->aggrdecls=_T30(_T33,_T34,_T35);goto _TL7C;_TL7B: _TL7C: goto _LL3;case Cyc_Absyn_Register: goto _LL11;default: _LL11:{
# 375
int(*_T3E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T36=_T3E;}_T37=_tag_fat("add_aggrdecl",sizeof(char),13U);_T36(_T37);goto _LL3;}_LL3:;}}}}
# 379
static void Cyc_Interface_extract_enumdecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Enumdecl**dp){struct _tuple13*_T0;struct Cyc_Absyn_Enumdecl**_T1;struct Cyc_Absyn_Enumdecl*_T2;int _T3;int _T4;struct Cyc_Absyn_Enumdecl*_T5;struct Cyc_Core_Opt*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_Absyn_Enumdecl*_T9;struct _tuple0*_TA;struct Cyc_Absyn_Enumdecl*_TB;struct Cyc_Core_Opt*_TC;int _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Absyn_Enumdecl*_T10;struct _tuple0*_T11;struct Cyc_Absyn_Enumdecl*_T12;struct Cyc_Absyn_Enumdecl*_T13;struct Cyc_Absyn_Enumdecl*_T14;int _T15;struct Cyc_Interface_Ienv*_T16;struct Cyc_Dict_Dict(*_T17)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*);struct Cyc_Dict_Dict(*_T18)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T19;struct Cyc_Dict_Dict _T1A;struct _tuple0*_T1B;struct Cyc_Absyn_Enumdecl*_T1C;struct Cyc_Absyn_Enumdecl*_T1D;struct Cyc_Core_Opt*_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct Cyc_Absyn_Enumdecl*_T21;struct _tuple0*_T22;struct Cyc_Absyn_Enumdecl*_T23;struct Cyc_Absyn_Enumdecl*_T24;struct Cyc_Absyn_Enumdecl*_T25;int _T26;struct Cyc_Interface_Ienv*_T27;struct Cyc_Dict_Dict(*_T28)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*);struct Cyc_Dict_Dict(*_T29)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T2A;struct Cyc_Dict_Dict _T2B;struct _tuple0*_T2C;struct Cyc_Absyn_Enumdecl*_T2D;int _T2E;struct Cyc_Interface_Ienv*_T2F;struct Cyc_Dict_Dict(*_T30)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*);struct Cyc_Dict_Dict(*_T31)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T32;struct Cyc_Dict_Dict _T33;struct _tuple0*_T34;struct Cyc_Absyn_Enumdecl*_T35;int(*_T36)(struct _fat_ptr);struct _fat_ptr _T37;struct Cyc_Interface_Seen*_T38;struct Cyc_Tcenv_Genv*_T39;int _T3A;struct Cyc_Interface_Ienv*_T3B;struct Cyc_Interface_Ienv*_T3C;_T0=env;{struct _tuple13 _T3D=*_T0;_T3C=_T3D.f0;_T3B=_T3D.f1;_T3A=_T3D.f2;_T39=_T3D.f3;_T38=_T3D.f4;}{struct Cyc_Interface_Ienv*imp=_T3C;struct Cyc_Interface_Ienv*exp=_T3B;int check_complete_defs=_T3A;struct Cyc_Tcenv_Genv*ae=_T39;struct Cyc_Interface_Seen*seen=_T38;_T1=dp;{
# 382
struct Cyc_Absyn_Enumdecl*d=*_T1;_T2=d;{
enum Cyc_Absyn_Scope _T3D=_T2->sc;_T3=(int)_T3D;switch(_T3){case Cyc_Absyn_Static: _T4=check_complete_defs;
# 385
if(!_T4)goto _TL7E;_T5=d;_T6=_T5->fields;if(_T6!=0)goto _TL7E;_T7=
_tag_fat("static",sizeof(char),7U);_T8=_tag_fat("enum",sizeof(char),5U);_T9=d;_TA=_T9->name;Cyc_Interface_fields_err(_T7,_T8,_TA);goto _TL7F;_TL7E: _TL7F: goto _LL3;case Cyc_Absyn_Abstract: _TB=d;_TC=_TB->fields;
# 389
if(_TC!=0)goto _TL80;_TD=check_complete_defs;
if(!_TD)goto _TL82;_TE=
_tag_fat("abstract",sizeof(char),9U);_TF=_tag_fat("enum",sizeof(char),5U);_T10=d;_T11=_T10->name;Cyc_Interface_fields_err(_TE,_TF,_T11);goto _TL83;_TL82: _TL83: goto _TL81;
# 393
_TL80:{struct Cyc_Absyn_Enumdecl*_T3E=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T13=d;*_T3E=*_T13;_T12=(struct Cyc_Absyn_Enumdecl*)_T3E;}d=_T12;_T14=d;
_T14->fields=0;_TL81: _T15=
# 396
Cyc_Interface_check_public_enumdecl(ae,seen,d);if(!_T15)goto _TL84;_T16=exp;_T18=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T3E)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))_T18;_T17=_T3E;}_T19=exp;_T1A=_T19->enumdecls;_T1B=x;_T1C=d;_T16->enumdecls=_T17(_T1A,_T1B,_T1C);goto _TL85;_TL84: _TL85: goto _LL3;case Cyc_Absyn_Public: _T1D=d;_T1E=_T1D->fields;
# 400
if(_T1E!=0)goto _TL86;_T1F=
_tag_fat("public",sizeof(char),7U);_T20=_tag_fat("enum",sizeof(char),5U);_T21=d;_T22=_T21->name;Cyc_Interface_fields_err(_T1F,_T20,_T22);{struct Cyc_Absyn_Enumdecl*_T3E=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T24=d;
*_T3E=*_T24;_T23=(struct Cyc_Absyn_Enumdecl*)_T3E;}d=_T23;_T25=d;
_T25->sc=1U;goto _TL87;_TL86: _TL87: _T26=
# 405
Cyc_Interface_check_public_enumdecl(ae,seen,d);if(!_T26)goto _TL88;_T27=exp;_T29=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T3E)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))_T29;_T28=_T3E;}_T2A=exp;_T2B=_T2A->enumdecls;_T2C=x;_T2D=d;_T27->enumdecls=_T28(_T2B,_T2C,_T2D);goto _TL89;_TL88: _TL89: goto _LL3;case Cyc_Absyn_ExternC: goto _LLD;case Cyc_Absyn_Extern: _LLD: _T2E=
# 410
Cyc_Interface_check_public_enumdecl(ae,seen,d);if(!_T2E)goto _TL8A;_T2F=imp;_T31=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T3E)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))_T31;_T30=_T3E;}_T32=imp;_T33=_T32->enumdecls;_T34=x;_T35=d;_T2F->enumdecls=_T30(_T33,_T34,_T35);goto _TL8B;_TL8A: _TL8B: goto _LL3;case Cyc_Absyn_Register: goto _LL11;default: _LL11:{
# 413
int(*_T3E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T36=_T3E;}_T37=_tag_fat("add_enumdecl",sizeof(char),13U);_T36(_T37);}_LL3:;}}}}
# 417
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){struct Cyc_Absyn_Datatypefield*_T0;int _T1;struct Cyc_Interface_I*_T2;struct Cyc_Interface_I*_T3;int(*_T4)(struct _fat_ptr);struct _fat_ptr _T5;struct Cyc_Interface_Ienv*_T6;struct Cyc_Dict_Dict(*_T7)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*);struct Cyc_Dict_Dict(*_T8)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T9;struct Cyc_Dict_Dict _TA;struct Cyc_Absyn_Datatypefield*_TB;struct _tuple0*_TC;struct Cyc_Tcdecl_Xdatatypefielddecl*_TD;
# 419
struct Cyc_Interface_Ienv*env;_T0=f;{
enum Cyc_Absyn_Scope _TE=_T0->sc;_T1=(int)_TE;switch(_T1){case Cyc_Absyn_Static:
 return;case Cyc_Absyn_Extern: _T2=i;
env=_T2->imports;goto _LL0;case Cyc_Absyn_Public: _T3=i;
env=_T3->exports;goto _LL0;default:{
int(*_TF)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T4=_TF;}_T5=_tag_fat("add_xdatatypefielddecl",sizeof(char),23U);_T4(_T5);}_LL0:;}_T6=env;_T8=Cyc_Dict_insert;{
# 428
struct Cyc_Dict_Dict(*_TE)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*))_T8;_T7=_TE;}_T9=env;_TA=_T9->xdatatypefielddecls;_TB=f;_TC=_TB->name;{struct Cyc_Tcdecl_Xdatatypefielddecl*_TE=_cycalloc(sizeof(struct Cyc_Tcdecl_Xdatatypefielddecl));
_TE->base=d;_TE->field=f;_TD=(struct Cyc_Tcdecl_Xdatatypefielddecl*)_TE;}
# 427
_T6->xdatatypefielddecls=
_T7(_TA,_TC,_TD);}
# 432
static void Cyc_Interface_extract_datatypedecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Datatypedecl**dp){struct _tuple13*_T0;struct Cyc_Absyn_Datatypedecl**_T1;struct Cyc_Absyn_Datatypedecl*_T2;int _T3;struct Cyc_Absyn_Datatypedecl*_T4;int _T5;int _T6;struct Cyc_Absyn_Datatypedecl*_T7;struct Cyc_Core_Opt*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct Cyc_Absyn_Datatypedecl*_TB;struct _tuple0*_TC;struct Cyc_Absyn_Datatypedecl*_TD;struct Cyc_Core_Opt*_TE;struct Cyc_Absyn_Datatypedecl*_TF;int _T10;int _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_Absyn_Datatypedecl*_T14;struct _tuple0*_T15;struct Cyc_Absyn_Datatypedecl*_T16;struct Cyc_Absyn_Datatypedecl*_T17;struct Cyc_Absyn_Datatypedecl*_T18;int _T19;struct Cyc_Interface_Ienv*_T1A;struct Cyc_Dict_Dict(*_T1B)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*);struct Cyc_Dict_Dict(*_T1C)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T1D;struct Cyc_Dict_Dict _T1E;struct _tuple0*_T1F;struct Cyc_Absyn_Datatypedecl*_T20;struct Cyc_Absyn_Datatypedecl*_T21;struct Cyc_Absyn_Datatypedecl*_T22;struct Cyc_Absyn_Datatypedecl*_T23;int _T24;struct Cyc_Absyn_Datatypedecl*_T25;struct Cyc_Core_Opt*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc_Absyn_Datatypedecl*_T29;struct _tuple0*_T2A;struct Cyc_Absyn_Datatypedecl*_T2B;int _T2C;struct Cyc_Absyn_Datatypedecl*_T2D;int _T2E;struct Cyc_Absyn_Datatypedecl*_T2F;struct Cyc_Core_Opt*_T30;struct Cyc_Absyn_Datatypedecl*_T31;struct Cyc_Core_Opt*_T32;void*_T33;struct Cyc_Absyn_Datatypedecl*_T34;struct Cyc_Interface_I*_T35;struct Cyc_Absyn_Datatypedecl*_T36;struct Cyc_List_List*_T37;void*_T38;struct Cyc_Absyn_Datatypefield*_T39;struct Cyc_List_List*_T3A;struct Cyc_Interface_Ienv*_T3B;struct Cyc_Dict_Dict(*_T3C)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*);struct Cyc_Dict_Dict(*_T3D)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T3E;struct Cyc_Dict_Dict _T3F;struct _tuple0*_T40;struct Cyc_Absyn_Datatypedecl*_T41;int(*_T42)(struct _fat_ptr);struct _fat_ptr _T43;int _T44;struct Cyc_Absyn_Datatypedecl*_T45;int _T46;struct Cyc_Absyn_Datatypedecl*_T47;struct Cyc_Core_Opt*_T48;struct Cyc_Absyn_Datatypedecl*_T49;struct Cyc_Absyn_Datatypedecl*_T4A;struct Cyc_Absyn_Datatypedecl*_T4B;struct Cyc_Core_Opt*_T4C;struct Cyc_Core_Opt*_T4D;void*_T4E;struct Cyc_Absyn_Datatypedecl*_T4F;struct Cyc_Interface_I*_T50;struct Cyc_Absyn_Datatypedecl*_T51;struct Cyc_List_List*_T52;void*_T53;struct Cyc_Absyn_Datatypefield*_T54;struct Cyc_List_List*_T55;struct Cyc_Interface_Ienv*_T56;struct Cyc_Dict_Dict(*_T57)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*);struct Cyc_Dict_Dict(*_T58)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T59;struct Cyc_Dict_Dict _T5A;struct _tuple0*_T5B;struct Cyc_Absyn_Datatypedecl*_T5C;int(*_T5D)(struct _fat_ptr);struct _fat_ptr _T5E;struct Cyc_Interface_I*_T5F;struct Cyc_Interface_Seen*_T60;struct Cyc_Tcenv_Genv*_T61;int _T62;struct Cyc_Interface_Ienv*_T63;struct Cyc_Interface_Ienv*_T64;_T0=env;{struct _tuple13 _T65=*_T0;_T64=_T65.f0;_T63=_T65.f1;_T62=_T65.f2;_T61=_T65.f3;_T60=_T65.f4;_T5F=_T65.f5;}{struct Cyc_Interface_Ienv*imp=_T64;struct Cyc_Interface_Ienv*exp=_T63;int check_complete_defs=_T62;struct Cyc_Tcenv_Genv*ae=_T61;struct Cyc_Interface_Seen*seen=_T60;struct Cyc_Interface_I*i=_T5F;_T1=dp;{
# 435
struct Cyc_Absyn_Datatypedecl*d=*_T1;_T2=d;{
# 437
enum Cyc_Absyn_Scope _T65=_T2->sc;_T3=(int)_T65;switch(_T3){case Cyc_Absyn_Static: _T4=d;_T5=_T4->is_extensible;
# 439
if(_T5)goto _TL8E;else{goto _TL90;}_TL90: _T6=check_complete_defs;if(!_T6)goto _TL8E;_T7=d;_T8=_T7->fields;if(_T8!=0)goto _TL8E;_T9=
_tag_fat("static",sizeof(char),7U);_TA=_tag_fat("datatype",sizeof(char),9U);_TB=d;_TC=_TB->name;Cyc_Interface_fields_err(_T9,_TA,_TC);goto _TL8F;_TL8E: _TL8F: goto _LL3;case Cyc_Absyn_Abstract: _TD=d;_TE=_TD->fields;
# 443
if(_TE!=0)goto _TL91;_TF=d;_T10=_TF->is_extensible;
if(_T10)goto _TL93;else{goto _TL95;}_TL95: _T11=check_complete_defs;if(!_T11)goto _TL93;_T12=
_tag_fat("abstract",sizeof(char),9U);_T13=_tag_fat("datatype",sizeof(char),9U);_T14=d;_T15=_T14->name;Cyc_Interface_fields_err(_T12,_T13,_T15);goto _TL94;_TL93: _TL94: goto _TL92;
# 447
_TL91:{struct Cyc_Absyn_Datatypedecl*_T66=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T17=d;*_T66=*_T17;_T16=(struct Cyc_Absyn_Datatypedecl*)_T66;}d=_T16;_T18=d;
_T18->fields=0;_TL92: _T19=
# 450
Cyc_Interface_check_public_datatypedecl(ae,seen,d);if(!_T19)goto _TL96;_T1A=exp;_T1C=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T66)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))_T1C;_T1B=_T66;}_T1D=exp;_T1E=_T1D->datatypedecls;_T1F=x;_T20=d;_T1A->datatypedecls=_T1B(_T1E,_T1F,_T20);goto _TL97;_TL96: _TL97: goto _LL3;case Cyc_Absyn_Public:{struct Cyc_Absyn_Datatypedecl*_T66=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T22=d;
# 454
*_T66=*_T22;_T21=(struct Cyc_Absyn_Datatypedecl*)_T66;}d=_T21;_T23=d;_T24=_T23->is_extensible;
if(_T24)goto _TL98;else{goto _TL9A;}_TL9A: _T25=d;_T26=_T25->fields;if(_T26!=0)goto _TL98;_T27=
_tag_fat("public",sizeof(char),7U);_T28=_tag_fat("datatype",sizeof(char),9U);_T29=d;_T2A=_T29->name;Cyc_Interface_fields_err(_T27,_T28,_T2A);_T2B=d;
_T2B->sc=1U;goto _TL99;_TL98: _TL99: _T2C=
# 459
Cyc_Interface_check_public_datatypedecl(ae,seen,d);if(!_T2C)goto _TL9B;_T2D=d;_T2E=_T2D->is_extensible;
if(!_T2E)goto _TL9D;_T2F=d;_T30=_T2F->fields;if(_T30==0)goto _TL9D;_T31=d;_T32=_T31->fields;_T33=_T32->v;{
struct Cyc_List_List*fields=(struct Cyc_List_List*)_T33;_T34=d;
_T34->fields=0;{
struct Cyc_List_List*f=fields;_TLA2: if(f!=0)goto _TLA0;else{goto _TLA1;}
_TLA0: _T35=i;_T36=d;_T37=f;_T38=_T37->hd;_T39=(struct Cyc_Absyn_Datatypefield*)_T38;Cyc_Interface_extract_xdatatypefielddecl(_T35,_T36,_T39);_T3A=f;
# 463
f=_T3A->tl;goto _TLA2;_TLA1:;}}goto _TL9E;_TL9D: _TL9E: _T3B=exp;_T3D=Cyc_Dict_insert;{
# 466
struct Cyc_Dict_Dict(*_T66)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))_T3D;_T3C=_T66;}_T3E=exp;_T3F=_T3E->datatypedecls;_T40=x;_T41=d;_T3B->datatypedecls=_T3C(_T3F,_T40,_T41);goto _TL9C;_TL9B: _TL9C: goto _LL3;case Cyc_Absyn_ExternC:{
# 469
int(*_T66)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T42=_T66;}_T43=_tag_fat("extract_datatypedecl",sizeof(char),21U);_T42(_T43);case Cyc_Absyn_Extern: _T44=
# 471
Cyc_Interface_check_public_datatypedecl(ae,seen,d);if(!_T44)goto _TLA3;_T45=d;_T46=_T45->is_extensible;
if(!_T46)goto _TLA5;_T47=d;_T48=_T47->fields;if(_T48==0)goto _TLA5;{struct Cyc_Absyn_Datatypedecl*_T66=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T4A=d;
*_T66=*_T4A;_T49=(struct Cyc_Absyn_Datatypedecl*)_T66;}d=_T49;_T4B=d;_T4C=_T4B->fields;_T4D=
_check_null(_T4C);_T4E=_T4D->v;{struct Cyc_List_List*fields=(struct Cyc_List_List*)_T4E;_T4F=d;
_T4F->fields=0;{
struct Cyc_List_List*f=fields;_TLAA: if(f!=0)goto _TLA8;else{goto _TLA9;}
_TLA8: _T50=i;_T51=d;_T52=f;_T53=_T52->hd;_T54=(struct Cyc_Absyn_Datatypefield*)_T53;Cyc_Interface_extract_xdatatypefielddecl(_T50,_T51,_T54);_T55=f;
# 476
f=_T55->tl;goto _TLAA;_TLA9:;}_T56=imp;_T58=Cyc_Dict_insert;{
# 478
struct Cyc_Dict_Dict(*_T66)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))_T58;_T57=_T66;}_T59=imp;_T5A=_T59->datatypedecls;_T5B=x;_T5C=d;_T56->datatypedecls=_T57(_T5A,_T5B,_T5C);}goto _TLA6;_TLA5: _TLA6: goto _TLA4;_TLA3: _TLA4: goto _LL3;case Cyc_Absyn_Register: goto _LL11;default: _LL11:{
# 483
int(*_T66)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T5D=_T66;}_T5E=_tag_fat("add_datatypedecl",sizeof(char),17U);_T5D(_T5E);goto _LL3;}_LL3:;}}}}
# 488
static void Cyc_Interface_extract_typedef(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d){struct _tuple13*_T0;int _T1;struct Cyc_Interface_Ienv*_T2;struct Cyc_Dict_Dict(*_T3)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*);struct Cyc_Dict_Dict(*_T4)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T5;struct Cyc_Dict_Dict _T6;struct _tuple0*_T7;struct Cyc_Absyn_Typedefdecl*_T8;struct Cyc_Interface_Seen*_T9;struct Cyc_Tcenv_Genv*_TA;struct Cyc_Interface_Ienv*_TB;_T0=env;{struct _tuple13 _TC=*_T0;_TB=_TC.f1;_TA=_TC.f3;_T9=_TC.f4;}{struct Cyc_Interface_Ienv*exp=_TB;struct Cyc_Tcenv_Genv*ae=_TA;struct Cyc_Interface_Seen*seen=_T9;_T1=
# 491
Cyc_Interface_check_public_typedefdecl(ae,seen,d);if(!_T1)goto _TLAB;_T2=exp;_T4=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TC)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*))_T4;_T3=_TC;}_T5=exp;_T6=_T5->typedefdecls;_T7=x;_T8=d;_T2->typedefdecls=_T3(_T6,_T7,_T8);goto _TLAC;_TLAB: _TLAC:;}}struct _tuple14{void*f0;int f1;};
# 495
static void Cyc_Interface_extract_ordinarie(struct _tuple13*env,struct _tuple0*x,struct _tuple14*v){struct _tuple13*_T0;struct _tuple14*_T1;struct _tuple14 _T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_Absyn_Fndecl*_T6;struct Cyc_Absyn_Vardecl*_T7;struct Cyc_Absyn_Fndecl*_T8;struct Cyc_Absyn_Vardecl*_T9;struct Cyc_Absyn_Vardecl*_TA;struct Cyc_Absyn_Vardecl*_TB;struct Cyc_Absyn_Fndecl*_TC;void*_TD;struct Cyc_Absyn_Vardecl*_TE;struct Cyc_Absyn_Vardecl*_TF;struct Cyc_Absyn_Vardecl*_T10;struct Cyc_Absyn_Vardecl*_T11;struct Cyc_Absyn_Vardecl*_T12;struct Cyc_Absyn_Fndecl*_T13;struct Cyc_Absyn_Stmt*_T14;struct Cyc_Absyn_Vardecl*_T15;struct Cyc_Absyn_Vardecl*_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Absyn_Vardecl*_T18;struct Cyc_Absyn_Vardecl*_T19;struct Cyc_Absyn_Vardecl*_T1A;struct Cyc_Absyn_Vardecl*_T1B;int _T1C;int _T1D;struct Cyc_Absyn_Vardecl*_T1E;void*_T1F;int _T20;struct _fat_ptr _T21;struct Cyc_Absyn_Vardecl*_T22;struct _tuple0*_T23;int(*_T24)(struct _fat_ptr);struct _fat_ptr _T25;int _T26;struct Cyc_Absyn_Vardecl*_T27;void*_T28;int _T29;struct _fat_ptr _T2A;struct Cyc_Absyn_Vardecl*_T2B;struct _tuple0*_T2C;int _T2D;struct Cyc_Interface_Ienv*_T2E;struct Cyc_Dict_Dict(*_T2F)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*);struct Cyc_Dict_Dict(*_T30)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T31;struct Cyc_Dict_Dict _T32;struct _tuple0*_T33;struct Cyc_Absyn_Vardecl*_T34;int _T35;struct Cyc_Interface_Ienv*_T36;struct Cyc_Dict_Dict(*_T37)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*);struct Cyc_Dict_Dict(*_T38)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Interface_Ienv*_T39;struct Cyc_Dict_Dict _T3A;struct _tuple0*_T3B;struct Cyc_Absyn_Vardecl*_T3C;struct Cyc_Interface_Seen*_T3D;struct Cyc_Tcenv_Genv*_T3E;int _T3F;struct Cyc_Interface_Ienv*_T40;struct Cyc_Interface_Ienv*_T41;_T0=env;{struct _tuple13 _T42=*_T0;_T41=_T42.f0;_T40=_T42.f1;_T3F=_T42.f2;_T3E=_T42.f3;_T3D=_T42.f4;}{struct Cyc_Interface_Ienv*imp=_T41;struct Cyc_Interface_Ienv*exp=_T40;int check_complete_defs=_T3F;struct Cyc_Tcenv_Genv*ae=_T3E;struct Cyc_Interface_Seen*seen=_T3D;_T1=v;_T2=*_T1;{
# 499
void*_T42=_T2.f0;struct Cyc_Absyn_Vardecl*_T43;struct Cyc_Absyn_Fndecl*_T44;_T3=(int*)_T42;_T4=*_T3;switch(_T4){case 2:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T45=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T42;_T44=_T45->f1;}{struct Cyc_Absyn_Fndecl*fd=_T44;
# 501
struct Cyc_Absyn_Vardecl*vd;vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T5=vd;_T6=fd;
_T5->sc=_T6->sc;_T7=vd;_T8=fd;
_T7->name=_T8->name;_T9=vd;
_T9->varloc=0U;_TA=vd;
_TA->tq=Cyc_Absyn_empty_tqual(0U);_TB=vd;_TC=fd;_TD=_TC->cached_type;
_TB->type=_check_null(_TD);_TE=vd;
_TE->initializer=0;_TF=vd;
_TF->rgn=0;_T10=vd;
_T10->attributes=0;_T11=vd;
_T11->escapes=0;_T12=vd;_T13=fd;_T14=_T13->body;
_T12->is_proto=_T14!=0;_T15=vd;
_T15->rename=0;
# 514
check_complete_defs=0;_T43=vd;goto _LL7;}case 1:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T45=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T42;_T43=_T45->f1;}_LL7: {struct Cyc_Absyn_Vardecl*d=_T43;_T16=d;_T17=_T16->initializer;
# 517
if(_T17==0)goto _TLAE;{struct Cyc_Absyn_Vardecl*_T45=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T19=d;
*_T45=*_T19;_T18=(struct Cyc_Absyn_Vardecl*)_T45;}d=_T18;_T1A=d;
_T1A->initializer=0;goto _TLAF;_TLAE: _TLAF: _T1B=d;{
# 521
enum Cyc_Absyn_Scope _T45=_T1B->sc;_T1C=(int)_T45;switch(_T1C){case Cyc_Absyn_Static: _T1D=check_complete_defs;
# 523
if(!_T1D)goto _TLB1;_T1E=d;_T1F=_T1E->type;_T20=Cyc_Tcutil_is_function_type(_T1F);if(!_T20)goto _TLB1;_T21=
_tag_fat("static",sizeof(char),7U);_T22=d;_T23=_T22->name;Cyc_Interface_body_err(_T21,_T23);goto _TLB2;_TLB1: _TLB2: goto _LLA;case Cyc_Absyn_Register: goto _LL10;case Cyc_Absyn_Abstract: _LL10:{
# 527
int(*_T46)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_T24=_T46;}_T25=_tag_fat("extract_ordinarie",sizeof(char),18U);_T24(_T25);case Cyc_Absyn_Public: _T26=check_complete_defs;
# 529
if(!_T26)goto _TLB3;_T27=d;_T28=_T27->type;_T29=Cyc_Tcutil_is_function_type(_T28);if(!_T29)goto _TLB3;_T2A=
_tag_fat("public",sizeof(char),7U);_T2B=d;_T2C=_T2B->name;Cyc_Interface_body_err(_T2A,_T2C);goto _TLB4;_TLB3: _TLB4: _T2D=
Cyc_Interface_check_public_vardecl(ae,seen,d);if(!_T2D)goto _TLB5;_T2E=exp;_T30=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T46)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*))_T30;_T2F=_T46;}_T31=exp;_T32=_T31->vardecls;_T33=x;_T34=d;_T2E->vardecls=_T2F(_T32,_T33,_T34);goto _TLB6;_TLB5: _TLB6: goto _LLA;case Cyc_Absyn_ExternC: goto _LL16;case Cyc_Absyn_Extern: _LL16: goto _LL18;default: _LL18: _T35=
# 537
Cyc_Interface_check_public_vardecl(ae,seen,d);if(!_T35)goto _TLB7;_T36=imp;_T38=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T46)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*))_T38;_T37=_T46;}_T39=imp;_T3A=_T39->vardecls;_T3B=x;_T3C=d;_T36->vardecls=_T37(_T3A,_T3B,_T3C);goto _TLB8;_TLB7: _TLB8: goto _LLA;}_LLA:;}goto _LL3;}default: goto _LL3;}_LL3:;}}}struct _tuple15{void*f0;void*f1;};
# 546
static struct Cyc_List_List*Cyc_Interface_remove_decl_from_list(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*d){struct _tuple15 _T0;struct Cyc_Absyn_Decl*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;int*_T9;int _TA;void*_TB;void*_TC;struct Cyc_Absyn_Typedefdecl*_TD;struct _tuple0*_TE;struct Cyc_Absyn_Typedefdecl*_TF;struct _tuple0*_T10;int _T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_Absyn_Decl*_T14;struct Cyc_List_List*_T15;void*_T16;int*_T17;int _T18;void*_T19;void*_T1A;struct Cyc_Absyn_Enumdecl*_T1B;struct _tuple0*_T1C;struct Cyc_Absyn_Enumdecl*_T1D;struct _tuple0*_T1E;int _T1F;struct Cyc_Absyn_Enumdecl*_T20;enum Cyc_Absyn_Scope _T21;int _T22;struct Cyc_Absyn_Enumdecl*_T23;struct Cyc_Absyn_Enumdecl*_T24;struct Cyc_List_List*_T25;struct Cyc_List_List*_T26;struct Cyc_Absyn_Decl*_T27;struct Cyc_List_List*_T28;void*_T29;int*_T2A;int _T2B;void*_T2C;void*_T2D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2E;struct Cyc_Absyn_Vardecl**_T2F;void*_T30;struct Cyc_Absyn_Vardecl**_T31;struct Cyc_Absyn_Vardecl*_T32;struct _tuple0*_T33;struct Cyc_Absyn_Vardecl*_T34;struct _tuple0*_T35;int _T36;struct Cyc_Absyn_Vardecl**_T37;struct Cyc_Absyn_Vardecl*_T38;enum Cyc_Absyn_Scope _T39;int _T3A;struct Cyc_Absyn_Vardecl**_T3B;struct Cyc_Absyn_Vardecl*_T3C;struct Cyc_Absyn_Vardecl*_T3D;struct Cyc_List_List*_T3E;struct Cyc_List_List*_T3F;struct Cyc_Absyn_Decl*_T40;struct Cyc_List_List*_T41;struct Cyc_List_List*_T42;struct Cyc_List_List*_T43;void*_T44;struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;struct Cyc_Absyn_Decl*_T47;
if(l!=0)goto _TLB9;return 0;_TLB9:{struct _tuple15 _T48;_T1=d;
_T48.f0=_T1->r;_T2=l;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;_T48.f1=_T4->r;_T0=_T48;}{struct _tuple15 _T48=_T0;struct Cyc_Absyn_Vardecl*_T49;struct Cyc_Absyn_Enumdecl*_T4A;struct Cyc_Absyn_Typedefdecl*_T4B;void*_T4C;_T5=_T48.f0;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 8: _T8=_T48.f1;_T9=(int*)_T8;_TA=*_T9;if(_TA!=8)goto _TLBC;_TB=_T48.f0;{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T4D=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_TB;_T4C=_T4D->f1;}_TC=_T48.f1;{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T4D=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_TC;_T4B=_T4D->f1;}{struct Cyc_Absyn_Typedefdecl*a1=_T4C;struct Cyc_Absyn_Typedefdecl*a2=_T4B;_TD=a1;_TE=_TD->name;_TF=a2;_T10=_TF->name;_T11=
# 550
Cyc_Absyn_qvar_cmp(_TE,_T10);if(_T11==0)goto _TLBE;goto _LL0;_TLBE: _T12=l;_T13=_T12->tl;_T14=d;_T15=
Cyc_Interface_remove_decl_from_list(_T13,_T14);return _T15;}_TLBC: goto _LL7;case 7: _T16=_T48.f1;_T17=(int*)_T16;_T18=*_T17;if(_T18!=7)goto _TLC0;_T19=_T48.f0;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T4D=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T19;_T4C=_T4D->f1;}_T1A=_T48.f1;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T4D=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T1A;_T4A=_T4D->f1;}{struct Cyc_Absyn_Enumdecl*a1=_T4C;struct Cyc_Absyn_Enumdecl*a2=_T4A;_T1B=a1;_T1C=_T1B->name;_T1D=a2;_T1E=_T1D->name;_T1F=
# 553
Cyc_Absyn_qvar_cmp(_T1C,_T1E);if(_T1F==0)goto _TLC2;goto _LL0;_TLC2: _T20=a1;_T21=_T20->sc;_T22=(int)_T21;
if(_T22!=3)goto _TLC4;_T23=a1;_T24=a2;_T23->sc=_T24->sc;goto _TLC5;_TLC4: _TLC5: _T25=l;_T26=_T25->tl;_T27=d;_T28=
Cyc_Interface_remove_decl_from_list(_T26,_T27);return _T28;goto _LL0;}_TLC0: goto _LL7;case 0: _T29=_T48.f1;_T2A=(int*)_T29;_T2B=*_T2A;if(_T2B!=0)goto _TLC6;_T2C=_T48.f0;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2C;_T2D=_T48.f0;_T2E=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2D;_T2F=& _T2E->f1;_T4C=(struct Cyc_Absyn_Vardecl**)_T2F;}_T30=_T48.f1;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T30;_T49=_T4D->f1;}{struct Cyc_Absyn_Vardecl**vd1=(struct Cyc_Absyn_Vardecl**)_T4C;struct Cyc_Absyn_Vardecl*vd2=_T49;_T31=vd1;_T32=*_T31;_T33=_T32->name;_T34=vd2;_T35=_T34->name;_T36=
# 558
Cyc_Absyn_qvar_cmp(_T33,_T35);if(_T36==0)goto _TLC8;goto _LL0;_TLC8: _T37=vd1;_T38=*_T37;_T39=_T38->sc;_T3A=(int)_T39;
if(_T3A!=3)goto _TLCA;_T3B=vd1;{struct Cyc_Absyn_Vardecl*_T4D=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T3D=vd2;*_T4D=*_T3D;_T3C=(struct Cyc_Absyn_Vardecl*)_T4D;}*_T3B=_T3C;goto _TLCB;_TLCA: _TLCB: _T3E=l;_T3F=_T3E->tl;_T40=d;_T41=
Cyc_Interface_remove_decl_from_list(_T3F,_T40);return _T41;}_TLC6: goto _LL7;default: _LL7: goto _LL0;}_LL0:;}{struct Cyc_List_List*_T48=_cycalloc(sizeof(struct Cyc_List_List));_T43=l;_T44=_T43->hd;
# 564
_T48->hd=(struct Cyc_Absyn_Decl*)_T44;_T45=l;_T46=_T45->tl;_T47=d;_T48->tl=Cyc_Interface_remove_decl_from_list(_T46,_T47);_T42=(struct Cyc_List_List*)_T48;}return _T42;}
# 567
static struct Cyc_List_List*Cyc_Interface_uniqify_decl_list(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){struct Cyc_List_List*_T0;struct _tuple15 _T1;struct Cyc_Absyn_Decl*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Decl*_T5;void*_T6;int*_T7;unsigned _T8;void*_T9;int*_TA;int _TB;void*_TC;void*_TD;void*_TE;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TF;struct Cyc_Absyn_Vardecl**_T10;struct Cyc_Absyn_Vardecl*_T11;struct _tuple0*_T12;struct Cyc_Absyn_Vardecl**_T13;struct Cyc_Absyn_Vardecl*_T14;struct _tuple0*_T15;int _T16;struct Cyc_Absyn_Vardecl**_T17;struct Cyc_Absyn_Vardecl*_T18;enum Cyc_Absyn_Scope _T19;int _T1A;struct Cyc_Absyn_Vardecl**_T1B;struct Cyc_Absyn_Vardecl*_T1C;struct Cyc_Absyn_Vardecl*_T1D;struct Cyc_List_List*_T1E;void*_T1F;int*_T20;int _T21;void*_T22;void*_T23;struct Cyc_Absyn_Typedefdecl*_T24;struct _tuple0*_T25;struct Cyc_Absyn_Typedefdecl*_T26;struct _tuple0*_T27;int _T28;struct Cyc_List_List*_T29;void*_T2A;int*_T2B;int _T2C;void*_T2D;void*_T2E;struct Cyc_Absyn_Enumdecl*_T2F;struct _tuple0*_T30;struct Cyc_Absyn_Enumdecl*_T31;struct _tuple0*_T32;int _T33;struct Cyc_List_List*_T34;void*_T35;int*_T36;int _T37;void*_T38;void*_T39;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T3A;struct Cyc_List_List**_T3B;void*_T3C;int _T3D;struct Cyc_List_List**_T3E;struct Cyc_List_List**_T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*_T41;void*_T42;struct Cyc_Absyn_Decl*_T43;struct Cyc_List_List*_T44;struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;
if(accum!=0)goto _TLCC;{struct Cyc_List_List*_T47=_cycalloc(sizeof(struct Cyc_List_List));_T47->hd=d;_T47->tl=0;_T0=(struct Cyc_List_List*)_T47;}return _T0;_TLCC:{
struct Cyc_List_List*l=accum;_TLD1: if(l!=0)goto _TLCF;else{goto _TLD0;}
_TLCF:{struct _tuple15 _T47;_T2=d;_T47.f0=_T2->r;_T3=l;_T4=_T3->hd;_T5=(struct Cyc_Absyn_Decl*)_T4;_T47.f1=_T5->r;_T1=_T47;}{struct _tuple15 _T47=_T1;struct Cyc_List_List*_T48;struct _fat_ptr*_T49;struct _fat_ptr*_T4A;struct Cyc_Absyn_Enumdecl*_T4B;struct Cyc_Absyn_Typedefdecl*_T4C;void*_T4D;void*_T4E;_T6=_T47.f0;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 0: _T9=_T47.f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=0)goto _TLD3;_TC=_T47.f0;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_TC;_T4E=_T4F->f1;}_TD=_T47.f1;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_TD;_TE=_T47.f1;_TF=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_TE;_T10=& _TF->f1;_T4D=(struct Cyc_Absyn_Vardecl**)_T10;}{struct Cyc_Absyn_Vardecl*vd1=_T4E;struct Cyc_Absyn_Vardecl**vd2=(struct Cyc_Absyn_Vardecl**)_T4D;_T11=vd1;_T12=_T11->name;_T13=vd2;_T14=*_T13;_T15=_T14->name;_T16=
# 572
Cyc_Absyn_qvar_cmp(_T12,_T15);if(_T16==0)goto _TLD5;goto _LL0;_TLD5: _T17=vd2;_T18=*_T17;_T19=_T18->sc;_T1A=(int)_T19;
if(_T1A!=3)goto _TLD7;_T1B=vd2;{struct Cyc_Absyn_Vardecl*_T4F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T1D=vd1;*_T4F=*_T1D;_T1C=(struct Cyc_Absyn_Vardecl*)_T4F;}*_T1B=_T1C;goto _TLD8;_TLD7: _TLD8: _T1E=accum;
return _T1E;}_TLD3: goto _LL9;case 8: _T1F=_T47.f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=8)goto _TLD9;_T22=_T47.f0;{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T4F=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T22;_T4E=_T4F->f1;}_T23=_T47.f1;{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T4F=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T23;_T4C=_T4F->f1;}{struct Cyc_Absyn_Typedefdecl*a1=_T4E;struct Cyc_Absyn_Typedefdecl*a2=_T4C;_T24=a1;_T25=_T24->name;_T26=a2;_T27=_T26->name;_T28=
# 576
Cyc_Absyn_qvar_cmp(_T25,_T27);if(_T28==0)goto _TLDB;goto _LL0;_TLDB: _T29=accum;
return _T29;}_TLD9: goto _LL9;case 7: _T2A=_T47.f1;_T2B=(int*)_T2A;_T2C=*_T2B;if(_T2C!=7)goto _TLDD;_T2D=_T47.f0;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T4F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T2D;_T4E=_T4F->f1;}_T2E=_T47.f1;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T4F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T2E;_T4B=_T4F->f1;}{struct Cyc_Absyn_Enumdecl*a1=_T4E;struct Cyc_Absyn_Enumdecl*a2=_T4B;_T2F=a1;_T30=_T2F->name;_T31=a2;_T32=_T31->name;_T33=
# 579
Cyc_Absyn_qvar_cmp(_T30,_T32);if(_T33==0)goto _TLDF;goto _LL0;_TLDF: _T34=accum;
return _T34;}_TLDD: goto _LL9;case 9: _T35=_T47.f1;_T36=(int*)_T35;_T37=*_T36;if(_T37!=9)goto _TLE1;_T38=_T47.f0;{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T4F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T38;_T4A=_T4F->f1;_T39=_T47.f0;_T3A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T39;_T3B=& _T3A->f2;_T4E=(struct Cyc_List_List**)_T3B;}_T3C=_T47.f1;{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T4F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T3C;_T49=_T4F->f1;_T48=_T4F->f2;}{struct _fat_ptr*a1=_T4A;struct Cyc_List_List**b1=(struct Cyc_List_List**)_T4E;struct _fat_ptr*a2=_T49;struct Cyc_List_List*b2=_T48;_T3D=
# 582
Cyc_strptrcmp(a1,a2);if(_T3D==0)goto _TLE3;goto _LL0;_TLE3:{
struct Cyc_List_List*dl=b2;_TLE8: if(dl!=0)goto _TLE6;else{goto _TLE7;}
_TLE6: _T3E=b1;_T3F=b1;_T40=*_T3F;_T41=dl;_T42=_T41->hd;_T43=(struct Cyc_Absyn_Decl*)_T42;*_T3E=Cyc_Interface_remove_decl_from_list(_T40,_T43);_T44=dl;
# 583
dl=_T44->tl;goto _TLE8;_TLE7:;}goto _LL0;}_TLE1: goto _LL9;default: _LL9: goto _LL0;}_LL0:;}_T45=l;
# 569
l=_T45->tl;goto _TLD1;_TLD0:;}{struct Cyc_List_List*_T47=_cycalloc(sizeof(struct Cyc_List_List));
# 588
_T47->hd=d;_T47->tl=accum;_T46=(struct Cyc_List_List*)_T47;}return _T46;}
# 591
static struct Cyc_List_List*Cyc_Interface_filterstatics(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Vardecl*_T3;enum Cyc_Absyn_Scope _T4;int _T5;struct Cyc_List_List*_T6;struct Cyc_Absyn_Vardecl*_T7;enum Cyc_Absyn_Scope _T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_Absyn_Vardecl*_TB;struct Cyc_Absyn_Vardecl*_TC;struct Cyc_Absyn_Vardecl*_TD;struct Cyc_Absyn_Vardecl*_TE;void*_TF;int _T10;struct Cyc_Absyn_Vardecl*_T11;enum Cyc_Absyn_Scope _T12;int _T13;struct Cyc_Absyn_Vardecl*_T14;struct Cyc_List_List*_T15;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T16;void*_T17;struct Cyc_Absyn_Fndecl*_T18;enum Cyc_Absyn_Scope _T19;int _T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_Fndecl*_T1C;enum Cyc_Absyn_Scope _T1D;int _T1E;struct Cyc_List_List*_T1F;struct Cyc_Absyn_Fndecl*_T20;struct _tuple0*_T21;struct Cyc_Absyn_Fndecl*_T22;void*_T23;void*_T24;struct Cyc_Absyn_Vardecl*_T25;struct Cyc_Absyn_Fndecl*_T26;struct Cyc_List_List*_T27;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T28;void*_T29;struct Cyc_Absyn_Aggrdecl*_T2A;enum Cyc_Absyn_Scope _T2B;int _T2C;struct Cyc_List_List*_T2D;struct Cyc_Absyn_Datatypedecl*_T2E;enum Cyc_Absyn_Scope _T2F;int _T30;struct Cyc_List_List*_T31;struct Cyc_Absyn_Enumdecl*_T32;enum Cyc_Absyn_Scope _T33;int _T34;struct Cyc_List_List*_T35;struct Cyc_Absyn_Enumdecl*_T36;enum Cyc_Absyn_Scope _T37;int _T38;struct Cyc_List_List*_T39;struct Cyc_List_List*(*_T3A)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*);void*(*_T3B)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T3C;struct Cyc_List_List*_T3D;struct Cyc_List_List*_T3E;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T3F;void*_T40;struct Cyc_List_List*(*_T41)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*);void*(*_T42)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T43;struct Cyc_List_List*_T44;struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;struct Cyc_List_List*_T48;struct Cyc_Absyn_Decl*_T49;struct Cyc_Absyn_Decl*_T4A;_T0=d;{
void*_T4B=_T0->r;struct Cyc_List_List*_T4C;struct _fat_ptr*_T4D;struct Cyc_Absyn_Typedefdecl*_T4E;struct Cyc_Absyn_Enumdecl*_T4F;struct Cyc_Absyn_Datatypedecl*_T50;struct Cyc_Absyn_Aggrdecl*_T51;struct Cyc_Absyn_Fndecl*_T52;struct Cyc_Absyn_Vardecl*_T53;_T1=(int*)_T4B;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T54=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T4B;_T53=_T54->f1;}{struct Cyc_Absyn_Vardecl*vd=_T53;_T3=vd;_T4=_T3->sc;_T5=(int)_T4;
# 594
if(_T5!=4)goto _TLEA;_T6=accum;return _T6;_TLEA: _T7=vd;_T8=_T7->sc;_T9=(int)_T8;
if(_T9!=0)goto _TLEC;_TA=accum;return _TA;_TLEC: {
struct Cyc_Absyn_Vardecl*nvd;nvd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_TB=nvd;_TC=vd;*_TB=*_TC;_TD=nvd;
_TD->initializer=0;_TE=nvd;_TF=_TE->type;_T10=
Cyc_Tcutil_is_function_type(_TF);if(!_T10)goto _TLEE;_T11=nvd;_T12=_T11->sc;_T13=(int)_T12;if(_T13==3)goto _TLEE;_T14=nvd;
_T14->sc=3U;goto _TLEF;_TLEE: _TLEF:{struct Cyc_List_List*_T54=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T55=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T55->tag=0;
_T55->f1=nvd;_T16=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T55;}_T17=(void*)_T16;_T54->hd=Cyc_Absyn_new_decl(_T17,0U);_T54->tl=accum;_T15=(struct Cyc_List_List*)_T54;}return _T15;}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T54=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T4B;_T52=_T54->f1;}{struct Cyc_Absyn_Fndecl*fd=_T52;_T18=fd;_T19=_T18->sc;_T1A=(int)_T19;
# 602
if(_T1A!=0)goto _TLF0;_T1B=accum;return _T1B;_TLF0: _T1C=fd;_T1D=_T1C->sc;_T1E=(int)_T1D;
if(_T1E!=4)goto _TLF2;_T1F=accum;return _T1F;_TLF2: _T20=fd;_T21=_T20->name;_T22=fd;_T23=_T22->cached_type;_T24=
_check_null(_T23);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,_T21,_T24,0,0);_T25=vd;_T26=fd;
_T25->sc=_T26->sc;{struct Cyc_List_List*_T54=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T55=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T55->tag=0;
_T55->f1=vd;_T28=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T55;}_T29=(void*)_T28;_T54->hd=Cyc_Absyn_new_decl(_T29,0U);_T54->tl=accum;_T27=(struct Cyc_List_List*)_T54;}return _T27;}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T54=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T4B;_T51=_T54->f1;}{struct Cyc_Absyn_Aggrdecl*a=_T51;_T2A=a;_T2B=_T2A->sc;_T2C=(int)_T2B;
# 609
if(_T2C!=4)goto _TLF4;_T2D=accum;return _T2D;_TLF4: goto _LL0;}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T54=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T4B;_T50=_T54->f1;}{struct Cyc_Absyn_Datatypedecl*a=_T50;_T2E=a;_T2F=_T2E->sc;_T30=(int)_T2F;
# 613
if(_T30!=4)goto _TLF6;_T31=accum;return _T31;_TLF6: goto _LL0;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T54=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T4B;_T4F=_T54->f1;}{struct Cyc_Absyn_Enumdecl*a=_T4F;_T32=a;_T33=_T32->sc;_T34=(int)_T33;
# 616
if(_T34!=0)goto _TLF8;_T35=accum;return _T35;_TLF8: _T36=a;_T37=_T36->sc;_T38=(int)_T37;
if(_T38!=4)goto _TLFA;_T39=accum;return _T39;_TLFA: goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T54=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T4B;_T4E=_T54->f1;}{struct Cyc_Absyn_Typedefdecl*a=_T4E;goto _LL0;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T54=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T4B;_T4D=_T54->f1;_T4C=_T54->f2;}{struct _fat_ptr*a=_T4D;struct Cyc_List_List*b=_T4C;_T3B=Cyc_List_fold_left;{
# 622
struct Cyc_List_List*(*_T54)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))_T3B;_T3A=_T54;}_T3C=b;_T3D=_T3A(Cyc_Interface_filterstatics,0,_T3C);{struct Cyc_List_List*l=Cyc_List_rev(_T3D);{struct Cyc_List_List*_T54=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T55=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T55->tag=9;
_T55->f1=a;_T55->f2=l;_T3F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T55;}_T40=(void*)_T3F;_T54->hd=Cyc_Absyn_new_decl(_T40,0U);_T54->tl=accum;_T3E=(struct Cyc_List_List*)_T54;}return _T3E;}}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T54=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T4B;_T4C=_T54->f2;}{struct Cyc_List_List*b=_T4C;_T42=Cyc_List_fold_left;{
# 625
struct Cyc_List_List*(*_T54)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))_T42;_T41=_T54;}_T43=b;_T44=_T41(Cyc_Interface_filterstatics,0,_T43);_T45=accum;_T46=Cyc_List_append(_T44,_T45);return _T46;}case 2: goto _LL14;case 3: _LL14: goto _LL16;case 4: _LL16: goto _LL18;case 11: _LL18: goto _LL1A;case 12: _LL1A: goto _LL1C;case 13: _LL1C: goto _LL1E;case 14: _LL1E: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22: _T47=accum;
# 634
return _T47;}_LL0:;}{struct Cyc_List_List*_T4B=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T4C=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T4A=d;
# 636
*_T4C=*_T4A;_T49=(struct Cyc_Absyn_Decl*)_T4C;}_T4B->hd=_T49;_T4B->tl=accum;_T48=(struct Cyc_List_List*)_T4B;}return _T48;}struct _tuple16{struct Cyc_Interface_I*f0;int f1;struct Cyc_Tcenv_Genv*f2;struct Cyc_Interface_Seen*f3;};
# 639
static void Cyc_Interface_extract_f(struct _tuple16*env_f){struct _tuple16*_T0;struct _tuple13 _T1;struct Cyc_Interface_I*_T2;struct Cyc_Interface_I*_T3;void(*_T4)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple13*,struct Cyc_Dict_Dict);void(*_T5)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple13*_T6;struct Cyc_Tcenv_Genv*_T7;struct Cyc_Dict_Dict _T8;void(*_T9)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple13*,struct Cyc_Dict_Dict);void(*_TA)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple13*_TB;struct Cyc_Tcenv_Genv*_TC;struct Cyc_Dict_Dict _TD;void(*_TE)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple13*,struct Cyc_Dict_Dict);void(*_TF)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple13*_T10;struct Cyc_Tcenv_Genv*_T11;struct Cyc_Dict_Dict _T12;void(*_T13)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple13*,struct Cyc_Dict_Dict);void(*_T14)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple13*_T15;struct Cyc_Tcenv_Genv*_T16;struct Cyc_Dict_Dict _T17;void(*_T18)(void(*)(struct _tuple13*,struct _tuple0*,struct _tuple14*),struct _tuple13*,struct Cyc_Dict_Dict);void(*_T19)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple13*_T1A;struct Cyc_Tcenv_Genv*_T1B;struct Cyc_Dict_Dict _T1C;struct Cyc_Interface_Seen*_T1D;struct Cyc_Tcenv_Genv*_T1E;int _T1F;struct Cyc_Interface_I*_T20;_T0=env_f;{struct _tuple16 _T21=*_T0;_T20=_T21.f0;_T1F=_T21.f1;_T1E=_T21.f2;_T1D=_T21.f3;}{struct Cyc_Interface_I*i=_T20;int check_complete_defs=_T1F;struct Cyc_Tcenv_Genv*ae=_T1E;struct Cyc_Interface_Seen*seen=_T1D;{struct _tuple13 _T21;_T2=i;
# 641
_T21.f0=_T2->imports;_T3=i;_T21.f1=_T3->exports;_T21.f2=check_complete_defs;_T21.f3=ae;_T21.f4=seen;_T21.f5=i;_T1=_T21;}{struct _tuple13 env=_T1;_T5=Cyc_Dict_iter_c;{
void(*_T21)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple13*,struct Cyc_Dict_Dict))_T5;_T4=_T21;}_T6=& env;_T7=ae;_T8=_T7->aggrdecls;_T4(Cyc_Interface_extract_aggrdecl,_T6,_T8);_TA=Cyc_Dict_iter_c;{
void(*_T21)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple13*,struct Cyc_Dict_Dict))_TA;_T9=_T21;}_TB=& env;_TC=ae;_TD=_TC->datatypedecls;_T9(Cyc_Interface_extract_datatypedecl,_TB,_TD);_TF=Cyc_Dict_iter_c;{
void(*_T21)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple13*,struct Cyc_Dict_Dict))_TF;_TE=_T21;}_T10=& env;_T11=ae;_T12=_T11->enumdecls;_TE(Cyc_Interface_extract_enumdecl,_T10,_T12);_T14=Cyc_Dict_iter_c;{
void(*_T21)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple13*,struct Cyc_Dict_Dict))_T14;_T13=_T21;}_T15=& env;_T16=ae;_T17=_T16->typedefs;_T13(Cyc_Interface_extract_typedef,_T15,_T17);_T19=Cyc_Dict_iter_c;{
void(*_T21)(void(*)(struct _tuple13*,struct _tuple0*,struct _tuple14*),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct _tuple14*),struct _tuple13*,struct Cyc_Dict_Dict))_T19;_T18=_T21;}_T1A=& env;_T1B=ae;_T1C=_T1B->ordinaries;_T18(Cyc_Interface_extract_ordinarie,_T1A,_T1C);}}}
# 649
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Tcenv_Genv*ae,int check_complete_defs,struct Cyc_List_List*tds){struct _tuple16 _T0;struct _tuple16*_T1;struct _tuple16 _T2;struct Cyc_Interface_I*_T3;struct Cyc_List_List*(*_T4)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*);void*(*_T5)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Interface_I*_T8;struct Cyc_List_List*(*_T9)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*);void*(*_TA)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Interface_I*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_Interface_I*_TE;{struct _tuple16 _TF;
# 651
_TF.f0=Cyc_Interface_empty();_TF.f1=check_complete_defs;_TF.f2=ae;_TF.f3=Cyc_Interface_new_seen();_T0=_TF;}{
# 650
struct _tuple16 env=_T0;_T1=& env;
# 652
Cyc_Interface_extract_f(_T1);_T2=env;{
struct Cyc_Interface_I*i=_T2.f0;_T3=i;_T5=Cyc_List_fold_left;{
struct Cyc_List_List*(*_TF)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))_T5;_T4=_TF;}_T6=tds;_T7=_T4(Cyc_Interface_filterstatics,0,_T6);_T3->tds=Cyc_List_rev(_T7);_T8=i;_TA=Cyc_List_fold_left;{
struct Cyc_List_List*(*_TF)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))_TA;_T9=_TF;}_TB=i;_TC=_TB->tds;_TD=_T9(Cyc_Interface_uniqify_decl_list,0,_TC);_T8->tds=Cyc_List_rev(_TD);_TE=i;
return _TE;}}}
# 659
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*ae,struct Cyc_List_List*tds){struct Cyc_Interface_I*_T0;_T0=
Cyc_Interface_gen_extract(ae,1,tds);return _T0;}
# 665
static void Cyc_Interface_check_err(struct _fat_ptr*msg1,struct _fat_ptr msg2){struct Cyc_String_pa_PrintArg_struct _T0;void**_T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;{struct Cyc_String_pa_PrintArg_struct _T5;_T5.tag=0;
_T5.f1=msg2;_T0=_T5;}{struct Cyc_String_pa_PrintArg_struct _T5=_T0;void*_T6[1];_T1=_T6 + 0;*_T1=& _T5;_T2=msg1;_T3=_tag_fat("%s",sizeof(char),3U);_T4=_tag_fat(_T6,sizeof(void*),1);Cyc_Tcdecl_merr(0U,_T2,_T3,_T4);}}struct _tuple17{int f0;struct Cyc_Dict_Dict f1;int(*f2)(void*,void*,struct _fat_ptr*);struct _fat_ptr f3;struct _fat_ptr*f4;};
# 669
static void Cyc_Interface_incl_dict_f(struct _tuple17*env,struct _tuple0*x,void*y1){struct _tuple17*_T0;struct _tuple17*_T1;int*_T2;struct _handler_cons*_T3;int*_T4;int _T5;void*(*_T6)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T7)(struct Cyc_Dict_Dict,void*);int _T8;int*_T9;void*_TA;struct Cyc_Dict_Absent_exn_struct*_TB;char*_TC;char*_TD;struct _fat_ptr*_TE;struct _fat_ptr _TF;struct Cyc_String_pa_PrintArg_struct _T10;struct Cyc_String_pa_PrintArg_struct _T11;void**_T12;void**_T13;struct _fat_ptr _T14;struct _fat_ptr _T15;int*_T16;struct _fat_ptr*_T17;struct _fat_ptr _T18;int(*_T19)(void*,void*,struct _fat_ptr*);struct Cyc_Dict_Dict _T1A;void*_T1B;_T0=env;{struct _tuple17 _T1C=*_T0;_T1=env;_T2=& _T1->f0;_T1B=(int*)_T2;_T1A=_T1C.f1;_T19=_T1C.f2;_T18=_T1C.f3;_T17=_T1C.f4;}{int*res=(int*)_T1B;struct Cyc_Dict_Dict dic2=_T1A;int(*incl_f)(void*,void*,struct _fat_ptr*)=_T19;struct _fat_ptr t=_T18;struct _fat_ptr*msg=_T17;struct _handler_cons _T1C;_T3=& _T1C;_push_handler(_T3);{int _T1D=0;_T4=_T1C.handler;_T5=setjmp(_T4);if(!_T5)goto _TLFC;_T1D=1;goto _TLFD;_TLFC: _TLFD: if(_T1D)goto _TLFE;else{goto _TL100;}_TL100: _T7=Cyc_Dict_lookup;{
# 678
void*(*_T1E)(struct Cyc_Dict_Dict,struct _tuple0*)=(void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T7;_T6=_T1E;}{void*y2=_T6(dic2,x);_T8=
incl_f(y1,y2,msg);if(_T8)goto _TL101;else{goto _TL103;}_TL103: _T9=res;*_T9=0;goto _TL102;_TL101: _TL102:;}_pop_handler();goto _TLFF;_TLFE: _TA=Cyc_Core_get_exn_thrown();{void*_T1E=(void*)_TA;void*_T1F;_TB=(struct Cyc_Dict_Absent_exn_struct*)_T1E;_TC=_TB->tag;_TD=Cyc_Dict_Absent;if(_TC!=_TD)goto _TL104;_TE=msg;{struct Cyc_String_pa_PrintArg_struct _T20;_T20.tag=0;
# 681
_T20.f1=t;_T10=_T20;}{struct Cyc_String_pa_PrintArg_struct _T20=_T10;{struct Cyc_String_pa_PrintArg_struct _T21;_T21.tag=0;_T21.f1=Cyc_Absynpp_qvar2string(x);_T11=_T21;}{struct Cyc_String_pa_PrintArg_struct _T21=_T11;void*_T22[2];_T12=_T22 + 0;*_T12=& _T20;_T13=_T22 + 1;*_T13=& _T21;_T14=_tag_fat("%s %s is missing",sizeof(char),17U);_T15=_tag_fat(_T22,sizeof(void*),2);_TF=Cyc_aprintf(_T14,_T15);}}Cyc_Interface_check_err(_TE,_TF);_T16=res;
*_T16=0;goto _LL3;_TL104: _T1F=_T1E;{void*exn=_T1F;_rethrow(exn);}_LL3:;}_TLFF:;}}}
# 687
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _fat_ptr*),struct _fat_ptr t,struct _fat_ptr*msg){struct _tuple17 _T0;void(*_T1)(void(*)(struct _tuple17*,struct _tuple0*,void*),struct _tuple17*,struct Cyc_Dict_Dict);void(*_T2)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple17*_T3;struct Cyc_Dict_Dict _T4;struct _tuple17 _T5;int _T6;{struct _tuple17 _T7;
# 692
_T7.f0=1;_T7.f1=dic2;_T7.f2=incl_f;_T7.f3=t;_T7.f4=msg;_T0=_T7;}{struct _tuple17 env=_T0;_T2=Cyc_Dict_iter_c;{
void(*_T7)(void(*)(struct _tuple17*,struct _tuple0*,void*),struct _tuple17*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple17*,struct _tuple0*,void*),struct _tuple17*,struct Cyc_Dict_Dict))_T2;_T1=_T7;}_T3=& env;_T4=dic1;_T1(Cyc_Interface_incl_dict_f,_T3,_T4);_T5=env;_T6=_T5.f0;
return _T6;}}
# 709 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _fat_ptr*msg){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_Absyn_Aggrdecl*_T3;struct _tuple0*_T4;void**_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_Absyn_Aggrdecl*d=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0U,msg);if(d!=0)goto _TL106;return 0;_TL106: if(d0==d)goto _TL108;_T0=msg;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;_T3=d1;_T4=_T3->name;_T8.f1=Cyc_Absynpp_qvar2string(_T4);_T2=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T2;void*_T9[1];_T5=_T9 + 0;*_T5=& _T8;_T6=_tag_fat("declaration of type %s discloses too much information",sizeof(char),54U);_T7=_tag_fat(_T9,sizeof(void*),1);_T1=Cyc_aprintf(_T6,_T7);}Cyc_Interface_check_err(_T0,_T1);return 0;_TL108: return 1;}
# 711
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _fat_ptr*msg){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_Absyn_Datatypedecl*_T3;struct _tuple0*_T4;void**_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_Absyn_Datatypedecl*d=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0U,msg);if(d!=0)goto _TL10A;return 0;_TL10A: if(d0==d)goto _TL10C;_T0=msg;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;_T3=d1;_T4=_T3->name;_T8.f1=Cyc_Absynpp_qvar2string(_T4);_T2=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T2;void*_T9[1];_T5=_T9 + 0;*_T5=& _T8;_T6=_tag_fat("declaration of datatype %s discloses too much information",sizeof(char),58U);_T7=_tag_fat(_T9,sizeof(void*),1);_T1=Cyc_aprintf(_T6,_T7);}Cyc_Interface_check_err(_T0,_T1);return 0;_TL10C: return 1;}
# 713
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _fat_ptr*msg){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_Absyn_Enumdecl*_T3;struct _tuple0*_T4;void**_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_Absyn_Enumdecl*d=Cyc_Tcdecl_merge_enumdecl(d0,d1,0U,msg);if(d!=0)goto _TL10E;return 0;_TL10E: if(d0==d)goto _TL110;_T0=msg;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;_T3=d1;_T4=_T3->name;_T8.f1=Cyc_Absynpp_qvar2string(_T4);_T2=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T2;void*_T9[1];_T5=_T9 + 0;*_T5=& _T8;_T6=_tag_fat("declaration of enum %s discloses too much information",sizeof(char),54U);_T7=_tag_fat(_T9,sizeof(void*),1);_T1=Cyc_aprintf(_T6,_T7);}Cyc_Interface_check_err(_T0,_T1);return 0;_TL110: return 1;}
# 715
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _fat_ptr*msg){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_Absyn_Vardecl*_T3;struct _tuple0*_T4;void**_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_Absyn_Vardecl*d=Cyc_Tcdecl_merge_vardecl(d0,d1,0U,msg);if(d!=0)goto _TL112;return 0;_TL112: if(d0==d)goto _TL114;_T0=msg;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;_T3=d1;_T4=_T3->name;_T8.f1=Cyc_Absynpp_qvar2string(_T4);_T2=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T2;void*_T9[1];_T5=_T9 + 0;*_T5=& _T8;_T6=_tag_fat("declaration of variable %s discloses too much information",sizeof(char),58U);_T7=_tag_fat(_T9,sizeof(void*),1);_T1=Cyc_aprintf(_T6,_T7);}Cyc_Interface_check_err(_T0,_T1);return 0;_TL114: return 1;}
# 717
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _fat_ptr*msg){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_Absyn_Typedefdecl*_T3;struct _tuple0*_T4;void**_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_Absyn_Typedefdecl*d=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0U,msg);if(d!=0)goto _TL116;return 0;_TL116: if(d0==d)goto _TL118;_T0=msg;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;_T3=d1;_T4=_T3->name;_T8.f1=Cyc_Absynpp_qvar2string(_T4);_T2=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T2;void*_T9[1];_T5=_T9 + 0;*_T5=& _T8;_T6=_tag_fat("declaration of typedef %s discloses too much information",sizeof(char),57U);_T7=_tag_fat(_T9,sizeof(void*),1);_T1=Cyc_aprintf(_T6,_T7);}Cyc_Interface_check_err(_T0,_T1);return 0;_TL118: return 1;}
# 719
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _fat_ptr*msg){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_Tcdecl_Xdatatypefielddecl*_T3;struct Cyc_Absyn_Datatypefield*_T4;struct _tuple0*_T5;void**_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_Tcdecl_Xdatatypefielddecl*d=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0U,msg);if(d!=0)goto _TL11A;return 0;_TL11A: if(d0==d)goto _TL11C;_T0=msg;{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;_T3=d1;_T4=_T3->field;_T5=_T4->name;_T9.f1=Cyc_Absynpp_qvar2string(_T5);_T2=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T2;void*_TA[1];_T6=_TA + 0;*_T6=& _T9;_T7=_tag_fat("declaration of xdatatypefield %s discloses too much information",sizeof(char),64U);_T8=_tag_fat(_TA,sizeof(void*),1);_T1=Cyc_aprintf(_T7,_T8);}Cyc_Interface_check_err(_T0,_T1);return 0;_TL11C: return 1;}
# 724
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,void*(*)(void*,void*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);
# 733
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _fat_ptr*msg){int(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T1;struct Cyc_Dict_Dict _T2;struct Cyc_Interface_Ienv*_T3;struct Cyc_Dict_Dict _T4;struct _fat_ptr _T5;struct _fat_ptr*_T6;int(*_T7)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T8;struct Cyc_Dict_Dict _T9;struct Cyc_Interface_Ienv*_TA;struct Cyc_Dict_Dict _TB;struct _fat_ptr _TC;struct _fat_ptr*_TD;int(*_TE)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_TF;struct Cyc_Dict_Dict _T10;struct Cyc_Interface_Ienv*_T11;struct Cyc_Dict_Dict _T12;struct _fat_ptr _T13;struct _fat_ptr*_T14;struct Cyc_Dict_Dict*(*_T15)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Dict_Dict*(*_T16)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,void*(*)(void*,void*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T17;struct Cyc_Dict_Dict _T18;struct Cyc_Interface_Ienv*_T19;struct Cyc_Dict_Dict _T1A;struct Cyc_Dict_Dict(*_T1B)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T1C)(int(*)(void*,void*));int(*_T1D)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict _T1E;struct Cyc_Absyn_Typedefdecl*(*_T1F)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _T20;struct _fat_ptr*_T21;struct Cyc_Dict_Dict*_T22;int(*_T23)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T24;struct Cyc_Dict_Dict _T25;struct Cyc_Interface_Ienv*_T26;struct Cyc_Dict_Dict _T27;struct _fat_ptr _T28;struct _fat_ptr*_T29;int(*_T2A)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T2B;struct Cyc_Dict_Dict _T2C;struct Cyc_Interface_Ienv*_T2D;struct Cyc_Dict_Dict _T2E;struct _fat_ptr _T2F;struct _fat_ptr*_T30;int _T31;int _T32;int _T33;int _T34;int _T35;int _T36;{
int(*_T37)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_T0=_T37;}_T1=ie1;_T2=_T1->aggrdecls;_T3=ie2;_T4=_T3->aggrdecls;_T5=_tag_fat("type",sizeof(char),5U);_T6=msg;{int r1=_T0(_T2,_T4,Cyc_Interface_incl_aggrdecl,_T5,_T6);{
int(*_T37)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_T7=_T37;}_T8=ie1;_T9=_T8->datatypedecls;_TA=ie2;_TB=_TA->datatypedecls;_TC=_tag_fat("datatype",sizeof(char),9U);_TD=msg;{int r2=_T7(_T9,_TB,Cyc_Interface_incl_datatypedecl,_TC,_TD);{
int(*_T37)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_TE=_T37;}_TF=ie1;_T10=_TF->enumdecls;_T11=ie2;_T12=_T11->enumdecls;_T13=_tag_fat("enum",sizeof(char),5U);_T14=msg;{int r3=_TE(_T10,_T12,Cyc_Interface_incl_enumdecl,_T13,_T14);_T16=Cyc_Interface_compat_merge_dict;{
# 738
struct Cyc_Dict_Dict*(*_T37)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))_T16;_T15=_T37;}_T17=ie1;_T18=_T17->typedefdecls;_T19=ie2;_T1A=_T19->typedefdecls;_T1C=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T37)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T1C;_T1B=_T37;}_T1D=Cyc_Absyn_qvar_cmp;_T1E=_T1B(_T1D);_T1F=Cyc_Tcdecl_merge_typedefdecl;_T20=
_tag_fat("typedef",sizeof(char),8U);_T21=msg;_T22=
# 738
_T15(_T18,_T1A,_T1E,_T1F,_T20,_T21);{int r4=_T22!=0;{
# 741
int(*_T37)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_T23=_T37;}_T24=ie1;_T25=_T24->vardecls;_T26=ie2;_T27=_T26->vardecls;_T28=_tag_fat("variable",sizeof(char),9U);_T29=msg;{int r5=_T23(_T25,_T27,Cyc_Interface_incl_vardecl,_T28,_T29);{
int(*_T37)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_T2A=_T37;}_T2B=ie1;_T2C=_T2B->xdatatypefielddecls;_T2D=ie2;_T2E=_T2D->xdatatypefielddecls;_T2F=_tag_fat("xdatatypefield",sizeof(char),15U);_T30=msg;{int r6=_T2A(_T2C,_T2E,Cyc_Interface_incl_xdatatypefielddecl,_T2F,_T30);_T32=r1;
if(!_T32)goto _TL11E;_T33=r2;if(!_T33)goto _TL11E;_T34=r3;if(!_T34)goto _TL11E;_T35=r4;if(!_T35)goto _TL11E;_T36=r5;if(!_T36)goto _TL11E;_T31=r6;goto _TL11F;_TL11E: _T31=0;_TL11F: return _T31;}}}}}}}
# 746
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info){struct _handler_cons*_T0;int*_T1;int _T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct _tuple11*_T6;struct _tuple11 _T7;struct Cyc_String_pa_PrintArg_struct _T8;struct _tuple11*_T9;struct _tuple11 _TA;void**_TB;void**_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_Interface_I*_TF;struct Cyc_Interface_Ienv*_T10;struct Cyc_Interface_I*_T11;struct Cyc_Interface_Ienv*_T12;struct _fat_ptr*_T13;int _T14;struct _fat_ptr*_T15;struct _fat_ptr _T16;struct Cyc_String_pa_PrintArg_struct _T17;struct _tuple11*_T18;struct _tuple11 _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct _tuple11*_T1B;struct _tuple11 _T1C;void**_T1D;void**_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct Cyc_Interface_I*_T21;struct Cyc_Interface_Ienv*_T22;struct Cyc_Interface_I*_T23;struct Cyc_Interface_Ienv*_T24;struct _fat_ptr*_T25;int _T26;void*_T27;struct Cyc_Tcdecl_Incompatible_exn_struct*_T28;char*_T29;char*_T2A;struct _handler_cons _T2B;_T0=& _T2B;_push_handler(_T0);{int _T2C=0;_T1=_T2B.handler;_T2=setjmp(_T1);if(!_T2)goto _TL120;_T2C=1;goto _TL121;_TL120: _TL121: if(_T2C)goto _TL122;else{goto _TL124;}_TL124:{
# 748
int res=1;
struct _fat_ptr*msg=0;
# 751
if(info==0)goto _TL125;{struct _fat_ptr*_T2D=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T2E;_T2E.tag=0;_T6=info;_T7=*_T6;
_T2E.f1=_T7.f0;_T5=_T2E;}{struct Cyc_String_pa_PrintArg_struct _T2E=_T5;{struct Cyc_String_pa_PrintArg_struct _T2F;_T2F.tag=0;_T9=info;_TA=*_T9;_T2F.f1=_TA.f1;_T8=_T2F;}{struct Cyc_String_pa_PrintArg_struct _T2F=_T8;void*_T30[2];_TB=_T30 + 0;*_TB=& _T2E;_TC=_T30 + 1;*_TC=& _T2F;_TD=_tag_fat("checking inclusion of %s exports into %s exports,",sizeof(char),50U);_TE=_tag_fat(_T30,sizeof(void*),2);_T4=Cyc_aprintf(_TD,_TE);}}*_T2D=_T4;_T3=(struct _fat_ptr*)_T2D;}msg=_T3;goto _TL126;_TL125: _TL126: _TF=i1;_T10=_TF->exports;_T11=i2;_T12=_T11->exports;_T13=msg;_T14=
Cyc_Interface_incl_ienv(_T10,_T12,_T13);if(_T14)goto _TL127;else{goto _TL129;}_TL129: res=0;goto _TL128;_TL127: _TL128:
# 755
 if(info==0)goto _TL12A;{struct _fat_ptr*_T2D=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T2E;_T2E.tag=0;_T18=info;_T19=*_T18;
_T2E.f1=_T19.f1;_T17=_T2E;}{struct Cyc_String_pa_PrintArg_struct _T2E=_T17;{struct Cyc_String_pa_PrintArg_struct _T2F;_T2F.tag=0;_T1B=info;_T1C=*_T1B;_T2F.f1=_T1C.f0;_T1A=_T2F;}{struct Cyc_String_pa_PrintArg_struct _T2F=_T1A;void*_T30[2];_T1D=_T30 + 0;*_T1D=& _T2E;_T1E=_T30 + 1;*_T1E=& _T2F;_T1F=_tag_fat("checking inclusion of %s imports into %s imports,",sizeof(char),50U);_T20=_tag_fat(_T30,sizeof(void*),2);_T16=Cyc_aprintf(_T1F,_T20);}}*_T2D=_T16;_T15=(struct _fat_ptr*)_T2D;}msg=_T15;goto _TL12B;_TL12A: _TL12B: _T21=i2;_T22=_T21->imports;_T23=i1;_T24=_T23->imports;_T25=msg;_T26=
Cyc_Interface_incl_ienv(_T22,_T24,_T25);if(_T26)goto _TL12C;else{goto _TL12E;}_TL12E: res=0;goto _TL12D;_TL12C: _TL12D: {int _T2D=res;_npop_handler(0);return _T2D;}}_pop_handler();goto _TL123;_TL122: _T27=Cyc_Core_get_exn_thrown();{void*_T2D=(void*)_T27;void*_T2E;_T28=(struct Cyc_Tcdecl_Incompatible_exn_struct*)_T2D;_T29=_T28->tag;_T2A=Cyc_Tcdecl_Incompatible;if(_T29!=_T2A)goto _TL12F;
# 760
return 0;_TL12F: _T2E=_T2D;{void*exn=_T2E;_rethrow(exn);};}_TL123:;}}struct _tuple18{int f0;struct Cyc_Dict_Dict f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;void*(*f4)(void*,void*,unsigned,struct _fat_ptr*);struct _fat_ptr f5;struct _fat_ptr*f6;};
# 767
void Cyc_Interface_compat_merge_dict_f(struct _tuple18*env,struct _tuple0*x,void*y2){struct _tuple18*_T0;struct _tuple18*_T1;int*_T2;struct _tuple18*_T3;struct Cyc_Dict_Dict*_T4;struct _handler_cons*_T5;int*_T6;int _T7;void*(*_T8)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T9)(struct Cyc_Dict_Dict,void*);void*_TA;unsigned _TB;int*_TC;void*_TD;struct Cyc_Dict_Absent_exn_struct*_TE;char*_TF;char*_T10;struct _handler_cons*_T11;int*_T12;int _T13;void*(*_T14)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T15)(struct Cyc_Dict_Dict,void*);void*_T16;unsigned _T17;struct _fat_ptr*_T18;struct _fat_ptr _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct Cyc_String_pa_PrintArg_struct _T1B;void**_T1C;void**_T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;int*_T20;void*_T21;struct Cyc_Dict_Absent_exn_struct*_T22;char*_T23;char*_T24;int*_T25;int _T26;struct Cyc_Dict_Dict*_T27;struct Cyc_Dict_Dict(*_T28)(struct Cyc_Dict_Dict,struct _tuple0*,void*);struct Cyc_Dict_Dict(*_T29)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T2A;struct Cyc_Dict_Dict _T2B;struct _tuple0*_T2C;void*_T2D;struct _fat_ptr*_T2E;struct _fat_ptr _T2F;void*(*_T30)(void*,void*,unsigned,struct _fat_ptr*);struct Cyc_Dict_Dict _T31;struct Cyc_Dict_Dict _T32;void*_T33;void*_T34;_T0=env;{struct _tuple18 _T35=*_T0;_T1=env;_T2=& _T1->f0;_T34=(int*)_T2;_T3=env;_T4=& _T3->f1;_T33=(struct Cyc_Dict_Dict*)_T4;_T32=_T35.f2;_T31=_T35.f3;_T30=_T35.f4;_T2F=_T35.f5;_T2E=_T35.f6;}{int*res=(int*)_T34;struct Cyc_Dict_Dict*res_dic=(struct Cyc_Dict_Dict*)_T33;struct Cyc_Dict_Dict dic1=_T32;struct Cyc_Dict_Dict excl=_T31;void*(*merge_f)(void*,void*,unsigned,struct _fat_ptr*)=_T30;struct _fat_ptr t=_T2F;struct _fat_ptr*msg=_T2E;
# 775
void*y;{struct _handler_cons _T35;_T5=& _T35;_push_handler(_T5);{int _T36=0;_T6=_T35.handler;_T7=setjmp(_T6);if(!_T7)goto _TL131;_T36=1;goto _TL132;_TL131: _TL132: if(_T36)goto _TL133;else{goto _TL135;}_TL135: _T9=Cyc_Dict_lookup;{
# 777
void*(*_T37)(struct Cyc_Dict_Dict,struct _tuple0*)=(void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T9;_T8=_T37;}{void*y1=_T8(dic1,x);
# 780
void*yt=merge_f(y1,y2,0U,msg);_TA=yt;_TB=(unsigned)_TA;
if(_TB)goto _TL136;else{goto _TL138;}
_TL138: _TC=res;*_TC=0;_npop_handler(0);return;_TL136:
# 785
 y=yt;}_pop_handler();goto _TL134;_TL133: _TD=Cyc_Core_get_exn_thrown();{void*_T37=(void*)_TD;void*_T38;_TE=(struct Cyc_Dict_Absent_exn_struct*)_T37;_TF=_TE->tag;_T10=Cyc_Dict_Absent;if(_TF!=_T10)goto _TL139;
# 787
y=y2;goto _LL3;_TL139: _T38=_T37;{void*exn=_T38;_rethrow(exn);}_LL3:;}_TL134:;}}{struct _handler_cons _T35;_T11=& _T35;_push_handler(_T11);{int _T36=0;_T12=_T35.handler;_T13=setjmp(_T12);if(!_T13)goto _TL13B;_T36=1;goto _TL13C;_TL13B: _TL13C: if(_T36)goto _TL13D;else{goto _TL13F;}_TL13F: _T15=Cyc_Dict_lookup;{
# 792
void*(*_T37)(struct Cyc_Dict_Dict,struct _tuple0*)=(void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T15;_T14=_T37;}{void*ye=_T14(excl,x);
# 796
void*yt=merge_f(ye,y,0U,msg);
if(yt==ye)goto _TL140;_T16=yt;_T17=(unsigned)_T16;
if(!_T17)goto _TL142;_T18=msg;{struct Cyc_String_pa_PrintArg_struct _T37;_T37.tag=0;
# 800
_T37.f1=t;_T1A=_T37;}{struct Cyc_String_pa_PrintArg_struct _T37=_T1A;{struct Cyc_String_pa_PrintArg_struct _T38;_T38.tag=0;_T38.f1=Cyc_Absynpp_qvar2string(x);_T1B=_T38;}{struct Cyc_String_pa_PrintArg_struct _T38=_T1B;void*_T39[2];_T1C=_T39 + 0;*_T1C=& _T37;_T1D=_T39 + 1;*_T1D=& _T38;_T1E=
# 799
_tag_fat("abstract %s %s is being imported as non-abstract",sizeof(char),49U);_T1F=_tag_fat(_T39,sizeof(void*),2);_T19=Cyc_aprintf(_T1E,_T1F);}}Cyc_Interface_check_err(_T18,_T19);goto _TL143;_TL142: _TL143: _T20=res;
# 801
*_T20=0;goto _TL141;_TL140: _TL141:;}_pop_handler();goto _TL13E;_TL13D: _T21=Cyc_Core_get_exn_thrown();{void*_T37=(void*)_T21;void*_T38;_T22=(struct Cyc_Dict_Absent_exn_struct*)_T37;_T23=_T22->tag;_T24=Cyc_Dict_Absent;if(_T23!=_T24)goto _TL144;_T25=res;_T26=*_T25;
# 804
if(!_T26)goto _TL146;_T27=res_dic;_T29=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T39)(struct Cyc_Dict_Dict,struct _tuple0*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,void*))_T29;_T28=_T39;}_T2A=res_dic;_T2B=*_T2A;_T2C=x;_T2D=y;*_T27=_T28(_T2B,_T2C,_T2D);goto _TL147;_TL146: _TL147: goto _LL8;_TL144: _T38=_T37;{void*exn=_T38;_rethrow(exn);}_LL8:;}_TL13E:;}}}}
# 810
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned,struct _fat_ptr*),struct _fat_ptr t,struct _fat_ptr*msg){struct _tuple18 _T0;void(*_T1)(void(*)(struct _tuple18*,struct _tuple0*,void*),struct _tuple18*,struct Cyc_Dict_Dict);void(*_T2)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple18*_T3;struct Cyc_Dict_Dict _T4;struct Cyc_Dict_Dict*_T5;struct _tuple18 _T6;int _T7;struct Cyc_Dict_Dict*_T8;struct _tuple18 _T9;{struct _tuple18 _TA;
# 818
_TA.f0=1;_TA.f1=dic1;_TA.f2=dic1;_TA.f3=excl;_TA.f4=merge_f;_TA.f5=t;_TA.f6=msg;_T0=_TA;}{struct _tuple18 env=_T0;_T2=Cyc_Dict_iter_c;{
void(*_TA)(void(*)(struct _tuple18*,struct _tuple0*,void*),struct _tuple18*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple18*,struct _tuple0*,void*),struct _tuple18*,struct Cyc_Dict_Dict))_T2;_T1=_TA;}_T3=& env;_T4=dic2;_T1(Cyc_Interface_compat_merge_dict_f,_T3,_T4);_T6=env;_T7=_T6.f0;
if(!_T7)goto _TL148;{struct Cyc_Dict_Dict*_TA=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T9=env;*_TA=_T9.f1;_T8=(struct Cyc_Dict_Dict*)_TA;}_T5=_T8;goto _TL149;_TL148: _T5=0;_TL149: return _T5;}}
# 828
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _fat_ptr*msg){struct Cyc_Dict_Dict*(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T1;struct Cyc_Dict_Dict _T2;struct Cyc_Interface_Ienv*_T3;struct Cyc_Dict_Dict _T4;struct Cyc_Interface_Ienv*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_Absyn_Aggrdecl*(*_T7)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _T8;struct _fat_ptr*_T9;struct Cyc_Dict_Dict*(*_TA)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Datatypedecl*(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_TB;struct Cyc_Dict_Dict _TC;struct Cyc_Interface_Ienv*_TD;struct Cyc_Dict_Dict _TE;struct Cyc_Interface_Ienv*_TF;struct Cyc_Dict_Dict _T10;struct Cyc_Absyn_Datatypedecl*(*_T11)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _T12;struct _fat_ptr*_T13;struct Cyc_Dict_Dict*(*_T14)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T15;struct Cyc_Dict_Dict _T16;struct Cyc_Interface_Ienv*_T17;struct Cyc_Dict_Dict _T18;struct Cyc_Interface_Ienv*_T19;struct Cyc_Dict_Dict _T1A;struct Cyc_Absyn_Enumdecl*(*_T1B)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _T1C;struct _fat_ptr*_T1D;struct Cyc_Dict_Dict*(*_T1E)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T1F;struct Cyc_Dict_Dict _T20;struct Cyc_Interface_Ienv*_T21;struct Cyc_Dict_Dict _T22;struct Cyc_Interface_Ienv*_T23;struct Cyc_Dict_Dict _T24;struct Cyc_Absyn_Typedefdecl*(*_T25)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _T26;struct _fat_ptr*_T27;struct Cyc_Dict_Dict*(*_T28)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T29;struct Cyc_Dict_Dict _T2A;struct Cyc_Interface_Ienv*_T2B;struct Cyc_Dict_Dict _T2C;struct Cyc_Interface_Ienv*_T2D;struct Cyc_Dict_Dict _T2E;struct Cyc_Absyn_Vardecl*(*_T2F)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _T30;struct _fat_ptr*_T31;struct Cyc_Dict_Dict*(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Tcdecl_Xdatatypefielddecl*(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T33;struct Cyc_Dict_Dict _T34;struct Cyc_Interface_Ienv*_T35;struct Cyc_Dict_Dict _T36;struct Cyc_Interface_Ienv*_T37;struct Cyc_Dict_Dict _T38;struct Cyc_Tcdecl_Xdatatypefielddecl*(*_T39)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _T3A;struct _fat_ptr*_T3B;struct Cyc_Dict_Dict*_T3C;unsigned _T3D;struct Cyc_Dict_Dict*_T3E;unsigned _T3F;struct Cyc_Dict_Dict*_T40;unsigned _T41;struct Cyc_Dict_Dict*_T42;unsigned _T43;struct Cyc_Dict_Dict*_T44;unsigned _T45;struct Cyc_Dict_Dict*_T46;unsigned _T47;struct Cyc_Interface_Ienv*_T48;struct Cyc_Dict_Dict*_T49;struct Cyc_Dict_Dict*_T4A;struct Cyc_Dict_Dict*_T4B;struct Cyc_Dict_Dict*_T4C;struct Cyc_Dict_Dict*_T4D;struct Cyc_Dict_Dict*_T4E;{
struct Cyc_Dict_Dict*(*_T4F)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_T0=_T4F;}_T1=ie1;_T2=_T1->aggrdecls;_T3=ie2;_T4=_T3->aggrdecls;_T5=iexcl;_T6=_T5->aggrdecls;_T7=Cyc_Tcdecl_merge_aggrdecl;_T8=_tag_fat("type",sizeof(char),5U);_T9=msg;{struct Cyc_Dict_Dict*r1=_T0(_T2,_T4,_T6,_T7,_T8,_T9);{
struct Cyc_Dict_Dict*(*_T4F)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Datatypedecl*(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Datatypedecl*(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_TA=_T4F;}_TB=ie1;_TC=_TB->datatypedecls;_TD=ie2;_TE=_TD->datatypedecls;_TF=iexcl;_T10=_TF->datatypedecls;_T11=Cyc_Tcdecl_merge_datatypedecl;_T12=_tag_fat("datatype",sizeof(char),9U);_T13=msg;{struct Cyc_Dict_Dict*r2=_TA(_TC,_TE,_T10,_T11,_T12,_T13);{
struct Cyc_Dict_Dict*(*_T4F)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_T14=_T4F;}_T15=ie1;_T16=_T15->enumdecls;_T17=ie2;_T18=_T17->enumdecls;_T19=iexcl;_T1A=_T19->enumdecls;_T1B=Cyc_Tcdecl_merge_enumdecl;_T1C=_tag_fat("enum",sizeof(char),5U);_T1D=msg;{struct Cyc_Dict_Dict*r3=_T14(_T16,_T18,_T1A,_T1B,_T1C,_T1D);{
struct Cyc_Dict_Dict*(*_T4F)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_T1E=_T4F;}_T1F=ie1;_T20=_T1F->typedefdecls;_T21=ie2;_T22=_T21->typedefdecls;_T23=iexcl;_T24=_T23->typedefdecls;_T25=Cyc_Tcdecl_merge_typedefdecl;_T26=_tag_fat("typedef",sizeof(char),8U);_T27=msg;{struct Cyc_Dict_Dict*r4=_T1E(_T20,_T22,_T24,_T25,_T26,_T27);{
struct Cyc_Dict_Dict*(*_T4F)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_T28=_T4F;}_T29=ie1;_T2A=_T29->vardecls;_T2B=ie2;_T2C=_T2B->vardecls;_T2D=iexcl;_T2E=_T2D->vardecls;_T2F=Cyc_Tcdecl_merge_vardecl;_T30=_tag_fat("variable",sizeof(char),9U);_T31=msg;{struct Cyc_Dict_Dict*r5=_T28(_T2A,_T2C,_T2E,_T2F,_T30,_T31);{
struct Cyc_Dict_Dict*(*_T4F)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Tcdecl_Xdatatypefielddecl*(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Tcdecl_Xdatatypefielddecl*(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_T32=_T4F;}_T33=ie1;_T34=_T33->xdatatypefielddecls;_T35=ie2;_T36=_T35->xdatatypefielddecls;_T37=iexcl;_T38=_T37->xdatatypefielddecls;_T39=Cyc_Tcdecl_merge_xdatatypefielddecl;_T3A=_tag_fat("xdatatypefield",sizeof(char),15U);_T3B=msg;{struct Cyc_Dict_Dict*r6=_T32(_T34,_T36,_T38,_T39,_T3A,_T3B);_T3C=r1;_T3D=(unsigned)_T3C;
if(_T3D)goto _TL151;else{goto _TL14C;}_TL151: _T3E=r2;_T3F=(unsigned)_T3E;if(_T3F)goto _TL150;else{goto _TL14C;}_TL150: _T40=r3;_T41=(unsigned)_T40;if(_T41)goto _TL14F;else{goto _TL14C;}_TL14F: _T42=r4;_T43=(unsigned)_T42;if(_T43)goto _TL14E;else{goto _TL14C;}_TL14E: _T44=r5;_T45=(unsigned)_T44;if(_T45)goto _TL14D;else{goto _TL14C;}_TL14D: _T46=r6;_T47=(unsigned)_T46;if(_T47)goto _TL14A;else{goto _TL14C;}
_TL14C: return 0;_TL14A:{struct Cyc_Interface_Ienv*_T4F=_cycalloc(sizeof(struct Cyc_Interface_Ienv));_T49=r1;
_T4F->aggrdecls=*_T49;_T4A=r2;_T4F->datatypedecls=*_T4A;_T4B=r3;_T4F->enumdecls=*_T4B;_T4C=r4;_T4F->typedefdecls=*_T4C;_T4D=r5;_T4F->vardecls=*_T4D;_T4E=r6;_T4F->xdatatypefielddecls=*_T4E;_T48=(struct Cyc_Interface_Ienv*)_T4F;}return _T48;}}}}}}}struct _tuple19{int f0;struct Cyc_Dict_Dict f1;struct Cyc_Dict_Dict f2;struct _fat_ptr f3;struct _fat_ptr*f4;};
# 841
void Cyc_Interface_disj_merge_dict_f(struct _tuple19*env,struct _tuple0*x,void*y){struct _tuple19*_T0;struct _tuple19*_T1;int*_T2;struct _tuple19*_T3;struct Cyc_Dict_Dict*_T4;int(*_T5)(struct Cyc_Dict_Dict,struct _tuple0*);int(*_T6)(struct Cyc_Dict_Dict,void*);int _T7;struct _fat_ptr*_T8;struct _fat_ptr _T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc_String_pa_PrintArg_struct _TB;void**_TC;void**_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;int*_T10;int*_T11;int _T12;struct Cyc_Dict_Dict*_T13;struct Cyc_Dict_Dict(*_T14)(struct Cyc_Dict_Dict,struct _tuple0*,void*);struct Cyc_Dict_Dict(*_T15)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T16;struct Cyc_Dict_Dict _T17;struct _tuple0*_T18;void*_T19;struct _fat_ptr*_T1A;struct _fat_ptr _T1B;struct Cyc_Dict_Dict _T1C;void*_T1D;void*_T1E;_T0=env;{struct _tuple19 _T1F=*_T0;_T1=env;_T2=& _T1->f0;_T1E=(int*)_T2;_T3=env;_T4=& _T3->f1;_T1D=(struct Cyc_Dict_Dict*)_T4;_T1C=_T1F.f2;_T1B=_T1F.f3;_T1A=_T1F.f4;}{int*res=(int*)_T1E;struct Cyc_Dict_Dict*res_dic=(struct Cyc_Dict_Dict*)_T1D;struct Cyc_Dict_Dict dic1=_T1C;struct _fat_ptr t=_T1B;struct _fat_ptr*msg=_T1A;_T6=Cyc_Dict_member;{
# 845
int(*_T1F)(struct Cyc_Dict_Dict,struct _tuple0*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T6;_T5=_T1F;}_T7=_T5(dic1,x);if(!_T7)goto _TL152;_T8=msg;{struct Cyc_String_pa_PrintArg_struct _T1F;_T1F.tag=0;
_T1F.f1=t;_TA=_T1F;}{struct Cyc_String_pa_PrintArg_struct _T1F=_TA;{struct Cyc_String_pa_PrintArg_struct _T20;_T20.tag=0;
_T20.f1=Cyc_Absynpp_qvar2string(x);_TB=_T20;}{struct Cyc_String_pa_PrintArg_struct _T20=_TB;void*_T21[2];_TC=_T21 + 0;*_TC=& _T1F;_TD=_T21 + 1;*_TD=& _T20;_TE=
# 846
_tag_fat("%s %s is exported more than once",sizeof(char),33U);_TF=_tag_fat(_T21,sizeof(void*),2);_T9=Cyc_aprintf(_TE,_TF);}}Cyc_Interface_check_err(_T8,_T9);_T10=res;
# 848
*_T10=0;goto _TL153;
_TL152: _T11=res;_T12=*_T11;if(!_T12)goto _TL154;_T13=res_dic;_T15=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1F)(struct Cyc_Dict_Dict,struct _tuple0*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,void*))_T15;_T14=_T1F;}_T16=res_dic;_T17=*_T16;_T18=x;_T19=y;*_T13=_T14(_T17,_T18,_T19);goto _TL155;_TL154: _TL155: _TL153:;}}
# 853
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _fat_ptr t,struct _fat_ptr*msg){struct _tuple19 _T0;void(*_T1)(void(*)(struct _tuple19*,struct _tuple0*,void*),struct _tuple19*,struct Cyc_Dict_Dict);void(*_T2)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple19*_T3;struct Cyc_Dict_Dict _T4;struct Cyc_Dict_Dict*_T5;struct _tuple19 _T6;int _T7;struct Cyc_Dict_Dict*_T8;struct _tuple19 _T9;{struct _tuple19 _TA;
# 857
_TA.f0=1;_TA.f1=dic1;_TA.f2=dic1;_TA.f3=t;_TA.f4=msg;_T0=_TA;}{struct _tuple19 env=_T0;_T2=Cyc_Dict_iter_c;{
void(*_TA)(void(*)(struct _tuple19*,struct _tuple0*,void*),struct _tuple19*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple19*,struct _tuple0*,void*),struct _tuple19*,struct Cyc_Dict_Dict))_T2;_T1=_TA;}_T3=& env;_T4=dic2;_T1(Cyc_Interface_disj_merge_dict_f,_T3,_T4);_T6=env;_T7=_T6.f0;
if(!_T7)goto _TL156;{struct Cyc_Dict_Dict*_TA=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T9=env;*_TA=_T9.f1;_T8=(struct Cyc_Dict_Dict*)_TA;}_T5=_T8;goto _TL157;_TL156: _T5=0;_TL157: return _T5;}}
# 862
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _fat_ptr*msg){struct Cyc_Dict_Dict*(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T1;struct Cyc_Dict_Dict _T2;struct Cyc_Interface_Ienv*_T3;struct Cyc_Dict_Dict _T4;struct Cyc_Dict_Dict(*_T5)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T6)(int(*)(void*,void*));int(*_T7)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict _T8;struct Cyc_Absyn_Aggrdecl*(*_T9)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _TA;struct _fat_ptr*_TB;struct Cyc_Interface_Ienv*_TC;struct Cyc_Dict_Dict _TD;struct Cyc_Interface_Ienv*_TE;struct Cyc_Dict_Dict _TF;struct _fat_ptr _T10;struct _fat_ptr*_T11;struct Cyc_Interface_Ienv*_T12;struct Cyc_Dict_Dict _T13;struct Cyc_Interface_Ienv*_T14;struct Cyc_Dict_Dict _T15;struct _fat_ptr _T16;struct _fat_ptr*_T17;struct Cyc_Dict_Dict*(*_T18)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);struct Cyc_Interface_Ienv*_T19;struct Cyc_Dict_Dict _T1A;struct Cyc_Interface_Ienv*_T1B;struct Cyc_Dict_Dict _T1C;struct Cyc_Dict_Dict(*_T1D)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T1E)(int(*)(void*,void*));int(*_T1F)(struct _tuple0*,struct _tuple0*);struct Cyc_Dict_Dict _T20;struct Cyc_Absyn_Typedefdecl*(*_T21)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*);struct _fat_ptr _T22;struct _fat_ptr*_T23;struct Cyc_Interface_Ienv*_T24;struct Cyc_Dict_Dict _T25;struct Cyc_Interface_Ienv*_T26;struct Cyc_Dict_Dict _T27;struct _fat_ptr _T28;struct _fat_ptr*_T29;struct Cyc_Interface_Ienv*_T2A;struct Cyc_Dict_Dict _T2B;struct Cyc_Interface_Ienv*_T2C;struct Cyc_Dict_Dict _T2D;struct _fat_ptr _T2E;struct _fat_ptr*_T2F;struct Cyc_Dict_Dict*_T30;unsigned _T31;struct Cyc_Dict_Dict*_T32;unsigned _T33;struct Cyc_Dict_Dict*_T34;unsigned _T35;struct Cyc_Dict_Dict*_T36;unsigned _T37;struct Cyc_Dict_Dict*_T38;unsigned _T39;struct Cyc_Dict_Dict*_T3A;unsigned _T3B;struct Cyc_Interface_Ienv*_T3C;struct Cyc_Dict_Dict*_T3D;struct Cyc_Dict_Dict*_T3E;struct Cyc_Dict_Dict*_T3F;struct Cyc_Dict_Dict*_T40;struct Cyc_Dict_Dict*_T41;struct Cyc_Dict_Dict*_T42;{
struct Cyc_Dict_Dict*(*_T43)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_T0=_T43;}_T1=ie1;_T2=_T1->aggrdecls;_T3=ie2;_T4=_T3->aggrdecls;_T6=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T43)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T6;_T5=_T43;}_T7=Cyc_Absyn_qvar_cmp;_T8=_T5(_T7);_T9=Cyc_Tcdecl_merge_aggrdecl;_TA=
_tag_fat("type",sizeof(char),5U);_TB=msg;{
# 863
struct Cyc_Dict_Dict*r1=_T0(_T2,_T4,_T8,_T9,_TA,_TB);_TC=ie1;_TD=_TC->datatypedecls;_TE=ie2;_TF=_TE->datatypedecls;_T10=
# 866
_tag_fat("datatype",sizeof(char),9U);_T11=msg;{struct Cyc_Dict_Dict*r2=Cyc_Interface_disj_merge_dict(_TD,_TF,_T10,_T11);_T12=ie1;_T13=_T12->enumdecls;_T14=ie2;_T15=_T14->enumdecls;_T16=
_tag_fat("enum",sizeof(char),5U);_T17=msg;{struct Cyc_Dict_Dict*r3=Cyc_Interface_disj_merge_dict(_T13,_T15,_T16,_T17);{
# 869
struct Cyc_Dict_Dict*(*_T43)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_T18=_T43;}_T19=ie1;_T1A=_T19->typedefdecls;_T1B=ie2;_T1C=_T1B->typedefdecls;_T1E=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T43)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T1E;_T1D=_T43;}_T1F=Cyc_Absyn_qvar_cmp;_T20=_T1D(_T1F);_T21=Cyc_Tcdecl_merge_typedefdecl;_T22=
_tag_fat("typedef",sizeof(char),8U);_T23=msg;{
# 869
struct Cyc_Dict_Dict*r4=_T18(_T1A,_T1C,_T20,_T21,_T22,_T23);_T24=ie1;_T25=_T24->vardecls;_T26=ie2;_T27=_T26->vardecls;_T28=
# 872
_tag_fat("variable",sizeof(char),9U);_T29=msg;{struct Cyc_Dict_Dict*r5=Cyc_Interface_disj_merge_dict(_T25,_T27,_T28,_T29);_T2A=ie1;_T2B=_T2A->xdatatypefielddecls;_T2C=ie2;_T2D=_T2C->xdatatypefielddecls;_T2E=
# 874
_tag_fat("xdatatypefield",sizeof(char),15U);_T2F=msg;{
# 873
struct Cyc_Dict_Dict*r6=Cyc_Interface_disj_merge_dict(_T2B,_T2D,_T2E,_T2F);_T30=r1;_T31=(unsigned)_T30;
# 876
if(_T31)goto _TL15F;else{goto _TL15A;}_TL15F: _T32=r2;_T33=(unsigned)_T32;if(_T33)goto _TL15E;else{goto _TL15A;}_TL15E: _T34=r3;_T35=(unsigned)_T34;if(_T35)goto _TL15D;else{goto _TL15A;}_TL15D: _T36=r4;_T37=(unsigned)_T36;if(_T37)goto _TL15C;else{goto _TL15A;}_TL15C: _T38=r5;_T39=(unsigned)_T38;if(_T39)goto _TL15B;else{goto _TL15A;}_TL15B: _T3A=r6;_T3B=(unsigned)_T3A;if(_T3B)goto _TL158;else{goto _TL15A;}
_TL15A: return 0;_TL158:{struct Cyc_Interface_Ienv*_T43=_cycalloc(sizeof(struct Cyc_Interface_Ienv));_T3D=r1;
_T43->aggrdecls=*_T3D;_T3E=r2;_T43->datatypedecls=*_T3E;_T3F=r3;_T43->enumdecls=*_T3F;_T40=r4;_T43->typedefdecls=*_T40;_T41=r5;_T43->vardecls=*_T41;_T42=r6;_T43->xdatatypefielddecls=*_T42;_T3C=(struct Cyc_Interface_Ienv*)_T43;}return _T3C;}}}}}}}
# 882
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info){struct _handler_cons*_T0;int*_T1;int _T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct _tuple11*_T6;struct _tuple11 _T7;struct Cyc_String_pa_PrintArg_struct _T8;struct _tuple11*_T9;struct _tuple11 _TA;void**_TB;void**_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_Interface_I*_TF;struct Cyc_Interface_Ienv*_T10;struct Cyc_Interface_I*_T11;struct Cyc_Interface_Ienv*_T12;struct _fat_ptr*_T13;struct _fat_ptr*_T14;struct _fat_ptr _T15;struct Cyc_String_pa_PrintArg_struct _T16;struct _tuple11*_T17;struct _tuple11 _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct _tuple11*_T1A;struct _tuple11 _T1B;void**_T1C;void**_T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;struct Cyc_Interface_I*_T20;struct Cyc_Interface_Ienv*_T21;struct Cyc_Interface_I*_T22;struct Cyc_Interface_Ienv*_T23;struct Cyc_Interface_Ienv*_T24;struct _fat_ptr*_T25;struct Cyc_Interface_I*_T26;struct Cyc_List_List*_T27;struct Cyc_Absyn_Decl*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_Absyn_Decl*_T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*(*_T2D)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*);void*(*_T2E)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T2F;struct Cyc_Interface_I*_T30;struct Cyc_List_List*_T31;struct Cyc_List_List*_T32;void*_T33;struct Cyc_Tcdecl_Incompatible_exn_struct*_T34;char*_T35;char*_T36;struct _handler_cons _T37;_T0=& _T37;_push_handler(_T0);{int _T38=0;_T1=_T37.handler;_T2=setjmp(_T1);if(!_T2)goto _TL160;_T38=1;goto _TL161;_TL160: _TL161: if(_T38)goto _TL162;else{goto _TL164;}_TL164:{
# 884
struct _fat_ptr*msg=0;
# 886
if(info==0)goto _TL165;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T3A;_T3A.tag=0;_T6=info;_T7=*_T6;
# 888
_T3A.f1=_T7.f0;_T5=_T3A;}{struct Cyc_String_pa_PrintArg_struct _T3A=_T5;{struct Cyc_String_pa_PrintArg_struct _T3B;_T3B.tag=0;_T9=info;_TA=*_T9;_T3B.f1=_TA.f1;_T8=_T3B;}{struct Cyc_String_pa_PrintArg_struct _T3B=_T8;void*_T3C[2];_TB=_T3C + 0;*_TB=& _T3A;_TC=_T3C + 1;*_TC=& _T3B;_TD=
# 887
_tag_fat("merging exports of %s and %s,",sizeof(char),30U);_TE=_tag_fat(_T3C,sizeof(void*),2);_T4=Cyc_aprintf(_TD,_TE);}}*_T39=_T4;_T3=(struct _fat_ptr*)_T39;}msg=_T3;goto _TL166;_TL165: _TL166: _TF=i1;_T10=_TF->exports;_T11=i2;_T12=_T11->exports;_T13=msg;{
# 890
struct Cyc_Interface_Ienv*exp=Cyc_Interface_disj_merge_ienv(_T10,_T12,_T13);
if(exp!=0)goto _TL167;{struct Cyc_Interface_I*_T39=0;_npop_handler(0);return _T39;}_TL167:
# 893
 if(info==0)goto _TL169;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T3A;_T3A.tag=0;_T17=info;_T18=*_T17;
# 895
_T3A.f1=_T18.f0;_T16=_T3A;}{struct Cyc_String_pa_PrintArg_struct _T3A=_T16;{struct Cyc_String_pa_PrintArg_struct _T3B;_T3B.tag=0;_T1A=info;_T1B=*_T1A;_T3B.f1=_T1B.f1;_T19=_T3B;}{struct Cyc_String_pa_PrintArg_struct _T3B=_T19;void*_T3C[2];_T1C=_T3C + 0;*_T1C=& _T3A;_T1D=_T3C + 1;*_T1D=& _T3B;_T1E=
# 894
_tag_fat("merging imports of %s and %s,",sizeof(char),30U);_T1F=_tag_fat(_T3C,sizeof(void*),2);_T15=Cyc_aprintf(_T1E,_T1F);}}*_T39=_T15;_T14=(struct _fat_ptr*)_T39;}msg=_T14;goto _TL16A;_TL169: _TL16A: _T20=i1;_T21=_T20->imports;_T22=i2;_T23=_T22->imports;_T24=exp;_T25=msg;{
# 897
struct Cyc_Interface_Ienv*imp=Cyc_Interface_compat_merge_ienv(_T21,_T23,_T24,_T25);
if(imp!=0)goto _TL16B;{struct Cyc_Interface_I*_T39=0;_npop_handler(0);return _T39;}_TL16B: {
# 900
struct Cyc_List_List*newtds=0;_T26=i2;{struct Cyc_List_List*l=_T26->tds;
_TL170: if(l!=0)goto _TL16E;else{goto _TL16F;}
_TL16E:{struct Cyc_List_List*_T39=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T3A=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T29=l;_T2A=_T29->hd;_T2B=(struct Cyc_Absyn_Decl*)_T2A;*_T3A=*_T2B;_T28=(struct Cyc_Absyn_Decl*)_T3A;}_T39->hd=_T28;_T39->tl=newtds;_T27=(struct Cyc_List_List*)_T39;}newtds=_T27;_T2C=l;
# 901
l=_T2C->tl;goto _TL170;_TL16F: _T2E=Cyc_List_fold_left;{
# 903
struct Cyc_List_List*(*_T39)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))_T2E;_T2D=_T39;}_T2F=newtds;_T30=i1;_T31=_T30->tds;_T32=_T2D(Cyc_Interface_uniqify_decl_list,_T2F,_T31);newtds=Cyc_List_rev(_T32);{struct Cyc_Interface_I*_T39;_T39=_cycalloc(sizeof(struct Cyc_Interface_I));
_T39->imports=imp;_T39->exports=exp;_T39->tds=newtds;_npop_handler(0);return _T39;}}}}}}_pop_handler();goto _TL163;_TL162: _T33=Cyc_Core_get_exn_thrown();{void*_T39=(void*)_T33;void*_T3A;_T34=(struct Cyc_Tcdecl_Incompatible_exn_struct*)_T39;_T35=_T34->tag;_T36=Cyc_Tcdecl_Incompatible;if(_T35!=_T36)goto _TL171;
return 0;_TL171: _T3A=_T39;{void*exn=_T3A;_rethrow(exn);};}_TL163:;}}
# 908
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){struct Cyc_Interface_I*_T0;struct Cyc_List_List*_T1;void*_T2;struct _fat_ptr*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Interface_I*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_Interface_I*_TB;struct _tuple11*_TC;struct _tuple11*_TD;struct _fat_ptr*_TE;struct Cyc_List_List*_TF;void*_T10;struct _fat_ptr*_T11;struct _fat_ptr*_T12;struct _fat_ptr*_T13;struct _fat_ptr _T14;struct Cyc_String_pa_PrintArg_struct _T15;struct _fat_ptr*_T16;struct Cyc_String_pa_PrintArg_struct _T17;struct Cyc_List_List*_T18;void*_T19;struct _fat_ptr*_T1A;void**_T1B;void**_T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_Interface_I*_T21;
if(li!=0)goto _TL173;_T0=Cyc_Interface_empty();return _T0;_TL173: _T1=li;_T2=_T1->hd;{
# 911
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)_T2;
if(linfo==0)goto _TL175;_T4=linfo;_T5=_T4->hd;_T3=(struct _fat_ptr*)_T5;goto _TL176;_TL175: _T3=0;_TL176: {struct _fat_ptr*curr_info=_T3;_T6=li;
li=_T6->tl;
if(linfo==0)goto _TL177;_T7=linfo;linfo=_T7->tl;goto _TL178;_TL177: _TL178:
# 916
 _TL17C: if(li!=0)goto _TL17A;else{goto _TL17B;}
_TL17A: _T8=curr_i;_T9=li;_TA=_T9->hd;_TB=(struct Cyc_Interface_I*)_TA;
if(curr_info==0)goto _TL17D;if(linfo==0)goto _TL17D;{struct _tuple11*_T22=_cycalloc(sizeof(struct _tuple11));_TE=curr_info;_T22->f0=*_TE;_TF=linfo;_T10=_TF->hd;_T11=(struct _fat_ptr*)_T10;_T22->f1=*_T11;_TD=(struct _tuple11*)_T22;}_TC=_TD;goto _TL17E;_TL17D: _TC=0;_TL17E:{
# 917
struct Cyc_Interface_I*i=Cyc_Interface_merge(_T8,_TB,_TC);
# 919
if(i!=0)goto _TL17F;return 0;_TL17F:
 curr_i=i;
if(curr_info==0)goto _TL181;
if(linfo==0)goto _TL183;{struct _fat_ptr*_T22=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T23;_T23.tag=0;_T16=curr_info;_T23.f1=*_T16;_T15=_T23;}{struct Cyc_String_pa_PrintArg_struct _T23=_T15;{struct Cyc_String_pa_PrintArg_struct _T24;_T24.tag=0;_T18=linfo;_T19=_T18->hd;_T1A=(struct _fat_ptr*)_T19;_T24.f1=*_T1A;_T17=_T24;}{struct Cyc_String_pa_PrintArg_struct _T24=_T17;void*_T25[2];_T1B=_T25 + 0;*_T1B=& _T23;_T1C=_T25 + 1;*_T1C=& _T24;_T1D=_tag_fat("%s+%s",sizeof(char),6U);_T1E=_tag_fat(_T25,sizeof(void*),2);_T14=Cyc_aprintf(_T1D,_T1E);}}*_T22=_T14;_T13=(struct _fat_ptr*)_T22;}_T12=_T13;goto _TL184;_TL183: _T12=0;_TL184: curr_info=_T12;goto _TL182;_TL181: _TL182:
 if(linfo==0)goto _TL185;_T1F=linfo;linfo=_T1F->tl;goto _TL186;_TL185: _TL186:;}_T20=li;
# 916
li=_T20->tl;goto _TL17C;_TL17B: _T21=curr_i;
# 925
return _T21;}}}
# 928
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){struct Cyc_Interface_I*_T0;struct Cyc_List_List*_T1;void*_T2;struct _fat_ptr*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Interface_I*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_Interface_I*_TB;struct _tuple11*_TC;struct _tuple11*_TD;struct _fat_ptr*_TE;struct Cyc_List_List*_TF;void*_T10;struct _fat_ptr*_T11;struct _fat_ptr*_T12;struct _fat_ptr*_T13;struct _fat_ptr _T14;struct Cyc_String_pa_PrintArg_struct _T15;struct _fat_ptr*_T16;struct Cyc_String_pa_PrintArg_struct _T17;struct Cyc_List_List*_T18;void*_T19;struct _fat_ptr*_T1A;void**_T1B;void**_T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_Interface_I*_T21;
if(la!=0)goto _TL187;_T0=Cyc_Interface_empty();return _T0;_TL187: _T1=la;_T2=_T1->hd;{
# 931
struct Cyc_Interface_I*curr_i=get(_T2);
if(linfo==0)goto _TL189;_T4=linfo;_T5=_T4->hd;_T3=(struct _fat_ptr*)_T5;goto _TL18A;_TL189: _T3=0;_TL18A: {struct _fat_ptr*curr_info=_T3;_T6=la;
la=_T6->tl;
if(linfo==0)goto _TL18B;_T7=linfo;linfo=_T7->tl;goto _TL18C;_TL18B: _TL18C:
# 936
 _TL190: if(la!=0)goto _TL18E;else{goto _TL18F;}
_TL18E: _T8=curr_i;_T9=la;_TA=_T9->hd;_TB=get(_TA);
if(curr_info==0)goto _TL191;if(linfo==0)goto _TL191;{struct _tuple11*_T22=_cycalloc(sizeof(struct _tuple11));_TE=curr_info;_T22->f0=*_TE;_TF=linfo;_T10=_TF->hd;_T11=(struct _fat_ptr*)_T10;_T22->f1=*_T11;_TD=(struct _tuple11*)_T22;}_TC=_TD;goto _TL192;_TL191: _TC=0;_TL192:{
# 937
struct Cyc_Interface_I*i=Cyc_Interface_merge(_T8,_TB,_TC);
# 939
if(i!=0)goto _TL193;return 0;_TL193:
 curr_i=i;
if(curr_info==0)goto _TL195;
if(linfo==0)goto _TL197;{struct _fat_ptr*_T22=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T23;_T23.tag=0;_T16=curr_info;_T23.f1=*_T16;_T15=_T23;}{struct Cyc_String_pa_PrintArg_struct _T23=_T15;{struct Cyc_String_pa_PrintArg_struct _T24;_T24.tag=0;_T18=linfo;_T19=_T18->hd;_T1A=(struct _fat_ptr*)_T19;_T24.f1=*_T1A;_T17=_T24;}{struct Cyc_String_pa_PrintArg_struct _T24=_T17;void*_T25[2];_T1B=_T25 + 0;*_T1B=& _T23;_T1C=_T25 + 1;*_T1C=& _T24;_T1D=_tag_fat("%s+%s",sizeof(char),6U);_T1E=_tag_fat(_T25,sizeof(void*),2);_T14=Cyc_aprintf(_T1D,_T1E);}}*_T22=_T14;_T13=(struct _fat_ptr*)_T22;}_T12=_T13;goto _TL198;_TL197: _T12=0;_TL198: curr_info=_T12;goto _TL196;_TL195: _TL196:
# 944
 if(linfo==0)goto _TL199;_T1F=linfo;linfo=_T1F->tl;goto _TL19A;_TL199: _TL19A:;}_T20=la;
# 936
la=_T20->tl;goto _TL190;_TL18F: _T21=curr_i;
# 946
return _T21;}}}
# 952
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_Absyn_Fndecl*_T6;struct Cyc_Absyn_Aggrdecl*_T7;struct Cyc_Absyn_Datatypedecl*_T8;struct Cyc_Absyn_Enumdecl*_T9;struct Cyc_Absyn_Typedefdecl*_TA;struct _fat_ptr _TB;struct _tuple0*_TC;struct _tuple0 _TD;struct _union_Nmspace_C_n _TE;unsigned _TF;struct _union_Nmspace_Abs_n _T10;struct _union_Nmspace_C_n _T11;struct _fat_ptr _T12;struct _tuple0*_T13;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_List_List*_T17;void*_T18;struct Cyc_Absyn_Decl*_T19;unsigned _T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;
struct Cyc_List_List*ans=0;
_TL19E: if(tds!=0)goto _TL19C;else{goto _TL19D;}
_TL19C:{struct _tuple0*qv;_T0=tds;_T1=_T0->hd;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_T1;_T2=d;{
void*_T1F=_T2->r;struct Cyc_Absyn_Typedefdecl*_T20;struct Cyc_Absyn_Enumdecl*_T21;struct Cyc_Absyn_Datatypedecl*_T22;struct Cyc_Absyn_Aggrdecl*_T23;struct Cyc_Absyn_Fndecl*_T24;struct Cyc_Absyn_Vardecl*_T25;_T3=(int*)_T1F;_T4=*_T3;switch(_T4){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T26=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1F;_T25=_T26->f1;}{struct Cyc_Absyn_Vardecl*vd=_T25;_T5=vd;
qv=_T5->name;goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T26=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1F;_T24=_T26->f1;}{struct Cyc_Absyn_Fndecl*fd=_T24;_T6=fd;
qv=_T6->name;goto _LL0;}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T26=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T1F;_T23=_T26->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T23;_T7=ad;
qv=_T7->name;goto _LL0;}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T26=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T1F;_T22=_T26->f1;}{struct Cyc_Absyn_Datatypedecl*dd=_T22;_T8=dd;
qv=_T8->name;goto _LL0;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T26=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T1F;_T21=_T26->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T21;_T9=ed;
qv=_T9->name;goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T26=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T1F;_T20=_T26->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T20;_TA=td;
qv=_TA->name;goto _LL0;}default: _TB=
_tag_fat("bad decl form in Interface::add_namespace",sizeof(char),42U);Cyc_Interface_err(_TB);return 0;}_LL0:;}{
# 966
struct Cyc_List_List*vs;_TC=qv;_TD=*_TC;{
union Cyc_Absyn_Nmspace _T1F=_TD.f0;struct Cyc_List_List*_T20;_TE=_T1F.C_n;_TF=_TE.tag;switch(_TF){case 1: _T10=_T1F.Abs_n;_T20=_T10.val;{struct Cyc_List_List*x=_T20;
vs=x;goto _LLF;}case 3: _T11=_T1F.C_n;_T20=_T11.val;{struct Cyc_List_List*x=_T20;
vs=x;goto _LLF;}default: _T12=
_tag_fat("bad namespace in Interface::add_namespace",sizeof(char),42U);Cyc_Interface_err(_T12);return 0;}_LLF:;}
# 972
vs=Cyc_List_imp_rev(vs);_T13=qv;
(*_T13).f0=Cyc_Absyn_Rel_n(0);
_TL1A4: if(vs!=0)goto _TL1A2;else{goto _TL1A3;}
_TL1A2:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1F=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T1F->tag=9;_T15=vs;_T16=_T15->hd;_T1F->f1=(struct _fat_ptr*)_T16;{struct Cyc_List_List*_T20=_cycalloc(sizeof(struct Cyc_List_List));_T20->hd=d;_T20->tl=0;_T17=(struct Cyc_List_List*)_T20;}_T1F->f2=_T17;_T14=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1F;}_T18=(void*)_T14;_T19=d;_T1A=_T19->loc;d=Cyc_Absyn_new_decl(_T18,_T1A);_T1B=vs;
# 974
vs=_T1B->tl;goto _TL1A4;_TL1A3:{struct Cyc_List_List*_T1F=_cycalloc(sizeof(struct Cyc_List_List));
# 976
_T1F->hd=d;_T1F->tl=ans;_T1C=(struct Cyc_List_List*)_T1F;}ans=_T1C;}}}_T1D=tds;
# 954
tds=_T1D->tl;goto _TL19E;_TL19D: _T1E=
# 978
Cyc_List_imp_rev(ans);return _T1E;}
# 981
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T1;void*_T2;{struct Cyc_List_List*_T3=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T4=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T4->tag=5;
_T4->f1=d;_T1=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T4;}_T2=(void*)_T1;_T3->hd=Cyc_Absyn_new_decl(_T2,0U);_T3->tl=tds;_T0=(struct Cyc_List_List*)_T3;}return _T0;}
# 985
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){struct Cyc_Absyn_Aggrdecl*_T0;struct Cyc_Absyn_Aggrdecl*_T1;struct Cyc_Absyn_Aggrdecl*_T2;struct Cyc_Absyn_Aggrdecl*_T3;enum Cyc_Absyn_Scope _T4;int _T5;struct Cyc_Absyn_Aggrdecl*_T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T8;void*_T9;{struct Cyc_Absyn_Aggrdecl*_TA=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T1=d;
# 987
*_TA=*_T1;_T0=(struct Cyc_Absyn_Aggrdecl*)_TA;}d=_T0;_T2=d;
_T2->impl=0;_T3=d;_T4=_T3->sc;_T5=(int)_T4;
if(_T5==4)goto _TL1A5;_T6=d;_T6->sc=3U;goto _TL1A6;_TL1A5: _TL1A6:{struct Cyc_List_List*_TA=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TB->tag=5;
_TB->f1=d;_T8=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_TB;}_T9=(void*)_T8;_TA->hd=Cyc_Absyn_new_decl(_T9,0U);_TA->tl=tds;_T7=(struct Cyc_List_List*)_TA;}return _T7;}
# 993
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T1;void*_T2;{struct Cyc_List_List*_T3=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T4=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_T4->tag=6;
_T4->f1=d;_T1=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T4;}_T2=(void*)_T1;_T3->hd=Cyc_Absyn_new_decl(_T2,0U);_T3->tl=tds;_T0=(struct Cyc_List_List*)_T3;}return _T0;}static char _TmpG0[2U]="_";
# 997
static struct _fat_ptr Cyc_Interface_us={_TmpG0,_TmpG0,_TmpG0 + 2U};
static struct _fat_ptr*Cyc_Interface_us_p=(struct _fat_ptr*)& Cyc_Interface_us;
# 1000
static struct _tuple12*Cyc_Interface_rewrite_datatypefield_type(struct _tuple12*x){struct _tuple12*_T0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T1;struct Cyc_Absyn_Tvar*_T2;struct Cyc_Absyn_Kind*_T3;struct Cyc_Absyn_Kind*_T4;{struct _tuple12*_T5=_cycalloc(sizeof(struct _tuple12));
_T5->f0=Cyc_Absyn_empty_tqual(0U);{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T6=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T6->tag=2;{struct Cyc_Absyn_Tvar*_T7=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_T7->name=Cyc_Interface_us_p;_T7->identity=-1;_T3=& Cyc_Kinds_bk;_T4=(struct Cyc_Absyn_Kind*)_T3;_T7->kind=Cyc_Kinds_kind_to_bound(_T4);_T7->aquals_bound=0;_T2=(struct Cyc_Absyn_Tvar*)_T7;}_T6->f1=_T2;_T1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T6;}_T5->f1=(void*)_T1;_T0=(struct _tuple12*)_T5;}
# 1001
return _T0;}
# 1005
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){struct Cyc_Absyn_Datatypefield*_T0;struct Cyc_Absyn_Datatypefield*_T1;struct Cyc_Absyn_Datatypefield*_T2;struct Cyc_List_List*(*_T3)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_List_List*(*_T4)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Datatypefield*_T5;struct Cyc_List_List*_T6;struct Cyc_Absyn_Datatypefield*_T7;{struct Cyc_Absyn_Datatypefield*_T8=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T1=f;
*_T8=*_T1;_T0=(struct Cyc_Absyn_Datatypefield*)_T8;}f=_T0;_T2=f;_T4=Cyc_List_map;{
struct Cyc_List_List*(*_T8)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*))_T4;_T3=_T8;}_T5=f;_T6=_T5->typs;_T2->typs=_T3(Cyc_Interface_rewrite_datatypefield_type,_T6);_T7=f;
return _T7;}
# 1011
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){struct Cyc_Absyn_Datatypedecl*_T0;struct Cyc_Absyn_Datatypedecl*_T1;struct Cyc_Absyn_Datatypedecl*_T2;struct Cyc_Core_Opt*_T3;struct Cyc_Absyn_Datatypedecl*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_List_List*(*_T6)(struct Cyc_Absyn_Datatypefield*(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*);struct Cyc_List_List*(*_T7)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Datatypedecl*_T8;struct Cyc_Core_Opt*_T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_Absyn_Datatypedecl*_TC;enum Cyc_Absyn_Scope _TD;int _TE;struct Cyc_Absyn_Datatypedecl*_TF;struct Cyc_List_List*_T10;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T11;void*_T12;{struct Cyc_Absyn_Datatypedecl*_T13=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T1=d;
# 1013
*_T13=*_T1;_T0=(struct Cyc_Absyn_Datatypedecl*)_T13;}d=_T0;_T2=d;_T3=_T2->fields;
# 1015
if(_T3==0)goto _TL1A7;_T4=d;{struct Cyc_Core_Opt*_T13=_cycalloc(sizeof(struct Cyc_Core_Opt));_T7=Cyc_List_map;{struct Cyc_List_List*(*_T14)(struct Cyc_Absyn_Datatypefield*(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*))_T7;_T6=_T14;}_T8=d;_T9=_T8->fields;_TA=_T9->v;_TB=(struct Cyc_List_List*)_TA;_T13->v=_T6(Cyc_Interface_rewrite_datatypefield,_TB);_T5=(struct Cyc_Core_Opt*)_T13;}_T4->fields=_T5;goto _TL1A8;_TL1A7: _TL1A8: _TC=d;_TD=_TC->sc;_TE=(int)_TD;
if(_TE==4)goto _TL1A9;_TF=d;_TF->sc=3U;goto _TL1AA;_TL1A9: _TL1AA:{struct Cyc_List_List*_T13=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T14=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_T14->tag=6;
_T14->f1=d;_T11=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T14;}_T12=(void*)_T11;_T13->hd=Cyc_Absyn_new_decl(_T12,0U);_T13->tl=tds;_T10=(struct Cyc_List_List*)_T13;}return _T10;}
# 1020
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T1;void*_T2;{struct Cyc_List_List*_T3=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T4=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_T4->tag=7;
_T4->f1=d;_T1=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T4;}_T2=(void*)_T1;_T3->hd=Cyc_Absyn_new_decl(_T2,0U);_T3->tl=tds;_T0=(struct Cyc_List_List*)_T3;}return _T0;}
# 1024
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){struct Cyc_Absyn_Enumdecl*_T0;struct Cyc_Absyn_Enumdecl*_T1;struct Cyc_Absyn_Enumdecl*_T2;struct Cyc_Absyn_Enumdecl*_T3;enum Cyc_Absyn_Scope _T4;int _T5;struct Cyc_Absyn_Enumdecl*_T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T8;void*_T9;{struct Cyc_Absyn_Enumdecl*_TA=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T1=d;
# 1026
*_TA=*_T1;_T0=(struct Cyc_Absyn_Enumdecl*)_TA;}d=_T0;_T2=d;
_T2->fields=0;_T3=d;_T4=_T3->sc;_T5=(int)_T4;
if(_T5==4)goto _TL1AB;_T6=d;_T6->sc=3U;goto _TL1AC;_TL1AB: _TL1AC:{struct Cyc_List_List*_TA=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TB=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TB->tag=7;
_TB->f1=d;_T8=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TB;}_T9=(void*)_T8;_TA->hd=Cyc_Absyn_new_decl(_T9,0U);_TA->tl=tds;_T7=(struct Cyc_List_List*)_TA;}return _T7;}
# 1032
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T1;void*_T2;{struct Cyc_List_List*_T3=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T4=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_T4->tag=8;
# 1034
_T4->f1=d;_T1=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T4;}_T2=(void*)_T1;_T3->hd=Cyc_Absyn_new_decl(_T2,0U);_T3->tl=tds;_T0=(struct Cyc_List_List*)_T3;}return _T0;}
# 1037
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _tuple0*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T1;void*_T2;{struct Cyc_List_List*_T3=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T4->tag=0;
_T4->f1=d;_T1=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T4;}_T2=(void*)_T1;_T3->hd=Cyc_Absyn_new_decl(_T2,0U);_T3->tl=tds;_T0=(struct Cyc_List_List*)_T3;}return _T0;}
# 1041
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){struct Cyc_Tcdecl_Xdatatypefielddecl*_T0;struct Cyc_Absyn_Datatypedecl*_T1;struct Cyc_Absyn_Datatypedecl*_T2;struct Cyc_Absyn_Datatypedecl*_T3;struct Cyc_Core_Opt*_T4;struct Cyc_List_List*_T5;struct Cyc_Absyn_Datatypedecl*_T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T8;void*_T9;struct Cyc_Absyn_Datatypefield*_TA;struct Cyc_Absyn_Datatypedecl*_TB;_T0=d;{struct Cyc_Tcdecl_Xdatatypefielddecl _TC=*_T0;_TB=_TC.base;_TA=_TC.field;}{struct Cyc_Absyn_Datatypedecl*b=_TB;struct Cyc_Absyn_Datatypefield*f=_TA;{struct Cyc_Absyn_Datatypedecl*_TC=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T2=b;
# 1044
*_TC=*_T2;_T1=(struct Cyc_Absyn_Datatypedecl*)_TC;}b=_T1;_T3=b;{struct Cyc_Core_Opt*_TC=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));
_TD->hd=f;_TD->tl=0;_T5=(struct Cyc_List_List*)_TD;}_TC->v=_T5;_T4=(struct Cyc_Core_Opt*)_TC;}_T3->fields=_T4;_T6=b;
_T6->sc=3U;{struct Cyc_List_List*_TC=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TD=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TD->tag=6;
_TD->f1=b;_T8=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TD;}_T9=(void*)_T8;_TC->hd=Cyc_Absyn_new_decl(_T9,0U);_TC->tl=tds;_T7=(struct Cyc_List_List*)_TC;}return _T7;}}
# 1050
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){struct Cyc_Tcdecl_Xdatatypefielddecl*_T0;struct Cyc_Absyn_Datatypedecl*_T1;struct Cyc_Absyn_Datatypedecl*_T2;struct Cyc_Absyn_Datatypefield*_T3;struct Cyc_Absyn_Datatypedecl*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_List_List*_T6;struct Cyc_Absyn_Datatypedecl*_T7;struct Cyc_List_List*_T8;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T9;void*_TA;struct Cyc_Absyn_Datatypefield*_TB;struct Cyc_Absyn_Datatypedecl*_TC;_T0=d;{struct Cyc_Tcdecl_Xdatatypefielddecl _TD=*_T0;_TC=_TD.base;_TB=_TD.field;}{struct Cyc_Absyn_Datatypedecl*b=_TC;struct Cyc_Absyn_Datatypefield*f=_TB;{struct Cyc_Absyn_Datatypedecl*_TD=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T2=b;
# 1053
*_TD=*_T2;_T1=(struct Cyc_Absyn_Datatypedecl*)_TD;}b=_T1;
f=Cyc_Interface_rewrite_datatypefield(f);_T3=f;
_T3->sc=3U;_T4=b;{struct Cyc_Core_Opt*_TD=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_List_List*_TE=_cycalloc(sizeof(struct Cyc_List_List));
_TE->hd=f;_TE->tl=0;_T6=(struct Cyc_List_List*)_TE;}_TD->v=_T6;_T5=(struct Cyc_Core_Opt*)_TD;}_T4->fields=_T5;_T7=b;
_T7->sc=3U;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TE=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TE->tag=6;
_TE->f1=b;_T9=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TE;}_TA=(void*)_T9;_TD->hd=Cyc_Absyn_new_decl(_TA,0U);_TD->tl=tds;_T8=(struct Cyc_List_List*)_TD;}return _T8;}}
# 1061
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*(*_T0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_T2;struct Cyc_Dict_Dict _T3;struct Cyc_List_List*(*_T4)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T5)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_T6;struct Cyc_Dict_Dict _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*(*_T9)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_TA)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_TB;struct Cyc_Dict_Dict _TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc___cycFILE*_T10;_T1=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T11)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1;_T0=_T11;}_T2=ie;_T3=_T2->aggrdecls;{struct Cyc_List_List*tds=_T0(Cyc_Interface_add_aggrdecl_header,_T3,0);_T5=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T11)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_T5;_T4=_T11;}_T6=ie;_T7=_T6->datatypedecls;_T8=tds;tds=_T4(Cyc_Interface_add_datatypedecl_header,_T7,_T8);_TA=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T11)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_TA;_T9=_T11;}_TB=ie;_TC=_TB->enumdecls;_TD=tds;tds=_T9(Cyc_Interface_add_enumdecl_header,_TC,_TD);
if(tds==0)goto _TL1AD;_TE=
Cyc_List_imp_rev(tds);_TF=Cyc_Interface_add_namespace(_TE);_T10=f;Cyc_Absynpp_decllist2file(_TF,_T10);goto _TL1AE;_TL1AD: _TL1AE:;}}
# 1069
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*(*_T0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_T2;struct Cyc_Dict_Dict _T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc___cycFILE*_T6;_T1=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T7)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1;_T0=_T7;}_T2=ie;_T3=_T2->xdatatypefielddecls;{struct Cyc_List_List*tds=_T0(Cyc_Interface_add_xdatatypefielddecl_header,_T3,0);
if(tds==0)goto _TL1AF;_T4=
Cyc_List_imp_rev(tds);_T5=Cyc_Interface_add_namespace(_T4);_T6=f;Cyc_Absynpp_decllist2file(_T5,_T6);goto _TL1B0;_TL1AF: _TL1B0:;}}
# 1075
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*(*_T0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_T2;struct Cyc_Dict_Dict _T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc___cycFILE*_T6;_T1=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T7)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1;_T0=_T7;}_T2=ie;_T3=_T2->typedefdecls;{struct Cyc_List_List*tds=_T0(Cyc_Interface_add_typedef,_T3,0);
if(tds==0)goto _TL1B1;_T4=
Cyc_List_imp_rev(tds);_T5=Cyc_Interface_add_namespace(_T4);_T6=f;Cyc_Absynpp_decllist2file(_T5,_T6);goto _TL1B2;_TL1B1: _TL1B2:;}}
# 1081
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*(*_T0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_T2;struct Cyc_Dict_Dict _T3;struct Cyc_List_List*(*_T4)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T5)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_T6;struct Cyc_Dict_Dict _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*(*_T9)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_TA)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_TB;struct Cyc_Dict_Dict _TC;struct Cyc_List_List*_TD;struct Cyc_List_List*(*_TE)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_TF)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_T10;struct Cyc_Dict_Dict _T11;struct Cyc_List_List*_T12;struct Cyc_List_List*(*_T13)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T14)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Interface_Ienv*_T15;struct Cyc_Dict_Dict _T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;struct Cyc___cycFILE*_T1A;_T1=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T1B)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1;_T0=_T1B;}_T2=ie;_T3=_T2->aggrdecls;{struct Cyc_List_List*tds=_T0(Cyc_Interface_add_aggrdecl,_T3,0);_T5=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T1B)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_T5;_T4=_T1B;}_T6=ie;_T7=_T6->datatypedecls;_T8=tds;tds=_T4(Cyc_Interface_add_datatypedecl,_T7,_T8);_TA=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T1B)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_TA;_T9=_T1B;}_TB=ie;_TC=_TB->enumdecls;_TD=tds;tds=_T9(Cyc_Interface_add_enumdecl,_TC,_TD);_TF=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T1B)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_TF;_TE=_T1B;}_T10=ie;_T11=_T10->vardecls;_T12=tds;tds=_TE(Cyc_Interface_add_vardecl,_T11,_T12);_T14=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T1B)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_T14;_T13=_T1B;}_T15=ie;_T16=_T15->xdatatypefielddecls;_T17=tds;tds=_T13(Cyc_Interface_add_xdatatypefielddecl,_T16,_T17);
if(tds==0)goto _TL1B3;_T18=
Cyc_List_imp_rev(tds);_T19=Cyc_Interface_add_namespace(_T18);_T1A=f;Cyc_Absynpp_decllist2file(_T19,_T1A);goto _TL1B4;_TL1B3: _TL1B4:;}}
# 1094
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){struct Cyc_Absynpp_Params*_T0;struct Cyc_Absynpp_Params*_T1;struct Cyc___cycFILE*_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc___cycFILE*_T5;struct Cyc_Interface_I*_T6;struct Cyc_Interface_Ienv*_T7;struct Cyc___cycFILE*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct Cyc___cycFILE*_TB;struct Cyc_Interface_I*_TC;struct Cyc_Interface_Ienv*_TD;struct Cyc___cycFILE*_TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc___cycFILE*_T11;struct Cyc_Interface_I*_T12;struct Cyc_Interface_Ienv*_T13;struct Cyc___cycFILE*_T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc___cycFILE*_T17;struct Cyc_Interface_I*_T18;struct Cyc_Interface_Ienv*_T19;struct Cyc___cycFILE*_T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;struct Cyc___cycFILE*_T1D;struct Cyc_Interface_I*_T1E;struct Cyc_Interface_Ienv*_T1F;struct Cyc___cycFILE*_T20;struct _fat_ptr _T21;struct _fat_ptr _T22;struct Cyc___cycFILE*_T23;struct Cyc_Interface_I*_T24;struct Cyc_Interface_Ienv*_T25;struct Cyc___cycFILE*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc___cycFILE*_T29;struct Cyc_Interface_I*_T2A;struct Cyc_Interface_Ienv*_T2B;_T0=& Cyc_Absynpp_cyci_params_r;_T1=(struct Cyc_Absynpp_Params*)_T0;
Cyc_Absynpp_set_params(_T1);_T2=f;_T3=
_tag_fat("/****** needed (headers) ******/\n",sizeof(char),34U);_T4=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2,_T3,_T4);_T5=f;_T6=i;_T7=_T6->imports;
Cyc_Interface_print_ns_headers(_T5,_T7);_T8=f;_T9=
# 1099
_tag_fat("\n/****** provided (headers) ******/\n",sizeof(char),37U);_TA=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T8,_T9,_TA);_TB=f;_TC=i;_TD=_TC->exports;
Cyc_Interface_print_ns_headers(_TB,_TD);_TE=f;_TF=
# 1104
_tag_fat("\n/****** needed (headers of xdatatypefielddecls) ******/\n",sizeof(char),58U);_T10=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TE,_TF,_T10);_T11=f;_T12=i;_T13=_T12->imports;
Cyc_Interface_print_ns_xdatatypefielddecl_headers(_T11,_T13);_T14=f;_T15=
# 1107
_tag_fat("\n/****** provided (headers of xdatatypefielddecls) ******/\n",sizeof(char),60U);_T16=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T14,_T15,_T16);_T17=f;_T18=i;_T19=_T18->exports;
Cyc_Interface_print_ns_xdatatypefielddecl_headers(_T17,_T19);_T1A=f;_T1B=
# 1111
_tag_fat("\n/****** provided (typedefs) ******/\n",sizeof(char),38U);_T1C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1A,_T1B,_T1C);_T1D=f;_T1E=i;_T1F=_T1E->exports;
Cyc_Interface_print_ns_typedefs(_T1D,_T1F);_T20=f;_T21=
# 1114
_tag_fat("\n/****** needed (declarations) ******/\n",sizeof(char),40U);_T22=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T20,_T21,_T22);_T23=f;_T24=i;_T25=_T24->imports;
Cyc_Interface_print_ns_decls(_T23,_T25);_T26=f;_T27=
# 1117
_tag_fat("\n/****** provided (declarations) ******/\n",sizeof(char),42U);_T28=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T26,_T27,_T28);_T29=f;_T2A=i;_T2B=_T2A->exports;
Cyc_Interface_print_ns_decls(_T29,_T2B);}
# 1124
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Genv*_T1;struct Cyc_List_List*_T2;struct Cyc_Interface_I*_T3;
Cyc_Lex_lex_init();{
struct Cyc_List_List*tds=Cyc_Parse_parse_file(f);
Cyc_Binding_resolve_all(tds);{
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_tc_init();
Cyc_Tc_tc(te,0,tds);_T0=te;_T1=_T0->ae;_T2=tds;_T3=
Cyc_Interface_gen_extract(_T1,0,_T2);return _T3;}}}
# 1137
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1143
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){struct Cyc_Interface_I*_T0;_T0=
Cyc_Interface_parse(f);return _T0;}

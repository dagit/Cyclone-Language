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
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
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
extern void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 168
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 186 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 914 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 923
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 929
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 935
void*Cyc_Absyn_compress(void*);
# 991
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 1156
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1161
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1220
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 68 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);struct Cyc_Interface_I;struct _tuple11{struct _fat_ptr f0;struct _fat_ptr f1;};
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 35 "warn.h"
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
# 40 "tcutil.h"
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
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv (void){
struct Cyc_Interface_Ienv*_Tmp0=_cycalloc(sizeof(struct Cyc_Interface_Ienv));({
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->aggrdecls=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->datatypedecls=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->enumdecls=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->typedefdecls=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->vardecls=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->xdatatypefielddecls=_Tmp1;});return _Tmp0;}
# 80
struct Cyc_Interface_I*Cyc_Interface_empty (void){
struct Cyc_Interface_I*_Tmp0=_cycalloc(sizeof(struct Cyc_Interface_I));({struct Cyc_Interface_Ienv*_Tmp1=Cyc_Interface_new_ienv();_Tmp0->imports=_Tmp1;}),({struct Cyc_Interface_Ienv*_Tmp1=Cyc_Interface_new_ienv();_Tmp0->exports=_Tmp1;}),_Tmp0->tds=0;return _Tmp0;}
# 85
struct Cyc_Interface_I*Cyc_Interface_final (void){
struct Cyc_Interface_I*i=Cyc_Interface_empty();
# 88
struct Cyc_Absyn_Datatypedecl*exn_d;exn_d=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl)),({struct Cyc_Absyn_Datatypedecl _Tmp0=*Cyc_Absyn_exn_tud();*exn_d=_Tmp0;});
exn_d->sc=2U;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))Cyc_Dict_insert;_Tmp1;})(i->exports->datatypedecls,Cyc_Absyn_exn_name,exn_d);i->exports->datatypedecls=_Tmp0;});
# 92
{struct Cyc_List_List*tufs=(struct Cyc_List_List*)_check_null(Cyc_Absyn_exn_tud()->fields)->v;for(0;tufs!=0;tufs=tufs->tl){
struct Cyc_Absyn_Datatypefield*exn_f;exn_f=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield)),*exn_f=*((struct Cyc_Absyn_Datatypefield*)tufs->hd);
exn_f->sc=2U;{
struct Cyc_Tcdecl_Xdatatypefielddecl*exn_fd;exn_fd=_cycalloc(sizeof(struct Cyc_Tcdecl_Xdatatypefielddecl)),exn_fd->base=exn_d,exn_fd->field=exn_f;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*))Cyc_Dict_insert;_Tmp1;})(i->exports->xdatatypefielddecls,((struct Cyc_Absyn_Datatypefield*)tufs->hd)->name,exn_fd);i->exports->xdatatypefielddecls=_Tmp0;});}}}
# 118 "interface.cyc"
i->imports=i->exports;
return i;}
# 138 "interface.cyc"
static void Cyc_Interface_err(struct _fat_ptr msg){
Cyc_Warn_err(0U,msg,_tag_fat(0U,sizeof(void*),0));}
# 141
static void*Cyc_Interface_invalid_arg(struct _fat_ptr s){
_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=s;_Tmp0;}));}
# 144
static void Cyc_Interface_fields_err(struct _fat_ptr sc,struct _fat_ptr t,struct _tuple0*n){
Cyc_Interface_err(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=sc;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=t;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({
struct _fat_ptr _Tmp4=Cyc_Absynpp_qvar2string(n);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_aprintf(_tag_fat("fields of %s %s %s have never been defined",sizeof(char),43U),_tag_fat(_Tmp3,sizeof(void*),3));}));}
# 148
static void Cyc_Interface_body_err(struct _fat_ptr sc,struct _tuple0*n){
Cyc_Interface_err(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=sc;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(n);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("the body of %s function %s has never been defined",sizeof(char),50U),_tag_fat(_Tmp2,sizeof(void*),2));}));}
# 154
static void Cyc_Interface_static_err(struct _fat_ptr obj1,struct _tuple0*name1,struct _fat_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr!=(char*)_tag_fat(0,0,0).curr)
Cyc_Interface_err(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=obj1;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(name1);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=obj2;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
struct _fat_ptr _Tmp5=Cyc_Absynpp_qvar2string(name2);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_aprintf(_tag_fat("declaration of %s %s relies on static %s %s",sizeof(char),44U),_tag_fat(_Tmp4,sizeof(void*),4));}));}
# 160
static void Cyc_Interface_abstract_err(struct _fat_ptr obj1,struct _tuple0*name1,struct _fat_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr!=(char*)_tag_fat(0,0,0).curr)
Cyc_Interface_err(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=obj1;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(name1);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=obj2;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
struct _fat_ptr _Tmp5=Cyc_Absynpp_qvar2string(name2);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_aprintf(_tag_fat("declaration of %s %s relies on fields of abstract %s %s",sizeof(char),56U),_tag_fat(_Tmp4,sizeof(void*),4));}));}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 176
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen (void){
struct Cyc_Interface_Seen*_Tmp0=_cycalloc(sizeof(struct Cyc_Interface_Seen));({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->aggrs=_Tmp1;}),({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->datatypes=_Tmp1;});return _Tmp0;}
# 181
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*);
# 183
static int Cyc_Interface_check_public_type_list(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _fat_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){
# 185
int res=1;
for(1;l!=0;l=l->tl){
if(!({struct Cyc_Tcenv_Genv*_Tmp0=ae;struct Cyc_Interface_Seen*_Tmp1=seen;struct _fat_ptr _Tmp2=obj;struct _tuple0*_Tmp3=name;Cyc_Interface_check_public_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,f(l->hd));}))
res=0;}
return res;}
# 192
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){{int _Tmp2=({int(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple0*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp3;})(seen->aggrs,d->name);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent)
goto _LL0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}{
int res=1;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))Cyc_Dict_insert;_Tmp1;})(seen->aggrs,d->name,res);seen->aggrs=_Tmp0;});
if(d->impl!=0){
struct Cyc_List_List*fs=d->impl->fields;for(0;fs!=0;fs=fs->tl){
if(!Cyc_Interface_check_public_type(ae,seen,_tag_fat("type",sizeof(char),5U),d->name,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
res=0;}}
# 202
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))Cyc_Dict_insert;_Tmp1;})(seen->aggrs,d->name,res);seen->aggrs=_Tmp0;});
return res;}}struct _tuple12{struct Cyc_Absyn_Tqual f0;void*f1;};
# 206
static void*Cyc_Interface_get_type1(struct _tuple12*x){
return(*x).f1;}
# 209
static void*Cyc_Interface_get_type2(struct _tuple8*x){
return(*x).f2;}
# 213
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){{int _Tmp2=({int(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple0*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp3;})(seen->datatypes,d->name);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent)
goto _LL0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}{
int res=1;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))Cyc_Dict_insert;_Tmp1;})(seen->datatypes,d->name,res);seen->datatypes=_Tmp0;});
if(d->fields!=0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)d->fields->v;for(0;fs!=0;fs=fs->tl){
if(!({int(*_Tmp0)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple12*),struct Cyc_List_List*)=(int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list;_Tmp0;})(ae,seen,_tag_fat("datatype",sizeof(char),9U),d->name,Cyc_Interface_get_type1,((struct Cyc_Absyn_Datatypefield*)fs->hd)->typs))
# 222
res=0;}}
# 224
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))Cyc_Dict_insert;_Tmp1;})(seen->datatypes,d->name,res);seen->datatypes=_Tmp0;});
return res;}}
# 228
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 232
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){
if(d->defn!=0)
return Cyc_Interface_check_public_type(ae,seen,_tag_fat(0,0,0),d->name,d->defn);
return 1;}
# 239
static int Cyc_Interface_check_public_vardecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
return Cyc_Interface_check_public_type(ae,seen,_tag_fat("variable",sizeof(char),9U),d->name,d->type);}
# 243
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _fat_ptr obj,struct _tuple0*name,void*t){
void*_Tmp0=Cyc_Absyn_compress(t);union Cyc_Absyn_AggrInfo _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 4: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*t=_Tmp4;
_Tmp4=t;goto _LL4;}case 5: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_LL4: {void*t=_Tmp4;
_Tmp4=t;goto _LL6;}case 8: _Tmp4=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f4;if((void*)_Tmp4!=0){_LL6: {void*t=_Tmp4;
# 248
return Cyc_Interface_check_public_type(ae,seen,obj,name,t);}}else{goto _LL11;}case 6: _Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ret_type;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.args;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.cyc_varargs;{void*ret=_Tmp4;struct Cyc_List_List*args=_Tmp3;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp2;
# 252
int b=({int(*_Tmp5)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple8*),struct Cyc_List_List*)=(int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list;_Tmp5;})(ae,seen,obj,name,Cyc_Interface_get_type2,args)&&
 Cyc_Interface_check_public_type(ae,seen,obj,name,ret);
if(cyc_varargs!=0){
struct Cyc_Absyn_VarargInfo _Tmp5=*cyc_varargs;void*_Tmp6;_Tmp6=_Tmp5.type;{void*vt=_Tmp6;
b=Cyc_Interface_check_public_type(ae,seen,obj,name,vt);}}
# 258
return b;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 24: _Tmp1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp1;struct Cyc_List_List*targs=_Tmp4;
# 261
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if((int)ad->sc==0){
Cyc_Interface_static_err(obj,name,_tag_fat("type",sizeof(char),5U),ad->name);
return 0;}
# 266
return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,targs)&&
 Cyc_Interface_check_public_aggrdecl(ae,seen,ad);}case 19: _Tmp4=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{struct _tuple0*name=_Tmp4;
# 270
struct Cyc_Absyn_Enumdecl*ed;
{struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){ed=*({struct Cyc_Absyn_Enumdecl**(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp7;})(ae->enumdecls,name);;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp7)->tag==Cyc_Dict_Absent)
# 273
({int(*_Tmp9)(struct _fat_ptr)=({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_TmpA;});_Tmp9(({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_Absynpp_qvar2string(name);_TmpB.f1=_TmpC;});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_aprintf(_tag_fat("check_public_type (can't find enum %s)",sizeof(char),39U),_tag_fat(_TmpB,sizeof(void*),1));}));});else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}};}}}
# 276
if((int)ed->sc==0){
Cyc_Interface_static_err(obj,name,_tag_fat("enum",sizeof(char),5U),ed->name);
return 0;}
# 280
return 1;}case 22: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatype.tag==2){_Tmp4=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatype.val;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Datatypedecl*tud0=_Tmp4;struct Cyc_List_List*targs=_Tmp3;
# 283
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){tud=*({struct Cyc_Absyn_Datatypedecl**(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp7;})(ae->datatypedecls,tud0->name);;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp7)->tag==Cyc_Dict_Absent)
# 286
({int(*_Tmp9)(struct _fat_ptr)=({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_TmpA;});_Tmp9(({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_Absynpp_qvar2string(tud0->name);_TmpB.f1=_TmpC;});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_aprintf(_tag_fat("check_public_type (can't find datatype %s)",sizeof(char),43U),_tag_fat(_TmpB,sizeof(void*),1));}));});else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}};}}}
# 289
if((int)tud->sc==0){
Cyc_Interface_static_err(obj,name,_tag_fat("datatype",sizeof(char),9U),tud->name);
return 0;}
# 293
return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,targs)&&
 Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}else{goto _LL11;}case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatypefield.tag==2){_Tmp4=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatypefield.val.f0;_Tmp3=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatypefield.val.f1;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Datatypedecl*tud0=_Tmp4;struct Cyc_Absyn_Datatypefield*tuf0=_Tmp3;struct Cyc_List_List*targs=_Tmp2;
# 297
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){tud=*({struct Cyc_Absyn_Datatypedecl**(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp7;})(ae->datatypedecls,tud0->name);;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp7)->tag==Cyc_Dict_Absent)
# 300
({int(*_Tmp9)(struct _fat_ptr)=({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_TmpA;});_Tmp9(({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_Absynpp_qvar2string(tud0->name);_TmpB.f1=_TmpC;});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_aprintf(_tag_fat("check_public_type (can't find datatype %s and search its fields)",sizeof(char),65U),_tag_fat(_TmpB,sizeof(void*),1));}));});else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}};}}}
# 303
if(tud->fields==0)
({int(*_Tmp5)(struct _fat_ptr)=({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_Tmp6;});_Tmp5(({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=Cyc_Absynpp_qvar2string(tud->name);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("check_public_type (datatype %s has no fields)",sizeof(char),46U),_tag_fat(_Tmp7,sizeof(void*),1));}));});{
# 307
struct Cyc_Absyn_Datatypefield*tuf1=0;
{struct Cyc_List_List*fs=(struct Cyc_List_List*)tud->fields->v;for(0;fs!=0;fs=fs->tl){
if(Cyc_strptrcmp((*tuf0->name).f1,(*((struct Cyc_Absyn_Datatypefield*)fs->hd)->name).f1)==0){
tuf1=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 314
if(tuf1==0)
({int(*_Tmp5)(struct _fat_ptr)=({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_Tmp6;});_Tmp5(({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=Cyc_Absynpp_qvar2string(tuf0->name);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("check_public_type (can't find datatypefield %s)",sizeof(char),48U),_tag_fat(_Tmp7,sizeof(void*),1));}));});
# 318
if((int)tud->sc==0){
Cyc_Interface_static_err(obj,name,_tag_fat("datatype",sizeof(char),9U),tud->name);
return 0;}
# 322
if((int)tud->sc==1){
Cyc_Interface_abstract_err(obj,name,_tag_fat("datatype",sizeof(char),9U),tud->name);
return 0;}
# 326
if((int)tuf1->sc==0){
({struct _fat_ptr _Tmp5=obj;struct _tuple0*_Tmp6=name;struct _fat_ptr _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_qvar2string(tuf1->name);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("field %s of",sizeof(char),12U),_tag_fat(_Tmp9,sizeof(void*),1));});Cyc_Interface_static_err(_Tmp5,_Tmp6,_Tmp7,tud->name);});
return 0;}
# 330
return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,targs)&&
 Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}}else{goto _LL11;}default: goto _LL11;}default: _LL11:
# 333
 return 1;};}struct _tuple13{struct Cyc_Interface_Ienv*f0;struct Cyc_Interface_Ienv*f1;int f2;struct Cyc_Tcenv_Genv*f3;struct Cyc_Interface_Seen*f4;struct Cyc_Interface_I*f5;};
# 339
static void Cyc_Interface_extract_aggrdecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Aggrdecl**dp){
# 341
void*_Tmp0;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=env->f0;_Tmp3=env->f1;_Tmp2=env->f2;_Tmp1=env->f3;_Tmp0=env->f4;{struct Cyc_Interface_Ienv*imp=_Tmp4;struct Cyc_Interface_Ienv*exp=_Tmp3;int check_complete_defs=_Tmp2;struct Cyc_Tcenv_Genv*ae=_Tmp1;struct Cyc_Interface_Seen*seen=_Tmp0;
struct Cyc_Absyn_Aggrdecl*d=*dp;
enum Cyc_Absyn_Scope _Tmp5=d->sc;switch((int)_Tmp5){case Cyc_Absyn_Static:
# 345
 if(check_complete_defs && d->impl==0)
Cyc_Interface_fields_err(_tag_fat("static",sizeof(char),7U),_tag_fat("struct/union",sizeof(char),13U),d->name);
goto _LL3;case Cyc_Absyn_Abstract:
# 349
 if(d->impl==0){
if(check_complete_defs)
Cyc_Interface_fields_err(_tag_fat("abstract",sizeof(char),9U),_tag_fat("struct/union",sizeof(char),13U),d->name);}else{
# 353
d=({struct Cyc_Absyn_Aggrdecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));*_Tmp6=*d;_Tmp6;});
d->impl=0;}
# 356
if(Cyc_Interface_check_public_aggrdecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert;_Tmp7;})(exp->aggrdecls,x,d);exp->aggrdecls=_Tmp6;});
goto _LL3;case Cyc_Absyn_Public:
# 360
 if(d->impl==0){
Cyc_Interface_fields_err(_tag_fat("public",sizeof(char),7U),_tag_fat("struct/union",sizeof(char),13U),d->name);
d=({struct Cyc_Absyn_Aggrdecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));*_Tmp6=*d;_Tmp6;});
d->sc=1U;}
# 365
if(Cyc_Interface_check_public_aggrdecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert;_Tmp7;})(exp->aggrdecls,x,d);exp->aggrdecls=_Tmp6;});
goto _LL3;case Cyc_Absyn_ExternC:
 goto _LLD;case Cyc_Absyn_Extern: _LLD:
# 370
 if(Cyc_Interface_check_public_aggrdecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert;_Tmp7;})(imp->aggrdecls,x,d);imp->aggrdecls=_Tmp6;});
goto _LL3;case Cyc_Absyn_Register:
 goto _LL11;default: _LL11:
# 375
({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_Tmp6;})(_tag_fat("add_aggrdecl",sizeof(char),13U));
goto _LL3;}_LL3:;}}
# 379
static void Cyc_Interface_extract_enumdecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Enumdecl**dp){
# 381
void*_Tmp0;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=env->f0;_Tmp3=env->f1;_Tmp2=env->f2;_Tmp1=env->f3;_Tmp0=env->f4;{struct Cyc_Interface_Ienv*imp=_Tmp4;struct Cyc_Interface_Ienv*exp=_Tmp3;int check_complete_defs=_Tmp2;struct Cyc_Tcenv_Genv*ae=_Tmp1;struct Cyc_Interface_Seen*seen=_Tmp0;
struct Cyc_Absyn_Enumdecl*d=*dp;
enum Cyc_Absyn_Scope _Tmp5=d->sc;switch((int)_Tmp5){case Cyc_Absyn_Static:
# 385
 if(check_complete_defs && d->fields==0)
Cyc_Interface_fields_err(_tag_fat("static",sizeof(char),7U),_tag_fat("enum",sizeof(char),5U),d->name);
goto _LL3;case Cyc_Absyn_Abstract:
# 389
 if(d->fields==0){
if(check_complete_defs)
Cyc_Interface_fields_err(_tag_fat("abstract",sizeof(char),9U),_tag_fat("enum",sizeof(char),5U),d->name);}else{
# 393
d=({struct Cyc_Absyn_Enumdecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));*_Tmp6=*d;_Tmp6;});
d->fields=0;}
# 396
if(Cyc_Interface_check_public_enumdecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert;_Tmp7;})(exp->enumdecls,x,d);exp->enumdecls=_Tmp6;});
goto _LL3;case Cyc_Absyn_Public:
# 400
 if(d->fields==0){
Cyc_Interface_fields_err(_tag_fat("public",sizeof(char),7U),_tag_fat("enum",sizeof(char),5U),d->name);
d=({struct Cyc_Absyn_Enumdecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));*_Tmp6=*d;_Tmp6;});
d->sc=1U;}
# 405
if(Cyc_Interface_check_public_enumdecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert;_Tmp7;})(exp->enumdecls,x,d);exp->enumdecls=_Tmp6;});
goto _LL3;case Cyc_Absyn_ExternC:
 goto _LLD;case Cyc_Absyn_Extern: _LLD:
# 410
 if(Cyc_Interface_check_public_enumdecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert;_Tmp7;})(imp->enumdecls,x,d);imp->enumdecls=_Tmp6;});
goto _LL3;case Cyc_Absyn_Register:
 goto _LL11;default: _LL11:({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_Tmp6;})(_tag_fat("add_enumdecl",sizeof(char),13U));}_LL3:;}}
# 417
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){
# 419
struct Cyc_Interface_Ienv*env;
{enum Cyc_Absyn_Scope _Tmp0=f->sc;switch((int)_Tmp0){case Cyc_Absyn_Static:
 return;case Cyc_Absyn_Extern:
 env=i->imports;goto _LL0;case Cyc_Absyn_Public:
 env=i->exports;goto _LL0;default:
({int(*_Tmp1)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_Tmp1;})(_tag_fat("add_xdatatypefielddecl",sizeof(char),23U));}_LL0:;}
# 427
({struct Cyc_Dict_Dict _Tmp0=({
struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*)=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*))Cyc_Dict_insert;_Tmp2;});struct Cyc_Dict_Dict _Tmp2=env->xdatatypefielddecls;struct _tuple0*_Tmp3=f->name;_Tmp1(_Tmp2,_Tmp3,({struct Cyc_Tcdecl_Xdatatypefielddecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcdecl_Xdatatypefielddecl));
_Tmp4->base=d,_Tmp4->field=f;_Tmp4;}));});
# 427
env->xdatatypefielddecls=_Tmp0;});}
# 432
static void Cyc_Interface_extract_datatypedecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Datatypedecl**dp){
# 434
void*_Tmp0;void*_Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=env->f0;_Tmp4=env->f1;_Tmp3=env->f2;_Tmp2=env->f3;_Tmp1=env->f4;_Tmp0=env->f5;{struct Cyc_Interface_Ienv*imp=_Tmp5;struct Cyc_Interface_Ienv*exp=_Tmp4;int check_complete_defs=_Tmp3;struct Cyc_Tcenv_Genv*ae=_Tmp2;struct Cyc_Interface_Seen*seen=_Tmp1;struct Cyc_Interface_I*i=_Tmp0;
struct Cyc_Absyn_Datatypedecl*d=*dp;
# 437
enum Cyc_Absyn_Scope _Tmp6=d->sc;switch((int)_Tmp6){case Cyc_Absyn_Static:
# 439
 if((!d->is_extensible && check_complete_defs)&& d->fields==0)
Cyc_Interface_fields_err(_tag_fat("static",sizeof(char),7U),_tag_fat("datatype",sizeof(char),9U),d->name);
goto _LL3;case Cyc_Absyn_Abstract:
# 443
 if(d->fields==0){
if(!d->is_extensible && check_complete_defs)
Cyc_Interface_fields_err(_tag_fat("abstract",sizeof(char),9U),_tag_fat("datatype",sizeof(char),9U),d->name);}else{
# 447
d=({struct Cyc_Absyn_Datatypedecl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));*_Tmp7=*d;_Tmp7;});
d->fields=0;}
# 450
if(Cyc_Interface_check_public_datatypedecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp7=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))Cyc_Dict_insert;_Tmp8;})(exp->datatypedecls,x,d);exp->datatypedecls=_Tmp7;});
goto _LL3;case Cyc_Absyn_Public:
# 454
 d=({struct Cyc_Absyn_Datatypedecl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));*_Tmp7=*d;_Tmp7;});
if(!d->is_extensible && d->fields==0){
Cyc_Interface_fields_err(_tag_fat("public",sizeof(char),7U),_tag_fat("datatype",sizeof(char),9U),d->name);
d->sc=1U;}
# 459
if(Cyc_Interface_check_public_datatypedecl(ae,seen,d)){
if(d->is_extensible && d->fields!=0){
struct Cyc_List_List*fields=(struct Cyc_List_List*)d->fields->v;
d->fields=0;{
struct Cyc_List_List*f=fields;for(0;f!=0;f=f->tl){
Cyc_Interface_extract_xdatatypefielddecl(i,d,(struct Cyc_Absyn_Datatypefield*)f->hd);}}}
# 466
({struct Cyc_Dict_Dict _Tmp7=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))Cyc_Dict_insert;_Tmp8;})(exp->datatypedecls,x,d);exp->datatypedecls=_Tmp7;});}
# 468
goto _LL3;case Cyc_Absyn_ExternC:
({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_Tmp7;})(_tag_fat("extract_datatypedecl",sizeof(char),21U));case Cyc_Absyn_Extern:
# 471
 if(Cyc_Interface_check_public_datatypedecl(ae,seen,d)){
if(d->is_extensible && d->fields!=0){
d=({struct Cyc_Absyn_Datatypedecl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));*_Tmp7=*d;_Tmp7;});{
struct Cyc_List_List*fields=(struct Cyc_List_List*)_check_null(d->fields)->v;
d->fields=0;
{struct Cyc_List_List*f=fields;for(0;f!=0;f=f->tl){
Cyc_Interface_extract_xdatatypefielddecl(i,d,(struct Cyc_Absyn_Datatypefield*)f->hd);}}
({struct Cyc_Dict_Dict _Tmp7=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))Cyc_Dict_insert;_Tmp8;})(imp->datatypedecls,x,d);imp->datatypedecls=_Tmp7;});}}}
# 480
goto _LL3;case Cyc_Absyn_Register:
 goto _LL11;default: _LL11:
# 483
({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_Tmp7;})(_tag_fat("add_datatypedecl",sizeof(char),17U));
goto _LL3;}_LL3:;}}
# 488
static void Cyc_Interface_extract_typedef(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d){
# 490
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=env->f1;_Tmp1=env->f3;_Tmp0=env->f4;{struct Cyc_Interface_Ienv*exp=_Tmp2;struct Cyc_Tcenv_Genv*ae=_Tmp1;struct Cyc_Interface_Seen*seen=_Tmp0;
if(Cyc_Interface_check_public_typedefdecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict(*_Tmp4)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*))Cyc_Dict_insert;_Tmp4;})(exp->typedefdecls,x,d);exp->typedefdecls=_Tmp3;});}}struct _tuple14{void*f0;int f1;};
# 495
static void Cyc_Interface_extract_ordinarie(struct _tuple13*env,struct _tuple0*x,struct _tuple14*v){
# 497
void*_Tmp0;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=env->f0;_Tmp3=env->f1;_Tmp2=env->f2;_Tmp1=env->f3;_Tmp0=env->f4;{struct Cyc_Interface_Ienv*imp=_Tmp4;struct Cyc_Interface_Ienv*exp=_Tmp3;int check_complete_defs=_Tmp2;struct Cyc_Tcenv_Genv*ae=_Tmp1;struct Cyc_Interface_Seen*seen=_Tmp0;
# 499
void*_Tmp5=(*v).f0;void*_Tmp6;switch(*((int*)_Tmp5)){case 2: _Tmp6=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp6;
# 501
struct Cyc_Absyn_Vardecl*vd;vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),vd->sc=fd->sc,vd->name=fd->name,vd->varloc=0U,({
# 505
struct Cyc_Absyn_Tqual _Tmp7=Cyc_Absyn_empty_tqual(0U);vd->tq=_Tmp7;}),vd->type=
_check_null(fd->cached_type),vd->initializer=0,vd->rgn=0,vd->attributes=0,vd->escapes=0,vd->is_proto=fd->body!=0,vd->rename=0;
# 514
check_complete_defs=0;
_Tmp6=vd;goto _LL7;}case 1: _Tmp6=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_Tmp5)->f1;_LL7: {struct Cyc_Absyn_Vardecl*d=_Tmp6;
# 517
if(d->initializer!=0){
d=({struct Cyc_Absyn_Vardecl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));*_Tmp7=*d;_Tmp7;});
d->initializer=0;}
# 521
{enum Cyc_Absyn_Scope _Tmp7=d->sc;switch((int)_Tmp7){case Cyc_Absyn_Static:
# 523
 if(check_complete_defs && Cyc_Tcutil_is_function_type(d->type))
Cyc_Interface_body_err(_tag_fat("static",sizeof(char),7U),d->name);
goto _LLA;case Cyc_Absyn_Register:
 goto _LL10;case Cyc_Absyn_Abstract: _LL10:
({int(*_Tmp8)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg;_Tmp8;})(_tag_fat("extract_ordinarie",sizeof(char),18U));case Cyc_Absyn_Public:
# 529
 if(check_complete_defs && Cyc_Tcutil_is_function_type(d->type))
Cyc_Interface_body_err(_tag_fat("public",sizeof(char),7U),d->name);
if(Cyc_Interface_check_public_vardecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict(*_Tmp9)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*))Cyc_Dict_insert;_Tmp9;})(exp->vardecls,x,d);exp->vardecls=_Tmp8;});
goto _LLA;case Cyc_Absyn_ExternC:
 goto _LL16;case Cyc_Absyn_Extern: _LL16:
 goto _LL18;default: _LL18:
# 537
 if(Cyc_Interface_check_public_vardecl(ae,seen,d))
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict(*_Tmp9)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*))Cyc_Dict_insert;_Tmp9;})(imp->vardecls,x,d);imp->vardecls=_Tmp8;});
goto _LLA;}_LLA:;}
# 541
goto _LL3;}default:
 goto _LL3;}_LL3:;}}struct _tuple15{void*f0;void*f1;};
# 546
static struct Cyc_List_List*Cyc_Interface_remove_decl_from_list(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*d){
if(l==0)return 0;
{struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=d->r,_Tmp1.f1=((struct Cyc_Absyn_Decl*)l->hd)->r;_Tmp1;});void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0.f0)){case 8: if(*((int*)_Tmp0.f1)==8){_Tmp2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Typedefdecl*a1=_Tmp2;struct Cyc_Absyn_Typedefdecl*a2=_Tmp1;
# 550
if(Cyc_Absyn_qvar_cmp(a1->name,a2->name)!=0)goto _LL0;
return Cyc_Interface_remove_decl_from_list(l->tl,d);}}else{goto _LL7;}case 7: if(*((int*)_Tmp0.f1)==7){_Tmp2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Enumdecl*a1=_Tmp2;struct Cyc_Absyn_Enumdecl*a2=_Tmp1;
# 553
if(Cyc_Absyn_qvar_cmp(a1->name,a2->name)!=0)goto _LL0;
if((int)a1->sc==3)a1->sc=a2->sc;
return Cyc_Interface_remove_decl_from_list(l->tl,d);
goto _LL0;}}else{goto _LL7;}case 0: if(*((int*)_Tmp0.f1)==0){_Tmp2=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl**vd1=(struct Cyc_Absyn_Vardecl**)_Tmp2;struct Cyc_Absyn_Vardecl*vd2=_Tmp1;
# 558
if(Cyc_Absyn_qvar_cmp((*vd1)->name,vd2->name)!=0)goto _LL0;
if((int)(*vd1)->sc==3)({struct Cyc_Absyn_Vardecl*_Tmp3=({struct Cyc_Absyn_Vardecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));*_Tmp4=*vd2;_Tmp4;});*vd1=_Tmp3;});
return Cyc_Interface_remove_decl_from_list(l->tl,d);}}else{goto _LL7;}default: _LL7:
# 562
 goto _LL0;}_LL0:;}{
# 564
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(struct Cyc_Absyn_Decl*)l->hd,({struct Cyc_List_List*_Tmp1=Cyc_Interface_remove_decl_from_list(l->tl,d);_Tmp0->tl=_Tmp1;});return _Tmp0;}}
# 567
static struct Cyc_List_List*Cyc_Interface_uniqify_decl_list(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
if(accum==0){struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=d,_Tmp0->tl=0;return _Tmp0;}
{struct Cyc_List_List*l=accum;for(0;l!=0;l=l->tl){
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=d->r,_Tmp1.f1=((struct Cyc_Absyn_Decl*)l->hd)->r;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0.f0)){case 0: if(*((int*)_Tmp0.f1)==0){_Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp3=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl*vd1=_Tmp4;struct Cyc_Absyn_Vardecl**vd2=_Tmp3;
# 572
if(Cyc_Absyn_qvar_cmp(vd1->name,(*vd2)->name)!=0)goto _LL0;
if((int)(*vd2)->sc==3)({struct Cyc_Absyn_Vardecl*_Tmp5=({struct Cyc_Absyn_Vardecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));*_Tmp6=*vd1;_Tmp6;});*vd2=_Tmp5;});
return accum;}}else{goto _LL9;}case 8: if(*((int*)_Tmp0.f1)==8){_Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Typedefdecl*a1=_Tmp4;struct Cyc_Absyn_Typedefdecl*a2=_Tmp3;
# 576
if(Cyc_Absyn_qvar_cmp(a1->name,a2->name)!=0)goto _LL0;
return accum;}}else{goto _LL9;}case 7: if(*((int*)_Tmp0.f1)==7){_Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Enumdecl*a1=_Tmp4;struct Cyc_Absyn_Enumdecl*a2=_Tmp3;
# 579
if(Cyc_Absyn_qvar_cmp(a1->name,a2->name)!=0)goto _LL0;
return accum;}}else{goto _LL9;}case 9: if(*((int*)_Tmp0.f1)==9){_Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp3=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f2;_Tmp2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f2;{struct _fat_ptr*a1=_Tmp4;struct Cyc_List_List**b1=(struct Cyc_List_List**)_Tmp3;struct _fat_ptr*a2=_Tmp2;struct Cyc_List_List*b2=_Tmp1;
# 582
if(Cyc_strptrcmp(a1,a2)!=0)goto _LL0;
{struct Cyc_List_List*dl=b2;for(0;dl!=0;dl=dl->tl){
({struct Cyc_List_List*_Tmp5=Cyc_Interface_remove_decl_from_list(*b1,(struct Cyc_Absyn_Decl*)dl->hd);*b1=_Tmp5;});}}
goto _LL0;}}else{goto _LL9;}default: _LL9:
 goto _LL0;}_LL0:;}}{
# 588
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=d,_Tmp0->tl=accum;return _Tmp0;}}
# 591
static struct Cyc_List_List*Cyc_Interface_filterstatics(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
{void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 594
if((int)vd->sc==4)return accum;
if((int)vd->sc==0)return accum;{
struct Cyc_Absyn_Vardecl*nvd;nvd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),*nvd=*vd;
nvd->initializer=0;
if(Cyc_Tcutil_is_function_type(nvd->type)&&(int)nvd->sc!=3)
nvd->sc=3U;{
struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp5->tag=0,_Tmp5->f1=nvd;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=accum;return _Tmp3;}}}case 1: _Tmp2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp2;
# 602
if((int)fd->sc==0)return accum;
if((int)fd->sc==4)return accum;{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,fd->name,_check_null(fd->cached_type),0,0);
vd->sc=fd->sc;{
struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp5->tag=0,_Tmp5->f1=vd;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=accum;return _Tmp3;}}}case 5: _Tmp2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*a=_Tmp2;
# 609
if((int)a->sc==4)return accum;
goto _LL0;}case 6: _Tmp2=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*a=_Tmp2;
# 613
if((int)a->sc==4)return accum;
goto _LL0;}case 7: _Tmp2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*a=_Tmp2;
# 616
if((int)a->sc==0)return accum;
if((int)a->sc==4)return accum;
goto _LL0;}case 8: _Tmp2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*a=_Tmp2;
# 620
goto _LL0;}case 9: _Tmp2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*a=_Tmp2;struct Cyc_List_List*b=_Tmp1;
# 622
struct Cyc_List_List*l=Cyc_List_rev(({struct Cyc_List_List*(*_Tmp3)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left;_Tmp3;})(Cyc_Interface_filterstatics,0,b));
struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,_Tmp5->f1=a,_Tmp5->f2=l;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=accum;return _Tmp3;}case 10: _Tmp2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*b=_Tmp2;
# 625
struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left;_Tmp4;})(Cyc_Interface_filterstatics,0,b);return Cyc_List_append(_Tmp3,accum);}case 2:
 goto _LL14;case 3: _LL14:
 goto _LL16;case 4: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
 return accum;}_LL0:;}{
# 636
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=({struct Cyc_Absyn_Decl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Decl));*_Tmp2=*d;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=accum;return _Tmp0;}}struct _tuple16{struct Cyc_Interface_I*f0;int f1;struct Cyc_Tcenv_Genv*f2;struct Cyc_Interface_Seen*f3;};
# 639
static void Cyc_Interface_extract_f(struct _tuple16*env_f){
void*_Tmp0;void*_Tmp1;int _Tmp2;void*_Tmp3;_Tmp3=env_f->f0;_Tmp2=env_f->f1;_Tmp1=env_f->f2;_Tmp0=env_f->f3;{struct Cyc_Interface_I*i=_Tmp3;int check_complete_defs=_Tmp2;struct Cyc_Tcenv_Genv*ae=_Tmp1;struct Cyc_Interface_Seen*seen=_Tmp0;
struct _tuple13 env=({struct _tuple13 _Tmp4;_Tmp4.f0=i->imports,_Tmp4.f1=i->exports,_Tmp4.f2=check_complete_defs,_Tmp4.f3=ae,_Tmp4.f4=seen,_Tmp4.f5=i;_Tmp4;});
({void(*_Tmp4)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp4;})(Cyc_Interface_extract_aggrdecl,& env,ae->aggrdecls);
({void(*_Tmp4)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp4;})(Cyc_Interface_extract_datatypedecl,& env,ae->datatypedecls);
({void(*_Tmp4)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp4;})(Cyc_Interface_extract_enumdecl,& env,ae->enumdecls);
({void(*_Tmp4)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp4;})(Cyc_Interface_extract_typedef,& env,ae->typedefs);
({void(*_Tmp4)(void(*)(struct _tuple13*,struct _tuple0*,struct _tuple14*),struct _tuple13*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct _tuple14*),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp4;})(Cyc_Interface_extract_ordinarie,& env,ae->ordinaries);}}
# 649
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Tcenv_Genv*ae,int check_complete_defs,struct Cyc_List_List*tds){
struct _tuple16 env=({struct _tuple16 _Tmp0;({
struct Cyc_Interface_I*_Tmp1=Cyc_Interface_empty();_Tmp0.f0=_Tmp1;}),_Tmp0.f1=check_complete_defs,_Tmp0.f2=ae,({struct Cyc_Interface_Seen*_Tmp1=Cyc_Interface_new_seen();_Tmp0.f3=_Tmp1;});_Tmp0;});
Cyc_Interface_extract_f(& env);{
struct Cyc_Interface_I*i=env.f0;
({struct Cyc_List_List*_Tmp0=Cyc_List_rev(({struct Cyc_List_List*(*_Tmp1)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left;_Tmp1;})(Cyc_Interface_filterstatics,0,tds));i->tds=_Tmp0;});
({struct Cyc_List_List*_Tmp0=Cyc_List_rev(({struct Cyc_List_List*(*_Tmp1)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left;_Tmp1;})(Cyc_Interface_uniqify_decl_list,0,i->tds));i->tds=_Tmp0;});
return i;}}
# 659
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*ae,struct Cyc_List_List*tds){
return Cyc_Interface_gen_extract(ae,1,tds);}
# 665
static void Cyc_Interface_check_err(struct _fat_ptr*msg1,struct _fat_ptr msg2){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=msg2;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Tcdecl_merr(0U,msg1,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});}struct _tuple17{int f0;struct Cyc_Dict_Dict f1;int(*f2)(void*,void*,struct _fat_ptr*);struct _fat_ptr f3;struct _fat_ptr*f4;};
# 669
static void Cyc_Interface_incl_dict_f(struct _tuple17*env,struct _tuple0*x,void*y1){
# 676
void*_Tmp0;struct _fat_ptr _Tmp1;void*_Tmp2;struct Cyc_Dict_Dict _Tmp3;void*_Tmp4;_Tmp4=(int*)& env->f0;_Tmp3=env->f1;_Tmp2=env->f2;_Tmp1=env->f3;_Tmp0=env->f4;{int*res=(int*)_Tmp4;struct Cyc_Dict_Dict dic2=_Tmp3;int(*incl_f)(void*,void*,struct _fat_ptr*)=_Tmp2;struct _fat_ptr t=_Tmp1;struct _fat_ptr*msg=_Tmp0;
struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){
{void*y2=({void*(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple0*)=(void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp7;})(dic2,x);
if(!incl_f(y1,y2,msg))*res=0;}
# 678
;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp7)->tag==Cyc_Dict_Absent){
# 681
({struct _fat_ptr*_Tmp9=msg;Cyc_Interface_check_err(_Tmp9,({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=t;_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({struct _fat_ptr _TmpD=Cyc_Absynpp_qvar2string(x);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_aprintf(_tag_fat("%s %s is missing",sizeof(char),17U),_tag_fat(_TmpC,sizeof(void*),2));}));});
*res=0;
goto _LL3;}else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}}_LL3:;}}}}
# 687
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _fat_ptr*),struct _fat_ptr t,struct _fat_ptr*msg){
# 692
struct _tuple17 env=({struct _tuple17 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=dic2,_Tmp0.f2=incl_f,_Tmp0.f3=t,_Tmp0.f4=msg;_Tmp0;});
({void(*_Tmp0)(void(*)(struct _tuple17*,struct _tuple0*,void*),struct _tuple17*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple17*,struct _tuple0*,void*),struct _tuple17*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp0;})(Cyc_Interface_incl_dict_f,& env,dic1);
return env.f0;}
# 709 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Aggrdecl*d=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0U,msg);if(d==0)return 0;if(d0!=d){({struct _fat_ptr*_Tmp0=msg;Cyc_Interface_check_err(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(d1->name);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("declaration of type %s discloses too much information",sizeof(char),54U),_tag_fat(_Tmp2,sizeof(void*),1));}));});return 0;}return 1;}
# 711
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Datatypedecl*d=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0U,msg);if(d==0)return 0;if(d0!=d){({struct _fat_ptr*_Tmp0=msg;Cyc_Interface_check_err(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(d1->name);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("declaration of datatype %s discloses too much information",sizeof(char),58U),_tag_fat(_Tmp2,sizeof(void*),1));}));});return 0;}return 1;}
# 713
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Enumdecl*d=Cyc_Tcdecl_merge_enumdecl(d0,d1,0U,msg);if(d==0)return 0;if(d0!=d){({struct _fat_ptr*_Tmp0=msg;Cyc_Interface_check_err(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(d1->name);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("declaration of enum %s discloses too much information",sizeof(char),54U),_tag_fat(_Tmp2,sizeof(void*),1));}));});return 0;}return 1;}
# 715
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Vardecl*d=Cyc_Tcdecl_merge_vardecl(d0,d1,0U,msg);if(d==0)return 0;if(d0!=d){({struct _fat_ptr*_Tmp0=msg;Cyc_Interface_check_err(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(d1->name);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("declaration of variable %s discloses too much information",sizeof(char),58U),_tag_fat(_Tmp2,sizeof(void*),1));}));});return 0;}return 1;}
# 717
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Typedefdecl*d=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0U,msg);if(d==0)return 0;if(d0!=d){({struct _fat_ptr*_Tmp0=msg;Cyc_Interface_check_err(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(d1->name);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("declaration of typedef %s discloses too much information",sizeof(char),57U),_tag_fat(_Tmp2,sizeof(void*),1));}));});return 0;}return 1;}
# 719
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _fat_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*d=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0U,msg);if(d==0)return 0;if(d0!=d){({struct _fat_ptr*_Tmp0=msg;Cyc_Interface_check_err(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_qvar2string(d1->field->name);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("declaration of xdatatypefield %s discloses too much information",sizeof(char),64U),_tag_fat(_Tmp2,sizeof(void*),1));}));});return 0;}return 1;}
# 724
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,void*(*)(void*,void*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);
# 733
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _fat_ptr*msg){
int r1=({int(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_Tmp0;})(ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,_tag_fat("type",sizeof(char),5U),msg);
int r2=({int(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_Tmp0;})(ie1->datatypedecls,ie2->datatypedecls,Cyc_Interface_incl_datatypedecl,_tag_fat("datatype",sizeof(char),9U),msg);
int r3=({int(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_Tmp0;})(ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,_tag_fat("enum",sizeof(char),5U),msg);
# 738
int r4=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=({struct Cyc_Dict_Dict*(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp1;});struct Cyc_Dict_Dict _Tmp1=ie1->typedefdecls;struct Cyc_Dict_Dict _Tmp2=ie2->typedefdecls;struct Cyc_Dict_Dict _Tmp3=
({struct Cyc_Dict_Dict(*_Tmp4)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp4;})(Cyc_Absyn_qvar_cmp);
# 738
struct _fat_ptr _Tmp4=
# 740
_tag_fat("typedef",sizeof(char),8U);
# 738
_Tmp0(_Tmp1,_Tmp2,_Tmp3,Cyc_Tcdecl_merge_typedefdecl,_Tmp4,msg);})!=0;
# 741
int r5=({int(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_Tmp0;})(ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,_tag_fat("variable",sizeof(char),9U),msg);
int r6=({int(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict;_Tmp0;})(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,_tag_fat("xdatatypefield",sizeof(char),15U),msg);
return((((r1 && r2)&& r3)&& r4)&& r5)&& r6;}
# 746
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{int res=1;
struct _fat_ptr*msg=0;
# 751
if(info!=0)
msg=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(*info).f0;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(*info).f1;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("checking inclusion of %s exports into %s exports,",sizeof(char),50U),_tag_fat(_Tmp6,sizeof(void*),2));});*_Tmp2=_Tmp3;});_Tmp2;});
if(!Cyc_Interface_incl_ienv(i1->exports,i2->exports,msg))res=0;
# 755
if(info!=0)
msg=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(*info).f1;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(*info).f0;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("checking inclusion of %s imports into %s imports,",sizeof(char),50U),_tag_fat(_Tmp6,sizeof(void*),2));});*_Tmp2=_Tmp3;});_Tmp2;});
if(!Cyc_Interface_incl_ienv(i2->imports,i1->imports,msg))res=0;{
# 759
int _Tmp2=res;_npop_handler(0);return _Tmp2;}}
# 748
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_Tmp2)->tag==Cyc_Tcdecl_Incompatible)
# 760
return 0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}struct _tuple18{int f0;struct Cyc_Dict_Dict f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;void*(*f4)(void*,void*,unsigned,struct _fat_ptr*);struct _fat_ptr f5;struct _fat_ptr*f6;};
# 767
void Cyc_Interface_compat_merge_dict_f(struct _tuple18*env,struct _tuple0*x,void*y2){
# 774
void*_Tmp0;struct _fat_ptr _Tmp1;void*_Tmp2;struct Cyc_Dict_Dict _Tmp3;struct Cyc_Dict_Dict _Tmp4;void*_Tmp5;void*_Tmp6;_Tmp6=(int*)& env->f0;_Tmp5=(struct Cyc_Dict_Dict*)& env->f1;_Tmp4=env->f2;_Tmp3=env->f3;_Tmp2=env->f4;_Tmp1=env->f5;_Tmp0=env->f6;{int*res=(int*)_Tmp6;struct Cyc_Dict_Dict*res_dic=(struct Cyc_Dict_Dict*)_Tmp5;struct Cyc_Dict_Dict dic1=_Tmp4;struct Cyc_Dict_Dict excl=_Tmp3;void*(*merge_f)(void*,void*,unsigned,struct _fat_ptr*)=_Tmp2;struct _fat_ptr t=_Tmp1;struct _fat_ptr*msg=_Tmp0;
void*y;
{struct _handler_cons _Tmp7;_push_handler(& _Tmp7);{int _Tmp8=0;if(setjmp(_Tmp7.handler))_Tmp8=1;if(!_Tmp8){
{void*y1=({void*(*_Tmp9)(struct Cyc_Dict_Dict,struct _tuple0*)=(void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp9;})(dic1,x);
# 780
void*yt=merge_f(y1,y2,0U,msg);
if(!((unsigned)yt)){
*res=0;
_npop_handler(0);return;}
# 785
y=yt;}
# 777
;_pop_handler();}else{void*_Tmp9=(void*)Cyc_Core_get_exn_thrown();void*_TmpA;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp9)->tag==Cyc_Dict_Absent){
# 787
y=y2;
goto _LL3;}else{_TmpA=_Tmp9;{void*exn=_TmpA;_rethrow(exn);}}_LL3:;}}}{
# 791
struct _handler_cons _Tmp7;_push_handler(& _Tmp7);{int _Tmp8=0;if(setjmp(_Tmp7.handler))_Tmp8=1;if(!_Tmp8){
{void*ye=({void*(*_Tmp9)(struct Cyc_Dict_Dict,struct _tuple0*)=(void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp9;})(excl,x);
# 796
void*yt=merge_f(ye,y,0U,msg);
if(_check_null(yt)!=ye){
if((unsigned)yt)
({struct _fat_ptr*_Tmp9=msg;Cyc_Interface_check_err(_Tmp9,({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=t;_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({
struct _fat_ptr _TmpD=Cyc_Absynpp_qvar2string(x);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_aprintf(_tag_fat("abstract %s %s is being imported as non-abstract",sizeof(char),49U),_tag_fat(_TmpC,sizeof(void*),2));}));});
*res=0;}}
# 792
;_pop_handler();}else{void*_Tmp9=(void*)Cyc_Core_get_exn_thrown();void*_TmpA;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp9)->tag==Cyc_Dict_Absent){
# 804
if(*res)
({struct Cyc_Dict_Dict _TmpB=({struct Cyc_Dict_Dict(*_TmpC)(struct Cyc_Dict_Dict,struct _tuple0*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,void*))Cyc_Dict_insert;_TmpC;})(*res_dic,x,y);*res_dic=_TmpB;});
goto _LL8;}else{_TmpA=_Tmp9;{void*exn=_TmpA;_rethrow(exn);}}_LL8:;}}}}}
# 810
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned,struct _fat_ptr*),struct _fat_ptr t,struct _fat_ptr*msg){
# 818
struct _tuple18 env=({struct _tuple18 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=dic1,_Tmp0.f2=dic1,_Tmp0.f3=excl,_Tmp0.f4=merge_f,_Tmp0.f5=t,_Tmp0.f6=msg;_Tmp0;});
({void(*_Tmp0)(void(*)(struct _tuple18*,struct _tuple0*,void*),struct _tuple18*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple18*,struct _tuple0*,void*),struct _tuple18*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp0;})(Cyc_Interface_compat_merge_dict_f,& env,dic2);
if(env.f0){struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));*_Tmp0=env.f1;return _Tmp0;}else{return 0;}}
# 828
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _fat_ptr*msg){
struct Cyc_Dict_Dict*r1=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp0;})(ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,_tag_fat("type",sizeof(char),5U),msg);
struct Cyc_Dict_Dict*r2=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Datatypedecl*(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Datatypedecl*(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp0;})(ie1->datatypedecls,ie2->datatypedecls,iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,_tag_fat("datatype",sizeof(char),9U),msg);
struct Cyc_Dict_Dict*r3=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp0;})(ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,_tag_fat("enum",sizeof(char),5U),msg);
struct Cyc_Dict_Dict*r4=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp0;})(ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,_tag_fat("typedef",sizeof(char),8U),msg);
struct Cyc_Dict_Dict*r5=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp0;})(ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,_tag_fat("variable",sizeof(char),9U),msg);
struct Cyc_Dict_Dict*r6=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Tcdecl_Xdatatypefielddecl*(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Tcdecl_Xdatatypefielddecl*(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp0;})(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,_tag_fat("xdatatypefield",sizeof(char),15U),msg);
if(((((!((unsigned)r1)|| !((unsigned)r2))|| !((unsigned)r3))|| !((unsigned)r4))|| !((unsigned)r5))|| !((unsigned)r6))
return 0;{
struct Cyc_Interface_Ienv*_Tmp0=_cycalloc(sizeof(struct Cyc_Interface_Ienv));_Tmp0->aggrdecls=*r1,_Tmp0->datatypedecls=*r2,_Tmp0->enumdecls=*r3,_Tmp0->typedefdecls=*r4,_Tmp0->vardecls=*r5,_Tmp0->xdatatypefielddecls=*r6;return _Tmp0;}}struct _tuple19{int f0;struct Cyc_Dict_Dict f1;struct Cyc_Dict_Dict f2;struct _fat_ptr f3;struct _fat_ptr*f4;};
# 841
void Cyc_Interface_disj_merge_dict_f(struct _tuple19*env,struct _tuple0*x,void*y){
# 844
void*_Tmp0;struct _fat_ptr _Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=(int*)& env->f0;_Tmp3=(struct Cyc_Dict_Dict*)& env->f1;_Tmp2=env->f2;_Tmp1=env->f3;_Tmp0=env->f4;{int*res=(int*)_Tmp4;struct Cyc_Dict_Dict*res_dic=(struct Cyc_Dict_Dict*)_Tmp3;struct Cyc_Dict_Dict dic1=_Tmp2;struct _fat_ptr t=_Tmp1;struct _fat_ptr*msg=_Tmp0;
if(({int(*_Tmp5)(struct Cyc_Dict_Dict,struct _tuple0*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_member;_Tmp5;})(dic1,x)){
({struct _fat_ptr*_Tmp5=msg;Cyc_Interface_check_err(_Tmp5,({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=t;_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=Cyc_Absynpp_qvar2string(x);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s %s is exported more than once",sizeof(char),33U),_tag_fat(_Tmp8,sizeof(void*),2));}));});
*res=0;}else{
if(*res)
({struct Cyc_Dict_Dict _Tmp5=({struct Cyc_Dict_Dict(*_Tmp6)(struct Cyc_Dict_Dict,struct _tuple0*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,void*))Cyc_Dict_insert;_Tmp6;})(*res_dic,x,y);*res_dic=_Tmp5;});}}}
# 853
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _fat_ptr t,struct _fat_ptr*msg){
# 857
struct _tuple19 env=({struct _tuple19 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=dic1,_Tmp0.f2=dic1,_Tmp0.f3=t,_Tmp0.f4=msg;_Tmp0;});
({void(*_Tmp0)(void(*)(struct _tuple19*,struct _tuple0*,void*),struct _tuple19*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple19*,struct _tuple0*,void*),struct _tuple19*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp0;})(Cyc_Interface_disj_merge_dict_f,& env,dic2);
if(env.f0){struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));*_Tmp0=env.f1;return _Tmp0;}else{return 0;}}
# 862
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _fat_ptr*msg){
struct Cyc_Dict_Dict*r1=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=({struct Cyc_Dict_Dict*(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp1;});struct Cyc_Dict_Dict _Tmp1=ie1->aggrdecls;struct Cyc_Dict_Dict _Tmp2=ie2->aggrdecls;struct Cyc_Dict_Dict _Tmp3=
({struct Cyc_Dict_Dict(*_Tmp4)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp4;})(Cyc_Absyn_qvar_cmp);
# 863
struct _fat_ptr _Tmp4=
# 865
_tag_fat("type",sizeof(char),5U);
# 863
_Tmp0(_Tmp1,_Tmp2,_Tmp3,Cyc_Tcdecl_merge_aggrdecl,_Tmp4,msg);});
# 866
struct Cyc_Dict_Dict*r2=Cyc_Interface_disj_merge_dict(ie1->datatypedecls,ie2->datatypedecls,_tag_fat("datatype",sizeof(char),9U),msg);
struct Cyc_Dict_Dict*r3=Cyc_Interface_disj_merge_dict(ie1->enumdecls,ie2->enumdecls,_tag_fat("enum",sizeof(char),5U),msg);
# 869
struct Cyc_Dict_Dict*r4=({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=({struct Cyc_Dict_Dict*(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict;_Tmp1;});struct Cyc_Dict_Dict _Tmp1=ie1->typedefdecls;struct Cyc_Dict_Dict _Tmp2=ie2->typedefdecls;struct Cyc_Dict_Dict _Tmp3=
({struct Cyc_Dict_Dict(*_Tmp4)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp4;})(Cyc_Absyn_qvar_cmp);
# 869
struct _fat_ptr _Tmp4=
# 871
_tag_fat("typedef",sizeof(char),8U);
# 869
_Tmp0(_Tmp1,_Tmp2,_Tmp3,Cyc_Tcdecl_merge_typedefdecl,_Tmp4,msg);});
# 872
struct Cyc_Dict_Dict*r5=Cyc_Interface_disj_merge_dict(ie1->vardecls,ie2->vardecls,_tag_fat("variable",sizeof(char),9U),msg);
struct Cyc_Dict_Dict*r6=Cyc_Interface_disj_merge_dict(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,
_tag_fat("xdatatypefield",sizeof(char),15U),msg);
# 876
if(((((!((unsigned)r1)|| !((unsigned)r2))|| !((unsigned)r3))|| !((unsigned)r4))|| !((unsigned)r5))|| !((unsigned)r6))
return 0;{
struct Cyc_Interface_Ienv*_Tmp0=_cycalloc(sizeof(struct Cyc_Interface_Ienv));_Tmp0->aggrdecls=*r1,_Tmp0->datatypedecls=*r2,_Tmp0->enumdecls=*r3,_Tmp0->typedefdecls=*r4,_Tmp0->vardecls=*r5,_Tmp0->xdatatypefielddecls=*r6;return _Tmp0;}}
# 882
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _fat_ptr*msg=0;
# 886
if(info!=0)
msg=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(*info).f0;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(*info).f1;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("merging exports of %s and %s,",sizeof(char),30U),_tag_fat(_Tmp6,sizeof(void*),2));});*_Tmp2=_Tmp3;});_Tmp2;});{
# 890
struct Cyc_Interface_Ienv*exp=Cyc_Interface_disj_merge_ienv(i1->exports,i2->exports,msg);
if(exp==0){struct Cyc_Interface_I*_Tmp2=0;_npop_handler(0);return _Tmp2;}
# 893
if(info!=0)
msg=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(*info).f0;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(*info).f1;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("merging imports of %s and %s,",sizeof(char),30U),_tag_fat(_Tmp6,sizeof(void*),2));});*_Tmp2=_Tmp3;});_Tmp2;});{
# 897
struct Cyc_Interface_Ienv*imp=Cyc_Interface_compat_merge_ienv(i1->imports,i2->imports,exp,msg);
if(imp==0){struct Cyc_Interface_I*_Tmp2=0;_npop_handler(0);return _Tmp2;}{
# 900
struct Cyc_List_List*newtds=0;struct Cyc_List_List*l=i2->tds;
for(1;l!=0;l=l->tl){
newtds=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));*_Tmp4=*((struct Cyc_Absyn_Decl*)l->hd);_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=newtds;_Tmp2;});}
newtds=Cyc_List_rev(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left;_Tmp2;})(Cyc_Interface_uniqify_decl_list,newtds,i1->tds));{
struct Cyc_Interface_I*_Tmp2;_Tmp2=_cycalloc(sizeof(struct Cyc_Interface_I)),_Tmp2->imports=imp,_Tmp2->exports=exp,_Tmp2->tds=newtds;_npop_handler(0);return _Tmp2;}}}}}
# 884
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_Tmp2)->tag==Cyc_Tcdecl_Incompatible)
# 905
return 0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 908
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){
if(li==0)return Cyc_Interface_empty();{
# 911
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _fat_ptr*curr_info=linfo!=0?(struct _fat_ptr*)linfo->hd: 0;
li=li->tl;
if(linfo!=0)linfo=linfo->tl;
# 916
for(1;li!=0;li=li->tl){
struct Cyc_Interface_I*i=({struct Cyc_Interface_I*_Tmp0=curr_i;struct Cyc_Interface_I*_Tmp1=(struct Cyc_Interface_I*)li->hd;Cyc_Interface_merge(_Tmp0,_Tmp1,
curr_info!=0 && linfo!=0?({struct _tuple11*_Tmp2=_cycalloc(sizeof(struct _tuple11));_Tmp2->f0=*curr_info,_Tmp2->f1=*((struct _fat_ptr*)linfo->hd);_Tmp2;}): 0);});
if(i==0)return 0;
curr_i=i;
if(curr_info!=0)
curr_info=linfo!=0?({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=*curr_info;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*((struct _fat_ptr*)linfo->hd);_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(_tag_fat("%s+%s",sizeof(char),6U),_tag_fat(_Tmp4,sizeof(void*),2));});*_Tmp0=_Tmp1;});_Tmp0;}): 0;
if(linfo!=0)linfo=linfo->tl;}
# 925
return curr_i;}}
# 928
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){
if(la==0)return Cyc_Interface_empty();{
# 931
struct Cyc_Interface_I*curr_i=get(la->hd);
struct _fat_ptr*curr_info=linfo!=0?(struct _fat_ptr*)linfo->hd: 0;
la=la->tl;
if(linfo!=0)linfo=linfo->tl;
# 936
for(1;la!=0;la=la->tl){
struct Cyc_Interface_I*i=({struct Cyc_Interface_I*_Tmp0=curr_i;struct Cyc_Interface_I*_Tmp1=get(la->hd);Cyc_Interface_merge(_Tmp0,_Tmp1,
curr_info!=0 && linfo!=0?({struct _tuple11*_Tmp2=_cycalloc(sizeof(struct _tuple11));_Tmp2->f0=*curr_info,_Tmp2->f1=*((struct _fat_ptr*)linfo->hd);_Tmp2;}): 0);});
if(i==0)return 0;
curr_i=i;
if(curr_info!=0)
curr_info=linfo!=0?({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=*curr_info;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*((struct _fat_ptr*)linfo->hd);_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(_tag_fat("%s+%s",sizeof(char),6U),_tag_fat(_Tmp4,sizeof(void*),2));});*_Tmp0=_Tmp1;});_Tmp0;}): 0;
# 944
if(linfo!=0)linfo=linfo->tl;}
# 946
return curr_i;}}
# 952
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds){
struct Cyc_List_List*ans=0;
for(1;tds!=0;tds=tds->tl){
struct _tuple0*qv;
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)tds->hd;
{void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
qv=vd->name;goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
qv=fd->name;goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
qv=ad->name;goto _LL0;}case 6: _Tmp1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp1;
qv=dd->name;goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
qv=ed->name;goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
qv=td->name;goto _LL0;}default:
 Cyc_Interface_err(_tag_fat("bad decl form in Interface::add_namespace",sizeof(char),42U));return 0;}_LL0:;}{
# 966
struct Cyc_List_List*vs;
{union Cyc_Absyn_Nmspace _Tmp0=(*qv).f0;void*_Tmp1;switch(_Tmp0.C_n.tag){case 2: _Tmp1=_Tmp0.Abs_n.val;{struct Cyc_List_List*x=_Tmp1;
vs=x;goto _LLF;}case 3: _Tmp1=_Tmp0.C_n.val;{struct Cyc_List_List*x=_Tmp1;
vs=x;goto _LLF;}default:
 Cyc_Interface_err(_tag_fat("bad namespace in Interface::add_namespace",sizeof(char),42U));return 0;}_LLF:;}
# 972
vs=Cyc_List_imp_rev(vs);
({union Cyc_Absyn_Nmspace _Tmp0=Cyc_Absyn_Rel_n(0);(*qv).f0=_Tmp0;});
for(1;vs!=0;vs=vs->tl){
d=({void*_Tmp0=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp1->tag=9,_Tmp1->f1=(struct _fat_ptr*)vs->hd,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=d,_Tmp3->tl=0;_Tmp3;});_Tmp1->f2=_Tmp2;});_Tmp1;});Cyc_Absyn_new_decl(_Tmp0,d->loc);});}
ans=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=d,_Tmp0->tl=ans;_Tmp0;});}}
# 978
return Cyc_List_imp_rev(ans);}
# 981
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp2->tag=5,_Tmp2->f1=d;_Tmp2;}),0U);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tds;return _Tmp0;}
# 985
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
# 987
d=({struct Cyc_Absyn_Aggrdecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));*_Tmp0=*d;_Tmp0;});
d->impl=0;
if((int)d->sc!=4)d->sc=3U;{
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp2->tag=5,_Tmp2->f1=d;_Tmp2;}),0U);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tds;return _Tmp0;}}
# 993
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_Tmp2->tag=6,_Tmp2->f1=d;_Tmp2;}),0U);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tds;return _Tmp0;}static char _TmpG0[2U]="_";
# 997
static struct _fat_ptr Cyc_Interface_us={_TmpG0,_TmpG0,_TmpG0 + 2U};
static struct _fat_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
# 1000
static struct _tuple12*Cyc_Interface_rewrite_datatypefield_type(struct _tuple12*x){
struct _tuple12*_Tmp0=_cycalloc(sizeof(struct _tuple12));({struct Cyc_Absyn_Tqual _Tmp1=Cyc_Absyn_empty_tqual(0U);_Tmp0->f0=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp2->tag=2,({struct Cyc_Absyn_Tvar*_Tmp3=({struct Cyc_Absyn_Tvar*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp4->name=Cyc_Interface_us_p,_Tmp4->identity=-1,({void*_Tmp5=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_Tmp4->kind=_Tmp5;}),_Tmp4->aquals_bound=0;_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;});return _Tmp0;}
# 1005
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){
f=({struct Cyc_Absyn_Datatypefield*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));*_Tmp0=*f;_Tmp0;});
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*(*_Tmp1)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map;_Tmp1;})(Cyc_Interface_rewrite_datatypefield_type,f->typs);f->typs=_Tmp0;});
return f;}
# 1011
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
# 1013
d=({struct Cyc_Absyn_Datatypedecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));*_Tmp0=*d;_Tmp0;});
# 1015
if(d->fields!=0)({struct Cyc_Core_Opt*_Tmp0=({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*(*_Tmp3)(struct Cyc_Absyn_Datatypefield*(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)d->fields->v);_Tmp1->v=_Tmp2;});_Tmp1;});d->fields=_Tmp0;});
if((int)d->sc!=4)d->sc=3U;{
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_Tmp2->tag=6,_Tmp2->f1=d;_Tmp2;}),0U);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tds;return _Tmp0;}}
# 1020
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_Tmp2->tag=7,_Tmp2->f1=d;_Tmp2;}),0U);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tds;return _Tmp0;}
# 1024
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
# 1026
d=({struct Cyc_Absyn_Enumdecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));*_Tmp0=*d;_Tmp0;});
d->fields=0;
if((int)d->sc!=4)d->sc=3U;{
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_Tmp2->tag=7,_Tmp2->f1=d;_Tmp2;}),0U);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tds;return _Tmp0;}}
# 1032
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
# 1034
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_Tmp2->tag=8,_Tmp2->f1=d;_Tmp2;}),0U);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tds;return _Tmp0;}
# 1037
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _tuple0*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp2->tag=0,_Tmp2->f1=d;_Tmp2;}),0U);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tds;return _Tmp0;}
# 1041
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1043
void*_Tmp0;void*_Tmp1;_Tmp1=d->base;_Tmp0=d->field;{struct Cyc_Absyn_Datatypedecl*b=_Tmp1;struct Cyc_Absyn_Datatypefield*f=_Tmp0;
b=({struct Cyc_Absyn_Datatypedecl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));*_Tmp2=*b;_Tmp2;});
({struct Cyc_Core_Opt*_Tmp2=({struct Cyc_Core_Opt*_Tmp3=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=f,_Tmp5->tl=0;_Tmp5;});_Tmp3->v=_Tmp4;});_Tmp3;});b->fields=_Tmp2;});
b->sc=3U;{
struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_Tmp4->tag=6,_Tmp4->f1=b;_Tmp4;}),0U);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=tds;return _Tmp2;}}}
# 1050
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1052
void*_Tmp0;void*_Tmp1;_Tmp1=d->base;_Tmp0=d->field;{struct Cyc_Absyn_Datatypedecl*b=_Tmp1;struct Cyc_Absyn_Datatypefield*f=_Tmp0;
b=({struct Cyc_Absyn_Datatypedecl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));*_Tmp2=*b;_Tmp2;});
f=Cyc_Interface_rewrite_datatypefield(f);
f->sc=3U;
({struct Cyc_Core_Opt*_Tmp2=({struct Cyc_Core_Opt*_Tmp3=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=f,_Tmp5->tl=0;_Tmp5;});_Tmp3->v=_Tmp4;});_Tmp3;});b->fields=_Tmp2;});
b->sc=3U;{
struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_Tmp4->tag=6,_Tmp4->f1=b;_Tmp4;}),0U);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=tds;return _Tmp2;}}}
# 1061
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,0);
tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,tds);
tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_enumdecl_header,ie->enumdecls,tds);
if(tds!=0)
({struct Cyc_List_List*_Tmp0=Cyc_Interface_add_namespace(Cyc_List_imp_rev(tds));Cyc_Absynpp_decllist2file(_Tmp0,f);});}
# 1069
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);
if(tds!=0)
({struct Cyc_List_List*_Tmp0=Cyc_Interface_add_namespace(Cyc_List_imp_rev(tds));Cyc_Absynpp_decllist2file(_Tmp0,f);});}
# 1075
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_typedef,ie->typedefdecls,0);
if(tds!=0)
({struct Cyc_List_List*_Tmp0=Cyc_Interface_add_namespace(Cyc_List_imp_rev(tds));Cyc_Absynpp_decllist2file(_Tmp0,f);});}
# 1081
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_aggrdecl,ie->aggrdecls,0);
tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_datatypedecl,ie->datatypedecls,tds);
tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_enumdecl,ie->enumdecls,tds);
tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_vardecl,ie->vardecls,tds);
tds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,tds);
if(tds!=0)
({struct Cyc_List_List*_Tmp0=Cyc_Interface_add_namespace(Cyc_List_imp_rev(tds));Cyc_Absynpp_decllist2file(_Tmp0,f);});}
# 1094
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_fprintf(f,_tag_fat("/****** needed (headers) ******/\n",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
Cyc_Interface_print_ns_headers(f,i->imports);
# 1099
Cyc_fprintf(f,_tag_fat("\n/****** provided (headers) ******/\n",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));
Cyc_Interface_print_ns_headers(f,i->exports);
# 1104
Cyc_fprintf(f,_tag_fat("\n/****** needed (headers of xdatatypefielddecls) ******/\n",sizeof(char),58U),_tag_fat(0U,sizeof(void*),0));
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->imports);
# 1107
Cyc_fprintf(f,_tag_fat("\n/****** provided (headers of xdatatypefielddecls) ******/\n",sizeof(char),60U),_tag_fat(0U,sizeof(void*),0));
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->exports);
# 1111
Cyc_fprintf(f,_tag_fat("\n/****** provided (typedefs) ******/\n",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));
Cyc_Interface_print_ns_typedefs(f,i->exports);
# 1114
Cyc_fprintf(f,_tag_fat("\n/****** needed (declarations) ******/\n",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
Cyc_Interface_print_ns_decls(f,i->imports);
# 1117
Cyc_fprintf(f,_tag_fat("\n/****** provided (declarations) ******/\n",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));
Cyc_Interface_print_ns_decls(f,i->exports);}
# 1124
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{
struct Cyc_List_List*tds=Cyc_Parse_parse_file(f);
Cyc_Binding_resolve_all(tds);{
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_tc_init();
Cyc_Tc_tc(te,0,tds);
return Cyc_Interface_gen_extract(te->ae,0,tds);}}}
# 1137
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1143
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){
return Cyc_Interface_parse(f);}
